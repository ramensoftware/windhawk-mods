// ==WindhawkMod==
// @id              david-window-pack
// @name            David's Window Pack
// @description     Alt-drag window movement, keyboard snapping, monitor hotkeys and AltSnap keyboard shortcuts in one mod
// @version         1.0.0
// @author          DavidHiFi
// @github          https://github.com/DavidHiFi
// @homepage        https://github.com/DavidHiFi/davids-windhawk-mods
// @license         GPL-3.0
// @include         *
// @include         windhawk.exe
// @compilerOptions -luser32 -ldwmapi -lgdi32 -lshcore -lcomctl32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# David's Window Pack

One mod combining the three window-management mods it replaces:

* **AltSnap drag** (from local@alt-snap-drag, a fork of alt-drag by m417z,
  based on AltSnap by RamonUnch): hold Alt and drag with the left button to
  move a window, with the right button to resize it; Alt+F maximizes,
  Alt+M minimizes, Alt+Shift+Q closes, and a right click while moving
  toggles maximize.
* **Snap Commander** (by Asteski): move the active window to screen halves
  and corners with keyboard shortcuts (Alt+Q/W/E/R, Alt+U/I/J/K, Alt+G
  center, Alt+H maximize, Alt+M minimize, Alt+]/[ next/previous display
  and more; see the settings).
* **Move Window to Monitor** (by TomberWolf): move the active window to
  another monitor with hotkeys (default Ctrl+Alt+arrow keys).

Settings from all three mods are preserved in one settings page. The
mouse and in-process shortcut behaviour is unchanged from
local@alt-snap-drag; the keyboard tools run in one dedicated elevated
Windhawk tool process instead of two. The tool runs elevated so it can move
administrator windows. A saved monitor index that is no longer present uses
the nearest monitor in the requested direction.

Published under GPL-3.0 because the drag code derives from alt-drag.

## Installation

Disable Snap Commander, Move Window to Monitor, AltDrag and AltSnap Drag before
enabling this mod. Also close a standalone AltSnap or AltDrag application if it
owns the same shortcuts. This mod replaces their window controls.

Until this mod is accepted into the official catalog, create a new mod in
Windhawk, paste this complete source and compile it. Configure the combined
settings page to choose your shortcuts and screen gaps. Existing separate-mod
settings are not imported automatically by a fresh editor installation.

The keyboard helper runs elevated. Windows can request administrator permission
when it starts. Allow that request to use the keyboard controls on administrator
windows. Declining the request prevents the keyboard helper from starting.

The default monitor shortcuts are Ctrl+Alt+arrows. Select Alt+Shift in Modifier
Keys to use Alt+Shift+arrows. Monitor targets default to automatic spatial
movement. A saved monitor target that no longer exists also uses spatial movement.

The source, installation instructions and licensing notices are available at
https://github.com/DavidHiFi/davids-windhawk-mods/tree/main/mods/local/david-window-pack.

*/
// ==/WindhawkModReadme==


/*
Third-party notices

AltDrag: m417z, based on AltSnap by RamonUnch and AltDrag by Stefan Sundin.
The combined mod is distributed under GPL-3.0.

Snap Commander: Copyright (c) Asteski.
Move Window to Monitor: Copyright (c) TomberWolf.
These components are available under the MIT license:

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// Merged from the installed sources:
//   local@alt-snap-drag.wh.cpp <- drag
//   snap-commander.wh.cpp <- snap
//   move-window-to-monitor.wh.cpp <- move

#include <commctrl.h>
#include <algorithm>
#include <atomic>
#include <bit>
#include <cstdlib>
#include <array>
#include <cwctype>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_set>
#include <windhawk_utils.h>
#include <windows.h>
#include <dwmapi.h>
#include <deque>
#include <unordered_map>
#include <vector>
#include <shellscalingapi.h>

//////// drag ////////
namespace altSnapDrag {

// A fork of the alt-drag mod by m417z (https://github.com/m417z/my-windhawk-mods),
// which is based on AltSnap by RamonUnch (https://github.com/RamonUnch/AltSnap)
// and the original AltDrag by Stefan Sundin. Modified in 2026 by DavidHiFi to
// add configurable AltSnap mouse actions and keyboard shortcuts.
//
// Source code is published under The GNU General Public License v3.0, like the
// original. For bug reports and feature requests, please open an issue here:
// https://github.com/DavidHiFi/davids-windhawk-mods/issues



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
// The Alt release which ends such a drag is therefore taken as well. A
// swallowed press which isn't followed by a drag is replayed on the release
// as a click, like a press taken for the loop.
//
// The Start menu, which a Win key tapped on its own opens, is put off by
// another key alone, not by a press, swallowed or not. The Win release ending
// a drag is therefore masked by a low level keyboard hook of the thread
// running the drag, the way AutoHotkey masks menu keys: it's swallowed and
// sent again behind a tap of an unassigned key.



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

enum class WindowAction : int {
    None,
    Menu,
    Maximize,
    Minimize,
    Close,
    Topmost,
    Lower,
    Center,
    SideSnap,
};

struct KeyBinding {
    std::atomic<int> key;
    std::atomic<DWORD> modifiers;
};

constexpr std::array<WindowAction, 8> kHotkeyActions{
    WindowAction::Maximize, WindowAction::Minimize, WindowAction::Close,
    WindowAction::Menu, WindowAction::Topmost, WindowAction::Lower,
    WindowAction::Center, WindowAction::SideSnap};
constexpr std::array<PCWSTR, 8> kHotkeyNames{
    L"hotkeyMaximize", L"hotkeyMinimize", L"hotkeyClose", L"hotkeyMenu",
    L"hotkeyTopmost", L"hotkeyLower", L"hotkeyCenter", L"hotkeySideSnap"};

struct {
    TriggerSettings moveTrigger;
    TriggerSettings sizeTrigger;
    std::atomic<bool> dragWindowsWithoutTitleBar;
    std::atomic<WindowAction> middleAction;
    std::atomic<WindowAction> rightClickAction;
    std::atomic<WindowAction> x1Action;
    std::atomic<WindowAction> x2Action;
    std::atomic<WindowAction> leftDoubleAction;
    std::atomic<WindowAction> middleDoubleAction;
    std::atomic<WindowAction> rightDoubleAction;
    std::array<KeyBinding, kHotkeyActions.size()> hotkeys;
} g_settings;

std::atomic<bool> g_uninitializing;
std::atomic<int> g_hookRefCount;

// A drag is identified by the system command it amounts to: SC_MOVE |
// HTCAPTION for a move, SC_SIZE | WMSZ_* for a resize from that edge or corner.
bool IsSizeCommand(UINT command) {
    return (command & 0xFFF0) == SC_SIZE;
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

thread_local bool g_threadHooksAttempted;
thread_local HHOOK g_getMessageHook;
thread_local HHOOK g_callWndProcHook;
std::mutex g_allThreadHooksMutex;
std::unordered_set<HHOOK> g_allThreadHooks;

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
thread_local int g_actionButton;
thread_local int g_actionKey;

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
// Set when a press was swallowed during the current hold with Alt held.
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
// Where the drag started: a move takes its grab from it, a resize measures
// from it.
POINT g_llDragStartPt;
// Where a move holds the window relative to its origin.
POINT g_llDragGrab;
// The window's rect when a resize started.
RECT g_llDragStartRect;
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

void ReplayPress(int button, bool asClick);
void MaskWinKeyReleaseIfNeeded(UINT command);
void OnLowLevelDragDelayElapsed();
void OnDragUpdate();
void RemoveLowLevelMouseHook();
void InstallLowLevelMouseHookIfNeeded();
void RemoveLowLevelMouseHookIfIdle();

auto HookRefCountScope() {
    g_hookRefCount++;
    return std::unique_ptr<decltype(g_hookRefCount),
                           void (*)(decltype(g_hookRefCount)*)>{
        &g_hookRefCount, [](auto hookRefCount) { (*hookRefCount)--; }};
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

bool CanActOnRootWindow(HWND hWnd) {
    return hWnd && GetAncestor(hWnd, GA_ROOT) == hWnd &&
           !IsExcludedRootWindow(hWnd) &&
           (GetWindowLong(hWnd, GWL_STYLE) & WS_CHILD) == 0;
}

WindowAction ActionFromSetting(PCWSTR value) {
    if (_wcsicmp(value, L"menu") == 0) return WindowAction::Menu;
    if (_wcsicmp(value, L"maximize") == 0) return WindowAction::Maximize;
    if (_wcsicmp(value, L"minimize") == 0) return WindowAction::Minimize;
    if (_wcsicmp(value, L"close") == 0) return WindowAction::Close;
    if (_wcsicmp(value, L"topmost") == 0) return WindowAction::Topmost;
    if (_wcsicmp(value, L"lower") == 0) return WindowAction::Lower;
    if (_wcsicmp(value, L"center") == 0) return WindowAction::Center;
    if (_wcsicmp(value, L"sideSnap") == 0) return WindowAction::SideSnap;
    return WindowAction::None;
}

bool RunWindowAction(HWND hWnd, WindowAction action, POINT pt) {
    if (action == WindowAction::None || !CanActOnRootWindow(hWnd)) return false;

    if (action == WindowAction::Menu) {
        HMENU menu = CreatePopupMenu();
        if (!menu) return false;
        constexpr std::array<std::pair<WindowAction, PCWSTR>, 7> items{{
            {WindowAction::Maximize, L"Maximize / restore"},
            {WindowAction::Minimize, L"Minimize"},
            {WindowAction::Topmost, L"Always on top"},
            {WindowAction::Lower, L"Lower"},
            {WindowAction::Center, L"Center"},
            {WindowAction::SideSnap, L"Snap to side"},
            {WindowAction::Close, L"Close"},
        }};
        for (size_t i = 0; i < items.size(); i++) {
            if (items[i].first == WindowAction::Close) AppendMenu(menu, MF_SEPARATOR, 0, nullptr);
            UINT flags = MF_STRING;
            if (items[i].first == WindowAction::Topmost &&
                (GetWindowLongPtr(hWnd, GWL_EXSTYLE) & WS_EX_TOPMOST)) flags |= MF_CHECKED;
            AppendMenu(menu, flags, i + 1, items[i].second);
        }
        int selected = TrackPopupMenuEx(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON,
                                        pt.x, pt.y, hWnd, nullptr);
        DestroyMenu(menu);
        if (selected > 0 && selected <= (int)items.size())
            return RunWindowAction(hWnd, items[selected - 1].first, pt);
        return true;
    }

    if (action == WindowAction::Maximize) {
        ShowWindowAsync(hWnd, IsZoomed(hWnd) ? SW_RESTORE : SW_MAXIMIZE);
    } else if (action == WindowAction::Minimize) {
        ShowWindowAsync(hWnd, SW_MINIMIZE);
    } else if (action == WindowAction::Close) {
        PostMessage(hWnd, WM_CLOSE, 0, 0);
    } else if (action == WindowAction::Topmost) {
        HWND order = (GetWindowLongPtr(hWnd, GWL_EXSTYLE) & WS_EX_TOPMOST)
                         ? HWND_NOTOPMOST : HWND_TOPMOST;
        SetWindowPos(hWnd, order, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    } else if (action == WindowAction::Lower) {
        SetWindowPos(hWnd, HWND_BOTTOM, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    } else if (action == WindowAction::Center || action == WindowAction::SideSnap) {
        MONITORINFO monitor{.cbSize = sizeof(monitor)};
        RECT rc;
        if (!GetMonitorInfo(MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST),
                            &monitor) || !GetWindowRect(hWnd, &rc)) return false;
        if (IsZoomed(hWnd)) ShowWindowAsync(hWnd, SW_RESTORE);
        const RECT& work = monitor.rcWork;
        int width = rc.right - rc.left;
        int height = rc.bottom - rc.top;
        if (action == WindowAction::Center) {
            SetWindowPos(hWnd, nullptr,
                         work.left + ((work.right - work.left) - width) / 2,
                         work.top + ((work.bottom - work.top) - height) / 2,
                         0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
        } else {
            int half = (work.right - work.left) / 2;
            bool right = pt.x >= work.left + half;
            SetWindowPos(hWnd, nullptr, right ? work.left + half : work.left,
                         work.top, right ? work.right - work.left - half : half,
                         work.bottom - work.top, SWP_NOZORDER | SWP_NOACTIVATE);
        }
    }
    return true;
}

WindowAction MouseActionForMessage(UINT message, int button) {
    bool dbl = message == WM_LBUTTONDBLCLK || message == WM_NCLBUTTONDBLCLK ||
               message == WM_RBUTTONDBLCLK || message == WM_NCRBUTTONDBLCLK ||
               message == WM_MBUTTONDBLCLK || message == WM_NCMBUTTONDBLCLK ||
               message == WM_XBUTTONDBLCLK || message == WM_NCXBUTTONDBLCLK;
    if (dbl) {
        if (button == VK_LBUTTON) return g_settings.leftDoubleAction;
        if (button == VK_MBUTTON) return g_settings.middleDoubleAction;
        if (button == VK_RBUTTON) return g_settings.rightDoubleAction;
    } else {
        if (button == VK_MBUTTON) return g_settings.middleAction;
        if (button == VK_XBUTTON1) return g_settings.x1Action;
        if (button == VK_XBUTTON2) return g_settings.x2Action;
    }
    return WindowAction::None;
}

bool HandleActionMouseMessage(MSG* msg, int button, bool down) {
    if (!down && button == g_actionButton) {
        g_actionButton = 0;
        TakeMessage(msg);
        return true;
    }
    if (!down || !button || !(HeldModifiers(GetKeyState) & kModifierAlt)) return false;
    WindowAction action = MouseActionForMessage(msg->message, button);
    if (action == WindowAction::None && button == VK_RBUTTON &&
        g_settings.rightClickAction != WindowAction::None) {
        HWND root = GetAncestor(msg->hwnd, GA_ROOT);
        if (root && IsZoomed(root)) action = g_settings.rightClickAction;
    }
    if (action == WindowAction::None) return false;
    HWND root = GetAncestor(msg->hwnd, GA_ROOT);
    if (!RunWindowAction(root, action, msg->pt)) return false;
    g_actionButton = button;
    g_swallowedPress = true;
    TakeMessage(msg);
    return true;
}

bool HandleActionKeyMessage(MSG* msg) {
    bool down = msg->message == WM_KEYDOWN || msg->message == WM_SYSKEYDOWN;
    if (!down && msg->wParam == (WPARAM)g_actionKey) {
        g_actionKey = 0;
        TakeMessage(msg);
        return true;
    }
    if (!down || msg->wParam == 0) return false;
    if (g_actionKey == (int)msg->wParam) {
        TakeMessage(msg);
        return true;
    }
    DWORD modifiers = HeldModifiers(GetKeyState);
    for (size_t i = 0; i < g_settings.hotkeys.size(); i++) {
        if (g_settings.hotkeys[i].key == (int)msg->wParam &&
            g_settings.hotkeys[i].modifiers == modifiers) {
            HWND root = GetAncestor(msg->hwnd, GA_ROOT);
            if (!root || !RunWindowAction(root, kHotkeyActions[i], msg->pt)) return false;
            g_actionKey = (int)msg->wParam;
            g_swallowedPress = true;
            TakeMessage(msg);
            return true;
        }
    }
    return false;
}

// The edge or corner a resize which grabs the window at pt takes hold of, from
// a 3x3 grid over the window. The center cell resizes from the bottom right
// corner.
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
        {WMSZ_LEFT, WMSZ_BOTTOMRIGHT, WMSZ_RIGHT},
        {WMSZ_BOTTOMLEFT, WMSZ_BOTTOM, WMSZ_BOTTOMRIGHT},
    };
    return SC_SIZE | kEdges[row][col];
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
// which nothing is found.
HWND CompositionHostedWindowFromPoint(POINT pt) {
    HWND hWnd = GetDesktopWindow();
    for (int depth = 0; depth < 9; depth++) {
        POINT clientPt = pt;
        if (!ScreenToClient(hWnd, &clientPt)) {
            break;
        }

        HWND hChildWnd = ChildWindowFromPointEx(
            hWnd, clientPt,
            CWP_SKIPINVISIBLE | CWP_SKIPDISABLED | CWP_SKIPTRANSPARENT);
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
           edge <= WMSZ_BOTTOMRIGHT && CanSizeRootWindow(hRootWnd);
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

    g_loop = {
        .command = command,
        .button = button,
        .hWnd = msg->hwnd,
        .startPt = msg->pt,
    };

    msg->message = WM_SYSCOMMAND;
    msg->wParam = command;
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
        SetCursor(LoadCursor(nullptr, CursorOfCommand(g_loop.command)));
        return;
    }

    bool down;
    int button = ButtonOfMessage(msg->message, HIWORD(msg->wParam), &down);

    // Same as AltSnap: while a window is being moved, the right button toggles
    // its maximized state. A maximized window is handled by the action path in
    // OnMessageRemoved, which runs first; this covers the half where a normal
    // window is maximized.
    //
    // The loop is ended at this press rather than left running: moving a
    // maximized window restores it, and the loop is cancelled, which restores
    // the window as well, if the Alt release gets through and opens the menu.
    // Ending the loop with the release it consumes, and keeping that Alt
    // release from reaching the window, leaves the toggled state as the last
    // thing that happened to the window.
    if (down && button == VK_RBUTTON && g_loop.dragged &&
        !IsSizeCommand(g_loop.command)) {
        HWND hRootWnd = g_loop.hWnd;
        POINT pt = msg->pt;

        if (IsNonClientMessage(msg->message)) {
            msg->message = WM_NCLBUTTONUP;
        } else {
            msg->message = WM_LBUTTONUP;
            msg->wParam = 0;
        }

        g_swallowedPress = true;

        if (RunWindowAction(hRootWnd, WindowAction::Maximize, pt)) {
            Wh_Log(
                L"Right button while moving: toggled the maximized state of "
                L"%08X",
                (DWORD)(ULONG_PTR)hRootWnd);
        }
        return;
    }

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
        if (button == VK_RBUTTON &&
            RunWindowAction(g_loop.hWnd, g_settings.rightClickAction, msg->pt)) {
            g_swallowedPress = true;
        } else {
            Wh_Log(L"Released within the drag threshold, replaying the click");
            ReplayPress(button, true);
        }
    }

    // The loop consumes the release.
    g_loop = {};
    g_takenPressButton = 0;
}

// The loop ended without its release being seen, e.g. cancelled with Esc.
void OnLoopOver() {
    if (GetAsyncKeyState(VK_LBUTTON) >= 0 && GetKeyState(VK_LBUTTON) < 0) {
        SetSyncKeyState(VK_LBUTTON, false);
    }

    g_loop = {};
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
        PostSizeMoveRequest(g_delayedPress.hRootWnd, g_delayedPress.command,
                            g_delayedPress.button, g_delayedPress.downMsg.pt)) {
        Wh_Log(L"Drag delay elapsed, requesting a %s of root window %08X",
               NameOfCommand(g_delayedPress.command),
               (DWORD)(ULONG_PTR)g_delayedPress.hRootWnd);

        // Shown from here on, ahead of the first move the loop sets it for:
        // the window under the cursor is this thread's, which retrieved the
        // press.
        SetCursor(LoadCursor(nullptr, CursorOfCommand(g_delayedPress.command)));
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
        } else {
            HandleActionKeyMessage(msg);
        }
        return;
    }

    bool actionDown;
    int actionButton = ButtonOfMessage(msg->message, HIWORD(msg->wParam),
                                       &actionDown);
    if (actionButton && HandleActionMouseMessage(msg, actionButton, actionDown))
        return;

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

    Wh_Log(L"Message %04X for %08X, requesting a %s of root window %08X",
           msg->message, (DWORD)(ULONG_PTR)msg->hwnd, NameOfCommand(command),
           (DWORD)(ULONG_PTR)hRootWnd);

    if (!PostSizeMoveRequest(hRootWnd, command, button, msg->pt)) {
        g_takenPressButton = 0;
        return;
    }

    TakeMessage(msg);
}

LRESULT CALLBACK GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam) {
    auto hookScope = HookRefCountScope();

    if (nCode == HC_ACTION && (wParam & PM_REMOVE) && lParam &&
        !g_uninitializing) {
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
// move which follows queues up behind it.
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
// thread, which keeps a busy owner from blocking the caller.
void MoveDraggedWindow(HWND hRootWnd, POINT grab, POINT pt) {
    SetWindowPos(
        hRootWnd, nullptr, pt.x - grab.x, pt.y - grab.y, 0, 0,
        SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
}

// The edges the command takes hold of follow the pointer from where the drag
// started, the others stay put. SetWindowPos doesn't ask the window for its
// minimum size, so the system's minimum is kept instead.
void SizeDraggedWindow(HWND hRootWnd,
                       UINT command,
                       const RECT& rcStart,
                       POINT ptStart,
                       POINT pt) {
    UINT edge = command & 0xF;
    LONG dx = pt.x - ptStart.x;
    LONG dy = pt.y - ptStart.y;
    LONG minWidth = GetSystemMetrics(SM_CXMINTRACK);
    LONG minHeight = GetSystemMetrics(SM_CYMINTRACK);

    RECT rc = rcStart;
    switch (edge) {
        case WMSZ_LEFT:
        case WMSZ_TOPLEFT:
        case WMSZ_BOTTOMLEFT:
            rc.left = std::min(rcStart.left + dx, rc.right - minWidth);
            break;
        case WMSZ_RIGHT:
        case WMSZ_TOPRIGHT:
        case WMSZ_BOTTOMRIGHT:
            rc.right = std::max(rcStart.right + dx, rc.left + minWidth);
            break;
    }

    switch (edge) {
        case WMSZ_TOP:
        case WMSZ_TOPLEFT:
        case WMSZ_TOPRIGHT:
            rc.top = std::min(rcStart.top + dy, rc.bottom - minHeight);
            break;
        case WMSZ_BOTTOM:
        case WMSZ_BOTTOMLEFT:
        case WMSZ_BOTTOMRIGHT:
            rc.bottom = std::max(rcStart.bottom + dy, rc.top + minHeight);
            break;
    }

    SetWindowPos(hRootWnd, nullptr, rc.left, rc.top, rc.right - rc.left,
                 rc.bottom - rc.top,
                 SWP_NOZORDER | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
}

void DragWindowTo(POINT pt) {
    if (IsSizeCommand(g_llCommand)) {
        SizeDraggedWindow(g_llRootWnd, g_llCommand, g_llDragStartRect,
                          g_llDragStartPt, pt);
    } else {
        MoveDraggedWindow(g_llRootWnd, g_llDragGrab, pt);
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

// Through DefWindowProc, so on the window's own thread wherever this runs.
void DestroyDragOverlay() {
    if (HWND hOverlayWnd = g_llDragOverlayWnd) {
        g_llDragOverlayWnd = nullptr;
        PostMessage(hOverlayWnd, WM_CLOSE, 0, 0);
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
    HWND hFocusWnd = GetFocus();
    if (!hFocusWnd ||
        !PostMessage(hFocusWnd, g_replayPressMessage, button, asClick)) {
        PostThreadMessage(GetCurrentThreadId(), g_replayPressMessage, button,
                          asClick);
    }
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
    g_llDragStartPt = pt;
    KillLowLevelDelayTimer();
}

// The work a drag starts with, none of which belongs in the callback: the
// restore of a maximized window runs the program's layout, and it runs inline
// when the window is of the hook's own thread, which the foreground process's
// window commonly is.
void SetUpLowLevelDrag() {
    if (IsSizeCommand(g_llCommand)) {
        GetWindowRect(g_llRootWnd, &g_llDragStartRect);
    } else {
        g_llDragGrab = CalcDragGrab(g_llRootWnd, g_llDragStartPt);
    }

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
            CreateDragOverlay(g_llDragStartPt, CursorOfCommand(g_llCommand));
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
        DragWindowTo(g_llDragPt);
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

    // GetFocus is of this thread, so the update is retrieved here, where the
    // drag state belongs.
    HWND hFocusWnd = GetFocus();
    if (hFocusWnd && PostMessage(hFocusWnd, g_dragUpdateMessage, 0, 0)) {
        g_llDragUpdatePosted = true;
    } else if (PostThreadMessage(GetCurrentThreadId(), g_dragUpdateMessage, 0,
                                 0)) {
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
        } else if (button == VK_RBUTTON && g_llRootWnd && g_llDragging &&
                   !IsSizeCommand(g_llCommand) &&
                   RunWindowAction(g_llRootWnd, WindowAction::Maximize,
                                   ToLogicalPoint(ms->pt))) {
            // Same as AltSnap: while a window is being moved, the right button
            // toggles its maximized state. The Alt release of the drag is kept
            // from reaching the window, which would open its menu.
            g_swallowedPress = true;
            return 1;
        }

        if (!g_llRootWnd && OnLowLevelButtonDown(ms, button)) {
            return 1;
        }
    } else if (button && button == g_llButton) {
        // The reset goes first: it closes the overlay, if the delay put one
        // up, with a message queued ahead of the replay, which would
        // otherwise land on it.
        bool dragging = g_llDragging;
        HWND root = g_llRootWnd;
        ResetLowLevelDragState();
        if (!dragging) {
            if (button == VK_RBUTTON &&
                RunWindowAction(root, g_settings.rightClickAction,
                                ToLogicalPoint(ms->pt))) {
                g_swallowedPress = true;
            } else {
                PostReplayPress(button, true);
            }
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

    [[clang::musttail]] return NtUserPeekMessage_Original(
        pMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg, flags);
}

BOOL WINAPI DragDllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
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

void LoadActionSetting(std::atomic<WindowAction>& action, PCWSTR name) {
    PCWSTR value = Wh_GetStringSetting(L"%ls", name);
    action = ActionFromSetting(value);
    Wh_FreeStringSetting(value);
}

void ParseHotkey(PCWSTR value, int* key, DWORD* modifiers) {
    *key = 0;
    *modifiers = 0;
    if (!value || !*value) return;

    WCHAR* end = nullptr;
    unsigned long encoded = wcstoul(value, &end, 10);
    if (end != value && *end == 0 && encoded <= 0xFFFF) {
        *key = encoded & 0xFF;
        unsigned long flags = encoded >> 8;
        if (flags & MOD_ALT) *modifiers |= kModifierAlt;
        if (flags & MOD_CONTROL) *modifiers |= kModifierCtrl;
        if (flags & MOD_SHIFT) *modifiers |= kModifierShift;
        if (flags & MOD_WIN) *modifiers |= kModifierWin;
        return;
    }

    std::wstring part;
    auto parsePart = [&](const std::wstring& token) {
        if (token == L"ALT") *modifiers |= kModifierAlt;
        else if (token == L"CTRL" || token == L"CONTROL") *modifiers |= kModifierCtrl;
        else if (token == L"SHIFT") *modifiers |= kModifierShift;
        else if (token == L"WIN" || token == L"WINDOWS") *modifiers |= kModifierWin;
        else if (token.size() == 1 && token[0] >= L'A' && token[0] <= L'Z') *key = token[0];
        else if (token.size() == 1 && token[0] >= L'0' && token[0] <= L'9') *key = token[0];
        else if (token[0] == L'F' && token.size() <= 3) {
            int number = _wtoi(token.c_str() + 1);
            if (number >= 1 && number <= 24) *key = VK_F1 + number - 1;
        } else if (token == L"SPACE") *key = VK_SPACE;
        else if (token == L"TAB") *key = VK_TAB;
        else if (token == L"ESC" || token == L"ESCAPE") *key = VK_ESCAPE;
        else if (token == L"DELETE" || token == L"DEL") *key = VK_DELETE;
        else if (token == L"INSERT" || token == L"INS") *key = VK_INSERT;
        else if (token == L"HOME") *key = VK_HOME;
        else if (token == L"END") *key = VK_END;
        else if (token == L"PAGEUP" || token == L"PGUP") *key = VK_PRIOR;
        else if (token == L"PAGEDOWN" || token == L"PGDN") *key = VK_NEXT;
        else if (token == L"LEFT") *key = VK_LEFT;
        else if (token == L"RIGHT") *key = VK_RIGHT;
        else if (token == L"UP") *key = VK_UP;
        else if (token == L"DOWN") *key = VK_DOWN;
    };
    for (PCWSTR p = value;; p++) {
        if (*p == L'+' || *p == 0) {
            if (!part.empty()) parsePart(part);
            part.clear();
            if (!*p) break;
        } else if (!iswspace(*p)) {
            part += (WCHAR)towupper(*p);
        }
    }
    // A shortcut without modifiers would intercept ordinary typing.
    if (!*modifiers) *key = 0;
}

void LoadSettings() {
    LoadTriggerSettings(g_settings.moveTrigger, L"moveTrigger");
    LoadTriggerSettings(g_settings.sizeTrigger, L"sizeTrigger");
    g_settings.dragWindowsWithoutTitleBar =
        Wh_GetIntSetting(L"dragWindowsWithoutTitleBar");
    LoadActionSetting(g_settings.middleAction, L"middleAction");
    LoadActionSetting(g_settings.rightClickAction, L"rightClickAction");
    LoadActionSetting(g_settings.x1Action, L"x1Action");
    LoadActionSetting(g_settings.x2Action, L"x2Action");
    LoadActionSetting(g_settings.leftDoubleAction, L"leftDoubleAction");
    LoadActionSetting(g_settings.middleDoubleAction, L"middleDoubleAction");
    LoadActionSetting(g_settings.rightDoubleAction, L"rightDoubleAction");
    for (size_t i = 0; i < kHotkeyNames.size(); i++) {
        PCWSTR value = Wh_GetStringSetting(L"%ls", kHotkeyNames[i]);
        int key;
        DWORD modifiers;
        ParseHotkey(value, &key, &modifiers);
        Wh_FreeStringSetting(value);
        g_settings.hotkeys[i].key = key;
        g_settings.hotkeys[i].modifiers = modifiers;
    }
}

BOOL ModInit() {
    Wh_Log(L">");

    LoadSettings();

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

void ModUninit() {
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
    DestroyDragOverlay();
    UnregisterClass(kDragOverlayClassName, GetModuleHandle(nullptr));
}

void ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}
}  // namespace altSnapDrag

//////// snap ////////
namespace snapCommander {





enum class Action {
    None,
    LeftHalf,
    RightHalf,
    TopHalf,
    BottomHalf,
    CenterHalf,
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight,
    Maximize,
    AlmostMaximize,
    Center,
    Minimize,
    NextDisplay,
    PreviousDisplay,
    MakeSmaller,
    MakeLarger,
    RestorePrevious,
};

constexpr UINT MOD_F_ALT = 0x1;
constexpr UINT MOD_F_CTRL = 0x2;
constexpr UINT MOD_F_SHIFT = 0x4;
constexpr UINT MOD_F_WIN = 0x8;

constexpr int ACTION_COUNT = 18;

// Posted to the hook thread after settings load, so it can re-register the hotkeys
// and install/remove the auto-snap WinEvent hook (both must run on that thread).
constexpr UINT WM_APP_SETTINGS_CHANGED = WM_APP + 1;

// Gaps (in pixels) left on each side of a snapped window.
static int g_gapTop = 0;
static int g_gapBottom = 0;
static int g_gapLeft = 0;
static int g_gapRight = 0;

// How the gap on a window side is sized depending on whether that side touches a
// screen edge (outer) or borders another tiled window (inner divider):
//   Even   - outer sides get the full gap, inner sides get half, so two adjacent
//            windows show the same total gap as a single window at the edge.
//   Full   - every side gets the full gap (adjacent windows show a doubled gap).
//   Screen - only sides touching a screen edge get a gap; inner sides get none.
enum class GapMode { Even, Full, Screen };
static GapMode g_gapMode = GapMode::Even;

// Target size for the Center action; 0 means keep the window's current size.
static int g_centerWidth = 0;
static int g_centerHeight = 0;

// Step (percentage of the monitor work area) by which Make smaller / larger
// grow or shrink the active window.
static int g_resizeStepPercent = 5;

// Per-action trigger key and required modifier mask; both indexes line up with
// g_actionByIndex below. A key of 0 means the action is unbound. The mask uses the
// MOD_F_* bits, which deliberately match RegisterHotKey's MOD_ALT/CONTROL/SHIFT/WIN
// values, so it can be passed straight to RegisterHotKey as the fsModifiers.
static UINT g_actionKeys[ACTION_COUNT] = {};
static UINT g_actionMods[ACTION_COUNT] = {};
static const Action g_actionByIndex[ACTION_COUNT] = {
    Action::LeftHalf,    Action::RightHalf,     Action::TopHalf,
    Action::BottomHalf,  Action::CenterHalf,    Action::TopLeft,
    Action::TopRight,    Action::BottomLeft,    Action::BottomRight,
    Action::Maximize,    Action::AlmostMaximize, Action::Center,
    Action::Minimize,    Action::NextDisplay,    Action::PreviousDisplay,
    Action::MakeSmaller, Action::MakeLarger,     Action::RestorePrevious,
};

// Setting names for each action's shortcut, lined up with g_actionByIndex. Used both
// to read the bindings and to name a binding in the log when it can't be registered.
static const PCWSTR g_keyNames[ACTION_COUNT] = {
    L"shortcuts.keyLeftHalf",       L"shortcuts.keyRightHalf",
    L"shortcuts.keyTopHalf",        L"shortcuts.keyBottomHalf",
    L"shortcuts.keyCenterHalf",     L"shortcuts.keyTopLeft",
    L"shortcuts.keyTopRight",       L"shortcuts.keyBottomLeft",
    L"shortcuts.keyBottomRight",    L"shortcuts.keyMaximize",
    L"shortcuts.keyAlmostMaximize", L"shortcuts.keyCenter",
    L"shortcuts.keyMinimize",       L"shortcuts.keyNextDisplay",
    L"shortcuts.keyPrevDisplay",    L"shortcuts.keySmaller",
    L"shortcuts.keyLarger",         L"shortcuts.keyRestore",
};

// Which hotkey ids are currently registered. Touched only on the hook thread.
static bool g_hotkeyRegistered[ACTION_COUNT] = {};

// Window placement captured before the first snap, so RestorePrevious can revert.
// Accessed only from the worker thread, so no synchronization is needed.
static std::unordered_map<HWND, WINDOWPLACEMENT> g_savedPlacements;

// Auto-snap rules: when a window of a matching executable is shown, the chosen
// action is applied to it automatically.
struct AppRule {
    std::vector<std::wstring> executables;
    Action action = Action::None;
    bool noResize = false;  // Move the window into the region but keep its size.
};
static std::vector<AppRule> g_appRules;
static std::mutex g_appRulesMutex;

// Windows already auto-snapped by a rule, so a single window isn't snapped
// repeatedly as it raises further show events.
static std::unordered_set<HWND> g_ruleHandledWindows;
static std::mutex g_ruleHandledMutex;

static HWINEVENTHOOK g_winEventHook = nullptr;
static HANDLE g_hookThread = nullptr;
static DWORD g_hookThreadId = 0;
static HANDLE g_hookReadyEvent = nullptr;

// Work queue drained by the worker thread; producers are the hotkey/window-event
// hooks, so it is guarded by a mutex. A "cleanup" item carries no action and just
// tells the worker (the sole owner of g_savedPlacements) to drop a destroyed window.
struct WorkItem {
    HWND hWnd;
    Action action;
    bool noResize;
    bool cleanup;
};
static std::deque<WorkItem> g_workQueue;
static std::mutex g_workQueueMutex;

static HANDLE g_workerThread = nullptr;
static HANDLE g_workEvent = nullptr;
static std::atomic<bool> g_quit{false};

static void EnqueueWork(HWND hWnd, Action action, bool noResize = false) {
    if (!hWnd || action == Action::None) {
        return;
    }
    {
        std::lock_guard<std::mutex> lock(g_workQueueMutex);
        g_workQueue.push_back({hWnd, action, noResize, false});
    }
    SetEvent(g_workEvent);
}

// Ask the worker thread to forget a window's saved placement once it's destroyed,
// so the map doesn't grow without bound and a recycled HWND can't inherit a stale
// placement.
static void EnqueueCleanup(HWND hWnd) {
    if (!hWnd) {
        return;
    }
    {
        std::lock_guard<std::mutex> lock(g_workQueueMutex);
        g_workQueue.push_back({hWnd, Action::None, false, true});
    }
    SetEvent(g_workEvent);
}

static std::wstring ToLower(std::wstring s) {
    for (auto& c : s) {
        c = (wchar_t)towlower(c);
    }
    return s;
}

static std::wstring Trim(const std::wstring& s) {
    size_t start = 0;
    while (start < s.size() && iswspace(s[start])) {
        start++;
    }
    size_t end = s.size();
    while (end > start && iswspace(s[end - 1])) {
        end--;
    }
    return s.substr(start, end - start);
}

static UINT GetVkFromKeyName(const std::wstring& name) {
    if (name == L"left") {
        return VK_LEFT;
    }
    if (name == L"right") {
        return VK_RIGHT;
    }
    if (name == L"up") {
        return VK_UP;
    }
    if (name == L"down") {
        return VK_DOWN;
    }
    // Punctuation keys, by symbol or by word. Note: the shortcut parser splits
    // on "+", so the plus key must be written as "plus" (or "=" / "equals").
    if (name == L"[" || name == L"lbracket" || name == L"leftbracket") {
        return VK_OEM_4;
    }
    if (name == L"]" || name == L"rbracket" || name == L"rightbracket") {
        return VK_OEM_6;
    }
    if (name == L"-" || name == L"minus") {
        return VK_OEM_MINUS;
    }
    if (name == L"+" || name == L"plus" || name == L"=" || name == L"equals") {
        return VK_OEM_PLUS;
    }
    if (name.size() == 1) {
        wchar_t c = name[0];
        if (c >= L'a' && c <= L'z') {
            return (UINT)(L'A' + (c - L'a'));
        }
        if (c >= L'0' && c <= L'9') {
            return (UINT)c;
        }
    }
    return 0;
}

static UINT ParseModifierMask(const std::wstring& value) {
    if (value == L"none") {
        return 0;
    }
    if (value == L"alt") {
        return MOD_F_ALT;
    }
    if (value == L"ctrl") {
        return MOD_F_CTRL;
    }
    if (value == L"shift") {
        return MOD_F_SHIFT;
    }
    if (value == L"win") {
        return MOD_F_WIN;
    }
    if (value == L"alt_ctrl") {
        return MOD_F_ALT | MOD_F_CTRL;
    }
    if (value == L"alt_shift") {
        return MOD_F_ALT | MOD_F_SHIFT;
    }
    if (value == L"ctrl_shift") {
        return MOD_F_CTRL | MOD_F_SHIFT;
    }
    if (value == L"win_alt") {
        return MOD_F_WIN | MOD_F_ALT;
    }
    if (value == L"win_ctrl") {
        return MOD_F_WIN | MOD_F_CTRL;
    }
    if (value == L"win_shift") {
        return MOD_F_WIN | MOD_F_SHIFT;
    }
    return MOD_F_ALT;
}

// Maps a single modifier word (alt/ctrl/shift/win and common aliases) to its bit.
// Returns true and sets *bit if the token is a recognised modifier.
static bool ParseModifierToken(const std::wstring& token, UINT* bit) {
    if (token == L"alt" || token == L"menu") {
        *bit = MOD_F_ALT;
        return true;
    }
    if (token == L"ctrl" || token == L"control") {
        *bit = MOD_F_CTRL;
        return true;
    }
    if (token == L"shift") {
        *bit = MOD_F_SHIFT;
        return true;
    }
    if (token == L"win" || token == L"super" || token == L"meta" ||
        token == L"cmd") {
        *bit = MOD_F_WIN;
        return true;
    }
    return false;
}

// Parses a "[mod+]...key" shortcut string into a key code and modifier mask.
// Returns true only if a key token was found; modifier-only strings are invalid.
static bool ParseShortcut(const std::wstring& text, UINT* outVk, UINT* outMods) {
    *outVk = 0;
    *outMods = 0;
    bool haveKey = false;

    size_t start = 0;
    while (start <= text.size()) {
        size_t pos = text.find(L'+', start);
        std::wstring token = ToLower(Trim(
            pos == std::wstring::npos ? text.substr(start)
                                      : text.substr(start, pos - start)));

        if (!token.empty()) {
            UINT bit = 0;
            if (ParseModifierToken(token, &bit)) {
                *outMods |= bit;
            } else {
                UINT vk = GetVkFromKeyName(token);
                if (vk != 0) {
                    *outVk = vk;
                    haveKey = true;
                }
            }
        }

        if (pos == std::wstring::npos) {
            break;
        }
        start = pos + 1;
    }

    return haveKey;
}

// File name portion of a path (handles both slash styles).
static std::wstring BaseName(const std::wstring& path) {
    size_t pos = path.find_last_of(L"\\/");
    return pos == std::wstring::npos ? path : path.substr(pos + 1);
}

// Reduces a user-entered executable reference to a bare lowercase file name,
// e.g. `"C:\Windows\notepad"` -> `notepad.exe`.
static std::wstring NormalizeExecutableName(std::wstring name) {
    name = Trim(name);
    if (name.size() >= 2 && name.front() == L'"' && name.back() == L'"') {
        name = name.substr(1, name.size() - 2);
    }
    if (name.empty()) {
        return name;
    }
    name = ToLower(Trim(BaseName(name)));
    if (!name.empty() && name.find(L'.') == std::wstring::npos) {
        name += L".exe";
    }
    return name;
}

// Splits a comma/semicolon/pipe-separated list of executable names, normalizing
// and de-duplicating each entry.
static std::vector<std::wstring> SplitExecutableList(const std::wstring& value) {
    std::vector<std::wstring> result;
    size_t start = 0;
    while (start <= value.size()) {
        size_t pos = value.find_first_of(L",;|", start);
        std::wstring part = pos == std::wstring::npos
                                ? value.substr(start)
                                : value.substr(start, pos - start);

        std::wstring normalized = NormalizeExecutableName(part);
        if (!normalized.empty() &&
            std::find(result.begin(), result.end(), normalized) ==
                result.end()) {
            result.push_back(normalized);
        }

        if (pos == std::wstring::npos) {
            break;
        }
        start = pos + 1;
    }
    return result;
}

static Action ParseActionName(const std::wstring& value) {
    std::wstring v = ToLower(Trim(value));
    if (v == L"left-half") return Action::LeftHalf;
    if (v == L"right-half") return Action::RightHalf;
    if (v == L"top-half") return Action::TopHalf;
    if (v == L"bottom-half") return Action::BottomHalf;
    if (v == L"center-half") return Action::CenterHalf;
    if (v == L"top-left") return Action::TopLeft;
    if (v == L"top-right") return Action::TopRight;
    if (v == L"bottom-left") return Action::BottomLeft;
    if (v == L"bottom-right") return Action::BottomRight;
    if (v == L"maximize") return Action::Maximize;
    if (v == L"almost-maximize") return Action::AlmostMaximize;
    if (v == L"center") return Action::Center;
    if (v == L"minimize") return Action::Minimize;
    return Action::None;
}

// Bare lowercase executable name of the process that owns the window.
static std::wstring GetWindowProcessExeName(HWND hWnd) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (!pid) {
        return L"";
    }

    HANDLE process =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        return L"";
    }

    wchar_t path[MAX_PATH] = {};
    DWORD size = ARRAYSIZE(path);
    std::wstring result;
    if (QueryFullProcessImageNameW(process, 0, path, &size)) {
        result = ToLower(BaseName(std::wstring(path, size)));
    }
    CloseHandle(process);
    return result;
}

// Finds the action (and no-resize flag) for the first rule matching exeName.
// Returns Action::None if no rule matches.
static Action FindRuleAction(const std::wstring& exeName, bool* outNoResize) {
    *outNoResize = false;
    if (exeName.empty()) {
        return Action::None;
    }
    std::lock_guard<std::mutex> lock(g_appRulesMutex);
    for (const auto& rule : g_appRules) {
        for (const auto& exe : rule.executables) {
            if (exe == exeName) {
                *outNoResize = rule.noResize;
                return rule.action;
            }
        }
    }
    return Action::None;
}

static bool IsEligibleWindow(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) {
        return false;
    }

    if (GetAncestor(hWnd, GA_ROOT) != hWnd) {
        return false;
    }

    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_TOOLWINDOW) {
        return false;
    }

    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    if (!(style & WS_CAPTION)) {
        return false;
    }

    return true;
}

// Position the window so its *visible* bounds match the given screen rectangle.
// GetWindowRect/SetWindowPos coordinates include the invisible drop-shadow border
// that DWM draws around a window, so measure it and expand the target accordingly.
static void ApplyVisibleRect(HWND hWnd, int x, int y, int w, int h) {
    RECT windowRect = {};
    RECT frameRect = {};
    int marginLeft = 0, marginTop = 0, marginRight = 0, marginBottom = 0;
    if (GetWindowRect(hWnd, &windowRect) &&
        SUCCEEDED(DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS,
                                        &frameRect, sizeof(frameRect)))) {
        marginLeft = frameRect.left - windowRect.left;
        marginTop = frameRect.top - windowRect.top;
        marginRight = windowRect.right - frameRect.right;
        marginBottom = windowRect.bottom - frameRect.bottom;
    }

    SetWindowPos(hWnd, nullptr,
                 x - marginLeft,
                 y - marginTop,
                 w + marginLeft + marginRight,
                 h + marginTop + marginBottom,
                 SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
}

// Current visible bounds of the window (excludes the invisible drop-shadow border).
static void GetVisibleRect(HWND hWnd, RECT* rect) {
    if (FAILED(DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, rect,
                                     sizeof(*rect)))) {
        GetWindowRect(hWnd, rect);
    }
}

// Current visible size of the window (excludes the invisible drop-shadow border).
static void GetVisibleSize(HWND hWnd, int* width, int* height) {
    RECT rect = {};
    GetVisibleRect(hWnd, &rect);
    *width = rect.right - rect.left;
    *height = rect.bottom - rect.top;
}

static BOOL CALLBACK CollectMonitorsProc(HMONITOR hMonitor, HDC, LPRECT,
                                         LPARAM lParam) {
    auto* works = reinterpret_cast<std::vector<RECT>*>(lParam);
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    if (GetMonitorInfoW(hMonitor, &mi)) {
        works->push_back(mi.rcWork);
    }
    return TRUE;
}

// Moves the window to the next/previous monitor (direction +1/-1, wrapping),
// keeping its size and position relative to the work area (scaled if the target
// monitor has a different work-area size). Preserves a maximized state.
static void MoveToAdjacentDisplay(HWND hWnd, int direction) {
    std::vector<RECT> works;
    EnumDisplayMonitors(nullptr, nullptr, CollectMonitorsProc,
                        reinterpret_cast<LPARAM>(&works));
    if (works.size() < 2) {
        return;  // Nothing to move to.
    }

    // Order monitors left-to-right, then top-to-bottom, for predictable cycling.
    std::sort(works.begin(), works.end(), [](const RECT& a, const RECT& b) {
        if (a.left != b.left) {
            return a.left < b.left;
        }
        return a.top < b.top;
    });

    MONITORINFO cur = {};
    cur.cbSize = sizeof(cur);
    if (!GetMonitorInfoW(MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST),
                         &cur)) {
        return;
    }

    int curIndex = -1;
    for (size_t i = 0; i < works.size(); i++) {
        if (EqualRect(&works[i], &cur.rcWork)) {
            curIndex = (int)i;
            break;
        }
    }
    if (curIndex < 0) {
        return;
    }

    const int n = (int)works.size();
    const RECT& src = works[curIndex];
    const RECT& dst = works[((curIndex + direction) % n + n) % n];

    const int srcW = src.right - src.left;
    const int srcH = src.bottom - src.top;
    const int dstW = dst.right - dst.left;
    const int dstH = dst.bottom - dst.top;
    if (srcW <= 0 || srcH <= 0 || dstW <= 0 || dstH <= 0) {
        return;
    }

    const bool wasZoomed = IsZoomed(hWnd) != FALSE;
    if (wasZoomed || IsIconic(hWnd)) {
        ShowWindow(hWnd, SW_RESTORE);
    }

    RECT vis = {};
    GetVisibleRect(hWnd, &vis);

    // Map the window rect from the source work area to the destination one.
    auto mapX = [&](LONG x) {
        return dst.left + (int)((double)(x - src.left) * dstW / srcW);
    };
    auto mapY = [&](LONG y) {
        return dst.top + (int)((double)(y - src.top) * dstH / srcH);
    };

    int x = mapX(vis.left);
    int y = mapY(vis.top);
    int w = (int)((double)(vis.right - vis.left) * dstW / srcW);
    int h = (int)((double)(vis.bottom - vis.top) * dstH / srcH);
    if (w < 1) w = 1;
    if (h < 1) h = 1;

    ApplyVisibleRect(hWnd, x, y, w, h);

    if (wasZoomed) {
        ShowWindow(hWnd, SW_MAXIMIZE);  // Re-maximize on the new monitor.
    }
}

// Grows or shrinks the window about its center by the configured step
// (direction +1 = larger, -1 = smaller), clamped to the monitor work area.
static void ResizeFromCenter(HWND hWnd, int direction) {
    if (IsIconic(hWnd) || IsZoomed(hWnd)) {
        ShowWindow(hWnd, SW_RESTORE);
    }

    RECT vis = {};
    GetVisibleRect(hWnd, &vis);
    const int w = vis.right - vis.left;
    const int h = vis.bottom - vis.top;
    const int cx = vis.left + w / 2;
    const int cy = vis.top + h / 2;

    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST),
                         &mi)) {
        return;
    }
    const RECT& work = mi.rcWork;
    const int workW = work.right - work.left;
    const int workH = work.bottom - work.top;

    int stepX = workW * g_resizeStepPercent / 100;
    int stepY = workH * g_resizeStepPercent / 100;
    if (stepX < 1) stepX = 1;
    if (stepY < 1) stepY = 1;

    int newW = w + direction * stepX;
    int newH = h + direction * stepY;

    // Clamp between a small minimum and the full work area.
    const int minW = 200;
    const int minH = 150;
    newW = std::max(minW, std::min(newW, workW));
    newH = std::max(minH, std::min(newH, workH));

    int x = cx - newW / 2;
    int y = cy - newH / 2;
    if (x < work.left) x = work.left;
    if (y < work.top) y = work.top;
    if (x + newW > work.right) x = work.right - newW;
    if (y + newH > work.bottom) y = work.bottom - newH;

    ApplyVisibleRect(hWnd, x, y, newW, newH);
}

static void SavePlacementIfNew(HWND hWnd) {
    if (g_savedPlacements.find(hWnd) != g_savedPlacements.end()) {
        return;
    }
    WINDOWPLACEMENT wp = {};
    wp.length = sizeof(wp);
    if (GetWindowPlacement(hWnd, &wp)) {
        g_savedPlacements[hWnd] = wp;
    }
}

static void RestorePreviousPlacement(HWND hWnd) {
    auto it = g_savedPlacements.find(hWnd);
    if (it == g_savedPlacements.end()) {
        return;
    }
    SetWindowPlacement(hWnd, &it->second);
    g_savedPlacements.erase(it);
}

static void SnapWindow(HWND hWnd, Action action, bool noResize) {
    if (action == Action::None || !IsEligibleWindow(hWnd)) {
        return;
    }

    if (action == Action::RestorePrevious) {
        RestorePreviousPlacement(hWnd);
        return;
    }

    // Remember where the window was before its first snap so it can be restored.
    SavePlacementIfNew(hWnd);

    if (action == Action::Minimize) {
        ShowWindow(hWnd, SW_MINIMIZE);
        return;
    }

    // Maximize fills the work area; with "don't resize" it instead keeps its
    // size and is centered in the work area (handled by the cell logic below).
    if (action == Action::Maximize && !noResize) {
        ShowWindow(hWnd, SW_MAXIMIZE);
        return;
    }

    if (action == Action::NextDisplay || action == Action::PreviousDisplay) {
        MoveToAdjacentDisplay(hWnd, action == Action::NextDisplay ? 1 : -1);
        return;
    }

    if (action == Action::MakeSmaller || action == Action::MakeLarger) {
        ResizeFromCenter(hWnd, action == Action::MakeLarger ? 1 : -1);
        return;
    }

    if (IsIconic(hWnd) || IsZoomed(hWnd)) {
        ShowWindow(hWnd, SW_RESTORE);
    }

    HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(monitor, &mi)) {
        return;
    }

    const RECT& work = mi.rcWork;
    const int fullWidth = work.right - work.left;
    const int fullHeight = work.bottom - work.top;
    const int halfWidth = fullWidth / 2;
    const int midX = work.left + halfWidth;
    const int midY = work.top + fullHeight / 2;

    if (action == Action::Center) {
        int curW = 0, curH = 0;
        GetVisibleSize(hWnd, &curW, &curH);
        int w = (!noResize && g_centerWidth > 0) ? g_centerWidth : curW;
        int h = (!noResize && g_centerHeight > 0) ? g_centerHeight : curH;
        int x = work.left + (fullWidth - w) / 2;
        int y = work.top + (fullHeight - h) / 2;
        ApplyVisibleRect(hWnd, x, y, w, h);
        return;
    }

    // The region of the work area this action targets, before gaps.
    RECT cell = work;
    switch (action) {
        case Action::LeftHalf:
            cell = {work.left, work.top, midX, work.bottom};
            break;
        case Action::RightHalf:
            cell = {midX, work.top, work.right, work.bottom};
            break;
        case Action::TopHalf:
            cell = {work.left, work.top, work.right, midY};
            break;
        case Action::BottomHalf:
            cell = {work.left, midY, work.right, work.bottom};
            break;
        case Action::CenterHalf: {
            int left = work.left + (fullWidth - halfWidth) / 2;
            cell = {left, work.top, left + halfWidth, work.bottom};
            break;
        }
        case Action::TopLeft:
            cell = {work.left, work.top, midX, midY};
            break;
        case Action::TopRight:
            cell = {midX, work.top, work.right, midY};
            break;
        case Action::BottomLeft:
            cell = {work.left, midY, midX, work.bottom};
            break;
        case Action::BottomRight:
            cell = {midX, midY, work.right, work.bottom};
            break;
        case Action::AlmostMaximize:
        case Action::Maximize:  // Only reached with noResize; ignores gaps.
            cell = work;
            break;
        default:
            return;
    }

    // Inset each side by its gap. Maximize ignores gaps entirely. Inner sides
    // (those not coinciding with the work-area edge) are sized per the gap mode:
    // full gap (doubles between windows), half gap (even spacing), or none.
    if (action != Action::Maximize) {
        auto sideGap = [](bool atScreenEdge, int gap) -> int {
            if (atScreenEdge) {
                return gap;
            }
            switch (g_gapMode) {
                case GapMode::Even:
                    return gap / 2;
                case GapMode::Screen:
                    return 0;
                case GapMode::Full:
                default:
                    return gap;
            }
        };
        cell.left += sideGap(cell.left == work.left, g_gapLeft);
        cell.top += sideGap(cell.top == work.top, g_gapTop);
        cell.right -= sideGap(cell.right == work.right, g_gapRight);
        cell.bottom -= sideGap(cell.bottom == work.bottom, g_gapBottom);
    }
    if (cell.right <= cell.left || cell.bottom <= cell.top) {
        return;
    }

    if (!noResize) {
        ApplyVisibleRect(hWnd, cell.left, cell.top, cell.right - cell.left,
                         cell.bottom - cell.top);
        return;
    }

    // Keep the window's size and anchor it within the target cell. The anchor
    // edge follows the action (e.g. top-right -> top and right edges of the
    // cell), so the unresized window sits in the matching corner/side.
    int w = 0, h = 0;
    GetVisibleSize(hWnd, &w, &h);
    const int cellW = cell.right - cell.left;
    const int cellH = cell.bottom - cell.top;

    int ax = 0, ay = 0;  // -1 = left/top, 0 = center, 1 = right/bottom.
    switch (action) {
        case Action::LeftHalf:   ax = -1; ay = 0;  break;
        case Action::RightHalf:  ax = 1;  ay = 0;  break;
        case Action::TopHalf:    ax = 0;  ay = -1; break;
        case Action::BottomHalf: ax = 0;  ay = 1;  break;
        case Action::TopLeft:    ax = -1; ay = -1; break;
        case Action::TopRight:   ax = 1;  ay = -1; break;
        case Action::BottomLeft: ax = -1; ay = 1;  break;
        case Action::BottomRight:ax = 1;  ay = 1;  break;
        default:                 ax = 0;  ay = 0;  break;  // CenterHalf, (Almost)Maximize.
    }

    int x = ax < 0 ? cell.left
            : ax > 0 ? cell.right - w
                     : cell.left + (cellW - w) / 2;
    int y = ay < 0 ? cell.top
            : ay > 0 ? cell.bottom - h
                     : cell.top + (cellH - h) / 2;

    if (x < work.left) {
        x = work.left;
    }
    if (y < work.top) {
        y = work.top;
    }

    ApplyVisibleRect(hWnd, x, y, w, h);
}

// Drops every currently-registered hotkey. Must run on the hook thread (the thread
// that called RegisterHotKey owns the hotkeys and receives their WM_HOTKEY messages).
static void UnregisterHotkeys() {
    for (int i = 0; i < ACTION_COUNT; i++) {
        if (g_hotkeyRegistered[i]) {
            UnregisterHotKey(nullptr, i);
            g_hotkeyRegistered[i] = false;
        }
    }
}

// (Re)registers a thread-global hotkey per bound action, using the action index as the
// hotkey id. Consuming the whole combo avoids the stray-modifier side effects of a
// low-level keyboard hook. Must run on the hook thread.
static void RegisterHotkeys() {
    UnregisterHotkeys();
    for (int i = 0; i < ACTION_COUNT; i++) {
        if (g_actionKeys[i] == 0) {
            continue;
        }
        // MOD_NOREPEAT: one action per physical press, not once per auto-repeat.
        if (RegisterHotKey(nullptr, i, g_actionMods[i] | MOD_NOREPEAT,
                           g_actionKeys[i])) {
            g_hotkeyRegistered[i] = true;
        } else {
            Wh_Log(L"'%s' could not be registered (error %lu); the combination may "
                   L"be reserved or already claimed by another app",
                   g_keyNames[i], GetLastError());
        }
    }
}

// Applies the matching rule's action to a window the first time we see it shown.
static void TryApplyRuleToWindow(HWND hWnd) {
    if (!hWnd || !IsEligibleWindow(hWnd)) {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(g_appRulesMutex);
        if (g_appRules.empty()) {
            return;
        }
    }

    {
        std::lock_guard<std::mutex> lock(g_ruleHandledMutex);
        if (g_ruleHandledWindows.count(hWnd)) {
            return;
        }
    }

    bool noResize = false;
    Action action = FindRuleAction(GetWindowProcessExeName(hWnd), &noResize);
    if (action == Action::None) {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(g_ruleHandledMutex);
        if (!g_ruleHandledWindows.insert(hWnd).second) {
            return;  // Another event handled it first.
        }
    }

    EnqueueWork(hWnd, action, noResize);
}

void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD event, HWND hWnd, LONG idObject,
                           LONG idChild, DWORD, DWORD) {
    if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF || !hWnd) {
        return;
    }

    if (event == EVENT_OBJECT_DESTROY) {
        {
            std::lock_guard<std::mutex> lock(g_ruleHandledMutex);
            g_ruleHandledWindows.erase(hWnd);
        }
        EnqueueCleanup(hWnd);  // Drop any saved placement for this window.
        return;
    }

    // EVENT_OBJECT_SHOW.
    TryApplyRuleToWindow(hWnd);
}

// Installs the global show/destroy hook only while at least one auto-snap rule
// exists, and removes it otherwise. EVENT_OBJECT_SHOW fires for every window shown
// in every process, so the default (no-rules) configuration shouldn't pay for it.
// Must run on the hook thread (the thread that owns the WinEvent hook). The
// WINEVENT_SKIPOWNPROCESS flag keeps this tool process's own windows untouched.
static void UpdateWinEventHook() {
    bool wantHook;
    {
        std::lock_guard<std::mutex> lock(g_appRulesMutex);
        wantHook = !g_appRules.empty();
    }

    if (wantHook && !g_winEventHook) {
        g_winEventHook = SetWinEventHook(
            EVENT_OBJECT_DESTROY, EVENT_OBJECT_SHOW, nullptr, WinEventProc, 0, 0,
            WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
        if (!g_winEventHook) {
            Wh_Log(L"SetWinEventHook failed, error=%lu", GetLastError());
        }
    } else if (!wantHook && g_winEventHook) {
        UnhookWinEvent(g_winEventHook);
        g_winEventHook = nullptr;
    }
}

static BOOL CALLBACK EnumExistingWindowsProc(HWND hWnd, LPARAM) {
    if (IsWindowVisible(hWnd)) {
        TryApplyRuleToWindow(hWnd);
    }
    return TRUE;
}

// Snaps already-open windows that match a rule (on init and after a settings
// change), so adding a rule affects windows that are already on screen.
static void ApplyRulesToExistingWindows() {
    {
        std::lock_guard<std::mutex> lock(g_appRulesMutex);
        if (g_appRules.empty()) {
            return;
        }
    }
    EnumWindows(EnumExistingWindowsProc, 0);
}

static void LoadSettings() {
    // Wh_GetStringSetting never returns NULL (an unset value comes back as L""),
    // and WindhawkUtils::StringSetting frees the string for us on scope exit.
    UINT modifierMask =
        ParseModifierMask(WindhawkUtils::StringSetting::make(L"modifier").get());

    g_gapTop = Wh_GetIntSetting(L"dimensions.gapTop");
    g_gapBottom = Wh_GetIntSetting(L"dimensions.gapBottom");
    g_gapLeft = Wh_GetIntSetting(L"dimensions.gapLeft");
    g_gapRight = Wh_GetIntSetting(L"dimensions.gapRight");

    std::wstring gapModeValue =
        WindhawkUtils::StringSetting::make(L"dimensions.gapMode").get();
    if (gapModeValue == L"full") {
        g_gapMode = GapMode::Full;
    } else if (gapModeValue == L"screen") {
        g_gapMode = GapMode::Screen;
    } else {
        g_gapMode = GapMode::Even;
    }

    g_centerWidth =
        _wtoi(WindhawkUtils::StringSetting::make(L"dimensions.centerWidth").get());
    g_centerHeight =
        _wtoi(WindhawkUtils::StringSetting::make(L"dimensions.centerHeight").get());

    g_resizeStepPercent = Wh_GetIntSetting(L"dimensions.resizeStep");
    if (g_resizeStepPercent < 1) {
        g_resizeStepPercent = 1;
    }
    if (g_resizeStepPercent > 50) {
        g_resizeStepPercent = 50;
    }

    for (int i = 0; i < ACTION_COUNT; i++) {
        WindhawkUtils::StringSetting value =
            WindhawkUtils::StringSetting::make(g_keyNames[i]);

        UINT vk = 0, textMods = 0;
        if (ParseShortcut(value.get(), &vk, &textMods)) {
            g_actionKeys[i] = vk;
            // The global modifier is required on top of whatever the shortcut typed.
            g_actionMods[i] = modifierMask | textMods;
        } else {
            g_actionKeys[i] = 0;  // Unbound or invalid.
            g_actionMods[i] = 0;
        }
    }

    // De-duplicate bindings: if two actions resolve to the same key+modifier
    // combination, only the first one (in g_keyNames order) stays bound. Later
    // duplicates are disabled so a single keypress doesn't fire two actions.
    for (int i = 0; i < ACTION_COUNT; i++) {
        if (g_actionKeys[i] == 0) {
            continue;
        }
        for (int j = 0; j < i; j++) {
            if (g_actionKeys[j] == g_actionKeys[i] &&
                g_actionMods[j] == g_actionMods[i]) {
                Wh_Log(L"'%s' duplicates '%s' (same key+modifier); disabling '%s'",
                       g_keyNames[i], g_keyNames[j], g_keyNames[i]);
                g_actionKeys[i] = 0;
                g_actionMods[i] = 0;
                break;
            }
        }
    }

    // Auto-snap rules.
    std::vector<AppRule> newRules;
    for (int i = 0; i <= 255; i++) {
        wchar_t key[256] = {};

        swprintf_s(key, L"rules[%d].executable_names", i);
        std::wstring execNames = WindhawkUtils::StringSetting::make(key).get();

        swprintf_s(key, L"rules[%d].action", i);
        std::wstring actionValue = WindhawkUtils::StringSetting::make(key).get();

        swprintf_s(key, L"rules[%d].no_resize", i);
        bool noResize = Wh_GetIntSetting(key) != 0;

        if (execNames.empty() && actionValue.empty()) {
            // Tolerate a small gap of blank entries before giving up.
            if (i >= (int)newRules.size() + 2) {
                break;
            }
            continue;
        }

        AppRule rule;
        rule.executables = SplitExecutableList(execNames);
        rule.action = ParseActionName(actionValue);
        rule.noResize = noResize;
        if (!rule.executables.empty() && rule.action != Action::None) {
            newRules.push_back(std::move(rule));
        }
    }

    {
        std::lock_guard<std::mutex> lock(g_appRulesMutex);
        g_appRules = std::move(newRules);
    }
    {
        std::lock_guard<std::mutex> lock(g_ruleHandledMutex);
        g_ruleHandledWindows.clear();
    }
}

static DWORD WINAPI WorkerThreadProc(LPVOID) {
    for (;;) {
        WaitForSingleObject(g_workEvent, INFINITE);

        for (;;) {
            WorkItem item;
            {
                std::lock_guard<std::mutex> lock(g_workQueueMutex);
                if (g_workQueue.empty()) {
                    break;
                }
                item = g_workQueue.front();
                g_workQueue.pop_front();
            }
            if (item.cleanup) {
                g_savedPlacements.erase(item.hWnd);
            } else {
                SnapWindow(item.hWnd, item.action, item.noResize);
            }
        }

        if (g_quit.load()) {
            break;
        }
    }
    return 0;
}

static DWORD WINAPI HookThreadProc(LPVOID) {
    // Register the shortcut hotkeys on this thread; WM_HOTKEY (for a NULL window)
    // is posted to the queue of the thread that called RegisterHotKey. The auto-snap
    // WinEvent hook is installed here too, but only if any rules are configured.
    RegisterHotkeys();
    UpdateWinEventHook();

    SetEvent(g_hookReadyEvent);

    MSG msg;
    while (!g_quit.load()) {
        BOOL gm = GetMessageW(&msg, nullptr, 0, 0);
        if (gm == 0 || gm == -1) {
            break;
        }
        // WM_HOTKEY and our settings-changed signal are thread messages (no window),
        // so handle them here rather than via DispatchMessage.
        if (msg.hwnd == nullptr) {
            if (msg.message == WM_HOTKEY) {
                int idx = (int)msg.wParam;
                if (idx >= 0 && idx < ACTION_COUNT) {
                    EnqueueWork(GetForegroundWindow(), g_actionByIndex[idx]);
                }
                continue;
            }
            if (msg.message == WM_APP_SETTINGS_CHANGED) {
                RegisterHotkeys();
                UpdateWinEventHook();
                continue;
            }
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    UnregisterHotkeys();
    if (g_winEventHook) {
        UnhookWinEvent(g_winEventHook);
        g_winEventHook = nullptr;
    }
    return 0;
}

BOOL ToolModInit() {
    LoadSettings();

    g_quit = false;

    g_workEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_workEvent) {
        Wh_Log(L"CreateEvent (work) failed");
        return FALSE;
    }

    g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, nullptr);
    if (!g_workerThread) {
        Wh_Log(L"CreateThread (worker) failed");
        CloseHandle(g_workEvent);
        g_workEvent = nullptr;
        return FALSE;
    }

    g_hookReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_hookReadyEvent) {
        Wh_Log(L"CreateEvent (hook ready) failed");
        g_quit = true;
        SetEvent(g_workEvent);
        WaitForSingleObject(g_workerThread, 2000);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
        CloseHandle(g_workEvent);
        g_workEvent = nullptr;
        return FALSE;
    }

    g_hookThread = CreateThread(nullptr, 0, HookThreadProc, nullptr, 0,
                               &g_hookThreadId);
    if (!g_hookThread) {
        Wh_Log(L"CreateThread (hook) failed");
        CloseHandle(g_hookReadyEvent);
        g_hookReadyEvent = nullptr;
        g_quit = true;
        SetEvent(g_workEvent);
        WaitForSingleObject(g_workerThread, 2000);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
        CloseHandle(g_workEvent);
        g_workEvent = nullptr;
        return FALSE;
    }

    WaitForSingleObject(g_hookReadyEvent, 3000);
    CloseHandle(g_hookReadyEvent);
    g_hookReadyEvent = nullptr;

    ApplyRulesToExistingWindows();

    return TRUE;
}

void ToolModSettingsChanged() {
    LoadSettings();
    // Reflect the new bindings/rules on the hook thread (it owns the hotkeys and the
    // WinEvent hook).
    if (g_hookThreadId) {
        PostThreadMessageW(g_hookThreadId, WM_APP_SETTINGS_CHANGED, 0, 0);
    }
    ApplyRulesToExistingWindows();
}

void ToolModUninit() {
    g_quit = true;

    if (g_hookThreadId) {
        PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
    }
    if (g_hookThread) {
        WaitForSingleObject(g_hookThread, 2000);
        CloseHandle(g_hookThread);
        g_hookThread = nullptr;
        g_hookThreadId = 0;
    }

    if (g_workEvent) {
        SetEvent(g_workEvent);
    }
    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, 2000);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }
    if (g_workEvent) {
        CloseHandle(g_workEvent);
        g_workEvent = nullptr;
    }

    if (g_winEventHook) {
        UnhookWinEvent(g_winEventHook);
        g_winEventHook = nullptr;
    }

    g_savedPlacements.clear();
    {
        std::lock_guard<std::mutex> lock(g_workQueueMutex);
        g_workQueue.clear();
    }
    {
        std::lock_guard<std::mutex> lock(g_ruleHandledMutex);
        g_ruleHandledWindows.clear();
    }
}

////////////////////////////////////////////////////////////////////////////////
}  // namespace snapCommander

//////// move ////////
namespace moveToMonitor {




#define HOTKEY_UP    1
#define HOTKEY_DOWN  2
#define HOTKEY_LEFT  3
#define HOTKEY_RIGHT 4

// Custom message posted from ToolModSettingsChanged to the hotkey thread
// so that (Un)RegisterHotKey always runs on the thread that owns the window.
#define WM_APP_SETTINGS_CHANGED (WM_APP + 1)

struct ModSettings {
    UINT modifierKeys = MOD_CONTROL | MOD_ALT;
    int  targetUp     = -1;
    int  targetDown   = -1;
    int  targetLeft   = -1;
    int  targetRight  = -1;
    bool keepSize     = true;
    bool centerOnMove = false;
};

static ModSettings       g_settings;
static HANDLE            g_hThread = nullptr;
static HWND              g_hMsgWnd = nullptr;
static std::atomic<bool> g_running{false};

// ── settings ──────────────────────────────────────────────────────────────────

static UINT SettingToMod(const wchar_t* v) {
    if (wcscmp(v, L"ctrl_shift")     == 0) return MOD_CONTROL | MOD_SHIFT;
    if (wcscmp(v, L"alt_shift")      == 0) return MOD_ALT     | MOD_SHIFT;
    if (wcscmp(v, L"ctrl_alt_shift") == 0) return MOD_CONTROL | MOD_ALT | MOD_SHIFT;
    return MOD_CONTROL | MOD_ALT;
}

static void LoadSettings() {
    PCWSTR s = Wh_GetStringSetting(L"modifierKeys");
    g_settings.modifierKeys = SettingToMod(s);
    Wh_FreeStringSetting(s);
    g_settings.targetUp     = Wh_GetIntSetting(L"targetUp");
    g_settings.targetDown   = Wh_GetIntSetting(L"targetDown");
    g_settings.targetLeft   = Wh_GetIntSetting(L"targetLeft");
    g_settings.targetRight  = Wh_GetIntSetting(L"targetRight");
    g_settings.keepSize     = Wh_GetIntSetting(L"keepSize") != 0;
    g_settings.centerOnMove = Wh_GetIntSetting(L"centerOnMove") != 0;
}

// ── monitor enumeration ───────────────────────────────────────────────────────

struct MonitorInfo {
    HMONITOR hMon;
    RECT     rcWork;
    UINT     dpiX;
    UINT     dpiY;
};

static BOOL CALLBACK MonitorEnumProc(HMONITOR hMon, HDC, LPRECT, LPARAM lParam) {
    auto* list = reinterpret_cast<std::vector<MonitorInfo>*>(lParam);
    MONITORINFO mi = { sizeof(mi) };
    if (!GetMonitorInfo(hMon, &mi)) return TRUE;

    MonitorInfo info;
    info.hMon   = hMon;
    info.rcWork = mi.rcWork;
    info.dpiX   = 96;
    info.dpiY   = 96;

    typedef HRESULT (WINAPI* GetDpiForMonitorFn)(HMONITOR, int, UINT*, UINT*);
    static auto fn = reinterpret_cast<GetDpiForMonitorFn>(
        GetProcAddress(GetModuleHandle(L"shcore.dll"), "GetDpiForMonitor"));
    if (fn) fn(hMon, 0 /*MDT_EFFECTIVE_DPI*/, &info.dpiX, &info.dpiY);

    list->push_back(info);
    return TRUE;
}

static std::vector<MonitorInfo> GetAllMonitors() {
    std::vector<MonitorInfo> list;
    EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc,
                        reinterpret_cast<LPARAM>(&list));
    std::sort(list.begin(), list.end(), [](const MonitorInfo& a, const MonitorInfo& b) {
        if (a.rcWork.left != b.rcWork.left)
            return a.rcWork.left < b.rcWork.left;
        return a.rcWork.top < b.rcWork.top;
    });
    return list;
}

static POINT RectCenter(const RECT& r) {
    return { (r.left + r.right) / 2, (r.top + r.bottom) / 2 };
}

static int FindMonitorIndex(const std::vector<MonitorInfo>& monitors, HMONITOR hMon) {
    for (int i = 0; i < (int)monitors.size(); i++)
        if (monitors[i].hMon == hMon) return i;
    return -1;
}

static void LogAllMonitors(const std::vector<MonitorInfo>& monitors) {
    Wh_Log(L"=== Move Window to Monitor v6: %d monitor(s) detected ===",
           (int)monitors.size());
    Wh_Log(L"Sorted left-to-right, then top-to-bottom:");
    for (int i = 0; i < (int)monitors.size(); i++) {
        const auto& m = monitors[i];
        int w = m.rcWork.right  - m.rcWork.left;
        int h = m.rcWork.bottom - m.rcWork.top;
        Wh_Log(L"  Index %d | pos (%d, %d) | size %dx%d | DPI %u",
               i, m.rcWork.left, m.rcWork.top, w, h, m.dpiX);
    }
    Wh_Log(L"Settings: UP=%d DOWN=%d LEFT=%d RIGHT=%d  keepSize=%d  center=%d",
           g_settings.targetUp, g_settings.targetDown,
           g_settings.targetLeft, g_settings.targetRight,
           (int)g_settings.keepSize, (int)g_settings.centerOnMove);
}

// ── window moving ─────────────────────────────────────────────────────────────

enum class Direction { Up, Down, Left, Right };

static void MoveWindowToMonitor(HWND hwnd, const MonitorInfo& src, const MonitorInfo& dst) {
    bool wasMaximized = IsZoomed(hwnd);
    if (wasMaximized) ShowWindowAsync(hwnd, SW_RESTORE);

    RECT rcWin = {};
    GetWindowRect(hwnd, &rcWin);

    int ww   = rcWin.right  - rcWin.left;
    int wh   = rcWin.bottom - rcWin.top;
    int srcW = src.rcWork.right  - src.rcWork.left;
    int srcH = src.rcWork.bottom - src.rcWork.top;
    int dstW = dst.rcWork.right  - dst.rcWork.left;
    int dstH = dst.rcWork.bottom - dst.rcWork.top;

    float relX = srcW > 0 ? (float)(rcWin.left - src.rcWork.left) / srcW : 0.0f;
    float relY = srcH > 0 ? (float)(rcWin.top  - src.rcWork.top)  / srcH : 0.0f;

    int newW, newH;
    if (g_settings.keepSize) {
        float dpiScaleX = (dst.dpiX > 0 && src.dpiX > 0)
                          ? (float)dst.dpiX / src.dpiX : 1.0f;
        float dpiScaleY = (dst.dpiY > 0 && src.dpiY > 0)
                          ? (float)dst.dpiY / src.dpiY : 1.0f;
        newW = (int)(ww * dpiScaleX);
        newH = (int)(wh * dpiScaleY);
    } else {
        newW = (int)(ww * (float)dstW / srcW);
        newH = (int)(wh * (float)dstH / srcH);
    }

    int newX, newY;
    if (g_settings.centerOnMove) {
        newX = dst.rcWork.left + (dstW - newW) / 2;
        newY = dst.rcWork.top  + (dstH - newH) / 2;
    } else {
        newX = dst.rcWork.left + (int)(relX * dstW);
        newY = dst.rcWork.top  + (int)(relY * dstH);
    }

    if (newX + newW > dst.rcWork.right)  newX = dst.rcWork.right  - newW;
    if (newY + newH > dst.rcWork.bottom) newY = dst.rcWork.bottom - newH;
    if (newX < dst.rcWork.left)          newX = dst.rcWork.left;
    if (newY < dst.rcWork.top)           newY = dst.rcWork.top;

    SetWindowPos(hwnd, nullptr, newX, newY, newW, newH,
                 SWP_NOZORDER | SWP_NOACTIVATE);

    if (wasMaximized) ShowWindowAsync(hwnd, SW_MAXIMIZE);
}

static void MoveActiveWindowInDirection(Direction dir) {
    HWND hwnd = GetForegroundWindow();
    if (!hwnd) return;

    HMONITOR hCurrent = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO miCurrent = { sizeof(miCurrent) };
    if (!GetMonitorInfo(hCurrent, &miCurrent)) return;

    auto monitors = GetAllMonitors();
    int  curIdx   = FindMonitorIndex(monitors, hCurrent);
    if (curIdx < 0) return;

    int fixedIndex = -1;
    switch (dir) {
        case Direction::Up:    fixedIndex = g_settings.targetUp;    break;
        case Direction::Down:  fixedIndex = g_settings.targetDown;  break;
        case Direction::Left:  fixedIndex = g_settings.targetLeft;  break;
        case Direction::Right: fixedIndex = g_settings.targetRight; break;
    }

    if (fixedIndex >= (int)monitors.size()) fixedIndex = -1; // Missing saved display: use spatial movement.
    int dstIdx = -1;

    if (fixedIndex >= 0 && fixedIndex < (int)monitors.size()) {
        if (fixedIndex != curIdx)
            dstIdx = fixedIndex;
    }

    if (dstIdx < 0 && fixedIndex < 0) {
        POINT curCenter = RectCenter(monitors[curIdx].rcWork);
        int   bestScore = INT_MAX;

        for (int i = 0; i < (int)monitors.size(); i++) {
            if (i == curIdx) continue;
            POINT c  = RectCenter(monitors[i].rcWork);
            int   dx = c.x - curCenter.x;
            int   dy = c.y - curCenter.y;
            bool  candidate = false;
            int   primary   = 0;

            switch (dir) {
                case Direction::Up:
                    candidate = (dy < 0) && (abs(dy) >= abs(dx)); primary = -dy; break;
                case Direction::Down:
                    candidate = (dy > 0) && (abs(dy) >= abs(dx)); primary = dy;  break;
                case Direction::Left:
                    candidate = (dx < 0) && (abs(dx) >= abs(dy)); primary = -dx; break;
                case Direction::Right:
                    candidate = (dx > 0) && (abs(dx) >= abs(dy)); primary = dx;  break;
            }

            if (candidate && primary < bestScore) {
                bestScore = primary;
                dstIdx    = i;
            }
        }
    }

    if (dstIdx < 0) return;
    MoveWindowToMonitor(hwnd, monitors[curIdx], monitors[dstIdx]);
}

// ── hotkey thread ─────────────────────────────────────────────────────────────

static void RegisterHotkeys(HWND hwnd) {
    UINT mod = g_settings.modifierKeys | MOD_NOREPEAT;
    RegisterHotKey(hwnd, HOTKEY_UP,    mod, VK_UP);
    RegisterHotKey(hwnd, HOTKEY_DOWN,  mod, VK_DOWN);
    RegisterHotKey(hwnd, HOTKEY_LEFT,  mod, VK_LEFT);
    RegisterHotKey(hwnd, HOTKEY_RIGHT, mod, VK_RIGHT);
}

static void UnregisterHotkeys(HWND hwnd) {
    UnregisterHotKey(hwnd, HOTKEY_UP);
    UnregisterHotKey(hwnd, HOTKEY_DOWN);
    UnregisterHotKey(hwnd, HOTKEY_LEFT);
    UnregisterHotKey(hwnd, HOTKEY_RIGHT);
}

static LRESULT CALLBACK HotkeyWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_HOTKEY) {
        switch ((int)wParam) {
            case HOTKEY_UP:    MoveActiveWindowInDirection(Direction::Up);    break;
            case HOTKEY_DOWN:  MoveActiveWindowInDirection(Direction::Down);  break;
            case HOTKEY_LEFT:  MoveActiveWindowInDirection(Direction::Left);  break;
            case HOTKEY_RIGHT: MoveActiveWindowInDirection(Direction::Right); break;
        }
        return 0;
    }

    if (msg == WM_APP_SETTINGS_CHANGED) {
        // (Un)RegisterHotKey must run on the thread that owns the window
        UnregisterHotkeys(hwnd);
        LoadSettings();
        auto monitors = GetAllMonitors();
        LogAllMonitors(monitors);
        RegisterHotkeys(hwnd);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

static void HotkeyThreadProc() {
    const wchar_t CLASS_NAME[] = L"WH_MoveToMonitor_MsgWnd";
    WNDCLASSEX wc = {};
    wc.cbSize        = sizeof(wc);
    wc.lpfnWndProc   = HotkeyWndProc;
    wc.hInstance     = GetModuleHandle(nullptr);
    wc.lpszClassName = CLASS_NAME;
    RegisterClassEx(&wc);

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, nullptr, 0, 0, 0, 0, 0,
                               HWND_MESSAGE, nullptr, GetModuleHandle(nullptr), nullptr);
    if (!hwnd) { Wh_Log(L"Failed to create message window"); return; }

    g_hMsgWnd = hwnd;
    RegisterHotkeys(hwnd);

    MSG msg;
    while (g_running && GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnregisterHotkeys(hwnd);
    DestroyWindow(hwnd);
    g_hMsgWnd = nullptr;
    UnregisterClass(CLASS_NAME, GetModuleHandle(nullptr));
}

static DWORD WINAPI HotkeyThreadEntry(LPVOID) {
    HotkeyThreadProc();
    return 0;
}

// ── Tool mod callbacks (renamed as required by the tool mod pattern) ──────────

BOOL ToolModInit() {
    Wh_Log(L"MoveWindowToMonitor v6: Init");
    LoadSettings();
    auto monitors = GetAllMonitors();
    LogAllMonitors(monitors);
    g_running = true;
    g_hThread = CreateThread(nullptr, 0, HotkeyThreadEntry, nullptr, 0, nullptr);
    return g_hThread != nullptr;
}

void ToolModSettingsChanged() {
    Wh_Log(L"MoveWindowToMonitor: Settings changed, notifying hotkey thread");
    if (g_hMsgWnd) PostMessage(g_hMsgWnd, WM_APP_SETTINGS_CHANGED, 0, 0);
}

void ToolModUninit() {
    g_running = false;
    if (g_hMsgWnd) PostMessage(g_hMsgWnd, WM_QUIT, 0, 0);
    if (g_hThread) {
        WaitForSingleObject(g_hThread, 3000);
        CloseHandle(g_hThread);
        g_hThread = nullptr;
    }
}

////////////////////////////////////////////////////////////////////////////////
}  // namespace moveToMonitor

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
- dragWindowsWithoutTitleBar: false
  $name: Drag windows without a title bar
  $description: >-
    Also drag windows without a title bar, such as popup menus, tooltips
    and flyouts.
- middleAction: menu
  $name: Alt + middle click
  $description: Action when middle clicking a window while Alt is held. Values are menu, maximize, minimize, close, topmost, lower, center, sideSnap, or none.
- rightClickAction: maximize
  $name: Alt + right click without dragging
  $description: A short click toggles maximize. Dragging with the right button still resizes.
- x1Action: none
  $name: Alt + X1 click
- x2Action: none
  $name: Alt + X2 click
- leftDoubleAction: none
  $name: Alt + left double click
- middleDoubleAction: none
  $name: Alt + middle double click
- rightDoubleAction: none
  $name: Alt + right double click
- hotkeyMaximize: Alt+F
  $name: Toggle maximize shortcut
- hotkeyMinimize: Alt+M
  $name: Minimize shortcut
- hotkeyClose: Alt+Shift+Q
  $name: Close shortcut
- hotkeyMenu: ''
  $name: Window menu shortcut
- hotkeyTopmost: ''
  $name: Toggle always on top shortcut
- hotkeyLower: ''
  $name: Lower window shortcut
- hotkeyCenter: ''
  $name: Center window shortcut
- hotkeySideSnap: ''
  $name: Snap to nearest screen side shortcut
- modifier: alt
  $name: Modifier key
  $description: >-
    Added to every action's shortcut, on top of any modifiers typed into the action
    itself. Choose None to use only the modifiers written in each shortcut.
  $options:
    - none: None
    - alt: Alt
    - ctrl: Ctrl
    - shift: Shift
    - win: Win
    - alt_ctrl: Alt + Ctrl
    - alt_shift: Alt + Shift
    - ctrl_shift: Ctrl + Shift
    - win_alt: Win + Alt
    - win_ctrl: Win + Ctrl
    - win_shift: Win + Shift
- shortcuts:
    - keyLeftHalf: q
      $name: Left half
      $description: >-
        Shortcut text, e.g. "q", "ctrl+q", or "ctrl+shift+left". The Modifier key above is
        added on top. Leave empty to unbind. Same format for every action below.
    - keyRightHalf: w
      $name: Right half
    - keyTopHalf: e
      $name: Top half
    - keyBottomHalf: r
      $name: Bottom half
    - keyCenterHalf: t
      $name: Center half
      $description: A half-width strip in the middle of the screen (full height).
    - keyTopLeft: u
      $name: Top-left
    - keyTopRight: i
      $name: Top-right
    - keyBottomLeft: j
      $name: Bottom-left
    - keyBottomRight: k
      $name: Bottom-right
    - keyMaximize: h
      $name: Maximize
      $description: Fills the whole work area, ignoring any configured gaps.
    - keyAlmostMaximize: y
      $name: Almost-maximize
      $description: Fills the work area but keeps the configured gaps.
    - keyCenter: g
      $name: Center
      $description: Centers the window without changing its size (unless a size is set below).
    - keyMinimize: m
      $name: Minimize
      $description: Minimizes the active window.
    - keyNextDisplay: "]"
      $name: Next display
      $description: Moves the window to the next monitor.
    - keyPrevDisplay: "["
      $name: Previous display
      $description: Moves the window to the previous monitor.
    - keySmaller: "-"
      $name: Make smaller
      $description: Shrinks the window around its center.
    - keyLarger: plus
      $name: Make larger
      $description: >-
        Grows the window around its center. Because "+" is the shortcut separator,
        write this key as "plus" (or "="); it triggers on the unshifted "=" / "+" key.
    - keyRestore: z
      $name: Restore
      $description: Returns the window to where it was before the first snap.
  $name: Shortcuts
  $description: Keyboard shortcuts for each window action.
- dimensions:
    - gapTop: 0
      $name: Top gap (pixels)
      $description: Space left above a snapped window.
    - gapBottom: 0
      $name: Bottom gap (pixels)
      $description: Space left below a snapped window.
    - gapLeft: 0
      $name: Left gap (pixels)
      $description: Space left to the left of a snapped window.
    - gapRight: 0
      $name: Right gap (pixels)
      $description: Space left to the right of a snapped window.
    - gapMode: even
      $name: Gap distribution
      $description: >-
        How gaps are applied between adjacent windows. "Even spacing" halves the gap on
        shared edges so two windows side by side show the same gap as a single window
        against the screen edge (no doubling). "Full gap on every side" applies the whole
        gap to each window side, so adjacent windows show a doubled gap. "Screen edges
        only" gaps just the sides that touch a screen edge, so adjacent windows meet flush.
      $options:
        - even: Even spacing (no doubling)
        - full: Full gap on every side
        - screen: Screen edges only
    - centerWidth: ""
      $name: Center action width (pixels)
      $description: Width to use for the Center action. Leave empty to keep the window's current width.
    - centerHeight: ""
      $name: Center action height (pixels)
      $description: Height to use for the Center action. Leave empty to keep the window's current height.
    - resizeStep: 5
      $name: Resize step (%)
      $description: >-
        How much Make smaller / larger change the window each press, as a percentage
        of the monitor work area. Clamped to 1-50.
  $name: Dimensions
  $description: Gaps around snapped windows and sizes for the Center action.
- rules:
    - - executable_names: ""
        $name: Executable name(s)
        $description: >-
          One or more executable names. Separators: comma, semicolon, pipe.
          Case-insensitive; the ".exe" suffix is optional. Example:
          "notepad; calc.exe".
      - action: maximize
        $name: Action
        $options:
          - left-half: Left half
          - right-half: Right half
          - top-half: Top half
          - bottom-half: Bottom half
          - center-half: Center half
          - top-left: Top-left
          - top-right: Top-right
          - bottom-left: Bottom-left
          - bottom-right: Bottom-right
          - maximize: Maximize
          - almost-maximize: Almost maximize
          - center: Center
          - minimize: Minimize
      - no_resize: false
        $name: Don't resize
        $description: >-
          Move the window into the chosen region but keep its current size
          instead of resizing it to fill the region. Ignored by Minimize.
  $name: Auto-snap rules
  $description: >-
    Automatically apply an action to an app's windows when they open. Add one
    item per executable/action combination.
- modifierKeys: "ctrl_alt"
  $name: Modifier Keys
  $options:
    - ctrl_alt: Ctrl+Alt
    - ctrl_shift: Ctrl+Shift
    - alt_shift: Alt+Shift
    - ctrl_alt_shift: Ctrl+Alt+Shift
- targetUp: -1
  $name: "Target monitor UP (-1 = automatic)"
  $description: Index of the monitor to move to when pressing the UP hotkey
- targetDown: -1
  $name: "Target monitor DOWN (-1 = automatic)"
  $description: Index of the monitor to move to when pressing the DOWN hotkey
- targetLeft: -1
  $name: "Target monitor LEFT (-1 = automatic)"
  $description: Index of the monitor to move to when pressing the LEFT hotkey
- targetRight: -1
  $name: "Target monitor RIGHT (-1 = automatic)"
  $description: Index of the monitor to move to when pressing the RIGHT hotkey
- keepSize: true
  $name: Keep window size when moving
  $description: If disabled, the window is scaled proportionally to the target monitor
- centerOnMove: false
  $name: Center window on target monitor
  $description: If enabled, the window is centered on the target monitor instead of keeping its relative position
*/
// ==/WindhawkModSettings==


////////////////////////////////////////////////////////////////////////////////
// Shared dispatcher: one set of engine callbacks for the three modules.
//
// Host selection:
//   windhawk.exe -tool-mod <id>  -> tool process: both hotkey tools run here.
//   windhawk.exe                 -> launcher: spawns the tool process above. The
//                              ShellExecuteEx requests an elevated child so
//                              keyboard controls can move administrator windows.
//   anything else           -> drag module only (it must run inside every
//                              process that owns a draggable window).
//
// The launcher gate on the executable name keeps a single spawner: without
// it, Include=* would spawn a tool from every injected process.

static bool g_packIsTool;
static bool g_packIsLauncher;
static bool g_packDragInited;

static bool PackIsToolProcess(int argc, LPWSTR* argv) {
    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            return wcscmp(argv[i + 1], WH_MOD_ID) == 0;
        }
    }
    return false;
}

static bool PackIsExcludedProcess(int argc, LPWSTR* argv) {
    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            return true;
        }
    }
    return false;
}

static bool PackIsWindhawkProcess() {
    WCHAR path[MAX_PATH];
    DWORD len = GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    if (!len || len >= ARRAYSIZE(path)) {
        return false;
    }
    PCWSTR name = path;
    for (PCWSTR p = path; *p; p++) {
        if (*p == L'\\' || *p == L'/') name = p + 1;
    }
    return _wcsicmp(name, L"windhawk.exe") == 0;
}

static void WINAPI PackEntryPointHook() {
    Wh_Log(L">");
    ExitThread(0);
}

// Spawns "<this process>" -tool-mod "<id>", exactly like the stock tool-mod
// pattern, with ShellExecuteEx and the runas verb requesting elevation.
static void PackSpawnTool() {
    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR commandLine[MAX_PATH + 2 +
                      (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    SHELLEXECUTEINFOW sei{sizeof(sei)};
    sei.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
    sei.lpVerb = L"runas";
    sei.lpFile = currentProcessPath;
    sei.lpParameters = L"-tool-mod \"" WH_MOD_ID L"\"";
    sei.nShow = SW_HIDE;
    if (!ShellExecuteExW(&sei)) {
        Wh_Log(L"Elevated tool launch failed: %u", GetLastError());
        return;
    }
    if (sei.hProcess) CloseHandle(sei.hProcess);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    bool excluded = PackIsExcludedProcess(argc, argv);
    bool ourTool = PackIsToolProcess(argc, argv);
    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0 && !ourTool) excluded = true;
    }
    LocalFree(argv);

    if (excluded) {
        return FALSE;
    }

    if (ourTool) {
        g_packIsTool = true;

        HANDLE mutex = CreateMutex(nullptr, TRUE,
                                   L"windhawk-tool-mod_" WH_MOD_ID);
        if (!mutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }
        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!snapCommander::ToolModInit()) {
            Wh_Log(L"snapCommander tool init failed");
            ExitProcess(1);
        }
        if (!moveToMonitor::ToolModInit()) {
            Wh_Log(L"moveToMonitor tool init failed");
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);
        void* entryPoint =
            (BYTE*)dosHeader + ntHeaders->OptionalHeader.AddressOfEntryPoint;
        Wh_SetFunctionHook(entryPoint, (void*)PackEntryPointHook, nullptr);
        return TRUE;
    }

    if (PackIsWindhawkProcess()) {
        // Windhawk launcher: starts the elevated keyboard helper and
        // loads drag support for its own native dialogs.
        g_packIsLauncher = true;
        g_packDragInited = altSnapDrag::ModInit();
        return TRUE;
    }

    g_packDragInited = altSnapDrag::ModInit();
    return g_packDragInited ? TRUE : FALSE;
}

void Wh_ModAfterInit() {
    if (g_packIsLauncher) {
        PackSpawnTool();
    }
}

void Wh_ModSettingsChanged() {
    if (g_packIsTool) {
        snapCommander::ToolModSettingsChanged();
        moveToMonitor::ToolModSettingsChanged();
        return;
    }
    if (g_packDragInited) {
        altSnapDrag::ModSettingsChanged();
    }
}

void Wh_ModUninit() {
    if (g_packIsTool) {
        snapCommander::ToolModUninit();
        moveToMonitor::ToolModUninit();
        ExitProcess(0);
    }
    if (g_packDragInited) {
        altSnapDrag::ModUninit();
    }
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
    return altSnapDrag::DragDllMain(hinstDLL, fdwReason, lpReserved);
}
