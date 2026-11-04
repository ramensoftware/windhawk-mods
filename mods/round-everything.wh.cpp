// ==WindhawkMod==
// @id              round-everything
// @name            Round Everything
// @description     Turns the taskbar into a frame around a rounded screen, rounds the monitor corners, and gives every window - maximized included - one corner radius
// @version         1.0.0
// @author          Deen-0x
// @github          https://github.com/Deen-0x
// @include         explorer.exe
// @include         dwm.exe
// @architecture    x86-64
// @compilerOptions -lgdiplus -lgdi32 -lshell32 -ldwmapi -lole32 -loleaut32 -lruntimeobject
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Round Everything

Turns the taskbar into a frame around the whole screen. The taskbar keeps its
rectangular shape, and the other three screen edges get a band of the same
color, pushed inwards by a configurable offset. The visible "screen" becomes a
rounded rectangle that sits flush against the taskbar, with one configurable
corner radius.

Works with the taskbar on any edge (top, bottom, left, right) and on every
monitor. A monitor without a taskbar ("Show my taskbar on all displays" off)
gets the frame on all four edges.

![Demo](https://raw.githubusercontent.com/Deen-0x/windhawk-assets/main/round-everything/demo.gif)

## How it works

The mod runs inside the taskbar's `explorer.exe` and creates one layered,
always-on-top window per monitor covering the area next to the taskbar:
three bands of the configured offset with the rounded corners. The taskbar
itself, its background painted in the frame color, is the fourth side. The
frame shape is drawn with GDI+ (anti-aliased); the inner area is fully
transparent and lets the mouse through, while the painted frame is solid to
the mouse, like the taskbar. No explorer function is hooked: the taskbar's
XAML tree is reached through a few `taskbar.dll` symbol lookups (no XAML
diagnostics, so it coexists with Taskbar Styler).

* **Color** – the frame has one color of your choice. By default the
  taskbar's own background (`Taskbar.TaskbarBackground`) is painted with it
  at the same opacity, so the taskbar and the frame are one surface. The
  taskbar's own background is restored when the option or the mod is turned
  off.
* Frame and taskbar come and go together. When the mod starts, the frame
  appears in the same instant as the taskbar's new look. When the
  taskbar moves, resizes or rebuilds (changing its position, a taskbar mod
  reloading) its background turns transparent and the frame goes away the
  instant the taskbar window moves; once everything has settled - taskbar
  at its new edge and uncloaked, its corners in place, the work area
  reserved - the taskbar paints its background again and, from inside that
  same rendering pass, reveals the frame (re-shaped for the new edge), so
  the two reach the screen in one composition. When the mod is turned off
  the frame disappears as the taskbar is handed back.
* The frame stays visible during Aero Peek (hovering a taskbar thumbnail or
  the show-desktop corner), like the taskbar itself.
* **Task View** (Win+Tab) – the frame goes away and the taskbar's background
  and monitor corners turn transparent while it is open; everything comes
  back together when it closes.
* **Full-screen apps** – the frame hides on a monitor while a full-screen
  window (game, video, F11 browser) is showing there, and comes back
  afterwards.
* **Work area** – *Shrink the work area to the rounded screen* (on by
  default) pushes the work area inwards so maximized and snapped windows stay
  inside the frame; turned off, the frame is purely visual and overlaps the
  edges of maximized windows. Shrinking registers one
  invisible appbar per frame edge (the same mechanism the taskbar uses), so
  the shell reserves the space itself; the appbars are removed and the work
  area returns to normal when the option or the mod is turned off. *Extra
  work-area inset* shrinks it further, on all four sides, so a strip of
  wallpaper stays visible between the frame/taskbar and your windows.
* **Monitor corners** – the four outer corners of each monitor are rounded
  too, so the whole screen reads as a rounded panel: the sliver outside a
  quarter circle is painted in the *Monitor corner color* (black by default).
  The two corners away from the taskbar are part of the frame itself; the
  two on the taskbar are XAML shapes added inside the taskbar (no extra
  windows, so nothing to fight over z-order with), click-through, removed
  when the mod is disabled.
* **Taskbar border line** – the taskbar draws a 1px line along its inner edge
  (`BackgroundStroke`), which shows up as a seam between the taskbar and the
  frame. The mod sets that line's opacity to 0 and restores it when disabled.
* Offsets and radii are in pixels at 100% scaling and scaled per monitor
  (configurable).

## Window corners (dwm.exe)

Windows 11 rounds normal windows with a fixed 8 px radius and draws maximized
windows square. With **Round window corners** on, the mod also runs inside
`dwm.exe` and hooks the corner-radius decision
(`CTopLevelWindow::GetFloatCornerRadiusForCurrentStyle`): every window that
Windows would round - normal, snapped or maximized - gets the one **Window
corner radius** you set, and maximized windows are rounded too, so they sit in
the rounded screen like cards. Windows that opt out of rounding (`DONOTROUND`)
and small-radius surfaces (menus, tooltips) are left alone. Changes apply
instantly: every visible window has its DWM corner preference toggled and set
straight back, which makes DWM rebuild its clip. Full-screen windows share the
maximized path in DWM, so they get rounded corners as well.

`udwm.dll` symbols are required for this part (downloaded by Windhawk on first
use); the frame itself does not depend on them.

## Notes

* Shell flyouts (Start, notification center, Task View, …) live above the
  frame, so they are never covered.
* The frame is solid to the mouse, like the taskbar: clicks on it (and on
  the extra work-area inset strip) go nowhere, which keeps the invisible
  resize borders of maximized windows underneath from being hit.
* With an auto-hide taskbar the frame spans the whole monitor and the taskbar
  slides in over it.
* Setting all offsets to 0 and keeping a radius gives you just the rounded
  screen corners against the taskbar.

## Credits

The taskbar-thread helpers (`RunFromWindowThreadTimeout`,
`FindCurrentProcessTaskbarWnd`) and the `taskbar.dll` XAML-root lookup are
ported from [Taskbar multi-row](https://windhawk.net/mods/taskbar-multirow)
by m417z, licensed under the GNU GPL v3.0. This mod is published under the
same license.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- offset: 8
  $name: Inward offset
  $description: How far (in pixels) the frame extends inwards from the three screen edges that don't carry the taskbar. The taskbar edge gets no offset, so the rounded screen sits flush against the taskbar.
- edgeOffsets: "-1,-1,-1,-1"
  $name: Per-edge offset overrides
  $description: Four values in pixels in the order left,top,right,bottom (for example -1,-1,-1,24). A value of 0 or more overrides the inward offset of that screen edge (including the taskbar edge, to leave a gap between the taskbar and the rounded screen); -1 keeps the default behavior for that edge.
- radius: 20
  $name: Screen corner radius (px)
  $description: Radius of the four corners of the rounded screen, including the two where the frame meets the taskbar.
- monitorCorners:
  - enabled: true
    $name: Round the monitor corners
    $description: Paint the four outer corners of each monitor (over the taskbar as well) so the whole screen looks like a rounded panel.
  - radius: 16
    $name: Monitor corner radius (px)
  - color: "#000000"
    $name: Monitor corner color
    $description: Hex color (#RRGGBB) painted outside the rounded monitor corners.
  $name: Monitor corners
- scaleWithDpi: true
  $name: Scale sizes with the monitor DPI
  $description: Treat offsets and radii as values at 100% scaling and scale them for each monitor.
- customColor: "#585858"
  $name: Frame color
  $description: Hex color (#RRGGBB).
- colorTaskbar: true
  $name: Taskbar background in the frame color
  $description: Paint the taskbar's own background (Taskbar.TaskbarBackground) with the frame color at the frame's opacity, so the taskbar and the frame are one surface. The taskbar's own background is restored when the option or the mod is turned off.
- opacity: 100
  $name: Opacity (%)
- reserveWorkArea: true
  $name: Shrink the work area to the rounded screen
  $description: Also push the work area inwards so maximized and snapped windows stay inside the frame instead of being covered by it. Done with invisible appbars, one per frame edge; removed when the option or the mod is turned off.
- workAreaInset: 12
  $name: Extra work-area inset (px)
  $description: Only with the option above. Shrinks the work area this many extra pixels inwards beyond the colored frame, on all four sides (next to the taskbar too), leaving a strip of wallpaper around maximized and snapped windows. 0 ends the work area exactly at the frame's inner edge.
- monitors: all
  $name: Monitors
  $options:
  - all: Every monitor
  - primary: Primary monitor only
- windowCorners:
  - enabled: true
    $name: Round window corners
    $description: Round the corners of every window Windows would round - normal, snapped and maximized - with one radius. Maximized windows, which Windows draws square, become rounded like the screen. Runs inside dwm.exe (udwm.dll symbols are downloaded by Windhawk on first use).
  - radius: 16
    $name: Window corner radius (px)
    $description: Corner radius for all windows at 100% scaling. 0 keeps Windows' own radius (8 px) for normal windows and gives maximized windows that same radius.
  $name: Window corners (DWM)
*/
// ==/WindhawkModSettings==

#include <windows.h>

#include <windhawk_utils.h>

#include <dwmapi.h>
#include <shellapi.h>
#include <gdiplus.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cwchar>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace wu = winrt::Windows::UI;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxm = winrt::Windows::UI::Xaml::Media;
namespace wuxs = winrt::Windows::UI::Xaml::Shapes;

namespace {

enum Edge : int { kEdgeLeft = 0, kEdgeTop = 1, kEdgeRight = 2, kEdgeBottom = 3 };
constexpr int kEdgeNone = -1;  // monitor without a taskbar
int DetectEdge(const RECT& taskbar, const RECT& monitor);  // defined with the layout code
enum Corner : int { kCornerTL = 0, kCornerTR = 1, kCornerBR = 2, kCornerBL = 3 };

constexpr WCHAR kFrameClassName[] = L"RoundEverything_Frame";
constexpr WCHAR kCtrlClassName[] = L"RoundEverything_Ctrl";
constexpr WCHAR kAppBarClassName[] = L"RoundEverything_AppBar";
// Messages to the mod's control window (all handled on the mod's UI thread).
constexpr UINT kMsgReload = WM_APP + 1;            // settings changed
constexpr UINT kMsgAppBarCallback = WM_APP + 2;    // ABN_* notifications to the appbar windows
constexpr UINT kMsgTaskbarEvent = WM_APP + 3;      // a taskbar / Task View window showed, hid or
                                                   // moved, or the foreground changed
constexpr UINT kMsgPassResult = WM_APP + 4;        // a XAML pass finished on the taskbar thread
constexpr UINT kMsgTaskbarReady = WM_APP + 5;      // a taskbar's tree is whole and rendered
constexpr UINT kMsgTaskbarMoving = WM_APP + 6;     // a taskbar window just moved/resized
constexpr UINT kMsgTaskbarRecolored = WM_APP + 7;  // the taskbar's background is painted again
// A recolor request to the taskbar thread is waited for at most this long
// before the frame shows anyway (a taskbar thread still busy after a layout
// change answers late).
constexpr ULONGLONG kRecolorWaitMaxMs = 1000;
// A frame waiting for its taskbar (to appear together with it) shows anyway
// after this long, whatever is still missing.
constexpr ULONGLONG kTaskbarWaitMaxMs = 2500;
// Tick rate while a frame waits for its taskbar, so it appears within a
// frame of the taskbar being uncloaked.
constexpr UINT kWaitTickIntervalMs = 16;
// A re-created taskbar window is cloaked (DWM) until its tree reports the
// ears, stroke and fill in place, at most this long.
constexpr ULONGLONG kVeilMaxMs = 1500;
// A request to the taskbar thread that got no answer in this long is
// considered lost (the window went away) and is sent again.
constexpr ULONGLONG kTaskbarCallLostMs = 2000;
constexpr UINT_PTR kTimerId = 1;
constexpr UINT kTickIntervalMs = 100;
// A pass that answered "not found" (the taskbar's tree is being rebuilt) is
// sent again after this long.
constexpr ULONGLONG kPassRetryMs = 100;

// How often the slow, cross-thread maintenance passes re-check their work.
constexpr ULONGLONG kWorkAreaRetryMs = 1000;
constexpr ULONGLONG kTaskbarXamlRetryMs = 2000;  // stroke, fill and corner ears

struct Settings {
    int offset = 8;
    int edgeOffset[4] = {-1, -1, -1, -1};  // left, top, right, bottom
    int radius = 20;
    bool monitorCorners = true;
    int monitorCornerRadius = 16;
    COLORREF monitorCornerColor = RGB(0, 0, 0);
    bool scaleWithDpi = true;
    COLORREF customColor = RGB(0x58, 0x58, 0x58);
    bool colorTaskbar = true;
    int opacity = 100;
    bool reserveWorkArea = true;
    int workAreaInset = 12;
    bool primaryOnly = false;
};

// The frame is one always-on-top window per monitor along the three edges
// next to the taskbar; the taskbar itself, with its background painted in
// the frame color, is the fourth side. The two are shown and hidden
// together: the frame waits for the taskbar's tree to be whole and rendered
// (and the taskbar to be uncloaked by the shell) before it appears, and
// goes away the moment the taskbar starts moving or is cloaked. A monitor
// without a taskbar gets the frame on all four edges.
struct Layout {
    HMONITOR monitor = nullptr;
    HWND taskbar = nullptr;  // null on a monitor without a (live) taskbar window
    RECT monitorRect{};
    RECT taskbarRect{};   // the taskbar strip (empty when there is none)
    RECT baseWorkArea{};  // monitor minus the taskbar strip
    RECT overlayRect{};   // the frame window (baseWorkArea, minus the auto-hide reveal strip)
    RECT innerRect{};     // the rounded "screen" rectangle, screen coordinates
    int edge = kEdgeNone;
    UINT dpi = 96;
    int offset[4] = {0, 0, 0, 0};  // left, top, right, bottom (scaled px)
    int radius = 0;                // rounded-screen corner radius (scaled px)
    int workAreaInset = 0;         // extra work-area shrink beyond the frame (scaled px)
    int monitorCornerRadius = 0;   // outer monitor corner radius (scaled px), 0 = off
    std::wstring signature;        // this monitor's part of the layout signature
};

// One invisible appbar reserving a strip of the work area on one edge.
struct AppBar {
    HWND hwnd = nullptr;
    UINT edge = ABE_BOTTOM;
    int thickness = 0;
};

// Rasterized shape of one frame window, built once per layout. A repaint
// only recolors it, so a color change is cheap.
struct FrameSurface {
    int width = 0;
    int height = 0;
    HDC memDc = nullptr;
    HBITMAP dib = nullptr;
    HGDIOBJ oldBitmap = nullptr;
    DWORD* pixels = nullptr;        // the DIB bits, premultiplied BGRA, top-down
    std::vector<BYTE> coverage;     // frame anti-aliased coverage per pixel
    std::vector<DWORD> staticLayer; // color-independent pixels (monitor corner ears,
                                    // hit-test strip), premultiplied; 0 = none

    FrameSurface() = default;
    FrameSurface(const FrameSurface&) = delete;
    FrameSurface& operator=(const FrameSurface&) = delete;
    ~FrameSurface() {
        if (memDc) {
            SelectObject(memDc, oldBitmap);
            DeleteDC(memDc);
        }
        if (dib) {
            DeleteObject(dib);
        }
    }
};

struct Overlay {
    Layout layout;
    HWND hwnd = nullptr;
    std::unique_ptr<FrameSurface> surface;
    bool shown = false;
    // Waiting for the taskbar: the frame stays off the screen from the
    // moment its taskbar changes (moves, is cloaked, is new) until the
    // taskbar's tree has reported whole-and-rendered after that moment and
    // the shell has uncloaked it - so both appear in the same frame.
    bool waiting = false;
    ULONGLONG waitSince = 0;
    ULONGLONG recolorRequestTick = 0;  // when the recolor was asked for (0: not yet)
    UINT recolorSerial = 0;            // which request (its answer is waited for)
    // The frame window is visible at alpha 0 and registered in g_reveals: the
    // taskbar thread presents it together with the recolor (see BeginReveal).
    bool revealPending = false;
    // Task View: frame hidden and taskbar background transparent while it
    // is open; both come back together when it closes.
    bool taskViewHidden = false;
    ULONGLONG taskViewRecolorTick = 0;
    UINT taskViewRecolorSerial = 0;
    bool fullscreen = false;
    HWND fullscreenWnd = nullptr;  // the window the frame is hidden for
    int fullscreenTicks = 0;  // consecutive ticks the monitor looked full-screen
    bool painted = false;
    COLORREF paintedColor = 0;  // what is on screen right now
    // Presentation alpha (UpdateLayeredWindow's constant alpha): 0 takes
    // the frame off the screen before its window is hidden or destroyed.
    BYTE presentAlpha = 255;
};

// The appbars reserving the work area on one monitor.
struct AppBarSet {
    HMONITOR monitor = nullptr;
    RECT baseWorkArea{};
    int thickness[4] = {0, 0, 0, 0};  // left, top, right, bottom
    std::vector<AppBar> bars;
    RECT workAreaAfterApply{};  // rcWork observed right after the last ABM_SETPOS pass
};

// What the UI thread wants reserved on one monitor (published every tick).
struct AppBarWish {
    HMONITOR monitor = nullptr;
    RECT baseWorkArea{};
    int thickness[4] = {0, 0, 0, 0};
};

// What the appbar thread has in place on one monitor.
struct AppBarStatus {
    HMONITOR monitor = nullptr;
    RECT baseWorkArea{};
};

// What the appbar thread last learned from the shell.
struct ShellState {
    bool autoHide = false;
    int primaryEdge = -1;  // ABE_* of the primary taskbar, -1 unknown
    RECT primaryRect{};    // the taskbar rect that edge was reported for
};

// One taskbar-side pass (stroke, ears, fill) on one taskbar.
struct PassState {
    bool done = false;          // last answer: applied
    bool outstanding = false;   // a request is on the taskbar thread
    ULONGLONG sentTick = 0;
    ULONGLONG answerTick = 0;
    ULONGLONG doneTick = 0;     // when it was last confirmed applied
    UINT serial = 0;
};
enum XamlPass { kPassStroke = 0, kPassCorners = 1, kPassFill = 2, kPassCount = 3 };

enum class HostProcess { Explorer, Dwm };

Settings g_settings;
HostProcess g_host = HostProcess::Explorer;
HMODULE g_module = nullptr;
HANDLE g_thread = nullptr;
HANDLE g_stopEvent = nullptr;

// Shell calls (SHAppBarMessage) are synchronous messages to the taskbar
// window. A taskbar that is relaying out (a layout change keeps its thread
// busy for a second or more) blocks such a call until it is responsive
// again - which is exactly when the frame must not be stalled. The UI
// thread therefore never makes them: it publishes wishes, the appbar thread
// reconciles them and reports back what is in place.
HANDLE g_appBarThread = nullptr;
HANDLE g_appBarWake = nullptr;         // UI thread -> appbar thread: wishes changed
HANDLE g_appBarStop = nullptr;         // UI thread -> appbar thread: remove everything and exit
HANDLE g_taskbarRestored = nullptr;    // taskbar thread -> UI thread: hand-back is on screen
HANDLE g_shellStateReady = nullptr;    // first shell state fetched
std::mutex g_shellMutex;               // guards the four below
std::vector<AppBarWish> g_appBarWishes;
bool g_appBarsWanted = true;           // reserveWorkArea
std::vector<AppBarStatus> g_appBarStatus;
ShellState g_shellState;
std::atomic<bool> g_appBarsDirty{false};  // ABN_POSCHANGED: re-query positions
std::atomic<HWND> g_ctrlWnd{nullptr};
std::atomic<bool> g_reloadRequested{false};
std::atomic<bool> g_unloading{false};
bool g_taskbarXamlAvailable = false;  // taskbar.dll symbols resolved

// State owned by the mod's UI thread.
std::vector<Overlay> g_overlays;
std::unordered_map<HWND, PassState> g_passState[kPassCount];
UINT g_requestSerial = 0;  // tags requests to the taskbar thread; stale answers are dropped
// The last usable taskbar seen on each monitor, to tell a window that moved
// or resized from one that was re-created (which gets the DWM veil below).
struct SeenTaskbar {
    HWND hwnd = nullptr;
    RECT rect{};
    int edge = kEdgeNone;
};
std::unordered_map<HMONITOR, SeenTaskbar> g_seenTaskbars;

// A taskbar window that was just re-created is cloaked (DWM) until its tree
// reports everything in place for its edge (or kVeilMaxMs pass), so it never
// shows with square corners. A window that merely moved is veiled from inside
// its own thread instead (its content at opacity 0, see TaskbarWants).
struct Veil {
    ULONGLONG since = 0;
    int edge = kEdgeNone;
};
std::unordered_map<HWND, Veil> g_veils;
// When each taskbar last reported its tree whole and rendered, and for which
// edge; and when it last reported its background painted again (UI thread).
struct TaskbarReady {
    ULONGLONG tick = 0;
    int edge = kEdgeNone;
};
std::unordered_map<HWND, TaskbarReady> g_taskbarReady;
// Recolor requests are numbered, so the answer to an earlier request (a
// Task View closing right before a layout change, say) can't pass for the
// answer to a later one. The last serial each taskbar reported rendered:
std::unordered_map<HWND, UINT> g_taskbarRecolored;
UINT g_recolorSerial = 0;

// A frame window handed to the taskbar thread to be revealed in the same
// rendering pass as the taskbar's color (the handshake that puts both on the
// screen in one composition). Keyed by taskbar. The UI thread registers and
// withdraws entries and holds the lock whenever it touches a frame's bitmap;
// the taskbar thread holds it while presenting from that bitmap.
struct Reveal {
    UINT serial = 0;  // the recolor request it belongs to
    HWND frame = nullptr;
    HDC memDc = nullptr;
    int width = 0;
    int height = 0;
    RECT rect{};
};
std::mutex g_revealMutex;
std::unordered_map<HWND, Reveal> g_reveals;
bool g_fastTick = false;  // the tick timer runs at kWaitTickIntervalMs

// A taskbar window the shell has cloaked stays part of the layout (at its
// current rect, so the frame follows a move at once) for this long; one that
// stays cloaked longer has been taken away (taskbar on another display
// turned off) and its monitor gets the four-sided frame.
constexpr ULONGLONG kCloakGraceMs = 1500;
std::unordered_map<HWND, ULONGLONG> g_cloakedSince;
std::wstring g_layoutSignature;
bool g_inTick = false;
bool g_taskbarEventPending = false;  // a kMsgTaskbarEvent is queued
ULONGLONG g_lastTickTime = 0;        // when the last tick ran (throttles event-driven ticks)
ULONGLONG g_threadStartTick = 0;
bool g_fillOwned = false;         // at least one taskbar background was painted
bool g_xamlCornersOwned = false;  // corner ears were added to at least one taskbar
DWORD g_lastFillArgb = 0;         // the brush last asked for (a change is applied at once)

// ---------------------------------------------------------------------------
// Small helpers
// ---------------------------------------------------------------------------

HostProcess DetectHostProcess() {
    WCHAR path[MAX_PATH];
    DWORD len = GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    if (len == 0 || len >= ARRAYSIZE(path)) {
        return HostProcess::Explorer;
    }
    PCWSTR name = wcsrchr(path, L'\\');
    name = name ? name + 1 : path;
    return _wcsicmp(name, L"dwm.exe") == 0 ? HostProcess::Dwm : HostProcess::Explorer;
}

HMODULE GetCurrentModule() {
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&GetCurrentModule), &module);
    return module;
}

UINT GetWindowDpi(HWND hwnd) {
    using GetDpiForWindow_t = UINT(WINAPI*)(HWND);
    static GetDpiForWindow_t pGetDpiForWindow =
        reinterpret_cast<GetDpiForWindow_t>(GetProcAddress(
            GetModuleHandleW(L"user32.dll"), "GetDpiForWindow"));
    UINT dpi = pGetDpiForWindow ? pGetDpiForWindow(hwnd) : 0;
    return dpi ? dpi : 96;
}

UINT GetMonitorDpi(HMONITOR monitor) {
    using GetDpiForMonitor_t = HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*);
    static GetDpiForMonitor_t pGetDpiForMonitor = [] {
        HMODULE shcore = LoadLibraryExW(L"shcore.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        return shcore ? reinterpret_cast<GetDpiForMonitor_t>(
                            GetProcAddress(shcore, "GetDpiForMonitor"))
                      : nullptr;
    }();
    UINT dpiX = 0, dpiY = 0;
    if (pGetDpiForMonitor &&
        SUCCEEDED(pGetDpiForMonitor(monitor, 0 /* MDT_EFFECTIVE_DPI */, &dpiX, &dpiY)) && dpiX) {
        return dpiX;
    }
    return 96;
}

int ScaleForDpi(int value, UINT dpi) {
    if (!g_settings.scaleWithDpi || dpi == 96) {
        return value;
    }
    return MulDiv(value, static_cast<int>(dpi), 96);
}

void SetThreadPerMonitorDpiAware() {
    using SetThreadDpiAwarenessContext_t = HANDLE(WINAPI*)(HANDLE);
    auto pSetThreadDpiAwarenessContext =
        reinterpret_cast<SetThreadDpiAwarenessContext_t>(GetProcAddress(
            GetModuleHandleW(L"user32.dll"), "SetThreadDpiAwarenessContext"));
    if (pSetThreadDpiAwarenessContext) {
        // DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
        pSetThreadDpiAwarenessContext(reinterpret_cast<HANDLE>(-4));
    }
}

void CloakWindow(HWND hwnd, bool cloak) {
    BOOL value = cloak ? TRUE : FALSE;
    DwmSetWindowAttribute(hwnd, static_cast<DWMWINDOWATTRIBUTE>(13) /* DWMWA_CLOAK */, &value,
                          sizeof(value));
}

bool IsWindowCloaked(HWND hwnd) {
    DWORD cloaked = 0;
    HRESULT hr = DwmGetWindowAttribute(hwnd, static_cast<DWMWINDOWATTRIBUTE>(14) /* DWMWA_CLOAKED */,
                                       &cloaked, sizeof(cloaked));
    return SUCCEEDED(hr) && cloaked != 0;
}

bool RectContains(const RECT& outer, const RECT& inner) {
    return outer.left <= inner.left && outer.top <= inner.top &&
           outer.right >= inner.right && outer.bottom >= inner.bottom;
}

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

COLORREF ParseHexColor(PCWSTR text, COLORREF fallback) {
    if (!text) {
        return fallback;
    }

    std::wstring s(text);
    size_t begin = s.find_first_not_of(L" \t\r\n");
    size_t end = s.find_last_not_of(L" \t\r\n");
    if (begin == std::wstring::npos) {
        return fallback;
    }
    s = s.substr(begin, end - begin + 1);
    if (!s.empty() && s[0] == L'#') {
        s.erase(0, 1);
    }
    if (s.size() != 6) {
        return fallback;
    }

    wchar_t* parseEnd = nullptr;
    unsigned long value = wcstoul(s.c_str(), &parseEnd, 16);
    if (!parseEnd || *parseEnd != L'\0') {
        return fallback;
    }

    return RGB((value >> 16) & 0xFF, (value >> 8) & 0xFF, value & 0xFF);
}

// "left,top,right,bottom" -> out[kEdgeLeft..kEdgeBottom]. Missing or
// unparsable entries fall back to -1 (default behavior for that edge).
void ParseEdgeOffsets(PCWSTR text, int out[4]) {
    for (int i = 0; i < 4; i++) {
        out[i] = -1;
    }
    if (!text) {
        return;
    }

    std::wstring_view rest(text);
    for (int i = 0; i < 4 && !rest.empty(); i++) {
        size_t comma = rest.find(L',');
        std::wstring_view token = rest.substr(0, comma);
        rest = comma == std::wstring_view::npos ? std::wstring_view() : rest.substr(comma + 1);

        size_t begin = token.find_first_not_of(L" \t");
        if (begin == std::wstring_view::npos) {
            continue;
        }
        token = token.substr(begin);

        std::wstring number(token);
        wchar_t* end = nullptr;
        long value = wcstol(number.c_str(), &end, 10);
        if (end == number.c_str()) {
            continue;  // not a number: keep -1
        }
        out[i] = value < 0 ? -1 : static_cast<int>(std::min<long>(value, 10000));
    }
}

void LoadSettings() {
    Settings s;

    s.offset = std::max(0, Wh_GetIntSetting(L"offset"));

    PCWSTR edgeOffsets = Wh_GetStringSetting(L"edgeOffsets");
    ParseEdgeOffsets(edgeOffsets, s.edgeOffset);
    Wh_FreeStringSetting(edgeOffsets);

    s.radius = std::max(0, Wh_GetIntSetting(L"radius"));

    s.monitorCorners = Wh_GetIntSetting(L"monitorCorners.enabled") != 0;
    s.monitorCornerRadius = std::max(0, Wh_GetIntSetting(L"monitorCorners.radius"));
    PCWSTR monitorCornerColor = Wh_GetStringSetting(L"monitorCorners.color");
    s.monitorCornerColor = ParseHexColor(monitorCornerColor, RGB(0, 0, 0));
    Wh_FreeStringSetting(monitorCornerColor);

    s.scaleWithDpi = Wh_GetIntSetting(L"scaleWithDpi") != 0;

    PCWSTR customColor = Wh_GetStringSetting(L"customColor");
    s.customColor = ParseHexColor(customColor, RGB(0x58, 0x58, 0x58));
    Wh_FreeStringSetting(customColor);

    s.colorTaskbar = Wh_GetIntSetting(L"colorTaskbar") != 0;

    s.opacity = std::clamp(Wh_GetIntSetting(L"opacity"), 0, 100);

    s.reserveWorkArea = Wh_GetIntSetting(L"reserveWorkArea") != 0;
    s.workAreaInset = std::max(0, Wh_GetIntSetting(L"workAreaInset"));

    PCWSTR monitors = Wh_GetStringSetting(L"monitors");
    s.primaryOnly = monitors && wcscmp(monitors, L"primary") == 0;
    Wh_FreeStringSetting(monitors);

    g_settings = s;
}

// ---------------------------------------------------------------------------
// Taskbar-thread helpers (from taskbar-multirow by m417z, GPL-3.0)
// ---------------------------------------------------------------------------

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcessId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

// Runs `proc` synchronously on the thread that owns `hWnd`. With a timeout,
// gives up after that long (the taskbar thread may be busy rebuilding its
// XAML); the parameter block is then intentionally leaked, since the thread
// could still pick the message up.
bool RunFromWindowThreadTimeout(HWND hWnd, RunFromWindowThreadProc_t proc, void* procParam,
                                DWORD timeoutMs) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        void* procParam;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    RUN_FROM_WINDOW_THREAD_PARAM* param =
                        (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
                    param->proc(param->procParam);
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    bool ok = true;
    if (timeoutMs == 0) {
        RUN_FROM_WINDOW_THREAD_PARAM param;
        param.proc = proc;
        param.procParam = procParam;
        SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);
    } else {
        auto* param = new RUN_FROM_WINDOW_THREAD_PARAM{proc, procParam};
        DWORD_PTR result = 0;
        if (SendMessageTimeoutW(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)param,
                                SMTO_BLOCK | SMTO_ABORTIFHUNG, timeoutMs, &result)) {
            delete param;
        } else {
            ok = false;  // timed out: `param` is leaked on purpose
        }
    }

    UnhookWindowsHookEx(hook);

    return ok;
}

// ---- Fire-and-forget calls on a window's thread -----------------------------
// Everything the mod does to the taskbar's XAML tree during normal operation
// is posted: a WH_GETMESSAGE hook on the taskbar thread runs the procedure
// when the message is retrieved, and the answer comes back as a message to
// the mod's control window. The mod's UI thread never waits on the taskbar
// thread, so a taskbar busy with a relayout (which can take seconds and run
// nested message loops) can't stall it.
struct AsyncCall {
    std::function<void()> fn;
};

UINT g_asyncMsg = 0;
std::mutex g_asyncHookMutex;
struct ThreadHooks {
    HHOOK getMessage = nullptr;   // runs posted procedures
    HHOOK callWndProc = nullptr;  // sees the taskbar window's own messages (moves)
};
std::unordered_map<DWORD, ThreadHooks> g_asyncHooks;  // thread id -> hooks

// Defined with the taskbar-side code: a taskbar window just moved/resized.
void OnTaskbarWindowPosChanged(HWND hwnd, const WINDOWPOS* pos);

LRESULT CALLBACK TaskbarCallWndProcHook(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        const CWPSTRUCT* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
        if (cwp->message == WM_WINDOWPOSCHANGED && cwp->lParam) {
            try {
                OnTaskbarWindowPosChanged(cwp->hwnd, reinterpret_cast<const WINDOWPOS*>(cwp->lParam));
            } catch (...) {
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

LRESULT CALLBACK AsyncGetMessageHook(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && (wParam & PM_REMOVE)) {
        MSG* msg = reinterpret_cast<MSG*>(lParam);
        if (msg->message == g_asyncMsg && msg->lParam) {
            std::unique_ptr<AsyncCall> call(reinterpret_cast<AsyncCall*>(msg->lParam));
            msg->message = WM_NULL;
            msg->lParam = 0;
            try {
                call->fn();
            } catch (...) {
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

bool PostToWindowThread(HWND hWnd, std::function<void()> fn) {
    if (!g_asyncMsg) {
        g_asyncMsg = RegisterWindowMessage(L"Windhawk_PostToWindowThread_" WH_MOD_ID);
    }
    DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (!threadId) {
        return false;
    }
    if (threadId == GetCurrentThreadId()) {
        fn();
        return true;
    }
    {
        std::lock_guard<std::mutex> lock(g_asyncHookMutex);
        if (g_asyncHooks.find(threadId) == g_asyncHooks.end()) {
            ThreadHooks hooks;
            hooks.getMessage = SetWindowsHookEx(WH_GETMESSAGE, AsyncGetMessageHook, nullptr, threadId);
            if (!hooks.getMessage) {
                Wh_Log(L"SetWindowsHookEx (WH_GETMESSAGE) failed: %u", GetLastError());
                return false;
            }
            hooks.callWndProc =
                SetWindowsHookEx(WH_CALLWNDPROC, TaskbarCallWndProcHook, nullptr, threadId);
            if (!hooks.callWndProc) {
                Wh_Log(L"SetWindowsHookEx (WH_CALLWNDPROC) failed: %u", GetLastError());
            }
            g_asyncHooks[threadId] = hooks;
        }
    }
    auto* call = new AsyncCall{std::move(fn)};
    if (!PostMessageW(hWnd, g_asyncMsg, 0, reinterpret_cast<LPARAM>(call))) {
        delete call;
        return false;
    }
    return true;
}

void RemoveAsyncHooks() {
    std::lock_guard<std::mutex> lock(g_asyncHookMutex);
    for (auto& entry : g_asyncHooks) {
        if (entry.second.getMessage) {
            UnhookWindowsHookEx(entry.second.getMessage);
        }
        if (entry.second.callWndProc) {
            UnhookWindowsHookEx(entry.second.callWndProc);
        }
    }
    g_asyncHooks.clear();
}

// ---- taskbar.dll: XAML root of a taskbar window ----------------------------

void* CTaskBand_ITaskListWndSite_vftable;

void* CSecondaryTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

void* TaskbarHost_FrameHeight_Original;

using CSecondaryTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CSecondaryTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

wux::XamlRoot XamlRootFromTaskbarHostSharedPtr(void* taskbarHostSharedPtr[2]) {
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        return nullptr;
    }

    size_t taskbarElementIUnknownOffset = 0x10;

#if defined(_M_X64)
    {
        // 48:83EC 28 | sub rsp,28
        // 48:83C1 48 | add rcx,48
        const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
        if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 && b[5] == 0x83 &&
            b[6] == 0xC1 && b[7] <= 0x7F) {
            taskbarElementIUnknownOffset = b[7];
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }
#elif defined(_M_ARM64)
    {
        // 7f2303d5 pacibsp
        // fd7bbfa9 stp     fp, lr, [sp, #-0x10]!
        // fd030091 mov     fp, sp
        // 080c41f8 ldr     x8, [x0, #0x10]!
        const DWORD* p = (const DWORD*)TaskbarHost_FrameHeight_Original;
        if (p[0] == 0xD503237F && (p[1] & 0xFFC07FFF) == 0xA9807BFD && p[2] == 0x910003FD &&
            (p[3] & 0xFFF00FE0) == 0xF8400C00) {
            taskbarElementIUnknownOffset = (p[3] >> 12) & 0xFF;
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }
#else
#error "Unsupported architecture"
#endif

    auto* taskbarElementIUnknown =
        *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] + taskbarElementIUnknownOffset);

    wux::FrameworkElement taskbarElement = nullptr;
    if (taskbarElementIUnknown) {  // null while the host is still being set up
        taskbarElementIUnknown->QueryInterface(winrt::guid_of<wux::FrameworkElement>(),
                                               winrt::put_abi(taskbarElement));
    }

    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);

    return result;
}

wux::XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    if (!taskBand) {
        return nullptr;  // taskbar still being constructed
    }

    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite != CTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }

        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite, taskbarHostSharedPtr);

    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

wux::XamlRoot GetSecondaryTaskbarXamlRoot(HWND hSecondaryTaskbarWnd) {
    HWND hTaskSwWnd = (HWND)FindWindowEx(hSecondaryTaskbarWnd, nullptr, L"WorkerW", nullptr);
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    if (!taskBand) {
        return nullptr;  // taskbar still being constructed
    }

    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0;
         *(void**)taskBandForTaskListWndSite != CSecondaryTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }

        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CSecondaryTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite, taskbarHostSharedPtr);

    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

// XAML root of a Shell_TrayWnd / Shell_SecondaryTrayWnd window; nullptr otherwise.
wux::XamlRoot XamlRootOfTaskbarWindow(HWND hWnd) {
    WCHAR cls[32];
    if (!GetClassName(hWnd, cls, ARRAYSIZE(cls))) {
        return nullptr;
    }
    if (_wcsicmp(cls, L"Shell_TrayWnd") == 0) {
        return GetTaskbarXamlRoot(hWnd);
    }
    if (_wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0) {
        return GetSecondaryTaskbarXamlRoot(hWnd);
    }
    return nullptr;
}

bool HookTaskbarDllSymbols() {
    HMODULE module = LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    // Address lookups only - nothing is actually hooked.
    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CSecondaryTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
            &CTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
            &TaskbarHost_FrameHeight_Original,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
            &CSecondaryTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            &std__Ref_count_base__Decref_Original,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookSymbols failed (taskbar.dll)");
        return false;
    }

    return true;
}

// ---------------------------------------------------------------------------
// Taskbar XAML tree helpers (taskbar thread only)
// ---------------------------------------------------------------------------

wux::FrameworkElement FindDescendantByName(wux::DependencyObject const& root,
                                           std::wstring_view name, int depth) {
    if (!root || depth < 0) {
        return nullptr;
    }
    int count = wuxm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        auto child = wuxm::VisualTreeHelper::GetChild(root, i);
        if (auto fe = child.try_as<wux::FrameworkElement>(); fe && fe.Name() == name) {
            return fe;
        }
        if (auto found = FindDescendantByName(child, name, depth - 1)) {
            return found;
        }
    }
    return nullptr;
}

wux::DependencyObject FindDescendantByClassName(wux::DependencyObject const& root,
                                                std::wstring_view className, int depth) {
    if (!root || depth < 0) {
        return nullptr;
    }
    int count = wuxm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        auto child = wuxm::VisualTreeHelper::GetChild(root, i);
        if (winrt::get_class_name(child) == className) {
            return child;
        }
        if (auto found = FindDescendantByClassName(child, className, depth - 1)) {
            return found;
        }
    }
    return nullptr;
}

// Root element of a taskbar window's XAML tree (its ScrollViewer), or null.
wux::UIElement TaskbarXamlContent(HWND taskbar) {
    auto xamlRoot = XamlRootOfTaskbarWindow(taskbar);
    return xamlRoot ? xamlRoot.Content() : nullptr;
}

// ... > Taskbar.TaskbarFrame > RootGrid > Taskbar.TaskbarBackground#BackgroundControl
wux::DependencyObject FindTaskbarBackground(HWND taskbar) {
    auto content = TaskbarXamlContent(taskbar);
    return content ? FindDescendantByClassName(content, L"Taskbar.TaskbarBackground", 12)
                   : nullptr;
}

// COM identity comparison (a Brush and a SolidColorBrush projection of the same
// object hold different interface pointers).
bool SameObject(winrt::Windows::Foundation::IInspectable const& a,
                winrt::Windows::Foundation::IInspectable const& b) {
    if (!a || !b) {
        return !a && !b;
    }
    return a.as<winrt::Windows::Foundation::IUnknown>() ==
           b.as<winrt::Windows::Foundation::IUnknown>();
}

// What the mod wants applied on each taskbar, kept on the taskbar thread so
// a LayoutUpdated handler can re-apply it inside the taskbar's own layout
// pass after a tree rebuild (see "Keeping the taskbar-side tweaks" below).
struct TaskbarWants {
    bool strokeHidden = false;
    bool fill = false;
    DWORD fillArgb = 0;
    bool ears = false;
    int edge = kEdgeNone;
    float radiusDips = 0.0f;
    DWORD earRgb = 0;
    bool lastReady = false;  // everything wanted was in place at the last check
    // The background is kept transparent - from the start, and again from
    // the moment the window moves in a layout change - until the UI thread
    // asks for the color once everything has settled, together with showing
    // the frame. So the taskbar never colors ahead of the frame.
    bool fillSuspended = true;
    // XAML-level veil: the root content is kept at opacity 0 from the moment
    // the window moves to another edge (or the tree is found not whole) until
    // the tree has been whole across two rendered frames, so the first frame
    // the taskbar shows again is one with the ears on the right corners.
    bool hidden = false;
    ULONGLONG hiddenSince = 0;
    bool lastWhole = false;         // the last layout pass found the tree whole
    unsigned changedAtRender = 0;   // g_renderCounter when something was last changed
    winrt::weak_ref<wux::FrameworkElement> watched;  // root content the handler is on
    winrt::event_token layoutToken{};
    winrt::weak_ref<wux::XamlRoot> rootWatched;      // XamlRoot the Changed handler is on
    winrt::event_token rootToken{};
};

// Keyed by taskbar HWND. Touched only on the taskbar thread. Heap-allocated
// and never freed (like g_strokes, g_fills and g_xamlCorners below) so no
// XAML reference is released from the wrong thread at unload.
auto* g_wants = new std::unordered_map<HWND, TaskbarWants>();

// ---------------------------------------------------------------------------
// Taskbar border line (BackgroundStroke) - taskbar thread only
// ---------------------------------------------------------------------------

struct StrokeState {
    winrt::weak_ref<wux::FrameworkElement> element;
    double originalOpacity = 1.0;
    bool hidden = false;
};

// Keyed by taskbar HWND. Touched only on the taskbar thread; leaked on purpose
// (see g_wants).
auto* g_strokes = new std::unordered_map<HWND, StrokeState>();

// Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundStroke
wux::FrameworkElement FindBackgroundStroke(HWND taskbar) {
    if (auto background = FindTaskbarBackground(taskbar)) {
        if (auto stroke = FindDescendantByName(background, L"BackgroundStroke", 4)) {
            return stroke;
        }
    }
    auto content = TaskbarXamlContent(taskbar);
    return content ? FindDescendantByName(content, L"BackgroundStroke", 16) : nullptr;
}

struct StrokeOp {
    HWND taskbar;
    bool found;    // out
    bool changed;  // out: something had to be (re)applied
};

// Hides (opacity 0) the border line of one taskbar, re-applying it if something
// reset it since. Records the original opacity the first time.
void WINAPI HideStrokeOnTaskbarThread(void* param) try {
    auto* op = static_cast<StrokeOp*>(param);
    op->found = false;
    op->changed = false;
    if (g_unloading) {
        return;
    }

    auto& strokes = *g_strokes;
    auto it = strokes.find(op->taskbar);
    wux::FrameworkElement stroke = it != strokes.end() ? it->second.element.get() : nullptr;
    if (stroke && !stroke.IsLoaded()) {
        stroke = nullptr;  // left over from a tree the taskbar has rebuilt
    }

    if (!stroke) {
        stroke = FindBackgroundStroke(op->taskbar);
        if (!stroke) {
            return;
        }
        StrokeState state;
        state.element = stroke;
        state.originalOpacity = stroke.Opacity();
        it = strokes.insert_or_assign(op->taskbar, state).first;
        Wh_Log(L"taskbar %p: BackgroundStroke found (opacity %.2f)", op->taskbar,
               state.originalOpacity);
    }

    op->found = true;
    if (stroke.Opacity() != 0.0) {
        if (!it->second.hidden) {
            it->second.originalOpacity = stroke.Opacity();
        }
        stroke.Opacity(0.0);
        it->second.hidden = true;
        op->changed = true;
    }
} catch (...) {
    Wh_Log(L"HideStrokeOnTaskbarThread: exception %08X", winrt::to_hresult().value);
}

// Restores every border line we hid and forgets them.
void WINAPI RestoreStrokesOnTaskbarThread(void*) try {
    for (auto& entry : *g_wants) {
        entry.second.strokeHidden = false;
    }
    for (auto& entry : *g_strokes) {
        StrokeState& state = entry.second;
        if (!state.hidden) {
            continue;
        }
        if (auto stroke = state.element.get()) {
            stroke.Opacity(state.originalOpacity);
            Wh_Log(L"taskbar %p: BackgroundStroke restored (opacity %.2f)", entry.first,
                   state.originalOpacity);
        }
    }
    g_strokes->clear();
} catch (...) {
    Wh_Log(L"RestoreStrokesOnTaskbarThread: exception %08X", winrt::to_hresult().value);
}

// ---------------------------------------------------------------------------
// Taskbar background fill (the frame color on the taskbar) - taskbar thread only
// ---------------------------------------------------------------------------

struct FillState {
    winrt::weak_ref<wux::FrameworkElement> target;  // Rectangle#BackgroundFill, or the
                                                    // TaskbarBackground control itself
    bool isRectangle = false;
    bool hadLocalValue = false;
    wuxm::Brush originalBrush{nullptr};
    wuxm::SolidColorBrush ourBrush{nullptr};
    bool applied = false;
};

// Keyed by taskbar HWND. Touched only on the taskbar thread; leaked on purpose
// (see g_wants).
auto* g_fills = new std::unordered_map<HWND, FillState>();

// The element that paints the taskbar background:
// Taskbar.TaskbarBackground#BackgroundControl > Grid > Rectangle#BackgroundFill
bool FindBackgroundFillTarget(HWND taskbar, FillState& state) {
    auto background = FindTaskbarBackground(taskbar);
    if (!background) {
        return false;
    }

    if (auto fill = FindDescendantByName(background, L"BackgroundFill", 4)) {
        if (fill.try_as<wuxs::Rectangle>()) {
            state.target = fill;
            state.isRectangle = true;
            return true;
        }
    }

    // No fill rectangle (changed template?): paint the control itself.
    if (auto control = background.try_as<wuxc::Control>()) {
        state.target = control;
        state.isRectangle = false;
        return true;
    }
    return false;
}

wuxm::Brush GetTargetBrush(wux::FrameworkElement const& target, bool isRectangle) {
    if (isRectangle) {
        return target.as<wuxs::Rectangle>().Fill();
    }
    return target.as<wuxc::Control>().Background();
}

void SetTargetBrush(wux::FrameworkElement const& target, bool isRectangle,
                    wuxm::Brush const& brush) {
    if (isRectangle) {
        target.as<wuxs::Rectangle>().Fill(brush);
    } else {
        target.as<wuxc::Control>().Background(brush);
    }
}

wux::DependencyProperty TargetBrushProperty(bool isRectangle) {
    return isRectangle ? wuxs::Shape::FillProperty() : wuxc::Control::BackgroundProperty();
}

struct FillOp {
    HWND taskbar;
    DWORD argb;    // AARRGGBB
    bool found;    // out
    bool changed;  // out: something had to be (re)applied
};

// Paints one taskbar's background with our solid brush, re-applying it if the
// taskbar (or another mod) swapped the brush since. Records the original the
// first time.
void WINAPI ApplyFillOnTaskbarThread(void* param) try {
    auto* op = static_cast<FillOp*>(param);
    op->found = false;
    op->changed = false;
    if (g_unloading) {
        return;
    }

    auto& fills = *g_fills;
    auto it = fills.find(op->taskbar);
    wux::FrameworkElement target = it != fills.end() ? it->second.target.get() : nullptr;
    if (target && !target.IsLoaded()) {
        target = nullptr;  // left over from a tree the taskbar has rebuilt
    }

    if (!target) {
        FillState state;
        if (!FindBackgroundFillTarget(op->taskbar, state)) {
            return;
        }
        target = state.target.get();
        it = fills.insert_or_assign(op->taskbar, state).first;
        Wh_Log(L"taskbar %p: background target found (%s)", op->taskbar,
               state.isRectangle ? L"Rectangle#BackgroundFill" : L"TaskbarBackground control");
    }

    FillState& state = it->second;
    op->found = true;

    wu::Color color{static_cast<uint8_t>(op->argb >> 24), static_cast<uint8_t>(op->argb >> 16),
                    static_cast<uint8_t>(op->argb >> 8), static_cast<uint8_t>(op->argb)};

    if (!state.ourBrush) {
        state.ourBrush = wuxm::SolidColorBrush(color);
    } else if (state.ourBrush.Color() != color) {
        state.ourBrush.Color(color);
        op->changed = true;
    }

    wuxm::Brush current = GetTargetBrush(target, state.isRectangle);
    if (!state.applied) {
        auto local = target.ReadLocalValue(TargetBrushProperty(state.isRectangle));
        state.hadLocalValue = local != wux::DependencyProperty::UnsetValue();
        state.originalBrush = current;
    }
    if (!SameObject(current, state.ourBrush)) {
        SetTargetBrush(target, state.isRectangle, state.ourBrush);
        state.applied = true;
        op->changed = true;
    }
} catch (...) {
    Wh_Log(L"ApplyFillOnTaskbarThread: exception %08X", winrt::to_hresult().value);
}

// Puts every taskbar background we painted back and forgets them.
void WINAPI RestoreFillsOnTaskbarThread(void*) try {
    for (auto& entry : *g_wants) {
        entry.second.fill = false;
        entry.second.fillSuspended = false;
    }
    for (auto& entry : *g_fills) {
        FillState& state = entry.second;
        if (!state.applied) {
            continue;
        }
        auto target = state.target.get();
        if (!target) {
            continue;
        }
        if (!SameObject(GetTargetBrush(target, state.isRectangle), state.ourBrush)) {
            continue;  // someone else already replaced our brush; leave theirs
        }
        if (state.hadLocalValue && state.originalBrush) {
            SetTargetBrush(target, state.isRectangle, state.originalBrush);
        } else {
            target.ClearValue(TargetBrushProperty(state.isRectangle));
        }
        Wh_Log(L"taskbar %p: background restored", entry.first);
    }
    g_fills->clear();
} catch (...) {
    Wh_Log(L"RestoreFillsOnTaskbarThread: exception %08X", winrt::to_hresult().value);
}

// ---------------------------------------------------------------------------
// Monitor corners on the taskbar side: XAML shapes inside the taskbar
// (taskbar thread only). A separate window would have to fight the taskbar
// for z-order; a child of the taskbar's own tree moves, hides and raises
// with it for free.
// ---------------------------------------------------------------------------

struct XamlCornerState {
    winrt::weak_ref<wuxc::Grid> host;
    wuxs::Path ear[2]{nullptr, nullptr};
    int edge = -1;             // what the ears are configured for
    float radiusDips = 0.0f;
    wu::Color color{0, 0, 0, 0};
    bool placed = true;        // host laid out at the window's size (for the log)
};

// Keyed by taskbar HWND. Touched only on the taskbar thread; leaked on purpose
// (see g_wants).
auto* g_xamlCorners = new std::unordered_map<HWND, XamlCornerState>();

// The two monitor corners that lie on the taskbar's edge.
void TaskbarEdgeCorners(int edge, int out[2]) {
    switch (edge) {
        case kEdgeTop:
            out[0] = kCornerTL;
            out[1] = kCornerTR;
            break;
        case kEdgeBottom:
            out[0] = kCornerBL;
            out[1] = kCornerBR;
            break;
        case kEdgeLeft:
            out[0] = kCornerTL;
            out[1] = kCornerBL;
            break;
        default:
            out[0] = kCornerTR;
            out[1] = kCornerBR;
            break;
    }
}

// The edge the taskbar window is on right now, from its own rect. During a
// layout change the window has already moved while the mod's posted request
// (carrying the edge the UI thread saw) may still be queued; the ears must
// follow the window, not the request.
int LiveTaskbarEdge(HWND taskbar, int fallback) {
    RECT rect{};
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    HMONITOR monitor = MonitorFromWindow(taskbar, MONITOR_DEFAULTTONEAREST);
    if (!GetWindowRect(taskbar, &rect) || rect.right <= rect.left || rect.bottom <= rect.top ||
        !monitor || !GetMonitorInfoW(monitor, &mi)) {
        return fallback;
    }
    return DetectEdge(rect, mi.rcMonitor);
}

// A Grid that spans the whole taskbar window: the parent of
// Taskbar.TaskbarFrame (Styler themes shrink the frame, not its parent), or
// the frame's RootGrid as a fallback.
wuxc::Grid FindCornerHost(HWND taskbar) {
    auto content = TaskbarXamlContent(taskbar);
    if (!content) {
        return nullptr;
    }
    auto frame = FindDescendantByClassName(content, L"Taskbar.TaskbarFrame", 12);
    if (!frame) {
        return nullptr;
    }
    if (auto parent = wuxm::VisualTreeHelper::GetParent(frame)) {
        if (auto grid = parent.try_as<wuxc::Grid>()) {
            return grid;
        }
    }
    if (auto rootGrid = FindDescendantByName(frame, L"RootGrid", 2)) {
        return rootGrid.try_as<wuxc::Grid>();
    }
    return nullptr;
}

// Ear shape in DIPs: an r x r square minus a circle of radius r centered on
// the square's inner corner (the one pointing into the screen).
void ConfigureEar(wuxs::Path const& ear, int corner, float r) {
    bool left = corner == kCornerTL || corner == kCornerBL;
    bool top = corner == kCornerTL || corner == kCornerTR;

    wuxm::GeometryGroup group;
    group.FillRule(wuxm::FillRule::EvenOdd);
    wuxm::RectangleGeometry square;
    square.Rect(winrt::Windows::Foundation::Rect{0.0f, 0.0f, r, r});
    wuxm::EllipseGeometry circle;
    circle.Center(winrt::Windows::Foundation::Point{left ? r : 0.0f, top ? r : 0.0f});
    circle.RadiusX(r);
    circle.RadiusY(r);
    group.Children().Append(square);
    group.Children().Append(circle);

    ear.Data(group);
    ear.Width(r);
    ear.Height(r);
    ear.HorizontalAlignment(left ? wux::HorizontalAlignment::Left
                                 : wux::HorizontalAlignment::Right);
    ear.VerticalAlignment(top ? wux::VerticalAlignment::Top : wux::VerticalAlignment::Bottom);
    ear.Margin(wux::Thickness{0, 0, 0, 0});
    ear.IsHitTestVisible(false);
    wuxc::Canvas::SetZIndex(ear, 10000);
}

void SetEarColor(wuxs::Path const& ear, wu::Color color) {
    auto brush = ear.Fill() ? ear.Fill().try_as<wuxm::SolidColorBrush>() : nullptr;
    if (brush) {
        brush.Color(color);
    } else {
        ear.Fill(wuxm::SolidColorBrush(color));
    }
}

struct XamlCornerOp {
    HWND taskbar;
    int edge;
    float radiusDips;  // 0 = remove
    DWORD rgb;         // COLORREF
    bool found;        // out
    bool changed;      // out: ears were added or reconfigured
    bool placed;       // out: the ears' host spans the window, so they sit on its corners
};

void RemoveEars(XamlCornerState& state) {
    auto host = state.host.get();
    for (auto& ear : state.ear) {
        if (!ear) {
            continue;
        }
        if (host) {
            uint32_t index = 0;
            if (host.Children().IndexOf(ear, index)) {
                host.Children().RemoveAt(index);
            }
        }
        ear = nullptr;
    }
}

// Adds / updates the two ears of one taskbar, re-adding them if the taskbar
// rebuilt its tree since.
void WINAPI ApplyXamlCornersOnTaskbarThread(void* param) try {
    auto* op = static_cast<XamlCornerOp*>(param);
    op->found = false;
    op->changed = false;
    op->placed = false;
    if (g_unloading) {
        return;
    }

    auto& corners = *g_xamlCorners;
    auto it = corners.find(op->taskbar);

    if (op->radiusDips <= 0.0f) {
        if (it != corners.end()) {
            RemoveEars(it->second);
            corners.erase(it);
        }
        return;
    }

    wuxc::Grid host = it != corners.end() ? it->second.host.get() : nullptr;
    bool attached = host && host.IsLoaded() && it->second.ear[0] && it->second.ear[0].Parent() &&
                    it->second.ear[1] && it->second.ear[1].Parent();
    if (!attached) {
        if (it != corners.end()) {
            RemoveEars(it->second);
            corners.erase(it);
        }
        host = FindCornerHost(op->taskbar);
        if (!host) {
            return;
        }
        XamlCornerState state;
        state.host = host;
        for (int i = 0; i < 2; i++) {
            wuxs::Path ear;
            ear.Name(i == 0 ? L"RoundEverythingCornerA" : L"RoundEverythingCornerB");
            host.Children().Append(ear);
            state.ear[i] = ear;
        }
        it = corners.insert_or_assign(op->taskbar, state).first;
        op->changed = true;
        Wh_Log(L"taskbar %p: corner ears added to %s", op->taskbar,
               host.Name().empty() ? L"(unnamed grid)" : host.Name().c_str());
    }

    // Geometry and color are only touched when something changed; this runs
    // every couple of seconds and on every layout pass of the taskbar.
    XamlCornerState& state = it->second;
    int edge = LiveTaskbarEdge(op->taskbar, op->edge);
    if (state.edge != edge || state.radiusDips != op->radiusDips) {
        int edgeCorners[2];
        TaskbarEdgeCorners(edge, edgeCorners);
        for (int i = 0; i < 2; i++) {
            ConfigureEar(state.ear[i], edgeCorners[i], op->radiusDips);
        }
        if (state.edge != edge) {
            Wh_Log(L"taskbar %p: corner ears configured for edge %d", op->taskbar, edge);
        }
        state.edge = edge;
        state.radiusDips = op->radiusDips;
        op->changed = true;
    }
    wu::Color color{255, GetRValue(op->rgb), GetGValue(op->rgb), GetBValue(op->rgb)};
    if (state.color != color) {
        for (auto const& ear : state.ear) {
            SetEarColor(ear, color);
        }
        state.color = color;
        op->changed = true;
    }
    op->found = true;

    // The ears sit on the host's corners; they are only where the window's
    // corners are once the host has been laid out at the window's size
    // (during a layout change it keeps the old size for a pass or two).
    RECT client{};
    if (GetClientRect(op->taskbar, &client)) {
        double scale = 1.0;
        if (auto root = host.XamlRoot()) {
            scale = root.RasterizationScale();
        }
        double hostW = host.ActualWidth() * scale;
        double hostH = host.ActualHeight() * scale;
        double winW = client.right - client.left;
        double winH = client.bottom - client.top;
        op->placed = std::fabs(hostW - winW) <= 2.0 && std::fabs(hostH - winH) <= 2.0;
        if (!op->placed && state.placed) {
            Wh_Log(L"taskbar %p: ears' host %.0fx%.0f vs window %.0fx%.0f, not placed yet",
                   op->taskbar, hostW, hostH, winW, winH);
        }
        state.placed = op->placed;
    }
} catch (...) {
    Wh_Log(L"ApplyXamlCornersOnTaskbarThread: exception %08X", winrt::to_hresult().value);
}

void WINAPI RemoveXamlCornersOnTaskbarThread(void*) try {
    for (auto& entry : *g_wants) {
        entry.second.ears = false;
    }
    for (auto& entry : *g_xamlCorners) {
        RemoveEars(entry.second);
    }
    g_xamlCorners->clear();
} catch (...) {
    Wh_Log(L"RemoveXamlCornersOnTaskbarThread: exception %08X", winrt::to_hresult().value);
}

// ---------------------------------------------------------------------------
// Layout: monitors, taskbars, frame geometry
// ---------------------------------------------------------------------------

struct MonitorEntry {
    HMONITOR handle;
    RECT rect;
};

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC, LPRECT, LPARAM lParam) {
    auto* list = reinterpret_cast<std::vector<MonitorEntry>*>(lParam);
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (GetMonitorInfoW(hMonitor, &mi)) {
        list->push_back({hMonitor, mi.rcMonitor});
    }
    return TRUE;
}

BOOL CALLBACK SecondaryTaskbarEnumProc(HWND hwnd, LPARAM lParam) {
    WCHAR className[64];
    if (GetClassNameW(hwnd, className, ARRAYSIZE(className)) &&
        wcscmp(className, L"Shell_SecondaryTrayWnd") == 0) {
        reinterpret_cast<std::vector<HWND>*>(lParam)->push_back(hwnd);
    }
    return TRUE;
}

int DetectEdge(const RECT& taskbar, const RECT& monitor) {
    LONG width = taskbar.right - taskbar.left;
    LONG height = taskbar.bottom - taskbar.top;
    if (width >= height) {
        LONG centerY = (taskbar.top + taskbar.bottom) / 2;
        return centerY < (monitor.top + monitor.bottom) / 2 ? kEdgeTop : kEdgeBottom;
    }
    LONG centerX = (taskbar.left + taskbar.right) / 2;
    return centerX < (monitor.left + monitor.right) / 2 ? kEdgeLeft : kEdgeRight;
}

void AppendRect(std::wstring& s, const RECT& r) {
    s += std::to_wstring(r.left) + L',' + std::to_wstring(r.top) + L',' +
         std::to_wstring(r.right) + L',' + std::to_wstring(r.bottom);
}

// ---- DWM veil for a re-created taskbar window (mod UI thread) ---------------

void StartVeil(HWND taskbar, int edge, ULONGLONG now) {
    Veil& veil = g_veils[taskbar];
    bool fresh = veil.since == 0;
    veil.since = now;
    veil.edge = edge;
    CloakWindow(taskbar, true);
    Wh_Log(L"taskbar %p: %s (edge %d)", taskbar, fresh ? L"veiled" : L"veil renewed", edge);
}

void LiftVeil(HWND taskbar, PCWSTR why) {
    auto it = g_veils.find(taskbar);
    if (it == g_veils.end()) {
        return;
    }
    if (IsWindow(taskbar)) {
        CloakWindow(taskbar, false);
    }
    Wh_Log(L"taskbar %p: veil lifted (%s)", taskbar, why);
    g_veils.erase(it);
}

// Each tick: a veil that has run its course is lifted; one the shell has
// uncloaked in the meantime is put back.
void MaintainVeils(ULONGLONG now) {
    std::vector<HWND> expired;
    for (auto& entry : g_veils) {
        if (!IsWindow(entry.first) || now - entry.second.since >= kVeilMaxMs) {
            expired.push_back(entry.first);
        } else if (!IsWindowCloaked(entry.first)) {
            CloakWindow(entry.first, true);
            Wh_Log(L"taskbar %p: re-cloaked", entry.first);
        }
    }
    for (HWND taskbar : expired) {
        LiftVeil(taskbar, L"timed out");
    }
}

void LiftAllVeils() {
    while (!g_veils.empty()) {
        LiftVeil(g_veils.begin()->first, L"unload");
    }
}

// kMsgTaskbarReady: the taskbar's tree is whole and rendered, with ears for
// `edge`.
void OnTaskbarReady(WPARAM wParam, LPARAM lParam) {
    HWND taskbar = reinterpret_cast<HWND>(lParam);
    int edge = static_cast<int>(wParam) - 1;
    g_taskbarReady[taskbar] = {GetTickCount64(), edge};
    auto it = g_veils.find(taskbar);
    if (it == g_veils.end()) {
        return;
    }
    if (edge != kEdgeNone && edge != it->second.edge) {
        Wh_Log(L"taskbar %p: tree whole for edge %d, veil is for edge %d; waiting", taskbar, edge,
               it->second.edge);
        return;
    }
    LiftVeil(taskbar, L"tree whole");
}

// Computes one layout per monitor (with or without a taskbar). Returns a
// signature string that changes whenever the geometry changes.
std::wstring ComputeLayouts(std::vector<Layout>& layouts) {
    layouts.clear();
    ULONGLONG now = GetTickCount64();

    std::vector<MonitorEntry> monitors;
    EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc,
                        reinterpret_cast<LPARAM>(&monitors));

    HWND primaryTaskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    std::vector<HWND> taskbars;
    if (primaryTaskbar) {
        taskbars.push_back(primaryTaskbar);
    }
    EnumWindows(SecondaryTaskbarEnumProc, reinterpret_cast<LPARAM>(&taskbars));

    // Auto-hide state and the primary taskbar's edge come from the appbar
    // thread (never a blocking shell call here). The reported edge is only
    // trusted while the taskbar still has the rect it was reported for;
    // during a move the rect decides.
    ShellState shell;
    {
        std::lock_guard<std::mutex> lock(g_shellMutex);
        shell = g_shellState;
    }
    bool autoHide = shell.autoHide;
    int primaryEdge = -1;
    RECT primaryRectNow{};
    if (shell.primaryEdge >= 0 && primaryTaskbar && GetWindowRect(primaryTaskbar, &primaryRectNow) &&
        EqualRect(&primaryRectNow, &shell.primaryRect)) {
        primaryEdge = shell.primaryEdge;
    }

    std::wstring signature = autoHide ? L"autohide;" : L"docked;";

    for (const MonitorEntry& mon : monitors) {
        HWND taskbar = nullptr;
        for (HWND candidate : taskbars) {
            if (MonitorFromWindow(candidate, MONITOR_DEFAULTTONEAREST) == mon.handle) {
                taskbar = candidate;
                break;
            }
        }
        if (g_settings.primaryOnly && taskbar != primaryTaskbar) {
            continue;
        }

        // The taskbar strip, as it is right now. A taskbar window that is
        // hidden, cloaked (a secondary taskbar after "show my taskbar on all
        // displays" is turned off) or empty is no taskbar: the monitor gets
        // the four-sided frame until a usable taskbar is back, and the frame
        // re-shapes the moment that happens.
        int edge = kEdgeNone;
        RECT taskbarRect{};
        bool usable = false;
        bool cloakedTooLong = false;
        if (taskbar) {
            if (IsWindowCloaked(taskbar) && !g_veils.count(taskbar)) {
                ULONGLONG& since = g_cloakedSince[taskbar];
                if (since == 0) {
                    since = now;
                }
                cloakedTooLong = now - since >= kCloakGraceMs;
            } else {
                g_cloakedSince.erase(taskbar);
            }
        }
        if (taskbar && GetWindowRect(taskbar, &taskbarRect) && IsWindowVisible(taskbar) &&
            !cloakedTooLong && taskbarRect.right > taskbarRect.left &&
            taskbarRect.bottom > taskbarRect.top) {
            usable = true;
            edge = (taskbar == primaryTaskbar && primaryEdge >= 0)
                       ? primaryEdge
                       : DetectEdge(taskbarRect, mon.rect);
            SeenTaskbar& seen = g_seenTaskbars[mon.handle];
            if (seen.hwnd != taskbar || !EqualRect(&seen.rect, &taskbarRect) || seen.edge != edge) {
                bool change = !autoHide || seen.hwnd != taskbar;
                if (change) {
                    Wh_Log(L"taskbar %p: %s (%ld,%ld,%ld,%ld), edge %d", taskbar,
                           seen.hwnd == taskbar ? L"moved/resized" : L"in use", taskbarRect.left,
                           taskbarRect.top, taskbarRect.right, taskbarRect.bottom, edge);
                }
                // A re-created window is cloaked until its tree is whole (not
                // at startup, where the taskbar has been fine all along). A
                // window that merely moved is handled inside its own thread:
                // the content stays at opacity 0 until the tree is whole.
                if (change && seen.hwnd != taskbar && g_taskbarXamlAvailable &&
                    now - g_threadStartTick >= 3000) {
                    StartVeil(taskbar, edge, now);
                }
                seen = {taskbar, taskbarRect, edge};
            }
        } else {
            auto seenIt = g_seenTaskbars.find(mon.handle);
            if (seenIt != g_seenTaskbars.end() && seenIt->second.hwnd &&
                !IsRectEmpty(&seenIt->second.rect)) {
                // Remembered (not erased): the same window coming back is a
                // move, not a new window.
                seenIt->second.rect = RECT{};
                seenIt->second.edge = kEdgeNone;
                Wh_Log(L"monitor %p: no usable taskbar (%s)", mon.handle,
                       !taskbar ? L"no window" : !IsWindowVisible(taskbar) ? L"hidden"
                                                 : cloakedTooLong ? L"cloaked for too long"
                                                                  : L"empty");
            }
            taskbar = nullptr;
            taskbarRect = RECT{};
        }

        RECT base = mon.rect;
        if (usable && !autoHide) {
            RECT clipped{};
            if (IntersectRect(&clipped, &taskbarRect, &mon.rect)) {
                switch (edge) {
                    case kEdgeTop:
                        base.top = clipped.bottom;
                        break;
                    case kEdgeBottom:
                        base.bottom = clipped.top;
                        break;
                    case kEdgeLeft:
                        base.left = clipped.right;
                        break;
                    default:
                        base.right = clipped.left;
                        break;
                }
            }
        }

        if (base.right - base.left < 4 || base.bottom - base.top < 4) {
            continue;
        }

        // The frame window covers the area next to the taskbar. With an
        // auto-hide taskbar it leaves the reveal strip to the taskbar so the
        // shell never mistakes the frame for a full-screen window.
        RECT overlayRect = base;
        if (usable && autoHide) {
            switch (edge) {
                case kEdgeTop:
                    overlayRect.top += 1;
                    break;
                case kEdgeBottom:
                    overlayRect.bottom -= 1;
                    break;
                case kEdgeLeft:
                    overlayRect.left += 1;
                    break;
                default:
                    overlayRect.right -= 1;
                    break;
            }
        }

        Layout layout;
        layout.monitor = mon.handle;
        layout.taskbar = taskbar;
        layout.monitorRect = mon.rect;
        RECT strip{};
        if (usable && !autoHide) {
            IntersectRect(&strip, &taskbarRect, &mon.rect);
        }
        layout.taskbarRect = strip;
        layout.baseWorkArea = base;
        layout.overlayRect = overlayRect;
        layout.edge = edge;
        layout.dpi = taskbar ? GetWindowDpi(taskbar) : GetMonitorDpi(mon.handle);

        // Per-edge inward offsets: the override if set, else the uniform offset
        // on the free edges and 0 on the taskbar edge (if any).
        for (int e = 0; e < 4; e++) {
            int value = g_settings.edgeOffset[e];
            if (value < 0) {
                value = (e == edge) ? 0 : g_settings.offset;
            }
            layout.offset[e] = ScaleForDpi(value, layout.dpi);
        }
        layout.workAreaInset = ScaleForDpi(g_settings.workAreaInset, layout.dpi);

        int maxCornerRadius = static_cast<int>(
            std::min(mon.rect.right - mon.rect.left, mon.rect.bottom - mon.rect.top) / 4);
        layout.monitorCornerRadius =
            g_settings.monitorCorners
                ? std::clamp(ScaleForDpi(g_settings.monitorCornerRadius, layout.dpi), 0,
                             maxCornerRadius)
                : 0;

        RECT inner = base;
        inner.left += layout.offset[kEdgeLeft];
        inner.top += layout.offset[kEdgeTop];
        inner.right -= layout.offset[kEdgeRight];
        inner.bottom -= layout.offset[kEdgeBottom];
        if (inner.right - inner.left < 2) {
            LONG cx = (base.left + base.right) / 2;
            inner.left = cx - 1;
            inner.right = cx + 1;
        }
        if (inner.bottom - inner.top < 2) {
            LONG cy = (base.top + base.bottom) / 2;
            inner.top = cy - 1;
            inner.bottom = cy + 1;
        }
        layout.innerRect = inner;

        int maxRadius =
            static_cast<int>(std::min(inner.right - inner.left, inner.bottom - inner.top) / 2);
        layout.radius = std::clamp(ScaleForDpi(g_settings.radius, layout.dpi), 0, maxRadius);

        std::wstring entry = std::to_wstring(
            static_cast<unsigned long long>(reinterpret_cast<ULONG_PTR>(mon.handle)));
        entry += L':';
        AppendRect(entry, base);
        entry += L':';
        AppendRect(entry, overlayRect);
        entry += L':';
        AppendRect(entry, layout.taskbarRect);
        entry += L':' + std::to_wstring(edge) + L':' + std::to_wstring(layout.dpi) + L':' +
                 std::to_wstring(reinterpret_cast<ULONG_PTR>(taskbar)) + L';';
        layout.signature = entry;
        signature += entry;

        layouts.push_back(layout);
    }

    return signature;
}

// ---------------------------------------------------------------------------
// Painting
// ---------------------------------------------------------------------------

void AddRoundedRectPath(Gdiplus::GraphicsPath& path, const RECT& rect, int radius) {
    using Gdiplus::REAL;
    REAL l = static_cast<REAL>(rect.left);
    REAL t = static_cast<REAL>(rect.top);
    REAL r = static_cast<REAL>(rect.right);
    REAL b = static_cast<REAL>(rect.bottom);
    REAL rad = static_cast<REAL>(radius);

    if (rad <= 0) {
        path.AddRectangle(Gdiplus::RectF(l, t, r - l, b - t));
        return;
    }

    REAL d = 2 * rad;
    path.StartFigure();
    path.AddArc(l, t, d, d, 180.0f, 90.0f);          // top-left
    path.AddArc(r - d, t, d, d, 270.0f, 90.0f);      // top-right
    path.AddArc(r - d, b - d, d, d, 0.0f, 90.0f);    // bottom-right
    path.AddArc(l, b - d, d, d, 90.0f, 90.0f);       // bottom-left
    path.CloseFigure();
}

// Renders a transparent PARGB bitmap of the frame window's size through
// `draw` and returns its pixels. Used for the one-time rasterizations.
bool RenderToPixels(int width, int height, std::vector<DWORD>& out,
                    void (*draw)(Gdiplus::Graphics&, const Layout&), const Layout& layout) {
    out.assign(static_cast<size_t>(width) * height, 0);
    Gdiplus::Bitmap bitmap(width, height, width * 4, PixelFormat32bppPARGB,
                           reinterpret_cast<BYTE*>(out.data()));
    Gdiplus::Graphics graphics(&bitmap);
    if (graphics.GetLastStatus() != Gdiplus::Ok) {
        return false;
    }
    graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    // Pixel centers at .5 so integer-aligned edges are fully covered.
    // Without this the first row/column of the bitmap only gets 50%
    // coverage and shows up as a seam next to the taskbar / screen edge.
    graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
    graphics.Clear(Gdiplus::Color(0, 0, 0, 0));
    draw(graphics, layout);
    graphics.Flush(Gdiplus::FlushIntentionSync);
    return true;
}

// Frame = whole window minus the inner rounded rectangle, in opaque white:
// the alpha channel is the coverage mask.
void DrawFrameShape(Gdiplus::Graphics& graphics, const Layout& layout) {
    int width = layout.overlayRect.right - layout.overlayRect.left;
    int height = layout.overlayRect.bottom - layout.overlayRect.top;

    // The outer rectangle overshoots the bitmap by a pixel so its edges can
    // never be anti-aliased.
    Gdiplus::GraphicsPath path(Gdiplus::FillModeAlternate);
    path.AddRectangle(Gdiplus::Rect(-1, -1, width + 2, height + 2));

    RECT inner = layout.innerRect;
    OffsetRect(&inner, -layout.overlayRect.left, -layout.overlayRect.top);
    AddRoundedRectPath(path, inner, layout.radius);

    Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 255, 255));
    graphics.FillPath(&brush, &path);
}

// Paints the sliver outside a quarter circle at each of a rectangle's corners
// that coincides with a monitor corner.
void DrawMonitorEars(Gdiplus::Graphics& graphics, const RECT& rect, const Layout& layout) {
    int cr = layout.monitorCornerRadius;
    if (cr <= 0) {
        return;
    }
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    const RECT& m = layout.monitorRect;
    auto closeTo = [](LONG a, LONG b) { return a - b <= 1 && b - a <= 1; };
    bool atLeft = closeTo(rect.left, m.left), atRight = closeTo(rect.right, m.right);
    bool atTop = closeTo(rect.top, m.top), atBottom = closeTo(rect.bottom, m.bottom);
    Gdiplus::SolidBrush cornerBrush(Gdiplus::Color(255, GetRValue(g_settings.monitorCornerColor),
                                                   GetGValue(g_settings.monitorCornerColor),
                                                   GetBValue(g_settings.monitorCornerColor)));
    for (int corner = 0; corner < 4; corner++) {
        bool left = corner == kCornerTL || corner == kCornerBL;
        bool top = corner == kCornerTL || corner == kCornerTR;
        if (!((left ? atLeft : atRight) && (top ? atTop : atBottom))) {
            continue;
        }
        int x = left ? 0 : width - cr;
        int y = top ? 0 : height - cr;
        int cx = left ? x + cr : x;  // circle center = the inner corner
        int cy = top ? y + cr : y;
        Gdiplus::GraphicsPath ear(Gdiplus::FillModeAlternate);
        ear.AddRectangle(Gdiplus::Rect(x - 1, y - 1, cr + 2, cr + 2));
        ear.AddEllipse(Gdiplus::Rect(cx - cr, cy - cr, 2 * cr, 2 * cr));
        graphics.SetClip(Gdiplus::Rect(x, y, cr, cr));
        graphics.FillPath(&cornerBrush, &ear);
        graphics.ResetClip();
    }
}

// Everything in the frame window that doesn't follow the frame color.
void DrawStaticLayer(Gdiplus::Graphics& graphics, const Layout& layout) {
    int width = layout.overlayRect.right - layout.overlayRect.left;
    int height = layout.overlayRect.bottom - layout.overlayRect.top;

    RECT inner = layout.innerRect;
    OffsetRect(&inner, -layout.overlayRect.left, -layout.overlayRect.top);

    // Extra work-area inset strip: visually wallpaper, but it holds the
    // resize borders of maximized windows, so give it 1/255 alpha to make
    // it hit-testable (zero-alpha pixels would let the mouse through).
    int hitInset = g_settings.reserveWorkArea ? layout.workAreaInset : 0;
    if (hitInset > 0) {
        RECT hole = inner;
        InflateRect(&hole, -hitInset, -hitInset);
        if (hole.right > hole.left && hole.bottom > hole.top) {
            Gdiplus::GraphicsPath strip(Gdiplus::FillModeAlternate);
            AddRoundedRectPath(strip, inner, layout.radius);
            strip.AddRectangle(Gdiplus::Rect(hole.left, hole.top, hole.right - hole.left,
                                             hole.bottom - hole.top));
            Gdiplus::SolidBrush hitBrush(Gdiplus::Color(1, 0, 0, 0));
            graphics.FillPath(&hitBrush, &strip);
        }
    }

    // Monitor corners that coincide with this window's corners (the two
    // away from the taskbar; all four without one).
    DrawMonitorEars(graphics, layout.overlayRect, layout);
}

// Rasterizes the frame's shape and static layer once and allocates the
// persistent DIB the window is updated from.
std::unique_ptr<FrameSurface> BuildFrameSurface(const Layout& layout) {
    const RECT& rect = layout.overlayRect;
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    if (width <= 0 || height <= 0) {
        return nullptr;
    }

    auto surface = std::make_unique<FrameSurface>();
    surface->width = width;
    surface->height = height;

    std::vector<DWORD> shapePixels;
    if (!RenderToPixels(width, height, shapePixels, DrawFrameShape, layout) ||
        !RenderToPixels(width, height, surface->staticLayer, DrawStaticLayer, layout)) {
        Wh_Log(L"frame rasterization failed");
        return nullptr;
    }
    surface->coverage.resize(shapePixels.size());
    for (size_t i = 0; i < shapePixels.size(); i++) {
        surface->coverage[i] = static_cast<BYTE>(shapePixels[i] >> 24);
    }

    HDC screenDc = GetDC(nullptr);
    if (!screenDc) {
        return nullptr;
    }
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;  // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    surface->dib = CreateDIBSection(screenDc, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (surface->dib && bits) {
        surface->memDc = CreateCompatibleDC(screenDc);
        surface->oldBitmap = SelectObject(surface->memDc, surface->dib);
        surface->pixels = static_cast<DWORD*>(bits);
    }
    ReleaseDC(nullptr, screenDc);
    if (!surface->memDc) {
        Wh_Log(L"CreateDIBSection failed: %u", GetLastError());
        return nullptr;
    }
    return surface;
}

// Premultiplied "source over": `top` over `bottom`. Two channels per multiply;
// the >> 8 (instead of / 255) keeps the lanes separate and is only used on
// anti-aliased edge pixels of the static layer.
inline DWORD BlendOver(DWORD top, DWORD bottom) {
    unsigned inv = 255 - (top >> 24);
    DWORD rb = (((bottom & 0x00FF00FF) * inv) >> 8) & 0x00FF00FF;
    DWORD ag = ((((bottom >> 8) & 0x00FF00FF) * inv) >> 8) & 0x00FF00FF;
    return top + (rb | (ag << 8));
}

// Pushes a frame bitmap to its window at a constant alpha (255 to show, 0
// to take it off the screen); UpdateLayeredWindow also moves and resizes the
// window to `rect`. Any thread; the caller holds g_revealMutex.
bool PresentFrame(HWND hwnd, HDC memDc, int width, int height, const RECT& rect, BYTE alpha) {
    HDC screenDc = GetDC(nullptr);
    if (!screenDc) {
        return false;
    }
    POINT dstPos{rect.left, rect.top};
    SIZE size{width, height};
    POINT srcPos{0, 0};
    BLENDFUNCTION blend{AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA};
    BOOL ok = UpdateLayeredWindow(hwnd, screenDc, &dstPos, &size, memDc, &srcPos, 0, &blend,
                                  ULW_ALPHA);
    ReleaseDC(nullptr, screenDc);
    if (!ok) {
        Wh_Log(L"UpdateLayeredWindow failed: %u", GetLastError());
    }
    return ok != FALSE;
}

// Pushes the frame's bitmap to the screen at its presentation alpha.
bool PresentOverlay(Overlay& overlay) {
    if (!overlay.hwnd || !overlay.surface) {
        return false;
    }
    std::lock_guard<std::mutex> lock(g_revealMutex);
    const FrameSurface& surface = *overlay.surface;
    return PresentFrame(overlay.hwnd, surface.memDc, surface.width, surface.height,
                        overlay.layout.overlayRect, overlay.presentAlpha);
}

// Colors the cached shape (one table lookup per pixel) and pushes it to the
// window.
bool PaintOverlay(Overlay& overlay, COLORREF rgb) {
    const Layout& layout = overlay.layout;
    if (!overlay.hwnd) {
        return false;
    }
    if (!overlay.surface) {
        overlay.surface = BuildFrameSurface(layout);
        if (!overlay.surface) {
            return false;
        }
    }
    std::lock_guard<std::mutex> lock(g_revealMutex);

    BYTE alpha = static_cast<BYTE>(g_settings.opacity * 255 / 100);
    if (alpha == 0) {
        alpha = 1;  // keep the frame hit-testable even when invisible
    }

    // Premultiplied BGRA for every coverage value.
    DWORD lut[256];
    for (unsigned c = 0; c < 256; c++) {
        unsigned a = c * alpha / 255;
        unsigned r = GetRValue(rgb) * a / 255;
        unsigned g = GetGValue(rgb) * a / 255;
        unsigned b = GetBValue(rgb) * a / 255;
        lut[c] = (a << 24) | (r << 16) | (g << 8) | b;
    }

    FrameSurface& surface = *overlay.surface;
    size_t count = surface.coverage.size();
    const BYTE* coverage = surface.coverage.data();
    const DWORD* staticLayer = surface.staticLayer.data();
    DWORD* dst = surface.pixels;
    for (size_t i = 0; i < count; i++) {
        DWORD frame = lut[coverage[i]];
        DWORD fixed = staticLayer[i];
        dst[i] = fixed ? BlendOver(fixed, frame) : frame;
    }

    overlay.painted = true;
    overlay.paintedColor = rgb;
    return PresentFrame(overlay.hwnd, surface.memDc, surface.width, surface.height,
                        layout.overlayRect, overlay.presentAlpha);
}

// ---------------------------------------------------------------------------
// Full-screen detection
// ---------------------------------------------------------------------------

bool IsIgnoredWindowClass(HWND hwnd) {
    WCHAR className[128];
    if (!GetClassNameW(hwnd, className, ARRAYSIZE(className))) {
        return true;
    }

    static const PCWSTR kIgnored[] = {
        kFrameClassName,
        kAppBarClassName,
        L"Shell_TrayWnd",
        L"Shell_SecondaryTrayWnd",
        L"Progman",
        L"WorkerW",
        L"XamlExplorerHostIslandWindow",
        L"Windows.UI.Core.CoreWindow",
        L"ForegroundStaging",
        L"MultitaskingViewFrame",
        L"TaskListThumbnailWnd",
        L"TopLevelWindowForOverflowXamlIsland",
        L"Shell_LightDismissOverlay",  // monitor-sized click catcher behind tray flyouts
        L"Shell_InputSwitchTopLevelWindow",
        L"Xaml_WindowedPopupClass",
        L"Windows.Internal.Shell.TabProxyWindow",
        L"Shell_SystemDialog",
    };
    for (PCWSTR ignored : kIgnored) {
        if (wcscmp(className, ignored) == 0) {
            return true;
        }
    }

    // Anything else explorer itself hosts (flyout islands, dismiss overlays,
    // drag images) is shell chrome, never a full-screen app. File Explorer
    // windows are the one exception worth keeping.
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == GetCurrentProcessId() && wcscmp(className, L"CabinetWClass") != 0) {
        return true;
    }

    return false;
}

// Walks the top-level windows in z-order. A visible, uncloaked window in the
// foreground that covers the whole monitor is full-screen, unless an ordinary
// window is layered above it on that monitor. `confirmed` is the window this
// monitor is already hidden for (if any): it keeps counting while it covers
// the monitor even after losing focus.
bool HasFullscreenWindow(const Layout& layout, HWND confirmed, HWND* fullscreenWnd) {
    *fullscreenWnd = nullptr;
    HWND foreground = GetForegroundWindow();

    // A maximized window fills the work area. While the work area is the
    // whole monitor (in flux: taskbar moving, appbars being re-registered)
    // a maximized window spanning the monitor means nothing; once the work
    // area is smaller than the monitor, a maximized window that still spans
    // the whole monitor is ignoring the work area - a full-screen app (a
    // browser in F11 keeps its maximized state).
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    bool workAreaReduced =
        GetMonitorInfoW(layout.monitor, &mi) && !EqualRect(&mi.rcWork, &mi.rcMonitor);

    for (HWND hwnd = GetTopWindow(nullptr); hwnd; hwnd = GetWindow(hwnd, GW_HWNDNEXT)) {
        if (!IsWindowVisible(hwnd) || IsIconic(hwnd)) {
            continue;
        }

        RECT rect{};
        if (!GetWindowRect(hwnd, &rect)) {
            continue;
        }
        RECT intersection{};
        if (!IntersectRect(&intersection, &rect, &layout.monitorRect)) {
            continue;
        }

        if (IsIgnoredWindowClass(hwnd) || IsWindowCloaked(hwnd)) {
            continue;
        }

        LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
        if (exStyle & (WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT)) {
            continue;  // overlays, click-through utilities, tool windows
        }

        if (RectContains(rect, layout.monitorRect)) {
            // A full-screen app is the window the user is in, or was in when
            // it went full-screen: a desktop-sized window that is never in
            // the foreground (wallpaper hosts, widget layers) doesn't count.
            if ((!IsZoomed(hwnd) || workAreaReduced) &&
                (hwnd == foreground || hwnd == confirmed)) {
                *fullscreenWnd = hwnd;
                return true;
            }
        }

        // Owned popups (browser "press F11 to exit" bubbles, tooltips, menus)
        // sit above their owner in z-order; they never decide the outcome.
        if (GetWindow(hwnd, GW_OWNER)) {
            continue;
        }

        // The first ordinary window on this monitor decides: if it isn't
        // full-screen, nothing beneath it matters.
        if (!(exStyle & WS_EX_TOPMOST)) {
            return false;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// Overlays
// ---------------------------------------------------------------------------

// Takes a frame's pending reveal away from the taskbar thread (a no-op if
// it already presented the frame, or none was registered). After this the
// taskbar thread won't touch the frame's bitmap.
void WithdrawReveal(Overlay& overlay) {
    if (!overlay.revealPending) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_revealMutex);
    auto it = g_reveals.find(overlay.layout.taskbar);
    if (it != g_reveals.end() && it->second.frame == overlay.hwnd) {
        g_reveals.erase(it);
    }
    overlay.revealPending = false;
}

// Takes a frame off the screen on the spot: blanked (alpha 0) and hidden, so
// nothing that might animate a window's disappearance has anything visible
// to animate. Also ends a pending reveal (whose window is visible at alpha
// 0, or already revealed by the taskbar thread).
void HideFrame(Overlay& overlay) {
    bool visible = overlay.shown || overlay.revealPending;
    WithdrawReveal(overlay);
    if (overlay.hwnd && visible) {
        overlay.presentAlpha = 0;
        PresentOverlay(overlay);  // a no-op without a surface
        ShowWindow(overlay.hwnd, SW_HIDE);
    }
    overlay.shown = false;
}

// Removes a frame on the spot.
void DestroyOverlay(Overlay& overlay) {
    if (overlay.hwnd) {
        HideFrame(overlay);
        DestroyWindow(overlay.hwnd);
        overlay.hwnd = nullptr;
    }
}

// Takes every frame off the screen on the spot, leaving the windows to be
// destroyed later.
void HideOverlays(std::vector<Overlay>& overlays) {
    for (Overlay& overlay : overlays) {
        HideFrame(overlay);
    }
}

void DestroyOverlays(std::vector<Overlay>& overlays) {
    for (Overlay& overlay : overlays) {
        DestroyOverlay(overlay);
    }
    overlays.clear();
}

Overlay* FindOverlay(HMONITOR monitor) {
    for (Overlay& overlay : g_overlays) {
        if (overlay.layout.monitor == monitor) {
            return &overlay;
        }
    }
    return nullptr;
}

HWND CreateFrameWindow(const RECT& rect) {
    // No WS_EX_TRANSPARENT: the layered window still passes the mouse
    // through wherever its pixels have zero alpha (the inner area), while
    // the painted frame swallows it - like the taskbar does. That keeps
    // the resize borders of maximized windows under the frame unreachable.
    DWORD exStyle = WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TOPMOST;
    HWND hwnd = CreateWindowExW(exStyle, kFrameClassName, L"Round Everything", WS_POPUP,
                                rect.left, rect.top, rect.right - rect.left,
                                rect.bottom - rect.top, nullptr, nullptr, g_module, nullptr);
    if (!hwnd) {
        Wh_Log(L"CreateWindowExW failed: %u", GetLastError());
        return nullptr;
    }

    // Aero Peek (taskbar thumbnail hover, show-desktop corner hover) fades
    // every other window to glass; keep the frame painted through it.
    BOOL excludedFromPeek = TRUE;
    DwmSetWindowAttribute(hwnd, static_cast<DWMWINDOWATTRIBUTE>(12) /* DWMWA_EXCLUDED_FROM_PEEK */,
                          &excludedFromPeek, sizeof(excludedFromPeek));
    // No DWM show/hide animation: a frame that is hidden or destroyed must
    // vanish on the spot rather than dim out.
    BOOL noTransitions = TRUE;
    DwmSetWindowAttribute(hwnd,
                          static_cast<DWMWINDOWATTRIBUTE>(3) /* DWMWA_TRANSITIONS_FORCEDISABLED */,
                          &noTransitions, sizeof(noTransitions));
    return hwnd;
}

// Brings the frames in line with the layouts: one persistent frame window per
// monitor. A monitor whose layout changed (the taskbar moved, resized, came
// or went) keeps its window and gets a new shape; with a taskbar it is then
// shown again together with it. New monitors get a new frame, vanished
// monitors lose theirs.
void WaitForTaskbar(Overlay& overlay, ULONGLONG now, PCWSTR why);  // defined with the visibility code

void SyncOverlays(const std::vector<Layout>& layouts) {
    // Frames of monitors that are gone.
    for (Overlay& overlay : g_overlays) {
        bool present = false;
        for (const Layout& layout : layouts) {
            present = present || layout.monitor == overlay.layout.monitor;
        }
        if (!present) {
            DestroyOverlay(overlay);
        }
    }
    g_overlays.erase(std::remove_if(g_overlays.begin(), g_overlays.end(),
                                    [](const Overlay& o) { return o.hwnd == nullptr; }),
                     g_overlays.end());

    for (const Layout& layout : layouts) {
        if (Overlay* existing = FindOverlay(layout.monitor)) {
            if (existing->layout.signature == layout.signature) {
                existing->layout = layout;  // same geometry, refreshed handles
                continue;
            }
            bool taskbarChanged = existing->layout.taskbar != layout.taskbar ||
                                  existing->layout.edge != layout.edge ||
                                  !EqualRect(&existing->layout.taskbarRect, &layout.taskbarRect);
            if (existing->revealPending) {
                HideFrame(*existing);  // the old shape must not be revealed
            }
            existing->layout = layout;
            existing->surface.reset();  // rebuilt by the paint below
            if (layout.taskbar && taskbarChanged) {
                // Shown again together with the taskbar once it is whole
                // (a change of settings alone re-shapes on the spot).
                WaitForTaskbar(*existing, GetTickCount64(), L"taskbar changed");
            }
            if (existing->shown) {
                // Re-shape in place; UpdateLayeredWindow moves and resizes
                // the window along with the new bitmap.
                PaintOverlay(*existing, existing->painted ? existing->paintedColor
                                                          : g_settings.customColor);
            } else {
                SetWindowPos(existing->hwnd, nullptr, layout.overlayRect.left,
                             layout.overlayRect.top,
                             layout.overlayRect.right - layout.overlayRect.left,
                             layout.overlayRect.bottom - layout.overlayRect.top,
                             SWP_NOZORDER | SWP_NOACTIVATE);
                existing->painted = false;
            }
            Wh_Log(L"monitor %p: frame re-shaped (taskbar %p, edge %d)", layout.monitor,
                   layout.taskbar, layout.edge);
            continue;
        }

        Overlay overlay;
        overlay.layout = layout;
        overlay.hwnd = CreateFrameWindow(layout.overlayRect);
        if (!overlay.hwnd) {
            continue;
        }
        if (layout.taskbar) {
            overlay.waiting = true;  // appears together with the taskbar
            overlay.waitSince = GetTickCount64();
        }
        PaintOverlay(overlay, g_settings.customColor);
        g_overlays.push_back(std::move(overlay));
    }
}

// True when the frame is no longer above ordinary windows: it lost the
// topmost bit, or a visible non-topmost window precedes it in z-order over
// the frame's area.
bool FrameNeedsRaise(const Overlay& overlay) {
    if (!(GetWindowLongPtrW(overlay.hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST)) {
        return true;
    }
    for (HWND hwnd = GetTopWindow(nullptr); hwnd && hwnd != overlay.hwnd;
         hwnd = GetWindow(hwnd, GW_HWNDNEXT)) {
        if (!IsWindowVisible(hwnd) || IsIconic(hwnd)) {
            continue;
        }
        if (GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST) {
            continue;  // other topmost windows above us are fine
        }
        RECT rect{};
        if (!GetWindowRect(hwnd, &rect)) {
            continue;
        }
        RECT intersection{};
        if (!IntersectRect(&intersection, &rect, &overlay.layout.overlayRect)) {
            continue;
        }
        if (IsIgnoredWindowClass(hwnd) || IsWindowCloaked(hwnd)) {
            continue;
        }
        return true;
    }
    return false;
}

void RaiseFrame(Overlay& overlay, bool show) {
    SetWindowPos(overlay.hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | (show ? SWP_SHOWWINDOW : 0));
}

// Takes a frame off the screen on the spot and starts waiting for its
// taskbar; it comes back together with the taskbar.
void WaitForTaskbar(Overlay& overlay, ULONGLONG now, PCWSTR why) {
    if (!overlay.layout.taskbar) {
        return;
    }
    if (overlay.shown || overlay.revealPending) {
        HideFrame(overlay);
        Wh_Log(L"monitor %p: frame hidden (%s)", overlay.layout.monitor, why);
    } else if (!overlay.waiting) {
        Wh_Log(L"monitor %p: frame waits for its taskbar (%s)", overlay.layout.monitor, why);
    }
    if (!overlay.waiting) {
        overlay.waitSince = now;
        overlay.recolorRequestTick = 0;
        overlay.recolorSerial = 0;
    }
    overlay.waiting = true;
}

bool AppBarSetInPlace(HMONITOR monitor, const RECT& baseWorkArea);  // appbar code, below

// Defined with the taskbar-side code: asks the taskbar thread to paint the
// background again in its next rendering pass, revealing the frame registered
// in g_reveals with it; it answers with kMsgTaskbarRecolored, carrying the
// serial, from that same rendering pass.
void RequestTaskbarRecolor(HWND taskbar, UINT serial);

// Whether the taskbar has rendered the recolor with the given serial.
bool TaskbarRecolored(HWND taskbar, UINT serial) {
    auto it = g_taskbarRecolored.find(taskbar);
    return it != g_taskbarRecolored.end() && it->second >= serial;
}

// Whether the frame is revealed through the taskbar (there must be something
// of the mod inside the taskbar for its rendering pass to carry).
bool UsesHandshake() {
    return g_taskbarXamlAvailable && (g_settings.colorTaskbar || g_settings.monitorCorners);
}

// The handshake: the frame is made ready (painted, its window visible but at
// alpha 0, so there is nothing to see yet) and handed to the taskbar thread,
// which presents it at full alpha in the rendering pass that also paints the
// taskbar's background, before that pass commits - the two reach DWM for the
// same composition. Returns the recolor request's serial; its answer says
// the frame has been presented.
UINT BeginReveal(Overlay& overlay) {
    HWND taskbar = overlay.layout.taskbar;
    overlay.presentAlpha = 0;
    if (!overlay.painted) {
        PaintOverlay(overlay, g_settings.customColor);
    } else {
        PresentOverlay(overlay);
    }
    RaiseFrame(overlay, true);
    UINT serial = ++g_recolorSerial;
    if (overlay.surface) {
        std::lock_guard<std::mutex> lock(g_revealMutex);
        const FrameSurface& surface = *overlay.surface;
        g_reveals[taskbar] = {serial, overlay.hwnd, surface.memDc, surface.width, surface.height,
                              overlay.layout.overlayRect};
        overlay.revealPending = true;
    }
    RequestTaskbarRecolor(taskbar, serial);
    return serial;
}

// Whether a waiting frame may show. Everything has to have settled first:
// the taskbar's tree reported whole-and-rendered for the frame's edge after
// the wait began, the shell isn't cloaking the taskbar, and the work area
// is reserved for the new layout. Then the frame is handed to the taskbar
// thread (BeginReveal), which paints the taskbar's background and reveals
// the frame in one rendering pass; this returns true once that has happened
// (the frame is on its way to the screen by then). A wait that runs too long
// skips to that step, so even then the two still appear together whenever
// the taskbar answers in time; a reveal that isn't answered in time is done
// by the UI thread itself.
bool TaskbarReadyFor(Overlay& overlay, ULONGLONG now) {
    HWND taskbar = overlay.layout.taskbar;
    if (!taskbar || !overlay.waiting) {
        return true;
    }
    if (overlay.recolorRequestTick) {
        // Settled; waiting for the taskbar to have painted its background.
        return TaskbarRecolored(taskbar, overlay.recolorSerial) ||
               now - overlay.recolorRequestTick >= kRecolorWaitMaxMs;
    }
    if (now - overlay.waitSince >= kTaskbarWaitMaxMs) {
        const TaskbarReady& ready = g_taskbarReady[taskbar];
        Wh_Log(L"monitor %p: wait given up (taskbar %s, tree %s, work area %s); taskbar asked "
               L"to color",
               overlay.layout.monitor, IsWindowCloaked(taskbar) ? L"still cloaked" : L"uncloaked",
               ready.tick >= overlay.waitSince ? L"whole" : L"not reported whole",
               AppBarSetInPlace(overlay.layout.monitor, overlay.layout.baseWorkArea) ? L"in place"
                                                                                     : L"pending");
        if (!UsesHandshake()) {
            return true;
        }
        overlay.recolorRequestTick = now;
        overlay.recolorSerial = BeginReveal(overlay);
        return false;
    }
    if (IsWindowCloaked(taskbar) || !IsWindowVisible(taskbar)) {
        return false;
    }
    const TaskbarReady& ready = g_taskbarReady[taskbar];
    if (ready.tick < overlay.waitSince ||
        (ready.edge != kEdgeNone && ready.edge != overlay.layout.edge)) {
        return false;
    }
    if (g_settings.reserveWorkArea &&
        !AppBarSetInPlace(overlay.layout.monitor, overlay.layout.baseWorkArea)) {
        return false;
    }
    if (!UsesHandshake()) {
        return true;
    }
    overlay.recolorRequestTick = now;
    overlay.recolorSerial = BeginReveal(overlay);
    Wh_Log(L"monitor %p: settled, taskbar asked to color and reveal the frame",
           overlay.layout.monitor);
    return false;
}

// Defined with the taskbar-side code: makes the taskbar's background
// transparent right away (the color comes back with RequestTaskbarRecolor).
void RequestTaskbarTransparent(HWND taskbar);

// Task View (Win+Tab) is a visible, monitor-sized island window that holds
// the foreground (it is driven by the keyboard). The same window class also
// hosts Alt+Tab and snap assist (not monitor-sized) and the taskbar's
// thumbnail previews (monitor-sized, but hovering a button activates
// nothing, so they are never in the foreground). Checked the moment such a
// window shows or hides (in-process WinEvent), so the reaction doesn't wait
// for the next tick.
bool IsTaskViewWindowClass(HWND hwnd) {
    WCHAR cls[64];
    return GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) &&
           (wcscmp(cls, L"XamlExplorerHostIslandWindow") == 0 ||
            wcscmp(cls, L"MultitaskingViewFrame") == 0);
}

bool SpansItsMonitor(HWND hwnd) {
    RECT rect{};
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    return GetWindowRect(hwnd, &rect) && monitor && GetMonitorInfoW(monitor, &mi) &&
           RectContains(rect, mi.rcMonitor);
}

// A visible, monitor-sized window of the Task View classes that is in the
// foreground or titled "Task View" (the title is there from the moment the
// window shows, before the foreground has moved to it).
bool IsTaskViewWindow(HWND hwnd) {
    if (!IsWindowVisible(hwnd) || IsWindowCloaked(hwnd) || !SpansItsMonitor(hwnd)) {
        return false;
    }
    if (hwnd == GetForegroundWindow()) {
        return true;
    }
    WCHAR title[64];
    return GetWindowTextW(hwnd, title, ARRAYSIZE(title)) > 0 && wcscmp(title, L"Task View") == 0;
}

// The open Task View window, or null.
HWND FindTaskViewWindow() {
    static const PCWSTR kClasses[] = {L"XamlExplorerHostIslandWindow", L"MultitaskingViewFrame"};
    for (PCWSTR cls : kClasses) {
        for (HWND hwnd = FindWindowExW(nullptr, nullptr, cls, nullptr); hwnd;
             hwnd = FindWindowExW(nullptr, hwnd, cls, nullptr)) {
            if (IsTaskViewWindow(hwnd)) {
                return hwnd;
            }
        }
    }
    return nullptr;
}

// kMsgTaskbarMoving (from the taskbar thread's hook): the window just moved
// or resized - the frame on that monitor goes away right now.
void OnTaskbarMoving(LPARAM lParam) {
    HWND taskbar = reinterpret_cast<HWND>(lParam);
    ULONGLONG now = GetTickCount64();
    for (Overlay& overlay : g_overlays) {
        if (overlay.layout.taskbar == taskbar) {
            WaitForTaskbar(overlay, now, L"taskbar moving");
        }
    }
}

void UpdateVisibility() {
    static ULONGLONG lastRaiseLogTick = 0;
    ULONGLONG now = GetTickCount64();
    HWND taskViewWnd = FindTaskViewWindow();
    bool taskView = taskViewWnd != nullptr;

    for (Overlay& overlay : g_overlays) {
        HWND fullscreenWnd = nullptr;
        bool detected = HasFullscreenWindow(overlay.layout, overlay.fullscreenWnd, &fullscreenWnd);
        // The foreground window itself spanning the monitor (F11, a game) is
        // acted on at once. Anything else detected must hold for two
        // consecutive ticks before the frame hides, so a transient z-order
        // state (show desktop, window switching bursts) can't blink it.
        // Coming back is immediate.
        overlay.fullscreenTicks = detected ? overlay.fullscreenTicks + 1 : 0;
        bool immediate = detected && fullscreenWnd && fullscreenWnd == GetForegroundWindow();
        bool fullscreen =
            detected && (immediate || overlay.fullscreenTicks >= 2 || overlay.fullscreen);
        if (fullscreen != overlay.fullscreen) {
            WCHAR cls[64] = L"";
            if (fullscreenWnd) {
                GetClassNameW(fullscreenWnd, cls, ARRAYSIZE(cls));
            }
            Wh_Log(L"monitor %p: full-screen %s%s%s", overlay.layout.monitor,
                   fullscreen ? L"on" : L"off", fullscreen ? L" - " : L"",
                   fullscreen ? cls : L"");
        }
        overlay.fullscreen = fullscreen;
        overlay.fullscreenWnd = fullscreen ? fullscreenWnd : nullptr;

        // A taskbar the shell has cloaked (a layout change in progress) takes
        // its frame with it; the frame comes back with the taskbar.
        HWND taskbar = overlay.layout.taskbar;
        if (taskbar && !overlay.waiting && overlay.shown && IsWindowCloaked(taskbar) &&
            !g_veils.count(taskbar)) {
            WaitForTaskbar(overlay, now, L"taskbar cloaked");
        }

        // Task View: taskbar transparent and frame gone the moment it opens;
        // when it closes, the taskbar is asked to color again and to reveal
        // the frame with it (the same handshake as after a layout change).
        if (taskView && !overlay.taskViewHidden) {
            overlay.taskViewHidden = true;
            overlay.taskViewRecolorTick = 0;
            overlay.taskViewRecolorSerial = 0;
            if (taskbar) {
                RequestTaskbarTransparent(taskbar);
            }
            WCHAR title[64] = L"";
            GetWindowTextW(taskViewWnd, title, ARRAYSIZE(title));
            Wh_Log(L"monitor %p: Task View open (%p \"%s\"%s), frame off and taskbar transparent",
                   overlay.layout.monitor, taskViewWnd, title,
                   taskViewWnd == GetForegroundWindow() ? L", foreground" : L"");
        }
        bool taskViewHolds = false;
        if (!taskView && overlay.taskViewHidden) {
            if (!taskbar) {
                overlay.taskViewHidden = false;
            } else if (overlay.waiting) {
                overlay.taskViewHidden = false;  // the wait recolors it, with the frame
            } else if (overlay.fullscreen || !UsesHandshake()) {
                // The frame stays off (or shows on its own); the taskbar just
                // gets its color and ears back.
                overlay.taskViewHidden = false;
                RequestTaskbarRecolor(taskbar, ++g_recolorSerial);
            } else if (!overlay.taskViewRecolorTick) {
                overlay.taskViewRecolorTick = now;
                overlay.taskViewRecolorSerial = BeginReveal(overlay);
                taskViewHolds = true;
            } else if (TaskbarRecolored(taskbar, overlay.taskViewRecolorSerial) ||
                       now - overlay.taskViewRecolorTick >= kRecolorWaitMaxMs) {
                overlay.taskViewHidden = false;
                Wh_Log(L"monitor %p: Task View closed, frame back with the taskbar's color",
                       overlay.layout.monitor);
            } else {
                taskViewHolds = true;
            }
        }

        bool show = !overlay.fullscreen && !taskView && !taskViewHolds &&
                    TaskbarReadyFor(overlay, now);

        if (show && !overlay.shown) {
            // Revealed by the taskbar thread already (the usual case: the
            // recolor answered), or presented here (no handshake, or one
            // that wasn't answered in time).
            bool revealed = false;
            if (overlay.revealPending) {
                std::lock_guard<std::mutex> lock(g_revealMutex);
                auto it = g_reveals.find(taskbar);
                revealed = it == g_reveals.end() || it->second.frame != overlay.hwnd;
            }
            WithdrawReveal(overlay);
            overlay.presentAlpha = 255;
            if (!revealed) {
                if (!overlay.painted) {
                    PaintOverlay(overlay, g_settings.customColor);
                } else {
                    PresentOverlay(overlay);
                }
                RaiseFrame(overlay, true);
            }
            overlay.shown = true;
            if (overlay.waiting) {
                overlay.waiting = false;
                Wh_Log(L"monitor %p: taskbar ready, frame shown %s", overlay.layout.monitor,
                       revealed ? L"by the taskbar with its color" : L"by the mod");
            } else {
                Wh_Log(L"monitor %p: frame shown%s", overlay.layout.monitor,
                       revealed ? L" by the taskbar with its color" : L"");
            }
        } else if (!show && (overlay.shown || overlay.revealPending)) {
            Wh_Log(L"monitor %p: frame hidden (%s)", overlay.layout.monitor,
                   overlay.fullscreen ? L"full-screen" : taskView ? L"Task View" : L"waiting");
            HideFrame(overlay);
        } else if (show) {
            if (!IsWindowVisible(overlay.hwnd) || IsIconic(overlay.hwnd)) {
                Wh_Log(L"monitor %p: frame not visible although shown, re-showing",
                       overlay.layout.monitor);
                RaiseFrame(overlay, true);
            }
            // Watchdog: keep the frame above ordinary windows, like the taskbar.
            if (FrameNeedsRaise(overlay)) {
                RaiseFrame(overlay, false);
                if (now - lastRaiseLogTick >= 5000) {
                    lastRaiseLogTick = now;
                    Wh_Log(L"monitor %p: frame re-ordered", overlay.layout.monitor);
                }
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Work area: invisible appbars, one per frame edge with a non-zero offset.
// Everything in this section runs on the appbar thread (see g_appBarThread).
// ---------------------------------------------------------------------------

LRESULT CALLBACK AppBarWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case kMsgAppBarCallback:
            if (wParam == ABN_POSCHANGED) {
                g_appBarsDirty = true;  // re-query on the next pass
            }
            return 0;
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_ERASEBKGND:
            return 1;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

RECT ProposedAppBarRect(const RECT& base, UINT edge, int thickness) {
    RECT rc = base;
    switch (edge) {
        case ABE_LEFT:
            rc.right = rc.left + thickness;
            break;
        case ABE_TOP:
            rc.bottom = rc.top + thickness;
            break;
        case ABE_RIGHT:
            rc.left = rc.right - thickness;
            break;
        default:
            rc.top = rc.bottom - thickness;
            break;
    }
    return rc;
}

// ABM_QUERYPOS + ABM_SETPOS for one appbar, keeping its thickness.
void SetAppBarPosition(const RECT& base, AppBar& bar) {
    APPBARDATA abd{};
    abd.cbSize = sizeof(abd);
    abd.hWnd = bar.hwnd;
    abd.uEdge = bar.edge;
    abd.rc = ProposedAppBarRect(base, bar.edge, bar.thickness);

    SHAppBarMessage(ABM_QUERYPOS, &abd);

    // The shell may have shifted the strip (e.g. past the taskbar); keep the
    // thickness and the far edge where the query left them.
    switch (bar.edge) {
        case ABE_LEFT:
            abd.rc.right = abd.rc.left + bar.thickness;
            break;
        case ABE_TOP:
            abd.rc.bottom = abd.rc.top + bar.thickness;
            break;
        case ABE_RIGHT:
            abd.rc.left = abd.rc.right - bar.thickness;
            break;
        default:
            abd.rc.top = abd.rc.bottom - bar.thickness;
            break;
    }

    SHAppBarMessage(ABM_SETPOS, &abd);

    SetWindowPos(bar.hwnd, nullptr, abd.rc.left, abd.rc.top, abd.rc.right - abd.rc.left,
                 abd.rc.bottom - abd.rc.top, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOOWNERZORDER);
}

void RecordWorkArea(AppBarSet& set) {
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (GetMonitorInfoW(set.monitor, &mi)) {
        set.workAreaAfterApply = mi.rcWork;
    }
}

void SetAppBarPositions(AppBarSet& set) {
    for (AppBar& bar : set.bars) {
        SetAppBarPosition(set.baseWorkArea, bar);
    }
    RecordWorkArea(set);
}

// Frame strip plus the extra inset per edge; the taskbar edge gets only the
// extra inset (the shell stacks that appbar right next to the taskbar).
void DesiredThickness(const Layout& layout, int out[4]) {
    for (int e = 0; e < 4; e++) {
        out[e] = layout.offset[e] + layout.workAreaInset;
    }
}

// Registers one appbar on one edge and positions it.
bool CreateAppBar(const RECT& base, UINT edge, int thickness, AppBar& bar) {
    bar.edge = edge;  // kEdge* values match ABE_*
    bar.thickness = thickness;

    RECT rc = ProposedAppBarRect(base, bar.edge, thickness);
    bar.hwnd = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT | WS_EX_LAYERED, kAppBarClassName,
        L"Round Everything AppBar", WS_POPUP, rc.left, rc.top, rc.right - rc.left,
        rc.bottom - rc.top, nullptr, nullptr, g_module, nullptr);
    if (!bar.hwnd) {
        Wh_Log(L"CreateWindowExW (appbar) failed: %u", GetLastError());
        return false;
    }
    SetLayeredWindowAttributes(bar.hwnd, 0, 0, LWA_ALPHA);  // invisible, but "visible"

    APPBARDATA abd{};
    abd.cbSize = sizeof(abd);
    abd.hWnd = bar.hwnd;
    abd.uCallbackMessage = kMsgAppBarCallback;
    if (!SHAppBarMessage(ABM_NEW, &abd)) {
        Wh_Log(L"ABM_NEW failed for edge %u", edge);
        DestroyWindow(bar.hwnd);
        bar.hwnd = nullptr;
        return false;
    }

    ShowWindow(bar.hwnd, SW_SHOWNOACTIVATE);
    SetAppBarPosition(base, bar);
    return true;
}

void RemoveAppBar(AppBar& bar) {
    if (!bar.hwnd) {
        return;
    }
    APPBARDATA abd{};
    abd.cbSize = sizeof(abd);
    abd.hWnd = bar.hwnd;
    SHAppBarMessage(ABM_REMOVE, &abd);
    DestroyWindow(bar.hwnd);
    bar.hwnd = nullptr;
}

// Brings one monitor's set in line with a wish, touching only the edges that
// differ: an appbar that stays is re-positioned, not re-registered. Every
// shell call here changes the work area and makes every window re-layout,
// so the fewer the better.
void UpdateAppBarSet(AppBarSet& set, const AppBarWish& wish) {
    set.baseWorkArea = wish.baseWorkArea;
    for (int e = 0; e < 4; e++) {
        int wanted = wish.thickness[e];
        AppBar* bar = nullptr;
        for (AppBar& candidate : set.bars) {
            if (candidate.edge == static_cast<UINT>(e)) {
                bar = &candidate;
                break;
            }
        }
        if (wanted <= 0) {
            if (bar) {
                RemoveAppBar(*bar);
                set.bars.erase(set.bars.begin() + (bar - set.bars.data()));
            }
        } else if (bar) {
            bar->thickness = wanted;
            SetAppBarPosition(set.baseWorkArea, *bar);
        } else {
            AppBar created;
            if (CreateAppBar(set.baseWorkArea, static_cast<UINT>(e), wanted, created)) {
                set.bars.push_back(created);
            }
        }
        set.thickness[e] = wanted;
    }
    RecordWorkArea(set);
    Wh_Log(L"monitor %p: %u appbar(s) in place", wish.monitor,
           static_cast<unsigned>(set.bars.size()));
}

void RemoveAppBarSet(AppBarSet& set) {
    for (AppBar& bar : set.bars) {
        RemoveAppBar(bar);
    }
    set.bars.clear();
}

void RemoveAllAppBars(std::vector<AppBarSet>& sets) {
    for (AppBarSet& set : sets) {
        RemoveAppBarSet(set);
    }
    sets.clear();
}

// Tells the UI thread what is in place.
void PublishAppBarStatus(const std::vector<AppBarSet>& sets) {
    std::vector<AppBarStatus> status;
    for (const AppBarSet& set : sets) {
        status.push_back({set.monitor, set.baseWorkArea});
    }
    std::lock_guard<std::mutex> lock(g_shellMutex);
    g_appBarStatus = std::move(status);
}

// Keeps one appbar set per monitor, re-positioning only when the wished
// strips changed or when the shell changed the
// work area since the last pass (taskbar moved/resized, another appbar came
// or went) or told us so through ABN_POSCHANGED. Never fights the shell.
void ReconcileAppBars(std::vector<AppBarSet>& sets, ULONGLONG& lastWorkAreaTick,
                      std::vector<AppBarWish>& lastWishes) {
    std::vector<AppBarWish> wishes;
    bool wanted;
    {
        std::lock_guard<std::mutex> lock(g_shellMutex);
        wishes = g_appBarWishes;
        wanted = g_appBarsWanted;
    }
    if (!wanted) {
        if (!sets.empty()) {
            RemoveAllAppBars(sets);
            PublishAppBarStatus(sets);
        }
        lastWishes.clear();
        return;
    }

    // A changed wish list gets its positions right away, not after the
    // retry interval.
    bool wishesChanged = wishes.size() != lastWishes.size();
    for (size_t i = 0; !wishesChanged && i < wishes.size(); i++) {
        wishesChanged = wishes[i].monitor != lastWishes[i].monitor ||
                        !EqualRect(&wishes[i].baseWorkArea, &lastWishes[i].baseWorkArea) ||
                        !std::equal(wishes[i].thickness, wishes[i].thickness + 4,
                                    lastWishes[i].thickness);
    }
    if (wishesChanged) {
        lastWorkAreaTick = 0;
    }
    lastWishes = wishes;

    bool dirty = g_appBarsDirty.exchange(false);
    ULONGLONG now = GetTickCount64();

    for (const AppBarWish& wish : wishes) {
        AppBarSet* set = nullptr;
        for (AppBarSet& candidate : sets) {
            if (candidate.monitor == wish.monitor) {
                set = &candidate;
                break;
            }
        }
        if (!set) {
            AppBarSet created;
            created.monitor = wish.monitor;
            sets.push_back(std::move(created));
            set = &sets.back();
            UpdateAppBarSet(*set, wish);
            continue;
        }
        if (!EqualRect(&set->baseWorkArea, &wish.baseWorkArea) ||
            !std::equal(wish.thickness, wish.thickness + 4, set->thickness)) {
            UpdateAppBarSet(*set, wish);
            continue;
        }
        if (set->bars.empty()) {
            continue;
        }

        MONITORINFO mi{};
        mi.cbSize = sizeof(mi);
        if (!GetMonitorInfoW(wish.monitor, &mi)) {
            continue;
        }
        bool changed = !EqualRect(&mi.rcWork, &set->workAreaAfterApply);
        if ((changed || dirty) && now - lastWorkAreaTick >= kWorkAreaRetryMs) {
            SetAppBarPositions(*set);
            lastWorkAreaTick = now;
        }
    }

    // Monitors that are gone take their appbars with them.
    for (size_t i = 0; i < sets.size();) {
        HMONITOR monitor = sets[i].monitor;
        bool present = false;
        for (const AppBarWish& wish : wishes) {
            present = present || wish.monitor == monitor;
        }
        if (present) {
            i++;
        } else {
            RemoveAppBarSet(sets[i]);
            sets.erase(sets.begin() + i);
        }
    }

    PublishAppBarStatus(sets);
}

// ABM_GETSTATE / ABM_GETTASKBARPOS, for the UI thread to read.
void RefreshShellState() {
    ShellState state;
    APPBARDATA stateData{};
    stateData.cbSize = sizeof(stateData);
    state.autoHide = (SHAppBarMessage(ABM_GETSTATE, &stateData) & ABS_AUTOHIDE) != 0;

    APPBARDATA posData{};
    posData.cbSize = sizeof(posData);
    if (SHAppBarMessage(ABM_GETTASKBARPOS, &posData) && posData.uEdge <= ABE_BOTTOM) {
        state.primaryEdge = static_cast<int>(posData.uEdge);
        state.primaryRect = posData.rc;
    }

    std::lock_guard<std::mutex> lock(g_shellMutex);
    g_shellState = state;
}

DWORD WINAPI AppBarThreadProc(LPVOID) {
    SetThreadPerMonitorDpiAware();

    std::vector<AppBarSet> sets;
    std::vector<AppBarWish> lastWishes;
    ULONGLONG lastWorkAreaTick = 0;
    ULONGLONG lastStateTick = 0;

    RefreshShellState();
    SetEvent(g_shellStateReady);

    // Not the mod's stop event: the appbars (each removal is a work-area
    // change that keeps the taskbar thread busy) are only taken down once
    // the UI thread says so - after the taskbar has been handed back.
    HANDLE handles[] = {g_appBarStop, g_appBarWake};
    for (;;) {
        DWORD wait = MsgWaitForMultipleObjectsEx(2, handles, 1000, QS_ALLINPUT,
                                                 MWMO_INPUTAVAILABLE);
        if (wait == WAIT_OBJECT_0 || wait == WAIT_FAILED) {
            break;
        }

        // The appbar windows live on this thread: their ABN callbacks are
        // pumped here.
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        ULONGLONG now = GetTickCount64();
        if (now - lastStateTick >= 250) {
            RefreshShellState();
            lastStateTick = now;
        }
        ReconcileAppBars(sets, lastWorkAreaTick, lastWishes);
    }

    RemoveAllAppBars(sets);
    PublishAppBarStatus(sets);
    return 0;
}

// UI thread: publishes what every monitor should reserve and wakes the
// appbar thread. Cheap, called every tick.
void PublishAppBarWishes() {
    std::vector<AppBarWish> wishes;
    for (const Overlay& overlay : g_overlays) {
        const Layout& layout = overlay.layout;
        AppBarWish wish;
        wish.monitor = layout.monitor;
        wish.baseWorkArea = layout.baseWorkArea;
        DesiredThickness(layout, wish.thickness);
        wishes.push_back(wish);
    }
    {
        std::lock_guard<std::mutex> lock(g_shellMutex);
        g_appBarWishes = std::move(wishes);
        g_appBarsWanted = g_settings.reserveWorkArea;
    }
    if (g_appBarWake) {
        SetEvent(g_appBarWake);
    }
}

// UI thread: whether the appbar thread has this monitor's set in place for
// the given work area.
bool AppBarSetInPlace(HMONITOR monitor, const RECT& baseWorkArea) {
    std::lock_guard<std::mutex> lock(g_shellMutex);
    for (const AppBarStatus& status : g_appBarStatus) {
        if (status.monitor == monitor) {
            return EqualRect(&status.baseWorkArea, &baseWorkArea) != FALSE;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// Keeping the taskbar-side tweaks across tree rebuilds - taskbar thread only
// ---------------------------------------------------------------------------
// When the taskbar rebuilds its XAML tree (a layout change), the stroke, fill
// and ears are gone with the old tree. The mod's posted requests only run
// once the taskbar thread pumps messages again, which can be a second after
// the new tree is already on screen. So what the mod wants on each taskbar is
// kept here, and a LayoutUpdated handler on the taskbar's XAML root content
// re-applies it from inside the taskbar's own layout pass - before the new
// tree is first presented.

// Longest the XAML-level veil keeps a taskbar's content invisible.
constexpr ULONGLONG kXamlVeilMaxMs = 1500;

// Counts the taskbar thread's rendering ticks while content is hidden or a
// recolor is waiting (CompositionTarget.Rendering; subscribing to it also
// keeps frames coming).
unsigned g_renderCounter = 0;
winrt::event_token g_renderingToken{};
bool g_renderingSubscribed = false;

void OnRendering(winrt::Windows::Foundation::IInspectable const&,
                 winrt::Windows::Foundation::IInspectable const&);

void EnsureRenderingSubscribed() {
    if (g_renderingSubscribed) {
        return;
    }
    g_renderingToken = wuxm::CompositionTarget::Rendering(OnRendering);
    g_renderingSubscribed = true;
}

void UnsubscribeRendering() {
    if (g_renderingSubscribed) {
        wuxm::CompositionTarget::Rendering(g_renderingToken);
        g_renderingSubscribed = false;
    }
}

void SetContentHidden(HWND taskbar, TaskbarWants& wants, bool hide) {
    auto content = TaskbarXamlContent(taskbar);
    if (!content) {
        return;
    }
    if (hide) {
        if (content.Opacity() != 0.0) {
            content.Opacity(0.0);
        }
        if (!wants.hidden) {
            wants.hidden = true;
            wants.hiddenSince = GetTickCount64();
            Wh_Log(L"taskbar %p: content hidden until whole", taskbar);
        }
        wants.lastWhole = false;
        wants.changedAtRender = g_renderCounter;
        EnsureRenderingSubscribed();
    } else if (wants.hidden) {
        content.ClearValue(wux::UIElement::OpacityProperty());
        wants.hidden = false;
        Wh_Log(L"taskbar %p: content shown (after %llu ms)", taskbar,
               GetTickCount64() - wants.hiddenSince);
    }
}

// The UI thread learns that a taskbar is whole only once that state has
// been rendered (CompositionTarget.Rendered), so the frame it shows on that
// signal appears in the same frame as the taskbar's new look.
std::vector<std::pair<HWND, int>> g_readyPending;
winrt::event_token g_readyRenderToken{};

void ReportReadyAfterRender(HWND taskbar, int edge) try {
    for (auto& entry : g_readyPending) {
        if (entry.first == taskbar) {
            entry.second = edge;
            return;
        }
    }
    g_readyPending.push_back({taskbar, edge});
    if (g_readyRenderToken) {
        return;
    }
    g_readyRenderToken = wuxm::CompositionTarget::Rendered(
        [](winrt::Windows::Foundation::IInspectable const&, wuxm::RenderedEventArgs const&) {
            wuxm::CompositionTarget::Rendered(g_readyRenderToken);
            g_readyRenderToken = {};
            auto pending = std::move(g_readyPending);
            g_readyPending.clear();
            HWND ctrl = g_ctrlWnd.load();
            for (auto const& entry : pending) {
                if (ctrl) {
                    PostMessageW(ctrl, kMsgTaskbarReady, static_cast<WPARAM>(entry.second + 1),
                                 reinterpret_cast<LPARAM>(entry.first));
                }
            }
        });
} catch (...) {
    Wh_Log(L"ReportReadyAfterRender: exception %08X", winrt::to_hresult().value);
}

// Recolors (the UI thread asks for one once everything has settled, or when
// Task View closes). A request waits for the next CompositionTarget.Rendering
// tick: the brush change made there is part of the frame that tick builds,
// and the frame window registered for it is presented in the same tick,
// right after the change - before the tick commits its frame to DWM. Both
// then reach DWM ahead of the same vertical blank and come up in one
// composition. (Presenting on CompositionTarget.Rendered instead trails by a
// frame or two: that event fires once the taskbar's frame is already out.)
struct PendingRecolor {
    HWND taskbar;
    UINT serial;
};
std::vector<PendingRecolor> g_recolorsWaiting;  // for the next Rendering tick

// The monitor-corner ears inside the taskbar go with the background: both
// off in Task View, both back together.
void SetEarsHidden(HWND taskbar, bool hidden) {
    auto corner = g_xamlCorners->find(taskbar);
    if (corner == g_xamlCorners->end()) {
        return;
    }
    for (auto const& ear : corner->second.ear) {
        if (ear) {
            ear.Opacity(hidden ? 0.0 : 1.0);
        }
    }
}

void QueueRecolor(std::vector<PendingRecolor>& list, HWND taskbar, UINT serial) {
    for (auto& entry : list) {
        if (entry.taskbar == taskbar) {
            entry.serial = std::max(entry.serial, serial);  // the later one covers it
            return;
        }
    }
    list.push_back({taskbar, serial});
}

// Forgets recolors asked for before a taskbar went transparent again (a
// move, Task View): they must not color it behind the UI thread's back.
void DropRecolors(HWND taskbar) {
    auto drop = [taskbar](std::vector<PendingRecolor>& list) {
        list.erase(std::remove_if(list.begin(), list.end(),
                                  [taskbar](const PendingRecolor& r) { return r.taskbar == taskbar; }),
                   list.end());
    };
    drop(g_recolorsWaiting);
}

// Makes sure this tick renders a frame (so Rendered fires) even when the
// recolor changed nothing visible (the color was already on): a change of
// opacity by one millionth on the fill brush, or failing that on an ear -
// invisible, but it dirties the tree.
void NudgeRender(HWND taskbar) {
    static bool flip = false;
    flip = !flip;
    double value = flip ? 0.999999 : 1.0;
    if (auto fill = g_fills->find(taskbar); fill != g_fills->end() && fill->second.ourBrush) {
        fill->second.ourBrush.Opacity(value);
        return;
    }
    if (auto corner = g_xamlCorners->find(taskbar);
        corner != g_xamlCorners->end() && corner->second.ear[0]) {
        corner->second.ear[0].Opacity(value);
    }
}

// Rendering tick: for every taskbar with a recolor waiting, paints the
// background and shows the ears (part of the frame this tick builds), then
// presents the frame window registered for it and tells the UI thread.
void ApplyRecolorsInRenderingTick() {
    if (g_recolorsWaiting.empty()) {
        return;
    }
    auto waiting = std::move(g_recolorsWaiting);
    g_recolorsWaiting.clear();
    HWND ctrl = g_ctrlWnd.load();
    for (const PendingRecolor& recolor : waiting) {
        auto it = g_wants->find(recolor.taskbar);
        if (it != g_wants->end() && !g_unloading) {
            TaskbarWants& wants = it->second;
            wants.fillSuspended = false;
            if (wants.fill) {
                FillOp op{recolor.taskbar, wants.fillArgb, false, false};
                ApplyFillOnTaskbarThread(&op);
            }
            SetEarsHidden(recolor.taskbar, false);
        }
        NudgeRender(recolor.taskbar);
        {
            std::lock_guard<std::mutex> lock(g_revealMutex);
            auto reveal = g_reveals.find(recolor.taskbar);
            if (reveal != g_reveals.end() && reveal->second.serial <= recolor.serial) {
                const Reveal& r = reveal->second;
                PresentFrame(r.frame, r.memDc, r.width, r.height, r.rect, 255);
                g_reveals.erase(reveal);
            }
        }
        if (ctrl) {
            PostMessageW(ctrl, kMsgTaskbarRecolored, recolor.serial,
                         reinterpret_cast<LPARAM>(recolor.taskbar));
        }
    }
}

// UI thread: posts the recolor (with the request's serial, which the
// kMsgTaskbarRecolored answer carries) to the taskbar's thread.
void RequestTaskbarRecolor(HWND taskbar, UINT serial) {
    PostToWindowThread(taskbar, [taskbar, serial] {
        if (g_unloading) {
            return;
        }
        QueueRecolor(g_recolorsWaiting, taskbar, serial);
        EnsureRenderingSubscribed();
    });
}

void WINAPI TransparentOnTaskbarThread(void* param) try {
    HWND taskbar = static_cast<HWND>(param);
    auto it = g_wants->find(taskbar);
    if (it == g_wants->end() || g_unloading) {
        return;
    }
    TaskbarWants& wants = it->second;
    DropRecolors(taskbar);
    wants.fillSuspended = true;
    if (wants.fill) {
        FillOp op{taskbar, wants.fillArgb & 0x00FFFFFF, false, false};
        ApplyFillOnTaskbarThread(&op);
    }
    SetEarsHidden(taskbar, true);
} catch (...) {
    Wh_Log(L"TransparentOnTaskbarThread: exception %08X", winrt::to_hresult().value);
}

// UI thread: "background and ears transparent now" on the taskbar's thread.
// Sent (sent messages go ahead of everything queued) with a short wait, so
// an idle taskbar thread does it within the call; if it is busy, posted.
void RequestTaskbarTransparent(HWND taskbar) {
    if (RunFromWindowThreadTimeout(taskbar, TransparentOnTaskbarThread, taskbar, 30)) {
        return;
    }
    PostToWindowThread(taskbar, [taskbar] { TransparentOnTaskbarThread(taskbar); });
}

// Re-applies what is wanted on one taskbar; a no-op (a few cheap checks)
// while everything is in place. Returns whether it all is, and tells the UI
// thread (kMsgTaskbarReady, with the edge the ears are built for) whenever
// that becomes true - or again when `requested` - so a waiting frame can be
// shown the moment the taskbar is whole.
bool ReapplyWants(HWND taskbar, bool requested) try {
    if (g_unloading) {
        return false;
    }
    auto it = g_wants->find(taskbar);
    if (it == g_wants->end()) {
        return false;
    }
    TaskbarWants& wants = it->second;
    bool found = true;
    bool clean = true;
    bool placed = true;
    bool earsRight = true;  // what the XAML-level veil is about: the corners
    int edge = kEdgeNone;
    if (wants.strokeHidden) {
        StrokeOp op{taskbar, false, false};
        HideStrokeOnTaskbarThread(&op);
        found = found && op.found;
        clean = clean && !op.changed;
    }
    if (wants.fill) {
        FillOp op{taskbar, wants.fillSuspended ? (wants.fillArgb & 0x00FFFFFF) : wants.fillArgb,
                  false, false};
        ApplyFillOnTaskbarThread(&op);
        found = found && op.found;
        clean = clean && !op.changed;
    }
    if (wants.ears) {
        XamlCornerOp op{taskbar, wants.edge, wants.radiusDips, wants.earRgb, false, false, false};
        ApplyXamlCornersOnTaskbarThread(&op);
        found = found && op.found;
        clean = clean && !op.changed;
        placed = op.placed;
        earsRight = op.found && !op.changed && op.placed;
        auto corner = g_xamlCorners->find(taskbar);
        if (corner != g_xamlCorners->end()) {
            edge = corner->second.edge;
        }
    }
    bool whole = found && clean && placed;

    // The XAML-level veil goes up here whenever the corners aren't right
    // (ears missing, just added or reconfigured in this pass, or not yet on
    // the window's corners). It comes down in OnRendering, once the tree has
    // been whole and untouched across two rendered frames: a change made
    // during a layout pass is not in that pass's frame yet.
    if (!earsRight) {
        if (!wants.hidden || wants.changedAtRender != g_renderCounter) {
            SetContentHidden(taskbar, wants, true);
        }
    }
    wants.lastWhole = whole;

    if (whole && (!wants.lastReady || requested)) {
        Wh_Log(L"taskbar %p: tree whole (ears for edge %d)", taskbar, edge);
        ReportReadyAfterRender(taskbar, edge);
    }
    wants.lastReady = whole;
    return whole;
} catch (...) {
    Wh_Log(L"ReapplyWants: exception %08X", winrt::to_hresult().value);
    return false;
}

// Once per rendering tick while content is hidden or a recolor is waiting.
// Changes made in this handler are part of the frame about to be rendered.
void OnRendering(winrt::Windows::Foundation::IInspectable const&,
                 winrt::Windows::Foundation::IInspectable const&) try {
    g_renderCounter++;
    ApplyRecolorsInRenderingTick();
    bool anyHidden = !g_recolorsWaiting.empty();
    for (auto& entry : *g_wants) {
        HWND taskbar = entry.first;
        TaskbarWants& wants = entry.second;
        if (!wants.hidden) {
            continue;
        }
        if (g_unloading || !IsWindow(taskbar)) {
            SetContentHidden(taskbar, wants, false);
            continue;
        }
        if (!wants.lastWhole) {
            ReapplyWants(taskbar, false);  // no layout pass came by itself
        }
        bool settled = wants.lastWhole && g_renderCounter >= wants.changedAtRender + 2;
        bool overdue = GetTickCount64() - wants.hiddenSince >= kXamlVeilMaxMs;
        if (settled || overdue) {
            if (overdue && !settled) {
                Wh_Log(L"taskbar %p: shown anyway (whole %d, renders since change %u)", taskbar,
                       wants.lastWhole ? 1 : 0, g_renderCounter - wants.changedAtRender);
            }
            SetContentHidden(taskbar, wants, false);
        } else {
            anyHidden = true;
        }
    }
    if (!anyHidden) {
        UnsubscribeRendering();
    }
} catch (...) {
    Wh_Log(L"OnRendering: exception %08X", winrt::to_hresult().value);
}

// WM_WINDOWPOSCHANGED on a taskbar window (WH_CALLWNDPROC hook, taskbar
// thread): the earliest moment a layout change can be seen from inside. If
// the window is now on another edge than the ears are built for, the content
// goes invisible before anything of the new position is rendered.
void OnTaskbarWindowPosChanged(HWND hwnd, const WINDOWPOS* pos) {
    if (g_unloading || ((pos->flags & SWP_NOMOVE) && (pos->flags & SWP_NOSIZE))) {
        return;
    }
    auto it = g_wants->find(hwnd);
    if (it == g_wants->end()) {
        return;
    }
    // Edge or size changed: the frame on this monitor goes away with the
    // taskbar and comes back once the tree is whole again (a slide along the
    // same edge - auto-hide - is left alone).
    auto corner = g_xamlCorners->find(hwnd);
    int oldEdge = corner != g_xamlCorners->end() ? corner->second.edge : kEdgeNone;
    int edge = LiveTaskbarEdge(hwnd, oldEdge);
    if (edge == oldEdge && (pos->flags & SWP_NOSIZE)) {
        return;
    }
    Wh_Log(L"taskbar %p: window now on edge %d (ears for %d)", hwnd, edge, oldEdge);
    TaskbarWants& wants = it->second;
    wants.lastReady = false;  // report whole again, for the new place
    DropRecolors(hwnd);
    if (wants.fill && !wants.fillSuspended) {
        // Transparent right away; the color comes back with the frame.
        wants.fillSuspended = true;
        FillOp op{hwnd, wants.fillArgb & 0x00FFFFFF, false, false};
        ApplyFillOnTaskbarThread(&op);
        Wh_Log(L"taskbar %p: background made transparent for the move", hwnd);
    }
    if (HWND ctrl = g_ctrlWnd.load()) {
        PostMessageW(ctrl, kMsgTaskbarMoving, 0, reinterpret_cast<LPARAM>(hwnd));
    }
    if (it->second.ears && corner != g_xamlCorners->end()) {
        SetContentHidden(hwnd, it->second, true);
    }
}

// Puts the LayoutUpdated handler on the taskbar's current XAML root content
// (moving it when the content was replaced).
void WatchTaskbarLayout(HWND taskbar) try {
    if (g_unloading) {
        return;
    }
    TaskbarWants& wants = (*g_wants)[taskbar];
    auto content = TaskbarXamlContent(taskbar);
    auto element = content ? content.try_as<wux::FrameworkElement>() : nullptr;
    auto watched = wants.watched.get();
    if (watched == element) {
        return;
    }
    if (watched && wants.layoutToken) {
        watched.LayoutUpdated(wants.layoutToken);
    }
    wants.watched = nullptr;
    wants.layoutToken = {};
    if (!element) {
        return;
    }
    wants.layoutToken = element.LayoutUpdated(
        [taskbar](winrt::Windows::Foundation::IInspectable const&,
                  winrt::Windows::Foundation::IInspectable const&) { ReapplyWants(taskbar, false); });
    wants.watched = element;
    wants.lastReady = false;  // a new tree: report again once it is whole
    Wh_Log(L"taskbar %p: watching its layout (%s content)", taskbar,
           watched ? L"replaced" : L"first");
} catch (...) {
    Wh_Log(L"WatchTaskbarLayout: exception %08X", winrt::to_hresult().value);
}

// The XamlRoot outlives its content: its Changed event (size, scale,
// visibility) is where a replaced content tree is picked up and watched.
void WatchTaskbarRoot(HWND taskbar) try {
    if (g_unloading) {
        return;
    }
    TaskbarWants& wants = (*g_wants)[taskbar];
    auto root = XamlRootOfTaskbarWindow(taskbar);
    auto watched = wants.rootWatched.get();
    if (watched == root) {
        return;
    }
    if (watched && wants.rootToken) {
        watched.Changed(wants.rootToken);
    }
    wants.rootWatched = nullptr;
    wants.rootToken = {};
    if (!root) {
        return;
    }
    wants.rootToken = root.Changed([taskbar](wux::XamlRoot const&,
                                             wux::XamlRootChangedEventArgs const&) {
        Wh_Log(L"taskbar %p: XamlRoot changed", taskbar);
        WatchTaskbarLayout(taskbar);
        ReapplyWants(taskbar, false);
    });
    wants.rootWatched = root;
    Wh_Log(L"taskbar %p: watching its XamlRoot (%s)", taskbar, watched ? L"replaced" : L"first");
} catch (...) {
    Wh_Log(L"WatchTaskbarRoot: exception %08X", winrt::to_hresult().value);
}

// Takes every LayoutUpdated handler off again (unload). Must complete before
// the mod's code goes away: a handler left behind would be called into
// unloaded code.
void WINAPI UnwatchTaskbarsOnTaskbarThread(void*) try {
    UnsubscribeRendering();
    if (g_readyRenderToken) {
        wuxm::CompositionTarget::Rendered(g_readyRenderToken);
        g_readyRenderToken = {};
    }
    g_readyPending.clear();
    g_recolorsWaiting.clear();
    for (auto& entry : *g_wants) {
        TaskbarWants& wants = entry.second;
        if (wants.hidden && IsWindow(entry.first)) {
            SetContentHidden(entry.first, wants, false);
        }
        if (auto watched = wants.watched.get(); watched && wants.layoutToken) {
            watched.LayoutUpdated(wants.layoutToken);
        }
        wants.watched = nullptr;
        wants.layoutToken = {};
        if (auto root = wants.rootWatched.get(); root && wants.rootToken) {
            root.Changed(wants.rootToken);
        }
        wants.rootWatched = nullptr;
        wants.rootToken = {};
    }
    g_wants->clear();
} catch (...) {
    Wh_Log(L"UnwatchTaskbarsOnTaskbarThread: exception %08X", winrt::to_hresult().value);
}

// Unload: one trip to the taskbar thread hands everything back at once -
// handlers off, content shown, ears removed, background and stroke restored -
// so the taskbar changes in a single frame of its own.
winrt::event_token g_restoredRenderToken{};

// Drops the Rendered subscription if it never fired (the handler must not
// outlive the mod).
void WINAPI UnsubscribeRestoredRenderOnTaskbarThread(void*) try {
    if (g_restoredRenderToken) {
        wuxm::CompositionTarget::Rendered(g_restoredRenderToken);
        g_restoredRenderToken = {};
    }
} catch (...) {
}

void WINAPI RestoreTaskbarsOnTaskbarThread(void*) try {
    UnwatchTaskbarsOnTaskbarThread(nullptr);
    RemoveXamlCornersOnTaskbarThread(nullptr);
    RestoreFillsOnTaskbarThread(nullptr);
    RestoreStrokesOnTaskbarThread(nullptr);

    // Tell the UI thread once the handed-back taskbar has actually been
    // rendered, so the frames can go in the same instant it changes.
    g_restoredRenderToken = wuxm::CompositionTarget::Rendered(
        [](winrt::Windows::Foundation::IInspectable const&, wuxm::RenderedEventArgs const&) {
            wuxm::CompositionTarget::Rendered(g_restoredRenderToken);
            g_restoredRenderToken = {};
            if (g_taskbarRestored) {
                SetEvent(g_taskbarRestored);
            }
        });
} catch (...) {
    Wh_Log(L"RestoreTaskbarsOnTaskbarThread: exception %08X", winrt::to_hresult().value);
    if (g_taskbarRestored) {
        SetEvent(g_taskbarRestored);
    }
}

// ---------------------------------------------------------------------------
// Taskbar XAML drivers (mod UI thread -> taskbar thread)
// ---------------------------------------------------------------------------

// Runs a restore procedure on the taskbar thread (all taskbars share it). The
// `owned` flag is cleared either way; a missing taskbar window means Explorer
// is going away and there is no tree left to restore.
void RestoreOnTaskbarThread(RunFromWindowThreadProc_t proc, bool& owned, PCWSTR what) {
    if (!g_taskbarXamlAvailable || !owned) {
        return;
    }
    if (HWND taskbar = FindCurrentProcessTaskbarWnd()) {
        RunFromWindowThreadTimeout(taskbar, proc, nullptr, 2000);
    } else {
        Wh_Log(L"no taskbar window, skipping %s restore", what);
    }
    owned = false;
}

// A pass is requested on a taskbar (posted to its thread) unless a request is
// already out; the answer arrives through kMsgPassResult. A pass is re-sent
// when it reported "not found" (the tree is being rebuilt), every
// kTaskbarXamlRetryMs to re-assert it, or right away when `force` is set.
void RequestPass(XamlPass pass, HWND taskbar, ULONGLONG now, bool force,
                 std::function<bool()> op) {
    PassState& state = g_passState[pass][taskbar];
    if (state.outstanding && now - state.sentTick < kTaskbarCallLostMs) {
        return;
    }
    bool due = force || (!state.done && now - state.answerTick >= kPassRetryMs) ||
               (state.done && now - state.doneTick >= kTaskbarXamlRetryMs);
    if (!due) {
        return;
    }
    UINT serial = ++g_requestSerial;
    HWND ctrl = g_ctrlWnd.load();
    if (PostToWindowThread(taskbar, [op, pass, serial, taskbar, ctrl] {
            bool found = false;
            try {
                found = op();
            } catch (...) {
            }
            if (ctrl) {
                PostMessageW(ctrl, kMsgPassResult, MAKEWPARAM(pass | (found ? 0x100 : 0), serial),
                             reinterpret_cast<LPARAM>(taskbar));
            }
        })) {
        state.outstanding = true;
        state.sentTick = now;
        state.serial = serial;
    }
}

// kMsgPassResult handler (UI thread).
void OnPassResult(WPARAM wParam, LPARAM lParam) {
    int pass = LOWORD(wParam) & 0xFF;
    bool found = (LOWORD(wParam) & 0x100) != 0;
    UINT serial = HIWORD(wParam);
    HWND taskbar = reinterpret_cast<HWND>(lParam);
    if (pass < 0 || pass >= kPassCount) {
        return;
    }
    auto it = g_passState[pass].find(taskbar);
    if (it == g_passState[pass].end() || (it->second.serial & 0xFFFF) != serial) {
        return;  // a request from before a layout change
    }
    PassState& state = it->second;
    ULONGLONG now = GetTickCount64();
    state.outstanding = false;
    state.answerTick = now;
    state.done = found;
    if (found) {
        state.doneTick = now;
        if (pass == kPassCorners) {
            g_xamlCornersOwned = true;
        } else if (pass == kPassFill) {
            g_fillOwned = true;
        }
    }
}

// Forgets every pass state: after a layout change each taskbar has to be
// confirmed again (answers to older requests are dropped by their serial).
void ResetPassStates() {
    for (auto& states : g_passState) {
        states.clear();
    }
}

void HideStrokes(ULONGLONG now) {
    if (!g_taskbarXamlAvailable) {
        return;
    }
    for (const Overlay& overlay : g_overlays) {
        HWND taskbar = overlay.layout.taskbar;
        if (!taskbar) {
            continue;
        }
        RequestPass(kPassStroke, taskbar, now, false, [taskbar] {
            (*g_wants)[taskbar].strokeHidden = true;
            WatchTaskbarRoot(taskbar);
            WatchTaskbarLayout(taskbar);
            StrokeOp op{taskbar, false, false};
            HideStrokeOnTaskbarThread(&op);
            ReapplyWants(taskbar, true);
            return op.found;
        });
    }
}

// The brush painted onto the taskbar background: the frame color at the
// frame's opacity, so the taskbar is the frame's fourth side.
DWORD CustomColorArgb() {
    BYTE alpha = static_cast<BYTE>(g_settings.opacity * 255 / 100);
    COLORREF rgb = g_settings.customColor;
    return (static_cast<DWORD>(alpha) << 24) | (static_cast<DWORD>(GetRValue(rgb)) << 16) |
           (static_cast<DWORD>(GetGValue(rgb)) << 8) | GetBValue(rgb);
}

void ApplyFills(DWORD argb, ULONGLONG now) {
    if (!g_taskbarXamlAvailable) {
        return;
    }
    bool force = argb != g_lastFillArgb;  // a new color is applied at once
    g_lastFillArgb = argb;
    for (const Overlay& overlay : g_overlays) {
        HWND taskbar = overlay.layout.taskbar;
        if (!taskbar) {
            continue;
        }
        RequestPass(kPassFill, taskbar, now, force, [taskbar, argb] {
            TaskbarWants& wants = (*g_wants)[taskbar];
            wants.fill = true;
            wants.fillArgb = argb;
            WatchTaskbarRoot(taskbar);
            WatchTaskbarLayout(taskbar);
            FillOp op{taskbar, wants.fillSuspended ? (argb & 0x00FFFFFF) : argb, false, false};
            ApplyFillOnTaskbarThread(&op);
            ReapplyWants(taskbar, true);
            return op.found;
        });
    }
}

void RestoreFills() {
    RestoreOnTaskbarThread(RestoreFillsOnTaskbarThread, g_fillOwned, L"taskbar background");
}

// Radius in DIPs for the XAML ears (XAML scales by the taskbar's DPI itself).
float XamlCornerRadiusDips(const Layout& layout) {
    if (!g_settings.monitorCorners || layout.monitorCornerRadius <= 0) {
        return 0.0f;
    }
    if (g_settings.scaleWithDpi) {
        return static_cast<float>(g_settings.monitorCornerRadius);
    }
    return static_cast<float>(layout.monitorCornerRadius) * 96.0f / static_cast<float>(layout.dpi);
}

void ApplyXamlCorners(ULONGLONG now) {
    if (!g_taskbarXamlAvailable) {
        return;
    }
    for (const Overlay& overlay : g_overlays) {
        const Layout& layout = overlay.layout;
        HWND taskbar = layout.taskbar;
        if (!taskbar) {
            continue;  // all four ears are painted into the frame itself
        }
        int edge = layout.edge;
        float radius = XamlCornerRadiusDips(layout);
        DWORD rgb = g_settings.monitorCornerColor;
        RequestPass(kPassCorners, taskbar, now, false, [taskbar, edge, radius, rgb] {
            TaskbarWants& wants = (*g_wants)[taskbar];
            wants.ears = radius > 0.0f;
            wants.edge = edge;
            wants.radiusDips = radius;
            wants.earRgb = rgb;
            WatchTaskbarRoot(taskbar);
            WatchTaskbarLayout(taskbar);
            XamlCornerOp op{taskbar, edge, radius, rgb, false, false, false};
            ApplyXamlCornersOnTaskbarThread(&op);
            ReapplyWants(taskbar, true);
            return op.found;
        });
    }
}

void RemoveXamlCorners() {
    RestoreOnTaskbarThread(RemoveXamlCornersOnTaskbarThread, g_xamlCornersOwned, L"corner ear");
}

// ---------------------------------------------------------------------------
// Periodic update (mod UI thread)
// ---------------------------------------------------------------------------

void Tick(bool forceRebuild) {
    if (g_inTick) {
        return;
    }
    g_inTick = true;
    ULONGLONG tStart = GetTickCount64();
    // Diagnostics: a tick that comes much later than scheduled means this
    // thread was held up somewhere (message loop starved or a call blocked).
    if (g_lastTickTime && tStart - g_lastTickTime > 1000) {
        Wh_Log(L"tick gap: %llu ms since the previous tick", tStart - g_lastTickTime);
    }
    g_lastTickTime = tStart;

    if (g_reloadRequested.exchange(false)) {
        LoadSettings();
        forceRebuild = true;
    }

    std::vector<Layout> layouts;
    std::wstring signature = ComputeLayouts(layouts);
    if (forceRebuild || signature != g_layoutSignature ||
        layouts.size() != g_overlays.size()) {
        if (forceRebuild) {
            // Settings changed: every frame is rebuilt from scratch.
            for (Overlay& overlay : g_overlays) {
                overlay.layout.signature.clear();
            }
        }
        SyncOverlays(layouts);
        g_layoutSignature = signature;
        ResetPassStates();
    }

    ULONGLONG now = GetTickCount64();
    MaintainVeils(now);

    // Work area: the sooner the shell shrinks it after a taskbar change, the
    // sooner maximized windows stop spanning the monitor. The appbar thread
    // does the (blocking) shell calls.
    PublishAppBarWishes();

    // Taskbar-side tweaks: each is requested on the taskbar thread and
    // confirmed asynchronously, re-sent while a taskbar's tree isn't ready
    // and re-asserted every couple of seconds (the taskbar may rebuild its
    // tree or another mod may reset a value). Nothing here waits.
    HideStrokes(now);

    if (g_settings.monitorCorners) {
        ApplyXamlCorners(now);
    } else if (g_xamlCornersOwned) {
        RemoveXamlCorners();
    }

    if (g_settings.colorTaskbar) {
        ApplyFills(CustomColorArgb(), now);
    } else if (g_fillOwned) {
        RestoreFills();
        g_lastFillArgb = 0;
    }

    UpdateVisibility();

    // Tick fast while a frame waits for its taskbar (it must appear within a
    // frame of the taskbar being uncloaked), at the regular rate otherwise.
    bool anyWaiting = false;
    for (const Overlay& overlay : g_overlays) {
        anyWaiting = anyWaiting || overlay.waiting;
    }
    if (anyWaiting != g_fastTick) {
        if (HWND ctrl = g_ctrlWnd.load()) {
            SetTimer(ctrl, kTimerId, anyWaiting ? kWaitTickIntervalMs : kTickIntervalMs, nullptr);
        }
        g_fastTick = anyWaiting;
    }

    // A changed color (settings) is painted right away.
    for (Overlay& overlay : g_overlays) {
        if (!overlay.painted || overlay.paintedColor != g_settings.customColor) {
            PaintOverlay(overlay, g_settings.customColor);
        }
    }

    ULONGLONG tEnd = GetTickCount64();
    if (tEnd - tStart > 500) {
        Wh_Log(L"slow tick: %llu ms", tEnd - tStart);
    }
    g_inTick = false;
}

// ---------------------------------------------------------------------------
// Windows and thread
// ---------------------------------------------------------------------------

LRESULT CALLBACK FrameWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_NCHITTEST:
            // Solid: the painted pixels are "client area" - the mouse stops
            // here (arrow cursor, clicks swallowed). Zero-alpha pixels never
            // reach this handler; the system passes them through.
            return HTCLIENT;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_ERASEBKGND:
            return 1;
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MBUTTONDBLCLK:
        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP:
        case WM_XBUTTONDBLCLK:
        case WM_MOUSEWHEEL:
        case WM_MOUSEHWHEEL:
        case WM_CONTEXTMENU:
            return 0;  // swallowed
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

bool IsTaskbarWindow(HWND hwnd) {
    WCHAR cls[32];
    return GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) &&
           (wcscmp(cls, L"Shell_TrayWnd") == 0 || wcscmp(cls, L"Shell_SecondaryTrayWnd") == 0);
}

// Delivered on the mod thread (out-of-context hook). A taskbar or Task View
// window that shows, hides or moves triggers a tick right away, so the change
// is picked up when it happens instead of at the next timer tick.
void CALLBACK TaskbarWinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject,
                                  LONG idChild, DWORD, DWORD) {
    if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF || !hwnd) {
        return;
    }
    if (event != EVENT_OBJECT_SHOW && event != EVENT_OBJECT_HIDE &&
        event != EVENT_OBJECT_LOCATIONCHANGE) {
        return;
    }
    if (g_taskbarEventPending || !(IsTaskbarWindow(hwnd) || IsTaskViewWindowClass(hwnd))) {
        return;
    }
    if (HWND ctrl = g_ctrlWnd.load()) {
        if (PostMessageW(ctrl, kMsgTaskbarEvent, 0, 0)) {
            g_taskbarEventPending = true;
        }
    }
}

// A foreground change anywhere, or the foreground window moving or resizing
// (the moment it can go full-screen - F11, a game switching modes), triggers
// a tick right away so the frame hides in the same instant.
void CALLBACK ForegroundWinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject,
                                     LONG idChild, DWORD, DWORD) {
    if (g_taskbarEventPending) {
        return;
    }
    if (event == EVENT_OBJECT_LOCATIONCHANGE) {
        if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF || !hwnd ||
            hwnd != GetForegroundWindow()) {
            return;
        }
    }
    if (HWND ctrl = g_ctrlWnd.load()) {
        if (PostMessageW(ctrl, kMsgTaskbarEvent, 0, 0)) {
            g_taskbarEventPending = true;
        }
    }
}

LRESULT CALLBACK CtrlWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_TIMER:
            if (wParam == kTimerId) {
                Tick(false);
            }
            return 0;
        case kMsgTaskbarEvent:
            g_taskbarEventPending = false;
            // An auto-hide slide fires location changes every frame; the
            // regular tick covers anything a throttled event misses.
            if (GetTickCount64() - g_lastTickTime >= 16) {
                Tick(false);
            }
            return 0;
        case kMsgPassResult:
            OnPassResult(wParam, lParam);
            return 0;
        case kMsgTaskbarReady:
            OnTaskbarReady(wParam, lParam);
            return 0;
        case kMsgTaskbarMoving:
            OnTaskbarMoving(lParam);
            Tick(false);
            return 0;
        case kMsgTaskbarRecolored: {
            UINT& last = g_taskbarRecolored[reinterpret_cast<HWND>(lParam)];
            last = std::max(last, static_cast<UINT>(wParam));
            Tick(false);
            return 0;
        }
        case kMsgReload:
            Tick(false);  // the reload flag is picked up inside
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

bool RegisterClasses() {
    WNDCLASSEXW frameClass{};
    frameClass.cbSize = sizeof(frameClass);
    frameClass.lpfnWndProc = FrameWndProc;
    frameClass.hInstance = g_module;
    frameClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    frameClass.lpszClassName = kFrameClassName;
    if (!RegisterClassExW(&frameClass) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        Wh_Log(L"RegisterClassExW (frame) failed: %u", GetLastError());
        return false;
    }

    WNDCLASSEXW ctrlClass{};
    ctrlClass.cbSize = sizeof(ctrlClass);
    ctrlClass.lpfnWndProc = CtrlWndProc;
    ctrlClass.hInstance = g_module;
    ctrlClass.lpszClassName = kCtrlClassName;
    if (!RegisterClassExW(&ctrlClass) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        Wh_Log(L"RegisterClassExW (ctrl) failed: %u", GetLastError());
        UnregisterClassW(kFrameClassName, g_module);
        return false;
    }

    WNDCLASSEXW appBarClass{};
    appBarClass.cbSize = sizeof(appBarClass);
    appBarClass.lpfnWndProc = AppBarWndProc;
    appBarClass.hInstance = g_module;
    appBarClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    appBarClass.lpszClassName = kAppBarClassName;
    if (!RegisterClassExW(&appBarClass) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        Wh_Log(L"RegisterClassExW (appbar) failed: %u", GetLastError());
        UnregisterClassW(kFrameClassName, g_module);
        UnregisterClassW(kCtrlClassName, g_module);
        return false;
    }

    return true;
}

DWORD WINAPI ModThreadProc(LPVOID) {
    // Wait until the taskbar exists and make sure this process owns it.
    for (;;) {
        HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (tray) {
            DWORD pid = 0;
            GetWindowThreadProcessId(tray, &pid);
            if (pid != GetCurrentProcessId()) {
                Wh_Log(L"Taskbar belongs to another process, nothing to do here");
                return 0;
            }
            break;
        }
        if (WaitForSingleObject(g_stopEvent, 1000) == WAIT_OBJECT_0) {
            return 0;
        }
    }

    SetThreadPerMonitorDpiAware();
    g_threadStartTick = GetTickCount64();

    Gdiplus::GdiplusStartupInput gdiplusInput;
    ULONG_PTR gdiplusToken = 0;
    if (Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusInput, nullptr) != Gdiplus::Ok) {
        Wh_Log(L"GdiplusStartup failed");
        return 0;
    }

    if (!RegisterClasses()) {
        Gdiplus::GdiplusShutdown(gdiplusToken);
        return 0;
    }

    HWND ctrl = CreateWindowExW(0, kCtrlClassName, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr,
                                g_module, nullptr);
    if (!ctrl) {
        Wh_Log(L"CreateWindowExW (ctrl) failed: %u", GetLastError());
        UnregisterClassW(kFrameClassName, g_module);
        UnregisterClassW(kCtrlClassName, g_module);
        UnregisterClassW(kAppBarClassName, g_module);
        Gdiplus::GdiplusShutdown(gdiplusToken);
        return 0;
    }
    g_ctrlWnd = ctrl;

    // The appbar thread; its first shell state is waited for briefly so the
    // first layouts know about auto-hide and the primary taskbar's edge.
    g_appBarWake = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_appBarStop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_taskbarRestored = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_shellStateReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (g_appBarWake && g_appBarStop && g_shellStateReady) {
        g_appBarThread = CreateThread(nullptr, 0, AppBarThreadProc, nullptr, 0, nullptr);
        if (g_appBarThread) {
            WaitForSingleObject(g_shellStateReady, 500);
        } else {
            Wh_Log(L"CreateThread (appbar) failed: %u", GetLastError());
        }
    } else {
        Wh_Log(L"CreateEventW (appbar) failed: %u", GetLastError());
    }

    // Clear before loading so a settings change racing with the load is
    // picked up by the next tick instead of lost.
    g_reloadRequested = false;
    LoadSettings();
    Tick(true);
    SetTimer(ctrl, kTimerId, kTickIntervalMs, nullptr);

    // Taskbar show/hide/move events from this process, delivered to this
    // thread's message loop.
    HWINEVENTHOOK taskbarEventHook =
        SetWinEventHook(EVENT_OBJECT_SHOW, EVENT_OBJECT_LOCATIONCHANGE, nullptr,
                        TaskbarWinEventProc, GetCurrentProcessId(), 0, WINEVENT_OUTOFCONTEXT);
    if (!taskbarEventHook) {
        Wh_Log(L"SetWinEventHook failed: %u", GetLastError());
    }
    HWINEVENTHOOK foregroundEventHook =
        SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
                        ForegroundWinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!foregroundEventHook) {
        Wh_Log(L"SetWinEventHook (foreground) failed: %u", GetLastError());
    }
    HWINEVENTHOOK locationEventHook =
        SetWinEventHook(EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE, nullptr,
                        ForegroundWinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!locationEventHook) {
        Wh_Log(L"SetWinEventHook (location) failed: %u", GetLastError());
    }

    for (;;) {
        DWORD wait = MsgWaitForMultipleObjectsEx(1, &g_stopEvent, INFINITE, QS_ALLINPUT,
                                                 MWMO_INPUTAVAILABLE);
        if (wait == WAIT_OBJECT_0 || wait == WAIT_FAILED) {
            break;
        }

        bool quit = false;
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                quit = true;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        if (quit) {
            break;
        }
    }

    g_ctrlWnd = nullptr;
    KillTimer(ctrl, kTimerId);
    if (taskbarEventHook) {
        UnhookWinEvent(taskbarEventHook);
    }
    if (foregroundEventHook) {
        UnhookWinEvent(foregroundEventHook);
    }
    if (locationEventHook) {
        UnhookWinEvent(locationEventHook);
    }

    // No more posted work; the synchronous restores below also drain any
    // hook call still in flight on the taskbar thread.
    RemoveAsyncHooks();
    LiftAllVeils();

    // Everything vanishes together. One trip to the taskbar thread hands the
    // taskbar back in a single step - handlers off (they are the one thing on
    // that thread that calls back into this code, so this must complete),
    // ears removed, background and stroke restored - and reports when that
    // has been rendered; the frames are taken off the screen at that moment
    // (an immediate layered-window update). The slow part (appbars, work
    // area) comes after, when nothing of the mod is visible.
    if (g_taskbarXamlAvailable) {
        if (HWND taskbar = FindCurrentProcessTaskbarWnd()) {
            if (g_taskbarRestored) {
                ResetEvent(g_taskbarRestored);
            }
            ULONGLONG t0 = GetTickCount64();
            if (!RunFromWindowThreadTimeout(taskbar, RestoreTaskbarsOnTaskbarThread, nullptr,
                                            10000)) {
                Wh_Log(L"taskbar thread didn't answer; taskbar may be left as it was");
            } else if (g_taskbarRestored) {
                DWORD rendered = WaitForSingleObject(g_taskbarRestored, 1000);
                Wh_Log(L"taskbar handed back in %llu ms (%s)", GetTickCount64() - t0,
                       rendered == WAIT_OBJECT_0 ? L"rendered" : L"render not seen");
                if (rendered != WAIT_OBJECT_0) {
                    RunFromWindowThreadTimeout(taskbar, UnsubscribeRestoredRenderOnTaskbarThread,
                                               nullptr, 5000);
                }
            }
        } else {
            Wh_Log(L"no taskbar window, nothing to hand back");
        }
    }
    HideOverlays(g_overlays);
    g_fillOwned = false;
    g_xamlCornersOwned = false;
    ResetPassStates();
    // Only now the appbars: the appbar thread removes them and exits. A
    // taskbar that is busy can delay that, so the wait is capped.
    if (g_appBarStop) {
        SetEvent(g_appBarStop);
    }
    if (g_appBarThread) {
        if (WaitForSingleObject(g_appBarThread, 5000) != WAIT_OBJECT_0) {
            Wh_Log(L"appbar thread didn't finish in time");
        }
        CloseHandle(g_appBarThread);
        g_appBarThread = nullptr;
    }
    if (g_appBarWake) {
        CloseHandle(g_appBarWake);
        g_appBarWake = nullptr;
    }
    if (g_appBarStop) {
        CloseHandle(g_appBarStop);
        g_appBarStop = nullptr;
    }
    if (g_taskbarRestored) {
        HANDLE restored = g_taskbarRestored;
        g_taskbarRestored = nullptr;
        CloseHandle(restored);
    }
    if (g_shellStateReady) {
        CloseHandle(g_shellStateReady);
        g_shellStateReady = nullptr;
    }
    {
        std::lock_guard<std::mutex> lock(g_shellMutex);
        g_appBarWishes.clear();
        g_appBarStatus.clear();
        g_shellState = ShellState{};
    }
    DestroyOverlays(g_overlays);
    {
        std::lock_guard<std::mutex> lock(g_revealMutex);
        g_reveals.clear();
    }
    g_seenTaskbars.clear();
    g_veils.clear();
    g_taskbarReady.clear();
    g_taskbarRecolored.clear();
    g_cloakedSince.clear();
    g_layoutSignature.clear();

    DestroyWindow(ctrl);
    UnregisterClassW(kFrameClassName, g_module);
    UnregisterClassW(kCtrlClassName, g_module);
    UnregisterClassW(kAppBarClassName, g_module);
    Gdiplus::GdiplusShutdown(gdiplusToken);
    return 0;
}

// ---------------------------------------------------------------------------
// dwm.exe side: one corner radius for normal, snapped and maximized windows
// (mechanism from the Maximized Window Corners mod)
// ---------------------------------------------------------------------------

namespace dwm {

std::atomic<bool> g_enabled{true};
std::atomic<int> g_radius{16};

// Windows' full rounding radius at 100%; anything smaller is the "small"
// style (menus, tooltips) or no rounding at all, which the mod leaves alone.
constexpr float kFullRoundRadius = 8.0f;

void LoadDwmSettings() {
    g_enabled = Wh_GetIntSetting(L"windowCorners.enabled") != 0;
    int radius = Wh_GetIntSetting(L"windowCorners.radius");
    g_radius = radius < 0 ? 0 : radius;
    Wh_Log(L"[dwm] window corners %s, radius %d (0 = Windows default)",
           g_enabled ? L"on" : L"off", g_radius.load());
}

// private: float __cdecl CTopLevelWindow::GetRadiusFromCornerStyle(void)
// Pure style -> radius mapping with no maximized check. Resolved, not hooked.
using GetRadiusFromCornerStyle_t = float(WINAPI*)(void* self);
GetRadiusFromCornerStyle_t GetRadiusFromCornerStyle;

// public: float __cdecl CTopLevelWindow::GetFloatCornerRadiusForCurrentStyle(void)
// Returns 0 for maximized / full-screen windows even when the style is rounded.
using GetFloatCornerRadiusForCurrentStyle_t = float(WINAPI*)(void* self);
GetFloatCornerRadiusForCurrentStyle_t GetFloatCornerRadiusForCurrentStyle_Original;
float WINAPI GetFloatCornerRadiusForCurrentStyle_Hook(void* self) {
    float r = GetFloatCornerRadiusForCurrentStyle_Original(self);
    if (!g_enabled) {
        return r;
    }

    float base = GetRadiusFromCornerStyle(self);
    if (base < kFullRoundRadius) {
        return r;  // DONOTROUND or the small style: not ours to change
    }

    int radius = g_radius;
    if (r == 0.0f) {
        // Maximized (or full-screen): Windows squares it, round it instead.
        return radius > 0 ? static_cast<float>(radius) : base;
    }
    // Normal / snapped window: same radius.
    return radius > 0 ? static_cast<float>(radius) : r;
}

#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif
constexpr DWORD kDwmwcpDefault = 0;
constexpr DWORD kDwmwcpDoNotRound = 1;
constexpr DWORD kDwmwcpRound = 2;

// DWM rebuilds a window's corner radius / clip only when something it tracks
// changes. A corner-preference change always goes through
// CWindowList::SetCornerStyle -> radius -> border -> clip, so: set the
// preference to a different value and straight back to the window's own
// value (read first; Default assumed if it can't be read).
void RefreshAllWindows(const wchar_t* why) {
    struct Ctx {
        int refreshed = 0;
        int failed = 0;
    } ctx;
    EnumWindows(
        [](HWND hwnd, LPARAM lp) -> BOOL {
            auto* c = reinterpret_cast<Ctx*>(lp);
            if (!IsWindowVisible(hwnd) || IsIconic(hwnd)) {
                return TRUE;
            }

            DWORD own = kDwmwcpDefault;
            if (FAILED(DwmGetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &own,
                                             sizeof(own)))) {
                own = kDwmwcpDefault;
            }
            DWORD other = (own == kDwmwcpDoNotRound) ? kDwmwcpRound : kDwmwcpDoNotRound;
            HRESULT hr1 = DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &other,
                                                sizeof(other));
            HRESULT hr2 =
                DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &own, sizeof(own));
            if (SUCCEEDED(hr1) && SUCCEEDED(hr2)) {
                c->refreshed++;
            } else {
                c->failed++;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&ctx));
    Wh_Log(L"[dwm] refresh (%s): %d window(s) refreshed, %d failed", why, ctx.refreshed,
           ctx.failed);
}

BOOL Init() {
    LoadDwmSettings();

    HMODULE udwm = GetModuleHandleW(L"udwm.dll");
    if (!udwm) {
        Wh_Log(L"[dwm] udwm.dll not loaded");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK udwmDllHooks[] = {
        {
            {LR"(private: float __cdecl CTopLevelWindow::GetRadiusFromCornerStyle(void))"},
            &GetRadiusFromCornerStyle,  // address only, no hook
        },
        {
            {LR"(public: float __cdecl CTopLevelWindow::GetFloatCornerRadiusForCurrentStyle(void))"},
            &GetFloatCornerRadiusForCurrentStyle_Original,
            GetFloatCornerRadiusForCurrentStyle_Hook,
        },
    };

    if (!WindhawkUtils::HookSymbols(udwm, udwmDllHooks, ARRAYSIZE(udwmDllHooks))) {
        Wh_Log(L"[dwm] HookSymbols failed (udwm.dll)");
        return FALSE;
    }

    Wh_Log(L"[dwm] hooks installed");
    return TRUE;
}

void AfterInit() {
    // Hooks are active from here on; rebuild existing windows' clips.
    RefreshAllWindows(L"init");
}

void Uninit() {
    // Hooks are already removed: this rebuild restores the stock corners.
    RefreshAllWindows(L"uninit");
}

void SettingsChanged() {
    LoadDwmSettings();
    RefreshAllWindows(L"settings");
}

}  // namespace dwm

}  // namespace

// ---------------------------------------------------------------------------
// Windhawk entry points
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    g_host = DetectHostProcess();
    if (g_host == HostProcess::Dwm) {
        return dwm::Init();
    }

    // If the taskbar already exists and belongs to another explorer.exe
    // instance, stay out of this process.
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (tray) {
        DWORD pid = 0;
        GetWindowThreadProcessId(tray, &pid);
        if (pid != GetCurrentProcessId()) {
            Wh_Log(L"Not the taskbar process");
            return FALSE;
        }
    }

    g_module = GetCurrentModule();
    g_unloading = false;

    // Everything done inside the taskbar (border line, background color,
    // corner ears, showing the frame together with it) needs these symbols;
    // the frame itself works without them.
    g_taskbarXamlAvailable = HookTaskbarDllSymbols();
    if (!g_taskbarXamlAvailable) {
        Wh_Log(L"taskbar.dll symbols unavailable - the taskbar is left as it is");
    }

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        return FALSE;
    }

    g_thread = CreateThread(nullptr, 0, ModThreadProc, nullptr, 0, nullptr);
    if (!g_thread) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_host == HostProcess::Dwm) {
        dwm::AfterInit();
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"BeforeUninit");
    g_unloading = true;  // no new taskbar-side changes from here on
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    if (g_host == HostProcess::Dwm) {
        dwm::Uninit();
        return;
    }

    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }
    if (g_thread) {
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");

    if (g_host == HostProcess::Dwm) {
        dwm::SettingsChanged();
        return;
    }

    g_reloadRequested = true;
    HWND ctrl = g_ctrlWnd.load();
    if (ctrl) {
        PostMessageW(ctrl, kMsgReload, 0, 0);
    }
}
