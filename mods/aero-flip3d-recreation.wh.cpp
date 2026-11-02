// ==WindhawkMod==
// @id              aero-flip3d-recreation
// @name            Aero Flip 3D Recreation
// @description     This mod recreates the classic Windows Vista/7 Flip 3D switcher on modern Windows using a native DirectComposition engine, without replacing or patching any system file
// @version         2.0.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @license         Apache-2.0
// @include         windhawk.exe
// @compilerOptions -ldwmapi -ldcomp -ldxgi -ld3d11 -luser32 -lgdi32 -loleacc -lole32 -loleaut32 -luuid -lwindowscodecs -ladvapi32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Aero Flip 3D Recreation
## Screenshot

![Screenshot](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/aeroflip3d2.png)

## About the mod
This mod recreates the classic Windows Vista/7 Flip 3D window switcher on modern Windows versions.

Version 2.0.0 is a complete rewrite of the earlier thumbnail-strip version (1.2.0). It now uses a real
DirectComposition engine: window previews are live DWM shared thumbnails placed on true 3D-transformed
visuals, with the desktop shown as a backdrop behind the deck, and a Start button card like the original.

This is a best-effort mod: it is a recreation, not Microsoft's original compositor, and it relies on
undocumented DWM entry points, so a Windows update may break it. **It does not replace any system file
and does not modify DWM or Explorer**; it runs only in its own Windhawk tool process.

To help the author improve the mod, feel free to send suggestions.

## Keyboard shortcuts

| Shortcut | Action |
| --- | --- |
| `Win+Tab` | Opens Aero Flip 3D, or steps to the next window if it is already open |
| `Win+Shift+Tab` | Opens Aero Flip 3D, or steps backwards if it is already open |
| `Ctrl+Alt+F12` | Opens or closes Aero Flip 3D manually. Use this as a fallback. |

Releasing the Windows key selects the window in front.

### While Aero Flip 3D is open

- Navigate: `Tab` / `Shift+Tab`, arrow keys, `Home`, or mouse wheel
- Select: `Enter`, `Space`, or a left click on a card
- Close: `Esc`
- Emergency exit: `Ctrl+Shift+Esc` opens Task Manager

## Notes

- Aero Flip 3D appears on the primary monitor and shows the open windows in the 3D deck.
- The native Alt+Tab window switcher is not modified and continues to work normally.
- While the mod is enabled, Win+Tab is redirected from the modern Task View to the classic Flip 3D experience.
- To restore the original Windows Task View behavior, simply disable or uninstall the mod.
- Tested on Windows 11 24H2.

## Credits and license

**Original implementation:** Flip3DComp by **ALTaleX531** and **milestprower92**
(DirectComposition engine, DWM shared-thumbnail visuals, 3D transforms, shell backdrop and
accessibility model). This mod is a port of that source into a single Windhawk tool mod, adapted by
**babamohammed**, who made substantial modifications: Windhawk tool-process integration, the Win+Tab
hotkey and low-level-hook lifecycle, Win-key conflict handling with other mods, crash logging,
and a per-load window class that fixes a crash when the mod is reloaded.

This is not Microsoft's original Windows 7 compositor, only a recreation.

The original Flip3DComp source is licensed under the **Apache License, Version 2.0**, and this
derived work is distributed under the same license. In short, you may use, copy, modify and
redistribute it, in source or binary form, provided that you:

- keep the original copyright, license and attribution notices (this section),
- state that the files were changed (see above), and
- include a copy of the license (https://www.apache.org/licenses/LICENSE-2.0).

The software is provided "AS IS", without warranties or conditions of any kind, and the authors are
not liable for any damage arising from its use. The full license text is available at
https://www.apache.org/licenses/LICENSE-2.0.
*/
// ==/WindhawkModReadme==

// Flip3DComp source port: substantial modifications for a self-contained
// Windhawk tool-mod process and hotkey lifecycle. Original implementation by
// ALTaleX531 and milestprower92, Apache License 2.0 (see the mod readme).

#define WIN32_LEAN_AND_MEAN
#define FLIP3D_ENABLE_ANIMATION_EASING 0
#include <windows.h>
#include <shellapi.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <dcomp.h>
#include <windhawk_api.h>
#include <wrl/client.h>

// MinGW-w64's bundled dcomp.h omits IDCompositionVisual3 even though the
// Windows SDK defines it. Supply the official ABI declaration only for MinGW;
// real Windhawk/Windows builds use the SDK declaration from dcomp.h.
#if defined(__MINGW32__)
// MinGW's dcomp.h also omits the UUID helper for IDCompositionVisual.
#ifdef __CRT_UUID_DECL
__CRT_UUID_DECL(IDCompositionVisual, 0x4d93059d, 0x097b, 0x4651,
                0x9a, 0x60, 0xf0, 0xf2, 0x51, 0x16, 0xe2, 0xf3);
#endif
#endif

#if defined(__MINGW32__) && !defined(__IDCompositionVisual3_INTERFACE_DEFINED__)
#undef INTERFACE
#define INTERFACE IDCompositionVisual3
DECLARE_INTERFACE_IID_(IDCompositionVisual3, IDCompositionVisualDebug,
                       "2775F462-B6C1-4015-B0BE-B3E7D6A4976D") {
    using IDCompositionVisualDebug::SetTransform;
    // Vtable order matters: the Windows SDK lists the pointer overload
    // (animation / transform object) BEFORE the value overload. Itanium-ABI
    // compilers (MinGW/clang) lay out virtuals in declaration order, so the
    // order here must match the real binary layout.
    STDMETHOD(SetDepthMode)(DCOMPOSITION_DEPTH_MODE mode) PURE;
    STDMETHOD(SetOffsetZ)(IDCompositionAnimation* animation) PURE;
    STDMETHOD(SetOffsetZ)(float offsetZ) PURE;
    STDMETHOD(SetOpacity)(IDCompositionAnimation* animation) PURE;
    STDMETHOD(SetOpacity)(float opacity) PURE;
    STDMETHOD(SetTransform)(IDCompositionTransform3D* transform) PURE;
    STDMETHOD(SetTransform)(const D2D_MATRIX_4X4_F& matrix) PURE;
    STDMETHOD(SetVisible)(BOOL visible) PURE;
};
#ifdef __CRT_UUID_DECL
__CRT_UUID_DECL(IDCompositionVisual3, 0x2775f462, 0xb6c1, 0x4015,
                0xb0, 0xbe, 0xb3, 0xe7, 0xd6, 0xa4, 0x97, 0x6d);
#endif
#define __IDCompositionVisual3_INTERFACE_DEFINED__
#endif
#include <d3d11.h>
#include <dxgi1_2.h>
#include <oleacc.h>
#include <oaidl.h>
#include <wincodec.h>
#include <chrono>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <atomic>
#include <cwchar>
#include <cwctype>
#include <memory>
#include <utility>
#include <exception>

#if defined(__clang__)
// D2D_MATRIX_4X4_F uses an anonymous union; the flat 16-float initializers
// below are intentional row-major values, not missing data.
#pragma clang diagnostic ignored "-Wmissing-braces"
#endif


// ===== Ported from src/Config.h =====
// ============================================================================
// Config.h — Constants, enums, type aliases, and DWM thumbnail API definitions
// ============================================================================
// Mirrors the constant / enum organization pattern from CFlip3D.h
// ============================================================================
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <dwmapi.h>
#include <dcomp.h>
#include <wrl/client.h>
#include <d3d11.h>
#include <dxgi1_2.h>

// ============================================================================
// Type aliases
// ============================================================================
using Microsoft::WRL::ComPtr;
using Matrix4x4 = D2D_MATRIX_4X4_F;

// ============================================================================
// uDWM animation / camera constants
// ============================================================================
constexpr float  kEnterExitDurationSec     = 0.25f;    // uDWM binary: 0.25s
constexpr float  kExitDurationSec          = 0.25f;    // uDWM: same timeline for exit + exit-rotate
#ifndef FLIP3D_ENABLE_ANIMATION_EASING
#define FLIP3D_ENABLE_ANIMATION_EASING 1
#endif
constexpr bool   kEnableAnimationEasing    = FLIP3D_ENABLE_ANIMATION_EASING != 0; // controls if Flip3D should use animation easing or not

// Timeline easing: CSS cubic-bezier(0, 0, 0, 1) — fast start, soft landing
constexpr float  kTimelineBezierX1 = 0.0f;
constexpr float  kTimelineBezierY1 = 0.0f;
constexpr float  kTimelineBezierX2 = 0.0f;
constexpr float  kTimelineBezierY2 = 1.0f;

constexpr float  kScrollSmoothTimeSec = 0.15f - (kEnableAnimationEasing ? 0.05f : 0.0f);    // smooth-scroll ease time constant, if animation easing is enabled, it will be 0.10f, so the scroll speed can be similar to the original uDWM Flip3D's non-eased animation
constexpr float  kAnimationRate = 1.0f - (kEnableAnimationEasing ? 0.2f : 0.0f); // animation speed multiplier (1 = normal), if animation easing is enabled, it will be 0.8f, so the animation speed can be similar to the original uDWM Flip3D's non-eased animation
constexpr float  kShiftAnimationRate = 0.1f - (kEnableAnimationEasing ? 0.05f : 0.0f); // shift animation speed multiplier, if animation easing is enabled, it will be 0.05f, so the shift animation speed can be similar to the original uDWM Flip3D's non-eased animation
constexpr float  kScrollSettleEpsilon      = 0.002f;   // scrollPos≈scrollTarget threshold (browse
constexpr float  kFrontCardExitSlot        = -0.5f;    // front card slot when exiting (fade-out)
constexpr float  kFrontCardFadeSpeed       = 0.0f;     // front card fade speed (0 = instant, 1 = linear)
constexpr float  kScrollWheelNotchFraction = 1.0f;     // carousel slots per mouse wheel notch
constexpr float  kWheelActiveTimeoutSec    = 0.15f;    // throttle repeated wheel processing (uDWM CFlip3D::OnWheel)
constexpr float  kKeyRepeatIntervalSec     = 0.135f;   // throttle repeated keydown processing (uDWM CFlip3D::OnKey)
constexpr float  kHeldKeyStartDelaySec     = 0.20f;    // delay continuous rotation so a key tap moves one card
constexpr float  kHeldKeyRotateSpeed       = 6.0f;     // carousel slots per second while a navigation key is held
constexpr float  kOpeningTabDelaySec       = 0.40f;    // delay the first automatic Tab move after opening
constexpr float  kRotateListDurationSec    = 0.175f;   // uDWM kRotateListDurationSeconds
constexpr float  kNearPlaneEdgeSize        = 1.15f;    // near plane half-extent
constexpr float  kNearPlaneDistance        = 1.0f;     // near plane Z distance
constexpr int    kMaxVisibleCards          = 10;       // max simultaneously visible cards
constexpr int    kMaxCards                 = 24;       // absolute max cards in carousel

enum class CardThumbnailQuality
{Low,MediumLow,Medium,MediumHigh,High,};

constexpr CardThumbnailQuality kCardThumbnailQuality = CardThumbnailQuality::Medium; // This controls the DWM thumbnail resolution used by 3D window cards. By default Windows Vista & 7 have it set to a Medium value (0.50f)

constexpr float  kBackCardOpacityScale     = 1.0f;     // scale opacity for cards near the back edge (slot ~ span-2..span)
constexpr float  kBackCardMinOpacity       = 0.60f;    // minimum opacity for cards entering from the back (wrap fade)
constexpr float  kBackCardFadeTimeSec      = 0.015f;    // fade-in time for cards entering from the back (wrap fade)
constexpr bool   kEnableBackCardOpacityEasing = true;

// Pre-computed camera poses (radians)
constexpr float kCameraFinalTranslateX = -1.1f;             
constexpr float kCameraFinalTranslateY =  0.35f;   // uDWM InitializeModelAndCamera
constexpr float kCameraFinalTranslateZ =  0.35f;
constexpr float kCameraFinalRotateX    =  0.08726646f;   //  5.0 deg pitch
constexpr float kCameraFinalRotateY    =  0.43633232f;   // 25.0 deg yaw
constexpr float kCameraFinalRotateZ    =  0.061086524f;  //  3.5 deg roll

// Bezier control points for carousel path
constexpr float kBezierControls[3][3] = {
    { -2.0f,   0.5f,   -2.25f },
    { -1.8f,   0.55f,  -1.25f },
    { -1.45f, -0.1f,   -0.3f  },        
};

constexpr float CardThumbnailQualityScale()
{
    switch (kCardThumbnailQuality)
    {
    case CardThumbnailQuality::Low:    return 0.10f;
    case CardThumbnailQuality::MediumLow:    return 0.25f;
    case CardThumbnailQuality::Medium: return 0.55f;
    case CardThumbnailQuality::MediumHigh:    return 0.75f;
    case CardThumbnailQuality::High:   return 1.00f;
    }
    return 1.00f;
}

// Normalization Bezier coefficients (quadratic)
constexpr float kNormalizationBezier[3] = { 1.0f, 0.85f, 0.75f };

// uDWM c_dFlipRotationPercent — steady desktop wash while Flip3D is open
constexpr float kDesktopWashOpacityScale = 0.55f;

// uDWM CTopLevelWindow3D::GetFinalMinRect / flip3d BuildFinalMinRect
constexpr float kFinalMinRectWidthPercentage = 0.6f;
constexpr float kFinalMinRectLeftPercentage  = 0.3f;

// ============================================================================
// Enums
// ============================================================================

// Interpolation mode for timeline-driven animations
enum class InterpolationMode
{
    Linear      = 0,
    Cubic       = 1,  // uDWM CTimeline: smoothstep t²(3-2t)
    CubicBezier = 2,  // CSS cubic-bezier (Config.h kTimelineBezier*)
};

// Decoupled display-slot phase during smooth-scroll list wrap (uDWM cycle card).
enum class CarouselWrapPhase
{
    None          = 0,
    ExitingFront  = 1,  // slot → [-1, 0]   front fade-out (forward scroll)
    EnteringBack  = 2,  // opacity slot span→listSlot; position stays on list slot
    EnteringFront = 3,  // slot → [-1, 0]   front fade-in (backward scroll)
};

// Flip3D view state machine (uDWM CFlip3D::Flip3DState ordering preserved)
enum class ViewState
{
    Inactive                  = 0,  // not visible
    Enter                     = 1,  // animating in
    Exit                      = 2,  // animating out (stationary carousel)
    Interactive               = 3,  // fully visible, accepting input
    ExitRepeatedRotate        = 4,  // exit flatten + discrete list rotation in parallel
};

// ============================================================================
// Private DWM thumbnail API (dwmapi.dll ordinal exports)
// ============================================================================
constexpr DWORD DWM_TNP_DISABLEFORCECVI  = 0x80000000;
constexpr DWORD DWM_TNP_ENABLE3D         = 0x04000000;

// DwmpCreateSharedThumbnailVisual dwThumbnailFlags (see dwthumbnailflags-reference.md)
constexpr DWORD DWM_TNF_DWMWINDOW       = 0x2;   // fDwmWindow → PostMessage(0x327) on size change

// Posted to hwndDestination when a DWM_TNF_DWMWINDOW thumbnail source resizes.
constexpr UINT WM_DWMTHUMBNAILSOURCESIZECHANGED = 0x327;

using DwmpCreateSharedThumbnailVisual_fn = HRESULT (WINAPI *)(
    HWND hwndDestination, HWND hwndSource, DWORD dwThumbnailFlags,
    DWM_THUMBNAIL_PROPERTIES* pThumbnailProperties,
    void* pDCompDevice, void** ppVisual, HTHUMBNAIL* phThumbnailId);

using DwmpQueryWindowThumbnailSourceSize_fn = HRESULT (WINAPI *)(
    HWND hwndSource, BOOL fSourceClientAreaOnly, SIZE* pSize);

using GetWindowMinimizeRect_fn = BOOL (WINAPI *)(HWND, LPRECT);

using DwmpUpdateDesktopThumbnail_fn = HRESULT (WINAPI *)(
    HWND hwnd, LPCRECT rcDest, LPCRECT rcSrc, BYTE opacity, DWORD dwFlags);


// ===== Ported from src/MathTypes.h =====
// ============================================================================
// MathTypes.h — POD vector types for 3D math
// ============================================================================
struct Vec3
{
    float x, y, z;
};

struct Vec2
{
    float x, y;
};


// ===== Ported from src/FlipMath.h =====
// ============================================================================
// FlipMath.h — 3D math utilities (row-vector DirectComposition convention)
// ============================================================================
// All functions are inline to match the zero-overhead pattern from CFlip3D.h.
// Matrix multiplication follows DComp's DX-style 4×4 layout.
// ============================================================================
#include <algorithm>
#include <cmath>
namespace Math
{

// ---- Identity / basic transforms ----

inline Matrix4x4 Identity()
{
    return { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
}

inline Matrix4x4 Translation(float x, float y, float z)
{
    return { 1,0,0,0, 0,1,0,0, 0,0,1,0, x,y,z,1 };
}

inline Matrix4x4 Scale(float sx, float sy, float sz)
{
    return { sx,0,0,0, 0,sy,0,0, 0,0,sz,0, 0,0,0,1 };
}

// ---- Composition (must precede Rotation — used by RotationRollPitchYaw) ----

inline Matrix4x4 Multiply(const Matrix4x4& a, const Matrix4x4& b)
{
    Matrix4x4 r = {};
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            r.m[i][j] = a.m[i][0] * b.m[0][j]
                      + a.m[i][1] * b.m[1][j]
                      + a.m[i][2] * b.m[2][j]
                      + a.m[i][3] * b.m[3][j];
        }
    }
    return r;
}

inline Matrix4x4 Transpose(const Matrix4x4& m)
{
    Matrix4x4 r = {};
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            r.m[i][j] = m.m[j][i];
    return r;
}

// ---- Rotation ----

inline Matrix4x4 RotationYPR(float yaw, float pitch, float roll)
{
    float cy = cos(yaw),  sy = sin(yaw);
    float cp = cos(pitch), sp = sin(pitch);
    float cr = cos(roll),  sr = sin(roll);
    return {
        cr*cy + sr*sp*sy, sr*cp, -cr*sy + sr*sp*cy, 0,
       -sr*cy + cr*sp*sy, cr*cp,  sr*sy + cr*sp*cy, 0,
        cp*sy,            -sp,    cp*cy,             0,
        0, 0, 0, 1
    };
}

// Matches XMMatrixRotationRollPitchYaw(Pitch, Yaw, Roll) = Rz(Roll)*Rx(Pitch)*Ry(Yaw)
// Row-vector: v' = v * Ry * Rx * Rz
inline Matrix4x4 RotationRollPitchYaw(float pitch, float yaw, float roll)
{
    float cp = cos(pitch), sp = sin(pitch);
    float cy = cos(yaw),   sy = sin(yaw);
    float cr = cos(roll),  sr = sin(roll);

    Matrix4x4 Ry = { cy, 0, -sy, 0,  0, 1, 0, 0,  sy, 0, cy, 0,  0,0,0,1 };
    Matrix4x4 Rx = { 1, 0, 0, 0,  0, cp, sp, 0,  0, -sp, cp, 0,  0,0,0,1 };
    Matrix4x4 Rz = { cr, sr, 0, 0,  -sr, cr, 0, 0,  0, 0, 1, 0,  0,0,0,1 };

    Matrix4x4 RzRx = Multiply(Rz, Rx);
    return Multiply(RzRx, Ry);
}

// ---- Camera matrices ----

inline Matrix4x4 LookAtRH(Vec3 eye, Vec3 at, Vec3 up)
{
    Vec3 z = { eye.x - at.x, eye.y - at.y, eye.z - at.z };
    float zl = 1.0f / sqrt(z.x*z.x + z.y*z.y + z.z*z.z);
    z.x *= zl; z.y *= zl; z.z *= zl;

    Vec3 x = { up.y*z.z - up.z*z.y, up.z*z.x - up.x*z.z, up.x*z.y - up.y*z.x };
    float xl = 1.0f / sqrt(x.x*x.x + x.y*x.y + x.z*x.z);
    x.x *= xl; x.y *= xl; x.z *= xl;

    Vec3 y = { z.y*x.z - z.z*x.y, z.z*x.x - z.x*x.z, z.x*x.y - z.y*x.x };

    return {
        x.x, y.x, z.x, 0,
        x.y, y.y, z.y, 0,
        x.z, y.z, z.z, 0,
        -(x.x*eye.x + x.y*eye.y + x.z*eye.z),
        -(y.x*eye.x + y.y*eye.y + y.z*eye.z),
        -(z.x*eye.x + z.y*eye.y + z.z*eye.z),
        1
    };
}

inline Matrix4x4 PerspectiveRH(float nearExtent, float aspect, float nearZ, float farZ)
{
    float w = nearExtent;
    float h = nearExtent / aspect;
    float q = farZ / (nearZ - farZ);
    return {
        2.0f * nearZ / w, 0,              0,  0,
        0,                2.0f * nearZ / h, 0,  0,
        0, 0, q, -1,
        0, 0, q * nearZ, 0
    };
}

// ---- Bezier & interpolation ----

inline Vec3 Bezier(float t)
{
    float s = 1.0f - t;
    return {
        s*s*kBezierControls[0][0] + 2*s*t*kBezierControls[1][0] + t*t*kBezierControls[2][0],
        s*s*kBezierControls[0][1] + 2*s*t*kBezierControls[1][1] + t*t*kBezierControls[2][1],
        s*s*kBezierControls[0][2] + 2*s*t*kBezierControls[1][2] + t*t*kBezierControls[2][2],
    };
}

inline float NormalizationScale(float o)
{
    o = std::clamp(o, 0.0f, 1.0f);
    float inv = 1.0f - o;
    return inv*inv * kNormalizationBezier[0]
         + 2.0f * kNormalizationBezier[1] * o * inv
         + o*o   * kNormalizationBezier[2];
}

// uDWM CFlip3D::NormalizeWindowSize — pixel space fit + occupancy bezier.
inline void NormalizeWindowSize(float& w, float& h, float monW, float monH)
{
    monW = std::max(monW, 1.0f);
    monH = std::max(monH, 1.0f);

    if (w > monW || h > monH)
    {
        const float s = std::min(monW / w, monH / h);
        w *= s;
        h *= s;
    }

    float o = w / monW;
    const float oy = h / monH;
    if (oy > o)
        o = oy;

    const float ns = NormalizationScale(o);
    w *= ns;
    h *= ns;
}

// DWM thumbnail pixel size → monitor-world flat (2D) and carousel (3D) dimensions.
inline void WorldSizesFromThumbPixels(
    float thumbW, float thumbH,
    float normMonW, float normMonH,
    Vec2& flatSize, Vec2& targetSize, float& occupancy)
{
    thumbW = std::max(thumbW, 1.0f);
    thumbH = std::max(thumbH, 1.0f);

    flatSize = { thumbW / normMonW, thumbH / normMonH };

    float finalW = thumbW;
    float finalH = thumbH;
    float occW = finalW, occH = finalH;
    if (occW > normMonW || occH > normMonH)
    {
        const float s = std::min(normMonW / occW, normMonH / occH);
        occW *= s;
        occH *= s;
    }
    occupancy = std::clamp(std::max(occW / normMonW, occH / normMonH), 0.05f, 1.0f);
    NormalizeWindowSize(finalW, finalH, normMonW, normMonH);
    targetSize = { finalW / normMonW, -(finalH / normMonH) };
}

// uDWM GetFinalMinRect: taskbar button rect → small card above the button.
inline RECT BuildFinalMinRect(RECT minimizeRect, float aspectRatio)
{
    const float minW = (float)std::max(0L, minimizeRect.right - minimizeRect.left);
    const float finalW = minW * kFinalMinRectWidthPercentage;
    const float finalH = finalW * aspectRatio;
    const float finalLeft = (float)minimizeRect.left + finalW * kFinalMinRectLeftPercentage;
    const float finalTop  = (float)minimizeRect.top - finalH;

    RECT result = {};
    result.left   = (LONG)std::lround(finalLeft);
    result.top    = (LONG)std::lround(finalTop);
    result.right  = (LONG)std::lround(finalLeft + finalW);
    result.bottom = (LONG)std::lround(finalTop + finalH);
    return result;
}

inline float Lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}

// CSS cubic-bezier(x1,y1,x2,y2): map linear time x in [0,1] to eased progress.
// Newton-Raphson first, then bisection (WebKit bezier-easing) for robustness.
inline float CubicBezierProgress(float x, float x1, float y1, float x2, float y2)
{
    x = std::clamp(x, 0.0f, 1.0f);
    if (x <= 0.0f) return 0.0f;
    if (x >= 1.0f) return 1.0f;

    auto sampleX = [&](float t) {
        const float inv = 1.0f - t;
        return 3.0f * inv * inv * t * x1
             + 3.0f * inv * t * t * x2
             + t * t * t;
    };
    auto sampleY = [&](float t) {
        const float inv = 1.0f - t;
        return 3.0f * inv * inv * t * y1
             + 3.0f * inv * t * t * y2
             + t * t * t;
    };
    auto sampleDx = [&](float t) {
        const float inv = 1.0f - t;
        return 3.0f * inv * inv * x1
             + 6.0f * inv * t * (x2 - x1)
             + 3.0f * t * t * (1.0f - x2);
    };

    constexpr float kEpsilon = 1e-6f;

    float t = x;
    for (int i = 0; i < 8; ++i)
    {
        const float cx = sampleX(t) - x;
        if (std::abs(cx) < kEpsilon)
            return sampleY(t);

        const float dx = sampleDx(t);
        if (std::abs(dx) < 1e-6f)
            break;

        t -= cx / dx;
    }

    float t0 = 0.0f;
    float t1 = 1.0f;
    t = x;
    for (int i = 0; i < 32; ++i)
    {
        t = (t0 + t1) * 0.5f;
        const float cx = sampleX(t);
        if (std::abs(cx - x) < kEpsilon)
            return sampleY(t);
        if (cx < x)
            t0 = t;
        else
            t1 = t;
    }

    return sampleY((t0 + t1) * 0.5f);
}

inline float TimelineEase(float t)
{
    return CubicBezierProgress(
        t,
        kTimelineBezierX1, kTimelineBezierY1,
        kTimelineBezierX2, kTimelineBezierY2);
}

inline Vec3 Lerp(const Vec3& a, const Vec3& b, float t)
{
    return { Lerp(a.x, b.x, t), Lerp(a.y, b.y, t), Lerp(a.z, b.z, t) };
}

// ---- Ray-triangle intersection (Möller–Trumbore) ----

// Returns distance along ray (t > 0) or -1.0f on miss.
inline float IntersectRayTriangle(
    Vec3 rayOrig, Vec3 rayDir,
    Vec3 v0, Vec3 v1, Vec3 v2)
{
    Vec3 e1 = { v1.x - v0.x, v1.y - v0.y, v1.z - v0.z };
    Vec3 e2 = { v2.x - v0.x, v2.y - v0.y, v2.z - v0.z };
    Vec3 h  = {
        rayDir.y * e2.z - rayDir.z * e2.y,
        rayDir.z * e2.x - rayDir.x * e2.z,
        rayDir.x * e2.y - rayDir.y * e2.x,
    };

    float a = e1.x*h.x + e1.y*h.y + e1.z*h.z;
    if (a > -1e-6f && a < 1e-6f) return -1.0f;

    float f = 1.0f / a;
    Vec3 s = { rayOrig.x - v0.x, rayOrig.y - v0.y, rayOrig.z - v0.z };
    float u = f * (s.x*h.x + s.y*h.y + s.z*h.z);
    if (u < 0.0f || u > 1.0f) return -1.0f;

    Vec3 q = {
        s.y*e1.z - s.z*e1.y,
        s.z*e1.x - s.x*e1.z,
        s.x*e1.y - s.y*e1.x,
    };
    float v = f * (rayDir.x*q.x + rayDir.y*q.y + rayDir.z*q.z);
    if (v < 0.0f || u + v > 1.0f) return -1.0f;

    float t = f * (e2.x*q.x + e2.y*q.y + e2.z*q.z);
    return (t > 1e-6f) ? t : -1.0f;
}

// uDWM GetMonitorToWorldTransform — primary rcWork (screen px) → centered world.
inline void MonitorToWorldTopLeft(float screenX, float screenY,
                                  float monX, float monY,
                                  float monW, float monH,
                                  float& worldX, float& worldY)
{
    monW = std::max(monW, 1.0f);
    monH = std::max(monH, 1.0f);
    worldX = (screenX - monX - monW * 0.5f) / monW;
    worldY = (monY + monH * 0.5f - screenY) / monH;
}

inline Matrix4x4 BuildViewportMatrix(float width, float height,
                                     float originX = 0.0f, float originY = 0.0f)
{
    return {
        width * 0.5f, 0,              0, 0,
        0,            -height * 0.5f, 0, 0,
        0,             0,             1, 0,
        originX + width * 0.5f, originY + height * 0.5f, 0, 1,
    };
}

} // namespace Math


// ===== Ported from src/Timeline.h =====
// ============================================================================
// Timeline.h — Simple animation timeline with interpolation
// ============================================================================
// Modeled after CTimeline<float> from the uDWM reference.
// Drives enter/exit and rotation animations (CubicBezier easing by default).
// ============================================================================
#include <algorithm>
// ============================================================================
// Timeline
// ============================================================================
struct Timeline
{
    // ---- Configuration ----
    float             m_durationSeconds  = 0.0f;
    float             m_elapsedSeconds      = 0.0f;
    float             m_timelineStartValue   = 0.0f;
    float             m_timelineEndValue     = 0.0f;
    InterpolationMode m_interpolationMode         = InterpolationMode::CubicBezier;
    bool              m_timelineIsActive       = false;

    // ---- Construction ----
    Timeline() = default;

    // ---- API ----

    // Begin a new animation segment (default: CubicBezier easing).
    void Restart(float startValue, float endValue, float durationSec);
    void Restart(float startValue, float endValue,
                 float durationSec, InterpolationMode mode);

    // Advance the timeline by dt seconds.
    void Update(float dtSeconds);

    // Returns the raw linear progress [0,1].
    float RawProgress() const;

    // Returns the interpolated value after easing.
    float Value() const;

    // Linear start→end lerp (RawProgress only, ignores m_interpolationMode).
    float LinearValue() const;

    // True if the timeline has completed.
    bool IsActive() const { return m_timelineIsActive; }
};


// ===== Ported from src/CardModel.h =====
// ============================================================================
// CardModel.h — Per-window data for the Flip3D carousel
// ============================================================================
// Each CardModel represents one window in the 3D carousel.
// Modeled after Flip3DWindow from CFlip3D.h — annotated with logical groupings.
// ============================================================================
#include <wrl/client.h>
#include <dcomp.h>
using Microsoft::WRL::ComPtr;

// ============================================================================
// Move-only RAII owner for the DWM thumbnail registration associated with a card.
class ScopedDwmThumbnail {
public:
    ScopedDwmThumbnail() noexcept = default;
    explicit ScopedDwmThumbnail(HTHUMBNAIL handle) noexcept : handle_(handle) {}
    ~ScopedDwmThumbnail() { reset(); }

    ScopedDwmThumbnail(const ScopedDwmThumbnail&) = delete;
    ScopedDwmThumbnail& operator=(const ScopedDwmThumbnail&) = delete;

    ScopedDwmThumbnail(ScopedDwmThumbnail&& other) noexcept
        : handle_(std::exchange(other.handle_, nullptr)) {}
    ScopedDwmThumbnail& operator=(ScopedDwmThumbnail&& other) noexcept {
        if (this != &other) {
            reset();
            handle_ = std::exchange(other.handle_, nullptr);
        }
        return *this;
    }

    ScopedDwmThumbnail& operator=(HTHUMBNAIL handle) noexcept {
        reset(handle);
        return *this;
    }

    HTHUMBNAIL get() const noexcept { return handle_; }
    HTHUMBNAIL* put() noexcept {
        reset();
        return &handle_;
    }
    explicit operator bool() const noexcept { return handle_ != nullptr; }
    operator HTHUMBNAIL() const noexcept { return handle_; }

    void reset(HTHUMBNAIL replacement = nullptr) noexcept {
        if (handle_ && handle_ != replacement) {
            DwmUnregisterThumbnail(handle_);
        }
        handle_ = replacement;
    }

private:
    HTHUMBNAIL handle_ = nullptr;
};

// CardModel owns its DWM registration and is intentionally move-only.
struct CardModel
{
    // ---- Window identity ----
    HWND                m_windowHandle        = nullptr;   // source window handle
    int                 m_initialCarouselIndex = 0;  // order at Flip3D enter (before rotates)
    ScopedDwmThumbnail m_thumbnailRegistration;                    // owns/unregisters the DWM thumbnail

    // ---- DirectComposition visuals ----
    ComPtr<IDCompositionVisual3> m_thumbnailVisual;          // thumbnail content visual (DWM)
    ComPtr<IDCompositionVisual3> m_cardTransformVisual; // baked Model×Camera + opacity

    // ---- Geometry (monitor-world units) ----
    Vec2                m_cardTargetSize   = {};        // 3D carousel size (thumb px + NormalizeWindowSize)
    Vec3                m_originalPosition  = {};        // desktop top-left (from window rect)
    Vec3                m_flatPosition      = {};        // same anchor for 2D enter layout
    Vec2                m_flatSize     = {};        // 2D size (thumb px, or tile/rcWork)
    float               m_sourceScreenLeft      = 0.0f;      // raw screen pixel: left
    float               m_sourceScreenTop       = 0.0f;      // raw screen pixel: top
    float               m_sourceScreenWidth         = 0.0f;      // raw screen pixel: width
    float               m_sourceScreenHeight         = 0.0f;      // raw screen pixel: height

    // ---- Source content ----
    float               m_windowAspectRatio  = 16.0f / 10.0f;
    float               m_windowOccupancy    = 0.7f;       // normalized occupancy factor
    int                 m_nativeSourceWidth  = 400;     // queried DWM source width (pixels)
    int                 m_nativeSourceHeight = 300;     // queried DWM source height (pixels)
    int                 m_thumbnailSourceWidth     = 400;        // thumbnail source width (pixels)
    int                 m_thumbnailSourceHeight    = 300;        // thumbnail source height (pixels)
    bool                m_isMinimized  = false;      // window is iconic
    bool                m_isShellDesktop = false;    // Progman / GetShellWindow()

    // ---- Carousel display slot ----
    // Continuous slot for rendering (index - scrollPos), decoupled across list wrap
    // while a card completes the front-edge opacity ramp (slot in [-1, 0]).
    float               m_carouselDisplaySlot     = 0.0f;
    bool                m_hasValidCarouselDisplaySlot = false;
    CarouselWrapPhase   m_carouselWrapPhase     = CarouselWrapPhase::None;
    float               m_wrapFadeStartListSlot = 0.0f; // list slot when EnteringBack began
    // Progress [0..1] of the wrap fade (entering from back or entering
    // front). Updated each frame so ComputeUpdateAlpha can drive a smooth
    // fade-in when a card wraps into view.
    float               m_wrapFadeProgress = 0.0f;
    // Actual opacity used while a card is entering through a wrap.
    float               m_wrapOpacity = 1.0f;
    bool                m_isWrapFadeActive = false;
    bool                m_isOpacityInitialized = false;
    float               m_lastTargetOpacity = 0.0f;
};


// ===== Ported from src/Flip3DComp.h =====
// ============================================================================
// Flip3DComp.h — DirectComposition 3D Window Switcher Application
// ============================================================================
//
//   Visual tree layout:
//     Root (DesktopWindowTarget)
//       └── SceneVisual (IDCompositionVisual3)  ← 3D carousel
//       └── WashVisual (dark overlay)
//       └── Per-monitor shell thumbnails + wash (virtual desktop layout)
//
//   Thumbnails: DwmpCreateSharedThumbnailVisual (dwmapi.dll ord 147)
//   3D carousel: parent Visual carries camera matrix; children carry model matrices.
//
// ============================================================================
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <dwmapi.h>
#include <dcomp.h>
#include <wrl/client.h>
#include <d3d11.h>
#include <dxgi1_2.h>

#include <chrono>
#include <string>
#include <vector>

#include <oleacc.h>
using Microsoft::WRL::ComPtr;

class Flip3DAccessible;

enum class WallpaperPlacement
{
    Center,
    Tile,
    Stretch,
    Fit,
    Fill,
    Span,
};

inline constexpr UINT kBuildShellBackdropTimerId = 1;

// ============================================================================
// Flip3DCompApp — Main application class
// ============================================================================
class Flip3DCompApp
{
    friend class Flip3DAccessible;

public:
    // ========================================================================
    // Public interface
    // ========================================================================

    Flip3DCompApp() = default;
    Flip3DCompApp(const Flip3DCompApp&) = delete;
    Flip3DCompApp& operator=(const Flip3DCompApp&) = delete;
    ~Flip3DCompApp() noexcept;

    bool Initialize(HINSTANCE hInstance);
    HWND WindowHandle() const { return m_windowHandle; }
    const wchar_t* InitErrorMessage() const { return m_initializationError.c_str(); }
    int  Run();

private:
    // ========================================================================
    // Window procedure forwarding
    // ========================================================================

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);

    // ========================================================================
    // Lifecycle
    // ========================================================================

    bool CreateAppWindow();
    void    ApplyFullscreenLayout();
    void    EnsureTopmost();

    // ========================================================================
    // DWM Thumbnail API (dwmapi.dll ordinals 147, 162)
    // ========================================================================

    bool    LoadThumbApi();
    void    UnloadThumbApi();

    // ========================================================================
    // DirectComposition composition
    // ========================================================================

    HRESULT InitComposition();
    HRESULT CreateShellBackdrop();
    void    DestroyMonitorBackdrops();
    void    UpdateBackdropLayout();
    bool    RebuildMonitorBackdropsIfNeeded();

    // ========================================================================
    // Window enumeration
    // ========================================================================

    std::vector<HWND> EnumerateWindows();
    static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);
    struct EnumContext;
    bool    QualifiesForView(HWND hwnd) const;
    bool    IsFlip3DViewActive() const;
    bool    IsNeverHiddenWindow(HWND hwnd) const;

    // Shell Hook drives dynamic card add/remove (no ShowWindow on live windows).
    void    EnterFlip3DWindowMode();
    void    LeaveFlip3DWindowMode();
    void    SetCardTransitionsDisabled(bool disabled);
    void    OnShellHookMessage(WPARAM wParam, LPARAM lParam);
    void    OnWindowShowHide(HWND hwnd);

    int     FindCardIndex(HWND hwnd) const;
    bool    AddCardForWindow(HWND hwnd);
    void    RemoveCardAt(size_t index);
    HRESULT CreateCardVisual(CardModel& card);

    // ========================================================================
    // Card building
    // ========================================================================

    void    BuildCards();
    void    UpdateMonitorRect();
    void    UpdateCardGeometry(CardModel& card, float normMonW, float normMonH,
                               bool selectedRestore = false);
    void    OnThumbnailSourceSizeChanged();
    void    UpdateCardThumbnailDest(CardModel& card);
    HRESULT CreateCardVisuals();

    // ========================================================================
    // Per-frame update
    // ========================================================================

    void    Update(float dtSeconds);

    // ========================================================================
    // View state management
    // ========================================================================

    float   EnterProgress() const;
    void    ReplayEnterAnimation();
    void    ExitView(bool commitScroll = true,
                     float exitDurationSec = kExitDurationSec);
    void    BeginExitView();
    void    TickSmoothScroll(float dtSeconds);
    void    TickRepeatedRotate();
    void    StartRotationStep(bool backward, float durationSec);
    void    OnRotationTimeUpdated();
    int     DistanceBetween(size_t sourcePos, size_t targetPos, bool forward) const;
    float   RotationDurationForRotateList() const;
    void    SelectWindow(HWND hwndTarget);
    void    SelectFront();

    // ========================================================================
    // Rotation (uDWM m_windowList linked-list model)
    // ========================================================================

    void    RotateBy(int deltaSteps);
    void    RotateListPhysically(bool backward);
    void    RotateToWindow(HWND targetHwnd);
    float   ComputeScrollTargetForCard(int listIndex) const;
    float   CardListSlot(int listIndex) const;
    float   CaptureCarouselVisualSlot(int listIndex) const;
    float   GetCardDisplaySlot(int listIndex) const;
    void    InvalidateDisplaySlots();
    void    SyncDisplaySlotsToList();
    void    AdvanceWrapDisplaySlots(float deltaScrollPos);
    void    FinishWrapPhase(CardModel& card, int listIndex);
    void    BeginEnteringBackWrap(CardModel& card, int listIndex);
    void    StepEnteringBackWrap(CardModel& card, int listIndex);
    void    FreezeCarouselVisuals();
    void    UpdateVisualSlots(float enterProgress);
    void    SettleWrapDisplaySlots();
    void    OnCarouselWrapForward(HWND outgoingHwnd, float outgoingSlot);
    void    OnCarouselWrapBackward(HWND incomingHwnd);
    float   CarouselEdgeSpan() const;
    void    WrapCarouselScroll();
    void    RotateSelectedToListFront();
    void    AlignCarouselScrollSettled();
    void    CommitCarouselScroll();
    bool    IsSelectionAtCarouselFront() const;
    void    StepCarouselScroll(float dtSeconds, bool notifyFrontChange);

    // ========================================================================
    // Input processing
    // ========================================================================

    bool    OnKey(bool down, UINT vkCode, LPARAM lParam);
    bool    OnWheel(int wheelDelta);
    bool    OnMouse(LONG x, LONG y, bool pressed);

    // ========================================================================
    // Hit testing (3D ray-triangle intersection)
    // ========================================================================

    HWND    HitTest3DScene(LONG screenX, LONG screenY) const;

    // ========================================================================
    // Camera & transforms
    // ========================================================================

    void    UpdateCamera(float enterProgress);
    void    UpdateCards(float enterProgress, float dtSeconds);
    Matrix4x4 BuildCameraMatrix(float enterProgress) const;
    Matrix4x4 BuildViewMatrix(float enterProgress) const;
    Matrix4x4 BuildProjMatrix(float aspect, float enterProgress) const;
    Matrix4x4 BuildModelMatrix(const CardModel& card, float paramT,
                               float enterProgress, float flatDepthRank) const;

    float ComputeCarouselEdgeOpacity(float slot) const;
    float ComputeUpdateAlpha(const CardModel& card, float enterProgress,
                             float carouselSlot) const;
    float ComputeRotationDisplaySlot(int listIndex) const;
    float ComputeRotationAlpha(const CardModel& card, float enterProgress,
                               int listIndex, float carouselSlot) const;
    bool  ShouldDrawRotationCard(int listIndex) const;
    float ComputeCarouselPathDenom() const;
    float ComputeCarouselBezierT(float slot) const;

    float   ComputeFlatDepthRank(float slot, float enterProgress,
                                 int listIndex) const;
    float   ComputePaintKey(float slot, float enterProgress, int listIndex) const;

    // Accessibility (uDWM CFlip3DAccessible + NotifyWinEvent)
    bool    InitAccessibility();
    void    ShutdownAccessibility();
    IAccessible* GetAccessibleObject();
    void    NotifyAccessibilityEvent(DWORD event, LONG childId = CHILDID_SELF);
    void    NotifyAccessibilityFocusFront();
    int     AccessibleChildCount() const;
    HRESULT AccessibleWindowName(int index, BSTR* pszName) const;
    bool    AccessibleCardScreenRect(int index, long* pxLeft, long* pyTop,
                                     long* pcxWidth, long* pcyHeight) const;
    int     AccessibleHitTest(long screenX, long screenY) const;
    bool    AccessiblePointInView(POINT screenPt) const;
    HRESULT AccessibleRotateToIndex(int index);
    HRESULT AccessibleSelectIndex(int index);

    // ========================================================================
    // Member variables
    // ========================================================================

    // ---- Window / instance ----
    HINSTANCE               m_moduleInstance     = nullptr;
    HWND                    m_windowHandle          = nullptr;
    bool                    m_isMessageLoopActive   = false;
    std::wstring            m_initializationError;
    bool                    m_isShiftSlowdownSettingEnabled = false;  // DWM AnimationsShiftKey, read once in Initialize.

    // Per-monitor shell thumbnail + dark wash (client coords = virtual desktop).
    struct MonitorBackdrop
    {
        RECT                        rcMonitor = {};
        RECT                        rcWork    = {};
        RECT                        rcTaskbar = {};
        ComPtr<IDCompositionVisual3> washVisual;
        ComPtr<IDCompositionVisual3> shellContainer;
        ComPtr<IDCompositionVisual3> shellThumb;
        ScopedDwmThumbnail          hShellThumb;
        ComPtr<IDCompositionVisual3> taskbarContainer;
        ComPtr<IDCompositionVisual3> taskbarThumb;
        ScopedDwmThumbnail          hTaskbarThumb;
        ComPtr<IDCompositionVisual3> wallpaperVisual;
        ComPtr<IDCompositionSurface> wallpaperSurface;
        UINT                        wallpaperWidth = 0;
        UINT                        wallpaperHeight = 0;
        WallpaperPlacement          wallpaperPlacement = WallpaperPlacement::Stretch;
        std::vector<ComPtr<IDCompositionVisual3>> wallpaperTiles;
        std::vector<POINT>           wallpaperTileOrigins;
        // Separate top-level Start button window (e.g. StartAllBack orb).
        RECT                        rcStart = {};
        ComPtr<IDCompositionVisual3> startContainer;
        ComPtr<IDCompositionVisual3> startThumb;
        ScopedDwmThumbnail          hStartThumb;
    };

    void    TryCreateStartButtonThumbnail(MonitorBackdrop& mon);

    // ---- Dimensions ----
    UINT                    m_viewWidth         = 1600;
    UINT                    m_viewHeight        = 900;
    float                   m_monitorWidth          = 1920.0f;
    float                   m_monitorHeight          = 1080.0f;
    float                   m_monitorOriginX    = 0.0f;   // primary rcWork.left (screen px)
    float                   m_monitorOriginY    = 0.0f;   // primary rcWork.top  (screen px)
    float                   m_viewOriginX         = 0.0f;   // primary rcWork origin in client px
    float                   m_viewOriginY         = 0.0f;
    std::vector<MonitorBackdrop> m_monitorBackdropLayers;
    ComPtr<IDCompositionSurface> m_desktopWashSurface;
    bool                    m_isBackdropBuildPending = false;
    bool                    m_isBackdropReady = false;
    bool                    m_isMinimized     = false;
    bool                    m_isRightToLeft           = false;
    bool                    m_areThumbnailsDirty = false; // coalesce WM 0x327 bursts
    UINT                    m_shellHookMessage     = 0;
    bool                    m_isShellHookRegistered = false;

    // ---- Frame timing ----
    std::chrono::steady_clock::time_point   m_previousFrameTime;
    std::chrono::steady_clock::time_point   m_lastProcessedKey{};
    int                                       m_pendingWheelSlots = 0;
    std::chrono::steady_clock::time_point     m_lastMouseWheelTime{};
    UINT                                      m_heldNavigationVirtualKey = 0;
    int                                       m_heldNavigationDirection = 0;
    std::chrono::steady_clock::time_point     m_heldNavigationStartTime{};
    bool                                      m_isOpeningTabPending = false;
    std::chrono::steady_clock::time_point     m_openingTabStartTime{};

    // ---- Cards & view state ----
    std::vector<CardModel>  m_windowCards;
    ViewState               m_viewState         = ViewState::Inactive;
    Timeline                m_enterExitAnimation;

    // ---- Smooth carousel scroll ----
    float                   m_scrollPosition     = 0.0f;
    float                   m_scrollTargetPosition  = 0.0f;
    float                   m_wrapScrollAdjustmentThisFrame = 0.0f;

    // Frozen browse scroll at exit start; held constant during ExitRepeatedRotate
    float                   m_exitScrollSnapshot    = 0.0f;

    // ---- Discrete list rotation (uDWM m_rotateListTimeline) ----
    Timeline                m_rotateAnimation;
    bool                    m_rotateBackward = false;
    bool                    m_showOutgoingCardDuringRotation = false;
    float                   m_repeatedRotateRate = 0.0f;
    int                     m_repeatedRotateStepsRemaining = 0;

    // ---- Selection ----
    HWND                    m_initialFrontWindowHandle = nullptr;
    HWND                    m_selectedWindowHandle   = nullptr;
    std::vector<HWND>       m_lastPaintOrder;
    HWND                    m_hitWindowHandle        = nullptr;

    // ---- Accessibility ----
    IAccessible*            m_accessibleObject    = nullptr;
    bool                    m_isComInitialized = false;
    bool                    m_shouldUninitializeCom = false;
    // ---- D3D11 device ----
    ComPtr<ID3D11Device>              m_d3dDevice;

    // ---- DirectComposition resources ----
    ComPtr<IDCompositionDesktopDevice>  m_compositionDevice;
    ComPtr<IDCompositionTarget>         m_compositionTarget;
    ComPtr<IDCompositionVisual2>        m_rootCompositionVisual;
    ComPtr<IDCompositionVisual3>        m_sceneCompositionVisual;

    // ---- DWM thumbnail API ----
    HMODULE                               m_dwmApiModule        = nullptr;
    // Required at startup (LoadThumbApi); always non-null after Initialize succeeds.
    DwmpCreateSharedThumbnailVisual_fn    m_createSharedThumbnailVisual = nullptr;
    DwmpQueryWindowThumbnailSourceSize_fn m_queryThumbnailSourceSize          = nullptr;
    GetWindowMinimizeRect_fn              m_getWindowMinimizeRect    = nullptr;
};


// ===== Ported from src/Flip3DAccessible.h =====
// ============================================================================
// Flip3DAccessible.h — COM IAccessible for Flip3D (uDWM CFlip3DAccessible port)
// ============================================================================
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <oleacc.h>
#include <oaidl.h>

class Flip3DCompApp;

class Flip3DAccessible final : public IAccessible
{
public:
    static HRESULT Create(Flip3DCompApp* app, IAccessible** ppAccessible);

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, void** ppvObj) override;
    STDMETHODIMP_(ULONG) AddRef() override;
    STDMETHODIMP_(ULONG) Release() override;

    // IDispatch (uDWM: E_NOTIMPL stubs)
    STDMETHODIMP GetTypeInfoCount(UINT* pctinfo) override;
    STDMETHODIMP GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo) override;
    STDMETHODIMP GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames,
                               LCID lcid, DISPID* rgDispId) override;
    STDMETHODIMP Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags,
                        DISPPARAMS* pDispParams, VARIANT* pVarResult,
                        EXCEPINFO* pExcepInfo, UINT* puArgErr) override;

    // IAccessible
    STDMETHODIMP get_accParent(IDispatch** ppdispParent) override;
    STDMETHODIMP get_accChildCount(long* pChildCount) override;
    STDMETHODIMP get_accChild(VARIANT varChild, IDispatch** ppdispChild) override;
    STDMETHODIMP get_accName(VARIANT varChild, BSTR* pszName) override;
    STDMETHODIMP get_accValue(VARIANT varChild, BSTR* pszValue) override;
    STDMETHODIMP get_accDescription(VARIANT varChild, BSTR* pszDescription) override;
    STDMETHODIMP get_accRole(VARIANT varChild, VARIANT* pvarRole) override;
    STDMETHODIMP get_accState(VARIANT varChild, VARIANT* pvarState) override;
    STDMETHODIMP get_accHelp(VARIANT varChild, BSTR* pszHelp) override;
    STDMETHODIMP get_accHelpTopic(BSTR* pszHelpFile, VARIANT varChild,
                                  long* pidTopic) override;
    STDMETHODIMP get_accKeyboardShortcut(VARIANT varChild,
                                         BSTR* pszKeyboardShortcut) override;
    STDMETHODIMP get_accFocus(VARIANT* pvarFocusChild) override;
    STDMETHODIMP get_accSelection(VARIANT* pvarSelectedChildren) override;
    STDMETHODIMP get_accDefaultAction(VARIANT varChild,
                                      BSTR* pszDefaultAction) override;
    STDMETHODIMP accSelect(long flagsSelect, VARIANT varChild) override;
    STDMETHODIMP accLocation(long* pxLeft, long* pyTop, long* pcxWidth,
                             long* pcyHeight, VARIANT varChild) override;
    STDMETHODIMP accNavigate(long navDir, VARIANT varStart,
                             VARIANT* pvarEndUpAt) override;
    STDMETHODIMP accHitTest(long xLeft, long yTop,
                            VARIANT* pvarChildAtPoint) override;
    STDMETHODIMP accDoDefaultAction(VARIANT varChild) override;
    STDMETHODIMP put_accName(VARIANT varChild, BSTR szName) override;
    STDMETHODIMP put_accValue(VARIANT varChild, BSTR szValue) override;

private:
    explicit Flip3DAccessible(Flip3DCompApp* app);

    int  GetChildrenCount() const;
    void SetChildIndex(int index, VARIANT* pvarPlace) const;

    ULONG           m_referenceCount = 1;
    Flip3DCompApp*  m_accessibleOwnerApp = nullptr;
};


// ===== Ported from src/Timeline.cpp =====
// ============================================================================
// Timeline.cpp — Animation timeline implementation
// ============================================================================
// ============================================================================
// Timeline implementation
// ============================================================================

void Timeline::Restart(float startValue, float endValue, float durationSec)
{
    Restart(startValue, endValue, durationSec, InterpolationMode::CubicBezier);
}

void Timeline::Restart(float startValue, float endValue,
                       float durationSec, InterpolationMode mode)
{
    m_timelineStartValue  = startValue;
    m_timelineEndValue    = endValue;
    m_durationSeconds = std::max(durationSec, 0.0001f);
    m_elapsedSeconds     = 0.0f;
    m_interpolationMode        = mode;
    m_timelineIsActive      = true;
}

void Timeline::Update(float dtSeconds)
{
    if (m_timelineIsActive)
    {
        m_elapsedSeconds = std::min(m_elapsedSeconds + dtSeconds, m_durationSeconds);
        if (m_elapsedSeconds >= m_durationSeconds)
            m_timelineIsActive = false;
    }
}

float Timeline::RawProgress() const
{
    return std::clamp(m_elapsedSeconds / std::max(m_durationSeconds, 0.0001f), 0.0f, 1.0f);
}

float Timeline::Value() const
{
    float t = RawProgress();

    if (m_interpolationMode == InterpolationMode::Cubic)
    {
        // uDWM / flip3d CTimeline::Progress — smoothstep
        t = t * t * (3.0f - (2.0f * t));
    }
    else if (m_interpolationMode == InterpolationMode::CubicBezier)
    {
        t = Math::TimelineEase(t);
    }

    return Math::Lerp(m_timelineStartValue, m_timelineEndValue, t);
}

float Timeline::LinearValue() const
{
    return Math::Lerp(m_timelineStartValue, m_timelineEndValue, RawProgress());
}


// ===== Ported from src/Flip3DAccessible.cpp =====
// ============================================================================
// Flip3DAccessible.cpp — COM IAccessible (uDWM CFlip3DAccessible port)
// ============================================================================
#include <algorithm>
#include <cstddef>

namespace {

constexpr int kAccStringContainer     = 3000;  // uDWM resource 0xBB8
constexpr int kAccStringDefaultAction = 3001;  // uDWM resource 0xBB9

// Fallbacks when string resources are absent (standalone exe).
constexpr wchar_t kFallbackContainerName[]     = L"Flip 3D";
constexpr wchar_t kFallbackDefaultActionName[] = L"Switch";

HRESULT LoadAccString(int stringId, BSTR* pbstrTarget)
{
    if (!pbstrTarget)
        return E_POINTER;

    *pbstrTarget = nullptr;

    wchar_t buffer[260] = {};
    if (LoadStringW(GetModuleHandleW(nullptr),
                    (UINT)stringId, buffer, (int)std::size(buffer)))
    {
        *pbstrTarget = SysAllocString(buffer);
        return *pbstrTarget ? S_OK : E_OUTOFMEMORY;
    }

    const wchar_t* fallback = (stringId == kAccStringDefaultAction)
        ? kFallbackDefaultActionName : kFallbackContainerName;
    *pbstrTarget = SysAllocString(fallback);
    return *pbstrTarget ? S_OK : E_OUTOFMEMORY;
}

} // namespace

// ============================================================================
Flip3DAccessible::Flip3DAccessible(Flip3DCompApp* app)
    : m_accessibleOwnerApp(app)
{
}

HRESULT Flip3DAccessible::Create(Flip3DCompApp* app, IAccessible** ppAccessible)
{
    if (!app || !ppAccessible)
        return E_POINTER;

    *ppAccessible = new (std::nothrow) Flip3DAccessible(app);
    if (!*ppAccessible)
        return E_OUTOFMEMORY;

    return S_OK;
}

STDMETHODIMP Flip3DAccessible::QueryInterface(REFIID riid, void** ppvObj)
{
    if (!ppvObj)
        return E_POINTER;

    if (riid == IID_IUnknown || riid == IID_IDispatch || riid == IID_IAccessible)
    {
        *ppvObj = static_cast<IAccessible*>(this);
        AddRef();
        return S_OK;
    }

    *ppvObj = nullptr;
    return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) Flip3DAccessible::AddRef()
{
    return (ULONG)InterlockedIncrement((LONG*)&m_referenceCount);
}

STDMETHODIMP_(ULONG) Flip3DAccessible::Release()
{
    const ULONG ref = (ULONG)InterlockedDecrement((LONG*)&m_referenceCount);
    if (ref == 0)
        delete this;
    return ref;
}

STDMETHODIMP Flip3DAccessible::GetTypeInfoCount(UINT* /*pctinfo*/)
{
    return E_NOTIMPL;
}

STDMETHODIMP Flip3DAccessible::GetTypeInfo(UINT /*iTInfo*/, LCID /*lcid*/,
                                             ITypeInfo** /*ppTInfo*/)
{
    return E_NOTIMPL;
}

STDMETHODIMP Flip3DAccessible::GetIDsOfNames(REFIID /*riid*/, LPOLESTR* /*rgszNames*/,
                                             UINT /*cNames*/, LCID /*lcid*/,
                                             DISPID* /*rgDispId*/)
{
    return E_NOTIMPL;
}

STDMETHODIMP Flip3DAccessible::Invoke(DISPID /*dispIdMember*/, REFIID /*riid*/,
                                      LCID /*lcid*/, WORD /*wFlags*/,
                                      DISPPARAMS* /*pDispParams*/,
                                      VARIANT* /*pVarResult*/,
                                      EXCEPINFO* /*pExcepInfo*/, UINT* /*puArgErr*/)
{
    return E_NOTIMPL;
}

int Flip3DAccessible::GetChildrenCount() const
{
    return m_accessibleOwnerApp ? m_accessibleOwnerApp->AccessibleChildCount() : 0;
}

void Flip3DAccessible::SetChildIndex(int index, VARIANT* pvarPlace) const
{
    VariantInit(pvarPlace);
    pvarPlace->vt   = VT_I4;
    pvarPlace->lVal = index;
}

STDMETHODIMP Flip3DAccessible::get_accParent(IDispatch** ppdispParent)
{
    if (!ppdispParent)
        return E_POINTER;

    *ppdispParent = nullptr;
    // uDWM passes object id 0 for the desktop root accessible.
    return AccessibleObjectFromWindow(
        GetDesktopWindow(), 0, IID_IDispatch, (void**)ppdispParent);
}

STDMETHODIMP Flip3DAccessible::get_accChildCount(long* pChildCount)
{
    if (!pChildCount)
        return E_POINTER;

    *pChildCount = GetChildrenCount();
    return S_OK;
}

STDMETHODIMP Flip3DAccessible::get_accChild(VARIANT varChild,
                                            IDispatch** ppdispChild)
{
    if (varChild.vt == VT_EMPTY)
        return E_INVALIDARG;

    if (!ppdispChild)
        return E_POINTER;

    *ppdispChild = nullptr;
    return S_FALSE;
}

STDMETHODIMP Flip3DAccessible::get_accName(VARIANT varChild, BSTR* pszName)
{
    if (!pszName)
        return E_POINTER;

    if (varChild.vt != VT_I4)
        return E_INVALIDARG;

    if (varChild.lVal == CHILDID_SELF)
        return LoadAccString(kAccStringContainer, pszName);

    const int index = varChild.lVal - 1;
    if (index < 0 || index >= GetChildrenCount())
        return E_INVALIDARG;

    if (!m_accessibleOwnerApp)
        return DISP_E_MEMBERNOTFOUND;

    try { return m_accessibleOwnerApp->AccessibleWindowName(index, pszName); }
    catch (...) { return E_FAIL; }
}

STDMETHODIMP Flip3DAccessible::get_accValue(VARIANT /*varChild*/, BSTR* /*pszValue*/)
{
    return DISP_E_MEMBERNOTFOUND;
}

STDMETHODIMP Flip3DAccessible::get_accDescription(VARIANT /*varChild*/,
                                                    BSTR* pszDescription)
{
    if (!pszDescription)
        return E_POINTER;

    *pszDescription = nullptr;
    return S_FALSE;
}

STDMETHODIMP Flip3DAccessible::get_accRole(VARIANT varChild, VARIANT* pvarRole)
{
    if (!pvarRole)
        return E_POINTER;

    if (varChild.vt != VT_I4)
        return E_INVALIDARG;

    VariantInit(pvarRole);
    pvarRole->vt   = VT_I4;
    pvarRole->lVal = (varChild.lVal == CHILDID_SELF)
        ? ROLE_SYSTEM_CLIENT
        : ROLE_SYSTEM_WINDOW;
    return S_OK;
}

STDMETHODIMP Flip3DAccessible::get_accState(VARIANT varChild, VARIANT* pvarState)
{
    if (!pvarState)
        return E_POINTER;

    if (varChild.vt != VT_I4)
        return E_INVALIDARG;

    DWORD state = STATE_SYSTEM_FOCUSABLE;

    if (varChild.lVal > 0)
    {
        const int index = varChild.lVal - 1;
        if (index < 0)
            return E_INVALIDARG;

        state = STATE_SYSTEM_FOCUSABLE | STATE_SYSTEM_SELECTABLE;

        if (GetChildrenCount() > 0 && index == 0)
            state |= STATE_SYSTEM_SELECTED | STATE_SYSTEM_FOCUSED;
    }

    VariantInit(pvarState);
    pvarState->vt   = VT_I4;
    pvarState->lVal = (LONG)state;
    return S_OK;
}

STDMETHODIMP Flip3DAccessible::get_accHelp(VARIANT /*varChild*/, BSTR* /*pszHelp*/)
{
    return E_NOTIMPL;
}

STDMETHODIMP Flip3DAccessible::get_accHelpTopic(BSTR* pszHelpFile, VARIANT /*varChild*/,
                                                long* pidTopic)
{
    if (!pszHelpFile || !pidTopic)
        return E_POINTER;

    *pszHelpFile = nullptr;
    *pidTopic    = 0;
    return S_FALSE;
}

STDMETHODIMP Flip3DAccessible::get_accKeyboardShortcut(VARIANT /*varChild*/,
                                                       BSTR* /*pszKeyboardShortcut*/)
{
    return E_NOTIMPL;
}

STDMETHODIMP Flip3DAccessible::get_accFocus(VARIANT* pvarFocusChild)
{
    if (!pvarFocusChild)
        return E_POINTER;

    if (GetChildrenCount() <= 0)
        return DISP_E_MEMBERNOTFOUND;

    SetChildIndex(1, pvarFocusChild);
    return S_OK;
}

STDMETHODIMP Flip3DAccessible::get_accSelection(VARIANT* pvarSelectedChildren)
{
    if (!pvarSelectedChildren)
        return E_POINTER;

    VariantInit(pvarSelectedChildren);
    pvarSelectedChildren->vt = VT_EMPTY;
    return S_OK;
}

STDMETHODIMP Flip3DAccessible::get_accDefaultAction(VARIANT /*varChild*/,
                                                    BSTR* pszDefaultAction)
{
    if (!pszDefaultAction)
        return E_POINTER;

    return LoadAccString(kAccStringDefaultAction, pszDefaultAction);
}

STDMETHODIMP Flip3DAccessible::accSelect(long flagsSelect, VARIANT varChild)
{
    if (varChild.vt != VT_I4 || !(flagsSelect & SELFLAG_TAKEFOCUS))
        return E_INVALIDARG;

    if (varChild.lVal == CHILDID_SELF || !m_accessibleOwnerApp)
        return E_INVALIDARG;

    const int index = varChild.lVal - 1;
    if (index < 0 || index >= GetChildrenCount())
        return E_INVALIDARG;

    try { return m_accessibleOwnerApp->AccessibleRotateToIndex(index); }
    catch (...) { return E_FAIL; }
}

STDMETHODIMP Flip3DAccessible::accLocation(long* pxLeft, long* pyTop, long* pcxWidth,
                                           long* pcyHeight, VARIANT varChild)
{
    if (!pxLeft || !pyTop || !pcxWidth || !pcyHeight || varChild.vt != VT_I4)
        return E_INVALIDARG;

    if (varChild.lVal == CHILDID_SELF)
    {
        if (!m_accessibleOwnerApp || !m_accessibleOwnerApp->WindowHandle())
            return E_INVALIDARG;

        RECT rc = {};
        if (!GetWindowRect(m_accessibleOwnerApp->WindowHandle(), &rc))
            return E_INVALIDARG;

        *pxLeft   = rc.left;
        *pyTop    = rc.top;
        *pcxWidth  = std::max(0L, rc.right - rc.left);
        *pcyHeight = std::max(0L, rc.bottom - rc.top);
        return S_OK;
    }

    const int index = varChild.lVal - 1;
    if (index < 0 || index >= GetChildrenCount() || !m_accessibleOwnerApp)
        return E_INVALIDARG;

    try
    {
        if (!m_accessibleOwnerApp->AccessibleCardScreenRect(index, pxLeft, pyTop, pcxWidth, pcyHeight))
            return E_INVALIDARG;
    }
    catch (...) { return E_FAIL; }

    return S_OK;
}

STDMETHODIMP Flip3DAccessible::accNavigate(long navDir, VARIANT varStart,
                                           VARIANT* pvarEndUpAt)
{
    if (!pvarEndUpAt)
        return E_POINTER;

    if (varStart.vt != VT_I4)
        return E_INVALIDARG;

    const int childCount = GetChildrenCount();
    HRESULT hr           = DISP_E_MEMBERNOTFOUND;

    if (varStart.lVal == CHILDID_SELF)
    {
        if (navDir == NAVDIR_FIRSTCHILD)
        {
            SetChildIndex(childCount > 0 ? 1 : 0, pvarEndUpAt);
            hr = S_OK;
        }
        else if (navDir == NAVDIR_LASTCHILD)
        {
            SetChildIndex(childCount, pvarEndUpAt);
            hr = S_OK;
        }
        else
        {
            hr = E_INVALIDARG;
        }
    }
    else if (varStart.lVal > 0 && varStart.lVal <= childCount)
    {
        int current  = varStart.lVal;
        int newIndex = current;

        switch (navDir)
        {
        case NAVDIR_NEXT:
        case NAVDIR_PREVIOUS:
        case NAVDIR_RIGHT:
        case NAVDIR_UP:
            if (current < childCount)
                newIndex = current + 1;
            break;

        case NAVDIR_LEFT:
        case NAVDIR_DOWN:
            if (current > 1)
                newIndex = current - 1;
            break;

        default:
            return E_INVALIDARG;
        }

        SetChildIndex(newIndex, pvarEndUpAt);
        hr = S_OK;
    }

    return hr;
}

STDMETHODIMP Flip3DAccessible::accHitTest(long xLeft, long yTop,
                                          VARIANT* pvarChildAtPoint)
{
    if (!pvarChildAtPoint || !m_accessibleOwnerApp)
        return E_POINTER;

    int index = -1;
    try { index = m_accessibleOwnerApp->AccessibleHitTest(xLeft, yTop); }
    catch (...) { return E_FAIL; }
    if (index >= 0)
    {
        SetChildIndex(index + 1, pvarChildAtPoint);
        return S_OK;
    }

    POINT pt = { (int)xLeft, (int)yTop };
    if (m_accessibleOwnerApp->AccessiblePointInView(pt))
    {
        SetChildIndex(CHILDID_SELF, pvarChildAtPoint);
        return S_OK;
    }

    return S_FALSE;
}

STDMETHODIMP Flip3DAccessible::accDoDefaultAction(VARIANT varChild)
{
    if (varChild.vt != VT_I4 || varChild.lVal == CHILDID_SELF || !m_accessibleOwnerApp)
        return E_INVALIDARG;

    const int index = varChild.lVal - 1;
    if (index < 0 || index >= GetChildrenCount())
        return E_INVALIDARG;

    try { return m_accessibleOwnerApp->AccessibleSelectIndex(index); }
    catch (...) { return E_FAIL; }
}

STDMETHODIMP Flip3DAccessible::put_accName(VARIANT /*varChild*/, BSTR /*szName*/)
{
    return E_NOTIMPL;
}

STDMETHODIMP Flip3DAccessible::put_accValue(VARIANT /*varChild*/, BSTR /*szValue*/)
{
    return E_NOTIMPL;
}


// ===== Ported from src/Flip3DComp.cpp =====
// ============================================================================
// Flip3DComp.cpp — Application lifecycle: Initialize, Run, WndProc, HandleMessage
// ============================================================================
#include <windowsx.h>


namespace { bool ReadDwmShiftSlowdownSetting(); }  // Defined with the animation code below.

// ============================================================================
// Unique window-class name per DLL load.
// Windhawk reloads the mod DLL inside the same windhawk.exe process whenever the
// mod is recompiled/updated. A window class is NOT removed when its DLL is
// unloaded, and the new DLL is often mapped at the same base address, so the
// fixed name "Flip3DCompClass" stayed registered with the OLD WndProc address.
// RegisterClassExW then failed (ERROR_CLASS_ALREADY_EXISTS), CreateWindowExW used
// the stale pointer, and the first message executed unrelated bytes of the new
// DLL -> 0xC000001D (illegal instruction) right after "BuildCards done".
// A per-load name plus explicit UnregisterClassW makes that impossible.
static const wchar_t* Flip3DWindowClassName(HINSTANCE inst)
{
    static wchar_t s_name[64] = {};
    if (!s_name[0])
        wsprintfW(s_name, L"Flip3DCompClass_%08X_%08X",
                  (unsigned)(UINT_PTR)inst, (unsigned)GetTickCount());
    return s_name;
}

// ============================================================================
// Flip3DCompApp::~Flip3DCompApp — final safety net for failed initialization and
// exceptional exits. DComp resources use ComPtr; thumbnail registrations are
// move-only RAII owners; these calls restore the remaining shell state.
Flip3DCompApp::~Flip3DCompApp() noexcept
{
    if (m_windowHandle && IsWindow(m_windowHandle))
        DestroyWindow(m_windowHandle);
    ShutdownAccessibility();
    LeaveFlip3DWindowMode();
    DestroyMonitorBackdrops();
    SetCardTransitionsDisabled(false);
    m_windowCards.clear();
    UnloadThumbApi();
    // The class must not outlive this DLL (see Flip3DWindowClassName).
    if (m_moduleInstance)
        UnregisterClassW(Flip3DWindowClassName(m_moduleInstance), m_moduleInstance);
}

// Flip3DCompApp::EnsureTopmost
// ============================================================================
void Flip3DCompApp::EnsureTopmost()
{
    if (!m_windowHandle || !IsWindow(m_windowHandle))
        return;

    SetWindowPos(m_windowHandle, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

// ============================================================================
// Flip3DCompApp::Initialize
// ============================================================================
bool Flip3DCompApp::Initialize(HINSTANCE hInstance)
{
    m_moduleInstance = hInstance;

    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    // The registry value is read once per session, not on every frame.
    m_isShiftSlowdownSettingEnabled = ReadDwmShiftSlowdownSetting();

    Wh_Log(L"[init] LoadThumbApi");
    if (!LoadThumbApi())
        return false;

    Wh_Log(L"[init] BuildCards");
    BuildCards();
    Wh_Log(L"[init] BuildCards done: %d cards", (int)m_windowCards.size());

    if (!CreateAppWindow())
    {
        if (m_initializationError.empty())
            m_initializationError = L"Failed to create the Flip3D input window.";
        return false;
    }

    Wh_Log(L"[init] window created, UpdateMonitorRect");
    UpdateMonitorRect();

    Wh_Log(L"[init] InitComposition");
    if (FAILED(InitComposition()))
    {
        if (m_initializationError.empty())
            m_initializationError = L"Failed to initialize DirectComposition.";
        return false;
    }

    Wh_Log(L"[init] CreateCardVisuals");
    if (FAILED(CreateCardVisuals()))
    {
        if (m_initializationError.empty())
            m_initializationError = L"Failed to create DWM thumbnail visuals.";
        return false;
    }

    Wh_Log(L"[init] CreateShellBackdrop");
    if (FAILED(CreateShellBackdrop()))
    {
        if (m_initializationError.empty())
            m_initializationError = L"Failed to create the shell desktop backdrop.";
        return false;
    }
    
    m_viewState = ViewState::Enter;
    m_enterExitAnimation.Restart(0.0f, 1.0f, kEnterExitDurationSec,
                        kEnableAnimationEasing
                            ? InterpolationMode::CubicBezier
                            : InterpolationMode::Linear);
    m_previousFrameTime = std::chrono::steady_clock::now();
    m_isOpeningTabPending = (GetAsyncKeyState(VK_TAB) & 0x8000) != 0;
    m_openingTabStartTime = m_isOpeningTabPending ? m_previousFrameTime
                                            : std::chrono::steady_clock::time_point{};

    EnterFlip3DWindowMode();
    InitAccessibility();

    // First-frame layout + DComp commit before ShowWindow (avoids blank flash).
    Wh_Log(L"[init] first Update");
    Update(0.0f);
    Wh_Log(L"[init] done");

    return true;
}

// ============================================================================
// Flip3DCompApp::Run
// ============================================================================
int Flip3DCompApp::Run()
{
    m_isMessageLoopActive = true;
    struct MessageLoopStateGuard {
        bool& active;
        ~MessageLoopStateGuard() noexcept { active = false; }
    } messageLoopGuard{m_isMessageLoopActive};

    MSG msg = {};

    // Keep the overlay above everything, but not with a SetWindowPos call on
    // every pass: once now, then at most twice a second.
    EnsureTopmost();
    ULONGLONG nextTopmostTick = GetTickCount64() + 500;

    while (msg.message != WM_QUIT)
    {
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        if (msg.message == WM_QUIT)
            break;

        if (!m_isMinimized)
        {
            if (GetTickCount64() >= nextTopmostTick)
            {
                EnsureTopmost();
                nextTopmostTick = GetTickCount64() + 500;
            }
            auto now = std::chrono::steady_clock::now();
            float dt = std::chrono::duration<float>(now - m_previousFrameTime).count();
            m_previousFrameTime = now;
            try
            {
                Update(std::min(dt, 0.05f));
            }
            catch (const std::exception& e)
            {
                Wh_Log(L"Exception in Flip3D frame update: %S", e.what());
                if (m_windowHandle && IsWindow(m_windowHandle))
                    DestroyWindow(m_windowHandle);
                else
                    PostQuitMessage(0);
            }
            catch (...)
            {
                Wh_Log(L"Unknown exception in Flip3D frame update");
                if (m_windowHandle && IsWindow(m_windowHandle))
                    DestroyWindow(m_windowHandle);
                else
                    PostQuitMessage(0);
            }

            // IDCompositionDevice::Commit is asynchronous, so without waiting
            // this loop would spin at 100% of a core. DwmFlush blocks until the
            // next composition pass (one pass per display refresh); if it fails
            // (e.g. composition is being torn down) back off briefly instead.
            if (FAILED(DwmFlush()))
                Sleep(8);
        }
        else
        {
            WaitMessage();
            m_previousFrameTime = std::chrono::steady_clock::now();
        }
    }

    UnloadThumbApi();
    return (int)msg.wParam;
}

// ============================================================================
// Flip3DCompApp::WndProc — static window procedure
// ============================================================================
LRESULT CALLBACK Flip3DCompApp::WndProc(HWND hwnd, UINT msg,
                                         WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_NCCREATE)
    {
        auto* self = (Flip3DCompApp*)((CREATESTRUCTW*)lParam)->lpCreateParams;
        if (self)
        {
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)self);
            self->m_windowHandle = hwnd;
        }
    }

    auto* self = reinterpret_cast<Flip3DCompApp*>(
        GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    try {
        return self ? self->HandleMessage(msg, wParam, lParam)
                    : DefWindowProcW(hwnd, msg, wParam, lParam);
    } catch (const std::exception&) {
        Wh_Log(L"C++ exception caught at Flip3D window-procedure boundary");
    } catch (...) {
        Wh_Log(L"Unknown exception caught at Flip3D window-procedure boundary");
    }
    // Never let a C++ exception unwind through a Win32 callback.
    if (IsWindow(hwnd)) PostMessageW(hwnd, WM_CLOSE, 0, 0);
    return 0;
}

// ============================================================================
// Flip3DCompApp::HandleMessage
// ============================================================================
LRESULT Flip3DCompApp::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_TIMER:
        if (wParam != kBuildShellBackdropTimerId)
            break;

        KillTimer(m_windowHandle, kBuildShellBackdropTimerId);
        m_isBackdropBuildPending = false;
        m_isBackdropReady = RebuildMonitorBackdropsIfNeeded() || !m_monitorBackdropLayers.empty();
        if (m_isBackdropReady)
            UpdateBackdropLayout();
        return 0;

    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
        {
            m_isMinimized = true;
            return 0;
        }
        m_isMinimized = false;
        m_viewWidth     = std::max(1u, (UINT)LOWORD(lParam));
        m_viewHeight    = std::max(1u, (UINT)HIWORD(lParam));
        UpdateMonitorRect();
        return 0;

    case WM_DISPLAYCHANGE:
        ApplyFullscreenLayout();
        UpdateMonitorRect();
        return 0;

    case WM_DWMTHUMBNAILSOURCESIZECHANGED:
        // Each registered thumbnail posts 0x327 independently; coalesce to one
        // refresh per frame in Update() (wParam = adapter LUID low part).
        m_areThumbnailsDirty = true;
        return 0;

    case WM_MOUSEWHEEL:
        OnWheel(GET_WHEEL_DELTA_WPARAM(wParam));
        return 0;

    case WM_MOUSEMOVE:
        m_hitWindowHandle = HitTest3DScene(
            (LONG)(short)LOWORD(lParam),
            (LONG)(short)HIWORD(lParam));
        SetCursor(LoadCursorW(nullptr, m_hitWindowHandle ? IDC_HAND : IDC_ARROW));
        return 0;

    case WM_LBUTTONDOWN:
        OnMouse((LONG)(short)LOWORD(lParam),
                (LONG)(short)HIWORD(lParam), true);
        return 0;

    case WM_KEYDOWN:
        if (OnKey(true, (UINT)wParam, lParam))
            return 0;
        break;

    case WM_KEYUP:
        if (OnKey(false, (UINT)wParam, lParam))
            return 0;
        break;

    case WM_CLOSE:
        if (m_viewState == ViewState::Exit ||
            m_viewState == ViewState::ExitRepeatedRotate)
        {
            DestroyWindow(m_windowHandle);
        }
        else
        {
            ExitView();
        }
        return 0;

    case WM_GETOBJECT:
    {
        const DWORD objectId = static_cast<DWORD>(lParam);
        if (objectId != static_cast<DWORD>(OBJID_CLIENT) && objectId != 0)
            break;

        IAccessible* pAccessible = GetAccessibleObject();
        if (!pAccessible)
            break;

        return LresultFromObject(IID_IAccessible, wParam, pAccessible);
    }

    case WM_DESTROY:
        ShutdownAccessibility();
        LeaveFlip3DWindowMode();
        m_windowHandle = nullptr;
        if (m_isMessageLoopActive)
            PostQuitMessage(0);
        return 0;
    }

    if (m_shellHookMessage && msg == m_shellHookMessage)
    {
        OnShellHookMessage(wParam, lParam);
        return 0;
    }

    return DefWindowProcW(m_windowHandle, msg, wParam, lParam);
}


// ===== Ported from src/Flip3DComp_Window.cpp =====
// ============================================================================
// Flip3DComp_Window.cpp — Window creation + enumeration
// ============================================================================
#include <algorithm>

struct Flip3DCompApp::EnumContext
{
    Flip3DCompApp*    app;
    std::vector<HWND> hwnds;
};

// ============================================================================
// Flip3DCompApp::EnumWindowsProc
// ============================================================================
BOOL CALLBACK Flip3DCompApp::EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    auto* ctx = (EnumContext*)lParam;

    if (!hwnd || !ctx->app || hwnd == ctx->app->WindowHandle())
        return TRUE;
    if (ctx->hwnds.size() >= (size_t)kMaxCards)
        return FALSE;
    if (!ctx->app->QualifiesForView(hwnd))
        return TRUE;

    ctx->hwnds.push_back(hwnd);
    return TRUE;
}

// ============================================================================
// Flip3DCompApp::EnumerateWindows
// ============================================================================
std::vector<HWND> Flip3DCompApp::EnumerateWindows()
{
    EnumContext ctx = { this };
    EnumWindows(EnumWindowsProc, (LPARAM)&ctx);

    HWND shell = GetShellWindow();
    if (shell && shell != m_windowHandle && QualifiesForView(shell))
    {
        auto it = std::find(ctx.hwnds.begin(), ctx.hwnds.end(), shell);
        if (it == ctx.hwnds.end() && ctx.hwnds.size() < (size_t)kMaxCards)
            ctx.hwnds.push_back(shell);
    }

    return ctx.hwnds;
}

// ============================================================================
// Flip3DCompApp::ApplyFullscreenLayout
// uDWM EnableInputHooksHelper: WS_POPUP covering m_virtualScreenRect.
// ============================================================================
void Flip3DCompApp::ApplyFullscreenLayout()
{
    if (!m_windowHandle)
        return;

    const int x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    const int y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    const int w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    const int h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    SetWindowPos(m_windowHandle, HWND_TOPMOST, x, y, w, h, SWP_SHOWWINDOW);

    RECT client = {};
    if (GetClientRect(m_windowHandle, &client))
    {
        m_viewWidth  = std::max(1u, (UINT)(client.right  - client.left));
        m_viewHeight = std::max(1u, (UINT)(client.bottom - client.top));
    }

    UpdateMonitorRect();
}

// ============================================================================
// Flip3DCompApp::CreateAppWindow
// uDWM Flip3D input window: borderless popup, topmost, full virtual desktop.
// ============================================================================
bool Flip3DCompApp::CreateAppWindow()
{
    WNDCLASSEXW wc = {
        sizeof(wc),
        CS_HREDRAW | CS_VREDRAW,
        &Flip3DCompApp::WndProc,
        0, 0,
        m_moduleInstance,
        nullptr,
        LoadCursorW(nullptr, IDC_ARROW),
        nullptr, nullptr,
        Flip3DWindowClassName(m_moduleInstance),
        nullptr,
    };
    // Drop any stale registration left by a previous session, then register
    // and verify. Never create the window with a class we did not register.
    UnregisterClassW(wc.lpszClassName, m_moduleInstance);
    if (!RegisterClassExW(&wc))
    {
        m_initializationError = L"Failed to register the Flip3D window class.";
        Wh_Log(L"[init] RegisterClassExW failed (%u)", GetLastError());
        return false;
    }

    const int x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    const int y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    const int w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    const int h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    m_windowHandle = CreateWindowExW(
        WS_EX_NOREDIRECTIONBITMAP | WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        wc.lpszClassName,
        L"",
        WS_POPUP,
        x, y, w, h,
        nullptr, nullptr,
        m_moduleInstance,
        this);

    if (!m_windowHandle)
        return false;

    m_isRightToLeft = (GetWindowLongPtrW(m_windowHandle, GWL_EXSTYLE) & WS_EX_LAYOUTRTL) != 0;

    RECT client = {};
    if (GetClientRect(m_windowHandle, &client))
    {
        m_viewWidth  = std::max(1u, (UINT)(client.right  - client.left));
        m_viewHeight = std::max(1u, (UINT)(client.bottom - client.top));
    }

    return true;
}


// ===== Ported from src/Flip3DComp_Cards.cpp =====
// ============================================================================
// Flip3DComp_Cards.cpp — Card building + thumbnail visual creation + DWM API
// ============================================================================
#include <cmath>

namespace {

MONITORINFO QueryPrimaryMonitor()
{
    MONITORINFO mi = { sizeof(mi) };
    HMONITOR hPrimary = MonitorFromWindow(nullptr, MONITOR_DEFAULTTOPRIMARY);
    if (hPrimary)
        GetMonitorInfoW(hPrimary, &mi);
    return mi;
}

// 2D screen anchor for non-minimized-tile layouts (extended frame or restore rect).
bool FillRestoredScreenRect(HWND h, const MONITORINFO& mi, RECT& out)
{
    bool maximized = IsZoomed(h);

    if (IsIconic(h))
    {
        WINDOWPLACEMENT wp = { sizeof(wp) };
        if (GetWindowPlacement(h, &wp) && !IsRectEmpty(&wp.rcNormalPosition))
        {
            out = wp.rcNormalPosition;
            OffsetRect(&out, mi.rcWork.left, mi.rcWork.top);
            if (wp.flags & WPF_RESTORETOMAXIMIZED)
            {
                maximized = true;
                out = mi.rcWork;
            }
            goto leave;
        }
    }

    DwmGetWindowAttribute(h, DWMWA_EXTENDED_FRAME_BOUNDS, &out, sizeof(out));
leave:
    if (maximized)
    {
        OffsetRect(&out, mi.rcWork.left - out.left, mi.rcWork.top - out.top);
    }

    return true;
}

} // namespace

// ============================================================================
// Flip3DCompApp::LoadThumbApi
// ============================================================================
bool Flip3DCompApp::LoadThumbApi()
{
    m_initializationError.clear();

    m_dwmApiModule = LoadLibraryW(L"dwmapi.dll");
    if (!m_dwmApiModule)
    {
        m_initializationError = L"Failed to load dwmapi.dll.";
        return false;
    }

    m_createSharedThumbnailVisual = (DwmpCreateSharedThumbnailVisual_fn)
        GetProcAddress(m_dwmApiModule, MAKEINTRESOURCEA(147));
    m_queryThumbnailSourceSize = (DwmpQueryWindowThumbnailSourceSize_fn)
        GetProcAddress(m_dwmApiModule, MAKEINTRESOURCEA(162));
    m_getWindowMinimizeRect = (GetWindowMinimizeRect_fn)
        GetProcAddress(GetModuleHandleW(L"user32.dll"), "GetWindowMinimizeRect");

    if (!m_createSharedThumbnailVisual)
    {
        m_initializationError = L"DwmpCreateSharedThumbnailVisual (dwmapi ord 147) is required.";
        UnloadThumbApi();
        return false;
    }
    if (!m_queryThumbnailSourceSize)
    {
        m_initializationError = L"DwmpQueryWindowThumbnailSourceSize (dwmapi ord 162) is required.";
        UnloadThumbApi();
        return false;
    }
    if (!m_getWindowMinimizeRect)
    {
        m_initializationError = L"GetWindowMinimizeRect (user32) is required.";
        UnloadThumbApi();
        return false;
    }

    return true;
}

// ============================================================================
// Flip3DCompApp::UnloadThumbApi
// ============================================================================
void Flip3DCompApp::UnloadThumbApi()
{
    m_createSharedThumbnailVisual = nullptr;
    m_queryThumbnailSourceSize          = nullptr;
    m_getWindowMinimizeRect     = nullptr;

    if (m_dwmApiModule)
    {
        FreeLibrary(m_dwmApiModule);
        m_dwmApiModule = nullptr;
    }
}

// ============================================================================
// Flip3DCompApp::UpdateCardGeometry
// uDWM Flip3DWindow::OnOriginalRectUpdated:
//   - flatBounds in screen pixels (per-window monitor for taskbar/minimize)
//   - NormalizeWindowSize + world mapping via shared PRIMARY rcWork (normMon*)
//   - GetMonitorToWorldTransform on primary m_monitorRect for all cards
// ============================================================================
void Flip3DCompApp::UpdateCardGeometry(CardModel& c, float normMonW, float normMonH,
                                       bool selectedRestore)
{
    HWND h = c.m_windowHandle;
    if (!h)
        return;

    normMonW = std::max(normMonW, 1.0f);
    normMonH = std::max(normMonH, 1.0f);

    c.m_isMinimized    = !!IsIconic(h) && !selectedRestore;
    c.m_isShellDesktop = (h == GetShellWindow());

    HMONITOR mon = MonitorFromWindow(h, MONITOR_DEFAULTTONEAREST);
    if (!mon)
        mon = MonitorFromWindow(nullptr, MONITOR_DEFAULTTOPRIMARY);

    MONITORINFO mi = { sizeof(mi) };
    if (mon)
        GetMonitorInfoW(mon, &mi);
    else
        mi = QueryPrimaryMonitor();

    SIZE srcSize = {};
    if (FAILED(m_queryThumbnailSourceSize(h, FALSE, &srcSize))
        || srcSize.cx < 1 || srcSize.cy < 1)
        return;

    float thumbW = (float)srcSize.cx;
    float thumbH = (float)srcSize.cy;
    const float thumbAspect = thumbH / thumbW;

    RECT flatBounds = {};

    if (c.m_isShellDesktop)
    {
        // uDWM shell: relative origin {0,0} on primary - use primary rcWork.
        MONITORINFO primaryMi = QueryPrimaryMonitor();
        flatBounds = primaryMi.rcWork;
    }
    else if (c.m_isMinimized)
    {
        // 2D minimize destination: taskbar tile position only.
        RECT minRect = {};
        if (m_getWindowMinimizeRect(h, &minRect) && !IsRectEmpty(&minRect))
            flatBounds = Math::BuildFinalMinRect(minRect, thumbAspect);
    }
    else if (!FillRestoredScreenRect(h, mi, flatBounds))
    {
        flatBounds = mi.rcWork;
    }

    if (IsRectEmpty(&flatBounds))
        flatBounds = mi.rcWork;

    c.m_nativeSourceWidth  = (int)thumbW;
    c.m_nativeSourceHeight = (int)thumbH;

    // targetSize / occupancy = 3D carousel (uDWM finalSize).
    Math::WorldSizesFromThumbPixels(
        thumbW, thumbH, normMonW, normMonH,
        c.m_flatSize, c.m_cardTargetSize, c.m_windowOccupancy);

    c.m_windowAspectRatio = thumbW / thumbH;

    const float qualityScale = CardThumbnailQualityScale();
    c.m_thumbnailSourceWidth = std::max(1, (int)std::lround(thumbW * qualityScale));
    c.m_thumbnailSourceHeight = std::max(1, (int)std::lround(thumbH * qualityScale));

    // 2D flat: position from flatBounds; size from QueryThumbSize (restored pixels).
    // Exceptions: shell uses rcWork; iconic minimize uses taskbar tile dimensions.
    float flatW = thumbW;
    float flatH = thumbH;
    if (c.m_isShellDesktop || c.m_isMinimized)
    {
        flatW = (float)std::max(1L, flatBounds.right  - flatBounds.left);
        flatH = (float)std::max(1L, flatBounds.bottom - flatBounds.top);
    }

    c.m_flatSize = { flatW / normMonW, flatH / normMonH };

    float anchorX = (float)flatBounds.left;
    float anchorY = (float)flatBounds.top;
    if (m_isRightToLeft)
    {
        const float relX = anchorX - m_monitorOriginX;
        anchorX = m_monitorOriginX + (normMonW - (flatW + relX));
    }

    float worldX = 0.0f;
    float worldY = 0.0f;
    Math::MonitorToWorldTopLeft(
        anchorX, anchorY,
        m_monitorOriginX, m_monitorOriginY, normMonW, normMonH,
        worldX, worldY);

    c.m_originalPosition = { worldX, worldY, 0.0f };
    c.m_flatPosition     = { worldX, worldY, 0.0f };
}

// ============================================================================
// Flip3DCompApp::UpdateMonitorRect
// uDWM UpdateMonitorRect: normMon from primary rcWork; SetSize uses work-area
// pixels. The input window covers the virtual desktop, but the 3D viewport must
// match rcWork (size + origin), not the full client rect.
// ============================================================================
void Flip3DCompApp::UpdateMonitorRect()
{
    HMONITOR hMon = MonitorFromWindow(nullptr, MONITOR_DEFAULTTOPRIMARY);
    if (!hMon)
        return;

    MONITORINFO mi = { sizeof(mi) };
    if (!GetMonitorInfoW(hMon, &mi))
        return;

    const int vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    const int vy = GetSystemMetrics(SM_YVIRTUALSCREEN);
    m_viewOriginX = (float)(mi.rcWork.left - vx);
    m_viewOriginY = (float)(mi.rcWork.top  - vy);

    const float newMonW     = (float)std::max(1L, mi.rcWork.right  - mi.rcWork.left);
    const float newMonH     = (float)std::max(1L, mi.rcWork.bottom - mi.rcWork.top);
    const float newOriginX  = (float)mi.rcWork.left;
    const float newOriginY  = (float)mi.rcWork.top;

    const bool layoutChanged =
        newMonW    != m_monitorWidth       ||
        newMonH    != m_monitorHeight       ||
        newOriginX != m_monitorOriginX ||
        newOriginY != m_monitorOriginY;

    m_monitorWidth       = newMonW;
    m_monitorHeight       = newMonH;
    m_monitorOriginX = newOriginX;
    m_monitorOriginY = newOriginY;

    if (layoutChanged)
    {
        for (auto& card : m_windowCards)
            UpdateCardGeometry(card, m_monitorWidth, m_monitorHeight);
    }

    if (m_isBackdropReady)
    {
        RebuildMonitorBackdropsIfNeeded();
        UpdateBackdropLayout();
    }
}

// ============================================================================
// Flip3DCompApp::BuildCards
// ============================================================================
void Flip3DCompApp::BuildCards()
{
    m_windowCards.clear();

    if (!m_d3dDevice)
    {
        D3D_FEATURE_LEVEL fl = D3D_FEATURE_LEVEL_11_0;
        HRESULT hr = D3D11CreateDevice(
            nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
            D3D11_CREATE_DEVICE_BGRA_SUPPORT,
            &fl, 1, D3D11_SDK_VERSION,
            &m_d3dDevice, nullptr, nullptr);

        if (FAILED(hr))
        {
            D3D11CreateDevice(
                nullptr, D3D_DRIVER_TYPE_WARP, nullptr,
                D3D11_CREATE_DEVICE_BGRA_SUPPORT,
                &fl, 1, D3D11_SDK_VERSION,
                &m_d3dDevice, nullptr, nullptr);
        }
    }

    auto hwnds = EnumerateWindows();

    MONITORINFO primaryMi = QueryPrimaryMonitor();
    m_monitorWidth       = (float)std::max(1L, primaryMi.rcWork.right  - primaryMi.rcWork.left);
    m_monitorHeight       = (float)std::max(1L, primaryMi.rcWork.bottom - primaryMi.rcWork.top);
    m_monitorOriginX = (float)primaryMi.rcWork.left;
    m_monitorOriginY = (float)primaryMi.rcWork.top;

    int carouselIndex = 0;
    for (auto h : hwnds)
    {
        CardModel c;
        c.m_windowHandle                 = h;
        c.m_initialCarouselIndex = carouselIndex++;
        UpdateCardGeometry(c, m_monitorWidth, m_monitorHeight);
        m_windowCards.push_back(std::move(c));
    }

    m_initialFrontWindowHandle = m_windowCards.empty() ? nullptr : m_windowCards[0].m_windowHandle;
}

// ============================================================================
// Flip3DCompApp::CreateCardVisuals
// ============================================================================
HRESULT Flip3DCompApp::CreateCardVisuals()
{
    if (!m_compositionDevice || !m_sceneCompositionVisual)
        return E_FAIL;

    for (auto& card : m_windowCards)
    {
        if (!card.m_windowHandle || card.m_cardTransformVisual)
            continue;

        if (FAILED(CreateCardVisual(card)))
            continue;
    }

    return S_OK;
}

// ============================================================================
// Flip3DCompApp::UpdateCardThumbnailDest
// Sync DWM thumbnail rcDestination to the current source pixel size.
// ============================================================================
void Flip3DCompApp::UpdateCardThumbnailDest(CardModel& card)
{
    if (!card.m_thumbnailRegistration || card.m_thumbnailSourceWidth <= 0 || card.m_thumbnailSourceHeight <= 0)
        return;

    DWM_THUMBNAIL_PROPERTIES tp = {};
    tp.dwFlags   = DWM_TNP_VISIBLE | DWM_TNP_RECTDESTINATION
                    | DWM_TNP_ENABLE3D | DWM_TNP_DISABLEFORCECVI;
    tp.fVisible  = TRUE;
    tp.rcDestination   = { 0, 0, card.m_thumbnailSourceWidth, card.m_thumbnailSourceHeight };
    DwmUpdateThumbnailProperties(card.m_thumbnailRegistration.get(), &tp);
}

// ============================================================================
// Flip3DCompApp::OnThumbnailSourceSizeChanged
// uDWM CThumbnailVisual::OnSizeChanged posts WM 0x327 to hwndDestination when
// DWM_TNF_DWMWINDOW is set and the live preview bitmap changes size. Every
// card thumbnail posts independently, so the WndProc only sets m_areThumbnailsDirty
// and this runs once per frame, touching cards whose queried source size differs.
// ============================================================================
void Flip3DCompApp::OnThumbnailSourceSizeChanged()
{
    m_areThumbnailsDirty = false;

    bool anyChange = false;
    for (auto& card : m_windowCards)
    {
        if (!card.m_windowHandle)
            continue;

        SIZE querySize = {};
        if (FAILED(m_queryThumbnailSourceSize(card.m_windowHandle, FALSE, &querySize)))
            continue;

        const int queryW = (int)std::max(0L, querySize.cx);
        const int queryH = (int)std::max(0L, querySize.cy);
        if (queryW == card.m_nativeSourceWidth && queryH == card.m_nativeSourceHeight)
            continue;

        const bool selectedRestore = card.m_windowHandle == m_selectedWindowHandle;
        UpdateCardGeometry(card, m_monitorWidth, m_monitorHeight, selectedRestore);
        UpdateCardThumbnailDest(card);
        anyChange = true;
    }

    if (anyChange && m_compositionDevice)
        m_compositionDevice->Commit();
}


// ===== Ported from src/Flip3DComp_Composition.cpp =====
// ============================================================================
// Flip3DComp_Composition.cpp — DirectComposition device init, desktop wash
// ============================================================================
#include <algorithm>
#include <cmath>
#include <cwchar>
#include <vector>
#include <wincodec.h>

namespace {

struct EnumMonitorsContext
{
    std::vector<MONITORINFO> monitors;
};

BOOL CALLBACK EnumMonitorsProc(HMONITOR hMon, HDC, LPRECT, LPARAM lParam)
{
    auto* ctx = reinterpret_cast<EnumMonitorsContext*>(lParam);
    MONITORINFO mi = { sizeof(mi) };
    if (GetMonitorInfoW(hMon, &mi))
        ctx->monitors.push_back(mi);
    return TRUE;
}

int ReadDesktopSetting(const wchar_t* name, int defaultValue)
{
    wchar_t value[32] = {};
    DWORD size = sizeof(value);
    DWORD type = 0;
    if (RegGetValueW(HKEY_CURRENT_USER, L"Control Panel\\Desktop", name,
                     RRF_RT_REG_SZ, &type, value, &size) != ERROR_SUCCESS)
    {
        return defaultValue;
    }

    wchar_t* end = nullptr;
    const long parsed = std::wcstol(value, &end, 10);
    return end != value ? static_cast<int>(parsed) : defaultValue;
}

WallpaperPlacement GetWallpaperPlacement()
{
    if (ReadDesktopSetting(L"TileWallpaper", 0) != 0)
        return WallpaperPlacement::Tile;

    switch (ReadDesktopSetting(L"WallpaperStyle", 2))
    {
    case 0:  return WallpaperPlacement::Center;
    case 6:  return WallpaperPlacement::Fit;
    case 10: return WallpaperPlacement::Fill;
    case 22: return WallpaperPlacement::Span;
    case 2:
    default: return WallpaperPlacement::Stretch;
    }
}

std::vector<MONITORINFO> EnumerateMonitors()
{
    EnumMonitorsContext ctx;
    EnumDisplayMonitors(nullptr, nullptr, EnumMonitorsProc, (LPARAM)&ctx);
    return ctx.monitors;
}

struct TaskbarSearchContext
{
    HMONITOR monitor = nullptr;
    HWND taskbar = nullptr;
};

BOOL CALLBACK FindTaskbarProc(HWND hwnd, LPARAM lParam)
{
    auto* ctx = reinterpret_cast<TaskbarSearchContext*>(lParam);
    wchar_t className[64] = {};
    if (!GetClassNameW(hwnd, className, ARRAYSIZE(className)))
        return TRUE;

    const bool isTaskbar = !_wcsicmp(className, L"Shell_TrayWnd")       
                        || !_wcsicmp(className, L"Shell_SecondaryTrayWnd");
    if (isTaskbar && MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST) == ctx->monitor)
    {
        ctx->taskbar = hwnd;
        return FALSE;
    }
    return TRUE;
}

HWND FindTaskbarForMonitor(HMONITOR monitor)
{
    TaskbarSearchContext ctx = { monitor, nullptr };
    EnumWindows(FindTaskbarProc, reinterpret_cast<LPARAM>(&ctx));
    return ctx.taskbar;
}

HRESULT CreateWallpaperSurface(ID3D11Device* d3d,
                               IDCompositionDesktopDevice* dcomp,
                               ComPtr<IDCompositionSurface>& outSurface,
                               UINT& outWidth,
                               UINT& outHeight)
{
    if (!d3d || !dcomp)
        return E_INVALIDARG;

    struct ComScope
    {
        HRESULT result = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        ~ComScope()
        {
            if (result == S_OK)
                CoUninitialize();
        }
    } comScope;
    if (FAILED(comScope.result) && comScope.result != RPC_E_CHANGED_MODE)
        return comScope.result;

    wchar_t wallpaperPath[MAX_PATH] = {};
    if (!SystemParametersInfoW(SPI_GETDESKWALLPAPER, ARRAYSIZE(wallpaperPath),
                                wallpaperPath, 0)
        || wallpaperPath[0] == L'\0')
    {
        return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
    }

    ComPtr<IWICImagingFactory> factory;
    HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr,
                                  CLSCTX_INPROC_SERVER,
                                  IID_PPV_ARGS(&factory));
    if (FAILED(hr))
        return hr;

    ComPtr<IWICBitmapDecoder> decoder;
    hr = factory->CreateDecoderFromFilename(
        wallpaperPath, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad,
        &decoder);
    if (FAILED(hr))
        return hr;

    ComPtr<IWICBitmapFrameDecode> frame;
    hr = decoder->GetFrame(0, &frame);
    if (FAILED(hr))
        return hr;

    ComPtr<IWICFormatConverter> converter;
    hr = factory->CreateFormatConverter(&converter);
    if (FAILED(hr))
        return hr;

    hr = converter->Initialize(frame.Get(), GUID_WICPixelFormat32bppBGRA,
                               WICBitmapDitherTypeNone, nullptr, 0.0,
                               WICBitmapPaletteTypeCustom);
    if (FAILED(hr))
        return hr;

    UINT width = 0;
    UINT height = 0;
    hr = converter->GetSize(&width, &height);
    if (FAILED(hr) || width == 0 || height == 0)
        return FAILED(hr) ? hr : E_FAIL;

    std::vector<BYTE> pixels(static_cast<size_t>(width) * height * 4);
    hr = converter->CopyPixels(nullptr, width * 4, static_cast<UINT>(pixels.size()),
                               pixels.data());
    if (FAILED(hr))
        return hr;

    ComPtr<IDCompositionSurfaceFactory> surfaceFactory;
    hr = dcomp->CreateSurfaceFactory(d3d, &surfaceFactory);
    if (FAILED(hr))
        return hr;

    ComPtr<IDCompositionSurface> surface;
    hr = surfaceFactory->CreateSurface(width, height, DXGI_FORMAT_B8G8R8A8_UNORM,
                                       DXGI_ALPHA_MODE_IGNORE, &surface);
    if (FAILED(hr))
        return hr;

    ComPtr<IDXGISurface> dxgiSurface;
    POINT offset = {};
    hr = surface->BeginDraw(nullptr, IID_PPV_ARGS(&dxgiSurface), &offset);
    if (FAILED(hr))
        return hr;

    ComPtr<ID3D11Texture2D> destination;
    hr = dxgiSurface.As(&destination);
    if (SUCCEEDED(hr))
    {
        ComPtr<ID3D11DeviceContext> context;
        d3d->GetImmediateContext(&context);
        context->UpdateSubresource(destination.Get(), 0, nullptr,
                                   pixels.data(), width * 4, 0);
        context->Flush();
    }

    HRESULT endDrawHr = surface->EndDraw();
    if (SUCCEEDED(hr))
        hr = endDrawHr;
    if (FAILED(hr))
        return hr;

    outSurface = std::move(surface);
    outWidth = width;
    outHeight = height;
    return S_OK;
}

} // namespace

namespace {

HRESULT CreateSolidSurface(ID3D11Device* d3d,
                           IDCompositionDesktopDevice* dcomp,
                           const float color[4],
                           ComPtr<IDCompositionSurface>& outSurface)
{
    if (!d3d || !dcomp)
        return E_INVALIDARG;

    ComPtr<IDCompositionSurfaceFactory> sf;
    HRESULT hr = dcomp->CreateSurfaceFactory(d3d, &sf);
    if (FAILED(hr))
        return hr;

    ComPtr<IDCompositionSurface> bg;
    hr = sf->CreateSurface(1, 1, DXGI_FORMAT_B8G8R8A8_UNORM,
                           DXGI_ALPHA_MODE_IGNORE, &bg);
    if (FAILED(hr))
        return hr;

    ComPtr<IDXGISurface> dxgiSurf;
    POINT offset = {};
    hr = bg->BeginDraw(nullptr, IID_PPV_ARGS(&dxgiSurf), &offset);
    if (SUCCEEDED(hr))
    {
        ComPtr<ID3D11Texture2D> tex;
        ComPtr<ID3D11RenderTargetView> rtv;
        if (SUCCEEDED(dxgiSurf.As(&tex)) &&
            SUCCEEDED(d3d->CreateRenderTargetView(tex.Get(), nullptr, &rtv)))
        {
            ComPtr<ID3D11DeviceContext> ctx;
            d3d->GetImmediateContext(&ctx);
            ctx->ClearRenderTargetView(rtv.Get(), color);
        }
        bg->EndDraw();
    }

    outSurface = std::move(bg);
    return outSurface ? S_OK : E_FAIL;
}

} // namespace

// ============================================================================
// Flip3DCompApp::InitComposition
// ============================================================================
HRESULT Flip3DCompApp::InitComposition()
{
    if (!m_d3dDevice)
        return E_FAIL;

    ComPtr<IDXGIDevice> dxgi;
    HRESULT hr = m_d3dDevice.As(&dxgi);
    if (FAILED(hr))
        return hr;

    hr = DCompositionCreateDevice2(dxgi.Get(), IID_PPV_ARGS(&m_compositionDevice));
    if (FAILED(hr))
        return hr;

    hr = m_compositionDevice->CreateTargetForHwnd(m_windowHandle, TRUE, &m_compositionTarget);
    if (FAILED(hr))
        return hr;

    ComPtr<IDCompositionVisual2> root;
    hr = m_compositionDevice->CreateVisual(&root);
    if (FAILED(hr))
        return hr;
    m_rootCompositionVisual = root;
    m_compositionTarget->SetRoot(root.Get());

    ComPtr<IDCompositionVisual2> sceneBase;
    hr = m_compositionDevice->CreateVisual(&sceneBase);
    if (FAILED(hr))
        return hr;
    sceneBase.As(&m_sceneCompositionVisual);
    m_sceneCompositionVisual->SetDepthMode(DCOMPOSITION_DEPTH_MODE_TREE);

    ComPtr<IDCompositionVisual> rootBase;
    root.As(&rootBase);
    rootBase->AddVisual(m_sceneCompositionVisual.Get(), FALSE, nullptr);

    if (m_d3dDevice)
    {
        const float wash[4] = { 0.04f, 0.05f, 0.08f, 1.0f };
        CreateSolidSurface(m_d3dDevice.Get(), m_compositionDevice.Get(), wash, m_desktopWashSurface);
    }

    return m_compositionDevice->Commit();
}

// ============================================================================
// Flip3DCompApp::DestroyMonitorBackdrops
// ============================================================================
void Flip3DCompApp::DestroyMonitorBackdrops()
{
    if (!m_rootCompositionVisual)
    {
        m_monitorBackdropLayers.clear();
        return;
    }

    ComPtr<IDCompositionVisual> rootBase;
    if (SUCCEEDED(m_rootCompositionVisual.As(&rootBase)))
    {
        for (auto& mon : m_monitorBackdropLayers)
        {
            if (mon.shellContainer)
                rootBase->RemoveVisual(mon.shellContainer.Get());
            if (mon.washVisual)
                rootBase->RemoveVisual(mon.washVisual.Get());
            if (mon.hShellThumb)
            {
                mon.hShellThumb.reset();
            }
            if (mon.taskbarContainer)
                rootBase->RemoveVisual(mon.taskbarContainer.Get());
            if (mon.wallpaperVisual)
                rootBase->RemoveVisual(mon.wallpaperVisual.Get());
            for (const auto& tile : mon.wallpaperTiles)
            {
                if (tile)
                    rootBase->RemoveVisual(tile.Get());
            }
            if (mon.hTaskbarThumb)
            {
                mon.hTaskbarThumb.reset();
            }
            if (mon.startContainer)
                rootBase->RemoveVisual(mon.startContainer.Get());
            if (mon.hStartThumb)
            {
                mon.hStartThumb.reset();
            }
        }
    }

    m_monitorBackdropLayers.clear();
}

// ============================================================================
// Flip3DCompApp::UpdateBackdropLayout
// Per-monitor wash (rcMonitor) and shell thumbnail (rcWork crop).
// Client coordinates match the virtual-desktop–sized Flip3D window.
// ============================================================================
void Flip3DCompApp::UpdateBackdropLayout()
{
    const int vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    const int vy = GetSystemMetrics(SM_YVIRTUALSCREEN);

    HWND shell = GetShellWindow();
    RECT shellWnd = {};
    if (shell)
        GetWindowRect(shell, &shellWnd);

    for (auto& mon : m_monitorBackdropLayers)
    {
        const LONG washW = mon.rcMonitor.right - mon.rcMonitor.left;
        const LONG washH = mon.rcMonitor.bottom - mon.rcMonitor.top;
        const float washX = (float)(mon.rcMonitor.left - vx);
        const float washY = (float)(mon.rcMonitor.top  - vy);

        if (mon.washVisual)
        {
            ComPtr<IDCompositionVisual2> wash2;
            if (SUCCEEDED(mon.washVisual.As(&wash2)))
            {
                const D2D_MATRIX_3X2_F xform = {
                    (float)std::max(washW, 1L), 0.f,
                    0.f, (float)std::max(washH, 1L),
                    washX, washY,
                };
                wash2->SetTransform(xform);
            }
        }

        const float shellX = (float)(mon.rcMonitor.left - vx);
        const float shellY = (float)(mon.rcMonitor.top  - vy);

        if (mon.shellContainer)
        {
            ComPtr<IDCompositionVisual2> shell2;
            if (SUCCEEDED(mon.shellContainer.As(&shell2)))
            {
                const D2D_MATRIX_3X2_F xform = {
                    1.f, 0.f,
                    0.f, 1.f,
                    shellX, shellY,
                };
                shell2->SetTransform(xform);
            }
        }

        if (mon.wallpaperVisual)
        {
            ComPtr<IDCompositionVisual2> wallpaper2;
            if (SUCCEEDED(mon.wallpaperVisual.As(&wallpaper2)))
            {
                const float imageW = (float)std::max(mon.wallpaperWidth, 1u);
                const float imageH = (float)std::max(mon.wallpaperHeight, 1u);
                float scaleX = 1.0f;
                float scaleY = 1.0f;
                float wallpaperX = washX;
                float wallpaperY = washY;

                switch (mon.wallpaperPlacement)
                {
                case WallpaperPlacement::Center:
                    wallpaperX += (washW - imageW) * 0.5f;
                    wallpaperY += (washH - imageH) * 0.5f;
                    break;
                case WallpaperPlacement::Fit:
                {
                    const float scale = std::min(washW / imageW, washH / imageH);
                    scaleX = scaleY = scale;
                    wallpaperX += (washW - imageW * scale) * 0.5f;
                    wallpaperY += (washH - imageH * scale) * 0.5f;
                    break;
                }
                case WallpaperPlacement::Fill:
                case WallpaperPlacement::Span:
                {
                    const float scale = std::max(washW / imageW, washH / imageH);
                    scaleX = scaleY = scale;
                    wallpaperX += (washW - imageW * scale) * 0.5f;
                    wallpaperY += (washH - imageH * scale) * 0.5f;
                    break;
                }
                case WallpaperPlacement::Stretch:
                    scaleX = washW / imageW;
                    scaleY = washH / imageH;
                    break;
                case WallpaperPlacement::Tile:
                {
                    const LONG rootX = mon.rcMonitor.left - vx;
                    const LONG rootY = mon.rcMonitor.top - vy;
                    const LONG tileW = static_cast<LONG>(mon.wallpaperWidth);
                    const LONG tileH = static_cast<LONG>(mon.wallpaperHeight);
                    wallpaperX = (float)(rootX - ((rootX % tileW) + tileW) % tileW);
                    wallpaperY = (float)(rootY - ((rootY % tileH) + tileH) % tileH);
                    break;
                }
                }

                const D2D_MATRIX_3X2_F xform = {
                    scaleX, 0.f,
                    0.f, scaleY,
                    wallpaperX, wallpaperY,
                };
                wallpaper2->SetTransform(xform);

                for (size_t i = 0; i < mon.wallpaperTiles.size(); ++i)
                {
                    ComPtr<IDCompositionVisual2> tile2;
                    if (i < mon.wallpaperTileOrigins.size()
                        && SUCCEEDED(mon.wallpaperTiles[i].As(&tile2)))
                    {
                        const POINT origin = mon.wallpaperTileOrigins[i];
                        const D2D_MATRIX_3X2_F tileTransform = {
                            1.f, 0.f,
                            0.f, 1.f,
                            (float)origin.x, (float)origin.y,
                        };
                        tile2->SetTransform(tileTransform);
                    }
                }
            }
        }

        if (mon.taskbarContainer)
        {
            ComPtr<IDCompositionVisual2> taskbar2;
            if (SUCCEEDED(mon.taskbarContainer.As(&taskbar2)))
            {
                const D2D_MATRIX_3X2_F xform = {
                    1.f, 0.f,
                    0.f, 1.f,
                    (float)(mon.rcTaskbar.left - vx),
                    (float)(mon.rcTaskbar.top - vy),
                };
                taskbar2->SetTransform(xform);
            }
        }

        if (mon.startContainer)
        {
            ComPtr<IDCompositionVisual2> start2;
            if (SUCCEEDED(mon.startContainer.As(&start2)))
            {
                const D2D_MATRIX_3X2_F xform = {
                    1.f, 0.f,
                    0.f, 1.f,
                    (float)(mon.rcStart.left - vx),
                    (float)(mon.rcStart.top - vy),
                };
                start2->SetTransform(xform);
            }
        }

        const LONG startW = mon.rcStart.right - mon.rcStart.left;
        const LONG startH = mon.rcStart.bottom - mon.rcStart.top;
        if (mon.hStartThumb && startW > 0 && startH > 0)
        {
            DWM_THUMBNAIL_PROPERTIES tp = {};
            tp.dwFlags = DWM_TNP_VISIBLE | DWM_TNP_RECTDESTINATION
                       | DWM_TNP_RECTSOURCE | DWM_TNP_DISABLEFORCECVI;
            tp.fVisible = TRUE;
            tp.rcSource = { 0, 0, startW, startH };
            tp.rcDestination = tp.rcSource;
            DwmUpdateThumbnailProperties(mon.hStartThumb.get(), &tp);
        }

        const LONG shellW = mon.rcMonitor.right - mon.rcMonitor.left;
        const LONG shellH = mon.rcMonitor.bottom - mon.rcMonitor.top;
        if (mon.hShellThumb && shellW > 0 && shellH > 0)
        {
            RECT rcSource = mon.rcMonitor;
            OffsetRect(&rcSource, -shellWnd.left, -shellWnd.top);

            DWM_THUMBNAIL_PROPERTIES tp = {};
            tp.dwFlags   = DWM_TNP_VISIBLE | DWM_TNP_RECTDESTINATION | DWM_TNP_RECTSOURCE
                         | DWM_TNP_DISABLEFORCECVI;
            tp.fVisible  = TRUE;
            tp.rcSource       = rcSource;
            tp.rcDestination  = {
                0, 0,
                mon.rcMonitor.right - mon.rcMonitor.left,
                mon.rcMonitor.bottom - mon.rcMonitor.top,
            };
            DwmUpdateThumbnailProperties(mon.hShellThumb.get(), &tp);
        }

        const LONG taskbarW = mon.rcTaskbar.right - mon.rcTaskbar.left;
        const LONG taskbarH = mon.rcTaskbar.bottom - mon.rcTaskbar.top;
        if (mon.hTaskbarThumb && taskbarW > 0 && taskbarH > 0)
        {
            DWM_THUMBNAIL_PROPERTIES tp = {};
            tp.dwFlags = DWM_TNP_VISIBLE | DWM_TNP_RECTDESTINATION
                       | DWM_TNP_RECTSOURCE | DWM_TNP_DISABLEFORCECVI;
            tp.fVisible = TRUE;
            tp.rcSource = { 0, 0, taskbarW, taskbarH };
            tp.rcDestination = { 0, 0, taskbarW, taskbarH };
            DwmUpdateThumbnailProperties(mon.hTaskbarThumb.get(), &tp);
        }
    }

    if (m_compositionDevice)
        m_compositionDevice->Commit();
}

// ============================================================================
// Flip3DCompApp::RebuildMonitorBackdropsIfNeeded
// ============================================================================
bool Flip3DCompApp::RebuildMonitorBackdropsIfNeeded()
{
    const std::vector<MONITORINFO> monitors = EnumerateMonitors();
    if (monitors.empty())
        return false;

    bool layoutSame = monitors.size() == m_monitorBackdropLayers.size();
    if (layoutSame)
    {
        for (size_t i = 0; i < monitors.size(); ++i)
        {
            const MONITORINFO& a = monitors[i];
            const MonitorBackdrop& b = m_monitorBackdropLayers[i];
            if (a.rcMonitor.left   != b.rcMonitor.left
             || a.rcMonitor.top    != b.rcMonitor.top
             || a.rcMonitor.right  != b.rcMonitor.right
             || a.rcMonitor.bottom != b.rcMonitor.bottom
             || a.rcWork.left      != b.rcWork.left
             || a.rcWork.top       != b.rcWork.top
             || a.rcWork.right     != b.rcWork.right
             || a.rcWork.bottom    != b.rcWork.bottom)
            {
                layoutSame = false;
                break;
            }
        }
    }

    if (layoutSame)
    {
        UpdateBackdropLayout();
        return false;
    }

    DestroyMonitorBackdrops();

    if (!m_desktopWashSurface && m_d3dDevice && m_compositionDevice)
    {
        const float wash[4] = { 0.04f, 0.05f, 0.08f, 1.0f };
        CreateSolidSurface(m_d3dDevice.Get(), m_compositionDevice.Get(), wash, m_desktopWashSurface);
    }

    HWND shell = GetShellWindow();
    if (!m_compositionDevice || !m_rootCompositionVisual || !m_desktopWashSurface)
        return false;

    ComPtr<IDCompositionVisual> rootBase;
    if (FAILED(m_rootCompositionVisual.As(&rootBase)))
        return false;

    ComPtr<IDCompositionSurface> wallpaperSurface;
    UINT wallpaperWidth = 0;
    UINT wallpaperHeight = 0;
    if (!shell
        && FAILED(CreateWallpaperSurface(m_d3dDevice.Get(), m_compositionDevice.Get(),
                                          wallpaperSurface, wallpaperWidth,
                                          wallpaperHeight)))
    {
        const float black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
        if (FAILED(CreateSolidSurface(m_d3dDevice.Get(), m_compositionDevice.Get(),
                                      black, wallpaperSurface)))
            return false;
        wallpaperWidth = 1;
        wallpaperHeight = 1;
    }

    RECT shellWnd = {};
    if (shell)
        GetWindowRect(shell, &shellWnd);

    for (const MONITORINFO& mi : monitors)
    {
        MonitorBackdrop mon = {};
        mon.rcMonitor = mi.rcMonitor;
        mon.rcWork    = mi.rcWork;

        const LONG shellW = mon.rcWork.right - mon.rcWork.left;
        const LONG shellH = mon.rcWork.bottom - mon.rcWork.top;
        if (shellW <= 0 || shellH <= 0)
            continue;

        HRESULT hr = S_OK;
        if (shell)
        {
            RECT rcSource = mon.rcMonitor;
            OffsetRect(&rcSource, -shellWnd.left, -shellWnd.top);

            DWM_THUMBNAIL_PROPERTIES tp = {};
            tp.dwFlags   = DWM_TNP_VISIBLE | DWM_TNP_RECTDESTINATION | DWM_TNP_RECTSOURCE
                         | DWM_TNP_DISABLEFORCECVI;
            tp.fVisible  = TRUE;
            tp.rcSource      = rcSource;
            tp.rcDestination = {
                0, 0,
                mon.rcMonitor.right - mon.rcMonitor.left,
                mon.rcMonitor.bottom - mon.rcMonitor.top,
            };

            void* pv = nullptr;
            hr = m_createSharedThumbnailVisual(
                m_windowHandle,
                shell,
                DWM_TNF_DWMWINDOW,
                &tp,
                m_compositionDevice.Get(),
                &pv,
                mon.hShellThumb.put());

            if (FAILED(hr) || !pv)
                continue;

            HWND taskbar = FindTaskbarForMonitor(MonitorFromRect(&mon.rcMonitor,
                                                                 MONITOR_DEFAULTTONEAREST));
            if (taskbar)
            {
            RECT taskbarRect = {};
            if (GetWindowRect(taskbar, &taskbarRect)
                && taskbarRect.right > taskbarRect.left
                && taskbarRect.bottom > taskbarRect.top)
            {
                DWM_THUMBNAIL_PROPERTIES taskbarProperties = {};
                taskbarProperties.dwFlags = DWM_TNP_VISIBLE | DWM_TNP_RECTDESTINATION
                                           | DWM_TNP_RECTSOURCE | DWM_TNP_DISABLEFORCECVI;
                taskbarProperties.fVisible = TRUE;
                taskbarProperties.rcSource = {
                    0, 0,
                    taskbarRect.right - taskbarRect.left,
                    taskbarRect.bottom - taskbarRect.top,
                };
                taskbarProperties.rcDestination = taskbarProperties.rcSource;

                ScopedDwmThumbnail taskbarThumbnail;
                void* taskbarPv = nullptr;
                hr = m_createSharedThumbnailVisual(
                    m_windowHandle, taskbar, DWM_TNF_DWMWINDOW, &taskbarProperties,
                    m_compositionDevice.Get(), &taskbarPv, taskbarThumbnail.put());
                if (SUCCEEDED(hr) && taskbarPv)
                {
                    mon.rcTaskbar = taskbarRect;
                    mon.hTaskbarThumb = std::move(taskbarThumbnail);

                    ComPtr<IDCompositionVisual3> taskbarVisual;
                    taskbarVisual.Attach((IDCompositionVisual3*)taskbarPv);
                    mon.taskbarThumb = taskbarVisual;
                    ComPtr<IDCompositionVisual2> taskbarContainer;
                    hr = m_compositionDevice->CreateVisual(&taskbarContainer);
                    if (SUCCEEDED(hr))
                        hr = taskbarContainer.As(&mon.taskbarContainer);
                    if (SUCCEEDED(hr))
                        hr = mon.taskbarContainer->AddVisual(mon.taskbarThumb.Get(), FALSE, nullptr);
                    if (FAILED(hr))
                    {
                        mon.hTaskbarThumb.reset();
                        mon.taskbarThumb.Reset();
                        mon.taskbarContainer.Reset();
                        mon.rcTaskbar = {};
                    }
                }
            }
            }

            try { TryCreateStartButtonThumbnail(mon); }
            catch (...) { Wh_Log(L"Exception while creating the Start button thumbnail"); }

            ComPtr<IDCompositionVisual> thumbBase;
            thumbBase.Attach((IDCompositionVisual*)pv);
            hr = thumbBase.As(&mon.shellThumb);
            if (FAILED(hr))
            {
                mon.hShellThumb.reset();
                continue;
            }

            ComPtr<IDCompositionVisual2> shellContainer;
            hr = m_compositionDevice->CreateVisual(&shellContainer);
            if (FAILED(hr))
                continue;
            hr = shellContainer.As(&mon.shellContainer);
            if (FAILED(hr))
                continue;

            hr = mon.shellContainer->AddVisual(mon.shellThumb.Get(), FALSE, nullptr);
            if (FAILED(hr))
                continue;
        }
        else
        {
            ComPtr<IDCompositionVisual2> wallpaperVisual;
            HRESULT hr = m_compositionDevice->CreateVisual(&wallpaperVisual);
            if (FAILED(hr) || FAILED(wallpaperVisual->SetContent(wallpaperSurface.Get())))
                continue;
            if (FAILED(wallpaperVisual.As(&mon.wallpaperVisual)))
                continue;
            mon.wallpaperSurface = wallpaperSurface;
            mon.wallpaperWidth = wallpaperWidth;
            mon.wallpaperHeight = wallpaperHeight;
            mon.wallpaperPlacement = GetWallpaperPlacement();

            if (mon.wallpaperPlacement == WallpaperPlacement::Tile)
            {
                const int vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
                const int vy = GetSystemMetrics(SM_YVIRTUALSCREEN);
                const LONG monitorWidth = mi.rcMonitor.right - mi.rcMonitor.left;
                const LONG monitorHeight = mi.rcMonitor.bottom - mi.rcMonitor.top;
                const LONG imageWidth = static_cast<LONG>(wallpaperWidth);
                const LONG imageHeight = static_cast<LONG>(wallpaperHeight);
                const LONG rootX = mi.rcMonitor.left - vx;
                const LONG rootY = mi.rcMonitor.top - vy;
                const LONG firstX = rootX - ((rootX % imageWidth) + imageWidth) % imageWidth;
                const LONG firstY = rootY - ((rootY % imageHeight) + imageHeight) % imageHeight;

                for (LONG y = firstY; y < rootY + monitorHeight; y += imageHeight)
                {
                    for (LONG x = firstX; x < rootX + monitorWidth; x += imageWidth)
                    {
                        if (x == firstX && y == firstY)
                            continue;

                        ComPtr<IDCompositionVisual2> tileVisual;
                        if (FAILED(m_compositionDevice->CreateVisual(&tileVisual))
                            || FAILED(tileVisual->SetContent(wallpaperSurface.Get())))
                            continue;

                        ComPtr<IDCompositionVisual3> tileVisual3;
                        if (FAILED(tileVisual.As(&tileVisual3)))
                            continue;
                        mon.wallpaperTiles.push_back(tileVisual3);
                        mon.wallpaperTileOrigins.push_back({ x, y });
                    }
                }
            }
        }

        ComPtr<IDCompositionVisual2> washVis;
        hr = m_compositionDevice->CreateVisual(&washVis);
        if (FAILED(hr))
            continue;
        hr = washVis->SetContent(m_desktopWashSurface.Get());
        if (FAILED(hr))
            continue;
        hr = washVis.As(&mon.washVisual);
        if (FAILED(hr))
            continue;

        // Z-order back→front: shell desktop, wash, then scene (added first in InitComposition).
        if (mon.shellContainer)
            rootBase->AddVisual(mon.shellContainer.Get(), FALSE, m_sceneCompositionVisual.Get());
        if (mon.wallpaperVisual)
        {
            ComPtr<IDCompositionVisual> wallpaperBase;
            if (SUCCEEDED(mon.wallpaperVisual.As(&wallpaperBase)))
                rootBase->AddVisual(wallpaperBase.Get(), FALSE, m_sceneCompositionVisual.Get());
        }
        for (const auto& tile : mon.wallpaperTiles)
        {
            if (tile)
                rootBase->AddVisual(tile.Get(), FALSE, m_sceneCompositionVisual.Get());
        }
        if (mon.taskbarContainer)
            rootBase->AddVisual(mon.taskbarContainer.Get(), FALSE, m_sceneCompositionVisual.Get());
        if (mon.startContainer)
            rootBase->AddVisual(mon.startContainer.Get(), FALSE, m_sceneCompositionVisual.Get());
        rootBase->AddVisual(mon.washVisual.Get(), FALSE, m_sceneCompositionVisual.Get());

        if (mon.shellContainer)
            mon.shellContainer->SetOpacity(1.0f);
        m_monitorBackdropLayers.push_back(std::move(mon));
    }

    UpdateBackdropLayout();
    return true;
}


// ============================================================================
// Start button support (StartAllBack, ExplorerPatcher, classic shells).
// The orb is often a separate top-level window sitting on top of the taskbar,
// so the Shell_TrayWnd thumbnail does not contain it. Find it by geometry
// (fully inside the taskbar rect) + name, and mirror it with its own thumbnail.
// ============================================================================
namespace {

struct StartSearchContext
{
    RECT  taskbar = {};
    HWND  self    = nullptr;
    HWND  hit     = nullptr;
    bool  logCandidates = false;
};

bool IsInsideRectWithPad(const RECT& r, const RECT& outer, int pad)
{
    return r.left >= outer.left - pad && r.top >= outer.top - pad
        && r.right <= outer.right + pad && r.bottom <= outer.bottom + pad;
}

BOOL CALLBACK FindStartButtonProc(HWND hwnd, LPARAM lParam)
{
    auto* ctx = reinterpret_cast<StartSearchContext*>(lParam);
    if (!ctx || hwnd == ctx->self || !IsWindowVisible(hwnd))
        return TRUE;

    RECT r = {};
    if (!GetWindowRect(hwnd, &r) || IsRectEmpty(&r))
        return TRUE;
    if (!IsInsideRectWithPad(r, ctx->taskbar, 6))
        return TRUE;

    wchar_t cls[128] = {};
    wchar_t title[128] = {};
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    GetWindowTextW(hwnd, title, ARRAYSIZE(title));

    if (!_wcsicmp(cls, L"Shell_TrayWnd") || !_wcsicmp(cls, L"Shell_SecondaryTrayWnd"))
        return TRUE;

    if (ctx->logCandidates)
        Wh_Log(L"[start] window inside taskbar: class=\"%s\" title=\"%s\" rect=%d,%d,%d,%d",
               cls, title, (int)r.left, (int)r.top, (int)r.right, (int)r.bottom);

    wchar_t lowered[128] = {};
    for (size_t i = 0; i < ARRAYSIZE(lowered) - 1 && cls[i]; ++i)
        lowered[i] = (wchar_t)towlower(cls[i]);

    const bool nameMatch = wcsstr(lowered, L"start") != nullptr
                        || !_wcsicmp(title, L"Start");
    if (!nameMatch)
        return TRUE;

    // The button is small compared to the whole taskbar.
    const LONG tbW = ctx->taskbar.right - ctx->taskbar.left;
    const LONG tbH = ctx->taskbar.bottom - ctx->taskbar.top;
    if ((r.right - r.left) > std::max(tbW, tbH) / 2)
        return TRUE;

    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked)))
        && cloaked != 0)
        return TRUE;

    ctx->hit = hwnd;
    return FALSE;
}

} // namespace

void Flip3DCompApp::TryCreateStartButtonThumbnail(MonitorBackdrop& mon)
{
    if (mon.rcTaskbar.right <= mon.rcTaskbar.left || mon.rcTaskbar.bottom <= mon.rcTaskbar.top)
        return;
    if (!m_compositionDevice || !m_createSharedThumbnailVisual)
        return;

    StartSearchContext ctx;
    ctx.taskbar = mon.rcTaskbar;
    ctx.self    = m_windowHandle;
    EnumWindows(FindStartButtonProc, reinterpret_cast<LPARAM>(&ctx));

    if (!ctx.hit)
    {
        // Diagnostics: list everything that lives inside the taskbar so the
        // right class name can be identified from the Windhawk log.
        ctx.logCandidates = true;
        Wh_Log(L"[start] no Start button window found, listing windows inside the taskbar:");
        EnumWindows(FindStartButtonProc, reinterpret_cast<LPARAM>(&ctx));
        return;
    }

    RECT r = {};
    if (!GetWindowRect(ctx.hit, &r) || IsRectEmpty(&r))
        return;

    DWM_THUMBNAIL_PROPERTIES tp = {};
    tp.dwFlags = DWM_TNP_VISIBLE | DWM_TNP_RECTDESTINATION
               | DWM_TNP_RECTSOURCE | DWM_TNP_DISABLEFORCECVI;
    tp.fVisible = TRUE;
    tp.rcSource = { 0, 0, r.right - r.left, r.bottom - r.top };
    tp.rcDestination = tp.rcSource;

    ScopedDwmThumbnail thumb;
    void* pv = nullptr;
    HRESULT hr = m_createSharedThumbnailVisual(
        m_windowHandle, ctx.hit, DWM_TNF_DWMWINDOW, &tp,
        m_compositionDevice.Get(), &pv, thumb.put());
    if (FAILED(hr) || !pv)
    {
        Wh_Log(L"[start] thumbnail creation failed (hr=0x%08X)", (unsigned)hr);
        return;
    }

    ComPtr<IDCompositionVisual> baseVisual;
    baseVisual.Attach((IDCompositionVisual*)pv);
    ComPtr<IDCompositionVisual3> startVisual;
    if (FAILED(baseVisual.As(&startVisual)))
        return;

    ComPtr<IDCompositionVisual2> container;
    if (FAILED(m_compositionDevice->CreateVisual(&container)))
        return;
    ComPtr<IDCompositionVisual3> container3;
    if (FAILED(container.As(&container3)))
        return;
    if (FAILED(container3->AddVisual(startVisual.Get(), FALSE, nullptr)))
        return;

    mon.rcStart         = r;
    mon.hStartThumb     = std::move(thumb);
    mon.startThumb      = startVisual;
    mon.startContainer  = container3;
    Wh_Log(L"[start] Start button mirrored (hwnd=%p)", ctx.hit);
}

// ============================================================================
// Flip3DCompApp::CreateShellBackdrop
// ============================================================================
HRESULT Flip3DCompApp::CreateShellBackdrop()
{
    if (m_isBackdropReady || m_isBackdropBuildPending)
        return S_OK;

    m_isBackdropBuildPending = true;
    if (!SetTimer(m_windowHandle, kBuildShellBackdropTimerId, 0, nullptr))
    {
        m_isBackdropBuildPending = false;
        return HRESULT_FROM_WIN32(GetLastError());
    }

    return S_OK;
}


// ===== Ported from src/Flip3DComp_Update.cpp =====
// ============================================================================
// Flip3DComp_Update.cpp — Per-frame update, camera, card transforms, scroll
// ============================================================================
//
// Carousel uses fractional smooth scroll (m_scrollPosition) for browse.
// Each card's m_carouselDisplaySlot is the single render coordinate during exit;
// browse computes it on the fly (CaptureCarouselVisualSlot), exit updates it
// in UpdateVisualSlots once per frame.
//
// ============================================================================
#include <algorithm>
#include <cmath>
#include <winreg.h>
#include <vector>

namespace
{
bool ReadDwmShiftSlowdownSetting()
{
    DWORD enabled = 0;
    DWORD size = sizeof(enabled);
    const LSTATUS status = RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\DWM",
        L"AnimationsShiftKey",
        RRF_RT_REG_DWORD,
        nullptr,
        &enabled,
        &size);

    return status == ERROR_SUCCESS && enabled != 0;
}
} // This makes Flip3D compatible with the dwm slow animations DWORD Value like it was on Vista/7

// ============================================================================
// Flip3DCompApp::EnterProgress
// ============================================================================
float Flip3DCompApp::EnterProgress() const
{
    return std::max(std::min(m_enterExitAnimation.Value(), 1.0f), 0.0f);
}

// ============================================================================
// Flip3DCompApp::ReplayEnterAnimation
// ============================================================================
void Flip3DCompApp::ReplayEnterAnimation()
{
    m_viewState = ViewState::Enter;
    m_enterExitAnimation.Restart(0.0f, 1.0f, kEnterExitDurationSec,
                        kEnableAnimationEasing
                            ? InterpolationMode::CubicBezier
                            : InterpolationMode::Linear);
    m_scrollPosition           = 0.0f;
    m_scrollTargetPosition        = 0.0f;
    m_wrapScrollAdjustmentThisFrame = 0.0f;
    m_heldNavigationVirtualKey   = 0;
    m_heldNavigationDirection = 0;
    m_heldNavigationStartTime = {};
    m_repeatedRotateStepsRemaining = 0;
    m_isOpeningTabPending = false;
    m_openingTabStartTime = {};
    m_selectedWindowHandle        = nullptr;
    m_repeatedRotateRate = 0.0f;
    m_showOutgoingCardDuringRotation = false;
    m_exitScrollSnapshot    = 0.0f;
    m_lastPaintOrder.clear();
    m_initialFrontWindowHandle   = m_windowCards.empty() ? nullptr : m_windowCards[0].m_windowHandle;
    InvalidateDisplaySlots();
}

// ============================================================================
// Flip3DCompApp::RotateListPhysically
// List splice when committing fractional scroll to discrete slots (exit only).
// ============================================================================
void Flip3DCompApp::RotateListPhysically(bool backward)
{
    if (m_windowCards.size() <= 1)
        return;

    if (backward)
        std::rotate(m_windowCards.rbegin(), m_windowCards.rbegin() + 1, m_windowCards.rend());
    else
        std::rotate(m_windowCards.begin(), m_windowCards.begin() + 1, m_windowCards.end());
}

// ============================================================================
// Flip3DCompApp::WrapCarouselScroll
// Keep scrollPos in (-0.5, 0.5) by rotating the list at integer boundaries.
// Forward wrap decouples the outgoing card's display slot so it can finish the
// front-edge opacity ramp (slot in [-1, 0]) without jumping to the back.
// ============================================================================
void Flip3DCompApp::WrapCarouselScroll()
{
    if (m_windowCards.size() <= 1)
        return;

    m_wrapScrollAdjustmentThisFrame = 0.0f;

    const int N = (int)m_windowCards.size();
    int guard = 0;
    while (m_scrollPosition >= 0.5f && guard++ <= N)
    {
        const HWND  outgoingHwnd = m_windowCards[0].m_windowHandle;
        const float outgoingSlot = GetCardDisplaySlot(0);

        RotateListPhysically(false);
        m_scrollPosition    -= 1.0f;
        m_scrollTargetPosition -= 1.0f;
        m_wrapScrollAdjustmentThisFrame -= 1.0f;

        OnCarouselWrapForward(outgoingHwnd, outgoingSlot);
    }
    guard = 0;
    while (m_scrollPosition <= -0.5f && guard++ <= N)
    {
        const HWND incomingHwnd = m_windowCards[(size_t)(N - 1)].m_windowHandle;

        RotateListPhysically(true);
        m_scrollPosition    += 1.0f;
        m_scrollTargetPosition += 1.0f;
        m_wrapScrollAdjustmentThisFrame += 1.0f;

        OnCarouselWrapBackward(incomingHwnd);
    }
}

// ============================================================================
// Display slot helpers — continuous carousel coordinates per card
// ============================================================================
float Flip3DCompApp::CardListSlot(int listIndex) const
{
    return (float)listIndex - m_scrollPosition;
}

float Flip3DCompApp::CaptureCarouselVisualSlot(int listIndex) const
{
    if (listIndex < 0 || listIndex >= (int)m_windowCards.size())
        return 0.0f;

    const CardModel& c = m_windowCards[(size_t)listIndex];

    if (c.m_carouselWrapPhase == CarouselWrapPhase::EnteringBack)
        return CardListSlot(listIndex);

    if (c.m_carouselWrapPhase != CarouselWrapPhase::None)
        return c.m_carouselDisplaySlot;

    return CardListSlot(listIndex);
}

float Flip3DCompApp::GetCardDisplaySlot(int listIndex) const
{
    if (listIndex < 0 || listIndex >= (int)m_windowCards.size())
        return 0.0f;

    const CardModel& c = m_windowCards[(size_t)listIndex];

    if (m_viewState == ViewState::Exit || m_viewState == ViewState::ExitRepeatedRotate)
        return c.m_carouselDisplaySlot;

    return CaptureCarouselVisualSlot(listIndex);
}

void Flip3DCompApp::FinishWrapPhase(CardModel& card, int listIndex)
{
    card.m_carouselWrapPhase              = CarouselWrapPhase::None;
    card.m_wrapFadeStartListSlot  = 0.0f;
    card.m_carouselDisplaySlot            = CardListSlot(listIndex);
    card.m_hasValidCarouselDisplaySlot       = true;
    card.m_wrapFadeProgress           = 1.0f;
}

void Flip3DCompApp::BeginEnteringBackWrap(CardModel& card, int listIndex)
{
    card.m_carouselWrapPhase             = CarouselWrapPhase::EnteringBack;
    card.m_wrapFadeStartListSlot = CardListSlot(listIndex);
    card.m_carouselDisplaySlot           = CarouselEdgeSpan();
    card.m_hasValidCarouselDisplaySlot      = true;
    card.m_wrapFadeProgress          = 0.0f;
    // Start fully transparent; the rendered opacity will fade toward the
    // configured back-card target instead of appearing at the minimum value.
    card.m_wrapOpacity           = 0.0f;
    card.m_isWrapFadeActive        = true;
}

void Flip3DCompApp::StepEnteringBackWrap(CardModel& card, int listIndex)
{
    const float listSlot  = CardListSlot(listIndex);
    const float span      = CarouselEdgeSpan();
    const float fadeRange = span - card.m_wrapFadeStartListSlot;

    constexpr float kWrapSettleEpsilon = 0.06f;

    if (fadeRange <= 1e-4f)
    {
        FinishWrapPhase(card, listIndex);
        return;
    }

    const float scrolled = card.m_wrapFadeStartListSlot - listSlot;
    const float t        = std::clamp(scrolled / fadeRange, 0.0f, 1.0f);
    card.m_carouselDisplaySlot   = listSlot + fadeRange * (1.0f - t);
    card.m_wrapFadeProgress = t;

    if (t >= 1.0f - kWrapSettleEpsilon)
        FinishWrapPhase(card, listIndex);
}

void Flip3DCompApp::SettleWrapDisplaySlots()
{
    if (std::abs(m_scrollTargetPosition - m_scrollPosition) > kScrollSettleEpsilon)
        return;

    constexpr float kWrapSettleEpsilon = 0.06f;

    for (int k = 0; k < (int)m_windowCards.size(); ++k)
    {
        CardModel& c = m_windowCards[(size_t)k];
        const float listSlot = CardListSlot(k);

        switch (c.m_carouselWrapPhase)
        {
        case CarouselWrapPhase::EnteringFront:
            if (std::abs(c.m_carouselDisplaySlot - listSlot) < kWrapSettleEpsilon
                || c.m_carouselDisplaySlot > listSlot + kWrapSettleEpsilon)
            {
                FinishWrapPhase(c, k);
            }
            break;

        case CarouselWrapPhase::ExitingFront:
            if (c.m_carouselDisplaySlot <= kFrontCardExitSlot)
                BeginEnteringBackWrap(c, k);
            break;

        case CarouselWrapPhase::EnteringBack:
            StepEnteringBackWrap(c, k);
            break;

        default:
            break;
        }
    }
}

float Flip3DCompApp::CarouselEdgeSpan() const
{
    const int count = (int)m_windowCards.size();
    return (float)std::min(std::max(count, 2), kMaxVisibleCards);
}

void Flip3DCompApp::InvalidateDisplaySlots()
{
    for (auto& c : m_windowCards)
    {
        c.m_hasValidCarouselDisplaySlot = false;
        c.m_carouselWrapPhase        = CarouselWrapPhase::None;
    }
}

void Flip3DCompApp::SyncDisplaySlotsToList()
{
    for (int k = 0; k < (int)m_windowCards.size(); ++k)
    {
        CardModel& c = m_windowCards[(size_t)k];
        if (c.m_carouselWrapPhase != CarouselWrapPhase::None)
            continue;

        c.m_carouselDisplaySlot      = CardListSlot(k);
        c.m_hasValidCarouselDisplaySlot = true;
    }
}

void Flip3DCompApp::AdvanceWrapDisplaySlots(float deltaScrollPos)
{
    if (deltaScrollPos == 0.0f && m_wrapScrollAdjustmentThisFrame == 0.0f)
        return;

    constexpr float kWrapSettleEpsilon = 0.06f;
    const float enteringFrontDelta = deltaScrollPos - m_wrapScrollAdjustmentThisFrame;

    for (int k = 0; k < (int)m_windowCards.size(); ++k)
    {
        CardModel& c = m_windowCards[(size_t)k];
        const float listSlot = CardListSlot(k);

        switch (c.m_carouselWrapPhase)
        {
        case CarouselWrapPhase::ExitingFront:
            c.m_carouselDisplaySlot -= deltaScrollPos * kFrontCardFadeSpeed;
            if (c.m_carouselDisplaySlot <= kFrontCardExitSlot)
                BeginEnteringBackWrap(c, k);
            break;

        case CarouselWrapPhase::EnteringBack:
            StepEnteringBackWrap(c, k);
            break;

        case CarouselWrapPhase::EnteringFront:
            c.m_carouselDisplaySlot -= enteringFrontDelta;
            // Update wrap progress for entering-front so opacity can be
            // interpolated from the minimum to target value.
            {
                const float fadeRangeFront = listSlot - c.m_wrapFadeStartListSlot;
                if (fadeRangeFront > 1e-4f)
                {
                    const float scrolledFront = listSlot - c.m_carouselDisplaySlot;
                    c.m_wrapFadeProgress = std::clamp(scrolledFront / fadeRangeFront, 0.0f, 1.0f);
                }
            }
            if (std::abs(c.m_carouselDisplaySlot - listSlot) < kWrapSettleEpsilon
                || c.m_carouselDisplaySlot > listSlot + kWrapSettleEpsilon)
            {
                FinishWrapPhase(c, k);
            }
            break;

        default:
            break;
        }
    }
}

void Flip3DCompApp::OnCarouselWrapForward(HWND outgoingHwnd, float outgoingSlot)
{
    for (int k = 0; k < (int)m_windowCards.size(); ++k)
    {
        CardModel& c = m_windowCards[(size_t)k];
        if (c.m_windowHandle != outgoingHwnd
            && c.m_carouselWrapPhase == CarouselWrapPhase::EnteringFront)
        {
            FinishWrapPhase(c, k);
        }
    }

    for (auto& c : m_windowCards)
    {
        if (c.m_windowHandle != outgoingHwnd)
            continue;

        c.m_carouselDisplaySlot      = outgoingSlot;
        c.m_carouselWrapPhase        = CarouselWrapPhase::ExitingFront;
        c.m_hasValidCarouselDisplaySlot = true;
        break;
    }

    SyncDisplaySlotsToList();
}

void Flip3DCompApp::OnCarouselWrapBackward(HWND incomingHwnd)
{
    for (int k = 0; k < (int)m_windowCards.size(); ++k)
    {
        CardModel& c = m_windowCards[(size_t)k];
        if (c.m_windowHandle == incomingHwnd)
            continue;

        if (c.m_carouselWrapPhase == CarouselWrapPhase::EnteringFront
            || c.m_carouselWrapPhase == CarouselWrapPhase::ExitingFront
            || c.m_carouselWrapPhase == CarouselWrapPhase::EnteringBack)
        {
            FinishWrapPhase(c, k);
        }
    }

    for (int k = 0; k < (int)m_windowCards.size(); ++k)
    {
        CardModel& c = m_windowCards[(size_t)k];
        if (c.m_windowHandle != incomingHwnd)
            continue;

        c.m_carouselWrapPhase             = CarouselWrapPhase::EnteringFront;
        c.m_carouselDisplaySlot           = CardListSlot(k) - 0.55f;
        c.m_wrapFadeStartListSlot = c.m_carouselDisplaySlot; // start offset for fade progress
        c.m_wrapFadeProgress          = 0.0f;
        c.m_hasValidCarouselDisplaySlot      = true;
        break;
    }

    SyncDisplaySlotsToList();
}

void Flip3DCompApp::FreezeCarouselVisuals()
{
    std::vector<std::pair<HWND, float>> visualByHwnd;
    visualByHwnd.reserve(m_windowCards.size());
    for (int k = 0; k < (int)m_windowCards.size(); ++k)
        visualByHwnd.emplace_back(m_windowCards[(size_t)k].m_windowHandle,
                                    CaptureCarouselVisualSlot(k));

    WrapCarouselScroll();

    for (int k = 0; k < (int)m_windowCards.size(); ++k)
    {
        CardModel& c = m_windowCards[(size_t)k];
        c.m_carouselWrapPhase             = CarouselWrapPhase::None;
        c.m_wrapFadeStartListSlot = 0.0f;

        for (const auto& entry : visualByHwnd)
        {
            if (entry.first == c.m_windowHandle)
            {
                c.m_carouselDisplaySlot = entry.second;
                break;
            }
        }

        c.m_hasValidCarouselDisplaySlot = true;
    }

    m_exitScrollSnapshot = m_scrollPosition;
    m_scrollTargetPosition       = m_scrollPosition;
}

void Flip3DCompApp::UpdateVisualSlots(float enterProgress)
{
    if (m_viewState != ViewState::Exit && m_viewState != ViewState::ExitRepeatedRotate)
        return;

    const int cardCount = (int)m_windowCards.size();

    if (m_viewState == ViewState::ExitRepeatedRotate && m_rotateAnimation.IsActive())
    {
        for (int k = 0; k < cardCount; ++k)
            m_windowCards[(size_t)k].m_carouselDisplaySlot = ComputeRotationDisplaySlot(k);

        return;
    }

    const bool rotationDone = m_viewState == ViewState::ExitRepeatedRotate
                           && !m_windowCards.empty()
                           && m_selectedWindowHandle
                           && m_windowCards[0].m_windowHandle == m_selectedWindowHandle;

    if (m_viewState == ViewState::Exit || rotationDone)
    {
        m_scrollPosition    = m_exitScrollSnapshot * enterProgress;
        m_scrollTargetPosition = m_scrollPosition;

        for (int k = 0; k < cardCount; ++k)
            m_windowCards[(size_t)k].m_carouselDisplaySlot = CardListSlot(k);
    }

    // ExitRepeatedRotate with more steps pending: keep m_carouselDisplaySlot unchanged
}

// ============================================================================
// Flip3DCompApp::RotateSelectedToListFront
// uDWM: after scroll settles, splice the ring so the selection is list index 0
// (e.g. A B C D E F G → C D E F G A B). Cards after C in the ring follow C at
// slots 1…, not the windows that were before C (A B).
// ============================================================================
void Flip3DCompApp::RotateSelectedToListFront()
{
    if (!m_selectedWindowHandle || m_windowCards.size() <= 1)
        return;

    const int idx = FindCardIndex(m_selectedWindowHandle);
    if (idx <= 0)
        return;

    for (int i = 0; i < idx; ++i)
        RotateListPhysically(false);
}

// ============================================================================
// Flip3DCompApp::CommitCarouselScroll
// Snap fractional scroll to integer slots and align the window list.
// ============================================================================
void Flip3DCompApp::CommitCarouselScroll()
{
    if (m_windowCards.size() <= 1)
    {
        m_scrollPosition    = 0.0f;
        m_scrollTargetPosition = 0.0f;
        InvalidateDisplaySlots();
        return;
    }

    AlignCarouselScrollSettled();
}

// ============================================================================
// Flip3DCompApp::AlignCarouselScrollSettled
// Wrap + rotate selection to list front + zero scroll (shared by commit & exit).
// ============================================================================
void Flip3DCompApp::AlignCarouselScrollSettled()
{
    if (m_windowCards.size() <= 1)
    {
        m_scrollPosition    = 0.0f;
        m_scrollTargetPosition = 0.0f;
        return;
    }

    WrapCarouselScroll();
    RotateSelectedToListFront();
    m_scrollPosition    = 0.0f;
    m_scrollTargetPosition = 0.0f;

    for (int k = 0; k < (int)m_windowCards.size(); ++k)
    {
        CardModel& c = m_windowCards[(size_t)k];
        c.m_carouselWrapPhase             = CarouselWrapPhase::None;
        c.m_wrapFadeStartListSlot = 0.0f;
    }

    SyncDisplaySlotsToList();

    m_exitScrollSnapshot = 0.0f;
}

// ============================================================================
// Flip3DCompApp::RotateBy
// ============================================================================
void Flip3DCompApp::RotateBy(int deltaSteps)
{
    if (!deltaSteps || m_windowCards.size() <= 1
        || m_viewState == ViewState::Exit
        || m_viewState == ViewState::ExitRepeatedRotate)
        return;

    // Prevent fractional drift by anchoring the scroll target to the
    // nearest integer before applying a discrete rotate step. This ensures
    // repeated rotates increment by exactly one slot.
    const float base = std::round(m_scrollTargetPosition);
    m_scrollTargetPosition = base + (float)deltaSteps;
}

// ============================================================================
// Flip3DCompApp::ComputeScrollTargetForCard
// Scroll value that places listIndex at the visual front (slot ≈ 0).
// Short-path wrap is only used when it moves scrollPos in the direction that
// reduces |visualSlot| (slot = index - scrollPos increases as scrollPos falls).
// ============================================================================
float Flip3DCompApp::ComputeScrollTargetForCard(int listIndex) const
{
    const int N = (int)m_windowCards.size();
    if (N <= 1)
        return m_scrollPosition;

    const float visualSlot = (float)listIndex - m_scrollPosition;
    if (std::abs(visualSlot) < kScrollSettleEpsilon)
        return m_scrollPosition;

    float shortDelta = visualSlot;
    if (shortDelta > (float)N * 0.5f)
        shortDelta -= (float)N;
    else if (shortDelta < -(float)N * 0.5f)
        shortDelta += (float)N;

    if (visualSlot > 0.0f)
    {
        if (shortDelta > 0.0f && std::abs(shortDelta) < std::abs(visualSlot))
            return m_scrollPosition + shortDelta;
    }
    else
    {
        if (shortDelta < 0.0f && std::abs(shortDelta) < std::abs(visualSlot))
            return m_scrollPosition + shortDelta;
    }

    return m_scrollPosition + visualSlot;
}

// ============================================================================
// Flip3DCompApp::IsSelectionAtCarouselFront
// Exit scroll completes when the selection is list index 0 and scroll settled.
// ============================================================================
bool Flip3DCompApp::IsSelectionAtCarouselFront() const
{
    if (!m_selectedWindowHandle || m_windowCards.empty())
        return true;

    if (m_windowCards[0].m_windowHandle != m_selectedWindowHandle)
        return false;

    return std::abs(m_scrollPosition) < 0.05f
        && std::abs(m_scrollTargetPosition - m_scrollPosition) < 0.05f;
}

// ============================================================================
// Flip3DCompApp::StepCarouselScroll
// Shared smooth scroll + wrap + front-edge fade (browse and exit).
// ============================================================================
void Flip3DCompApp::StepCarouselScroll(float dtSeconds, bool notifyFrontChange)
{
    const float prevFront  = std::floor(m_scrollPosition + 0.5f);
    const float prevScroll = m_scrollPosition;
    if (kEnableAnimationEasing)
    {
        const float a = 1.0f - std::exp(
            -dtSeconds / std::max(kScrollSmoothTimeSec, 1e-4f));
        m_scrollPosition += (m_scrollTargetPosition - m_scrollPosition) * a;
    }
    else
    {
        const float maxDistance = dtSeconds
            / std::max(kScrollSmoothTimeSec, 1e-4f);
        const float distance = m_scrollTargetPosition - m_scrollPosition;
        if (std::abs(distance) <= maxDistance)
            m_scrollPosition = m_scrollTargetPosition;
        else
            m_scrollPosition += std::copysign(maxDistance, distance);
    }
    const float smoothDelta = m_scrollPosition - prevScroll;
    WrapCarouselScroll();
    AdvanceWrapDisplaySlots(smoothDelta);

    SettleWrapDisplaySlots();

    // If scroll has essentially settled to the target, snap exactly to the
    // target to avoid accumulating tiny fractional drift across repeated
    // discrete moves. This keeps which card is front unambiguous.
    if (std::abs(m_scrollTargetPosition - m_scrollPosition) < kScrollSettleEpsilon)
    {
        m_scrollPosition = m_scrollTargetPosition;
    }

    if (notifyFrontChange)
    {
        const float newFront = std::floor(m_scrollPosition + 0.5f);
        if (newFront != prevFront)
            NotifyAccessibilityFocusFront();
    }
}

// ============================================================================
// Flip3DCompApp::RotateToWindow
// ============================================================================
void Flip3DCompApp::RotateToWindow(HWND targetHwnd)
{
    if (m_windowCards.size() <= 1 || m_viewState == ViewState::Exit
        || m_viewState == ViewState::ExitRepeatedRotate)
        return;

    int idx = -1;
    for (int i = 0; i < (int)m_windowCards.size(); ++i)
    {
        if (m_windowCards[(size_t)i].m_windowHandle == targetHwnd) { idx = i; break; }
    }
    if (idx < 0)
        return;

    m_scrollTargetPosition = ComputeScrollTargetForCard(idx);
}

// ============================================================================
// Flip3DCompApp::TickSmoothScroll
// ============================================================================
void Flip3DCompApp::TickSmoothScroll(float dtSeconds)
{
    if (m_windowCards.size() <= 1 || m_viewState != ViewState::Interactive)
        return;

    const auto now = std::chrono::steady_clock::now();
    const float heldTime = std::chrono::duration<float>(now - m_heldNavigationStartTime).count();
    if (m_heldNavigationDirection != 0 && heldTime >= kHeldKeyStartDelaySec)
    {
        m_scrollTargetPosition += (float)m_heldNavigationDirection
            * kHeldKeyRotateSpeed * dtSeconds;
    }

    if (m_pendingWheelSlots != 0)
    {
        const auto now = std::chrono::steady_clock::now();
        const float sinceWheel = std::chrono::duration<float>(now - m_lastMouseWheelTime).count();
        if (sinceWheel > kWheelActiveTimeoutSec)
        {
            m_pendingWheelSlots = 0;
        }
        else
        {
            const float sinceKey = std::chrono::duration<float>(now - m_lastProcessedKey).count();
            if (sinceKey >= kKeyRepeatIntervalSec)
            {
                const int step = (m_pendingWheelSlots > 0) ? 1 : -1;
                RotateBy(step);
                m_pendingWheelSlots -= step;
                m_lastProcessedKey = now;
            }
        }
    }

    StepCarouselScroll(dtSeconds, /*notifyFrontChange=*/true);
}

// ============================================================================
// Flip3DCompApp::Update
// ============================================================================
void Flip3DCompApp::Update(float dtSeconds)
{
    if (m_areThumbnailsDirty)
        OnThumbnailSourceSizeChanged();

    float animationRate = std::max(kAnimationRate, 0.0f);
    if (m_isShiftSlowdownSettingEnabled && (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0)
        animationRate *= kShiftAnimationRate;
    dtSeconds *= animationRate;

    m_enterExitAnimation.Update(dtSeconds);

    if (m_rotateAnimation.IsActive())
    {
        m_rotateAnimation.Update(dtSeconds);
        OnRotationTimeUpdated();
    }

    if (!m_rotateAnimation.IsActive())
        TickRepeatedRotate();

    const float enterProgress = EnterProgress();
    UpdateVisualSlots(enterProgress);

    TickSmoothScroll(dtSeconds);

    const float washProgress  = std::clamp(m_enterExitAnimation.LinearValue(), 0.0f, 1.0f);

    if (m_sceneCompositionVisual)
        m_sceneCompositionVisual->SetOpacity(1.0f);
    for (auto& mon : m_monitorBackdropLayers)
    {
        if (mon.washVisual)
            mon.washVisual->SetOpacity(washProgress * kDesktopWashOpacityScale);
        if (mon.shellContainer)
            mon.shellContainer->SetOpacity(1.0f);
    }

    if (m_viewState == ViewState::Enter && !m_enterExitAnimation.IsActive())
    {
        m_viewState = ViewState::Interactive;
        InvalidateDisplaySlots();
        NotifyAccessibilityEvent(EVENT_SYSTEM_DIALOGSTART);
        NotifyAccessibilityFocusFront();
    }

    if (m_isOpeningTabPending && m_viewState == ViewState::Interactive)
    {
        const auto now = std::chrono::steady_clock::now();
        const bool tabStillDown = (GetAsyncKeyState(VK_TAB) & 0x8000) != 0;
        const float pendingTime = std::chrono::duration<float>(now - m_openingTabStartTime).count();
        if (!tabStillDown)
        {
            m_isOpeningTabPending = false;
        }
        else if (pendingTime >= kOpeningTabDelaySec)
        {
            m_isOpeningTabPending = false;
            OnKey(true, VK_TAB, 0);
            m_heldNavigationStartTime = now
                - std::chrono::duration_cast<std::chrono::steady_clock::duration>(
                    std::chrono::duration<float>(kHeldKeyStartDelaySec));
        }
    }

    UpdateCamera(enterProgress);
    UpdateCards(enterProgress, dtSeconds);

    if (m_compositionDevice)
        m_compositionDevice->Commit();

    const auto finishProcess = [this]()
    {
        if (!m_windowHandle || !IsWindow(m_windowHandle) || !DestroyWindow(m_windowHandle))
            PostQuitMessage(0);
    };

    if (m_viewState == ViewState::Exit && !m_enterExitAnimation.IsActive())
        finishProcess();

    if (m_viewState == ViewState::ExitRepeatedRotate
        && !m_enterExitAnimation.IsActive()
        && !m_rotateAnimation.IsActive()
        && m_repeatedRotateStepsRemaining == 0)
    {
        finishProcess();
    }
}

// ============================================================================
// Flip3DCompApp::BuildCameraMatrix
// View × Proj × Viewport — shared by card transforms and hit-testing.
// ============================================================================
Matrix4x4 Flip3DCompApp::BuildCameraMatrix(float enterProgress) const
{
    auto view     = BuildViewMatrix(enterProgress);
    auto proj     = BuildProjMatrix(1.0f, enterProgress);
    auto viewport = Math::BuildViewportMatrix(m_monitorWidth, m_monitorHeight, m_viewOriginX, m_viewOriginY);
    return Math::Multiply(Math::Multiply(view, proj), viewport);
}

// ============================================================================
// Flip3DCompApp::UpdateCamera
// Scene root stays identity. Each card bakes Model × View × Proj × VP on its
// container visual (same as the pre-SPATIAL single-matrix path).
// ============================================================================
void Flip3DCompApp::UpdateCamera(float /*enterProgress*/)
{
    if (!m_sceneCompositionVisual)
        return;

    m_sceneCompositionVisual->SetTransform(Math::Identity());
}

float Flip3DCompApp::ComputeFlatDepthRank(float slot, float enterProgress,
                                          int listIndex) const
{
    const float p = enterProgress;

    if (m_viewState == ViewState::Exit || m_viewState == ViewState::ExitRepeatedRotate)
        return slot;

    if (p < 1.0f - 1e-5f)
    {
        if (listIndex >= 0 && listIndex < (int)m_windowCards.size())
        {
            const CardModel& c = m_windowCards[(size_t)listIndex];
            if (m_selectedWindowHandle && c.m_windowHandle == m_selectedWindowHandle)
                return -1.0f;
            return (float)c.m_initialCarouselIndex;
        }
    }

    return slot;
}

// ============================================================================
// Flip3DCompApp::ComputePaintKey
// DirectComposition has no depth buffer — sibling order must track carousel depth.
// Blending slot with desktop rank mid-exit causes keys to cross and flicker.
// ============================================================================
float Flip3DCompApp::ComputePaintKey(float slot, float enterProgress,
                                     int listIndex) const
{
    const float p = enterProgress;

    auto flatRank = [this, listIndex](int k) -> float
    {
        if (k >= 0 && k < (int)m_windowCards.size())
        {
            const CardModel& c = m_windowCards[(size_t)k];
            if (m_selectedWindowHandle && c.m_windowHandle == m_selectedWindowHandle)
                return -1.0f;
            return (float)c.m_initialCarouselIndex;
        }
        return (float)listIndex;
    };

    if (m_viewState == ViewState::Exit || m_viewState == ViewState::ExitRepeatedRotate)
        return slot;

    if (m_viewState == ViewState::Enter)
        return p * slot + (1.0f - p) * flatRank(listIndex);

    return slot;
}

// ============================================================================
// Flip3DCompApp::UpdateCards
// ============================================================================
void Flip3DCompApp::UpdateCards(float enterProgress, float dtSeconds)
{
    if (!m_sceneCompositionVisual)
        return;

    const int N = (int)m_windowCards.size();
    if (N == 0)
        return;

    const float p      = enterProgress;
    const auto  camera = BuildCameraMatrix(p);
    const int   maxVis = kMaxVisibleCards;

    const bool scrollPath = (m_viewState == ViewState::Interactive);
    const bool exitRotate = (m_viewState == ViewState::ExitRepeatedRotate);
    if (scrollPath)
        SyncDisplaySlotsToList();

    struct CardDraw { CardModel* card; float slot; float opacity; float key; int listIndex; };
    std::vector<CardDraw> draws;
    draws.reserve((size_t)N);

    for (int k = 0; k < N; ++k)
    {
        CardModel& c = m_windowCards[(size_t)k];
        if (!c.m_cardTransformVisual)
            continue;

        const float slot = GetCardDisplaySlot(k);
        float opacity = ComputeUpdateAlpha(c, p, slot);

        // A short carousel can introduce a back card without entering one of
        // the explicit wrap phases. Detect the transition from invisible to
        // visible and start the same fade used by wrapped cards.
        if (!c.m_isOpacityInitialized)
        {
            c.m_wrapOpacity          = opacity;
            c.m_lastTargetOpacity    = opacity;
            c.m_isOpacityInitialized   = true;
        }
        else if (!c.m_isWrapFadeActive
                 && c.m_lastTargetOpacity <= 0.01f
                 && opacity > 0.01f)
        {
            c.m_wrapOpacity     = 0.0f;
            c.m_isWrapFadeActive  = true;
        }
        c.m_lastTargetOpacity = opacity;

        const float frontCull = exitRotate ? -2.0f
                              : (m_viewState == ViewState::Exit) ? -1.5f
                              :                                -1.0f;

        if (scrollPath)
        {
            const bool wrapDecoupled = (c.m_carouselWrapPhase == CarouselWrapPhase::ExitingFront
                                     || c.m_carouselWrapPhase == CarouselWrapPhase::EnteringFront);
            const float span = CarouselEdgeSpan();
            if (!wrapDecoupled && slot >= (float)maxVis)
            {
                c.m_cardTransformVisual->SetVisible(FALSE);
                continue;
            }
            if (wrapDecoupled && (slot < -1.5f || slot > span + 0.5f))
            {
                c.m_cardTransformVisual->SetVisible(FALSE);
                continue;
            }
            if (opacity < 0.01f && !c.m_isWrapFadeActive)
            {
                c.m_cardTransformVisual->SetVisible(FALSE);
                continue;
            }
        }
        else if (exitRotate)
        {
            if (!ShouldDrawRotationCard(k) && opacity < 0.01f)
            {
                c.m_cardTransformVisual->SetVisible(FALSE);
                continue;
            }
            if (opacity < 0.01f)
            {
                c.m_cardTransformVisual->SetVisible(FALSE);
                continue;
            }
        }
        else if (slot >= (float)maxVis
              || (slot <= frontCull && opacity < 0.01f))
        {
            c.m_cardTransformVisual->SetVisible(FALSE);
            continue;
        }

        const float key = ComputePaintKey(slot, p, k);
        draws.push_back({ &c, slot, opacity, key, k });
    }

    for (int k = 0; k < N; ++k)
    {
        CardModel& c = m_windowCards[(size_t)k];
        if (!c.m_cardTransformVisual)
            continue;
        bool listed = false;
        for (const auto& d : draws)
            if (d.card == &c) { listed = true; break; }
        if (!listed)
            c.m_cardTransformVisual->SetVisible(FALSE);
    }

    std::sort(draws.begin(), draws.end(),
        [](const CardDraw& a, const CardDraw& b)
        {
            if (a.key != b.key)
                return a.key > b.key;
            return a.listIndex > b.listIndex;
        });

    std::vector<HWND> paintOrder;
    paintOrder.reserve(draws.size());
    for (const auto& d : draws)
        paintOrder.push_back(d.card->m_windowHandle);

    const bool orderChanged = (paintOrder != m_lastPaintOrder);
    if (orderChanged)
    {
        for (auto& d : draws)
            m_sceneCompositionVisual->RemoveVisual(d.card->m_cardTransformVisual.Get());
        m_lastPaintOrder = std::move(paintOrder);
    }

    IDCompositionVisual* prevVis = nullptr;
    for (auto& d : draws)
    {
        d.card->m_cardTransformVisual->SetVisible(TRUE);
        if (kEnableBackCardOpacityEasing && d.card->m_isWrapFadeActive)
        {
            const float fade = 1.0f - std::exp(
                -dtSeconds / std::max(kBackCardFadeTimeSec, 1e-4f));
            d.card->m_wrapOpacity +=
                (d.opacity - d.card->m_wrapOpacity) * fade;
            if (std::abs(d.card->m_wrapOpacity - d.opacity) < 0.002f)
            {
                d.card->m_wrapOpacity = d.opacity;
                d.card->m_isWrapFadeActive = false;
            }
        }
        else
        {
            d.card->m_wrapOpacity = d.opacity;
            d.card->m_isWrapFadeActive = false;
        }
        d.card->m_cardTransformVisual->SetOpacity(
            std::clamp(d.card->m_wrapOpacity, 0.0f, 1.0f));

        const float t = ComputeCarouselBezierT(d.slot);
        const float flatRank = ComputeFlatDepthRank(d.slot, p, d.listIndex);
        auto model = BuildModelMatrix(*d.card, t, p, flatRank);
        d.card->m_cardTransformVisual->SetTransform(Math::Multiply(model, camera));

        if (orderChanged)
        {
            if (prevVis)
                m_sceneCompositionVisual->AddVisual(d.card->m_cardTransformVisual.Get(), TRUE, prevVis);
            else
                m_sceneCompositionVisual->AddVisual(d.card->m_cardTransformVisual.Get(), FALSE, nullptr);
            prevVis = d.card->m_cardTransformVisual.Get();
        }
    }
}

// ============================================================================
// Flip3DCompApp::ComputeCarouselEdgeOpacity
// Continuous carousel edge roll-off used by the smooth-scroll path. Matches the
// uDWM steady-state alpha: full inside, fading toward the camera-side (slot < 0)
// as a card flies out the front. With fewer cards, keep the last card more opaque
// so a short carousel does not appear to dissolve at its back edge.
// ============================================================================
float Flip3DCompApp::ComputeCarouselEdgeOpacity(float slot) const
{
    if (slot < 0.0f)
    {
        const float fadeDistance = std::max(-kFrontCardExitSlot, 1e-4f);
        return std::clamp(1.0f + slot / fadeDistance, 0.0f, 1.0f);
    }

    const float span = CarouselEdgeSpan();
    if (slot >= span)
        return 0.0f;
    if (slot >= span - 2.0f)
    {
        const float countRange = (float)std::max(kMaxVisibleCards - 2, 1);
        const float fewerCards = std::clamp(
            (float)(kMaxVisibleCards - (int)m_windowCards.size()) / countRange,
            0.0f, 1.0f);
        const float lastCardOpacity = 0.5f + 0.5f * fewerCards;

        if (slot >= span - 1.0f)
            return lastCardOpacity * (span - slot);

        return lastCardOpacity
             + (1.0f - lastCardOpacity) * (span - 1.0f - slot);
    }
    return 1.0f;
}

// ============================================================================
// Flip3DCompApp::ComputeUpdateAlpha
// ============================================================================
float Flip3DCompApp::ComputeUpdateAlpha(const CardModel& card, float enterProgress,
                                        float carouselSlot) const
{
    float opacitySlot = carouselSlot;
    if (m_viewState == ViewState::Interactive
        && card.m_carouselWrapPhase == CarouselWrapPhase::EnteringBack)
    {
        opacitySlot = card.m_carouselDisplaySlot;
    }

    float rotationOpacity = ComputeCarouselEdgeOpacity(opacitySlot);
    // Boost opacity for cards near the back edge so they're more visible but
    // still follow the same fade curve. Clamp to 1.0.
    const float span = CarouselEdgeSpan();
    if (opacitySlot >= span - 2.0f)
    {
        rotationOpacity = std::min(1.0f, rotationOpacity * kBackCardOpacityScale);
    }

    // For cards that are entering from a wrap (either front or back), blend
    // their opacity from a minimum up to the computed rotationOpacity based
    // on the per-card wrap progress updated each frame in the wrap step.
    if (card.m_carouselWrapPhase == CarouselWrapPhase::EnteringBack
        || card.m_carouselWrapPhase == CarouselWrapPhase::EnteringFront)
    {
        const float t = std::clamp(card.m_wrapFadeProgress, 0.0f, 1.0f);
        rotationOpacity = (1.0f - t) * kBackCardMinOpacity + t * rotationOpacity;
    }
    float enterScale      = 1.0f;

    const bool exitRotateFlattenTail =
        m_viewState == ViewState::ExitRepeatedRotate
        && !m_rotateAnimation.IsActive()
        && !m_windowCards.empty()
        && m_windowCards[0].m_windowHandle == m_selectedWindowHandle;

    if ((m_viewState == ViewState::Exit || exitRotateFlattenTail)
        && (int)m_windowCards.size() > kMaxVisibleCards
        && carouselSlot >= (float)kMaxVisibleCards - 1.0f)
    {
        rotationOpacity = kDesktopWashOpacityScale;
    }

    if (m_viewState == ViewState::ExitRepeatedRotate && m_rotateAnimation.IsActive())
    {
        const int idx = FindCardIndex(card.m_windowHandle);
        if (idx >= 0)
            return ComputeRotationAlpha(card, enterProgress, idx, carouselSlot);
    }

    if (std::abs(enterProgress - 1.0f) >= 1e-6f)
    {
        if (std::abs(rotationOpacity - 1.0f) >= 1e-6f)
            rotationOpacity = enterProgress * rotationOpacity
                            + (1.0f - enterProgress);

        if (card.m_isMinimized)
            enterScale = enterProgress * enterProgress;
        else if (card.m_isShellDesktop)
            enterScale = enterProgress;
    }

    return std::clamp(rotationOpacity * enterScale, 0.0f, 1.0f);
}

// ============================================================================
// Flip3DCompApp::ComputeCarouselPathDenom
// uDWM UpdateModels: m_windowCount = max(count, 5), capped by max visible.
// ============================================================================
float Flip3DCompApp::ComputeCarouselPathDenom() const
{
    const int N = (int)m_windowCards.size();
    return (float)std::min(std::max(N, 5), kMaxVisibleCards);
}

// ============================================================================
// Flip3DCompApp::ComputeCarouselBezierT
// flip3d/uDWM: zIndex = -pathRelative; t = 1 - (zIndex - 0.5) / -m_windowCount
// (pathRelative == carousel slot: 0 = front, larger = further back)
// ============================================================================
float Flip3DCompApp::ComputeCarouselBezierT(float slot) const
{
    const float denom  = ComputeCarouselPathDenom();
    const float zIndex = -slot;
    // uDWM GetWorldFromParametric evaluates the quadratic path with NO clamp.
    // Windows past the view are culled by the caller, so in practice only the
    // back-edge card (t slightly < 0, against P0) and the front/outgoing card
    // (slot < 0 → t > 1, extending past P2 toward the camera) leave [0,1].
    return 1.0f - ((zIndex - 0.5f) / -denom);
}

// ============================================================================
// Flip3DCompApp::BuildViewMatrix
// ============================================================================
Matrix4x4 Flip3DCompApp::BuildViewMatrix(float p) const
{
    using namespace Math;

    float yaw   = kCameraFinalRotateY * p;
    float pitch = kCameraFinalRotateX * p;
    float roll  = kCameraFinalRotateZ * p;

    auto T1  = Translation(0, 0, 1);
    auto Rot = RotationRollPitchYaw(pitch, yaw, roll);
    auto T2  = Translation(0, 0, -1);
    auto T3  = Translation(-kCameraFinalTranslateX * p,
                            -kCameraFinalTranslateY * p,
                            -kCameraFinalTranslateZ * p);
    auto LA  = LookAtRH({0, 0, 1}, {0, 0, 0}, {0, 1, 0});

    auto V = T1;
    V = Multiply(V, Rot);
    V = Multiply(V, T2);
    V = Multiply(V, T3);
    V = Multiply(V, LA);
    return V;
}

// ============================================================================
// Flip3DCompApp::BuildProjMatrix
// ============================================================================
Matrix4x4 Flip3DCompApp::BuildProjMatrix(float /*aspect*/, float p) const
{
    float nearExtent = p * kNearPlaneEdgeSize + (1.0f - p);
    return Math::PerspectiveRH(nearExtent, 1.0f, kNearPlaneDistance, 50.0f);
}

// ============================================================================
// Flip3DCompApp::BuildModelMatrix
// uDWM UpdateModels / flip3d GetWorldFromParametric: carousel pathRelative for
// both 3D bezier and flatZ; lerp to desktop when enterProgress != 1.
// ============================================================================
Matrix4x4 Flip3DCompApp::BuildModelMatrix(const CardModel& c, float t,
                                          float enterProgress,
                                          float flatDepthRank) const
{
    using namespace Math;

    Vec3  cv     = Bezier(t);
    float fullW  = std::abs(c.m_cardTargetSize.x);
    float fullH  = std::abs(c.m_cardTargetSize.y);

    Vec3 pos3d = { cv.x, cv.y + fullH, cv.z };

    float p = std::clamp(enterProgress, 0.0f, 1.0f);
    const float flatZ = (-flatDepthRank) / 10000.0f;

    float tx, ty, tz, worldW, worldH;

    if (p >= 1.0f - 1e-5f)
    {
        tx = pos3d.x;  ty = pos3d.y;  tz = pos3d.z;
        worldW = fullW;
        worldH = fullH;
    }
    else if (p <= 1e-5f)
    {
        tx = c.m_flatPosition.x;
        ty = c.m_flatPosition.y;
        tz = flatZ;
        worldW = c.m_flatSize.x;
        worldH = c.m_flatSize.y;
    }
    else
    {
        Vec3 pos2d = { c.m_flatPosition.x, c.m_flatPosition.y, flatZ };
        tx     = Lerp(pos2d.x, pos3d.x, p);
        ty     = Lerp(pos2d.y, pos3d.y, p);
        tz     = Lerp(pos2d.z, pos3d.z, p);
        worldW = Lerp(c.m_flatSize.x, fullW, p);
        worldH = Lerp(c.m_flatSize.y, fullH, p);
    }

    float sx =  worldW / (float)std::max(c.m_thumbnailSourceWidth,  1);
    float sy = -worldH / (float)std::max(c.m_thumbnailSourceHeight, 1);

    auto S = Scale(sx, sy, 1.0f);
    auto T = Translation(tx, ty, tz);
    auto M = Multiply(S, T);

    if (m_isRightToLeft)
    {
        auto Mirror = Multiply(
            Scale(-1.0f, 1.0f, 1.0f),
            Translation((float)c.m_thumbnailSourceWidth, 0.0f, 0.0f));
        M = Multiply(Mirror, M);
    }

    return M;
}


// ===== Ported from src/Flip3DComp_Rotation.cpp =====
// ============================================================================
// Flip3DComp_Rotation.cpp — uDWM-style discrete list rotation during exit
// ============================================================================
#include <algorithm>
#include <cmath>

namespace
{
struct RotationContext
{
    int   countInt      = 0;
    int   visibleCount  = 0;
    bool  hasHidden     = false;
    bool  isRotating    = false;
    int   rotationSteps = 0;   // +1 front→back, -1 back→front, 0 idle
    float rotationProgress = 1.0f;
    float hiddenFlipOut  = 1.0f;
    float hiddenFlipIn   = 0.0f;
    int   rotatingOutIndex = -1;
    int   rotatingInIndex  = -1;
    int   flipCardIndex    = -1;
    bool  useHiddenRotation = false;
    bool  showOutgoing      = false;
};

struct CardRotationEntry
{
    int  listIndex   = 0;
    int  startSlot   = 0;
    int  endSlot     = 0;
    bool isOutgoing  = false;
    bool isIncoming  = false;
    bool isBoundaryIn = false;
    bool isCycle     = false;
    bool shouldDraw  = false;
};

RotationContext BuildRotationContext(int countInt, bool isRotating,
                                     bool rotateBackward, float rotationProgress,
                                     bool showOutgoing)
{
    RotationContext ctx = {};
    ctx.countInt = countInt;
    if (ctx.countInt <= 0)
        return ctx;

    ctx.visibleCount = std::min(kMaxVisibleCards, ctx.countInt);
    ctx.hasHidden    = ctx.countInt > ctx.visibleCount;
    ctx.isRotating   = isRotating;
    ctx.rotationSteps = ctx.isRotating ? (rotateBackward ? -1 : 1) : 0;
    ctx.rotationProgress = rotationProgress;
    ctx.hiddenFlipOut = std::clamp(ctx.rotationProgress * 2.0f, 0.0f, 1.0f);
    ctx.hiddenFlipIn  = std::clamp((ctx.rotationProgress - 0.5f) * 2.0f, 0.0f, 1.0f);
    ctx.showOutgoing  = showOutgoing;

    if (ctx.rotationSteps != 0)
    {
        if (ctx.hasHidden)
        {
            if (rotateBackward)
            {
                ctx.rotatingOutIndex = ctx.visibleCount;
                ctx.rotatingInIndex  = 0;
                ctx.flipCardIndex    = ctx.rotatingInIndex;
            }
            else
            {
                ctx.rotatingOutIndex = ctx.countInt - 1;
                ctx.rotatingInIndex  = ctx.visibleCount - 1;
                ctx.flipCardIndex    = ctx.rotatingOutIndex;
            }
        }
        else
        {
            const int cycleIndex = rotateBackward ? 0 : (ctx.countInt - 1);
            ctx.rotatingInIndex  = cycleIndex;
            ctx.rotatingOutIndex = cycleIndex;
            ctx.flipCardIndex    = cycleIndex;
        }
        ctx.useHiddenRotation = ctx.hasHidden;
    }

    return ctx;
}

float SteadyOpacityForRelative(const RotationContext& ctx, float relative)
{
    if (!ctx.hasHidden)
        return 1.0f;
    if (relative < 0.0f)
        return 1.0f;
    if (relative >= (float)ctx.visibleCount)
        return 0.5f;
    return (relative >= (float)(ctx.visibleCount - 1)) ? 0.5f : 1.0f;
}

bool IsVisibleInRotationView(const RotationContext& ctx, int position,
                             bool isVisibleInFinalView)
{
    if (isVisibleInFinalView)
    {
        if (position == ctx.rotatingInIndex)
        {
            if (!ctx.showOutgoing)
                return true;
        }
        else if (position != ctx.rotatingOutIndex)
        {
            return true;
        }
    }
    else if (position == ctx.rotatingInIndex)
    {
        if (!ctx.showOutgoing)
            return true;
    }

    if (position == ctx.rotatingOutIndex && ctx.showOutgoing)
        return true;

    return false;
}

CardRotationEntry BuildCardRotationEntry(int listIndex, const RotationContext& ctx)
{
    CardRotationEntry entry = {};
    entry.listIndex = listIndex;
    entry.endSlot   = listIndex;

    const int n   = ctx.countInt;
    const int vis = ctx.visibleCount;
    entry.startSlot = listIndex;
    if (ctx.rotationSteps != 0)
    {
        if (ctx.rotationSteps < 0)
            entry.startSlot = (listIndex + n - 1) % n;
        else
            entry.startSlot = (listIndex + 1) % n;
    }

    const bool isVis  = entry.endSlot < vis;
    const bool wasVis = entry.startSlot < vis;

    if (!ctx.isRotating)
    {
        entry.shouldDraw = isVis;
        return entry;
    }

    entry.isOutgoing = wasVis && !isVis;
    entry.isBoundaryIn = ctx.useHiddenRotation && !wasVis && isVis;
    entry.isIncoming = !ctx.useHiddenRotation && !wasVis && isVis;
    entry.isCycle = !ctx.hasHidden
        && ((ctx.rotationSteps < 0 && listIndex == 0)
            || (ctx.rotationSteps > 0 && listIndex == n - 1));

    entry.shouldDraw = IsVisibleInRotationView(ctx, listIndex, isVis);
    return entry;
}

float ResolveRotationPathSlot(const CardRotationEntry& entry, const RotationContext& ctx)
{
    if (!ctx.isRotating || ctx.rotationSteps == 0)
        return (float)entry.endSlot;

    const float p = ctx.rotationProgress;

    if (entry.isCycle)
    {
        if (ctx.rotationSteps > 0)
        {
            if (p < 0.5f)
                return (1.0f - p * 2.0f) * 0.0f + (p * 2.0f) * -1.0f;
            {
                const float phase = (p - 0.5f) * 2.0f;
                return (1.0f - phase) * (float)ctx.visibleCount
                     + phase * (float)(ctx.visibleCount - 1);
            }
        }
        if (p < 0.5f)
            return ((1.0f - p * 2.0f) * (float)(ctx.visibleCount - 1))
                 + (p * 2.0f) * (float)ctx.visibleCount;
        return ((p - 0.5f) * 2.0f) * 0.0f
             + (1.0f - (p - 0.5f) * 2.0f) * -1.0f;
    }

    if (entry.isOutgoing)
    {
        const float start = (ctx.rotationSteps > 0) ? 0.0f : (float)(ctx.visibleCount - 1);
        const float end   = (ctx.rotationSteps > 0) ? -1.0f : (float)ctx.visibleCount;
        return start + (end - start) * p;
    }

    if (entry.isBoundaryIn || entry.isIncoming)
    {
        const float start = (ctx.rotationSteps > 0) ? (float)ctx.visibleCount : -1.0f;
        const float end   = (float)entry.endSlot;
        return start + (end - start) * p;
    }

    float startRelative = (float)entry.startSlot;
    const float endRelative = (float)entry.endSlot;

    if (ctx.useHiddenRotation)
    {
        startRelative = endRelative + ((ctx.rotationSteps > 0) ? 1.0f : -1.0f);
        return startRelative + (endRelative - startRelative) * p;
    }

    if (ctx.rotationSteps < 0 && entry.startSlot > entry.endSlot)
        startRelative -= (float)ctx.countInt;

    return startRelative + (endRelative - startRelative) * p;
}

float ResolveRotationTransitionOpacity(const CardRotationEntry& entry,
                                       const RotationContext& ctx)
{
    if (!ctx.isRotating || ctx.rotationSteps == 0)
        return 1.0f;

    if (entry.isCycle)
    {
        if (ctx.rotationProgress < 0.5f)
            return 1.0f - ctx.rotationProgress * 2.0f;
        return (ctx.rotationProgress - 0.5f) * 2.0f;
    }
    if (entry.isOutgoing)
    {
        if (ctx.useHiddenRotation)
            return 1.0f - ctx.hiddenFlipOut;
        return 1.0f - 2.0f * std::min(ctx.rotationProgress, 0.5f);
    }
    if (entry.isBoundaryIn || entry.isIncoming)
    {
        if (ctx.useHiddenRotation)
            return ctx.hiddenFlipIn;
        return std::clamp((ctx.rotationProgress - 0.5f) * 2.0f, 0.0f, 1.0f);
    }
    return 1.0f;
}

float ResolveRotationWindowOpacity(const CardRotationEntry& entry,
                                   const RotationContext& ctx, float pathSlot)
{
    if (!ctx.isRotating || ctx.rotationSteps == 0)
        return SteadyOpacityForRelative(ctx, pathSlot);

    const float startSteady = SteadyOpacityForRelative(ctx, (float)entry.startSlot);
    const float endSteady   = SteadyOpacityForRelative(ctx, (float)entry.endSlot);

    if (entry.isOutgoing)
        return startSteady;
    if (entry.isIncoming)
        return endSteady;

    if (ctx.useHiddenRotation)
    {
        if (ctx.rotationSteps > 0)
        {
            if (entry.endSlot >= ctx.visibleCount - 1)
                return 0.5f;
            if (entry.endSlot == ctx.visibleCount - 2)
                return 0.5f + (1.0f - 0.5f) * ctx.rotationProgress;
            return 1.0f;
        }
        if (entry.endSlot == ctx.visibleCount - 1)
            return 1.0f + (0.5f - 1.0f) * ctx.rotationProgress;
        return 1.0f;
    }

    return startSteady + (endSteady - startSteady) * ctx.rotationProgress;
}

} // namespace

// ============================================================================
int Flip3DCompApp::DistanceBetween(size_t sourcePos, size_t targetPos,
                                   bool forward) const
{
    const size_t count = m_windowCards.size();
    if (count <= 1)
        return 0;

    size_t dist = 0;
    size_t cur  = sourcePos;
    while (cur != targetPos)
    {
        if (forward)
            cur = (cur + count - 1) % count;
        else
            cur = (cur + 1) % count;
        ++dist;
    }
    return (int)dist;
}

float Flip3DCompApp::RotationDurationForRotateList() const
{
    if (m_viewState == ViewState::ExitRepeatedRotate)
        return std::max(std::abs(m_repeatedRotateRate), 0.005f);
    return kRotateListDurationSec;
}

void Flip3DCompApp::StartRotationStep(bool backward, float durationSec)
{
    m_rotateBackward             = backward;
    m_showOutgoingCardDuringRotation = true;
    m_rotateAnimation.Restart(0.0f, 1.0f, durationSec, InterpolationMode::Linear);
}

void Flip3DCompApp::OnRotationTimeUpdated()
{
    if (m_showOutgoingCardDuringRotation && m_rotateAnimation.IsActive()
        && m_rotateAnimation.RawProgress() > 0.5f)
    {
        m_showOutgoingCardDuringRotation = false;
    }
}

void Flip3DCompApp::TickRepeatedRotate()
{
    if (m_rotateAnimation.IsActive() || m_windowCards.empty())
        return;

    if (m_viewState == ViewState::ExitRepeatedRotate)
    {
        if (m_repeatedRotateStepsRemaining > 0)
        {
            constexpr bool backward = false;
            RotateListPhysically(backward);
            --m_repeatedRotateStepsRemaining;
            StartRotationStep(backward, RotationDurationForRotateList());

            for (int k = 0; k < (int)m_windowCards.size(); ++k)
            {
                m_windowCards[(size_t)k].m_carouselDisplaySlot =
                    ComputeRotationDisplaySlot(k);
            }
        }
        else
        {
            m_repeatedRotateRate = 0.0f;
            m_exitScrollSnapshot  = 0.0f;
            m_scrollPosition           = 0.0f;
            m_scrollTargetPosition        = 0.0f;
        }
        return;
    }
}

float Flip3DCompApp::ComputeRotationDisplaySlot(int listIndex) const
{
    const bool isRotating = m_rotateAnimation.IsActive();
    const RotationContext ctx = BuildRotationContext(
        (int)m_windowCards.size(), isRotating, m_rotateBackward,
        isRotating ? m_rotateAnimation.RawProgress() : 1.0f,
        m_showOutgoingCardDuringRotation);
    if (!ctx.isRotating)
        return CardListSlot(listIndex);

    const CardRotationEntry entry = BuildCardRotationEntry(listIndex, ctx);
    const float scrollOffset = (m_viewState == ViewState::ExitRepeatedRotate)
        ? m_exitScrollSnapshot
        : m_scrollPosition;
    return ResolveRotationPathSlot(entry, ctx) - scrollOffset;
}

bool Flip3DCompApp::ShouldDrawRotationCard(int listIndex) const
{
    const bool isRotating = m_rotateAnimation.IsActive();
    const RotationContext ctx = BuildRotationContext(
        (int)m_windowCards.size(), isRotating, m_rotateBackward,
        isRotating ? m_rotateAnimation.RawProgress() : 1.0f,
        m_showOutgoingCardDuringRotation);
    if (!ctx.isRotating)
        return listIndex < ctx.visibleCount;

    const CardRotationEntry entry = BuildCardRotationEntry(listIndex, ctx);
    return entry.shouldDraw;
}

float Flip3DCompApp::ComputeRotationAlpha(const CardModel& card, float enterProgress,
                                          int listIndex, float carouselSlot) const
{
    const bool isRotating = m_rotateAnimation.IsActive();
    const RotationContext ctx = BuildRotationContext(
        (int)m_windowCards.size(), isRotating, m_rotateBackward,
        isRotating ? m_rotateAnimation.RawProgress() : 1.0f,
        m_showOutgoingCardDuringRotation);
    const CardRotationEntry entry = BuildCardRotationEntry(listIndex, ctx);

    if (!entry.shouldDraw && ctx.isRotating)
        return 0.0f;

    const float transitionOpacity = ResolveRotationTransitionOpacity(entry, ctx);
    float windowOpacity = ResolveRotationWindowOpacity(entry, ctx, carouselSlot);

    if (std::abs(enterProgress - 1.0f) >= 1e-6f
        && std::abs(windowOpacity - 1.0f) >= 1e-6f)
    {
        windowOpacity = enterProgress * windowOpacity + (1.0f - enterProgress);
    }

    float enterScale = 1.0f;
    if (std::abs(enterProgress - 1.0f) >= 1e-6f)
    {
        if (card.m_isMinimized)
            enterScale = enterProgress * enterProgress;
        else if (card.m_isShellDesktop)
            enterScale = enterProgress;
    }

    return std::clamp(transitionOpacity * windowOpacity * enterScale, 0.0f, 1.0f);
}


// ===== Ported from src/Flip3DComp_Input.cpp =====
// ============================================================================
// Flip3DComp_Input.cpp — Keyboard, mouse, and wheel input handling
// ============================================================================
#include <cmath>

// ============================================================================
// Flip3DCompApp::OnWheel
// Modern smooth scroll: each WHEEL_DELTA notch nudges the scroll target by one
// slot. Wheel down (delta < 0) scrolls front→back; wheel up scrolls back→front.
// ============================================================================
bool Flip3DCompApp::OnWheel(int wheelDelta)
{
    if (wheelDelta == 0 ||
        m_viewState == ViewState::Exit ||
        m_viewState == ViewState::ExitRepeatedRotate)
        return false;

    const float scaled = (float)wheelDelta / (float)WHEEL_DELTA * kScrollWheelNotchFraction;
    int deltaSlots = (int)std::round(scaled);
    if (deltaSlots == 0 && wheelDelta != 0)
        deltaSlots = (wheelDelta > 0) ? 1 : -1;
    m_pendingWheelSlots += deltaSlots;
    m_lastMouseWheelTime = std::chrono::steady_clock::now();
    return true;
}

// ============================================================================
// Flip3DCompApp::OnKey
// ============================================================================
bool Flip3DCompApp::OnKey(bool down, UINT vkCode, LPARAM lParam)
{
    if (!down)
    {
        if (vkCode == m_heldNavigationVirtualKey)
        {
            m_heldNavigationVirtualKey = 0;
            m_heldNavigationDirection = 0;
            m_heldNavigationStartTime = {};
            m_scrollTargetPosition = std::round(m_scrollTargetPosition);
        }
        return false;
    }

    if (m_viewState == ViewState::Exit ||
        m_viewState == ViewState::ExitRepeatedRotate)
        return false;

    // Detect Windows autorepeat: bit 30 of lParam is set if the key was
    // previously down. Throttle repeated keydown processing so holding an
    // arrow or tab key doesn't rotate the carousel too quickly.
    const bool isRepeat = (lParam & (1 << 30)) != 0;
    if (isRepeat)
        return true; // held navigation is driven from the frame update

    switch (vkCode)
    {
    case VK_ESCAPE:
        ExitView();
        return true;

    case VK_TAB:
    {
        const int direction = (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? -1 : 1;
        m_heldNavigationVirtualKey = vkCode;
        m_heldNavigationDirection = direction;
        m_heldNavigationStartTime = std::chrono::steady_clock::now();
        RotateBy(direction);
        return true;
    }

    case VK_UP:
        m_heldNavigationVirtualKey = vkCode;
        m_heldNavigationDirection = -1;
        m_heldNavigationStartTime = std::chrono::steady_clock::now();
        RotateBy(-1);
        return true;

    case VK_DOWN:
        m_heldNavigationVirtualKey = vkCode;
        m_heldNavigationDirection = 1;
        m_heldNavigationStartTime = std::chrono::steady_clock::now();
        RotateBy(1);
        return true;

    case VK_LEFT:
        m_heldNavigationVirtualKey = vkCode;
        m_heldNavigationDirection = m_isRightToLeft ? 1 : -1;
        m_heldNavigationStartTime = std::chrono::steady_clock::now();
        RotateBy(m_isRightToLeft ? 1 : -1);
        return true;

    case VK_RIGHT:
        m_heldNavigationVirtualKey = vkCode;
        m_heldNavigationDirection = m_isRightToLeft ? -1 : 1;
        m_heldNavigationStartTime = std::chrono::steady_clock::now();
        RotateBy(m_isRightToLeft ? -1 : 1);
        return true;

    case VK_HOME:
        RotateToWindow(m_initialFrontWindowHandle);
        return true;

    case VK_F5:
        ReplayEnterAnimation();
        return true;

    case VK_RETURN:
    case VK_SPACE:
        SelectFront();
        return true;
    }

    return false;
}

// ============================================================================
// Flip3DCompApp::OnMouse
// ============================================================================
bool Flip3DCompApp::OnMouse(LONG x, LONG y, bool pressed)
{
    if (!pressed ||
        m_viewState == ViewState::Exit ||
        m_viewState == ViewState::ExitRepeatedRotate)
        return false;

    HWND hit = HitTest3DScene(x, y);
    if (hit)
        SelectWindow(hit);
    else
        ExitView();

    return true;
}


// ===== Ported from src/Flip3DComp_View.cpp =====
// ============================================================================
// Flip3DComp_View.cpp — View state: Exit, Select, HitTest3DScene
// ============================================================================
#include <algorithm>
#include <cmath>

// ============================================================================
// Flip3DCompApp::ExitView
// ============================================================================
void Flip3DCompApp::ExitView(bool commitScroll, float exitDurationSec)
{
    if (m_viewState == ViewState::Exit || m_viewState == ViewState::ExitRepeatedRotate)
        return;

    if (commitScroll)
        CommitCarouselScroll();

    m_lastPaintOrder.clear();
    m_viewState = ViewState::Exit;
    NotifyAccessibilityEvent(EVENT_SYSTEM_DIALOGEND);
    m_enterExitAnimation.Restart(EnterProgress(), 0.0f, exitDurationSec,
                        kEnableAnimationEasing
                            ? InterpolationMode::CubicBezier
                            : InterpolationMode::Linear);
}

// ============================================================================
// Flip3DCompApp::BeginExitView — uDWM CFlip3D::BeginExitView (parallel flatten)
// ============================================================================
void Flip3DCompApp::BeginExitView()
{
    if (m_viewState == ViewState::Exit || m_viewState == ViewState::ExitRepeatedRotate)
        return;

    m_lastPaintOrder.clear();
    NotifyAccessibilityEvent(EVENT_SYSTEM_DIALOGEND);
    m_enterExitAnimation.Restart(EnterProgress(), 0.0f, kExitDurationSec,
                        kEnableAnimationEasing
                            ? InterpolationMode::CubicBezier
                            : InterpolationMode::Linear);
    m_viewState = ViewState::Exit;
}

// ============================================================================
// Flip3DCompApp::SelectFront
// ============================================================================
void Flip3DCompApp::SelectFront()
{
    if (m_windowCards.empty())
        return;

    int   bestIdx  = 0;
    float bestSlot = 1e9f;
    for (int i = 0; i < (int)m_windowCards.size(); ++i)
    {
        const float slot = GetCardDisplaySlot(i);
        if (slot < -0.5f)
            continue;
        if (slot < bestSlot)
        {
            bestSlot = slot;
            bestIdx  = i;
        }
    }

    SelectWindow(m_windowCards[(size_t)bestIdx].m_windowHandle);
}

// ============================================================================
// Flip3DCompApp::SelectWindow — uDWM: BeginExitView then ExitRepeatedRotate
// ============================================================================
void Flip3DCompApp::SelectWindow(HWND hwndTarget)
{
    if (!hwndTarget || !IsWindow(hwndTarget))
        return;

    const bool isShell = (hwndTarget == GetShellWindow());

    if (isShell)
    {
        if (HWND shellTray = FindWindowW(L"Shell_TrayWnd", nullptr))
            PostMessageW(shellTray, 0x579, 1, 0);
    }
    else if (!IsWindowEnabled(hwndTarget))
    {
        SwitchToThisWindow(
            GetLastActivePopup(GetAncestor(hwndTarget, GA_ROOTOWNER)), TRUE);
    }
    else
    {
        SwitchToThisWindow(hwndTarget, TRUE);
    }

    EnsureTopmost();

    const int selIdx = FindCardIndex(hwndTarget);
    if (selIdx < 0)
    {
        ExitView();
        return;
    }

    if (m_windowCards[(size_t)selIdx].m_isMinimized)
        UpdateCardGeometry(m_windowCards[(size_t)selIdx], m_monitorWidth, m_monitorHeight, /*selectedRestore=*/true);

    m_selectedWindowHandle = hwndTarget;
    m_lastPaintOrder.clear();

    FreezeCarouselVisuals();

    BeginExitView();

    // Re-index after wrap may have rotated the list; step count = list index of selection.
    const int selIdxAfter = FindCardIndex(hwndTarget);
    if (selIdxAfter > 0)
    {
        m_repeatedRotateRate = -(kExitDurationSec / (float)selIdxAfter);
        m_repeatedRotateStepsRemaining = selIdxAfter;
        m_viewState = ViewState::ExitRepeatedRotate;
        TickRepeatedRotate();
    }
}

// ============================================================================
// Flip3DCompApp::HitTest3DScene
// ============================================================================
HWND Flip3DCompApp::HitTest3DScene(LONG screenX, LONG screenY) const
{
    if (m_windowCards.empty())
        return nullptr;

    const float p    = EnterProgress();
    const auto  cam  = BuildCameraMatrix(p);

    float bestNdcZ = 1e10f;
    HWND  bestHwnd = nullptr;

    for (int ki = (int)m_windowCards.size() - 1; ki >= 0; --ki)
    {
        const CardModel& c = m_windowCards[(size_t)ki];
        if (!c.m_cardTransformVisual)
            continue;

        const float slot = GetCardDisplaySlot(ki);
        if (slot <= -0.5f || slot >= (float)kMaxVisibleCards)
            continue;

        float t   = ComputeCarouselBezierT(slot);
        const float flatRank = ComputeFlatDepthRank(slot, p, ki);
        auto  MVP = Math::Multiply(BuildModelMatrix(c, t, p, flatRank), cam);

        float sw = (float)std::max(c.m_thumbnailSourceWidth,  1);
        float sh = (float)std::max(c.m_thumbnailSourceHeight, 1);

        auto project = [&](float px, float py) -> Vec2
        {
            float x = px*MVP.m[0][0] + py*MVP.m[1][0] + MVP.m[3][0];
            float y = px*MVP.m[0][1] + py*MVP.m[1][1] + MVP.m[3][1];
            float w = px*MVP.m[0][3] + py*MVP.m[1][3] + MVP.m[3][3];
            if (fabsf(w) < 1e-6f) w = 1e-6f;
            return { x / w, y / w };
        };

        Vec2 c0 = project(0.0f, 0.0f);
        Vec2 c1 = project(sw,    0.0f);
        Vec2 c2 = project(sw,    sh);
        Vec2 c3 = project(0.0f,  sh);

        float sx = (float)screenX;
        float sy = (float)screenY;

        auto cross = [](float x1, float y1, float x2, float y2) {
            return x1 * y2 - y1 * x2;
        };

        float d0 = cross(c1.x - c0.x, c1.y - c0.y, sx - c0.x, sy - c0.y);
        float d1 = cross(c2.x - c1.x, c2.y - c1.y, sx - c1.x, sy - c1.y);
        float d2 = cross(c3.x - c2.x, c3.y - c2.y, sx - c2.x, sy - c2.y);
        float d3 = cross(c0.x - c3.x, c0.y - c3.y, sx - c3.x, sy - c3.y);

        bool inside = (d0 >= 0 && d1 >= 0 && d2 >= 0 && d3 >= 0)
                   || (d0 <= 0 && d1 <= 0 && d2 <= 0 && d3 <= 0);

        if (!inside)
            continue;

        float pixZ = 0.0f*MVP.m[0][2] + 0.0f*MVP.m[1][2] + MVP.m[3][2];
        float pixW = 0.0f*MVP.m[0][3] + 0.0f*MVP.m[1][3] + MVP.m[3][3];
        float ndcZ = pixW != 0.0f ? pixZ / pixW : 0.0f;

        if (ndcZ < bestNdcZ)
        {
            bestNdcZ = ndcZ;
            bestHwnd = c.m_windowHandle;
        }
    }

    return bestHwnd;
}
        

// ===== Ported from src/Flip3DComp_ShellHook.cpp =====
// ============================================================================
// Flip3DComp_ShellHook.cpp — dynamic card list via RegisterShellHookWindow
// ============================================================================
#include <algorithm>

namespace
{
    void SetWindowTransitionsDisabled(HWND hwnd, bool disabled) {
        if (!hwnd || !IsWindow(hwnd))
            return;

        const BOOL value = disabled ? TRUE : FALSE;
        DwmSetWindowAttribute(hwnd, DWMWA_TRANSITIONS_FORCEDISABLED,
            &value, sizeof(value));
    }
}

// ============================================================================
// Flip3DCompApp::QualifiesForView
// ============================================================================
bool Flip3DCompApp::QualifiesForView(HWND hwnd) const
{
    if (!hwnd || hwnd == m_windowHandle || hwnd == GetDesktopWindow())
        return false;

    if (!IsWindowVisible(hwnd))
        return false;

    if (m_selectedWindowHandle && hwnd == m_selectedWindowHandle)
        return true;

    const LONG_PTR style   = GetWindowLongPtrW(hwnd, GWL_STYLE);
    const LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);

    if ((style & WS_CHILD) != 0)
        return false;

    const bool isShellDesktop = (hwnd == GetShellWindow());

    if (!isShellDesktop
        && (exStyle & 0x08000080) != 0
        && (exStyle & WS_EX_APPWINDOW) == 0)
        return false;

    if (!isShellDesktop && (exStyle & WS_EX_TOOLWINDOW) != 0)
        return false;

    if (!isShellDesktop
        && (exStyle & WS_EX_APPWINDOW) == 0
        && GetWindow(hwnd, GW_OWNER) != nullptr)
        return false;

    if (IsNeverHiddenWindow(hwnd))
        return false;

    if (!isShellDesktop)
    {
        DWORD cloaked = 0;
        if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked)))
            && cloaked != 0)
            return false;
    }

    return true;
}

// ============================================================================
bool Flip3DCompApp::IsFlip3DViewActive() const
{
    return m_viewState != ViewState::Inactive;
}

bool Flip3DCompApp::IsNeverHiddenWindow(HWND hwnd) const
{
    if (!hwnd)
        return true;

    if (hwnd == m_windowHandle)
        return true;

    wchar_t cls[64] = {};
    if (!GetClassNameW(hwnd, cls, 63))
        return false;

    return !_wcsicmp(cls, L"Shell_TrayWnd")
        || !_wcsicmp(cls, L"Shell_SecondaryTrayWnd")
        || !_wcsicmp(cls, L"WorkerW");
}

void Flip3DCompApp::SetCardTransitionsDisabled(bool disabled)
{
    for (const CardModel& card : m_windowCards)
        SetWindowTransitionsDisabled(card.m_windowHandle, disabled);
}

// ============================================================================
void Flip3DCompApp::EnterFlip3DWindowMode()
{
    if (!m_windowHandle || m_isShellHookRegistered)
        return;

    SetCardTransitionsDisabled(true);

    if (!m_shellHookMessage)
        m_shellHookMessage = RegisterWindowMessageW(L"SHELLHOOK");

    if (RegisterShellHookWindow(m_windowHandle))
        m_isShellHookRegistered = true;
}

void Flip3DCompApp::LeaveFlip3DWindowMode()
{
    SetCardTransitionsDisabled(false);

    if (m_windowHandle && m_isShellHookRegistered)
    {
        DeregisterShellHookWindow(m_windowHandle);
        m_isShellHookRegistered = false;
    }
}

// ============================================================================
void Flip3DCompApp::OnShellHookMessage(WPARAM wParam, LPARAM lParam)
{
    if (!IsFlip3DViewActive())
        return;

    const UINT code = (UINT)(wParam & ~HSHELL_HIGHBIT);
    const HWND hwnd = (HWND)lParam;

    switch (code)
    {
    case HSHELL_WINDOWCREATED:
    case HSHELL_WINDOWACTIVATED:
    case HSHELL_WINDOWREPLACED:
    case HSHELL_RUDEAPPACTIVATED:
        if (hwnd)
            OnWindowShowHide(hwnd);
        break;

    case HSHELL_WINDOWDESTROYED:
        if (hwnd)
        {
            const int idx = FindCardIndex(hwnd);
            if (idx >= 0)
                RemoveCardAt((size_t)idx);
        }
        break;

    default:
        break;
    }
}

// ============================================================================
void Flip3DCompApp::OnWindowShowHide(HWND hwnd)
{
    if (!IsFlip3DViewActive() || !hwnd || !IsWindow(hwnd))
        return;

    if (hwnd == m_windowHandle || IsNeverHiddenWindow(hwnd))
        return;

    const bool qualifies = QualifiesForView(hwnd);
    const int  cardIdx   = FindCardIndex(hwnd);

    if (qualifies)
    {
        if (cardIdx < 0)
            AddCardForWindow(hwnd);
    }
    else if (cardIdx >= 0)
    {
        RemoveCardAt((size_t)cardIdx);
    }
}

// ============================================================================
int Flip3DCompApp::FindCardIndex(HWND hwnd) const
{
    if (!hwnd)
        return -1;

    for (int i = 0; i < (int)m_windowCards.size(); ++i)
    {
        if (m_windowCards[(size_t)i].m_windowHandle == hwnd)
            return i;
    }
    return -1;
}

// ============================================================================
HRESULT Flip3DCompApp::CreateCardVisual(CardModel& card)
{
    if (!m_compositionDevice || !m_sceneCompositionVisual || !card.m_windowHandle)
        return E_INVALIDARG;

    DWM_THUMBNAIL_PROPERTIES tp = {};
    tp.dwFlags   = DWM_TNP_VISIBLE | DWM_TNP_RECTDESTINATION
                 | DWM_TNP_ENABLE3D | DWM_TNP_DISABLEFORCECVI;
    tp.fVisible  = TRUE;
    tp.rcDestination = { 0, 0, card.m_thumbnailSourceWidth, card.m_thumbnailSourceHeight };

    void* pv = nullptr;
    HRESULT hr = m_createSharedThumbnailVisual(
        m_windowHandle,
        card.m_windowHandle,
        DWM_TNF_DWMWINDOW,
        &tp,
        m_compositionDevice.Get(),
        &pv,
        card.m_thumbnailRegistration.put());

    if (FAILED(hr) || !pv)
        return FAILED(hr) ? hr : E_FAIL;

    ComPtr<IDCompositionVisual> thumbBase;
    thumbBase.Attach((IDCompositionVisual*)pv);
    hr = thumbBase.As(&card.m_thumbnailVisual);
    if (FAILED(hr))
        return hr;

    ComPtr<IDCompositionVisual2> container;
    hr = m_compositionDevice->CreateVisual(&container);
    if (FAILED(hr))
        return hr;

    hr = container->AddVisual(card.m_thumbnailVisual.Get(), FALSE, nullptr);
    if (FAILED(hr))
        return hr;

    hr = container.As(&card.m_cardTransformVisual);
    if (FAILED(hr))
        return hr;

    hr = m_sceneCompositionVisual->AddVisual(container.Get(), TRUE, nullptr);
    return hr;
}

// ============================================================================
bool Flip3DCompApp::AddCardForWindow(HWND hwnd)
{
    if (!hwnd || m_windowCards.size() >= (size_t)kMaxCards)
        return false;

    if (FindCardIndex(hwnd) >= 0)
        return false;

    CardModel card;
    card.m_windowHandle                 = hwnd;
    card.m_initialCarouselIndex = (int)m_windowCards.size();
    UpdateCardGeometry(card, m_monitorWidth, m_monitorHeight);

    if (FAILED(CreateCardVisual(card)))
        return false;

    m_windowCards.push_back(std::move(card));
    SetWindowTransitionsDisabled(hwnd, true);

    if (m_compositionDevice)
        m_compositionDevice->Commit();

    return true;
}

// ============================================================================
void Flip3DCompApp::RemoveCardAt(size_t index)
{
    if (index >= m_windowCards.size())
        return;

    CardModel& card = m_windowCards[index];

    if (card.m_cardTransformVisual && m_sceneCompositionVisual)
    {
        ComPtr<IDCompositionVisual> sceneBase;
        if (SUCCEEDED(m_sceneCompositionVisual.As(&sceneBase)))
            sceneBase->RemoveVisual(card.m_cardTransformVisual.Get());
    }

    if (card.m_thumbnailRegistration)
    {
        card.m_thumbnailRegistration.reset();
    }

    SetWindowTransitionsDisabled(card.m_windowHandle, false);

    card.m_thumbnailVisual.Reset();
    card.m_cardTransformVisual.Reset();

    m_windowCards.erase(m_windowCards.begin() + (ptrdiff_t)index);

    if (m_compositionDevice)
        m_compositionDevice->Commit();
}


// ===== Ported from src/Flip3DComp_Accessible.cpp =====
// ============================================================================
// Flip3DComp_Accessible.cpp — Accessibility helpers + NotifyWinEvent (uDWM port)
// ============================================================================
#include <algorithm>
#include <cmath>

// ============================================================================
// Flip3DCompApp::InitAccessibility
// ============================================================================
bool Flip3DCompApp::InitAccessibility()
{
    if (m_isComInitialized)
        return true;

    const HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (hr == RPC_E_CHANGED_MODE)
    {
        // Another module initialized COM with a different model; still usable.
        m_isComInitialized = true;
        return true;
    }

    if (FAILED(hr))
        return false;

    m_isComInitialized    = true;
    m_shouldUninitializeCom    = true;
    return true;
}

void Flip3DCompApp::ShutdownAccessibility()
{
    if (m_accessibleObject)
    {
        m_accessibleObject->Release();
        m_accessibleObject = nullptr;
    }

    if (m_shouldUninitializeCom)
    {
        CoUninitialize();
        m_shouldUninitializeCom = false;
    }
    m_isComInitialized = false;
}

IAccessible* Flip3DCompApp::GetAccessibleObject()
{
    if (!m_accessibleObject)
    {
        IAccessible* pAcc = nullptr;
        if (FAILED(Flip3DAccessible::Create(this, &pAcc)))
            return nullptr;
        m_accessibleObject = pAcc;
    }
    return m_accessibleObject;
}

// ============================================================================
// Flip3DCompApp::NotifyAccessibilityEvent
// uDWM: 0x14 dialog start, 0x15 dialog end, 0x8005 focus on front child.
// ============================================================================
void Flip3DCompApp::NotifyAccessibilityEvent(DWORD event, LONG childId)
{
    if (!m_windowHandle)
        return;

    NotifyWinEvent(event, m_windowHandle, OBJID_CLIENT, childId);
}

void Flip3DCompApp::NotifyAccessibilityFocusFront()
{
    NotifyAccessibilityEvent(EVENT_OBJECT_FOCUS, 1);
}

// ============================================================================
// Flip3DCompApp::AccessibleChildCount
// uDWM CFlip3DAccessible::GetChildrenCount — front→back inclusive stack.
// ============================================================================
int Flip3DCompApp::AccessibleChildCount() const
{
    return (int)m_windowCards.size();
}

// ============================================================================
// Flip3DCompApp::AccessibleWindowName
// ============================================================================
HRESULT Flip3DCompApp::AccessibleWindowName(int index, BSTR* pszName) const
{
    if (!pszName)
        return E_POINTER;

    *pszName = nullptr;

    if (index < 0 || index >= (int)m_windowCards.size())
        return E_INVALIDARG;

    const CardModel& card = m_windowCards[(size_t)index];
    wchar_t title[512] = {};
    if (card.m_windowHandle && IsWindow(card.m_windowHandle))
        GetWindowTextW(card.m_windowHandle, title, (int)std::size(title));

    *pszName = SysAllocString(title);
    return *pszName ? S_OK : E_OUTOFMEMORY;
}

// ============================================================================
// Flip3DCompApp::AccessibleCardScreenRect
// Project the live 3D card quad to screen pixels (uDWM GetFlip3DWindowBoundingBox).
// ============================================================================
bool Flip3DCompApp::AccessibleCardScreenRect(int index, long* pxLeft, long* pyTop,
                                             long* pcxWidth, long* pcyHeight) const
{
    if (!pxLeft || !pyTop || !pcxWidth || !pcyHeight
        || index < 0 || index >= (int)m_windowCards.size())
        return false;

    const CardModel& c = m_windowCards[(size_t)index];

    const float p         = EnterProgress();
    const auto  camera    = BuildCameraMatrix(p);
    const float carouselSlot = GetCardDisplaySlot(index);

    const float t        = ComputeCarouselBezierT(carouselSlot);
    const float flatRank = ComputeFlatDepthRank(carouselSlot, p, index);
    const auto  model    = BuildModelMatrix(c, t, p, flatRank);
    const auto  mvp      = Math::Multiply(model, camera);

    const float sw = (float)std::max(c.m_thumbnailSourceWidth,  1);
    const float sh = (float)std::max(c.m_thumbnailSourceHeight, 1);

    auto project = [&](float px, float py) -> Vec2
    {
        const float x = px * mvp.m[0][0] + py * mvp.m[1][0] + mvp.m[3][0];
        const float y = px * mvp.m[0][1] + py * mvp.m[1][1] + mvp.m[3][1];
        float       w = px * mvp.m[0][3] + py * mvp.m[1][3] + mvp.m[3][3];
        if (std::fabs(w) < 1e-6f)
            w = 1e-6f;
        return { x / w, y / w };
    };

    const Vec2 corners[4] = {
        project(0.0f, 0.0f),
        project(sw,    0.0f),
        project(sw,    sh),
        project(0.0f,  sh),
    };

    float minX =  1e10f, minY =  1e10f;
    float maxX = -1e10f, maxY = -1e10f;
    for (const Vec2& v : corners)
    {
        minX = std::min(minX, v.x);
        minY = std::min(minY, v.y);
        maxX = std::max(maxX, v.x);
        maxY = std::max(maxY, v.y);
    }

    POINT origin = { 0, 0 };
    ClientToScreen(m_windowHandle, &origin);

    *pxLeft   = origin.x + (long)std::floor(minX);
    *pyTop    = origin.y + (long)std::floor(minY);
    *pcxWidth  = (long)std::ceil(maxX - minX);
    *pcyHeight = (long)std::ceil(maxY - minY);
    if (*pcxWidth < 0)  *pcxWidth  = 0;
    if (*pcyHeight < 0) *pcyHeight = 0;
    return true;
}

// ============================================================================
// Flip3DCompApp::AccessibleHitTest
// Screen coordinates → carousel list index, or -1 if no card.
// ============================================================================
int Flip3DCompApp::AccessibleHitTest(long screenX, long screenY) const
{
    POINT pt = { (int)screenX, (int)screenY };
    ScreenToClient(m_windowHandle, &pt);

    HWND hit = HitTest3DScene((LONG)pt.x, (LONG)pt.y);
    if (!hit)
        return -1;

    // uDWM: child id = DistanceBetween(front, hit, false); list index matches.
    return FindCardIndex(hit);
}

// ============================================================================
// Flip3DCompApp::AccessiblePointInView
// ============================================================================
bool Flip3DCompApp::AccessiblePointInView(POINT screenPt) const
{
    RECT rc = {};
    if (!GetWindowRect(m_windowHandle, &rc))
        return false;

    return PtInRect(&rc, screenPt) != FALSE;
}

// ============================================================================
// Flip3DCompApp::AccessibleRotateToIndex
// accSelect → smooth scroll the window to the carousel front.
// ============================================================================
HRESULT Flip3DCompApp::AccessibleRotateToIndex(int index)
{
    if (index < 0 || index >= (int)m_windowCards.size())
        return E_INVALIDARG;

    if (m_viewState == ViewState::Exit || m_viewState == ViewState::ExitRepeatedRotate)
        return DISP_E_MEMBERNOTFOUND;

    RotateToWindow(m_windowCards[(size_t)index].m_windowHandle);
    return S_OK;
}

// ============================================================================
// Flip3DCompApp::AccessibleSelectIndex
// uDWM accDoDefaultAction → SelectWindow.
// ============================================================================
HRESULT Flip3DCompApp::AccessibleSelectIndex(int index)
{
    if (index < 0 || index >= (int)m_windowCards.size())
        return E_INVALIDARG;

    SelectWindow(m_windowCards[(size_t)index].m_windowHandle);
    return S_OK;
}


// ============================================================================
// Windhawk integration layer — run the ported compositor in-process
// ============================================================================
namespace {
constexpr UINT kHotkeyWinTab = 0xF31D;
constexpr UINT kHotkeyManual = 0xF31E;
constexpr UINT kHotkeyShiftTab = 0xF31F;
constexpr UINT kMsgOpenOrAdvance = WM_APP + 0x351;
constexpr UINT kMsgCommitSelection = WM_APP + 0x352;
constexpr UINT kMsgStopController = WM_APP + 0x353;
constexpr wchar_t kControllerClass[] = L"WindhawkFlip3DCompController";
constexpr ULONGLONG kDuplicateChordMs = 90;

int s_moduleAddressAnchor = 0;
HMODULE GetThisModuleHandle() {
    HMODULE module = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            reinterpret_cast<LPCWSTR>(&s_moduleAddressAnchor),
                            &module) || !module) {
        return GetModuleHandleW(nullptr);
    }
    return module;
}

HANDLE s_controllerThreadHandle = nullptr;
DWORD s_controllerThreadId = 0;
HANDLE s_controllerReadyEvent = nullptr;
HWND s_controllerWindow = nullptr;
HHOOK s_keyboardHook = nullptr;
HANDLE s_hookThreadHandle = nullptr;
DWORD s_hookThreadId = 0;
HANDLE s_hookReadyEvent = nullptr;
std::unique_ptr<Flip3DCompApp> s_flip3dApp;
std::atomic<bool> s_isLeftWinKeyHeld{false};
std::atomic<bool> s_isRightWinKeyHeld{false};
std::atomic<bool> s_isTabKeyConsumed{false};
bool s_isWinTabSessionOpen = false;
bool s_isManualSessionOpen = false;
bool s_shutdownRequested = false;
bool s_isWinTabHotkeyRegistered = false;
bool s_isManualHotkeyRegistered = false;
bool s_isShiftTabHotkeyRegistered = false;
ULONGLONG s_lastChordTickMs = 0;
std::atomic<bool> s_controllerReady{false};
std::atomic<bool> s_initializationSucceeded{false};

bool IsWinKeyHeld() { return s_isLeftWinKeyHeld || s_isRightWinKeyHeld; }

// Win-key conflict handling (other mods such as the Win 8.1 Start Screen
// recreation, or Windows itself, treat "Win pressed and released alone" as a
// Start trigger). Because this mod consumes Tab while Win is held, other
// low-level hooks never see a key between Win-down and Win-up and fire Start
// when Win is released. Inject one unassigned key (VK 0xE8) while Win is held
// so every other hook and the shell see a Win+<key> combination instead.
// Injected events are ignored by this mod's own hook (LLKHF_INJECTED).
std::atomic<bool> s_isWinMaskKeyInjected{false};

void InjectWinMaskKeystroke() {
    if (s_isWinMaskKeyInjected) return;
    s_isWinMaskKeyInjected = true;
    INPUT in[2] = {};
    in[0].type = INPUT_KEYBOARD;
    in[0].ki.wVk = 0xE8;
    in[0].ki.dwExtraInfo = 0xF31DF31D;
    in[1] = in[0];
    in[1].ki.dwFlags = KEYEVENTF_KEYUP;
    if (SendInput(2, in, sizeof(INPUT)) != 2)
        Wh_Log(L"SendInput (Win mask key) failed: %u", GetLastError());
}

LONG CALLBACK CrashLogVectoredHandler(PEXCEPTION_POINTERS info) {
    const DWORD code = info->ExceptionRecord->ExceptionCode;
    if (code == 0xC0000005 || code == 0xC0000094 || code == 0xC00000FD ||
        code == 0xC0000409 || code == 0xC000001D) {
        HMODULE m = nullptr;
        GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (LPCWSTR)info->ExceptionRecord->ExceptionAddress, &m);
        wchar_t name[MAX_PATH] = {};
        if (m) GetModuleFileNameW(m, name, MAX_PATH);
        Wh_Log(L"[CRASH] code=0x%08X addr=%p module=%s offset=0x%llX", code,
               info->ExceptionRecord->ExceptionAddress, name,
               (unsigned long long)((BYTE*)info->ExceptionRecord->ExceptionAddress - (BYTE*)m));
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

// True while a Flip3DCompApp session is on this thread's stack (starting,
// running or closing). It stops a hotkey dispatched during the closing phase
// from creating a second app on top of the one whose Run() is still executing.
bool s_isSessionOnStack = false;

void OpenOrAdvanceSwitcher(bool winChord) {
    Wh_Log(L"[OpenOrAdvanceSwitcher] enter winChord=%d running=%d", (int)winChord, (int)(s_flip3dApp != nullptr));
    PVOID vh = AddVectoredExceptionHandler(1, CrashLogVectoredHandler);
    struct VehGuard { PVOID h; ~VehGuard() { if (h) RemoveVectoredExceptionHandler(h); } } vehGuard{vh};
    // Runs on every exit path of the outermost call (success, failed start,
    // exception): releases the session flag and, if the mod was disabled in
    // the meantime, destroys the controller so WhTool_ModUninit can finish.
    struct SessionGuard {
        bool armed = false;
        ~SessionGuard() {
            if (!armed) return;
            s_isSessionOnStack = false;
            if (s_shutdownRequested && s_controllerWindow && IsWindow(s_controllerWindow))
                DestroyWindow(s_controllerWindow);
        }
    } sessionGuard;
    try {
    const ULONGLONG now = GetTickCount64();
    if (s_lastChordTickMs && now - s_lastChordTickMs < kDuplicateChordMs) return;
    s_lastChordTickMs = now;

    if (winChord) InjectWinMaskKeystroke();

    if (s_isSessionOnStack) {
        if (s_flip3dApp && IsWindow(s_flip3dApp->WindowHandle())) {
            HWND appWindow = s_flip3dApp->WindowHandle();
            PostMessageW(appWindow, WM_KEYDOWN, VK_TAB, 0);
            PostMessageW(appWindow, WM_KEYUP, VK_TAB, 0);
            if (winChord) s_isWinTabSessionOpen = true;
            else s_isManualSessionOpen = !s_isManualSessionOpen;
        }
        return;  // Never start a second session over a live one.
    }
    s_isSessionOnStack = true;
    sessionGuard.armed = true;

    s_flip3dApp = std::make_unique<Flip3DCompApp>();
    HINSTANCE instance = GetThisModuleHandle();
    Wh_Log(L"[OpenOrAdvanceSwitcher] Initialize...");
    if (!s_flip3dApp->Initialize(instance)) {
        const wchar_t* why = s_flip3dApp->InitErrorMessage();
        Wh_Log(L"Flip3DComp initialization failed: %s", why ? why : L"unknown error");
        s_flip3dApp.reset();
        return;
    }

    HWND appWindow = s_flip3dApp->WindowHandle();
    ShowWindow(appWindow, SW_SHOW);
    SetForegroundWindow(appWindow);
    UpdateWindow(appWindow);
    Wh_Log(L"Flip3DComp window shown (hwnd=%p)", appWindow);
    s_isWinTabSessionOpen = winChord;
    s_isManualSessionOpen = !winChord;

    // Run owns the UI thread's message pump. It dispatches controller-window
    // messages too, so later hotkey steps and Win-release selection still work.
    s_flip3dApp->Run();
    s_flip3dApp.reset();
    s_isWinTabSessionOpen = false;
    s_isManualSessionOpen = false;
    if (s_shutdownRequested && s_controllerWindow && IsWindow(s_controllerWindow)) {
        DestroyWindow(s_controllerWindow);
    }
    } catch (const std::exception&) {
        Wh_Log(L"C++ exception while starting/running Flip3DComp");
    } catch (...) {
        Wh_Log(L"Unknown exception while starting/running Flip3DComp");
    }

    if (s_flip3dApp) {
        HWND failedWindow = s_flip3dApp->WindowHandle();
        if (failedWindow && IsWindow(failedWindow)) DestroyWindow(failedWindow);
        s_flip3dApp.reset();
    }
    s_isWinTabSessionOpen = false;
    s_isManualSessionOpen = false;
}

void CommitFrontCardSelection() {
    if (!s_flip3dApp || !IsWindow(s_flip3dApp->WindowHandle())) return;
    PostMessageW(s_flip3dApp->WindowHandle(), WM_KEYDOWN, VK_RETURN, 0);
    PostMessageW(s_flip3dApp->WindowHandle(), WM_KEYUP, VK_RETURN, 0);
    s_isWinTabSessionOpen = false;
    s_isManualSessionOpen = false;
}

LRESULT CALLBACK ControllerWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    try {
        switch (msg) {
        case WM_HOTKEY:
            if (wp == kHotkeyWinTab) {
                Wh_Log(L"Win+Tab received by Flip3D controller hotkey");
                OpenOrAdvanceSwitcher(true);
            } else if (wp == kHotkeyShiftTab) {
                Wh_Log(L"Win+Shift+Tab received by Flip3D controller hotkey");
                OpenOrAdvanceSwitcher(true);
            } else if (wp == kHotkeyManual) {
                if (s_flip3dApp && IsWindow(s_flip3dApp->WindowHandle()))
                    PostMessageW(s_flip3dApp->WindowHandle(), WM_CLOSE, 0, 0);
                else
                    OpenOrAdvanceSwitcher(false);
            }
            return 0;
        case kMsgOpenOrAdvance:
            Wh_Log(L"Win+Tab delivered through low-level-hook fallback");
            OpenOrAdvanceSwitcher(wp != 0);
            return 0;
        case kMsgCommitSelection:
            if (s_isWinTabSessionOpen) CommitFrontCardSelection();
            return 0;
        case kMsgStopController:
        case WM_CLOSE:
            s_shutdownRequested = true;
            if (s_flip3dApp && IsWindow(s_flip3dApp->WindowHandle())) {
                PostMessageW(s_flip3dApp->WindowHandle(), WM_CLOSE, 0, 0);
            } else {
                DestroyWindow(hwnd);
            }
            return 0;
        case WM_DESTROY:
            s_controllerWindow = nullptr;
            PostQuitMessage(0);
            return 0;
        }
    } catch (const std::exception&) {
        Wh_Log(L"C++ exception caught at controller window-procedure boundary");
    } catch (...) {
        Wh_Log(L"Unknown exception caught at controller window-procedure boundary");
    }
    if (s_flip3dApp && IsWindow(s_flip3dApp->WindowHandle()))
        PostMessageW(s_flip3dApp->WindowHandle(), WM_CLOSE, 0, 0);
    else if (msg == WM_CLOSE || msg == kMsgStopController)
        DestroyWindow(hwnd);
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static LRESULT Flip3DKeyboardHookProcImpl(int code, WPARAM wp, LPARAM lp) {
    if (code != HC_ACTION || !lp)
        return CallNextHookEx(s_keyboardHook, code, wp, lp);

    const auto* key = reinterpret_cast<const KBDLLHOOKSTRUCT*>(lp);
    if (key->flags & LLKHF_INJECTED)
        return CallNextHookEx(s_keyboardHook, code, wp, lp);

    const bool keyDown = wp == WM_KEYDOWN || wp == WM_SYSKEYDOWN;
    const bool keyUp = wp == WM_KEYUP || wp == WM_SYSKEYUP;
    const bool isWinDownAsync =
        (GetAsyncKeyState(VK_LWIN) & 0x8000) != 0 ||
        (GetAsyncKeyState(VK_RWIN) & 0x8000) != 0;
    const bool altDown = (key->flags & LLKHF_ALTDOWN) != 0 ||
        (GetAsyncKeyState(VK_MENU) & 0x8000) != 0 ||
        (GetAsyncKeyState(VK_LMENU) & 0x8000) != 0 ||
        (GetAsyncKeyState(VK_RMENU) & 0x8000) != 0;
    const bool ctrlDown =
        (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0 ||
        (GetAsyncKeyState(VK_LCONTROL) & 0x8000) != 0 ||
        (GetAsyncKeyState(VK_RCONTROL) & 0x8000) != 0;

    // Track both Windows keys so releasing one while the other is still held
    // doesn't accidentally commit the selection.
    if (key->vkCode == VK_LWIN) {
        if (keyDown) s_isLeftWinKeyHeld = true;
        if (keyUp) {
            s_isLeftWinKeyHeld = false;
            if (!IsWinKeyHeld()) s_isWinMaskKeyInjected = false;
            if (!IsWinKeyHeld() && s_controllerWindow)
                PostMessageW(s_controllerWindow, kMsgCommitSelection, 0, 0);
        }
    } else if (key->vkCode == VK_RWIN) {
        if (keyDown) s_isRightWinKeyHeld = true;
        if (keyUp) {
            s_isRightWinKeyHeld = false;
            if (!IsWinKeyHeld()) s_isWinMaskKeyInjected = false;
            if (!IsWinKeyHeld() && s_controllerWindow)
                PostMessageW(s_controllerWindow, kMsgCommitSelection, 0, 0);
        }
    }

    if (key->vkCode != VK_TAB)
        return CallNextHookEx(s_keyboardHook, code, wp, lp);

    // GetAsyncKeyState alone can miss a fast Win+Tab chord; the hook's own
    // tracking of the Win keys is the second source. If either says "held",
    // the Tab is ours. Otherwise the system opens the modern Task View.
    const bool isWinChordActive = isWinDownAsync || IsWinKeyHeld();

    // Match the upstream mod's Win+Tab policy: if RegisterHotKey succeeded,
    // let its WM_HOTKEY handler own activation; otherwise consume the chord
    // here and post to the controller HWND. Do not use PostThreadMessage here:
    // Flip3D's nested Run() pump would remove and discard hwnd==NULL messages.
    if (keyDown && isWinChordActive && !altDown && !ctrlDown &&
        !s_isWinTabHotkeyRegistered) {
        if (!s_isTabKeyConsumed) {
            s_isTabKeyConsumed = true;
            if (s_controllerWindow)
                PostMessageW(s_controllerWindow, kMsgOpenOrAdvance, 1, 0);
        }
        return 1; // Prevent Task View from also opening.
    }

    if (keyUp && s_isTabKeyConsumed) {
        s_isTabKeyConsumed = false;
        return 1;
    }

    return CallNextHookEx(s_keyboardHook, code, wp, lp);
}

LRESULT CALLBACK Flip3DKeyboardHookProc(int code, WPARAM wp, LPARAM lp) noexcept {
    try {
        return Flip3DKeyboardHookProcImpl(code, wp, lp);
    } catch (...) {
        // Low-level hooks must never unwind into User32.
        return CallNextHookEx(s_keyboardHook, code, wp, lp);
    }
}

// ----------------------------------------------------------------------------
// Dedicated keyboard-hook thread.
// A WH_KEYBOARD_LL hook must answer within LowLevelHooksTimeout (~300 ms). The
// controller thread blocks for longer than that while a session starts
// (Initialize/BuildCards took 400+ ms in the logs), so Windows skipped, or even
// silently removed, the hook; the Tab then reached the system and the modern Task
// View opened. This thread does nothing but pump messages for the hook, and the
// hook only posts to the controller window, so it can never be starved.
// ----------------------------------------------------------------------------
static DWORD WINAPI KeyboardHookThreadProc(void*) noexcept {
    MSG bootstrap{};
    PeekMessageW(&bootstrap, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

    s_keyboardHook = SetWindowsHookExW(WH_KEYBOARD_LL, Flip3DKeyboardHookProc,
                                       GetThisModuleHandle(), 0);
    if (!s_keyboardHook)
        Wh_Log(L"Keyboard hook unavailable (%u)", GetLastError());
    if (s_hookReadyEvent) SetEvent(s_hookReadyEvent);
    if (!s_keyboardHook) return 1;

    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    UnhookWindowsHookEx(s_keyboardHook);
    s_keyboardHook = nullptr;
    return 0;
}

static void StartKeyboardHookThread() {
    if (s_hookThreadHandle) return;
    s_hookReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    s_hookThreadHandle = CreateThread(nullptr, 0, KeyboardHookThreadProc, nullptr, 0, &s_hookThreadId);
    if (s_hookThreadHandle && s_hookReadyEvent)
        WaitForSingleObject(s_hookReadyEvent, 3000);
    if (s_hookReadyEvent) { CloseHandle(s_hookReadyEvent); s_hookReadyEvent = nullptr; }
}

static void StopKeyboardHookThread() {
    if (!s_hookThreadHandle) return;
    PostThreadMessageW(s_hookThreadId, WM_QUIT, 0, 0);
    WaitForSingleObject(s_hookThreadHandle, 3000);
    CloseHandle(s_hookThreadHandle);
    s_hookThreadHandle = nullptr;
    s_hookThreadId = 0;
    s_keyboardHook = nullptr;
}

static DWORD ControllerThreadProcImpl(void*) {
    MSG bootstrap{};
    PeekMessageW(&bootstrap, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    s_controllerThreadId = GetCurrentThreadId();

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = ControllerWndProc;
    wc.hInstance = GetThisModuleHandle();
    wc.lpszClassName = kControllerClass;
    RegisterClassExW(&wc);
    s_controllerWindow = CreateWindowExW(0, kControllerClass, L"", 0,
        0, 0, 0, 0, HWND_MESSAGE, nullptr, wc.hInstance, nullptr);

    if (s_controllerWindow) {
        s_isWinTabHotkeyRegistered = RegisterHotKey(s_controllerWindow, kHotkeyWinTab,
                                               MOD_WIN | MOD_NOREPEAT, VK_TAB) != FALSE;
        s_isManualHotkeyRegistered = RegisterHotKey(s_controllerWindow, kHotkeyManual,
                                                   MOD_CONTROL | MOD_ALT | MOD_NOREPEAT,
                                                   VK_F12) != FALSE;
        // Second shortcut: Win+Shift+Tab (opens, or steps backwards when open).
        s_isShiftTabHotkeyRegistered = RegisterHotKey(s_controllerWindow, kHotkeyShiftTab,
                                               MOD_WIN | MOD_SHIFT | MOD_NOREPEAT, VK_TAB) != FALSE;
        Wh_Log(L"Hotkeys: Win+Tab=%d Ctrl+Alt+F12=%d Win+Shift+Tab=%d",
               (int)s_isWinTabHotkeyRegistered, (int)s_isManualHotkeyRegistered,
               (int)s_isShiftTabHotkeyRegistered);
    }
    StartKeyboardHookThread();

    const bool ok = s_controllerWindow && (s_keyboardHook || s_isWinTabHotkeyRegistered || s_isShiftTabHotkeyRegistered);
    s_initializationSucceeded.store(ok, std::memory_order_release);
    s_controllerReady.store(true, std::memory_order_release);
    if (s_controllerReadyEvent) SetEvent(s_controllerReadyEvent);

    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (s_isWinTabHotkeyRegistered) UnregisterHotKey(s_controllerWindow, kHotkeyWinTab);
    if (s_isManualHotkeyRegistered) UnregisterHotKey(s_controllerWindow, kHotkeyManual);
    if (s_isShiftTabHotkeyRegistered) UnregisterHotKey(s_controllerWindow, kHotkeyShiftTab);
    StopKeyboardHookThread();
    if (s_controllerWindow && IsWindow(s_controllerWindow)) DestroyWindow(s_controllerWindow);
    UnregisterClassW(kControllerClass, GetThisModuleHandle());
    s_controllerThreadId = 0;
    return 0;
}

DWORD WINAPI ControllerThreadProc(void* context) noexcept {
    try {
        return ControllerThreadProcImpl(context);
    } catch (const std::exception&) {
        Wh_Log(L"C++ exception caught in controller thread");
    } catch (...) {
        Wh_Log(L"Unknown exception caught in controller thread");
    }

    // Failure-path cleanup is idempotent and stays on the hook-owning thread.
    if (s_controllerWindow) {
        if (s_isWinTabHotkeyRegistered)
            UnregisterHotKey(s_controllerWindow, kHotkeyWinTab);
        if (s_isManualHotkeyRegistered)
            UnregisterHotKey(s_controllerWindow, kHotkeyManual);
    }
    StopKeyboardHookThread();
    if (s_controllerWindow && IsWindow(s_controllerWindow)) {
        DestroyWindow(s_controllerWindow);
        s_controllerWindow = nullptr;
    }
    UnregisterClassW(kControllerClass, GetThisModuleHandle());
    s_initializationSucceeded.store(false, std::memory_order_release);
    s_controllerReady.store(true, std::memory_order_release);
    if (s_controllerReadyEvent) SetEvent(s_controllerReadyEvent);
    s_controllerThreadId = 0;
    return 1;
}
} // unnamed namespace: internal controller state and callbacks

// Checks once, before any hotkey or hook exists, that the system entry points
// the renderer needs are present. If they are not, the mod does not start and
// Win+Tab keeps opening the normal Task View.
static bool ProbeRequiredSystemApis() {
    HMODULE dwmapi = LoadLibraryW(L"dwmapi.dll");
    if (!dwmapi) {
        Wh_Log(L"[init] dwmapi.dll could not be loaded; the mod will not start");
        return false;
    }
    const bool hasCreateVisual = GetProcAddress(dwmapi, MAKEINTRESOURCEA(147)) != nullptr;
    const bool hasQuerySize = GetProcAddress(dwmapi, MAKEINTRESOURCEA(162)) != nullptr;
    FreeLibrary(dwmapi);
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    const bool hasMinimizeRect =
        user32 && GetProcAddress(user32, "GetWindowMinimizeRect") != nullptr;
    if (!hasCreateVisual || !hasQuerySize || !hasMinimizeRect) {
        Wh_Log(L"[init] Required system entry points are missing on this Windows build "
               L"(dwmapi#147=%d dwmapi#162=%d GetWindowMinimizeRect=%d); the mod will not start",
               (int)hasCreateVisual, (int)hasQuerySize, (int)hasMinimizeRect);
        return false;
    }
    return true;
}

BOOL WhTool_ModInit() {
    if (!ProbeRequiredSystemApis()) return FALSE;
    s_controllerReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!s_controllerReadyEvent) return FALSE;
    DWORD tid = 0;
    s_controllerThreadHandle = CreateThread(nullptr, 0, ControllerThreadProc, nullptr, 0, &tid);
    if (!s_controllerThreadHandle) {
        CloseHandle(s_controllerReadyEvent); s_controllerReadyEvent = nullptr; return FALSE;
    }
    s_controllerThreadId = tid;
    WaitForSingleObject(s_controllerReadyEvent, INFINITE);
    CloseHandle(s_controllerReadyEvent); s_controllerReadyEvent = nullptr;
    if (!s_initializationSucceeded.load(std::memory_order_acquire)) {
        PostThreadMessageW(s_controllerThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(s_controllerThreadHandle, INFINITE);
        CloseHandle(s_controllerThreadHandle); s_controllerThreadHandle = nullptr;
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModUninit() {
    if (s_controllerWindow && IsWindow(s_controllerWindow))
        PostMessageW(s_controllerWindow, kMsgStopController, 0, 0);
    else if (s_controllerThreadId)
        PostThreadMessageW(s_controllerThreadId, WM_QUIT, 0, 0);
    if (s_controllerThreadHandle) {
        WaitForSingleObject(s_controllerThreadHandle, INFINITE);
        CloseHandle(s_controllerThreadHandle); s_controllerThreadHandle = nullptr;
    }
    s_controllerThreadId = 0;
}

void WhTool_ModSettingsChanged() {
    // The mod has no settings.
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process

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
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
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
