// ==WindhawkMod==
// @id              hyprland-windows
// @name            Hyprland Windows
// @description     Hyprland-style window handling: move, resize, maximize and close windows with Win + mouse from anywhere on them, hide title bars, color window borders, and go round the virtual desktops with Win+Tab
// @version         1.2.1
// @author          hiword9
// @github          https://github.com/HiWord9
// @homepage        https://github.com/HiWord9/hyprland-windows
// @include         *
// @compilerOptions -lcomctl32 -ldwmapi -lgdi32 -lole32 -luuid
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Hyprland Windows

Window handling the way Hyprland does it, on Windows.

![Hyprland Windows in action](https://raw.githubusercontent.com/HiWord9/hyprland-windows/extra/demo/1.2.0.gif)

* **Win + left mouse button** - drag a window from anywhere on it, not just by
  its title bar.
* **Win + right mouse button** - resize a window from anywhere on it, from
  whichever corner is closest to the cursor. With a setting turned on,
  **Win + Alt + left mouse button** does the same, which a touchpad can hold
  through a drag.
* **Win + mouse wheel** - make a window larger or smaller, around its center or
  the cursor; two fingers on a touchpad do it too.
* **Win + double click** - maximize a window, or restore it. The title bar is
  gone, so the double click that used to be on it now works anywhere on the
  window; the settings offer other things for it to do.
* **Win + middle click** - close the window under the cursor. The button, the
  modifiers and the action are all settings; it can be a key instead, such as
  Win+Q - even one Windows already uses, like Win+W.
* **Ctrl+Alt+H** - hide the focused window's title bar; press it again to bring
  the title bar back.
* **Win+Tab** - go to the next virtual desktop that has windows on it, at once;
  Win+Shift+Tab goes back. With no other one that has windows, it goes to an
  empty desktop - a new one, if there is no other desktop at all, which stays
  like one made with Win+Ctrl+D. Task View moves to Win+Ctrl+Tab.

Moving and resizing feel like the real thing: windows still snap to the screen
edges, dragging one to the top still offers the Windows 11 snap layouts, and
`Esc` cancels a drag in progress. A window you are dragging fades to slightly
translucent while you hold it, the way a Hyprland window does. Hold `Ctrl` as
well and its edges turn magnetic: bring one close to another window or to the
edge of the screen and it lines up with it. Hold `Shift` while resizing to
keep the window's proportions.

Title bars can also go away on their own: with hiding by default turned on,
every new window opens without one, except the programs on a list of
exceptions (Paint and the Snipping Tool to begin with).

Give windows a border color - your own, or `accent` to follow the Windows
accent color - and it fades from one color to the other as focus moves, the
way a Hyprland border does. That goes for every window with a frame, or only
for the ones whose title bar is hidden, whichever the settings say.

Everything is configurable in the settings - the hotkey, the modifier key, the
translucency, the magnet, the border, and how much of the frame goes away with
the title bar.

## Good to know

* Apps that draw their own title bar instead of using the system one (Chrome,
  Electron apps, VS Code, Office) are not affected by the hotkey.
* A window with a classic menu bar (File, Edit, ...) can't keep that menu where it
  is once the title bar is gone, so by default it is hidden along with it and
  stays reachable from the keyboard: letting go of `Alt`, or `F10`, brings it
  up over the window until you are done with it. The settings offer the other
  choices.
* Win+Tab switches desktops through a part of Windows that Microsoft doesn't
  publish. It is known for Windows 10 and for Windows 11 from 22H2 on; on a
  build the mod doesn't recognize, Win+Tab stays Task View.
* File Explorer scrolls with two fingers on a touchpad its own way, which the
  mod has no say in: there, Win + two fingers scroll as they always do, and
  the mouse wheel is what scales the window.
* The hotkey, and a window shortcut that is a key, are taken everywhere: a
  program that uses the same keys for something of its own doesn't get them
  while the mod is on. Pick other keys in the settings if one does.

## Conflicts

* [AltDrag](https://windhawk.net/mods/alt-drag) moves and resizes windows with
  a key and the mouse as well, and
  [Slick Window Arrangement](https://windhawk.net/mods/slick-window-arrangement)
  snaps windows while they are dragged. Running either of them alongside this
  mod, two mods act on the same drag: use one of them at a time - or, with
  Slick Window Arrangement, turn this mod's magnetic edges off.

## Source and feedback

The source is on [GitHub](https://github.com/HiWord9/hyprland-windows) -
feedback is welcome.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- drag:
  - modifier: win
    $name: Modifier key
    $description: >-
      Hold it and drag with the left mouse button to move a window, or with
      the right one to resize it
    $options:
    - win: Win (Super)
    - alt: Alt
  - keepAspect: shift
    $name: Keep the shape while resizing
    $description: >-
      Hold this key while resizing to keep the window's proportions. The
      magnet steps aside while it is held.
    $options:
    - shift: Shift
    - ctrl: Ctrl
    - alt: Alt
    - none: Off
  - translucency: fade
    $name: Translucency while dragging
    $description: >-
      Fade a window to translucent while it is dragged, the way Hyprland does
    $options:
    - fade: While moving and while resizing
    - move: While moving only
    - opaque: Never
  - opacity: 85
    $name: Opacity while dragging
    $description: In percent - 100 is fully opaque, 10 is barely there
  - fadeIn: 120
    $name: Fade-in time
    $description: How long the window takes to fade, in milliseconds
  - fadeOut: 60
    $name: Fade-out time
    $description: How long it takes to come back once the drag ends, in milliseconds
  - click: auto
    $name: Modifier + click without moving
    $options:
    - auto: Goes to the app with Alt, grabs the window with Win
    - app: Goes to the app
    - window: Grabs the window
  - wheel: true
    $name: Modifier + mouse wheel
    $description: >-
      Turning the wheel over a window with the modifier key held makes it
      larger or smaller
  - wheelStep: 10
    $name: Mouse wheel step
    $description: How much larger or smaller each notch makes the window, in percent
  - wheelAround: center
    $name: What the mouse wheel scales around
    $options:
    - center: >-
        The window's center - and while it is dragged, the point the cursor
        holds it by
    - cursor: The cursor
    - centerAlways: The window's center, even while it is dragged
  - leftResize: false
    $name: Resize with the left button too
    $description: >-
      With the other one of Win and Alt held as well, dragging with the left
      button resizes the window instead of moving it - for a touchpad, where
      the right button can't be held down through a drag. Not while Alt is
      also the key for keeping the shape or for the magnet.
  $name: Moving and resizing
- snap:
  - mode: both
    $name: Magnetic edges
    $description: >-
      A window being moved or resized sticks to the edges of other windows and
      of the screen when it comes close, the way a floating window does in
      Hyprland
    $options:
    - both: To other windows and to the screen
    - windows: To other windows only
    - monitor: To the screen only
    - none: Off
  - modifier: ctrl
    $name: Extra key
    $description: >-
      A key held along with the modifier key, so that edges are only magnetic
      when you want them to be
    $options:
    - ctrl: Ctrl
    - shift: Shift
    - alt: Alt
    - none: No extra key - always magnetic
  - modifierWhen: hold
    $name: What the extra key does
    $options:
    - hold: The edges are magnetic only while it is held
    - suppress: The edges are magnetic unless it is held
  - distance: 12
    $name: Magnet strength
    $description: How close an edge has to come before it sticks, in pixels at 100% scaling
  - windowGap: 0
    $name: Gap between windows
    $description: Room left between windows that stick together, in pixels
  - monitorGap: 0
    $name: Gap at the screen edge
    $description: Room left at the edge of the screen, in pixels
  $name: Magnetic edges
- gestures:
  - doubleClickAction: maximize
    $name: Modifier + double click
    $description: >-
      What a double click with the modifier key held does to the window under
      the cursor - handy once there is no title bar left to double click
    $options:
    - maximize: Maximize the window, or restore it
    - titleBar: Hide the window's title bar, or bring it back
    - close: Close the window
    - none: Nothing
  - doubleClickTime: 0
    $name: Double click speed
    $description: >-
      How long the second click may take, in milliseconds. 0 follows the
      Windows mouse settings.
  - shortcut: Win+MButton
    $name: Window shortcut
    $description: >-
      A mouse button acts on the window under the cursor, a key on the focused
      one - for example Win+MButton, Alt+XButton1 or Win+Q. A key wins over a
      Windows shortcut on the same keys, such as Win+W. "none" turns it off.
  - shortcutAction: close
    $name: What the window shortcut does
    $options:
    - close: Close the window
    - maximize: Maximize the window, or restore it
    - titleBar: Hide the window's title bar, or bring it back
    - none: Nothing
  $name: Gestures
- titleBar:
  - hotkey: Ctrl+Alt+H
    $name: Hotkey
    $description: >-
      Hides or restores the title bar of the focused window, for example
      Ctrl+Alt+H or Win+Shift+T. It wins over a Windows shortcut on the same
      keys. "none" turns it off.
  - hideByDefault: false
    $name: Hide title bars by default
    $description: >-
      Every new window opens without a title bar; the hotkey brings it back
  - exclude:
    - mspaint.exe
    - SnippingTool.exe
    $name: Keep the title bar in these programs
    $description: >-
      By file name. Paint and the Snipping Tool are here because Win + mouse
      can't move them, which leaves nothing to grab them by without a title
      bar.
  - menuBar: hide
    $name: Windows with a menu bar
    $description: What to do with windows that have a classic menu bar (File, Edit, ...)
    $options:
    - hide: Hide the menu bar too (Alt / F10 still reach it)
    - keepMenu: Keep the menu bar, remove only the title bar
    - skip: Don't touch such windows
  - corners: default
    $name: Corners
    $description: Of windows whose title bar is hidden
    $options:
    - default: System default
    - round: Rounded
    - roundsmall: Slightly rounded
    - none: Square
  - topEdgeResize: false
    $name: Resize from the top edge
    $description: >-
      The top few pixels of the window become a resize handle, like the other
      edges - at the cost of whatever the app draws there
  $name: Title bar
- border:
  - active: ""
    $name: Active window
    $description: >-
      "#RRGGBB" for a color, "accent" for the Windows accent color, "none" for
      no border, empty for the system default
  - inactive: ""
    $name: Inactive windows
    $description: The same, for every other window
  - framelessOnly: false
    $name: Only windows with a hidden title bar
    $description: Off, the colors go to every window with a frame
  - fadeDuration: 150
    $name: Fade time
    $description: >-
      How long the color takes to change when a window gains or loses focus,
      in milliseconds. 0 changes it at once.
  $name: Border colors
- desktops:
  - winTab: true
    $name: Win+Tab switches desktops
    $description: >-
      Win+Tab goes to the next virtual desktop that has windows on it, at once,
      and Win+Shift+Tab to the previous one - round and round, the way Alt+Tab
      goes round windows. Task View, which Win+Tab opens in Windows, is on
      Win+Ctrl+Tab then. Off, Win+Tab is left alone.
  - toEmpty: true
    $name: With nowhere to go, an empty desktop
    $description: >-
      While no other desktop has windows, Win+Tab goes to an empty one - a new
      one, if there is no other desktop at all, which stays like one made with
      Win+Ctrl+D. Off, Win+Tab opens Task View then.
  $name: Virtual desktops
*/
// ==/WindhawkModSettings==

// Assembled by tools/bundle.py from the modules in src/ -
// https://github.com/HiWord9/hyprland-windows

////////////////////////////////////////////////////////////////////////////////
// src/common.h
////////////////////////////////////////////////////////////////////////////////
// Everything the modules share: the system headers they all need, the settings
// they all read, and a declaration for every function or global that crosses a
// file boundary.
//
// Each src/*.cpp includes only this header, which keeps every module a valid
// translation unit on its own (so an IDE resolves references and can check a
// single file), while `tools/bundle.py` concatenates them into the one
// mod.wh.cpp file Windhawk wants.

#include <windhawk_utils.h>

#include <dwmapi.h>
#include <objectarray.h>
#include <servprov.h>
#include <shellapi.h>
#include <shobjidl.h>
#include <windowsx.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdlib>
#include <cwctype>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Unloading

// Set before anything is torn down. Everything that installs a hook or a
// subclass checks it (holding the same lock the teardown takes) so that
// nothing of ours is put back in place behind the teardown's back.
extern std::atomic<bool> g_uninitializing;

// How many hook procedures and worker threads of ours are running right now.
// UnhookWindowsHookEx does not wait for a hook procedure that is executing on
// another thread, so Wh_ModUninit waits for this to reach zero before letting
// the image go.
extern std::atomic<int> g_modRefCount;

struct ModRef {
    ModRef() { g_modRefCount++; }
    ~ModRef() { g_modRefCount--; }
    ModRef(const ModRef&) = delete;
    ModRef& operator=(const ModRef&) = delete;
};

// Starts a worker thread that holds a reference, which the thread drops as
// it ends, and keeps its handle for Wh_ModUninit to wait on.
bool StartModThread(LPTHREAD_START_ROUTINE proc,
                    void* param,
                    DWORD* threadId = nullptr);

////////////////////////////////////////////////////////////////////////////////
// Animation

// How far along an animation is, eased in and out (smoothstep) so that a short
// one reads as a movement rather than a jump. Every animation in the mod runs
// on this curve, so they all feel like the same piece of work.
inline double AnimationProgress(int elapsedMs, int durationMs) {
    if (durationMs <= 0 || elapsedMs >= durationMs) {
        return 1.0;
    }
    if (elapsedMs <= 0) {
        return 0.0;
    }
    double t = (double)elapsedMs / durationMs;
    return t * t * (3.0 - 2.0 * t);
}

////////////////////////////////////////////////////////////////////////////////
// Settings

enum class DragModifier { Win, Alt };
enum class MenuBarMode { Hide, KeepMenu, Skip };
// What a mouse gesture with the modifier held does to a window.
enum class WindowAction { None, ToggleMaximize, ToggleTitleBar, Close };
// Which drags fade the window they are dragging.
enum class DragTranslucency { Both, MoveOnly, Off };
// What the mouse wheel grows and shrinks a window around: its center - or
// the point the cursor holds it by while it is moved - the cursor, or its
// center even while it is moved.
enum class WheelAnchor { CenterOrHold, Cursor, Center };
// Where a click with the modifier held goes when the mouse doesn't move.
enum class ModifierClick { Auto, App, Window };
// What a dragged window's edges are magnetic to.
enum class SnapMode { Both, Monitor, Windows, Off };

// Sentinel for "leave the DWM attribute alone" (not a valid DWM color value).
constexpr COLORREF kColorUntouched = 0xFFFFFFFD;
// And for "whatever the Windows accent color is". Kept as a sentinel rather
// than resolved when the settings are read, so that an accent color that
// changes afterwards is followed without reparsing anything.
constexpr COLORREF kColorAccent = 0xFFFFFFFC;

// The translucency of a dragged window, and how long it takes to get there
// and back. Also what a setting Windhawk has never written out means, since
// none of the three is usefully zero.
constexpr int kDefaultDragOpacity = 85;
constexpr int kDefaultDragFadeIn = 120;
constexpr int kDefaultDragFadeOut = 60;
constexpr int kDefaultWheelStep = 10;  // percent per notch

// How long the border color takes to cross from one setting to the other when
// focus moves.
constexpr int kDefaultBorderFade = 150;

// How close an edge has to come before it sticks, in units of a 96 dpi pixel
// so that the pull feels the same whatever a monitor is scaled to.
constexpr int kDefaultSnapDistance = 12;

// A parsed combination of modifiers and one key or mouse button. vk == 0
// means there is no binding. Small and trivially copyable, so the settings
// can hold one whole rather than a field at a time.
struct Hotkey {
    UINT vk = 0;
    bool ctrl = false;
    bool alt = false;
    bool shift = false;
    bool win = false;
};

constexpr PCWSTR kDefaultHotkey = L"Ctrl+Alt+H";
constexpr PCWSTR kDefaultWindowShortcut = L"Win+MButton";

struct Settings {
    std::atomic<Hotkey> hotkey{};
    std::atomic<Hotkey> windowShortcut{};
    std::atomic<WindowAction> windowShortcutAction{WindowAction::Close};
    std::atomic<DragModifier> dragModifier{DragModifier::Win};
    std::atomic<ModifierClick> modifierClick{ModifierClick::Auto};
    // The other one of Win and Alt held as well: the left button resizes.
    std::atomic<bool> leftResize{false};
    std::atomic<bool> topEdgeResize{false};
    std::atomic<WindowAction> doubleClickAction{WindowAction::ToggleMaximize};
    std::atomic<int> doubleClickTime{0};  // 0: the Windows double-click speed
    std::atomic<DragTranslucency> dragTranslucency{DragTranslucency::Both};
    std::atomic<int> dragOpacity{kDefaultDragOpacity};
    std::atomic<int> dragFadeIn{kDefaultDragFadeIn};
    std::atomic<int> dragFadeOut{kDefaultDragFadeOut};
    std::atomic<bool> wheelScale{true};
    std::atomic<int> wheelStep{kDefaultWheelStep};
    std::atomic<WheelAnchor> wheelAnchor{WheelAnchor::CenterOrHold};
    std::atomic<MenuBarMode> menuBarMode{MenuBarMode::Hide};
    std::atomic<bool> hideByDefault{false};
    std::atomic<COLORREF> borderActive{kColorUntouched};
    std::atomic<COLORREF> borderInactive{kColorUntouched};
    // Off: the border colors apply to every window with a frame. On: only to
    // the ones the mod has taken the title bar from.
    std::atomic<bool> borderFramelessOnly{false};
    std::atomic<int> borderFadeDuration{kDefaultBorderFade};
    std::atomic<bool> desktopWinTab{true};
    std::atomic<bool> desktopToEmpty{true};  // with nowhere to go
    std::atomic<int> corners{DWMWCP_DEFAULT};
    std::atomic<SnapMode> snap{SnapMode::Both};
    std::atomic<int> snapDistance{kDefaultSnapDistance};
    std::atomic<int> snapWindowGap{0};
    std::atomic<int> snapMonitorGap{0};
    std::atomic<UINT> keepAspectVk{VK_SHIFT};   // 0: never keep the ratio
    std::atomic<UINT> snapModifierVk{VK_CONTROL};  // 0: no extra key at all
    std::atomic<bool> snapModifierHold{true};   // false: it suppresses instead
};

extern Settings g_settings;

std::wstring NormalizeSettingString(PCWSTR raw);
std::wstring ThisProgramName();
bool ProgramEntryMatches(PCWSTR entry, const std::wstring& program);
UINT ParseKeyName(PCWSTR raw);
// An empty setting means the documented default, which is not the same one
// for every binding - Windhawk hands out an empty string for a setting it has
// never written.
Hotkey ParseHotkey(PCWSTR raw, PCWSTR whenEmpty = kDefaultHotkey);
COLORREF ParseBorderColor(PCWSTR raw);
MenuBarMode ParseMenuBarMode(PCWSTR raw);
int ParseCorners(PCWSTR raw);
DragTranslucency ParseDragTranslucency(PCWSTR raw);
WheelAnchor ParseWheelAnchor(PCWSTR raw);
ModifierClick ParseModifierClick(PCWSTR raw);
SnapMode ParseSnapMode(PCWSTR raw);
bool ParseSnapModifierHold(PCWSTR raw);
// A single modifier key, as a virtual key; 0 when the setting turns it off.
UINT ParseModifierVk(PCWSTR raw, UINT whenEmpty);
WindowAction ParseWindowAction(PCWSTR raw, WindowAction whenEmpty);
int ClampedSetting(int value, int fallback, int low, int high);
void LoadSettings();

////////////////////////////////////////////////////////////////////////////////
// Which windows we may touch, and their metrics

bool IsExcludedClass(HWND hwnd);
bool HasFrameStyles(LONG_PTR style);
bool IsFrameWindow(HWND hwnd);
bool IsAutoHideCandidate(HWND hwnd);
UINT WindowDpi(HWND hwnd);
int ResizeHandleHeight(HWND hwnd);

////////////////////////////////////////////////////////////////////////////////
// Non-client layout of a window with a hidden title bar

int MaximizedTopInset(HWND hwnd, const RECT& proposed);
LRESULT OnNcCalcSize(HWND hwnd, WPARAM wParam, LPARAM lParam);
LRESULT AdjustHitTest(HWND hwnd, LRESULT hit, LPARAM lParam);

////////////////////////////////////////////////////////////////////////////////
// DWM decorations

bool BorderColorsWanted();
bool IsBorderColorTarget(HWND hwnd);
COLORREF AccentBorderColor();
COLORREF BorderColorFor(bool active);
// The border color last written to a window, which is where a fade starts
// from. kColorUntouched when nothing of ours is on it.
COLORREF CurrentBorderColor(HWND hwnd);
bool IsBlendableColor(COLORREF color);
COLORREF BlendColor(COLORREF from, COLORREF to, double t);
void ApplyBorderColor(HWND hwnd, bool active);
void RestoreBorderColor(HWND hwnd);
void RefreshBorderColor(HWND hwnd);
void RefreshBorderColors();
void RestoreAllBorderColors();
std::vector<HWND> SnapshotColoredWindows();
void ForgetBorderColor(HWND hwnd);
// A window's own thread reporting that it has gained or lost focus.
void OnWindowActivation(HWND hwnd, bool active);
// The border color on a focus change, faded across instead of switched.
void AnimateBorderColor(HWND hwnd, bool active);
void CancelBorderFade(HWND hwnd);
void FinishBorderFades();
void ApplyCorners(HWND hwnd);
void ApplyDwmAttributes(HWND hwnd);
void RestoreDwmAttributes(HWND hwnd);

////////////////////////////////////////////////////////////////////////////////
// Hiding / restoring the title bar

// Every request to hide/restore a title bar is posted to the window as
// `g_msgFrameless` and handled on the window's own thread, which is the only
// thread that may (un)subclass it.
// kActionAutoHide is the "hide by default" path. Unlike the explicit actions it
// is re-checked when it runs, because by then the window may have turned out to
// be something we should not touch.
enum FramelessAction : WPARAM {
    kActionToggle = 0,
    kActionHide = 1,
    kActionShow = 2,
    kActionAutoHide = 3,
};

extern UINT g_msgFrameless;  // RegisterWindowMessage, set in Wh_ModInit

constexpr UINT_PTR kSubclassId = 0x48597072;  // 'Hypr'

// dwRefData flag for the subclass: the frame layout was left to the system.
constexpr DWORD_PTR kRefKeepMenu = 1;

bool IsFrameless(HWND hwnd);
void MarkDwmTouched(HWND hwnd);
bool IsDwmTouched(HWND hwnd);
std::vector<HWND> SnapshotFramelessWindows();
std::vector<HWND> SnapshotAutoHiddenWindows();
void RefreshFrame(HWND hwnd);
bool MakeFrameless(HWND hwnd, bool autoHidden);
void RestoreFrame(HWND hwnd);
void HandleFramelessRequest(HWND hwnd, WPARAM action);
void RequestFrameless(HWND hwnd, WPARAM action);
void AutoHideExistingWindows();
void RestoreAutoHiddenWindows();

////////////////////////////////////////////////////////////////////////////////
// Hotkey

bool HandleHotkey(const MSG* msg);

////////////////////////////////////////////////////////////////////////////////
// Win + mouse: Start-menu suppression and the drags themselves

constexpr WORD kMaskVk = 0xE8;  // unassigned VK, only used as "a key was hit"
// Marks the keystrokes the mod injects to keep the Start menu shut, so its
// own low-level hook lets them through.
constexpr ULONG_PTR kInjectedMarker = 0x48797072;  // 'Hypr'
// And a press the shell's mouse hook took, pressed again by the window it was
// for - see mouse.cpp. An ordinary press for everything but that hook.
constexpr ULONG_PTR kReplayMarker = 0x48797073;  // 'Hyps'

void ArmWinMask(bool usingWin, bool usingAlt = false);
void MaskModifierTap();
bool WinKeyDown();
bool AltKeyDown();
bool IsShellProcess();
bool IsElevatedProcess();
void StartKeyboardServer();
void StartKeyboardServerForWindow();
void ShutdownKeyboardServer();
bool HandleBindingKey(UINT vk, bool down);

// Win+Tab through the virtual desktops - see desktops.cpp.
void StartDesktopThread();
void DesktopSettingsChanged();
void ShutdownDesktopThread();
bool HandleDesktopHotkey(const MSG* msg);
void BringDesktopForwardForKey(UINT vk);

// A drag is requested with this message, posted to the window that is to be
// moved or resized - see the comment at the top of drag.cpp.
extern UINT g_msgDrag;  // RegisterWindowMessage, set in Wh_ModInit

enum DragKind : WPARAM {
    kDragMove = 0,
    kDragResize = 1,
    // Not a drag: the drag is over, and the window's own thread is the only
    // one that may take the mod's WS_EX_LAYERED back off - see drag_fade.cpp.
    kDragUnfade = 2,
    // Nor is this one: a gesture that asks the window to do something to
    // itself, with the WindowAction in lParam - see actions.cpp.
    kDragAction = 3,
    // Sent, not posted, and answered by the drag subclass rather than by the
    // message hook: the teardown taking that subclass off - see snap.cpp.
    kDragUnsnap = 4,
    // From the shell's mouse hook: a press it took over content that gets its
    // input past the message queue, to be pressed again where the window sees
    // it, with the MOUSEEVENTF_ flag of the left or right button going down
    // in lParam - see mouse.cpp.
    kDragUncover = 5,
    // And once that press has gone through: the window covers up again.
    kDragCover = 6,
    // From the shell's mouse hook as well: the modifier held and the wheel
    // turned over the window, by the wheel delta in lParam - see actions.cpp.
    kDragScale = 7,
    // Sent by the teardown: a subclass waiting for a touchpad's question,
    // taken off - see mouse.cpp.
    kDragUntouch = 8,
    // A resize, with the left button held rather than the right - see
    // drag.cpp.
    kDragResizeLeft = 9,
};

// How long the thread that ends a resize waits for its loop to start. The
// request that starts it is posted, so the loop is a moment away - and the
// release it posts is no use at all before the loop is running.
constexpr int kMoveSizeStartWaitMs = 500;

bool IsDragModifierDown();
bool IsLeftResizeDown();
LRESULT CALLBACK LowLevelMouseProc(int code, WPARAM wParam, LPARAM lParam);
bool HasCompositionContentWindow();
void EndWheelTurn();
bool TouchpadGoesToWheel(WPARAM pointerHitTest);
void KeepTouchpadFromApp(HWND hwnd, WPARAM pointerHitTest);
std::vector<HWND> SnapshotTouchpadWindows();
void UncoverAndPress(HWND root, const MSG* msg);
void RestoreUncoveredAfterReplay();
void RestoreUncovered(bool allThreads = false);
void RestoreAllUncovered();
bool IsThreadInMoveSizeLoop(DWORD threadId);
bool IsInMoveSizeLoop();
int PhysicalButtonVk(bool right);
void ForceLeftButtonDown();
UINT ResizeEdgeForPoint(const RECT& rc, POINT pt);
void RequestDrag(HWND root, WPARAM kind, POINT pt);

// Runs on the target window's thread and rewrites the request into the system
// command that starts a move or size loop. Returns false if there is nothing
// to start, which means the message is to be swallowed.

// Per-thread: a button-up to swallow because we swallowed its button-down.
extern thread_local bool g_swallowButtonUp[2];  // [0] = left, [1] = right
bool HandleDragRequest(MSG* msg);
bool HandleScaleTimer(const MSG* msg);
// After a drag: the left button the loop needed held, let go again.
void ReleaseForcedLeftButton();
// Alt, kept from a move loop that would take it for a snap modifier.
void TrackAltInMove(const MSG* msg);
void RestoreHiddenAlt();
bool HandleModifierButtonDown(const MSG* msg, bool right);
// A held press that turned out to be a click, handed to the application.
bool ReplayHeldPress(MSG* msg, bool right);
bool HandleButtonUp(bool right);
bool HandleLeftButtonUp();

////////////////////////////////////////////////////////////////////////////////
// Gestures with the modifier held, and what they do to a window

// A binding matches when its key or button is the one that just arrived and
// the modifiers are held.
bool IsMouseButtonVk(UINT vk);
UINT ButtonVkForMessage(UINT message, WPARAM wParam);
bool MatchesShortcut(const Hotkey& binding, UINT vk, bool now = false);
bool HandleShortcutButton(const MSG* msg);
bool HandleShortcutButtonUp(const MSG* msg);

int DoubleClickTimeMs();
// Whether this press and the one before it on this thread are a double click.
// Takes the tick instead of reading the clock, so the rules are testable.
bool IsDoubleClickAt(HWND root, POINT pt, DWORD tick);
bool TakeWindowAction(MSG* msg, WindowAction action);
bool CanTakeAction(HWND root, WindowAction action);
bool CanTakeActionUnderCursor(HWND root, WindowAction action);
bool CanScale(HWND root);
void ScaleWindow(HWND root, int wheelDelta);
void RequestWindowAction(HWND root, WindowAction action);

////////////////////////////////////////////////////////////////////////////////
// Magnetic edges, and the aspect ratio while resizing

RECT VisibleFrameOf(HWND hwnd);
RECT FrameInsetOf(HWND hwnd);
RECT WindowToFrame(const RECT& rc, const RECT& inset);
RECT FrameToWindow(const RECT& rc, const RECT& inset);
bool SnapAllowedWith(bool modifierDown);
bool SnapAllowed();
void BeginDragSnap(HWND root, WPARAM kind, POINT pt);
void EndDragSnap(HWND hwnd);
bool ScaleDraggedWindow(HWND hwnd, double factor);
std::vector<HWND> SnapshotSnappedWindows();

////////////////////////////////////////////////////////////////////////////////
// Translucency while dragging

BYTE DragAlphaFor(BYTE baseAlpha, int opacityPercent);
BYTE FadeAlphaAt(BYTE from, BYTE to, int durationMs, int elapsedMs);
void BeginDragFade(HWND root, WPARAM kind);
void EndDragFade(HWND hwnd);
void UnfadeDraggedWindows();

////////////////////////////////////////////////////////////////////////////////
// The hooked APIs

void ProcessRetrievedMessage(MSG* msg);
void OnWindowCreated(HWND hwnd, DWORD dwStyle);

// Message interception lives in a WH_GETMESSAGE hook, one per pumping thread -
// see the comment at the top of hooks.cpp for why it is not an API hook.
void InstallMessageHookForThread();
void InstallMessageHooks();
void RemoveMessageHooks();
void RefreshCallWndProcHooks();

using CreateWindowExW_t = decltype(&CreateWindowExW);
using CreateWindowExA_t = decltype(&CreateWindowExA);

extern CreateWindowExW_t CreateWindowExW_Original;
extern CreateWindowExA_t CreateWindowExA_Original;

HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,
                                 LPCWSTR lpClassName,
                                 LPCWSTR lpWindowName,
                                 DWORD dwStyle,
                                 int X,
                                 int Y,
                                 int nWidth,
                                 int nHeight,
                                 HWND hWndParent,
                                 HMENU hMenu,
                                 HINSTANCE hInstance,
                                 LPVOID lpParam);
HWND WINAPI CreateWindowExA_Hook(DWORD dwExStyle,
                                 LPCSTR lpClassName,
                                 LPCSTR lpWindowName,
                                 DWORD dwStyle,
                                 int X,
                                 int Y,
                                 int nWidth,
                                 int nHeight,
                                 HWND hWndParent,
                                 HMENU hMenu,
                                 HINSTANCE hInstance,
                                 LPVOID lpParam);

////////////////////////////////////////////////////////////////////////////////
// src/settings.cpp
////////////////////////////////////////////////////////////////////////////////
// Reading and parsing the mod's user settings.

Settings g_settings;

std::wstring NormalizeSettingString(PCWSTR raw) {
    std::wstring s = raw ? raw : L"";
    auto notSpace = [](wchar_t c) { return !std::iswspace(c); };
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), notSpace));
    s.erase(std::find_if(s.rbegin(), s.rend(), notSpace).base(), s.end());
    for (auto& c : s) {
        c = (wchar_t)std::towupper(c);
    }
    return s;
}

// The file name of this process's executable, e.g. "explorer.exe".
std::wstring ThisProgramName() {
    WCHAR path[MAX_PATH];
    DWORD len = GetModuleFileNameW(nullptr, path, MAX_PATH);
    if (!len || len >= MAX_PATH) {
        return L"";
    }
    PCWSTR slash = wcsrchr(path, L'\\');
    return slash ? slash + 1 : path;
}

// Whether a program list entry names this program. Compared by file name and
// case-insensitively, so a full path pasted into the setting works as well.
bool ProgramEntryMatches(PCWSTR entry, const std::wstring& program) {
    std::wstring name = NormalizeSettingString(entry);
    size_t slash = name.find_last_of(L"\\/");
    if (slash != std::wstring::npos) {
        name = name.substr(slash + 1);
    }
    return !name.empty() && name == NormalizeSettingString(program.c_str());
}

bool ProgramInListSetting(PCWSTR name) {
    std::wstring program = ThisProgramName();
    for (int i = 0; i < 256; i++) {
        WindhawkUtils::StringSetting entry(
            Wh_GetStringSetting(L"%s[%d]", name, i));
        if (!*entry.get()) {
            break;  // the end of the list
        }
        if (ProgramEntryMatches(entry, program)) {
            return true;
        }
    }
    return false;
}

// Turns the "key" setting into a virtual key code, 0 if unusable.
UINT ParseKeyName(PCWSTR raw) {
    std::wstring s = NormalizeSettingString(raw);
    if (s.empty()) {
        return 0;
    }

    if (s.rfind(L"VK_", 0) == 0) {
        s = s.substr(3);
    }

    if (s.size() == 1) {
        wchar_t c = s[0];
        if ((c >= L'A' && c <= L'Z') || (c >= L'0' && c <= L'9')) {
            return c;
        }
        SHORT vk = VkKeyScanW(c);
        return vk == -1 ? 0 : (vk & 0xFF);
    }

    if (s.size() > 2 && s[0] == L'0' && s[1] == L'X') {
        return (UINT)wcstoul(s.c_str() + 2, nullptr, 16) & 0xFF;
    }

    if (s[0] == L'F' && s.size() <= 3 &&
        std::all_of(s.begin() + 1, s.end(),
                    [](wchar_t c) { return std::iswdigit(c) != 0; })) {
        int n = _wtoi(s.c_str() + 1);
        if (n >= 1 && n <= 24) {
            return VK_F1 + n - 1;
        }
        return 0;
    }

    static const struct {
        PCWSTR name;
        UINT vk;
    } kNames[] = {
        {L"SPACE", VK_SPACE},        {L"TAB", VK_TAB},
        {L"ENTER", VK_RETURN},       {L"RETURN", VK_RETURN},
        {L"ESC", VK_ESCAPE},         {L"ESCAPE", VK_ESCAPE},
        {L"BACKSPACE", VK_BACK},     {L"BACK", VK_BACK},
        {L"INSERT", VK_INSERT},      {L"INS", VK_INSERT},
        {L"DELETE", VK_DELETE},      {L"DEL", VK_DELETE},
        {L"HOME", VK_HOME},          {L"END", VK_END},
        {L"PAGEUP", VK_PRIOR},       {L"PRIOR", VK_PRIOR},
        {L"PAGEDOWN", VK_NEXT},      {L"NEXT", VK_NEXT},
        {L"UP", VK_UP},              {L"DOWN", VK_DOWN},
        {L"LEFT", VK_LEFT},          {L"RIGHT", VK_RIGHT},
        {L"PAUSE", VK_PAUSE},        {L"SCROLLLOCK", VK_SCROLL},
        {L"SCROLL", VK_SCROLL},      {L"PRINTSCREEN", VK_SNAPSHOT},
        {L"SNAPSHOT", VK_SNAPSHOT},  {L"CAPSLOCK", VK_CAPITAL},
        {L"CAPITAL", VK_CAPITAL},    {L"NUMLOCK", VK_NUMLOCK},
        {L"APPS", VK_APPS},          {L"MENU", VK_APPS},
        {L"NUMPAD0", VK_NUMPAD0},    {L"NUMPAD1", VK_NUMPAD1},
        {L"NUMPAD2", VK_NUMPAD2},    {L"NUMPAD3", VK_NUMPAD3},
        {L"NUMPAD4", VK_NUMPAD4},    {L"NUMPAD5", VK_NUMPAD5},
        {L"NUMPAD6", VK_NUMPAD6},    {L"NUMPAD7", VK_NUMPAD7},
        {L"NUMPAD8", VK_NUMPAD8},    {L"NUMPAD9", VK_NUMPAD9},
        {L"MULTIPLY", VK_MULTIPLY},  {L"ADD", VK_ADD},
        {L"SUBTRACT", VK_SUBTRACT},  {L"DECIMAL", VK_DECIMAL},
        {L"DIVIDE", VK_DIVIDE},
        // Mouse buttons, so that a binding can be a click as easily as a key.
        {L"MBUTTON", VK_MBUTTON},    {L"MIDDLE", VK_MBUTTON},
        {L"XBUTTON1", VK_XBUTTON1},  {L"X1", VK_XBUTTON1},
        {L"XBUTTON2", VK_XBUTTON2},  {L"X2", VK_XBUTTON2},
        {L"LBUTTON", VK_LBUTTON},    {L"RBUTTON", VK_RBUTTON},
    };
    for (const auto& entry : kNames) {
        if (s == entry.name) {
            return entry.vk;
        }
    }

    return 0;
}

// "" / "default" -> untouched, "none" -> DWMWA_COLOR_NONE, "#RRGGBB" -> color.
COLORREF ParseBorderColor(PCWSTR raw) {
    std::wstring s = NormalizeSettingString(raw);
    if (s.empty() || s == L"DEFAULT") {
        return kColorUntouched;
    }
    if (s == L"NONE") {
        return DWMWA_COLOR_NONE;
    }
    if (s == L"ACCENT") {
        return kColorAccent;
    }
    if (s[0] == L'#') {
        s.erase(0, 1);
    }
    if (s.size() == 3) {
        s = {s[0], s[0], s[1], s[1], s[2], s[2]};
    }
    if (s.size() != 6 || !std::all_of(s.begin(), s.end(), [](wchar_t c) {
            return std::iswxdigit(c) != 0;
        })) {
        return kColorUntouched;
    }
    unsigned long v = wcstoul(s.c_str(), nullptr, 16);
    return RGB((v >> 16) & 0xFF, (v >> 8) & 0xFF, v & 0xFF);
}

MenuBarMode ParseMenuBarMode(PCWSTR raw) {
    std::wstring s = NormalizeSettingString(raw);
    if (s == L"KEEPMENU") {
        return MenuBarMode::KeepMenu;
    }
    if (s == L"SKIP") {
        return MenuBarMode::Skip;
    }
    return MenuBarMode::Hide;
}

// Anything unrecognized fades both drags, so a setting Windhawk has never
// written out - an empty string - means what the settings say it does.
DragTranslucency ParseDragTranslucency(PCWSTR raw) {
    std::wstring s = NormalizeSettingString(raw);
    if (s == L"OPAQUE" || s == L"OFF" || s == L"NONE") {
        return DragTranslucency::Off;
    }
    if (s == L"MOVE" || s == L"MOVEONLY") {
        return DragTranslucency::MoveOnly;
    }
    return DragTranslucency::Both;
}

// Anything unrecognized is the default, the center or the hold.
WheelAnchor ParseWheelAnchor(PCWSTR raw) {
    std::wstring s = NormalizeSettingString(raw);
    if (s == L"CURSOR") {
        return WheelAnchor::Cursor;
    }
    if (s == L"CENTERALWAYS") {
        return WheelAnchor::Center;
    }
    return WheelAnchor::CenterOrHold;
}

ModifierClick ParseModifierClick(PCWSTR raw) {
    std::wstring s = NormalizeSettingString(raw);
    if (s == L"APP") {
        return ModifierClick::App;
    }
    if (s == L"WINDOW") {
        return ModifierClick::Window;
    }
    return ModifierClick::Auto;
}

SnapMode ParseSnapMode(PCWSTR raw) {
    std::wstring s = NormalizeSettingString(raw);
    if (s == L"OFF" || s == L"NONE") {
        return SnapMode::Off;
    }
    if (s == L"MONITOR" || s == L"SCREEN") {
        return SnapMode::Monitor;
    }
    if (s == L"WINDOWS") {
        return SnapMode::Windows;
    }
    return SnapMode::Both;
}

// "suppress" turns the extra key around: magnetic unless it is held.
bool ParseSnapModifierHold(PCWSTR raw) {
    return NormalizeSettingString(raw) != L"SUPPRESS";
}

UINT ParseModifierVk(PCWSTR raw, UINT whenEmpty) {
    std::wstring s = NormalizeSettingString(raw);
    if (s.empty()) {
        return whenEmpty;
    }
    if (s == L"SHIFT") {
        return VK_SHIFT;
    }
    if (s == L"CTRL" || s == L"CONTROL") {
        return VK_CONTROL;
    }
    if (s == L"ALT") {
        return VK_MENU;
    }
    return 0;  // "off", and anything else nobody can hold down
}

// A setting Windhawk has never written out reads as zero, which is why zero
// means "the documented default" rather than a value of its own.
int ClampedSetting(int value, int fallback, int low, int high) {
    return value <= 0 ? fallback : std::clamp(value, low, high);
}

int ParseCorners(PCWSTR raw) {
    std::wstring s = NormalizeSettingString(raw);
    if (s == L"ROUND") {
        return DWMWCP_ROUND;
    }
    if (s == L"ROUNDSMALL") {
        return DWMWCP_ROUNDSMALL;
    }
    if (s == L"NONE") {
        return DWMWCP_DONOTROUND;
    }
    return DWMWCP_DEFAULT;
}

// Splits "Ctrl+Alt+H" into the key and its modifiers. An empty setting means
// "use the default", so the hotkey works even before Windhawk has written the
// settings out; "none" is how you turn it off.
Hotkey ParseHotkey(PCWSTR raw, PCWSTR whenEmpty) {
    std::wstring spec = NormalizeSettingString(raw);
    if (spec.empty()) {
        spec = NormalizeSettingString(whenEmpty);
    }
    if (spec == L"NONE" || spec == L"OFF" || spec == L"-") {
        return {};
    }

    Hotkey hotkey{};
    size_t pos = 0;
    while (pos <= spec.size()) {
        size_t plus = spec.find(L'+', pos);
        std::wstring token =
            spec.substr(pos, plus == std::wstring::npos ? plus : plus - pos);
        pos = plus == std::wstring::npos ? spec.size() + 1 : plus + 1;

        token = NormalizeSettingString(token.c_str());
        if (token.empty()) {
            continue;  // a stray or trailing "+"
        }
        if (token == L"CTRL" || token == L"CONTROL") {
            hotkey.ctrl = true;
        } else if (token == L"ALT") {
            hotkey.alt = true;
        } else if (token == L"SHIFT") {
            hotkey.shift = true;
        } else if (token == L"WIN" || token == L"SUPER" || token == L"META") {
            hotkey.win = true;
        } else {
            hotkey.vk = ParseKeyName(token.c_str());
        }
    }
    return hotkey;
}

void LoadSettings() {
    WindhawkUtils::StringSetting hotkey(Wh_GetStringSetting(L"titleBar.hotkey"));
    g_settings.hotkey = ParseHotkey(hotkey, kDefaultHotkey);

    WindhawkUtils::StringSetting shortcut(
        Wh_GetStringSetting(L"gestures.shortcut"));
    g_settings.windowShortcut = ParseHotkey(shortcut, kDefaultWindowShortcut);
    WindhawkUtils::StringSetting shortcutAction(
        Wh_GetStringSetting(L"gestures.shortcutAction"));
    g_settings.windowShortcutAction =
        ParseWindowAction(shortcutAction, WindowAction::Close);

    WindhawkUtils::StringSetting modifier(Wh_GetStringSetting(L"drag.modifier"));
    g_settings.dragModifier = NormalizeSettingString(modifier) == L"ALT"
                                  ? DragModifier::Alt
                                  : DragModifier::Win;
    WindhawkUtils::StringSetting click(Wh_GetStringSetting(L"drag.click"));
    g_settings.modifierClick = ParseModifierClick(click);
    g_settings.leftResize = Wh_GetIntSetting(L"drag.leftResize") != 0;

    g_settings.topEdgeResize = Wh_GetIntSetting(L"titleBar.topEdgeResize") != 0;

    WindhawkUtils::StringSetting doubleClick(
        Wh_GetStringSetting(L"gestures.doubleClickAction"));
    g_settings.doubleClickAction =
        ParseWindowAction(doubleClick, WindowAction::ToggleMaximize);
    // Zero is a value of its own here - "whatever the mouse settings say" -
    // so it survives the clamp.
    g_settings.doubleClickTime =
        ClampedSetting(Wh_GetIntSetting(L"gestures.doubleClickTime"), 0, 100, 2000);

    WindhawkUtils::StringSetting translucency(
        Wh_GetStringSetting(L"drag.translucency"));
    g_settings.dragTranslucency = ParseDragTranslucency(translucency);
    g_settings.dragOpacity = ClampedSetting(Wh_GetIntSetting(L"drag.opacity"),
                                            kDefaultDragOpacity, 10, 100);
    g_settings.dragFadeIn = ClampedSetting(Wh_GetIntSetting(L"drag.fadeIn"),
                                           kDefaultDragFadeIn, 1, 2000);
    g_settings.dragFadeOut = ClampedSetting(Wh_GetIntSetting(L"drag.fadeOut"),
                                            kDefaultDragFadeOut, 1, 2000);
    g_settings.wheelScale = Wh_GetIntSetting(L"drag.wheel") != 0;
    g_settings.wheelStep = ClampedSetting(Wh_GetIntSetting(L"drag.wheelStep"),
                                          kDefaultWheelStep, 1, 100);
    WindhawkUtils::StringSetting wheelAnchor(
        Wh_GetStringSetting(L"drag.wheelAround"));
    g_settings.wheelAnchor = ParseWheelAnchor(wheelAnchor);

    WindhawkUtils::StringSetting menuBar(Wh_GetStringSetting(L"titleBar.menuBar"));
    g_settings.menuBarMode = ParseMenuBarMode(menuBar);
    // Decided once for the whole process: the mod runs inside the program,
    // so a program on the list simply never hides anything by default.
    g_settings.hideByDefault = Wh_GetIntSetting(L"titleBar.hideByDefault") != 0 &&
                               !ProgramInListSetting(L"titleBar.exclude");

    WindhawkUtils::StringSetting active(Wh_GetStringSetting(L"border.active"));
    WindhawkUtils::StringSetting inactive(
        Wh_GetStringSetting(L"border.inactive"));
    g_settings.borderActive = ParseBorderColor(active);
    g_settings.borderInactive = ParseBorderColor(inactive);

    g_settings.borderFramelessOnly =
        Wh_GetIntSetting(L"border.framelessOnly") != 0;
    // Zero is a value of its own here - no fade - so it is not the fallback
    // other settings make of it.
    g_settings.borderFadeDuration =
        std::clamp(Wh_GetIntSetting(L"border.fadeDuration"), 0, 2000);

    g_settings.desktopWinTab = Wh_GetIntSetting(L"desktops.winTab") != 0;
    g_settings.desktopToEmpty = Wh_GetIntSetting(L"desktops.toEmpty") != 0;

    WindhawkUtils::StringSetting corners(Wh_GetStringSetting(L"titleBar.corners"));
    g_settings.corners = ParseCorners(corners);

    WindhawkUtils::StringSetting snap(Wh_GetStringSetting(L"snap.mode"));
    g_settings.snap = ParseSnapMode(snap);
    g_settings.snapDistance = ClampedSetting(Wh_GetIntSetting(L"snap.distance"),
                                             kDefaultSnapDistance, 1, 64);
    // Zero is a gap of its own here, so it survives the clamp.
    g_settings.snapWindowGap =
        ClampedSetting(Wh_GetIntSetting(L"snap.windowGap"), 0, 0, 64);
    g_settings.snapMonitorGap =
        ClampedSetting(Wh_GetIntSetting(L"snap.monitorGap"), 0, 0, 64);
    WindhawkUtils::StringSetting keepAspect(
        Wh_GetStringSetting(L"drag.keepAspect"));
    g_settings.keepAspectVk = ParseModifierVk(keepAspect, VK_SHIFT);
    WindhawkUtils::StringSetting snapModifier(
        Wh_GetStringSetting(L"snap.modifier"));
    g_settings.snapModifierVk = ParseModifierVk(snapModifier, VK_CONTROL);
    WindhawkUtils::StringSetting snapModifierWhen(
        Wh_GetStringSetting(L"snap.modifierWhen"));
    g_settings.snapModifierHold = ParseSnapModifierHold(snapModifierWhen);

    Hotkey key = g_settings.hotkey;
    Hotkey shortcutKey = g_settings.windowShortcut;
    Wh_Log(L"Settings: hotkey vk=0x%02X ctrl=%d alt=%d shift=%d win=%d, "
           L"dragModifier=%s, modifierClick=%d, topEdgeResize=%d, menuBar=%d, hideByDefault=%d, "
           L"dragTranslucency=%d, dragOpacity=%d",
           key.vk, (int)key.ctrl, (int)key.alt, (int)key.shift, (int)key.win,
           g_settings.dragModifier == DragModifier::Alt ? L"alt" : L"win",
           (int)g_settings.modifierClick.load(),
           (int)g_settings.topEdgeResize, (int)g_settings.menuBarMode.load(),
           (int)g_settings.hideByDefault,
           (int)g_settings.dragTranslucency.load(),
           g_settings.dragOpacity.load());
    Wh_Log(L"Settings: doubleClickAction=%d, doubleClickTime=%d (%d ms), "
           L"windowShortcut vk=0x%02X ctrl=%d alt=%d shift=%d win=%d -> %d",
           (int)g_settings.doubleClickAction.load(),
           g_settings.doubleClickTime.load(), DoubleClickTimeMs(),
           shortcutKey.vk, (int)shortcutKey.ctrl, (int)shortcutKey.alt,
           (int)shortcutKey.shift, (int)shortcutKey.win,
           (int)g_settings.windowShortcutAction.load());
}

////////////////////////////////////////////////////////////////////////////////
// src/window_info.cpp
////////////////////////////////////////////////////////////////////////////////
// Deciding which windows the mod may touch, and basic window metrics.

bool IsExcludedClass(HWND hwnd) {
    WCHAR cls[64];
    if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls))) {
        return false;
    }

    static constexpr PCWSTR kExcluded[] = {
        L"Progman",                      // desktop
        L"WorkerW",                      // desktop (wallpaper host)
        L"Shell_TrayWnd",                // taskbar
        L"Shell_SecondaryTrayWnd",       // taskbar on other monitors
        L"#32768",                       // popup menus
        L"tooltips_class32",             // tooltips
        L"Windows.UI.Core.CoreWindow",   // Start menu, search, etc.
        L"Xaml_WindowedPopupClass",      // XAML popups
        L"SysShadow",                    // menu shadows
    };
    for (PCWSTR excluded : kExcluded) {
        if (_wcsicmp(cls, excluded) == 0) {
            return true;
        }
    }
    return false;
}

bool HasFrameStyles(LONG_PTR style) {
    return (style & WS_CAPTION) == WS_CAPTION || (style & WS_THICKFRAME);
}

// Top-level windows with a real frame: those may be moved/resized with the
// modifier key and may have their title bar hidden.
bool IsFrameWindow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) {
        return false;
    }
    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    if (style & WS_CHILD) {
        return false;
    }
    if (!HasFrameStyles(style)) {
        return false;
    }
    return !IsExcludedClass(hwnd);
}

// Stricter rule for "hide by default": only ordinary windows with a real
// title bar, no tool windows and no non-activatable helper windows.
bool IsAutoHideCandidate(HWND hwnd) {
    if (!IsFrameWindow(hwnd)) {
        return false;
    }
    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    if ((style & WS_CAPTION) != WS_CAPTION) {
        return false;
    }
    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (exStyle & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) {
        return false;
    }
    return true;
}
UINT WindowDpi(HWND hwnd) {
    UINT dpi = GetDpiForWindow(hwnd);
    return dpi ? dpi : 96;
}

// Thickness of the (invisible) resize frame around a window.
int ResizeHandleHeight(HWND hwnd) {
    UINT dpi = WindowDpi(hwnd);
    return GetSystemMetricsForDpi(SM_CYSIZEFRAME, dpi) +
           GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);
}

////////////////////////////////////////////////////////////////////////////////
// src/frame_geometry.cpp
////////////////////////////////////////////////////////////////////////////////
// Non-client layout of a window whose title bar is hidden: where the
// client area starts and which edges still resize.

// A maximized window is positioned so that its resize frame lies outside the
// monitor. Without a caption the client area would start in that invisible
// strip, so push it down to where the monitor's work area begins.
int MaximizedTopInset(HWND hwnd, const RECT& proposed) {
    if (!(GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_MAXIMIZE)) {
        return 0;
    }

    int handle = ResizeHandleHeight(hwnd);
    HMONITOR monitor = MonitorFromRect(&proposed, MONITOR_DEFAULTTONEAREST);
    MONITORINFO info{sizeof(info)};
    if (!monitor || !GetMonitorInfoW(monitor, &info)) {
        return handle;
    }
    return (int)std::clamp<LONG>(info.rcWork.top - proposed.top, 0, handle);
}

LRESULT OnNcCalcSize(HWND hwnd, WPARAM wParam, LPARAM lParam) {
    RECT* rc = wParam ? &reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam)->rgrc[0]
                      : reinterpret_cast<RECT*>(lParam);
    const RECT proposed = *rc;

    // Let the app / DefWindowProc lay out the frame as usual, then take the
    // whole top part (frame + caption) away. Left, right and bottom frames
    // stay, so DWM keeps drawing the border, the shadow and rounded corners.
    LRESULT result = DefSubclassProc(hwnd, WM_NCCALCSIZE, wParam, lParam);

    LONG top = proposed.top + MaximizedTopInset(hwnd, proposed);
    if (top < rc->top && top < rc->bottom) {
        rc->top = top;
    }

    return result;
}

// With the caption gone, DefWindowProc reports HTCLIENT for the top edge.
// Optionally turn the topmost few pixels back into a resize handle.
LRESULT AdjustHitTest(HWND hwnd, LRESULT hit, LPARAM lParam) {
    if (!g_settings.topEdgeResize) {
        return hit;
    }
    if (hit != HTCLIENT && hit != HTLEFT && hit != HTRIGHT) {
        return hit;
    }

    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    if (!(style & WS_THICKFRAME) || (style & WS_MAXIMIZE)) {
        return hit;
    }

    POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
    RECT rc;
    if (!GetWindowRect(hwnd, &rc)) {
        return hit;
    }

    int handle = ResizeHandleHeight(hwnd);
    if (pt.y < rc.top || pt.y >= rc.top + handle) {
        return hit;
    }
    if (hit == HTLEFT || pt.x < rc.left + 2 * handle) {
        return HTTOPLEFT;
    }
    if (hit == HTRIGHT || pt.x >= rc.right - 2 * handle) {
        return HTTOPRIGHT;
    }
    return HTTOP;
}

////////////////////////////////////////////////////////////////////////////////
// src/decorations.cpp
////////////////////////////////////////////////////////////////////////////////
// DWM decorations (border color, corner preference) of frameless windows, and
// the fade from one border color to the other when focus moves.
//
// Anything set here has to be written back as the system default when the
// setting is cleared, not merely skipped: the window keeps whatever was last
// written to it, so a border that is no longer configured would otherwise stay
// on screen until its title bar comes back.

// The Windows accent color, as a real color to paint with.
//
// AccentColor in the registry is 0xAABBGGRR - a COLORREF with an alpha on top
// of it - so the alpha is all that has to come off. That it is that way round
// and not ARGB is measurable: ColorizationColor next to it holds the same
// color, is documented as ARGB, and the two values are byte-swapped copies of
// each other. DwmGetColorizationColor, the fallback here, is ARGB as well, so
// that one does need its red and blue exchanged.
COLORREF AccentBorderColor() {
    DWORD accent = 0;
    DWORD size = sizeof(accent);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\DWM",
                     L"AccentColor", RRF_RT_REG_DWORD, nullptr, &accent,
                     &size) == ERROR_SUCCESS) {
        return accent & 0x00FFFFFF;
    }

    DWORD colorization = 0;
    BOOL opaque = FALSE;
    if (SUCCEEDED(DwmGetColorizationColor(&colorization, &opaque))) {
        return RGB((colorization >> 16) & 0xFF, (colorization >> 8) & 0xFF,
                   colorization & 0xFF);
    }
    return kColorUntouched;  // no accent to be had; leave the border alone
}

// The color a window's border is meant to have, the way DWM wants it: a color,
// or DWMWA_COLOR_DEFAULT when the setting is empty.
COLORREF BorderColorFor(bool active) {
    COLORREF color =
        active ? g_settings.borderActive : g_settings.borderInactive;
    if (color == kColorAccent) {
        color = AccentBorderColor();
    }
    return color == kColorUntouched ? (COLORREF)DWMWA_COLOR_DEFAULT : color;
}

// Whether a color is one that can be faded through. The sentinels - "leave it
// alone", "system default", "no border" - are states rather than colors, and
// there is no halfway between a color and no border at all, so those switch
// at once. A real COLORREF has nothing in its top byte.
bool IsBlendableColor(COLORREF color) {
    return (color & 0xFF000000) == 0;
}

COLORREF BlendColor(COLORREF from, COLORREF to, double t) {
    auto channel = [&](int shift) {
        int a = (from >> shift) & 0xFF;
        int b = (to >> shift) & 0xFF;
        return (COLORREF)(int)(a + (b - a) * t + 0.5) << shift;
    };
    return channel(0) | channel(8) | channel(16);
}

////////////////////////////////////////////////////////////////////////////////
// Which windows carry a border color of ours
//
// Every window with a frame, or only the ones the mod has taken the title bar
// from when the setting says so. The bookkeeping lives here rather than with
// the title bars, because a window with a border color of ours is not
// necessarily one the mod has touched in any other way - and the color it
// last got is where a fade to the other one starts from.

std::mutex g_coloredMutex;
std::unordered_map<HWND, COLORREF> g_colored;

bool BorderColorsWanted() {
    return g_settings.borderActive != kColorUntouched ||
           g_settings.borderInactive != kColorUntouched;
}

bool IsBorderColorTarget(HWND hwnd) {
    if (!BorderColorsWanted()) {
        return false;
    }
    return g_settings.borderFramelessOnly ? IsFrameless(hwnd)
                                          : IsFrameWindow(hwnd);
}

COLORREF CurrentBorderColor(HWND hwnd) {
    std::lock_guard<std::mutex> lock(g_coloredMutex);
    auto it = g_colored.find(hwnd);
    return it == g_colored.end() ? kColorUntouched : it->second;
}

void ForgetBorderColor(HWND hwnd) {
    std::lock_guard<std::mutex> lock(g_coloredMutex);
    g_colored.erase(hwnd);
}

std::vector<HWND> SnapshotColoredWindows() {
    std::lock_guard<std::mutex> lock(g_coloredMutex);
    std::vector<HWND> result;
    result.reserve(g_colored.size());
    for (const auto& [hwnd, color] : g_colored) {
        result.push_back(hwnd);
    }
    return result;
}

void WriteBorderColor(HWND hwnd, COLORREF color) {
    DwmSetWindowAttribute(hwnd, DWMWA_BORDER_COLOR, &color, sizeof(color));
    if (color == (COLORREF)DWMWA_COLOR_DEFAULT) {
        ForgetBorderColor(hwnd);  // the border is the system's own again
        return;
    }
    std::lock_guard<std::mutex> lock(g_coloredMutex);
    g_colored[hwnd] = color;
}

////////////////////////////////////////////////////////////////////////////////
// The fade between the two colors
//
// One thread drives every window that is fading right now and stops as soon as
// there is nothing left to fade, so alt-tabbing through ten windows costs one
// thread rather than ten. A window that is already fading is retargeted rather
// than restarted: focus that comes back mid-fade turns the color around from
// where it is instead of jumping to the far end first.
//
// DwmSetWindowAttribute sends the window nothing, so it is safe to call from
// this thread. g_borderMutex is never held while it is called, and never while
// the window bookkeeping's own lock is taken.

struct BorderFade {
    COLORREF from;
    COLORREF to;
    DWORD startTick;
    int durationMs;
    // Which fade of this window's this is: a retarget gets a new one, so a
    // batch of writes that was in flight at the time knows not to end it.
    unsigned long long seq;
};

std::mutex g_borderMutex;
std::unordered_map<HWND, BorderFade> g_borderFades;
bool g_borderThreadRunning;        // guarded by g_borderMutex
unsigned long long g_borderSeq;    // guarded by g_borderMutex
// Set while the thread is handing a batch of colors to DWM, which it does
// outside the lock - the window bookkeeping has a lock of its own, and the
// two are never held at once.
std::atomic<bool> g_borderWriting;

constexpr DWORD kBorderStepMs = 16;    // ~60 Hz
constexpr int kBorderFinishWaitMs = 300;
constexpr int kBorderWriteWaitMs = 20;

// Stops the fade and waits for any color already on its way to the window, so
// that whatever the caller writes next is what stays on it.
void CancelBorderFade(HWND hwnd) {
    {
        std::lock_guard<std::mutex> lock(g_borderMutex);
        g_borderFades.erase(hwnd);
    }
    for (int waited = 0; waited < kBorderWriteWaitMs && g_borderWriting;
         waited++) {
        Sleep(1);
    }
}

DWORD WINAPI BorderFadeThread(LPVOID param) {
    bool more = true;
    while (more) {
        std::vector<std::pair<HWND, COLORREF>> writes;
        std::vector<std::pair<HWND, unsigned long long>> finished;
        {
            std::lock_guard<std::mutex> lock(g_borderMutex);
            if (g_uninitializing) {
                // The teardown writes the default back over every window of
                // ours, and a color landing after that would stay on it for
                // good.
                g_borderFades.clear();
            }
            DWORD now = GetTickCount();
            for (const auto& [hwnd, fade] : g_borderFades) {
                double t = AnimationProgress((int)(now - fade.startTick),
                                             fade.durationMs);
                bool alive = IsWindow(hwnd);
                if (alive) {
                    writes.emplace_back(hwnd, BlendColor(fade.from, fade.to, t));
                }
                if (!alive || t >= 1.0) {
                    finished.emplace_back(hwnd, fade.seq);
                }
            }
        }

        g_borderWriting = true;
        for (const auto& [hwnd, color] : writes) {
            WriteBorderColor(hwnd, color);
        }
        g_borderWriting = false;

        // Only now is a finished fade over. Taking it out any earlier would
        // say the window is done while its last color is still on its way,
        // which is exactly what the teardown and the restore wait for.
        {
            std::lock_guard<std::mutex> lock(g_borderMutex);
            for (const auto& [hwnd, seq] : finished) {
                auto it = g_borderFades.find(hwnd);
                if (it != g_borderFades.end() && it->second.seq == seq) {
                    g_borderFades.erase(it);
                }
            }
            more = !g_borderFades.empty();
            if (!more) {
                g_borderThreadRunning = false;
            }
        }
        if (more) {
            Sleep(kBorderStepMs);
        }
    }
    g_modRefCount--;  // the last thing this thread does in the mod's image
    return 0;
}

void ApplyBorderColor(HWND hwnd, bool active) {
    CancelBorderFade(hwnd);

    COLORREF target = BorderColorFor(active);
    if (target == (COLORREF)DWMWA_COLOR_DEFAULT &&
        CurrentBorderColor(hwnd) == kColorUntouched) {
        return;  // nothing to set, and nothing of ours to undo
    }
    WriteBorderColor(hwnd, target);
}

// Hands the border back to the system, if the mod had it at all.
void RestoreBorderColor(HWND hwnd) {
    CancelBorderFade(hwnd);
    if (CurrentBorderColor(hwnd) == kColorUntouched) {
        return;
    }
    WriteBorderColor(hwnd, (COLORREF)DWMWA_COLOR_DEFAULT);
}

// Puts one window's border where the settings say it belongs: colored when it
// is a window they apply to, and the system's own when it is not one any more.
void RefreshBorderColor(HWND hwnd) {
    if (IsBorderColorTarget(hwnd)) {
        ApplyBorderColor(hwnd, GetForegroundWindow() == hwnd);
    } else {
        RestoreBorderColor(hwnd);
    }
}

// What a window's own thread reports through the hook on sent messages, which
// is the only place a window that the mod has not otherwise touched says
// anything about its focus - see hooks.cpp.
void OnWindowActivation(HWND hwnd, bool active) {
    if (IsBorderColorTarget(hwnd)) {
        AnimateBorderColor(hwnd, active);
    }
}

BOOL CALLBACK RefreshBorderColorProc(HWND hwnd, LPARAM lParam) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == (DWORD)lParam) {
        RefreshBorderColor(hwnd);
    }
    return TRUE;
}

// Every window of this process at once: at start-up, and whenever the
// settings change what the answer is.
void RefreshBorderColors() {
    for (HWND hwnd : SnapshotColoredWindows()) {
        if (!IsBorderColorTarget(hwnd)) {
            RestoreBorderColor(hwnd);
        }
    }
    EnumWindows(RefreshBorderColorProc, (LPARAM)GetCurrentProcessId());
}

// The teardown. DwmSetWindowAttribute sends the window nothing, so unlike a
// subclass this needs no round trip to anybody's thread.
void RestoreAllBorderColors() {
    for (HWND hwnd : SnapshotColoredWindows()) {
        RestoreBorderColor(hwnd);
    }
}

// The same color, reached over the configured time instead of at once. Used
// when focus moves; the frame being taken over, or the settings changing, sets
// the color outright - a screenful of windows fading in at once on start-up is
// not what anybody asked for.
void AnimateBorderColor(HWND hwnd, bool active) {
    COLORREF to = BorderColorFor(active);
    COLORREF from = CurrentBorderColor(hwnd);
    int durationMs = g_settings.borderFadeDuration;
    if (g_uninitializing || durationMs <= 0 || from == to ||
        !IsBlendableColor(from) || !IsBlendableColor(to)) {
        ApplyBorderColor(hwnd, active);  // nothing to fade between
        return;
    }

    MarkDwmTouched(hwnd);
    {
        std::lock_guard<std::mutex> lock(g_borderMutex);
        g_borderFades[hwnd] =
            BorderFade{from, to, GetTickCount(), durationMs, ++g_borderSeq};
        if (g_borderThreadRunning) {
            return;  // the thread picks this one up on its next step
        }
        g_borderThreadRunning = true;
    }

    if (!StartModThread(BorderFadeThread, nullptr)) {
        {
            std::lock_guard<std::mutex> lock(g_borderMutex);
            g_borderThreadRunning = false;
        }
        ApplyBorderColor(hwnd, active);
    }
}

// The teardown waits here, with g_uninitializing already set, before the
// windows get their defaults back.
void FinishBorderFades() {
    for (int waited = 0; waited < kBorderFinishWaitMs;
         waited += (int)kBorderStepMs) {
        {
            std::lock_guard<std::mutex> lock(g_borderMutex);
            if (!g_borderThreadRunning) {
                return;
            }
        }
        Sleep(kBorderStepMs);
    }
    Wh_Log(L"Border fades did not stop in time");
}

////////////////////////////////////////////////////////////////////////////////
// Corners

void ApplyCorners(HWND hwnd) {
    int corners = g_settings.corners;
    if (corners == DWMWCP_DEFAULT && !IsDwmTouched(hwnd)) {
        return;
    }
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &corners,
                          sizeof(corners));
    if (corners != DWMWCP_DEFAULT) {
        MarkDwmTouched(hwnd);
    }
}

void ApplyDwmAttributes(HWND hwnd) {
    RefreshBorderColor(hwnd);
    ApplyCorners(hwnd);
}

// The corners go back with the title bar; the border color is decided by
// whether the window is still one the colors apply to, which is not the same
// question any more.
void RestoreDwmAttributes(HWND hwnd) {
    int corners = DWMWCP_DEFAULT;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &corners,
                          sizeof(corners));
}

////////////////////////////////////////////////////////////////////////////////
// src/titlebar.cpp
////////////////////////////////////////////////////////////////////////////////
// Hiding and restoring a window's title bar, and the per-window
// bookkeeping that goes with it. Runs on the window's own thread.

UINT g_msgFrameless;  // RegisterWindowMessage, set in Wh_ModInit

struct FramelessState {
    bool dwmTouched = false;
    // The title bar was hidden by "hide by default", not by the hotkey, so
    // turning that setting off brings it back.
    bool autoHidden = false;
    // Style bits removed from the window (KeepMenu mode), restored later.
    DWORD removedStyle = 0;
    // Hide mode: the menu bar is drawn for now, while the keyboard is in it.
    bool menuBarShown = false;
};
std::mutex g_windowsMutex;
std::unordered_map<HWND, FramelessState> g_windows;

bool IsFrameless(HWND hwnd) {
    std::lock_guard<std::mutex> lock(g_windowsMutex);
    return g_windows.count(hwnd) != 0;
}

void MarkDwmTouched(HWND hwnd) {
    std::lock_guard<std::mutex> lock(g_windowsMutex);
    auto it = g_windows.find(hwnd);
    if (it != g_windows.end()) {
        it->second.dwmTouched = true;
    }
}

bool IsDwmTouched(HWND hwnd) {
    std::lock_guard<std::mutex> lock(g_windowsMutex);
    auto it = g_windows.find(hwnd);
    return it != g_windows.end() && it->second.dwmTouched;
}


std::vector<HWND> SnapshotFramelessWindows() {
    std::lock_guard<std::mutex> lock(g_windowsMutex);
    std::vector<HWND> result;
    result.reserve(g_windows.size());
    for (const auto& [hwnd, state] : g_windows) {
        result.push_back(hwnd);
    }
    return result;
}

std::vector<HWND> SnapshotAutoHiddenWindows() {
    std::lock_guard<std::mutex> lock(g_windowsMutex);
    std::vector<HWND> result;
    for (const auto& [hwnd, state] : g_windows) {
        if (state.autoHidden) {
            result.push_back(hwnd);
        }
    }
    return result;
}
void RefreshFrame(HWND hwnd) {
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER |
                     SWP_NOACTIVATE | SWP_FRAMECHANGED);
}

void HandleFramelessRequest(HWND hwnd, WPARAM action);

// A classic menu bar stays where the system lays it out, below where the
// caption was, which is now the client area. The system draws it whenever it
// sees fit - the frame painted on a resize or a focus change, Alt showing its
// underlines - over whatever the window has there. So it is drawn only while
// the keyboard is in it (Alt let go of, F10, Alt and a letter), and the window
// paints over it again once it is done. Of the frame, nothing else is lost:
// DWM draws what is left of it.

// Undocumented: what a themed window is sent to draw its menu bar - before
// the frame is painted, with the device context it is painted into; the bar
// itself; each item on it. Answering TRUE to the latter two says it was drawn.
constexpr UINT WM_UAHDRAWMENU = 0x0091;
constexpr UINT WM_UAHDRAWMENUITEM = 0x0092;
constexpr UINT WM_UAHINITMENU = 0x0093;
struct UAHMENU {
    HMENU menu;
    HDC dc;
    DWORD flags;
};

bool IsMenuBarShown(HWND hwnd) {
    std::lock_guard<std::mutex> lock(g_windowsMutex);
    auto it = g_windows.find(hwnd);
    return it != g_windows.end() && it->second.menuBarShown;
}

// Where the bar is drawn, in screen pixels; with a little room around it for
// the lines drawn along its edges, for keeping drawing off it.
bool MenuBarRect(HWND hwnd, RECT* rect, bool withEdges) {
    MENUBARINFO info{sizeof(info)};
    if (!GetMenuBarInfo(hwnd, OBJID_MENU, 0, &info) ||
        IsRectEmpty(&info.rcBar)) {
        return false;
    }
    *rect = info.rcBar;
    if (withEdges) {
        InflateRect(rect, 0,
                    MulDiv(3, GetDpiForWindow(hwnd), USER_DEFAULT_SCREEN_DPI));
    }
    return true;
}

// The bar is cut out of the device context the frame is about to be painted
// into. That is how the background of a bar the app draws itself is kept out
// as well: the system fills that in, unasked.
void KeepFramePaintingOffMenuBar(HWND hwnd, HDC dc) {
    RECT strip, window;
    if (!dc || WindowFromDC(dc) != hwnd || !MenuBarRect(hwnd, &strip, true) ||
        !GetWindowRect(hwnd, &window)) {
        return;  // one that is only measured with, say
    }
    OffsetRect(&strip, -window.left, -window.top);
    ExcludeClipRect(dc, strip.left, strip.top, strip.right, strip.bottom);
}

void ShowHiddenMenuBar(HWND hwnd, bool show) {
    {
        std::lock_guard<std::mutex> lock(g_windowsMutex);
        auto it = g_windows.find(hwnd);
        if (it == g_windows.end() || it->second.menuBarShown == show) {
            return;
        }
        it->second.menuBarShown = show;
    }
    RECT bar;
    if (!MenuBarRect(hwnd, &bar, !show)) {
        return;
    }
    if (show) {
        // Painting the frame would not reach it: what is painted of the frame
        // is what is outside the client area. So the frame is printed into
        // the window instead, onto just the bar.
        RECT window;
        GetWindowRect(hwnd, &window);
        OffsetRect(&bar, -window.left, -window.top);
        if (HDC dc = GetWindowDC(hwnd)) {
            IntersectClipRect(dc, bar.left, bar.top, bar.right, bar.bottom);
            DefWindowProcW(hwnd, WM_PRINT, reinterpret_cast<WPARAM>(dc),
                           PRF_NONCLIENT);
            ReleaseDC(hwnd, dc);
        }
        return;
    }
    MapWindowPoints(nullptr, hwnd, reinterpret_cast<POINT*>(&bar), 2);
    RedrawWindow(hwnd, &bar, nullptr,
                 RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
}

LRESULT CALLBACK FramelessSubclassProc(HWND hwnd,
                                       UINT uMsg,
                                       WPARAM wParam,
                                       LPARAM lParam,
                                       UINT_PTR uIdSubclass,
                                       DWORD_PTR dwRefData) {
    ModRef ref;  // the image must not go away under this procedure

    bool keepMenu = (dwRefData & kRefKeepMenu) != 0;

    switch (uMsg) {
        case WM_NCCALCSIZE:
            if (keepMenu) {
                break;  // WS_CAPTION was removed instead; default layout
            }
            return OnNcCalcSize(hwnd, wParam, lParam);

        case WM_UAHINITMENU:
            if (!keepMenu && !IsMenuBarShown(hwnd) && lParam) {
                KeepFramePaintingOffMenuBar(
                    hwnd, reinterpret_cast<UAHMENU*>(lParam)->dc);
            }
            break;

        case WM_NCPAINT:
        case WM_NCACTIVATE:
            // Past the app: what an app paints of its frame itself is what it
            // adds to its bar (a dark one's edges, for one), and nothing else
            // of the frame is left to paint. DefWindowProc still has to: DWM
            // only draws the frame of a window it sees painted the usual way.
            if (!keepMenu && GetMenu(hwnd) && !IsMenuBarShown(hwnd)) {
                return DefWindowProcW(hwnd, uMsg, wParam, lParam);
            }
            break;

        case WM_UAHDRAWMENU:
        case WM_UAHDRAWMENUITEM:
            if (!keepMenu && !IsMenuBarShown(hwnd)) {
                return TRUE;
            }
            break;

        case WM_DRAWITEM:
            // Items of an owner-drawn bar, which the app draws itself.
            if (!keepMenu && !IsMenuBarShown(hwnd)) {
                auto* item = reinterpret_cast<DRAWITEMSTRUCT*>(lParam);
                if (item->CtlType == ODT_MENU &&
                    reinterpret_cast<HMENU>(item->hwndItem) == GetMenu(hwnd)) {
                    return TRUE;
                }
            }
            break;

        case WM_MENUSELECT:
            // One of the bar's own items, not the window menu Alt+Space opens
            // (which leaves the keyboard on the bar's window menu icon on its
            // way out) or a context menu.
            if (!keepMenu && lParam &&
                reinterpret_cast<HMENU>(lParam) == GetMenu(hwnd) &&
                HIWORD(wParam) != 0xFFFF && !(HIWORD(wParam) & MF_SYSMENU)) {
                ShowHiddenMenuBar(hwnd, true);
            }
            break;

        case WM_EXITMENULOOP:
            if (!keepMenu) {
                ShowHiddenMenuBar(hwnd, false);
            }
            break;

        case WM_NCHITTEST: {
            LRESULT hit = DefSubclassProc(hwnd, uMsg, wParam, lParam);
            return keepMenu ? hit : AdjustHitTest(hwnd, hit, lParam);
        }

        case WM_NCDESTROY: {
            RemoveWindowSubclass(hwnd, FramelessSubclassProc, uIdSubclass);
            std::lock_guard<std::mutex> lock(g_windowsMutex);
            g_windows.erase(hwnd);
            break;
        }

        default:
            if (uMsg == g_msgFrameless) {
                // Sent (not posted) requests, e.g. the restore on unload.
                HandleFramelessRequest(hwnd, wParam);
                return 0;
            }
            break;
    }

    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

bool MakeFrameless(HWND hwnd, bool autoHidden) {
    if (!IsFrameWindow(hwnd)) {
        Wh_Log(L"Window %p is not eligible", hwnd);
        return false;
    }

    // A classic menu bar lives in the non-client area right below the
    // caption, laid out by the system from the title bar metrics. It can't
    // be kept there once the caption is gone, so either hide it along with
    // the title bar or fall back to plain WS_CAPTION removal.
    DWORD removedStyle = 0;
    if (GetMenu(hwnd)) {
        switch (g_settings.menuBarMode) {
            case MenuBarMode::Skip:
                Wh_Log(L"Window %p has a menu bar, skipping", hwnd);
                return false;
            case MenuBarMode::KeepMenu:
                // WS_CAPTION is WS_BORDER | WS_DLGFRAME; dropping only
                // WS_DLGFRAME removes the title bar but keeps the frame
                // metrics (and thus the client edges) unchanged.
                removedStyle =
                    (DWORD)GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_DLGFRAME;
                break;
            case MenuBarMode::Hide:
                break;
        }
    }

    if (!SetWindowSubclass(hwnd, FramelessSubclassProc, kSubclassId,
                           removedStyle ? kRefKeepMenu : 0)) {
        Wh_Log(L"SetWindowSubclass failed for %p", hwnd);
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(g_windowsMutex);
        FramelessState state;
        state.autoHidden = autoHidden;
        state.removedStyle = removedStyle;
        g_windows[hwnd] = state;
    }

    if (removedStyle) {
        LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
        SetWindowLongPtrW(hwnd, GWL_STYLE, style & ~(LONG_PTR)removedStyle);
    }

    ApplyDwmAttributes(hwnd);
    RefreshFrame(hwnd);
    Wh_Log(L"Title bar hidden for %p", hwnd);
    return true;
}

void RestoreFrame(HWND hwnd) {
    // Before the bookkeeping goes away: a fade still running would write a
    // color over the default this is about to put back.
    CancelBorderFade(hwnd);

    FramelessState state;
    {
        std::lock_guard<std::mutex> lock(g_windowsMutex);
        auto it = g_windows.find(hwnd);
        if (it == g_windows.end()) {
            return;
        }
        state = it->second;
        g_windows.erase(it);
    }

    RemoveWindowSubclass(hwnd, FramelessSubclassProc, kSubclassId);
    if (state.removedStyle) {
        LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
        SetWindowLongPtrW(hwnd, GWL_STYLE, style | state.removedStyle);
    }
    if (state.dwmTouched) {
        RestoreDwmAttributes(hwnd);
    }
    RefreshBorderColor(hwnd);
    RefreshFrame(hwnd);
    Wh_Log(L"Title bar restored for %p", hwnd);
}

void HandleFramelessRequest(HWND hwnd, WPARAM action) {
    bool autoHide = action == kActionAutoHide;
    if (autoHide) {
        // Re-check now that the window is done being created. A window that
        // never becomes visible is left alone: toolkits create throwaway
        // top-level windows during start-up (to probe for a pixel format, for
        // instance) and touching those can break the app's initialization.
        if (!IsWindowVisible(hwnd) || !IsAutoHideCandidate(hwnd)) {
            return;
        }
        action = kActionHide;
    }

    bool frameless = IsFrameless(hwnd);
    if (action == kActionToggle) {
        action = frameless ? kActionShow : kActionHide;
    }
    if (action == kActionHide && !frameless) {
        if (g_uninitializing) {
            return;  // the title bars are being put back, not taken away
        }
        MakeFrameless(hwnd, autoHide);
    } else if (action == kActionShow && frameless) {
        RestoreFrame(hwnd);
    }
}

// Asks the window (on its own thread, possibly in another process that also
// runs this mod) to hide/restore its title bar.
void RequestFrameless(HWND hwnd, WPARAM action) {
    if (!hwnd) {
        return;
    }
    if (!PostMessageW(hwnd, g_msgFrameless, action, 0)) {
        Wh_Log(L"PostMessage to %p failed (%u)", hwnd, GetLastError());
    }
}

BOOL CALLBACK AutoHideEnumProc(HWND hwnd, LPARAM lParam) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == (DWORD)lParam && IsAutoHideCandidate(hwnd)) {
        RequestFrameless(hwnd, kActionAutoHide);
    }
    return TRUE;
}

void AutoHideExistingWindows() {
    EnumWindows(AutoHideEnumProc, (LPARAM)GetCurrentProcessId());
}

// "Hide by default" was turned off: the windows it took are handed back. Ones
// hidden with the hotkey stay as they are.
void RestoreAutoHiddenWindows() {
    for (HWND hwnd : SnapshotAutoHiddenWindows()) {
        RequestFrameless(hwnd, kActionShow);
    }
}

////////////////////////////////////////////////////////////////////////////////
// src/hotkey.cpp
////////////////////////////////////////////////////////////////////////////////
// The key bindings: the one that toggles the title bar of the focused window,
// and the window shortcut when it has been bound to a key rather than to a
// mouse button.

bool HandleHotkey(const MSG* msg) {
    if (msg->lParam & (1 << 30)) {
        return false;  // auto-repeat
    }

    UINT vk = (UINT)msg->wParam;
    HWND target =
        msg->hwnd ? GetAncestor(msg->hwnd, GA_ROOT) : GetForegroundWindow();

    Hotkey titleBar = g_settings.hotkey;
    if (MatchesShortcut(titleBar, vk)) {
        Wh_Log(L"Hotkey: toggling %p", target);
        RequestFrameless(target, kActionToggle);
        return true;
    }

    // A shortcut bound to a key acts on the focused window - there is no
    // cursor in the gesture to point at anything else.
    Hotkey shortcut = g_settings.windowShortcut;
    WindowAction action = g_settings.windowShortcutAction;
    if (!g_uninitializing && action != WindowAction::None &&
        !IsMouseButtonVk(shortcut.vk) && MatchesShortcut(shortcut, vk) &&
        CanTakeAction(target, action)) {
        Wh_Log(L"Shortcut key on %p", target);
        RequestWindowAction(target, action);
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
// The same bindings, before Windows sees them
//
// Windows takes the keys of its own shortcuts - Win+W, Win+E... - before any
// window gets them, so a binding on one of those never reaches HandleHotkey.
// The keyboard thread's hook (keyboard.cpp) sees every key first and hands
// it here. A key it takes never reaches a window, so nothing is done
// twice, and HandleHotkey keeps working where there is no such hook.

// The key whose press was taken, so that its repeats and its release are
// taken too. Only touched on the server thread.
UINT g_takenKey;

// Returns true if the key is to be swallowed.
bool HandleBindingKey(UINT vk, bool down) {
    if (!down) {
        if (vk != g_takenKey) {
            return false;
        }
        g_takenKey = 0;
        return true;
    }
    if (vk == g_takenKey) {
        return true;  // auto-repeat of a key that was taken
    }

    Hotkey titleBar = g_settings.hotkey;
    Hotkey shortcut = g_settings.windowShortcut;
    WindowAction action = g_settings.windowShortcutAction;
    bool toggle = MatchesShortcut(titleBar, vk, true);
    bool act = !toggle && action != WindowAction::None &&
               !IsMouseButtonVk(shortcut.vk) &&
               MatchesShortcut(shortcut, vk, true);
    if (!toggle && !act) {
        return false;
    }
    // Taken even with nothing to act on - the desktop, the taskbar: a binding
    // is the user's, and Windows' own shortcut on the same keys doesn't come
    // back just because no window is in front.
    HWND target = GetAncestor(GetForegroundWindow(), GA_ROOT);
    g_takenKey = vk;
    // Win or Alt would otherwise count as tapped on their own when they come
    // up, the key between them having been taken: Start would open, or the
    // window's menu.
    Hotkey used = toggle ? titleBar : shortcut;
    if (used.win || used.alt) {
        MaskModifierTap();
    }
    if (toggle) {
        Wh_Log(L"Hotkey: toggling %p (keyboard hook)", target);
        RequestFrameless(target, kActionToggle);
    } else if (CanTakeAction(target, action)) {
        Wh_Log(L"Shortcut key on %p (keyboard hook)", target);
        RequestWindowAction(target, action);
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// src/keyboard.cpp
////////////////////////////////////////////////////////////////////////////////
// The mod's keyboard thread: a thread of its own with a low-level keyboard
// hook, for two things - keeping the Start menu shut after a Win + mouse
// gesture (the mask, below), and seeing the key bindings before Windows'
// own shortcuts do (hotkey.cpp).
//
// Windows opens the Start menu when the Win key is released and the last key
// pressed while it was held was the Win key itself (a lone Win tap). A mouse
// click doesn't count as a key, so after a Win + drag the release would open
// Start. Pressing another key marks the chord as "used", but only if it is the
// LAST key-down before the release - and because the user physically holds
// Win, it auto-repeats, so any mask sent at button-down is undone by the next
// Win auto-repeat. The reliable fix is to catch the physical Win key-up with a
// low-level keyboard hook, swallow it, and re-inject a masking key immediately
// followed by a fresh Win key-up (test/startmenu_probe.cpp tries the
// alternatives).
//
// Where that hook lives matters as much as what it does. Windows calls a
// low-level hook on the thread that installed it, and when that thread does
// not answer in time it goes on without it (test/mask_busy_probe.cpp):
//
//   * On a thread that is busy when Win comes up - an application taking a
//     window down, the taskbar rearranging its buttons after one has gone -
//     the release goes through and Start opens. The mask is still armed
//     afterwards, so it swallows the next Win press instead: the one that was
//     meant for Start.
//   * On the thread of a window that is being closed, the hook is simply gone
//     by then, together with the whole process when that was its last window.
//
// So the hook lives on a thread of the mod's own that does nothing else and is
// never busy, in the shell's process, which is never the one going away. A
// gesture anywhere asks that thread to arm through a message-only window. If
// no such window can be found - the shell is running an older copy of the mod,
// or there is no shell - the process starts a thread of its own for it. And
// the thread watches the Win key itself: a mask whose press is over comes
// down, whether or not the release ever reached the hook.
//
// Wherever its keyboard hook stays, the thread runs a low-level mouse hook as
// well, for the presses no message hook gets to see (mouse.cpp).
//
// The same hook also sees the key bindings before Windows does, so that one
// can take over a shortcut of Windows' own, such as Win+W (hotkey.cpp). For
// that it stays in place for good in the shell. Keys typed into an elevated
// window don't reach it there - Windows keeps them from the hooks of the
// processes below - so an elevated process with windows runs a server of its
// own as well, whose hook is in place while one of its windows is in front:
// never more than one of those at a time.

std::atomic<bool> g_winMaskArmed{false};

constexpr WCHAR kKeyboardServerClass[] = L"HyprlandWindowsKeyboard_" WH_MOD_ID;
constexpr UINT kMaskArm = WM_APP + 1;  // posted to the server: arm for this press
// Posted to the server: a mask in an elevated process has had the release.
constexpr UINT kMaskStandDown = WM_APP + 2;
constexpr UINT kMaskPollMs = 30;
constexpr UINT_PTR kMaskTimerId = 1;
constexpr DWORD kServerStartWaitMs = 2000;

// This process's own server, if it runs one. Guarded by g_serverMutex.
std::mutex g_serverMutex;
HWND g_keyboardServer;
DWORD g_serverThreadId;

// Only ever touched on the server thread.
HHOOK g_keyboardHook;
HHOOK g_mouseHook;
// The hook stays even with no mask armed: in the shell, and in an elevated
// process while one of its windows is in front.
bool g_keepKeyboardHook;
HWINEVENTHOOK g_foregroundHook;

bool WinKeyDown() {
    return ((GetAsyncKeyState(VK_LWIN) | GetAsyncKeyState(VK_RWIN)) & 0x8000) !=
           0;
}

// Which keys the armed mask is for. Alt has a lone tap of its own to keep
// from happening, when it is the modifier: the window's menu, or the access
// keys of a XAML app. Only ever touched on the server thread.
bool g_maskWin, g_maskAlt;
constexpr WPARAM kMaskForWin = 1, kMaskForAlt = 2;

HWND FindKeyboardServer();

// A key pressed while an elevated window is in front is never seen down by
// the processes below it, not even once that window is gone: an elevated
// process arming the shell's mask says the keys are down (kMaskOnWord), and
// the mask takes its word for it - until a release has been masked, here or
// there, or for no longer than a press is ever held.
constexpr WPARAM kMaskOnWord = 4;
constexpr DWORD kMaskOnWordMaxMs = 10000;
bool g_maskOnWord;
DWORD g_maskOnWordSince;

bool AltKeyDown() {
    return (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
}

// A modifier held through a drag repeats, and every repeat in the window's
// queue keeps the mouse moves on either side of it from merging into one: a
// window slow to resize then takes a step for every move the mouse made, and
// catches up long after it has stopped. Nothing does anything with a repeated
// modifier, so the repeats of the ones a drag is done with go while a move or
// size loop runs in the window in front. A repeat is a press of a key that is
// down already - this hook is called before Windows has taken the press in.
bool IsDragKey(DWORD vk) {
    UINT keepAspect = g_settings.keepAspectVk;
    UINT snap = g_settings.snapModifierVk;
    switch (vk) {
        case VK_LWIN:
        case VK_RWIN:
            return g_settings.dragModifier == DragModifier::Win ||
                   g_settings.leftResize;
        case VK_LMENU:
        case VK_RMENU:
            return g_settings.dragModifier == DragModifier::Alt ||
                   g_settings.leftResize || keepAspect == VK_MENU ||
                   snap == VK_MENU;
        case VK_LCONTROL:
        case VK_RCONTROL:
            return keepAspect == VK_CONTROL || snap == VK_CONTROL;
        case VK_LSHIFT:
        case VK_RSHIFT:
            return keepAspect == VK_SHIFT || snap == VK_SHIFT;
    }
    return false;
}

// The window of the move or size loop running in the window in front, if one
// is.
HWND ForegroundMoveSizeWindow() {
    GUITHREADINFO gui{sizeof(gui)};
    HWND front = GetForegroundWindow();
    if (!front ||
        !GetGUIThreadInfo(GetWindowThreadProcessId(front, nullptr), &gui) ||
        !(gui.flags & GUI_INMOVESIZE)) {
        return nullptr;
    }
    return gui.hwndMoveSize;
}

bool IsModifierRepeatDuringDrag(DWORD vk) {
    return IsDragKey(vk) && (GetAsyncKeyState(vk) & 0x8000) &&
           ((GetAsyncKeyState(VK_LBUTTON) | GetAsyncKeyState(VK_RBUTTON)) &
            0x8000) &&
           ForegroundMoveSizeWindow();
}

// Esc with Alt held is Alt+Esc to Windows - the window sent to the back and
// the next one brought forward - well before a drag gets to see it as the Esc
// that cancels it. A drag with Alt as the modifier holds Alt, and so does a
// resize with Win + Alt. So while a move or size loop runs in the window in
// front with Alt held, the Esc goes to the loop itself, as a plain one, and
// its release goes nowhere. Only ever touched on the server thread.
bool g_escForLoop;

bool HandleEscInAltDrag(bool keyUp) {
    if (keyUp) {
        return std::exchange(g_escForLoop, false);
    }
    if (!AltKeyDown()) {
        return false;
    }
    HWND loop = ForegroundMoveSizeWindow();
    if (!loop || !PostMessageW(loop, WM_KEYDOWN, VK_ESCAPE, 1)) {
        return false;
    }
    g_escForLoop = true;
    return true;
}

LRESULT CALLBACK LowLevelKeyboardProc(int code, WPARAM wParam, LPARAM lParam) {
    ModRef ref;  // the image must not go away under this procedure

    if (code != HC_ACTION || g_uninitializing) {
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }
    auto* info = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
    bool keyUp = wParam == WM_KEYUP || wParam == WM_SYSKEYUP;
    bool ours = info->dwExtraInfo == kInjectedMarker;
    if (!ours && !keyUp && IsModifierRepeatDuringDrag(info->vkCode)) {
        return 1;
    }
    if (!ours && info->vkCode == VK_ESCAPE && HandleEscInAltDrag(keyUp)) {
        return 1;
    }
    if (!ours && !keyUp) {
        BringDesktopForwardForKey(info->vkCode);
    }
    if (!ours && HandleBindingKey(info->vkCode, !keyUp)) {
        return 1;
    }
    bool isWin = info->vkCode == VK_LWIN || info->vkCode == VK_RWIN;
    bool isModifier = g_settings.dragModifier == DragModifier::Win
                          ? isWin
                          : info->vkCode == VK_LMENU || info->vkCode == VK_RMENU;
    if (keyUp && isModifier) {
        EndWheelTurn();  // the next turn is for the window it starts over
    }
    bool isAlt = info->vkCode == VK_LMENU || info->vkCode == VK_RMENU ||
                 info->vkCode == VK_MENU;
    if (g_winMaskArmed && keyUp &&
        ((isWin && g_maskWin) || (isAlt && g_maskAlt))) {
        // An injected release is another mask's - armed in some other
        // process for the same press - that got to it first. Standing down
        // with it, rather than staying armed, keeps this one from eating the
        // next tap.
        (isWin ? g_maskWin : g_maskAlt) = false;
        g_winMaskArmed = g_maskWin || g_maskAlt;
        if (!ours) {
            INPUT input[3]{};
            input[0].type = INPUT_KEYBOARD;
            input[0].ki.wVk = kMaskVk;
            input[0].ki.dwExtraInfo = kInjectedMarker;
            input[1] = input[0];
            input[1].ki.dwFlags = KEYEVENTF_KEYUP;
            input[2].type = INPUT_KEYBOARD;
            input[2].ki.wVk = (WORD)info->vkCode;
            input[2].ki.dwFlags = KEYEVENTF_KEYUP;
            input[2].ki.dwExtraInfo = kInjectedMarker;
            SendInput(ARRAYSIZE(input), input, sizeof(INPUT));
            // The shell's mask, armed on this process's word, never sees
            // this release go by: it is told.
            if (IsElevatedProcess()) {
                HWND shell = FindKeyboardServer();
                if (shell && shell != g_keyboardServer) {
                    PostMessageW(shell, kMaskStandDown,
                                 isWin ? kMaskForWin : kMaskForAlt, 0);
                }
            }
            return 1;  // swallow the physical key-up
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////
// The server thread

// In place while there is a mask armed or a reason to keep it, and not a
// moment longer.
void UpdateKeyboardHook() {
    bool wanted = !g_uninitializing && (g_keepKeyboardHook || g_winMaskArmed);
    if (wanted && !g_keyboardHook) {
        // A low-level hook procedure is called in the thread that installed
        // it, so no module handle is needed.
        g_keyboardHook =
            SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, nullptr, 0);
        if (!g_keyboardHook) {
            Wh_Log(L"WH_KEYBOARD_LL hook failed (%u)", GetLastError());
        }
    } else if (!wanted && g_keyboardHook) {
        UnhookWindowsHookEx(g_keyboardHook);
        g_keyboardHook = nullptr;
    }

    // And the mouse hook (mouse.cpp) where the keyboard one stays for good:
    // the shell, and an elevated process while a window of its own is in
    // front - the presses on its windows are kept from the shell's hook. One
    // with content the message hook cannot see keeps it in the background
    // too, so that a press there finds it. Not every elevated process at all
    // times: each low-level hook is a stop every move of the mouse makes.
    bool mouseWanted =
        !g_uninitializing &&
        (g_keepKeyboardHook ||
         (IsElevatedProcess() && HasCompositionContentWindow()));
    if (mouseWanted && !g_mouseHook) {
        g_mouseHook =
            SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, nullptr, 0);
        if (!g_mouseHook) {
            Wh_Log(L"WH_MOUSE_LL hook failed (%u)", GetLastError());
        }
    } else if (!mouseWanted && g_mouseHook) {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = nullptr;
    }
}

void DisarmMask(HWND hwnd) {
    g_winMaskArmed = false;
    g_maskWin = g_maskAlt = false;
    g_maskOnWord = false;
    KillTimer(hwnd, kMaskTimerId);
    UpdateKeyboardHook();
}

void ArmMask(HWND hwnd, WPARAM keys) {
    // A press that is already over gets none: a mask armed for it now would
    // be left for the next one.
    bool onWord = (keys & kMaskOnWord) != 0;
    g_maskWin |= (keys & kMaskForWin) && (onWord || WinKeyDown());
    g_maskAlt |= (keys & kMaskForAlt) && (onWord || AltKeyDown());
    if (g_uninitializing || !(g_maskWin || g_maskAlt)) {
        g_maskWin = g_maskAlt = false;
        return;
    }
    if (onWord) {
        g_maskOnWord = true;
        g_maskOnWordSince = GetTickCount();
    }
    g_winMaskArmed = true;
    UpdateKeyboardHook();
    if (!g_keyboardHook) {
        g_winMaskArmed = false;
        return;
    }
    SetTimer(hwnd, kMaskTimerId, kMaskPollMs, nullptr);
}

// What the mask sends ahead of a release, sent by itself: for a key binding
// the hook took, whose Win or Alt is then no lone tap.
void MaskModifierTap() {
    INPUT input[2]{};
    input[0].type = INPUT_KEYBOARD;
    input[0].ki.wVk = kMaskVk;
    input[0].ki.dwExtraInfo = kInjectedMarker;
    input[1] = input[0];
    input[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(ARRAYSIZE(input), input, sizeof(INPUT));
}

// An elevated process keeps the hook while a window of its own is in front,
// which is when the shell's hook is blind.
void FollowForeground(HWND foreground) {
    DWORD pid = 0;
    GetWindowThreadProcessId(foreground, &pid);
    g_keepKeyboardHook = pid == GetCurrentProcessId();
    UpdateKeyboardHook();
}

void CALLBACK OnForegroundChanged(HWINEVENTHOOK, DWORD, HWND hwnd, LONG, LONG,
                                  DWORD, DWORD) {
    FollowForeground(hwnd);
}

LRESULT CALLBACK KeyboardServerProc(HWND hwnd, UINT msg, WPARAM wParam,
                                LPARAM lParam) {
    switch (msg) {
        case kMaskArm:
            ArmMask(hwnd, wParam);
            return 0;
        case WM_TIMER:
            // The hook takes the mask down when it masks a release. A release
            // can also go by without reaching it at all, and a mask that
            // outlived its press would swallow the next one - so the key being
            // up is what ends it - or, armed on an elevated process's word, a
            // release masked there, or the longest a press is ever held.
            if (g_maskOnWord &&
                GetTickCount() - g_maskOnWordSince > kMaskOnWordMaxMs) {
                g_maskOnWord = false;
            }
            g_maskWin = g_maskWin && (g_maskOnWord || WinKeyDown());
            g_maskAlt = g_maskAlt && (g_maskOnWord || AltKeyDown());
            if (!g_winMaskArmed || !(g_maskWin || g_maskAlt)) {
                DisarmMask(hwnd);
            }
            return 0;
        case kMaskStandDown:
            if (g_maskOnWord) {
                g_maskWin = g_maskWin && !(wParam & kMaskForWin);
                g_maskAlt = g_maskAlt && !(wParam & kMaskForAlt);
                if (!(g_maskWin || g_maskAlt)) {
                    DisarmMask(hwnd);
                }
            }
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

HINSTANCE ThisModule() {
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&KeyboardServerProc), &module);
    return module;
}

DWORD WINAPI KeyboardServerThread(LPVOID param) {
    // The queue first. The shutdown posts WM_QUIT here, and a post to a thread
    // that has no queue yet is lost: the loop below would wait for good.
    MSG msg;
    PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE);

    HANDLE ready = param;
    // Registered against this image rather than the process: two copies of
    // the mod can be loaded side by side while one replaces the other, and a
    // window of this one must never end up running the other's procedure,
    // which goes away when that copy does. The class name is the same for
    // both, which is what lets a gesture find whichever server is up.
    HINSTANCE instance = ThisModule();
    WNDCLASSW wc{};
    wc.lpfnWndProc = KeyboardServerProc;
    wc.hInstance = instance;
    wc.lpszClassName = kKeyboardServerClass;
    RegisterClassW(&wc);
    HWND hwnd = CreateWindowExW(0, kKeyboardServerClass, nullptr, 0, 0, 0, 0, 0,
                                HWND_MESSAGE, nullptr, instance, nullptr);
    if (hwnd) {
        // Gestures in processes of any integrity level ask for a mask here.
        ChangeWindowMessageFilterEx(hwnd, kMaskArm, MSGFLT_ALLOW, nullptr);
        ChangeWindowMessageFilterEx(hwnd, kMaskStandDown, MSGFLT_ALLOW,
                                    nullptr);
    }
    {
        std::lock_guard<std::mutex> lock(g_serverMutex);
        g_keyboardServer = hwnd;
    }
    SetEvent(ready);

    if (hwnd) {
        if (IsShellProcess()) {
            g_keepKeyboardHook = true;
            UpdateKeyboardHook();
        } else if (IsElevatedProcess()) {
            // Only the foreground changing, delivered to this thread's queue:
            // Windows does not wait for it, and nothing else comes of it.
            g_foregroundHook = SetWinEventHook(
                EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
                OnForegroundChanged, 0, 0, WINEVENT_OUTOFCONTEXT);
            FollowForeground(GetForegroundWindow());
        }
        // A shutdown from before the queue was there is seen here instead: it
        // sets the flag before it posts.
        while (!g_uninitializing && GetMessageW(&msg, nullptr, 0, 0) > 0) {
            DispatchMessageW(&msg);
        }
        if (g_foregroundHook) {
            UnhookWinEvent(g_foregroundHook);
            g_foregroundHook = nullptr;
        }
        g_keepKeyboardHook = false;
        DisarmMask(hwnd);  // which takes the hook down with it
        DestroyWindow(hwnd);
    }
    // The class's procedure is in this image, so it has to go with it.
    UnregisterClassW(kKeyboardServerClass, instance);
    {
        std::lock_guard<std::mutex> lock(g_serverMutex);
        g_keyboardServer = nullptr;
        g_serverThreadId = 0;
    }
    g_modRefCount--;  // the last thing this thread does in the mod's image
    return 0;
}

// This process's server, started if it is not running yet. Waits for its
// window, since the caller is about to post to it.
HWND StartServer() {
    std::unique_lock<std::mutex> lock(g_serverMutex);
    if (g_keyboardServer || g_serverThreadId || g_uninitializing) {
        return g_keyboardServer;  // up, or on its way up
    }
    HANDLE ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!ready) {
        return nullptr;
    }
    if (!StartModThread(KeyboardServerThread, ready, &g_serverThreadId)) {
        CloseHandle(ready);
        return nullptr;
    }
    // The thread takes the lock to publish its window, so it is let go of
    // while waiting.
    lock.unlock();
    WaitForSingleObject(ready, kServerStartWaitMs);
    lock.lock();
    CloseHandle(ready);
    return g_keyboardServer;
}

// The server that should take this press: the one in the shell's own process
// when there is one. Explorer can run a second process for folder windows,
// and that one goes away with them.
HWND FindKeyboardServer() {
    DWORD shellPid = 0;
    if (HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr)) {
        GetWindowThreadProcessId(tray, &shellPid);
    }
    HWND any = nullptr;
    HWND found = nullptr;
    while ((found = FindWindowExW(HWND_MESSAGE, found, kKeyboardServerClass,
                                  nullptr))) {
        DWORD pid = 0;
        GetWindowThreadProcessId(found, &pid);
        if (shellPid && pid == shellPid) {
            return found;
        }
        if (!any) {
            any = found;
        }
    }
    return any;
}

// The shell's own Explorer. With folder windows in a separate process, other
// copies of explorer.exe run them, and those must not take the shell's part: a
// second permanent keyboard hook, and hotkeys only the shell gets. The shell's
// copy is the one that owns the shell window, or the one starting up before
// there is any, as after an Explorer restart. Decided once, because that
// window comes and goes with the shell.
bool IsShellProcess() {
    static const bool shell = [] {
        if (_wcsicmp(ThisProgramName().c_str(), L"explorer.exe") != 0) {
            return false;
        }
        HWND shellWindow = GetShellWindow();
        DWORD owner = 0;
        return !shellWindow ||
               (GetWindowThreadProcessId(shellWindow, &owner) &&
                owner == GetCurrentProcessId());
    }();
    return shell;
}

BOOL CALLBACK FindOwnWindowProc(HWND hwnd, LPARAM lParam) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == GetCurrentProcessId()) {
        *reinterpret_cast<bool*>(lParam) = true;
        return FALSE;
    }
    return TRUE;
}

// Called from Wh_ModAfterInit: the shell has its server up from the start, so
// that it is there to be found before anybody needs it, and its hook sees the
// key bindings. So does an elevated process that has windows already.
void StartKeyboardServer() {
    bool hasWindows = false;
    if (!IsShellProcess() && IsElevatedProcess()) {
        EnumWindows(FindOwnWindowProc, (LPARAM)&hasWindows);
    }
    if (IsShellProcess() || hasWindows) {
        StartServer();
    }
}

// Called for every top-level window a process creates: the one that gives an
// elevated process a window to follow. Processes without one - services and
// the like - never run a server at all.
void StartKeyboardServerForWindow() {
    if (IsElevatedProcess()) {
        StartServer();
    }
}

bool IsElevatedProcess() {
    static const bool elevated = [] {
        HANDLE token;
        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
            return false;
        }
        TOKEN_ELEVATION elevation{};
        DWORD len = 0;
        bool result = GetTokenInformation(token, TokenElevation, &elevation,
                                          sizeof(elevation), &len) &&
                      elevation.TokenIsElevated;
        CloseHandle(token);
        return result;
    }();
    return elevated;
}

// Called by whatever just consumed a Win + mouse gesture; `usingWin` and
// `usingAlt` are that gesture's own answer to which of the keys are part of
// it, because the modifier is not the same setting for every one of them.
void ArmWinMask(bool usingWin, bool usingAlt) {
    WPARAM keys = (usingWin && WinKeyDown() ? kMaskForWin : 0) |
                  (usingAlt && AltKeyDown() ? kMaskForAlt : 0);
    if (!keys || g_uninitializing) {
        return;
    }
    HWND server = FindKeyboardServer();
    if (!server) {
        server = StartServer();
    }
    // Windows keeps the keys typed into an elevated window - Task Manager,
    // anything run as administrator - from the low-level hooks of the
    // processes below it, the shell's included. Such a process arms a mask
    // of its own as well, which sees them. The shell's is armed too, on this
    // process's word that the keys are down, since it cannot tell: if the
    // gesture closed the process's last window, the release goes to whatever
    // window comes forward next.
    HWND own = IsElevatedProcess() ? StartServer() : nullptr;
    WPARAM word = own && own != server ? kMaskOnWord : 0;
    if (!server || !PostMessageW(server, kMaskArm, keys | word, 0)) {
        Wh_Log(L"No Start menu mask to arm (%u)", GetLastError());
    }
    if (own && own != server) {
        PostMessageW(own, kMaskArm, keys, 0);
    }
}

// Tear the suppression down: this process's server, if it has one, and with
// it the hook it may still hold. The thread keeps a reference on the image
// until it is out, which Wh_ModUninit waits for.
void ShutdownKeyboardServer() {
    g_winMaskArmed = false;
    std::lock_guard<std::mutex> lock(g_serverMutex);
    if (g_serverThreadId) {
        PostThreadMessageW(g_serverThreadId, WM_QUIT, 0, 0);
    }
}

////////////////////////////////////////////////////////////////////////////////
// src/desktops.cpp
////////////////////////////////////////////////////////////////////////////////
// Win+Tab through the virtual desktops, the way Alt+Tab goes through windows:
// Win+Tab goes to the next desktop that has windows on it, Win+Shift+Tab to
// the previous one, round and round - at once, on every press. Task View, which
// Win+Tab opens otherwise, is on Win+Ctrl+Tab instead.
//
// It all happens in the shell, on a thread of the mod's own. Win+Tab is a
// hotkey the shell holds for Task View; the shell's thread it is posted to
// hands it over to this one instead (hooks.cpp), and this thread holds the
// other two. A hotkey reaches the shell whichever window is in front, one run
// as administrator included, and Windows keeps the Start menu shut after one.
//
// Switching at once takes the shell's own desktop manager, which Windows does
// not publish: its interface IDs and layout are known for the builds below
// (from VirtualDesktopAccessor) and change with new ones. Which one a build
// has is asked of the shell rather than worked out from the version. On a
// build none of them fits, Win+Tab is left to Windows.
//
// What the shell does after a switch is where the care goes. It brings forward
// the window last used on the desktop it went to, and whenever a window comes
// forward, it makes the desktop that window is on the current one. Both come a
// while later - a few hundred milliseconds, when the shell is busy, as it is
// right after a switch - and by then the next press may have gone on to
// another desktop: the shell then takes it back, and the desktops flicker back
// and forth. So while the presses come, the front is held by a window of the
// mod's own that belongs to no desktop: nothing is brought forward, and there
// is nothing to follow. The window on top of the desktop the presses ended on
// comes forward with the first key that is not for switching, or a while after
// the last one, and a press right after that waits for the shell to have taken
// it in.
//
// Task View and the Alt+Tab switcher are left alone while they are up: a
// Win+Tab then goes to Windows, which closes Task View for it. With no other
// desktop that has windows, Win+Tab goes to an empty one - made first, when
// there is only the one - the way Hyprland makes a workspace by going to it;
// or, as the settings may have it, opens Task View, as in Windows.

// Set by the desktop thread once the shell has said it has no desktop manager
// the mod knows.
std::atomic<bool> g_desktopSwitchUnsupported;

////////////////////////////////////////////////////////////////////////////////
// The desktops

const CLSID CLSID_ImmersiveShell = {
    0xC2F03A33, 0x21F5, 0x47FA, {0xB4, 0xBB, 0x15, 0x63, 0x62, 0xA2, 0xF2, 0x39}};
const CLSID CLSID_VirtualDesktopManagerInternal = {
    0xC5E0CDCA, 0x7B6E, 0x41B2, {0x9F, 0xC4, 0xD9, 0x39, 0x75, 0xCC, 0x46, 0x7B}};

// The builds whose desktop manager has GetCurrentDesktop, GetDesktops and
// SwitchDesktop in slots 6, 7 and 9, and a desktop with GetId in slot 4.
// CreateDesktop comes after SwitchDesktop, one further on where 24H2 has put
// SwitchDesktopAndMoveForegroundView in between. The Windows 11 21H2 and
// Server 2022 ones take a monitor as well, and are left to Windows.
struct DesktopManagerLayout {
    IID manager;
    IID desktop;
    int createSlot;
};
const DesktopManagerLayout kLayouts[] = {
    // Windows 11 24H2 and later
    {{0x53F5CA0B, 0x158F, 0x4124, {0x90, 0x0C, 0x05, 0x71, 0x58, 0x06, 0x0B, 0x27}},
     {0x3F07F4BE, 0xB107, 0x441A, {0xAF, 0x0F, 0x39, 0xD8, 0x25, 0x29, 0x07, 0x2C}},
     11},
    // Windows 11 22H2 and 23H2
    {{0xA3175F2D, 0x239C, 0x4BD2, {0x8A, 0xA0, 0xEE, 0xBA, 0x8B, 0x0B, 0x13, 0x8E}},
     {0x3F07F4BE, 0xB107, 0x441A, {0xAF, 0x0F, 0x39, 0xD8, 0x25, 0x29, 0x07, 0x2C}},
     10},
    // Windows 10
    {{0xF31574D6, 0xB682, 0x4CDC, {0xBD, 0x56, 0x18, 0x27, 0x86, 0x0A, 0xBE, 0xC6}},
     {0xFF72FFDD, 0xBE7E, 0x43FC, {0x9C, 0x03, 0xAD, 0x81, 0x68, 0x1E, 0x88, 0xE4}},
     10},
};
constexpr int kGetIdSlot = 4;
constexpr int kGetCurrentDesktopSlot = 6;
constexpr int kGetDesktopsSlot = 7;
constexpr int kSwitchDesktopSlot = 9;

template <typename Method>
Method Slot(void* object, int index) {
    return reinterpret_cast<Method>((*reinterpret_cast<void***>(object))[index]);
}

// Only touched on the desktop thread.
IVirtualDesktopManager* g_desktopManager;  // the public one
IUnknown* g_internalManager;
const DesktopManagerLayout* g_layout;

void ReleaseDesktopManagers() {
    if (g_internalManager) {
        g_internalManager->Release();
        g_internalManager = nullptr;
    }
    if (g_desktopManager) {
        g_desktopManager->Release();
        g_desktopManager = nullptr;
    }
    g_layout = nullptr;
}

bool ConnectDesktopManagers() {
    if (g_internalManager && g_desktopManager) {
        return true;
    }
    ReleaseDesktopManagers();
    CoCreateInstance(CLSID_VirtualDesktopManager, nullptr, CLSCTX_ALL,
                     IID_PPV_ARGS(&g_desktopManager));
    IServiceProvider* shell = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_ImmersiveShell, nullptr, CLSCTX_ALL,
                                  IID_PPV_ARGS(&shell));
    if (FAILED(hr) || !g_desktopManager) {
        // The shell may still be starting up: tried again on the next press.
        Wh_Log(L"No immersive shell yet (0x%08X)", hr);
        ReleaseDesktopManagers();
        return false;
    }
    for (const DesktopManagerLayout& layout : kLayouts) {
        if (SUCCEEDED(shell->QueryService(CLSID_VirtualDesktopManagerInternal,
                                          layout.manager,
                                          (void**)&g_internalManager))) {
            g_layout = &layout;
            break;
        }
    }
    shell->Release();
    if (!g_layout) {
        Wh_Log(L"Unknown desktop manager: Win+Tab is left to Windows");
        g_desktopSwitchUnsupported = true;
        ReleaseDesktopManagers();
        return false;
    }
    return true;
}

GUID DesktopIdOf(IUnknown* desktop) {
    GUID id{};
    using GetId = HRESULT(STDMETHODCALLTYPE*)(void*, GUID*);
    Slot<GetId>(desktop, kGetIdSlot)(desktop, &id);
    return id;
}

struct Desktops {
    std::vector<GUID> ids;
    std::vector<IUnknown*> objects;
    GUID current{};
    ~Desktops() {
        for (IUnknown* object : objects) {
            object->Release();
        }
    }
};

bool ReadDesktops(Desktops* desktops) {
    using GetDesktops = HRESULT(STDMETHODCALLTYPE*)(void*, IObjectArray**);
    using GetCurrent = HRESULT(STDMETHODCALLTYPE*)(void*, IUnknown**);
    IObjectArray* array = nullptr;
    if (FAILED(Slot<GetDesktops>(g_internalManager, kGetDesktopsSlot)(
            g_internalManager, &array))) {
        return false;
    }
    UINT count = 0;
    array->GetCount(&count);
    for (UINT i = 0; i < count; i++) {
        IUnknown* desktop = nullptr;
        if (SUCCEEDED(array->GetAt(i, g_layout->desktop, (void**)&desktop))) {
            desktops->ids.push_back(DesktopIdOf(desktop));
            desktops->objects.push_back(desktop);
        }
    }
    array->Release();
    IUnknown* current = nullptr;
    if (FAILED(Slot<GetCurrent>(g_internalManager, kGetCurrentDesktopSlot)(
            g_internalManager, &current)) ||
        !current) {
        return false;
    }
    desktops->current = DesktopIdOf(current);
    current->Release();
    return !desktops->ids.empty();
}

// The desktops with a window of their own on them.
BOOL CALLBACK CollectDesktopProc(HWND hwnd, LPARAM lParam) {
    auto* withWindows = reinterpret_cast<std::vector<GUID>*>(lParam);
    if (!IsWindowVisible(hwnd) || GetWindow(hwnd, GW_OWNER) ||
        (GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) ||
        !IsFrameWindow(hwnd)) {
        return TRUE;
    }
    // A window pinned to every desktop answers with an ID that is none of
    // theirs, and so counts for none of them.
    GUID desktop{};
    if (SUCCEEDED(g_desktopManager->GetWindowDesktopId(hwnd, &desktop)) &&
        desktop != GUID_NULL &&
        std::find(withWindows->begin(), withWindows->end(), desktop) ==
            withWindows->end()) {
        withWindows->push_back(desktop);
    }
    return TRUE;
}

// One of the shell's XAML views you work in - Task View, the Alt+Tab
// switcher - is up, or coming up. The shell makes the window when it opens one
// and destroys it once it has closed, so this holds for the closing animation
// as well; while it opens, the window is in front a moment before it is shown.
// The little one that shows a desktop's name after every switch is a view
// too, but one that never takes the focus, and it is not what this is about.
// They are looked for by class. EnumWindows passes over them altogether, and
// FindWindowEx over Task View once it is up, in front - so the window in front
// is asked first. Windows 10's are of a class of their own.
constexpr PCWSTR kShellViewClasses[] = {L"XamlExplorerHostIslandWindow",
                                        L"MultitaskingViewFrame"};

bool IsShellViewClass(HWND hwnd) {
    WCHAR cls[64];
    if (!hwnd || !GetClassNameW(hwnd, cls, ARRAYSIZE(cls))) {
        return false;
    }
    for (PCWSTR viewClass : kShellViewClasses) {
        if (_wcsicmp(cls, viewClass) == 0) {
            return true;
        }
    }
    return false;
}

bool ShellViewUp() {
    if (IsShellViewClass(GetForegroundWindow())) {
        return true;
    }
    for (PCWSTR viewClass : kShellViewClasses) {
        HWND view = nullptr;
        while ((view = FindWindowExW(nullptr, view, viewClass, nullptr))) {
            if (IsWindowVisible(view) &&
                !(GetWindowLongPtrW(view, GWL_EXSTYLE) & WS_EX_NOACTIVATE)) {
                return true;
            }
        }
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
// The desktop thread, in the shell

// The window that holds the front while the presses come: a tool window,
// which no desktop has for its own, out of sight.
//
// The window on top of the desktop the presses ended on comes forward with the
// first key that is not for switching - before that key gets anywhere, so it
// goes to that window - or by itself once Win is up and this long has gone by
// since the last switch. Not sooner: a window that comes forward between two
// presses is one the shell may take the desktops back to.
constexpr DWORD kBringForwardAfterMs = 1000;
constexpr UINT kBringForwardPollMs = 15;
constexpr UINT_PTR kBringForwardTimer = 1;
// And a switch waits for the front to have stayed put this long: the shell
// takes in every window that comes forward a while later, and a switch made
// before it has is undone by it. While the mod holds the front, it stays put.
constexpr DWORD kQuietFrontMs = 200;
// A window that has just lost the front to the holder can take it back - File
// Explorer's do, at once, and not only theirs - so the holder is watched this
// long before a switch.
constexpr DWORD kHoldWatchMs = 50;
constexpr int kHoldAttempts = 3;

constexpr UINT kDesktopStepMessage = WM_APP + 2;      // wParam: +1 or -1
constexpr UINT kDesktopSettingsMessage = WM_APP + 3;  // the settings changed
constexpr int kPreviousHotkey = 1;                    // Win+Shift+Tab
constexpr int kTaskViewHotkey = 2;                    // Win+Ctrl+Tab

std::mutex g_desktopThreadMutex;
std::atomic<DWORD> g_desktopThreadId;
constexpr DWORD kDesktopThreadStartWaitMs = 2000;

// Made and destroyed on the desktop thread, and looked at by the keyboard
// thread as well.
std::atomic<HWND> g_frontHolder;

// Only touched on the desktop thread.
bool g_hotkeysRegistered;
bool g_holdingFront;  // the holder has the front on purpose
bool g_bringForwardPending;
DWORD g_switchedAt;
HWINEVENTHOOK g_frontChangeHook;
DWORD g_frontChangedAt;  // last time a window other than the holder came forward

// The window the shell would bring forward on the current desktop: the
// topmost one there that takes the focus.
BOOL CALLBACK FindTopWindowProc(HWND hwnd, LPARAM lParam) {
    DWORD cloaked = 0;
    DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
    if (hwnd == g_frontHolder || cloaked || !IsWindowVisible(hwnd) ||
        IsIconic(hwnd) || !IsWindowEnabled(hwnd) ||
        (GetWindowLongPtrW(hwnd, GWL_EXSTYLE) &
         (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) ||
        !IsFrameWindow(hwnd)) {
        return TRUE;
    }
    *reinterpret_cast<HWND*>(lParam) = hwnd;
    return FALSE;
}

// Once a window has the front again, the holder goes to the bottom: when the
// window in front closes, Windows hands the front to the one under it, and
// that is not to be the holder.
void StopBringingForward() {
    g_holdingFront = false;
    g_bringForwardPending = false;
    if (HWND holder = g_frontHolder) {
        KillTimer(holder, kBringForwardTimer);
        if (GetForegroundWindow() != holder) {
            SetWindowPos(holder, HWND_BOTTOM, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        }
    }
}

void BringTopWindowForward() {
    HWND top = nullptr;
    EnumWindows(FindTopWindowProc, (LPARAM)&top);
    // A desktop with no window of that kind has its wallpaper in front.
    SetForegroundWindow(top ? top : GetShellWindow());
}

void BringForward() {
    BringTopWindowForward();
    StopBringingForward();
}

// On the timer, while a window is to come forward.
void OnBringForwardTimer() {
    if (!g_bringForwardPending || GetForegroundWindow() != g_frontHolder) {
        // Someone has brought something forward meanwhile: the user, with a
        // click. That is the window for this desktop, then.
        StopBringingForward();
        return;
    }
    if (WinKeyDown() || GetTickCount() - g_switchedAt < kBringForwardAfterMs) {
        return;
    }
    BringForward();
}

void CALLBACK OnFrontChanged(HWINEVENTHOOK, DWORD, HWND hwnd, LONG, LONG, DWORD,
                             DWORD) {
    if (hwnd != g_frontHolder) {
        g_frontChangedAt = GetTickCount();
    } else if (!g_holdingFront) {
        // The holder came forward by itself - handed the front of a window
        // that closed: it goes on to the window that should have it.
        BringForward();
    }
}

// Waits, hearing of the front's changes meanwhile - the event is delivered by
// looking at the queue - until `done` says so or `ms` have gone by.
template <typename Done>
void WaitHearingFront(DWORD ms, Done done) {
    DWORD start = GetTickCount();
    for (;;) {
        MSG msg;
        PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE);
        DWORD spent = GetTickCount() - start;
        if (done() || spent >= ms) {
            return;
        }
        MsgWaitForMultipleObjects(0, nullptr, FALSE, ms - spent, QS_ALLINPUT);
    }
}

bool FrontQuiet() {
    return !g_frontChangedAt || GetTickCount() - g_frontChangedAt >= kQuietFrontMs;
}

// The front held by the holder, and quiet: what a switch needs.
bool HoldFront() {
    if (!g_frontHolder || !IsWindow(g_frontHolder)) {
        g_frontHolder = CreateWindowExW(
            WS_EX_TOOLWINDOW, L"STATIC", nullptr, WS_POPUP | WS_VISIBLE, -32000,
            -32000, 1, 1, nullptr, nullptr, nullptr, nullptr);
    }
    g_holdingFront = true;
    for (int attempt = 0; attempt < kHoldAttempts; attempt++) {
        WaitHearingFront(kQuietFrontMs, FrontQuiet);
        if (GetForegroundWindow() == g_frontHolder) {
            return true;
        }
        if (!g_frontHolder || !SetForegroundWindow(g_frontHolder)) {
            return false;
        }
        WaitHearingFront(kHoldWatchMs, [] {
            return GetForegroundWindow() != g_frontHolder;
        });
        if (GetForegroundWindow() == g_frontHolder) {
            return true;
        }
    }
    return false;
}

enum class Target { kNone, kNowhere, kFound };

// The desktop `steps` stops round from the current one, among those that have
// windows on them (and the current one, which is where the counting starts).
Target FindTarget(int steps, Desktops* desktops, int* target) {
    if (!ReadDesktops(desktops)) {
        // The shell may have been restarted under us: connect again.
        ReleaseDesktopManagers();
        if (!ConnectDesktopManagers() || !ReadDesktops(desktops)) {
            return Target::kNone;
        }
    }
    std::vector<GUID> withWindows;
    EnumWindows(CollectDesktopProc, (LPARAM)&withWindows);

    std::vector<int> stops;
    int here = -1;
    for (int i = 0; i < (int)desktops->ids.size(); i++) {
        bool current = desktops->ids[i] == desktops->current;
        if (current || std::find(withWindows.begin(), withWindows.end(),
                                 desktops->ids[i]) != withWindows.end()) {
            if (current) {
                here = (int)stops.size();
            }
            stops.push_back(i);
        }
    }
    if (here < 0) {
        return Target::kNone;
    }
    if (stops.size() < 2) {
        return Target::kNowhere;
    }
    int count = (int)stops.size();
    *target = stops[((here + steps) % count + count) % count];
    return *target != stops[here] ? Target::kFound : Target::kNone;
}

// With nowhere to go: the desktop next to this one, `steps` stops round,
// windows or not - or, with only the one there is, a new one, at the end.
bool FindEmptyTarget(int steps, Desktops* desktops, int* target) {
    int count = (int)desktops->ids.size();
    if (count > 1) {
        int here = (int)(std::find(desktops->ids.begin(), desktops->ids.end(),
                                   desktops->current) -
                         desktops->ids.begin());
        *target = ((here + steps) % count + count) % count;
        return true;
    }
    using Create = HRESULT(STDMETHODCALLTYPE*)(void*, IUnknown**);
    IUnknown* made = nullptr;
    HRESULT hr = Slot<Create>(g_internalManager, g_layout->createSlot)(
        g_internalManager, &made);
    if (FAILED(hr) || !made) {
        Wh_Log(L"CreateDesktop failed (0x%08X)", hr);
        return false;
    }
    desktops->ids.push_back(DesktopIdOf(made));
    desktops->objects.push_back(made);
    *target = count;
    return true;
}

// Task View, opened - or closed, when it is up - the way its shortcut does it.
void ToggleTaskView() {
    ShellExecuteW(nullptr, L"open",
                  L"shell:::{3080F90E-D7AD-11D9-BD98-0000947B0257}", nullptr,
                  nullptr, SW_SHOWNORMAL);
}

void StepDesktop(int steps) {
    if (ShellViewUp() || !ConnectDesktopManagers()) {
        return;
    }
    Desktops desktops;
    int target = -1;
    Target found = FindTarget(steps, &desktops, &target);
    if (found == Target::kNowhere) {
        // No other desktop has windows: an empty one, or Win+Tab is what it
        // is in Windows.
        if (!g_settings.desktopToEmpty) {
            ToggleTaskView();
            return;
        }
        if (!FindEmptyTarget(steps, &desktops, &target)) {
            return;
        }
    } else if (found != Target::kFound) {
        return;
    }
    if (!HoldFront()) {
        Wh_Log(L"Could not hold the front (%u)", GetLastError());
    }
    using Switch = HRESULT(STDMETHODCALLTYPE*)(void*, IUnknown*);
    HRESULT hr = Slot<Switch>(g_internalManager, kSwitchDesktopSlot)(
        g_internalManager, desktops.objects[target]);
    if (FAILED(hr)) {
        Wh_Log(L"SwitchDesktop failed (0x%08X)", hr);
        ReleaseDesktopManagers();
        BringForward();
        return;
    }
    g_switchedAt = GetTickCount();
    g_bringForwardPending = true;
    SetTimer(g_frontHolder, kBringForwardTimer, kBringForwardPollMs, nullptr);
}

// The two hotkeys of the mod's own, held while Win+Tab is the mod's.
void UpdateHotkeys() {
    bool wanted = g_settings.desktopWinTab && !g_desktopSwitchUnsupported &&
                  !g_uninitializing;
    if (wanted && !g_hotkeysRegistered) {
        if (!RegisterHotKey(nullptr, kPreviousHotkey,
                            MOD_WIN | MOD_SHIFT | MOD_NOREPEAT, VK_TAB) ||
            !RegisterHotKey(nullptr, kTaskViewHotkey,
                            MOD_WIN | MOD_CONTROL | MOD_NOREPEAT, VK_TAB)) {
            Wh_Log(L"RegisterHotKey failed (%u)", GetLastError());
        }
        g_hotkeysRegistered = true;
    } else if (!wanted && g_hotkeysRegistered) {
        UnregisterHotKey(nullptr, kPreviousHotkey);
        UnregisterHotKey(nullptr, kTaskViewHotkey);
        g_hotkeysRegistered = false;
    }
}

DWORD WINAPI DesktopThread(LPVOID ready) {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    MSG msg;
    PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE);  // the queue, before anyone posts
    UpdateHotkeys();
    // Only the front changing, delivered to this thread's queue.
    g_frontChangeHook =
        SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
                        OnFrontChanged, 0, 0, WINEVENT_OUTOFCONTEXT);
    SetEvent(static_cast<HANDLE>(ready));
    while (!g_uninitializing && GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (g_uninitializing) {
            continue;
        }
        if (msg.message == kDesktopStepMessage) {
            StepDesktop((int)msg.wParam);
        } else if (msg.message == WM_HOTKEY && !msg.hwnd) {
            if (msg.wParam == kPreviousHotkey) {
                StepDesktop(-1);
            } else if (msg.wParam == kTaskViewHotkey) {
                ToggleTaskView();
            }
        } else if (msg.message == kDesktopSettingsMessage) {
            UpdateHotkeys();
        } else if (msg.message == WM_TIMER && msg.hwnd == g_frontHolder &&
                   msg.wParam == kBringForwardTimer) {
            OnBringForwardTimer();
        } else {
            DispatchMessageW(&msg);
        }
        if (g_desktopSwitchUnsupported) {
            UpdateHotkeys();
        }
    }
    UnregisterHotKey(nullptr, kPreviousHotkey);
    UnregisterHotKey(nullptr, kTaskViewHotkey);
    g_hotkeysRegistered = false;
    if (g_frontChangeHook) {
        UnhookWinEvent(g_frontChangeHook);
        g_frontChangeHook = nullptr;
    }
    if (g_frontHolder) {
        if (GetForegroundWindow() == g_frontHolder) {
            BringForward();
        }
        DestroyWindow(g_frontHolder);
        g_frontHolder = nullptr;
    }
    ReleaseDesktopManagers();
    CoUninitialize();
    g_desktopThreadId = 0;
    g_modRefCount--;  // the last thing this thread does in the mod's image
    return 0;
}

// Called from Wh_ModAfterInit in the shell.
void StartDesktopThread() {
    std::lock_guard<std::mutex> lock(g_desktopThreadMutex);
    if (g_desktopThreadId || g_uninitializing) {
        return;
    }
    HANDLE ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!ready) {
        return;
    }
    DWORD threadId = 0;
    if (StartModThread(DesktopThread, ready, &threadId)) {
        // Published once the thread has its queue: a step posted before that
        // would be lost.
        WaitForSingleObject(ready, kDesktopThreadStartWaitMs);
        g_desktopThreadId = threadId;
    }
    CloseHandle(ready);
}

void DesktopSettingsChanged() {
    if (DWORD threadId = g_desktopThreadId) {
        PostThreadMessageW(threadId, kDesktopSettingsMessage, 0, 0);
    }
}

void ShutdownDesktopThread() {
    if (DWORD threadId = g_desktopThreadId) {
        PostThreadMessageW(threadId, WM_QUIT, 0, 0);
    }
}

// Whether there is more than the one desktop. Asked of the registry, where
// the shell keeps their IDs, rather than of the shell, which the thread asking
// - one of the shell's own - must not wait on.
bool SeveralDesktops() {
    DWORD size = 0;
    return RegGetValueW(HKEY_CURRENT_USER,
                        L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer"
                        L"\\VirtualDesktops",
                        L"VirtualDesktopIDs", RRF_RT_REG_BINARY, nullptr, nullptr,
                        &size) == ERROR_SUCCESS &&
           size > sizeof(GUID);
}

// Called for every WM_HOTKEY a thread of this process retrieves: the shell's
// Win+Tab, posted to one of its own, goes to the desktop thread instead - but
// with only the one desktop, and none to be made, it is left to the shell, as
// Task View.
// Returns true if it was taken.
bool HandleDesktopHotkey(const MSG* msg) {
    DWORD threadId = g_desktopThreadId;
    if (!threadId || LOWORD(msg->lParam) != MOD_WIN ||
        HIWORD(msg->lParam) != VK_TAB || !g_settings.desktopWinTab ||
        g_desktopSwitchUnsupported || ShellViewUp() ||
        !(g_settings.desktopToEmpty || SeveralDesktops())) {
        return false;
    }
    return PostThreadMessageW(threadId, kDesktopStepMessage, 1, 0) != FALSE;
}

// Called by the shell's keyboard thread for every key that goes down. While
// the front is held, a key that is not part of a switch brings the window on
// top of the desktop forward at once - before the key gets anywhere, so that it
// goes to that window rather than to the holder.
void BringDesktopForwardForKey(UINT vk) {
    HWND holder = g_frontHolder;
    if (!holder || GetForegroundWindow() != holder) {
        return;
    }
    switch (vk) {
        case VK_LWIN:
        case VK_RWIN:
        case VK_SHIFT:
        case VK_LSHIFT:
        case VK_RSHIFT:
        case VK_CONTROL:
        case VK_LCONTROL:
        case VK_RCONTROL:
        case VK_MENU:
        case VK_LMENU:
        case VK_RMENU:
            return;  // held for a chord - the next switch, perhaps
        case VK_TAB:
            if (WinKeyDown()) {
                return;  // the next switch itself
            }
            break;
    }
    BringTopWindowForward();
}

////////////////////////////////////////////////////////////////////////////////
// src/drag.cpp
////////////////////////////////////////////////////////////////////////////////
// Win + mouse: moving and resizing windows through the system's own
// move/resize loops.
//
// The mod never runs such a loop itself. A drag is requested by posting
// g_msgDrag to the window that is to be moved or resized, and when that
// window's own thread retrieves the request, the message is rewritten into the
// system command the loop starts from. The loop then runs where a title-bar
// drag would run it - inside the application's DispatchMessage - instead of
// inside the mod's message hook, which matters twice:
//
//   * A drag can last minutes. With the loop below one of our frames, the mod
//     could not be unloaded for as long as it lasts: the image would go away
//     under a thread that still has to return into it.
//   * A request survives crossing a thread or a process boundary, which a
//     WM_SYSCOMMAND posted into another process would not - the UIPI message
//     filter drops that one.
//
// Neither drag touches the global mouse state. The only thing the loops want
// that a Win + mouse drag cannot give them is the left mouse button: they
// track the mouse while it is held and end when it is released. Both of those
// are per-thread rather than global - the button state a loop reads is its own
// thread's synchronized copy, and the release is just a message - so the mod
// fakes the first and posts the second, and no other window ever sees a click
// that wasn't there.

UINT g_msgDrag;  // RegisterWindowMessage, set in Wh_ModInit

bool IsDragModifierDown() {
    return g_settings.dragModifier == DragModifier::Alt ? AltKeyDown()
                                                        : WinKeyDown();
}

// The other one of Win and Alt, held along with the modifier: the left button
// then resizes, for a touchpad, whose right button can be clicked but not held
// down through a drag. Not while Alt is a key of the drag's own already, for
// the shape or the magnet.
bool IsLeftResizeDown() {
    if (!g_settings.leftResize || !IsDragModifierDown()) {
        return false;
    }
    if (g_settings.dragModifier == DragModifier::Alt) {
        return WinKeyDown();
    }
    return g_settings.keepAspectVk != VK_MENU &&
           g_settings.snapModifierVk != VK_MENU && AltKeyDown();
}

// Whether a system move/resize loop is running on a thread.
bool IsThreadInMoveSizeLoop(DWORD threadId) {
    GUITHREADINFO info{sizeof(info)};
    return GetGUIThreadInfo(threadId, &info) && (info.flags & GUI_INMOVESIZE);
}

// The same for this thread. The loop pumps messages, so the mod sees them
// while it runs.
bool IsInMoveSizeLoop() {
    return IsThreadInMoveSizeLoop(GetCurrentThreadId());
}

int PhysicalButtonVk(bool right) {
    bool swapped = GetSystemMetrics(SM_SWAPBUTTON) != 0;
    return (right != swapped) ? VK_RBUTTON : VK_LBUTTON;
}

// A move or size loop only follows the mouse if this thread's synchronized
// button state says the left button is down. For a resize it never is - the
// user is holding the right one - and for a move it isn't either when another
// thread retrieved the press. Without this the loop starts in its keyboard
// mode instead, where it waits for the arrow keys and ignores the mouse.
//
// Nothing lets the button go again in that state afterwards: the release that
// ends a resize is posted, and a posted message does not update it. So the
// thread remembers, and the first message it retrieves after the loop puts
// the button back the way the mouse has it - or GetKeyState would go on
// saying it is down until the next real click.
thread_local bool g_leftButtonForced;

void ForceLeftButtonDown() {
    if (GetKeyState(VK_LBUTTON) < 0) {
        return;
    }
    BYTE keyState[256];
    if (GetKeyboardState(keyState)) {
        keyState[VK_LBUTTON] |= 0x80;
        SetKeyboardState(keyState);
        g_leftButtonForced = true;
    }
}

void ReleaseForcedLeftButton() {
    if (!g_leftButtonForced || IsInMoveSizeLoop()) {
        return;
    }
    g_leftButtonForced = false;
    if (GetAsyncKeyState(PhysicalButtonVk(false)) & 0x8000) {
        return;  // held for real by now
    }
    BYTE keyState[256];
    if (GetKeyboardState(keyState)) {
        keyState[VK_LBUTTON] &= ~0x80;
        SetKeyboardState(keyState);
    }
}

// A move with Alt held is Aero Snap's cue for the other layouts, the ones
// without maximizing at the top edge. With Alt as the modifier it is held
// through every move, so the loop is kept from seeing it: it goes from this
// thread's key state as the move starts, and again after each repeat of the
// key, and comes back as the keyboard has it once the loop is over. Let go of
// and pressed again during the move, it is meant for Aero Snap and stays.
thread_local bool g_altHidden;
// The first messages of a loop come before the thread is marked as in one, so
// "not in a loop" only means "over" once it has been seen running.
thread_local bool g_altLoopSeen;

void SetAltState(bool left, bool right) {
    BYTE keyState[256];
    if (!GetKeyboardState(keyState)) {
        return;
    }
    auto set = [&](int vk, bool down) {
        keyState[vk] = (keyState[vk] & ~0x80) | (down ? 0x80 : 0);
    };
    set(VK_LMENU, left);
    set(VK_RMENU, right);
    set(VK_MENU, left || right);
    SetKeyboardState(keyState);
}

void HideAlt() {
    SetAltState(false, false);
    g_altHidden = true;
    g_altLoopSeen = false;
}

void TrackAltInMove(const MSG* msg) {
    if (!g_altHidden || msg->wParam != VK_MENU) {
        return;
    }
    if (msg->message == WM_KEYUP || msg->message == WM_SYSKEYUP) {
        g_altHidden = false;
    } else if (IsInMoveSizeLoop()) {
        SetAltState(false, false);
    }
}

void RestoreHiddenAlt() {
    if (!g_altHidden) {
        return;
    }
    if (IsInMoveSizeLoop()) {
        g_altLoopSeen = true;
        return;
    }
    if (!g_altLoopSeen) {
        return;
    }
    g_altHidden = g_altLoopSeen = false;
    SetAltState(GetAsyncKeyState(VK_LMENU) & 0x8000,
                GetAsyncKeyState(VK_RMENU) & 0x8000);
}

// Nearest corner to the cursor, as the WMSZ_* code SC_SIZE expects (Hyprland
// resizes from the nearest corner).
UINT ResizeEdgeForPoint(const RECT& rc, POINT pt) {
    bool left = pt.x < (rc.left + rc.right) / 2;
    bool top = pt.y < (rc.top + rc.bottom) / 2;
    if (top) {
        return left ? WMSZ_TOPLEFT : WMSZ_TOPRIGHT;
    }
    return left ? WMSZ_BOTTOMLEFT : WMSZ_BOTTOMRIGHT;
}

void RequestDrag(HWND root, WPARAM kind, POINT pt) {
    // Posted, not sent: the loop has to run on the window's own thread. It
    // also means the request is retrieved before any input still queued, so a
    // button release already on its way is seen by the loop rather than by the
    // application.
    if (!PostMessageW(root, g_msgDrag, kind, MAKELPARAM(pt.x, pt.y))) {
        Wh_Log(L"Drag request for %p failed (%u)", root, GetLastError());
    }
}

////////////////////////////////////////////////////////////////////////////////
// Move

bool StartMove(HWND root, POINT pt, MSG* msg) {
    if (IsIconic(root)) {
        return false;
    }
    if (!(GetAsyncKeyState(PhysicalButtonVk(false)) & 0x8000)) {
        return false;  // released already; the loop would stick to the cursor
    }
    ForceLeftButtonDown();
    if (g_settings.dragModifier == DragModifier::Alt) {
        HideAlt();
    }

    // The system's own caption-drag loop, so the drag gives everything a
    // title-bar drag would: Aero Snap at the screen edges, the Windows 11
    // snap-layouts flyout when the cursor reaches the top, and automatic
    // restore of a maximized window.
    msg->message = WM_SYSCOMMAND;
    msg->wParam = SC_MOVE | HTCAPTION;
    msg->lParam = MAKELPARAM(pt.x, pt.y);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Resize
//
// Started from where the cursor is rather than from the corner, so the grab
// offset is kept and the resize is relative, like Hyprland's. Because this is
// the system's own resize loop, GPU-composited windows (Chrome, Electron)
// reflow live and native menu bars don't flicker - moving the window from the
// outside leaves stale content behind instead (test/chrome_resize_probe.cpp).
//
// The loop ends when the left button is released, which is never going to
// happen here - the user releases the right one - so a thread waits for the
// loop to be running, then for the right button, and then posts the release
// the loop is waiting for.

// Set while a resize of ours is running: the release below is the mod's, and
// if the loop has already ended without it - cancelled with Esc - it is the
// mod's to swallow as well.
thread_local bool g_pendingRelease;

constexpr int kResizePollMs = 8;
constexpr int kResizeLoopPollEvery = 12;  // about a tenth of a second
// A second release 60 ms later is not something anyone can see; a resize that
// stays glued to the cursor is.
constexpr int kReleaseAttempts = 10;
constexpr int kReleaseRetryMs = 60;

struct ResizeRelease {
    HWND root;
    DWORD threadId;  // the one whose loop this release is for
    int rightVk;
};

void PostResizeRelease(const ResizeRelease& release) {
    POINT pt;
    GetCursorPos(&pt);
    PostMessageW(release.root, WM_LBUTTONUP, 0, MAKELPARAM(pt.x, pt.y));
}

DWORD WINAPI ResizeReleaseThread(LPVOID param) {
    {
        std::unique_ptr<ResizeRelease> release(
            static_cast<ResizeRelease*>(param));

        // The loop has to be running before a release is any use. One posted
        // while the loop is still starting up is lost - or is retrieved by
        // the application first, which swallows it as the mod's own - and the
        // loop is then left following the cursor with nothing to end it. A
        // button let go of in the meantime is not: the wait below sees it.
        for (int waited = 0; waited < kMoveSizeStartWaitMs;
             waited += kResizePollMs) {
            if (IsThreadInMoveSizeLoop(release->threadId) || g_uninitializing) {
                break;
            }
            Sleep(kResizePollMs);
        }

        // Then the button, and the loop, whichever ends first: one cancelled
        // with Esc has already put the window back and wants no release. The
        // mod being on its way out counts as the button being let go of - a
        // loop nobody is left to post to would never end. The loop is asked
        // after less often than the button: asking takes a lock the loop
        // itself needs at every step, and a heavy window feels it.
        for (int poll = 1; (GetAsyncKeyState(release->rightVk) & 0x8000) &&
                           !g_uninitializing;
             poll++) {
            if (poll % kResizeLoopPollEvery == 0 &&
                !IsThreadInMoveSizeLoop(release->threadId)) {
                break;
            }
            Sleep(kResizePollMs);
        }

        // The release is then offered until the loop takes it. Once is
        // normally enough, and more is what keeps a loop that did not get it
        // from following the cursor with no button held: the application can
        // have retrieved it first, which the mod answers for by swallowing
        // it. The loop is checked again before every attempt, so at most one
        // release can outlive it, which is the one g_pendingRelease is for.
        for (int attempt = 0; attempt < kReleaseAttempts; attempt++) {
            if (!IsThreadInMoveSizeLoop(release->threadId)) {
                break;
            }
            PostResizeRelease(*release);
            Sleep(kReleaseRetryMs);
        }
    }
    g_modRefCount--;  // the last thing this thread does in the mod's image
    return 0;
}

// The thread owns its state and nothing waits for it, so a resize leaves no
// frame of the mod's on the stack of the window's thread. The reference it
// holds is what keeps the image around for as long as it runs.
bool StartResizeRelease(HWND root) {
    auto* release = new ResizeRelease{root,
                                      GetWindowThreadProcessId(root, nullptr),
                                      PhysicalButtonVk(true)};
    if (!StartModThread(ResizeReleaseThread, release)) {
        delete release;
        return false;
    }
    return true;
}

// With the left button held for it instead, the loop has the button it wants
// and ends on its release, the way a move does: nothing to wait for or post.
bool StartResize(HWND root, POINT pt, MSG* msg, bool byLeft) {
    if (IsIconic(root) || IsZoomed(root)) {
        return false;
    }
    if (!(GetWindowLongPtrW(root, GWL_STYLE) & WS_THICKFRAME)) {
        return false;  // fixed-size window
    }
    RECT rc;
    if (!GetWindowRect(root, &rc)) {
        return false;
    }
    if (!(GetAsyncKeyState(PhysicalButtonVk(!byLeft)) & 0x8000)) {
        return false;  // released already; the loop would stick to the cursor
    }
    if (!byLeft) {
        if (!StartResizeRelease(root)) {
            return false;  // nothing would end the loop
        }
        g_pendingRelease = true;
    }

    ForceLeftButtonDown();
    msg->message = WM_SYSCOMMAND;
    msg->wParam = SC_SIZE | ResizeEdgeForPoint(rc, pt);
    msg->lParam = MAKELPARAM(pt.x, pt.y);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// A click that may yet go to the application
//
// Alt + click means something in plenty of applications, so with Alt the
// press is held on to instead of starting a drag at once: the drag starts once
// the mouse moves, and a release before that hands the application the click
// after all. Win + click means nothing anywhere, so with Win the window is
// grabbed at once - unless the settings say otherwise.

bool ClickGoesToApp() {
    switch (g_settings.modifierClick) {
        case ModifierClick::App:
            return true;
        case ModifierClick::Window:
            return false;
        default:
            return g_settings.dragModifier == DragModifier::Alt;
    }
}

struct HeldPress {
    bool active;
    bool replayed;  // pressed again by the window for the shell's mouse hook
    HWND hwnd;
    UINT message;
    WPARAM wParam;
    LPARAM lParam;
};
thread_local HeldPress g_heldPress[2];  // [0] = left, [1] = right

struct PressWatch {
    HWND root;
    POINT pt;
    WPARAM kind;
    int buttonVk;
};

// Waits for the mouse to move past the drag threshold, and asks for the drag
// then. A release first leaves the press to ReplayHeldPress.
DWORD WINAPI PressWatchThread(LPVOID param) {
    {
        std::unique_ptr<PressWatch> watch(static_cast<PressWatch*>(param));
        int cx = GetSystemMetrics(SM_CXDRAG);
        int cy = GetSystemMetrics(SM_CYDRAG);
        while (!g_uninitializing &&
               (GetAsyncKeyState(watch->buttonVk) & 0x8000)) {
            POINT pt;
            GetCursorPos(&pt);
            if (abs(pt.x - watch->pt.x) > cx || abs(pt.y - watch->pt.y) > cy) {
                RequestDrag(watch->root, watch->kind, watch->pt);
                break;
            }
            Sleep(kResizePollMs);
        }
    }
    g_modRefCount--;  // the last thing this thread does in the mod's image
    return 0;
}

bool HoldPress(HWND root, const MSG* msg, bool right, WPARAM kind) {
    auto* watch = new PressWatch{root, msg->pt, kind, PhysicalButtonVk(right)};
    if (!StartModThread(PressWatchThread, watch)) {
        delete watch;
        return false;
    }
    g_heldPress[right] = {true,
                          GetMessageExtraInfo() == (LPARAM)kReplayMarker,
                          msg->hwnd, msg->message, msg->wParam, msg->lParam};
    return true;
}

// The release comes before any drag did: the press goes to the application
// in its place, and the release is posted to follow it.
bool ReplayHeldPress(MSG* msg, bool right) {
    HeldPress press = g_heldPress[right];
    if (!press.active) {
        return false;
    }
    g_heldPress[right].active = false;

    if (press.replayed) {
        // It went through a hole in content that takes its input past the
        // queue (mouse.cpp), so posting it would miss the content. As input,
        // with the hole closed, it goes where the click was meant to.
        INPUT input[2]{};
        input[0].type = INPUT_MOUSE;
        input[0].mi.dwFlags =
            right ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_LEFTDOWN;
        input[0].mi.dwExtraInfo = kInjectedMarker;
        input[1] = input[0];
        input[1].mi.dwFlags = right ? MOUSEEVENTF_RIGHTUP : MOUSEEVENTF_LEFTUP;
        SendInput(ARRAYSIZE(input), input, sizeof(INPUT));
        msg->message = WM_NULL;
        return true;
    }

    bool nonClient = press.message == WM_NCLBUTTONDOWN ||
                     press.message == WM_NCLBUTTONDBLCLK ||
                     press.message == WM_NCRBUTTONDOWN ||
                     press.message == WM_NCRBUTTONDBLCLK;
    UINT up = nonClient ? (right ? WM_NCRBUTTONUP : WM_NCLBUTTONUP)
                        : (right ? WM_RBUTTONUP : WM_LBUTTONUP);
    WPARAM upParam = nonClient ? press.wParam
                               : press.wParam & ~(MK_LBUTTON | MK_RBUTTON);
    PostMessageW(press.hwnd, up, upParam, press.lParam);

    msg->hwnd = press.hwnd;
    msg->message = press.message;
    msg->wParam = press.wParam;
    msg->lParam = press.lParam;
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// The requests, and the presses that make them

// Set while the requests to scale that are still waiting are taken out of
// the queue: the message hook sees them go, and leaves them be.
thread_local bool g_takingScaleSteps;

// A window draws itself somewhere else than on the thread that takes these -
// a browser in its own processes, a XAML app on its own thread - so taking
// one step after another keeps up while the drawing does not: a touchpad's
// stream of small turns would have the window lay itself out anew for every
// one, and its picture fall further and further behind the fingers. So the
// steps come no closer together than this, with what came in between added
// to the next one; the first of a turn goes at once.
constexpr DWORD kScaleStepMs = 40;
struct PendingScale {
    HWND root;
    int delta;
    UINT_PTR timer;  // a timer of the thread's, for the next step
};
thread_local PendingScale g_pendingScale;
thread_local DWORD g_lastScaleStep;

void TakeScaleStep() {
    PendingScale step = g_pendingScale;
    if (step.timer) {
        KillTimer(nullptr, step.timer);
    }
    g_pendingScale = {};
    g_lastScaleStep = GetTickCount();
    if (step.root && step.delta && !g_uninitializing) {
        ScaleWindow(step.root, step.delta);
    }
}

// The timer has nothing of the mod's to call: its message is taken here,
// before the application gets it.
bool HandleScaleTimer(const MSG* msg) {
    if (msg->hwnd || !g_pendingScale.timer ||
        msg->wParam != g_pendingScale.timer) {
        return false;
    }
    TakeScaleStep();
    return true;
}

bool HandleDragRequest(MSG* msg) {
    HWND root = msg->hwnd;
    POINT pt{GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam)};

    if (msg->wParam == kDragUnfade) {
        EndDragFade(root);
        return false;  // ours, and there is nothing to dispatch
    }

    if (msg->wParam == kDragCover) {
        RestoreUncovered();
        return false;
    }
    if (msg->wParam == kDragUncover) {
        UncoverAndPress(root, msg);
        return false;
    }

    if (msg->wParam == kDragScale) {
        if (g_takingScaleSteps) {
            return true;  // left as it is, for the loop below to add up
        }
        // The wheel turns notch by notch, and a window slow to resize would
        // take them one at a time, long after the wheel has stopped: the
        // notches already waiting go into the same step.
        int delta = (int)msg->lParam;
        MSG next;
        g_takingScaleSteps = true;
        while (PeekMessageW(&next, root, g_msgDrag, g_msgDrag,
                            PM_NOREMOVE | PM_NOYIELD) &&
               next.wParam == kDragScale &&
               PeekMessageW(&next, root, g_msgDrag, g_msgDrag,
                            PM_REMOVE | PM_NOYIELD)) {
            delta += (int)next.lParam;
        }
        g_takingScaleSteps = false;
        if (g_pendingScale.root && g_pendingScale.root != root) {
            TakeScaleStep();  // another window's, which is not to wait
        }
        g_pendingScale.root = root;
        g_pendingScale.delta += delta;
        if (g_pendingScale.timer || g_uninitializing) {
            return false;
        }
        DWORD since = GetTickCount() - g_lastScaleStep;
        if (since < kScaleStepMs) {
            g_pendingScale.timer =
                SetTimer(nullptr, 0, kScaleStepMs - since, nullptr);
        }
        if (!g_pendingScale.timer) {
            TakeScaleStep();
        }
        return false;
    }

    if (msg->wParam == kDragAction) {
        auto action = (WindowAction)msg->lParam;
        if (g_uninitializing || !CanTakeAction(root, action)) {
            return false;
        }
        return TakeWindowAction(msg, action);
    }

    // Checked again here: the request came from another thread, possibly in
    // another process, so the window may no longer be what it was.
    if (g_uninitializing || !IsFrameWindow(root)) {
        return false;
    }
    // Kept, because starting the drag is what overwrites it with the system
    // command the loop needs.
    WPARAM kind = msg->wParam;
    bool resize = kind == kDragResize || kind == kDragResizeLeft;
    bool started = resize ? StartResize(root, pt, msg, kind == kDragResizeLeft)
                          : StartMove(root, pt, msg);
    if (started) {
        // A held press is a drag after all, and the release is the loop's -
        // or, for a resize with the right button, the mod's to swallow.
        g_heldPress[kind == kDragResize].active = false;
        if (kind == kDragResize) {
            g_swallowButtonUp[1] = true;
        }
        BeginDragSnap(root, resize ? kDragResize : kDragMove, pt);
        BeginDragFade(root, kind);
    }
    return started;
}

// Per-thread: a button-up to swallow because we swallowed its button-down.
thread_local bool g_swallowButtonUp[2];  // [0] = left, [1] = right

bool HandleModifierButtonDown(const MSG* msg, bool right) {
    if (GetMessageExtraInfo() == (LPARAM)kInjectedMarker) {
        return false;  // a held click, handed back to the application
    }
    // A fresh press means a release we were still waiting to swallow is not
    // coming: a drag cancelled with Esc can leave the cursor outside the
    // window it started on, and the release then goes somewhere else. Same
    // for a resize whose loop never started, which leaves nothing to post
    // the release the flag below is waiting for.
    g_swallowButtonUp[right] = false;
    g_pendingRelease = false;
    g_heldPress[right].active = false;

    // A drag is already running on this thread: its loop pumps messages, so
    // presses during one arrive here too.
    if (g_uninitializing || IsInMoveSizeLoop() || !IsDragModifierDown()) {
        return false;
    }

    HWND root = GetAncestor(msg->hwnd, GA_ROOT);
    if (!root) {
        root = msg->hwnd;
    }
    if (!IsFrameWindow(root)) {
        return false;  // desktop, taskbar, menus... - normal click
    }

    bool byLeft = !right && IsLeftResizeDown();
    // Both keys are part of a resize with the left button, and either let go
    // of alone would be a tap of its own.
    bool usingWin = byLeft || g_settings.dragModifier == DragModifier::Win;
    bool usingAlt = byLeft || g_settings.dragModifier == DragModifier::Alt;

    // The second press of a double click is a gesture of its own rather than
    // another drag. The first one has already started a drag by then, which
    // is what a double click on a title bar does in Windows as well.
    WindowAction action = g_settings.doubleClickAction;
    if (!right && !byLeft && action != WindowAction::None &&
        IsDoubleClickAt(root, msg->pt, GetTickCount())) {
        Wh_Log(L"Double click on %p", root);
        ArmWinMask(usingWin, usingAlt);
        g_swallowButtonUp[0] = true;  // we took the press, so its release too
        RequestWindowAction(root, action);
        return true;
    }

    WPARAM kind = right ? kDragResize : byLeft ? kDragResizeLeft : kDragMove;
    Wh_Log(L"%s %p", kind == kDragMove ? L"Move" : L"Resize", root);
    ArmWinMask(usingWin, usingAlt);

    if (ClickGoesToApp()) {
        // Swallowed for now, and handed back if the mouse doesn't move.
        return HoldPress(root, msg, right, kind);
    }
    if (right) {
        // We consumed the right button-down, so swallow its matching up too:
        // the resize loop ends on the release the mod posts, not on the
        // physical right-up, which would otherwise reach the app.
        g_swallowButtonUp[1] = true;
    }
    // The loop consumes the physical left-up itself, so a drag with the left
    // button leaves nothing to swallow.
    RequestDrag(root, kind, msg->pt);
    return true;
}

bool HandleButtonUp(bool right) {
    if (!g_swallowButtonUp[right]) {
        return false;
    }
    g_swallowButtonUp[right] = false;
    return true;
}

bool HandleLeftButtonUp() {
    if (IsInMoveSizeLoop()) {
        // The loop is what this release is for, and what consumes it.
        g_pendingRelease = false;
        return false;
    }
    if (g_pendingRelease) {
        // The loop ended without it, so the application never saw the press
        // this would have released either.
        g_pendingRelease = false;
        return true;
    }
    return HandleButtonUp(false);
}

////////////////////////////////////////////////////////////////////////////////
// src/drag_fade.cpp
////////////////////////////////////////////////////////////////////////////////
// Fading a window to translucent while it is being dragged, the way a
// Hyprland window dims while you move it around.
//
// The fade is animated from a worker thread, because it has to keep going
// while the window's own thread is inside the system's move/size loop.
// SetLayeredWindowAttributes is safe to call from there - it sends the window
// nothing. Adding and removing WS_EX_LAYERED is not: SetWindowLongPtr sends
// WM_STYLECHANGED, and a cross-thread send waits for however long the
// application takes to answer, which a thread the unload waits for must not
// risk. So the style goes on in the drag request, which already runs on the
// window's own thread, and comes back off on that same thread, through a
// kDragUnfade request the worker posts once it is done.

// What a window looked like before the drag, so it can be put back exactly.
struct DragFade {
    bool addedLayered = false;  // the mod made it layered, and undoes that
    BYTE baseAlpha = 255;
    COLORREF baseKey = 0;
    DWORD baseFlags = 0;
};

std::mutex g_fadeMutex;
std::unordered_map<HWND, DragFade> g_fades;  // windows being dragged right now

// Everything a fade needs. The worker owns its copy, so nothing it reads can
// change or go away underneath it.
struct DragFadeWork {
    HWND root;
    DWORD threadId;  // whose move/size loop the fade follows
    int buttonVk;    // the physical button that holds this drag
    DragFade fade;
    BYTE target;
    int fadeIn;
    int fadeOut;
    bool loopSeen = false;  // the loop has been seen running at least once
};

constexpr DWORD kFadeStepMs = 8;       // ~120 Hz, about as fine as Sleep gets
constexpr DWORD kFadeHoldStepMs = 16;  // while waiting for the drag to end

// The alpha a window is dragged at. Relative to what it had, so a window that
// was already translucent keeps that much of a head start.
BYTE DragAlphaFor(BYTE baseAlpha, int opacityPercent) {
    return (BYTE)(baseAlpha * opacityPercent / 100);
}

BYTE FadeAlphaAt(BYTE from, BYTE to, int durationMs, int elapsedMs) {
    return (BYTE)(from + (to - from) *
                             AnimationProgress(elapsedMs, durationMs) +
                  0.5);
}

void SetFadeAlpha(const DragFadeWork& work, BYTE alpha) {
    SetLayeredWindowAttributes(work.root, work.fade.baseKey, alpha,
                               work.fade.baseFlags | LWA_ALPHA);
}

// Whether the drag is still going. The loop is not what says so on its own: a
// move loop does not count as one until the cursor has moved far enough to be
// a drag, so before that there is nothing to see but the button - and a loop
// that has been running and is gone means the drag is over (cancelled with
// Esc, say) even though the button is still held.
bool DragStillHeld(DragFadeWork& work) {
    if (g_uninitializing || !IsWindow(work.root)) {
        return false;
    }
    if (IsThreadInMoveSizeLoop(work.threadId)) {
        work.loopSeen = true;
        return true;
    }
    if (work.loopSeen) {
        return false;
    }
    return (GetAsyncKeyState(work.buttonVk) & 0x8000) != 0;
}

// Walks the alpha from one value to the other and returns where it got to:
// `to`, unless `whileDragging` and the drag ended on the way - the fade back
// then starts from wherever the window was instead of jumping.
BYTE FadeOver(DragFadeWork& work,
              BYTE from,
              BYTE to,
              int durationMs,
              bool whileDragging) {
    BYTE alpha = from;
    DWORD start = GetTickCount();
    int elapsed = 0;
    while (elapsed < durationMs && !g_uninitializing &&
           (!whileDragging || DragStillHeld(work))) {
        alpha = FadeAlphaAt(from, to, durationMs, elapsed);
        SetFadeAlpha(work, alpha);
        Sleep(kFadeStepMs);
        elapsed = (int)(GetTickCount() - start);
    }
    if (!whileDragging || elapsed >= durationMs) {
        alpha = to;  // it ran to its end, or it has to land no matter what
        SetFadeAlpha(work, alpha);
    }
    return alpha;
}

void RunDragFade(DragFadeWork& work) {
    // Straight into the fade, with nothing waited for first: the window dims
    // when the button goes down, which for a move is well before the loop
    // that moves it has anything to show.
    BYTE alpha =
        FadeOver(work, work.fade.baseAlpha, work.target, work.fadeIn, true);
    while (DragStillHeld(work)) {
        Sleep(kFadeHoldStepMs);
    }
    // Snapped back rather than faded when the mod is on its way out: the
    // unload is waiting for this thread to be done.
    FadeOver(work, alpha, work.fade.baseAlpha,
             g_uninitializing ? 0 : work.fadeOut, false);

    // Only the window's own thread may take WS_EX_LAYERED back off. During the
    // unload the teardown asks it to (UnfadeDraggedWindows), and the window's
    // entry stays for that. Otherwise, when the request cannot be handed over,
    // the window keeps a layered style at full opacity, which nothing can see.
    bool handed = !g_uninitializing && IsWindow(work.root) &&
                  PostMessageW(work.root, g_msgDrag, kDragUnfade, 0);
    if (!handed && (!g_uninitializing || !IsWindow(work.root))) {
        std::lock_guard<std::mutex> lock(g_fadeMutex);
        g_fades.erase(work.root);
    }
}

// The windows being dragged when the mod unloads have WS_EX_LAYERED of ours
// on them, which only their own threads may take off. Asked here through the
// request their message hook picks up, before that hook goes, and waited for
// a little: a thread that does not answer leaves its window layered.
void UnfadeDraggedWindows() {
    std::vector<HWND> windows;
    {
        std::lock_guard<std::mutex> lock(g_fadeMutex);
        for (const auto& [hwnd, fade] : g_fades) {
            windows.push_back(hwnd);
        }
    }
    for (HWND hwnd : windows) {
        PostMessageW(hwnd, g_msgDrag, kDragUnfade, 0);
    }
    for (int waited = 0; !windows.empty() && waited < 2000; waited += 20) {
        {
            std::lock_guard<std::mutex> lock(g_fadeMutex);
            if (g_fades.empty()) {
                return;
            }
        }
        Sleep(20);
    }
}

DWORD WINAPI DragFadeThread(LPVOID param) {
    {
        std::unique_ptr<DragFadeWork> work(static_cast<DragFadeWork*>(param));
        RunDragFade(*work);
    }
    g_modRefCount--;  // the last thing this thread does in the mod's image
    return 0;
}

// Called from the drag request, on the window's own thread, once a move or
// size loop is about to start.
void BeginDragFade(HWND root, WPARAM kind) {
    DragTranslucency mode = g_settings.dragTranslucency;
    int opacity = g_settings.dragOpacity;
    if (g_uninitializing || mode == DragTranslucency::Off || opacity >= 100) {
        return;
    }
    // A resize is the one drag you may want to watch reflow as it happens,
    // rather than through the window.
    if (mode == DragTranslucency::MoveOnly && kind != kDragMove) {
        return;
    }

    DragFade fade;
    LONG_PTR exStyle = GetWindowLongPtrW(root, GWL_EXSTYLE);
    if (exStyle & WS_EX_LAYERED) {
        // Two states to stay out of (test/layered_probe.cpp shows all of
        // them), and the same reason for both: a window
        // that has nothing to read either composites itself with
        // UpdateLayeredWindow already (which reports no attributes at all) or
        // is layered without having said how yet (which reports none set),
        // and once a flat alpha has been put on a window, its own
        // UpdateLayeredWindow fails from then on.
        if (!GetLayeredWindowAttributes(root, &fade.baseKey, &fade.baseAlpha,
                                        &fade.baseFlags) ||
            !fade.baseFlags) {
            return;
        }
        if (!(fade.baseFlags & LWA_ALPHA)) {
            fade.baseAlpha = 255;  // color-keyed only; no alpha to keep
        }
    } else {
        fade.addedLayered = true;
    }

    BYTE target = DragAlphaFor(fade.baseAlpha, opacity);
    if (target >= fade.baseAlpha) {
        return;  // nothing anyone could see
    }

    {
        std::lock_guard<std::mutex> lock(g_fadeMutex);
        // An earlier drag of this window is still fading back. It owns the
        // window's state until it is done, so this drag goes without.
        if (!g_fades.emplace(root, fade).second) {
            return;
        }
    }

    auto* work = new DragFadeWork{root,
                                  GetWindowThreadProcessId(root, nullptr),
                                  PhysicalButtonVk(kind == kDragResize),
                                  fade,
                                  target,
                                  g_settings.dragFadeIn,
                                  g_settings.dragFadeOut};
    if (fade.addedLayered) {
        // Layered but unchanged, so nothing flashes before the fade starts.
        SetWindowLongPtrW(root, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
        SetLayeredWindowAttributes(root, 0, fade.baseAlpha, LWA_ALPHA);
    }

    if (!StartModThread(DragFadeThread, work)) {
        delete work;
        EndDragFade(root);  // on the window's own thread, so undo it here
    }
}

// The window's thread is asked to take the mod's WS_EX_LAYERED back off, the
// one part of the fade that only it may do. This is the window being put back
// the way it was, so it runs during the teardown as well.
void EndDragFade(HWND hwnd) {
    DragFade fade;
    {
        std::lock_guard<std::mutex> lock(g_fadeMutex);
        auto it = g_fades.find(hwnd);
        if (it == g_fades.end()) {
            return;
        }
        fade = it->second;
        g_fades.erase(it);
    }

    if (fade.addedLayered) {
        LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
        SetWindowLongPtrW(hwnd, GWL_EXSTYLE,
                          exStyle & ~(LONG_PTR)WS_EX_LAYERED);
    } else {
        SetLayeredWindowAttributes(hwnd, fade.baseKey, fade.baseAlpha,
                                   fade.baseFlags);
    }
}

////////////////////////////////////////////////////////////////////////////////
// src/actions.cpp
////////////////////////////////////////////////////////////////////////////////
// What a mouse gesture with the modifier held does to the window under the
// cursor, and the gesture recognition that decides one happened.
//
// The action itself runs on the window's own thread, like everything else in
// the mod that touches a window: the request is posted with the drag message
// and carried out where the window lives. That way an application that
// handles the system command itself still gets its say, and the UIPI filter
// does not drop the request on its way to a window that belongs to another
// process - a UWP frame, for one.

WindowAction ParseWindowAction(PCWSTR raw, WindowAction whenEmpty) {
    std::wstring s = NormalizeSettingString(raw);
    if (s == L"OFF" || s == L"NONE") {
        return WindowAction::None;
    }
    if (s == L"MAXIMIZE") {
        return WindowAction::ToggleMaximize;
    }
    if (s == L"TITLEBAR") {
        return WindowAction::ToggleTitleBar;
    }
    if (s == L"CLOSE") {
        return WindowAction::Close;
    }
    // Anything unrecognized, an empty setting included, is what this binding
    // says it does by default.
    return whenEmpty;
}

// Zero follows the double-click speed from the mouse settings, which is what
// the rest of Windows goes by and what most people want.
int DoubleClickTimeMs() {
    int configured = g_settings.doubleClickTime;
    return configured > 0 ? configured : (int)GetDoubleClickTime();
}

// Runs on the window's own thread, in the message hook, with the request the
// window has just retrieved. Maximize and close turn it into the system
// command for the application's own DispatchMessage to run, the way a drag
// does: sent from the hook, a close that asks about unsaved work would hold
// its prompt with the hook still on the stack, and the mod could not unload
// until it was answered. Returns whether msg is now that command.
bool TakeWindowAction(MSG* msg, WindowAction action) {
    switch (action) {
        case WindowAction::ToggleMaximize:
            // Through the window's system menu rather than ShowWindow, so an
            // application that does its own thing with SC_MAXIMIZE keeps
            // doing it.
            msg->message = WM_SYSCOMMAND;
            msg->wParam = IsZoomed(msg->hwnd) ? SC_RESTORE : SC_MAXIMIZE;
            msg->lParam = 0;
            return true;
        case WindowAction::Close:
            // SC_CLOSE, not a kill: an application with unsaved work gets to
            // ask about it.
            msg->message = WM_SYSCOMMAND;
            msg->wParam = SC_CLOSE;
            msg->lParam = 0;
            return true;
        case WindowAction::ToggleTitleBar:
            HandleFramelessRequest(msg->hwnd, kActionToggle);
            return false;
        case WindowAction::None:
            return false;
    }
    return false;
}

// Closing is Alt+F4 pointed at the window under the cursor rather than at
// the focused one. DefWindowProc answers Alt+F4 with SC_CLOSE to the top-level
// window the focus is in unless its class says CS_NOCLOSE, whatever its
// styles - so any window that could have the focus closes, frame or not. The
// rest of the actions want a real frame, as a drag does.
bool CanTakeAction(HWND root, WindowAction action) {
    if (action != WindowAction::Close) {
        return IsFrameWindow(root);
    }
    if (!root || !IsWindow(root) || IsExcludedClass(root)) {
        return false;
    }
    LONG_PTR style = GetWindowLongPtrW(root, GWL_STYLE);
    LONG_PTR exStyle = GetWindowLongPtrW(root, GWL_EXSTYLE);
    return !(style & (WS_CHILD | WS_DISABLED)) &&
           !(exStyle & WS_EX_NOACTIVATE) &&
           !(GetClassLongPtrW(root, GCL_STYLE) & CS_NOCLOSE);
}

// With the mouse, the window is the one under the cursor rather than the
// focused one - and that can be one that never takes the focus at all, a
// flyout or a thumbnail of the shell, which closed breaks that part of the
// shell until it restarts. So a window closed this way is a visible one that
// is not a tool window, too.
bool CanTakeActionUnderCursor(HWND root, WindowAction action) {
    return CanTakeAction(root, action) &&
           (action != WindowAction::Close ||
            (IsWindowVisible(root) &&
             !(GetWindowLongPtrW(root, GWL_EXSTYLE) & WS_EX_TOOLWINDOW)));
}

void RequestWindowAction(HWND root, WindowAction action) {
    if (!PostMessageW(root, g_msgDrag, kDragAction, (LPARAM)action)) {
        Wh_Log(L"Action request for %p failed (%u)", root, GetLastError());
    }
}

////////////////////////////////////////////////////////////////////////////////
// The shortcut: modifiers and one key or mouse button

bool IsMouseButtonVk(UINT vk) {
    return vk == VK_LBUTTON || vk == VK_RBUTTON || vk == VK_MBUTTON ||
           vk == VK_XBUTTON1 || vk == VK_XBUTTON2;
}

// Which button a message is about, as a virtual key; 0 for anything that is
// not a button press or release.
UINT ButtonVkForMessage(UINT message, WPARAM wParam) {
    switch (message) {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
        case WM_LBUTTONUP:
        case WM_NCLBUTTONDOWN:
        case WM_NCLBUTTONDBLCLK:
        case WM_NCLBUTTONUP:
            return VK_LBUTTON;
        case WM_RBUTTONDOWN:
        case WM_RBUTTONDBLCLK:
        case WM_RBUTTONUP:
        case WM_NCRBUTTONDOWN:
        case WM_NCRBUTTONDBLCLK:
        case WM_NCRBUTTONUP:
            return VK_RBUTTON;
        case WM_MBUTTONDOWN:
        case WM_MBUTTONDBLCLK:
        case WM_MBUTTONUP:
        case WM_NCMBUTTONDOWN:
        case WM_NCMBUTTONDBLCLK:
        case WM_NCMBUTTONUP:
            return VK_MBUTTON;
        case WM_XBUTTONDOWN:
        case WM_XBUTTONDBLCLK:
        case WM_XBUTTONUP:
        case WM_NCXBUTTONDOWN:
        case WM_NCXBUTTONDBLCLK:
        case WM_NCXBUTTONUP:
            // The button is in the high word for both the client and the
            // non-client messages; only the low word differs between them.
            return GET_XBUTTON_WPARAM(wParam) == XBUTTON2 ? VK_XBUTTON2
                                                          : VK_XBUTTON1;
        default:
            return 0;
    }
}

bool MatchesShortcut(const Hotkey& binding, UINT vk, bool now) {
    if (!binding.vk || binding.vk != vk) {
        return false;
    }
    // Ctrl, Alt and Shift as the thread saw them when it took the message,
    // which is what a keyboard shortcut is about - or, for the keyboard hook,
    // whose thread takes no keyboard input of its own, as they are now. The
    // Win key is asked for globally either way: it belongs to the shell, and
    // a thread's own copy of the keyboard state does not reliably hear about
    // it.
    auto held = [now](int vk) {
        return now ? (GetAsyncKeyState(vk) & 0x8000) != 0 : GetKeyState(vk) < 0;
    };
    bool ctrl = held(VK_CONTROL);
    bool alt = held(VK_MENU);
    bool shift = held(VK_SHIFT);
    bool win = WinKeyDown();
    return ctrl == binding.ctrl && alt == binding.alt &&
           shift == binding.shift && win == binding.win;
}

// A button press we consumed, so that its release goes the same way instead
// of reaching the application on its own.
thread_local UINT g_swallowShortcutButton;

bool HandleShortcutButton(const MSG* msg) {
    UINT vk = ButtonVkForMessage(msg->message, msg->wParam);
    if (!vk) {
        return false;
    }
    // Any fresh press means the release we were waiting to swallow is not
    // coming any more.
    g_swallowShortcutButton = 0;

    Hotkey binding = g_settings.windowShortcut;
    WindowAction action = g_settings.windowShortcutAction;
    if (g_uninitializing || action == WindowAction::None ||
        !MatchesShortcut(binding, vk)) {
        return false;
    }

    HWND root = GetAncestor(msg->hwnd, GA_ROOT);
    if (!root) {
        root = msg->hwnd;
    }
    if (!CanTakeActionUnderCursor(root, action)) {
        return false;  // desktop, taskbar, menus... - normal click
    }

    Wh_Log(L"Shortcut on %p", root);
    ArmWinMask(binding.win, binding.alt);
    g_swallowShortcutButton = vk;
    RequestWindowAction(root, action);
    return true;
}

bool HandleShortcutButtonUp(const MSG* msg) {
    UINT vk = ButtonVkForMessage(msg->message, msg->wParam);
    if (!vk || g_swallowShortcutButton != vk) {
        return false;
    }
    g_swallowShortcutButton = 0;
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Spotting a double click
//
// Not from WM_LBUTTONDBLCLK: that one is only ever sent to windows whose
// class asked for double clicks, so half the applications out there would
// never see the gesture. The first click of the pair has also usually been
// swallowed into a drag of ours by then, which is fine - a double click on a
// title bar starts a zero-length caption drag in Windows too.

thread_local DWORD g_lastPressTick;
thread_local POINT g_lastPressPt;
thread_local HWND g_lastPressRoot;

// Takes the tick rather than reading the clock, so the rules can be tested
// without waiting half a second for each of them.
bool IsDoubleClickAt(HWND root, POINT pt, DWORD tick) {
    // The double-click rectangle is a width and a height around the first
    // click, so what each press may be off by is half of it.
    bool together =
        g_lastPressRoot == root && g_lastPressTick != 0 &&
        (int)(tick - g_lastPressTick) <= DoubleClickTimeMs() &&
        abs(pt.x - g_lastPressPt.x) <= GetSystemMetrics(SM_CXDOUBLECLK) / 2 &&
        abs(pt.y - g_lastPressPt.y) <= GetSystemMetrics(SM_CYDOUBLECLK) / 2;

    g_lastPressPt = pt;
    g_lastPressRoot = together ? nullptr : root;
    // A third click starts over rather than counting as a second double one.
    g_lastPressTick = together ? 0 : tick;
    return together;
}

////////////////////////////////////////////////////////////////////////////////
// The modifier and the wheel: the window grows or shrinks around its center

bool CanScale(HWND root) {
    return IsFrameWindow(root) && !IsZoomed(root) && !IsIconic(root);
}

// Where the steps so far have taken the window, unrounded. The window itself
// only ever has whole pixels, and a step started from them would let the
// rounding creep: off its place, and to a size a notch back does not undo.
// Good for as long as the window is where the last step put it.
struct ScaleState {
    HWND hwnd;
    RECT applied;  // the frame that step left
    double left, top, width, height;
};
thread_local ScaleState g_scale;

// On the window's own thread. By the step for every notch, either way the
// same, so that a notch up and one down come back to where they began - and
// as far as the window lets itself be sized, and no larger than its monitor's
// work area, which a window keeps its shape by stopping at. Around its center,
// or the cursor, as the settings have it.
void ScaleWindow(HWND root, int wheelDelta) {
    if (!wheelDelta || !CanScale(root)) {
        return;
    }
    // The visible frame is what gets scaled and fitted: the invisible borders
    // around it are the window's, whatever its size.
    RECT window;
    GetWindowRect(root, &window);
    RECT inset = FrameInsetOf(root);
    RECT frame = WindowToFrame(window, inset);
    int extraW = inset.left - inset.right, extraH = inset.top - inset.bottom;
    if (frame.right <= frame.left || frame.bottom <= frame.top) {
        return;
    }
    if (g_scale.hwnd != root || !EqualRect(&g_scale.applied, &frame)) {
        g_scale = {root,
                   frame,
                   (double)frame.left,
                   (double)frame.top,
                   (double)(frame.right - frame.left),
                   (double)(frame.bottom - frame.top)};
    }

    MINMAXINFO limits{};
    limits.ptMinTrackSize = {GetSystemMetrics(SM_CXMINTRACK),
                             GetSystemMetrics(SM_CYMINTRACK)};
    limits.ptMaxTrackSize = {GetSystemMetrics(SM_CXMAXTRACK),
                             GetSystemMetrics(SM_CYMAXTRACK)};
    SendMessageW(root, WM_GETMINMAXINFO, 0, (LPARAM)&limits);
    MONITORINFO monitor{sizeof(monitor)};
    GetMonitorInfoW(MonitorFromWindow(root, MONITOR_DEFAULTTONEAREST), &monitor);
    const RECT& work = monitor.rcWork;

    double factor = pow(1 + g_settings.wheelStep / 100.0,
                        (double)wheelDelta / WHEEL_DELTA);
    double most = std::min(
        {(work.right - work.left) / g_scale.width,
         (work.bottom - work.top) / g_scale.height,
         (limits.ptMaxTrackSize.x - extraW) / g_scale.width,
         (limits.ptMaxTrackSize.y - extraH) / g_scale.height});
    double least = std::max((limits.ptMinTrackSize.x - extraW) / g_scale.width,
                            (limits.ptMinTrackSize.y - extraH) / g_scale.height);
    // Only ever towards a limit: a window already past one is not made to
    // jump to it, the other way.
    if (factor > 1) {
        factor = std::min(factor, std::max(most, 1.0));
    } else {
        factor = std::max(factor, std::min(least, 1.0));
    }
    if (factor == 1) {
        return;
    }
    if (IsInMoveSizeLoop()) {
        // Mid-move, the loop would put the window back to the size it
        // started with at the next move of the mouse: the size goes to the
        // drag's subclass, which keeps it up (snap.cpp). Mid-resize, the size
        // is the cursor's.
        ScaleDraggedWindow(root, factor);
        return;
    }

    // The point that stays where it is.
    double anchorX = g_scale.left + g_scale.width / 2;
    double anchorY = g_scale.top + g_scale.height / 2;
    POINT cursor;
    if (g_settings.wheelAnchor == WheelAnchor::Cursor && GetCursorPos(&cursor)) {
        anchorX = cursor.x;
        anchorY = cursor.y;
    }
    g_scale.left = anchorX - (anchorX - g_scale.left) * factor;
    g_scale.top = anchorY - (anchorY - g_scale.top) * factor;
    g_scale.width *= factor;
    g_scale.height *= factor;
    int left = (int)lround(g_scale.left), top = (int)lround(g_scale.top);
    RECT target = FrameToWindow(
        RECT{left, top, left + (int)lround(g_scale.width),
             top + (int)lround(g_scale.height)},
        inset);
    SetWindowPos(root, nullptr, target.left, target.top,
                 target.right - target.left, target.bottom - target.top,
                 SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER |
                     SWP_NOCOPYBITS);
    GetWindowRect(root, &window);
    g_scale.applied = WindowToFrame(window, inset);
}

////////////////////////////////////////////////////////////////////////////////
// src/mouse.cpp
////////////////////////////////////////////////////////////////////////////////
// The presses no message hook gets to see.
//
// Everything in the mod meets a press in the message hook of the thread it is
// for. Content drawn by composition alone - WinUI and XAML: Paint, Windows
// Terminal, WhatsApp, the tabs of File Explorer - is not delivered that way:
// Windows hands the input over such a window to the app's own input stack,
// past every message queue, and goes by the window's region to decide.
//
// So the shell runs a low-level mouse hook as well, next to the keyboard one
// on the same never-busy thread (keyboard.cpp). A press the mod would want
// that lands on such content is taken there and handed to the window, whose
// own thread opens a hole of a few pixels in the content's region under the
// cursor and presses again: the press goes through the hole to the window
// underneath, as a message, and the region closes again the moment it has
// arrived. From there on it is an ordinary press - Windows keeps the button
// state for it and holds the mouse for the window it went to - and the
// message hook does the rest the way it always does, natively.
//
// The window shortcut is simpler: it wants nothing of the press but which
// window it was over, so that hook takes it as it comes, for any window.

constexpr int kHoleHalf = 1;      // around the cursor's pixel
constexpr DWORD kHoldCursorMaxMs = 200;  // see LowLevelMouseProc
constexpr DWORD kUncoverMaxWaitMs = 500;

// Composition draws all of it: a child window without a redirection bitmap.
bool IsCompositionContent(HWND hwnd) {
    return hwnd && (GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_CHILD) &&
           (GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & WS_EX_NOREDIRECTIONBITMAP);
}

BOOL CALLBACK FindCompositionChildProc(HWND child, LPARAM lParam) {
    if (IsCompositionContent(child)) {
        *reinterpret_cast<bool*>(lParam) = true;
        return FALSE;
    }
    return TRUE;
}

BOOL CALLBACK FindCompositionWindowProc(HWND hwnd, LPARAM lParam) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == GetCurrentProcessId() && IsWindowVisible(hwnd) &&
        IsFrameWindow(hwnd)) {
        EnumChildWindows(hwnd, FindCompositionChildProc, lParam);
    }
    return !*reinterpret_cast<bool*>(lParam);
}

// A visible window of this process with such content in it.
bool HasCompositionContentWindow() {
    bool found = false;
    EnumWindows(FindCompositionWindowProc, (LPARAM)&found);
    return found;
}

// The content under the point: the window a hit test finds, or a child of
// it that the hit test went past - XAML Islands content is not found by one,
// and gets the input all the same.
HWND CompositionContentAt(HWND hwnd, POINT pt) {
    if (IsCompositionContent(hwnd)) {
        return hwnd;
    }
    POINT client = pt;
    ScreenToClient(hwnd, &client);
    HWND child = ChildWindowFromPointEx(hwnd, client, CWP_SKIPINVISIBLE);
    return child != hwnd && IsCompositionContent(child) ? child : nullptr;
}

////////////////////////////////////////////////////////////////////////////////
// On the window's thread: the hole, and the press through it

struct Uncovered {
    HWND hwnd;
    HRGN region;  // what it had, or null for none
    DWORD threadId;
};
// In one place for every thread, so that the teardown can close what is open.
std::mutex g_uncoveredMutex;
std::vector<Uncovered> g_uncovered;

// What shows through the hole is the window underneath, which draws nothing
// there: a speck, for a frame. So the pixels under the cursor are taken from
// the screen first, and shown over the hole until it has closed again - in a
// window of the system's own class, layered and passed through by the mouse,
// so that nothing of the mod's runs in it.
struct Patch {
    HWND hwnd;
    DWORD threadId;
};
std::vector<Patch> g_patches;  // guarded by g_uncoveredMutex
constexpr int kHoleSize = 2 * kHoleHalf + 1;

// With the screen magnified, a picture of it is the magnified one: the
// pixels under the cursor are found where the magnifier shows them, and
// shrunk back to size. Asked only while Windows' Magnifier is up.
struct Magnification {
    float level;
    POINT offset;
};

Magnification ScreenMagnification() {
    Magnification none{1, {0, 0}};
    if (!FindWindowW(L"MagUIClass", nullptr)) {
        return none;
    }
    // From System32 only: in a program run from a folder anyone can write
    // to, a DLL of that name next to it would be loaded instead.
    HMODULE lib = LoadLibraryExW(L"magnification.dll", nullptr,
                                 LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!lib) {
        return none;
    }
    using Init = BOOL(WINAPI*)();
    using Get = BOOL(WINAPI*)(float*, int*, int*);
    auto init = (Init)GetProcAddress(lib, "MagInitialize");
    auto uninit = (Init)GetProcAddress(lib, "MagUninitialize");
    auto get = (Get)GetProcAddress(lib, "MagGetFullscreenTransform");
    Magnification found = none;
    if (init && uninit && get && init()) {
        float level;
        int x, y;
        if (get(&level, &x, &y) && level > 1) {
            found = {level, {x, y}};
        }
        uninit();
    }
    FreeLibrary(lib);
    return found;
}

HWND ShowPatch(POINT pt) {
    POINT pos{pt.x - kHoleHalf, pt.y - kHoleHalf};
    HDC screen = GetDC(nullptr);
    HDC memory = CreateCompatibleDC(screen);
    HBITMAP bitmap = CreateCompatibleBitmap(screen, kHoleSize, kHoleSize);
    HGDIOBJ old = SelectObject(memory, bitmap);
    Magnification mag = ScreenMagnification();
    if (mag.level > 1) {
        int side = (int)(kHoleSize * mag.level + 0.5f);
        StretchBlt(memory, 0, 0, kHoleSize, kHoleSize, screen,
                   (int)((pos.x - mag.offset.x) * mag.level + 0.5f),
                   (int)((pos.y - mag.offset.y) * mag.level + 0.5f), side, side,
                   SRCCOPY);
    } else {
        BitBlt(memory, 0, 0, kHoleSize, kHoleSize, screen, pos.x, pos.y,
               SRCCOPY);
    }
    HWND patch = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW |
            WS_EX_NOACTIVATE,
        L"Static", nullptr, WS_POPUP, pos.x, pos.y, kHoleSize, kHoleSize,
        nullptr, nullptr, nullptr, nullptr);
    if (patch) {
        POINT source{0, 0};
        SIZE size{kHoleSize, kHoleSize};
        BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, 0};
        UpdateLayeredWindow(patch, screen, &pos, &size, memory, &source, 0,
                            &blend, ULW_OPAQUE);
        ShowWindow(patch, SW_SHOWNOACTIVATE);
    }
    SelectObject(memory, old);
    DeleteObject(bitmap);
    DeleteDC(memory);
    ReleaseDC(nullptr, screen);
    return patch;
}

void RestoreUncovered(bool allThreads) {
    DWORD threadId = GetCurrentThreadId();
    std::vector<Uncovered> closing;
    std::vector<HWND> patches;
    {
        std::lock_guard<std::mutex> lock(g_uncoveredMutex);
        for (auto it = g_uncovered.begin(); it != g_uncovered.end();) {
            if (allThreads || it->threadId == threadId) {
                closing.push_back(*it);
                it = g_uncovered.erase(it);
            } else {
                it++;
            }
        }
        for (auto it = g_patches.begin(); it != g_patches.end();) {
            if (it->threadId == threadId) {
                patches.push_back(it->hwnd);
                it = g_patches.erase(it);
            } else if (allThreads) {
                PostMessageW(it->hwnd, WM_CLOSE, 0, 0);
                it = g_patches.erase(it);
            } else {
                it++;
            }
        }
    }
    // Outside the lock: the teardown, closing a window of another thread,
    // waits for that thread, which may be about to take the lock itself. The
    // window keeps the region it is given, so the saved one is handed back
    // rather than freed here.
    for (const Uncovered& u : closing) {
        SetWindowRgn(u.hwnd, u.region, FALSE);
    }
    // The patch goes once the closed hole is on the screen, and no later: a
    // drag about to start takes the window away from under it, and a patch
    // left behind would show where the window was.
    if (!patches.empty()) {
        DwmFlush();
        for (HWND patch : patches) {
            DestroyWindow(patch);
        }
    }
}

void RestoreAllUncovered() {
    RestoreUncovered(true);
}

// The press has come through, so the content can have its region back.
// Should it have gone somewhere else after all, the shell's hook, which saw
// it go, says so too (kDragCover).
void RestoreUncoveredAfterReplay() {
    if (GetMessageExtraInfo() == (LPARAM)kReplayMarker) {
        RestoreUncovered();
    }
}

struct LayerSearch {
    POINT pt;
    std::vector<HWND> layers;
};

BOOL CALLBACK FindLayerProc(HWND child, LPARAM lParam) {
    auto* search = reinterpret_cast<LayerSearch*>(lParam);
    RECT rc;
    if (IsWindowVisible(child) && IsCompositionContent(child) &&
        GetWindowRect(child, &rc) && PtInRect(&rc, search->pt)) {
        search->layers.push_back(child);
    }
    return TRUE;
}

// Not under g_uncoveredMutex: the content can be another thread's, and
// SetWindowRgn on its window waits for that thread. The hole is added to
// `holes`, for the caller to record.
bool OpenHole(HWND content, POINT pt, std::vector<Uncovered>* holes) {
    RECT rc;
    GetWindowRect(content, &rc);
    HRGN saved = CreateRectRgn(0, 0, 0, 0);
    if (GetWindowRgn(content, saved) == ERROR) {
        DeleteObject(saved);
        saved = nullptr;
    }
    HRGN region = CreateRectRgn(0, 0, rc.right - rc.left, rc.bottom - rc.top);
    if (saved) {
        CombineRgn(region, saved, nullptr, RGN_COPY);
    }
    int x = pt.x - rc.left, y = pt.y - rc.top;
    HRGN hole = CreateRectRgn(x - kHoleHalf, y - kHoleHalf, x + kHoleHalf + 1,
                              y + kHoleHalf + 1);
    CombineRgn(region, region, hole, RGN_DIFF);
    DeleteObject(hole);
    if (!SetWindowRgn(content, region, FALSE)) {
        DeleteObject(region);
        if (saved) {
            DeleteObject(saved);
        }
        return false;
    }
    holes->push_back({content, saved, GetCurrentThreadId()});
    return true;
}

// The holes under the cursor, in the content of `root` there. Returns whether
// one opened.
bool UncoverUnderCursor(HWND root) {
    // In pixels, whatever the application is aware of: the region, the
    // cursor and the window under it all have to agree.
    DPI_AWARENESS_CONTEXT previous = SetThreadDpiAwarenessContext(
        DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    POINT pt;
    GetCursorPos(&pt);
    bool opened = false;
    HWND content = CompositionContentAt(WindowFromPoint(pt), pt);
    if (content && GetAncestor(content, GA_ROOT) == root) {
        // Such content can lie on more of it - File Explorer's title bar on
        // its tabs: the hole goes through every layer at the point, or the
        // press would only find the next one down.
        LayerSearch search{pt, {}};
        EnumChildWindows(root, FindLayerProc, (LPARAM)&search);

        // The patch on the screen first, then the holes under it.
        HWND patch = ShowPatch(pt);
        if (patch) {
            DwmFlush();
        }
        std::vector<Uncovered> holes;
        for (HWND layer : search.layers) {
            if (!g_uninitializing) {
                OpenHole(layer, pt, &holes);
            }
        }
        // Recorded under the lock - unless the teardown has closed everything
        // there was meanwhile, and would miss these: they close again here.
        {
            std::lock_guard<std::mutex> lock(g_uncoveredMutex);
            opened = !g_uninitializing && !holes.empty();
            if (opened) {
                g_uncovered.insert(g_uncovered.end(), holes.begin(),
                                   holes.end());
                if (patch) {
                    g_patches.push_back({patch, GetCurrentThreadId()});
                }
            }
        }
        if (!opened) {
            for (const Uncovered& u : holes) {
                SetWindowRgn(u.hwnd, u.region, FALSE);
            }
            if (patch) {
                DestroyWindow(patch);
            }
        }
    }
    SetThreadDpiAwarenessContext(previous);
    return opened;
}

// What the shell's hook takes for a drag: the left or the right button going
// down, nothing else.
bool IsTakenPress(LPARAM lParam) {
    return HIWORD(lParam) == 0 && (LOWORD(lParam) == MOUSEEVENTF_LEFTDOWN ||
                                   LOWORD(lParam) == MOUSEEVENTF_RIGHTDOWN);
}

// The request is a message let through UIPI, so that the shell's hook can
// reach elevated windows - which lets any process send it. So the press goes
// in again only as the hook asks for it: a drag's button, with the modifier
// held this moment, through a hole that opened. Anything else presses
// nothing.
void UncoverAndPress(HWND root, const MSG* msg) {
    RestoreUncovered();
    // A window too busy to get to it in time doesn't press long after the
    // fact: the press is lost, the way it would be to a window that hangs.
    if (g_uninitializing || GetTickCount() - msg->time > kUncoverMaxWaitMs ||
        !IsTakenPress(msg->lParam) || !IsDragModifierDown() ||
        !UncoverUnderCursor(root)) {
        return;
    }

    // On the hole, where the cursor is: the shell's hook keeps it there for
    // as long as the press is waiting to go in again.
    INPUT input{};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = LOWORD(msg->lParam);
    input.mi.dwExtraInfo = kReplayMarker;
    if (!SendInput(1, &input, sizeof(input))) {
        RestoreUncovered();  // no press is coming to close it
    }
}

////////////////////////////////////////////////////////////////////////////////
// In the shell: the hook

constexpr UINT kButtonVk[5] = {VK_LBUTTON, VK_RBUTTON, VK_MBUTTON, VK_XBUTTON1,
                               VK_XBUTTON2};
constexpr DWORD kDownFlag[5] = {MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_RIGHTDOWN,
                                MOUSEEVENTF_MIDDLEDOWN, MOUSEEVENTF_XDOWN,
                                MOUSEEVENTF_XDOWN};
constexpr DWORD kUpFlag[5] = {MOUSEEVENTF_LEFTUP, MOUSEEVENTF_RIGHTUP,
                              MOUSEEVENTF_MIDDLEUP, MOUSEEVENTF_XUP,
                              MOUSEEVENTF_XUP};
constexpr DWORD kXButton[5] = {0, 0, 0, XBUTTON1, XBUTTON2};

int ButtonIndex(WPARAM wParam, DWORD mouseData) {
    switch (wParam) {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
            return 0;
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
            return 1;
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
            return 2;
        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP:
            return HIWORD(mouseData) == XBUTTON2 ? 4 : 3;
    }
    return -1;
}

// Would the message hook take a press of this button for a drag, over a
// window it can drag? Asked first, before anything about where the press is:
// every click on the system comes by here.
bool WantsButton(int button) {
    return button < 2 && IsDragModifierDown();
}

// Only ever touched on the server thread: the button of a shortcut press
// taken here, whose release goes with it; -1 for none.
int g_shortcutButton = -1;

// The window shortcut needs nothing of the press but where it was, so it is
// asked of the window under the cursor right from here, for whatever window
// that is. A press only reaches the message hook once the window takes it as
// a message, and not every one does: Task Manager's title bar takes its
// presses past every queue, and a window busy enough to open a hole late
// loses them to the Start menu, the Win key let go of by then.
bool TakeShortcutPress(int button, POINT pt) {
    Hotkey shortcut = g_settings.windowShortcut;
    WindowAction action = g_settings.windowShortcutAction;
    if (action == WindowAction::None ||
        !MatchesShortcut(shortcut, kButtonVk[button], true)) {
        return false;
    }
    HWND root = GetAncestor(WindowFromPoint(pt), GA_ROOT);
    if (!CanTakeActionUnderCursor(root, action)) {
        return false;  // desktop, taskbar, menus... - a normal click
    }
    ArmWinMask(shortcut.win, shortcut.alt);
    RequestWindowAction(root, action);
    g_shortcutButton = button;
    return true;
}

// Only ever touched on the server thread: a press taken and not pressed again
// yet, per button, and whether its button has come up meanwhile.
struct TakenPress {
    bool active;
    bool released;
    DWORD time;
    HWND root;
};
TakenPress g_taken[5];

// The release of a press that came up before the press went in again: sent
// once it has, from a timer, outside the hook.
int g_lateRelease = -1;

void CALLBACK LateReleaseTimer(HWND, UINT, UINT_PTR id, DWORD) {
    KillTimer(nullptr, id);
    int button = g_lateRelease;
    g_lateRelease = -1;
    if (button < 0) {
        return;
    }
    INPUT input{};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = kUpFlag[button];
    input.mi.mouseData = kXButton[button];
    input.mi.dwExtraInfo = kReplayMarker;
    SendInput(1, &input, sizeof(input));
}

// Two fingers on a touchpad, over a program that scrolls with them itself -
// Chrome, Discord, Spotify: Windows asks the program first, with
// DM_POINTERHITTEST, whether it takes the contact. With the modifier held,
// and the wheel scaling windows, the program is not to hear the question, and
// is left the wheel Windows makes of the gesture for a program that does not
// take it - which the shell's mouse hook takes in turn (TakeWheel).
//
// The question is sent, not posted, so the message hook never sees it. What
// does is the hook on sent messages, a moment before the window's procedure
// is called: it puts a subclass of ours on the window for just that one
// message, which answers it the way a window that does not take touchpads
// does, and takes itself off again. File Explorer asks nothing of the kind:
// it scrolls its touchpad gestures some other way, and goes on doing so.
bool TouchpadGoesToWheel(WPARAM pointerHitTest) {
    POINTER_INPUT_TYPE type;
    return g_settings.wheelScale && IsDragModifierDown() &&
           GetPointerType(GET_POINTERID_WPARAM(pointerHitTest), &type) &&
           type == PT_TOUCHPAD;
}

constexpr UINT_PTR kTouchpadSubclassId = 0x48597074;  // 'Hypt'
// The windows with that subclass on, for the teardown to take it off should
// the message it waits for not have come.
std::mutex g_touchpadMutex;
std::vector<HWND> g_touchpadAsked;

void EndKeepingTouchpad(HWND hwnd);

LRESULT CALLBACK TouchpadSubclassProc(HWND hwnd,
                                      UINT uMsg,
                                      WPARAM wParam,
                                      LPARAM lParam,
                                      UINT_PTR,
                                      DWORD_PTR) {
    ModRef ref;  // the image must not go away under this procedure

    if (uMsg == DM_POINTERHITTEST) {
        EndKeepingTouchpad(hwnd);
        if (TouchpadGoesToWheel(wParam)) {
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);
        }
    } else if (uMsg == WM_NCDESTROY) {
        EndKeepingTouchpad(hwnd);
    } else if (uMsg == g_msgDrag && wParam == kDragUntouch) {
        EndKeepingTouchpad(hwnd);
        return 0;
    }
    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

void EndKeepingTouchpad(HWND hwnd) {
    {
        std::lock_guard<std::mutex> lock(g_touchpadMutex);
        auto it = std::find(g_touchpadAsked.begin(), g_touchpadAsked.end(), hwnd);
        if (it == g_touchpadAsked.end()) {
            return;
        }
        g_touchpadAsked.erase(it);
    }
    RemoveWindowSubclass(hwnd, TouchpadSubclassProc, kTouchpadSubclassId);
}

// From the hook on sent messages, on the window's own thread.
void KeepTouchpadFromApp(HWND hwnd, WPARAM pointerHitTest) {
    if (g_uninitializing || !TouchpadGoesToWheel(pointerHitTest)) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_touchpadMutex);
    if (std::find(g_touchpadAsked.begin(), g_touchpadAsked.end(), hwnd) !=
            g_touchpadAsked.end() ||
        g_uninitializing) {
        return;
    }
    if (SetWindowSubclass(hwnd, TouchpadSubclassProc, kTouchpadSubclassId, 0)) {
        g_touchpadAsked.push_back(hwnd);
    }
}

std::vector<HWND> SnapshotTouchpadWindows() {
    std::lock_guard<std::mutex> lock(g_touchpadMutex);
    return g_touchpadAsked;
}

// Only touched on the server thread: the window the wheel last went to, and
// when. A turn of the wheel stays with its window for as long as it goes on:
// a window grown or shrunk out from under the cursor is still the one the
// next notch is for, not whatever the cursor is over now. It ends when the
// modifier comes up, or when the wheel has rested this long.
HWND g_wheelRoot;
DWORD g_wheelAt;
constexpr DWORD kWheelTurnMs = 500;

void EndWheelTurn() {
    g_wheelRoot = nullptr;
}

// The window the wheel is for: the one the turn is going to, or one being
// moved, wherever the cursor has got to by now - or else the one under the
// cursor.
HWND WheelTarget(POINT pt, DWORD time) {
    if (g_wheelRoot && time - g_wheelAt < kWheelTurnMs &&
        IsWindow(g_wheelRoot) && CanScale(g_wheelRoot)) {
        return g_wheelRoot;
    }
    HWND under = GetAncestor(WindowFromPoint(pt), GA_ROOT);
    for (HWND candidate : {under, g_wheelRoot}) {
        GUITHREADINFO info{sizeof(info)};
        if (candidate && IsWindow(candidate) &&
            GetGUIThreadInfo(GetWindowThreadProcessId(candidate, nullptr),
                             &info) &&
            (info.flags & GUI_INMOVESIZE) && info.hwndMoveSize &&
            (candidate == under ||
             GetAncestor(info.hwndMoveSize, GA_ROOT) == candidate)) {
            return GetAncestor(info.hwndMoveSize, GA_ROOT);
        }
    }
    return under;
}

// The wheel turned with the modifier held, over a window: it is the window's
// to grow or shrink by (actions.cpp), rather than the app's to scroll. Taken
// here for every window alike, so it needs no hole: the window is asked, and
// not pressed again.
bool TakeWheel(const MSLLHOOKSTRUCT* info) {
    if (!g_settings.wheelScale || info->dwExtraInfo == kInjectedMarker ||
        !IsDragModifierDown()) {
        return false;
    }
    // Any window's, here as in the shell: with an elevated window in front,
    // the shell's hook is not called at all, and this one - in front of it in
    // the chain - is the only one to see the wheel.
    HWND root = WheelTarget(info->pt, info->time);
    if (!CanScale(root)) {
        return false;
    }
    g_wheelRoot = root;
    g_wheelAt = info->time;
    PostMessageW(root, g_msgDrag, kDragScale, (short)HIWORD(info->mouseData));
    ArmWinMask(g_settings.dragModifier == DragModifier::Win,
               g_settings.dragModifier == DragModifier::Alt);
    return true;
}

LRESULT CALLBACK LowLevelMouseProc(int code, WPARAM wParam, LPARAM lParam) {
    ModRef ref;  // the image must not go away under this procedure

    auto* info = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
    if (code != HC_ACTION || g_uninitializing) {
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }
    // The cursor stays put between a press taken and the press again: moved
    // on, it would take the press past the hole and onto the content. A few
    // milliseconds, and never long, should the window not answer.
    if (wParam == WM_MOUSEMOVE && info->dwExtraInfo != kReplayMarker) {
        for (TakenPress& taken : g_taken) {
            if (taken.active && info->time - taken.time < kHoldCursorMaxMs) {
                return 1;
            }
        }
    }
    if (wParam == WM_MOUSEWHEEL && TakeWheel(info)) {
        return 1;
    }
    int button = ButtonIndex(wParam, info->mouseData);
    if (button < 0) {
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }
    bool down = wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN ||
                wParam == WM_MBUTTONDOWN || wParam == WM_XBUTTONDOWN;
    TakenPress& taken = g_taken[button];

    if (info->dwExtraInfo == kReplayMarker) {
        // The press again, through the hole: what it does is up to the
        // window now. One whose button is up already gets its release.
        if (down && taken.active) {
            taken.active = false;
            PostMessageW(taken.root, g_msgDrag, kDragCover, 0);
            if (taken.released) {
                g_lateRelease = button;
                SetTimer(nullptr, 0, 0, LateReleaseTimer);
            }
        }
    } else if (info->dwExtraInfo != kInjectedMarker) {
        if (down && TakeShortcutPress(button, info->pt)) {
            return 1;
        }
        if (!down && button == g_shortcutButton) {
            g_shortcutButton = -1;
            return 1;
        }
        if (down && WantsButton(button)) {
            HWND hwnd = WindowFromPoint(info->pt);
            HWND root = GetAncestor(hwnd, GA_ROOT);
            // An elevated process takes the presses on its own windows, the
            // ones kept from the shell's hook, and leaves the rest to it.
            DWORD pid = 0;
            GetWindowThreadProcessId(root, &pid);
            if ((IsShellProcess() || pid == GetCurrentProcessId()) &&
                IsFrameWindow(root) && CompositionContentAt(hwnd, info->pt)) {
                taken = {true, false, info->time, root};
                PostMessageW(root, g_msgDrag, kDragUncover,
                             MAKELPARAM(kDownFlag[button], kXButton[button]));
                return 1;
            }
            taken.active = false;
        } else if (down) {
            taken.active = false;
        } else if (taken.active) {
            taken.released = true;
            return 1;  // let go of before it went in again: it comes after
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////
// src/snap.cpp
////////////////////////////////////////////////////////////////////////////////
// Magnetic edges while a window is being dragged, the aspect ratio held
// while it is being resized, and the size the mouse wheel gives a window
// while it is being moved.
//
// All three live in the same place: a subclass put on the window for the length of
// the drag, which edits the rectangle the system's loop is about to apply.
// The loop hands it over as WM_MOVING / WM_SIZING - sent messages, which the
// message hook never sees, so a subclass is the only way to them. It goes on
// in the drag request, where we are already on the window's own thread, and
// comes off at WM_EXITSIZEMOVE, which ends every drag including one cancelled
// with Esc.
//
// Two things about those loops are measured rather than assumed (see
// test/snap_probe.cpp):
//
//   * The size loop works out its rectangle from the cursor and the corner
//     that is staying put, so overriding it costs nothing: pull away from a
//     snapped edge and the window comes off it.
//   * The move loop instead takes the position it last applied and adds the
//     cursor's movement to it, so an override sticks forever - the window
//     would stay glued to the first line it touched. The position is
//     therefore worked out here from the cursor and the grab offset, which
//     takes that computation away from the loop entirely.
//
// Distances are in the coordinates of the *visible* frame rather than the
// window rectangle: a window carries an invisible resize border several
// pixels wide on three sides, so two windows whose window rectangles touch
// have a visible gap of twice that between them.

struct SnapState {
    WPARAM kind = kDragMove;
    POINT grabOffset{};  // cursor minus the window's top left, at the start
    SIZE grabSize{};     // the size that offset was taken at
    RECT startFrame{};   // the visible frame at the start, for the ratio
    RECT inset{};        // the visible frame minus the window rectangle
    std::vector<RECT> windows;   // the frames of the other windows
    std::vector<RECT> monitors;  // the work areas to snap to
    // The frame size the wheel has given the window mid-move, unrounded - or
    // none. The loop goes on with the size it started with otherwise.
    double scaledWidth = 0, scaledHeight = 0;
    // Where on that frame the cursor holds it, unrounded.
    double holdX = 0, holdY = 0;
};

std::mutex g_snapMutex;
std::unordered_map<HWND, SnapState> g_snaps;

constexpr UINT_PTR kSnapSubclassId = 0x48597073;  // 'Hyps'

////////////////////////////////////////////////////////////////////////////////
// Geometry

// How far the visible frame sits inside the window rectangle, which for a
// window with a resize border is several pixels on three sides.
//
// DWM is the only one who knows, and it answers in physical pixels whatever
// the process has been told about DPI - measured, see test/dpi_probe.cpp -
// while GetWindowRect, GetCursorPos and the work areas are all virtualized
// for a DPI-unaware process. In one of those, on a scaled monitor, the two
// are not the same coordinates at all: the inset came out as 211 pixels in
// the probe rather than 9. An inset that is not a small step inward is that
// disagreement showing, and then it is better to work in window rectangles
// than in wrong ones - windows line up a hair apart instead of exactly,
// which is what the system's own snapping does anyway.
constexpr int kMaxFrameInset = 32;

RECT FrameInsetOf(HWND hwnd) {
    RECT window{}, frame{};
    if (!GetWindowRect(hwnd, &window) ||
        FAILED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &frame,
                                     sizeof(frame)))) {
        return RECT{};
    }
    RECT inset{frame.left - window.left, frame.top - window.top,
               frame.right - window.right, frame.bottom - window.bottom};
    bool sane = inset.left >= 0 && inset.left <= kMaxFrameInset &&
                inset.top >= 0 && inset.top <= kMaxFrameInset &&
                inset.right <= 0 && inset.right >= -kMaxFrameInset &&
                inset.bottom <= 0 && inset.bottom >= -kMaxFrameInset;
    return sane ? inset : RECT{};
}

RECT WindowToFrame(const RECT& rc, const RECT& inset) {
    return RECT{rc.left + inset.left, rc.top + inset.top,
                rc.right + inset.right, rc.bottom + inset.bottom};
}

RECT FrameToWindow(const RECT& rc, const RECT& inset) {
    return RECT{rc.left - inset.left, rc.top - inset.top,
                rc.right - inset.right, rc.bottom - inset.bottom};
}

// Where a window's visible frame is, in the coordinates this process sees.
RECT VisibleFrameOf(HWND hwnd) {
    RECT window{};
    if (!GetWindowRect(hwnd, &window)) {
        return RECT{};
    }
    return WindowToFrame(window, FrameInsetOf(hwnd));
}

bool RangesOverlap(int aLow, int aHigh, int bLow, int bHigh) {
    return aLow < bHigh && bLow < aHigh;
}

// The closest of the lines to this edge, or the edge itself when none of them
// is near enough.
int SnappedEdge(int edge, const std::vector<int>& lines, int limit) {
    int best = edge;
    int bestDistance = limit + 1;
    for (int line : lines) {
        int distance = abs(line - edge);
        if (distance <= limit && distance < bestDistance) {
            best = line;
            bestDistance = distance;
        }
    }
    return best;
}

// Where an edge of ours may land: against a neighbour with the gap between
// them, flush with the same edge of one, or at the edge of a work area. Only
// neighbours that overlap us on the other axis count, or sit right beside us
// on it - a window far above is not something this one is lining up with,
// but the one it has just been put next to is, top to top.
void CollectLines(const SnapState& state,
                  const RECT& frame,
                  bool horizontal,
                  int limit,
                  std::vector<int>* forLow,
                  std::vector<int>* forHigh) {
    int windowGap = g_settings.snapWindowGap;
    int monitorGap = g_settings.snapMonitorGap;
    SnapMode mode = g_settings.snap;
    int reach = windowGap + limit;
    int acrossLow = (horizontal ? frame.top : frame.left) - reach;
    int acrossHigh = (horizontal ? frame.bottom : frame.right) + reach;

    if (mode == SnapMode::Both || mode == SnapMode::Windows) {
        for (const RECT& other : state.windows) {
            int theirLow = horizontal ? other.left : other.top;
            int theirHigh = horizontal ? other.right : other.bottom;
            int theirAcrossLow = horizontal ? other.top : other.left;
            int theirAcrossHigh = horizontal ? other.bottom : other.right;
            if (!RangesOverlap(acrossLow, acrossHigh, theirAcrossLow,
                               theirAcrossHigh)) {
                continue;
            }
            forLow->push_back(theirHigh + windowGap);  // against its far side
            forLow->push_back(theirLow);               // or flush with it
            forHigh->push_back(theirLow - windowGap);
            forHigh->push_back(theirHigh);
        }
    }

    if (mode == SnapMode::Both || mode == SnapMode::Monitor) {
        for (const RECT& work : state.monitors) {
            int workLow = horizontal ? work.left : work.top;
            int workHigh = horizontal ? work.right : work.bottom;
            forLow->push_back(workLow + monitorGap);
            forHigh->push_back(workHigh - monitorGap);
        }
    }
}

// A move shifts the whole window, so each axis takes the one correction that
// suits it best: moving a single edge here would resize the window instead.
void SnapMovedFrame(const SnapState& state, int limit, RECT* frame) {
    for (int axis = 0; axis < 2; axis++) {
        bool horizontal = axis == 0;
        std::vector<int> forLow, forHigh;
        CollectLines(state, *frame, horizontal, limit, &forLow, &forHigh);

        int low = horizontal ? frame->left : frame->top;
        int high = horizontal ? frame->right : frame->bottom;
        int shiftLow = SnappedEdge(low, forLow, limit) - low;
        int shiftHigh = SnappedEdge(high, forHigh, limit) - high;
        int shift = shiftLow;
        if (shiftLow == 0) {
            shift = shiftHigh;
        } else if (shiftHigh != 0 && abs(shiftHigh) < abs(shiftLow)) {
            shift = shiftHigh;
        }

        if (horizontal) {
            frame->left += shift;
            frame->right += shift;
        } else {
            frame->top += shift;
            frame->bottom += shift;
        }
    }
}

bool EdgeMovesLeft(UINT edge) {
    return edge == WMSZ_LEFT || edge == WMSZ_TOPLEFT || edge == WMSZ_BOTTOMLEFT;
}
bool EdgeMovesRight(UINT edge) {
    return edge == WMSZ_RIGHT || edge == WMSZ_TOPRIGHT ||
           edge == WMSZ_BOTTOMRIGHT;
}
bool EdgeMovesTop(UINT edge) {
    return edge == WMSZ_TOP || edge == WMSZ_TOPLEFT || edge == WMSZ_TOPRIGHT;
}
bool EdgeMovesBottom(UINT edge) {
    return edge == WMSZ_BOTTOM || edge == WMSZ_BOTTOMLEFT ||
           edge == WMSZ_BOTTOMRIGHT;
}

// A resize moves the edges the user has hold of and leaves the rest where
// they are, so each of those edges snaps on its own.
void SnapSizedFrame(const SnapState& state, UINT edge, int limit, RECT* frame) {
    for (int axis = 0; axis < 2; axis++) {
        bool horizontal = axis == 0;
        std::vector<int> forLow, forHigh;
        CollectLines(state, *frame, horizontal, limit, &forLow, &forHigh);

        bool lowMoves = horizontal ? EdgeMovesLeft(edge) : EdgeMovesTop(edge);
        bool highMoves =
            horizontal ? EdgeMovesRight(edge) : EdgeMovesBottom(edge);
        LONG& low = horizontal ? frame->left : frame->top;
        LONG& high = horizontal ? frame->right : frame->bottom;
        if (lowMoves) {
            low = SnappedEdge(low, forLow, limit);
        }
        if (highMoves) {
            high = SnappedEdge(high, forHigh, limit);
        }
    }
}

// Keeps the shape the window started the resize with. The corner the user is
// not holding stays where it is, and on a corner drag the axis that moved
// further decides - so the window follows the cursor instead of fighting it.
void ApplyAspectRatio(UINT edge, const RECT& start, RECT* rc) {
    int startWidth = start.right - start.left;
    int startHeight = start.bottom - start.top;
    if (startWidth <= 0 || startHeight <= 0) {
        return;
    }
    double ratio = (double)startWidth / startHeight;
    int width = rc->right - rc->left;
    int height = rc->bottom - rc->top;

    bool horizontalOnly = edge == WMSZ_LEFT || edge == WMSZ_RIGHT;
    bool verticalOnly = edge == WMSZ_TOP || edge == WMSZ_BOTTOM;
    if (horizontalOnly) {
        height = (int)(width / ratio + 0.5);
    } else if (verticalOnly) {
        width = (int)(height * ratio + 0.5);
    } else {
        // A corner follows both axes at once, so the size to take is the one
        // on the shape's own line that is nearest to what the cursor asked
        // for - its projection onto that line. Picking whichever axis moved
        // further instead looks the same most of the time and jumps every
        // time the two swap places, which is wherever the cursor happens to
        // cross the diagonal.
        double t = ((double)width * startWidth + (double)height * startHeight) /
                   ((double)startWidth * startWidth +
                    (double)startHeight * startHeight);
        if (t < 0.0) {
            t = 0.0;
        }
        width = (int)(startWidth * t + 0.5);
        height = (int)(startHeight * t + 0.5);
    }

    if (EdgeMovesLeft(edge)) {
        rc->left = rc->right - width;
    } else {
        rc->right = rc->left + width;
    }
    if (EdgeMovesTop(edge)) {
        rc->top = rc->bottom - height;
    } else {
        rc->bottom = rc->top + height;
    }
}

////////////////////////////////////////////////////////////////////////////////
// What the loop hands over

int SnapDistancePx(HWND hwnd) {
    return MulDiv(g_settings.snapDistance, (int)WindowDpi(hwnd), 96);
}

// Asked about globally rather than through the thread's own copy of the
// keyboard state: these keys are pressed and let go of in the middle of a
// drag, and what matters is whether the key is down now, not whether the
// window's thread has got round to hearing about it.
bool ModifierHeld(UINT vk) {
    return vk != 0 && (GetAsyncKeyState((int)vk) & 0x8000) != 0;
}

bool KeepAspectHeld() {
    return ModifierHeld(g_settings.keepAspectVk);
}

// Whether the magnet applies right now. The extra key can turn it on while it
// is held, or turn it off while it is held, or not exist at all.
bool SnapAllowedWith(bool modifierDown) {
    if (g_settings.snap == SnapMode::Off) {
        return false;
    }
    if (g_settings.snapModifierVk == 0) {
        return true;
    }
    return g_settings.snapModifierHold ? modifierDown : !modifierDown;
}

bool SnapAllowed() {
    return SnapAllowedWith(ModifierHeld(g_settings.snapModifierVk));
}

void AdjustMove(HWND hwnd, const SnapState& state, RECT* rc) {
    bool snapping = g_settings.snap != SnapMode::Off && SnapAllowed();
    bool scaled = state.scaledWidth > 0;
    if (!snapping && !scaled) {
        return;  // nothing to add to the loop's own rectangle
    }
    // The loop's own proposal is thrown away: it works the position out from
    // the rectangle it last applied, so anything changed here would be built
    // on from then on and the window would never come off the line again.
    POINT cursor;
    if (!GetCursorPos(&cursor)) {
        return;
    }
    int width = rc->right - rc->left;
    int height = rc->bottom - rc->top;
    if (scaled) {
        // The frame's size plus the invisible border's.
        width = (int)lround(state.scaledWidth) + state.inset.left -
                state.inset.right;
        height = (int)lround(state.scaledHeight) + state.inset.top -
                 state.inset.bottom;
    }

    POINT offset = state.grabOffset;
    if (state.grabSize.cx > 0 && state.grabSize.cy > 0 &&
        (width != state.grabSize.cx || height != state.grabSize.cy)) {
        // The loop resized the window mid-drag: a maximized one being
        // restored, or a move onto a monitor with a different scale. Keep the
        // cursor where it was on the window rather than where it was in
        // pixels.
        offset.x = MulDiv(offset.x, width, state.grabSize.cx);
        offset.y = MulDiv(offset.y, height, state.grabSize.cy);
    }

    RECT window{cursor.x - offset.x, cursor.y - offset.y, 0, 0};
    window.right = window.left + width;
    window.bottom = window.top + height;

    RECT frame = WindowToFrame(window, state.inset);
    if (snapping) {
        SnapMovedFrame(state, SnapDistancePx(hwnd), &frame);
    }
    *rc = FrameToWindow(frame, state.inset);
}

// The wheel turned while the window is being moved: the window takes the new
// size at once, and the loop with every step from now on. Around the point
// the cursor holds it by, which stays under the cursor - or, as the settings
// may have it, around its center.
//
// Where the cursor holds it is the drag's own grab offset, not where the
// window is on the screen: the window is always a step of the loop behind
// the cursor, and a hold worked out from it would take that lag in with
// every notch, until the window had crept out from under the cursor.
// Returns false if this is not a move of the mod's, which then has no size to
// keep up.
bool ScaleDraggedWindow(HWND hwnd, double factor) {
    POINT cursor;
    RECT window;
    if (!GetCursorPos(&cursor) || !GetWindowRect(hwnd, &window)) {
        return false;
    }
    RECT target;
    {
        std::lock_guard<std::mutex> lock(g_snapMutex);
        auto it = g_snaps.find(hwnd);
        if (it == g_snaps.end() || it->second.kind != kDragMove) {
            return false;
        }
        SnapState& state = it->second;
        const RECT& inset = state.inset;
        if (state.scaledWidth <= 0) {
            // The size the loop has given the window, and the offset at it -
            // which the loop keeps in proportion when it changes the size, a
            // maximized window being restored.
            int width = window.right - window.left;
            int height = window.bottom - window.top;
            POINT offset = state.grabOffset;
            if (state.grabSize.cx > 0 && state.grabSize.cy > 0) {
                offset.x = MulDiv(offset.x, width, state.grabSize.cx);
                offset.y = MulDiv(offset.y, height, state.grabSize.cy);
            }
            state.scaledWidth = width - inset.left + inset.right;
            state.scaledHeight = height - inset.top + inset.bottom;
            state.holdX = offset.x - inset.left;
            state.holdY = offset.y - inset.top;
        }
        double width = state.scaledWidth * factor;
        double height = state.scaledHeight * factor;
        if (g_settings.wheelAnchor == WheelAnchor::Center) {
            state.holdX += (width - state.scaledWidth) / 2;
            state.holdY += (height - state.scaledHeight) / 2;
        } else {
            state.holdX *= factor;
            state.holdY *= factor;
        }
        state.scaledWidth = width;
        state.scaledHeight = height;

        int left = cursor.x - (int)lround(state.holdX);
        int top = cursor.y - (int)lround(state.holdY);
        RECT frame{left, top, left + (int)lround(width),
                   top + (int)lround(height)};
        target = FrameToWindow(frame, inset);
        state.grabOffset = POINT{cursor.x - target.left, cursor.y - target.top};
        state.grabSize = SIZE{target.right - target.left,
                              target.bottom - target.top};
    }
    SetWindowPos(hwnd, nullptr, target.left, target.top,
                 target.right - target.left, target.bottom - target.top,
                 SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER |
                     SWP_NOCOPYBITS);
    return true;
}

void AdjustSize(HWND hwnd, const SnapState& state, UINT edge, RECT* rc) {
    bool keepAspect = KeepAspectHeld();
    if (!keepAspect && !SnapAllowed()) {
        return;
    }
    // Both work on the visible frame: the shape the user sees is the one
    // between the borders, not the one the window rectangle describes.
    RECT frame = WindowToFrame(*rc, state.inset);
    if (keepAspect) {
        // The ratio and the magnet want different rectangles, and the one the
        // user is holding a key down for wins.
        ApplyAspectRatio(edge, state.startFrame, &frame);
    } else {
        SnapSizedFrame(state, edge, SnapDistancePx(hwnd), &frame);
    }
    *rc = FrameToWindow(frame, state.inset);
}

////////////////////////////////////////////////////////////////////////////////
// The neighbours to line up with

struct NeighbourSearch {
    HWND dragged;
    SnapState* state;
};

BOOL CALLBACK CollectWindowProc(HWND hwnd, LPARAM lParam) {
    auto* search = (NeighbourSearch*)lParam;
    if (hwnd == search->dragged || !IsWindowVisible(hwnd) || IsIconic(hwnd) ||
        !IsFrameWindow(hwnd)) {
        return TRUE;
    }
    BOOL cloaked = FALSE;
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked,
                                        sizeof(cloaked))) &&
        cloaked) {
        return TRUE;  // another virtual desktop, or a suspended app
    }

    RECT frame = VisibleFrameOf(hwnd);
    if (frame.right > frame.left && frame.bottom > frame.top) {
        search->state->windows.push_back(frame);
    }
    return TRUE;
}

BOOL CALLBACK CollectMonitorProc(HMONITOR monitor, HDC, LPRECT, LPARAM lParam) {
    auto* state = (SnapState*)lParam;
    MONITORINFO info{sizeof(info)};
    if (GetMonitorInfoW(monitor, &info)) {
        state->monitors.push_back(info.rcWork);
    }
    return TRUE;
}

// Once per drag: windows do not move while one is going on, and asking for
// them again at every step would be forty times a second.
void CollectNeighbours(HWND dragged, SnapState* state) {
    NeighbourSearch search{dragged, state};
    EnumWindows(CollectWindowProc, (LPARAM)&search);
    EnumDisplayMonitors(nullptr, nullptr, CollectMonitorProc, (LPARAM)state);
}

////////////////////////////////////////////////////////////////////////////////
// The subclass

LRESULT CALLBACK SnapSubclassProc(HWND hwnd,
                                  UINT uMsg,
                                  WPARAM wParam,
                                  LPARAM lParam,
                                  UINT_PTR uIdSubclass,
                                  DWORD_PTR dwRefData) {
    ModRef ref;  // the image must not go away under this procedure

    switch (uMsg) {
        case WM_MOVING:
        case WM_SIZING: {
            // Ours first and the application's afterwards: whatever it wants
            // to do to the rectangle - a terminal rounding it to whole rows,
            // a window with rules of its own - gets the last word.
            std::lock_guard<std::mutex> lock(g_snapMutex);
            auto it = g_snaps.find(hwnd);
            if (it == g_snaps.end()) {
                break;
            }
            if (uMsg == WM_MOVING) {
                AdjustMove(hwnd, it->second, (RECT*)lParam);
            } else {
                AdjustSize(hwnd, it->second, (UINT)wParam, (RECT*)lParam);
            }
            break;
        }

        case WM_WINDOWPOSCHANGING: {
            // The loop's last word, as the button comes up: its own
            // rectangle, put in place without a WM_MOVING - at the size it
            // started with, after a turn of the wheel with the mouse held
            // still. It gets the one the moves would have had.
            auto* pos = reinterpret_cast<WINDOWPOS*>(lParam);
            if (pos->flags & SWP_NOSIZE) {
                break;
            }
            std::lock_guard<std::mutex> lock(g_snapMutex);
            auto it = g_snaps.find(hwnd);
            if (it == g_snaps.end() || it->second.kind != kDragMove ||
                it->second.scaledWidth <= 0 ||
                (pos->cx == it->second.grabSize.cx &&
                 pos->cy == it->second.grabSize.cy)) {
                break;
            }
            RECT rc{pos->x, pos->y, pos->x + pos->cx, pos->y + pos->cy};
            AdjustMove(hwnd, it->second, &rc);
            pos->x = rc.left;
            pos->y = rc.top;
            pos->cx = rc.right - rc.left;
            pos->cy = rc.bottom - rc.top;
            pos->flags &= ~SWP_NOMOVE;
            break;
        }

        case WM_DPICHANGED: {
            // The invisible border is measured in pixels, and the window has
            // just changed how big a pixel is.
            std::lock_guard<std::mutex> lock(g_snapMutex);
            auto it = g_snaps.find(hwnd);
            if (it != g_snaps.end()) {
                it->second.inset = FrameInsetOf(hwnd);
                // And the loop's own size for the new scale is the one to go
                // on with.
                it->second.scaledWidth = it->second.scaledHeight = 0;
            }
            break;
        }

        case WM_EXITSIZEMOVE:
        case WM_NCDESTROY:
            EndDragSnap(hwnd);
            break;

        default:
            if (uMsg == g_msgDrag && wParam == kDragUnsnap) {
                // Sent rather than posted: the teardown taking the subclass
                // off before the image goes away.
                EndDragSnap(hwnd);
                return 0;
            }
            break;
    }

    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

std::vector<HWND> SnapshotSnappedWindows() {
    std::lock_guard<std::mutex> lock(g_snapMutex);
    std::vector<HWND> result;
    result.reserve(g_snaps.size());
    for (const auto& [hwnd, state] : g_snaps) {
        result.push_back(hwnd);
    }
    return result;
}

// Called from the drag request, on the window's own thread, once a move or
// size loop is about to start.
void BeginDragSnap(HWND root, WPARAM kind, POINT pt) {
    // Every move gets the subclass, magnet or none: the wheel may yet give
    // it a size to keep.
    bool wantSnap = g_settings.snap != SnapMode::Off;
    bool wantRatio = kind == kDragResize && g_settings.keepAspectVk != 0;
    bool wantScale = kind == kDragMove && g_settings.wheelScale;
    if (g_uninitializing || (!wantSnap && !wantRatio && !wantScale)) {
        return;
    }

    RECT window{};
    if (!GetWindowRect(root, &window)) {
        return;
    }

    SnapState state;
    state.kind = kind;
    state.inset = FrameInsetOf(root);
    state.startFrame = WindowToFrame(window, state.inset);

    state.grabOffset = POINT{pt.x - window.left, pt.y - window.top};
    state.grabSize =
        SIZE{window.right - window.left, window.bottom - window.top};
    if (wantSnap) {
        CollectNeighbours(root, &state);
    }

    {
        std::lock_guard<std::mutex> lock(g_snapMutex);
        if (!g_snaps.emplace(root, std::move(state)).second) {
            return;  // a drag of this window is somehow still going on
        }
    }

    if (!SetWindowSubclass(root, SnapSubclassProc, kSnapSubclassId, 0)) {
        Wh_Log(L"SetWindowSubclass failed for %p", root);
        std::lock_guard<std::mutex> lock(g_snapMutex);
        g_snaps.erase(root);
    }
}

// On the window's own thread: at the end of the drag, when the window dies,
// or when the teardown asks for it.
void EndDragSnap(HWND hwnd) {
    bool had = false;
    {
        std::lock_guard<std::mutex> lock(g_snapMutex);
        had = g_snaps.erase(hwnd) != 0;
    }
    if (had) {
        RemoveWindowSubclass(hwnd, SnapSubclassProc, kSnapSubclassId);
    }
}

////////////////////////////////////////////////////////////////////////////////
// src/hooks.cpp
////////////////////////////////////////////////////////////////////////////////
// Every message an app retrieves passes through here, as does every window it
// creates.
//
// Messages are intercepted with a WH_GETMESSAGE hook rather than by hooking
// GetMessage. Hooking GetMessage looks simpler, but GetMessage *blocks*: an
// idle thread parks inside the hook function, leaving a frame that belongs to
// this mod on its stack for as long as the app has nothing to do. Windhawk
// then cannot unload the mod without those threads eventually returning into
// freed memory, which crashed every process with a message loop (shell
// included) on every disable. A hook procedure only runs for the moment the
// message is handed over, so nothing of ours stays on the stack.

// Called for every message an application removes from its queue. Returns
// with the message replaced by WM_NULL if it was consumed by the mod.
void ProcessRetrievedMessage(MSG* msg) {
    ReleaseForcedLeftButton();
    RestoreHiddenAlt();
    RestoreUncoveredAfterReplay();

    bool consumed = false;

    switch (msg->message) {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            TrackAltInMove(msg);
            consumed = HandleHotkey(msg);
            break;

        case WM_KEYUP:
        case WM_SYSKEYUP:
            TrackAltInMove(msg);
            break;

        case WM_HOTKEY:
            consumed = HandleDesktopHotkey(msg);
            break;

        case WM_TIMER:
            consumed = HandleScaleTimer(msg);
            break;

        // The shortcut gets first refusal on every button: it can be bound
        // to any of them, and one bound to left or right takes that button
        // away from the drag, which is the user's business to decide.
        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
        case WM_NCLBUTTONDOWN:
        case WM_NCLBUTTONDBLCLK:
            consumed =
                HandleShortcutButton(msg) || HandleModifierButtonDown(msg, false);
            break;

        case WM_RBUTTONDOWN:
        case WM_RBUTTONDBLCLK:
        case WM_NCRBUTTONDOWN:
        case WM_NCRBUTTONDBLCLK:
            consumed =
                HandleShortcutButton(msg) || HandleModifierButtonDown(msg, true);
            break;

        case WM_MBUTTONDOWN:
        case WM_MBUTTONDBLCLK:
        case WM_NCMBUTTONDOWN:
        case WM_NCMBUTTONDBLCLK:
        case WM_XBUTTONDOWN:
        case WM_XBUTTONDBLCLK:
        case WM_NCXBUTTONDOWN:
        case WM_NCXBUTTONDBLCLK:
            consumed = HandleShortcutButton(msg);
            break;

        case WM_LBUTTONUP:
        case WM_NCLBUTTONUP:
            if (ReplayHeldPress(msg, false)) {
                break;
            }
            consumed = HandleShortcutButtonUp(msg) || HandleLeftButtonUp();
            break;

        case WM_RBUTTONUP:
        case WM_NCRBUTTONUP:
            if (ReplayHeldPress(msg, true)) {
                break;
            }
            consumed = HandleShortcutButtonUp(msg) || HandleButtonUp(true);
            break;

        case WM_MBUTTONUP:
        case WM_NCMBUTTONUP:
        case WM_XBUTTONUP:
        case WM_NCXBUTTONUP:
            consumed = HandleShortcutButtonUp(msg);
            break;

        default:
            if (!msg->hwnd) {
                break;
            }
            if (msg->message == g_msgDrag) {
                consumed = !HandleDragRequest(msg);
            } else if (msg->message == g_msgFrameless) {
                HandleFramelessRequest(msg->hwnd, msg->wParam);
                consumed = true;
            }
            break;
    }

    if (consumed) {
        msg->message = WM_NULL;
        msg->wParam = 0;
        msg->lParam = 0;
    }
}

////////////////////////////////////////////////////////////////////////////////
// The message hook, one per message-pumping thread of this process

// Two hooks per pumping thread. The first is for the messages an application
// retrieves; the second is for the ones sent to its windows, which never go
// near a queue - a window says that it has gained or lost focus by being sent
// WM_NCACTIVATE, and that is the only word the mod gets about a window it has
// not otherwise touched - and a touchpad's two fingers are about to go to
// the app by DM_POINTERHITTEST, sent too (mouse.cpp). So the second one is
// installed for either: a border color to paint with, or the mouse wheel
// scaling windows; with neither, it listens for nothing.
struct ThreadHooks {
    HHOOK getMessage = nullptr;
    HHOOK callWndProc = nullptr;
};

std::mutex g_messageHooksMutex;
std::unordered_map<DWORD, ThreadHooks> g_messageHooks;

// Set once per thread, so the common case - a thread that has its hook
// already - costs nothing. With "*" as the include pattern this runs for every
// window every application ever creates.
thread_local bool g_messageHookAttempted;

LRESULT CALLBACK GetMessageProc(int code, WPARAM wParam, LPARAM lParam) {
    ModRef ref;  // the image must not go away under this procedure

    // wParam carries the flags the caller passed to PeekMessage, which often
    // include the PM_QS_* filter bits, so it is a bitwise test. PM_NOREMOVE
    // means the app is only looking at the message; it stays in the queue and
    // we must not consume it.
    if (code == HC_ACTION && (wParam & PM_REMOVE) && lParam) {
        auto* msg = reinterpret_cast<MSG*>(lParam);
        if (!g_uninitializing) {
            ProcessRetrievedMessage(msg);
        } else if (msg->message == g_msgDrag && msg->wParam == kDragUnfade) {
            // The one request still taken during the teardown, which makes
            // it itself: a dragged window put back the way it was.
            EndDragFade(msg->hwnd);
            msg->message = WM_NULL;
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

bool CallWndProcHookWanted() {
    return BorderColorsWanted() || g_settings.wheelScale;
}

LRESULT CALLBACK CallWndProc(int code, WPARAM wParam, LPARAM lParam) {
    ModRef ref;  // the image must not go away under this procedure

    if (code == HC_ACTION && lParam && !g_uninitializing) {
        auto* sent = reinterpret_cast<CWPSTRUCT*>(lParam);
        switch (sent->message) {
            case WM_NCACTIVATE:
                OnWindowActivation(sent->hwnd, sent->wParam != FALSE);
                break;
            case WM_DWMCOLORIZATIONCOLORCHANGED:
                // The accent color moved, so a border set to "accent" follows
                // it. At once, not faded: this is not a focus change.
                if (IsBorderColorTarget(sent->hwnd)) {
                    ApplyBorderColor(sent->hwnd,
                                     GetForegroundWindow() == sent->hwnd);
                }
                break;
            case WM_NCDESTROY:
                ForgetBorderColor(sent->hwnd);
                break;
            case DM_POINTERHITTEST:
                // Two fingers on a touchpad, about to go to the app.
                KeepTouchpadFromApp(sent->hwnd, sent->wParam);
                break;
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

// Callers hold g_messageHooksMutex, which is also what keeps an installation
// from slipping past the removal at uninit.
void InstallMessageHookLocked(DWORD threadId) {
    if (g_uninitializing) {
        return;
    }
    // A thread hook on a thread of this process wants no module handle.
    ThreadHooks& hooks = g_messageHooks[threadId];
    if (!hooks.getMessage) {
        hooks.getMessage =
            SetWindowsHookExW(WH_GETMESSAGE, GetMessageProc, nullptr, threadId);
        if (!hooks.getMessage) {
            Wh_Log(L"WH_GETMESSAGE hook failed for thread %u (%u)", threadId,
                   GetLastError());
        }
    }
    if (!hooks.callWndProc && CallWndProcHookWanted()) {
        hooks.callWndProc =
            SetWindowsHookExW(WH_CALLWNDPROC, CallWndProc, nullptr, threadId);
        if (!hooks.callWndProc) {
            Wh_Log(L"WH_CALLWNDPROC hook failed for thread %u (%u)", threadId,
                   GetLastError());
        }
    }
}

void InstallMessageHookForThread() {
    if (g_messageHookAttempted) {
        return;
    }
    g_messageHookAttempted = true;

    std::lock_guard<std::mutex> lock(g_messageHooksMutex);
    // Windows reuses the IDs of threads that have ended, and File Explorer
    // ends the thread of every folder window that closes. An entry for this
    // ID can be one such thread's, whose hooks went with it - and taking it
    // for this thread's left the window with no hook at all. So whatever is
    // there is taken off and the hooks are put in afresh: taking off a dead
    // thread's hook does nothing, and one this thread was given when the mod
    // was loaded comes straight back.
    DWORD threadId = GetCurrentThreadId();
    auto it = g_messageHooks.find(threadId);
    if (it != g_messageHooks.end()) {
        if (it->second.getMessage) {
            UnhookWindowsHookEx(it->second.getMessage);
        }
        if (it->second.callWndProc) {
            UnhookWindowsHookEx(it->second.callWndProc);
        }
        g_messageHooks.erase(it);
    }
    InstallMessageHookLocked(threadId);
}

// Covers the threads that already had windows when the mod was loaded; threads
// that come later are caught when they create their first window.
BOOL CALLBACK InstallHookEnumProc(HWND hwnd, LPARAM lParam) {
    DWORD pid = 0;
    DWORD threadId = GetWindowThreadProcessId(hwnd, &pid);
    if (pid != (DWORD)lParam || !threadId) {
        return TRUE;
    }

    std::lock_guard<std::mutex> lock(g_messageHooksMutex);
    InstallMessageHookLocked(threadId);
    return TRUE;
}

void InstallMessageHooks() {
    EnumWindows(InstallHookEnumProc, (LPARAM)GetCurrentProcessId());
}

void RemoveMessageHooks() {
    std::lock_guard<std::mutex> lock(g_messageHooksMutex);
    for (const auto& [threadId, hooks] : g_messageHooks) {
        if (hooks.getMessage) {
            UnhookWindowsHookEx(hooks.getMessage);
        }
        if (hooks.callWndProc) {
            UnhookWindowsHookEx(hooks.callWndProc);
        }
    }
    g_messageHooks.clear();
}

// The settings can take the border colors and the wheel away again, and then
// there is nothing left for the hook on sent messages to listen for.
void RefreshCallWndProcHooks() {
    if (CallWndProcHookWanted()) {
        InstallMessageHooks();
        return;
    }
    std::lock_guard<std::mutex> lock(g_messageHooksMutex);
    for (auto& [threadId, hooks] : g_messageHooks) {
        if (hooks.callWndProc) {
            UnhookWindowsHookEx(hooks.callWndProc);
            hooks.callWndProc = nullptr;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Window creation hooks ("hide by default")

void OnWindowCreated(HWND hwnd, DWORD dwStyle) {
    if (!hwnd) {
        return;
    }
    // A thread that creates a window is a thread that will pump messages, so
    // this is where threads born after the mod loaded get their hook.
    InstallMessageHookForThread();
    if (!(dwStyle & WS_CHILD)) {
        StartKeyboardServerForWindow();
    }

    if ((dwStyle & WS_CHILD) || !g_settings.hideByDefault) {
        return;
    }
    if (IsAutoHideCandidate(hwnd)) {
        // Post instead of hiding right here. We are still inside the app's
        // CreateWindowEx call: the window exists but the code that owns it has
        // not run yet, and changing the frame at that point delivers a resize
        // into a half-initialized window. Some apps don't survive that - they
        // fail to start at all. Posting means the work happens once the window
        // is pumping messages, which is exactly when the hotkey path (which
        // has always worked) does it. The cost is that the title bar can be
        // visible for a frame or two first.
        RequestFrameless(hwnd, kActionAutoHide);
    }
}

CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,
                                 LPCWSTR lpClassName,
                                 LPCWSTR lpWindowName,
                                 DWORD dwStyle,
                                 int X,
                                 int Y,
                                 int nWidth,
                                 int nHeight,
                                 HWND hWndParent,
                                 HMENU hMenu,
                                 HINSTANCE hInstance,
                                 LPVOID lpParam) {
    HWND hwnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);
    OnWindowCreated(hwnd, dwStyle);
    return hwnd;
}

CreateWindowExA_t CreateWindowExA_Original;
HWND WINAPI CreateWindowExA_Hook(DWORD dwExStyle,
                                 LPCSTR lpClassName,
                                 LPCSTR lpWindowName,
                                 DWORD dwStyle,
                                 int X,
                                 int Y,
                                 int nWidth,
                                 int nHeight,
                                 HWND hWndParent,
                                 HMENU hMenu,
                                 HINSTANCE hInstance,
                                 LPVOID lpParam) {
    HWND hwnd = CreateWindowExA_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);
    OnWindowCreated(hwnd, dwStyle);
    return hwnd;
}

////////////////////////////////////////////////////////////////////////////////
// src/mod.cpp
////////////////////////////////////////////////////////////////////////////////
// Mod lifecycle: what Windhawk calls to load, reconfigure and unload us.

std::atomic<bool> g_uninitializing;
std::atomic<int> g_modRefCount;

// Every thread of the mod is started here, and its handle kept for
// Wh_ModUninit to wait on. The reference count alone is not enough: dropping
// its reference is the last thing a thread does, but it still has the rest
// of its function to return through, in the image.
std::mutex g_threadsMutex;
std::vector<HANDLE> g_threads;

bool StartModThread(LPTHREAD_START_ROUTINE proc, void* param, DWORD* threadId) {
    g_modRefCount++;  // the thread's own, dropped as it ends
    HANDLE thread = CreateThread(nullptr, 0, proc, param, 0, threadId);
    if (!thread) {
        Wh_Log(L"CreateThread failed (%u)", GetLastError());
        g_modRefCount--;
        return false;
    }
    std::lock_guard<std::mutex> lock(g_threadsMutex);
    // The ones that are done go, or a long session piles up a handle a drag.
    g_threads.erase(std::remove_if(g_threads.begin(), g_threads.end(),
                                   [](HANDLE h) {
                                       if (WaitForSingleObject(h, 0) !=
                                           WAIT_OBJECT_0) {
                                           return false;
                                       }
                                       CloseHandle(h);
                                       return true;
                                   }),
                    g_threads.end());
    g_threads.push_back(thread);
    return true;
}

void JoinModThreads() {
    std::vector<HANDLE> threads;
    {
        std::lock_guard<std::mutex> lock(g_threadsMutex);
        threads.swap(g_threads);
    }
    for (HANDLE thread : threads) {
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
    }
}

// Has the window's own thread carry out a teardown request, and waits for it.
// A subclass procedure left behind in an unmapped image crashes its
// application the next time the window gets a message, so a window that does
// not answer is tried again, and then waited for: hanging the unload is bad,
// crashing the app is worse.
void SendTeardown(HWND hwnd, UINT message, WPARAM wParam) {
    for (int attempt = 0; attempt < 3; attempt++) {
        DWORD_PTR result;
        if (SendMessageTimeoutW(hwnd, message, wParam, 0,
                                SMTO_ABORTIFHUNG | SMTO_BLOCK, 2000, &result)) {
            return;
        }
    }
    if (IsWindow(hwnd)) {
        Wh_Log(L"Waiting for %p to answer (%u)", hwnd, GetLastError());
        SendMessageW(hwnd, message, wParam, 0);
    }
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    g_msgFrameless = RegisterWindowMessageW(L"HyprlandWindows_" WH_MOD_ID);
    g_msgDrag = RegisterWindowMessageW(L"HyprlandWindowsDrag_" WH_MOD_ID);
    if (!g_msgFrameless || !g_msgDrag) {
        Wh_Log(L"RegisterWindowMessage failed");
        return FALSE;
    }

    // Both messages cross process boundaries: a UWP window, for one, is framed
    // by a window of ApplicationFrameHost.exe, which runs at a higher
    // integrity level than the app itself. Without this the UIPI message
    // filter drops the request and nothing happens at all.
    ChangeWindowMessageFilter(g_msgFrameless, MSGFLT_ADD);
    ChangeWindowMessageFilter(g_msgDrag, MSGFLT_ADD);

    LoadSettings();

    // Only short, non-blocking functions are hooked. Message interception is a
    // WH_GETMESSAGE hook instead, installed per thread below.
    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                   &CreateWindowExW_Original);
    WindhawkUtils::SetFunctionHook(CreateWindowExA, CreateWindowExA_Hook,
                                   &CreateWindowExA_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    InstallMessageHooks();
    // In the shell, the thread that keeps the Start menu shut after a Win +
    // mouse gesture and sees the key bindings first - up from the start, so it
    // is there to be found.
    StartKeyboardServer();
    // And the one that takes Win+Tab through the desktops.
    if (IsShellProcess()) {
        StartDesktopThread();
    }
    RefreshBorderColors();
    if (g_settings.hideByDefault) {
        AutoHideExistingWindows();
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"BeforeUninit: restoring title bars");

    // Before anything else, so that nothing installs a hook or subclasses a
    // window behind the teardown's back.
    g_uninitializing = true;

    // A window being dragged is put back while the hook that does it is
    // still there.
    UnfadeDraggedWindows();

    // Take our hook procedures out before the DLL goes away, and give content
    // a press was going through its region back.
    RemoveMessageHooks();
    RestoreAllUncovered();
    ShutdownKeyboardServer();
    ShutdownDesktopThread();

    // And stop the border fades before the windows get their defaults back:
    // a color written after that would stay on the window for good.
    FinishBorderFades();
    RestoreAllBorderColors();

    // A drag has a subclass of ours on its window, and one whose loop never
    // ended keeps it. Like the title bars below, it comes off on the window's
    // own thread.
    for (HWND hwnd : SnapshotSnappedWindows()) {
        SendTeardown(hwnd, g_msgDrag, kDragUnsnap);
    }
    for (HWND hwnd : SnapshotTouchpadWindows()) {
        SendTeardown(hwnd, g_msgDrag, kDragUntouch);
    }
    for (HWND hwnd : SnapshotFramelessWindows()) {
        SendTeardown(hwnd, g_msgFrameless, kActionShow);
    }
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    ChangeWindowMessageFilter(g_msgFrameless, MSGFLT_REMOVE);
    ChangeWindowMessageFilter(g_msgDrag, MSGFLT_REMOVE);

    // UnhookWindowsHookEx does not wait for a hook procedure that is running
    // on another thread, and the resize watcher is a thread of ours, so the
    // image can only be let go once both are done with it.
    while (g_modRefCount > 0) {
        Sleep(100);
    }
    JoinModThreads();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");

    bool wasHidingByDefault = g_settings.hideByDefault;
    LoadSettings();

    for (HWND hwnd : SnapshotFramelessWindows()) {
        ApplyCorners(hwnd);
    }
    // Which windows the border colors apply to is a setting of its own, so
    // this goes over all of them rather than over the frameless ones.
    RefreshCallWndProcHooks();
    RefreshBorderColors();
    DesktopSettingsChanged();

    if (g_settings.hideByDefault) {
        AutoHideExistingWindows();
    } else if (wasHidingByDefault) {
        RestoreAutoHiddenWindows();
    }
}
