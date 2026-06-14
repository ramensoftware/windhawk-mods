// ==WindhawkMod==
// @id              taskbar-tray-on-all-monitors
// @name            Taskbar tray on all monitors
// @description     Mirrors the system tray (clock, volume, network, battery, notification icons) from the primary taskbar onto all secondary monitor taskbars
// @version         1.0
// @author          RYJASM
// @github          https://github.com/RYJASM
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion -luuid
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues

// ==WindhawkModReadme==
/*
# Taskbar Tray on All Monitors

Mirrors the system tray from the primary taskbar onto all secondary monitor
taskbars — including the clock, volume, network, battery, notification area
icons, input indicator, overflow chevron, and Show Desktop button.

**Only Windows 11 (build 26200+) is supported.**

## Features

- All system tray icons visible on secondary taskbars (click, hover,
  right-click context menus work natively)
- Dynamic width syncing — secondary tray resizes automatically when icons
  are added or removed on the primary
- Show Desktop button synced to match primary visibility
- Flyout repositioning — Quick Settings, notification panels, and context
  menus open on the correct monitor
- Auto-refresh on display resolution, scale, or DPI changes

## How it works

Hooks into `SystemTrayController` and `SystemTraySecondaryController` in
`SystemTray.dll`, then shares XAML `ItemsSource` bindings from the primary
tray containers to the secondary ones. A `SizeChanged` event on the primary
frame keeps the secondary width in sync at runtime.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#include <winrt/base.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

#include <atomic>
#include <functional>
#include <list>

using namespace winrt::Windows::UI::Xaml;

// ─── globals ────────────────────────────────────────────────────────────────

static std::atomic<bool> g_unloading{false};

// Captured controller pointers
static void* g_primaryController = nullptr;
static void* g_secondaryControllers[8] = {};
static int   g_secondaryControllerCount = 0;

// Track whether we've dumped the XAML tree for primary/secondary
static bool g_primaryTreeDumped = false;
static bool g_secondaryTreeDumped = false;

// Saved XAML tree references for primary/secondary SystemTrayFrame
static FrameworkElement g_primarySystemTrayFrame{nullptr};
static FrameworkElement g_secondarySystemTrayFrame{nullptr};
static bool g_populationAttempted = false;
static double g_lastPrimaryFrameSize = 0;
static winrt::event_token g_primarySizeChangedToken{};

// Hidden window for display change notifications
static HWND g_displayChangeWindow = nullptr;
static const wchar_t* g_displayChangeClassName =
    L"WindhawkTrayMod_DisplayChangeListener";

// Forward declarations
static void RefreshSecondaryTray();
static void InstallDisplayChangeListener();
static void UninstallDisplayChangeListener();

// Loaded event revokers (prevent crash on unload)
using FrameworkElementLoadedEventRevoker = winrt::impl::event_revoker<
    IFrameworkElement,
    &winrt::impl::abi<IFrameworkElement>::type::remove_Loaded>;

std::list<FrameworkElementLoadedEventRevoker> g_autoRevokerList;

// ─── helper: get module version ─────────────────────────────────────────────

static VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;

    HRSRC hResource =
        FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource) {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal) {
            void* pData = LockResource(hGlobal);
            if (pData) {
                VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen);
            }
        }
    }

    if (puPtrLen)
        *puPtrLen = uPtrLen;

    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

// ─── XAML tree utilities ────────────────────────────────────────────────────

FrameworkElement EnumChildElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> enumCallback) {
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            continue;
        }

        if (enumCallback(child)) {
            return child;
        }
    }

    return nullptr;
}

FrameworkElement FindChildByName(FrameworkElement element, PCWSTR name) {
    return EnumChildElements(element, [name](FrameworkElement child) {
        return child.Name() == name;
    });
}

FrameworkElement EnumParentElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> enumCallback) {
    auto parent = element;
    while (true) {
        parent = Media::VisualTreeHelper::GetParent(parent)
                     .try_as<FrameworkElement>();
        if (!parent) {
            return nullptr;
        }

        if (enumCallback(parent)) {
            return parent;
        }
    }
}

FrameworkElement GetParentElementByClassName(FrameworkElement element,
                                             PCWSTR className) {
    return EnumParentElements(element, [className](FrameworkElement parent) {
        return winrt::get_class_name(parent) == className;
    });
}

// ─── XAML tree dump (recursive, depth-limited) ──────────────────────────────

void DumpXamlTree(FrameworkElement element, int depth, int maxDepth) {
    if (depth > maxDepth) return;

    auto name = element.Name();
    auto className = winrt::get_class_name(element);
    auto width = element.ActualWidth();
    auto height = element.ActualHeight();
    auto visibility = element.Visibility();

    wchar_t indent[64] = {};
    for (int i = 0; i < depth && i < 30; i++) {
        wcscat_s(indent, L"  ");
    }

    Wh_Log(L"%s[%s] name=\"%s\" %.0fx%.0f %s",
           indent,
           className.c_str(),
           name.c_str(),
           width, height,
           visibility == Visibility::Collapsed ? L"COLLAPSED" : L"");

    int childCount = Media::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < childCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (child) {
            DumpXamlTree(child, depth + 1, maxDepth);
        }
    }
}

// ─── Flyout repositioning ──────────────────────────────────────────────────
// Detects flyout windows that appear on the primary monitor when the cursor
// is on a secondary monitor, and repositions them to the correct monitor.
// Uses EVENT_OBJECT_LOCATIONCHANGE to persistently reposition windows that
// the system moves back to primary (e.g. Quick Settings XAML island).

static HWINEVENTHOOK g_winEventHook = nullptr;

// Track repositioned flyouts so we can fight back on LOCATIONCHANGE
static HWND g_trackedFlyouts[4] = {};
static HMONITOR g_trackedTargets[4] = {};
static DWORD g_trackedTimes[4] = {};
static int g_trackedCount = 0;

static bool IsPrimaryMonitor(HMONITOR hMon) {
    MONITORINFO mi = {sizeof(mi)};
    return GetMonitorInfo(hMon, &mi) && (mi.dwFlags & MONITORINFOF_PRIMARY);
}

static void TrackFlyout(HWND hwnd, HMONITOR target) {
    for (int i = 0; i < g_trackedCount; i++) {
        if (g_trackedFlyouts[i] == hwnd) {
            g_trackedTargets[i] = target;
            g_trackedTimes[i] = GetTickCount();
            return;
        }
    }
    if (g_trackedCount < 4) {
        g_trackedFlyouts[g_trackedCount] = hwnd;
        g_trackedTargets[g_trackedCount] = target;
        g_trackedTimes[g_trackedCount] = GetTickCount();
        g_trackedCount++;
    }
}

static void UntrackFlyout(HWND hwnd) {
    for (int i = 0; i < g_trackedCount; i++) {
        if (g_trackedFlyouts[i] == hwnd) {
            g_trackedFlyouts[i] = g_trackedFlyouts[--g_trackedCount];
            g_trackedTargets[i] = g_trackedTargets[g_trackedCount];
            g_trackedTimes[i] = g_trackedTimes[g_trackedCount];
            return;
        }
    }
}

static bool IsTrayFlyoutClass(const wchar_t* className) {
    if (wcscmp(className, L"TopLevelWindowForOverflowXamlIsland") == 0)
        return true;
    if (wcscmp(className, L"ControlCenterWindow") == 0)
        return true;
    if (wcscmp(className, L"#32768") == 0)
        return true;
    if (wcscmp(className, L"Shell_LightDismissOverlay") == 0)
        return true;
    if (wcscmp(className, L"XamlExplorerHostIslandWindow") == 0)
        return true;
    if (wcscmp(className, L"Microsoft.UI.Content.DesktopChildSiteBridge") == 0)
        return true;
    if (wcscmp(className, L"Windows.UI.Composition.DesktopWindowContentBridge") == 0)
        return true;
    return false;
}

static void RepositionFlyoutToMonitor(HWND hwnd, HMONITOR fromMon,
                                       HMONITOR toMon) {
    RECT wr;
    if (!GetWindowRect(hwnd, &wr)) return;

    MONITORINFO fromMi = {sizeof(fromMi)};
    MONITORINFO toMi = {sizeof(toMi)};
    if (!GetMonitorInfo(fromMon, &fromMi)) return;
    if (!GetMonitorInfo(toMon, &toMi)) return;

    int ww = wr.right - wr.left;
    int wh = wr.bottom - wr.top;
    int fromMonW = fromMi.rcMonitor.right - fromMi.rcMonitor.left;
    int fromMonH = fromMi.rcMonitor.bottom - fromMi.rcMonitor.top;

    // Log extended style to understand rendering mode
    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    Wh_Log(L"[Flyout] hwnd=%p style=0x%08X exStyle=0x%08X "
           L"WS_EX_NOREDIRECTIONBITMAP=%s",
           hwnd, GetWindowLong(hwnd, GWL_STYLE), exStyle,
           (exStyle & 0x00200000 /*WS_EX_NOREDIRECTIONBITMAP*/) ? L"YES" : L"NO");

    // Check if this is a full-screen overlay (e.g. Quick Settings XAML island)
    if (ww >= fromMonW && wh >= fromMonH) {
        // If the overlay already spans multiple monitors (e.g.
        // Shell_LightDismissOverlay at (0,0,7680,7680)), it already covers
        // the secondary monitor — don't resize/move it or we'll break the
        // internal coordinate system of popups rendered within it.
        int totalW = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        int totalH = GetSystemMetrics(SM_CYVIRTUALSCREEN);
        if (ww > fromMonW || wh > fromMonH) {
            Wh_Log(L"[Flyout] Multi-monitor overlay (%dx%d vs virtual %dx%d)"
                   L" — skipping reposition",
                   ww, wh, totalW, totalH);
            return;
        }

        int toMonW = toMi.rcMonitor.right - toMi.rcMonitor.left;
        int toMonH = toMi.rcMonitor.bottom - toMi.rcMonitor.top;

        Wh_Log(L"[Flyout] Full-screen overlay: moving to (%d,%d %dx%d)",
               toMi.rcMonitor.left, toMi.rcMonitor.top, toMonW, toMonH);

        // Try hide-move-show to force re-render on new monitor
        ShowWindow(hwnd, SW_HIDE);
        BOOL ok = SetWindowPos(hwnd, nullptr,
                               toMi.rcMonitor.left, toMi.rcMonitor.top,
                               toMonW, toMonH,
                               SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
        ShowWindow(hwnd, SW_SHOWNOACTIVATE);

        // Verify the move
        RECT afterRect;
        GetWindowRect(hwnd, &afterRect);
        Wh_Log(L"[Flyout] SetWindowPos returned %d, error=%d, "
               L"after rect=(%d,%d,%d,%d)",
               ok, GetLastError(),
               afterRect.left, afterRect.top,
               afterRect.right, afterRect.bottom);
        return;
    }

    // Normal flyout: position relative to taskbar
    int offRight = fromMi.rcWork.right - wr.right;
    int newX = toMi.rcWork.right - ww - offRight;

    // Find the secondary taskbar to align above it (handles auto-hide)
    int taskbarTop = toMi.rcWork.bottom;
    HWND findHwnd = nullptr;
    while ((findHwnd = FindWindowEx(nullptr, findHwnd,
                                     L"Shell_SecondaryTrayWnd",
                                     nullptr)) != nullptr) {
        HMONITOR trayMon =
            MonitorFromWindow(findHwnd, MONITOR_DEFAULTTONEAREST);
        if (trayMon == toMon) {
            RECT trayRect;
            if (GetWindowRect(findHwnd, &trayRect)) {
                taskbarTop = trayRect.top;
            }
            break;
        }
    }

    int newY = taskbarTop - wh;

    Wh_Log(L"[Flyout] Repositioning from (%d,%d) to (%d,%d)",
           wr.left, wr.top, newX, newY);

    SetWindowPos(hwnd, nullptr, newX, newY, 0, 0,
                 SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

static void CALLBACK FlyoutWinEventProc(HWINEVENTHOOK hWinEventHook,
                                         DWORD event, HWND hwnd,
                                         LONG idObject, LONG idChild,
                                         DWORD idEventThread,
                                         DWORD dwmsEventTime) {
    if (g_unloading) return;
    if (!hwnd || idObject != 0 || idChild != 0) return;

    // EVENT_OBJECT_HIDE: stop tracking
    if (event == EVENT_OBJECT_HIDE) {
        UntrackFlyout(hwnd);
        return;
    }

    // EVENT_OBJECT_LOCATIONCHANGE: reposition tracked flyouts that moved
    // back to primary (system fighting our SetWindowPos)
    if (event == EVENT_OBJECT_LOCATIONCHANGE) {
        for (int i = 0; i < g_trackedCount; i++) {
            if (g_trackedFlyouts[i] != hwnd) continue;

            // Expire tracking after 3 seconds
            if (GetTickCount() - g_trackedTimes[i] > 3000) {
                UntrackFlyout(hwnd);
                return;
            }

            RECT wr;
            if (!GetWindowRect(hwnd, &wr)) return;
            HMONITOR windowMon =
                MonitorFromRect(&wr, MONITOR_DEFAULTTONEAREST);
            if (IsPrimaryMonitor(windowMon)) {
                RepositionFlyoutToMonitor(
                    hwnd, windowMon, g_trackedTargets[i]);
            }
            return;
        }
        return;
    }

    // EVENT_OBJECT_SHOW: detect new flyouts
    if (event != EVENT_OBJECT_SHOW) return;
    if (!IsWindowVisible(hwnd)) return;

    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    if (style & WS_CHILD) return;

    // Check cursor is on a secondary monitor
    POINT cursorPos;
    GetCursorPos(&cursorPos);
    HMONITOR cursorMon =
        MonitorFromPoint(cursorPos, MONITOR_DEFAULTTONEAREST);
    if (IsPrimaryMonitor(cursorMon)) return;

    // Check window is on the primary monitor
    RECT wr;
    if (!GetWindowRect(hwnd, &wr)) return;
    HMONITOR windowMon = MonitorFromRect(&wr, MONITOR_DEFAULTTONEAREST);
    if (!IsPrimaryMonitor(windowMon)) return;

    // Mismatch: cursor on secondary, window on primary
    wchar_t className[256] = {};
    GetClassName(hwnd, className, ARRAYSIZE(className));

    DWORD ownerPid = 0;
    GetWindowThreadProcessId(hwnd, &ownerPid);
    Wh_Log(L"[Flyout] Show on primary while cursor on secondary: "
           L"class='%s' hwnd=%p pid=%u rect=(%d,%d,%d,%d)",
           className, hwnd, ownerPid, wr.left, wr.top, wr.right, wr.bottom);

    if (IsTrayFlyoutClass(className)) {
        int ww = wr.right - wr.left;
        int wh = wr.bottom - wr.top;

        // Skip multi-monitor overlays — they already cover the secondary
        // monitor and repositioning them breaks internal popup coordinates
        MONITORINFO wmi = {sizeof(wmi)};
        GetMonitorInfo(windowMon, &wmi);
        int monW = wmi.rcMonitor.right - wmi.rcMonitor.left;
        int monH = wmi.rcMonitor.bottom - wmi.rcMonitor.top;
        if (ww > monW || wh > monH) {
            Wh_Log(L"[Flyout] Multi-monitor overlay (%dx%d vs mon %dx%d)"
                   L" — skipping",
                   ww, wh, monW, monH);
        } else if (ww > 0 && wh > 0) {
            // Window has real dimensions — reposition now
            RepositionFlyoutToMonitor(hwnd, windowMon, cursorMon);
            TrackFlyout(hwnd, cursorMon);
        } else {
            // Zero-sized at SHOW time — will be positioned later,
            // LOCATIONCHANGE will handle it
            Wh_Log(L"[Flyout] Zero-sized at SHOW, deferring to LOCATIONCHANGE");
            TrackFlyout(hwnd, cursorMon);
        }
    }

    // Also scan for any XamlExplorerHostIslandWindow on primary that we
    // might have missed (pre-existing windows that didn't fire SHOW)
    HWND found = nullptr;
    while ((found = FindWindowEx(nullptr, found,
                                  L"XamlExplorerHostIslandWindow",
                                  nullptr)) != nullptr) {
        if (found == hwnd) continue;  // already handled above
        RECT fr;
        if (!GetWindowRect(found, &fr)) continue;
        if (!IsWindowVisible(found)) continue;
        HMONITOR fMon = MonitorFromRect(&fr, MONITOR_DEFAULTTONEAREST);
        if (IsPrimaryMonitor(fMon)) {
            Wh_Log(L"[Flyout] SCAN: found extra XamlExplorerHostIslandWindow "
                   L"hwnd=%p rect=(%d,%d,%d,%d) on primary",
                   found, fr.left, fr.top, fr.right, fr.bottom);
        }
    }
}

static void InstallFlyoutHook() {
    if (g_winEventHook) return;

    // Cover SHOW (0x8002) through LOCATIONCHANGE (0x800B) to also
    // catch HIDE (0x8003) for cleanup and LOCATIONCHANGE for persistent
    // repositioning of flyouts that the system moves back to primary.
    g_winEventHook = SetWinEventHook(
        EVENT_OBJECT_SHOW, EVENT_OBJECT_LOCATIONCHANGE,
        nullptr, FlyoutWinEventProc,
        0, 0, WINEVENT_OUTOFCONTEXT);

    if (g_winEventHook) {
        Wh_Log(L"[Flyout] WinEvent hook installed (SHOW+LOCATIONCHANGE)");
    } else {
        Wh_Log(L"[Flyout] Failed to install WinEvent hook: error %d",
               GetLastError());
    }
}

static void UninstallFlyoutHook() {
    if (g_winEventHook) {
        UnhookWinEvent(g_winEventHook);
        g_winEventHook = nullptr;
        g_trackedCount = 0;
        Wh_Log(L"[Flyout] WinEvent hook removed");
    }
}

// ─── Identify primary vs secondary from XAML tree ───────────────────────────
// On secondary taskbars, ControlCenterButton exists but is empty (width < 5)

bool IsSecondaryTaskbar(FrameworkElement systemTrayFrame) {
    auto grid = FindChildByName(systemTrayFrame, L"SystemTrayFrameGrid");
    if (!grid) return false;

    auto ccButton = FindChildByName(grid, L"ControlCenterButton");
    if (!ccButton) return false;

    return ccButton.ActualWidth() < 5;
}

// Forward declarations
void ProbeContainerInterfaces(FrameworkElement systemTrayFrame,
                               const wchar_t* label);
void TryPopulateSecondaryTray();

// Forward-declared so TryPopulateSecondaryTray can call UpdateFrameSize
using SystemTraySecondaryController_UpdateFrameSize_t = void(WINAPI*)(void* pThis);
extern SystemTraySecondaryController_UpdateFrameSize_t SystemTraySecondaryController_UpdateFrameSize_Original;

// ─── Fallback: extract SystemTrayFrame from controller object ────────────────
// Scans the controller's member fields for a WinRT reference to the frame.
// Used when mod is loaded after startup (no new IconViews being created).

static bool g_primaryFrameSearchDone = false;
static bool g_secondaryFrameSearchDone = false;

FrameworkElement TryExtractFrameFromController(void* controller,
                                                const wchar_t* label) {
    uintptr_t* slots = (uintptr_t*)controller;

    for (int i = 2; i < 48; i++) {
        // Check slot is readable
        if (IsBadReadPtr(&slots[i], sizeof(uintptr_t))) break;

        uintptr_t value = slots[i];

        // Basic pointer sanity checks
        if (value < 0x10000) continue;
        if ((value & 0x7) != 0) continue;  // must be 8-byte aligned

        // Check pointed-to memory is readable (object header / vtable ptr)
        if (IsBadReadPtr((void*)value, 2 * sizeof(void*))) continue;

        // Check vtable pointer itself is readable
        uintptr_t vtablePtr = *(uintptr_t*)value;
        if (vtablePtr < 0x10000) continue;
        if (IsBadReadPtr((void*)vtablePtr, 3 * sizeof(void*))) continue;

        // Check first vtable entry (QueryInterface) points to executable code
        uintptr_t qiAddr = *(uintptr_t*)vtablePtr;
        MEMORY_BASIC_INFORMATION mbi;
        if (!VirtualQuery((void*)qiAddr, &mbi, sizeof(mbi))) continue;
        if (!(mbi.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ |
                             PAGE_EXECUTE_READWRITE |
                             PAGE_EXECUTE_WRITECOPY)))
            continue;

        // Looks like a valid COM vtable - try QI for FrameworkElement
        try {
            FrameworkElement fe{nullptr};
            HRESULT hr = ((IUnknown*)value)->QueryInterface(
                winrt::guid_of<FrameworkElement>(), winrt::put_abi(fe));
            if (SUCCEEDED(hr) && fe) {
                auto className = winrt::get_class_name(fe);
                if (className == L"SystemTray.SystemTrayFrame") {
                    Wh_Log(L"[%s] Found SystemTrayFrame at controller "
                           L"offset %d (0x%X)",
                           label, i, i * 8);
                    return fe;
                }
            }
        } catch (...) {
            continue;
        }
    }

    Wh_Log(L"[%s] SystemTrayFrame not found in controller object", label);
    return nullptr;
}

void HandleExtractedFrame(FrameworkElement frame, bool isSecondary) {
    const wchar_t* label = isSecondary ? L"SECONDARY" : L"PRIMARY";

    if (isSecondary) {
        g_secondarySystemTrayFrame = frame;
        if (!g_secondaryTreeDumped) {
            g_secondaryTreeDumped = true;
            Wh_Log(L"=== SECONDARY TASKBAR XAML TREE (via controller scan) ===");
            DumpXamlTree(frame, 0, 8);
            ProbeContainerInterfaces(frame, label);
        }
    } else {
        g_primarySystemTrayFrame = frame;
        if (!g_primaryTreeDumped) {
            g_primaryTreeDumped = true;
            Wh_Log(L"=== PRIMARY TASKBAR XAML TREE (via controller scan) ===");
            DumpXamlTree(frame, 0, 8);
            ProbeContainerInterfaces(frame, label);
        }
    }

    if (g_primarySystemTrayFrame && g_secondarySystemTrayFrame) {
        TryPopulateSecondaryTray();
    }
}

// ─── Probe containers for WinRT interfaces ──────────────────────────────────

void ProbeContainerInterfaces(FrameworkElement systemTrayFrame,
                               const wchar_t* label) {
    auto grid = FindChildByName(systemTrayFrame, L"SystemTrayFrameGrid");
    if (!grid) {
        Wh_Log(L"[%s] No SystemTrayFrameGrid found", label);
        return;
    }

    int childCount = Media::VisualTreeHelper::GetChildrenCount(grid);
    for (int i = 0; i < childCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(grid, i)
                         .try_as<FrameworkElement>();
        if (!child) continue;

        auto name = child.Name();
        auto className = winrt::get_class_name(child);

        // Probe standard XAML interfaces
        auto asItemsControl = child.try_as<Controls::ItemsControl>();
        auto asContentControl = child.try_as<Controls::ContentControl>();
        auto asPanel = child.try_as<Controls::Panel>();

        int itemsCount = -1;
        bool hasItemsSource = false;
        if (asItemsControl) {
            itemsCount = (int)asItemsControl.Items().Size();
            hasItemsSource = (asItemsControl.ItemsSource() != nullptr);
        }

        Wh_Log(L"[%s] '%s' (%s) - ItemsControl:%s ContentControl:%s Panel:%s"
               L" Items:%d ItemsSource:%s",
               label, name.c_str(), className.c_str(),
               asItemsControl ? L"YES" : L"NO",
               asContentControl ? L"YES" : L"NO",
               asPanel ? L"YES" : L"NO",
               itemsCount,
               hasItemsSource ? L"YES" : L"NO");
    }
}

// ─── Try to populate secondary tray from primary ────────────────────────────

// Track what we changed so we can undo on unload
static bool g_secondaryContainersUncollapsed = false;

// Containers whose visibility is synced between primary and secondary
static const wchar_t* g_syncedContainers[] = {
    L"ControlCenterButton",
    L"NotificationCenterButton",
    L"NotifyIconStack",
    L"NonActivatableStack",
    L"MainStack",
    L"ShowDesktopStack",
};

// Stack containers that use Content > IconStack for ItemsSource sharing
static const wchar_t* g_stackContainers[] = {
    L"NotifyIconStack",
    L"NonActivatableStack",
    L"MainStack",
    L"ShowDesktopStack",
};

void TryPopulateSecondaryTray() {
    if (g_populationAttempted) return;
    g_populationAttempted = true;

    Wh_Log(L"=== ATTEMPTING TO POPULATE SECONDARY TRAY ===");

    auto primaryGrid =
        FindChildByName(g_primarySystemTrayFrame, L"SystemTrayFrameGrid");
    auto secondaryGrid =
        FindChildByName(g_secondarySystemTrayFrame, L"SystemTrayFrameGrid");
    if (!primaryGrid || !secondaryGrid) {
        Wh_Log(L"Missing grid(s), aborting");
        return;
    }

    // Log primary frame dimensions for reference
    double primaryFrameWidth = g_primarySystemTrayFrame.ActualWidth();
    double secondaryFrameWidth = g_secondarySystemTrayFrame.ActualWidth();
    Wh_Log(L"Frame widths - primary:%.0f secondary:%.0f",
           primaryFrameWidth, secondaryFrameWidth);

    // Step 1: Sync visibility of containers between primary and secondary
    for (auto containerName : g_syncedContainers) {
        auto primaryContainer = FindChildByName(primaryGrid, containerName);
        auto secondaryContainer = FindChildByName(secondaryGrid, containerName);

        if (!primaryContainer || !secondaryContainer) {
            Wh_Log(L"[%s] Missing on one side", containerName);
            continue;
        }

        auto primaryVis = primaryContainer.Visibility();
        auto secondaryVis = secondaryContainer.Visibility();
        double primaryW = primaryContainer.ActualWidth();
        double secondaryW = secondaryContainer.ActualWidth();

        Wh_Log(L"[%s] primary: %.0fx%.0f %s | secondary: %.0fx%.0f %s",
               containerName,
               primaryW, primaryContainer.ActualHeight(),
               primaryVis == Visibility::Collapsed ? L"COLLAPSED" : L"Visible",
               secondaryW, secondaryContainer.ActualHeight(),
               secondaryVis == Visibility::Collapsed ? L"COLLAPSED" : L"Visible");

        // Sync visibility: match secondary to primary
        if (primaryVis == Visibility::Visible &&
            secondaryVis == Visibility::Collapsed) {
            Wh_Log(L"[%s] Uncollapsing secondary container", containerName);
            secondaryContainer.Visibility(Visibility::Visible);
            g_secondaryContainersUncollapsed = true;
        } else if (primaryVis == Visibility::Collapsed &&
                   secondaryVis == Visibility::Visible) {
            Wh_Log(L"[%s] Collapsing secondary to match primary", containerName);
            secondaryContainer.Visibility(Visibility::Collapsed);
            g_secondaryContainersUncollapsed = true;
        }

        // If secondary is visible but very narrow (< 5px), it might be
        // effectively hidden - log this for diagnosis
        if (secondaryVis == Visibility::Visible && secondaryW < 5 &&
            primaryW > 5) {
            Wh_Log(L"[%s] Secondary is visible but very narrow (%.0f vs %.0f)",
                   containerName, secondaryW, primaryW);
        }
    }

    // Step 2: Share NotificationAreaIcons ItemsSource for third-party tray icons
    // NotificationAreaIcons is a DIRECT child of SystemTrayFrameGrid (sibling
    // of NotifyIconStack, not inside it)
    auto primaryNAI = FindChildByName(primaryGrid, L"NotificationAreaIcons");
    auto secondaryNAI = FindChildByName(secondaryGrid, L"NotificationAreaIcons");

    if (primaryNAI && secondaryNAI) {
        auto primaryIC = primaryNAI.try_as<Controls::ItemsControl>();
        auto secondaryIC = secondaryNAI.try_as<Controls::ItemsControl>();

        if (primaryIC && secondaryIC) {
            int pCount = (int)primaryIC.Items().Size();
            int sCount = (int)secondaryIC.Items().Size();
            auto pSource = primaryIC.ItemsSource();

            Wh_Log(L"[NotificationAreaIcons] primary:%d secondary:%d "
                   L"primarySource:%s",
                   pCount, sCount, pSource ? L"YES" : L"NO");

            if (pSource && sCount == 0 && pCount > 0) {
                try {
                    secondaryIC.ItemsSource(pSource);
                    int newCount = (int)secondaryIC.Items().Size();
                    Wh_Log(L"[NotificationAreaIcons] ItemsSource shared! "
                           L"Secondary now has %d items",
                           newCount);
                } catch (winrt::hresult_error const& ex) {
                    Wh_Log(L"[NotificationAreaIcons] Share failed: 0x%08X %s",
                           (unsigned)ex.code(), ex.message().c_str());
                }
            }

            // Uncollapse if needed
            if (secondaryNAI.Visibility() == Visibility::Collapsed &&
                primaryNAI.Visibility() == Visibility::Visible) {
                secondaryNAI.Visibility(Visibility::Visible);
                Wh_Log(L"[NotificationAreaIcons] Uncollapsed secondary");
            }
        }
    } else {
        Wh_Log(L"[NotificationAreaIcons] Not found (primary:%s secondary:%s)",
               primaryNAI ? L"found" : L"MISSING",
               secondaryNAI ? L"found" : L"MISSING");
    }

    // Step 3: Share ItemsSource for ControlCenterButton and
    //         NotificationCenterButton (in case secondary has 0 items)
    const wchar_t* icContainers[] = {
        L"ControlCenterButton",
        L"NotificationCenterButton",
    };
    for (auto name : icContainers) {
        auto primaryC = FindChildByName(primaryGrid, name);
        auto secondaryC = FindChildByName(secondaryGrid, name);
        if (!primaryC || !secondaryC) continue;

        auto pIC = primaryC.try_as<Controls::ItemsControl>();
        auto sIC = secondaryC.try_as<Controls::ItemsControl>();
        if (!pIC || !sIC) continue;

        int pCount = (int)pIC.Items().Size();
        int sCount = (int)sIC.Items().Size();
        auto pSource = pIC.ItemsSource();

        if (pSource && sCount == 0 && pCount > 0) {
            try {
                sIC.ItemsSource(pSource);
                Wh_Log(L"[%s] ItemsSource shared! Secondary now has %d items",
                       name, (int)sIC.Items().Size());
            } catch (winrt::hresult_error const& ex) {
                Wh_Log(L"[%s] ItemsSource share failed: 0x%08X",
                       name, (unsigned)ex.code());
            }
        }
    }

    // Step 3b: Share StackListView ItemsSource for Stack containers
    // SystemTray.Stack is NOT an ItemsControl, but it contains a child
    // StackListView (via Content grid > IconStack) which IS an ItemsControl.
    for (auto stackName : g_stackContainers) {
        auto primaryStack = FindChildByName(primaryGrid, stackName);
        auto secondaryStack = FindChildByName(secondaryGrid, stackName);
        if (!primaryStack || !secondaryStack) continue;

        // Drill down: Stack > Content (Grid) > IconStack (StackListView)
        auto primaryContent = FindChildByName(primaryStack, L"Content");
        auto secondaryContent = FindChildByName(secondaryStack, L"Content");
        if (!primaryContent || !secondaryContent) continue;

        auto primaryIconStack = FindChildByName(primaryContent, L"IconStack");
        auto secondaryIconStack = FindChildByName(secondaryContent, L"IconStack");
        if (!primaryIconStack || !secondaryIconStack) continue;

        auto pIC = primaryIconStack.try_as<Controls::ItemsControl>();
        auto sIC = secondaryIconStack.try_as<Controls::ItemsControl>();

        Wh_Log(L"[%s/IconStack] primary IC:%s secondary IC:%s",
               stackName,
               pIC ? L"YES" : L"NO",
               sIC ? L"YES" : L"NO");

        if (pIC && sIC) {
            int pCount = (int)pIC.Items().Size();
            int sCount = (int)sIC.Items().Size();
            auto pSource = pIC.ItemsSource();

            Wh_Log(L"[%s/IconStack] primary:%d secondary:%d source:%s",
                   stackName, pCount, sCount, pSource ? L"YES" : L"NO");

            if (pSource && sCount == 0 && pCount > 0) {
                try {
                    sIC.ItemsSource(pSource);
                    int newCount = (int)sIC.Items().Size();
                    Wh_Log(L"[%s/IconStack] ItemsSource shared! "
                           L"Secondary now has %d items",
                           stackName, newCount);
                } catch (winrt::hresult_error const& ex) {
                    Wh_Log(L"[%s/IconStack] Share failed: 0x%08X %s",
                           stackName, (unsigned)ex.code(),
                           ex.message().c_str());
                }
            }
        }
    }

    // Step 4: Set the secondary frame width to match primary
    // (GetFrameSize symbol may not exist, so set Width directly)
    if (primaryFrameWidth > secondaryFrameWidth) {
        Wh_Log(L"Setting secondary frame Width: %.0f -> %.0f",
               secondaryFrameWidth, primaryFrameWidth);
        g_secondarySystemTrayFrame.Width(primaryFrameWidth);
    }

    // Force the secondary controller to recalculate frame size
    if (g_secondaryControllerCount > 0 &&
        SystemTraySecondaryController_UpdateFrameSize_Original) {
        Wh_Log(L"Forcing UpdateFrameSize on secondary controller");
        SystemTraySecondaryController_UpdateFrameSize_Original(
            g_secondaryControllers[0]);
    }

    // Also force layout update on secondary frame
    g_secondarySystemTrayFrame.InvalidateMeasure();
    g_secondarySystemTrayFrame.InvalidateArrange();
    g_secondarySystemTrayFrame.UpdateLayout();

    // Also invalidate the grid
    secondaryGrid.InvalidateMeasure();
    secondaryGrid.InvalidateArrange();
    secondaryGrid.UpdateLayout();

    // Step 5: Log final state
    Wh_Log(L"=== POST-POPULATION STATE ===");
    int gridChildCount = Media::VisualTreeHelper::GetChildrenCount(secondaryGrid);
    for (int i = 0; i < gridChildCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(secondaryGrid, i)
                         .try_as<FrameworkElement>();
        if (!child) continue;
        auto cname = child.Name();
        auto cclass = winrt::get_class_name(child);
        Wh_Log(L"  [%s] (%s) %.0fx%.0f %s",
               cname.c_str(), cclass.c_str(),
               child.ActualWidth(), child.ActualHeight(),
               child.Visibility() == Visibility::Collapsed ? L"COLLAPSED" : L"Visible");
    }
    Wh_Log(L"Secondary frame size now: %.0fx%.0f",
           g_secondarySystemTrayFrame.ActualWidth(),
           g_secondarySystemTrayFrame.ActualHeight());

    Wh_Log(L"=== POPULATION ATTEMPT COMPLETE ===");

    // Step 6: Watch primary frame for size changes so we can sync to secondary
    g_primarySizeChangedToken = g_primarySystemTrayFrame.SizeChanged(
        [](winrt::Windows::Foundation::IInspectable const& sender,
           SizeChangedEventArgs const& args) {
            if (g_unloading || !g_secondarySystemTrayFrame) return;

            double newWidth = args.NewSize().Width;
            double oldWidth = args.PreviousSize().Width;
            if (newWidth == oldWidth) return;

            Wh_Log(L"[PrimarySizeChanged] %.0f -> %.0f", oldWidth, newWidth);

            // Update the cached primary size
            g_lastPrimaryFrameSize = newWidth;

            // Apply to secondary frame
            double secWidth = g_secondarySystemTrayFrame.Width();
            if (secWidth != newWidth) {
                Wh_Log(L"[PrimarySizeChanged] Syncing secondary: %.0f -> %.0f",
                       secWidth, newWidth);
                g_secondarySystemTrayFrame.Width(newWidth);
                g_secondarySystemTrayFrame.InvalidateMeasure();
                g_secondarySystemTrayFrame.InvalidateArrange();
                auto parent = Media::VisualTreeHelper::GetParent(
                    g_secondarySystemTrayFrame).try_as<FrameworkElement>();
                while (parent) {
                    parent.InvalidateMeasure();
                    parent.InvalidateArrange();
                    parent = Media::VisualTreeHelper::GetParent(parent)
                        .try_as<FrameworkElement>();
                }
            }
        });
    Wh_Log(L"[SizeChanged] Registered on primary frame");

    // Install flyout repositioning hook now that secondary tray is populated
    InstallFlyoutHook();
}

// ─── Process a loaded IconView element ──────────────────────────────────────

void HandleLoadedIconView(FrameworkElement iconView) {
    // Navigate up to find SystemTray.SystemTrayFrame
    auto systemTrayFrame = GetParentElementByClassName(
        iconView, L"SystemTray.SystemTrayFrame");
    if (!systemTrayFrame) {
        Wh_Log(L"IconView loaded but no SystemTrayFrame parent found");
        return;
    }

    bool isSecondary = IsSecondaryTaskbar(systemTrayFrame);

    if (isSecondary) {
        if (!g_secondaryTreeDumped) {
            g_secondaryTreeDumped = true;
            g_secondarySystemTrayFrame = systemTrayFrame;
            Wh_Log(L"=== SECONDARY TASKBAR XAML TREE ===");
            DumpXamlTree(systemTrayFrame, 0, 8);
            ProbeContainerInterfaces(systemTrayFrame, L"SECONDARY");
        }
    } else {
        if (!g_primaryTreeDumped) {
            g_primaryTreeDumped = true;
            g_primarySystemTrayFrame = systemTrayFrame;
            Wh_Log(L"=== PRIMARY TASKBAR XAML TREE ===");
            DumpXamlTree(systemTrayFrame, 0, 8);
            ProbeContainerInterfaces(systemTrayFrame, L"PRIMARY");
        }
    }

    // Once we have both, try to populate the secondary tray
    if (g_primarySystemTrayFrame && g_secondarySystemTrayFrame) {
        TryPopulateSecondaryTray();
    }
}

// ─── SystemTrayController hooks ─────────────────────────────────────────────

// Constructor: captures primary controller this pointer
using SystemTrayController_ctor_t = void*(WINAPI*)(void* pThis, void* taskbarModel);
SystemTrayController_ctor_t SystemTrayController_ctor_Original;
void* WINAPI SystemTrayController_ctor_Hook(void* pThis, void* taskbarModel) {
    Wh_Log(L">>> SystemTrayController::ctor this=%p", pThis);
    g_primaryController = pThis;
    void* ret = SystemTrayController_ctor_Original(pThis, taskbarModel);
    Wh_Log(L">>> SystemTrayController::ctor done, ret=%p", ret);
    return ret;
}

// InitializeMainStack
using SystemTrayController_InitializeMainStack_t = void(WINAPI*)(void* pThis);
SystemTrayController_InitializeMainStack_t SystemTrayController_InitializeMainStack_Original;
void WINAPI SystemTrayController_InitializeMainStack_Hook(void* pThis) {
    Wh_Log(L">>> SystemTrayController::InitializeMainStack this=%p", pThis);
    g_primaryController = pThis;
    SystemTrayController_InitializeMainStack_Original(pThis);
}

// UpdateFrameSize - captures controller after startup
using SystemTrayController_UpdateFrameSize_t = void(WINAPI*)(void* pThis);
SystemTrayController_UpdateFrameSize_t SystemTrayController_UpdateFrameSize_Original;
void WINAPI SystemTrayController_UpdateFrameSize_Hook(void* pThis) {
    if (!g_primaryController) {
        g_primaryController = pThis;
        Wh_Log(L">>> Primary controller captured via UpdateFrameSize: %p", pThis);
    }
    SystemTrayController_UpdateFrameSize_Original(pThis);

    // Fallback: extract frame from controller if not yet captured
    if (!g_primarySystemTrayFrame && !g_primaryFrameSearchDone && !g_unloading) {
        g_primaryFrameSearchDone = true;
        auto frame = TryExtractFrameFromController(pThis, L"PRIMARY");
        if (frame) {
            HandleExtractedFrame(frame, false);
        }
    }
}

// ─── SystemTraySecondaryController hooks ────────────────────────────────────

// Constructor
using SystemTraySecondaryController_ctor_t = void*(WINAPI*)(void* pThis, void* taskbarModel);
SystemTraySecondaryController_ctor_t SystemTraySecondaryController_ctor_Original;
void* WINAPI SystemTraySecondaryController_ctor_Hook(void* pThis, void* taskbarModel) {
    Wh_Log(L">>> SystemTraySecondaryController::ctor this=%p", pThis);

    if (g_secondaryControllerCount < 8) {
        g_secondaryControllers[g_secondaryControllerCount++] = pThis;
    }

    void* ret = SystemTraySecondaryController_ctor_Original(pThis, taskbarModel);
    Wh_Log(L">>> SystemTraySecondaryController::ctor done, ret=%p", ret);
    return ret;
}

// UpdateFrameSize
using SystemTraySecondaryController_UpdateFrameSize_t = void(WINAPI*)(void* pThis);
SystemTraySecondaryController_UpdateFrameSize_t SystemTraySecondaryController_UpdateFrameSize_Original;
void WINAPI SystemTraySecondaryController_UpdateFrameSize_Hook(void* pThis) {
    // Track this secondary controller
    bool found = false;
    for (int i = 0; i < g_secondaryControllerCount; i++) {
        if (g_secondaryControllers[i] == pThis) { found = true; break; }
    }
    if (!found && g_secondaryControllerCount < 8) {
        g_secondaryControllers[g_secondaryControllerCount++] = pThis;
        Wh_Log(L">>> Secondary controller captured via UpdateFrameSize: %p", pThis);
    }
    SystemTraySecondaryController_UpdateFrameSize_Original(pThis);

    // Fallback: extract frame from controller if not yet captured
    if (!g_secondarySystemTrayFrame && !g_secondaryFrameSearchDone && !g_unloading) {
        g_secondaryFrameSearchDone = true;
        auto frame = TryExtractFrameFromController(pThis, L"SECONDARY");
        if (frame) {
            HandleExtractedFrame(frame, true);
        }
    }

    // Re-apply width override after the original sets it to native value
    if (g_populationAttempted && g_secondarySystemTrayFrame &&
        g_lastPrimaryFrameSize > 0 && !g_unloading) {
        double currentWidth = g_secondarySystemTrayFrame.Width();
        if (currentWidth != g_lastPrimaryFrameSize) {
            g_secondarySystemTrayFrame.Width(g_lastPrimaryFrameSize);
        }
    }
}

// GetFrameSize - override to match primary tray width when populated
using SystemTrayController_GetFrameSize_t = double(WINAPI*)(void* pThis, int enumTaskbarSize);
SystemTrayController_GetFrameSize_t SystemTrayController_GetFrameSize_Original;
double WINAPI SystemTrayController_GetFrameSize_Hook(void* pThis, int enumTaskbarSize) {
    double size = SystemTrayController_GetFrameSize_Original(pThis, enumTaskbarSize);
    g_lastPrimaryFrameSize = size;
    return size;
}

using SystemTraySecondaryController_GetFrameSize_t = double(WINAPI*)(void* pThis, int enumTaskbarSize);
SystemTraySecondaryController_GetFrameSize_t SystemTraySecondaryController_GetFrameSize_Original;
double WINAPI SystemTraySecondaryController_GetFrameSize_Hook(void* pThis, int enumTaskbarSize) {
    double originalSize = SystemTraySecondaryController_GetFrameSize_Original(pThis, enumTaskbarSize);

    if (g_populationAttempted && g_lastPrimaryFrameSize > originalSize) {
        return g_lastPrimaryFrameSize;
    }

    return originalSize;
}

// ─── IconView::IconView hook ────────────────────────────────────────────────

using IconView_IconView_t = void*(WINAPI*)(void* pThis);
IconView_IconView_t IconView_IconView_Original;
void* WINAPI IconView_IconView_Hook(void* pThis) {
    void* ret = IconView_IconView_Original(pThis);

    if (g_unloading) return ret;

    // Get FrameworkElement from IconView WinRT object
    // WinRT implementation objects have IInspectable at ((IUnknown**)pThis)[1]
    FrameworkElement iconView = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(iconView));
    if (!iconView) {
        return ret;
    }

    Wh_Log(L">>> IconView created: %p", pThis);

    // Hook the Loaded event to explore the tree once the element is in the visual tree
    g_autoRevokerList.emplace_back();
    auto autoRevokerIt = g_autoRevokerList.end();
    --autoRevokerIt;

    *autoRevokerIt = iconView.Loaded(
        winrt::auto_revoke_t{},
        [autoRevokerIt](winrt::Windows::Foundation::IInspectable const& sender,
                        RoutedEventArgs const& e) {
            g_autoRevokerList.erase(autoRevokerIt);

            if (g_unloading) return;

            auto iconView = sender.try_as<FrameworkElement>();
            if (!iconView) return;

            auto className = winrt::get_class_name(iconView);
            auto name = iconView.Name();
            Wh_Log(L">>> IconView Loaded: class=%s name=%s",
                   className.c_str(), name.c_str());

            HandleLoadedIconView(iconView);
        });

    return ret;
}

// ─── module hooking ─────────────────────────────────────────────────────────

bool HookSystemTraySymbols(HMODULE module) {
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        // Primary controller
        {
            {LR"(public: __cdecl winrt::SystemTray::implementation::SystemTrayController::SystemTrayController(struct winrt::WindowsUdk::UI::Shell::TaskbarModel const &))"},
            &SystemTrayController_ctor_Original,
            SystemTrayController_ctor_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::SystemTray::implementation::SystemTrayController::InitializeMainStack(void))"},
            &SystemTrayController_InitializeMainStack_Original,
            SystemTrayController_InitializeMainStack_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::SystemTray::implementation::SystemTrayController::UpdateFrameSize(void))"},
            &SystemTrayController_UpdateFrameSize_Original,
            SystemTrayController_UpdateFrameSize_Hook,
            true,
        },
        {
            {LR"(private: double __cdecl winrt::SystemTray::implementation::SystemTrayController::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
            &SystemTrayController_GetFrameSize_Original,
            SystemTrayController_GetFrameSize_Hook,
            true,
        },
        // Secondary controller
        {
            {LR"(public: __cdecl winrt::SystemTray::implementation::SystemTraySecondaryController::SystemTraySecondaryController(struct winrt::WindowsUdk::UI::Shell::TaskbarModel const &))"},
            &SystemTraySecondaryController_ctor_Original,
            SystemTraySecondaryController_ctor_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::SystemTray::implementation::SystemTraySecondaryController::UpdateFrameSize(void))"},
            &SystemTraySecondaryController_UpdateFrameSize_Original,
            SystemTraySecondaryController_UpdateFrameSize_Hook,
            true,
        },
        {
            {LR"(private: double __cdecl winrt::SystemTray::implementation::SystemTraySecondaryController::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
            &SystemTraySecondaryController_GetFrameSize_Original,
            SystemTraySecondaryController_GetFrameSize_Hook,
            true,
        },
        // IconView constructor - key hook for XAML tree access
        {
            {LR"(public: __cdecl winrt::SystemTray::implementation::IconView::IconView(void))"},
            &IconView_IconView_Original,
            IconView_IconView_Hook,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

// ─── module discovery ───────────────────────────────────────────────────────

static HMODULE GetSystemTrayModuleHandle() {
    HMODULE module = GetModuleHandle(L"SystemTray.dll");
    if (!module) {
        module = GetModuleHandle(L"Taskbar.View.dll");
        if (module) {
            VS_FIXEDFILEINFO* fixedFileInfo =
                GetModuleVersionInfo(module, nullptr);
            WORD moduleMajor =
                fixedFileInfo ? HIWORD(fixedFileInfo->dwFileVersionMS) : 0;
            if (!moduleMajor || moduleMajor >= 2604) {
                Wh_Log(L"Skipping Taskbar.View.dll version %d", moduleMajor);
                module = nullptr;
            }
        }
    }
    return module;
}

static void HandleLoadedModuleIfSystemTray(HMODULE module,
                                            LPCWSTR lpLibFileName) {
    if (!lpLibFileName) return;

    LPCWSTR fileName = wcsrchr(lpLibFileName, L'\\');
    fileName = fileName ? fileName + 1 : lpLibFileName;

    if (_wcsicmp(fileName, L"SystemTray.dll") == 0 ||
        _wcsicmp(fileName, L"Taskbar.View.dll") == 0) {
        Wh_Log(L"SystemTray module loaded: %s", fileName);
        if (HookSystemTraySymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module && !g_unloading) {
        HandleLoadedModuleIfSystemTray(module, lpLibFileName);
    }
    return module;
}

// ─── Windhawk callbacks ─────────────────────────────────────────────────────

BOOL Wh_ModInit() {
    Wh_Log(L">");

    HMODULE systemTrayModule = GetSystemTrayModuleHandle();
    if (systemTrayModule) {
        Wh_Log(L"SystemTray module already loaded");
        if (!HookSystemTraySymbols(systemTrayModule)) {
            Wh_Log(L"Failed to hook SystemTray symbols");
            return FALSE;
        }
    } else {
        Wh_Log(L"SystemTray module not loaded yet, hooking LoadLibraryExW");

        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                      "LoadLibraryExW");
        WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");
    Wh_Log(L"Primary controller: %p", g_primaryController);
    Wh_Log(L"Secondary controllers: %d", g_secondaryControllerCount);

    // If the mod was loaded after the taskbar is already running, we need to
    // force the taskbar to rebuild so our constructor and UpdateFrameSize
    // hooks fire. Broadcast WM_SETTINGCHANGE with "TraySettings" to trigger
    // the taskbar to re-read its configuration and rebuild its XAML tree.
    if (!g_primaryController && !g_unloading) {
        Wh_Log(L"No controllers captured during init - broadcasting "
               L"WM_SETTINGCHANGE to trigger taskbar rebuild");

        SendNotifyMessage(HWND_BROADCAST, WM_SETTINGCHANGE,
                          0, (LPARAM)L"TraySettings");
    }

    InstallDisplayChangeListener();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");
    g_unloading = true;

    // Remove display change listener
    UninstallDisplayChangeListener();

    // Remove flyout repositioning hook
    UninstallFlyoutHook();

    // Remove SizeChanged listener from primary frame
    if (g_primarySystemTrayFrame && g_primarySizeChangedToken.value) {
        g_primarySystemTrayFrame.SizeChanged(g_primarySizeChangedToken);
        g_primarySizeChangedToken = {};
    }

    // Clear loaded event revokers
    g_autoRevokerList.clear();

    // Restore secondary tray to original state
    if (g_secondaryContainersUncollapsed && g_secondarySystemTrayFrame) {
        try {
            auto grid = FindChildByName(g_secondarySystemTrayFrame,
                                         L"SystemTrayFrameGrid");
            if (grid) {
                // Re-collapse containers that we modified
                for (auto name : g_syncedContainers) {
                    auto child = FindChildByName(grid, name);
                    if (child && child.ActualWidth() > 0) {
                        // Only re-collapse if it was originally collapsed
                        // (we can't perfectly know, but narrow ones likely were)
                        child.Visibility(Visibility::Collapsed);
                    }
                }

                // Clear shared ItemsSource on NotificationAreaIcons
                // (it's a direct child of SystemTrayFrameGrid)
                auto nai = FindChildByName(grid, L"NotificationAreaIcons");
                if (nai) {
                    auto ic = nai.try_as<Controls::ItemsControl>();
                    if (ic) {
                        ic.ItemsSource(nullptr);
                    }
                }

                // Restore frame width and force layout update
                g_secondarySystemTrayFrame.ClearValue(
                    FrameworkElement::WidthProperty());
                g_secondarySystemTrayFrame.InvalidateMeasure();
                g_secondarySystemTrayFrame.UpdateLayout();
            }
        } catch (...) {
            Wh_Log(L"Error during cleanup");
        }
    }

    // Release XAML references
    g_primarySystemTrayFrame = nullptr;
    g_secondarySystemTrayFrame = nullptr;
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

// ─── refresh helper ─────────────────────────────────────────────────────────

static void RefreshSecondaryTray() {
    if (g_unloading) return;

    if (g_primarySystemTrayFrame && g_secondarySystemTrayFrame) {
        double primaryWidth = g_primarySystemTrayFrame.ActualWidth();
        if (primaryWidth > 0) {
            Wh_Log(L"[Refresh] Syncing secondary width to %.0f", primaryWidth);
            g_lastPrimaryFrameSize = primaryWidth;
            g_secondarySystemTrayFrame.Width(primaryWidth);
            g_secondarySystemTrayFrame.InvalidateMeasure();
            g_secondarySystemTrayFrame.InvalidateArrange();

            // Walk up the visual tree to force parent re-layout
            auto parent = Media::VisualTreeHelper::GetParent(
                g_secondarySystemTrayFrame).try_as<FrameworkElement>();
            while (parent) {
                parent.InvalidateMeasure();
                parent.InvalidateArrange();
                parent = Media::VisualTreeHelper::GetParent(parent)
                    .try_as<FrameworkElement>();
            }

            g_secondarySystemTrayFrame.UpdateLayout();
        }
    }
}

// ─── display change listener ────────────────────────────────────────────────

static LRESULT CALLBACK DisplayChangeWndProc(HWND hwnd, UINT msg,
                                              WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_DISPLAYCHANGE:
            Wh_Log(L"[DisplayChange] Resolution changed: %dx%d bpp=%d",
                   LOWORD(lParam), HIWORD(lParam), (int)wParam);
            RefreshSecondaryTray();
            return 0;

        case WM_DPICHANGED:
            Wh_Log(L"[DisplayChange] DPI changed: X=%d Y=%d",
                   LOWORD(wParam), HIWORD(wParam));
            RefreshSecondaryTray();
            return 0;

        case WM_SETTINGCHANGE:
            // Windows fires WM_SETTINGCHANGE on various display/DPI changes
            if (lParam) {
                auto param = reinterpret_cast<const wchar_t*>(lParam);
                // "WindowMetrics" fires on DPI/scale changes
                if (wcscmp(param, L"WindowMetrics") == 0) {
                    Wh_Log(L"[DisplayChange] WM_SETTINGCHANGE: %s", param);
                    RefreshSecondaryTray();
                }
            }
            return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

static void InstallDisplayChangeListener() {
    if (g_displayChangeWindow) return;

    WNDCLASS wc = {};
    wc.lpfnWndProc = DisplayChangeWndProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = g_displayChangeClassName;

    RegisterClass(&wc);

    g_displayChangeWindow = CreateWindowEx(
        0, g_displayChangeClassName, L"", 0,
        0, 0, 0, 0,
        HWND_MESSAGE,  // message-only window
        nullptr, wc.hInstance, nullptr);

    if (g_displayChangeWindow) {
        Wh_Log(L"[DisplayChange] Listener window installed");
    } else {
        Wh_Log(L"[DisplayChange] Failed to create listener window: %d",
               GetLastError());
    }
}

static void UninstallDisplayChangeListener() {
    if (g_displayChangeWindow) {
        DestroyWindow(g_displayChangeWindow);
        g_displayChangeWindow = nullptr;
    }
    UnregisterClass(g_displayChangeClassName, GetModuleHandle(nullptr));
}

