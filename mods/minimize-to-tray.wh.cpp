// ==WindhawkMod==
// @id              minimize-to-tray
// @name            Minimize to tray
// @description     Right-click a window's minimize button to hide it in the system tray.
// @version         1.0
// @author          0Allu
// @github          https://github.com/0Allu
// @homepage        https://github.com/0Allu/minimize-to-tray
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lshell32 -ldwmapi
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Minimize to tray

Right-click a window's minimize button to hide the window in the system tray.

## Usage

- Right-click a window's minimize button to send it to the tray.
- Left-click its tray icon to restore it.
- Right-click its tray icon to restore or close the window.

The application continues running while its window is hidden.

## Compatibility

The mod supports standard Windows title bars and many applications with
custom title bars, including Chromium/Electron applications and Steam.

Some applications with unusual custom title bars might not be detected.

Applications running with administrator privileges are not supported when
Windows Explorer is running at normal user privileges. This is due to Windows
User Interface Privilege Isolation (UIPI).

## Safety

If the mod is disabled or unloaded normally, all windows hidden by the mod
are automatically restored.
*/
// ==/WindhawkModReadme==

#define NOMINMAX

#include <dwmapi.h>
#include <shellapi.h>
#include <windows.h>

#include <algorithm>
#include <string>
#include <vector>

namespace {

// -----------------------------------------------------------------------------
// Constants
// -----------------------------------------------------------------------------

constexpr UINT WM_TRAY_ICON = WM_APP + 100;
constexpr UINT WM_HIDE_WINDOW = WM_APP + 101;

constexpr UINT_PTR TIMER_CLEANUP = 1;

constexpr UINT MENU_RESTORE = 1;
constexpr UINT MENU_CLOSE = 2;

constexpr wchar_t HIDDEN_WINDOW_PROPERTY[] =
    L"Windhawk.MinimizeToTray.HiddenWindow.v1";

constexpr wchar_t CONTROLLER_CLASS[] = L"WindhawkMinimizeToTrayController";

// -----------------------------------------------------------------------------
// Tray item
// -----------------------------------------------------------------------------

struct TrayItem {
    HWND hwnd = nullptr;
    DWORD processId = 0;

    UINT id = 0;

    HICON icon = nullptr;
    bool ownsIcon = false;

    std::wstring title;
};

// -----------------------------------------------------------------------------
// Globals
// -----------------------------------------------------------------------------

HWND g_controllerWindow = nullptr;
HHOOK g_mouseHook = nullptr;

HANDLE g_workerThread = nullptr;
HANDLE g_workerStartedEvent = nullptr;

bool g_workerStartedSuccessfully = false;
bool g_activeExplorerInstance = false;

UINT g_taskbarCreatedMessage = 0;
UINT g_nextTrayId = 1;

HWND g_rightClickWindow = nullptr;

std::vector<TrayItem> g_trayItems;

// -----------------------------------------------------------------------------
// Basic window helpers
// -----------------------------------------------------------------------------

DWORD GetWindowProcessId(HWND hwnd) {
    DWORD processId = 0;

    if (hwnd) {
        GetWindowThreadProcessId(hwnd, &processId);
    }

    return processId;
}

bool IsTrayItemWindowValid(const TrayItem& item) {
    if (!IsWindow(item.hwnd)) {
        return false;
    }

    return GetWindowProcessId(item.hwnd) == item.processId;
}

TrayItem* FindTrayItemByWindow(HWND hwnd) {
    DWORD processId = GetWindowProcessId(hwnd);

    for (auto& item : g_trayItems) {
        if (item.hwnd == hwnd && item.processId == processId) {
            return &item;
        }
    }

    return nullptr;
}

TrayItem* FindTrayItemById(UINT id) {
    for (auto& item : g_trayItems) {
        if (item.id == id) {
            return &item;
        }
    }

    return nullptr;
}

std::wstring GetWindowTitleSafe(HWND hwnd) {
    wchar_t title[256] = {};

    GetWindowTextW(hwnd, title, ARRAYSIZE(title));

    if (!title[0]) {
        return L"Minimized window";
    }

    return title;
}

// -----------------------------------------------------------------------------
// Window icon
// -----------------------------------------------------------------------------

HICON CopyWindowIcon(HWND hwnd, bool* ownsIcon) {
    if (ownsIcon) {
        *ownsIcon = false;
    }

    const WPARAM iconTypes[] = {ICON_SMALL2, ICON_SMALL, ICON_BIG};

    for (WPARAM iconType : iconTypes) {
        DWORD_PTR result = 0;

        if (SendMessageTimeoutW(hwnd, WM_GETICON, iconType, 0,
                                SMTO_ABORTIFHUNG | SMTO_BLOCK, 50, &result) &&
            result) {
            HICON copiedIcon = CopyIcon(reinterpret_cast<HICON>(result));

            if (copiedIcon) {
                if (ownsIcon) {
                    *ownsIcon = true;
                }

                return copiedIcon;
            }
        }
    }

    HICON classIcon =
        reinterpret_cast<HICON>(GetClassLongPtrW(hwnd, GCLP_HICONSM));

    if (!classIcon) {
        classIcon = reinterpret_cast<HICON>(GetClassLongPtrW(hwnd, GCLP_HICON));
    }

    if (classIcon) {
        HICON copiedIcon = CopyIcon(classIcon);

        if (copiedIcon) {
            if (ownsIcon) {
                *ownsIcon = true;
            }

            return copiedIcon;
        }
    }

    HICON fallback = LoadIconW(nullptr, IDI_APPLICATION);

    if (fallback) {
        HICON copiedIcon = CopyIcon(fallback);

        if (copiedIcon) {
            if (ownsIcon) {
                *ownsIcon = true;
            }

            return copiedIcon;
        }
    }

    return fallback;
}

// -----------------------------------------------------------------------------
// Window filtering
// -----------------------------------------------------------------------------

bool IsExcludedWindow(HWND hwnd) {
    if (!hwnd) {
        return true;
    }

    if (hwnd == g_controllerWindow || hwnd == GetDesktopWindow()) {
        return true;
    }

    wchar_t className[128] = {};

    GetClassNameW(hwnd, className, ARRAYSIZE(className));

    return wcscmp(className, L"Shell_TrayWnd") == 0 ||

           wcscmp(className, L"Shell_SecondaryTrayWnd") == 0 ||

           wcscmp(className, L"Progman") == 0 ||

           wcscmp(className, L"WorkerW") == 0;
}

// -----------------------------------------------------------------------------
// Custom title-bar detection
// -----------------------------------------------------------------------------

bool IsPointInApproximateMinimizeButton(HWND hwnd, POINT screenPoint) {
    RECT windowRect = {};

    if (!GetWindowRect(hwnd, &windowRect)) {
        return false;
    }

    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);

    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);

    if (style & WS_CHILD) {
        return false;
    }

    if (exStyle & WS_EX_NOACTIVATE) {
        return false;
    }

    UINT dpi = GetDpiForWindow(hwnd);

    if (!dpi) {
        dpi = 96;
    }

    wchar_t className[128] = {};

    GetClassNameW(hwnd, className, ARRAYSIZE(className));

    // -------------------------------------------------------------------------
    // Steam / SDL
    // -------------------------------------------------------------------------
    //
    // Steam uses an SDL top-level window and custom-drawn caption buttons.
    // Its buttons are narrower than standard Windows caption buttons.
    //

    if (wcscmp(className, L"SDL_app") == 0) {
        int relativeY = screenPoint.y - windowRect.top;

        int distanceFromRight = windowRect.right - screenPoint.x;

        int maxTitleBarHeight = MulDiv(48, dpi, 96);

        int minimumDistance = MulDiv(68, dpi, 96);

        int maximumDistance = MulDiv(115, dpi, 96);

        return relativeY >= 0 && relativeY < maxTitleBarHeight &&
               distanceFromRight >= minimumDistance &&
               distanceFromRight <= maximumDistance;
    }

    // -------------------------------------------------------------------------
    // Generic custom frame
    // -------------------------------------------------------------------------

    int buttonWidth = GetSystemMetricsForDpi(SM_CXSIZE, dpi);

    int buttonHeight = GetSystemMetricsForDpi(SM_CYSIZE, dpi);

    int frameWidth = GetSystemMetricsForDpi(SM_CXSIZEFRAME, dpi);

    int frameHeight = GetSystemMetricsForDpi(SM_CYSIZEFRAME, dpi);

    int paddedBorder = GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);

    int titleBarHeight = std::max(buttonHeight + frameHeight + paddedBorder,
                                  MulDiv(48, dpi, 96));

    if (screenPoint.y < windowRect.top ||

        screenPoint.y >= windowRect.top + titleBarHeight) {
        return false;
    }

    int customButtonWidth = std::max(buttonWidth, MulDiv(46, dpi, 96));

    bool rightToLeft = (exStyle & WS_EX_LAYOUTRTL) != 0;

    if (!rightToLeft) {
        int rightEdge = windowRect.right - frameWidth;

        int minimizeLeft = rightEdge - customButtonWidth * 3;

        int minimizeRight = rightEdge - customButtonWidth * 2;

        return screenPoint.x >= minimizeLeft && screenPoint.x < minimizeRight;
    }

    int leftEdge = windowRect.left + frameWidth;

    int minimizeLeft = leftEdge + customButtonWidth * 2;

    int minimizeRight = leftEdge + customButtonWidth * 3;

    return screenPoint.x >= minimizeLeft && screenPoint.x < minimizeRight;
}

// -----------------------------------------------------------------------------
// Main minimize-button hit testing
// -----------------------------------------------------------------------------

HWND FindMinimizeButtonWindow(POINT screenPoint) {
    HWND hwnd = WindowFromPoint(screenPoint);

    if (!hwnd) {
        return nullptr;
    }

    hwnd = GetAncestor(hwnd, GA_ROOT);

    if (!hwnd || !IsWindow(hwnd) || !IsWindowVisible(hwnd) ||
        IsExcludedWindow(hwnd)) {
        return nullptr;
    }

    LPARAM pointParam = MAKELPARAM(static_cast<SHORT>(screenPoint.x),
                                   static_cast<SHORT>(screenPoint.y));

    // -------------------------------------------------------------------------
    // Method 1: WM_NCHITTEST
    // -------------------------------------------------------------------------

    DWORD_PTR hitTestResult = HTNOWHERE;

    if (SendMessageTimeoutW(hwnd, WM_NCHITTEST, 0, pointParam,
                            SMTO_ABORTIFHUNG | SMTO_BLOCK, 50,
                            &hitTestResult)) {
        if (static_cast<LRESULT>(hitTestResult) == HTMINBUTTON) {
            return hwnd;
        }
    }

    // -------------------------------------------------------------------------
    // Method 2: DWM caption-button hit testing
    // -------------------------------------------------------------------------

    LRESULT dwmHitTest = HTNOWHERE;

    if (DwmDefWindowProc(hwnd, WM_NCHITTEST, 0, pointParam, &dwmHitTest)) {
        if (dwmHitTest == HTMINBUTTON) {
            return hwnd;
        }
    }

    // -------------------------------------------------------------------------
    // Method 3: Custom-title-bar geometry
    // -------------------------------------------------------------------------

    if (IsPointInApproximateMinimizeButton(hwnd, screenPoint)) {
        return hwnd;
    }

    return nullptr;
}

// -----------------------------------------------------------------------------
// Notification area icons
// -----------------------------------------------------------------------------

bool AddNotificationIcon(const TrayItem& item) {
    NOTIFYICONDATAW nid = {};

    nid.cbSize = sizeof(nid);

    nid.hWnd = g_controllerWindow;

    nid.uID = item.id;

    nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;

    nid.uCallbackMessage = WM_TRAY_ICON;

    nid.hIcon = item.icon;

    wcsncpy_s(nid.szTip, ARRAYSIZE(nid.szTip), item.title.c_str(), _TRUNCATE);

    return Shell_NotifyIconW(NIM_ADD, &nid) != FALSE;
}

void DeleteNotificationIcon(const TrayItem& item) {
    NOTIFYICONDATAW nid = {};

    nid.cbSize = sizeof(nid);

    nid.hWnd = g_controllerWindow;

    nid.uID = item.id;

    Shell_NotifyIconW(NIM_DELETE, &nid);
}

void RecreateNotificationIcons() {
    for (const auto& item : g_trayItems) {
        AddNotificationIcon(item);
    }
}

// -----------------------------------------------------------------------------
// Tray item management
// -----------------------------------------------------------------------------

void AddTrayItem(HWND hwnd, bool hideWindow) {
    if (!IsWindow(hwnd)) {
        return;
    }

    if (FindTrayItemByWindow(hwnd)) {
        if (hideWindow) {
            ShowWindowAsync(hwnd, SW_HIDE);
        }

        return;
    }

    TrayItem item;

    item.hwnd = hwnd;

    item.processId = GetWindowProcessId(hwnd);

    item.id = g_nextTrayId++;

    item.title = GetWindowTitleSafe(hwnd);

    item.icon = CopyWindowIcon(hwnd, &item.ownsIcon);

    SetPropW(hwnd, HIDDEN_WINDOW_PROPERTY,
             reinterpret_cast<HANDLE>(static_cast<INT_PTR>(1)));

    g_trayItems.push_back(item);

    if (!AddNotificationIcon(g_trayItems.back())) {
        RemovePropW(hwnd, HIDDEN_WINDOW_PROPERTY);

        if (item.icon && item.ownsIcon) {
            DestroyIcon(item.icon);
        }

        g_trayItems.pop_back();

        Wh_Log(L"Failed to create tray icon for window %p", hwnd);

        return;
    }

    if (hideWindow) {
        ShowWindowAsync(hwnd, SW_HIDE);
    }
}

void RemoveTrayItemByIndex(size_t index,
                           bool restoreWindow,
                           bool activateWindow) {
    if (index >= g_trayItems.size()) {
        return;
    }

    TrayItem item = g_trayItems[index];

    DeleteNotificationIcon(item);

    if (IsTrayItemWindowValid(item)) {
        RemovePropW(item.hwnd, HIDDEN_WINDOW_PROPERTY);

        if (restoreWindow) {
            ShowWindowAsync(item.hwnd, SW_SHOW);

            if (activateWindow) {
                SetForegroundWindow(item.hwnd);
            }
        }
    }

    if (item.icon && item.ownsIcon) {
        DestroyIcon(item.icon);
    }

    g_trayItems.erase(g_trayItems.begin() + index);
}

void RestoreTrayItem(UINT id) {
    for (size_t i = 0; i < g_trayItems.size(); i++) {
        if (g_trayItems[i].id != id) {
            continue;
        }

        HWND hwnd = g_trayItems[i].hwnd;

        RemoveTrayItemByIndex(i, true, false);

        if (IsWindow(hwnd)) {
            SetForegroundWindow(hwnd);
        }

        return;
    }
}

void CloseTrayItem(UINT id) {
    TrayItem* item = FindTrayItemById(id);

    if (!item || !IsTrayItemWindowValid(*item)) {
        return;
    }

    PostMessageW(item->hwnd, WM_CLOSE, 0, 0);
}

void RestoreAllWindows() {
    while (!g_trayItems.empty()) {
        RemoveTrayItemByIndex(g_trayItems.size() - 1, true, false);
    }
}

void CleanupTrayItems() {
    for (size_t i = g_trayItems.size(); i > 0; i--) {
        size_t index = i - 1;

        TrayItem& item = g_trayItems[index];

        // Application was closed.
        if (!IsTrayItemWindowValid(item)) {
            RemoveTrayItemByIndex(index, false, false);

            continue;
        }

        // The application restored its window itself, for example through
        // its own notification-area icon.
        if (IsWindowVisible(item.hwnd)) {
            RemoveTrayItemByIndex(index, false, false);
        }
    }
}

// -----------------------------------------------------------------------------
// Recovery after Explorer restart
// -----------------------------------------------------------------------------

BOOL CALLBACK RecoverWindowProc(HWND hwnd, LPARAM) {
    if (!GetPropW(hwnd, HIDDEN_WINDOW_PROPERTY)) {
        return TRUE;
    }

    // The recovery property is stale if the window is already visible.
    if (IsWindowVisible(hwnd)) {
        RemovePropW(hwnd, HIDDEN_WINDOW_PROPERTY);

        return TRUE;
    }

    AddTrayItem(hwnd, false);

    return TRUE;
}

// -----------------------------------------------------------------------------
// Tray menu
// -----------------------------------------------------------------------------

void ShowTrayMenu(UINT id) {
    if (!FindTrayItemById(id)) {
        return;
    }

    HMENU menu = CreatePopupMenu();

    if (!menu) {
        return;
    }

    AppendMenuW(menu, MF_STRING, MENU_RESTORE, L"Restore");

    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);

    AppendMenuW(menu, MF_STRING, MENU_CLOSE, L"Close");

    POINT cursor = {};

    GetCursorPos(&cursor);

    SetForegroundWindow(g_controllerWindow);

    UINT command =
        TrackPopupMenu(menu, TPM_RETURNCMD | TPM_NONOTIFY | TPM_RIGHTBUTTON,
                       cursor.x, cursor.y, 0, g_controllerWindow, nullptr);

    DestroyMenu(menu);

    PostMessageW(g_controllerWindow, WM_NULL, 0, 0);

    switch (command) {
        case MENU_RESTORE:
            RestoreTrayItem(id);
            break;

        case MENU_CLOSE:
            CloseTrayItem(id);
            break;
    }
}

// -----------------------------------------------------------------------------
// Mouse hook
// -----------------------------------------------------------------------------

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode != HC_ACTION) {
        return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
    }

    const auto* mouseInfo = reinterpret_cast<const MSLLHOOKSTRUCT*>(lParam);

    // -------------------------------------------------------------------------
    // Right-button down
    // -------------------------------------------------------------------------

    if (wParam == WM_RBUTTONDOWN) {
        HWND hwnd = FindMinimizeButtonWindow(mouseInfo->pt);

        if (hwnd) {
            g_rightClickWindow = hwnd;

            // Suppress the original right-click.
            return 1;
        }

        g_rightClickWindow = nullptr;
    }

    // -------------------------------------------------------------------------
    // Right-button up
    // -------------------------------------------------------------------------

    else if (wParam == WM_RBUTTONUP && g_rightClickWindow) {
        HWND originalWindow = g_rightClickWindow;

        g_rightClickWindow = nullptr;

        HWND currentWindow = FindMinimizeButtonWindow(mouseInfo->pt);

        if (currentWindow == originalWindow) {
            PostMessageW(g_controllerWindow, WM_HIDE_WINDOW,
                         reinterpret_cast<WPARAM>(originalWindow), 0);
        }

        // The button-down event was swallowed, so swallow its matching
        // button-up event too.
        return 1;
    }

    return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
}

// -----------------------------------------------------------------------------
// Controller window
// -----------------------------------------------------------------------------

LRESULT CALLBACK ControllerWindowProc(HWND hwnd,
                                      UINT message,
                                      WPARAM wParam,
                                      LPARAM lParam) {
    if (g_taskbarCreatedMessage && message == g_taskbarCreatedMessage) {
        RecreateNotificationIcons();
        return 0;
    }

    switch (message) {
        case WM_HIDE_WINDOW:
            AddTrayItem(reinterpret_cast<HWND>(wParam), true);

            return 0;

        case WM_TRAY_ICON: {
            UINT id = static_cast<UINT>(wParam);

            switch (static_cast<UINT>(lParam)) {
                case WM_LBUTTONUP:
                    RestoreTrayItem(id);
                    break;

                case WM_RBUTTONUP:
                    ShowTrayMenu(id);
                    break;
            }

            return 0;
        }

        case WM_TIMER:
            if (wParam == TIMER_CLEANUP) {
                CleanupTrayItems();
            }

            return 0;

        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            KillTimer(hwnd, TIMER_CLEANUP);

            if (g_mouseHook) {
                UnhookWindowsHookEx(g_mouseHook);

                g_mouseHook = nullptr;
            }

            // Never leave applications inaccessible when the mod is
            // disabled or unloaded.
            RestoreAllWindows();

            g_controllerWindow = nullptr;

            PostQuitMessage(0);

            return 0;
    }

    return DefWindowProcW(hwnd, message, wParam, lParam);
}

// -----------------------------------------------------------------------------
// Worker thread
// -----------------------------------------------------------------------------

DWORD WINAPI WorkerThreadProc(LPVOID) {
    g_taskbarCreatedMessage = RegisterWindowMessageW(L"TaskbarCreated");

    HINSTANCE instance = GetModuleHandleW(nullptr);

    WNDCLASSW windowClass = {};

    windowClass.lpfnWndProc = ControllerWindowProc;

    windowClass.hInstance = instance;

    windowClass.lpszClassName = CONTROLLER_CLASS;

    if (!RegisterClassW(&windowClass) &&
        GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        Wh_Log(L"RegisterClassW failed: %u", GetLastError());

        SetEvent(g_workerStartedEvent);

        return 1;
    }

    g_controllerWindow = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, CONTROLLER_CLASS, L"", WS_POPUP, 0,
        0, 0, 0, nullptr, nullptr, instance, nullptr);

    if (!g_controllerWindow) {
        Wh_Log(L"CreateWindowExW failed: %u", GetLastError());

        SetEvent(g_workerStartedEvent);

        return 1;
    }

    // This is the same low-level hook architecture as the working version.
    // WH_MOUSE_LL callbacks are delivered back to this thread, which owns
    // the message loop below.
    g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, nullptr, 0);

    if (!g_mouseHook) {
        Wh_Log(L"SetWindowsHookExW failed: %u", GetLastError());

        DestroyWindow(g_controllerWindow);

        SetEvent(g_workerStartedEvent);

        return 1;
    }

    SetTimer(g_controllerWindow, TIMER_CLEANUP, 2000, nullptr);

    // Recreate tray icons for windows which survived an Explorer restart.
    EnumWindows(RecoverWindowProc, 0);

    g_workerStartedSuccessfully = true;

    SetEvent(g_workerStartedEvent);

    Wh_Log(L"Minimize to tray started");

    MSG message;

    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);

        DispatchMessageW(&message);
    }

    UnregisterClassW(CONTROLLER_CLASS, instance);

    return 0;
}

}  // namespace

// -----------------------------------------------------------------------------
// Windhawk callbacks
// -----------------------------------------------------------------------------

BOOL Wh_ModInit() {
    // Explorer can optionally run folder windows in separate explorer.exe
    // processes. Only run the controller inside the Explorer process which
    // owns the Windows shell.
    HWND shellWindow = GetShellWindow();

    if (shellWindow) {
        DWORD shellProcessId = 0;

        GetWindowThreadProcessId(shellWindow, &shellProcessId);

        if (shellProcessId != 0 && shellProcessId != GetCurrentProcessId()) {
            return TRUE;
        }
    }

    g_activeExplorerInstance = true;

    g_workerStartedSuccessfully = false;

    g_workerStartedEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);

    if (!g_workerStartedEvent) {
        Wh_Log(L"CreateEventW failed: %u", GetLastError());

        return FALSE;
    }

    g_workerThread =
        CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, nullptr);

    if (!g_workerThread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());

        CloseHandle(g_workerStartedEvent);

        g_workerStartedEvent = nullptr;

        return FALSE;
    }

    WaitForSingleObject(g_workerStartedEvent, INFINITE);

    CloseHandle(g_workerStartedEvent);

    g_workerStartedEvent = nullptr;

    if (!g_workerStartedSuccessfully) {
        WaitForSingleObject(g_workerThread, INFINITE);

        CloseHandle(g_workerThread);

        g_workerThread = nullptr;

        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    if (!g_activeExplorerInstance) {
        return;
    }

    if (g_controllerWindow) {
        SendMessageW(g_controllerWindow, WM_CLOSE, 0, 0);
    }

    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, INFINITE);

        CloseHandle(g_workerThread);

        g_workerThread = nullptr;
    }

    g_activeExplorerInstance = false;
}
