// ==WindhawkMod==
// @id              vlc-pip-toggle-v2
// @name            VLC One-Key PiP
// @description     VLC PiP with F8 / Ctrl+Alt+V, title-bar double-click, dragging and resizing.
// @version         4.2
// @author          Maverick1254
// @github          https://github.com/Desmond1254
// @include         vlc.exe
// @compilerOptions -luser32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# VLC One-Key PiP

A simple Windhawk mod that adds a fast and convenient Picture-in-Picture mode to VLC Media Player.

## Features

* **F8** — Toggle Picture-in-Picture mode
* **Ctrl + Alt + V** — Toggle Picture-in-Picture mode
* **Double-click VLC's title bar** — Toggle PiP mode
* Opens PiP at **480×270** by default
* **Always-on-top** PiP window
* Drag the borderless window anywhere on the screen
* Resize the window from all four edges and four corners
* Automatically restores VLC's original window size, position, and window style when exiting PiP

## How to Use

1. Install and enable the mod in Windhawk.
2. Open VLC and play a video.
3. Press **F8** or **Ctrl + Alt + V** to enter PiP mode.
4. Drag the PiP window to move it.
5. Drag any edge or corner to resize it.
6. Press the same hotkey again to return VLC to its previous state.

You can also **double-click VLC's normal title bar** to toggle PiP.

## Notes

The mod is designed for VLC's standard Windows window and does not require any additional PiP software or external utilities.

The original VLC window position, size, window style, and Always-on-Top state are saved when entering PiP and restored when leaving it.

*/
// ==/WindhawkModReadme==


#include <windows.h>

static HANDLE g_hotkeyThread = nullptr;
static HANDLE g_stopEvent = nullptr;

static DWORD g_hotkeyThreadId = 0;
static HHOOK g_mouseHook = nullptr;

static HWND g_vlcWindow = nullptr;

static RECT g_originalRect = {};
static LONG_PTR g_originalStyle = 0;
static LONG_PTR g_originalExStyle = 0;

static bool g_pipMode = false;
static bool g_wasTopMost = false;

enum MouseOperation
{
OP_NONE,
OP_DRAG,
OP_RESIZE_LEFT,
OP_RESIZE_RIGHT,
OP_RESIZE_TOP,
OP_RESIZE_BOTTOM,
OP_RESIZE_TOPLEFT,
OP_RESIZE_TOPRIGHT,
OP_RESIZE_BOTTOMLEFT,
OP_RESIZE_BOTTOMRIGHT
};

static MouseOperation g_mouseOperation = OP_NONE;

static POINT g_mouseStart = {};
static RECT g_resizeStartRect = {};

static const int RESIZE_BORDER = 10;

#define HOTKEY_ID_CTRL_ALT_V 1001
#define HOTKEY_ID_F8 1002

#define WM_PIP_TITLEBAR_DBLCLICK (WM_APP + 100)

bool IsVLC(HWND hwnd)
{
if (!hwnd)
return false;

DWORD pid = 0;

GetWindowThreadProcessId(hwnd, &pid);

if (!pid)
    return false;

HANDLE process = OpenProcess(
    PROCESS_QUERY_LIMITED_INFORMATION,
    FALSE,
    pid
);

if (!process)
    return false;

wchar_t path[MAX_PATH] = {};
DWORD size = MAX_PATH;

bool result = false;

if (QueryFullProcessImageNameW(
        process,
        0,
        path,
        &size))
{
    const wchar_t* filename = path;

    for (const wchar_t* p = path; *p; ++p)
    {
        if (*p == L'\\' || *p == L'/')
            filename = p + 1;
    }

    if (_wcsicmp(filename, L"vlc.exe") == 0)
        result = true;
}

CloseHandle(process);

return result;

}

HWND GetVLCParent(HWND hwnd)
{
if (!hwnd)
return nullptr;

HWND root = GetAncestor(hwnd, GA_ROOT);

if (IsVLC(root))
    return root;

return nullptr;

}

void SendCtrlH(HWND hwnd)
{
if (!IsWindow(hwnd))
return;

SetForegroundWindow(hwnd);

Sleep(500);

INPUT input[4] = {};

input[0].type = INPUT_KEYBOARD;
input[0].ki.wVk = VK_CONTROL;

input[1].type = INPUT_KEYBOARD;
input[1].ki.wVk = 'H';

input[2].type = INPUT_KEYBOARD;
input[2].ki.wVk = 'H';
input[2].ki.dwFlags = KEYEVENTF_KEYUP;

input[3].type = INPUT_KEYBOARD;
input[3].ki.wVk = VK_CONTROL;
input[3].ki.dwFlags = KEYEVENTF_KEYUP;

SendInput(4, input, sizeof(INPUT));

Sleep(1000);

}

void EnterPiP()
{
HWND hwnd = GetForegroundWindow();

if (!IsVLC(hwnd))
    hwnd = GetVLCParent(hwnd);

if (!IsVLC(hwnd))
    return;

g_vlcWindow = hwnd;

GetWindowRect(hwnd, &g_originalRect);

g_originalStyle =
    GetWindowLongPtrW(hwnd, GWL_STYLE);

g_originalExStyle =
    GetWindowLongPtrW(hwnd, GWL_EXSTYLE);

g_wasTopMost =
    (g_originalExStyle & WS_EX_TOPMOST) != 0;

SendCtrlH(hwnd);

LONG_PTR newStyle = g_originalStyle;

newStyle &= ~WS_CAPTION;
newStyle &= ~WS_THICKFRAME;
newStyle &= ~WS_MINIMIZEBOX;
newStyle &= ~WS_MAXIMIZEBOX;
newStyle &= ~WS_SYSMENU;

SetWindowLongPtrW(
    hwnd,
    GWL_STYLE,
    newStyle
);

HMONITOR monitor =
    MonitorFromWindow(
        hwnd,
        MONITOR_DEFAULTTONEAREST
    );

MONITORINFO mi = {};
mi.cbSize = sizeof(mi);

GetMonitorInfoW(monitor, &mi);

const int pipWidth = 480;
const int pipHeight = 270;
const int margin = 15;

int x =
    mi.rcWork.right -
    pipWidth -
    margin;

int y =
    mi.rcWork.bottom -
    pipHeight -
    margin;

SetWindowPos(
    hwnd,
    HWND_TOPMOST,
    x,
    y,
    pipWidth,
    pipHeight,
    SWP_FRAMECHANGED |
    SWP_SHOWWINDOW
);

g_pipMode = true;
g_mouseOperation = OP_NONE;

}

void ExitPiP()
{
if (!g_vlcWindow ||
!IsWindow(g_vlcWindow))
{
g_vlcWindow = nullptr;
g_pipMode = false;
g_mouseOperation = OP_NONE;
return;
}

HWND hwnd = g_vlcWindow;

g_mouseOperation = OP_NONE;

SetWindowLongPtrW(
    hwnd,
    GWL_STYLE,
    g_originalStyle
);

SetWindowLongPtrW(
    hwnd,
    GWL_EXSTYLE,
    g_originalExStyle
);

SendCtrlH(hwnd);

int width =
    g_originalRect.right -
    g_originalRect.left;

int height =
    g_originalRect.bottom -
    g_originalRect.top;

HWND zOrder =
    g_wasTopMost
        ? HWND_TOPMOST
        : HWND_NOTOPMOST;

SetWindowPos(
    hwnd,
    zOrder,
    g_originalRect.left,
    g_originalRect.top,
    width,
    height,
    SWP_FRAMECHANGED |
    SWP_SHOWWINDOW
);

g_vlcWindow = nullptr;
g_pipMode = false;

}

void TogglePiP()
{
if (g_pipMode)
ExitPiP();
else
EnterPiP();
}

MouseOperation GetResizeOperation(
HWND hwnd,
POINT cursor
)
{
if (!hwnd)
return OP_NONE;

RECT r = {};

GetWindowRect(hwnd, &r);

bool left =
    cursor.x >= r.left &&
    cursor.x <= r.left + RESIZE_BORDER;

bool right =
    cursor.x >= r.right - RESIZE_BORDER &&
    cursor.x <= r.right;

bool top =
    cursor.y >= r.top &&
    cursor.y <= r.top + RESIZE_BORDER;

bool bottom =
    cursor.y >= r.bottom - RESIZE_BORDER &&
    cursor.y <= r.bottom;

if (left && top)
    return OP_RESIZE_TOPLEFT;

if (right && top)
    return OP_RESIZE_TOPRIGHT;

if (left && bottom)
    return OP_RESIZE_BOTTOMLEFT;

if (right && bottom)
    return OP_RESIZE_BOTTOMRIGHT;

if (left)
    return OP_RESIZE_LEFT;

if (right)
    return OP_RESIZE_RIGHT;

if (top)
    return OP_RESIZE_TOP;

if (bottom)
    return OP_RESIZE_BOTTOM;

return OP_NONE;

}

void UpdateResizeCursor(
MouseOperation operation
)
{
LPCWSTR cursor = IDC_ARROW;

switch (operation)
{
    case OP_RESIZE_LEFT:
    case OP_RESIZE_RIGHT:
        cursor = IDC_SIZEWE;
        break;

    case OP_RESIZE_TOP:
    case OP_RESIZE_BOTTOM:
        cursor = IDC_SIZENS;
        break;

    case OP_RESIZE_TOPLEFT:
    case OP_RESIZE_BOTTOMRIGHT:
        cursor = IDC_SIZENWSE;
        break;

    case OP_RESIZE_TOPRIGHT:
    case OP_RESIZE_BOTTOMLEFT:
        cursor = IDC_SIZENESW;
        break;

    default:
        cursor = IDC_ARROW;
        break;
}

SetCursor(
    LoadCursorW(
        nullptr,
        cursor
    )
);

}

void StartMouseOperation(POINT cursor)
{
if (!g_pipMode ||
!g_vlcWindow ||
!IsWindow(g_vlcWindow))
return;

MouseOperation resizeOperation =
    GetResizeOperation(
        g_vlcWindow,
        cursor
    );

if (resizeOperation != OP_NONE)
{
    g_mouseOperation = resizeOperation;
    g_mouseStart = cursor;

    GetWindowRect(
        g_vlcWindow,
        &g_resizeStartRect
    );

    return;
}

HWND underCursor =
    WindowFromPoint(cursor);

HWND root =
    GetAncestor(
        underCursor,
        GA_ROOT
    );

if (root == g_vlcWindow ||
    underCursor == g_vlcWindow ||
    IsChild(g_vlcWindow, underCursor))
{
    g_mouseOperation = OP_DRAG;
    g_mouseStart = cursor;

    GetWindowRect(
        g_vlcWindow,
        &g_resizeStartRect
    );
}

}

void HandleMouseOperation()
{
if (!g_pipMode ||
!g_vlcWindow ||
!IsWindow(g_vlcWindow))
{
g_mouseOperation = OP_NONE;
return;
}

POINT cursor = {};
GetCursorPos(&cursor);

if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000))
{
    g_mouseOperation = OP_NONE;
    return;
}

if (g_mouseOperation == OP_DRAG)
{
    int dx =
        cursor.x - g_mouseStart.x;

    int dy =
        cursor.y - g_mouseStart.y;

    int width =
        g_resizeStartRect.right -
        g_resizeStartRect.left;

    int height =
        g_resizeStartRect.bottom -
        g_resizeStartRect.top;

    SetWindowPos(
        g_vlcWindow,
        HWND_TOPMOST,
        g_resizeStartRect.left + dx,
        g_resizeStartRect.top + dy,
        width,
        height,
        SWP_NOACTIVATE
    );

    return;
}

if (g_mouseOperation >= OP_RESIZE_LEFT)
{
    int dx =
        cursor.x - g_mouseStart.x;

    int dy =
        cursor.y - g_mouseStart.y;

    int left =
        g_resizeStartRect.left;

    int right =
        g_resizeStartRect.right;

    int top =
        g_resizeStartRect.top;

    int bottom =
        g_resizeStartRect.bottom;

    const int minWidth = 240;
    const int minHeight = 135;

    if (g_mouseOperation == OP_RESIZE_LEFT ||
        g_mouseOperation == OP_RESIZE_TOPLEFT ||
        g_mouseOperation == OP_RESIZE_BOTTOMLEFT)
    {
        int newLeft =
            g_resizeStartRect.left + dx;

        if (right - newLeft >= minWidth)
            left = newLeft;
    }

    if (g_mouseOperation == OP_RESIZE_RIGHT ||
        g_mouseOperation == OP_RESIZE_TOPRIGHT ||
        g_mouseOperation == OP_RESIZE_BOTTOMRIGHT)
    {
        int newRight =
            g_resizeStartRect.right + dx;

        if (newRight - left >= minWidth)
            right = newRight;
    }

    if (g_mouseOperation == OP_RESIZE_TOP ||
        g_mouseOperation == OP_RESIZE_TOPLEFT ||
        g_mouseOperation == OP_RESIZE_TOPRIGHT)
    {
        int newTop =
            g_resizeStartRect.top + dy;

        if (bottom - newTop >= minHeight)
            top = newTop;
    }

    if (g_mouseOperation == OP_RESIZE_BOTTOM ||
        g_mouseOperation == OP_RESIZE_BOTTOMLEFT ||
        g_mouseOperation == OP_RESIZE_BOTTOMRIGHT)
    {
        int newBottom =
            g_resizeStartRect.bottom + dy;

        if (newBottom - top >= minHeight)
            bottom = newBottom;
    }

    SetWindowPos(
        g_vlcWindow,
        HWND_TOPMOST,
        left,
        top,
        right - left,
        bottom - top,
        SWP_NOACTIVATE
    );
}

}

void HandleDraggingAndResizing()
{
if (!g_pipMode ||
!g_vlcWindow ||
!IsWindow(g_vlcWindow))
return;

POINT cursor = {};
GetCursorPos(&cursor);

bool leftButton =
    (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

if (leftButton &&
    g_mouseOperation == OP_NONE)
{
    StartMouseOperation(cursor);
}

if (leftButton &&
    g_mouseOperation != OP_NONE)
{
    HandleMouseOperation();
}

if (!leftButton)
{
    g_mouseOperation = OP_NONE;

    MouseOperation hoverOperation =
        GetResizeOperation(
            g_vlcWindow,
            cursor
        );

    UpdateResizeCursor(
        hoverOperation
    );
}

}

bool IsVLCNativeTitleBarClick(
HWND hwnd,
POINT point
)
{
if (!hwnd ||
!IsWindow(hwnd))
return false;

if (!IsVLC(hwnd))
    return false;

LONG_PTR style =
    GetWindowLongPtrW(
        hwnd,
        GWL_STYLE
    );

if (!(style & WS_CAPTION))
    return false;

RECT r = {};
GetWindowRect(hwnd, &r);

int frame =
    GetSystemMetrics(
        SM_CYSIZEFRAME
    );

int caption =
    GetSystemMetrics(
        SM_CYCAPTION
    );

int titleBottom =
    r.top + frame + caption;

return
    point.x >= r.left &&
    point.x <= r.right &&
    point.y >= r.top &&
    point.y <= titleBottom;

}

LRESULT CALLBACK LowLevelMouseProc(
int nCode,
WPARAM wParam,
LPARAM lParam
)
{
if (nCode == HC_ACTION &&
wParam == WM_LBUTTONDOWN)
{
MSLLHOOKSTRUCT* mouse =
reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);

    if (mouse)
    {
        static DWORD lastClickTime = 0;
        static HWND lastWindow = nullptr;

        DWORD now = GetTickCount();

        HWND hwnd =
            WindowFromPoint(mouse->pt);

        HWND root =
            GetAncestor(
                hwnd,
                GA_ROOT
            );

        if (IsVLC(root) &&
            IsVLCNativeTitleBarClick(
                root,
                mouse->pt
            ))
        {
            DWORD doubleClickTime =
                GetDoubleClickTime();

            bool isDoubleClick =
                lastWindow == root &&
                now - lastClickTime <=
                    doubleClickTime;

            lastClickTime = now;
            lastWindow = root;

            if (isDoubleClick)
            {
                if (g_hotkeyThreadId)
                {
                    PostThreadMessageW(
                        g_hotkeyThreadId,
                        WM_PIP_TITLEBAR_DBLCLICK,
                        0,
                        0
                    );
                }

                return 1;
            }
        }
        else
        {
            lastWindow = nullptr;
            lastClickTime = 0;
        }
    }
}

return CallNextHookEx(
    g_mouseHook,
    nCode,
    wParam,
    lParam
);

}

DWORD WINAPI HotkeyThread(LPVOID)
{
g_hotkeyThreadId =
GetCurrentThreadId();

// Ctrl + Alt + V
if (!RegisterHotKey(
        nullptr,
        HOTKEY_ID_CTRL_ALT_V,
        MOD_CONTROL | MOD_ALT,
        'V'
    ))
{
    g_hotkeyThreadId = 0;
    return 1;
}

// F8
if (!RegisterHotKey(
        nullptr,
        HOTKEY_ID_F8,
        0,
        VK_F8
    ))
{
    UnregisterHotKey(
        nullptr,
        HOTKEY_ID_CTRL_ALT_V
    );

    g_hotkeyThreadId = 0;
    return 1;
}

g_mouseHook =
    SetWindowsHookExW(
        WH_MOUSE_LL,
        LowLevelMouseProc,
        GetModuleHandleW(nullptr),
        0
    );

MSG msg = {};

while (
    WaitForSingleObject(
        g_stopEvent,
        5
    ) == WAIT_TIMEOUT)
{
    while (
        PeekMessageW(
            &msg,
            nullptr,
            0,
            0,
            PM_REMOVE
        ))
    {
        if (msg.message == WM_HOTKEY)
        {
            if (msg.wParam == HOTKEY_ID_CTRL_ALT_V ||
                msg.wParam == HOTKEY_ID_F8)
            {
                TogglePiP();
            }
        }
        else if (
            msg.message ==
            WM_PIP_TITLEBAR_DBLCLICK)
        {
            TogglePiP();
        }
    }

    HandleDraggingAndResizing();

    Sleep(5);
}

if (g_mouseHook)
{
    UnhookWindowsHookEx(
        g_mouseHook
    );

    g_mouseHook = nullptr;
}

UnregisterHotKey(
    nullptr,
    HOTKEY_ID_CTRL_ALT_V
);

UnregisterHotKey(
    nullptr,
    HOTKEY_ID_F8
);

g_hotkeyThreadId = 0;

return 0;

}

BOOL Wh_ModInit()
{
g_stopEvent =
CreateEventW(
nullptr,
TRUE,
FALSE,
nullptr
);

if (!g_stopEvent)
    return FALSE;

g_hotkeyThread =
    CreateThread(
        nullptr,
        0,
        HotkeyThread,
        nullptr,
        0,
        nullptr
    );

if (!g_hotkeyThread)
{
    CloseHandle(g_stopEvent);
    g_stopEvent = nullptr;
    return FALSE;
}

return TRUE;

}

void Wh_ModUninit()
{
if (g_stopEvent)
{
SetEvent(
g_stopEvent
);
}

if (g_hotkeyThread)
{
    WaitForSingleObject(
        g_hotkeyThread,
        2000
    );

    CloseHandle(
        g_hotkeyThread
    );

    g_hotkeyThread = nullptr;
}

if (g_stopEvent)
{
    CloseHandle(
        g_stopEvent
    );

    g_stopEvent = nullptr;
}

if (g_vlcWindow &&
    IsWindow(g_vlcWindow))
{
    SetWindowLongPtrW(
        g_vlcWindow,
        GWL_STYLE,
        g_originalStyle
    );

    SetWindowLongPtrW(
        g_vlcWindow,
        GWL_EXSTYLE,
        g_originalExStyle
    );

    SetWindowPos(
        g_vlcWindow,
        g_wasTopMost
            ? HWND_TOPMOST
            : HWND_NOTOPMOST,
        g_originalRect.left,
        g_originalRect.top,
        g_originalRect.right -
            g_originalRect.left,
        g_originalRect.bottom -
            g_originalRect.top,
        SWP_FRAMECHANGED |
        SWP_SHOWWINDOW
    );
}

g_vlcWindow = nullptr;
g_pipMode = false;
g_mouseOperation = OP_NONE;

}
