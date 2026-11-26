// ==WindhawkMod==
// @id              taskbar-custom-background
// @name            Alternate Custom Taskbar Background
// @description     Draws a solid overlay (with animated slide/fade) behind the taskbar, always or when a window is maximized.
// @version         2.0.0
// @author          threethan
// @github          https://github.com/threethan
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -lgdi32
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// Based "Taskbar auto-hide when maximized" by m417z.

// ==WindhawkModReadme==
/*
# Taskbar black box when maximized

Draws a solid overlay behind the taskbar area (bottom of the
screen) whenever a window is maximized (or always).

Includes a separate background layer with a blurred copy of whatever is behind the taskbar,
only shown when a window overlaps the taskbar area and suppressed when an app is in fullscreen!

Useful alongside Taskbar Styles w/ a transparent taskbar on OLED monitors.

Since this is its own layer, it won't fade with taskbar-fade mods.

Optionally, can add a blurred background when windows overlap the taskbar.

The overlay animates in/out per-monitor.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- showMode: maximized
  $name: Show overlay
  $description: >-
    Controls when the solid overlay is drawn: only while a window is maximized
    on a given monitor ("When window maximized"), or all the time
    ("Always").
  $options:
  - maximized: When window maximized
  - always: Always
- overlayHeight: 72
  $name: Overlay height (px)
  $description: >-
    Total height, in pixels, of the overlay drawn at the bottom of the
    screen, behind the taskbar. Includes the 1px top border.
- color: "000000"
  $name: Overlay color (hex RRGGBB)
  $description: >-
    Color of the overlay body, as a hex RRGGBB string. Defaults to OLED
    black.
- borderColor: "282828"
  $name: Top border color (hex RRGGBB)
  $description: >-
    Color of the 1px border drawn along the top edge.
- enableWallpaperBlur: true
  $name: Blurred backdrop background
  $description: >-
    Draws a blurred copy of whatever is behind the taskbar as a separate background layer.
- gradientWallpaperBlur: true
  $name: Wallpaper blur vertical gradient
  $description: >-
    Fades the blurred backdrop from transparent at the top of the taskbar
    to opaque at the bottom of the screen.
- centerBlur: true
  $name: Center blur fade
  $description: >-
    Fades out the blurred backdrop background further away from the center of the screen.
- blurRadius: 20
  $name: Blur radius (px)
  $description: >-
    Radius of the blur applied to the backdrop background layer.
- overlayOpacity: 200
  $name: Solid overlay opacity (0-255)
  $description: >-
    Opacity of the solid overlay body drawn over the blurred backdrop
    (0 = transparent backdrop only, 255 = solid black overlay).
- cornerRadius: 0
  $name: Corner flare radius (px)
  $description: >-
    Radius, in pixels, of a concave black "flare" at the top-left and
    top-right corners of the overlay: it rises above the overlay's flat
    top edge at the outer screen edges and curves smoothly down to meet
    it, like the corner of a floating shelf. The flares only appear while
    a window is actually maximized on that monitor, even if "Show
    overlay" is set to Always. Unlike the rest of the overlay (which sits
    behind the taskbar), the flares always render on top of every window,
    including the taskbar itself, so they're never covered by a
    maximized window's content reaching the screen edge. Set to 0 to
    disable them.
- animationDurationMs: 180
  $name: Animation duration (ms)
  $description: >-
    How long the slide/fade in and out animation takes, in milliseconds.
    Set to 0 to disable the animation and show/hide instantly.
- animationStyleIn: fade
  $name: In animation style
  $description: >-
    How the overlay animates in (appears).
  $options:
  - none: None
  - slide: Slide
  - fade: Fade
  - slideFade: Slide + fade
- animationStyleOut: slideFade
  $name: Out animation style
  $description: >-
    How the overlay animates out (disappears).
  $options:
  - none: None
  - slide: Slide
  - fade: Fade
  - slideFade: Slide + fade
- primaryMonitorOnly: false
  $name: Primary monitor only
  $description: >-
    Only draw the overlay on the primary monitor. When disabled, the
    overlay is drawn on every monitor that currently has a maximized
    window.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>

#include <unordered_map>
#include <string>
#include <vector>
#include <algorithm>

#ifndef DWMWA_CLOAKED
#define DWMWA_CLOAKED 14
#endif

#ifndef WDA_EXCLUDEFROMCAPTURE
#define WDA_EXCLUDEFROMCAPTURE 0x00000011
#endif

// -----------------------------------------------------------------------
// Settings
// -----------------------------------------------------------------------

enum class ShowMode {
    Maximized,
    Always,
};

enum class AnimationStyle {
    None,
    Slide,
    Fade,
    SlideFade,
};

struct {
    ShowMode showMode;
    int overlayHeight;
    COLORREF color; 
    COLORREF borderColor;
    int cornerRadius;
    int animationDurationMs;
    AnimationStyle animationStyleIn;
    AnimationStyle animationStyleOut;
    bool primaryMonitorOnly;
    bool enableWallpaperBlur;
    bool gradientWallpaperBlur;
    bool centerBlur;
    int blurRadius;
    int overlayOpacity;
} g_settings;

constexpr int kBorderHeight = 1;

COLORREF ParseHexColor(PCWSTR hex, COLORREF fallback) {
    if (!hex) {
        return fallback;
    }

    if (*hex == L'#') {
        hex++;
    }

    if (wcslen(hex) != 6) {
        return fallback;
    }

    WCHAR* end = nullptr;
    unsigned long value = wcstoul(hex, &end, 16);
    if (!end || *end != L'\0') {
        return fallback;
    }

    BYTE r = (BYTE)((value >> 16) & 0xFF);
    BYTE g = (BYTE)((value >> 8) & 0xFF);
    BYTE b = (BYTE)(value & 0xFF);
    return RGB(r, g, b);
}

AnimationStyle ParseAnimationStyle(PCWSTR str, AnimationStyle fallback) {
    if (!str) {
        return fallback;
    }
    if (_wcsicmp(str, L"none") == 0) {
        return AnimationStyle::None;
    }
    if (_wcsicmp(str, L"slide") == 0) {
        return AnimationStyle::Slide;
    }
    if (_wcsicmp(str, L"fade") == 0) {
        return AnimationStyle::Fade;
    }
    if (_wcsicmp(str, L"slideFade") == 0) {
        return AnimationStyle::SlideFade;
    }
    return fallback;
}

void LoadSettings() {
    PCWSTR showModeStr = Wh_GetStringSetting(L"showMode");
    g_settings.showMode = (showModeStr && _wcsicmp(showModeStr, L"always") == 0)
                               ? ShowMode::Always
                               : ShowMode::Maximized;
    Wh_FreeStringSetting(showModeStr);

    g_settings.overlayHeight = Wh_GetIntSetting(L"overlayHeight");
    if (g_settings.overlayHeight <= kBorderHeight) {
        g_settings.overlayHeight = 72;
    }

    PCWSTR colorStr = Wh_GetStringSetting(L"color");
    g_settings.color = ParseHexColor(colorStr, RGB(0, 0, 0));
    Wh_FreeStringSetting(colorStr);

    PCWSTR borderColorStr = Wh_GetStringSetting(L"borderColor");
    g_settings.borderColor = ParseHexColor(borderColorStr, RGB(0x0A, 0x0A, 0x0A));
    Wh_FreeStringSetting(borderColorStr);

    g_settings.enableWallpaperBlur = Wh_GetIntSetting(L"enableWallpaperBlur") != 0;
    g_settings.gradientWallpaperBlur = Wh_GetIntSetting(L"gradientWallpaperBlur") != 0;
    g_settings.centerBlur = Wh_GetIntSetting(L"centerBlur") != 0;

    g_settings.blurRadius = Wh_GetIntSetting(L"blurRadius");
    if (g_settings.blurRadius <= 0) {
        g_settings.blurRadius = 20;
    }

    g_settings.overlayOpacity = Wh_GetIntSetting(L"overlayOpacity");
    PCWSTR opacityStr = Wh_GetStringSetting(L"overlayOpacity");
    if (!opacityStr) {
        g_settings.overlayOpacity = 200;
    } else {
        Wh_FreeStringSetting(opacityStr);
        g_settings.overlayOpacity = std::clamp(g_settings.overlayOpacity, 0, 255);
    }

    g_settings.cornerRadius = Wh_GetIntSetting(L"cornerRadius");
    if (g_settings.cornerRadius < 0) {
        g_settings.cornerRadius = 0;
    }

    g_settings.animationDurationMs = Wh_GetIntSetting(L"animationDurationMs");
    if (g_settings.animationDurationMs < 0) {
        g_settings.animationDurationMs = 180;
    }

    PCWSTR animationStyleInStr = Wh_GetStringSetting(L"animationStyleIn");
    g_settings.animationStyleIn =
        ParseAnimationStyle(animationStyleInStr, AnimationStyle::Fade);
    Wh_FreeStringSetting(animationStyleInStr);

    PCWSTR animationStyleOutStr = Wh_GetStringSetting(L"animationStyleOut");
    g_settings.animationStyleOut =
        ParseAnimationStyle(animationStyleOutStr, AnimationStyle::SlideFade);
    Wh_FreeStringSetting(animationStyleOutStr);

    g_settings.primaryMonitorOnly = Wh_GetIntSetting(L"primaryMonitorOnly") != 0;
}

// -----------------------------------------------------------------------
// Blur Algorithm (Fast 3-Pass Box Blur for DIB Section)
// -----------------------------------------------------------------------

void BoxBlurRow(const DWORD* src, DWORD* dst, int width, int radius) {
    if (width <= 0 || radius <= 0) return;
    int windowSize = radius * 2 + 1;
    long sumR = 0, sumG = 0, sumB = 0;

    for (int i = -radius; i <= radius; ++i) {
        int x = std::clamp(i, 0, width - 1);
        DWORD pixel = src[x];
        sumR += (pixel >> 16) & 0xFF;
        sumG += (pixel >> 8) & 0xFF;
        sumB += pixel & 0xFF;
    }

    for (int x = 0; x < width; ++x) {
        BYTE r = (BYTE)(sumR / windowSize);
        BYTE g = (BYTE)(sumG / windowSize);
        BYTE b = (BYTE)(sumB / windowSize);
        dst[x] = 0xFF000000 | (r << 16) | (g << 8) | b;

        int removeX = std::clamp(x - radius, 0, width - 1);
        int addX = std::clamp(x + radius + 1, 0, width - 1);

        DWORD pRemove = src[removeX];
        DWORD pAdd = src[addX];

        sumR += (long)((pAdd >> 16) & 0xFF) - (long)((pRemove >> 16) & 0xFF);
        sumG += (long)((pAdd >> 8) & 0xFF) - (long)((pRemove >> 8) & 0xFF);
        sumB += (long)(pAdd & 0xFF) - (long)(pRemove & 0xFF);
    }
}

void BoxBlurCol(const DWORD* src, DWORD* dst, int width, int height, int radius) {
    if (height <= 0 || radius <= 0) return;
    int windowSize = radius * 2 + 1;

    for (int x = 0; x < width; ++x) {
        long sumR = 0, sumG = 0, sumB = 0;

        for (int i = -radius; i <= radius; ++i) {
            int y = std::clamp(i, 0, height - 1);
            DWORD pixel = src[y * width + x];
            sumR += (pixel >> 16) & 0xFF;
            sumG += (pixel >> 8) & 0xFF;
            sumB += pixel & 0xFF;
        }

        for (int y = 0; y < height; ++y) {
            BYTE r = (BYTE)(sumR / windowSize);
            BYTE g = (BYTE)(sumG / windowSize);
            BYTE b = (BYTE)(sumB / windowSize);
            dst[y * width + x] = 0xFF000000 | (r << 16) | (g << 8) | b;

            int removeY = std::clamp(y - radius, 0, height - 1);
            int addY = std::clamp(y + radius + 1, 0, height - 1);

            DWORD pRemove = src[removeY * width + x];
            DWORD pAdd = src[addY * width + x];

            sumR += (long)((pAdd >> 16) & 0xFF) - (long)((pRemove >> 16) & 0xFF);
            sumG += (long)((pAdd >> 8) & 0xFF) - (long)((pRemove >> 8) & 0xFF);
            sumB += (long)(pAdd & 0xFF) - (long)(pRemove & 0xFF);
        }
    }
}

void FastBlur32(DWORD* pixels, int width, int height, int radius, int passes = 3) {
    if (radius <= 0 || width <= 0 || height <= 0 || !pixels) return;
    std::vector<DWORD> temp(width * height);

    for (int p = 0; p < passes; ++p) {
        for (int y = 0; y < height; ++y) {
            BoxBlurRow(pixels + y * width, temp.data() + y * width, width, radius);
        }
        BoxBlurCol(temp.data(), pixels, width, height, radius);
    }
}

// -----------------------------------------------------------------------
// Overlay window management
// -----------------------------------------------------------------------

std::wstring g_overlayClassName;
std::wstring g_timerOwnerClassName;
std::wstring g_cornerClassName;
std::wstring g_bgClassName;

std::wstring MakeUniqueClassName(PCWSTR baseName) {
    return std::wstring(baseName) + L"_" + std::to_wstring(GetCurrentProcessId()) +
           L"_" + std::to_wstring(GetTickCount64());
}

HWND g_timerOwnerWnd;

struct OverlappingWindowInfo {
    HWND hwnd;
    RECT rect;

    bool operator==(const OverlappingWindowInfo& other) const {
        return hwnd == other.hwnd &&
               rect.left == other.rect.left &&
               rect.top == other.rect.top &&
               rect.right == other.rect.right &&
               rect.bottom == other.rect.bottom;
    }
};

struct OverlayInfo {
    HWND hwnd = nullptr;           // Solid color overlay
    HWND bgHwnd = nullptr;         // Blurred backdrop background
    RECT monitorRect{};

    bool solidTargetVisible = false;
    float solidProgress = 0.0f;
    ULONGLONG solidAnimStartTick = 0;
    float solidAnimStartProgress = 0.0f;

    bool bgTargetVisible = false;
    float bgProgress = 0.0f;
    ULONGLONG bgAnimStartTick = 0;
    float bgAnimStartProgress = 0.0f;

    HWND cornerLeftHwnd = nullptr;
    HWND cornerRightHwnd = nullptr;
    int cornerRegionRadius = -1;

    HBITMAP hBlurredBmp = nullptr;
    int bmpWidth = 0;
    int bmpHeight = 0;
    int blurOffsetY = 0;

    std::vector<OverlappingWindowInfo> lastOverlappingWindows;
};

std::unordered_map<HMONITOR, OverlayInfo> g_overlays;
HBRUSH g_overlayBrush;
HBRUSH g_borderBrush;

constexpr UINT_PTR kStateTimerId = 1;
constexpr UINT_PTR kAnimTimerId = 2;
constexpr UINT kStatePollIntervalMs = 200;
constexpr UINT kAnimIntervalMs = 15;

bool IsWindowClassName(HWND hWnd, PCWSTR className) {
    WCHAR buffer[64];
    return GetClassName(hWnd, buffer, ARRAYSIZE(buffer)) &&
           _wcsicmp(buffer, className) == 0;
}

bool IsMonitorFullscreen(HMONITOR monitor, const RECT& monitorRect) {
    struct EnumParam {
        HMONITOR monitor;
        RECT monitorRect;
        bool isFullscreen;
    } param{monitor, monitorRect, false};

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto& p = *reinterpret_cast<EnumParam*>(lParam);

            if (IsWindowClassName(hWnd, g_overlayClassName.c_str()) ||
                IsWindowClassName(hWnd, g_bgClassName.c_str()) ||
                IsWindowClassName(hWnd, g_cornerClassName.c_str()) ||
                IsWindowClassName(hWnd, g_timerOwnerClassName.c_str()) ||
                IsWindowClassName(hWnd, L"Shell_TrayWnd") ||
                IsWindowClassName(hWnd, L"Shell_SecondaryTrayWnd") ||
                IsWindowClassName(hWnd, L"Progman") ||
                IsWindowClassName(hWnd, L"WorkerW")) {
                return TRUE;
            }

            if (!IsWindowVisible(hWnd) || IsIconic(hWnd)) {
                return TRUE;
            }

            BOOL isCloaked = FALSE;
            DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &isCloaked, sizeof(isCloaked));
            if (isCloaked) {
                return TRUE;
            }

            RECT winRect{};
            if (!GetWindowRect(hWnd, &winRect)) {
                return TRUE;
            }

            // Check if window covers or exceeds full monitor bounds (with 2px tolerance for borderless windows)
            if (winRect.left <= p.monitorRect.left + 2 &&
                winRect.top <= p.monitorRect.top + 2 &&
                winRect.right >= p.monitorRect.right - 2 &&
                winRect.bottom >= p.monitorRect.bottom - 2) {
                p.isFullscreen = true;
                return FALSE;
            }

            return TRUE;
        },
        reinterpret_cast<LPARAM>(&param));

    return param.isFullscreen;
}

void CollectOverlappingWindowsForMonitor(
    HMONITOR monitor,
    const RECT& overlayRect,
    std::vector<OverlappingWindowInfo>& outList) {
    outList.clear();

    struct EnumParam {
        HMONITOR monitor;
        RECT overlayRect;
        std::vector<OverlappingWindowInfo>* list;
    } param{monitor, overlayRect, &outList};

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto& p = *reinterpret_cast<EnumParam*>(lParam);

            if (IsWindowClassName(hWnd, g_overlayClassName.c_str()) ||
                IsWindowClassName(hWnd, g_bgClassName.c_str()) ||
                IsWindowClassName(hWnd, g_cornerClassName.c_str()) ||
                IsWindowClassName(hWnd, g_timerOwnerClassName.c_str()) ||
                IsWindowClassName(hWnd, L"Shell_TrayWnd") ||
                IsWindowClassName(hWnd, L"Shell_SecondaryTrayWnd") ||
                IsWindowClassName(hWnd, L"Progman") ||
                IsWindowClassName(hWnd, L"WorkerW")) {
                return TRUE;
            }

            if (!IsWindowVisible(hWnd) || IsIconic(hWnd)) {
                return TRUE;
            }

            BOOL isCloaked = FALSE;
            DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &isCloaked, sizeof(isCloaked));
            if (isCloaked) {
                return TRUE;
            }

            RECT winRect{};
            if (!GetWindowRect(hWnd, &winRect)) {
                return TRUE;
            }

            RECT intersectRect{};
            if (IntersectRect(&intersectRect, &winRect, &p.overlayRect) && !IsRectEmpty(&intersectRect)) {
                p.list->push_back({hWnd, winRect});
            }

            return TRUE;
        },
        reinterpret_cast<LPARAM>(&param));
}

void ClearWallpaperBitmap(OverlayInfo& info) {
    if (info.hBlurredBmp) {
        DeleteObject(info.hBlurredBmp);
        info.hBlurredBmp = nullptr;
    }
    info.bmpWidth = 0;
    info.bmpHeight = 0;
    info.blurOffsetY = 0;
}

void RefreshScreenBlurBitmap(OverlayInfo& info) {
    ClearWallpaperBitmap(info);

    if (!g_settings.enableWallpaperBlur) {
        return;
    }

    int width = info.monitorRect.right - info.monitorRect.left;
    int height = g_settings.overlayHeight;
    int radius = g_settings.blurRadius;

    if (width <= 0 || height <= 0) {
        return;
    }

    int finalY = info.monitorRect.bottom - height;
    int captureY = std::max((int)info.monitorRect.top, finalY - radius);
    int blurOffsetY = finalY - captureY;
    int captureHeight = height + blurOffsetY;

    // Exclude overlay windows from screen capture so BitBlt captures whatever is behind them
    if (info.hwnd) SetWindowDisplayAffinity(info.hwnd, WDA_EXCLUDEFROMCAPTURE);
    if (info.bgHwnd) SetWindowDisplayAffinity(info.bgHwnd, WDA_EXCLUDEFROMCAPTURE);

    HDC hdcScreen = GetDC(nullptr);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -captureHeight;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    DWORD* pBits = nullptr;
    HBITMAP hDib = CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS, (void**)&pBits, nullptr, 0);

    if (hDib && pBits) {
        HBITMAP hOldBmp = (HBITMAP)SelectObject(hdcMem, hDib);

        // Capture live screen pixels (whatever is behind the taskbar)
        BitBlt(hdcMem, 0, 0, width, captureHeight, hdcScreen, info.monitorRect.left, captureY, SRCCOPY);

        SelectObject(hdcMem, hOldBmp);

        // 1. Multi-pass box blur on captured screen content
        FastBlur32(pBits, width, captureHeight, radius, 3);

        // 2. Pre-compute horizontal center falloff factor for each column
        std::vector<float> hFactors(width);
        float centerX = (float)width / 2.0f;
        float rCenter = (width * 0.60f) / 2.0f;

        for (int x = 0; x < width; ++x) {
            if (!g_settings.centerBlur) {
                hFactors[x] = 1.0f;
            } else {
                float dx = std::abs((float)x - centerX);
                float tx = std::clamp(dx / rCenter, 0.0f, 1.0f);
                hFactors[x] = 1.0f - (3.0f * tx * tx - 2.0f * tx * tx * tx); // Smoothstep falloff
            }
        }

        // 3. Apply 2D alpha mask & pre-multiply alpha for UpdateLayeredWindow
        for (int y = 0; y < captureHeight; ++y) {
            int overlayRowY = y - blurOffsetY;
            float vFactor = 1.0f;

            if (g_settings.gradientWallpaperBlur) {
                if (overlayRowY <= 0) {
                    vFactor = 0.0f;
                } else {
                    vFactor = std::clamp((float)overlayRowY / (float)(height - 1), 0.0f, 1.0f);
                }
            }

            for (int x = 0; x < width; ++x) {
                float combinedFactor = std::clamp(vFactor * hFactors[x], 0.0f, 1.0f);
                BYTE alpha = (BYTE)(combinedFactor * 255.0f);

                DWORD pixel = pBits[y * width + x];
                BYTE r = (BYTE)((pixel >> 16) & 0xFF);
                BYTE g = (BYTE)((pixel >> 8) & 0xFF);
                BYTE b = (BYTE)(pixel & 0xFF);

                BYTE premulR = (BYTE)((r * alpha) / 255);
                BYTE premulG = (BYTE)((g * alpha) / 255);
                BYTE premulB = (BYTE)((b * alpha) / 255);

                pBits[y * width + x] = ((DWORD)alpha << 24) | ((DWORD)premulR << 16) | ((DWORD)premulG << 8) | (DWORD)premulB;
            }
        }

        info.hBlurredBmp = hDib;
        info.bmpWidth = width;
        info.bmpHeight = captureHeight;
        info.blurOffsetY = blurOffsetY;
    } else {
        if (hDib) DeleteObject(hDib);
    }

    DeleteDC(hdcMem);
    ReleaseDC(nullptr, hdcScreen);
}

LRESULT CALLBACK BgWndProc(HWND hWnd,
                           UINT uMsg,
                           WPARAM wParam,
                           LPARAM lParam) {
    switch (uMsg) {
        case WM_NCHITTEST:
            return HTTRANSPARENT;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK OverlayWndProc(HWND hWnd,
                                 UINT uMsg,
                                 WPARAM wParam,
                                 LPARAM lParam) {
    switch (uMsg) {
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_ERASEBKGND: {
            HDC hdc = (HDC)wParam;
            RECT rect;
            GetClientRect(hWnd, &rect);

            RECT borderRect = rect;
            borderRect.bottom = borderRect.top + kBorderHeight;
            FillRect(hdc, &borderRect, g_borderBrush);

            RECT bodyRect = rect;
            bodyRect.top += kBorderHeight;
            FillRect(hdc, &bodyRect, g_overlayBrush);

            return 1;
        }
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

HWND FindTaskbarForMonitor(HMONITOR monitor) {
    struct EnumParam {
        HMONITOR monitor;
        HWND result;
    } param{monitor, nullptr};

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto& p = *reinterpret_cast<EnumParam*>(lParam);

            if (!IsWindowClassName(hWnd, L"Shell_TrayWnd") &&
                !IsWindowClassName(hWnd, L"Shell_SecondaryTrayWnd")) {
                return TRUE;
            }

            if (MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST) ==
                p.monitor) {
                p.result = hWnd;
                return FALSE;
            }

            return TRUE;
        },
        reinterpret_cast<LPARAM>(&param));

    return param.result;
}

HWND CreateOverlayWindow() {
    HWND hwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        g_overlayClassName.c_str(), L"", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr,
        GetModuleHandle(nullptr), nullptr);
    if (hwnd) {
        SetWindowDisplayAffinity(hwnd, WDA_EXCLUDEFROMCAPTURE);
    }
    return hwnd;
}

HWND CreateBgWindow() {
    HWND hwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        g_bgClassName.c_str(), L"", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr,
        GetModuleHandle(nullptr), nullptr);
    if (hwnd) {
        SetWindowDisplayAffinity(hwnd, WDA_EXCLUDEFROMCAPTURE);
    }
    return hwnd;
}

LRESULT CALLBACK CornerWndProc(HWND hWnd,
                                UINT uMsg,
                                WPARAM wParam,
                                LPARAM lParam) {
    switch (uMsg) {
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_ERASEBKGND: {
            HDC hdc = (HDC)wParam;
            RECT rect;
            GetClientRect(hWnd, &rect);
            FillRect(hdc, &rect, g_overlayBrush);
            return 1;
        }
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

HWND CreateCornerWindow() {
    return CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        g_cornerClassName.c_str(), L"", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr,
        GetModuleHandle(nullptr), nullptr);
}

void SetCornerWindowShape(HWND hwnd, int radius, bool isLeft) {
    SetWindowPos(hwnd, nullptr, 0, 0, radius, radius,
                 SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

    HRGN square = CreateRectRgn(0, 0, radius, radius);
    HRGN circle = isLeft
                      ? CreateEllipticRgn(0, -radius, radius * 2, radius)
                      : CreateEllipticRgn(-radius, -radius, radius, radius);
    CombineRgn(square, square, circle, RGN_DIFF);
    DeleteObject(circle);

    if (!SetWindowRgn(hwnd, square, TRUE)) {
        DeleteObject(square);
    }
}

void EnsureCornerWindows(OverlayInfo& info, bool monitorHasMaximizedWindow) {
    int width = info.monitorRect.right - info.monitorRect.left;
    int radius = monitorHasMaximizedWindow
                     ? std::clamp(g_settings.cornerRadius, 0, width / 2)
                     : 0;

    if (radius <= 0) {
        if (info.cornerLeftHwnd) {
            DestroyWindow(info.cornerLeftHwnd);
            info.cornerLeftHwnd = nullptr;
        }
        if (info.cornerRightHwnd) {
            DestroyWindow(info.cornerRightHwnd);
            info.cornerRightHwnd = nullptr;
        }
        info.cornerRegionRadius = -1;
        return;
    }

    if (!info.cornerLeftHwnd) {
        info.cornerLeftHwnd = CreateCornerWindow();
    }
    if (!info.cornerRightHwnd) {
        info.cornerRightHwnd = CreateCornerWindow();
    }

    if (info.cornerRegionRadius == radius) {
        return;
    }
    info.cornerRegionRadius = radius;

    if (info.cornerLeftHwnd) {
        SetCornerWindowShape(info.cornerLeftHwnd, radius, /*isLeft=*/true);
    }
    if (info.cornerRightHwnd) {
        SetCornerWindowShape(info.cornerRightHwnd, radius, /*isLeft=*/false);
    }
}

void ApplyOverlayVisual(const OverlayInfo& info) {
    int height = g_settings.overlayHeight;
    int width = info.monitorRect.right - info.monitorRect.left;
    int x = info.monitorRect.left;
    int finalY = info.monitorRect.bottom - height;

    // 1. Backdrop background layer
    if (info.bgHwnd && g_settings.enableWallpaperBlur && info.bgProgress > 0.0f && info.hBlurredBmp) {
        AnimationStyle bgStyle = info.bgTargetVisible ? g_settings.animationStyleIn : g_settings.animationStyleOut;
        bool useBgSlide = (bgStyle == AnimationStyle::Slide || bgStyle == AnimationStyle::SlideFade);
        bool useBgFade = (bgStyle == AnimationStyle::Fade || bgStyle == AnimationStyle::SlideFade);

        int bgSlideOffset = useBgSlide ? (int)((1.0f - info.bgProgress) * height) : 0;
        int bgY = finalY + bgSlideOffset;

        BYTE animAlpha = useBgFade ? (BYTE)std::clamp(info.bgProgress * 255.0f, 0.0f, 255.0f) : 255;

        BLENDFUNCTION blend{AC_SRC_OVER, 0, animAlpha, AC_SRC_ALPHA};
        POINT ptDst{x, bgY};
        SIZE sizeDst{width, height};
        POINT ptSrc{0, info.blurOffsetY};

        HDC hdcScreen = GetDC(nullptr);
        HDC hdcMem = CreateCompatibleDC(hdcScreen);
        HBITMAP hOld = (HBITMAP)SelectObject(hdcMem, info.hBlurredBmp);

        UpdateLayeredWindow(info.bgHwnd, hdcScreen, &ptDst, &sizeDst, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);

        SelectObject(hdcMem, hOld);
        DeleteDC(hdcMem);
        ReleaseDC(nullptr, hdcScreen);

        if (!IsWindowVisible(info.bgHwnd)) {
            ShowWindow(info.bgHwnd, SW_SHOWNOACTIVATE);
        }
    } else if (info.bgHwnd && IsWindowVisible(info.bgHwnd)) {
        ShowWindow(info.bgHwnd, SW_HIDE);
    }

    // 2. Solid color overlay
    if (info.solidProgress > 0.0f) {
        AnimationStyle solidStyle = info.solidTargetVisible ? g_settings.animationStyleIn : g_settings.animationStyleOut;
        bool useSolidSlide = (solidStyle == AnimationStyle::Slide || solidStyle == AnimationStyle::SlideFade);
        bool useSolidFade = (solidStyle == AnimationStyle::Fade || solidStyle == AnimationStyle::SlideFade);

        int solidSlideOffset = useSolidSlide ? (int)((1.0f - info.solidProgress) * height) : 0;
        int solidY = finalY + solidSlideOffset;

        SetWindowPos(info.hwnd, nullptr, x, solidY, width, height,
                     SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);

        BYTE animAlpha = useSolidFade ? (BYTE)std::clamp(info.solidProgress * 255.0f, 0.0f, 255.0f)
                                      : (BYTE)(info.solidProgress > 0.0f ? 255 : 0);
        BYTE overlayAlpha = (BYTE)((animAlpha * g_settings.overlayOpacity) / 255);
        SetLayeredWindowAttributes(info.hwnd, 0, overlayAlpha, LWA_ALPHA);

        int radius = info.cornerRegionRadius;
        if (radius > 0 && info.cornerLeftHwnd && info.cornerRightHwnd) {
            int cornerY = solidY - radius + 1;
            SetWindowPos(info.cornerLeftHwnd, nullptr, x, cornerY, radius, radius,
                         SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
            SetWindowPos(info.cornerRightHwnd, nullptr, x + width - radius, cornerY,
                         radius, radius, SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
            SetLayeredWindowAttributes(info.cornerLeftHwnd, 0, overlayAlpha, LWA_ALPHA);
            SetLayeredWindowAttributes(info.cornerRightHwnd, 0, overlayAlpha, LWA_ALPHA);
        }
    } else {
        if (IsWindowVisible(info.hwnd)) {
            ShowWindow(info.hwnd, SW_HIDE);
        }
        if (info.cornerLeftHwnd && IsWindowVisible(info.cornerLeftHwnd)) {
            ShowWindow(info.cornerLeftHwnd, SW_HIDE);
        }
        if (info.cornerRightHwnd && IsWindowVisible(info.cornerRightHwnd)) {
            ShowWindow(info.cornerRightHwnd, SW_HIDE);
        }
    }
}

void AnimateOverlays() {
    ULONGLONG now = GetTickCount64();

    for (auto& [monitor, info] : g_overlays) {
        bool solidAnimating = (info.solidProgress != (info.solidTargetVisible ? 1.0f : 0.0f));
        bool bgAnimating = (info.bgProgress != (info.bgTargetVisible ? 1.0f : 0.0f));

        if (!solidAnimating && !bgAnimating) {
            if (info.solidProgress == 0.0f && IsWindowVisible(info.hwnd)) {
                ShowWindow(info.hwnd, SW_HIDE);
                if (info.cornerLeftHwnd) ShowWindow(info.cornerLeftHwnd, SW_HIDE);
                if (info.cornerRightHwnd) ShowWindow(info.cornerRightHwnd, SW_HIDE);
            }
            if (info.bgProgress == 0.0f && info.bgHwnd && IsWindowVisible(info.bgHwnd)) {
                ShowWindow(info.bgHwnd, SW_HIDE);
            }
            continue;
        }

        if (solidAnimating) {
            float target = info.solidTargetVisible ? 1.0f : 0.0f;
            AnimationStyle style = info.solidTargetVisible ? g_settings.animationStyleIn : g_settings.animationStyleOut;
            int duration = (style == AnimationStyle::None) ? 0 : g_settings.animationDurationMs;
            if (duration <= 0) {
                info.solidProgress = target;
            } else {
                ULONGLONG elapsed = now - info.solidAnimStartTick;
                float t = std::clamp((float)elapsed / (float)duration, 0.0f, 1.0f);
                info.solidProgress = info.solidAnimStartProgress + (target - info.solidAnimStartProgress) * t;
            }
        }

        if (bgAnimating) {
            float target = info.bgTargetVisible ? 1.0f : 0.0f;
            AnimationStyle style = info.bgTargetVisible ? g_settings.animationStyleIn : g_settings.animationStyleOut;
            int duration = (style == AnimationStyle::None) ? 0 : g_settings.animationDurationMs;
            if (duration <= 0) {
                info.bgProgress = target;
            } else {
                ULONGLONG elapsed = now - info.bgAnimStartTick;
                float t = std::clamp((float)elapsed / (float)duration, 0.0f, 1.0f);
                info.bgProgress = info.bgAnimStartProgress + (target - info.bgAnimStartProgress) * t;
            }
        }

        ApplyOverlayVisual(info);
    }
}

BOOL CALLBACK CollectAllMonitorsProc(HMONITOR hMonitor,
                                     HDC /*hdcMonitor*/,
                                     LPRECT /*lprcMonitor*/,
                                     LPARAM lParam) {
    auto& result = *reinterpret_cast<std::unordered_map<HMONITOR, RECT>*>(lParam);

    if (g_settings.primaryMonitorOnly &&
        hMonitor != MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY)) {
        return TRUE;
    }

    MONITORINFO mi{.cbSize = sizeof(MONITORINFO)};
    GetMonitorInfo(hMonitor, &mi);
    result[hMonitor] = mi.rcMonitor;

    return TRUE;
}

void CollectMonitorsWithMaximizedWindow(
    std::unordered_map<HMONITOR, RECT>& result) {
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto& result =
                *reinterpret_cast<std::unordered_map<HMONITOR, RECT>*>(lParam);

            if (IsWindowClassName(hWnd, g_overlayClassName.c_str())) {
                return TRUE;
            }

            if (!IsWindowVisible(hWnd) || IsIconic(hWnd)) {
                return TRUE;
            }

            BOOL isCloaked = FALSE;
            DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &isCloaked,
                                   sizeof(isCloaked));
            if (isCloaked) {
                return TRUE;
            }

            WINDOWPLACEMENT wp{.length = sizeof(WINDOWPLACEMENT)};
            if (!GetWindowPlacement(hWnd, &wp) ||
                wp.showCmd != SW_SHOWMAXIMIZED) {
                return TRUE;
            }

            HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
            if (!monitor) {
                return TRUE;
            }

            if (g_settings.primaryMonitorOnly &&
                monitor != MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY)) {
                return TRUE;
            }

            if (result.contains(monitor)) {
                return TRUE;
            }

            MONITORINFO mi{.cbSize = sizeof(MONITORINFO)};
            GetMonitorInfo(monitor, &mi);
            result[monitor] = mi.rcMonitor;

            return TRUE;
        },
        reinterpret_cast<LPARAM>(&result));
}

void UpdateTargets() {
    std::unordered_map<HMONITOR, RECT> allMonitors;
    EnumDisplayMonitors(nullptr, nullptr, CollectAllMonitorsProc,
                         reinterpret_cast<LPARAM>(&allMonitors));

    std::unordered_map<HMONITOR, RECT> monitorsWithMaximizedWindow;
    CollectMonitorsWithMaximizedWindow(monitorsWithMaximizedWindow);

    ULONGLONG now = GetTickCount64();

    for (const auto& [monitor, monitorRect] : allMonitors) {
        auto it = g_overlays.find(monitor);
        if (it == g_overlays.end()) {
            OverlayInfo info;
            info.hwnd = CreateOverlayWindow();
            info.bgHwnd = CreateBgWindow();
            info.monitorRect = monitorRect;
            it = g_overlays.emplace(monitor, info).first;
        }

        OverlayInfo& info = it->second;
        bool rectChanged = (info.monitorRect.left != monitorRect.left ||
                            info.monitorRect.top != monitorRect.top ||
                            info.monitorRect.right != monitorRect.right ||
                            info.monitorRect.bottom != monitorRect.bottom);
        info.monitorRect = monitorRect;

        // Check if an application is in fullscreen on this monitor
        bool isFullscreen = IsMonitorFullscreen(monitor, monitorRect);

        // Calculate taskbar overlay rect
        int height = g_settings.overlayHeight;
        RECT overlayRect{
            monitorRect.left,
            monitorRect.bottom - height,
            monitorRect.right,
            monitorRect.bottom
        };

        // Collect current overlapping windows for this monitor
        std::vector<OverlappingWindowInfo> currentOverlappingWindows;
        if (!isFullscreen) {
            CollectOverlappingWindowsForMonitor(monitor, overlayRect, currentOverlappingWindows);
        }

        bool windowsChanged = (currentOverlappingWindows != info.lastOverlappingWindows);
        bool hasOverlappingWindows = !currentOverlappingWindows.empty();

        // Only re-capture and re-blur when an overlapping window's rect changes!
        if (!info.hBlurredBmp || rectChanged || windowsChanged) {
            info.lastOverlappingWindows = currentOverlappingWindows;
            if (hasOverlappingWindows && g_settings.enableWallpaperBlur && !isFullscreen) {
                RefreshScreenBlurBitmap(info);
            }
        }

        bool monitorHasMaximized = monitorsWithMaximizedWindow.contains(monitor);
        EnsureCornerWindows(info, monitorHasMaximized && !isFullscreen);

        bool nextSolidTarget = false;
        bool nextBgTarget = false;

        if (!isFullscreen) {
            nextSolidTarget = (g_settings.showMode == ShowMode::Always) || monitorHasMaximized;

            if (g_settings.enableWallpaperBlur) {
                if (g_settings.showMode == ShowMode::Always) {
                    nextBgTarget = hasOverlappingWindows;
                } else {
                    nextBgTarget = (!nextSolidTarget) && hasOverlappingWindows;
                }
            }
        }

        if (info.solidTargetVisible != nextSolidTarget) {
            info.solidTargetVisible = nextSolidTarget;
            info.solidAnimStartTick = now;
            info.solidAnimStartProgress = info.solidProgress;
        }

        if (info.bgTargetVisible != nextBgTarget) {
            info.bgTargetVisible = nextBgTarget;
            info.bgAnimStartTick = now;
            info.bgAnimStartProgress = info.bgProgress;
        }

        ApplyOverlayVisual(info);

        HWND hTaskbar = FindTaskbarForMonitor(monitor);
        HWND insertAfter = hTaskbar ? hTaskbar : HWND_TOPMOST;

        if (info.bgHwnd && g_settings.enableWallpaperBlur) {
            SetWindowPos(info.bgHwnd, insertAfter, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            SetWindowPos(info.hwnd, info.bgHwnd, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        } else {
            SetWindowPos(info.hwnd, insertAfter, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        }

        if (info.cornerLeftHwnd) {
            SetWindowPos(info.cornerLeftHwnd, HWND_TOPMOST, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        }
        if (info.cornerRightHwnd) {
            SetWindowPos(info.cornerRightHwnd, HWND_TOPMOST, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        }
    }
}

void CALLBACK StateTimerProc(HWND, UINT, UINT_PTR, DWORD) {
    UpdateTargets();
}

void CALLBACK AnimTimerProc(HWND, UINT, UINT_PTR, DWORD) {
    AnimateOverlays();
}

// -----------------------------------------------------------------------
// Mod entry points
// -----------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    LoadSettings();

    g_overlayBrush = CreateSolidBrush(g_settings.color);
    g_borderBrush = CreateSolidBrush(g_settings.borderColor);

    g_overlayClassName = MakeUniqueClassName(L"BlackTaskbarOverlay_" WH_MOD_ID);
    g_bgClassName = MakeUniqueClassName(L"BlackTaskbarOverlayBg_" WH_MOD_ID);
    g_timerOwnerClassName =
        MakeUniqueClassName(L"BlackTaskbarOverlayTimerOwner_" WH_MOD_ID);
    g_cornerClassName =
        MakeUniqueClassName(L"BlackTaskbarOverlayCorner_" WH_MOD_ID);

    UnregisterClass(g_overlayClassName.c_str(), GetModuleHandle(nullptr));
    UnregisterClass(g_bgClassName.c_str(), GetModuleHandle(nullptr));
    UnregisterClass(g_timerOwnerClassName.c_str(), GetModuleHandle(nullptr));
    UnregisterClass(g_cornerClassName.c_str(), GetModuleHandle(nullptr));

    WNDCLASS wc{};
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = g_overlayClassName.c_str();
    if (!RegisterClass(&wc)) {
        Wh_Log(L"Failed to register overlay window class");
        return FALSE;
    }

    WNDCLASS bgWc{};
    bgWc.lpfnWndProc = BgWndProc;
    bgWc.hInstance = GetModuleHandle(nullptr);
    bgWc.lpszClassName = g_bgClassName.c_str();
    if (!RegisterClass(&bgWc)) {
        Wh_Log(L"Failed to register bg window class");
        UnregisterClass(g_overlayClassName.c_str(), GetModuleHandle(nullptr));
        return FALSE;
    }

    WNDCLASS cornerWc{};
    cornerWc.lpfnWndProc = CornerWndProc;
    cornerWc.hInstance = GetModuleHandle(nullptr);
    cornerWc.lpszClassName = g_cornerClassName.c_str();
    if (!RegisterClass(&cornerWc)) {
        Wh_Log(L"Failed to register corner window class");
        UnregisterClass(g_bgClassName.c_str(), GetModuleHandle(nullptr));
        UnregisterClass(g_overlayClassName.c_str(), GetModuleHandle(nullptr));
        return FALSE;
    }

    WNDCLASS timerOwnerWc{};
    timerOwnerWc.lpfnWndProc = DefWindowProc;
    timerOwnerWc.hInstance = GetModuleHandle(nullptr);
    timerOwnerWc.lpszClassName = g_timerOwnerClassName.c_str();
    if (!RegisterClass(&timerOwnerWc)) {
        Wh_Log(L"Failed to register timer owner window class");
        UnregisterClass(g_cornerClassName.c_str(), GetModuleHandle(nullptr));
        UnregisterClass(g_bgClassName.c_str(), GetModuleHandle(nullptr));
        UnregisterClass(g_overlayClassName.c_str(), GetModuleHandle(nullptr));
        return FALSE;
    }

    g_timerOwnerWnd = CreateWindowEx(
        0, g_timerOwnerClassName.c_str(), L"", WS_POPUP, 0, 0, 0, 0, HWND_MESSAGE,
        nullptr, GetModuleHandle(nullptr), nullptr);
    if (!g_timerOwnerWnd) {
        Wh_Log(L"Failed to create timer owner window");
        UnregisterClass(g_timerOwnerClassName.c_str(), GetModuleHandle(nullptr));
        UnregisterClass(g_cornerClassName.c_str(), GetModuleHandle(nullptr));
        UnregisterClass(g_bgClassName.c_str(), GetModuleHandle(nullptr));
        UnregisterClass(g_overlayClassName.c_str(), GetModuleHandle(nullptr));
        return FALSE;
    }

    SetTimer(g_timerOwnerWnd, kStateTimerId, kStatePollIntervalMs,
              StateTimerProc);
    SetTimer(g_timerOwnerWnd, kAnimTimerId, kAnimIntervalMs, AnimTimerProc);

    UpdateTargets();

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    if (g_timerOwnerWnd) {
        KillTimer(g_timerOwnerWnd, kStateTimerId);
        KillTimer(g_timerOwnerWnd, kAnimTimerId);
        DestroyWindow(g_timerOwnerWnd);
        g_timerOwnerWnd = nullptr;
    }
    UnregisterClass(g_timerOwnerClassName.c_str(), GetModuleHandle(nullptr));
    g_timerOwnerClassName.clear();

    for (auto& [monitor, info] : g_overlays) {
        DestroyWindow(info.hwnd);
        if (info.bgHwnd) {
            DestroyWindow(info.bgHwnd);
        }
        if (info.cornerLeftHwnd) {
            DestroyWindow(info.cornerLeftHwnd);
        }
        if (info.cornerRightHwnd) {
            DestroyWindow(info.cornerRightHwnd);
        }
        ClearWallpaperBitmap(info);
    }
    g_overlays.clear();

    UnregisterClass(g_cornerClassName.c_str(), GetModuleHandle(nullptr));
    g_cornerClassName.clear();

    UnregisterClass(g_bgClassName.c_str(), GetModuleHandle(nullptr));
    g_bgClassName.clear();

    UnregisterClass(g_overlayClassName.c_str(), GetModuleHandle(nullptr));
    g_overlayClassName.clear();

    if (g_overlayBrush) {
        DeleteObject(g_overlayBrush);
        g_overlayBrush = nullptr;
    }
    if (g_borderBrush) {
        DeleteObject(g_borderBrush);
        g_borderBrush = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed");

    LoadSettings();

    HBRUSH oldOverlayBrush = g_overlayBrush;
    g_overlayBrush = CreateSolidBrush(g_settings.color);
    if (oldOverlayBrush) {
        DeleteObject(oldOverlayBrush);
    }

    HBRUSH oldBorderBrush = g_borderBrush;
    g_borderBrush = CreateSolidBrush(g_settings.borderColor);
    if (oldBorderBrush) {
        DeleteObject(oldBorderBrush);
    }

    for (auto& [monitor, info] : g_overlays) {
        RefreshScreenBlurBitmap(info);

        if (IsWindowVisible(info.hwnd)) {
            InvalidateRect(info.hwnd, nullptr, TRUE);
        }
        if (info.bgHwnd && IsWindowVisible(info.bgHwnd)) {
            InvalidateRect(info.bgHwnd, nullptr, TRUE);
        }
        if (info.cornerLeftHwnd && IsWindowVisible(info.cornerLeftHwnd)) {
            InvalidateRect(info.cornerLeftHwnd, nullptr, TRUE);
        }
        if (info.cornerRightHwnd && IsWindowVisible(info.cornerRightHwnd)) {
            InvalidateRect(info.cornerRightHwnd, nullptr, TRUE);
        }
    }

    UpdateTargets();
}
