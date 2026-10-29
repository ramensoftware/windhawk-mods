// ==WindhawkMod==
// @id              chrome-middle-click-new-tab
// @name            Middle-Click New Tab Chrome/Edge/Brave
// @description     Middle-click the empty area of the tab bar to open a new tab (Chrome/Edge/Brave)
// @version         1.1
// @author          erenalpars
// @github          https://github.com/erenalpars
// @include         chrome.exe
// @include         msedge.exe
// @include         brave.exe
// @compilerOptions -lcomctl32 -lgdi32 -lole32 -loleaut32 -lshell32
// @license         GPL-3.0
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// Based on "Chrome/Edge scroll tabs with mouse wheel" by m417z, published under
// The GNU General Public License v3.0:
// https://github.com/m417z/my-windhawk-mods/blob/main/mods/chrome-wheel-scroll-tabs.wh.cpp
// The browser window detection and enumeration, DPI helper and CreateWindowExW
// hook code are taken from that mod.

// ==WindhawkModReadme==
/*
# Middle-Click New Tab Chrome/Edge/Brave

Middle-click the empty area of the tab bar (between the "+" button and the
minimize/maximize/close buttons) to open a new tab, just like clicking "+".

![demonstration](https://i.imgur.com/KBVoOg9.gif)

Currently supported browsers: Google Chrome, Microsoft Edge, Brave. Other
Chromium-based browsers may work too: add their executable in the mod's
Advanced settings (custom process inclusion list).

The empty tab bar area is detected using the window's hit-testing: Chrome
reports it as a caption (draggable) area, while tabs and buttons are reported
as client area. By default, only caption areas that have a tab or a button
between them and the side away from the window buttons are used, so the thin
drag strip above the tabs and the area before the first tab are ignored. This
also works with right-to-left UI languages, where the tab bar is mirrored.

With vertical tabs, the empty area below the tabs works too. Chrome and Brave
report it as a caption area as well, but Edge reports it as client area, so in
Edge it's detected with UI Automation instead: on a middle-click in Edge's
client area, the mod looks for the tab strip at the clicked point, without
querying the web page itself. This happens with any tab layout, e.g. also when
middle-clicking a bookmark, except in the rows of a horizontal tab strip.
Middle-clicks on web pages, such as on links, aren't checked, as Edge doesn't
pass them on as regular mouse messages. As a side effect, after the first
checked click, Edge enables its native accessibility API support (shown in
`edge://accessibility`) until it's restarted, but not web content
accessibility. To avoid this, turn off the "Edge vertical tabs" setting.

Installed web apps (PWAs) are ignored, since they have a title bar instead of a
tab bar.

The new tab is opened with the browser's "new" app command (the same command as
Ctrl+T), so no keyboard input is simulated.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- onlyNextToTabs: true
  $name: Only next to the tabs
  $description: >-
    Only react to the empty area next to the tabs (e.g. between the "+" button
    and the window buttons). When disabled, any empty part of the title bar
    works, including the thin strip above the tabs and the title bar of windows
    without tabs, such as popups and DevTools; there, a new tab opens in a
    regular browser window.
- edgeVerticalTabs: true
  $name: Edge vertical tabs
  $description: >-
    In Edge, also react to the empty area below the vertical tabs. Uses UI
    Automation, see the description for details.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <commctrl.h>
#include <propsys.h>
#include <shobjidl.h>
#include <uiautomation.h>
#include <windowsx.h>

struct {
    bool onlyNextToTabs;
    bool edgeVerticalTabs;
} g_settings;

bool g_isEdge;
DWORD g_uiThreadId;
HWND g_middleButtonDownWnd;

// Edge vertical tabs: a middle-click in the client area is checked in the
// background, starting on press. Used on the UI thread, except for the thread
// handle, which Wh_ModUninit also waits for.
UINT g_verticalTabsCheckDoneMsg;
HANDLE g_verticalTabsCheckThread;
DWORD g_lastClickId;

struct {
    // The window of the click being checked, or null if there's none.
    HWND hWnd;
    // Passed to the check thread and back, to match its result to the click.
    DWORD id;
    // Client coordinates.
    POINT downPt;
    bool done;
    bool isEmpty;
    // If the button is released before the check completes, the release is
    // held until then. Chromium closes a tab on release, which would make the
    // point look empty if the check saw the tab strip after that.
    bool releaseHeld;
    WPARAM upWParam;
    LPARAM upLParam;
} g_pendingClick;

// Used as the timer id, which only has to be unique for the window.
#define HELD_RELEASE_TIMER_ID ((UINT_PTR)&g_pendingClick)
constexpr UINT kHeldReleaseTimeoutMs = 1000;

UINT GetDpiForWindowWithFallback(HWND hWnd) {
    using GetDpiForWindow_t = UINT(WINAPI*)(HWND hwnd);
    static GetDpiForWindow_t pGetDpiForWindow = []() {
        HMODULE hUser32 = GetModuleHandle(L"user32.dll");
        if (hUser32) {
            return (GetDpiForWindow_t)GetProcAddress(hUser32,
                                                     "GetDpiForWindow");
        }

        return (GetDpiForWindow_t) nullptr;
    }();

    UINT dpi = 96;
    if (pGetDpiForWindow) {
        dpi = pGetDpiForWindow(hWnd);
    } else {
        HDC hdc = GetDC(nullptr);
        if (hdc) {
            dpi = GetDeviceCaps(hdc, LOGPIXELSX);
            ReleaseDC(nullptr, hdc);
        }
    }

    return dpi;
}

// Installed web apps (PWAs) use the same window class as browser windows, but
// their AppUserModelID looks like "Chrome._crx_<app id>".
bool IsWebAppWindow(HWND hWnd) {
    // PKEY_AppUserModel_ID, defined here to avoid linking with uuid.lib.
    constexpr PROPERTYKEY kAppUserModelIdKey = {
        {0x9F4C2855,
         0x9F79,
         0x4B39,
         {0xA8, 0xD0, 0xE1, 0xD4, 0x2D, 0xE1, 0xD5, 0xF3}},
        5};

    IPropertyStore* propertyStore = nullptr;
    if (FAILED(SHGetPropertyStoreForWindow(hWnd, IID_PPV_ARGS(&propertyStore)))) {
        return false;
    }

    bool isWebApp = false;

    PROPVARIANT value;
    PropVariantInit(&value);
    if (SUCCEEDED(propertyStore->GetValue(kAppUserModelIdKey, &value)) &&
        value.vt == VT_LPWSTR && value.pwszVal) {
        isWebApp = wcsstr(value.pwszVal, L"_crx_") != nullptr;
    }

    PropVariantClear(&value);
    propertyStore->Release();

    return isWebApp;
}

// With a right-to-left UI language, the tab strip is mirrored and the window
// buttons are on the left. Chrome doesn't set WS_EX_LAYOUTRTL on the window, so
// check which top corner has the close button.
bool AreWindowButtonsOnLeft(HWND hWnd, const RECT& rect, UINT dpi) {
    int x = rect.left + MulDiv(24, dpi, 96);
    int y = rect.top + MulDiv(16, dpi, 96);
    return SendMessage(hWnd, WM_NCHITTEST, 0, MAKELPARAM(x, y)) == HTCLOSE;
}

// Steps from the point in the given direction while the window reports a
// caption area, and checks whether a client area element (a tab or a button)
// is reached.
bool IsClientAreaInDirection(HWND hWnd,
                             const RECT& rect,
                             int xPos,
                             int yPos,
                             int xStep,
                             int yStep) {
    for (int x = xPos + xStep, y = yPos + yStep;
         x >= rect.left && x < rect.right && y >= rect.top && y < rect.bottom;
         x += xStep, y += yStep) {
        LRESULT hitTest = SendMessage(hWnd, WM_NCHITTEST, 0, MAKELPARAM(x, y));
        if (hitTest == HTCLIENT) {
            return true;
        }

        if (hitTest != HTCAPTION) {
            return false;
        }
    }

    return false;
}

// Checks whether there's a client area element (a tab or the "+" button) on the
// same row, on the side away from the window buttons, or above the point. The
// former is true for the empty area after the tabs, the latter for the empty
// area below vertical tabs. Neither is true for the drag strip above the tabs
// or the area before the first tab.
bool IsNextToTabs(HWND hWnd, int xPos, int yPos) {
    RECT rect{};
    GetWindowRect(hWnd, &rect);

    UINT dpi = GetDpiForWindowWithFallback(hWnd);

    int step = MulDiv(2, dpi, 96);
    if (step < 1) {
        step = 1;
    }

    int xStep = -step;
    if (AreWindowButtonsOnLeft(hWnd, rect, dpi)) {
        Wh_Log(L"Window buttons are on the left, scanning to the right");
        xStep = step;
    }

    if (IsClientAreaInDirection(hWnd, rect, xPos, yPos, xStep, 0)) {
        return true;
    }

    if (IsClientAreaInDirection(hWnd, rect, xPos, yPos, 0, -step)) {
        Wh_Log(L"Found a client area element above, assuming vertical tabs");
        return true;
    }

    return false;
}

bool IsEmptyTabBarArea(HWND hWnd, int xPos, int yPos) {
    if (IsWebAppWindow(hWnd)) {
        Wh_Log(L"Web app window, ignoring");
        return false;
    }

    if (g_settings.onlyNextToTabs && !IsNextToTabs(hWnd, xPos, yPos)) {
        Wh_Log(L"Not next to the tabs, ignoring");
        return false;
    }

    return true;
}

void OpenNewTab(HWND hWnd) {
    Wh_Log(L"Opening a new tab in window %08X", (DWORD)(ULONG_PTR)hWnd);

    // Chrome maps APPCOMMAND_NEW to IDC_NEW_TAB (same as Ctrl+T).
    PostMessage(hWnd, WM_APPCOMMAND, (WPARAM)hWnd,
                MAKELPARAM(0, FAPPCOMMAND_MOUSE | APPCOMMAND_NEW));
}

// Edge vertical tabs: the empty area below the tabs is a client area, so it's
// found with UI Automation. Only the frame views and the tab container are
// queried. Querying the web page's elements, even listing them, makes the
// browser enable web content accessibility, which is expensive.

// The views between the window and the tab container, e.g. BrowserRootView ->
// NonClientView -> EdgeBrowserFrameViewWin -> BrowserView.
bool IsFrameViewClassName(PCWSTR className) {
    return wcscmp(className, L"BrowserRootView") == 0 ||
           wcscmp(className, L"NonClientView") == 0 ||
           wcsstr(className, L"FrameView") || wcsstr(className, L"BrowserView");
}

struct UiaElementInfo {
    CONTROLTYPEID controlType;
    WCHAR className[64];
    RECT rect;
};

bool GetCachedElementInfo(IUIAutomationElement* element, UiaElementInfo* info) {
    *info = {};

    if (FAILED(element->get_CachedControlType(&info->controlType)) ||
        FAILED(element->get_CachedBoundingRectangle(&info->rect))) {
        return false;
    }

    BSTR className = nullptr;
    if (SUCCEEDED(element->get_CachedClassName(&className)) && className) {
        wcsncpy_s(info->className, className, _TRUNCATE);
        SysFreeString(className);
    }

    return true;
}

// Returns the last child that contains the point, which is the topmost one, as
// views paint later children on top of earlier ones.
IUIAutomationElement* GetChildAtPoint(IUIAutomationElement* element,
                                      IUIAutomationCondition* condition,
                                      IUIAutomationCacheRequest* cacheRequest,
                                      POINT pt,
                                      bool (*filter)(const UiaElementInfo&),
                                      UiaElementInfo* info) {
    IUIAutomationElementArray* children = nullptr;
    if (FAILED(element->FindAllBuildCache(TreeScope_Children, condition,
                                          cacheRequest, &children)) ||
        !children) {
        return nullptr;
    }

    IUIAutomationElement* result = nullptr;

    int length = 0;
    children->get_Length(&length);
    for (int i = 0; i < length; i++) {
        IUIAutomationElement* child = nullptr;
        if (FAILED(children->GetElement(i, &child)) || !child) {
            continue;
        }

        UiaElementInfo childInfo;
        if (GetCachedElementInfo(child, &childInfo) &&
            PtInRect(&childInfo.rect, pt) && (!filter || filter(childInfo))) {
            if (result) {
                result->Release();
            }

            result = child;
            *info = childInfo;
        } else {
            child->Release();
        }
    }

    children->Release();

    return result;
}

bool IsTabContainerOrFrameView(const UiaElementInfo& info) {
    return info.controlType == UIA_TabControlTypeId ||
           IsFrameViewClassName(info.className);
}

bool IsEmptyVerticalTabArea(IUIAutomation* automation, HWND hWnd, POINT pt) {
    IUIAutomationCacheRequest* cacheRequest = nullptr;
    IUIAutomationCondition* condition = nullptr;
    IUIAutomationElement* element = nullptr;
    bool result = false;

    if (FAILED(automation->CreateCacheRequest(&cacheRequest)) ||
        FAILED(automation->get_RawViewCondition(&condition))) {
        goto cleanup;
    }

    cacheRequest->AddProperty(UIA_ControlTypePropertyId);
    cacheRequest->AddProperty(UIA_ClassNamePropertyId);
    cacheRequest->AddProperty(UIA_BoundingRectanglePropertyId);
    cacheRequest->put_TreeFilter(condition);

    if (FAILED(automation->ElementFromHandleBuildCache(hWnd, cacheRequest,
                                                       &element)) ||
        !element) {
        goto cleanup;
    }

    {
        // Go down the frame views until reaching a tab container. Other
        // elements, such as the web page container, aren't entered.
        UiaElementInfo info{};
        bool foundTabContainer = false;
        for (int depth = 0; depth < 8; depth++) {
            IUIAutomationElement* child =
                GetChildAtPoint(element, condition, cacheRequest, pt,
                                IsTabContainerOrFrameView, &info);
            element->Release();
            element = child;
            if (!element) {
                break;
            }

            if (info.controlType == UIA_TabControlTypeId) {
                foundTabContainer = true;
                break;
            }
        }

        if (!foundTabContainer) {
            Wh_Log(L"No tab container at the point");
            goto cleanup;
        }

        Wh_Log(L"Tab container: %s", info.className);

        // Go down to the topmost element at the point. The empty area is a
        // plain view right below a tab container, while a tab, a button or
        // the views wrapping a tab are something else or deeper.
        int levelsBelowTabContainer = 0;
        for (int depth = 0; depth < 12; depth++) {
            IUIAutomationElement* child = GetChildAtPoint(
                element, condition, cacheRequest, pt, nullptr, &info);
            if (!child) {
                break;
            }

            element->Release();
            element = child;

            if (info.controlType == UIA_TabControlTypeId) {
                levelsBelowTabContainer = 0;
            } else if (info.controlType == UIA_PaneControlTypeId) {
                levelsBelowTabContainer++;
            } else {
                Wh_Log(L"Element at the point: %s (control type %d)",
                       info.className, info.controlType);
                goto cleanup;
            }
        }

        Wh_Log(L"Element at the point: %s, %d levels below a tab container",
               info.className, levelsBelowTabContainer);
        result = levelsBelowTabContainer <= 1;
    }

cleanup:
    if (element) {
        element->Release();
    }

    if (condition) {
        condition->Release();
    }

    if (cacheRequest) {
        cacheRequest->Release();
    }

    return result;
}

struct VerticalTabsCheckParam {
    HWND hWnd;
    POINT pt;
    DWORD clickId;
};

DWORD WINAPI VerticalTabsCheckThread(void* parameter) {
    VerticalTabsCheckParam param = *(VerticalTabsCheckParam*)parameter;
    delete (VerticalTabsCheckParam*)parameter;

    bool isEmpty = false;

    // UI Automation calls are forwarded to the window's thread, which must
    // keep processing messages, so they can't be made on that thread.
    if (SUCCEEDED(CoInitializeEx(nullptr, COINIT_MULTITHREADED))) {
        IUIAutomation* automation = nullptr;
        if (SUCCEEDED(CoCreateInstance(__uuidof(CUIAutomation), nullptr,
                                       CLSCTX_INPROC_SERVER,
                                       IID_PPV_ARGS(&automation)))) {
            isEmpty = IsEmptyVerticalTabArea(automation, param.hWnd, param.pt);
            automation->Release();
        }

        CoUninitialize();
    }

    PostMessage(param.hWnd, g_verticalTabsCheckDoneMsg, param.clickId, isEmpty);

    return 0;
}

bool StartVerticalTabsCheck(HWND hWnd, POINT pt, DWORD clickId) {
    if (g_verticalTabsCheckThread) {
        if (WaitForSingleObject(g_verticalTabsCheckThread, 0) == WAIT_TIMEOUT) {
            Wh_Log(L"A check is already running, ignoring");
            return false;
        }

        CloseHandle(g_verticalTabsCheckThread);
        g_verticalTabsCheckThread = nullptr;
    }

    auto* param = new VerticalTabsCheckParam{hWnd, pt, clickId};
    g_verticalTabsCheckThread =
        CreateThread(nullptr, 0, VerticalTabsCheckThread, param, 0, nullptr);
    if (!g_verticalTabsCheckThread) {
        delete param;
        return false;
    }

    return true;
}

// A horizontal tab strip has caption area next to the tabs, while the rows of
// a vertical tab strip have none. Clicks in a row with caption area, such as a
// middle-click that closes a horizontal tab, don't need a check.
bool RowHasCaptionArea(HWND hWnd, int yPos) {
    RECT rect{};
    GetWindowRect(hWnd, &rect);

    int step = MulDiv(8, GetDpiForWindowWithFallback(hWnd), 96);
    if (step < 1) {
        step = 1;
    }

    for (int x = rect.left; x < rect.right; x += step) {
        if (SendMessage(hWnd, WM_NCHITTEST, 0, MAKELPARAM(x, yPos)) ==
            HTCAPTION) {
            return true;
        }
    }

    return false;
}

void OnClientMiddleButtonDown(HWND hWnd, LPARAM lParam) {
    g_pendingClick.hWnd = nullptr;

    if (!g_isEdge || !g_settings.edgeVerticalTabs || IsWebAppWindow(hWnd)) {
        return;
    }

    POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
    POINT screenPt = pt;
    ClientToScreen(hWnd, &screenPt);

    if (RowHasCaptionArea(hWnd, screenPt.y)) {
        return;
    }

    DWORD clickId = ++g_lastClickId;
    if (!StartVerticalTabsCheck(hWnd, screenPt, clickId)) {
        return;
    }

    g_pendingClick = {};
    g_pendingClick.hWnd = hWnd;
    g_pendingClick.id = clickId;
    g_pendingClick.downPt = pt;
}

// Opens a tab if the click was on the empty area below vertical tabs and the
// button was released close to where it was pressed, like a regular click.
void CompletePendingClick(HWND hWnd, LPARAM upLParam) {
    int dx = GET_X_LPARAM(upLParam) - g_pendingClick.downPt.x;
    int dy = GET_Y_LPARAM(upLParam) - g_pendingClick.downPt.y;
    bool isClick = abs(dx) <= GetSystemMetrics(SM_CXDRAG) &&
                   abs(dy) <= GetSystemMetrics(SM_CYDRAG);

    if (g_pendingClick.isEmpty && isClick) {
        OpenNewTab(hWnd);
    }

    g_pendingClick.hWnd = nullptr;
}

// Passes a held release on to the browser, opening a tab first if needed.
void DeliverHeldRelease(HWND hWnd, bool complete) {
    KillTimer(hWnd, HELD_RELEASE_TIMER_ID);

    WPARAM upWParam = g_pendingClick.upWParam;
    LPARAM upLParam = g_pendingClick.upLParam;
    if (complete) {
        CompletePendingClick(hWnd, upLParam);
    } else {
        g_pendingClick.hWnd = nullptr;
    }

    DefSubclassProc(hWnd, WM_MBUTTONUP, upWParam, upLParam);
}

LRESULT CALLBACK BrowserWindowSubclassProc(HWND hWnd,
                                           UINT uMsg,
                                           WPARAM wParam,
                                           LPARAM lParam,
                                           DWORD_PTR dwRefData) {
    // The check result, or with a zero id, a request to pass on a held release
    // before the mod is unloaded.
    if (uMsg == g_verticalTabsCheckDoneMsg) {
        if (g_pendingClick.hWnd == hWnd) {
            if (wParam == 0) {
                if (g_pendingClick.releaseHeld) {
                    DeliverHeldRelease(hWnd, false);
                }
            } else if (wParam == g_pendingClick.id) {
                g_pendingClick.done = true;
                g_pendingClick.isEmpty = lParam;
                if (g_pendingClick.releaseHeld) {
                    DeliverHeldRelease(hWnd, true);
                }
            }
        }

        return 0;
    }

    switch (uMsg) {
        case WM_NCMBUTTONDOWN:
        // A quick second click arrives as a double click, treat it as another
        // click.
        case WM_NCMBUTTONDBLCLK:
            g_middleButtonDownWnd = nullptr;
            if (wParam == HTCAPTION &&
                IsEmptyTabBarArea(hWnd, GET_X_LPARAM(lParam),
                                  GET_Y_LPARAM(lParam))) {
                g_middleButtonDownWnd = hWnd;
                return 0;
            }
            break;

        // A press in the client area means that a release on the tab bar
        // doesn't complete a click that started there.
        // Edge doesn't do anything on a middle-click on the empty area below
        // vertical tabs, so the press isn't swallowed. The check starts in the
        // background, and a tab is opened on release if the area is empty.
        case WM_MBUTTONDOWN:
        case WM_MBUTTONDBLCLK:
            g_middleButtonDownWnd = nullptr;
            if (g_pendingClick.hWnd && g_pendingClick.releaseHeld) {
                if (g_pendingClick.hWnd == hWnd) {
                    DeliverHeldRelease(hWnd, false);
                } else {
                    // Mouse capture makes this unlikely. DefSubclassProc can
                    // only be used for the current window, so send it.
                    HWND pendingWnd = g_pendingClick.hWnd;
                    g_pendingClick.hWnd = nullptr;
                    KillTimer(pendingWnd, HELD_RELEASE_TIMER_ID);
                    SendMessage(pendingWnd, WM_MBUTTONUP,
                                g_pendingClick.upWParam,
                                g_pendingClick.upLParam);
                }
            }

            OnClientMiddleButtonDown(hWnd, lParam);
            break;

        case WM_MBUTTONUP:
            if (g_pendingClick.hWnd == hWnd && !g_pendingClick.releaseHeld) {
                if (!g_pendingClick.done) {
                    g_pendingClick.releaseHeld = true;
                    g_pendingClick.upWParam = wParam;
                    g_pendingClick.upLParam = lParam;
                    SetTimer(hWnd, HELD_RELEASE_TIMER_ID, kHeldReleaseTimeoutMs,
                             nullptr);
                    return 0;
                }

                CompletePendingClick(hWnd, lParam);
            }
            break;

        // Don't hold the release for too long if the check is slow.
        case WM_TIMER:
            if (wParam == HELD_RELEASE_TIMER_ID) {
                Wh_Log(L"The check took too long");
                if (g_pendingClick.hWnd == hWnd &&
                    g_pendingClick.releaseHeld) {
                    DeliverHeldRelease(hWnd, false);
                } else {
                    KillTimer(hWnd, HELD_RELEASE_TIMER_ID);
                }

                return 0;
            }
            break;

        case WM_NCMBUTTONUP:
            if (g_middleButtonDownWnd == hWnd) {
                g_middleButtonDownWnd = nullptr;
                // Only open a tab if the button was released in the same
                // area, like a regular button click.
                if (wParam == HTCAPTION &&
                    IsEmptyTabBarArea(hWnd, GET_X_LPARAM(lParam),
                                      GET_Y_LPARAM(lParam))) {
                    OpenNewTab(hWnd);
                }
                return 0;
            }
            break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Best effort determination as of whether the window is a browser window.
bool IsBrowserWindow(HWND hWnd) {
    if (!(GetWindowLong(hWnd, GWL_STYLE) & WS_SYSMENU)) {
        return false;
    }

    WCHAR windowClassName[256];
    if (!GetClassName(hWnd, windowClassName, ARRAYSIZE(windowClassName))) {
        return false;
    }

    bool classNameMatch = false;
    PCWSTR classNames[] = {
        L"Chrome_WidgetWin_1",
        L"YandexBrowser_WidgetWin_1",
    };
    for (PCWSTR className : classNames) {
        if (_wcsicmp(windowClassName, className) == 0) {
            classNameMatch = true;
            break;
        }
    }

    return classNameMatch;
}

BOOL CALLBACK InitialEnumBrowserWindowsFunc(HWND hWnd, LPARAM lParam) {
    DWORD dwProcessId = 0;
    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, &dwProcessId);
    if (!dwThreadId || dwProcessId != GetCurrentProcessId()) {
        return TRUE;
    }

    if (g_uiThreadId && g_uiThreadId != dwThreadId) {
        return TRUE;
    }

    if (IsBrowserWindow(hWnd)) {
        Wh_Log(L"Browser window found: %08X", (DWORD)(ULONG_PTR)hWnd);

        if (!g_uiThreadId) {
            g_uiThreadId = dwThreadId;
        }

        WindhawkUtils::SetWindowSubclassFromAnyThread(
            hWnd, BrowserWindowSubclassProc, 0);
    }

    return TRUE;
}

BOOL CALLBACK EnumBrowserWindowsUnsubclassFunc(HWND hWnd, LPARAM lParam) {
    if (IsBrowserWindow(hWnd)) {
        Wh_Log(L"Browser window to unsubclass: %08X", (DWORD)(ULONG_PTR)hWnd);

        // Pass on a release that's held for a check.
        SendMessage(hWnd, g_verticalTabsCheckDoneMsg, 0, 0);

        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            hWnd, BrowserWindowSubclassProc);
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

    if (IsBrowserWindow(hWnd)) {
        Wh_Log(L"Browser window created: %08X", (DWORD)(ULONG_PTR)hWnd);

        if (!g_uiThreadId) {
            g_uiThreadId = GetCurrentThreadId();
        }

        WindhawkUtils::SetWindowSubclassFromAnyThread(
            hWnd, BrowserWindowSubclassProc, 0);
    }

    return hWnd;
}

// Renderer, GPU and other child processes have a "--type=" switch and own no
// browser windows.
bool IsBrowserProcess() {
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return false;
    }

    bool isBrowserProcess = true;
    for (int i = 1; i < argc; i++) {
        if (wcsncmp(argv[i], L"--type=", sizeof("--type=") - 1) == 0) {
            isBrowserProcess = false;
            break;
        }
    }

    LocalFree(argv);

    return isBrowserProcess;
}

void LoadSettings() {
    g_settings.onlyNextToTabs = Wh_GetIntSetting(L"onlyNextToTabs");
    g_settings.edgeVerticalTabs = Wh_GetIntSetting(L"edgeVerticalTabs");
}

BOOL Wh_ModInit() {
    if (!IsBrowserProcess()) {
        return FALSE;
    }

    Wh_Log(L">");

    LoadSettings();

    g_isEdge = !!GetModuleHandle(L"msedge.exe");

    // Registered before subclassing, since the subclass procedure compares
    // messages to it.
    g_verticalTabsCheckDoneMsg = RegisterWindowMessage(
        L"Windhawk_VerticalTabsCheckDone_" WH_MOD_ID);

    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExWHook,
                                   &pOriginalCreateWindowExW);

    EnumWindows(InitialEnumBrowserWindowsFunc, 0);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_uiThreadId != 0) {
        EnumThreadWindows(g_uiThreadId, EnumBrowserWindowsUnsubclassFunc, 0);
    }

    // No new check can start once the subclasses are removed. The check's UI
    // Automation calls are handled by the UI thread, which isn't blocked here.
    if (g_verticalTabsCheckThread) {
        WaitForSingleObject(g_verticalTabsCheckThread, INFINITE);
        CloseHandle(g_verticalTabsCheckThread);
        g_verticalTabsCheckThread = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}
