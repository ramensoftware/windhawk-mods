// ==WindhawkMod==
// @id              taskbar-tray-on-all-monitors
// @name            Taskbar tray on all monitors
// @description     Mirrors the system tray (clock, volume, network, battery, notification icons) from the primary taskbar onto all secondary monitor taskbars
// @version         1.1
// @author          RYJASM
// @github          https://github.com/RYJASM
// @include         explorer.exe
// @include         ShellHost.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion -luuid -lshcore -lcomctl32
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

Supports any number of monitors (2, 3, 4, etc.).

**Only Windows 11 (build 26200+) is supported.**

## Features

- All system tray icons visible on every secondary taskbar (click, hover,
  right-click context menus work natively)
- Multi-monitor support — works with 3+ displays, not just 2
- Dynamic width syncing — all secondary trays resize automatically when icons
  are added or removed on the primary
- Show Desktop button synced to match primary visibility
- Flyout repositioning — Quick Settings, notification panels, and context
  menus open on the correct monitor
- Auto-refresh on display resolution, scale, or DPI changes

## How it works

Hooks into `SystemTrayController` and `SystemTraySecondaryController` in
`SystemTray.dll`, then shares XAML `ItemsSource` bindings from the primary
tray containers to each secondary. A `SizeChanged` event on the primary
frame keeps all secondary widths in sync at runtime.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#include <commctrl.h>
#include <shellapi.h>
#include <ShellScalingApi.h>
#include <windowsx.h>
#include <winrt/base.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

#include <algorithm>
#include <atomic>
#include <functional>
#include <list>
#include <vector>

using namespace winrt::Windows::UI::Xaml;

// ─── globals ────────────────────────────────────────────────────────────────

static std::atomic<bool> g_unloading{false};

// Captured primary controller pointer
static void* g_primaryController = nullptr;

// Track whether we've dumped the primary XAML tree
static bool g_primaryTreeDumped = false;

// Primary SystemTrayFrame reference
static FrameworkElement g_primarySystemTrayFrame{nullptr};
static double g_lastPrimaryFrameSize = 0;
static winrt::event_token g_primarySizeChangedToken{};
static winrt::Windows::UI::Core::CoreDispatcher g_xamlDispatcher{nullptr};

struct TrayMetrics {
    double notifyIconStack = 0;
    double notificationAreaIcons = 0;
    double mainStack = 0;
    double nonActivatableStack = 0;
    double controlCenterButton = 0;
    double notificationCenterButton = 0;
    double showDesktopStack = 0;
    double frameWidth = 0;
    bool valid = false;
};

static TrayMetrics g_trayMetrics{};

// Per-secondary-controller state
struct SecondaryInfo {
    void* controller = nullptr;
    FrameworkElement frame{nullptr};
    bool treeDumped = false;
    bool frameSearchDone = false;
    bool populated = false;
    bool containersUncollapsed = false;
};

static std::vector<SecondaryInfo> g_secondaries;

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

static double GetChildWidthByName(FrameworkElement parent, PCWSTR name) {
    auto child = FindChildByName(parent, name);
    return child ? child.ActualWidth() : 0;
}

static void UpdateTrayMetricsFromFrame(FrameworkElement frame,
                                       const wchar_t* reason) {
    if (!frame) return;

    auto grid = FindChildByName(frame, L"SystemTrayFrameGrid");
    if (!grid) return;

    TrayMetrics metrics = {};
    metrics.notifyIconStack = GetChildWidthByName(grid, L"NotifyIconStack");
    metrics.notificationAreaIcons =
        GetChildWidthByName(grid, L"NotificationAreaIcons");
    metrics.mainStack = GetChildWidthByName(grid, L"MainStack");
    metrics.nonActivatableStack =
        GetChildWidthByName(grid, L"NonActivatableStack");
    metrics.controlCenterButton =
        GetChildWidthByName(grid, L"ControlCenterButton");
    metrics.notificationCenterButton =
        GetChildWidthByName(grid, L"NotificationCenterButton");
    metrics.showDesktopStack = GetChildWidthByName(grid, L"ShowDesktopStack");
    metrics.frameWidth = frame.ActualWidth();

    double measuredWidth =
        metrics.notifyIconStack +
        metrics.notificationAreaIcons +
        metrics.mainStack +
        metrics.nonActivatableStack +
        metrics.controlCenterButton +
        metrics.notificationCenterButton +
        metrics.showDesktopStack;

    metrics.valid = measuredWidth > 0 && metrics.frameWidth > 0;
    g_trayMetrics = metrics;

    Wh_Log(L"[TrayMetrics:%s] frame=%.0f sum=%.0f show=%.0f notif=%.0f "
           L"cc=%.0f nonAct=%.0f main=%.0f nai=%.0f overflow=%.0f valid=%d",
           reason, metrics.frameWidth, measuredWidth,
           metrics.showDesktopStack, metrics.notificationCenterButton,
           metrics.controlCenterButton, metrics.nonActivatableStack,
           metrics.mainStack, metrics.notificationAreaIcons,
           metrics.notifyIconStack, metrics.valid);
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

// ─── Flyout repositioning (proactive hook-based) ────────────────────────────
//
// ## Overview
// When a user clicks a tray icon on a secondary monitor, the system normally
// opens the corresponding flyout on the primary monitor. We intercept the
// Win32 APIs that control flyout placement so they open on the correct monitor.
//
// ## Architecture
// 1. A Win32 subclass on each Shell_SecondaryTrayWnd detects clicks and arms
//    a short-lived FlyoutContext with the target monitor, anchor point, and
//    flyout kind.
// 2. MonitorFrom* hooks redirect monitor queries to the target monitor so
//    the system's own layout code picks the right display.
// 3. SetWindowPos/MoveWindow/DeferWindowPos hooks intercept the final window
//    placement and call TranslateFlyoutRect to compute the correct position.
// 4. TrackPopupMenuEx hook handles Win32 popup menus (#32768 class) which
//    bypass SetWindowPos entirely — e.g. "Safely Remove Hardware".
//
// ## Flyout kinds and their positioning behavior
// - kControlCenter / kNotificationCenter: Full-height panels (Quick Settings,
//   Notification Center). The system sizes these to fill the work area height,
//   and ShellHost.exe already computes dimensions at the target monitor's DPI
//   thanks to our MonitorFrom* redirections. No DPI rescaling needed — only
//   X/Y translation to the target monitor.
// - kOverflowTray: Fixed-size popup (overflow tray chevron). The system
//   computes the window size using the primary monitor's DPI. When the target
//   monitor has a different DPI, dimensions must be rescaled.
// - kTrayPopup: Context menus and other small popups from tray icons. Same
//   rescaling behavior as kOverflowTray.
//
// ## Cross-DPI scaling
// When primary and secondary monitors have different DPI (e.g. 200% vs 150%),
// fixed-size flyouts (kOverflowTray, kTrayPopup) are computed at the primary
// DPI but displayed on the secondary. We rescale: newDim = MulDiv(dim,
// targetDpi, primaryDpi). Full-height panels (kControlCenter,
// kNotificationCenter) don't need this because the system sizes them to the
// work area after our MonitorFrom* hooks tell it the correct monitor.
//
// Note: We always look up the PRIMARY monitor's DPI as the source, not the
// requested rect's monitor, because our MonitorFrom* hooks may have already
// redirected the system to compute coordinates on the target monitor.

// Returns the DPI for a monitor, falling back to 96 on failure.
static UINT GetMonitorDpi(HMONITOR hMon) {
    UINT dpiX = 96, dpiY = 96;
    GetDpiForMonitor(hMon, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
    return dpiX;
}

static bool IsPrimaryMonitor(HMONITOR hMon) {
    MONITORINFO mi = {sizeof(mi)};
    return GetMonitorInfo(hMon, &mi) && (mi.dwFlags & MONITORINFOF_PRIMARY);
}

// ─── Flyout context ─────────────────────────────────────────────────────────

asm(".section .shared,\"dws\"\n");
#define SHARED_SECTION __attribute__((section(".shared")))

// Flyout kinds — determines positioning and DPI scaling behavior.
// Full-height panels (kControlCenter, kNotificationCenter) are sized by the
// system to fill the work area; no DPI rescaling needed, only X/Y translation.
// Fixed-size popups (kOverflowTray, kTrayPopup) need DPI rescaling when the
// target monitor has a different DPI than the primary.
constexpr int kFlyoutNone          = 0;  // No flyout
constexpr int kControlCenter       = 1;  // Quick Settings panel (full-height)
constexpr int kOverflowTray        = 2;  // Overflow tray popup (fixed-size, needs DPI rescale)
constexpr int kNotificationCenter  = 3;  // Notification Center panel (full-height)
constexpr int kTrayPopup           = 4;  // Context menus, tray icon popups (fixed-size, needs DPI rescale)

// Context durations (milliseconds)
constexpr DWORD kControlCenterContextMs  = 1800;
constexpr DWORD kOverflowTrayContextMs   = 800;
constexpr DWORD kAfterPlacementMs        = 650;

// Hit-test metrics (logical pixels at 96 DPI)
constexpr int kShowDesktopWidth        = 2;
constexpr int kNotificationCenterWidth = 78;
constexpr int kControlCenterWidth      = 117;
constexpr int kNotifyIconStackWidth    = 32;
constexpr int kHitSlop                 = 8;

struct FlyoutContext {
    HMONITOR targetMonitor;
    HWND     taskbarWnd;
    POINT    anchorPoint;
    RECT     taskbarRect;
    UINT     targetDpi;
    volatile DWORD expireTick;
    int      flyoutKind;
};

FlyoutContext g_flyoutCtx SHARED_SECTION = {};

// Suppression depth — incremented while our own hooks call Original functions
// to prevent re-entrant redirection.
static thread_local int g_flyoutRedirectionSuppressed = 0;

struct FlyoutRedirectionGuard {
    FlyoutRedirectionGuard() { g_flyoutRedirectionSuppressed++; }
    ~FlyoutRedirectionGuard() { g_flyoutRedirectionSuppressed--; }
};

static void ArmFlyoutContext(HWND taskbarWnd, int kind, POINT anchor) {
    // Clear first so MonitorFromRect below doesn't get redirected by stale context
    g_flyoutCtx.expireTick = 0;

    RECT tbRect = {};
    GetWindowRect(taskbarWnd, &tbRect);
    // Use suppression guard + plain API call. expireTick is already 0 so
    // our hook would fall through anyway, but the guard is extra safety.
    FlyoutRedirectionGuard guard;
    HMONITOR mon = MonitorFromRect(&tbRect, MONITOR_DEFAULTTONEAREST);

    DWORD durationMs = (kind == kControlCenter || kind == kNotificationCenter)
                           ? kControlCenterContextMs
                           : kOverflowTrayContextMs;

    g_flyoutCtx.targetMonitor = mon;
    g_flyoutCtx.taskbarWnd    = taskbarWnd;
    g_flyoutCtx.anchorPoint   = anchor;
    g_flyoutCtx.taskbarRect   = tbRect;
    g_flyoutCtx.targetDpi     = GetMonitorDpi(mon);
    g_flyoutCtx.flyoutKind    = kind;
    // Write expireTick last with a store barrier so readers see
    // all fields populated before expiry becomes valid.
    MemoryBarrier();
    g_flyoutCtx.expireTick    = GetTickCount() + durationMs;

    MONITORINFOEX armMi = {};
    armMi.cbSize = sizeof(armMi);
    GetMonitorInfo(mon, &armMi);
    APPBARDATA abd = {sizeof(abd)};
    UINT abState = (UINT)SHAppBarMessage(ABM_GETSTATE, &abd);
    Wh_Log(L"[FlyoutCtx] Armed kind=%d mon=%p \"%s\" dpi=%u anchor=(%d,%d) "
           L"duration=%u taskbar=(%d,%d,%d,%d) monWork=(%d,%d,%d,%d) "
           L"autoHide=%s",
           kind, mon, armMi.szDevice, g_flyoutCtx.targetDpi,
           anchor.x, anchor.y, durationMs,
           tbRect.left, tbRect.top, tbRect.right, tbRect.bottom,
           armMi.rcWork.left, armMi.rcWork.top,
           armMi.rcWork.right, armMi.rcWork.bottom,
           (abState & ABS_AUTOHIDE) ? L"YES" : L"NO");
}

static void ClearFlyoutContext() {
    g_flyoutCtx.expireTick = 0;
    g_flyoutCtx.flyoutKind = kFlyoutNone;
    g_flyoutCtx.targetMonitor = nullptr;
}

static FlyoutContext* GetActiveFlyoutContext() {
    DWORD expire = g_flyoutCtx.expireTick;
    if (!expire) return nullptr;
    if (GetTickCount() > expire) {
        Wh_Log(L"[FlyoutCtx] Context EXPIRED kind=%d (tick=%u expire=%u)",
               g_flyoutCtx.flyoutKind, GetTickCount(), expire);
        g_flyoutCtx.expireTick = 0;
        return nullptr;
    }
    if (!g_flyoutCtx.targetMonitor) return nullptr;
    return &g_flyoutCtx;
}

static void ShortenFlyoutContext(DWORD newDurationMs) {
    DWORD newExpire = GetTickCount() + newDurationMs;
    DWORD currentExpire = g_flyoutCtx.expireTick;
    if (currentExpire && newExpire < currentExpire) {
        g_flyoutCtx.expireTick = newExpire;
    }
}

// ─── Known flyout window identification ─────────────────────────────────────

static bool IsKnownFlyoutWindow(HWND hWnd, wchar_t* outClass = nullptr,
                                 int outClassSize = 0) {
    wchar_t className[128] = {};
    if (!GetClassName(hWnd, className, ARRAYSIZE(className)))
        return false;

    if (outClass && outClassSize > 0)
        wcsncpy_s(outClass, outClassSize, className, _TRUNCATE);

    if (wcscmp(className, L"TopLevelWindowForOverflowXamlIsland") == 0)
        return true;
    if (wcscmp(className, L"ControlCenterWindow") == 0)
        return true;
    // Note: Xaml_WindowedPopupClass is excluded — it matches tooltips and
    // small XAML popups that the system positions correctly. Real CC/overflow
    // flyouts use ControlCenterWindow or TopLevelWindowForOverflowXamlIsland.
    if (wcscmp(className, L"#32768") == 0)
        return true;
    if (wcscmp(className, L"XamlExplorerHostIslandWindow") == 0)
        return true;
    if (wcscmp(className, L"Microsoft.UI.Content.DesktopChildSiteBridge") == 0)
        return true;
    // Note: Windows.UI.Composition.DesktopWindowContentBridge is excluded —
    // it's the taskbar's own XAML bridge, not a flyout popup.
    return false;
}

// ─── TranslateFlyoutRect — core DPI-aware position translation ──────────────
//
// Translates a flyout window's requested rect from its system-computed
// position (typically on/near the primary monitor) to the correct position
// on the target secondary monitor.
//
// For fixed-size flyouts (overflow tray, tray popups), rescales the window
// dimensions from primary DPI to target DPI. For full-height panels (Control
// Center, Notification Center), preserves the system's dimensions since they
// already span the work area correctly.
//
// Positioning strategy: center horizontally on the anchor point (where the
// user clicked), pin the bottom edge flush against the taskbar top.

// Original function pointers (set during hook installation)
static decltype(&MonitorFromPoint)  MonitorFromPoint_Original  = nullptr;
static decltype(&MonitorFromRect)   MonitorFromRect_Original   = nullptr;
static decltype(&MonitorFromWindow) MonitorFromWindow_Original = nullptr;
static decltype(&SetWindowPos)      SetWindowPos_Original      = nullptr;
static decltype(&MoveWindow)        MoveWindow_Original        = nullptr;
static decltype(&DeferWindowPos)    DeferWindowPos_Original    = nullptr;
static decltype(&TrackPopupMenuEx)  TrackPopupMenuEx_Original  = nullptr;

static bool TranslateFlyoutRect(HWND hWnd, const RECT& requested,
                                 RECT* translated) {
    auto* ctx = GetActiveFlyoutContext();
    if (!ctx) return false;

    HMONITOR targetMon = ctx->targetMonitor;
    MONITORINFO targetMi = {sizeof(targetMi)};
    if (!GetMonitorInfo(targetMon, &targetMi)) return false;
    const RECT& targetWork = targetMi.rcWork;

    LONG width  = requested.right - requested.left;
    LONG height = requested.bottom - requested.top;
    if (width <= 0 || height <= 0) return false;

    UINT targetDpi = ctx->targetDpi;
    if (!targetDpi) targetDpi = GetMonitorDpi(targetMon);
    if (!targetDpi) targetDpi = 96;

    // The system computes flyout window size at the PRIMARY monitor's DPI,
    // regardless of which monitor the flyout will appear on (because the
    // XAML layout engine runs at the primary DPI). We need to find the
    // primary monitor's DPI to rescale correctly.
    // Note: we can't use the requested rect position to find the source
    // monitor because our MonitorFrom* hooks may have already redirected
    // the coordinates to the target monitor.
    UINT sourceDpi = 96;
    {
        FlyoutRedirectionGuard guard;
        POINT origin = {0, 0};
        HMONITOR primaryMon = MonitorFromPoint_Original
            ? MonitorFromPoint_Original(origin, MONITOR_DEFAULTTOPRIMARY)
            : MonitorFromPoint(origin, MONITOR_DEFAULTTOPRIMARY);
        sourceDpi = GetMonitorDpi(primaryMon);
        if (!sourceDpi) sourceDpi = 96;
    }

    // Rescale dimensions if source and target monitors have different DPI.
    // Only rescale for fixed-size flyouts (overflow tray, tray popups).
    // Control Center and Notification Center fill the work area height and
    // are already sized correctly by the system for the target monitor.
    bool shouldRescale = (sourceDpi != targetDpi) &&
        (ctx->flyoutKind == kOverflowTray || ctx->flyoutKind == kTrayPopup);
    if (shouldRescale) {
        width  = MulDiv(width, targetDpi, sourceDpi);
        height = MulDiv(height, targetDpi, sourceDpi);
    }

    // Get class name for kind-specific placement
    wchar_t className[128] = {};
    GetClassName(hWnd, className, ARRAYSIZE(className));

    Wh_Log(L"[Translate] class=%s kind=%d anchor=(%d,%d) "
           L"targetMon=%p srcDpi=%u tgtDpi=%u "
           L"taskbar=(%d,%d,%d,%d) targetWork=(%d,%d,%d,%d) "
           L"requested=(%d,%d,%d,%d) scaledSize=(%d,%d)",
           className, ctx->flyoutKind,
           ctx->anchorPoint.x, ctx->anchorPoint.y,
           targetMon, sourceDpi, targetDpi,
           ctx->taskbarRect.left, ctx->taskbarRect.top,
           ctx->taskbarRect.right, ctx->taskbarRect.bottom,
           targetWork.left, targetWork.top,
           targetWork.right, targetWork.bottom,
           requested.left, requested.top,
           requested.right, requested.bottom,
           width, height);

    // Position flyout: center horizontally on anchor, pin bottom to taskbar top.
    // Width and height have already been rescaled to the target monitor's DPI,
    // so this positions the correctly-sized flyout flush against the taskbar.
    LONG newX = ctx->anchorPoint.x - width / 2;
    LONG newY = ctx->taskbarRect.top - height;

    Wh_Log(L"[Translate] Anchor: newX=%d newY=%d tbTop=%d w=%d h=%d srcDpi=%u tgtDpi=%u",
           newX, newY, ctx->taskbarRect.top, width, height, sourceDpi, targetDpi);

    // Clamp into target work area
    if (newX + width > targetWork.right)
        newX = targetWork.right - width;
    if (newX < targetWork.left)
        newX = targetWork.left;
    if (newY + height > targetWork.bottom)
        newY = targetWork.bottom - height;
    if (newY < targetWork.top)
        newY = targetWork.top;

    translated->left   = newX;
    translated->top    = newY;
    translated->right  = newX + width;
    translated->bottom = newY + height;

    // Shorten context after successful placement
    ShortenFlyoutContext(kAfterPlacementMs);

    bool changed = (translated->left != requested.left ||
                    translated->top != requested.top ||
                    translated->right != requested.right ||
                    translated->bottom != requested.bottom);

    if (changed) {
        Wh_Log(L"[Translate] %s: (%d,%d,%d,%d) -> (%d,%d,%d,%d) kind=%d",
               className,
               requested.left, requested.top,
               requested.right, requested.bottom,
               translated->left, translated->top,
               translated->right, translated->bottom,
               ctx->flyoutKind);
    }

    return changed;
}

// ─── Cursor rescue fallback ─────────────────────────────────────────────────
// For cases where click detection missed (pen/touch, icon shifts), detect
// that a flyout is about to open on the wrong monitor and arm a context.

static bool TryCursorRescue(HWND hWnd, const RECT& requestedRect) {
    wchar_t className[128] = {};
    if (!GetClassName(hWnd, className, ARRAYSIZE(className)))
        return false;

    int kind = kFlyoutNone;
    if (wcscmp(className, L"TopLevelWindowForOverflowXamlIsland") == 0)
        kind = kOverflowTray;
    else if (wcscmp(className, L"ControlCenterWindow") == 0)
        kind = kControlCenter;
    else if (wcscmp(className, L"#32768") == 0 ||
             wcscmp(className, L"XamlExplorerHostIslandWindow") == 0 ||
             wcscmp(className, L"Microsoft.UI.Content.DesktopChildSiteBridge") == 0)
        kind = kTrayPopup;
    else
        return false;

    POINT cursorPos = {};
    if (!GetCursorPos(&cursorPos)) return false;

    // Find the taskbar window under the cursor
    FlyoutRedirectionGuard guard;
    HWND cursorWnd = WindowFromPoint(cursorPos);
    HWND rootWnd = cursorWnd ? GetAncestor(cursorWnd, GA_ROOT) : nullptr;
    if (!rootWnd) return false;

    // Must be a secondary taskbar
    wchar_t rootClass[128] = {};
    GetClassName(rootWnd, rootClass, ARRAYSIZE(rootClass));
    if (wcscmp(rootClass, L"Shell_SecondaryTrayWnd") != 0) {
        Wh_Log(L"[CursorRescue] Skip: root class=%s (not secondary taskbar) "
               L"cursor=(%d,%d) class=%s",
               rootClass, cursorPos.x, cursorPos.y, className);
        return false;
    }

    Wh_Log(L"[CursorRescue] Arming for class=%s kind=%d taskbar=%p "
           L"cursor=(%d,%d)",
           className, kind, rootWnd, cursorPos.x, cursorPos.y);

    ArmFlyoutContext(rootWnd, kind, cursorPos);
    return true;
}

// ─── MonitorFrom* hooks — redirect to target monitor ────────────────────────

static bool ShouldForceForPoint(POINT pt, HMONITOR targetMon) {
    if (!targetMon) return false;
    // Ambiguous origin queries
    if (pt.x == 0 && pt.y == 0) return true;
    // Only redirect if the point resolves to the target monitor already,
    // or to primary (where the system would normally place the flyout)
    FlyoutRedirectionGuard guard;
    HMONITOR actualMon = MonitorFromPoint_Original
        ? MonitorFromPoint_Original(pt, MONITOR_DEFAULTTONEAREST)
        : MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    if (!actualMon) return true;
    bool force = (actualMon == targetMon || IsPrimaryMonitor(actualMon));
    if (!force) {
        Wh_Log(L"[ShouldForcePoint] REJECTED pt=(%d,%d) actualMon=%p "
               L"targetMon=%p (not target or primary)",
               pt.x, pt.y, actualMon, targetMon);
    }
    return force;
}

static bool ShouldForceForRect(LPCRECT rect, HMONITOR targetMon) {
    if (!targetMon) return false;
    if (!rect || (rect->left == 0 && rect->top == 0 &&
                  rect->right == 0 && rect->bottom == 0))
        return true;
    FlyoutRedirectionGuard guard;
    HMONITOR actualMon = MonitorFromRect_Original
        ? MonitorFromRect_Original(rect, MONITOR_DEFAULTTONEAREST)
        : MonitorFromRect(rect, MONITOR_DEFAULTTONEAREST);
    if (!actualMon) return true;
    bool force = (actualMon == targetMon || IsPrimaryMonitor(actualMon));
    if (!force) {
        Wh_Log(L"[ShouldForceRect] REJECTED rect=(%d,%d,%d,%d) actualMon=%p "
               L"targetMon=%p (not target or primary)",
               rect->left, rect->top, rect->right, rect->bottom,
               actualMon, targetMon);
    }
    return force;
}

static HMONITOR WINAPI MonitorFromPoint_Hook(POINT pt, DWORD flags) {
    if (!g_flyoutRedirectionSuppressed && !g_unloading) {
        if (auto* ctx = GetActiveFlyoutContext()) {
            if (ShouldForceForPoint(pt, ctx->targetMonitor)) {
                Wh_Log(L"[MFP_Hook] Redirecting pt=(%d,%d) -> mon=%p kind=%d",
                       pt.x, pt.y, ctx->targetMonitor, ctx->flyoutKind);
                return ctx->targetMonitor;
            }
        }
    }
    return MonitorFromPoint_Original(pt, flags);
}

static HMONITOR WINAPI MonitorFromRect_Hook(LPCRECT rect, DWORD flags) {
    if (!g_flyoutRedirectionSuppressed && !g_unloading) {
        if (auto* ctx = GetActiveFlyoutContext()) {
            if (ShouldForceForRect(rect, ctx->targetMonitor)) {
                Wh_Log(L"[MFR_Hook] Redirecting rect=(%d,%d,%d,%d) -> mon=%p kind=%d",
                       rect ? rect->left : 0, rect ? rect->top : 0,
                       rect ? rect->right : 0, rect ? rect->bottom : 0,
                       ctx->targetMonitor, ctx->flyoutKind);
                return ctx->targetMonitor;
            }
        }
    }
    return MonitorFromRect_Original(rect, flags);
}

// Auto-arm: when a known flyout class queries its monitor and cursor is over
// a secondary taskbar, arm the context. This replaces click-based arming since
// XAML islands consume input before WM_LBUTTONDOWN reaches the Win32 parent.
static bool TryAutoArmFromCursor(HWND flyoutWnd, const wchar_t* className) {
    POINT cursorPos = {};
    if (!GetCursorPos(&cursorPos)) return false;

    FlyoutRedirectionGuard guard;
    HWND cursorWnd = WindowFromPoint(cursorPos);
    HWND rootWnd = cursorWnd ? GetAncestor(cursorWnd, GA_ROOT) : nullptr;
    if (!rootWnd) return false;

    wchar_t rootClass[128] = {};
    GetClassName(rootWnd, rootClass, ARRAYSIZE(rootClass));
    if (wcscmp(rootClass, L"Shell_SecondaryTrayWnd") != 0)
        return false;

    int kind = kFlyoutNone;
    if (wcscmp(className, L"TopLevelWindowForOverflowXamlIsland") == 0)
        kind = kOverflowTray;
    else if (wcscmp(className, L"ControlCenterWindow") == 0)
        kind = kControlCenter;
    // Note: Xaml_WindowedPopupClass is NOT auto-armed here — it matches both
    // real CC sub-popups and small tooltips. CC flow starts with
    // ControlCenterWindow which sets context; sub-popups inherit it.
    else if (wcscmp(className, L"#32768") == 0 ||
             wcscmp(className, L"XamlExplorerHostIslandWindow") == 0 ||
             wcscmp(className, L"Microsoft.UI.Content.DesktopChildSiteBridge") == 0)
        kind = kTrayPopup;
    else
        return false;

    Wh_Log(L"[AutoArm] Arming from cursor for class=%s kind=%d "
           L"taskbar=%p cursor=(%d,%d)",
           className, kind, rootWnd, cursorPos.x, cursorPos.y);

    ArmFlyoutContext(rootWnd, kind, cursorPos);
    return true;
}

static HMONITOR WINAPI MonitorFromWindow_Hook(HWND hWnd, DWORD flags) {
    if (!g_flyoutRedirectionSuppressed && !g_unloading) {
        // Check for active context first
        auto* ctx = GetActiveFlyoutContext();

        // If no context, try auto-arming from cursor position when we see
        // a known flyout class. This handles the case where XAML islands
        // consume clicks before they reach the Win32 subclass.
        if (!ctx && hWnd) {
            wchar_t className[128] = {};
            if (IsKnownFlyoutWindow(hWnd, className, ARRAYSIZE(className))) {
                if (TryAutoArmFromCursor(hWnd, className)) {
                    ctx = GetActiveFlyoutContext();
                }
            }
        }

        if (ctx) {
            wchar_t className[128] = {};
            bool isFlyoutPopup = false;
            if (hWnd) {
                isFlyoutPopup = IsKnownFlyoutWindow(hWnd, className, ARRAYSIZE(className));
            }
            bool isClickedTaskbar = (hWnd == ctx->taskbarWnd);

            if (isFlyoutPopup || isClickedTaskbar) {
                Wh_Log(L"[MFW_Hook] Redirecting class=%s hwnd=%p -> mon=%p "
                       L"kind=%d (flyout=%d taskbar=%d)",
                       className, hWnd, ctx->targetMonitor, ctx->flyoutKind,
                       isFlyoutPopup, isClickedTaskbar);
                // Shorten context when placement is clearly happening
                if (wcscmp(className, L"ControlCenterWindow") == 0 ||
                    wcscmp(className, L"TopLevelWindowForOverflowXamlIsland") == 0) {
                    ShortenFlyoutContext(kAfterPlacementMs);
                }
                return ctx->targetMonitor;
            } else if (className[0] != L'\0') {
                // Log unmatched top-level popup windows to spot missing flyout types
                LONG style = GetWindowLong(hWnd, GWL_STYLE);
                HWND parent = GetParent(hWnd);
                if (!parent && (style & WS_POPUP)) {
                    Wh_Log(L"[MFW_Hook] SKIPPED popup class=%s hwnd=%p "
                           L"style=0x%X kind=%d",
                           className, hWnd, style, ctx->flyoutKind);
                }
            }
        }
    }
    return MonitorFromWindow_Original(hWnd, flags);
}

// ─── Placement hooks — translate flyout coordinates ─────────────────────────

static bool ShouldTranslatePlacement(UINT uFlags) {
    // Skip pure z-order / activation changes
    if ((uFlags & SWP_NOMOVE) && (uFlags & SWP_NOSIZE))
        return false;
    return true;
}

static BOOL WINAPI SetWindowPos_Hook(HWND hWnd, HWND hWndInsertAfter,
                                      int X, int Y, int cx, int cy,
                                      UINT uFlags) {
    if (!g_unloading && !g_flyoutRedirectionSuppressed) {
        bool isFlyout = IsKnownFlyoutWindow(hWnd);

        if (isFlyout && ShouldTranslatePlacement(uFlags)) {
            bool hasContext = (GetActiveFlyoutContext() != nullptr);
            wchar_t cls[128] = {};
            GetClassName(hWnd, cls, ARRAYSIZE(cls));
            Wh_Log(L"[SWP_Hook] class=%s hwnd=%p pos=(%d,%d) size=(%d,%d) "
                   L"flags=0x%X hasCtx=%d",
                   cls, hWnd, X, Y, cx, cy, uFlags, hasContext);

            RECT currentRect = {};
            GetWindowRect(hWnd, &currentRect);

            LONG width = (uFlags & SWP_NOSIZE) ? currentRect.right - currentRect.left : cx;
            LONG height = (uFlags & SWP_NOSIZE) ? currentRect.bottom - currentRect.top : cy;
            RECT requested = {
                (uFlags & SWP_NOMOVE) ? currentRect.left : X,
                (uFlags & SWP_NOMOVE) ? currentRect.top : Y,
                ((uFlags & SWP_NOMOVE) ? currentRect.left : X) + width,
                ((uFlags & SWP_NOMOVE) ? currentRect.top : Y) + height,
            };

            // If no context, try auto-arm then cursor rescue
            if (!hasContext) {
                TryAutoArmFromCursor(hWnd, cls);
                hasContext = (GetActiveFlyoutContext() != nullptr);
            }
            if (!hasContext) {
                TryCursorRescue(hWnd, requested);
                hasContext = (GetActiveFlyoutContext() != nullptr);
            }

            if (hasContext) {
                RECT translated = {};
                if (TranslateFlyoutRect(hWnd, requested, &translated)) {
                    Wh_Log(L"[SWP_Hook] TRANSLATED (%d,%d,%d,%d) -> (%d,%d,%d,%d)",
                           requested.left, requested.top,
                           requested.right, requested.bottom,
                           translated.left, translated.top,
                           translated.right, translated.bottom);
                    uFlags &= ~(SWP_NOMOVE | SWP_NOSIZE);
                    X  = translated.left;
                    Y  = translated.top;
                    cx = translated.right - translated.left;
                    cy = translated.bottom - translated.top;
                }
            }
        }
    }
    return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

static BOOL WINAPI MoveWindow_Hook(HWND hWnd, int X, int Y,
                                    int nWidth, int nHeight, BOOL bRepaint) {
    if (!g_unloading && !g_flyoutRedirectionSuppressed) {
        bool isFlyout = IsKnownFlyoutWindow(hWnd);

        if (isFlyout) {
            bool hasContext = (GetActiveFlyoutContext() != nullptr);
            wchar_t cls[128] = {};
            GetClassName(hWnd, cls, ARRAYSIZE(cls));
            Wh_Log(L"[MW_Hook] class=%s hwnd=%p pos=(%d,%d) size=(%d,%d) "
                   L"hasCtx=%d",
                   cls, hWnd, X, Y, nWidth, nHeight, hasContext);

            RECT requested = {X, Y, X + nWidth, Y + nHeight};

            if (!hasContext) {
                TryAutoArmFromCursor(hWnd, cls);
                hasContext = (GetActiveFlyoutContext() != nullptr);
            }
            if (!hasContext) {
                TryCursorRescue(hWnd, requested);
                hasContext = (GetActiveFlyoutContext() != nullptr);
            }

            if (hasContext) {
                RECT translated = {};
                if (TranslateFlyoutRect(hWnd, requested, &translated)) {
                    Wh_Log(L"[MW_Hook] TRANSLATED (%d,%d,%d,%d) -> (%d,%d,%d,%d)",
                           requested.left, requested.top,
                           requested.right, requested.bottom,
                           translated.left, translated.top,
                           translated.right, translated.bottom);
                    X       = translated.left;
                    Y       = translated.top;
                    nWidth  = translated.right - translated.left;
                    nHeight = translated.bottom - translated.top;
                }
            }
        }
    }
    return MoveWindow_Original(hWnd, X, Y, nWidth, nHeight, bRepaint);
}

static HDWP WINAPI DeferWindowPos_Hook(HDWP hWinPosInfo, HWND hWnd,
                                        HWND hWndInsertAfter,
                                        int x, int y, int cx, int cy,
                                        UINT uFlags) {
    if (!g_unloading && !g_flyoutRedirectionSuppressed) {
        bool isFlyout = IsKnownFlyoutWindow(hWnd);

        if (isFlyout && ShouldTranslatePlacement(uFlags)) {
            bool hasContext = (GetActiveFlyoutContext() != nullptr);
            wchar_t cls[128] = {};
            GetClassName(hWnd, cls, ARRAYSIZE(cls));
            Wh_Log(L"[DWP_Hook] class=%s hwnd=%p pos=(%d,%d) size=(%d,%d) "
                   L"flags=0x%X hasCtx=%d",
                   cls, hWnd, x, y, cx, cy, uFlags, hasContext);

            RECT currentRect = {};
            GetWindowRect(hWnd, &currentRect);

            LONG width = (uFlags & SWP_NOSIZE) ? currentRect.right - currentRect.left : cx;
            LONG height = (uFlags & SWP_NOSIZE) ? currentRect.bottom - currentRect.top : cy;
            RECT requested = {
                (uFlags & SWP_NOMOVE) ? currentRect.left : x,
                (uFlags & SWP_NOMOVE) ? currentRect.top : y,
                ((uFlags & SWP_NOMOVE) ? currentRect.left : x) + width,
                ((uFlags & SWP_NOMOVE) ? currentRect.top : y) + height,
            };

            if (!hasContext) {
                TryAutoArmFromCursor(hWnd, cls);
                hasContext = (GetActiveFlyoutContext() != nullptr);
            }
            if (!hasContext) {
                TryCursorRescue(hWnd, requested);
                hasContext = (GetActiveFlyoutContext() != nullptr);
            }

            if (hasContext) {
                RECT translated = {};
                if (TranslateFlyoutRect(hWnd, requested, &translated)) {
                    Wh_Log(L"[DWP_Hook] TRANSLATED (%d,%d,%d,%d) -> (%d,%d,%d,%d)",
                           requested.left, requested.top,
                           requested.right, requested.bottom,
                           translated.left, translated.top,
                           translated.right, translated.bottom);
                    uFlags &= ~(SWP_NOMOVE | SWP_NOSIZE);
                    x  = translated.left;
                    y  = translated.top;
                    cx = translated.right - translated.left;
                    cy = translated.bottom - translated.top;
                }
            }
        }
    }
    return DeferWindowPos_Original(hWinPosInfo, hWnd, hWndInsertAfter,
                                    x, y, cx, cy, uFlags);
}

// ─── TrackPopupMenuEx hook — intercept Win32 popup menu positioning ──────────
// Win32 popup menus (window class #32768) — e.g. "Safely Remove Hardware" —
// are positioned internally by TrackPopupMenuEx. Unlike XAML flyouts, these
// never call SetWindowPos/MoveWindow, so our placement hooks don't see them.
// This hook translates the (x, y) coordinates to the anchor point on the
// target monitor. The menu auto-sizes itself, so no DPI rescaling is needed.

static BOOL WINAPI TrackPopupMenuEx_Hook(HMENU hMenu, UINT uFlags,
                                          int x, int y,
                                          HWND hWnd, LPTPMPARAMS lptpm) {
    if (!g_unloading && !g_flyoutRedirectionSuppressed) {
        auto* ctx = GetActiveFlyoutContext();

        // If no context, check if cursor is over a secondary taskbar
        if (!ctx) {
            POINT cursorPos = {};
            if (GetCursorPos(&cursorPos)) {
                FlyoutRedirectionGuard guard;
                HWND cursorWnd = WindowFromPoint(cursorPos);
                HWND rootWnd = cursorWnd ? GetAncestor(cursorWnd, GA_ROOT) : nullptr;
                if (rootWnd) {
                    wchar_t rootClass[128] = {};
                    GetClassName(rootWnd, rootClass, ARRAYSIZE(rootClass));
                    if (wcscmp(rootClass, L"Shell_SecondaryTrayWnd") == 0) {
                        Wh_Log(L"[TPM_Hook] Auto-arming from cursor at (%d,%d) "
                               L"taskbar=%p", cursorPos.x, cursorPos.y, rootWnd);
                        ArmFlyoutContext(rootWnd, kTrayPopup, cursorPos);
                        ctx = GetActiveFlyoutContext();
                    }
                }
            }
        }

        if (ctx) {
            // Translate popup menu to the anchor point on the target monitor.
            // Keep the system's original alignment flags and Y coordinate
            // strategy — just shift X to the anchor and Y to the same
            // offset from the taskbar on the target monitor.
            int newX = ctx->anchorPoint.x;
            int newY = y;

            // The system's Y is relative to the primary tray position.
            // Translate: compute offset from primary taskbar top, apply to target.
            // Primary taskbar top == targetWork.bottom for same-DPI monitors.
            // Original y was in absolute coords near the primary taskbar.
            // Use anchor Y (cursor position near taskbar) for a reliable Y.
            newY = ctx->anchorPoint.y;

            Wh_Log(L"[TPM_Hook] Translating popup menu: (%d,%d) -> (%d,%d) "
                   L"flags=0x%X anchor=(%d,%d) kind=%d",
                   x, y, newX, newY, uFlags,
                   ctx->anchorPoint.x, ctx->anchorPoint.y,
                   ctx->flyoutKind);

            ShortenFlyoutContext(kAfterPlacementMs);
            return TrackPopupMenuEx_Original(hMenu, uFlags, newX, newY,
                                              hWnd, lptpm);
        }
    }
    return TrackPopupMenuEx_Original(hMenu, uFlags, x, y, hWnd, lptpm);
}

// ─── Hit-test for secondary taskbar tray clicks ─────────────────────────────

static int PhysicalPixelsToDips(UINT dpi, int pixels) {
    return MulDiv(pixels, 96, dpi ? dpi : 96);
}

static int DipsToPhysicalPixels(UINT dpi, double dips) {
    return MulDiv((int)(dips + 0.5), dpi ? dpi : 96, 96);
}

static UINT GetWindowDpi(HWND hWnd) {
    // GetDpiForWindow requires Windows 10 1607+
    UINT dpi = GetDpiForWindow(hWnd);
    return dpi ? dpi : 96;
}

static int HitTestTrayClick(HWND hWnd, LPARAM lParam) {
    RECT clientRect = {};
    if (!GetClientRect(hWnd, &clientRect)) return kFlyoutNone;

    int width  = clientRect.right - clientRect.left;
    int height = clientRect.bottom - clientRect.top;

    // Only handle horizontal taskbars
    if (width <= height) return kFlyoutNone;

    int x = GET_X_LPARAM(lParam);
    UINT dpi = GetWindowDpi(hWnd);
    int xFromRight = clientRect.right - x;
    int xFromRightDip = PhysicalPixelsToDips(dpi, xFromRight);

    int showDesktopEnd = 0;
    int notifCenterEnd = 0;
    int controlCenterEnd = 0;
    int trayIconsStart = 0;
    int trayIconsEnd = 0;
    int overflowStart = 0;
    int overflowEnd = 0;

    if (g_trayMetrics.valid) {
        showDesktopEnd = (int)(g_trayMetrics.showDesktopStack + 0.5);
        notifCenterEnd = showDesktopEnd +
            (int)(g_trayMetrics.notificationCenterButton + 0.5);
        controlCenterEnd = notifCenterEnd +
            (int)(g_trayMetrics.controlCenterButton + 0.5);
        trayIconsStart = controlCenterEnd +
            (int)(g_trayMetrics.nonActivatableStack + 0.5) +
            (int)(g_trayMetrics.mainStack + 0.5);
        trayIconsEnd = trayIconsStart +
            (int)(g_trayMetrics.notificationAreaIcons + 0.5);
        overflowStart = trayIconsEnd;
        overflowEnd = overflowStart +
            (int)(g_trayMetrics.notifyIconStack + 0.5);
    } else {
        showDesktopEnd = kShowDesktopWidth;
        notifCenterEnd = showDesktopEnd + kNotificationCenterWidth;
        controlCenterEnd = notifCenterEnd + kControlCenterWidth;
        trayIconsStart = controlCenterEnd;
        trayIconsEnd = trayIconsStart + 100;
        overflowStart = trayIconsEnd;
        overflowEnd = overflowStart + kNotifyIconStackWidth;
    }

    int slop = kHitSlop;

    // Overflow tray chevron
    if (xFromRightDip >= overflowStart - slop &&
        xFromRightDip < overflowEnd + slop) {
        Wh_Log(L"[HitTest] -> kOverflowTray");
        return kOverflowTray;
    }

    // Promoted tray icons
    if (xFromRightDip >= trayIconsStart - slop &&
        xFromRightDip < trayIconsEnd + slop) {
        return kTrayPopup;
    }

    // Control center (volume/wifi/battery)
    if (xFromRightDip >= notifCenterEnd - slop &&
        xFromRightDip < controlCenterEnd + slop) {
        Wh_Log(L"[HitTest] -> kControlCenter");
        return kControlCenter;
    }

    // Notification center (clock/date)
    if (xFromRightDip >= showDesktopEnd - slop &&
        xFromRightDip < notifCenterEnd + slop) {
        Wh_Log(L"[HitTest] -> kNotificationCenter");
        return kNotificationCenter;
    }

    Wh_Log(L"[HitTest] -> kFlyoutNone (no zone matched)");
    return kFlyoutNone;
}

static bool GetFlyoutAnchorPoint(HWND hWnd, LPARAM lParam, int flyoutKind,
                                  POINT* anchor) {
    RECT clientRect = {};
    if (!GetClientRect(hWnd, &clientRect)) return false;

    LONG clientX = GET_X_LPARAM(lParam);
    LONG origClientX = clientX;
    UINT dpi = GetWindowDpi(hWnd);

    if (flyoutKind == kOverflowTray) {
        // Compute chevron midpoint from metrics
        double overflowStart = 0;
        double overflowW = kNotifyIconStackWidth;
        if (g_trayMetrics.valid) {
            overflowStart =
                g_trayMetrics.showDesktopStack +
                g_trayMetrics.notificationCenterButton +
                g_trayMetrics.controlCenterButton +
                g_trayMetrics.nonActivatableStack +
                g_trayMetrics.mainStack +
                g_trayMetrics.notificationAreaIcons;
            overflowW = g_trayMetrics.notifyIconStack;
        } else {
            overflowStart =
                kShowDesktopWidth +
                kNotificationCenterWidth +
                kControlCenterWidth;
        }
        int overflowMidPx =
            DipsToPhysicalPixels(dpi, overflowStart + overflowW / 2);
        clientX = clientRect.right - overflowMidPx;
        if (clientX < clientRect.left) clientX = clientRect.left;
        if (clientX >= clientRect.right) clientX = clientRect.right - 1;
        Wh_Log(L"[Anchor] Overflow chevron: origX=%d computedX=%d "
               L"overflowStart=%.0f overflowW=%.0f midPx=%d "
               L"clientRight=%d",
               origClientX, clientX, overflowStart, overflowW,
               overflowMidPx, clientRect.right);
    }

    POINT pt = {clientX, (clientRect.top + clientRect.bottom) / 2};
    if (!ClientToScreen(hWnd, &pt)) return false;

    Wh_Log(L"[Anchor] kind=%d client=(%d,%d) screen=(%d,%d)",
           flyoutKind, clientX, (clientRect.top + clientRect.bottom) / 2,
           pt.x, pt.y);

    *anchor = pt;
    return true;
}

// ─── Taskbar subclass ───────────────────────────────────────────────────────

static constexpr UINT_PTR kTaskbarSubclassId = 0x54524159; // 'TRAY'

// Track subclassed windows for cleanup
static std::vector<HWND> g_subclassedTaskbars;

static LRESULT CALLBACK TaskbarSubclassProc(HWND hWnd, UINT uMsg,
                                             WPARAM wParam, LPARAM lParam,
                                             UINT_PTR uIdSubclass,
                                             DWORD_PTR dwRefData) {
    if (g_unloading) {
        if (uMsg == WM_NCDESTROY) {
            RemoveWindowSubclass(hWnd, TaskbarSubclassProc, kTaskbarSubclassId);
        }
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    bool isDown = false;
    bool isRight = false;

    if (uMsg == WM_LBUTTONDOWN) {
        isDown = true;
    } else if (uMsg == WM_RBUTTONDOWN) {
        isDown = true;
        isRight = true;
    } else if (uMsg == WM_PARENTNOTIFY) {
        WORD event = LOWORD(wParam);
        if (event == WM_LBUTTONDOWN) {
            isDown = true;
        } else if (event == WM_RBUTTONDOWN) {
            isDown = true;
            isRight = true;
        }
    }

    if (isDown) {
        int kind = HitTestTrayClick(hWnd, lParam);
        if (kind != kFlyoutNone) {
            if (isRight && kind != kOverflowTray) {
                kind = kTrayPopup;
            }
            POINT anchor = {};
            GetFlyoutAnchorPoint(hWnd, lParam, kind, &anchor);
            ArmFlyoutContext(hWnd, kind, anchor);
        } else {
            ClearFlyoutContext();
        }
    }

    if (uMsg == WM_NCDESTROY) {
        RemoveWindowSubclass(hWnd, TaskbarSubclassProc, kTaskbarSubclassId);
        auto it = std::find(g_subclassedTaskbars.begin(),
                            g_subclassedTaskbars.end(), hWnd);
        if (it != g_subclassedTaskbars.end())
            g_subclassedTaskbars.erase(it);
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

static void InstallTaskbarSubclass(HWND hWnd) {
    if (SetWindowSubclass(hWnd, TaskbarSubclassProc, kTaskbarSubclassId, 0)) {
        g_subclassedTaskbars.push_back(hWnd);
        RECT winRect = {};
        GetWindowRect(hWnd, &winRect);
        RECT clientRect = {};
        GetClientRect(hWnd, &clientRect);
        UINT dpi = GetWindowDpi(hWnd);
        HMONITOR hMon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
        Wh_Log(L"[Subclass] Installed on hwnd=%p mon=%p dpi=%u "
               L"winRect=(%d,%d,%d,%d) clientSize=(%dx%d)",
               hWnd, hMon, dpi,
               winRect.left, winRect.top, winRect.right, winRect.bottom,
               clientRect.right - clientRect.left,
               clientRect.bottom - clientRect.top);
    } else {
        Wh_Log(L"[Subclass] Failed for hwnd=%p error=%d",
               hWnd, GetLastError());
    }
}

static void RemoveAllTaskbarSubclasses() {
    for (HWND hwnd : g_subclassedTaskbars) {
        if (IsWindow(hwnd)) {
            RemoveWindowSubclass(hwnd, TaskbarSubclassProc, kTaskbarSubclassId);
        }
    }
    g_subclassedTaskbars.clear();
}

// Install subclasses on all Shell_SecondaryTrayWnd windows
static void InstallFlyoutHooks() {
    HWND tbWnd = nullptr;
    while ((tbWnd = FindWindowEx(nullptr, tbWnd,
                                  L"Shell_SecondaryTrayWnd",
                                  nullptr)) != nullptr) {
        // Check if already subclassed
        bool alreadySubclassed = false;
        for (HWND existing : g_subclassedTaskbars) {
            if (existing == tbWnd) {
                alreadySubclassed = true;
                break;
            }
        }
        if (!alreadySubclassed) {
            InstallTaskbarSubclass(tbWnd);
        }
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

// Check if a pointer range is readable using VirtualQuery
static bool IsReadableMemory(const void* ptr, size_t size) {
    MEMORY_BASIC_INFORMATION mbi;
    if (!VirtualQuery(ptr, &mbi, sizeof(mbi))) return false;
    if (mbi.State != MEM_COMMIT) return false;
    if (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD)) return false;
    // Ensure the entire range fits within this region
    uintptr_t regionEnd = (uintptr_t)mbi.BaseAddress + mbi.RegionSize;
    uintptr_t rangeEnd = (uintptr_t)ptr + size;
    return rangeEnd <= regionEnd;
}

FrameworkElement TryExtractFrameFromController(void* controller,
                                                const wchar_t* label) {
    uintptr_t* slots = (uintptr_t*)controller;

    for (int i = 2; i < 48; i++) {
        // Check that the slot itself is readable
        if (!IsReadableMemory(&slots[i], sizeof(uintptr_t))) break;

        uintptr_t value = slots[i];

        // Basic pointer sanity checks
        if (value < 0x10000) continue;
        if ((value & 0x7) != 0) continue;  // must be 8-byte aligned

        // Read vtable pointer safely
        if (!IsReadableMemory((void*)value, sizeof(uintptr_t))) continue;
        uintptr_t vtablePtr = *(uintptr_t*)value;

        if (vtablePtr < 0x10000) continue;

        // Check first vtable entry points to executable code
        if (!IsReadableMemory((void*)vtablePtr, sizeof(uintptr_t))) continue;
        uintptr_t qiAddr = *(uintptr_t*)vtablePtr;

        MEMORY_BASIC_INFORMATION mbi;
        if (!VirtualQuery((void*)qiAddr, &mbi, sizeof(mbi))) continue;
        if (!(mbi.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ |
                             PAGE_EXECUTE_READWRITE |
                             PAGE_EXECUTE_WRITECOPY)))
            continue;

        // Try QI for FrameworkElement
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

// Find or create a SecondaryInfo for a given controller
static SecondaryInfo* FindOrCreateSecondary(void* controller) {
    for (auto& si : g_secondaries) {
        if (si.controller == controller) return &si;
    }
    g_secondaries.push_back({controller});
    return &g_secondaries.back();
}

// Find the SecondaryInfo that owns a given frame
static SecondaryInfo* FindSecondaryByFrame(FrameworkElement frame) {
    for (auto& si : g_secondaries) {
        if (si.frame == frame) return &si;
    }
    return nullptr;
}

void HandleExtractedFrame(FrameworkElement frame, bool isSecondary,
                          void* controller = nullptr) {
    const wchar_t* label = isSecondary ? L"SECONDARY" : L"PRIMARY";

    // Check if the XAML tree is actually built yet (has SystemTrayFrameGrid)
    auto grid = FindChildByName(frame, L"SystemTrayFrameGrid");
    if (!grid) {
        Wh_Log(L"[%s] Frame found via controller scan but XAML tree not "
               L"ready yet (no SystemTrayFrameGrid) — deferring to IconView",
               label);
        if (isSecondary && controller) {
            auto* si = FindOrCreateSecondary(controller);
            si->frame = frame;
        } else if (!isSecondary) {
            g_primarySystemTrayFrame = frame;
        }
        return;
    }

    if (isSecondary) {
        SecondaryInfo* si = controller ? FindOrCreateSecondary(controller)
                                       : FindSecondaryByFrame(frame);
        if (!si) {
            // No controller association yet — create one with nullptr controller
            // (will be associated later via UpdateFrameSize)
            g_secondaries.push_back({nullptr, frame});
            si = &g_secondaries.back();
        }
        si->frame = frame;
        if (!si->treeDumped) {
            si->treeDumped = true;
            Wh_Log(L"=== SECONDARY TASKBAR XAML TREE (via controller scan) [%zu] ===",
                   g_secondaries.size());
            DumpXamlTree(frame, 0, 8);
            ProbeContainerInterfaces(frame, label);
        }
    } else {
        g_primarySystemTrayFrame = frame;
        g_xamlDispatcher = frame.Dispatcher();
        UpdateTrayMetricsFromFrame(frame, L"primary-extracted");
        if (!g_primaryTreeDumped) {
            g_primaryTreeDumped = true;
            Wh_Log(L"=== PRIMARY TASKBAR XAML TREE (via controller scan) ===");
            DumpXamlTree(frame, 0, 8);
            ProbeContainerInterfaces(frame, label);
        }
    }

    if (g_primarySystemTrayFrame) {
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

static void SyncTrayState(SecondaryInfo& si) {
    auto primaryGrid = FindChildByName(g_primarySystemTrayFrame, L"SystemTrayFrameGrid");
    auto secondaryGrid = FindChildByName(si.frame, L"SystemTrayFrameGrid");
    if (!primaryGrid || !secondaryGrid) return;

    for (auto containerName : g_syncedContainers) {
        auto primaryContainer = FindChildByName(primaryGrid, containerName);
        auto secondaryContainer = FindChildByName(secondaryGrid, containerName);
        if (!primaryContainer || !secondaryContainer) continue;

        auto primaryVis = primaryContainer.Visibility();
        auto secondaryVis = secondaryContainer.Visibility();
        if (primaryVis == Visibility::Visible && secondaryVis == Visibility::Collapsed) {
            secondaryContainer.Visibility(Visibility::Visible);
            si.containersUncollapsed = true;
        } else if (primaryVis == Visibility::Collapsed && secondaryVis == Visibility::Visible) {
            secondaryContainer.Visibility(Visibility::Collapsed);
            si.containersUncollapsed = true;
        }
    }

    auto primaryNAI = FindChildByName(primaryGrid, L"NotificationAreaIcons");
    auto secondaryNAI = FindChildByName(secondaryGrid, L"NotificationAreaIcons");
    if (primaryNAI && secondaryNAI) {
        auto primaryIC = primaryNAI.try_as<Controls::ItemsControl>();
        auto secondaryIC = secondaryNAI.try_as<Controls::ItemsControl>();
        if (primaryIC && secondaryIC) {
            auto pSource = primaryIC.ItemsSource();
            if (pSource && secondaryIC.ItemsSource() != pSource) {
                try { secondaryIC.ItemsSource(pSource); } catch (...) {}
            }
        }
    }

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
        if (pIC && sIC) {
            auto pSource = pIC.ItemsSource();
            if (pSource && sIC.ItemsSource() != pSource) {
                try { sIC.ItemsSource(pSource); } catch (...) {}
            }
        }
    }

    for (auto stackName : g_stackContainers) {
        auto primaryStack = FindChildByName(primaryGrid, stackName);
        auto secondaryStack = FindChildByName(secondaryGrid, stackName);
        if (!primaryStack || !secondaryStack) continue;
        auto primaryContent = FindChildByName(primaryStack, L"Content");
        auto secondaryContent = FindChildByName(secondaryStack, L"Content");
        if (!primaryContent || !secondaryContent) continue;
        auto primaryIconStack = FindChildByName(primaryContent, L"IconStack");
        auto secondaryIconStack = FindChildByName(secondaryContent, L"IconStack");
        if (!primaryIconStack || !secondaryIconStack) continue;

        auto pIC = primaryIconStack.try_as<Controls::ItemsControl>();
        auto sIC = secondaryIconStack.try_as<Controls::ItemsControl>();
        if (pIC && sIC) {
            auto pSource = pIC.ItemsSource();
            if (pSource && sIC.ItemsSource() != pSource) {
                try { sIC.ItemsSource(pSource); } catch (...) {}
            }
        }
    }
}

void TryPopulateOneSecondary(SecondaryInfo& si);

void TryPopulateSecondaryTray() {
    if (!g_primarySystemTrayFrame) return;

    bool anyPopulated = false;
    for (auto& si : g_secondaries) {
        if (si.populated || !si.frame) continue;

        try {
            TryPopulateOneSecondary(si);
            if (si.populated) anyPopulated = true;
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Population failed (hresult): 0x%08X %s",
                   (unsigned)ex.code(), ex.message().c_str());
        } catch (...) {
            Wh_Log(L"Population failed (unknown exception)");
        }
    }

    // Register SizeChanged on primary (once) if any secondary got populated
    if (anyPopulated && !g_primarySizeChangedToken.value) {
        g_primarySizeChangedToken = g_primarySystemTrayFrame.SizeChanged(
            [](winrt::Windows::Foundation::IInspectable const& sender,
               SizeChangedEventArgs const& args) {
                if (g_unloading) return;

                try {
                    double newWidth = args.NewSize().Width;
                    double oldWidth = args.PreviousSize().Width;
                    if (newWidth == oldWidth) return;

                    Wh_Log(L"[PrimarySizeChanged] %.0f -> %.0f",
                           oldWidth, newWidth);
                    g_lastPrimaryFrameSize = newWidth;
                    UpdateTrayMetricsFromFrame(g_primarySystemTrayFrame,
                                               L"primary-size");

                    for (auto& si : g_secondaries) {
                        if (!si.populated || !si.frame) continue;
                        SyncTrayState(si);
                        try {
                            double secWidth = si.frame.Width();
                            if (secWidth != newWidth) {
                                si.frame.Width(newWidth);
                                si.frame.InvalidateMeasure();
                                si.frame.InvalidateArrange();
                                auto parent = Media::VisualTreeHelper::GetParent(
                                    si.frame).try_as<FrameworkElement>();
                                while (parent) {
                                    parent.InvalidateMeasure();
                                    parent.InvalidateArrange();
                                    parent = Media::VisualTreeHelper::GetParent(
                                        parent).try_as<FrameworkElement>();
                                }
                            }
                        } catch (...) {}
                    }
                } catch (...) {
                    Wh_Log(L"[PrimarySizeChanged] Exception during sync");
                }
            });
        Wh_Log(L"[SizeChanged] Registered on primary frame");
    }

    // Install taskbar subclasses for flyout redirection
    if (anyPopulated) {
        InstallFlyoutHooks();
    }
}

void TryPopulateOneSecondary(SecondaryInfo& si) {
    Wh_Log(L"=== ATTEMPTING TO POPULATE SECONDARY TRAY (controller=%p) ===",
           si.controller);

    auto primaryGrid =
        FindChildByName(g_primarySystemTrayFrame, L"SystemTrayFrameGrid");
    auto secondaryGrid =
        FindChildByName(si.frame, L"SystemTrayFrameGrid");
    if (!primaryGrid || !secondaryGrid) {
        Wh_Log(L"Missing grid(s), aborting (will retry)");
        return;  // si.populated stays false — will retry
    }

    si.populated = true;

    double primaryFrameWidth = g_primarySystemTrayFrame.ActualWidth();
    double secondaryFrameWidth = si.frame.ActualWidth();
    Wh_Log(L"Frame widths - primary:%.0f secondary:%.0f",
           primaryFrameWidth, secondaryFrameWidth);

    SyncTrayState(si);

    // Step 4: Set the secondary frame width to match primary
    if (primaryFrameWidth > secondaryFrameWidth) {
        Wh_Log(L"Setting secondary frame Width: %.0f -> %.0f",
               secondaryFrameWidth, primaryFrameWidth);
        si.frame.Width(primaryFrameWidth);
    }

    // Force the secondary controller to recalculate frame size
    if (si.controller &&
        SystemTraySecondaryController_UpdateFrameSize_Original) {
        Wh_Log(L"Forcing UpdateFrameSize on secondary controller %p",
               si.controller);
        SystemTraySecondaryController_UpdateFrameSize_Original(si.controller);
    }

    // Force layout update on secondary frame
    si.frame.InvalidateMeasure();
    si.frame.InvalidateArrange();
    si.frame.UpdateLayout();

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
           si.frame.ActualWidth(), si.frame.ActualHeight());

    Wh_Log(L"=== POPULATION ATTEMPT COMPLETE ===");

    // Schedule a deferred refresh to re-sync width after layout settles.
    // During monitor hot-plug or DPI changes, the primary frame's ActualWidth
    // may not reflect the final value yet at population time.
    RefreshSecondaryTray();
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
        // Find existing SecondaryInfo with this frame, or find one without
        // a frame yet, or create a new one
        SecondaryInfo* si = FindSecondaryByFrame(systemTrayFrame);
        if (!si) {
            // Try to find a secondary entry that has no frame yet
            for (auto& s : g_secondaries) {
                if (!s.frame) { si = &s; break; }
            }
            if (!si) {
                g_secondaries.push_back({nullptr, systemTrayFrame});
                si = &g_secondaries.back();
            } else {
                si->frame = systemTrayFrame;
            }
        }
        if (!si->treeDumped) {
            si->treeDumped = true;
            Wh_Log(L"=== SECONDARY TASKBAR XAML TREE [%zu] ===",
                   g_secondaries.size());
            DumpXamlTree(systemTrayFrame, 0, 8);
            ProbeContainerInterfaces(systemTrayFrame, L"SECONDARY");
        }
    } else {
        if (!g_primaryTreeDumped) {
            g_primaryTreeDumped = true;
            g_primarySystemTrayFrame = systemTrayFrame;
            g_xamlDispatcher = systemTrayFrame.Dispatcher();
            UpdateTrayMetricsFromFrame(systemTrayFrame, L"primary-loaded");
            Wh_Log(L"=== PRIMARY TASKBAR XAML TREE ===");
            DumpXamlTree(systemTrayFrame, 0, 8);
            ProbeContainerInterfaces(systemTrayFrame, L"PRIMARY");
        }
    }

    if (g_primarySystemTrayFrame) {
        TryPopulateSecondaryTray();
    }
}

// ─── SystemTrayController hooks ─────────────────────────────────────────────

// Reset all XAML state — called when a new controller is created (explorer restart)
static void ResetXamlState() {
    Wh_Log(L"Resetting XAML state (new controller detected)");

    // Unregister SizeChanged before releasing the reference
    if (g_primarySystemTrayFrame && g_primarySizeChangedToken.value) {
        try {
            g_primarySystemTrayFrame.SizeChanged(g_primarySizeChangedToken);
        } catch (...) {}
        g_primarySizeChangedToken = {};
    }

    RemoveAllTaskbarSubclasses();
    ClearFlyoutContext();
    g_autoRevokerList.clear();

    g_primarySystemTrayFrame = nullptr;
    g_xamlDispatcher = nullptr;
    g_trayMetrics = {};
    g_primaryTreeDumped = false;
    g_primaryFrameSearchDone = false;
    g_lastPrimaryFrameSize = 0;
    g_primaryController = nullptr;

    // Clear all secondary state
    for (auto& si : g_secondaries) {
        si.frame = nullptr;
    }
    g_secondaries.clear();
}

// Constructor: captures primary controller this pointer
using SystemTrayController_ctor_t = void*(WINAPI*)(void* pThis, void* taskbarModel);
SystemTrayController_ctor_t SystemTrayController_ctor_Original;
void* WINAPI SystemTrayController_ctor_Hook(void* pThis, void* taskbarModel) {
    Wh_Log(L">>> SystemTrayController::ctor this=%p (prev=%p)", pThis,
           g_primaryController);

    // Always reset if we already had a controller — explorer may reuse the
    // same heap address after restart, so pointer comparison is unreliable
    if (g_primaryController) {
        ResetXamlState();
    }

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

    FindOrCreateSecondary(pThis);

    void* ret = SystemTraySecondaryController_ctor_Original(pThis, taskbarModel);
    Wh_Log(L">>> SystemTraySecondaryController::ctor done, ret=%p", ret);
    return ret;
}

// UpdateFrameSize
using SystemTraySecondaryController_UpdateFrameSize_t = void(WINAPI*)(void* pThis);
SystemTraySecondaryController_UpdateFrameSize_t SystemTraySecondaryController_UpdateFrameSize_Original;
void WINAPI SystemTraySecondaryController_UpdateFrameSize_Hook(void* pThis) {
    // Ensure this controller is tracked
    auto* si = FindOrCreateSecondary(pThis);

    SystemTraySecondaryController_UpdateFrameSize_Original(pThis);

    // Fallback: extract frame from controller if not yet captured
    if (!si->frame && !si->frameSearchDone && !g_unloading) {
        si->frameSearchDone = true;
        auto frame = TryExtractFrameFromController(pThis, L"SECONDARY");
        if (frame) {
            HandleExtractedFrame(frame, true, pThis);
        }
    }

    // Schedule a deferred refresh rather than immediately re-applying width.
    // During DPI changes, the primary frame hasn't re-laid-out yet when this
    // hook fires, so ActualWidth() returns the old-DPI value. A deferred
    // refresh lets layout settle before reading the correct width.
    if (si->populated && si->frame &&
        g_primarySystemTrayFrame && !g_unloading) {
        RefreshSecondaryTray();
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

    // Check if this controller's secondary has been populated
    for (auto& si : g_secondaries) {
        if (si.controller == pThis && si.populated) {
            // Read live primary width to avoid returning stale DPI values
            double primaryWidth = g_lastPrimaryFrameSize;
            if (g_primarySystemTrayFrame) {
                try {
                    double live = g_primarySystemTrayFrame.ActualWidth();
                    if (live > 0) primaryWidth = live;
                } catch (...) {}
            }
            if (primaryWidth > originalSize) {
                return primaryWidth;
            }
        }
    }

    return originalSize;
}

// ─── IconView::IconView hook ────────────────────────────────────────────────

using IconView_IconView_t = void*(WINAPI*)(void* pThis);
IconView_IconView_t IconView_IconView_Original;
void* WINAPI IconView_IconView_Hook(void* pThis) {
    void* ret = IconView_IconView_Original(pThis);

    if (g_unloading) return ret;

    try {
        // Get FrameworkElement from IconView WinRT object
        FrameworkElement iconView = nullptr;
        ((IUnknown**)pThis)[1]->QueryInterface(
            winrt::guid_of<FrameworkElement>(), winrt::put_abi(iconView));
        if (!iconView) {
            return ret;
        }

        Wh_Log(L">>> IconView created: %p", pThis);

        // Hook the Loaded event to explore the tree once in the visual tree
        g_autoRevokerList.emplace_back();
        auto autoRevokerIt = g_autoRevokerList.end();
        --autoRevokerIt;

        *autoRevokerIt = iconView.Loaded(
            winrt::auto_revoke_t{},
            [autoRevokerIt](
                winrt::Windows::Foundation::IInspectable const& sender,
                RoutedEventArgs const& e) {
                g_autoRevokerList.erase(autoRevokerIt);

                if (g_unloading) return;

                try {
                    auto iconView = sender.try_as<FrameworkElement>();
                    if (!iconView) return;

                    HandleLoadedIconView(iconView);
                } catch (...) {
                    Wh_Log(L"Exception in IconView Loaded handler");
                }
            });
    } catch (...) {
        Wh_Log(L"Exception in IconView_IconView_Hook");
    }

    return ret;
}

// ─── module hooking ─────────────────────────────────────────────────────────

bool HookSystemTraySymbols(HMODULE module) {
    // SystemTray.dll, Taskbar.View.dll
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

    wchar_t modulePath[MAX_PATH];
    GetModuleFileName(nullptr, modulePath, ARRAYSIZE(modulePath));
    PCWSTR fileName = wcsrchr(modulePath, L'\\');
    fileName = fileName ? fileName + 1 : modulePath;

    bool isExplorer = _wcsicmp(fileName, L"explorer.exe") == 0;

    if (isExplorer) {
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
    }

    // Hook Win32 APIs for proactive flyout repositioning in ALL included processes (explorer.exe and ShellHost.exe).
    HMODULE user32 = GetModuleHandle(L"user32.dll");
    if (user32) {
        auto pMonitorFromPoint = (decltype(&MonitorFromPoint))
            GetProcAddress(user32, "MonitorFromPoint");
        auto pMonitorFromRect = (decltype(&MonitorFromRect))
            GetProcAddress(user32, "MonitorFromRect");
        auto pMonitorFromWindow = (decltype(&MonitorFromWindow))
            GetProcAddress(user32, "MonitorFromWindow");
        auto pSetWindowPos = (decltype(&SetWindowPos))
            GetProcAddress(user32, "SetWindowPos");
        auto pMoveWindow = (decltype(&MoveWindow))
            GetProcAddress(user32, "MoveWindow");
        auto pDeferWindowPos = (decltype(&DeferWindowPos))
            GetProcAddress(user32, "DeferWindowPos");
        auto pTrackPopupMenuEx = (decltype(&TrackPopupMenuEx))
            GetProcAddress(user32, "TrackPopupMenuEx");

        if (pMonitorFromPoint)
            WindhawkUtils::SetFunctionHook(pMonitorFromPoint,
                                           MonitorFromPoint_Hook,
                                           &MonitorFromPoint_Original);
        if (pMonitorFromRect)
            WindhawkUtils::SetFunctionHook(pMonitorFromRect,
                                           MonitorFromRect_Hook,
                                           &MonitorFromRect_Original);
        if (pMonitorFromWindow)
            WindhawkUtils::SetFunctionHook(pMonitorFromWindow,
                                           MonitorFromWindow_Hook,
                                           &MonitorFromWindow_Original);
        if (pSetWindowPos)
            WindhawkUtils::SetFunctionHook(pSetWindowPos,
                                           SetWindowPos_Hook,
                                           &SetWindowPos_Original);
        if (pMoveWindow)
            WindhawkUtils::SetFunctionHook(pMoveWindow,
                                           MoveWindow_Hook,
                                           &MoveWindow_Original);
        if (pDeferWindowPos)
            WindhawkUtils::SetFunctionHook(pDeferWindowPos,
                                           DeferWindowPos_Hook,
                                           &DeferWindowPos_Original);
        if (pTrackPopupMenuEx)
            WindhawkUtils::SetFunctionHook(pTrackPopupMenuEx,
                                           TrackPopupMenuEx_Hook,
                                           &TrackPopupMenuEx_Original);

        Wh_Log(L"Win32 flyout hooks: MFP=%p MFR=%p MFW=%p SWP=%p MW=%p DWP=%p TPM=%p",
               MonitorFromPoint_Original, MonitorFromRect_Original,
               MonitorFromWindow_Original, SetWindowPos_Original,
               MoveWindow_Original, DeferWindowPos_Original,
               TrackPopupMenuEx_Original);
    }

    // Log taskbar auto-hide state
    {
        APPBARDATA abd = {sizeof(abd)};
        UINT state = (UINT)SHAppBarMessage(ABM_GETSTATE, &abd);
        Wh_Log(L"[Taskbar] auto-hide=%s always-on-top=%s",
               (state & ABS_AUTOHIDE) ? L"YES" : L"NO",
               (state & ABS_ALWAYSONTOP) ? L"YES" : L"NO");
    }

    // Log all monitor geometries for diagnostics
    Wh_Log(L"[MonitorEnum] Enumerating all displays:");
    EnumDisplayMonitors(nullptr, nullptr,
        [](HMONITOR hMon, HDC, LPRECT, LPARAM) -> BOOL {
            MONITORINFOEX mi = {};
            mi.cbSize = sizeof(mi);
            if (GetMonitorInfo(hMon, &mi)) {
                UINT dpiX = 96, dpiY = 96;
                GetDpiForMonitor(hMon, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
                int scale = MulDiv(dpiX, 100, 96);
                Wh_Log(L"[MonitorEnum] hMon=%p \"%s\"%s "
                       L"full=(%d,%d,%d,%d) work=(%d,%d,%d,%d) "
                       L"dpi=%u scale=%d%%",
                       hMon, mi.szDevice,
                       (mi.dwFlags & MONITORINFOF_PRIMARY)
                           ? L" [PRIMARY]" : L"",
                       mi.rcMonitor.left, mi.rcMonitor.top,
                       mi.rcMonitor.right, mi.rcMonitor.bottom,
                       mi.rcWork.left, mi.rcWork.top,
                       mi.rcWork.right, mi.rcWork.bottom,
                       dpiX, scale);
            }
            return TRUE;
        }, 0);

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");
    Wh_Log(L"Primary controller: %p", g_primaryController);
    Wh_Log(L"Secondary controllers: %zu", g_secondaries.size());

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

    // Remove flyout subclasses and clear context
    RemoveAllTaskbarSubclasses();
    ClearFlyoutContext();

    // Remove SizeChanged listener from primary frame
    if (g_primarySystemTrayFrame && g_primarySizeChangedToken.value) {
        g_primarySystemTrayFrame.SizeChanged(g_primarySizeChangedToken);
        g_primarySizeChangedToken = {};
    }

    // Clear loaded event revokers
    g_autoRevokerList.clear();

    // Restore all secondary trays to original state
    for (auto& si : g_secondaries) {
        if (!si.containersUncollapsed || !si.frame) continue;
        try {
            auto grid = FindChildByName(si.frame, L"SystemTrayFrameGrid");
            if (grid) {
                for (auto name : g_syncedContainers) {
                    auto child = FindChildByName(grid, name);
                    if (child && child.ActualWidth() > 0) {
                        child.Visibility(Visibility::Collapsed);
                    }
                }

                auto nai = FindChildByName(grid, L"NotificationAreaIcons");
                if (nai) {
                    auto ic = nai.try_as<Controls::ItemsControl>();
                    if (ic) {
                        ic.ItemsSource(nullptr);
                    }
                }

                si.frame.ClearValue(FrameworkElement::WidthProperty());
                si.frame.InvalidateMeasure();
                si.frame.UpdateLayout();
            }
        } catch (...) {
            Wh_Log(L"Error during cleanup of secondary controller %p",
                   si.controller);
        }
    }

    // Release XAML references
    g_primarySystemTrayFrame = nullptr;
    g_xamlDispatcher = nullptr;
    g_trayMetrics = {};
    for (auto& si : g_secondaries) {
        si.frame = nullptr;
    }
    g_secondaries.clear();
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

// ─── refresh helper ─────────────────────────────────────────────────────────

static void RefreshSecondaryTrayImpl() {
    if (g_unloading) return;
    if (!g_primarySystemTrayFrame) return;

    try {
        double primaryWidth = g_primarySystemTrayFrame.ActualWidth();
        if (primaryWidth <= 0) return;

        Wh_Log(L"[Refresh] Syncing %zu secondaries to width %.0f",
               g_secondaries.size(), primaryWidth);
        g_lastPrimaryFrameSize = primaryWidth;

        for (auto& si : g_secondaries) {
            if (!si.populated || !si.frame) continue;
            try {
                si.frame.Width(primaryWidth);
                si.frame.InvalidateMeasure();
                si.frame.InvalidateArrange();

                auto parent = Media::VisualTreeHelper::GetParent(
                    si.frame).try_as<FrameworkElement>();
                while (parent) {
                    parent.InvalidateMeasure();
                    parent.InvalidateArrange();
                    parent = Media::VisualTreeHelper::GetParent(parent)
                        .try_as<FrameworkElement>();
                }

                si.frame.UpdateLayout();
            } catch (...) {}
        }
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"[Refresh] Failed (hresult): 0x%08X %s",
               (unsigned)ex.code(), ex.message().c_str());
    } catch (...) {
        Wh_Log(L"[Refresh] Failed (unknown exception)");
    }
}

static void RefreshSecondaryTrayOnXamlThread() {
    if (g_unloading) return;

    try {
        if (g_xamlDispatcher) {
            if (g_xamlDispatcher.HasThreadAccess()) {
                RefreshSecondaryTrayImpl();
            } else {
                g_xamlDispatcher.TryRunAsync(
                    winrt::Windows::UI::Core::CoreDispatcherPriority::Normal,
                    winrt::Windows::UI::Core::DispatchedHandler([]() {
                        RefreshSecondaryTrayImpl();
                    }));
            }
            return;
        }
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"[Refresh] Dispatcher marshal failed: 0x%08X %s",
               (unsigned)ex.code(), ex.message().c_str());
    } catch (...) {
        Wh_Log(L"[Refresh] Dispatcher marshal failed (unknown exception)");
    }

    RefreshSecondaryTrayImpl();
}

// Timer IDs for deferred refresh after DPI/display changes
static UINT_PTR g_refreshTimerId = 0;
static UINT_PTR g_refreshTimerId2 = 0;

static void CALLBACK RefreshTimerProc(HWND hwnd, UINT msg, UINT_PTR id,
                                       DWORD time) {
    KillTimer(hwnd, id);
    if (id == 1) {
        g_refreshTimerId = 0;
        Wh_Log(L"[Refresh] Deferred refresh firing (pass 1)");
    } else {
        g_refreshTimerId2 = 0;
        Wh_Log(L"[Refresh] Deferred refresh firing (pass 2)");
    }
    RefreshSecondaryTrayOnXamlThread();
}

static void RefreshSecondaryTray() {
    if (g_unloading) return;

    // During DPI/display changes the primary frame hasn't re-laid-out yet
    // when the notification arrives. Defer the refresh so ActualWidth()
    // returns the new DPI-scaled value. A second pass at 1000ms catches
    // cases where layout settles late (e.g. complex DPI transitions).
    if (g_displayChangeWindow) {
        if (g_refreshTimerId) {
            KillTimer(g_displayChangeWindow, g_refreshTimerId);
        }
        g_refreshTimerId = SetTimer(g_displayChangeWindow, 1, 350,
                                     RefreshTimerProc);

        if (g_refreshTimerId2) {
            KillTimer(g_displayChangeWindow, g_refreshTimerId2);
        }
        g_refreshTimerId2 = SetTimer(g_displayChangeWindow, 2, 1000,
                                      RefreshTimerProc);
    } else {
        RefreshSecondaryTrayOnXamlThread();
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
        if (g_refreshTimerId) {
            KillTimer(g_displayChangeWindow, g_refreshTimerId);
            g_refreshTimerId = 0;
        }
        if (g_refreshTimerId2) {
            KillTimer(g_displayChangeWindow, g_refreshTimerId2);
            g_refreshTimerId2 = 0;
        }
        DestroyWindow(g_displayChangeWindow);
        g_displayChangeWindow = nullptr;
    }
    UnregisterClass(g_displayChangeClassName, GetModuleHandle(nullptr));
}
