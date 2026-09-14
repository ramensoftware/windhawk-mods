// ==WindhawkMod==
// @id              alt-drag
// @name            AltDrag
// @description     Move any window by holding Alt and dragging it from anywhere, without having to grab the title bar
// @version         1.0
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
grab the title bar.

The idea was inspired by [the original AltDrag
tool](https://stefansundin.github.io/altdrag/).

![Demonstration](https://i.imgur.com/PY0arDE.gif)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- dragWindowsWithoutTitleBar: false
  $name: Drag windows without a title bar
  $description: >-
    Also drag windows without a title bar, such as popup menus, tooltips
    and flyouts.
*/
// ==/WindhawkModSettings==

// The drag is started by taking the button press away from the target window as
// it's retrieved from the message queue, and posting a move request to the root
// window, which the mod instance in the root window's thread turns into the
// system command a title bar drag generates. Intercepting at retrieval time
// means the window procedure never sees the press, so it doesn't matter how the
// program handles mouse input, and the system move loop provides snapping, DWM
// animations and maximized window handling the same way a title bar drag does.
// The root window may belong to another process, e.g. ApplicationFrameHost.exe
// hosting a UWP app's CoreWindow, so the request is a registered message which
// is allowed through the UIPI message filter.
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
// WH_CALLWNDPROC hook subclasses the window under the pointer while Alt is
// held, and the subclass routes an Alt-initiated contact to DefWindowProc,
// which promotes it to the legacy mouse messages the retrieval hook handles.
//
// Content hosted in a composition input sink, such as a WinUI XAML island,
// receives its pointer input over a side channel and produces no window message
// at all, so neither hook nor subclass sees the press, and it reaches the
// island, which turns the release into a click. Only input which hasn't been
// routed yet can be taken away from it, so while Alt is held a low level mouse
// hook swallows the press over composition hosted content, recognized by the
// class of the hosting window, and moves the window itself with SetWindowPos.
// The cursor shown over such content is chosen by the content, on every move it
// sees, from a thread of its own, so nothing set from outside sticks. For the
// duration of the drag a small invisible topmost window of the hook's thread
// follows the cursor instead: it receives the moves, and with them the right to
// choose the cursor. Windows which deliver the press as a message are left to
// the paths above and keep the system move loop, which is of no use here: it
// retrieves no mouse input while the sink owns the contact, so it starts and
// then tracks nothing. The hook exists only while Alt is held, keeping it out
// of the input path the rest of the time.
//
// The hook is global, and the island of a window which isn't focused belongs to
// a process which never saw Alt go down and so has no hook of its own. The
// process which does hold the hook therefore handles any window, not just its
// own, and moves it asynchronously since it doesn't own it.
//
// A swallowed press never reaches the input queue, so as far as the system is
// concerned Alt was tapped on its own, and DefWindowProc turns the release into
// SC_KEYMENU, activating the menu bar. The Alt release which ends such a drag
// is therefore taken as well. A swallowed press which isn't followed by a drag
// is replayed as a click on the release, so that an Alt+click keeps whatever
// meaning the program gives it.

#include <commctrl.h>

#include <atomic>
#include <cstdlib>
#include <memory>
#include <mutex>
#include <unordered_set>

struct {
    std::atomic<bool> dragWindowsWithoutTitleBar;
} g_settings;

std::atomic<bool> g_uninitializing;
std::atomic<int> g_hookRefCount;

UINT g_moveRequestMessage =
    RegisterWindowMessage(L"Windhawk_MoveRequest_" WH_MOD_ID);
UINT g_unsubclassRegisteredMessage =
    RegisterWindowMessage(L"Windhawk_Unsubclass_" WH_MOD_ID);

thread_local bool g_threadHooksAttempted;
thread_local HHOOK g_getMessageHook;
thread_local HHOOK g_callWndProcHook;
std::mutex g_allThreadHooksMutex;
std::unordered_set<HHOOK> g_allThreadHooks;

thread_local HWND g_lastSubclassedWnd;
std::mutex g_subclassedWindowsMutex;
std::unordered_set<HWND> g_subclassedWindows;

// The pointer contact being routed to DefWindowProc, if any.
thread_local HWND g_contactWnd;
thread_local UINT g_contactPointerId;

// The low level mouse hook, installed only while Alt is held, by whichever
// thread retrieves the press. The mutex guards the handle and the thread, and
// keeps an installation from slipping past the removal at uninit. The drag
// state below is touched only on the hook's thread, which its callback runs
// on, and reset with the hook gone: at uninit once the callbacks are waited
// out, and at the start of an Alt hold.
std::mutex g_lowLevelMouseHookMutex;
HHOOK g_lowLevelMouseHook;
DWORD g_lowLevelMouseHookThreadId;
// Set when a press was swallowed during the current Alt hold.
std::atomic<bool> g_swallowedPress;

// Marks the input of a replayed click, which the hook lets through.
constexpr ULONG_PTR kReplayedClickExtraInfo = 0x57484152;

bool g_llCandidate;
HWND g_llCandidateRoot;
POINT g_llDownPt;
HWND g_llDragRoot;
POINT g_llDragGrab;
HWND g_llDragOverlayWnd;

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

void ResetLowLevelMouseHook();
void InstallLowLevelMouseHookIfNeeded();
void RemoveLowLevelMouseHookIfIdle();

auto HookRefCountScope() {
    g_hookRefCount++;
    return std::unique_ptr<decltype(g_hookRefCount),
                           void (*)(decltype(g_hookRefCount)*)>{
        &g_hookRefCount, [](auto hookRefCount) { (*hookRefCount)--; }};
}

bool IsExcludedRootWindow(HWND hRootWnd) {
    WCHAR className[64];
    if (!GetClassName(hRootWnd, className, ARRAYSIZE(className))) {
        return true;
    }

    // The taskbar and the desktop.
    if (_wcsicmp(className, L"Shell_TrayWnd") == 0 ||
        _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0 ||
        _wcsicmp(className, L"Progman") == 0 ||
        _wcsicmp(className, L"WorkerW") == 0) {
        return true;
    }

    if (!g_settings.dragWindowsWithoutTitleBar) {
        LONG style = GetWindowLong(hRootWnd, GWL_STYLE);
        if ((style & WS_CAPTION) != WS_CAPTION) {
            return true;
        }
    }

    return false;
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

bool IsPointerMessage(UINT message) {
    return message >= WM_NCPOINTERUPDATE && message <= WM_POINTERROUTEDRELEASED;
}

bool IsPrimaryButtonDownMessage(UINT message) {
    switch (message) {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
        case WM_NCLBUTTONDOWN:
        case WM_NCLBUTTONDBLCLK:
            return true;
    }

    return false;
}

bool IsPrimaryPointerDownMessage(UINT message, WPARAM wParam) {
    switch (message) {
        case WM_POINTERDOWN:
        case WM_NCPOINTERDOWN:
            return IS_POINTER_FIRSTBUTTON_WPARAM(wParam);
    }

    return false;
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
bool IsPointerContactGoneMessage(UINT message, WPARAM wParam) {
    switch (message) {
        case WM_POINTERLEAVE:
            return true;

        case WM_POINTERUPDATE:
        case WM_NCPOINTERUPDATE:
            return !IS_POINTER_FIRSTBUTTON_WPARAM(wParam);
    }

    return false;
}

// Whether the system move loop is running on this thread.
bool IsInMoveLoop() {
    GUITHREADINFO gti{.cbSize = sizeof(gti)};
    return GetGUIThreadInfo(GetCurrentThreadId(), &gti) &&
           (gti.flags & GUI_INMOVESIZE);
}

// Turns a move request retrieved by the root window's thread into the system
// command. The move loop only starts if this thread's synchronized button state
// is down, which it isn't when the press was retrieved by another thread.
void OnMoveRequestRemoved(MSG* msg) {
    if (GetAsyncKeyState(VK_LBUTTON) >= 0) {
        // Released already, a move loop would stick to the cursor.
        Wh_Log(L"Move request for %08X, button already released",
               (DWORD)(ULONG_PTR)msg->hwnd);
        msg->message = WM_NULL;
        msg->wParam = 0;
        msg->lParam = 0;
        return;
    }

    if (GetKeyState(VK_LBUTTON) >= 0) {
        BYTE keyState[256];
        if (GetKeyboardState(keyState)) {
            keyState[VK_LBUTTON] |= 0x80;
            SetKeyboardState(keyState);
        }
    }

    Wh_Log(L"Move request for %08X, starting the move loop",
           (DWORD)(ULONG_PTR)msg->hwnd);

    msg->message = WM_SYSCOMMAND;
    msg->wParam = SC_MOVE | HTCAPTION;
}

// Runs for every message removed from the queue of the current thread, before
// the program sees it.
void OnMessageRemoved(MSG* msg) {
    if (!msg->hwnd) {
        return;
    }

    if (msg->message == g_moveRequestMessage) {
        OnMoveRequestRemoved(msg);
        return;
    }

    // Track Alt so the low level mouse hook exists only while it's held.
    if (msg->message == WM_SYSKEYDOWN || msg->message == WM_KEYDOWN) {
        if (msg->wParam == VK_MENU) {
            // Bit 30 of lParam is set for the auto repeats which arrive while
            // the key is held, including throughout a drag.
            constexpr LPARAM kPreviousKeyStateDown = 1 << 30;
            if (!(msg->lParam & kPreviousKeyStateDown)) {
                g_swallowedPress = false;
                ResetLowLevelMouseHook();
            }

            InstallLowLevelMouseHookIfNeeded();
        }
        return;
    }

    if (msg->message == WM_SYSKEYUP || msg->message == WM_KEYUP) {
        if (msg->wParam == VK_MENU) {
            RemoveLowLevelMouseHookIfIdle();

            if (g_swallowedPress.exchange(false)) {
                Wh_Log(L"Swallowing the Alt release of a drag");
                msg->message = WM_NULL;
                msg->wParam = 0;
                msg->lParam = 0;
            }
        }
        return;
    }

    if (msg->message != WM_MOUSEMOVE &&
        !IsPrimaryButtonDownMessage(msg->message)) {
        return;
    }

    // GetKeyState reflects the state at the time of the retrieved message.
    if (GetKeyState(VK_MENU) >= 0) {
        return;
    }

    if (msg->message == WM_MOUSEMOVE) {
        // The move loop retrieves the moves itself, without dispatching them,
        // holds an internal capture for which no WM_SETCURSOR is sent, and
        // shows the class cursor.
        if (IsInMoveLoop()) {
            SetCursor(LoadCursor(nullptr, IDC_SIZEALL));
        }
        return;
    }

    HWND hRootWnd = GetAncestor(msg->hwnd, GA_ROOT);
    if (!hRootWnd || IsExcludedRootWindow(hRootWnd)) {
        return;
    }

    Wh_Log(L"Message %04X for %08X, requesting a move of root window %08X",
           msg->message, (DWORD)(ULONG_PTR)msg->hwnd,
           (DWORD)(ULONG_PTR)hRootWnd);

    // Posted messages are retrieved before input, so a button release that's
    // already queued is seen by the move loop rather than by the program.
    if (!PostMessage(hRootWnd, g_moveRequestMessage, 0,
                     MAKELPARAM(msg->pt.x, msg->pt.y))) {
        Wh_Log(L"PostMessage error: %u", GetLastError());
        return;
    }

    msg->message = WM_NULL;
    msg->wParam = 0;
    msg->lParam = 0;
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
    } else if (GetAsyncKeyState(VK_MENU) >= 0 && hWnd != g_contactWnd) {
        // Needed only while Alt is held or a contact is being routed. The hit
        // test preceding the next Alt press puts it back.
        UnsubclassWindow(hWnd);
    } else if (IsPointerMessage(uMsg)) {
        UINT pointerId = GET_POINTERID_WPARAM(wParam);
        bool tracked = hWnd == g_contactWnd && pointerId == g_contactPointerId;

        if (IsPrimaryPointerDownMessage(uMsg, wParam)) {
            // A new contact, the mouse reuses its pointer id for every one.
            if (tracked) {
                g_contactWnd = nullptr;
            }

            if (GetAsyncKeyState(VK_MENU) < 0 && !g_uninitializing) {
                Wh_Log(
                    L"Message %04X for %08X, routing pointer %u to "
                    L"DefWindowProc",
                    uMsg, (DWORD)(ULONG_PTR)hWnd, pointerId);
                g_contactWnd = hWnd;
                g_contactPointerId = pointerId;
                return DefWindowProc(hWnd, uMsg, wParam, lParam);
            }
        } else if (tracked && IsPointerContactGoneMessage(uMsg, wParam)) {
            // The program saw the pointer enter, so it gets to see this.
            g_contactWnd = nullptr;
        } else if (tracked) {
            if (IsPointerContactEndMessage(uMsg)) {
                g_contactWnd = nullptr;
            }

            return DefWindowProc(hWnd, uMsg, wParam, lParam);
        }
    }

    LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);

    switch (uMsg) {
        case WM_MOUSEMOVE:
        case WM_NCMOUSEMOVE:
        case WM_POINTERUPDATE:
        case WM_NCPOINTERUPDATE:
            // Overrides the cursor the program chose for the move.
            if (GetAsyncKeyState(VK_MENU) < 0 && !g_uninitializing) {
                SetCursor(LoadCursor(nullptr, IDC_SIZEALL));
            }
            break;
    }

    return result;
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
    if (!hRootWnd || IsExcludedRootWindow(hRootWnd)) {
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
        // The thread's synchronized key state is stale if the window isn't
        // active yet, e.g. an Alt+click on a background window.
        if (cwp->message == WM_NCHITTEST && GetAsyncKeyState(VK_MENU) < 0) {
            SubclassWindowIfNeeded(cwp->hwnd);
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

void MoveDraggedWindow(HWND hRootWnd, POINT grab, POINT pt) {
    // SWP_ASYNCWINDOWPOS posts the request when the window belongs to another
    // thread, which keeps a busy owner from blocking the caller.
    SetWindowPos(
        hRootWnd, nullptr, pt.x - grab.x, pt.y - grab.y, 0, 0,
        SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
}

// The class supplies the size cursor, and with DefWindowProc as the window
// procedure no mod code is on the window's call path.
HWND CreateDragOverlay(POINT pt) {
    WNDCLASS wc{
        .lpfnWndProc = DefWindowProc,
        .hInstance = GetModuleHandle(nullptr),
        .hCursor = LoadCursor(nullptr, IDC_SIZEALL),
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

// Since this press does enter the input queue, the Alt release needs no
// swallowing on its account.
void ReplaySwallowedClick() {
    INPUT inputs[2]{};
    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    inputs[0].mi.dwExtraInfo = kReplayedClickExtraInfo;
    inputs[1].type = INPUT_MOUSE;
    inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
    inputs[1].mi.dwExtraInfo = kReplayedClickExtraInfo;
    if (SendInput(ARRAYSIZE(inputs), inputs, sizeof(inputs[0])) ==
        ARRAYSIZE(inputs)) {
        g_swallowedPress = false;
    } else {
        Wh_Log(L"SendInput error: %u", GetLastError());
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

    if (ms->dwExtraInfo == kReplayedClickExtraInfo) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    // Focus can move away while Alt is held, e.g. Alt+Tab, and the Alt release
    // then goes to another process. Drop the hook here instead of keeping it
    // for the rest of the process's life.
    RemoveLowLevelMouseHookIfIdle();

    switch (wParam) {
        case WM_LBUTTONDOWN: {
            g_llCandidate = false;
            g_llDragRoot = nullptr;
            DestroyDragOverlay();

            if (GetAsyncKeyState(VK_MENU) >= 0) {
                break;
            }

            // Looked up afresh in case the display layout changed.
            g_llMonitor = {};
            POINT pt = ToLogicalPoint(ms->pt);

            HWND hWnd = CompositionHostedWindowFromPoint(pt);
            if (!hWnd) {
                // Delivered as a message, so the paths above handle it and keep
                // the system move loop.
                break;
            }

            HWND hRootWnd = GetAncestor(hWnd, GA_ROOT);
            if (!hRootWnd || IsExcludedRootWindow(hRootWnd)) {
                break;
            }

            Wh_Log(L"Swallowed the press over %08X, root window %08X",
                   (DWORD)(ULONG_PTR)hWnd, (DWORD)(ULONG_PTR)hRootWnd);

            g_llCandidate = true;
            g_llCandidateRoot = hRootWnd;
            g_llDownPt = pt;
            g_swallowedPress = true;
            return 1;
        }

        case WM_LBUTTONUP:
            if (g_llCandidate || g_llDragRoot) {
                if (g_llCandidate) {
                    ReplaySwallowedClick();
                }

                g_llCandidate = false;
                g_llDragRoot = nullptr;
                DestroyDragOverlay();
                return 1;
            }
            break;

        case WM_MOUSEMOVE: {
            if (!g_llCandidate && !g_llDragRoot) {
                break;
            }

            POINT pt = ToLogicalPoint(ms->pt);

            if (g_llDragRoot) {
                PlaceDragOverlay(pt);
                MoveDraggedWindow(g_llDragRoot, g_llDragGrab, pt);
            } else if (abs(pt.x - g_llDownPt.x) >=
                           GetSystemMetrics(SM_CXDRAG) ||
                       abs(pt.y - g_llDownPt.y) >=
                           GetSystemMetrics(SM_CYDRAG)) {
                Wh_Log(L"Moving root window %08X",
                       (DWORD)(ULONG_PTR)g_llCandidateRoot);
                g_llCandidate = false;
                g_llDragGrab = CalcDragGrab(g_llCandidateRoot, g_llDownPt);
                g_llDragRoot = g_llCandidateRoot;

                // The press was swallowed, so the window wasn't brought to the
                // front the way a click on it would have been. Allowed because
                // the process holding the hook is the foreground one.
                DWORD processId = 0;
                GetWindowThreadProcessId(g_llDragRoot, &processId);
                if (!SetForegroundWindow(g_llDragRoot)) {
                    Wh_Log(L"SetForegroundWindow error: %u", GetLastError());
                } else if (processId != GetCurrentProcessId()) {
                    // The Alt release goes there now, and a later one here
                    // isn't to be taken for it.
                    g_swallowedPress = false;
                }

                g_llDragOverlayWnd = CreateDragOverlay(pt);
                MoveDraggedWindow(g_llDragRoot, g_llDragGrab, pt);
            }
            break;
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// A hook left over from before is discarded along with its drag state. The
// release ending a drag may never arrive: a hook further along the chain can
// swallow it, and the system drops a hook which took too long to return. What
// is left then keeps the hook, or a dead handle, in place for good.
void ResetLowLevelMouseHook() {
    std::lock_guard<std::mutex> guard(g_lowLevelMouseHookMutex);

    if (g_lowLevelMouseHook) {
        UnhookWindowsHookEx(g_lowLevelMouseHook);
        g_lowLevelMouseHook = nullptr;
    }

    g_llCandidate = false;
    g_llDragRoot = nullptr;
    DestroyDragOverlay();
}

void InstallLowLevelMouseHookIfNeeded() {
    std::lock_guard<std::mutex> guard(g_lowLevelMouseHookMutex);
    if (g_lowLevelMouseHook || g_uninitializing) {
        return;
    }

    g_lowLevelMouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc,
                                           GetModuleHandle(nullptr), 0);
    if (g_lowLevelMouseHook) {
        g_lowLevelMouseHookThreadId = GetCurrentThreadId();
    } else {
        Wh_Log(L"SetWindowsHookEx(WH_MOUSE_LL) error: %u", GetLastError());
    }
}

void RemoveLowLevelMouseHookIfIdle() {
    std::lock_guard<std::mutex> guard(g_lowLevelMouseHookMutex);

    // Only the hook's thread can tell whether a press is being held or a drag
    // is in progress, which keep the hook, as does Alt. An Alt release
    // retrieved elsewhere leaves the removal to the callback, which checks on
    // the next mouse event.
    if (!g_lowLevelMouseHook ||
        GetCurrentThreadId() != g_lowLevelMouseHookThreadId || g_llCandidate ||
        g_llDragRoot || GetAsyncKeyState(VK_MENU) < 0) {
        return;
    }

    UnhookWindowsHookEx(g_lowLevelMouseHook);
    g_lowLevelMouseHook = nullptr;
}

void SetThreadHooksIfNeeded() {
    if (g_threadHooksAttempted) {
        return;
    }

    g_threadHooksAttempted = true;

    std::lock_guard<std::mutex> guard(g_allThreadHooksMutex);
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

            // The low level hook and its overlay go away with their thread.
            if (g_lowLevelMouseHook &&
                GetCurrentThreadId() == g_lowLevelMouseHookThreadId) {
                g_lowLevelMouseHook = nullptr;
                g_llCandidate = false;
                g_llDragRoot = nullptr;
                g_llDragOverlayWnd = nullptr;
            }
            break;

        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;
}

void LoadSettings() {
    g_settings.dragWindowsWithoutTitleBar =
        Wh_GetIntSetting(L"dragWindowsWithoutTitleBar");
}

BOOL Wh_ModInit() {
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

    // Lets a lower integrity process, e.g. a UWP app, request a move of a root
    // window in this process.
    ChangeWindowMessageFilter(g_moveRequestMessage, MSGFLT_ADD);

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

    {
        std::lock_guard<std::mutex> guard(g_allThreadHooksMutex);

        for (HHOOK hook : g_allThreadHooks) {
            UnhookWindowsHookEx(hook);
        }

        g_allThreadHooks.clear();
    }

    ChangeWindowMessageFilter(g_moveRequestMessage, MSGFLT_REMOVE);

    {
        std::lock_guard<std::mutex> guard(g_lowLevelMouseHookMutex);

        if (g_lowLevelMouseHook) {
            UnhookWindowsHookEx(g_lowLevelMouseHook);
            g_lowLevelMouseHook = nullptr;
        }
    }

    while (g_hookRefCount > 0) {
        Sleep(200);
    }

    // The class may outlive the window, and is then found in place next time.
    DestroyDragOverlay();
    UnregisterClass(kDragOverlayClassName, GetModuleHandle(nullptr));
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}
