// ==WindhawkMod==
// @id              win7-window-animations-restorer
// @name            Windows 7 Window Animations Restorer
// @description     This mod restores the Windows 7 Aero minimize, restore and close animations on Windows 10 and Windows 11
// @version         1.0.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @license         MIT
// @include         *
// @exclude         dwm.exe
// @exclude         ntoskrnl.exe
// @exclude         smss.exe
// @exclude         csrss.exe
// @exclude         wininit.exe
// @exclude         winlogon.exe
// @exclude         services.exe
// @exclude         lsass.exe
// @exclude         svchost.exe
// @exclude         fontdrvhost.exe
// @exclude         audiodg.exe
// @exclude         LogonUI.exe
// @exclude         consent.exe
// @exclude         WerFault.exe
// @exclude         sihost.exe
// @exclude         ctfmon.exe
// @exclude         RuntimeBroker.exe
// @exclude         ShellExperienceHost.exe
// @exclude         StartMenuExperienceHost.exe
// @exclude         SearchHost.exe
// @exclude         TextInputHost.exe
// @exclude         windhawk.exe
// @compilerOptions -lgdi32 -lmsimg32 -lshcore -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*

# Windows 7 Window Animations Restorer

## About

This mod tries to restore the Windows 7 window animations on Windows 10 and 11 **without modifying the DWM** (the part of Windows that draws window effects).

## Sample Animation

![Demo GIF](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/bandicam2026-08-3014-19-03-266-ezgif.com-video-to-gif-converter.gif)

## What it does

- **Minimize / restore**: the window shrinks toward its taskbar button with the Windows 7 tilt-and-fade look (a slight 3D perspective, about a quarter of a second), then grows back the same way when you restore it. The real window only appears once the animation has caught up to it, so you never see a flash of the "wrong" state.
- **Close**: when a window closes, a snapshot of it tilts away and fades out over about a fifth of a second. The frames are rendered on the mod's own overlay thread; the closing app's UI thread only waits for the effect to finish (answering sent messages in the meantime, with a hard deadline), so a "save changes?" prompt the app itself puts up first still works exactly as it would without the mod, but the app won't otherwise process input or posted messages until the animation finishes.
- **Open**: opening a window is not animated by this mod as Windows handles that on its own (it may be added in a future update).

## Known Limitations

- Opening a window isn't animated (see above).
- If a window can't be minimized, its animation is simply skipped.
- Dialogs without a minimize button aren't animated.
- MDI child windows (documents inside an MDI frame) are left to Windows: they have no taskbar button to fly to.
- Some UWP apps may not support the closing animation.
- Snipping Tool doesn't play the closing animation for stability reasons.

## Which applications are affected

The mod loads into every running program, except for a short list of system, shell, and UWP components that couldn't be animated anyway. It only ever touches a program's own top-level windows. If you want to limit it to specific programs, use the "Custom process inclusion/exclusion list" in the mod's advanced settings.

## Notes

This is a best-effort recreation of the Windows 7 look, not a perfect copy. If there are problems or suggestions, please reach out to the author.
The mod has been tested on Windows 10 21H2, Windows 11 24H2, and Windows 11 25H2. System files are not modified and Windows components are not replaced because the modification just recreates the Windows 7 timing and motion on top of the current system. It is recommended to turn on logging in the advanced settings to see why a specific window wasn't animated.
For best results, avoid running this alongside other window-animation mods.

## Credits

Visual references only (no code was taken from these):

- **DWM 3D Transforms** by [xalejandro](https://github.com/tetawaves).
- [OpenGlass](https://github.com/ALTaleX531/OpenGlass) by ALTaleX.
- **3D Aero Transforms mod** by [kieldbg](https://github.com/kieldbg).


- The overall approach (a dedicated overlay thread plus `user32` hooks on the
  `Def*Proc`/`ShowWindow` family) was inspired by
  [Classic Minimize/Maximize Animations](https://windhawk.net/mods/classic-min-max-animations)
  by [aubymori](https://github.com/aubymori). This mod is a from-scratch
  reimplementation of that idea; no code from that mod is included here.
- `Mat::Matrix4x4F` is a from-scratch implementation of the public Microsoft
  `D2D1::Matrix4x4F` helper API (`d2d1_1helper.h`).

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- animateMinimize: true
  $name: Animate minimizing and restoring windows
  $description: This setting enables the Windows 7-like minimizing and restoring animations. Turn this setting off to let Windows animate (or not) on its own.
- animateClose: true
  $name: Animate closing windows
  $description: This setting enables the Windows 7-like closing animation. It runs after the application itself decides to destroy the window, so it can never discard unsaved work.
- minimizeDurationMs: 250
  $name: Minimize animation duration (ms)
  $description: This setting modifies how long the fly-out to the taskbar takes, from 50 to 1000 ms. Windows 7 used about 250 ms.
- restoreDurationMs: 210
  $name: Restore animation duration (ms)
  $description: This setting modifies how long the fly-in from the taskbar takes, from 50 to 1000 ms. Windows 7 used about 210 ms.
- closeDurationMs: 200
  $name: Close animation duration (ms)
  $description: This setting modifies how long the closing fade takes, from 50 to 1000 ms. Windows 7 used about 200 ms. The closing app waits for this long before the window is really destroyed, so keep it short on slower machines.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <dwmapi.h>
#include <shellscalingapi.h>
#include <winternl.h>
#include <algorithm>
#include <atomic>
#include <cstdlib>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <deque>
#include <iterator>
#include <list>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <utility>
#include <vector>


#ifndef DWMWA_TRANSITIONS_FORCEDISABLED
#define DWMWA_TRANSITIONS_FORCEDISABLED 3
#endif
#ifndef DWMWA_EXTENDED_FRAME_BOUNDS
#define DWMWA_EXTENDED_FRAME_BOUNDS 9
#endif
#ifndef PW_RENDERFULLCONTENT
#define PW_RENDERFULLCONTENT 0x00000002
#endif
#ifndef DWM_BB_ENABLE
#define DWM_BB_ENABLE 1
#define DWM_BB_BLURREGION 2
#endif

#define RECTW(rc) ((rc).right - (rc).left)
#define RECTH(rc) ((rc).bottom - (rc).top)

static wchar_t g_exeName[MAX_PATH] = L"?";
static void InitExeName() {
    wchar_t path[MAX_PATH] = {};
    if (!GetModuleFileNameW(nullptr, path, MAX_PATH))
        return;
    const wchar_t* slash = path;
    for (const wchar_t* p = path; *p; ++p)
        if (*p == L'\\' || *p == L'/')
            slash = p + 1;
    wcsncpy_s(g_exeName, slash, _TRUNCATE);
}
static bool IsSnippingTool() {
    return _wcsicmp(g_exeName, L"SnippingTool.exe") == 0;
}

class ScopedDc {
  public:
    ScopedDc() = default;
    explicit ScopedDc(HDC hdc) noexcept : m_hdc(hdc) {}
    ~ScopedDc() { reset(); }
    ScopedDc(const ScopedDc&) = delete;
    ScopedDc& operator=(const ScopedDc&) = delete;
    ScopedDc(ScopedDc&& o) noexcept : m_hdc(o.m_hdc) { o.m_hdc = nullptr; }
    ScopedDc& operator=(ScopedDc&& o) noexcept {
        if (this != &o) {
            reset(o.m_hdc);
            o.m_hdc = nullptr;
        }
        return *this;
    }
    void reset(HDC hdc = nullptr) noexcept {
        if (m_hdc)
            DeleteDC(m_hdc);
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
    ScopedGdiObj(ScopedGdiObj&& o) noexcept : m_obj(o.m_obj) {
        o.m_obj = nullptr;
    }
    ScopedGdiObj& operator=(ScopedGdiObj&& o) noexcept {
        if (this != &o) {
            reset(o.m_obj);
            o.m_obj = nullptr;
        }
        return *this;
    }
    void reset(HGDIOBJ obj = nullptr) noexcept {
        if (m_obj)
            DeleteObject(m_obj);
        m_obj = obj;
    }
    HGDIOBJ get() const noexcept { return m_obj; }
    explicit operator bool() const noexcept { return m_obj != nullptr; }

  private:
    HGDIOBJ m_obj = nullptr;
};
class ScopedSelect {
  public:
    ScopedSelect(HDC hdc, HGDIOBJ obj) noexcept
        : m_hdc(hdc), m_prev(SelectObject(hdc, obj)) {}
    ~ScopedSelect() {
        if (m_hdc && m_prev)
            SelectObject(m_hdc, m_prev);
    }
    ScopedSelect(const ScopedSelect&) = delete;
    ScopedSelect& operator=(const ScopedSelect&) = delete;

  private:
    HDC m_hdc;
    HGDIOBJ m_prev;
};
class ScopedDpiAware {
  public:
    ScopedDpiAware() noexcept
        : m_prev(SetThreadDpiAwarenessContext(
              DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)) {}
    ~ScopedDpiAware() {
        if (m_prev)
            SetThreadDpiAwarenessContext(m_prev);
    }
    ScopedDpiAware(const ScopedDpiAware&) = delete;
    ScopedDpiAware& operator=(const ScopedDpiAware&) = delete;

  private:
    DPI_AWARENESS_CONTEXT m_prev;
};
class ScopedScreenDc {
  public:
    explicit ScopedScreenDc(const RECT& rc) noexcept {
        const RECT prim{0, 0, LONG(GetSystemMetrics(SM_CXSCREEN)),
                        LONG(GetSystemMetrics(SM_CYSCREEN))};
        if (rc.left >= prim.left && rc.top >= prim.top &&
            rc.right <= prim.right && rc.bottom <= prim.bottom) {
            m_dc = GetDC(nullptr);
            return;
        }
        m_dc = CreateDCW(L"DISPLAY", nullptr, nullptr, nullptr);
        if (m_dc)
            m_owned = true;
        else
            m_dc = GetDC(nullptr);
    }
    ~ScopedScreenDc() {
        if (!m_dc)
            return;
        if (m_owned)
            DeleteDC(m_dc);
        else
            ReleaseDC(nullptr, m_dc);
    }
    HDC get() const noexcept { return m_dc; }
    explicit operator bool() const noexcept { return m_dc != nullptr; }
    ScopedScreenDc(const ScopedScreenDc&) = delete;
    ScopedScreenDc& operator=(const ScopedScreenDc&) = delete;

  private:
    HDC m_dc = nullptr;
    bool m_owned = false;
};
class ScopedHandle {
  public:
    ScopedHandle() = default;
    explicit ScopedHandle(HANDLE h) noexcept : m_h(h) {}
    ~ScopedHandle() { reset(); }
    ScopedHandle(const ScopedHandle&) = delete;
    ScopedHandle& operator=(const ScopedHandle&) = delete;
    void reset(HANDLE h = nullptr) noexcept {
        if (m_h && m_h != INVALID_HANDLE_VALUE)
            CloseHandle(m_h);
        m_h = h;
    }
    HANDLE release() noexcept {
        HANDLE t = m_h;
        m_h = nullptr;
        return t;
    }
    HANDLE get() const noexcept { return m_h; }
    explicit operator bool() const noexcept {
        return m_h && m_h != INVALID_HANDLE_VALUE;
    }

  private:
    HANDLE m_h = nullptr;
};
class ScopedProp {
  public:
    ScopedProp(HWND hwnd, LPCWSTR name) noexcept : m_hwnd(hwnd), m_name(name) {
        m_set = SetPropW(hwnd, name, HANDLE(1)) != FALSE;
    }
    ~ScopedProp() {
        if (m_set && m_hwnd && IsWindow(m_hwnd))
            RemovePropW(m_hwnd, m_name);
    }
    bool ok() const noexcept { return m_set; }
    ScopedProp(const ScopedProp&) = delete;
    ScopedProp& operator=(const ScopedProp&) = delete;

  private:
    HWND m_hwnd;
    LPCWSTR m_name;
    bool m_set = false;
};
class ScopedThreadpoolWork {
  public:
    explicit ScopedThreadpoolWork(PTP_WORK w) noexcept : m_w(w) {}
    ~ScopedThreadpoolWork() {
        if (m_w) {
            WaitForThreadpoolWorkCallbacks(m_w, FALSE);
            CloseThreadpoolWork(m_w);
        }
    }
    PTP_WORK get() const noexcept { return m_w; }
    explicit operator bool() const noexcept { return m_w != nullptr; }
    ScopedThreadpoolWork(const ScopedThreadpoolWork&) = delete;
    ScopedThreadpoolWork& operator=(const ScopedThreadpoolWork&) = delete;

  private:
    PTP_WORK m_w;
};
template <class F> class ScopedExit {
  public:
    explicit ScopedExit(F f) noexcept : m_f(f) {}
    ~ScopedExit() {
        if (m_armed)
            m_f();
    }
    void Dismiss() noexcept { m_armed = false; }
    ScopedExit(const ScopedExit&) = delete;
    ScopedExit& operator=(const ScopedExit&) = delete;

  private:
    F m_f;
    bool m_armed = true;
};
class ScopedDwmTransitions {
  public:
    explicit ScopedDwmTransitions(HWND hwnd) noexcept
        : m_hwnd(hwnd), m_disabled(false) {}
    void Disable() {
        if (!m_hwnd || !IsWindow(m_hwnd))
            return;
        BOOL dis = TRUE;
        if (SUCCEEDED(DwmSetWindowAttribute(
                m_hwnd, DWMWA_TRANSITIONS_FORCEDISABLED, &dis, sizeof(dis))))
            m_disabled = true;
    }
    void Restore() {
        if (m_disabled && m_hwnd && IsWindow(m_hwnd)) {
            BOOL dis = FALSE;
            DwmSetWindowAttribute(m_hwnd, DWMWA_TRANSITIONS_FORCEDISABLED, &dis,
                                  sizeof(dis));
        }
        m_disabled = false;
    }
    ~ScopedDwmTransitions() { Restore(); }
    void Dismiss() { m_disabled = false; }

  private:
    HWND m_hwnd;
    bool m_disabled;
};

namespace Mat {
struct Matrix4x4F {
    FLOAT _11, _12, _13, _14, _21, _22, _23, _24, _31, _32, _33, _34, _41, _42,
        _43, _44;
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
    static Matrix4x4F RotationX(FLOAT d) noexcept {
        FLOAT a = d * 3.141592654f / 180.f;
        FLOAT s = std::sin(a), c = std::cos(a);
        Matrix4x4F m;
        m._22 = c;
        m._23 = s;
        m._32 = -s;
        m._33 = c;
        return m;
    }
    static Matrix4x4F RotationY(FLOAT d) noexcept {
        FLOAT a = d * 3.141592654f / 180.f;
        FLOAT s = std::sin(a), c = std::cos(a);
        Matrix4x4F m;
        m._11 = c;
        m._13 = -s;
        m._31 = s;
        m._33 = c;
        return m;
    }
    static Matrix4x4F PerspectiveProjection(FLOAT depth) noexcept {
        Matrix4x4F m;
        if (depth > 0)
            m._34 = -1.f / depth;
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
    void TransformPoint(float x, float y, float z, float& ox,
                        float& oy) const noexcept {
        float rx = x * _11 + y * _21 + z * _31 + _41;
        float ry = x * _12 + y * _22 + z * _32 + _42;
        float rw = x * _14 + y * _24 + z * _34 + _44;
        if (rw > 0.0001f || rw < -0.0001f) {
            ox = rx / rw;
            oy = ry / rw;
        } else {
            ox = rx;
            oy = ry;
        }
    }
};
}
using Mat::Matrix4x4F;
using ShowWindow_t = decltype(&ShowWindow);
using ShowWindowAsync_t = decltype(&ShowWindowAsync);
using DestroyWindow_t = decltype(&DestroyWindow);

// Defaults match Windows 7's own timings; the user can retune each one from
// the settings (see LoadSettings), which matters most for the close animation
// because it sits on the critical path of closing a window.
constexpr UINT kDefaultMinimizeDurationMs = 250;
constexpr UINT kDefaultCloseDurationMs = 200;
constexpr UINT kDefaultRestoreDurationMs = 210;
constexpr UINT kMinDurationMs = 50;
constexpr UINT kMaxDurationMs = 1000;
static std::atomic<UINT> g_minimizeDurationMs{kDefaultMinimizeDurationMs};
static std::atomic<UINT> g_closeDurationMs{kDefaultCloseDurationMs};
static std::atomic<UINT> g_restoreDurationMs{kDefaultRestoreDurationMs};

// Capture size guard. A window can't usefully exceed the virtual screen, so
// 8192 px per side already covers anything a real desktop shows edge to
// edge; the pixel cap on top of it (an 8K frame, ~133 MB of BGRA) is what
// actually bounds the transient allocation -- the per-side limit alone would
// still admit 8192x8192 = 256 MB, and the old 16384 limit a full gigabyte.
// Larger windows just don't animate.
constexpr int kMaxCaptureSide = 8192;
constexpr size_t kMaxCapturePixels = size_t(7680) * size_t(4320);

enum class AnimationType {
    None = 0,
    Close,
    Minimize,
    RestoreFromMinimized
};

static float Lerp(float a, float b, float t) {
    return a + (b - a) * t;
}
// Windows 7's Aero minimize/restore glide decelerates into place rather than
// moving at constant speed -- a cubic ease-out reproduces that "settling"
// feel much better than the raw linear t we used before.
static float EaseOutCubic(float t) {
    float f = 1.f - std::clamp(t, 0.f, 1.f);
    return 1.f - f * f * f;
}
static bool IsRectUsable(const RECT& rc) {
    return rc.right > rc.left && rc.bottom > rc.top;
}
static RECT LerpRect(const RECT& a, const RECT& b, float t) {
    RECT r;
    r.left = LONG(std::lround(Lerp(float(a.left), float(b.left), t)));
    r.top = LONG(std::lround(Lerp(float(a.top), float(b.top), t)));
    r.right = LONG(std::lround(Lerp(float(a.right), float(b.right), t)));
    r.bottom = LONG(std::lround(Lerp(float(a.bottom), float(b.bottom), t)));
    return r;
}
// The taskbar button rect is wide and short (e.g. 160x48 on Windows 11),
// nothing like the window that flies into it. Windows 7 shrank the window
// uniformly, so the final frame is the window's own aspect ratio fitted
// inside the button rect and centered on it: the capture is never squashed
// and the last frame still lands on the button itself.
static RECT AspectCorrectedMinimizeTarget(const RECT& button,
                                          const RECT& window) {
    const float bw = float(RECTW(button)), bh = float(RECTH(button));
    const float ww = float(RECTW(window)), wh = float(RECTH(window));
    if (bw < 1 || bh < 1 || ww < 1 || wh < 1)
        return button;
    const float s = std::min(bw / ww, bh / wh);
    const LONG tw = std::max<LONG>(1, LONG(std::lround(ww * s))),
               th = std::max<LONG>(1, LONG(std::lround(wh * s)));
    RECT t;
    t.left = button.left + (LONG(bw) - tw) / 2;
    t.top = button.top + (LONG(bh) - th) / 2;
    t.right = t.left + tw;
    t.bottom = t.top + th;
    if (!IsRectUsable(t))
        return button;
    return t;
}
struct Win7TransformParams {
    float rotX = 0, rotY = 0, transZ = 0, opacity = 1, ease = 0;
    float yTrans = 0, zTrans = 0, pivotY = 0;
};

static Win7TransformParams ParamsFor(AnimationType type, float t, float h = 0) {
    Win7TransformParams p;
    t = std::clamp(t, 0.f, 1.f);
    switch (type) {
        case AnimationType::Minimize: {
            float e = EaseOutCubic(t);
            p.rotX = 5.f * e;
            p.rotY = 8.f * e;
            p.transZ = -4.f * e;
            p.opacity = 1.f - 0.35f * e;
            p.ease = e;
            p.pivotY = h * 0.5f;
            break;
        }
        case AnimationType::RestoreFromMinimized: {
            float e = EaseOutCubic(t);
            float away = 1.f - e;
            p.rotX = 5.f * away;
            p.rotY = 8.f * away;
            p.transZ = -4.f * away;
            p.opacity = 0.65f + 0.35f * e;
            p.ease = e;
            p.pivotY = h * 0.5f;
            break;
        }
        case AnimationType::Close: {
            float ease = 1.f - std::sqrt(1.f - t);
            p.ease = ease;
            p.rotX = -5.f * ease;
            p.rotY = -2.f * ease;
            p.pivotY = h;
            p.opacity = 1.f - t;
            break;
        }
        default:
            break;
    }
    return p;
}
static RECT RectFor(AnimationType type, float t, const RECT& win,
                    const RECT& dest) {
    t = std::clamp(t, 0.f, 1.f);
    switch (type) {
        case AnimationType::Minimize:
            return LerpRect(win, dest, EaseOutCubic(t));
        case AnimationType::RestoreFromMinimized:
            return LerpRect(dest, win, EaseOutCubic(t));
        default:
            return win;
    }
}
static UINT DurationMsFor(AnimationType type) {
    UINT ms = g_minimizeDurationMs.load();
    if (type == AnimationType::Close) {
        ms = g_closeDurationMs.load();
    } else if (type == AnimationType::RestoreFromMinimized) {
        ms = g_restoreDurationMs.load();
    }
    return std::clamp(ms, kMinDurationMs, kMaxDurationMs);
}
static Matrix4x4F BuildCameraMatrix(float w, float h, float df = 0.8f) {
    float depth = std::fmax(h, 1.f) * df;
    return Matrix4x4F::Translation(-w * 0.5f, -h * 0.5f, 0) *
           Matrix4x4F::Scale(1, 1, -1) *
           Matrix4x4F::PerspectiveProjection(depth) *
           Matrix4x4F::Translation(w * 0.5f, h * 0.5f, 0);
}
static Matrix4x4F BuildCornerMatrix(
    const Win7TransformParams& p, const RECT& rcCurrent, float ow, float oh,
    AnimationType type = AnimationType::Minimize) {
    float w = ow, h = oh;
    float pivotY = (type == AnimationType::Close) ? h : h * 0.5f;
    if (p.pivotY != 0)
        pivotY = p.pivotY;
    if (type == AnimationType::Close) {
        Matrix4x4F model = Matrix4x4F::Translation(0, -pivotY, 0) *
                           Matrix4x4F::RotationY(p.rotY) *
                           Matrix4x4F::RotationX(p.rotX) *
                           Matrix4x4F::Translation(0, pivotY, 0);
        Matrix4x4F camera = BuildCameraMatrix(w, h, 0.8f);
        Matrix4x4F place = Matrix4x4F::Translation(float(rcCurrent.left),
                                                   float(rcCurrent.top), 0);
        return model * camera * place;
    }
    float width = float(RECTW(rcCurrent)), height = float(RECTH(rcCurrent));
    float sx = ow > 1 ? width / ow : 1, sy = oh > 1 ? height / oh : 1;
    float cx = ow * 0.5f, cy = oh * 0.5f;
    Matrix4x4F m =
        Matrix4x4F::Translation(-cx, -cy, 0) *
        (Matrix4x4F::RotationX(-p.rotX) * Matrix4x4F::RotationY(-p.rotY)) *
        Matrix4x4F::Translation(cx, cy, 0) * Matrix4x4F::Scale(sx, sy, 1) *
        Matrix4x4F::Translation(float(rcCurrent.left), float(rcCurrent.top), 0);
    float invH = 1.f / std::fmax(oh, 1.f);
    m._43 += p.transZ;
    m._44 += -p.transZ * invH;
    return m;
}

bool g_animateMinimize = true;
bool g_animateClose = true;
static UINT LoadDurationSetting(PCWSTR name, UINT def) {
    int v = Wh_GetIntSetting(name);
    if (v <= 0)
        return def;
    return std::clamp(UINT(v), kMinDurationMs, kMaxDurationMs);
}
static void LoadSettings() {
    g_animateMinimize = Wh_GetIntSetting(L"animateMinimize") != 0;
    g_animateClose = Wh_GetIntSetting(L"animateClose") != 0;
    g_minimizeDurationMs.store(
        LoadDurationSetting(L"minimizeDurationMs", kDefaultMinimizeDurationMs));
    g_restoreDurationMs.store(
        LoadDurationSetting(L"restoreDurationMs", kDefaultRestoreDurationMs));
    g_closeDurationMs.store(
        LoadDurationSetting(L"closeDurationMs", kDefaultCloseDurationMs));
}

typedef BOOL(WINAPI* GetWindowMinimizeRect_t)(HWND, LPRECT);
GetWindowMinimizeRect_t pGetWindowMinimizeRect = nullptr;
typedef BOOL(WINAPI* IsHungAppWindow_t)(HWND);
static IsHungAppWindow_t pIsHungAppWindow = nullptr;
ShowWindow_t ShowWindow_orig = nullptr;
ShowWindowAsync_t ShowWindowAsync_orig = nullptr;
DestroyWindow_t DestroyWindow_orig = nullptr;

struct CaptureBits {
    std::vector<uint32_t> pixels;
    int width = 0, height = 0;
    int srcW = 0, srcH = 0;
    bool empty() const { return pixels.empty() || width <= 0 || height <= 0; }
    int LogicalW() const { return srcW ? srcW : width; }
    int LogicalH() const { return srcH ? srcH : height; }
};
static const size_t kMaxCachedCaptures = 3;
// Budget for the whole cache. The most recently minimized window is kept at
// full resolution (up to kMaxFullResCachedBytes: a 4K frame, ~33 MB; a
// 1080p window is ~8 MB), because minimize-then-immediately-restore is by
// far the common case and a restore rendered from a half-size source is
// visibly softer than the minimize was. Anything larger, and every older
// entry, is stored in the downscaled format (see DemoteOlderLocked), and the
// full-res entry itself is released the moment the window is restored or
// destroyed -- so per-process retention is one frame of the last minimized
// window plus a few MB, no matter how many windows a process cycles through.
static const size_t kMaxFullResCachedBytes = 3840u * 2160u * 4u;
static const size_t kMaxCachedBytes = kMaxFullResCachedBytes + 16u * 1024u * 1024u;
// Downscale floor for the *older* entries: DownscaleForCache scales to half
// the window's own size when that's larger, so a large window still keeps
// proportionally more detail than a fixed low cap would give.
static const int kMaxCachedSide = 384;
static std::mutex g_cacheMutex;
struct CacheEntry {
    CaptureBits bits;
    std::list<HWND>::iterator lruIt;
    bool fullRes = false;
};
static CaptureBits DownscaleForCache(const CaptureBits& in);
static std::unordered_map<HWND, CacheEntry> g_captureCache;
static std::list<HWND> g_captureLru;
static size_t g_cacheBytes = 0;
static size_t CaptureBytes(const CaptureBits& b) {
    return b.pixels.size() * sizeof(uint32_t);
}
static void EvictLocked() {
    while (!g_captureLru.empty() &&
           (g_captureCache.size() > kMaxCachedCaptures ||
            g_cacheBytes > kMaxCachedBytes)) {
        HWND v = g_captureLru.back();
        g_captureLru.pop_back();
        auto it = g_captureCache.find(v);
        if (it != g_captureCache.end()) {
            g_cacheBytes -=
                std::min(g_cacheBytes, CaptureBytes(it->second.bits));
            g_captureCache.erase(it);
        }
    }
}
// Only the LRU head may hold a full-resolution capture: every other entry is
// shrunk to the compact format so the full-res budget is spent exactly once.
// The shrink is a full bilinear resample (up to ~2M output pixels through
// SampleBilinear, plus a ForceOpaqueAlpha pass) -- tens of milliseconds of
// CPU. g_cacheMutex is also taken by ForgetCapture on every WM_NCDESTROY in
// the process (Explorer alone fires that constantly for menus/tooltips/
// dialogs) and by HasCachedCapture on the restore path, so that work cannot
// run while the lock is held. Collect the oversized entries and move their
// bits out while locked, unlock to do the resample, then relock and store
// the compact result back -- only if the entry is still exactly the
// placeholder we left (nobody else touched it while we were unlocked).
static void DemoteOlder(std::unique_lock<std::mutex>& lock) {
    if (g_captureLru.empty())
        return;
    std::vector<std::pair<HWND, CaptureBits>> toShrink;
    auto lit = std::next(g_captureLru.begin());
    while (lit != g_captureLru.end()) {
        HWND h = *lit;
        auto it = g_captureCache.find(h);
        if (it == g_captureCache.end() || !it->second.fullRes) {
            ++lit;
            continue;
        }
        if (it->second.bits.width <= kMaxCachedSide &&
            it->second.bits.height <= kMaxCachedSide) {
            it->second.fullRes = false; // already compact: nothing to shrink
            ++lit;
            continue;
        }
        g_cacheBytes -= std::min(g_cacheBytes, CaptureBytes(it->second.bits));
        toShrink.emplace_back(h, std::move(it->second.bits));
        it->second.bits = CaptureBits{}; // placeholder while unlocked
        ++lit;
    }
    if (toShrink.empty())
        return;
    lock.unlock();
    for (auto& entry : toShrink)
        entry.second = DownscaleForCache(entry.second);
    lock.lock();
    for (auto& entry : toShrink) {
        HWND h = entry.first;
        auto it = g_captureCache.find(h);
        // Gone (ForgetCapture/TakeCachedCapture ran while unlocked), or
        // already refreshed by a newer CacheCapture for the same HWND: leave
        // whatever is there now alone, it owns its own byte accounting.
        if (it == g_captureCache.end() || !it->second.bits.empty())
            continue;
        if (entry.second.empty()) {
            // Allocation failed: drop the entry rather than leave it empty.
            g_captureLru.erase(it->second.lruIt);
            g_captureCache.erase(it);
            continue;
        }
        it->second.bits = std::move(entry.second);
        it->second.fullRes = false;
        g_cacheBytes += CaptureBytes(it->second.bits);
    }
}
static void CacheCapture(HWND hwnd, CaptureBits&& bits) {
    if (!hwnd || bits.empty())
        return;
    // Keep the full-size frame when it fits the full-res budget; otherwise
    // store the compact form right away.
    bool fullRes = CaptureBytes(bits) <= kMaxFullResCachedBytes;
    if (!fullRes) {
        CaptureBits small = DownscaleForCache(bits);
        if (small.empty() || CaptureBytes(small) > kMaxCachedBytes)
            return;
        bits = std::move(small);
    }
    try {
        std::unique_lock<std::mutex> lock(g_cacheMutex);
        auto it = g_captureCache.find(hwnd);
        if (it != g_captureCache.end()) {
            g_cacheBytes -=
                std::min(g_cacheBytes, CaptureBytes(it->second.bits));
            it->second.bits = std::move(bits);
            it->second.fullRes = fullRes;
            g_cacheBytes += CaptureBytes(it->second.bits);
            g_captureLru.splice(g_captureLru.begin(), g_captureLru,
                                it->second.lruIt);
        } else {
            g_captureLru.push_front(hwnd);
            g_cacheBytes += CaptureBytes(bits);
            g_captureCache.emplace(
                hwnd,
                CacheEntry{std::move(bits), g_captureLru.begin(), fullRes});
        }
        DemoteOlder(lock);
        EvictLocked();
    } catch (...) {
    }
}
static bool HasCachedCapture(HWND hwnd) {
    if (!hwnd)
        return false;
    try {
        std::lock_guard<std::mutex> lock(g_cacheMutex);
        return g_captureCache.find(hwnd) != g_captureCache.end();
    } catch (...) {
        return false;
    }
}
static bool TakeCachedCapture(HWND hwnd, int ew, int eh, CaptureBits& out) {
    try {
        std::lock_guard<std::mutex> lock(g_cacheMutex);
        auto it = g_captureCache.find(hwnd);
        if (it == g_captureCache.end())
            return false;
        CaptureBits bits = std::move(it->second.bits);
        g_cacheBytes -= std::min(g_cacheBytes, CaptureBytes(bits));
        g_captureLru.erase(it->second.lruIt);
        g_captureCache.erase(it);
        if (bits.empty())
            return false;
        // The cache may hold a downscaled copy, so compare the logical size.
        if (ew > 0 && eh > 0 &&
            (std::abs(bits.LogicalW() - ew) > 128 ||
             std::abs(bits.LogicalH() - eh) > 128))
            return false;
        out = std::move(bits);
        return true;
    } catch (...) {
        return false;
    }
}
static void ForgetCapture(HWND hwnd) {
    try {
        std::lock_guard<std::mutex> lock(g_cacheMutex);
        auto it = g_captureCache.find(hwnd);
        if (it != g_captureCache.end()) {
            g_cacheBytes -=
                std::min(g_cacheBytes, CaptureBytes(it->second.bits));
            g_captureLru.erase(it->second.lruIt);
            g_captureCache.erase(it);
        }
    } catch (...) {
    }
}
static void ForceOpaqueAlpha(uint32_t* p, size_t c) {
    for (size_t i = 0; i < c; ++i)
        p[i] |= 0xFF000000u;
}
// RtlGetVersion, unlike GetVersion(Ex), isn't lied to by the app compatibility
// shim -- needed since this runs inside arbitrary host processes.
static bool IsWindows11OrGreaterRuntime() {
    static const bool result = [] {
        HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
        if (!ntdll)
            return false;
        using RtlGetVersionFn = LONG(WINAPI*)(PRTL_OSVERSIONINFOW);
        auto pRtlGetVersion =
            RtlGetVersionFn(GetProcAddress(ntdll, "RtlGetVersion"));
        if (!pRtlGetVersion)
            return false;
        RTL_OSVERSIONINFOW vi{};
        vi.dwOSVersionInfoSize = sizeof(vi);
        if (pRtlGetVersion(&vi) != 0)
            return false;
        return vi.dwMajorVersion > 10 ||
               (vi.dwMajorVersion == 10 && vi.dwBuildNumber >= 22000);
    }();
    return result;
}
// The screen scrape is a literal BitBlt(..., CAPTUREBLT) off the display DC
// over DWMWA_EXTENDED_FRAME_BOUNDS. On Windows 11, DWM rounds the frame's
// corners, so the scrape also picks up whatever desktop pixels show through
// them -- and ForceOpaqueAlpha then makes those fully opaque, leaving the
// animated frame with square corners and a sliver of background in each one.
// Punch the corner pixels outside the rounded rect back to transparent
// (alpha AND color, since the overlay is composited as premultiplied alpha)
// so the mismatch is invisible instead of forced opaque.
static void PunchRoundedCorners(uint32_t* p, int w, int h, int radius) {
    if (!p || w <= 0 || h <= 0 || radius <= 0)
        return;
    radius = std::min(radius, std::min(w, h) / 2);
    if (radius <= 0)
        return;
    for (int y = 0; y < radius; ++y) {
        // Distance of this row/col from the corner's circle center, in the
        // radius x radius box anchored at the corner.
        long long dy = radius - y;
        for (int x = 0; x < radius; ++x) {
            long long dx = radius - x;
            if (dx * dx + dy * dy <= (long long)radius * radius)
                continue; // inside the rounded corner -- keep the pixel
            p[size_t(y) * size_t(w) + size_t(x)] = 0;                     // top-left
            p[size_t(y) * size_t(w) + size_t(w - 1 - x)] = 0;             // top-right
            p[size_t(h - 1 - y) * size_t(w) + size_t(x)] = 0;             // bottom-left
            p[size_t(h - 1 - y) * size_t(w) + size_t(w - 1 - x)] = 0;     // bottom-right
        }
    }
}

static bool GetWindowRectPhysical(HWND hwnd, RECT* rc) {
    if (!hwnd || !rc)
        return false;
    if (GetWindowRect(hwnd, rc))
        return IsRectUsable(*rc);
    return false;
}
// DWM frame bounds, not GetWindowRect: the raw rect includes the invisible resize margins, black when maximized.
static bool GetFrameBoundsPhysical(HWND hwnd, RECT* rc) {
    if (!hwnd || !rc)
        return false;
    RECT ext{};
    HRESULT hr = DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &ext,
                                       sizeof(ext));
    if (SUCCEEDED(hr) && IsRectUsable(ext) && RECTW(ext) >= 8 &&
        RECTH(ext) >= 8 && RECTW(ext) <= kMaxCaptureSide &&
        RECTH(ext) <= kMaxCaptureSide) {
        *rc = ext;
        return true;
    }
    return GetWindowRectPhysical(hwnd, rc);
}
static bool GetVisibleWindowRectForMinimize(HWND hwnd, RECT* rc) {
    return GetFrameBoundsPhysical(hwnd, rc);
}
// A window that has sat behind another window for a while may never have
// had its composed surface refreshed there: the screen-scrape branch below
// is a literal BitBlt off the display DC, so it captures whatever is
// actually on screen at the target's rect -- if another, already-open
// window overlaps it, the capture ends up showing THAT window's pixels
// instead of the target's. Sample a grid of interior points (well inside
// the frame, away from the borders and the rounded corners already handled
// separately) and only trust the scrape if every one of them resolves back
// to this window's own top-level hierarchy.
static bool IsWindowUnoccludedAt(HWND hwnd, const RECT& rc) {
    HWND root = GetAncestor(hwnd, GA_ROOT);
    if (!root)
        root = hwnd;
    const int w = RECTW(rc), h = RECTH(rc);
    if (w <= 0 || h <= 0)
        return false;
    // No foreground fast path: even the active window can have a topmost
    // window over it (media players, on-screen keyboards, other mods'
    // overlays), and then the scrape would carry a rectangle of someone
    // else's content into the animation. Five WindowFromPoint calls are
    // negligible next to the BitBlt + GetDIBits that follow.
    // 5-point diamond instead of a 3x3 grid: enough coverage to catch a
    // partially-overlapping window while staying cheap, and the loop still
    // exits on the very first occluded sample.
    static const float kPts[5][2] = {
        {0.5f, 0.5f}, {0.25f, 0.25f}, {0.75f, 0.25f}, {0.25f, 0.75f}, {0.75f, 0.75f}};
    for (const auto& p : kPts) {
        POINT pt{rc.left + LONG(float(w) * p[0]),
                 rc.top + LONG(float(h) * p[1])};
        HWND hit = WindowFromPoint(pt);
        if (!hit)
            return false;
        HWND hitRoot = GetAncestor(hit, GA_ROOT);
        if (!hitRoot)
            hitRoot = hit;
        if (hitRoot != root)
            return false;
    }
    return true;
}
// The screen scrape reads off the actual display surface via a "DISPLAY" DC:
// that DC only has real pixels where a monitor is actually present. A window
// that is only partially on-screen (dragged mostly past the right/bottom edge
// of the desktop, or spanning past a disconnected/disabled monitor) still
// reports a full DWM frame-bounds rect, so the scrape silently comes back
// black for the off-desktop slice while the on-desktop slice looks fine --
// exactly the "half black, half visible" artifact. Guard against it with the
// public virtual-screen metrics and require the whole capture rect to sit
// inside the actual desktop before trusting the scrape at all; otherwise fall
// through to the PrintWindow path below, which paints the window's own
// content into an off-screen bitmap and doesn't care where the window sits
// on screen.
static bool IsRectFullyOnVirtualScreen(const RECT& rc) {
    const int vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    const int vy = GetSystemMetrics(SM_YVIRTUALSCREEN);
    const int vw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    const int vh = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    if (vw <= 0 || vh <= 0)
        return false; // couldn't query the desktop bounds -- don't trust the scrape
    const RECT virt{vx, vy, vx + vw, vy + vh};
    return rc.left >= virt.left && rc.top >= virt.top &&
           rc.right <= virt.right && rc.bottom <= virt.bottom;
}
// A frame where DWM hasn't yet recomposed the window's border/shadow --
// typically right after DisableTransitions() forces a frame update -- shows
// up as a solid black run along one or more *edges* of the scrape while the
// rest of the window is fine: a one-frame compositing race, not real content.
// Check only a thin 2px strip along each edge (cheap) for a long run of
// fully-black pixels; a real dark-themed border is extremely unlikely to be
// perfectly (0,0,0) for most of an edge's length, so this stays specific to
// the artifact instead of flagging legitimate dark UIs.
static bool HasBlackBorderArtifact(const uint32_t* p, int w, int h) {
    if (!p || w <= 4 || h <= 4)
        return false;
    auto isBlack = [&](int x, int y) {
        return (p[size_t(y) * size_t(w) + size_t(x)] & 0x00FFFFFF) == 0;
    };
    auto longestRun = [&](int len, auto&& at) {
        int longest = 0, run = 0;
        for (int i = 0; i < len; ++i) {
            if (at(i)) {
                if (++run > longest)
                    longest = run;
            } else {
                run = 0;
            }
        }
        return longest;
    };
    const int topRun = longestRun(
        w, [&](int x) { return isBlack(x, 0) && isBlack(x, 1); });
    const int botRun = longestRun(w, [&](int x) {
        return isBlack(x, h - 1) && isBlack(x, h - 2);
    });
    const int leftRun = longestRun(
        h, [&](int y) { return isBlack(0, y) && isBlack(1, y); });
    const int rightRun = longestRun(h, [&](int y) {
        return isBlack(w - 1, y) && isBlack(w - 2, y);
    });
    return topRun > w / 2 || botRun > w / 2 || leftRun > h / 2 ||
           rightRun > h / 2;
}
// PrintWindow re-reads the window surface: the DWM-composed border, the rounded
// corners and the glass are not in it, which is why the shell frames came out
// borderless. Callers that need the real frame ask for the screen scrape only.
static bool CaptureWindowForClose(HWND hwnd, CaptureBits& out,
                                  bool composedOnly) {
    if (!hwnd || !IsWindow(hwnd))
        return false;
    RECT rc{};
    {
        ScopedDpiAware dpi; // geometry only -- never app code
        if (!GetFrameBoundsPhysical(hwnd, &rc))
            return false;
    }
    int w = RECTW(rc), h = RECTH(rc);
    if (w < 1 || h < 1 || w > kMaxCaptureSide || h > kMaxCaptureSide ||
        size_t(w) * size_t(h) > kMaxCapturePixels)
        return false;
    {
        ScopedDpiAware dpi; // screen scrape only -- no app code runs here
        // Guards against capturing an overlapping, already-open window's
        // pixels instead of this window's own composed surface (see
        // IsWindowUnoccludedAt above). If occluded, skip straight to the
        // PrintWindow fallback below, which asks the window itself to
        // paint, so it can never show someone else's content.
        if (IsRectFullyOnVirtualScreen(rc) && IsWindowUnoccludedAt(hwnd, rc)) {
            ScopedScreenDc screenDc(rc);
            if (screenDc) {
            ScopedDc memDc(CreateCompatibleDC(screenDc.get()));
            if (memDc) {
                ScopedGdiObj hBmp(CreateCompatibleBitmap(screenDc.get(), w, h));
                if (hBmp) {
                    ScopedSelect sel(memDc.get(), hBmp.get());
                    if (BitBlt(memDc.get(), 0, 0, w, h, screenDc.get(), rc.left,
                               rc.top, SRCCOPY | 0x40000000)) {
                        BITMAPINFO bmi{};
                        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                        bmi.bmiHeader.biWidth = w;
                        bmi.bmiHeader.biHeight = -h;
                        bmi.bmiHeader.biPlanes = 1;
                        bmi.bmiHeader.biBitCount = 32;
                        bmi.bmiHeader.biCompression = BI_RGB;
                        try {
                            out.width = w;
                            out.height = h;
                            out.pixels.resize(size_t(w) * size_t(h));
                            if (GetDIBits(memDc.get(), (HBITMAP)hBmp.get(), 0,
                                          h, out.pixels.data(), &bmi,
                                          DIB_RGB_COLORS)) {
                                // One frame right after DisableTransitions()
                                // can land with the border/shadow not yet
                                // recomposed (see HasBlackBorderArtifact). A
                                // single immediate re-scrape is enough in
                                // practice and keeps the retry bounded and
                                // cheap -- if it's still black we accept it
                                // rather than loop or add a sleep that would
                                // delay every capture, not just this rare
                                // one.
                                if (HasBlackBorderArtifact(out.pixels.data(),
                                                           w, h) &&
                                    BitBlt(memDc.get(), 0, 0, w, h,
                                           screenDc.get(), rc.left, rc.top,
                                           SRCCOPY | 0x40000000)) {
                                    GetDIBits(memDc.get(), (HBITMAP)hBmp.get(),
                                              0, h, out.pixels.data(), &bmi,
                                              DIB_RGB_COLORS);
                                }
                                size_t nonBlack = 0;
                                for (size_t i = 0;
                                     i < out.pixels.size() && nonBlack < 100;
                                     ++i)
                                    if ((out.pixels[i] & 0x00FFFFFF) != 0)
                                        ++nonBlack;
                                if (nonBlack >= 10) {
                                    if (!IsSnippingTool()) {
                                        ForceOpaqueAlpha(out.pixels.data(),
                                                         out.pixels.size());
                                        if (IsWindows11OrGreaterRuntime()) {
                                            UINT dpi = GetDpiForWindow(hwnd);
                                            if (!dpi)
                                                dpi = 96;
                                            int radius =
                                                MulDiv(8, int(dpi), 96);
                                            PunchRoundedCorners(
                                                out.pixels.data(), w, h,
                                                radius);
                                        }
                                    }
                                    return true;
                                }
                            }
                        } catch (...) {
                            out = {};
                        }
                    }
                }
            }
        }
        }
    }
    if (composedOnly) {
        out = {};
        return false;
    }
    {
        // PrintWindow below dispatches WM_PRINT/WM_PRINTCLIENT straight into the
        // target window's own WndProc, on this same thread: that is application
        // code, so it must run under the thread's normal DPI awareness context,
        // never under the per-monitor-aware override used for geometry/capture.
        // Note that on the minimize path this re-enters the app's WndProc from
        // inside its own DefWindowProc(WM_SYSCOMMAND) call. Well-behaved
        // windows handle WM_PRINT re-entrantly (the taskbar's own live
        // thumbnails and Alt+Tab issue it at arbitrary times), and the
        // screen scrape above is the primary path, so this only runs when
        // the window is occluded or its surface scraped black.
        BITMAPINFO bmi{};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = w;
        bmi.bmiHeader.biHeight = -h;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        void* bits = nullptr;
        ScopedScreenDc screenDc(rc);
        if (!screenDc)
            return false;
        ScopedGdiObj dib(CreateDIBSection(screenDc.get(), &bmi, DIB_RGB_COLORS,
                                          &bits, nullptr, 0));
        if (!dib || !bits)
            return false;
        ScopedDc memDc(CreateCompatibleDC(screenDc.get()));
        if (!memDc)
            return false;
        ScopedSelect sel(memDc.get(), dib.get());
        bool painted =
            PrintWindow(hwnd, memDc.get(), PW_RENDERFULLCONTENT) != FALSE;
        if (!painted)
            painted = PrintWindow(hwnd, memDc.get(), 0) != FALSE;
        if (!painted)
            return false;
        GdiFlush();
        try {
            out.width = w;
            out.height = h;
            out.pixels.resize(size_t(w) * size_t(h));
            std::memcpy(out.pixels.data(), bits, out.pixels.size() * 4);
            if (!IsSnippingTool())
                ForceOpaqueAlpha(out.pixels.data(), out.pixels.size());
            return true;
        } catch (...) {
            out = {};
            return false;
        }
    }
}
// Fallback target when GetWindowMinimizeRect is unavailable: the task list of the
// taskbar on the window's own monitor. A fixed bottom-left corner would send the
// window to an empty spot on a centered Windows 11 taskbar.
static bool GetTaskbarTargetPhysical(HWND hwnd, RECT* rc) {
    if (!rc)
        return false;
    const HMONITOR target = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    HWND best = nullptr;
    RECT bestRc{};
    for (const wchar_t* cls : {L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd"}) {
        for (HWND tb = FindWindowExW(nullptr, nullptr, cls, nullptr); tb;
             tb = FindWindowExW(nullptr, tb, cls, nullptr)) {
            RECT r{};
            if (!GetWindowRect(tb, &r) || !IsRectUsable(r))
                continue;
            if (!best) {
                best = tb;
                bestRc = r;
            }
            if (MonitorFromWindow(tb, MONITOR_DEFAULTTONEAREST) == target) {
                best = tb;
                bestRc = r;
                break;
            }
        }
    }
    if (!best)
        return false;
    HWND list = FindWindowExW(best, nullptr, L"MSTaskListWClass", nullptr);
    if (!list) {
        HWND rebar = FindWindowExW(best, nullptr, L"ReBarWindow32", nullptr);
        HWND sw =
            rebar ? FindWindowExW(rebar, nullptr, L"MSTaskSwWClass", nullptr)
                  : nullptr;
        if (sw)
            list = FindWindowExW(sw, nullptr, L"MSTaskListWClass", nullptr);
    }
    RECT area = bestRc;
    if (list) {
        RECT lr{};
        if (GetWindowRect(list, &lr) && IsRectUsable(lr))
            area = lr;
    }
    const LONG cx = (area.left + area.right) / 2,
               cy = (area.top + area.bottom) / 2;
    rc->left = cx - 12;
    rc->top = cy - 12;
    rc->right = cx + 12;
    rc->bottom = cy + 12;
    return IsRectUsable(*rc);
}
static bool GetMinimizeRectPhysical(HWND hwnd, RECT* rc) {
    if (pGetWindowMinimizeRect && pGetWindowMinimizeRect(hwnd, rc) &&
        IsRectUsable(*rc))
        return true;
    if (GetTaskbarTargetPhysical(hwnd, rc))
        return true;
    HMONITOR hmon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{sizeof(mi)};
    if (!GetMonitorInfoW(hmon, &mi))
        return false;
    rc->left = mi.rcWork.left + 8;
    rc->bottom = mi.rcWork.bottom - 8;
    rc->right = rc->left + 24;
    rc->top = rc->bottom - 24;
    return IsRectUsable(*rc);
}
// Deliberately does NOT probe WM_GETMINMAXINFO: that message is app code and
// answers in the app's own coordinate space, while everything this rect is
// mixed with (rcMin, the capture) is taken under the PMv2 DPI override and is
// physical. Mixing the two spaces sends restore-to-maximized animations to the
// wrong place on a secondary monitor, or on a work area that doesn't start at
// the monitor origin. The work area covers the overwhelming majority of
// cases, so this is called under the same ScopedDpiAware override as
// rcMin/the capture.
static bool GetMaximizeRectPhysical(HWND hwnd, RECT* rc) {
    HMONITOR hmon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{sizeof(mi)};
    if (!GetMonitorInfoW(hmon, &mi))
        return false;
    *rc = mi.rcWork;
    return IsRectUsable(*rc);
}
// rcNormalPosition of a top-level window without WS_EX_TOOLWINDOW is reported
// in "workspace coordinates" (MSDN, WINDOWPLACEMENT): screen coordinates
// shifted by the top/left work-area inset of the monitor the rect lives on,
// i.e. by the height/width of a taskbar or appbar docked at that monitor's
// top or left edge. The window itself is minimized (parked at -32000,-32000)
// when this runs, so MonitorFromWindow is useless for picking that monitor:
// resolve it from the rect instead. The rect is still in workspace
// coordinates when it is first resolved, which can pick the wrong monitor
// when the inset pushes it across a monitor boundary, so resolve once more
// after the shift (the same double resolution PowerToys FancyZones uses for
// SetWindowPlacement). Wine, ReactOS and Chromium apply the same per-monitor
// inset; nothing in this conversion refers specifically to the primary
// monitor beyond MONITOR_DEFAULTTOPRIMARY as the off-screen fallback.
static bool WorkAreaInset(HMONITOR hmon, LONG* dx, LONG* dy) {
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (!hmon || !GetMonitorInfoW(hmon, &mi))
        return false;
    *dx = mi.rcWork.left - mi.rcMonitor.left;
    *dy = mi.rcWork.top - mi.rcMonitor.top;
    return true;
}
static void WorkspaceToScreen(RECT* rc) {
    // First guess: the monitor the (still workspace-relative) rect lands on.
    HMONITOR hmon = MonitorFromRect(rc, MONITOR_DEFAULTTOPRIMARY);
    LONG dx = 0, dy = 0;
    if (!WorkAreaInset(hmon, &dx, &dy))
        return;
    RECT shifted = *rc;
    OffsetRect(&shifted, dx, dy);
    // If the shift carried the rect onto a different monitor, the inset that
    // actually applies is that monitor's: redo the shift from the original.
    HMONITOR hmon2 = MonitorFromRect(&shifted, MONITOR_DEFAULTTOPRIMARY);
    if (hmon2 && hmon2 != hmon && WorkAreaInset(hmon2, &dx, &dy)) {
        shifted = *rc;
        OffsetRect(&shifted, dx, dy);
    }
    *rc = shifted;
}
static bool GetRestoreRectPhysical(HWND hwnd, RECT* rc) {
    if (!rc)
        return false;
    WINDOWPLACEMENT wp{};
    wp.length = sizeof(wp);
    if (!GetWindowPlacement(hwnd, &wp))
        return false;
    *rc = wp.rcNormalPosition;
    LONG style = LONG(GetWindowLongPtrW(hwnd, GWL_STYLE)),
         ex = LONG(GetWindowLongPtrW(hwnd, GWL_EXSTYLE));
    if (style & WS_CHILD) {
        // Never reached for animated windows (IsAnimateCandidate rejects
        // WS_CHILD), kept for completeness: a child's placement is relative
        // to its parent's client area, so map it to the screen.
        if (HWND parent = GetParent(hwnd))
            MapWindowPoints(parent, HWND_DESKTOP, LPPOINT(rc), 2);
    } else if (!(ex & WS_EX_TOOLWINDOW)) {
        WorkspaceToScreen(rc);
    }
    return IsRectUsable(*rc);
}
static void DisableTransitions(HWND hwnd, BOOL dis) {
    if (!hwnd || !IsWindow(hwnd))
        return;
    DwmSetWindowAttribute(hwnd, DWMWA_TRANSITIONS_FORCEDISABLED, &dis,
                          sizeof(dis));
}

struct Vertex {
    float x, y, u, v;
};
static float Edge(const Vertex& a, const Vertex& b, const Vertex& c) {
    return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}
static uint32_t SampleBilinear(const uint32_t* src, int sw, int sh, float u,
                               float v) {
    u = std::clamp(u, 0.f, 1.f) * float(sw - 1);
    v = std::clamp(v, 0.f, 1.f) * float(sh - 1);
    int x0 = std::clamp(int(u), 0, sw - 1), y0 = std::clamp(int(v), 0, sh - 1),
        x1 = std::min(x0 + 1, sw - 1), y1 = std::min(y0 + 1, sh - 1);
    float fx = u - float(x0), fy = v - float(y0);
    auto unpack = [](uint32_t p, float& b, float& g, float& r) {
        b = float(p & 0xFF);
        g = float((p >> 8) & 0xFF);
        r = float((p >> 16) & 0xFF);
    };
    float b00, g00, r00, b10, g10, r10, b01, g01, r01, b11, g11, r11;
    unpack(src[y0 * sw + x0], b00, g00, r00);
    unpack(src[y0 * sw + x1], b10, g10, r10);
    unpack(src[y1 * sw + x0], b01, g01, r01);
    unpack(src[y1 * sw + x1], b11, g11, r11);
    float b0 = b00 + (b10 - b00) * fx, g0 = g00 + (g10 - g00) * fx,
          r0 = r00 + (r10 - r00) * fx, b1 = b01 + (b11 - b01) * fx,
          g1 = g01 + (g11 - g01) * fx, r1 = r01 + (r11 - r01) * fx;
    BYTE b = BYTE(b0 + (b1 - b0) * fy + 0.5f),
         g = BYTE(g0 + (g1 - g0) * fy + 0.5f),
         r = BYTE(r0 + (r1 - r0) * fy + 0.5f);
    return uint32_t(b) | (uint32_t(g) << 8) | (uint32_t(r) << 16);
}
static inline float CubicWeight(float x) {
    x = std::fabs(x);
    const float a = -0.5f;
    if (x <= 1.f)
        return (a + 2.f) * x * x * x - (a + 3.f) * x * x + 1.f;
    if (x < 2.f)
        return a * x * x * x - 5.f * a * x * x + 8.f * a * x - 4.f * a;
    return 0.f;
}
static uint32_t SampleBicubic(const uint32_t* src, int sw, int sh, float u,
                              float v) {
    if (sw <= 2 || sh <= 2)
        return SampleBilinear(src, sw, sh, u, v);
    float fx = std::clamp(u, 0.f, 1.f) * float(sw - 1),
          fy = std::clamp(v, 0.f, 1.f) * float(sh - 1);
    int ix = int(std::floor(fx)), iy = int(std::floor(fy));
    float tx = fx - float(ix), ty = fy - float(iy);
    float wx[4], wy[4];
    for (int i = 0; i < 4; ++i) {
        wx[i] = CubicWeight(float(i - 1) - tx);
        wy[i] = CubicWeight(float(i - 1) - ty);
    }
    float b = 0, g = 0, r = 0;
    for (int j = 0; j < 4; ++j) {
        int sy = std::clamp(iy + j - 1, 0, sh - 1);
        float rb = 0, rg = 0, rr = 0;
        for (int i = 0; i < 4; ++i) {
            int sx = std::clamp(ix + i - 1, 0, sw - 1);
            uint32_t p = src[sy * sw + sx];
            rb += wx[i] * float(p & 0xFF);
            rg += wx[i] * float((p >> 8) & 0xFF);
            rr += wx[i] * float((p >> 16) & 0xFF);
        }
        b += wy[j] * rb;
        g += wy[j] * rg;
        r += wy[j] * rr;
    }
    auto clampByte = [](float v) {
        return BYTE(std::clamp(v, 0.f, 255.f) + 0.5f);
    };
    return uint32_t(clampByte(b)) | (uint32_t(clampByte(g)) << 8) |
           (uint32_t(clampByte(r)) << 16);
}
// Bilinear downscale to the compact cache format, used for every cached
// capture except the most recent one (which stays full-size, see
// CacheCapture/DemoteOlderLocked). The target is half the window's own size,
// floored at kMaxCachedSide -- so a small window isn't downscaled at all,
// while a large one keeps proportionally more detail than a fixed low cap
// would give. kMaxCachedBytes remains the real backstop against unbounded
// memory for pathologically large windows.
static CaptureBits DownscaleForCache(const CaptureBits& in) {
    if (in.empty())
        return {};
    int w = in.width, h = in.height;
    if (w <= kMaxCachedSide && h <= kMaxCachedSide)
        return in;
    int target = std::max(kMaxCachedSide, std::max(w, h) / 2);
    if (w <= target && h <= target)
        return in;
    double s = std::min(double(target) / double(w),
                        double(target) / double(h));
    int nw = std::max(1, int(std::lround(w * s))),
        nh = std::max(1, int(std::lround(h * s)));
    CaptureBits out;
    out.width = nw;
    out.height = nh;
    out.srcW = w;
    out.srcH = h;
    try {
        out.pixels.resize(size_t(nw) * size_t(nh));
    } catch (...) {
        return {};
    }
    for (int y = 0; y < nh; ++y) {
        float v = (float(y) + 0.5f) / float(nh);
        for (int x = 0; x < nw; ++x) {
            float u = (float(x) + 0.5f) / float(nw);
            out.pixels[size_t(y) * size_t(nw) + size_t(x)] =
                SampleBilinear(in.pixels.data(), w, h, u, v);
        }
    }
    ForceOpaqueAlpha(out.pixels.data(), out.pixels.size());
    return out;
}
// Barycentrics and UVs are stepped per pixel (exact at each row start) and the rows
// are split over the thread pool: same pixels, far cheaper on full-size frames.
static void RasterTriangle(uint32_t* dst, int stride, int dw, int dh,
                           const uint32_t* src, int sw, int sh, Vertex v0,
                           Vertex v1, Vertex v2, BYTE alpha, bool hq,
                           int clipY0, int clipY1) {
    float area = Edge(v0, v1, v2);
    if (std::fabs(area) < 0.5f)
        return;
    const double inv = 1.0 / double(area);
    int minX = int(std::floor(std::min({v0.x, v1.x, v2.x}))),
        maxX = int(std::ceil(std::max({v0.x, v1.x, v2.x})));
    int minY = int(std::floor(std::min({v0.y, v1.y, v2.y}))),
        maxY = int(std::ceil(std::max({v0.y, v1.y, v2.y})));
    minX = std::clamp(minX, 0, dw - 1);
    maxX = std::clamp(maxX, 0, dw - 1);
    minY = std::clamp(std::max(minY, clipY0), 0, dh - 1);
    maxY = std::clamp(std::min(maxY, clipY1), 0, dh - 1);
    const float af = float(alpha) / 255.f;
    const double w0dx = double(v2.y - v1.y) * inv,
                 w1dx = double(v0.y - v2.y) * inv,
                 w2dx = double(v1.y - v0.y) * inv;
    const double udx = w0dx * v0.u + w1dx * v1.u + w2dx * v2.u,
                 vdx = w0dx * v0.v + w1dx * v1.v + w2dx * v2.v;
    for (int y = minY; y <= maxY; ++y) {
        uint32_t* row = dst + size_t(y) * size_t(stride);
        Vertex p{float(minX) + 0.5f, float(y) + 0.5f, 0, 0};
        double w0 = double(Edge(v1, v2, p)) * inv,
               w1 = double(Edge(v2, v0, p)) * inv,
               w2 = double(Edge(v0, v1, p)) * inv;
        double u = w0 * v0.u + w1 * v1.u + w2 * v2.u,
               v = w0 * v0.v + w1 * v1.v + w2 * v2.v;
        for (int x = minX; x <= maxX;
             ++x, w0 += w0dx, w1 += w1dx, w2 += w2dx, u += udx, v += vdx) {
            if (w0 < 0 || w1 < 0 || w2 < 0)
                continue;
            uint32_t s = hq ? SampleBicubic(src, sw, sh, float(u), float(v))
                            : SampleBilinear(src, sw, sh, float(u), float(v));
            float sb = float(s & 0xFF), sg = float((s >> 8) & 0xFF),
                  sr = float((s >> 16) & 0xFF);
            BYTE b = BYTE(sb * af + 0.5f), g = BYTE(sg * af + 0.5f),
                 r = BYTE(sr * af + 0.5f);
            row[x] = uint32_t(b) | (uint32_t(g) << 8) | (uint32_t(r) << 16) |
                     (uint32_t(alpha) << 24);
        }
    }
}
struct RasterBands {
    uint32_t* dst = nullptr;
    int stride = 0, dw = 0, dh = 0;
    const uint32_t* src = nullptr;
    int sw = 0, sh = 0;
    const Vertex* c = nullptr;
    BYTE alpha = 255;
    bool hq = false;
    int y0 = 0, bandH = 1, bands = 1;
    std::atomic<int> next{0};
};
static void RasterBand(RasterBands& j, int band) {
    const int y0 = j.y0 + band * j.bandH, y1 = y0 + j.bandH - 1;
    RasterTriangle(j.dst, j.stride, j.dw, j.dh, j.src, j.sw, j.sh, j.c[0],
                   j.c[1], j.c[2], j.alpha, j.hq, y0, y1);
    RasterTriangle(j.dst, j.stride, j.dw, j.dh, j.src, j.sw, j.sh, j.c[0],
                   j.c[2], j.c[3], j.alpha, j.hq, y0, y1);
}
static void CALLBACK RasterWorkCallback(PTP_CALLBACK_INSTANCE, PVOID ctx,
                                        PTP_WORK) {
    auto* j = static_cast<RasterBands*>(ctx);
    for (int b = j->next.fetch_add(1); b < j->bands; b = j->next.fetch_add(1))
        RasterBand(*j, b);
}
static void RasterQuad(uint32_t* dst, int stride, int dw, int dh,
                       const uint32_t* src, int sw, int sh, const Vertex c[4],
                       BYTE alpha, bool hq) {
    if (dw <= 0 || dh <= 0 || sw <= 0 || sh <= 0 || !dst || !src || alpha == 0)
        return;
    const int y0 = std::clamp(
        int(std::floor(std::min({c[0].y, c[1].y, c[2].y, c[3].y}))), 0, dh - 1);
    const int y1 = std::clamp(
        int(std::ceil(std::max({c[0].y, c[1].y, c[2].y, c[3].y}))), 0, dh - 1);
    const int rows = y1 - y0 + 1;
    static int s_cpus = 0;
    if (!s_cpus) {
        SYSTEM_INFO si{};
        GetSystemInfo(&si);
        s_cpus = int(si.dwNumberOfProcessors ? si.dwNumberOfProcessors : 1);
    }
    int workers = 1;
    if (rows > 128 && size_t(rows) * size_t(dw) > size_t(400000) && s_cpus > 1)
        workers = std::min({s_cpus, 4, rows / 64});
    if (workers > 1) {
        RasterBands j;
        j.dst = dst;
        j.stride = stride;
        j.dw = dw;
        j.dh = dh;
        j.src = src;
        j.sw = sw;
        j.sh = sh;
        j.c = c;
        j.alpha = alpha;
        j.hq = hq;
        j.bands = workers * 2;
        j.bandH = (rows + j.bands - 1) / j.bands;
        j.bands = (rows + j.bandH - 1) / j.bandH;
        j.y0 = y0;
        ScopedThreadpoolWork work(
            CreateThreadpoolWork(RasterWorkCallback, &j, nullptr));
        if (work) {
            for (int i = 1; i < workers; ++i)
                SubmitThreadpoolWork(work.get());
            RasterWorkCallback(nullptr, &j, nullptr);
            return;
        }
    }
    RasterTriangle(dst, stride, dw, dh, src, sw, sh, c[0], c[1], c[2], alpha,
                   hq, y0, y1);
    RasterTriangle(dst, stride, dw, dh, src, sw, sh, c[0], c[2], c[3], alpha,
                   hq, y0, y1);
}
class PresentGdi {
  public:
    PresentGdi() = default;
    PresentGdi(const PresentGdi&) = delete;
    PresentGdi& operator=(const PresentGdi&) = delete;
    ~PresentGdi() { Release(); }
    bool EnsureSource(const CaptureBits& cap) {
        if (m_hdcSrc)
            return true;
        if (!EnsureScreenDc())
            return false;
        BITMAPINFO bmi{};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = cap.width;
        bmi.bmiHeader.biHeight = -cap.height;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        m_hbmSrc = CreateDIBSection(m_hdcScreen, &bmi, DIB_RGB_COLORS, &m_pvSrc,
                                    nullptr, 0);
        if (!m_hbmSrc || !m_pvSrc) {
            Release();
            return false;
        }
        std::memcpy(m_pvSrc, cap.pixels.data(), cap.pixels.size() * 4);
        GdiFlush();
        m_hdcSrc = CreateCompatibleDC(m_hdcScreen);
        if (!m_hdcSrc) {
            Release();
            return false;
        }
        SelectObject(m_hdcSrc, m_hbmSrc);
        return true;
    }
    bool EnsureDest(int dw, int dh) {
        if (m_hdcDst && dw <= m_dstW && dh <= m_dstH)
            return true;
        if (m_hdcDst) {
            DeleteDC(m_hdcDst);
            m_hdcDst = nullptr;
        }
        if (m_hbmDst) {
            DeleteObject(m_hbmDst);
            m_hbmDst = nullptr;
            m_pvDst = nullptr;
        }
        if (!EnsureScreenDc())
            return false;
        BITMAPINFO bmi{};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = dw;
        bmi.bmiHeader.biHeight = -dh;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        m_hbmDst = CreateDIBSection(m_hdcScreen, &bmi, DIB_RGB_COLORS, &m_pvDst,
                                    nullptr, 0);
        if (!m_hbmDst || !m_pvDst) {
            Release();
            return false;
        }
        m_hdcDst = CreateCompatibleDC(m_hdcScreen);
        if (!m_hdcDst) {
            Release();
            return false;
        }
        m_dstW = dw;
        m_dstH = dh;
        SelectObject(m_hdcDst, m_hbmDst);
        SetStretchBltMode(m_hdcDst, HALFTONE);
        SetBrushOrgEx(m_hdcDst, 0, 0, nullptr);
        return true;
    }
    void Release() {
        if (m_hdcDst) {
            DeleteDC(m_hdcDst);
            m_hdcDst = nullptr;
        }
        if (m_hbmDst) {
            DeleteObject(m_hbmDst);
            m_hbmDst = nullptr;
            m_pvDst = nullptr;
        }
        if (m_hdcSrc) {
            DeleteDC(m_hdcSrc);
            m_hdcSrc = nullptr;
        }
        if (m_hbmSrc) {
            DeleteObject(m_hbmSrc);
            m_hbmSrc = nullptr;
            m_pvSrc = nullptr;
        }
        if (m_hdcScreen) {
            DeleteDC(m_hdcScreen);
            m_hdcScreen = nullptr;
        }
    }
    HDC hdcScreen() const { return m_hdcScreen; }
    HDC hdcDst() const { return m_hdcDst; }
    HDC hdcSrc() const { return m_hdcSrc; }
    void* dstBits() const { return m_pvDst; }
    int dstStride() const { return m_dstW; }

  private:
    // GetDC/ReleaseDC(nullptr,...) must be paired on the same thread; this object
    // can be created on the overlay thread and destroyed on the app thread (the
    // two close paths), so a plain DC of the whole display has no such affinity.
    bool EnsureScreenDc() {
        if (m_hdcScreen)
            return true;
        m_hdcScreen = CreateDCW(L"DISPLAY", nullptr, nullptr, nullptr);
        return m_hdcScreen != nullptr;
    }
    HDC m_hdcScreen = nullptr, m_hdcSrc = nullptr, m_hdcDst = nullptr;
    HGDIOBJ m_hbmSrc = nullptr, m_hbmDst = nullptr;
    void *m_pvSrc = nullptr, *m_pvDst = nullptr;
    int m_dstW = 0, m_dstH = 0;
};
struct AnimRequest {
    HWND hwnd = nullptr;
    AnimationType type = AnimationType::None;
    RECT rcWindow{};
    RECT rcDest{};
    CaptureBits capture;
    UINT durationMs = 250;
    std::unique_ptr<PresentGdi> gdi;
    // Close path only: a manual-reset event the closing app's thread waits
    // on while the overlay thread runs the timeline. It is signalled from the
    // destructor, so EVERY way a request can die (FinishQueued, a failed
    // QueueRun, SafeCleanup draining the queue at unload) wakes the waiter --
    // an app thread must never be left parked on an event nobody will set.
    HANDLE hDone = nullptr;

    AnimRequest() = default;
    AnimRequest(const AnimRequest&) = delete;
    AnimRequest& operator=(const AnimRequest&) = delete;
    AnimRequest(AnimRequest&& o) noexcept
        : hwnd(o.hwnd), type(o.type), rcWindow(o.rcWindow), rcDest(o.rcDest),
          capture(std::move(o.capture)), durationMs(o.durationMs),
          gdi(std::move(o.gdi)), hDone(o.hDone) {
        o.hDone = nullptr;
    }
    AnimRequest& operator=(AnimRequest&&) = delete;
    ~AnimRequest() { SignalDone(); }
    void SignalDone() noexcept {
        if (hDone) {
            SetEvent(hDone);
            CloseHandle(hDone);
            hDone = nullptr;
        }
    }
};

static bool PresentOverlay(HWND hwndOverlay, PresentGdi& gdi,
                           const AnimRequest& req, const RECT& rcCurrent,
                           const Win7TransformParams& params,
                           bool hqFinal = false) {
    BYTE alpha = BYTE(std::clamp(params.opacity, 0.f, 1.f) * 255.f + 0.5f);
    if (alpha == 0 || !IsRectUsable(rcCurrent)) {
        POINT pt{rcCurrent.left, rcCurrent.top};
        SIZE sz{1, 1};
        BLENDFUNCTION bf{AC_SRC_OVER, 0, 0, AC_SRC_ALPHA};
        UpdateLayeredWindow(hwndOverlay, nullptr, &pt, &sz, nullptr, nullptr, 0,
                            &bf, ULW_ALPHA);
        return true;
    }
    const CaptureBits& cap = req.capture;
    float ow = float(cap.width), oh = float(cap.height);
    bool isClose = (req.type == AnimationType::Close);
    bool tiny3d = !isClose && std::fabs(params.rotX) < 0.35f &&
                  std::fabs(params.rotY) < 0.35f &&
                  std::fabs(params.transZ) < 0.25f;
    RECT bbox = rcCurrent;
    Vertex corners[4]{};
    if (!tiny3d) {
        Matrix4x4F m = BuildCornerMatrix(params, rcCurrent, ow, oh, req.type);
        float xs[4] = {0, ow, ow, 0}, ys[4] = {0, 0, oh, oh},
              us[4] = {0, 1, 1, 0}, vs[4] = {0, 0, 1, 1};
        float minX = 1e9f, minY = 1e9f, maxX = -1e9f, maxY = -1e9f, sx[4],
              sy[4];
        for (int i = 0; i < 4; ++i) {
            m.TransformPoint(xs[i], ys[i], 0, sx[i], sy[i]);
            minX = std::min(minX, sx[i]);
            minY = std::min(minY, sy[i]);
            maxX = std::max(maxX, sx[i]);
            maxY = std::max(maxY, sy[i]);
        }
        bbox.left = LONG(std::floor(minX)) - 3;
        bbox.top = LONG(std::floor(minY)) - 3;
        bbox.right = LONG(std::ceil(maxX)) + 3;
        bbox.bottom = LONG(std::ceil(maxY)) + 3;
        for (int i = 0; i < 4; ++i)
            corners[i] = {sx[i] - bbox.left, sy[i] - bbox.top, us[i], vs[i]};
    }
    if (!IsRectUsable(bbox))
        return false;
    int dw = RECTW(bbox), dh = RECTH(bbox);
    if (dw > 16384 || dh > 16384)
        return false;
    if (!gdi.EnsureSource(cap))
        return false;
    if (!gdi.EnsureDest(dw, dh))
        return false;
    void* bits = gdi.dstBits();
    int stride = gdi.dstStride();
    if (tiny3d) {
        StretchBlt(gdi.hdcDst(), 0, 0, dw, dh, gdi.hdcSrc(), 0, 0, cap.width,
                   cap.height, SRCCOPY);
        GdiFlush();
        BYTE lut[256];
        for (int i = 0; i < 256; ++i)
            lut[i] = BYTE((i * alpha) / 255);
        auto* px = static_cast<uint32_t*>(bits);
        for (int y = 0; y < dh; ++y) {
            uint32_t* row = px + size_t(y) * size_t(stride);
            for (int x = 0; x < dw; ++x) {
                uint32_t p = row[x];
                BYTE b = lut[p & 0xFF], g = lut[(p >> 8) & 0xFF],
                     r = lut[(p >> 16) & 0xFF];
                row[x] = uint32_t(b) | (uint32_t(g) << 8) |
                         (uint32_t(r) << 16) | (uint32_t(alpha) << 24);
            }
        }
    } else {
        for (int y = 0; y < dh; ++y)
            std::memset(static_cast<uint32_t*>(bits) +
                            size_t(y) * size_t(stride),
                        0, size_t(dw) * 4);
        RasterQuad(static_cast<uint32_t*>(bits), stride, dw, dh,
                   cap.pixels.data(), cap.width, cap.height, corners, alpha,
                   hqFinal);
    }
    POINT pt{bbox.left, bbox.top};
    SIZE sz{dw, dh};
    POINT srcPt{0, 0};
    BLENDFUNCTION bf{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    if (!UpdateLayeredWindow(hwndOverlay, gdi.hdcScreen(), &pt, &sz,
                             gdi.hdcDst(), &srcPt, 0, &bf, ULW_ALPHA))
        return false;
    return true;
}
static void ShowOverlayWindow(HWND hwndOverlay) {
    if (ShowWindow_orig)
        ShowWindow_orig(hwndOverlay, SW_SHOWNA);
    else
        ::ShowWindow(hwndOverlay, SW_SHOWNA);
    SetWindowPos(hwndOverlay, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}
static void HideOverlayWindow(HWND hwndOverlay) {
    POINT pt{0, 0};
    SIZE sz{1, 1};
    BLENDFUNCTION bf{AC_SRC_OVER, 0, 0, AC_SRC_ALPHA};
    UpdateLayeredWindow(hwndOverlay, nullptr, &pt, &sz, nullptr, nullptr, 0,
                        &bf, ULW_ALPHA);
    SetWindowPos(hwndOverlay, HWND_BOTTOM, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_HIDEWINDOW);
}

enum class AnimMsg : UINT {
    FirstFrame = 1,
    Drain = 2,
    Hide = 3,
    GhostArm = 4
};
constexpr UINT_PTR kGhostTimer = 0x57A1;
// Explorer and Control Panel hide their frame before destroying it, so at DestroyWindow
// the window is already invisible; PlayClose can't screen-scrape it there. This is the
// grace window: a pre-capture taken right before the hide and used if the destroy follows
// within kShellHideToCloseMs.
constexpr ULONGLONG kShellHideToCloseMs = 400;
static void GhostWatchdog();
static std::atomic<UINT> g_msgAnim{0};
// The single animation slot. Whoever holds it (CAS false->true) owns the
// overlay window until it is released: StartQueuedAnimation/PlayCloseAnimation
// hand it to the overlay thread (FinishQueued releases it), and
// ShellPreCaptureForClose holds it across its capture-and-present. That is
// what makes SendFirstFrame below race-free and, just as importantly, what
// guarantees the overlay thread is idle in its message loop whenever someone
// does a blocking SendMessageW to it -- a caller that skipped the slot could
// otherwise block behind a running animation for its whole duration.
static std::atomic<bool> g_fAnimating{false};
static std::atomic<HWND> g_hwndAnim{nullptr}, g_hwndCurrent{nullptr};
static std::atomic<int> g_typeCurrent{0};
// AnimMsg::FirstFrame used to carry an AnimRequest* straight in lParam. Both
// the window class name and the registered message are process-wide public
// values (any process can FindWindowW + RegisterWindowMessageW the same
// strings), so anything on the machine could send that message with an
// arbitrary lParam and make the overlay thread dereference it. The pointer
// now travels only through this mod-owned, mutex-guarded slot; the message
// itself carries no payload. Every sender holds g_fAnimating, so there is no
// contention on the slot -- just don't hold the lock across a cross-thread
// send.
static std::mutex g_firstFrameMutex;
static AnimRequest* g_firstFrameReq = nullptr;  // guarded by g_firstFrameMutex
static LRESULT SendFirstFrame(HWND hwndAnim, UINT msg, AnimRequest* req) {
    // g_firstFrameMutex must NOT be held across SendMessageW: it's a
    // blocking cross-thread call that waits for AnimWndProc to run, and
    // AnimWndProc needs to take this same mutex to read the pointer back.
    // Holding it here would deadlock the sender against its own receiver.
    // Callers serialize via g_fAnimating (asserted below), so there's no
    // race in setting-then-sending-then-clearing without holding the lock
    // across all three steps.
    if (!g_fAnimating.load()) {
        Wh_Log(L"SendFirstFrame called without the animation slot -- bug");
        return 0;
    }
    {
        std::lock_guard<std::mutex> lock(g_firstFrameMutex);
        if (g_firstFrameReq) {
            Wh_Log(L"SendFirstFrame: slot already in use -- bug");
            return 0;
        }
        g_firstFrameReq = req;
    }
    LRESULT r = SendMessageW(hwndAnim, msg, WPARAM(AnimMsg::FirstFrame), 0);
    {
        std::lock_guard<std::mutex> lock(g_firstFrameMutex);
        g_firstFrameReq = nullptr;
    }
    return r;
}
// Availability of the mod in this process. g_stopping is permanent (unload).
// A failure to bring the overlay thread up is NOT permanent any more: it only
// disables the mod until g_retryNotBefore, with a backoff that doubles from
// 2 s to 60 s, so a transient hiccup (RegisterClassW losing a race with a
// previous instance's teardown, the 1 s startup wait timing out on a loaded
// machine) no longer silently kills the mod until the app is restarted.
// Wh_ModSettingsChanged also clears the backoff so the user can force a retry.
static std::atomic<bool> g_stopping{false};
static std::atomic<ULONGLONG> g_retryNotBefore{0};
static std::atomic<DWORD> g_retryBackoffMs{0};
static bool IsDisabled() {
    if (g_stopping.load())
        return true;
    ULONGLONG t = g_retryNotBefore.load();
    return t != 0 && GetTickCount64() < t;
}
static void NoteOverlayThreadFailure(const wchar_t* why) {
    DWORD b = g_retryBackoffMs.load();
    b = b ? std::min<DWORD>(b * 2, 60000) : 2000;
    g_retryBackoffMs.store(b);
    g_retryNotBefore.store(GetTickCount64() + b);
    Wh_Log(L"overlay thread unavailable (%s); animations paused for %u ms",
           why, b);
}
static void ClearOverlayThreadFailure() {
    g_retryBackoffMs.store(0);
    g_retryNotBefore.store(0);
}
// During a restore-from-minimized fly-in the real window must not be rendered,
// or the user would see it pop open first and the overlay catch up afterwards.
// ShowWindow(SW_RESTORE) still changes state synchronously (not iconic, normal
// rects, activatable); DWM cloaking only keeps the window off the screen for the
// ~210 ms of the animation, then the overlay's last frame is swapped for the
// real surface. Cloaking is preferred over SW_HIDE because a hidden window
// defeats the classic restore;SetForegroundWindow() pattern, a cloaked one does
// not.
static std::atomic<HWND> g_cloakedHwnd{nullptr};
static void CloakForRestoreAnim(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd))
        return;
    if (g_hwndCurrent.load() != hwnd)
        return; // its animation was superseded
    BOOL already = FALSE;
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &already,
                                        sizeof(already))) &&
        already)
        return; // cloaked by someone else (e.g. UWP self-cloak): not ours to manage
    BOOL on = TRUE;
    if (SUCCEEDED(DwmSetWindowAttribute(hwnd, DWMWA_CLOAK, &on, sizeof(on))))
        g_cloakedHwnd.store(hwnd);
}
static void UncloakAfterRestore(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd))
        return;
    BOOL off = FALSE;
    DwmSetWindowAttribute(hwnd, DWMWA_CLOAK, &off, sizeof(off));
}
static LONG g_isUninitializing = 0;
static HINSTANCE g_hinst = nullptr;
static std::mutex g_animThreadMutex;
static HANDLE g_hAnimWndThread = nullptr;
static DWORD g_dwAnimThreadId = 0;
static std::mutex g_queueMutex;
static std::deque<AnimRequest*> g_queue;
static void PresentTime(HWND hwndOverlay, AnimRequest& req, float t) {
    auto p = ParamsFor(req.type, t, float(RECTH(req.rcWindow)));
    auto rc = RectFor(req.type, t, req.rcWindow, req.rcDest);
    PresentOverlay(hwndOverlay, *req.gdi, req, rc, p, t >= 1.f);
}
static bool PresentFirstFrame(HWND hwndOverlay, AnimRequest& req) {
    if (req.capture.empty() || !IsRectUsable(req.rcWindow))
        return false;
    ShowOverlayWindow(hwndOverlay);
    PresentTime(hwndOverlay, req, 0.f);
    return true;
}
// Frame pacing for RunAnimation. DwmFlush() is the primary pacer (it returns
// once per composition pass, i.e. per vblank), but it is not reliable: it
// fails outright while the DWM is restarting or in some RDP sessions, and it
// can return immediately when composition is idle. Without a floor the loop
// would then spin, re-rasterizing every iteration and fanning out threadpool
// workers for frames nobody can see. So each presented frame is stamped, and
// a new one is not rasterized until at least kMinFramePeriodMs has passed --
// the remainder is slept instead. 8 ms allows up to 120 Hz displays through
// while still capping the worst case at ~125 rasterizations per second.
constexpr DWORD kMinFramePeriodMs = 8;
// A single DwmFlush that blocks this long means the compositor is not
// ticking normally (restart, hung, or a session without composition timing);
// for the rest of that timeline the loop paces on Sleep alone, so one bad
// call is the most a compositor stall can cost the animation.
constexpr ULONGLONG kMaxDwmFlushMs = 100;
static void PaceFrame(ULONGLONG lastPresentTick, bool& useDwmFlush) {
    HRESULT hr = E_FAIL;
    if (useDwmFlush) {
        ULONGLONG before = GetTickCount64();
        hr = DwmFlush();
        if (FAILED(hr) || GetTickCount64() - before > kMaxDwmFlushMs)
            useDwmFlush = false;
    }
    ULONGLONG since = GetTickCount64() - lastPresentTick;
    if (since < kMinFramePeriodMs) {
        // DwmFlush came back early (or wasn't used): top up to the minimum
        // period so the loop never rasterizes faster than ~125 fps.
        Sleep(DWORD(kMinFramePeriodMs - since));
    } else if (FAILED(hr)) {
        // No compositor to pace against and a frame is already due: yield
        // one scheduler tick so a broken DwmFlush can't turn into a hot spin.
        Sleep(1);
    }
}
// Runs the overlay timeline, on the animation thread only. The minimize and
// restore hooks never wait on this: the caller's UI thread is back in
// application code long before the animation finishes. The close path waits
// on req.hDone (see PlayCloseAnimation), which the request signals as soon as
// FinishQueued deletes it.
//
// The loop is bounded by wall clock, not by frame count: t is derived from
// the elapsed time and the loop ends `dur` ms after it started no matter how
// many frames were (or were not) presented in between. That is what keeps
// the restore path's uncloak (FinishQueued) and the close path's wait
// predictable even when the pacer misbehaves; the frame floor in PaceFrame
// only decides how much work is done inside that fixed window.
static void RunAnimation(HWND hwndOverlay, AnimRequest& req) {
    if (req.capture.empty() || !IsRectUsable(req.rcWindow))
        return;
    if (g_stopping.load())
        return;
    UINT dur = req.durationMs ? req.durationMs : 250;
    ShowOverlayWindow(hwndOverlay);
    if (req.gdi)
        req.gdi->EnsureSource(req.capture);
    const ULONGLONG start = GetTickCount64();
    ULONGLONG lastPresent = 0;
    bool useDwmFlush = true;
    float lastT = -1;
    // A cancellation (g_hwndCurrent no longer pointing at this request, e.g.
    // AfterOrigMinimize clearing it for a synchronous re-minimize) is
    // distinguished from a normal timeout so the cancelled animation doesn't
    // still paint its final frame below.
    bool cancelled = false;
    for (;;) {
        if (g_stopping.load())
            break;
        if (g_hwndCurrent.load() != req.hwnd) {
            cancelled = true;
            break;
        }
        ULONGLONG elapsed = GetTickCount64() - start;
        if (elapsed >= dur)
            break;
        float t = std::clamp(float(elapsed) / float(dur), 0.f, 1.f);
        if (t - lastT >= 0.001f) {
            lastT = t;
            PresentTime(hwndOverlay, req, t);
            lastPresent = GetTickCount64();
        }
        PaceFrame(lastPresent, useDwmFlush);
    }
    if (!g_stopping.load() && !cancelled)
        PresentTime(hwndOverlay, req, 1.f);
}
static void FinishQueued(AnimRequest* req) {
    if (!req)
        return;
    // Restore path: the real window stayed cloaked while the overlay flew in.
    // Uncloak it UNDER the overlay's final frame and only then drop the
    // overlay, so no frame can ever show both layers or neither.
    if (req->type == AnimationType::RestoreFromMinimized && req->hwnd) {
        HWND c = g_cloakedHwnd.load();
        if (c == req->hwnd) {
            g_cloakedHwnd.store(nullptr);
            UncloakAfterRestore(req->hwnd);
        }
    }
    HWND ha = g_hwndAnim.load();
    if (ha && IsWindow(ha))
        HideOverlayWindow(ha);
    // Close requests keep DWM transitions disabled: the (hidden) window is
    // destroyed by the waiting app thread right after this, and re-enabling
    // now would let the stock DWM close effect play on top of ours.
    if (req->type != AnimationType::Close && req->hwnd && IsWindow(req->hwnd))
        DisableTransitions(req->hwnd, FALSE);
    if (req->type == AnimationType::Minimize && req->hwnd &&
        IsWindow(req->hwnd) && IsIconic(req->hwnd)) {
        // CacheCapture keeps this one at full resolution (most recent entry)
        // and demotes older entries to the compact format itself.
        CacheCapture(req->hwnd, std::move(req->capture));
    }
    // Only clear g_hwndCurrent if it still points at this request (a newer
    // animation can't have started in the meantime -- g_fAnimating gates
    // that -- but a cancellation, e.g. from AfterOrigMinimize, can already
    // have cleared it to nullptr). g_typeCurrent/g_fAnimating belong to this
    // request either way and must be cleared unconditionally, or a
    // cancelled request would leave g_fAnimating stuck true forever.
    HWND cur = g_hwndCurrent.load();
    if (cur == req->hwnd)
        g_hwndCurrent.store(nullptr);
    g_typeCurrent.store(0);
    g_fAnimating.store(false);
    delete req;
}
static void DrainQueue(HWND hwndOverlay) {
    for (;;) {
        if (g_stopping.load())
            break;
        AnimRequest* req = nullptr;
        {
            std::lock_guard<std::mutex> lock(g_queueMutex);
            if (g_queue.empty())
                break;
            req = g_queue.front();
            g_queue.pop_front();
        }
        if (!req)
            continue;
        RunAnimation(hwndOverlay, *req);
        FinishQueued(req);
    }
}
static const wchar_t kAnimClassName[] = L"Windhawk_Win7AeroAnim";
static LRESULT CALLBACK AnimWndProc(HWND hwnd, UINT uMsg, WPARAM wParam,
                                    LPARAM lParam) {
    UINT msg = g_msgAnim.load();
    if (msg && uMsg == msg) {
        switch (AnimMsg(wParam)) {
        case AnimMsg::FirstFrame:
        {
            AnimRequest* req;
            {
                std::lock_guard<std::mutex> lock(g_firstFrameMutex);
                req = g_firstFrameReq;
            }
            if (req && PresentFirstFrame(hwnd, *req))
                return 1;
        }
        break;
            case AnimMsg::Drain:
                DrainQueue(hwnd);
                break;
            case AnimMsg::Hide:
                HideOverlayWindow(hwnd);
                break;
            case AnimMsg::GhostArm:
                // GhostArm's lParam is the same untrusted-message-payload
                // shape as FirstFrame's was: any process can send this
                // message. The only legitimate caller always arms for
                // kShellHideToCloseMs, so ignore the payload and use that
                // constant directly instead of trusting an attacker-supplied
                // timer elapse value.
                SetTimer(hwnd, kGhostTimer, UINT(kShellHideToCloseMs),
                         nullptr);
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
        case WM_TIMER:
            if (wParam == WPARAM(kGhostTimer)) {
                KillTimer(hwnd, kGhostTimer);
                GhostWatchdog();
            }
            return 0;
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
    if (!RegisterClassW(&wc)) {
        SetEvent(hEvent);
        return 0;
    }
    HWND hwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kAnimClassName,
        nullptr, WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, g_hinst, nullptr);
    if (!hwnd) {
        UnregisterClassW(kAnimClassName, g_hinst);
        SetEvent(hEvent);
        return 0;
    }
    LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    SetWindowLongPtrW(hwnd, GWL_EXSTYLE,
                      ex | WS_EX_LAYERED | WS_EX_TRANSPARENT);
    {
        BOOL dis = TRUE;
        DwmSetWindowAttribute(hwnd, DWMWA_TRANSITIONS_FORCEDISABLED, &dis,
                              sizeof(dis));
    }
    g_hwndAnim.store(hwnd);
    g_msgAnim.store(RegisterWindowMessageW(L"Windhawk_Win7AeroAnim_Run"));
    SetEvent(hEvent);
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    g_hwndAnim.store(nullptr);
    UnregisterClassW(kAnimClassName, g_hinst);
    return 0;
}
// Makes sure the overlay thread is up, (re)starting it if needed. Failures
// are reported through NoteOverlayThreadFailure, which pauses the mod for a
// backoff period instead of latching it off for the life of the process; the
// next candidate animation after the backoff simply lands here again and
// retries. Callers check IsDisabled() first so the backoff costs nothing.
static bool WaitForAnimWndThread() {
    if (IsDisabled())
        return false;
    std::lock_guard<std::mutex> lock(g_animThreadMutex);
    if (IsDisabled())
        return false;
    if (g_hAnimWndThread) {
        HWND ha = g_hwndAnim.load();
        if (ha && IsWindow(ha)) {
            ClearOverlayThreadFailure(); // e.g. a slow start that made it after all
            return true;
        }
        if (g_dwAnimThreadId)
            PostThreadMessageW(g_dwAnimThreadId, WM_QUIT, 0, 0);
        // the handle is kept so StopAnimThread() can still join it at uninit
        if (WaitForSingleObject(g_hAnimWndThread, 2000) != WAIT_OBJECT_0) {
            NoteOverlayThreadFailure(L"previous overlay thread did not exit");
            return false;
        }
        CloseHandle(g_hAnimWndThread);
        g_hAnimWndThread = nullptr;
        g_dwAnimThreadId = 0;
    }
    ScopedHandle hEvent(CreateEventW(nullptr, TRUE, FALSE, nullptr));
    if (!hEvent) {
        NoteOverlayThreadFailure(L"CreateEvent failed");
        return false;
    }
    g_hAnimWndThread =
        CreateThread(nullptr, 0, AnimWndThreadProc, hEvent.get(), 0, nullptr);
    if (!g_hAnimWndThread) {
        NoteOverlayThreadFailure(L"CreateThread failed");
        return false;
    }
    g_dwAnimThreadId = GetThreadId(g_hAnimWndThread);
    if (WaitForSingleObject(hEvent.get(), 1000) != WAIT_OBJECT_0) {
        // The thread may still be starting and will SetEvent on this handle
        // later, so it is leaked on purpose rather than closed under it. If
        // it does come up late, the next attempt finds g_hwndAnim valid and
        // succeeds without recreating anything.
        hEvent.release();
        NoteOverlayThreadFailure(L"overlay thread start timed out");
        return false;
    }
    if (!g_hwndAnim.load()) {
        NoteOverlayThreadFailure(L"overlay window could not be created");
        return false;
    }
    ClearOverlayThreadFailure();
    return true;
}
static bool QueueRun(AnimRequest&& req) {
    AnimRequest* heap = nullptr;
    try {
        heap = new AnimRequest(std::move(req));
    } catch (...) {
        return false;
    }
    {
        std::lock_guard<std::mutex> lock(g_queueMutex);
        g_queue.push_back(heap);
    }
    HWND ha = g_hwndAnim.load();
    UINT msg = g_msgAnim.load();
    if (!ha || !msg || !IsWindow(ha) ||
        !PostMessageW(ha, msg, WPARAM(AnimMsg::Drain), 0)) {
        // Erase by value: another thread may have pushed after us, so this request is
        // not necessarily at the back and must never be left dangling in the queue.
        {
            std::lock_guard<std::mutex> lock(g_queueMutex);
            auto it = std::find(g_queue.begin(), g_queue.end(), heap);
            if (it != g_queue.end())
                g_queue.erase(it);
        }
        delete heap;
        return false;
    }
    return true;
}
static bool IsAnimateCandidate(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd))
        return false;
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId()) {
        Wh_Log(L"skipped: window of another process");
        return false;
    }
    // Top-level windows only. This deliberately excludes MDI child windows
    // (WS_CHILD | WS_CAPTION inside an MDI client): they minimize to an icon
    // strip at the bottom of the MDI client, not to the taskbar, so flying
    // them towards a taskbar button -- or trying to capture/close-animate a
    // child that is clipped by its parent -- is simply wrong. The
    // DefMDIChildProc hooks are kept for the WM_NCDESTROY cache cleanup they
    // share with the other Def*Proc hooks; their SC_MINIMIZE/SC_RESTORE
    // handling ends up here and is skipped. Animating MDI children inside
    // the client area (as Windows 7 did) is out of scope for now.
    LONG style = LONG(GetWindowLongPtrW(hwnd, GWL_STYLE)),
         ex = LONG(GetWindowLongPtrW(hwnd, GWL_EXSTYLE));
    if (style & WS_CHILD)
        return false;
    if (GetAncestor(hwnd, GA_ROOT) != hwnd)
        return false;
    if (!(style & WS_CAPTION))
        return false;
    if (ex & WS_EX_TOOLWINDOW)
        return false;
    if (ex & WS_EX_NOACTIVATE)
        return false;
    if (hwnd == g_hwndAnim.load())
        return false;
    // Same-thread requirement, matching IsTopLevelCloseCandidate: without it,
    // PlayMinimize/PlayRestore can be hooked for a window owned by another
    // thread in this process, and the synchronous capture (which can fall
    // back to an untimed PrintWindow SendMessage) plus the SendMessage to the
    // overlay thread then block the calling thread on that foreign thread.
    DWORD tid = GetWindowThreadProcessId(hwnd, nullptr);
    if (tid != GetCurrentThreadId())
        return false;
    return true;
}
static bool IsTopLevelCloseCandidate(HWND hwnd) {
    if (!IsAnimateCandidate(hwnd))
        return false;
    LONG style = LONG(GetWindowLongPtrW(hwnd, GWL_STYLE));
    if (style & WS_CHILD)
        return false;
    if (!(style & WS_SYSMENU))
        return false;
    if (!(style & WS_MINIMIZEBOX))
        return false;
    if (GetWindow(hwnd, GW_OWNER))
        return false;
    if (GetParent(hwnd))
        return false;
    if (GetAncestor(hwnd, GA_ROOT) != hwnd)
        return false;
    DWORD tid = GetWindowThreadProcessId(hwnd, nullptr);
    if (tid != GetCurrentThreadId())
        return false;
    return true;
}
static bool ShouldAnimateWindow(HWND hwnd) {
    if (IsDisabled() || g_fAnimating.load())
        return false;
    if (!IsAnimateCandidate(hwnd))
        return false;
    if (!IsWindowVisible(hwnd) || IsIconic(hwnd))
        return false;
    return true;
}
static bool ShouldAnimateClose(HWND hwnd) {
    if (IsDisabled() || g_fAnimating.load())
        return false;
    if (!IsTopLevelCloseCandidate(hwnd))
        return false;
    if (!IsWindowVisible(hwnd) || IsIconic(hwnd))
        return false;
    return true;
}
static bool StartQueuedAnimation(HWND hwnd, AnimationType type,
                                 const RECT& rcWin, const RECT& rcDest,
                                 CaptureBits&& cap) {
    auto cleanup = [hwnd]() {
        if (g_hwndCurrent.load() == hwnd) {
            g_hwndCurrent.store(nullptr);
            g_typeCurrent.store(0);
        }
        g_fAnimating.store(false);
    };
    if (!hwnd || !IsWindow(hwnd) || cap.empty() || !WaitForAnimWndThread()) {
        cleanup();
        return false;
    }
    if (g_stopping.load()) {
        cleanup();
        return false;
    }
    AnimRequest req;
    req.hwnd = hwnd;
    req.type = type;
    req.rcWindow = rcWin;
    req.rcDest = rcDest;
    req.capture = std::move(cap);
    req.durationMs = DurationMsFor(type);
    try {
        req.gdi = std::make_unique<PresentGdi>();
    } catch (...) {
        cleanup();
        return false;
    }
    g_hwndCurrent.store(hwnd);
    g_typeCurrent.store(int(type));
    SendFirstFrame(g_hwndAnim.load(), g_msgAnim.load(), &req);
    // SendFirstFrame above returns once the overlay's first frame is drawn,
    // not once DWM has actually composited and presented it. Without this
    // flush, DisableTransitions() below can land on-screen a frame before
    // the overlay does, showing the real window with its glass/transition
    // just switched off for a beat -- the "aero flash" during minimize.
    DwmFlush();
    DisableTransitions(hwnd, TRUE);
    // Cloak here, after g_hwndCurrent is published but before QueueRun makes
    // the request visible to the overlay thread: CloakForRestoreAnim's own
    // check (g_hwndCurrent.load() == hwnd) is guaranteed to still hold at
    // this point, and the overlay thread cannot have run FinishQueued's
    // uncloak for this request yet, because it hasn't seen the request. Doing
    // this from the callers instead (after PlayRestore returns) raced
    // against the overlay thread finishing first, which could leave the
    // window cloaked forever with no user-visible recovery.
    if (type == AnimationType::RestoreFromMinimized)
        CloakForRestoreAnim(hwnd);
    Wh_Log(L"animation queued: type=%d %dx%d", int(type), RECTW(rcWin),
           RECTH(rcWin));
    if (!QueueRun(std::move(req))) {
        DisableTransitions(hwnd, FALSE);
        if (type == AnimationType::RestoreFromMinimized) {
            HWND c = g_cloakedHwnd.exchange(nullptr);
            if (c == hwnd)
                UncloakAfterRestore(hwnd);
        }
        HWND ha = g_hwndAnim.load();
        if (ha && IsWindow(ha))
            HideOverlayWindow(ha);
        cleanup();
        return false;
    }
    return true;
}
static bool BeginAnimation(HWND hwnd, AnimationType type, const RECT& rcWin,
                           const RECT& rcDest, CaptureBits&& cap) {
    bool exp = false;
    if (!g_fAnimating.compare_exchange_strong(exp, true))
        return false;
    return StartQueuedAnimation(hwnd, type, rcWin, rcDest, std::move(cap));
}
static void StopAnimThread() {
    std::lock_guard<std::mutex> lock(g_animThreadMutex);
    if (!g_hAnimWndThread)
        return;
    HWND ha = g_hwndAnim.load();
    if (ha && IsWindow(ha))
        SendMessageW(ha, WM_CLOSE, 0, 0);
    if (g_dwAnimThreadId)
        PostThreadMessageW(g_dwAnimThreadId, WM_QUIT, 0, 0);
    WaitForSingleObject(g_hAnimWndThread, INFINITE);
    CloseHandle(g_hAnimWndThread);
    g_hAnimWndThread = nullptr;
    g_dwAnimThreadId = 0;
}
static void AfterOrigMinimize(HWND hwnd, bool async) {
    if (!hwnd || !IsWindow(hwnd))
        return;
    if (!g_fAnimating.load() || g_hwndCurrent.load() != hwnd)
        return;
    if (g_typeCurrent.load() != int(AnimationType::Minimize))
        return;
    DisableTransitions(hwnd, TRUE);
    if (!async && !IsIconic(hwnd)) {
        // The overlay thread does not pump messages while inside RunAnimation
        // (it sits in DwmFlush() for the whole timeline), so a direct,
        // cross-thread HideOverlayWindow() from here -- the app's UI thread,
        // inside its own ShowWindow/DefWindowProc call -- would block until
        // the very animation it's trying to cancel finishes: the opposite of
        // the intent. Instead, just clear g_hwndCurrent: RunAnimation polls
        // it every frame and bails as soon as it no longer matches, and
        // FinishQueued (on the overlay thread) does the hide and the rest of
        // the state reset from there.
        HWND cur = g_hwndCurrent.load();
        if (cur == hwnd)
            g_hwndCurrent.store(nullptr);
    }
}
static bool PlayMinimize(HWND hwnd) {
    if (!g_animateMinimize || !ShouldAnimateWindow(hwnd))
        return false;
    LONG s = LONG(GetWindowLongPtrW(hwnd, GWL_STYLE));
    if (s & WS_MINIMIZE)
        return false;
    RECT rcWin{}, rcMin{};
    {
        ScopedDpiAware dpi;
        if (!GetVisibleWindowRectForMinimize(hwnd, &rcWin)) {
            Wh_Log(L"minimize: no usable window rect");
            return false;
        }
        if (!GetMinimizeRectPhysical(hwnd, &rcMin)) {
            Wh_Log(L"minimize: no taskbar target");
            return false;
        }
        rcMin = AspectCorrectedMinimizeTarget(rcMin, rcWin);
    }
    CaptureBits
        cap; // CaptureWindowForClose scopes its own DPI override; it may fall back to PrintWindow, which is app code
    if (!CaptureWindowForClose(hwnd, cap, false)) {
        Wh_Log(L"minimize: capture failed");
        return false;
    }
    return BeginAnimation(hwnd, AnimationType::Minimize, rcWin, rcMin,
                          std::move(cap));
}
static bool PlayRestore(HWND hwnd) {
    if (!g_animateMinimize)
        return false;
    LONG s = LONG(GetWindowLongPtrW(hwnd, GWL_STYLE));
    if (s & WS_MINIMIZE) {
        if (IsDisabled() || g_fAnimating.load())
            return false;
        if (!HasCachedCapture(hwnd))
            return false;
        RECT rcMin{}, rcRest{};
        bool have = false, restoreToMax = false;
        {
            ScopedDpiAware dpi;
            if (!GetMinimizeRectPhysical(hwnd, &rcMin))
                return false;
            WINDOWPLACEMENT wp{};
            wp.length = sizeof(wp);
            if (GetWindowPlacement(hwnd, &wp))
                restoreToMax = (wp.flags & WPF_RESTORETOMAXIMIZED) != 0;
            if (!restoreToMax)
                have = GetRestoreRectPhysical(hwnd, &rcRest);
            else
                // GetMaximizeRectPhysical no longer probes WM_GETMINMAXINFO
                // (app code, app coordinate space), so it's plain
                // GetMonitorInfoW and stays inside this same DPI override --
                // the same physical space as rcMin and the capture.
                have = GetMaximizeRectPhysical(hwnd, &rcRest);
        }
        if (!have || !IsRectUsable(rcRest))
            return false;
        CaptureBits cap;
        if (!TakeCachedCapture(hwnd, RECTW(rcRest), RECTH(rcRest), cap))
            return false;
        if (cap.width > 0 && cap.height > 0 &&
            (RECTW(rcRest) != cap.LogicalW() ||
             RECTH(rcRest) != cap.LogicalH())) {
            rcRest.right = rcRest.left + cap.LogicalW();
            rcRest.bottom = rcRest.top + cap.LogicalH();
        }
        // Fit the (final) window rect's aspect ratio into the button, so the
        // fly-in starts from the same shape the fly-out ended on.
        rcMin = AspectCorrectedMinimizeTarget(rcMin, rcRest);
        // The caller falls through to the original ShowWindow right after this,
        // so the restore happens synchronously and the overlay animates on top --
        // exactly like the minimize path. Nothing is deferred any more.
        return BeginAnimation(hwnd, AnimationType::RestoreFromMinimized, rcRest,
                              rcMin, std::move(cap));
    }
    return false;
}

// The close effect must finish before the real destroy is allowed to proceed,
// because the overlay lives in the SAME process as the window: with a fully
// asynchronous handoff, DestroyWindow returns at once and an app whose last
// window just closed typically posts WM_QUIT and exits within milliseconds --
// taking the overlay thread (and the animation) down with it. So the closing
// thread has to wait for the effect here, inside the app's own DestroyWindow
// call, but it no longer RUNS the effect:
//   - the timeline is queued to the overlay thread exactly like minimize and
//     restore (RunAnimation, with its frame pacing; it ends `duration` ms
//     after it starts by construction), so the frames are rasterized and
//     presented off this thread;
//   - this thread only parks in MsgWaitForMultipleObjectsEx on the request's
//     completion event, waking for SENT messages (SendMessage from other
//     threads/processes, DWM, IsHungAppWindow) so it never looks hung.
//     Posted messages and input are deliberately left queued and NOT
//     dispatched, because this runs inside DestroyWindow (from a
//     WM_CLOSE/WM_COMMAND handler, a destructor, possibly under the app's
//     own locks) -- a point the app never expects to be re-entered from;
//   - because nothing is removed from the queue, a WM_QUIT already pending
//     (e.g. PostQuitMessage() called right before DestroyWindow()) is left
//     completely untouched and is seen by the app's own loop the moment the
//     effect ends, with its normal semantics -- no special-casing needed;
//   - the wait has a hard deadline (duration + slack): whatever the overlay
//     thread does, the app gets its thread back on time. The request's
//     destructor signals the event on every path (normal finish,
//     cancellation, a drained queue at unload), so the deadline is only the
//     backstop, not the usual exit;
//   - the DPI override stays scoped to the geometry/capture and is NEVER held
//     while application code runs (the hide below runs under the thread's
//     own, unmodified awareness context).
// Slack on top of the nominal duration before the app thread stops waiting:
// covers the final-frame present, FinishQueued and scheduling jitter. Past
// this the app's thread is released regardless; the overlay thread finishes
// (or abandons) the timeline on its own.
constexpr DWORD kCloseWaitSlackMs = 500;
static void WaitForCloseAnimation(HANDLE hDone, DWORD durationMs) {
    const ULONGLONG deadline =
        GetTickCount64() + durationMs + kCloseWaitSlackMs;
    for (;;) {
        if (g_stopping.load())
            break;
        ULONGLONG now = GetTickCount64();
        if (now >= deadline)
            break;
        DWORD r = MsgWaitForMultipleObjectsEx(1, &hDone, DWORD(deadline - now),
                                              QS_SENDMESSAGE,
                                              MWMO_INPUTAVAILABLE);
        if (r != WAIT_OBJECT_0 + 1)
            break; // signalled, timed out or failed: all mean "stop waiting"
        // PM_NOREMOVE with PM_QS_SENDMESSAGE dispatches pending sent
        // messages without touching posted messages or input.
        MSG msg;
        PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE | PM_QS_SENDMESSAGE);
    }
}
static bool PlayCloseAnimation(HWND hwnd, CaptureBits&& preCap,
                               const RECT& preRect) {
    bool exp = false;
    if (!g_fAnimating.compare_exchange_strong(exp, true))
        return false;
    // Released on every early exit below; dismissed once the request has
    // been handed to the overlay thread (FinishQueued releases it then).
    ScopedExit releaseSlot([]() { g_fAnimating.store(false); });
    if (!WaitForAnimWndThread() || g_stopping.load())
        return false;
    ScopedDwmTransitions transWnd(hwnd);
    transWnd.Disable();
    RECT rcWin = preRect;
    CaptureBits cap = std::move(preCap);
    if (cap.empty()) {
        {
            ScopedDpiAware dpi;
            if (!GetFrameBoundsPhysical(hwnd, &rcWin)) {
                Wh_Log(L"close: capture failed");
                return false;
            }
        }
        // CaptureWindowForClose scopes its own DPI override; it may fall back to
        // PrintWindow, which is app code and must not run under this one.
        if (!CaptureWindowForClose(hwnd, cap, false)) {
            Wh_Log(L"close: capture failed");
            return false;
        }
    }
    if (cap.empty() || !IsRectUsable(rcWin))
        return false;
    ScopedHandle hDone(CreateEventW(nullptr, TRUE, FALSE, nullptr));
    if (!hDone)
        return false;
    HANDLE hDoneForReq = nullptr;
    if (!DuplicateHandle(GetCurrentProcess(), hDone.get(), GetCurrentProcess(),
                         &hDoneForReq, 0, FALSE, DUPLICATE_SAME_ACCESS))
        return false;
    AnimRequest req;
    req.hwnd = hwnd;
    req.type = AnimationType::Close;
    req.rcWindow = rcWin;
    req.rcDest = rcWin;
    req.capture = std::move(cap);
    req.durationMs = DurationMsFor(AnimationType::Close);
    req.hDone = hDoneForReq; // owned (and signalled) by the request from here
    try {
        req.gdi = std::make_unique<PresentGdi>();
    } catch (...) {
        return false;
    }
    HWND ha = g_hwndAnim.load();
    UINT msg = g_msgAnim.load();
    if (!ha || !msg || !IsWindow(ha))
        return false;
    g_hwndCurrent.store(hwnd);
    g_typeCurrent.store(int(AnimationType::Close));
    ScopedExit clearCurrent([hwnd]() {
        if (g_hwndCurrent.load() == hwnd) {
            g_hwndCurrent.store(nullptr);
            g_typeCurrent.store(0);
        }
    });
    if (SendFirstFrame(ha, msg, &req) != 1) {
        // Nothing is on screen yet, so a silent fallback to a plain close is
        // the right outcome: the window stays visible and DestroyWindow runs.
        if (IsWindow(ha))
            HideOverlayWindow(ha);
        return false;
    }
    // The hide dispatches WM_SHOWWINDOW & co. inside the app: no DPI override
    // is held here, so the app runs under its own awareness context. Hiding
    // ahead of DestroyWindow_orig is what puts the real window out of sight
    // under the overlay's first frame; DestroyWindow hides the window itself
    // as its very first step anyway (before WM_DESTROY), so the only change
    // in the app-visible sequence is that the hide -- and the activation
    // hand-off to the next window -- happens a couple of hundred
    // milliseconds earlier than the WM_DESTROY that follows.
    if (IsWindowVisible(hwnd)) {
        if (ShowWindow_orig)
            ShowWindow_orig(hwnd, SW_HIDE);
        else
            ::ShowWindow(hwnd, SW_HIDE);
    }
    const DWORD durationMs = req.durationMs;
    // From here the overlay thread owns the request, the animation slot and
    // g_hwndCurrent: FinishQueued clears all three and, by deleting the
    // request, signals hDone. If QueueRun fails it deletes the request
    // itself (which also signals the event) and the slot stays ours -- but
    // the window is already hidden, so report "animated" anyway: that is
    // what makes DestroyWindow_hook re-show it should the destroy then be
    // refused. There is simply no animation to wait for in that case.
    if (!QueueRun(std::move(req))) {
        Wh_Log(L"close: could not queue the animation");
        if (IsWindow(ha))
            HideOverlayWindow(ha);
        transWnd.Dismiss();
        return true;
    }
    releaseSlot.Dismiss();
    clearCurrent.Dismiss();
    WaitForCloseAnimation(hDone.get(), durationMs);
    // DWMWA_TRANSITIONS_FORCEDISABLED is left set on purpose: the window is
    // about to be destroyed and must not get the stock DWM close transition
    // on top of ours (FinishQueued skips the re-enable for Close requests for
    // the same reason; DestroyWindow_hook re-enables it if the destroy is
    // refused and the window comes back).
    transWnd.Dismiss();
    return true;
}
// The close animation only decorates a close the app already committed to: it runs
// from DestroyWindow, so WM_CLOSE / SC_CLOSE keep their own semantics.
static constexpr wchar_t kClosingProp[] =
    L"win7-window-animations-restorer.Closing";
static bool CloseIsProgressSafe(
    HWND hwnd) { // never delay a close that could cost work
    if (!hwnd || !IsWindow(hwnd))
        return false;
    if (GetWindowThreadProcessId(hwnd, nullptr) != GetCurrentThreadId())
        return false;
    if (GetSystemMetrics(SM_SHUTTINGDOWN))
        return false;
    if (pIsHungAppWindow && pIsHungAppWindow(hwnd))
        return false;
    if (!IsWindowEnabled(hwnd))
        return false;
    HWND popup = GetLastActivePopup(hwnd);
    if (popup && popup != hwnd && IsWindow(popup) && IsWindowVisible(popup))
        return false;
    return true;
}
struct PendingClose {
    RECT rc{};
    CaptureBits cap;
    ULONGLONG tick = 0;
    bool ghost = false;
};
static std::mutex g_pendingCloseMutex;
static PendingClose g_pendingClose;
static std::atomic<HWND> g_pendingCloseHwnd{nullptr};
static bool IsShellBrowserWindow(HWND hwnd) {
    wchar_t cls[32] = {};
    if (!GetClassNameW(hwnd, cls, _countof(cls)))
        return false;
    return _wcsicmp(cls, L"CabinetWClass") == 0 ||
           _wcsicmp(cls, L"ExploreWClass") == 0;
}
static void HideGhostOverlay() {
    HWND ha = g_hwndAnim.load();
    if (ha && IsWindow(ha) && !g_fAnimating.load())
        HideOverlayWindow(ha);
}
static void ForgetShellPreCapture(HWND hwnd) {
    if (g_pendingCloseHwnd.load() != hwnd)
        return;
    bool ghost = false;
    try {
        std::lock_guard<std::mutex> lock(g_pendingCloseMutex);
        ghost = g_pendingClose.ghost;
        g_pendingClose = PendingClose{};
        g_pendingCloseHwnd.store(nullptr);
    } catch (...) {
        return;
    }
    if (ghost)
        HideGhostOverlay();
}
static void GhostWatchdog() {
    HWND hwnd = g_pendingCloseHwnd.load();
    if (!hwnd)
        return;
    bool expired = false, ghost = false;
    try {
        std::lock_guard<std::mutex> lock(g_pendingCloseMutex);
        expired = GetTickCount64() - g_pendingClose.tick >= kShellHideToCloseMs;
        ghost = g_pendingClose.ghost;
        if (expired) {
            g_pendingClose = PendingClose{};
            g_pendingCloseHwnd.store(nullptr);
        }
    } catch (...) {
        return;
    }
    if (expired && ghost)
        HideGhostOverlay();
}
// Strict bounds for the shell path: only the real composed frame is accepted. The
// GetWindowRect fallback would add the invisible resize margins, and the window then
// animates inset inside a band of desktop pixels, which is what ate the borders.
static bool GetShellFrameBounds(HWND hwnd, RECT* rc) {
    if (!hwnd || !rc)
        return false;
    RECT ext{}, win{};
    if (FAILED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &ext,
                                     sizeof(ext))))
        return false;
    if (!IsRectUsable(ext) || RECTW(ext) < 32 || RECTH(ext) < 32)
        return false;
    if (RECTW(ext) > kMaxCaptureSide || RECTH(ext) > kMaxCaptureSide)
        return false;
    if (!GetWindowRectPhysical(hwnd, &win) || !IsRectUsable(win))
        return false;
    if (ext.left < win.left - 2 || ext.top < win.top - 2 ||
        ext.right > win.right + 2 || ext.bottom > win.bottom + 2)
        return false;
    const double area = double(RECTW(ext)) * double(RECTH(ext)),
                 full = double(RECTW(win)) * double(RECTH(win));
    if (full <= 0.0 || area < full * 0.5)
        return false;
    *rc = ext;
    return true;
}
static bool CaptureLooksComposed(const CaptureBits& cap) {
    if (cap.empty() || cap.width < 32 || cap.height < 32)
        return false;
    const int w = cap.width, h = cap.height;
    const uint32_t* p = cap.pixels.data();
    const int stepX = std::max(1, w / 64), stepY = std::max(1, h / 64);
    size_t samples = 0, lit = 0;
    for (int x = 0; x < w; x += stepX) {
        samples += 2;
        if (p[x] & 0x00FFFFFFu)
            ++lit;
        if (p[size_t(h - 1) * size_t(w) + size_t(x)] & 0x00FFFFFFu)
            ++lit;
    }
    for (int y = 0; y < h; y += stepY) {
        const uint32_t* row = p + size_t(y) * size_t(w);
        samples += 2;
        if (row[0] & 0x00FFFFFFu)
            ++lit;
        if (row[w - 1] & 0x00FFFFFFu)
            ++lit;
    }
    return samples > 0 && lit * 16 >= samples;
}
static void ShellPreCaptureForClose(HWND hwnd) {
    try {
        if (!g_animateClose || IsDisabled() || g_fAnimating.load() ||
            g_stopping.load())
            return;
        if (IsSnippingTool() || !IsShellBrowserWindow(hwnd))
            return;
        if (g_pendingCloseHwnd.load() == hwnd) {
            bool fresh = false;
            try {
                std::lock_guard<std::mutex> lock(g_pendingCloseMutex);
                fresh = (GetTickCount64() - g_pendingClose.tick) < 100;
            } catch (...) {
            }
            if (fresh)
                return;
        }
        if (!IsWindowVisible(hwnd) || IsIconic(hwnd))
            return;
        if (!IsTopLevelCloseCandidate(hwnd) || !CloseIsProgressSafe(hwnd))
            return;
        RECT rc{};
        CaptureBits cap;
        {
            ScopedDpiAware dpi;
            if (!GetShellFrameBounds(hwnd, &rc))
                return;
            if (!CaptureWindowForClose(hwnd, cap, true))
                return;
        }
        if (cap.empty() || !IsRectUsable(rc))
            return;
        if (cap.width != RECTW(rc) || cap.height != RECTH(rc))
            return;
        if (!CaptureLooksComposed(cap)) {
            Wh_Log(L"shell: capture does not look composed, skipping");
            return;
        }
        // The overlay puts the very same pixels on screen before the window disappears,
        // so the hide-then-destroy sequence never shows a hole where the window was.
        bool ghost = false;
        {
            // Own the animation slot for the capture-and-present, like every
            // other SendFirstFrame caller. A plain !g_fAnimating check was a
            // TOCTOU: an animation started in between would (a) overwrite
            // g_firstFrameReq under us and (b) leave this Explorer thread
            // blocked in SendMessageW behind the running timeline. If the
            // slot is busy the ghost is simply skipped -- the close still
            // animates from the pre-capture, just without the bridge frame.
            bool exp = false;
            bool haveSlot = g_fAnimating.compare_exchange_strong(exp, true);
            ScopedExit releaseSlot([haveSlot]() {
                if (haveSlot)
                    g_fAnimating.store(false);
            });
            if (haveSlot && WaitForAnimWndThread() && !g_stopping.load()) {
                HWND ha = g_hwndAnim.load();
                UINT msg = g_msgAnim.load();
                std::unique_ptr<PresentGdi> gdi;
                try {
                    gdi = std::make_unique<PresentGdi>();
                } catch (...) {
                    gdi.reset();
                }
                if (ha && msg && IsWindow(ha) && gdi) {
                    AnimRequest frame;
                    frame.hwnd = hwnd;
                    frame.type = AnimationType::Close;
                    frame.rcWindow = rc;
                    frame.rcDest = rc;
                    frame.capture = std::move(cap);
                    frame.gdi = std::move(gdi);
                    // The ghost frame is presented by the overlay thread
                    // (already per-monitor DPI-aware), so the DPI override on
                    // this thread stays scoped to the capture above and is
                    // never held across presents. SendFirstFrame is
                    // synchronous: once it returns the overlay thread is back
                    // in its message loop and holds no reference to `frame`,
                    // so the slot can be released right after (the ghost
                    // pixels themselves live in the overlay's own surface).
                    ghost = SendFirstFrame(ha, msg, &frame) == 1;
                    if (!ghost)
                        HideOverlayWindow(
                            ha); // 1x1 transparent + SWP_HIDEWINDOW: coordinate-free
                    cap = std::move(frame.capture);
                }
            }
        }
        ScopedExit hideGhost([ghost]() {
            if (ghost)
                HideGhostOverlay();
        });
        if (cap.empty())
            return;
        try {
            std::lock_guard<std::mutex> lock(g_pendingCloseMutex);
            g_pendingClose.rc = rc;
            g_pendingClose.cap = std::move(cap);
            g_pendingClose.tick = GetTickCount64();
            g_pendingClose.ghost = ghost;
            g_pendingCloseHwnd.store(hwnd);
        } catch (...) {
            return;
        }
        hideGhost.Dismiss();
        if (ghost) {
            HWND ha = g_hwndAnim.load();
            UINT msg = g_msgAnim.load();
            if (ha && msg && IsWindow(ha))
                PostMessageW(ha, msg, WPARAM(AnimMsg::GhostArm),
                             LPARAM(kShellHideToCloseMs));
        }
    } catch (...) {
        try {
            ForgetShellPreCapture(hwnd);
        } catch (...) {
        }
    }
}
// ghost reports whether a shell "ghost" overlay is currently on screen for
// this capture. On every return, either this function has already hidden it
// (the two failure branches below), or the caller now owns it via `ghost`
// and is responsible for hiding it if it doesn't go on to actually play the
// close animation with this capture -- see the ScopedExit in PlayClose.
static bool TakeShellPreCapture(HWND hwnd, CaptureBits& cap, RECT& rc,
                                bool& ghost) {
    ghost = false;
    try {
        if (g_pendingCloseHwnd.load() != hwnd)
            return false;
        ULONGLONG tick = 0;
        try {
            std::lock_guard<std::mutex> lock(g_pendingCloseMutex);
            cap = std::move(g_pendingClose.cap);
            rc = g_pendingClose.rc;
            tick = g_pendingClose.tick;
            ghost = g_pendingClose.ghost;
            g_pendingClose = PendingClose{};
            g_pendingCloseHwnd.store(nullptr);
        } catch (...) {
            return false;
        }
        if (cap.empty() || !IsRectUsable(rc) ||
            GetTickCount64() - tick > kShellHideToCloseMs) {
            cap = {};
            if (ghost) {
                HideGhostOverlay();
                ghost = false;
            }
            return false;
        }
        if (!CaptureLooksComposed(cap)) {
            cap = {};
            if (ghost) {
                HideGhostOverlay();
                ghost = false;
            }
            return false;
        }
        return true;
    } catch (...) {
        cap = {};
        if (ghost) {
            HideGhostOverlay();
            ghost = false;
        }
        return false;
    }
}
static bool PlayClose(HWND hwnd) {
    if (!g_animateClose)
        return false;
    if (IsSnippingTool())
        return false;
    if (!CloseIsProgressSafe(hwnd)) {
        Wh_Log(L"close: skipped, animating could cost the user work");
        return false;
    }
    CaptureBits pre;
    RECT preRc{};
    bool usePre = false, ghost = false;
    if (!ShouldAnimateClose(hwnd)) {
        if (IsDisabled() || g_fAnimating.load())
            return false;
        if (IsWindowVisible(hwnd) || IsIconic(hwnd))
            return false;
        if (!IsTopLevelCloseCandidate(hwnd))
            return false;
        usePre = TakeShellPreCapture(hwnd, pre, preRc, ghost);
        if (!usePre) {
            Wh_Log(L"close: hidden window without a fresh shell capture");
            return false;
        }
    } else
        ForgetShellPreCapture(hwnd);
    // If TakeShellPreCapture handed us an armed ghost overlay and we don't
    // end up actually playing the animation with it (any early return below,
    // or PlayCloseAnimation itself bailing out), nothing else will ever hide
    // it -- the pending-close bookkeeping that the watchdog relies on was
    // already cleared by TakeShellPreCapture's success path. Tie it to this
    // scope and only release it once PlayCloseAnimation has taken over.
    ScopedExit hideGhost([ghost]() {
        if (ghost)
            HideGhostOverlay();
    });
    // PlayCloseAnimation takes the animation slot with a CAS, hands the
    // request (and the slot) to the overlay thread and waits for the fly-out
    // to complete before the real destroy is allowed through.
    if (PlayCloseAnimation(hwnd, std::move(pre), preRc)) {
        hideGhost.Dismiss();
        return true;
    }
    return false;
}

#define DWP_HOOK_(name, defArgs, callArgs)                                     \
    LRESULT(CALLBACK* name##_orig) defArgs;                                    \
    LRESULT CALLBACK name##_hook defArgs {                                     \
        /* This runs on every message to every window in every process --   */ \
        /* nothing our bookkeeping does may ever throw back into the host's */ \
        /* message loop. calledOrig tracks whether name##_orig already ran  */ \
        /* inside the try, so the catch handler below never calls it twice. */ \
        bool calledOrig = false;                                               \
        LRESULT lr = 0;                                                        \
        try {                                                                  \
            if (uMsg == WM_NCDESTROY)                                          \
                /* Catches windows torn down as a side effect of an ancestor's */ \
                /* destruction, which never go through the DestroyWindow hook. */ \
                /* Without this, a stale cache entry can survive under a */    \
                /* recycled HWND value until evicted by the 2-entry cap. */    \
                ForgetCapture(hWnd);                                           \
            if (uMsg == WM_SHOWWINDOW && wParam == FALSE && lParam == 0)       \
                ShellPreCaptureForClose(hWnd);                                 \
            if (uMsg == WM_SYSCOMMAND) {                                       \
                UINT cmd = UINT(wParam) & 0xFFF0;                              \
                if (cmd == SC_MINIMIZE && PlayMinimize(hWnd)) {                \
                    lr = name##_orig callArgs;                                 \
                    calledOrig = true;                                         \
                    AfterOrigMinimize(hWnd, false);                            \
                } else if (cmd == SC_RESTORE) {                                \
                    /* PlayRestore cloaks internally, atomically with queueing */ \
                    /* the animation -- see StartQueuedAnimation. */           \
                    if (!PlayRestore(hWnd))                                    \
                        ForgetCapture(hWnd);                                   \
                }                                                              \
            }                                                                  \
        } catch (...) {                                                       \
            Wh_Log(L"DefWindowProc-style hook: unexpected exception, "        \
                   L"falling back to original");                               \
        }                                                                      \
        if (calledOrig)                                                        \
            return lr;                                                         \
        return name##_orig callArgs;                                           \
    }
#define DWP_HOOK(name, defArgs, callArgs)                                      \
    DWP_HOOK_(name##A, defArgs, callArgs) DWP_HOOK_(name##W, defArgs, callArgs)
DWP_HOOK(DefWindowProc, (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam),
         (hWnd, uMsg, wParam, lParam))
DWP_HOOK(DefFrameProc,
         (HWND hWnd, HWND hWndMDIClient, UINT uMsg, WPARAM wParam,
          LPARAM lParam),
         (hWnd, hWndMDIClient, uMsg, wParam, lParam))
DWP_HOOK(DefMDIChildProc, (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam),
         (hWnd, uMsg, wParam, lParam))
DWP_HOOK(DefDlgProc, (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam),
         (hWnd, uMsg, wParam, lParam))

static UINT CmdFromShow(int c) {
    switch (c) {
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
    if (IsDisabled() || g_fAnimating.load())
        return ShowWindow_orig(hWnd, nCmdShow);
    // ShowWindow is documented as synchronous: start the overlay animation
    // if possible, then let the original run straight away, so the classic
    // restore-then-act patterns (SetForegroundWindow, GetWindowRect,
    // IsIconic, WM_SIZE-dependent layout) see a window that is already
    // restored when this call returns.
    // This hook fires on the UI thread of every process that calls
    // ShowWindow -- i.e. nearly everything. Anything our animation setup
    // throws here must never reach the caller; ShowWindow_orig always runs
    // regardless of what happens above it.
    bool playedMin = false;
    try {
        UINT cmd = CmdFromShow(nCmdShow);
        if (cmd == SC_MINIMIZE)
            playedMin = PlayMinimize(hWnd);
        else if (cmd == SC_RESTORE) {
            // PlayRestore cloaks internally, atomically with queueing the
            // animation -- see StartQueuedAnimation.
            if (!PlayRestore(hWnd))
                ForgetCapture(hWnd);
        }
        if (nCmdShow == SW_HIDE)
            ShellPreCaptureForClose(hWnd);
    } catch (...) {
        Wh_Log(L"ShowWindow_hook: unexpected exception, falling back to original");
        playedMin = false;
    }
    BOOL r = ShowWindow_orig(hWnd, nCmdShow);
    if (playedMin) {
        try {
            AfterOrigMinimize(hWnd, false);
        } catch (...) {
            Wh_Log(L"ShowWindow_hook: AfterOrigMinimize threw, ignoring");
        }
    }
    return r;
}
BOOL WINAPI ShowWindowAsync_hook(HWND hWnd, int nCmdShow) {
    if (IsDisabled() || g_fAnimating.load())
        return ShowWindowAsync_orig(hWnd, nCmdShow);
    // Same reasoning as ShowWindow_hook above: this must never let an
    // exception from our own bookkeeping reach the caller.
    bool playedMin = false;
    try {
        UINT cmd = CmdFromShow(nCmdShow);
        if (cmd == SC_MINIMIZE)
            playedMin = PlayMinimize(hWnd);
        else if (cmd == SC_RESTORE) {
            // PlayRestore cloaks internally, atomically with queueing the
            // animation -- see StartQueuedAnimation.
            if (!PlayRestore(hWnd))
                ForgetCapture(hWnd);
        }
        if (nCmdShow == SW_HIDE)
            ShellPreCaptureForClose(hWnd);
    } catch (...) {
        Wh_Log(L"ShowWindowAsync_hook: unexpected exception, falling back to original");
        playedMin = false;
    }
    BOOL r = ShowWindowAsync_orig(hWnd, nCmdShow);
    if (playedMin) {
        try {
            AfterOrigMinimize(hWnd, true);
        } catch (...) {
            Wh_Log(L"ShowWindowAsync_hook: AfterOrigMinimize threw, ignoring");
        }
    }
    return r;
}
// Window prop guards against re-entry; the original DestroyWindow is always called.
BOOL WINAPI DestroyWindow_hook(HWND hWnd) {
    bool animated = false;
    if (g_animateClose && !GetPropW(hWnd, kClosingProp) &&
        CloseIsProgressSafe(hWnd)) {
        ScopedProp closing(hWnd, kClosingProp);
        // nothing thrown by the animation may ever reach the application's close path
        try {
            if (closing.ok())
                animated = PlayClose(hWnd);
        } catch (...) {
            animated = false;
        }
    }
    // The map/mutex-backed cache cleanup below can't be allowed to stop
    // DestroyWindow_orig from running -- same rule as the animation above.
    try {
        ForgetCapture(hWnd);
        ForgetShellPreCapture(hWnd);
    } catch (...) {
        Wh_Log(L"DestroyWindow_hook: cache cleanup threw, ignoring");
    }
    BOOL r = DestroyWindow_orig(hWnd);
    if (!r && animated && IsWindow(hWnd)) {
        // The real destroy was refused (e.g. WM_CLOSE was cancelled): undo
        // our own state so the window isn't left cloaked/transition-disabled
        // forever. This recovery path must itself be exception-safe, or a
        // window that already failed to close could be left invisible too.
        try {
            DisableTransitions(hWnd, FALSE);
            // Abort any overlay animation still running for this window
            // (possible only if WaitForCloseAnimation hit its deadline)
            // before re-showing it. Clearing g_hwndCurrent is the cancel
            // signal RunAnimation polls; the overlay thread's FinishQueued
            // then hides the overlay and releases the animation slot, so
            // the slot is deliberately NOT released from here.
            HWND cur = g_hwndCurrent.load();
            if (cur == hWnd) {
                g_hwndCurrent.store(nullptr);
                g_typeCurrent.store(0);
            }
            if (ShowWindow_orig)
                ShowWindow_orig(hWnd, SW_SHOWNA);
            else
                ::ShowWindow(hWnd, SW_SHOWNA);
        } catch (...) {
            Wh_Log(L"DestroyWindow_hook: recovery path threw, ignoring");
        }
    }
    return r;
}

static HMODULE GetCurrentModule() {
    HMODULE m = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       LPCWSTR(&GetCurrentModule), &m);
    return m;
}

static void SafeCleanup() {
    if (InterlockedExchange(&g_isUninitializing, 1))
        return;
    g_stopping.store(true);
    HWND ha = g_hwndAnim.load();
    if (ha && IsWindow(ha)) {
        // AnimWndProc handles WM_CLOSE by calling DestroyWindow(hwnd) itself, on its own
        // thread. We don't own that thread here, so we don't pump/dispatch its queue, and
        // we don't attempt a cross-thread DestroyWindow (it would fail with
        // ERROR_ACCESS_DENIED regardless). StopAnimThread() below posts WM_QUIT and joins
        // the thread, which is what actually guarantees teardown.
        SendMessageW(ha, WM_CLOSE, 0, 0);
    }
    {
        std::lock_guard<std::mutex> lock(g_queueMutex);
        while (!g_queue.empty()) {
            auto* req = g_queue.front();
            g_queue.pop_front();
            if (!req)
                continue;
            if (req->hwnd && IsWindow(req->hwnd))
                DisableTransitions(req->hwnd, FALSE);
            // ~AnimRequest signals hDone, releasing any app thread parked in
            // WaitForCloseAnimation on this request.
            delete req;
        }
    }
    HWND cur = g_hwndCurrent.load();
    if (cur && IsWindow(cur))
        DisableTransitions(cur, FALSE);
    StopAnimThread();
    {
        HWND c = g_cloakedHwnd.exchange(nullptr);
        if (c && IsWindow(c))
            UncloakAfterRestore(c);
    }
    {
        std::lock_guard<std::mutex> lock(g_cacheMutex);
        g_captureCache.clear();
        g_captureLru.clear();
        g_cacheBytes = 0;
    }
    {
        std::lock_guard<std::mutex> lock(g_pendingCloseMutex);
        g_pendingClose = PendingClose{};
        g_pendingCloseHwnd.store(nullptr);
    }
}

BOOL Wh_ModInit() {
    // Anything unexpected during init must not crash the host process --
    // fail the mod load instead and leave the process untouched.
    try {
        InitExeName();
        g_hinst = GetCurrentModule();
        LoadSettings();
        HMODULE user32 = GetModuleHandleW(L"user32.dll");
        if (!user32)
            return FALSE;
        pGetWindowMinimizeRect = GetWindowMinimizeRect_t(
            GetProcAddress(user32, "GetWindowMinimizeRect"));
        pIsHungAppWindow =
            IsHungAppWindow_t(GetProcAddress(user32, "IsHungAppWindow"));
        InterlockedExchange(&g_isUninitializing, 0);
#define HOOK_WIDE2(s) L##s
#define HOOK_WIDE(s) HOOK_WIDE2(s)
#define HOOK(f)                                                                \
    if (!WindhawkUtils::SetFunctionHook(f, f##_hook, &f##_orig)) {             \
        Wh_Log(L"hook failed: %s", HOOK_WIDE(#f));                             \
        return FALSE;                                                          \
    }
        HOOK(DefWindowProcA)
        HOOK(DefWindowProcW) HOOK(DefFrameProcA) HOOK(DefFrameProcW)
            HOOK(DefMDIChildProcA) HOOK(DefMDIChildProcW) HOOK(DefDlgProcA)
                HOOK(DefDlgProcW) HOOK(ShowWindow) HOOK(ShowWindowAsync)
                    HOOK(DestroyWindow)
#undef HOOK
#undef HOOK_WIDE
#undef HOOK_WIDE2
                        return TRUE;
    } catch (...) {
        Wh_Log(L"Wh_ModInit: unexpected exception, cancelling mod load");
        return FALSE;
    }
}
void Wh_ModSettingsChanged() {
    try {
        LoadSettings();
        // A settings change is also the user's way of asking for a retry
        // after the overlay thread failed to come up: drop the backoff so the
        // next candidate window tries again immediately.
        if (!g_stopping.load())
            ClearOverlayThreadFailure();
    } catch (...) {
        Wh_Log(L"Wh_ModSettingsChanged: unexpected exception");
    }
}
void Wh_ModBeforeUninit() {
    try {
        SafeCleanup();
    } catch (...) {
        Wh_Log(L"Wh_ModBeforeUninit: unexpected exception");
    }
}
void Wh_ModUninit() {
    // Mirrors Wh_ModInit: teardown must not throw back into the host either,
    // or unloading the mod could take the process down with it.
    try {
        SafeCleanup();
        if (g_hinst)
            UnregisterClassW(kAnimClassName, g_hinst);
    } catch (...) {
        Wh_Log(L"Wh_ModUninit: unexpected exception");
    }
}
