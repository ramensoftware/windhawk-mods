// ==WindhawkMod==
// @id              win11-taskbar-overflow-hover
// @name            Taskbar & System Tray Overflow on Hover
// @description     Opens the Windows 11 taskbar app overflow and system tray hidden icons menu on hover, auto-closing on exit or click.
// @version         1.1.0
// @author          Cougar16
// @github          https://github.com/Cougar16
// @include         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -luiautomationcore -luuid -lshell32 -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar & System Tray Overflow on Hover 🚀

A zero-click QoL upgrade for Windows 11. Hover over the taskbar app overflow button
or the system tray chevron to instantly expand hidden items without clicking.

![Hover overflow menu](https://raw.githubusercontent.com/Cougar16/win11-taskbar-overflow-hover/main/Assets/Hover-Overflow-Menu.png)

✨ Features:
- Dual-Zone Zero-Click Open: Auto-expands both the running app overflow menu ("...") and the system tray hidden icons flyout (the "^" chevron) simply by hovering.
- Configurable Toggles: Individually enable or disable hover activation for either menu in the mod settings.
- Auto-Dismiss: Smoothly closes the active menu the moment your cursor leaves the button and flyout hit area, complete with debounced overshoot protection so quick mouse sweeps won't cause flickering.
- Instant Item Launch: Automatically dismisses the flyout the millisecond you launch an app or click an item inside it, bypassing native Windows UI delay while keeping context menus intact.
- Multi-DPI & Multi-Language Support: Per-Monitor V2 DPI awareness guarantees pixel-accurate coordinate mapping on high-scale displays, with native support across localized Windows editions.

Developed by Cougar16 for the Windows customization community.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enableAppOverflow: true
  $name: Enable App Overflow on Hover
  $description: Auto-open the running taskbar apps overflow menu ("...") when hovering.
- enableTrayChevron: true
  $name: Enable System Tray Chevron on Hover
  $description: Auto-open the system tray hidden icons flyout ("^") when hovering.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <uiautomation.h>
#include <windhawk_utils.h>
#include <windhawk_api.h>

#include <cwchar>

// Time the cursor must rest on the overflow button before the menu opens.
constexpr int kHoverDelayMs = 100;

// Time the cursor must stay outside the button + menu area before the menu
// closes (a tiny debounce so a brief overshoot does not make it flicker).
constexpr int kCloseDelayMs = 150;

// Signalled on unload to stop the monitor thread (also interrupts its sleep).
HANDLE g_stopEvent = nullptr;
HANDLE g_hThread = nullptr;

// Mod settings state
struct ModSettings {
    bool enableAppOverflow = true;
    bool enableTrayChevron = true;
} g_settings;

void LoadSettings() {
    g_settings.enableAppOverflow = Wh_GetIntSetting(L"enableAppOverflow") != 0;
    g_settings.enableTrayChevron = Wh_GetIntSetting(L"enableTrayChevron") != 0;
}

// After toggling the button the flyout needs a moment to appear. During this
// grace period the cursor being away from it does not close it.
constexpr ULONGLONG kOpenGraceMs = 600;

// After a click inside the flyout, wait this long before deciding whether the
// click opened a popup menu (keep the flyout) or not (dismiss the flyout).
constexpr ULONGLONG kClickSettleMs = 100;

// The app overflow ("...") flyout and the thumbnail previews are each hosted in
// a window of this class on the taskbar's thread. The tray's "Show hidden icons"
// popup is a different window, TopLevelWindowForOverflowXamlIsland.
constexpr wchar_t kXamlHostClass[] = L"XamlExplorerHostIslandWindow";
constexpr wchar_t kTrayOverflowHostClass[] = L"TopLevelWindowForOverflowXamlIsland";
constexpr wchar_t kLegacyTrayOverflowHostClass[] = L"NotifyIconOverflowWindow";

// Return true only when the point belongs to a taskbar top-level window
// owned by the same process as the primary taskbar.
bool IsTaskbarWindowAtPoint(POINT pt, DWORD* taskbarPidOut = nullptr) {
    HWND hWnd = WindowFromPoint(pt);
    if (!hWnd) {
        return false;
    }

    HWND hRoot = GetAncestor(hWnd, GA_ROOT);
    if (!hRoot) {
        return false;
    }

    wchar_t className[256] = {};
    if (!GetClassNameW(hRoot, className, ARRAYSIZE(className))) {
        return false;
    }

    if (wcscmp(className, L"Shell_TrayWnd") != 0 &&
        wcscmp(className, L"Shell_SecondaryTrayWnd") != 0) {
        return false;
    }

    HWND hPrimaryTaskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!hPrimaryTaskbar) {
        return false;
    }

    DWORD primaryPid = 0;
    DWORD pointPid = 0;
    GetWindowThreadProcessId(hPrimaryTaskbar, &primaryPid);
    GetWindowThreadProcessId(hRoot, &pointPid);

    if (!primaryPid || pointPid != primaryPid) {
        return false;
    }

    if (taskbarPidOut) {
        *taskbarPidOut = primaryPid;
    }

    return true;
}

// Gets the on-screen bounds of a visible XAML host. A host can span the whole
// monitor with a window region limiting it to its UI, so its window rect alone
// says nothing about where that UI is.
bool GetXamlHostBounds(HWND hWnd, RECT* bounds) {
    RECT windowRect = {};
    if (!IsWindowVisible(hWnd) || !GetWindowRect(hWnd, &windowRect)) {
        return false;
    }

    *bounds = windowRect;

    HRGN region = CreateRectRgn(0, 0, 0, 0);
    if (region) {
        // GetWindowRgn fails for a window without a region.
        if (GetWindowRgn(hWnd, region) != ERROR &&
            GetRgnBox(region, bounds) != ERROR) {
            OffsetRect(bounds, windowRect.left, windowRect.top);
        }
        DeleteObject(region);
    }

    return !IsRectEmpty(bounds);
}

// Returns true if the system tray's "Show hidden icons" flyout window
// (TopLevelWindowForOverflowXamlIsland) is currently open and visible.
bool IsTrayOverflowWindowOpen(DWORD taskbarPid, RECT* boundsOut = nullptr) {
    const wchar_t* classes[] = { kTrayOverflowHostClass, kLegacyTrayOverflowHostClass };
    for (const wchar_t* cls : classes) {
        HWND hWnd = nullptr;
        while ((hWnd = FindWindowExW(nullptr, hWnd, cls, nullptr)) != nullptr) {
            DWORD pid = 0;
            GetWindowThreadProcessId(hWnd, &pid);
            if (pid != taskbarPid) {
                continue;
            }
            RECT bounds = {};
            if (GetXamlHostBounds(hWnd, &bounds)) {
                if (boundsOut) {
                    *boundsOut = bounds;
                }
                return true;
            }
        }
    }
    return false;
}

// Scans the visible XAML hosts on the taskbar's UI thread. The thread match
// excludes the other XAML islands that live in Explorer (Alt+Tab, Task View...).
// Returns true if the point is over one of them. flyoutBounds receives the
// bounds of the host nearest the overflow button: that is the flyout, since the
// preview of an icon inside the flyout appears beyond it.
bool QueryXamlHosts(HWND hTaskbar,
                    const RECT& chevronRect,
                    POINT pt,
                    RECT* flyoutBounds) {
    *flyoutBounds = {};

    DWORD taskbarPid = 0;
    DWORD taskbarThreadId = GetWindowThreadProcessId(hTaskbar, &taskbarPid);
    if (!taskbarPid || !taskbarThreadId) {
        return false;
    }

    const LONGLONG chevronCenterX = (chevronRect.left + chevronRect.right) / 2;
    const LONGLONG chevronCenterY = (chevronRect.top + chevronRect.bottom) / 2;

    bool overHost = false;
    LONGLONG nearestDistance = -1;

    HWND hWnd = nullptr;
    while ((hWnd = FindWindowExW(nullptr, hWnd, kXamlHostClass, nullptr)) !=
           nullptr) {
        DWORD pid = 0;
        DWORD threadId = GetWindowThreadProcessId(hWnd, &pid);
        if (pid != taskbarPid || threadId != taskbarThreadId) {
            continue;
        }

        RECT bounds = {};
        if (!GetXamlHostBounds(hWnd, &bounds)) {
            continue;
        }

        overHost = overHost || PtInRect(&bounds, pt);

        const LONGLONG dx = (bounds.left + bounds.right) / 2 - chevronCenterX;
        const LONGLONG dy = (bounds.top + bounds.bottom) / 2 - chevronCenterY;
        const LONGLONG distance = dx * dx + dy * dy;
        if (nearestDistance < 0 || distance < nearestDistance) {
            nearestDistance = distance;
            *flyoutBounds = bounds;
        }
    }

    // Also scan the system tray hidden-icons flyout window
    // (TopLevelWindowForOverflowXamlIsland / NotifyIconOverflowWindow).
    const wchar_t* trayClasses[] = { kTrayOverflowHostClass, kLegacyTrayOverflowHostClass };
    for (const wchar_t* cls : trayClasses) {
        HWND hTrayWnd = nullptr;
        while ((hTrayWnd = FindWindowExW(nullptr, hTrayWnd, cls, nullptr)) != nullptr) {
            DWORD pid = 0;
            GetWindowThreadProcessId(hTrayWnd, &pid);
            if (pid != taskbarPid) {
                continue;
            }

            RECT bounds = {};
            if (!GetXamlHostBounds(hTrayWnd, &bounds)) {
                continue;
            }

            overHost = overHost || PtInRect(&bounds, pt);

            const LONGLONG dx = (bounds.left + bounds.right) / 2 - chevronCenterX;
            const LONGLONG dy = (bounds.top + bounds.bottom) / 2 - chevronCenterY;
            const LONGLONG distance = dx * dx + dy * dy;
            if (nearestDistance < 0 || distance < nearestDistance) {
                nearestDistance = distance;
                *flyoutBounds = bounds;
            }
        }
    }

    return overHost;
}

// True while a popup menu (for example a tray icon's context menu) is showing.
bool IsPopupMenuOpen() {
    GUITHREADINFO gti = {};
    gti.cbSize = sizeof(gti);
    if (GetGUIThreadInfo(0, &gti) &&
        (gti.flags & (GUI_INMENUMODE | GUI_POPUPMENUMODE))) {
        return true;
    }

    HWND hMenu = nullptr;
    while ((hMenu = FindWindowExW(nullptr, hMenu, L"#32768", nullptr)) != nullptr) {
        if (IsWindowVisible(hMenu)) {
            return true;
        }
    }

    return false;
}

// Case-insensitive substring helper for fallback localized name matching.
bool ContainsInsensitive(const wchar_t* haystack, const wchar_t* needle) {
    if (!haystack || !needle) return false;
    size_t hLen = wcslen(haystack);
    size_t nLen = wcslen(needle);
    if (nLen == 0 || nLen > hLen) return false;
    for (size_t i = 0; i <= hLen - nLen; ++i) {
        if (_wcsnicmp(haystack + i, needle, nLen) == 0) {
            return true;
        }
    }
    return false;
}

// Checks if the element is the taskbar's app overflow button ("Chevron" / "OverflowButton")
// OR the system tray's hidden icons chevron ("Afficher les icônes cachées" / "Show hidden icons").
bool IsOverflowAutomationId(IUIAutomationElement* element, bool* isTrayChevronOut = nullptr) {
    if (isTrayChevronOut) {
        *isTrayChevronOut = false;
    }
    if (!element) {
        return false;
    }

    BSTR autoId = nullptr;
    element->get_CurrentAutomationId(&autoId);

    BSTR className = nullptr;
    element->get_CurrentClassName(&className);

    bool isAppOverflow = false;
    bool isTrayChevron = false;

    // 1. Original app overflow check (subject to user setting)
    if (autoId &&
        (wcscmp(autoId, L"Chevron") == 0 ||
         wcscmp(autoId, L"OverflowButton") == 0)) {
        if (g_settings.enableAppOverflow) {
            isAppOverflow = true;
        }
    }

    // 2. System tray hidden-icons chevron check (subject to user setting):
    // On Windows 11 XAML taskbar, the chevron has:
    //   - AutomationId: "SystemTrayIcon" AND ClassName: "SystemTray.NormalButton" (or "SystemTray.ChevronButton")
    //   - Or AutomationId: "ChevronButton" / "NotificationChevron"
    if (!isAppOverflow && g_settings.enableTrayChevron) {
        if (autoId &&
            (wcscmp(autoId, L"ChevronButton") == 0 ||
             wcscmp(autoId, L"NotificationChevron") == 0)) {
            isTrayChevron = true;
        } else if (autoId && wcscmp(autoId, L"SystemTrayIcon") == 0) {
            if (className &&
                (wcscmp(className, L"SystemTray.NormalButton") == 0 ||
                 wcscmp(className, L"SystemTray.ChevronButton") == 0)) {
                isTrayChevron = true;
            } else {
                // Fallback: check the element's accessible Name (e.g. French "Afficher les icônes cachées" / "Masquer"
                // or English "Show hidden icons" / "Hide") in case a Windows build or styler alters ClassName.
                BSTR name = nullptr;
                if (SUCCEEDED(element->get_CurrentName(&name)) && name) {
                    if (ContainsInsensitive(name, L"icônes cachées") ||
                        ContainsInsensitive(name, L"icones cachees") ||
                        ContainsInsensitive(name, L"hidden icons") ||
                        wcscmp(name, L"Masquer") == 0 ||
                        wcscmp(name, L"Hide") == 0) {
                        isTrayChevron = true;
                    }
                    SysFreeString(name);
                }
            }
        }
    }

    if (className) {
        SysFreeString(className);
    }
    if (autoId) {
        SysFreeString(autoId);
    }

    if (isTrayChevronOut) {
        *isTrayChevronOut = isTrayChevron;
    }

    return isAppOverflow || isTrayChevron;
}

// Walk the UIA control tree only after the Win32 taskbar check passed.
// Every element is verified to belong to the taskbar process.
bool FindTaskbarOverflowElement(
    IUIAutomation* automation,
    POINT pt,
    DWORD taskbarPid,
    IUIAutomationElement** elementOut,
    IUIAutomationTogglePattern** toggleOut,
    IUIAutomationInvokePattern** invokeOut = nullptr,
    IUIAutomationExpandCollapsePattern** expandCollapseOut = nullptr,
    bool* isTrayChevronOut = nullptr) {

    *elementOut = nullptr;
    *toggleOut = nullptr;
    if (invokeOut) *invokeOut = nullptr;
    if (expandCollapseOut) *expandCollapseOut = nullptr;
    if (isTrayChevronOut) *isTrayChevronOut = false;

    IUIAutomationElement* pointElement = nullptr;
    HRESULT hr = automation->ElementFromPoint(pt, &pointElement);
    if (FAILED(hr) || !pointElement) {
        return false;
    }

    IUIAutomationTreeWalker* walker = nullptr;
    hr = automation->get_ControlViewWalker(&walker);
    if (FAILED(hr) || !walker) {
        pointElement->Release();
        return false;
    }

    IUIAutomationElement* node = pointElement;
    node->AddRef();

    bool found = false;

    for (int i = 0; i < 15 && node; ++i) {
        int elementPid = 0;
        hr = node->get_CurrentProcessId(&elementPid);

        // Do not inspect or interact with elements from another process.
        if (FAILED(hr) || elementPid != static_cast<int>(taskbarPid)) {
            node->Release();
            node = nullptr;
            break;
        }

        bool isTrayChevron = false;
        if (IsOverflowAutomationId(node, &isTrayChevron)) {
            IUIAutomationTogglePattern* togglePattern = nullptr;
            node->GetCurrentPatternAs(
                UIA_TogglePatternId, IID_PPV_ARGS(&togglePattern));

            IUIAutomationInvokePattern* invokePattern = nullptr;
            IUIAutomationExpandCollapsePattern* expandPattern = nullptr;

            // The system tray chevron does NOT expose TogglePattern on Windows 11;
            // it uses InvokePattern (or ExpandCollapsePattern).
            if (isTrayChevron) {
                if (invokeOut) {
                    node->GetCurrentPatternAs(
                        UIA_InvokePatternId, IID_PPV_ARGS(&invokePattern));
                }
                if (expandCollapseOut) {
                    node->GetCurrentPatternAs(
                        UIA_ExpandCollapsePatternId, IID_PPV_ARGS(&expandPattern));
                }
            }

            if (togglePattern || invokePattern || expandPattern) {
                *elementOut = node;
                *toggleOut = togglePattern;
                if (invokeOut) *invokeOut = invokePattern;
                if (expandCollapseOut) *expandCollapseOut = expandPattern;
                if (isTrayChevronOut) *isTrayChevronOut = isTrayChevron;
                node = nullptr; // Ownership transferred to elementOut.
                found = true;
                break;
            }
        }

        IUIAutomationElement* parent = nullptr;
        hr = walker->GetParentElement(node, &parent);
        node->Release();
        node = SUCCEEDED(hr) ? parent : nullptr;
    }

    if (node) {
        node->Release();
    }

    walker->Release();
    pointElement->Release();
    return found;
}

// The app overflow flyout is open while its TogglePattern state is On.
// The system tray chevron flyout is open while TopLevelWindowForOverflowXamlIsland is visible.
bool IsOverflowOpen(IUIAutomationTogglePattern* toggle,
                    bool isTrayChevron = false,
                    DWORD taskbarPid = 0) {
    if (isTrayChevron && taskbarPid != 0) {
        return IsTrayOverflowWindowOpen(taskbarPid);
    }
    if (!toggle) {
        return false;
    }
    ToggleState state = ToggleState_Off;
    return SUCCEEDED(toggle->get_CurrentToggleState(&state)) &&
           state == ToggleState_On;
}

DWORD WINAPI HoverMonitorThread(LPVOID) {
    // Ensure physical screen coordinates from GetCursorPos match UIA ElementFromPoint
    // on DPI-scaled displays (125%, 150%, etc.).
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    HRESULT coHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(coHr)) {
        Wh_Log(L"CoInitializeEx failed: 0x%08X", coHr);
        return 1;
    }

    IUIAutomation* automation = nullptr;
    HRESULT hr = CoCreateInstance(
        CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER,
        IID_IUIAutomation, reinterpret_cast<void**>(&automation));

    if (FAILED(hr) || !automation) {
        Wh_Log(L"Failed to initialize UI Automation: 0x%08X", hr);
        CoUninitialize();
        return 1;
    }

    int currentHoverTime = 0;
    int currentCloseHoverTime = 0;
    bool menuOpened = false;      // True only while the mod owns an open flyout.
    bool hoverConsumed = false;   // No opening until the cursor leaves the button.
    bool wasMouseDown = false;
    bool clickPending = false;    // A click inside the flyout is waiting to dismiss it.
    ULONGLONG openedAt = 0;
    ULONGLONG clickAt = 0;

    IUIAutomationTogglePattern* chevron = nullptr;               // Set while menuOpened (app overflow).
    IUIAutomationInvokePattern* chevronInvoke = nullptr;         // Set while menuOpened (tray chevron).
    IUIAutomationExpandCollapsePattern* chevronExpand = nullptr; // Optional tray chevron pattern.
    bool activeIsTrayChevron = false;
    DWORD activeTaskbarPid = 0;
    RECT chevronRect = {};                                       // Button bounds, captured when opened.

    // Forget everything about the flyout the mod opened.
    auto resetOwnership = [&]() {
        menuOpened = false;
        clickPending = false;
        currentHoverTime = 0;
        currentCloseHoverTime = 0;
        activeIsTrayChevron = false;
        activeTaskbarPid = 0;
        if (chevron) {
            chevron->Release();
            chevron = nullptr;
        }
        if (chevronInvoke) {
            chevronInvoke->Release();
            chevronInvoke = nullptr;
        }
        if (chevronExpand) {
            chevronExpand->Release();
            chevronExpand = nullptr;
        }
    };

    auto triggerAction = [&](IUIAutomationTogglePattern* tog,
                             IUIAutomationInvokePattern* inv,
                             IUIAutomationExpandCollapsePattern* exp,
                             bool opening) -> HRESULT {
        if (tog) {
            return tog->Toggle();
        }
        if (exp) {
            HRESULT hrExp = opening ? exp->Expand() : exp->Collapse();
            if (SUCCEEDED(hrExp)) {
                return hrExp;
            }
        }
        if (inv) {
            return inv->Invoke();
        }
        return E_FAIL;
    };

    auto closeFlyout = [&]() {
        HRESULT closeHr = triggerAction(chevron, chevronInvoke, chevronExpand, false);
        if (FAILED(closeHr)) {
            Wh_Log(L"Failed to close the overflow flyout: 0x%08X", closeHr);
        }
        resetOwnership();
    };

    // The 50 ms wait doubles as the poll interval and is interrupted by the
    // stop event, so unloading never has to wait for a sleep to finish.
    while (WaitForSingleObject(g_stopEvent, 50) == WAIT_TIMEOUT) {
        const ULONGLONG now = GetTickCount64();

        // The flyout was closed some other way (Win key, Alt+Tab, Escape typed
        // by the user, click elsewhere...). Stop considering it ours.
        // For the tray chevron, wait until kOpenGraceMs has passed before
        // declaring it externally closed so the XAML island has time to appear.
        if (menuOpened) {
            bool pastGrace = !activeIsTrayChevron || (now - openedAt >= kOpenGraceMs);
            if (pastGrace && !IsOverflowOpen(chevron, activeIsTrayChevron, activeTaskbarPid)) {
                resetOwnership();
            }
        }

        SHORT lButtonState = GetAsyncKeyState(VK_LBUTTON);
        bool isMouseDown = (lButtonState & 0x8000) != 0;
        // Also catch clicks that began and ended between two 50 ms polls (bit 0).
        bool mouseClicked = (wasMouseDown && !isMouseDown) || (!isMouseDown && (lButtonState & 0x0001) != 0);
        wasMouseDown = isMouseDown;

        POINT pt = {};
        if (!GetCursorPos(&pt)) {
            currentHoverTime = 0;
            continue;
        }

        RECT flyoutBounds = {};
        bool overXamlHost =
            menuOpened && QueryXamlHosts(FindWindowW(L"Shell_TrayWnd", nullptr),
                                         chevronRect, pt, &flyoutBounds);

        // Only a click inside the flyout counts as "clicked an icon".
        if (mouseClicked && overXamlHost) {
            clickPending = true;
            clickAt = now;
        }

        // A clicked icon that opened a popup menu keeps the flyout open until
        // the menu is gone.
        if (clickPending && now - clickAt >= kClickSettleMs && !IsPopupMenuOpen()) {
            closeFlyout();
            continue;
        }

        DWORD taskbarPid = 0;
        bool overTaskbar = IsTaskbarWindowAtPoint(pt, &taskbarPid);
        bool isOverflow = false;

        if (overTaskbar) {
            IUIAutomationElement* overflowElement = nullptr;
            IUIAutomationTogglePattern* togglePattern = nullptr;
            IUIAutomationInvokePattern* invokePattern = nullptr;
            IUIAutomationExpandCollapsePattern* expandPattern = nullptr;
            bool isTrayChevron = false;

            isOverflow = FindTaskbarOverflowElement(
                automation, pt, taskbarPid,
                &overflowElement, &togglePattern, &invokePattern, &expandPattern, &isTrayChevron);

            // If the mod currently has one flyout open (e.g. App Overflow) and
            // the user moves onto the other button (e.g. Tray Chevron), close the first.
            if (isOverflow && menuOpened && isTrayChevron != activeIsTrayChevron) {
                closeFlyout();
                hoverConsumed = false;
            }

            // Do not toggle a flyout that is already open (for example one the
            // user opened with a click), and do not reopen it for this hover
            // if the user closes it by clicking the button.
            if (isOverflow && IsOverflowOpen(togglePattern, isTrayChevron, taskbarPid)) {
                hoverConsumed = true;
            }

            if (isOverflow && !hoverConsumed) {
                currentHoverTime += 50;

                if (currentHoverTime >= kHoverDelayMs) {
                    HRESULT actionHr = triggerAction(
                        togglePattern, invokePattern, expandPattern, true);

                    if (SUCCEEDED(actionHr)) {
                        menuOpened = true;
                        hoverConsumed = true;
                        openedAt = now;
                        currentCloseHoverTime = 0;
                        clickPending = false;
                        activeIsTrayChevron = isTrayChevron;
                        activeTaskbarPid = taskbarPid;

                        // Keep the button patterns so the flyout can be closed through them.
                        if (chevron) {
                            chevron->Release();
                            chevron = nullptr;
                        }
                        if (chevronInvoke) {
                            chevronInvoke->Release();
                            chevronInvoke = nullptr;
                        }
                        if (chevronExpand) {
                            chevronExpand->Release();
                            chevronExpand = nullptr;
                        }

                        if (togglePattern) {
                            chevron = togglePattern;
                            chevron->AddRef();
                        }
                        if (invokePattern) {
                            chevronInvoke = invokePattern;
                            chevronInvoke->AddRef();
                        }
                        if (expandPattern) {
                            chevronExpand = expandPattern;
                            chevronExpand->AddRef();
                        }

                        // Remember where the button is, so the area between the
                        // button and the menu counts as "still hovering".
                        if (FAILED(overflowElement->get_CurrentBoundingRectangle(
                                &chevronRect))) {
                            chevronRect = {};
                        }
                    } else {
                        Wh_Log(L"Overflow action failed: 0x%08X", actionHr);
                    }

                    currentHoverTime = 0;
                }
            } else {
                currentHoverTime = 0;
            }

            if (expandPattern) expandPattern->Release();
            if (invokePattern) invokePattern->Release();
            if (togglePattern) togglePattern->Release();
            if (overflowElement) overflowElement->Release();
        } else {
            currentHoverTime = 0;
        }

        if (!isOverflow) {
            hoverConsumed = false;
        }

        // Closing logic: while the flyout is open, the safe cursor area is the
        // overflow button, the flyout, and the space between them (their
        // combined bounding box), plus a thumbnail preview that is showing.
        // Nothing is closed while a tray icon's popup menu is showing.
        if (menuOpened) {
            bool flyoutSettled =
                !IsRectEmpty(&flyoutBounds) || now - openedAt >= kOpenGraceMs;

            RECT area = {};
            UnionRect(&area, &chevronRect, &flyoutBounds);
            bool inArea = isOverflow || overXamlHost || PtInRect(&area, pt);

            if (inArea || !flyoutSettled || IsPopupMenuOpen()) {
                currentCloseHoverTime = 0;
            } else {
                currentCloseHoverTime += 50;

                if (currentCloseHoverTime >= kCloseDelayMs) {
                    closeFlyout();
                }
            }
        } else {
            currentCloseHoverTime = 0;
        }
    }

    if (chevron) {
        chevron->Release();
        chevron = nullptr;
    }
    if (chevronInvoke) {
        chevronInvoke->Release();
        chevronInvoke = nullptr;
    }
    if (chevronExpand) {
        chevronExpand->Release();
        chevronExpand = nullptr;
    }

    automation->Release();
    CoUninitialize();
    return 0;
}

BOOL WhTool_ModInit() {
    Wh_Log(L"Initializing Taskbar Overflow on Hover v1.2");

    LoadSettings();

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        return FALSE;
    }

    g_hThread = CreateThread(nullptr, 0, HoverMonitorThread, nullptr, 0, nullptr);
    if (!g_hThread) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModUninit() {
    Wh_Log(L"Unloading Taskbar Overflow on Hover");

    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }

    // Wait without a timeout: the thread must be fully finished before the
    // process continues to tear down.
    if (g_hThread) {
        WaitForSingleObject(g_hThread, INFINITE);
        CloseHandle(g_hThread);
        g_hThread = nullptr;
    }

    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}

// Reload mod settings dynamically when changed in the Windhawk UI.
void WhTool_ModSettingsChanged() {
    LoadSettings();
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
