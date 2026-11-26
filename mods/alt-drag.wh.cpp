// ==WindhawkMod==
// @id              alt-drag
// @name            AltDrag
// @description     Move or resize any window by holding Alt and dragging it from anywhere, without having to grab the title bar or the borders
// @version         1.2
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         *
// @compilerOptions -lcomctl32
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m417z/my-windhawk-mods

// ==WindhawkModReadme==
/*
# AltDrag

Move any window by holding Alt and dragging it from anywhere, without having to
grab the title bar. Resize any window by holding Alt and dragging it with the
right mouse button: the edge or corner which follows the mouse is the one
closest to where the drag starts. Starting the drag from the center of the
window resizes it in all directions at once, keeping its center in place.

The mouse buttons and the keys can be changed in the mod settings. A delay can
be set for each as well, so that a click keeps its usual meaning and only a
longer press starts a drag.

By default, the mod is applied for all programs. To exclude specific programs,
add them to the custom process exclusion list in the Advanced tab of this mod.

The idea was inspired by [the original AltDrag
tool](https://stefansundin.github.io/altdrag/).

## Compatibility with Slick Window Arrangement

The mod works together with the [Slick Window
Arrangement](https://windhawk.net/mods/slick-window-arrangement) mod, but note
that by default, that mod uses the Alt key to temporarily disable snapping, so
windows won't snap while being dragged with Alt. To have snapping while
dragging, change the "Keys to temporarily disable snapping" setting of Slick
Window Arrangement to a different key, or change the key used by this mod.

![Demonstration](https://i.imgur.com/PY0arDE.gif)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- moveTrigger:
  - button: left
    $name: Mouse button
    $options:
    - left: Left button
    - right: Right button
    - middle: Middle button
    - x1: X1 (back) button
    - x2: X2 (forward) button
  - ctrl: false
    $name: Ctrl
  - alt: true
    $name: Alt
  - shift: false
    $name: Shift
  - win: false
    $name: Win
  - delay: 0
    $name: Delay
    $description: >-
      The time in milliseconds to hold the mouse button before the window
      starts moving. A shorter press is passed to the window as a click.
  $name: Move trigger
  $description: >-
    The mouse button and the keys to hold for moving a window.
- sizeTrigger:
  - button: right
    $name: Mouse button
    $options:
    - left: Left button
    - right: Right button
    - middle: Middle button
    - x1: X1 (back) button
    - x2: X2 (forward) button
  - ctrl: false
    $name: Ctrl
  - alt: true
    $name: Alt
  - shift: false
    $name: Shift
  - win: false
    $name: Win
  - delay: 0
    $name: Delay
    $description: >-
      The time in milliseconds to hold the mouse button before the window
      starts resizing. A shorter press is passed to the window as a click.
  $name: Resize trigger
  $description: >-
    The mouse button and the keys to hold for resizing a window. The edge or
    corner which follows the mouse is the one closest to where the drag starts.
- centerResizeAllDirections: true
  $name: Resize from the center in all directions
  $description: >-
    Dragging from the center of the window resizes it in all directions at
    once, keeping its center in place. When disabled, a drag from the center
    resizes from the bottom right corner.
- dragWindowsWithoutTitleBar: false
  $name: Drag windows without a title bar
  $description: >-
    Also drag windows without a title bar, such as popup menus, tooltips
    and flyouts.
*/
// ==/WindhawkModSettings==

// A drag is started by taking the button press away from the target window as
// it's retrieved from the message queue, and posting a request to the root
// window, which the mod instance in the root window's thread turns into the
// system command a title bar or border drag generates. Intercepting at
// retrieval time means the window procedure never sees the press, so it
// doesn't matter how the program handles mouse input, and the system size/move
// loop provides snapping, DWM animations and maximized window handling the
// same way a title bar drag does. The root window may belong to another
// process, e.g. ApplicationFrameHost.exe hosting a UWP app's CoreWindow, so
// the request is a registered message which is allowed through the UIPI
// message filter.
//
// The loop is made for the left button: it starts only with that button down
// in the thread's synchronized key state, and ends only on WM_LBUTTONUP, which
// the release of any other button is turned into as the loop retrieves it. A
// release within the drag threshold ends it before it got going, and the
// press taken for it is then replayed as a click, simulated at the cursor, so
// that a click with the keys held keeps whatever meaning the program gives it.
// A press held for the drag delay is taken from the queue right away and
// turned into the request once the delay is over, posted back as a click if
// the button was released before that, or handed back in place of the move
// which left the drag threshold before that, the drag being the program's own.
//
// A resize from the center of the window, which moves every edge at once, has
// no system command either. The loop is run as a resize from the bottom right
// corner, and the rect it proposes with each WM_SIZING is replaced, ahead of
// the program's own handling of the message, by the one which moves the edges
// in opposite directions by the distance the pointer traveled. The loop thereby
// keeps the cancellation with Esc, the animations and the program's size
// limits. The message is sent to the window procedure, and only the window
// procedure can change it: a hook procedure may look at a message but what it
// writes to is ignored, as measured on the size loop. A subclass of the root
// window serves for the length of the drag, put on with the request, on the
// thread of the window, and taken off when the loop ends.
//
// The interception is done with a WH_GETMESSAGE hook per thread. Threads which
// retrieve messages are discovered by hooking the win32u syscall stubs, which
// every message loop goes through, including user32's internal modal loops such
// as DialogBox2 which bypass the exported PeekMessage. The stub hooks only
// install the thread hooks and then tail call the original function, so no mod
// frame is left on the stack while a thread waits for a message, which would
// otherwise prevent unloading the mod.
//
// Pointer input (WM_POINTER*, used by XAML and other mouse-in-pointer windows)
// is delivered straight to the window procedure and is never seen by
// WH_GETMESSAGE or WH_CALLWNDPROC hooks, so it's intercepted by subclassing. A
// WH_CALLWNDPROC hook subclasses the window under the pointer while a
// trigger's keys are held, and the subclass routes a contact those keys
// started to DefWindowProc, which promotes it to the legacy mouse messages the
// retrieval hook handles. A trigger with no keys is armed all the time, so it
// keeps the window under the pointer subclassed all the same.
//
// With the mouse in pointer mode a contact belongs to the window it went down
// on, and when that window's thread isn't the root window's, as with Notepad's
// text box, the loop the root window's thread runs gets none of it: it waits
// for a move that never comes, starts on a stray one once the contact is over,
// and never ends. The thread which took such a press drags the window itself,
// with SetWindowPos, from the moves the contact keeps sending it, promoted
// like the press was.
//
// Content hosted in a composition input sink, such as a WinUI XAML island,
// receives its pointer input over a side channel and produces no window message
// at all, so neither hook nor subclass sees the press, and it reaches the
// island, which turns the release into a click. Only input which hasn't been
// routed yet can be taken away from it, so while a trigger's keys are held a
// low level mouse hook swallows the press over composition hosted content,
// recognized by the class of the hosting window, and moves or resizes the
// window itself with SetWindowPos. A trigger with no keys has the process
// which owns the foreground window hold the hook instead, which keeps it to
// one hook rather than one per process; being global, that one serves a drag
// of any window. The cursor shown over such content is
// chosen by the content, on every move it sees, from a thread of its own, so
// nothing set from outside sticks. For the duration of the drag, and ahead of
// it from the end of the drag delay, a small invisible topmost window of the
// hook's thread follows the cursor instead: it receives the moves, and with
// them the right to choose the cursor. Windows
// which deliver the press as a message are left to the paths above and keep
// the system loop, which is of no use here: it retrieves no mouse input while
// the sink owns the contact, so it starts and then tracks nothing. With keys
// to hold, the hook exists only while they are, keeping it out of the input
// path the rest of the time.
//
// The hook is global, and the island of a window which isn't focused belongs to
// a process which never saw the keys go down and so has no hook of its own. The
// process which does hold the hook therefore handles any window, not just its
// own, and moves it asynchronously since it doesn't own it.
//
// A root window is dragged only if its thread runs the mod's hooks: a request
// to it would otherwise go unserved, and a program the mod isn't loaded in,
// e.g. one excluded from it, keeps its input as is, as it does for a press in
// its own windows, which no hook sees. Each hooked thread marks its top level
// windows with a window property, readable from any thread and process, and
// the low level hook checks it like the message paths do. That hook is the
// focused thread's, so while a program without the mod is focused there is
// none, and composition hosted content elsewhere gets the press as a click,
// which moves the focus there.
//
// A swallowed press never reaches the input queue, so as far as the system is
// concerned Alt, when it's among the keys, was tapped on its own, and
// DefWindowProc turns the release into SC_KEYMENU, activating the menu bar.
// The Alt release which ends such a drag is therefore taken as well. So is
// the one which ends a drag run by the pressing thread: a XAML program, whose
// text box such a press is commonly in, takes it for a lone tap of Alt and
// puts up its access keys, whatever the mouse did in between. A swallowed
// press which isn't followed by a drag is replayed on the release as a click,
// like a press taken for the loop.
//
// The Start menu, which a Win key tapped on its own opens, is put off by
// another key alone, not by a press, swallowed or not. The Win release ending
// a drag is therefore masked by a low level keyboard hook of the thread
// running the drag, the way AutoHotkey masks menu keys: it's swallowed and
// sent again behind a tap of an unassigned key.

#include <commctrl.h>

#include <algorithm>
#include <atomic>
#include <bit>
#include <cstdlib>
#include <memory>
#include <mutex>
#include <unordered_set>

enum : DWORD {
    kModifierCtrl = 1 << 0,
    kModifierAlt = 1 << 1,
    kModifierShift = 1 << 2,
    kModifierWin = 1 << 3,
};

// The button, as its virtual key, the modifier keys to hold with it, and the
// time to hold it for before the drag starts.
struct TriggerSettings {
    std::atomic<int> button;
    std::atomic<DWORD> modifiers;
    std::atomic<int> delay;
};

struct {
    TriggerSettings moveTrigger;
    TriggerSettings sizeTrigger;
    std::atomic<bool> centerResizeAllDirections;
    std::atomic<bool> dragWindowsWithoutTitleBar;
} g_settings;

std::atomic<bool> g_uninitializing;
std::atomic<int> g_hookRefCount;

// A drag is identified by the system command it amounts to: SC_MOVE |
// HTCAPTION for a move, SC_SIZE | WMSZ_* for a resize from that edge or corner.
bool IsSizeCommand(UINT command) {
    return (command & 0xFFF0) == SC_SIZE;
}

// The resize which takes hold of all the edges at once, keeping the center of
// the window in place. The system has no such command, so it's the mod's own,
// past the WMSZ_* values: the system loop runs it as a resize from the bottom
// right corner, and the rect it proposes is replaced on the way.
constexpr UINT kWmszCenter = WMSZ_BOTTOMRIGHT + 1;

bool IsCenterSizeCommand(UINT command) {
    return command == (SC_SIZE | kWmszCenter);
}

const TriggerSettings& TriggerOfCommand(UINT command) {
    return IsSizeCommand(command) ? g_settings.sizeTrigger
                                  : g_settings.moveTrigger;
}

UINT g_sizeMoveRequestMessage =
    RegisterWindowMessage(L"Windhawk_SizeMoveRequest_" WH_MOD_ID);
UINT g_unsubclassRegisteredMessage =
    RegisterWindowMessage(L"Windhawk_Unsubclass_" WH_MOD_ID);
UINT g_replayPressMessage =
    RegisterWindowMessage(L"Windhawk_ReplayPress_" WH_MOD_ID);
UINT g_dragUpdateMessage =
    RegisterWindowMessage(L"Windhawk_DragUpdate_" WH_MOD_ID);
UINT g_refreshCursorMessage =
    RegisterWindowMessage(L"Windhawk_RefreshCursor_" WH_MOD_ID);

thread_local bool g_threadHooksAttempted;
thread_local HHOOK g_getMessageHook;
thread_local HHOOK g_callWndProcHook;
std::mutex g_allThreadHooksMutex;
std::unordered_set<HHOOK> g_allThreadHooks;

// What a drag done with SetWindowPos holds the window by: the point it takes
// hold of it at, and from then on, for a move where the pointer was relative
// to the window's origin, for a resize the rect the edges follow the pointer
// from and the sizes the window accepts.
struct DragHold {
    HWND hRootWnd;
    UINT command;
    POINT startPt;
    POINT grab;
    RECT startRect;
    SIZE minSize;
    SIZE maxSize;
};

// The drag this thread was asked to run, from the request's retrieval until
// the loop is seen to be over. It's dragged once the loop got going: a move
// past the threshold wait the caption drag holds the capture through, a resize
// once the pointer leaves the drag threshold, the size loop having no such
// wait. Until then a release is a click.
thread_local struct {
    UINT command;
    int button;
    HWND hWnd;
    POINT startPt;
    bool dragged;
} g_loop;

// The drag this thread runs itself, from the press it took until the release,
// for a root window whose thread's loop is out of the contact's reach. Held
// once the pointer leaves the drag threshold. Until then a release is a click.
thread_local struct {
    int button;
    bool dragged;
    DragHold hold;
} g_threadDrag;

// The resize from the center this thread's size loop runs, from the request
// until the loop ends. It's kept apart from g_loop, which is reset by the
// release the loop is still to act on. The pointer's travel is measured from
// where the request found it, as the drag threshold is. The rect and the size
// limits are taken with the loop's first WM_SIZING, which comes with the first
// move.
thread_local struct {
    HWND hWnd;
    bool started;
    RECT startRect;
    POINT startPt;
    SIZE minSize;
    SIZE maxSize;
} g_centerResize;

// The windows with the subclass of a resize from the center, for it to be taken
// off at uninit. Each is told on its own thread, the only one it can be taken
// off from.
constexpr UINT_PTR kCenterResizeSubclassId = 1;
std::mutex g_centerResizeWindowsMutex;
std::unordered_set<HWND> g_centerResizeWindows;

// A press taken from this thread's queue for as long as the drag delay runs.
thread_local struct {
    UINT_PTR timerId;
    MSG downMsg;
    int button;
    UINT command;
    HWND hRootWnd;
} g_delayedPress;

// The button of the last press taken from this thread's queue, whose release
// is taken as well unless a loop consumes it.
thread_local int g_takenPressButton;

// Set while the press of a replayed click is posted and yet to be retrieved,
// so that it's passed on rather than taken again.
thread_local bool g_replayedPressPending;

thread_local HWND g_lastSubclassedWnd;
std::mutex g_subclassedWindowsMutex;
std::unordered_set<HWND> g_subclassedWindows;

// The pointer contact being routed to DefWindowProc, if any.
thread_local HWND g_contactWnd;
thread_local UINT g_contactPointerId;
thread_local int g_contactButton;

// The low level mouse hook, installed only while a trigger's keys are held, by
// whichever thread retrieves the press. The mutex guards the handle and the
// thread, and keeps an installation from slipping past the removal at uninit.
// It's taken from DllMain as well, under the loader lock, so nothing may call
// into the loader while holding it, which would invert the two.
std::mutex g_lowLevelMouseHookMutex;
HHOOK g_lowLevelMouseHook;
DWORD g_lowLevelMouseHookThreadId;
// Set when a press was swallowed, or taken for a drag run by its thread,
// during the current hold with Alt held.
std::atomic<bool> g_swallowedPress;

// Marks the input sent by the mod, which the hooks let through.
constexpr ULONG_PTR kOwnInputExtraInfo = 0x57484152;

// The low level keyboard hook masking the Win release which ends a drag,
// installed by the thread running the drag and removed with that release. The
// mutex carries the same rule as the one above.
std::mutex g_winKeyMaskHookMutex;
HHOOK g_winKeyMaskHook;
DWORD g_winKeyMaskHookThreadId;

// Unassigned, the key AutoHotkey masks menu keys with (A_MenuMaskKey).
constexpr WORD kMenuMaskKey = 0xE8;

// The press the low level hook swallowed, until its release: the root window
// a drag takes hold of and the drag it amounts to, a candidate for one until
// the pointer leaves the drag threshold with the delay over. Touched only on
// the hook's thread, which its callback runs on, and reset with the hook
// gone: at uninit once the callbacks are waited out, and at the start of a
// hold.
HWND g_llRootWnd;
UINT g_llCommand;
int g_llButton;
POINT g_llDownPt;
DWORD g_llDownTime;
bool g_llDragging;
// The hold of the drag, taken once the drag is set up.
DragHold g_llHold;
// Whether the work a drag starts with has been done. The callback decides
// that a drag is on, the message loop then sets it up.
bool g_llDragSetUp;
// Where the drag last saw the pointer, and whether an update for it is queued.
POINT g_llDragPt;
bool g_llDragUpdatePosted;
HWND g_llDragOverlayWnd;
// Times the drag delay of a candidate for the cursor at its end. The drag
// itself waits for a move, timed from the press. A timer is its thread's, and
// only that thread can kill it.
thread_local UINT_PTR g_llDelayTimerId;

constexpr WCHAR kDragOverlayClassName[] = L"Windhawk_AltDragOverlay_" WH_MOD_ID;

// Sized for the cursor alone: a window covering a monitor counts as a full
// screen one, which the taskbar and notifications make way for.
constexpr int kDragOverlaySize = 32;

// The monitor the pointer was last seen on, in physical coordinates and in
// those of this thread, which are scaled per monitor unless the thread is per
// monitor DPI aware.
struct {
    RECT physicalRect;
    RECT logicalRect;
} g_llMonitor;

// Windows 10 1607 and later.
using GetWindowDpiAwarenessContext_t = DPI_AWARENESS_CONTEXT(WINAPI*)(HWND);
using GetThreadDpiAwarenessContext_t = DPI_AWARENESS_CONTEXT(WINAPI*)();
using SetThreadDpiAwarenessContext_t =
    DPI_AWARENESS_CONTEXT(WINAPI*)(DPI_AWARENESS_CONTEXT);
using AreDpiAwarenessContextsEqual_t = BOOL(WINAPI*)(DPI_AWARENESS_CONTEXT,
                                                     DPI_AWARENESS_CONTEXT);
GetWindowDpiAwarenessContext_t pGetWindowDpiAwarenessContext;
GetThreadDpiAwarenessContext_t pGetThreadDpiAwarenessContext;
SetThreadDpiAwarenessContext_t pSetThreadDpiAwarenessContext;
AreDpiAwarenessContextsEqual_t pAreDpiAwarenessContextsEqual;

// The DPI awareness this thread retrieves its messages in, which their points
// are in. Hook procedures are called in the process's awareness instead, which
// differs for a thread that sets one of its own.
thread_local DPI_AWARENESS_CONTEXT g_messageLoopDpiContext;

bool BeginCenterResize(HWND hWnd, POINT startPt);
void EndCenterResize(HWND hWnd);
void ReplayPress(int button, bool asClick);
void MaskWinKeyReleaseIfNeeded(UINT command);
void TakeHold(DragHold* hold);
void DragHeldWindowTo(const DragHold& hold, POINT pt);
void OnLowLevelDragDelayElapsed();
void OnDragUpdate();
void RefreshCursorAfterOverlay();
void RemoveLowLevelMouseHook();
void InstallLowLevelMouseHookIfNeeded();
void RemoveLowLevelMouseHookIfIdle();

auto HookRefCountScope() {
    g_hookRefCount++;
    return std::unique_ptr<decltype(g_hookRefCount),
                           void (*)(decltype(g_hookRefCount)*)>{
        &g_hookRefCount, [](auto hookRefCount) { (*hookRefCount)--; }};
}

// Puts the message loop's DPI awareness in place for a hook procedure, so that
// points it gets and the window coordinates it works with agree.
auto MessageLoopDpiScope() {
    DPI_AWARENESS_CONTEXT previous = nullptr;
    if (pSetThreadDpiAwarenessContext && g_messageLoopDpiContext) {
        previous = pSetThreadDpiAwarenessContext(g_messageLoopDpiContext);
    }

    return std::unique_ptr<std::remove_pointer_t<DPI_AWARENESS_CONTEXT>,
                           void (*)(DPI_AWARENESS_CONTEXT)>{
        previous, [](DPI_AWARENESS_CONTEXT context) {
            pSetThreadDpiAwarenessContext(context);
        }};
}

void TakeMessage(MSG* msg) {
    msg->message = WM_NULL;
    msg->wParam = 0;
    msg->lParam = 0;
}

void SetSyncKeyState(int vk, bool down) {
    BYTE keyState[256];
    if (GetKeyboardState(keyState)) {
        if (down) {
            keyState[vk] |= 0x80;
        } else {
            keyState[vk] &= ~0x80;
        }
        SetKeyboardState(keyState);
    }
}

// The modifier keys held, from GetKeyState or GetAsyncKeyState.
DWORD HeldModifiers(SHORT(WINAPI* getKeyState)(int)) {
    DWORD held = 0;
    if (getKeyState(VK_CONTROL) < 0) {
        held |= kModifierCtrl;
    }
    if (getKeyState(VK_MENU) < 0) {
        held |= kModifierAlt;
    }
    if (getKeyState(VK_SHIFT) < 0) {
        held |= kModifierShift;
    }
    if (getKeyState(VK_LWIN) < 0 || getKeyState(VK_RWIN) < 0) {
        held |= kModifierWin;
    }
    return held;
}

DWORD ModifierOfKey(WPARAM vk) {
    switch (vk) {
        case VK_CONTROL:
            return kModifierCtrl;
        case VK_MENU:
            return kModifierAlt;
        case VK_SHIFT:
            return kModifierShift;
        case VK_LWIN:
        case VK_RWIN:
            return kModifierWin;
    }

    return 0;
}

// Other keys held along with the trigger's don't get in the way, so that AltGr,
// which is Ctrl+Alt, counts as Alt.
bool IsTriggerHeld(const TriggerSettings& trigger, DWORD held) {
    DWORD modifiers = trigger.modifiers;
    return (held & modifiers) == modifiers;
}

// Whether a trigger's keys are held, for the paths their hold gates. A
// trigger with no keys is never held this way: it's armed all the time, and
// what it needs is kept up for as long as it's configured.
bool IsModifierTriggerHeld(const TriggerSettings& trigger, DWORD held) {
    return trigger.modifiers && IsTriggerHeld(trigger, held);
}

bool IsAnyModifierTriggerHeld(DWORD held) {
    return IsModifierTriggerHeld(g_settings.moveTrigger, held) ||
           IsModifierTriggerHeld(g_settings.sizeTrigger, held);
}

bool IsAnyTriggerKeyless() {
    return !g_settings.moveTrigger.modifiers ||
           !g_settings.sizeTrigger.modifiers;
}

// The low level hook of a keyless trigger is held by the process which owns
// the foreground window, so that one hook is in the input path rather than one
// per process. Activation is what puts it up and takes it down, the way a key
// press and release do for a trigger which has keys.
bool ShouldHoldKeylessHook() {
    if (!IsAnyTriggerKeyless()) {
        return false;
    }

    DWORD processId = 0;
    GetWindowThreadProcessId(GetForegroundWindow(), &processId);
    return processId == GetCurrentProcessId();
}

// The button of a client or non-client button message, or 0 for any other
// message. An X button is told by the high word of wParam, or of mouseData for
// the messages of the low level hook.
int ButtonOfMessage(UINT message, WORD xButton, bool* down) {
    switch (message) {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
        case WM_NCLBUTTONDOWN:
        case WM_NCLBUTTONDBLCLK:
            *down = true;
            return VK_LBUTTON;

        case WM_LBUTTONUP:
        case WM_NCLBUTTONUP:
            *down = false;
            return VK_LBUTTON;

        case WM_RBUTTONDOWN:
        case WM_RBUTTONDBLCLK:
        case WM_NCRBUTTONDOWN:
        case WM_NCRBUTTONDBLCLK:
            *down = true;
            return VK_RBUTTON;

        case WM_RBUTTONUP:
        case WM_NCRBUTTONUP:
            *down = false;
            return VK_RBUTTON;

        case WM_MBUTTONDOWN:
        case WM_MBUTTONDBLCLK:
        case WM_NCMBUTTONDOWN:
        case WM_NCMBUTTONDBLCLK:
            *down = true;
            return VK_MBUTTON;

        case WM_MBUTTONUP:
        case WM_NCMBUTTONUP:
            *down = false;
            return VK_MBUTTON;

        case WM_XBUTTONDOWN:
        case WM_XBUTTONDBLCLK:
        case WM_NCXBUTTONDOWN:
        case WM_NCXBUTTONDBLCLK:
            *down = true;
            break;

        case WM_XBUTTONUP:
        case WM_NCXBUTTONUP:
            *down = false;
            break;

        default:
            return 0;
    }

    switch (xButton) {
        case XBUTTON1:
            return VK_XBUTTON1;
        case XBUTTON2:
            return VK_XBUTTON2;
    }

    return 0;
}

UINT ButtonUpMessageOf(UINT downMessage) {
    switch (downMessage) {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
            return WM_LBUTTONUP;
        case WM_NCLBUTTONDOWN:
        case WM_NCLBUTTONDBLCLK:
            return WM_NCLBUTTONUP;
        case WM_RBUTTONDOWN:
        case WM_RBUTTONDBLCLK:
            return WM_RBUTTONUP;
        case WM_NCRBUTTONDOWN:
        case WM_NCRBUTTONDBLCLK:
            return WM_NCRBUTTONUP;
        case WM_MBUTTONDOWN:
        case WM_MBUTTONDBLCLK:
            return WM_MBUTTONUP;
        case WM_NCMBUTTONDOWN:
        case WM_NCMBUTTONDBLCLK:
            return WM_NCMBUTTONUP;
        case WM_XBUTTONDOWN:
        case WM_XBUTTONDBLCLK:
            return WM_XBUTTONUP;
        case WM_NCXBUTTONDOWN:
        case WM_NCXBUTTONDBLCLK:
            return WM_NCXBUTTONUP;
    }

    return 0;
}

// The MK_* flag of the button in the wParam of a client mouse message.
WPARAM ButtonMask(int button) {
    switch (button) {
        case VK_LBUTTON:
            return MK_LBUTTON;
        case VK_RBUTTON:
            return MK_RBUTTON;
        case VK_MBUTTON:
            return MK_MBUTTON;
        case VK_XBUTTON1:
            return MK_XBUTTON1;
        case VK_XBUTTON2:
            return MK_XBUTTON2;
    }

    return 0;
}

bool IsNonClientMessage(UINT message) {
    return message < WM_MOUSEFIRST;
}

bool IsPastDragThreshold(POINT from, POINT to) {
    return abs(to.x - from.x) >= GetSystemMetrics(SM_CXDRAG) ||
           abs(to.y - from.y) >= GetSystemMetrics(SM_CYDRAG);
}

bool IsPointerMessage(UINT message) {
    return message >= WM_NCPOINTERUPDATE && message <= WM_POINTERROUTEDRELEASED;
}

bool IsPointerButtonDown(WPARAM wParam, int button) {
    switch (button) {
        case VK_LBUTTON:
            return IS_POINTER_FIRSTBUTTON_WPARAM(wParam);
        case VK_RBUTTON:
            return IS_POINTER_SECONDBUTTON_WPARAM(wParam);
        case VK_MBUTTON:
            return IS_POINTER_THIRDBUTTON_WPARAM(wParam);
        case VK_XBUTTON1:
            return IS_POINTER_FOURTHBUTTON_WPARAM(wParam);
        case VK_XBUTTON2:
            return IS_POINTER_FIFTHBUTTON_WPARAM(wParam);
    }

    return false;
}

// The button a new contact was made with, or 0 for any other message. The
// mouse is in contact while any of its buttons is down, so at that point it's
// the only one.
int ButtonOfPointerDownMessage(UINT message, WPARAM wParam) {
    switch (message) {
        case WM_POINTERDOWN:
        case WM_NCPOINTERDOWN:
            for (int button : {VK_LBUTTON, VK_RBUTTON, VK_MBUTTON, VK_XBUTTON1,
                               VK_XBUTTON2}) {
                if (IsPointerButtonDown(wParam, button)) {
                    return button;
                }
            }
            break;
    }

    return 0;
}

bool IsPointerContactEndMessage(UINT message) {
    switch (message) {
        case WM_POINTERUP:
        case WM_NCPOINTERUP:
        case WM_POINTERCAPTURECHANGED:
            return true;
    }

    return false;
}

// A message showing that a contact is no longer this window's: the pointer
// left, e.g. captured by the move loop, or the button is up already, the
// release having gone elsewhere.
bool IsPointerContactGoneMessage(UINT message, WPARAM wParam, int button) {
    switch (message) {
        case WM_POINTERLEAVE:
            return true;

        case WM_POINTERUPDATE:
        case WM_NCPOINTERUPDATE:
            return !IsPointerButtonDown(wParam, button);
    }

    return false;
}

// Set on the top level windows of a thread which runs the mod's hooks.
constexpr WCHAR kHookedWindowProp[] = L"Windhawk_Hooked_" WH_MOD_ID;

void MarkHookedWindow(HWND hWnd) {
    if (!SetProp(hWnd, kHookedWindowProp, (HANDLE)1)) {
        Wh_Log(L"SetProp error for %08X: %u", (DWORD)(ULONG_PTR)hWnd,
               GetLastError());
    }
}

// This thread's hooks are in place, another thread's are told by the mark.
bool IsRootWindowHooked(HWND hRootWnd) {
    return GetWindowThreadProcessId(hRootWnd, nullptr) ==
               GetCurrentThreadId() ||
           GetProp(hRootWnd, kHookedWindowProp);
}

bool IsExcludedRootWindow(HWND hRootWnd) {
    // A window of a thread without the mod's hooks, e.g. of a process the mod
    // isn't loaded in, keeps its input as is.
    if (!IsRootWindowHooked(hRootWnd)) {
        return true;
    }

    WCHAR className[64];
    if (!GetClassName(hRootWnd, className, ARRAYSIZE(className))) {
        return true;
    }

    // The taskbar, the desktop, and the mod's own drag overlay.
    return _wcsicmp(className, L"Shell_TrayWnd") == 0 ||
           _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0 ||
           _wcsicmp(className, L"Progman") == 0 ||
           _wcsicmp(className, L"WorkerW") == 0 ||
           _wcsicmp(className, kDragOverlayClassName) == 0;
}

bool CanMoveRootWindow(HWND hRootWnd) {
    if (IsExcludedRootWindow(hRootWnd)) {
        return false;
    }

    if (g_settings.dragWindowsWithoutTitleBar) {
        return true;
    }

    LONG style = GetWindowLong(hRootWnd, GWL_STYLE);
    return (style & WS_CAPTION) == WS_CAPTION;
}

bool CanSizeRootWindow(HWND hRootWnd) {
    if (IsExcludedRootWindow(hRootWnd)) {
        return false;
    }

    LONG style = GetWindowLong(hRootWnd, GWL_STYLE);
    return (style & WS_THICKFRAME) && !(style & WS_MAXIMIZE);
}

// The edge or corner a resize which grabs the window at pt takes hold of, from
// a 3x3 grid over the window. The center cell resizes in all directions, or
// from the bottom right corner if that's what the settings ask for.
UINT SizeCommandAtPoint(HWND hRootWnd, POINT pt) {
    RECT rc;
    if (!GetWindowRect(hRootWnd, &rc) || IsRectEmpty(&rc)) {
        return SC_SIZE | WMSZ_BOTTOMRIGHT;
    }

    int col =
        std::clamp<int>((pt.x - rc.left) * 3 / (rc.right - rc.left), 0, 2);
    int row = std::clamp<int>((pt.y - rc.top) * 3 / (rc.bottom - rc.top), 0, 2);

    static constexpr UINT kEdges[3][3] = {
        {WMSZ_TOPLEFT, WMSZ_TOP, WMSZ_TOPRIGHT},
        {WMSZ_LEFT, kWmszCenter, WMSZ_RIGHT},
        {WMSZ_BOTTOMLEFT, WMSZ_BOTTOM, WMSZ_BOTTOMRIGHT},
    };
    UINT edge = kEdges[row][col];
    if (edge == kWmszCenter && !g_settings.centerResizeAllDirections) {
        edge = WMSZ_BOTTOMRIGHT;
    }
    return SC_SIZE | edge;
}

// The drag a press of the button starts, given the modifier keys held, or 0.
// When the press fits both triggers, the one with more keys is the closer fit.
UINT DragCommandForPress(int button, DWORD held, HWND hRootWnd, POINT pt) {
    bool move = button == g_settings.moveTrigger.button &&
                IsTriggerHeld(g_settings.moveTrigger, held) &&
                CanMoveRootWindow(hRootWnd);
    bool size = button == g_settings.sizeTrigger.button &&
                IsTriggerHeld(g_settings.sizeTrigger, held) &&
                CanSizeRootWindow(hRootWnd);
    if (move && size &&
        std::popcount<DWORD>(g_settings.sizeTrigger.modifiers) >
            std::popcount<DWORD>(g_settings.moveTrigger.modifiers)) {
        move = false;
    }

    if (move) {
        return SC_MOVE | HTCAPTION;
    }

    if (size) {
        return SizeCommandAtPoint(hRootWnd, pt);
    }

    return 0;
}

PCWSTR CursorOfCommand(UINT command) {
    switch (command) {
        case SC_SIZE | WMSZ_LEFT:
        case SC_SIZE | WMSZ_RIGHT:
            return IDC_SIZEWE;
        case SC_SIZE | WMSZ_TOP:
        case SC_SIZE | WMSZ_BOTTOM:
            return IDC_SIZENS;
        case SC_SIZE | WMSZ_TOPLEFT:
        case SC_SIZE | WMSZ_BOTTOMRIGHT:
            return IDC_SIZENWSE;
        case SC_SIZE | WMSZ_TOPRIGHT:
        case SC_SIZE | WMSZ_BOTTOMLEFT:
            return IDC_SIZENESW;
    }

    return IDC_SIZEALL;
}

PCWSTR NameOfCommand(UINT command) {
    return IsSizeCommand(command) ? L"resize" : L"move";
}

// The cursor this thread showed before a drag set its own. A program which
// keeps the cursor it last set, e.g. a XAML one, doesn't set it again until its
// own state changes, so the drag's cursor would stay up after the drag, until
// the pointer left the window. Put back at each end of a drag, on the thread
// which set it.
thread_local struct {
    bool saved;
    HCURSOR cursor;
} g_cursorBeforeDrag;

void ShowDragCursor(UINT command) {
    HCURSOR previous = SetCursor(LoadCursor(nullptr, CursorOfCommand(command)));
    if (!g_cursorBeforeDrag.saved) {
        g_cursorBeforeDrag = {.saved = true, .cursor = previous};
    }
}

void RestoreCursorAfterDrag() {
    if (g_cursorBeforeDrag.saved) {
        SetCursor(g_cursorBeforeDrag.cursor);
        g_cursorBeforeDrag = {};
    }
}

// Windows which host their content in a composition input sink, such as a WinUI
// XAML island. Their pointer input never becomes a window message.
bool IsCompositionHostedWindow(HWND hWnd) {
    WCHAR className[64];
    if (!GetClassName(hWnd, className, ARRAYSIZE(className))) {
        return false;
    }

    return _wcsnicmp(className, L"Microsoft.UI.Content.",
                     ARRAYSIZE(L"Microsoft.UI.Content.") - 1) == 0 ||
           _wcsnicmp(className, L"Windows.UI.Composition.",
                     ARRAYSIZE(L"Windows.UI.Composition.") - 1) == 0 ||
           _wcsnicmp(className, L"Windows.UI.Input.InputSite.",
                     ARRAYSIZE(L"Windows.UI.Input.InputSite.") - 1) == 0 ||
           _wcsicmp(className, L"InputSiteWindowClass") == 0;
}

// Found by geometry alone, from the desktop window down. WindowFromPoint sends
// WM_NCHITTEST to windows of other threads, and from the low level hook a
// program which stopped responding a moment ago would hold up the mouse
// system wide. An island host commonly answers that message with HTTRANSPARENT
// anyway, to do its own hit testing, which would stop the lookup at the host.
// What geometry misses is per pixel transparency of layered windows, behind
// which nothing is found. A disabled island, e.g. Notepad's status bar, is
// found like an enabled one: the press over it reaches nothing the message
// paths see.
HWND CompositionHostedWindowFromPoint(POINT pt) {
    HWND hWnd = GetDesktopWindow();
    for (int depth = 0; depth < 9; depth++) {
        POINT clientPt = pt;
        if (!ScreenToClient(hWnd, &clientPt)) {
            break;
        }

        HWND hChildWnd = ChildWindowFromPointEx(
            hWnd, clientPt, CWP_SKIPINVISIBLE | CWP_SKIPTRANSPARENT);
        if (!hChildWnd || hChildWnd == hWnd) {
            break;
        }

        hWnd = hChildWnd;
        if (IsCompositionHostedWindow(hWnd)) {
            return hWnd;
        }
    }

    return nullptr;
}

// Whether the system size/move loop is running on this thread.
bool IsInMoveLoop() {
    GUITHREADINFO gti{.cbSize = sizeof(gti)};
    return GetGUIThreadInfo(GetCurrentThreadId(), &gti) &&
           (gti.flags & GUI_INMOVESIZE);
}

// Posted messages are retrieved before input, so a button release that's
// already queued is seen by the loop rather than by the program.
bool PostSizeMoveRequest(HWND hRootWnd, UINT command, int button, POINT pt) {
    if (!PostMessage(hRootWnd, g_sizeMoveRequestMessage,
                     MAKEWPARAM(command, button), MAKELPARAM(pt.x, pt.y))) {
        Wh_Log(L"PostMessage error: %u", GetLastError());
        return false;
    }

    return true;
}

// Whether the loop the root window's thread would run gets the input of a
// press this thread retrieved. With the mouse in pointer mode the contact
// belongs to the window it went down on, and a loop on another thread sees
// neither its moves nor its release. A root window of another process keeps
// the request: it's let through from a lower integrity level, which
// SetWindowPos isn't.
bool WouldLoopGetContact(HWND hRootWnd) {
    DWORD processId = 0;
    DWORD threadId = GetWindowThreadProcessId(hRootWnd, &processId);
    return threadId == GetCurrentThreadId() ||
           processId != GetCurrentProcessId() || !IsMouseInPointerEnabled();
}

// A request carries the command, the button and the window from another
// process, and the command is handed to DefWindowProc, so each is held to
// what the mod itself asks for. The filter which lets the message through
// from any integrity level is meant for a drag and for nothing else: any
// other command, SC_CLOSE or one of the program's own among them, would
// otherwise be the sender's to send.
bool IsRequestAcceptable(UINT command, int button, HWND hRootWnd) {
    if (!ButtonMask(button) || GetAncestor(hRootWnd, GA_ROOT) != hRootWnd) {
        return false;
    }

    if (command == (SC_MOVE | HTCAPTION)) {
        return CanMoveRootWindow(hRootWnd);
    }

    UINT edge = command & 0xF;
    return IsSizeCommand(command) && edge >= WMSZ_LEFT &&
           edge <= kWmszCenter && CanSizeRootWindow(hRootWnd);
}

// Turns a request retrieved by the root window's thread into the system
// command. The loop only starts if this thread's synchronized state has the
// left button down: it isn't when the press was retrieved by another thread,
// and it's the left button which counts whichever button is really held.
void OnSizeMoveRequestRemoved(MSG* msg) {
    UINT command = LOWORD(msg->wParam);
    int button = HIWORD(msg->wParam);

    if (!IsRequestAcceptable(command, button, msg->hwnd)) {
        Wh_Log(L"Dropping the request %04X %02X for %08X", command, button,
               (DWORD)(ULONG_PTR)msg->hwnd);
        TakeMessage(msg);
        return;
    }

    if (GetAsyncKeyState(button) >= 0) {
        // Released already, a loop would stick to the cursor, and short of a
        // held button nothing backs the request.
        Wh_Log(L"Request to %s %08X, button already released",
               NameOfCommand(command), (DWORD)(ULONG_PTR)msg->hwnd);
        TakeMessage(msg);
        return;
    }

    if (GetKeyState(VK_LBUTTON) >= 0) {
        SetSyncKeyState(VK_LBUTTON, true);
    }

    Wh_Log(L"Request to %s %08X, starting the loop", NameOfCommand(command),
           (DWORD)(ULONG_PTR)msg->hwnd);

    // The loop knows no resize from the center: it runs a resize from the
    // bottom right corner, which the subclass turns into the one asked for. If
    // there is no subclass, the corner is all there is.
    UINT systemCommand = command;
    UINT loopCommand = command;
    if (IsCenterSizeCommand(command)) {
        systemCommand = SC_SIZE | WMSZ_BOTTOMRIGHT;
        if (!BeginCenterResize(msg->hwnd, msg->pt)) {
            loopCommand = systemCommand;
        }
    }

    g_loop = {
        .command = loopCommand,
        .button = button,
        .hWnd = msg->hwnd,
        .startPt = msg->pt,
    };

    // Shown from the start, by the thread which puts the cursor back at the
    // end, and saved ahead of the cursors the loop sets.
    ShowDragCursor(g_loop.command);

    msg->message = WM_SYSCOMMAND;
    msg->wParam = systemCommand;
}

// Clears the loop this thread was asked to run, and puts back the cursor the
// drag replaced.
void EndLoop() {
    g_loop = {};
    RestoreCursorAfterDrag();
}

// Runs for the messages the loop retrieves, and ahead of it the wait for the
// drag threshold, which inLoop tells apart.
void OnLoopMessageRemoved(MSG* msg, bool inLoop) {
    if (!g_loop.dragged) {
        // A resize starts the loop with GUI_INMOVESIZE already set, so the
        // pointer leaving the drag threshold is what tells it apart from a
        // click; a move waits at the threshold and sets GUI_INMOVESIZE then.
        bool started = IsSizeCommand(g_loop.command)
                           ? IsPastDragThreshold(g_loop.startPt, msg->pt)
                           : inLoop;
        if (started) {
            g_loop.dragged = true;
            MaskWinKeyReleaseIfNeeded(g_loop.command);
        }
    }

    if (msg->message == WM_MOUSEMOVE) {
        // The loop retrieves the moves itself, without dispatching them,
        // holds an internal capture for which no WM_SETCURSOR is sent, and
        // shows the class cursor.
        ShowDragCursor(g_loop.command);
        return;
    }

    bool down;
    int button = ButtonOfMessage(msg->message, HIWORD(msg->wParam), &down);
    if (button != g_loop.button || down) {
        return;
    }

    if (button != VK_LBUTTON) {
        Wh_Log(L"Ending the loop with the release of button %02X", button);
        if (IsNonClientMessage(msg->message)) {
            msg->message = WM_NCLBUTTONUP;
        } else {
            msg->message = WM_LBUTTONUP;
            msg->wParam = 0;
        }

        if (GetAsyncKeyState(VK_LBUTTON) >= 0) {
            SetSyncKeyState(VK_LBUTTON, false);
        }
    }

    if (!g_loop.dragged) {
        Wh_Log(L"Released within the drag threshold, replaying the click");
        ReplayPress(button, true);
    }

    // The loop consumes the release.
    EndLoop();
    g_takenPressButton = 0;
}

// The loop ended without its release being seen, e.g. cancelled with Esc.
void OnLoopOver() {
    if (GetAsyncKeyState(VK_LBUTTON) >= 0 && GetKeyState(VK_LBUTTON) < 0) {
        SetSyncKeyState(VK_LBUTTON, false);
    }

    EndLoop();
    EndCenterResize(g_centerResize.hWnd);
}

// Clears the drag this thread runs itself, and puts back the cursor the drag
// replaced.
void EndThreadDrag() {
    g_threadDrag = {};
    RestoreCursorAfterDrag();
}

// Runs for the messages retrieved while this thread holds a drag: the moves
// and the release of the contact, promoted like the press. Returns whether the
// drag is still on, with the message either taken or passed on untouched.
bool OnThreadDragMessageRemoved(MSG* msg) {
    const DragHold& hold = g_threadDrag.hold;

    if (msg->message == WM_MOUSEMOVE || msg->message == WM_NCMOUSEMOVE) {
        if (msg->message == WM_MOUSEMOVE &&
            !(msg->wParam & ButtonMask(g_threadDrag.button))) {
            // The button is up already, the release having gone elsewhere.
            Wh_Log(L"Moved with the button up, ending the %s",
                   NameOfCommand(hold.command));
            EndThreadDrag();
            return false;
        }

        if (!g_threadDrag.dragged &&
            IsPastDragThreshold(hold.startPt, msg->pt)) {
            Wh_Log(L"Starting the %s of root window %08X",
                   NameOfCommand(hold.command),
                   (DWORD)(ULONG_PTR)hold.hRootWnd);
            g_threadDrag.dragged = true;
            TakeHold(&g_threadDrag.hold);
            MaskWinKeyReleaseIfNeeded(hold.command);
        }

        if (g_threadDrag.dragged) {
            DragHeldWindowTo(hold, msg->pt);
        }

        ShowDragCursor(hold.command);
        TakeMessage(msg);
        return true;
    }

    bool down;
    int button = ButtonOfMessage(msg->message, HIWORD(msg->wParam), &down);
    if (button != g_threadDrag.button) {
        return true;
    }

    if (down) {
        // A press of the drag's button again means its release was missed.
        EndThreadDrag();
        return false;
    }

    if (!g_threadDrag.dragged) {
        Wh_Log(L"Released within the drag threshold, replaying the click");
        ReplayPress(button, true);
    }

    EndThreadDrag();
    g_takenPressButton = 0;
    TakeMessage(msg);
    return true;
}

// Starts the drag of a press this thread took: at the root window's thread,
// or here when its loop would get no input.
bool StartDrag(HWND hRootWnd, UINT command, int button, POINT pt) {
    if (WouldLoopGetContact(hRootWnd)) {
        return PostSizeMoveRequest(hRootWnd, command, button, pt);
    }

    Wh_Log(L"Running the %s of root window %08X on this thread",
           NameOfCommand(command), (DWORD)(ULONG_PTR)hRootWnd);

    g_cursorBeforeDrag = {};

    g_threadDrag = {
        .button = button,
        .hold = {.hRootWnd = hRootWnd, .command = command, .startPt = pt},
    };
    if (GetAsyncKeyState(VK_MENU) < 0) {
        g_swallowedPress = true;
    }

    return true;
}

// Posts the press taken for the drag delay back to its window, followed by a
// release, as the click it turned out to be.
void ReplayDelayedPress() {
    const MSG& down = g_delayedPress.downMsg;
    WPARAM upWParam = IsNonClientMessage(down.message)
                          ? down.wParam
                          : down.wParam & ~ButtonMask(g_delayedPress.button);

    Wh_Log(L"Replaying the press %04X for %08X as a click", down.message,
           (DWORD)(ULONG_PTR)down.hwnd);

    g_replayedPressPending = true;
    if (!PostMessage(down.hwnd, down.message, down.wParam, down.lParam)) {
        Wh_Log(L"PostMessage error: %u", GetLastError());
        g_replayedPressPending = false;
        return;
    }

    PostMessage(down.hwnd, ButtonUpMessageOf(down.message), upWParam,
                down.lParam);
}

void EndDragDelay() {
    KillTimer(nullptr, g_delayedPress.timerId);
    g_delayedPress = {};
}

// The mouse left the drag threshold before the delay was over: the drag is
// the program's own, e.g. a selection, so the press is handed back in place
// of this move, ahead of the moves which follow, and its release is left
// alone.
void HandDelayedPressBack(MSG* msg) {
    Wh_Log(
        L"Moved before the drag delay elapsed, handing the press %04X for "
        L"%08X back",
        g_delayedPress.downMsg.message,
        (DWORD)(ULONG_PTR)g_delayedPress.downMsg.hwnd);

    *msg = g_delayedPress.downMsg;
    g_takenPressButton = 0;
    EndDragDelay();
}

bool StartDragDelay(const MSG* downMsg,
                    int button,
                    UINT command,
                    HWND hRootWnd,
                    int delay) {
    UINT_PTR timerId = SetTimer(nullptr, 0, delay, nullptr);
    if (!timerId) {
        Wh_Log(L"SetTimer error: %u", GetLastError());
        return false;
    }

    Wh_Log(L"Message %04X for %08X, holding the press for %d ms",
           downMsg->message, (DWORD)(ULONG_PTR)downMsg->hwnd, delay);

    g_delayedPress = {
        .timerId = timerId,
        .downMsg = *downMsg,
        .button = button,
        .command = command,
        .hRootWnd = hRootWnd,
    };
    return true;
}

void OnDragDelayElapsed() {
    if (GetAsyncKeyState(g_delayedPress.button) < 0 &&
        StartDrag(g_delayedPress.hRootWnd, g_delayedPress.command,
                  g_delayedPress.button, g_delayedPress.downMsg.pt)) {
        Wh_Log(L"Drag delay elapsed, starting a %s of root window %08X",
               NameOfCommand(g_delayedPress.command),
               (DWORD)(ULONG_PTR)g_delayedPress.hRootWnd);

        // Shown from here on, ahead of the first move, for a drag this thread
        // runs: the window under the cursor is this thread's, which retrieved
        // the press. A loop shows it as it starts, on its own thread.
        if (g_threadDrag.button) {
            ShowDragCursor(g_delayedPress.command);
        }
    } else {
        ReplayDelayedPress();
    }

    EndDragDelay();
}

// Tracks the modifier keys so the low level mouse hook exists only while a
// trigger's keys are held.
void OnModifierKeyMessageRemoved(MSG* msg, DWORD modifier) {
    bool down = msg->message == WM_KEYDOWN || msg->message == WM_SYSKEYDOWN;

    // GetKeyState reflects the state at the time of the retrieved message.
    DWORD held = HeldModifiers(GetKeyState);
    bool triggerHeld = IsAnyModifierTriggerHeld(held);

    if (down) {
        // Bit 30 of lParam is set for the auto repeats which arrive while
        // the key is held, including throughout a drag.
        constexpr LPARAM kPreviousKeyStateDown = 1 << 30;
        if (!(msg->lParam & kPreviousKeyStateDown) && triggerHeld &&
            !IsAnyModifierTriggerHeld(held & ~modifier)) {
            // A hold starts with this key. A keyless trigger keeps its own
            // hook, and with it whatever drag is under way.
            g_swallowedPress = false;
            if (!IsAnyTriggerKeyless()) {
                RemoveLowLevelMouseHook();
            }
        }
    }

    if (triggerHeld) {
        InstallLowLevelMouseHookIfNeeded();
    } else {
        RemoveLowLevelMouseHookIfIdle();
    }

    if (!down && msg->wParam == VK_MENU && g_swallowedPress.exchange(false)) {
        Wh_Log(L"Swallowing the Alt release of a drag");
        TakeMessage(msg);
    }
}

// Runs for every message removed from the queue of the current thread, before
// the program sees it.
void OnMessageRemoved(MSG* msg) {
    if (msg->message == g_replayPressMessage) {
        ReplayPress((int)msg->wParam, msg->lParam != 0);
        TakeMessage(msg);
        return;
    }

    if (msg->message == g_dragUpdateMessage) {
        OnDragUpdate();
        TakeMessage(msg);
        return;
    }

    if (msg->message == g_refreshCursorMessage) {
        RefreshCursorAfterOverlay();
        TakeMessage(msg);
        return;
    }

    if (msg->message == WM_TIMER && !msg->hwnd) {
        if (g_delayedPress.timerId && msg->wParam == g_delayedPress.timerId) {
            OnDragDelayElapsed();
            TakeMessage(msg);
            return;
        }

        if (g_llDelayTimerId && msg->wParam == g_llDelayTimerId) {
            OnLowLevelDragDelayElapsed();
            TakeMessage(msg);
            return;
        }
    }

    if (!msg->hwnd) {
        return;
    }

    if (msg->message == g_sizeMoveRequestMessage) {
        OnSizeMoveRequestRemoved(msg);
        return;
    }

    if (msg->message == WM_SYSKEYDOWN || msg->message == WM_KEYDOWN ||
        msg->message == WM_SYSKEYUP || msg->message == WM_KEYUP) {
        if (DWORD modifier = ModifierOfKey(msg->wParam)) {
            OnModifierKeyMessageRemoved(msg, modifier);
        }
        return;
    }

    if (g_loop.command) {
        // Ahead of the loop is the wait for the drag threshold, during which
        // the window holds the capture.
        bool inLoop = IsInMoveLoop();
        if (inLoop || GetCapture() == g_loop.hWnd) {
            OnLoopMessageRemoved(msg, inLoop);
            return;
        }

        OnLoopOver();
    }

    if (g_threadDrag.button && OnThreadDragMessageRemoved(msg)) {
        return;
    }

    if (g_delayedPress.timerId &&
        (msg->message == WM_MOUSEMOVE || msg->message == WM_NCMOUSEMOVE) &&
        IsPastDragThreshold(g_delayedPress.downMsg.pt, msg->pt)) {
        HandDelayedPressBack(msg);
        return;
    }

    bool down;
    int button = ButtonOfMessage(msg->message, HIWORD(msg->wParam), &down);
    if (!button) {
        return;
    }

    if (g_delayedPress.timerId) {
        if (button == g_delayedPress.button && !down) {
            Wh_Log(L"Released before the drag delay elapsed");
            ReplayDelayedPress();
            EndDragDelay();
            TakeMessage(msg);
        }
        return;
    }

    if (!down) {
        if (button == g_takenPressButton) {
            g_takenPressButton = 0;
            Wh_Log(
                L"Message %04X for %08X, taking the release of a taken press",
                msg->message, (DWORD)(ULONG_PTR)msg->hwnd);
            TakeMessage(msg);
        }
        return;
    }

    // A new press: the release of the last one, if it's still awaited, went
    // elsewhere.
    if (button == g_takenPressButton) {
        g_takenPressButton = 0;
    }

    if (g_replayedPressPending) {
        g_replayedPressPending = false;
        return;
    }

    // The extra info of a retrieved input message is the thread's until the
    // next one.
    if (GetMessageExtraInfo() == kOwnInputExtraInfo) {
        return;
    }

    HWND hRootWnd = GetAncestor(msg->hwnd, GA_ROOT);
    if (!hRootWnd) {
        return;
    }

    UINT command = DragCommandForPress(button, HeldModifiers(GetKeyState),
                                       hRootWnd, msg->pt);
    if (!command) {
        return;
    }

    g_takenPressButton = button;

    // A press on the title bar or the border isn't held: DefWindowProc
    // answers such a press, when replayed, with a drag wait of its own which
    // only a client release ends.
    int delay = TriggerOfCommand(command).delay;
    if (delay > 0 && !IsNonClientMessage(msg->message) &&
        StartDragDelay(msg, button, command, hRootWnd, delay)) {
        TakeMessage(msg);
        return;
    }

    Wh_Log(L"Message %04X for %08X, starting a %s of root window %08X",
           msg->message, (DWORD)(ULONG_PTR)msg->hwnd, NameOfCommand(command),
           (DWORD)(ULONG_PTR)hRootWnd);

    if (!StartDrag(hRootWnd, command, button, msg->pt)) {
        g_takenPressButton = 0;
        return;
    }

    TakeMessage(msg);
}

LRESULT CALLBACK GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam) {
    auto hookScope = HookRefCountScope();

    if (nCode == HC_ACTION && (wParam & PM_REMOVE) && lParam &&
        !g_uninitializing) {
        auto dpiScope = MessageLoopDpiScope();
        OnMessageRemoved((MSG*)lParam);
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

void UnsubclassWindow(HWND hWnd);

LRESULT CALLBACK SubclassProc(HWND hWnd,
                              UINT uMsg,
                              WPARAM wParam,
                              LPARAM lParam,
                              UINT_PTR uIdSubclass,
                              DWORD_PTR dwRefData) {
    auto hookScope = HookRefCountScope();

    if (uMsg == WM_NCDESTROY) {
        if (hWnd == g_contactWnd) {
            g_contactWnd = nullptr;
        }

        UnsubclassWindow(hWnd);
    } else if (uMsg == g_unsubclassRegisteredMessage) {
        UnsubclassWindow(hWnd);
        return 0;
    } else if (!IsAnyTriggerKeyless() &&
               !IsAnyModifierTriggerHeld(HeldModifiers(GetAsyncKeyState)) &&
               hWnd != g_contactWnd) {
        // Needed only while a trigger's keys are held or a contact is being
        // routed. The hit test preceding the next press puts it back.
        UnsubclassWindow(hWnd);
    } else if (uMsg == WM_SETCURSOR && g_threadDrag.button) {
        // Asked of the window under the pointer ahead of each promoted move.
        ShowDragCursor(g_threadDrag.hold.command);
        return TRUE;
    } else if (IsPointerMessage(uMsg)) {
        UINT pointerId = GET_POINTERID_WPARAM(wParam);
        bool tracked = hWnd == g_contactWnd && pointerId == g_contactPointerId;

        if (int button = ButtonOfPointerDownMessage(uMsg, wParam)) {
            // A new contact, the mouse reuses its pointer id for every one.
            if (tracked) {
                g_contactWnd = nullptr;
            }

            HWND hRootWnd = GetAncestor(hWnd, GA_ROOT);
            POINT pt{(short)LOWORD(lParam), (short)HIWORD(lParam)};
            if (hRootWnd && !g_uninitializing &&
                DragCommandForPress(button, HeldModifiers(GetAsyncKeyState),
                                    hRootWnd, pt)) {
                Wh_Log(
                    L"Message %04X for %08X, routing pointer %u to "
                    L"DefWindowProc",
                    uMsg, (DWORD)(ULONG_PTR)hWnd, pointerId);
                g_contactWnd = hWnd;
                g_contactPointerId = pointerId;
                g_contactButton = button;
                return DefWindowProc(hWnd, uMsg, wParam, lParam);
            }
        } else if (tracked &&
                   IsPointerContactGoneMessage(uMsg, wParam, g_contactButton)) {
            // The program saw the pointer enter, so it gets to see this.
            g_contactWnd = nullptr;
        } else if (tracked) {
            if (IsPointerContactEndMessage(uMsg)) {
                g_contactWnd = nullptr;
            }

            return DefWindowProc(hWnd, uMsg, wParam, lParam);
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

void UnsubclassWindow(HWND hWnd) {
    RemoveWindowSubclass(hWnd, SubclassProc, 0);

    if (hWnd == g_lastSubclassedWnd) {
        g_lastSubclassedWnd = nullptr;
    }

    std::lock_guard<std::mutex> guard(g_subclassedWindowsMutex);
    g_subclassedWindows.erase(hWnd);
}

void SubclassWindowIfNeeded(HWND hWnd) {
    if (hWnd == g_lastSubclassedWnd) {
        return;
    }

    HWND hRootWnd = GetAncestor(hWnd, GA_ROOT);
    if (!hRootWnd ||
        (!CanMoveRootWindow(hRootWnd) && !CanSizeRootWindow(hRootWnd))) {
        return;
    }

    std::lock_guard<std::mutex> guard(g_subclassedWindowsMutex);
    if (g_uninitializing) {
        return;
    }

    if (!g_subclassedWindows.contains(hWnd)) {
        if (!SetWindowSubclass(hWnd, SubclassProc, 0, 0)) {
            Wh_Log(L"SetWindowSubclass error for %08X", (DWORD)(ULONG_PTR)hWnd);
            return;
        }

        g_subclassedWindows.insert(hWnd);
    }

    g_lastSubclassedWnd = hWnd;
}

// The rect of a window resized from its center: the two edges of an axis move
// by the pointer's travel since the drag started, one each way, so the center
// stays in place. A size past the limits is held to them, and the window stays
// centered where it stops instead of being pinned to one side.
RECT CenterResizedRect(const RECT& rcStart,
                       POINT ptStart,
                       POINT pt,
                       SIZE minSize,
                       SIZE maxSize) {
    auto resizeAxis = [](LONG start, LONG end, LONG travel, LONG minLength,
                         LONG maxLength, LONG* newStart, LONG* newEnd) {
        LONG length = end - start + 2 * travel;
        LONG clamped =
            std::clamp(length, minLength, std::max(minLength, maxLength));
        *newStart = start - travel + (length - clamped) / 2;
        *newEnd = *newStart + clamped;
    };

    RECT rc;
    resizeAxis(rcStart.left, rcStart.right, pt.x - ptStart.x, minSize.cx,
               maxSize.cx, &rc.left, &rc.right);
    resizeAxis(rcStart.top, rcStart.bottom, pt.y - ptStart.y, minSize.cy,
               maxSize.cy, &rc.top, &rc.bottom);
    return rc;
}

// The sizes the window accepts, asked of it the way the system does for a drag:
// the defaults are offered for the window to change. Bounded, the window being
// possibly of another thread or process.
void GetSizeLimits(HWND hWnd, SIZE* minSize, SIZE* maxSize) {
    MINMAXINFO defaults{};
    defaults.ptMaxSize = {GetSystemMetrics(SM_CXMAXIMIZED),
                          GetSystemMetrics(SM_CYMAXIMIZED)};
    defaults.ptMinTrackSize = {GetSystemMetrics(SM_CXMINTRACK),
                               GetSystemMetrics(SM_CYMINTRACK)};
    defaults.ptMaxTrackSize = {GetSystemMetrics(SM_CXMAXTRACK),
                               GetSystemMetrics(SM_CYMAXTRACK)};
    *minSize = {defaults.ptMinTrackSize.x, defaults.ptMinTrackSize.y};
    *maxSize = {defaults.ptMaxTrackSize.x, defaults.ptMaxTrackSize.y};

    // The answer is in the window's coordinates, which are this thread's only
    // with the same DPI awareness.
    if (pAreDpiAwarenessContextsEqual &&
        !pAreDpiAwarenessContextsEqual(pGetWindowDpiAwarenessContext(hWnd),
                                       pGetThreadDpiAwarenessContext())) {
        return;
    }

    // A window of another thread of this process may still write its answer
    // after the call failed, so the memory is left to it then.
    auto* info = new MINMAXINFO(defaults);
    constexpr UINT kTimeoutMs = 50;
    if (!SendMessageTimeout(hWnd, WM_GETMINMAXINFO, 0, (LPARAM)info,
                            SMTO_ABORTIFHUNG, kTimeoutMs, nullptr)) {
        return;
    }

    *minSize = {info->ptMinTrackSize.x, info->ptMinTrackSize.y};
    *maxSize = {info->ptMaxTrackSize.x, info->ptMaxTrackSize.y};
    delete info;
}

// The rect the loop proposes for the bottom right corner is replaced by the one
// which moves every edge. The loop adds the pointer's movement to the rect it
// is handed back, so the new rect is worked out from the start each time. It's
// done ahead of the program's own handling of the message, so that a program
// which limits its size, e.g. to an aspect ratio, still gets to have its say.
void ReplaceSizingRect(HWND hWnd, RECT* rc) {
    POINT pt;
    if (!GetCursorPos(&pt)) {
        return;
    }

    if (!g_centerResize.started) {
        // The loop's first message, with the window still as it started.
        if (!GetWindowRect(hWnd, &g_centerResize.startRect)) {
            return;
        }

        g_centerResize.started = true;
        GetSizeLimits(hWnd, &g_centerResize.minSize, &g_centerResize.maxSize);
    }

    // A press which isn't dragged yet may still turn out to be a click, so the
    // window stays as it is until the pointer leaves the drag threshold.
    if (g_loop.command && !g_loop.dragged) {
        *rc = g_centerResize.startRect;
        return;
    }

    *rc = CenterResizedRect(g_centerResize.startRect, g_centerResize.startPt,
                            pt, g_centerResize.minSize, g_centerResize.maxSize);
}

LRESULT CALLBACK CenterResizeSubclassProc(HWND hWnd,
                                          UINT uMsg,
                                          WPARAM wParam,
                                          LPARAM lParam,
                                          UINT_PTR uIdSubclass,
                                          DWORD_PTR dwRefData) {
    auto hookScope = HookRefCountScope();

    if (uMsg == WM_SIZING) {
        if (wParam == WMSZ_BOTTOMRIGHT && lParam &&
            hWnd == g_centerResize.hWnd && !g_uninitializing) {
            ReplaceSizingRect(hWnd, (RECT*)lParam);
        }
    } else if (uMsg == WM_EXITSIZEMOVE || uMsg == WM_NCDESTROY ||
               uMsg == g_unsubclassRegisteredMessage) {
        // Ends every drag, one cancelled with Esc included. The message to
        // unsubclass goes on to the window's other subclass.
        EndCenterResize(hWnd);
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Runs on the root window's thread, with the request that starts the loop.
bool BeginCenterResize(HWND hWnd, POINT startPt) {
    EndCenterResize(g_centerResize.hWnd);

    std::lock_guard<std::mutex> guard(g_centerResizeWindowsMutex);
    if (g_uninitializing) {
        return false;
    }

    if (!SetWindowSubclass(hWnd, CenterResizeSubclassProc,
                           kCenterResizeSubclassId, 0)) {
        Wh_Log(L"SetWindowSubclass error for %08X", (DWORD)(ULONG_PTR)hWnd);
        return false;
    }

    g_centerResizeWindows.insert(hWnd);
    g_centerResize = {.hWnd = hWnd, .startPt = startPt};
    return true;
}

// Safe to call whether or not the subclass is on, from the window's thread.
void EndCenterResize(HWND hWnd) {
    if (!hWnd) {
        return;
    }

    RemoveWindowSubclass(hWnd, CenterResizeSubclassProc,
                         kCenterResizeSubclassId);

    {
        std::lock_guard<std::mutex> guard(g_centerResizeWindowsMutex);
        g_centerResizeWindows.erase(hWnd);
    }

    if (g_centerResize.hWnd == hWnd) {
        g_centerResize = {};
    }
}

LRESULT CALLBACK CallWndProc(int nCode, WPARAM wParam, LPARAM lParam) {
    auto hookScope = HookRefCountScope();

    if (nCode == HC_ACTION && lParam) {
        const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
        if (cwp->message == WM_NCCREATE) {
            // The mark is looked up on root windows, so a child window goes
            // without.
            const CREATESTRUCT* cs = (const CREATESTRUCT*)cwp->lParam;
            if (!(cs->style & WS_CHILD) && g_getMessageHook) {
                MarkHookedWindow(cwp->hwnd);
            }
        } else if (cwp->message == WM_NCHITTEST &&
                   (IsAnyTriggerKeyless() ||
                    IsAnyModifierTriggerHeld(
                        HeldModifiers(GetAsyncKeyState)))) {
            // The thread's synchronized key state is stale if the window
            // isn't active yet, e.g. a click on a background window.
            SubclassWindowIfNeeded(cwp->hwnd);
        } else if (cwp->message == WM_ACTIVATEAPP && IsAnyTriggerKeyless()) {
            // Told by the message rather than by the foreground window, which
            // the two processes see change at their own times.
            if (cwp->wParam) {
                InstallLowLevelMouseHookIfNeeded();
            } else {
                RemoveLowLevelMouseHookIfIdle();
            }
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// Where the window is held relative to its origin. A maximized window is
// restored first, like a title bar drag does, keeping the grab proportional to
// the size it restores to. The restore is queued rather than performed: this
// runs while the input thread waits, and the window's thread may be busy. The
// moves which follow wait for it.
POINT CalcDragGrab(HWND hRootWnd, POINT ptDown) {
    RECT rc;
    GetWindowRect(hRootWnd, &rc);

    WINDOWPLACEMENT placement{.length = sizeof(placement)};
    if (!GetWindowPlacement(hRootWnd, &placement) ||
        placement.showCmd != SW_SHOWMAXIMIZED) {
        return POINT{ptDown.x - rc.left, ptDown.y - rc.top};
    }

    const RECT& normal = placement.rcNormalPosition;
    POINT grab{
        MulDiv(ptDown.x - rc.left, normal.right - normal.left,
               rc.right - rc.left),
        MulDiv(ptDown.y - rc.top, normal.bottom - normal.top,
               rc.bottom - rc.top),
    };

    // For a window of this thread the move is applied at once, so the restore
    // has to be as well.
    if (GetWindowThreadProcessId(hRootWnd, nullptr) == GetCurrentThreadId()) {
        ShowWindow(hRootWnd, SW_RESTORE);
    } else {
        ShowWindowAsync(hRootWnd, SW_RESTORE);
    }

    return grab;
}

// SWP_ASYNCWINDOWPOS posts the request when the window belongs to another
// thread, which keeps a busy owner from blocking the caller. Not when the
// threads share an input queue, as those of a window and its child of another
// thread do: the request is then carried out at once, and would get ahead of
// the queued restore of a maximized window.
void MoveDraggedWindow(HWND hRootWnd, POINT grab, POINT pt) {
    if (IsZoomed(hRootWnd)) {
        return;
    }

    SetWindowPos(
        hRootWnd, nullptr, pt.x - grab.x, pt.y - grab.y, 0, 0,
        SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
}

// The edges the command takes hold of follow the pointer from where the drag
// started, the others stay put; a resize from the center takes hold of them
// all, each side moving opposite to the other. The window's size limits are
// applied here rather than left to the window, which keeps its top left corner
// in place applying them and so moves the other edges.
void SizeDraggedWindow(const DragHold& hold, POINT pt) {
    UINT edge = hold.command & 0xF;
    const RECT& rcStart = hold.startRect;
    LONG dx = pt.x - hold.startPt.x;
    LONG dy = pt.y - hold.startPt.y;
    LONG minWidth = hold.minSize.cx;
    LONG minHeight = hold.minSize.cy;
    LONG maxWidth = std::max(hold.maxSize.cx, minWidth);
    LONG maxHeight = std::max(hold.maxSize.cy, minHeight);

    RECT rc = rcStart;
    if (edge == kWmszCenter) {
        rc = CenterResizedRect(rcStart, hold.startPt, pt, hold.minSize,
                               hold.maxSize);
    }

    switch (edge) {
        case WMSZ_LEFT:
        case WMSZ_TOPLEFT:
        case WMSZ_BOTTOMLEFT:
            rc.left = std::clamp(rcStart.left + dx, rc.right - maxWidth,
                                 rc.right - minWidth);
            break;
        case WMSZ_RIGHT:
        case WMSZ_TOPRIGHT:
        case WMSZ_BOTTOMRIGHT:
            rc.right = std::clamp(rcStart.right + dx, rc.left + minWidth,
                                  rc.left + maxWidth);
            break;
    }

    switch (edge) {
        case WMSZ_TOP:
        case WMSZ_TOPLEFT:
        case WMSZ_TOPRIGHT:
            rc.top = std::clamp(rcStart.top + dy, rc.bottom - maxHeight,
                                rc.bottom - minHeight);
            break;
        case WMSZ_BOTTOM:
        case WMSZ_BOTTOMLEFT:
        case WMSZ_BOTTOMRIGHT:
            rc.bottom = std::clamp(rcStart.bottom + dy, rc.top + minHeight,
                                   rc.top + maxHeight);
            break;
    }

    SetWindowPos(hold.hRootWnd, nullptr, rc.left, rc.top, rc.right - rc.left,
                 rc.bottom - rc.top,
                 SWP_NOZORDER | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
}

void TakeHold(DragHold* hold) {
    if (IsSizeCommand(hold->command)) {
        GetWindowRect(hold->hRootWnd, &hold->startRect);
        GetSizeLimits(hold->hRootWnd, &hold->minSize, &hold->maxSize);
    } else {
        hold->grab = CalcDragGrab(hold->hRootWnd, hold->startPt);
    }
}

void DragHeldWindowTo(const DragHold& hold, POINT pt) {
    if (IsSizeCommand(hold.command)) {
        SizeDraggedWindow(hold, pt);
    } else {
        MoveDraggedWindow(hold.hRootWnd, hold.grab, pt);
    }
}

// The class supplies the cursor, and with DefWindowProc as the window procedure
// no mod code is on the window's call path.
HWND CreateDragOverlay(POINT pt, PCWSTR cursor) {
    WNDCLASS wc{
        .lpfnWndProc = DefWindowProc,
        .hInstance = GetModuleHandle(nullptr),
        .hCursor = LoadCursor(nullptr, cursor),
        .lpszClassName = kDragOverlayClassName,
    };
    if (!RegisterClass(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        Wh_Log(L"RegisterClass error: %u", GetLastError());
        return nullptr;
    }

    HWND hWnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED,
        kDragOverlayClassName, nullptr, WS_POPUP, pt.x - kDragOverlaySize / 2,
        pt.y - kDragOverlaySize / 2, kDragOverlaySize, kDragOverlaySize,
        nullptr, nullptr, wc.hInstance, nullptr);
    if (!hWnd) {
        Wh_Log(L"CreateWindowEx error: %u", GetLastError());
        return nullptr;
    }

    // The class is registered once and kept, with the cursor of that time.
    SetClassLongPtr(hWnd, GCLP_HCURSOR, (LONG_PTR)wc.hCursor);

    // As good as invisible, while an alpha of zero would let the mouse through.
    SetLayeredWindowAttributes(hWnd, 0, 1, LWA_ALPHA);
    ShowWindow(hWnd, SW_SHOWNOACTIVATE);
    return hWnd;
}

void PlaceDragOverlay(POINT pt) {
    if (g_llDragOverlayWnd) {
        SetWindowPos(g_llDragOverlayWnd, nullptr, pt.x - kDragOverlaySize / 2,
                     pt.y - kDragOverlaySize / 2, 0, 0,
                     SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

// Posts a message of the mod to the message loop of the current thread, where
// the hooks retrieve it. The focus window is passed over when it's of another
// thread sharing this thread's input queue, whose loop would retrieve it.
bool PostToThisThread(UINT message, WPARAM wParam = 0, LPARAM lParam = 0) {
    HWND hFocusWnd = GetFocus();
    if (hFocusWnd &&
        GetWindowThreadProcessId(hFocusWnd, nullptr) != GetCurrentThreadId()) {
        hFocusWnd = nullptr;
    }

    return (hFocusWnd && PostMessage(hFocusWnd, message, wParam, lParam)) ||
           PostThreadMessage(GetCurrentThreadId(), message, wParam, lParam);
}

// The overlay's cursor is what the pointer shows until the window under it is
// asked for its own, which the system does only on a real mouse move. A program
// which sets its cursor from its own pointer input, e.g. a XAML island, keeps
// the overlay's cursor up until then. Asked here, once the overlay is gone,
// the way the system asks on a move, with the hit test code the window gives
// for the spot. Bounded, the window being possibly of another process.
void RefreshCursorAfterOverlay() {
    POINT pt;
    if (!GetCursorPos(&pt)) {
        return;
    }

    HWND hWnd = WindowFromPoint(pt);
    if (!hWnd) {
        return;
    }

    constexpr UINT kTimeoutMs = 50;
    DWORD_PTR hitTest = HTCLIENT;
    if (!SendMessageTimeout(hWnd, WM_NCHITTEST, 0, MAKELPARAM(pt.x, pt.y),
                            SMTO_ABORTIFHUNG, kTimeoutMs, &hitTest)) {
        return;
    }

    SendMessageTimeout(hWnd, WM_SETCURSOR, (WPARAM)hWnd,
                       MAKELPARAM((WORD)hitTest, WM_MOUSEMOVE),
                       SMTO_ABORTIFHUNG, kTimeoutMs, nullptr);
}

// Through DefWindowProc, so on the window's own thread wherever this runs. The
// cursor refresh is queued behind the close, which is retrieved first, if the
// overlay is this thread's: that of another thread may still be up when the
// refresh is retrieved here.
void DestroyDragOverlay(bool refreshCursor = true) {
    if (HWND hOverlayWnd = g_llDragOverlayWnd) {
        g_llDragOverlayWnd = nullptr;
        bool overlayOfThisThread =
            GetWindowThreadProcessId(hOverlayWnd, nullptr) ==
            GetCurrentThreadId();
        PostMessage(hOverlayWnd, WM_CLOSE, 0, 0);
        if (refreshCursor && overlayOfThisThread) {
            PostToThisThread(g_refreshCursorMessage);
        }
    }
}

void KillLowLevelDelayTimer() {
    if (g_llDelayTimerId) {
        KillTimer(nullptr, g_llDelayTimerId);
        g_llDelayTimerId = 0;
    }
}

void ResetLowLevelDragState() {
    g_llRootWnd = nullptr;
    g_llButton = 0;
    g_llDragging = false;
    g_llDragSetUp = false;
    g_llDragUpdatePosted = false;
    KillLowLevelDelayTimer();
    DestroyDragOverlay();
}

// Display settings are never scaled, so they give the physical side.
BOOL CALLBACK FindMonitorEnumProc(HMONITOR hMonitor,
                                  HDC hdc,
                                  LPRECT lprcMonitor,
                                  LPARAM lParam) {
    MONITORINFOEX monitorInfo;
    monitorInfo.cbSize = sizeof(monitorInfo);
    DEVMODE mode{};
    mode.dmSize = sizeof(mode);
    if (!GetMonitorInfo(hMonitor, &monitorInfo) ||
        !EnumDisplaySettings(monitorInfo.szDevice, ENUM_CURRENT_SETTINGS,
                             &mode)) {
        return TRUE;
    }

    RECT physicalRect{mode.dmPosition.x, mode.dmPosition.y,
                      mode.dmPosition.x + (LONG)mode.dmPelsWidth,
                      mode.dmPosition.y + (LONG)mode.dmPelsHeight};
    if (!PtInRect(&physicalRect, *(const POINT*)lParam)) {
        return TRUE;
    }

    g_llMonitor.physicalRect = physicalRect;
    g_llMonitor.logicalRect = monitorInfo.rcMonitor;
    return FALSE;
}

bool FindMonitorForPhysicalPoint(POINT physicalPt) {
    g_llMonitor = {};
    EnumDisplayMonitors(nullptr, nullptr, FindMonitorEnumProc,
                        (LPARAM)&physicalPt);
    return !IsRectEmpty(&g_llMonitor.physicalRect);
}

// The hook reports physical coordinates, while this thread looks windows up
// and places them in its own.
POINT ToLogicalPoint(POINT physicalPt) {
    if (!PtInRect(&g_llMonitor.physicalRect, physicalPt) &&
        !FindMonitorForPhysicalPoint(physicalPt)) {
        return physicalPt;
    }

    const RECT& physical = g_llMonitor.physicalRect;
    const RECT& logical = g_llMonitor.logicalRect;
    return POINT{
        logical.left + MulDiv(physicalPt.x - physical.left,
                              logical.right - logical.left,
                              physical.right - physical.left),
        logical.top + MulDiv(physicalPt.y - physical.top,
                             logical.bottom - logical.top,
                             physical.bottom - physical.top),
    };
}

// Simulates the press of the button at the cursor, and its release too for a
// click. Runs from a message loop, never from the low level mouse hook's
// callback: input sent from within a low level hook callback isn't processed
// until the hook's timeout has run out, and the callbacks arriving in the
// meantime see state the callback hasn't updated yet. Since this press does
// enter the input queue, the Alt release needs no swallowing on its account.
void ReplayPress(int button, bool asClick) {
    DWORD downFlag;
    DWORD upFlag;
    DWORD mouseData = 0;
    switch (button) {
        case VK_RBUTTON:
            downFlag = MOUSEEVENTF_RIGHTDOWN;
            upFlag = MOUSEEVENTF_RIGHTUP;
            break;
        case VK_MBUTTON:
            downFlag = MOUSEEVENTF_MIDDLEDOWN;
            upFlag = MOUSEEVENTF_MIDDLEUP;
            break;
        case VK_XBUTTON1:
        case VK_XBUTTON2:
            downFlag = MOUSEEVENTF_XDOWN;
            upFlag = MOUSEEVENTF_XUP;
            mouseData = button == VK_XBUTTON1 ? XBUTTON1 : XBUTTON2;
            break;
        default:
            downFlag = MOUSEEVENTF_LEFTDOWN;
            upFlag = MOUSEEVENTF_LEFTUP;
            break;
    }

    INPUT inputs[2]{};
    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi.dwFlags = downFlag;
    inputs[0].mi.mouseData = mouseData;
    inputs[0].mi.dwExtraInfo = kOwnInputExtraInfo;
    inputs[1].type = INPUT_MOUSE;
    inputs[1].mi.dwFlags = upFlag;
    inputs[1].mi.mouseData = mouseData;
    inputs[1].mi.dwExtraInfo = kOwnInputExtraInfo;
    UINT count = asClick ? 2 : 1;
    if (SendInput(count, inputs, sizeof(inputs[0])) == count) {
        g_swallowedPress = false;
    } else {
        Wh_Log(L"SendInput error: %u", GetLastError());
    }
}

// Hands a replay over to the message loop of this thread, where it's retrieved
// ahead of the input which follows, including an Alt release.
void PostReplayPress(int button, bool asClick) {
    PostToThisThread(g_replayPressMessage, button, asClick);
}

// The press of a trigger's button over composition hosted content. Returns
// whether it was swallowed.
bool OnLowLevelButtonDown(const MSLLHOOKSTRUCT* ms, int button) {
    // Looked up afresh in case the display layout changed.
    g_llMonitor = {};
    POINT pt = ToLogicalPoint(ms->pt);

    HWND hWnd = CompositionHostedWindowFromPoint(pt);
    if (!hWnd) {
        // Delivered as a message, so the paths above handle it and keep the
        // system loop.
        return false;
    }

    HWND hRootWnd = GetAncestor(hWnd, GA_ROOT);
    if (!hRootWnd) {
        return false;
    }

    // A disabled window, such as the owner of a modal dialog, takes no mouse
    // input, so it isn't dragged either.
    if (!IsWindowEnabled(hRootWnd)) {
        return false;
    }

    UINT command = DragCommandForPress(button, HeldModifiers(GetAsyncKeyState),
                                       hRootWnd, pt);
    if (!command) {
        return false;
    }

    Wh_Log(L"Swallowed the press of button %02X over %08X, root window %08X",
           button, (DWORD)(ULONG_PTR)hWnd, (DWORD)(ULONG_PTR)hRootWnd);

    g_llRootWnd = hRootWnd;
    g_llCommand = command;
    g_llButton = button;
    g_llDownPt = pt;
    g_llDownTime = ms->time;
    if (GetAsyncKeyState(VK_MENU) < 0) {
        g_swallowedPress = true;
    }

    // A timer may be left from a candidate reset on another thread.
    KillLowLevelDelayTimer();
    if (int delay = TriggerOfCommand(command).delay; delay > 0) {
        g_llDelayTimerId = SetTimer(nullptr, 0, delay, nullptr);
        if (!g_llDelayTimerId) {
            Wh_Log(L"SetTimer error: %u", GetLastError());
        }
    }

    return true;
}

// Puts the overlay up for a press which is still a candidate, if this is
// still the hook's thread: the timer is this thread's, the press the hook's.
// The press being held is what the state means, its release clearing it; the
// key state can't tell, a swallowed press never reaching it.
void OnLowLevelDragDelayElapsed() {
    KillLowLevelDelayTimer();

    {
        std::lock_guard<std::mutex> guard(g_lowLevelMouseHookMutex);
        if (GetCurrentThreadId() != g_lowLevelMouseHookThreadId) {
            return;
        }
    }

    POINT pt;
    if (!g_llRootWnd || g_llDragging || !GetCursorPos(&pt)) {
        return;
    }

    Wh_Log(L"Drag delay elapsed, showing the %s cursor",
           NameOfCommand(g_llCommand));
    g_llDragOverlayWnd = CreateDragOverlay(pt, CursorOfCommand(g_llCommand));
}

// The first move past the drag threshold, once the drag delay is over. Only
// what a low level hook callback can afford is done here, the rest waiting for
// the update this leaves to be posted.
void BeginLowLevelDrag(POINT pt) {
    Wh_Log(L"Starting a %s of root window %08X", NameOfCommand(g_llCommand),
           (DWORD)(ULONG_PTR)g_llRootWnd);

    g_llDragging = true;
    g_llHold = {.hRootWnd = g_llRootWnd, .command = g_llCommand, .startPt = pt};
    KillLowLevelDelayTimer();
}

// The work a drag starts with, none of which belongs in the callback: the
// restore of a maximized window runs the program's layout, and it runs inline
// when the window is of the hook's own thread, which the foreground process's
// window commonly is.
void SetUpLowLevelDrag() {
    TakeHold(&g_llHold);

    // The press was swallowed, so the window wasn't brought to the front the
    // way a click on it would have been. Allowed because the process holding
    // the hook is the foreground one.
    DWORD processId = 0;
    GetWindowThreadProcessId(g_llRootWnd, &processId);
    if (!SetForegroundWindow(g_llRootWnd)) {
        Wh_Log(L"SetForegroundWindow error: %u", GetLastError());
    } else if (processId != GetCurrentProcessId()) {
        // The Alt release goes there now, and a later one here isn't to be
        // taken for it.
        g_swallowedPress = false;
    }

    MaskWinKeyReleaseIfNeeded(g_llCommand);

    if (!g_llDragOverlayWnd) {
        g_llDragOverlayWnd =
            CreateDragOverlay(g_llHold.startPt, CursorOfCommand(g_llCommand));
    }
}

// Runs on the hook's thread once the update posted for it is retrieved. The
// press may be over by then, its release having reset the state.
void OnDragUpdate() {
    g_llDragUpdatePosted = false;

    if (!g_llRootWnd) {
        return;
    }

    if (g_llDragging && !g_llDragSetUp) {
        g_llDragSetUp = true;
        SetUpLowLevelDrag();
    }

    PlaceDragOverlay(g_llDragPt);

    if (g_llDragging) {
        DragHeldWindowTo(g_llHold, g_llDragPt);
    }
}

// The callback records where the pointer is and leaves the windows to the
// message loop: a low level hook which takes too long to return is dropped by
// the system, and moving or resizing a window of the hook's own thread runs
// the program's handlers inline. One update is queued at a time, and it acts
// on whatever position the last move left.
void PostDragUpdate(POINT pt) {
    g_llDragPt = pt;
    if (g_llDragUpdatePosted) {
        return;
    }

    if (PostToThisThread(g_dragUpdateMessage)) {
        g_llDragUpdatePosted = true;
    }
}

// Runs for mouse input before it's routed anywhere, which is the only point at
// which a press can be taken away from composition hosted content. Moves are
// never swallowed: that would stop the cursor.
LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    auto hookScope = HookRefCountScope();

    if (nCode != HC_ACTION || g_uninitializing) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    const MSLLHOOKSTRUCT* ms = (const MSLLHOOKSTRUCT*)lParam;

    if (ms->dwExtraInfo == kOwnInputExtraInfo) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    // The points recorded here are applied by the message loop's hook.
    auto dpiScope = MessageLoopDpiScope();

    // Focus can move away while the keys are held, e.g. Alt+Tab, and their
    // release then goes to another process. Drop the hook here instead of
    // keeping it for the rest of the process's life.
    RemoveLowLevelMouseHookIfIdle();

    bool down;
    int button = ButtonOfMessage((UINT)wParam, HIWORD(ms->mouseData), &down);
    if (button && down) {
        // A press of the drag's button again means its release was missed.
        if (button == g_llButton) {
            ResetLowLevelDragState();
        }

        if (!g_llRootWnd && OnLowLevelButtonDown(ms, button)) {
            return 1;
        }
    } else if (button && button == g_llButton) {
        // The reset goes first: it closes the overlay, if the delay put one
        // up, with a message queued ahead of the replay, which would
        // otherwise land on it.
        bool dragging = g_llDragging;
        ResetLowLevelDragState();
        if (!dragging) {
            PostReplayPress(button, true);
        }

        return 1;
    } else if (wParam == WM_MOUSEMOVE && g_llRootWnd) {
        POINT pt = ToLogicalPoint(ms->pt);

        DWORD delay = TriggerOfCommand(g_llCommand).delay;

        if (g_llDragging || !IsPastDragThreshold(g_llDownPt, pt)) {
            PostDragUpdate(pt);
        } else if (ms->time - g_llDownTime >= delay) {
            BeginLowLevelDrag(pt);
            PostDragUpdate(pt);
        } else {
            // The drag is the program's own, so the press goes back to it,
            // where the cursor is by now. The release then follows on its
            // own.
            Wh_Log(L"Moved before the drag delay elapsed, replaying the press");
            int pressButton = g_llButton;
            ResetLowLevelDragState();
            PostReplayPress(pressButton, false);
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// A hook left over from before is discarded. The release ending a drag may
// never arrive: a hook further along the chain can swallow it, and the system
// drops a hook which took too long to return. What is left then keeps the
// hook, or a dead handle, in place for good. The drag state it leaves behind
// is cleared by the next installation, on the thread which then owns it.
void RemoveLowLevelMouseHook() {
    std::lock_guard<std::mutex> guard(g_lowLevelMouseHookMutex);

    if (g_lowLevelMouseHook) {
        UnhookWindowsHookEx(g_lowLevelMouseHook);
        g_lowLevelMouseHook = nullptr;
    }
}

void InstallLowLevelMouseHookIfNeeded() {
    // Outside the lock, the loader being out of bounds while it's held.
    HMODULE module = GetModuleHandle(nullptr);

    std::lock_guard<std::mutex> guard(g_lowLevelMouseHookMutex);
    if (g_lowLevelMouseHook || g_uninitializing) {
        return;
    }

    // With no hook there's no thread the drag state belongs to, so whatever a
    // discarded hook left behind is cleared here, before the callbacks of the
    // new one start reading it on this thread.
    ResetLowLevelDragState();

    g_lowLevelMouseHook =
        SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, module, 0);
    if (g_lowLevelMouseHook) {
        g_lowLevelMouseHookThreadId = GetCurrentThreadId();
    } else {
        Wh_Log(L"SetWindowsHookEx(WH_MOUSE_LL) error: %u", GetLastError());
    }
}

void RemoveLowLevelMouseHookIfIdle() {
    std::lock_guard<std::mutex> guard(g_lowLevelMouseHookMutex);

    // Only the hook's thread can tell whether a press is being held or a drag
    // is in progress, which keep the hook, as do the keys and, for a keyless
    // trigger, this process being the foreground one. A key release or a
    // deactivation retrieved elsewhere leaves the removal to the callback,
    // which checks on the next mouse event.
    if (!g_lowLevelMouseHook ||
        GetCurrentThreadId() != g_lowLevelMouseHookThreadId || g_llRootWnd ||
        IsAnyModifierTriggerHeld(HeldModifiers(GetAsyncKeyState)) ||
        ShouldHoldKeylessHook()) {
        return;
    }

    UnhookWindowsHookEx(g_lowLevelMouseHook);
    g_lowLevelMouseHook = nullptr;
}

void RemoveWinKeyMaskHook() {
    std::lock_guard<std::mutex> guard(g_winKeyMaskHookMutex);

    if (g_winKeyMaskHook) {
        UnhookWindowsHookEx(g_winKeyMaskHook);
        g_winKeyMaskHook = nullptr;
    }
}

// Sends the release again behind a tap of the mask key. Returns whether the
// original is to be swallowed, which it is only with the whole sequence sent:
// short of the release the key would stay down, e.g. with SendInput refused
// while an elevated window is in the foreground.
bool ResendWinKeyReleaseMasked(const KBDLLHOOKSTRUCT* kbd) {
    INPUT inputs[3]{};
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = kMenuMaskKey;
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = kMenuMaskKey;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
    // The Win keys are extended keys, and a release without the flag may not
    // register, leaving the key down.
    inputs[2].type = INPUT_KEYBOARD;
    inputs[2].ki.wVk = (WORD)kbd->vkCode;
    inputs[2].ki.wScan = (WORD)kbd->scanCode;
    inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
    if (kbd->flags & LLKHF_EXTENDED) {
        inputs[2].ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
    }

    if (SendInput(ARRAYSIZE(inputs), inputs, sizeof(inputs[0])) !=
        ARRAYSIZE(inputs)) {
        Wh_Log(L"SendInput error: %u", GetLastError());
        return false;
    }

    return true;
}

// Keyboard input sent from within the callback is processed on the spot, the
// hook included, which is where the injected flag matters.
LRESULT CALLBACK WinKeyMaskProc(int nCode, WPARAM wParam, LPARAM lParam) {
    auto hookScope = HookRefCountScope();

    if (nCode != HC_ACTION || g_uninitializing) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    const KBDLLHOOKSTRUCT* kbd = (const KBDLLHOOKSTRUCT*)lParam;
    if ((kbd->vkCode != VK_LWIN && kbd->vkCode != VK_RWIN) ||
        (kbd->flags & LLKHF_INJECTED)) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
        Wh_Log(L"Masking the Win release of a drag");
        bool swallow = ResendWinKeyReleaseMasked(kbd);
        RemoveWinKeyMaskHook();
        if (swallow) {
            return 1;
        }
    } else if (GetAsyncKeyState(kbd->vkCode) >= 0) {
        // The key state is updated once the hook lets the event through, so
        // the key shows up for an auto repeat and not for a new hold, which
        // means the release the hook was installed for went by unseen.
        RemoveWinKeyMaskHook();
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// The Start menu is armed by every auto repeat of the held key, so the mask
// has to go right before the release, and the hook is installed on the drag
// thread to see it. One left over from an earlier drag is replaced.
void InstallWinKeyMaskHook() {
    // Outside the lock, the loader being out of bounds while it's held.
    HMODULE module = GetModuleHandle(nullptr);

    std::lock_guard<std::mutex> guard(g_winKeyMaskHookMutex);
    if (g_uninitializing) {
        return;
    }

    if (g_winKeyMaskHook) {
        UnhookWindowsHookEx(g_winKeyMaskHook);
    }

    g_winKeyMaskHook =
        SetWindowsHookEx(WH_KEYBOARD_LL, WinKeyMaskProc, module, 0);
    if (g_winKeyMaskHook) {
        g_winKeyMaskHookThreadId = GetCurrentThreadId();
    } else {
        Wh_Log(L"SetWindowsHookEx(WH_KEYBOARD_LL) error: %u", GetLastError());
    }
}

// For a drag started with a trigger which has Win among its keys.
void MaskWinKeyReleaseIfNeeded(UINT command) {
    const TriggerSettings& trigger = TriggerOfCommand(command);
    if ((trigger.modifiers & kModifierWin) &&
        (HeldModifiers(GetAsyncKeyState) & kModifierWin)) {
        InstallWinKeyMaskHook();
    }
}

void SetThreadHooksIfNeeded() {
    if (g_threadHooksAttempted) {
        return;
    }

    g_threadHooksAttempted = true;

    std::unique_lock<std::mutex> guard(g_allThreadHooksMutex);
    if (g_uninitializing) {
        return;
    }

    DWORD dwThreadId = GetCurrentThreadId();

    g_getMessageHook =
        SetWindowsHookEx(WH_GETMESSAGE, GetMsgProc, nullptr, dwThreadId);
    if (g_getMessageHook) {
        g_allThreadHooks.insert(g_getMessageHook);
    } else {
        Wh_Log(L"SetWindowsHookEx(WH_GETMESSAGE) error for thread %u: %u",
               dwThreadId, GetLastError());
    }

    g_callWndProcHook =
        SetWindowsHookEx(WH_CALLWNDPROC, CallWndProc, nullptr, dwThreadId);
    if (g_callWndProcHook) {
        g_allThreadHooks.insert(g_callWndProcHook);
    } else {
        Wh_Log(L"SetWindowsHookEx(WH_CALLWNDPROC) error for thread %u: %u",
               dwThreadId, GetLastError());
    }

    if (g_getMessageHook && g_callWndProcHook) {
        Wh_Log(L"SetWindowsHookEx succeeded for thread %u", dwThreadId);
    }

    if (g_getMessageHook) {
        // The windows the thread has by now, later ones are marked as created.
        EnumThreadWindows(
            dwThreadId,
            [](HWND hWnd, LPARAM) WINAPI -> BOOL {
                MarkHookedWindow(hWnd);
                return TRUE;
            },
            0);
    }

    guard.unlock();

    // A process which is the foreground one already sees no activation to put
    // the hook of a keyless trigger up on.
    if (ShouldHoldKeylessHook()) {
        InstallLowLevelMouseHookIfNeeded();
    }
}

void RecordMessageLoopDpiContext() {
    if (pGetThreadDpiAwarenessContext) {
        g_messageLoopDpiContext = pGetThreadDpiAwarenessContext();
    }
}

using NtUserGetMessage_t = BOOL(WINAPI*)(MSG* pMsg,
                                         HWND hWnd,
                                         UINT wMsgFilterMin,
                                         UINT wMsgFilterMax);
NtUserGetMessage_t NtUserGetMessage_Original;
BOOL WINAPI NtUserGetMessage_Hook(MSG* pMsg,
                                  HWND hWnd,
                                  UINT wMsgFilterMin,
                                  UINT wMsgFilterMax) {
    SetThreadHooksIfNeeded();
    RecordMessageLoopDpiContext();

    [[clang::musttail]] return NtUserGetMessage_Original(
        pMsg, hWnd, wMsgFilterMin, wMsgFilterMax);
}

// The last parameter is a flags value which user32 sets from the calling
// PeekMessage variant.
using NtUserPeekMessage_t = BOOL(WINAPI*)(MSG* pMsg,
                                          HWND hWnd,
                                          UINT wMsgFilterMin,
                                          UINT wMsgFilterMax,
                                          UINT wRemoveMsg,
                                          UINT flags);
NtUserPeekMessage_t NtUserPeekMessage_Original;
BOOL WINAPI NtUserPeekMessage_Hook(MSG* pMsg,
                                   HWND hWnd,
                                   UINT wMsgFilterMin,
                                   UINT wMsgFilterMax,
                                   UINT wRemoveMsg,
                                   UINT flags) {
    SetThreadHooksIfNeeded();
    RecordMessageLoopDpiContext();

    [[clang::musttail]] return NtUserPeekMessage_Original(
        pMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg, flags);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            break;

        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH:
            if (g_getMessageHook || g_callWndProcHook) {
                std::lock_guard<std::mutex> guard(g_allThreadHooksMutex);

                for (HHOOK hook : {g_getMessageHook, g_callWndProcHook}) {
                    auto it = g_allThreadHooks.find(hook);
                    if (it != g_allThreadHooks.end()) {
                        UnhookWindowsHookEx(hook);
                        g_allThreadHooks.erase(it);
                    }
                }
            }

            // The low level hooks and the overlay go away with their
            // thread. The drag state is this thread's to clear, the hook
            // being this thread's.
            {
                std::lock_guard<std::mutex> guard(g_lowLevelMouseHookMutex);

                if (g_lowLevelMouseHook &&
                    GetCurrentThreadId() == g_lowLevelMouseHookThreadId) {
                    g_lowLevelMouseHook = nullptr;
                    g_llRootWnd = nullptr;
                    g_llButton = 0;
                    g_llDragging = false;
                    g_llDragSetUp = false;
                    g_llDragUpdatePosted = false;
                    g_llDragOverlayWnd = nullptr;
                }
            }

            {
                std::lock_guard<std::mutex> guard(g_winKeyMaskHookMutex);

                if (g_winKeyMaskHook &&
                    GetCurrentThreadId() == g_winKeyMaskHookThreadId) {
                    g_winKeyMaskHook = nullptr;
                }
            }
            break;

        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;
}

int ButtonFromSetting(PCWSTR value) {
    if (_wcsicmp(value, L"right") == 0) {
        return VK_RBUTTON;
    }
    if (_wcsicmp(value, L"middle") == 0) {
        return VK_MBUTTON;
    }
    if (_wcsicmp(value, L"x1") == 0) {
        return VK_XBUTTON1;
    }
    if (_wcsicmp(value, L"x2") == 0) {
        return VK_XBUTTON2;
    }
    return VK_LBUTTON;
}

void LoadTriggerSettings(TriggerSettings& trigger, PCWSTR name) {
    PCWSTR button = Wh_GetStringSetting(L"%ls.button", name);
    trigger.button = ButtonFromSetting(button);
    Wh_FreeStringSetting(button);

    DWORD modifiers = 0;
    if (Wh_GetIntSetting(L"%ls.ctrl", name)) {
        modifiers |= kModifierCtrl;
    }
    if (Wh_GetIntSetting(L"%ls.alt", name)) {
        modifiers |= kModifierAlt;
    }
    if (Wh_GetIntSetting(L"%ls.shift", name)) {
        modifiers |= kModifierShift;
    }
    if (Wh_GetIntSetting(L"%ls.win", name)) {
        modifiers |= kModifierWin;
    }
    trigger.modifiers = modifiers;

    trigger.delay = std::max(Wh_GetIntSetting(L"%ls.delay", name), 0);
}

void LoadSettings() {
    LoadTriggerSettings(g_settings.moveTrigger, L"moveTrigger");
    LoadTriggerSettings(g_settings.sizeTrigger, L"sizeTrigger");
    g_settings.centerResizeAllDirections =
        Wh_GetIntSetting(L"centerResizeAllDirections");
    g_settings.dragWindowsWithoutTitleBar =
        Wh_GetIntSetting(L"dragWindowsWithoutTitleBar");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    HMODULE user32Module = GetModuleHandle(L"user32.dll");
    pGetWindowDpiAwarenessContext =
        (GetWindowDpiAwarenessContext_t)GetProcAddress(
            user32Module, "GetWindowDpiAwarenessContext");
    pGetThreadDpiAwarenessContext =
        (GetThreadDpiAwarenessContext_t)GetProcAddress(
            user32Module, "GetThreadDpiAwarenessContext");
    pSetThreadDpiAwarenessContext =
        (SetThreadDpiAwarenessContext_t)GetProcAddress(
            user32Module, "SetThreadDpiAwarenessContext");
    pAreDpiAwarenessContextsEqual =
        (AreDpiAwarenessContextsEqual_t)GetProcAddress(
            user32Module, "AreDpiAwarenessContextsEqual");

    HMODULE win32uModule = GetModuleHandle(L"win32u.dll");
    if (!win32uModule) {
        Wh_Log(L"win32u.dll isn't loaded");
        return FALSE;
    }

    void* pNtUserGetMessage =
        (void*)GetProcAddress(win32uModule, "NtUserGetMessage");
    void* pNtUserPeekMessage =
        (void*)GetProcAddress(win32uModule, "NtUserPeekMessage");
    if (!pNtUserGetMessage || !pNtUserPeekMessage) {
        Wh_Log(L"NtUserGetMessage or NtUserPeekMessage not found");
        return FALSE;
    }

    Wh_SetFunctionHook(pNtUserGetMessage, (void*)NtUserGetMessage_Hook,
                       (void**)&NtUserGetMessage_Original);
    Wh_SetFunctionHook(pNtUserPeekMessage, (void*)NtUserPeekMessage_Hook,
                       (void**)&NtUserPeekMessage_Original);

    // Lets a lower integrity process, e.g. a UWP app, request a drag of a root
    // window in this process.
    ChangeWindowMessageFilter(g_sizeMoveRequestMessage, MSGFLT_ADD);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");

    g_uninitializing = true;

    std::unordered_set<HWND> subclassedWindows;
    {
        std::lock_guard<std::mutex> guard(g_subclassedWindowsMutex);
        subclassedWindows = std::move(g_subclassedWindows);
        g_subclassedWindows.clear();
    }

    for (HWND hWnd : subclassedWindows) {
        SendMessage(hWnd, g_unsubclassRegisteredMessage, 0, 0);
    }

    std::unordered_set<HWND> centerResizeWindows;
    {
        std::lock_guard<std::mutex> guard(g_centerResizeWindowsMutex);
        centerResizeWindows = std::move(g_centerResizeWindows);
        g_centerResizeWindows.clear();
    }

    for (HWND hWnd : centerResizeWindows) {
        SendMessage(hWnd, g_unsubclassRegisteredMessage, 0, 0);
    }

    {
        std::lock_guard<std::mutex> guard(g_allThreadHooksMutex);

        for (HHOOK hook : g_allThreadHooks) {
            UnhookWindowsHookEx(hook);
        }

        g_allThreadHooks.clear();
    }

    ChangeWindowMessageFilter(g_sizeMoveRequestMessage, MSGFLT_REMOVE);

    {
        std::lock_guard<std::mutex> guard(g_lowLevelMouseHookMutex);

        if (g_lowLevelMouseHook) {
            UnhookWindowsHookEx(g_lowLevelMouseHook);
            g_lowLevelMouseHook = nullptr;
        }
    }

    RemoveWinKeyMaskHook();

    while (g_hookRefCount > 0) {
        Sleep(200);
    }

    // With the hooks gone, no window is marked from here on.
    EnumWindows(
        [](HWND hWnd, LPARAM) WINAPI -> BOOL {
            DWORD processId = 0;
            GetWindowThreadProcessId(hWnd, &processId);
            if (processId == GetCurrentProcessId()) {
                RemoveProp(hWnd, kHookedWindowProp);
            }
            return TRUE;
        },
        0);

    // The class may outlive the window, and is then found in place next time.
    DestroyDragOverlay(false);
    UnregisterClass(kDragOverlayClassName, GetModuleHandle(nullptr));
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}
