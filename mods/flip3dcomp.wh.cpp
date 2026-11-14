// ==WindhawkMod==
// @id              flip3dcomp
// @name            Flip3DComp
// @description     a 1:1 replica of the Windows Aero Flip3D window switcher based on DirectComposition
// @version         2.0
// @author          Tails (milestprower92)
// @github          https://github.com/milestprower92
// @include         windhawk.exe
// @include         explorer.exe
// @include         rundll32.exe
// @compilerOptions -lcomdlg32 -lwindowscodecs -lole32 -loleaut32 -loleacc -luuid -ladvapi32 -ldxgi -ld3d11 -ldcomp -ldwmapi -lgdi32 -luser32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Flip3DComp

Original version by: [ALTaleX531](https://github.com/ALTaleX531)

A 1:1 replica of the Flip3D Window switcher introduced on Windows Vista, for newer versions of Windows, ported from my fork [Flip3DComp](https://github.com/milestprower92/Flip3DComp)

## Demo

![Demo](https://raw.githubusercontent.com/milestprower92/Flip3DComp/refs/heads/main/media/Flip3DComp_demo.gif)

Live window previews follow the original Flip3D carousel layout and timing.

## Requirements

- Windows 10 versions 1703 and higher
- Windhawk 1.7.3 or higher

## ⚠️ Notes

- The optional Task View override currently uses the Windows 11 Task View activation path on AMD64. It is unavailable on x86, ARM64 & Windows 10.
- **Do not enable more than one Win+Tab replacement. Disable Any mods that override Win+Tab, or leave Win+Tab disabled in Alt+Tab Flip 3D, while using Flip3DComp.**

## Controls

- `Win+Tab`: Open Flip3D
- `Ctrl + Win + Tab`: Open Flip3D in Sticky Mode
- `Tab` / `Shift + Tab`, arrow keys, mouse wheel: scroll the carousel
- `Enter` or left click a card: activate the selected window
- `Home`: return to the original front window
- `Esc`: exit
- `F5`: replay the enter animation

*/
// ==/WindhawkModReadme==
// ==WindhawkModSettings==
/*
- CanHasEasing: false
  $name: Enable animation easing
  $description: Use eased animations on flip3d.
- TaskViewOverride: false
  $name: Replace Task View (BETA)
  $description: Redirect Task View to Flip3D. AMD64 & Windows 11 only.
*/
// ==/WindhawkModSettings==

// Dependencies
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif
#if !defined(_WIN32_WINNT) || _WIN32_WINNT < 0x0A00
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif
#if !defined(WINVER) || WINVER < 0x0A00
#undef WINVER
#define WINVER 0x0A00
#endif

#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <windows.h>
#include <shellapi.h>
#include <tlhelp32.h>
#include <dwmapi.h>
#include <dcomp.h>
#include <wrl/client.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <oleacc.h>
#include <oaidl.h>
#include <wincodec.h>


struct IDCompositionVisual3Compat : IDCompositionVisualDebug
{
    virtual HRESULT STDMETHODCALLTYPE SetDepthMode(
        DCOMPOSITION_DEPTH_MODE mode) = 0;
#if defined(_MSC_VER) && defined(__cplusplus)
    virtual HRESULT STDMETHODCALLTYPE SetOffsetZ(float offsetZ) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetOffsetZ(IDCompositionAnimation* animation) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetOpacity(float opacity) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetOpacity(IDCompositionAnimation* animation) = 0;
    using IDCompositionVisualDebug::SetTransform;
    virtual HRESULT STDMETHODCALLTYPE SetTransform(
        const D2D_MATRIX_4X4_F& matrix) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetTransform(
        IDCompositionTransform3D* transform) = 0;
#else
    virtual HRESULT STDMETHODCALLTYPE SetOffsetZ(IDCompositionAnimation* animation) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetOffsetZ(float offsetZ) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetOpacity(IDCompositionAnimation* animation) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetOpacity(float opacity) = 0;
    using IDCompositionVisualDebug::SetTransform;
    virtual HRESULT STDMETHODCALLTYPE SetTransform(
        IDCompositionTransform3D* transform) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetTransform(
        const D2D_MATRIX_4X4_F& matrix) = 0;
#endif
    virtual HRESULT STDMETHODCALLTYPE SetVisible(BOOL visible) = 0;
};

#ifdef __CRT_UUID_DECL
__CRT_UUID_DECL(IDCompositionVisual, 0x4d93059d, 0x097b, 0x4651,
    0x9a, 0x60, 0xf0, 0xf2, 0x51, 0x16, 0xe2, 0xf3);
__CRT_UUID_DECL(IDCompositionVisual3Compat, 0x2775f462, 0xb6c1, 0x4015,
    0xb0, 0xbe, 0xb3, 0xe7, 0xd6, 0xa4, 0x97, 0x6d);
#endif

inline constexpr IID kIidIDCompositionVisual3Compat = {
    0x2775F462, 0xB6C1, 0x4015,{ 0xB0, 0xBE, 0xB3, 0xE7, 0xD6, 0xA4, 0x97, 0x6D }
};

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cwchar>
#include <memory>
#include <string>
#include <vector>

// Data Types
struct Vec3
{
    float x, y, z;
};

struct Vec2
{
    float x, y;
};

using Microsoft::WRL::ComPtr;
using Matrix4x4 = D2D_MATRIX_4X4_F;

// uDWM animation / camera constants
constexpr float  kEnterExitDurationSec = 0.25f;    // uDWM binary: 0.25s
constexpr float  kExitDurationSec = 0.25f;    // uDWM: same timeline for exit + exit-rotate
inline std::atomic_bool kCanHasEasing{ false };
inline std::atomic_bool g_navigationKeysHeld[256]{};

// Timeline easing: CSS cubic-bezier(0, 0, 0, 1) — fast start, soft landing
constexpr float  kTimelineBezierX1 = 0.0f;
constexpr float  kTimelineBezierY1 = 0.0f;
constexpr float  kTimelineBezierX2 = 0.0f;
constexpr float  kTimelineBezierY2 = 1.0f;

inline std::atomic<float> kScrollSmoothTimeSec{ 0.15f };
inline std::atomic<float> kAnimationRate{ 1.0f };
inline std::atomic<float> kShiftAnimationRate{ 0.1f };
constexpr float  kScrollSettleEpsilon = 0.002f;   // scrollPos≈scrollTarget threshold (browse
constexpr float  kFrontCardExitSlot = -0.5f;    // front card slot when exiting (fade-out)
constexpr float  kFrontCardFadeSpeed = 0.0f;     // front card fade speed (0 = instant, 1 = linear)
constexpr float  kScrollWheelNotchFraction = 1.0f;     // carousel slots per mouse wheel notch
constexpr float  kWheelActiveTimeoutSec = 0.15f;    // throttle repeated wheel processing (uDWM CFlip3D::OnWheel)
constexpr float  kKeyRepeatIntervalSec = 0.135f;   // throttle repeated keydown processing (uDWM CFlip3D::OnKey)
constexpr float  kHeldKeyStartDelaySec = 0.50f;    // delay continuous rotation so a key tap moves one card
constexpr float  kHeldKeyRotateSpeed = 6.0f;     // carousel slots per second while a navigation key is held
constexpr float  kTouchSwipeThresholdPx = 8.0f;     // minimum travel before a touch becomes a swipe
constexpr float  kTouchSwipeVelocityLimit = 12.0f;    // soft fling speed limit in carousel slots per second
constexpr float  kTouchSwipeFriction = 3.5f;     // velocity decay per second after a touch swipe
constexpr float  kTouchFlingMinVelocity = 1.5f;   // slots/s needed on release to start coasting
constexpr float  kTouchFlingStopVelocity = 0.1f;  // slots/s below which coasting ends and snaps to a card
constexpr DWORD  kTouchFlingIdleMs = 100;         // finger held still this long before release cancels the fling
constexpr float  kTouchScrollMinSmoothTimeSec = 0.025f; // minimum easing time while following a touch swipe
constexpr float  kTouchScrollSpeedResponse = 0.25f;    // faster swipes reduce touch-follow easing time
constexpr UINT   kPenHoverScrollZonePx = 120;      // screen-corner zone size for pen hover scrolling
constexpr float  kPenHoverScrollSpeed = 6.0f;     // carousel slots per second while pen hovers in a corner
constexpr float  kPenHoveredCardScale = 1.04f;    // slight enlargement for the pen-hovered card
constexpr float  kPenHoverScaleSmoothTimeSec = 0.08f;    // hover-scale transition time when easing is enabled
constexpr float  kOpeningTabDelaySec = 0.75f;
constexpr float  kRotateListDurationSec = 0.175f;   // uDWM g_secRotateListDuration
constexpr float  kNearPlaneEdgeSize = 1.15f;    // near plane half-extent
constexpr float  kNearPlaneDistance = 1.0f;     // near plane Z distance
constexpr int    kMaxVisibleCards = 10;       // max simultaneously visible cards
constexpr int    kMaxCards = 24;       // absolute max cards in carousel

enum class CardThumbnailQuality
{
    Low, MediumLow, Medium, MediumHigh, High,
};

constexpr CardThumbnailQuality kCardThumbnailQuality = CardThumbnailQuality::Medium; // This controls the DWM thumbnail resolution used by 3D window cards. By default Windows Vista & 7 have it set to a Medium value (0.50f)

constexpr float  kBackCardOpacityScale = 1.0f;     // scale opacity for cards near the back edge (slot ~ span-2..span)
constexpr float  kBackCardMinOpacity = 0.60f;    // minimum opacity for cards entering from the back (wrap fade)
constexpr float  kBackCardFadeTimeSec = 0.015f;    // fade-in time for cards entering from the back (wrap fade)
constexpr bool   kEnableBackCardOpacityEasing = true;

// Pre-computed camera poses (radians)
constexpr float kCameraFinalTranslateX = -1.1f;
constexpr float kCameraFinalTranslateY = 0.35f;   // uDWM InitializeModelAndCamera
constexpr float kCameraFinalTranslateZ = 0.35f;
constexpr float kCameraFinalRotateX = 0.08726646f;   //  5.0 deg pitch
constexpr float kCameraFinalRotateY = 0.43633232f;   // 25.0 deg yaw
constexpr float kCameraFinalRotateZ = 0.061086524f;  //  3.5 deg roll

// Bezier control points for carousel path
constexpr float kBezierControls[3][3] = {
    { -2.0f,   0.5f,   -2.25f },
    { -1.8f,   0.55f,  -1.25f },
    { -1.45f, -0.1f,   -0.3f },
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
constexpr float kFinalMinRectLeftPercentage = 0.3f;

// Enums

// Interpolation mode for timeline-driven animations
enum class InterpolationMode
{
    Linear = 0,
    Cubic = 1,  // uDWM CTimeline: smoothstep t²(3-2t)
    CubicBezier = 2,  // CSS cubic-bezier (Config.h kTimelineBezier*)
};

enum class CarouselWrapPhase
{
    None = 0,
    ExitingFront = 1,  // slot → [-1, 0]   front fade-out (forward scroll)
    EnteringBack = 2,  // opacity slot span→listSlot; position stays on list slot
    EnteringFront = 3,  // slot → [-1, 0]   front fade-in (backward scroll)
};

// Flip3D view state machine (uDWM CFlip3D::Flip3DState ordering preserved)
enum class ViewState
{
    Inactive = 0,  // not visible
    Enter = 1,  // animating in
    Exit = 2,  // animating out (stationary carousel)
    Interactive = 3,  // fully visible, accepting input
    ExitRepeatedRotate = 4,  // exit flatten + discrete list rotation in parallel
};

enum class SwitcherInputAction
{
    None,
    Open,
    OpenSticky,
    Next,
    Previous,
    CommitSelection,
    ActivateClickedCard,
    Cancel,
};

class SwitcherInputController
{
public:
    SwitcherInputAction HandleKey(UINT vkCode, bool keyDown)
    {
        if (vkCode == VK_LWIN || vkCode == VK_RWIN)
        {
            SetWindowsKeyDown(vkCode, keyDown);
            if (!keyDown && m_active && !m_sticky && !IsWindowsKeyDown())
            {
                ResetSession();
                return SwitcherInputAction::CommitSelection;
            }
            return SwitcherInputAction::None;
        }

        if (vkCode == VK_CONTROL || vkCode == VK_LCONTROL || vkCode == VK_RCONTROL)
        {
            SetControlKeyDown(vkCode, keyDown);
            return SwitcherInputAction::None;
        }

        if (vkCode == VK_SHIFT || vkCode == VK_LSHIFT || vkCode == VK_RSHIFT)
        {
            SetShiftKeyDown(vkCode, keyDown);
            return SwitcherInputAction::None;
        }

        if (!keyDown)
            return SwitcherInputAction::None;

        if (vkCode == VK_ESCAPE && m_active)
        {
            ResetSession();
            return SwitcherInputAction::Cancel;
        }

        if (vkCode == VK_RETURN && m_active)
        {
            ResetSession();
            return SwitcherInputAction::CommitSelection;
        }

        if (vkCode == VK_TAB)
        {
            if (!m_active && IsWindowsKeyDown())
            {
                m_active = true;
                m_sticky = IsControlKeyDown();
                return m_sticky ? SwitcherInputAction::OpenSticky
                    : SwitcherInputAction::Open;
            }

            if (CanNavigate())
            {
                if (IsControlKeyDown() && IsWindowsKeyDown())
                    m_sticky = true;
                return IsShiftKeyDown() ? SwitcherInputAction::Previous
                    : SwitcherInputAction::Next;
            }
        }

        if (CanNavigate())
        {
            switch (vkCode)
            {
            case VK_LEFT:
            case VK_UP:
                return SwitcherInputAction::Previous;
            case VK_RIGHT:
            case VK_DOWN:
                return SwitcherInputAction::Next;
            }
        }

        return SwitcherInputAction::None;
    }

    SwitcherInputAction HandleClick(bool hitCard)
    {
        if (!m_active)
            return SwitcherInputAction::None;

        ResetSession();
        return hitCard ? SwitcherInputAction::ActivateClickedCard
            : SwitcherInputAction::Cancel;
    }

    void Reset()
    {
        ResetSession();
        m_leftWindowsDown = false;
        m_rightWindowsDown = false;
        m_genericControlDown = false;
        m_leftControlDown = false;
        m_rightControlDown = false;
        m_genericShiftDown = false;
        m_leftShiftDown = false;
        m_rightShiftDown = false;
    }

    bool IsActive() const { return m_active; }
    bool IsSticky() const { return m_active && m_sticky; }
    bool IsWindowsKeyDown() const { return m_leftWindowsDown || m_rightWindowsDown; }

private:
    bool CanNavigate() const
    {
        return m_active && (m_sticky || IsWindowsKeyDown());
    }

    bool IsControlKeyDown() const
    {
        return m_genericControlDown || m_leftControlDown || m_rightControlDown;
    }

    bool IsShiftKeyDown() const
    {
        return m_genericShiftDown || m_leftShiftDown || m_rightShiftDown;
    }

    void SetWindowsKeyDown(UINT vkCode, bool keyDown)
    {
        if (vkCode == VK_LWIN)
            m_leftWindowsDown = keyDown;
        else
            m_rightWindowsDown = keyDown;
    }

    void SetControlKeyDown(UINT vkCode, bool keyDown)
    {
        if (vkCode == VK_CONTROL)
            m_genericControlDown = keyDown;
        else if (vkCode == VK_LCONTROL)
            m_leftControlDown = keyDown;
        else
            m_rightControlDown = keyDown;
    }

    void SetShiftKeyDown(UINT vkCode, bool keyDown)
    {
        if (vkCode == VK_SHIFT)
            m_genericShiftDown = keyDown;
        else if (vkCode == VK_LSHIFT)
            m_leftShiftDown = keyDown;
        else
            m_rightShiftDown = keyDown;
    }

    void ResetSession()
    {
        m_active = false;
        m_sticky = false;
    }

    bool m_active = false;
    bool m_sticky = false;
    bool m_leftWindowsDown = false;
    bool m_rightWindowsDown = false;
    bool m_genericControlDown = false;
    bool m_leftControlDown = false;
    bool m_rightControlDown = false;
    bool m_genericShiftDown = false;
    bool m_leftShiftDown = false;
    bool m_rightShiftDown = false;
};

// Private DWM thumbnail API (dwmapi.dll ordinal exports)
constexpr DWORD DWM_TNP_DISABLEFORCECVI = 0x80000000;
constexpr DWORD DWM_TNP_ENABLE3D = 0x04000000;

// DwmpCreateSharedThumbnailVisual dwThumbnailFlags (see dwthumbnailflags-reference.md)
constexpr DWORD DWM_TNF_DWMWINDOW = 0x2;   // fDwmWindow → PostMessage(0x327) on size change

// Posted to hwndDestination when a DWM_TNF_DWMWINDOW thumbnail source resizes.
constexpr UINT WM_DWMTHUMBNAILSOURCESIZECHANGED = 0x327;

using DwmpCreateSharedThumbnailVisual_fn = HRESULT(WINAPI*)(
    HWND hwndDestination, HWND hwndSource, DWORD dwThumbnailFlags,
    DWM_THUMBNAIL_PROPERTIES* pThumbnailProperties,
    void* pDCompDevice, void** ppVisual, HTHUMBNAIL* phThumbnailId);

using DwmpQueryWindowThumbnailSourceSize_fn = HRESULT(WINAPI*)(
    HWND hwndSource, BOOL fSourceClientAreaOnly, SIZE* pSize);

using GetWindowMinimizeRect_fn = BOOL(WINAPI*)(HWND, LPRECT);

using DwmpUpdateDesktopThumbnail_fn = HRESULT(WINAPI*)(
    HWND hwnd, LPCRECT rcDest, LPCRECT rcSrc, BYTE opacity, DWORD dwFlags);

// CardModel — Per-window data for the Flip3D carousel
struct CardModel
{
    HWND                m_hwnd = nullptr;   // source window handle
    int                 m_initialCarouselIndex = 0;  // order at Flip3D enter (before rotates)
    HTHUMBNAIL          m_hThumb = nullptr;   // DWM shared thumbnail handle

    // ---- DirectComposition visuals ----
    ComPtr<IDCompositionVisual2> m_visual;          // thumbnail content visual (DWM)
    ComPtr<IDCompositionVisual3Compat> m_containerVisual; // baked Model×Camera + opacity

    // ---- Geometry (monitor-world units) ----
    Vec2                m_targetSize = {};        // 3D carousel size (thumb px + NormalizeWindowSize)
    Vec3                m_originalPos = {};        // desktop top-left (from window rect)
    Vec3                m_flatPos = {};        // same anchor for 2D enter layout
    Vec2                m_flatSize = {};        // 2D size (thumb px, or tile/rcWork)
    float               m_scrLeft = 0.0f;      // raw screen pixel: left
    float               m_scrTop = 0.0f;      // raw screen pixel: top
    float               m_scrW = 0.0f;      // raw screen pixel: width
    float               m_scrH = 0.0f;      // raw screen pixel: height

    // ---- Source content ----
    float               m_aspectRatio = 16.0f / 10.0f;
    float               m_occupancy = 0.7f;       // normalized occupancy factor
    int                 m_nativeSrcWidth = 400;     // queried DWM source width (pixels)
    int                 m_nativeSrcHeight = 300;     // queried DWM source height (pixels)
    int                 m_srcWidth = 400;        // thumbnail source width (pixels)
    int                 m_srcHeight = 300;        // thumbnail source height (pixels)
    bool                m_isMinimized = false;      // window is iconic
    bool                m_isShellDesktop = false;    // Progman / GetShellWindow()

    // ---- Carousel display slot ----
    float               m_displaySlot = 0.0f;
    bool                m_displaySlotValid = false;
    float               m_penHoverScale = 1.0f;
    CarouselWrapPhase   m_wrapPhase = CarouselWrapPhase::None;
    float               m_wrapFadeStartListSlot = 0.0f; // list slot when EnteringBack began
    
    // fade-in when a card wraps into view.
    float               m_wrapProgress = 0.0f;
    // Actual opacity used while a card is entering through a wrap.
    float               m_wrapOpacity = 1.0f;
    bool                m_wrapFadeActive = false;
    bool                m_opacityInitialized = false;
    float               m_lastTargetOpacity = 0.0f;
};


// 3D math utilities (row-vector DirectComposition convention)
namespace Math
{

    // Identity/basic transforms

    inline Matrix4x4 Identity()
    {
        Matrix4x4 matrix = {};
        matrix.m[0][0] = 1.0f;
        matrix.m[1][1] = 1.0f;
        matrix.m[2][2] = 1.0f;
        matrix.m[3][3] = 1.0f;
        return matrix;
    }

    inline Matrix4x4 Translation(float x, float y, float z)
    {
        return { { { 1,0,0,0, 0,1,0,0, 0,0,1,0, x,y,z,1 } } };
    }

    inline Matrix4x4 Scale(float sx, float sy, float sz)
    {
        return { { { sx,0,0,0, 0,sy,0,0, 0,0,sz,0, 0,0,0,1 } } };
    }

    // Composition

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

    // Rotation 

    inline Matrix4x4 RotationYPR(float yaw, float pitch, float roll)
    {
        float cy = cos(yaw), sy = sin(yaw);
        float cp = cos(pitch), sp = sin(pitch);
        float cr = cos(roll), sr = sin(roll);
        return { { {
            cr * cy + sr * sp * sy, sr * cp, -cr * sy + sr * sp * cy, 0,
           -sr * cy + cr * sp * sy, cr * cp,  sr * sy + cr * sp * cy, 0,
            cp * sy,            -sp,    cp * cy,             0,
            0, 0, 0, 1
        } } };
    }

    inline Matrix4x4 RotationRollPitchYaw(float pitch, float yaw, float roll)
    {
        float cp = cos(pitch), sp = sin(pitch);
        float cy = cos(yaw), sy = sin(yaw);
        float cr = cos(roll), sr = sin(roll);

        Matrix4x4 Ry = { { { cy, 0, -sy, 0,  0, 1, 0, 0,  sy, 0, cy, 0,  0,0,0,1 } } };
        Matrix4x4 Rx = { { { 1, 0, 0, 0,  0, cp, sp, 0,  0, -sp, cp, 0,  0,0,0,1 } } };
        Matrix4x4 Rz = { { { cr, sr, 0, 0,  -sr, cr, 0, 0,  0, 0, 1, 0,  0,0,0,1 } } };

        Matrix4x4 RzRx = Multiply(Rz, Rx);
        return Multiply(RzRx, Ry);
    }

    // Camera matrices

    inline Matrix4x4 LookAtRH(Vec3 eye, Vec3 at, Vec3 up)
    {
        Vec3 z = { eye.x - at.x, eye.y - at.y, eye.z - at.z };
        float zl = 1.0f / sqrt(z.x * z.x + z.y * z.y + z.z * z.z);
        z.x *= zl; z.y *= zl; z.z *= zl;

        Vec3 x = { up.y * z.z - up.z * z.y, up.z * z.x - up.x * z.z, up.x * z.y - up.y * z.x };
        float xl = 1.0f / sqrt(x.x * x.x + x.y * x.y + x.z * x.z);
        x.x *= xl; x.y *= xl; x.z *= xl;

        Vec3 y = { z.y * x.z - z.z * x.y, z.z * x.x - z.x * x.z, z.x * x.y - z.y * x.x };

        return { { {
            x.x, y.x, z.x, 0,
            x.y, y.y, z.y, 0,
            x.z, y.z, z.z, 0,
            -(x.x * eye.x + x.y * eye.y + x.z * eye.z),
            -(y.x * eye.x + y.y * eye.y + y.z * eye.z),
            -(z.x * eye.x + z.y * eye.y + z.z * eye.z),
            1
        } } };
    }

    inline Matrix4x4 PerspectiveRH(float nearExtent, float aspect, float nearZ, float farZ)
    {
        float w = nearExtent;
        float h = nearExtent / aspect;
        float q = farZ / (nearZ - farZ);
        return { { {
            2.0f * nearZ / w, 0,              0,  0,
            0,                2.0f * nearZ / h, 0,  0,
            0, 0, q, -1,
            0, 0, q * nearZ, 0
        } } };
    }

    // Bezier & interpolation

    inline Vec3 Bezier(float t)
    {
        float s = 1.0f - t;
        return {
            s * s * kBezierControls[0][0] + 2 * s * t * kBezierControls[1][0] + t * t * kBezierControls[2][0],
            s * s * kBezierControls[0][1] + 2 * s * t * kBezierControls[1][1] + t * t * kBezierControls[2][1],
            s * s * kBezierControls[0][2] + 2 * s * t * kBezierControls[1][2] + t * t * kBezierControls[2][2],
        };
    }

    inline float NormalizationScale(float o)
    {
        o = std::clamp(o, 0.0f, 1.0f);
        float inv = 1.0f - o;
        return inv * inv * kNormalizationBezier[0]
            + 2.0f * kNormalizationBezier[1] * o * inv
            + o * o * kNormalizationBezier[2];
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
        const float finalTop = (float)minimizeRect.top - finalH;

        RECT result = {};
        result.left = (LONG)std::lround(finalLeft);
        result.top = (LONG)std::lround(finalTop);
        result.right = (LONG)std::lround(finalLeft + finalW);
        result.bottom = (LONG)std::lround(finalTop + finalH);
        return result;
    }

    inline float Lerp(float a, float b, float t)
    {
        return a + (b - a) * t;
    }

    // CSS cubic-bezier(x1,y1,x2,y2): map linear time x in [0,1] to eased progress.
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

    // Ray-triangle intersection (Möller–Trumbore)

    // Returns distance along ray (t > 0) or -1.0f on miss.
    inline float IntersectRayTriangle(
        Vec3 rayOrig, Vec3 rayDir,
        Vec3 v0, Vec3 v1, Vec3 v2)
    {
        Vec3 e1 = { v1.x - v0.x, v1.y - v0.y, v1.z - v0.z };
        Vec3 e2 = { v2.x - v0.x, v2.y - v0.y, v2.z - v0.z };
        Vec3 h = {
            rayDir.y * e2.z - rayDir.z * e2.y,
            rayDir.z * e2.x - rayDir.x * e2.z,
            rayDir.x * e2.y - rayDir.y * e2.x,
        };

        float a = e1.x * h.x + e1.y * h.y + e1.z * h.z;
        if (a > -1e-6f && a < 1e-6f) return -1.0f;

        float f = 1.0f / a;
        Vec3 s = { rayOrig.x - v0.x, rayOrig.y - v0.y, rayOrig.z - v0.z };
        float u = f * (s.x * h.x + s.y * h.y + s.z * h.z);
        if (u < 0.0f || u > 1.0f) return -1.0f;

        Vec3 q = {
            s.y * e1.z - s.z * e1.y,
            s.z * e1.x - s.x * e1.z,
            s.x * e1.y - s.y * e1.x,
        };
        float v = f * (rayDir.x * q.x + rayDir.y * q.y + rayDir.z * q.z);
        if (v < 0.0f || u + v > 1.0f) return -1.0f;

        float t = f * (e2.x * q.x + e2.y * q.y + e2.z * q.z);
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
        return { { {
            width * 0.5f, 0,              0, 0,
            0,            -height * 0.5f, 0, 0,
            0,             0,             1, 0,
            originX + width * 0.5f, originY + height * 0.5f, 0, 1,
        } } };
    }

}


// Accessibility interface (uDWM CFlip3DAccessible port)
class Flip3DCompApp;

class Flip3DAccessible final : public IAccessible
{
public:
    static HRESULT Create(Flip3DCompApp* app, IAccessible** ppAccessible);
    void Disconnect();
    STDMETHODIMP QueryInterface(REFIID riid, void** ppvObj) override;
    STDMETHODIMP_(ULONG) AddRef() override;
    STDMETHODIMP_(ULONG) Release() override;
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

    ULONG           m_ref = 1;
    Flip3DCompApp* m_app = nullptr;
};

// Animation timeline with interpolation
struct Timeline
{
    // Configuration 
    float             m_durationSec = 0.0f;
    float             m_elapsed = 0.0f;
    float             m_startValue = 0.0f;
    float             m_endValue = 0.0f;
    InterpolationMode m_mode = InterpolationMode::CubicBezier;
    bool              m_active = false;

    // Construction 
    Timeline() = default;

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

    // Linear start→end lerp (RawProgress only, ignores m_mode).
    float LinearValue() const;

    // True if the timeline has completed.
    bool IsActive() const { return m_active; }
};


// ============================================================================
// DirectComposition 3D window switcher application
// ============================================================================
//
//   Visual tree layout:
//     Root (DesktopWindowTarget)
//       └── SceneVisual  ← 3D carousel
//       └── WashVisual (dark overlay)
//       └── Per-monitor shell thumbnails + wash (virtual desktop layout)
//
//   Thumbnails: DwmpCreateSharedThumbnailVisual (dwmapi.dll ord 147)
//   3D carousel: parent Visual carries camera matrix; children carry model matrices.
//
// ============================================================================
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
inline constexpr UINT kSwitcherActionMessage = WM_APP + 0x31;

inline std::vector<HWND> g_mruWindows;
inline HWINEVENTHOOK g_mruHook = nullptr;

// Flip3DCompApp — Main application class
class Flip3DCompApp
{
    friend class Flip3DAccessible;

public:
    // Public interface
    bool Initialize(HINSTANCE hInstance, bool openingTabHeld = false);
    HWND WindowHandle() const { return m_hwnd; }
    const wchar_t* InitErrorMessage() const { return m_initError.c_str(); }
    int  Run();
    void Shutdown();

private:
    // Window procedure forwarding
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);
    void HandleInputAction(SwitcherInputAction action, HWND clickedHwnd = nullptr);

    // Lifecycle
    bool CreateAppWindow();
    void    ApplyFullscreenLayout();
    void    EnsureTopmost();

    // DWM Thumbnail API (dwmapi.dll ordinals 147, 162)
    bool    LoadThumbApi();
    void    UnloadThumbApi();

    
    // DirectComposition composition
        HRESULT InitComposition();
    HRESULT CreateShellBackdrop();
    void    DestroyMonitorBackdrops();
    void    UpdateBackdropLayout();
    bool    RebuildMonitorBackdropsIfNeeded();

    
    // Window enumeration
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

    // Card building
    bool    BuildCards();
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
    // Rotation (uDWM m_leWindows linked-list model)
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
    bool    OnPenPointer(UINT msg, WPARAM wParam);
    void    SetPenHoverScrollDirection(int direction);
    void    OnTouchContact(UINT id, DWORD flags, POINT point, DWORD eventTime);
    bool    OnTouch(WPARAM wParam, LPARAM lParam);

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
    HINSTANCE               m_hInstance = nullptr;
    HWND                    m_hwnd = nullptr;
    std::wstring            m_initError;

    // Per-monitor shell thumbnail + dark wash (client coords = virtual desktop).
    struct MonitorBackdrop
    {
        RECT                        rcMonitor = {};
        RECT                        rcWork = {};
        RECT                        rcTaskbar = {};
        ComPtr<IDCompositionVisual3Compat> washVisual;
        ComPtr<IDCompositionVisual3Compat> shellContainer;
        ComPtr<IDCompositionVisual2> shellThumb;
        HTHUMBNAIL                  hShellThumb = nullptr;
        ComPtr<IDCompositionVisual2> taskbarContainer;
        ComPtr<IDCompositionVisual2> taskbarThumb;
        HTHUMBNAIL                  hTaskbarThumb = nullptr;
        ComPtr<IDCompositionVisual2> wallpaperVisual;
        ComPtr<IDCompositionSurface> wallpaperSurface;
        UINT                        wallpaperWidth = 0;
        UINT                        wallpaperHeight = 0;
        WallpaperPlacement          wallpaperPlacement = WallpaperPlacement::Stretch;
        std::vector<ComPtr<IDCompositionVisual2>> wallpaperTiles;
        std::vector<POINT>           wallpaperTileOrigins;
    };

    // ---- Dimensions ----
    UINT                    m_width = 1600;
    UINT                    m_height = 900;
    float                   m_monW = 1920.0f;
    float                   m_monH = 1080.0f;
    float                   m_monOriginX = 0.0f;   // primary rcWork.left (screen px)
    float                   m_monOriginY = 0.0f;   // primary rcWork.top  (screen px)
    float                   m_viewX = 0.0f;   // primary rcWork origin in client px
    float                   m_viewY = 0.0f;
    std::vector<MonitorBackdrop> m_monitorBackdrops;
    ComPtr<IDCompositionSurface> m_washSurface;
    bool                    m_backdropBuildPending = false;
    bool                    m_backdropReady = false;
    bool                    m_minimized = false;
    bool                    m_rtl = false;
    bool                    m_thumbnailsDirty = false; // coalesce WM 0x327 bursts
    UINT                    m_wmShellHook = 0;
    bool                    m_shellHookRegistered = false;

    // ---- Frame timing ----
    SwitcherInputController          m_inputController;
    bool                                      m_windowClassRegistered = false;
    bool                                      m_inRunLoop = false;
    std::chrono::steady_clock::time_point   m_prevFrame;
    std::chrono::steady_clock::time_point   m_lastKeyProcessed{};
    int                                       m_wheelPendingSlots = 0;
    std::chrono::steady_clock::time_point     m_lastWheelTime{};
    UINT                                      m_heldNavigationKey = 0;
    bool                                      m_navigationKeyDown[256] = {};
    int                                       m_heldNavigationDirection = 0;
    std::chrono::steady_clock::time_point     m_heldNavigationStart{};
    UINT                                      m_touchId = 0;
    bool                                      m_touchActive = false;
    bool                                      m_touchMoved = false;
    int                                       m_touchAxis = -1;
    POINT                                     m_touchStartPoint{};
    POINT                                     m_touchLastPoint{};
    HWND                                      m_touchStartHwnd = nullptr;
    DWORD                                     m_touchLastEventTime = 0;
    float                                     m_touchScrollVelocity = 0.0f;
    bool                                      m_scrollSnapActive = false;
    UINT                                      m_penPointerId = 0;
    HWND                                      m_penHoveredHwnd = nullptr;
    int                                       m_penHoverScrollDirection = 0;
    bool                                      m_openingTabPending = false;
    std::chrono::steady_clock::time_point     m_openingTabStart{};

    // ---- Cards & view state ----
    std::vector<CardModel>  m_cards;
    ViewState               m_state = ViewState::Inactive;
    Timeline                m_animEnter;

    // ---- Smooth carousel scroll ----
    float                   m_scrollPos = 0.0f;
    float                   m_scrollTarget = 0.0f;
    float                   m_wrapScrollAdjustThisFrame = 0.0f;

    // Frozen browse scroll at exit start; held constant during ExitRepeatedRotate
    float                   m_exitScrollSnapshot = 0.0f;

    // ---- Discrete list rotation (uDWM m_ptlRotateListTimeline) ----
    Timeline                m_rotateTimeline;
    bool                    m_rotateBackward = false;
    bool                    m_showOutgoingDuringRotation = false;
    float                   m_rRepeatedRotateRate = 0.0f;
    int                     m_repeatedRotateStepsRemaining = 0;

    // ---- Selection ----
    HWND                    m_originalFrontHwnd = nullptr;
    HWND                    m_selectedHwnd = nullptr;
    HWND                    m_pendingActivateHwnd = nullptr;
    std::vector<HWND>       m_lastPaintOrder;
    HWND                    m_hitHwnd = nullptr;

    // ---- Accessibility ----
    IAccessible* m_pAccessible = nullptr;
    bool                    m_comInitialized = false;
    bool                    m_comNeedsUninit = false;
    // ---- D3D11 device ----
    ComPtr<ID3D11Device>              m_d3dDevice;

    // ---- DirectComposition resources ----
    ComPtr<IDCompositionDesktopDevice>  m_dcompDevice;
    ComPtr<IDCompositionTarget>         m_dcompTarget;
    ComPtr<IDCompositionVisual2>        m_rootVisual;
    ComPtr<IDCompositionVisual3Compat>  m_washVisual;
    ComPtr<IDCompositionVisual3Compat>  m_sceneVisual;

    // ---- DWM thumbnail API ----
    HMODULE                               m_dwmapi = nullptr;
    // Required at startup (LoadThumbApi); always non-null after Initialize succeeds.
    DwmpCreateSharedThumbnailVisual_fn    m_pfnCreateSharedThumbVisual = nullptr;
    DwmpQueryWindowThumbnailSourceSize_fn m_pfnQueryThumbSize = nullptr;
    GetWindowMinimizeRect_fn              m_pfnGetWindowMinimizeRect = nullptr;
};

// ============================================================================
// Flip3DComp_Composition.cpp — DirectComposition device init, desktop wash
// ============================================================================

// ============================================================================
// Flip3DCompApp::InitComposition
// ============================================================================

void Flip3DCompApp::Shutdown()
{
    // WM_DESTROY queues transition restoration while the card list still exists.
    if (m_hwnd && IsWindow(m_hwnd))
        DestroyWindow(m_hwnd);
    m_hwnd = nullptr;

    for (auto& card : m_cards)
    {
        if (card.m_hThumb)
        {
            DwmUnregisterThumbnail(card.m_hThumb);
            card.m_hThumb = nullptr;
        }
    }
    m_cards.clear();

    m_dcompTarget.Reset();
    m_washVisual.Reset();
    m_sceneVisual.Reset();
    m_rootVisual.Reset();
    m_dcompDevice.Reset();
    m_d3dDevice.Reset();
    UnloadThumbApi();

    if (m_windowClassRegistered)
    {
        UnregisterClassW(L"WindhawkFlip3DCompWindow", m_hInstance);
        m_windowClassRegistered = false;
    }

    m_inputController.Reset();
    m_state = ViewState::Inactive;
    ShutdownAccessibility();
}

void Flip3DCompApp::HandleInputAction(
    SwitcherInputAction action, HWND clickedHwnd)
{
    switch (action)
    {
    case SwitcherInputAction::Open:
    case SwitcherInputAction::OpenSticky:
        m_originalFrontHwnd = GetForegroundWindow();
        m_selectedHwnd = m_originalFrontHwnd;
        if (!BuildCards())
        {
            m_inputController.Reset();
            return;
        }
        m_state = ViewState::Interactive;
        UpdateCards(1.0f, 0.0f);
        m_dcompDevice->Commit();
        ShowWindow(m_hwnd, SW_SHOWNOACTIVATE);
        EnsureTopmost();
        break;
    case SwitcherInputAction::Next:
        RotateBy(1);
        UpdateCards(1.0f, 0.0f);
        m_dcompDevice->Commit();
        break;
    case SwitcherInputAction::Previous:
        RotateBy(-1);
        UpdateCards(1.0f, 0.0f);
        m_dcompDevice->Commit();
        break;
    case SwitcherInputAction::CommitSelection:
        SelectWindow(m_selectedHwnd);
        break;
    case SwitcherInputAction::ActivateClickedCard:
        SelectWindow(clickedHwnd ? clickedHwnd : m_selectedHwnd);
        break;
    case SwitcherInputAction::Cancel:
        m_state = ViewState::Inactive;
        ShowWindow(m_hwnd, SW_HIDE);
        break;
    default:
        break;
    }
}

void Flip3DCompApp::SelectWindow(HWND hwndTarget)
{
    if (!hwndTarget || !IsWindow(hwndTarget))
        return;

    m_pendingActivateHwnd = hwndTarget;

    EnsureTopmost();

    const int selectedIndex = FindCardIndex(hwndTarget);
    if (selectedIndex < 0)
    {
        ExitView();
        return;
    }

    if (m_cards[(size_t)selectedIndex].m_isMinimized)
        UpdateCardGeometry(m_cards[(size_t)selectedIndex], m_monW, m_monH, true);

    m_selectedHwnd = hwndTarget;
    m_lastPaintOrder.clear();
    FreezeCarouselVisuals();
    BeginExitView();

    const int selectedIndexAfterFreeze = FindCardIndex(hwndTarget);
    if (selectedIndexAfterFreeze > 0)
    {
        m_rRepeatedRotateRate = -(kExitDurationSec
            / (float)selectedIndexAfterFreeze);
        m_repeatedRotateStepsRemaining = selectedIndexAfterFreeze;
        m_state = ViewState::ExitRepeatedRotate;
        TickRepeatedRotate();
    }
}

void Flip3DCompApp::SelectFront()
{
    if (!m_cards.empty())
        SelectWindow(m_cards.front().m_hwnd);
}

void Flip3DCompApp::ExitView(bool commitScroll, float exitDurationSec)
{
    if (m_state == ViewState::Inactive
        || m_state == ViewState::Exit
        || m_state == ViewState::ExitRepeatedRotate)
    {
        return;
    }

    if (commitScroll)
        CommitCarouselScroll();

    FreezeCarouselVisuals();
    m_state = ViewState::Exit;
    m_animEnter.Restart(EnterProgress(), 0.0f, exitDurationSec,
        kCanHasEasing.load(std::memory_order_relaxed)
        ? InterpolationMode::CubicBezier
        : InterpolationMode::Linear);
    m_openingTabPending = false;
    m_heldNavigationKey = 0;
    ZeroMemory(m_navigationKeyDown, sizeof(m_navigationKeyDown));
    m_heldNavigationDirection = 0;
    m_heldNavigationStart = {};
}

void Flip3DCompApp::BeginExitView()
{
    if (m_state == ViewState::Inactive
        || m_state == ViewState::Exit
        || m_state == ViewState::ExitRepeatedRotate)
    {
        return;
    }

    m_lastPaintOrder.clear();
    NotifyAccessibilityEvent(EVENT_SYSTEM_DIALOGEND);
    FreezeCarouselVisuals();
    m_animEnter.Restart(EnterProgress(), 0.0f, kExitDurationSec,
        kCanHasEasing.load(std::memory_order_relaxed)
        ? InterpolationMode::CubicBezier
        : InterpolationMode::Linear);
    m_state = ViewState::Exit;
    m_openingTabPending = false;
    m_heldNavigationKey = 0;
    ZeroMemory(m_navigationKeyDown, sizeof(m_navigationKeyDown));
    m_heldNavigationDirection = 0;
    m_heldNavigationStart = {};
}

HWND Flip3DCompApp::HitTest3DScene(LONG x, LONG y) const
{
    const float p = EnterProgress();
    const auto camera = BuildCameraMatrix(p);
    const float px = static_cast<float>(x) + 0.5f;
    const float py = static_cast<float>(y) + 0.5f;

    HWND bestHwnd = nullptr;
    float bestKey = 1e10f;
    for (int i = 0; i < static_cast<int>(m_cards.size()); ++i)
    {
        const CardModel& c = m_cards[static_cast<size_t>(i)];
        if (!c.m_containerVisual)
            continue;

        const float slot = GetCardDisplaySlot(i);
        if (slot >= static_cast<float>(kMaxVisibleCards))
            continue;
        if (c.m_wrapOpacity < 0.01f && c.m_lastTargetOpacity < 0.01f)
            continue;

        const float t = ComputeCarouselBezierT(slot);
        const float flatRank = ComputeFlatDepthRank(slot, p, i);
        const auto model = BuildModelMatrix(c, t, p, flatRank);
        const auto mvp = Math::Multiply(model, camera);

        const float sw = static_cast<float>(std::max(c.m_srcWidth, 1));
        const float sh = static_cast<float>(std::max(c.m_srcHeight, 1));

        bool valid = true;
        auto project = [&](float lx, float ly) -> Vec2
            {
                const float vx = lx * mvp.m[0][0] + ly * mvp.m[1][0] + mvp.m[3][0];
                const float vy = lx * mvp.m[0][1] + ly * mvp.m[1][1] + mvp.m[3][1];
                const float w = lx * mvp.m[0][3] + ly * mvp.m[1][3] + mvp.m[3][3];
                if (w < 1e-6f)
                {
                    valid = false;
                    return { 0.0f, 0.0f };
                }
                return { vx / w, vy / w };
            };

        const Vec2 q[4] = {
            project(0.0f, 0.0f),
            project(sw, 0.0f),
            project(sw, sh),
            project(0.0f, sh),
        };
        if (!valid)
            continue;

        bool hasPositive = false;
        bool hasNegative = false;
        for (int e = 0; e < 4; ++e)
        {
            const Vec2& a = q[e];
            const Vec2& b = q[(e + 1) % 4];
            const float cross = (b.x - a.x) * (py - a.y) - (b.y - a.y) * (px - a.x);
            if (cross > 0.0f)
                hasPositive = true;
            else if (cross < 0.0f)
                hasNegative = true;
        }
        if (hasPositive && hasNegative)
            continue;

        const float key = ComputePaintKey(slot, p, i);
        if (key < bestKey)
        {
            bestKey = key;
            bestHwnd = c.m_hwnd;
        }
    }
    return bestHwnd;
}
static void LoadAnimationSettings()
{
    const bool enabled = Wh_GetIntSetting(L"CanHasEasing") != 0;
    kCanHasEasing.store(enabled, std::memory_order_relaxed);
    kScrollSmoothTimeSec.store(0.15f - (enabled ? 0.05f : 0.0f),
        std::memory_order_relaxed);
    kAnimationRate.store(1.0f - (enabled ? 0.2f : 0.0f),
        std::memory_order_relaxed);
    kShiftAnimationRate.store(0.1f - (enabled ? 0.05f : 0.0f),
        std::memory_order_relaxed);
}

// ============================================================================
// Flip3DComp_ShellHook.cpp — dynamic card list via RegisterShellHookWindow
// ============================================================================

#include <algorithm>

namespace
{
    std::atomic<HWND> g_hostWindow{ nullptr };
    HANDLE g_hostStopEvent = nullptr;

    // Only the host thread accesses pending animation restoration.
    const UINT_PTR kRestoreTransitionsTimerId = 0x7F36;
    std::vector<HWND> g_transitionWindows;
    ULONGLONG g_restoreTransitionsAt = 0;


    void SetWindowTransitionsDisabled(HWND hwnd, bool disabled) {
        if (!hwnd || !IsWindow(hwnd))
            return;

        const BOOL value = disabled ? TRUE : FALSE;
        DwmSetWindowAttribute(hwnd, DWMWA_TRANSITIONS_FORCEDISABLED,
            &value, sizeof(value));
    }

    void RestorePendingTransitions()
    {
        if (g_restoreTransitionsAt)
        {
            KillTimer(g_hostWindow, kRestoreTransitionsTimerId);
            g_restoreTransitionsAt = 0;
        }

        for (HWND hwnd : g_transitionWindows)
            SetWindowTransitionsDisabled(hwnd, false);
        g_transitionWindows.clear();
    }
}

// ============================================================================
// Flip3DCompApp::QualifiesForView
// ============================================================================
bool Flip3DCompApp::QualifiesForView(HWND hwnd) const
{
    if (!hwnd || hwnd == m_hwnd || hwnd == GetDesktopWindow())
        return false;

    if (!IsWindowVisible(hwnd))
        return false;

    if (m_selectedHwnd && hwnd == m_selectedHwnd)
        return true;

    const LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
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
    return m_state != ViewState::Inactive;
}

bool Flip3DCompApp::IsNeverHiddenWindow(HWND hwnd) const
{
    if (!hwnd)
        return true;

    if (hwnd == m_hwnd)
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
    for (const CardModel& card : m_cards)
        SetWindowTransitionsDisabled(card.m_hwnd, disabled);
}

// ============================================================================
void Flip3DCompApp::EnterFlip3DWindowMode()
{
    if (!m_hwnd || m_shellHookRegistered)
        return;

    // Finish the previous session before disabling this session's transitions.
    RestorePendingTransitions();

    SetCardTransitionsDisabled(true);

    if (!m_wmShellHook)
        m_wmShellHook = RegisterWindowMessageW(L"SHELLHOOK");

    if (RegisterShellHookWindow(m_hwnd))
        m_shellHookRegistered = true;
}

void Flip3DCompApp::LeaveFlip3DWindowMode()
{
    // Restore/activation completes asynchronously. Keep the delay on the host
    // thread so teardown can finish restoration before the module unloads.
    g_transitionWindows.reserve(g_transitionWindows.size() + m_cards.size());
    for (const CardModel& card : m_cards)
        g_transitionWindows.push_back(card.m_hwnd);

    g_restoreTransitionsAt = GetTickCount64() + 2000;
    if (!g_hostWindow
        || !SetTimer(g_hostWindow, kRestoreTransitionsTimerId, 2000, nullptr))
    {
        Wh_Log(L"Failed to schedule transition restoration; restoring now");
        RestorePendingTransitions();
    }

    if (m_hwnd && m_shellHookRegistered)
    {
        DeregisterShellHookWindow(m_hwnd);
        m_shellHookRegistered = false;
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

    if (hwnd == m_hwnd || IsNeverHiddenWindow(hwnd))
        return;

    const bool qualifies = QualifiesForView(hwnd);
    const int  cardIdx = FindCardIndex(hwnd);

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

    for (int i = 0; i < (int)m_cards.size(); ++i)
    {
        if (m_cards[(size_t)i].m_hwnd == hwnd)
            return i;
    }
    return -1;
}

// ============================================================================
HRESULT Flip3DCompApp::CreateCardVisual(CardModel& card)
{
    if (!m_dcompDevice || !m_sceneVisual || !card.m_hwnd)
        return E_INVALIDARG;

    DWM_THUMBNAIL_PROPERTIES tp = {};
    tp.dwFlags = DWM_TNP_VISIBLE | DWM_TNP_RECTDESTINATION
        | DWM_TNP_ENABLE3D | DWM_TNP_DISABLEFORCECVI;
    tp.fVisible = TRUE;
    tp.rcDestination = { 0, 0, card.m_srcWidth, card.m_srcHeight };

    HTHUMBNAIL thumbnail = nullptr;
    void* pv = nullptr;
    HRESULT hr = m_pfnCreateSharedThumbVisual(
        m_hwnd,
        card.m_hwnd,
        DWM_TNF_DWMWINDOW,
        &tp,
        m_dcompDevice.Get(),
        &pv,
        &thumbnail);

    ComPtr<IDCompositionVisual> thumbBase;
    thumbBase.Attach(static_cast<IDCompositionVisual*>(pv));
    ComPtr<IDCompositionVisual2> visual;
    ComPtr<IDCompositionVisual2> container;
    ComPtr<IDCompositionVisual3Compat> containerVisual;
    if (SUCCEEDED(hr) && !thumbBase)
        hr = E_FAIL;
    if (SUCCEEDED(hr))
        hr = thumbBase.As(&visual);
    if (SUCCEEDED(hr))
        hr = m_dcompDevice->CreateVisual(&container);
    if (SUCCEEDED(hr))
        hr = container->AddVisual(visual.Get(), FALSE, nullptr);
    if (SUCCEEDED(hr))
        hr = container.As(&containerVisual);
    if (SUCCEEDED(hr))
        hr = m_sceneVisual->AddVisual(container.Get(), TRUE, nullptr);
    if (FAILED(hr))
    {
        if (thumbnail)
            DwmUnregisterThumbnail(thumbnail);
        return hr;
    }

    card.m_hThumb = thumbnail;
    card.m_visual = std::move(visual);
    card.m_containerVisual = std::move(containerVisual);
    return S_OK;
}

// ============================================================================
bool Flip3DCompApp::AddCardForWindow(HWND hwnd)
{
    if (!hwnd || m_cards.size() >= (size_t)kMaxCards)
        return false;

    if (FindCardIndex(hwnd) >= 0)
        return false;

    CardModel card;
    card.m_hwnd = hwnd;
    card.m_initialCarouselIndex = (int)m_cards.size();
    UpdateCardGeometry(card, m_monW, m_monH);

    if (FAILED(CreateCardVisual(card)))
        return false;

    m_cards.push_back(std::move(card));
    SetWindowTransitionsDisabled(hwnd, true);

    if (m_dcompDevice)
        m_dcompDevice->Commit();

    return true;
}

// ============================================================================
void Flip3DCompApp::RemoveCardAt(size_t index)
{
    if (index >= m_cards.size())
        return;

    CardModel& card = m_cards[index];
    if (m_pendingActivateHwnd == card.m_hwnd)
        m_pendingActivateHwnd = nullptr;

    if (card.m_containerVisual && m_sceneVisual)
    {
        ComPtr<IDCompositionVisual> sceneBase;
        if (SUCCEEDED(m_sceneVisual.As(&sceneBase)))
            sceneBase->RemoveVisual(card.m_containerVisual.Get());
    }

    if (card.m_hThumb)
    {
        DwmUnregisterThumbnail(card.m_hThumb);
        card.m_hThumb = nullptr;
    }

    SetWindowTransitionsDisabled(card.m_hwnd, false);

    card.m_visual.Reset();
    card.m_containerVisual.Reset();

    m_cards.erase(m_cards.begin() + (ptrdiff_t)index);
    if (m_cards.empty() && m_state == ViewState::ExitRepeatedRotate)
    {
        m_repeatedRotateStepsRemaining = 0;
        m_rRepeatedRotateRate = 0.0f;
        m_rotateTimeline = {};
        m_state = ViewState::Exit;
    }

    if (m_dcompDevice)
        m_dcompDevice->Commit();
}

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
    if (m_comInitialized)
        return true;

    const HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (hr == RPC_E_CHANGED_MODE)
    {
        // COM is usable, but this thread does not own an initialization count.
        return true;
    }

    if (FAILED(hr))
        return false;

    m_comInitialized = true;
    m_comNeedsUninit = true;
    return true;
}

void Flip3DCompApp::ShutdownAccessibility()
{
    if (m_pAccessible)
    {
        static_cast<Flip3DAccessible*>(m_pAccessible)->Disconnect();
        m_pAccessible->Release();
        m_pAccessible = nullptr;
    }

    if (m_comNeedsUninit)
    {
        CoUninitialize();
        m_comNeedsUninit = false;
    }
    m_comInitialized = false;
}

IAccessible* Flip3DCompApp::GetAccessibleObject()
{
    if (!m_pAccessible)
    {
        IAccessible* pAcc = nullptr;
        if (FAILED(Flip3DAccessible::Create(this, &pAcc)))
            return nullptr;
        m_pAccessible = pAcc;
    }
    return m_pAccessible;
}

// ============================================================================
// Flip3DCompApp::NotifyAccessibilityEvent
// uDWM: 0x14 dialog start, 0x15 dialog end, 0x8005 focus on front child.
// ============================================================================
void Flip3DCompApp::NotifyAccessibilityEvent(DWORD event, LONG childId)
{
    if (!m_hwnd)
        return;

    NotifyWinEvent(event, m_hwnd, OBJID_CLIENT, childId);
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
    return (int)m_cards.size();
}

// ============================================================================
// Flip3DCompApp::AccessibleWindowName
// ============================================================================
HRESULT Flip3DCompApp::AccessibleWindowName(int index, BSTR* pszName) const
{
    if (!pszName)
        return E_POINTER;

    *pszName = nullptr;

    if (index < 0 || index >= (int)m_cards.size())
        return E_INVALIDARG;

    const CardModel& card = m_cards[(size_t)index];
    wchar_t title[512] = {};
    if (card.m_hwnd && IsWindow(card.m_hwnd))
        GetWindowTextW(card.m_hwnd, title, (int)std::size(title));

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
        || index < 0 || index >= (int)m_cards.size())
        return false;

    const CardModel& c = m_cards[(size_t)index];

    const float p = EnterProgress();
    const auto  camera = BuildCameraMatrix(p);
    const float carouselSlot = GetCardDisplaySlot(index);

    const float t = ComputeCarouselBezierT(carouselSlot);
    const float flatRank = ComputeFlatDepthRank(carouselSlot, p, index);
    const auto  model = BuildModelMatrix(c, t, p, flatRank);
    const auto  mvp = Math::Multiply(model, camera);

    const float sw = (float)std::max(c.m_srcWidth, 1);
    const float sh = (float)std::max(c.m_srcHeight, 1);

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

    float minX = 1e10f, minY = 1e10f;
    float maxX = -1e10f, maxY = -1e10f;
    for (const Vec2& v : corners)
    {
        minX = std::min(minX, v.x);
        minY = std::min(minY, v.y);
        maxX = std::max(maxX, v.x);
        maxY = std::max(maxY, v.y);
    }

    POINT origin = { 0, 0 };
    ClientToScreen(m_hwnd, &origin);

    *pxLeft = origin.x + (long)std::floor(minX);
    *pyTop = origin.y + (long)std::floor(minY);
    *pcxWidth = (long)std::ceil(maxX - minX);
    *pcyHeight = (long)std::ceil(maxY - minY);
    if (*pcxWidth < 0)  *pcxWidth = 0;
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
    ScreenToClient(m_hwnd, &pt);

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
    if (!GetWindowRect(m_hwnd, &rc))
        return false;

    return PtInRect(&rc, screenPt) != FALSE;
}

// ============================================================================
// Flip3DCompApp::AccessibleRotateToIndex
// accSelect → smooth scroll the window to the carousel front.
// ============================================================================
HRESULT Flip3DCompApp::AccessibleRotateToIndex(int index)
{
    if (index < 0 || index >= (int)m_cards.size())
        return E_INVALIDARG;

    if (m_state == ViewState::Exit || m_state == ViewState::ExitRepeatedRotate)
        return DISP_E_MEMBERNOTFOUND;

    RotateToWindow(m_cards[(size_t)index].m_hwnd);
    return S_OK;
}

// ============================================================================
// Flip3DCompApp::AccessibleSelectIndex
// uDWM accDoDefaultAction → SelectWindow.
// ============================================================================
HRESULT Flip3DCompApp::AccessibleSelectIndex(int index)
{
    if (index < 0 || index >= (int)m_cards.size())
        return E_INVALIDARG;

    SelectWindow(m_cards[(size_t)index].m_hwnd);
    return S_OK;
}

// ============================================================================
// Flip3DAccessible.cpp — COM IAccessible (uDWM CFlip3DAccessible port)
// ============================================================================

#include <algorithm>
#include <cstddef>

namespace {

    constexpr int kAccStringContainer = 3000;  // uDWM resource 0xBB8
    constexpr int kAccStringDefaultAction = 3001;  // uDWM resource 0xBB9

    // Fallbacks when string resources are absent (standalone exe).
    constexpr wchar_t kFallbackContainerName[] = L"Flip 3D";
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
    : m_app(app)
{
}

void Flip3DAccessible::Disconnect()
{
    // The app can close while an accessibility client still holds its interface.
    m_app = nullptr;
    const HRESULT hr = CoDisconnectObject(this, 0);
    if (FAILED(hr))
        Wh_Log(L"Failed to disconnect the accessibility object: 0x%08X", hr);
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

    if (riid == __uuidof(IUnknown) || riid == __uuidof(IDispatch)
        || riid == __uuidof(IAccessible))
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
    return (ULONG)InterlockedIncrement((LONG*)&m_ref);
}

STDMETHODIMP_(ULONG) Flip3DAccessible::Release()
{
    const ULONG ref = (ULONG)InterlockedDecrement((LONG*)&m_ref);
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
    return m_app ? m_app->AccessibleChildCount() : 0;
}

void Flip3DAccessible::SetChildIndex(int index, VARIANT* pvarPlace) const
{
    VariantInit(pvarPlace);
    pvarPlace->vt = VT_I4;
    pvarPlace->lVal = index;
}

STDMETHODIMP Flip3DAccessible::get_accParent(IDispatch** ppdispParent)
{
    if (!ppdispParent)
        return E_POINTER;

    *ppdispParent = nullptr;
    // uDWM passes object id 0 for the desktop root accessible.
    return AccessibleObjectFromWindow(
        GetDesktopWindow(), 0, __uuidof(IDispatch), (void**)ppdispParent);
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

    if (!m_app)
        return DISP_E_MEMBERNOTFOUND;

    return m_app->AccessibleWindowName(index, pszName);
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
    pvarRole->vt = VT_I4;
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
    pvarState->vt = VT_I4;
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
    *pidTopic = 0;
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

    if (varChild.lVal == CHILDID_SELF || !m_app)
        return E_INVALIDARG;

    const int index = varChild.lVal - 1;
    if (index < 0 || index >= GetChildrenCount())
        return E_INVALIDARG;

    return m_app->AccessibleRotateToIndex(index);
}

STDMETHODIMP Flip3DAccessible::accLocation(long* pxLeft, long* pyTop, long* pcxWidth,
    long* pcyHeight, VARIANT varChild)
{
    if (!pxLeft || !pyTop || !pcxWidth || !pcyHeight || varChild.vt != VT_I4)
        return E_INVALIDARG;

    if (varChild.lVal == CHILDID_SELF)
    {
        if (!m_app || !m_app->WindowHandle())
            return E_INVALIDARG;

        RECT rc = {};
        if (!GetWindowRect(m_app->WindowHandle(), &rc))
            return E_INVALIDARG;

        *pxLeft = rc.left;
        *pyTop = rc.top;
        *pcxWidth = std::max(0L, rc.right - rc.left);
        *pcyHeight = std::max(0L, rc.bottom - rc.top);
        return S_OK;
    }

    const int index = varChild.lVal - 1;
    if (index < 0 || index >= GetChildrenCount() || !m_app)
        return E_INVALIDARG;

    if (!m_app->AccessibleCardScreenRect(index, pxLeft, pyTop, pcxWidth, pcyHeight))
        return E_INVALIDARG;

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
    HRESULT hr = DISP_E_MEMBERNOTFOUND;

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
        int current = varStart.lVal;
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
    if (!pvarChildAtPoint || !m_app)
        return E_POINTER;

    const int index = m_app->AccessibleHitTest(xLeft, yTop);
    if (index >= 0)
    {
        SetChildIndex(index + 1, pvarChildAtPoint);
        return S_OK;
    }

    POINT pt = { (int)xLeft, (int)yTop };
    if (m_app->AccessiblePointInView(pt))
    {
        SetChildIndex(CHILDID_SELF, pvarChildAtPoint);
        return S_OK;
    }

    return S_FALSE;
}

STDMETHODIMP Flip3DAccessible::accDoDefaultAction(VARIANT varChild)
{
    if (varChild.vt != VT_I4 || varChild.lVal == CHILDID_SELF || !m_app)
        return E_INVALIDARG;

    const int index = varChild.lVal - 1;
    if (index < 0 || index >= GetChildrenCount())
        return E_INVALIDARG;

    return m_app->AccessibleSelectIndex(index);
}

STDMETHODIMP Flip3DAccessible::put_accName(VARIANT /*varChild*/, BSTR /*szName*/)
{
    return E_NOTIMPL;
}

STDMETHODIMP Flip3DAccessible::put_accValue(VARIANT /*varChild*/, BSTR /*szValue*/)
{
    return E_NOTIMPL;
}

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
    m_startValue = startValue;
    m_endValue = endValue;
    m_durationSec = std::max(durationSec, 0.0001f);
    m_elapsed = 0.0f;
    m_mode = mode;
    m_active = true;
}

void Timeline::Update(float dtSeconds)
{
    if (m_active)
    {
        m_elapsed = std::min(m_elapsed + dtSeconds, m_durationSec);
        if (m_elapsed >= m_durationSec)
            m_active = false;
    }
}

float Timeline::RawProgress() const
{
    return std::clamp(m_elapsed / std::max(m_durationSec, 0.0001f), 0.0f, 1.0f);
}

float Timeline::Value() const
{
    float t = RawProgress();

    if (m_mode == InterpolationMode::Cubic)
    {
        // uDWM / flip3d CTimeline::Progress — smoothstep
        t = t * t * (3.0f - (2.0f * t));
    }
    else if (m_mode == InterpolationMode::CubicBezier)
    {
        t = Math::TimelineEase(t);
    }

    return Math::Lerp(m_startValue, m_endValue, t);
}

float Timeline::LinearValue() const
{
    return Math::Lerp(m_startValue, m_endValue, RawProgress());
}

// ============================================================================
// Flip3DComp_Update.cpp — Per-frame update, camera, card transforms, scroll
// ============================================================================
//
// Carousel uses fractional smooth scroll (m_scrollPos) for browse.
// Each card's m_displaySlot is the single render coordinate during exit;
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
    bool IsDwmShiftAnimationSlowdownActive()
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

        return status == ERROR_SUCCESS && enabled != 0
            && (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
    }
} // This makes Flip3D compatible with the dwm slow animations DWORD Value like it was on Vista/7

// ============================================================================
// Flip3DCompApp::EnterProgress
// ============================================================================
float Flip3DCompApp::EnterProgress() const
{
    return std::max(std::min(m_animEnter.Value(), 1.0f), 0.0f);
}

// ============================================================================
// Flip3DCompApp::ReplayEnterAnimation
// ============================================================================
void Flip3DCompApp::ReplayEnterAnimation()
{
    m_state = ViewState::Enter;
    m_animEnter.Restart(0.0f, 1.0f, kEnterExitDurationSec,
        kCanHasEasing
        ? InterpolationMode::CubicBezier
        : InterpolationMode::Linear);
    m_scrollPos = 0.0f;
    m_scrollTarget = 0.0f;
    m_wrapScrollAdjustThisFrame = 0.0f;
    m_heldNavigationKey = 0;
    ZeroMemory(m_navigationKeyDown, sizeof(m_navigationKeyDown));
    m_heldNavigationDirection = 0;
    m_heldNavigationStart = {};
    m_repeatedRotateStepsRemaining = 0;
    m_openingTabPending = false;
    m_openingTabStart = {};
    m_selectedHwnd = nullptr;
    m_rRepeatedRotateRate = 0.0f;
    m_showOutgoingDuringRotation = false;
    m_exitScrollSnapshot = 0.0f;
    m_lastPaintOrder.clear();
    m_originalFrontHwnd = m_cards.empty() ? nullptr : m_cards[0].m_hwnd;
    InvalidateDisplaySlots();
}

// ============================================================================
// Flip3DCompApp::RotateListPhysically
// List splice when committing fractional scroll to discrete slots (exit only).
// ============================================================================
void Flip3DCompApp::RotateListPhysically(bool backward)
{
    if (m_cards.size() <= 1)
        return;

    if (backward)
        std::rotate(m_cards.rbegin(), m_cards.rbegin() + 1, m_cards.rend());
    else
        std::rotate(m_cards.begin(), m_cards.begin() + 1, m_cards.end());
}

// ============================================================================
// Flip3DCompApp::WrapCarouselScroll
// Keep scrollPos in (-0.5, 0.5) by rotating the list at integer boundaries.
// Forward wrap decouples the outgoing card's display slot so it can finish the
// front-edge opacity ramp (slot in [-1, 0]) without jumping to the back.
// ============================================================================
void Flip3DCompApp::WrapCarouselScroll()
{
    if (m_cards.size() <= 1)
        return;

    m_wrapScrollAdjustThisFrame = 0.0f;

    const int N = (int)m_cards.size();
    int guard = 0;
    while (m_scrollPos >= 0.5f && guard++ <= N)
    {
        const HWND  outgoingHwnd = m_cards[0].m_hwnd;
        const float outgoingSlot = GetCardDisplaySlot(0);

        RotateListPhysically(false);
        m_scrollPos -= 1.0f;
        m_scrollTarget -= 1.0f;
        m_wrapScrollAdjustThisFrame -= 1.0f;

        OnCarouselWrapForward(outgoingHwnd, outgoingSlot);
    }
    guard = 0;
    while (m_scrollPos <= -0.5f && guard++ <= N)
    {
        const HWND incomingHwnd = m_cards[(size_t)(N - 1)].m_hwnd;

        RotateListPhysically(true);
        m_scrollPos += 1.0f;
        m_scrollTarget += 1.0f;
        m_wrapScrollAdjustThisFrame += 1.0f;

        OnCarouselWrapBackward(incomingHwnd);
    }
}

// ============================================================================
// Display slot helpers — continuous carousel coordinates per card
// ============================================================================
float Flip3DCompApp::CardListSlot(int listIndex) const
{
    return (float)listIndex - m_scrollPos;
}

float Flip3DCompApp::CaptureCarouselVisualSlot(int listIndex) const
{
    if (listIndex < 0 || listIndex >= (int)m_cards.size())
        return 0.0f;

    const CardModel& c = m_cards[(size_t)listIndex];

    if (c.m_wrapPhase == CarouselWrapPhase::EnteringBack)
        return CardListSlot(listIndex);

    if (c.m_wrapPhase != CarouselWrapPhase::None)
        return c.m_displaySlot;

    return CardListSlot(listIndex);
}

float Flip3DCompApp::GetCardDisplaySlot(int listIndex) const
{
    if (listIndex < 0 || listIndex >= (int)m_cards.size())
        return 0.0f;

    const CardModel& c = m_cards[(size_t)listIndex];

    if (m_state == ViewState::Exit || m_state == ViewState::ExitRepeatedRotate)
        return c.m_displaySlot;

    return CaptureCarouselVisualSlot(listIndex);
}

void Flip3DCompApp::FinishWrapPhase(CardModel& card, int listIndex)
{
    card.m_wrapPhase = CarouselWrapPhase::None;
    card.m_wrapFadeStartListSlot = 0.0f;
    card.m_displaySlot = CardListSlot(listIndex);
    card.m_displaySlotValid = true;
    card.m_wrapProgress = 1.0f;
}

void Flip3DCompApp::BeginEnteringBackWrap(CardModel& card, int listIndex)
{
    card.m_wrapPhase = CarouselWrapPhase::EnteringBack;
    card.m_wrapFadeStartListSlot = CardListSlot(listIndex);
    card.m_displaySlot = CarouselEdgeSpan();
    card.m_displaySlotValid = true;
    card.m_wrapProgress = 0.0f;
    // Start fully transparent; the rendered opacity will fade toward the
    // configured back-card target instead of appearing at the minimum value.
    card.m_wrapOpacity = 0.0f;
    card.m_wrapFadeActive = true;
}

void Flip3DCompApp::StepEnteringBackWrap(CardModel& card, int listIndex)
{
    const float listSlot = CardListSlot(listIndex);
    const float span = CarouselEdgeSpan();
    const float fadeRange = span - card.m_wrapFadeStartListSlot;

    constexpr float kWrapSettleEpsilon = 0.06f;

    if (fadeRange <= 1e-4f)
    {
        FinishWrapPhase(card, listIndex);
        return;
    }

    const float scrolled = card.m_wrapFadeStartListSlot - listSlot;
    const float t = std::clamp(scrolled / fadeRange, 0.0f, 1.0f);
    card.m_displaySlot = listSlot + fadeRange * (1.0f - t);
    card.m_wrapProgress = t;

    if (t >= 1.0f - kWrapSettleEpsilon)
        FinishWrapPhase(card, listIndex);
}

void Flip3DCompApp::SettleWrapDisplaySlots()
{
    if (std::abs(m_scrollTarget - m_scrollPos) > kScrollSettleEpsilon)
        return;

    constexpr float kWrapSettleEpsilon = 0.06f;

    for (int k = 0; k < (int)m_cards.size(); ++k)
    {
        CardModel& c = m_cards[(size_t)k];
        const float listSlot = CardListSlot(k);

        switch (c.m_wrapPhase)
        {
        case CarouselWrapPhase::EnteringFront:
            if (std::abs(c.m_displaySlot - listSlot) < kWrapSettleEpsilon
                || c.m_displaySlot > listSlot + kWrapSettleEpsilon)
            {
                FinishWrapPhase(c, k);
            }
            break;

        case CarouselWrapPhase::ExitingFront:
            if (c.m_displaySlot <= kFrontCardExitSlot)
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
    const int count = (int)m_cards.size();
    return (float)std::min(std::max(count, 2), kMaxVisibleCards);
}

void Flip3DCompApp::InvalidateDisplaySlots()
{
    for (auto& c : m_cards)
    {
        c.m_displaySlotValid = false;
        c.m_wrapPhase = CarouselWrapPhase::None;
    }
}

void Flip3DCompApp::SyncDisplaySlotsToList()
{
    for (int k = 0; k < (int)m_cards.size(); ++k)
    {
        CardModel& c = m_cards[(size_t)k];
        if (c.m_wrapPhase != CarouselWrapPhase::None)
            continue;

        c.m_displaySlot = CardListSlot(k);
        c.m_displaySlotValid = true;
    }
}

void Flip3DCompApp::AdvanceWrapDisplaySlots(float deltaScrollPos)
{
    if (deltaScrollPos == 0.0f && m_wrapScrollAdjustThisFrame == 0.0f)
        return;

    constexpr float kWrapSettleEpsilon = 0.06f;
    const float enteringFrontDelta = deltaScrollPos - m_wrapScrollAdjustThisFrame;

    for (int k = 0; k < (int)m_cards.size(); ++k)
    {
        CardModel& c = m_cards[(size_t)k];
        const float listSlot = CardListSlot(k);

        switch (c.m_wrapPhase)
        {
        case CarouselWrapPhase::ExitingFront:
            c.m_displaySlot -= deltaScrollPos * kFrontCardFadeSpeed;
            if (c.m_displaySlot <= kFrontCardExitSlot)
                BeginEnteringBackWrap(c, k);
            break;

        case CarouselWrapPhase::EnteringBack:
            StepEnteringBackWrap(c, k);
            break;

        case CarouselWrapPhase::EnteringFront:
            c.m_displaySlot -= enteringFrontDelta;
            // Update wrap progress for entering-front so opacity can be
            // interpolated from the minimum to target value.
            {
                const float fadeRangeFront = listSlot - c.m_wrapFadeStartListSlot;
                if (fadeRangeFront > 1e-4f)
                {
                    const float scrolledFront = listSlot - c.m_displaySlot;
                    c.m_wrapProgress = std::clamp(scrolledFront / fadeRangeFront, 0.0f, 1.0f);
                }
            }
            if (std::abs(c.m_displaySlot - listSlot) < kWrapSettleEpsilon
                || c.m_displaySlot > listSlot + kWrapSettleEpsilon)
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
    for (int k = 0; k < (int)m_cards.size(); ++k)
    {
        CardModel& c = m_cards[(size_t)k];
        if (c.m_hwnd != outgoingHwnd
            && c.m_wrapPhase == CarouselWrapPhase::EnteringFront)
        {
            FinishWrapPhase(c, k);
        }
    }

    for (auto& c : m_cards)
    {
        if (c.m_hwnd != outgoingHwnd)
            continue;

        c.m_displaySlot = outgoingSlot;
        c.m_wrapPhase = CarouselWrapPhase::ExitingFront;
        c.m_displaySlotValid = true;
        break;
    }

    SyncDisplaySlotsToList();
}

void Flip3DCompApp::OnCarouselWrapBackward(HWND incomingHwnd)
{
    for (int k = 0; k < (int)m_cards.size(); ++k)
    {
        CardModel& c = m_cards[(size_t)k];
        if (c.m_hwnd == incomingHwnd)
            continue;

        if (c.m_wrapPhase == CarouselWrapPhase::EnteringFront
            || c.m_wrapPhase == CarouselWrapPhase::ExitingFront
            || c.m_wrapPhase == CarouselWrapPhase::EnteringBack)
        {
            FinishWrapPhase(c, k);
        }
    }

    for (int k = 0; k < (int)m_cards.size(); ++k)
    {
        CardModel& c = m_cards[(size_t)k];
        if (c.m_hwnd != incomingHwnd)
            continue;

        c.m_wrapPhase = CarouselWrapPhase::EnteringFront;
        c.m_displaySlot = CardListSlot(k) - 0.55f;
        c.m_wrapFadeStartListSlot = c.m_displaySlot; // start offset for fade progress
        c.m_wrapProgress = 0.0f;
        c.m_displaySlotValid = true;
        break;
    }

    SyncDisplaySlotsToList();
}

void Flip3DCompApp::FreezeCarouselVisuals()
{
    std::vector<std::pair<HWND, float>> visualByHwnd;
    visualByHwnd.reserve(m_cards.size());
    for (int k = 0; k < (int)m_cards.size(); ++k)
        visualByHwnd.emplace_back(m_cards[(size_t)k].m_hwnd,
            CaptureCarouselVisualSlot(k));

    WrapCarouselScroll();

    for (int k = 0; k < (int)m_cards.size(); ++k)
    {
        CardModel& c = m_cards[(size_t)k];
        c.m_wrapPhase = CarouselWrapPhase::None;
        c.m_wrapFadeStartListSlot = 0.0f;

        for (const auto& entry : visualByHwnd)
        {
            if (entry.first == c.m_hwnd)
            {
                c.m_displaySlot = entry.second;
                break;
            }
        }

        c.m_displaySlotValid = true;
    }

    m_exitScrollSnapshot = m_scrollPos;
    m_scrollTarget = m_scrollPos;
}

void Flip3DCompApp::UpdateVisualSlots(float enterProgress)
{
    if (m_state != ViewState::Exit && m_state != ViewState::ExitRepeatedRotate)
        return;

    const int cardCount = (int)m_cards.size();

    if (m_state == ViewState::ExitRepeatedRotate && m_rotateTimeline.IsActive())
    {
        for (int k = 0; k < cardCount; ++k)
            m_cards[(size_t)k].m_displaySlot = ComputeRotationDisplaySlot(k);

        return;
    }

    const bool rotationDone = m_state == ViewState::ExitRepeatedRotate
        && !m_cards.empty()
        && m_selectedHwnd
        && m_cards[0].m_hwnd == m_selectedHwnd;

    if (m_state == ViewState::Exit || rotationDone)
    {
        m_scrollPos = m_exitScrollSnapshot * enterProgress;
        m_scrollTarget = m_scrollPos;

        for (int k = 0; k < cardCount; ++k)
            m_cards[(size_t)k].m_displaySlot = CardListSlot(k);
    }

    // ExitRepeatedRotate with more steps pending: keep m_displaySlot unchanged
}

// ============================================================================
// Flip3DCompApp::RotateSelectedToListFront
// uDWM: after scroll settles, splice the ring so the selection is list index 0
// (e.g. A B C D E F G → C D E F G A B). Cards after C in the ring follow C at
// slots 1…, not the windows that were before C (A B).
// ============================================================================
void Flip3DCompApp::RotateSelectedToListFront()
{
    if (!m_selectedHwnd || m_cards.size() <= 1)
        return;

    const int idx = FindCardIndex(m_selectedHwnd);
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
    if (m_cards.size() <= 1)
    {
        m_scrollPos = 0.0f;
        m_scrollTarget = 0.0f;
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
    if (m_cards.size() <= 1)
    {
        m_scrollPos = 0.0f;
        m_scrollTarget = 0.0f;
        return;
    }

    WrapCarouselScroll();
    RotateSelectedToListFront();
    m_scrollPos = 0.0f;
    m_scrollTarget = 0.0f;

    for (int k = 0; k < (int)m_cards.size(); ++k)
    {
        CardModel& c = m_cards[(size_t)k];
        c.m_wrapPhase = CarouselWrapPhase::None;
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
    if (!deltaSteps || m_cards.size() <= 1
        || m_state == ViewState::Exit
        || m_state == ViewState::ExitRepeatedRotate)
        return;

    // Prevent fractional drift by anchoring the scroll target to the
    // nearest integer before applying a discrete rotate step. This ensures
    // repeated rotates increment by exactly one slot.
    const float base = std::round(m_scrollTarget);
    m_scrollTarget = base + (float)deltaSteps;
}

// ============================================================================
// Flip3DCompApp::ComputeScrollTargetForCard
// Scroll value that places listIndex at the visual front (slot ≈ 0).
// Short-path wrap is only used when it moves scrollPos in the direction that
// reduces |visualSlot| (slot = index - scrollPos increases as scrollPos falls).
// ============================================================================
float Flip3DCompApp::ComputeScrollTargetForCard(int listIndex) const
{
    const int N = (int)m_cards.size();
    if (N <= 1)
        return m_scrollPos;

    const float visualSlot = (float)listIndex - m_scrollPos;
    if (std::abs(visualSlot) < kScrollSettleEpsilon)
        return m_scrollPos;

    float shortDelta = visualSlot;
    if (shortDelta > (float)N * 0.5f)
        shortDelta -= (float)N;
    else if (shortDelta < -(float)N * 0.5f)
        shortDelta += (float)N;

    if (visualSlot > 0.0f)
    {
        if (shortDelta > 0.0f && std::abs(shortDelta) < std::abs(visualSlot))
            return m_scrollPos + shortDelta;
    }
    else
    {
        if (shortDelta < 0.0f && std::abs(shortDelta) < std::abs(visualSlot))
            return m_scrollPos + shortDelta;
    }

    return m_scrollPos + visualSlot;
}

// ============================================================================
// Flip3DCompApp::IsSelectionAtCarouselFront
// Exit scroll completes when the selection is list index 0 and scroll settled.
// ============================================================================
bool Flip3DCompApp::IsSelectionAtCarouselFront() const
{
    if (!m_selectedHwnd || m_cards.empty())
        return true;

    if (m_cards[0].m_hwnd != m_selectedHwnd)
        return false;

    return std::abs(m_scrollPos) < 0.05f
        && std::abs(m_scrollTarget - m_scrollPos) < 0.05f;
}

// ============================================================================
// Flip3DCompApp::StepCarouselScroll
// Shared smooth scroll + wrap + front-edge fade (browse and exit).
// ============================================================================
void Flip3DCompApp::StepCarouselScroll(float dtSeconds, bool notifyFrontChange)
{
    const float prevFront = std::floor(m_scrollPos + 0.5f);
    const float prevScroll = m_scrollPos;
    const float scrollSmoothTime = kScrollSmoothTimeSec.load(
        std::memory_order_relaxed);
    const float smoothTime = (m_touchActive || m_scrollSnapActive
        || m_touchScrollVelocity != 0.0f)
        ? std::max(kTouchScrollMinSmoothTimeSec,
            scrollSmoothTime
            / (1.0f + std::abs(m_touchScrollVelocity)
                * kTouchScrollSpeedResponse))
        : scrollSmoothTime;
    if (kCanHasEasing.load(std::memory_order_relaxed))
    {
        const float a = 1.0f - std::exp(
            -dtSeconds / std::max(smoothTime, 1e-4f));
        m_scrollPos += (m_scrollTarget - m_scrollPos) * a;
    }
    else
    {
        const float maxDistance = dtSeconds
            / std::max(smoothTime, 1e-4f);
        const float distance = m_scrollTarget - m_scrollPos;
        if (std::abs(distance) <= maxDistance)
            m_scrollPos = m_scrollTarget;
        else
            m_scrollPos += std::copysign(maxDistance, distance);
    }
    const float smoothDelta = m_scrollPos - prevScroll;
    WrapCarouselScroll();
    AdvanceWrapDisplaySlots(smoothDelta);

    SettleWrapDisplaySlots();

    // If scroll has essentially settled to the target, snap exactly to the
    // target to avoid accumulating tiny fractional drift across repeated
    // discrete moves. This keeps which card is front unambiguous.
    if (std::abs(m_scrollTarget - m_scrollPos) < kScrollSettleEpsilon)
    {
        m_scrollPos = m_scrollTarget;
        if (!m_touchActive && m_scrollSnapActive)
        {
            m_scrollSnapActive = false;
            m_touchScrollVelocity = 0.0f;
        }
    }

    if (notifyFrontChange)
    {
        const float newFront = std::floor(m_scrollPos + 0.5f);
        if (newFront != prevFront)
            NotifyAccessibilityFocusFront();
    }
}

// ============================================================================
// Flip3DCompApp::RotateToWindow
// ============================================================================
void Flip3DCompApp::RotateToWindow(HWND targetHwnd)
{
    if (m_cards.size() <= 1 || m_state == ViewState::Exit
        || m_state == ViewState::ExitRepeatedRotate)
        return;

    int idx = -1;
    for (int i = 0; i < (int)m_cards.size(); ++i)
    {
        if (m_cards[(size_t)i].m_hwnd == targetHwnd) { idx = i; break; }
    }
    if (idx < 0)
        return;

    m_scrollTarget = ComputeScrollTargetForCard(idx);
}

// ============================================================================
// Flip3DCompApp::TickSmoothScroll
// ============================================================================
void Flip3DCompApp::TickSmoothScroll(float dtSeconds)
{
    if (m_cards.size() <= 1 || m_state != ViewState::Interactive)
        return;

    // A delayed activation or key message is not evidence that the key is
    // still held. Repeat follows the input thread's current state.
    if (m_heldNavigationKey && !g_navigationKeysHeld[m_heldNavigationKey].load())
        OnKey(false, m_heldNavigationKey, 0);

    const auto now = std::chrono::steady_clock::now();
    const float heldTime = std::chrono::duration<float>(now - m_heldNavigationStart).count();
    if (m_heldNavigationDirection != 0 && heldTime >= kHeldKeyStartDelaySec)
    {
        m_scrollTarget += (float)m_heldNavigationDirection
            * kHeldKeyRotateSpeed * dtSeconds;
    }

    if (m_penHoverScrollDirection != 0)
    {
        m_scrollTarget += (float)m_penHoverScrollDirection
            * kPenHoverScrollSpeed * dtSeconds;
    }

    if (!m_touchActive && !m_scrollSnapActive && m_touchScrollVelocity != 0.0f
        && !kCanHasEasing.load(std::memory_order_relaxed))
    {
        m_touchScrollVelocity = 0.0f;
        m_scrollTarget = std::round(m_scrollTarget);
        m_scrollSnapActive = true;
    }

    if (!m_touchActive && !m_scrollSnapActive && m_touchScrollVelocity != 0.0f)
    {
        m_scrollTarget += m_touchScrollVelocity * dtSeconds;
        m_touchScrollVelocity *= std::exp(-kTouchSwipeFriction * dtSeconds);
        if (std::abs(m_touchScrollVelocity) < kTouchFlingStopVelocity)
        {
            m_touchScrollVelocity = 0.0f;
            m_scrollTarget = std::round(m_scrollTarget);
            m_scrollSnapActive = true;
        }
    }

    if (m_wheelPendingSlots != 0)
    {
        const auto now = std::chrono::steady_clock::now();
        const float sinceWheel = std::chrono::duration<float>(now - m_lastWheelTime).count();
        if (sinceWheel > kWheelActiveTimeoutSec)
        {
            m_wheelPendingSlots = 0;
        }
        else
        {
            const float sinceKey = std::chrono::duration<float>(now - m_lastKeyProcessed).count();
            if (sinceKey >= kKeyRepeatIntervalSec)
            {
                const int step = (m_wheelPendingSlots > 0) ? 1 : -1;
                RotateBy(step);
                m_wheelPendingSlots -= step;
                m_lastKeyProcessed = now;
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
    if (m_thumbnailsDirty)
        OnThumbnailSourceSizeChanged();

    float animationRate = std::max(
        kAnimationRate.load(std::memory_order_relaxed), 0.0f);
    if (IsDwmShiftAnimationSlowdownActive())
        animationRate *= kShiftAnimationRate.load(std::memory_order_relaxed);
    dtSeconds *= animationRate;

    m_animEnter.Update(dtSeconds);

    if (m_rotateTimeline.IsActive())
    {
        m_rotateTimeline.Update(dtSeconds);
        OnRotationTimeUpdated();
    }

    if (!m_rotateTimeline.IsActive())
        TickRepeatedRotate();

    const float enterProgress = EnterProgress();
    UpdateVisualSlots(enterProgress);

    TickSmoothScroll(dtSeconds);

    const float washProgress = std::clamp(m_animEnter.LinearValue(), 0.0f, 1.0f);

    if (m_sceneVisual)
        m_sceneVisual->SetOpacity(1.0f);
    for (auto& mon : m_monitorBackdrops)
    {
        if (mon.washVisual)
            mon.washVisual->SetOpacity(washProgress * kDesktopWashOpacityScale);
        if (mon.shellContainer)
            mon.shellContainer->SetOpacity(1.0f);
    }

    if (m_state == ViewState::Enter && !m_animEnter.IsActive())
    {
        m_state = ViewState::Interactive;
        InvalidateDisplaySlots();
        NotifyAccessibilityEvent(EVENT_SYSTEM_DIALOGSTART);
        NotifyAccessibilityFocusFront();
    }

    if (m_openingTabPending && m_state == ViewState::Interactive)
    {
        const auto now = std::chrono::steady_clock::now();
        const bool tabStillDown = g_navigationKeysHeld[VK_TAB].load();
        const float pendingTime =
            std::chrono::duration<float>(now - m_openingTabStart).count();
        if (!tabStillDown)
        {
            m_openingTabPending = false;
        }
        else if (pendingTime >= kOpeningTabDelaySec)
        {
            m_openingTabPending = false;
            OnKey(true, VK_TAB, 0);
            m_heldNavigationStart = now
                - std::chrono::duration_cast<std::chrono::steady_clock::duration>(
                    std::chrono::duration<float>(kHeldKeyStartDelaySec));
        }
    }

    UpdateCamera(enterProgress);
    UpdateCards(enterProgress, dtSeconds);

    if (m_dcompDevice)
    {
        m_dcompDevice->Commit();
    }

    const auto finishProcess = [this]()
        {
            if (m_pendingActivateHwnd && IsWindow(m_pendingActivateHwnd))
            {
                const HWND target = m_pendingActivateHwnd;
                m_pendingActivateHwnd = nullptr;
                if (target == GetShellWindow())
                {
                    SetCardTransitionsDisabled(true);
                    bool needShellMinimizeAll = false;
                    for (const CardModel& card : m_cards)
                    {
                        if (card.m_hwnd && card.m_hwnd != target
                            && IsWindow(card.m_hwnd) && !IsIconic(card.m_hwnd))
                        {
                            if (!ShowWindowAsync(card.m_hwnd, SW_SHOWMINNOACTIVE))
                                needShellMinimizeAll = true;
                        }
                    }
                    if (needShellMinimizeAll)
                    {
                        // Elevated windows (e.g. Task Manager) reject messages
                        // from this process; let the shell minimize everything.
                        HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
                        if (tray)
                            PostMessageW(tray, WM_COMMAND, 419, 0);
                    }
                }
                if (!IsWindowEnabled(target))
                    SwitchToThisWindow(
                        GetLastActivePopup(GetAncestor(target, GA_ROOTOWNER)), TRUE);
                else
                    SwitchToThisWindow(target, TRUE);
            }
            if (!m_hwnd || !IsWindow(m_hwnd) || !DestroyWindow(m_hwnd))
                PostQuitMessage(0);
        };

    if (m_state == ViewState::Exit && !m_animEnter.IsActive())
        finishProcess();

    if (m_state == ViewState::ExitRepeatedRotate
        && !m_animEnter.IsActive()
        && !m_rotateTimeline.IsActive()
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
    auto view = BuildViewMatrix(enterProgress);
    auto proj = BuildProjMatrix(1.0f, enterProgress);
    auto viewport = Math::BuildViewportMatrix(m_monW, m_monH, m_viewX, m_viewY);
    return Math::Multiply(Math::Multiply(view, proj), viewport);
}

// ============================================================================
// Flip3DCompApp::UpdateCamera
// Scene root stays identity. Each card bakes Model × View × Proj × VP on its
// container visual (same as the pre-SPATIAL single-matrix path).
// ============================================================================
void Flip3DCompApp::UpdateCamera(float /*enterProgress*/)
{
    if (!m_sceneVisual)
        return;

    Wh_Log(L"Scene visual uses its default identity transform");
}

float Flip3DCompApp::ComputeFlatDepthRank(float slot, float enterProgress,
    int listIndex) const
{
    const float p = enterProgress;

    if (m_state == ViewState::Exit || m_state == ViewState::ExitRepeatedRotate)
        return slot;

    if (p < 1.0f - 1e-5f)
    {
        if (listIndex >= 0 && listIndex < (int)m_cards.size())
        {
            const CardModel& c = m_cards[(size_t)listIndex];
            if (m_selectedHwnd && c.m_hwnd == m_selectedHwnd)
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
            if (k >= 0 && k < (int)m_cards.size())
            {
                const CardModel& c = m_cards[(size_t)k];
                if (m_selectedHwnd && c.m_hwnd == m_selectedHwnd)
                    return -1.0f;
                return (float)c.m_initialCarouselIndex;
            }
            return (float)listIndex;
        };

    if (m_state == ViewState::Exit || m_state == ViewState::ExitRepeatedRotate)
        return slot;

    if (m_state == ViewState::Enter)
        return p * slot + (1.0f - p) * flatRank(listIndex);

    return slot;
}

// ============================================================================
// Flip3DCompApp::UpdateCards
// ============================================================================
void Flip3DCompApp::UpdateCards(float enterProgress, float dtSeconds)
{
    if (!m_sceneVisual)
        return;

    const int N = (int)m_cards.size();
    if (N == 0)
        return;

    const float p = enterProgress;
    const auto  camera = BuildCameraMatrix(p);
    const int   maxVis = kMaxVisibleCards;

    const bool scrollPath = (m_state == ViewState::Interactive);
    const bool exitRotate = (m_state == ViewState::ExitRepeatedRotate);
    if (scrollPath)
        SyncDisplaySlotsToList();

    struct CardDraw { CardModel* card; float slot; float opacity; float key; int listIndex; };
    std::vector<CardDraw> draws;
    draws.reserve((size_t)N);

    for (int k = 0; k < N; ++k)
    {
        CardModel& c = m_cards[(size_t)k];
        const bool traceFirstFrame = m_state == ViewState::Enter
            && m_lastPaintOrder.empty();
        if (traceFirstFrame)
            Wh_Log(L"First-frame card %d/%d hwnd=%p: begin",
                k + 1, N, c.m_hwnd);
        const float hoverScaleTarget = c.m_hwnd == m_penHoveredHwnd
            ? kPenHoveredCardScale
            : 1.0f;
        if (kCanHasEasing)
        {
            const float scaleEase = 1.0f - std::exp(
                -dtSeconds / std::max(kPenHoverScaleSmoothTimeSec, 1e-4f));
            c.m_penHoverScale += (hoverScaleTarget - c.m_penHoverScale) * scaleEase;
            if (std::abs(hoverScaleTarget - c.m_penHoverScale) < 0.001f)
                c.m_penHoverScale = hoverScaleTarget;
        }
        else
        {
            c.m_penHoverScale = hoverScaleTarget;
        }

        if (!c.m_containerVisual)
            continue;

        const float slot = GetCardDisplaySlot(k);
        float opacity = ComputeUpdateAlpha(c, p, slot);
        if (traceFirstFrame)
            Wh_Log(L"First-frame card %d: slot=%f opacity=%f",
                k + 1, slot, opacity);

        // A short carousel can introduce a back card without entering one of
        // the explicit wrap phases. Detect the transition from invisible to
        // visible and start the same fade used by wrapped cards.
        if (!c.m_opacityInitialized)
        {
            c.m_wrapOpacity = opacity;
            c.m_lastTargetOpacity = opacity;
            c.m_opacityInitialized = true;
        }
        else if (!c.m_wrapFadeActive
            && c.m_lastTargetOpacity <= 0.01f
            && opacity > 0.01f)
        {
            c.m_wrapOpacity = 0.0f;
            c.m_wrapFadeActive = true;
        }
        c.m_lastTargetOpacity = opacity;

        const float frontCull = exitRotate ? -2.0f
            : (m_state == ViewState::Exit) ? -1.5f
            : -1.0f;

        if (scrollPath)
        {
            const bool wrapDecoupled = (c.m_wrapPhase == CarouselWrapPhase::ExitingFront
                || c.m_wrapPhase == CarouselWrapPhase::EnteringFront);
            const float span = CarouselEdgeSpan();
            if (!wrapDecoupled && slot >= (float)maxVis)
            {
                if (m_state == ViewState::Enter && m_lastPaintOrder.empty())
                    Wh_Log(L"First-frame card %d: hidden at max visible slot", k + 1);
                c.m_containerVisual->SetVisible(FALSE);
                continue;
            }
            if (wrapDecoupled && (slot < -1.5f || slot > span + 0.5f))
            {
                c.m_containerVisual->SetVisible(FALSE);
                continue;
            }
            if (opacity < 0.01f && !c.m_wrapFadeActive)
            {
                if (m_state == ViewState::Enter && m_lastPaintOrder.empty())
                    Wh_Log(L"First-frame card %d: hidden at zero opacity", k + 1);
                c.m_containerVisual->SetVisible(FALSE);
                continue;
            }
        }
        else if (exitRotate)
        {
            if (!ShouldDrawRotationCard(k) && opacity < 0.01f)
            {
                c.m_containerVisual->SetVisible(FALSE);
                continue;
            }
            if (opacity < 0.01f)
            {
                c.m_containerVisual->SetVisible(FALSE);
                continue;
            }
        }
        else if (slot >= (float)maxVis
            || (slot <= frontCull && opacity < 0.01f))
        {
            if (traceFirstFrame)
                Wh_Log(L"First-frame card %d: culled", k + 1);
            c.m_containerVisual->SetVisible(FALSE);
            continue;
        }

        const float key = ComputePaintKey(slot, p, k);
        draws.push_back({ &c, slot, opacity, key, k });
    }

    for (int k = 0; k < N; ++k)
    {
        CardModel& c = m_cards[(size_t)k];
        if (!c.m_containerVisual)
            continue;
        bool listed = false;
        for (const auto& d : draws)
            if (d.card == &c) { listed = true; break; }
        if (!listed)
            c.m_containerVisual->SetVisible(FALSE);
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
        paintOrder.push_back(d.card->m_hwnd);

    const bool orderChanged = (paintOrder != m_lastPaintOrder);
    if (orderChanged)
    {
        for (auto& d : draws)
        {
            if (m_state == ViewState::Enter)
                Wh_Log(L"First-frame: removing visual hwnd=%p", d.card->m_hwnd);
            const HRESULT hr = m_sceneVisual->RemoveVisual(
                d.card->m_containerVisual.Get());
            if (m_state == ViewState::Enter)
                Wh_Log(L"First-frame: removed visual hwnd=%p hr=0x%08X",
                    d.card->m_hwnd, static_cast<unsigned>(hr));
            if (FAILED(hr))
                Wh_Log(L"RemoveVisual failed for card hwnd=%p hr=0x%08X",
                    d.card->m_hwnd, static_cast<unsigned>(hr));
        }
        m_lastPaintOrder = std::move(paintOrder);
    }

    IDCompositionVisual* prevVis = nullptr;
    for (auto& d : draws)
    {
        HRESULT hr = S_OK;
        if (kEnableBackCardOpacityEasing && d.card->m_wrapFadeActive)
        {
            const float fade = 1.0f - std::exp(
                -dtSeconds / std::max(kBackCardFadeTimeSec, 1e-4f));
            d.card->m_wrapOpacity +=
                (d.opacity - d.card->m_wrapOpacity) * fade;
            if (std::abs(d.card->m_wrapOpacity - d.opacity) < 0.002f)
            {
                d.card->m_wrapOpacity = d.opacity;
                d.card->m_wrapFadeActive = false;
            }
        }
        else
        {
            d.card->m_wrapOpacity = d.opacity;
            d.card->m_wrapFadeActive = false;
        }
        const float wrapOpacity = std::clamp(d.card->m_wrapOpacity, 0.0f, 1.0f);
        hr = d.card->m_containerVisual->SetVisible(wrapOpacity > 0.0f);
        if (FAILED(hr))
            Wh_Log(L"SetVisible failed for card hwnd=%p hr=0x%08X",
                d.card->m_hwnd, static_cast<unsigned>(hr));

        const float t = ComputeCarouselBezierT(d.slot);
        const float flatRank = ComputeFlatDepthRank(d.slot, p, d.listIndex);
        auto model = BuildModelMatrix(*d.card, t, p, flatRank);
        const Matrix4x4 transform = Math::Multiply(model, camera);
        hr = d.card->m_containerVisual->SetTransform(transform);
        if (FAILED(hr))
            Wh_Log(L"Card SetTransform failed for hwnd=%p hr=0x%08X",
                d.card->m_hwnd, static_cast<unsigned>(hr));

        if (wrapOpacity > 0.0f)
        {
            hr = d.card->m_containerVisual->SetOpacity(wrapOpacity);
            if (FAILED(hr))
                Wh_Log(L"SetOpacity failed for card hwnd=%p hr=0x%08X",
                    d.card->m_hwnd, static_cast<unsigned>(hr));
        }

        if (orderChanged)
        {
            if (m_state == ViewState::Enter)
                Wh_Log(L"First-frame: adding visual hwnd=%p", d.card->m_hwnd);
            if (prevVis)
                hr = m_sceneVisual->AddVisual(
                    d.card->m_containerVisual.Get(), TRUE, prevVis);
            else
                hr = m_sceneVisual->AddVisual(
                    d.card->m_containerVisual.Get(), FALSE, nullptr);
            if (FAILED(hr))
                Wh_Log(L"AddVisual failed for card hwnd=%p hr=0x%08X",
                    d.card->m_hwnd, static_cast<unsigned>(hr));
            if (m_state == ViewState::Enter)
                Wh_Log(L"First-frame: AddVisual hwnd=%p hr=0x%08X",
                    d.card->m_hwnd, static_cast<unsigned>(hr));
            prevVis = d.card->m_containerVisual.Get();
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
            (float)(kMaxVisibleCards - (int)m_cards.size()) / countRange,
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
    if (m_state == ViewState::Interactive
        && card.m_wrapPhase == CarouselWrapPhase::EnteringBack)
    {
        opacitySlot = card.m_displaySlot;
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
    if (card.m_wrapPhase == CarouselWrapPhase::EnteringBack
        || card.m_wrapPhase == CarouselWrapPhase::EnteringFront)
    {
        const float t = std::clamp(card.m_wrapProgress, 0.0f, 1.0f);
        rotationOpacity = (1.0f - t) * kBackCardMinOpacity + t * rotationOpacity;
    }
    float enterScale = 1.0f;

    const bool exitRotateFlattenTail =
        m_state == ViewState::ExitRepeatedRotate
        && !m_rotateTimeline.IsActive()
        && !m_cards.empty()
        && m_cards[0].m_hwnd == m_selectedHwnd;

    if ((m_state == ViewState::Exit || exitRotateFlattenTail)
        && (int)m_cards.size() > kMaxVisibleCards
        && carouselSlot >= (float)kMaxVisibleCards - 1.0f)
    {
        rotationOpacity = kDesktopWashOpacityScale;
    }

    if (m_state == ViewState::ExitRepeatedRotate && m_rotateTimeline.IsActive())
    {
        const int idx = FindCardIndex(card.m_hwnd);
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
// uDWM UpdateModels: m_cWindows = max(count, 5), capped by max visible.
// ============================================================================
float Flip3DCompApp::ComputeCarouselPathDenom() const
{
    const int N = (int)m_cards.size();
    return (float)std::min(std::max(N, 5), kMaxVisibleCards);
}

// ============================================================================
// Flip3DCompApp::ComputeCarouselBezierT
// flip3d/uDWM: zIndex = -pathRelative; t = 1 - (zIndex - 0.5) / -m_cWindows
// (pathRelative == carousel slot: 0 = front, larger = further back)
// ============================================================================
float Flip3DCompApp::ComputeCarouselBezierT(float slot) const
{
    const float denom = ComputeCarouselPathDenom();
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

    float yaw = kCameraFinalRotateY * p;
    float pitch = kCameraFinalRotateX * p;
    float roll = kCameraFinalRotateZ * p;

    auto T1 = Translation(0, 0, 1);
    auto Rot = RotationRollPitchYaw(pitch, yaw, roll);
    auto T2 = Translation(0, 0, -1);
    auto T3 = Translation(-kCameraFinalTranslateX * p,
        -kCameraFinalTranslateY * p,
        -kCameraFinalTranslateZ * p);
    auto LA = LookAtRH({ 0, 0, 1 }, { 0, 0, 0 }, { 0, 1, 0 });

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

    Vec3  cv = Bezier(t);
    float fullW = std::abs(c.m_targetSize.x);
    float fullH = std::abs(c.m_targetSize.y);

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
        tx = c.m_flatPos.x;
        ty = c.m_flatPos.y;
        tz = flatZ;
        worldW = c.m_flatSize.x;
        worldH = c.m_flatSize.y;
    }
    else
    {
        Vec3 pos2d = { c.m_flatPos.x, c.m_flatPos.y, flatZ };
        tx = Lerp(pos2d.x, pos3d.x, p);
        ty = Lerp(pos2d.y, pos3d.y, p);
        tz = Lerp(pos2d.z, pos3d.z, p);
        worldW = Lerp(c.m_flatSize.x, fullW, p);
        worldH = Lerp(c.m_flatSize.y, fullH, p);
    }

    if (c.m_penHoverScale != 1.0f)
    {
        const float hoveredW = worldW * c.m_penHoverScale;
        const float hoveredH = worldH * c.m_penHoverScale;
        tx -= (hoveredW - worldW) * 0.5f;
        ty += (hoveredH - worldH) * 0.5f;
        worldW = hoveredW;
        worldH = hoveredH;
    }

    float sx = worldW / (float)std::max(c.m_srcWidth, 1);
    float sy = -worldH / (float)std::max(c.m_srcHeight, 1);

    auto S = Scale(sx, sy, 1.0f);
    auto T = Translation(tx, ty, tz);
    auto M = Multiply(S, T);

    if (m_rtl)
    {
        auto Mirror = Multiply(
            Scale(-1.0f, 1.0f, 1.0f),
            Translation((float)c.m_srcWidth, 0.0f, 0.0f));
        M = Multiply(Mirror, M);
    }

    return M;
}

// ============================================================================
// Flip3DComp_Rotation.cpp — uDWM-style discrete list rotation during exit
// ============================================================================

#include <algorithm>
#include <cmath>

namespace
{
    struct RotationContext
    {
        int   countInt = 0;
        int   visibleCount = 0;
        bool  hasHidden = false;
        bool  isRotating = false;
        int   rotationSteps = 0;   // +1 front→back, -1 back→front, 0 idle
        float rotationProgress = 1.0f;
        float hiddenFlipOut = 1.0f;
        float hiddenFlipIn = 0.0f;
        int   rotatingOutIndex = -1;
        int   rotatingInIndex = -1;
        int   flipCardIndex = -1;
        bool  useHiddenRotation = false;
        bool  showOutgoing = false;
    };

    struct CardRotationEntry
    {
        int  listIndex = 0;
        int  startSlot = 0;
        int  endSlot = 0;
        bool isOutgoing = false;
        bool isIncoming = false;
        bool isBoundaryIn = false;
        bool isCycle = false;
        bool shouldDraw = false;
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
        ctx.hasHidden = ctx.countInt > ctx.visibleCount;
        ctx.isRotating = isRotating;
        ctx.rotationSteps = ctx.isRotating ? (rotateBackward ? -1 : 1) : 0;
        ctx.rotationProgress = rotationProgress;
        ctx.hiddenFlipOut = std::clamp(ctx.rotationProgress * 2.0f, 0.0f, 1.0f);
        ctx.hiddenFlipIn = std::clamp((ctx.rotationProgress - 0.5f) * 2.0f, 0.0f, 1.0f);
        ctx.showOutgoing = showOutgoing;

        if (ctx.rotationSteps != 0)
        {
            if (ctx.hasHidden)
            {
                if (rotateBackward)
                {
                    ctx.rotatingOutIndex = ctx.visibleCount;
                    ctx.rotatingInIndex = 0;
                    ctx.flipCardIndex = ctx.rotatingInIndex;
                }
                else
                {
                    ctx.rotatingOutIndex = ctx.countInt - 1;
                    ctx.rotatingInIndex = ctx.visibleCount - 1;
                    ctx.flipCardIndex = ctx.rotatingOutIndex;
                }
            }
            else
            {
                const int cycleIndex = rotateBackward ? 0 : (ctx.countInt - 1);
                ctx.rotatingInIndex = cycleIndex;
                ctx.rotatingOutIndex = cycleIndex;
                ctx.flipCardIndex = cycleIndex;
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
        entry.endSlot = listIndex;

        const int n = ctx.countInt;
        const int vis = ctx.visibleCount;
        entry.startSlot = listIndex;
        if (ctx.rotationSteps != 0)
        {
            if (ctx.rotationSteps < 0)
                entry.startSlot = (listIndex + n - 1) % n;
            else
                entry.startSlot = (listIndex + 1) % n;
        }

        const bool isVis = entry.endSlot < vis;
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
            const float end = (ctx.rotationSteps > 0) ? -1.0f : (float)ctx.visibleCount;
            return start + (end - start) * p;
        }

        if (entry.isBoundaryIn || entry.isIncoming)
        {
            const float start = (ctx.rotationSteps > 0) ? (float)ctx.visibleCount : -1.0f;
            const float end = (float)entry.endSlot;
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
        const float endSteady = SteadyOpacityForRelative(ctx, (float)entry.endSlot);

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
    const size_t count = m_cards.size();
    if (count <= 1)
        return 0;

    size_t dist = 0;
    size_t cur = sourcePos;
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
    if (m_state == ViewState::ExitRepeatedRotate)
        return std::max(std::abs(m_rRepeatedRotateRate), 0.005f);
    return kRotateListDurationSec;
}

void Flip3DCompApp::StartRotationStep(bool backward, float durationSec)
{
    m_rotateBackward = backward;
    m_showOutgoingDuringRotation = true;
    m_rotateTimeline.Restart(0.0f, 1.0f, durationSec, InterpolationMode::Linear);
}

void Flip3DCompApp::OnRotationTimeUpdated()
{
    if (m_showOutgoingDuringRotation && m_rotateTimeline.IsActive()
        && m_rotateTimeline.RawProgress() > 0.5f)
    {
        m_showOutgoingDuringRotation = false;
    }
}

void Flip3DCompApp::TickRepeatedRotate()
{
    if (m_rotateTimeline.IsActive() || m_cards.empty())
        return;

    if (m_state == ViewState::ExitRepeatedRotate)
    {
        if (m_repeatedRotateStepsRemaining > 0)
        {
            constexpr bool backward = false;
            RotateListPhysically(backward);
            --m_repeatedRotateStepsRemaining;
            StartRotationStep(backward, RotationDurationForRotateList());

            for (int k = 0; k < (int)m_cards.size(); ++k)
            {
                m_cards[(size_t)k].m_displaySlot =
                    ComputeRotationDisplaySlot(k);
            }
        }
        else
        {
            m_rRepeatedRotateRate = 0.0f;
            m_exitScrollSnapshot = 0.0f;
            m_scrollPos = 0.0f;
            m_scrollTarget = 0.0f;
        }
        return;
    }
}

float Flip3DCompApp::ComputeRotationDisplaySlot(int listIndex) const
{
    const bool isRotating = m_rotateTimeline.IsActive();
    const RotationContext ctx = BuildRotationContext(
        (int)m_cards.size(), isRotating, m_rotateBackward,
        isRotating ? m_rotateTimeline.RawProgress() : 1.0f,
        m_showOutgoingDuringRotation);
    if (!ctx.isRotating)
        return CardListSlot(listIndex);

    const CardRotationEntry entry = BuildCardRotationEntry(listIndex, ctx);
    const float scrollOffset = (m_state == ViewState::ExitRepeatedRotate)
        ? m_exitScrollSnapshot
        : m_scrollPos;
    return ResolveRotationPathSlot(entry, ctx) - scrollOffset;
}

bool Flip3DCompApp::ShouldDrawRotationCard(int listIndex) const
{
    const bool isRotating = m_rotateTimeline.IsActive();
    const RotationContext ctx = BuildRotationContext(
        (int)m_cards.size(), isRotating, m_rotateBackward,
        isRotating ? m_rotateTimeline.RawProgress() : 1.0f,
        m_showOutgoingDuringRotation);
    if (!ctx.isRotating)
        return listIndex < ctx.visibleCount;

    const CardRotationEntry entry = BuildCardRotationEntry(listIndex, ctx);
    return entry.shouldDraw;
}

float Flip3DCompApp::ComputeRotationAlpha(const CardModel& card, float enterProgress,
    int listIndex, float carouselSlot) const
{
    const bool isRotating = m_rotateTimeline.IsActive();
    const RotationContext ctx = BuildRotationContext(
        (int)m_cards.size(), isRotating, m_rotateBackward,
        isRotating ? m_rotateTimeline.RawProgress() : 1.0f,
        m_showOutgoingDuringRotation);
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

// ============================================================================
// Flip3DComp_Input.cpp — Keyboard, mouse, and wheel input handling
// ============================================================================
#include <windowsx.h>
#include <algorithm>
#include <cmath>

// ============================================================================
// Flip3DCompApp::OnWheel
// Modern smooth scroll: each WHEEL_DELTA notch nudges the scroll target by one
// slot. Wheel down (delta < 0) scrolls front→back; wheel up scrolls back→front.
// ============================================================================
bool Flip3DCompApp::OnWheel(int wheelDelta)
{
    if (wheelDelta == 0 ||
        m_state == ViewState::Exit ||
        m_state == ViewState::ExitRepeatedRotate)
        return false;

    const float scaled = (float)wheelDelta / (float)WHEEL_DELTA * kScrollWheelNotchFraction;
    int deltaSlots = (int)std::round(scaled);
    if (deltaSlots == 0 && wheelDelta != 0)
        deltaSlots = (wheelDelta > 0) ? 1 : -1;
    m_wheelPendingSlots += deltaSlots;
    m_lastWheelTime = std::chrono::steady_clock::now();
    return true;
}

// ============================================================================
// Flip3DCompApp::OnKey
// ============================================================================
bool Flip3DCompApp::OnKey(bool down, UINT vkCode, LPARAM lParam)
{
    const bool isNavigationKey = vkCode == VK_TAB
        || vkCode == VK_LEFT || vkCode == VK_RIGHT
        || vkCode == VK_UP || vkCode == VK_DOWN;

    if (!down)
    {
        if (isNavigationKey)
            m_navigationKeyDown[vkCode] = false;
        if (vkCode == VK_TAB)
            m_openingTabPending = false;
        if (vkCode == m_heldNavigationKey)
        {
            m_heldNavigationKey = 0;
            m_heldNavigationDirection = 0;
            m_heldNavigationStart = {};
            m_scrollTarget = std::round(m_scrollTarget);
        }
        return false;
    }

    if (m_state == ViewState::Exit ||
        m_state == ViewState::ExitRepeatedRotate)
        return false;

    // Detect Windows autorepeat: bit 30 of lParam is set if the key was
    // previously down. Throttle repeated keydown processing so holding an
    // arrow or tab key doesn't rotate the carousel too quickly.
    const bool isRepeat = (lParam & (1 << 30)) != 0;
    if (isRepeat)
        return true; // held navigation is driven from the frame update

    // The low-level hook and Raw Input can both forward the same physical
    // key-down. Accept only the first fresh down edge for each navigation key.
    if (isNavigationKey)
    {
        if (m_navigationKeyDown[vkCode])
            return true;
        m_navigationKeyDown[vkCode] = true;
    }

    switch (vkCode)
    {
    case VK_ESCAPE:
        ExitView();
        return true;

    case VK_TAB:
    {
        const int direction = (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? -1 : 1;
        m_heldNavigationKey = vkCode;
        m_heldNavigationDirection = direction;
        m_heldNavigationStart = std::chrono::steady_clock::now();
        RotateBy(direction);
        return true;
    }

    case VK_UP:
        m_heldNavigationKey = vkCode;
        m_heldNavigationDirection = -1;
        m_heldNavigationStart = std::chrono::steady_clock::now();
        RotateBy(-1);
        return true;

    case VK_DOWN:
        m_heldNavigationKey = vkCode;
        m_heldNavigationDirection = 1;
        m_heldNavigationStart = std::chrono::steady_clock::now();
        RotateBy(1);
        return true;

    case VK_LEFT:
        m_heldNavigationKey = vkCode;
        m_heldNavigationDirection = m_rtl ? 1 : -1;
        m_heldNavigationStart = std::chrono::steady_clock::now();
        RotateBy(m_rtl ? 1 : -1);
        return true;

    case VK_RIGHT:
        m_heldNavigationKey = vkCode;
        m_heldNavigationDirection = m_rtl ? -1 : 1;
        m_heldNavigationStart = std::chrono::steady_clock::now();
        RotateBy(m_rtl ? -1 : 1);
        return true;

    case VK_HOME:
        RotateToWindow(m_originalFrontHwnd);
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
        m_state == ViewState::Exit ||
        m_state == ViewState::ExitRepeatedRotate)
        return false;

    HWND hit = HitTest3DScene(x, y);
    if (hit)
        SelectWindow(hit);
    else
        ExitView();

    return true;
}

// ============================================================================
// Flip3DCompApp::OnPenPointer
// ============================================================================
void Flip3DCompApp::SetPenHoverScrollDirection(int direction)
{
    if (direction == m_penHoverScrollDirection)
        return;

    if (direction == 0 && m_penHoverScrollDirection != 0)
    {
        m_scrollTarget = std::round(m_scrollTarget);
        m_scrollSnapActive = true;
    }
    else if (direction != 0)
    {
        m_scrollSnapActive = false;
        m_touchScrollVelocity = 0.0f;
    }

    m_penHoverScrollDirection = direction;
}

bool Flip3DCompApp::OnPenPointer(UINT msg, WPARAM wParam)
{
    const UINT pointerId = GET_POINTERID_WPARAM(wParam);
    if (msg == WM_POINTERLEAVE)
    {
        if (pointerId != m_penPointerId)
            return false;

        m_penPointerId = 0;
        m_penHoveredHwnd = nullptr;
        SetPenHoverScrollDirection(0);
        return true;
    }

    POINTER_INPUT_TYPE pointerType = PT_POINTER;
    if (!GetPointerType(pointerId, &pointerType) || pointerType != PT_PEN)
        return false;

    POINTER_INFO pointerInfo = {};
    if (!GetPointerInfo(pointerId, &pointerInfo))
        return false;

    POINT point = pointerInfo.ptPixelLocation;
    ScreenToClient(m_hwnd, &point);
    const bool inContact = (pointerInfo.pointerFlags & POINTER_FLAG_INCONTACT) != 0;

    if (msg == WM_POINTERDOWN)
    {
        m_penPointerId = pointerId;
        m_penHoveredHwnd = nullptr;
        SetPenHoverScrollDirection(0);
        OnTouchContact(pointerId, TOUCHEVENTF_DOWN, point, pointerInfo.dwTime);
        return true;
    }

    if (msg == WM_POINTERUPDATE && inContact)
    {
        m_penPointerId = pointerId;
        m_penHoveredHwnd = nullptr;
        SetPenHoverScrollDirection(0);
        OnTouchContact(pointerId, TOUCHEVENTF_MOVE, point, pointerInfo.dwTime);
        return true;
    }

    if (msg == WM_POINTERUP)
    {
        if (m_touchActive && pointerId == m_touchId)
        {
            OnTouchContact(pointerId, TOUCHEVENTF_MOVE | TOUCHEVENTF_UP,
                point, pointerInfo.dwTime);
        }
        m_penPointerId = 0;
        m_penHoveredHwnd = nullptr;
        SetPenHoverScrollDirection(0);
        return true;
    }

    if (!(pointerInfo.pointerFlags & POINTER_FLAG_INRANGE) || inContact)
    {
        if (pointerId == m_penPointerId)
        {
            m_penPointerId = 0;
            m_penHoveredHwnd = nullptr;
            SetPenHoverScrollDirection(0);
        }
        return false;
    }

    m_penPointerId = pointerId;
    m_penHoveredHwnd = HitTest3DScene(point.x, point.y);

    const LONG zone = (LONG)std::min<UINT>(
        kPenHoverScrollZonePx, std::min(m_width, m_height) / 4);
    const bool topLeft = point.x >= 0 && point.y >= 0
        && point.x < zone && point.y < zone;
    const bool bottomRight = point.x >= (LONG)m_width - zone
        && point.y >= (LONG)m_height - zone
        && point.x < (LONG)m_width && point.y < (LONG)m_height;
    SetPenHoverScrollDirection(topLeft ? 1 : bottomRight ? -1 : 0);
    return true;
}

// ============================================================================
// Flip3DCompApp::OnTouchContact
// ============================================================================
void Flip3DCompApp::OnTouchContact(UINT id, DWORD flags, POINT point,
    DWORD eventTime)
{
    if (flags & TOUCHEVENTF_DOWN)
    {
        if (m_touchActive)
            return;

        m_touchId = id;
        m_touchActive = true;
        m_touchMoved = false;
        m_touchAxis = -1;
        m_touchStartPoint = point;
        m_touchLastPoint = point;
        m_touchLastEventTime = eventTime;
        m_touchScrollVelocity = 0.0f;
        m_touchStartHwnd = HitTest3DScene(point.x, point.y);
        return;
    }

    if (!m_touchActive || id != m_touchId)
        return;

    const DWORD idleMs = eventTime - m_touchLastEventTime;

    if (flags & (TOUCHEVENTF_MOVE | TOUCHEVENTF_UP))
    {
        if (!m_touchMoved)
        {
            const LONG dx = point.x - m_touchStartPoint.x;
            const LONG dy = point.y - m_touchStartPoint.y;
            if (std::max(std::abs(dx), std::abs(dy)) < kTouchSwipeThresholdPx)
            {
                if (!(flags & TOUCHEVENTF_UP))
                    return;
            }
            else
            {
                m_touchAxis = std::abs(dx) >= std::abs(dy) ? 0 : 1;
                m_touchMoved = true;
                m_touchLastPoint = m_touchStartPoint;
                m_scrollSnapActive = false;
            }
        }

        if (m_touchMoved)
        {
            const DWORD elapsedMs = eventTime - m_touchLastEventTime;
            const float elapsed = (float)elapsedMs * 0.001f;
            const LONG deltaPixels = m_touchAxis == 0
                ? point.x - m_touchLastPoint.x
                : point.y - m_touchLastPoint.y;
            const float extent = (float)(m_touchAxis == 0 ? m_width : m_height);
            const float deltaSlots = (float)deltaPixels * (float)kMaxVisibleCards
                / std::max(extent, 1.0f);
            m_scrollTarget += deltaSlots;
            if (std::abs(deltaSlots) > 1e-4f && elapsed > 1e-4f)
            {
                const float rawVelocity = deltaSlots / elapsed;
                const float sampleVelocity = kTouchSwipeVelocityLimit
                    * std::tanh(rawVelocity / kTouchSwipeVelocityLimit);
                m_touchScrollVelocity = 0.5f * m_touchScrollVelocity
                    + 0.5f * sampleVelocity;
            }
            m_touchLastPoint = point;
            m_touchLastEventTime = eventTime;
        }
    }

    if (flags & TOUCHEVENTF_UP)
    {
        const bool wasSwipe = m_touchMoved;
        HWND touchedWindow = m_touchStartHwnd;
        m_touchActive = false;
        m_touchId = 0;
        m_touchStartHwnd = nullptr;
        if (!wasSwipe)
        {
            m_touchScrollVelocity = 0.0f;
            if (touchedWindow)
                SelectWindow(touchedWindow);
            else
                ExitView();
        }
        else
        {
            if (idleMs > kTouchFlingIdleMs)
                m_touchScrollVelocity = 0.0f;

            if (kCanHasEasing.load(std::memory_order_relaxed)
                && std::abs(m_touchScrollVelocity) >= kTouchFlingMinVelocity)
            {
                m_scrollSnapActive = false;
            }
            else
            {
                m_touchScrollVelocity = 0.0f;
                m_scrollTarget = std::round(m_scrollTarget);
                m_scrollSnapActive = true;
            }
        }
    }
}

bool Flip3DCompApp::OnTouch(WPARAM wParam, LPARAM lParam)
{
    const UINT touchCount = LOWORD(wParam);
    std::vector<TOUCHINPUT> touches(touchCount);
    const HTOUCHINPUT touchHandle = (HTOUCHINPUT)lParam;
    if (!GetTouchInputInfo(touchHandle, touchCount, touches.data(), sizeof(TOUCHINPUT)))
    {
        CloseTouchInputHandle(touchHandle);
        return false;
    }

    for (const TOUCHINPUT& touch : touches)
    {
        POINT point = { touch.x / 100, touch.y / 100 };
        ScreenToClient(m_hwnd, &point);
        OnTouchContact(touch.dwID, touch.dwFlags, point, touch.dwTime);
    }

    CloseTouchInputHandle(touchHandle);
    return true;
}

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
        if (FAILED(hr))
            return hr;

        ComPtr<ID3D11Texture2D> tex;
        ComPtr<ID3D11RenderTargetView> rtv;
        hr = dxgiSurf.As(&tex);
        if (SUCCEEDED(hr))
            hr = d3d->CreateRenderTargetView(tex.Get(), nullptr, &rtv);
        if (SUCCEEDED(hr))
        {
            ComPtr<ID3D11DeviceContext> ctx;
            d3d->GetImmediateContext(&ctx);
            ctx->ClearRenderTargetView(rtv.Get(), color);
        }
        const HRESULT endDrawResult = bg->EndDraw();
        if (FAILED(hr))
            return hr;
        if (FAILED(endDrawResult))
            return endDrawResult;

        outSurface = std::move(bg);
        return S_OK;
    }

} // namespace

// ============================================================================
// Flip3DCompApp::InitComposition
// ============================================================================
HRESULT Flip3DCompApp::InitComposition()
{
    Wh_Log(L"DComp init: begin");
    if (!m_d3dDevice)
        return E_FAIL;

    ComPtr<IDXGIDevice> dxgi;
    HRESULT hr = m_d3dDevice.As(&dxgi);
    if (FAILED(hr))
        return hr;

    hr = DCompositionCreateDevice2(dxgi.Get(), IID_PPV_ARGS(&m_dcompDevice));
    if (FAILED(hr))
        return hr;
    Wh_Log(L"DComp init: device created");

    hr = m_dcompDevice->CreateTargetForHwnd(m_hwnd, TRUE, &m_dcompTarget);
    if (FAILED(hr))
        return hr;
    Wh_Log(L"DComp init: target created");

    ComPtr<IDCompositionVisual2> root;
    hr = m_dcompDevice->CreateVisual(&root);
    if (FAILED(hr))
        return hr;
    m_rootVisual = root;
    hr = m_dcompTarget->SetRoot(root.Get());
    if (FAILED(hr))
        return hr;

    ComPtr<IDCompositionVisual2> sceneBase;
    hr = m_dcompDevice->CreateVisual(&sceneBase);
    if (FAILED(hr))
        return hr;
    hr = sceneBase.As(&m_sceneVisual);
    if (FAILED(hr))
        return hr;
    hr = m_sceneVisual->SetDepthMode(DCOMPOSITION_DEPTH_MODE_TREE);
    if (FAILED(hr))
        return hr;
    hr = m_sceneVisual->SetTransform(Math::Identity());
    if (FAILED(hr))
        return hr;

    ComPtr<IDCompositionVisual> rootBase;
    root.As(&rootBase);
    hr = rootBase->AddVisual(m_sceneVisual.Get(), FALSE, nullptr);
    if (FAILED(hr))
        return hr;
    Wh_Log(L"DComp init: root and scene visuals created");

    const float wash[4] = { 0.04f, 0.05f, 0.08f, 1.0f };
    hr = CreateSolidSurface(m_d3dDevice.Get(), m_dcompDevice.Get(), wash, m_washSurface);
    if (FAILED(hr))
        return hr;

    hr = m_dcompDevice->Commit();
    Wh_Log(L"DComp init: initial commit returned 0x%08X", hr);
    return hr;
}

// ============================================================================
// Flip3DCompApp::DestroyMonitorBackdrops
// ============================================================================
void Flip3DCompApp::DestroyMonitorBackdrops()
{
    if (!m_rootVisual)
    {
        m_monitorBackdrops.clear();
        return;
    }

    ComPtr<IDCompositionVisual> rootBase;
    if (SUCCEEDED(m_rootVisual.As(&rootBase)))
    {
        for (auto& mon : m_monitorBackdrops)
        {
            if (mon.shellContainer)
                rootBase->RemoveVisual(mon.shellContainer.Get());
            if (mon.washVisual)
                rootBase->RemoveVisual(mon.washVisual.Get());
            if (mon.hShellThumb)
            {
                DwmUnregisterThumbnail(mon.hShellThumb);
                mon.hShellThumb = nullptr;
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
                DwmUnregisterThumbnail(mon.hTaskbarThumb);
                mon.hTaskbarThumb = nullptr;
            }
        }
    }

    m_monitorBackdrops.clear();
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

    for (auto& mon : m_monitorBackdrops)
    {
        const LONG washW = mon.rcMonitor.right - mon.rcMonitor.left;
        const LONG washH = mon.rcMonitor.bottom - mon.rcMonitor.top;
        const float washX = (float)(mon.rcMonitor.left - vx);
        const float washY = (float)(mon.rcMonitor.top - vy);

        if (mon.washVisual)
        {
            ComPtr<IDCompositionVisual2> wash2;
            if (SUCCEEDED(mon.washVisual.As(&wash2)))
            {
                const D2D_MATRIX_3X2_F xform = {
                    { {
                    (float)std::max(washW, 1L), 0.f,
                    0.f, (float)std::max(washH, 1L),
                    washX, washY,
                    } }
                };
                wash2->SetTransform(xform);
            }
        }

        const float shellX = (float)(mon.rcMonitor.left - vx);
        const float shellY = (float)(mon.rcMonitor.top - vy);

        if (mon.shellContainer)
        {
            ComPtr<IDCompositionVisual2> shell2;
            if (SUCCEEDED(mon.shellContainer.As(&shell2)))
            {
                const D2D_MATRIX_3X2_F xform = {
                    { {
                    1.f, 0.f,
                    0.f, 1.f,
                    shellX, shellY,
                    } }
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
                    { {
                    scaleX, 0.f,
                    0.f, scaleY,
                    wallpaperX, wallpaperY,
                    } }
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
                            { {
                            1.f, 0.f,
                            0.f, 1.f,
                            (float)origin.x, (float)origin.y,
                            } }
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
                    { {
                    1.f, 0.f,
                    0.f, 1.f,
                    (float)(mon.rcTaskbar.left - vx),
                    (float)(mon.rcTaskbar.top - vy),
                    } }
                };
                taskbar2->SetTransform(xform);
            }
        }

        const LONG shellW = mon.rcMonitor.right - mon.rcMonitor.left;
        const LONG shellH = mon.rcMonitor.bottom - mon.rcMonitor.top;
        if (mon.hShellThumb && shellW > 0 && shellH > 0)
        {
            RECT rcSource = mon.rcMonitor;
            OffsetRect(&rcSource, -shellWnd.left, -shellWnd.top);

            DWM_THUMBNAIL_PROPERTIES tp = {};
            tp.dwFlags = DWM_TNP_VISIBLE | DWM_TNP_RECTDESTINATION | DWM_TNP_RECTSOURCE
                | DWM_TNP_DISABLEFORCECVI;
            tp.fVisible = TRUE;
            tp.rcSource = rcSource;
            tp.rcDestination = {
                0, 0,
                mon.rcMonitor.right - mon.rcMonitor.left,
                mon.rcMonitor.bottom - mon.rcMonitor.top,
            };
            DwmUpdateThumbnailProperties(mon.hShellThumb, &tp);
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
            DwmUpdateThumbnailProperties(mon.hTaskbarThumb, &tp);
        }
    }

    if (m_dcompDevice)
        m_dcompDevice->Commit();
}

// ============================================================================
// Flip3DCompApp::RebuildMonitorBackdropsIfNeeded
// ============================================================================
bool Flip3DCompApp::RebuildMonitorBackdropsIfNeeded()
{
    const std::vector<MONITORINFO> monitors = EnumerateMonitors();
    if (monitors.empty())
        return false;

    bool layoutSame = monitors.size() == m_monitorBackdrops.size();
    if (layoutSame)
    {
        for (size_t i = 0; i < monitors.size(); ++i)
        {
            const MONITORINFO& a = monitors[i];
            const MonitorBackdrop& b = m_monitorBackdrops[i];
            if (a.rcMonitor.left != b.rcMonitor.left
                || a.rcMonitor.top != b.rcMonitor.top
                || a.rcMonitor.right != b.rcMonitor.right
                || a.rcMonitor.bottom != b.rcMonitor.bottom
                || a.rcWork.left != b.rcWork.left
                || a.rcWork.top != b.rcWork.top
                || a.rcWork.right != b.rcWork.right
                || a.rcWork.bottom != b.rcWork.bottom)
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

    if (!m_washSurface && m_d3dDevice && m_dcompDevice)
    {
        const float wash[4] = { 0.04f, 0.05f, 0.08f, 1.0f };
        if (FAILED(CreateSolidSurface(
            m_d3dDevice.Get(), m_dcompDevice.Get(), wash, m_washSurface)))
            return false;
    }

    HWND shell = GetShellWindow();
    if (!m_dcompDevice || !m_rootVisual || !m_washSurface)
        return false;

    ComPtr<IDCompositionVisual> rootBase;
    if (FAILED(m_rootVisual.As(&rootBase)))
        return false;

    ComPtr<IDCompositionSurface> wallpaperSurface;
    UINT wallpaperWidth = 0;
    UINT wallpaperHeight = 0;
    if (!shell
        && FAILED(CreateWallpaperSurface(m_d3dDevice.Get(), m_dcompDevice.Get(),
            wallpaperSurface, wallpaperWidth,
            wallpaperHeight)))
    {
        const float black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
        if (FAILED(CreateSolidSurface(m_d3dDevice.Get(), m_dcompDevice.Get(),
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
        Wh_Log(L"Backdrop: creating visuals for monitor %ld,%ld-%ld,%ld",
            mi.rcMonitor.left, mi.rcMonitor.top,
            mi.rcMonitor.right, mi.rcMonitor.bottom);
        MonitorBackdrop mon = {};
        mon.rcMonitor = mi.rcMonitor;
        mon.rcWork = mi.rcWork;

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
            tp.dwFlags = DWM_TNP_VISIBLE | DWM_TNP_RECTDESTINATION | DWM_TNP_RECTSOURCE
                | DWM_TNP_DISABLEFORCECVI;
            tp.fVisible = TRUE;
            tp.rcSource = rcSource;
            tp.rcDestination = {
                0, 0,
                mon.rcMonitor.right - mon.rcMonitor.left,
                mon.rcMonitor.bottom - mon.rcMonitor.top,
            };

            void* pv = nullptr;
            Wh_Log(L"Backdrop: creating shell thumbnail for hwnd=%p", shell);
            hr = m_pfnCreateSharedThumbVisual(
                m_hwnd,
                shell,
                DWM_TNF_DWMWINDOW,
                &tp,
                m_dcompDevice.Get(),
                &pv,
                &mon.hShellThumb);

            if (FAILED(hr) || !pv)
            {
                Wh_Log(L"Backdrop: shell thumbnail failed hr=0x%08X visual=%p",
                    static_cast<unsigned>(hr), pv);
                continue;
            }
            Wh_Log(L"Backdrop: shell thumbnail created visual=%p", pv);

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

                    HTHUMBNAIL hTaskbarThumb = nullptr;
                    void* taskbarPv = nullptr;
                    Wh_Log(L"Backdrop: creating taskbar thumbnail for hwnd=%p", taskbar);
                    hr = m_pfnCreateSharedThumbVisual(
                        m_hwnd, taskbar, DWM_TNF_DWMWINDOW, &taskbarProperties,
                        m_dcompDevice.Get(), &taskbarPv, &hTaskbarThumb);
                    Wh_Log(L"Backdrop: taskbar thumbnail returned hr=0x%08X visual=%p",
                        static_cast<unsigned>(hr), taskbarPv);
                    if (SUCCEEDED(hr) && taskbarPv)
                    {
                        mon.rcTaskbar = taskbarRect;
                        mon.hTaskbarThumb = hTaskbarThumb;

                        ComPtr<IDCompositionVisual> taskbarVisual;
                        taskbarVisual.Attach((IDCompositionVisual*)taskbarPv);
                        hr = taskbarVisual.As(&mon.taskbarThumb);
                        if (FAILED(hr))
                        {
                            DwmUnregisterThumbnail(mon.hTaskbarThumb);
                            mon.hTaskbarThumb = nullptr;
                            mon.rcTaskbar = {};
                            continue;
                        }
                        ComPtr<IDCompositionVisual2> taskbarContainer;
                        hr = m_dcompDevice->CreateVisual(&taskbarContainer);
                        if (SUCCEEDED(hr))
                            hr = taskbarContainer.As(&mon.taskbarContainer);
                        if (SUCCEEDED(hr))
                            hr = mon.taskbarContainer->AddVisual(mon.taskbarThumb.Get(), FALSE, nullptr);
                        if (FAILED(hr))
                        {
                            DwmUnregisterThumbnail(mon.hTaskbarThumb);
                            mon.hTaskbarThumb = nullptr;
                            mon.taskbarThumb.Reset();
                            mon.taskbarContainer.Reset();
                            mon.rcTaskbar = {};
                        }
                    }
                }
            }

            ComPtr<IDCompositionVisual> thumbBase;
            thumbBase.Attach((IDCompositionVisual*)pv);
            hr = thumbBase.As(&mon.shellThumb);
            if (FAILED(hr))
            {
                DwmUnregisterThumbnail(mon.hShellThumb);
                continue;
            }

            ComPtr<IDCompositionVisual2> shellContainer;
            hr = m_dcompDevice->CreateVisual(&shellContainer);
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
            HRESULT hr = m_dcompDevice->CreateVisual(&wallpaperVisual);
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
                        if (FAILED(m_dcompDevice->CreateVisual(&tileVisual))
                            || FAILED(tileVisual->SetContent(wallpaperSurface.Get())))
                            continue;

                        mon.wallpaperTiles.push_back(tileVisual);
                        mon.wallpaperTileOrigins.push_back({ x, y });
                    }
                }
            }
        }

        ComPtr<IDCompositionVisual2> washVis;
        hr = m_dcompDevice->CreateVisual(&washVis);
        if (FAILED(hr))
            continue;
        hr = washVis->SetContent(m_washSurface.Get());
        if (FAILED(hr))
            continue;
        hr = washVis.As(&mon.washVisual);
        if (FAILED(hr))
            continue;

        // Z-order back→front: shell desktop, wash, then scene (added first in InitComposition).
        if (mon.shellContainer)
            rootBase->AddVisual(mon.shellContainer.Get(), FALSE, m_sceneVisual.Get());
        if (mon.wallpaperVisual)
        {
            ComPtr<IDCompositionVisual> wallpaperBase;
            if (SUCCEEDED(mon.wallpaperVisual.As(&wallpaperBase)))
                rootBase->AddVisual(wallpaperBase.Get(), FALSE, m_sceneVisual.Get());
        }
        for (const auto& tile : mon.wallpaperTiles)
        {
            if (tile)
                rootBase->AddVisual(tile.Get(), FALSE, m_sceneVisual.Get());
        }
        if (mon.taskbarContainer)
            rootBase->AddVisual(mon.taskbarContainer.Get(), FALSE, m_sceneVisual.Get());
        rootBase->AddVisual(mon.washVisual.Get(), FALSE, m_sceneVisual.Get());

        if (mon.shellContainer)
            mon.shellContainer->SetOpacity(1.0f);
        m_monitorBackdrops.push_back(std::move(mon));
    }

    UpdateBackdropLayout();
    return true;
}

// ============================================================================
// Flip3DCompApp::CreateShellBackdrop
// ============================================================================
HRESULT Flip3DCompApp::CreateShellBackdrop()
{
    if (m_backdropReady || m_backdropBuildPending)
        return S_OK;

    m_backdropBuildPending = true;
    if (!SetTimer(m_hwnd, kBuildShellBackdropTimerId, 0, nullptr))
    {
        m_backdropBuildPending = false;
        return HRESULT_FROM_WIN32(GetLastError());
    }

    return S_OK;
}

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
    m_initError.clear();

    m_dwmapi = LoadLibraryW(L"dwmapi.dll");
    if (!m_dwmapi)
    {
        m_initError = L"Failed to load dwmapi.dll.";
        return false;
    }

    m_pfnCreateSharedThumbVisual = (DwmpCreateSharedThumbnailVisual_fn)
        GetProcAddress(m_dwmapi, MAKEINTRESOURCEA(147));
    m_pfnQueryThumbSize = (DwmpQueryWindowThumbnailSourceSize_fn)
        GetProcAddress(m_dwmapi, MAKEINTRESOURCEA(162));
    m_pfnGetWindowMinimizeRect = (GetWindowMinimizeRect_fn)
        GetProcAddress(GetModuleHandleW(L"user32.dll"), "GetWindowMinimizeRect");

    if (!m_pfnCreateSharedThumbVisual)
    {
        m_initError = L"DwmpCreateSharedThumbnailVisual (dwmapi ord 147) is required.";
        UnloadThumbApi();
        return false;
    }
    if (!m_pfnQueryThumbSize)
    {
        m_initError = L"DwmpQueryWindowThumbnailSourceSize (dwmapi ord 162) is required.";
        UnloadThumbApi();
        return false;
    }
    if (!m_pfnGetWindowMinimizeRect)
    {
        m_initError = L"GetWindowMinimizeRect (user32) is required.";
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
    m_pfnCreateSharedThumbVisual = nullptr;
    m_pfnQueryThumbSize = nullptr;
    m_pfnGetWindowMinimizeRect = nullptr;

    if (m_dwmapi)
    {
        FreeLibrary(m_dwmapi);
        m_dwmapi = nullptr;
    }
}

// ============================================================================
// Flip3DCompApp::UpdateCardGeometry
// uDWM Flip3DWindow::OnOriginalRectUpdated:
//   - flatBounds in screen pixels (per-window monitor for taskbar/minimize)
//   - NormalizeWindowSize + world mapping via shared PRIMARY rcWork (normMon*)
//   - GetMonitorToWorldTransform on primary m_rcMonitor for all cards
// ============================================================================
void Flip3DCompApp::UpdateCardGeometry(CardModel& c, float normMonW, float normMonH,
    bool selectedRestore)
{
    HWND h = c.m_hwnd;
    if (!h)
        return;

    normMonW = std::max(normMonW, 1.0f);
    normMonH = std::max(normMonH, 1.0f);

    c.m_isMinimized = !!IsIconic(h) && !selectedRestore;
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
    if (FAILED(m_pfnQueryThumbSize(h, FALSE, &srcSize))
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
        if (m_pfnGetWindowMinimizeRect(h, &minRect) && !IsRectEmpty(&minRect))
            flatBounds = Math::BuildFinalMinRect(minRect, thumbAspect);
    }
    else if (!FillRestoredScreenRect(h, mi, flatBounds))
    {
        flatBounds = mi.rcWork;
    }

    if (IsRectEmpty(&flatBounds))
        flatBounds = mi.rcWork;

    c.m_nativeSrcWidth = (int)thumbW;
    c.m_nativeSrcHeight = (int)thumbH;

    // targetSize / occupancy = 3D carousel (uDWM finalSize).
    Math::WorldSizesFromThumbPixels(
        thumbW, thumbH, normMonW, normMonH,
        c.m_flatSize, c.m_targetSize, c.m_occupancy);

    c.m_aspectRatio = thumbW / thumbH;

    const float qualityScale = CardThumbnailQualityScale();
    c.m_srcWidth = std::max(1, (int)std::lround(thumbW * qualityScale));
    c.m_srcHeight = std::max(1, (int)std::lround(thumbH * qualityScale));

    // 2D flat: position from flatBounds; size from QueryThumbSize (restored pixels).
    // Exceptions: shell uses rcWork; iconic minimize uses taskbar tile dimensions.
    float flatW = thumbW;
    float flatH = thumbH;
    if (c.m_isShellDesktop || c.m_isMinimized)
    {
        flatW = (float)std::max(1L, flatBounds.right - flatBounds.left);
        flatH = (float)std::max(1L, flatBounds.bottom - flatBounds.top);
    }

    c.m_flatSize = { flatW / normMonW, flatH / normMonH };

    float anchorX = (float)flatBounds.left;
    float anchorY = (float)flatBounds.top;
    if (m_rtl)
    {
        const float relX = anchorX - m_monOriginX;
        anchorX = m_monOriginX + (normMonW - (flatW + relX));
    }

    float worldX = 0.0f;
    float worldY = 0.0f;
    Math::MonitorToWorldTopLeft(
        anchorX, anchorY,
        m_monOriginX, m_monOriginY, normMonW, normMonH,
        worldX, worldY);

    c.m_originalPos = { worldX, worldY, 0.0f };
    c.m_flatPos = { worldX, worldY, 0.0f };
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
    m_viewX = (float)(mi.rcWork.left - vx);
    m_viewY = (float)(mi.rcWork.top - vy);

    const float newMonW = (float)std::max(1L, mi.rcWork.right - mi.rcWork.left);
    const float newMonH = (float)std::max(1L, mi.rcWork.bottom - mi.rcWork.top);
    const float newOriginX = (float)mi.rcWork.left;
    const float newOriginY = (float)mi.rcWork.top;

    const bool layoutChanged =
        newMonW != m_monW ||
        newMonH != m_monH ||
        newOriginX != m_monOriginX ||
        newOriginY != m_monOriginY;

    m_monW = newMonW;
    m_monH = newMonH;
    m_monOriginX = newOriginX;
    m_monOriginY = newOriginY;

    if (layoutChanged)
    {
        for (auto& card : m_cards)
            UpdateCardGeometry(card, m_monW, m_monH);
    }

    if (m_backdropReady)
    {
        RebuildMonitorBackdropsIfNeeded();
        UpdateBackdropLayout();
    }
}

// ============================================================================
// Flip3DCompApp::BuildCards
// ============================================================================
bool Flip3DCompApp::BuildCards()
{
    Wh_Log(L"Switcher startup: building cards");
    m_cards.clear();

    if (!m_d3dDevice)
    {
        D3D_FEATURE_LEVEL fl = D3D_FEATURE_LEVEL_11_0;
        HRESULT hr = D3D11CreateDevice(
            nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
            D3D11_CREATE_DEVICE_BGRA_SUPPORT,
            &fl, 1, D3D11_SDK_VERSION,
            &m_d3dDevice, nullptr, nullptr);
        Wh_Log(L"D3D device creation returned 0x%08X", hr);

        if (FAILED(hr))
        {
            hr = D3D11CreateDevice(
                nullptr, D3D_DRIVER_TYPE_WARP, nullptr,
                D3D11_CREATE_DEVICE_BGRA_SUPPORT,
                &fl, 1, D3D11_SDK_VERSION,
                &m_d3dDevice, nullptr, nullptr);
            Wh_Log(L"D3D WARP device creation returned 0x%08X", hr);
        }
    }

    auto hwnds = EnumerateWindows();

    std::stable_sort(hwnds.begin(), hwnds.end(),
        [](HWND a, HWND b)
        {
            auto rank = [](HWND h)
                {
                    auto it = std::find(g_mruWindows.begin(), g_mruWindows.end(), h);
                    return it == g_mruWindows.end()
                        ? g_mruWindows.size()
                        : static_cast<size_t>(it - g_mruWindows.begin());
                };
            return rank(a) < rank(b);
        });

    MONITORINFO primaryMi = QueryPrimaryMonitor();
    m_monW = (float)std::max(1L, primaryMi.rcWork.right - primaryMi.rcWork.left);
    m_monH = (float)std::max(1L, primaryMi.rcWork.bottom - primaryMi.rcWork.top);
    m_monOriginX = (float)primaryMi.rcWork.left;
    m_monOriginY = (float)primaryMi.rcWork.top;

    int carouselIndex = 0;
    for (auto h : hwnds)
    {
        CardModel c;
        c.m_hwnd = h;
        c.m_initialCarouselIndex = carouselIndex++;
        UpdateCardGeometry(c, m_monW, m_monH);
        m_cards.push_back(std::move(c));
    }

    m_originalFrontHwnd = m_cards.empty() ? nullptr : m_cards[0].m_hwnd;
    Wh_Log(L"Switcher startup: enumerated %u cards", static_cast<unsigned>(m_cards.size()));
    return !m_cards.empty();
}

// ============================================================================
// Flip3DCompApp::CreateCardVisuals
// ============================================================================
HRESULT Flip3DCompApp::CreateCardVisuals()
{
    if (!m_dcompDevice || !m_sceneVisual || m_cards.empty())
        return E_FAIL;

    for (auto& card : m_cards)
    {
        if (card.m_containerVisual)
            continue;

        const HRESULT hr = CreateCardVisual(card);
        if (FAILED(hr))
            return hr;
    }

    return S_OK;
}

// ============================================================================
// Flip3DCompApp::UpdateCardThumbnailDest
// Sync DWM thumbnail rcDestination to the current source pixel size.
// ============================================================================
void Flip3DCompApp::UpdateCardThumbnailDest(CardModel& card)
{
    if (!card.m_hThumb || card.m_srcWidth <= 0 || card.m_srcHeight <= 0)
        return;

    DWM_THUMBNAIL_PROPERTIES tp = {};
    tp.dwFlags = DWM_TNP_VISIBLE | DWM_TNP_RECTDESTINATION
        | DWM_TNP_ENABLE3D | DWM_TNP_DISABLEFORCECVI;
    tp.fVisible = TRUE;
    tp.rcDestination = { 0, 0, card.m_srcWidth, card.m_srcHeight };
    DwmUpdateThumbnailProperties(card.m_hThumb, &tp);
}

// ============================================================================
// Flip3DCompApp::OnThumbnailSourceSizeChanged
// uDWM CThumbnailVisual::OnSizeChanged posts WM 0x327 to hwndDestination when
// DWM_TNF_DWMWINDOW is set and the live preview bitmap changes size. Every
// card thumbnail posts independently, so the WndProc only sets m_thumbnailsDirty
// and this runs once per frame, touching cards whose queried source size differs.
// ============================================================================
void Flip3DCompApp::OnThumbnailSourceSizeChanged()
{
    m_thumbnailsDirty = false;

    bool anyChange = false;
    for (auto& card : m_cards)
    {
        if (!card.m_hwnd)
            continue;

        SIZE querySize = {};
        if (FAILED(m_pfnQueryThumbSize(card.m_hwnd, FALSE, &querySize)))
            continue;

        const int queryW = (int)std::max(0L, querySize.cx);
        const int queryH = (int)std::max(0L, querySize.cy);
        if (queryW == card.m_nativeSrcWidth && queryH == card.m_nativeSrcHeight)
            continue;

        const bool selectedRestore = card.m_hwnd == m_selectedHwnd;
        UpdateCardGeometry(card, m_monW, m_monH, selectedRestore);
        UpdateCardThumbnailDest(card);
        anyChange = true;
    }

    if (anyChange && m_dcompDevice)
        m_dcompDevice->Commit();
}

// ============================================================================
// Flip3DComp_Window.cpp — Window creation + enumeration
// ============================================================================

#include <algorithm>

struct Flip3DCompApp::EnumContext
{
    Flip3DCompApp* app;
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
    if (shell && shell != m_hwnd && QualifiesForView(shell))
    {
        auto it = std::find(ctx.hwnds.begin(), ctx.hwnds.end(), shell);
        if (it == ctx.hwnds.end() && ctx.hwnds.size() < (size_t)kMaxCards)
            ctx.hwnds.push_back(shell);
    }

    return ctx.hwnds;
}

// ============================================================================
// Flip3DCompApp::ApplyFullscreenLayout
// uDWM EnableInputHooksHelper: WS_POPUP covering m_rcVirtualScreen.
// ============================================================================
void Flip3DCompApp::ApplyFullscreenLayout()
{
    if (!m_hwnd)
        return;

    const int x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    const int y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    const int w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    const int h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    SetWindowPos(m_hwnd, HWND_TOPMOST, x, y, w, h, SWP_SHOWWINDOW);

    RECT client = {};
    if (GetClientRect(m_hwnd, &client))
    {
        m_width = std::max(1u, (UINT)(client.right - client.left));
        m_height = std::max(1u, (UINT)(client.bottom - client.top));
    }

    UpdateMonitorRect();
}

// ============================================================================
// Flip3DCompApp::CreateAppWindow
// uDWM Flip3D input window: borderless popup, topmost, full virtual desktop.
// ============================================================================
bool Flip3DCompApp::CreateAppWindow()
{
    constexpr wchar_t kAppWindowClassName[] = L"WindhawkFlip3DCompWindow";
    WNDCLASSEXW wc = {
        sizeof(wc),
        CS_HREDRAW | CS_VREDRAW,
        &Flip3DCompApp::WndProc,
        0, 0,
        m_hInstance,
        nullptr,
        LoadCursorW(nullptr, IDC_ARROW),
        nullptr, nullptr,
        kAppWindowClassName,
        nullptr,
    };
    if (!RegisterClassExW(&wc))
        return false;
    m_windowClassRegistered = true;

    const int x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    const int y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    const int w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    const int h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    m_hwnd = CreateWindowExW(
        WS_EX_NOREDIRECTIONBITMAP | WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        kAppWindowClassName,
        L"",
        WS_POPUP,
        x, y, w, h,
        nullptr, nullptr,
        m_hInstance,
        this);

    if (!m_hwnd)
    {
        if (m_windowClassRegistered)
        {
            UnregisterClassW(kAppWindowClassName, m_hInstance);
            m_windowClassRegistered = false;
        }
        return false;
    }

    RegisterTouchWindow(m_hwnd, 0);

    m_rtl = (GetWindowLongPtrW(m_hwnd, GWL_EXSTYLE) & WS_EX_LAYOUTRTL) != 0;

    RECT client = {};
    if (GetClientRect(m_hwnd, &client))
    {
        m_width = std::max(1u, (UINT)(client.right - client.left));
        m_height = std::max(1u, (UINT)(client.bottom - client.top));
    }

    return true;
}

// ============================================================================
// Flip3DComp.cpp — Application lifecycle: Initialize, Run, WndProc, HandleMessage
// ============================================================================

#include <windowsx.h>

namespace
{
    constexpr ULONG_PTR kMousePromotionSignature = 0xFF515700u;
    constexpr ULONG_PTR kMousePromotionSignatureMask = 0xFFFFFF00u;
    constexpr ULONG_PTR kMousePromotionTouchFlag = 0x00000080u;
    constexpr ULONG_PTR kMousePromotionPenFlag = 0x00000040u;

    bool IsPromotedPointerMouseMessage()
    {
        const ULONG_PTR extraInfo = (ULONG_PTR)GetMessageExtraInfo();
        return (extraInfo & kMousePromotionSignatureMask) == kMousePromotionSignature
            && (extraInfo & (kMousePromotionTouchFlag | kMousePromotionPenFlag)) != 0;
    }
}

// ============================================================================
// Flip3DCompApp::EnsureTopmost
// ============================================================================
void Flip3DCompApp::EnsureTopmost()
{
    if (!m_hwnd || !IsWindow(m_hwnd))
        return;

    SetWindowPos(m_hwnd, HWND_TOPMOST, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

// ============================================================================
// Flip3DCompApp::Initialize
// ============================================================================
bool Flip3DCompApp::Initialize(HINSTANCE hInstance, bool openingTabHeld)
{
    Wh_Log(L"Switcher startup: initialize begin");
    m_hInstance = hInstance;

    BOOL compositionEnabled = FALSE;
    if (FAILED(DwmIsCompositionEnabled(&compositionEnabled)) || !compositionEnabled)
    {
        m_initError = L"Desktop composition is disabled, No Flip3D.";
        return false;
    }

    if (!InitAccessibility())
    {
        m_initError = L"Failed to initialize COM for the switcher thread.";
        return false;
    }

    if (!LoadThumbApi())
        return false;

    BuildCards();
    if (m_cards.size() <= 1)
    {
        m_initError = L"Fewer than two windows to switch between. No Flip3D.";
        return false;
    }
    Wh_Log(L"Switcher startup: card models built");

    if (!CreateAppWindow())
    {
        if (m_initError.empty())
            m_initError = L"Failed to create the Flip3D input window.";
        return false;
    }

    UpdateMonitorRect();

    if (FAILED(InitComposition()))
    {
        if (m_initError.empty())
            m_initError = L"Failed to initialize DirectComposition.";
        return false;
    }
    Wh_Log(L"Switcher startup: composition initialized");

    if (FAILED(CreateCardVisuals()))
    {
        if (m_initError.empty())
            m_initError = L"Failed to create DWM thumbnail visuals.";
        return false;
    }
    Wh_Log(L"Switcher startup: card visuals created");

    if (FAILED(CreateShellBackdrop()))
    {
        if (m_initError.empty())
            m_initError = L"Failed to create the shell desktop backdrop.";
        return false;
    }
    Wh_Log(L"Switcher startup: backdrop scheduled");

    m_state = ViewState::Enter;
    m_animEnter.Restart(0.0f, 1.0f, kEnterExitDurationSec,
        kCanHasEasing
        ? InterpolationMode::CubicBezier
        : InterpolationMode::Linear);
    m_prevFrame = std::chrono::steady_clock::now();
    m_openingTabPending = openingTabHeld
        || g_navigationKeysHeld[VK_TAB].load();
    m_openingTabStart = m_openingTabPending ? m_prevFrame
        : std::chrono::steady_clock::time_point{};

    EnterFlip3DWindowMode();
    InitAccessibility();

    // First-frame layout + DComp commit before ShowWindow (avoids blank flash).
    Update(0.0f);
    Wh_Log(L"Switcher startup: first frame updated");

    return true;
}

// ============================================================================
// Flip3DCompApp::Run
// ============================================================================
int Flip3DCompApp::Run()
{
    MSG msg = {};
    m_inRunLoop = true;

    if (m_state == ViewState::Enter)
    {
        m_animEnter.Restart(0.0f, 1.0f, kEnterExitDurationSec,
            kCanHasEasing
            ? InterpolationMode::CubicBezier
            : InterpolationMode::Linear);
    }
    m_prevFrame = std::chrono::steady_clock::now();

    while (msg.message != WM_QUIT)
    {
        if (g_hostStopEvent
            && WaitForSingleObject(g_hostStopEvent, 0) == WAIT_OBJECT_0)
            break;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        if (msg.message == WM_QUIT)
            break;

        if (!m_minimized)
        {
            EnsureTopmost();
            auto now = std::chrono::steady_clock::now();
            float dt = std::chrono::duration<float>(now - m_prevFrame).count();
            m_prevFrame = now;
            Update(std::min(dt, 0.05f));
            DwmFlush();
        }
        else
        {
            MsgWaitForMultipleObjects(g_hostStopEvent ? 1 : 0,
                g_hostStopEvent ? &g_hostStopEvent : nullptr,
                FALSE, INFINITE, QS_ALLINPUT);
            m_prevFrame = std::chrono::steady_clock::now();
        }
    }

    m_inRunLoop = false;

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
            self->m_hwnd = hwnd;
        }
    }

    auto* self = (Flip3DCompApp*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
    return self
        ? self->HandleMessage(msg, wParam, lParam)
        : DefWindowProcW(hwnd, msg, wParam, lParam);
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

        KillTimer(m_hwnd, kBuildShellBackdropTimerId);
        m_backdropBuildPending = false;
        m_backdropReady = RebuildMonitorBackdropsIfNeeded() || !m_monitorBackdrops.empty();
        if (m_backdropReady)
            UpdateBackdropLayout();
        return 0;

    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
        {
            m_minimized = true;
            return 0;
        }
        m_minimized = false;
        m_width = std::max(1u, (UINT)LOWORD(lParam));
        m_height = std::max(1u, (UINT)HIWORD(lParam));
        UpdateMonitorRect();
        return 0;

    case WM_DISPLAYCHANGE:
        ApplyFullscreenLayout();
        UpdateMonitorRect();
        return 0;

    case WM_DWMTHUMBNAILSOURCESIZECHANGED:
        // Each registered thumbnail posts 0x327 independently; coalesce to one
        // refresh per frame in Update() (wParam = adapter LUID low part).
        m_thumbnailsDirty = true;
        return 0;

    case WM_MOUSEWHEEL:
        OnWheel(GET_WHEEL_DELTA_WPARAM(wParam));
        return 0;

    case WM_TOUCH:
        OnTouch(wParam, lParam);
        return 0;

    case WM_POINTERENTER:
    case WM_POINTERUPDATE:
    case WM_POINTERDOWN:
    case WM_POINTERUP:
    case WM_POINTERLEAVE:
        if (OnPenPointer(msg, wParam))
            return 0;
        break;

    case WM_MOUSEMOVE:
        if (IsPromotedPointerMouseMessage())
            return 0;
        m_hitHwnd = HitTest3DScene(
            (LONG)(short)LOWORD(lParam),
            (LONG)(short)HIWORD(lParam));
        SetCursor(LoadCursorW(nullptr, m_hitHwnd ? IDC_HAND : IDC_ARROW));
        return 0;

    case WM_LBUTTONDOWN:
        if (IsPromotedPointerMouseMessage())
            return 0;
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
        if (m_state == ViewState::Exit ||
            m_state == ViewState::ExitRepeatedRotate)
        {
            DestroyWindow(m_hwnd);
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

        return LresultFromObject(__uuidof(IAccessible), wParam, pAccessible);
    }

    case WM_DESTROY:
        LeaveFlip3DWindowMode();
        if (m_inRunLoop)
            PostQuitMessage(0);
        return 0;
    }

    if (m_wmShellHook && msg == m_wmShellHook)
    {
        OnShellHookMessage(wParam, lParam);
        return 0;
    }

    return DefWindowProcW(m_hwnd, msg, wParam, lParam);
}


const wchar_t kHostClassName[] = L"Flip3DCompWindhawkHost";
const wchar_t kHostWindowName[] = L"Flip3DCompWindhawkHostWindow";
const wchar_t kKeyboardClassName[] = L"Flip3DCompKeyboardInput";
const UINT kHostKeyMessage = WM_APP + 0x71;
const UINT kHostCommitMessage = WM_APP + 0x72;
const UINT_PTR kHotkeyPollTimerId = 0x7F33;
const int kHotkeyIdCtrlWinTab = 0x7F32;
const ULONG_PTR kReplayedWindowsKeyTag = 0x3344434F;

HMODULE g_modModule = nullptr;
UINT g_activationMessage = 0;
HANDLE g_hostThread = nullptr;
HANDLE g_hostReadyEvent = nullptr;
bool g_hostInitializationSucceeded = false;
HANDLE g_keyboardThread = nullptr;
HANDLE g_keyboardReadyEvent = nullptr;
HANDLE g_keyboardStopEvent = nullptr;
bool g_keyboardInitializationSucceeded = false;
HHOOK g_keyboardHook = nullptr;
std::atomic_bool g_switcherEngaged{ false };
std::atomic_bool g_TaskViewOverride{ false };

// The render thread owns the app and commit policy. The input thread only posts
// messages and reads the published engaged flag.
std::unique_ptr<Flip3DCompApp> g_app;
bool g_commitOnWindowsKeyRelease = false;

// The hook records Win-key state even when it suppresses delivery to Windows.
// The UI release timer must include that state rather than just GetAsyncKeyState.
std::atomic<unsigned> g_windowsKeyMask{ 0 };

// Only the keyboard thread accesses the remaining key bookkeeping.
unsigned g_deferredWindowsKeyMask = 0;
unsigned g_forwardedWindowsKeyMask = 0;
unsigned g_suppressWindowsKeyMask = 0;
unsigned g_controlKeyMask = 0;
bool g_tabDown = false;
bool g_suppressTabUntilUp = false;
bool g_navigationDown[256] = {};
bool g_rawKeysDown[256] = {};

bool SendWindowsKeyEvent(unsigned bit, bool down)
{
    INPUT input = {};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = bit == 1u ? VK_LWIN : VK_RWIN;
    input.ki.dwExtraInfo = kReplayedWindowsKeyTag;
    input.ki.dwFlags = KEYEVENTF_EXTENDEDKEY | (down ? 0 : KEYEVENTF_KEYUP);
    return SendInput(1, &input, sizeof(input)) == 1;
}

void SendWindowsKeyTap(unsigned bit)
{
    INPUT inputs[2] = {};
    for (INPUT& input : inputs)
    {
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = bit == 1u ? VK_LWIN : VK_RWIN;
        input.ki.dwExtraInfo = kReplayedWindowsKeyTag;
        input.ki.dwFlags = KEYEVENTF_EXTENDEDKEY;
    }
    inputs[1].ki.dwFlags |= KEYEVENTF_KEYUP;
    if (SendInput(2, inputs, sizeof(INPUT)) == 1)
        SendWindowsKeyEvent(bit, false);
}

void ForwardDeferredWindowsKeys()
{
    for (unsigned bit : { 1u, 2u })
    {
        if ((g_deferredWindowsKeyMask & bit) && SendWindowsKeyEvent(bit, true))
        {
            g_deferredWindowsKeyMask &= ~bit;
            g_forwardedWindowsKeyMask |= bit;
        }
    }
}

void RecordForegroundWindow(HWND hwnd)
{
    if (!hwnd || !IsWindow(hwnd))
        return;
    hwnd = GetAncestor(hwnd, GA_ROOT);
    if (!hwnd || hwnd == GetShellWindow() || hwnd == GetDesktopWindow())
        return;
    WCHAR cls[64] = {};
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    if (wcscmp(cls, L"WindhawkFlip3DCompWindow") == 0
        || wcscmp(cls, L"Shell_TrayWnd") == 0
        || wcscmp(cls, L"Shell_SecondaryTrayWnd") == 0
        || wcscmp(cls, L"WorkerW") == 0
        || wcscmp(cls, L"Progman") == 0)
        return;
    auto it = std::find(g_mruWindows.begin(), g_mruWindows.end(), hwnd);
    if (it != g_mruWindows.end())
        g_mruWindows.erase(it);
    g_mruWindows.insert(g_mruWindows.begin(), hwnd);
    if (g_mruWindows.size() > 256)
        g_mruWindows.resize(256);
}

void CALLBACK ForegroundEventProc(HWINEVENTHOOK, DWORD, HWND hwnd,
    LONG, LONG, DWORD, DWORD)
{
    RecordForegroundWindow(hwnd);
}

bool IsWindowsKeyPhysicallyDown()
{
    return (GetAsyncKeyState(VK_LWIN) & 0x8000)
        || (GetAsyncKeyState(VK_RWIN) & 0x8000);
}

void LaunchSwitcher(bool sticky, bool openingTabHeld)
{
    if (WaitForSingleObject(g_hostStopEvent, 0) == WAIT_OBJECT_0)
        return;
    if (g_app)
    {
        if (sticky)
            g_commitOnWindowsKeyRelease = false;
        if (openingTabHeld)
            PostMessageW(g_app->WindowHandle(), WM_KEYDOWN, VK_TAB, 0);
        return;
    }

    g_switcherEngaged = true;
    g_commitOnWindowsKeyRelease = !sticky;
    auto app = std::make_unique<Flip3DCompApp>();
    // Enumeration, D3D, DWM and WIC work must stay off the keyboard thread.
    if (!app->Initialize(g_modModule, openingTabHeld))
    {
        Wh_Log(L"Flip3DComp initialization failed: %s", app->InitErrorMessage());
        app->Shutdown();
        g_commitOnWindowsKeyRelease = false;
        g_switcherEngaged = false;
        return;
    }

    g_app = std::move(app);
    ShowWindow(g_app->WindowHandle(), SW_SHOW);
    SetWindowPos(g_app->WindowHandle(), HWND_TOPMOST, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    if (!SetForegroundWindow(g_app->WindowHandle()))
        Wh_Log(L"Switcher could not take foreground focus; navigation remains on the keyboard hook");
    UpdateWindow(g_app->WindowHandle());
    if (!sticky && !SetTimer(g_hostWindow, kHotkeyPollTimerId, 30, nullptr))
    {
        Wh_Log(L"Failed to start the Windows-key release timer: %lu", GetLastError());
        g_app->Shutdown();
    }
    else
    {
        g_app->Run();
        g_app->Shutdown();
    }
    KillTimer(g_hostWindow, kHotkeyPollTimerId);
    g_app.reset();
    g_commitOnWindowsKeyRelease = false;
    g_switcherEngaged = false;
}

LRESULT CALLBACK HostWindowProc(HWND hwnd, UINT message, WPARAM wParam,
    LPARAM lParam)
{
    if (message == g_activationMessage)
    {
        LaunchSwitcher(wParam != 0, lParam != 0);
        return 0;
    }
    switch (message)
    {
    case WM_TIMER:
        if (wParam == kRestoreTransitionsTimerId)
        {
            if (g_restoreTransitionsAt && GetTickCount64() >= g_restoreTransitionsAt)
                RestorePendingTransitions();
        }
        else if (wParam == kHotkeyPollTimerId
            && !g_windowsKeyMask.load() && !IsWindowsKeyPhysicallyDown())
        {
            if (g_app && g_commitOnWindowsKeyRelease)
            {
                g_commitOnWindowsKeyRelease = false;
                PostMessageW(g_app->WindowHandle(), WM_KEYDOWN, VK_RETURN, 0);
            }
            KillTimer(hwnd, kHotkeyPollTimerId);
        }
        return 0;

    case kHostKeyMessage:
        if (g_app)
        {
            // Both windows belong to this thread. Dispatch before the next
            // queued input instead of posting through a second queue.
            SendMessageW(g_app->WindowHandle(),
                (static_cast<ULONG_PTR>(lParam) & 0x80000000u) ? WM_KEYUP : WM_KEYDOWN,
                wParam, lParam);
        }
        return 0;

    case kHostCommitMessage:
        if (g_app && g_commitOnWindowsKeyRelease)
        {
            g_commitOnWindowsKeyRelease = false;
            PostMessageW(g_app->WindowHandle(), WM_KEYDOWN, VK_RETURN, 0);
        }
        return 0;

    case WM_DESTROY:
        RestorePendingTransitions();
        KillTimer(hwnd, kHotkeyPollTimerId);
        return 0;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

bool QueueKeyboardActivation(bool sticky, bool openingTabHeld = true)
{
    if (g_switcherEngaged.exchange(true))
        return true;
    if (PostMessageW(g_hostWindow, g_activationMessage, sticky, openingTabHeld))
        return true;
    g_switcherEngaged = false;
    return false;
}

void QueueNativeActivation(bool sticky, bool openingTabHeld)
{
    // A registered hotkey can consume Tab's make event before WM_INPUT.
    // Seed its held state here, on the thread which also receives its break.
    if (openingTabHeld)
    {
        g_tabDown = (GetAsyncKeyState(VK_TAB) & 0x8000) != 0;
        g_rawKeysDown[VK_TAB] = g_tabDown;
        g_navigationKeysHeld[VK_TAB] = g_tabDown;
    }
    if (g_switcherEngaged)
        PostMessageW(g_hostWindow, g_activationMessage, sticky, openingTabHeld);
    else
        QueueKeyboardActivation(sticky, openingTabHeld);
}

LRESULT CALLBACK LowLevelKeyboardProc(int code, WPARAM message, LPARAM data)
{
    if (code != HC_ACTION)
        return CallNextHookEx(nullptr, code, message, data);
    const auto* key = reinterpret_cast<const KBDLLHOOKSTRUCT*>(data);
    if ((key->flags & LLKHF_INJECTED) && key->dwExtraInfo == kReplayedWindowsKeyTag)
        return CallNextHookEx(nullptr, code, message, data);
    const bool down = message == WM_KEYDOWN || message == WM_SYSKEYDOWN;
    const bool up = message == WM_KEYUP || message == WM_SYSKEYUP;
    if (!down && !up)
        return CallNextHookEx(nullptr, code, message, data);

    const bool engaged = g_switcherEngaged.load();
    const bool windowsKey = key->vkCode == VK_LWIN || key->vkCode == VK_RWIN;
    if (!windowsKey && key->vkCode != VK_TAB && g_deferredWindowsKeyMask)
    {
        if (engaged)
        {
            g_suppressWindowsKeyMask |= g_deferredWindowsKeyMask;
            g_deferredWindowsKeyMask = 0;
        }
        else
        {
            ForwardDeferredWindowsKeys();
        }
    }

    switch (key->vkCode)
    {
    case VK_LWIN:
    case VK_RWIN:
    {
        const unsigned bit = key->vkCode == VK_LWIN ? 1u : 2u;
        if (down)
        {
            if (g_windowsKeyMask & bit)
                return 1;
            g_windowsKeyMask |= bit;
            if (engaged)
                g_suppressWindowsKeyMask |= bit;
            else
                g_deferredWindowsKeyMask |= bit;
            return 1;
        }
        g_windowsKeyMask &= ~bit;
        if (!g_windowsKeyMask && engaged)
            PostMessageW(g_hostWindow, kHostCommitMessage, 0, 0);
        if (g_suppressWindowsKeyMask & bit)
        {
            g_suppressWindowsKeyMask &= ~bit;
            g_deferredWindowsKeyMask &= ~bit;
            if (g_forwardedWindowsKeyMask & bit)
            {
                g_forwardedWindowsKeyMask &= ~bit;
                break;
            }
            return 1;
        }
        if (g_deferredWindowsKeyMask & bit)
        {
            g_deferredWindowsKeyMask &= ~bit;
            if (!engaged)
                SendWindowsKeyTap(bit);
            return 1;
        }
        g_forwardedWindowsKeyMask &= ~bit;
        break;
    }
    case VK_LCONTROL:
    case VK_RCONTROL:
        if (down)
            g_controlKeyMask |= key->vkCode == VK_LCONTROL ? 1u : 2u;
        else
            g_controlKeyMask &= ~(key->vkCode == VK_LCONTROL ? 1u : 2u);
        break;

    case VK_LEFT:
    case VK_RIGHT:
    case VK_UP:
    case VK_DOWN:
    case VK_HOME:
    case VK_END:
    case VK_ESCAPE:
    case VK_RETURN:
    case VK_F5:
    {
        const UINT vk = key->vkCode;
        const bool wasDown = g_navigationDown[vk];
        g_navigationDown[vk] = down && engaged;
        g_navigationKeysHeld[vk] = down;
        if (engaged || wasDown)
        {
            const LPARAM flags = ((wasDown || g_rawKeysDown[vk]) ? 0x40000000u : 0u)
                | (up ? 0x80000000u : 0u);
            PostMessageW(g_hostWindow, kHostKeyMessage, vk, flags);
        }
        // Raw input means the original down event reached Windows. Its up
        // event must also pass through, even if the switcher has since opened.
        if ((down && engaged) || (up && wasDown && !g_rawKeysDown[vk]))
            return 1;
        break;
    }
    case VK_TAB:
    {
        const bool repeated = g_tabDown;
        g_tabDown = down;
        g_navigationKeysHeld[VK_TAB] = down;
        if (down && (engaged || g_windowsKeyMask || IsWindowsKeyPhysicallyDown()))
        {
            const bool sticky = g_controlKeyMask
                || (GetAsyncKeyState(VK_CONTROL) & 0x8000);
            if (!engaged && !QueueKeyboardActivation(sticky))
            {
                ForwardDeferredWindowsKeys();
                break;
            }
            g_suppressWindowsKeyMask |= g_deferredWindowsKeyMask;
            g_deferredWindowsKeyMask = 0;
            g_suppressTabUntilUp = true;
            if (engaged)
                PostMessageW(g_hostWindow, kHostKeyMessage, VK_TAB,
                    repeated ? 0x40000000u : 0u);
            return 1;
        }
        if (up)
        {
            const bool suppress = g_suppressTabUntilUp && !g_rawKeysDown[VK_TAB];
            if (engaged || g_suppressTabUntilUp)
                PostMessageW(g_hostWindow, kHostKeyMessage, VK_TAB, 0xC0000000u);
            g_suppressTabUntilUp = false;
            if (suppress)
                return 1;
        }
        break;
    }
    }
    return CallNextHookEx(nullptr, code, message, data);
}

void OnRawKeyboard(const RAWKEYBOARD& key)
{
    if (key.ExtraInformation == kReplayedWindowsKeyTag)
        return;
    UINT vk = key.VKey;
    if (vk == VK_CONTROL)
        vk = (key.Flags & RI_KEY_E0) ? VK_RCONTROL : VK_LCONTROL;
    if (vk >= ARRAYSIZE(g_rawKeysDown))
        return;
    const bool down = !(key.Flags & RI_KEY_BREAK);
    const bool repeated = g_rawKeysDown[vk];
    g_rawKeysDown[vk] = down;
    const bool engaged = g_switcherEngaged.load();

    switch (vk)
    {
    case VK_LWIN:
    case VK_RWIN:
    {
        const unsigned bit = vk == VK_LWIN ? 1u : 2u;
        if (down)
        {
            g_windowsKeyMask |= bit;
            g_forwardedWindowsKeyMask |= bit;
            // Our hook suppresses an original Win-down. Seeing it here means
            // that path was bypassed. Without Explorer's hotkey bridge,
            // restore the hook before Tab can be consumed as a system hotkey.
            if (!repeated && !GetShellWindow())
            {
                if (g_keyboardHook)
                    UnhookWindowsHookEx(g_keyboardHook);
                g_keyboardHook = SetWindowsHookExW(WH_KEYBOARD_LL,
                    LowLevelKeyboardProc, g_modModule, 0);
                if (!g_keyboardHook)
                    Wh_Log(L"Failed to restore the keyboard hook: %lu", GetLastError());
            }
        }
        else
        {
            g_windowsKeyMask &= ~bit;
            g_forwardedWindowsKeyMask &= ~bit;
            if (!g_windowsKeyMask && engaged)
                PostMessageW(g_hostWindow, kHostCommitMessage, 0, 0);
        }
        return;
    }
    case VK_LCONTROL:
    case VK_RCONTROL:
        if (down)
            g_controlKeyMask |= vk == VK_LCONTROL ? 1u : 2u;
        else
            g_controlKeyMask &= ~(vk == VK_LCONTROL ? 1u : 2u);
        return;

    case VK_TAB:
        g_tabDown = down;
        g_navigationKeysHeld[vk] = down;
        // Without Explorer there is no shell WM_HOTKEY to forward. Raw input
        // observes keys the low-level hook did not consume, without reserving
        // Win+Tab and preventing a restarted Explorer from registering it.
        if (down && !engaged && g_windowsKeyMask && !GetShellWindow())
        {
            QueueKeyboardActivation(g_controlKeyMask != 0);
            return;
        }
        break;

    case VK_LEFT:
    case VK_RIGHT:
    case VK_UP:
    case VK_DOWN:
    case VK_HOME:
    case VK_END:
    case VK_ESCAPE:
    case VK_RETURN:
    case VK_F5:
        g_navigationKeysHeld[vk] = down;
        break;

    default:
        return;
    }

    if (engaged)
    {
        const LPARAM flags = (repeated ? 0x40000000u : 0u)
            | (!down ? 0x80000000u : 0u);
        PostMessageW(g_hostWindow, kHostKeyMessage, vk, flags);
    }
}

LRESULT CALLBACK KeyboardWindowProc(HWND hwnd, UINT message, WPARAM wParam,
    LPARAM lParam)
{
    if (message == g_activationMessage)
    {
        QueueNativeActivation(wParam != 0, lParam != 0);
        return 0;
    }
    if (message == WM_INPUT)
    {
        RAWINPUT input = {};
        UINT size = sizeof(input);
        if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT,
            &input, &size, sizeof(RAWINPUTHEADER)) != UINT(-1)
            && input.header.dwType == RIM_TYPEKEYBOARD)
            OnRawKeyboard(input.data.keyboard);
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

HWND CreateKeyboardWindow()
{
    WNDCLASSW wc = {};
    wc.lpfnWndProc = KeyboardWindowProc;
    wc.hInstance = g_modModule;
    wc.lpszClassName = kKeyboardClassName;
    if (!RegisterClassW(&wc))
    {
        Wh_Log(L"Failed to register the keyboard window class: %lu", GetLastError());
        return nullptr;
    }
    HWND hwnd = CreateWindowExW(0, kKeyboardClassName, L"", 0,
        0, 0, 0, 0, HWND_MESSAGE, nullptr, g_modModule, nullptr);
    if (hwnd)
    {
        // This runs only in the dedicated tool process, never in Explorer.
        RAWINPUTDEVICE keyboard = { 0x01, 0x06, RIDEV_INPUTSINK, hwnd };
        if (RegisterRawInputDevices(&keyboard, 1, sizeof(keyboard)))
            return hwnd;
        Wh_Log(L"Failed to register background keyboard input: %lu", GetLastError());
        DestroyWindow(hwnd);
    }
    else
    {
        Wh_Log(L"Failed to create the keyboard window: %lu", GetLastError());
    }
    UnregisterClassW(kKeyboardClassName, g_modModule);
    return nullptr;
}

DWORD WINAPI KeyboardThreadProc(void*)
{
    MSG msg = {};
    PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE);
    HWND keyboardWindow = CreateKeyboardWindow();
    if (keyboardWindow)
    {
        for (UINT vk : { VK_LWIN, VK_RWIN, VK_LCONTROL, VK_RCONTROL, VK_TAB })
            g_rawKeysDown[vk] = (GetAsyncKeyState(vk) & 0x8000) != 0;
        g_windowsKeyMask = (g_rawKeysDown[VK_LWIN] ? 1u : 0u)
            | (g_rawKeysDown[VK_RWIN] ? 2u : 0u);
        g_forwardedWindowsKeyMask = g_windowsKeyMask.load();
        g_controlKeyMask = (g_rawKeysDown[VK_LCONTROL] ? 1u : 0u)
            | (g_rawKeysDown[VK_RCONTROL] ? 2u : 0u);
        g_tabDown = g_rawKeysDown[VK_TAB];
        g_navigationKeysHeld[VK_TAB] = g_tabDown;
        g_keyboardHook = SetWindowsHookExW(WH_KEYBOARD_LL,
            LowLevelKeyboardProc, g_modModule, 0);
    }
    g_keyboardInitializationSucceeded = g_keyboardHook != nullptr;
    if (!g_keyboardHook)
        Wh_Log(L"Failed to install the keyboard hook: %lu", GetLastError());
    // Win+Tab remains registered by Explorer. This additional chord has no
    // registration on shells which only reserve Win+Tab.
    const bool stickyHotkey = g_keyboardHook && RegisterHotKey(nullptr,
        kHotkeyIdCtrlWinTab, MOD_WIN | MOD_CONTROL | MOD_NOREPEAT, VK_TAB);
    SetEvent(g_keyboardReadyEvent);

    while (g_keyboardInitializationSucceeded
        && MsgWaitForMultipleObjects(1, &g_keyboardStopEvent, FALSE,
            INFINITE, QS_ALLINPUT) == WAIT_OBJECT_0 + 1)
    {
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_HOTKEY && msg.wParam == kHotkeyIdCtrlWinTab)
                QueueNativeActivation(true, true);
            else
            {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }
    }
    if (stickyHotkey)
        UnregisterHotKey(nullptr, kHotkeyIdCtrlWinTab);
    if (g_keyboardHook)
        UnhookWindowsHookEx(g_keyboardHook);
    g_keyboardHook = nullptr;
    if (keyboardWindow)
    {
        RAWINPUTDEVICE keyboard = { 0x01, 0x06, RIDEV_REMOVE, nullptr };
        if (!RegisterRawInputDevices(&keyboard, 1, sizeof(keyboard)))
            Wh_Log(L"Failed to unregister background keyboard input: %lu", GetLastError());
        DestroyWindow(keyboardWindow);
        UnregisterClassW(kKeyboardClassName, g_modModule);
    }
    // Do not leave a deferred physical Windows key without its down event
    // when the hook stops before the user releases the key.
    for (unsigned bit : { 1u, 2u })
    {
        if ((g_deferredWindowsKeyMask | g_suppressWindowsKeyMask) & bit)
            SendWindowsKeyEvent(bit, true);
    }
    g_windowsKeyMask = g_deferredWindowsKeyMask = 0;
    g_forwardedWindowsKeyMask = g_suppressWindowsKeyMask = 0;
    g_controlKeyMask = 0;
    g_tabDown = g_suppressTabUntilUp = false;
    std::fill(std::begin(g_navigationDown), std::end(g_navigationDown), false);
    std::fill(std::begin(g_rawKeysDown), std::end(g_rawKeysDown), false);
    for (auto& held : g_navigationKeysHeld)
        held = false;
    return g_keyboardInitializationSucceeded ? 0 : 1;
}

DWORD WINAPI HostThreadProc(void*)
{
    if (!SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2))
    {
        Wh_Log(L"Failed to set switcher thread DPI awareness: %lu", GetLastError());
        SetEvent(g_hostReadyEvent);
        return 1;
    }
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = HostWindowProc;
    wc.hInstance = g_modModule;
    wc.lpszClassName = kHostClassName;
    if (!RegisterClassExW(&wc))
    {
        Wh_Log(L"Failed to register the host window class: %lu", GetLastError());
        SetEvent(g_hostReadyEvent);
        return 1;
    }
    HWND host = CreateWindowExW(WS_EX_TOOLWINDOW, kHostClassName,
        kHostWindowName, WS_POPUP, -32000, -32000, 1, 1,
        nullptr, nullptr, g_modModule, nullptr);
    if (!host)
    {
        Wh_Log(L"Failed to create the switcher host window: %lu", GetLastError());
        UnregisterClassW(kHostClassName, g_modModule);
        SetEvent(g_hostReadyEvent);
        return 1;
    }
    g_hostWindow = host;
    RecordForegroundWindow(GetForegroundWindow());
    g_mruHook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND,
        EVENT_SYSTEM_FOREGROUND, nullptr, ForegroundEventProc, 0, 0,
        WINEVENT_OUTOFCONTEXT);
    g_hostInitializationSucceeded = g_mruHook != nullptr;
    if (!g_mruHook)
        Wh_Log(L"Failed to track foreground windows: %lu", GetLastError());
    SetEvent(g_hostReadyEvent);
    MSG msg = {};
    while (g_hostInitializationSucceeded
        && MsgWaitForMultipleObjects(1, &g_hostStopEvent, FALSE,
            INFINITE, QS_ALLINPUT) == WAIT_OBJECT_0 + 1)
    {
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
    if (g_mruHook)
    {
        UnhookWinEvent(g_mruHook);
        g_mruHook = nullptr;
    }
    g_hostWindow = nullptr;
    DestroyWindow(host);
    UnregisterClassW(kHostClassName, g_modModule);
    g_mruWindows.clear();
    return 0;
}

bool ForwardSwitcherActivation(bool sticky, bool openingTabHeld)
{
    HWND input = FindWindowExW(HWND_MESSAGE, nullptr, kKeyboardClassName, nullptr);
    if (!input || !g_activationMessage)
        return false;
    DWORD processId = 0;
    GetWindowThreadProcessId(input, &processId);
    if (!processId || !AllowSetForegroundWindow(processId))
        return false;
    return PostMessageW(input, g_activationMessage, sticky, openingTabHeld) != FALSE;
}

enum ALL_UP_VIEW_FLAGS : int;
using CAllUpViewService__ToggleAllUpView_t =
    LONG(__thiscall*)(void*, HMONITOR, ALL_UP_VIEW_FLAGS);
CAllUpViewService__ToggleAllUpView_t CAllUpViewService__ToggleAllUpView_orig = nullptr;

LONG __thiscall CAllUpViewService__ToggleAllUpView_hook(
    void* pThis, HMONITOR monitor, ALL_UP_VIEW_FLAGS flags)
{
    // twinui.pcshell AMD64 22621.317 RVA 0x493D0 and 26100.863 RVA 0x461AD0:
    // this service queues the task which creates/shows Task View. SetActive
    // only focuses an already shown frame.
    // Redirect before the native task is queued, not after its windows exist.
    if (g_TaskViewOverride && ForwardSwitcherActivation(true, false))
        return S_OK;
    return CAllUpViewService__ToggleAllUpView_orig(pThis, monitor, flags);
}

using DwmOrdinal105_t = HRESULT(WINAPI*)();
DwmOrdinal105_t g_originalDwmOrdinal105 = nullptr;

HRESULT WINAPI DwmOrdinal105Hook()
{
    if (ForwardSwitcherActivation(true, false))
        return S_OK;
    return g_originalDwmOrdinal105();
}

bool HookDwmOrdinal105()
{
    HMODULE dwmapi = LoadLibraryExW(L"dwmapi.dll", nullptr,
        LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!dwmapi)
    {
        Wh_Log(L"Failed to load dwmapi.dll: %lu", GetLastError());
        return false;
    }
    void* target = reinterpret_cast<void*>(
        GetProcAddress(dwmapi, MAKEINTRESOURCEA(105)));
    if (!target || !Wh_SetFunctionHook(target,
        reinterpret_cast<void*>(DwmOrdinal105Hook),
        reinterpret_cast<void**>(&g_originalDwmOrdinal105)))
    {
        Wh_Log(L"Failed to hook dwmapi ordinal 105");
        return false;
    }
    return true;
}

void LoadTaskViewSettings()
{
    g_TaskViewOverride = Wh_GetIntSetting(L"TaskViewOverride") != 0;
}

bool HookNativeTaskView()
{
#if defined(_M_X64) || defined(__x86_64__)
    HMODULE module = LoadLibraryExW(L"twinui.pcshell.dll", nullptr,
        LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (module)
    {
        const WindhawkUtils::SYMBOL_HOOK hooks[] = {
            {
                {
                    L"public: virtual long __cdecl CAllUpViewService::ToggleAllUpView(struct HMONITOR__ *,enum ALL_UP_VIEW_FLAGS)"
                },
                &CAllUpViewService__ToggleAllUpView_orig,
                CAllUpViewService__ToggleAllUpView_hook,
                true
            }
        };
        if (!WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks)))
            return false;
    }
#endif
    if (g_TaskViewOverride && !CAllUpViewService__ToggleAllUpView_orig)
    {
        Wh_Log(L"Task View override is unavailable in this twinui.pcshell.dll");
        return false;
    }
    return true;
}

void WhTool_ModUninit()
{
    if (g_keyboardStopEvent)
        SetEvent(g_keyboardStopEvent);
    if (g_keyboardThread)
    {
        WaitForSingleObject(g_keyboardThread, INFINITE);
        CloseHandle(g_keyboardThread);
        g_keyboardThread = nullptr;
    }
    if (g_hostStopEvent)
        SetEvent(g_hostStopEvent);
    if (g_hostThread)
    {
        WaitForSingleObject(g_hostThread, INFINITE);
        CloseHandle(g_hostThread);
        g_hostThread = nullptr;
    }
    for (HANDLE* event : { &g_hostReadyEvent, &g_hostStopEvent,
        &g_keyboardReadyEvent, &g_keyboardStopEvent })
    {
        if (*event)
            CloseHandle(*event);
        *event = nullptr;
    }
    g_switcherEngaged = false;
}

BOOL WhTool_ModInit()
{
    LoadAnimationSettings();
    g_hostInitializationSucceeded = false;
    g_keyboardInitializationSucceeded = false;
    g_hostStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_hostReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_keyboardStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_keyboardReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_hostStopEvent || !g_hostReadyEvent
        || !g_keyboardStopEvent || !g_keyboardReadyEvent)
    {
        Wh_Log(L"Failed to create switcher thread events: %lu", GetLastError());
        WhTool_ModUninit();
        return FALSE;
    }
    g_hostThread = CreateThread(nullptr, 0, HostThreadProc, nullptr, 0, nullptr);
    if (!g_hostThread)
    {
        Wh_Log(L"Failed to create the render thread: %lu", GetLastError());
        WhTool_ModUninit();
        return FALSE;
    }
    WaitForSingleObject(g_hostReadyEvent, INFINITE);
    if (!g_hostInitializationSucceeded)
    {
        WhTool_ModUninit();
        return FALSE;
    }
    g_keyboardThread = CreateThread(nullptr, 0, KeyboardThreadProc, nullptr, 0, nullptr);
    if (!g_keyboardThread)
    {
        Wh_Log(L"Failed to create the keyboard thread: %lu", GetLastError());
        WhTool_ModUninit();
        return FALSE;
    }
    WaitForSingleObject(g_keyboardReadyEvent, INFINITE);
    if (!g_keyboardInitializationSucceeded)
    {
        WhTool_ModUninit();
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModSettingsChanged()
{
    LoadAnimationSettings();
}

// Only this bridge runs inside Explorer. Native hotkey registrations are left
// intact; an unavailable tool leaves WM_HOTKEY untouched.
HHOOK g_messageHook = nullptr;
HANDLE g_messageHookThread = nullptr;
HANDLE g_explorerStopEvent = nullptr;
HANDLE g_explorerThread = nullptr;
std::atomic<unsigned> g_messageHookCalls{ 0 };

LRESULT CALLBACK ExplorerGetMessageProc(int code, WPARAM wParam, LPARAM lParam)
{
    g_messageHookCalls.fetch_add(1);
    if (code == HC_ACTION && wParam == PM_REMOVE)
    {
        MSG* msg = reinterpret_cast<MSG*>(lParam);
        if (msg->message == WM_HOTKEY && HIWORD(msg->lParam) == VK_TAB)
        {
            const UINT modifiers = LOWORD(msg->lParam);
            if ((modifiers & MOD_WIN)
                && !(modifiers & ~(MOD_WIN | MOD_CONTROL | MOD_SHIFT | MOD_NOREPEAT))
                && ForwardSwitcherActivation((modifiers & MOD_CONTROL) != 0, true))
                msg->message = WM_NULL;
        }
    }
    const LRESULT result = CallNextHookEx(nullptr, code, wParam, lParam);
    g_messageHookCalls.fetch_sub(1);
    return result;
}

bool HookExplorerHotkeyThread()
{
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
        return false;
    THREADENTRY32 entry = { sizeof(entry) };
    for (BOOL more = Thread32First(snapshot, &entry); more;
        more = Thread32Next(snapshot, &entry))
    {
        if (entry.th32OwnerProcessID != GetCurrentProcessId())
            continue;
        HANDLE thread = OpenThread(THREAD_QUERY_LIMITED_INFORMATION | SYNCHRONIZE,
            FALSE, entry.th32ThreadID);
        if (!thread)
            continue;
        PWSTR name = nullptr;
        const bool matches = SUCCEEDED(GetThreadDescription(thread, &name))
            && name && wcscmp(name, L"Immersive Shell") == 0;
        LocalFree(name);
        if (matches)
        {
            g_messageHook = SetWindowsHookExW(WH_GETMESSAGE,
                ExplorerGetMessageProc, nullptr, entry.th32ThreadID);
            if (g_messageHook)
            {
                g_messageHookThread = thread;
                break;
            }
        }
        CloseHandle(thread);
    }
    CloseHandle(snapshot);
    return g_messageHook != nullptr;
}

void UnhookExplorerHotkeyThread()
{
    if (g_messageHook)
    {
        UnhookWindowsHookEx(g_messageHook);
        g_messageHook = nullptr;
    }
    // UnhookWindowsHookEx does not wait for a callback already in progress.
    while (g_messageHookCalls.load())
        Sleep(1);
    if (g_messageHookThread)
    {
        CloseHandle(g_messageHookThread);
        g_messageHookThread = nullptr;
    }
}

DWORD WINAPI ExplorerThreadProc(void*)
{
    while (WaitForSingleObject(g_explorerStopEvent, 0) == WAIT_TIMEOUT)
    {
        HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (taskbar)
        {
            DWORD processId = 0;
            GetWindowThreadProcessId(taskbar, &processId);
            if (processId != GetCurrentProcessId())
                return 0;
            break;
        }
        if (WaitForSingleObject(g_explorerStopEvent, 1000) != WAIT_TIMEOUT)
            return 0;
    }

    bool loggedMissing = false;
    for (;;)
    {
        if (!g_messageHook && !HookExplorerHotkeyThread() && !loggedMissing)
        {
            Wh_Log(L"Waiting for Explorer's Immersive Shell hotkey thread");
            loggedMissing = true;
        }
        const HANDLE events[] = { g_explorerStopEvent, g_messageHookThread };
        DWORD result = WaitForMultipleObjects(g_messageHook ? 2 : 1, events,
            FALSE, g_messageHook ? INFINITE : 5000);
        if (result == WAIT_OBJECT_0 + 1)
        {
            UnhookExplorerHotkeyThread();
            loggedMissing = false;
        }
        else if (result != WAIT_TIMEOUT)
            break;
    }
    UnhookExplorerHotkeyThread();
    return 0;
}

void StopExplorerPart()
{
    if (g_explorerStopEvent)
        SetEvent(g_explorerStopEvent);
    if (g_explorerThread)
    {
        WaitForSingleObject(g_explorerThread, INFINITE);
        CloseHandle(g_explorerThread);
        g_explorerThread = nullptr;
    }
    if (g_explorerStopEvent)
    {
        CloseHandle(g_explorerStopEvent);
        g_explorerStopEvent = nullptr;
    }
}

BOOL StartExplorerPart()
{
    g_explorerStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_explorerStopEvent)
        return FALSE;
    g_explorerThread = CreateThread(nullptr, 0, ExplorerThreadProc, nullptr, 0, nullptr);
    if (!g_explorerThread)
    {
        StopExplorerPart();
        return FALSE;
    }
    return TRUE;
}

bool g_isExplorer = false;
bool g_isRundll32 = false;

bool InitializeModuleGlobals()
{
    g_activationMessage = RegisterWindowMessageW(L"Flip3DComp.Windhawk.Activate");
    return g_activationMessage && GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(&InitializeModuleGlobals), &g_modModule);
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
    if (!InitializeModuleGlobals()) {
        Wh_Log(L"Failed to initialize module identity or activation message");
        return FALSE;
    }
    WCHAR exe[MAX_PATH] = {};
    if (!GetModuleFileNameW(nullptr, exe, ARRAYSIZE(exe))) {
        return FALSE;
    }
    const WCHAR* name = wcsrchr(exe, L'\\');
    name = name ? name + 1 : exe;
    if (_wcsicmp(name, L"explorer.exe") == 0) {
        g_isExplorer = true;
        LoadTaskViewSettings();
        if (!HookNativeTaskView() || !HookDwmOrdinal105()) {
            return FALSE;
        }
        return StartExplorerPart();
    }
    if (_wcsicmp(name, L"rundll32.exe") == 0) {
        g_isRundll32 = true;
        int argc = 0;
        LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
        bool shortcut = false;
        if (argv && argc > 1) {
            const WCHAR* entry = wcsrchr(argv[1], L'\\');
            entry = entry ? entry + 1 : argv[1];
            shortcut = _wcsicmp(entry, L"dwmapi.dll,#105") == 0;
        }
        LocalFree(argv);
        return shortcut && HookDwmOrdinal105();
    }
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
    if (g_isExplorer || g_isRundll32) {
        return;
    }
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
    if (g_isExplorer || g_isRundll32) {
        LoadTaskViewSettings();
        if (g_isExplorer && g_TaskViewOverride
            && !CAllUpViewService__ToggleAllUpView_orig) {
            Wh_Log(L"Task View override is unavailable; leave this option disabled");
        }
        return;
    }
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isExplorer || g_isRundll32) {
        StopExplorerPart();
        return;
    }
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
