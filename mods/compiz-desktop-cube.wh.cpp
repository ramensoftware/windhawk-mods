// ==WindhawkMod==
// @id              compiz-desktop-cube
// @name            Compiz Desktop Cube for Windows 11
// @description     Compiz/GNOME style 3D glass desktop prism for virtual desktops. Drag up from the taskbar to open it, turn it to switch desktops, right-click the screen edges to add or remove desktops. Live desktop, blurred background.
// @version         5.9.0
// @author          Soham
// @github          https://github.com/Soham-Barate
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lwindowsapp -ldwmapi -luuid -luser32 -lgdi32 -lcomctl32 -ld3d11 -ld2d1 -ldxgi -ldxguid -ldwrite -lwindowscodecs -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Compiz Desktop Cube for Windows 11

A Compiz / GNOME style 3D desktop prism for Windows 11 virtual desktops.
Built on Windows.UI.Composition + Direct2D with a GPU blurred background.

## What's new in 5.9
* Lower CPU use: the mouse hook no longer wakes the render thread with a system call on every mouse event, and the
  compositor is no longer sent property values it already has (opacity / visibility of the faces).
* Hidden glass rims and shading layers are really hidden (no GPU fill) instead of drawn at zero opacity.
* The desktop composer runs in background mode and skips desktops it captured less than 20 seconds ago.
* The emoji panel, touch keyboard, Start / Search hosts and other shell overlays no longer appear on the desktop faces.

## How to use
1. Press and hold the **left mouse button** on **empty space** of the taskbar
   (works with an auto-hide taskbar too).
2. Drag **upward**. The desktop shrinks gradually into the cube.
3. Keep holding and drag **left / right** to turn it. After a moment **up / down** tilts it.
4. **Mouse wheel** zooms the cube in and out (scroll up = closer, scroll down = farther).
   (Optional: set "Mouse Wheel Action" to the window switcher to get the Alt+Tab style window cycling instead.)
5. **Add / remove desktops:** while the cube is open, **right-click the right edge** of the screen to add a desktop (+1) or
   **right-click the left edge** to remove one (-1). The frosted gray zones are hidden by default: move the pointer to the far left or
   far right edge and the matching zone (a - or a +) slides in. It stays while the pointer is inside it, flashes when
   you use it and hides again. "-" closes the LAST desktop (its windows move to the previous one). The cube rebuilds itself
   with a little spin and the new face fades in.
6. Release. The cube snaps to the nearest face and switches desktop.
   A quick flick also switches. Press **Escape** to cancel.

## Faces follow your desktop count
- 1 desktop: a single face with limited rotation.
- 2 desktops: two faces (a flip card).
- 3, 4, 5, 6 desktops: triangle, cube, pentagon, hexagon.
- More than 6 desktops: hexagon, faces are re-mapped while you turn it, so every desktop is reachable.

## Live desktop
- The desktop you are on is streamed LIVE (Desktop Duplication) into the front face
  and into the blurred background. A video that is playing keeps playing, blurred
  behind the cube and sharp on the face while it turns.
- The cube overlay is excluded from capture, so it never shows up inside itself.
- Desktops you are not on are rebuilt from your wallpaper plus a capture of each of
  their windows (PrintWindow). Windows that cannot be captured are simply left out
  (no more black placeholder cards). Those faces cannot be live, Windows does not
  render apps of other virtual desktops.
- If Desktop Duplication is not available (some hybrid GPU laptops), a slower GDI
  live capture is used automatically.

## Smoothness
- Persistent render thread, devices stay warm between gestures (pre-warmed as soon as you press the taskbar).
- Mouse input is read on the render thread, not on the busy taskbar thread.
- Rendering is locked to the monitor's vertical blank, so it follows 60 / 120 / 144 / 240 Hz by itself. All motion is time based.
- Live frames are presented without ever blocking the render thread, so a busy GPU can no longer make the cube miss a refresh.
- Power: no permanent 1 ms timer, normal-ish thread priorities, the blurred background refreshes at a low rate, and while the cube is held still the render loop drops to about 30 Hz (it jumps back to full refresh the instant you move the mouse).
- Turn on "Enable Diagnostic Logging" to see the measured fps and the monitor Hz in the Windhawk log.

## Notes
- The real taskbar is hidden from the capture, so every face is the full, uncropped desktop (wallpaper where the taskbar was). The taskbar is restored the moment the gesture ends. If Windows refuses to hide it, the old crop is used as a fallback.
- Window switcher: every window becomes a live thumbnail card (the selected one is refreshed every frame, the others a few times per second). It shows what is visible on screen, so a part hidden behind another window shows that other window.
- The GPU device is created on the graphics adapter that drives your monitor, so screen capture stays on the fast path.
- PrintWindow cannot render every app (some games, protected content). Those windows are left out of other desktops' faces.
- The overlay is hidden from screenshots and screen recorders on purpose (needed for the live capture).
- Desktop switching sends Win+Ctrl+Left/Right. No undocumented COM is used.
- The Windows desktop slide animation is hidden only while the cube switches desktops (explorer is told "animations off" for that moment). Your Windows "Animation effects" setting is never changed.
- If you still see a slide on your build, raise "Switch Settle Delay".
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- ActivationThreshold: 10
  $name: Drag Activation Distance (px)
  $description: Distance you must drag away from the taskbar before the shrink starts.
- HorizontalSensitivity: 45
  $name: Horizontal Rotation Sensitivity (%)
  $description: Degrees of yaw per pixel of horizontal mouse movement.
- VerticalSensitivity: 35
  $name: Vertical Tilt Sensitivity (%)
  $description: Degrees of pitch per pixel of vertical mouse movement.
- EnableVerticalRotation: true
  $name: Enable Vertical Tilt
  $description: Allow tilting the cube up and down.
- MaxTiltAngle: 60
  $name: Maximum Tilt Angle (degrees)
  $description: How far the cube can be tilted. It springs back to level on release.
- InvertHorizontal: false
  $name: Invert Horizontal Drag
  $description: By default dragging right shows the desktop on the left (like grabbing the cube). Enable to reverse.
- InvertVertical: false
  $name: Invert Vertical Drag
  $description: Reverse the tilt direction.
- WrapAround: true
  $name: Wrap Around
  $description: Treat the desktops as a loop. When off, the cube stops (rubber band) at the first and last desktop.
- FaceSize: 66
  $name: Cube Face Size (% of screen width)
  $description: Width of each face of a 4 sided cube. Prisms with other face counts are sized to match. 30 to 90.
- ShrinkDistance: 260
  $name: Shrink Distance (px)
  $description: How far you drag up for the desktop to shrink completely into the cube.
- AutoHideTaskbarFix: true
  $name: Re-hide Auto-hide Taskbar
  $description: After the gesture, an auto-hide taskbar is hidden again right away and ready for the next gesture.
- Perspective: 1100
  $name: Camera Perspective Distance (px)
  $description: Lower values give stronger 3D distortion. Minimum 700.
- DragSmoothing: 35
  $name: Drag Smoothing (ms)
  $description: How softly the cube follows the mouse. 0 means it sticks to the mouse exactly.
- SnapSpeed: 90
  $name: Snap Speed (%)
  $description: Speed of the snap-to-face spring.
- SnapBounce: 30
  $name: Snap Bounce (%)
  $description: Overshoot when the cube settles on a face. 0 is no bounce.
- ZoomAnimation: true
  $name: Zoom Open and Close Animation
  $description: The desktop shrinks gradually into the cube while you drag and grows back on release.
- ZoomSpeed: 90
  $name: Zoom Speed (%)
  $description: Speed of the open and close zoom.
- CloseFadeDuration: 140
  $name: Close Fade Duration (ms)
  $description: Final crossfade from the cube to the real desktop. 0 for instant.
- FlickToSwitch: true
  $name: Flick To Switch
  $description: A fast flick switches desktops even if the cube was not rotated far enough.
- FlickStrength: 100
  $name: Flick Strength (%)
  $description: How strongly mouse speed at release pushes the cube to the next face.
- SwitchAngle: 25
  $name: Switch Angle (degrees)
  $description: Rotation needed before the cube commits to the next face when you release slowly.
- BackgroundBlur: true
  $name: Enable Background Blur
  $description: Blur the desktop behind the cube using a GPU Gaussian blur.
- BlurStrength: 28
  $name: Blur Strength
  $description: Standard deviation of the background blur.
- BackgroundDim: 25
  $name: Background Dim (%)
  $description: Dark tint over the blurred background.
- BackgroundParallax: true
  $name: Background Parallax
  $description: The blurred background drifts slightly as the cube turns.
- FaceBrightness: 100
  $name: Face Shading (%)
  $description: How much faces turning away are darkened. The front face is never darkened.
- ShowDesktopLabels: true
  $name: Show Desktop Badges
  $description: Show a "Desktop N" badge at the top of each face.
- FaceGap: 14
  $name: Gap Between Faces (%)
  $description: Empty space between neighbouring faces. 0 closes the prism.
- FrontFaceOpacity: 100
  $name: Front Face Opacity (%)
  $description: How solid the face looking at you is.
- SideFaceOpacity: 55
  $name: Side Face Opacity (%)
  $description: Opacity of the faces at the sides (translucent glass).
- BackFaceOpacity: 10
  $name: Far Faces Opacity (%)
  $description: Opacity of the faces on the far side (seen through the gap).
- HorizontalFirst: true
  $name: Horizontal First
  $description: The cube first only turns left/right. Vertical tilt unlocks afterwards.
- VerticalUnlockDelay: 350
  $name: Vertical Unlock Delay (ms)
  $description: Time after the cube opens before up/down tilting is enabled.
- ShowShadow: true
  $name: Show Floor Shadow
  $description: Soft shadow below the cube.
- TextureQuality: 100
  $name: Face Texture Quality (%)
  $description: Resolution of face textures relative to your screen (capped at 1920 wide). 25 to 100.
- SuppressSystemAnimation: true
  $name: Suppress Windows Desktop Slide Animation
  $description: Hides the Windows desktop slide animation only while the cube switches desktops. Your Windows "Animation effects" setting is never changed.
- SwitchSettleDelay: 120
  $name: Switch Settle Delay (ms)
  $description: Extra time the overlay stays up after Windows reports the new desktop.
- FrameRateLimit: 0
  $name: Frame Rate Limit (FPS)
  $description: 0 means match the monitor refresh rate exactly (recommended).
- WheelAction: 0
  $name: Mouse Wheel Action (0 zoom, 1 window switcher, 2 off)
  $description: What the mouse wheel does while you drag the cube. 0 zooms the cube in and out. 1 is the Alt+Tab style window switcher (uses more resources). 2 leaves the wheel alone.
- EdgeDesktopControl: true
  $name: Right-click Edges To Add / Remove Desktops
  $description: While the cube is open, right-click the right edge to add a desktop and the left edge to remove the last one.
- EdgeZoneWidth: 10
  $name: Edge Zone Width (% of screen width)
  $description: Width of the add / remove zones once they have appeared. 4 to 25.
- EdgeRevealWidth: 40
  $name: Edge Reveal Distance (pixels)
  $description: The zones stay invisible until the pointer is this close to the left or right screen edge. 4 to 300.
- WheelZoomStep: 10
  $name: Wheel Zoom Step (%)
  $description: How much one wheel notch zooms the cube.
- RememberZoom: false
  $name: Remember Zoom Level
  $description: Keep the wheel zoom for the next time you open the cube.
- BackgroundRefreshRate: 15
  $name: Blurred Background Refresh (Hz)
  $description: How often the blurred live background is redrawn. Lower saves power and GPU time. 1 to 60.
- IdlePowerSave: true
  $name: Idle Power Saving
  $description: While the cube is held still and nothing moves, the render loop slows to about 30 Hz. It returns to full refresh as soon as anything moves.
- AltTabLift: 90
  $name: Window Lift (px)
  $description: How far the windows rise off the face in the window switcher. The selected one rises most.
- LiveMethod: 0
  $name: Live Capture Method (0 auto, 1 Duplication, 2 GDI)
  $description: Troubleshooting. 0 tries Desktop Duplication then GDI. 2 forces the slower but simpler GDI capture.
- EnableDebugLogging: false
  $name: Enable Diagnostic Logging
  $description: Write detailed traces to the Windhawk log.
*/
// ==/WindhawkModSettings==

// Target Windows 10 and later. Defined here (before any SDK header) instead of through compiler flags.
#ifndef WINVER
#define WINVER 0x0A00
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif

#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <commctrl.h>
#include <shobjidl.h>
#include <shellapi.h>
#include <uiautomation.h>
#include <wincodec.h>
#include <d3d11.h>
#include <d2d1_1.h>
#include <d2d1effects.h>
#include <dwrite.h>
#include <dxgi.h>
#include <dxgi1_2.h>

#include <windhawk_api.h>

#include <windhawk_utils.h>

#ifndef WS_EX_NOREDIRECTIONBITMAP
#define WS_EX_NOREDIRECTIONBITMAP 0x00200000L
#endif
#ifndef WDA_EXCLUDEFROMCAPTURE
#define WDA_EXCLUDEFROMCAPTURE 0x00000011
#endif
#ifndef PW_RENDERFULLCONTENT
#define PW_RENDERFULLCONTENT 0x00000002
#endif

#include <atomic>
#include <cmath>
#include <cstdint>
#include <chrono>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <optional>

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Composition.Desktop.h>
#include <winrt/Windows.Graphics.DirectX.h>

using namespace winrt;
using namespace winrt::Windows::UI::Composition;
using namespace winrt::Windows::UI::Composition::Desktop;
using namespace winrt::Windows::Foundation::Numerics;
using namespace winrt::Windows::Graphics::DirectX;

// ============================================================================
// Composition interop definitions (custom names to avoid SDK header clashes)
// ============================================================================

// ICompositorInterop {25297D5C-3AD4-4C9C-B5CF-E36A38512330}
struct ICompositorInteropX : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE CreateCompositionSurfaceForHandle(HANDLE, void**) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateCompositionSurfaceForSwapChain(IUnknown*, void**) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateGraphicsDevice(IUnknown*, void**) = 0;
};
static const IID IID_ICompositorInteropX =
    {0x25297D5C, 0x3AD4, 0x4C9C, {0xB5, 0xCF, 0xE3, 0x6A, 0x38, 0x51, 0x23, 0x30}};

// ICompositionDrawingSurfaceInterop {FD04E6E3-FE0C-4C3C-AB19-A07601A576EE}
struct ICompositionDrawingSurfaceInteropX : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE BeginDraw(const RECT* updateRect, REFIID iid, void** updateObject, POINT* updateOffset) = 0;
    virtual HRESULT STDMETHODCALLTYPE EndDraw() = 0;
    virtual HRESULT STDMETHODCALLTYPE Resize(SIZE sizePixels) = 0;
    virtual HRESULT STDMETHODCALLTYPE Scroll(const RECT* scrollRect, const RECT* clipRect, int offsetX, int offsetY) = 0;
    virtual HRESULT STDMETHODCALLTYPE SuspendDraw() = 0;
    virtual HRESULT STDMETHODCALLTYPE ResumeDraw() = 0;
};
static const IID IID_ICompositionDrawingSurfaceInteropX =
    {0xFD04E6E3, 0xFE0C, 0x4C3C, {0xAB, 0x19, 0xA0, 0x76, 0x01, 0xA5, 0x76, 0xEE}};

// ICompositorDesktopInterop {29E691FA-4567-4DCA-B319-D0F207EB6807}
struct ICompositorDesktopInteropX : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE CreateDesktopWindowTarget(HWND hwndTarget, BOOL isTopmost, void** result) = 0;
};
static const IID IID_ICompositorDesktopInteropX =
    {0x29E691FA, 0x4567, 0x4DCA, {0xB3, 0x19, 0xD0, 0xF2, 0x07, 0xEB, 0x68, 0x07}};

// DispatcherQueue creation (CoreMessaging.dll)
struct DispatcherQueueOptionsX {
    DWORD dwSize;
    int threadType;      // DQTYPE_THREAD_CURRENT = 2
    int apartmentType;   // DQTAT_COM_NONE = 0
};

// WIC constants defined locally so no extra import libraries are needed
static const GUID kPixelFormat32bppBGR =
    {0x6fddc324, 0x4e03, 0x4bfe, {0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x0e}};
static const CLSID kClsidWICImagingFactory =
    {0xcacaf262, 0x9370, 0x4615, {0xa1, 0x3b, 0x9f, 0x55, 0x39, 0xda, 0x4c, 0x0a}};

// ============================================================================
// Logging
// ============================================================================
static bool g_debugLog = false;

#define LOG_DBG(fmt, ...) \
    do { if (g_debugLog) { Wh_Log(L"[Cube] " fmt, ##__VA_ARGS__); } } while (0)
#define LOG_INF(fmt, ...) Wh_Log(L"[Cube] " fmt, ##__VA_ARGS__)
#define LOG_ERR(fmt, ...) Wh_Log(L"[Cube ERROR] " fmt, ##__VA_ARGS__)

// Set at the very start of Wh_ModUninit. Once set, no new thread may be started and no gesture may begin.
static std::atomic<bool> g_unloading{ false };

// Handle of THIS mod's module (not explorer.exe's). Used to register the overlay window class.
static HMODULE GetThisModuleHandle() {
    HMODULE h = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&GetThisModuleHandle, &h);
    return h;
}

// ============================================================================
// Settings
// ============================================================================
struct ModSettings {
    int   activationThreshold = 10;
    float hSens = 0.45f;
    float vSens = 0.35f;
    bool  verticalRotation = true;
    float maxTilt = 60.0f;
    bool  invertH = false;
    bool  invertV = false;
    bool  wrap = true;
    float faceSize = 0.66f;
    float shrinkDist = 260.0f;
    bool  wheelZoom = true;
    float wheelStep = 0.10f;
    bool  rememberZoom = false;
    bool  autoHideFix = true;
    float sideOpacity = 0.55f;
    float perspective = 1100.0f;
    int   dragSmoothMs = 35;
    float snapSpeed = 0.9f;
    float bounce = 0.3f;
    bool  zoomAnim = true;
    float zoomSpeed = 0.9f;
    int   fadeMs = 140;
    bool  flick = true;
    float flickStrength = 1.0f;
    float switchAngle = 25.0f;
    bool  blur = true;
    float blurStrength = 28.0f;
    int   dim = 25;
    bool  parallax = true;
    int   brightness = 100;
    bool  labels = true;
    float faceGap = 0.14f;
    float frontOpacity = 1.0f;
    float backOpacity = 0.10f;
    bool  horizFirst = true;
    int   vUnlockMs = 350;
    bool  shadow = true;
    int   texQuality = 100;
    bool  suppressSysAnim = true;
    int   settleMs = 120;
    int   fpsLimit = 0;
    int   liveMethod = 0;
    bool  altTab = false;
    int   bgHz = 15;
    bool  idleSave = true;
    bool  edgeEdit = true;
    float edgeZone = 0.10f;
    int   edgeReveal = 40;
    float altLift = 90.0f;
};
static ModSettings g_settings;
static float g_userZoom = 1.0f;   // remembered wheel zoom
static const float kMinUserZoom = 0.30f;
static const float kMaxUserZoom = 1.60f;
static const int kMaxFaces = 6;
static const int kMaxDesktops = 16;

static int GetIntSetting(PCWSTR name, int def, int lo, int hi) {
    int v = Wh_GetIntSetting(name);
    if (v < lo || v > hi) return def;
    return v;
}

static void LoadSettings() {
    ModSettings s;
    s.activationThreshold = GetIntSetting(L"ActivationThreshold", 10, 1, 600);
    s.hSens = GetIntSetting(L"HorizontalSensitivity", 45, 1, 1000) / 100.0f;
    s.vSens = GetIntSetting(L"VerticalSensitivity", 35, 1, 1000) / 100.0f;
    s.verticalRotation = Wh_GetIntSetting(L"EnableVerticalRotation") != 0;
    s.maxTilt = (float)GetIntSetting(L"MaxTiltAngle", 60, 5, 85);
    s.invertH = Wh_GetIntSetting(L"InvertHorizontal") != 0;
    s.invertV = Wh_GetIntSetting(L"InvertVertical") != 0;
    s.wrap = Wh_GetIntSetting(L"WrapAround") != 0;
    s.faceSize = GetIntSetting(L"FaceSize", 66, 30, 90) / 100.0f;
    s.shrinkDist = (float)GetIntSetting(L"ShrinkDistance", 260, 60, 1200);
    s.autoHideFix = Wh_GetIntSetting(L"AutoHideTaskbarFix") != 0;
    s.sideOpacity = GetIntSetting(L"SideFaceOpacity", 55, 0, 100) / 100.0f;
    s.perspective = (float)GetIntSetting(L"Perspective", 1100, 700, 6000);
    s.dragSmoothMs = GetIntSetting(L"DragSmoothing", 35, 0, 400);
    s.snapSpeed = GetIntSetting(L"SnapSpeed", 90, 10, 500) / 100.0f;
    s.bounce = GetIntSetting(L"SnapBounce", 30, 0, 100) / 100.0f;
    s.zoomAnim = Wh_GetIntSetting(L"ZoomAnimation") != 0;
    s.zoomSpeed = GetIntSetting(L"ZoomSpeed", 90, 10, 500) / 100.0f;
    s.fadeMs = GetIntSetting(L"CloseFadeDuration", 140, 0, 1500);
    s.flick = Wh_GetIntSetting(L"FlickToSwitch") != 0;
    s.flickStrength = GetIntSetting(L"FlickStrength", 100, 0, 500) / 100.0f;
    s.switchAngle = (float)GetIntSetting(L"SwitchAngle", 25, 5, 85);
    s.blur = Wh_GetIntSetting(L"BackgroundBlur") != 0;
    s.blurStrength = (float)GetIntSetting(L"BlurStrength", 28, 1, 200);
    s.dim = GetIntSetting(L"BackgroundDim", 25, 0, 100);
    s.parallax = Wh_GetIntSetting(L"BackgroundParallax") != 0;
    s.brightness = GetIntSetting(L"FaceBrightness", 100, 0, 100);
    s.labels = Wh_GetIntSetting(L"ShowDesktopLabels") != 0;
    s.faceGap = GetIntSetting(L"FaceGap", 14, 0, 60) / 100.0f;
    s.frontOpacity = GetIntSetting(L"FrontFaceOpacity", 100, 10, 100) / 100.0f;
    s.backOpacity = GetIntSetting(L"BackFaceOpacity", 10, 0, 100) / 100.0f;
    s.horizFirst = Wh_GetIntSetting(L"HorizontalFirst") != 0;
    s.vUnlockMs = GetIntSetting(L"VerticalUnlockDelay", 350, 0, 3000);
    s.shadow = Wh_GetIntSetting(L"ShowShadow") != 0;
    s.texQuality = GetIntSetting(L"TextureQuality", 100, 25, 100);
    s.suppressSysAnim = Wh_GetIntSetting(L"SuppressSystemAnimation") != 0;
    s.settleMs = GetIntSetting(L"SwitchSettleDelay", 120, 0, 2000);
    s.fpsLimit = GetIntSetting(L"FrameRateLimit", 0, 0, 1000);
    s.liveMethod = GetIntSetting(L"LiveMethod", 0, 0, 2);
    {
        int wa = GetIntSetting(L"WheelAction", 0, 0, 2);
        s.wheelZoom = (wa == 0);
        s.altTab = (wa == 1);
    }
    s.wheelStep = GetIntSetting(L"WheelZoomStep", 10, 1, 50) / 100.0f;
    s.rememberZoom = Wh_GetIntSetting(L"RememberZoom") != 0;
    s.bgHz = GetIntSetting(L"BackgroundRefreshRate", 15, 1, 60);
    s.idleSave = Wh_GetIntSetting(L"IdlePowerSave") != 0;
    s.edgeEdit = Wh_GetIntSetting(L"EdgeDesktopControl") != 0;
    s.edgeZone = GetIntSetting(L"EdgeZoneWidth", 10, 4, 25) / 100.0f;
    s.edgeReveal = GetIntSetting(L"EdgeRevealWidth", 40, 4, 300);
    s.altLift = (float)GetIntSetting(L"AltTabLift", 90, 10, 300);
    g_debugLog = Wh_GetIntSetting(L"EnableDebugLogging") != 0;
    g_settings = s;

    LOG_INF(L"Settings loaded: threshold=%d blur=%d wrap=%d", s.activationThreshold, (int)s.blur, (int)s.wrap);
}

// ============================================================================
// Math
// ============================================================================
static const float kPi = 3.14159265f;

struct Vec3 { float x, y, z; };

// Row-vector convention (as used by Windows.UI.Composition).
struct Mat4 {
    float m[4][4];

    static Mat4 Identity() {
        Mat4 r{};
        for (int i = 0; i < 4; ++i) r.m[i][i] = 1.0f;
        return r;
    }
    static Mat4 Translation(float x, float y, float z) {
        Mat4 r = Identity();
        r.m[3][0] = x; r.m[3][1] = y; r.m[3][2] = z;
        return r;
    }
    static Mat4 RotationY(float rad) {
        Mat4 r = Identity();
        float c = std::cos(rad), s = std::sin(rad);
        r.m[0][0] = c;  r.m[0][2] = -s;
        r.m[2][0] = s;  r.m[2][2] = c;
        return r;
    }
    static Mat4 RotationX(float rad) {
        Mat4 r = Identity();
        float c = std::cos(rad), s = std::sin(rad);
        r.m[1][1] = c;  r.m[1][2] = -s;
        r.m[2][1] = s;  r.m[2][2] = c;
        return r;
    }
    static Mat4 Perspective(float d) {
        Mat4 r = Identity();
        if (d > 1.0f) r.m[2][3] = -1.0f / d;
        return r;
    }
    Mat4 operator*(const Mat4& o) const {
        Mat4 r{};
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j) {
                float sum = 0.0f;
                for (int k = 0; k < 4; ++k) sum += m[i][k] * o.m[k][j];
                r.m[i][j] = sum;
            }
        return r;
    }
    float4x4 ToWinRT() const {
        return {
            m[0][0], m[0][1], m[0][2], m[0][3],
            m[1][0], m[1][1], m[1][2], m[1][3],
            m[2][0], m[2][1], m[2][2], m[2][3],
            m[3][0], m[3][1], m[3][2], m[3][3]
        };
    }
};

// Damped spring. omega = natural frequency (rad/s), zeta = damping ratio (1 = critical).
struct Spring {
    float x = 0.0f;
    float v = 0.0f;
    void Step(float target, float omega, float zeta, float dt) {
        const float maxStep = 1.0f / 360.0f;
        while (dt > 0.0f) {
            float h = std::min(dt, maxStep);
            float a = omega * omega * (target - x) - 2.0f * zeta * omega * v;
            v += a * h;
            x += v * h;
            dt -= h;
        }
    }
};

static float RubberBand(float v, float lo, float hi, float range) {
    if (v < lo) return lo - range * (1.0f - std::exp(-(lo - v) / range));
    if (v > hi) return hi + range * (1.0f - std::exp(-(v - hi) / range));
    return v;
}

static float SmoothStep(float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

// ============================================================================
// MODULE 1: VDManager (registry enumeration + hotkey based switching)
// ============================================================================
namespace VDManager {

    struct DesktopItem {
        int index = 0;
        GUID id{};
        std::wstring name;
    };

    struct DesktopList {
        int currentIndex = 0;
        std::vector<DesktopItem> items;
        int TotalCount() const { return (int)items.size(); }
    };

    struct WindowCard {
        std::wstring title;
        RECT rect{};
        HWND hwnd = nullptr;
        bool minimized = false;
    };

    static bool ReadDesktopsFromKey(const wchar_t* subKey, DesktopList& out, bool* pHaveCurrent) {
        HKEY hKey = nullptr;
        if (RegOpenKeyExW(HKEY_CURRENT_USER, subKey, 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
            return false;
        }

        bool ok = false;
        DWORD type = 0, size = 0;
        if (RegQueryValueExW(hKey, L"VirtualDesktopIDs", nullptr, &type, nullptr, &size) == ERROR_SUCCESS &&
            size >= sizeof(GUID)) {
            std::vector<BYTE> buf(size);
            if (RegQueryValueExW(hKey, L"VirtualDesktopIDs", nullptr, &type, buf.data(), &size) == ERROR_SUCCESS) {
                int count = (int)(size / sizeof(GUID));
                GUID* guids = (GUID*)buf.data();

                GUID cur{};
                DWORD curSize = sizeof(cur);
                bool haveCur = RegQueryValueExW(hKey, L"CurrentVirtualDesktop", nullptr, nullptr,
                                                (LPBYTE)&cur, &curSize) == ERROR_SUCCESS &&
                               curSize == sizeof(GUID);
                if (pHaveCurrent) *pHaveCurrent = haveCur;

                for (int i = 0; i < count; ++i) {
                    DesktopItem item;
                    item.index = i;
                    item.id = guids[i];
                    wchar_t name[64];
                    swprintf_s(name, L"Desktop %d", i + 1);
                    item.name = name;
                    out.items.push_back(item);
                    if (haveCur && IsEqualGUID(item.id, cur)) out.currentIndex = i;
                }
                ok = !out.items.empty();
            }
        }
        RegCloseKey(hKey);
        return ok;
    }

    static bool QueryDesktopsRegistry(DesktopList& out, bool& haveCurrent) {
        out.items.clear();
        out.currentIndex = 0;
        haveCurrent = false;

        DWORD session = 0;
        ProcessIdToSessionId(GetCurrentProcessId(), &session);
        wchar_t sessionKey[256];
        swprintf_s(sessionKey,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\SessionInfo\\%lu\\VirtualDesktops",
            session);

        bool ok = ReadDesktopsFromKey(sessionKey, out, &haveCurrent);
        if (!ok) {
            out.items.clear();
            out.currentIndex = 0;
            haveCurrent = false;
            ok = ReadDesktopsFromKey(
                L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\VirtualDesktops", out, &haveCurrent);
        }
        return ok;
    }

    static bool QueryDesktops(DesktopList& out) {
        bool haveCur = false;
        bool ok = QueryDesktopsRegistry(out, haveCur);

        if (!ok) {
            out.items.clear();
            DesktopItem item;
            item.index = 0;
            item.name = L"Desktop 1";
            out.items.push_back(item);
            out.currentIndex = 0;
        }

        // Cross-check the current desktop using the documented API on the foreground window.
        if (out.TotalCount() > 1) {
            com_ptr<IVirtualDesktopManager> vdm;
            if (SUCCEEDED(CoCreateInstance(CLSID_VirtualDesktopManager, nullptr, CLSCTX_INPROC_SERVER,
                                           IID_PPV_ARGS(vdm.put()))) && vdm) {
                HWND fg = GetForegroundWindow();
                GUID wid{};
                if (fg && SUCCEEDED(vdm->GetWindowDesktopId(fg, &wid))) {
                    for (const auto& it : out.items) {
                        if (IsEqualGUID(it.id, wid)) {
                            out.currentIndex = it.index;
                            break;
                        }
                    }
                }
            }
        }

        LOG_DBG(L"Desktops: total=%d current=%d", out.TotalCount(), out.currentIndex);
        return !out.items.empty();
    }

    static bool ReadCurrentIndex(int& idx) {
        DesktopList tmp;
        bool haveCur = false;
        if (QueryDesktopsRegistry(tmp, haveCur) && haveCur) {
            idx = tmp.currentIndex;
            return true;
        }
        return false;
    }

    static void SendSwitchStep(int dir) {
        WORD key = dir > 0 ? VK_RIGHT : VK_LEFT;
        INPUT in[6]{};
        for (auto& x : in) x.type = INPUT_KEYBOARD;
        in[0].ki.wVk = VK_LWIN;
        in[1].ki.wVk = VK_LCONTROL;
        in[2].ki.wVk = key;
        in[2].ki.dwFlags = KEYEVENTF_EXTENDEDKEY;
        in[3].ki.wVk = key;
        in[3].ki.dwFlags = KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;
        in[4].ki.wVk = VK_LCONTROL;
        in[4].ki.dwFlags = KEYEVENTF_KEYUP;
        in[5].ki.wVk = VK_LWIN;
        in[5].ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(6, in, sizeof(INPUT));
    }

    // Win+Ctrl+D: new desktop (Windows also switches to it). Win+Ctrl+F4: close the current desktop.
    static void SendWinCtrlKey(WORD vk) {
        INPUT in[6]{};
        for (auto& x : in) x.type = INPUT_KEYBOARD;
        in[0].ki.wVk = VK_LWIN;
        in[1].ki.wVk = VK_LCONTROL;
        in[2].ki.wVk = vk;
        in[3].ki.wVk = vk;
        in[3].ki.dwFlags = KEYEVENTF_KEYUP;
        in[4].ki.wVk = VK_LCONTROL;
        in[4].ki.dwFlags = KEYEVENTF_KEYUP;
        in[5].ki.wVk = VK_LWIN;
        in[5].ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(6, in, sizeof(INPUT));
    }
    static void SendAddDesktop() { SendWinCtrlKey('D'); }
    static void SendCloseDesktop() { SendWinCtrlKey(VK_F4); }

    static bool ReadTotalCount(int& total) {
        DesktopList tmp;
        bool haveCur = false;
        if (QueryDesktopsRegistry(tmp, haveCur)) { total = tmp.TotalCount(); return true; }
        return false;
    }

    // The user's "Animation effects" setting is never changed. While this flag is set, the SystemParametersInfoW hook
    // (see the taskbar hooking section) answers FALSE to SPI_GETCLIENTAREAANIMATION, which is the check explorer.exe makes
    // to decide whether the desktop slide animation plays.
    static std::atomic<bool> s_animSuppressed{ false };
    static void SuppressSystemAnimation(bool on) {
        if (s_animSuppressed.exchange(on) != on) {
            if (on) LOG_DBG(L"Desktop slide animation suppressed");
            else LOG_DBG(L"Desktop slide animation restored");
        }
    }

    static bool IsWindowOnCurrentDesktop(HWND h) {
        com_ptr<IVirtualDesktopManager> vdm;
        if (FAILED(CoCreateInstance(CLSID_VirtualDesktopManager, nullptr, CLSCTX_INPROC_SERVER,
                                    IID_PPV_ARGS(vdm.put()))) || !vdm) return true;
        BOOL on = TRUE;
        if (SUCCEEDED(vdm->IsWindowOnCurrentVirtualDesktop(h, &on))) return on != FALSE;
        return true;
    }

    struct EnumCardsCtx {
        const GUID* targetId;
        IVirtualDesktopManager* pVdm;
        std::vector<WindowCard>* pOut;
    };

    // Windows that belong to the shell / input stack (emoji panel, touch keyboard, Start, Search, overlays).
    // They are not real app windows and must never be painted onto a desktop face.
    static bool IsShellHelperWindow(HWND hWnd, const WCHAR* cls, const WCHAR* title, LONG exStyle) {
        static const wchar_t* kClasses[] = {
            L"Windows.UI.Core.CoreWindow", L"XamlExplorerHostIslandWindow", L"ForegroundStaging",
            L"NotifyIconOverflowWindow", L"TopLevelWindowForOverflowXamlIsland", L"Xaml_WindowedPopupClass",
            L"MultitaskingViewFrame", L"TaskListThumbnailWnd", L"TaskListOverlayWnd", L"ApplicationManager_ImmersiveShellWindow",
            L"IPTip_Main_Window", L"IPTip_Window", L"ImeUIWndTouch", L"TipbarGripper", L"TIPBand",
            L"Windows.UI.Input.InputSite.WindowClass", L"EdgeUiInputTopWndClass",
        };
        for (const wchar_t* k : kClasses) if (_wcsicmp(cls, k) == 0) return true;

        static const wchar_t* kTitles[] = {
            L"Windows Input Experience", L"Microsoft Text Input Application", L"Text input", L"Emoji",
            L"NVIDIA GeForce Overlay", L"Windows Shell Experience Host", L"Task Switching", L"Task View",
        };
        for (const wchar_t* t : kTitles) if (_wcsicmp(title, t) == 0) return true;

        // Click-through / non-activating windows that are not explicitly app windows are overlays, not apps.
        if ((exStyle & WS_EX_NOACTIVATE) && !(exStyle & WS_EX_APPWINDOW)) return true;
        if ((exStyle & WS_EX_TRANSPARENT) && (exStyle & WS_EX_LAYERED) && !(exStyle & WS_EX_APPWINDOW)) return true;

        DWORD pid = 0;
        GetWindowThreadProcessId(hWnd, &pid);
        if (pid) {
            HANDLE hp = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
            if (hp) {
                WCHAR path[MAX_PATH]{};
                DWORD n = MAX_PATH;
                bool hit = false;
                if (QueryFullProcessImageNameW(hp, 0, path, &n)) {
                    const WCHAR* base = wcsrchr(path, L'\\');
                    base = base ? base + 1 : path;
                    static const wchar_t* kProcs[] = {
                        L"TextInputHost.exe", L"SearchHost.exe", L"SearchApp.exe", L"StartMenuExperienceHost.exe",
                        L"ShellExperienceHost.exe", L"ShellHost.exe", L"LockApp.exe", L"ctfmon.exe", L"Widgets.exe",
                        L"WidgetService.exe", L"dwm.exe", L"SecurityHealthSystray.exe", L"PhoneExperienceHost.exe",
                    };
                    for (const wchar_t* pn : kProcs) if (_wcsicmp(base, pn) == 0) { hit = true; break; }
                }
                CloseHandle(hp);
                if (hit) return true;
            }
        }
        return false;
    }

    static BOOL CALLBACK EnumWindowsCardsProc(HWND hWnd, LPARAM lParam) {
        EnumCardsCtx* c = (EnumCardsCtx*)lParam;
        if (!IsWindowVisible(hWnd)) return TRUE;
        const bool iconic = IsIconic(hWnd) != 0;

        LONG exStyle = GetWindowLongW(hWnd, GWL_EXSTYLE);
        if (exStyle & WS_EX_TOOLWINDOW) return TRUE;

        DWORD cloaked = 0;
        if (SUCCEEDED(DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked)))) {
            // 2 = DWM_CLOAKED_SHELL (hidden because it lives on another virtual desktop)
            if (cloaked != 0 && cloaked != 2) return TRUE;
        }

        RECT rc{};
        if (iconic) {
            WINDOWPLACEMENT wp{};
            wp.length = sizeof(wp);
            if (!GetWindowPlacement(hWnd, &wp)) return TRUE;
            rc = wp.rcNormalPosition;
        } else if (FAILED(DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rc, sizeof(rc)))) {
            GetWindowRect(hWnd, &rc);
        }
        if ((rc.right - rc.left) < 100 || (rc.bottom - rc.top) < 100) return TRUE;

        if (!c->pVdm) return TRUE;   // cannot tell which desktop it is on: show nothing rather than the wrong thing
        {
            // A window whose desktop cannot be determined (or is "none") used to be painted onto EVERY desktop.
            GUID wId{};
            if (FAILED(c->pVdm->GetWindowDesktopId(hWnd, &wId))) return TRUE;
            if (IsEqualGUID(wId, GUID{}) || !IsEqualGUID(wId, *c->targetId)) return TRUE;
        }

        WCHAR title[256]{};
        GetWindowTextW(hWnd, title, ARRAYSIZE(title));
        if (wcslen(title) == 0) return TRUE;

        WCHAR cls[128]{};
        GetClassNameW(hWnd, cls, ARRAYSIZE(cls));
        if (IsShellHelperWindow(hWnd, cls, title, exStyle)) return TRUE;
        if (_wcsicmp(cls, L"Progman") == 0 || _wcsicmp(cls, L"WorkerW") == 0 ||
            _wcsicmp(cls, L"Shell_TrayWnd") == 0 || _wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0 ||
            _wcsicmp(cls, L"CompizDesktopCubeOverlayWindow") == 0) {
            return TRUE;
        }

        WindowCard card;
        card.title = title;
        card.rect = rc;
        card.hwnd = hWnd;
        card.minimized = iconic;
        c->pOut->push_back(card);
        return c->pOut->size() < 16;
    }

    // Windows are returned top-most first.
    static void GetWindowsForDesktop(const GUID& desktopId, std::vector<WindowCard>& outCards) {
        outCards.clear();
        com_ptr<IVirtualDesktopManager> vdm;
        CoCreateInstance(CLSID_VirtualDesktopManager, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(vdm.put()));
        EnumCardsCtx ctx = { &desktopId, vdm.get(), &outCards };
        EnumWindows(EnumWindowsCardsProc, (LPARAM)&ctx);
    }

} // namespace VDManager

// ============================================================================
// MODULE 2: DesktopCaptureManager (shared snapshots, versioned)
// ============================================================================
namespace DesktopCaptureManager {

    struct SnapshotData {
        int width = 0;
        int height = 0;
        std::vector<uint32_t> pixels;   // BGRA, alpha forced to 0xFF
        bool composed = false;          // true: rebuilt from wallpaper + window captures
        ULONGLONG tick = 0;
    };
    using SnapPtr = std::shared_ptr<const SnapshotData>;

    struct Entry {
        SnapPtr data;
        uint64_t version = 0;
    };

    static std::mutex s_mutex;
    static std::unordered_map<std::wstring, Entry> s_cache;
    static uint64_t s_versionCounter = 0;

    // Faces are never drawn wider than this (the texture quality cap) and the background uses a quarter scale,
    // so stored snapshots never need more pixels than this.
    static constexpr int kSnapMaxW = 1920;

    // Box-filter downscale of a BGRA image (tw <= w, th <= h). Alpha is forced to 0xFF.
    static void BoxDownscale(const uint32_t* src, int w, int h, std::vector<uint32_t>& dst, int tw, int th) {
        dst.resize((size_t)tw * th);
        std::vector<int> xs((size_t)tw + 1);
        for (int x = 0; x <= tw; ++x) xs[x] = (int)((int64_t)x * w / tw);
        for (int y = 0; y < th; ++y) {
            const int y0 = (int)((int64_t)y * h / th);
            int y1 = (int)((int64_t)(y + 1) * h / th);
            if (y1 <= y0) y1 = y0 + 1;
            if (y1 > h) y1 = h;
            uint32_t* d = &dst[(size_t)y * tw];
            for (int x = 0; x < tw; ++x) {
                const int x0 = xs[x];
                int x1 = xs[x + 1];
                if (x1 <= x0) x1 = x0 + 1;
                if (x1 > w) x1 = w;
                uint32_t b = 0, g = 0, r = 0;
                for (int yy = y0; yy < y1; ++yy) {
                    const uint32_t* row = src + (size_t)yy * w;
                    for (int xx = x0; xx < x1; ++xx) {
                        const uint32_t px = row[xx];
                        b += px & 0xFFu; g += (px >> 8) & 0xFFu; r += (px >> 16) & 0xFFu;
                    }
                }
                const uint32_t n = (uint32_t)((y1 - y0) * (x1 - x0));
                d[x] = 0xFF000000u | ((r / n) << 16) | ((g / n) << 8) | (b / n);
            }
        }
    }

    // Nearest-neighbour scale (used to expand the cached, reduced wallpaper back to the monitor size).
    static void NearestScale(const uint32_t* src, int sw, int sh, std::vector<uint32_t>& dst, int w, int h) {
        dst.resize((size_t)w * h);
        std::vector<int> xmap((size_t)w);
        for (int x = 0; x < w; ++x) xmap[x] = (int)((int64_t)x * sw / w);
        for (int y = 0; y < h; ++y) {
            const uint32_t* row = src + (size_t)((int64_t)y * sh / h) * sw;
            uint32_t* d = &dst[(size_t)y * w];
            for (int x = 0; x < w; ++x) d[x] = row[xmap[x]];
        }
    }

    // Copies a w*h BGRA image into a snapshot that is at most kSnapMaxW wide (alpha forced to 0xFF).
    static std::shared_ptr<SnapshotData> MakeCompactSnapshot(const uint32_t* px, int w, int h, bool composed, ULONGLONG tick) {
        if (!px || w <= 0 || h <= 0) return nullptr;
        auto out = std::make_shared<SnapshotData>();
        out->composed = composed;
        out->tick = tick;
        if (w > kSnapMaxW) {
            const int tw = kSnapMaxW;
            const int th = std::max(1, (int)((int64_t)h * tw / w));
            out->width = tw;
            out->height = th;
            BoxDownscale(px, w, h, out->pixels, tw, th);
        } else {
            out->width = w;
            out->height = h;
            out->pixels.assign(px, px + (size_t)w * h);
            for (auto& v : out->pixels) v |= 0xFF000000u;
        }
        return out;
    }

    static std::wstring GuidToString(const GUID& id) {
        wchar_t sz[48]{};
        swprintf_s(sz, L"{%08lX-%04hX-%04hX-%02X%02X-%02X%02X%02X%02X%02X%02X}",
            id.Data1, id.Data2, id.Data3,
            (unsigned)id.Data4[0], (unsigned)id.Data4[1], (unsigned)id.Data4[2], (unsigned)id.Data4[3],
            (unsigned)id.Data4[4], (unsigned)id.Data4[5], (unsigned)id.Data4[6], (unsigned)id.Data4[7]);
        return sz;
    }

    static std::shared_ptr<SnapshotData> CaptureMonitorScreen(RECT rcMon) {
        int w = rcMon.right - rcMon.left;
        int h = rcMon.bottom - rcMon.top;
        if (w <= 0 || h <= 0) return nullptr;

        HDC hdcScreen = GetDC(nullptr);
        if (!hdcScreen) return nullptr;
        HDC hdcMem = CreateCompatibleDC(hdcScreen);
        if (!hdcMem) { ReleaseDC(nullptr, hdcScreen); return nullptr; }

        BITMAPINFO bmi{};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = w;
        bmi.bmiHeader.biHeight = -h;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;

        void* bits = nullptr;
        HBITMAP dib = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
        if (!dib || !bits) {
            DeleteDC(hdcMem);
            ReleaseDC(nullptr, hdcScreen);
            return nullptr;
        }

        HGDIOBJ old = SelectObject(hdcMem, dib);
        BitBlt(hdcMem, 0, 0, w, h, hdcScreen, rcMon.left, rcMon.top, SRCCOPY | CAPTUREBLT);

        auto out = std::make_shared<SnapshotData>();
        out->width = w;
        out->height = h;
        out->pixels.resize((size_t)w * h);
        memcpy(out->pixels.data(), bits, (size_t)w * h * sizeof(uint32_t));
        for (auto& px : out->pixels) px |= 0xFF000000;
        out->composed = false;
        out->tick = GetTickCount64();

        SelectObject(hdcMem, old);
        DeleteObject(dib);
        DeleteDC(hdcMem);
        ReleaseDC(nullptr, hdcScreen);
        return out;
    }

    static void Store(const GUID& id, std::shared_ptr<SnapshotData> d) {
        if (!d) return;
        // The caller may hand in a full-resolution capture (it keeps its own copy for the live feed's first frame).
        // The cache only ever holds the compact version.
        if (d->width > kSnapMaxW && !d->pixels.empty()) {
            auto small = MakeCompactSnapshot(d->pixels.data(), d->width, d->height, d->composed, d->tick);
            if (small) d = small;
        }
        std::lock_guard<std::mutex> lock(s_mutex);
        Entry& e = s_cache[GuidToString(id)];
        // A recent exact screenshot beats a rebuilt one.
        if (e.data && !e.data->composed && d->composed &&
            (GetTickCount64() - e.data->tick) < 120000ULL) {
            return;
        }
        e.data = d;
        e.version = ++s_versionCounter;
    }

    static SnapPtr Get(const GUID& id, uint64_t* ver = nullptr) {
        std::lock_guard<std::mutex> lock(s_mutex);
        auto it = s_cache.find(GuidToString(id));
        if (it != s_cache.end() && it->second.data && !it->second.data->pixels.empty()) {
            if (ver) *ver = it->second.version;
            return it->second.data;
        }
        if (ver) *ver = 0;
        return nullptr;
    }

    static uint64_t Version(const GUID& id) {
        std::lock_guard<std::mutex> lock(s_mutex);
        auto it = s_cache.find(GuidToString(id));
        return it != s_cache.end() ? it->second.version : 0;
    }

    static void Clear() {
        std::lock_guard<std::mutex> lock(s_mutex);
        s_cache.clear();
    }

    // Drops snapshots of desktops that no longer exist and snapshots older than maxAgeMs.
    static void Prune(const VDManager::DesktopList& list, ULONGLONG maxAgeMs) {
        std::lock_guard<std::mutex> lock(s_mutex);
        const ULONGLONG now = GetTickCount64();
        for (auto it = s_cache.begin(); it != s_cache.end();) {
            bool keep = false;
            for (const auto& item : list.items) {
                if (GuidToString(item.id) == it->first) { keep = true; break; }
            }
            if (keep && it->second.data && (now - it->second.data->tick) > maxAgeMs) keep = false;
            if (keep) ++it; else it = s_cache.erase(it);
        }
    }

} // namespace DesktopCaptureManager

// ============================================================================
// MODULE 2b: Wallpaper loader (WIC) and desktop composer (PrintWindow)
// ============================================================================
namespace WallpaperLoader {

    static std::mutex s_mtx;
    static std::wstring s_cachePath;
    static int s_cw = 0, s_ch = 0;       // size that was requested
    static int s_sw = 0, s_sh = 0;       // size of the stored (reduced) copy
    static std::vector<uint32_t> s_cache;

    static int GetStyle(bool& tile) {
        tile = false;
        int style = 10;
        HKEY h = nullptr;
        if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\Desktop", 0, KEY_READ, &h) == ERROR_SUCCESS) {
            wchar_t buf[16]{};
            DWORD sz = sizeof(buf);
            if (RegQueryValueExW(h, L"WallpaperStyle", nullptr, nullptr, (LPBYTE)buf, &sz) == ERROR_SUCCESS) {
                style = _wtoi(buf);
            }
            wchar_t tbuf[16]{};
            sz = sizeof(tbuf);
            if (RegQueryValueExW(h, L"TileWallpaper", nullptr, nullptr, (LPBYTE)tbuf, &sz) == ERROR_SUCCESS) {
                tile = _wtoi(tbuf) != 0;
            }
            RegCloseKey(h);
        }
        return style;
    }

    // Fills out with a w*h BGRA image of the wallpaper (or a solid desktop colour).
    static bool Load(int w, int h, std::vector<uint32_t>& out) {
        COLORREF bg = GetSysColor(COLOR_DESKTOP);
        uint32_t bgPx = 0xFF000000u | (GetRValue(bg) << 16) | (GetGValue(bg) << 8) | GetBValue(bg);
        out.assign((size_t)w * h, bgPx);

        WCHAR path[MAX_PATH]{};
        if (!SystemParametersInfoW(SPI_GETDESKWALLPAPER, MAX_PATH, path, 0) || !path[0]) return false;

        {
            std::lock_guard<std::mutex> lk(s_mtx);
            if (s_cw == w && s_ch == h && s_cachePath == path && !s_cache.empty()) {
                if (s_sw == w && s_sh == h) out = s_cache;
                else DesktopCaptureManager::NearestScale(s_cache.data(), s_sw, s_sh, out, w, h);
                return true;
            }
        }

        bool tile = false;
        int style = GetStyle(tile);

        com_ptr<IWICImagingFactory> fac;
        if (FAILED(CoCreateInstance(kClsidWICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                                    IID_PPV_ARGS(fac.put()))) || !fac) return false;

        com_ptr<IWICBitmapDecoder> dec;
        if (FAILED(fac->CreateDecoderFromFilename(path, nullptr, GENERIC_READ,
                                                  WICDecodeMetadataCacheOnDemand, dec.put())) || !dec) return false;
        com_ptr<IWICBitmapFrameDecode> frame;
        if (FAILED(dec->GetFrame(0, frame.put())) || !frame) return false;

        UINT iw = 0, ih = 0;
        frame->GetSize(&iw, &ih);
        if (iw == 0 || ih == 0) return false;

        double sx = (double)w / iw, sy = (double)h / ih;
        UINT tw = iw, th = ih;
        if (style == 2) {                 // stretch
            tw = (UINT)w; th = (UINT)h;
        } else if (style == 6) {          // fit
            double s = std::min(sx, sy);
            tw = std::max<UINT>(1, (UINT)std::lround(iw * s));
            th = std::max<UINT>(1, (UINT)std::lround(ih * s));
        } else if (style == 10 || style == 22) {   // fill / span
            double s = std::max(sx, sy);
            tw = std::max<UINT>(1, (UINT)std::lround(iw * s));
            th = std::max<UINT>(1, (UINT)std::lround(ih * s));
        }                                 // else: center, no scaling

        com_ptr<IWICBitmapSource> src = frame;
        com_ptr<IWICBitmapScaler> scaler;
        if (tw != iw || th != ih) {
            if (SUCCEEDED(fac->CreateBitmapScaler(scaler.put())) && scaler &&
                SUCCEEDED(scaler->Initialize(frame.get(), tw, th, WICBitmapInterpolationModeFant))) {
                src = scaler;
            } else {
                return false;
            }
        }

        com_ptr<IWICFormatConverter> conv;
        if (FAILED(fac->CreateFormatConverter(conv.put())) || !conv) return false;
        if (FAILED(conv->Initialize(src.get(), kPixelFormat32bppBGR, WICBitmapDitherTypeNone,
                                    nullptr, 0.0, WICBitmapPaletteTypeCustom))) return false;

        int dx = ((int)w - (int)tw) / 2;
        int dy = ((int)h - (int)th) / 2;
        int srcX = std::max(0, -dx), srcY = std::max(0, -dy);
        int dstX = std::max(0, dx), dstY = std::max(0, dy);
        int cw = std::min((int)tw - srcX, w - dstX);
        int ch = std::min((int)th - srcY, h - dstY);
        if (cw <= 0 || ch <= 0) return false;

        std::vector<uint32_t> tmp((size_t)cw * ch);
        WICRect rc{ srcX, srcY, cw, ch };
        if (FAILED(conv->CopyPixels(&rc, cw * 4, (UINT)(tmp.size() * 4), (BYTE*)tmp.data()))) return false;

        for (int y = 0; y < ch; ++y) {
            uint32_t* d = &out[(size_t)(dstY + y) * w + dstX];
            const uint32_t* s = &tmp[(size_t)y * cw];
            for (int x = 0; x < cw; ++x) d[x] = s[x] | 0xFF000000u;
        }

        {
            // Keep only a reduced copy (at most 1920 px wide) between gestures, not a full-resolution one.
            std::vector<uint32_t> reduced;
            int rw = w, rh = h;
            if (w > DesktopCaptureManager::kSnapMaxW) {
                rw = DesktopCaptureManager::kSnapMaxW;
                rh = std::max(1, (int)((int64_t)h * rw / w));
                DesktopCaptureManager::BoxDownscale(out.data(), w, h, reduced, rw, rh);
            } else {
                reduced = out;
            }
            std::lock_guard<std::mutex> lk(s_mtx);
            s_cachePath = path;
            s_cw = w; s_ch = h;
            s_sw = rw; s_sh = rh;
            s_cache = std::move(reduced);
        }
        return true;
    }

    static void ClearCache() {
        std::lock_guard<std::mutex> lk(s_mtx);
        std::vector<uint32_t>().swap(s_cache);
        s_cachePath.clear();
        s_cw = s_ch = s_sw = s_sh = 0;
    }

} // namespace WallpaperLoader

namespace Composer {

    static std::atomic<bool> s_abort{ false };

    struct Job {
        RECT mon{};
        VDManager::DesktopList list;
    };

    static bool LooksBlank(const uint32_t* bits, int w, int h) {
        int nonBlack = 0;
        for (int gy = 0; gy < 12; ++gy)
            for (int gx = 0; gx < 12; ++gx) {
                int x = (w - 1) * gx / 11, y = (h - 1) * gy / 11;
                if ((bits[(size_t)y * w + x] & 0x00FFFFFFu) != 0) ++nonBlack;
            }
        return nonBlack == 0;
    }

    static std::shared_ptr<DesktopCaptureManager::SnapshotData> ComposeDesktop(
        const Job& job, const std::vector<uint32_t>& wall, const GUID& id) {

        const int w = job.mon.right - job.mon.left;
        const int h = job.mon.bottom - job.mon.top;

        std::vector<VDManager::WindowCard> cards;
        VDManager::GetWindowsForDesktop(id, cards);

        HDC hdcScreen = GetDC(nullptr);
        if (!hdcScreen) return nullptr;
        HDC mem = CreateCompatibleDC(hdcScreen);
        BITMAPINFO bmi{};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = w;
        bmi.bmiHeader.biHeight = -h;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        void* bits = nullptr;
        HBITMAP dib = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
        if (!mem || !dib || !bits) {
            if (dib) DeleteObject(dib);
            if (mem) DeleteDC(mem);
            ReleaseDC(nullptr, hdcScreen);
            return nullptr;
        }
        HGDIOBJ old = SelectObject(mem, dib);

        if (wall.size() == (size_t)w * h) memcpy(bits, wall.data(), (size_t)w * h * 4);
        else memset(bits, 0x20, (size_t)w * h * 4);

        int captured = 0;
        for (auto it = cards.rbegin(); it != cards.rend(); ++it) {
            if (s_abort.load()) break;
            if (it->minimized) continue;
            if (IsHungAppWindow(it->hwnd)) continue;   // PrintWindow would block on it, and unload waits for this thread

            RECT wr{};
            if (!GetWindowRect(it->hwnd, &wr)) continue;
            RECT fb = it->rect;     // visible frame bounds
            int ww = wr.right - wr.left, wh = wr.bottom - wr.top;
            if (ww <= 0 || wh <= 0 || ww > 8192 || wh > 8192) continue;

            HDC hw = CreateCompatibleDC(hdcScreen);
            BITMAPINFO wb{};
            wb.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            wb.bmiHeader.biWidth = ww;
            wb.bmiHeader.biHeight = -wh;
            wb.bmiHeader.biPlanes = 1;
            wb.bmiHeader.biBitCount = 32;
            wb.bmiHeader.biCompression = BI_RGB;
            void* wbits = nullptr;
            HBITMAP wdib = CreateDIBSection(hdcScreen, &wb, DIB_RGB_COLORS, &wbits, nullptr, 0);
            if (!hw || !wdib || !wbits) {
                if (wdib) DeleteObject(wdib);
                if (hw) DeleteDC(hw);
                continue;
            }
            HGDIOBJ wold = SelectObject(hw, wdib);
            memset(wbits, 0, (size_t)ww * wh * 4);

            BOOL ok = PrintWindow(it->hwnd, hw, PW_RENDERFULLCONTENT);
            bool blank = !ok || LooksBlank((const uint32_t*)wbits, ww, wh);

            int dstX = fb.left - job.mon.left;
            int dstY = fb.top - job.mon.top;
            int fw = fb.right - fb.left, fh = fb.bottom - fb.top;

            if (!blank) {
                int sx = fb.left - wr.left, sy = fb.top - wr.top;
                if (sx < 0) sx = 0;
                if (sy < 0) sy = 0;
                BitBlt(mem, dstX, dstY, fw, fh, hw, sx, sy, SRCCOPY);
                ++captured;
            }
            // else: PrintWindow could not render this app. Leave the wallpaper visible
            // instead of drawing a black placeholder card.

            SelectObject(hw, wold);
            DeleteObject(wdib);
            DeleteDC(hw);
        }

        auto out = DesktopCaptureManager::MakeCompactSnapshot((const uint32_t*)bits, w, h, true, GetTickCount64());

        SelectObject(mem, old);
        DeleteObject(dib);
        DeleteDC(mem);
        ReleaseDC(nullptr, hdcScreen);
        LOG_DBG(L"Composed desktop: %d windows captured of %d", captured, (int)cards.size());
        return out;
    }

    static DWORD WINAPI Proc(LPVOID p) {
        std::unique_ptr<Job> job((Job*)p);
        bool com = SUCCEEDED(CoInitializeEx(nullptr, COINIT_MULTITHREADED));
        SetThreadPriority(GetCurrentThread(), THREAD_MODE_BACKGROUND_BEGIN);   // low CPU, I/O and memory priority

        try {
            const int w = job->mon.right - job->mon.left;
            const int h = job->mon.bottom - job->mon.top;
            std::vector<uint32_t> wall;
            WallpaperLoader::Load(w, h, wall);

            // Desktops next to the current one first.
            int total = job->list.TotalCount();
            std::vector<int> order;
            for (int dstep = 1; dstep < total; ++dstep) {
                int a = (job->list.currentIndex + dstep) % total;
                int b = ((job->list.currentIndex - dstep) % total + total) % total;
                if (std::find(order.begin(), order.end(), a) == order.end()) order.push_back(a);
                if (std::find(order.begin(), order.end(), b) == order.end()) order.push_back(b);
            }

            for (int idx : order) {
                if (s_abort.load()) break;
                if (idx == job->list.currentIndex) continue;
                const GUID& id = job->list.items[idx].id;
                auto existing = DesktopCaptureManager::Get(id);
                if (existing && (GetTickCount64() - existing->tick) <
                                    (existing->composed ? 20000ULL : 120000ULL)) continue;   // fresh enough, skip the expensive re-capture

                auto snap = ComposeDesktop(*job, wall, id);
                if (snap) DesktopCaptureManager::Store(id, snap);
            }
        } catch (...) {
        }

        SetThreadPriority(GetCurrentThread(), THREAD_MODE_BACKGROUND_END);
        if (com) CoUninitialize();
        return 0;
    }

    static HANDLE Start(std::unique_ptr<Job> job) {
        return CreateThread(nullptr, 0, Proc, job.release(), 0, nullptr);
    }

} // namespace Composer

// ============================================================================
// Gesture state + cross-module forward declarations
// ============================================================================
enum class GestureState { Idle, Arming, ActiveDrag, Committing };

namespace GestureController {
    void OnSnapCompleted();
    void OnCancel();
    GestureState GetState();
    void AttachHooks();
    void DetachHooks();
    void ResyncMouse();
    void OnActiveMouseUp();
    void RescaleYaw(float k);
}

// ============================================================================
// Direct2D helpers
// ============================================================================
template <typename F>
static bool DrawToSurface(CompositionDrawingSurface const& surface, F&& fn) {
    if (!surface) return false;
    ICompositionDrawingSurfaceInteropX* si = nullptr;
    IUnknown* unk = (IUnknown*)winrt::get_abi(surface);
    if (FAILED(unk->QueryInterface(IID_ICompositionDrawingSurfaceInteropX, (void**)&si)) || !si) {
        return false;
    }

    bool ok = false;
    ID2D1DeviceContext* ctx = nullptr;
    POINT offset{};
    HRESULT hr = si->BeginDraw(nullptr, __uuidof(ID2D1DeviceContext), (void**)&ctx, &offset);
    if (SUCCEEDED(hr) && ctx) {
        ctx->SetDpi(96.0f, 96.0f);
        fn(ctx, offset);
        ctx->SetTransform(D2D1::Matrix3x2F::Identity());
        si->EndDraw();
        ctx->Release();
        ok = true;
    } else {
        LOG_ERR(L"BeginDraw failed 0x%08X", (unsigned)hr);
    }
    si->Release();
    return ok;
}

static bool CreateBitmapFromSnapshot(ID2D1DeviceContext* ctx,
                                     const DesktopCaptureManager::SnapshotData& snap,
                                     ID2D1Bitmap1** outBitmap) {
    D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_NONE,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
        96.0f, 96.0f);
    return SUCCEEDED(ctx->CreateBitmap(
        D2D1::SizeU(snap.width, snap.height),
        snap.pixels.data(),
        snap.width * sizeof(uint32_t),
        props,
        outBitmap));
}

struct FaceColor { uint8_t r, g, b; };
static const FaceColor kFaceAccents[kMaxFaces] = {
    { 56, 140, 255 },   // blue
    { 168, 85, 247 },   // purple
    { 249, 115, 22 },   // orange
    { 20, 184, 166 },   // teal
    { 236, 72, 153 },   // pink
    { 234, 179, 8 }     // amber
};

// ============================================================================
// MODULE 3: CubeRenderer
//
// One PERSISTENT render thread (window, compositor, D3D/D2D devices stay warm between
// gestures). Per gesture it builds the scene, runs a vblank-locked loop and tears the scene down.
// Mouse input is read on this thread (not on the busy taskbar thread).
// ============================================================================
static const float kBadgeW = 250.0f;
static const int kMaxAlt = 12;

// One live thumbnail card per window of the switcher.
struct AltSurf {
    CompositionDrawingSurface surf{ nullptr };
    RECT box{};          // window area inside the work-area texture (pixels)
    int w = 0, h = 0;    // thumbnail size
    bool ok = false;
    // Real per-window capture (PrintWindow), used when another window covers this one on screen
    std::vector<BYTE> snap;
    int snapW = 0, snapH = 0;
    RECT snapSrc{};
    ULONGLONG snapTick = 0;
    bool snapValid = false;
};

struct AltWin {
    HWND hwnd = nullptr;
    RECT r{};                 // visible part on this monitor (screen coordinates)
    std::wstring title;
};

// Brings a window to the front like Alt+Tab does.
static void ActivateWindowNow(HWND h) {
    if (!h || !IsWindow(h)) return;
    if (IsIconic(h)) ShowWindow(h, SW_RESTORE);
    // Allows to steal focus. An empty SendInput right before SetForegroundWindow is the supported way
    // (no AttachThreadInput: attached threads share one input queue, so a hung app would freeze this thread).
    INPUT input{};
    SendInput(1, &input, sizeof(INPUT));
    BringWindowToTop(h);
    SetForegroundWindow(h);
}

// True when a window above this one in the Alt+Tab order overlaps it, so a screen crop would show the wrong content.
static bool AltIsOccluded(const std::vector<AltWin>& v, int i) {
    for (int j = 0; j < i && j < (int)v.size(); ++j) {
        RECT x{};
        if (IntersectRect(&x, &v[j].r, &v[i].r)) return true;
    }
    return false;
}

// Captures the window's own pixels (works even when it is covered). Returns false if the app is hung or the capture fails.
static bool CaptureWindowPixels(HWND hw, const RECT& visible, AltSurf& s) {
    if (!hw || !IsWindow(hw) || IsHungAppWindow(hw)) return false;
    RECT wr{};
    if (!GetWindowRect(hw, &wr)) return false;
    const int bw = wr.right - wr.left, bh = wr.bottom - wr.top;
    if (bw < 16 || bh < 16 || bw > 8192 || bh > 8192) return false;

    HDC scr = GetDC(nullptr);
    HDC mem = CreateCompatibleDC(scr);
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = bw;
    bi.bmiHeader.biHeight = -bh;   // top-down
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP dib = CreateDIBSection(scr, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    bool ok = false;
    if (dib && bits && mem) {
        HGDIOBJ old = SelectObject(mem, dib);
        memset(bits, 0, (size_t)bw * bh * 4);
        ok = PrintWindow(hw, mem, 0x00000002 /*PW_RENDERFULLCONTENT*/) != 0;
        SelectObject(mem, old);
        if (ok) {
            s.snap.assign((BYTE*)bits, (BYTE*)bits + (size_t)bw * bh * 4);
            s.snapW = bw; s.snapH = bh;
            s.snapSrc = { std::clamp((int)(visible.left - wr.left), 0, bw), std::clamp((int)(visible.top - wr.top), 0, bh),
                          std::clamp((int)(visible.right - wr.left), 0, bw), std::clamp((int)(visible.bottom - wr.top), 0, bh) };
            s.snapValid = (s.snapSrc.right - s.snapSrc.left >= 16 && s.snapSrc.bottom - s.snapSrc.top >= 16);
            ok = s.snapValid;
        }
    }
    if (dib) DeleteObject(dib);
    if (mem) DeleteDC(mem);
    if (scr) ReleaseDC(nullptr, scr);
    return ok;
}

// Hides the taskbar from every capture API (the mod runs inside explorer.exe, so the window is ours to flag).
// The live feed and the snapshots then show the plain desktop where the taskbar is, so no cropping is needed.
static std::vector<HWND> g_excludedTaskbars;
static bool ExcludeTaskbarsFromCapture() {
    std::vector<HWND> list;
    if (HWND t = FindWindowW(L"Shell_TrayWnd", nullptr)) list.push_back(t);
    HWND sec = nullptr;
    while ((sec = FindWindowExW(nullptr, sec, L"Shell_SecondaryTrayWnd", nullptr)) != nullptr) list.push_back(sec);
    if (list.empty()) return false;
    bool all = true;
    for (HWND h : list) {
        if (SetWindowDisplayAffinity(h, WDA_EXCLUDEFROMCAPTURE)) g_excludedTaskbars.push_back(h);
        else all = false;
    }
    return all;
}
static void RestoreTaskbarCapture() {
    for (HWND h : g_excludedTaskbars) if (IsWindow(h)) SetWindowDisplayAffinity(h, 0 /*WDA_NONE*/);
    g_excludedTaskbars.clear();
}

static const float kBadgeH = 24.0f;

class CubeRenderer {
public:
    // The one instance lives in g_renderer (below), created in Wh_ModInit and destroyed in Wh_ModUninit.
    static CubeRenderer& Instance();

    CubeRenderer() = default;
    ~CubeRenderer() = default;

    void Prewarm();
    bool Start(POINT anchorPoint, float userZoom);
    void SubmitInput(float yawDeg, float pitchDeg, float shrink01) {
        m_inYaw.store(yawDeg);
        m_inPitch.store(pitchDeg);
        m_inZoom.store(shrink01);
        Wake();
    }
    void SubmitUserZoom(float z) { m_inUserZoom.store(z); Wake(); }
    void SubmitWheelSelect(int steps) { m_selSteps.fetch_add(steps); Wake(); }
    void Commit(float flickVelDegPerSec) {
        m_inFlick.store(flickVelDegPerSec);
        m_cmd.store(1);
        Wake();
    }
    void Cancel() { m_cmd.store(2); Wake(); }
    // Right click while the cube is open. Returns true when the click hit an edge zone (and must be swallowed).
    bool OnRightDown(POINT pt);
    // Leaves the idle (power saving) pace right away.
    void Wake() {
        // Only the idle (30 Hz) pacer waits on this event. At full rate the call is two atomic ops, no syscall.
        if (m_idle.exchange(false) && m_wakeEvent) SetEvent(m_wakeEvent);
    }
    void Shutdown();
    bool IsActive() const { return m_sceneActive.load(); }

private:
    // thread plumbing
    bool EnsureThread();
    static DWORD WINAPI ThreadEntry(LPVOID p);
    static DWORD WINAPI PacerEntry(LPVOID p);
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    DWORD ThreadMain();
    void PacerLoop();
    bool WarmInit();
    void Teardown();
    void RunScene();
    bool BeginScene();
    void EndScene();
    void RunLoop();
    bool Frame();
    void UpdateIdle();

    // ---- add / remove desktops, edge zones, effects ----
    void ComputeGeometry(int total);
    void BuildFaceVisuals();
    void RebuildPrism();
    void BuildOverlayFx();
    void DrawZoneSurface(bool right);
    void ShowToast(const wchar_t* text);
    void StepFx(float dt);
    void StartVdOp(int kind);
    void UpdateVdOp();
    void FinishVdOp(bool ok);
    void ApplyDesktopCountChange(const VDManager::DesktopList& nl, int oldTotal);
    void KickComposer();

    // setup
    bool EnsureDispatcherQueue();
    bool InitializeGraphicsDevice();
    void FindOutput();
    CompositionDrawingSurface MakeSurface(int w, int h);
    void BuildVisuals();
    void BuildBackground();
    ID2D1Bitmap1* GetDesktopBitmap(ID2D1DeviceContext* ctx, int dIdx);

    // drawing
    void RenderFace(int i, int dIdx);
    void RenderShadow();
    ID2D1Bitmap1* FaceBitmap(ID2D1DeviceContext* ctx, int dIdx);

    // live capture
    bool StartLive(const DesktopCaptureManager::SnapshotData* initial);
    void StopLive();
    bool CreateDuplication(int* outW, int* outH);
    bool PollLive(bool wantTex);
    void DistributeFrame(ID3D11Texture2D* src, bool wantTex);
    void DrawLive();
    bool CreateCompSwapChain(int w, int h, IDXGISwapChain1** outSc, ICompositionSurface* outSurf);
    void ComputeWorkArea(const RECT& mon, const RECT& work);

    // alt+tab style switcher
    void CollectAltWindows();
    void BuildAltVisuals();
    bool EnsureAltSurfaces();
    void DrawThumb(int i);
    void UpdateThumbs();
    void UpdateAltTab(float dt);
    void LayoutAlt(const Mat4& view, float faceDeg, float lifeEase);
    void HideAlt();
    void RedrawLabel();

    // animation
    float ComputeYawTarget(float rawYaw) const;
    void BeginCommit(bool cancel);
    void UpdateSwitching();
    void ApplyTransforms();
    int DesktopForOffset(int k) const;

    // ---- persistent thread / sync ----
    std::mutex m_threadMutex;
    HANDLE m_thread = nullptr;
    HANDLE m_goEvent = nullptr;
    HANDLE m_quitEvent = nullptr;
    HANDLE m_frameEvent = nullptr;
    HANDLE m_wakeEvent = nullptr;

    struct ZoneVisual {
        SpriteVisual vis{ nullptr };
        CompositionDrawingSurface surf{ nullptr };
        float op = 0.0f;
        float pulse = 0.0f;
        float baseX = 0.0f;     // resting x offset of the visual
        bool  shown = false;    // revealed by the pointer (read by the mouse hook, benign race)
    };
    ZoneVisual m_zoneL, m_zoneR;
    SpriteVisual m_toast{ nullptr };
    CompositionDrawingSurface m_toastSurface{ nullptr };
    float m_toastT = 0.0f;                  // 1 -> 0 while the toast is shown
    float m_zoneW = 192.0f;
    std::chrono::steady_clock::time_point m_zoneQuietUntil{};
    std::chrono::steady_clock::time_point m_vdCooldown{};
    std::atomic<int> m_pendingVdOp{ 0 };   // 0 none, 1 add, 2 remove. Set by the mouse hook, consumed in Frame().
    Spring m_kick, m_pop;                   // spin and pop when the prism is rebuilt
    int   m_bornDesktop = -1;
    float m_bornT = 1.0f;
    bool  m_fxDirty = false;
    bool  m_composeRestart = false;
    struct VdTask {
        bool active = false;
        int kind = 0;                       // 1 add, 2 remove
        int stage = 0;                      // 0 go to start desktop, 1 act, 2 wait for count, 3 go back
        int oldTotal = 0, origCur = 0, goalA = 0, goalC = 0;
        bool waiting = false;
        int waitCur = -1;
        std::chrono::steady_clock::time_point nextAt, deadline, waitDeadline, actDeadline;
    } m_vd;
    std::atomic<bool> m_idle{ false };      // pacer runs at a low rate while nothing moves
    int  m_idleFrames = 0;
    bool m_busy = false;                    // something changed in this frame
    bool m_presentPending = false;          // a live Present was refused (GPU busy), retry next frame
    std::atomic<bool> m_sceneActive{ false };
    std::atomic<int> m_cmd{ 0 };            // 1 = commit, 2 = cancel
    std::atomic<float> m_inYaw{ 0.0f };
    std::atomic<float> m_inPitch{ 0.0f };
    std::atomic<float> m_inZoom{ 0.0f };
    std::atomic<float> m_inFlick{ 0.0f };
    std::atomic<float> m_inUserZoom{ 1.0f };

    ModSettings m_cfg;
    POINT m_anchor{};

    // ---- persistent overlay objects (render thread only) ----
    HWND m_hwnd = nullptr;
    Compositor m_compositor{ nullptr };
    DesktopWindowTarget m_target{ nullptr };
    ContainerVisual m_emptyRoot{ nullptr };
    CompositionGraphicsDevice m_graphicsDevice{ nullptr };
    winrt::Windows::System::DispatcherQueueController m_dqController{ nullptr };

    ID3D11Device* m_d3dDevice = nullptr;
    ID2D1Factory1* m_d2dFactory = nullptr;
    ID2D1Device* m_d2dDevice = nullptr;
    IDWriteFactory* m_dwriteFactory = nullptr;
    IDWriteTextFormat* m_textFormatBadge = nullptr;

    // ---- per-scene objects ----
    ContainerVisual m_rootVisual{ nullptr };
    SpriteVisual m_backdropVisual{ nullptr };
    SpriteVisual m_backgroundVisual{ nullptr };
    CompositionDrawingSurface m_bgSurface{ nullptr };
    SpriteVisual m_shadowVisual{ nullptr };
    CompositionDrawingSurface m_shadowSurface{ nullptr };
    ContainerVisual m_cubeContainer{ nullptr };

    IDXGIOutput* m_output = nullptr;       // used for vblank pacing (any adapter)
    IDXGIOutput* m_dupOutput = nullptr;    // same adapter as the D3D device (duplication)
    bool m_canExclude = false;
    float m_monitorHz = 0.0f;
    int m_bgW = 0, m_bgH = 0;

    // ---- live desktop feed ----
    enum class LiveSource { None, Dup, Gdi };
    LiveSource m_liveSource = LiveSource::None;
    IDXGIOutputDuplication* m_dup = nullptr;
    ID3D11DeviceContext* m_d3dCtx = nullptr;
    ID3D11Texture2D* m_liveTex = nullptr;
    ID2D1Bitmap1* m_liveBmp = nullptr;
    int m_liveW = 0, m_liveH = 0;
    int m_liveDesktop = -1;
    bool m_liveHasFrame = false;
    int m_liveFrames = 0;
    ULONGLONG m_dupRetryAt = 0;
    ULONGLONG m_gdiNext = 0;
    HDC m_gdiMem = nullptr;
    HBITMAP m_gdiDib = nullptr;
    HGDIOBJ m_gdiOld = nullptr;
    void* m_gdiBits = nullptr;
    int m_fpsFrames = 0;
    const wchar_t* m_liveStatus = L"STATIC";
    IDXGISwapChain1* m_sc = nullptr;
    ICompositionSurface m_liveSurface{ nullptr };
    LONGLONG m_bgNext = 0;
    bool m_texFresh = false;               // m_liveTex holds a frame the blurred background has not drawn yet
    ID3D11Texture2D* m_gdiTex = nullptr;   // GDI fallback frame (full monitor)
    RECT m_workPx{};                       // work area (no taskbar) in capture pixels
    RECT m_workRect{};                     // work area in screen coordinates
    int m_workW = 0, m_workH = 0;
    std::vector<AltSurf> m_altSurf;
    bool m_thumbFresh = false;
    int m_thumbRR = 0;

    // work area (taskbar removed) as fractions of the monitor
    float m_wl = 0.0f, m_wt = 0.0f, m_wr = 1.0f, m_wb = 1.0f;

    // alt+tab switcher state
    std::vector<AltWin> m_alt;
    bool m_altReady = false;
    bool m_altAttached = false;
    bool m_altShown = false;
    bool m_altOrderDirty = true;
    bool m_altDirty = false;
    bool m_selMode = false;
    int m_selIdx = 0;
    int m_labelIdx = -1;
    float m_selT = 0.0f;
    std::atomic<int> m_selSteps{ 0 };
    std::vector<SpriteVisual> m_winVis;
    std::vector<CompositionSurfaceBrush> m_winBrush;
    float m_winLift[kMaxAlt] = {};
    float m_winOp[kMaxAlt] = {};
    SpriteVisual m_glow{ nullptr };
    SpriteVisual m_label{ nullptr };
    CompositionDrawingSurface m_labelSurface{ nullptr };
    CompositionSurfaceBrush m_labelBrush{ nullptr };
    LARGE_INTEGER m_fpsT0{};
    HANDLE m_pacerThread = nullptr;
    std::atomic<bool> m_pacerStop{ false };
    HANDLE m_composeThread = nullptr;

    struct CachedBitmap {
        ID2D1Bitmap1* bmp = nullptr;
        uint64_t ver = 0;
    };
    std::unordered_map<int, CachedBitmap> m_bitmapCache;

    struct FaceVisuals {
        ContainerVisual container{ nullptr };
        SpriteVisual background{ nullptr };
        SpriteVisual shade{ nullptr };
        SpriteVisual rim{ nullptr };
        SpriteVisual layer{ nullptr };
        CompositionDrawingSurface surface{ nullptr };
        CompositionDrawingSurface layerSurface{ nullptr };
        CompositionSurfaceBrush brush{ nullptr };
        CompositionSurfaceBrush layerBrush{ nullptr };
        int renderedDesktop = -99;
        uint64_t renderedVersion = 0;
        CompositionSurfaceBrush liveBrush{ nullptr };
        bool usingLive = false;
        // Last values written to the compositor. Re-writing an identical value still dirties the visual tree.
        float cBg = -1.0f, cRim = -1.0f, cShade = -1.0f, cLayer = -1.0f;
        signed char cVisContainer = -1, cVisShade = -1, cVisLayer = -1, cVisRim = -1;
    };
    FaceVisuals m_faces[kMaxFaces];
    int m_zOrder[kMaxFaces] = { 0, 1, 2, 3, 4, 5 };
    bool m_zOrderValid = false;

    // ---- geometry ----
    int m_faceCount = 4;
    float m_stepDeg = 90.0f;
    float m_D = 300.0f;
    float m_faceWidth = 480.0f;
    float m_faceHeight = 320.0f;
    float m_screenWidth = 1920.0f;
    float m_screenHeight = 1080.0f;
    int m_texW = 1920;
    int m_texH = 1080;
    float m_zoomRelease = 1.0f;
    float m_applied[4] = { 1e9f, 1e9f, 1e9f, 1e9f };
    float m_shadowW = 600.0f;
    float m_shadowH = 150.0f;
    int m_frameCounter = 0;

    VDManager::DesktopList m_desktopList;
    RECT m_monitorRect{};
    HMONITOR m_hMonitor = nullptr;

    // ---- animation state ----
    enum class Phase { Drag, Settle, Fade };
    Phase m_phase = Phase::Drag;
    Spring m_yaw, m_pitch, m_zoom, m_uz;
    float m_snapYaw = 0.0f;
    LARGE_INTEGER m_qpcFreq{};
    LARGE_INTEGER m_lastQpc{};
    std::chrono::steady_clock::time_point m_settleStart;
    std::chrono::steady_clock::time_point m_fadeStart;

    // ---- desktop switch state ----
    bool m_needSwitch = false;
    bool m_switchConfirmed = false;
    int m_targetDesktop = -1;
    int m_stepsLeft = 0;
    int m_stepDir = 0;
    int m_stepIntervalMs = 45;
    int m_switchTimeoutMs = 1500;
    std::chrono::steady_clock::time_point m_switchStart;
    std::chrono::steady_clock::time_point m_nextStep;
    std::chrono::steady_clock::time_point m_lastStepSent;
    std::chrono::steady_clock::time_point m_nextPoll;
    std::chrono::steady_clock::time_point m_confirmTime;
};

// Only touched on the render thread (registered in WarmInit, unregistered in Teardown).
static bool g_overlayClassRegistered = false;
static const wchar_t* kOverlayClassName = L"CompizDesktopCubeOverlayWindow";

// The one renderer. [[clang::no_destroy]]: when explorer.exe itself exits Wh_ModUninit is not called, the render thread
// is already gone, and the C++ runtime must not destroy Composition objects of a dead thread under the loader lock.
[[clang::no_destroy]] static std::optional<CubeRenderer> g_renderer;

CubeRenderer& CubeRenderer::Instance() {
    return *g_renderer;
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------
bool CubeRenderer::EnsureThread() {
    std::lock_guard<std::mutex> lk(m_threadMutex);
    // Checked under the same mutex Shutdown() uses to read m_thread: either Shutdown sees the thread created here,
    // or this call sees the flag and refuses. A thread nobody waits for can never start during unload.
    if (g_unloading.load()) return false;
    if (m_thread) {
        if (WaitForSingleObject(m_thread, 0) == WAIT_OBJECT_0) {
            CloseHandle(m_thread);
            m_thread = nullptr;
        } else {
            return true;
        }
    }
    if (!m_goEvent) m_goEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!m_quitEvent) m_quitEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!m_frameEvent) m_frameEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!m_wakeEvent) m_wakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!m_goEvent || !m_quitEvent || !m_frameEvent || !m_wakeEvent) return false;
    ResetEvent(m_quitEvent);
    m_thread = CreateThread(nullptr, 0, ThreadEntry, this, 0, nullptr);
    return m_thread != nullptr;
}

void CubeRenderer::Prewarm() {
    EnsureThread();
}

bool CubeRenderer::Start(POINT anchorPoint, float userZoom) {
    if (m_sceneActive.load()) return false;
    if (!EnsureThread()) return false;

    m_cfg = g_settings;
    m_anchor = anchorPoint;
    m_inYaw.store(0.0f);
    m_inPitch.store(0.0f);
    m_inZoom.store(0.0f);
    m_inFlick.store(0.0f);
    m_inUserZoom.store(userZoom);
    m_cmd.store(0);

    m_sceneActive.store(true);
    SetEvent(m_goEvent);
    return true;
}

void CubeRenderer::Shutdown() {
    Composer::s_abort.store(true);
    HANDLE t = nullptr;
    {
        std::lock_guard<std::mutex> lk(m_threadMutex);
        t = m_thread;
    }
    if (t) {
        if (m_quitEvent) SetEvent(m_quitEvent);
        // No timeout: the mod image must not be unmapped while this thread (or the pacer / composer it waits for) still runs.
        WaitForSingleObject(t, INFINITE);
        std::lock_guard<std::mutex> lk(m_threadMutex);
        CloseHandle(m_thread);
        m_thread = nullptr;
    }
    m_sceneActive.store(false);
}

// ---------------------------------------------------------------------------
// Thread
// ---------------------------------------------------------------------------
DWORD WINAPI CubeRenderer::ThreadEntry(LPVOID p) {
    return ((CubeRenderer*)p)->ThreadMain();
}

DWORD CubeRenderer::ThreadMain() {
    bool comInit = false;
    try {
        winrt::init_apartment(winrt::apartment_type::single_threaded);
        comInit = true;
    } catch (...) {
    }

    // Slightly raised priority + the light "Window Manager" MMCSS profile (not "Games", which keeps the CPU/GPU clocks high).
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
    {
        HMODULE hAv = LoadLibraryW(L"avrt.dll");
        if (hAv) {
            typedef HANDLE(WINAPI* AvSetFn)(LPCWSTR, LPDWORD);
            auto fn = (AvSetFn)GetProcAddress(hAv, "AvSetMmThreadCharacteristicsW");
            if (fn) {
                DWORD idx = 0;
                fn(L"Window Manager", &idx);
            }
        }
    }

    bool ok = false;
    try {
        ok = WarmInit();
    } catch (winrt::hresult_error const& e) {
        LOG_ERR(L"WinRT exception in warm init 0x%08X", (unsigned)e.code().value);
    } catch (...) {
        LOG_ERR(L"Unknown exception in warm init");
    }

    if (!ok) {
        if (m_sceneActive.exchange(false)) GestureController::OnSnapCompleted();
    } else {
        LOG_INF(L"Render thread warm");
        for (;;) {
            HANDLE h[2] = { m_goEvent, m_quitEvent };
            DWORD r = MsgWaitForMultipleObjectsEx(2, h, INFINITE, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
            if (r == WAIT_OBJECT_0 + 2) {
                MSG msg;
                while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                    TranslateMessage(&msg);
                    DispatchMessageW(&msg);
                }
                continue;
            }
            if (r == WAIT_OBJECT_0 + 1) break;
            if (r == WAIT_OBJECT_0) {
                RunScene();
                if (WaitForSingleObject(m_quitEvent, 0) == WAIT_OBJECT_0) break;
            }
        }
    }

    try { Teardown(); } catch (...) {}
    VDManager::SuppressSystemAnimation(false);
    if (comInit) {
        try { winrt::uninit_apartment(); } catch (...) {}
    }
    return 0;
}

void CubeRenderer::RunScene() {
    bool ok = false;
    timeBeginPeriod(1);   // only while the cube is on screen
    try {
        ok = BeginScene();
    } catch (winrt::hresult_error const& e) {
        LOG_ERR(L"WinRT exception in BeginScene 0x%08X", (unsigned)e.code().value);
    } catch (...) {
        LOG_ERR(L"Unknown exception in BeginScene");
    }

    if (ok) {
        try {
            RunLoop();
        } catch (winrt::hresult_error const& e) {
            LOG_ERR(L"WinRT exception in loop 0x%08X", (unsigned)e.code().value);
        } catch (...) {
            LOG_ERR(L"Unknown exception in loop");
        }
    }

    try { EndScene(); } catch (...) {}
    timeEndPeriod(1);
    VDManager::SuppressSystemAnimation(false);
    m_sceneActive.store(false);
    GestureController::OnSnapCompleted();
}

bool CubeRenderer::EnsureDispatcherQueue() {
    if (winrt::Windows::System::DispatcherQueue::GetForCurrentThread()) return true;

    HMODULE hCore = LoadLibraryExW(L"CoreMessaging.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hCore) return false;

    typedef HRESULT(WINAPI* CreateDQCFn)(DispatcherQueueOptionsX, void**);
    auto fn = (CreateDQCFn)GetProcAddress(hCore, "CreateDispatcherQueueController");
    if (!fn) return false;

    DispatcherQueueOptionsX opts{ sizeof(opts), 2 /*CURRENT*/, 0 /*COM_NONE*/ };
    HRESULT hr = fn(opts, winrt::put_abi(m_dqController));
    if (FAILED(hr)) {
        LOG_ERR(L"CreateDispatcherQueueController failed 0x%08X", (unsigned)hr);
        return false;
    }
    return winrt::Windows::System::DispatcherQueue::GetForCurrentThread() != nullptr;
}

bool CubeRenderer::InitializeGraphicsDevice() {
    if (m_graphicsDevice) return true;

    // Use the GPU that actually drives the monitor under the mouse. With "high performance" set for
    // explorer.exe (or on a hybrid laptop) the default adapter can be a different GPU, which breaks
    // fast screen capture and adds a copy across GPUs for every frame.
    IDXGIAdapter1* chosen = nullptr;
    {
        POINT cp{};
        GetCursorPos(&cp);
        HMONITOR hm = MonitorFromPoint(cp, MONITOR_DEFAULTTOPRIMARY);
        IDXGIFactory1* fac = nullptr;
        if (SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&fac))) && fac) {
            for (UINT a = 0; !chosen; ++a) {
                IDXGIAdapter1* ad = nullptr;
                if (FAILED(fac->EnumAdapters1(a, &ad)) || !ad) break;
                bool match = false;
                for (UINT o = 0; !match; ++o) {
                    IDXGIOutput* out = nullptr;
                    if (FAILED(ad->EnumOutputs(o, &out)) || !out) break;
                    DXGI_OUTPUT_DESC od{};
                    if (SUCCEEDED(out->GetDesc(&od)) && od.Monitor == hm) match = true;
                    out->Release();
                }
                if (match) chosen = ad; else ad->Release();
            }
            fac->Release();
        }
    }

    D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_0 };
    HRESULT hr = E_FAIL;
    if (chosen) {
        hr = D3D11CreateDevice(chosen, D3D_DRIVER_TYPE_UNKNOWN, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT,
                               levels, 1, D3D11_SDK_VERSION, &m_d3dDevice, nullptr, nullptr);
        DXGI_ADAPTER_DESC1 ad1{};
        if (SUCCEEDED(chosen->GetDesc1(&ad1))) {
            LOG_INF(L"GPU for the cube: %s (%u MB dedicated)", ad1.Description, (unsigned)(ad1.DedicatedVideoMemory >> 20));
        }
        chosen->Release();
    }
    if (FAILED(hr) || !m_d3dDevice) {
        hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
                               D3D11_CREATE_DEVICE_BGRA_SUPPORT,
                               levels, 1, D3D11_SDK_VERSION,
                               &m_d3dDevice, nullptr, nullptr);
    }
    if (FAILED(hr) || !m_d3dDevice) {
        LOG_ERR(L"D3D11CreateDevice failed 0x%08X", (unsigned)hr);
        return false;
    }

    IDXGIDevice* dxgiDevice = nullptr;
    m_d3dDevice->QueryInterface(IID_PPV_ARGS(&dxgiDevice));
    if (!dxgiDevice) return false;

    D2D1_FACTORY_OPTIONS options{};
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1), &options, (void**)&m_d2dFactory);
    if (m_d2dFactory) m_d2dFactory->CreateDevice(dxgiDevice, &m_d2dDevice);
    dxgiDevice->SetGPUThreadPriority(7);   // our GPU work wins against other apps on a busy iGPU
    {
        IDXGIDevice1* d1 = nullptr;
        if (SUCCEEDED(dxgiDevice->QueryInterface(IID_PPV_ARGS(&d1))) && d1) { d1->SetMaximumFrameLatency(3); d1->Release(); }
    }
    dxgiDevice->Release();
    if (!m_d2dDevice) {
        LOG_ERR(L"Direct2D device creation failed");
        return false;
    }

    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&m_dwriteFactory);
    if (m_dwriteFactory) {
        m_dwriteFactory->CreateTextFormat(L"Segoe UI", nullptr,
            DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
            13.0f, L"en-us", &m_textFormatBadge);
        if (m_textFormatBadge) {
            m_textFormatBadge->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            m_textFormatBadge->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
    }

    ICompositorInteropX* ci = nullptr;
    IUnknown* compUnk = (IUnknown*)winrt::get_abi(m_compositor);
    if (SUCCEEDED(compUnk->QueryInterface(IID_ICompositorInteropX, (void**)&ci)) && ci) {
        hr = ci->CreateGraphicsDevice(m_d2dDevice, winrt::put_abi(m_graphicsDevice));
        ci->Release();
        if (FAILED(hr)) {
            LOG_ERR(L"CreateGraphicsDevice failed 0x%08X", (unsigned)hr);
            return false;
        }
    } else {
        LOG_ERR(L"ICompositorInterop not available");
        return false;
    }
    return m_graphicsDevice != nullptr;
}

void CubeRenderer::FindOutput() {
    if (m_output) { m_output->Release(); m_output = nullptr; }
    if (m_dupOutput) { m_dupOutput->Release(); m_dupOutput = nullptr; }
    if (!m_d3dDevice) return;

    IDXGIDevice* dd = nullptr;
    if (SUCCEEDED(m_d3dDevice->QueryInterface(IID_PPV_ARGS(&dd))) && dd) {
        IDXGIAdapter* ad = nullptr;
        if (SUCCEEDED(dd->GetAdapter(&ad)) && ad) {
            for (UINT o = 0; ; ++o) {
                IDXGIOutput* out = nullptr;
                if (FAILED(ad->EnumOutputs(o, &out)) || !out) break;
                DXGI_OUTPUT_DESC od{};
                if (SUCCEEDED(out->GetDesc(&od)) && od.Monitor == m_hMonitor) {
                    m_dupOutput = out;
                    m_output = out;
                    m_output->AddRef();
                    break;
                }
                out->Release();
            }
            ad->Release();
        }
        dd->Release();
    }

    // Hybrid GPU: the monitor hangs off another adapter. Still pace on its vblank.
    if (!m_output) {
        IDXGIFactory1* fac = nullptr;
        if (SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&fac))) && fac) {
            for (UINT a = 0; !m_output; ++a) {
                IDXGIAdapter1* ad1 = nullptr;
                if (FAILED(fac->EnumAdapters1(a, &ad1)) || !ad1) break;
                for (UINT o = 0; ; ++o) {
                    IDXGIOutput* out = nullptr;
                    if (FAILED(ad1->EnumOutputs(o, &out)) || !out) break;
                    DXGI_OUTPUT_DESC od{};
                    if (SUCCEEDED(out->GetDesc(&od)) && od.Monitor == m_hMonitor) { m_output = out; break; }
                    out->Release();
                }
                ad1->Release();
            }
            fac->Release();
        }
    }
    LOG_DBG(L"VBlank source: %s, duplication output: %s",
            m_output ? L"DXGI output" : L"DwmFlush", m_dupOutput ? L"yes" : L"no (GDI fallback)");
}

bool CubeRenderer::WarmInit() {
    // Registered with the mod's own module handle. ANY failure (including ERROR_CLASS_ALREADY_EXISTS) is an error:
    // an existing class could still point at a window procedure inside an earlier, unloaded copy of the mod.
    const HMODULE hMod = GetThisModuleHandle();
    if (!hMod) {
        LOG_ERR(L"Cannot get the mod module handle: %lu", GetLastError());
        return false;
    }
    if (!g_overlayClassRegistered) {
        WNDCLASSEXW wc{ sizeof(wc) };
        wc.lpfnWndProc = WndProc;
        wc.hInstance = hMod;
        wc.lpszClassName = kOverlayClassName;
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        if (!RegisterClassExW(&wc)) {
            LOG_ERR(L"RegisterClassExW failed: %lu", GetLastError());
            return false;
        }
        g_overlayClassRegistered = true;
    }

    m_hwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_NOREDIRECTIONBITMAP | WS_EX_TRANSPARENT,
        kOverlayClassName, L"Compiz Desktop Cube", WS_POPUP,
        0, 0, 16, 16, nullptr, nullptr, hMod, this);
    if (!m_hwnd) {
        LOG_ERR(L"CreateWindowExW failed: %lu", GetLastError());
        return false;
    }
    // Hide the overlay from every capture API so the live feed never films itself.
    m_canExclude = SetWindowDisplayAffinity(m_hwnd, WDA_EXCLUDEFROMCAPTURE) != 0;
    if (!m_canExclude) LOG_INF(L"WDA_EXCLUDEFROMCAPTURE unavailable (needs Windows 10 2004+), live capture off");

    if (!EnsureDispatcherQueue()) {
        LOG_ERR(L"No DispatcherQueue on render thread");
        return false;
    }

    m_compositor = Compositor();

    IUnknown* compUnk = (IUnknown*)winrt::get_abi(m_compositor);
    ICompositorDesktopInteropX* interop = nullptr;
    HRESULT hr = compUnk->QueryInterface(IID_ICompositorDesktopInteropX, (void**)&interop);
    if (FAILED(hr) || !interop) {
        LOG_ERR(L"ICompositorDesktopInterop unavailable 0x%08X", (unsigned)hr);
        return false;
    }
    hr = interop->CreateDesktopWindowTarget(m_hwnd, TRUE, winrt::put_abi(m_target));
    interop->Release();
    if (FAILED(hr) || !m_target) {
        LOG_ERR(L"CreateDesktopWindowTarget failed 0x%08X", (unsigned)hr);
        return false;
    }

    m_emptyRoot = m_compositor.CreateContainerVisual();
    m_target.Root(m_emptyRoot);

    if (!InitializeGraphicsDevice()) return false;

    QueryPerformanceFrequency(&m_qpcFreq);
    return true;
}

CompositionDrawingSurface CubeRenderer::MakeSurface(int w, int h) {
    return m_graphicsDevice.CreateDrawingSurface(
        { (float)w, (float)h },
        DirectXPixelFormat::B8G8R8A8UIntNormalized,
        DirectXAlphaMode::Premultiplied);
}

ID2D1Bitmap1* CubeRenderer::GetDesktopBitmap(ID2D1DeviceContext* ctx, int dIdx) {
    if (dIdx < 0 || dIdx >= m_desktopList.TotalCount()) return nullptr;

    uint64_t ver = 0;
    auto snap = DesktopCaptureManager::Get(m_desktopList.items[dIdx].id, &ver);
    if (!snap) return nullptr;

    auto it = m_bitmapCache.find(dIdx);
    if (it != m_bitmapCache.end()) {
        if (it->second.ver == ver) return it->second.bmp;
        if (it->second.bmp) it->second.bmp->Release();
        m_bitmapCache.erase(it);
    }

    ID2D1Bitmap1* bmp = nullptr;
    if (!CreateBitmapFromSnapshot(ctx, *snap, &bmp) || !bmp) return nullptr;
    CachedBitmap cb;
    cb.bmp = bmp;
    cb.ver = ver;
    m_bitmapCache[dIdx] = cb;
    return bmp;
}

void CubeRenderer::BuildBackground() {
    int cur = m_desktopList.currentIndex;
    float sw = m_screenWidth, sh = m_screenHeight;
    bool blur = m_cfg.blur;
    float sigma = m_cfg.blurStrength;

    if (!DesktopCaptureManager::Get(m_desktopList.items[cur].id)) return;

    const float bgScale = 0.25f;
    m_bgW = (int)(sw * bgScale); m_bgH = (int)(sh * bgScale);
    m_bgSurface = MakeSurface(m_bgW, m_bgH);
    bool ok = DrawToSurface(m_bgSurface, [&](ID2D1DeviceContext* ctx, POINT off) {
        ctx->Clear(D2D1::ColorF(0.04f, 0.05f, 0.08f, 1.0f));

        ID2D1Bitmap1* bmp = GetDesktopBitmap(ctx, cur);
        if (!bmp) return;
        // Snapshots are stored at most 1920 px wide: kx / ky map bitmap pixels back to screen pixels.
        const D2D1_SIZE_U bps = bmp->GetPixelSize();
        const float bw = (float)std::max<UINT32>(1, bps.width), bh = (float)std::max<UINT32>(1, bps.height);
        const float kx = sw / bw, ky = sh / bh;
        ctx->SetTransform(D2D1::Matrix3x2F::Scale(bgScale * kx, bgScale * ky) *
                          D2D1::Matrix3x2F::Translation((float)off.x, (float)off.y));

        bool drawn = false;
        if (blur) {
            ID2D1Effect* fx = nullptr;
            if (SUCCEEDED(ctx->CreateEffect(CLSID_D2D1GaussianBlur, &fx)) && fx) {
                fx->SetInput(0, bmp);
                fx->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, sigma / kx);
                fx->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, (UINT32)D2D1_BORDER_MODE_SOFT);

                ID2D1Image* out = nullptr;
                fx->GetOutput(&out);
                if (out) {
                    D2D1_POINT_2F pt = D2D1::Point2F(0.0f, 0.0f);
                    D2D1_RECT_F src = D2D1::RectF(0.0f, 0.0f, bw, bh);
                    ctx->DrawImage(out, &pt, &src, D2D1_INTERPOLATION_MODE_LINEAR, D2D1_COMPOSITE_MODE_SOURCE_OVER);
                    out->Release();
                    drawn = true;
                }
                fx->Release();
            }
        }
        if (!drawn) ctx->DrawBitmap(bmp, D2D1::RectF(0.0f, 0.0f, bw, bh));

        ID2D1SolidColorBrush* dimBrush = nullptr;
        ctx->CreateSolidColorBrush(D2D1::ColorF(8.0f / 255.0f, 10.0f / 255.0f, 16.0f / 255.0f, (float)m_cfg.dim / 100.0f), &dimBrush);
        if (dimBrush) {
            ctx->SetTransform(D2D1::Matrix3x2F::Translation((float)off.x, (float)off.y));
            ctx->FillRectangle(D2D1::RectF(0.0f, 0.0f, sw * bgScale, sh * bgScale), dimBrush);
            dimBrush->Release();
        }
    });

    if (ok) {
        m_backgroundVisual = m_compositor.CreateSpriteVisual();
        m_backgroundVisual.Size({ sw, sh });
        m_backgroundVisual.CenterPoint({ sw * 0.5f, sh * 0.5f, 0.0f });
        m_backgroundVisual.Brush(m_compositor.CreateSurfaceBrush(m_bgSurface));
        m_rootVisual.Children().InsertAtTop(m_backgroundVisual);
    }
}

void CubeRenderer::RenderShadow() {
    m_shadowW = m_faceWidth * 1.35f;
    m_shadowH = m_faceWidth * 0.28f;
    m_shadowSurface = MakeSurface((int)m_shadowW, (int)m_shadowH);
    float w = m_shadowW, h = m_shadowH;

    DrawToSurface(m_shadowSurface, [&](ID2D1DeviceContext* ctx, POINT off) {
        ctx->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
        ctx->SetTransform(D2D1::Matrix3x2F::Translation((float)off.x, (float)off.y));

        D2D1_GRADIENT_STOP stops[2] = {
            { 0.0f, D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.75f) },
            { 1.0f, D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f) }
        };
        ID2D1GradientStopCollection* gs = nullptr;
        ctx->CreateGradientStopCollection(stops, 2, &gs);
        if (gs) {
            ID2D1RadialGradientBrush* rb = nullptr;
            ctx->CreateRadialGradientBrush(
                D2D1::RadialGradientBrushProperties(D2D1::Point2F(w * 0.5f, h * 0.5f), D2D1::Point2F(0.0f, 0.0f), w * 0.5f, h * 0.5f),
                gs, &rb);
            if (rb) {
                ctx->FillRectangle(D2D1::RectF(0.0f, 0.0f, w, h), rb);
                rb->Release();
            }
            gs->Release();
        }
    });

    m_shadowVisual = m_compositor.CreateSpriteVisual();
    m_shadowVisual.Size({ m_shadowW, m_shadowH });
    m_shadowVisual.CenterPoint({ m_shadowW * 0.5f, m_shadowH * 0.5f, 0.0f });
    m_shadowVisual.Brush(m_compositor.CreateSurfaceBrush(m_shadowSurface));
    m_shadowVisual.Opacity(0.0f);
    m_rootVisual.Children().InsertAtTop(m_shadowVisual);
}

ID2D1Bitmap1* CubeRenderer::FaceBitmap(ID2D1DeviceContext* ctx, int dIdx) {
    if (m_liveBmp && m_liveHasFrame && dIdx == m_liveDesktop) return m_liveBmp;
    return GetDesktopBitmap(ctx, dIdx);
}

void CubeRenderer::RenderFace(int i, int dIdx) {
    auto& f = m_faces[i];
    f.renderedDesktop = dIdx;
    if (!f.surface) f.surface = MakeSurface(m_texW, m_texH);
    if (!f.layerSurface) f.layerSurface = MakeSurface((int)(kBadgeW * 2.0f), (int)(kBadgeH * 2.0f));

    int total = m_desktopList.TotalCount();
    bool valid = dIdx >= 0 && dIdx < total;
    uint64_t ver = 0;
    DesktopCaptureManager::SnapPtr snap;
    if (valid) snap = DesktopCaptureManager::Get(m_desktopList.items[dIdx].id, &ver);
    f.renderedVersion = ver;
    const bool haveImage = (snap != nullptr) || (m_liveBmp && m_liveHasFrame && dIdx == m_liveDesktop);

    FaceColor accent = kFaceAccents[i % kMaxFaces];
    const float faceW = m_faceWidth, faceH = m_faceHeight;
    const bool showLabels = m_cfg.labels;

    // ---- base plane: the desktop image (live for the desktop you are on) ----
    bool ok = DrawToSurface(f.surface, [&](ID2D1DeviceContext* ctx, POINT off) {
        ctx->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
        ctx->Clear(D2D1::ColorF(0.07f, 0.09f, 0.13f, 1.0f));
        float k = (float)m_texW / faceW;
        ctx->SetTransform(D2D1::Matrix3x2F::Scale(k, k) *
                          D2D1::Matrix3x2F::Translation((float)off.x, (float)off.y));

        ID2D1SolidColorBrush* textBrush = nullptr;
        ctx->CreateSolidColorBrush(D2D1::ColorF(0.92f, 0.94f, 0.98f, 1.0f), &textBrush);
        ctx->PushAxisAlignedClip(D2D1::RectF(0.0f, 0.0f, faceW, faceH), D2D1_ANTIALIAS_MODE_ALIASED);

        ID2D1Bitmap1* bmp = haveImage ? FaceBitmap(ctx, dIdx) : nullptr;
        if (bmp) {
            D2D1_SIZE_U ps = bmp->GetPixelSize();
            D2D1_RECT_F srcR = (bmp == m_liveBmp)
                ? D2D1::RectF(0.0f, 0.0f, (float)ps.width, (float)ps.height)
                : D2D1::RectF(m_wl * (float)ps.width, m_wt * (float)ps.height,
                              m_wr * (float)ps.width, m_wb * (float)ps.height);
            ctx->DrawBitmap(bmp, D2D1::RectF(0.0f, 0.0f, faceW, faceH), 1.0f,
                            D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &srcR);
        } else if (textBrush && m_textFormatBadge) {
            const wchar_t* msg = !valid ? L"No desktop" : L"Loading desktop...";
            ctx->DrawText(msg, (UINT32)wcslen(msg), m_textFormatBadge,
                          D2D1::RectF(0.0f, faceH * 0.40f, faceW, faceH * 0.60f),
                          textBrush, D2D1_DRAW_TEXT_OPTIONS_NONE);
        }

        ctx->PopAxisAlignedClip();
        if (textBrush) textBrush->Release();
    });
    if (ok && !f.brush) {
        f.brush = m_compositor.CreateSurfaceBrush(f.surface);
        f.background.Brush(f.brush);
        f.usingLive = false;
    }

    // ---- badge: a small separate visual ("Desktop N") ----
    DrawToSurface(f.layerSurface, [&](ID2D1DeviceContext* ctx, POINT off) {
        ctx->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
        ctx->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
        ctx->SetTransform(D2D1::Matrix3x2F::Scale(2.0f, 2.0f) *
                          D2D1::Matrix3x2F::Translation((float)off.x, (float)off.y));
        if (!(showLabels && valid && m_textFormatBadge)) return;

        ID2D1SolidColorBrush* textBrush = nullptr;
        ID2D1SolidColorBrush* pillBrush = nullptr;
        ID2D1SolidColorBrush* accentBrush = nullptr;
        ctx->CreateSolidColorBrush(D2D1::ColorF(0.92f, 0.94f, 0.98f, 1.0f), &textBrush);
        ctx->CreateSolidColorBrush(D2D1::ColorF(0.06f, 0.07f, 0.09f, 0.82f), &pillBrush);
        ctx->CreateSolidColorBrush(D2D1::ColorF(accent.r / 255.0f, accent.g / 255.0f, accent.b / 255.0f, 1.0f), &accentBrush);
        if (textBrush && pillBrush && accentBrush) {
            ctx->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(0.0f, 0.0f, kBadgeW, kBadgeH), 12.0f, 12.0f), pillBrush);
            ctx->FillEllipse(D2D1::Ellipse(D2D1::Point2F(14.0f, kBadgeH * 0.5f), 4.0f, 4.0f), accentBrush);
            wchar_t label[96];
            if (dIdx == m_liveDesktop) swprintf_s(label, L"Desktop %d  -  %s", dIdx + 1, m_liveStatus);
            else swprintf_s(label, L"Desktop %d", dIdx + 1);
            ctx->DrawText(label, (UINT32)wcslen(label), m_textFormatBadge,
                          D2D1::RectF(20.0f, 0.0f, kBadgeW - 6.0f, kBadgeH),
                          textBrush, D2D1_DRAW_TEXT_OPTIONS_NONE);
        }
        if (textBrush) textBrush->Release();
        if (pillBrush) pillBrush->Release();
        if (accentBrush) accentBrush->Release();
    });

    if (!f.layerBrush && f.layer) {
        f.layerBrush = m_compositor.CreateSurfaceBrush(f.layerSurface);
        f.layer.Brush(f.layerBrush);
    }
}

// ---------------------------------------------------------------------------
// Live desktop feed
// ---------------------------------------------------------------------------
bool CubeRenderer::CreateDuplication(int* outW, int* outH) {
    if (!m_dupOutput || !m_d3dDevice) return false;
    IDXGIOutput1* o1 = nullptr;
    if (FAILED(m_dupOutput->QueryInterface(IID_PPV_ARGS(&o1))) || !o1) return false;
    IDXGIOutputDuplication* dup = nullptr;
    HRESULT hr = o1->DuplicateOutput(m_d3dDevice, &dup);
    o1->Release();
    if (FAILED(hr) || !dup) {
        LOG_INF(L"DuplicateOutput failed 0x%08X", (unsigned)hr);
        return false;
    }
    DXGI_OUTDUPL_DESC dd{};
    dup->GetDesc(&dd);
    if (dd.ModeDesc.Format != DXGI_FORMAT_B8G8R8A8_UNORM || dd.ModeDesc.Width == 0 || dd.ModeDesc.Height == 0) {
        LOG_INF(L"Duplication format %d not supported (HDR?)", (int)dd.ModeDesc.Format);
        dup->Release();
        return false;
    }
    if (outW) *outW = (int)dd.ModeDesc.Width;
    if (outH) *outH = (int)dd.ModeDesc.Height;
    m_dup = dup;
    return true;
}

bool CubeRenderer::CreateCompSwapChain(int w, int h, IDXGISwapChain1** outSc, ICompositionSurface* outSurf) {
    IDXGIDevice* dd = nullptr;
    IDXGIAdapter* ad = nullptr;
    IDXGIFactory2* fac = nullptr;
    IDXGISwapChain1* sc = nullptr;
    bool ok = false;
    if (SUCCEEDED(m_d3dDevice->QueryInterface(IID_PPV_ARGS(&dd))) && dd &&
        SUCCEEDED(dd->GetAdapter(&ad)) && ad &&
        SUCCEEDED(ad->GetParent(IID_PPV_ARGS(&fac))) && fac) {
        DXGI_SWAP_CHAIN_DESC1 sd{};
        sd.Width = (UINT)w;
        sd.Height = (UINT)h;
        sd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        sd.SampleDesc.Count = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = 2;
        sd.Scaling = DXGI_SCALING_STRETCH;
        sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        sd.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
        if (SUCCEEDED(fac->CreateSwapChainForComposition(m_d3dDevice, &sd, nullptr, &sc)) && sc) {
            ICompositorInteropX* ci = nullptr;
            IUnknown* compUnk = (IUnknown*)winrt::get_abi(m_compositor);
            if (SUCCEEDED(compUnk->QueryInterface(IID_ICompositorInteropX, (void**)&ci)) && ci) {
                ICompositionSurface surf{ nullptr };
                HRESULT hr = ci->CreateCompositionSurfaceForSwapChain(sc, winrt::put_abi(surf));
                ci->Release();
                if (SUCCEEDED(hr) && surf) { *outSurf = surf; ok = true; }
            }
        }
    }
    if (fac) fac->Release();
    if (ad) ad->Release();
    if (dd) dd->Release();
    if (ok) *outSc = sc; else if (sc) sc->Release();
    return ok;
}

static bool MakeLiveTexture(ID3D11Device* dev, int w, int h, ID3D11Texture2D** out) {
    D3D11_TEXTURE2D_DESC td{};
    td.Width = (UINT)w; td.Height = (UINT)h;
    td.MipLevels = 1; td.ArraySize = 1;
    td.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    td.SampleDesc.Count = 1;
    td.Usage = D3D11_USAGE_DEFAULT;
    td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    return SUCCEEDED(dev->CreateTexture2D(&td, nullptr, out)) && *out;
}

bool CubeRenderer::StartLive(const DesktopCaptureManager::SnapshotData* initial) {
    StopLive();
    m_liveFrames = 0;
    m_liveStatus = L"STATIC";
    if (!m_canExclude) { m_liveStatus = L"STATIC (cannot hide overlay)"; return false; }
    if (!m_d3dDevice || !m_d2dDevice) { m_liveStatus = L"STATIC (no GPU device)"; return false; }

    int w = (int)m_screenWidth, h = (int)m_screenHeight;
    if (m_cfg.liveMethod != 2 && CreateDuplication(&w, &h)) m_liveSource = LiveSource::Dup;
    else m_liveSource = LiveSource::Gdi;
    if (w < 128 || h < 128) { StopLive(); return false; }
    m_liveW = w; m_liveH = h;

    // Everything below the taskbar line is cut away right here, so the taskbar can never show up.
    m_workPx.left = std::clamp((int)std::lround(m_wl * (float)w), 0, w - 64);
    m_workPx.top = std::clamp((int)std::lround(m_wt * (float)h), 0, h - 64);
    m_workPx.right = std::clamp((int)std::lround(m_wr * (float)w), (int)m_workPx.left + 64, w);
    m_workPx.bottom = std::clamp((int)std::lround(m_wb * (float)h), (int)m_workPx.top + 64, h);
    m_workW = m_workPx.right - m_workPx.left;
    m_workH = m_workPx.bottom - m_workPx.top;

    m_d3dDevice->GetImmediateContext(&m_d3dCtx);
    if (!m_d3dCtx || !MakeLiveTexture(m_d3dDevice, m_workW, m_workH, &m_liveTex)) {
        m_liveStatus = L"STATIC (texture failed)";
        StopLive();
        return false;
    }

    bool haveInitial = false;
    if (initial && initial->width == w && initial->height == h && !initial->pixels.empty()) {
        const uint8_t* base = (const uint8_t*)initial->pixels.data() +
                              ((size_t)m_workPx.top * (size_t)w + (size_t)m_workPx.left) * 4;
        m_d3dCtx->UpdateSubresource(m_liveTex, 0, nullptr, base, (UINT)w * 4, 0);
        haveInitial = true;
    }

    IDXGISurface* surf = nullptr;
    ID2D1DeviceContext* dc = nullptr;
    bool ok = false;
    if (SUCCEEDED(m_liveTex->QueryInterface(IID_PPV_ARGS(&surf))) && surf &&
        SUCCEEDED(m_d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &dc)) && dc) {
        D2D1_BITMAP_PROPERTIES1 bp = D2D1::BitmapProperties1(
            D2D1_BITMAP_OPTIONS_NONE,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE), 96.0f, 96.0f);
        ok = SUCCEEDED(dc->CreateBitmapFromDxgiSurface(surf, &bp, &m_liveBmp)) && m_liveBmp;
    }
    if (dc) dc->Release();
    if (surf) surf->Release();
    if (!ok) {
        m_liveStatus = L"STATIC (bitmap failed)";
        StopLive();
        return false;
    }

    if (!CreateCompSwapChain(m_workW, m_workH, &m_sc, &m_liveSurface)) {
        m_liveStatus = L"STATIC (swapchain failed)";
        LOG_ERR(L"Live swap chain creation failed");
        StopLive();
        return false;
    }
    if (haveInitial) {
        ID3D11Texture2D* bb = nullptr;
        if (SUCCEEDED(m_sc->GetBuffer(0, IID_PPV_ARGS(&bb))) && bb) {
            m_d3dCtx->CopyResource(bb, m_liveTex);
            bb->Release();
            m_sc->Present(0, 0);
        }
    }

    if (m_liveSource == LiveSource::Gdi) {
        HDC scr = GetDC(nullptr);
        m_gdiMem = scr ? CreateCompatibleDC(scr) : nullptr;
        BITMAPINFO bmi{};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = w;
        bmi.bmiHeader.biHeight = -h;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        if (m_gdiMem) m_gdiDib = CreateDIBSection(scr, &bmi, DIB_RGB_COLORS, &m_gdiBits, nullptr, 0);
        if (scr) ReleaseDC(nullptr, scr);
        if (!m_gdiMem || !m_gdiDib || !m_gdiBits || !MakeLiveTexture(m_d3dDevice, w, h, &m_gdiTex)) {
            m_liveStatus = L"STATIC (GDI failed)";
            StopLive();
            return false;
        }
        m_gdiOld = SelectObject(m_gdiMem, m_gdiDib);
    }

    m_liveHasFrame = haveInitial;
    m_texFresh = haveInitial;
    m_liveStatus = (m_liveSource == LiveSource::Dup) ? L"LIVE" : L"LIVE (GDI)";
    LOG_INF(L"Live capture: %s %dx%d, work area %dx%d", m_liveSource == LiveSource::Dup ? L"Desktop Duplication" : L"GDI fallback",
            w, h, m_workW, m_workH);
    return true;
}

void CubeRenderer::StopLive() {
    m_liveHasFrame = false;
    m_texFresh = false;
    m_presentPending = false;
    m_liveSource = LiveSource::None;
    m_altSurf.clear();
    if (m_liveBmp) { m_liveBmp->Release(); m_liveBmp = nullptr; }
    if (m_dup) { m_dup->Release(); m_dup = nullptr; }
    if (m_sc) { m_sc->Release(); m_sc = nullptr; }
    m_liveSurface = nullptr;
    if (m_liveTex) { m_liveTex->Release(); m_liveTex = nullptr; }
    if (m_gdiTex) { m_gdiTex->Release(); m_gdiTex = nullptr; }
    if (m_d3dCtx) { m_d3dCtx->Release(); m_d3dCtx = nullptr; }
    if (m_gdiMem) {
        if (m_gdiOld) SelectObject(m_gdiMem, m_gdiOld);
        DeleteDC(m_gdiMem);
    }
    if (m_gdiDib) DeleteObject(m_gdiDib);
    m_gdiMem = nullptr; m_gdiDib = nullptr; m_gdiOld = nullptr; m_gdiBits = nullptr;
}

// One new screen frame: crop it once (taskbar gone) and hand it to the compositor.
// Extra copies only happen when something needs them (background at 30 Hz, switcher windows).
void CubeRenderer::DistributeFrame(ID3D11Texture2D* src, bool wantTex) {
    const D3D11_BOX box{ (UINT)m_workPx.left, (UINT)m_workPx.top, 0, (UINT)m_workPx.right, (UINT)m_workPx.bottom, 1 };
    ID3D11Texture2D* bb = nullptr;
    if (m_sc && SUCCEEDED(m_sc->GetBuffer(0, IID_PPV_ARGS(&bb))) && bb) {
        m_d3dCtx->CopySubresourceRegion(bb, 0, 0, 0, 0, src, 0, &box);
        bb->Release();
        // Never block the render thread: if the GPU is behind, keep the (already copied) frame and retry next frame.
        m_presentPending = (m_sc->Present(0, DXGI_PRESENT_DO_NOT_WAIT) == DXGI_ERROR_WAS_STILL_DRAWING);
    }
    if (wantTex || m_selMode) {
        m_d3dCtx->CopySubresourceRegion(m_liveTex, 0, 0, 0, 0, src, 0, &box);
        m_texFresh = true;
    }
    if (m_selMode) m_thumbFresh = true;
}

// Returns true when a NEW frame was received.
bool CubeRenderer::PollLive(bool wantTex) {
    if (!m_liveTex || !m_d3dCtx) return false;

    if (m_liveSource == LiveSource::Gdi) {
        ULONGLONG now = GetTickCount64();
        if (now < m_gdiNext) return false;
        m_gdiNext = now + 33;
        HDC scr = GetDC(nullptr);
        if (!scr) return false;
        BitBlt(m_gdiMem, 0, 0, m_liveW, m_liveH, scr, m_monitorRect.left, m_monitorRect.top, SRCCOPY | CAPTUREBLT);
        ReleaseDC(nullptr, scr);
        m_d3dCtx->UpdateSubresource(m_gdiTex, 0, nullptr, m_gdiBits, (UINT)m_liveW * 4, 0);
        ++m_liveFrames;
        DistributeFrame(m_gdiTex, wantTex);
        return true;
    }
    if (m_liveSource != LiveSource::Dup) return false;

    if (!m_dup) {   // lost (mode change, UAC, lock screen): retry now and then
        ULONGLONG now = GetTickCount64();
        if (now < m_dupRetryAt) return false;
        m_dupRetryAt = now + 400;
        int w = 0, h = 0;
        if (!CreateDuplication(&w, &h)) return false;
        if (w != m_liveW || h != m_liveH) { m_dup->Release(); m_dup = nullptr; return false; }
    }

    DXGI_OUTDUPL_FRAME_INFO fi{};
    IDXGIResource* res = nullptr;
    HRESULT hr = m_dup->AcquireNextFrame(0, &fi, &res);
    if (hr == DXGI_ERROR_WAIT_TIMEOUT) return false;
    if (FAILED(hr)) {
        m_dup->Release(); m_dup = nullptr;
        m_dupRetryAt = GetTickCount64() + 400;
        return false;
    }
    bool got = false;
    if (fi.LastPresentTime.QuadPart != 0 && res) {
        ID3D11Texture2D* t = nullptr;
        if (SUCCEEDED(res->QueryInterface(IID_PPV_ARGS(&t))) && t) {
            DistributeFrame(t, wantTex);
            t->Release();
            got = true;
        }
    }
    if (res) res->Release();
    m_dup->ReleaseFrame();
    if (got) ++m_liveFrames;
    return got;
}

// Blurred live background, drawn small and throttled (the blur hides everything a high rate would show).
void CubeRenderer::DrawLive() {
    if (!m_liveBmp || !m_bgSurface || m_bgW <= 0) return;
    const float sigma = m_cfg.blurStrength;
    const bool blur = m_cfg.blur;
    DrawToSurface(m_bgSurface, [&](ID2D1DeviceContext* ctx, POINT off) {
        D2D1_SIZE_U ps = m_liveBmp->GetPixelSize();
        ctx->SetTransform(D2D1::Matrix3x2F::Scale((float)m_bgW / (float)ps.width, (float)m_bgH / (float)ps.height) *
                          D2D1::Matrix3x2F::Translation((float)off.x, (float)off.y));
        bool drawn = false;
        if (blur) {
            ID2D1Effect* fx = nullptr;
            if (SUCCEEDED(ctx->CreateEffect(CLSID_D2D1GaussianBlur, &fx)) && fx) {
                fx->SetInput(0, m_liveBmp);
                fx->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, sigma);
                fx->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, (UINT32)D2D1_BORDER_MODE_SOFT);
                ID2D1Image* out = nullptr;
                fx->GetOutput(&out);
                if (out) {
                    D2D1_POINT_2F pt = D2D1::Point2F(0.0f, 0.0f);
                    D2D1_RECT_F srcR = D2D1::RectF(0.0f, 0.0f, (float)ps.width, (float)ps.height);
                    ctx->DrawImage(out, &pt, &srcR, D2D1_INTERPOLATION_MODE_LINEAR, D2D1_COMPOSITE_MODE_SOURCE_OVER);
                    out->Release();
                    drawn = true;
                }
                fx->Release();
            }
        }
        if (!drawn) ctx->DrawBitmap(m_liveBmp, D2D1::RectF(0.0f, 0.0f, (float)ps.width, (float)ps.height));

        ID2D1SolidColorBrush* dimBrush = nullptr;
        ctx->CreateSolidColorBrush(D2D1::ColorF(8.0f / 255.0f, 10.0f / 255.0f, 16.0f / 255.0f, (float)m_cfg.dim / 100.0f), &dimBrush);
        if (dimBrush) {
            ctx->SetTransform(D2D1::Matrix3x2F::Translation((float)off.x, (float)off.y));
            ctx->FillRectangle(D2D1::RectF(0.0f, 0.0f, (float)m_bgW, (float)m_bgH), dimBrush);
            dimBrush->Release();
        }
    });
}

// Work area = monitor minus the taskbar (also when an auto-hide taskbar is currently shown).
void CubeRenderer::ComputeWorkArea(const RECT& mon, const RECT& work) {
    const float SW = (float)(mon.right - mon.left), SH = (float)(mon.bottom - mon.top);
    float l = (float)(work.left - mon.left), t = (float)(work.top - mon.top);
    float r = (float)(mon.right - work.right), b = (float)(mon.bottom - work.bottom);

    auto consider = [&](HWND tray) {
        if (!tray || !IsWindowVisible(tray)) return;
        if (MonitorFromWindow(tray, MONITOR_DEFAULTTONEAREST) != m_hMonitor) return;
        RECT tb{}, x{};
        if (!GetWindowRect(tray, &tb) || !IntersectRect(&x, &tb, &mon)) return;
        int tw = x.right - x.left, th = x.bottom - x.top;
        if (tw >= (mon.right - mon.left) - 4 && (float)th < SH / 3.0f) {
            if (x.top <= mon.top + 2) t = std::max(t, (float)(x.bottom - mon.top));
            else if (x.bottom >= mon.bottom - 2) b = std::max(b, (float)(mon.bottom - x.top));
        } else if (th >= (mon.bottom - mon.top) - 4 && (float)tw < SW / 3.0f) {
            if (x.left <= mon.left + 2) l = std::max(l, (float)(x.right - mon.left));
            else if (x.right >= mon.right - 2) r = std::max(r, (float)(mon.right - x.left));
        }
    };
    consider(FindWindowW(L"Shell_TrayWnd", nullptr));
    HWND sec = nullptr;
    while ((sec = FindWindowExW(nullptr, sec, L"Shell_SecondaryTrayWnd", nullptr)) != nullptr) consider(sec);

    l = std::clamp(l, 0.0f, SW * 0.4f); r = std::clamp(r, 0.0f, SW * 0.4f);
    t = std::clamp(t, 0.0f, SH * 0.4f); b = std::clamp(b, 0.0f, SH * 0.4f);
    m_wl = l / SW; m_wt = t / SH; m_wr = 1.0f - r / SW; m_wb = 1.0f - b / SH;
    m_workRect = { mon.left + (LONG)l, mon.top + (LONG)t, mon.right - (LONG)r, mon.bottom - (LONG)b };
}

// ---------------------------------------------------------------------------
// Alt+Tab style window switcher
// ---------------------------------------------------------------------------
struct AltEnumCtx {
    std::vector<AltWin>* out;
    RECT mon;
    HWND self;
    IVirtualDesktopManager* vdm;
};

static BOOL CALLBACK AltEnumProc(HWND h, LPARAM lp) {
    AltEnumCtx* c = (AltEnumCtx*)lp;
    if (c->out->size() >= (size_t)kMaxAlt) return FALSE;
    if (h == c->self || !IsWindowVisible(h) || IsIconic(h)) return TRUE;

    LONG ex = GetWindowLongW(h, GWL_EXSTYLE);
    const bool app = (ex & WS_EX_APPWINDOW) != 0;
    if (!app && (ex & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE))) return TRUE;
    if (!app && GetWindow(h, GW_OWNER)) return TRUE;

    DWORD cloaked = 0;
    DwmGetWindowAttribute(h, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
    if (cloaked) return TRUE;

    if (ex & WS_EX_LAYERED) {
        BYTE a = 255; DWORD fl = 0;
        if (GetLayeredWindowAttributes(h, nullptr, &a, &fl) && (fl & LWA_ALPHA) && a < 8) return TRUE;
    }

    WCHAR title[256]{};
    GetWindowTextW(h, title, ARRAYSIZE(title));
    if (!title[0]) return TRUE;

    WCHAR cls[128]{};
    GetClassNameW(h, cls, ARRAYSIZE(cls));
    static const wchar_t* kSkip[] = { L"Progman", L"WorkerW", L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd",
        L"Windows.UI.Core.CoreWindow", L"XamlExplorerHostIslandWindow", L"NotifyIconOverflowWindow",
        L"TopLevelWindowForOverflowXamlIsland" };
    for (const wchar_t* s : kSkip) if (_wcsicmp(cls, s) == 0) return TRUE;
    if (_wcsicmp(cls, kOverlayClassName) == 0) return TRUE;

    RECT rc{};
    if (FAILED(DwmGetWindowAttribute(h, DWMWA_EXTENDED_FRAME_BOUNDS, &rc, sizeof(rc)))) GetWindowRect(h, &rc);
    RECT cl{};
    if (!IntersectRect(&cl, &rc, &c->mon)) return TRUE;
    if ((cl.right - cl.left) < 120 || (cl.bottom - cl.top) < 80) return TRUE;

    if (c->vdm) {
        BOOL on = TRUE;
        if (SUCCEEDED(c->vdm->IsWindowOnCurrentVirtualDesktop(h, &on)) && !on) return TRUE;
    }

    AltWin w;
    w.hwnd = h;
    w.r = cl;
    w.title = title;
    c->out->push_back(w);
    return TRUE;
}

void CubeRenderer::CollectAltWindows() {
    m_alt.clear();
    if (!m_cfg.altTab) return;
    com_ptr<IVirtualDesktopManager> vdm;
    CoCreateInstance(CLSID_VirtualDesktopManager, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(vdm.put()));
    AltEnumCtx ctx{ &m_alt, m_workRect, m_hwnd, vdm.get() };
    EnumWindows(AltEnumProc, (LPARAM)&ctx);   // top-most first, like the Alt+Tab list
    LOG_DBG(L"Window switcher: %d windows", (int)m_alt.size());
}

// Window visuals are created now, their live surfaces on the first scroll notch.
void CubeRenderer::BuildAltVisuals() {
    m_altReady = false;
    m_altAttached = false;
    m_altShown = false;
    m_altOrderDirty = true;
    m_labelIdx = -1;
    m_selMode = false;
    m_selT = 0.0f;
    m_selIdx = 0;
    m_selSteps.store(0);
    m_winVis.clear();
    m_winBrush.clear();
    if (!m_cfg.altTab || !m_liveSurface || m_alt.empty()) return;

    for (int i = 0; i < (int)m_alt.size() && i < kMaxAlt; ++i) {
        SpriteVisual v = m_compositor.CreateSpriteVisual();
        v.IsVisible(false);
        m_winVis.push_back(v);
        m_winLift[i] = 0.0f;
        m_winOp[i] = 0.6f;
    }
    m_winBrush.assign(m_winVis.size(), CompositionSurfaceBrush{ nullptr });
    m_glow = m_compositor.CreateSpriteVisual();
    m_glow.Brush(m_compositor.CreateColorBrush({ 235, 56, 140, 255 }));
    m_glow.IsVisible(false);

    m_label = m_compositor.CreateSpriteVisual();
    m_label.Size({ 280.0f, 22.0f });
    m_labelSurface = MakeSurface(560, 44);
    m_labelBrush = m_compositor.CreateSurfaceBrush(m_labelSurface);
    m_label.Brush(m_labelBrush);
    m_label.IsVisible(false);
    m_altReady = true;
}

// Every window of the switcher gets a thumbnail card, drawn from the live screen frame.
// (It shows what is visible on screen, so a part hidden behind another window shows that window.)
bool CubeRenderer::EnsureAltSurfaces() {
    if (!m_altSurf.empty() && m_altSurf.size() == m_winVis.size()) return true;
    m_altSurf.clear();
    if (!m_liveBmp) return false;

    const float sx = (float)m_liveW / m_screenWidth, sy = (float)m_liveH / m_screenHeight;
    bool any = false;
    for (size_t i = 0; i < m_winVis.size(); ++i) {
        AltSurf s;
        const RECT r = m_alt[i].r;
        int x0 = (int)std::lround((float)(r.left - m_monitorRect.left) * sx) - (int)m_workPx.left;
        int y0 = (int)std::lround((float)(r.top - m_monitorRect.top) * sy) - (int)m_workPx.top;
        int x1 = (int)std::lround((float)(r.right - m_monitorRect.left) * sx) - (int)m_workPx.left;
        int y1 = (int)std::lround((float)(r.bottom - m_monitorRect.top) * sy) - (int)m_workPx.top;
        x0 = std::clamp(x0, 0, m_workW); x1 = std::clamp(x1, 0, m_workW);
        y0 = std::clamp(y0, 0, m_workH); y1 = std::clamp(y1, 0, m_workH);
        if (x1 - x0 >= 16 && y1 - y0 >= 16) {
            const float k = std::min(1.0f, 960.0f / (float)(x1 - x0));
            s.w = std::max(16, (int)((float)(x1 - x0) * k));
            s.h = std::max(16, (int)((float)(y1 - y0) * k));
            s.surf = MakeSurface(s.w, s.h);
            if (s.surf) { s.box = { x0, y0, x1, y1 }; s.ok = true; }
        }
        m_altSurf.push_back(s);
        if (s.ok) {
            DrawThumb((int)i);
            m_winBrush[i] = m_compositor.CreateSurfaceBrush(s.surf);
            m_winBrush[i].Stretch(CompositionStretch::Fill);
            m_winVis[i].Brush(m_winBrush[i]);
            any = true;
        }
    }
    LOG_DBG(L"Window switcher: %d thumbnail cards", (int)m_altSurf.size());
    return any;
}

void CubeRenderer::DrawThumb(int i) {
    if (i < 0 || i >= (int)m_altSurf.size() || !m_liveBmp) return;
    AltSurf& s = m_altSurf[i];
    if (!s.ok) return;
    const D2D1_RECT_F src = D2D1::RectF((float)s.box.left, (float)s.box.top, (float)s.box.right, (float)s.box.bottom);
    if (s.snapValid && !s.snap.empty() && AltIsOccluded(m_alt, i)) {   // covered window: show its real pixels
        DrawToSurface(s.surf, [&](ID2D1DeviceContext* ctx, POINT off) {
            ctx->SetTransform(D2D1::Matrix3x2F::Translation((float)off.x, (float)off.y));
            ID2D1Bitmap* sb = nullptr;
            if (SUCCEEDED(ctx->CreateBitmap(D2D1::SizeU(s.snapW, s.snapH), s.snap.data(), (UINT32)s.snapW * 4,
                    D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE)), &sb)) && sb) {
                const D2D1_RECT_F ss = D2D1::RectF((float)s.snapSrc.left, (float)s.snapSrc.top, (float)s.snapSrc.right, (float)s.snapSrc.bottom);
                ctx->DrawBitmap(sb, D2D1::RectF(0.0f, 0.0f, (float)s.w, (float)s.h), 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &ss);
                sb->Release();
            }
        });
        return;
    }
    DrawToSurface(s.surf, [&](ID2D1DeviceContext* ctx, POINT off) {
        ctx->SetTransform(D2D1::Matrix3x2F::Translation((float)off.x, (float)off.y));
        ctx->DrawBitmap(m_liveBmp, D2D1::RectF(0.0f, 0.0f, (float)s.w, (float)s.h), 1.0f,
                        D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &src);
    });
}

// Selected card every frame, one other card per frame (round robin).
void CubeRenderer::UpdateThumbs() {
    if (!m_selMode) return;
    const int n = (int)m_altSurf.size();
    if (n <= 0) return;
    if (m_selIdx >= 0 && m_selIdx < n && m_altSurf[m_selIdx].ok && m_selIdx < (int)m_alt.size() && AltIsOccluded(m_alt, m_selIdx)) {
        // The selected window is covered: grab its own pixels (once on selection, then about 3 times a second).
        AltSurf& ss = m_altSurf[m_selIdx];
        const ULONGLONG t = GetTickCount64();
        if (ss.snapTick == 0 || t - ss.snapTick > 300) {
            ss.snapTick = t;
            if (CaptureWindowPixels(m_alt[m_selIdx].hwnd, m_alt[m_selIdx].r, ss)) DrawThumb(m_selIdx);
        }
    }
    if (!m_thumbFresh) return;
    m_thumbFresh = false;
    DrawThumb(m_selIdx);
    if (n > 1 && (m_frameCounter & 3) == 0) {
        m_thumbRR = (m_thumbRR + 1) % n;
        if (m_thumbRR == m_selIdx) m_thumbRR = (m_thumbRR + 1) % n;
        DrawThumb(m_thumbRR);
    }
}

void CubeRenderer::RedrawLabel() {
    m_labelIdx = m_selIdx;
    if (!m_labelSurface || m_selIdx < 0 || m_selIdx >= (int)m_alt.size()) return;
    const std::wstring title = m_alt[m_selIdx].title;
    DrawToSurface(m_labelSurface, [&](ID2D1DeviceContext* ctx, POINT off) {
        ctx->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
        ctx->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
        ctx->SetTransform(D2D1::Matrix3x2F::Scale(2.0f, 2.0f) *
                          D2D1::Matrix3x2F::Translation((float)off.x, (float)off.y));
        ID2D1SolidColorBrush* pill = nullptr;
        ID2D1SolidColorBrush* txt = nullptr;
        ctx->CreateSolidColorBrush(D2D1::ColorF(0.05f, 0.06f, 0.09f, 0.90f), &pill);
        ctx->CreateSolidColorBrush(D2D1::ColorF(0.95f, 0.97f, 1.0f, 1.0f), &txt);
        if (pill && txt && m_textFormatBadge) {
            ctx->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(0.0f, 0.0f, 280.0f, 22.0f), 11.0f, 11.0f), pill);
            ctx->DrawText(title.c_str(), (UINT32)title.length(), m_textFormatBadge,
                          D2D1::RectF(10.0f, 0.0f, 270.0f, 22.0f), txt, D2D1_DRAW_TEXT_OPTIONS_CLIP);
        }
        if (pill) pill->Release();
        if (txt) txt->Release();
    });
}

void CubeRenderer::UpdateAltTab(float dt) {
    if (!m_altReady) return;
    const int n = (int)m_alt.size();
    int st = m_selSteps.exchange(0);
    if (st != 0 && m_phase == Phase::Drag && n > 0) {
        if (!m_selMode) {
            if (!EnsureAltSurfaces()) return;
            m_selMode = true;
            m_selIdx = 0;
            for (int i = 0; i < n; ++i) { m_winLift[i] = 0.0f; m_winOp[i] = 0.6f; }
        }
        m_selIdx = ((m_selIdx + st) % n + n) % n;
        m_altOrderDirty = true;
        m_altDirty = true;
    }

    const float target = (m_selMode && m_phase == Phase::Drag) ? 1.0f : 0.0f;
    const float k = 1.0f - std::exp(-dt * 16.0f);
    const float prev = m_selT;
    m_selT += (target - m_selT) * k;
    if (std::fabs(target - m_selT) < 0.003f) m_selT = target;
    bool moved = (m_selT != prev);

    for (int i = 0; i < n && i < kMaxAlt; ++i) {
        float tl = m_selMode ? ((i == m_selIdx) ? m_cfg.altLift : m_cfg.altLift * 0.35f) : 0.0f;
        float to = (i == m_selIdx) ? 1.0f : 0.35f;
        float nl = m_winLift[i] + (tl - m_winLift[i]) * k;
        float no = m_winOp[i] + (to - m_winOp[i]) * k;
        if (std::fabs(nl - m_winLift[i]) > 0.01f || std::fabs(no - m_winOp[i]) > 0.001f) moved = true;
        m_winLift[i] = nl;
        m_winOp[i] = no;
    }
    if (moved) m_altDirty = true;
}

void CubeRenderer::HideAlt() {
    if (!m_altShown) return;
    for (auto& v : m_winVis) if (v) v.IsVisible(false);
    if (m_glow) m_glow.IsVisible(false);
    if (m_label) m_label.IsVisible(false);
    m_altShown = false;
}

void CubeRenderer::LayoutAlt(const Mat4& view, float faceDeg, float lifeEase) {
    if (!m_altReady || !m_cubeContainer) return;
    const int n = std::min((int)m_winVis.size(), kMaxAlt);
    if (n <= 0 || m_selIdx < 0 || m_selIdx >= n || (int)m_altSurf.size() < n) return;

    const float W = m_faceWidth, H = m_faceHeight, D = m_D;
    const float workW = (float)(m_workRect.right - m_workRect.left);
    const float workH = (float)(m_workRect.bottom - m_workRect.top);
    if (workW < 2.0f || workH < 2.0f) return;

    if (!m_altAttached || m_altOrderDirty) {
        auto ch = m_cubeContainer.Children();
        if (m_altAttached) {
            for (int i = 0; i < n; ++i) { try { ch.Remove(m_winVis[i]); } catch (...) {} }
            try { ch.Remove(m_glow); } catch (...) {}
            try { ch.Remove(m_label); } catch (...) {}
        }
        for (int i = n - 1; i >= 0; --i) if (i != m_selIdx) ch.InsertAtTop(m_winVis[i]);
        ch.InsertAtTop(m_glow);
        ch.InsertAtTop(m_winVis[m_selIdx]);
        ch.InsertAtTop(m_label);
        m_altAttached = true;
        m_altOrderDirty = false;
    }
    if (m_labelIdx != m_selIdx) RedrawLabel();

    Mat4 rotFace = Mat4::RotationY(faceDeg * kPi / 180.0f);
    float selX = 0.0f, selY = 0.0f, selW = 0.0f, selLift = 0.0f;
    for (int i = 0; i < n; ++i) {
        auto& v = m_winVis[i];
        if (!m_altSurf[i].ok) { v.IsVisible(false); continue; }
        const RECT rc = m_alt[i].r;
        float fw = (float)(rc.right - rc.left) / workW * W, fh = (float)(rc.bottom - rc.top) / workH * H;
        float fx = (float)(rc.left - m_workRect.left) / workW * W;
        float fy = (float)(rc.top - m_workRect.top) / workH * H;
        if (i == m_selIdx) {   // the selected card grows a little around its centre
            const float gs = 1.0f + 0.08f * m_selT;
            fx -= fw * (gs - 1.0f) * 0.5f; fy -= fh * (gs - 1.0f) * 0.5f;
            fw *= gs; fh *= gs;
        }

        v.Size({ fw, fh });
        const float lift = m_winLift[i] * lifeEase;
        v.TransformMatrix((Mat4::Translation(-W * 0.5f + fx, -H * 0.5f + fy, 0.0f) *
                           Mat4::Translation(0.0f, 0.0f, D + lift) * rotFace * view).ToWinRT());
        v.Opacity(std::clamp(m_winOp[i] * m_selT, 0.0f, 1.0f));
        v.IsVisible(true);
        if (i == m_selIdx) { selX = fx; selY = fy; selW = fw; selLift = lift; m_glow.Size({ fw + 24.0f, fh + 24.0f }); }
    }

    // highlight frame just under the selected window, name tag above it
    m_glow.TransformMatrix((Mat4::Translation(-W * 0.5f + selX - 12.0f, -H * 0.5f + selY - 12.0f, 0.0f) *
                            Mat4::Translation(0.0f, 0.0f, D + selLift - 0.5f) * rotFace * view).ToWinRT());
    m_glow.Opacity(std::clamp(1.0f * m_selT, 0.0f, 1.0f));
    m_glow.IsVisible(true);

    float ly = selY - 30.0f;
    if (ly < 4.0f) ly = selY + 8.0f;
    m_label.TransformMatrix((Mat4::Translation(-W * 0.5f + selX + selW * 0.5f - 140.0f, -H * 0.5f + ly, 0.0f) *
                             Mat4::Translation(0.0f, 0.0f, D + selLift + 2.0f) * rotFace * view).ToWinRT());
    m_label.Opacity(std::clamp(m_selT, 0.0f, 1.0f));
    m_label.IsVisible(true);
    m_altShown = true;
}

void CubeRenderer::BuildVisuals() {
    BuildBackground();
    if (!m_backgroundVisual) {   // only needed when there is no blurred background
        m_backdropVisual = m_compositor.CreateSpriteVisual();
        m_backdropVisual.Size({ m_screenWidth, m_screenHeight });
        m_backdropVisual.Brush(m_compositor.CreateColorBrush({ 255, 8, 10, 16 }));
        m_rootVisual.Children().InsertAtBottom(m_backdropVisual);
    }

    if (m_cfg.shadow) RenderShadow();

    m_cubeContainer = m_compositor.CreateContainerVisual();
    m_cubeContainer.Size({ m_screenWidth, m_screenHeight });
    m_rootVisual.Children().InsertAtTop(m_cubeContainer);

    BuildFaceVisuals();
    BuildOverlayFx();
}

void CubeRenderer::BuildFaceVisuals() {
    const float W = m_faceWidth, H = m_faceHeight;
    for (int i = 0; i < m_faceCount; ++i) {
        auto& f = m_faces[i];
        f.container = m_compositor.CreateContainerVisual();
        f.container.Size({ W, H });

        f.background = m_compositor.CreateSpriteVisual();
        f.background.Size({ W, H });
        f.background.Brush(m_compositor.CreateColorBrush({ 255, 24, 28, 38 }));
        f.container.Children().InsertAtBottom(f.background);

        f.rim = m_compositor.CreateSpriteVisual();   // thin glass edge light behind the face
        f.rim.Size({ W + 4.0f, H + 4.0f });
        f.rim.Offset({ -2.0f, -2.0f, 0.0f });
        f.rim.Brush(m_compositor.CreateColorBrush({ 255, 255, 255, 255 }));
        f.rim.Opacity(0.0f);
        f.container.Children().InsertAtBottom(f.rim);

        f.shade = m_compositor.CreateSpriteVisual();
        f.shade.Size({ W, H });
        f.shade.Brush(m_compositor.CreateColorBrush({ 255, 0, 0, 0 }));
        f.shade.Opacity(0.0f);
        f.container.Children().InsertAtTop(f.shade);

        f.container.IsVisible(false);
        m_cubeContainer.Children().InsertAtTop(f.container);

        f.layer = m_compositor.CreateSpriteVisual();
        f.layer.Size({ kBadgeW, kBadgeH });
        f.layer.Opacity(0.0f);
        f.layer.IsVisible(false);
        m_cubeContainer.Children().InsertAtTop(f.layer);
    }
    m_zOrderValid = false;
}


// ---------------------------------------------------------------------------
// Add / remove desktops from the cube
// ---------------------------------------------------------------------------
// Rebuilds the prism for the current desktop count. The face textures survive, only the visuals change size.
void CubeRenderer::RebuildPrism() {
    if (!m_cubeContainer) return;
    FaceVisuals keep[kMaxFaces];
    for (int i = 0; i < kMaxFaces; ++i) keep[i] = m_faces[i];

    ComputeGeometry(m_desktopList.TotalCount());

    m_cubeContainer.Children().RemoveAll();
    m_altAttached = false;
    for (int i = 0; i < kMaxFaces; ++i) m_faces[i] = FaceVisuals{};
    BuildFaceVisuals();

    for (int i = 0; i < m_faceCount; ++i) {
        auto& f = m_faces[i];
        const auto& o = keep[i];
        f.surface = o.surface;
        f.layerSurface = o.layerSurface;
        f.brush = o.brush;
        f.layerBrush = o.layerBrush;
        f.liveBrush = o.liveBrush;
        f.renderedDesktop = o.renderedDesktop;
        f.renderedVersion = o.renderedVersion;
        f.usingLive = false;
        if (f.brush) f.background.Brush(f.brush);
        if (f.layerBrush && f.layer) f.layer.Brush(f.layerBrush);
    }
    m_zOrderValid = false;
    for (auto& a : m_applied) a = 1e9f;
}

void CubeRenderer::KickComposer() {
    const int total = m_desktopList.TotalCount();
    if (total <= 1) return;
    bool busy = m_composeThread && WaitForSingleObject(m_composeThread, 0) == WAIT_TIMEOUT;
    if (busy) { m_composeRestart = true; return; }
    if (m_composeThread) { CloseHandle(m_composeThread); m_composeThread = nullptr; }
    Composer::s_abort.store(false);
    auto job = std::make_unique<Composer::Job>();
    job->mon = m_monitorRect;
    job->list = m_desktopList;
    m_composeThread = Composer::Start(std::move(job));
    m_composeRestart = false;
}

void CubeRenderer::DrawZoneSurface(bool right) {
    ZoneVisual& z = right ? m_zoneR : m_zoneL;
    if (!z.surf) return;
    const float w = m_zoneW, h = m_screenHeight;
    DrawToSurface(z.surf, [&](ID2D1DeviceContext* ctx, POINT off) {
        ctx->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
        ctx->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
        ctx->SetTransform(D2D1::Matrix3x2F::Translation((float)off.x, (float)off.y));

        // Frosted gray gradient: strongest at the screen edge, gone towards the cube.
        D2D1_GRADIENT_STOP stops[3] = {
            { 0.0f, D2D1::ColorF(0.74f, 0.77f, 0.84f, 0.60f) },
            { 0.55f, D2D1::ColorF(0.64f, 0.67f, 0.74f, 0.26f) },
            { 1.0f, D2D1::ColorF(0.60f, 0.63f, 0.70f, 0.0f) }
        };
        ID2D1GradientStopCollection* gs = nullptr;
        ctx->CreateGradientStopCollection(stops, 3, &gs);
        if (gs) {
            const float x0 = right ? w : 0.0f, x1 = right ? 0.0f : w;
            ID2D1LinearGradientBrush* lb = nullptr;
            ctx->CreateLinearGradientBrush(
                D2D1::LinearGradientBrushProperties(D2D1::Point2F(x0, 0.0f), D2D1::Point2F(x1, 0.0f)), gs, &lb);
            if (lb) { ctx->FillRectangle(D2D1::RectF(0.0f, 0.0f, w, h), lb); lb->Release(); }
            gs->Release();
        }

        // Round button with + or - and a caption.
        const float cx = w * 0.5f, cy = h * 0.5f, r = 30.0f;
        ID2D1SolidColorBrush* fill = nullptr;
        ID2D1SolidColorBrush* line = nullptr;
        ID2D1SolidColorBrush* txt = nullptr;
        ctx->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.16f), &fill);
        ctx->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.92f), &line);
        ctx->CreateSolidColorBrush(D2D1::ColorF(0.96f, 0.97f, 1.0f, 0.95f), &txt);
        if (fill && line && txt) {
            ctx->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), r, r), fill);
            ctx->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), r, r), line, 2.0f);
            ctx->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(cx - 13.0f, cy - 2.0f, cx + 13.0f, cy + 2.0f), 2.0f, 2.0f), line);
            if (right) {
                ctx->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(cx - 2.0f, cy - 13.0f, cx + 2.0f, cy + 13.0f), 2.0f, 2.0f), line);
            }
            if (m_textFormatBadge) {
                const wchar_t* cap = right ? L"Add desktop" : L"Remove desktop";
                ctx->DrawText(cap, (UINT32)wcslen(cap), m_textFormatBadge,
                              D2D1::RectF(0.0f, cy + r + 8.0f, w, cy + r + 32.0f), txt, D2D1_DRAW_TEXT_OPTIONS_NONE);
                const wchar_t* hint = L"right-click";
                ctx->DrawText(hint, (UINT32)wcslen(hint), m_textFormatBadge,
                              D2D1::RectF(0.0f, cy + r + 30.0f, w, cy + r + 52.0f), txt, D2D1_DRAW_TEXT_OPTIONS_NONE);
            }
        }
        if (fill) fill->Release();
        if (line) line->Release();
        if (txt) txt->Release();
    });
}

void CubeRenderer::BuildOverlayFx() {
    m_zoneL = ZoneVisual{};
    m_zoneR = ZoneVisual{};
    m_toast = nullptr;
    m_toastSurface = nullptr;
    m_toastT = 0.0f;
    if (!m_cfg.edgeEdit) return;

    m_zoneW = std::clamp(m_screenWidth * m_cfg.edgeZone, 120.0f, 360.0f);
    for (int side = 0; side < 2; ++side) {
        ZoneVisual& z = side ? m_zoneR : m_zoneL;
        z.surf = MakeSurface((int)m_zoneW, (int)m_screenHeight);
        z.vis = m_compositor.CreateSpriteVisual();
        z.vis.Size({ m_zoneW, m_screenHeight });
        z.vis.CenterPoint({ side ? m_zoneW : 0.0f, m_screenHeight * 0.5f, 0.0f });
        z.baseX = side ? m_screenWidth - m_zoneW : 0.0f;
        z.vis.Offset({ z.baseX + (side ? 1.0f : -1.0f) * m_zoneW * 0.3f, 0.0f, 0.0f });   // starts hidden, slightly off screen
        auto br = m_compositor.CreateSurfaceBrush(z.surf);
        br.Stretch(CompositionStretch::Fill);
        z.vis.Brush(br);
        z.vis.Opacity(0.0f);
        DrawZoneSurface(side != 0);
        m_rootVisual.Children().InsertAtTop(z.vis);
    }

    m_toastSurface = MakeSurface(640, 56);
    m_toast = m_compositor.CreateSpriteVisual();
    m_toast.Size({ 320.0f, 28.0f });
    m_toast.Offset({ (m_screenWidth - 320.0f) * 0.5f, 36.0f, 0.0f });
    auto tb = m_compositor.CreateSurfaceBrush(m_toastSurface);
    tb.Stretch(CompositionStretch::Fill);
    m_toast.Brush(tb);
    m_toast.Opacity(0.0f);
    m_rootVisual.Children().InsertAtTop(m_toast);
}

void CubeRenderer::ShowToast(const wchar_t* text) {
    if (!m_toastSurface || !m_toast) return;
    std::wstring msg = text;
    DrawToSurface(m_toastSurface, [&](ID2D1DeviceContext* ctx, POINT off) {
        ctx->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
        ctx->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
        ctx->SetTransform(D2D1::Matrix3x2F::Scale(2.0f, 2.0f) *
                          D2D1::Matrix3x2F::Translation((float)off.x, (float)off.y));
        ID2D1SolidColorBrush* pill = nullptr;
        ID2D1SolidColorBrush* txt = nullptr;
        ctx->CreateSolidColorBrush(D2D1::ColorF(0.05f, 0.06f, 0.09f, 0.88f), &pill);
        ctx->CreateSolidColorBrush(D2D1::ColorF(0.95f, 0.97f, 1.0f, 1.0f), &txt);
        if (pill && txt && m_textFormatBadge) {
            ctx->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(0.0f, 0.0f, 320.0f, 28.0f), 14.0f, 14.0f), pill);
            ctx->DrawText(msg.c_str(), (UINT32)msg.length(), m_textFormatBadge,
                          D2D1::RectF(10.0f, 0.0f, 310.0f, 28.0f), txt, D2D1_DRAW_TEXT_OPTIONS_CLIP);
        }
        if (pill) pill->Release();
        if (txt) txt->Release();
    });
    m_toastT = 1.0f;
}

// Zones fade in at the edges, flash when used and then vanish for a moment. Also steps spin / pop / birth.
void CubeRenderer::StepFx(float dt) {
    bool moving = false;

    if (m_kick.x != 0.0f || m_kick.v != 0.0f) {
        m_kick.Step(0.0f, 9.0f, 0.55f, dt);
        if (std::fabs(m_kick.x) < 0.01f && std::fabs(m_kick.v) < 0.05f) { m_kick.x = 0.0f; m_kick.v = 0.0f; }
        moving = true;
    }
    if (m_pop.x != 0.0f || m_pop.v != 0.0f) {
        m_pop.Step(0.0f, 11.0f, 0.5f, dt);
        if (std::fabs(m_pop.x) < 0.0005f && std::fabs(m_pop.v) < 0.005f) { m_pop.x = 0.0f; m_pop.v = 0.0f; }
        moving = true;
    }
    if (m_bornT < 1.0f) {
        m_bornT = std::min(1.0f, m_bornT + dt / 0.6f);
        moving = true;
    }
    if (moving) m_fxDirty = true;

    if (m_toast && m_toastT > 0.0f) {
        m_toastT = std::max(0.0f, m_toastT - dt / 1.6f);
        float a = std::min(1.0f, m_toastT * 6.0f) * std::min(1.0f, (1.0f - m_toastT) * 10.0f + 0.001f);
        m_toast.Opacity(m_toastT > 0.0f ? a : 0.0f);
        moving = true;
    }

    if (m_zoneL.vis && m_zoneR.vis) {
        POINT cp{};
        GetCursorPos(&cp);
        const bool inY = cp.y >= m_monitorRect.top && cp.y < m_monitorRect.bottom;
        const LONG distL = cp.x - m_monitorRect.left;           // distance to the left screen edge
        const LONG distR = m_monitorRect.right - 1 - cp.x;      // distance to the right screen edge
        const LONG revealW = (LONG)std::min<float>((float)m_cfg.edgeReveal, m_zoneW);
        const bool open = m_phase == Phase::Drag && m_zoom.x > 0.6f && !m_vd.active &&
                          std::chrono::steady_clock::now() >= m_zoneQuietUntil;
        const float kIn = 1.0f - std::exp(-dt * 14.0f);
        const float kOut = 1.0f - std::exp(-dt * 9.0f);
        for (int side = 0; side < 2; ++side) {
            ZoneVisual& z = side ? m_zoneR : m_zoneL;
            const LONG d = side ? distR : distL;
            // Reveal when the pointer touches the edge strip, keep it while the pointer stays inside the zone.
            if (!open || !inY || d < 0 || d >= (LONG)m_zoneW) z.shown = false;
            else if (d < revealW) z.shown = true;
            const float target = z.shown ? 1.0f : 0.0f;
            const float prevOp = z.op, prevPulse = z.pulse;
            z.op += (target - z.op) * (target > z.op ? kIn : kOut);
            if (std::fabs(target - z.op) < 0.003f) z.op = target;
            z.pulse *= std::exp(-dt * 4.5f);
            if (z.pulse < 0.01f) z.pulse = 0.0f;
            if (z.op != prevOp || z.pulse != prevPulse) {
                const float vis = std::min(1.0f, z.op + z.pulse * 0.6f);
                z.vis.Opacity(vis);
                z.vis.IsVisible(vis > 0.002f);   // fully hidden zones cost nothing
                const float slide = (1.0f - z.op) * m_zoneW * 0.3f;
                z.vis.Offset({ z.baseX + (side ? slide : -slide), 0.0f, 0.0f });
                z.vis.Scale({ 1.0f + 0.7f * z.pulse, 1.0f, 1.0f });
                moving = true;
            }
        }
    }

    if (moving) m_busy = true;
}

bool CubeRenderer::OnRightDown(POINT pt) {
    if (!m_cfg.edgeEdit || !m_sceneActive.load() || !m_zoneL.vis) return false;
    if (m_phase != Phase::Drag || m_zoom.x < 0.8f) return false;
    if (pt.y < m_monitorRect.top || pt.y >= m_monitorRect.bottom) return false;
    const bool L = pt.x >= m_monitorRect.left && pt.x < m_monitorRect.left + (LONG)m_zoneW;
    const bool R = pt.x < m_monitorRect.right && pt.x >= m_monitorRect.right - (LONG)m_zoneW;
    if (!L && !R) return false;
    // A zone that is not showing is not clickable (no invisible buttons).
    if (!(R ? m_zoneR : m_zoneL).shown || (R ? m_zoneR : m_zoneL).op < 0.35f) return false;
    if (m_vd.active || m_pendingVdOp.load() != 0 || std::chrono::steady_clock::now() < m_vdCooldown) return true;   // swallowed, ignored
    (R ? m_zoneR : m_zoneL).pulse = 1.0f;
    m_zoneQuietUntil = std::chrono::steady_clock::now() + std::chrono::milliseconds(1300);
    // This runs inside the WH_MOUSE_LL callback, so it must return at once. Only record the request,
    // Frame() does the actual work (COM, registry, drawing).
    m_pendingVdOp.store(R ? 1 : 2);
    Wake();
    return true;
}

void CubeRenderer::StartVdOp(int kind) {
    VDManager::DesktopList now;
    VDManager::QueryDesktops(now);
    const int T = now.TotalCount();
    int c = now.currentIndex;
    int rc = -1;
    if (VDManager::ReadCurrentIndex(rc) && rc >= 0 && rc < T) c = rc;

    if (kind == 1 && T >= kMaxDesktops) { ShowToast(L"Desktop limit reached"); return; }
    if (kind == 2 && T <= 1) { ShowToast(L"Can't remove the only desktop"); return; }

    const auto t = std::chrono::steady_clock::now();
    m_vd = VdTask{};
    m_vd.active = true;
    m_vd.kind = kind;
    m_vd.oldTotal = T;
    m_vd.origCur = c;
    m_vd.goalA = (kind == 1) ? c : (T - 1);   // adding acts where we are, removing acts on the last desktop
    m_vd.stage = 0;
    m_vd.nextAt = t;
    m_vd.deadline = t + std::chrono::milliseconds(4500);
    if (m_cfg.suppressSysAnim) VDManager::SuppressSystemAnimation(true);
    LOG_DBG(L"Desktop %s: total=%d current=%d", kind == 1 ? L"add" : L"remove", T, c);
}

// Runs every frame while an add / remove is in progress. All of it happens under the opaque overlay.
void CubeRenderer::UpdateVdOp() {
    if (!m_vd.active) return;
    const auto now = std::chrono::steady_clock::now();
    const bool timedOut = now > m_vd.deadline;
    if (now < m_vd.nextAt && !timedOut) return;
    const auto ms = [](int v) { return std::chrono::milliseconds(v); };

    if (m_vd.stage == 0 || m_vd.stage == 3) {
        const int goal = (m_vd.stage == 0) ? m_vd.goalA : m_vd.goalC;
        int cur = -1;
        if (!VDManager::ReadCurrentIndex(cur)) cur = goal;
        if (m_vd.waiting) {
            if (cur != m_vd.waitCur) m_vd.waiting = false;
            else if (now > m_vd.waitDeadline || timedOut) { FinishVdOp(false); return; }
            else { m_vd.nextAt = now + ms(8); return; }
        }
        if (cur == goal || timedOut) {
            if (m_vd.stage == 3 || timedOut) { FinishVdOp(!timedOut); return; }
            m_vd.stage = 1;
        } else {
            VDManager::SendSwitchStep(goal > cur ? 1 : -1);
            m_vd.waiting = true;
            m_vd.waitCur = cur;
            m_vd.waitDeadline = now + ms(900);
            m_vd.nextAt = now + ms(45);
            return;
        }
    }

    if (m_vd.stage == 1) {
        if (m_vd.kind == 1) VDManager::SendAddDesktop(); else VDManager::SendCloseDesktop();
        m_vd.stage = 2;
        m_vd.actDeadline = now + ms(1600);
        m_vd.nextAt = now + ms(40);
        return;
    }

    if (m_vd.stage == 2) {
        int total = -1;
        const int want = (m_vd.kind == 1) ? m_vd.oldTotal + 1 : m_vd.oldTotal - 1;
        if (VDManager::ReadTotalCount(total) && total == want) {
            int cur = -1;
            VDManager::ReadCurrentIndex(cur);
            if (m_vd.kind == 1) m_vd.goalC = m_vd.origCur;
            else m_vd.goalC = (m_vd.origCur < m_vd.oldTotal - 1) ? m_vd.origCur : (cur >= 0 ? cur : 0);
            m_vd.stage = 3;
            m_vd.waiting = false;
            m_vd.nextAt = now + ms(40);
        } else if (now > m_vd.actDeadline || timedOut) {
            FinishVdOp(false);
        } else {
            m_vd.nextAt = now + ms(10);
        }
    }
}

void CubeRenderer::FinishVdOp(bool ok) {
    m_vd.active = false;
    if (m_cfg.suppressSysAnim) VDManager::SuppressSystemAnimation(false);
    m_vdCooldown = std::chrono::steady_clock::now() + std::chrono::milliseconds(300);

    VDManager::DesktopList nl;
    VDManager::QueryDesktops(nl);
    int rc = -1;
    if (VDManager::ReadCurrentIndex(rc) && rc >= 0 && rc < nl.TotalCount()) nl.currentIndex = rc;

    if (nl.TotalCount() != m_vd.oldTotal) {
        ApplyDesktopCountChange(nl, m_vd.oldTotal);
    } else {
        ShowToast(m_vd.kind == 1 ? L"Could not add a desktop" : L"Could not remove the desktop");
        m_desktopList = nl;
        if (m_liveDesktop != nl.currentIndex) { m_liveDesktop = nl.currentIndex; m_applied[0] = 1e9f; }
    }
    (void)ok;
}

void CubeRenderer::ApplyDesktopCountChange(const VDManager::DesktopList& nl, int oldTotal) {
    const bool added = nl.TotalCount() > oldTotal;
    const int oldFaces = m_faceCount;
    const float oldStep = m_stepDeg;

    m_desktopList = nl;
    const int total = nl.TotalCount();
    if (m_liveDesktop != nl.currentIndex) m_liveDesktop = nl.currentIndex;

    if (std::clamp(total, 1, kMaxFaces) != oldFaces) {
        RebuildPrism();
        const float k = (oldStep > 0.0f) ? m_stepDeg / oldStep : 1.0f;   // the same face stays in front
        m_yaw.x *= k; m_yaw.v *= k;
        GestureController::RescaleYaw(k);
    }

    m_kick.x = (added ? 1.0f : -1.0f) * m_stepDeg * 0.9f;
    m_kick.v = 0.0f;
    m_pop.x = -0.30f;
    m_pop.v = 0.0f;
    if (added) { m_bornDesktop = total - 1; m_bornT = 0.0f; } else { m_bornDesktop = -1; m_bornT = 1.0f; }
    m_fxDirty = true;
    m_busy = true;
    m_zOrderValid = false;
    for (auto& a : m_applied) a = 1e9f;

    KickComposer();

    wchar_t msg[96];
    if (added) swprintf_s(msg, L"Desktop %d added   (%d desktops)", total, total);
    else swprintf_s(msg, L"Desktop %d removed   (%d desktops)", oldTotal, total);
    ShowToast(msg);
    LOG_INF(L"Desktops now %d (faces %d)", total, m_faceCount);
}

void CubeRenderer::ComputeGeometry(int total) {
    m_faceCount = std::clamp(total, 1, kMaxFaces);
    m_stepDeg = 360.0f / (float)m_faceCount;

    // Prism geometry. Face width is chosen so the prism footprint matches a 4 sided cube.
    float W4 = m_screenWidth * m_cfg.faceSize;
    float R4 = W4 * 0.70711f;
    float W;
    if (m_faceCount >= 3) W = 2.0f * R4 * std::sin(kPi / (float)m_faceCount);
    else W = W4 * 1.15f;
    W = std::clamp(W, m_screenWidth * 0.30f, m_screenWidth * 0.94f);
    m_faceWidth = W;
    m_faceHeight = W * (m_screenHeight / m_screenWidth);
    float gapMul = 1.0f + m_cfg.faceGap * 2.0f;
    if (m_faceCount >= 3) m_D = (W * 0.5f) / std::tan(kPi / (float)m_faceCount) * gapMul;
    else if (m_faceCount == 2) m_D = W * 0.10f * gapMul;
    else m_D = W * 0.04f;
}

bool CubeRenderer::BeginScene() {
    // ---- monitor / geometry ----
    m_hMonitor = MonitorFromPoint(m_anchor, MONITOR_DEFAULTTONEAREST);
    MONITORINFOEXW mi{};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(m_hMonitor, &mi)) return false;
    m_monitorRect = mi.rcMonitor;
    int monW = mi.rcMonitor.right - mi.rcMonitor.left;
    int monH = mi.rcMonitor.bottom - mi.rcMonitor.top;
    if (monW <= 0 || monH <= 0) return false;

    m_screenWidth = (float)monW;
    m_screenHeight = (float)monH;
    {
        DEVMODEW dm{};
        dm.dmSize = sizeof(dm);
        m_monitorHz = EnumDisplaySettingsW(mi.szDevice, ENUM_CURRENT_SETTINGS, &dm) ? (float)dm.dmDisplayFrequency : 0.0f;
    }
    ComputeWorkArea(mi.rcMonitor, mi.rcWork);
    if (m_canExclude && ExcludeTaskbarsFromCapture()) {
        // Taskbar is invisible to capture now: the faces use the whole screen, nothing is cropped.
        m_wl = 0.0f; m_wt = 0.0f; m_wr = 1.0f; m_wb = 1.0f;
        m_workRect = mi.rcMonitor;
        DwmFlush(); DwmFlush();   // let DWM drop the taskbar from the next captured frame
        LOG_INF(L"Taskbar hidden from capture, faces are uncropped");
    } else {
        RestoreTaskbarCapture();
        LOG_INF(L"Taskbar could not be hidden from capture, using cropped work area");
    }

    // ---- desktops, then the face count follows the desktop count ----
    VDManager::QueryDesktops(m_desktopList);
    int total = m_desktopList.TotalCount();
    DesktopCaptureManager::Prune(m_desktopList, 10ULL * 60ULL * 1000ULL);   // removed desktops / very old snapshots
    ComputeGeometry(total);

    float q = (float)m_cfg.texQuality / 100.0f;
    m_texW = std::clamp((int)(m_screenWidth * q), 320, 1920);
    m_texH = std::max(180, (int)((float)m_texW * m_screenHeight / m_screenWidth));

    // ---- capture BEFORE the overlay is visible so it never appears in its own snapshot ----
    auto currentSnap = DesktopCaptureManager::CaptureMonitorScreen(m_monitorRect);
    if (currentSnap && m_desktopList.currentIndex >= 0 && m_desktopList.currentIndex < total) {
        DesktopCaptureManager::Store(m_desktopList.items[m_desktopList.currentIndex].id, currentSnap);
    }

    // ---- kick off the background composer for the other desktops ----
    if (total > 1 && currentSnap) {
        bool busy = m_composeThread && WaitForSingleObject(m_composeThread, 0) == WAIT_TIMEOUT;
        if (!busy) {
            if (m_composeThread) { CloseHandle(m_composeThread); m_composeThread = nullptr; }
            Composer::s_abort.store(false);
            auto job = std::make_unique<Composer::Job>();
            job->mon = m_monitorRect;
            job->list = m_desktopList;
            m_composeThread = Composer::Start(std::move(job));
        }
    }

    // ---- window / scene ----
    SetWindowPos(m_hwnd, HWND_TOPMOST, m_monitorRect.left, m_monitorRect.top, monW, monH,
                 SWP_NOACTIVATE);
    FindOutput();

    m_rootVisual = m_compositor.CreateContainerVisual();
    m_rootVisual.Size({ m_screenWidth, m_screenHeight });
    BuildVisuals();
    m_target.Root(m_rootVisual);

    m_liveDesktop = m_desktopList.currentIndex;
    StartLive(currentSnap.get());
    CollectAltWindows();
    BuildAltVisuals();

    m_yaw = Spring{};
    m_pitch = Spring{};
    m_zoom = Spring{};
    m_uz = Spring{};
    m_kick = Spring{}; m_pop = Spring{};
    m_bornDesktop = -1; m_bornT = 1.0f; m_fxDirty = false; m_composeRestart = false;
    m_vd = VdTask{};
    m_zoneQuietUntil = std::chrono::steady_clock::time_point{};
    m_vdCooldown = std::chrono::steady_clock::time_point{};
    m_pendingVdOp.store(0);
    m_uz.x = std::clamp(m_inUserZoom.load(), kMinUserZoom, kMaxUserZoom);
    m_zoom.x = m_cfg.zoomAnim ? 0.0f : 1.0f;
    m_phase = Phase::Drag;
    m_zoomRelease = 1.0f;
    m_needSwitch = false;
    m_switchConfirmed = false;
    m_frameCounter = 0;
    m_fpsFrames = 0;
    QueryPerformanceCounter(&m_fpsT0);
    for (auto& a : m_applied) a = 1e9f;

    QueryPerformanceCounter(&m_lastQpc);
    ApplyTransforms();
    m_applied[0] = m_yaw.x; m_applied[1] = m_pitch.x; m_applied[2] = m_zoom.x; m_applied[3] = m_uz.x;

    ShowWindow(m_hwnd, SW_SHOWNA);

    // ---- frame pacer + input hooks (both live on / feed this thread) ----
    m_pacerStop.store(false);
    m_idle.store(false);
    m_idleFrames = 0;
    m_busy = false;
    m_presentPending = false;
    m_pacerThread = CreateThread(nullptr, 0, PacerEntry, this, 0, nullptr);

    GestureController::AttachHooks();
    GestureController::ResyncMouse();

    LOG_INF(L"Scene started: %dx%d faces=%d desktops=%d refresh=%.0fHz live=%d", monW, monH, m_faceCount, total, m_monitorHz, (int)(m_liveBmp != nullptr));
    return true;
}

DWORD WINAPI CubeRenderer::PacerEntry(LPVOID p) {
    ((CubeRenderer*)p)->PacerLoop();
    return 0;
}

void CubeRenderer::PacerLoop() {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
    bool useDxgi = (m_output != nullptr);
    while (!m_pacerStop.load()) {
        if (m_idle.load()) {
            // Idle: no vblank wake-ups. ~30 Hz, and Wake() ends the wait instantly.
            WaitForSingleObject(m_wakeEvent, 33);
            SetEvent(m_frameEvent);
            continue;
        }
        LARGE_INTEGER a, b;
        QueryPerformanceCounter(&a);
        bool waited = false;
        if (useDxgi && m_output) {
            if (SUCCEEDED(m_output->WaitForVBlank())) waited = true;
            else useDxgi = false;
        }
        if (!waited) {
            if (FAILED(DwmFlush())) Sleep(4);
        }
        QueryPerformanceCounter(&b);
        double ms = (double)(b.QuadPart - a.QuadPart) * 1000.0 / (double)m_qpcFreq.QuadPart;
        if (ms < 0.5) Sleep(1);   // vblank source that returns instantly (display asleep etc.)
        SetEvent(m_frameEvent);
    }
}

void CubeRenderer::EndScene() {
    GestureController::DetachHooks();
    StopLive();

    m_pacerStop.store(true);
    if (m_wakeEvent) SetEvent(m_wakeEvent);
    if (m_frameEvent) SetEvent(m_frameEvent);
    if (m_pacerThread) {
        WaitForSingleObject(m_pacerThread, INFINITE);   // see Shutdown(): never leave a thread running in the mod image
        CloseHandle(m_pacerThread);
        m_pacerThread = nullptr;
    }
    if (m_output) { m_output->Release(); m_output = nullptr; }
    if (m_dupOutput) { m_dupOutput->Release(); m_dupOutput = nullptr; }

    if (m_hwnd) ShowWindow(m_hwnd, SW_HIDE);
    RestoreTaskbarCapture();
    if (m_target && m_emptyRoot) m_target.Root(m_emptyRoot);

    m_backgroundVisual = nullptr;
    m_backdropVisual = nullptr;
    m_bgSurface = nullptr;
    m_shadowVisual = nullptr;
    m_shadowSurface = nullptr;
    m_cubeContainer = nullptr;
    m_rootVisual = nullptr;
    for (int i = 0; i < kMaxFaces; ++i) m_faces[i] = FaceVisuals{};
    m_zOrderValid = false;
    m_winVis.clear();
    m_winBrush.clear();
    m_glow = nullptr; m_label = nullptr; m_labelSurface = nullptr; m_labelBrush = nullptr;
    m_zoneL = ZoneVisual{}; m_zoneR = ZoneVisual{};
    m_toast = nullptr; m_toastSurface = nullptr; m_toastT = 0.0f;
    m_vd.active = false;
    m_alt.clear();
    m_altReady = false; m_altAttached = false; m_altShown = false; m_selMode = false; m_selT = 0.0f;

    for (auto& kv : m_bitmapCache) {
        if (kv.second.bmp) kv.second.bmp->Release();
    }
    m_bitmapCache.clear();
}

void CubeRenderer::Teardown() {
    try { EndScene(); } catch (...) {}

    Composer::s_abort.store(true);
    if (m_composeThread) {
        WaitForSingleObject(m_composeThread, INFINITE);   // see Shutdown(): never leave a thread running in the mod image
        CloseHandle(m_composeThread);
        m_composeThread = nullptr;
    }

    m_target = nullptr;
    m_emptyRoot = nullptr;
    m_graphicsDevice = nullptr;

    if (m_textFormatBadge) { m_textFormatBadge->Release(); m_textFormatBadge = nullptr; }
    if (m_dwriteFactory) { m_dwriteFactory->Release(); m_dwriteFactory = nullptr; }
    if (m_d2dDevice) { m_d2dDevice->Release(); m_d2dDevice = nullptr; }
    if (m_d2dFactory) { m_d2dFactory->Release(); m_d2dFactory = nullptr; }
    if (m_d3dDevice) { m_d3dDevice->Release(); m_d3dDevice = nullptr; }

    HWND h = m_hwnd;
    m_hwnd = nullptr;
    if (h && IsWindow(h)) DestroyWindow(h);
    // Right after the window is gone, on the thread that owns it, so this can never run while the window still exists.
    if (g_overlayClassRegistered) {
        if (!UnregisterClassW(kOverlayClassName, GetThisModuleHandle())) {
            LOG_ERR(L"UnregisterClassW failed: %lu", GetLastError());
        }
        g_overlayClassRegistered = false;
    }

    m_compositor = nullptr;
    if (m_dqController) {
        try { m_dqController.ShutdownQueueAsync(); } catch (...) {}
        m_dqController = nullptr;
    }
}

// ---------------------------------------------------------------------------
// Render loop
// ---------------------------------------------------------------------------
void CubeRenderer::RunLoop() {
    HANDLE h[1] = { m_frameEvent };
    for (;;) {
        // Waking for input messages too keeps the low-level mouse hook responsive.
        DWORD r = MsgWaitForMultipleObjectsEx(1, h, 100, QS_ALLINPUT, MWMO_INPUTAVAILABLE);

        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        if (WaitForSingleObject(m_quitEvent, 0) == WAIT_OBJECT_0) break;

        if (r == WAIT_OBJECT_0 || r == WAIT_TIMEOUT) {
            if (!Frame()) break;
            UpdateIdle();
        }
    }
}

// Power saving: after about half a second without any change the pacer drops to ~30 Hz.
// Any input, commit or change brings it straight back to the full refresh rate.
void CubeRenderer::UpdateIdle() {
    if (!m_cfg.idleSave || m_phase != Phase::Drag || m_busy || m_presentPending || m_cmd.load() != 0) {
        m_idleFrames = 0;
        if (m_idle.load()) m_idle.store(false);
    } else if (++m_idleFrames > 40) {
        m_idle.store(true);
    }
    m_busy = false;
}

float CubeRenderer::ComputeYawTarget(float rawYaw) const {
    int total = m_desktopList.TotalCount();
    if (m_faceCount <= 1) return RubberBand(rawYaw, -28.0f, 28.0f, 14.0f);
    if (m_cfg.wrap) return rawYaw;
    int cur = m_desktopList.currentIndex;
    float lo = -(float)(total - 1 - cur) * m_stepDeg;   // dragging left goes to higher desktops
    float hi = (float)cur * m_stepDeg;                  // dragging right goes to lower desktops
    return RubberBand(rawYaw, lo, hi, 35.0f);
}

bool CubeRenderer::Frame() {
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    float dt = (float)((double)(now.QuadPart - m_lastQpc.QuadPart) / (double)m_qpcFreq.QuadPart);

    if (m_cfg.fpsLimit > 0 && dt < (1.0f / (float)m_cfg.fpsLimit) * 0.85f) return true;
    m_lastQpc = now;
    dt = std::clamp(dt, 0.0005f, 0.05f);
    ++m_frameCounter;

    // ---- live desktop feed: newest real screen frame goes straight to the compositor ----
    if (m_liveBmp) {
        const int prevLive = m_liveDesktop;
        if (m_needSwitch && m_switchConfirmed) m_liveDesktop = m_targetDesktop;   // the real screen now shows the target
        if (prevLive != m_liveDesktop) m_applied[0] = 1e9f;
        const bool hold = (m_needSwitch && !m_switchConfirmed) || m_vd.active;    // do not film Windows mid-switch
        const bool bgDue = m_zoom.x > 0.01f && now.QuadPart >= m_bgNext;
        if (!hold && PollLive(bgDue)) {
            m_liveHasFrame = true;
            m_busy = true;
        } else if (m_presentPending && m_sc) {
            if (m_sc->Present(0, DXGI_PRESENT_DO_NOT_WAIT) != DXGI_ERROR_WAS_STILL_DRAWING) m_presentPending = false;
            m_busy = true;
        }
        if (m_texFresh && bgDue) {
            DrawLive();                                                           // small blurred background, low rate on purpose
            m_texFresh = false;
            m_bgNext = now.QuadPart + m_qpcFreq.QuadPart / (LONGLONG)std::max(1, m_cfg.bgHz);
        }
    }
    UpdateAltTab(dt);
    UpdateThumbs();
    {
        const int pendingOp = m_pendingVdOp.exchange(0);
        if (pendingOp != 0 && m_phase == Phase::Drag && !m_vd.active) StartVdOp(pendingOp);
    }
    if (m_phase == Phase::Drag) UpdateVdOp();
    if (m_vd.active) m_busy = true;

    ++m_fpsFrames;
    {
        double sec = (double)(now.QuadPart - m_fpsT0.QuadPart) / (double)m_qpcFreq.QuadPart;
        if (sec >= 2.0) {
            LOG_DBG(L"Render rate %.1f fps (monitor %.0f Hz), live frames %d", (double)m_fpsFrames / sec, m_monitorHz, m_liveFrames);
            m_fpsFrames = 0;
            m_liveFrames = 0;
            m_fpsT0 = now;
        }
    }

    const float zoomOmega = 18.0f * m_cfg.zoomSpeed;

    // User (wheel) zoom follows its target in every phase.
    m_uz.Step(std::clamp(m_inUserZoom.load(), kMinUserZoom, kMaxUserZoom), 16.0f, 1.0f, dt);
    StepFx(dt);

    auto applyIfChanged = [&]() {
        bool changed = std::fabs(m_yaw.x - m_applied[0]) > 2e-3f ||
                       std::fabs(m_pitch.x - m_applied[1]) > 2e-3f ||
                       std::fabs(m_zoom.x - m_applied[2]) > 1e-4f ||
                       std::fabs(m_uz.x - m_applied[3]) > 1e-4f || m_altDirty || m_fxDirty;
        if (changed) {
            m_busy = true;
            ApplyTransforms();
            m_altDirty = false;
            m_fxDirty = false;
            m_applied[0] = m_yaw.x; m_applied[1] = m_pitch.x; m_applied[2] = m_zoom.x; m_applied[3] = m_uz.x;
        }
    };

    // Newly composed desktop images arrive in the background: refresh one face per check.
    if ((m_frameCounter % 6) == 0) {
        if (m_composeRestart) KickComposer();
        for (int i = 0; i < m_faceCount; ++i) {
            auto& f = m_faces[i];
            int d = f.renderedDesktop;
            if (d < 0 || d >= m_desktopList.TotalCount()) continue;
            uint64_t v = DesktopCaptureManager::Version(m_desktopList.items[d].id);
            if (v != f.renderedVersion) {
                RenderFace(i, d);
                m_busy = true;
                m_applied[0] = 1e9f;   // force a transform refresh (lift / blur flags may have changed)
                break;
            }
        }
    }

    if (m_phase == Phase::Drag) {
        // Safety net: a button release that happened before the hook was attached.
        if (GestureController::GetState() == GestureState::ActiveDrag &&
            !(GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
            GestureController::OnActiveMouseUp();
        }
        int cmd = m_vd.active ? 0 : m_cmd.exchange(0);   // finish the desktop operation first
        if (cmd != 0) {
            GestureController::DetachHooks();
            BeginCommit(cmd == 2);
        }
    }

    if (m_phase == Phase::Drag) {
        float tYaw = ComputeYawTarget(m_inYaw.load());
        float tPitch = m_cfg.verticalRotation
            ? RubberBand(m_inPitch.load(), -m_cfg.maxTilt, m_cfg.maxTilt, 18.0f)
            : 0.0f;

        if (m_cfg.dragSmoothMs <= 0) {
            m_yaw.x = tYaw;     m_yaw.v = 0.0f;
            m_pitch.x = tPitch; m_pitch.v = 0.0f;
        } else {
            float om = 5.8f / ((float)m_cfg.dragSmoothMs * 0.001f);
            m_yaw.Step(tYaw, om, 1.0f, dt);
            m_pitch.Step(tPitch, om, 1.0f, dt);
        }

        if (m_cfg.zoomAnim) {
            float tZoom = std::clamp(m_inZoom.load(), 0.0f, 1.0f);
            float omZ = 5.8f / ((float)std::max(m_cfg.dragSmoothMs, 50) * 0.001f);
            m_zoom.Step(tZoom, omZ, 1.0f, dt);
        } else {
            m_zoom.x = 1.0f; m_zoom.v = 0.0f;
        }

        applyIfChanged();
        return true;
    }

    // Settle and Fade
    float omS = 24.0f * m_cfg.snapSpeed;
    float zeta = std::clamp(1.0f - 0.6f * m_cfg.bounce, 0.4f, 1.0f);
    m_yaw.Step(m_snapYaw, omS, zeta, dt);
    m_pitch.Step(0.0f, omS, std::max(zeta, 0.65f), dt);

    float yawErr = std::fabs(m_yaw.x - m_snapYaw);
    float zErr = std::max(yawErr, std::fabs(m_pitch.x)) / 30.0f;
    float zt = m_cfg.zoomAnim ? std::clamp((zErr - 0.2f) / 0.8f, 0.0f, 1.0f) : 1.0f;
    zt = std::min(zt, m_zoomRelease);
    m_zoom.Step(zt, zoomOmega, 1.0f, dt);

    auto tnow = std::chrono::steady_clock::now();

    if (m_phase == Phase::Settle) {
        UpdateSwitching();

        bool covered = m_cfg.zoomAnim ? (m_zoom.x < 0.05f) : true;
        bool aligned = yawErr < 0.6f && std::fabs(m_pitch.x) < 0.6f;
        bool switchDone = !m_needSwitch ||
            (m_switchConfirmed &&
             std::chrono::duration_cast<std::chrono::milliseconds>(tnow - m_confirmTime).count() >= m_cfg.settleMs);
        bool tooLong = std::chrono::duration_cast<std::chrono::milliseconds>(tnow - m_settleStart).count() > 5000;

        if ((covered && aligned && switchDone) || tooLong) {
            m_phase = Phase::Fade;
            m_fadeStart = tnow;
            LOG_DBG(L"Settle -> Fade");
        }
        applyIfChanged();
        return true;
    }

    // Fade
    float elapsed = (float)std::chrono::duration_cast<std::chrono::microseconds>(tnow - m_fadeStart).count() / 1000.0f;
    float t = m_cfg.fadeMs > 0 ? elapsed / (float)m_cfg.fadeMs : 1.0f;
    applyIfChanged();
    if (t >= 1.0f) {
        VDManager::SuppressSystemAnimation(false);
        if (m_rootVisual) m_rootVisual.Opacity(0.0f);
        return false;
    }
    if (m_rootVisual) m_rootVisual.Opacity(1.0f - SmoothStep(t));
    return true;
}

void CubeRenderer::BeginCommit(bool cancel) {
    m_zoomRelease = std::clamp(m_zoom.x, 0.0f, 1.0f);

    int total = m_desktopList.TotalCount();
    int cur = m_desktopList.currentIndex;

    int n = 0;   // number of steps (of m_stepDeg) the prism rotates by, positive = to the right
    if (!cancel && m_faceCount > 1) {
        float yawT = ComputeYawTarget(m_inYaw.load());
        float vel = m_cfg.flick ? m_inFlick.load() : 0.0f;
        float proj = yawT + vel * 0.16f * m_cfg.flickStrength;
        float p = proj / m_stepDeg;
        int base = (int)std::trunc(p);
        float frac = p - (float)base;
        float thr = std::min(m_cfg.switchAngle / m_stepDeg, 0.45f);
        if (std::fabs(frac) > thr) base += (frac > 0.0f ? 1 : -1);
        n = base;
        // A quick, short flick always moves one face, even if the cube was barely turned.
        if (m_cfg.flick && n == 0 && std::fabs(vel) > 240.0f / std::max(m_cfg.flickStrength, 0.25f)) n = vel > 0.0f ? 1 : -1;
        n = std::clamp(n, -m_faceCount, m_faceCount);

        if (total <= 1) {
            n = 0;
        } else if (!m_cfg.wrap) {
            int d = std::clamp(-n, -cur, total - 1 - cur);
            n = -d;
        }
    }

    // Window switcher: releasing activates the selected window and stays on this desktop.
    if (!cancel && m_selMode && m_selIdx >= 0 && m_selIdx < (int)m_alt.size()) {
        n = 0;
        ActivateWindowNow(m_alt[m_selIdx].hwnd);
    }

    m_snapYaw = (float)n * m_stepDeg;

    // Rotating the prism to the right by one face brings the PREVIOUS desktop to the front.
    int target = cur;
    if (n != 0 && total > 0) {
        int d = -n;
        if (m_cfg.wrap) target = (((cur + d) % total) + total) % total;
        else target = std::clamp(cur + d, 0, total - 1);
    }

    m_targetDesktop = target;
    m_needSwitch = (target != cur);
    m_switchConfirmed = false;

    auto tnow = std::chrono::steady_clock::now();
    m_settleStart = tnow;

    if (m_needSwitch) {
        int delta = target - cur;
        m_stepDir = delta > 0 ? 1 : -1;
        m_stepsLeft = std::abs(delta);
        m_stepIntervalMs = m_cfg.suppressSysAnim ? 45 : 120;
        m_switchTimeoutMs = 800 + 350 * m_stepsLeft;
        m_switchStart = tnow;
        m_nextStep = tnow;
        m_lastStepSent = tnow;
        m_nextPoll = tnow;
        if (m_cfg.suppressSysAnim) VDManager::SuppressSystemAnimation(true);
    }

    LOG_INF(L"Commit: cancel=%d steps=%d cur=%d target=%d switch=%d", (int)cancel, n, cur, target, (int)m_needSwitch);
    m_phase = Phase::Settle;
}

// The desktop switch runs UNDER the opaque overlay, so the slide animation is never seen.
void CubeRenderer::UpdateSwitching() {
    if (!m_needSwitch || m_switchConfirmed) return;
    auto now = std::chrono::steady_clock::now();

    if (m_stepsLeft > 0 && now >= m_nextStep) {
        VDManager::SendSwitchStep(m_stepDir);
        --m_stepsLeft;
        m_lastStepSent = now;
        m_nextStep = now + std::chrono::milliseconds(m_stepIntervalMs);
    }

    if (m_stepsLeft == 0 && now >= m_nextPoll) {
        m_nextPoll = now + std::chrono::milliseconds(12);
        int idx = -1;
        if (VDManager::ReadCurrentIndex(idx)) {
            if (idx == m_targetDesktop) {
                m_switchConfirmed = true;
                m_confirmTime = now;
                LOG_DBG(L"Switch confirmed (registry)");
            }
        } else if (std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastStepSent).count() > 160) {
            m_switchConfirmed = true;
            m_confirmTime = now;
            LOG_DBG(L"Switch assumed (no registry value)");
        }
    }

    if (!m_switchConfirmed &&
        std::chrono::duration_cast<std::chrono::milliseconds>(now - m_switchStart).count() > m_switchTimeoutMs) {
        m_switchConfirmed = true;
        m_confirmTime = now;
        LOG_INF(L"Switch timed out, continuing");
    }
}

int CubeRenderer::DesktopForOffset(int k) const {
    int total = m_desktopList.TotalCount();
    if (total <= 0) return -1;
    int idx = m_desktopList.currentIndex + k;
    if (m_cfg.wrap) {
        idx %= total;
        if (idx < 0) idx += total;
        return idx;
    }
    return (idx >= 0 && idx < total) ? idx : -1;
}

// ---------------------------------------------------------------------------
// Per-frame scene update
// ---------------------------------------------------------------------------
void CubeRenderer::ApplyTransforms() {
    if (!m_rootVisual || !m_cubeContainer) return;

    const int F = m_faceCount;
    const float W = m_faceWidth, H = m_faceHeight;
    const float D = m_D;
    const float d = m_cfg.perspective;
    const float zoom = std::clamp(m_zoom.x, 0.0f, 1.2f);
    const float yaw = m_yaw.x + m_kick.x;
    const float pitch = m_pitch.x;
    const float yawR = yaw * kPi / 180.0f;
    const float pitchR = pitch * kPi / 180.0f;
    const float cx = m_screenWidth * 0.5f, cy = m_screenHeight * 0.5f;

    // zoom 0: the front face exactly fills the screen. zoom 1: the open prism at the user's wheel zoom.
    int liveFace = -1;
    bool liveBack = false;

    const float sf = std::max(m_screenWidth / W, 1.0f);
    const float uz = std::clamp(m_uz.x * (1.0f + m_pop.x), 0.25f, 1.9f);
    const float sNow = std::max(sf + (uz - sf) * zoom, 0.2f);
    const float frontZ = d * (1.0f - 1.0f / sNow);

    Mat4 view = Mat4::RotationY(yawR) * Mat4::RotationX(pitchR) *
                Mat4::Translation(0.0f, 0.0f, -D + frontZ) *
                Mat4::Perspective(d) * Mat4::Translation(cx, cy, 0.0f);

    const float cp = std::cos(pitchR), sp = std::sin(pitchR);
    const float bright = (float)m_cfg.brightness / 100.0f;
    auto S01 = [](float t) { return SmoothStep(t); };

    float depthOut = 0.0f;
    auto facingOf = [&](Vec3 c0, Vec3 n0, float yawTotalR, float& outFacing) -> bool {
        float cs = std::cos(yawTotalR), sn = std::sin(yawTotalR);
        auto rot = [&](Vec3 v) -> Vec3 {
            float x1 = v.x * cs + v.z * sn;
            float z1 = -v.x * sn + v.z * cs;
            float y2 = v.y * cp + z1 * sp;
            float z2 = -v.y * sp + z1 * cp;
            return Vec3{ x1, y2, z2 };
        };
        Vec3 c = rot(c0);
        Vec3 n = rot(n0);
        c.z += -D + frontZ;
        depthOut = c.z;
        Vec3 v{ -c.x, -c.y, d - c.z };
        if (v.z < 5.0f) return false;
        float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        if (len < 1e-3f) return false;
        outFacing = (n.x * v.x + n.y * v.y + n.z * v.z) / len;
        return outFacing > 0.02f;
    };

    const float nc = -yaw / m_stepDeg;
    float faceDepth[kMaxFaces] = { 0, 0, 0, 0, 0, 0 };
    auto setOp = [](auto& vis, float& cache, float v) {
        if (std::fabs(v - cache) > 0.0015f || (v == 0.0f) != (cache == 0.0f)) { vis.Opacity(v); cache = v; }
    };
    auto setVis = [](auto& vis, signed char& cache, bool v) {
        const signed char n = v ? 1 : 0;
        if (cache != n) { vis.IsVisible(v); cache = n; }
    };

    for (int i = 0; i < F; ++i) {
        auto& f = m_faces[i];
        if (!f.container) continue;

        // Faces that swing behind the prism are re-mapped so every desktop stays reachable.
        int q = (int)std::lround((nc - (float)i) / (float)F);
        int k = i + F * q;
        int dIdx = DesktopForOffset(k);
        if (dIdx != f.renderedDesktop) RenderFace(i, dIdx);

        // The face that shows the desktop you are on samples the live swap chain directly.
        {
            const bool wantLive = m_liveSurface && m_liveHasFrame && f.renderedDesktop == m_liveDesktop;
            if (wantLive) {
                if (!f.liveBrush) {
                    f.liveBrush = m_compositor.CreateSurfaceBrush(m_liveSurface);
                    f.liveBrush.Stretch(CompositionStretch::Fill);
                }
                if (!f.usingLive) { f.background.Brush(f.liveBrush); f.usingLive = true; }
            } else if (f.usingLive && f.brush) {
                f.background.Brush(f.brush);
                f.usingLive = false;
            }
        }

        float faceDeg = (float)i * m_stepDeg;
        float fac = 0.0f;
        depthOut = 0.0f;
        facingOf(Vec3{ 0.0f, 0.0f, D }, Vec3{ 0.0f, 0.0f, 1.0f }, (faceDeg + yaw) * kPi / 180.0f, fac);
        faceDepth[i] = depthOut;

        bool backSide = fac <= 0.02f;
        if (f.usingLive && liveFace < 0) { liveFace = i; liveBack = backSide; }
        Mat4 baseLocal = Mat4::Translation(-W * 0.5f, -H * 0.5f, 0.0f);
        if (backSide) {
            Mat4 flip = Mat4::Identity();
            flip.m[0][0] = -1.0f;
            baseLocal = baseLocal * flip;
        }
        Mat4 rotFace = Mat4::RotationY(faceDeg * kPi / 180.0f);
        f.container.TransformMatrix((baseLocal * Mat4::Translation(0.0f, 0.0f, D) * rotFace * view).ToWinRT());

        float a;
        if (backSide) {
            a = m_cfg.backOpacity;
        } else {
            a = m_cfg.backOpacity + (m_cfg.sideOpacity - m_cfg.backOpacity) * S01(fac / 0.30f);
            a += (m_cfg.frontOpacity - m_cfg.sideOpacity) * S01((fac - 0.55f) / 0.43f);
        }
        a = std::clamp(a, 0.0f, 1.0f);
        if (dIdx == m_bornDesktop && m_bornT < 1.0f) a *= SmoothStep(m_bornT);   // new desktop fades in
        // Opacity lives on the sprites, not on the container: a group opacity would force an
        // extra full-size offscreen pass for every translucent face.
        setOp(f.background, f.cBg, a);
        setVis(f.container, f.cVisContainer, a > 0.02f);

        float facP = std::max(fac, 0.0f);
        float shadeOp = std::clamp(bright * 0.55f * (1.0f - facP), 0.0f, 0.85f);
        if (f.usingLive) shadeOp = std::max(shadeOp, 0.50f * m_selT);   // dim the desktop under the raised windows
        if (f.rim) {
            const float rimOp = a * 0.28f * (0.4f + 0.6f * facP) * std::min(zoom, 1.0f);
            setOp(f.rim, f.cRim, rimOp);
            setVis(f.rim, f.cVisRim, rimOp > 0.01f);   // hidden rims cost no GPU fill
        }
        setOp(f.shade, f.cShade, shadeOp * a);
        setVis(f.shade, f.cVisShade, shadeOp * a > 0.01f);

        if (f.layer) {
            const bool showBadge = m_cfg.labels && a > 0.01f;
            setVis(f.layer, f.cVisLayer, showBadge);
            if (showBadge) {
                Mat4 badgeLocal = Mat4::Translation(-kBadgeW * 0.5f, -H * 0.5f + 10.0f, 0.0f);
                if (backSide) {
                    Mat4 fl = Mat4::Identity();
                    fl.m[0][0] = -1.0f;
                    badgeLocal = badgeLocal * fl;
                }
                f.layer.TransformMatrix((badgeLocal * Mat4::Translation(0.0f, 0.0f, D) * rotFace * view).ToWinRT());
                setOp(f.layer, f.cLayer, a * (0.75f + 0.25f * facP));
            }
        }
    }

    // Composition does not depth sort, so keep the child order far -> near.
    {
        int order[kMaxFaces];
        for (int i = 0; i < F; ++i) order[i] = i;
        std::sort(order, order + F, [&](int a, int b) { return faceDepth[a] < faceDepth[b]; });
        bool same = m_zOrderValid;
        for (int k2 = 0; k2 < F && same; ++k2) if (order[k2] != m_zOrder[k2]) same = false;
        if (!same) {
            auto children = m_cubeContainer.Children();
            children.RemoveAll();
            m_altAttached = false;
            for (int k2 = 0; k2 < F; ++k2) {
                m_zOrder[k2] = order[k2];
                if (m_faces[order[k2]].container) children.InsertAtTop(m_faces[order[k2]].container);
                if (m_faces[order[k2]].layer) children.InsertAtTop(m_faces[order[k2]].layer);
            }
            m_zOrderValid = true;
        }
    }

    if (m_selT > 0.002f && liveFace >= 0 && !liveBack && m_liveSurface && m_altReady) {
        LayoutAlt(view, (float)liveFace * m_stepDeg, SmoothStep(std::min(zoom, 1.0f) / 0.4f));
    } else {
        HideAlt();
    }

    float z1 = std::min(zoom, 1.0f);
    if (m_backgroundVisual) {
        float s = 1.0f + 0.08f * z1;
        float px = 0.0f, py = 0.0f;
        if (m_cfg.parallax) {
            px = -std::sin(yawR) * 26.0f * z1;
            py = -std::sin(pitchR) * 20.0f * z1;
        }
        m_backgroundVisual.Scale({ s, s, 1.0f });
        m_backgroundVisual.Offset({ px, py, 0.0f });
    }

    if (m_shadowVisual) {
        float scaleF = d / std::max(d - frontZ, 1.0f);
        float sy = cy + (H * 0.5f + 34.0f) * scaleF;
        float tiltFade = std::clamp(1.0f - std::fabs(pitch) / 40.0f, 0.0f, 1.0f);
        m_shadowVisual.Scale({ scaleF, scaleF, 1.0f });
        m_shadowVisual.Offset({ cx - m_shadowW * 0.5f, sy - m_shadowH * 0.5f, 0.0f });
        m_shadowVisual.Opacity(z1 * tiltFade * 0.9f);
    }
}

// ============================================================================
// MODULE 4: GestureController
// ============================================================================
namespace GestureController {

    static std::atomic<GestureState> s_state{ GestureState::Idle };
    static POINT s_startPoint{};
    static POINT s_activationPoint{};
    static POINT s_prevMousePoint{};
    static float s_yaw = 0.0f;
    static float s_pitch = 0.0f;
    static bool s_vUnlocked = false;
    static bool s_open = false;
    static float s_prog = 0.0f;
    static float s_userZoom = 1.0f;
    static int s_inwardSign = -1;
    static DWORD s_openTick = 0;
    static float s_flickVel = 0.0f;
    static DWORD s_lastMoveTick = 0;
    struct YawSample { DWORD t; float yaw; };
    static YawSample s_samples[24];
    static int s_sampleN = 0, s_sampleHead = 0;
    static HWND s_anchorTaskbar = nullptr;

    static HHOOK s_hKeyboardHook = nullptr;
    static HHOOK s_hMouseHook = nullptr;
    // No destructor at process exit (it would release COM objects during loader-lock shutdown). Reset in Shutdown().
    [[clang::no_destroy]] static com_ptr<IUIAutomation> s_pUIAutomation;
    static constexpr UINT_PTR TIMER_ARMING_CHECK = 1002;

    void OnActiveMouseMove(POINT pt, DWORD tick);

    GestureState GetState() { return s_state.load(); }

    static bool InitUIAutomation() {
        if (!s_pUIAutomation) {
            CoCreateInstance(CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER,
                             IID_IUIAutomation, s_pUIAutomation.put_void());
        }
        return s_pUIAutomation != nullptr;
    }

    static bool IsTaskbarClass(const WCHAR* cls) {
        // Only the real taskbar windows. The generic XAML island classes (InputSite, DesktopWindowContentBridge)
        // are shared with Task View, Alt+Tab, Snap and others and must not count as the taskbar.
        return _wcsicmp(cls, L"Shell_TrayWnd") == 0 ||
               _wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0;
    }

    static bool WindowChainIsTaskbar(HWND h) {
        while (h) {
            WCHAR cls[128]{};
            GetClassNameW(h, cls, ARRAYSIZE(cls));
            if (IsTaskbarClass(cls)) return true;
            HWND parent = GetParent(h);
            if (!parent || parent == h) break;
            h = parent;
        }
        return false;
    }

    static bool IsEmptyTaskbarSpace(HWND hWndOrigin, POINT pt) {
        HWND hTarget = WindowFromPoint(pt);
        HWND hCheck = hTarget ? hTarget : hWndOrigin;
        if (!hCheck) return false;

        bool isTaskbar = WindowChainIsTaskbar(hCheck) || (hWndOrigin && WindowChainIsTaskbar(hWndOrigin));
        if (!isTaskbar) {
            LOG_DBG(L"Rejected: 0x%p is not the taskbar", hCheck);
            return false;
        }

        HWND hRoot = GetAncestor(hCheck, GA_ROOT);
        HWND hTaskbar = hRoot ? hRoot : hCheck;

        HWND hTray = FindWindowExW(hTaskbar, nullptr, L"TrayNotifyWnd", nullptr);
        if (hTray && IsWindowVisible(hTray)) {
            RECT rc{};
            GetWindowRect(hTray, &rc);
            if (PtInRect(&rc, pt)) return false;
        }

        HWND hStart = FindWindowExW(hTaskbar, nullptr, L"Start", nullptr);
        if (hStart && IsWindowVisible(hStart)) {
            RECT rc{};
            GetWindowRect(hStart, &rc);
            if (PtInRect(&rc, pt)) return false;
        }

        HWND hMenu = FindWindowW(L"#32768", nullptr);
        if (hMenu && IsWindowVisible(hMenu)) return false;

        if (InitUIAutomation()) {
            com_ptr<IUIAutomationElement> element;
            if (SUCCEEDED(s_pUIAutomation->ElementFromPoint(pt, element.put())) && element) {
                BSTR bstr = nullptr;
                element->get_CurrentClassName(&bstr);
                std::wstring cn = bstr ? bstr : L"";
                if (bstr) SysFreeString(bstr);

                if (cn == L"Taskbar.TaskListButtonAutomationPeer" ||
                    cn == L"Taskbar.SearchBoxButtonAutomationPeer" ||
                    cn == L"Taskbar.TaskViewButtonAutomationPeer" ||
                    cn == L"Taskbar.WidgetsButtonAutomationPeer" ||
                    cn == L"Taskbar.ShowDesktopButtonAutomationPeer" ||
                    cn == L"Start" || cn == L"StartButton" ||
                    cn.find(L"NotifyIcon") != std::wstring::npos ||
                    cn.find(L"Clock") != std::wstring::npos) {
                    LOG_DBG(L"Rejected: UIA class %s", cn.c_str());
                    return false;
                }
            }
        }

        LOG_DBG(L"Taskbar hit accepted at (%ld, %ld)", pt.x, pt.y);
        return true;
    }

    // ---- auto-hide taskbar: hide it again after the gesture so it is ready for the next one ----
    struct FocusSearch { HWND found = nullptr; };

    static BOOL CALLBACK EnumFocusTarget(HWND h, LPARAM lp) {
        FocusSearch* fs = (FocusSearch*)lp;
        if (!IsWindowVisible(h) || IsIconic(h)) return TRUE;
        LONG ex = GetWindowLongW(h, GWL_EXSTYLE);
        if (ex & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) return TRUE;
        DWORD cloaked = 0;
        DwmGetWindowAttribute(h, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
        if (cloaked != 0) return TRUE;
        WCHAR cls[128]{};
        GetClassNameW(h, cls, ARRAYSIZE(cls));
        if (IsTaskbarClass(cls) || _wcsicmp(cls, L"Progman") == 0 || _wcsicmp(cls, L"WorkerW") == 0 ||
            _wcsicmp(cls, L"CompizDesktopCubeOverlayWindow") == 0) return TRUE;
        WCHAR title[8]{};
        if (GetWindowTextW(h, title, ARRAYSIZE(title)) == 0) return TRUE;
        if (!VDManager::IsWindowOnCurrentDesktop(h)) return TRUE;
        fs->found = h;
        return FALSE;
    }

    static void RehideTaskbar() {
        if (!g_settings.autoHideFix) return;
        APPBARDATA abd{};
        abd.cbSize = sizeof(abd);
        UINT_PTR st = SHAppBarMessage(ABM_GETSTATE, &abd);
        if (!(st & ABS_AUTOHIDE)) return;

        Sleep(60);

        HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
        POINT pt{};
        GetCursorPos(&pt);
        if (tray) {
            RECT rc{};
            if (GetWindowRect(tray, &rc) && PtInRect(&rc, pt)) return;   // mouse is on it, leave it
        }

        // An auto-hide taskbar stays up while it owns the focus (we clicked it to start the gesture).
        HWND fg = GetForegroundWindow();
        bool taskbarHasFocus = false;
        if (!fg) {
            taskbarHasFocus = true;
        } else {
            WCHAR cls[128]{};
            GetClassNameW(fg, cls, ARRAYSIZE(cls));
            taskbarHasFocus = IsTaskbarClass(cls) || _wcsicmp(cls, L"Windows.UI.Core.CoreWindow") == 0;
        }
        if (!taskbarHasFocus) return;

        FocusSearch fs;
        EnumWindows(EnumFocusTarget, (LPARAM)&fs);
        HWND target = fs.found ? fs.found : GetShellWindow();
        if (target) {
            SetForegroundWindow(target);
            LOG_DBG(L"Re-hide taskbar: focus moved to 0x%p", target);
        }
    }

    static void CancelArming() {
        if (s_state.load() == GestureState::Arming) {
            if (s_anchorTaskbar && IsWindow(s_anchorTaskbar)) KillTimer(s_anchorTaskbar, TIMER_ARMING_CHECK);
            s_state.store(GestureState::Idle);
            LOG_INF(L"ARMING cancelled -> IDLE");
        }
    }

    static void ActivateCube(POINT pt) {
        if (s_state.load() != GestureState::Arming) return;
        if (g_unloading.load()) { CancelArming(); return; }

        if (s_anchorTaskbar && IsWindow(s_anchorTaskbar)) KillTimer(s_anchorTaskbar, TIMER_ARMING_CHECK);

        s_state.store(GestureState::ActiveDrag);
        s_activationPoint = pt;
        s_prevMousePoint = pt;
        s_yaw = 0.0f;
        s_pitch = 0.0f;
        s_vUnlocked = false;
        s_open = false;
        s_prog = 0.0f;
        s_openTick = GetTickCount();
        s_flickVel = 0.0f;
        s_lastMoveTick = s_openTick;
        s_userZoom = g_settings.rememberZoom ? std::clamp(g_userZoom, kMinUserZoom, kMaxUserZoom) : 1.0f;
        s_sampleN = 0; s_sampleHead = 0;
        {
            RECT rcBar{};
            if (s_anchorTaskbar && IsWindow(s_anchorTaskbar)) GetWindowRect(s_anchorTaskbar, &rcBar);
            s_inwardSign = (rcBar.top > 100) ? -1 : 1;   // bottom taskbar: up is inward (dy < 0)
        }
        LOG_INF(L"ARMING -> ACTIVE_DRAG at (%ld, %ld)", pt.x, pt.y);

        // Asynchronous: the render thread builds the scene and attaches the input hooks itself.
        if (!CubeRenderer::Instance().Start(pt, s_userZoom)) {
            LOG_ERR(L"Failed to start cube overlay");
            s_state.store(GestureState::Idle);
        }
    }

    bool OnMouseDown(HWND hWnd, POINT pt) {
        if (g_unloading.load() || !g_renderer) return false;
        if (s_state.load() != GestureState::Idle || CubeRenderer::Instance().IsActive()) return false;

        if (IsEmptyTaskbarSpace(hWnd, pt)) {
            s_state.store(GestureState::Arming);
            s_startPoint = pt;
            s_prevMousePoint = pt;
            s_anchorTaskbar = hWnd;
            SetTimer(hWnd, TIMER_ARMING_CHECK, 8, nullptr);
            CubeRenderer::Instance().Prewarm();   // devices are ready by the time the drag threshold is crossed
            LOG_INF(L"IDLE -> ARMING at (%ld, %ld)", pt.x, pt.y);
            return true;
        }
        return false;
    }

    static int GetInwardDragDistance(POINT pt) {
        RECT rcBar{};
        if (s_anchorTaskbar && IsWindow(s_anchorTaskbar)) GetWindowRect(s_anchorTaskbar, &rcBar);
        return (rcBar.top > 100) ? (s_startPoint.y - pt.y) : (pt.y - s_startPoint.y);
    }

    void OnArmingMouseMove(POINT pt) {
        if (s_state.load() != GestureState::Arming) return;
        if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000)) { CancelArming(); return; }
        if (GetInwardDragDistance(pt) >= g_settings.activationThreshold) ActivateCube(pt);
    }

    void OnArmingTimerTick() {
        if (s_state.load() != GestureState::Arming) {
            if (s_anchorTaskbar && IsWindow(s_anchorTaskbar)) KillTimer(s_anchorTaskbar, TIMER_ARMING_CHECK);
            return;
        }
        if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000)) { CancelArming(); return; }

        POINT pt{};
        GetCursorPos(&pt);
        if (GetInwardDragDistance(pt) >= g_settings.activationThreshold) ActivateCube(pt);
    }

    void OnArmingMouseUp() {
        if (s_state.load() == GestureState::Arming) CancelArming();
    }

    // Called on the render thread right after the hooks are attached: catch up with where
    // the mouse is now, so the first frame does not jump.
    void ResyncMouse() {
        POINT cur{};
        GetCursorPos(&cur);
        s_prevMousePoint = cur;
        s_lastMoveTick = GetTickCount();
        s_openTick = s_lastMoveTick;

        if (g_settings.zoomAnim) {
            float prog = (float)(s_inwardSign * (cur.y - s_activationPoint.y)) / g_settings.shrinkDist;
            s_prog = std::clamp(prog, 0.0f, 1.0f);
            if (s_prog >= 0.999f) { s_prog = 1.0f; s_open = true; }
        } else {
            s_prog = 1.0f;
            s_open = true;
        }
        CubeRenderer::Instance().SubmitInput(s_yaw, s_pitch, s_prog);
    }

    void OnActiveMouseMove(POINT pt, DWORD tick) {
        if (s_state.load() != GestureState::ActiveDrag) return;

        int dx = pt.x - s_prevMousePoint.x;
        int dy = pt.y - s_prevMousePoint.y;
        s_prevMousePoint = pt;
        if (dx == 0 && dy == 0) return;

        // Stage 1: dragging inward shrinks the desktop gradually (and reverses if you drag back).
        if (!s_open) {
            if (g_settings.zoomAnim) {
                s_prog = std::clamp(s_prog + (float)(s_inwardSign * dy) / g_settings.shrinkDist, 0.0f, 1.0f);
                if (s_prog >= 0.999f) { s_prog = 1.0f; s_open = true; s_openTick = tick; }
            } else {
                s_prog = 1.0f; s_open = true; s_openTick = tick;
            }
        }
        float gate = std::clamp((s_prog - 0.35f) / 0.5f, 0.0f, 1.0f);
        gate = gate * gate * (3.0f - 2.0f * gate);

        float dYaw = (float)dx * g_settings.hSens * (g_settings.invertH ? -1.0f : 1.0f) * gate;
        float dPitch = (float)dy * g_settings.vSens * (g_settings.invertV ? -1.0f : 1.0f);
        s_yaw += dYaw;
        if (s_open && g_settings.verticalRotation) {
            if (!s_vUnlocked) {
                if (!g_settings.horizFirst || std::fabs(s_yaw) > 6.0f ||
                    (tick - s_openTick) >= (DWORD)g_settings.vUnlockMs) {
                    s_vUnlocked = true;
                }
            } else {
                s_pitch += dPitch;
            }
        }

        // Flick speed = yaw travelled over the last ~100 ms (stable even with 1000 Hz mice).
        s_samples[s_sampleHead] = { tick, s_yaw };
        s_sampleHead = (s_sampleHead + 1) % 24;
        if (s_sampleN < 24) ++s_sampleN;
        {
            float v = 0.0f;
            for (int k = 2; k <= s_sampleN; ++k) {
                const YawSample& o = s_samples[(s_sampleHead - k + 48) % 24];
                DWORD age = tick - o.t;
                if (age > 100) break;
                if (age >= 16) v = (s_yaw - o.yaw) / (float)age * 1000.0f;
            }
            s_flickVel = std::clamp(v, -3500.0f, 3500.0f);
        }
        s_lastMoveTick = tick;

        CubeRenderer::Instance().SubmitInput(s_yaw, s_pitch, s_prog);
    }

    static void OnWheelEvent(int delta) {
        if (g_settings.wheelZoom) {
            // Scroll up = closer. Multiplicative, so every notch feels the same at any size; high-res wheels give fractions of a notch.
            float notches = (float)delta / 120.0f;
            s_userZoom = std::clamp(s_userZoom * std::pow(1.0f + g_settings.wheelStep, notches), kMinUserZoom, kMaxUserZoom);
            if (g_settings.rememberZoom) g_userZoom = s_userZoom;
            CubeRenderer::Instance().SubmitUserZoom(s_userZoom);
        } else if (g_settings.altTab) {
            int notches = std::max(1, std::abs(delta) / 120);
            CubeRenderer::Instance().SubmitWheelSelect(delta > 0 ? -notches : notches);
        }
    }

    void OnActiveMouseUp() {
        if (s_state.load() != GestureState::ActiveDrag) return;

        float vel = (GetTickCount() - s_lastMoveTick < 90) ? s_flickVel : 0.0f;
        LOG_INF(L"ACTIVE_DRAG -> COMMITTING yaw=%.2f pitch=%.2f flick=%.1f", s_yaw, s_pitch, vel);
        s_state.store(GestureState::Committing);
        CubeRenderer::Instance().Commit(vel);   // the render thread detaches the hooks on its next frame
    }

    void OnSnapCompleted() {
        s_state.store(GestureState::Idle);
        LOG_INF(L"Gesture finished -> IDLE");
        RehideTaskbar();
    }

    void OnCancel() {
        GestureState st = s_state.load();
        if (st == GestureState::Arming) {
            CancelArming();
        } else if (st == GestureState::ActiveDrag) {
            LOG_INF(L"Escape: cancelling cube gesture");
            s_state.store(GestureState::Committing);
            CubeRenderer::Instance().Cancel();
        }
    }

    static bool s_swallowRUp = false;

    void RescaleYaw(float k) {
        s_yaw *= k;
        s_sampleN = 0; s_sampleHead = 0;
        s_flickVel = 0.0f;
        CubeRenderer::Instance().SubmitInput(s_yaw, s_pitch, s_prog);
    }

    static LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
        if (nCode == HC_ACTION && wParam == WM_RBUTTONUP && s_swallowRUp) {
            s_swallowRUp = false;
            return 1;
        }
        if (nCode == HC_ACTION && s_state.load() == GestureState::ActiveDrag) {
            MSLLHOOKSTRUCT* ms = (MSLLHOOKSTRUCT*)lParam;
            if (ms) {
                if (wParam == WM_MOUSEMOVE) {
                    OnActiveMouseMove(ms->pt, ms->time);
                } else if (wParam == WM_LBUTTONUP) {
                    // Not swallowed: the XAML taskbar must see the release or it keeps mouse capture.
                    OnActiveMouseUp();
                } else if (wParam == WM_RBUTTONDOWN) {
                    if (CubeRenderer::Instance().OnRightDown(ms->pt)) {
                        s_swallowRUp = true;
                        return 1;   // right click on an edge zone: add / remove desktop, apps never see it
                    }
                } else if (wParam == WM_MOUSEWHEEL && (g_settings.wheelZoom || g_settings.altTab)) {
                    OnWheelEvent((int)(short)HIWORD(ms->mouseData));
                    return 1;   // swallow so windows under the cursor do not scroll
                }
            }
        }
        return CallNextHookEx(s_hMouseHook, nCode, wParam, lParam);
    }

    static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
        if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
            KBDLLHOOKSTRUCT* kb = (KBDLLHOOKSTRUCT*)lParam;
            if (kb && kb->vkCode == VK_ESCAPE && s_state.load() == GestureState::ActiveDrag) {
                OnCancel();
                return 1;
            }
        }
        return CallNextHookEx(s_hKeyboardHook, nCode, wParam, lParam);
    }

    // Hooks are attached and detached by the render thread, which pumps messages every frame.
    void AttachHooks() {
        HMODULE hMod = nullptr;
        GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (LPCWSTR)&LowLevelMouseProc, &hMod);
        if (!s_hMouseHook) s_hMouseHook = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, hMod, 0);
        if (!s_hKeyboardHook) s_hKeyboardHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, hMod, 0);
        LOG_DBG(L"Hooks attached mouse=0x%p kbd=0x%p", s_hMouseHook, s_hKeyboardHook);
    }

    void DetachHooks() {
        if (s_hMouseHook) { UnhookWindowsHookEx(s_hMouseHook); s_hMouseHook = nullptr; }
        if (s_hKeyboardHook) { UnhookWindowsHookEx(s_hKeyboardHook); s_hKeyboardHook = nullptr; }
    }

    // Used on mod unload (after the render thread has stopped).
    void Shutdown() {
        if (s_anchorTaskbar && IsWindow(s_anchorTaskbar)) KillTimer(s_anchorTaskbar, TIMER_ARMING_CHECK);
        s_state.store(GestureState::Idle);
        s_pUIAutomation = nullptr;
    }

} // namespace GestureController

// ============================================================================
// Overlay window procedure
// ============================================================================
LRESULT CALLBACK CubeRenderer::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_ERASEBKGND:
            return 1;
        case WM_CAPTURECHANGED:
        case WM_CANCELMODE:
            return 0;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// ============================================================================
// Taskbar hooking
// ============================================================================
using CreateWindowInBand_t = HWND(WINAPI*)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID, DWORD);
using CreateWindowExW_t = HWND(WINAPI*)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);

static CreateWindowInBand_t CreateWindowInBand_orig = nullptr;
static CreateWindowExW_t CreateWindowExW_orig = nullptr;

// All InputSite windows of explorer.exe (taskbar, Task View, Alt+Tab, Snap, ...) share ONE window procedure, so a
// single flag says whether it is hooked. The hook then reacts only to windows whose root is a taskbar.
static std::atomic<bool> g_inputSiteProcHooked{ false };
static std::mutex g_taskbarMutex;                       // guards g_subclassedTaskbars (windows are created on many threads)
static std::unordered_set<HWND> g_subclassedTaskbars;
static WNDPROC InputSiteWindowProc_Original = nullptr;
static std::atomic<bool> g_isWhInitialized{ false };

// Returns the root window when hWnd belongs to a taskbar (Shell_TrayWnd / Shell_SecondaryTrayWnd), otherwise nullptr.
static HWND GetTaskbarRoot(HWND hWnd) {
    WCHAR cls[32];
    HWND root = GetAncestor(hWnd, GA_ROOT);
    if (root && GetClassNameW(root, cls, ARRAYSIZE(cls)) &&
        (_wcsicmp(cls, L"Shell_TrayWnd") == 0 || _wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0)) {
        return root;
    }
    return nullptr;
}

static bool IsTaskbarRoot(HWND hWnd) {
    return GetTaskbarRoot(hWnd) != nullptr;
}

static LRESULT CALLBACK InputSiteWindowProc_Hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_POINTERDOWN:
            if (IS_POINTER_FIRSTBUTTON_WPARAM(wParam) && !g_unloading.load()) {
                if (HWND hRoot = GetTaskbarRoot(hWnd)) {
                    POINT pt{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) }; // screen coords
                    GestureController::OnMouseDown(hRoot, pt);
                }
            }
            break;

        case WM_POINTERUPDATE:
            if (GestureController::GetState() == GestureState::Arming && IsTaskbarRoot(hWnd)) {
                POINT pt{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
                GestureController::OnArmingMouseMove(pt);
            }
            break;

        case WM_POINTERUP:
            if (IS_POINTER_FIRSTBUTTON_WPARAM(wParam) && GestureController::GetState() == GestureState::Arming &&
                IsTaskbarRoot(hWnd)) {
                GestureController::OnArmingMouseUp();
            }
            break;

        case WM_LBUTTONDOWN:
            if (!g_unloading.load()) {
                if (HWND hRoot = GetTaskbarRoot(hWnd)) {
                    POINT pt{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
                    ClientToScreen(hWnd, &pt);
                    GestureController::OnMouseDown(hRoot, pt);
                }
            }
            break;

        case WM_MOUSEMOVE:
            if (GestureController::GetState() == GestureState::Arming && IsTaskbarRoot(hWnd)) {
                POINT pt{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
                ClientToScreen(hWnd, &pt);
                GestureController::OnArmingMouseMove(pt);
            }
            break;

        case WM_LBUTTONUP:
            if (GestureController::GetState() == GestureState::Arming && IsTaskbarRoot(hWnd)) {
                GestureController::OnArmingMouseUp();
            }
            break;
    }
    return InputSiteWindowProc_Original(hWnd, uMsg, wParam, lParam);
}

// While a desktop switch is in progress, explorer's "client area animation" check says no. The user's real setting
// is never touched (see VDManager::SuppressSystemAnimation).
using SystemParametersInfoW_t = decltype(&SystemParametersInfoW);
static SystemParametersInfoW_t SystemParametersInfoW_orig = nullptr;
static BOOL WINAPI SystemParametersInfoW_hook(UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni) {
    if (uiAction == SPI_GETCLIENTAREAANIMATION && pvParam && VDManager::s_animSuppressed.load()) {
        *(BOOL*)pvParam = FALSE;
        return TRUE;
    }
    return SystemParametersInfoW_orig(uiAction, uiParam, pvParam, fWinIni);
}

static LRESULT CALLBACK TaskbarSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR) {
    switch (uMsg) {
        case WM_LBUTTONDOWN: {
            POINT pt{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            ClientToScreen(hWnd, &pt);
            GestureController::OnMouseDown(hWnd, pt);
            break;
        }
        case WM_MOUSEMOVE:
            if (GestureController::GetState() == GestureState::Arming) {
                POINT pt{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
                ClientToScreen(hWnd, &pt);
                GestureController::OnArmingMouseMove(pt);
            }
            break;
        case WM_LBUTTONUP:
            if (GestureController::GetState() == GestureState::Arming) GestureController::OnArmingMouseUp();
            break;
        case WM_TIMER:
            if (wParam == 1002 /* TIMER_ARMING_CHECK */) {
                GestureController::OnArmingTimerTick();
                return 0;
            }
            break;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

static void HookInputSiteWindow(HWND hWnd) {
    // Only a taskbar's InputSite window may trigger the hook (the procedure itself is shared by every XAML island).
    if (!hWnd || g_unloading.load() || !IsTaskbarRoot(hWnd) || g_inputSiteProcHooked.exchange(true)) return;

    void* wndProc = (void*)GetWindowLongPtrW(hWnd, GWLP_WNDPROC);
    if (!wndProc) {
        g_inputSiteProcHooked.store(false);
        return;
    }

    if (Wh_SetFunctionHook(wndProc, (void*)InputSiteWindowProc_Hook, (void**)&InputSiteWindowProc_Original)) {
        if (g_isWhInitialized.load()) Wh_ApplyHookOperations();
        LOG_INF(L"Hooked InputSite wndproc %p", wndProc);
    } else {
        g_inputSiteProcHooked.store(false);
        LOG_ERR(L"Failed to hook InputSite wndproc %p", wndProc);
    }
}

static BOOL CALLBACK EnumChildFindInputSite(HWND hChild, LPARAM) {
    WCHAR cls[128]{};
    if (GetClassNameW(hChild, cls, ARRAYSIZE(cls)) &&
        _wcsicmp(cls, L"Windows.UI.Input.InputSite.WindowClass") == 0) {
        HookInputSiteWindow(hChild);
    }
    return TRUE;
}

static void SubclassTaskbar(HWND hWnd) {
    if (!hWnd || g_unloading.load()) return;
    bool already = false;
    {
        std::lock_guard<std::mutex> lk(g_taskbarMutex);
        already = g_subclassedTaskbars.contains(hWnd);
    }
    if (!already) {
        // Not under the lock: this call can wait for the window's thread.
        if (WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, TaskbarSubclassProc, 0)) {
            bool unloadingNow = false;
            {
                std::lock_guard<std::mutex> lk(g_taskbarMutex);
                // Wh_ModUninit sets g_unloading before it takes this lock to collect the windows, so either it
                // collects this one, or we see the flag here and remove the subclass ourselves.
                unloadingNow = g_unloading.load();
                if (!unloadingNow) g_subclassedTaskbars.insert(hWnd);
            }
            if (unloadingNow) {
                WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, TaskbarSubclassProc);
                return;
            }
            LOG_INF(L"Subclassed taskbar window 0x%p", hWnd);
        }
    }
    EnumChildWindows(hWnd, EnumChildFindInputSite, 0);
}

static void HandleCreatedWindow(HWND hWnd, LPCWSTR lpClassName) {
    if (!hWnd || !lpClassName || IS_INTRESOURCE(lpClassName)) return;
    if (_wcsicmp(lpClassName, L"Windows.UI.Input.InputSite.WindowClass") == 0) {
        HookInputSiteWindow(hWnd);
    } else if (_wcsicmp(lpClassName, L"Shell_TrayWnd") == 0 ||
               _wcsicmp(lpClassName, L"Shell_SecondaryTrayWnd") == 0) {
        SubclassTaskbar(hWnd);
    }
}

static HWND WINAPI CreateWindowInBand_hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle,
    int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam, DWORD dwBand) {
    HWND hWnd = CreateWindowInBand_orig(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
                                        hWndParent, hMenu, hInstance, lpParam, dwBand);
    HandleCreatedWindow(hWnd, lpClassName);
    return hWnd;
}

static HWND WINAPI CreateWindowExW_hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle,
    int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_orig(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
                                     hWndParent, hMenu, hInstance, lpParam);
    HandleCreatedWindow(hWnd, lpClassName);
    return hWnd;
}

static BOOL CALLBACK EnumWindowsInitProc(HWND hWnd, LPARAM) {
    WCHAR cls[128]{};
    if (GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) &&
        (_wcsicmp(cls, L"Shell_TrayWnd") == 0 || _wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0)) {
        SubclassTaskbar(hWnd);
    }
    return TRUE;
}

static bool InitMod() {
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (hUser32) {
        Wh_SetFunctionHook((void*)GetProcAddress(hUser32, "CreateWindowExW"),
                           (void*)CreateWindowExW_hook, (void**)&CreateWindowExW_orig);

        void* pBand = (void*)GetProcAddress(hUser32, "CreateWindowInBand");
        if (pBand) {
            Wh_SetFunctionHook(pBand, (void*)CreateWindowInBand_hook, (void**)&CreateWindowInBand_orig);
        }

        void* pSpi = (void*)GetProcAddress(hUser32, "SystemParametersInfoW");
        if (pSpi && !Wh_SetFunctionHook(pSpi, (void*)SystemParametersInfoW_hook, (void**)&SystemParametersInfoW_orig)) {
            SystemParametersInfoW_orig = nullptr;
            LOG_ERR(L"Could not hook SystemParametersInfoW, the desktop slide animation will not be suppressed");
        }
    }
    EnumWindows(EnumWindowsInitProc, 0);
    return true;
}

// ============================================================================
// Windhawk lifecycle
// ============================================================================
BOOL Wh_ModInit() {
    LOG_INF(L"Compiz Desktop Cube initializing");
    g_unloading.store(false);
    g_renderer.emplace();
    LoadSettings();
    if (!InitMod()) {
        LOG_ERR(L"Failed to initialize mod");
        g_renderer.reset();
        return FALSE;
    }
    g_isWhInitialized = true;
    return TRUE;
}

void Wh_ModUninit() {
    LOG_INF(L"Compiz Desktop Cube uninitializing");

    // 1. Stop new work first: no new render thread (EnsureThread refuses), no new gesture, no new subclass.
    g_unloading.store(true);
    g_isWhInitialized = false;

    // 2. Remove the taskbar subclasses so no more mouse messages reach the mod.
    std::vector<HWND> taskbars;
    {
        std::lock_guard<std::mutex> lk(g_taskbarMutex);
        taskbars.assign(g_subclassedTaskbars.begin(), g_subclassedTaskbars.end());
        g_subclassedTaskbars.clear();
    }
    for (HWND hWnd : taskbars) {
        if (IsWindow(hWnd)) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, TaskbarSubclassProc);
        }
    }

    // 3. Stop the render thread and wait for it (and the pacer and composer threads it owns) without a timeout.
    //    The render thread detaches the input hooks, destroys the overlay window and unregisters its class.
    CubeRenderer::Instance().Shutdown();

    RestoreTaskbarCapture();
    GestureController::Shutdown();
    VDManager::SuppressSystemAnimation(false);
    DesktopCaptureManager::Clear();
    WallpaperLoader::ClearCache();

    // 4. Every thread is gone and Teardown released all WinRT / D3D objects on the render thread: only memory is left.
    g_renderer.reset();
}

void Wh_ModSettingsChanged() {
    LOG_INF(L"Settings changed, reloading");
    LoadSettings();
}
