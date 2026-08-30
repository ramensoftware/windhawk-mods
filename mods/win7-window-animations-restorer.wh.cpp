// ==WindhawkMod==
// @id              win7-window-animations-restorer
// @name            Windows 7 Window Animations Restorer
// @description     This mod restores the Windows 7 Aero open, close, minimize and restore on classic Win32 windows without hooking DWM.
// @version         1.0.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @include         explorer.exe
// @include         notepad.exe
// @include         write.exe
// @include         wordpad.exe
// @include         mspaint.exe
// @include         SnippingTool.exe
// @include         iexplore.exe
// @include         regedit.exe
// @include         control.exe
// @include         rundll32.exe
// @include         calc.exe
// @include         charmap.exe
// @include         taskmgr.exe
// @include         cmd.exe
// @include         winver.exe
// @include         msconfig.exe
// @include         mstsc.exe
// @include         wmplayer.exe
// @include         sol.exe
// @include         spider.exe
// @include         winmine.exe
// @include         freecell.exe
// @include         Chess.exe
// @include         Mahjong.exe
// @include         stikynot.exe
// @include         outlook.exe
// @include         winword.exe
// @include         excel.exe
// @include         powerpnt.exe
// @include         thunderbird.exe
// @exclude         dwm.exe
// @exclude         windhawk.exe
// @exclude         csrss.exe
// @exclude         wininit.exe
// @exclude         winlogon.exe
// @exclude         services.exe
// @exclude         lsass.exe
// @exclude         smss.exe
// @exclude         svchost.exe
// @exclude         ApplicationFrameHost.exe
// @exclude         RuntimeBroker.exe
// @exclude         ShellExperienceHost.exe
// @exclude         StartMenuExperienceHost.exe
// @exclude         SearchHost.exe
// @exclude         SearchApp.exe
// @exclude         SystemSettings.exe
// @exclude         TextInputHost.exe
// @exclude         LockApp.exe
// @exclude         sihost.exe
// @exclude         fontdrvhost.exe
// @exclude         conhost.exe
// @exclude         dllhost.exe
// @exclude         taskhostw.exe
// @exclude         wwahost.exe
// @exclude         MsMpEng.exe
// @exclude         smartscreen.exe
// @exclude         SecurityHealthService.exe
// @exclude         SecurityHealthSystray.exe
// @exclude         backgroundTaskHost.exe
// @exclude         UserOOBEBroker.exe
// @compilerOptions -lgdi32 -lmsimg32 -lshcore -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
## About

This mod tries to restore the Windows 7 window animations on Windows 10 and Windows 11.

Note: This is a best effort recreation: the new Windows draws windows differently, so
a perfect copy is not possible. The mod simply does its best to make opening,
closing, minimizing and restoring a window feel like it did in Windows 7, with
the same gentle fades and the same movement towards the taskbar.

The timings and the movements are taken from how Windows 7 really did it, as
far as the new Windows allows.

This mod is new, and it can get better over time thanks to user feedback and
contributions. For any problems, please report them to the author.

It never injects into `dwm.exe`. The animation is drawn on a layered overlay
in the program that owns the window.

## What it does

* **Minimize** - the window shrinks towards its taskbar button while fading
  out, and restore plays the same movement backwards.
* **Open / close** - not used. Recreating those with an overlay fights the
  way programs show and destroy windows, and looked wrong (ghosts, tiny
  thumbnails on the taskbar). Windows handles open/close itself.
* **Maximize / restore down** - optional and off by default, because Windows 7
  itself had no real animation for it; you can turn it on in the mod options.

## Requirements

Works with the default Windhawk injection. You do **not** need
"Inject into critical system processes".

## Good to know

* The mod only changes how the animations look; when in doubt it steps aside
  and lets Windows behave normally.
* It does not modify system files and it does not replace parts of Windows.
* Glass and blur effects from other mods (such as OpenGlass or DWMBlurGlass)
  keep working.
* The real window is never cloaked. Cloaking empties Explorer, Control Panel,
  Task Manager and other apps (blank white client). Minimize runs under the
  overlay; restore grows the overlay first, then shows the real window.
* Known limitation: closing some Windows 10 style (UWP) windows still shows
  the normal Windows animation.

## Credits

- **DWM 3D Transforms** by [xalejandro](https://github.com/tetawaves).
- [OpenGlass](https://github.com/ALTaleX531/OpenGlass) by ALTaleX
- **3D Aero Transforms mod** by [kieldbg](https://github.com/kieldbg).
- Overlay / `user32` architecture:
  [Classic Minimize/Maximize Animations](https://windhawk.net/mods/classic-min-max-animations)
  by [aubymori](https://github.com/aubymori).

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- animateOpenClose: false
  $name: Animate opening and closing windows
  $description: Off. Overlay open/close fights with how programs show and destroy windows (ghost thumbnails, windows that never appear). Leave this off; minimize/restore is the Win7 motion that matters.

- animateMinimize: true
  $name: Animate minimizing and restoring windows
  $description: 250 ms shrink/grow toward the taskbar button, with the Win7 3D tilt.

- animateMaximize: false
  $name: Animate maximizing and restoring down
  $description: Windows 7 had no dedicated maximize animation. Enable to use the 200 ms linear resize.

- respectMinAnimate: true
  $name: Respect "Animate windows when minimizing and maximizing"
  $description: When on, the Performance Options checkbox still disables minimize/maximize animations.

- durationScale: 100
  $name: Duration scale (%)
  $description: 100 is authentic Windows 7. 200 plays everything at half speed.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <dwmapi.h>
#include <shellscalingapi.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <deque>
#include <mutex>
#include <unordered_map>
#include <vector>

#ifndef DWMWA_TRANSITIONS_FORCEDISABLED
#define DWMWA_TRANSITIONS_FORCEDISABLED 3
#endif

#ifndef DWMWA_CLOAK
#define DWMWA_CLOAK 13
#endif

#ifndef PW_RENDERFULLCONTENT
#define PW_RENDERFULLCONTENT 0x00000002
#endif

#define RECTW(rc) ((rc).right - (rc).left)
#define RECTH(rc) ((rc).bottom - (rc).top)

// ---------------------------------------------------------------------------
// RAII
// ---------------------------------------------------------------------------

class ScopedDc {
   public:
    ScopedDc() = default;
    explicit ScopedDc(HDC hdc) noexcept : m_hdc(hdc) {}
    ~ScopedDc() { reset(); }
    ScopedDc(const ScopedDc&) = delete;
    ScopedDc& operator=(const ScopedDc&) = delete;
    ScopedDc(ScopedDc&& other) noexcept : m_hdc(other.m_hdc) { other.m_hdc = nullptr; }
    ScopedDc& operator=(ScopedDc&& other) noexcept {
        if (this != &other) {
            reset(other.m_hdc);
            other.m_hdc = nullptr;
        }
        return *this;
    }
    void reset(HDC hdc = nullptr) noexcept {
        if (m_hdc) {
            DeleteDC(m_hdc);
        }
        m_hdc = hdc;
    }
    HDC get() const noexcept { return m_hdc; }
    explicit operator bool() const noexcept { return m_hdc != nullptr; }

   private:
    HDC m_hdc = nullptr;
};

class ScopedWindowDc {
   public:
    ScopedWindowDc() = default;
    ScopedWindowDc(HWND hwnd, HDC hdc) noexcept : m_hwnd(hwnd), m_hdc(hdc) {}
    ~ScopedWindowDc() { reset(); }
    ScopedWindowDc(const ScopedWindowDc&) = delete;
    ScopedWindowDc& operator=(const ScopedWindowDc&) = delete;
    void reset() noexcept {
        if (m_hdc) {
            ReleaseDC(m_hwnd, m_hdc);
            m_hdc = nullptr;
            m_hwnd = nullptr;
        }
    }
    HDC get() const noexcept { return m_hdc; }
    explicit operator bool() const noexcept { return m_hdc != nullptr; }

   private:
    HWND m_hwnd = nullptr;
    HDC m_hdc = nullptr;
};

class ScopedGdiObj {
   public:
    ScopedGdiObj() = default;
    explicit ScopedGdiObj(HGDIOBJ obj) noexcept : m_obj(obj) {}
    ~ScopedGdiObj() { reset(); }
    ScopedGdiObj(const ScopedGdiObj&) = delete;
    ScopedGdiObj& operator=(const ScopedGdiObj&) = delete;
    ScopedGdiObj(ScopedGdiObj&& other) noexcept : m_obj(other.m_obj) { other.m_obj = nullptr; }
    ScopedGdiObj& operator=(ScopedGdiObj&& other) noexcept {
        if (this != &other) {
            reset(other.m_obj);
            other.m_obj = nullptr;
        }
        return *this;
    }
    void reset(HGDIOBJ obj = nullptr) noexcept {
        if (m_obj) {
            DeleteObject(m_obj);
        }
        m_obj = obj;
    }
    HGDIOBJ get() const noexcept { return m_obj; }
    explicit operator bool() const noexcept { return m_obj != nullptr; }

   private:
    HGDIOBJ m_obj = nullptr;
};

class ScopedSelect {
   public:
    ScopedSelect(HDC hdc, HGDIOBJ obj) noexcept : m_hdc(hdc), m_prev(SelectObject(hdc, obj)) {}
    ~ScopedSelect() {
        if (m_hdc && m_prev) {
            SelectObject(m_hdc, m_prev);
        }
    }
    ScopedSelect(const ScopedSelect&) = delete;
    ScopedSelect& operator=(const ScopedSelect&) = delete;

   private:
    HDC m_hdc;
    HGDIOBJ m_prev;
};

// Clang (Windhawk's compiler) has no __try/__except. Guard WinAPI with
// IsWindow and treat FALSE as failure; do not wrap in catch(...).
// ---------------------------------------------------------------------------
// 4x4 matrix (row vectors, D3D convention v * M)
// ---------------------------------------------------------------------------

namespace Mat {

struct Matrix4x4F {
    FLOAT _11, _12, _13, _14;
    FLOAT _21, _22, _23, _24;
    FLOAT _31, _32, _33, _34;
    FLOAT _41, _42, _43, _44;

    Matrix4x4F() noexcept {
        _11 = 1;
        _12 = 0;
        _13 = 0;
        _14 = 0;
        _21 = 0;
        _22 = 1;
        _23 = 0;
        _24 = 0;
        _31 = 0;
        _32 = 0;
        _33 = 1;
        _34 = 0;
        _41 = 0;
        _42 = 0;
        _43 = 0;
        _44 = 1;
    }

    static Matrix4x4F Translation(FLOAT x, FLOAT y, FLOAT z) noexcept {
        Matrix4x4F m;
        m._41 = x;
        m._42 = y;
        m._43 = z;
        return m;
    }

    static Matrix4x4F Scale(FLOAT x, FLOAT y, FLOAT z) noexcept {
        Matrix4x4F m;
        m._11 = x;
        m._22 = y;
        m._33 = z;
        return m;
    }

    static Matrix4x4F RotationX(FLOAT degrees) noexcept {
        const FLOAT a = degrees * (3.141592654f / 180.0f);
        const FLOAT s = std::sin(a), c = std::cos(a);
        Matrix4x4F m;
        m._22 = c;
        m._23 = s;
        m._32 = -s;
        m._33 = c;
        return m;
    }

    static Matrix4x4F RotationY(FLOAT degrees) noexcept {
        const FLOAT a = degrees * (3.141592654f / 180.0f);
        const FLOAT s = std::sin(a), c = std::cos(a);
        Matrix4x4F m;
        m._11 = c;
        m._13 = -s;
        m._31 = s;
        m._33 = c;
        return m;
    }

    Matrix4x4F operator*(const Matrix4x4F& b) const noexcept {
        const Matrix4x4F& a = *this;
        Matrix4x4F r;
        r._11 = a._11 * b._11 + a._12 * b._21 + a._13 * b._31 + a._14 * b._41;
        r._12 = a._11 * b._12 + a._12 * b._22 + a._13 * b._32 + a._14 * b._42;
        r._13 = a._11 * b._13 + a._12 * b._23 + a._13 * b._33 + a._14 * b._43;
        r._14 = a._11 * b._14 + a._12 * b._24 + a._13 * b._34 + a._14 * b._44;
        r._21 = a._21 * b._11 + a._22 * b._21 + a._23 * b._31 + a._24 * b._41;
        r._22 = a._21 * b._12 + a._22 * b._22 + a._23 * b._32 + a._24 * b._42;
        r._23 = a._21 * b._13 + a._22 * b._23 + a._23 * b._33 + a._24 * b._43;
        r._24 = a._21 * b._14 + a._22 * b._24 + a._23 * b._34 + a._24 * b._44;
        r._31 = a._31 * b._11 + a._32 * b._21 + a._33 * b._31 + a._34 * b._41;
        r._32 = a._31 * b._12 + a._32 * b._22 + a._33 * b._32 + a._34 * b._42;
        r._33 = a._31 * b._13 + a._32 * b._23 + a._33 * b._33 + a._34 * b._43;
        r._34 = a._31 * b._14 + a._32 * b._24 + a._33 * b._34 + a._34 * b._44;
        r._41 = a._41 * b._11 + a._42 * b._21 + a._43 * b._31 + a._44 * b._41;
        r._42 = a._41 * b._12 + a._42 * b._22 + a._43 * b._32 + a._44 * b._42;
        r._43 = a._41 * b._13 + a._42 * b._23 + a._43 * b._33 + a._44 * b._43;
        r._44 = a._41 * b._14 + a._42 * b._24 + a._43 * b._34 + a._44 * b._44;
        return r;
    }

    void TransformPoint(float x, float y, float z, float& ox, float& oy) const noexcept {
        const float rx = x * _11 + y * _21 + z * _31 + _41;
        const float ry = x * _12 + y * _22 + z * _32 + _42;
        const float rw = x * _14 + y * _24 + z * _34 + _44;
        if (rw > 0.0001f || rw < -0.0001f) {
            ox = rx / rw;
            oy = ry / rw;
        } else {
            ox = rx;
            oy = ry;
        }
    }
};

}  // namespace Mat

using Mat::Matrix4x4F;

// ---------------------------------------------------------------------------
// Windows 7 constants from uDWM.dll 6.1.7600.16385
// ---------------------------------------------------------------------------

constexpr double kShowHideDurationSec = 0.25;
constexpr double kRectMoveDurationSec = 0.20;

enum class AnimationType {
    None = 0,
    Open,
    Close,
    Minimize,
    RestoreFromMinimized,
    Maximize,
    RestoreFromMaximized,
};

static float Lerp(float a, float b, float t) { return a + (b - a) * t; }

static bool IsRectUsable(const RECT& rc) {
    return rc.right > rc.left && rc.bottom > rc.top;
}

static RECT LerpRect(const RECT& a, const RECT& b, float t) {
    RECT r;
    r.left = static_cast<LONG>(std::lround(Lerp(static_cast<float>(a.left), static_cast<float>(b.left), t)));
    r.top = static_cast<LONG>(std::lround(Lerp(static_cast<float>(a.top), static_cast<float>(b.top), t)));
    r.right = static_cast<LONG>(std::lround(Lerp(static_cast<float>(a.right), static_cast<float>(b.right), t)));
    r.bottom = static_cast<LONG>(std::lround(Lerp(static_cast<float>(a.bottom), static_cast<float>(b.bottom), t)));
    return r;
}

static RECT AspectCorrectedMinimizeTarget(const RECT& button) {
    // uDWM.dll+0x17D11: dest is a square sitting on the left of the button.
    const float bw = static_cast<float>(RECTW(button));
    const float bh = static_cast<float>(RECTH(button));
    if (bw < 1.0f || bh < 1.0f) {
        return button;
    }
    const float ar = bh / bw;
    RECT t = button;
    t.right = t.left + static_cast<LONG>(bw * ar);
    t.bottom = t.top + static_cast<LONG>(bh * ar);
    if (!IsRectUsable(t)) {
        return button;
    }
    return t;
}

struct Win7TransformParams {
    float rotX = 0.0f;
    float rotY = 0.0f;
    float transZ = 0.0f;
    float opacity = 1.0f;
};

static Win7TransformParams ParamsFor(AnimationType type, float t) {
    Win7TransformParams p;
    t = std::clamp(t, 0.0f, 1.0f);
    switch (type) {
        case AnimationType::Minimize:
            p.rotX = 5.0f * t;
            p.rotY = 8.0f * t;
            p.transZ = -4.0f * t;
            p.opacity = 1.0f - 0.35f * t;  // never 0: last frame still covers
            break;
        case AnimationType::RestoreFromMinimized: {
            const float away = 1.0f - t;
            p.rotX = 5.0f * away;
            p.rotY = 8.0f * away;
            p.transZ = -4.0f * away;
            p.opacity = 0.65f + 0.35f * t;
            break;
        }
        case AnimationType::Open: {
            const float away = 1.0f - t;
            p.rotX = -0.017f * away;
            p.rotY = -0.1f * away;
            p.transZ = -5.0f * away;
            p.opacity = 0.15f + 0.85f * t;
            break;
        }
        case AnimationType::Close:
            p.rotX = -0.017f * t;
            p.rotY = -0.1f * t;
            p.transZ = -5.0f * t;
            p.opacity = 1.0f - 0.85f * t;
            break;
        case AnimationType::Maximize:
        case AnimationType::RestoreFromMaximized:
            p.opacity = std::min(1.0f, t * 2.5f);
            break;
        default:
            break;
    }
    return p;
}

static RECT RectFor(AnimationType type, float t, const RECT& windowRect, const RECT& destRect) {
    t = std::clamp(t, 0.0f, 1.0f);
    switch (type) {
        case AnimationType::Minimize:
            return LerpRect(windowRect, destRect, t);
        case AnimationType::RestoreFromMinimized:
            return LerpRect(destRect, windowRect, t);
        case AnimationType::Maximize:
        case AnimationType::RestoreFromMaximized:
            return LerpRect(windowRect, destRect, t);
        default:
            return windowRect;
    }
}

static UINT DurationMsFor(AnimationType type, int scalePercent) {
    double sec = kShowHideDurationSec;
    if (type == AnimationType::Maximize || type == AnimationType::RestoreFromMaximized) {
        sec = kRectMoveDurationSec;
    }
    double ms = sec * 1000.0 * (scalePercent / 100.0);
    if (ms < 16.0) {
        ms = 16.0;
    }
    return static_cast<UINT>(std::lround(ms));
}

static Matrix4x4F BuildCornerMatrix(const Win7TransformParams& p,
                                    const RECT& rcCurrent,
                                    float originalWidth,
                                    float originalHeight) {
    const float width = static_cast<float>(RECTW(rcCurrent));
    const float height = static_cast<float>(RECTH(rcCurrent));
    const float scaleX = originalWidth > 1.0f ? width / originalWidth : 1.0f;
    const float scaleY = originalHeight > 1.0f ? height / originalHeight : 1.0f;
    const float cx = originalWidth * 0.5f;
    const float cy = originalHeight * 0.5f;

    const Matrix4x4F toCenter = Matrix4x4F::Translation(-cx, -cy, 0.0f);
    const Matrix4x4F rot = Matrix4x4F::RotationX(-p.rotX) * Matrix4x4F::RotationY(-p.rotY);
    const Matrix4x4F fromCenter = Matrix4x4F::Translation(cx, cy, 0.0f);
    const Matrix4x4F scale = Matrix4x4F::Scale(scaleX, scaleY, 1.0f);
    const Matrix4x4F place = Matrix4x4F::Translation(static_cast<float>(rcCurrent.left),
                                                     static_cast<float>(rcCurrent.top), 0.0f);
    Matrix4x4F m = toCenter * rot * fromCenter * scale * place;
    const float invH = 1.0f / std::fmax(originalHeight, 1.0f);
    m._43 += p.transZ;
    m._44 += -p.transZ * invH;
    return m;
}

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

bool g_animateOpenClose = false;
bool g_animateMinimize = true;
bool g_animateMaximize = false;
bool g_respectMinAnimate = true;
int g_durationScale = 100;
wchar_t g_exeName[MAX_PATH] = L"?";

static void LoadSettings() {
    g_animateOpenClose = Wh_GetIntSetting(L"animateOpenClose") != 0;
    g_animateMinimize = Wh_GetIntSetting(L"animateMinimize") != 0;
    g_animateMaximize = Wh_GetIntSetting(L"animateMaximize") != 0;
    g_respectMinAnimate = Wh_GetIntSetting(L"respectMinAnimate") != 0;
    g_durationScale = Wh_GetIntSetting(L"durationScale");
    if (g_durationScale < 10) {
        g_durationScale = 10;
    }
    if (g_durationScale > 800) {
        g_durationScale = 800;
    }
    Wh_Log(L"[%s] Settings: openClose=%d minimize=%d maximize=%d respect=%d scale=%d",
           g_exeName, (int)g_animateOpenClose, (int)g_animateMinimize, (int)g_animateMaximize,
           (int)g_respectMinAnimate, g_durationScale);
}

static bool IsMinAnimateEnabled() {
    if (!g_respectMinAnimate) {
        return true;
    }
    ANIMATIONINFO ai{};
    ai.cbSize = sizeof(ai);
    if (SystemParametersInfoW(SPI_GETANIMATION, sizeof(ai), &ai, 0)) {
        return ai.iMinAnimate != 0;
    }
    return true;
}

// ---------------------------------------------------------------------------
// Undocumented but exported user32 API.
// ---------------------------------------------------------------------------

typedef BOOL(WINAPI* GetWindowMinimizeRect_t)(HWND hwnd, LPRECT prcMin);
GetWindowMinimizeRect_t pGetWindowMinimizeRect = nullptr;

using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t ShowWindow_orig = nullptr;

// ---------------------------------------------------------------------------
// Capture
// ---------------------------------------------------------------------------

struct CaptureBits {
    std::vector<uint32_t> pixels;
    int width = 0;
    int height = 0;
    bool empty() const { return pixels.empty() || width <= 0 || height <= 0; }
};

static std::mutex g_cacheMutex;
static std::unordered_map<HWND, CaptureBits> g_captureCache;

static void CacheCapture(HWND hwnd, const CaptureBits& bits) {
    if (!hwnd || bits.empty()) {
        return;
    }
    try {
        std::lock_guard<std::mutex> lock(g_cacheMutex);
        g_captureCache[hwnd] = bits;
    } catch (const std::exception& ex) {
        Wh_Log(L"[%s] CacheCapture: %S", g_exeName, ex.what());
    }
}

static bool TakeCachedCapture(HWND hwnd, CaptureBits& out) {
    try {
        std::lock_guard<std::mutex> lock(g_cacheMutex);
        auto it = g_captureCache.find(hwnd);
        if (it == g_captureCache.end()) {
            return false;
        }
        out = std::move(it->second);
        g_captureCache.erase(it);
        return !out.empty();
    } catch (const std::exception& ex) {
        Wh_Log(L"[%s] TakeCachedCapture: %S", g_exeName, ex.what());
        return false;
    }
}

static void ForgetCapture(HWND hwnd) {
    try {
        std::lock_guard<std::mutex> lock(g_cacheMutex);
        g_captureCache.erase(hwnd);
    } catch (const std::exception&) {
    }
}

static void ForceOpaqueAlpha(uint32_t* pixels, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        pixels[i] |= 0xFF000000u;
    }
}

static bool CaptureWindow(HWND hwnd, CaptureBits& out, bool allowScreenBlt) {
    if (!hwnd || !IsWindow(hwnd)) {
        return false;
    }
    RECT rc{};
    if (!GetWindowRect(hwnd, &rc) || !IsRectUsable(rc)) {
        return false;
    }
    const int width = RECTW(rc);
    const int height = RECTH(rc);
    if (width < 1 || height < 1 || width > 16384 || height > 16384) {
        return false;
    }

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    ScopedWindowDc winDc(nullptr, GetDC(nullptr));
    if (!winDc) {
        return false;
    }
    ScopedGdiObj dib(CreateDIBSection(winDc.get(), &bmi, DIB_RGB_COLORS, &bits, nullptr, 0));
    if (!dib || !bits) {
        return false;
    }
    ScopedDc memDc(CreateCompatibleDC(winDc.get()));
    if (!memDc) {
        return false;
    }
    ScopedSelect select(memDc.get(), dib.get());

    bool painted = false;
    if (allowScreenBlt) {
        painted = BitBlt(memDc.get(), 0, 0, width, height, winDc.get(), rc.left, rc.top, SRCCOPY) !=
                  FALSE;
    }
    if (!painted) {
        painted = PrintWindow(hwnd, memDc.get(), PW_RENDERFULLCONTENT) != FALSE;
    }
    if (!painted) {
        painted = PrintWindow(hwnd, memDc.get(), 0) != FALSE;
    }
    if (!painted) {
        return false;
    }

    try {
        out.width = width;
        out.height = height;
        out.pixels.resize(static_cast<size_t>(width) * static_cast<size_t>(height));
        std::memcpy(out.pixels.data(), bits, out.pixels.size() * sizeof(uint32_t));
        ForceOpaqueAlpha(out.pixels.data(), out.pixels.size());
    } catch (const std::exception& ex) {
        Wh_Log(L"[%s] CaptureWindow alloc: %S", g_exeName, ex.what());
        out = {};
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------
// DPI
// ---------------------------------------------------------------------------

static bool IsWindowPerMonitorDpiAware(HWND hwnd) {
    const DPI_AWARENESS_CONTEXT ctx = GetWindowDpiAwarenessContext(hwnd);
    return AreDpiAwarenessContextsEqual(ctx, DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE) ||
           AreDpiAwarenessContextsEqual(ctx, DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
}

static void RectToPhysical(HWND hwnd, RECT* rc) {
    if (!rc || IsWindowPerMonitorDpiAware(hwnd)) {
        return;
    }
    const UINT dpiWnd = GetDpiForWindow(hwnd);
    if (dpiWnd == 0) {
        return;
    }
    HMONITOR hmon = MonitorFromRect(rc, MONITOR_DEFAULTTONEAREST);
    UINT dpiX = 96, dpiY = 96;
    if (FAILED(GetDpiForMonitor(hmon, MDT_EFFECTIVE_DPI, &dpiX, &dpiY)) || dpiX == 0 || dpiY == 0) {
        return;
    }
    if (dpiX == dpiWnd && dpiY == dpiWnd) {
        return;
    }
    const LONG w = RECTW(*rc);
    const LONG h = RECTH(*rc);
    rc->left = MulDiv(rc->left, dpiX, dpiWnd);
    rc->top = MulDiv(rc->top, dpiY, dpiWnd);
    rc->right = rc->left + MulDiv(w, dpiX, dpiWnd);
    rc->bottom = rc->top + MulDiv(h, dpiY, dpiWnd);
}

static bool GetMinimizeRectPhysical(HWND hwnd, RECT* rc) {
    if (pGetWindowMinimizeRect && pGetWindowMinimizeRect(hwnd, rc) && IsRectUsable(*rc)) {
        RectToPhysical(hwnd, rc);
        return true;
    }
    // Fallback if the export is gone: a 24×24 square at the bottom of the work area.
    HMONITOR hmon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{sizeof(mi)};
    if (!GetMonitorInfoW(hmon, &mi)) {
        return false;
    }
    rc->left = mi.rcWork.left + 8;
    rc->bottom = mi.rcWork.bottom - 8;
    rc->right = rc->left + 24;
    rc->top = rc->bottom - 24;
    RectToPhysical(hwnd, rc);
    return IsRectUsable(*rc);
}

static bool GetMaximizeRectPhysical(HWND hwnd, RECT* rc) {
    MINMAXINFO mmi{};
    HMONITOR hmon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{sizeof(mi)};
    if (!GetMonitorInfoW(hmon, &mi)) {
        return false;
    }
    const RECT work = mi.rcWork;
    mmi.ptMaxPosition.x = work.left;
    mmi.ptMaxPosition.y = work.top;
    mmi.ptMaxSize.x = RECTW(work);
    mmi.ptMaxSize.y = RECTH(work);
    SendMessageW(hwnd, WM_GETMINMAXINFO, 0, reinterpret_cast<LPARAM>(&mmi));
    rc->left = mmi.ptMaxPosition.x;
    rc->top = mmi.ptMaxPosition.y;
    rc->right = mmi.ptMaxPosition.x + mmi.ptMaxSize.x;
    rc->bottom = mmi.ptMaxPosition.y + mmi.ptMaxSize.y;
    RectToPhysical(hwnd, rc);
    return IsRectUsable(*rc);
}

static bool GetRestoreRectPhysical(HWND hwnd, RECT* rc) {
    if (!rc) {
        return false;
    }
    WINDOWPLACEMENT wp{sizeof(wp)};
    if (!GetWindowPlacement(hwnd, &wp)) {
        return false;
    }
    *rc = wp.rcNormalPosition;
    const LONG style = static_cast<LONG>(GetWindowLongPtrW(hwnd, GWL_STYLE));
    if (!(style & WS_CHILD)) {
        HMONITOR hmon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi{sizeof(mi)};
        if (hmon && GetMonitorInfoW(hmon, &mi)) {
            OffsetRect(rc, -(mi.rcMonitor.left - mi.rcWork.left),
                       -(mi.rcMonitor.top - mi.rcWork.top));
        }
    } else if (HWND parent = GetParent(hwnd)) {
        MapWindowPoints(parent, HWND_DESKTOP, reinterpret_cast<LPPOINT>(rc), 2);
    }
    RectToPhysical(hwnd, rc);
    return IsRectUsable(*rc);
}

// ---------------------------------------------------------------------------
// Never DWMWA_CLOAK. Cloak empties Explorer, Control Panel, Task Manager
// and other apps (blank white client). Overlay + TRANSITIONS_FORCEDISABLED
// is enough: minimize orig() under the overlay, restore orig() only after
// the overlay has grown to full size.
// ---------------------------------------------------------------------------

static void DisableTransitions(HWND hwnd, BOOL disable) {
    if (!hwnd || !IsWindow(hwnd)) {
        return;
    }
    DwmSetWindowAttribute(hwnd, DWMWA_TRANSITIONS_FORCEDISABLED, &disable, sizeof(disable));
}

// ---------------------------------------------------------------------------
// Software textured-quad rasterizer
// ---------------------------------------------------------------------------

struct Vertex {
    float x, y, u, v;
};

static float Edge(const Vertex& a, const Vertex& b, const Vertex& c) {
    return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}

static uint32_t SampleBilinear(const uint32_t* src, int sw, int sh, float u, float v) {
    u = std::clamp(u, 0.0f, 1.0f) * static_cast<float>(sw - 1);
    v = std::clamp(v, 0.0f, 1.0f) * static_cast<float>(sh - 1);
    const int x0 = std::clamp(static_cast<int>(u), 0, sw - 1);
    const int y0 = std::clamp(static_cast<int>(v), 0, sh - 1);
    const int x1 = std::min(x0 + 1, sw - 1);
    const int y1 = std::min(y0 + 1, sh - 1);
    const float fx = u - static_cast<float>(x0);
    const float fy = v - static_cast<float>(y0);

    auto unpack = [](uint32_t p, float& b, float& g, float& r) {
        b = static_cast<float>(p & 0xFF);
        g = static_cast<float>((p >> 8) & 0xFF);
        r = static_cast<float>((p >> 16) & 0xFF);
    };
    float b00, g00, r00, b10, g10, r10, b01, g01, r01, b11, g11, r11;
    unpack(src[y0 * sw + x0], b00, g00, r00);
    unpack(src[y0 * sw + x1], b10, g10, r10);
    unpack(src[y1 * sw + x0], b01, g01, r01);
    unpack(src[y1 * sw + x1], b11, g11, r11);

    const float b0 = b00 + (b10 - b00) * fx;
    const float g0 = g00 + (g10 - g00) * fx;
    const float r0 = r00 + (r10 - r00) * fx;
    const float b1 = b01 + (b11 - b01) * fx;
    const float g1 = g01 + (g11 - g01) * fx;
    const float r1 = r01 + (r11 - r01) * fx;
    const BYTE b = static_cast<BYTE>(b0 + (b1 - b0) * fy + 0.5f);
    const BYTE g = static_cast<BYTE>(g0 + (g1 - g0) * fy + 0.5f);
    const BYTE r = static_cast<BYTE>(r0 + (r1 - r0) * fy + 0.5f);
    return static_cast<uint32_t>(b) | (static_cast<uint32_t>(g) << 8) | (static_cast<uint32_t>(r) << 16);
}

static void RasterTriangle(uint32_t* dst,
                           int dw,
                           int dh,
                           const uint32_t* src,
                           int sw,
                           int sh,
                           Vertex v0,
                           Vertex v1,
                           Vertex v2,
                           BYTE alpha) {
    const float area = Edge(v0, v1, v2);
    if (std::fabs(area) < 0.5f) {
        return;
    }
    const float invArea = 1.0f / area;
    int minX = static_cast<int>(std::floor(std::min({v0.x, v1.x, v2.x})));
    int maxX = static_cast<int>(std::ceil(std::max({v0.x, v1.x, v2.x})));
    int minY = static_cast<int>(std::floor(std::min({v0.y, v1.y, v2.y})));
    int maxY = static_cast<int>(std::ceil(std::max({v0.y, v1.y, v2.y})));
    minX = std::clamp(minX, 0, dw - 1);
    maxX = std::clamp(maxX, 0, dw - 1);
    minY = std::clamp(minY, 0, dh - 1);
    maxY = std::clamp(maxY, 0, dh - 1);
    const float af = static_cast<float>(alpha) / 255.0f;
    for (int y = minY; y <= maxY; ++y) {
        uint32_t* row = dst + static_cast<size_t>(y) * static_cast<size_t>(dw);
        for (int x = minX; x <= maxX; ++x) {
            Vertex p{static_cast<float>(x) + 0.5f, static_cast<float>(y) + 0.5f, 0, 0};
            const float w0 = Edge(v1, v2, p) * invArea;
            const float w1 = Edge(v2, v0, p) * invArea;
            const float w2 = Edge(v0, v1, p) * invArea;
            if (w0 < 0.0f || w1 < 0.0f || w2 < 0.0f) {
                continue;
            }
            const float u = w0 * v0.u + w1 * v1.u + w2 * v2.u;
            const float v = w0 * v0.v + w1 * v1.v + w2 * v2.v;
            const uint32_t s = SampleBilinear(src, sw, sh, u, v);
            const float sb = static_cast<float>(s & 0xFF);
            const float sg = static_cast<float>((s >> 8) & 0xFF);
            const float sr = static_cast<float>((s >> 16) & 0xFF);
            const BYTE b = static_cast<BYTE>(sb * af + 0.5f);
            const BYTE g = static_cast<BYTE>(sg * af + 0.5f);
            const BYTE r = static_cast<BYTE>(sr * af + 0.5f);
            row[x] = static_cast<uint32_t>(b) | (static_cast<uint32_t>(g) << 8) |
                     (static_cast<uint32_t>(r) << 16) | (static_cast<uint32_t>(alpha) << 24);
        }
    }
}

static void RasterQuad(uint32_t* dst,
                       int dw,
                       int dh,
                       const uint32_t* src,
                       int sw,
                       int sh,
                       const Vertex corners[4],
                       BYTE alpha) {
    if (dw <= 0 || dh <= 0 || sw <= 0 || sh <= 0 || !dst || !src || alpha == 0) {
        return;
    }
    RasterTriangle(dst, dw, dh, src, sw, sh, corners[0], corners[1], corners[2], alpha);
    RasterTriangle(dst, dw, dh, src, sw, sh, corners[0], corners[2], corners[3], alpha);
}

static bool BlitAxisAligned(HDC hdcDst, int dw, int dh, const CaptureBits& cap, const RECT& rcLocal) {
    if (dw <= 0 || dh <= 0) {
        return true;
    }
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = cap.width;
    bmi.bmiHeader.biHeight = -cap.height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    ScopedDc hdcSrc(CreateCompatibleDC(hdcDst));
    if (!hdcSrc) {
        return false;
    }
    void* bits = nullptr;
    ScopedGdiObj dib(CreateDIBSection(hdcDst, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0));
    if (!dib || !bits) {
        return false;
    }
    std::memcpy(bits, cap.pixels.data(), cap.pixels.size() * sizeof(uint32_t));
    ScopedSelect select(hdcSrc.get(), dib.get());
    SetStretchBltMode(hdcDst, HALFTONE);
    SetBrushOrgEx(hdcDst, 0, 0, nullptr);
    return StretchBlt(hdcDst, rcLocal.left, rcLocal.top, RECTW(rcLocal), RECTH(rcLocal), hdcSrc.get(), 0,
                      0, cap.width, cap.height, SRCCOPY) != FALSE;
}

// ---------------------------------------------------------------------------
// Overlay present
// ---------------------------------------------------------------------------

static bool PresentOverlay(HWND hwndOverlay,
                           const CaptureBits& cap,
                           const RECT& rcCurrent,
                           const Win7TransformParams& params,
                           float originalWidth,
                           float originalHeight) {
    const BYTE alpha = static_cast<BYTE>(std::clamp(params.opacity, 0.0f, 1.0f) * 255.0f + 0.5f);
    if (alpha == 0 || !IsRectUsable(rcCurrent)) {
        POINT pt{rcCurrent.left, rcCurrent.top};
        SIZE sz{1, 1};
        BLENDFUNCTION bf{AC_SRC_OVER, 0, 0, AC_SRC_ALPHA};
        UpdateLayeredWindow(hwndOverlay, nullptr, &pt, &sz, nullptr, nullptr, 0, &bf, ULW_ALPHA);
        return true;
    }

    const bool tiny3d = std::fabs(params.rotX) < 0.35f && std::fabs(params.rotY) < 0.35f &&
                        std::fabs(params.transZ) < 0.25f;

    RECT bbox = rcCurrent;
    Vertex corners[4]{};
    if (!tiny3d) {
        const Matrix4x4F m = BuildCornerMatrix(params, rcCurrent, originalWidth, originalHeight);
        const float xs[4] = {0.0f, originalWidth, originalWidth, 0.0f};
        const float ys[4] = {0.0f, 0.0f, originalHeight, originalHeight};
        const float us[4] = {0.0f, 1.0f, 1.0f, 0.0f};
        const float vs[4] = {0.0f, 0.0f, 1.0f, 1.0f};
        float minX = 1e9f, minY = 1e9f, maxX = -1e9f, maxY = -1e9f;
        float sx[4], sy[4];
        for (int i = 0; i < 4; ++i) {
            m.TransformPoint(xs[i], ys[i], 0.0f, sx[i], sy[i]);
            minX = std::min(minX, sx[i]);
            minY = std::min(minY, sy[i]);
            maxX = std::max(maxX, sx[i]);
            maxY = std::max(maxY, sy[i]);
        }
        bbox.left = static_cast<LONG>(std::floor(minX)) - 1;
        bbox.top = static_cast<LONG>(std::floor(minY)) - 1;
        bbox.right = static_cast<LONG>(std::ceil(maxX)) + 1;
        bbox.bottom = static_cast<LONG>(std::ceil(maxY)) + 1;
        for (int i = 0; i < 4; ++i) {
            corners[i] = {sx[i] - bbox.left, sy[i] - bbox.top, us[i], vs[i]};
        }
    }

    if (!IsRectUsable(bbox)) {
        return false;
    }
    const int dw = RECTW(bbox);
    const int dh = RECTH(bbox);
    if (dw > 16384 || dh > 16384) {
        return false;
    }

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = dw;
    bmi.bmiHeader.biHeight = -dh;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    ScopedWindowDc screenDc(nullptr, GetDC(nullptr));
    if (!screenDc) {
        return false;
    }
    void* bits = nullptr;
    ScopedGdiObj dib(CreateDIBSection(screenDc.get(), &bmi, DIB_RGB_COLORS, &bits, nullptr, 0));
    if (!dib || !bits) {
        return false;
    }
    std::memset(bits, 0, static_cast<size_t>(dw) * static_cast<size_t>(dh) * 4);
    ScopedDc memDc(CreateCompatibleDC(screenDc.get()));
    if (!memDc) {
        return false;
    }
    ScopedSelect select(memDc.get(), dib.get());

    if (tiny3d) {
        RECT local{0, 0, dw, dh};
        if (!BlitAxisAligned(memDc.get(), dw, dh, cap, local)) {
            return false;
        }
        auto* px = static_cast<uint32_t*>(bits);
        const size_t n = static_cast<size_t>(dw) * static_cast<size_t>(dh);
        for (size_t i = 0; i < n; ++i) {
            const uint32_t p = px[i];
            const BYTE b = static_cast<BYTE>(((p & 0xFF) * alpha) / 255);
            const BYTE g = static_cast<BYTE>((((p >> 8) & 0xFF) * alpha) / 255);
            const BYTE r = static_cast<BYTE>((((p >> 16) & 0xFF) * alpha) / 255);
            px[i] = static_cast<uint32_t>(b) | (static_cast<uint32_t>(g) << 8) |
                    (static_cast<uint32_t>(r) << 16) | (static_cast<uint32_t>(alpha) << 24);
        }
    } else {
        RasterQuad(static_cast<uint32_t*>(bits), dw, dh, cap.pixels.data(), cap.width, cap.height,
                   corners, alpha);
    }

    POINT pt{bbox.left, bbox.top};
    SIZE sz{dw, dh};
    POINT srcPt{0, 0};
    BLENDFUNCTION bf{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    if (!UpdateLayeredWindow(hwndOverlay, screenDc.get(), &pt, &sz, memDc.get(), &srcPt, 0, &bf,
                             ULW_ALPHA)) {
        Wh_Log(L"[%s] UpdateLayeredWindow failed: %u", g_exeName, GetLastError());
        return false;
    }
    return true;
}

static void HideOverlayWindow(HWND hwndOverlay) {
    POINT pt{0, 0};
    SIZE sz{1, 1};
    BLENDFUNCTION bf{AC_SRC_OVER, 0, 0, AC_SRC_ALPHA};
    UpdateLayeredWindow(hwndOverlay, nullptr, &pt, &sz, nullptr, nullptr, 0, &bf, ULW_ALPHA);
    SetWindowPos(hwndOverlay, HWND_BOTTOM, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_HIDEWINDOW);
}

struct AnimRequest {
    HWND hwnd = nullptr;
    AnimationType type = AnimationType::None;
    RECT rcWindow{};
    RECT rcDest{};
    CaptureBits capture;
    UINT durationMs = 250;
    bool deferOrig = false;   // restore: orig() after last overlay frame
    int origShowCmd = 0;
};

enum class AnimMsg : UINT {
    FirstFrame = 1,  // lParam = AnimRequest* (caller-owned, sync)
    Drain = 2,       // pop the queue
    Hide = 3,
};

static UINT g_msgAnim = 0;
static bool g_fAnimating = false;
static bool g_fDisabled = false;
static HINSTANCE g_hinst = nullptr;
static HWND g_hwndAnim = nullptr;
static HANDLE g_hAnimWndThread = nullptr;
static HWND g_hwndCurrent = nullptr;
static AnimationType g_typeCurrent = AnimationType::None;

static std::mutex g_queueMutex;
static std::deque<AnimRequest*> g_queue;

static void PresentTime(HWND hwndOverlay, const AnimRequest& req, float t) {
    const Win7TransformParams params = ParamsFor(req.type, t);
    const RECT rc = RectFor(req.type, t, req.rcWindow, req.rcDest);
    PresentOverlay(hwndOverlay, req.capture, rc, params, static_cast<float>(req.capture.width),
                   static_cast<float>(req.capture.height));
}

static void PresentFirstFrame(HWND hwndOverlay, const AnimRequest& req) {
    if (req.capture.empty() || !IsRectUsable(req.rcWindow)) {
        return;
    }
    if (ShowWindow_orig) {
        ShowWindow_orig(hwndOverlay, SW_SHOWNA);
    } else {
        ::ShowWindow(hwndOverlay, SW_SHOWNA);
    }
    SetWindowPos(hwndOverlay, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    PresentTime(hwndOverlay, req, 0.0f);
}

static void RunAnimation(HWND hwndOverlay, const AnimRequest& req) {
    if (req.capture.empty() || !IsRectUsable(req.rcWindow)) {
        return;
    }
    const UINT durationMs = req.durationMs ? req.durationMs : 250;
    if (ShowWindow_orig) {
        ShowWindow_orig(hwndOverlay, SW_SHOWNA);
    } else {
        ::ShowWindow(hwndOverlay, SW_SHOWNA);
    }
    const ULONGLONG start = GetTickCount64();
    ULONGLONG elapsed = 0;
    float lastT = -1.0f;
    while ((elapsed = GetTickCount64() - start) <= durationMs) {
        const float t = durationMs == 0
                            ? 1.0f
                            : std::clamp(static_cast<float>(elapsed) / static_cast<float>(durationMs),
                                         0.0f, 1.0f);
        if (t - lastT < 0.001f && elapsed + 8 < durationMs) {
            Sleep(1);
            continue;
        }
        lastT = t;
        PresentTime(hwndOverlay, req, t);
        Sleep(1);
    }
    PresentTime(hwndOverlay, req, 1.0f);
}

static void FinishQueued(AnimRequest* req) {
    if (!req) {
        return;
    }
    // Restore: window stayed iconic the whole time. Show it under the last
    // overlay frame, then drop the overlay. No cloak, so the client is intact.
    if (req->deferOrig && req->hwnd && IsWindow(req->hwnd) && ShowWindow_orig) {
        const int cmd = req->origShowCmd ? req->origShowCmd : SW_RESTORE;
        ShowWindow_orig(req->hwnd, cmd);
        DwmFlush();
        Sleep(16);
    }
    if (req->hwnd && IsWindow(req->hwnd)) {
        DisableTransitions(req->hwnd, FALSE);
    }
    if (g_hwndAnim && IsWindow(g_hwndAnim)) {
        HideOverlayWindow(g_hwndAnim);
    }
    if (g_hwndCurrent == req->hwnd) {
        g_hwndCurrent = nullptr;
        g_typeCurrent = AnimationType::None;
        g_fAnimating = false;
    }
    delete req;
}

static void DrainQueue(HWND hwndOverlay) {
    for (;;) {
        AnimRequest* req = nullptr;
        {
            std::lock_guard<std::mutex> lock(g_queueMutex);
            if (g_queue.empty()) {
                break;
            }
            req = g_queue.front();
            g_queue.pop_front();
        }
        if (!req) {
            continue;
        }
        RunAnimation(hwndOverlay, *req);
        FinishQueued(req);
    }
}

static const wchar_t kAnimClassName[] = L"Windhawk_Win7AeroAnim";

static LRESULT CALLBACK AnimWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (g_msgAnim && uMsg == g_msgAnim) {
        switch (static_cast<AnimMsg>(wParam)) {
            case AnimMsg::FirstFrame:
                if (lParam) {
                    PresentFirstFrame(hwnd, *reinterpret_cast<const AnimRequest*>(lParam));
                }
                break;
            case AnimMsg::Drain:
                DrainQueue(hwnd);
                break;
            case AnimMsg::Hide:
                HideOverlayWindow(hwnd);
                break;
        }
        return 0;
    }
    switch (uMsg) {
        case WM_PAINT:
        case WM_ERASEBKGND:
            return 0;
        case WM_NCHITTEST:
            return HTNOWHERE;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
}

static DWORD CALLBACK AnimWndThreadProc(HANDLE hEvent) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    WNDCLASSW wc{};
    wc.lpfnWndProc = AnimWndProc;
    wc.hInstance = g_hinst;
    wc.lpszClassName = kAnimClassName;
    RegisterClassW(&wc);
    g_hwndAnim = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kAnimClassName,
                                 nullptr, WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, g_hinst, nullptr);
    if (g_hwndAnim) {
        const LONG_PTR ex = GetWindowLongPtrW(g_hwndAnim, GWL_EXSTYLE);
        SetWindowLongPtrW(g_hwndAnim, GWL_EXSTYLE, ex | WS_EX_LAYERED | WS_EX_TRANSPARENT);
    }
    g_msgAnim = RegisterWindowMessageW(L"Windhawk_Win7AeroAnim_Run");
    SetEvent(hEvent);
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    g_hwndAnim = nullptr;
    return 0;
}

static bool WaitForAnimWndThread() {
    if (g_hAnimWndThread && g_hwndAnim && IsWindow(g_hwndAnim)) {
        return true;
    }
    HANDLE hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!hEvent) {
        return false;
    }
    g_hAnimWndThread = CreateThread(nullptr, 0, AnimWndThreadProc, hEvent, 0, nullptr);
    const DWORD wait = WaitForSingleObject(hEvent, 1000);
    CloseHandle(hEvent);
    if (wait != WAIT_OBJECT_0 || !g_hwndAnim) {
        MessageBoxW(nullptr,
                    L"Creating the animation overlay window failed. The mod has been "
                    L"disabled for this process so it cannot crash it.",
                    L"Windhawk: Windows 7 Window Animations", MB_ICONERROR | MB_OK);
        g_fDisabled = true;
        Wh_Log(L"[%s] Overlay window was not created within 1s, disabling", g_exeName);
        return false;
    }
    Sleep(30);
    return true;
}

static bool QueueRun(AnimRequest&& req) {
    AnimRequest* heap = nullptr;
    try {
        heap = new AnimRequest(std::move(req));
    } catch (const std::exception& ex) {
        Wh_Log(L"[%s] QueueRun alloc: %S", g_exeName, ex.what());
        return false;
    }
    {
        std::lock_guard<std::mutex> lock(g_queueMutex);
        g_queue.push_back(heap);
    }
    if (!PostMessageW(g_hwndAnim, g_msgAnim, static_cast<WPARAM>(AnimMsg::Drain), 0)) {
        Wh_Log(L"[%s] PostMessage Drain failed: %u", g_exeName, GetLastError());
        std::lock_guard<std::mutex> lock(g_queueMutex);
        if (!g_queue.empty() && g_queue.back() == heap) {
            g_queue.pop_back();
        }
        delete heap;
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------
// Who gets an animation
// ---------------------------------------------------------------------------

static bool ShouldAnimateWindow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd) || g_fDisabled || g_fAnimating) {
        return false;
    }
    if (!IsWindowVisible(hwnd) || IsIconic(hwnd)) {
        return false;
    }
    if (GetAncestor(hwnd, GA_ROOT) != hwnd) {
        const LONG style = static_cast<LONG>(GetWindowLongPtrW(hwnd, GWL_STYLE));
        if (!(style & WS_CHILD) || !(style & WS_CAPTION)) {
            return false;
        }
    }
    const LONG style = static_cast<LONG>(GetWindowLongPtrW(hwnd, GWL_STYLE));
    const LONG ex = static_cast<LONG>(GetWindowLongPtrW(hwnd, GWL_EXSTYLE));
    if (!(style & WS_CAPTION)) {
        return false;
    }
    if (ex & WS_EX_TOOLWINDOW) {
        return false;
    }
    if (ex & WS_EX_NOACTIVATE) {
        return false;
    }
    if (hwnd == g_hwndAnim) {
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------
// Play. Never cloak.
// Minimize: overlay t=0 → orig() under it → overlay shrinks → hide overlay.
// Restore: overlay grows from the taskbar while the window stays iconic →
// orig() under the last frame → hide overlay.
// ---------------------------------------------------------------------------

static bool BeginAnimation(HWND hwnd,
                           AnimationType type,
                           const RECT& rcWindow,
                           const RECT& rcDest,
                           CaptureBits&& capture,
                           bool deferOrig,
                           int origShowCmd) {
    if (!hwnd || !IsWindow(hwnd) || capture.empty() || !WaitForAnimWndThread()) {
        return false;
    }

    AnimRequest req;
    req.hwnd = hwnd;
    req.type = type;
    req.rcWindow = rcWindow;
    req.rcDest = rcDest;
    req.capture = std::move(capture);
    req.durationMs = DurationMsFor(type, g_durationScale);
    req.deferOrig = deferOrig;
    req.origShowCmd = origShowCmd;

    g_fAnimating = true;
    g_hwndCurrent = hwnd;
    g_typeCurrent = type;

    SendMessageW(g_hwndAnim, g_msgAnim, static_cast<WPARAM>(AnimMsg::FirstFrame),
                 reinterpret_cast<LPARAM>(&req));

    DisableTransitions(hwnd, TRUE);

    if (!QueueRun(std::move(req))) {
        DisableTransitions(hwnd, FALSE);
        HideOverlayWindow(g_hwndAnim);
        g_fAnimating = false;
        g_hwndCurrent = nullptr;
        g_typeCurrent = AnimationType::None;
        return false;
    }
    return true;
}

static void AfterOrigMinimize(HWND hwnd) {
    if (!g_fAnimating || g_hwndCurrent != hwnd || !hwnd || !IsWindow(hwnd)) {
        return;
    }
    DisableTransitions(hwnd, TRUE);
    if (g_typeCurrent == AnimationType::Minimize && !IsIconic(hwnd) && ShowWindow_orig) {
        Wh_Log(L"[%s] Minimize did not stick, SW_FORCEMINIMIZE hwnd=%p", g_exeName, hwnd);
        ShowWindow_orig(hwnd, SW_FORCEMINIMIZE);
    }
}

static bool PlayMinimize(HWND hwnd) {
    if (!g_animateMinimize || !IsMinAnimateEnabled() || !ShouldAnimateWindow(hwnd)) {
        return false;
    }
    const LONG style = static_cast<LONG>(GetWindowLongPtrW(hwnd, GWL_STYLE));
    if (style & WS_MINIMIZE) {
        return false;
    }
    RECT rcWindow{};
    if (!GetWindowRect(hwnd, &rcWindow)) {
        return false;
    }
    RectToPhysical(hwnd, &rcWindow);
    RECT rcMin{};
    if (!GetMinimizeRectPhysical(hwnd, &rcMin)) {
        Wh_Log(L"[%s] GetWindowMinimizeRect failed", g_exeName);
        return false;
    }
    rcMin = AspectCorrectedMinimizeTarget(rcMin);
    CaptureBits cap;
    if (!CaptureWindow(hwnd, cap, /*allowScreenBlt=*/true)) {
        Wh_Log(L"[%s] CaptureWindow failed on minimize", g_exeName);
        return false;
    }
    CacheCapture(hwnd, cap);
    Wh_Log(L"[%s] Minimize hwnd=%p src=(%d,%d)-(%d,%d) dest=(%d,%d)-(%d,%d)", g_exeName, hwnd,
           rcWindow.left, rcWindow.top, rcWindow.right, rcWindow.bottom, rcMin.left, rcMin.top,
           rcMin.right, rcMin.bottom);
    return BeginAnimation(hwnd, AnimationType::Minimize, rcWindow, rcMin, std::move(cap),
                          /*deferOrig=*/false, 0);
}

static bool PlayRestore(HWND hwnd) {
    if (!g_animateMinimize || !IsMinAnimateEnabled()) {
        return false;
    }
    const LONG style = static_cast<LONG>(GetWindowLongPtrW(hwnd, GWL_STYLE));
    if (style & WS_MINIMIZE) {
        if (g_fDisabled || g_fAnimating) {
            return false;
        }
        RECT rcMin{};
        if (!GetMinimizeRectPhysical(hwnd, &rcMin)) {
            return false;
        }
        rcMin = AspectCorrectedMinimizeTarget(rcMin);
        RECT rcRest{};
        bool haveRest = false;
        WINDOWPLACEMENT wp{sizeof(wp)};
        if (GetWindowPlacement(hwnd, &wp) && (wp.flags & WPF_RESTORETOMAXIMIZED)) {
            haveRest = GetMaximizeRectPhysical(hwnd, &rcRest);
        } else {
            haveRest = GetRestoreRectPhysical(hwnd, &rcRest);
        }
        if (!haveRest || !IsRectUsable(rcRest)) {
            return false;
        }
        CaptureBits cap;
        if (!TakeCachedCapture(hwnd, cap)) {
            Wh_Log(L"[%s] No cached capture for restore, skipping", g_exeName);
            return false;
        }
        return BeginAnimation(hwnd, AnimationType::RestoreFromMinimized, rcRest, rcMin, std::move(cap),
                              /*deferOrig=*/true, SW_RESTORE);
    }
    if (style & WS_MAXIMIZE) {
        if (!g_animateMaximize || !ShouldAnimateWindow(hwnd)) {
            return false;
        }
        RECT rcFrom{};
        GetWindowRect(hwnd, &rcFrom);
        RectToPhysical(hwnd, &rcFrom);
        RECT rcTo{};
        if (!GetRestoreRectPhysical(hwnd, &rcTo)) {
            return false;
        }
        CaptureBits cap;
        if (!CaptureWindow(hwnd, cap, true)) {
            return false;
        }
        return BeginAnimation(hwnd, AnimationType::RestoreFromMaximized, rcFrom, rcTo, std::move(cap),
                              /*deferOrig=*/false, 0);
    }
    return false;
}

static bool PlayMaximize(HWND hwnd) {
    if (!g_animateMaximize || !IsMinAnimateEnabled() || !ShouldAnimateWindow(hwnd)) {
        return false;
    }
    const LONG style = static_cast<LONG>(GetWindowLongPtrW(hwnd, GWL_STYLE));
    if (style & WS_MAXIMIZE) {
        return false;
    }
    RECT rcFrom{};
    GetWindowRect(hwnd, &rcFrom);
    RectToPhysical(hwnd, &rcFrom);
    RECT rcTo{};
    if (!GetMaximizeRectPhysical(hwnd, &rcTo)) {
        return false;
    }
    CaptureBits cap;
    if (!CaptureWindow(hwnd, cap, true)) {
        return false;
    }
    return BeginAnimation(hwnd, AnimationType::Maximize, rcFrom, rcTo, std::move(cap),
                          /*deferOrig=*/false, 0);
}





// ---------------------------------------------------------------------------
// Hooks
// ---------------------------------------------------------------------------

#define DWP_HOOK_(name, defArgs, callArgs)                                 \
    LRESULT(CALLBACK* name##_orig) defArgs;                                \
    LRESULT CALLBACK name##_hook defArgs {                                 \
        if (uMsg == WM_SYSCOMMAND) {                                       \
            const UINT cmd = static_cast<UINT>(wParam) & 0xFFF0;           \
            if (cmd == SC_MINIMIZE && PlayMinimize(hWnd)) {                \
                LRESULT lr = name##_orig callArgs;                         \
                AfterOrigMinimize(hWnd);                                   \
                return lr;                                                 \
            }                                                              \
            if (cmd == SC_MAXIMIZE && PlayMaximize(hWnd)) {                \
                LRESULT lr = name##_orig callArgs;                         \
                return lr;                                                 \
            }                                                              \
            if (cmd == SC_RESTORE && PlayRestore(hWnd)) {                  \
                return 0;                                                  \
            }                                                              \
        }                                                                  \
        return name##_orig callArgs;                                       \
    }

#define DWP_HOOK(name, defArgs, callArgs) \
    DWP_HOOK_(name##A, defArgs, callArgs) \
    DWP_HOOK_(name##W, defArgs, callArgs)

DWP_HOOK(DefWindowProc,
         (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam),
         (hWnd, uMsg, wParam, lParam))
DWP_HOOK(DefFrameProc,
         (HWND hWnd, HWND hWndMDIClient, UINT uMsg, WPARAM wParam, LPARAM lParam),
         (hWnd, hWndMDIClient, uMsg, wParam, lParam))
DWP_HOOK(DefMDIChildProc,
         (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam),
         (hWnd, uMsg, wParam, lParam))
DWP_HOOK(DefDlgProc,
         (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam),
         (hWnd, uMsg, wParam, lParam))

static UINT CmdFromShow(int nCmdShow) {
    switch (nCmdShow) {
        case SW_MAXIMIZE:
            return SC_MAXIMIZE;
        case SW_MINIMIZE:
        case SW_SHOWMINIMIZED:
        case SW_FORCEMINIMIZE:
            return SC_MINIMIZE;
        case SW_RESTORE:
            return SC_RESTORE;
        default:
            return 0;
    }
}

BOOL WINAPI ShowWindow_hook(HWND hWnd, int nCmdShow) {
    if (g_fDisabled || g_fAnimating) {
        return ShowWindow_orig(hWnd, nCmdShow);
    }
    const UINT cmd = CmdFromShow(nCmdShow);
    if (cmd == SC_RESTORE && PlayRestore(hWnd)) {
        return TRUE;  // orig() runs after the overlay, in FinishQueued
    }
    bool playedMin = false;
    if (cmd == SC_MINIMIZE) {
        playedMin = PlayMinimize(hWnd);
    } else if (cmd == SC_MAXIMIZE) {
        PlayMaximize(hWnd);
    }
    const BOOL r = ShowWindow_orig(hWnd, nCmdShow);
    if (playedMin) {
        AfterOrigMinimize(hWnd);
    }
    return r;
}

using ShowWindowAsync_t = decltype(&ShowWindowAsync);
ShowWindowAsync_t ShowWindowAsync_orig;
BOOL WINAPI ShowWindowAsync_hook(HWND hWnd, int nCmdShow) {
    if (g_fDisabled || g_fAnimating) {
        return ShowWindowAsync_orig(hWnd, nCmdShow);
    }
    const UINT cmd = CmdFromShow(nCmdShow);
    if (cmd == SC_RESTORE && PlayRestore(hWnd)) {
        return TRUE;
    }
    bool playedMin = false;
    if (cmd == SC_MINIMIZE) {
        playedMin = PlayMinimize(hWnd);
    } else if (cmd == SC_MAXIMIZE) {
        PlayMaximize(hWnd);
    }
    const BOOL r = ShowWindowAsync_orig(hWnd, nCmdShow);
    if (playedMin) {
        AfterOrigMinimize(hWnd);
    }
    return r;
}

using DestroyWindow_t = decltype(&DestroyWindow);
DestroyWindow_t DestroyWindow_orig;
BOOL WINAPI DestroyWindow_hook(HWND hWnd) {
    ForgetCapture(hWnd);
    return DestroyWindow_orig(hWnd);
}

// ---------------------------------------------------------------------------
// Entry points
// ---------------------------------------------------------------------------

static HMODULE GetCurrentModule() {
    HMODULE hModule = nullptr;
    GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(&GetCurrentModule), &hModule);
    return hModule;
}

static void InitExeName() {
    wchar_t path[MAX_PATH] = {};
    if (!GetModuleFileNameW(nullptr, path, MAX_PATH)) {
        return;
    }
    const wchar_t* slash = path;
    for (const wchar_t* p = path; *p; ++p) {
        if (*p == L'\\' || *p == L'/') {
            slash = p + 1;
        }
    }
    wcsncpy_s(g_exeName, slash, _TRUNCATE);
}

static bool IsDeniedProcess(const wchar_t* name) {
    // Belt-and-suspenders with @exclude. Never hook the compositor, the
    // loader, or UWP/system hosts even if someone later adds @include *.
    static const wchar_t* kDenied[] = {
        L"dwm.exe",
        L"windhawk.exe",
        L"csrss.exe",
        L"wininit.exe",
        L"winlogon.exe",
        L"services.exe",
        L"lsass.exe",
        L"smss.exe",
        L"svchost.exe",
        L"applicationframehost.exe",
        L"runtimebroker.exe",
        L"shellexperiencehost.exe",
        L"startmenuexperiencehost.exe",
        L"searchhost.exe",
        L"searchapp.exe",
        L"systemsettings.exe",
        L"textinputhost.exe",
        L"lockapp.exe",
        L"sihost.exe",
        L"fontdrvhost.exe",
        L"conhost.exe",
        L"dllhost.exe",
        L"taskhostw.exe",
        L"wwahost.exe",
        L"msmpeng.exe",
        L"smartscreen.exe",
        L"securityhealthservice.exe",
        L"securityhealthsystray.exe",
        L"backgroundtaskhost.exe",
        L"useroobebroker.exe",
    };
    if (!name || !name[0]) {
        return true;
    }
    for (const wchar_t* denied : kDenied) {
        if (_wcsicmp(name, denied) == 0) {
            return true;
        }
    }
    return false;
}

BOOL Wh_ModInit() {
    InitExeName();
    Wh_Log(L"[%s] Init", g_exeName);
    if (IsDeniedProcess(g_exeName)) {
        Wh_Log(L"[%s] Denied process, not hooking", g_exeName);
        return FALSE;
    }
    LoadSettings();

    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32) {
        Wh_Log(L"[%s] user32.dll is not loaded", g_exeName);
        return FALSE;
    }
    pGetWindowMinimizeRect =
        reinterpret_cast<GetWindowMinimizeRect_t>(GetProcAddress(user32, "GetWindowMinimizeRect"));
    if (!pGetWindowMinimizeRect) {
        Wh_Log(L"[%s] GetWindowMinimizeRect not exported, using taskbar fallback", g_exeName);
    }

#define HOOK(func)                                                                                 \
    if (!Wh_SetFunctionHook(reinterpret_cast<void*>(func), reinterpret_cast<void*>(func##_hook),   \
                            reinterpret_cast<void**>(&func##_orig))) {                             \
        Wh_Log(L"[%s] Failed to hook " #func, g_exeName);                                          \
        return FALSE;                                                                              \
    }

    HOOK(DefWindowProcA)
    HOOK(DefWindowProcW)
    HOOK(DefFrameProcA)
    HOOK(DefFrameProcW)
    HOOK(DefMDIChildProcA)
    HOOK(DefMDIChildProcW)
    HOOK(DefDlgProcA)
    HOOK(DefDlgProcW)
    HOOK(ShowWindow)
    HOOK(ShowWindowAsync)
    HOOK(DestroyWindow)
#undef HOOK

    g_hinst = GetCurrentModule();
    Wh_Log(L"[%s] Hooks installed", g_exeName);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"[%s] Settings changed", g_exeName);
    LoadSettings();
}

void Wh_ModUninit() {
    Wh_Log(L"[%s] Uninit", g_exeName);
    g_fDisabled = true;
    {
        std::lock_guard<std::mutex> lock(g_queueMutex);
        while (!g_queue.empty()) {
            AnimRequest* req = g_queue.front();
            g_queue.pop_front();
            if (req && req->hwnd && IsWindow(req->hwnd)) {
                DisableTransitions(req->hwnd, FALSE);
            }
            delete req;
        }
    }
    if (g_hwndCurrent && IsWindow(g_hwndCurrent)) {
        DisableTransitions(g_hwndCurrent, FALSE);
    }
    if (g_hwndAnim && IsWindow(g_hwndAnim)) {
        SendMessageW(g_hwndAnim, WM_CLOSE, 0, 0);
    }
    if (g_hAnimWndThread) {
        WaitForSingleObject(g_hAnimWndThread, 1000);
        CloseHandle(g_hAnimWndThread);
        g_hAnimWndThread = nullptr;
    }
    if (g_hinst) {
        UnregisterClassW(kAnimClassName, g_hinst);
    }
    std::lock_guard<std::mutex> lock(g_cacheMutex);
    g_captureCache.clear();
}
