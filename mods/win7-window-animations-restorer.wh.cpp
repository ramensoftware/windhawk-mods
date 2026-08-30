// ==WindhawkMod==
// @id              win7-window-animations-restorer
// @name            Windows 7 Window Animations Restorer
// @description     This mod restores the Windows 7 Aero minimize and restore animation on classic Win32 windows without hooking DWM. 
// @version         1.0.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @include         explorer.exe
// @include         notepad.exe
// @include         wordpad.exe
// @include         ApplicationFrameHost.exe
// @include         mspaint.exe
// @include         SnippingTool.exe
// @include         iexplore.exe
// @include         regedit.exe
// @include         rundll32.exe
// @include         calc.exe
// @include         charmap.exe
// @include         taskmgr.exe
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
// @include         chrome.exe
// @include         msedge.exe
// @include         firefox.exe
// @include         code.exe
// @include         devenv.exe
// @include         notepad++.exe
// @include         7zFM.exe
// @include         powershell.exe
// @include         pwsh.exe
// @compilerOptions -lgdi32 -lmsimg32 -lshcore -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
## About

This mod restores the Windows 7 window animations on Windows 10 and Windows 11,
on classic Win32 programs, **without hooking DWM**.
This is a best effort recreation. The new Windows draws windows
differently, so a perfect copy is not possible. The timings and the movements
are taken from how Windows 7 really did it (`uDWM.dll` 6.1.7600.16385), as far
as the new Windows allows.

This mod is new, and it can get better over time thanks to user feedback and
contributions. For any problems, please report them to the author.

## Sample Animation

![Demo GIF](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/bandicam2026-08-3014-19-03-266-ezgif.com-video-to-gif-converter.gif)

## What it does

* **Minimize / restore** — the window shrinks towards its taskbar button
  (pitch 5°, yaw 8°, 250 ms linear) and restore plays the same movement
  backwards. This is the Win7 motion.
* **Open / close** — left to Windows: the mod does not touch the open and
  close animations at all, so the native Windows animation plays. Only the
  minimize / restore motion is recreated.
* **Maximize / restore down** — not animated: Windows 7 had no dedicated
  maximize animation, and a resize overlay was too unstable.

## Requirements

The mod hooks generic `user32` functions (`DefWindowProc*`, `ShowWindow`,
`ShowWindowAsync`, `DestroyWindow`) and is deliberately injected only into a
curated list of normal applications (the `@include` list at the top of this
file: Explorer, Notepad, Office, common browsers, common editors, Windows
utilities, games, ...). It is intentionally *not* injected into every process:
keeping the list small limits both the hooking overhead and the surface for
problems in unrelated software. The capture itself is a screen scrape, so a
minimizing window can still be animated from the taskbar even when the
application that owns it is not itself listed (e.g. via explorer.exe).

To animate an application that is not in the list, add its `.exe` name to the
`@include` lines at the top of this file, then optionally restrict it further
with the `processes` setting below. Because the `@include` list is decided at
compile time by Windhawk, it cannot be changed at runtime — the `processes`
setting is an extra runtime allowlist *on top of* the static list (leave it
empty to allow every injected process).

## Notes

* The mod has been tested on Windows 10 21H2.
* The mod only changes how the animations look; when in doubt it steps aside
  and lets Windows behave normally.
* It does not modify system files and it does not replace parts of Windows.
* Glass and blur effects from other mods (such as OpenGlass or DWMBlurGlass)
  keep working.
* The real window is never cloaked. Cloaking empties the client of Explorer,
  Control Panel, Task Manager and other apps.
* Known limitation: UWP / WinUI windows still show the normal Windows animation.
* Known limitation: during the ~250 ms restore animation the window is kept
  iconic and the overlay grows over it; a program that calls
  `ShowWindow(SW_RESTORE)` and immediately acts on the result
  (`SetForegroundWindow`, `GetClientRect`, ...) will observe the iconic state
  until the animation ends. The window itself is restored as soon as the
  animation finishes, so this only matters to code that acts mid-animation.
* Known limitation: a window that deliberately vetoes its own minimize is
  left alone — the animation is cancelled rather than forcing the window down.

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
- animateMinimize: true
  $name: Animate minimizing and restoring windows
  $description: This setting enables the 250 ms shrink/grow toward the taskbar button with the Win7 3D tilt when a window minimizes or restores. Turn it off to let Windows animate (or not) on its own.
- animateClose: false
  $name: Animate closing windows (experimental)
  $description: This setting fades the window out over a short scale-down animation before it is actually closed, instead of the normal Windows close. It briefly blocks the closing thread for the duration of the fade, which is a bit riskier than minimize/restore, so it stays off by default. Enable it if you want the closer-to-Win7 look and are fine with that tradeoff.
- processes: ""
  $name: Animate only in these processes
  $description: Optional comma/space-separated list of executable names to restrict the animation to. Leave it empty to animate in every process the mod is injected into (the ones listed in @include at the top of this file). To animate a program that is not already in that @include list, add its .exe name to @include first, then it can be filtered here too.
*/
// ==/WindhawkModSettings==

// ---------------------------------------------------------------------------
// Why this mod exists / how it differs from the other window-animation mods.
//
// The Windhawk catalog already has Classic Minimize/Maximize Animations,
// Windows Animations, macOS Minimize Animation and Genie Minimize Animation,
// all of which replace the minimize/restore animation with their own style.
// This mod is NOT a duplicate of any of them: it specifically recreates the
// Windows 7 Aero 3D tilt (pitch 5°, yaw 8° toward the taskbar button) rather
// than a flat or genie motion, and it does it without hooking the DWM. If a
// user only wants a different window animation, one of those other mods is the
// right choice; if they want the Windows 7 look, this is the one. It is kept
// intentionally conservative: it only animates minimize/restore, leaves open
// and close to Windows, and never cloaks the real window.
// ---------------------------------------------------------------------------

#include <windhawk_utils.h>

#include <dwmapi.h>
#include <shellscalingapi.h>

#include <algorithm>
#include <atomic>
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

#ifndef DWMWA_CLOAK
#define DWMWA_CLOAK 13
#endif

#ifndef DWMWA_EXTENDED_FRAME_BOUNDS
#define DWMWA_EXTENDED_FRAME_BOUNDS 9
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

// Runs the calling thread as per-monitor DPI aware for the duration of the
// scope. The window geometry (GetWindowRect, GetWindowPlacement, the screen
// capture DC) is then expressed in the same physical coordinates that DWM's
// DWMWA_EXTENDED_FRAME_BOUNDS returns, so the two never get mixed and the
// mod no longer needs to hand-convert between virtualized and physical rects.
class ScopedDpiAware {
   public:
    ScopedDpiAware() noexcept
        : m_prev(SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)) {}
    ~ScopedDpiAware() {
        if (m_prev) {
            SetThreadDpiAwarenessContext(m_prev);
        }
    }
    ScopedDpiAware(const ScopedDpiAware&) = delete;
    ScopedDpiAware& operator=(const ScopedDpiAware&) = delete;

   private:
    DPI_AWARENESS_CONTEXT m_prev;
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
// Nella sezione delle dichiarazioni globali (dopo le altre dichiarazioni)
using ShowWindow_t = decltype(&ShowWindow);

// AGGIUNGI QUESTA RIGA QUI:
using ShowWindowAsync_t = decltype(&ShowWindowAsync);
ShowWindowAsync_t ShowWindowAsync_orig = nullptr;

using DestroyWindow_t = decltype(&DestroyWindow);
DestroyWindow_t DestroyWindow_orig = nullptr;
// ---------------------------------------------------------------------------
// Windows 7 constants from uDWM.dll 6.1.7600.16385
// ---------------------------------------------------------------------------

constexpr double kShowHideDurationSec = 0.25;
// Win7's close fade was noticeably quicker than the minimize/restore motion.
constexpr double kCloseDurationSec = 0.12;

enum class AnimationType {
    None = 0,
    Open,
    Close,
    Minimize,
    RestoreFromMinimized,
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
        case AnimationType::Close:
            // No 3D tilt, and no transZ trick either: transZ pushes this
            // into the slow per-pixel rasterizer (see the tiny3d check in
            // PresentOverlay). The shrink for close comes from RectFor
            // instead, which keeps this on the cheap StretchBlt path.
            p.opacity = 1.0f - t;
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
        case AnimationType::Close: {
            // Win7's close wasn't a plain fade: the window also shrank
            // slightly toward its own center while dissolving. 6% by the
            // end is subtle but reads as "closing" rather than just
            // "disappearing". Shrinking the destination rect (rather than
            // using the transZ/rotation matrix) keeps this on the fast
            // StretchBlt path in PresentOverlay instead of the software
            // rasterizer.
            const float scale = 1.0f - 0.06f * t;
            const float cx = (windowRect.left + windowRect.right) * 0.5f;
            const float cy = (windowRect.top + windowRect.bottom) * 0.5f;
            const float halfW = RECTW(windowRect) * 0.5f * scale;
            const float halfH = RECTH(windowRect) * 0.5f * scale;
            RECT rc;
            rc.left = static_cast<LONG>(std::lround(cx - halfW));
            rc.right = static_cast<LONG>(std::lround(cx + halfW));
            rc.top = static_cast<LONG>(std::lround(cy - halfH));
            rc.bottom = static_cast<LONG>(std::lround(cy + halfH));
            return rc;
        }
        default:
            return windowRect;
    }
}

static UINT DurationMsFor(AnimationType type) {
    // Fixed Win7 timing (show/hide fade = open/close, minimize/restore 3D
    // motion, all 250 ms). The speed is never user-scaled.
    double ms = (type == AnimationType::Close ? kCloseDurationSec : kShowHideDurationSec) * 1000.0;
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

bool g_animateMinimize = true;
bool g_animateClose = false;
// Runtime process allowlist: when the `processes` setting is non-empty, only
// the listed executables are allowed to animate (a safety restriction on top
// of the static @include list). Default (empty) allows every injected process.
bool g_processAllowed = true;
wchar_t g_exeName[MAX_PATH] = L"?";

// Case-insensitive comparison, supporting a trailing '*' wildcard
// (e.g. "notepad*", "code*").
static bool MatchesExeName(const wchar_t* pattern, const wchar_t* name) {
    if (!pattern || !name) {
        return false;
    }
    const size_t plen = wcslen(pattern);
    const size_t nlen = wcslen(name);
    if (plen == 0) {
        return false;
    }
    if (pattern[plen - 1] == L'*') {
        const size_t pl = plen - 1;
        if (nlen < pl) {
            return false;
        }
        for (size_t i = 0; i < pl; ++i) {
            wchar_t pa = pattern[i], na = name[i];
            if (pa >= L'A' && pa <= L'Z') pa += 32;
            if (na >= L'A' && na <= L'Z') na += 32;
            if (pa != na) {
                return false;
            }
        }
        return true;
    }
    if (nlen != plen) {
        return false;
    }
    for (size_t i = 0; i < plen; ++i) {
        wchar_t pa = pattern[i], na = name[i];
        if (pa >= L'A' && pa <= L'Z') pa += 32;
        if (na >= L'A' && na <= L'Z') na += 32;
        if (pa != na) {
            return false;
        }
    }
    return true;
}

static void LoadSettings() {
    g_animateMinimize = Wh_GetIntSetting(L"animateMinimize") != 0;
    g_animateClose = Wh_GetIntSetting(L"animateClose") != 0;
    // Parse the optional process allowlist. The returned string is reused by
    // Windhawk, so copy it before scanning.
    g_processAllowed = true;
    const wchar_t* setting = Wh_GetStringSetting(L"processes");
    if (setting && *setting) {
        const std::wstring list(setting);
        g_processAllowed = false;
        size_t start = 0;
        for (;;) {
            size_t end = list.find_first_of(L",; \t\r\n", start);
            if (end == std::wstring::npos) {
                end = list.size();
            }
            const std::wstring tok = list.substr(start, end - start);
            if (!tok.empty() && MatchesExeName(tok.c_str(), g_exeName)) {
                g_processAllowed = true;
                break;
            }
            if (end == list.size()) {
                break;
            }
            start = end + 1;
        }
    }
    Wh_Log(L"[%s] Settings: minimize=%d, close=%d, processAllowed=%d", g_exeName,
           (int)g_animateMinimize, (int)g_animateClose, (int)g_processAllowed);
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

// The cache is LRU-capped: a full-resolution capture of every minimized window
// would otherwise be held for the whole process lifetime by windows that are
// destroyed implicitly (children, thread exit, EndDialog, ...) and never pass
// through DestroyWindow_hook, and a recycled HWND could pick up a stale
// capture. Entries are also validated against the expected restore size.
static const size_t kMaxCachedCaptures = 4;

static std::mutex g_cacheMutex;

struct CacheEntry {
    CaptureBits bits;
    std::list<HWND>::iterator lruIt;
};

static std::unordered_map<HWND, CacheEntry> g_captureCache;
static std::list<HWND> g_captureLru;

static void CacheCapture(HWND hwnd, const CaptureBits& bits) {
    if (!hwnd || bits.empty()) {
        return;
    }
    try {
        std::lock_guard<std::mutex> lock(g_cacheMutex);
        auto it = g_captureCache.find(hwnd);
        if (it != g_captureCache.end()) {
            it->second.bits = bits;
            g_captureLru.splice(g_captureLru.begin(), g_captureLru, it->second.lruIt);
            return;
        }
        g_captureLru.push_front(hwnd);
        g_captureCache.emplace(hwnd, CacheEntry{bits, g_captureLru.begin()});
        while (g_captureCache.size() > kMaxCachedCaptures) {
            const HWND victim = g_captureLru.back();
            g_captureLru.pop_back();
            g_captureCache.erase(victim);
        }
    } catch (const std::exception& ex) {
        Wh_Log(L"[%s] CacheCapture: %S", g_exeName, ex.what());
    }
}

// Cheap presence check for the PlayRestore common path (the cache is almost
// always empty — foreign windows, windows minimized before the mod loaded,
// entries evicted by the LRU). Doing geometry work first and only then finding
// there is nothing to animate would waste the caller's time on every restore.
static bool HasCachedCapture(HWND hwnd) {
    if (!hwnd) {
        return false;
    }
    try {
        std::lock_guard<std::mutex> lock(g_cacheMutex);
        return g_captureCache.find(hwnd) != g_captureCache.end();
    } catch (const std::exception&) {
        return false;
    }
}

static bool TakeCachedCapture(HWND hwnd, int expectedW, int expectedH, CaptureBits& out) {
    try {
        std::lock_guard<std::mutex> lock(g_cacheMutex);
        auto it = g_captureCache.find(hwnd);
        if (it == g_captureCache.end()) {
            return false;
        }
        CaptureBits bits = std::move(it->second.bits);
        g_captureLru.erase(it->second.lruIt);
        g_captureCache.erase(it);
        if (bits.empty()) {
            return false;
        }
        // A stale entry (the window was resized while minimized, or the HWND
        // was recycled) must never be reused for the animation. The tolerance
        // covers fullscreen windows whose real size differs from the
        // work-area rect by more than the taskbar height.
        if (expectedW > 0 && expectedH > 0 &&
            (std::abs(bits.width - expectedW) > 128 || std::abs(bits.height - expectedH) > 128)) {
            Wh_Log(L"[%s] Stale capture for %p (%dx%d, expected %dx%d), dropping", g_exeName, hwnd,
                   bits.width, bits.height, expectedW, expectedH);
            return false;
        }
        out = std::move(bits);
        return true;
    } catch (const std::exception& ex) {
        Wh_Log(L"[%s] TakeCachedCapture: %S", g_exeName, ex.what());
        return false;
    }
}

static void ForgetCapture(HWND hwnd) {
    try {
        std::lock_guard<std::mutex> lock(g_cacheMutex);
        auto it = g_captureCache.find(hwnd);
        if (it != g_captureCache.end()) {
            g_captureLru.erase(it->second.lruIt);
            g_captureCache.erase(it);
        }
    } catch (const std::exception&) {
    }
}

static void ForceOpaqueAlpha(uint32_t* pixels, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        pixels[i] |= 0xFF000000u;
    }
}

// The real visible window rect: GetWindowRect on a maximized window includes
// the 8 px invisible resize borders (and can include the shadow), so a
// capture made with it is bigger and offset by 8 px from what the user sees —
// at the end of a restore the overlay then leaves the unpainted window border
// sticking out as a black rim. DWMWA_EXTENDED_FRAME_BOUNDS returns exactly
// the visible frame.
static bool GetVisibleWindowRect(HWND hwnd, RECT* rc) {
    if (hwnd && rc) {
        RECT ext{};
        if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &ext, sizeof(ext))) &&
            IsRectUsable(ext)) {
            *rc = ext;
            return true;
        }
        if (GetWindowRect(hwnd, rc)) {
            return IsRectUsable(*rc);
        }
    }
    return false;
}

static bool CaptureWindow(HWND hwnd, CaptureBits& out, bool allowScreenBlt) {
    if (!hwnd || !IsWindow(hwnd)) {
        return false;
    }
    RECT rc{};
    if (!GetVisibleWindowRect(hwnd, &rc)) {
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
    // The only blit that shows the real window content (including
    // DirectComposition content — Explorer's file list, web views — that
    // PrintWindow cannot render) is reading the screen itself. PrintWindow is
    // kept only as a fallback for windows that cannot be blitted (off-screen,
    // etc.). CreateDIBSection requires GdiFlush before the bits are read
    // directly by the CPU: without it the DIB can hold a mix of the previous
    // frame and the new blit, which showed up as the horizontal stripes this
    // function used to work around with a read-compare-Sleep loop.
    if (allowScreenBlt) {
        painted = BitBlt(memDc.get(), 0, 0, width, height, winDc.get(), rc.left, rc.top,
                         SRCCOPY) != FALSE;
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
    // Required before reading the DIB section bits directly (CPU access).
    GdiFlush();

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
// Geometry. All of these run inside ScopedDpiAware (see PlayMinimize /
// PlayRestore), so every rect is in physical screen coordinates — the same
// space DWM's DWMWA_EXTENDED_FRAME_BOUNDS and the screen capture DC use.
// No manual DPI conversion is needed anywhere.
// ---------------------------------------------------------------------------

static bool GetMinimizeRectPhysical(HWND hwnd, RECT* rc) {
    if (pGetWindowMinimizeRect && pGetWindowMinimizeRect(hwnd, rc) && IsRectUsable(*rc)) {
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
    // The hwnd can belong to another process (taskbar button / "restore all"
    // in explorer.exe). SendMessageW would block the caller's UI thread
    // indefinitely if that process is hung, so use a bounded timeout and bail
    // out instead.
    if (!SendMessageTimeoutW(hwnd, WM_GETMINMAXINFO, 0, reinterpret_cast<LPARAM>(&mmi),
                             SMTO_ABORTIFHUNG, 100, nullptr)) {
        return false;
    }
    if (mmi.ptMaxSize.x <= 0 || mmi.ptMaxSize.y <= 0) {
        return false;
    }
    rc->left = mmi.ptMaxPosition.x;
    rc->top = mmi.ptMaxPosition.y;
    rc->right = mmi.ptMaxPosition.x + mmi.ptMaxSize.x;
    rc->bottom = mmi.ptMaxPosition.y + mmi.ptMaxSize.y;
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
// Overlay geometry + software textured-quad rasterizer.
// Every routine takes the *allocated* stride of the destination buffer
// (dstStride >= dw). The destination DIB is reused across frames and grows
// only when needed, so it can be wider than the current frame; using dw as
// the stride mixed written rows with presented rows and left the bands that
// showed up as horizontal lines in the animation.
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
                           int dstStride,
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
        uint32_t* row = dst + static_cast<size_t>(y) * static_cast<size_t>(dstStride);
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
                       int dstStride,
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
    RasterTriangle(dst, dstStride, dw, dh, src, sw, sh, corners[0], corners[1], corners[2], alpha);
    RasterTriangle(dst, dstStride, dw, dh, src, sw, sh, corners[0], corners[2], corners[3], alpha);
}

// ---------------------------------------------------------------------------
// Per-animation rendering objects.
//
// The capture never changes during an animation, so the source DIB is created
// once per AnimRequest, and the destination DIB is grown to the largest bbox
// the animation needs and then reused. Everything that touches the pixels
// (memset, alpha pass, rasterizer) uses gdi.dstStride() — the allocated width
// — never the current frame width, so written rows always match the rows
// UpdateLayeredWindow presents.
// ---------------------------------------------------------------------------

class PresentGdi {
   public:
    PresentGdi() = default;
    PresentGdi(const PresentGdi&) = delete;
    PresentGdi& operator=(const PresentGdi&) = delete;
    ~PresentGdi() { Release(); }

    bool EnsureSource(const CaptureBits& cap) {
        if (m_hdcSrc) {
            return true;
        }
        if (!EnsureScreenDc()) {
            return false;
        }
        BITMAPINFO bmi{};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = cap.width;
        bmi.bmiHeader.biHeight = -cap.height;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        m_hbmSrc = CreateDIBSection(m_hdcScreen, &bmi, DIB_RGB_COLORS, &m_pvSrc, nullptr, 0);
        if (!m_hbmSrc || !m_pvSrc) {
            Release();
            return false;
        }
        std::memcpy(m_pvSrc, cap.pixels.data(), cap.pixels.size() * sizeof(uint32_t));
        // The bits were written directly by the CPU; flush before GDI reads
        // them through StretchBlt so it does not use a stale cached copy.
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
        if (m_hdcDst && dw <= m_dstW && dh <= m_dstH) {
            return true;
        }
        if (m_hdcDst) {
            DeleteDC(m_hdcDst);  // deselects m_hbmDst
            m_hdcDst = nullptr;
        }
        if (m_hbmDst) {
            DeleteObject(m_hbmDst);
            m_hbmDst = nullptr;
            m_pvDst = nullptr;
        }
        if (!EnsureScreenDc()) {
            return false;
        }
        BITMAPINFO bmi{};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = dw;
        bmi.bmiHeader.biHeight = -dh;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        m_hbmDst = CreateDIBSection(m_hdcScreen, &bmi, DIB_RGB_COLORS, &m_pvDst, nullptr, 0);
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
            DeleteDC(m_hdcDst);  // deselects m_hbmDst
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
            ReleaseDC(nullptr, m_hdcScreen);
            m_hdcScreen = nullptr;
        }
    }

    HDC hdcScreen() const { return m_hdcScreen; }
    HDC hdcDst() const { return m_hdcDst; }
    HDC hdcSrc() const { return m_hdcSrc; }
    void* dstBits() const { return m_pvDst; }
    int dstStride() const { return m_dstW; }

   private:
    bool EnsureScreenDc() {
        if (m_hdcScreen) {
            return true;
        }
        m_hdcScreen = GetDC(nullptr);
        return m_hdcScreen != nullptr;
    }

    HDC m_hdcScreen = nullptr;
    HDC m_hdcSrc = nullptr;
    HDC m_hdcDst = nullptr;
    HGDIOBJ m_hbmSrc = nullptr;
    HGDIOBJ m_hbmDst = nullptr;
    void* m_pvSrc = nullptr;
    void* m_pvDst = nullptr;
    int m_dstW = 0;
    int m_dstH = 0;
};

struct AnimRequest {
    HWND hwnd = nullptr;
    AnimationType type = AnimationType::None;
    RECT rcWindow{};
    RECT rcDest{};
    CaptureBits capture;
    UINT durationMs = 250;
    bool deferOrig = false;   // restore/close: orig() runs after the animation
    int origShowCmd = 0;
    std::unique_ptr<PresentGdi> gdi;  // created once, reused for every frame
};

// ---------------------------------------------------------------------------
// Overlay present
// ---------------------------------------------------------------------------

static bool PresentOverlay(HWND hwndOverlay,
                           PresentGdi& gdi,
                           const AnimRequest& req,
                           const RECT& rcCurrent,
                           const Win7TransformParams& params) {
    const BYTE alpha = static_cast<BYTE>(std::clamp(params.opacity, 0.0f, 1.0f) * 255.0f + 0.5f);
    if (alpha == 0 || !IsRectUsable(rcCurrent)) {
        POINT pt{rcCurrent.left, rcCurrent.top};
        SIZE sz{1, 1};
        BLENDFUNCTION bf{AC_SRC_OVER, 0, 0, AC_SRC_ALPHA};
        UpdateLayeredWindow(hwndOverlay, nullptr, &pt, &sz, nullptr, nullptr, 0, &bf, ULW_ALPHA);
        return true;
    }

    const CaptureBits& cap = req.capture;
    const float originalWidth = static_cast<float>(cap.width);
    const float originalHeight = static_cast<float>(cap.height);

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

    if (!gdi.EnsureSource(cap)) {
        return false;
    }
    if (!gdi.EnsureDest(dw, dh)) {
        return false;
    }

    void* bits = gdi.dstBits();
    const int stride = gdi.dstStride();  // allocated width, may be > dw

    // Clear every row the presented dw x dh area occupies (with the real
    // stride), so nothing stale can show through.
    std::memset(bits, 0, static_cast<size_t>(stride) * static_cast<size_t>(dh) * 4);

    if (tiny3d) {
        StretchBlt(gdi.hdcDst(), 0, 0, dw, dh, gdi.hdcSrc(), 0, 0, cap.width, cap.height, SRCCOPY);
        // GDI may batch the blit; flush before the per-pixel loop reads the
        // DIB bits directly, or the alpha pass would read a stale surface.
        GdiFlush();
        auto* px = static_cast<uint32_t*>(bits);
        for (int y = 0; y < dh; ++y) {
            uint32_t* row = px + static_cast<size_t>(y) * static_cast<size_t>(stride);
            for (int x = 0; x < dw; ++x) {
                const uint32_t p = row[x];
                const BYTE b = static_cast<BYTE>(((p & 0xFF) * alpha) / 255);
                const BYTE g = static_cast<BYTE>((((p >> 8) & 0xFF) * alpha) / 255);
                const BYTE r = static_cast<BYTE>((((p >> 16) & 0xFF) * alpha) / 255);
                row[x] = static_cast<uint32_t>(b) | (static_cast<uint32_t>(g) << 8) |
                         (static_cast<uint32_t>(r) << 16) | (static_cast<uint32_t>(alpha) << 24);
            }
        }
    } else {
        RasterQuad(static_cast<uint32_t*>(bits), stride, dw, dh, cap.pixels.data(), cap.width,
                   cap.height, corners, alpha);
    }

    POINT pt{bbox.left, bbox.top};
    SIZE sz{dw, dh};
    POINT srcPt{0, 0};
    BLENDFUNCTION bf{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    if (!UpdateLayeredWindow(hwndOverlay, gdi.hdcScreen(), &pt, &sz, gdi.hdcDst(), &srcPt, 0, &bf,
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

enum class AnimMsg : UINT {
    FirstFrame = 1,  // lParam = AnimRequest* (caller-owned, sync)
    Drain = 2,       // pop the queue
    Hide = 3,
};

// Shared state. Every window of every Explorer process runs on its own UI
// thread and all of them can reach these hooks at the same time (and the
// animation thread reads and writes them too), so none of this is plain
// global state. "Am I allowed to start" is decided by a single
// compare_exchange in BeginAnimation, never by check-then-set.
static std::atomic<UINT> g_msgAnim{0};
static std::atomic<bool> g_fAnimating{false};
static std::atomic<bool> g_fDisabled{false};
static std::atomic<HWND> g_hwndAnim{nullptr};
static std::atomic<HWND> g_hwndCurrent{nullptr};
static std::atomic<int> g_typeCurrent{static_cast<int>(AnimationType::None)};

// Set while the mod is unloading. RunAnimation / DrainQueue bail out on it.
static std::atomic<bool> g_stopping{false};
static HINSTANCE g_hinst = nullptr;

// Guarded by g_animThreadMutex: at most one animation window thread exists.
static std::mutex g_animThreadMutex;
static HANDLE g_hAnimWndThread = nullptr;
static DWORD g_dwAnimThreadId = 0;

static std::mutex g_queueMutex;
static std::deque<AnimRequest*> g_queue;

static void PresentTime(HWND hwndOverlay, AnimRequest& req, float t) {
    const Win7TransformParams params = ParamsFor(req.type, t);
    const RECT rc = RectFor(req.type, t, req.rcWindow, req.rcDest);
    PresentOverlay(hwndOverlay, *req.gdi, req, rc, params);
}

static void PresentFirstFrame(HWND hwndOverlay, AnimRequest& req) {
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

static void RunAnimation(HWND hwndOverlay, AnimRequest& req) {
    if (req.capture.empty() || !IsRectUsable(req.rcWindow)) {
        return;
    }
    // Unload in progress: do not even show the overlay.
    if (g_stopping.load()) {
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
    // g_hwndCurrent is cleared by AfterOrigMinimize when a synchronous
    // minimize was refused by the app; that is the cancellation signal, so
    // this request stops presenting and FinishQueued drops the overlay.
    while (!g_stopping.load() && g_hwndCurrent.load() == req.hwnd &&
           (elapsed = GetTickCount64() - start) <= durationMs) {
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
    if (!g_stopping.load()) {
        PresentTime(hwndOverlay, req, 1.0f);
    }
}

static void FinishQueued(AnimRequest* req) {
    if (!req) {
        return;
    }
    // Restore: the window stayed iconic the whole time. Show it under the last
    // overlay frame, then drop the overlay. No cloak, so the client is intact.
    // While the mod is unloading, Wh_ModBeforeUninit already restored the
    // queued requests; this handles the in-flight one, and the hooks are still
    // installed there, so ShowWindowAsync_orig is valid.
    if (req->deferOrig && req->hwnd && IsWindow(req->hwnd) && ShowWindowAsync_orig) {
        const int cmd = req->origShowCmd ? req->origShowCmd : SW_RESTORE;
        // This runs on the animation thread, which the mod unload joins with
        // an INFINITE wait. The window belongs to another thread (often
        // another process), so a synchronous ShowWindow could block forever on
        // a busy owner and hang the unload. ShowWindowAsync only posts the
        // request and never blocks.
        ShowWindowAsync_orig(req->hwnd, cmd);
        // Only draw the final overlay frame when the mod is not stopping: it
        // is a best-effort cover so no black rim is visible while the just-
        // restored window paints. During unload the window paints on its own.
        if (!g_stopping.load()) {
            // ShowWindowAsync only *posts* the restore to the window's owning
            // thread; it can take one or more frames for that thread to
            // actually clear WS_MINIMIZE and repaint. Hiding the overlay
            // right away used to race that repaint, exposing an unpainted /
            // stale DWM surface for a frame (the reported flash). Wait here,
            // with a hard timeout so a stuck/foreign thread can never hang
            // the animation thread, until the window has actually come out
            // of the iconic state (or until we give up and hide anyway).
            const ULONGLONG waitStart = GetTickCount64();
            constexpr ULONGLONG kMaxWaitMs = 120;  // ~a few frames, never more
            while (!g_stopping.load() && IsWindow(req->hwnd) && IsIconic(req->hwnd) &&
                   (GetTickCount64() - waitStart) < kMaxWaitMs) {
                Sleep(1);
            }
            HWND hwndAnim = g_hwndAnim.load();
            RECT rcNow{};
            if (hwndAnim && IsWindow(hwndAnim) && GetVisibleWindowRect(req->hwnd, &rcNow) &&
                IsRectUsable(rcNow) && req->gdi) {
                Win7TransformParams p;  // t = 1 for restore: opacity 1, no tilt
                p.opacity = 1.0f;
                PresentOverlay(hwndAnim, *req->gdi, *req, rcNow, p);
            }
            // Give the DWM redirection surface one more tick to actually
            // present the restored content before the overlay (its cover)
            // goes away. Cheap, and it is the difference between a clean
            // hand-off and a one-frame flash.
            if (!g_stopping.load()) {
                Sleep(1);
            }
        }
    }
    HWND hwndAnim = g_hwndAnim.load();
    if (hwndAnim && IsWindow(hwndAnim)) {
        HideOverlayWindow(hwndAnim);
    }
    // Re-enable DWM transitions only *after* the overlay is gone: doing it
    // before risked DWM starting its own transition on the real window while
    // the overlay was still covering it, i.e. two animations stacked on top
    // of each other.
    if (req->hwnd && IsWindow(req->hwnd)) {
        DisableTransitions(req->hwnd, FALSE);
    }
    HWND cur = g_hwndCurrent.load();
    if (cur == req->hwnd) {
        g_hwndCurrent.store(nullptr);
        g_typeCurrent.store(static_cast<int>(AnimationType::None));
        g_fAnimating.store(false);
    }
    delete req;
}

static void DrainQueue(HWND hwndOverlay) {
    for (;;) {
        if (g_stopping.load()) {
            break;
        }
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
    const UINT msgAnim = g_msgAnim.load();
    if (msgAnim && uMsg == msgAnim) {
        switch (static_cast<AnimMsg>(wParam)) {
            case AnimMsg::FirstFrame:
                if (lParam) {
                    PresentFirstFrame(hwnd, *reinterpret_cast<AnimRequest*>(lParam));
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
    // A stale registration (left over from an instance that failed to
    // unregister, or from a crash) points its lpfnWndProc into a previous,
    // now-unmapped mod image. Never create the window against it: fail the
    // thread cleanly instead.
    if (!RegisterClassW(&wc)) {
        Wh_Log(L"[%s] RegisterClassW failed: %u", g_exeName, GetLastError());
        SetEvent(hEvent);
        return 0;
    }
    HWND hwnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kAnimClassName,
                                nullptr, WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, g_hinst, nullptr);
    if (!hwnd) {
        Wh_Log(L"[%s] CreateWindowExW failed: %u", g_exeName, GetLastError());
        UnregisterClassW(kAnimClassName, g_hinst);
        SetEvent(hEvent);
        return 0;
    }
    const LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    SetWindowLongPtrW(hwnd, GWL_EXSTYLE, ex | WS_EX_LAYERED | WS_EX_TRANSPARENT);
    g_hwndAnim.store(hwnd);
    g_msgAnim.store(RegisterWindowMessageW(L"Windhawk_Win7AeroAnim_Run"));
    // The thread never touches hEvent again after this point.
    SetEvent(hEvent);
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    g_hwndAnim.store(nullptr);
    return 0;
}

static bool WaitForAnimWndThread() {
    // Unloading: never create a thread whose code lives in an image that is
    // about to be unmapped.
    if (g_stopping.load()) {
        return false;
    }
    // Every Explorer window runs on its own UI thread and every one of them
    // can reach this at the same time: without the mutex two threads would
    // both CreateThread and leak the winner's handle (and Wh_ModUninit would
    // then wait on the wrong thread).
    std::lock_guard<std::mutex> lock(g_animThreadMutex);
    if (g_stopping.load()) {
        return false;
    }
    if (g_hAnimWndThread) {
        HWND hwndAnim = g_hwndAnim.load();
        if (hwndAnim && IsWindow(hwndAnim)) {
            return true;
        }
        // The overlay window is gone but the thread is still alive: stop and
        // wait for it so exactly one thread ever exists.
        if (g_dwAnimThreadId) {
            PostThreadMessageW(g_dwAnimThreadId, WM_QUIT, 0, 0);
        }
        WaitForSingleObject(g_hAnimWndThread, INFINITE);
        CloseHandle(g_hAnimWndThread);
        g_hAnimWndThread = nullptr;
        g_dwAnimThreadId = 0;
    }
    // The event is only closed on the success path. If the 1 s wait times out,
    // the new thread may still be about to call SetEvent on it: closing it
    // there would be a use of a closed (possibly recycled) handle.
    HANDLE hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!hEvent) {
        return false;
    }
    g_hAnimWndThread = CreateThread(nullptr, 0, AnimWndThreadProc, hEvent, 0, nullptr);
    if (!g_hAnimWndThread) {
        CloseHandle(hEvent);
        return false;
    }
    g_dwAnimThreadId = GetThreadId(g_hAnimWndThread);
    const DWORD wait = WaitForSingleObject(hEvent, 1000);
    if (wait == WAIT_OBJECT_0) {
        // The thread is done with the event (see AnimWndThreadProc).
        CloseHandle(hEvent);
    } else {
        // Timeout: leave the handle open and disable the mod quietly. A modal
        // MessageBoxW here would sit inside a window-message hook and block
        // (and re-enter) an arbitrary host UI thread.
        Wh_Log(L"[%s] Overlay window was not created within 1s, disabling", g_exeName);
        g_fDisabled.store(true);
        return false;
    }
    if (!g_hwndAnim.load()) {
        g_fDisabled.store(true);
        return false;
    }
    // The event already signalled that the overlay window exists; there is
    // nothing left to wait for, so return immediately instead of sleeping on
    // the caller's UI thread.
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
    HWND hwndAnim = g_hwndAnim.load();
    UINT msgAnim = g_msgAnim.load();
    if (!hwndAnim || !msgAnim || !IsWindow(hwndAnim) ||
        !PostMessageW(hwndAnim, msgAnim, static_cast<WPARAM>(AnimMsg::Drain), 0)) {
        Wh_Log(L"[%s] PostMessage Drain failed: %u", g_exeName, GetLastError());
        std::lock_guard<std::mutex> lock(g_queueMutex);
        if (!g_queue.empty() && g_queue.back() == heap) {
            g_queue.pop_back();
        }
        // Honoring the deferred restore here is what keeps the window from
        // staying minimized when the overlay can never play. Close requests
        // are handled by their PlayClose* caller instead.
        if (heap->type != AnimationType::Close && heap->deferOrig && heap->hwnd &&
            IsWindow(heap->hwnd) && ShowWindow_orig) {
            ShowWindow_orig(heap->hwnd, heap->origShowCmd ? heap->origShowCmd : SW_RESTORE);
        }
        delete heap;
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------
// Who gets an animation
// ---------------------------------------------------------------------------

static bool IsAnimateCandidate(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) {
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
    if (hwnd == g_hwndAnim.load()) {
        return false;
    }
    return true;
}

static bool ShouldAnimateWindow(HWND hwnd) {
    if (g_fDisabled.load() || g_fAnimating.load() || !g_processAllowed) {
        return false;
    }
    if (!IsAnimateCandidate(hwnd)) {
        return false;
    }
    if (!IsWindowVisible(hwnd) || IsIconic(hwnd)) {
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
    // The "may I start" decision is one atomic compare-exchange. A
    // check-then-set would let two window threads (each window lives on its
    // own thread) start two animations at once, or would let a stale
    // g_fAnimating == false read block a UI thread in the synchronous
    // FirstFrame send for the whole duration of the in-flight animation.
    bool expected = false;
    if (!g_fAnimating.compare_exchange_strong(expected, true)) {
        return false;
    }
    auto cleanupFailedStart = [hwnd]() {
        if (g_hwndCurrent.load() == hwnd) {
            g_hwndCurrent.store(nullptr);
            g_typeCurrent.store(static_cast<int>(AnimationType::None));
        }
        g_fAnimating.store(false);
    };
    if (!hwnd || !IsWindow(hwnd) || capture.empty() || !WaitForAnimWndThread()) {
        cleanupFailedStart();
        return false;
    }
    if (g_stopping.load()) {
        cleanupFailedStart();
        return false;
    }

    AnimRequest req;
    req.hwnd = hwnd;
    req.type = type;
    req.rcWindow = rcWindow;
    req.rcDest = rcDest;
    req.capture = std::move(capture);
    req.durationMs = DurationMsFor(type);
    req.deferOrig = deferOrig;
    req.origShowCmd = origShowCmd;
    try {
        req.gdi = std::make_unique<PresentGdi>();
    } catch (const std::exception& ex) {
        Wh_Log(L"[%s] BeginAnimation alloc: %S", g_exeName, ex.what());
        cleanupFailedStart();
        return false;
    }

    g_hwndCurrent.store(hwnd);
    g_typeCurrent.store(static_cast<int>(type));

    // Minimize must have its first frame presented *before* orig() runs on
    // the caller's thread, otherwise the window disappears with nothing to
    // cover it, so that send has to be synchronous (the request is still in
    // scope, so the raw pointer stays valid). A deferred (restore) animation
    // keeps the window iconic and RunAnimation presents from t=0 on the
    // overlay thread anyway, so no first frame is needed here and the
    // caller's UI thread never blocks on the overlay.
    if (!deferOrig) {
        SendMessageW(g_hwndAnim.load(), g_msgAnim.load(),
                     static_cast<WPARAM>(AnimMsg::FirstFrame),
                     reinterpret_cast<LPARAM>(&req));
    }

    DisableTransitions(hwnd, TRUE);

    if (!QueueRun(std::move(req))) {
        DisableTransitions(hwnd, FALSE);
        HWND hwndAnim = g_hwndAnim.load();
        if (hwndAnim && IsWindow(hwndAnim)) {
            HideOverlayWindow(hwndAnim);
        }
        cleanupFailedStart();
        return false;
    }
    return true;
}

static void StopAnimThread() {
    std::lock_guard<std::mutex> lock(g_animThreadMutex);
    if (!g_hAnimWndThread) {
        return;
    }
    HWND hwndAnim = g_hwndAnim.load();
    if (hwndAnim && IsWindow(hwndAnim)) {
        // WM_CLOSE → DefWindowProc → DestroyWindow → WM_DESTROY →
        // PostQuitMessage. Processed while the thread pumps, right after
        // RunAnimation bailed out on g_stopping.
        SendMessageW(hwndAnim, WM_CLOSE, 0, 0);
    }
    if (g_dwAnimThreadId) {
        // In case the overlay window was already gone, or never existed.
        PostThreadMessageW(g_dwAnimThreadId, WM_QUIT, 0, 0);
    }
    // The thread checks g_stopping at least once per frame (≈1 ms) and only
    // touches GDI/kernel objects, so this cannot take long. Still, the wait is
    // unconditional: returning while the thread is inside the mod image is
    // what crashed the host process when Windhawk FreeLibrary'd it right
    // after Wh_ModUninit returned.
    WaitForSingleObject(g_hAnimWndThread, INFINITE);
    CloseHandle(g_hAnimWndThread);
    g_hAnimWndThread = nullptr;
    g_dwAnimThreadId = 0;
}

// async is true only for the ShowWindowAsync path: there the minimize is
// posted to the window's thread and simply may not have been applied yet, so
// a non-iconic check is meaningless and must not cancel the animation.
static void AfterOrigMinimize(HWND hwnd, bool async) {
    if (!hwnd || !IsWindow(hwnd)) {
        return;
    }
    if (!g_fAnimating.load() || g_hwndCurrent.load() != hwnd) {
        return;
    }
    if (g_typeCurrent.load() != static_cast<int>(AnimationType::Minimize)) {
        return;
    }
    DisableTransitions(hwnd, TRUE);
    if (!async && !IsIconic(hwnd)) {
        // A synchronous minimize that did not stick was refused by the app
        // (its own WM_SYSCOMMAND handling, a WM_WINDOWPOSCHANGING handler, a
        // modal state, ...). Never override that with SW_FORCEMINIMIZE. Cancel
        // the animation instead: clearing g_hwndCurrent stops RunAnimation and
        // FinishQueued drops the overlay, leaving the window exactly as the
        // app wants it.
        Wh_Log(L"[%s] Minimize refused by app, cancelling animation hwnd=%p", g_exeName, hwnd);
        g_hwndCurrent.store(nullptr);
        g_typeCurrent.store(static_cast<int>(AnimationType::None));
        g_fAnimating.store(false);
        HWND hwndAnim = g_hwndAnim.load();
        if (hwndAnim && IsWindow(hwndAnim)) {
            HideOverlayWindow(hwndAnim);
        }
    }
    // async: let the posted minimize apply on the window's own thread. No
    // force, no cancel here.
}

static bool PlayMinimize(HWND hwnd) {
    if (!g_animateMinimize || !ShouldAnimateWindow(hwnd)) {
        return false;
    }
    const LONG style = static_cast<LONG>(GetWindowLongPtrW(hwnd, GWL_STYLE));
    if (style & WS_MINIMIZE) {
        return false;
    }
    // Geometry and the screen capture must run in the same (physical) DPI
    // coordinate space that DWMWA_EXTENDED_FRAME_BOUNDS uses; otherwise a
    // DPI-unaware caller would mix virtualized and physical rects and start
    // the animation in the wrong place. ScopedDpiAware makes this thread
    // per-monitor aware for the block, so every rect here is physical.
    RECT rcWindow{};
    RECT rcMin{};
    CaptureBits cap;
    {
        ScopedDpiAware dpi;
        if (!GetVisibleWindowRect(hwnd, &rcWindow)) {
            return false;
        }
        if (!GetMinimizeRectPhysical(hwnd, &rcMin)) {
            Wh_Log(L"[%s] GetWindowMinimizeRect failed", g_exeName);
            return false;
        }
        rcMin = AspectCorrectedMinimizeTarget(rcMin);
        if (!CaptureWindow(hwnd, cap, /*allowScreenBlt=*/true)) {
            Wh_Log(L"[%s] CaptureWindow failed on minimize", g_exeName);
            return false;
        }
    }
    CacheCapture(hwnd, cap);
    Wh_Log(L"[%s] Minimize hwnd=%p src=(%d,%d)-(%d,%d) dest=(%d,%d)-(%d,%d)", g_exeName, hwnd,
           rcWindow.left, rcWindow.top, rcWindow.right, rcWindow.bottom, rcMin.left, rcMin.top,
           rcMin.right, rcMin.bottom);
    return BeginAnimation(hwnd, AnimationType::Minimize, rcWindow, rcMin, std::move(cap),
                          /*deferOrig=*/false, 0);
}

static bool PlayRestore(HWND hwnd) {
    if (!g_animateMinimize || !g_processAllowed) {
        return false;
    }
    const LONG style = static_cast<LONG>(GetWindowLongPtrW(hwnd, GWL_STYLE));
    if (style & WS_MINIMIZE) {
        if (g_fDisabled.load() || g_fAnimating.load()) {
            return false;
        }
        // Common path first: the cached capture is almost always absent
        // (foreign windows, windows minimized before the mod loaded, entries
        // evicted by the LRU), so bail before doing any geometry work.
        if (!HasCachedCapture(hwnd)) {
            return false;
        }
        // Same DPI reasoning as PlayMinimize: all geometry is physical.
        RECT rcMin{};
        RECT rcRest{};
        bool haveRest = false;
        {
            ScopedDpiAware dpi;
            if (!GetMinimizeRectPhysical(hwnd, &rcMin)) {
                return false;
            }
            rcMin = AspectCorrectedMinimizeTarget(rcMin);
            WINDOWPLACEMENT wp{sizeof(wp)};
            if (GetWindowPlacement(hwnd, &wp) && (wp.flags & WPF_RESTORETOMAXIMIZED)) {
                haveRest = GetMaximizeRectPhysical(hwnd, &rcRest);
            } else {
                haveRest = GetRestoreRectPhysical(hwnd, &rcRest);
            }
        }
        if (!haveRest || !IsRectUsable(rcRest)) {
            return false;
        }
        CaptureBits cap;
        if (!TakeCachedCapture(hwnd, RECTW(rcRest), RECTH(rcRest), cap)) {
            Wh_Log(L"[%s] No cached capture for restore, skipping", g_exeName);
            return false;
        }
        // Fullscreen / maximized apps (games, players) occupy their whole
        // monitor, while the math above can report the smaller work-area
        // rect. Ending the overlay short of the real window lets the
        // unpainted rim of the restored window stick out as black borders
        // (right/bottom in the recording), and the final swap jumps. The
        // capture knows the window's real on-screen size at minimize time:
        // end the overlay exactly there.
        if (cap.width > 0 && cap.height > 0 &&
            (RECTW(rcRest) != cap.width || RECTH(rcRest) != cap.height)) {
            Wh_Log(L"[%s] Restore target corrected to capture size %dx%d (was %dx%d)", g_exeName,
                   cap.width, cap.height, RECTW(rcRest), RECTH(rcRest));
            rcRest.right = rcRest.left + cap.width;
            rcRest.bottom = rcRest.top + cap.height;
        }
        return BeginAnimation(hwnd, AnimationType::RestoreFromMinimized, rcRest, rcMin, std::move(cap),
                              /*deferOrig=*/true, SW_RESTORE);
    }
    // Maximized windows are not animated on restore: the resize animation was
    // removed (too unstable); Windows restores them normally.
    return false;
}

// Close: unlike minimize/restore, this does NOT hand off to the shared
// animation thread. DestroyWindow (unlike ShowWindow/ShowWindowAsync) must be
// called from the thread that owns the window, and DestroyWindow_hook already
// runs on that thread (it's whoever the caller is). So the whole thing —
// show the overlay, fade it, hide it, then really destroy the window — runs
// right here, synchronously, blocking the caller for the fade's duration.
// The overlay HWND itself is safe to drive from any thread (ShowWindow,
// SetWindowPos, UpdateLayeredWindow are not restricted to the owning thread
// the way window creation/destruction is), so this is just reusing our
// existing overlay window from a different caller thread.
// Returns true if the real destroy has ALREADY happened (caller must not
// call DestroyWindow_orig again); false if nothing was done (caller should
// proceed normally).
static bool PlayClose(HWND hwnd) {
    if (!g_animateClose || !ShouldAnimateWindow(hwnd)) {
        return false;
    }
    const LONG style = static_cast<LONG>(GetWindowLongPtrW(hwnd, GWL_STYLE));
    if (style & WS_MINIMIZE) {
        return false;  // already iconic, nothing to fade
    }
    if (GetWindow(hwnd, GW_OWNER)) {
        // Owned windows (dialogs, message boxes, ...) are frequently closed
        // from inside a modal loop the caller is actively driving; blocking
        // that loop for the fade is more likely to look like a hitch than an
        // effect. Stay conservative and only animate top-level, unowned
        // windows.
        return false;
    }

    // Single shared overlay: never let a close fade collide with an
    // in-progress minimize/restore (or another close) touching the same
    // overlay HWND from two threads at once.
    bool expected = false;
    if (!g_fAnimating.compare_exchange_strong(expected, true)) {
        return false;
    }
    auto release = [&]() { g_fAnimating.store(false); };

    if (!WaitForAnimWndThread() || g_stopping.load()) {
        release();
        return false;
    }

    RECT rcWindow{};
    CaptureBits cap;
    {
        ScopedDpiAware dpi;
        if (!GetVisibleWindowRect(hwnd, &rcWindow) ||
            !CaptureWindow(hwnd, cap, /*allowScreenBlt=*/true)) {
            release();
            return false;
        }
    }

    AnimRequest req;  // stack-local: never queued, never touches g_queue
    req.hwnd = hwnd;
    req.type = AnimationType::Close;
    req.rcWindow = rcWindow;
    req.rcDest = rcWindow;
    req.capture = std::move(cap);
    req.durationMs = DurationMsFor(AnimationType::Close);
    try {
        req.gdi = std::make_unique<PresentGdi>();
    } catch (const std::exception& ex) {
        Wh_Log(L"[%s] PlayClose alloc: %S", g_exeName, ex.what());
        release();
        return false;
    }

    Wh_Log(L"[%s] Close hwnd=%p rc=(%d,%d)-(%d,%d)", g_exeName, hwnd, rcWindow.left, rcWindow.top,
           rcWindow.right, rcWindow.bottom);

    HWND hwndAnim = g_hwndAnim.load();
    if (!hwndAnim || !IsWindow(hwndAnim)) {
        release();
        return false;
    }
    if (ShowWindow_orig) {
        ShowWindow_orig(hwndAnim, SW_SHOWNA);
    } else {
        ::ShowWindow(hwndAnim, SW_SHOWNA);
    }

    const ULONGLONG start = GetTickCount64();
    float lastT = -1.0f;
    ULONGLONG elapsed = 0;
    while (!g_stopping.load() &&
           (elapsed = GetTickCount64() - start) <= req.durationMs) {
        const float t = req.durationMs == 0
                            ? 1.0f
                            : std::clamp(static_cast<float>(elapsed) / static_cast<float>(req.durationMs),
                                         0.0f, 1.0f);
        if (t - lastT >= 0.001f || elapsed + 8 >= req.durationMs) {
            lastT = t;
            const Win7TransformParams params = ParamsFor(req.type, t);
            const RECT rc = RectFor(req.type, t, rcWindow, rcWindow);
            PresentOverlay(hwndAnim, *req.gdi, req, rc, params);
        }
        Sleep(1);
    }

    HideOverlayWindow(hwndAnim);
    release();

    // The real close, finally — on the correct (this) thread.
    if (IsWindow(hwnd) && DestroyWindow_orig) {
        DestroyWindow_orig(hwnd);
    }
    return true;
}


// ---------------------------------------------------------------------------
// Hooks
// ---------------------------------------------------------------------------
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
                AfterOrigMinimize(hWnd, /*async=*/false);                  \
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
    if (g_fDisabled.load() || g_fAnimating.load()) {
        return ShowWindow_orig(hWnd, nCmdShow);
    }
    const UINT cmd = CmdFromShow(nCmdShow);
    if (cmd == SC_RESTORE && PlayRestore(hWnd)) {
        return TRUE;  // orig() runs after the overlay, in FinishQueued
    }
    bool playedMin = false;
    if (cmd == SC_MINIMIZE) {
        playedMin = PlayMinimize(hWnd);
    }
    const BOOL r = ShowWindow_orig(hWnd, nCmdShow);
    if (playedMin) {
        AfterOrigMinimize(hWnd, /*async=*/false);
    }
    return r;
}

// AGGIUNGI QUI la dichiarazione e la funzione hook per ShowWindowAsync
using ShowWindowAsync_t = decltype(&ShowWindowAsync);

BOOL WINAPI ShowWindowAsync_hook(HWND hWnd, int nCmdShow) {
    if (g_fDisabled.load() || g_fAnimating.load()) {
        return ShowWindowAsync_orig(hWnd, nCmdShow);
    }
    const UINT cmd = CmdFromShow(nCmdShow);
    if (cmd == SC_RESTORE && PlayRestore(hWnd)) {
        return TRUE;
    }
    bool playedMin = false;
    if (cmd == SC_MINIMIZE) {
        playedMin = PlayMinimize(hWnd);
    }
    const BOOL r = ShowWindowAsync_orig(hWnd, nCmdShow);
    if (playedMin) {
        AfterOrigMinimize(hWnd, /*async=*/true);
    }
    return r;
}

using DestroyWindow_t = decltype(&DestroyWindow);
BOOL WINAPI DestroyWindow_hook(HWND hWnd) {
    ForgetCapture(hWnd);
    if (!g_fDisabled.load() && !g_stopping.load() && PlayClose(hWnd)) {
        // PlayClose already ran the fade and called the real
        // DestroyWindow_orig itself (synchronously, on this thread — the
        // only thread allowed to destroy this window), so there is nothing
        // left to do here.
        return TRUE;
    }
    return DestroyWindow_orig(hWnd);
}

// ---------------------------------------------------------------------------
// Entry points
// ---------------------------------------------------------------------------
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

// Raw user32 ShowWindow, used only by the defensive drain in Wh_ModUninit
// where the Windhawk trampolines may already be removed.
typedef BOOL(WINAPI* ShowWindowRaw_t)(HWND, int);
static ShowWindowRaw_t pShowWindowRaw = nullptr;

BOOL Wh_ModInit() {
    InitExeName();
    Wh_Log(L"[%s] Init", g_exeName);
    // Set before any hook: the animation thread needs it, and each window of
    // this process can reach the hooks on its own thread.
    g_hinst = GetCurrentModule();
    LoadSettings();

    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32) {
        Wh_Log(L"[%s] user32.dll is not loaded", g_exeName);
        return FALSE;
    }
    pShowWindowRaw = reinterpret_cast<ShowWindowRaw_t>(GetProcAddress(user32, "ShowWindow"));
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

    Wh_Log(L"[%s] Hooks installed", g_exeName);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"[%s] Settings changed", g_exeName);
    LoadSettings();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"[%s] BeforeUninit", g_exeName);
    g_fDisabled.store(true);
    g_stopping.store(true);
    // The hooks are still installed here (they are only removed after this
    // function returns), so ShowWindow_orig and every trampoline are valid.
    // Honor the deferred restore/open of every queued request before deleting
    // it: dropping it would leave the window minimized forever.
    {
        std::lock_guard<std::mutex> lock(g_queueMutex);
        while (!g_queue.empty()) {
            AnimRequest* req = g_queue.front();
            g_queue.pop_front();
            if (!req) {
                continue;
            }
            if (req->deferOrig && req->hwnd && IsWindow(req->hwnd) && ShowWindowAsync_orig) {
                const int cmd = req->origShowCmd ? req->origShowCmd : SW_RESTORE;
                // The target window may live on another (possibly hung)
                // thread; a synchronous ShowWindow here would block unload.
                // ShowWindowAsync posts the restore — the queued message is
                // processed by the owner even after the mod is unmapped, so
                // the window still comes back.
                ShowWindowAsync_orig(req->hwnd, cmd);
            }
            if (req->hwnd && IsWindow(req->hwnd)) {
                DisableTransitions(req->hwnd, FALSE);
            }
            delete req;
        }
    }
    // In-flight request: RunAnimation bails out on g_stopping within one
    // frame and FinishQueued performs the deferred restore (the hooks are
    // still installed here).
    HWND hwndCurrent = g_hwndCurrent.load();
    if (hwndCurrent && IsWindow(hwndCurrent)) {
        DisableTransitions(hwndCurrent, FALSE);
    }
    // The thread must be fully dead before this function returns: Windhawk
    // removes the hooks and FreeLibrary's the mod right after, and a thread
    // still inside the image (or calling a removed trampoline) would crash
    // the host process.
    StopAnimThread();
}

void Wh_ModUninit() {
    Wh_Log(L"[%s] Uninit", g_exeName);
    g_fDisabled.store(true);
    g_stopping.store(true);
    // Wh_ModBeforeUninit already stopped the thread and honored the deferred
    // shows; this is defensive cleanup for abnormal unloads only.
    StopAnimThread();
    {
        std::lock_guard<std::mutex> lock(g_queueMutex);
        while (!g_queue.empty()) {
            AnimRequest* req = g_queue.front();
            g_queue.pop_front();
            if (!req) {
                continue;
            }
            if (req->deferOrig && req->hwnd && IsWindow(req->hwnd) && pShowWindowRaw) {
                // Never touch the trampoline here (the hooks are already
                // removed by the time Wh_ModUninit runs): call user32 directly.
                pShowWindowRaw(req->hwnd, req->origShowCmd ? req->origShowCmd : SW_RESTORE);
            }
            if (req->hwnd && IsWindow(req->hwnd)) {
                DisableTransitions(req->hwnd, FALSE);
            }
            delete req;
        }
    }
    HWND hwndCurrent = g_hwndCurrent.load();
    if (hwndCurrent && IsWindow(hwndCurrent)) {
        DisableTransitions(hwndCurrent, FALSE);
    }
    if (g_hinst) {
        UnregisterClassW(kAnimClassName, g_hinst);
    }
    std::lock_guard<std::mutex> lock(g_cacheMutex);
    g_captureCache.clear();
    g_captureLru.clear();

}
