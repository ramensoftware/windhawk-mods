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

Use the mouse wheel while hovering over the tab bar of SumatraPDF to switch
between tabs, the same way the "Chrome/Edge scroll tabs with mouse wheel" mod
does it for browsers.

* Scrolling down switches to the next tab (to the right), scrolling up to the
  previous tab, wrapping around at both ends. The direction can be reversed in
  the settings.
* Only plain wheel events over the tab bar are intercepted. Ctrl+wheel (zoom),
  Shift+wheel (horizontal scrolling) and wheel events anywhere else are left to
  SumatraPDF.
* Works with the regular tab bar as well as with tabs in the title bar
  (`UseTabs` / `TabsInTitlebar` advanced settings).
* Both the installed (`SumatraPDF.exe`) and the portable
  (`SumatraPDF-x.y.z-64.exe`) builds are covered.

## How it works

SumatraPDF 3.5 and newer have "Next Tab" / "Previous Tab" commands, bound to
Ctrl+PageDown / Ctrl+PageUp by default. The mod reads the ids of these commands
from the accelerator table SumatraPDF registers and posts the matching command
to the main window, so a wheel notch does exactly the same as pressing the
shortcut. If you remapped Ctrl+PageDown / Ctrl+PageUp in the advanced settings,
the command ids can be set manually in the mod settings.

Versions before 3.5 (3.1 - 3.4) don't have these commands, but react to
Ctrl+Tab / Ctrl+Shift+Tab. For them the mod simulates that keyboard shortcut.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- reverseScrollingDirection: false
  $name: Reverse scrolling direction
  $description: >-
    By default, scrolling down switches to the next tab (to the right) and
    scrolling up to the previous tab.
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
- commandIds:
  - nextTab: 0
    $name: Next Tab
  - prevTab: 0
    $name: Previous Tab
  $name: Command ids (advanced)
  $description: >-
    The mod normally detects the ids of SumatraPDF's "Next Tab" / "Previous Tab"
    commands automatically from the Ctrl+PageDown / Ctrl+PageUp shortcuts. Only
    set both values if the automatic detection doesn't work for you, e.g.
    because you remapped these shortcuts. Set to 0 for automatic detection.
*/
// ==/WindhawkModSettings==

#include <commctrl.h>
#include <windowsx.h>

#include <vector>

struct {
    bool reverseScrollingDirection;
    bool horizontalScrolling;
    int throttleMs;
    int commandIdNextTab;
    int commandIdPrevTab;
} g_settings;

// Window class of SumatraPDF's main window.
constexpr PCWSTR kFrameClassName = L"SUMATRA_PDF_FRAME";

// Window classes of the tab bar. SumatraPDF 3.x uses a custom drawn standard
// tab control, the 3.7 pre-release builds use their own window class.
constexpr PCWSTR kTabBarClassNames[] = {
    WC_TABCONTROLW,  // "SysTabControl32"
    L"SumatraTabsCtrlClass",
};

DWORD g_uiThreadId;
DWORD g_lastScrollTime;
HWND g_lastScrollWnd;
short g_lastScrollDeltaRemainder;
DWORD g_lastActionTime;

// Command ids of "Next Tab" / "Previous Tab", detected from the accelerator
// table (SumatraPDF 3.5 and newer). The ids change between versions.
WORD g_cmdNextTab;
WORD g_cmdPrevTab;
HACCEL g_scannedAccelTables[8];
int g_scannedAccelTablesCount;

// SumatraPDF before 3.5 has no "Next Tab" command (Ctrl+PageDown/PageUp go to
// the next/previous page there), but its key handler reacts to Ctrl+Tab. For
// these versions keyboard input is simulated instead, with the Ctrl (and
// Shift) key state faked via a GetKeyState hook.
bool g_legacyKeyboardMode;
thread_local bool g_simulateKeys;
thread_local bool g_simulateShiftKeyDown;

// wParam - TRUE to subclass, FALSE to unsubclass
// lParam - subclass data
UINT g_subclassRegisteredMsg = RegisterWindowMessage(
    L"Windhawk_SetWindowSubclassFromAnyThread_sumatrapdf-wheel-scroll-tabs");

struct SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM {
    SUBCLASSPROC pfnSubclass;
    UINT_PTR uIdSubclass;
    DWORD_PTR dwRefData;
    BOOL result;
};

LRESULT CALLBACK CallWndProcForWindowSubclass(int nCode,
                                              WPARAM wParam,
                                              LPARAM lParam) {
    if (nCode == HC_ACTION) {
        const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
        if (cwp->message == g_subclassRegisteredMsg && cwp->wParam) {
            SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM* param =
                (SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM*)cwp->lParam;
            param->result =
                SetWindowSubclass(cwp->hwnd, param->pfnSubclass,
                                  param->uIdSubclass, param->dwRefData);
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

BOOL SetWindowSubclassFromAnyThread(HWND hWnd,
                                    SUBCLASSPROC pfnSubclass,
                                    UINT_PTR uIdSubclass,
                                    DWORD_PTR dwRefData) {
    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return FALSE;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        return SetWindowSubclass(hWnd, pfnSubclass, uIdSubclass, dwRefData);
    }

    HHOOK hook = SetWindowsHookEx(WH_CALLWNDPROC, CallWndProcForWindowSubclass,
                                  nullptr, dwThreadId);
    if (!hook) {
        return FALSE;
    }

    SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM param;
    param.pfnSubclass = pfnSubclass;
    param.uIdSubclass = uIdSubclass;
    param.dwRefData = dwRefData;
    param.result = FALSE;
    SendMessage(hWnd, g_subclassRegisteredMsg, TRUE, (LPARAM)&param);

    UnhookWindowsHookEx(hook);

    return param.result;
}

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
// binds to the "Next Tab" / "Previous Tab" commands.
bool ResolveTabCommandsFromAccels(const ACCEL* accels, int count) {
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
        return false;
    }

    g_cmdNextTab = next;
    g_cmdPrevTab = prev;
    Wh_Log(L"Tab commands resolved: next=%u, prev=%u", next, prev);
    return true;
}

bool AreTabCommandsResolved() {
    return g_cmdNextTab && g_cmdPrevTab;
}

void ResolveTabCommandsFromTable(HACCEL hAccel) {
    if (AreTabCommandsResolved()) {
        return;
    }

    // Don't scan the same table over and over again. SumatraPDF uses several
    // tables (e.g. a reduced one while an edit control has the focus), not all
    // of them contain the tab shortcuts.
    for (int i = 0; i < g_scannedAccelTablesCount; i++) {
        if (g_scannedAccelTables[i] == hAccel) {
            return;
        }
    }

    if (g_scannedAccelTablesCount < (int)ARRAYSIZE(g_scannedAccelTables)) {
        g_scannedAccelTables[g_scannedAccelTablesCount++] = hAccel;
    }

    int count = CopyAcceleratorTable(hAccel, nullptr, 0);
    if (count <= 0) {
        return;
    }

    std::vector<ACCEL> accels(count);
    count = CopyAcceleratorTable(hAccel, accels.data(), count);
    if (count <= 0) {
        return;
    }

    ResolveTabCommandsFromAccels(accels.data(), count);
}

bool IsTabBarWindow(HWND hWnd) {
    WCHAR windowClassName[64];
    if (!GetClassName(hWnd, windowClassName, ARRAYSIZE(windowClassName))) {
        return false;
    }

    for (PCWSTR className : kTabBarClassNames) {
        if (_wcsicmp(windowClassName, className) == 0) {
            return true;
        }
    }

    return false;
}

BOOL CALLBACK FindTabBarEnumFunc(HWND hWnd, LPARAM lParam) {
    if (IsTabBarWindow(hWnd) && IsWindowVisible(hWnd)) {
        *(HWND*)lParam = hWnd;
        return FALSE;
    }

    return TRUE;
}

// The tab bar is a child of the main window, or of the custom caption window
// when the tabs are shown in the title bar. It's hidden when tabs are disabled
// or (depending on the version) when only a single document is open.
HWND FindVisibleTabBar(HWND hFrameWnd) {
    HWND hTabBar = nullptr;
    EnumChildWindows(hFrameWnd, FindTabBarEnumFunc, (LPARAM)&hTabBar);
    return hTabBar;
}

// Fallback for SumatraPDF < 3.5: Ctrl+Tab (or Ctrl+Shift+Tab) is handled in
// the key handler of the main window. The Ctrl/Shift state is faked via the
// GetKeyState hook while the message is being processed.
void SimulateCtrlTab(HWND hFrameWnd, bool reverse) {
    Wh_Log(L"Simulating Ctrl+%sTab for window %08X", reverse ? L"Shift+" : L"",
           (DWORD)(ULONG_PTR)hFrameWnd);

    g_simulateKeys = true;
    g_simulateShiftKeyDown = reverse;

    SendMessage(hFrameWnd, WM_KEYDOWN, VK_TAB, 0);

    g_simulateKeys = false;
    g_simulateShiftKeyDown = false;
}

bool SwitchTab(HWND hFrameWnd, bool reverse) {
    WORD cmdId = 0;
    if (g_settings.commandIdNextTab > 0 && g_settings.commandIdPrevTab > 0) {
        cmdId = (WORD)(reverse ? g_settings.commandIdPrevTab
                               : g_settings.commandIdNextTab);
    } else if (!g_legacyKeyboardMode) {
        cmdId = reverse ? g_cmdPrevTab : g_cmdNextTab;
    }

    if (cmdId) {
        Wh_Log(L"Posting command %u to window %08X", cmdId,
               (DWORD)(ULONG_PTR)hFrameWnd);

        // This is what TranslateAccelerator does for Ctrl+PageDown/PageUp. The
        // message is posted rather than sent so that the tab switch runs from
        // the message loop and not nested inside the mouse wheel handling.
        return !!PostMessage(hFrameWnd, WM_COMMAND, MAKEWPARAM(cmdId, 1), 0);
    }

    if (g_legacyKeyboardMode) {
        SimulateCtrlTab(hFrameWnd, reverse);
        return true;
    }

    Wh_Log(L"Tab command ids are not known (yet)");
    return false;
}

bool OnMouseWheel(HWND hWnd, WORD keys, short delta, int xPos, int yPos) {
    if (keys) {
        // A modifier key or mouse button is held down, e.g. Ctrl+wheel (zoom)
        // or Shift+wheel (horizontal scrolling). Leave that to SumatraPDF.
        return false;
    }

    HWND hTabBar = FindVisibleTabBar(hWnd);
    if (!hTabBar) {
        return false;
    }

    RECT rect{};
    if (!GetWindowRect(hTabBar, &rect)) {
        return false;
    }

    POINT pt{xPos, yPos};
    if (!PtInRect(&rect, pt)) {
        return false;
    }

    if (GetKeyState(VK_MENU) < 0 || GetKeyState(VK_LWIN) < 0 ||
        GetKeyState(VK_RWIN) < 0) {
        return false;
    }

    if (hWnd == g_lastScrollWnd &&
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

    bool reverse = false;
    if (clicks < 0) {
        clicks = -clicks;
        reverse = true;
    }

    if (clicks > 0) {
        for (int i = 0; i < clicks; i++) {
            if (!SwitchTab(hWnd, reverse)) {
                break;
            }
        }

        g_lastActionTime = GetTickCount();
    }

    g_lastScrollTime = GetTickCount();
    g_lastScrollWnd = hWnd;
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
                                         UINT_PTR uIdSubclass,
                                         DWORD_PTR dwRefData) {
    if (uMsg == WM_NCDESTROY || (uMsg == g_subclassRegisteredMsg && !wParam)) {
        RemoveWindowSubclass(hWnd, FrameWindowSubclassProc, 0);
    }

    switch (uMsg) {
        case WM_MOUSEWHEEL:
        case WM_MOUSEHWHEEL: {
            WORD fwKeys = GET_KEYSTATE_WPARAM(wParam);
            short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);

            if (uMsg == WM_MOUSEHWHEEL) {
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
            } else if (!g_settings.reverseScrollingDirection) {
                zDelta = -zDelta;
            }

            if (OnMouseWheel(hWnd, fwKeys, zDelta, xPos, yPos)) {
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

    if (g_uiThreadId && g_uiThreadId != dwThreadId) {
        return TRUE;
    }

    if (IsSumatraFrameWindow(hWnd)) {
        Wh_Log(L"SumatraPDF window found: %08X", (DWORD)(ULONG_PTR)hWnd);

        if (!g_uiThreadId) {
            g_uiThreadId = dwThreadId;
        }

        SetWindowSubclassFromAnyThread(hWnd, FrameWindowSubclassProc, 0, 0);
    }

    return TRUE;
}

BOOL CALLBACK EnumFrameWindowsUnsubclassFunc(HWND hWnd, LPARAM lParam) {
    if (IsSumatraFrameWindow(hWnd)) {
        Wh_Log(L"SumatraPDF window to unsubclass: %08X",
               (DWORD)(ULONG_PTR)hWnd);

        SendMessage(hWnd, g_subclassRegisteredMsg, FALSE, 0);
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

    if (g_uiThreadId && g_uiThreadId != GetCurrentThreadId()) {
        return hWnd;
    }

    if (IsSumatraFrameWindow(hWnd)) {
        Wh_Log(L"SumatraPDF window created: %08X", (DWORD)(ULONG_PTR)hWnd);

        if (!g_uiThreadId) {
            g_uiThreadId = GetCurrentThreadId();
        }

        SetWindowSubclass(hWnd, FrameWindowSubclassProc, 0, 0);
    }

    return hWnd;
}

// SumatraPDF 3.5+ creates its accelerator tables on startup. Catching the
// creation makes the command ids known before the first wheel event.
using CreateAcceleratorTableW_t = decltype(&CreateAcceleratorTableW);
CreateAcceleratorTableW_t pOriginalCreateAcceleratorTableW;
HACCEL WINAPI CreateAcceleratorTableWHook(LPACCEL paccel, int cAccel) {
    HACCEL hAccel = pOriginalCreateAcceleratorTableW(paccel, cAccel);

    if (hAccel && paccel && cAccel > 0 && !AreTabCommandsResolved()) {
        ResolveTabCommandsFromAccels(paccel, cAccel);
    }

    return hAccel;
}

// Covers the case that the mod is loaded into an already running SumatraPDF:
// the message loop calls TranslateAccelerator for every key and mouse message
// of the main window.
using TranslateAcceleratorW_t = decltype(&TranslateAcceleratorW);
TranslateAcceleratorW_t pOriginalTranslateAcceleratorW;
int WINAPI TranslateAcceleratorWHook(HWND hWnd, HACCEL hAccTable, LPMSG lpMsg) {
    if (hAccTable && !AreTabCommandsResolved()) {
        ResolveTabCommandsFromTable(hAccTable);
    }

    return pOriginalTranslateAcceleratorW(hWnd, hAccTable, lpMsg);
}

using GetKeyState_t = decltype(&GetKeyState);
GetKeyState_t pOriginalGetKeyState;
SHORT WINAPI GetKeyStateHook(int nVirtKey) {
    if (g_simulateKeys) {
        // High bit set = key is down.
        switch (nVirtKey) {
            case VK_CONTROL:
            case VK_LCONTROL:
                return (SHORT)0x8000;

            case VK_SHIFT:
            case VK_LSHIFT:
                return g_simulateShiftKeyDown ? (SHORT)0x8000 : 0;

            default:
                return 0;
        }
    }

    return pOriginalGetKeyState(nVirtKey);
}

void LoadSettings() {
    g_settings.reverseScrollingDirection =
        Wh_GetIntSetting(L"reverseScrollingDirection");
    g_settings.horizontalScrolling = Wh_GetIntSetting(L"horizontalScrolling");
    g_settings.throttleMs = Wh_GetIntSetting(L"throttleMs");
    g_settings.commandIdNextTab = Wh_GetIntSetting(L"commandIds.nextTab");
    g_settings.commandIdPrevTab = Wh_GetIntSetting(L"commandIds.prevTab");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    WORD versionMajor = 0;
    WORD versionMinor = 0;
    if (GetProcessFileVersion(&versionMajor, &versionMinor)) {
        Wh_Log(L"SumatraPDF version %u.%u", versionMajor, versionMinor);
        g_legacyKeyboardMode =
            versionMajor < 3 || (versionMajor == 3 && versionMinor < 5);
    } else {
        Wh_Log(L"Failed to read the file version, assuming 3.5 or newer");
    }

    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExWHook,
                       (void**)&pOriginalCreateWindowExW);

    if (g_legacyKeyboardMode) {
        Wh_Log(L"Using the Ctrl+Tab fallback");
        Wh_SetFunctionHook((void*)GetKeyState, (void*)GetKeyStateHook,
                           (void**)&pOriginalGetKeyState);
    } else {
        Wh_SetFunctionHook((void*)CreateAcceleratorTableW,
                           (void*)CreateAcceleratorTableWHook,
                           (void**)&pOriginalCreateAcceleratorTableW);
        Wh_SetFunctionHook((void*)TranslateAcceleratorW,
                           (void*)TranslateAcceleratorWHook,
                           (void**)&pOriginalTranslateAcceleratorW);
    }

    EnumWindows(InitialEnumFrameWindowsFunc, 0);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_uiThreadId != 0) {
        EnumThreadWindows(g_uiThreadId, EnumFrameWindowsUnsubclassFunc, 0);
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}
