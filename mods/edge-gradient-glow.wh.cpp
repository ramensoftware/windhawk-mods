// ==WindhawkMod==
// @id              edge-gradient-glow
// @name            Edge Gradient Glow
// @description     A soft gradient line spreads from the point where your mouse touches a screen edge or corner.
// @version         1.1.0
// @author          Mokafela
// @github          https://github.com/Mokafela
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lgdi32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Edge Gradient Glow

Touch any screen edge with your mouse: a cyan-to-purple line grows outwards from
the contact point, wrapping smoothly around corners. Move away and it fades out.
The glow follows your mouse as you slide along an edge. Earlier hits finish
fading independently, including hits on another monitor.

Change colors, line length, thickness, glow, opacity and timings in
**Settings**. Colors use `#RRGGBB`. Windhawk 2.0 provides visual color pickers;
Windhawk 1.7.3 provides hex text fields. Dimensions are physical screen pixels.
A line length of `0` spreads around the entire monitor. The effect works on each
connected monitor, including the boundaries between monitors, and never
intercepts mouse clicks.

The effect runs in the Windows desktop shell. Disabling this mod removes it
immediately. Display layout changes are detected automatically. Exclusive
fullscreen applications and the Windows secure desktop may cover the glow.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- colorCenter: "#42E8FF"
  $name: Color at the contact point
  $description: "RGB color in #RRGGBB format."
  #! $format: colorRgb
- colorEnds: "#A855F7"
  $name: Color at the ends
  $description: "RGB color in #RRGGBB format."
  #! $format: colorRgb
- lineLength: 1000
  $name: Total line length (pixels)
  $description: Grows equally in both directions. 0 fills the entire monitor perimeter. Range 0-32000.
- thickness: 3
  $name: Line thickness (pixels)
  $description: Range 1-20.
- glowSize: 12
  $name: Soft glow size (pixels)
  $description: Range 0-40. 0 disables the soft halo.
- opacity: 90
  $name: Opacity (percent)
  $description: Range 1-100.
- spreadDuration: 420
  $name: Spread animation (milliseconds)
  $description: Time to reach the full line length. Range 50-3000.
- fadeDuration: 320
  $name: Fade out (milliseconds)
  $description: Range 50-3000.
- lingerDuration: 90
  $name: Linger after leaving (milliseconds)
  $description: Keeps the line briefly before fading. Range 0-2000.
- edgeDistance: 2
  $name: Edge activation distance (pixels)
  $description: Range 1-20. 1 requires touching the very last pixel.
- frameRate: 60
  $name: Animation frame rate
  $description: Range 30-144. A settled glow is not repainted until it changes.
*/
// ==/WindhawkModSettings==

#define NOMINMAX
#include <windows.h>
#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <list>
#include <vector>

namespace {
constexpr wchar_t kClass[] = L"Windhawk.EdgeGradientGlow.1";
struct Color {
    double r, g, b;
};
struct Settings {
    Color center{66, 232, 255}, ends{168, 85, 247};
    int length = 1000, thickness = 3, glow = 12, opacity = 90;
    int spread = 420, fade = 320, linger = 90, threshold = 2, fps = 60;
} g_settings;
HANDLE g_stop = nullptr, g_thread = nullptr, g_ready = nullptr,
       g_singleton = nullptr;
HMODULE g_module = nullptr;
std::atomic_bool g_started{false};
bool g_displayChanged = false;

double Smooth(double t) {
    t = std::clamp(t, 0.0, 1.0);
    return t * t * (3.0 - 2.0 * t);
}

bool SameRect(const RECT& a, const RECT& b) {
    return EqualRect(&a, &b) != FALSE;
}

// Clockwise perimeter coordinate; left/top and right/bottom corners share
// exactly the same coordinates on both adjoining strips, so the glow cannot
// jump there.
double PerimeterAt(const RECT& r, POINT p, int* distance = nullptr) {
    const int x = p.x - r.left, y = p.y - r.top;
    const int w = r.right - r.left - 1, h = r.bottom - r.top - 1;
    std::array<int, 4> d{y, w - x, h - y, x};
    int side =
        static_cast<int>(std::min_element(d.begin(), d.end()) - d.begin());
    if (distance)
        *distance = d[side];
    switch (side) {
        case 0:
            return x;
        case 1:
            return w + y;
        case 2:
            return w + h + (w - x);
        default:
            return 2.0 * w + h + (h - y);
    }
}

double PerimeterLength(const RECT& r) {
    return 2.0 * (r.right - r.left + r.bottom - r.top - 2);
}

double CircularDistance(double a, double b, double perimeter) {
    double d = std::abs(a - b);
    return std::min(d, perimeter - d);
}

struct Wave {
    bool active = false, touching = false;
    RECT monitor{};
    double origin = 0, leaveAlpha = 1;
    ULONGLONG born = 0, left = 0;

    void Begin(RECT r, double contact, ULONGLONG now) {
        active = touching = true;
        monitor = r;
        origin = contact;
        born = now;
        left = 0;
        leaveAlpha = 1;
    }
    double Alpha(ULONGLONG now) const {
        if (touching)
            return Smooth(double(now - born) / 110.0);
        double elapsed = double(now - left) - g_settings.linger;
        return leaveAlpha * (1.0 - Smooth(elapsed / g_settings.fade));
    }
    double Radius(ULONGLONG now) const {
        double maximum = PerimeterLength(monitor) / 2.0;
        if (g_settings.length)
            maximum = std::min(maximum, g_settings.length / 2.0);
        double t = std::clamp(double(now - born) / g_settings.spread, 0.0, 1.0);
        return std::max(0.5, maximum * (1.0 - std::pow(1.0 - t, 3.0)));
    }
    void Leave(ULONGLONG now) {
        if (touching) {
            leaveAlpha = Alpha(now);
            touching = false;
            left = now;
        }
    }
};

// Contact updates are kept separate from Win32 rendering so input sequences can
// be checked directly without moving the user's mouse.
struct Contacts {
    std::vector<Wave> waves;
    bool Touching(const RECT& monitor) const {
        return std::any_of(waves.begin(), waves.end(), [&](const Wave& wave) {
            return wave.touching && SameRect(wave.monitor, monitor);
        });
    }
    bool Update(const RECT* hit, double contact, ULONGLONG now) {
        bool changed = false;
        auto current =
            std::find_if(waves.begin(), waves.end(),
                         [](const Wave& wave) { return wave.touching; });
        if (hit && current != waves.end() && SameRect(current->monitor, *hit)) {
            // Following the cursor preserves the original spread/fade-in time.
            changed = current->origin != contact;
            current->origin = contact;
        } else {
            if (current != waves.end()) {
                current->Leave(now);
                changed = true;
            }
            if (hit) {
                waves.emplace_back();
                waves.back().Begin(*hit, contact, now);
                changed = true;
            }
        }
        size_t previousSize = waves.size();
        waves.erase(std::remove_if(waves.begin(), waves.end(),
                                   [&](const Wave& wave) {
                                       return !wave.touching &&
                                              wave.Alpha(now) <= 0;
                                   }),
                    waves.end());
        return changed || waves.size() != previousSize;
    }
    bool Animating(ULONGLONG now) const {
        return std::any_of(waves.begin(), waves.end(), [&](const Wave& wave) {
            return !wave.touching ||
                   now - wave.born <
                       static_cast<ULONGLONG>(std::max(110, g_settings.spread));
        });
    }
};

// A 32-bit premultiplied BGRA pixel, as required by UpdateLayeredWindow.
uint32_t Pixel(double along, int inward, double radius, double alpha) {
    if (along >= radius)
        return 0;
    double u = along / radius;
    double tip = 1.0 - Smooth((u - 0.55) / 0.45);
    double core = 1.0 - Smooth(double(inward) - g_settings.thickness + 1.0);
    double halo = 0;
    if (g_settings.glow) {
        double d = std::max(0.0, double(inward) - g_settings.thickness + 1.0);
        double falloff = std::max(0.0, 1.0 - d / (g_settings.glow + 1.0));
        halo = 0.40 * falloff * falloff;
    }
    double a = std::clamp(
        alpha * g_settings.opacity / 100.0 * tip * std::max(core, halo), 0.0,
        1.0);
    double mix = Smooth(u);
    auto channel = [&](double start, double end) -> uint32_t {
        return static_cast<uint32_t>(
            std::lround((start + (end - start) * mix) * a));
    };
    return (uint32_t(std::lround(a * 255)) << 24) |
           (channel(g_settings.center.r, g_settings.ends.r) << 16) |
           (channel(g_settings.center.g, g_settings.ends.g) << 8) |
           channel(g_settings.center.b, g_settings.ends.b);
}

uint32_t Composite(uint32_t below, uint32_t above) {
    unsigned alpha = above >> 24;
    if (!alpha)
        return below;
    if (alpha == 255)
        return above;
    auto channel = [&](int shift) {
        return ((above >> shift) & 255) +
               ((((below >> shift) & 255) * (255 - alpha) + 127) / 255);
    };
    return (channel(24) << 24) | (channel(16) << 16) | (channel(8) << 8) |
           channel(0);
}

struct PaintWave {
    double origin, radius, alpha;
};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wp, LPARAM lp) {
    if (message == WM_NCHITTEST)
        return HTTRANSPARENT;
    if (message == WM_MOUSEACTIVATE)
        return MA_NOACTIVATE;
    if (message == WM_DISPLAYCHANGE || message == WM_DPICHANGED)
        g_displayChanged = true;
    if (message == WM_ERASEBKGND)
        return 1;
    return DefWindowProcW(hwnd, message, wp, lp);
}

// Four thin windows avoid allocating or copying a full-screen bitmap every
// frame. The vertical strips exclude the horizontal strips to avoid double
// alpha.
struct Strip {
    HWND hwnd = nullptr;
    HDC dc = nullptr;
    HBITMAP bitmap = nullptr;
    HGDIOBJ previous = nullptr;
    uint32_t* pixels = nullptr;
    RECT rect{};
    bool visible = false;

    void Destroy() {
        if (hwnd)
            DestroyWindow(hwnd);
        if (previous)
            SelectObject(dc, previous);
        if (bitmap)
            DeleteObject(bitmap);
        if (dc)
            DeleteDC(dc);
        hwnd = nullptr;
        dc = nullptr;
        bitmap = nullptr;
        previous = nullptr;
        pixels = nullptr;
        visible = false;
    }
    bool Create(RECT r) {
        Destroy();
        rect = r;
        int w = r.right - r.left, h = r.bottom - r.top;
        if (w <= 0 || h <= 0)
            return true;
        hwnd =
            CreateWindowExW(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST |
                                WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
                            kClass, L"Edge Gradient Glow", WS_POPUP, r.left,
                            r.top, w, h, nullptr, nullptr, g_module, nullptr);
        dc = CreateCompatibleDC(nullptr);
        BITMAPINFO info{};
        info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth = w;
        info.bmiHeader.biHeight = -h;
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;
        bitmap =
            CreateDIBSection(dc, &info, DIB_RGB_COLORS,
                             reinterpret_cast<void**>(&pixels), nullptr, 0);
        if (!hwnd || !dc || !bitmap || !pixels) {
            Destroy();
            return false;
        }
        previous = SelectObject(dc, bitmap);
        return true;
    }
    void Hide() {
        if (visible)
            ShowWindow(hwnd, SW_HIDE);
        visible = false;
    }
    void Paint(const RECT& monitor, const std::vector<PaintWave>& waves) {
        if (!hwnd)
            return;
        if (waves.empty()) {
            Hide();
            return;
        }
        int w = rect.right - rect.left, h = rect.bottom - rect.top;
        double perimeter = PerimeterLength(monitor);
        bool nonzero = false;
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                int inward;
                double s = PerimeterAt(
                    monitor, POINT{rect.left + x, rect.top + y}, &inward);
                uint32_t pixel = 0;
                for (const auto& wave : waves) {
                    pixel = Composite(
                        pixel,
                        Pixel(CircularDistance(s, wave.origin, perimeter),
                              inward, wave.radius, wave.alpha));
                }
                pixels[y * w + x] = pixel;
                nonzero |= (pixel >> 24) != 0;
            }
        }
        if (!nonzero) {
            Hide();
            return;
        }
        POINT dest{rect.left, rect.top}, source{};
        SIZE size{w, h};
        BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        if (UpdateLayeredWindow(hwnd, nullptr, &dest, &size, dc, &source, 0,
                                &blend, ULW_ALPHA)) {
            SetWindowPos(
                hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
            visible = true;
        }
    }
};

struct MonitorSurface {
    RECT monitor;
    std::array<Strip, 4> strips;
    bool ready = true;
    explicit MonitorSurface(RECT r) : monitor(r) {
        int band =
            std::min<int>(g_settings.thickness + g_settings.glow + 1,
                          std::min(r.right - r.left, r.bottom - r.top) / 2);
        std::array<RECT, 4> bounds{
            {{r.left, r.top, r.right, r.top + band},
             {r.left, r.bottom - band, r.right, r.bottom},
             {r.left, r.top + band, r.left + band, r.bottom - band},
             {r.right - band, r.top + band, r.right, r.bottom - band}}};
        for (size_t i = 0; i < strips.size(); ++i)
            ready &= strips[i].Create(bounds[i]);
        if (!ready)
            for (auto& strip : strips)
                strip.Destroy();
    }
    MonitorSurface(const MonitorSurface&) = delete;
    MonitorSurface& operator=(const MonitorSurface&) = delete;
    ~MonitorSurface() {
        for (auto& strip : strips)
            strip.Destroy();
    }

    void Paint(const Contacts& contacts, ULONGLONG now) {
        if (!ready)
            return;
        std::vector<PaintWave> frame;
        for (const auto& wave : contacts.waves) {
            if (SameRect(wave.monitor, monitor) && wave.Alpha(now) > 0)
                frame.push_back(
                    {wave.origin, wave.Radius(now), wave.Alpha(now)});
        }
        for (auto& strip : strips)
            strip.Paint(monitor, frame);
    }
};

BOOL CALLBACK CollectMonitor(HMONITOR, HDC, LPRECT rect, LPARAM data) {
    reinterpret_cast<std::vector<RECT>*>(data)->push_back(*rect);
    return TRUE;
}

DWORD WINAPI Worker(void*) {
    // This changes only this worker thread, never Explorer's DPI mode.
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    WNDCLASSW wc{};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = g_module;
    wc.lpszClassName = kClass;
    if (!RegisterClassW(&wc)) {
        SetEvent(g_ready);
        return 1;
    }
    HWND control =
        CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kClass,
                        L"Edge Gradient Glow Controller", WS_POPUP, 0, 0, 0, 0,
                        nullptr, nullptr, g_module, nullptr);
    if (!control) {
        UnregisterClassW(kClass, g_module);
        SetEvent(g_ready);
        return 1;
    }
    std::list<MonitorSurface> surfaces;
    std::vector<RECT> monitors;
    Contacts contacts;
    bool painted = false, wasAnimating = false;
    ULONGLONG nextMonitorCheck = 0, lastPaint = 0;
    g_started = true;
    SetEvent(g_ready);

    while (WaitForSingleObject(g_stop, 0) != WAIT_OBJECT_0) {
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        ULONGLONG now = GetTickCount64();
        if (g_displayChanged || now >= nextMonitorCheck) {
            std::vector<RECT> current;
            EnumDisplayMonitors(nullptr, nullptr, CollectMonitor,
                                reinterpret_cast<LPARAM>(&current));
            bool changed = current.size() != monitors.size();
            if (!changed) {
                for (size_t i = 0; i < current.size(); ++i)
                    changed |= !SameRect(current[i], monitors[i]);
            }
            if (changed || g_displayChanged) {
                contacts.waves.clear();
                surfaces.clear();
                painted = wasAnimating = false;
            }
            monitors = std::move(current);
            g_displayChanged = false;
            nextMonitorCheck = now + 2000;
        }

        POINT cursor{};
        const RECT* hit = nullptr;
        double contact = 0;
        if (GetCursorPos(&cursor)) {
            for (const RECT& monitor : monitors) {
                if (!PtInRect(&monitor, cursor))
                    continue;
                int distance;
                contact = PerimeterAt(monitor, cursor, &distance);
                int threshold = g_settings.threshold;
                // A small release margin prevents flickering at the threshold.
                if (contacts.Touching(monitor))
                    threshold += 3;
                if (distance < threshold)
                    hit = &monitor;
                break;
            }
        }
        bool changed = contacts.Update(hit, contact, now);
        bool animating = contacts.Animating(now);
        DWORD interval = (1000 + g_settings.fps - 1) / g_settings.fps;
        if (changed || !painted || (wasAnimating && !animating) ||
            (animating && now - lastPaint >= interval)) {
            for (const auto& wave : contacts.waves) {
                auto surface = std::find_if(surfaces.begin(), surfaces.end(),
                                            [&](const MonitorSurface& item) {
                                                return SameRect(item.monitor,
                                                                wave.monitor);
                                            });
                if (surface == surfaces.end())
                    surfaces.emplace_back(wave.monitor);
            }
            for (auto& surface : surfaces)
                surface.Paint(contacts, now);
            lastPaint = now;
            painted = true;
        }
        wasAnimating = animating;
        if (contacts.waves.empty())
            interval = 16;
        MsgWaitForMultipleObjectsEx(1, &g_stop, interval, QS_ALLINPUT,
                                    MWMO_INPUTAVAILABLE);
    }
    surfaces.clear();
    DestroyWindow(control);
    UnregisterClassW(kClass, g_module);
    return 0;
}

#ifndef EDGE_GLOW_STANDALONE
Color ReadColor(PCWSTR name, Color fallback) {
    PCWSTR value = Wh_GetStringSetting(name);
    PCWSTR p = value;
    if (p && *p == L'#')
        ++p;
    unsigned rgb = 0;
    int digits = 0;
    if (p) {
        while (*p && digits < 6) {
            wchar_t c = *p++;
            int d = c >= L'0' && c <= L'9'   ? c - L'0'
                    : c >= L'a' && c <= L'f' ? c - L'a' + 10
                    : c >= L'A' && c <= L'F' ? c - L'A' + 10
                                             : -1;
            if (d < 0) {
                digits = -1;
                break;
            }
            rgb = (rgb << 4) | d;
            ++digits;
        }
    }
    bool valid = p && !*p && digits == 6;
    if (value)
        Wh_FreeStringSetting(value);
    return valid ? Color{double((rgb >> 16) & 255), double((rgb >> 8) & 255),
                         double(rgb & 255)}
                 : fallback;
}

void LoadSettings() {
    g_settings.center = ReadColor(L"colorCenter", g_settings.center);
    g_settings.ends = ReadColor(L"colorEnds", g_settings.ends);
    auto read = [](PCWSTR key, int lo, int hi) {
        return std::clamp(Wh_GetIntSetting(key), lo, hi);
    };
    g_settings.length = read(L"lineLength", 0, 32000);
    g_settings.thickness = read(L"thickness", 1, 20);
    g_settings.glow = read(L"glowSize", 0, 40);
    g_settings.opacity = read(L"opacity", 1, 100);
    g_settings.spread = read(L"spreadDuration", 50, 3000);
    g_settings.fade = read(L"fadeDuration", 50, 3000);
    g_settings.linger = read(L"lingerDuration", 0, 2000);
    g_settings.threshold = read(L"edgeDistance", 1, 20);
    g_settings.fps = read(L"frameRate", 30, 144);
}
#endif

void Stop() {
    if (g_stop)
        SetEvent(g_stop);
    // Do not let Windhawk unload code while this thread or its window procedure
    // runs.
    if (g_thread) {
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
    }
    if (g_stop)
        CloseHandle(g_stop);
    if (g_ready)
        CloseHandle(g_ready);
    if (g_singleton)
        CloseHandle(g_singleton);
    g_thread = g_stop = g_ready = g_singleton = nullptr;
    g_started = false;
}

BOOL Start() {
    g_singleton =
        CreateMutexW(nullptr, FALSE, L"Local\\Windhawk.EdgeGradientGlow.1");
    if (!g_singleton || GetLastError() == ERROR_ALREADY_EXISTS) {
        Stop();
        return FALSE;
    }
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<PCWSTR>(&WindowProc), &g_module);
    g_stop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stop || !g_ready) {
        Stop();
        return FALSE;
    }
    g_thread = CreateThread(nullptr, 0, Worker, nullptr, 0, nullptr);
    if (!g_thread || WaitForSingleObject(g_ready, 5000) != WAIT_OBJECT_0 ||
        !g_started) {
        Stop();
        return FALSE;
    }
    return TRUE;
}
}  // namespace

#ifndef EDGE_GLOW_STANDALONE
BOOL Wh_ModInit() {
    // Separate Explorer folder processes must not create duplicate overlays.
    HWND shell = GetShellWindow();
    DWORD shellPid = 0;
    if (shell)
        GetWindowThreadProcessId(shell, &shellPid);
    if (shellPid && shellPid != GetCurrentProcessId())
        return FALSE;
    LoadSettings();
    return Start();
}

void Wh_ModUninit() {
    Stop();
}

BOOL Wh_ModSettingsChanged(BOOL* reload) {
    *reload = TRUE;
    return TRUE;
}
#else
// Preview host uses the exact same renderer, input loop and cleanup as the mod.
int main(int argc, char** argv) {
    if (!Start())
        return 1;
    DWORD duration =
        argc > 1 ? std::clamp(std::atoi(argv[1]), 1, 120) * 1000 : 20000;
    WaitForSingleObject(g_stop, duration);
    Stop();
    return 0;
}
#endif
