// ==WindhawkMod==
// @id              explorer-folder-hover-menu
// @name            Folder Hover Menu
// @description     Hover a folder in File Explorer to get an expand button that opens a cascading menu of the folder's contents
// @version         1.2
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -luuid -lshlwapi -lshell32 -lgdi32 -lgdiplus -ladvapi32 -luiautomationcore -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Folder Hover Menu

When you hover the mouse over a folder in the File Explorer file list, a small
expand button appears in the bottom-right corner of that folder. Click it to pop
up a cascading menu of the folder's contents. You can navigate into sub-folders
and launch items straight from the menu, without opening the folder first.

It also works in open and save file dialogs. There, "Open in the current window"
navigates the dialog into the folder, and clicking a file in the menu puts it
into the dialog's file name box.

Inspired by [QTTabBar](https://qttabbar.wikidot.com/).

![Screenshot](https://i.imgur.com/ZPceXoZ.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- roundedCorners: true
  $name: Rounded menu corners
  $description: >-
    Round the corners of the pop-up menu. This option requires Windows 11.
- iconSize: 20
  $name: Button size
  $description: The size of the expand button shown on hover.
- opacity: 100
  $name: Button opacity
  $description: >-
    Opacity of the expand button, as a percentage from 1 (nearly transparent)
    to 100 (fully opaque).
- position: rightBottom
  $name: Button position
  $description: The corner of the folder item where the expand button is shown.
  $options:
  - rightBottom: Bottom-right corner
  - leftBottom: Bottom-left corner
  - leftTop: Top-left corner
  - rightTop: Top-right corner
- offsetX: 0
  $name: Horizontal offset
  $description: >-
    Shifts the button horizontally from the selected corner, in pixels. Positive
    values move it right, negative values move it left.
- offsetY: 0
  $name: Vertical offset
  $description: >-
    Shifts the button vertically from the selected corner, in pixels. Positive
    values move it down, negative values move it up.
- clickAction: currentWindow
  $name: Folder left click action
  $description: What left-clicking a folder in the pop-up menu does.
  $options:
  - nothing: Nothing
  - currentWindow: Open in the current window
  - newWindow: Open in a new window
  - newTab: Open in a new tab
- middleClickAction: newTab
  $name: Folder middle click action
  $description: What middle-clicking a folder in the pop-up menu does.
  $options:
  - nothing: Nothing
  - currentWindow: Open in the current window
  - newWindow: Open in a new window
  - newTab: Open in a new tab
- fileDialogs: true
  $name: Enable in file dialogs
  $description: >-
    Also show the expand button in open and save file dialogs. The folder click
    actions above apply there too. Clicking a file in the menu puts it into the
    dialog's file name box (it is not launched).
- maxItems: 200
  $name: Maximum items
  $description: >-
    The most items to load into a single folder's menu. When a folder has more,
    the list is truncated and a notice item is shown instead. This keeps the
    menu quick to open for folders that contain a very large number of items.
- timeoutSeconds: 5
  $name: Timeout (seconds)
  $description: >-
    Stop loading a folder's contents into the menu after this many seconds. A
    safety net for folders whose contents are slow to enumerate.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <initguid.h>  // Must come first so the shell GUIDs we use get storage.

#include <commctrl.h>
#include <comutil.h>
#include <docobj.h>
#include <dwmapi.h>
#include <exdisp.h>
#include <gdiplus.h>
#include <servprov.h>
#include <shlguid.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <uiautomation.h>
#include <windowsx.h>

#include <winrt/base.h>

#include <climits>
#include <new>
#include <string>
#include <unordered_map>
#include <vector>

// Undocumented command id in the CLSID_MenuBand command group that dismisses
// the menu band. Used to tear the popup down when the foreground process
// changes.
#define MBAND_CMDID_CLOSE 22

#ifndef SMSET_USEBKICONEXTRACTION
#define SMSET_USEBKICONEXTRACTION 0x00000008
#endif

// {ECD4FC4F-521C-11D0-B792-00A0C90312E1}. Not declared by the public headers,
// so we provide it ourselves (initguid.h above gives it storage in this TU).
DEFINE_GUID(CLSID_MenuDeskBar,
            0xecd4fc4f,
            0x521c,
            0x11d0,
            0xb7,
            0x92,
            0x0,
            0xa0,
            0xc9,
            0x3,
            0x12,
            0xe1);

#define WM_APP_SETTINGS_CHANGED (WM_APP + 1)
// Posted to ask the UI thread to quit. Its handler calls PostQuitMessage;
// posting WM_QUIT directly with PostThreadMessage is discouraged (a modal loop
// can drop it).
#define WM_APP_QUIT (WM_APP + 2)
// Worker -> UI sink window: a fresh snapshot is ready, re-evaluate the hover.
#define WM_APP_REFRESH_DONE (WM_APP + 3)
// UI -> worker thread: build a snapshot for the pending request.
#define WM_APP_DO_REFRESH (WM_APP + 4)
// UI -> worker thread: open a clicked folder (wParam is a FolderActionRequest*,
// which the worker takes ownership of). Done off the UI thread because some
// actions (notably opening a new tab) wait on Explorer.
#define WM_APP_DO_ACTION (WM_APP + 5)

////////////////////////////////////////////////////////////////////////////////
// Settings.

// Which corner of the folder item the button is anchored to.
enum class ButtonPosition {
    rightBottom,
    leftBottom,
    leftTop,
    rightTop,
};

// What clicking a folder in the pop-up menu does.
enum class FolderAction {
    nothing,
    currentWindow,
    newWindow,
    newTab,
};

struct {
    bool roundedCorners;
    int iconSize;
    int opacity;
    ButtonPosition position;
    int offsetX;
    int offsetY;
    FolderAction clickAction;
    FolderAction middleClickAction;
    bool fileDialogs;
    int maxEnumItems;
    int enumTimeoutMs;
} g_settings;

FolderAction ParseFolderAction(PCWSTR value) {
    if (wcscmp(value, L"nothing") == 0) {
        return FolderAction::nothing;
    }
    if (wcscmp(value, L"currentWindow") == 0) {
        return FolderAction::currentWindow;
    }
    if (wcscmp(value, L"newTab") == 0) {
        return FolderAction::newTab;
    }
    return FolderAction::newWindow;
}

void LoadSettings() {
    g_settings.roundedCorners = Wh_GetIntSetting(L"roundedCorners");

    int iconSize = Wh_GetIntSetting(L"iconSize");
    if (iconSize < 10) {
        iconSize = 10;
    } else if (iconSize > 64) {
        iconSize = 64;
    }
    g_settings.iconSize = iconSize;

    int opacity = Wh_GetIntSetting(L"opacity");
    if (opacity < 1) {
        opacity = 1;
    } else if (opacity > 100) {
        opacity = 100;
    }
    g_settings.opacity = opacity;

    PCWSTR position = Wh_GetStringSetting(L"position");
    g_settings.position = ButtonPosition::rightBottom;
    if (wcscmp(position, L"leftBottom") == 0) {
        g_settings.position = ButtonPosition::leftBottom;
    } else if (wcscmp(position, L"leftTop") == 0) {
        g_settings.position = ButtonPosition::leftTop;
    } else if (wcscmp(position, L"rightTop") == 0) {
        g_settings.position = ButtonPosition::rightTop;
    }
    Wh_FreeStringSetting(position);

    g_settings.offsetX = Wh_GetIntSetting(L"offsetX");
    g_settings.offsetY = Wh_GetIntSetting(L"offsetY");

    PCWSTR clickAction = Wh_GetStringSetting(L"clickAction");
    g_settings.clickAction = ParseFolderAction(clickAction);
    Wh_FreeStringSetting(clickAction);

    PCWSTR middleClickAction = Wh_GetStringSetting(L"middleClickAction");
    g_settings.middleClickAction = ParseFolderAction(middleClickAction);
    Wh_FreeStringSetting(middleClickAction);

    g_settings.fileDialogs = Wh_GetIntSetting(L"fileDialogs");

    int maxItems = Wh_GetIntSetting(L"maxItems");
    if (maxItems < 1) {
        maxItems = 1;
    }
    g_settings.maxEnumItems = maxItems;

    int timeoutSeconds = Wh_GetIntSetting(L"timeoutSeconds");
    if (timeoutSeconds < 1) {
        timeoutSeconds = 1;
    }
    // Multiply in 64-bit and clamp so a large setting can't overflow the int
    // millisecond field (which would wrap to a tiny/negative timeout and cap
    // every enumeration immediately).
    LONGLONG timeoutMs = (LONGLONG)timeoutSeconds * 1000;
    if (timeoutMs > INT_MAX) {
        timeoutMs = INT_MAX;
    }
    g_settings.enumTimeoutMs = (int)timeoutMs;
}

////////////////////////////////////////////////////////////////////////////////
// Tool process state (everything below runs on the UI thread).

HINSTANCE g_hInst;
HANDLE g_uiThread;
DWORD g_uiThreadId;
HANDLE g_readyEvent;

ULONG_PTR g_gdiplusToken;
HWND g_chevronWnd;  // The visible expand button.
HWND g_sinkWnd;     // Hidden window: raw input + app messages (UI thread).
HWINEVENTHOOK g_foregroundHook;
// "TaskbarCreated" broadcast id; Explorer broadcasts it when the shell
// (re)starts, so the sink window re-checks the active state then (see
// SinkWndProc), recovering the desktop after an Explorer restart without
// polling - the tool process keeps running and never re-inits.
UINT g_taskbarCreatedMsg;
// "SHELLHOOK" message id; the sink window is registered with
// RegisterShellHookWindow so it receives HSHELL_* notifications. Used as an
// additional activation signal (see SinkWndProc) - the shell hook is driven by
// different machinery than EVENT_SYSTEM_FOREGROUND, so it may fire on a return
// to Explorer that emits no foreground or focus event.
UINT g_shellHookMsg;

// Background worker (its own STA) that does all UI Automation + shell work, so
// the UI thread only ever hit-tests a cached snapshot and renders the button.
HANDLE g_workerThread;
DWORD g_workerThreadId;
HANDLE g_workerReadyEvent;
// Worker thread only. Heap-allocated and intentionally never freed. The worker
// releases these on its own STA at clean shutdown (see WorkerThreadProc). If
// the UI thread hangs, WhTool_ModUninit falls back to ExitProcess, which kills
// the worker first, then runs C++ static destructors on another thread -
// letting a com_ptr destructor Release these here would marshal into the dead
// worker STA and crash. Leaking the holder avoids that; the OS reclaims it on
// exit anyway.
winrt::com_ptr<IUIAutomation>& g_workerUia =
    *new winrt::com_ptr<IUIAutomation>();
winrt::com_ptr<IUIAutomationElement>& g_workerContainer =
    *new winrt::com_ptr<IUIAutomationElement>();
HWND g_workerContainerTab;           // Worker thread only: the tab the
                                     // cached container belongs to.
PIDLIST_ABSOLUTE g_workerFolderAbs;  // Worker thread only: the folder
bool g_workerFolderIsDesktop;        // the shared children map holds.
bool g_workerChildrenValid;

// True while a File Explorer window or the desktop is the foreground window.
// Raw input is ignored otherwise, so the mod does no work for other apps.
bool g_active;

bool g_chevronVisible;
bool g_menuActive;
RECT g_hoverItemRect;
RECT g_chevronRect;
PIDLIST_ABSOLUTE g_targetPidl;

// The Explorer tab (see GetExplorerTabWindow) the hovered folder lives in, and
// whether that is the desktop. Captured when the button is shown and used as
// the "current window" when a folder is opened from the menu. UI thread only.
HWND g_targetTab;
bool g_targetIsDesktop;
// Whether the hovered view is an open/save file dialog (its menu clicks drive
// the dialog instead of Explorer). UI thread only.
bool g_targetIsDialog;

// How long (ms) a snapshot is trusted before a mouse move triggers a background
// rebuild. This is not a timer; it only gates work done on actual input events.
constexpr ULONGLONG kRefreshTtlMs = 500;

// Raw input is high frequency; coalesce moves to roughly this interval (ms).
constexpr ULONGLONG kInputCoalesceMs = 10;
ULONGLONG g_lastInputTick;
BYTE g_rawInputBuffer[1024];
bool g_rawInputRegistered;

// While the button is visible, re-check this often so navigation under a
// stationary cursor (double-click / keyboard Enter) is noticed without a move.
constexpr UINT kWatchdogIntervalMs = 500;
constexpr UINT_PTR kWatchdogTimerId = 1;

// One visible item in the active view: its rectangle (screen coords) and name.
struct CachedItem {
    RECT rect;
    std::wstring name;
};

// Snapshot shared between the worker (producer) and the UI thread (consumer),
// guarded by g_snapshotLock. The UI thread hit-tests it locally; the worker
// rebuilds it off-thread on request.
CRITICAL_SECTION g_snapshotLock;
bool g_snapValid;
HWND g_snapTab;  // The tab (see GetExplorerTabWindow) the snapshot holds.
bool g_snapIsDesktop;
std::vector<CachedItem> g_snapItems;
LONG g_snapNameColumnRight;  // Right edge of the name column, 0 if none.
ULONGLONG g_snapBuiltTick;
// Child folder display name (lowercased) -> target absolute pidl. Rebuilt only
// when the folder changes; owned here (freed on rebuild and shutdown).
std::unordered_map<std::wstring, PIDLIST_ABSOLUTE> g_snapChildren;

// Refresh request, UI thread -> worker (guarded by g_snapshotLock).
bool g_refreshRequested;
HWND g_reqTab;
bool g_reqIsDesktop;
POINT g_reqPoint;

// Non-owning observer of the live menu band, valid only while the modal loop in
// ShowFolderMenuModal is running (UI thread only). The owning reference lives
// in that function's local com_ptr.
IMenuBand* g_pActiveMenuBand;

// "CMBExecute": the menu band's own registered message for "execute item N".
// Posting it to an item toolbar (with the command id as wParam) makes the band
// open the item under the cursor exactly as a double-click does - it resolves
// the pidl itself, so we don't have to. Registered once on the UI thread.
UINT g_cmbExecuteMsg;

// When we post CMBExecute for a folder click, this holds the action to run; the
// band then fires SMC_SFEXEC back into our callback, which performs it. nothing
// when no click of ours is pending, so the band's own executes (a leaf item, a
// real double-click) fall through to its default. UI thread only.
FolderAction g_pendingExecAction;

// The folder item a left-button press landed on in the open menu (its toolbar
// and command id; a null toolbar means none). A left-click runs its action only
// when the matching release is on the same item, so the press that opened the
// menu (on the chevron) or a drag that ends elsewhere does not fire. UI thread
// only.
HWND g_leftDownToolbar;
int g_leftDownIdCmd;

std::wstring ToLower(const std::wstring& s) {
    std::wstring r = s;
    if (!r.empty()) {
        CharLowerBuffW(&r[0], (DWORD)r.size());
    }
    return r;
}

bool IsLightTheme() {
    DWORD value = 1;
    DWORD size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\"
                     L"Personalize",
                     L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &value,
                     &size) == ERROR_SUCCESS) {
        return value != 0;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Dark mode for the pop-up menu (undocumented uxtheme exports). The button is
// drawn by us (theme-aware already); the shell menu band needs the process put
// into "allow dark" mode so it follows the system light/dark setting.

enum PreferredAppMode {
    PAM_Default,
    PAM_AllowDark,
    PAM_ForceDark,
    PAM_ForceLight,
    PAM_Max,
};

PreferredAppMode(WINAPI* g_pSetPreferredAppMode)(PreferredAppMode);
void(WINAPI* g_pFlushMenuThemes)();
void(WINAPI* g_pRefreshImmersiveColorPolicyState)();
bool(WINAPI* g_pAllowDarkModeForWindow)(HWND, bool);

// Tracks the system theme (updated at startup and on theme changes); gates the
// GetSysColor overrides below so they only apply when our menu should be dark.
bool g_menuDark;

// Re-syncs the immersive color state with the current system theme. The app
// mode is set to "allow dark" once at startup; each menu window then opts in
// per-window at creation (see CreateWindowExW_Hook), so the band follows the
// system theme.
void RefreshDarkMode() {
    g_menuDark = !IsLightTheme();
    if (g_pRefreshImmersiveColorPolicyState) {
        g_pRefreshImmersiveColorPolicyState();
    }
    if (g_pFlushMenuThemes) {
        g_pFlushMenuThemes();
    }
}

void InitDarkMode() {
    HMODULE uxtheme =
        LoadLibraryExW(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!uxtheme) {
        return;
    }

    // Ordinals: 135 SetPreferredAppMode (AllowDarkModeForApp on 1809), 104
    // RefreshImmersiveColorPolicyState, 136 FlushMenuThemes, 133
    // AllowDarkModeForWindow.
    g_pSetPreferredAppMode =
        (PreferredAppMode(WINAPI*)(PreferredAppMode))GetProcAddress(
            uxtheme, MAKEINTRESOURCEA(135));
    g_pRefreshImmersiveColorPolicyState =
        (void(WINAPI*)())GetProcAddress(uxtheme, MAKEINTRESOURCEA(104));
    g_pFlushMenuThemes =
        (void(WINAPI*)())GetProcAddress(uxtheme, MAKEINTRESOURCEA(136));
    g_pAllowDarkModeForWindow = (bool(WINAPI*)(HWND, bool))GetProcAddress(
        uxtheme, MAKEINTRESOURCEA(133));

    if (g_pSetPreferredAppMode) {
        g_pSetPreferredAppMode(PAM_AllowDark);
    }
    RefreshDarkMode();
}

// The menu band paints its background and text from system colors, which
// Windows keeps light even in dark mode. We host the menu in our own process,
// so we hook GetSysColor / GetSysColorBrush to return dark equivalents while
// our dark menu is up. Other indices and every call outside the menu pass
// straight through.
struct DarkColorEntry {
    int index;
    COLORREF color;
};

constexpr DarkColorEntry kDarkColors[] = {
    {COLOR_MENU, RGB(43, 43, 43)},
    {COLOR_WINDOW, RGB(43, 43, 43)},
    {COLOR_BTNFACE, RGB(43, 43, 43)},
    {COLOR_INFOBK, RGB(43, 43, 43)},
    {COLOR_MENUTEXT, RGB(240, 240, 240)},
    {COLOR_WINDOWTEXT, RGB(240, 240, 240)},
    {COLOR_BTNTEXT, RGB(240, 240, 240)},
    {COLOR_INFOTEXT, RGB(240, 240, 240)},
    {COLOR_HIGHLIGHT, RGB(64, 64, 64)},
    {COLOR_MENUHILIGHT, RGB(64, 64, 64)},
    {COLOR_HIGHLIGHTTEXT, RGB(255, 255, 255)},
    {COLOR_GRAYTEXT, RGB(150, 150, 150)},
    {COLOR_3DSHADOW, RGB(60, 60, 60)},
    {COLOR_3DDKSHADOW, RGB(60, 60, 60)},
    {COLOR_3DHILIGHT, RGB(80, 80, 80)},
    {COLOR_3DLIGHT, RGB(80, 80, 80)},
};
HBRUSH g_darkBrushes[ARRAYSIZE(kDarkColors)];

int FindDarkColor(int index) {
    for (int i = 0; i < (int)ARRAYSIZE(kDarkColors); i++) {
        if (kDarkColors[i].index == index) {
            return i;
        }
    }
    return -1;
}

using GetSysColor_t = decltype(&GetSysColor);
GetSysColor_t GetSysColor_Orig;
DWORD WINAPI GetSysColor_Hook(int index) {
    if (g_menuActive && g_menuDark) {
        int i = FindDarkColor(index);
        if (i >= 0) {
            return kDarkColors[i].color;
        }
    }
    return GetSysColor_Orig(index);
}

using GetSysColorBrush_t = decltype(&GetSysColorBrush);
GetSysColorBrush_t GetSysColorBrush_Orig;
HBRUSH WINAPI GetSysColorBrush_Hook(int index) {
    if (g_menuActive && g_menuDark) {
        int i = FindDarkColor(index);
        if (i >= 0 && g_darkBrushes[i]) {
            return g_darkBrushes[i];
        }
    }
    return GetSysColorBrush_Orig(index);
}

// The menu band hosts each menu surface (the top-level popup and every
// cascading submenu) in a top-level window of this shell class (registered by
// CMenuDeskBar / CBaseBar). We match it so only the menu's own windows are
// rounded, not other top-level windows that merely happen to be created while
// the menu is up (item tooltips, the desktop's WorkerW, etc.).
constexpr WCHAR kMenuHostClass[] = L"BaseBar";

// While our dark menu is being built, the only windows this process creates are
// the menu band's. Dark-allowing each one at creation (before its first paint)
// makes it render dark from the first frame instead of flashing light then
// dark.
using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Orig;
HWND WINAPI CreateWindowExW_Hook(DWORD exStyle,
                                 LPCWSTR className,
                                 LPCWSTR windowName,
                                 DWORD style,
                                 int x,
                                 int y,
                                 int width,
                                 int height,
                                 HWND parent,
                                 HMENU menu,
                                 HINSTANCE instance,
                                 LPVOID param) {
    HWND hwnd =
        CreateWindowExW_Orig(exStyle, className, windowName, style, x, y, width,
                             height, parent, menu, instance, param);
    if (hwnd && g_menuActive && g_menuDark && g_pAllowDarkModeForWindow) {
        g_pAllowDarkModeForWindow(hwnd, true);
    }
    // Round the menu's own surfaces (the top-level popup and each cascading
    // submenu). Match the host class explicitly rather than rounding every
    // top-level window created while the menu is up, so other windows the band
    // spins up meanwhile - notably item tooltips - keep their normal corners.
    // DWM keeps the preference across resizes; it is a no-op on Windows 10.
    if (hwnd && g_menuActive && g_settings.roundedCorners &&
        !(style & WS_CHILD)) {
        WCHAR cls[64];
        if (GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) &&
            wcscmp(cls, kMenuHostClass) == 0) {
            DWM_WINDOW_CORNER_PREFERENCE pref = DWMWCP_ROUNDSMALL;
            DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &pref,
                                  sizeof(pref));
        }
    }
    return hwnd;
}

void InitMenuColorHooks() {
    for (int i = 0; i < (int)ARRAYSIZE(kDarkColors); i++) {
        g_darkBrushes[i] = CreateSolidBrush(kDarkColors[i].color);
    }
    WindhawkUtils::SetFunctionHook(GetSysColor, GetSysColor_Hook,
                                   &GetSysColor_Orig);
    WindhawkUtils::SetFunctionHook(GetSysColorBrush, GetSysColorBrush_Hook,
                                   &GetSysColorBrush_Orig);
    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                   &CreateWindowExW_Orig);
}

////////////////////////////////////////////////////////////////////////////////
// UI Automation (worker thread only): enumerate the visible file-list items.

// True if the element is one of the file-list containers we enumerate items
// from (a List, DataGrid or Table).
bool IsListContainer(IUIAutomationElement* element) {
    CONTROLTYPEID controlType = 0;
    element->get_CurrentControlType(&controlType);
    return controlType == UIA_ListControlTypeId ||
           controlType == UIA_DataGridControlTypeId ||
           controlType == UIA_TableControlTypeId;
}

// Binds UI Automation directly to the list window `hwnd` and returns its
// element to use as the items container. Used as a fallback when hit-testing
// (ElementFromPoint, below) does not land on the list. A transparent, topmost,
// click-through overlay - notably the NVIDIA GeForce overlay (window class
// "CEF-OSC-WIDGET"), and likewise the Steam and Discord game overlays - sits
// above the desktop and steals UIA's ElementFromPoint, which (unlike
// WindowFromPoint) ignores WS_EX_TRANSPARENT and so returns the overlay's own
// Chromium element tree instead of the file list under the cursor. Binding
// straight to a window handle does no hit-testing, so the overlay cannot
// intercept it and the real list provider is reached. The returned element need
// not be a List itself - WorkerBuildItems looks for ListItem / DataItem
// descendants under it - so the bound window element is returned as-is.
winrt::com_ptr<IUIAutomationElement> FindContainerFromWindow(HWND hwnd) {
    if (!g_workerUia || !hwnd) {
        return nullptr;
    }

    winrt::com_ptr<IUIAutomationElement> element;
    if (FAILED(g_workerUia->ElementFromHandle(hwnd, element.put())) ||
        !element) {
        return nullptr;
    }
    return element;
}

// Finds the file list / grid container for the given point. First hit-tests the
// point with ElementFromPoint and walks up to a List, DataGrid or Table
// container (the fast path, used for Explorer views). When that does not reach
// one - e.g. a transparent topmost overlay hijacked the UIA hit-test (see
// FindContainerFromWindow) - falls back to binding directly to the window under
// the point, which recovers the desktop. Returns the container, or nullptr.
winrt::com_ptr<IUIAutomationElement> FindContainerFromPoint(POINT pt) {
    if (!g_workerUia) {
        return nullptr;
    }

    winrt::com_ptr<IUIAutomationElement> element;
    if (SUCCEEDED(g_workerUia->ElementFromPoint(pt, element.put())) &&
        element) {
        winrt::com_ptr<IUIAutomationTreeWalker> walker;
        g_workerUia->get_ControlViewWalker(walker.put());

        winrt::com_ptr<IUIAutomationElement> current = element;
        for (int depth = 0; current && depth < 16; depth++) {
            if (IsListContainer(current.get())) {
                return current;
            }

            winrt::com_ptr<IUIAutomationElement> parent;
            if (!walker ||
                FAILED(walker->GetParentElement(current.get(), parent.put())) ||
                !parent) {
                break;
            }
            current = std::move(parent);
        }
    }

    // Hit-testing did not reach a list container. The click-through overlay
    // (WS_EX_TRANSPARENT) that hijacked it is ignored by WindowFromPoint, which
    // still returns the real list window; bind to that directly to bypass it.
    return FindContainerFromWindow(WindowFromPoint(pt));
}

// In a columned view (Details or List) the row's children are the cells laid
// out side by side. From the first row's cached cells, work out the right edge
// of the name column so the button can be placed next to the file name. Leaves
// *outNameColumnRight at 0 for non-columned views (icons, tiles, content).
void ComputeNameColumn(IUIAutomationElement* row,
                       const RECT& rowRect,
                       LONG* outNameColumnRight) {
    winrt::com_ptr<IUIAutomationElementArray> cells;
    if (FAILED(row->GetCachedChildren(cells.put())) || !cells) {
        return;
    }

    int count = 0;
    cells->get_Length(&count);
    if (count >= 2) {
        winrt::com_ptr<IUIAutomationElement> c0;
        winrt::com_ptr<IUIAutomationElement> c1;
        cells->GetElement(0, c0.put());
        cells->GetElement(1, c1.put());

        RECT r0;
        RECT r1;
        // The first cell must end before the row's right edge, the next cell
        // must begin at or after it (a horizontal column layout), and the first
        // cell must be wider than the row is tall, so a narrow icon-only first
        // cell is not mistaken for the name column.
        if (c0 && c1 && SUCCEEDED(c0->get_CachedBoundingRectangle(&r0)) &&
            SUCCEEDED(c1->get_CachedBoundingRectangle(&r1)) &&
            r0.right > rowRect.left && r0.right < rowRect.right &&
            (r0.right - r0.left) > (rowRect.bottom - rowRect.top) &&
            r1.left >= r0.right - 8) {
            *outNameColumnRight = r0.right;
        }
    }
}

// Enumerates the active container's visible items (rect + name) in a single UI
// Automation round trip, into the given output vector. Worker thread only.
void WorkerBuildItems(std::vector<CachedItem>& items, LONG& nameColumnRight) {
    items.clear();
    nameColumnRight = 0;

    if (!g_workerUia || !g_workerContainer) {
        return;
    }

    winrt::com_ptr<IUIAutomationCacheRequest> cache;
    if (FAILED(g_workerUia->CreateCacheRequest(cache.put())) || !cache) {
        return;
    }
    cache->AddProperty(UIA_BoundingRectanglePropertyId);
    cache->AddProperty(UIA_NamePropertyId);
    cache->put_TreeScope((TreeScope)(TreeScope_Element | TreeScope_Children));

    _variant_t vListItem((long)UIA_ListItemControlTypeId);
    _variant_t vDataItem((long)UIA_DataItemControlTypeId);

    winrt::com_ptr<IUIAutomationCondition> condList;
    winrt::com_ptr<IUIAutomationCondition> condData;
    winrt::com_ptr<IUIAutomationCondition> cond;
    g_workerUia->CreatePropertyCondition(UIA_ControlTypePropertyId, vListItem,
                                         condList.put());
    g_workerUia->CreatePropertyCondition(UIA_ControlTypePropertyId, vDataItem,
                                         condData.put());
    if (condList && condData) {
        g_workerUia->CreateOrCondition(condList.get(), condData.get(),
                                       cond.put());
    }

    if (cond) {
        // Descendants (not just children) so grouped views, where items sit
        // under group headers, are still found.
        winrt::com_ptr<IUIAutomationElementArray> rows;
        if (SUCCEEDED(g_workerContainer->FindAllBuildCache(
                TreeScope_Descendants, cond.get(), cache.get(), rows.put())) &&
            rows) {
            int length = 0;
            rows->get_Length(&length);
            bool firstDone = false;
            for (int i = 0; i < length; i++) {
                winrt::com_ptr<IUIAutomationElement> row;
                if (FAILED(rows->GetElement(i, row.put())) || !row) {
                    continue;
                }

                RECT r;
                _bstr_t name;
                if (SUCCEEDED(row->get_CachedBoundingRectangle(&r)) &&
                    SUCCEEDED(row->get_CachedName(name.GetAddress())) &&
                    name.length() > 0) {
                    items.push_back({r, std::wstring(name, name.length())});
                    if (!firstDone) {
                        ComputeNameColumn(row.get(), r, &nameColumnRight);
                        firstDone = true;
                    }
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Shell: resolve the current folder of an Explorer tab and look up a child.

// File Explorer tabs share a single CabinetWClass top-level window; each tab is
// a separate ShellTabWindowClass child of it, and the visible tab is the
// topmost child. Identify the tab a window belongs to by the top-level window's
// direct child that contains it. For a non-tabbed Explorer window (older
// builds) or the desktop there is no per-tab child, but the returned window is
// still a stable per-view identity, so the same matching works everywhere. Used
// to tell tabs apart so the hover button and its menu act on the tab actually
// under the cursor, not on whichever tab enumerates first.
HWND GetExplorerTabWindow(HWND hwnd) {
    HWND root = GetAncestor(hwnd, GA_ROOT);
    if (!root || hwnd == root) {
        return root;
    }
    HWND child = hwnd;
    HWND parent = GetParent(child);
    while (parent && parent != root) {
        child = parent;
        parent = GetParent(child);
    }
    return parent == root ? child : root;
}

// Finds the top-level IShellBrowser of the Explorer view that belongs to the
// given tab (see GetExplorerTabWindow), by matching it among all open shell
// views. Returns nullptr if no live view maps to that tab.
winrt::com_ptr<IShellBrowser> GetShellBrowserForTab(HWND tab) {
    winrt::com_ptr<IShellWindows> shellWindows;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                IID_PPV_ARGS(shellWindows.put()))) ||
        !shellWindows) {
        return nullptr;
    }

    long count = 0;
    shellWindows->get_Count(&count);

    for (long i = 0; i < count; i++) {
        _variant_t v(i);
        winrt::com_ptr<IDispatch> dispatch;
        if (FAILED(shellWindows->Item(v, dispatch.put())) || !dispatch) {
            continue;
        }

        winrt::com_ptr<IServiceProvider> serviceProvider;
        if (FAILED(dispatch->QueryInterface(
                IID_PPV_ARGS(serviceProvider.put()))) ||
            !serviceProvider) {
            continue;
        }

        winrt::com_ptr<IShellBrowser> browser;
        if (FAILED(serviceProvider->QueryService(
                SID_STopLevelBrowser, IID_PPV_ARGS(browser.put()))) ||
            !browser) {
            continue;
        }

        winrt::com_ptr<IShellView> view;
        if (FAILED(browser->QueryActiveShellView(view.put())) || !view) {
            continue;
        }

        HWND viewHwnd = nullptr;
        if (SUCCEEDED(view->GetWindow(&viewHwnd)) && viewHwnd &&
            GetExplorerTabWindow(viewHwnd) == tab) {
            return browser;
        }
    }

    return nullptr;
}

// Finds the shell view that belongs to the given Explorer tab and returns its
// current folder as an IShellFolder plus the folder's absolute pidl (the caller
// frees the pidl with ILFree; the folder is owned by the com_ptr).
bool GetFolderForExplorerTab(HWND tab,
                             winrt::com_ptr<IShellFolder>& outFolder,
                             PIDLIST_ABSOLUTE* outFolderAbs) {
    outFolder = nullptr;
    *outFolderAbs = nullptr;

    winrt::com_ptr<IShellBrowser> browser = GetShellBrowserForTab(tab);
    if (!browser) {
        return false;
    }

    winrt::com_ptr<IShellView> view;
    if (FAILED(browser->QueryActiveShellView(view.put())) || !view) {
        return false;
    }

    winrt::com_ptr<IFolderView> folderView;
    if (FAILED(view->QueryInterface(IID_PPV_ARGS(folderView.put()))) ||
        !folderView) {
        return false;
    }

    winrt::com_ptr<IPersistFolder2> persistFolder;
    if (FAILED(folderView->GetFolder(IID_PPV_ARGS(persistFolder.put()))) ||
        !persistFolder) {
        return false;
    }

    PIDLIST_ABSOLUTE folderAbs = nullptr;
    if (FAILED(persistFolder->GetCurFolder(&folderAbs)) || !folderAbs) {
        return false;
    }

    bool result = false;
    winrt::com_ptr<IShellFolder> desktop;
    if (SUCCEEDED(SHGetDesktopFolder(desktop.put())) && desktop) {
        winrt::com_ptr<IShellFolder> folder;
        if (SUCCEEDED(desktop->BindToObject(folderAbs, nullptr,
                                            IID_PPV_ARGS(folder.put()))) &&
            folder) {
            outFolder = std::move(folder);
            *outFolderAbs = folderAbs;
            folderAbs = nullptr;
            result = true;
        }
    }
    if (folderAbs) {
        ILFree(folderAbs);
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// Shell: resolve the current folder of an open/save file dialog.
//
// A common file dialog is hosted in the application's own process (not
// Explorer) and is not registered with IShellWindows, so the cross-process
// IShellBrowser path used for Explorer tabs does not reach it. Instead we read
// the current folder from the dialog's address bar (the breadcrumb
// ToolbarWindow32 inside the "Address Band Root" control) and turn it into a
// pidl with the local shell. The breadcrumb's window text is the full path for
// a file-system folder ("Address: C:\...") but only a localized display name
// for a special folder ("Address: Desktop"), so both are handled. GetWindowText
// does not cross process boundaries for a control owned by another process, so
// the text is fetched with an explicit WM_GETTEXT.

// Defined below (window-tree helpers section); used here to locate the address
// bar inside a file dialog.
HWND FindDescendantOfClass(HWND parent, PCWSTR className);

// The common item dialog top-level window class (a standard Win32 dialog).
constexpr WCHAR kFileDialogClass[] = L"#32770";

bool IsFileDialogWindow(HWND root) {
    WCHAR cls[64];
    return root && GetClassNameW(root, cls, ARRAYSIZE(cls)) &&
           wcscmp(cls, kFileDialogClass) == 0;
}

// Returns the substring of `text` starting at the first file-system path it
// contains (a drive path like "C:\..." or a UNC path "\\..."), or empty. This
// skips the address bar's localized "Address: " prefix without depending on its
// exact wording.
std::wstring ExtractFsPath(PCWSTR text) {
    for (PCWSTR p = text; *p; p++) {
        if (((p[0] >= L'A' && p[0] <= L'Z') ||
             (p[0] >= L'a' && p[0] <= L'z')) &&
            p[1] == L':' && (p[2] == L'\\' || p[2] == L'/')) {
            return p;
        }
        if (p[0] == L'\\' && p[1] == L'\\') {
            return p;
        }
    }
    return std::wstring();
}

struct FindAddressToolbarParams {
    std::wstring text;
};

BOOL CALLBACK FindAddressToolbarProc(HWND hwnd, LPARAM lParam) {
    WCHAR cls[64];
    if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) ||
        wcscmp(cls, L"ToolbarWindow32") != 0) {
        return TRUE;
    }
    // The address breadcrumb is a list-style toolbar (TBSTYLE_LIST) that
    // carries window text ("Address: <path or name>"); the command bar and the
    // other toolbars are not, so this pair of traits identifies it without
    // depending on the text content.
    if (!(GetWindowLongPtrW(hwnd, GWL_STYLE) & TBSTYLE_LIST)) {
        return TRUE;
    }
    WCHAR text[4096];
    text[0] = L'\0';
    DWORD_PTR copied = 0;
    if (SendMessageTimeoutW(hwnd, WM_GETTEXT, ARRAYSIZE(text), (LPARAM)text,
                            SMTO_ABORTIFHUNG, 1000, &copied) &&
        copied > 0) {
        reinterpret_cast<FindAddressToolbarParams*>(lParam)->text = text;
        return FALSE;  // Found the address breadcrumb; stop.
    }
    return TRUE;
}

std::wstring FindAddressToolbarText(HWND root) {
    FindAddressToolbarParams params;
    EnumChildWindows(root, FindAddressToolbarProc, (LPARAM)&params);
    return params.text;
}

// Reads the address bar's raw text (e.g. "Address: C:\Users" or "Address:
// Desktop"). The breadcrumb is found by its toolbar style and window text (see
// FindAddressToolbarProc), so a special folder, whose text is just a display
// name, is read just like a path. Scoped to the "Address Band Root" control to
// avoid any other list toolbar, falling back to the whole dialog if absent.
std::wstring GetFileDialogAddressText(HWND dlg) {
    HWND band = FindDescendantOfClass(dlg, L"Address Band Root");
    std::wstring text = band ? FindAddressToolbarText(band) : std::wstring();
    if (text.empty()) {
        text = FindAddressToolbarText(dlg);
    }
    return text;
}

// Adds {lowercased display name -> owned absolute pidl} for every immediate
// child folder of `parent` (whose absolute pidl is `parentAbs`, or nullptr for
// the desktop root) that is not already present.
void AddNamespaceChildren(
    IShellFolder* parent,
    PCIDLIST_ABSOLUTE parentAbs,
    std::unordered_map<std::wstring, PIDLIST_ABSOLUTE>& out) {
    winrt::com_ptr<IEnumIDList> enumerator;
    if (FAILED(parent->EnumObjects(
            nullptr, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, enumerator.put())) ||
        !enumerator) {
        return;
    }

    LPITEMIDLIST child = nullptr;
    ULONG fetched = 0;
    while (enumerator->Next(1, &child, &fetched) == S_OK && fetched == 1) {
        STRRET strret;
        WCHAR name[MAX_PATH];
        if (SUCCEEDED(parent->GetDisplayNameOf(child, SHGDN_NORMAL, &strret)) &&
            SUCCEEDED(StrRetToBufW(&strret, child, name, ARRAYSIZE(name))) &&
            *name) {
            std::wstring key = ToLower(name);
            if (out.find(key) == out.end()) {
                if (PIDLIST_ABSOLUTE abs = ILCombine(parentAbs, child)) {
                    out[key] = abs;
                }
            }
        }
        CoTaskMemFree(child);
        child = nullptr;
    }
}

// Builds a map of the display names the address bar shows instead of a path -
// the namespace roots (This PC, Network, Libraries, ...) and the user's known
// folders under This PC (Desktop, Documents, Downloads, drives, ...) - to their
// absolute pidls. The map owns its pidls.
void BuildSpecialFolderMap(
    std::unordered_map<std::wstring, PIDLIST_ABSOLUTE>& out) {
    winrt::com_ptr<IShellFolder> desktop;
    if (FAILED(SHGetDesktopFolder(desktop.put())) || !desktop) {
        return;
    }

    AddNamespaceChildren(desktop.get(), nullptr, out);

    PIDLIST_ABSOLUTE thisPcAbs = nullptr;
    if (SUCCEEDED(
            SHGetSpecialFolderLocation(nullptr, CSIDL_DRIVES, &thisPcAbs)) &&
        thisPcAbs) {
        winrt::com_ptr<IShellFolder> thisPc;
        if (SUCCEEDED(desktop->BindToObject(thisPcAbs, nullptr,
                                            IID_PPV_ARGS(thisPc.put()))) &&
            thisPc) {
            AddNamespaceChildren(thisPc.get(), thisPcAbs, out);
        }
        ILFree(thisPcAbs);
    }
}

// Resolves the address bar text of a special folder (a display name, not a
// path) to its absolute pidl, or nullptr. Matches the longest known display
// name that the text ends with, at a word boundary - this both ignores the
// localized "Address: " prefix and works in any UI language, since the names
// are read from the same shell. Caller frees the pidl.
PIDLIST_ABSOLUTE ResolveSpecialFolderPidl(PCWSTR addressText) {
    std::unordered_map<std::wstring, PIDLIST_ABSOLUTE> map;
    BuildSpecialFolderMap(map);

    std::wstring text = ToLower(addressText);
    PIDLIST_ABSOLUTE best = nullptr;
    size_t bestLen = 0;
    for (const auto& entry : map) {
        const std::wstring& name = entry.first;
        if (name.size() <= bestLen || name.size() > text.size()) {
            continue;
        }
        size_t at = text.size() - name.size();
        if (text.compare(at, name.size(), name) != 0) {
            continue;
        }
        // Require a separator (not a letter or digit) before the match, so
        // "...: Desktop" matches but a folder ending in "...Desktop" does not.
        WCHAR before = at > 0 ? text[at - 1] : L' ';
        if ((before >= L'a' && before <= L'z') ||
            (before >= L'0' && before <= L'9')) {
            continue;
        }
        best = entry.second;
        bestLen = name.size();
    }

    PIDLIST_ABSOLUTE result = best ? ILClone(best) : nullptr;
    for (const auto& entry : map) {
        ILFree(entry.second);
    }
    return result;
}

// Resolves the current folder of the file dialog `tab` belongs to (read from
// its address bar) into an IShellFolder plus its absolute pidl. Mirrors
// GetFolderForExplorerTab's output contract (the caller frees the pidl).
bool GetFolderForFileDialog(HWND tab,
                            winrt::com_ptr<IShellFolder>& outFolder,
                            PIDLIST_ABSOLUTE* outFolderAbs) {
    outFolder = nullptr;
    *outFolderAbs = nullptr;

    HWND dlg = GetAncestor(tab, GA_ROOT);
    if (!dlg) {
        return false;
    }

    std::wstring address = GetFileDialogAddressText(dlg);
    if (address.empty()) {
        return false;
    }

    // A file-system folder shows its full path; a special folder shows only a
    // display name, which we map back to a pidl through the namespace.
    PIDLIST_ABSOLUTE folderAbs = nullptr;
    std::wstring fsPath = ExtractFsPath(address.c_str());
    if (!fsPath.empty()) {
        if (FAILED(SHParseDisplayName(fsPath.c_str(), nullptr, &folderAbs, 0,
                                      nullptr))) {
            folderAbs = nullptr;
        }
    } else {
        folderAbs = ResolveSpecialFolderPidl(address.c_str());
    }
    if (!folderAbs) {
        Wh_Log(
            L"GetFolderForFileDialog: could not resolve address to a folder");
        return false;
    }

    bool result = false;
    winrt::com_ptr<IShellFolder> desktop;
    if (SUCCEEDED(SHGetDesktopFolder(desktop.put())) && desktop) {
        winrt::com_ptr<IShellFolder> folder;
        if (SUCCEEDED(desktop->BindToObject(folderAbs, nullptr,
                                            IID_PPV_ARGS(folder.put()))) &&
            folder) {
            outFolder = std::move(folder);
            *outFolderAbs = folderAbs;
            folderAbs = nullptr;
            result = true;
        }
    }
    if (folderAbs) {
        ILFree(folderAbs);
    }

    return result;
}

bool IsFolderPidl(PCIDLIST_ABSOLUTE pidl) {
    winrt::com_ptr<IShellItem> item;
    bool isFolder = false;
    if (SUCCEEDED(SHCreateItemFromIDList(pidl, IID_PPV_ARGS(item.put()))) &&
        item) {
        SFGAOF attrs = 0;
        if (SUCCEEDED(item->GetAttributes(SFGAO_FOLDER, &attrs)) &&
            (attrs & SFGAO_FOLDER)) {
            isFolder = true;
        }
    }
    return isFolder;
}

// If `child` (a shortcut item in `folder`) points to a folder, returns the
// target's absolute pidl (caller frees); otherwise nullptr. Only stored target
// data is read (no link resolution / network access), so it cannot stall.
PIDLIST_ABSOLUTE ResolveFolderShortcut(IShellFolder* folder,
                                       LPCITEMIDLIST child) {
    winrt::com_ptr<IShellLinkW> link;
    if (FAILED(folder->GetUIObjectOf(nullptr, 1, &child, IID_IShellLinkW,
                                     nullptr, link.put_void())) ||
        !link) {
        return nullptr;
    }

    PIDLIST_ABSOLUTE result = nullptr;
    PIDLIST_ABSOLUTE target = nullptr;
    if (SUCCEEDED(link->GetIDList(&target)) && target) {
        if (IsFolderPidl(target)) {
            result = target;
            target = nullptr;
        }
    }
    if (!result) {
        // Fall back to the link's raw stored path.
        WCHAR path[MAX_PATH];
        if (SUCCEEDED(
                link->GetPath(path, ARRAYSIZE(path), nullptr, SLGP_RAWPATH)) &&
            *path) {
            PIDLIST_ABSOLUTE pathPidl = nullptr;
            if (SUCCEEDED(
                    SHParseDisplayName(path, nullptr, &pathPidl, 0, nullptr)) &&
                pathPidl) {
                if (IsFolderPidl(pathPidl)) {
                    result = pathPidl;
                } else {
                    ILFree(pathPidl);
                }
            }
        }
    }
    if (target) {
        ILFree(target);
    }
    return result;
}

// Enumerates the folder's children into a name -> target-pidl map. Real
// sub-folders map to their own pidl; shortcuts to a folder map to the target's.
// The map owns its pidls. Worker thread only.
void WorkerBuildChildren(
    IShellFolder* folder,
    PCIDLIST_ABSOLUTE folderAbs,
    bool isDesktop,
    std::unordered_map<std::wstring, PIDLIST_ABSOLUTE>& out) {
    // Non-folders are enumerated too, so shortcuts (.lnk files) that point to a
    // folder can be picked up.
    winrt::com_ptr<IEnumIDList> enumerator;
    if (FAILED(folder->EnumObjects(
            nullptr,
            SHCONTF_FOLDERS | SHCONTF_NONFOLDERS | SHCONTF_INCLUDEHIDDEN,
            enumerator.put())) ||
        !enumerator) {
        return;
    }

    LPITEMIDLIST child = nullptr;
    ULONG fetched = 0;
    while (enumerator->Next(1, &child, &fetched) == S_OK && fetched == 1) {
        LPCITEMIDLIST childConst = child;
        SFGAOF attrs = SFGAO_FOLDER | SFGAO_LINK;
        if (FAILED(folder->GetAttributesOf(1, &childConst, &attrs))) {
            attrs = 0;
        }

        PIDLIST_ABSOLUTE childAbs = nullptr;
        if (attrs & SFGAO_FOLDER) {
            // For the desktop, folderAbs is the root, so the child relative
            // pidl already is the absolute pidl. ILCombine(nullptr, child)
            // clones it.
            childAbs = ILCombine(isDesktop ? nullptr : folderAbs, child);
        } else if (attrs & SFGAO_LINK) {
            childAbs = ResolveFolderShortcut(folder, child);
        }

        if (childAbs) {
            std::wstring keys[2];
            int keyCount = 0;

            STRRET strret;
            WCHAR name[MAX_PATH];
            if (SUCCEEDED(
                    folder->GetDisplayNameOf(child, SHGDN_NORMAL, &strret)) &&
                SUCCEEDED(
                    StrRetToBufW(&strret, child, name, ARRAYSIZE(name))) &&
                *name) {
                keys[keyCount++] = ToLower(name);
            }
            // Shortcuts can be shown with their .lnk extension, so also key by
            // the in-folder parsing name to match the UIA item name in that
            // case.
            if ((attrs & SFGAO_LINK) &&
                SUCCEEDED(folder->GetDisplayNameOf(
                    child, SHGDN_INFOLDER | SHGDN_FORPARSING, &strret)) &&
                SUCCEEDED(
                    StrRetToBufW(&strret, child, name, ARRAYSIZE(name))) &&
                *name) {
                std::wstring key = ToLower(name);
                if (keyCount == 0 || key != keys[0]) {
                    keys[keyCount++] = key;
                }
            }

            bool stored = false;
            for (int k = 0; k < keyCount; k++) {
                if (out.find(keys[k]) != out.end()) {
                    continue;
                }
                PIDLIST_ABSOLUTE p = stored ? ILClone(childAbs) : childAbs;
                if (!p) {
                    continue;
                }
                out[keys[k]] = p;
                stored = true;
            }
            if (!stored) {
                ILFree(childAbs);
            }
        }

        CoTaskMemFree(child);
        child = nullptr;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Worker thread: produces snapshots off the UI thread.

bool EnsureWorkerUia() {
    if (g_workerUia) {
        return true;
    }
    HRESULT hr =
        CoCreateInstance(CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER,
                         IID_PPV_ARGS(g_workerUia.put()));
    if (FAILED(hr) || !g_workerUia) {
        g_workerUia = nullptr;
        Wh_Log(L"worker CoCreateInstance(CUIAutomation) failed, hr=0x%08X", hr);
        return false;
    }
    return true;
}

// Builds a fresh snapshot for the given window/point and installs it for the UI
// thread to hit-test. The expensive UIA + shell work happens here, off the UI
// thread; only the brief install is done under the lock.
void WorkerBuildSnapshot(HWND tab, bool isDesktop, POINT pt) {
    ULONGLONG tick = GetTickCount64();

    winrt::com_ptr<IShellFolder> folder;
    PIDLIST_ABSOLUTE folderAbs = nullptr;
    if (isDesktop) {
        SHGetDesktopFolder(folder.put());
    } else if (IsFileDialogWindow(GetAncestor(tab, GA_ROOT))) {
        GetFolderForFileDialog(tab, folder, &folderAbs);
    } else {
        GetFolderForExplorerTab(tab, folder, &folderAbs);
    }

    if (!folder) {
        // Install an empty snapshot so the UI throttles instead of
        // re-requesting on every move.
        EnterCriticalSection(&g_snapshotLock);
        g_snapItems.clear();
        g_snapNameColumnRight = 0;
        g_snapTab = tab;
        g_snapIsDesktop = isDesktop;
        g_snapBuiltTick = tick;
        g_snapValid = true;
        LeaveCriticalSection(&g_snapshotLock);
        return;
    }

    bool folderChanged =
        isDesktop ? (!g_workerChildrenValid || !g_workerFolderIsDesktop)
                  : (!g_workerChildrenValid || g_workerFolderIsDesktop ||
                     !g_workerFolderAbs || !folderAbs ||
                     !ILIsEqual(g_workerFolderAbs, folderAbs));

    // (Re)acquire the list element when the tab changed.
    if (tab != g_workerContainerTab || !g_workerContainer) {
        g_workerContainer = FindContainerFromPoint(pt);
        g_workerContainerTab = g_workerContainer ? tab : nullptr;
    }

    std::vector<CachedItem> items;
    LONG nameColumnRight = 0;
    WorkerBuildItems(items, nameColumnRight);

    // An empty rebuild over a populated view means the element went stale
    // (navigation, re-sort, view-mode change): re-acquire it and try once more.
    if (items.empty()) {
        auto container = FindContainerFromPoint(pt);
        if (container) {
            g_workerContainer = std::move(container);
            g_workerContainerTab = tab;
            WorkerBuildItems(items, nameColumnRight);
        }
    }

    std::unordered_map<std::wstring, PIDLIST_ABSOLUTE> children;
    bool rebuiltChildren = false;
    if (folderChanged) {
        WorkerBuildChildren(folder.get(), folderAbs, isDesktop, children);
        rebuiltChildren = true;
    }

    // Swap the new data in under the lock (O(1)); the old data ends up in the
    // local temporaries and is destroyed/freed below, outside the lock.
    EnterCriticalSection(&g_snapshotLock);
    g_snapItems.swap(items);
    g_snapNameColumnRight = nameColumnRight;
    g_snapTab = tab;
    g_snapIsDesktop = isDesktop;
    g_snapBuiltTick = tick;
    g_snapValid = true;
    if (rebuiltChildren) {
        g_snapChildren.swap(children);
    }
    LeaveCriticalSection(&g_snapshotLock);

    if (rebuiltChildren) {
        for (auto& entry : children) {  // The previous folder's child pidls.
            ILFree(entry.second);
        }
        if (g_workerFolderAbs) {
            ILFree(g_workerFolderAbs);
            g_workerFolderAbs = nullptr;
        }
        g_workerFolderAbs =
            (folderAbs && !isDesktop) ? ILClone(folderAbs) : nullptr;
        g_workerFolderIsDesktop = isDesktop;
        g_workerChildrenValid = true;
    }

    if (folderAbs) {
        ILFree(folderAbs);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Opening a clicked folder. Runs on the worker thread (off the UI thread)
// because opening a new tab waits for Explorer to spin one up.

// Internal File Explorer WM_COMMAND id for "new tab" (the Ctrl+T accelerator),
// posted to a tab window to open a new tab. This is the same value File
// Explorer itself uses; it is undocumented and could change in a future Windows
// build, in which case the new-tab action falls back to a new window.
constexpr WPARAM kExplorerNewTabCommand = 0xA21B;

// A pending open request, handed from the UI thread to the worker via
// WM_APP_DO_ACTION. The worker owns it and frees pidl + the struct.
struct FolderActionRequest {
    PIDLIST_ABSOLUTE pidl;  // The folder (or, for selectFile, the file) pidl.
    FolderAction action;
    // File dialog only: instead of opening a folder, put the file `pidl` into
    // the dialog's file name box. `action` is ignored when this is set.
    bool selectFile;
    HWND sourceTab;  // The view the menu was opened from (current window).
    bool sourceIsDesktop;  // If true there is no current window/tab to use.
};

// Opens the folder in a new Explorer window. When the menu was opened from an
// Explorer view, spin the new window up through that view's existing
// IShellBrowser (BrowseObject with SBSP_NEWBROWSER) - the same path the other
// actions use, and more direct than ShellExecuteEx, which would resolve and
// invoke the folder's default verb. Falls back to ShellExecuteEx when there is
// no live source browser (opened from the desktop, or the view went away).
void OpenInNewWindow(HWND tab, PCIDLIST_ABSOLUTE pidl) {
    if (tab) {
        winrt::com_ptr<IShellBrowser> browser = GetShellBrowserForTab(tab);
        if (browser &&
            SUCCEEDED(browser->BrowseObject((PCUIDLIST_RELATIVE)pidl,
                                            SBSP_NEWBROWSER | SBSP_ABSOLUTE))) {
            return;
        }
    }

    SHELLEXECUTEINFOW sei = {sizeof(sei)};
    sei.fMask = SEE_MASK_IDLIST | SEE_MASK_FLAG_NO_UI;
    sei.lpIDList = const_cast<void*>(static_cast<const void*>(pidl));
    sei.lpVerb = L"open";
    sei.nShow = SW_SHOWNORMAL;
    if (!ShellExecuteExW(&sei)) {
        Wh_Log(L"OpenInNewWindow: ShellExecuteEx failed, le=%lu",
               GetLastError());
    }
}

// Navigates the existing view of `tab` to the folder (the "current window").
// Returns false if that tab no longer has a live browser.
bool OpenInCurrentWindow(HWND tab, PCIDLIST_ABSOLUTE pidl) {
    winrt::com_ptr<IShellBrowser> browser = GetShellBrowserForTab(tab);
    if (!browser) {
        return false;
    }
    return SUCCEEDED(browser->BrowseObject((PCUIDLIST_RELATIVE)pidl,
                                           SBSP_SAMEBROWSER | SBSP_ABSOLUTE));
}

// Opens the folder in a new tab of the window `tab` belongs to. Asks Explorer
// to open a new (blank) tab via its own new-tab command, waits for that tab to
// appear, then navigates it. Returns false if the window has no tabs (older
// Explorer) or the new tab never showed up, so the caller can fall back.
bool OpenInNewTab(HWND tab, PCIDLIST_ABSOLUTE pidl) {
    HWND frame = GetAncestor(tab, GA_ROOT);
    if (!frame) {
        return false;
    }

    // The active tab is the topmost ShellTabWindowClass child; post the command
    // there. No tab child means this is not a tabbed Explorer build.
    HWND prevActive =
        FindWindowExW(frame, nullptr, L"ShellTabWindowClass", nullptr);
    if (!prevActive) {
        return false;
    }

    PostMessageW(prevActive, WM_COMMAND, kExplorerNewTabCommand, 0);

    // The new tab becomes the active (topmost) one; wait for it to differ from
    // the previous active tab.
    HWND newTab = nullptr;
    for (int i = 0; i < 100 && !newTab; i++) {
        Sleep(20);
        HWND active =
            FindWindowExW(frame, nullptr, L"ShellTabWindowClass", nullptr);
        if (active && active != prevActive) {
            newTab = active;
        }
    }
    if (!newTab) {
        return false;
    }

    // Its browser registers a moment after the tab window appears.
    winrt::com_ptr<IShellBrowser> browser;
    for (int i = 0; i < 100 && !browser; i++) {
        browser = GetShellBrowserForTab(newTab);
        if (!browser) {
            Sleep(20);
        }
    }
    if (!browser) {
        return false;
    }

    return SUCCEEDED(browser->BrowseObject((PCUIDLIST_RELATIVE)pidl,
                                           SBSP_SAMEBROWSER | SBSP_ABSOLUTE));
}

// The file-system path of an absolute pidl, or empty for a non-file-system
// item (which a file dialog cannot navigate to or select anyway).
std::wstring PathFromPidl(PCIDLIST_ABSOLUTE pidl) {
    std::wstring out;
    PWSTR str = nullptr;
    if (SUCCEEDED(SHGetNameFromIDList(pidl, SIGDN_FILESYSPATH, &str)) && str) {
        out = str;
    }
    if (str) {
        CoTaskMemFree(str);
    }
    return out;
}

struct FindCtrlByIdParams {
    int id;
    HWND result;
};

BOOL CALLBACK FindCtrlByIdProc(HWND hwnd, LPARAM lParam) {
    auto* params = reinterpret_cast<FindCtrlByIdParams*>(lParam);
    if (GetDlgCtrlID(hwnd) == params->id) {
        params->result = hwnd;
        return FALSE;
    }
    return TRUE;
}

HWND FindDescendantById(HWND parent, int id) {
    FindCtrlByIdParams params = {id, nullptr};
    EnumChildWindows(parent, FindCtrlByIdProc, (LPARAM)&params);
    return params.result;
}

bool IsDescendantOf(HWND child, HWND ancestor) {
    for (HWND h = child; h; h = GetParent(h)) {
        if (h == ancestor) {
            return true;
        }
    }
    return false;
}

// Diagnostic: logs every child control of the dialog (id, class, style,
// visibility, rect, text) so the file name field can be identified in dialogs
// that do not use the classic control ids.
BOOL CALLBACK DumpControlsProc(HWND hwnd, LPARAM lParam) {
    WCHAR cls[64] = L"";
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    WCHAR text[96];
    text[0] = L'\0';
    DWORD_PTR copied = 0;
    SendMessageTimeoutW(hwnd, WM_GETTEXT, ARRAYSIZE(text), (LPARAM)text,
                        SMTO_ABORTIFHUNG, 200, &copied);
    LONG style = (LONG)GetWindowLongPtrW(hwnd, GWL_STYLE);
    RECT rc = {};
    GetWindowRect(hwnd, &rc);
    Wh_Log(
        L"  hwnd=%p id=0x%04X class=%s style=0x%08X vis=%d rc=(%ld,%ld) "
        L"text=\"%s\"",
        hwnd, GetDlgCtrlID(hwnd), cls, (DWORD)style,
        (style & WS_VISIBLE) ? 1 : 0, rc.left, rc.top, text);
    return TRUE;
}

void DumpDialogControls(HWND dlg) {
    Wh_Log(L"--- file dialog controls, dlg=%p ---", dlg);
    EnumChildWindows(dlg, DumpControlsProc, 0);
    Wh_Log(L"--- end file dialog controls ---");
}

struct FindFilenameComboParams {
    HWND addressBand;
    bool haveButtonRow;
    LONG buttonCenterY;
    HWND best;
    LONG bestScore;
};

BOOL CALLBACK FindFilenameComboProc(HWND hwnd, LPARAM lParam) {
    auto* params = reinterpret_cast<FindFilenameComboParams*>(lParam);
    WCHAR cls[64];
    if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) ||
        wcscmp(cls, L"ComboBox") != 0 || !IsWindowVisible(hwnd)) {
        return TRUE;
    }
    // The file name field is an editable combo (CBS_SIMPLE or CBS_DROPDOWN);
    // the file-type combo is a non-editable CBS_DROPDOWNLIST, and the address
    // bar (a ComboBoxEx32 inside the address band) is excluded explicitly.
    LONG type = (LONG)GetWindowLongPtrW(hwnd, GWL_STYLE) & 0x3;
    if (type != CBS_SIMPLE && type != CBS_DROPDOWN) {
        return TRUE;
    }
    if (params->addressBand && IsDescendantOf(hwnd, params->addressBand)) {
        return TRUE;
    }
    RECT rc;
    if (!GetWindowRect(hwnd, &rc)) {
        return TRUE;
    }
    // The file name combo shares the default button's row, so when several
    // editable combos exist (an app can add its own) prefer the one closest to
    // that row; otherwise prefer the lowest one.
    LONG centerY = (rc.top + rc.bottom) / 2;
    LONG score;
    if (params->haveButtonRow) {
        LONG dy = centerY - params->buttonCenterY;
        score = dy < 0 ? -dy : dy;
    } else {
        score = -rc.top;
    }
    if (!params->best || score < params->bestScore) {
        params->best = hwnd;
        params->bestScore = score;
    }
    return TRUE;
}

// Finds the file dialog's file name field (the combo/edit where a name or path
// is typed). The classic common dialog uses control ids cmb13 (editable combo)
// and edt1 (plain edit); the Vista+ common item dialog (used by mspaint and
// most modern apps) gives it no usable id, so we fall back to the editable
// combo, not in the address bar, nearest the default button's row. Searched
// among all descendants, not just direct children.
HWND FindDialogFilenameField(HWND dlg) {
    constexpr int kIdFilenameCombo = 0x047C;  // cmb13
    constexpr int kIdFilenameEdit = 0x0480;   // edt1
    if (HWND h = FindDescendantById(dlg, kIdFilenameCombo)) {
        return h;
    }
    if (HWND h = FindDescendantById(dlg, kIdFilenameEdit)) {
        return h;
    }

    HWND ok = GetDlgItem(dlg, IDOK);
    RECT okRc = {};
    bool haveButtonRow = ok && GetWindowRect(ok, &okRc);
    FindFilenameComboParams params = {
        FindDescendantOfClass(dlg, L"Address Band Root"),
        haveButtonRow,
        haveButtonRow ? (okRc.top + okRc.bottom) / 2 : 0,
        nullptr,
        0,
    };
    EnumChildWindows(dlg, FindFilenameComboProc, (LPARAM)&params);
    return params.best;
}

// Drives a common file dialog by putting `path` into its file name field. When
// `submit` is set it then presses the default button, which makes the dialog
// navigate into a folder path (it does not close on a folder). Without submit
// the path is only selected, so the user still confirms the dialog. All the
// window messages used cross process boundaries. Returns false if the field
// could not be found.
bool DriveFileDialog(HWND dlg, PCWSTR path, bool submit) {
    HWND field = FindDialogFilenameField(dlg);
    if (!field) {
        Wh_Log(L"DriveFileDialog: file name field not found");
        DumpDialogControls(dlg);
        return false;
    }

    DWORD_PTR result = 0;
    SendMessageTimeoutW(field, WM_SETTEXT, 0, (LPARAM)path, SMTO_ABORTIFHUNG,
                        5000, &result);

    if (submit) {
        HWND ok = GetDlgItem(dlg, IDOK);
        SendMessageTimeoutW(dlg, WM_COMMAND, MAKEWPARAM(IDOK, BN_CLICKED),
                            (LPARAM)ok, SMTO_ABORTIFHUNG, 5000, &result);
    }
    return true;
}

// Carries out one open request. The current-window and new-tab actions need a
// source view; without one (the menu was opened from the desktop, or the view
// went away) they fall back to opening a new window. When the source is a file
// dialog, "current window" navigates the dialog itself and a file selection
// fills its file name box; the new-window / new-tab actions still open
// Explorer.
void PerformFolderAction(const FolderActionRequest& req) {
    HWND dlg = req.sourceTab ? GetAncestor(req.sourceTab, GA_ROOT) : nullptr;
    bool isDialog = IsFileDialogWindow(dlg);

    if (isDialog &&
        (req.selectFile || req.action == FolderAction::currentWindow)) {
        // Let the dialog reclaim the foreground from our (closing) menu.
        DWORD dialogPid = 0;
        GetWindowThreadProcessId(dlg, &dialogPid);
        AllowSetForegroundWindow(dialogPid ? dialogPid : ASFW_ANY);

        std::wstring path = PathFromPidl(req.pidl);
        if (!path.empty()) {
            // A folder (current window) is submitted to navigate into it; a
            // file selection only fills the box, leaving the user to confirm.
            DriveFileDialog(dlg, path.c_str(), !req.selectFile);
        }
        return;
    }

    if (req.selectFile) {
        return;  // Only meaningful for a file dialog source.
    }

    bool haveSource =
        !req.sourceIsDesktop && req.sourceTab && IsWindow(req.sourceTab);
    // The source tab to open a new window from, or null to fall back to
    // ShellExecuteEx (no live source view).
    HWND sourceTab = haveSource ? req.sourceTab : nullptr;

    // Our menu process held the foreground, so the Explorer window the open
    // activates would be blocked from taking focus and flash in the taskbar
    // instead. Grant Explorer the right to set the foreground. We are still
    // eligible to do so because our window received the click that started
    // this. From a file dialog the opened Explorer is a different process, so
    // allow any.
    DWORD explorerPid = 0;
    if (!isDialog && req.sourceTab) {
        GetWindowThreadProcessId(req.sourceTab, &explorerPid);
    }
    AllowSetForegroundWindow(explorerPid ? explorerPid : ASFW_ANY);

    switch (req.action) {
        case FolderAction::currentWindow:
            if (haveSource && OpenInCurrentWindow(req.sourceTab, req.pidl)) {
                return;
            }
            OpenInNewWindow(sourceTab, req.pidl);
            return;

        case FolderAction::newTab:
            if (haveSource && OpenInNewTab(req.sourceTab, req.pidl)) {
                return;
            }
            OpenInNewWindow(sourceTab, req.pidl);
            return;

        case FolderAction::newWindow:
        case FolderAction::nothing:  // Never posted, but keep the open safe.
            OpenInNewWindow(sourceTab, req.pidl);
            return;
    }
}

DWORD WINAPI WorkerThreadProc(LPVOID param) {
    // Match the UI thread's physical-pixel coordinate space.
    if (HMODULE user32 = GetModuleHandleW(L"user32.dll")) {
        auto pSetThreadDpiAwarenessContext =
            (void*(WINAPI*)(void*))GetProcAddress(
                user32, "SetThreadDpiAwarenessContext");
        if (pSetThreadDpiAwarenessContext) {
            pSetThreadDpiAwarenessContext((void*)(LONG_PTR)-4);
        }
    }

    OleInitialize(nullptr);
    EnsureWorkerUia();

    // Force the thread message queue to exist before signaling ready, so the UI
    // thread's PostThreadMessage requests are never lost.
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    SetEvent(g_workerReadyEvent);

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.hwnd == nullptr && msg.message == WM_APP_QUIT) {
            PostQuitMessage(0);
            continue;
        }
        if (msg.hwnd == nullptr && msg.message == WM_APP_DO_REFRESH) {
            HWND tab = nullptr;
            bool isDesktop = false;
            POINT pt = {};
            bool have = false;
            EnterCriticalSection(&g_snapshotLock);
            if (g_refreshRequested) {
                tab = g_reqTab;
                isDesktop = g_reqIsDesktop;
                pt = g_reqPoint;
                g_refreshRequested = false;
                have = true;
            }
            LeaveCriticalSection(&g_snapshotLock);
            if (have) {
                WorkerBuildSnapshot(tab, isDesktop, pt);
                if (g_sinkWnd) {
                    PostMessageW(g_sinkWnd, WM_APP_REFRESH_DONE, 0, 0);
                }
            }
            continue;
        }
        if (msg.hwnd == nullptr && msg.message == WM_APP_DO_ACTION) {
            auto* req = reinterpret_cast<FolderActionRequest*>(msg.wParam);
            if (req) {
                PerformFolderAction(*req);
                if (req->pidl) {
                    ILFree(req->pidl);
                }
                delete req;
            }
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    g_workerContainer = nullptr;
    g_workerUia = nullptr;
    if (g_workerFolderAbs) {
        ILFree(g_workerFolderAbs);
        g_workerFolderAbs = nullptr;
    }
    OleUninitialize();
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Enumeration bound. A folder with a very large number of items makes the menu
// band take a long time to open: the enumeration is fast, but the band builds
// one menu entry per item, so a few thousand items cost several seconds. We
// bound that by wrapping the folder's enumerator (see the EnumObjects hook
// below): it returns at most g_settings.maxEnumItems real items and then a
// single synthetic notice entry, after which the menu opens promptly with a
// truncated list. A time budget (g_settings.enumTimeoutMs) is also enforced as
// a safety net for folders where the enumeration itself is slow. Both limits
// are exposed in the "Timeout" settings group.

// Magic stored in the synthetic child pidl so we can recognize it later.
constexpr DWORD kTimeoutPidlMagic = 0x544D4F45;  // 'EOMT'.

// Builds the synthetic one-item pidl the bounded enumerator returns when it
// stops early. Caller frees it with CoTaskMemFree / ILFree (the menu band does
// so for us).
LPITEMIDLIST CreateTimeoutPidl() {
    // A single SHITEMID carrying our magic, followed by the null terminator.
    const SIZE_T size = sizeof(USHORT) + sizeof(DWORD) + sizeof(USHORT);
    BYTE* p = (BYTE*)CoTaskMemAlloc(size);
    if (!p) {
        return nullptr;
    }
    USHORT cb = (USHORT)(sizeof(USHORT) + sizeof(DWORD));
    memcpy(p, &cb, sizeof(cb));
    memcpy(p + sizeof(USHORT), &kTimeoutPidlMagic, sizeof(kTimeoutPidlMagic));
    USHORT terminator = 0;
    memcpy(p + sizeof(USHORT) + sizeof(DWORD), &terminator, sizeof(terminator));
    return (LPITEMIDLIST)p;
}

bool IsTimeoutPidl(LPCITEMIDLIST pidl) {
    if (!pidl || pidl->mkid.cb != sizeof(USHORT) + sizeof(DWORD)) {
        return false;
    }
    DWORD magic;
    memcpy(&magic, pidl->mkid.abID, sizeof(magic));
    if (magic != kTimeoutPidlMagic) {
        return false;
    }
    LPCITEMIDLIST next = (LPCITEMIDLIST)((const BYTE*)pidl + pidl->mkid.cb);
    return next->mkid.cb == 0;
}

// Wraps the folder's real enumerator and bounds it. While inside the item-count
// and time budgets it forwards to the inner enumerator; once either is exceeded
// it returns one synthetic notice item and then reports end-of-enumeration, so
// the menu band stops asking for more (and ignores the remaining items).
class CTimeoutEnumIDList final : public IEnumIDList {
   public:
    CTimeoutEnumIDList(IEnumIDList* inner) : m_inner(inner) {
        m_inner->AddRef();
        m_deadline = GetTickCount64() + g_settings.enumTimeoutMs;
    }

    // IUnknown.
    IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) {
            return E_POINTER;
        }
        if (IsEqualIID(riid, IID_IUnknown) ||
            IsEqualIID(riid, IID_IEnumIDList)) {
            *ppv = static_cast<IEnumIDList*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }

    IFACEMETHODIMP_(ULONG) AddRef() override {
        return InterlockedIncrement(&m_ref);
    }

    IFACEMETHODIMP_(ULONG) Release() override {
        LONG ref = InterlockedDecrement(&m_ref);
        if (ref == 0) {
            delete this;
        }
        return ref;
    }

    // IEnumIDList.
    IFACEMETHODIMP Next(ULONG celt,
                        LPITEMIDLIST* rgelt,
                        ULONG* pceltFetched) override {
        if (pceltFetched) {
            *pceltFetched = 0;
        }
        if (celt == 0) {
            return S_OK;
        }
        if (!rgelt) {
            return E_INVALIDARG;
        }

        // Stop and emit the notice once either budget is exceeded: the item
        // count (the band's cost is dominated by building one entry per item,
        // so a huge folder must be truncated) or, as a safety net, the time
        // budget (in case the enumeration itself is slow). Checked here,
        // between items.
        if (m_count >= (UINT)g_settings.maxEnumItems ||
            GetTickCount64() >= m_deadline) {
            if (m_timedOutEmitted) {
                return S_FALSE;  // Notice already returned; nothing more.
            }
            LPITEMIDLIST pidl = CreateTimeoutPidl();
            if (!pidl) {
                return E_OUTOFMEMORY;
            }
            rgelt[0] = pidl;
            m_timedOutEmitted = true;
            if (pceltFetched) {
                *pceltFetched = 1;
            }
            Wh_Log(
                L"Enumeration capped after %u items (timedOut=%d), emitting "
                L"notice",
                m_count, (int)(GetTickCount64() >= m_deadline));
            return celt == 1 ? S_OK : S_FALSE;
        }

        HRESULT hr = m_inner->Next(celt, rgelt, pceltFetched);
        m_count += pceltFetched ? *pceltFetched : (hr == S_OK ? celt : 0);
        return hr;
    }

    IFACEMETHODIMP Skip(ULONG celt) override { return m_inner->Skip(celt); }

    IFACEMETHODIMP Reset() override {
        m_timedOutEmitted = false;
        m_count = 0;
        m_deadline = GetTickCount64() + g_settings.enumTimeoutMs;
        return m_inner->Reset();
    }

    IFACEMETHODIMP Clone(IEnumIDList** ppenum) override {
        if (!ppenum) {
            return E_POINTER;
        }
        *ppenum = nullptr;
        winrt::com_ptr<IEnumIDList> innerClone;
        HRESULT hr = m_inner->Clone(innerClone.put());
        if (FAILED(hr)) {
            return hr;
        }
        auto* clone = new (std::nothrow) CTimeoutEnumIDList(innerClone.get());
        if (!clone) {
            return E_OUTOFMEMORY;
        }
        clone->m_deadline = m_deadline;
        clone->m_timedOutEmitted = m_timedOutEmitted;
        clone->m_count = m_count;
        *ppenum = clone;
        return S_OK;
    }

   private:
    ~CTimeoutEnumIDList() { m_inner->Release(); }

    LONG m_ref = 1;
    IEnumIDList* m_inner;
    ULONGLONG m_deadline;
    bool m_timedOutEmitted = false;
    UINT m_count = 0;  // Real items returned so far (for the item-count cap).
};

// The menu band does not enumerate through the IShellFolder we hand it (it uses
// the real folder it binds from the pidl), so wrapping that folder has no
// effect. Instead we inline-hook the IShellFolder methods directly. Every
// instance of a given shell folder class in this process shares a single
// vtable, so one hook on EnumObjects bounds every enumeration the menu band
// drives, cascading sub-folders included. The pidl-handling methods are hooked
// too so the synthetic "timed out" item the bounded enumerator returns renders
// cleanly and stays inert - the band calls those on the real folder, not on us.
//
// Different shell folder classes (file-system folders, the Recycle Bin,
// Network, This PC, compressed folders, ...) are distinct C++ classes with
// distinct vtables and method implementations, so a hook on one class' methods
// does not cover another. We therefore hook each class we encounter and keep a
// per-class set of original pointers, picking the right one in the shared hooks
// by the instance's vtable. CFSFolder (almost every menu) is hooked up front;
// the rest are hooked lazily the first time a menu is opened for them (see
// EnsureFolderHooked / InitEnumTimeoutHooks).

using EnumObjects_t = HRESULT(STDMETHODCALLTYPE*)(IShellFolder*,
                                                  HWND,
                                                  SHCONTF,
                                                  IEnumIDList**);
using BindToObject_t = HRESULT(STDMETHODCALLTYPE*)(IShellFolder*,
                                                   PCUIDLIST_RELATIVE,
                                                   IBindCtx*,
                                                   REFIID,
                                                   void**);
using CompareIDs_t = HRESULT(STDMETHODCALLTYPE*)(IShellFolder*,
                                                 LPARAM,
                                                 PCUIDLIST_RELATIVE,
                                                 PCUIDLIST_RELATIVE);
using GetAttributesOf_t = HRESULT(STDMETHODCALLTYPE*)(IShellFolder*,
                                                      UINT,
                                                      PCUITEMID_CHILD_ARRAY,
                                                      SFGAOF*);
using GetUIObjectOf_t = HRESULT(STDMETHODCALLTYPE*)(IShellFolder*,
                                                    HWND,
                                                    UINT,
                                                    PCUITEMID_CHILD_ARRAY,
                                                    REFIID,
                                                    UINT*,
                                                    void**);
using GetDisplayNameOf_t = HRESULT(STDMETHODCALLTYPE*)(IShellFolder*,
                                                       PCUITEMID_CHILD,
                                                       SHGDNF,
                                                       STRRET*);

// Original IShellFolder methods for one folder class, plus its vtable (the map
// key below).
struct FolderOrigs {
    void** vtable;
    EnumObjects_t enumObjects;
    BindToObject_t bindToObject;
    CompareIDs_t compareIDs;
    GetAttributesOf_t getAttributesOf;
    GetUIObjectOf_t getUIObjectOf;
    GetDisplayNameOf_t getDisplayNameOf;
};

// Cap on distinct folder classes we hook (CFSFolder, Recycle Bin, Network, This
// PC, Control Panel, compressed folders, ...). Far above any realistic count; a
// backstop so we can't end up hooking without limit.
constexpr size_t kMaxFolderClasses = 32;

// vtable -> originals for every folder class we have hooked. Classes are added
// lazily the first time a menu is opened for one (see EnsureFolderHooked), so
// this bounds whatever the user actually opens rather than a hard-coded list.
// Entries are never removed and unordered_map keeps element references stable,
// so the trampoline storage handed to SetFunctionHook stays valid for the life
// of the process. Guarded by g_folderHookLock: written (exclusive) on the UI
// thread when a class is first seen, read (shared) on the UI and worker threads
// by the hooks that dispatch through it.
//
// An SRW lock (not a CRITICAL_SECTION) on purpose: it needs no init/cleanup, so
// the inline hooks - which acquire it on every call and stay installed until
// Windhawk removes them, past WhTool_ModUninit - can never touch a destroyed
// lock. It is NOT recursive, so nothing inside a locked region may round-trip
// through a hooked IShellFolder method on the same thread (that
// self-deadlocks); none does.
SRWLOCK g_folderHookLock = SRWLOCK_INIT;
std::unordered_map<void**, FolderOrigs> g_folderOrigs;

// Originals for the class `pThis` belongs to, or nullptr if that class is not
// hooked. A hook only fires for a class we hooked, so this effectively always
// finds a match; the nullptr path is a guard against the (not expected for
// distinct shell folders) case of two classes sharing a method implementation.
const FolderOrigs* OrigsForFolder(IShellFolder* pThis) {
    void** vtable = *(void***)pThis;
    AcquireSRWLockShared(&g_folderHookLock);
    auto it = g_folderOrigs.find(vtable);
    const FolderOrigs* origs =
        it != g_folderOrigs.end() ? &it->second : nullptr;
    ReleaseSRWLockShared(&g_folderHookLock);
    return origs;
}

// Defined below; forward-declared because BindToObject_Hook propagates hooking
// to bound sub-folders through it.
void EnsureFolderHooked(IShellFolder* folder, PCWSTR source);

HRESULT STDMETHODCALLTYPE EnumObjects_Hook(IShellFolder* pThis,
                                           HWND hwnd,
                                           SHCONTF grfFlags,
                                           IEnumIDList** ppenumIDList) {
    const FolderOrigs* origs = OrigsForFolder(pThis);
    if (!origs) {
        // No original to forward to; fail rather than recurse into ourselves.
        return E_FAIL;
    }

    HRESULT hr = origs->enumObjects(pThis, hwnd, grfFlags, ppenumIDList);

    // Only the menu band's enumeration is bounded. The background worker also
    // enumerates folders (to build its hover map) and must see the complete
    // list, so its own enumeration - and only its - is left untouched.
    if (hr != S_OK || !ppenumIDList || !*ppenumIDList ||
        GetCurrentThreadId() == g_workerThreadId) {
        return hr;
    }

    IEnumIDList* inner = *ppenumIDList;
    auto* wrapper = new (std::nothrow) CTimeoutEnumIDList(inner);
    if (wrapper) {
        inner->Release();  // The wrapper holds its own reference now.
        *ppenumIDList = wrapper;
    }
    return hr;
}

// The menu band binds each cascade sub-folder through its parent's
// BindToObject. Hooking it (on every class we hook) lets us hook the bound
// sub-folder's own class right here - before the band enumerates it - so a
// cascade into a different folder class (e.g. a compressed folder inside a
// normal one) is bounded on first expansion. Because the opened root folder is
// always hooked, this propagates coverage down the whole cascade tree.
HRESULT STDMETHODCALLTYPE BindToObject_Hook(IShellFolder* pThis,
                                            PCUIDLIST_RELATIVE pidl,
                                            IBindCtx* pbc,
                                            REFIID riid,
                                            void** ppv) {
    const FolderOrigs* origs = OrigsForFolder(pThis);
    HRESULT hr =
        origs ? origs->bindToObject(pThis, pidl, pbc, riid, ppv) : E_FAIL;
    if (SUCCEEDED(hr) && ppv && *ppv &&
        (IsEqualIID(riid, IID_IShellFolder) ||
         IsEqualIID(riid, IID_IShellFolder2))) {
        // IShellFolder2 derives from IShellFolder, so the pointer is a valid
        // IShellFolder either way.
        EnsureFolderHooked(reinterpret_cast<IShellFolder*>(*ppv), L"cascade");
    }
    return hr;
}

HRESULT STDMETHODCALLTYPE CompareIDs_Hook(IShellFolder* pThis,
                                          LPARAM lParam,
                                          PCUIDLIST_RELATIVE pidl1,
                                          PCUIDLIST_RELATIVE pidl2) {
    bool t1 = IsTimeoutPidl(pidl1);
    bool t2 = IsTimeoutPidl(pidl2);
    if (t1 || t2) {
        // Keep the synthetic item at the bottom of the list (and away from the
        // real folder's comparer, which would choke on our fake pidl).
        int order = (t1 == t2) ? 0 : (t1 ? 1 : -1);
        return MAKE_HRESULT(SEVERITY_SUCCESS, 0, (USHORT)(SHORT)order);
    }
    const FolderOrigs* origs = OrigsForFolder(pThis);
    return origs ? origs->compareIDs(pThis, lParam, pidl1, pidl2)
                 : MAKE_HRESULT(SEVERITY_SUCCESS, 0, 0);
}

HRESULT STDMETHODCALLTYPE GetAttributesOf_Hook(IShellFolder* pThis,
                                               UINT cidl,
                                               PCUITEMID_CHILD_ARRAY apidl,
                                               SFGAOF* rgfInOut) {
    for (UINT i = 0; i < cidl; i++) {
        if (IsTimeoutPidl(apidl[i])) {
            // Inert, non-folder entry: no expand arrow, nothing to launch.
            if (rgfInOut) {
                *rgfInOut = 0;
            }
            return S_OK;
        }
    }
    const FolderOrigs* origs = OrigsForFolder(pThis);
    return origs ? origs->getAttributesOf(pThis, cidl, apidl, rgfInOut)
                 : E_FAIL;
}

HRESULT STDMETHODCALLTYPE GetUIObjectOf_Hook(IShellFolder* pThis,
                                             HWND hwndOwner,
                                             UINT cidl,
                                             PCUITEMID_CHILD_ARRAY apidl,
                                             REFIID riid,
                                             UINT* rgfReserved,
                                             void** ppv) {
    for (UINT i = 0; i < cidl; i++) {
        if (IsTimeoutPidl(apidl[i])) {
            // No icon, context menu or launch object for the notice item.
            if (ppv) {
                *ppv = nullptr;
            }
            return E_NOINTERFACE;
        }
    }
    const FolderOrigs* origs = OrigsForFolder(pThis);
    return origs ? origs->getUIObjectOf(pThis, hwndOwner, cidl, apidl, riid,
                                        rgfReserved, ppv)
                 : E_NOINTERFACE;
}

HRESULT STDMETHODCALLTYPE GetDisplayNameOf_Hook(IShellFolder* pThis,
                                                PCUITEMID_CHILD pidl,
                                                SHGDNF uFlags,
                                                STRRET* pName) {
    if (IsTimeoutPidl(pidl)) {
        if (!pName) {
            return E_POINTER;
        }
        static constexpr WCHAR text[] = L"(too many items, list truncated)";
        LPWSTR copy = (LPWSTR)CoTaskMemAlloc(sizeof(text));
        if (!copy) {
            return E_OUTOFMEMORY;
        }
        memcpy(copy, text, sizeof(text));
        pName->uType = STRRET_WSTR;
        pName->pOleStr = copy;
        return S_OK;
    }
    const FolderOrigs* origs = OrigsForFolder(pThis);
    return origs ? origs->getDisplayNameOf(pThis, pidl, uFlags, pName) : E_FAIL;
}

// IShellFolder vtable slot indices (after the three IUnknown slots).
enum {
    kVtblEnumObjects = 4,
    kVtblBindToObject = 5,
    kVtblCompareIDs = 7,
    kVtblGetAttributesOf = 9,
    kVtblGetUIObjectOf = 10,
    kVtblGetDisplayNameOf = 11,
};

// Installs all six method hooks on `folder`'s vtable and records its originals
// (and the vtable itself, used to match instances to this class). A method
// whose hook fails to install is logged and simply left unhooked - its calls
// then go straight to the real method (so that one method is not bounded), but
// nothing crashes, because OrigsForFolder is only consulted from a hook that
// did install.
void HookFolderVtable(IShellFolder* folder, FolderOrigs& origs) {
    void** vtable = *(void***)folder;
    origs.vtable = vtable;

    // The inline hooks live in the class' DLL and are never removed, so keep
    // that DLL loaded for the rest of the process. Otherwise an on-demand
    // shell-extension DLL (zipfldr, wpdshext, third-party folder providers)
    // could be unloaded out from under our still-installed hooks, leaving the
    // trampolines and the stale g_folderOrigs entry dangling. Pinning resolves
    // the module from a hooked function address; one pin per class covers all
    // six methods, which belong to the same DLL. It is a no-op for shell32 (the
    // file-system / Recycle Bin / Network / This PC classes), already
    // permanent.
    HMODULE pinned = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_PIN |
                                GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                            reinterpret_cast<LPCWSTR>(vtable[kVtblEnumObjects]),
                            &pinned)) {
        Wh_Log(L"Failed to pin module for folder vtable %p", vtable);
    }

    auto check = [vtable](PCWSTR label, BOOL hooked) {
        if (!hooked) {
            Wh_Log(L"SetFunctionHook(%s) failed for folder vtable %p", label,
                   vtable);
        }
    };

    check(L"EnumObjects", WindhawkUtils::SetFunctionHook(
                              (EnumObjects_t)vtable[kVtblEnumObjects],
                              EnumObjects_Hook, &origs.enumObjects));
    check(L"BindToObject", WindhawkUtils::SetFunctionHook(
                               (BindToObject_t)vtable[kVtblBindToObject],
                               BindToObject_Hook, &origs.bindToObject));
    check(L"CompareIDs",
          WindhawkUtils::SetFunctionHook((CompareIDs_t)vtable[kVtblCompareIDs],
                                         CompareIDs_Hook, &origs.compareIDs));
    check(L"GetAttributesOf",
          WindhawkUtils::SetFunctionHook(
              (GetAttributesOf_t)vtable[kVtblGetAttributesOf],
              GetAttributesOf_Hook, &origs.getAttributesOf));
    check(L"GetUIObjectOf", WindhawkUtils::SetFunctionHook(
                                (GetUIObjectOf_t)vtable[kVtblGetUIObjectOf],
                                GetUIObjectOf_Hook, &origs.getUIObjectOf));
    check(L"GetDisplayNameOf",
          WindhawkUtils::SetFunctionHook(
              (GetDisplayNameOf_t)vtable[kVtblGetDisplayNameOf],
              GetDisplayNameOf_Hook, &origs.getDisplayNameOf));
}

// Registers `folder`'s class (its vtable) and queues its method hooks. Must be
// called with g_folderHookLock held exclusively (or single-threaded, as at
// init). Returns true if the class was newly added (hooks queued and still need
// applying), false if it was already known or the class limit was reached.
// `source` tags the log with what triggered the hook (init / menu / cascade).
// Does NOT apply the queued hooks - init relies on Windhawk's auto-apply after
// Wh_ModInit; runtime callers apply via Wh_ApplyHookOperations.
bool AddFolderClassLocked(IShellFolder* folder, PCWSTR source) {
    void** vtable = *(void***)folder;
    if (g_folderOrigs.find(vtable) != g_folderOrigs.end()) {
        return false;
    }
    if (g_folderOrigs.size() >= kMaxFolderClasses) {
        Wh_Log(L"Folder class limit reached; vtable %p left unbounded", vtable);
        return false;
    }
    HookFolderVtable(folder, g_folderOrigs[vtable]);
    Wh_Log(L"Hooking enumeration on folder vtable %p (%s)", vtable, source);
    return true;
}

// Ensures the class of `folder` is hooked so the menu band's enumeration of it
// is bounded. Called just before a menu is shown (source "menu") and when a
// cascade sub-folder is bound (source "cascade"), so the hook is in place
// before the band enumerates the folder. Safe to call repeatedly; it no-ops
// once the class is known. A newly hooked class is applied here (runtime),
// unlike the init pre-hooks.
void EnsureFolderHooked(IShellFolder* folder, PCWSTR source) {
    if (!folder) {
        return;
    }
    AcquireSRWLockExclusive(&g_folderHookLock);
    if (AddFolderClassLocked(folder, source)) {
        if (!Wh_ApplyHookOperations()) {
            Wh_Log(L"Wh_ApplyHookOperations failed (%s)", source);
        }
    }
    ReleaseSRWLockExclusive(&g_folderHookLock);
}

// Initializes the folder-hook table and pre-hooks the dominant folder class
// (CFSFolder - the file-system folders that make up almost every menu) so it is
// bounded from the first menu, before any thread that reads the table exists,
// and without runtime patching. Every other folder class (the Recycle Bin,
// Network, This PC, compressed folders, ...) is hooked lazily by
// EnsureFolderHooked the first time its menu is opened. Called once at init;
// needs a COM apartment to bind the sample folder, so it sets one up
// temporarily. The pre-hook is queued during Wh_ModInit and so is applied
// automatically when init returns - no Wh_ApplyHookOperations here.
void InitEnumTimeoutHooks() {
    bool comInited =
        SUCCEEDED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED));

    winrt::com_ptr<IShellFolder> desktop;
    if (FAILED(SHGetDesktopFolder(desktop.put())) || !desktop) {
        Wh_Log(L"InitEnumTimeoutHooks: SHGetDesktopFolder failed");
        if (comInited) {
            CoUninitialize();
        }
        return;
    }

    // CFSFolder: bind any real file-system folder to reach its shared vtable.
    // Use the Windows directory rather than a hard-coded "C:\\" so this still
    // works when Windows lives on another drive.
    WCHAR fsPath[MAX_PATH];
    if (!GetWindowsDirectoryW(fsPath, ARRAYSIZE(fsPath))) {
        fsPath[0] = L'\0';
    }
    PIDLIST_ABSOLUTE fsPidl = nullptr;
    winrt::com_ptr<IShellFolder> fsFolder;
    if (fsPath[0] &&
        SUCCEEDED(SHParseDisplayName(fsPath, nullptr, &fsPidl, 0, nullptr)) &&
        fsPidl &&
        SUCCEEDED(desktop->BindToObject(fsPidl, nullptr,
                                        IID_PPV_ARGS(fsFolder.put()))) &&
        fsFolder) {
        AddFolderClassLocked(fsFolder.get(), L"init");
    } else {
        Wh_Log(L"InitEnumTimeoutHooks: failed to resolve CFSFolder vtable");
    }

    if (fsPidl) {
        ILFree(fsPidl);
    }
    fsFolder = nullptr;
    desktop = nullptr;

    if (comInited) {
        CoUninitialize();
    }
}

////////////////////////////////////////////////////////////////////////////////
// The cascading folder menu (adapted from folder_menu.c / "Quick Folder Menu").

// Tears the menu band down. Used both when another window takes the foreground
// and when we cannot bring the popup to the foreground ourselves.
void CloseMenuBand(IMenuBand* band) {
    winrt::com_ptr<IOleCommandTarget> commandTarget;
    if (SUCCEEDED(band->QueryInterface(IID_PPV_ARGS(commandTarget.put())))) {
        commandTarget->Exec(&CLSID_MenuBand, MBAND_CMDID_CLOSE, 0, nullptr,
                            nullptr);
    }
}

// The top-level window hosting the menu band, or nullptr.
HWND GetMenuBandWindow(IMenuBand* band) {
    winrt::com_ptr<IOleWindow> oleWindow;
    HWND hwnd = nullptr;
    if (SUCCEEDED(band->QueryInterface(IID_PPV_ARGS(oleWindow.put()))) &&
        oleWindow) {
        oleWindow->GetWindow(&hwnd);
    }
    return hwnd ? GetAncestor(hwnd, GA_ROOT) : nullptr;
}

// The shell menu band hosts its item toolbar inside a standard Pager control
// (WC_PAGESCROLLER, class "SysPager"); this is its window class name.
constexpr WCHAR kPagerClass[] = L"SysPager";

// Walks up from `hwnd` (inclusive) to the menu's Pager control, or nullptr.
HWND FindMenuPagerFromWindow(HWND hwnd) {
    for (; hwnd; hwnd = GetParent(hwnd)) {
        WCHAR cls[64];
        if (GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) &&
            wcscmp(cls, kPagerClass) == 0) {
            return hwnd;
        }
    }
    return nullptr;
}

// Finds the first descendant of `parent` with the given class, or nullptr.
// Uses EnumChildWindows rather than walking the tree with GetWindow: it is
// snapshot-based, so it can't loop or trip over a stale handle if a window is
// destroyed mid-search.
struct FindDescendantParams {
    PCWSTR className;
    HWND result;
};

BOOL CALLBACK FindDescendantProc(HWND hwnd, LPARAM lParam) {
    auto* params = (FindDescendantParams*)lParam;
    WCHAR cls[64];
    if (GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) &&
        wcscmp(cls, params->className) == 0) {
        params->result = hwnd;
        return FALSE;  // Found it; stop enumerating.
    }
    return TRUE;
}

HWND FindDescendantOfClass(HWND parent, PCWSTR className) {
    FindDescendantParams params = {className, nullptr};
    EnumChildWindows(parent, FindDescendantProc, (LPARAM)&params);
    return params.result;
}

// Scrolls a long folder menu with the mouse wheel. The menu band has no native
// wheel support: it parks its (taller-than-screen) item toolbar in a Pager
// control whose top and bottom arrows are the only built-in way to scroll, and
// neither the toolbar nor the pager reacts to the wheel. Translate the wheel
// delta into a Pager scroll-position change (PGM_SETPOS), scaled by the
// toolbar's row height so a notch moves a few items.
void ScrollMenuWithWheel(HWND pager, int wheelDelta) {
    UINT linesPerNotch = 3;
    SystemParametersInfoW(SPI_GETWHEELSCROLLLINES, 0, &linesPerNotch, 0);
    if (linesPerNotch == 0) {
        return;  // Wheel scrolling is turned off system-wide.
    }
    if (linesPerNotch == WHEEL_PAGESCROLL) {
        linesPerNotch = 5;  // "One screen" per notch; approximate for a menu.
    }

    HWND toolbar = GetWindow(pager, GW_CHILD);
    if (!toolbar) {
        return;
    }

    // Nothing to scroll unless the toolbar is taller than the pager's viewport.
    // maxPos matches the pager's own internal clamp (child height minus pager
    // client height). Poking PGM_SETPOS when the menu already fits makes the
    // pager flash a scroll arrow, so bail out (without consuming the delta)
    // when it is not scrollable.
    RECT pagerRc = {};
    RECT toolbarRc = {};
    GetClientRect(pager, &pagerRc);
    GetWindowRect(toolbar, &toolbarRc);
    int maxPos = (toolbarRc.bottom - toolbarRc.top) - pagerRc.bottom;
    if (maxPos <= 0) {
        return;
    }

    // Accumulate sub-notch deltas so high-resolution wheels and touchpads still
    // step smoothly.
    static int s_accumulatedDelta;
    s_accumulatedDelta += wheelDelta;
    int notches = s_accumulatedDelta / WHEEL_DELTA;
    s_accumulatedDelta -= notches * WHEEL_DELTA;
    if (notches == 0) {
        return;
    }

    // Step by the toolbar's row height; fall back to a sane default.
    int itemHeight = 20;
    if (SendMessageW(toolbar, TB_BUTTONCOUNT, 0, 0) > 0) {
        RECT br = {};
        if (SendMessageW(toolbar, TB_GETITEMRECT, 0, (LPARAM)&br) &&
            br.bottom > br.top) {
            itemHeight = br.bottom - br.top;
        }
    }

    // Positive delta (wheel up) scrolls toward the top, i.e. a smaller pos.
    int pos = (int)SendMessageW(pager, PGM_GETPOS, 0, 0);
    int newPos = pos - notches * (int)linesPerNotch * itemHeight;
    if (newPos < 0) {
        newPos = 0;
    } else if (newPos > maxPos) {
        newPos = maxPos;
    }
    if (newPos != pos) {
        SendMessageW(pager, PGM_SETPOS, 0, newPos);
    }
}

// Finds the menu Pager to scroll for a wheel event at screen point `pt` and
// drives it. Scrolls the menu directly under the pointer (so a hovered submenu
// scrolls rather than its parent), and only that menu.
//
// Scrolling is driven from global raw input (see SinkWndProc), so the wheel can
// turn while the cursor is over any window, in any process. We act only when
// the cursor is over one of our own menu windows: this both scopes scrolling to
// the thing under the pointer (matching normal wheel behavior, and avoiding
// fighting a scrollable window behind the menu) and guards the pager lookup -
// "SysPager" is a stock common control hosted elsewhere too (notably the
// taskbar notification area), so a class-name match alone could otherwise drive
// an unrelated process's control.
void ScrollMenuAtPoint(POINT pt, int wheelDelta) {
    HWND under = WindowFromPoint(pt);
    DWORD underPid = 0;
    if (under) {
        GetWindowThreadProcessId(under, &underPid);
    }
    if (!under || underPid != GetCurrentProcessId()) {
        return;
    }

    HWND pager = FindMenuPagerFromWindow(under);
    if (!pager) {
        if (HWND root = GetAncestor(under, GA_ROOT)) {
            pager = FindDescendantOfClass(root, kPagerClass);
        }
    }

    if (pager) {
        ScrollMenuWithWheel(pager, wheelDelta);
    }
}

// Returns the item toolbar under the cursor and, via outIdCmd, the command id
// of the folder item there - or nullptr if the cursor is not over a folder
// item. Files and empty menu chrome return nullptr (the band already opens a
// file on a click). `pt` is a screen point.
HWND GetMenuFolderItemUnderCursor(POINT pt, int* outIdCmd) {
    HWND under = WindowFromPoint(pt);
    DWORD underPid = 0;
    if (under) {
        GetWindowThreadProcessId(under, &underPid);
    }
    if (!under || underPid != GetCurrentProcessId()) {
        return nullptr;
    }

    // The menu's items live in a toolbar that is the SysPager's child (the same
    // toolbar the wheel code scrolls). Reach it through the pager rather than
    // by class name - the band may register the toolbar under its own class, so
    // a class-name match would miss it. Walk up from the window under the
    // cursor to the pager; if that fails (the cursor is not under the pager
    // subtree), fall back to the pager anywhere under the menu's root window.
    HWND pager = FindMenuPagerFromWindow(under);
    if (!pager) {
        if (HWND root = GetAncestor(under, GA_ROOT)) {
            pager = FindDescendantOfClass(root, kPagerClass);
        }
    }
    HWND toolbar = pager ? GetWindow(pager, GW_CHILD) : nullptr;
    if (!toolbar) {
        return nullptr;
    }

    POINT client = pt;
    if (!ScreenToClient(toolbar, &client)) {
        return nullptr;
    }

    int index = (int)SendMessageW(toolbar, TB_HITTEST, 0, (LPARAM)&client);
    if (index < 0) {
        return nullptr;  // Not on an item (a gap, or a scroll arrow).
    }

    // A folder (cascade) item carries the dropdown style in a vertical menu; a
    // leaf (file) item does not.
    TBBUTTON button = {};
    if (!SendMessageW(toolbar, TB_GETBUTTON, index, (LPARAM)&button) ||
        !(button.fsStyle & (BTNS_DROPDOWN | BTNS_WHOLEDROPDOWN))) {
        return nullptr;
    }

    *outIdCmd = button.idCommand;
    return toolbar;
}

// Hands `pidl` (a folder, borrowed) off to the worker thread to open. A null or
// non-folder pidl is ignored. Does not dismiss the menu - the band does that
// itself once the execute completes.
void PostFolderActionToWorker(FolderAction action, PCIDLIST_ABSOLUTE pidl) {
    if (action == FolderAction::nothing || !pidl || !IsFolderPidl(pidl)) {
        return;
    }

    // The worker may wait on Explorer (a new tab); it owns the request and
    // frees it.
    auto* req = new (std::nothrow) FolderActionRequest{};
    if (!req) {
        return;
    }
    req->pidl = ILClone(pidl);
    req->action = action;
    req->sourceTab = g_targetTab;
    req->sourceIsDesktop = g_targetIsDesktop;
    if (!req->pidl || !g_workerThreadId ||
        !PostThreadMessageW(g_workerThreadId, WM_APP_DO_ACTION, (WPARAM)req,
                            0)) {
        if (req->pidl) {
            ILFree(req->pidl);
        }
        delete req;
    }
}

// Hands `pidl` (a clicked file in a file dialog's menu, borrowed) off to the
// worker thread to select into the dialog's file name box. The worker owns the
// request and frees it.
void PostFileSelectToWorker(PCIDLIST_ABSOLUTE pidl) {
    if (!pidl) {
        return;
    }

    auto* req = new (std::nothrow) FolderActionRequest{};
    if (!req) {
        return;
    }
    req->pidl = ILClone(pidl);
    req->action = FolderAction::nothing;
    req->selectFile = true;
    req->sourceTab = g_targetTab;
    req->sourceIsDesktop = g_targetIsDesktop;
    if (!req->pidl || !g_workerThreadId ||
        !PostThreadMessageW(g_workerThreadId, WM_APP_DO_ACTION, (WPARAM)req,
                            0)) {
        if (req->pidl) {
            ILFree(req->pidl);
        }
        delete req;
    }
}

// Runs `action` on a clicked folder item (the toolbar + command id a click was
// hit-tested to). We don't resolve the pidl ourselves: we post the band's own
// "execute" message for that command id, exactly as a double-click does. The
// band resolves the pidl and calls us back via SMC_SFEXEC (see CMenuCallback),
// where the action runs - so it always acts on the item the cursor is over,
// even after navigating back out of a submenu. A null toolbar (not on a folder
// item) is ignored.
void ExecMenuFolderItem(HWND toolbar, int idCmd, FolderAction action) {
    if (!toolbar || action == FolderAction::nothing || !g_cmbExecuteMsg) {
        return;
    }
    g_pendingExecAction = action;
    PostMessageW(toolbar, g_cmbExecuteMsg, idCmd, 0);
}

VOID CALLBACK ForegroundChangedProc(HWINEVENTHOOK hook,
                                    DWORD event,
                                    HWND hwnd,
                                    LONG idObject,
                                    LONG idChild,
                                    DWORD idEventThread,
                                    DWORD dwmsEventTime) {
    IMenuBand* band = g_pActiveMenuBand;
    if (band) {
        CloseMenuBand(band);
    }
}

// Receives notifications from the menu band. We use SMC_SFEXEC: when the band
// executes an item it hands us that item's folder and relative pidl, which it
// resolved itself - so we never map a toolbar button to a pidl. When the
// execute is one we triggered for a folder click (g_pendingExecAction set), we
// run the configured action on that pidl and return S_OK so the band does not
// also open it. Everything else (a leaf item, a real double-click) passes
// through (S_FALSE) to the band's default.
class CMenuCallback final : public IShellMenuCallback {
   public:
    IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) {
            return E_POINTER;
        }
        if (IsEqualIID(riid, IID_IUnknown) ||
            IsEqualIID(riid, __uuidof(IShellMenuCallback))) {
            *ppv = static_cast<IShellMenuCallback*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }

    IFACEMETHODIMP_(ULONG) AddRef() override {
        return InterlockedIncrement(&m_ref);
    }

    IFACEMETHODIMP_(ULONG) Release() override {
        LONG ref = InterlockedDecrement(&m_ref);
        if (ref == 0) {
            delete this;
        }
        return ref;
    }

    IFACEMETHODIMP CallbackSM(LPSMDATA psmd,
                              UINT uMsg,
                              WPARAM wParam,
                              LPARAM lParam) override {
        if (uMsg == SMC_SFEXEC &&
            g_pendingExecAction != FolderAction::nothing && psmd &&
            (psmd->dwMask & SMDM_SHELLFOLDER) && psmd->pidlFolder &&
            psmd->pidlItem) {
            FolderAction action = g_pendingExecAction;
            g_pendingExecAction = FolderAction::nothing;
            if (PIDLIST_ABSOLUTE itemAbs =
                    ILCombine(psmd->pidlFolder, psmd->pidlItem)) {
                PostFolderActionToWorker(action, itemAbs);
                ILFree(itemAbs);
            }
            return S_OK;  // Handled; the band must not open it itself.
        }
        // In a file dialog, clicking a file would otherwise launch it.
        // Intercept the band's own execute (no forced action pending) and
        // select the file into the dialog instead. Folders are left to the
        // band's default (cascade); our folder click actions go through the
        // branch above.
        if (uMsg == SMC_SFEXEC && g_targetIsDialog && psmd &&
            (psmd->dwMask & SMDM_SHELLFOLDER) && psmd->pidlFolder &&
            psmd->pidlItem) {
            if (PIDLIST_ABSOLUTE itemAbs =
                    ILCombine(psmd->pidlFolder, psmd->pidlItem)) {
                bool isFolder = IsFolderPidl(itemAbs);
                if (!isFolder) {
                    PostFileSelectToWorker(itemAbs);
                    ILFree(itemAbs);
                    return S_OK;  // Handled; the band must not launch it.
                }
                ILFree(itemAbs);
            }
        }
        // Everything else takes the band's default.
        return S_FALSE;
    }

   private:
    LONG m_ref = 1;
};

// Hosts the folder's contents in a menu band inside a menu desk bar and pops it
// up next to the anchor rect (the expand button). Returns the live IMenuBand or
// nullptr on failure.
winrt::com_ptr<IMenuBand> PopupFolderMenu(PCIDLIST_ABSOLUTE pidlAbs,
                                          RECT anchorRect) {
    winrt::com_ptr<IShellMenu> shellMenu;
    winrt::com_ptr<IDeskBand> deskBand;
    winrt::com_ptr<IMenuBand> menuBand;
    HRESULT hr = CoCreateInstance(CLSID_MenuBand, nullptr, CLSCTX_INPROC_SERVER,
                                  IID_PPV_ARGS(shellMenu.put()));
    if (SUCCEEDED(hr)) {
        // Pass a callback so we can learn each item's pidl as the selection
        // follows the cursor (used by the folder click actions). The band keeps
        // its default behavior; the callback only observes. A null callback
        // (allocation failure) just restores the default no-callback behavior.
        winrt::com_ptr<IShellMenuCallback> callback;
        callback.attach(new (std::nothrow) CMenuCallback());
        hr = shellMenu->Initialize(callback.get(), -1, ANCESTORDEFAULT,
                                   SMINIT_TOPLEVEL | SMINIT_VERTICAL);

        winrt::com_ptr<IShellFolder> desktop;
        if (SUCCEEDED(hr)) {
            hr = SHGetDesktopFolder(desktop.put());
        }

        if (SUCCEEDED(hr)) {
            winrt::com_ptr<IShellFolder> folder;
            hr = desktop->BindToObject(pidlAbs, nullptr,
                                       IID_PPV_ARGS(folder.put()));
            if (SUCCEEDED(hr)) {
                // Make sure this folder's class is bounded before the menu band
                // enumerates it below. Covers any folder class the user opens
                // (Network, This PC, compressed folders, ...), not just the
                // one pre-hooked at init. Cascades into sub-folders of other
                // classes are then hooked as they are bound (see
                // BindToObject_Hook), so they are bounded too.
                EnsureFolderHooked(folder.get(), L"menu");
                hr = shellMenu->SetShellFolder(
                    folder.get(), pidlAbs, nullptr,
                    SMSET_BOTTOM | SMSET_USEBKICONEXTRACTION);
                if (SUCCEEDED(hr)) {
                    hr =
                        shellMenu->QueryInterface(IID_PPV_ARGS(deskBand.put()));
                }
            }
        }
    }

    if (deskBand) {
        winrt::com_ptr<IUnknown> deskBarUnk;
        hr = CoCreateInstance(CLSID_MenuDeskBar, nullptr, CLSCTX_INPROC_SERVER,
                              IID_PPV_ARGS(deskBarUnk.put()));
        if (SUCCEEDED(hr)) {
            winrt::com_ptr<IMenuPopup> menuPopup;
            hr = deskBarUnk->QueryInterface(IID_PPV_ARGS(menuPopup.put()));
            if (SUCCEEDED(hr)) {
                winrt::com_ptr<IBandSite> bandSite;
                hr = CoCreateInstance(CLSID_MenuBandSite, nullptr,
                                      CLSCTX_INPROC_SERVER,
                                      IID_PPV_ARGS(bandSite.put()));
                if (SUCCEEDED(hr)) {
                    hr = menuPopup->SetClient(bandSite.get());
                    if (SUCCEEDED(hr)) {
                        hr = bandSite->AddBand(deskBand.get());
                    }

                    if (SUCCEEDED(hr)) {
                        // Anchor the menu to the button's right edge and pass
                        // its rect as the exclude region so the menu opens
                        // alongside the button without covering it.
                        POINTL pt = {anchorRect.right, anchorRect.top};
                        RECTL exclude = {anchorRect.left, anchorRect.top,
                                         anchorRect.right, anchorRect.bottom};

                        hr = menuPopup->Popup(&pt, &exclude, MPPF_SETFOCUS);
                        if (SUCCEEDED(hr)) {
                            hr = deskBand->QueryInterface(
                                IID_PPV_ARGS(menuBand.put()));
                        }
                    }
                }
            }
        }
    }

    // The desk bar's window holds an internal self-reference while the popup is
    // shown, so the menu stays alive after the local com_ptrs (the desk bar and
    // band site) are released here; the returned menu band keeps it reachable.
    if (SUCCEEDED(hr) && menuBand) {
        return menuBand;
    }

    return nullptr;
}

// Shows the folder menu and pumps a nested message loop until it is dismissed.
void ShowFolderMenuModal(PCIDLIST_ABSOLUTE pidlAbs, RECT anchorRect) {
    g_menuActive = true;
    g_leftDownToolbar = nullptr;

    winrt::com_ptr<IMenuBand> band = PopupFolderMenu(pidlAbs, anchorRect);
    if (band) {
        // The popup belongs to our background tool process, so the menu band's
        // own focus attempt can lose to the foreground lock and leave the menu
        // on screen with no way to dismiss it. Claim the foreground explicitly;
        // if that fails, tear the popup down instead of letting it linger.
        HWND hwndMenu = GetMenuBandWindow(band.get());
        if (!hwndMenu || !SetForegroundWindow(hwndMenu)) {
            Wh_Log(L"Could not bring menu window to foreground, closing");
            CloseMenuBand(band.get());
            g_menuActive = false;
            return;
        }

        g_pActiveMenuBand = band.get();

        HWINEVENTHOOK hook =
            SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
                            nullptr, ForegroundChangedProc, 0, 0,
                            WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);

        bool done = false;
        while (!done) {
            MSG msg;
            BOOL bRet = GetMessageW(&msg, nullptr, 0, 0);
            if (bRet == 0) {
                // WM_QUIT. Re-post it so the outer loop also exits.
                PostQuitMessage((int)msg.wParam);
                break;
            }
            if (bRet == -1) {
                break;
            }
            if (msg.hwnd == nullptr && msg.message == WM_APP_QUIT) {
                // Uninit while the menu is open: turn it into a real quit and
                // leave the menu loop so the outer loop can exit too.
                PostQuitMessage(0);
                break;
            }
            if (msg.hwnd == nullptr && msg.message == WM_APP_SETTINGS_CHANGED) {
                // The outer loop won't see this null-hwnd thread message once
                // it is consumed here, so apply it now.
                LoadSettings();
                continue;
            }
            // The mouse wheel is handled from raw input in SinkWndProc, not
            // here: for some folders (notably the recycle bin) the cooked
            // WM_MOUSEWHEEL is dequeued and dispatched by a nested message pump
            // the shell runs while populating the menu, so it never reaches
            // this loop. Raw input is delivered to our sink window regardless
            // of which loop is pumping, so scrolling is driven from there
            // instead.

            LRESULT lr;
            switch (band->IsMenuMessage(&msg)) {
                case S_OK:
                    band->TranslateMenuMessage(&msg, &lr);
                    break;
                case E_FAIL:
                    done = true;
                    break;
                default:
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                    break;
            }
        }

        if (hook) {
            UnhookWinEvent(hook);
        }

        // Clear the observer before the owning com_ptr releases the band below.
        g_pActiveMenuBand = nullptr;
    }

    // Deferred work posted by the menu band (e.g. launching an item) runs once
    // we return to the outer message loop, which keeps pumping.
    g_pendingExecAction = FolderAction::nothing;
    g_menuActive = false;
}

////////////////////////////////////////////////////////////////////////////////
// The expand-button overlay window.

// Effective DPI of the monitor that contains the given rectangle.
UINT GetDpiForRect(const RECT& rc) {
    using GetDpiForMonitor_t = HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*);
    static GetDpiForMonitor_t pGetDpiForMonitor = []() -> GetDpiForMonitor_t {
        HMODULE shcore = LoadLibraryExW(L"shcore.dll", nullptr,
                                        LOAD_LIBRARY_SEARCH_SYSTEM32);
        return shcore ? (GetDpiForMonitor_t)GetProcAddress(shcore,
                                                           "GetDpiForMonitor")
                      : nullptr;
    }();

    POINT center = {(rc.left + rc.right) / 2, (rc.top + rc.bottom) / 2};
    HMONITOR monitor = MonitorFromPoint(center, MONITOR_DEFAULTTONEAREST);
    UINT dpiX = 96;
    UINT dpiY = 96;
    if (pGetDpiForMonitor &&
        SUCCEEDED(pGetDpiForMonitor(monitor, 0 /* MDT_EFFECTIVE_DPI */, &dpiX,
                                    &dpiY))) {
        return dpiX;
    }
    return 96;
}

void AddRoundedRectPath(Gdiplus::GraphicsPath& path,
                        const Gdiplus::RectF& rc,
                        float radius) {
    float d = radius * 2.0f;
    path.AddArc(rc.X, rc.Y, d, d, 180.0f, 90.0f);
    path.AddArc(rc.GetRight() - d, rc.Y, d, d, 270.0f, 90.0f);
    path.AddArc(rc.GetRight() - d, rc.GetBottom() - d, d, d, 0.0f, 90.0f);
    path.AddArc(rc.X, rc.GetBottom() - d, d, d, 90.0f, 90.0f);
    path.CloseFigure();
}

// Renders the WinUI3-style button into a per-pixel-alpha layered window: an
// antialiased rounded surface with a subtle border and a smooth chevron glyph.
void RenderChevron(HWND hwnd, int x, int y, int size) {
    HDC screenDC = GetDC(nullptr);
    HDC memDC = CreateCompatibleDC(screenDC);

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = size;
    bmi.bmiHeader.biHeight = -size;  // Top-down.
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP dib =
        CreateDIBSection(screenDC, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, dib);

    bool light = IsLightTheme();
    Gdiplus::Color fill = light ? Gdiplus::Color(250, 252, 252, 252)
                                : Gdiplus::Color(245, 50, 50, 50);
    Gdiplus::Color border = light ? Gdiplus::Color(150, 0, 0, 0)
                                  : Gdiplus::Color(120, 255, 255, 255);
    Gdiplus::Color glyph = light ? Gdiplus::Color(235, 20, 20, 20)
                                 : Gdiplus::Color(245, 240, 240, 240);

    {
        // Wrap the DIB's bits in a premultiplied GDI+ bitmap so antialiased
        // edges blend correctly through UpdateLayeredWindow.
        Gdiplus::Bitmap bitmap(size, size, size * 4, PixelFormat32bppPARGB,
                               (BYTE*)bits);
        Gdiplus::Graphics g(&bitmap);
        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
        g.Clear(Gdiplus::Color(0, 0, 0, 0));

        float inset = 0.75f;
        Gdiplus::RectF surface(inset, inset, size - inset * 2.0f,
                               size - inset * 2.0f);
        Gdiplus::GraphicsPath path;
        AddRoundedRectPath(path, surface, size * 0.25f);

        Gdiplus::SolidBrush fillBrush(fill);
        g.FillPath(&fillBrush, &path);
        Gdiplus::Pen borderPen(border, 1.0f);
        g.DrawPath(&borderPen, &path);

        float cx = size / 2.0f;
        float cy = size / 2.0f;
        float ext = size * 0.17f;
        float thickness = size / 9.0f;
        if (thickness < 1.4f) {
            thickness = 1.4f;
        }
        Gdiplus::Pen glyphPen(glyph, thickness);
        glyphPen.SetStartCap(Gdiplus::LineCapRound);
        glyphPen.SetEndCap(Gdiplus::LineCapRound);
        glyphPen.SetLineJoin(Gdiplus::LineJoinRound);
        Gdiplus::PointF pts[3] = {{cx - ext * 0.55f, cy - ext},
                                  {cx + ext * 0.55f, cy},
                                  {cx - ext * 0.55f, cy + ext}};
        g.DrawLines(&glyphPen, pts, 3);
    }

    POINT dst = {x, y};
    SIZE sz = {size, size};
    POINT src = {0, 0};
    // SourceConstantAlpha scales the per-pixel alpha uniformly, fading the
    // whole button (fill, border and glyph) by the configured opacity.
    BYTE constAlpha = (BYTE)((g_settings.opacity * 255 + 50) / 100);
    BLENDFUNCTION blend = {AC_SRC_OVER, 0, constAlpha, AC_SRC_ALPHA};
    UpdateLayeredWindow(hwnd, screenDC, &dst, &sz, memDC, &src, 0, &blend,
                        ULW_ALPHA);

    SelectObject(memDC, oldBmp);
    DeleteObject(dib);
    DeleteDC(memDC);
    ReleaseDC(nullptr, screenDC);
}

void HideChevron() {
    if (g_chevronVisible) {
        ShowWindow(g_chevronWnd, SW_HIDE);
        g_chevronVisible = false;
        KillTimer(g_sinkWnd, kWatchdogTimerId);
    }
    SetRectEmpty(&g_hoverItemRect);
    SetRectEmpty(&g_chevronRect);
}

// Takes ownership of childAbs.
void ShowChevronForItem(PIDLIST_ABSOLUTE childAbs, RECT itemRect) {
    // No change since last time: keep the existing button. This makes the
    // re-checks (mouse moves, refreshes, the watchdog) cheap - they only
    // repaint when the hovered folder or its rect actually changes.
    if (g_chevronVisible && g_targetPidl &&
        EqualRect(&g_hoverItemRect, &itemRect) &&
        ILIsEqual(g_targetPidl, childAbs)) {
        ILFree(childAbs);
        return;
    }

    if (g_targetPidl) {
        ILFree(g_targetPidl);
    }
    g_targetPidl = childAbs;
    g_hoverItemRect = itemRect;

    UINT dpi = GetDpiForRect(itemRect);
    int size = MulDiv(g_settings.iconSize, dpi, 96);
    int margin = MulDiv(2, dpi, 96);

    bool left = g_settings.position == ButtonPosition::leftBottom ||
                g_settings.position == ButtonPosition::leftTop;
    bool top = g_settings.position == ButtonPosition::leftTop ||
               g_settings.position == ButtonPosition::rightTop;

    int x;
    if (left) {
        x = itemRect.left + margin;
    } else {
        x = itemRect.right - size - margin;
        if (x < itemRect.left) {
            x = itemRect.left;
        }
    }

    int y;
    if (top) {
        y = itemRect.top + margin;
    } else {
        y = itemRect.bottom - size - margin;
        if (y < itemRect.top) {
            y = itemRect.top;
        }
    }

    // Apply the user-configured offset (DPI-scaled). Positive moves right/down.
    x += MulDiv(g_settings.offsetX, dpi, 96);
    y += MulDiv(g_settings.offsetY, dpi, 96);

    SetRect(&g_chevronRect, x, y, x + size, y + size);

    // UpdateLayeredWindow (inside RenderChevron) sets the position, size, and
    // per-pixel-alpha content; SetWindowPos only asserts top-most and shows it.
    RenderChevron(g_chevronWnd, x, y, size);
    SetWindowPos(g_chevronWnd, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
    g_chevronVisible = true;
    SetTimer(g_sinkWnd, kWatchdogTimerId, kWatchdogIntervalMs, nullptr);
}

LRESULT CALLBACK ChevronWndProc(HWND hwnd,
                                UINT msg,
                                WPARAM wParam,
                                LPARAM lParam) {
    switch (msg) {
        case WM_LBUTTONDOWN: {
            if (g_targetPidl && !g_menuActive) {
                // Leave the button on screen while the menu is open; the hover
                // engine is frozen (g_menuActive) so it stays put until
                // dismissed. Anchor the menu to the button itself, not the
                // folder row.
                RECT anchorRect = g_chevronRect;
                PIDLIST_ABSOLUTE pidl = ILClone(g_targetPidl);
                if (pidl) {
                    ShowFolderMenuModal(pidl, anchorRect);
                    ILFree(pidl);
                }
            }
            return 0;
        }
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////
// Hover engine: raw input driven, hit-testing against the cached item rects.

// Classifies a top-level window as one of the surfaces the mod operates on: an
// Explorer window, the desktop, or (when enabled) an open/save file dialog.
// Returns false for anything else. Cheap (a class-name check); callers that
// gate activation additionally require a file dialog to host a shell view (see
// HasShellView) so the mod does not wake for ordinary dialogs.
bool ClassifyRoot(HWND root, bool* outIsDesktop, bool* outIsDialog) {
    *outIsDesktop = false;
    *outIsDialog = false;

    WCHAR className[64];
    if (!GetClassNameW(root, className, ARRAYSIZE(className))) {
        return false;
    }

    if (wcscmp(className, L"CabinetWClass") == 0 ||
        wcscmp(className, L"ExploreWClass") == 0) {
        return true;
    }

    if (wcscmp(className, L"Progman") == 0 ||
        wcscmp(className, L"WorkerW") == 0) {
        *outIsDesktop = true;
        return true;
    }

    if (g_settings.fileDialogs && wcscmp(className, kFileDialogClass) == 0) {
        *outIsDialog = true;
        return true;
    }

    return false;
}

// True if the window hosts a shell file-list view. Used to tell an actual file
// dialog from any other dialog of the same window class.
bool HasShellView(HWND root) {
    return FindDescendantOfClass(root, L"SHELLDLL_DefView") != nullptr;
}

// True if hwnd or one of its ancestors (up to maxDepth) has the given class.
bool IsWithinClass(HWND hwnd, PCWSTR className, int maxDepth) {
    for (int i = 0; hwnd && i < maxDepth; i++) {
        WCHAR c[64];
        if (GetClassNameW(hwnd, c, ARRAYSIZE(c)) && wcscmp(c, className) == 0) {
            return true;
        }
        hwnd = GetParent(hwnd);
    }
    return false;
}

// The heart of the mod: hit-test the cursor against the current snapshot and
// show or hide the button. All UIA/shell work is delegated to the worker
// thread; this only ever does a local scan + render, so it never blocks. Called
// from raw input (mouse move / wheel), foreground changes, and worker
// refresh-done messages.
void Evaluate(bool forceRefresh) {
    if (g_menuActive || !g_active) {
        return;
    }

    POINT pt;
    if (!GetCursorPos(&pt)) {
        return;
    }

    bool onButton = g_chevronVisible && PtInRect(&g_chevronRect, pt);
    HWND tab = nullptr;
    bool isDesktop = false;

    if (onButton) {
        // The cursor is on our own button; the button still follows the item
        // under it. Hit-test the item rect's center against the view the
        // snapshot already represents (adopted under the lock below). We
        // deliberately do not use WindowFromPoint here: for a small item the
        // item center can sit under our button window, and WindowFromPoint
        // would then return our window and wrongly hide the button.
        pt.x = (g_hoverItemRect.left + g_hoverItemRect.right) / 2;
        pt.y = (g_hoverItemRect.top + g_hoverItemRect.bottom) / 2;
    } else {
        // Cheap window-level gate: only consider the file list area. The shell
        // view check (below) also confirms a #32770 is an actual file dialog,
        // so the cheap class check in ClassifyRoot is enough here.
        HWND under = WindowFromPoint(pt);
        HWND root = under ? GetAncestor(under, GA_ROOT) : nullptr;
        bool isDialog = false;
        if (!under || !root || !ClassifyRoot(root, &isDesktop, &isDialog) ||
            !IsWithinClass(under, L"SHELLDLL_DefView", 8)) {
            HideChevron();
            return;
        }
        // Identify the specific tab under the cursor so the snapshot and folder
        // lookup track it, not whichever tab of this window enumerates first.
        tab = GetExplorerTabWindow(under);
    }

    PIDLIST_ABSOLUTE childAbs = nullptr;
    RECT hitRect = {};
    bool requestRefresh = false;

    EnterCriticalSection(&g_snapshotLock);
    if (onButton) {
        if (!g_snapValid) {
            // No view to validate against; leave the button as-is.
            LeaveCriticalSection(&g_snapshotLock);
            return;
        }
        tab = g_snapTab;
        isDesktop = g_snapIsDesktop;
    }
    bool snapMatches =
        g_snapValid && g_snapTab == tab && g_snapIsDesktop == isDesktop;
    if (snapMatches) {
        for (const CachedItem& item : g_snapItems) {
            RECT r = item.rect;
            if (g_snapNameColumnRight && g_snapNameColumnRight > r.left &&
                g_snapNameColumnRight < r.right) {
                r.right = g_snapNameColumnRight;
            }
            if (PtInRect(&r, pt)) {
                auto it = g_snapChildren.find(ToLower(item.name));
                if (it != g_snapChildren.end()) {
                    childAbs = ILClone(it->second);
                    hitRect = r;
                }
                break;
            }
        }
        if (forceRefresh ||
            (GetTickCount64() - g_snapBuiltTick) > kRefreshTtlMs) {
            requestRefresh = true;
        }
    } else {
        requestRefresh = true;
    }

    if (requestRefresh) {
        g_reqTab = tab;
        g_reqIsDesktop = isDesktop;
        g_reqPoint = pt;
        g_refreshRequested = true;
    }
    LeaveCriticalSection(&g_snapshotLock);

    if (requestRefresh && g_workerThreadId) {
        PostThreadMessageW(g_workerThreadId, WM_APP_DO_REFRESH, 0, 0);
    }

    // When forcing a refresh, the snapshot just hit-tested may be stale (a
    // scroll or navigation prompted the force), so don't update the button from
    // it - leave it as-is and let the refresh-done re-evaluation apply fresh
    // data.
    if (forceRefresh) {
        if (childAbs) {
            ILFree(childAbs);
        }
        return;
    }

    if (childAbs) {
        // Remember which view the hovered folder belongs to, so a folder opened
        // from the menu can navigate "the current window" (the snapshot's tab,
        // adopted under the lock above when on the button), and whether that
        // view is a file dialog (so its menu clicks drive the dialog).
        g_targetTab = tab;
        g_targetIsDesktop = isDesktop;
        g_targetIsDialog =
            !isDesktop && IsFileDialogWindow(GetAncestor(tab, GA_ROOT));
        ShowChevronForItem(childAbs, hitRect);
    } else {
        HideChevron();
    }
}

// Subscribes to or unsubscribes from raw mouse input. While Explorer is not in
// the foreground we unregister entirely, so no WM_INPUT is delivered and the
// mod does no input handling at all.
void SetRawInputActive(bool enable) {
    if (enable == g_rawInputRegistered || !g_sinkWnd) {
        return;
    }
    RAWINPUTDEVICE rid = {};
    rid.usUsagePage = 0x01;  // Generic desktop controls.
    rid.usUsage = 0x02;      // Mouse.
    rid.dwFlags = enable ? RIDEV_INPUTSINK : RIDEV_REMOVE;
    rid.hwndTarget = enable ? g_sinkWnd : nullptr;
    if (RegisterRawInputDevices(&rid, 1, sizeof(rid))) {
        g_rawInputRegistered = enable;
    } else {
        Wh_Log(
            L"SetRawInputActive: RegisterRawInputDevices(enable=%d) failed, "
            L"le=%lu",
            (int)enable, GetLastError());
    }
}

// Re-derives whether Explorer or the desktop is the active foreground window
// and syncs raw-input registration to match. Returns the new active state. On a
// transition into the active state the hover button is hidden/shown as needed;
// the snapshot is dropped when going inactive so re-activation rebuilds it.
//
// Shared by the foreground-change hook, the periodic re-check timer, and
// startup. Classifying the *actual* current foreground (not an event hwnd) is
// what makes this self-correcting: EVENT_SYSTEM_FOREGROUND is delivered
// asynchronously (WINEVENT_OUTOFCONTEXT), and the shell's transition windows -
// the alt+tab / Task View host (XamlExplorerHostIslandWindow), the
// ForegroundStaging helper, and the windows churned during an Explorer restart
// - briefly hold the foreground, so an event can carry an already-superseded
// window. The periodic caller additionally covers the case where the settling
// event is missed entirely (notably the desktop returning after an Explorer
// restart, when the tool process keeps running and never re-inits).
bool SyncActiveState() {
    HWND fg = GetForegroundWindow();

    // Ignore our own windows (the button or a closing popup) being foreground;
    // they are part of the mod's UI, so they must not look like a switch away
    // from Explorer and deactivate the mod.
    if (fg) {
        DWORD fgProcessId = 0;
        GetWindowThreadProcessId(fg, &fgProcessId);
        if (fgProcessId == GetCurrentProcessId()) {
            return g_active;
        }
    }

    bool isDesktop = false;
    bool isDialog = false;
    // A null foreground (nobody owns it, seen mid-transition and right after an
    // Explorer restart) is treated as the desktop being the active surface.
    bool active = !fg || ClassifyRoot(fg, &isDesktop, &isDialog);
    // Don't wake for every #32770 dialog; only ones that host a file list.
    if (active && isDialog && !HasShellView(fg)) {
        active = false;
    }
    if (active == g_active) {
        return active;
    }

    g_active = active;
    SetRawInputActive(active);
    if (active) {
        Evaluate(false);
    } else {
        HideChevron();
        // Invalidate the snapshot so re-activation rebuilds from scratch.
        EnterCriticalSection(&g_snapshotLock);
        g_snapValid = false;
        LeaveCriticalSection(&g_snapshotLock);
    }
    return active;
}

// Tracks whether Explorer / the desktop is the foreground window, so raw input
// is only acted on then. Also re-checks the hover when Explorer is activated.
VOID CALLBACK ActivationChangedProc(HWINEVENTHOOK hook,
                                    DWORD event,
                                    HWND hwnd,
                                    LONG idObject,
                                    LONG idChild,
                                    DWORD idEventThread,
                                    DWORD dwmsEventTime) {
    if (g_menuActive) {
        return;
    }

    // Re-evaluate the hover whenever Explorer/the desktop is (still) active,
    // including when staying active across a switch between Explorer windows.
    // SyncActiveState already re-evaluates on the inactive->active transition,
    // so only the stay-active case is handled here, avoiding a double refresh.
    bool wasActive = g_active;
    if (SyncActiveState() && wasActive) {
        Evaluate(false);
    }
}

LRESULT CALLBACK SinkWndProc(HWND hwnd,
                             UINT msg,
                             WPARAM wParam,
                             LPARAM lParam) {
    if (msg == WM_APP_REFRESH_DONE) {
        // The worker installed a fresh snapshot; re-evaluate at the cursor.
        Evaluate(false);
        return 0;
    }

    if (msg == WM_TIMER && wParam == kWatchdogTimerId) {
        // Periodic re-check while the button is shown, to catch navigation that
        // moved no mouse (double-click / keyboard Enter).
        Evaluate(true);
        return 0;
    }

    if (g_taskbarCreatedMsg && msg == g_taskbarCreatedMsg) {
        // Explorer (re)started: the foreground hook can miss the desktop coming
        // back (its activating event is delivered already superseded), so
        // re-derive the active state here instead. This event fires once per
        // restart, so no polling is needed.
        if (!g_menuActive) {
            SyncActiveState();
        }
        return 0;
    }

    if (g_shellHookMsg && msg == g_shellHookMsg) {
        // Shell hook notification. HSHELL_WINDOWACTIVATED (optionally with the
        // HSHELL_RUDEAPPACTIVATED high bit) means the activated window changed;
        // re-derive the active state as the foreground hook does. This is the
        // signal that fires on a return to Explorer when the foreground event
        // is dropped (the disappearing-button case), since it runs through
        // different machinery and is delivered as a posted message, by which
        // point GetForegroundWindow has settled. Other codes are ignored.
        if ((int)(wParam & 0x7FFF) == HSHELL_WINDOWACTIVATED && !g_menuActive) {
            bool wasActive = g_active;
            if (SyncActiveState() && wasActive) {
                Evaluate(false);
            }
        }
        return 0;
    }

    if (msg == WM_SETTINGCHANGE) {
        // System light/dark theme changed: re-sync the cached immersive/menu
        // theme state and our dark flag now (not when a menu is opening), so
        // the menu never flashes the old theme on its first paint.
        if (lParam && wcscmp((LPCWSTR)lParam, L"ImmersiveColorSet") == 0) {
            RefreshDarkMode();
        }
        return 0;
    }

    if (msg == WM_INPUT) {
        UINT size = 0;
        if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, nullptr, &size,
                            sizeof(RAWINPUTHEADER)) == 0 &&
            size > 0 && size <= sizeof(g_rawInputBuffer) &&
            GetRawInputData((HRAWINPUT)lParam, RID_INPUT, g_rawInputBuffer,
                            &size, sizeof(RAWINPUTHEADER)) == size) {
            RAWINPUT* raw = (RAWINPUT*)g_rawInputBuffer;
            if (raw->header.dwType == RIM_TYPEMOUSE) {
                USHORT flags = raw->data.mouse.usButtonFlags;
                // The shell menu band has no native wheel support, and we
                // cannot reliably intercept the cooked WM_MOUSEWHEEL in the
                // menu's modal loop: for some folders (notably the recycle bin)
                // a nested message pump the shell runs while the menu is
                // populating dequeues and dispatches the wheel before our loop
                // sees it, so it bubbles up to the band, which ignores it. Raw
                // input is delivered to this sink window regardless of which
                // loop is pumping, so drive the scroll from here instead.
                // usButtonData carries the signed wheel delta in WHEEL_DELTA
                // units.
                if ((flags & RI_MOUSE_WHEEL) && g_menuActive) {
                    POINT cur = {};
                    GetCursorPos(&cur);
                    ScrollMenuAtPoint(cur, (SHORT)raw->data.mouse.usButtonData);
                }
                // Left click: remember which folder item the press landed on,
                // so only a release on the same item runs the action (a real
                // click, not the press that opened the menu or a drag). The
                // band keeps the menu open for a left-click, so the toolbar is
                // alive to hit-test on both the down and the up. Driven from
                // raw input so it is seen no matter which loop is pumping the
                // menu.
                if (g_menuActive && (flags & RI_MOUSE_LEFT_BUTTON_DOWN) &&
                    g_settings.clickAction != FolderAction::nothing) {
                    POINT cur = {};
                    GetCursorPos(&cur);
                    g_leftDownToolbar =
                        GetMenuFolderItemUnderCursor(cur, &g_leftDownIdCmd);
                }
                if (g_menuActive && (flags & RI_MOUSE_LEFT_BUTTON_UP) &&
                    g_settings.clickAction != FolderAction::nothing) {
                    POINT cur = {};
                    GetCursorPos(&cur);
                    int idCmd = 0;
                    HWND toolbar = GetMenuFolderItemUnderCursor(cur, &idCmd);
                    if (toolbar && toolbar == g_leftDownToolbar &&
                        idCmd == g_leftDownIdCmd) {
                        ExecMenuFolderItem(toolbar, idCmd,
                                           g_settings.clickAction);
                    }
                    g_leftDownToolbar = nullptr;
                }
                // Middle click: the band dismisses the menu on a middle press,
                // so act on the raw button-DOWN, while the menu is still alive
                // (by the button-up it is already gone).
                if (g_menuActive && (flags & RI_MOUSE_MIDDLE_BUTTON_DOWN) &&
                    g_settings.middleClickAction != FolderAction::nothing) {
                    POINT cur = {};
                    GetCursorPos(&cur);
                    int idCmd = 0;
                    HWND toolbar = GetMenuFolderItemUnderCursor(cur, &idCmd);
                    ExecMenuFolderItem(toolbar, idCmd,
                                       g_settings.middleClickAction);
                }
                // A wheel or button event may scroll or navigate, so force a
                // refresh; plain moves only refresh once the cache goes stale.
                bool force =
                    (flags &
                     (RI_MOUSE_WHEEL | RI_MOUSE_HWHEEL |
                      RI_MOUSE_LEFT_BUTTON_DOWN | RI_MOUSE_LEFT_BUTTON_UP |
                      RI_MOUSE_RIGHT_BUTTON_DOWN | RI_MOUSE_RIGHT_BUTTON_UP |
                      RI_MOUSE_MIDDLE_BUTTON_DOWN |
                      RI_MOUSE_MIDDLE_BUTTON_UP)) != 0;
                ULONGLONG now = GetTickCount64();
                if (force || now - g_lastInputTick >= kInputCoalesceMs) {
                    g_lastInputTick = now;
                    Evaluate(force);
                }
            }
        }
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////
// UI thread and tool-mod lifecycle.

DWORD WINAPI UiThreadProc(LPVOID param) {
    // Match Explorer's physical-pixel coordinate space so UI Automation
    // rectangles, the cursor, and our window all line up under mixed DPI.
    if (HMODULE user32 = GetModuleHandleW(L"user32.dll")) {
        auto pSetThreadDpiAwarenessContext =
            (void*(WINAPI*)(void*))GetProcAddress(
                user32, "SetThreadDpiAwarenessContext");
        if (pSetThreadDpiAwarenessContext) {
            // DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2.
            pSetThreadDpiAwarenessContext((void*)(LONG_PTR)-4);
        }
    }

    OleInitialize(nullptr);
    InitDarkMode();

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, nullptr);

    WNDCLASSEXW wcChevron = {sizeof(wcChevron)};
    wcChevron.lpfnWndProc = ChevronWndProc;
    wcChevron.hInstance = g_hInst;
    wcChevron.hCursor = LoadCursorW(nullptr, IDC_HAND);
    wcChevron.lpszClassName = L"WH_FolderHoverChevron";
    RegisterClassExW(&wcChevron);

    WNDCLASSEXW wcSink = {sizeof(wcSink)};
    wcSink.lpfnWndProc = SinkWndProc;
    wcSink.hInstance = g_hInst;
    wcSink.lpszClassName = L"WH_FolderHoverSink";
    RegisterClassExW(&wcSink);

    // Per-pixel alpha (WS_EX_LAYERED + UpdateLayeredWindow) for an antialiased,
    // rounded button; content is supplied by RenderChevron.
    g_chevronWnd = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_LAYERED,
        wcChevron.lpszClassName, L"", WS_POPUP, 0, 0, g_settings.iconSize,
        g_settings.iconSize, nullptr, nullptr, g_hInst, nullptr);

    // Hidden window that receives raw mouse input (movement + wheel) even when
    // in the background, replacing any polling. Raw input is only registered
    // while Explorer/the desktop is the foreground window (see
    // SetRawInputActive).
    g_sinkWnd =
        CreateWindowExW(WS_EX_TOOLWINDOW, wcSink.lpszClassName, L"", WS_POPUP,
                        0, 0, 0, 0, nullptr, nullptr, g_hInst, nullptr);

    // Start the worker thread that does all UIA + shell work off this thread.
    g_workerReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0,
                                  &g_workerThreadId);
    if (g_workerThread && g_workerReadyEvent) {
        WaitForSingleObject(g_workerReadyEvent, 5000);
    }
    if (g_workerReadyEvent) {
        CloseHandle(g_workerReadyEvent);
        g_workerReadyEvent = nullptr;
    }

    // Start active if Explorer or the desktop is already in the foreground,
    // instead of waiting for the next foreground change.
    SyncActiveState();
    g_foregroundHook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
        ActivationChangedProc, 0, 0, WINEVENT_OUTOFCONTEXT);

    // Listen for the shell's "TaskbarCreated" broadcast so an Explorer restart
    // re-derives the active state (see SinkWndProc), recovering the desktop
    // without polling.
    g_taskbarCreatedMsg = RegisterWindowMessageW(L"TaskbarCreated");
    if (g_taskbarCreatedMsg) {
        // Let the broadcast through UIPI in case this process runs at a higher
        // integrity level than Explorer (which sends it).
        ChangeWindowMessageFilterEx(g_sinkWnd, g_taskbarCreatedMsg,
                                    MSGFLT_ALLOW, nullptr);
    }

    // Register for shell hook notifications (HSHELL_WINDOWACTIVATED etc.) as an
    // additional activation signal. The genuine return to Explorer after the
    // modern context menu emits no foreground or focus event (see SinkWndProc),
    // and the shell hook runs through different machinery, so it may fire
    // there.
    g_shellHookMsg = RegisterWindowMessageW(L"SHELLHOOK");
    if (g_shellHookMsg) {
        // Allow it through UIPI, like TaskbarCreated above.
        ChangeWindowMessageFilterEx(g_sinkWnd, g_shellHookMsg, MSGFLT_ALLOW,
                                    nullptr);
    }
    if (!RegisterShellHookWindow(g_sinkWnd)) {
        Wh_Log(L"RegisterShellHookWindow failed, le=%lu", GetLastError());
    }

    // The menu band's own "execute item" message. Posting it to an item toolbar
    // is how a folder click opens the item (see ExecMenuFolderItem).
    g_cmbExecuteMsg = RegisterWindowMessageW(L"CMBExecute");

    SetEvent(g_readyEvent);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.hwnd == nullptr && msg.message == WM_APP_QUIT) {
            PostQuitMessage(0);
            continue;
        }
        if (msg.hwnd == nullptr && msg.message == WM_APP_SETTINGS_CHANGED) {
            LoadSettings();
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_foregroundHook) {
        UnhookWinEvent(g_foregroundHook);
        g_foregroundHook = nullptr;
    }
    SetRawInputActive(false);

    // Stop the worker before tearing down windows it posts to.
    if (g_workerThreadId) {
        PostThreadMessageW(g_workerThreadId, WM_APP_QUIT, 0, 0);
    }
    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, 5000);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
        g_workerThreadId = 0;
    }

    // The worker is gone; free the snapshot's child pidls it owned.
    for (auto& entry : g_snapChildren) {
        ILFree(entry.second);
    }
    g_snapChildren.clear();

    if (g_sinkWnd) {
        DeregisterShellHookWindow(g_sinkWnd);
        DestroyWindow(g_sinkWnd);
        g_sinkWnd = nullptr;
    }
    if (g_chevronWnd) {
        DestroyWindow(g_chevronWnd);
        g_chevronWnd = nullptr;
    }
    if (g_targetPidl) {
        ILFree(g_targetPidl);
        g_targetPidl = nullptr;
    }
    if (g_gdiplusToken) {
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
    OleUninitialize();
    return 0;
}

BOOL WhTool_ModInit() {
    g_hInst = GetModuleHandleW(nullptr);

    LoadSettings();

    InitMenuColorHooks();
    InitEnumTimeoutHooks();

    InitializeCriticalSection(&g_snapshotLock);

    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_readyEvent) {
        Wh_Log(L"CreateEvent failed");
        DeleteCriticalSection(&g_snapshotLock);
        return FALSE;
    }

    g_uiThread =
        CreateThread(nullptr, 0, UiThreadProc, nullptr, 0, &g_uiThreadId);
    if (!g_uiThread) {
        Wh_Log(L"CreateThread failed");
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
        DeleteCriticalSection(&g_snapshotLock);
        return FALSE;
    }

    if (WaitForSingleObject(g_readyEvent, 5000) != WAIT_OBJECT_0) {
        Wh_Log(L"UI thread did not signal ready in time");
    }
    CloseHandle(g_readyEvent);
    g_readyEvent = nullptr;
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    if (g_uiThreadId) {
        PostThreadMessageW(g_uiThreadId, WM_APP_SETTINGS_CHANGED, 0, 0);
    }
}

void WhTool_ModUninit() {
    if (g_uiThreadId) {
        PostThreadMessageW(g_uiThreadId, WM_APP_QUIT, 0, 0);
    }
    if (g_uiThread) {
        if (WaitForSingleObject(g_uiThread, 5000) != WAIT_OBJECT_0) {
            Wh_Log(L"UI thread did not exit in time");
            ExitProcess(1);
        }
        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
        g_uiThreadId = 0;
    }

    DeleteCriticalSection(&g_snapshotLock);
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

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
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

void Wh_ModAfterInit() {
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

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
