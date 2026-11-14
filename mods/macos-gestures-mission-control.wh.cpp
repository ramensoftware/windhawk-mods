// ==WindhawkMod==
// @id              macos-gestures-mission-control
// @name            macOS Gestures - Mission Control
// @description     Mission Control for Windows: a three-finger swipe up/down opens a live, animated overview of your windows (app icons, minimized windows, taskbar slide, spring physics) that follows your fingers; left/right swipes keep switching apps like Alt+Tab
// @version         1.3.1
// @author          Frixed
// @github          https://github.com/frixed
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -luser32 -lwindowsapp -ldwmapi -lgdi32 -ladvapi32 -lole32 -lwindowscodecs -luuid -ld3d11
// @license         MIT
// ==/WindhawkMod==

// Development assistance:
// AI-assisted development with ChatGPT and Claude.
// Design, implementation, testing and maintenance by Frixed.

// ==WindhawkModReadme==
/*
# macOS Gestures - Mission Control

![macOS Gestures for Windows](https://i.imgur.com/9ONH63E.gif)

Mission Control for Windows. A **three-finger swipe up** on a precision touchpad opens a
live, animated overview of all your windows; the overview **follows your fingers** the whole
way, and springs into place when you let go. Swipe back down, press Esc, or pick a window to
leave.

## What you get

* Live window previews (the same technology the taskbar and Alt+Tab use, so videos keep
  playing) laid out in a clean grid, with the app icon and title under each window.
* Minimized windows are shown too, and dialogs are drawn on top of the window they belong to.
* The taskbar slides out of the screen with your fingers and comes back when you pick a window.
* A **virtual desktops bar** flush with the bottom edge: see your desktops, switch to one or
  create a new one. It fades in with the swipe.
* A close button on the window under the mouse (also the middle mouse button and the Delete
  key), with the remaining windows gliding to their new places.
* Keyboard: Esc closes, arrow keys / Tab move the selection, Enter or Space picks it.
* Spring physics with the finger speed carried over; pushing further up when everything is
  already open stretches the grid with rubber-band resistance.
* Respects the Windows "Animation effects" setting, and is switched off over fullscreen
  apps and games.

## Three-finger left / right

Windows only lets a program take over *all* three-finger swipes at once, so the mod performs
the Alt+Tab itself: swipe right for Alt+Tab, left for Alt+Shift+Tab, and keep swiping to keep
cycling. It can also be set to do nothing.

## Requirements

* Windows 11 with the touchpad gestures API (`TouchpadGesturesController`) and a precision
  touchpad. On builds without that API the mod stays idle and the log says
  "TouchpadGesturesController not supported".
* The mod runs only in the shell's `explorer.exe` (not in folder windows opened in a separate
  process, nor in Explorer's COM helper processes).

## What changes in Windows

* While the mod runs it takes over **all** three-finger swipes, so Windows' own three-finger
  actions are off: swipe up (Task View) and swipe down (show desktop) have no Windows
  replacement while Mission Control is closed. Turning the mod off gives them back.
* The left/right swipe sends Alt+Tab as keystrokes. When an app running as administrator has
  the focus, Windows blocks those keystrokes, so that swipe does nothing there. The desktops
  bar uses Windows' Win+Ctrl shortcuts the same way and has the same limit.

## Conflicts

* **CoverTabby** with its touchpad option turned on also takes over three-finger swipes. Use
  only one of the two for touchpad gestures.

## Settings

Everything above can be changed from the **Settings** tab: background material (Mica, Mica
Alt, Acrylic, live blur, blurred wallpaper), swipe distance, which parts to show, and the
animation tuning (speed, bounce, stretch, background fade).

## Notes

* The live previews use private DWM functions, which Windows updates can change. If they stop
  working the mod falls back to the classic thumbnails on its own.
* Other mods that animate windows closing (like *Windows Animations*) are kept from playing
  their animation in front of Mission Control: a window is hidden just before it is closed.
  Keep that mod's option to animate hidden windows turned off. If the window is still there a
  moment later (it asked "Save changes?", or it closes to the tray), it is shown again.
* Mission Control shows up to 24 windows.
* If the mod does not start at logon, turn on logging in the mod's **Advanced** tab in
  Windhawk and check the log output: the start-up steps are logged there.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- backdrop: mica
  $name: Background
  $description: Material shown behind the windows while Mission Control is open.
  $options:
  - mica: Mica
  - micaAlt: Mica Alt
  - acrylic: Acrylic (system)
  - liveBlur: Live blur (windows behind stay visible)
  - wallpaper: Blurred wallpaper
- swipeDistance: 150
  $name: Swipe distance
  $description: How far the three fingers travel to go from 0% to 100% (60 to 400). Lower = more sensitive.
- invertSwipe: false
  $name: Invert swipe direction
- horizontalSwipe: altTab
  $name: Three-finger left/right swipe
  $description: Windows cannot be asked to keep only the horizontal swipe, so the mod can perform the Alt+Tab itself.
  $options:
  - altTab: Switch apps (Alt+Tab)
  - ignore: Do nothing
- altTabReverse: false
  $name: Reverse the Alt+Tab swipe direction
  $description: By default a swipe to the right is Alt+Tab and to the left is Alt+Shift+Tab.
- showIcons: true
  $name: Show app icons under the windows
- showTitles: true
  $name: Show window titles
  $description: The title of each window, under its icon.
- showMinimized: true
  $name: Show minimized windows
- showAlwaysOnTop: true
  $name: Show always-on-top windows
- slideTaskbar: true
  $name: Slide the taskbar away
  $description: The taskbar leaves the screen following your fingers and comes back when you pick an app.
- desktopBar: true
  $name: Virtual desktops bar
  $description: A bar flush with the bottom edge to see your desktops, switch to one or create a new one. It fades in following your fingers.
- closeButton: true
  $name: Close button on hover
  $description: Shows a close button on the window under the mouse.
- middleClickClose: true
  $name: Close windows with the middle mouse button
- hideBeforeClose: true
  $name: Hide a window right before closing it from Mission Control
  $description: Keeps the close animation of other mods (like Windows Animations) from playing on screen in front of Mission Control, because an already hidden window has nothing to animate. If such a mod animates windows that get hidden, turn that option off in it. A window that is still there a moment later is shown again, so an app that asks for confirmation (Save changes?) brings its prompt back, and an app that closes to the tray shows up once more and can be closed to the tray from there.
- keyboardSupport: true
  $name: Keyboard support
  $description: Esc closes, arrow keys or Tab move the selection, Enter or Space picks it, Delete closes the selected window.
- disableInFullscreen: true
  $name: Disable Mission Control over fullscreen apps and games
  $description: The three-finger swipe up/down is ignored while a fullscreen app has the screen. Left/right keeps working.
- respectReducedMotion: true
  $name: Respect Windows "Animation effects"
  $description: When animations are turned off in Windows accessibility settings, springs and fades are skipped.
- animationSpeed: 100
  $name: Animation speed (%)
  $description: 50 to 200. Higher = snappier.
- springDamping: 60
  $name: Spring damping (%)
  $description: 30 to 100. 100 = no bounce, 60 = a nice bounce, 40 = very bouncy.
- elasticStretch: 12
  $name: Elastic stretch past 100% (%)
  $description: 0 to 25. How far the grid stretches if you keep pushing up when everything is already open.
- backdropFadeStart: 6
  $name: Background fade starts at (%)
  $description: Progress at which the background starts to appear (0 to 50).
- backdropFadeEnd: 18
  $name: Background fade ends at (%)
  $description: Progress at which the background is fully there (1 to 60). Shorter = less see-through ghost of the real windows.
- prewarm: true
  $name: Prepare icons in the background at startup
  $description: Makes the first gesture after loading the mod smoother.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <wincodec.h>
#include <d3d11.h>
#include <dxgi.h>
#include <dcomp.h>
#include <shobjidl.h>
#include <windhawk_utils.h>

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Core.h>  // PointerEventArgs
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <cwchar>
#include <map>
#include <memory>
#include <string>
#include <vector>

using namespace winrt;
using namespace Windows::UI::Input;
using namespace Windows::UI::Core;

// ---------------------------------------------------------------------------
// Tunables
// ---------------------------------------------------------------------------

// Centroid travel (in PhysicalPosition units) needed to go from 0% to 100%.
static float GESTURE_DISTANCE = 150.0f;

// If the touchpad Y axis turns out to be inverted, flip this.
static bool INVERT_Y = false;

// On release: progress >= this commits (stays open), otherwise closes.
constexpr float COMMIT_THRESHOLD = 0.5f;

// Overlay only appears after this much progress (avoids flashes on taps).
constexpr float SHOW_THRESHOLD = 0.01f;

// Backdrop:
//   2 = your wallpaper, blurred and dimmed, painted by the mod (opaque, so the
//       real windows underneath never show through)
//   1 = live acrylic blur of whatever is behind (windows included)
//   0 = DWM system backdrop (BACKDROP_TYPE below). Follows light/dark mode.
// Modes 2 and 1 fall back to mode 0 if they can't be applied.
static int BACKDROP_MODE = 0;

// Mode 2: the wallpaper is shrunk by this factor, blurred, then scaled back up.
constexpr int WALLPAPER_SHRINK = 8;
// Mode 2: blur radius in shrunk pixels (blur strength ~ radius * SHRINK).
constexpr int WALLPAPER_BLUR_RADIUS = 3;
// Mode 2: brightness of the wallpaper (1.0 = unchanged).
constexpr float WALLPAPER_DIM = 0.55f;

// Biggest scale a thumbnail may be drawn at (1.0 = never bigger than the
// real window). Raise it if you want small windows to fill their cell.
constexpr float MAX_TILE_SCALE = 1.0f;

// Tint opacity for mode 1: lower = more see-through. 0 = clear blur, 255 = solid.
constexpr int BACKDROP_TINT_ALPHA = 0x66;

// DWM system backdrop for mode 0 (also the fallback if mode 1 fails):
// 2 = Mica, 3 = Acrylic, 4 = Mica Alt.
static int BACKDROP_TYPE = 2;  // Mica

// Hover outline around the window under the mouse (physical pixels, scaled
// with the monitor DPI in UpdateUiScale()).
static int HL_GAP = 4;      // space between thumbnail and outline
static int HL_THICK = 3;    // outline thickness
static int HL_RADIUS = 12;  // outline corner radius
static int HL_PAD = 10;     // room around the outline

// Focus handling:
// * Mica only renders while its window is ACTIVE (inactive windows get a flat
//   fallback color), so the overlay takes focus while open and gives it back
//   to the previous window when it closes without picking anything.
// * An inactive window also draws its own Mica flat, and its live thumbnail
//   shows exactly that. The picked window is therefore activated as soon as it
//   is picked, so its Mica comes in while the thumbnail is still flying back,
//   and the overlay is forced to keep its active look meanwhile.
// The overlay fades in with layered alpha (see OVERLAY_MAX_ALPHA).

// Max alpha of the overlay. Keep this at 255: any value below it makes DWM
// blend the window as a semi-transparent layered window, which breaks the
// Mica / Mica Alt system backdrop. (Lowering it to 254 might stop Chromium
// from treating the overlay as an opaque cover and pausing video thumbnails,
// but it costs you Mica, so it is off by default.)
constexpr BYTE OVERLAY_MAX_ALPHA = 255;

static int OVERLAY_MARGIN = 80;   // space around the grid
static int TILE_PADDING = 32;     // space between tiles
constexpr size_t MAX_TILES = 24;
constexpr UINT_PTR ANIM_TIMER_ID = 1;
constexpr UINT_PTR RENDER_TIMER_ID = 2;
constexpr UINT_PTR HL_TIMER_ID = 3;

// The taskbar slides out of the screen (down for a bottom taskbar) following
// your fingers while Mission Control opens, and slides back in as you pick an
// app. Explorer puts the real taskbar window back whenever it is moved, so a
// live copy of it (a DirectComposition thumbnail, like the app tiles) is what
// slides, while the real one is hidden (cloaked) meanwhile and shown again when
// Mission Control closes. TASKBAR_HIDE_END is the progress at which it is gone.
static bool SLIDE_TASKBAR = true;
constexpr float TASKBAR_HIDE_END = 0.55f;
constexpr float TASKBAR_EXTRA_PX = 12.0f;  // slide a bit past its own size (floating taskbar margin, at 100 % DPI)

// Start-up: the touchpad stack / shell may not be ready when explorer loads the
// mod at logon, so initialization is retried. This also re-checks the
// controller settings now and then in case something else changed them.
constexpr UINT INIT_RETRY_MS = 2000;
// After 30 failed attempts every 2 s the retries slow down to every 10 s, and
// after this many more the mod stops trying (no supported touchpad).
constexpr int SLOW_RETRY_ATTEMPTS = 30;
constexpr UINT ANIM_INTERVAL_MS = 8;  // fallback timer only (see the vsync clock below)

// Release animation = a real spring, started with the speed your fingers had
// when you let go (so there is no jump between dragging and the animation).
// Opening: underdamped, overshoots the final grid a little and settles.
//   SPRING_OMEGA   speed (rad/s). Higher = snappier.   ~24 is about 0.3 s.
//   SPRING_DAMPING 1.0 = no bounce, 0.6 = nice bounce, 0.4 = very bouncy.
// Closing: critically damped (no bounce), CLOSE_OMEGA is its speed.
static float SPRING_OMEGA = 24.0f;
static float SPRING_DAMPING = 0.6f;
static float CLOSE_OMEGA = 28.0f;
// How far past "fully open" the tiles may travel while bouncing.
constexpr float SPRING_MAX_OVERSHOOT = 0.30f;  // upper bound; the setting stays below it
// Fastest release speed carried into the spring (progress units / second).
constexpr float MAX_RELEASE_VELOCITY = 5.0f;
// On release, "progress + velocity * this" decides open vs close, so a quick
// flick commits even if it didn't travel far. 0 = only the position counts.
constexpr float FLING_PROJECTION_S = 0.12f;

// The overlay fades in over this much progress (0.25 = old behavior). The
// real windows are still visible underneath while it fades, which is what
// shows up as a "ghost", so a shorter fade = less ghosting. Too short and the
// backdrop pops in.
static float FADE_START_PROGRESS = 0.06f;  // the backdrop starts to appear here...
static float FADE_END_PROGRESS = 0.18f;    // ...and is fully there here

constexpr UINT WM_APP_FRAME = WM_APP + 1;     // posted by the vsync clock thread
constexpr UINT WM_APP_SETTINGS = WM_APP + 2;  // thread message: reload the settings

// App icon centered under each tile. Drawn as DirectComposition visuals, so
// moving and fading them costs nothing on the CPU side. Its fade is driven by
// the same progress as the finger swipe: invisible until ICON_FADE_START, fully
// visible at 100%, and it fades back out as you close.
static bool SHOW_ICONS = true;
static float g_uiScale = 1.0f;   // see UpdateUiScale()
static float g_dpiScale = 1.0f;  // monitor DPI / 96
static int ICON_SIZE = 56;          // px
static int ICON_GAP = 14;           // px between the tile and its icon
constexpr float ICON_FADE_START = 0.55f;

// Minimized windows are shown too. Since there is nothing on screen at their
// position, their tile fades in over the first part of the swipe.
constexpr float MINIMIZED_FADE_END = 0.5f;

// "No apps opened" fades in with the swipe (text only, the backdrop is not
// touched): invisible until this progress, fully visible at 100%.
constexpr float EMPTY_FADE_START = 0.25f;

// Pushing further up once everything is already open stretches the grid with
// rubber-band resistance, and letting go springs it back. Max stretch, in
// progress units, and how soon the resistance kicks in (higher = stiffer).
static float DRAG_OVERSHOOT_MAX = 0.12f;
constexpr float DRAG_RUBBER_STIFFNESS = 0.6f;

// Three-finger swipes are sorted by direction once the fingers have travelled
// AXIS_LOCK_DISTANCE (touchpad units): up/down drives Mission Control, left/right
// switches apps like Windows does. Windows itself can't be asked to keep only
// the horizontal swipe (a gesture type goes either to the system or to us as a
// whole), so the mod performs the Alt+Tab itself, in the direction you swipe,
// and keeps Alt held until you lift your fingers, with one extra step for every
// ALT_TAB_STEP of further travel. Set ENABLE_ALT_TAB_SWIPE to false to ignore
// horizontal swipes entirely.
static bool ENABLE_ALT_TAB_SWIPE = true;
static bool ALT_TAB_RIGHT_IS_FORWARD = true;  // right = Alt+Tab, left = Alt+Shift+Tab
constexpr float AXIS_LOCK_DISTANCE = 8.0f;
constexpr float ALT_TAB_STEP = 45.0f;

// Hover outline fade (ms).
constexpr double HL_FADE_IN_MS = 110.0;
constexpr double HL_FADE_OUT_MS = 90.0;
// After picking a minimized window the overlay waits for it to be restored,
// and for Windows' own restore animation to finish, before hiding.
constexpr double RESTORE_SETTLE_MS = 240.0;
constexpr double RESTORE_TIMEOUT_MS = 700.0;
constexpr double MIN_RENDER_INTERVAL_MS = 7.0;  // cap on thumbnail updates while dragging
constexpr COLORREF BACKDROP_COLOR = RGB(18, 18, 22);
constexpr DWORD kDwmwaCloaked = 14;  // DWMWA_CLOAKED (13 is DWMWA_CLOAK, the one that SETS it)

// More options from the Windhawk settings window (see LoadSettings()).
static bool SHOW_MINIMIZED = true;
static bool SHOW_ALWAYS_ON_TOP = true;
static bool CLOSE_BUTTON = true;
static bool SHOW_TITLES = true;
static bool DESKTOP_BAR = true;
static bool HIDE_BEFORE_CLOSE = true;
static bool MIDDLE_CLICK_CLOSE = true;
static bool KEYBOARD_SUPPORT = true;
static bool DISABLE_IN_FULLSCREEN = true;
static bool RESPECT_REDUCED_MOTION = true;
static bool PREWARM = true;

// Close button drawn on the window under the mouse.
static int CB_RADIUS = 13;
static int CB_INSET = 10;
constexpr double RELAYOUT_MS = 240.0;       // grid glides to its new layout after a close
constexpr UINT_PTR CLOSE_POLL_TIMER_ID = 5; // waits for a closing window to disappear
constexpr UINT_PTR BAR_TIMER_ID = 6;        // hover animation of the desktops bar

// Window titles under the icons.
static int TITLE_FONT_PX = 14;
static int TITLE_H = 20;    // vertical room reserved for the title
static int TITLE_GAP = 4;   // between the icon and the title
constexpr int TITLE_PAD = 8;   // padding RenderTextPixels leaves around the text

// Virtual desktops bar, flush with the bottom edge.
static int BAR_H = 92;
static int BAR_CARD_W = 116;
static int BAR_CARD_H = 76;
static int BAR_ADD_W = 76;
static int BAR_GAP = 12;
constexpr float BAR_FADE_START = 0.45f;  // progress at which the bar starts to appear
static int BAR_X_R = 9;               // close button of a desktop card
static int BAR_X_INSET = 6;
static int BAR_WELL_W = 90;           // the little screen inside a card
static int BAR_WELL_H = 50;
static int BAR_WELL_Y = 6;
constexpr size_t BAR_MAX_THUMBS = 8;     // windows drawn per desktop
constexpr int BAR_THUMBS_PER_FRAME = 3;  // live thumbnails created per frame (no hitch)
constexpr UINT_PTR DESKTOP_CLOSE_TIMER_ID = 7;  // Win+Ctrl+F4, after the switch to that desktop

// Landing of the window you pick: the closing spring aims a little past the
// end so it arrives with some speed instead of crawling the last pixels (which
// is what made it look like it stopped just short and then snapped in), and the
// whole layer of tiles cross-fades into the real windows during the last
// stretch (all of it, not just the picked one: the others sit above the real
// windows too, so fading only one would reveal them instead of the real window).
constexpr float CLOSE_AIM_PAST_ZERO = 0.02f;
constexpr float TILE_XFADE_END = 0.08f;

// ---------------------------------------------------------------------------
// Globals: WinRT controller / thread
// ---------------------------------------------------------------------------

// Released on its own STA thread in CleanupController(). Windhawk doesn't call
// Wh_ModUninit when explorer itself exits, and a destructor running then, after
// that apartment is gone, could hang or crash the exit: so no destructor.
[[clang::no_destroy]] static TouchpadGesturesController g_controller{ nullptr };

static HANDLE g_thread = nullptr;
static HANDLE g_readyEvent = nullptr;  // set once the thread has a message queue
static DWORD g_threadId = 0;

static event_token g_pressedToken{};
static event_token g_movedToken{};
static event_token g_releasedToken{};

static bool g_pressedRegistered = false;
static bool g_movedRegistered = false;
static bool g_releasedRegistered = false;

static bool g_previousEnabled = false;
static TouchpadGlobalGestureKinds g_previousGestures =
    TouchpadGlobalGestureKinds::None;

static bool g_previousStateSaved = false;

struct TrackedPointer {
    uint32_t id = 0;
    float x = 0.0f;
    float y = 0.0f;
    bool active = false;
};

static std::array<TrackedPointer, 3> g_pointers{};
static bool g_hasPreviousCentroid = false;
static float g_previousCentroidX = 0.0f;
static float g_previousCentroidY = 0.0f;

// ---------------------------------------------------------------------------
// Globals: overlay
// ---------------------------------------------------------------------------

struct Tile {
    HWND hwnd = nullptr;
    HTHUMBNAIL thumb = nullptr;
    RECT from{};  // where the real window is (overlay client coords)
    RECT to{};    // where it ends up in the grid (overlay client coords)
    RECT last{};  // last rect sent to DWM, to skip redundant updates
    bool placed = false;

    // DirectComposition path (null when the tile uses the legacy thumbnail).
    IDCompositionVisual2* dcVisual = nullptr;
    HTHUMBNAIL dcThumb = nullptr;
    float srcW = 1.0f;             // natural size of the thumbnail visual
    float srcH = 1.0f;

    bool minimized = false;        // tile of a minimized window
    bool fadeIn = false;           // its opacity follows the swipe progress
    IDCompositionEffectGroup* dcFx = nullptr;  // tile opacity
    float fxOp = 1.0f;

    IDCompositionVisual2* iconVisual = nullptr;
    IDCompositionEffectGroup* iconFx = nullptr;
    bool iconPlaced = false;
    float iconOp = 0.0f;

    bool topmost = false;          // always-on-top window
    RECT toPrev{};                 // where it was drawn before the last relayout
    std::vector<Tile> kids;        // owned dialogs, drawn on top of this tile
};

static std::vector<Tile> g_tiles;
static HWND g_overlay = nullptr;
static HBRUSH g_backdropBrush = nullptr;
static bool g_classRegistered = false;
static bool g_micaOk = false;  // true when a blurred backdrop was applied
static HWND g_prevForeground = nullptr;
static HWND g_highlight = nullptr;
static HWND g_thumbHost = nullptr;  // click-through window that hosts the DComp thumbnails
static bool g_hlClassRegistered = false;
static int g_hover = -1;
static bool g_trackingMouse = false;

// Blurred wallpaper background (mode 2)
static HDC g_bgDC = nullptr;
static HBITMAP g_bgBmp = nullptr;
static HGDIOBJ g_bgOld = nullptr;
static int g_bgW = 0, g_bgH = 0;
static std::wstring g_bgKey;
static bool g_wallpaperOk = false;
static BYTE g_accR = 0, g_accG = 120, g_accB = 212;
static RECT g_workRect{};  // despite the name: the full monitor rect

static bool g_visible = false;   // overlay window shown
static bool g_open = false;      // committed open (accepts clicks)
static bool g_dragging = false;  // three fingers currently driving it
static float g_progress = 0.0f;  // 0 = windows in place, 1 = grid
static float g_target = 0.0f;
static float g_dragBase = 0.0f;
static float g_dragAccumY = 0.0f;

// Render pacing / animation
static BYTE g_lastAlpha = 0;
static double g_lastRenderMs = 0.0;
static double g_animStartMs = 0.0;
static float g_animVel = 0.0f;       // spring velocity, progress units / s
static double g_animLastMs = 0.0;

// Finger velocity while dragging (unclamped, so a hard swipe past 100% still
// carries speed into the spring).
static float g_dragRaw = 0.0f;
static float g_dragVelRaw = 0.0f;    // raw progress at the last velocity sample
static double g_dragVelMs = 0.0;     // time of the last velocity sample
static float g_dragVel = 0.0f;

// Vsync clock: a helper thread sleeps in DwmFlush() and posts one message per
// display frame while an animation runs, so frames land at the monitor's
// refresh rate instead of on Windows' coarse ~15.6 ms timer tick.
static HANDLE g_clockThread = nullptr;
static HANDLE g_clockWake = nullptr;
static volatile LONG g_clockQuit = 0;
static volatile LONG g_animActive = 0;
static volatile LONG g_framePending = 0;
static bool g_renderDirty = false;
static bool g_renderTimerSet = false;
static HWND g_closeTarget = nullptr;  // window to activate when the close animation ends
static bool g_restoreIssued = false;  // restore requested for a minimized pick
static double g_restoreMs = 0.0;
static int g_iconBand = 0;            // vertical space reserved under each tile for its icon
static bool g_darkTheme = true;

static bool g_eventsSeen = false;     // touchpad events have arrived since the controller was (re)created

static bool g_reduceMotion = false;   // Windows "Animation effects" are off
static double g_lastHideMs = -1.0e9;  // when the overlay was last hidden
static int g_kbSel = -1;              // tile selected with the keyboard
static POINT g_lastMouse{ -1, -1 };
static bool g_hoverClose = false;     // mouse is over the close button
static bool g_relayoutActive = false; // grid gliding to a new layout
static float g_relayoutT = 1.0f;
static double g_relayoutStartMs = 0.0;

// Virtual desktops bar.
static int g_barBand = 0;              // vertical space the bar takes from the grid
static bool g_barBuilt = false;
static int g_barTop = 0;               // top of the bar, overlay client coordinates
static float g_barOp = -1.0f;
static int g_barHover = -1;
static int g_barCloseHover = -1;       // card whose close button the mouse is on
static std::vector<std::pair<int, int>> g_barThumbQueue;  // (card, window) thumbnails still to create
static int g_pendingDesktop = 0;       // 1 = create a desktop, 2 = switch to another
static int g_pendingDesktopDelta = 0;

static IVirtualDesktopManager* g_vdm = nullptr;  // created on first use

static IVirtualDesktopManager* GetVdm() {
    if (!g_vdm) {
        const HRESULT hr = CoCreateInstance(CLSID_VirtualDesktopManager, nullptr, CLSCTX_ALL,
                                            IID_IVirtualDesktopManager,
                                            reinterpret_cast<void**>(&g_vdm));
        static bool logged = false;
        if (FAILED(hr) && !logged) {
            logged = true;
            Wh_Log(L"Virtual desktop manager unavailable (0x%08X): windows can't be told apart per desktop.",
                   static_cast<unsigned>(hr));
        }
    }
    return g_vdm;
}

// A window shown (as a live thumbnail or a wireframe) inside a desktop card.
struct CardWin {
    HWND hwnd;
    RECT rect;  // relative to the monitor, not clipped
};

struct DesktopCard {
    GUID id{};
    std::wstring name;
    bool isAdd = false;       // the "+" card
    bool current = false;
    std::vector<CardWin> wins;  // windows of this desktop (top-most first)
    int x = 0;                // position inside the bar
    int w = 0;
    IDCompositionVisual2* visual = nullptr;
    float hover = 0.0f;
    float hoverTarget = 0.0f;

    IDCompositionVisual2* thumbHost = nullptr;  // holds the live thumbnails
    IDCompositionVisual2* thumbTop = nullptr;   // top-most thumbnail added so far
    std::vector<IDCompositionVisual2*> thumbVisuals;
    std::vector<HTHUMBNAIL> thumbs;
    IDCompositionVisual2* xVisual = nullptr;  // the close button
    IDCompositionEffectGroup* xFx = nullptr;  // its opacity
};
static std::vector<DesktopCard> g_cards;

// "No apps opened" as a DirectComposition visual.
static IDCompositionVisual2* g_emptyVisual = nullptr;
static IDCompositionEffectGroup* g_emptyFx = nullptr;
static IDCompositionSurface* g_emptySurface = nullptr;
static int g_emptyW = 0, g_emptyH = 0;
static bool g_emptyPlaced = false;
static float g_emptyOp = 0.0f;

// Hover outline fade.
static BYTE g_hlAlpha = 0;
static BYTE g_hlFrom = 0, g_hlTo = 0;
static double g_hlStartMs = 0.0, g_hlDurMs = 1.0;
static bool g_hlHideAtEnd = false;

static const wchar_t* kOverlayClass = L"MacGesturesMissionControlOverlay";
static const wchar_t* kHighlightClass = L"MacGesturesMissionControlHighlight";
static const wchar_t* kThumbHostClass = L"MacGesturesMissionControlThumbs";

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static float Clamp01(float v) {
    return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v);
}

static double NowMs() {
    static const LONGLONG freq = [] {
        LARGE_INTEGER f;
        QueryPerformanceFrequency(&f);
        return f.QuadPart;
    }();
    LARGE_INTEGER c;
    QueryPerformanceCounter(&c);
    return c.QuadPart * 1000.0 / static_cast<double>(freq);
}

static LONG LerpL(LONG a, LONG b, float t) {
    return static_cast<LONG>(std::floor(a + (b - a) * t + 0.5f));
}

static RECT LerpRect(const RECT& a, const RECT& b, float t) {
    RECT r;
    r.left = LerpL(a.left, b.left, t);
    r.top = LerpL(a.top, b.top, t);
    r.right = LerpL(a.right, b.right, t);
    r.bottom = LerpL(a.bottom, b.bottom, t);
    return r;
}

// The mod's own module (window classes are registered with it, not explorer's).
extern "C" IMAGE_DOS_HEADER __ImageBase;
static HINSTANCE ModuleInstance() {
    return reinterpret_cast<HINSTANCE>(&__ImageBase);
}

// Window titles are read with InternalGetWindowText, which doesn't send
// WM_GETTEXT: File Explorer windows live on other threads of this same process,
// and a hung one (a stalled network drive) would otherwise freeze this thread.
static bool HasWindowTitle(HWND hwnd) {
    wchar_t c[2] = {};
    return InternalGetWindowText(hwnd, c, 2) > 0;
}

// 1 = this process owns the shell (desktop) window, -1 = another process does,
// 0 = there is no shell window yet (explorer is still starting).
static int ShellWindowOwner() {
    HWND shell = GetShellWindow();
    if (!shell) {
        return 0;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(shell, &pid);
    return pid == GetCurrentProcessId() ? 1 : -1;
}

// Explorer's extra processes: folder windows opened in a separate process and
// COM servers ("/factory,{...} -Embedding", "/separate").
static bool IsSecondaryExplorerCommandLine() {
    std::wstring cmd = GetCommandLineW();
    for (auto& ch : cmd) {
        ch = static_cast<wchar_t>(towlower(ch));
    }
    return cmd.find(L"/factory") != std::wstring::npos ||
           cmd.find(L"-embedding") != std::wstring::npos ||
           cmd.find(L"/separate") != std::wstring::npos;
}

// Effective DPI of a monitor (96 if it can't be read).
static UINT MonitorDpi(HMONITOR mon) {
    typedef HRESULT(WINAPI * PFN_GetDpiForMonitor)(HMONITOR, int, UINT*, UINT*);
    static PFN_GetDpiForMonitor pGetDpi = [] {
        HMODULE shcore = LoadLibraryExW(L"shcore.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        return shcore ? reinterpret_cast<PFN_GetDpiForMonitor>(reinterpret_cast<void*>(
                            GetProcAddress(shcore, "GetDpiForMonitor")))
                      : nullptr;
    }();
    UINT dx = 96, dy = 96;
    if (!pGetDpi || !mon || FAILED(pGetDpi(mon, 0 /*MDT_EFFECTIVE_DPI*/, &dx, &dy)) || dx < 96) {
        return 96;
    }
    return dx;
}

// ---------------------------------------------------------------------------
// Window collection + layout
// ---------------------------------------------------------------------------

struct EnumCtx {
    RECT work;
    std::vector<HWND> hwnds;
};

// Click-through / fully transparent overlays (GPU overlays, widgets...).
static bool IsInvisibleLayered(HWND hwnd, LONG_PTR ex) {
    if (!(ex & WS_EX_LAYERED)) {
        return false;
    }

    COLORREF key = 0;
    BYTE alpha = 255;
    DWORD flags = 0;
    if (GetLayeredWindowAttributes(hwnd, &key, &alpha, &flags)) {
        return (flags & LWA_ALPHA) && alpha < 10;
    }

    // Per-pixel-alpha layered window: can't read its alpha, but if it is also
    // click-through it is an overlay, not a real app window.
    return (ex & WS_EX_TRANSPARENT) != 0;
}

static bool IsDeniedClass(const wchar_t* cls) {
    static const wchar_t* kDenied[] = {
        L"Progman", L"WorkerW", L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd",
        L"Windows.UI.Core.CoreWindow", L"XamlExplorerHostIslandWindow",
        L"TopLevelWindowForOverflowXamlIsland", L"NotifyIconOverflowWindow",
        L"TaskListThumbnailWnd", L"MultitaskingViewFrame", L"ForegroundStaging",
        L"ApplicationManager_DesktopShellWindow", L"Shell_LightDismissOverlay",
        L"CEF-OSC-WIDGET",  // NVIDIA GeForce overlay
    };
    for (const wchar_t* d : kDenied) {
        if (!wcscmp(cls, d)) {
            return true;
        }
    }
    return false;
}

// Where the window is (or, if minimized, where it will be once restored), in
// screen coordinates.
static bool GetStartRect(HWND hwnd, RECT* out, bool* minimized) {
    *minimized = IsIconic(hwnd) != FALSE;
    if (!*minimized) {
        return GetWindowRect(hwnd, out) != FALSE;
    }

    WINDOWPLACEMENT wp{};
    wp.length = sizeof(wp);
    if (!GetWindowPlacement(hwnd, &wp)) {
        return false;
    }

    RECT r = wp.rcNormalPosition;  // workspace coordinates
    MONITORINFO pm{};
    pm.cbSize = sizeof(pm);
    if (GetMonitorInfoW(MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY), &pm)) {
        OffsetRect(&r, pm.rcWork.left - pm.rcMonitor.left,
                   pm.rcWork.top - pm.rcMonitor.top);
    }

    if (wp.flags & WPF_RESTORETOMAXIMIZED) {
        MONITORINFO mi{};
        mi.cbSize = sizeof(mi);
        if (GetMonitorInfoW(MonitorFromRect(&r, MONITOR_DEFAULTTONEAREST), &mi)) {
            r = mi.rcWork;
        }
    }

    *out = r;
    return true;
}

// Visible dialogs owned (directly or not) by `owner`, top-most first.
struct KidCtx {
    HWND owner;
    std::vector<HWND> list;
};

static bool IsOwnedByChain(HWND w, HWND root) {
    for (int i = 0; i < 8; i++) {
        w = GetWindow(w, GW_OWNER);
        if (!w) {
            return false;
        }
        if (w == root) {
            return true;
        }
    }
    return false;
}

static BOOL CALLBACK KidEnumProc(HWND h, LPARAM lp) {
    auto* c = reinterpret_cast<KidCtx*>(lp);
    if (h == c->owner || h == g_overlay || h == g_highlight || h == g_thumbHost ||
        !IsWindowVisible(h) || IsIconic(h) || !IsOwnedByChain(h, c->owner)) {
        return TRUE;
    }

    DWORD cloaked = 0;
    DwmGetWindowAttribute(h, kDwmwaCloaked, &cloaked, sizeof(cloaked));
    if (cloaked) {
        return TRUE;
    }

    const LONG_PTR ex = GetWindowLongPtrW(h, GWL_EXSTYLE);
    if ((ex & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) && !(ex & WS_EX_APPWINDOW)) {
        return TRUE;
    }
    if (IsInvisibleLayered(h, ex)) {
        return TRUE;
    }

    // Real dialogs have a caption or a title; menus and tooltips do not.
    const LONG_PTR st = GetWindowLongPtrW(h, GWL_STYLE);
    if ((st & WS_CAPTION) != WS_CAPTION && !HasWindowTitle(h)) {
        return TRUE;
    }

    wchar_t cls[96] = {};
    GetClassNameW(h, cls, 96);
    if (IsDeniedClass(cls) || !wcscmp(cls, L"#32768")) {
        return TRUE;
    }

    RECT r{};
    if (!GetWindowRect(h, &r) || r.right - r.left < 40 || r.bottom - r.top < 30) {
        return TRUE;
    }

    c->list.push_back(h);
    return c->list.size() < 4;
}

static BOOL CALLBACK EnumProc(HWND hwnd, LPARAM lp) {
    auto* ctx = reinterpret_cast<EnumCtx*>(lp);

    if (hwnd == g_overlay || hwnd == g_highlight || hwnd == g_thumbHost ||
        !IsWindowVisible(hwnd)) {
        return TRUE;
    }

    // Mission Control is per virtual desktop: windows of other desktops stay
    // out. (Their cloak would exclude most of them, but a minimized window of
    // another desktop is not cloaked.) A window shown on every desktop counts
    // as being on the current one.
    if (IVirtualDesktopManager* vdm = GetVdm()) {
        BOOL onThis = TRUE;
        if (SUCCEEDED(vdm->IsWindowOnCurrentVirtualDesktop(hwnd, &onThis)) && !onThis) {
            return TRUE;
        }
    }

    const bool iconic = IsIconic(hwnd) != FALSE;

    DWORD cloaked = 0;
    DwmGetWindowAttribute(hwnd, kDwmwaCloaked, &cloaked, sizeof(cloaked));
    // Other virtual desktop / suspended UWP are skipped. A minimized app may
    // also be cloaked by its own app, so for those only the "lives on another
    // virtual desktop" reason (0x2) excludes it.
    if (cloaked && !(iconic && !(cloaked & 0x2))) {
        return TRUE;
    }

    const LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if ((ex & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) && !(ex & WS_EX_APPWINDOW)) {
        return TRUE;
    }

    if (IsInvisibleLayered(hwnd, ex)) {
        return TRUE;
    }

    // A window owned by another visible window (a dialog) is not a tile of
    // its own: it is drawn on top of its owner's tile and activated with it.
    HWND owner = GetWindow(hwnd, GW_OWNER);
    if (owner && IsWindowVisible(owner)) {
        return TRUE;
    }

    if (!SHOW_ALWAYS_ON_TOP && (ex & WS_EX_TOPMOST)) {
        return TRUE;
    }
    if (!SHOW_MINIMIZED && iconic) {
        return TRUE;
    }

    if (!HasWindowTitle(hwnd)) {
        return TRUE;
    }

    wchar_t cls[96] = {};
    GetClassNameW(hwnd, cls, 96);
    if (IsDeniedClass(cls)) {
        return TRUE;
    }

    RECT r;
    bool minimizedNow = false;
    if (!GetStartRect(hwnd, &r, &minimizedNow)) {
        return TRUE;
    }

    if (r.right - r.left < 100 || r.bottom - r.top < 60) {
        return TRUE;
    }

    RECT inter;
    if (!IntersectRect(&inter, &r, &ctx->work)) {
        return TRUE;
    }

    ctx->hwnds.push_back(hwnd);
    return ctx->hwnds.size() < MAX_TILES;  // FALSE stops the enumeration
}

static float Smooth01(float t) {
    t = Clamp01(t);
    return t * t * (3.0f - 2.0f * t);
}

static bool OpacityChanged(float now, float last) {
    return std::fabs(now - last) > 0.004f ||
           ((now <= 0.0f || now >= 1.0f) && now != last);
}

// Fingers past 100%: display progress goes a bit beyond 1 with rubber-band
// resistance (RawToProgress), and ProgressToRaw is its inverse so a new swipe
// that starts mid-bounce doesn't jump.
static float RawToProgress(float raw) {
    if (raw <= 0.0f) {
        return 0.0f;
    }
    if (raw <= 1.0f) {
        return raw;
    }
    if (g_reduceMotion) {
        return 1.0f;  // no stretch with animations off
    }
    const float d = DRAG_OVERSHOOT_MAX;
    const float x = raw - 1.0f;
    return 1.0f + d * (1.0f - 1.0f / (x * DRAG_RUBBER_STIFFNESS / d + 1.0f));
}

static float ProgressToRaw(float p) {
    if (p <= 1.0f) {
        return p < 0.0f ? 0.0f : p;
    }
    const float d = DRAG_OVERSHOOT_MAX;
    float y = p - 1.0f;
    if (y > d * 0.98f) {
        y = d * 0.98f;
    }
    return 1.0f + (d / DRAG_RUBBER_STIFFNESS) * (1.0f / (1.0f - y / d) - 1.0f);
}

// "No apps opened" in the language Windows is displayed in.
struct EmptyText {
    const wchar_t* text;
    const wchar_t* font;  // face with the right glyphs for that script
    bool rtl;
};

static EmptyText GetEmptyMessage() {
    const LANGID lid = GetUserDefaultUILanguage();
    const WORD primary = PRIMARYLANGID(lid);
    const WORD sub = SUBLANGID(lid);
    const wchar_t* ui = L"Segoe UI";

    switch (primary) {
        case LANG_SPANISH:    return { L"No hay aplicaciones abiertas", ui, false };
        case LANG_PORTUGUESE:
            if (sub == SUBLANG_PORTUGUESE_BRAZILIAN) {
                return { L"Nenhum aplicativo aberto", ui, false };
            }
            return { L"Nenhuma aplicação aberta", ui, false };
        case LANG_FRENCH:     return { L"Aucune application ouverte", ui, false };
        case LANG_GERMAN:     return { L"Keine Apps geöffnet", ui, false };
        case LANG_ITALIAN:    return { L"Nessuna app aperta", ui, false };
        case LANG_DUTCH:      return { L"Geen apps geopend", ui, false };
        case LANG_CATALAN:    return { L"Cap aplicació oberta", ui, false };
        case LANG_POLISH:     return { L"Brak otwartych aplikacji", ui, false };
        case LANG_CZECH:      return { L"Žádné otevřené aplikace", ui, false };
        case LANG_SLOVAK:     return { L"Žiadne otvorené aplikácie", ui, false };
        case LANG_SLOVENIAN:  return { L"Ni odprtih aplikacij", ui, false };
        case LANG_CROATIAN:   return { L"Nema otvorenih aplikacija", ui, false };
        case LANG_HUNGARIAN:  return { L"Nincs megnyitott alkalmazás", ui, false };
        case LANG_ROMANIAN:   return { L"Nicio aplicație deschisă", ui, false };
        case LANG_BULGARIAN:  return { L"Няма отворени приложения", ui, false };
        case LANG_RUSSIAN:    return { L"Нет открытых приложений", ui, false };
        case LANG_UKRAINIAN:  return { L"Немає відкритих програм", ui, false };
        case LANG_GREEK:      return { L"Δεν υπάρχουν ανοιχτές εφαρμογές", ui, false };
        case LANG_TURKISH:    return { L"Açık uygulama yok", ui, false };
        case LANG_SWEDISH:    return { L"Inga appar öppna", ui, false };
        case LANG_DANISH:     return { L"Ingen åbne apps", ui, false };
        case LANG_NORWEGIAN:  return { L"Ingen åpne apper", ui, false };
        case LANG_FINNISH:    return { L"Ei avoimia sovelluksia", ui, false };
        case LANG_INDONESIAN: return { L"Tidak ada aplikasi yang terbuka", ui, false };
        case LANG_VIETNAMESE: return { L"Không có ứng dụng nào đang mở", ui, false };
        case LANG_THAI:       return { L"ไม่มีแอปที่เปิดอยู่", ui, false };
        case LANG_HINDI:      return { L"कोई ऐप खुला नहीं है", L"Nirmala UI", false };
        case LANG_ARABIC:     return { L"لا توجد تطبيقات مفتوحة", ui, true };
        case LANG_HEBREW:     return { L"אין אפליקציות פתוחות", ui, true };
        case LANG_JAPANESE:   return { L"開いているアプリはありません", L"Yu Gothic UI", false };
        case LANG_KOREAN:     return { L"열려 있는 앱이 없습니다", L"Malgun Gothic", false };
        case LANG_CHINESE:
            if (sub == SUBLANG_CHINESE_TRADITIONAL || sub == SUBLANG_CHINESE_HONGKONG ||
                sub == SUBLANG_CHINESE_MACAU) {
                return { L"沒有開啟的應用程式", L"Microsoft JhengHei UI", false };
            }
            return { L"没有打开的应用", L"Microsoft YaHei UI", false };
        default:              break;
    }
    return { L"No apps opened", ui, false };
}

// Segoe UI Variable (Text) when it is installed, otherwise the face asked for.
// Regular weight and plain grayscale smoothing: flat and thin, like the rest of
// Windows 11's interface.
static HFONT CreateUiFont(HDC dc, int px, const wchar_t* face) {
    const wchar_t* want = !wcscmp(face, L"Segoe UI") ? L"Segoe UI Variable Text" : face;
    auto make = [&](const wchar_t* f) {
        return CreateFontW(-px, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                           OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                           DEFAULT_PITCH | FF_DONTCARE, f);
    };

    HFONT font = make(want);
    if (font && want != face) {
        HGDIOBJ old = SelectObject(dc, font);
        wchar_t got[LF_FACESIZE] = {};
        GetTextFaceW(dc, LF_FACESIZE, got);
        SelectObject(dc, old);
        if (_wcsicmp(got, want) != 0) {  // not installed: the face we were given
            DeleteObject(font);
            font = make(face);
        }
    }
    return font;
}

static float MinF(float a, float b) {
    return a < b ? a : b;
}

static float MaxF(float a, float b) {
    return a > b ? a : b;
}

// Rows of windows, all at the same scale, each row centered, and the whole
// block centered on the screen (what GNOME's overview does). The number of
// rows is whichever lets the windows be biggest.
static void LayoutTiles() {
    const int n = static_cast<int>(g_tiles.size());
    if (n == 0) {
        return;
    }

    const float W = static_cast<float>(g_workRect.right - g_workRect.left);
    const float H = static_cast<float>(g_workRect.bottom - g_workRect.top);
    const float areaW = W - 2 * OVERLAY_MARGIN;
    const float areaH = H - 2 * OVERLAY_MARGIN - static_cast<float>(g_barBand);
    const float gap = static_cast<float>(TILE_PADDING);
    const float band = static_cast<float>(g_iconBand);  // room for the icons

    auto tw = [&](int i) { return static_cast<float>(g_tiles[i].from.right - g_tiles[i].from.left); };
    auto th = [&](int i) { return static_cast<float>(g_tiles[i].from.bottom - g_tiles[i].from.top); };

    // Largest uniform scale at which `rows` rows fit in the area.
    auto fitScale = [&](int rows) {
        float s = MAX_TILE_SCALE;
        float sumRowH = 0.0f;
        int idx = 0;
        for (int r = 0; r < rows; r++) {
            const int cnt = n / rows + (r < n % rows ? 1 : 0);
            float sumW = 0.0f, rowH = 0.0f;
            for (int k = 0; k < cnt; k++, idx++) {
                sumW += tw(idx);
                rowH = MaxF(rowH, th(idx));
            }
            s = MinF(s, (areaW - gap * (cnt - 1)) / sumW);
            sumRowH += rowH;
        }
        s = MinF(s, (areaH - gap * (rows - 1) - band * rows) / sumRowH);
        return MaxF(s, 0.03f);
    };

    // Fewer rows (windows side by side) wins unless more rows is clearly bigger.
    int bestRows = 1;
    float bestScale = fitScale(1);
    for (int rows = 2; rows <= n; rows++) {
        const float s = fitScale(rows);
        if (s > bestScale * 1.03f) {
            bestScale = s;
            bestRows = rows;
        }
    }

    const int rows = bestRows;
    const float s = bestScale;

    float totalH = gap * (rows - 1);
    {
        int idx = 0;
        for (int r = 0; r < rows; r++) {
            const int cnt = n / rows + (r < n % rows ? 1 : 0);
            float rowH = 0.0f;
            for (int k = 0; k < cnt; k++, idx++) {
                rowH = MaxF(rowH, th(idx));
            }
            totalH += rowH * s + band;
        }
    }

    float y = OVERLAY_MARGIN + (areaH - totalH) * 0.5f;
    int idx = 0;
    for (int r = 0; r < rows; r++) {
        const int cnt = n / rows + (r < n % rows ? 1 : 0);

        float rowW = gap * (cnt - 1);
        float rowH = 0.0f;
        for (int k = 0; k < cnt; k++) {
            rowW += tw(idx + k) * s;
            rowH = MaxF(rowH, th(idx + k) * s);
        }

        float x = OVERLAY_MARGIN + (areaW - rowW) * 0.5f;
        for (int k = 0; k < cnt; k++, idx++) {
            const float dw = tw(idx) * s;
            const float dh = th(idx) * s;
            const float top = y + (rowH - dh) * 0.5f;

            Tile& t = g_tiles[idx];
            t.to.left = static_cast<LONG>(x);
            t.to.top = static_cast<LONG>(top);
            t.to.right = static_cast<LONG>(x + dw);
            t.to.bottom = static_cast<LONG>(top + dh);

            x += dw + gap;
        }
        y += rowH + band + gap;
    }
}

// A tile is also clickable / hoverable over the icon under it.
static bool TileContains(const Tile& t, POINT pt) {
    RECT r = t.to;
    r.bottom += g_iconBand;
    return PtInRect(&r, pt) != FALSE;
}

// ---------------------------------------------------------------------------
// DirectComposition thumbnails
//
// The overlay window keeps the Mica backdrop and the input handling. The live
// thumbnails go in a second, click-through window on top of it whose content
// is a DirectComposition visual tree. Each tile is a visual made by DWM's
// private DwmpCreateSharedThumbnailVisual (ordinal 147): DWM keeps it live on
// its own, and moving it is one transform change on the visual, batched into
// a single Commit() per frame. No per-tile RPC to dwm.exe.
//
// If anything here is unavailable (missing exports, no D3D device, ...) the
// mod silently falls back to the old DwmRegisterThumbnail path.
// ---------------------------------------------------------------------------


typedef HRESULT(WINAPI* PFN_DwmpCreateSharedThumbnailVisual)(
    HWND hwndDestination, HWND hwndSource, DWORD dwThumbnailFlags,
    DWM_THUMBNAIL_PROPERTIES* pThumbnailProperties, void* pDCompDevice,
    void** ppVisual, HTHUMBNAIL* phThumbnailId);
typedef HRESULT(WINAPI* PFN_DwmpQueryWindowThumbnailSourceSize)(
    HWND hwndSource, BOOL fSourceClientAreaOnly, SIZE* pSize);
typedef HRESULT(WINAPI* PFN_DCompositionCreateDevice3)(
    IUnknown* renderingDevice, REFIID iid, void** dcompositionDevice);

static PFN_DwmpCreateSharedThumbnailVisual g_pCreateShared = nullptr;
static PFN_DwmpQueryWindowThumbnailSourceSize g_pQuerySize = nullptr;

static ID3D11Device* g_d3d = nullptr;
static IDXGIDevice* g_dxgi = nullptr;
static IDCompositionDesktopDevice* g_dcDevice = nullptr;
static IDCompositionTarget* g_dcTarget = nullptr;
static IDCompositionVisual2* g_dcRoot = nullptr;
// Top-most tile visual currently in the tree (not ref-counted, just a marker).
// AddVisual's z-order with a NULL reference is easy to get backwards, so every
// insertion names this visual explicitly as the one to go above.
static IDCompositionVisual2* g_dcTop = nullptr;
// Two layers under the root: tile thumbnails, and above them the icons.
static IDCompositionVisual2* g_dcTiles = nullptr;
static IDCompositionVisual2* g_dcIcons = nullptr;
static IDCompositionVisual2* g_dcIconTop = nullptr;
static IDCompositionVisual2* g_dcTaskbarLayer = nullptr;  // above the icons: the taskbar copy
static IDCompositionVisual2* g_dcBarLayer = nullptr;       // the virtual desktops bar
static IDCompositionEffectGroup* g_barFx = nullptr;        // its opacity
static IDCompositionEffectGroup* g_tilesFx = nullptr;      // opacity of the whole tile layer
static float g_tilesOp = -1.0f;
static bool g_holdTiles = false;                           // keep the tiles opaque (minimized pick: they hide its restore)
static IDCompositionVisual2* g_barStrip = nullptr;         // parent of the desktop cards
static IDCompositionSurface* g_xSurf[2] = { nullptr, nullptr };  // close button: normal / hovered
static ID3D11DeviceContext* g_d3dCtx = nullptr;
// App icons as DComp surfaces, kept between gestures (key: exe path, or the
// window handle when the icon comes from the window itself).
static std::map<std::wstring, IDCompositionSurface*> g_iconCache;
static bool g_dcOk = false;
static bool g_thumbHostClassRegistered = false;

static LRESULT CALLBACK ThumbHostProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_NCHITTEST:
            // Mouse goes to the overlay window right below (same thread).
            return HTTRANSPARENT;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static void ShutdownDComp() {
    g_dcOk = false;
    g_dcTop = nullptr;
    g_dcIconTop = nullptr;
    for (auto& kv : g_iconCache) {
        if (kv.second) {
            kv.second->Release();
        }
    }
    g_iconCache.clear();
    if (g_dcTiles) { g_dcTiles->Release(); g_dcTiles = nullptr; }
    if (g_dcIcons) { g_dcIcons->Release(); g_dcIcons = nullptr; }
    if (g_dcTaskbarLayer) { g_dcTaskbarLayer->Release(); g_dcTaskbarLayer = nullptr; }
    if (g_dcBarLayer) { g_dcBarLayer->Release(); g_dcBarLayer = nullptr; }
    if (g_barFx) { g_barFx->Release(); g_barFx = nullptr; }
    if (g_tilesFx) { g_tilesFx->Release(); g_tilesFx = nullptr; }
    if (g_barStrip) { g_barStrip->Release(); g_barStrip = nullptr; }
    for (auto& xs : g_xSurf) { if (xs) { xs->Release(); xs = nullptr; } }
    if (g_vdm) { g_vdm->Release(); g_vdm = nullptr; }
    if (g_d3dCtx) { g_d3dCtx->Release(); g_d3dCtx = nullptr; }
    if (g_dcRoot) { g_dcRoot->Release(); g_dcRoot = nullptr; }
    if (g_dcTarget) { g_dcTarget->Release(); g_dcTarget = nullptr; }
    if (g_dcDevice) { g_dcDevice->Release(); g_dcDevice = nullptr; }
    if (g_dxgi) { g_dxgi->Release(); g_dxgi = nullptr; }
    if (g_d3d) { g_d3d->Release(); g_d3d = nullptr; }
    if (g_thumbHost) {
        DestroyWindow(g_thumbHost);
        g_thumbHost = nullptr;
    }
    if (g_thumbHostClassRegistered) {
        UnregisterClassW(kThumbHostClass, ModuleInstance());
        g_thumbHostClassRegistered = false;
    }
}

static bool InitDComp() {
    HMODULE dwm = GetModuleHandleW(L"dwmapi.dll");
    HMODULE dcomp = LoadLibraryExW(L"dcomp.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!dwm || !dcomp) {
        Wh_Log(L"DComp: dwmapi/dcomp not loaded");
        return false;
    }

    g_pCreateShared = reinterpret_cast<PFN_DwmpCreateSharedThumbnailVisual>(
        GetProcAddress(dwm, MAKEINTRESOURCEA(147)));
    g_pQuerySize = reinterpret_cast<PFN_DwmpQueryWindowThumbnailSourceSize>(
        GetProcAddress(dwm, MAKEINTRESOURCEA(162)));
    auto createDevice3 = reinterpret_cast<PFN_DCompositionCreateDevice3>(
        GetProcAddress(dcomp, "DCompositionCreateDevice3"));
    if (!g_pCreateShared || !g_pQuerySize || !createDevice3) {
        Wh_Log(L"DComp: private DWM exports or DCompositionCreateDevice3 missing");
        return false;
    }

    // Sanity check that ordinal 162 really is the size query on this build:
    // for the taskbar it must agree with GetWindowRect. If not, the ordinals
    // moved and calling them would be unsafe.
    if (HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr)) {
        RECT wr{};
        SIZE sz{};
        if (!GetWindowRect(tray, &wr) ||
            FAILED(g_pQuerySize(tray, FALSE, &sz)) ||
            std::abs(sz.cx - (wr.right - wr.left)) > 8 ||
            std::abs(sz.cy - (wr.bottom - wr.top)) > 8) {
            Wh_Log(L"DComp: ordinal sanity check failed, using legacy thumbnails");
            return false;
        }
    }

    if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
                                 D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0,
                                 D3D11_SDK_VERSION, &g_d3d, nullptr, nullptr)) ||
        FAILED(g_d3d->QueryInterface(__uuidof(IDXGIDevice),
                                     reinterpret_cast<void**>(&g_dxgi)))) {
        Wh_Log(L"DComp: D3D11 device creation failed");
        ShutdownDComp();
        return false;
    }

    HINSTANCE inst = ModuleInstance();
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = ThumbHostProc;
    wc.hInstance = inst;
    wc.lpszClassName = kThumbHostClass;
    if (!RegisterClassExW(&wc)) {
        Wh_Log(L"DComp: RegisterClassExW failed: %lu", GetLastError());
        ShutdownDComp();
        return false;
    }
    g_thumbHostClassRegistered = true;

    g_thumbHost = CreateWindowExW(
        WS_EX_NOREDIRECTIONBITMAP | WS_EX_TOPMOST | WS_EX_TOOLWINDOW |
            WS_EX_NOACTIVATE | WS_EX_TRANSPARENT,
        kThumbHostClass, L"", WS_POPUP, 0, 0, 100, 100,
        nullptr, nullptr, inst, nullptr);
    if (!g_thumbHost) {
        Wh_Log(L"DComp: CreateWindowExW failed: %lu", GetLastError());
        ShutdownDComp();
        return false;
    }

    if (FAILED(createDevice3(g_dxgi, __uuidof(IDCompositionDesktopDevice),
                             reinterpret_cast<void**>(&g_dcDevice))) ||
        FAILED(g_dcDevice->CreateTargetForHwnd(g_thumbHost, TRUE, &g_dcTarget)) ||
        FAILED(g_dcDevice->CreateVisual(&g_dcRoot)) ||
        FAILED(g_dcTarget->SetRoot(g_dcRoot)) ||
        FAILED(g_dcDevice->Commit())) {
        Wh_Log(L"DComp: device/target/visual creation failed");
        ShutdownDComp();
        return false;
    }

    g_d3d->GetImmediateContext(&g_d3dCtx);
    if (!g_d3dCtx ||
        FAILED(g_dcDevice->CreateVisual(&g_dcTiles)) ||
        FAILED(g_dcDevice->CreateVisual(&g_dcIcons)) ||
        FAILED(g_dcDevice->CreateVisual(&g_dcTaskbarLayer)) ||
        FAILED(g_dcDevice->CreateVisual(&g_dcBarLayer)) ||
        FAILED(g_dcDevice->CreateEffectGroup(&g_barFx)) ||
        FAILED(g_dcDevice->CreateEffectGroup(&g_tilesFx)) ||
        FAILED(g_dcRoot->AddVisual(g_dcTiles, TRUE, nullptr)) ||
        FAILED(g_dcRoot->AddVisual(g_dcIcons, TRUE, g_dcTiles)) ||  // above the tiles
        FAILED(g_dcRoot->AddVisual(g_dcBarLayer, TRUE, g_dcIcons)) ||
        FAILED(g_dcRoot->AddVisual(g_dcTaskbarLayer, TRUE, g_dcBarLayer)) ||
        FAILED(g_dcBarLayer->SetEffect(g_barFx)) ||
        FAILED(g_dcTiles->SetEffect(g_tilesFx)) ||
        FAILED(g_dcDevice->Commit())) {
        Wh_Log(L"DComp: layer creation failed");
        ShutdownDComp();
        return false;
    }

    g_barFx->SetOpacity(0.0f);
    g_tilesFx->SetOpacity(1.0f);
    g_dcDevice->Commit();

    g_dcOk = true;
    Wh_Log(L"DComp thumbnails enabled");
    return true;
}

// The visual DWM gives us may cover only the VISIBLE frame of a window, while
// GetWindowRect also includes the invisible resize borders (about 8 px). If the
// tile's start rect is the full window rect, the thumbnail then doesn't sit
// exactly on top of the real window when the animation starts / ends. So ask
// DWM for the thumbnail's real size and, when it matches the visible frame
// instead of the full rect, start/end the tile on the visible frame.
// Must run before LayoutTiles(). Fills srcW/srcH (0 = DComp not usable).
static void CalibrateDCompSource(Tile& t) {
    t.srcW = t.srcH = 0.0f;
    if (!g_dcOk) {
        return;
    }

    SIZE sz{};
    if (FAILED(g_pQuerySize(t.hwnd, FALSE, &sz)) || sz.cx <= 0 || sz.cy <= 0) {
        return;
    }
    t.srcW = static_cast<float>(sz.cx);
    t.srcH = static_cast<float>(sz.cy);

    if (t.minimized) {
        // `from` is where the window will be once restored. Center a rect of
        // the thumbnail's size there, keeping the top edge.
        const LONG cx = (t.from.left + t.from.right) / 2;
        t.from.left = cx - sz.cx / 2;
        t.from.right = t.from.left + sz.cx;
        t.from.bottom = t.from.top + sz.cy;
        Wh_Log(L"  thumb (minimized) src=%ldx%ld", sz.cx, sz.cy);
        return;
    }

    const int winW = t.from.right - t.from.left;
    const int winH = t.from.bottom - t.from.top;

    RECT ext{};
    bool useExt = false;
    int extW = 0, extH = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(t.hwnd, 9 /*DWMWA_EXTENDED_FRAME_BOUNDS*/,
                                        &ext, sizeof(ext)))) {
        extW = ext.right - ext.left;
        extH = ext.bottom - ext.top;
        const bool matchesExt =
            std::abs(sz.cx - extW) <= 2 && std::abs(sz.cy - extH) <= 2;
        const bool matchesWin =
            std::abs(sz.cx - winW) <= 2 && std::abs(sz.cy - winH) <= 2;
        if (matchesExt && !matchesWin) {
            t.from = ext;
            OffsetRect(&t.from, -g_workRect.left, -g_workRect.top);
            useExt = true;
        }
    }

    Wh_Log(L"  thumb src=%ldx%ld window=%dx%d visibleFrame=%dx%d -> start on %s",
           sz.cx, sz.cy, winW, winH, extW, extH,
           useExt ? L"visible frame" : L"window rect");
}

// Creates the DComp visual for one tile and puts it on top of the tree.
// Returns false (tile stays legacy) if DWM refuses.
static bool CreateDCompTile(Tile& t) {
    if (!g_dcOk || t.srcW < 2.0f || t.srcH < 2.0f) {
        return false;
    }

    const LONG w = static_cast<LONG>(t.srcW);
    const LONG h = static_cast<LONG>(t.srcH);

    DWM_THUMBNAIL_PROPERTIES props{};
    props.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_RECTSOURCE |
                    DWM_TNP_VISIBLE | DWM_TNP_OPACITY |
                    DWM_TNP_SOURCECLIENTAREAONLY;
    props.rcDestination = { 0, 0, w, h };  // natural size; we scale it
    props.rcSource = { 0, 0, w, h };
    props.opacity = 255;
    props.fVisible = TRUE;
    props.fSourceClientAreaOnly = FALSE;

    IDCompositionVisual2* visual = nullptr;
    HTHUMBNAIL thumb = nullptr;
    HRESULT hr = g_pCreateShared(g_thumbHost, t.hwnd, 2, &props, g_dcDevice,
                                 reinterpret_cast<void**>(&visual), &thumb);
    if (FAILED(hr) || !visual) {
        return false;
    }

    visual->SetBitmapInterpolationMode(DCOMPOSITION_BITMAP_INTERPOLATION_MODE_LINEAR);
    // Tiles are created bottom-most first: each one goes directly above the
    // previous top, so the real top-most window ends up on top.
    if (FAILED(g_dcTiles->AddVisual(visual, TRUE, g_dcTop))) {
        visual->Release();
        if (thumb) {
            DwmUnregisterThumbnail(thumb);
        }
        return false;
    }
    g_dcTop = visual;

    // Minimized windows have nothing on screen to match, so the tile fades in.
    if (t.minimized) {
        IDCompositionEffectGroup* fx = nullptr;
        if (SUCCEEDED(g_dcDevice->CreateEffectGroup(&fx)) && fx) {
            fx->SetOpacity(0.0f);
            visual->SetEffect(fx);
            t.dcFx = fx;
            t.fadeIn = true;
            t.fxOp = 0.0f;
        }
    }

    t.dcVisual = visual;
    t.dcThumb = thumb;
    t.placed = false;
    return true;
}

// ---- App icons --------------------------------------------------------------

static bool GetWindowExePath(HWND hwnd, wchar_t* buf, DWORD cap) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!h) {
        return false;
    }
    DWORD n = cap;
    const BOOL ok = QueryFullProcessImageNameW(h, 0, buf, &n);
    CloseHandle(h);
    return ok != FALSE;
}

// The icon the window itself advertises (borrowed handle, don't destroy).
static HICON GetWindowIconHandle(HWND h) {
    DWORD_PTR r = 0;
    const UINT f = SMTO_ABORTIFHUNG | SMTO_BLOCK;
    if (SendMessageTimeoutW(h, WM_GETICON, ICON_BIG, 0, f, 50, &r) && r) {
        return reinterpret_cast<HICON>(r);
    }
    if (SendMessageTimeoutW(h, WM_GETICON, ICON_SMALL2, 0, f, 50, &r) && r) {
        return reinterpret_cast<HICON>(r);
    }
    HICON c = reinterpret_cast<HICON>(GetClassLongPtrW(h, GCLP_HICON));
    if (!c) {
        c = reinterpret_cast<HICON>(GetClassLongPtrW(h, GCLP_HICONSM));
    }
    return c ? c : LoadIconW(nullptr, IDI_APPLICATION);
}

// Draws the icon twice (on black, then on white) to recover a correct alpha
// for every kind of icon, and returns premultiplied BGRA pixels.
static bool RenderIconPixels(HICON icon, int size, std::vector<uint32_t>& out) {
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = size;
    bi.bmiHeader.biHeight = -size;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    HDC screen = GetDC(nullptr);
    HDC mem = CreateCompatibleDC(screen);
    void* bitsB = nullptr;
    void* bitsW = nullptr;
    HBITMAP bmpB = CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &bitsB, nullptr, 0);
    HBITMAP bmpW = CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &bitsW, nullptr, 0);

    bool ok = false;
    if (mem && bmpB && bmpW && bitsB && bitsW) {
        const size_t count = static_cast<size_t>(size) * size;

        HGDIOBJ old = SelectObject(mem, bmpB);
        memset(bitsB, 0x00, count * 4);
        DrawIconEx(mem, 0, 0, icon, size, size, 0, nullptr, DI_NORMAL);
        SelectObject(mem, bmpW);
        memset(bitsW, 0xFF, count * 4);
        DrawIconEx(mem, 0, 0, icon, size, size, 0, nullptr, DI_NORMAL);
        GdiFlush();
        SelectObject(mem, old);

        const uint32_t* pb = static_cast<const uint32_t*>(bitsB);
        const uint32_t* pw = static_cast<const uint32_t*>(bitsW);
        out.resize(count);
        for (size_t i = 0; i < count; i++) {
            int a = 255 - (static_cast<int>((pw[i] >> 8) & 0xFF) -
                           static_cast<int>((pb[i] >> 8) & 0xFF));
            a = a < 0 ? 0 : (a > 255 ? 255 : a);
            uint32_t b = pb[i] & 0xFF;
            uint32_t g = (pb[i] >> 8) & 0xFF;
            uint32_t r = (pb[i] >> 16) & 0xFF;
            const uint32_t ua = static_cast<uint32_t>(a);
            if (b > ua) b = ua;
            if (g > ua) g = ua;
            if (r > ua) r = ua;
            out[i] = (ua << 24) | (r << 16) | (g << 8) | b;
        }
        ok = true;
    }

    if (bmpB) DeleteObject(bmpB);
    if (bmpW) DeleteObject(bmpW);
    if (mem) DeleteDC(mem);
    if (screen) ReleaseDC(nullptr, screen);
    return ok;
}

// Uploads premultiplied BGRA pixels into a new DComp surface.
static IDCompositionSurface* CreatePixelSurface(const std::vector<uint32_t>& px, int w, int h) {
    IDCompositionSurface* surf = nullptr;
    if (FAILED(g_dcDevice->CreateSurface(w, h, DXGI_FORMAT_B8G8R8A8_UNORM,
                                         DXGI_ALPHA_MODE_PREMULTIPLIED, &surf)) ||
        !surf) {
        return nullptr;
    }

    ID3D11Texture2D* tex = nullptr;
    POINT off{};
    if (FAILED(surf->BeginDraw(nullptr, __uuidof(ID3D11Texture2D),
                               reinterpret_cast<void**>(&tex), &off)) ||
        !tex) {
        surf->Release();
        return nullptr;
    }

    D3D11_BOX box{};
    box.left = static_cast<UINT>(off.x);
    box.top = static_cast<UINT>(off.y);
    box.right = static_cast<UINT>(off.x + w);
    box.bottom = static_cast<UINT>(off.y + h);
    box.front = 0;
    box.back = 1;
    g_d3dCtx->UpdateSubresource(tex, 0, &box, px.data(), w * 4, 0);
    tex->Release();
    surf->EndDraw();
    return surf;
}

// Renders one line of text into premultiplied BGRA pixels.
//
// The text is drawn white on black with grayscale smoothing, which gives the
// coverage of every pixel directly; that coverage becomes the alpha of ONE flat
// color. (Recovering the alpha from a black and a white rendering, as the icons
// do, is inexact for text and leaves a bold, haloed look.)

static bool RenderTextPixels(const wchar_t* text, COLORREF color, int fontPx,
                             const wchar_t* face, bool rtl,
                             std::vector<uint32_t>& out, int& outW, int& outH,
                             int maxWidth = 0) {
    HDC screen = GetDC(nullptr);
    HDC mem = CreateCompatibleDC(screen);
    HFONT font = CreateUiFont(mem, fontPx, face);
    HGDIOBJ oldFont = SelectObject(mem, font);

    // DrawText (not TextOut) so complex scripts (Arabic, Hebrew, Hindi, Thai...)
    // are shaped and ordered correctly. The ellipsis flag must NOT be used to
    // measure: with an empty rectangle it would make the text look like it has
    // no room at all ("V...").
    const UINT flags = DT_NOPREFIX | DT_SINGLELINE | (rtl ? DT_RTLREADING : 0);
    RECT measure{ 0, 0, 0, 0 };
    DrawTextW(mem, text, -1, &measure, flags | DT_CALCRECT);
    const int pad = 8;
    int w = (measure.right - measure.left) + 2 * pad;
    const int h = (measure.bottom - measure.top) + 2 * pad;
    if (maxWidth > 0 && w > maxWidth) {
        w = maxWidth;  // longer text ends with an ellipsis
    }

    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP bmp = nullptr;
    if (w > 2 * pad && h > 2 * pad) {
        bmp = CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    }

    bool ok = false;
    if (bmp && bits) {
        const size_t count = static_cast<size_t>(w) * h;
        SetBkMode(mem, TRANSPARENT);
        SetTextColor(mem, RGB(255, 255, 255));

        HGDIOBJ oldBmp = SelectObject(mem, bmp);
        memset(bits, 0x00, count * 4);
        RECT area{ pad, pad, w - pad, h - pad };
        DrawTextW(mem, text, -1, &area, flags | DT_END_ELLIPSIS);  // only cuts what doesn't fit
        GdiFlush();
        SelectObject(mem, oldBmp);

        const uint32_t* src = static_cast<const uint32_t*>(bits);
        const float cr = static_cast<float>(GetRValue(color));
        const float cg = static_cast<float>(GetGValue(color));
        const float cb = static_cast<float>(GetBValue(color));
        out.resize(count);
        for (size_t i = 0; i < count; i++) {
            const float a = static_cast<float>(src[i] & 0xFF) / 255.0f;  // R = G = B
            const uint32_t ua = static_cast<uint32_t>(a * 255.0f + 0.5f);
            out[i] = (ua << 24) |
                     (static_cast<uint32_t>(cr * a + 0.5f) << 16) |
                     (static_cast<uint32_t>(cg * a + 0.5f) << 8) |
                     static_cast<uint32_t>(cb * a + 0.5f);
        }
        outW = w;
        outH = h;
        ok = true;
    }

    if (bmp) DeleteObject(bmp);
    SelectObject(mem, oldFont);
    DeleteObject(font);
    DeleteDC(mem);
    ReleaseDC(nullptr, screen);
    return ok;
}

// ---- UWP / packaged apps ----------------------------------------------------
// Their windows don't carry a usable HICON, and the process behind a UWP
// frame is ApplicationFrameHost. The right icon is the one Start shows: ask
// the shell for it through the app's AppUserModelID.

static bool GetAumidForWindow(HWND hwnd, std::wstring& out) {
    HWND target = hwnd;
    wchar_t cls[64] = {};
    GetClassNameW(hwnd, cls, 64);
    if (!wcscmp(cls, L"ApplicationFrameWindow")) {
        // The real app lives in the CoreWindow child, in its own process.
        target = FindWindowExW(hwnd, nullptr, L"Windows.UI.Core.CoreWindow", nullptr);
        if (!target) {
            return false;
        }
    }

    typedef LONG(WINAPI * PFN_GetApplicationUserModelId)(HANDLE, UINT32*, PWSTR);
    static PFN_GetApplicationUserModelId pGetAumid =
        reinterpret_cast<PFN_GetApplicationUserModelId>(
            GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "GetApplicationUserModelId"));
    if (!pGetAumid) {
        return false;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(target, &pid);
    HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!h) {
        return false;
    }
    wchar_t id[256] = {};
    UINT32 len = 256;
    const LONG rc = pGetAumid(h, &len, id);  // fails for unpackaged apps
    CloseHandle(h);
    if (rc != ERROR_SUCCESS || !id[0]) {
        return false;
    }
    out = id;
    return true;
}

// Icon of a packaged app from the shell, as premultiplied BGRA pixels.
static bool RenderShellAumidIcon(const std::wstring& aumid, int size,
                                 std::vector<uint32_t>& out) {
    typedef HRESULT(WINAPI * PFN_SHCreateItemFromParsingName)(PCWSTR, IBindCtx*, REFIID, void**);
    static PFN_SHCreateItemFromParsingName pCreate =
        reinterpret_cast<PFN_SHCreateItemFromParsingName>(
            GetProcAddress(LoadLibraryExW(L"shell32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32),
                           "SHCreateItemFromParsingName"));
    if (!pCreate) {
        return false;
    }

    const HRESULT initHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    bool ok = false;
    IShellItemImageFactory* factory = nullptr;
    const std::wstring path = L"shell:AppsFolder\\" + aumid;
    if (SUCCEEDED(pCreate(path.c_str(), nullptr, __uuidof(IShellItemImageFactory),
                          reinterpret_cast<void**>(&factory))) &&
        factory) {
        HBITMAP hbm = nullptr;
        SIZE want{ size, size };
        if (SUCCEEDED(factory->GetImage(want, SIIGBF_ICONONLY, &hbm)) && hbm) {
            BITMAP bm{};
            if (GetObjectW(hbm, sizeof(bm), &bm) && bm.bmBitsPixel == 32 &&
                bm.bmWidth > 0 && bm.bmHeight > 0) {
                const int w = bm.bmWidth;
                const int h = bm.bmHeight;
                std::vector<uint32_t> raw(static_cast<size_t>(w) * h);

                BITMAPINFO bi{};
                bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                bi.bmiHeader.biWidth = w;
                bi.bmiHeader.biHeight = -h;
                bi.bmiHeader.biPlanes = 1;
                bi.bmiHeader.biBitCount = 32;
                bi.bmiHeader.biCompression = BI_RGB;

                HDC dc = GetDC(nullptr);
                const int lines = GetDIBits(dc, hbm, 0, h, raw.data(), &bi, DIB_RGB_COLORS);
                ReleaseDC(nullptr, dc);

                if (lines == h) {
                    // Some bitmaps carry no alpha at all (all zero): then the
                    // image is simply opaque. Others are not premultiplied
                    // (a color channel above alpha): premultiply those.
                    bool anyAlpha = false;
                    bool needsPremul = false;
                    for (uint32_t px : raw) {
                        const uint32_t a = px >> 24;
                        if (a) anyAlpha = true;
                        if (((px >> 16) & 0xFF) > a || ((px >> 8) & 0xFF) > a ||
                            (px & 0xFF) > a) {
                            needsPremul = true;
                        }
                    }
                    for (uint32_t& px : raw) {
                        if (!anyAlpha) {
                            px |= 0xFF000000u;
                        } else if (needsPremul) {
                            const uint32_t a = px >> 24;
                            const uint32_t r = ((px >> 16) & 0xFF) * a / 255;
                            const uint32_t g = ((px >> 8) & 0xFF) * a / 255;
                            const uint32_t b = (px & 0xFF) * a / 255;
                            px = (a << 24) | (r << 16) | (g << 8) | b;
                        }
                    }

                    // Resample (nearest) if the shell didn't give the exact size.
                    out.assign(static_cast<size_t>(size) * size, 0);
                    for (int y = 0; y < size; y++) {
                        for (int x = 0; x < size; x++) {
                            out[static_cast<size_t>(y) * size + x] =
                                raw[static_cast<size_t>(y * h / size) * w + (x * w / size)];
                        }
                    }
                    ok = true;
                }
            }
            DeleteObject(hbm);
        }
        factory->Release();
    }

    if (SUCCEEDED(initHr)) {
        CoUninitialize();
    }
    return ok;
}

// Icon pixels per app. Extracting an icon is the slow part of the first
// gesture, so it can happen on a background thread at startup; everything here
// is plain Win32/GDI (no DirectComposition) and guarded by a lock.
using IconPixels = std::shared_ptr<std::vector<uint32_t>>;
static SRWLOCK g_iconPixelsLock = SRWLOCK_INIT;
static std::map<std::wstring, IconPixels> g_iconPixels;  // null value = known failure

struct SrwGuard {
    SRWLOCK* lock;
    explicit SrwGuard(SRWLOCK* l) : lock(l) { AcquireSRWLockExclusive(lock); }
    ~SrwGuard() { ReleaseSRWLockExclusive(lock); }
};

// The icon size is read once by the caller and passed down: the UI thread
// changes ICON_SIZE (UpdateUiScale) while the pre-warm thread may be running.
static std::wstring IconCacheKey(const std::wstring& key, int size) {
    return key + L"@" + std::to_wstring(size);
}

static bool LookupIconPixels(const std::wstring& key, int size, IconPixels& out) {
    const std::wstring sk = IconCacheKey(key, size);
    SrwGuard g(&g_iconPixelsLock);
    auto it = g_iconPixels.find(sk);
    if (it == g_iconPixels.end()) {
        return false;
    }
    out = it->second;
    return true;
}

static void StoreIconPixels(const std::wstring& key, int size, const IconPixels& px) {
    const std::wstring sk = IconCacheKey(key, size);
    SrwGuard g(&g_iconPixelsLock);
    g_iconPixels[sk] = px;
}

// Returns the app's icon (null if none) and the cache key it was stored under.
static IconPixels GetIconPixels(HWND hwnd, int size, std::wstring& keyOut) {
    // 1. Packaged / UWP apps: the icon the shell shows for them.
    std::wstring aumid;
    if (GetAumidForWindow(hwnd, aumid)) {
        const std::wstring key = L"aumid:" + aumid;
        IconPixels cached;
        if (LookupIconPixels(key, size, cached)) {
            if (cached) {
                keyOut = key;
                return cached;
            }
            // cached failure: fall through to the other sources
        } else {
            auto px = std::make_shared<std::vector<uint32_t>>();
            if (RenderShellAumidIcon(aumid, size, *px)) {
                StoreIconPixels(key, size, px);
                keyOut = key;
                return px;
            }
            Wh_Log(L"  icon: shell icon failed for %s", aumid.c_str());
            StoreIconPixels(key, size, nullptr);
        }
    }

    // 2. Classic apps: the icon from the executable, else the window's own.
    wchar_t cls[64] = {};
    GetClassNameW(hwnd, cls, 64);
    const bool uwpFrame = !wcscmp(cls, L"ApplicationFrameWindow");

    wchar_t path[2 * MAX_PATH] = {};
    std::wstring key;
    bool haveExe = false;
    if (!uwpFrame && GetWindowExePath(hwnd, path, 2 * MAX_PATH)) {
        key = path;
        haveExe = true;
    } else {
        key = L"hwnd:" + std::to_wstring(static_cast<unsigned long long>(
                             reinterpret_cast<uintptr_t>(hwnd)));
    }
    keyOut = key;

    IconPixels cached;
    if (LookupIconPixels(key, size, cached)) {
        return cached;
    }

    // user32's PrivateExtractIconsW can pull an icon of an exact size out of
    // an executable (sharper than the 32 px icon most windows advertise). It
    // is loaded by name so the build doesn't depend on a header declaring it.
    typedef UINT(WINAPI * PFN_PrivateExtractIconsW)(LPCWSTR, int, int, int, HICON*,
                                                     UINT*, UINT, UINT);
    static PFN_PrivateExtractIconsW pExtract =
        reinterpret_cast<PFN_PrivateExtractIconsW>(
            GetProcAddress(GetModuleHandleW(L"user32.dll"), "PrivateExtractIconsW"));

    HICON icon = nullptr;
    bool owned = false;
    if (haveExe && pExtract) {
        HICON h = nullptr;
        if (pExtract(path, 0, size, size, &h, nullptr, 1, 0) == 1 && h) {
            icon = h;
            owned = true;
        }
    }
    if (!icon) {
        icon = GetWindowIconHandle(hwnd);
    }

    auto px = std::make_shared<std::vector<uint32_t>>();
    const bool ok = icon && RenderIconPixels(icon, size, *px);
    if (owned) {
        DestroyIcon(icon);
    }
    if (!ok) {
        Wh_Log(L"  icon: could not render the icon for %s", key.c_str());
        px = nullptr;
    }
    StoreIconPixels(key, size, px);
    return px;
}

// The DirectComposition surface for an app's icon (UI thread only).
static IDCompositionSurface* GetIconSurface(HWND hwnd) {
    const int size = ICON_SIZE;
    std::wstring key;
    IconPixels px = GetIconPixels(hwnd, size, key);
    if (!px || px->size() != static_cast<size_t>(size) * static_cast<size_t>(size)) {
        return nullptr;
    }

    const std::wstring sk = IconCacheKey(key, size);
    auto it = g_iconCache.find(sk);
    if (it != g_iconCache.end()) {
        return it->second;
    }

    IDCompositionSurface* surf = CreatePixelSurface(*px, size, size);
    g_iconCache[sk] = surf;  // cache failures too
    return surf;
}

// Icons keyed by window handle (no exe path / AUMID) are dropped once their
// window is gone, so both caches don't keep growing.
static bool IsDeadWindowKey(const std::wstring& sk) {
    if (sk.compare(0, 5, L"hwnd:") != 0) {
        return false;
    }
    const auto h = reinterpret_cast<HWND>(
        static_cast<uintptr_t>(std::wcstoull(sk.c_str() + 5, nullptr, 10)));
    return !IsWindow(h);
}

// UI thread only (it owns g_iconCache).
static void PurgeDeadWindowIcons() {
    for (auto it = g_iconCache.begin(); it != g_iconCache.end();) {
        if (IsDeadWindowKey(it->first)) {
            if (it->second) {
                it->second->Release();
            }
            it = g_iconCache.erase(it);
        } else {
            ++it;
        }
    }
    SrwGuard g(&g_iconPixelsLock);
    for (auto it = g_iconPixels.begin(); it != g_iconPixels.end();) {
        it = IsDeadWindowKey(it->first) ? g_iconPixels.erase(it) : std::next(it);
    }
}

// ---- Start-up pre-warm --------------------------------------------------------

static HANDLE g_prewarmThread = nullptr;
static volatile LONG g_prewarmStop = 0;

static BOOL CALLBACK PrewarmEnumProc(HWND h, LPARAM lp) {
    if (g_prewarmStop) {
        return FALSE;
    }
    if (!IsWindowVisible(h)) {
        return TRUE;
    }
    const LONG_PTR ex = GetWindowLongPtrW(h, GWL_EXSTYLE);
    if ((ex & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) && !(ex & WS_EX_APPWINDOW)) {
        return TRUE;
    }
    if (!HasWindowTitle(h)) {
        return TRUE;
    }
    HWND owner = GetWindow(h, GW_OWNER);
    if (owner && IsWindowVisible(owner)) {
        return TRUE;
    }
    wchar_t cls[96] = {};
    GetClassNameW(h, cls, 96);
    if (IsDeniedClass(cls)) {
        return TRUE;
    }

    std::wstring key;
    GetIconPixels(h, static_cast<int>(lp), key);
    return TRUE;
}

static DWORD WINAPI PrewarmThreadProc(void* param) {
    const HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
    EnumWindows(PrewarmEnumProc, reinterpret_cast<LPARAM>(param));  // param = icon size
    if (SUCCEEDED(hr)) {
        CoUninitialize();
    }
    Wh_Log(L"Icon pre-warm finished.");
    return 0;
}

static void StartPrewarm() {
    if (!PREWARM || g_prewarmThread) {
        return;
    }
    g_prewarmStop = 0;
    g_prewarmThread = CreateThread(nullptr, 0, PrewarmThreadProc,
                                   reinterpret_cast<void*>(static_cast<INT_PTR>(ICON_SIZE)), 0,
                                   nullptr);
}

static void StopPrewarm() {
    if (!g_prewarmThread) {
        return;
    }
    // It checks the stop flag between windows, so this wait is short; it must
    // end before the mod is unloaded, so there is no timeout.
    InterlockedExchange(&g_prewarmStop, 1);
    WaitForSingleObject(g_prewarmThread, INFINITE);
    CloseHandle(g_prewarmThread);
    g_prewarmThread = nullptr;
}

// Creates the icon visual for a tile (invisible until the swipe fades it in).
// 2D transform [m11 m12; m21 m22; dx dy], built without depending on how the
// header lays out the struct's members (avoids brace-initialization warnings).
static D2D_MATRIX_3X2_F MakeMatrix(float m11, float m12, float m21, float m22,
                                   float dx, float dy) {
    static_assert(sizeof(D2D_MATRIX_3X2_F) == 6 * sizeof(float),
                  "unexpected D2D_MATRIX_3X2_F layout");
    const float v[6] = { m11, m12, m21, m22, dx, dy };
    D2D_MATRIX_3X2_F m;
    memcpy(&m, v, sizeof(m));
    return m;
}

// The window's title, as a child of the icon visual (so it moves, scales and
// fades together with it): centered under the icon, cut with an ellipsis if it
// is wider than the window's tile.
static void AddTitleChild(IDCompositionVisual2* parent, const Tile& t) {
    wchar_t title[256] = {};
    if (InternalGetWindowText(t.hwnd, title, 255) <= 0) {
        return;
    }

    const int tileW = t.to.right - t.to.left;
    const int maxW = (tileW > 120 ? tileW : 120) + 2 * TITLE_PAD;
    const COLORREF color = (g_darkTheme || g_wallpaperOk) ? RGB(238, 238, 238) : RGB(28, 28, 28);

    std::vector<uint32_t> px;
    int w = 0, h = 0;
    if (!RenderTextPixels(title, color, TITLE_FONT_PX, L"Segoe UI", false, px, w, h, maxW)) {
        return;
    }
    IDCompositionSurface* surf = CreatePixelSurface(px, w, h);
    if (!surf) {
        return;
    }

    IDCompositionVisual2* tv = nullptr;
    if (FAILED(g_dcDevice->CreateVisual(&tv)) || !tv) {
        surf->Release();
        return;
    }
    tv->SetContent(surf);
    surf->Release();  // the visual holds its own reference
    tv->SetBitmapInterpolationMode(DCOMPOSITION_BITMAP_INTERPOLATION_MODE_LINEAR);

    // Relative to the container (origin at the icon's top-left corner).
    const float x = (static_cast<float>(ICON_SIZE) - static_cast<float>(w)) * 0.5f;
    const float y = static_cast<float>((SHOW_ICONS ? ICON_SIZE + TITLE_GAP : 0) - TITLE_PAD);
    tv->SetTransform(MakeMatrix(1.0f, 0.0f, 0.0f, 1.0f, x, y));

    if (FAILED(parent->AddVisual(tv, TRUE, nullptr))) {
        tv->Release();
        return;
    }
    tv->Release();  // the parent holds it now
}

// Creates what goes under a tile: the app icon and the window title (either can
// be off). It starts invisible; its opacity follows the swipe.
static void CreateDCompIcon(Tile& t) {
    if ((!SHOW_ICONS && !SHOW_TITLES) || !g_dcOk || !t.dcVisual) {
        return;
    }

    IDCompositionSurface* surf = SHOW_ICONS ? GetIconSurface(t.hwnd) : nullptr;
    if (!surf && !SHOW_TITLES) {
        return;
    }

    IDCompositionVisual2* v = nullptr;
    IDCompositionEffectGroup* fx = nullptr;
    if (FAILED(g_dcDevice->CreateVisual(&v)) || !v) {
        return;
    }
    if (FAILED(g_dcDevice->CreateEffectGroup(&fx)) || !fx) {
        v->Release();
        return;
    }

    if (surf) {
        v->SetContent(surf);
        v->SetBitmapInterpolationMode(DCOMPOSITION_BITMAP_INTERPOLATION_MODE_LINEAR);
    }
    fx->SetOpacity(0.0f);
    v->SetEffect(fx);

    if (SHOW_TITLES) {
        AddTitleChild(v, t);
    }

    if (FAILED(g_dcIcons->AddVisual(v, TRUE, g_dcIconTop))) {
        fx->Release();
        v->Release();
        return;
    }
    g_dcIconTop = v;

    t.iconVisual = v;
    t.iconFx = fx;
    t.iconPlaced = false;
    t.iconOp = 0.0f;
}

// "No apps opened" as a visual whose opacity follows the swipe (text only).
static void CreateDCompEmptyMessage() {
    if (!g_dcOk || g_emptyVisual) {
        return;
    }

    std::vector<uint32_t> px;
    int w = 0, h = 0;
    const COLORREF color = (g_darkTheme || g_wallpaperOk) ? RGB(235, 235, 235)
                                                           : RGB(30, 30, 30);
    const EmptyText et = GetEmptyMessage();
    if (!RenderTextPixels(et.text, color, static_cast<int>(std::lround(32.0f * g_uiScale)), et.font,
                          et.rtl, px, w, h)) {
        return;
    }

    IDCompositionSurface* surf = CreatePixelSurface(px, w, h);
    if (!surf) {
        return;
    }

    IDCompositionVisual2* v = nullptr;
    IDCompositionEffectGroup* fx = nullptr;
    if (FAILED(g_dcDevice->CreateVisual(&v)) || !v ||
        FAILED(g_dcDevice->CreateEffectGroup(&fx)) || !fx) {
        if (v) v->Release();
        surf->Release();
        return;
    }

    v->SetContent(surf);
    v->SetBitmapInterpolationMode(DCOMPOSITION_BITMAP_INTERPOLATION_MODE_LINEAR);
    fx->SetOpacity(0.0f);
    v->SetEffect(fx);

    if (FAILED(g_dcIcons->AddVisual(v, TRUE, g_dcIconTop))) {
        fx->Release();
        v->Release();
        surf->Release();
        return;
    }

    g_emptyVisual = v;
    g_emptyFx = fx;
    g_emptySurface = surf;
    g_emptyW = w;
    g_emptyH = h;
    g_emptyPlaced = false;
    g_emptyOp = 0.0f;
}

// Moves a tile's visual to `r` (overlay client coords). Does not Commit().
static void PlaceDCompTile(Tile& t, const RECT& r) {
    const D2D_MATRIX_3X2_F m = MakeMatrix(
        static_cast<float>(r.right - r.left) / t.srcW, 0.0f,
        0.0f, static_cast<float>(r.bottom - r.top) / t.srcH,
        static_cast<float>(r.left), static_cast<float>(r.top));
    t.dcVisual->SetTransform(m);
}

// Dialogs ride along with their owner's tile: same scale, same relative spot.
static void PlaceKids(Tile& t, const RECT& r) {
    const float fw = static_cast<float>(t.from.right - t.from.left);
    const float fh = static_cast<float>(t.from.bottom - t.from.top);
    if (fw <= 0.0f || fh <= 0.0f) {
        return;
    }
    const float sx = static_cast<float>(r.right - r.left) / fw;
    const float sy = static_cast<float>(r.bottom - r.top) / fh;

    for (auto& k : t.kids) {
        if (!k.dcVisual) {
            continue;
        }
        RECT kr;
        kr.left = r.left + std::lround(static_cast<float>(k.from.left - t.from.left) * sx);
        kr.top = r.top + std::lround(static_cast<float>(k.from.top - t.from.top) * sy);
        kr.right = r.left + std::lround(static_cast<float>(k.from.right - t.from.left) * sx);
        kr.bottom = r.top + std::lround(static_cast<float>(k.from.bottom - t.from.top) * sy);
        if (kr.right <= kr.left) kr.right = kr.left + 1;
        if (kr.bottom <= kr.top) kr.bottom = kr.top + 1;
        PlaceDCompTile(k, kr);
        k.last = kr;
        k.placed = true;
    }
}

// Centers the icon under the tile's current rect. Does not Commit().
static void PlaceDCompIcon(Tile& t, const RECT& r) {
    const float x = (static_cast<float>(r.left) + static_cast<float>(r.right)) * 0.5f -
                    static_cast<float>(ICON_SIZE) * 0.5f;
    const float y = static_cast<float>(r.bottom + ICON_GAP);
    const D2D_MATRIX_3X2_F m = MakeMatrix(1.0f, 0.0f, 0.0f, 1.0f, x, y);
    t.iconVisual->SetTransform(m);
}



// ---------------------------------------------------------------------------
// Virtual desktops bar
//
// A strip flush with the bottom edge with one card per virtual desktop (a
// miniature of where its windows are, and its name) and a "+" card. The desktops
// come from the registry (the shell keeps them there), the windows of each one
// from IVirtualDesktopManager (documented). Switching / creating is done with the
// keyboard shortcuts Windows already has (Win+Ctrl+arrows / Win+Ctrl+D).
// ---------------------------------------------------------------------------

// "Desktop" in the language Windows is displayed in (the registry only holds a
// name once the user renamed the desktop).
static const wchar_t* DesktopWord() {
    switch (PRIMARYLANGID(GetUserDefaultUILanguage())) {
        case LANG_SPANISH:    return L"Escritorio";
        case LANG_PORTUGUESE: return L"Área de trabalho";
        case LANG_FRENCH:     return L"Bureau";
        case LANG_DUTCH:      return L"Bureaublad";
        case LANG_POLISH:     return L"Pulpit";
        case LANG_RUSSIAN:    return L"Рабочий стол";
        case LANG_UKRAINIAN:  return L"Робочий стіл";
        case LANG_TURKISH:    return L"Masaüstü";
        case LANG_JAPANESE:   return L"デスクトップ";
        case LANG_KOREAN:     return L"데스크톱";
        case LANG_CHINESE:    return L"桌面";
        default:              return L"Desktop";
    }
}

static void ReadDesktopIds(std::vector<GUID>& ids, GUID& current, bool& haveCurrent) {
    ids.clear();
    haveCurrent = false;
    HKEY key = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\VirtualDesktops",
                      0, KEY_READ, &key) != ERROR_SUCCESS) {
        return;
    }

    BYTE buf[16 * 64] = {};
    DWORD size = sizeof(buf);
    DWORD type = 0;
    if (RegQueryValueExW(key, L"VirtualDesktopIDs", nullptr, &type, buf, &size) == ERROR_SUCCESS &&
        type == REG_BINARY) {
        for (DWORD o = 0; o + 16 <= size; o += 16) {
            GUID g;
            memcpy(&g, buf + o, 16);
            ids.push_back(g);
        }
    }

    BYTE cur[16] = {};
    size = sizeof(cur);
    type = 0;
    if (RegQueryValueExW(key, L"CurrentVirtualDesktop", nullptr, &type, cur, &size) == ERROR_SUCCESS &&
        type == REG_BINARY && size == 16) {
        memcpy(&current, cur, 16);
        haveCurrent = true;
    }
    RegCloseKey(key);
}

static std::wstring ReadDesktopName(const GUID& id) {
    wchar_t guid[64] = {};
    if (!StringFromGUID2(id, guid, 64)) {
        return L"";
    }
    std::wstring path =
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\VirtualDesktops\\Desktops\\";
    path += guid;

    wchar_t name[256] = {};
    DWORD size = sizeof(name);
    if (RegGetValueW(HKEY_CURRENT_USER, path.c_str(), L"Name", RRF_RT_REG_SZ, nullptr, name,
                     &size) == ERROR_SUCCESS) {
        return name;
    }
    return L"";
}

static std::wstring GetWallpaperPath();

// The wallpaper of a desktop. Windows 11 can give each virtual desktop its own
// one (kept next to its name in the registry); otherwise, and for the current
// desktop (slideshows, Spotlight), it is the one Windows reports right now.
static std::wstring DesktopWallpaperPath(const GUID& id, bool current) {
    if (!current) {
        wchar_t guid[64] = {};
        if (StringFromGUID2(id, guid, 64)) {
            std::wstring key =
                L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\VirtualDesktops\\Desktops\\";
            key += guid;
            wchar_t path[MAX_PATH] = {};
            DWORD size = sizeof(path);
            if (RegGetValueW(HKEY_CURRENT_USER, key.c_str(), L"Wallpaper", RRF_RT_REG_SZ, nullptr,
                             path, &size) == ERROR_SUCCESS &&
                path[0] && GetFileAttributesW(path) != INVALID_FILE_ATTRIBUTES) {
                return path;
            }
        }
    }
    return GetWallpaperPath();
}

// Wallpapers shrunk to the size of a card's little screen, as opaque BGRA.
// Decoding a full-size wallpaper is slow, so they are kept between gestures
// (key: path, last write time and size).
static std::map<std::wstring, std::vector<uint32_t>> g_wallThumbs;

static const std::vector<uint32_t>* GetWallpaperThumb(const std::wstring& path, int w, int h) {
    if (path.empty() || w <= 0 || h <= 0) {
        return nullptr;
    }
    WIN32_FILE_ATTRIBUTE_DATA fa{};
    GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &fa);
    wchar_t tail[96];
    swprintf(tail, 96, L"|%lu|%lu|%d|%d",
             static_cast<unsigned long>(fa.ftLastWriteTime.dwHighDateTime),
             static_cast<unsigned long>(fa.ftLastWriteTime.dwLowDateTime), w, h);
    const std::wstring key = path + tail;

    auto it = g_wallThumbs.find(key);
    if (it != g_wallThumbs.end()) {
        return it->second.empty() ? nullptr : &it->second;  // empty = known failure
    }
    if (g_wallThumbs.size() > 16) {
        g_wallThumbs.clear();  // wallpapers or monitors changed a lot: start over
    }

    std::vector<uint32_t> px;
    try {
        winrt::com_ptr<IWICImagingFactory> factory;
        winrt::check_hresult(CoCreateInstance(CLSID_WICImagingFactory, nullptr,
                                              CLSCTX_INPROC_SERVER, IID_IWICImagingFactory,
                                              factory.put_void()));
        winrt::com_ptr<IWICBitmapDecoder> decoder;
        winrt::check_hresult(factory->CreateDecoderFromFilename(
            path.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, decoder.put()));
        winrt::com_ptr<IWICBitmapFrameDecode> frame;
        winrt::check_hresult(decoder->GetFrame(0, frame.put()));

        UINT sw = 0, sh = 0;
        winrt::check_hresult(frame->GetSize(&sw, &sh));
        if (sw && sh) {
            // "Fill": cover the little screen and crop the center.
            UINT cw = sw, ch = sh;
            const double srcAspect = static_cast<double>(sw) / sh;
            const double dstAspect = static_cast<double>(w) / h;
            if (srcAspect > dstAspect) {
                cw = static_cast<UINT>(sh * dstAspect + 0.5);
            } else {
                ch = static_cast<UINT>(sw / dstAspect + 0.5);
            }
            cw = cw ? cw : 1;
            ch = ch ? ch : 1;
            WICRect clip{ static_cast<INT>((sw - cw) / 2), static_cast<INT>((sh - ch) / 2),
                          static_cast<INT>(cw), static_cast<INT>(ch) };

            winrt::com_ptr<IWICBitmapClipper> clipper;
            winrt::check_hresult(factory->CreateBitmapClipper(clipper.put()));
            winrt::check_hresult(clipper->Initialize(frame.get(), &clip));
            winrt::com_ptr<IWICBitmapScaler> scaler;
            winrt::check_hresult(factory->CreateBitmapScaler(scaler.put()));
            winrt::check_hresult(scaler->Initialize(clipper.get(), static_cast<UINT>(w),
                                                    static_cast<UINT>(h),
                                                    WICBitmapInterpolationModeFant));
            winrt::com_ptr<IWICFormatConverter> conv;
            winrt::check_hresult(factory->CreateFormatConverter(conv.put()));
            winrt::check_hresult(conv->Initialize(scaler.get(), GUID_WICPixelFormat32bppBGRA,
                                                  WICBitmapDitherTypeNone, nullptr, 0.0,
                                                  WICBitmapPaletteTypeCustom));
            px.resize(static_cast<size_t>(w) * h);
            winrt::check_hresult(conv->CopyPixels(nullptr, static_cast<UINT>(w) * 4,
                                                  static_cast<UINT>(px.size() * 4),
                                                  reinterpret_cast<BYTE*>(px.data())));
            for (uint32_t& p : px) {
                p |= 0xFF000000u;  // a wallpaper is opaque
            }
        }
    } catch (...) {
        px.clear();
        Wh_Log(L"Desktop bar: could not load the wallpaper %s", path.c_str());
    }

    auto& slot = g_wallThumbs[key];
    slot = std::move(px);
    return slot.empty() ? nullptr : &slot;
}

struct DesktopWin {
    HWND hwnd;
    RECT rect;
    bool cloaked;
};

// App windows of every desktop (windows of other desktops are cloaked by the shell).
static BOOL CALLBACK DesktopWinProc(HWND hwnd, LPARAM lp) {
    auto* out = reinterpret_cast<std::vector<DesktopWin>*>(lp);
    if (hwnd == g_overlay || hwnd == g_highlight || hwnd == g_thumbHost ||
        !IsWindowVisible(hwnd) || IsIconic(hwnd)) {
        return TRUE;
    }

    DWORD cloaked = 0;
    DwmGetWindowAttribute(hwnd, kDwmwaCloaked, &cloaked, sizeof(cloaked));
    if (cloaked && !(cloaked & 0x2 /*DWM_CLOAKED_SHELL*/)) {
        return TRUE;
    }

    const LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if ((ex & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) && !(ex & WS_EX_APPWINDOW)) {
        return TRUE;
    }
    if (IsInvisibleLayered(hwnd, ex) || !HasWindowTitle(hwnd)) {
        return TRUE;
    }
    HWND owner = GetWindow(hwnd, GW_OWNER);
    if (owner && IsWindowVisible(owner)) {
        return TRUE;
    }
    wchar_t cls[96] = {};
    GetClassNameW(hwnd, cls, 96);
    if (IsDeniedClass(cls)) {
        return TRUE;
    }
    RECT r{};
    if (!GetWindowRect(hwnd, &r) || r.right - r.left < 100 || r.bottom - r.top < 60) {
        return TRUE;
    }

    out->push_back({ hwnd, r, cloaked != 0 });
    return TRUE;
}

// Reads the desktops and decides where the cards go. Runs before the grid is laid
// out, because the bar takes its space from the bottom of the screen.
static void PrepareDesktopCards() {
    g_cards.clear();
    g_barBand = 0;
    if (!DESKTOP_BAR || !g_dcOk || !g_dcBarLayer || !g_barFx) {
        return;
    }

    const int W = g_workRect.right - g_workRect.left;
    const int H = g_workRect.bottom - g_workRect.top;
    if (W < 600 || H < 500) {
        return;  // too small for a bar
    }

    std::vector<GUID> ids;
    GUID cur{};
    bool haveCur = false;
    ReadDesktopIds(ids, cur, haveCur);
    if (ids.empty()) {
        ids.push_back(cur);  // a single desktop: the shell hasn't written the list yet
    }

    int curIdx = 0;
    for (size_t i = 0; i < ids.size(); i++) {
        if (haveCur && IsEqualGUID(ids[i], cur)) {
            curIdx = static_cast<int>(i);
        }
    }

    for (size_t i = 0; i < ids.size(); i++) {
        DesktopCard c;
        c.id = ids[i];
        c.name = ReadDesktopName(ids[i]);
        if (c.name.empty()) {
            c.name = std::wstring(DesktopWord()) + L" " + std::to_wstring(i + 1);
        }
        c.current = static_cast<int>(i) == curIdx;
        c.w = BAR_CARD_W;
        g_cards.push_back(c);
    }

    // Where the windows of every desktop are, for the miniatures.
    GetVdm();
    std::vector<DesktopWin> wins;
    EnumWindows(DesktopWinProc, reinterpret_cast<LPARAM>(&wins));
    for (const auto& w : wins) {
        RECT mr = w.rect;
        OffsetRect(&mr, -g_workRect.left, -g_workRect.top);
        const RECT clip{ 0, 0, W, H };
        RECT vis{};
        if (!IntersectRect(&vis, &mr, &clip)) {
            continue;
        }

        int target = -1;
        if (g_vdm) {
            GUID wid{};
            if (SUCCEEDED(g_vdm->GetWindowDesktopId(w.hwnd, &wid))) {
                for (size_t i = 0; i < g_cards.size(); i++) {
                    if (IsEqualGUID(g_cards[i].id, wid)) {
                        target = static_cast<int>(i);
                    }
                }
                if (target < 0 && g_cards.size() == 1) {
                    target = 0;
                }
            }
        } else if (!w.cloaked) {
            target = curIdx;
        }
        if (target >= 0 && g_cards[static_cast<size_t>(target)].wins.size() < BAR_MAX_THUMBS) {
            g_cards[static_cast<size_t>(target)].wins.push_back({ w.hwnd, mr });
        }
    }

    DesktopCard add;
    add.isAdd = true;
    add.w = BAR_ADD_W;
    g_cards.push_back(add);

    // A centered row of cards.
    int total = BAR_GAP * (static_cast<int>(g_cards.size()) - 1);
    for (const auto& c : g_cards) {
        total += c.w;
    }
    int x = (W - total) / 2;
    if (x < 8) {
        x = 8;
    }
    for (auto& c : g_cards) {
        c.x = x;
        x += c.w + BAR_GAP;
    }

    g_barBand = BAR_H;
    Wh_Log(L"Desktop bar: %d desktop(s), current #%d, %d window(s) found",
           static_cast<int>(g_cards.size()) - 1, curIdx + 1, static_cast<int>(wins.size()));
    for (size_t i = 0; i + 1 < g_cards.size(); i++) {
        Wh_Log(L"Desktop bar:   desktop %d has %d window(s)", static_cast<int>(i) + 1,
               static_cast<int>(g_cards[i].wins.size()));
    }
}

// ---- Drawing (CPU, premultiplied BGRA) ----

static void PutOver(std::vector<uint32_t>& px, int w, int h, int x, int y,
                    float r, float g, float b, float a) {
    if (x < 0 || y < 0 || x >= w || y >= h || a <= 0.0f) {
        return;
    }
    uint32_t& d = px[static_cast<size_t>(y) * w + x];
    const float da = static_cast<float>((d >> 24) & 0xFF) / 255.0f;
    const float dr = static_cast<float>((d >> 16) & 0xFF) / 255.0f;
    const float dg = static_cast<float>((d >> 8) & 0xFF) / 255.0f;
    const float db = static_cast<float>(d & 0xFF) / 255.0f;
    const float k = 1.0f - a;
    const float oa = a + da * k;
    const float orr = r * a + dr * k;
    const float og = g * a + dg * k;
    const float ob = b * a + db * k;
    d = (static_cast<uint32_t>(oa * 255.0f + 0.5f) << 24) |
        (static_cast<uint32_t>(orr * 255.0f + 0.5f) << 16) |
        (static_cast<uint32_t>(og * 255.0f + 0.5f) << 8) |
        static_cast<uint32_t>(ob * 255.0f + 0.5f);
}

// Anti-aliased coverage of a rounded rectangle at a point.
static float RoundRectCoverage(float x, float y, float x0, float y0, float x1, float y1, float rad) {
    const float cx = (x0 + x1) * 0.5f;
    const float cy = (y0 + y1) * 0.5f;
    const float hx = (x1 - x0) * 0.5f - rad;
    const float hy = (y1 - y0) * 0.5f - rad;
    const float qx = std::fabs(x - cx) - hx;
    const float qy = std::fabs(y - cy) - hy;
    const float ox = qx > 0.0f ? qx : 0.0f;
    const float oy = qy > 0.0f ? qy : 0.0f;
    const float inner = (qx > qy ? qx : qy);
    const float d = std::sqrt(ox * ox + oy * oy) + (inner < 0.0f ? inner : 0.0f) - rad;
    return Clamp01(0.5f - d);
}

static void FillRoundRect(std::vector<uint32_t>& px, int w, int h, float x0, float y0, float x1,
                          float y1, float rad, float r, float g, float b, float a) {
    const int ix0 = static_cast<int>(std::floor(x0)) - 1;
    const int iy0 = static_cast<int>(std::floor(y0)) - 1;
    const int ix1 = static_cast<int>(std::ceil(x1)) + 1;
    const int iy1 = static_cast<int>(std::ceil(y1)) + 1;
    for (int y = iy0; y <= iy1; y++) {
        for (int x = ix0; x <= ix1; x++) {
            const float c = RoundRectCoverage(x + 0.5f, y + 0.5f, x0, y0, x1, y1, rad);
            if (c > 0.0f) {
                PutOver(px, w, h, x, y, r, g, b, a * c);
            }
        }
    }
}

static void StrokeRoundRect(std::vector<uint32_t>& px, int w, int h, float x0, float y0, float x1,
                            float y1, float rad, float thick, float r, float g, float b, float a) {
    const int ix0 = static_cast<int>(std::floor(x0)) - 1;
    const int iy0 = static_cast<int>(std::floor(y0)) - 1;
    const int ix1 = static_cast<int>(std::ceil(x1)) + 1;
    const int iy1 = static_cast<int>(std::ceil(y1)) + 1;
    for (int y = iy0; y <= iy1; y++) {
        for (int x = ix0; x <= ix1; x++) {
            const float outer = RoundRectCoverage(x + 0.5f, y + 0.5f, x0, y0, x1, y1, rad);
            const float inner = RoundRectCoverage(x + 0.5f, y + 0.5f, x0 + thick, y0 + thick,
                                                  x1 - thick, y1 - thick, rad - thick);
            const float c = outer - inner;
            if (c > 0.0f) {
                PutOver(px, w, h, x, y, r, g, b, a * c);
            }
        }
    }
}

// Premultiplied pixels (e.g. rendered text) over the destination.
static void BlendPremul(std::vector<uint32_t>& px, int w, int h, const std::vector<uint32_t>& src,
                        int sw, int sh, int dx, int dy) {
    for (int y = 0; y < sh; y++) {
        for (int x = 0; x < sw; x++) {
            const int tx = x + dx;
            const int ty = y + dy;
            if (tx < 0 || ty < 0 || tx >= w || ty >= h) {
                continue;
            }
            const uint32_t s = src[static_cast<size_t>(y) * sw + x];
            const uint32_t sa = s >> 24;
            if (!sa) {
                continue;
            }
            uint32_t& d = px[static_cast<size_t>(ty) * w + tx];
            const uint32_t k = 255 - sa;
            const uint32_t oa = sa + ((d >> 24) & 0xFF) * k / 255;
            const uint32_t orr = ((s >> 16) & 0xFF) + ((d >> 16) & 0xFF) * k / 255;
            const uint32_t og = ((s >> 8) & 0xFF) + ((d >> 8) & 0xFF) * k / 255;
            const uint32_t ob = (s & 0xFF) + (d & 0xFF) * k / 255;
            d = ((oa > 255 ? 255 : oa) << 24) | ((orr > 255 ? 255 : orr) << 16) |
                ((og > 255 ? 255 : og) << 8) | (ob > 255 ? 255 : ob);
        }
    }
}

static void RenderDesktopCard(const DesktopCard& c, std::vector<uint32_t>& px) {
    const int w = c.w;
    const int h = BAR_CARD_H;
    px.assign(static_cast<size_t>(w) * h, 0);

    const float k = static_cast<float>(BAR_CARD_H) / 76.0f;  // the design is drawn for 76 px
    const bool dark = g_darkTheme || g_wallpaperOk;
    const float tone = dark ? 1.0f : 0.0f;  // white on dark themes, black on light ones
    const float corner = 12.0f * k;

    FillRoundRect(px, w, h, 0.5f, 0.5f, w - 0.5f, h - 0.5f, corner, tone, tone, tone,
                  dark ? 0.12f : 0.08f);

    if (c.isAdd) {
        const float cx = w * 0.5f;
        const float cy = h * 0.5f;
        const float arm = 12.0f * k;
        const float thick = 1.6f * k;
        FillRoundRect(px, w, h, cx - arm, cy - thick, cx + arm, cy + thick, thick, tone, tone, tone, 0.85f);
        FillRoundRect(px, w, h, cx - thick, cy - arm, cx + thick, cy + arm, thick, tone, tone, tone, 0.85f);
        return;
    }

    // The little screen: where this desktop's windows are.
    const float wx0 = static_cast<float>(w - BAR_WELL_W) * 0.5f;
    const float wy0 = static_cast<float>(BAR_WELL_Y);
    const float wx1 = wx0 + static_cast<float>(BAR_WELL_W);
    const float wy1 = wy0 + static_cast<float>(BAR_WELL_H);
    const float wellRad = 6.0f * k;
    const std::vector<uint32_t>* wall = GetWallpaperThumb(
        DesktopWallpaperPath(c.id, c.current), BAR_WELL_W, BAR_WELL_H);
    if (wall) {
        // The desktop's wallpaper, with the screen's rounded corners.
        for (int y = 0; y < BAR_WELL_H; y++) {
            const int ty = BAR_WELL_Y + y;
            for (int x = 0; x < BAR_WELL_W; x++) {
                const int tx = static_cast<int>(std::floor(wx0)) + x;
                const float cov = RoundRectCoverage(tx + 0.5f, ty + 0.5f, wx0, wy0, wx1, wy1, wellRad);
                if (cov <= 0.0f) {
                    continue;
                }
                const uint32_t s = (*wall)[static_cast<size_t>(y) * BAR_WELL_W + x];
                PutOver(px, w, h, tx, ty, static_cast<float>((s >> 16) & 0xFF) / 255.0f,
                        static_cast<float>((s >> 8) & 0xFF) / 255.0f,
                        static_cast<float>(s & 0xFF) / 255.0f, cov);
            }
        }
    } else {
        FillRoundRect(px, w, h, wx0, wy0, wx1, wy1, wellRad, 1.0f - tone, 1.0f - tone, 1.0f - tone,
                      dark ? 0.30f : 0.55f);
    }

    const float monW = static_cast<float>(g_workRect.right - g_workRect.left);
    const float monH = static_cast<float>(g_workRect.bottom - g_workRect.top);
    for (const CardWin& cw : c.wins) {
        // (a wireframe underneath the live thumbnail: still there if DWM can't give one)
        const float rl = std::max(0.0f, static_cast<float>(cw.rect.left));
        const float rt = std::max(0.0f, static_cast<float>(cw.rect.top));
        const float rr = std::min(monW, static_cast<float>(cw.rect.right));
        const float rb = std::min(monH, static_cast<float>(cw.rect.bottom));
        const float x0 = wx0 + rl / monW * static_cast<float>(BAR_WELL_W);
        const float y0 = wy0 + rt / monH * static_cast<float>(BAR_WELL_H);
        const float x1 = wx0 + rr / monW * static_cast<float>(BAR_WELL_W);
        const float y1 = wy0 + rb / monH * static_cast<float>(BAR_WELL_H);
        if (x1 - x0 < 2.0f || y1 - y0 < 2.0f) {
            continue;
        }
        FillRoundRect(px, w, h, x0, y0, x1, y1, 1.5f * k, tone, tone, tone, dark ? 0.62f : 0.40f);
    }

    // Name, just under the little screen.
    std::vector<uint32_t> text;
    int tw = 0, th = 0;
    const COLORREF color = dark ? RGB(238, 238, 238) : RGB(28, 28, 28);
    const int fontPx = static_cast<int>(std::max(std::lround(9.0f * g_dpiScale), std::lround(11.0f * k)));
    if (RenderTextPixels(c.name.c_str(), color, fontPx, L"Segoe UI", false, text, tw, th, w - 4)) {
        BlendPremul(px, w, h, text, tw, th, (w - tw) / 2,
                    static_cast<int>(std::lround(wy1 + 2.0f * k)) - TITLE_PAD);
    }

    if (c.current) {
        StrokeRoundRect(px, w, h, 0.5f, 0.5f, w - 0.5f, h - 0.5f, corner, 2.0f * k,
                        g_accR / 255.0f, g_accG / 255.0f, g_accB / 255.0f, 1.0f);
    }
}

static void RenderBarStrip(std::vector<uint32_t>& px, int W) {
    px.assign(static_cast<size_t>(W) * BAR_H, 0);
    const bool dark = g_darkTheme || g_wallpaperOk;
    const float tone = dark ? 1.0f : 0.0f;
    for (int y = 0; y < BAR_H; y++) {
        // A faint glass layer over the backdrop, with a hairline on top.
        float a = (dark ? 0.085f : 0.06f) * (0.75f + 0.25f * static_cast<float>(y) / BAR_H);
        if (y == 0) {
            a = dark ? 0.18f : 0.14f;
        }
        const uint32_t ua = static_cast<uint32_t>(a * 255.0f + 0.5f);
        const uint32_t uc = static_cast<uint32_t>(tone * a * 255.0f + 0.5f);
        const uint32_t v = (ua << 24) | (uc << 16) | (uc << 8) | uc;
        std::fill(px.begin() + static_cast<std::ptrdiff_t>(y) * W,
                  px.begin() + static_cast<std::ptrdiff_t>(y + 1) * W, v);
    }
}

// ---- Visuals ----

// Hover: the card grows a little and lifts.
static void PlaceBarCard(const DesktopCard& c) {
    if (!c.visual) {
        return;
    }
    const float s = 1.0f + 0.06f * c.hover;
    const float cy = static_cast<float>(BAR_H - BAR_CARD_H) * 0.5f;
    c.visual->SetTransform(MakeMatrix(
        s, 0.0f, 0.0f, s,
        static_cast<float>(c.x) + (1.0f - s) * static_cast<float>(c.w) * 0.5f,
        cy + (1.0f - s) * static_cast<float>(BAR_CARD_H) * 0.5f - 4.0f * c.hover));
}

// The close button of a desktop card: the same look as the one on windows
// (dark glass, red when the mouse is on it, a white X), a little smaller.
static void RenderBarCloseButton(bool hot, std::vector<uint32_t>& px, int& size) {
    size = 2 * BAR_X_R + 4;
    px.assign(static_cast<size_t>(size) * size, 0);
    const float c = static_cast<float>(size) * 0.5f;
    const float fr = hot ? 232.0f : 32.0f;
    const float fg = hot ? 17.0f : 32.0f;
    const float fb = hot ? 35.0f : 36.0f;
    const float fillA = hot ? 1.0f : 0.82f;
    const float ext = 3.8f * static_cast<float>(BAR_X_R) / 9.0f;
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            const float dx = x + 0.5f - c;
            const float dy = y + 0.5f - c;
            const float cov = Clamp01(static_cast<float>(BAR_X_R) + 0.5f - std::sqrt(dx * dx + dy * dy));
            if (cov <= 0.0f) {
                continue;
            }
            float xa = 0.0f;
            if (std::fabs(dx) <= ext && std::fabs(dy) <= ext) {
                const float d1 = std::fabs(dx - dy) * 0.70711f;
                const float d2 = std::fabs(dx + dy) * 0.70711f;
                xa = Clamp01(1.3f - (d1 < d2 ? d1 : d2));
            }
            const float ca = fillA * cov;
            const float oa = xa + ca * (1.0f - xa);
            const float orr = xa + (fr / 255.0f) * ca * (1.0f - xa);
            const float og = xa + (fg / 255.0f) * ca * (1.0f - xa);
            const float ob = xa + (fb / 255.0f) * ca * (1.0f - xa);
            px[static_cast<size_t>(y) * size + x] =
                (static_cast<uint32_t>(oa * 255.0f + 0.5f) << 24) |
                (static_cast<uint32_t>(orr * 255.0f + 0.5f) << 16) |
                (static_cast<uint32_t>(og * 255.0f + 0.5f) << 8) |
                static_cast<uint32_t>(ob * 255.0f + 0.5f);
        }
    }
}

// A live DWM thumbnail of one window, scaled into the card's little screen.
static bool AddCardThumb(DesktopCard& c, const CardWin& w) {
    if (!c.thumbHost || !g_pQuerySize || !g_pCreateShared) {
        return false;
    }

    SIZE sz{};
    if (FAILED(g_pQuerySize(w.hwnd, FALSE, &sz)) || sz.cx <= 0 || sz.cy <= 0) {
        return false;  // the wireframe stays
    }

    DWM_THUMBNAIL_PROPERTIES props{};
    props.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_RECTSOURCE |
                    DWM_TNP_VISIBLE | DWM_TNP_OPACITY |
                    DWM_TNP_SOURCECLIENTAREAONLY;
    props.rcDestination = { 0, 0, sz.cx, sz.cy };
    props.rcSource = { 0, 0, sz.cx, sz.cy };
    props.opacity = 255;
    props.fVisible = TRUE;
    props.fSourceClientAreaOnly = FALSE;

    IDCompositionVisual2* v = nullptr;
    HTHUMBNAIL th = nullptr;
    if (FAILED(g_pCreateShared(g_thumbHost, w.hwnd, 2, &props, g_dcDevice,
                               reinterpret_cast<void**>(&v), &th)) ||
        !v) {
        return false;
    }
    v->SetBitmapInterpolationMode(DCOMPOSITION_BITMAP_INTERPOLATION_MODE_LINEAR);

    const float monW = static_cast<float>(g_workRect.right - g_workRect.left);
    const float monH = static_cast<float>(g_workRect.bottom - g_workRect.top);
    const float sx = static_cast<float>(BAR_WELL_W) / monW;
    const float sy = static_cast<float>(BAR_WELL_H) / monH;
    const float rw = static_cast<float>(w.rect.right - w.rect.left) * sx;
    const float rh = static_cast<float>(w.rect.bottom - w.rect.top) * sy;
    const float wx0 = static_cast<float>(c.w - BAR_WELL_W) * 0.5f;
    v->SetTransform(MakeMatrix(rw / static_cast<float>(sz.cx), 0.0f, 0.0f,
                               rh / static_cast<float>(sz.cy),
                               wx0 + static_cast<float>(w.rect.left) * sx,
                               static_cast<float>(BAR_WELL_Y) + static_cast<float>(w.rect.top) * sy));

    auto* host = c.thumbHost;
    if (FAILED(host->AddVisual(v, TRUE, c.thumbTop))) {
        v->Release();
        if (th) {
            DwmUnregisterThumbnail(th);
        }
        return false;
    }
    c.thumbVisuals.push_back(v);
    c.thumbs.push_back(th);
    c.thumbTop = v;
    return true;
}

// Creates a few live thumbnails per frame (the bar fades in while they appear).
// Returns true when the composition tree changed.
static bool ProcessBarThumbQueue() {
    bool changed = false;
    int done = 0;
    while (!g_barThumbQueue.empty() && done < BAR_THUMBS_PER_FRAME) {
        const std::pair<int, int> job = g_barThumbQueue.front();
        g_barThumbQueue.erase(g_barThumbQueue.begin());
        done++;
        if (job.first >= 0 && job.first < static_cast<int>(g_cards.size())) {
            DesktopCard& c = g_cards[static_cast<size_t>(job.first)];
            if (job.second >= 0 && job.second < static_cast<int>(c.wins.size()) &&
                AddCardThumb(c, c.wins[static_cast<size_t>(job.second)])) {
                changed = true;
            }
        }
    }
    return changed;
}

static void CreateDesktopBarVisuals() {
    if (g_cards.empty() || !g_dcOk || !g_dcBarLayer) {
        return;
    }

    const int W = g_workRect.right - g_workRect.left;
    const int H = g_workRect.bottom - g_workRect.top;

    // The strip is the parent of the cards, like the window title is a child of
    // the icon: that way the cards need no sibling ordering at all.
    {
        std::vector<uint32_t> px;
        RenderBarStrip(px, W);
        IDCompositionSurface* surf = CreatePixelSurface(px, W, BAR_H);
        IDCompositionVisual2* strip = nullptr;
        if (!surf || FAILED(g_dcDevice->CreateVisual(&strip)) || !strip) {
            Wh_Log(L"Desktop bar: could not create the strip");
            if (surf) {
                surf->Release();
            }
            return;
        }
        strip->SetContent(surf);
        surf->Release();
        strip->SetBitmapInterpolationMode(DCOMPOSITION_BITMAP_INTERPOLATION_MODE_LINEAR);
        if (FAILED(g_dcBarLayer->AddVisual(strip, TRUE, nullptr))) {
            Wh_Log(L"Desktop bar: could not add the strip");
            strip->Release();
            return;
        }
        g_barStrip = strip;  // we keep this reference until ClearDesktopBar()
    }

    // The close button (a desktop can only be closed if it isn't the only one).
    const int desktops = static_cast<int>(g_cards.size()) - 1;
    const int xs = 2 * BAR_X_R + 4;
    if (desktops > 1) {
        for (int hot = 0; hot < 2; hot++) {
            std::vector<uint32_t> xp;
            int size = 0;
            RenderBarCloseButton(hot != 0, xp, size);
            g_xSurf[hot] = CreatePixelSurface(xp, size, size);
        }
    }

    g_barThumbQueue.clear();
    int made = 0;
    for (size_t ci = 0; ci < g_cards.size(); ci++) {
        DesktopCard& c = g_cards[ci];

        std::vector<uint32_t> cardPx;  // its own buffer for every upload
        RenderDesktopCard(c, cardPx);
        IDCompositionSurface* surf = CreatePixelSurface(cardPx, c.w, BAR_CARD_H);
        if (!surf) {
            Wh_Log(L"Desktop bar: no surface for a card (%dx%d)", c.w, BAR_CARD_H);
            continue;
        }
        IDCompositionVisual2* v = nullptr;
        if (FAILED(g_dcDevice->CreateVisual(&v)) || !v) {
            surf->Release();
            continue;
        }
        v->SetContent(surf);
        surf->Release();
        v->SetBitmapInterpolationMode(DCOMPOSITION_BITMAP_INTERPOLATION_MODE_LINEAR);
        if (FAILED(g_barStrip->AddVisual(v, TRUE, nullptr))) {
            Wh_Log(L"Desktop bar: could not add a card");
            v->Release();
            continue;
        }
        c.visual = v;  // our reference, released in ClearDesktopBar()

        if (!c.isAdd) {
            // Where the live thumbnails of its windows go.
            IDCompositionVisual2* host = nullptr;
            if (SUCCEEDED(g_dcDevice->CreateVisual(&host)) && host) {
                if (SUCCEEDED(v->AddVisual(host, TRUE, nullptr))) {
                    c.thumbHost = host;
                } else {
                    host->Release();
                }
            }

            // The close button, above the thumbnails.
            if (desktops > 1 && g_xSurf[0] && g_xSurf[1]) {
                IDCompositionVisual2* xv = nullptr;
                IDCompositionEffectGroup* fx = nullptr;
                if (SUCCEEDED(g_dcDevice->CreateVisual(&xv)) && xv &&
                    SUCCEEDED(g_dcDevice->CreateEffectGroup(&fx)) && fx) {
                    xv->SetContent(g_xSurf[0]);
                    xv->SetBitmapInterpolationMode(DCOMPOSITION_BITMAP_INTERPOLATION_MODE_LINEAR);
                    fx->SetOpacity(0.0f);
                    xv->SetEffect(fx);
                    xv->SetTransform(MakeMatrix(1.0f, 0.0f, 0.0f, 1.0f,
                                                static_cast<float>(c.w - BAR_X_INSET - xs),
                                                static_cast<float>(BAR_X_INSET)));
                    if (SUCCEEDED(v->AddVisual(xv, TRUE, c.thumbHost))) {
                        c.xVisual = xv;
                        c.xFx = fx;
                    } else {
                        fx->Release();
                        xv->Release();
                    }
                } else {
                    if (fx) fx->Release();
                    if (xv) xv->Release();
                }
            }

            // Live thumbnails, bottom-most window first so they stack correctly.
            for (size_t wi = c.wins.size(); wi-- > 0;) {
                g_barThumbQueue.push_back({ static_cast<int>(ci), static_cast<int>(wi) });
            }
        }

        PlaceBarCard(c);
        made++;
    }
    Wh_Log(L"Desktop bar: %d of %d card(s) created", made, static_cast<int>(g_cards.size()));

    g_barTop = H - BAR_H;  // flush with the bottom edge
    g_barOp = -1.0f;       // forces the first ApplyProgress to set opacity and position
    g_barHover = -1;
    g_barCloseHover = -1;
    g_barBuilt = true;
}

static void ClearDesktopBar() {
    for (auto& c : g_cards) {
        for (HTHUMBNAIL th : c.thumbs) {
            if (th) {
                DwmUnregisterThumbnail(th);
            }
        }
        c.thumbs.clear();

        if (c.thumbHost) {
            c.thumbHost->RemoveAllVisuals();
        }
        for (IDCompositionVisual2* tv : c.thumbVisuals) {
            tv->Release();
        }
        c.thumbVisuals.clear();
        c.thumbTop = nullptr;

        if (c.visual) {
            c.visual->RemoveAllVisuals();
        }
        if (c.thumbHost) {
            c.thumbHost->Release();
            c.thumbHost = nullptr;
        }
        if (c.xVisual) {
            c.xVisual->Release();
            c.xVisual = nullptr;
        }
        if (c.xFx) {
            c.xFx->Release();
            c.xFx = nullptr;
        }
        if (c.visual) {
            c.visual->Release();
            c.visual = nullptr;
        }
    }
    if (g_barStrip) {
        g_barStrip->RemoveAllVisuals();
        g_barStrip->Release();
        g_barStrip = nullptr;
    }
    if (g_dcBarLayer) {
        g_dcBarLayer->RemoveAllVisuals();
    }
    if (g_barFx) {
        g_barFx->SetOpacity(0.0f);
    }
    for (auto& xs : g_xSurf) {
        if (xs) {
            xs->Release();
            xs = nullptr;
        }
    }
    g_barThumbQueue.clear();
    g_cards.clear();
    g_barBuilt = false;
    g_barOp = -1.0f;
    g_barHover = -1;
    g_barCloseHover = -1;
    g_barBand = 0;
}

static int HitBarCard(int x, int y) {
    if (!g_barBuilt || y < g_barTop) {
        return -1;
    }
    const int ly = y - g_barTop;
    const int cy = (BAR_H - BAR_CARD_H) / 2;
    if (ly < cy - 4 || ly > cy + BAR_CARD_H + 4) {
        return -1;
    }
    for (size_t i = 0; i < g_cards.size(); i++) {
        if (x >= g_cards[i].x - 4 && x < g_cards[i].x + g_cards[i].w + 4) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

// Is the point on the close button of a (hovered) desktop card?
static int HitBarClose(int x, int y) {
    if (!g_barBuilt || y < g_barTop) {
        return -1;
    }
    const int xs = 2 * BAR_X_R + 4;
    const int cardY = g_barTop + (BAR_H - BAR_CARD_H) / 2;
    for (size_t i = 0; i < g_cards.size(); i++) {
        const DesktopCard& c = g_cards[i];
        if (!c.xVisual || c.hover < 0.5f) {
            continue;
        }
        const int cx = c.x + c.w - BAR_X_INSET - xs / 2;
        const int cy = cardY + BAR_X_INSET + xs / 2;
        const int dx = x - cx;
        const int dy = y - cy;
        const int r = BAR_X_R + 3;
        if (dx * dx + dy * dy <= r * r) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

static void SetBarCloseHover(int idx) {
    if (idx == g_barCloseHover) {
        return;
    }
    auto show = [](int i, bool hot) {
        if (i >= 0 && i < static_cast<int>(g_cards.size()) &&
            g_cards[static_cast<size_t>(i)].xVisual && g_xSurf[hot ? 1 : 0]) {
            g_cards[static_cast<size_t>(i)].xVisual
                ->SetContent(g_xSurf[hot ? 1 : 0]);
        }
    };
    show(g_barCloseHover, false);
    g_barCloseHover = idx;
    show(idx, true);
    if (g_dcDevice) {
        g_dcDevice->Commit();
    }
}

static void SetBarHover(int idx) {
    bool changed = false;
    for (size_t i = 0; i < g_cards.size(); i++) {
        const float t = (static_cast<int>(i) == idx) ? 1.0f : 0.0f;
        if (g_cards[i].hoverTarget != t) {
            g_cards[i].hoverTarget = t;
            changed = true;
        }
    }
    if (changed && g_overlay) {
        SetTimer(g_overlay, BAR_TIMER_ID, 15, nullptr);
    }
}

static void StepBarHover() {
    bool moving = false;
    for (auto& c : g_cards) {
        const float d = c.hoverTarget - c.hover;
        if (std::fabs(d) < 0.01f || g_reduceMotion) {
            c.hover = c.hoverTarget;
        } else {
            c.hover += d * 0.32f;
            moving = true;
        }
        PlaceBarCard(c);
        if (c.xFx) {
            c.xFx->SetOpacity(c.hover);  // the X shows on hover
        }
    }
    if (g_dcDevice) {
        g_dcDevice->Commit();
    }
    if (!moving && g_overlay) {
        KillTimer(g_overlay, BAR_TIMER_ID);
    }
}


// Releases every DirectComposition object of a tile (and of its dialogs).
static void ReleaseDCompVisuals(Tile& t, bool& changed) {
    for (auto& k : t.kids) {
        ReleaseDCompVisuals(k, changed);
    }
    t.kids.clear();

    if (t.iconVisual) {
        auto* iv = t.iconVisual;
        if (g_dcIcons) {
            g_dcIcons->RemoveVisual(iv);
        }
        iv->Release();
        t.iconVisual = nullptr;
        changed = true;
    }
    if (t.iconFx) {
        t.iconFx->Release();
        t.iconFx = nullptr;
    }
    if (t.dcVisual) {
        auto* v = t.dcVisual;
        if (g_dcTiles) {
            g_dcTiles->RemoveVisual(v);
        }
        v->Release();
        t.dcVisual = nullptr;
        changed = true;
    }
    if (t.dcFx) {
        t.dcFx->Release();
        t.dcFx = nullptr;
    }
    if (t.dcThumb) {
        DwmUnregisterThumbnail(t.dcThumb);
        t.dcThumb = nullptr;
    }
}

static void UnregisterTiles() {
    ClearDesktopBar();
    g_tilesOp = -1.0f;
    g_holdTiles = false;
    bool dcChanged = false;
    for (auto& t : g_tiles) {
        ReleaseDCompVisuals(t, dcChanged);
    }
    if (g_emptyVisual) {
        auto* ev = g_emptyVisual;
        if (g_dcIcons) {
            g_dcIcons->RemoveVisual(ev);
        }
        ev->Release();
        g_emptyVisual = nullptr;
        dcChanged = true;
    }
    if (g_emptyFx) {
        g_emptyFx->Release();
        g_emptyFx = nullptr;
    }
    if (g_emptySurface) {
        g_emptySurface->Release();
        g_emptySurface = nullptr;
    }
    g_dcTop = nullptr;
    g_dcIconTop = nullptr;
    if (dcChanged && g_dcDevice) {
        g_dcDevice->Commit();
    }
    for (auto& t : g_tiles) {
        if (t.thumb) {
            DwmUnregisterThumbnail(t.thumb);
            t.thumb = nullptr;
        }
    }
    g_tiles.clear();
}

static void BuildTiles() {
    UnregisterTiles();
    g_kbSel = -1;
    g_hoverClose = false;
    g_relayoutActive = false;
    g_relayoutT = 1.0f;

    EnumCtx ctx{ g_workRect, {} };
    EnumWindows(EnumProc, reinterpret_cast<LPARAM>(&ctx));

    // EnumWindows returns top-most first. Minimized windows go after the
    // normal ones: they are "behind" everything and end up last in the grid.
    std::stable_partition(ctx.hwnds.begin(), ctx.hwnds.end(),
                          [](HWND h) { return !IsIconic(h); });

    for (HWND h : ctx.hwnds) {
        RECT r;
        bool minimized = false;
        if (!GetStartRect(h, &r, &minimized)) {
            continue;
        }
        if (r.right - r.left <= 0 || r.bottom - r.top <= 0) {
            continue;
        }

        Tile t;
        t.hwnd = h;
        t.minimized = minimized;
        t.topmost = (GetWindowLongPtrW(h, GWL_EXSTYLE) & WS_EX_TOPMOST) != 0;
        t.from = r;
        OffsetRect(&t.from, -g_workRect.left, -g_workRect.top);

        // Dialogs of this window, to draw them on top of its tile.
        if (!minimized) {
            KidCtx kc{ h, {} };
            EnumWindows(KidEnumProc, reinterpret_cast<LPARAM>(&kc));
            for (HWND kh : kc.list) {
                RECT kr{};
                if (!GetWindowRect(kh, &kr)) {
                    continue;
                }
                Tile kid;
                kid.hwnd = kh;
                kid.from = kr;
                OffsetRect(&kid.from, -g_workRect.left, -g_workRect.top);
                t.kids.push_back(kid);
            }
        }
        g_tiles.push_back(t);

        wchar_t cls[96] = {};
        wchar_t title[48] = {};
        GetClassNameW(h, cls, 96);
        InternalGetWindowText(h, title, 48);
        Wh_Log(L"  window [%s] \"%s\" (%ldx%ld)%s", cls, title,
               r.right - r.left, r.bottom - r.top,
               minimized ? L" minimized" : L"");
    }

    // Before layout, so the grid is computed from the rects the thumbnails
    // will really start from.
    for (auto& t : g_tiles) {
        CalibrateDCompSource(t);
        for (auto& k : t.kids) {
            CalibrateDCompSource(k);
        }
        // A dialog DWM can't give a thumbnail for is simply not drawn.
        t.kids.erase(std::remove_if(t.kids.begin(), t.kids.end(),
                                    [](const Tile& k) { return k.srcW < 2.0f; }),
                     t.kids.end());
    }
    if (g_dcOk) {
        // A minimized window DWM can't give a thumbnail for would be an empty slot.
        g_tiles.erase(std::remove_if(g_tiles.begin(), g_tiles.end(),
                                     [](const Tile& t) {
                                         return t.minimized && t.srcW < 2.0f;
                                     }),
                      g_tiles.end());
    }
    g_iconBand = (g_dcOk && (SHOW_ICONS || SHOW_TITLES))
                     ? ICON_GAP + (SHOW_ICONS ? ICON_SIZE : 0) +
                           (SHOW_TITLES ? TITLE_H + (SHOW_ICONS ? TITLE_GAP : 0) : 0)
                     : 0;
    PrepareDesktopCards();  // the bar takes space from the bottom of the grid

    LayoutTiles();

    Wh_Log(L"Layout: %d tiles in area %ldx%ld", static_cast<int>(g_tiles.size()),
           g_workRect.right - g_workRect.left, g_workRect.bottom - g_workRect.top);
    for (const auto& t : g_tiles) {
        Wh_Log(L"  tile from=(%ld,%ld,%ld,%ld) to=(%ld,%ld,%ld,%ld)",
               t.from.left, t.from.top, t.from.right, t.from.bottom,
               t.to.left, t.to.top, t.to.right, t.to.bottom);
    }

    // Register bottom-most first so the top-most window is drawn last.
    for (size_t i = g_tiles.size(); i-- > 0;) {
        if (CreateDCompTile(g_tiles[i])) {
            // Dialogs right above their owner (bottom-most first).
            for (size_t k = g_tiles[i].kids.size(); k-- > 0;) {
                CreateDCompTile(g_tiles[i].kids[k]);
            }
            CreateDCompIcon(g_tiles[i]);
            continue;
        }
        if (FAILED(DwmRegisterThumbnail(g_overlay, g_tiles[i].hwnd,
                                        &g_tiles[i].thumb))) {
            g_tiles[i].thumb = nullptr;
        }
    }

    CreateDesktopBarVisuals();
    if (g_tiles.empty()) {
        CreateDCompEmptyMessage();
    }
}

// ---------------------------------------------------------------------------
// Overlay rendering
// ---------------------------------------------------------------------------

static void SetClickThrough(bool clickThrough) {
    if (!g_overlay) {
        return;
    }
    LONG_PTR ex = GetWindowLongPtrW(g_overlay, GWL_EXSTYLE);
    LONG_PTR wanted = clickThrough ? (ex | WS_EX_TRANSPARENT)
                                   : (ex & ~static_cast<LONG_PTR>(WS_EX_TRANSPARENT));
    if (wanted != ex) {
        SetWindowLongPtrW(g_overlay, GWL_EXSTYLE, wanted);
    }
}

// ---------------------------------------------------------------------------
// Taskbar slide
// ---------------------------------------------------------------------------

struct TaskbarSlide {
    HWND hwnd = nullptr;
    RECT orig{};          // where it really lives (screen coordinates)
    int dx = 0, dy = 0;   // full displacement when completely hidden
    int lastX = 0, lastY = 0;
    bool placed = false;
    bool cloaked = false; // the real taskbar is hidden while its copy slides
    double createdMs = 0.0;
    float srcW = 1.0f, srcH = 1.0f;
    IDCompositionVisual2* visual = nullptr;  // the copy
    HTHUMBNAIL thumb = nullptr;
};
static std::vector<TaskbarSlide> g_taskbars;

static BOOL CALLBACK CollectMonitorsProc(HMONITOR, HDC, LPRECT r, LPARAM lp) {
    reinterpret_cast<std::vector<RECT>*>(lp)->push_back(*r);
    return TRUE;
}

// Hides / shows a taskbar window without moving it (DWM cloak). Only works on
// windows of the calling process, which is fine: this runs inside explorer.
static void SetTaskbarCloak(HWND hwnd, bool on) {
    BOOL v = on ? TRUE : FALSE;
    DwmSetWindowAttribute(hwnd, 13 /*DWMWA_CLOAK*/, &v, sizeof(v));
}

static void UncloakTaskbars() {
    for (auto& s : g_taskbars) {
        if (s.cloaked) {
            SetTaskbarCloak(s.hwnd, false);
            s.cloaked = false;
        }
    }
}

// Overlay hidden (or about to be): real taskbars back, copies removed.
static void RestoreTaskbars() {
    if (g_taskbars.empty()) {
        return;
    }

    UncloakTaskbars();

    for (auto& s : g_taskbars) {
        if (s.visual) {
            auto* v = s.visual;
            if (g_dcTaskbarLayer) {
                g_dcTaskbarLayer->RemoveVisual(v);
            }
            v->Release();
            s.visual = nullptr;
        }
        if (s.thumb) {
            DwmUnregisterThumbnail(s.thumb);
            s.thumb = nullptr;
        }
    }
    if (g_dcDevice) {
        g_dcDevice->Commit();
    }

    g_taskbars.clear();
}


static void AddSlidableTaskbar(HWND tb, const std::vector<RECT>& monitors) {
    if (!tb) {
        return;
    }
    if (!IsWindowVisible(tb)) {
        Wh_Log(L"Taskbar slide: taskbar is not visible, skipped");
        return;
    }

    RECT r{};
    if (!GetWindowRect(tb, &r)) {
        return;
    }

    RECT inter{};
    if (!IntersectRect(&inter, &r, &g_workRect)) {
        return;  // lives on another monitor
    }
    // Hidden by auto-hide (mostly outside the monitor): nothing to slide.
    if (r.left < g_workRect.left - 2 || r.right > g_workRect.right + 2 ||
        r.top < g_workRect.top - 2 || r.bottom > g_workRect.bottom + 2) {
        Wh_Log(L"Taskbar slide: taskbar rect (%ld,%ld,%ld,%ld) is not inside the monitor "
               L"(auto-hide?), skipped", r.left, r.top, r.right, r.bottom);
        return;
    }

    const int monW = g_workRect.right - g_workRect.left;
    const int monH = g_workRect.bottom - g_workRect.top;
    const int w = r.right - r.left;
    const int h = r.bottom - r.top;
    const int extra = static_cast<int>(std::lround(TASKBAR_EXTRA_PX * g_dpiScale));

    TaskbarSlide t;
    t.hwnd = tb;
    t.orig = r;
    if (w >= monW - 4 && h < monH / 2) {  // horizontal bar
        if (r.top > g_workRect.top + monH / 2) {
            t.dy = (g_workRect.bottom - r.top) + extra;    // bottom: down
        } else {
            t.dy = -((r.bottom - g_workRect.top) + extra);  // top: up
        }
    } else if (h >= monH - 4 && w < monW / 2) {  // vertical bar
        if (r.left > g_workRect.left + monW / 2) {
            t.dx = (g_workRect.right - r.left) + extra;     // right
        } else {
            t.dx = -((r.right - g_workRect.left) + extra);  // left
        }
    } else {
        Wh_Log(L"Taskbar slide: unusual taskbar rect (%ld,%ld,%ld,%ld), skipped",
               r.left, r.top, r.right, r.bottom);
        return;
    }

    // The copy never leaves the overlay, so nothing shows up on another
    // monitor, but keep the check in case the layout is exotic.
    RECT moved = r;
    OffsetRect(&moved, t.dx, t.dy);
    for (const RECT& m : monitors) {
        RECT x{};
        if (!EqualRect(&m, &g_workRect) && IntersectRect(&x, &moved, &m)) {
            Wh_Log(L"Taskbar slide: it would land on another monitor, skipped");
            return;
        }
    }

    // A live copy of the taskbar, as a visual in the overlay's host window.
    SIZE sz{};
    if (!g_dcOk || !g_dcTaskbarLayer || FAILED(g_pQuerySize(tb, FALSE, &sz)) ||
        sz.cx <= 0 || sz.cy <= 0) {
        Wh_Log(L"Taskbar slide: could not query the taskbar size, skipped");
        return;
    }

    DWM_THUMBNAIL_PROPERTIES props{};
    props.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_RECTSOURCE |
                    DWM_TNP_VISIBLE | DWM_TNP_OPACITY |
                    DWM_TNP_SOURCECLIENTAREAONLY;
    props.rcDestination = { 0, 0, sz.cx, sz.cy };
    props.rcSource = { 0, 0, sz.cx, sz.cy };
    props.opacity = 255;
    props.fVisible = TRUE;
    props.fSourceClientAreaOnly = FALSE;

    IDCompositionVisual2* visual = nullptr;
    HTHUMBNAIL thumb = nullptr;
    const HRESULT hr = g_pCreateShared(g_thumbHost, tb, 2, &props, g_dcDevice,
                                       reinterpret_cast<void**>(&visual), &thumb);
    if (FAILED(hr) || !visual) {
        Wh_Log(L"Taskbar slide: DWM would not give a thumbnail of the taskbar (0x%08X), skipped",
               static_cast<unsigned>(hr));
        return;
    }

    visual->SetBitmapInterpolationMode(DCOMPOSITION_BITMAP_INTERPOLATION_MODE_LINEAR);
    if (FAILED(g_dcTaskbarLayer->AddVisual(visual, TRUE, nullptr))) {
        visual->Release();
        if (thumb) {
            DwmUnregisterThumbnail(thumb);
        }
        return;
    }

    t.visual = visual;
    t.thumb = thumb;
    t.srcW = static_cast<float>(sz.cx);
    t.srcH = static_cast<float>(sz.cy);
    t.createdMs = NowMs();

    Wh_Log(L"Taskbar slide: copy of the taskbar ready, rect=(%ld,%ld,%ld,%ld) src=%ldx%ld shift=(%d,%d)",
           r.left, r.top, r.right, r.bottom, sz.cx, sz.cy, t.dx, t.dy);
    g_taskbars.push_back(t);
}

static void PlaceTaskbarCopy(TaskbarSlide& s, int ox, int oy) {
    const float w = static_cast<float>(s.orig.right - s.orig.left);
    const float h = static_cast<float>(s.orig.bottom - s.orig.top);
    const D2D_MATRIX_3X2_F m = MakeMatrix(
        w / s.srcW, 0.0f,
        0.0f, h / s.srcH,
        static_cast<float>(s.orig.left - g_workRect.left + ox),
        static_cast<float>(s.orig.top - g_workRect.top + oy));
    s.visual->SetTransform(m);
}


static void BeginTaskbarSlide() {
    if (!SLIDE_TASKBAR || !g_dcOk) {
        return;
    }

    RestoreTaskbars();  // leftovers from a previous gesture, if any

    std::vector<RECT> monitors;
    EnumDisplayMonitors(nullptr, nullptr, CollectMonitorsProc,
                        reinterpret_cast<LPARAM>(&monitors));

    AddSlidableTaskbar(FindWindowW(L"Shell_TrayWnd", nullptr), monitors);
    for (HWND h = nullptr;
         (h = FindWindowExW(nullptr, h, L"Shell_SecondaryTrayWnd", nullptr)) != nullptr;) {
        AddSlidableTaskbar(h, monitors);
    }

    if (g_taskbars.empty()) {
        Wh_Log(L"Taskbar slide: no taskbar to slide on this monitor");
    }
}

// p = Mission Control progress: the taskbar is gone at TASKBAR_HIDE_END.
// Returns true if the DirectComposition tree changed (caller commits).
static bool ApplyTaskbarSlide(float p) {
    if (g_taskbars.empty()) {
        return false;
    }

    const float t = Clamp01(p / TASKBAR_HIDE_END);
    const double now = NowMs();
    bool changed = false;

    for (auto& s : g_taskbars) {
        const int ox = static_cast<int>(std::lround(static_cast<float>(s.dx) * t));
        const int oy = static_cast<int>(std::lround(static_cast<float>(s.dy) * t));

        if (!(s.placed && ox == s.lastX && oy == s.lastY)) {
            PlaceTaskbarCopy(s, ox, oy);
            s.lastX = ox;
            s.lastY = oy;
            s.placed = true;
            changed = true;
        }

        // Swap the real taskbar for its copy only once it starts to move and
        // the copy has had a few frames to appear, so there is no blink.
        if (!s.cloaked && t > 0.02f && now - s.createdMs > 80.0) {
            SetTaskbarCloak(s.hwnd, true);
            s.cloaked = true;
        }
    }
    return changed;
}

// Windows being closed from Mission Control. Such a window is hidden first (so
// no close animation has anything to animate), then asked to close. If it is
// still there a moment later (it asked "Save changes?", it closes to the tray,
// or it isn't responding) a window we hid is always shown again: an unsaved
// document or a prompt must never stay invisible.
struct PendingClose {
    HWND hwnd = nullptr;
    double startMs = 0.0;
    double postMs = 0.0;
    double shownMs = 0.0;
    bool hidden = false;         // we hid it ourselves
    bool posted = false;         // WM_CLOSE sent
    bool noTransitions = false;  // we turned its DWM transitions off
    bool shownBack = false;      // it survived the close and was shown again
};
static std::vector<PendingClose> g_pendingCloses;

// DWM's open / close / show / hide transitions of ONE window. Turned off on the
// window being closed so Windows' own close animation doesn't play in front of
// Mission Control, and turned back on if the window is still there afterwards.
static HRESULT SetWindowTransitions(HWND hwnd, bool enabled) {
    BOOL disabled = enabled ? FALSE : TRUE;
    const HRESULT hr = DwmSetWindowAttribute(hwnd, DWMWA_TRANSITIONS_FORCEDISABLED,
                                             &disabled, sizeof(disabled));
    static bool logged = false;
    if (FAILED(hr) && !logged) {
        logged = true;
        Wh_Log(L"DWMWA_TRANSITIONS_FORCEDISABLED refused on another app's window (0x%08X)",
               static_cast<unsigned>(hr));
    }
    return hr;
}

// Unloading: finish what was started. A window we hid comes back, and one that
// was not asked to close yet is asked now.
static void FlushPendingCloses() {
    for (auto& pc : g_pendingCloses) {
        if (!IsWindow(pc.hwnd)) {
            continue;
        }
        if (pc.noTransitions) {
            SetWindowTransitions(pc.hwnd, true);
        }
        if (pc.hidden && !pc.shownBack) {
            ShowWindowAsync(pc.hwnd, SW_SHOWNA);
        }
        if (!pc.posted) {
            PostMessageW(pc.hwnd, WM_CLOSE, 0, 0);
        }
    }
    g_pendingCloses.clear();
}

// The size of everything follows the monitor, so the interface looks right from
// a 1366x768 laptop to a 2560x1440 or a 4K screen, at any display scaling. The
// resolution factor is computed on the logical size (physical pixels / DPI
// scale) and everything is then multiplied by the DPI scale, since this thread
// works in physical pixels. The icons are proportional to the logical height
// (56 px at 1440); the rest (gaps, titles, close button, the desktops bar)
// scales more gently so text stays readable on small screens. Run before every
// layout.
static void UpdateUiScale(int monitorHeight, UINT dpi) {
    const float d = static_cast<float>(dpi < 96 ? 96 : dpi) / 96.0f;
    g_dpiScale = d;

    float h = static_cast<float>(monitorHeight) / d;
    h = h < 480.0f ? 480.0f : h;
    float s = 0.55f + 0.45f * (h / 1440.0f);
    s = s < 0.78f ? 0.78f : (s > 1.5f ? 1.5f : s);
    g_uiScale = s * d;
    // `lo` is the smallest size at 100 % scaling.
    auto sc = [&](float v, int lo) {
        float l = v * s;
        if (l < static_cast<float>(lo)) {
            l = static_cast<float>(lo);
        }
        return static_cast<int>(std::lround(l * d));
    };
    auto px = [&](float v, int lo) {
        const int r = static_cast<int>(std::lround(v * d));
        return r < lo ? lo : r;
    };

    float icon = std::round(h * 56.0f / 1440.0f);
    icon = icon < 32.0f ? 32.0f : (icon > 96.0f ? 96.0f : icon);
    ICON_SIZE = static_cast<int>(std::lround(icon * d));
    ICON_GAP = sc(14.0f, 8);
    TITLE_FONT_PX = sc(14.0f, 11);
    TITLE_H = sc(20.0f, 16);
    TITLE_GAP = sc(4.0f, 3);
    CB_RADIUS = sc(13.0f, 10);
    CB_INSET = sc(10.0f, 7);
    OVERLAY_MARGIN = sc(80.0f, 40);
    TILE_PADDING = sc(32.0f, 18);

    BAR_CARD_H = sc(76.0f, 56);
    BAR_CARD_W = sc(116.0f, 84);
    BAR_ADD_W = sc(76.0f, 56);
    BAR_GAP = sc(12.0f, 8);
    BAR_H = BAR_CARD_H + sc(16.0f, 10);
    BAR_X_R = sc(9.0f, 8);
    BAR_X_INSET = sc(6.0f, 4);
    BAR_WELL_W = sc(90.0f, 64);
    BAR_WELL_H = sc(50.0f, 36);
    BAR_WELL_Y = sc(6.0f, 4);

    // The hover outline only follows the DPI.
    HL_GAP = px(4.0f, 1);
    HL_THICK = px(3.0f, 1);
    HL_RADIUS = px(12.0f, 2);
    HL_PAD = px(10.0f, 2);
}

// Windows "Animation effects" (Settings > Accessibility > Visual effects).
static void UpdateReducedMotion() {
    g_reduceMotion = false;
    if (!RESPECT_REDUCED_MOTION) {
        return;
    }
    BOOL on = TRUE;
    if (SystemParametersInfoW(0x1042 /*SPI_GETCLIENTAREAANIMATION*/, 0, &on, 0)) {
        g_reduceMotion = (on == FALSE);
    }
}

// Where a tile's grid slot is drawn right now (it glides after a window is closed).
static RECT EffectiveTo(const Tile& t) {
    if (!g_relayoutActive) {
        return t.to;
    }
    return LerpRect(t.toPrev, t.to, Smooth01(g_relayoutT));
}

// Advances the grid glide. Returns true while it is still running.
static bool StepRelayout(double now) {
    if (!g_relayoutActive) {
        return false;
    }
    const double t = (now - g_relayoutStartMs) / (g_reduceMotion ? 1.0 : RELAYOUT_MS);
    if (t >= 1.0) {
        g_relayoutT = 1.0f;
        g_relayoutActive = false;
    } else {
        g_relayoutT = static_cast<float>(t);
    }
    return true;
}

// The window to activate for a tile: if its app has a dialog open, that
// dialog (the owner itself may be disabled while the dialog is up).
static HWND ResolvePickTarget(HWND tileWnd) {
    HWND popup = GetLastActivePopup(tileWnd);
    if (popup && popup != tileWnd && IsWindowVisible(popup) && IsWindowEnabled(popup)) {
        return popup;
    }
    return tileWnd;
}

static void ApplyProgress(float p) {
    g_progress = p < 0.0f ? 0.0f : (p > 1.0f + SPRING_MAX_OVERSHOOT ? 1.0f + SPRING_MAX_OVERSHOOT : p);
    g_renderDirty = false;
    g_lastRenderMs = NowMs();

    // The backdrop fades in between FADE_START_PROGRESS and FADE_END_PROGRESS
    // (a bit later than the start so the taskbar is seen leaving first). The
    // alpha is constant outside that, so only touch the window when it changes.
    {
        const float f = Clamp01((g_progress - FADE_START_PROGRESS) /
                                (FADE_END_PROGRESS - FADE_START_PROGRESS));
        // Ease-out: most of the coverage happens early in the fade, which
        // is what hides the real windows still sitting behind the tiles.
        const BYTE alpha = static_cast<BYTE>(OVERLAY_MAX_ALPHA * (f * (2.0f - f)));
        if (alpha != g_lastAlpha) {
            SetLayeredWindowAttributes(g_overlay, 0, alpha, LWA_ALPHA);
            g_lastAlpha = alpha;
        }
    }

    const bool taskbarChanged = ApplyTaskbarSlide(g_progress);

    // Only touch the thumbnails that actually moved.
    bool dcChanged = taskbarChanged;
    // The icons fade with the swipe itself: same progress the fingers drive.
    const float iconOp = Smooth01((g_progress - ICON_FADE_START) / (1.0f - ICON_FADE_START));
    const float minOp = Smooth01(g_progress / MINIMIZED_FADE_END);
    for (auto& t : g_tiles) {
        if (t.dcVisual) {
            const RECT r = LerpRect(t.from, EffectiveTo(t), g_progress);
            const bool moved = !(t.placed && EqualRect(&t.last, &r));
            if (moved) {
                PlaceDCompTile(t, r);
                PlaceKids(t, r);
                t.last = r;
                t.placed = true;
                dcChanged = true;
            }
            if (t.dcFx && t.fadeIn && OpacityChanged(minOp, t.fxOp)) {
                t.dcFx->SetOpacity(minOp);
                t.fxOp = minOp;
                dcChanged = true;
            }

            if (t.iconVisual) {
                if (moved || !t.iconPlaced) {
                    PlaceDCompIcon(t, r);
                    t.iconPlaced = true;
                    dcChanged = true;
                }
                if (OpacityChanged(iconOp, t.iconOp)) {
                    t.iconFx->SetOpacity(iconOp);
                    t.iconOp = iconOp;
                    dcChanged = true;
                }
            }
            continue;
        }
        // Legacy tile: every DwmUpdateThumbnailProperties is a round trip to
        // dwm.exe, so only send what changed.
        if (!t.thumb) {
            continue;
        }

        const RECT r = LerpRect(t.from, EffectiveTo(t), g_progress);
        if (t.placed && EqualRect(&t.last, &r)) {
            continue;
        }

        DWM_THUMBNAIL_PROPERTIES props{};
        props.rcDestination = r;
        if (!t.placed) {
            props.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE |
                            DWM_TNP_OPACITY | DWM_TNP_SOURCECLIENTAREAONLY;
            props.fVisible = TRUE;
            props.opacity = 255;
            props.fSourceClientAreaOnly = FALSE;
        } else {
            props.dwFlags = DWM_TNP_RECTDESTINATION;
        }

        if (SUCCEEDED(DwmUpdateThumbnailProperties(t.thumb, &props))) {
            t.last = r;
            t.placed = true;
        }
    }
    if (g_tilesFx) {
        // The tiles fade in during the first stretch and out during the last one,
        // so the swap with the real windows can't be seen.
        const float to = g_holdTiles ? 1.0f : Smooth01(g_progress / TILE_XFADE_END);
        if (OpacityChanged(to, g_tilesOp)) {
            g_tilesFx->SetOpacity(to);
            g_tilesOp = to;
            dcChanged = true;
        }
    }
    if (g_barBuilt && g_dcBarLayer && g_barFx) {
        // Fades in during the second half of the swipe, sliding up from the edge.
        const float bop = Smooth01((g_progress - BAR_FADE_START) / (1.0f - BAR_FADE_START));
        if (OpacityChanged(bop, g_barOp)) {
            g_barFx->SetOpacity(bop);
            const float slide = (1.0f - bop) * static_cast<float>(BAR_H + 6);
            g_dcBarLayer->SetTransform(MakeMatrix(1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                                                  static_cast<float>(g_barTop) + slide));
            g_barOp = bop;
            dcChanged = true;
        }
    }
    if (g_barBuilt && g_progress > 0.2f && !g_barThumbQueue.empty()) {
        if (ProcessBarThumbQueue()) {
            dcChanged = true;
        }
    }
    if (g_emptyVisual) {
        auto* ev = g_emptyVisual;
        if (!g_emptyPlaced) {
            const float W = static_cast<float>(g_workRect.right - g_workRect.left);
            const float H = static_cast<float>(g_workRect.bottom - g_workRect.top);
            const D2D_MATRIX_3X2_F m = MakeMatrix(
                1.0f, 0.0f, 0.0f, 1.0f,
                (W - static_cast<float>(g_emptyW)) * 0.5f,
                (H - static_cast<float>(g_emptyH)) * 0.5f);
            ev->SetTransform(m);
            g_emptyPlaced = true;
            dcChanged = true;
        }
        const float eop = Smooth01((g_progress - EMPTY_FADE_START) / (1.0f - EMPTY_FADE_START));
        if (OpacityChanged(eop, g_emptyOp)) {
            g_emptyFx->SetOpacity(eop);
            g_emptyOp = eop;
            dcChanged = true;
        }
    }
    if (dcChanged && g_dcDevice) {
        g_dcDevice->Commit();  // one batched update for every tile
    }
}

// The touchpad reports each finger separately at a high rate. Render right
// away if the last render was long enough ago, otherwise remember that
// something is pending and let a short timer flush the newest state.
static void RequestRender() {
    if (NowMs() - g_lastRenderMs >= MIN_RENDER_INTERVAL_MS) {
        ApplyProgress(g_progress);
        return;
    }

    g_renderDirty = true;
    if (!g_renderTimerSet && g_overlay) {
        SetTimer(g_overlay, RENDER_TIMER_ID, 4, nullptr);
        g_renderTimerSet = true;
    }
}

// ---------------------------------------------------------------------------
// Hover outline (separate per-pixel-alpha window so the color is crisp)
// ---------------------------------------------------------------------------

static void ReadAccentColor() {
    // The same accent Windows 11's Alt+Tab uses for its selection: Fluent's
    // "accent fill" is Light 2 of the accent palette on dark themes and Dark 1
    // on light ones. (Not the base accent, and not the darker "menu" accent.)
    DWORD lightApps = 0;
    {
        DWORD sz = sizeof(lightApps);
        RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &lightApps, &sz);
    }

    BYTE pal[32] = {};
    DWORD palSize = sizeof(pal);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Accent",
                     L"AccentPalette", RRF_RT_REG_BINARY, nullptr, pal, &palSize) == ERROR_SUCCESS &&
        palSize >= 20) {
        const int i = lightApps ? 4 : 1;  // Dark 1 : Light 2 (RGBA, lightest first)
        g_accR = pal[i * 4];
        g_accG = pal[i * 4 + 1];
        g_accB = pal[i * 4 + 2];
        return;
    }

    // No palette (unusual): the accent color of the menus.
    DWORD v = 0;
    DWORD vsz = sizeof(v);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Accent",
                     L"AccentColorMenu", RRF_RT_REG_DWORD, nullptr, &v, &vsz) == ERROR_SUCCESS) {
        g_accR = static_cast<BYTE>(v & 0xFF);
        g_accG = static_cast<BYTE>((v >> 8) & 0xFF);
        g_accB = static_cast<BYTE>((v >> 16) & 0xFF);
    }
}


// Changes only the opacity of the (already drawn) outline.
static void SetHighlightAlpha(BYTE a) {
    g_hlAlpha = a;
    if (!g_highlight) {
        return;
    }
    BLENDFUNCTION bf{ AC_SRC_OVER, 0, a, AC_SRC_ALPHA };
    UpdateLayeredWindow(g_highlight, nullptr, nullptr, nullptr, nullptr, nullptr,
                        0, &bf, ULW_ALPHA);
}

static void StartHighlightFade(BYTE from, BYTE to, double durMs, bool hideAtEnd) {
    g_hlFrom = from;
    g_hlTo = to;
    g_hlStartMs = NowMs();
    g_hlDurMs = g_reduceMotion ? 1.0 : durMs;
    g_hlHideAtEnd = hideAtEnd;
    if (g_overlay) {
        SetTimer(g_overlay, HL_TIMER_ID, 10, nullptr);
    }
}

static void StepHighlightFade() {
    const float t = static_cast<float>((NowMs() - g_hlStartMs) / g_hlDurMs);
    const float k = Smooth01(t);
    const float a = static_cast<float>(g_hlFrom) +
                    (static_cast<float>(g_hlTo) - static_cast<float>(g_hlFrom)) * k;
    SetHighlightAlpha(static_cast<BYTE>(a + 0.5f));

    if (t >= 1.0f) {
        if (g_overlay) {
            KillTimer(g_overlay, HL_TIMER_ID);
        }
        if (g_hlHideAtEnd && g_highlight) {
            ShowWindow(g_highlight, SW_HIDE);
        }
    }
}

// `fade`: let it dissolve instead of vanishing (used when the pointer just
// leaves a tile). Everything else hides it right away.
static void HideHighlight(bool fade = false) {
    g_hover = -1;
    if (!g_highlight) {
        return;
    }
    if (fade && IsWindowVisible(g_highlight) && g_hlAlpha > 0) {
        StartHighlightFade(g_hlAlpha, 0, HL_FADE_OUT_MS, true);
        return;
    }
    if (g_overlay) {
        KillTimer(g_overlay, HL_TIMER_ID);
    }
    g_hlAlpha = 0;
    ShowWindow(g_highlight, SW_HIDE);
}

static void ShowHighlight(const RECT& tile, bool redraw = false) {
    if (!g_highlight) {
        return;
    }

    const int tw = tile.right - tile.left;
    const int th = tile.bottom - tile.top;
    const int w = tw + 2 * HL_PAD;
    const int h = th + 2 * HL_PAD;
    if (tw <= 0 || th <= 0) {
        return;
    }

    HDC screen = GetDC(nullptr);
    HDC mem = CreateCompatibleDC(screen);

    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;  // top-down
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP bmp = CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);

    if (bmp && bits) {
        HGDIOBJ old = SelectObject(mem, bmp);
        uint32_t* px = static_cast<uint32_t*>(bits);

        const float cx = w * 0.5f;
        const float cy = h * 0.5f;
        const float bx = tw * 0.5f + HL_GAP + HL_THICK;
        const float by = th * 0.5f + HL_GAP + HL_THICK;
        const float rad = static_cast<float>(HL_RADIUS);
        const float half = HL_THICK * 0.5f;

        // Only the band around the border (ring + glow) is ever non-transparent,
        // so skip the deep interior instead of evaluating every pixel.
        const float skipHalf = (bx - rad) - (HL_THICK + 2);
        for (int y = 0; y < h; y++) {
            const bool deepRow =
                std::fabs(y + 0.5f - cy) - (by - rad) < -(HL_THICK + 2);
            for (int x = 0; x < w; x++) {
                if (deepRow && std::fabs(x + 0.5f - cx) < skipHalf) {
                    x = static_cast<int>(std::ceil(cx + skipHalf - 0.5f)) - 1;
                    continue;
                }
                // Signed distance to a rounded rectangle (negative inside).
                const float dx = std::fabs(x + 0.5f - cx) - (bx - rad);
                const float dy = std::fabs(y + 0.5f - cy) - (by - rad);
                const float ox = dx > 0 ? dx : 0;
                const float oy = dy > 0 ? dy : 0;
                float m = dx > dy ? dx : dy;
                if (m > 0) m = 0;
                const float sdf = std::sqrt(ox * ox + oy * oy) + m - rad;

                // A flat, anti-aliased ring just inside the boundary, with no
                // glow: like the selection outline of Windows 11's Alt+Tab.
                const float a = Clamp01(0.5f - (std::fabs(sdf + half) - half));

                const uint32_t A = static_cast<uint32_t>(a * 255.0f + 0.5f);
                const uint32_t R = g_accR * A / 255;
                const uint32_t G = g_accG * A / 255;
                const uint32_t B = g_accB * A / 255;
                px[y * w + x] = (A << 24) | (R << 16) | (G << 8) | B;  // premultiplied
            }
        }

        // Close button in the top-right corner of the window under the mouse:
        // dark glass, red when the mouse is on it, with a white X.
        if (CLOSE_BUTTON && tw >= 90 && th >= 60) {
            const float bcx = static_cast<float>(HL_PAD + tw - CB_INSET - CB_RADIUS);
            const float bcy = static_cast<float>(HL_PAD + CB_INSET + CB_RADIUS);
            const float fr = g_hoverClose ? 232.0f : 32.0f;
            const float fg = g_hoverClose ? 17.0f : 32.0f;
            const float fb = g_hoverClose ? 35.0f : 36.0f;
            const float fillA = g_hoverClose ? 1.0f : 0.82f;
            const float xext = 5.0f * static_cast<float>(CB_RADIUS) / 13.0f;
            const int bx0 = static_cast<int>(bcx) - CB_RADIUS - 2;
            const int bx1 = static_cast<int>(bcx) + CB_RADIUS + 2;
            const int by0 = static_cast<int>(bcy) - CB_RADIUS - 2;
            const int by1 = static_cast<int>(bcy) + CB_RADIUS + 2;
            for (int y = by0; y <= by1; y++) {
                if (y < 0 || y >= h) {
                    continue;
                }
                for (int x = bx0; x <= bx1; x++) {
                    if (x < 0 || x >= w) {
                        continue;
                    }
                    const float ddx = x + 0.5f - bcx;
                    const float ddy = y + 0.5f - bcy;
                    const float cov = Clamp01(static_cast<float>(CB_RADIUS) + 0.5f -
                                              std::sqrt(ddx * ddx + ddy * ddy));
                    if (cov <= 0.0f) {
                        continue;
                    }
                    // White X: distance to the two diagonals, near the center only.
                    float xa = 0.0f;
                    if (std::fabs(ddx) <= xext && std::fabs(ddy) <= xext) {
                        const float d1 = std::fabs(ddx - ddy) * 0.70711f;
                        const float d2 = std::fabs(ddx + ddy) * 0.70711f;
                        xa = Clamp01(1.4f - (d1 < d2 ? d1 : d2));
                    }
                    const float ca = fillA * cov;
                    const float outA = xa + ca * (1.0f - xa);
                    const float outR = xa + (fr / 255.0f) * ca * (1.0f - xa);
                    const float outG = xa + (fg / 255.0f) * ca * (1.0f - xa);
                    const float outB = xa + (fb / 255.0f) * ca * (1.0f - xa);
                    px[y * w + x] =
                        (static_cast<uint32_t>(outA * 255.0f + 0.5f) << 24) |
                        (static_cast<uint32_t>(outR * 255.0f + 0.5f) << 16) |
                        (static_cast<uint32_t>(outG * 255.0f + 0.5f) << 8) |
                        static_cast<uint32_t>(outB * 255.0f + 0.5f);
                }
            }
        }

        POINT dst{ tile.left - HL_PAD, tile.top - HL_PAD };
        SIZE size{ w, h };
        POINT src{ 0, 0 };

        // Fade in. Coming from another tile it starts partly visible so the
        // outline doesn't blink out and back in.
        const BYTE startAlpha =
            redraw ? g_hlAlpha
                   : ((IsWindowVisible(g_highlight) && g_hlAlpha > 0) ? 70 : 0);
        BLENDFUNCTION bf{ AC_SRC_OVER, 0, startAlpha, AC_SRC_ALPHA };

        UpdateLayeredWindow(g_highlight, screen, &dst, &size, mem, &src, 0, &bf, ULW_ALPHA);
        g_hlAlpha = startAlpha;
        SetWindowPos(g_highlight, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
        if (!redraw) {
            StartHighlightFade(startAlpha, 255, HL_FADE_IN_MS, false);
        }

        SelectObject(mem, old);
    }

    if (bmp) DeleteObject(bmp);
    DeleteDC(mem);
    ReleaseDC(nullptr, screen);
}

// Is the point on the close button of this tile (overlay client coordinates)?
static bool OnCloseButton(const Tile& t, POINT pt) {
    if ((t.to.right - t.to.left) < 90 || (t.to.bottom - t.to.top) < 60) {
        return false;
    }
    const int cx = t.to.right - CB_INSET - CB_RADIUS;
    const int cy = t.to.top + CB_INSET + CB_RADIUS;
    const int dx = pt.x - cx;
    const int dy = pt.y - cy;
    const int r = CB_RADIUS + 3;
    return dx * dx + dy * dy <= r * r;
}

static void UpdateHover(int x, int y) {
    // Over the desktops bar: the card reacts, there is no window ring.
    if (g_barBuilt && y >= g_barTop) {
        const int bi = HitBarCard(x, y);
        if (bi != g_barHover) {
            g_barHover = bi;
            SetBarHover(bi);
        }
        SetBarCloseHover(HitBarClose(x, y));
        if (g_hover != -1) {
            g_hoverClose = false;
            HideHighlight(true);
        }
        return;
    }
    if (g_barHover != -1) {
        g_barHover = -1;
        SetBarHover(-1);
    }
    SetBarCloseHover(-1);

    int idx = -1;
    POINT pt{ x, y };
    for (size_t i = 0; i < g_tiles.size(); i++) {
        if (TileContains(g_tiles[i], pt)) {
            idx = static_cast<int>(i);
            break;
        }
    }

    const bool onClose = idx >= 0 && CLOSE_BUTTON && OnCloseButton(g_tiles[idx], pt);

    if (idx == g_hover) {
        if (idx >= 0 && onClose != g_hoverClose) {
            // Same tile, the mouse just entered / left the close button.
            g_hoverClose = onClose;
            RECT r = g_tiles[idx].to;
            OffsetRect(&r, g_workRect.left, g_workRect.top);
            ShowHighlight(r, true);
        }
        return;
    }

    if (idx < 0) {
        g_hoverClose = false;
        HideHighlight(true);
        return;
    }

    g_hover = idx;
    g_hoverClose = onClose;
    RECT r = g_tiles[idx].to;
    OffsetRect(&r, g_workRect.left, g_workRect.top);
    ShowHighlight(r);
}

// ---------------------------------------------------------------------------
// Blurred wallpaper background (mode 2)
// ---------------------------------------------------------------------------

static void FreeBackground() {
    if (g_bgDC) {
        if (g_bgOld) {
            SelectObject(g_bgDC, g_bgOld);
        }
        DeleteDC(g_bgDC);
        g_bgDC = nullptr;
    }
    g_bgOld = nullptr;
    if (g_bgBmp) {
        DeleteObject(g_bgBmp);
        g_bgBmp = nullptr;
    }
    g_bgW = 0;
    g_bgH = 0;
    g_bgKey.clear();
    g_wallpaperOk = false;
}

static std::wstring GetWallpaperPath() {
    wchar_t buf[MAX_PATH] = {};
    SystemParametersInfoW(SPI_GETDESKWALLPAPER, MAX_PATH, buf, 0);
    if (buf[0] && GetFileAttributesW(buf) != INVALID_FILE_ATTRIBUTES) {
        return buf;
    }

    // Slideshow / odd cases: Windows keeps a decoded copy here.
    wchar_t appdata[MAX_PATH] = {};
    if (GetEnvironmentVariableW(L"APPDATA", appdata, MAX_PATH)) {
        std::wstring p = std::wstring(appdata) +
            L"\\Microsoft\\Windows\\Themes\\TranscodedWallpaper";
        if (GetFileAttributesW(p.c_str()) != INVALID_FILE_ATTRIBUTES) {
            return p;
        }
    }
    return L"";
}

// Three box-blur passes ~ a gaussian. Works on tightly packed BGRA.
static void BoxBlur(std::vector<uint8_t>& px, int w, int h, int r) {
    std::vector<uint8_t> tmp(px.size());
    const int div = 2 * r + 1;

    for (int pass = 0; pass < 3; pass++) {
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                int sum[3] = { 0, 0, 0 };
                for (int k = -r; k <= r; k++) {
                    int xx = x + k;
                    xx = xx < 0 ? 0 : (xx >= w ? w - 1 : xx);
                    const uint8_t* p = &px[(y * w + xx) * 4];
                    sum[0] += p[0];
                    sum[1] += p[1];
                    sum[2] += p[2];
                }
                uint8_t* o = &tmp[(y * w + x) * 4];
                o[0] = static_cast<uint8_t>(sum[0] / div);
                o[1] = static_cast<uint8_t>(sum[1] / div);
                o[2] = static_cast<uint8_t>(sum[2] / div);
                o[3] = 255;
            }
        }
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                int sum[3] = { 0, 0, 0 };
                for (int k = -r; k <= r; k++) {
                    int yy = y + k;
                    yy = yy < 0 ? 0 : (yy >= h ? h - 1 : yy);
                    const uint8_t* p = &tmp[(yy * w + x) * 4];
                    sum[0] += p[0];
                    sum[1] += p[1];
                    sum[2] += p[2];
                }
                uint8_t* o = &px[(y * w + x) * 4];
                o[0] = static_cast<uint8_t>(sum[0] / div);
                o[1] = static_cast<uint8_t>(sum[1] / div);
                o[2] = static_cast<uint8_t>(sum[2] / div);
                o[3] = 255;
            }
        }
    }
}

// Decodes the wallpaper, crops it like the "Fill" fit mode, blurs it and keeps
// the result in a DIB the size of the overlay. Cached until the wallpaper or
// the monitor size changes.
static bool PrepareBackground(int W, int H) {
    if (W <= 0 || H <= 0) {
        return false;
    }

    const std::wstring path = GetWallpaperPath();
    if (path.empty()) {
        FreeBackground();
        return false;
    }

    WIN32_FILE_ATTRIBUTE_DATA fa{};
    GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &fa);
    wchar_t tail[96];
    swprintf(tail, 96, L"|%lu|%lu|%d|%d",
             static_cast<unsigned long>(fa.ftLastWriteTime.dwHighDateTime),
             static_cast<unsigned long>(fa.ftLastWriteTime.dwLowDateTime), W, H);
    const std::wstring key = path + tail;

    if (g_bgBmp && key == g_bgKey) {
        return true;
    }

    FreeBackground();

    try {
        winrt::com_ptr<IWICImagingFactory> factory;
        winrt::check_hresult(CoCreateInstance(
            CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
            IID_IWICImagingFactory, factory.put_void()));

        winrt::com_ptr<IWICBitmapDecoder> decoder;
        winrt::check_hresult(factory->CreateDecoderFromFilename(
            path.c_str(), nullptr, GENERIC_READ,
            WICDecodeMetadataCacheOnDemand, decoder.put()));

        winrt::com_ptr<IWICBitmapFrameDecode> frame;
        winrt::check_hresult(decoder->GetFrame(0, frame.put()));

        UINT sw = 0, sh = 0;
        winrt::check_hresult(frame->GetSize(&sw, &sh));
        if (sw == 0 || sh == 0) {
            return false;
        }

        // "Fill": scale to cover the monitor and crop the center.
        UINT cw = sw, ch = sh;
        const double srcAspect = static_cast<double>(sw) / sh;
        const double dstAspect = static_cast<double>(W) / H;
        if (srcAspect > dstAspect) {
            cw = static_cast<UINT>(sh * dstAspect + 0.5);
        } else {
            ch = static_cast<UINT>(sw / dstAspect + 0.5);
        }
        if (cw == 0) cw = 1;
        if (ch == 0) ch = 1;

        WICRect clip{ static_cast<INT>((sw - cw) / 2), static_cast<INT>((sh - ch) / 2),
                      static_cast<INT>(cw), static_cast<INT>(ch) };

        winrt::com_ptr<IWICBitmapClipper> clipper;
        winrt::check_hresult(factory->CreateBitmapClipper(clipper.put()));
        winrt::check_hresult(clipper->Initialize(frame.get(), &clip));

        const UINT smallW = static_cast<UINT>(W / WALLPAPER_SHRINK) < 8
                                ? 8 : static_cast<UINT>(W / WALLPAPER_SHRINK);
        const UINT smallH = static_cast<UINT>(H / WALLPAPER_SHRINK) < 8
                                ? 8 : static_cast<UINT>(H / WALLPAPER_SHRINK);

        winrt::com_ptr<IWICBitmapScaler> down;
        winrt::check_hresult(factory->CreateBitmapScaler(down.put()));
        winrt::check_hresult(down->Initialize(
            clipper.get(), smallW, smallH, WICBitmapInterpolationModeFant));

        winrt::com_ptr<IWICFormatConverter> conv;
        winrt::check_hresult(factory->CreateFormatConverter(conv.put()));
        winrt::check_hresult(conv->Initialize(
            down.get(), GUID_WICPixelFormat32bppBGRA, WICBitmapDitherTypeNone,
            nullptr, 0.0, WICBitmapPaletteTypeCustom));

        std::vector<uint8_t> blurBuf(static_cast<size_t>(smallW) * smallH * 4);
        winrt::check_hresult(conv->CopyPixels(
            nullptr, smallW * 4, static_cast<UINT>(blurBuf.size()), blurBuf.data()));

        BoxBlur(blurBuf, static_cast<int>(smallW), static_cast<int>(smallH),
                WALLPAPER_BLUR_RADIUS);

        for (size_t i = 0; i < blurBuf.size(); i += 4) {
            blurBuf[i + 0] = static_cast<uint8_t>(blurBuf[i + 0] * WALLPAPER_DIM);
            blurBuf[i + 1] = static_cast<uint8_t>(blurBuf[i + 1] * WALLPAPER_DIM);
            blurBuf[i + 2] = static_cast<uint8_t>(blurBuf[i + 2] * WALLPAPER_DIM);
        }

        // Back up to full size; the smooth interpolation hides the shrink.
        winrt::com_ptr<IWICBitmap> smallBmp;
        winrt::check_hresult(factory->CreateBitmapFromMemory(
            smallW, smallH, GUID_WICPixelFormat32bppBGRA, smallW * 4,
            static_cast<UINT>(blurBuf.size()), blurBuf.data(), smallBmp.put()));

        winrt::com_ptr<IWICBitmapScaler> up;
        winrt::check_hresult(factory->CreateBitmapScaler(up.put()));
        winrt::check_hresult(up->Initialize(
            smallBmp.get(), static_cast<UINT>(W), static_cast<UINT>(H),
            WICBitmapInterpolationModeCubic));

        HDC screen = GetDC(nullptr);

        BITMAPINFO bi{};
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = W;
        bi.bmiHeader.biHeight = -H;  // top-down
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;

        void* bits = nullptr;
        HBITMAP bmp = CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
        if (!bmp || !bits) {
            ReleaseDC(nullptr, screen);
            return false;
        }

        const HRESULT hr = up->CopyPixels(
            nullptr, static_cast<UINT>(W) * 4,
            static_cast<UINT>(W) * 4 * static_cast<UINT>(H),
            static_cast<BYTE*>(bits));
        if (FAILED(hr)) {
            DeleteObject(bmp);
            ReleaseDC(nullptr, screen);
            return false;
        }

        g_bgDC = CreateCompatibleDC(screen);
        g_bgOld = SelectObject(g_bgDC, bmp);
        g_bgBmp = bmp;
        g_bgW = W;
        g_bgH = H;
        g_bgKey = key;
        ReleaseDC(nullptr, screen);

        Wh_Log(L"Wallpaper background ready (%dx%d): %s", W, H, path.c_str());
        return true;
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Wallpaper background failed: 0x%08X", static_cast<unsigned>(e.code()));
    } catch (...) {
        Wh_Log(L"Wallpaper background failed (unknown error).");
    }

    FreeBackground();
    return false;
}

static bool SetAccentState(int state, DWORD color) {
    // Undocumented but widely used (acrylic / blur behind).
    struct AccentPolicy { int state; int flags; DWORD color; int anim; };
    struct WcaData { DWORD attr; PVOID data; ULONG size; };
    using SetWcaFn = BOOL(WINAPI*)(HWND, WcaData*);

    auto setWca = reinterpret_cast<SetWcaFn>(reinterpret_cast<void*>(
        GetProcAddress(GetModuleHandleW(L"user32.dll"),
                       "SetWindowCompositionAttribute")));
    if (!setWca) {
        return false;
    }

    AccentPolicy ap{ state, 2, color, 0 };
    WcaData d{ 19 /*ACCENT_POLICY*/, &ap, sizeof(ap) };
    return setWca(g_overlay, &d) != FALSE;
}

// Makes DWM draw the overlay as the active window even when another window is
// the foreground one (undocumented attribute 15, FORCE_ACTIVEWINDOW_APPEARANCE).
static void ForceOverlayActiveLook() {
    struct WcaData { DWORD attr; PVOID data; ULONG size; };
    using SetWcaFn = BOOL(WINAPI*)(HWND, WcaData*);

    auto setWca = reinterpret_cast<SetWcaFn>(reinterpret_cast<void*>(
        GetProcAddress(GetModuleHandleW(L"user32.dll"),
                       "SetWindowCompositionAttribute")));
    if (!setWca || !g_overlay) {
        return;
    }

    BOOL on = TRUE;
    WcaData d{ 15, &on, sizeof(on) };
    setWca(g_overlay, &d);
}

// Backdrop / theme setup. Re-run on every show so it follows light/dark changes.
static void ApplyBackdropStyle(bool wallpaper) {
    DWORD appsLight = 1;
    DWORD size = sizeof(appsLight);
    RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &appsLight, &size);

    BOOL dark = appsLight ? FALSE : TRUE;
    g_darkTheme = dark != FALSE;
    DwmSetWindowAttribute(g_overlay, 20 /*USE_IMMERSIVE_DARK_MODE*/, &dark, sizeof(dark));
    ForceOverlayActiveLook();

    int corner = 1;  // DWMWCP_DONOTROUND: full-screen, no rounded corners
    DwmSetWindowAttribute(g_overlay, 33 /*WINDOW_CORNER_PREFERENCE*/, &corner, sizeof(corner));

    COLORREF noBorder = 0xFFFFFFFE;  // DWMWA_COLOR_NONE
    DwmSetWindowAttribute(g_overlay, 34 /*BORDER_COLOR*/, &noBorder, sizeof(noBorder));

    g_micaOk = false;

    if (wallpaper) {
        // Opaque, self-painted background: no system material underneath.
        MARGINS none{ 0, 0, 0, 0 };
        DwmExtendFrameIntoClientArea(g_overlay, &none);
        int off = 1;  // DWMSBT_NONE
        DwmSetWindowAttribute(g_overlay, 38 /*SYSTEMBACKDROP_TYPE*/, &off, sizeof(off));
        SetAccentState(0 /*DISABLED*/, 0);
        return;
    }

    MARGINS m{ -1, -1, -1, -1 };
    DwmExtendFrameIntoClientArea(g_overlay, &m);

    if (BACKDROP_MODE >= 1) {
        const DWORD a = static_cast<DWORD>(BACKDROP_TINT_ALPHA);
        const DWORD rgb = dark ? 0x202020 : 0xF3F3F3;  // r=g=b, so ABGR == ARGB
        g_micaOk = SetAccentState(4 /*ACRYLICBLURBEHIND*/, (a << 24) | rgb);
    }

    if (!g_micaOk) {
        int type = BACKDROP_TYPE;
        g_micaOk = SUCCEEDED(
            DwmSetWindowAttribute(g_overlay, 38 /*SYSTEMBACKDROP_TYPE*/, &type, sizeof(type)));
    }

    if (!g_micaOk) {
        Wh_Log(L"System backdrop not available, falling back to solid color.");
    }
}

static void ShowOverlay() {
    UpdateReducedMotion();

    POINT pt{};
    GetCursorPos(&pt);

    HMONITOR mon = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    GetMonitorInfoW(mon, &mi);

    // Full monitor (taskbar included) so windows can use the whole screen.
    g_workRect = mi.rcMonitor;

    g_prevForeground = GetForegroundWindow();

    ReadAccentColor();

    SetLayeredWindowAttributes(g_overlay, 0, 0, LWA_ALPHA);
    g_lastAlpha = 0;

    SetWindowPos(
        g_overlay, HWND_TOPMOST,
        g_workRect.left, g_workRect.top,
        g_workRect.right - g_workRect.left,
        g_workRect.bottom - g_workRect.top,
        SWP_SHOWWINDOW);
    SetForegroundWindow(g_overlay);

    // Lay out against what the window REALLY got (DPI scaling or DWM can
    // make it differ from the monitor rect we asked for).
    RECT cr{};
    GetClientRect(g_overlay, &cr);
    POINT origin{ 0, 0 };
    ClientToScreen(g_overlay, &origin);
    Wh_Log(L"Overlay: monitor=(%ld,%ld,%ld,%ld) client=%ldx%ld origin=(%ld,%ld)",
           g_workRect.left, g_workRect.top, g_workRect.right, g_workRect.bottom,
           cr.right, cr.bottom, origin.x, origin.y);
    g_workRect = { origin.x, origin.y, origin.x + cr.right, origin.y + cr.bottom };

    g_wallpaperOk = (BACKDROP_MODE == 2) && PrepareBackground(cr.right, cr.bottom);
    ApplyBackdropStyle(g_wallpaperOk);
    InvalidateRect(g_overlay, nullptr, TRUE);

    // After a driver update or a GPU reset (TDR) the device is gone and every
    // surface made from it fails: start DirectComposition over. Nothing of the
    // old device is in use here, the previous gesture released it all.
    if (g_dcOk && g_d3d && FAILED(g_d3d->GetDeviceRemovedReason())) {
        Wh_Log(L"DComp: the GPU device was lost (0x%08X), recreating it",
               static_cast<unsigned>(g_d3d->GetDeviceRemovedReason()));
        ShutdownDComp();
        InitDComp();
    }

    if (g_dcOk && g_thumbHost) {
        // Same rect as the overlay, on top of it, never activated.
        SetWindowPos(g_thumbHost, HWND_TOPMOST,
                     g_workRect.left, g_workRect.top,
                     g_workRect.right - g_workRect.left,
                     g_workRect.bottom - g_workRect.top,
                     SWP_SHOWWINDOW | SWP_NOACTIVATE);
    }

    UpdateUiScale(g_workRect.bottom - g_workRect.top, MonitorDpi(mon));
    PurgeDeadWindowIcons();
    BuildTiles();
    g_visible = true;
    BeginTaskbarSlide();
    ApplyProgress(g_progress);
}

// ---------------------------------------------------------------------------
// Animation clock (vsync paced, with a plain timer as fallback)
// ---------------------------------------------------------------------------

static DWORD WINAPI ClockThreadProc(void*) {
    while (!g_clockQuit) {
        WaitForSingleObject(g_clockWake, INFINITE);  // idle: costs nothing
        while (g_animActive && !g_clockQuit) {
            if (FAILED(DwmFlush())) {
                Sleep(8);  // composition not available: don't spin
            }
            if (g_animActive && g_overlay &&
                InterlockedExchange(&g_framePending, 1) == 0) {
                if (!PostMessageW(g_overlay, WM_APP_FRAME, 0, 0)) {
                    InterlockedExchange(&g_framePending, 0);
                }
            }
        }
    }
    return 0;
}

static void StartClockThread() {
    g_clockQuit = 0;
    g_clockWake = CreateEventW(nullptr, TRUE, FALSE, nullptr);  // manual reset
    if (g_clockWake) {
        g_clockThread = CreateThread(nullptr, 0, ClockThreadProc, nullptr, 0, nullptr);
    }
    if (!g_clockThread) {
        Wh_Log(L"Vsync clock unavailable, using the timer fallback.");
    }
}

static void StopClockThread() {
    if (g_clockThread) {
        InterlockedExchange(&g_clockQuit, 1);
        InterlockedExchange(&g_animActive, 0);
        SetEvent(g_clockWake);
        // It wakes up within a frame; it must be gone before the mod is unloaded.
        WaitForSingleObject(g_clockThread, INFINITE);
        CloseHandle(g_clockThread);
        g_clockThread = nullptr;
    }
    if (g_clockWake) {
        CloseHandle(g_clockWake);
        g_clockWake = nullptr;
    }
}

static void StartAnimClock() {
    if (g_clockThread) {
        InterlockedExchange(&g_animActive, 1);
        SetEvent(g_clockWake);
    } else if (g_overlay) {
        SetTimer(g_overlay, ANIM_TIMER_ID, ANIM_INTERVAL_MS, nullptr);
    }
}

static void StopAnimClock() {
    if (g_clockThread) {
        InterlockedExchange(&g_animActive, 0);
        ResetEvent(g_clockWake);
    }
    if (g_overlay) {
        KillTimer(g_overlay, ANIM_TIMER_ID);
    }
}

static void HideOverlay(bool restoreFocus = false) {
    StopAnimClock();
    g_animVel = 0.0f;

    // The real taskbar comes back first; its copy is removed below, in the
    // same frame as the overlay disappears, so there is no gap.
    UncloakTaskbars();

    // Same reason as in StepAnimation: bring the previous window back BEFORE
    // hiding the overlay, so no other window gets activated in between.
    if (restoreFocus && g_prevForeground && IsWindow(g_prevForeground)) {
        SetForegroundWindow(g_prevForeground);
    }

    if (g_overlay) {
        KillTimer(g_overlay, RENDER_TIMER_ID);
        KillTimer(g_overlay, BAR_TIMER_ID);
        if (g_pendingCloses.empty()) {
            KillTimer(g_overlay, CLOSE_POLL_TIMER_ID);
        }
        ShowWindow(g_overlay, SW_HIDE);
    }
    RestoreTaskbars();
    HideHighlight();
    g_trackingMouse = false;
    UnregisterTiles();
    if (g_thumbHost) {
        ShowWindow(g_thumbHost, SW_HIDE);
    }

    g_prevForeground = nullptr;

    g_renderDirty = false;
    g_renderTimerSet = false;
    g_closeTarget = nullptr;
    g_restoreIssued = false;
    g_holdTiles = false;
    g_lastHideMs = NowMs();
    g_kbSel = -1;
    g_hoverClose = false;
    g_relayoutActive = false;
    g_lastMouse = POINT{ -1, -1 };

    g_visible = false;
    g_open = false;
    g_dragging = false;
    g_progress = 0.0f;
    g_target = 0.0f;
    SetClickThrough(true);
}

// Presses Win+Ctrl+<key> `times` times: the shortcuts Windows itself has for
// virtual desktops (arrows switch, D creates one, F4 closes the current one).
static void SendWinCtrlKey(WORD vk, int times) {
    std::vector<INPUT> in;
    auto key = [&](WORD k, bool up) {
        INPUT i{};
        i.type = INPUT_KEYBOARD;
        i.ki.wVk = k;
        i.ki.wScan = static_cast<WORD>(MapVirtualKeyW(k, MAPVK_VK_TO_VSC));
        i.ki.dwFlags = (up ? KEYEVENTF_KEYUP : 0) |
                       ((k == VK_LEFT || k == VK_RIGHT || k == VK_LWIN) ? KEYEVENTF_EXTENDEDKEY : 0);
        in.push_back(i);
    };

    key(VK_LWIN, false);
    key(VK_LCONTROL, false);
    for (int i = 0; i < times && i < 16; i++) {
        key(vk, false);
        key(vk, true);
    }
    key(VK_LCONTROL, true);
    key(VK_LWIN, true);

    SendInput(static_cast<UINT>(in.size()), in.data(), sizeof(INPUT));
}

// Runs what a click on the desktops bar asked for, once the overlay is gone:
// 1 = create a desktop, 2 = switch to another one, 3 = close one.
static void RunPendingDesktopAction() {
    const int action = g_pendingDesktop;
    const int delta = g_pendingDesktopDelta;
    g_pendingDesktop = 0;
    g_pendingDesktopDelta = 0;
    if (action == 0) {
        return;
    }

    if (action == 1) {
        SendWinCtrlKey('D', 1);
        return;
    }

    const WORD arrow = delta > 0 ? VK_RIGHT : VK_LEFT;
    const int n = std::abs(delta);
    if (n > 0) {
        SendWinCtrlKey(arrow, n);
    }
    if (action == 3 && g_overlay) {
        // Close it once Windows has finished switching to it.
        SetTimer(g_overlay, DESKTOP_CLOSE_TIMER_ID, n > 0 ? 450 : 120, nullptr);
    }
}

// Starts the spring toward `target` (1 = open, 0 = closed) from the current
// progress, carrying `velocity` (progress units / second) over from the fingers.
static void StartSpring(float target, float velocity) {
    g_target = target;
    g_animVel = velocity;
    g_animStartMs = NowMs();
    g_animLastMs = g_animStartMs;
    StartAnimClock();
}

static void StepAnimation() {
    const double now = NowMs();
    double dt = (now - g_animLastMs) / 1000.0;
    g_animLastMs = now;
    if (dt > 0.05) {
        dt = 0.05;  // after a hitch, don't fast-forward the physics
    }

    StepRelayout(now);

    const bool opening = g_target >= 1.0f;
    // With animations off in Windows the spring becomes a stiff, critically
    // damped one: it reaches the end in a couple of frames, with no bounce.
    const float w = g_reduceMotion ? 240.0f : (opening ? SPRING_OMEGA : CLOSE_OMEGA);
    const float zeta = (g_reduceMotion || !opening) ? 1.0f : SPRING_DAMPING;
    const float k = w * w;
    // Closing aims a little past the end (see CLOSE_AIM_PAST_ZERO): it arrives
    // with some speed instead of crawling the last pixels.
    const float aim = opening ? g_target : -CLOSE_AIM_PAST_ZERO;
    const float c = 2.0f * zeta * w;

    // Semi-implicit Euler in small fixed steps: stable and frame-rate independent.
    int steps = static_cast<int>(std::ceil(dt / 0.002));
    if (steps < 1) {
        steps = 1;
    }
    const float h = static_cast<float>(dt / steps);

    float x = g_progress;
    float v = g_animVel;
    for (int i = 0; i < steps; i++) {
        const float a = -k * (x - aim) - c * v;
        v += a * h;
        x += v * h;
    }

    const double elapsed = now - g_animStartMs;
    bool done;
    if (opening) {
        done = (std::fabs(x - 1.0f) < 0.0015f && std::fabs(v) < 0.03f) ||
               elapsed > 1500.0;
    } else {
        done = x <= 0.0f || elapsed > 1500.0;
    }

    if (done) {
        g_animVel = 0.0f;
        ApplyProgress(g_target);

        if (g_target > 0.0f) {
            if (!g_relayoutActive && g_barThumbQueue.empty()) {  // the clock keeps running while the grid glides / thumbnails appear
                StopAnimClock();
            }
            return;
        }

        // Closing is finished. Grab the pick before HideOverlay() clears it.
        HWND picked = g_closeTarget;
        if (picked && IsWindow(picked)) {
            if (IsIconic(picked)) {
                // Minimized pick: restore it, and keep the (static) tiles up
                // until it is back, otherwise there would be a flash of
                // whatever is behind it.
                if (!g_restoreIssued) {
                    g_restoreIssued = true;
                    g_restoreMs = now;
                    ShowWindowAsync(picked, SW_RESTORE);
                }
                if (now - g_restoreMs < RESTORE_TIMEOUT_MS) {
                    return;  // the clock keeps running
                }
            } else if (g_restoreIssued && now - g_restoreMs < RESTORE_SETTLE_MS) {
                return;  // restored; let Windows' restore animation finish under the tiles
            }

            // Activate (and raise) the pick while the overlay still covers
            // the screen. If the overlay is hidden first, Windows activates
            // whatever window is next in z-order, and that window flashes for
            // a frame before the pick comes up.
            SetForegroundWindow(ResolvePickTarget(picked));
        }
        StopAnimClock();
        HideOverlay(picked == nullptr);
        RunPendingDesktopAction();  // switching / creating a desktop, if a bar card was clicked
        return;
    }

    g_animVel = v;
    ApplyProgress(x);
}

// Re-registers a tile's thumbnail so DWM draws it on top of the other tiles.
// The new thumbnail is created and positioned BEFORE the old one is removed,
// so there is no frame without it. Stays live: it is still a DWM thumbnail.
static void BringTileToFront(Tile& t) {
    if (t.dcVisual && g_dcTiles && g_dcDevice) {
        auto* v = t.dcVisual;

        // A normal window goes above every normal window but stays below the
        // always-on-top ones, exactly like the real z-order will be once it is
        // activated. (The lowest always-on-top tile is the last one in the list.)
        IDCompositionVisual2* below = nullptr;
        if (!t.topmost) {
            for (size_t i = g_tiles.size(); i-- > 0;) {
                const Tile& o = g_tiles[i];
                if (&o != &t && o.topmost && !o.minimized && o.dcVisual) {
                    below = o.dcVisual;
                    break;
                }
            }
        }

        // Already on top (the owner or one of its dialogs is the top-most visual)?
        bool groupIsTop = (v == g_dcTop);
        for (auto& k : t.kids) {
            if (k.dcVisual && k.dcVisual == g_dcTop) {
                groupIsTop = true;
            }
        }
        if (!below && groupIsTop) {
            return;
        }

        g_dcTiles->RemoveVisual(v);
        for (auto& k : t.kids) {
            if (k.dcVisual) {
                g_dcTiles->RemoveVisual(k.dcVisual);
            }
        }

        if (below) {
            g_dcTiles->AddVisual(v, FALSE, below);   // just under the always-on-top ones
        } else {
            g_dcTiles->AddVisual(v, TRUE, g_dcTop);  // on top of everything
        }

        // Its dialogs go right above it, bottom-most first.
        IDCompositionVisual2* prev = v;
        for (size_t k = t.kids.size(); k-- > 0;) {
            if (!t.kids[k].dcVisual) {
                continue;
            }
            auto* kv = t.kids[k].dcVisual;
            g_dcTiles->AddVisual(kv, TRUE, prev);
            prev = kv;
        }
        if (!below) {
            g_dcTop = prev;
        }
        g_dcDevice->Commit();
        return;
    }
    if (!t.thumb) {
        return;
    }

    const RECT r = LerpRect(t.from, t.to, g_progress);

    HTHUMBNAIL fresh = nullptr;
    if (FAILED(DwmRegisterThumbnail(g_overlay, t.hwnd, &fresh)) || !fresh) {
        return;
    }

    DWM_THUMBNAIL_PROPERTIES props{};
    props.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE |
                    DWM_TNP_OPACITY | DWM_TNP_SOURCECLIENTAREAONLY;
    props.rcDestination = r;
    props.fVisible = TRUE;
    props.opacity = 255;
    props.fSourceClientAreaOnly = FALSE;

    if (SUCCEEDED(DwmUpdateThumbnailProperties(fresh, &props))) {
        DwmUnregisterThumbnail(t.thumb);
        t.thumb = fresh;
        t.last = r;
        t.placed = true;
    } else {
        DwmUnregisterThumbnail(fresh);
    }
}

// Plays the open animation backwards: every thumbnail flies back to where its
// real window is. If `pick` is set, that window is raised first (so the
// thumbnail lands on top, like the real window will be) and activated when
// the animation finishes. With no pick it just closes and restores focus.
static void BeginClose(int pickIndex) {
    if (!g_visible || !g_overlay) {
        return;
    }

    g_dragging = false;
    g_open = false;  // ignores clicks / hover while it closes
    HideHighlight();

    g_closeTarget = nullptr;
    if (pickIndex >= 0 && pickIndex < static_cast<int>(g_tiles.size())) {
        Tile& t = g_tiles[pickIndex];
        g_closeTarget = t.hwnd;

        if (t.dcFx && t.fadeIn) {
            // The picked minimized window becomes the real one: keep its tile
            // fully visible instead of fading it out with the swipe.
            t.fadeIn = false;
            t.fxOp = 1.0f;
            t.dcFx->SetOpacity(1.0f);
        }
        g_holdTiles = t.minimized;  // a minimized pick keeps the tiles up while it is restored
        BringTileToFront(t);

        if (t.minimized) {
            // Restore it right away: it is hidden behind the overlay while the
            // tile flies back, and its Mica comes in on the way.
            g_restoreIssued = true;
            g_restoreMs = NowMs();
            ShowWindowAsync(t.hwnd, SW_RESTORE);
        } else {
            // Activate (and raise) it now so its Mica turns on during the
            // flight instead of all at once when the animation ends.
            SetForegroundWindow(ResolvePickTarget(t.hwnd));
        }
    } else if (g_prevForeground && IsWindow(g_prevForeground) && !IsIconic(g_prevForeground)) {
        // Closing without picking: the window you were in comes back, same idea.
        SetForegroundWindow(g_prevForeground);
    }

    StartSpring(0.0f, g_animVel);
}

// ---------------------------------------------------------------------------
// Closing windows from Mission Control, keyboard selection
// ---------------------------------------------------------------------------

// Keeps the "top-most visual" marker valid after a tile was removed.
static void FixTopMarker() {
    g_dcTop = nullptr;
    g_dcIconTop = nullptr;
    for (auto& t : g_tiles) {
        if (!t.dcVisual) {
            continue;
        }
        g_dcTop = t.dcVisual;
        for (auto& k : t.kids) {  // dialogs, top-most first
            if (k.dcVisual) {
                g_dcTop = k.dcVisual;
                break;
            }
        }
        break;
    }
}

// The window is gone: take its tile out and let the others glide to their new slots.
static void RemoveTile(size_t i) {
    if (i >= g_tiles.size()) {
        return;
    }

    // Where every tile is drawn right now, so the rest glide from there.
    for (auto& t : g_tiles) {
        t.toPrev = EffectiveTo(t);
    }

    bool changed = false;
    ReleaseDCompVisuals(g_tiles[i], changed);
    if (g_tiles[i].thumb) {
        DwmUnregisterThumbnail(g_tiles[i].thumb);
        g_tiles[i].thumb = nullptr;
    }
    g_tiles.erase(g_tiles.begin() + static_cast<std::ptrdiff_t>(i));

    FixTopMarker();
    if (g_dcDevice) {
        g_dcDevice->Commit();
    }

    g_kbSel = -1;
    g_hoverClose = false;
    HideHighlight();

    if (g_tiles.empty()) {
        CreateDCompEmptyMessage();
        ApplyProgress(g_progress);
        InvalidateRect(g_overlay, nullptr, TRUE);  // text fallback without DComp
        return;
    }

    LayoutTiles();
    if (g_reduceMotion) {
        g_relayoutActive = false;
        g_relayoutT = 1.0f;
        ApplyProgress(g_progress);
    } else {
        g_relayoutActive = true;
        g_relayoutT = 0.0f;
        g_relayoutStartMs = NowMs();
        StartAnimClock();
    }
}

static void CloseTile(int idx) {
    if (idx < 0 || idx >= static_cast<int>(g_tiles.size())) {
        return;
    }
    const HWND hwnd = g_tiles[idx].hwnd;
    const bool minimized = g_tiles[idx].minimized;

    for (const auto& pc : g_pendingCloses) {
        if (pc.hwnd == hwnd) {
            return;  // already on its way out
        }
    }

    wchar_t cls[64] = {};
    GetClassNameW(hwnd, cls, 64);
    const bool uwpFrame = !wcscmp(cls, L"ApplicationFrameWindow");

    PendingClose pc;
    pc.hwnd = hwnd;
    pc.startMs = NowMs();

    // Windows' own close animation of this window would play in front of
    // Mission Control.
    pc.noTransitions = SUCCEEDED(SetWindowTransitions(hwnd, false));
    if (HIDE_BEFORE_CLOSE && !minimized && !uwpFrame && IsWindowVisible(hwnd)) {
        // Hidden first: the close then happens on an invisible window, so the
        // close animation of other mods (they play it when a visible window is
        // destroyed) has nothing to show. Async: never wait for the app here.
        pc.hidden = true;
        ShowWindowAsync(hwnd, SW_HIDE);
    } else {
        pc.posted = true;
        pc.postMs = pc.startMs;
        PostMessageW(hwnd, WM_CLOSE, 0, 0);
    }
    g_pendingCloses.push_back(pc);

    if (g_prevForeground == hwnd) {
        g_prevForeground = nullptr;
    }

    // The tile goes away right now and the others glide to their new slots.
    RemoveTile(static_cast<size_t>(idx));

    if (g_overlay) {
        SetTimer(g_overlay, CLOSE_POLL_TIMER_ID, 40, nullptr);
    }
}

// Follows the windows being closed.
static void PollPendingCloses() {
    const double now = NowMs();

    for (size_t i = 0; i < g_pendingCloses.size();) {
        PendingClose& pc = g_pendingCloses[i];

        if (!IsWindow(pc.hwnd)) {  // gone: done
            g_pendingCloses.erase(g_pendingCloses.begin() + static_cast<std::ptrdiff_t>(i));
            continue;
        }

        if (pc.shownBack) {
            // Its transitions come back on once the show has happened, so
            // showing it again doesn't animate in front of Mission Control.
            if (now - pc.shownMs > 500.0) {
                if (pc.noTransitions) {
                    SetWindowTransitions(pc.hwnd, true);
                }
                g_pendingCloses.erase(g_pendingCloses.begin() + static_cast<std::ptrdiff_t>(i));
                continue;
            }
            i++;
            continue;
        }

        if (!pc.posted) {
            // The hide has to reach the app before the close request does.
            if (!IsWindowVisible(pc.hwnd) || now - pc.startMs > 250.0) {
                PostMessageW(pc.hwnd, WM_CLOSE, 0, 0);
                pc.posted = true;
                pc.postMs = now;
            }
            i++;
            continue;
        }

        if (now - pc.postMs > 1200.0) {
            // Still there: it is asking for confirmation (in a dialog or inside
            // its own window), it closes to the tray, or it isn't responding. A
            // window we hid is always shown again; an app that closes to the
            // tray then simply shows up once more.
            if (pc.hidden) {
                ShowWindowAsync(pc.hwnd, SW_SHOWNA);
                pc.shownBack = true;
                pc.shownMs = now;
                i++;
                continue;
            }
            if (pc.noTransitions) {
                SetWindowTransitions(pc.hwnd, true);
            }
            g_pendingCloses.erase(g_pendingCloses.begin() + static_cast<std::ptrdiff_t>(i));
            continue;
        }
        i++;
    }

    if (g_pendingCloses.empty() && g_overlay) {
        KillTimer(g_overlay, CLOSE_POLL_TIMER_ID);
    }
}

static void SelectTile(int idx) {
    if (idx < 0 || idx >= static_cast<int>(g_tiles.size())) {
        return;
    }
    g_kbSel = idx;
    g_hover = idx;
    g_hoverClose = false;
    RECT r = g_tiles[idx].to;
    OffsetRect(&r, g_workRect.left, g_workRect.top);
    ShowHighlight(r);
}

// Arrow keys: the nearest tile in that direction (rows count less than distance).
static void MoveSelection(int dx, int dy) {
    const int n = static_cast<int>(g_tiles.size());
    if (n == 0) {
        return;
    }

    if (g_kbSel < 0 || g_kbSel >= n) {
        // First key press: start from the window you were in.
        int start = 0;
        if (g_prevForeground) {
            HWND root = GetAncestor(g_prevForeground, GA_ROOTOWNER);
            for (int i = 0; i < n; i++) {
                if (g_tiles[i].hwnd == g_prevForeground || g_tiles[i].hwnd == root) {
                    start = i;
                    break;
                }
            }
        }
        SelectTile(start);
        return;
    }

    const Tile& cur = g_tiles[g_kbSel];
    const float cx = (cur.to.left + cur.to.right) * 0.5f;
    const float cy = (cur.to.top + cur.to.bottom) * 0.5f;

    int best = -1;
    float bestScore = 1.0e30f;
    for (int i = 0; i < n; i++) {
        if (i == g_kbSel) {
            continue;
        }
        const Tile& o = g_tiles[i];
        const float vx = (o.to.left + o.to.right) * 0.5f - cx;
        const float vy = (o.to.top + o.to.bottom) * 0.5f - cy;
        if ((dx > 0 && vx <= 0) || (dx < 0 && vx >= 0) ||
            (dy > 0 && vy <= 0) || (dy < 0 && vy >= 0)) {
            continue;
        }
        const float primary = dx != 0 ? std::fabs(vx) : std::fabs(vy);
        const float secondary = dx != 0 ? std::fabs(vy) : std::fabs(vx);
        const float score = primary + 2.0f * secondary;
        if (score < bestScore) {
            bestScore = score;
            best = i;
        }
    }
    if (best >= 0) {
        SelectTile(best);
    }
}

static void CycleSelection(int step) {
    const int n = static_cast<int>(g_tiles.size());
    if (n == 0) {
        return;
    }
    if (g_kbSel < 0 || g_kbSel >= n) {
        SelectTile(step > 0 ? 0 : n - 1);
        return;
    }
    SelectTile((g_kbSel + step + n) % n);
}

// A card of the desktops bar was clicked: leave Mission Control and switch to
// that desktop (or create a new one). The shortcut is sent once the overlay is
// gone, so Windows' own desktop slide is what you see.
static void ActivateBarCard(int idx) {
    if (idx < 0 || idx >= static_cast<int>(g_cards.size())) {
        return;
    }
    const DesktopCard& c = g_cards[static_cast<size_t>(idx)];

    int cur = 0;
    for (size_t i = 0; i < g_cards.size(); i++) {
        if (g_cards[i].current) {
            cur = static_cast<int>(i);
        }
    }

    g_pendingDesktop = 0;
    g_pendingDesktopDelta = 0;
    if (c.isAdd) {
        g_pendingDesktop = 1;
    } else if (!c.current) {
        g_pendingDesktop = 2;
        g_pendingDesktopDelta = idx - cur;
    }

    // Giving focus back to a window of the old desktop would switch back to it.
    g_prevForeground = nullptr;
    BeginClose(-1);
}

// The X of a desktop card: leave Mission Control, go to that desktop and close it
// (Win+Ctrl+F4 closes the current one). Its windows move to the neighbor desktop.
static void CloseDesktopCard(int idx) {
    if (idx < 0 || idx >= static_cast<int>(g_cards.size()) ||
        g_cards[static_cast<size_t>(idx)].isAdd) {
        return;
    }

    int cur = 0;
    for (size_t i = 0; i < g_cards.size(); i++) {
        if (g_cards[i].current) {
            cur = static_cast<int>(i);
        }
    }

    g_pendingDesktop = 3;
    g_pendingDesktopDelta = idx - cur;
    g_prevForeground = nullptr;
    BeginClose(-1);
}

static void HandleClick(int x, int y) {
    if (!g_open || g_dragging) {
        return;
    }

    POINT pt{ x, y };

    if (g_barBuilt && y >= g_barTop) {
        const int xi = HitBarClose(x, y);
        if (xi >= 0) {
            CloseDesktopCard(xi);
            return;
        }
        const int bi = HitBarCard(x, y);
        if (bi >= 0) {
            ActivateBarCard(bi);
        }
        return;  // the empty part of the bar does nothing
    }
    int hit = -1;
    for (size_t i = 0; i < g_tiles.size(); i++) {
        if (TileContains(g_tiles[i], pt)) {
            hit = static_cast<int>(i);
            break;
        }
    }

    if (hit >= 0 && CLOSE_BUTTON && OnCloseButton(g_tiles[hit], pt)) {
        CloseTile(hit);
        return;
    }

    BeginClose(hit);
}

// "No apps opened", centered, when there is nothing to show.
static void DrawEmptyMessage(HDC dc, HWND hwnd) {
    RECT rc;
    GetClientRect(hwnd, &rc);

    const EmptyText et = GetEmptyMessage();
    HFONT font = CreateUiFont(dc, static_cast<int>(std::lround(30.0f * g_uiScale)), et.font);
    HGDIOBJ old = SelectObject(dc, font);
    SetBkMode(dc, TRANSPARENT);
    // Not pure black: black is what the Mica backdrop treats as "see-through".
    SetTextColor(dc, (g_darkTheme || g_wallpaperOk) ? RGB(235, 235, 235)
                                                    : RGB(40, 40, 40));
    DrawTextW(dc, et.text, -1, &rc,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX |
                  (et.rtl ? DT_RTLREADING : 0));
    SelectObject(dc, old);
    DeleteObject(font);
}

static LRESULT CALLBACK OverlayProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC dc = BeginPaint(hwnd, &ps);
            if (g_visible && g_tiles.empty() && !g_emptyVisual) {
                DrawEmptyMessage(dc, hwnd);  // fallback when DComp is unavailable
            }
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_ERASEBKGND: {
            RECT rc;
            GetClientRect(hwnd, &rc);
            if (g_wallpaperOk && g_bgDC) {
                BitBlt(reinterpret_cast<HDC>(wp), 0, 0, g_bgW, g_bgH,
                       g_bgDC, 0, 0, SRCCOPY);
                return 1;
            }
            // Pure black = transparent over the extended frame, which is
            // what lets Mica show through. Solid color only as a fallback.
            FillRect(reinterpret_cast<HDC>(wp), &rc,
                     g_micaOk ? static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH))
                              : g_backdropBrush);
            return 1;
        }
        case WM_NCACTIVATE:
            // Keep looking active (Mica on) while the picked window is already
            // the foreground one.
            return DefWindowProcW(hwnd, msg, TRUE, lp);
        case WM_MOUSEACTIVATE:
            return MA_ACTIVATE;
        case WM_LBUTTONDOWN:
            HandleClick(static_cast<short>(LOWORD(lp)),
                        static_cast<short>(HIWORD(lp)));
            return 0;
        case WM_MBUTTONUP:
            if (g_open && !g_dragging && MIDDLE_CLICK_CLOSE) {
                const POINT mp{ static_cast<short>(LOWORD(lp)), static_cast<short>(HIWORD(lp)) };
                for (size_t i = 0; i < g_tiles.size(); i++) {
                    if (TileContains(g_tiles[i], mp)) {
                        CloseTile(static_cast<int>(i));
                        break;
                    }
                }
            }
            return 0;
        case WM_KEYDOWN:
            if (KEYBOARD_SUPPORT && g_open && !g_dragging) {
                switch (wp) {
                    case VK_ESCAPE:
                        BeginClose(-1);
                        return 0;
                    case VK_RETURN:
                    case VK_SPACE:
                        if (g_kbSel >= 0) {
                            BeginClose(g_kbSel);
                        }
                        return 0;
                    case VK_LEFT:  MoveSelection(-1, 0); return 0;
                    case VK_RIGHT: MoveSelection(1, 0);  return 0;
                    case VK_UP:    MoveSelection(0, -1); return 0;
                    case VK_DOWN:  MoveSelection(0, 1);  return 0;
                    case VK_TAB:
                        CycleSelection((GetKeyState(VK_SHIFT) & 0x8000) ? -1 : 1);
                        return 0;
                    case VK_DELETE:
                        if (g_kbSel >= 0) {
                            CloseTile(g_kbSel);
                        }
                        return 0;
                    default:
                        break;
                }
            }
            break;
        case WM_CLOSE:
            // Alt+F4 on the overlay closes Mission Control, it must not destroy the window.
            if (g_open) {
                BeginClose(-1);
            }
            return 0;
        case WM_MOUSEMOVE:
            if (g_open && !g_dragging) {
                if (!g_trackingMouse) {
                    TRACKMOUSEEVENT tme{ sizeof(tme), TME_LEAVE, hwnd, 0 };
                    TrackMouseEvent(&tme);
                    g_trackingMouse = true;
                }
                // Only a real mouse movement counts (a few pixels), so Windows'
                // spurious WM_MOUSEMOVE doesn't override a keyboard selection.
                const POINT mp{ static_cast<short>(LOWORD(lp)), static_cast<short>(HIWORD(lp)) };
                if (std::abs(mp.x - g_lastMouse.x) + std::abs(mp.y - g_lastMouse.y) >= 3) {
                    g_lastMouse = mp;
                    g_kbSel = -1;
                    UpdateHover(mp.x, mp.y);
                }
            }
            return 0;
        case WM_MOUSELEAVE:
            g_trackingMouse = false;
            HideHighlight(true);
            return 0;
        case WM_RBUTTONDOWN:
            if (g_open) {
                BeginClose(-1);
            }
            return 0;
        case WM_APP_FRAME:
            InterlockedExchange(&g_framePending, 0);
            if (g_animActive && g_visible) {
                StepAnimation();
            }
            return 0;
        case WM_TIMER:
            if (wp == ANIM_TIMER_ID) {
                StepAnimation();
                return 0;
            }
            if (wp == DESKTOP_CLOSE_TIMER_ID) {
                KillTimer(hwnd, DESKTOP_CLOSE_TIMER_ID);
                SendWinCtrlKey(VK_F4, 1);
                return 0;
            }
            if (wp == BAR_TIMER_ID) {
                StepBarHover();
                return 0;
            }
            if (wp == CLOSE_POLL_TIMER_ID) {
                PollPendingCloses();
                return 0;
            }
            if (wp == HL_TIMER_ID) {
                StepHighlightFade();
                return 0;
            }
            if (wp == RENDER_TIMER_ID) {
                KillTimer(hwnd, RENDER_TIMER_ID);
                g_renderTimerSet = false;
                if (g_renderDirty && g_visible) {
                    ApplyProgress(g_progress);
                }
                return 0;
            }
            break;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

// Undoes a failed CreateOverlayWindow(), so the next retry starts clean.
static void FreeOverlayClass() {
    if (g_classRegistered) {
        UnregisterClassW(kOverlayClass, ModuleInstance());
        g_classRegistered = false;
    }
    if (g_backdropBrush) {
        DeleteObject(g_backdropBrush);
        g_backdropBrush = nullptr;
    }
}

static bool CreateOverlayWindow() {
    g_backdropBrush = CreateSolidBrush(BACKDROP_COLOR);

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = OverlayProc;
    wc.hInstance = ModuleInstance();
    wc.lpszClassName = kOverlayClass;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);

    if (!RegisterClassExW(&wc)) {
        Wh_Log(L"RegisterClassExW failed: %lu", GetLastError());
        FreeOverlayClass();
        return false;
    }
    g_classRegistered = true;

    // Layered: the overlay fades in with its alpha. Click-through until open.
    g_overlay = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT | WS_EX_LAYERED,
        kOverlayClass, L"", WS_POPUP,
        0, 0, 100, 100,
        nullptr, nullptr, wc.hInstance, nullptr);

    if (!g_overlay) {
        Wh_Log(L"CreateWindowExW failed: %lu", GetLastError());
        FreeOverlayClass();
        return false;
    }

    SetLayeredWindowAttributes(g_overlay, 0, 0, LWA_ALPHA);

    ApplyBackdropStyle(false);

    // Hover outline window (click-through, per-pixel alpha). Optional.
    WNDCLASSEXW hc{};
    hc.cbSize = sizeof(hc);
    hc.lpfnWndProc = DefWindowProcW;
    hc.hInstance = wc.hInstance;
    hc.lpszClassName = kHighlightClass;
    if (RegisterClassExW(&hc)) {
        g_hlClassRegistered = true;
        g_highlight = CreateWindowExW(
            WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST |
                WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
            kHighlightClass, L"", WS_POPUP,
            0, 0, 1, 1, nullptr, nullptr, wc.hInstance, nullptr);
    }

    InitDComp();  // on failure the legacy thumbnails are used

    return true;
}

static void DestroyOverlayWindow() {
    HideOverlay();
    FlushPendingCloses();
    RestoreTaskbars();
    StopClockThread();

    FreeBackground();

    ShutdownDComp();

    if (g_highlight) {
        DestroyWindow(g_highlight);
        g_highlight = nullptr;
    }
    if (g_hlClassRegistered) {
        UnregisterClassW(kHighlightClass, ModuleInstance());
        g_hlClassRegistered = false;
    }
    if (g_overlay) {
        DestroyWindow(g_overlay);
        g_overlay = nullptr;
    }
    FreeOverlayClass();
}

// ---------------------------------------------------------------------------
// Gesture state machine
// ---------------------------------------------------------------------------

static void OnGestureDelta(float deltaY) {
    if (!g_dragging) {
        g_dragging = true;
        g_open = false;
        g_dragBase = g_visible ? ProgressToRaw(g_progress) : 0.0f;
        g_dragAccumY = 0.0f;
        g_closeTarget = nullptr;  // a new swipe cancels a pending pick
        g_restoreIssued = false;
        UpdateReducedMotion();

        StopAnimClock();
        g_animVel = 0.0f;
        g_dragRaw = g_dragBase;
        g_dragVelRaw = g_visible ? g_progress : 0.0f;
        g_dragVelMs = NowMs();
        g_dragVel = 0.0f;
        HideHighlight();
        SetClickThrough(true);
    }

    g_dragAccumY += INVERT_Y ? -deltaY : deltaY;

    // Fingers moving up => Y decreases => progress increases.
    const float raw = g_dragBase - g_dragAccumY / GESTURE_DISTANCE;
    const float p = RawToProgress(raw);

    // Finger velocity from the unclamped value, sampled at most every 4 ms
    // (the three fingers report separately, so events arrive in bursts) and
    // lightly smoothed. Tracking itself is NOT filtered, only this estimate.
    {
        const double now = NowMs();
        if (now - g_dragVelMs >= 4.0) {
            const float inst = (p - g_dragVelRaw) /
                               static_cast<float>((now - g_dragVelMs) / 1000.0);
            g_dragVel = g_dragVel * 0.6f + inst * 0.4f;
            g_dragVelRaw = p;
            g_dragVelMs = now;
        }
        g_dragRaw = raw;
    }

    if (!g_visible) {
        if (p < SHOW_THRESHOLD) {
            return;
        }
        g_progress = p;
        ShowOverlay();  // renders the first frame itself
        return;
    }

    g_progress = p;
    RequestRender();
}

static void OnGestureEnd() {
    if (!g_dragging) {
        return;
    }
    g_dragging = false;

    if (!g_visible) {
        return;
    }

    // Fingers that stopped moving before lifting have no speed to hand over.
    float v = (NowMs() - g_dragVelMs < 80.0) ? g_dragVel : 0.0f;
    v = v > MAX_RELEASE_VELOCITY ? MAX_RELEASE_VELOCITY
        : (v < -MAX_RELEASE_VELOCITY ? -MAX_RELEASE_VELOCITY : v);

    const float projected = g_progress + v * FLING_PROJECTION_S;
    const float target = (projected >= COMMIT_THRESHOLD) ? 1.0f : 0.0f;
    g_open = (target >= 1.0f);
    SetClickThrough(!g_open);

    if (target <= 0.0f && g_prevForeground &&
        IsWindow(g_prevForeground) && !IsIconic(g_prevForeground)) {
        // Cancelling: bring back the window you were in while it closes.
        SetForegroundWindow(g_prevForeground);
    }

    StartSpring(target, v);
}

// ---------------------------------------------------------------------------
// Pointer tracking
// ---------------------------------------------------------------------------

static TrackedPointer* FindPointer(uint32_t id) {
    for (auto& pointer : g_pointers) {
        if (pointer.active && pointer.id == id) {
            return &pointer;
        }
    }
    return nullptr;
}

static TrackedPointer* AddPointer(uint32_t id) {
    if (auto* existing = FindPointer(id)) {
        return existing;
    }

    for (auto& pointer : g_pointers) {
        if (!pointer.active) {
            pointer.id = id;
            pointer.active = true;
            return &pointer;
        }
    }
    return nullptr;
}

// ---------------------------------------------------------------------------
// Direction of a three-finger swipe: vertical = Mission Control, horizontal =
// Alt+Tab (emulated, see ENABLE_ALT_TAB_SWIPE).
// ---------------------------------------------------------------------------

enum class SwipeAxis { Unknown, Vertical, Horizontal };
static SwipeAxis g_axis = SwipeAxis::Unknown;
static float g_axisAccX = 0.0f;  // travel before the direction is decided
static float g_axisAccY = 0.0f;
static bool g_altHeld = false;   // we are holding Alt down
static bool g_altSwipe = false;  // this horizontal swipe is driving Alt+Tab
static float g_altAccX = 0.0f;   // travel since the last Alt+Tab step

static void SendAltTabStep(bool forward) {
    INPUT in[5] = {};
    int n = 0;
    auto add = [&](WORD vk, bool up) {
        in[n].type = INPUT_KEYBOARD;
        in[n].ki.wVk = vk;
        in[n].ki.wScan = static_cast<WORD>(MapVirtualKeyW(vk, MAPVK_VK_TO_VSC));
        in[n].ki.dwFlags = up ? KEYEVENTF_KEYUP : 0;
        n++;
    };

    if (!g_altHeld) {
        add(VK_LMENU, false);
        g_altHeld = true;
    }
    if (!forward) {
        add(VK_LSHIFT, false);
    }
    add(VK_TAB, false);
    add(VK_TAB, true);
    if (!forward) {
        add(VK_LSHIFT, true);
    }
    SendInput(static_cast<UINT>(n), in, sizeof(INPUT));
}

// Always lets go of Alt (the switcher commits on release).
static void ReleaseAlt() {
    if (!g_altHeld) {
        return;
    }
    g_altHeld = false;

    INPUT in{};
    in.type = INPUT_KEYBOARD;
    in.ki.wVk = VK_LMENU;
    in.ki.wScan = static_cast<WORD>(MapVirtualKeyW(VK_LMENU, MAPVK_VK_TO_VSC));
    in.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &in, sizeof(in));
}

static void EndSwipe() {
    ReleaseAlt();
    g_altSwipe = false;
    g_altAccX = 0.0f;
    g_axis = SwipeAxis::Unknown;
    g_axisAccX = 0.0f;
    g_axisAccY = 0.0f;
}

// A game, a fullscreen video or a presentation has the screen. Mission Control
// is not wanted there (the horizontal Alt+Tab swipe still works).
static bool IsFullscreenAppActive() {
    // 1. Ask the shell (exclusive fullscreen, presentation mode, "busy" app).
    //    Skipped right after our own overlay was up: it covers the monitor too
    //    and the shell needs a moment to notice it is gone.
    if (NowMs() - g_lastHideMs > 1500.0) {
        typedef HRESULT(WINAPI * PFN_SHQueryUserNotificationState)(int*);
        static PFN_SHQueryUserNotificationState pQuery =
            reinterpret_cast<PFN_SHQueryUserNotificationState>(reinterpret_cast<void*>(
                GetProcAddress(LoadLibraryExW(L"shell32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32),
                               "SHQueryUserNotificationState")));
        int state = 0;
        if (pQuery && SUCCEEDED(pQuery(&state)) &&
            (state == 2 /*BUSY*/ || state == 3 /*D3D_FULL_SCREEN*/ || state == 4 /*PRESENTATION*/)) {
            return true;
        }
    }

    // 2. Borderless fullscreen: the foreground window has no caption and
    //    covers its whole monitor.
    HWND fg = GetForegroundWindow();
    if (!fg || fg == g_overlay || fg == g_thumbHost || fg == g_highlight) {
        return false;
    }
    wchar_t cls[64] = {};
    GetClassNameW(fg, cls, 64);
    if (IsDeniedClass(cls)) {
        return false;
    }
    const LONG_PTR style = GetWindowLongPtrW(fg, GWL_STYLE);
    if ((style & WS_CAPTION) == WS_CAPTION) {
        return false;  // a normal window, even maximized
    }
    RECT wr{};
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (!GetWindowRect(fg, &wr) ||
        !GetMonitorInfoW(MonitorFromWindow(fg, MONITOR_DEFAULTTONEAREST), &mi)) {
        return false;
    }
    return wr.left <= mi.rcMonitor.left && wr.top <= mi.rcMonitor.top &&
           wr.right >= mi.rcMonitor.right && wr.bottom >= mi.rcMonitor.bottom;
}

static void OnSwipeDelta(float dx, float dy) {
    if (g_axis == SwipeAxis::Vertical) {
        OnGestureDelta(dy);
        return;
    }

    if (g_axis == SwipeAxis::Horizontal) {
        if (!g_altSwipe) {
            return;  // horizontal swipe we deliberately ignore
        }
        g_altAccX += dx;
        while (std::fabs(g_altAccX) >= ALT_TAB_STEP) {
            const bool right = g_altAccX > 0.0f;
            SendAltTabStep(right == ALT_TAB_RIGHT_IS_FORWARD);
            g_altAccX += right ? -ALT_TAB_STEP : ALT_TAB_STEP;
        }
        return;
    }

    // Direction not decided yet: collect the travel until it is.
    g_axisAccX += dx;
    g_axisAccY += dy;
    if (std::hypot(g_axisAccX, g_axisAccY) < AXIS_LOCK_DISTANCE) {
        return;
    }

    if (std::fabs(g_axisAccY) >= std::fabs(g_axisAccX)) {
        if (!g_visible && DISABLE_IN_FULLSCREEN && IsFullscreenAppActive()) {
            Wh_Log(L"Fullscreen app active: Mission Control swipe ignored.");
            g_axis = SwipeAxis::Horizontal;  // "ignored" state: nothing more from this swipe
            g_altSwipe = false;
            return;
        }
        g_axis = SwipeAxis::Vertical;
        OnGestureDelta(g_axisAccY);  // replay what was collected
        return;
    }

    g_axis = SwipeAxis::Horizontal;
    // Not while Mission Control is on screen (or coming/going).
    if (ENABLE_ALT_TAB_SWIPE && !g_visible) {
        g_altSwipe = true;
        g_altAccX = 0.0f;
        SendAltTabStep((g_axisAccX > 0.0f) == ALT_TAB_RIGHT_IS_FORWARD);
    }
}

static void UpdateCentroid() {
    if (!g_eventsSeen) {
        g_eventsSeen = true;
        Wh_Log(L"Touchpad events are arriving (first one since the controller was created).");
    }

    float x = 0.0f;
    float y = 0.0f;
    int count = 0;

    for (const auto& pointer : g_pointers) {
        if (pointer.active) {
            x += pointer.x;
            y += pointer.y;
            count++;
        }
    }

    if (count != 3) {
        g_hasPreviousCentroid = false;
        EndSwipe();
        OnGestureEnd();
        return;
    }

    x /= 3.0f;
    y /= 3.0f;

    if (g_hasPreviousCentroid) {
        OnSwipeDelta(x - g_previousCentroidX, y - g_previousCentroidY);
    }

    g_previousCentroidX = x;
    g_previousCentroidY = y;
    g_hasPreviousCentroid = true;
}

static void OnPointerPressed(
    TouchpadGesturesController const&,
    PointerEventArgs const& args
) {
    auto point = args.CurrentPoint();
    auto physical = point.try_as<IPointerPointPhysicalPosition>();

    if (!physical || !physical.IsPhysicalPositionSupported()) {
        static bool logged = false;
        if (!logged) {
            logged = true;
            Wh_Log(L"The touchpad doesn't report physical positions: gestures ignored.");
        }
        return;
    }

    auto pos = physical.PhysicalPosition();
    auto* pointer = AddPointer(point.PointerId());

    if (!pointer) {
        return;
    }

    pointer->x = pos.X;
    pointer->y = pos.Y;

    UpdateCentroid();
}

void OnPointerMoved(
    TouchpadGesturesController const&,
    PointerEventArgs const& args
) {
    auto point = args.CurrentPoint();
    auto physical = point.try_as<IPointerPointPhysicalPosition>();

    if (!physical || !physical.IsPhysicalPositionSupported()) {
        return;
    }

    auto pos = physical.PhysicalPosition();
    auto* pointer = FindPointer(point.PointerId());

    if (!pointer) {
        return;
    }

    pointer->x = pos.X;
    pointer->y = pos.Y;

    UpdateCentroid();
}

void OnPointerReleased(
    TouchpadGesturesController const&,
    PointerEventArgs const& args
) {
    auto point = args.CurrentPoint();
    auto* pointer = FindPointer(point.PointerId());

    if (!pointer) {
        return;
    }

    pointer->active = false;
    g_hasPreviousCentroid = false;

    // Fewer than three fingers now: ends the gesture and commits/cancels.
    UpdateCentroid();
}

void CleanupController() {
    if (!g_controller) {
        return;
    }

    ReleaseAlt();  // never leave Alt stuck down

    try {
        g_controller.Enabled(false);

        if (g_pressedRegistered) {
            g_controller.PointerPressed(g_pressedToken);
            g_pressedRegistered = false;
        }

        if (g_movedRegistered) {
            g_controller.PointerMoved(g_movedToken);
            g_movedRegistered = false;
        }

        if (g_releasedRegistered) {
            g_controller.PointerReleased(g_releasedToken);
            g_releasedRegistered = false;
        }

        if (g_previousStateSaved) {
            g_controller.SupportedGestures(g_previousGestures);
            g_controller.Enabled(g_previousEnabled);
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Cleanup error: 0x%08X", static_cast<unsigned>(e.code()));
    }

    g_controller = nullptr;
}

// What we want the controller to be set to.
static TouchpadGlobalGestureKinds DesiredGestures() {
    // Manipulations only: we drive the animation ourselves, so Windows
    // should not also fire its own discrete Task View action.
    return TouchpadGlobalGestureKinds::ThreeFingerManipulations;
}

// One attempt at taking over the touchpad gestures. Returns false (after
// undoing anything half done) if the touchpad stack isn't ready yet.
static bool TryEnableController() {
    try {
        if (!TouchpadGesturesController::IsSupported()) {
            Wh_Log(L"TouchpadGesturesController not supported (yet).");
            return false;
        }

        g_controller = TouchpadGesturesController::CreateForProcess();
        if (!g_controller) {
            Wh_Log(L"CreateForProcess returned a null controller (yet).");
            return false;
        }

        // Save the existing controller configuration.
        g_previousEnabled = g_controller.Enabled();
        g_previousGestures = g_controller.SupportedGestures();
        g_previousStateSaved = true;

        // Register pointer event handlers.
        g_pressedToken = g_controller.PointerPressed(OnPointerPressed);
        g_pressedRegistered = true;

        g_movedToken = g_controller.PointerMoved(OnPointerMoved);
        g_movedRegistered = true;

        g_releasedToken = g_controller.PointerReleased(OnPointerReleased);
        g_releasedRegistered = true;

        g_controller.SupportedGestures(DesiredGestures());
        g_controller.Enabled(true);
        g_eventsSeen = false;
        return true;
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Controller setup failed: 0x%08X", static_cast<unsigned>(e.code()));
    } catch (...) {
        Wh_Log(L"Controller setup failed (unknown error).");
    }

    CleanupController();  // undo a partial setup, then try again later
    g_previousStateSaved = false;
    return false;
}

// At logon something else (the shell setting up its own gesture handling) can
// change the controller after we configured it. Put it back if so.
static void EnsureControllerConfig(bool& controllerReady) {
    if (!g_controller) {
        controllerReady = false;
        return;
    }

    try {
        if (!g_controller.Enabled() || g_controller.SupportedGestures() != DesiredGestures()) {
            Wh_Log(L"Controller settings were changed by someone else; re-applying.");
            g_controller.SupportedGestures(DesiredGestures());
            g_controller.Enabled(true);
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Controller check failed (0x%08X); setting it up again.",
               static_cast<unsigned>(e.code()));
        CleanupController();
        g_previousStateSaved = false;
        controllerReady = false;
    } catch (...) {
        CleanupController();
        g_previousStateSaved = false;
        controllerReady = false;
    }
}

static void LoadSettings();

DWORD WINAPI TouchpadThreadProc(void*) {
    // This thread's message queue comes first, so the WM_QUIT of Wh_ModUninit
    // and the settings messages can always reach it. Wh_ModInit waits for this.
    MSG msg{};
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    SetEvent(g_readyEvent);

    bool apartmentInitialized = false;

    try {
        winrt::init_apartment(
            winrt::apartment_type::single_threaded
        );
        apartmentInitialized = true;

        // Physical pixels everywhere, so window rects, the overlay and DWM
        // thumbnails all share one coordinate space on scaled displays.
        using SetDpiCtxFn = HANDLE(WINAPI*)(HANDLE);
        auto setDpiCtx = reinterpret_cast<SetDpiCtxFn>(reinterpret_cast<void*>(
            GetProcAddress(GetModuleHandleW(L"user32.dll"),
                           "SetThreadDpiAwarenessContext")));
        if (setDpiCtx) {
            setDpiCtx(reinterpret_cast<HANDLE>(-4));  // PER_MONITOR_AWARE_V2
        }

        Wh_Log(L"Touchpad thread started (thread %lu, system uptime %llu s).",
                GetCurrentThreadId(), static_cast<unsigned long long>(GetTickCount64() / 1000ULL));

        bool overlayReady = false;
        bool controllerReady = false;
        UINT_PTR tick = 0;
        int failedAttempts = 0;
        const double startMs = NowMs();
        double lastRefreshMs = startMs;
        double shellSeenMs = 0.0;
        bool shellSeen = false;
        bool shellRefreshed = false;
        bool quitting = false;

        // The overlay lives on this same thread, so gesture callbacks and
        // window messages never race each other.
        auto attempt = [&]() {
            if (quitting) {
                return;
            }
            // Explorer was still starting when the mod was loaded: once the
            // shell window exists, it must be this process's (see Wh_ModInit).
            if (ShellWindowOwner() < 0) {
                Wh_Log(L"The shell runs in another explorer.exe process: stopping here.");
                quitting = true;
                PostQuitMessage(0);
                return;
            }

            // Safety net: the real taskbar must never stay hidden.
            if (!g_visible && !g_taskbars.empty()) {
                RestoreTaskbars();
            }

            if (!overlayReady) {
                overlayReady = CreateOverlayWindow();
                if (overlayReady) {
                    StartClockThread();
                    {
                        // The size of the icons depends on the monitor: use the
                        // primary one for the pre-warm.
                        HMONITOR primary = MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY);
                        MONITORINFO scaleMi{};
                        scaleMi.cbSize = sizeof(scaleMi);
                        if (GetMonitorInfoW(primary, &scaleMi)) {
                            UpdateUiScale(scaleMi.rcMonitor.bottom - scaleMi.rcMonitor.top,
                                          MonitorDpi(primary));
                        }
                    }
                    StartPrewarm();  // icons for the open windows, off the UI thread

                    // Pre-render the blurred wallpaper so the first gesture doesn't hitch.
                    if (BACKDROP_MODE == 2) {
                        MONITORINFO pmi{};
                        pmi.cbSize = sizeof(pmi);
                        HMONITOR pm = MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY);
                        if (GetMonitorInfoW(pm, &pmi)) {
                            PrepareBackground(pmi.rcMonitor.right - pmi.rcMonitor.left,
                                              pmi.rcMonitor.bottom - pmi.rcMonitor.top);
                        }
                    }
                } else {
                    Wh_Log(L"Overlay window not ready yet.");
                }
            }

            if (overlayReady && !controllerReady) {
                controllerReady = TryEnableController();
                if (controllerReady) {
                    failedAttempts = 0;
                    Wh_Log(L"Touchpad controller enabled successfully.");
                }
            } else if (controllerReady) {
                EnsureControllerConfig(controllerReady);

                // At logon the shell finishes setting up its own gesture handling
                // after we did, and that can leave our controller registered but
                // not receiving anything. So, until the first touchpad event
                // arrives, the controller is re-created once the taskbar exists
                // and then every 15 s during the first minutes (only while idle).
                const double now = NowMs();
                const bool idle = !g_visible && !g_dragging && !g_hasPreviousCentroid &&
                                  g_axis == SwipeAxis::Unknown;
                if (controllerReady && idle && !g_eventsSeen) {
                    if (!shellSeen && FindWindowW(L"Shell_TrayWnd", nullptr)) {
                        shellSeen = true;
                        shellSeenMs = now;
                        Wh_Log(L"The taskbar is up; the controller will be refreshed in 3 s.");
                    }

                    bool refresh = false;
                    if (shellSeen && !shellRefreshed && now - shellSeenMs > 3000.0) {
                        shellRefreshed = true;
                        refresh = true;
                    } else if (now - startMs < 180000.0 && now - lastRefreshMs > 15000.0) {
                        refresh = true;
                    }

                    if (refresh) {
                        lastRefreshMs = now;
                        Wh_Log(L"Refreshing the touchpad controller (no events seen yet).");
                        CleanupController();
                        g_previousStateSaved = false;
                        controllerReady = TryEnableController();
                    }
                }
            }

            // After a minute of failures, back off so a machine that really
            // doesn't support this doesn't retry (and log) every 2 s, and after
            // a few more minutes stop trying.
            if (!(overlayReady && controllerReady) && tick) {
                ++failedAttempts;
                if (failedAttempts == 30) {
                    KillTimer(nullptr, tick);
                    tick = SetTimer(nullptr, 0, 10000, nullptr);
                    Wh_Log(L"Still not ready; now retrying every 10 s.");
                } else if (failedAttempts == 30 + SLOW_RETRY_ATTEMPTS) {
                    KillTimer(nullptr, tick);
                    tick = 0;
                    Wh_Log(L"Still not ready after several minutes; giving up until the mod is reloaded.");
                }
            }
        };

        // One failed attempt must never end the thread: it just tries again.
        auto safeAttempt = [&]() {
            try {
                attempt();
            } catch (...) {
                Wh_Log(L"An initialization attempt threw an exception; will retry.");
            }
        };

        safeAttempt();
        if (!(overlayReady && controllerReady)) {
            Wh_Log(L"Not ready at startup; retrying every %u ms.", INIT_RETRY_MS);
        }

        // Thread timer: retries until everything is up, then keeps checking.
        tick = SetTimer(nullptr, 0, INIT_RETRY_MS, nullptr);

        // Process messages and WinRT events on this same thread.
        while (true) {
            BOOL result = GetMessageW(&msg, nullptr, 0, 0);

            if (result <= 0) {
                if (result == -1) {
                    Wh_Log(L"GetMessageW failed.");
                }
                break;
            }

            if (msg.hwnd == nullptr) {
                if (msg.message == WM_TIMER && tick && msg.wParam == tick) {
                    safeAttempt();
                    continue;
                }
                if (msg.message == WM_APP_SETTINGS) {
                    LoadSettings();
                    Wh_Log(L"Settings reloaded.");
                    continue;
                }
            }

            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        if (tick) {
            KillTimer(nullptr, tick);
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(
            L"Touchpad thread failed: 0x%08X",
            static_cast<unsigned>(e.code())
        );
    } catch (...) {
        Wh_Log(L"Unknown exception in touchpad thread.");
    }

    CleanupController();

    StopPrewarm();
    DestroyOverlayWindow();

    if (apartmentInitialized) {
        winrt::uninit_apartment();
    }

    Wh_Log(L"Touchpad thread stopped.");

    return 0;
}

static int SettingInt(PCWSTR name, int lo, int hi) {
    const int v = Wh_GetIntSetting(name);
    return v < lo ? lo : (v > hi ? hi : v);
}

// Reads the Windhawk settings window into the variables the rest of the code uses.
static void LoadSettings() {
    BACKDROP_MODE = 0;
    BACKDROP_TYPE = 2;  // Mica
    const auto backdrop = WindhawkUtils::StringSetting::make(L"backdrop");
    PCWSTR bd = backdrop.get();
    if (!wcscmp(bd, L"micaAlt")) {
        BACKDROP_TYPE = 4;
    } else if (!wcscmp(bd, L"acrylic")) {
        BACKDROP_TYPE = 3;
    } else if (!wcscmp(bd, L"liveBlur")) {
        BACKDROP_MODE = 1;
    } else if (!wcscmp(bd, L"wallpaper")) {
        BACKDROP_MODE = 2;
    }

    GESTURE_DISTANCE = static_cast<float>(SettingInt(L"swipeDistance", 60, 400));
    INVERT_Y = Wh_GetIntSetting(L"invertSwipe") != 0;

    ENABLE_ALT_TAB_SWIPE =
        wcscmp(WindhawkUtils::StringSetting::make(L"horizontalSwipe").get(), L"ignore") != 0;
    ALT_TAB_RIGHT_IS_FORWARD = Wh_GetIntSetting(L"altTabReverse") == 0;

    SHOW_ICONS = Wh_GetIntSetting(L"showIcons") != 0;
    SHOW_MINIMIZED = Wh_GetIntSetting(L"showMinimized") != 0;
    SHOW_ALWAYS_ON_TOP = Wh_GetIntSetting(L"showAlwaysOnTop") != 0;
    SLIDE_TASKBAR = Wh_GetIntSetting(L"slideTaskbar") != 0;
    CLOSE_BUTTON = Wh_GetIntSetting(L"closeButton") != 0;
    MIDDLE_CLICK_CLOSE = Wh_GetIntSetting(L"middleClickClose") != 0;
    HIDE_BEFORE_CLOSE = Wh_GetIntSetting(L"hideBeforeClose") != 0;
    SHOW_TITLES = Wh_GetIntSetting(L"showTitles") != 0;
    DESKTOP_BAR = Wh_GetIntSetting(L"desktopBar") != 0;
    KEYBOARD_SUPPORT = Wh_GetIntSetting(L"keyboardSupport") != 0;
    DISABLE_IN_FULLSCREEN = Wh_GetIntSetting(L"disableInFullscreen") != 0;
    RESPECT_REDUCED_MOTION = Wh_GetIntSetting(L"respectReducedMotion") != 0;
    PREWARM = Wh_GetIntSetting(L"prewarm") != 0;

    const float speed = static_cast<float>(SettingInt(L"animationSpeed", 50, 200)) / 100.0f;
    SPRING_OMEGA = 24.0f * speed;
    CLOSE_OMEGA = 28.0f * speed;
    SPRING_DAMPING = static_cast<float>(SettingInt(L"springDamping", 30, 100)) / 100.0f;
    DRAG_OVERSHOOT_MAX =
        MaxF(0.001f, static_cast<float>(SettingInt(L"elasticStretch", 0, 25)) / 100.0f);

    const int fadeStart = SettingInt(L"backdropFadeStart", 0, 50);
    int fadeEnd = SettingInt(L"backdropFadeEnd", 1, 60);
    if (fadeEnd <= fadeStart) {
        fadeEnd = fadeStart + 1;
    }
    FADE_START_PROGRESS = static_cast<float>(fadeStart) / 100.0f;
    FADE_END_PROGRESS = static_cast<float>(fadeEnd) / 100.0f;

    UpdateReducedMotion();
}

// Called by Windhawk (on its own thread) when the settings window changes
// something: applied live, from the next gesture on. The settings are plain
// globals the touchpad thread reads, so they are loaded on that thread.
void Wh_ModSettingsChanged() {
    if (!g_threadId || !PostThreadMessageW(g_threadId, WM_APP_SETTINGS, 0, 0)) {
        LoadSettings();  // the thread isn't running
    }
}

BOOL Wh_ModInit() {
    Wh_Log(L"macOS Gestures Mission Control %s initializing.", WH_MOD_VERSION);

    // "explorer.exe" also matches folder windows opened in a separate process
    // and Explorer's COM servers. Only the shell process runs the mod: two
    // instances would both answer the swipes, and cloaking the taskbar only
    // works from the process that owns it. At logon the shell window doesn't
    // exist yet; the touchpad thread checks again once it does.
    const int shellOwner = ShellWindowOwner();
    if (shellOwner < 0 || (shellOwner == 0 && IsSecondaryExplorerCommandLine())) {
        Wh_Log(L"Not the shell's explorer.exe process: the mod doesn't run here.");
        return FALSE;
    }

    LoadSettings();

    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_readyEvent) {
        Wh_Log(L"CreateEventW failed: %lu", GetLastError());
        return FALSE;
    }

    g_thread = CreateThread(
        nullptr,
        0,
        TouchpadThreadProc,
        nullptr,
        0,
        &g_threadId
    );

    if (!g_thread) {
        Wh_Log(L"Failed to create touchpad thread.");
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
        return FALSE;
    }

    // Waits only until the thread has its message queue (its first step). The
    // slow part (touchpad stack, shell) is NOT waited for: at logon explorer is
    // still starting, and blocking its init on it could stall both. The thread
    // keeps retrying on its own until the touchpad is ready.
    HANDLE waits[2] = { g_readyEvent, g_thread };
    WaitForMultipleObjects(2, waits, FALSE, INFINITE);
    Wh_Log(L"macOS Gestures Mission Control started (initializing in background).");

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"macOS Gestures Mission Control shutting down.");

    if (g_thread) {
        PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);

        // Wait until the controller and overlay are cleaned up on their own thread.
        WaitForSingleObject(g_thread, INFINITE);

        CloseHandle(g_thread);
        g_thread = nullptr;
    }

    if (g_readyEvent) {
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
    }

    Wh_Log(L"macOS Gestures Mission Control unloaded.");
}
