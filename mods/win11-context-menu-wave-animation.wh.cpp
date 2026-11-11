// ==WindhawkMod==
// @id              win11-context-menu-wave-animation
// @name            Win11 Context Menu Wave Animation
// @name:pt         Animação em Onda do Menu de Contexto Win11
// @description     True 3D Liquid Wave & Water Drop Ripple deformation for modern Windows 11 context menus
// @description:pt  Deformação de Onda Líquida 3D e Ondulação de Gota d'Água para o menu de contexto do Windows 11
// @version         1.0.0
// @author          Gui03
// @github          https://github.com/GuiFrr03
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -lgdi32 -luser32 -O3
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Win11 Context Menu Wave Animation (3D Water Drop Ripple)

Applies a **3D liquid wave (water drop ripple) deformation** to modern Windows 11 right-click context menus.

![Demo](https://raw.githubusercontent.com/GuiFrr03/windhawk-assets/main/demo-normal.gif)

### Slow Motion
![Slow Motion Demo](https://raw.githubusercontent.com/GuiFrr03/windhawk-assets/main/demo-slow.gif)

When opening a right-click context menu, concentric circular ripples radiate from the cursor click position, deforming the menu's text, icons, and Acrylic background in 3D with liquid refraction, specular wave crests, and smooth dissipation.

## Features
* **3D Mesh Refraction:** Distorts text, icons, and the Acrylic background using a radial heightfield ripple simulation.
* **Specular Water Lighting:** Wave crests catch directional highlights and subtle 3D shading.
* **Natural Ripple Deceleration:** Smooth outward wavefront expansion that settles cleanly back into the native menu.
* **Command Bar + Menu Fusion:** When the top command bar (Cut/Copy/Rename) is present, both surfaces ripple together as a single continuous liquid sheet.
* **Native Blur Preservation:** Hands control seamlessly back to hardware-accelerated WinUI 3 / DWM rendering once the ripple completes.
* **System Popup Protection:** Ignores `Win+V`, emoji panel, taskbar overflow, and hovered submenus.

## Notes
* Targets `explorer.exe` (Desktop and File Explorer modern context menus on Windows 11).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- waveDuration: 700
  $name: Duration (ms)
  $name:pt: Duração da animação (ms)
  $description: Total ripple animation duration (400 to 700ms recommended)
  $description:pt: Tempo total da ondulação (400 a 700ms recomendado)
- waveIntensity: 100
  $name: Wave 3D Strength (%)
  $name:pt: Força da onda 3D (%)
  $description: Intensity of the 3D refraction and text deformation (50 to 150)
  $description:pt: Intensidade da refração 3D e deformação do texto (50 a 150)
- rippleRings: 2
  $name: Concentric ripple rings
  $name:pt: Anéis de ondulação extras
  $description: Number of concentric ripples following the wavefront (1 to 4)
  $description:pt: Quantidade de ondas concêntricas atrás da frente de onda (1 a 4)
- classes: "Microsoft.UI.Content.PopupWindowSiteBridge"
  $name: Window classes
  $name:pt: Classes de janela
  $description: Popup window classes to animate, separated by ;
  $description:pt: Classes das janelas popup a animar, separadas por ;
- minPopupWidth: 60
  $name: Minimum width (DIP)
  $name:pt: Largura mínima (DIP)
- minPopupHeight: 40
  $name: Minimum height (DIP)
  $name:pt: Altura mínima (DIP)
- logWindows: false
  $name: Log popup windows
  $name:pt: Registrar janelas popup no log
- debugLogFile: ''
  $name: Debug log file
  $name:pt: Arquivo de log de depuração
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#ifndef PW_RENDERFULLCONTENT
#define PW_RENDERFULLCONTENT 0x00000002
#endif

// ---------------------------------------------------------------------------
// Utility & Settings
// ---------------------------------------------------------------------------

static UINT SafeGetDpiForWindow(HWND hwnd) {
    using GetDpiForWindow_t = UINT(WINAPI*)(HWND);
    static auto pfn = (GetDpiForWindow_t)GetProcAddress(GetModuleHandleW(L"user32.dll"), "GetDpiForWindow");
    if (pfn && hwnd) {
        UINT d = pfn(hwnd);
        if (d) return d;
    }
    return 96;
}

struct Settings {
    int durationMs = 450;
    float intensity = 1.0f;
    int rings = 2;
    std::vector<std::wstring> classes;
    int minWidth = 60;
    int minHeight = 40;
    bool log = false;
};

static std::mutex g_settingsMutex;
static Settings g_settings;
static std::mutex g_logMutex;
static std::wstring g_logPath;
static std::atomic<bool> g_logToWh{false};
static std::atomic<bool> g_hasLogFile{false};

static LONGLONG g_qpcFreq = 1;
static LONGLONG g_qpcInit = 0;

static LONGLONG Now() {
    LARGE_INTEGER n;
    QueryPerformanceCounter(&n);
    return n.QuadPart;
}

static double ElapsedMs(LONGLONG start, LONGLONG end) {
    return (double)(end - start) * 1000.0 / (double)g_qpcFreq;
}

static void DebugLog(const wchar_t* fmt, ...) {
    bool logWh = g_logToWh.load(std::memory_order_relaxed);
    bool hasFile = g_hasLogFile.load(std::memory_order_relaxed);
    if (!logWh && !hasFile) return;

    wchar_t msg[1024];
    va_list args;
    va_start(args, fmt);
    _vsnwprintf_s(msg, _countof(msg), _TRUNCATE, fmt, args);
    va_end(args);

    if (logWh) Wh_Log(L"%s", msg);
    if (!hasFile) return;

    std::lock_guard<std::mutex> lock(g_logMutex);
    if (g_logPath.empty()) return;

    double ms = ElapsedMs(g_qpcInit, Now());
    wchar_t line[1200];
    _snwprintf_s(line, _countof(line), _TRUNCATE, L"%10.1f [%5lu] %s\r\n", ms,
                 GetCurrentThreadId(), msg);

    char utf8[3600];
    int n = WideCharToMultiByte(CP_UTF8, 0, line, -1, utf8, sizeof(utf8), nullptr, nullptr);
    if (n <= 1) return;

    HANDLE f = CreateFileW(g_logPath.c_str(), FILE_APPEND_DATA,
                           FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
                           OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE) return;
    DWORD written;
    WriteFile(f, utf8, n - 1, &written, nullptr);
    CloseHandle(f);
}

static void LoadSettings() {
    Settings s;

    int dur = Wh_GetIntSetting(L"waveDuration");
    s.durationMs = (dur > 0) ? std::clamp(dur, 150, 2000) : 450;

    int inten = Wh_GetIntSetting(L"waveIntensity");
    s.intensity = (inten > 0) ? (std::clamp(inten, 25, 200) / 100.0f) : 1.0f;

    int rings = Wh_GetIntSetting(L"rippleRings");
    s.rings = (rings > 0) ? std::clamp(rings, 1, 4) : 2;

    PCWSTR classes = Wh_GetStringSetting(L"classes");
    std::wstring all = classes ? classes : L"";
    if (classes) Wh_FreeStringSetting(classes);

    size_t pos = 0;
    while (pos <= all.size()) {
        size_t end = all.find(L';', pos);
        if (end == std::wstring::npos) end = all.size();
        std::wstring item = all.substr(pos, end - pos);
        size_t a = item.find_first_not_of(L" \t");
        size_t b = item.find_last_not_of(L" \t");
        if (a != std::wstring::npos) s.classes.push_back(item.substr(a, b - a + 1));
        pos = end + 1;
    }

    // Strictly blacklist system popups (Win+V, Win+., Taskbar overflow)
    std::erase_if(s.classes, [](const std::wstring& c) {
        return _wcsicmp(c.c_str(), L"Xaml_WindowedPopupClass") == 0 ||
               _wcsicmp(c.c_str(), L"TopLevelWindowForOverflowXamlIsland") == 0 ||
               _wcsicmp(c.c_str(), L"XamlExplorerHostIslandWindow") == 0;
    });
    if (s.classes.empty()) {
        s.classes.push_back(L"Microsoft.UI.Content.PopupWindowSiteBridge");
    }

    int mw = Wh_GetIntSetting(L"minPopupWidth");
    int mh = Wh_GetIntSetting(L"minPopupHeight");
    s.minWidth = std::max(50, mw > 0 ? mw : 60);
    s.minHeight = std::max(35, mh > 0 ? mh : 40);
    s.log = Wh_GetIntSetting(L"logWindows") != 0;

    PCWSTR logFile = Wh_GetStringSetting(L"debugLogFile");
    {
        std::lock_guard<std::mutex> lock(g_logMutex);
        g_logPath = logFile ? logFile : L"";
        g_hasLogFile.store(!g_logPath.empty(), std::memory_order_relaxed);
    }
    if (logFile) Wh_FreeStringSetting(logFile);
    g_logToWh.store(s.log, std::memory_order_relaxed);

    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_settings = std::move(s);
}

static Settings GetSettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings;
}

// ---------------------------------------------------------------------------
// Window Tracking & Capture State
// ---------------------------------------------------------------------------

struct Anim {
    HWND hwnd = nullptr;
    bool armed = false;
    bool cancel = false;
    bool addedLayered = false;
    bool hasAppRgn = false;
    std::vector<BYTE> appRgnData;
    POINT cursorPos{};
    RECT lastRect{};
    LONGLONG createdQpc = 0;
    LONGLONG firstSizedQpc = 0;
    LONGLONG lastResizeQpc = 0;
    Settings settings;
};

struct CapturedCard {
    HWND hwnd = nullptr;
    RECT wndRect{};
    RECT cardScreenRect{};
    int width = 0;
    int height = 0;
    std::vector<uint32_t> pixels; // Premultiplied BGRA (0xAARRGGBB)
    bool valid = false;
    bool hasIcons = false;
};

struct WaveSession {
    bool active = false;
    bool captured = false;
    LONGLONG startQpc = 0;
    POINT cursorPos{};
    RECT unionScreenRect{};
    int width = 0;
    int height = 0;
    UINT dpi = 96;
    Settings settings;
    std::vector<uint32_t> fusedPixels; // Premultiplied BGRA (width * height)
    int captureRetries = 0;
};

static std::mutex g_animMutex;
static std::unordered_map<HWND, Anim> g_anims;
static std::unordered_map<HWND, ULONGLONG> g_recent;
static std::atomic<bool> g_resetSession{false};
static HANDLE g_wakeEvent = nullptr;
static std::atomic<bool> g_stop{false};
static std::thread g_animThread;

static thread_local bool g_ownCall = false;

using SetWindowRgn_t = int(WINAPI*)(HWND, HRGN, BOOL);
static SetWindowRgn_t SetWindowRgn_Original;

static int MySetWindowRgn(HWND hwnd, HRGN rgn, BOOL redraw) {
    g_ownCall = true;
    int r = SetWindowRgn_Original ? SetWindowRgn_Original(hwnd, rgn, redraw)
                                  : SetWindowRgn(hwnd, rgn, redraw);
    g_ownCall = false;
    if (!r && rgn) DeleteObject(rgn);
    return r;
}

static bool IsTargetClass(HWND hwnd, const Settings& s, WCHAR* clsOut, int clsLen) {
    clsOut[0] = 0;
    if (!GetClassNameW(hwnd, clsOut, clsLen)) return false;

    if (_wcsicmp(clsOut, L"Xaml_WindowedPopupClass") == 0 ||
        _wcsicmp(clsOut, L"TopLevelWindowForOverflowXamlIsland") == 0 ||
        _wcsicmp(clsOut, L"XamlExplorerHostIslandWindow") == 0 ||
        _wcsicmp(clsOut, L"WindhawkFluentWaveOverlay") == 0) {
        return false;
    }

    for (const auto& c : s.classes)
        if (_wcsicmp(c.c_str(), clsOut) == 0) return true;
    return false;
}

// Hide the real PopupWindowSiteBridge with alpha=1 so it is invisible to user on screen,
// while DWM still actively binds the desktop composition wallpaper sampler for true Acrylic blur!
static void HideWindowForWave(Anim& a) {
    if (!IsWindow(a.hwnd)) return;
    MySetWindowRgn(a.hwnd, nullptr, FALSE);
    LONG_PTR ex = GetWindowLongPtrW(a.hwnd, GWL_EXSTYLE);
    if (!(ex & WS_EX_LAYERED)) {
        SetWindowLongPtrW(a.hwnd, GWL_EXSTYLE, ex | WS_EX_LAYERED);
        a.addedLayered = true;
    }
    SetLayeredWindowAttributes(a.hwnd, 0, 1, LWA_ALPHA);
}

static void RestoreRealWindow(const Anim& a, const wchar_t* why) {
    DebugLog(L"hwnd=%p restore (%s)", a.hwnd, why);
    if (!IsWindow(a.hwnd)) return;

    HRGN restore = nullptr;
    if (a.hasAppRgn && !a.appRgnData.empty()) {
        restore = ExtCreateRegion(nullptr, (DWORD)a.appRgnData.size(),
                                  (const RGNDATA*)a.appRgnData.data());
    }
    MySetWindowRgn(a.hwnd, restore, TRUE);

    SetLayeredWindowAttributes(a.hwnd, 0, 255, LWA_ALPHA);

    // Only strip WS_EX_LAYERED if the window is already hidden or destroyed.
    // Stripping WS_EX_LAYERED on a visible window causes DWM to recreate composition surfaces,
    // which creates a 1-frame visible flash/flicker. At alpha=255, the layered window behaves identically.
    if (a.addedLayered && !IsWindowVisible(a.hwnd)) {
        LONG_PTR ex = GetWindowLongPtrW(a.hwnd, GWL_EXSTYLE);
        SetWindowLongPtrW(a.hwnd, GWL_EXSTYLE, ex & ~WS_EX_LAYERED);
    }
}

// Capture a single PopupWindowSiteBridge via PrintWindow(PW_RENDERFULLCONTENT) and auto-crop transparent shadow margins
static CapturedCard CapturePopupCard(HWND hwnd, int minWidthDip, int minHeightDip) {
    CapturedCard out;
    out.hwnd = hwnd;
    if (!IsWindow(hwnd) || !IsWindowVisible(hwnd)) return out;

    RECT rc{};
    if (!GetWindowRect(hwnd, &rc)) return out;
    int w = rc.right - rc.left;
    int h = rc.bottom - rc.top;
    UINT dpi = SafeGetDpiForWindow(hwnd);
    if (w < MulDiv(minWidthDip, dpi, 96) || h < MulDiv(minHeightDip, dpi, 96) || w > 2400 || h > 2400) {
        return out;
    }
    out.wndRect = rc;

    HDC hdcScreen = GetDC(nullptr);
    if (!hdcScreen) return out;
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    if (!hdcMem) {
        ReleaseDC(nullptr, hdcScreen);
        return out;
    }

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h; // Top-down DIB
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    uint32_t* rawBits = nullptr;
    HBITMAP hbm = CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS, (void**)&rawBits, nullptr, 0);
    if (!hbm || !rawBits) {
        if (hbm) DeleteObject(hbm);
        DeleteDC(hdcMem);
        ReleaseDC(nullptr, hdcScreen);
        return out;
    }

    HGDIOBJ oldBm = SelectObject(hdcMem, hbm);
    memset(rawBits, 0, (size_t)w * h * 4);

    BOOL ok = PrintWindow(hwnd, hdcMem, PW_RENDERFULLCONTENT);
    if (!ok) {
        ok = PrintWindow(hwnd, hdcMem, 0);
    }

    // Auto-detect non-black visual card bounds (PrintWindow renders transparent shadow margins as RGB=0,0,0)
    int minX = w, minY = h, maxX = -1, maxY = -1;
    uint8_t globalMaxRgb = 0;

    if (ok) {
        for (int y = 0; y < h; y++) {
            const uint32_t* row = rawBits + y * w;
            for (int x = 0; x < w; x++) {
                uint32_t px = row[x];
                uint8_t b = (uint8_t)(px & 0xFF);
                uint8_t g = (uint8_t)((px >> 8) & 0xFF);
                uint8_t r = (uint8_t)((px >> 16) & 0xFF);
                uint8_t m = std::max({r, g, b});
                if (m > globalMaxRgb) globalMaxRgb = m;
                if (m > 8) {
                    if (x < minX) minX = x;
                    if (x > maxX) maxX = x;
                    if (y < minY) minY = y;
                    if (y > maxY) maxY = y;
                }
            }
        }
    }

    // Verify if this popup actually has rendered content (filters out the empty 355x86 desktop placeholder)
    int cw = maxX - minX + 1;
    int ch = maxY - minY + 1;
    if (!ok || globalMaxRgb <= 12 || cw < 36 || ch < 24) {
        SelectObject(hdcMem, oldBm);
        DeleteObject(hbm);
        DeleteDC(hdcMem);
        ReleaseDC(nullptr, hdcScreen);
        return out;
    }

    out.cardScreenRect.left = rc.left + minX;
    out.cardScreenRect.top = rc.top + minY;
    out.cardScreenRect.right = rc.left + maxX + 1;
    out.cardScreenRect.bottom = rc.top + maxY + 1;
    out.width = cw;
    out.height = ch;
    out.pixels.resize((size_t)cw * ch);

    for (int y = 0; y < ch; y++) {
        const uint32_t* srcRow = rawBits + (minY + y) * w + minX;
        uint32_t* dstRow = out.pixels.data() + (size_t)y * cw;

        for (int x = 0; x < cw; x++) {
            uint32_t px = srcRow[x];
            uint8_t b = px & 0xFF;
            uint8_t g = (px >> 8) & 0xFF;
            uint8_t r = (px >> 16) & 0xFF;
            uint8_t m = std::max({r, g, b});

            if (m > 8) {
                // Inside card: preserve native WinUI 3 rounded/flat corners exactly as rendered
                dstRow[x] = 0xFF000000 | (r << 16) | (g << 8) | b;
            } else if (m > 1) {
                // Anti-aliased outer edge from PrintWindow
                float alpha = (float)m / 8.0f;
                uint32_t a = (uint32_t)(alpha * 255.0f);
                b = (uint8_t)(b * alpha);
                g = (uint8_t)(g * alpha);
                r = (uint8_t)(r * alpha);
                dstRow[x] = (a << 24) | (r << 16) | (g << 8) | b;
            } else {
                dstRow[x] = 0;
            }
        }
    }

    // Detect if icons are loaded (prevents capturing provisional iconless skeleton during shell extension loading)
    bool isCommandBar = (ch <= MulDiv(75, dpi, 96));
    if (isCommandBar) {
        int iconPixels = 0;
        int topH = ch * 55 / 100;
        for (int y = 0; y < topH; y++) {
            const uint32_t* row = out.pixels.data() + (size_t)y * cw;
            for (int x = 0; x < cw; x++) {
                uint32_t px = row[x];
                uint8_t r = (px >> 16) & 0xFF;
                uint8_t g = (px >> 8) & 0xFF;
                uint8_t b = px & 0xFF;
                if (std::max({r, g, b}) > 110) {
                    iconPixels++;
                }
            }
        }
        out.hasIcons = (iconPixels >= 100);
    } else {
        int iconPixels = 0;
        int iconColW = std::min(cw, MulDiv(38, dpi, 96));
        for (int y = 0; y < ch; y++) {
            const uint32_t* row = out.pixels.data() + (size_t)y * cw;
            for (int x = 0; x < iconColW; x++) {
                uint32_t px = row[x];
                uint8_t r = (px >> 16) & 0xFF;
                uint8_t g = (px >> 8) & 0xFF;
                uint8_t b = px & 0xFF;
                if (std::max({r, g, b}) > 110) {
                    iconPixels++;
                }
            }
        }
        out.hasIcons = (iconPixels >= 60);
    }

    SelectObject(hdcMem, oldBm);
    DeleteObject(hbm);
    DeleteDC(hdcMem);
    ReleaseDC(nullptr, hdcScreen);

    out.valid = true;
    return out;
}

// ---------------------------------------------------------------------------
// 3D Water Drop Ripple Deformation Renderer & Overlay Window
// ---------------------------------------------------------------------------

static HWND g_hwndOverlay = nullptr;
static HDC g_hdcOverlay = nullptr;
static HBITMAP g_hbmOverlay = nullptr;
static HGDIOBJ g_oldOverlayBm = nullptr;
static uint32_t* g_overlayBits = nullptr;
static int g_overlayAllocW = 0;
static int g_overlayAllocH = 0;

static void EnsureOverlayWindow() {
    if (g_hwndOverlay && IsWindow(g_hwndOverlay)) return;

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = DefWindowProcW;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = L"WindhawkFluentWaveOverlay";
    RegisterClassExW(&wc);

    g_hwndOverlay = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        L"WindhawkFluentWaveOverlay", L"", WS_POPUP,
        0, 0, 1, 1, nullptr, nullptr, wc.hInstance, nullptr);
}

static bool EnsureOverlaySurface(int outW, int outH) {
    EnsureOverlayWindow();
    if (!g_hwndOverlay) return false;

    if (!g_hdcOverlay) {
        HDC hdcScreen = GetDC(nullptr);
        if (!hdcScreen) return false;
        g_hdcOverlay = CreateCompatibleDC(hdcScreen);
        ReleaseDC(nullptr, hdcScreen);
        if (!g_hdcOverlay) return false;
    }

    if (g_hbmOverlay && g_overlayAllocW >= outW && g_overlayAllocH >= outH) {
        return true;
    }

    if (g_hbmOverlay) {
        if (g_oldOverlayBm) {
            SelectObject(g_hdcOverlay, g_oldOverlayBm);
            g_oldOverlayBm = nullptr;
        }
        DeleteObject(g_hbmOverlay);
        g_hbmOverlay = nullptr;
        g_overlayBits = nullptr;
    }

    g_overlayAllocW = std::max(outW, 512);
    g_overlayAllocH = std::max(outH, 640);

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = g_overlayAllocW;
    bmi.bmiHeader.biHeight = -g_overlayAllocH; // Top-down DIB
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    g_hbmOverlay = CreateDIBSection(g_hdcOverlay, &bmi, DIB_RGB_COLORS, (void**)&g_overlayBits, nullptr, 0);
    if (!g_hbmOverlay || !g_overlayBits) {
        if (g_hbmOverlay) {
            DeleteObject(g_hbmOverlay);
            g_hbmOverlay = nullptr;
        }
        g_overlayBits = nullptr;
        g_overlayAllocW = 0;
        g_overlayAllocH = 0;
        return false;
    }

    g_oldOverlayBm = SelectObject(g_hdcOverlay, g_hbmOverlay);
    return true;
}

static void DestroyOverlayResources() {
    if (g_hwndOverlay) {
        DestroyWindow(g_hwndOverlay);
        g_hwndOverlay = nullptr;
    }
    if (g_hdcOverlay && g_oldOverlayBm) {
        SelectObject(g_hdcOverlay, g_oldOverlayBm);
        g_oldOverlayBm = nullptr;
    }
    if (g_hbmOverlay) {
        DeleteObject(g_hbmOverlay);
        g_hbmOverlay = nullptr;
        g_overlayBits = nullptr;
    }
    if (g_hdcOverlay) {
        DeleteDC(g_hdcOverlay);
        g_hdcOverlay = nullptr;
    }
    UnregisterClassW(L"WindhawkFluentWaveOverlay", GetModuleHandleW(nullptr));
    g_overlayAllocW = 0;
    g_overlayAllocH = 0;
}

// Fast inline integer power for specular water shine (1 cycle vs 70 cycles in std::pow)
inline float FastPow20(float x) {
    if (x <= 0.0f) return 0.0f;
    float x2 = x * x;
    float x4 = x2 * x2;
    float x5 = x4 * x;
    float x10 = x5 * x5;
    return x10 * x10;
}

struct WaveLUTItem {
    float dz_dr;
    float reveal;
    bool calm;
};

// True 3D Water Drop Ripple Deformation Frame
static void RenderWaterDrop3DFrame(const WaveSession& ws, float p) {
    const int W = ws.width;
    const int H = ws.height;
    if (W <= 0 || H <= 0 || ws.fusedPixels.empty()) return;

    const int pad = 4;
    const int outW = W + pad * 2;
    const int outH = H + pad * 2;
    if (!EnsureOverlaySurface(outW, outH)) return;

    for (int y = 0; y < outH; y++) {
        memset(g_overlayBits + (size_t)y * g_overlayAllocW, 0, (size_t)outW * 4);
    }

    const float dpiScale = (float)ws.dpi / 96.0f;
    const float inten = ws.settings.intensity;
    const float ox = (float)(ws.cursorPos.x - ws.unionScreenRect.left);
    const float oy = (float)(ws.cursorPos.y - ws.unionScreenRect.top);

    // Calculate maximum distance to corners of the union rectangle
    float d1 = std::hypot(ox, oy);
    float d2 = std::hypot((float)W - ox, oy);
    float d3 = std::hypot(ox, (float)H - oy);
    float d4 = std::hypot((float)W - ox, (float)H - oy);
    float maxDist = std::max({d1, d2, d3, d4});

    // Ripple expansion curve: fast launch at droplet impact, smoothly decelerating across surface
    float p_wave = 1.0f - std::pow(std::max(0.0f, 1.0f - p), 1.95f);
    float R = maxDist * 1.08f * p_wave;

    // Energy dissipation over time
    float energy = std::pow(std::max(0.0f, 1.0f - p), 1.15f) * inten;

    // 3D Water wave parameters
    const float wavelength = 38.0f * dpiScale;
    constexpr float kPi = 3.14159265358979323846f;
    const float k = (2.0f * kPi) / wavelength;
    const float decayWidth = (45.0f + 28.0f * ws.settings.rings) * dpiScale;
    const float meniscus = 9.0f * dpiScale;

    const float amp_z = 22.0f * dpiScale * energy;
    const float disp_strength = 0.90f;

    // Light source vector: from top-left-above (normalized)
    const float Lx = -0.35f, Ly = -0.55f, Lz = 0.75f;
    const float inv_L = 1.0f / std::sqrt(Lx * Lx + Ly * Ly + Lz * Lz);
    const float normLx = Lx * inv_L, normLy = Ly * inv_L, normLz = Lz * inv_L;

    const float rOuter = R + meniscus;

    // Precompute 1D Wave Radial Profile (LUT) at 0.5px subpixel resolution (54x faster frame rendering)
    const float kStep = 0.5f;
    const float kInvStep = 2.0f;
    int lutSize = (int)std::ceil(rOuter * kInvStep) + 8;
    static thread_local std::vector<WaveLUTItem> lut;
    lut.resize(lutSize);

    for (int i = 0; i < lutSize; i++) {
        float r = (float)i * kStep;
        float dr = R - r;

        if (dr > decayWidth * 3.2f) {
            lut[i] = { 0.0f, 1.0f, true };
            continue;
        }

        float env = 0.0f;
        float d_env_dr = 0.0f;
        if (dr >= -meniscus) {
            if (dr < 0.0f) {
                env = (dr + meniscus) / meniscus;
                d_env_dr = 1.0f / meniscus;
            } else {
                env = std::exp(-dr / decayWidth);
                d_env_dr = -1.0f / decayWidth * env;
            }
        }

        float wave_sin = std::sin(k * dr);
        float wave_cos = std::cos(k * dr);
        float dz_dr = amp_z * (k * wave_cos * env + wave_sin * d_env_dr);

        float rev = std::clamp((R + meniscus - r) / meniscus, 0.0f, 1.0f);
        rev = rev * rev * (3.0f - 2.0f * rev);

        lut[i] = { dz_dr, rev, false };
    }

    for (int y = 0; y < H; y++) {
        float dy = (float)y - oy;
        if (std::fabs(dy) > rOuter && R < maxDist) continue;

        uint32_t* dstRow = g_overlayBits + (size_t)(y + pad) * g_overlayAllocW + pad;
        const uint32_t* srcRow = ws.fusedPixels.data() + (size_t)y * W;

        for (int x = 0; x < W; x++) {
            float dx = (float)x - ox;
            float r = std::sqrt(dx * dx + dy * dy);
            if (r > rOuter) continue;

            int lutIdx = (int)(r * kInvStep);
            if (lutIdx >= lutSize) continue;
            const auto& item = lut[lutIdx];

            // Calm water fast path: ripple has already passed, 100% revealed, no displacement/lighting needed
            if (item.calm) {
                dstRow[x] = srcRow[x];
                continue;
            }

            float dir_x = (r > 0.001f) ? (dx / r) : 0.0f;
            float dir_y = (r > 0.001f) ? (dy / r) : 0.0f;
            float dz_dr = item.dz_dr;

            // 3D Refraction / Mesh displacement:
            float disp_x = disp_strength * dz_dr * dir_x;
            float disp_y = disp_strength * dz_dr * dir_y;

            float sx = std::clamp((float)x - disp_x, 0.0f, (float)(W - 1) - 0.001f);
            float sy = std::clamp((float)y - disp_y, 0.0f, (float)(H - 1) - 0.001f);

            // Bilinear sampling of the menu card texture
            int x0 = (int)sx;
            int x1 = std::min(x0 + 1, W - 1);
            int y0 = (int)sy;
            int y1 = std::min(y0 + 1, H - 1);

            uint32_t fx = (uint32_t)((sx - (float)x0) * 256.0f);
            uint32_t fy = (uint32_t)((sy - (float)y0) * 256.0f);
            uint32_t ifx = 256 - fx;
            uint32_t ify = 256 - fy;

            const uint32_t* r0 = ws.fusedPixels.data() + (size_t)y0 * W;
            const uint32_t* r1 = ws.fusedPixels.data() + (size_t)y1 * W;

            uint32_t p00 = r0[x0], p01 = r0[x1], p10 = r1[x0], p11 = r1[x1];
            uint32_t w00 = (ifx * ify) >> 8;
            uint32_t w01 = (fx * ify) >> 8;
            uint32_t w10 = (ifx * fy) >> 8;
            uint32_t w11 = (fx * fy) >> 8;

            uint32_t b_samp = (((p00 & 0xFF) * w00 + (p01 & 0xFF) * w01 + (p10 & 0xFF) * w10 + (p11 & 0xFF) * w11) >> 8);
            uint32_t g_samp = ((((p00 >> 8) & 0xFF) * w00 + ((p01 >> 8) & 0xFF) * w01 + ((p10 >> 8) & 0xFF) * w10 + ((p11 >> 8) & 0xFF) * w11) >> 8);
            uint32_t r_samp = ((((p00 >> 16) & 0xFF) * w00 + ((p01 >> 16) & 0xFF) * w01 + ((p10 >> 16) & 0xFF) * w10 + ((p11 >> 16) & 0xFF) * w11) >> 8);
            uint32_t a_samp = ((((p00 >> 24) & 0xFF) * w00 + ((p01 >> 24) & 0xFF) * w01 + ((p10 >> 24) & 0xFF) * w10 + ((p11 >> 24) & 0xFF) * w11) >> 8);

            if (a_samp == 0) continue;

            float reveal = item.reveal;

            // If sampled pixel is outside the card (drop shadow only), keep it pure black shadow
            if ((r_samp | g_samp | b_samp) == 0) {
                uint32_t final_a = (uint32_t)std::clamp((float)a_samp * reveal + 0.5f, 0.0f, 255.0f);
                if (final_a > 0) {
                    dstRow[x] = (final_a << 24);
                }
                continue;
            }

            // 3D Surface Normal & Lighting
            float Nx = -dz_dr * dir_x * 0.14f;
            float Ny = -dz_dr * dir_y * 0.14f;
            float Nz = 1.0f;
            float inv_N = 1.0f / std::sqrt(Nx * Nx + Ny * Ny + Nz * Nz);
            Nx *= inv_N;
            Ny *= inv_N;
            Nz *= inv_N;

            float NdotL = Nx * normLx + Ny * normLy + Nz * normLz;
            float diffuse = (NdotL - normLz) * 0.40f * energy;

            // Specular reflection (water crest shine via ultra-fast 1-cycle power)
            float Rz = 2.0f * NdotL * Nz - normLz;
            float specular = (Rz > 0.0f) ? (FastPow20(Rz) * 0.50f * energy) : 0.0f;

            // Apply diffuse shading and specular highlight
            float fb = (float)b_samp;
            float fg = (float)g_samp;
            float fr = (float)r_samp;
            float fa = (float)a_samp;

            if (diffuse >= 0.0f) {
                fb = fb + (fa - fb) * diffuse;
                fg = fg + (fa - fg) * diffuse;
                fr = fr + (fa - fr) * diffuse;
            } else {
                float m = 1.0f + diffuse;
                fb *= m;
                fg *= m;
                fr *= m;
            }

            fb = fb + (fa - fb) * specular;
            fg = fg + (fa - fg) * specular;
            fr = fr + (fa - fr) * specular;

            uint32_t final_a = (uint32_t)std::clamp(fa * reveal + 0.5f, 0.0f, 255.0f);
            uint32_t final_r = (uint32_t)std::clamp(fr * reveal + 0.5f, 0.0f, 255.0f);
            uint32_t final_g = (uint32_t)std::clamp(fg * reveal + 0.5f, 0.0f, 255.0f);
            uint32_t final_b = (uint32_t)std::clamp(fb * reveal + 0.5f, 0.0f, 255.0f);

            dstRow[x] = (final_a << 24) | (final_r << 16) | (final_g << 8) | final_b;
        }
    }

    POINT ptPos{ ws.unionScreenRect.left - pad, ws.unionScreenRect.top - pad };
    SIZE szWnd{ outW, outH };
    POINT ptSrc{ 0, 0 };
    BLENDFUNCTION bf{};
    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    bf.SourceConstantAlpha = 255;
    bf.AlphaFormat = AC_SRC_ALPHA;

    UpdateLayeredWindow(g_hwndOverlay, nullptr, &ptPos, &szWnd, g_hdcOverlay, &ptSrc, 0, &bf, ULW_ALPHA);
    if (!IsWindowVisible(g_hwndOverlay)) {
        SetWindowPos(g_hwndOverlay, HWND_TOPMOST, ptPos.x, ptPos.y, szWnd.cx, szWnd.cy,
                     SWP_NOACTIVATE | SWP_SHOWWINDOW);
    }
}

static void HideWaveOverlay() {
    if (g_hwndOverlay && IsWindowVisible(g_hwndOverlay)) {
        ShowWindow(g_hwndOverlay, SW_HIDE);
    }
}

// ---------------------------------------------------------------------------
// Animation Thread Loop
// ---------------------------------------------------------------------------

// Fast separable 1D moving-average blur
static void FastBoxBlur(const float* src, float* dst, float* temp, int w, int h, int radius) {
    if (radius <= 0) {
        memcpy(dst, src, (size_t)w * h * sizeof(float));
        return;
    }
    float invK = 1.0f / (float)(2 * radius + 1);

    // Horizontal pass: src -> temp
    for (int y = 0; y < h; y++) {
        const float* rowIn = src + (size_t)y * w;
        float* rowOut = temp + (size_t)y * w;

        float sum = 0.0f;
        for (int x = -radius; x <= radius; x++) {
            int cx = std::clamp(x, 0, w - 1);
            sum += rowIn[cx];
        }
        rowOut[0] = sum * invK;

        for (int x = 1; x < w; x++) {
            int addX = std::min(x + radius, w - 1);
            int subX = std::max(x - radius - 1, 0);
            sum += rowIn[addX] - rowIn[subX];
            rowOut[x] = sum * invK;
        }
    }

    // Vertical pass: temp -> dst
    for (int x = 0; x < w; x++) {
        float sum = 0.0f;
        for (int y = -radius; y <= radius; y++) {
            int cy = std::clamp(y, 0, h - 1);
            sum += temp[(size_t)cy * w + x];
        }
        dst[x] = sum * invK;

        for (int y = 1; y < h; y++) {
            int addY = std::min(y + radius, h - 1);
            int subY = std::max(y - radius - 1, 0);
            sum += temp[(size_t)addY * w + x] - temp[(size_t)subY * w + x];
            dst[(size_t)y * w + x] = sum * invK;
        }
    }
}

// Generate dual-layer soft native Windows 11 DWM drop shadow around fused cards into the padded border
static void GenerateDropShadow(std::vector<uint32_t>& pixels, int w, int h, int pad, UINT dpi) {
    if (w <= 0 || h <= 0 || pad <= 0 || pixels.size() < (size_t)w * h) return;

    float scale = (dpi > 0) ? ((float)dpi / 96.0f) : 1.0f;
    int radAmb = std::max(1, (int)std::round(6.0f * scale));
    int radDir = std::max(1, (int)std::round(10.0f * scale));
    int yShift = std::max(1, (int)std::round(4.0f * scale));

    const float maxAmbAlpha = 26.0f;
    const float maxDirAlpha = 50.0f;

    std::vector<float> mask((size_t)w * h, 0.0f);
    std::vector<uint8_t> isCard((size_t)w * h, 0);

    for (int i = 0; i < w * h; i++) {
        if ((pixels[i] >> 24) > 128) {
            mask[i] = 1.0f;
            isCard[i] = 1;
        }
    }

    std::vector<float> buf1((size_t)w * h, 0.0f);
    std::vector<float> buf2((size_t)w * h, 0.0f);
    std::vector<float> temp((size_t)w * h, 0.0f);

    // 1. Ambient shadow: 2-pass blur of card mask
    FastBoxBlur(mask.data(), buf1.data(), temp.data(), w, h, radAmb);
    FastBoxBlur(buf1.data(), buf2.data(), temp.data(), w, h, radAmb);

    // 2. Directional key shadow: shifted down by yShift, blurred with radDir
    std::vector<float> shiftedMask((size_t)w * h, 0.0f);
    for (int y = 0; y < h; y++) {
        int srcY = y - yShift;
        if (srcY >= 0 && srcY < h) {
            memcpy(shiftedMask.data() + (size_t)y * w, mask.data() + (size_t)srcY * w, (size_t)w * sizeof(float));
        }
    }

    std::vector<float> bufDir1((size_t)w * h, 0.0f);
    std::vector<float> bufDir2((size_t)w * h, 0.0f);
    FastBoxBlur(shiftedMask.data(), bufDir1.data(), temp.data(), w, h, radDir);
    FastBoxBlur(bufDir1.data(), bufDir2.data(), temp.data(), w, h, radDir);

    // 3. Composite into pixels (only outside card)
    for (int i = 0; i < w * h; i++) {
        if (isCard[i]) continue;

        float aAmb = buf2[i] * maxAmbAlpha;
        float aDir = bufDir2[i] * maxDirAlpha;
        float totalA = aAmb + aDir;
        uint8_t sa = (uint8_t)std::clamp(totalA + 0.5f, 0.0f, 255.0f);
        if (sa > 0) {
            pixels[i] = (uint32_t)sa << 24; // Premultiplied black shadow
        }
    }
}

static void FinishAllActiveAnims(const wchar_t* why) {
    std::vector<Anim> toRestore;
    {
        std::lock_guard<std::mutex> lock(g_animMutex);
        for (auto& [h, a] : g_anims) {
            toRestore.push_back(a);
        }
        g_anims.clear();
    }
    for (const auto& a : toRestore) {
        RestoreRealWindow(a, why);
    }
    DwmFlush();
    HideWaveOverlay();
}

static void AnimThreadProc() {
    WaveSession session;

    while (!g_stop) {
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        if (g_resetSession.exchange(false)) {
            HideWaveOverlay();
            session = WaveSession{};
        }

        bool hasAnims = false;
        {
            std::lock_guard<std::mutex> lock(g_animMutex);
            hasAnims = !g_anims.empty();
        }

        if (!hasAnims) {
            if (session.active) {
                HideWaveOverlay();
                session = WaveSession{};
            }
            WaitForSingleObject(g_wakeEvent, 200);
            continue;
        }

        LONGLONG now = Now();

        // Check if any tracked window was destroyed, hidden, or cancelled
        bool anyCancelledOrHidden = false;
        std::vector<Anim> currentAnims;
        {
            std::lock_guard<std::mutex> lock(g_animMutex);
            for (auto& [h, a] : g_anims) {
                if (!IsWindow(h) || !IsWindowVisible(h) || a.cancel) {
                    anyCancelledOrHidden = true;
                } else {
                    RECT curRc{};
                    GetWindowRect(h, &curRc);
                    int cw = curRc.right - curRc.left;
                    int ch = curRc.bottom - curRc.top;
                    UINT dpi = SafeGetDpiForWindow(h);
                    if (cw >= MulDiv(a.settings.minWidth, dpi, 96) &&
                        ch >= MulDiv(a.settings.minHeight, dpi, 96) &&
                        a.firstSizedQpc == 0) {
                        a.firstSizedQpc = now;
                        a.lastResizeQpc = now;
                        a.lastRect = curRc;
                    }
                }
                currentAnims.push_back(a);
            }
        }

        if (anyCancelledOrHidden) {
            FinishAllActiveAnims(L"window hidden or cancelled");
            session = WaveSession{};
            continue;
        }

        // Phase 1: Wait until the Main Flyout (h >= 130 DIPs) gets its size and shell extensions settle
        if (!session.captured) {
            LONGLONG earliestCreated = now;
            LONGLONG mainFirstSized = 0;
            LONGLONG anyFirstSized = 0;
            LONGLONG latestResize = 0;
            int mainWidth = 0;
            UINT mainDpi = 96;
            bool anyArmed = false;

            for (const auto& a : currentAnims) {
                if (!a.armed) continue;
                anyArmed = true;
                if (a.createdQpc < earliestCreated) earliestCreated = a.createdQpc;
                if (a.lastResizeQpc > latestResize) latestResize = a.lastResizeQpc;

                RECT rc{};
                GetWindowRect(a.hwnd, &rc);
                int w = rc.right - rc.left;
                int h = rc.bottom - rc.top;
                UINT dpi = SafeGetDpiForWindow(a.hwnd);

                if (w >= MulDiv(a.settings.minWidth, dpi, 96) &&
                    h >= MulDiv(a.settings.minHeight, dpi, 96)) {
                    LONGLONG fs = a.firstSizedQpc ? a.firstSizedQpc : now;
                    if (anyFirstSized == 0 || fs < anyFirstSized) anyFirstSized = fs;
                    if (h >= MulDiv(130, dpi, 96)) {
                        mainFirstSized = fs;
                        mainWidth = w;
                        mainDpi = dpi;
                    }
                }
            }

            if (!anyArmed) {
                Sleep(3);
                continue;
            }

            double sinceCreateMs = ElapsedMs(earliestCreated, now);

            // Wait for main flyout to become non-empty (up to 220ms before falling back to any sized popup)
            if (mainFirstSized == 0) {
                if (anyFirstSized == 0 || ElapsedMs(anyFirstSized, now) < 220.0) {
                    if (sinceCreateMs < 500.0) {
                        Sleep(3);
                        continue;
                    }
                }
                mainFirstSized = anyFirstSized ? anyFirstSized : earliestCreated;
            }

            double waitNeededMs = (mainWidth >= MulDiv(340, mainDpi, 96)) ? 36.0 : 58.0;
            double sinceMainSizedMs = ElapsedMs(mainFirstSized, now);
            double sinceResizeMs = ElapsedMs(latestResize, now);

            if (sinceMainSizedMs < waitNeededMs || (sinceResizeMs < 16.0 && sinceMainSizedMs < 110.0)) {
                Sleep(3);
                continue;
            }

            // Capture all currently active PopupWindowSiteBridge windows
            std::vector<CapturedCard> validCards;
            HWND primaryHwnd = nullptr;
            int maxArea = 0;
            Settings activeSettings = currentAnims[0].settings;
            POINT clickPt = currentAnims[0].cursorPos;

            for (const auto& a : currentAnims) {
                if (!a.armed) continue;
                CapturedCard card =
                    CapturePopupCard(a.hwnd, a.settings.minWidth, a.settings.minHeight);
                if (card.valid) {
                    int area = card.width * card.height;
                    if (area > maxArea) {
                        maxArea = area;
                        primaryHwnd = a.hwnd;
                        activeSettings = a.settings;
                        clickPt = a.cursorPos;
                    }
                    validCards.push_back(std::move(card));
                }
            }

            bool hasMainCandidate = (mainFirstSized != 0);
            bool mainFlyoutMissing = false;
            if (hasMainCandidate) {
                bool foundMain = false;
                for (const auto& c : validCards) {
                    if (c.height >= MulDiv(100, mainDpi, 96)) {
                        foundMain = true;
                        break;
                    }
                }
                if (!foundMain) mainFlyoutMissing = true;
            }

            bool anyMissingIcons = false;
            for (const auto& c : validCards) {
                if (!c.hasIcons) {
                    anyMissingIcons = true;
                    break;
                }
            }

            if (validCards.empty() || mainFlyoutMissing || (anyMissingIcons && sinceMainSizedMs < 250.0)) {
                session.captureRetries++;
                if (sinceMainSizedMs < 250.0 && session.captureRetries < 35) {
                    Sleep(6);
                    continue;
                }
                if (validCards.empty()) {
                    FinishAllActiveAnims(L"capture timeout fallback");
                    session = WaveSession{};
                    continue;
                }
            }

            // Fuse all valid cards (Command Bar + Main Flyout) into a single unified BGRA buffer
            const int SHADOW_PAD = 24;
            RECT unionRc = validCards[0].cardScreenRect;
            for (size_t i = 1; i < validCards.size(); i++) {
                unionRc.left = std::min(unionRc.left, validCards[i].cardScreenRect.left);
                unionRc.top = std::min(unionRc.top, validCards[i].cardScreenRect.top);
                unionRc.right = std::max(unionRc.right, validCards[i].cardScreenRect.right);
                unionRc.bottom = std::max(unionRc.bottom, validCards[i].cardScreenRect.bottom);
            }

            // Expand unionRc by SHADOW_PAD so the 3D ripple renders the soft native drop shadow
            unionRc.left -= SHADOW_PAD;
            unionRc.top -= SHADOW_PAD;
            unionRc.right += SHADOW_PAD;
            unionRc.bottom += SHADOW_PAD;

            int uw = unionRc.right - unionRc.left;
            int uh = unionRc.bottom - unionRc.top;
            if (uw <= 0 || uh <= 0 || uw > 2400 || uh > 2400) {
                FinishAllActiveAnims(L"invalid union size");
                session = WaveSession{};
                continue;
            }

            session.active = true;
            session.captured = true;
            session.startQpc = Now();
            session.cursorPos = clickPt;
            session.unionScreenRect = unionRc;
            session.width = uw;
            session.height = uh;
            session.dpi = SafeGetDpiForWindow(primaryHwnd);
            session.settings = activeSettings;
            session.fusedPixels.assign((size_t)uw * uh, 0);

            for (auto& a : currentAnims) {
                // Real Acrylic blur is now captured in fusedPixels!
                // During the 3D ripple animation, keep real window at alpha=0 so only the ripple is seen:
                SetLayeredWindowAttributes(a.hwnd, 0, 0, LWA_ALPHA);
            }

            for (const auto& c : validCards) {
                int ox = c.cardScreenRect.left - unionRc.left;
                int oy = c.cardScreenRect.top - unionRc.top;
                for (int y = 0; y < c.height; y++) {
                    if (oy + y < 0 || oy + y >= uh) continue;
                    const uint32_t* src = c.pixels.data() + (size_t)y * c.width;
                    uint32_t* dst = session.fusedPixels.data() + (size_t)(oy + y) * uw + ox;
                    for (int x = 0; x < c.width; x++) {
                        if (ox + x >= 0 && ox + x < uw && (src[x] >> 24) != 0) {
                            dst[x] = src[x];
                        }
                    }
                }
            }

            // Generate soft native drop shadow in the outer SHADOW_PAD margin
            GenerateDropShadow(session.fusedPixels, uw, uh, SHADOW_PAD, session.dpi);

            DebugLog(L"3D WaveSession started: cards=%zu union=(%ld,%ld %dx%d) cursor=(%ld,%ld)",
                     validCards.size(), unionRc.left, unionRc.top, uw, uh, clickPt.x, clickPt.y);
        }

        // Phase 2: Render 3D Water Ripple frame
        double elapsed = ElapsedMs(session.startQpc, Now());
        float p = (float)(elapsed / (double)session.settings.durationMs);

        if (p >= 1.0f || (GetAsyncKeyState(VK_ESCAPE) & 0x8000)) {
            FinishAllActiveAnims(L"3D wave done");
            session = WaveSession{};
            continue;
        }

        RenderWaterDrop3DFrame(session, p);

        if (FAILED(DwmFlush())) {
            Sleep(4);
        }
    }

    FinishAllActiveAnims(L"mod unload");
    DestroyOverlayResources();
}

// ---------------------------------------------------------------------------
// Lifecycle Helpers & Hooks
// ---------------------------------------------------------------------------

static bool WasRecentlyAnimated(HWND hwnd) {
    std::lock_guard<std::mutex> lock(g_animMutex);
    if (g_anims.count(hwnd)) return true;
    auto it = g_recent.find(hwnd);
    return it != g_recent.end() && GetTickCount64() - it->second < 1200;
}

static RECT CurrentRect(HWND hwnd) {
    RECT rc{};
    GetWindowRect(hwnd, &rc);
    return rc;
}

static bool PrepareAnim(HWND hwnd, const RECT& finalRect, const wchar_t* source) {
    if (!hwnd || GetAncestor(hwnd, GA_PARENT) != GetDesktopWindow()) return false;

    Settings s = GetSettings();
    WCHAR cls[256];
    if (!IsTargetClass(hwnd, s, cls, ARRAYSIZE(cls))) return false;

    int w = finalRect.right - finalRect.left;
    int h = finalRect.bottom - finalRect.top;
    DebugLog(L"[%s] hwnd=%p class=\"%s\" rect=(%ld,%ld %dx%d)",
             source, hwnd, cls, finalRect.left, finalRect.top, w, h);



    // Check if an existing root menu is currently open and visible on screen.
    // If so, this new window is a SUBMENU (hovered flyout) and should NOT be intercepted
    // or deformed by the 3D ripple engine — WinUI 3 will render it natively and smoothly!
    bool isSubmenu = false;
    {
        std::lock_guard<std::mutex> lock(g_animMutex);
        ULONGLONG nowTick = GetTickCount64();
        for (const auto& [recentHwnd, openTime] : g_recent) {
            if (recentHwnd != hwnd && (nowTick - openTime) > 150 && (nowTick - openTime) < 15000) {
                if (IsWindow(recentHwnd) && IsWindowVisible(recentHwnd)) {
                    isSubmenu = true;
                    break;
                }
            }
        }
    }

    if (isSubmenu) {
        DebugLog(L"hwnd=%p skipped (detected as submenu of open context menu)", hwnd);
        return false;
    }

    LONGLONG now = Now();

    std::vector<Anim> staleToRestore;
    {
        std::lock_guard<std::mutex> lock(g_animMutex);
        for (auto it = g_anims.begin(); it != g_anims.end();) {
            if (it->first != hwnd && ElapsedMs(it->second.createdQpc, now) > 180.0) {
                staleToRestore.push_back(it->second);
                it = g_anims.erase(it);
            } else {
                ++it;
            }
        }
        if (!staleToRestore.empty()) {
            g_resetSession = true;
        }
    }
    for (const auto& oldAnim : staleToRestore) {
        RestoreRealWindow(oldAnim, L"superseded by new popup");
    }

    UINT dpi = SafeGetDpiForWindow(hwnd);
    Anim a;
    a.hwnd = hwnd;
    a.settings = s;
    a.createdQpc = now;
    a.firstSizedQpc = (w >= MulDiv(s.minWidth, dpi, 96) && h >= MulDiv(s.minHeight, dpi, 96)) ? now : 0;
    a.lastResizeQpc = now;
    a.lastRect = finalRect;
    GetCursorPos(&a.cursorPos);

    // Make window invisible to user (alpha=1) immediately upon creation,
    // preventing any pre-animation flash while allowing WinUI 3 / DWM to compose full Acrylic blur!
    HideWindowForWave(a);

    {
        std::lock_guard<std::mutex> lock(g_animMutex);
        g_anims[hwnd] = a;
    }
    return true;
}

static void StartAnim(HWND hwnd) {
    {
        std::lock_guard<std::mutex> lock(g_animMutex);
        auto it = g_anims.find(hwnd);
        if (it == g_anims.end()) return;
        it->second.armed = true;
        g_recent[hwnd] = GetTickCount64();
        if (g_recent.size() > 64) {
            ULONGLONG t = GetTickCount64();
            std::erase_if(g_recent, [t](const auto& kv) { return t - kv.second > 5000; });
        }
    }
    SetEvent(g_wakeEvent);
}

static void CancelAnim(HWND hwnd, const wchar_t* why) {
    Anim a;
    bool found = false;
    {
        std::lock_guard<std::mutex> lock(g_animMutex);
        auto it = g_anims.find(hwnd);
        if (it != g_anims.end()) {
            a = it->second;
            g_anims.erase(it);
            found = true;
            g_resetSession = true;
        }
    }
    if (found) {
        RestoreRealWindow(a, why);
        SetEvent(g_wakeEvent);
    }
}

static void OnBeforeResize(HWND hwnd, const RECT& newRect, const wchar_t* source) {
    std::lock_guard<std::mutex> lock(g_animMutex);
    auto it = g_anims.find(hwnd);
    if (it == g_anims.end()) return;

    int oldW = it->second.lastRect.right - it->second.lastRect.left;
    int oldH = it->second.lastRect.bottom - it->second.lastRect.top;
    int newW = newRect.right - newRect.left;
    int newH = newRect.bottom - newRect.top;

    if (newW != oldW || newH != oldH ||
        newRect.left != it->second.lastRect.left ||
        newRect.top != it->second.lastRect.top) {
        LONGLONG now = Now();
        UINT dpi = SafeGetDpiForWindow(hwnd);
        if (newW >= MulDiv(it->second.settings.minWidth, dpi, 96) &&
            newH >= MulDiv(it->second.settings.minHeight, dpi, 96) &&
            it->second.firstSizedQpc == 0) {
            it->second.firstSizedQpc = now;
        }
        it->second.lastResizeQpc = now;
        it->second.lastRect = newRect;
        DebugLog(L"[%s] hwnd=%p resize (%dx%d -> %dx%d)", source, hwnd, oldW, oldH, newW, newH);
    }
}

using ShowWindow_t = BOOL(WINAPI*)(HWND, int);
static ShowWindow_t ShowWindow_Original;

static BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    if (hWnd == g_hwndOverlay) return ShowWindow_Original(hWnd, nCmdShow);
    bool prepared = false;
    if (nCmdShow == SW_HIDE) {
        CancelAnim(hWnd, L"ShowWindow(SW_HIDE)");
    } else if (nCmdShow != SW_MINIMIZE && nCmdShow != SW_SHOWMINIMIZED &&
               nCmdShow != SW_FORCEMINIMIZE && !IsWindowVisible(hWnd)) {
        prepared = PrepareAnim(hWnd, CurrentRect(hWnd), L"ShowWindow");
    }

    BOOL ret = ShowWindow_Original(hWnd, nCmdShow);

    if (prepared) {
        if (IsWindowVisible(hWnd)) StartAnim(hWnd);
        else CancelAnim(hWnd, L"not visible after ShowWindow");
    }
    return ret;
}

using SetWindowPos_t = BOOL(WINAPI*)(HWND, HWND, int, int, int, int, UINT);
static SetWindowPos_t SetWindowPos_Original;

static BOOL WINAPI SetWindowPos_Hook(HWND hWnd, HWND hWndInsertAfter, int X, int Y,
                                     int cx, int cy, UINT uFlags) {
    if (hWnd == g_hwndOverlay) return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
    RECT nr = CurrentRect(hWnd);
    if (!(uFlags & SWP_NOMOVE)) OffsetRect(&nr, X - nr.left, Y - nr.top);
    if (!(uFlags & SWP_NOSIZE)) {
        nr.right = nr.left + cx;
        nr.bottom = nr.top + cy;
    }

    bool prepared = false;
    if (uFlags & SWP_HIDEWINDOW) {
        CancelAnim(hWnd, L"SWP_HIDEWINDOW");
    } else if ((uFlags & SWP_SHOWWINDOW) && !IsWindowVisible(hWnd)) {
        prepared = PrepareAnim(hWnd, nr, L"SetWindowPos");
    } else if ((uFlags & (SWP_NOSIZE | SWP_NOMOVE)) != (SWP_NOSIZE | SWP_NOMOVE)) {
        OnBeforeResize(hWnd, nr, L"SetWindowPos");
    }

    BOOL ret = SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);

    if (prepared) {
        if (IsWindowVisible(hWnd)) StartAnim(hWnd);
        else CancelAnim(hWnd, L"not visible after SetWindowPos");
    }
    return ret;
}

using DestroyWindow_t = BOOL(WINAPI*)(HWND);
static DestroyWindow_t DestroyWindow_Original;

static BOOL WINAPI DestroyWindow_Hook(HWND hWnd) {
    if (hWnd && hWnd != g_hwndOverlay) {
        CancelAnim(hWnd, L"DestroyWindow");
    }
    return DestroyWindow_Original(hWnd);
}

using MoveWindow_t = BOOL(WINAPI*)(HWND, int, int, int, int, BOOL);
static MoveWindow_t MoveWindow_Original;

static BOOL WINAPI MoveWindow_Hook(HWND hWnd, int X, int Y, int w, int h, BOOL repaint) {
    RECT nr{X, Y, X + w, Y + h};
    OnBeforeResize(hWnd, nr, L"MoveWindow");
    return MoveWindow_Original(hWnd, X, Y, w, h, repaint);
}

static int WINAPI SetWindowRgn_Hook(HWND hWnd, HRGN hRgn, BOOL bRedraw) {
    if (g_ownCall) return SetWindowRgn_Original(hWnd, hRgn, bRedraw);

    {
        std::lock_guard<std::mutex> lock(g_animMutex);
        auto it = g_anims.find(hWnd);
        if (it == g_anims.end()) return SetWindowRgn_Original(hWnd, hRgn, bRedraw);

        Anim& a = it->second;
        a.hasAppRgn = hRgn != nullptr;
        a.appRgnData.clear();
        if (hRgn) {
            DWORD size = GetRegionData(hRgn, 0, nullptr);
            if (size) {
                a.appRgnData.resize(size);
                if (!GetRegionData(hRgn, size, (RGNDATA*)a.appRgnData.data()))
                    a.appRgnData.clear();
            }
        }
    }

    if (hRgn) DeleteObject(hRgn);
    // Keep window unclipped (NULL region) while tracked so PrintWindow captures full content!
    MySetWindowRgn(hWnd, nullptr, FALSE);
    return 1;
}

using CreateWindowExW_t = HWND(WINAPI*)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int,
                                        int, HWND, HMENU, HINSTANCE, LPVOID);
static CreateWindowExW_t CreateWindowExW_Original;

static HWND WINAPI CreateWindowExW_Hook(DWORD exStyle, LPCWSTR cls, LPCWSTR name,
                                        DWORD style, int x, int y, int w, int h,
                                        HWND parent, HMENU menu, HINSTANCE inst,
                                        LPVOID param) {
    HWND hwnd = CreateWindowExW_Original(exStyle, cls, name, style, x, y, w, h, parent,
                                         menu, inst, param);
    if (hwnd && (style & WS_VISIBLE) && !(style & WS_CHILD) && IsWindowVisible(hwnd)) {
        if (PrepareAnim(hwnd, CurrentRect(hwnd), L"CreateWindowExW")) StartAnim(hwnd);
    }
    return hwnd;
}

using DwmSetWindowAttribute_t = HRESULT(WINAPI*)(HWND, DWORD, LPCVOID, DWORD);
static DwmSetWindowAttribute_t DwmSetWindowAttribute_Original;

static HRESULT WINAPI DwmSetWindowAttribute_Hook(HWND hwnd, DWORD attr, LPCVOID pv,
                                                 DWORD cb) {
    if (attr != DWMWA_CLOAK || !pv || cb < sizeof(BOOL))
        return DwmSetWindowAttribute_Original(hwnd, attr, pv, cb);

    bool cloak = *(const BOOL*)pv != FALSE;
    bool prepared = false;
    if (cloak) {
        CancelAnim(hwnd, L"cloak");
    } else if (IsWindowVisible(hwnd)) {
        prepared = PrepareAnim(hwnd, CurrentRect(hwnd), L"Uncloak");
    }

    HRESULT hr = DwmSetWindowAttribute_Original(hwnd, attr, pv, cb);

    if (prepared) {
        if (SUCCEEDED(hr)) StartAnim(hwnd);
        else CancelAnim(hwnd, L"uncloak failed");
    }
    return hr;
}

// ---------------------------------------------------------------------------
// Fallback: WinEvents
// ---------------------------------------------------------------------------

#ifndef EVENT_OBJECT_UNCLOAKED
#define EVENT_OBJECT_UNCLOAKED 0x8018
#endif

static std::thread g_eventThread;
static std::atomic<DWORD> g_eventThreadId{0};

static void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject,
                                  LONG idChild, DWORD, DWORD) {
    if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF || !hwnd) return;
    if (event == EVENT_OBJECT_HIDE) {
        CancelAnim(hwnd, L"WinEvent:Hide");
        return;
    }
    if (!IsWindowVisible(hwnd) || WasRecentlyAnimated(hwnd)) return;
    const wchar_t* src = event == EVENT_OBJECT_SHOW ? L"WinEvent:Show" : L"WinEvent:Uncloak";
    if (PrepareAnim(hwnd, CurrentRect(hwnd), src)) StartAnim(hwnd);
}

static void EventThreadProc() {
    MSG msg;
    PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE);
    g_eventThreadId = GetCurrentThreadId();

    DWORD pid = GetCurrentProcessId();
    HWINEVENTHOOK h1 = SetWinEventHook(EVENT_OBJECT_SHOW, EVENT_OBJECT_SHOW, nullptr,
                                       WinEventProc, pid, 0, WINEVENT_OUTOFCONTEXT);
    HWINEVENTHOOK h2 = SetWinEventHook(EVENT_OBJECT_UNCLOAKED, EVENT_OBJECT_UNCLOAKED,
                                       nullptr, WinEventProc, pid, 0,
                                       WINEVENT_OUTOFCONTEXT);
    HWINEVENTHOOK h3 = SetWinEventHook(EVENT_OBJECT_HIDE, EVENT_OBJECT_HIDE, nullptr,
                                       WinEventProc, pid, 0, WINEVENT_OUTOFCONTEXT);
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    if (h1) UnhookWinEvent(h1);
    if (h2) UnhookWinEvent(h2);
    if (h3) UnhookWinEvent(h3);
}

// ---------------------------------------------------------------------------
// Mod Lifecycle
// ---------------------------------------------------------------------------

template <typename T>
static void Hook(HMODULE mod, const char* name, void* hook, T* original) {
    void* p = mod ? (void*)GetProcAddress(mod, name) : nullptr;
    BOOL ok = p && Wh_SetFunctionHook(p, hook, (void**)original);
    DebugLog(L"hook %S: %s", name, ok ? L"ok" : L"FAILED");
}

BOOL Wh_ModInit() {
    LARGE_INTEGER f;
    QueryPerformanceFrequency(&f);
    g_qpcFreq = f.QuadPart ? f.QuadPart : 1;
    g_qpcInit = Now();

    LoadSettings();
    DebugLog(L"==== Init v1.0.0 (3D Water Ripple), pid=%lu ====", GetCurrentProcessId());

    g_wakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_wakeEvent) return FALSE;

    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    Hook(user32, "DestroyWindow", (void*)DestroyWindow_Hook, &DestroyWindow_Original);
    Hook(user32, "ShowWindow", (void*)ShowWindow_Hook, &ShowWindow_Original);
    Hook(user32, "SetWindowPos", (void*)SetWindowPos_Hook, &SetWindowPos_Original);
    Hook(user32, "MoveWindow", (void*)MoveWindow_Hook, &MoveWindow_Original);
    Hook(user32, "SetWindowRgn", (void*)SetWindowRgn_Hook, &SetWindowRgn_Original);
    Hook(user32, "CreateWindowExW", (void*)CreateWindowExW_Hook, &CreateWindowExW_Original);
    HMODULE dwmapi = GetModuleHandleW(L"dwmapi.dll");
    if (!dwmapi) dwmapi = LoadLibraryW(L"dwmapi.dll");
    Hook(dwmapi, "DwmSetWindowAttribute",
         (void*)DwmSetWindowAttribute_Hook, &DwmSetWindowAttribute_Original);

    g_stop = false;
    g_animThread = std::thread(AnimThreadProc);
    g_eventThread = std::thread(EventThreadProc);
    return TRUE;
}

void Wh_ModUninit() {
    DebugLog(L"Uninit");

    if (g_eventThread.joinable()) {
        while (!g_eventThreadId) Sleep(1);
        PostThreadMessageW(g_eventThreadId, WM_QUIT, 0, 0);
        g_eventThread.join();
    }

    g_stop = true;
    if (g_wakeEvent) SetEvent(g_wakeEvent);
    if (g_animThread.joinable()) g_animThread.join();
    if (g_wakeEvent) {
        CloseHandle(g_wakeEvent);
        g_wakeEvent = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    DebugLog(L"SettingsChanged");
}
