// ==WindhawkMod==
// @id              cursor-tail
// @name            Cursor Tail
// @name:zh-CN      光标拖尾
// @description     Adds a smooth, speed-reactive motion-blur trail to the mouse cursor.
// @description:zh-CN 为鼠标指针添加平滑、随速度变化的运动模糊拖尾。
// @version         3.11
// @author          CYoJkoY
// @github          https://github.com/CYoJkoY
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -ld2d1 -lole32 -lgdi32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Cursor Tail

Cursor Tail adds a smooth, tapered motion-blur trail behind the mouse pointer
when it moves quickly. The trail is rendered in a transparent overlay window
with Direct2D, so it works independently of the application under the pointer.

## Preview

![Cursor Tail Demo](https://i.imgur.com/mbiW6QU.gif)

## Features

- **Speed-reactive trail:** Width, opacity, and effective length respond to pointer velocity.
- **Smooth motion:** Historical cursor samples can be smoothed with Chaikin subdivision for a continuous ribbon.
- **Custom appearance:** Configure trail color, outline color, gradient, and optional glow.
- **Adaptive color:** Automatically sample the active cursor image and prefer colors with sufficient contrast against the screen background.
- **Fade-out:** Smoothly fade the trail after the pointer slows down.
- **Per-app rules:** Enable or disable the trail for selected executables.
- **Fullscreen suppression:** Automatically hide the trail for exclusive or borderless fullscreen applications and restore it after fullscreen ends.

## Color modes

**Manual** mode uses the configured hexadecimal trail color. **Auto** mode samples the current cursor image and chooses a visible color based on the luminance around the pointer. Automatic resampling can be disabled or limited to a custom interval.

The outline can either be derived automatically from the trail luminance or set to a fixed hexadecimal color.

## Per-app rules

Use one rule per line:

`program.exe=on`

`program.exe=off`

Lines beginning with `#` are comments. Executable names are matched case-insensitively.

## Fullscreen behavior

The trail is suppressed when the foreground application reports a D3D fullscreen state or when a borderless, captionless window covers its monitor. Leaving fullscreen must remain stable for 500 ms before the trail is shown again, which reduces flicker during application transitions.

## Performance

Cursor state is sampled at 125 Hz while the trail is active and backs off to 30 Hz when the trail is fully idle. Layered-window submission is paced separately at 60 FPS while actively moving and 30 FPS while fading. Background luminance sampling is additionally limited by both time and cursor travel distance so repeated screen captures are avoided during short, fast sampling intervals. The back buffer and Direct2D resources are reused between frames and resized only when the required trail bounds grow beyond the current buffer.

## Attribution

This mod is derived in part from the Windhawk **Cursor Motion Blur** mod by TheatriChris. The original project is licensed under the MIT License.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Behavior:
    - trigger_velocity: 25
      $name: Trigger speed
      $name:zh-CN: 触发速度
      $description: Minimum pointer speed in pixels per sample required to start the trail.
      $description:zh-CN: 开始显示拖尾所需的最低指针速度，单位为每个采样的像素数。
    - stop_velocity: 10
      $name: Stop speed
      $name:zh-CN: 停止速度
      $description: Pointer speed below which an active trail begins fading. If this is set to the trigger speed or higher, it is automatically reduced to half the trigger speed.
      $description:zh-CN: 指针速度低于此值时，正在显示的拖尾开始淡出。如果该值大于或等于触发速度，会自动降低为触发速度的一半。
    - tail_length: 10
      $name: Tail length
      $name:zh-CN: 拖尾长度
      $description: Number of cursor samples kept for the trail. Higher values make the trail longer.
      $description:zh-CN: 保留的指针采样数量。数值越大，拖尾越长。
    - tail_offset_x: 6
      $name: Trail X offset
      $name:zh-CN: 拖尾 X 偏移
      $description: Horizontal offset from the cursor hotspot to the trail head, in pixels.
      $description:zh-CN: 拖尾起点相对于指针热点的水平偏移量，单位为像素。
    - tail_offset_y: 10
      $name: Trail Y offset
      $name:zh-CN: 拖尾 Y 偏移
      $description: Vertical offset from the cursor hotspot to the trail head, in pixels.
      $description:zh-CN: 拖尾起点相对于指针热点的垂直偏移量，单位为像素。
    - speed_scaling: true
      $name: Speed-reactive shape
      $name:zh-CN: 随速度变化的形状
      $description: Increase or decrease trail width, opacity, and effective length according to pointer speed.
      $description:zh-CN: 根据指针速度动态调整拖尾宽度、不透明度和实际长度。
    - fade_enabled: true
      $name: Fade-out
      $name:zh-CN: 淡出
      $description: Smoothly fade the trail after the pointer slows down.
      $description:zh-CN: 指针减速后平滑淡出拖尾。
    - fade_decay: 90
      $name: Fade speed
      $name:zh-CN: 淡出速度
      $description: Controls how quickly the trail fades. 90 means the remaining opacity is multiplied by 0.90 for each simulation sample.
      $description:zh-CN: 控制拖尾的淡出速度。90 表示每个模拟采样都会将剩余不透明度乘以 0.90。

- Appearance:
    - width_min: 4
      $name: Minimum trail width
      $name:zh-CN: 最小拖尾宽度
      $description: Half-width of the outer trail at lower speeds, in pixels.
      $description:zh-CN: 低速时外层拖尾的半宽，单位为像素。
    - width_max: 14
      $name: Maximum trail width
      $name:zh-CN: 最大拖尾宽度
      $description: Half-width of the outer trail at higher speeds, in pixels.
      $description:zh-CN: 高速时外层拖尾的半宽，单位为像素。
    - core_width_min: 2
      $name: Minimum core width
      $name:zh-CN: 最小核心宽度
      $description: Half-width of the inner core at lower speeds, in pixels.
      $description:zh-CN: 低速时内部核心的半宽，单位为像素。
    - core_width_max: 9
      $name: Maximum core width
      $name:zh-CN: 最大核心宽度
      $description: Half-width of the inner core at higher speeds, in pixels.
      $description:zh-CN: 高速时内部核心的半宽，单位为像素。
    - alpha_min: 45
      $name: Minimum opacity
      $name:zh-CN: 最小不透明度
      $description: Trail opacity at lower speeds, from 0 to 100 percent.
      $description:zh-CN: 低速时的拖尾不透明度，范围为 0 到 100%。
    - alpha_max: 90
      $name: Maximum opacity
      $name:zh-CN: 最大不透明度
      $description: Trail opacity at higher speeds, from 0 to 100 percent.
      $description:zh-CN: 高速时的拖尾不透明度，范围为 0 到 100%。
    - taper_power: 10
      $name: Tail taper
      $name:zh-CN: 拖尾渐缩
      $description: Controls how quickly the trail narrows toward its tail. 10 is linear; larger values produce a sharper taper. Range 5-30.
      $description:zh-CN: 控制拖尾向末端收窄的速度。10 为线性效果；数值越大，收窄越明显。范围 5-30。
    - smooth_iterations: 2
      $name: Smoothing iterations
      $name:zh-CN: 平滑迭代次数
      $description: Number of Chaikin subdivision passes. Higher values produce a smoother trail at the cost of more points to render. Range 0-4.
      $description:zh-CN: Chaikin 细分次数。数值越高，拖尾越平滑，但需要绘制更多点。范围 0-4。

- Color:
    - trail_color_mode: manual
      $name: Trail color mode
      $name:zh-CN: 拖尾颜色模式
      $description: Choose a fixed trail color or automatically sample the cursor image.
      $description:zh-CN: 选择固定的拖尾颜色，或自动从当前指针图像采样颜色。
      $options:
        - manual: Manual color
        - auto: Auto-sample cursor colors
      $options:zh-CN:
        - manual: 手动颜色
        - auto: 自动采样指针颜色
    - trail_color_manual: "#FFFFFF"
      $name: Manual trail color
      $name:zh-CN: 手动拖尾颜色
      $description: Hexadecimal RGB color used when Trail color mode is Manual.
      $description:zh-CN: 拖尾颜色模式设为“手动颜色”时使用的十六进制 RGB 颜色。
    - outline_color_mode: auto
      $name: Outline color mode
      $name:zh-CN: 轮廓颜色模式
      $description: Choose an automatic outline color or a fixed manual color.
      $description:zh-CN: 选择自动轮廓颜色或固定的手动颜色。
      $options:
        - auto: Automatic outline
        - manual: Manual color
      $options:zh-CN:
        - auto: 自动轮廓
        - manual: 手动颜色
    - outline_color_manual: "#000000"
      $name: Manual outline color
      $name:zh-CN: 手动轮廓颜色
      $description: Hexadecimal RGB color used when Outline color mode is Manual.
      $description:zh-CN: 轮廓颜色模式设为“手动颜色”时使用的十六进制 RGB 颜色。
    - auto_resample_interval: 0
      $name: Auto-color refresh interval
      $name:zh-CN: 自动颜色刷新间隔
      $description: How often Auto color mode resamples the cursor color, in milliseconds. Set to 0 to resample only when the cursor image changes.
      $description:zh-CN: 自动颜色模式重新采样指针颜色的间隔，单位为毫秒。设为 0 时，仅在指针图像发生变化时重新采样。

- Effects:
    - gradient_enabled: false
      $name: Tail gradient
      $name:zh-CN: 拖尾渐变
      $description: Fade the core color toward the configured tail color.
      $description:zh-CN: 将核心颜色向设定的末端颜色渐变。
    - gradient_tail_color: "#FF00FF"
      $name: Gradient tail color
      $name:zh-CN: 渐变末端颜色
      $description: Hexadecimal RGB color used at the tail end when Tail gradient is enabled.
      $description:zh-CN: 启用拖尾渐变时，拖尾末端使用的十六进制 RGB 颜色。
    - glow_enabled: false
      $name: Glow
      $name:zh-CN: 发光
      $description: Draw an additional soft glow around the trail.
      $description:zh-CN: 在拖尾周围绘制额外的柔和光晕。
    - glow_color: "#FFFFFF"
      $name: Glow color
      $name:zh-CN: 光晕颜色
      $description: Hexadecimal RGB color used for the glow.
      $description:zh-CN: 光晕使用的十六进制 RGB 颜色。
    - glow_width_factor: 18
      $name: Glow width
      $name:zh-CN: 光晕宽度
      $description: Glow width relative to the outer trail width. 18 means 1.8 times the outer width.
      $description:zh-CN: 光晕相对于外层拖尾宽度的比例。18 表示外层宽度的 1.8 倍。
    - glow_alpha: 25
      $name: Glow opacity
      $name:zh-CN: 光晕不透明度
      $description: Glow opacity, from 0 to 100 percent.
      $description:zh-CN: 光晕不透明度，范围为 0 到 100%。

- Application:
    - app_rules: ""
      $name: Per-app rules
      $name:zh-CN: 按应用规则
      $description: One rule per line using `exe=on` or `exe=off`. Lines beginning with `#` are comments. Executable names are matched case-insensitively.
      $description:zh-CN: 每行一个规则，格式为 `exe=on` 或 `exe=off`。以 `#` 开头的行为注释。可执行文件名不区分大小写。
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <d2d1.h>
#include <math.h>
#include <shellapi.h>
#include <cwctype>

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
#define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002u
#endif

constexpr UINT kSettingsChangedMessage = WM_APP + 1;
constexpr int kTargetFrameRate = 125;
constexpr int kIdleFrameRate = 30;
constexpr int kActiveRenderFrameRate = 60;
constexpr int kFadeRenderFrameRate = 30;
constexpr DWORD kFullscreenStrongCheckIntervalMs = 100;
constexpr DWORD kFullscreenExitConfirmMs = 500;
constexpr LONG kFullscreenTolerancePx = 2;
constexpr int kMaxTailLength = 64;
constexpr int kHistoryCapacity = kMaxTailLength;
constexpr int kRenderPadding = 2;
constexpr float kTrailAlpha = 0.86f;
constexpr float kFadeCutoff = 0.05f;
constexpr DWORD kMinCursorChangeResampleIntervalMs = 100;
constexpr int kAutoResampleIntervalMaxMs = 10000;
constexpr int kBackgroundSampleRadius = 24;
constexpr int kBackgroundSampleGrid = 5;
constexpr DWORD kBackgroundResampleIntervalMs = 50;
constexpr LONG kBackgroundResampleDistancePx = 20;
constexpr float kMinColorContrast = 3.0f;
constexpr uint8_t kCursorAlphaThreshold = 96;
constexpr uint32_t kFallbackCoreColor = 0x00FFFFFF;
constexpr uint32_t kFallbackOuterColor = 0x00000000;
constexpr DWORD kBackbufferIdleReleaseDelayMs = 5000;

std::atomic<HWND> g_overlayHwnd{nullptr};
HANDLE g_threadHandle = nullptr;
HANDLE g_stopEvent = nullptr;
std::atomic_bool g_settingsDirty{false};

POINT g_history[kHistoryCapacity] = {};
int g_historyHead = 0;
int g_historyCount = 0;
POINT g_lastPos = {0, 0};
bool g_isSmearing = false;
int g_lowVelocityFrames = 0;
float g_fadeAlpha = 0.0f;
float g_smoothedSpeedNorm = 0.0f;
float g_frozenSpeedNorm = 0.5f;
bool g_windowVisible = false;

ID2D1Factory* g_d2dFactory = nullptr;
ID2D1DCRenderTarget* g_renderTarget = nullptr;
ID2D1SolidColorBrush* g_outerBrush = nullptr;
ID2D1SolidColorBrush* g_coreBrush = nullptr;
ID2D1SolidColorBrush* g_glowBrush = nullptr;
ID2D1LinearGradientBrush* g_gradientBrush = nullptr;
ID2D1GradientStopCollection* g_gradientStops = nullptr;
ID2D1StrokeStyle* g_strokeStyle = nullptr;
uint32_t g_gradientHeadCache = 0xFFFFFFFFu;
uint32_t g_gradientTailCache = 0xFFFFFFFFu;
bool g_dcBound = false;
HDC g_backBufferDc = nullptr;
HBITMAP g_backBufferBitmap = nullptr;
HGDIOBJ g_originalBitmap = nullptr;
int g_cachedWidth = 0;
int g_cachedHeight = 0;
DWORD g_backbufferIdleSince = 0;
HDC g_screenDc = nullptr;

float g_triggerVelocity = 25.0f;
float g_stopVelocity = 10.0f;
int g_tailOffsetX = 6;
int g_tailOffsetY = 10;
int g_tailLength = 10;
int g_speedScaling = 1;
float g_widthMin = 4.0f;
float g_widthMax = 14.0f;
float g_coreWidthMin = 2.0f;
float g_coreWidthMax = 9.0f;
float g_alphaMin = 0.45f;
float g_alphaMax = 0.90f;
float g_taperPower = 1.0f;
int g_smoothIterations = 2;
int g_gradientEnabled = 0;
uint32_t g_gradientTailRGB = 0x00FF00FF;
int g_glowEnabled = 0;
uint32_t g_glowRGB = 0x00FFFFFF;
float g_glowWidthFactor = 1.8f;
float g_glowAlpha = 0.25f;
int g_fadeEnabled = 1;
float g_fadeDecay = 0.90f;
int g_trailColorMode = 0;
uint32_t g_manualColorRGB = 0x00FFFFFF;
int g_outlineColorMode = 0;
uint32_t g_manualOutlineRGB = 0x00000000;
int g_autoResampleInterval = 0;
uint32_t g_currentCoreRGB = kFallbackCoreColor;
uint32_t g_currentOuterRGB = kFallbackOuterColor;
DWORD g_lastAutoColorUpdate = 0;
HCURSOR g_lastAutoColorCursor = nullptr;
bool g_autoColorInitialized = false;

struct AppRule { std::wstring exe; bool enabled; };
std::vector<AppRule> g_appRules;
HWND g_cachedForegroundWindow = nullptr;
int g_cachedAppRule = 0;

bool g_fullscreenSuppressed = false;
DWORD g_fullscreenFalseSince = 0;
HWND g_fullscreenForegroundWindow = nullptr;
DWORD g_lastFullscreenStrongCheck = 0;
bool g_fullscreenStrongSignal = false;

struct TrailRenderCache {
    std::vector<D2D1_POINT_2F> smoothed;
    std::vector<D2D1_POINT_2F> subdivision;
    std::vector<float> taper;

    void ReserveForTailLength(int tailLength) {
        const size_t baseCount = static_cast<size_t>(std::max(tailLength, 2));
        const size_t maxPointCount = baseCount * 16 - 15;
        smoothed.reserve(maxPointCount);
        subdivision.reserve(maxPointCount);
        taper.reserve(maxPointCount);
    }

    void PrepareTaper() {
        taper.resize(smoothed.size());
        if (smoothed.size() <= 1) {
            if (!taper.empty()) taper[0] = 0.0f;
            return;
        }

        const float denominator = static_cast<float>(smoothed.size() - 1);
        for (size_t i = 0; i + 1 < smoothed.size(); ++i) {
            const float ratio = static_cast<float>(i) / denominator;
            taper[i] = powf(std::max(0.0f, 1.0f - ratio), g_taperPower);
        }
        taper.back() = 0.0f;
    }
};
TrailRenderCache g_renderCache;

static inline float RgbLuminance(uint8_t r, uint8_t g, uint8_t b) {
    const auto linear = [](float channel) {
        channel /= 255.0f;
        return channel <= 0.03928f ? channel / 12.92f : powf((channel + 0.055f) / 1.055f, 2.4f);
    };
    return 0.2126f * linear(r) + 0.7152f * linear(g) + 0.0722f * linear(b);
}

static inline float RgbLuminance(uint32_t rgb) {
    return RgbLuminance(
        static_cast<uint8_t>((rgb >> 16) & 0xFF),
        static_cast<uint8_t>((rgb >> 8) & 0xFF),
        static_cast<uint8_t>(rgb & 0xFF));
}

static inline float ContrastRatio(float a, float b) {
    const float lo = std::min(a, b);
    const float hi = std::max(a, b);
    return (hi + 0.05f) / (lo + 0.05f);
}

static inline D2D1_COLOR_F ToColorF(uint32_t rgb, float alpha) {
    return D2D1::ColorF(
        ((rgb >> 16) & 0xFF) / 255.0f,
        ((rgb >> 8) & 0xFF) / 255.0f,
        (rgb & 0xFF) / 255.0f,
        alpha);
}

static bool ParseHexColor(const wchar_t* str, uint32_t& out) {
    if (!str) return false;
    while (*str == L' ' || *str == L'\t') ++str;
    if (*str == L'#') ++str;
    else if (str[0] == L'0' && (str[1] == L'x' || str[1] == L'X')) str += 2;
    uint32_t value = 0;
    int digits = 0;
    while (digits < 6) {
        const wchar_t c = str[digits];
        int digit = -1;
        if (c >= L'0' && c <= L'9') digit = c - L'0';
        else if (c >= L'a' && c <= L'f') digit = c - L'a' + 10;
        else if (c >= L'A' && c <= L'F') digit = c - L'A' + 10;
        if (digit < 0) break;
        value = (value << 4) | static_cast<uint32_t>(digit);
        ++digits;
    }
    if (digits != 6) return false;
    out = value;
    return true;
}

static std::wstring ToLowerW(std::wstring value) {
    for (wchar_t& c : value) c = static_cast<wchar_t>(towlower(c));
    return value;
}

static uint32_t ResolveOutlineColor(uint32_t core);

void ReleaseRenderResources() {
    if (g_gradientBrush) { g_gradientBrush->Release(); g_gradientBrush = nullptr; }
    if (g_gradientStops) { g_gradientStops->Release(); g_gradientStops = nullptr; }
    if (g_strokeStyle) { g_strokeStyle->Release(); g_strokeStyle = nullptr; }
    if (g_glowBrush) { g_glowBrush->Release(); g_glowBrush = nullptr; }
    if (g_coreBrush) { g_coreBrush->Release(); g_coreBrush = nullptr; }
    if (g_outerBrush) { g_outerBrush->Release(); g_outerBrush = nullptr; }
    if (g_renderTarget) { g_renderTarget->Release(); g_renderTarget = nullptr; }
    g_gradientHeadCache = 0xFFFFFFFFu;
    g_gradientTailCache = 0xFFFFFFFFu;
    g_dcBound = false;
}

void ReleaseBackbuffer() {
    ReleaseRenderResources();
    if (g_backBufferDc) {
        if (g_originalBitmap) { SelectObject(g_backBufferDc, g_originalBitmap); g_originalBitmap = nullptr; }
        DeleteDC(g_backBufferDc);
        g_backBufferDc = nullptr;
    }
    if (g_backBufferBitmap) { DeleteObject(g_backBufferBitmap); g_backBufferBitmap = nullptr; }
    g_cachedWidth = 0;
    g_cachedHeight = 0;
    g_backbufferIdleSince = 0;
}

HBITMAP Create32BitDIB(int width, int height) {
    if (width <= 0 || height <= 0) return nullptr;
    BITMAPINFO info = {};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = width;
    info.bmiHeader.biHeight = -height;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    return CreateDIBSection(nullptr, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
}

HDC GetScreenDC() {
    if (!g_screenDc) g_screenDc = GetDC(nullptr);
    return g_screenDc;
}

void ReleaseScreenDC() {
    if (!g_screenDc) return;
    ReleaseDC(nullptr, g_screenDc);
    g_screenDc = nullptr;
}

void HideOverlay() {
    const HWND hwnd = g_overlayHwnd.load();
    if (!hwnd || !IsWindow(hwnd) || !g_windowVisible) return;
    ShowWindow(hwnd, SW_HIDE);
    g_windowVisible = false;
}

void ShowOverlay() {
    const HWND hwnd = g_overlayHwnd.load();
    if (!hwnd || !IsWindow(hwnd) || g_windowVisible) return;
    ShowWindow(hwnd, SW_SHOWNOACTIVATE);
    g_windowVisible = true;
}

void HistoryClear() { g_historyHead = 0; g_historyCount = 0; }

void HistoryPushFront(const POINT& point) {
    if (g_historyCount == kHistoryCapacity) --g_historyCount;
    g_historyHead = (g_historyHead - 1 + kHistoryCapacity) % kHistoryCapacity;
    g_history[g_historyHead] = point;
    ++g_historyCount;
}

void HistoryPopBack() { if (g_historyCount > 0) --g_historyCount; }

const POINT& HistoryAt(int index) { return g_history[(g_historyHead + index) % kHistoryCapacity]; }

static bool GetProcessExeName(DWORD pid, std::wstring& out) {
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) return false;
    WCHAR path[512] = {};
    DWORD size = ARRAYSIZE(path);
    const BOOL success = QueryFullProcessImageNameW(process, 0, path, &size);
    CloseHandle(process);
    if (!success) return false;
    const wchar_t* name = wcsrchr(path, L'\\');
    out = ToLowerW(name ? name + 1 : path);
    return true;
}

static void ParseAppRules(const wchar_t* text) {
    g_appRules.clear();
    if (!text) return;
    const std::wstring input(text);
    size_t start = 0;
    while (start <= input.size()) {
        size_t end = input.find_first_of(L"\r\n", start);
        if (end == std::wstring::npos) end = input.size();
        std::wstring line = input.substr(start, end - start);
        const size_t comment = line.find(L'#');
        if (comment != std::wstring::npos) line.resize(comment);
        const size_t equals = line.find(L'=');
        if (equals != std::wstring::npos) {
            std::wstring exe = line.substr(0, equals);
            std::wstring value = line.substr(equals + 1);
            const auto trim = [](std::wstring& value) {
                const size_t first = value.find_first_not_of(L" \t");
                if (first == std::wstring::npos) { value.clear(); return; }
                const size_t last = value.find_last_not_of(L" \t");
                value = value.substr(first, last - first + 1);
            };
            trim(exe);
            trim(value);
            if (!exe.empty()) g_appRules.push_back({ToLowerW(exe), value == L"on" || value == L"1" || value == L"true" || value == L"yes"});
        }
        if (end == input.size()) break;
        start = input.find_first_not_of(L"\r\n", end);
        if (start == std::wstring::npos) break;
    }
}

static int CheckAppRuleCached(HWND foreground) {
    if (foreground == g_cachedForegroundWindow) return g_cachedAppRule;
    g_cachedForegroundWindow = foreground;
    g_cachedAppRule = 0;
    if (!foreground || g_appRules.empty()) return 0;
    DWORD pid = 0;
    GetWindowThreadProcessId(foreground, &pid);
    if (!pid) return 0;
    std::wstring exe;
    if (!GetProcessExeName(pid, exe)) return 0;
    for (const auto& rule : g_appRules) {
        if (_wcsicmp(rule.exe.c_str(), exe.c_str()) == 0) {
            g_cachedAppRule = rule.enabled ? 1 : -1;
            break;
        }
    }
    return g_cachedAppRule;
}

void LoadSettings() {
    g_triggerVelocity = static_cast<float>(std::clamp(Wh_GetIntSetting(L"Behavior.trigger_velocity"), 1, 500));
    g_stopVelocity = static_cast<float>(std::clamp(Wh_GetIntSetting(L"Behavior.stop_velocity"), 1, 500));
    if (g_stopVelocity >= g_triggerVelocity) g_stopVelocity = std::max(1.0f, g_triggerVelocity * 0.5f);
    g_tailOffsetX = std::clamp(Wh_GetIntSetting(L"Behavior.tail_offset_x"), -64, 64);
    g_tailOffsetY = std::clamp(Wh_GetIntSetting(L"Behavior.tail_offset_y"), -64, 64);
    g_tailLength = std::clamp(Wh_GetIntSetting(L"Behavior.tail_length"), 2, kMaxTailLength);
    g_speedScaling = std::clamp(Wh_GetIntSetting(L"Behavior.speed_scaling"), 0, 1);
    g_widthMin = static_cast<float>(std::clamp(Wh_GetIntSetting(L"Appearance.width_min"), 1, 40));
    g_widthMax = static_cast<float>(std::clamp(Wh_GetIntSetting(L"Appearance.width_max"), 1, 60));
    g_coreWidthMin = static_cast<float>(std::clamp(Wh_GetIntSetting(L"Appearance.core_width_min"), 1, 40));
    g_coreWidthMax = static_cast<float>(std::clamp(Wh_GetIntSetting(L"Appearance.core_width_max"), 1, 60));
    g_alphaMin = std::clamp(Wh_GetIntSetting(L"Appearance.alpha_min"), 0, 100) / 100.0f;
    g_alphaMax = std::clamp(Wh_GetIntSetting(L"Appearance.alpha_max"), 0, 100) / 100.0f;
    g_taperPower = std::clamp(Wh_GetIntSetting(L"Appearance.taper_power"), 5, 30) / 10.0f;
    if (g_widthMax < g_widthMin) std::swap(g_widthMin, g_widthMax);
    if (g_coreWidthMax < g_coreWidthMin) std::swap(g_coreWidthMin, g_coreWidthMax);
    g_smoothIterations = std::clamp(Wh_GetIntSetting(L"Appearance.smooth_iterations"), 0, 4);
    g_gradientEnabled = std::clamp(Wh_GetIntSetting(L"Effects.gradient_enabled"), 0, 1);
    {
        PCWSTR value = Wh_GetStringSetting(L"Effects.gradient_tail_color");
        uint32_t parsed = 0;
        g_gradientTailRGB = ParseHexColor(value, parsed) ? parsed : 0x00FF00FF;
        Wh_FreeStringSetting(value);
    }
    g_glowEnabled = std::clamp(Wh_GetIntSetting(L"Effects.glow_enabled"), 0, 1);
    {
        PCWSTR value = Wh_GetStringSetting(L"Effects.glow_color");
        uint32_t parsed = 0;
        g_glowRGB = ParseHexColor(value, parsed) ? parsed : 0x00FFFFFF;
        Wh_FreeStringSetting(value);
    }
    g_glowWidthFactor = std::clamp(Wh_GetIntSetting(L"Effects.glow_width_factor"), 10, 30) / 10.0f;
    g_glowAlpha = std::clamp(Wh_GetIntSetting(L"Effects.glow_alpha"), 0, 100) / 100.0f;
    g_fadeEnabled = std::clamp(Wh_GetIntSetting(L"Behavior.fade_enabled"), 0, 1);
    g_fadeDecay = std::clamp(Wh_GetIntSetting(L"Behavior.fade_decay"), 50, 99) / 100.0f;
    {
        PCWSTR value = Wh_GetStringSetting(L"Color.trail_color_mode");
        g_trailColorMode = _wcsicmp(value, L"auto") == 0 ? 1 : 0;
        Wh_FreeStringSetting(value);
    }
    {
        PCWSTR value = Wh_GetStringSetting(L"Color.trail_color_manual");
        uint32_t parsed = 0;
        g_manualColorRGB = ParseHexColor(value, parsed) ? parsed : kFallbackCoreColor;
        Wh_FreeStringSetting(value);
    }
    {
        PCWSTR value = Wh_GetStringSetting(L"Color.outline_color_mode");
        g_outlineColorMode = _wcsicmp(value, L"manual") == 0 ? 1 : 0;
        Wh_FreeStringSetting(value);
    }
    {
        PCWSTR value = Wh_GetStringSetting(L"Color.outline_color_manual");
        uint32_t parsed = 0;
        g_manualOutlineRGB = ParseHexColor(value, parsed) ? parsed : kFallbackOuterColor;
        Wh_FreeStringSetting(value);
    }
    g_autoResampleInterval = std::clamp(Wh_GetIntSetting(L"Color.auto_resample_interval"), 0, kAutoResampleIntervalMaxMs);
    {
        PCWSTR value = Wh_GetStringSetting(L"Application.app_rules");
        ParseAppRules(value);
        Wh_FreeStringSetting(value);
    }
    g_cachedForegroundWindow = nullptr;
    g_cachedAppRule = 0;
    g_renderCache.ReserveForTailLength(g_tailLength);
    g_autoColorInitialized = false;
    g_lastAutoColorCursor = nullptr;
    g_lastAutoColorUpdate = 0;
    if (g_trailColorMode == 0) g_currentCoreRGB = g_manualColorRGB;
    g_currentOuterRGB = ResolveOutlineColor(g_currentCoreRGB);
}

static bool CoversMonitor(HWND hwnd) {
    const LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    if ((style & WS_CAPTION) != 0) return false;
    RECT windowRect = {};
    if (!GetWindowRect(hwnd, &windowRect)) return false;
    const HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo = {sizeof(monitorInfo)};
    if (!GetMonitorInfoW(monitor, &monitorInfo)) return false;
    return windowRect.left <= monitorInfo.rcMonitor.left + kFullscreenTolerancePx
        && windowRect.top <= monitorInfo.rcMonitor.top + kFullscreenTolerancePx
        && windowRect.right >= monitorInfo.rcMonitor.right - kFullscreenTolerancePx
        && windowRect.bottom >= monitorInfo.rcMonitor.bottom - kFullscreenTolerancePx;
}

static bool CheckStrongFullscreenSignal(HWND hwnd) {
    if (!hwnd) return false;
    QUERY_USER_NOTIFICATION_STATE state = QUNS_NOT_PRESENT;
    return SUCCEEDED(SHQueryUserNotificationState(&state)) && state == QUNS_RUNNING_D3D_FULL_SCREEN;
}

static bool IsFullscreenCandidate(DWORD now, HWND hwnd) {
    if (!hwnd || hwnd == GetDesktopWindow() || hwnd == GetShellWindow() || IsIconic(hwnd) || !IsWindowVisible(hwnd)) return false;
    if (hwnd != g_fullscreenForegroundWindow) {
        g_fullscreenForegroundWindow = hwnd;
        g_lastFullscreenStrongCheck = 0;
        g_fullscreenStrongSignal = false;
        g_fullscreenFalseSince = 0;
    }
    if (now - g_lastFullscreenStrongCheck >= kFullscreenStrongCheckIntervalMs) {
        g_lastFullscreenStrongCheck = now;
        g_fullscreenStrongSignal = CheckStrongFullscreenSignal(hwnd);
    }
    return g_fullscreenStrongSignal || CoversMonitor(hwnd);
}

static bool UpdateFullscreenState(DWORD now, HWND foreground) {
    const bool candidate = IsFullscreenCandidate(now, foreground);
    if (candidate) {
        g_fullscreenFalseSince = 0;
        if (!g_fullscreenSuppressed) {
            g_fullscreenSuppressed = true;
            HistoryClear();
            g_isSmearing = false;
            g_lowVelocityFrames = 0;
            g_fadeAlpha = 0.0f;
            HideOverlay();
        }
        return true;
    }
    if (g_fullscreenSuppressed) {
        if (g_fullscreenFalseSince == 0) g_fullscreenFalseSince = now;
        if (now - g_fullscreenFalseSince >= kFullscreenExitConfirmMs) {
            g_fullscreenSuppressed = false;
            g_fullscreenFalseSince = 0;
            HistoryClear();
            g_isSmearing = false;
            g_lowVelocityFrames = 0;
            g_fadeAlpha = 0.0f;
            HideOverlay();
        }
        return g_fullscreenSuppressed;
    }
    g_fullscreenFalseSince = 0;
    return false;
}

bool EnsureBackbuffer(int width, int height, HDC referenceDc) {
    if (width <= 0 || height <= 0 || !referenceDc) return false;
    if (g_backBufferDc && g_backBufferBitmap && width <= g_cachedWidth && height <= g_cachedHeight) return true;
    if (!g_backBufferDc) {
        g_backBufferDc = CreateCompatibleDC(referenceDc);
        if (!g_backBufferDc) return false;
    }
    const int newWidth = std::max(width, g_cachedWidth);
    const int newHeight = std::max(height, g_cachedHeight);
    ReleaseRenderResources();
    HBITMAP bitmap = Create32BitDIB(newWidth, newHeight);
    if (!bitmap) return false;
    const HGDIOBJ oldBitmap = SelectObject(g_backBufferDc, bitmap);
    if (!g_originalBitmap) g_originalBitmap = oldBitmap;
    else if (oldBitmap && oldBitmap != g_originalBitmap) DeleteObject(oldBitmap);
    g_backBufferBitmap = bitmap;
    g_cachedWidth = newWidth;
    g_cachedHeight = newHeight;
    g_backbufferIdleSince = 0;
    return true;
}

static void MaybeReleaseIdleBackbuffer(DWORD now) {
    if (g_isSmearing || g_historyCount > 0) {
        g_backbufferIdleSince = 0;
        return;
    }
    if (!g_backBufferDc || !g_backBufferBitmap) {
        g_backbufferIdleSince = 0;
        return;
    }
    if (g_backbufferIdleSince == 0) {
        g_backbufferIdleSince = now;
        return;
    }
    if (now - g_backbufferIdleSince >= kBackbufferIdleReleaseDelayMs) {
        ReleaseBackbuffer();
    }
}

bool EnsureD2DResources() {
    if (!g_d2dFactory) return false;
    if (g_renderTarget) return true;
    const D2D1_RENDER_TARGET_PROPERTIES properties = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
    HRESULT hr = g_d2dFactory->CreateDCRenderTarget(&properties, &g_renderTarget);
    if (FAILED(hr) || !g_renderTarget) return false;
    hr = g_renderTarget->CreateSolidColorBrush(ToColorF(g_currentOuterRGB, kTrailAlpha), &g_outerBrush);
    if (FAILED(hr)) { ReleaseRenderResources(); return false; }
    hr = g_renderTarget->CreateSolidColorBrush(ToColorF(g_currentCoreRGB, kTrailAlpha), &g_coreBrush);
    if (FAILED(hr)) { ReleaseRenderResources(); return false; }
    hr = g_renderTarget->CreateSolidColorBrush(ToColorF(g_glowRGB, g_glowAlpha), &g_glowBrush);
    if (FAILED(hr)) { ReleaseRenderResources(); return false; }
    D2D1_STROKE_STYLE_PROPERTIES strokeProperties = {};
    strokeProperties.startCap = D2D1_CAP_STYLE_ROUND;
    strokeProperties.endCap = D2D1_CAP_STYLE_ROUND;
    strokeProperties.lineJoin = D2D1_LINE_JOIN_ROUND;
    hr = g_d2dFactory->CreateStrokeStyle(strokeProperties, nullptr, 0, &g_strokeStyle);
    if (FAILED(hr)) { ReleaseRenderResources(); return false; }
    return true;
}

bool EnsureGradientBrush() {
    if (!g_gradientEnabled) {
        if (g_gradientBrush) { g_gradientBrush->Release(); g_gradientBrush = nullptr; }
        if (g_gradientStops) { g_gradientStops->Release(); g_gradientStops = nullptr; }
        g_gradientHeadCache = 0xFFFFFFFFu;
        g_gradientTailCache = 0xFFFFFFFFu;
        return false;
    }
    if (!g_renderTarget) return false;
    if (g_gradientBrush && g_gradientStops && g_gradientHeadCache == g_currentCoreRGB && g_gradientTailCache == g_gradientTailRGB) return true;
    if (g_gradientBrush) { g_gradientBrush->Release(); g_gradientBrush = nullptr; }
    if (g_gradientStops) { g_gradientStops->Release(); g_gradientStops = nullptr; }
    D2D1_GRADIENT_STOP stops[2] = {};
    stops[0].position = 0.0f;
    stops[0].color = ToColorF(g_currentCoreRGB, 1.0f);
    stops[1].position = 1.0f;
    stops[1].color = ToColorF(g_gradientTailRGB, 1.0f);
    HRESULT hr = g_renderTarget->CreateGradientStopCollection(stops, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &g_gradientStops);
    if (FAILED(hr)) return false;
    const D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES properties = {};
    hr = g_renderTarget->CreateLinearGradientBrush(properties, g_gradientStops, &g_gradientBrush);
    if (FAILED(hr)) { g_gradientStops->Release(); g_gradientStops = nullptr; return false; }
    g_gradientHeadCache = g_currentCoreRGB;
    g_gradientTailCache = g_gradientTailRGB;
    return true;
}

void SmoothTrail(int iterations, int renderLength) {
    auto& current = g_renderCache.smoothed;
    auto& next = g_renderCache.subdivision;
    current.clear();
    next.clear();
    const int pointCount = std::min(g_historyCount, std::max(renderLength, 2));
    for (int i = 0; i < pointCount; ++i) {
        const POINT& point = HistoryAt(i);
        current.push_back(D2D1::Point2F(static_cast<float>(point.x + g_tailOffsetX), static_cast<float>(point.y + g_tailOffsetY)));
    }
    for (int iteration = 0; iteration < iterations && current.size() >= 3; ++iteration) {
        next.clear();
        next.reserve(current.size() * 2);
        next.push_back(current.front());
        for (size_t i = 0; i + 1 < current.size(); ++i) {
            const auto& a = current[i];
            const auto& b = current[i + 1];
            next.push_back(D2D1::Point2F(0.75f * a.x + 0.25f * b.x, 0.75f * a.y + 0.25f * b.y));
            next.push_back(D2D1::Point2F(0.25f * a.x + 0.75f * b.x, 0.25f * a.y + 0.75f * b.y));
        }
        next.push_back(current.back());
        current.swap(next);
    }
    g_renderCache.PrepareTaper();
}

RECT CalculateTrailBounds(float maxHalfWidth) {
    if (g_renderCache.smoothed.empty()) return {0, 0, 0, 0};
    float minX = g_renderCache.smoothed.front().x;
    float minY = g_renderCache.smoothed.front().y;
    float maxX = minX;
    float maxY = minY;
    for (const auto& point : g_renderCache.smoothed) {
        minX = std::min(minX, point.x);
        minY = std::min(minY, point.y);
        maxX = std::max(maxX, point.x);
        maxY = std::max(maxY, point.y);
    }
    const float padding = maxHalfWidth + kRenderPadding;
    RECT result = {
        static_cast<LONG>(floorf(minX - padding)),
        static_cast<LONG>(floorf(minY - padding)),
        static_cast<LONG>(ceilf(maxX + padding)),
        static_cast<LONG>(ceilf(maxY + padding)),
    };
    if (result.right <= result.left) result.right = result.left + 1;
    if (result.bottom <= result.top) result.bottom = result.top + 1;
    return result;
}

static void DrawTrailStroke(ID2D1RenderTarget* target, ID2D1Brush* brush, float halfWidth, bool drawHead) {
    const auto& points = g_renderCache.smoothed;
    const auto& taper = g_renderCache.taper;
    if (points.size() < 2 || taper.size() < points.size() || !brush) return;
    for (size_t i = 0; i + 1 < points.size(); ++i) {
        const float width = std::max(0.1f, halfWidth * taper[i] * 2.0f);
        target->DrawLine(points[i], points[i + 1], brush, width, g_strokeStyle);
    }
    if (drawHead) target->FillEllipse(D2D1::Ellipse(points.front(), halfWidth, halfWidth), brush);
}

static int GetEffectiveTailLength(float speedNorm) {
    if (!g_speedScaling) return g_tailLength;
    return std::max(2, static_cast<int>(g_tailLength * (0.55f + 0.45f * std::clamp(speedNorm, 0.0f, 1.0f))));
}

void UpdateTrailState(const POINT& point, float velocity) {
    if (velocity > g_triggerVelocity && !g_isSmearing) {
        g_isSmearing = true;
        g_lowVelocityFrames = 0;
        g_fadeAlpha = 1.0f;
        g_smoothedSpeedNorm = 0.0f;
    } else if (velocity < g_stopVelocity && g_isSmearing) {
        if (++g_lowVelocityFrames > 2) {
            g_isSmearing = false;
            g_frozenSpeedNorm = g_smoothedSpeedNorm;
        }
    } else if (g_isSmearing) {
        g_lowVelocityFrames = 0;
    }
    if (g_isSmearing) {
        const float target = std::clamp((velocity - g_stopVelocity) / (g_triggerVelocity * 2.0f), 0.0f, 1.0f);
        g_smoothedSpeedNorm = g_smoothedSpeedNorm * 0.55f + target * 0.45f;
        HistoryPushFront(point);
        while (g_historyCount > g_tailLength) HistoryPopBack();
    } else if (g_fadeEnabled) {
        g_fadeAlpha *= g_fadeDecay;
        if (g_fadeAlpha < kFadeCutoff || g_historyCount < 2) {
            HistoryClear();
            g_fadeAlpha = 0.0f;
        }
    } else {
        if (g_historyCount > 0) HistoryPopBack();
        g_fadeAlpha = 1.0f;
    }
}

struct ColorCandidate { uint32_t rgb; int count; };

static bool ExtractCursorColorCandidates(std::vector<ColorCandidate>& out) {
    out.clear();
    CURSORINFO cursorInfo = {sizeof(cursorInfo)};
    if (!GetCursorInfo(&cursorInfo) || !(cursorInfo.flags & CURSOR_SHOWING) || !cursorInfo.hCursor) return false;
    ICONINFO iconInfo = {};
    if (!GetIconInfo(cursorInfo.hCursor, &iconInfo)) return false;
    const auto cleanup = [&] {
        if (iconInfo.hbmColor) DeleteObject(iconInfo.hbmColor);
        if (iconInfo.hbmMask) DeleteObject(iconInfo.hbmMask);
    };
    if (iconInfo.hbmColor) {
        BITMAP bitmap = {};
        if (GetObject(iconInfo.hbmColor, sizeof(bitmap), &bitmap) && bitmap.bmWidth > 0 && bitmap.bmHeight > 0 && bitmap.bmWidth <= 256 && bitmap.bmHeight <= 256) {
            const int width = bitmap.bmWidth;
            const int height = bitmap.bmHeight;
            BITMAPINFO info = {};
            info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            info.bmiHeader.biWidth = width;
            info.bmiHeader.biHeight = -height;
            info.bmiHeader.biPlanes = 1;
            info.bmiHeader.biBitCount = 32;
            info.bmiHeader.biCompression = BI_RGB;
            std::vector<uint32_t> pixels(static_cast<size_t>(width) * height);
            HDC screen = GetScreenDC();
            if (screen && GetDIBits(screen, iconInfo.hbmColor, 0, height, pixels.data(), &info, DIB_RGB_COLORS) == height) {
                std::unordered_map<uint32_t, int> histogram;
                histogram.reserve(64);
                for (uint32_t pixel : pixels) if (((pixel >> 24) & 0xFF) >= kCursorAlphaThreshold) ++histogram[pixel & 0x00FFFFFF];
                if (!histogram.empty()) {
                    out.reserve(histogram.size());
                    for (const auto& entry : histogram) out.push_back({entry.first, entry.second});
                    std::sort(out.begin(), out.end(), [](const ColorCandidate& a, const ColorCandidate& b) { return a.count > b.count; });
                    cleanup();
                    return true;
                }

                if (iconInfo.hbmMask) {
                    BITMAP maskBitmap = {};
                    if (GetObject(iconInfo.hbmMask, sizeof(maskBitmap), &maskBitmap) && maskBitmap.bmWidth >= width && maskBitmap.bmHeight >= height) {
                        BITMAPINFO maskInfo = {};
                        maskInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                        maskInfo.bmiHeader.biWidth = width;
                        maskInfo.bmiHeader.biHeight = -height;
                        maskInfo.bmiHeader.biPlanes = 1;
                        maskInfo.bmiHeader.biBitCount = 1;
                        maskInfo.bmiHeader.biCompression = BI_RGB;
                        maskInfo.bmiHeader.biClrUsed = 2;
                        const size_t maskStride = ((static_cast<size_t>(width) + 31u) / 32u) * 4u;
                        std::vector<uint8_t> maskBits(maskStride * static_cast<size_t>(height));
                        if (GetDIBits(screen, iconInfo.hbmMask, 0, height, maskBits.data(), &maskInfo, DIB_RGB_COLORS) == height) {
                            std::unordered_map<uint32_t, int> maskHistogram;
                            maskHistogram.reserve(64);
                            for (int y = 0; y < height; ++y) {
                                for (int x = 0; x < width; ++x) {
                                    const uint8_t rowByte = maskBits[static_cast<size_t>(y) * maskStride + static_cast<size_t>(x >> 3)];
                                    const bool transparent = (rowByte & (0x80u >> (x & 7))) != 0;
                                    if (!transparent) {
                                        const uint32_t rgb = pixels[static_cast<size_t>(y) * width + x] & 0x00FFFFFF;
                                        ++maskHistogram[rgb];
                                    }
                                }
                            }
                            out.reserve(maskHistogram.size());
                            for (const auto& entry : maskHistogram) out.push_back({entry.first, entry.second});
                            std::sort(out.begin(), out.end(), [](const ColorCandidate& a, const ColorCandidate& b) { return a.count > b.count; });
                            cleanup();
                            return !out.empty();
                        }
                    }
                }
            }
        }
    }
    cleanup();
    return false;
}

HDC g_backgroundSamplerDc = nullptr;
HBITMAP g_backgroundSamplerBitmap = nullptr;
HGDIOBJ g_backgroundSamplerOriginal = nullptr;
uint32_t* g_backgroundSamplerPixels = nullptr;
int g_backgroundSamplerSize = 0;
bool g_backgroundLuminanceValid = false;
POINT g_backgroundLuminanceCenter = {0, 0};
DWORD g_backgroundLuminanceUpdated = 0;
float g_backgroundLuminance = 0.5f;

static void ReleaseBackgroundSampler() {
    if (g_backgroundSamplerDc) {
        if (g_backgroundSamplerOriginal) SelectObject(g_backgroundSamplerDc, g_backgroundSamplerOriginal);
        DeleteDC(g_backgroundSamplerDc);
        g_backgroundSamplerDc = nullptr;
    }
    if (g_backgroundSamplerBitmap) {
        DeleteObject(g_backgroundSamplerBitmap);
        g_backgroundSamplerBitmap = nullptr;
    }
    g_backgroundSamplerOriginal = nullptr;
    g_backgroundSamplerPixels = nullptr;
    g_backgroundSamplerSize = 0;
    g_backgroundLuminanceValid = false;
}

static float SampleBackgroundLuminance(DWORD now, POINT center) {
    if (g_backgroundLuminanceValid) {
        const LONG dx = center.x - g_backgroundLuminanceCenter.x;
        const LONG dy = center.y - g_backgroundLuminanceCenter.y;
        const bool movedFarEnough = dx * dx + dy * dy >= kBackgroundResampleDistancePx * kBackgroundResampleDistancePx;
        if (!movedFarEnough && now - g_backgroundLuminanceUpdated < kBackgroundResampleIntervalMs) return g_backgroundLuminance;
    }

    const int size = kBackgroundSampleRadius * 2 + 1;
    HDC screen = GetScreenDC();
    if (!screen) return g_backgroundLuminanceValid ? g_backgroundLuminance : 0.5f;
    if (g_backgroundSamplerSize != size || !g_backgroundSamplerDc || !g_backgroundSamplerBitmap || !g_backgroundSamplerPixels) {
        ReleaseBackgroundSampler();
        BITMAPINFO info = {};
        info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth = size;
        info.bmiHeader.biHeight = -size;
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;
        void* bits = nullptr;
        g_backgroundSamplerBitmap = CreateDIBSection(nullptr, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
        if (!g_backgroundSamplerBitmap || !bits) { ReleaseBackgroundSampler(); return g_backgroundLuminanceValid ? g_backgroundLuminance : 0.5f; }
        g_backgroundSamplerPixels = static_cast<uint32_t*>(bits);
        g_backgroundSamplerDc = CreateCompatibleDC(screen);
        if (!g_backgroundSamplerDc) { ReleaseBackgroundSampler(); return g_backgroundLuminanceValid ? g_backgroundLuminance : 0.5f; }
        g_backgroundSamplerOriginal = SelectObject(g_backgroundSamplerDc, g_backgroundSamplerBitmap);
        g_backgroundSamplerSize = size;
    }
    if (!BitBlt(g_backgroundSamplerDc, 0, 0, size, size, screen, center.x - kBackgroundSampleRadius, center.y - kBackgroundSampleRadius, SRCCOPY)) return g_backgroundLuminanceValid ? g_backgroundLuminance : 0.5f;
    double sum = 0.0;
    int count = 0;
    for (int yIndex = 0; yIndex < kBackgroundSampleGrid; ++yIndex) {
        const int y = (size - 1) * yIndex / (kBackgroundSampleGrid - 1);
        for (int xIndex = 0; xIndex < kBackgroundSampleGrid; ++xIndex) {
            const int x = (size - 1) * xIndex / (kBackgroundSampleGrid - 1);
            const uint32_t pixel = g_backgroundSamplerPixels[y * size + x];
            sum += RgbLuminance(static_cast<uint8_t>((pixel >> 16) & 0xFF), static_cast<uint8_t>((pixel >> 8) & 0xFF), static_cast<uint8_t>(pixel & 0xFF));
            ++count;
        }
    }
    const float luminance = count ? static_cast<float>(sum / count) : 0.5f;
    g_backgroundLuminance = luminance;
    g_backgroundLuminanceCenter = center;
    g_backgroundLuminanceUpdated = now;
    g_backgroundLuminanceValid = true;
    return luminance;
}

static uint32_t ResolveOutlineColor(uint32_t core) {
    if (g_outlineColorMode == 1) return g_manualOutlineRGB;
    return RgbLuminance(core) > 0.5f ? kFallbackOuterColor : kFallbackCoreColor;
}

static void UpdateTrailColorIfNeeded(DWORD now) {
    if (g_trailColorMode == 0) {
        g_currentCoreRGB = g_manualColorRGB;
        g_currentOuterRGB = ResolveOutlineColor(g_currentCoreRGB);
        return;
    }
    CURSORINFO cursorInfo = {sizeof(cursorInfo)};
    const bool cursorInfoAvailable = GetCursorInfo(&cursorInfo);
    bool resample = !g_autoColorInitialized;
    if (!resample && cursorInfoAvailable) {
        if (g_autoResampleInterval > 0) resample = now - g_lastAutoColorUpdate >= static_cast<DWORD>(g_autoResampleInterval);
        else if (cursorInfo.hCursor != g_lastAutoColorCursor) resample = now - g_lastAutoColorUpdate >= kMinCursorChangeResampleIntervalMs;
    }
    if (!resample) return;
    if (cursorInfoAvailable) g_lastAutoColorCursor = cursorInfo.hCursor;
    g_lastAutoColorUpdate = now;
    g_autoColorInitialized = true;
    POINT point = {};
    if (!GetCursorPos(&point)) return;
    std::vector<ColorCandidate> candidates;
    if (!ExtractCursorColorCandidates(candidates) || candidates.empty()) {
        g_currentCoreRGB = kFallbackCoreColor;
        g_currentOuterRGB = ResolveOutlineColor(g_currentCoreRGB);
        return;
    }
    const bool wasOverlayVisible = g_windowVisible;
    if (wasOverlayVisible) HideOverlay();
    const float background = SampleBackgroundLuminance(now, point);
    if (wasOverlayVisible) ShowOverlay();
    for (const auto& candidate : candidates) {
        if (ContrastRatio(RgbLuminance(candidate.rgb), background) >= kMinColorContrast) {
            g_currentCoreRGB = candidate.rgb;
            g_currentOuterRGB = ResolveOutlineColor(candidate.rgb);
            return;
        }
    }
    g_currentCoreRGB = kFallbackCoreColor;
    g_currentOuterRGB = ResolveOutlineColor(g_currentCoreRGB);
}

bool RenderTrail(HWND hwnd) {
    if (g_historyCount < 2) return false;
    const float speed = g_speedScaling ? (g_isSmearing ? g_smoothedSpeedNorm : g_frozenSpeedNorm) : 1.0f;
    const int renderLength = GetEffectiveTailLength(speed);
    SmoothTrail(g_smoothIterations, renderLength);
    if (g_renderCache.smoothed.size() < 2) return false;
    const float outerHalf = g_widthMin + (g_widthMax - g_widthMin) * speed;
    const float coreHalf = g_coreWidthMin + (g_coreWidthMax - g_coreWidthMin) * speed;
    const float alphaScale = g_alphaMin + (g_alphaMax - g_alphaMin) * speed;
    const float alpha = kTrailAlpha * alphaScale * g_fadeAlpha;
    if (alpha < 0.02f) return false;
    const float boundsHalf = g_glowEnabled ? outerHalf * std::max(1.0f, g_glowWidthFactor) : outerHalf;
    const RECT bounds = CalculateTrailBounds(boundsHalf);
    const int width = bounds.right - bounds.left;
    const int height = bounds.bottom - bounds.top;
    HDC screen = GetScreenDC();
    if (!screen || !EnsureBackbuffer(width, height, screen) || !EnsureD2DResources()) return false;
    if (!g_dcBound) {
        const RECT bindRect = {0, 0, g_cachedWidth, g_cachedHeight};
        if (FAILED(g_renderTarget->BindDC(g_backBufferDc, &bindRect))) return false;
        g_dcBound = true;
    }
    g_renderTarget->BeginDraw();
    g_renderTarget->Clear(D2D1::ColorF(0, 0, 0, 0));
    g_renderTarget->SetTransform(D2D1::Matrix3x2F::Translation(-static_cast<float>(bounds.left), -static_cast<float>(bounds.top)));
    g_outerBrush->SetColor(ToColorF(g_currentOuterRGB, alpha));
    g_glowBrush->SetColor(ToColorF(g_glowRGB, g_glowAlpha * alpha));
    g_coreBrush->SetColor(ToColorF(g_currentCoreRGB, std::min(1.0f, alpha * 1.02f)));
    if (g_glowEnabled) DrawTrailStroke(g_renderTarget, g_glowBrush, outerHalf * std::max(1.0f, g_glowWidthFactor), true);
    DrawTrailStroke(g_renderTarget, g_outerBrush, outerHalf, true);
    ID2D1Brush* coreBrush = g_coreBrush;
    if (g_gradientEnabled && EnsureGradientBrush()) {
        g_gradientBrush->SetStartPoint(g_renderCache.smoothed.front());
        g_gradientBrush->SetEndPoint(g_renderCache.smoothed.back());
        g_gradientBrush->SetOpacity(alpha);
        coreBrush = g_gradientBrush;
    }
    DrawTrailStroke(g_renderTarget, coreBrush, coreHalf, true);
    g_renderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
    const HRESULT hr = g_renderTarget->EndDraw();
    if (hr == D2DERR_RECREATE_TARGET) {
        ReleaseRenderResources();
        return false;
    }
    if (FAILED(hr)) return false;
    BLENDFUNCTION blend = {};
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;
    POINT position = {bounds.left, bounds.top};
    SIZE size = {width, height};
    POINT source = {0, 0};
    return UpdateLayeredWindow(hwnd, screen, &position, &size, g_backBufferDc, &source, 0, &blend, ULW_ALPHA) != FALSE;
}

bool SmearFrame(HWND hwnd, DWORD now, bool renderFrame) {
    POINT point = {};
    if (!GetCursorPos(&point)) return false;
    const HWND foreground = GetForegroundWindow();
    const int appRule = CheckAppRuleCached(foreground);
    if (appRule < 0) {
        HistoryClear();
        g_isSmearing = false;
        g_lowVelocityFrames = 0;
        g_fadeAlpha = 0.0f;
        HideOverlay();
        MaybeReleaseIdleBackbuffer(now);
        return false;
    }
    if (appRule == 0 && UpdateFullscreenState(now, foreground)) {
        MaybeReleaseIdleBackbuffer(now);
        return false;
    }
    const bool wasSmearing = g_isSmearing;
    const int previousHistoryCount = g_historyCount;
    const int dx = point.x - g_lastPos.x;
    const int dy = point.y - g_lastPos.y;
    const float velocity = sqrtf(static_cast<float>(dx * dx + dy * dy));
    g_lastPos = point;
    UpdateTrailState(point, velocity);
    MaybeReleaseIdleBackbuffer(now);
    if (g_isSmearing || g_historyCount >= 2) UpdateTrailColorIfNeeded(now);
    const bool stateChanged = wasSmearing != g_isSmearing || (previousHistoryCount >= 2) != (g_historyCount >= 2);
    if (g_historyCount < 2) {
        HideOverlay();
        return false;
    }
    if (!renderFrame && !stateChanged) return false;
    if (!RenderTrail(hwnd)) {
        HideOverlay();
        return false;
    }
    ShowOverlay();
    return true;
}

LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case kSettingsChangedMessage:
        g_settingsDirty.store(true, std::memory_order_release);
        return 0;
    case WM_DISPLAYCHANGE:
        ReleaseBackbuffer();
        ReleaseBackgroundSampler();
        ReleaseScreenDC();
        g_lastFullscreenStrongCheck = 0;
        return 0;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProcW(hwnd, message, wParam, lParam);
    }
}

static LONGLONG QpcNow() {
    LARGE_INTEGER value = {};
    QueryPerformanceCounter(&value);
    return value.QuadPart;
}

static bool ArmFrameTimer(HANDLE timer, LONGLONG deadline, LONGLONG frequency) {
    const LONGLONG now = QpcNow();
    LONGLONG ticks = deadline - now;
    if (ticks < 1) ticks = 1;
    const LONGLONG due100ns = std::max<LONGLONG>(1, ticks * 10000000LL / frequency);
    LARGE_INTEGER due = {};
    due.QuadPart = -due100ns;
    return SetWaitableTimer(timer, &due, 0, nullptr, nullptr, FALSE) != FALSE;
}

static LONGLONG FrameIntervalTicks(LONGLONG frequency, int frameRate) {
    return std::max<LONGLONG>(1, frequency / frameRate);
}

DWORD WINAPI OverlayThreadProc(LPVOID) {
    const HRESULT coResult = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(coResult)) return 0;
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    LARGE_INTEGER qpcFrequency = {};
    if (!QueryPerformanceFrequency(&qpcFrequency) || qpcFrequency.QuadPart <= 0) {
        CoUninitialize();
        return 0;
    }
    HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_d2dFactory);
    if (FAILED(hr) || !g_d2dFactory) {
        CoUninitialize();
        return 0;
    }
    HINSTANCE instance = GetModuleHandle(nullptr);
    const wchar_t className[] = L"SmearFrameOverlayClass";
    WNDCLASSW windowClass = {};
    windowClass.lpfnWndProc = OverlayWndProc;
    windowClass.hInstance = instance;
    windowClass.lpszClassName = className;
    const ATOM atom = RegisterClassW(&windowClass);
    if (!atom) {
        g_d2dFactory->Release();
        g_d2dFactory = nullptr;
        CoUninitialize();
        return 0;
    }
    HWND hwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        className,
        L"SmearOverlay",
        WS_POPUP,
        0,
        0,
        1,
        1,
        nullptr,
        nullptr,
        instance,
        nullptr);
    if (!hwnd) {
        UnregisterClassW(className, instance);
        g_d2dFactory->Release();
        g_d2dFactory = nullptr;
        CoUninitialize();
        return 0;
    }
    if (!SetWindowDisplayAffinity(hwnd, WDA_EXCLUDEFROMCAPTURE)) {
        Wh_Log(L"SetWindowDisplayAffinity(WDA_EXCLUDEFROMCAPTURE) failed: %lu", GetLastError());
    }
    g_overlayHwnd.store(hwnd, std::memory_order_release);
    HideOverlay();
    GetCursorPos(&g_lastPos);
    g_fullscreenForegroundWindow = GetForegroundWindow();
    g_lastFullscreenStrongCheck = 0;
    g_fullscreenStrongSignal = false;
    g_fullscreenSuppressed = false;
    g_fullscreenFalseSince = 0;
    if (g_settingsDirty.exchange(false, std::memory_order_acq_rel)) LoadSettings();
    HANDLE frameTimer = CreateWaitableTimerExW(nullptr, nullptr, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);
    if (!frameTimer) frameTimer = CreateWaitableTimerW(nullptr, FALSE, nullptr);
    if (!frameTimer) {
        DestroyWindow(hwnd);
        g_overlayHwnd.store(nullptr, std::memory_order_release);
        UnregisterClassW(className, instance);
        g_d2dFactory->Release();
        g_d2dFactory = nullptr;
        CoUninitialize();
        return 0;
    }
    const LONGLONG simulationTicks = FrameIntervalTicks(qpcFrequency.QuadPart, kTargetFrameRate);
    const LONGLONG idleTicks = FrameIntervalTicks(qpcFrequency.QuadPart, kIdleFrameRate);
    const LONGLONG activeRenderTicks = FrameIntervalTicks(qpcFrequency.QuadPart, kActiveRenderFrameRate);
    const LONGLONG fadeRenderTicks = FrameIntervalTicks(qpcFrequency.QuadPart, kFadeRenderFrameRate);
    LONGLONG nextRenderDeadline = QpcNow();
    MSG message = {};
    bool running = true;
    while (running) {
        if (g_settingsDirty.exchange(false, std::memory_order_acq_rel)) LoadSettings();
        const bool idle = !g_isSmearing && g_historyCount < 2;
        const LONGLONG simulationInterval = idle ? idleTicks : simulationTicks;
        const LONGLONG sampleDeadline = QpcNow() + simulationInterval;
        if (!ArmFrameTimer(frameTimer, sampleDeadline, qpcFrequency.QuadPart)) break;
        HANDLE handles[] = {g_stopEvent, frameTimer};
        const DWORD waitResult = MsgWaitForMultipleObjectsEx(ARRAYSIZE(handles), handles, INFINITE, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
        if (waitResult == WAIT_OBJECT_0) {
            running = false;
            continue;
        }
        if (waitResult == WAIT_OBJECT_0 + 1) {
            const LONGLONG sampleNow = QpcNow();
            const bool renderFrame = (g_isSmearing || g_historyCount >= 2) && sampleNow >= nextRenderDeadline;
            const bool rendered = SmearFrame(hwnd, GetTickCount(), renderFrame);
            const bool active = g_isSmearing;
            const bool fading = !active && g_historyCount >= 2;
            if (active || fading) {
                const LONGLONG interval = active ? activeRenderTicks : fadeRenderTicks;
                if (rendered || sampleNow >= nextRenderDeadline) nextRenderDeadline = sampleNow + interval;
            } else {
                nextRenderDeadline = sampleNow + fadeRenderTicks;
            }
            continue;
        }
        if (waitResult == WAIT_OBJECT_0 + ARRAYSIZE(handles)) {
            while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
                if (message.message == WM_QUIT) {
                    running = false;
                    break;
                }
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
            continue;
        }
        break;
    }
    CancelWaitableTimer(frameTimer);
    CloseHandle(frameTimer);
    HideOverlay();
    ReleaseBackbuffer();
    ReleaseBackgroundSampler();
    ReleaseScreenDC();
    if (g_d2dFactory) {
        g_d2dFactory->Release();
        g_d2dFactory = nullptr;
    }
    g_overlayHwnd.store(nullptr, std::memory_order_release);
    UnregisterClassW(className, instance);
    CoUninitialize();
    return 0;
}

BOOL WhTool_ModInit() {
    LoadSettings();
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) return FALSE;
    g_threadHandle = CreateThread(nullptr, 0, OverlayThreadProc, nullptr, 0, nullptr);
    if (!g_threadHandle) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModUninit() {
    if (g_stopEvent) SetEvent(g_stopEvent);
    if (g_threadHandle) {
        WaitForSingleObject(g_threadHandle, INFINITE);
        CloseHandle(g_threadHandle);
        g_threadHandle = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}

void WhTool_ModSettingsChanged() {
    g_settingsDirty.store(true, std::memory_order_release);
    HWND hwnd = g_overlayHwnd.load(std::memory_order_acquire);
    if (hwnd && IsWindow(hwnd)) PostMessageW(hwnd, kSettingsChangedMessage, 0, 0);
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
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) && sessionId == 0) return FALSE;
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
        if (wcscmp(argv[i], L"-service") == 0
            || wcscmp(argv[i], L"-service-start") == 0
            || wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }
    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) isCurrentToolModProcess = true;
            break;
        }
    }
    LocalFree(argv);
    if (isExcluded) return FALSE;
    if (isCurrentToolModProcess) {
        g_toolModProcessMutex = CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }
        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }
        if (!WhTool_ModInit()) ExitProcess(1);
        IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders = (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);
        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;
        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }
    if (isToolModProcess) return FALSE;
    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) return;
    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath, ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }
    WCHAR commandLine[
        MAX_PATH + 2 +
        (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath, WH_MOD_ID);
    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }
    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken,
        LPCWSTR lpApplicationName,
        LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes,
        WINBOOL bInheritHandles,
        DWORD dwCreationFlags,
        LPVOID lpEnvironment,
        LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule, "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }
    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(
            nullptr,
            currentProcessPath,
            commandLine,
            nullptr,
            nullptr,
            FALSE,
            NORMAL_PRIORITY_CLASS,
            nullptr,
            nullptr,
            &si,
            &pi,
            nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) return;
    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) return;
    WhTool_ModUninit();
    ExitProcess(0);
}
