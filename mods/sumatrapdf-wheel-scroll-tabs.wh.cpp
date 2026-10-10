// ==WindhawkMod==
// @id              sumatrapdf-wheel-scroll-tabs
// @name            SumatraPDF scroll tabs with mouse wheel
// @description     Use the mouse wheel while hovering over the tab bar to switch between tabs in SumatraPDF
// @version         1.0.0
// @author          Martin Kaiser
// @github          https://github.com/Martin-Kaiser-0
// @include         SumatraPDF*.exe
// @compilerOptions -lcomctl32 -lversion
// @license         GPL-3.0
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// Based on the mod "Chrome/Edge scroll tabs with mouse wheel"
// (chrome-wheel-scroll-tabs) by m417z, https://m417z.com/, adapted for
// SumatraPDF, https://www.sumatrapdfreader.org/.

// ==WindhawkModReadme==
/*
# SumatraPDF scroll tabs with mouse wheel

![demonstration](https://i.imgur.com/xJ4gxoL.gif)

Use the mouse wheel while hovering over the tab bar of SumatraPDF to switch
between tabs, the same way the "Chrome/Edge scroll tabs with mouse wheel" mod
does it for browsers.

* Scrolling down switches to the next tab (to the right), scrolling up to the
  previous tab. SumatraPDF wraps around at both ends. The direction can be
  reversed in the settings.
* Only plain wheel events over the tab bar are intercepted. Ctrl+wheel (zoom),
  Shift+wheel (horizontal scrolling) and wheel events anywhere else are left to
  SumatraPDF.
* Works with the tab bar in SumatraPDF's own title bar (the default with
  `UseTabs = true`) as well as with a plain tab bar below the menu.
* Both the installed (`SumatraPDF.exe`) and the portable
  (`SumatraPDF-x.y.z-64.exe`) builds are covered.

## How it works

SumatraPDF 3.5 and newer have "Next Tab" / "Previous Tab" commands, bound to
Ctrl+PageDown / Ctrl+PageUp by default. The mod reads the ids of these commands
from the accelerator table SumatraPDF registers and posts the matching command
to the main window, so a wheel notch does exactly the same as pressing the
shortcut.

This means the mod relies on Ctrl+PageDown / Ctrl+PageUp being bound to
"Next Tab" / "Previous Tab". If you rebound these shortcuts to something else
in the advanced settings (`Shortcuts`), the wheel triggers whatever they are
bound to now.

SumatraPDF versions before 3.5 aren't supported: they have no tab switching
commands and use Ctrl+PageDown / Ctrl+PageUp for page navigation. The mod stays
inactive there.

## Credits

Based on the [Chrome/Edge scroll tabs with mouse wheel](https://windhawk.net/mods/chrome-wheel-scroll-tabs)
mod by m417z (GPL-3.0).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- reverseScrollingDirection: false
  $name: Reverse scrolling direction
  $description: >-
    By default, scrolling down (or tilting the wheel to the right) switches to
    the next tab and scrolling up (or tilting to the left) to the previous tab.
- horizontalScrolling: false
  $name: Horizontal scrolling
  $description: >-
    Switch between tabs when the mouse's horizontal scroll wheel is tilted or
    rotated.
- throttleMs: 0
  $name: Throttle time (milliseconds)
  $description: >-
    Prevents new actions from being triggered for this amount of time after the
    last one. Set to 0 to disable throttling. Useful for preventing a single
    scroll wheel 'flick' from switching multiple tabs.
*/
// ==/WindhawkModSettings==

#include <commctrl.h>
#include <windowsx.h>

#include <windhawk_utils.h>

#include <atomic>
#include <mutex>
#include <vector>

struct {
    bool reverseScrollingDirection;
    bool horizontalScrolling;
    int throttleMs;
} g_settings;

// Window classes of SumatraPDF's main window and of the document canvas
// (FRAME_CLASS_NAME / CANVAS_CLASS_NAME in SumatraPDF 3.x, kFrameClassName /
// kCanvasClassName in the current source).
constexpr PCWSTR kFrameClassName = L"SUMATRA_PDF_FRAME";
constexpr PCWSTR kCanvasClassName = L"SUMATRA_PDF_CANVAS";

// Window classes of the tab bar. SumatraPDF 3.x uses a custom drawn standard
// tab control (src/wingui/TabsCtrl.cpp), the current pre-release source has its
// own window class (kTabsCtrlClassName in src/gui/win/TabsCtrl.cpp).
constexpr PCWSTR kTabBarClassNames[] = {
    WC_TABCONTROLW,  // "SysTabControl32"
    L"SumatraTabsCtrlClass",
};

std::atomic<DWORD> g_uiThreadId;
DWORD g_lastScrollTime;
HWND g_lastScrollWnd;
bool g_lastScrollHorizontal;
short g_lastScrollDeltaRemainder;
DWORD g_lastActionTime;

// Command ids of "Next Tab" / "Previous Tab". They differ between SumatraPDF
// versions, so they're detected from the accelerator tables. SumatraPDF
// rebuilds its tables on every settings reload, and ids of commands defined in
// the "Shortcuts" advanced setting are assigned anew each time. Therefore the
// ids are resolved from every table that is created, and they're invalidated
// when the table they came from is destroyed, so a rebuild that no longer
// binds the shortcuts leaves the mod inactive instead of posting a stale id.
// The hooks feeding the detection are process-wide, hence the mutex.
// g_tabCommandsResolved doubles as the lock-free fast path.
std::atomic<bool> g_tabCommandsResolved;
std::mutex g_tabCommandsMutex;
WORD g_cmdNextTab;
WORD g_cmdPrevTab;
// The table that most recently resolved the ids. Several of SumatraPDF's
// tables contain the shortcuts, and all of them are destroyed together before
// a rebuild, so it doesn't matter which one it is.
HACCEL g_resolvedFromTable;
// Tables already scanned via their handle. Only used while the ids aren't
// known yet, e.g. when the mod was loaded into a running SumatraPDF.
std::vector<HACCEL> g_scannedAccelTables;

// Reads the file version (major.minor) of the executable of the current
// process, i.e. the SumatraPDF version.
bool GetProcessFileVersion(WORD* major, WORD* minor) {
    WCHAR path[MAX_PATH];
    DWORD pathLen = GetModuleFileName(nullptr, path, ARRAYSIZE(path));
    if (pathLen == 0 || pathLen >= ARRAYSIZE(path)) {
        return false;
    }

    DWORD handle = 0;
    DWORD size = GetFileVersionInfoSize(path, &handle);
    if (size == 0) {
        return false;
    }

    std::vector<BYTE> buffer(size);
    if (!GetFileVersionInfo(path, 0, size, buffer.data())) {
        return false;
    }

    VS_FIXEDFILEINFO* info = nullptr;
    UINT infoLen = 0;
    if (!VerQueryValue(buffer.data(), L"\\", (void**)&info, &infoLen) ||
        !info || infoLen < sizeof(VS_FIXEDFILEINFO)) {
        return false;
    }

    *major = HIWORD(info->dwFileVersionMS);
    *minor = LOWORD(info->dwFileVersionMS);
    return true;
}

// Looks for the Ctrl+PageDown / Ctrl+PageUp entries, which SumatraPDF 3.5+
// binds to the "Next Tab" / "Previous Tab" commands. hAccel is the table the
// entries belong to. Must be called with g_tabCommandsMutex held.
bool ResolveTabCommandsFromAccels(const ACCEL* accels,
                                  int count,
                                  HACCEL hAccel) {
    WORD next = 0;
    WORD prev = 0;
    for (int i = 0; i < count; i++) {
        const ACCEL& accel = accels[i];
        if ((accel.fVirt & (FVIRTKEY | FCONTROL | FSHIFT | FALT)) !=
            (FVIRTKEY | FCONTROL)) {
            continue;
        }

        if (accel.key == VK_NEXT) {
            next = accel.cmd;
        } else if (accel.key == VK_PRIOR) {
            prev = accel.cmd;
        }
    }

    if (!next || !prev) {
        // Expected for SumatraPDF's reduced tables (edit control, tree view),
        // which are created after the main one. Only if nothing resolved the
        // ids so far is this worth a diagnostic, and then what's bound to
        // PageUp/PageDown is the useful part.
        if (!g_tabCommandsResolved.load(std::memory_order_relaxed)) {
            Wh_Log(L"Tab commands not found in table with %d entries", count);
            for (int i = 0; i < count; i++) {
                if (accels[i].key == VK_NEXT || accels[i].key == VK_PRIOR) {
                    Wh_Log(L"  fVirt=%02X key=%04X cmd=%u", accels[i].fVirt,
                           accels[i].key, accels[i].cmd);
                }
            }
        }
        return false;
    }

    g_cmdNextTab = next;
    g_cmdPrevTab = prev;
    g_resolvedFromTable = hAccel;
    g_tabCommandsResolved.store(true, std::memory_order_release);
    Wh_Log(L"Tab commands resolved: next=%u, prev=%u", next, prev);
    return true;
}

// Used when the mod is loaded into an already running SumatraPDF, where the
// table creation was missed and only the handle is available.
void ResolveTabCommandsFromTable(HACCEL hAccel) {
    std::lock_guard<std::mutex> lock(g_tabCommandsMutex);

    if (g_tabCommandsResolved.load(std::memory_order_relaxed)) {
        return;
    }

    // Don't scan the same table over and over again. SumatraPDF uses several
    // tables (e.g. a reduced one while an edit control has the focus), not all
    // of them contain the tab shortcuts. A table is removed from the list when
    // it's destroyed (see DestroyAcceleratorTableHook), since its handle value
    // may be reused.
    for (HACCEL hScanned : g_scannedAccelTables) {
        if (hScanned == hAccel) {
            return;
        }
    }

    g_scannedAccelTables.push_back(hAccel);

    int count = CopyAcceleratorTable(hAccel, nullptr, 0);
    if (count <= 0) {
        return;
    }

    std::vector<ACCEL> accels(count);
    count = CopyAcceleratorTable(hAccel, accels.data(), count);
    if (count <= 0) {
        return;
    }

    ResolveTabCommandsFromAccels(accels.data(), count, hAccel);
}

struct TabBarInfo {
    HWND hWnd;
    // True for the standard tab control (SysTabControl32), which answers tab
    // control messages. The pre-release class doesn't.
    bool isStandardTabControl;
};

bool IsTabBarWindow(HWND hWnd, bool* isStandardTabControl) {
    WCHAR windowClassName[64];
    if (!GetClassName(hWnd, windowClassName, ARRAYSIZE(windowClassName))) {
        return false;
    }

    for (PCWSTR className : kTabBarClassNames) {
        if (_wcsicmp(windowClassName, className) == 0) {
            *isStandardTabControl =
                _wcsicmp(windowClassName, WC_TABCONTROLW) == 0;
            return true;
        }
    }

    return false;
}

BOOL CALLBACK FindTabBarEnumFunc(HWND hWnd, LPARAM lParam) {
    bool isStandardTabControl = false;
    if (IsTabBarWindow(hWnd, &isStandardTabControl) && IsWindowVisible(hWnd)) {
        TabBarInfo* info = (TabBarInfo*)lParam;
        info->hWnd = hWnd;
        info->isStandardTabControl = isStandardTabControl;
        return FALSE;
    }

    return TRUE;
}

// The tab bar is a child of the main window, or of the custom caption window
// when the tabs are shown in the title bar. It's hidden when tabs are disabled
// or (depending on the version) when only a single document is open.
TabBarInfo FindVisibleTabBar(HWND hFrameWnd) {
    TabBarInfo info{};
    EnumChildWindows(hFrameWnd, FindTabBarEnumFunc, (LPARAM)&info);
    return info;
}

bool IsCanvasWindow(HWND hWnd) {
    WCHAR windowClassName[64];
    if (!GetClassName(hWnd, windowClassName, ARRAYSIZE(windowClassName))) {
        return false;
    }

    return _wcsicmp(windowClassName, kCanvasClassName) == 0;
}

bool PostTabCommand(HWND hFrameWnd, WORD cmdId) {
    Wh_Log(L"Posting command %u to window %08X", cmdId,
           (DWORD)(ULONG_PTR)hFrameWnd);

    // This is what TranslateAccelerator does for Ctrl+PageDown/PageUp. The
    // message is posted rather than sent so that the tab switch runs from the
    // message loop and not nested inside the mouse wheel handling.
    return !!PostMessage(hFrameWnd, WM_COMMAND, MAKEWPARAM(cmdId, 1), 0);
}

bool OnMouseWheel(HWND hWnd,
                  bool horizontal,
                  WORD keys,
                  short delta,
                  int xPos,
                  int yPos) {
    if (keys) {
        // A modifier key or mouse button is held down, e.g. Ctrl+wheel (zoom)
        // or Shift+wheel (horizontal scrolling). Leave that to SumatraPDF.
        return false;
    }

    if (!IsWindowEnabled(hWnd)) {
        // A modal dialog is open. SumatraPDF wouldn't act on the shortcut
        // either, so don't switch tabs behind the dialog.
        return false;
    }

    // The main window gets every wheel event, most of them are plain document
    // scrolling. Two cheap checks before walking the child windows: the
    // window under the cursor must belong to this main window at all (with
    // the legacy "scroll the focused window" setting, the event arrives here
    // even if another window overlaps the tab bar), and if it's the canvas,
    // the cursor isn't over the tab bar.
    POINT pt{xPos, yPos};
    HWND hUnderCursor = WindowFromPoint(pt);
    if (!hUnderCursor || GetAncestor(hUnderCursor, GA_ROOT) != hWnd ||
        IsCanvasWindow(hUnderCursor)) {
        return false;
    }

    if (GetKeyState(VK_MENU) < 0 || GetKeyState(VK_LWIN) < 0 ||
        GetKeyState(VK_RWIN) < 0) {
        return false;
    }

    if (!g_tabCommandsResolved.load(std::memory_order_acquire)) {
        Wh_Log(L"Tab command ids aren't known (yet), leaving the event alone");
        return false;
    }

    // The window under the cursor isn't necessarily the tab bar itself: with
    // tabs in the title bar, the area next to the tabs hit-tests as
    // transparent so that the window can be dragged there. Like in the
    // browser mod, the whole tab bar rectangle counts. (The caption window,
    // and with it the tab bar, ends where the caption buttons begin.)
    TabBarInfo tabBar = FindVisibleTabBar(hWnd);
    if (!tabBar.hWnd) {
        return false;
    }

    RECT rect{};
    if (!GetWindowRect(tabBar.hWnd, &rect)) {
        return false;
    }

    if (!PtInRect(&rect, pt)) {
        return false;
    }

    // With a single tab there's nothing to switch to, leave the event to
    // SumatraPDF (which scrolls the document). Only the standard tab control
    // is asked - TCM_GETITEMCOUNT is a WM_USER range message that another
    // class may interpret differently.
    if (tabBar.isStandardTabControl &&
        SendMessage(tabBar.hWnd, TCM_GETITEMCOUNT, 0, 0) <= 1) {
        return false;
    }

    WORD cmdNextTab;
    WORD cmdPrevTab;
    {
        std::lock_guard<std::mutex> lock(g_tabCommandsMutex);
        cmdNextTab = g_cmdNextTab;
        cmdPrevTab = g_cmdPrevTab;
    }

    if (hWnd == g_lastScrollWnd && horizontal == g_lastScrollHorizontal &&
        GetTickCount() - g_lastScrollTime < 1000 * 5) {
        delta += g_lastScrollDeltaRemainder;
    }

    int clicks = delta / WHEEL_DELTA;
    Wh_Log(L"%d clicks (delta=%d)", clicks, delta);

    if (clicks != 0 && g_settings.throttleMs > 0) {
        if (GetTickCount() - g_lastActionTime < (DWORD)g_settings.throttleMs) {
            // It's too soon, ignore this scroll event.
            clicks = 0;

            // Reset remainder too.
            delta = 0;
        } else if (clicks < -1 || clicks > 1) {
            // Throttle to a single action at a time.
            clicks = clicks > 0 ? 1 : -1;

            // Reset remainder if going too fast.
            delta = 0;
        }
    }

    WORD cmdId = cmdNextTab;
    if (clicks < 0) {
        clicks = -clicks;
        cmdId = cmdPrevTab;
    }

    if (clicks > 0) {
        for (int i = 0; i < clicks; i++) {
            if (!PostTabCommand(hWnd, cmdId)) {
                break;
            }
        }

        g_lastActionTime = GetTickCount();
    }

    g_lastScrollTime = GetTickCount();
    g_lastScrollWnd = hWnd;
    g_lastScrollHorizontal = horizontal;
    g_lastScrollDeltaRemainder = delta % WHEEL_DELTA;

    return true;
}

// Mouse wheel messages end up in the main window in both cases: when it has
// the keyboard focus (SumatraPDF's canvas never has it), and when Windows
// delivers the message to the tab bar under the mouse, since the tab control
// doesn't handle it and DefWindowProc forwards it to the parent.
LRESULT CALLBACK FrameWindowSubclassProc(HWND hWnd,
                                         UINT uMsg,
                                         WPARAM wParam,
                                         LPARAM lParam,
                                         DWORD_PTR dwRefData) {
    switch (uMsg) {
        case WM_MOUSEWHEEL:
        case WM_MOUSEHWHEEL: {
            bool horizontal = uMsg == WM_MOUSEHWHEEL;
            WORD fwKeys = GET_KEYSTATE_WPARAM(wParam);
            short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);

            if (horizontal) {
                if (!g_settings.horizontalScrolling) {
                    break;
                }

                // For horizontal scrolling, a large delta value might be posted
                // for a single click (e.g. 480). Limit the value to 120.
                if (zDelta < -120) {
                    zDelta = -120;
                } else if (zDelta > 120) {
                    zDelta = 120;
                }

                // Tilting to the right (positive delta) means next tab.
                if (g_settings.reverseScrollingDirection) {
                    zDelta = -zDelta;
                }
            } else if (!g_settings.reverseScrollingDirection) {
                // Scrolling down (negative delta) means next tab.
                zDelta = -zDelta;
            }

            if (OnMouseWheel(hWnd, horizontal, fwKeys, zDelta, xPos, yPos)) {
                return 0;
            }
            break;
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

bool IsSumatraFrameWindow(HWND hWnd) {
    WCHAR windowClassName[64];
    if (!GetClassName(hWnd, windowClassName, ARRAYSIZE(windowClassName))) {
        return false;
    }

    return _wcsicmp(windowClassName, kFrameClassName) == 0;
}

BOOL CALLBACK InitialEnumFrameWindowsFunc(HWND hWnd, LPARAM lParam) {
    DWORD dwProcessId = 0;
    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, &dwProcessId);
    if (!dwThreadId || dwProcessId != GetCurrentProcessId()) {
        return TRUE;
    }

    DWORD uiThreadId = g_uiThreadId.load();
    if (uiThreadId && uiThreadId != dwThreadId) {
        return TRUE;
    }

    if (IsSumatraFrameWindow(hWnd)) {
        Wh_Log(L"SumatraPDF window found: %08X", (DWORD)(ULONG_PTR)hWnd);

        if (!uiThreadId) {
            g_uiThreadId.store(dwThreadId);
        }

        WindhawkUtils::SetWindowSubclassFromAnyThread(
            hWnd, FrameWindowSubclassProc, 0);
    }

    return TRUE;
}

BOOL CALLBACK EnumFrameWindowsUnsubclassFunc(HWND hWnd, LPARAM lParam) {
    if (IsSumatraFrameWindow(hWnd)) {
        Wh_Log(L"SumatraPDF window to unsubclass: %08X",
               (DWORD)(ULONG_PTR)hWnd);

        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            hWnd, FrameWindowSubclassProc);
    }

    return TRUE;
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t pOriginalCreateWindowExW;
HWND WINAPI CreateWindowExWHook(DWORD dwExStyle,
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
    HWND hWnd = pOriginalCreateWindowExW(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);

    if (!hWnd) {
        return hWnd;
    }

    DWORD uiThreadId = g_uiThreadId.load();
    if (uiThreadId && uiThreadId != GetCurrentThreadId()) {
        return hWnd;
    }

    if (IsSumatraFrameWindow(hWnd)) {
        Wh_Log(L"SumatraPDF window created: %08X", (DWORD)(ULONG_PTR)hWnd);

        if (!uiThreadId) {
            g_uiThreadId.store(GetCurrentThreadId());
        }

        WindhawkUtils::SetWindowSubclassFromAnyThread(
            hWnd, FrameWindowSubclassProc, 0);
    }

    return hWnd;
}

// SumatraPDF creates its accelerator tables on startup and again whenever the
// advanced settings change. Scanning the entries right here makes the command
// ids known before the first wheel event, and keeps them current across
// rebuilds: ids of commands defined in the "Shortcuts" advanced setting are
// assigned anew on every reload, and a custom shortcut on Ctrl+PageDown /
// Ctrl+PageUp takes precedence over the built-in one.
using CreateAcceleratorTableW_t = decltype(&CreateAcceleratorTableW);
CreateAcceleratorTableW_t pOriginalCreateAcceleratorTableW;
HACCEL WINAPI CreateAcceleratorTableWHook(LPACCEL paccel, int cAccel) {
    HACCEL hAccel = pOriginalCreateAcceleratorTableW(paccel, cAccel);

    if (hAccel && paccel && cAccel > 0) {
        std::lock_guard<std::mutex> lock(g_tabCommandsMutex);

        // Only a table that contains both shortcuts updates the ids, the
        // reduced tables leave them alone.
        ResolveTabCommandsFromAccels(paccel, cAccel, hAccel);
    }

    return hAccel;
}

// SumatraPDF destroys its tables before rebuilding them. Forget the ids that
// came from a destroyed table: if the new table binds the shortcuts again,
// they're resolved afresh, otherwise the mod stays inactive.
using DestroyAcceleratorTable_t = decltype(&DestroyAcceleratorTable);
DestroyAcceleratorTable_t pOriginalDestroyAcceleratorTable;
BOOL WINAPI DestroyAcceleratorTableHook(HACCEL hAccel) {
    if (hAccel) {
        std::lock_guard<std::mutex> lock(g_tabCommandsMutex);

        if (hAccel == g_resolvedFromTable) {
            Wh_Log(L"Table the tab commands came from is being destroyed");
            g_resolvedFromTable = nullptr;
            g_tabCommandsResolved.store(false, std::memory_order_release);
        }

        std::erase(g_scannedAccelTables, hAccel);
    }

    return pOriginalDestroyAcceleratorTable(hAccel);
}

// Covers the case that the mod is loaded into an already running SumatraPDF:
// the message loop calls TranslateAccelerator for every key and mouse message
// of the main window.
using TranslateAcceleratorW_t = decltype(&TranslateAcceleratorW);
TranslateAcceleratorW_t pOriginalTranslateAcceleratorW;
int WINAPI TranslateAcceleratorWHook(HWND hWnd, HACCEL hAccTable, LPMSG lpMsg) {
    if (hAccTable && !g_tabCommandsResolved.load(std::memory_order_acquire)) {
        ResolveTabCommandsFromTable(hAccTable);
    }

    return pOriginalTranslateAcceleratorW(hWnd, hAccTable, lpMsg);
}

void LoadSettings() {
    g_settings.reverseScrollingDirection =
        Wh_GetIntSetting(L"reverseScrollingDirection");
    g_settings.horizontalScrolling = Wh_GetIntSetting(L"horizontalScrolling");
    g_settings.throttleMs = Wh_GetIntSetting(L"throttleMs");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    WORD versionMajor = 0;
    WORD versionMinor = 0;
    if (GetProcessFileVersion(&versionMajor, &versionMinor)) {
        Wh_Log(L"SumatraPDF version %u.%u", versionMajor, versionMinor);
        if (versionMajor < 3 || (versionMajor == 3 && versionMinor < 5)) {
            // Before 3.5 there are no tab switching commands, and Ctrl+PageDown
            // / Ctrl+PageUp navigate pages instead, so the detection would pick
            // up the wrong commands.
            Wh_Log(L"SumatraPDF 3.5 or newer is required, not loading");
            return FALSE;
        }
    } else {
        Wh_Log(L"Failed to read the file version, assuming 3.5 or newer");
    }

    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExWHook,
                                   &pOriginalCreateWindowExW);
    WindhawkUtils::SetFunctionHook(CreateAcceleratorTableW,
                                   CreateAcceleratorTableWHook,
                                   &pOriginalCreateAcceleratorTableW);
    WindhawkUtils::SetFunctionHook(DestroyAcceleratorTable,
                                   DestroyAcceleratorTableHook,
                                   &pOriginalDestroyAcceleratorTable);
    WindhawkUtils::SetFunctionHook(TranslateAcceleratorW,
                                   TranslateAcceleratorWHook,
                                   &pOriginalTranslateAcceleratorW);

    EnumWindows(InitialEnumFrameWindowsFunc, 0);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");

    DWORD uiThreadId = g_uiThreadId.load();
    if (uiThreadId != 0) {
        EnumThreadWindows(uiThreadId, EnumFrameWindowsUnsubclassFunc, 0);
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}
