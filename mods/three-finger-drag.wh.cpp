// ==WindhawkMod==
// @id              three-finger-drag
// @name            Three Finger Drag
// @description     Move and snap any window by dragging three fingers on a precision touchpad, like on macOS
// @version         1.0.0
// @author          rodrigothomazi
// @github          https://github.com/RodrigoThomazi
// @include         *
// @compilerOptions -lhid -ldwmapi -lwtsapi32 -lgdi32
// ==/WindhawkMod==

// Licensed under the GNU General Public License v3.0.

// ==WindhawkModReadme==
/*
# Three Finger Drag

Drag three fingers on a precision touchpad to move the window under the cursor,
grabbing it from anywhere, not just the title bar. Lift your fingers to drop
it. Inspired by three finger drag on macOS.

![Moving a window with three fingers and snapping it at the side of the screen](https://raw.githubusercontent.com/RodrigoThomazi/three-finger-drag-assets/main/windows_snap.gif)

Windows moves the window itself, so holding **Shift** hands the drag to
PowerToys FancyZones, the way a title bar drag does:

![Holding Shift while dragging, so that FancyZones takes the window](https://raw.githubusercontent.com/RodrigoThomazi/three-finger-drag-assets/main/fancyzones_snap.gif)

## Features

- Moves the windows of all programs, including programs running as
  administrator. The mod runs in a process of its own, and loads a small
  helper into programs running as administrator, which Windows doesn't let
  other programs move.
- Native move: windows are moved by Windows itself, the way a title bar drag
  moves them, so Snap Layouts, Aero Snap and PowerToys FancyZones work as
  usual. Programs the mod doesn't run in are moved by the mod instead.
- Snapping of its own, when native move is off or unavailable: drop a window
  with the cursor at the top edge of the screen to maximize it, at a side to
  fill half of the screen, or at a corner to fill a quarter. A preview shows
  where the window will go.
- A maximized or snapped window goes back to its previous size as you drag it,
  keeping the spot you grabbed under the cursor, the way a title bar drag does.
- Acceleration: faster finger movements take the cursor farther.
- Between monitors, the cursor stops at the shared edge for a moment, so that
  a window can be snapped there too.
- Press **Esc** while dragging to put the window back where it was.
- Optional: resize a window by grabbing it near one of its edges or corners.
- Optional: macOS drag mode, where the fingers hold the left mouse button
  instead, to select text, drag files or move a window by its title bar.
- Lifting and putting your fingers back within a configurable delay keeps
  dragging the same window.

## Windows touchpad gestures

Windows has its own three and four finger swipes (switch apps, show the desktop,
switch desktops). While they're on, Windows responds to the same swipe as the
mod. Programs can't change these settings: only the Settings app applies them
right away.

With **Number of fingers** set to **Automatic** (the default), the mod reads
your touchpad settings and picks the gesture that's free:

- three fingers if the Windows three-finger swipes are off,
- otherwise four fingers if the Windows four-finger swipes are off.

If both are on, the mod uses three fingers and, the first time you drag, shows
a notice with a shortcut to the touchpad settings. The macOS setup works well:
set **Three-finger gestures → Swipes** to **Nothing**, and give the four-finger
swipes the actions you used with three.

Swipes have to be off in all four directions: Windows picks a swipe by the
direction the fingers start moving in, which a drag can start in too. Taps are
free, so the three-finger tap can keep any action.

## Limitations

- Requires a precision touchpad.
- The touchpad is read in a dedicated process of the mod's own. Programs
  running as administrator get a helper, and with native move on the mod also
  loads into the other programs, where it only hooks message retrieval and
  stays idle until a drag.
- What works while a program running as administrator is in the foreground
  depends on Windhawk's own level. Running Windhawk as administrator, as it
  asks to be, leaves nothing out. Otherwise such a program's helper moves the
  cursor and its windows, but the window isn't brought to the front, Esc
  doesn't cancel a direct move, and macOS drag mode can't click. A helper only
  ever moves its program's windows, moves the cursor and presses the button on
  the mod's own overlay, so that it can't be used to control the program.

## Credits

Native move follows the approach [AltDrag](https://windhawk.net/mods/alt-drag)
takes: hooks on message retrieval turn a press into the move loop Windows
itself runs. Both mods are under the GPL v3.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- mode: move
  $name: Mode
  $options:
  - move: Move the window under the cursor, from anywhere in it
  - drag: macOS drag (hold the left mouse button)
- nativeMove: true
  $name: Native move (Snap Layouts, FancyZones)
  $description: >-
    Windows are moved by Windows itself, the way a title bar drag moves them,
    so Snap Layouts, Aero Snap, PowerToys FancyZones and other tools which
    react to window drags work, in place of the mod's own snapping. When off,
    the mod unloads from every program but its own process and those running
    as administrator.
- fingers: auto
  $name: Number of fingers
  $description: >-
    Automatic uses whichever gesture Windows doesn't use for its own swipes,
    as set in the touchpad settings.
  $options:
  - auto: Automatic
  - "3": Three fingers
  - "4": Four fingers
- speed: 150
  $name: Speed (%)
  $description: >-
    At 100, moving across the whole touchpad moves across the whole width of
    the main screen.
- acceleration: medium
  $name: Acceleration
  $description: Faster finger movements take the cursor farther.
  $options:
  - none: None
  - low: Low
  - medium: Medium
  - high: High
- startDistance: 30
  $name: Start distance (px)
  $description: >-
    How far the cursor would move before a drag starts, so that resting or
    tapping your fingers doesn't move anything.
- releaseDelay: 0
  $name: Release delay (ms)
  $description: >-
    How long the window stays grabbed after you lift your fingers. Putting
    them back within this time keeps dragging it. 0 drops it right away.
- window:
  - activate: true
    $name: Bring the window to the front
  - restore: true
    $name: Restore maximized and snapped windows
    $description: >-
      The window goes back to its previous size as you drag it. When off,
      maximized windows aren't moved.
  - resizeFromEdges: false
    $name: Resize from the edges
    $description: >-
      Grabbing a window near one of its edges or corners resizes it instead
      of moving it.
  - withoutFrame: false
    $name: Move windows without a frame
    $description: >-
      Also move windows which have neither a title bar nor a resizable border,
      such as popups and splash screens.
  - fullScreen: false
    $name: Move full screen windows
    $description: Games, videos and presentations.
  $name: Windows
  $description: Move mode only.
- snap:
  - maximize: true
    $name: Top edge maximizes
  - halves: true
    $name: Sides fill half of the screen
  - quarters: true
    $name: Corners fill a quarter of the screen
  - preview: true
    $name: Show a preview
  - edgeDistance: 2
    $name: Edge distance (px)
    $description: How close to the edge the cursor has to be.
  $name: Snapping
  $description: >-
    The mod's own snapping, used when native move is off or unavailable for a
    program. Move mode only, resizable windows only.
- excludedPrograms: [""]
  $name: Excluded programs
  $description: >-
    Executable file names, such as game.exe. Windows of these programs aren't
    moved.
*/
// ==/WindhawkModSettings==

// The touchpad is read as raw HID input, which a window can receive in the
// background with RIDEV_INPUTSINK. Windows turns the touchpad into mouse input
// for one and two fingers only, and with its swipes for a finger count turned
// off it leaves that count alone, so the cursor is moved here, by the motion of
// the contacts' centroid, through SendInput. That part runs in a dedicated
// process of its own, started from windhawk.exe, so that an input thread of the
// highest priority, a low level keyboard hook and a dialog don't live in
// somebody else's program, see the tool mod implementation at the end.
//
// A precision touchpad reports its contacts in frames. In parallel mode all
// the contacts of a frame come in one report, in hybrid mode they're spread
// over several, the first of which carries the contact count while the others
// carry zero. Reports are collected until the frame is complete.
//
// In move mode the window under the cursor is moved with SetWindowPos, which
// works on the windows of any process at the same integrity level or below,
// without the window seeing any mouse input. SWP_ASYNCWINDOWPOS keeps a busy
// program from holding up the touchpad, and a new position isn't sent before
// the last one was applied, so that they don't pile up in a busy program's
// queue. Snapping is done here on the release, the system's own snapping
// belonging to its move loop, which isn't used.
//
// Windows keeps programs from moving the windows of programs at a higher
// integrity level, e.g. running as administrator, and from sending input while
// one is in the foreground. The mod is loaded into every such program as a
// helper: a message-only window on a thread of its own, which takes requests
// from below, let through its message filter, to move a window of its own
// process or the cursor, and to press the button for a native move, only ever
// where it lands on the mod's own overlay. That's all it does, so that it
// can't be used to drive the program, which clicks or keys would allow.
//
// While such a program is in the foreground, Windows doesn't deliver the
// touchpad's raw input to programs at a lower level either. Its helper reads
// the touchpad then and forwards each frame to the main part, which is
// allowed in that direction. Positions are in fractions of the touchpad's
// width, the same in any process. Windhawk asks to run as administrator, and
// then the main part is above the medium level itself: it reads the touchpad
// and sends input whatever is in the foreground, which a marker tells the
// helpers, so that they leave the touchpad alone. The replies of the programs
// below it are let through its window's message filter.
//
// Native move is a title bar drag in every way that counts: the left button
// is really pressed, the cursor moves with it held, and its release ends the
// drag, all as input, and the press is retrieved by the thread the window
// belongs to, as a press on its title bar would be. Windows ties the capture,
// the button state and the release to that thread, so the move loop it runs
// behaves exactly as with a mouse, and it's the loop Snap Layouts and tools
// such as FancyZones watch.
//
// Each thread which retrieves messages gets a WH_GETMESSAGE hook, set up from
// hooks on the win32u functions all message retrieval goes through. The main
// part asks the window's thread to prepare a drag; the thread puts up a small
// overlay window of its own under the cursor and says it's ready; the main
// part presses the button, which lands on the overlay, so the program sees no
// click, nor does content which takes the pointer over a channel of its own,
// e.g. the XAML parts of File Explorer or Task Manager; the thread turns the
// press, as it retrieves it, into the WM_SYSCOMMAND a title bar press
// produces, and says the loop started. The cursor stays put until then, so
// that the press lands on the overlay, and catches up with the fingers after.
// Without an answer, e.g. from a thread which stopped responding, the window
// is moved directly.
//
// The Windows swipe settings are read from the registry, which the Settings app
// writes them to, and watched for changes. There's no API to change them, and
// the registry alone is only read at sign in, so a conflict is pointed out
// rather than fixed.
//
// All coordinates are physical: the worker thread is per monitor DPI aware.

#include <commctrl.h>
#include <dwmapi.h>
#include <hidusage.h>
#include <hidpi.h>
#include <shellapi.h>
#include <wtsapi32.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <map>
#include <mutex>
#include <string>
#include <unordered_set>
#include <vector>

struct {
    std::atomic<bool> moveMode;
    std::atomic<bool> nativeMove;
    // The extra gain at full speed, in percent.
    std::atomic<int> acceleration;
    // 0 for automatic.
    std::atomic<int> fingers;
    std::atomic<int> speed;
    std::atomic<int> startDistance;
    std::atomic<int> releaseDelay;
    std::atomic<bool> activate;
    std::atomic<bool> restore;
    std::atomic<bool> resizeFromEdges;
    std::atomic<bool> withoutFrame;
    std::atomic<bool> fullScreen;
    std::atomic<bool> snapMaximize;
    std::atomic<bool> snapHalves;
    std::atomic<bool> snapQuarters;
    std::atomic<bool> snapPreview;
    std::atomic<int> snapEdgeDistance;
} g_settings;

// Executable file names, compared ignoring case.
std::mutex g_excludedProgramsMutex;
std::vector<std::wstring> g_excludedPrograms;

constexpr int kMaxFingers = 4;

constexpr USAGE kPageGeneric = 0x01;
constexpr USAGE kUsageX = 0x30;
constexpr USAGE kUsageY = 0x31;
constexpr USAGE kPageDigitizer = 0x0D;
constexpr USAGE kUsageTouchPad = 0x05;
constexpr USAGE kUsageTipSwitch = 0x42;
constexpr USAGE kUsageContactId = 0x51;
constexpr USAGE kUsageContactCount = 0x54;

constexpr UINT_PTR kReleaseTimerId = 1;
constexpr UINT_PTR kNativeTimerId = 2;

// How long a native move waits for the loop to start before the window is
// moved directly. It's only a safeguard, e.g. for a thread which stopped
// responding: whether the program has the mod is known up front, and a busy
// program takes a while to get to the request.
constexpr UINT kNativeStartTimeoutMs = 2000;

// How long a thread keeps the overlay of a drag it prepared for a press which
// doesn't come.
constexpr ULONGLONG kPressOverlayTimeoutMs = 3000;

// Finger speeds, in touchpad widths per second, between which acceleration
// goes from none to full.
constexpr double kAccelerationLowSpeed = 0.2;
constexpr double kAccelerationHighSpeed = 2.0;

// How far the cursor is pushed against an edge shared by two monitors before
// it goes over, in pixels.
constexpr int kEdgeResistance = 48;

// How long a window has to apply a position before the next one is sent
// regardless, e.g. when it adjusts the positions it's given.
constexpr ULONGLONG kRequestTimeoutMs = 50;

// How far inside a window's frame a grab resizes it, at 96 DPI.
constexpr int kResizeZone = 16;

// The margin around a snap preview.
constexpr int kPreviewMargin = 8;

// Sized for the cursor alone: a window covering a monitor counts as a full
// screen one, which the taskbar and notifications make way for.
constexpr int kCursorOverlaySize = 32;

// Unassigned, the key AutoHotkey masks menu keys with (A_MenuMaskKey).
constexpr WORD kMenuMaskKey = 0xE8;

// A swipe setting value meaning each direction is set on its own.
constexpr DWORD kCustomSwipes = 0xFFFF;

constexpr WCHAR kTouchpadKeyPath[] =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\PrecisionTouchPad";

constexpr WCHAR kNoticeDismissedValue[] = L"gestureNoticeDismissed";

// Not in every SDK's headers.
constexpr DWORD kDwmWindowCornerPreference = 33;
constexpr DWORD kDwmBorderColor = 34;
constexpr DWORD kDwmCornerRound = 2;

constexpr WCHAR kWindowClassName[] = L"Windhawk_ThreeFingerDrag_" WH_MOD_ID;
constexpr WCHAR kCursorOverlayClassName[] =
    L"Windhawk_ThreeFingerDrag_Cursor_" WH_MOD_ID;
constexpr WCHAR kPreviewClassName[] =
    L"Windhawk_ThreeFingerDrag_Preview_" WH_MOD_ID;
constexpr WCHAR kPressOverlayClassName[] =
    L"Windhawk_ThreeFingerDrag_Press_" WH_MOD_ID;
constexpr WCHAR kHelperClassName[] =
    L"Windhawk_ThreeFingerDrag_Helper_" WH_MOD_ID;
// Up while the main part sends input itself, see MainSendsInput.
constexpr WCHAR kDirectMarkerName[] =
    L"Local\\Windhawk_ThreeFingerDrag_Direct_" WH_MOD_ID;

// Requests to a helper. The window goes in wParam as its 32 significant bits,
// which is all a handle has, so that 32-bit and 64-bit processes agree, and
// points and sizes go in lParam as two signed 16-bit values.
UINT g_helperSizeMessage =
    RegisterWindowMessage(L"Windhawk_ThreeFingerDrag_Size_" WH_MOD_ID);
UINT g_helperMoveMessage =
    RegisterWindowMessage(L"Windhawk_ThreeFingerDrag_Move_" WH_MOD_ID);
UINT g_helperShowMessage =
    RegisterWindowMessage(L"Windhawk_ThreeFingerDrag_Show_" WH_MOD_ID);
UINT g_helperCursorMessage =
    RegisterWindowMessage(L"Windhawk_ThreeFingerDrag_Cursor_" WH_MOD_ID);
// The left button, down with a nonzero wParam, for a native move.
UINT g_helperButtonMessage =
    RegisterWindowMessage(L"Windhawk_ThreeFingerDrag_Button_" WH_MOD_ID);

// Native move, to the window: prepare a drag, with the system command in
// wParam, or call it off. From the window's thread, with the window in wParam:
// ready for the press, and the loop started.
UINT g_nativePrepareMessage =
    RegisterWindowMessage(L"Windhawk_ThreeFingerDrag_NativePrepare_" WH_MOD_ID);
UINT g_nativeCancelMessage =
    RegisterWindowMessage(L"Windhawk_ThreeFingerDrag_NativeCancel_" WH_MOD_ID);
UINT g_nativeReadyMessage =
    RegisterWindowMessage(L"Windhawk_ThreeFingerDrag_NativeReady_" WH_MOD_ID);
UINT g_nativeStartedMessage =
    RegisterWindowMessage(L"Windhawk_ThreeFingerDrag_NativeStarted_" WH_MOD_ID);

// A frame a helper forwards to the main part: the finger count in the top 8
// bits of wParam and the key of their set below, the centroid in lParam.
UINT g_frameMessage =
    RegisterWindowMessage(L"Windhawk_ThreeFingerDrag_Frame_" WH_MOD_ID);

// The fixed point of a forwarded position, a fraction of the touchpad's width.
constexpr double kFrameScale = 16384;

// What a show request asks for.
enum : LPARAM {
    kShowRestore = 1,
    kShowMaximize,
    kShowRaise,
};

enum class Role {
    kNone,
    // The dedicated process, reading the touchpad.
    kMain,
    // A process running at a higher integrity level.
    kHelper,
    // Any other process, which only runs native moves.
    kOther,
};

Role g_role;

HANDLE g_stopEvent;
HANDLE g_thread;

// This process's integrity level. Windows keeps a program from driving one
// at a higher level: moving its windows, or sending input while it's in the
// foreground.
DWORD g_integrity;

// Whether the main part is above the medium level, which is what Windhawk
// asks for: it then moves every window and sends input itself, and needs a
// helper for neither.
bool g_direct;

// Exists for as long as the mod runs native moves in this process, so that
// the main part can tell which programs have it.
HANDLE g_nativeMarker;

// Exists while the main part sends input itself, so that the helpers don't
// read the touchpad on top of it.
HANDLE g_directMarker;

std::wstring NativeMarkerName(DWORD processId) {
    return L"Local\\Windhawk_ThreeFingerDrag_Native_" WH_MOD_ID L"_" +
           std::to_wstring(processId);
}

// A marker of a process at a higher integrity level can't be opened from
// here, but the refusal still tells that it exists.
bool HasNativeMove(HWND hWnd) {
    DWORD processId = 0;
    if (!GetWindowThreadProcessId(hWnd, &processId)) {
        return false;
    }

    HANDLE marker = OpenEvent(SYNCHRONIZE, FALSE,
                              NativeMarkerName(processId).c_str());
    if (marker) {
        CloseHandle(marker);
        return true;
    }

    return GetLastError() == ERROR_ACCESS_DENIED;
}

// The helper's pending size, which the next move of that window applies.
HWND g_helperSizeWnd;
SIZE g_helperSize;

// The notice about the Windows swipes, shown on a thread of its own so that
// the touchpad keeps working meanwhile.
HANDLE g_noticeThread;
DWORD g_noticeThreadId;

using IsWindowArranged_t = BOOL(WINAPI*)(HWND);
IsWindowArranged_t g_isWindowArranged;

// Everything below belongs to the worker thread.

struct Device {
    bool valid;
    std::vector<BYTE> preparsed;
    // The link collections holding a contact's X value, one per finger slot.
    std::vector<USHORT> fingerCollections;
    bool hasContactCount;
    USHORT contactCountCollection;
    double rangeX;
    double rangeY;
    // In the device's physical unit, 0 if it doesn't report one.
    double physX;
    double physY;
};

std::map<HANDLE, Device> g_devices;

struct Contact {
    ULONG id;
    LONG x;
    LONG y;
    bool tip;
};

std::vector<Contact> g_frame;
ULONG g_frameExpected;

// Whether the Windows swipes for three and four fingers are on.
struct {
    bool three = true;
    bool four = true;
} g_windowsSwipes;

HKEY g_touchpadKey;
HANDLE g_touchpadKeyEvent;
bool g_noticeRequested;

// The windows this mod snapped, with the rect it gave them and the size they
// had before, which a drag away from the snapped rect restores.
struct SnapInfo {
    RECT snappedRect;
    SIZE normalSize;
};
std::map<HWND, SnapInfo> g_snappedWindows;

enum class NativePhase {
    kPreparing,
    kPressing,
    kRunning,
};

enum : UINT {
    kEdgeLeft = 1 << 0,
    kEdgeTop = 1 << 1,
    kEdgeRight = 1 << 2,
    kEdgeBottom = 1 << 3,
};

// The edges each WMSZ_* value of a system resize stands for, by that value.
constexpr UINT kEdgesOfSizeEdge[] = {
    0,
    kEdgeLeft,
    kEdgeRight,
    kEdgeTop,
    kEdgeLeft | kEdgeTop,
    kEdgeRight | kEdgeTop,
    kEdgeBottom,
    kEdgeLeft | kEdgeBottom,
    kEdgeRight | kEdgeBottom,
};

struct {
    // The fingers are down and followed, with these ids.
    bool tracking;
    DWORD idsKey;
    double lastX;
    double lastY;
    double startX;
    double startY;
    // The drag is on, and the release delay, if any, is running.
    bool dragging;
    bool releasePending;
    // Drag mode: the left button is held.
    bool buttonDown;
    // Where the cursor is being taken. Tracked here rather than read back,
    // SendInput being processed asynchronously.
    POINT cursor;
    double fracX;
    double fracY;
    // Move mode: the window being moved or resized, the helper of its
    // process, if it has one, and whether Esc put it back.
    HWND target;
    HWND targetHelper;
    bool cancelled;
    // The foreground window the cursor was last moved under, and the helper
    // of its process, which moves the cursor instead, if it has one.
    HWND cursorForeground;
    HWND cursorHelper;
    // The edges a resize takes hold of, 0 for a move.
    UINT resizeEdges;
    // Where the window is held relative to its origin, the size it's given
    // with the next position, if any, and the size it has while moved.
    POINT grab;
    SIZE pendingSize;
    SIZE dragSize;
    // Native move: on, how far along, its system command, whether the left
    // button is held for it, and when it was requested.
    bool native;
    NativePhase nativePhase;
    UINT nativeCommand;
    bool nativeButtonDown;
    ULONGLONG nativeRequestTime;
    // The cursor's push against an edge shared by two monitors.
    int edgePush;
    // The smoothed finger speed, and when the last frame came.
    double velocity;
    LONGLONG lastFrameTime;
    // How the window was when the drag started, which Esc goes back to.
    POINT startCursor;
    RECT startRect;
    bool startMaximized;
    bool startSnapped;
    SnapInfo startSnapInfo;
    // The last position sent, and when.
    RECT requested;
    bool requestedWithSize;
    ULONGLONG requestTime;
    bool escapeSwallowed;
} g_state;

HWND g_hWnd;
HWND g_cursorOverlayWnd;
HCURSOR g_overlayCursor;
HWND g_previewWnd;
HBRUSH g_previewBrush;
HHOOK g_escapeHook;

void SendKey(WORD vk, bool up) {
    INPUT input{};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = vk;
    input.ki.dwFlags = up ? KEYEVENTF_KEYUP : 0;
    if (!SendInput(1, &input, sizeof(input))) {
        Wh_Log(L"SendInput error: %u", GetLastError());
    }
}

// The primary button, which with the buttons swapped is the physical right one,
// the one sent input names.
void SendLeftButton(bool up) {
    bool swapped = GetSystemMetrics(SM_SWAPBUTTON);
    INPUT input{};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags =
        up ? (swapped ? MOUSEEVENTF_RIGHTUP : MOUSEEVENTF_LEFTUP)
           : (swapped ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_LEFTDOWN);
    if (!SendInput(1, &input, sizeof(input))) {
        Wh_Log(L"SendInput error: %u", GetLastError());
    }
}

// Absolute, so that the pointer speed and acceleration settings don't apply.
// The normalized coordinate is rounded up, which maps back to the same pixel.
void SendCursorPos(POINT pt) {
    int left = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int top = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int height = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    if (width <= 0 || height <= 0) {
        return;
    }

    INPUT input{};
    input.type = INPUT_MOUSE;
    input.mi.dx =
        (LONG)(((LONGLONG)(pt.x - left) * 65536 + width - 1) / width);
    input.mi.dy =
        (LONG)(((LONGLONG)(pt.y - top) * 65536 + height - 1) / height);
    input.mi.dwFlags =
        MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK;
    if (!SendInput(1, &input, sizeof(input))) {
        Wh_Log(L"SendInput error: %u", GetLastError());
    }
}

// A point off every monitor is brought onto the nearest one, where the system
// would put the cursor anyway.
POINT ClampToMonitors(POINT pt) {
    if (MonitorFromPoint(pt, MONITOR_DEFAULTTONULL)) {
        return pt;
    }

    MONITORINFO mi{.cbSize = sizeof(mi)};
    if (!GetMonitorInfo(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST), &mi)) {
        return pt;
    }

    const RECT& rc = mi.rcMonitor;
    return POINT{std::clamp<LONG>(pt.x, rc.left, rc.right - 1),
                 std::clamp<LONG>(pt.y, rc.top, rc.bottom - 1)};
}

// The frame as drawn, without the invisible resize borders around it.
RECT VisibleFrame(HWND hWnd) {
    RECT rc{};
    if (FAILED(DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rc,
                                     sizeof(rc)))) {
        GetWindowRect(hWnd, &rc);
    }
    return rc;
}

// Window and cursor operations, which go through a helper for a process at a
// higher integrity level.

HWND HelperOfWindow(HWND hWnd) {
    DWORD processId = 0;
    if (!GetWindowThreadProcessId(hWnd, &processId)) {
        return nullptr;
    }

    HWND hHelperWnd = nullptr;
    while ((hHelperWnd = FindWindowEx(HWND_MESSAGE, hHelperWnd,
                                      kHelperClassName, nullptr))) {
        DWORD helperProcessId = 0;
        GetWindowThreadProcessId(hHelperWnd, &helperProcessId);
        if (helperProcessId == processId) {
            return hHelperWnd;
        }
    }

    return nullptr;
}

LPARAM PackPair(LONG a, LONG b) {
    return MAKELPARAM((WORD)(SHORT)a, (WORD)(SHORT)b);
}

void PostToHelper(UINT message, HWND hWnd, LPARAM lParam) {
    if (!PostMessage(g_state.targetHelper, message,
                     (WPARAM)(DWORD)HandleToLong(hWnd), lParam)) {
        Wh_Log(L"PostMessage error: %u", GetLastError());
    }
}

void PlaceTarget(const RECT& rc, bool withSize) {
    if (g_state.targetHelper) {
        if (withSize) {
            PostToHelper(g_helperSizeMessage, g_state.target,
                         PackPair(rc.right - rc.left, rc.bottom - rc.top));
        }
        PostToHelper(g_helperMoveMessage, g_state.target,
                     PackPair(rc.left, rc.top));
        return;
    }

    UINT flags = SWP_NOZORDER | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS;
    if (!withSize) {
        flags |= SWP_NOSIZE;
    }

    if (!SetWindowPos(g_state.target, nullptr, rc.left, rc.top,
                      rc.right - rc.left, rc.bottom - rc.top, flags)) {
        Wh_Log(L"SetWindowPos error: %u", GetLastError());
    }
}

void ShowTarget(LPARAM show) {
    if (g_state.targetHelper) {
        PostToHelper(g_helperShowMessage, g_state.target, show);
        return;
    }

    switch (show) {
        case kShowRestore:
            ShowWindowAsync(g_state.target, SW_RESTORE);
            break;
        case kShowMaximize:
            ShowWindowAsync(g_state.target, SW_MAXIMIZE);
            break;
        case kShowRaise:
            SetWindowPos(g_state.target, HWND_TOP, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE |
                             SWP_ASYNCWINDOWPOS);
            break;
    }
}

// With a window of a higher integrity level in the foreground, input sent from
// here is dropped, so its process's helper sends it. Above that level there's
// nothing to stand in for.
HWND ForegroundHelper() {
    if (g_direct) {
        return nullptr;
    }

    HWND hForegroundWnd = GetForegroundWindow();
    if (hForegroundWnd != g_state.cursorForeground) {
        g_state.cursorForeground = hForegroundWnd;
        g_state.cursorHelper =
            hForegroundWnd ? HelperOfWindow(hForegroundWnd) : nullptr;
    }

    return g_state.cursorHelper;
}

void MoveCursorTo(POINT pt) {
    HWND hHelperWnd = ForegroundHelper();
    if (hHelperWnd && PostMessage(hHelperWnd, g_helperCursorMessage, 0,
                                  PackPair(pt.x, pt.y))) {
        return;
    }

    SendCursorPos(pt);
}

// The button of a native move, which only ever goes down over the overlay of
// the window's thread.
void SetNativeButton(bool down) {
    HWND hHelperWnd = ForegroundHelper();
    if (hHelperWnd &&
        PostMessage(hHelperWnd, g_helperButtonMessage, down, 0)) {
        return;
    }

    SendLeftButton(!down);
}

// Windows swipe settings.

bool ReadDword(HKEY key, PCWSTR name, DWORD* value) {
    DWORD type;
    DWORD size = sizeof(*value);
    return RegQueryValueEx(key, name, nullptr, &type, (BYTE*)value, &size) ==
               ERROR_SUCCESS &&
           type == REG_DWORD;
}

// A missing value is taken as the default, which is on.
bool AreSwipesOn(HKEY key, PCWSTR fingers) {
    std::wstring prefix = std::wstring(fingers) + L"Finger";

    DWORD value;
    if (!ReadDword(key, (prefix + L"SlideEnabled").c_str(), &value)) {
        return true;
    }

    if (value != kCustomSwipes) {
        return value != 0;
    }

    for (PCWSTR direction : {L"Up", L"Down", L"Left", L"Right"}) {
        if (ReadDword(key, (prefix + direction).c_str(), &value) && value) {
            return true;
        }
    }

    return false;
}

// Reads the settings and asks to be told of the next change.
void ReadWindowsSwipes() {
    if (!g_touchpadKey) {
        g_windowsSwipes = {};
        return;
    }

    g_windowsSwipes.three = AreSwipesOn(g_touchpadKey, L"Three");
    g_windowsSwipes.four = AreSwipesOn(g_touchpadKey, L"Four");
    Wh_Log(L"Windows swipes: three fingers %s, four fingers %s",
           g_windowsSwipes.three ? L"on" : L"off",
           g_windowsSwipes.four ? L"on" : L"off");

    LONG error = RegNotifyChangeKeyValue(g_touchpadKey, FALSE,
                                         REG_NOTIFY_CHANGE_LAST_SET,
                                         g_touchpadKeyEvent, TRUE);
    if (error != ERROR_SUCCESS) {
        Wh_Log(L"RegNotifyChangeKeyValue error: %d", error);
    }
}

void OpenTouchpadSettings() {
    RegOpenKeyEx(HKEY_CURRENT_USER, kTouchpadKeyPath, 0,
                 KEY_QUERY_VALUE | KEY_NOTIFY, &g_touchpadKey);
    g_touchpadKeyEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!g_touchpadKey || !g_touchpadKeyEvent) {
        Wh_Log(L"Can't watch the touchpad settings");
    }

    ReadWindowsSwipes();
}

void CloseTouchpadSettings() {
    if (g_touchpadKey) {
        RegCloseKey(g_touchpadKey);
        g_touchpadKey = nullptr;
    }

    if (g_touchpadKeyEvent) {
        CloseHandle(g_touchpadKeyEvent);
        g_touchpadKeyEvent = nullptr;
    }
}

int ActiveFingers() {
    if (int fingers = g_settings.fingers) {
        return fingers;
    }

    if (!g_windowsSwipes.three) {
        return 3;
    }

    if (!g_windowsSwipes.four) {
        return 4;
    }

    return 3;
}

bool AreWindowsSwipesOn(int fingers) {
    return fingers == 4 ? g_windowsSwipes.four : g_windowsSwipes.three;
}

// The notice about the Windows swipes.

using TaskDialogIndirect_t = HRESULT(WINAPI*)(const TASKDIALOGCONFIG*,
                                              int*,
                                              int*,
                                              BOOL*);

constexpr int kOpenSettingsButtonId = 100;

HRESULT CALLBACK NoticeCallback(HWND hWnd,
                                UINT msg,
                                WPARAM wParam,
                                LPARAM lParam,
                                LONG_PTR refData) {
    if (msg == TDN_CREATED) {
        SetForegroundWindow(hWnd);
    }
    return S_OK;
}

DWORD WINAPI NoticeThread(LPVOID param) {
    int fingers = (int)(INT_PTR)param;
    PCWSTR count = fingers == 4 ? L"four" : L"three";
    PCWSTR otherCount = fingers == 4 ? L"three" : L"four";

    WCHAR instruction[128];
    _snwprintf_s(instruction, ARRAYSIZE(instruction), _TRUNCATE,
                 L"Windows also responds to %s-finger swipes", count);

    WCHAR content[512];
    _snwprintf_s(
        content, ARRAYSIZE(content), _TRUNCATE,
        L"The %s-finger swipes are on in the touchpad settings, so Windows "
        L"switches apps or desktops along with every drag.\n\n"
        L"Set them to Nothing to drag without interruptions. To keep those "
        L"actions, give them to the %s-finger swipes.",
        count, otherCount);

    PCWSTR openSettings =
        L"Open touchpad settings\nThree-finger and four-finger gestures, "
        L"under Gestures & interactions";

    bool openClicked = false;
    bool dismissed = false;

    // Only in version 6 of the common controls, and only where the process
    // has them; a plain message box stands in where it doesn't.
    auto taskDialogIndirect = (TaskDialogIndirect_t)GetProcAddress(
        GetModuleHandle(L"comctl32.dll"), "TaskDialogIndirect");
    if (taskDialogIndirect) {
        TASKDIALOG_BUTTON buttons[] = {
            {kOpenSettingsButtonId, openSettings},
            {IDCANCEL, L"Not now"},
        };

        TASKDIALOGCONFIG config{};
        config.cbSize = sizeof(config);
        config.dwFlags = TDF_USE_COMMAND_LINKS | TDF_ALLOW_DIALOG_CANCELLATION;
        config.pszWindowTitle = L"Three Finger Drag";
        config.pszMainIcon = TD_INFORMATION_ICON;
        config.pszMainInstruction = instruction;
        config.pszContent = content;
        config.cButtons = ARRAYSIZE(buttons);
        config.pButtons = buttons;
        config.nDefaultButton = kOpenSettingsButtonId;
        config.pszVerificationText = L"Don't show this again";
        config.pfCallback = NoticeCallback;

        int button = 0;
        BOOL verified = FALSE;
        if (SUCCEEDED(
                taskDialogIndirect(&config, &button, nullptr, &verified))) {
            openClicked = button == kOpenSettingsButtonId;
            dismissed = verified;
        }
    } else {
        std::wstring text = std::wstring(instruction) + L".\n\n" + content +
                            L"\n\nOpen the touchpad settings now?";
        openClicked =
            MessageBox(nullptr, text.c_str(), L"Three Finger Drag",
                       MB_OKCANCEL | MB_ICONINFORMATION | MB_SETFOREGROUND) ==
            IDOK;
        // With no checkbox to ask, it's shown once.
        dismissed = true;
    }

    if (dismissed) {
        Wh_SetIntValue(kNoticeDismissedValue, 1);
    }

    if (openClicked) {
        ShellExecute(nullptr, L"open", L"ms-settings:devices-touchpad",
                     nullptr, nullptr, SW_SHOWNORMAL);
    }

    return 0;
}

// Once per session at most, on the first drag the Windows swipes get in the
// way of, which is when the notice makes sense.
void ShowNoticeIfNeeded(int fingers) {
    if (g_noticeRequested || g_noticeThread ||
        Wh_GetIntValue(kNoticeDismissedValue, 0)) {
        return;
    }

    g_noticeRequested = true;
    g_noticeThread = CreateThread(nullptr, 0, NoticeThread,
                                  (LPVOID)(INT_PTR)fingers, 0,
                                  &g_noticeThreadId);
}

// Which windows are moved.

bool IsExcludedClass(HWND hWnd) {
    WCHAR className[64];
    if (!GetClassName(hWnd, className, ARRAYSIZE(className))) {
        return true;
    }

    // The taskbar, the desktop, menus, the Start menu and other shell
    // flyouts, and this mod's own windows.
    static const PCWSTR kExcluded[] = {
        L"Shell_TrayWnd",
        L"Shell_SecondaryTrayWnd",
        L"Progman",
        L"WorkerW",
        L"#32768",
        L"Windows.UI.Core.CoreWindow",
        L"XamlExplorerHostIslandWindow",
        L"TopLevelWindowForOverflowXamlIsland",
        L"NotifyIconOverflowWindow",
        kWindowClassName,
        kCursorOverlayClassName,
        kPreviewClassName,
        kPressOverlayClassName,
    };
    for (PCWSTR excluded : kExcluded) {
        if (_wcsicmp(className, excluded) == 0) {
            return true;
        }
    }

    return false;
}

// A window covering its monitor exactly without being maximized is a full
// screen one, e.g. a game or a video.
bool IsFullScreen(HWND hWnd) {
    RECT rc;
    MONITORINFO mi{.cbSize = sizeof(mi)};
    return !IsZoomed(hWnd) && GetWindowRect(hWnd, &rc) &&
           GetMonitorInfo(MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST),
                          &mi) &&
           EqualRect(&rc, &mi.rcMonitor);
}

bool IsExcludedProgram(HWND hWnd) {
    std::lock_guard<std::mutex> guard(g_excludedProgramsMutex);
    if (g_excludedPrograms.empty()) {
        return false;
    }

    DWORD processId = 0;
    GetWindowThreadProcessId(hWnd, &processId);
    HANDLE process =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (!process) {
        return false;
    }

    WCHAR path[MAX_PATH];
    DWORD size = ARRAYSIZE(path);
    bool gotPath = QueryFullProcessImageName(process, 0, path, &size);
    CloseHandle(process);
    if (!gotPath) {
        return false;
    }

    PCWSTR fileName = wcsrchr(path, L'\\');
    fileName = fileName ? fileName + 1 : path;
    for (const std::wstring& excluded : g_excludedPrograms) {
        if (_wcsicmp(fileName, excluded.c_str()) == 0) {
            return true;
        }
    }

    return false;
}

bool CanMoveWindow(HWND hWnd) {
    if (!IsWindowVisible(hWnd) || IsIconic(hWnd) || IsExcludedClass(hWnd)) {
        return false;
    }

    // Moving a maximized window without restoring it would leave it
    // maximized, and wherever it was dropped.
    if (!g_settings.restore && IsZoomed(hWnd)) {
        return false;
    }

    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &cloaked,
                                        sizeof(cloaked))) &&
        cloaked) {
        return false;
    }

    LONG style = GetWindowLong(hWnd, GWL_STYLE);
    if (style & WS_CHILD) {
        return false;
    }

    if (!g_settings.withoutFrame && (style & WS_CAPTION) != WS_CAPTION &&
        !(style & WS_THICKFRAME)) {
        return false;
    }

    if (!g_settings.fullScreen && IsFullScreen(hWnd)) {
        return false;
    }

    return !IsExcludedProgram(hWnd);
}

HWND WindowToMoveAt(POINT pt) {
    HWND hWnd = WindowFromPoint(pt);
    HWND hRootWnd = hWnd ? GetAncestor(hWnd, GA_ROOT) : nullptr;
    if (!hRootWnd || !CanMoveWindow(hRootWnd)) {
        return nullptr;
    }

    return hRootWnd;
}

bool IsResizable(HWND hWnd) {
    return GetWindowLong(hWnd, GWL_STYLE) & WS_THICKFRAME;
}

// The edges a grab at pt resizes, if it's close enough to them.
UINT ResizeEdgesAt(HWND hWnd, POINT pt) {
    if (!IsResizable(hWnd) || IsZoomed(hWnd)) {
        return 0;
    }

    RECT frame = VisibleFrame(hWnd);
    UINT dpi = GetDpiForWindow(hWnd);
    int zone = MulDiv(kResizeZone, dpi ? dpi : USER_DEFAULT_SCREEN_DPI,
                      USER_DEFAULT_SCREEN_DPI);

    // A window too small for a zone on both sides has none.
    UINT edges = 0;
    if (frame.right - frame.left > zone * 3) {
        if (pt.x < frame.left + zone) {
            edges |= kEdgeLeft;
        } else if (pt.x >= frame.right - zone) {
            edges |= kEdgeRight;
        }
    }

    if (frame.bottom - frame.top > zone * 3) {
        if (pt.y < frame.top + zone) {
            edges |= kEdgeTop;
        } else if (pt.y >= frame.bottom - zone) {
            edges |= kEdgeBottom;
        }
    }

    return edges;
}

PCWSTR CursorOfEdges(UINT edges) {
    switch (edges) {
        case kEdgeLeft:
        case kEdgeRight:
            return IDC_SIZEWE;
        case kEdgeTop:
        case kEdgeBottom:
            return IDC_SIZENS;
        case kEdgeLeft | kEdgeTop:
        case kEdgeRight | kEdgeBottom:
            return IDC_SIZENWSE;
        case kEdgeRight | kEdgeTop:
        case kEdgeLeft | kEdgeBottom:
            return IDC_SIZENESW;
    }

    return IDC_SIZEALL;
}

// A foreground change is allowed to the process which received the last
// input, so an unassigned key is tapped first. Short of that, the window is at
// least raised.
void ActivateWindow(HWND hWnd) {
    if (GetForegroundWindow() == hWnd) {
        return;
    }

    SendKey(kMenuMaskKey, false);
    SendKey(kMenuMaskKey, true);
    if (!SetForegroundWindow(hWnd)) {
        Wh_Log(L"SetForegroundWindow error: %u", GetLastError());
        ShowTarget(kShowRaise);
    }
}

// Where the window is held relative to its origin, and the size it has while
// moved. A maximized or snapped window is restored to its normal size, like a
// title bar drag does, keeping the grab proportional to that size. A maximized
// window is restored right away, queued ahead of the moves; a snapped one gets
// its size with the first move.
POINT CalcGrab(HWND hWnd, POINT pt) {
    g_state.pendingSize = {};

    RECT rc;
    if (!GetWindowRect(hWnd, &rc)) {
        return POINT{};
    }

    POINT grab{pt.x - rc.left, pt.y - rc.top};
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;
    g_state.dragSize = {width, height};

    auto snapped = g_snappedWindows.find(hWnd);
    if (snapped != g_snappedWindows.end()) {
        g_state.startSnapped = true;
        g_state.startSnapInfo = snapped->second;
        g_snappedWindows.erase(snapped);
    }

    if (!g_settings.restore) {
        return grab;
    }

    SIZE normalSize{};
    WINDOWPLACEMENT placement{.length = sizeof(placement)};
    bool gotPlacement = GetWindowPlacement(hWnd, &placement);
    const RECT& normal = placement.rcNormalPosition;

    if (gotPlacement && placement.showCmd == SW_SHOWMAXIMIZED) {
        normalSize = {normal.right - normal.left, normal.bottom - normal.top};
        ShowTarget(kShowRestore);
    } else if (g_state.startSnapped &&
               EqualRect(&rc, &g_state.startSnapInfo.snappedRect)) {
        normalSize = g_state.startSnapInfo.normalSize;
        g_state.pendingSize = normalSize;
    } else if (gotPlacement && g_isWindowArranged && g_isWindowArranged(hWnd)) {
        // Snapped by the system, which keeps the size from before in the
        // normal placement.
        normalSize = {normal.right - normal.left, normal.bottom - normal.top};
        g_state.pendingSize = normalSize;
    }

    if (normalSize.cx > 0 && normalSize.cy > 0 && width > 0 && height > 0) {
        grab.x = MulDiv(grab.x, normalSize.cx, width);
        grab.y = MulDiv(grab.y, normalSize.cy, height);
        g_state.dragSize = normalSize;
    }

    return grab;
}

// The edges the resize takes hold of follow the cursor, the others stay put.
// SetWindowPos doesn't ask the window for its minimum size, so the system's
// minimum is kept instead.
RECT ResizedRect() {
    const RECT& start = g_state.startRect;
    LONG dx = g_state.cursor.x - g_state.startCursor.x;
    LONG dy = g_state.cursor.y - g_state.startCursor.y;
    LONG minWidth = GetSystemMetrics(SM_CXMINTRACK);
    LONG minHeight = GetSystemMetrics(SM_CYMINTRACK);
    UINT edges = g_state.resizeEdges;

    RECT rc = start;
    if (edges & kEdgeLeft) {
        rc.left = std::min(start.left + dx, rc.right - minWidth);
    } else if (edges & kEdgeRight) {
        rc.right = std::max(start.right + dx, rc.left + minWidth);
    }

    if (edges & kEdgeTop) {
        rc.top = std::min(start.top + dy, rc.bottom - minHeight);
    } else if (edges & kEdgeBottom) {
        rc.bottom = std::max(start.bottom + dy, rc.top + minHeight);
    }

    return rc;
}

bool IsRequestPending() {
    if (!g_state.requestTime ||
        GetTickCount64() - g_state.requestTime >= kRequestTimeoutMs) {
        return false;
    }

    RECT rc;
    if (!GetWindowRect(g_state.target, &rc)) {
        return false;
    }

    const RECT& requested = g_state.requested;
    if (g_state.requestedWithSize) {
        return !EqualRect(&rc, &requested);
    }

    return rc.left != requested.left || rc.top != requested.top;
}

// Sends the window where the cursor now puts it. Unless forced, nothing is
// sent while the last position is still on its way, the next frame sending
// whatever is current by then.
void UpdateTarget(bool force) {
    if (!g_state.target || g_state.native || g_state.cancelled) {
        return;
    }

    if (!IsWindow(g_state.target)) {
        g_state.target = nullptr;
        return;
    }

    RECT rc;
    bool withSize;
    if (g_state.resizeEdges) {
        rc = ResizedRect();
        withSize = true;
    } else {
        POINT pos{g_state.cursor.x - g_state.grab.x,
                  g_state.cursor.y - g_state.grab.y};
        SIZE size = g_state.pendingSize;
        withSize = size.cx > 0 && size.cy > 0;
        rc = withSize ? RECT{pos.x, pos.y, pos.x + size.cx, pos.y + size.cy}
                      : RECT{pos.x, pos.y, pos.x, pos.y};
    }

    if (g_state.requestTime && withSize == g_state.requestedWithSize &&
        EqualRect(&rc, &g_state.requested)) {
        return;
    }

    if (!force && IsRequestPending()) {
        return;
    }

    PlaceTarget(rc, withSize);

    g_state.requested = rc;
    g_state.requestedWithSize = withSize;
    g_state.requestTime = GetTickCount64();
    if (!g_state.resizeEdges) {
        g_state.pendingSize = {};
    }
}

// Snapping.

enum class Snap {
    kNone,
    kMaximize,
    kZone,
};

// Any edge of the monitor snaps: the cursor stops at an edge shared with
// another monitor for a while before going over, see ResistSharedEdge.
Snap SnapAt(POINT pt, RECT* zone) {
    MONITORINFO mi{.cbSize = sizeof(mi)};
    if (!GetMonitorInfo(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST), &mi)) {
        return Snap::kNone;
    }

    const RECT& m = mi.rcMonitor;
    const RECT& work = mi.rcWork;

    int edge = g_settings.snapEdgeDistance;
    bool left = pt.x <= m.left + edge;
    bool right = pt.x >= m.right - 1 - edge;
    bool top = pt.y <= m.top + edge;
    if (!left && !right && !top) {
        return Snap::kNone;
    }

    bool quarters = g_settings.snapQuarters;
    LONG cornerX = (m.right - m.left) / 8;
    LONG cornerY = (m.bottom - m.top) / 6;

    if (top && !left && !right) {
        if (quarters && pt.x < m.left + cornerX) {
            left = true;
        } else if (quarters && pt.x >= m.right - cornerX) {
            right = true;
        } else if (g_settings.snapMaximize) {
            *zone = work;
            return Snap::kMaximize;
        } else {
            return Snap::kNone;
        }
    }

    bool upper = quarters && (top || pt.y < m.top + cornerY);
    bool lower = quarters && !upper && pt.y >= m.bottom - cornerY;
    if (!upper && !lower && !g_settings.snapHalves) {
        return Snap::kNone;
    }

    LONG midX = (work.left + work.right) / 2;
    LONG midY = (work.top + work.bottom) / 2;
    *zone = work;
    if (left) {
        zone->right = midX;
    } else {
        zone->left = midX;
    }

    if (upper) {
        zone->bottom = midY;
    } else if (lower) {
        zone->top = midY;
    }

    return Snap::kZone;
}

// The snap the drag would end with, if any.
Snap SnapOfDrag(RECT* zone) {
    if (!g_state.target || g_state.native || g_state.cancelled ||
        g_state.resizeEdges || !IsResizable(g_state.target)) {
        return Snap::kNone;
    }

    return SnapAt(g_state.cursor, zone);
}

// The window is given the zone widened by its invisible resize borders, so
// that its visible frame fills the zone.
void PlaceSnapped(HWND hWnd, RECT zone) {
    RECT windowRect;
    if (!GetWindowRect(hWnd, &windowRect)) {
        return;
    }

    RECT rc = zone;
    RECT frame = VisibleFrame(hWnd);
    rc.left -= frame.left - windowRect.left;
    rc.top -= frame.top - windowRect.top;
    rc.right += windowRect.right - frame.right;
    rc.bottom += windowRect.bottom - frame.bottom;

    std::erase_if(g_snappedWindows,
                  [](const auto& entry) { return !IsWindow(entry.first); });
    g_snappedWindows[hWnd] = {rc, g_state.dragSize};

    PlaceTarget(rc, true);
}

void SnapTarget() {
    RECT zone;
    switch (SnapOfDrag(&zone)) {
        case Snap::kNone:
            break;

        case Snap::kMaximize:
            Wh_Log(L"Maximizing %08X", (DWORD)(ULONG_PTR)g_state.target);
            ShowTarget(kShowMaximize);
            break;

        case Snap::kZone:
            Wh_Log(L"Snapping %08X", (DWORD)(ULONG_PTR)g_state.target);
            PlaceSnapped(g_state.target, zone);
            break;
    }
}

// Overlays: the snap preview, and a window following the cursor which shows
// the move or resize cursor over whatever is underneath.

LRESULT CALLBACK OverlayWndProc(HWND hWnd,
                                UINT uMsg,
                                WPARAM wParam,
                                LPARAM lParam) {
    switch (uMsg) {
        case WM_SETCURSOR:
            SetCursor(g_overlayCursor);
            return TRUE;

        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void PlaceCursorOverlay();

void ShowCursorOverlay(PCWSTR cursor) {
    g_overlayCursor = LoadCursor(nullptr, cursor);
    if (g_cursorOverlayWnd) {
        PlaceCursorOverlay();
        return;
    }

    POINT pt = g_state.cursor;
    g_cursorOverlayWnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED,
        kCursorOverlayClassName, nullptr, WS_POPUP,
        pt.x - kCursorOverlaySize / 2, pt.y - kCursorOverlaySize / 2,
        kCursorOverlaySize, kCursorOverlaySize, nullptr, nullptr,
        (HINSTANCE)GetWindowLongPtr(g_hWnd, GWLP_HINSTANCE), nullptr);
    if (!g_cursorOverlayWnd) {
        Wh_Log(L"CreateWindowEx error: %u", GetLastError());
        return;
    }

    // As good as invisible, while an alpha of zero would let the mouse through.
    SetLayeredWindowAttributes(g_cursorOverlayWnd, 0, 1, LWA_ALPHA);
    ShowWindow(g_cursorOverlayWnd, SW_SHOWNOACTIVATE);
}

void PlaceCursorOverlay() {
    if (g_cursorOverlayWnd) {
        SetWindowPos(g_cursorOverlayWnd, nullptr,
                     g_state.cursor.x - kCursorOverlaySize / 2,
                     g_state.cursor.y - kCursorOverlaySize / 2, 0, 0,
                     SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

void HideCursorOverlay() {
    if (g_cursorOverlayWnd) {
        DestroyWindow(g_cursorOverlayWnd);
        g_cursorOverlayWnd = nullptr;
    }
}

void HidePreview() {
    if (g_previewWnd) {
        DestroyWindow(g_previewWnd);
        g_previewWnd = nullptr;
    }
}

// Translucent and neutral, with rounded corners and the accent color on its
// border where the system draws one, like the preview of the system's own
// snapping. It's placed right under the window being dragged, and lets the
// mouse through.
void ShowPreview(RECT zone) {
    InflateRect(&zone, -kPreviewMargin, -kPreviewMargin);

    if (!g_previewWnd) {
        g_previewWnd = CreateWindowEx(
            WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED |
                WS_EX_TRANSPARENT,
            kPreviewClassName, nullptr, WS_POPUP, zone.left, zone.top,
            zone.right - zone.left, zone.bottom - zone.top, nullptr, nullptr,
            (HINSTANCE)GetWindowLongPtr(g_hWnd, GWLP_HINSTANCE), nullptr);
        if (!g_previewWnd) {
            Wh_Log(L"CreateWindowEx error: %u", GetLastError());
            return;
        }

        SetLayeredWindowAttributes(g_previewWnd, 0, 96, LWA_ALPHA);

        DWORD corner = kDwmCornerRound;
        DwmSetWindowAttribute(g_previewWnd, kDwmWindowCornerPreference,
                              &corner, sizeof(corner));

        DWORD colorization;
        BOOL opaque;
        if (SUCCEEDED(DwmGetColorizationColor(&colorization, &opaque))) {
            COLORREF border =
                RGB((colorization >> 16) & 0xFF, (colorization >> 8) & 0xFF,
                    colorization & 0xFF);
            DwmSetWindowAttribute(g_previewWnd, kDwmBorderColor, &border,
                                  sizeof(border));
        }
    }

    SetWindowPos(g_previewWnd, g_state.target, zone.left, zone.top,
                 zone.right - zone.left, zone.bottom - zone.top,
                 SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

void UpdatePreview() {
    RECT zone;
    if (!g_settings.snapPreview || SnapOfDrag(&zone) == Snap::kNone) {
        HidePreview();
        return;
    }

    ShowPreview(zone);
}

// Esc puts the window back. It's taken while a window is being moved, so that
// the program doesn't see it too, e.g. a dialog closing.

void CancelDrag();
void StartDirectDrag();
bool StartNativeDrag();

LRESULT CALLBACK EscapeHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        const KBDLLHOOKSTRUCT* kbd = (const KBDLLHOOKSTRUCT*)lParam;
        if (kbd->vkCode == VK_ESCAPE && !(kbd->flags & LLKHF_INJECTED)) {
            if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
                g_state.escapeSwallowed = true;
                if (g_state.target && !g_state.cancelled) {
                    CancelDrag();
                }
                return 1;
            }

            if (g_state.escapeSwallowed) {
                g_state.escapeSwallowed = false;
                return 1;
            }
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

void InstallEscapeHook() {
    if (g_escapeHook) {
        return;
    }

    g_state.escapeSwallowed = false;
    g_escapeHook = SetWindowsHookEx(
        WH_KEYBOARD_LL, EscapeHookProc,
        (HINSTANCE)GetWindowLongPtr(g_hWnd, GWLP_HINSTANCE), 0);
    if (!g_escapeHook) {
        Wh_Log(L"SetWindowsHookEx error: %u", GetLastError());
    }
}

void RemoveEscapeHook() {
    if (g_escapeHook) {
        UnhookWindowsHookEx(g_escapeHook);
        g_escapeHook = nullptr;
    }
}

// The drag.

void BeginDrag() {
    POINT cursor;
    GetCursorPos(&cursor);

    // Everything but the finger tracking starts afresh.
    auto tracking = g_state.tracking;
    DWORD idsKey = g_state.idsKey;
    double lastX = g_state.lastX;
    double lastY = g_state.lastY;
    g_state = {};
    g_state.tracking = tracking;
    g_state.idsKey = idsKey;
    g_state.lastX = lastX;
    g_state.lastY = lastY;

    g_state.dragging = true;
    g_state.cursor = cursor;

    int fingers = ActiveFingers();
    if (AreWindowsSwipesOn(fingers)) {
        ShowNoticeIfNeeded(fingers);
    }

    if (!g_settings.moveMode) {
        Wh_Log(L"Starting a drag, holding the left button");
        SendLeftButton(false);
        g_state.buttonDown = true;
        return;
    }

    // With no window to move, the fingers still move the cursor.
    HWND hWnd = WindowToMoveAt(cursor);
    if (!hWnd) {
        Wh_Log(L"No window to move under the cursor");
        return;
    }

    WCHAR className[64] = L"";
    GetClassName(hWnd, className, ARRAYSIZE(className));
    DWORD processId = 0;
    GetWindowThreadProcessId(hWnd, &processId);

    g_state.target = hWnd;
    g_state.targetHelper = HelperOfWindow(hWnd);
    g_state.startCursor = cursor;
    GetWindowRect(hWnd, &g_state.startRect);
    g_state.startMaximized = IsZoomed(hWnd);
    g_state.resizeEdges =
        g_settings.resizeFromEdges ? ResizeEdgesAt(hWnd, cursor) : 0;

    Wh_Log(L"%s %08X, class %s, process %u%s",
           g_state.resizeEdges ? L"Resizing" : L"Moving",
           (DWORD)(ULONG_PTR)hWnd, className, processId,
           g_state.targetHelper ? L", through its helper" : L"");

    if (g_settings.activate) {
        ActivateWindow(hWnd);
    }

    // The press can only be sent from here or by the target's own helper: any
    // other helper turns down a press on a window of somebody else's, so the
    // native move would only time out. ForegroundHelper reads the foreground
    // window afresh, the one activation may just have changed.
    HWND hForegroundHelper = ForegroundHelper();
    bool canPress =
        !hForegroundHelper || hForegroundHelper == g_state.targetHelper;

    if (!g_settings.nativeMove || !canPress || !HasNativeMove(hWnd) ||
        !StartNativeDrag()) {
        StartDirectDrag();
    }
}

// Moves or resizes the window from here, as opposed to native move.
void StartDirectDrag() {
    if (!g_state.resizeEdges) {
        g_state.grab = CalcGrab(g_state.target, g_state.startCursor);
    }

    InstallEscapeHook();
    ShowCursorOverlay(CursorOfEdges(g_state.resizeEdges));
}

UINT SizeEdgeOfEdges(UINT edges) {
    for (UINT sizeEdge = WMSZ_LEFT; sizeEdge <= WMSZ_BOTTOMRIGHT; sizeEdge++) {
        if (kEdgesOfSizeEdge[sizeEdge] == edges) {
            return sizeEdge;
        }
    }

    return WMSZ_BOTTOMRIGHT;
}

// Asks the window's thread to prepare, see OnNativeReady. The cursor stays
// where it is until the loop runs, see MoveCursorBy.
bool StartNativeDrag() {
    UINT command = g_state.resizeEdges
                       ? SC_SIZE | SizeEdgeOfEdges(g_state.resizeEdges)
                       : SC_MOVE | HTCAPTION;
    if (!PostMessage(g_state.target, g_nativePrepareMessage, command, 0)) {
        Wh_Log(L"PostMessage error: %u", GetLastError());
        return false;
    }

    g_state.native = true;
    g_state.nativePhase = NativePhase::kPreparing;
    g_state.nativeCommand = command;
    g_state.nativeRequestTime = GetTickCount64();
    SetTimer(g_hWnd, kNativeTimerId, kNativeStartTimeoutMs, nullptr);
    return true;
}

// The overlay is up under the cursor: the press goes in.
void OnNativeReady(HWND hWnd) {
    if (!g_state.dragging || hWnd != g_state.target || !g_state.native ||
        g_state.nativePhase != NativePhase::kPreparing) {
        PostMessage(hWnd, g_nativeCancelMessage, 0, 0);
        return;
    }

    g_state.nativePhase = NativePhase::kPressing;
    Wh_Log(L"Ready after %llu ms, pressing%s",
           GetTickCount64() - g_state.nativeRequestTime,
           ForegroundHelper() ? L" through the helper" : L"");
    SetNativeButton(true);
    g_state.nativeButtonDown = true;
}

// The loop runs: the cursor catches up with the fingers, and the loop with it.
void OnNativeStarted(HWND hWnd) {
    if (!g_state.dragging || hWnd != g_state.target || !g_state.native ||
        g_state.nativePhase != NativePhase::kPressing) {
        return;
    }

    KillTimer(g_hWnd, kNativeTimerId);
    g_state.nativePhase = NativePhase::kRunning;
    Wh_Log(L"Native move of %08X started after %llu ms",
           (DWORD)(ULONG_PTR)hWnd,
           GetTickCount64() - g_state.nativeRequestTime);
    MoveCursorTo(g_state.cursor);
}

// Releases the button, which ends a running loop, and calls off a drag the
// window's thread is still preparing.
void StopNativeDrag() {
    KillTimer(g_hWnd, kNativeTimerId);

    if (g_state.nativeButtonDown) {
        SetNativeButton(false);
        g_state.nativeButtonDown = false;
    }

    if (g_state.nativePhase != NativePhase::kRunning && g_state.target) {
        PostMessage(g_state.target, g_nativeCancelMessage, 0, 0);
    }
}

void OnNativeTimeout() {
    KillTimer(g_hWnd, kNativeTimerId);
    if (!g_state.dragging || !g_state.target || !g_state.native ||
        g_state.nativePhase == NativePhase::kRunning) {
        return;
    }

    Wh_Log(L"No native move for %08X, moving it directly",
           (DWORD)(ULONG_PTR)g_state.target);
    StopNativeDrag();
    g_state.native = false;
    MoveCursorTo(g_state.cursor);
    StartDirectDrag();
}

// Puts the window back where and how it was, and leaves the rest of the drag
// to move the cursor only.
void CancelDrag() {
    Wh_Log(L"Cancelled, putting %08X back", (DWORD)(ULONG_PTR)g_state.target);

    HWND hWnd = g_state.target;
    const RECT& rc = g_state.startRect;
    if (g_state.startMaximized) {
        // Maximized again on the monitor it was on.
        MONITORINFO mi{.cbSize = sizeof(mi)};
        if (GetMonitorInfo(MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST),
                           &mi)) {
            PlaceTarget(RECT{mi.rcWork.left, mi.rcWork.top, mi.rcWork.left,
                             mi.rcWork.top},
                        false);
        }
        ShowTarget(kShowMaximize);
    } else {
        PlaceTarget(rc, true);
    }

    if (g_state.startSnapped) {
        g_snappedWindows[hWnd] = g_state.startSnapInfo;
    }

    g_state.cancelled = true;
    HidePreview();
    HideCursorOverlay();
}

void EndDrag() {
    Wh_Log(L"Ending the drag");

    KillTimer(g_hWnd, kReleaseTimerId);
    RemoveEscapeHook();

    if (g_state.buttonDown) {
        SendLeftButton(true);
        g_state.buttonDown = false;
    }

    // The release ends the loop, as it ends a title bar drag.
    if (g_state.native) {
        StopNativeDrag();
    } else if (g_state.target && !g_state.cancelled &&
               IsWindow(g_state.target)) {
        UpdateTarget(true);
        SnapTarget();
    }

    HidePreview();
    HideCursorOverlay();

    g_state.target = nullptr;
    g_state.dragging = false;
    g_state.releasePending = false;
    g_state.tracking = false;
}

// The cursor may have moved by other means while the fingers were off, e.g.
// with one finger during the release delay. The drag picks up from there, the
// window keeping its place relative to the cursor.
void ResyncDrag() {
    // The cursor is held still on purpose, see MoveCursorBy.
    if (g_state.native && g_state.nativePhase != NativePhase::kRunning) {
        return;
    }

    POINT pt;
    if (!GetCursorPos(&pt) ||
        (pt.x == g_state.cursor.x && pt.y == g_state.cursor.y)) {
        return;
    }

    g_state.fracX = 0;
    g_state.fracY = 0;

    if (g_state.resizeEdges) {
        g_state.startCursor.x += pt.x - g_state.cursor.x;
        g_state.startCursor.y += pt.y - g_state.cursor.y;
    } else if (RECT rc; g_state.target && !g_state.pendingSize.cx &&
                        GetWindowRect(g_state.target, &rc)) {
        g_state.grab = {pt.x - rc.left, pt.y - rc.top};
    }

    g_state.cursor = pt;
    PlaceCursorOverlay();
}

// Fractions of the touchpad's width, which positions are in, to screen pixels.
double PixelScale() {
    return GetSystemMetrics(SM_CXSCREEN) * g_settings.speed / 100.0;
}

// Snapping at an edge shared by two monitors takes the cursor staying there,
// which it does for the push it takes to go over.
bool ResistsSharedEdges() {
    return g_state.target && !g_state.native && !g_state.cancelled &&
           !g_state.resizeEdges &&
           (g_settings.snapMaximize || g_settings.snapHalves ||
            g_settings.snapQuarters) &&
           IsResizable(g_state.target);
}

POINT ResistSharedEdge(POINT next) {
    HMONITOR from = MonitorFromPoint(g_state.cursor, MONITOR_DEFAULTTONEAREST);
    HMONITOR to = MonitorFromPoint(next, MONITOR_DEFAULTTONULL);
    MONITORINFO mi{.cbSize = sizeof(mi)};
    if (!to || to == from || !GetMonitorInfo(from, &mi)) {
        g_state.edgePush = 0;
        return next;
    }

    const RECT& m = mi.rcMonitor;
    POINT held{std::clamp<LONG>(next.x, m.left, m.right - 1),
               std::clamp<LONG>(next.y, m.top, m.bottom - 1)};
    g_state.edgePush += abs(next.x - held.x) + abs(next.y - held.y);
    if (g_state.edgePush < kEdgeResistance) {
        return held;
    }

    g_state.edgePush = 0;
    return next;
}

// The gain for the finger speed of this frame, smoothed over the last few.
double AccelerationGain(double du, double dv, double dt) {
    double acceleration = g_settings.acceleration / 100.0;
    if (acceleration <= 0) {
        return 1;
    }

    if (dt > 0 && dt < 0.1) {
        double speed = std::hypot(du, dv) / dt;
        g_state.velocity = g_state.velocity * 0.5 + speed * 0.5;
    }

    double t = std::clamp(
        (g_state.velocity - kAccelerationLowSpeed) /
            (kAccelerationHighSpeed - kAccelerationLowSpeed),
        0.0, 1.0);
    return 1 + acceleration * t;
}

void MoveCursorBy(double du, double dv) {
    double scale = PixelScale();

    // The fraction of a pixel left over is kept for the next move, so that
    // slow motion isn't lost.
    g_state.fracX += du * scale;
    g_state.fracY += dv * scale;
    int ix = (int)g_state.fracX;
    int iy = (int)g_state.fracY;
    g_state.fracX -= ix;
    g_state.fracY -= iy;
    if (!ix && !iy) {
        return;
    }

    POINT next{g_state.cursor.x + ix, g_state.cursor.y + iy};

    // The press has to land on the overlay the window's thread put up where
    // the cursor is, so the cursor waits there, and the fingers' motion is
    // kept for when the loop runs.
    if (g_state.native && g_state.nativePhase != NativePhase::kRunning) {
        g_state.cursor = ClampToMonitors(next);
        return;
    }

    if (ResistsSharedEdges()) {
        next = ResistSharedEdge(next);
    }

    g_state.cursor = ClampToMonitors(next);
    PlaceCursorOverlay();
    MoveCursorTo(g_state.cursor);
}

void OnFingers(double u, double v, DWORD idsKey) {
    if (g_state.releasePending) {
        // Back within the release delay, the drag goes on.
        KillTimer(g_hWnd, kReleaseTimerId);
        g_state.releasePending = false;
    }

    LARGE_INTEGER now;
    LARGE_INTEGER frequency;
    QueryPerformanceCounter(&now);
    QueryPerformanceFrequency(&frequency);
    double dt = g_state.lastFrameTime
                    ? (double)(now.QuadPart - g_state.lastFrameTime) /
                          frequency.QuadPart
                    : 0;
    g_state.lastFrameTime = now.QuadPart;

    // A new set of fingers starts over from where it is, with no jump.
    if (!g_state.tracking || idsKey != g_state.idsKey) {
        g_state.velocity = 0;
        g_state.tracking = true;
        g_state.idsKey = idsKey;
        g_state.lastX = u;
        g_state.lastY = v;
        g_state.startX = u;
        g_state.startY = v;
        if (g_state.dragging) {
            ResyncDrag();
        }
        return;
    }

    double du = u - g_state.lastX;
    double dv = v - g_state.lastY;
    g_state.lastX = u;
    g_state.lastY = v;

    if (!g_state.dragging) {
        double travel = std::hypot(u - g_state.startX, v - g_state.startY) *
                        PixelScale();
        if (travel >= g_settings.startDistance) {
            BeginDrag();
        }
        return;
    }

    double gain = AccelerationGain(du, dv, dt);
    MoveCursorBy(du * gain, dv * gain);

    // Every frame, moved or not: a position held back for a busy window goes
    // out once it caught up.
    UpdateTarget(false);
    UpdatePreview();
}

void OnOtherFingerCount(int count, int fingers) {
    g_state.tracking = false;

    // An extra finger, e.g. a resting thumb, doesn't end the drag.
    if (!g_state.dragging || count > fingers || g_state.releasePending) {
        return;
    }

    int delay = g_settings.releaseDelay;
    if (delay <= 0) {
        EndDrag();
        return;
    }

    g_state.releasePending = true;
    SetTimer(g_hWnd, kReleaseTimerId, delay, nullptr);
}

// A frame, read here or forwarded by a helper: the number of fingers down, a
// key telling their set apart, and their centroid.
void OnFrame(int count, DWORD idsKey, double u, double v) {
    int fingers = ActiveFingers();
    if (count != fingers) {
        OnOtherFingerCount(count, fingers);
        return;
    }

    OnFingers(u, v, idsKey);
}

// A helper passes on the frames it reads while its process is in the
// foreground, when the main part gets no raw input at all.
void ForwardFrame(int count, DWORD idsKey, double u, double v) {
    DWORD processId = 0;
    HWND hForegroundWnd = GetForegroundWindow();
    if (!hForegroundWnd ||
        !GetWindowThreadProcessId(hForegroundWnd, &processId) ||
        processId != GetCurrentProcessId()) {
        return;
    }

    static HWND hMainWnd;
    if (!hMainWnd || !IsWindow(hMainWnd)) {
        hMainWnd = FindWindow(kWindowClassName, nullptr);
        if (!hMainWnd) {
            return;
        }
    }

    auto quantize = [](double value) {
        return (WORD)std::clamp<long>(std::lround(value * kFrameScale), 0,
                                      0xFFFF);
    };
    WPARAM wParam = ((DWORD)std::min(count, 0xFF) << 24) | idsKey;
    PostMessage(hMainWnd, g_frameMessage, wParam,
                MAKELPARAM(quantize(u), quantize(v)));
}

// Positions are in fractions of the touchpad's width on both axes, so that
// they mean the same whichever process read them. Units are taken as square
// when the device doesn't report its physical dimensions. The ids of up to
// four fingers make up the key, 6 bits each.
void ProcessFrame(const Device& dev) {
    int count = 0;
    ULONG ids[kMaxFingers]{};
    double x = 0;
    double y = 0;
    for (const Contact& contact : g_frame) {
        if (!contact.tip) {
            continue;
        }

        if (count < kMaxFingers) {
            ids[count] = contact.id;
        }
        x += contact.x;
        y += contact.y;
        count++;
    }

    double u = 0;
    double v = 0;
    if (count) {
        bool physical = dev.physX > 0 && dev.physY > 0;
        double unitX = physical ? dev.physX / dev.rangeX : 1.0;
        double unitY = physical ? dev.physY / dev.rangeY : 1.0;
        double width = physical ? dev.physX : dev.rangeX;
        u = x / count * unitX / width;
        v = y / count * unitY / width;
    }

    int keyed = std::min(count, kMaxFingers);
    std::sort(ids, ids + keyed);
    DWORD idsKey = 0;
    for (int i = 0; i < keyed; i++) {
        idsKey = (idsKey << 6) | (ids[i] & 0x3F);
    }

    if (g_role == Role::kMain) {
        OnFrame(count, idsKey, u, v);
    } else {
        ForwardFrame(count, idsKey, u, v);
    }
}

// The touchpad.

double LogicalRange(const HIDP_VALUE_CAPS& caps) {
    LONGLONG min = caps.LogicalMin;
    LONGLONG max = caps.LogicalMax;
    // Some devices declare a maximum which only fits unsigned.
    if (max <= min && caps.BitSize > 0 && caps.BitSize <= 32) {
        min = 0;
        max = (1LL << caps.BitSize) - 1;
    }
    return (double)(max - min);
}

double PhysicalRange(const HIDP_VALUE_CAPS& caps) {
    LONGLONG range = (LONGLONG)caps.PhysicalMax - caps.PhysicalMin;
    return range > 0 ? (double)range : 0;
}

const Device* GetDevice(HANDLE hDevice) {
    auto it = g_devices.find(hDevice);
    if (it != g_devices.end()) {
        return it->second.valid ? &it->second : nullptr;
    }

    Device& dev = g_devices[hDevice];

    UINT size = 0;
    if (GetRawInputDeviceInfo(hDevice, RIDI_PREPARSEDDATA, nullptr, &size) !=
            0 ||
        !size) {
        return nullptr;
    }

    dev.preparsed.resize(size);
    if (GetRawInputDeviceInfo(hDevice, RIDI_PREPARSEDDATA,
                              dev.preparsed.data(), &size) == (UINT)-1) {
        return nullptr;
    }

    auto preparsed = (PHIDP_PREPARSED_DATA)dev.preparsed.data();
    HIDP_CAPS caps;
    if (HidP_GetCaps(preparsed, &caps) != HIDP_STATUS_SUCCESS) {
        return nullptr;
    }

    USHORT valueCapsCount = caps.NumberInputValueCaps;
    std::vector<HIDP_VALUE_CAPS> valueCaps(valueCapsCount);
    if (!valueCapsCount ||
        HidP_GetValueCaps(HidP_Input, valueCaps.data(), &valueCapsCount,
                          preparsed) != HIDP_STATUS_SUCCESS) {
        return nullptr;
    }

    for (USHORT i = 0; i < valueCapsCount; i++) {
        const HIDP_VALUE_CAPS& vc = valueCaps[i];
        USAGE usage = vc.IsRange ? vc.Range.UsageMin : vc.NotRange.Usage;

        if (vc.UsagePage == kPageDigitizer && usage == kUsageContactCount) {
            dev.hasContactCount = true;
            dev.contactCountCollection = vc.LinkCollection;
        } else if (vc.UsagePage == kPageGeneric && usage == kUsageX) {
            if (std::find(dev.fingerCollections.begin(),
                          dev.fingerCollections.end(),
                          vc.LinkCollection) == dev.fingerCollections.end()) {
                dev.fingerCollections.push_back(vc.LinkCollection);
            }
            if (!dev.rangeX) {
                dev.rangeX = LogicalRange(vc);
                dev.physX = PhysicalRange(vc);
            }
        } else if (vc.UsagePage == kPageGeneric && usage == kUsageY) {
            if (!dev.rangeY) {
                dev.rangeY = LogicalRange(vc);
                dev.physY = PhysicalRange(vc);
            }
        }
    }

    dev.valid =
        !dev.fingerCollections.empty() && dev.rangeX > 0 && dev.rangeY > 0;

    Wh_Log(L"Device %p: valid=%d, slots=%u, contact count=%d, range %.0fx%.0f",
           hDevice, dev.valid, (UINT)dev.fingerCollections.size(),
           dev.hasContactCount, dev.rangeX, dev.rangeY);

    return dev.valid ? &dev : nullptr;
}

void OnReport(const Device& dev, PCHAR report, ULONG length) {
    auto preparsed = (PHIDP_PREPARSED_DATA)dev.preparsed.data();

    if (dev.hasContactCount) {
        ULONG count = 0;
        if (HidP_GetUsageValue(HidP_Input, kPageDigitizer,
                               dev.contactCountCollection, kUsageContactCount,
                               &count, preparsed, report,
                               length) == HIDP_STATUS_SUCCESS &&
            count > 0) {
            g_frame.clear();
            g_frameExpected = count;
        }

        // The rest of a frame whose start was missed.
        if (!g_frameExpected) {
            return;
        }
    }

    for (USHORT collection : dev.fingerCollections) {
        ULONG x;
        ULONG y;
        if (HidP_GetUsageValue(HidP_Input, kPageGeneric, collection, kUsageX,
                               &x, preparsed, report,
                               length) != HIDP_STATUS_SUCCESS ||
            HidP_GetUsageValue(HidP_Input, kPageGeneric, collection, kUsageY,
                               &y, preparsed, report,
                               length) != HIDP_STATUS_SUCCESS) {
            continue;
        }

        ULONG id = collection;
        HidP_GetUsageValue(HidP_Input, kPageDigitizer, collection,
                           kUsageContactId, &id, preparsed, report, length);

        bool tip = false;
        USAGE usages[16];
        ULONG usageCount = ARRAYSIZE(usages);
        if (HidP_GetUsages(HidP_Input, kPageDigitizer, collection, usages,
                           &usageCount, preparsed, report,
                           length) == HIDP_STATUS_SUCCESS) {
            tip = std::find(usages, usages + usageCount, kUsageTipSwitch) !=
                  usages + usageCount;
        }

        g_frame.push_back({id, (LONG)x, (LONG)y, tip});
    }

    if (!dev.hasContactCount || g_frame.size() >= g_frameExpected) {
        ProcessFrame(dev);
        g_frame.clear();
        g_frameExpected = 0;
    }
}

void OnRawInput(HRAWINPUT hRawInput) {
    UINT size = 0;
    if (GetRawInputData(hRawInput, RID_INPUT, nullptr, &size,
                        sizeof(RAWINPUTHEADER)) != 0 ||
        !size) {
        return;
    }

    static std::vector<BYTE> buffer;
    buffer.resize(size);
    if (GetRawInputData(hRawInput, RID_INPUT, buffer.data(), &size,
                        sizeof(RAWINPUTHEADER)) == (UINT)-1) {
        return;
    }

    const RAWINPUT* raw = (const RAWINPUT*)buffer.data();
    if (raw->header.dwType != RIM_TYPEHID) {
        return;
    }

    const Device* dev = GetDevice(raw->header.hDevice);
    if (!dev) {
        return;
    }

    const RAWHID& hid = raw->data.hid;
    for (DWORD i = 0; i < hid.dwCount; i++) {
        OnReport(*dev, (PCHAR)hid.bRawData + i * hid.dwSizeHid, hid.dwSizeHid);
    }
}

// A handle can be reused by another device, and a device that went away
// takes the fingers on it with it.
void OnInputDeviceChange(WPARAM change, HANDLE hDevice) {
    g_devices.erase(hDevice);

    if (change == GIDC_REMOVAL) {
        g_frame.clear();
        g_frameExpected = 0;
        if (g_state.dragging) {
            EndDrag();
        }
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == g_nativeReadyMessage) {
        OnNativeReady((HWND)LongToHandle((LONG)(DWORD)wParam));
        return 0;
    }

    if (uMsg == g_nativeStartedMessage) {
        OnNativeStarted((HWND)LongToHandle((LONG)(DWORD)wParam));
        return 0;
    }

    if (uMsg == g_frameMessage) {
        // Already read here, whoever else did.
        if (g_direct) {
            return 0;
        }

        DWORD frame = (DWORD)wParam;
        OnFrame(frame >> 24, frame & 0xFFFFFF, LOWORD(lParam) / kFrameScale,
                HIWORD(lParam) / kFrameScale);
        return 0;
    }

    switch (uMsg) {
        case WM_INPUT:
            OnRawInput((HRAWINPUT)lParam);
            break;

        case WM_INPUT_DEVICE_CHANGE:
            OnInputDeviceChange(wParam, (HANDLE)lParam);
            return 0;

        case WM_TIMER:
            if (wParam == kReleaseTimerId) {
                KillTimer(hWnd, kReleaseTimerId);
                if (g_state.releasePending) {
                    EndDrag();
                }
                return 0;
            }
            if (wParam == kNativeTimerId) {
                OnNativeTimeout();
                return 0;
            }
            break;

        // The lift of the fingers may never be seen from here on.
        case WM_POWERBROADCAST:
            if (wParam == PBT_APMSUSPEND && g_state.dragging) {
                EndDrag();
            }
            break;

        case WM_WTSSESSION_CHANGE:
            if ((wParam == WTS_SESSION_LOCK ||
                 wParam == WTS_CONSOLE_DISCONNECT ||
                 wParam == WTS_REMOTE_DISCONNECT) &&
                g_state.dragging) {
                EndDrag();
            }
            break;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void RunMessageLoop() {
    while (true) {
        HANDLE handles[] = {g_stopEvent, g_touchpadKeyEvent};
        DWORD count = g_touchpadKey && g_touchpadKeyEvent ? 2 : 1;
        DWORD result = MsgWaitForMultipleObjectsEx(
            count, handles, INFINITE, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
        if (result == WAIT_OBJECT_0 || result == WAIT_FAILED) {
            return;
        }

        if (count == 2 && result == WAIT_OBJECT_0 + 1) {
            ReadWindowsSwipes();
            continue;
        }

        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                return;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

// The classes point into this module, so they're registered under it and
// unregistered before the module can go away.
bool RegisterClasses(HINSTANCE hInstance) {
    g_previewBrush = CreateSolidBrush(RGB(160, 160, 160));

    WNDCLASS classes[] = {
        {.lpfnWndProc = WndProc,
         .hInstance = hInstance,
         .lpszClassName = kWindowClassName},
        {.lpfnWndProc = OverlayWndProc,
         .hInstance = hInstance,
         .lpszClassName = kCursorOverlayClassName},
        {.lpfnWndProc = DefWindowProc,
         .hInstance = hInstance,
         .hbrBackground = g_previewBrush,
         .lpszClassName = kPreviewClassName},
    };
    for (const WNDCLASS& wc : classes) {
        if (!RegisterClass(&wc)) {
            Wh_Log(L"RegisterClass error: %u", GetLastError());
            return false;
        }
    }

    return true;
}

void UnregisterClasses(HINSTANCE hInstance) {
    for (PCWSTR className :
         {kWindowClassName, kCursorOverlayClassName, kPreviewClassName}) {
        UnregisterClass(className, hInstance);
    }

    if (g_previewBrush) {
        DeleteObject(g_previewBrush);
        g_previewBrush = nullptr;
    }
}

void RunTouchpad(HINSTANCE hInstance) {
    // A hidden top level window rather than a message-only one, which raw
    // input in the background isn't reliably delivered to, nor broadcasts.
    g_hWnd = CreateWindowEx(WS_EX_TOOLWINDOW, kWindowClassName, nullptr,
                            WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, hInstance,
                            nullptr);
    if (!g_hWnd) {
        Wh_Log(L"CreateWindowEx error: %u", GetLastError());
        return;
    }

    // The answers of a window's thread come from any level, a sandboxed
    // program's below this one, an ordinary program's below it when Windhawk
    // runs as administrator.
    for (UINT message : {g_nativeReadyMessage, g_nativeStartedMessage}) {
        ChangeWindowMessageFilterEx(g_hWnd, message, MSGFLT_ALLOW, nullptr);
    }

    RAWINPUTDEVICE rid{
        .usUsagePage = kPageDigitizer,
        .usUsage = kUsageTouchPad,
        .dwFlags = RIDEV_INPUTSINK | RIDEV_DEVNOTIFY,
        .hwndTarget = g_hWnd,
    };
    if (RegisterRawInputDevices(&rid, 1, sizeof(rid))) {
        WTSRegisterSessionNotification(g_hWnd, NOTIFY_FOR_THIS_SESSION);
        OpenTouchpadSettings();

        RunMessageLoop();

        CloseTouchpadSettings();
        WTSUnRegisterSessionNotification(g_hWnd);

        rid.dwFlags = RIDEV_REMOVE;
        rid.hwndTarget = nullptr;
        RegisterRawInputDevices(&rid, 1, sizeof(rid));
    } else {
        Wh_Log(L"RegisterRawInputDevices error: %u", GetLastError());
    }

    if (g_state.dragging) {
        EndDrag();
    }

    DestroyWindow(g_hWnd);
    g_hWnd = nullptr;
}

DWORD WINAPI WorkerThread(LPVOID) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    // It turns touches into cursor moves, as the system's input threads do,
    // and waits the rest of the time, so a delay in scheduling it shows as
    // jitter in the drag.
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

    HINSTANCE hInstance = nullptr;
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                          GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      (LPCWSTR)&WndProc, &hInstance);

    if (RegisterClasses(hInstance)) {
        RunTouchpad(hInstance);
    }

    UnregisterClasses(hInstance);
    return 0;
}

// Native move, in the process of the window being moved.

std::atomic<bool> g_uninitializing;

// Hook callbacks under way, which unloading waits for.
std::atomic<int> g_activeCallbacks;

struct CallbackGuard {
    CallbackGuard() { g_activeCallbacks++; }
    ~CallbackGuard() { g_activeCallbacks--; }
};

// This thread's message hook, and all of them, to remove when unloading.
thread_local bool g_messageHookTried;
thread_local HHOOK g_messageHook;
std::mutex g_messageHooksMutex;
std::unordered_set<HHOOK> g_messageHooks;

// The drag this thread prepared, the window and the system command, until the
// press comes.
thread_local HWND g_pendingWnd;
thread_local UINT g_pendingCommand;

// The overlay the press lands on, this thread's, and when it went up. It stays
// until the loop is over and the button is up, so that a release after the
// loop ended on its own, e.g. with Esc, lands on it rather than on the
// program.
thread_local HWND g_pressOverlayWnd;
thread_local ULONGLONG g_pressOverlayTime;

// The loop this thread runs for a drag, its system command, and whether it's
// over, with the button still held.
thread_local HWND g_loopWnd;
thread_local UINT g_loopCommand;
thread_local bool g_loopEnded;

// The overlays of all threads, which unloading closes: a window can only be
// destroyed by its own thread.
std::mutex g_pressOverlaysMutex;
std::unordered_set<HWND> g_pressOverlays;

void DiscardMessage(MSG* msg) {
    msg->message = WM_NULL;
    msg->wParam = 0;
    msg->lParam = 0;
}

// Until the pointer leaves the drag threshold the loop only holds the
// capture; past it, the thread is flagged as moving or sizing.
bool IsLoopActive() {
    GUITHREADINFO info{.cbSize = sizeof(info)};
    if (!g_loopWnd || !GetGUIThreadInfo(GetCurrentThreadId(), &info)) {
        return false;
    }

    return (info.flags & GUI_INMOVESIZE) || info.hwndCapture == g_loopWnd;
}

// Anyone can post a request, so it's held to what a drag of a top level
// window can be: a move by the caption, or a resize from an edge of a window
// which can be resized.
bool IsDragCommand(UINT command, HWND hWnd) {
    if (!hWnd || GetAncestor(hWnd, GA_ROOT) != hWnd) {
        return false;
    }

    UINT detail = command & 0x000F;
    switch (command & 0xFFF0) {
        case SC_MOVE:
            return detail == HTCAPTION;
        case SC_SIZE:
            return detail >= WMSZ_LEFT && detail <= WMSZ_BOTTOMRIGHT &&
                   IsResizable(hWnd);
    }

    return false;
}

PCWSTR CursorOfCommand(UINT command) {
    UINT sizeEdge = command & 0x000F;
    bool sizing = (command & 0xFFF0) == SC_SIZE &&
                  sizeEdge < ARRAYSIZE(kEdgesOfSizeEdge);
    return CursorOfEdges(sizing ? kEdgesOfSizeEdge[sizeEdge] : 0);
}

void PlacePressOverlay(POINT pt) {
    SetWindowPos(g_pressOverlayWnd, nullptr, pt.x - kCursorOverlaySize / 2,
                 pt.y - kCursorOverlaySize / 2, 0, 0,
                 SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

// Nearly invisible, while an alpha of zero would let the mouse through. The
// class has DefWindowProc for its procedure and the program's own instance,
// so nothing of this module is on the window's path, and a window an unload
// leaves behind is harmless.
bool ShowPressOverlay(POINT pt, PCWSTR cursor) {
    if (g_pressOverlayWnd) {
        PlacePressOverlay(pt);
    } else {
        HINSTANCE hInstance = GetModuleHandle(nullptr);
        WNDCLASS wc{
            .lpfnWndProc = DefWindowProc,
            .hInstance = hInstance,
            .lpszClassName = kPressOverlayClassName,
        };
        if (!RegisterClass(&wc) &&
            GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
            Wh_Log(L"RegisterClass error: %u", GetLastError());
            return false;
        }

        g_pressOverlayWnd = CreateWindowEx(
            WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE |
                WS_EX_LAYERED,
            kPressOverlayClassName, nullptr, WS_POPUP,
            pt.x - kCursorOverlaySize / 2, pt.y - kCursorOverlaySize / 2,
            kCursorOverlaySize, kCursorOverlaySize, nullptr, nullptr,
            hInstance, nullptr);
        if (!g_pressOverlayWnd) {
            Wh_Log(L"CreateWindowEx error: %u", GetLastError());
            return false;
        }

        {
            std::lock_guard<std::mutex> lock(g_pressOverlaysMutex);
            g_pressOverlays.insert(g_pressOverlayWnd);
        }

        SetLayeredWindowAttributes(g_pressOverlayWnd, 0, 1, LWA_ALPHA);
        ShowWindow(g_pressOverlayWnd, SW_SHOWNOACTIVATE);
    }

    SetClassLongPtr(g_pressOverlayWnd, GCLP_HCURSOR,
                    (LONG_PTR)LoadCursor(nullptr, cursor));
    g_pressOverlayTime = GetTickCount64();
    return true;
}

void HidePressOverlay() {
    if (!g_pressOverlayWnd) {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(g_pressOverlaysMutex);
        g_pressOverlays.erase(g_pressOverlayWnd);
    }

    DestroyWindow(g_pressOverlayWnd);
    g_pressOverlayWnd = nullptr;
}

void NotifyMain(UINT message, HWND hWnd) {
    if (HWND hMainWnd = FindWindow(kWindowClassName, nullptr)) {
        PostMessage(hMainWnd, message, (WPARAM)(DWORD)HandleToLong(hWnd), 0);
    }
}

void ForgetDrag() {
    g_pendingWnd = nullptr;
    g_pendingCommand = 0;
    g_loopWnd = nullptr;
    g_loopCommand = 0;
    g_loopEnded = false;
    HidePressOverlay();
}

// Puts the overlay up where the cursor is, which the main part holds still
// until the loop runs.
void OnPrepareRequest(MSG* msg) {
    UINT command = (UINT)msg->wParam;
    HWND hWnd = msg->hwnd;
    POINT pt = msg->pt;
    DiscardMessage(msg);

    if (g_loopWnd || !IsDragCommand(command, hWnd)) {
        return;
    }

    if (!ShowPressOverlay(pt, CursorOfCommand(command))) {
        return;
    }

    g_pendingWnd = hWnd;
    g_pendingCommand = command;
    Wh_Log(L"Ready to drag %08X", (DWORD)(ULONG_PTR)hWnd);
    NotifyMain(g_nativeReadyMessage, hWnd);
}

void OnCancelRequest(MSG* msg) {
    DiscardMessage(msg);
    if (!g_loopWnd) {
        ForgetDrag();
    }
}

// The press this thread was prepared for: it's retrieved here, as a press on
// the title bar would be, and turned into what such a press turns into.
void OnOverlayPress(MSG* msg) {
    HWND hWnd = g_pendingWnd;
    UINT command = g_pendingCommand;
    g_pendingWnd = nullptr;
    g_pendingCommand = 0;

    if (!IsWindow(hWnd)) {
        DiscardMessage(msg);
        return;
    }

    g_loopWnd = hWnd;
    g_loopCommand = command;
    g_loopEnded = false;
    Wh_Log(L"Press taken, starting the loop of %08X", (DWORD)(ULONG_PTR)hWnd);
    NotifyMain(g_nativeStartedMessage, hWnd);

    msg->hwnd = hWnd;
    msg->message = WM_SYSCOMMAND;
    msg->wParam = command;
    msg->lParam = MAKELPARAM(msg->pt.x, msg->pt.y);
}

// Sees every message the thread takes from its queue, the loop's own
// included, before the program does.
void OnMessageRetrieved(MSG* msg) {
    if (msg->message == g_nativePrepareMessage) {
        OnPrepareRequest(msg);
        return;
    }

    if (msg->message == g_nativeCancelMessage) {
        OnCancelRequest(msg);
        return;
    }

    if (!g_pressOverlayWnd) {
        return;
    }

    if (msg->hwnd == g_pressOverlayWnd) {
        if (msg->message == WM_LBUTTONDOWN && g_pendingWnd) {
            OnOverlayPress(msg);
        }
        return;
    }

    if (g_loopWnd) {
        if (IsLoopActive()) {
            // The loop takes the moves without dispatching them, which would
            // leave the class cursor showing. The overlay isn't moved along:
            // the loop holds the capture, and any work here holds up the
            // loop.
            if (msg->message == WM_MOUSEMOVE) {
                SetCursor(LoadCursor(nullptr, CursorOfCommand(g_loopCommand)));
            }
        } else if (GetKeyState(VK_LBUTTON) >= 0) {
            ForgetDrag();
        } else if (!g_loopEnded) {
            // Over on its own, e.g. with Esc, with the button still held: the
            // overlay goes under the cursor for the release.
            g_loopEnded = true;
            POINT pt;
            if (GetCursorPos(&pt)) {
                PlacePressOverlay(pt);
            }
        }
        return;
    }

    // Prepared for a press which never came.
    if (GetTickCount64() - g_pressOverlayTime > kPressOverlayTimeoutMs) {
        ForgetDrag();
    }
}

LRESULT CALLBACK MessageHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    CallbackGuard guard;

    if (nCode == HC_ACTION && wParam == PM_REMOVE && lParam &&
        !g_uninitializing) {
        OnMessageRetrieved((MSG*)lParam);
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// Once per thread, the first time it retrieves a message with native move on.
// A thread which got here with it off is left to try again, the setting being
// one a program which is already running picks up.
void EnsureMessageHook() {
    if (g_messageHookTried || !g_settings.nativeMove || g_uninitializing) {
        return;
    }

    g_messageHookTried = true;

    std::lock_guard<std::mutex> lock(g_messageHooksMutex);
    if (g_uninitializing) {
        return;
    }

    g_messageHook = SetWindowsHookEx(WH_GETMESSAGE, MessageHookProc, nullptr,
                                     GetCurrentThreadId());
    if (g_messageHook) {
        g_messageHooks.insert(g_messageHook);
    }
}

// Every message loop ends up in these win32u functions, user32's own modal
// loops included. The replacements only make sure the thread has its hook,
// then jump to the original as a tail call, so that a thread waiting for a
// message has no frame of this module on its stack, which would get in the
// way of unloading it.
using GetMessageFn = BOOL(WINAPI*)(MSG*, HWND, UINT, UINT);
using PeekMessageFn = BOOL(WINAPI*)(MSG*, HWND, UINT, UINT, UINT, UINT);
GetMessageFn g_originalGetMessage;
PeekMessageFn g_originalPeekMessage;

BOOL WINAPI GetMessageStub(MSG* msg, HWND hWnd, UINT first, UINT last) {
    EnsureMessageHook();
    [[clang::musttail]] return g_originalGetMessage(msg, hWnd, first, last);
}

// The last argument is set by user32 from the PeekMessage variant called.
BOOL WINAPI PeekMessageStub(MSG* msg,
                            HWND hWnd,
                            UINT first,
                            UINT last,
                            UINT remove,
                            UINT variant) {
    EnsureMessageHook();
    [[clang::musttail]] return g_originalPeekMessage(msg, hWnd, first, last,
                                                     remove, variant);
}

bool HookMessageLoops() {
    HMODULE win32u = GetModuleHandle(L"win32u.dll");
    void* getMessage =
        win32u ? (void*)GetProcAddress(win32u, "NtUserGetMessage") : nullptr;
    void* peekMessage =
        win32u ? (void*)GetProcAddress(win32u, "NtUserPeekMessage") : nullptr;
    if (!getMessage || !peekMessage) {
        return false;
    }

    Wh_SetFunctionHook(getMessage, (void*)GetMessageStub,
                       (void**)&g_originalGetMessage);
    Wh_SetFunctionHook(peekMessage, (void*)PeekMessageStub,
                       (void**)&g_originalPeekMessage);
    return true;
}

void RemoveMessageHooks() {
    std::lock_guard<std::mutex> lock(g_messageHooksMutex);

    for (HHOOK hook : g_messageHooks) {
        UnhookWindowsHookEx(hook);
    }
    g_messageHooks.clear();
}

// A thread's hook goes away with it.
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
    if (fdwReason == DLL_THREAD_DETACH && g_messageHook) {
        std::lock_guard<std::mutex> lock(g_messageHooksMutex);
        if (g_messageHooks.erase(g_messageHook)) {
            UnhookWindowsHookEx(g_messageHook);
        }
        g_messageHook = nullptr;
    }

    return TRUE;
}

// The helper, in a process running at a higher integrity level.

// This process's integrity level, the medium one if it can't be read, which
// is what a program of the user's runs at.
DWORD ProcessIntegrity() {
    HANDLE token;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        return SECURITY_MANDATORY_MEDIUM_RID;
    }

    DWORD rid = SECURITY_MANDATORY_MEDIUM_RID;
    DWORD size = 0;
    GetTokenInformation(token, TokenIntegrityLevel, nullptr, 0, &size);
    std::vector<BYTE> buffer(size);
    if (size && GetTokenInformation(token, TokenIntegrityLevel, buffer.data(),
                                    size, &size)) {
        PSID sid = ((TOKEN_MANDATORY_LABEL*)buffer.data())->Label.Sid;
        rid = *GetSidSubAuthority(sid, *GetSidSubAuthorityCount(sid) - 1);
    }

    CloseHandle(token);
    return rid;
}

// The main part puts its marker up while it sends input itself, and then the
// helper would only read the touchpad a second time. A marker of a process
// above this level can't be opened, but the refusal still tells that it's
// there.
bool MainSendsInput() {
    HANDLE marker = OpenEvent(SYNCHRONIZE, FALSE, kDirectMarkerName);
    if (marker) {
        CloseHandle(marker);
        return true;
    }

    return GetLastError() == ERROR_ACCESS_DENIED;
}

// A request only ever concerns a top level window of this process, whoever
// sends it.
bool IsOwnRootWindow(HWND hWnd) {
    DWORD processId = 0;
    return hWnd && GetWindowThreadProcessId(hWnd, &processId) &&
           processId == GetCurrentProcessId() &&
           GetAncestor(hWnd, GA_ROOT) == hWnd;
}

// The overlay one of this process's threads put up for a drag.
bool IsPressOverlayUnderCursor() {
    POINT pt;
    DWORD processId = 0;
    HWND hWnd = GetCursorPos(&pt) ? WindowFromPoint(pt) : nullptr;
    if (!hWnd || !GetWindowThreadProcessId(hWnd, &processId) ||
        processId != GetCurrentProcessId()) {
        return false;
    }

    std::lock_guard<std::mutex> lock(g_pressOverlaysMutex);
    return g_pressOverlays.contains(hWnd);
}

LRESULT CALLBACK HelperWndProc(HWND hWnd,
                               UINT uMsg,
                               WPARAM wParam,
                               LPARAM lParam) {
    if (uMsg == WM_INPUT) {
        OnRawInput((HRAWINPUT)lParam);
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    LONG a = (SHORT)LOWORD(lParam);
    LONG b = (SHORT)HIWORD(lParam);

    if (uMsg == g_helperCursorMessage) {
        SendCursorPos(POINT{a, b});
        return 0;
    }

    // A press only where it lands on the overlay of a drag, never on this
    // program, so that it can't be used to click in it. A release is
    // harmless.
    if (uMsg == g_helperButtonMessage) {
        if (!wParam) {
            SendLeftButton(true);
        } else if (IsPressOverlayUnderCursor()) {
            SendLeftButton(false);
        }
        return 0;
    }

    if (uMsg != g_helperSizeMessage && uMsg != g_helperMoveMessage &&
        uMsg != g_helperShowMessage) {
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    HWND hTargetWnd = (HWND)LongToHandle((LONG)(DWORD)wParam);
    if (!IsOwnRootWindow(hTargetWnd)) {
        return 0;
    }

    if (uMsg == g_helperSizeMessage) {
        g_helperSizeWnd = hTargetWnd;
        g_helperSize = {a, b};
    } else if (uMsg == g_helperMoveMessage) {
        UINT flags = SWP_NOZORDER | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS;
        SIZE size{};
        if (g_helperSizeWnd == hTargetWnd) {
            size = g_helperSize;
            g_helperSizeWnd = nullptr;
        } else {
            flags |= SWP_NOSIZE;
        }
        SetWindowPos(hTargetWnd, nullptr, a, b, size.cx, size.cy, flags);
    } else {
        switch (lParam) {
            case kShowRestore:
                ShowWindowAsync(hTargetWnd, SW_RESTORE);
                break;
            case kShowMaximize:
                ShowWindowAsync(hTargetWnd, SW_MAXIMIZE);
                break;
            case kShowRaise:
                SetWindowPos(hTargetWnd, HWND_TOP, 0, 0, 0, 0,
                             SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE |
                                 SWP_ASYNCWINDOWPOS);
                break;
        }
    }

    return 0;
}

bool IsTouchpadRegistered() {
    UINT count = 0;
    if (GetRegisteredRawInputDevices(nullptr, &count,
                                     sizeof(RAWINPUTDEVICE)) != 0 ||
        !count) {
        return false;
    }

    std::vector<RAWINPUTDEVICE> devices(count);
    if (GetRegisteredRawInputDevices(devices.data(), &count,
                                     sizeof(RAWINPUTDEVICE)) == (UINT)-1) {
        return false;
    }

    return std::any_of(devices.begin(), devices.end(), [](const auto& rid) {
        return rid.usUsagePage == kPageDigitizer &&
               rid.usUsage == kUsageTouchPad;
    });
}

HWND g_helperWnd;
bool g_helperTouchpad;

void SetHelperTouchpad(bool on) {
    if (on == g_helperTouchpad) {
        return;
    }

    RAWINPUTDEVICE rid{
        .usUsagePage = kPageDigitizer,
        .usUsage = kUsageTouchPad,
        .dwFlags = on ? (DWORD)RIDEV_INPUTSINK : (DWORD)RIDEV_REMOVE,
        .hwndTarget = on ? g_helperWnd : nullptr,
    };
    if (!RegisterRawInputDevices(&rid, 1, sizeof(rid))) {
        Wh_Log(L"RegisterRawInputDevices error: %u", GetLastError());
        return;
    }

    g_helperTouchpad = on;
    g_frame.clear();
    g_frameExpected = 0;
}

// The helper reads the touchpad only while its process is in the foreground
// and the main part gets nothing itself, the only time it forwards frames, so
// that the programs in the background aren't woken up by every touch. The
// program's own registration for the touchpad, if it has one, is left alone:
// there's one per process.
void UpdateHelperTouchpad() {
    DWORD processId = 0;
    HWND hForegroundWnd = GetForegroundWindow();
    if (hForegroundWnd) {
        GetWindowThreadProcessId(hForegroundWnd, &processId);
    }

    bool forward = processId == GetCurrentProcessId() && !MainSendsInput();
    if (forward && !g_helperTouchpad && IsTouchpadRegistered()) {
        return;
    }

    SetHelperTouchpad(forward);
}

void CALLBACK ForegroundEventProc(HWINEVENTHOOK hWinEventHook,
                                  DWORD event,
                                  HWND hWnd,
                                  LONG idObject,
                                  LONG idChild,
                                  DWORD idEventThread,
                                  DWORD dwmsEventTime) {
    UpdateHelperTouchpad();
}

// Per monitor DPI aware like the main part, so that the coordinates it's sent
// mean the same here.
DWORD WINAPI HelperThread(LPVOID) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    // While its program is in the foreground, this thread is the touchpad's
    // way in and the cursor's way out, the main part getting neither, so a
    // delay in scheduling it shows as jitter in the drag, all the more in a
    // program which keeps its own threads busy, see WorkerThread.
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

    HINSTANCE hInstance = nullptr;
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                          GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      (LPCWSTR)&HelperWndProc, &hInstance);

    WNDCLASS wc{
        .lpfnWndProc = HelperWndProc,
        .hInstance = hInstance,
        .lpszClassName = kHelperClassName,
    };
    if (!RegisterClass(&wc)) {
        Wh_Log(L"RegisterClass error: %u", GetLastError());
        return 0;
    }

    HWND hWnd = CreateWindowEx(0, kHelperClassName, nullptr, 0, 0, 0, 0, 0,
                               HWND_MESSAGE, nullptr, hInstance, nullptr);
    if (hWnd) {
        for (UINT message : {g_helperSizeMessage, g_helperMoveMessage,
                             g_helperShowMessage, g_helperCursorMessage,
                             g_helperButtonMessage}) {
            ChangeWindowMessageFilterEx(hWnd, message, MSGFLT_ALLOW, nullptr);
        }

        g_helperWnd = hWnd;
        HWINEVENTHOOK foregroundHook = SetWinEventHook(
            EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
            ForegroundEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
        UpdateHelperTouchpad();

        RunMessageLoop();

        if (foregroundHook) {
            UnhookWinEvent(foregroundHook);
        }
        SetHelperTouchpad(false);
        g_helperWnd = nullptr;

        DestroyWindow(hWnd);
    } else {
        Wh_Log(L"CreateWindowEx error: %u", GetLastError());
    }

    UnregisterClass(kHelperClassName, hInstance);
    return 0;
}

// The mod's own process, and the others.

bool IsWindhawk() {
    WCHAR path[MAX_PATH];
    DWORD length = GetModuleFileName(nullptr, path, ARRAYSIZE(path));
    if (!length || length == ARRAYSIZE(path)) {
        return false;
    }

    PCWSTR fileName = wcsrchr(path, L'\\');
    fileName = fileName ? fileName + 1 : path;
    return _wcsicmp(fileName, L"windhawk.exe") == 0;
}

void LoadSettings() {
    PCWSTR mode = Wh_GetStringSetting(L"mode");
    g_settings.moveMode = _wcsicmp(mode, L"drag") != 0;
    Wh_FreeStringSetting(mode);

    g_settings.nativeMove = Wh_GetIntSetting(L"nativeMove");

    PCWSTR acceleration = Wh_GetStringSetting(L"acceleration");
    g_settings.acceleration = _wcsicmp(acceleration, L"none") == 0     ? 0
                              : _wcsicmp(acceleration, L"low") == 0    ? 75
                              : _wcsicmp(acceleration, L"high") == 0   ? 250
                                                                       : 150;
    Wh_FreeStringSetting(acceleration);

    PCWSTR fingers = Wh_GetStringSetting(L"fingers");
    g_settings.fingers = wcscmp(fingers, L"3") == 0   ? 3
                         : wcscmp(fingers, L"4") == 0 ? 4
                                                      : 0;
    Wh_FreeStringSetting(fingers);

    g_settings.speed = std::clamp(Wh_GetIntSetting(L"speed"), 10, 1000);
    g_settings.startDistance =
        std::clamp(Wh_GetIntSetting(L"startDistance"), 1, 500);
    g_settings.releaseDelay =
        std::clamp(Wh_GetIntSetting(L"releaseDelay"), 0, 5000);

    g_settings.activate = Wh_GetIntSetting(L"window.activate");
    g_settings.restore = Wh_GetIntSetting(L"window.restore");
    g_settings.resizeFromEdges = Wh_GetIntSetting(L"window.resizeFromEdges");
    g_settings.withoutFrame = Wh_GetIntSetting(L"window.withoutFrame");
    g_settings.fullScreen = Wh_GetIntSetting(L"window.fullScreen");

    g_settings.snapMaximize = Wh_GetIntSetting(L"snap.maximize");
    g_settings.snapHalves = Wh_GetIntSetting(L"snap.halves");
    g_settings.snapQuarters = Wh_GetIntSetting(L"snap.quarters");
    g_settings.snapPreview = Wh_GetIntSetting(L"snap.preview");
    g_settings.snapEdgeDistance =
        std::clamp(Wh_GetIntSetting(L"snap.edgeDistance"), 0, 50);

    std::vector<std::wstring> excludedPrograms;
    for (int i = 0;; i++) {
        PCWSTR program = Wh_GetStringSetting(L"excludedPrograms[%d]", i);
        bool hasValue = *program;
        if (hasValue) {
            excludedPrograms.push_back(program);
        }
        Wh_FreeStringSetting(program);
        if (!hasValue) {
            break;
        }
    }

    std::lock_guard<std::mutex> guard(g_excludedProgramsMutex);
    g_excludedPrograms = std::move(excludedPrograms);
}

// Closes the notice if it's up, its code being in this module.
void CloseNotice() {
    if (!g_noticeThread) {
        return;
    }

    while (WaitForSingleObject(g_noticeThread, 100) == WAIT_TIMEOUT) {
        EnumThreadWindows(
            g_noticeThreadId,
            [](HWND hWnd, LPARAM) WINAPI -> BOOL {
                PostMessage(hWnd, WM_CLOSE, 0, 0);
                return TRUE;
            },
            0);
    }

    CloseHandle(g_noticeThread);
    g_noticeThread = nullptr;
}

// The dedicated process: it reads the touchpad and runs the drags. Above the
// medium level, which is what Windhawk asks to run at, it moves every window
// and sends input itself and needs no helper for either, which its marker
// tells them.
BOOL WhTool_ModInit() {
    Wh_Log(L"> main");

    g_role = Role::kMain;
    LoadSettings();

    // Windows 10 2004 and later.
    g_isWindowArranged = (IsWindowArranged_t)GetProcAddress(
        GetModuleHandle(L"user32.dll"), "IsWindowArranged");

    g_direct = g_integrity > SECURITY_MANDATORY_MEDIUM_RID;
    if (g_direct) {
        g_directMarker = CreateEvent(nullptr, TRUE, FALSE, kDirectMarkerName);
    }

    g_stopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (g_stopEvent) {
        g_thread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
    }

    if (!g_thread) {
        Wh_Log(L"Can't start the touchpad thread: %u", GetLastError());
        if (g_stopEvent) {
            CloseHandle(g_stopEvent);
            g_stopEvent = nullptr;
        }
        if (g_directMarker) {
            CloseHandle(g_directMarker);
            g_directMarker = nullptr;
        }
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}

void WhTool_ModUninit() {
    Wh_Log(L">");

    SetEvent(g_stopEvent);
    WaitForSingleObject(g_thread, INFINITE);
    CloseHandle(g_thread);
    g_thread = nullptr;

    CloseNotice();

    CloseHandle(g_stopEvent);
    g_stopEvent = nullptr;

    if (g_directMarker) {
        CloseHandle(g_directMarker);
        g_directMarker = nullptr;
    }
}

// Every other process: a helper where the level is above the medium one, and
// the hooks which run native moves anywhere the mod is loaded. Without native
// move there's nothing to do in the rest, which the mod unloads from. A
// process it unloaded from has no native move until it's loaded again, and
// its windows are moved directly, see HasNativeMove.
BOOL ModInitOther() {
    g_role = g_integrity > SECURITY_MANDATORY_MEDIUM_RID ? Role::kHelper
                                                         : Role::kOther;

    g_settings.nativeMove = Wh_GetIntSetting(L"nativeMove");
    if (g_role == Role::kOther && !g_settings.nativeMove) {
        return FALSE;
    }

    bool hooked = HookMessageLoops();

    if (g_role == Role::kHelper) {
        Wh_Log(L"> helper");

        // The requests come from the main part, which runs below this level
        // unless Windhawk runs as administrator.
        ChangeWindowMessageFilter(g_nativePrepareMessage, MSGFLT_ADD);
        ChangeWindowMessageFilter(g_nativeCancelMessage, MSGFLT_ADD);

        g_stopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
        if (g_stopEvent) {
            g_thread =
                CreateThread(nullptr, 0, HelperThread, nullptr, 0, nullptr);
        }

        if (!g_thread) {
            Wh_Log(L"Can't start the helper: %u", GetLastError());
            ChangeWindowMessageFilter(g_nativePrepareMessage, MSGFLT_REMOVE);
            ChangeWindowMessageFilter(g_nativeCancelMessage, MSGFLT_REMOVE);
            if (g_stopEvent) {
                CloseHandle(g_stopEvent);
                g_stopEvent = nullptr;
            }
            return FALSE;
        }
    } else if (!hooked) {
        return FALSE;
    }

    // Last, so that it's only there once this process really runs them.
    if (hooked) {
        g_nativeMarker = CreateEvent(
            nullptr, TRUE, FALSE,
            NativeMarkerName(GetCurrentProcessId()).c_str());
    }

    return TRUE;
}

void ModUninitOther() {
    g_uninitializing = true;
    RemoveMessageHooks();

    {
        std::lock_guard<std::mutex> lock(g_pressOverlaysMutex);
        for (HWND hWnd : g_pressOverlays) {
            PostMessage(hWnd, WM_CLOSE, 0, 0);
        }
        g_pressOverlays.clear();
    }

    while (g_activeCallbacks > 0) {
        Sleep(50);
    }

    if (g_nativeMarker) {
        CloseHandle(g_nativeMarker);
        g_nativeMarker = nullptr;
    }

    // The launcher keeps the mod loaded whether its helper came up or not.
    if (g_role != Role::kHelper || !g_thread) {
        return;
    }

    Wh_Log(L">");

    ChangeWindowMessageFilter(g_nativePrepareMessage, MSGFLT_REMOVE);
    ChangeWindowMessageFilter(g_nativeCancelMessage, MSGFLT_REMOVE);

    SetEvent(g_stopEvent);
    WaitForSingleObject(g_thread, INFINITE);
    CloseHandle(g_thread);
    g_thread = nullptr;

    CloseHandle(g_stopEvent);
    g_stopEvent = nullptr;
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
//
// This mod loads into the other processes too, for the helpers and for native
// move, so the block below is kept as it is on the wiki apart from the names of
// its four entry points, and the routing lives in the callbacks after it.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL ToolMod_Init() {
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

void ToolMod_AfterInit() {
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

void ToolMod_SettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void ToolMod_Uninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}

// The mod's callbacks, which route each process to its part.

BOOL Wh_ModInit() {
    g_integrity = ProcessIntegrity();

    // Every process but Windhawk's own: a helper, or the native move hooks.
    if (!IsWindhawk()) {
        DWORD sessionId;
        if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
            sessionId == 0) {
            return FALSE;
        }

        return ModInitOther();
    }

    // The service, the process of a tool mod, and the launcher.
    if (!ToolMod_Init()) {
        return FALSE;
    }

    // Windhawk's own windows are dragged like any other program's.
    if (g_isToolModProcessLauncher) {
        ModInitOther();
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    ToolMod_AfterInit();
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    *bReload = FALSE;

    if (g_role == Role::kMain) {
        ToolMod_SettingsChanged();
        return TRUE;
    }

    bool nativeMove = Wh_GetIntSetting(L"nativeMove");
    g_settings.nativeMove = nativeMove;

    // Without native move there's nothing left for this process to do, and
    // the reload's Wh_ModInit unloads the mod from it. The launcher stays,
    // whatever it's set to.
    *bReload = g_role == Role::kOther && !nativeMove &&
               !g_isToolModProcessLauncher;
    return TRUE;
}

void Wh_ModUninit() {
    if (g_role == Role::kMain) {
        ToolMod_Uninit();
        return;
    }

    ModUninitOther();
}
