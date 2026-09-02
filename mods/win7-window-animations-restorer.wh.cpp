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

This mod tries to restore the Windows 7 Aero window animations on Windows 10 and 11 **without modifying the DWM**.

## Sample Animation

![Demo GIF](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/bandicam2026-08-3014-19-03-266-ezgif.com-video-to-gif-converter.gif)

## What it does

- **Minimize / restore**: the window shrinks toward its taskbar button with the Windows 7 Aero perspective tilt (5 degrees pitch, 8 degrees yaw, depth and fade, 250 ms), and grows back the same way. Restores keep `ShowWindow(SW_RESTORE)` fully synchronous (the window's state has already changed when the call returns); DWM cloaking keeps the real window off-screen for the ~210 ms of the fly-in, so no restored-before-animation frame is ever visible.
- **Close**: the composed window frame is captured and tilted away over 200 ms. The animation runs when the application's own `DestroyWindow` starts, while the caller's message queue keeps being pumped (the same pattern a modal dialog uses), so the app stays responsive, the process stays alive for the duration, and `WM_CLOSE` / `SC_CLOSE` keep their normal meaning: a "save your changes?" prompt can still cancel the close.
- **Open**: left to Windows, which animates it natively (this could change in future updates).

## Known Limitations

- The opening animation is left to Windows to avoid instability.
- If a window refuses to minimize, the animation is canceled.
- Dialogs without a minimize button are not animated.
- Some UWP apps might not support the closing animation.
- The Snipping tool does not support the closing animation for stability reasons.

## Which applications are affected

The mod is injected into every process (`@include *`) excluding a short list of system, shell and UWP hosts that cannot be animated anyway. Only real top-level windows of the host process itself are ever touched. To restrict it to a few programs, use the "Custom process inclusion/exclusion list" in Windhawk's advanced mod settings.

## Notes

This mod is a **best-effort recreation**: if there are suggestions or problems with the mod, please contact the author of the modification.
The mod has been tested on Windows 10 21H2, Windows 11 24H2 and Windows 11 25H2. It does not modify system files and does not replace any Windows component; it replicates the timing and the motion of Windows 7. Enable logging in the mod's advanced settings to get the reason why a given window was not animated.
This modification specifically recreates, within its capabilities, the Windows 7 Aero perspective animation for minimize, restore, and close. It is recommended to not run it together with other window animation mods.

## Credits

Visual references only (no code was taken from these):

- **DWM 3D Transforms** by [xalejandro](https://github.com/tetawaves).
- [OpenGlass](https://github.com/ALTaleX531/OpenGlass) by ALTaleX.
- **3D Aero Transforms mod** by [kieldbg](https://github.com/kieldbg).


- The development of this mod started from [Classic Minimize/Maximize Animations](https://windhawk.net/mods/classic-min-max-animations)
  by [aubymori](https://github.com/aubymori) (overlay / `user32` hooking architecture) and it was substantially modified. Copyright (c) aubymori, used under the MIT license
  (https://opensource.org/licenses/MIT).
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
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <dwmapi.h>
#include <shellscalingapi.h>
#include <algorithm>
#include <atomic>
#include <cstdlib>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <deque>
#include <list>
#include <memory>
#include <mutex>
#include <unordered_map>
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

constexpr double kShowHideDurationSec = 0.25;
constexpr double kCloseDurationSec = 0.20;
constexpr double kRestoreDurationSec = 0.21;

constexpr int kMaxCaptureSide = 16384;

enum class AnimationType {
    None = 0,
    Close,
    Minimize,
    RestoreFromMinimized
};

static float Lerp(float a, float b, float t) {
    return a + (b - a) * t;
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
static RECT AspectCorrectedMinimizeTarget(const RECT& button) {
    float bw = float(RECTW(button)), bh = float(RECTH(button));
    if (bw < 1 || bh < 1)
        return button;
    float ar = bh / bw;
    RECT t = button;
    t.right = t.left + LONG(bw * ar);
    t.bottom = t.top + LONG(bh * ar);
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
        case AnimationType::Minimize:
            p.rotX = 5.f * t;
            p.rotY = 8.f * t;
            p.transZ = -4.f * t;
            p.opacity = 1.f - 0.35f * t;
            p.ease = t;
            p.pivotY = h * 0.5f;
            break;
        case AnimationType::RestoreFromMinimized: {
            float away = 1.f - t;
            p.rotX = 5.f * away;
            p.rotY = 8.f * away;
            p.transZ = -4.f * away;
            p.opacity = 0.65f + 0.35f * t;
            p.ease = t;
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
            return LerpRect(win, dest, t);
        case AnimationType::RestoreFromMinimized:
            return LerpRect(dest, win, t);
        default:
            return win;
    }
}
static UINT DurationMsFor(AnimationType type) {
    double ms = kShowHideDurationSec * 1000.0;
    if (type == AnimationType::Close) {
        ms = kCloseDurationSec * 1000.0;
    } else if (type == AnimationType::RestoreFromMinimized) {
        ms = kRestoreDurationSec * 1000.0;
    }
    if (ms < 16)
        ms = 16;
    return UINT(std::lround(ms));
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
static void LoadSettings() {
    g_animateMinimize = Wh_GetIntSetting(L"animateMinimize") != 0;
    g_animateClose = Wh_GetIntSetting(L"animateClose") != 0;
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
static const size_t kMaxCachedCaptures = 2;
static const size_t kMaxCachedBytes = 2u * 1024u * 1024u;
// The restore overlay is stretched to the animated rect at present time anyway
// (and the real window is already visible when restore plays), so the cached
// bitmap never needs full resolution: a small copy is kept and the original
// size is remembered for geometry checks. Per-process retention drops from
// tens of MB per minimized window to ~1 MB total.
static const int kMaxCachedSide = 384;
static std::mutex g_cacheMutex;
struct CacheEntry {
    CaptureBits bits;
    std::list<HWND>::iterator lruIt;
};
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
static void CacheCapture(HWND hwnd, CaptureBits&& bits) {
    if (!hwnd || bits.empty())
        return;
    if (CaptureBytes(bits) > kMaxCachedBytes)
        return;
    try {
        std::lock_guard<std::mutex> lock(g_cacheMutex);
        auto it = g_captureCache.find(hwnd);
        if (it != g_captureCache.end()) {
            g_cacheBytes -=
                std::min(g_cacheBytes, CaptureBytes(it->second.bits));
            it->second.bits = std::move(bits);
            g_cacheBytes += CaptureBytes(it->second.bits);
            g_captureLru.splice(g_captureLru.begin(), g_captureLru,
                                it->second.lruIt);
            EvictLocked();
            return;
        }
        g_captureLru.push_front(hwnd);
        g_cacheBytes += CaptureBytes(bits);
        g_captureCache.emplace(
            hwnd, CacheEntry{std::move(bits), g_captureLru.begin()});
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
    if (w < 1 || h < 1 || w > 16384 || h > 16384)
        return false;
    {
        ScopedDpiAware dpi; // screen scrape only -- no app code runs here
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
                                size_t nonBlack = 0;
                                for (size_t i = 0;
                                     i < out.pixels.size() && nonBlack < 100;
                                     ++i)
                                    if ((out.pixels[i] & 0x00FFFFFF) != 0)
                                        ++nonBlack;
                                if (nonBlack >= 10) {
                                    if (!IsSnippingTool())
                                        ForceOpaqueAlpha(out.pixels.data(),
                                                         out.pixels.size());
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
    if (composedOnly) {
        out = {};
        return false;
    }
    {
        // PrintWindow below dispatches WM_PRINT/WM_PRINTCLIENT straight into the
        // target window's own WndProc, on this same thread: that is application
        // code, so it must run under the thread's normal DPI awareness context,
        // never under the per-monitor-aware override used for geometry/capture.
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
static bool GetMaximizeRectPhysical(HWND hwnd, RECT* rc) {
    MINMAXINFO mmi{};
    HMONITOR hmon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{sizeof(mi)};
    if (!GetMonitorInfoW(hmon, &mi))
        return false;
    RECT work = mi.rcWork;
    mmi.ptMaxPosition.x = work.left;
    mmi.ptMaxPosition.y = work.top;
    mmi.ptMaxSize.x = RECTW(work);
    mmi.ptMaxSize.y = RECTH(work);
    if (!SendMessageTimeoutW(hwnd, WM_GETMINMAXINFO, 0, LPARAM(&mmi),
                             SMTO_ABORTIFHUNG, 100, nullptr))
        return false;
    if (mmi.ptMaxSize.x <= 0 || mmi.ptMaxSize.y <= 0)
        return false;
    rc->left = mmi.ptMaxPosition.x;
    rc->top = mmi.ptMaxPosition.y;
    rc->right = rc->left + mmi.ptMaxSize.x;
    rc->bottom = rc->top + mmi.ptMaxSize.y;
    return IsRectUsable(*rc);
}
static bool GetRestoreRectPhysical(HWND hwnd, RECT* rc) {
    if (!rc)
        return false;
    WINDOWPLACEMENT wp{sizeof(wp)};
    if (!GetWindowPlacement(hwnd, &wp))
        return false;
    *rc = wp.rcNormalPosition;
    LONG style = LONG(GetWindowLongPtrW(hwnd, GWL_STYLE));
    if (!(style & WS_CHILD)) {
        HMONITOR hmon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi{sizeof(mi)};
        if (hmon && GetMonitorInfoW(hmon, &mi)) {
            OffsetRect(rc, -(mi.rcMonitor.left - mi.rcWork.left),
                       -(mi.rcMonitor.top - mi.rcWork.top));
        }
    } else {
        if (HWND parent = GetParent(hwnd)) {
            MapWindowPoints(parent, HWND_DESKTOP, LPPOINT(rc), 2);
        }
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
// Bilinear downscale to the cache format. Full-size captures are only used
// live; the copy retained for a later restore is shrunk to kMaxCachedSide.
static CaptureBits DownscaleForCache(const CaptureBits& in) {
    if (in.empty())
        return {};
    int w = in.width, h = in.height;
    if (w <= kMaxCachedSide && h <= kMaxCachedSide)
        return in;
    double s = std::min(double(kMaxCachedSide) / double(w),
                        double(kMaxCachedSide) / double(h));
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
static void GhostWatchdog();
static std::atomic<UINT> g_msgAnim{0};
static std::atomic<bool> g_fAnimating{false};
static std::atomic<bool> g_fDisabled{false};
static std::atomic<HWND> g_hwndAnim{nullptr}, g_hwndCurrent{nullptr};
static std::atomic<int> g_typeCurrent{0};
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
static std::atomic<bool> g_stopping{false};
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
// Runs the overlay timeline, on the animation thread only. No hook ever waits
// on this: the caller's UI thread is back in application code long before the
// animation finishes.
static void RunAnimation(HWND hwndOverlay, AnimRequest& req) {
    if (req.capture.empty() || !IsRectUsable(req.rcWindow))
        return;
    if (g_stopping.load())
        return;
    UINT dur = req.durationMs ? req.durationMs : 250;
    ShowOverlayWindow(hwndOverlay);
    if (req.gdi)
        req.gdi->EnsureSource(req.capture);
    ULONGLONG start = GetTickCount64(), elapsed = 0;
    float lastT = -1;
    while (!g_stopping.load() && g_hwndCurrent.load() == req.hwnd &&
           (elapsed = GetTickCount64() - start) < dur) {
        float t =
            dur == 0 ? 1.f : std::clamp(float(elapsed) / float(dur), 0.f, 1.f);
        if (t - lastT >= 0.001f) {
            lastT = t;
            PresentTime(hwndOverlay, req, t);
        }
        if (GetTickCount64() - start >= dur)
            break;
        DwmFlush();
    }
    if (!g_stopping.load())
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
    if (req->hwnd && IsWindow(req->hwnd))
        DisableTransitions(req->hwnd, FALSE);
    if (req->type == AnimationType::Minimize && req->hwnd &&
        IsWindow(req->hwnd) && IsIconic(req->hwnd)) {
        CaptureBits small = DownscaleForCache(req->capture);
        CacheCapture(req->hwnd, std::move(small));
    }
    HWND cur = g_hwndCurrent.load();
    if (cur == req->hwnd) {
        g_hwndCurrent.store(nullptr);
        g_typeCurrent.store(0);
        g_fAnimating.store(false);
    }
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
                if (lParam &&
                    PresentFirstFrame(hwnd,
                                      *reinterpret_cast<AnimRequest*>(lParam)))
                    return 1;
                break;
            case AnimMsg::Drain:
                DrainQueue(hwnd);
                break;
            case AnimMsg::Hide:
                HideOverlayWindow(hwnd);
                break;
            case AnimMsg::GhostArm:
                SetTimer(hwnd, kGhostTimer, UINT(lParam), nullptr);
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
static bool WaitForAnimWndThread() {
    if (g_stopping.load())
        return false;
    std::lock_guard<std::mutex> lock(g_animThreadMutex);
    if (g_stopping.load())
        return false;
    if (g_hAnimWndThread) {
        HWND ha = g_hwndAnim.load();
        if (ha && IsWindow(ha))
            return true;
        if (g_dwAnimThreadId)
            PostThreadMessageW(g_dwAnimThreadId, WM_QUIT, 0, 0);
        // the handle is kept so StopAnimThread() can still join it at uninit
        if (WaitForSingleObject(g_hAnimWndThread, 2000) != WAIT_OBJECT_0) {
            g_fDisabled.store(true);
            Wh_Log(L"overlay thread did not exit in time");
            return false;
        }
        CloseHandle(g_hAnimWndThread);
        g_hAnimWndThread = nullptr;
        g_dwAnimThreadId = 0;
    }
    ScopedHandle hEvent(CreateEventW(nullptr, TRUE, FALSE, nullptr));
    if (!hEvent)
        return false;
    g_hAnimWndThread =
        CreateThread(nullptr, 0, AnimWndThreadProc, hEvent.get(), 0, nullptr);
    if (!g_hAnimWndThread)
        return false;
    g_dwAnimThreadId = GetThreadId(g_hAnimWndThread);
    if (WaitForSingleObject(hEvent.get(), 1000) != WAIT_OBJECT_0) {
        hEvent.release();
        g_fDisabled.store(true);
        return false;
    }
    if (!g_hwndAnim.load()) {
        g_fDisabled.store(true);
        return false;
    }
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
    if (GetAncestor(hwnd, GA_ROOT) != hwnd) {
        LONG s = LONG(GetWindowLongPtrW(hwnd, GWL_STYLE));
        if (!(s & WS_CHILD) || !(s & WS_CAPTION))
            return false;
    }
    LONG style = LONG(GetWindowLongPtrW(hwnd, GWL_STYLE)),
         ex = LONG(GetWindowLongPtrW(hwnd, GWL_EXSTYLE));
    if (!(style & WS_CAPTION))
        return false;
    if (ex & WS_EX_TOOLWINDOW)
        return false;
    if (ex & WS_EX_NOACTIVATE)
        return false;
    if (hwnd == g_hwndAnim.load())
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
    if (g_fDisabled.load() || g_fAnimating.load())
        return false;
    if (!IsAnimateCandidate(hwnd))
        return false;
    if (!IsWindowVisible(hwnd) || IsIconic(hwnd))
        return false;
    return true;
}
static bool ShouldAnimateClose(HWND hwnd) {
    if (g_fDisabled.load() || g_fAnimating.load())
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
    SendMessageW(g_hwndAnim.load(), g_msgAnim.load(),
                 WPARAM(AnimMsg::FirstFrame), LPARAM(&req));
    DisableTransitions(hwnd, TRUE);
    Wh_Log(L"animation queued: type=%d %dx%d", int(type), RECTW(rcWin),
           RECTH(rcWin));
    if (!QueueRun(std::move(req))) {
        DisableTransitions(hwnd, FALSE);
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
        g_hwndCurrent.store(nullptr);
        g_typeCurrent.store(0);
        g_fAnimating.store(false);
        HWND ha = g_hwndAnim.load();
        if (ha && IsWindow(ha))
            HideOverlayWindow(ha);
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
        rcMin = AspectCorrectedMinimizeTarget(rcMin);
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
        if (g_fDisabled.load() || g_fAnimating.load())
            return false;
        if (!HasCachedCapture(hwnd))
            return false;
        RECT rcMin{}, rcRest{};
        bool have = false, restoreToMax = false;
        {
            ScopedDpiAware dpi;
            if (!GetMinimizeRectPhysical(hwnd, &rcMin))
                return false;
            rcMin = AspectCorrectedMinimizeTarget(rcMin);
            WINDOWPLACEMENT wp{sizeof(wp)};
            if (GetWindowPlacement(hwnd, &wp))
                restoreToMax = (wp.flags & WPF_RESTORETOMAXIMIZED) != 0;
            if (!restoreToMax)
                have = GetRestoreRectPhysical(hwnd, &rcRest);
        }
        // GetMaximizeRectPhysical sends WM_GETMINMAXINFO straight into the app's own
        // WndProc on this thread: that's app code, so it must run outside the DPI
        // override (it doesn't need it either -- MONITORINFO is already physical).
        if (restoreToMax)
            have = GetMaximizeRectPhysical(hwnd, &rcRest);
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
// window just closed exits within milliseconds -- taking the overlay thread
// (and the animation) down with it. So the fly-out runs here, but:
//   - for the duration, this thread only answers SENT messages (SendMessage
//     from other threads/processes, DWM, IsHungAppWindow) so it never looks
//     hung; posted messages and input are deliberately left queued and NOT
//     dispatched, because this runs inside the app's own DestroyWindow call
//     (from a WM_CLOSE/WM_COMMAND handler, a destructor, possibly under the
//     app's own locks) -- a point it never expects to be re-entered from;
//   - because nothing is removed from the queue, a WM_QUIT already pending
//     (e.g. PostQuitMessage() called right before DestroyWindow()) is left
//     completely untouched and is seen by the app's own loop the moment the
//     effect ends, with its normal semantics -- no special-casing needed;
//   - the loop bails out within a frame when the mod is unloading
//     (g_stopping), so disable/settings-reload never wait on the effect;
//   - the DPI override stays scoped to the geometry/capture and to each
//     single PresentOverlay call, and is NEVER held while application code
//     runs (the hide below runs under the thread's own, unmodified
//     awareness context).
static bool PlayCloseAnimation(HWND hwnd, CaptureBits&& preCap,
                               const RECT& preRect) {
    bool exp = false;
    if (!g_fAnimating.compare_exchange_strong(exp, true))
        return false;
    if (!WaitForAnimWndThread() || g_stopping.load()) {
        g_fAnimating.store(false);
        return false;
    }
    ScopedDwmTransitions transWnd(hwnd);
    transWnd.Disable();
    RECT rcWin = preRect;
    CaptureBits cap = std::move(preCap);
    if (cap.empty()) {
        {
            ScopedDpiAware dpi;
            if (!GetFrameBoundsPhysical(hwnd, &rcWin)) {
                Wh_Log(L"close: capture failed");
                g_fAnimating.store(false);
                return false;
            }
        }
        // CaptureWindowForClose scopes its own DPI override; it may fall back to
        // PrintWindow, which is app code and must not run under this one.
        if (!CaptureWindowForClose(hwnd, cap, false)) {
            Wh_Log(L"close: capture failed");
            g_fAnimating.store(false);
            return false;
        }
    }
    if (cap.empty() || !IsRectUsable(rcWin)) {
        g_fAnimating.store(false);
        return false;
    }
    AnimRequest req;
    req.hwnd = hwnd;
    req.type = AnimationType::Close;
    req.rcWindow = rcWin;
    req.rcDest = rcWin;
    req.capture = std::move(cap);
    req.durationMs = DurationMsFor(AnimationType::Close);
    try {
        req.gdi = std::make_unique<PresentGdi>();
    } catch (...) {
        g_fAnimating.store(false);
        return false;
    }
    HWND ha = g_hwndAnim.load();
    UINT msg = g_msgAnim.load();
    if (!ha || !msg || !IsWindow(ha)) {
        g_fAnimating.store(false);
        return false;
    }
    g_hwndCurrent.store(hwnd);
    g_typeCurrent.store(int(AnimationType::Close));
    SendMessageW(ha, msg, WPARAM(AnimMsg::FirstFrame), LPARAM(&req));
    // The hide dispatches WM_SHOWWINDOW & co. inside the app: no DPI override
    // is held here, so the app runs under its own awareness context.
    if (IsWindowVisible(hwnd)) {
        if (ShowWindow_orig)
            ShowWindow_orig(hwnd, SW_HIDE);
        else
            ::ShowWindow(hwnd, SW_HIDE);
    }
    ULONGLONG start = GetTickCount64(), elapsed = 0;
    float lastT = -1;
    while (!g_stopping.load() &&
           (elapsed = GetTickCount64() - start) <= req.durationMs) {
        float t =
            req.durationMs == 0
                ? 1.f
                : std::clamp(float(elapsed) / float(req.durationMs), 0.f, 1.f);
        if (t - lastT >= 0.001f) {
            lastT = t;
            auto p = ParamsFor(AnimationType::Close, t, float(RECTH(rcWin)));
            ScopedDpiAware dpi; // one present call only -- no app code inside
            PresentOverlay(ha, *req.gdi, req, rcWin, p);
        }
        // Answer sent messages only (see the comment above the function): this
        // keeps the thread from looking hung without dispatching posted
        // messages or input into the app, and without ever touching WM_QUIT.
        MsgWaitForMultipleObjectsEx(0, nullptr, 0, QS_SENDMESSAGE,
                                    MWMO_INPUTAVAILABLE);
        MSG m;
        PeekMessageW(&m, nullptr, 0, 0, PM_NOREMOVE | PM_QS_SENDMESSAGE);
        DwmFlush();
    }
    {
        Win7TransformParams pe =
            ParamsFor(AnimationType::Close, 1.f, float(RECTH(rcWin)));
        pe.opacity = 0.f;
        ScopedDpiAware dpi;
        PresentOverlay(ha, *req.gdi, req, rcWin, pe);
    }
    HideOverlayWindow(ha);
    HWND c = g_hwndCurrent.load();
    if (c == hwnd) {
        g_hwndCurrent.store(nullptr);
        g_typeCurrent.store(0);
    }
    g_fAnimating.store(false);
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
// Explorer and Control Panel hide their frame before destroying it, so at DestroyWindow
// there is nothing left to capture: for those two classes only, the bitmap is taken
// right before the hide and used if the destroy follows within kShellHideToCloseMs.
constexpr ULONGLONG kShellHideToCloseMs = 400;
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
        if (!g_animateClose || g_fDisabled.load() || g_fAnimating.load() ||
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
        if (WaitForAnimWndThread()) {
            HWND ha = g_hwndAnim.load();
            UINT msg = g_msgAnim.load();
            std::unique_ptr<PresentGdi> gdi;
            try {
                gdi = std::make_unique<PresentGdi>();
            } catch (...) {
                gdi.reset();
            }
            if (ha && msg && IsWindow(ha) && gdi && !g_fAnimating.load()) {
                AnimRequest frame;
                frame.hwnd = hwnd;
                frame.type = AnimationType::Close;
                frame.rcWindow = rc;
                frame.rcDest = rc;
                frame.capture = std::move(cap);
                frame.gdi = std::move(gdi);
                // The ghost frame is presented by the overlay thread (already
                // per-monitor DPI-aware), so the DPI override on this thread stays
                // scoped to the capture above and is never held across presents.
                ghost = SendMessageW(ha, msg, WPARAM(AnimMsg::FirstFrame),
                                     LPARAM(&frame)) == 1;
                if (!ghost)
                    HideOverlayWindow(
                        ha); // 1x1 transparent + SWP_HIDEWINDOW: coordinate-free
                cap = std::move(frame.capture);
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
static bool TakeShellPreCapture(HWND hwnd, CaptureBits& cap, RECT& rc) {
    try {
        if (g_pendingCloseHwnd.load() != hwnd)
            return false;
        ULONGLONG tick = 0;
        bool ghost = false;
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
            if (ghost)
                HideGhostOverlay();
            return false;
        }
        if (!CaptureLooksComposed(cap)) {
            cap = {};
            if (ghost)
                HideGhostOverlay();
            return false;
        }
        return true;
    } catch (...) {
        cap = {};
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
    bool usePre = false;
    if (!ShouldAnimateClose(hwnd)) {
        if (g_fDisabled.load() || g_fAnimating.load())
            return false;
        if (IsWindowVisible(hwnd) || IsIconic(hwnd))
            return false;
        if (!IsTopLevelCloseCandidate(hwnd))
            return false;
        usePre = TakeShellPreCapture(hwnd, pre, preRc);
        if (!usePre) {
            Wh_Log(L"close: hidden window without a fresh shell capture");
            return false;
        }
    } else
        ForgetShellPreCapture(hwnd);
    // PlayCloseAnimation owns the animation slot from its CAS to its end: the
    // whole fly-out completes before the real destroy is allowed through.
    return PlayCloseAnimation(hwnd, std::move(pre), preRc);
}

#define DWP_HOOK_(name, defArgs, callArgs)                                     \
    LRESULT(CALLBACK* name##_orig) defArgs;                                    \
    LRESULT CALLBACK name##_hook defArgs {                                     \
        if (uMsg == WM_SHOWWINDOW && wParam == FALSE && lParam == 0)           \
            ShellPreCaptureForClose(hWnd);                                     \
        if (uMsg == WM_SYSCOMMAND) {                                           \
            UINT cmd = UINT(wParam) & 0xFFF0;                                  \
            if (cmd == SC_MINIMIZE && PlayMinimize(hWnd)) {                    \
                LRESULT lr = name##_orig callArgs;                             \
                AfterOrigMinimize(hWnd, false);                                \
                return lr;                                                     \
            }                                                                  \
            if (cmd == SC_RESTORE) {                                           \
                if (PlayRestore(hWnd))                                         \
                    CloakForRestoreAnim(hWnd);                                 \
                else                                                           \
                    ForgetCapture(hWnd);                                       \
            }                                                                  \
        }                                                                      \
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
    if (g_fDisabled.load() || g_fAnimating.load())
        return ShowWindow_orig(hWnd, nCmdShow);
    UINT cmd = CmdFromShow(nCmdShow);
    // ShowWindow is documented as synchronous: start the overlay animation
    // if possible, then let the original run straight away, so the classic
    // restore-then-act patterns (SetForegroundWindow, GetWindowRect,
    // IsIconic, WM_SIZE-dependent layout) see a window that is already
    // restored when this call returns.
    bool playedMin = false, playedRestore = false;
    if (cmd == SC_MINIMIZE)
        playedMin = PlayMinimize(hWnd);
    else if (cmd == SC_RESTORE) {
        playedRestore = PlayRestore(hWnd);
        if (playedRestore)
            CloakForRestoreAnim(hWnd);
        else
            ForgetCapture(hWnd);
    }
    if (nCmdShow == SW_HIDE)
        ShellPreCaptureForClose(hWnd);
    BOOL r = ShowWindow_orig(hWnd, nCmdShow);
    if (playedMin)
        AfterOrigMinimize(hWnd, false);
    return r;
}
BOOL WINAPI ShowWindowAsync_hook(HWND hWnd, int nCmdShow) {
    if (g_fDisabled.load() || g_fAnimating.load())
        return ShowWindowAsync_orig(hWnd, nCmdShow);
    UINT cmd = CmdFromShow(nCmdShow);
    // ShowWindow is documented as synchronous: start the overlay animation
    // if possible, then let the original run straight away, so the classic
    // restore-then-act patterns (SetForegroundWindow, GetWindowRect,
    // IsIconic, WM_SIZE-dependent layout) see a window that is already
    // restored when this call returns.
    bool playedMin = false, playedRestore = false;
    if (cmd == SC_MINIMIZE)
        playedMin = PlayMinimize(hWnd);
    else if (cmd == SC_RESTORE) {
        playedRestore = PlayRestore(hWnd);
        if (playedRestore)
            CloakForRestoreAnim(hWnd);
        else
            ForgetCapture(hWnd);
    }
    if (nCmdShow == SW_HIDE)
        ShellPreCaptureForClose(hWnd);
    BOOL r = ShowWindowAsync_orig(hWnd, nCmdShow);
    if (playedMin)
        AfterOrigMinimize(hWnd, true);
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
    ForgetCapture(hWnd);
    ForgetShellPreCapture(hWnd);
    BOOL r = DestroyWindow_orig(hWnd);
    if (!r && animated && IsWindow(hWnd)) {
        DisableTransitions(hWnd, FALSE);
        // Abort any overlay animation still queued for this window before re-showing it.
        HWND cur = g_hwndCurrent.load();
        if (cur == hWnd) {
            g_hwndCurrent.store(nullptr);
            g_typeCurrent.store(0);
            g_fAnimating.store(false);
        }
        if (ShowWindow_orig)
            ShowWindow_orig(hWnd, SW_SHOWNA);
        else
            ::ShowWindow(hWnd, SW_SHOWNA);
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
    g_fDisabled.store(true);
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
}
void Wh_ModSettingsChanged() {
    LoadSettings();
}
void Wh_ModBeforeUninit() {
    SafeCleanup();
}
void Wh_ModUninit() {
    SafeCleanup();
    if (g_hinst)
        UnregisterClassW(kAnimClassName, g_hinst);
}
