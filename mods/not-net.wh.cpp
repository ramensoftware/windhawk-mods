// ==WindhawkMod==
// @id              not-net
// @name            !Net — Per-App Kill Switch
// @description     A panel to manage per app internet connection.
// @author          Ashix
// @version         1.2026.1310
// @github          https://github.com/k-ashix
// @twitter         https://x.com/k_ashix
// @donateUrl       https://ko-fi.com/kashix
// @include         windhawk-mod-elevated.exe
// @include         windhawk-mod.exe
// @include         windhawk.exe
// @compilerOptions -DWIN32_LEAN_AND_MEAN -lfwpuclnt -lrpcrt4 -lole32 -loleaut32 -luuid -lshell32 -lgdi32 -lgdiplus -ldwmapi -luser32 -lcomctl32 -liphlpapi -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*

## Two Modes
- **Session-Only (Floating Pill / Panel, default):** Uses Windows Filtering Platform (WFP) dynamic sessions (`FWPM_SESSION_DYNAMIC`). Blocks vanish automatically on Windows restart, shutdown, or when `!Net` exits or crashes.
- **Auto-Blocked Rules:** Saved in Mod Settings (`autoBlockedApps`) and applied automatically via WFP dynamic sessions (`FWPM_SESSION_DYNAMIC`) whenever `!Net` starts, vanishing cleanly on exit.

## Direction Modes
- **Offline (Fully offline):** Blocks incoming + outgoing traffic (default).
- **Outbound (Can't reach internet):** Blocks outgoing connections only.
- **Inbound (Can't be reached):** Blocks incoming connections only.

## Shortcuts
| Icon | Shortcut / Trigger | Action |
|---|---|---|
| ⚡ | `Ctrl+Alt+Shift+1` | Toggle block/allow for foreground app (`!` key) |
| ◧ | `Ctrl+Alt+Shift+M` | Expand / collapse floating pill |
| ↺ | `Ctrl+Alt+Shift+R` | Restore all (clear session blocks) |

## Cycle Mode
- **Trigger:** Double-click an app row (or right-click → **Cycle...**) — enabled by default in Settings.
- **Schedule:** Alternates **Blocked → Allowed** (`min | sec`, optional `ms` in Settings) using the row's direction. Run **Until stopped**, **For N cycles**, or **For a set time** (supports **Sync with Windows clock** in `12h`/`24h`).
- **Focus & Session:** **Only while app is focused** freezes the timer on focus loss. Cycles are session-only and wipe automatically on app exit/crash, `!Net` exit, or Windows restart.

## Filter Chips
- **Allowed / Blocked:** Click **Blocked** again to switch between `!Net` blocks and **Blocked · FW** (external Windows Defender Firewall rules).
- **Running / User:** Click **Running** again to toggle active-window/network apps vs. all running user apps. Locked system apps and `Blocked · FW` apps are hidden from **Running** by default (toggle **Show non-manageable apps in Running** in Settings).
- **System / Excluded:** View Windows system processes or restore excluded apps.

## Permissions & Safety
- **Administrator Rights:** Modifying WFP session filters requires elevation (via Windhawk's elevated tool host).
- Critical Windows processes (`svchost.exe`, `lsass.exe`, `services.exe`, `csrss.exe`, `winlogon.exe`, `dwm.exe`, `windhawk.exe`, DNS/Firewall services) are locked by default.
- Warns before blocking remote desktop or VPN clients when an active remote session or VPN process is detected.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- General:
  - defaultDirection: fully_offline
    $name: Default block direction
    $options:
    - fully_offline: Fully offline (Incoming + Outgoing)
    - cant_reach_internet: Can't reach internet (Outgoing only)
    - cant_be_reached: Can't be reached (Incoming only)
  - includeChildProcesses: true
    $name: Include child / helper processes by default
    $description: Also blocks descendant executables inside the app's install folder.
- Pill:
  - dockEdge: right
    $name: Dock edge / corner
    $options:
    - right: Right
    - left: Left
    - top: Top
    - bottom: Bottom
    - top_left: Top-Left Corner (L-shape)
    - bottom_left: Bottom-Left Corner (L-shape)
    - bottom_right: Bottom-Right Corner (L-shape)
  - followCursorMonitor: true
    $name: Follow cursor monitor
    $description: Automatically move the collapsed pill and panel to whichever monitor the mouse cursor is currently on.
  - rememberWindowPosition: true
    $name: Remember pill position
  - windowSizePreset: default
    $name: Panel size
    $options:
    - default: Default (436x480)
    - large: Large (680x680)
    - extralarge: Extra Large (840x840)
  - panelEdgeOffsetPx: 8
    $name: Expanded panel edge offset (px)
    $description: Breathing space between the screen edge and the expanded panel (0-12 px).
  - pillIndicatorMode: always
    $name: Pill status indicator
    $options:
    - always: Always show status (Green = Allowed, Red = Blocked)
    - pulse: Pulse on change (Neutral at rest, 1.5s pulse on focus/toggle)
  - hoverExpand: true
    $name: Expand pill on hover
  - expandHoverHitArea: false
    $name: Larger pill hover hit area
    $description: Expand the panel when hovering slightly near the collapsed edge pill.
  - hoverHitMarginPx: 12
    $name: Pill hover hit margin (px)
  - defaultFilter: user
    $name: Default app filter
    $options:
    - user: User apps
    - allowed: Allowed
    - blocked: Blocked
    - system: System apps
    - excluded: Excluded apps
  - showNonActionableInRunning: false
    $name: Show non-manageable apps in Running
    $description: Show locked system apps and Windows Firewall-blocked apps in the Running list (off by default).
  - showTooltips: false
    $name: Show hover tooltips (accessibility)
    $description: Enable descriptive tooltips when hovering over buttons, toggles, and the clock widget.
- Behavior:
  - enableCycleScheduler: true
    $name: Turn on Cycle Mode scheduler
    $description: Allow per-app Cycle Mode scheduling (double-click or right-click an app row).
  - showCycleWidget: true
    $name: Show clock widget near pill
    $description: Keep a compact clock button beside the collapsed pill when Cycle Mode is enabled.
  - cycleWidgetAction: scheduler_only
    $name: Clock widget open behavior
    $description: Choose whether opening from the clock widget shows only the Cycle Mode popup or expands the pill, selects the app, and opens its scheduler.
    $options:
    - scheduler_only: Scheduler popup only (default)
    - navigate_pill: Expand pill, select app, then open scheduler
  - showCycleMs: false
    $name: Show milliseconds (ms) in Cycle popup
    $description: Show the ms input box in the Cycle Mode popup.
  - fullscreenBehavior: hide
    $name: Fullscreen behavior
    $options:
    - hide: Hide
    - allow: Allow
  - hideDuringCapture: true
    $name: Hide during capture
- Safety:
  - lockSystemProcesses: true
    $name: Lock system-critical processes
  - warnOnVpnOrRemote: true
    $name: Warn on VPN or Remote Desktop block
- Hotkeys:
  - toggleForegroundApp: "Ctrl+Alt+Shift+1"
    $name: Toggle foreground app (! key)
    $description: Blocks the active foreground app if allowed, or unblocks it if already blocked.
  - expandCollapsePill: "Ctrl+Alt+Shift+M"
    $name: Expand / collapse pill
  - restoreAll: "Ctrl+Alt+Shift+R"
    $name: Restore all session blocks
- autoBlockedApps:
  - - appPath: ""
      $name: Executable full path
    - direction: fully_offline
      $name: Direction
      $options:
      - fully_offline: Fully offline
      - cant_reach_internet: Can't reach internet
      - cant_be_reached: Can't be reached
    - includeChildProcesses: true
      $name: Include child processes
  $name: Auto-Blocked Apps (WFP Session)
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <shellapi.h>
#include <shlobj.h>
#include <tlhelp32.h>
#include <iphlpapi.h>
#include <rpc.h>
#include <fwpmu.h>
#include <netfw.h>
#include <oleauto.h>
#include <gdiplus.h>

// Official Microsoft SDK WFP GUIDs (omitted in MinGW-w64 fwpmu.h)
static const GUID kWfpCondAleAppId = {
    0xd78e1e87, 0x8644, 0x4ea5, { 0x94, 0x37, 0xd8, 0x09, 0xec, 0xef, 0xc9, 0x71 }
};
static const GUID kWfpSublayerUniversal = {
    0xeebecc03, 0xced4, 0x4380, { 0x81, 0x9a, 0x27, 0x34, 0x39, 0x7b, 0x2b, 0x74 }
};
static const GUID kWfpLayerAleAuthConnectV4 = {
    0xc38d57d1, 0x05a7, 0x4c33, { 0x90, 0x4f, 0x7f, 0xbc, 0xee, 0xe6, 0x0e, 0x82 }
};
static const GUID kWfpLayerAleAuthConnectV6 = {
    0x4a72393b, 0x319f, 0x44bc, { 0x84, 0xc3, 0xba, 0x54, 0xdc, 0xb3, 0xb6, 0xb4 }
};
static const GUID kWfpLayerAleAuthRecvAcceptV4 = {
    0xe1cd9fe7, 0xf4b5, 0x4273, { 0x96, 0xc0, 0x59, 0x2e, 0x48, 0x7b, 0x86, 0x50 }
};
static const GUID kWfpLayerAleAuthRecvAcceptV6 = {
    0xa3b42c97, 0x9f04, 0x4672, { 0xb8, 0x7e, 0xce, 0xe9, 0xc4, 0x83, 0x25, 0x7f }
};

#ifndef FWPM_SESSION_DYNAMIC
#define FWPM_SESSION_DYNAMIC 0x00000001
#endif
#ifndef NET_FW_IP_PROTOCOL_ANY
#define NET_FW_IP_PROTOCOL_ANY 256
#endif

#include <algorithm>
#include <atomic>
#include <cmath>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <cwctype>
#include <mutex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif
#ifndef DWMWCP_DONOTROUND
#define DWMWCP_DONOTROUND 1
#endif
#ifndef DWMWCP_ROUND
#define DWMWCP_ROUND 2
#endif
#ifndef DWMWCP_ROUNDSMALL
#define DWMWCP_ROUNDSMALL 3
#endif
#ifndef WDA_NONE
#define WDA_NONE 0x00000000
#endif
#ifndef WDA_EXCLUDEFROMCAPTURE
#define WDA_EXCLUDEFROMCAPTURE 0x00000011
#endif

// ============================================================================
// Constants & Enums
// ============================================================================

constexpr wchar_t kWndClass[]       = L"ExclaimNet_FloatingWnd_v6";
constexpr wchar_t kCtrlWndClass[]   = L"ExclaimNet_ControlWnd_v6";
constexpr wchar_t kCyclePopupClass[] = L"ExclaimNet_CyclePopup_v1";
constexpr wchar_t kRulePrefix[]     = L"ExclaimNet_";
constexpr wchar_t kTrayTip[]        = L"!Net — Per-App Internet Control";

constexpr UINT WM_APP_RELOAD_SETTINGS = WM_APP + 101;
constexpr UINT WM_APP_TRAY_CALLBACK   = WM_APP + 102;
constexpr UINT WM_APP_REFRESH_APPS    = WM_APP + 103;
constexpr UINT WM_APP_FOREGROUND_CHG  = WM_APP + 104;
constexpr UINT WM_APP_CYCLE_STATE     = WM_APP + 105;
constexpr UINT WM_APP_CYCLE_APPLY     = WM_APP + 106;

constexpr UINT_PTR TIMER_COLLAPSE     = 1;
constexpr UINT_PTR TIMER_ANIM_VSYNC   = 2;
constexpr UINT_PTR TIMER_FULLSCREEN   = 3;
constexpr UINT_PTR TIMER_HOVER_SENSOR    = 4;
constexpr UINT_PTR TIMER_CYCLE_UI        = 5;
constexpr UINT_PTR TIMER_DEBOUNCE_REFRESH = 6;
constexpr UINT_PTR TIMER_WIDGET_PROXIMITY = 86;
constexpr UINT_PTR TIMER_OUTSIDE_CLICK    = 87;
constexpr UINT_PTR TIMER_LATE_CHILD_POLL  = 88;

constexpr int HK_TOGGLE_FG   = 1;
constexpr int HK_EXPAND_PILL = 2;
constexpr int HK_RESTORE_ALL = 3;

enum class Direction {
    FullyOffline = 0,
    CantReachInternet = 1,
    CantBeReached = 2
};

enum class DockEdge { Right = 0, Left = 1, Top = 2, Bottom = 3, TopLeft = 4, BottomLeft = 5, BottomRight = 6 };
static bool IsCornerDock(DockEdge e) {
    return e == DockEdge::TopLeft || e == DockEdge::BottomLeft || e == DockEdge::BottomRight;
}
enum class PillIndicatorMode { Always = 0, Pulse = 1 };
enum class AppFilter { Allowed = 0, Blocked, UserApps, SystemApps, Excluded };
enum class FullscreenBehavior { Hide = 0, Allow = 1 };
enum class SizePreset { Default = 0, Large, ExtraLarge };
enum class UiState { Line = 0, Panel };
enum class PulseKind { None = 0, AllowedGreen, BlockedRed };
enum class CycleWidgetAction { SchedulerOnly = 0, NavigatePill = 1 };

// ============================================================================
// Settings & Data Models
// ============================================================================

struct AutoBlockRuleCfg {
    std::wstring appPath;
    Direction direction = Direction::FullyOffline;
    bool includeChildProcesses = true;
};

struct HotkeyBinding {
    UINT modifiers = 0;
    UINT vk = 0;
    std::wstring raw;
};

struct ModSettings {
    Direction defaultDirection = Direction::FullyOffline;
    bool includeChildProcesses = true;

    DockEdge dockEdge = DockEdge::Right;
    PillIndicatorMode pillIndicatorMode = PillIndicatorMode::Always;
    bool hoverExpand = true;
    bool expandHoverHitArea = false;
    int hoverHitMarginPx = 12;
    int panelEdgeOffsetPx = 4;
    AppFilter defaultFilter = AppFilter::UserApps;
    bool showNonActionableInRunning = false;
    bool showTooltips = false;
    bool followCursorMonitor = true;
    bool rememberWindowPosition = true;
    SizePreset windowSizePreset = SizePreset::Default;

    FullscreenBehavior fullscreenBehavior = FullscreenBehavior::Hide;
    bool hideDuringCapture = true;
    bool enableCycleScheduler = true;
    bool showCycleMs = false;
    bool showCycleWidget = true;
    CycleWidgetAction cycleWidgetAction = CycleWidgetAction::SchedulerOnly;

    bool lockSystemProcesses = true;
    bool warnOnVpnOrRemote = true;

    HotkeyBinding hkToggleFg;
    HotkeyBinding hkExpandPill;
    HotkeyBinding hkRestoreAll;

    std::vector<AutoBlockRuleCfg> autoBlockedApps;
};

struct WfpFilterEntry {
    UINT64 filterId = 0;
    std::wstring exePath;
};

struct BlockState {
    bool blocked = false;
    bool includeChildProcesses = true;
    Direction direction = Direction::FullyOffline;
    std::vector<WfpFilterEntry> sessionFilters;
    std::vector<std::wstring> blockedChildren;
    std::unordered_set<std::wstring> blockedChildPaths;
    std::unordered_set<std::wstring> allowedChildPaths;
};

struct AppEntry {
    std::wstring exePath;
    std::wstring displayPath;
    std::wstring name;
    DWORD primaryPid = 0;
    std::vector<DWORD> pids;
    bool isForeground = false;
    bool hasVisibleWindow = false;
    bool hasLiveNetwork = false;
    bool isSystemApp = false;
    bool isCriticalLocked = false;
    bool isVpnOrRemote = false;
    bool isExcluded = false;
    bool exeExists = true;
    bool isExternalBlocked = false;
    int externalBlockMask = 0;
    std::wstring externalRuleName;
    BlockState block;
};

struct MotionClock {
    LARGE_INTEGER freq{};
    LARGE_INTEGER start{};
    double durationMs = 240.0;
    double fromVal = 0.0;
    double toVal = 0.0;
    double currentVal = 0.0;
    bool active = false;

    void Init() { QueryPerformanceFrequency(&freq); }

    void Start(double target, double durMs) {
        if (freq.QuadPart == 0) Init();
        QueryPerformanceCounter(&start);
        fromVal = currentVal;
        toVal = target;
        durationMs = std::max(1.0, durMs);
        active = (std::abs(toVal - fromVal) > 0.0005);
        if (!active) currentVal = toVal;
    }

    static double EaseInOutCubic(double t) {
        t = std::clamp(t, 0.0, 1.0);
        return (t < 0.5) ? (4.0 * t * t * t) : (1.0 - std::pow(-2.0 * t + 2.0, 3.0) / 2.0);
    }

    bool Step() {
        if (!active) return false;
        LARGE_INTEGER now{};
        QueryPerformanceCounter(&now);
        double elapsedMs = (double)(now.QuadPart - start.QuadPart) * 1000.0 / (double)freq.QuadPart;
        double t = elapsedMs / durationMs;
        if (t >= 1.0) {
            currentVal = toVal;
            active = false;
            return false;
        }
        currentVal = fromVal + (toVal - fromVal) * EaseInOutCubic(t);
        return true;
    }
};

struct PulseState {
    PulseKind kind = PulseKind::None;
    LARGE_INTEGER freq{};
    LARGE_INTEGER start{};
    double durationMs = 1500.0;
    double intensity = 0.0;
    bool active = false;

    void Trigger(PulseKind k, double durMs = 1500.0) {
        if (freq.QuadPart == 0) QueryPerformanceFrequency(&freq);
        QueryPerformanceCounter(&start);
        kind = k;
        durationMs = durMs;
        intensity = 1.0;
        active = (k != PulseKind::None);
    }

    bool Step() {
        if (!active) return false;
        LARGE_INTEGER now{};
        QueryPerformanceCounter(&now);
        double elapsedMs = (double)(now.QuadPart - start.QuadPart) * 1000.0 / (double)freq.QuadPart;
        if (elapsedMs >= durationMs) {
            intensity = 0.0;
            active = false;
            kind = PulseKind::None;
            return false;
        }
        double t = elapsedMs / durationMs;
        double env = (t < 0.15) ? std::sin((t / 0.15) * 1.5707963)
                                : (0.5 * (1.0 + std::cos(((t - 0.15) / 0.85) * 3.14159265)));
        intensity = std::clamp(env, 0.0, 1.0);
        return true;
    }
};

struct DragController {
    bool dragging = false;
    POINT cursorAnchor{};
    RECT startRect{};
    int axisMode = 0;
};

// ============================================================================
// Global State
// ============================================================================

static std::mutex g_settingsMtx;
static ModSettings g_settings;
static int g_lastSettingDockEdge = -1;
static int g_posX = 0;
static int g_posY = 0;
static bool g_posLoaded = false;
static bool g_isLightTheme = false;

static void RefreshLightThemeCache() {
    DWORD val = 0;
    DWORD sz = sizeof(val);
    g_isLightTheme = (RegGetValueW(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &val, &sz) == ERROR_SUCCESS)
        && (val != 0);
}

static std::mutex g_engineMtx;
static HANDLE g_wfpEngine = nullptr;
static GUID g_sublayerGuid{};
static bool g_hasCustomSublayer = false;
static std::unordered_map<std::wstring, BlockState> g_blocks;
static std::unordered_map<std::wstring, Direction> g_pendingDirs; // remembers direction choice before blocking
static std::unordered_set<std::wstring> g_excludedExes;

static HANDLE g_uiThread = nullptr;
static DWORD g_uiThreadId = 0;
static HANDLE g_uiReadyEvent = nullptr;

static HWND g_hWnd = nullptr;
static HWND g_hCtrlWnd = nullptr;
static HWND g_hTooltip = nullptr;
static NOTIFYICONDATAW g_nid{};
static bool g_trayAdded = false;
static UINT g_wmTaskbarCreated = 0;

static HWINEVENTHOOK g_hookFg = nullptr;
static HWINEVENTHOOK g_hookCreate = nullptr;

static ULONG_PTR g_gdiplusToken = 0;
static std::unordered_map<std::wstring, HICON> g_iconCache;

static UiState g_uiState = UiState::Line;
static bool g_pinned = false;
static bool g_mouseInside = false;
static bool g_hiddenByFullscreen = false;
static bool g_lastRoundedFrame = false;
static MotionClock g_expandClock;
static MotionClock g_scrollClock;
static PulseState g_pillPulse;
static DragController g_drag;

static AppFilter g_activeFilter = AppFilter::UserApps;
static bool g_userRunningOnly = true;
static bool g_blockedFirewallOnly = false;
static bool g_userPinnedFilter = false;
static int g_maxScrollPx = 0;
static int g_hoverHitId = -1;
static int g_pressedHitId = -1;
static int g_kbSelectedIdx = -1;
static int g_lastListH = 300;
static int g_lastRowStride = 54;

static std::vector<AppEntry> g_appList;
static std::wstring g_lastFgExe;
static std::wstring g_statusBanner;
static bool g_needsElevation = false;

struct HitRect {
    RECT rc{};
    int id = 0;
    int rowIndex = -1;
    std::wstring tooltip;
    enum Kind {
        None = 0, PinBtn, RestoreAllBtn, SettingsBtn, ElevateBtn,
                FilterChip, RowToggle, RowDirection, RowExclude, RowExternalLocked, RowBody, HeaderDrag
    } kind = None;
};
static std::vector<HitRect> g_hitRects;

extern HANDLE g_toolModProcessMutex;

// ============================================================================
// Forward Declarations
// ============================================================================

static void LoadSettings();
static void SaveExcludedApps();
static void LoadExcludedApps();
static void LogAudit(const std::wstring& msg);
static std::wstring ToLower(std::wstring s);
static std::wstring GetFileNameOnly(const std::wstring& path);
static Direction ParseDirection(PCWSTR s);
static const wchar_t* DirectionLabel(Direction d);
static const wchar_t* DirectionShortLabel(Direction d);
static void RefreshAppList();
static bool ApplyBlock(std::wstring exePath, Direction dir, bool includeChildren, bool askUser = true);
static bool ApplyAllow(std::wstring exePath);
static void RestoreAllSessionBlocks();
static void UpdateWindowGeometry(bool immediate);
static void RequestRepaint(const RECT* dirty = nullptr);
static void OpenWindhawkSettingsUI();
static void TriggerForegroundPulse(const std::wstring& fgExe);

// ============================================================================
// Cycle Mode Data Models & Forward Declarations
// ============================================================================

enum class CycleRunMode { UntilStopped = 0, ForCycles, ForDuration };
enum class CyclePhase { Blocked = 0, Allowed };

struct CycleConfig {
    uint64_t blockMs = 10000;
    uint64_t allowMs = 5000;
    CycleRunMode runMode = CycleRunMode::UntilStopped;
    uint64_t runValue = 0; // cycle count for ForCycles, or total ms for ForDuration
    bool focusedOnly = true;
    bool allowWhenUnfocused = true;
    uint64_t targetEndUtcMs = 0; // >0 when synced with Windows clock
    Direction direction = Direction::FullyOffline;
};

struct CycleStatusSnapshot {
    bool active = false;
    std::wstring exePath;
    std::wstring displayPath;
    Direction direction = Direction::FullyOffline;
    CyclePhase phase = CyclePhase::Blocked;
    uint64_t phaseRemainingMs = 0;
    bool pausedByFocus = false;
    bool pausedByUser = false;
    uint64_t completedCycles = 0;
    uint64_t targetCycles = 0;
    CycleRunMode runMode = CycleRunMode::UntilStopped;
    uint64_t totalRemainingMs = 0;
    bool focusedOnly = true;
    bool allowWhenUnfocused = true;
    uint64_t blockMs = 10000;
    uint64_t allowMs = 5000;
};

static bool StartCycleForApp(const std::wstring& exePath, const CycleConfig& cfg, std::wstring& outErr);
static bool StopCycleForApp(const std::wstring& exePath, bool setAllowed);
static void StopAllCycles(bool setAllowed);
static bool GetCycleSnapshot(const std::wstring& exePath, CycleStatusSnapshot& outSnap);
static bool HasAnyActiveCycle();
static bool ToggleCyclePauseForApp(const std::wstring& exePath, bool& outNowPaused);
static bool GetWidgetCycleTarget(CycleStatusSnapshot& outSnap);
static void NotifyCycleFocusChanged(const std::wstring& fgExeLower);
static void NotifyCycleSystemResume();
static HWND g_hCyclePopup = nullptr;
static HWND g_hCycleWidget = nullptr;
static void OpenCyclePopupForApp(const AppEntry& app);
static void SyncCycleWidgetWindow(bool reposition);
static void DestroyCycleWidgetWindow();


// ============================================================================
// Utility & Elevation Helpers
// ============================================================================

static std::wstring ToLower(std::wstring s) {
    for (auto& ch : s) ch = (wchar_t)towlower(ch);
    return s;
}

static std::wstring Trim(const std::wstring& s) {
    size_t a = s.find_first_not_of(L" \t\r\n");
    if (a == std::wstring::npos) return L"";
    size_t b = s.find_last_not_of(L" \t\r\n");
    return s.substr(a, b - a + 1);
}

static std::wstring GetFileNameOnly(const std::wstring& path) {
    size_t pos = path.find_last_of(L"\\/");
    return (pos == std::wstring::npos) ? path : path.substr(pos + 1);
}

static bool FileExistsW(const std::wstring& path) {
    if (path.empty()) return false;
    PVOID oldRedir = nullptr;
    BOOL redirDisabled = Wow64DisableWow64FsRedirection(&oldRedir);
    DWORD attr = GetFileAttributesW(path.c_str());
    if (redirDisabled) Wow64RevertWow64FsRedirection(oldRedir);
    return (attr != INVALID_FILE_ATTRIBUTES) && !(attr & FILE_ATTRIBUTE_DIRECTORY);
}

extern HANDLE g_toolModProcessMutex;
static void RelaunchElevated();
static bool IsProcessElevated() {
    HANDLE hToken = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) return false;
    TOKEN_ELEVATION elev{};
    DWORD retLen = 0;
    bool isElev = GetTokenInformation(hToken, TokenElevation, &elev, sizeof(elev), &retLen) && elev.TokenIsElevated != 0;
    CloseHandle(hToken);
    return isElev;
}

static void LoadExcludedApps() {
    g_excludedExes.clear();
    int count = Wh_GetIntValue(L"excludedCount", 0);
    for (int i = 0; i < count; ++i) {
        std::wstring key = L"excluded_" + std::to_wstring(i);
        wchar_t buf[1024]{};
        if (Wh_GetStringValue(key.c_str(), buf, ARRAYSIZE(buf)) > 0) {
            std::wstring item = ToLower(Trim(buf));
            if (!item.empty()) g_excludedExes.insert(item);
        }
    }
}

static void SaveExcludedApps() {
    Wh_SetIntValue(L"excludedCount", (int)g_excludedExes.size());
    int idx = 0;
    for (const auto& ex : g_excludedExes) {
        std::wstring key = L"excluded_" + std::to_wstring(idx++);
        Wh_SetStringValue(key.c_str(), ex.c_str());
    }
}

static Direction ParseDirection(PCWSTR s) {
    if (!s) return Direction::FullyOffline;
    std::wstring v = ToLower(s);
    if (v == L"cant_reach_internet" || v == L"outbound") return Direction::CantReachInternet;
    if (v == L"cant_be_reached" || v == L"inbound") return Direction::CantBeReached;
    return Direction::FullyOffline;
}

static const wchar_t* DirectionLabel(Direction d) {
    switch (d) {
        case Direction::CantReachInternet: return L"Outbound only — app cannot reach the internet";
        case Direction::CantBeReached:     return L"Inbound only — nothing can connect to this app";
        default:                           return L"Fully offline — blocks both incoming and outgoing";
    }
}

static const wchar_t* DirectionShortLabel(Direction d) {
    switch (d) {
        case Direction::CantReachInternet: return L"Outbound";
        case Direction::CantBeReached:     return L"Inbound";
        default:                           return L"Offline";
    }
}

static int DpiScale(int px, UINT dpi) {
    return MulDiv(px, (int)dpi, 96);
}

static void LogAudit(const std::wstring& msg) {
    Wh_Log(L"%s", msg.c_str());
}

// ============================================================================
// Safety Classification
// ============================================================================

static bool IsCriticalSystemExe(const std::wstring& lowerExePath) {
    std::wstring name = GetFileNameOnly(lowerExePath);
    static const wchar_t* kCritical[] = {
        L"svchost.exe", L"lsass.exe", L"services.exe", L"csrss.exe",
        L"winlogon.exe", L"dwm.exe", L"smss.exe", L"wininit.exe",
        L"system", L"ntoskrnl.exe", L"wuauclt.exe", L"usoclient.exe",
        L"windhawk.exe", L"audiodg.exe", L"fontdrvhost.exe", L"sihost.exe",
        L"conhost.exe", L"ctfmon.exe", L"wlanext.exe", L"dashost.exe",
        L"searchhost.exe", L"startmenuexperiencehost.exe", L"shellexperiencehost.exe",
        L"msmpeng.exe", L"nissrv.exe", L"smartscreen.exe", L"taskmgr.exe"
    };
    for (auto* c : kCritical) {
        if (name == c) return true;
    }
    return false;
}

static bool IsSystemPath(const std::wstring& lowerExePath) {
    if (lowerExePath.find(L"\\windows\\system32\\") != std::wstring::npos) return true;
    if (lowerExePath.find(L"\\windows\\syswow64\\") != std::wstring::npos) return true;
    if (lowerExePath.find(L"\\windows\\") != std::wstring::npos &&
        lowerExePath.find(L"\\windowsapps\\") == std::wstring::npos) {
        return true;
    }
    return false;
}

static bool IsVpnOrRemoteExe(const std::wstring& lowerExePath) {
    std::wstring name = GetFileNameOnly(lowerExePath);
    static const wchar_t* kRemote[] = {
        L"mstsc.exe", L"msrdc.exe", L"teamviewer.exe", L"anydesk.exe",
        L"rustdesk.exe", L"parsec.exe", L"openvpn.exe", L"openvpn-gui.exe",
        L"wireguard.exe", L"tailscaled.exe", L"tailscale-ipn.exe", L"tailscale.exe",
        L"zerotier-one_x64.exe", L"nordvpn.exe", L"protonvpn.exe", L"protonvpn.service.exe",
        L"mullvad-vpn.exe", L"mullvad-daemon.exe", L"expressvpn.exe", L"surfshark.exe",
        L"cloudflare-warp.exe", L"warp-svc.exe", L"sunshine.exe", L"moonlight.exe",
        L"splashtop.exe", L"chrome_remote_desktop_host.exe", L"nomachine.exe",
        L"nxplayer.exe", L"hamachi-2.exe", L"radmin.exe", L"vncviewer.exe"
    };
    for (auto* r : kRemote) {
        if (name == r) return true;
    }
    if (GetSystemMetrics(SM_REMOTESESSION) != 0 &&
        (name == L"rdpclip.exe" || name == L"tstheme.exe")) {
        return true;
    }
    return false;
}

// ============================================================================
// Process & Network Discovery
// ============================================================================

static std::wstring QueryProcessExePath(DWORD pid) {
    if (pid == 0 || pid == 4) return L"";
    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProc) return L"";
    wchar_t buf[MAX_PATH * 2]{};
    DWORD len = (DWORD)(sizeof(buf) / sizeof(buf[0]));
    std::wstring result;
    if (QueryFullProcessImageNameW(hProc, 0, buf, &len)) {
        result.assign(buf, len);
    }
    CloseHandle(hProc);
    return result;
}

struct ProcessSnapshotItem {
    DWORD pid = 0;
    DWORD parentPid = 0;
    std::wstring exePath;
};

static std::vector<ProcessSnapshotItem> CaptureProcessSnapshot() {
    std::vector<ProcessSnapshotItem> out;
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return out;
    PROCESSENTRY32W pe{};
    pe.dwSize = sizeof(pe);
    if (Process32FirstW(snap, &pe)) {
        do {
            std::wstring path = QueryProcessExePath(pe.th32ProcessID);
            if (!path.empty()) {
                out.push_back({pe.th32ProcessID, pe.th32ParentProcessID, path});
            }
        } while (Process32NextW(snap, &pe));
    }
    CloseHandle(snap);
    return out;
}

static std::vector<std::wstring> CollectChildExePathsFromSnap(const std::wstring& parentLowerPath,
                                                               const std::vector<ProcessSnapshotItem>& snap) {
    std::vector<std::wstring> result;
    std::unordered_set<DWORD> rootPids;
    std::unordered_map<DWORD, std::vector<DWORD>> childrenMap;
    std::unordered_map<DWORD, std::wstring> pidToPath;

    for (const auto& item : snap) {
        std::wstring low = ToLower(item.exePath);
        pidToPath[item.pid] = low;
        childrenMap[item.parentPid].push_back(item.pid);
        if (low == parentLowerPath) {
            rootPids.insert(item.pid);
        }
    }

    std::unordered_set<DWORD> visited;
    std::vector<DWORD> stack(rootPids.begin(), rootPids.end());
    std::unordered_set<std::wstring> uniqueExes;
    uniqueExes.insert(parentLowerPath);

    size_t slashPos = parentLowerPath.find_last_of(L"\\/");
    std::wstring rootDir = (slashPos != std::wstring::npos) ? parentLowerPath.substr(0, slashPos + 1) : L"";
    wchar_t winDirBuf[MAX_PATH]{};
    std::wstring winDir;
    if (GetWindowsDirectoryW(winDirBuf, ARRAYSIZE(winDirBuf)) > 0) {
        winDir = ToLower(winDirBuf);
        if (!winDir.empty() && winDir.back() != L'\\') winDir.push_back(L'\\');
    }
    auto isSharedRuntime = [](const std::wstring& p) -> bool {
        std::wstring fn = GetFileNameOnly(p);
        static const wchar_t* kShared[] = {
            L"msedgewebview2.exe", L"powershell.exe", L"pwsh.exe", L"cmd.exe",
            L"conhost.exe", L"git.exe", L"node.exe", L"python.exe", L"pythonw.exe"
        };
        for (auto* s : kShared) if (fn == s) return true;
        return false;
    };

    while (!stack.empty()) {
        DWORD cur = stack.back();
        stack.pop_back();
        if (!visited.insert(cur).second) continue;
        auto itPath = pidToPath.find(cur);
        if (itPath != pidToPath.end() && !IsCriticalSystemExe(itPath->second)) {
            const std::wstring& cp = itPath->second;
            bool inRoot = !rootDir.empty() && cp.size() >= rootDir.size() &&
                          cp.compare(0, rootDir.size(), rootDir) == 0;
            bool inWinDir = !winDir.empty() && cp.size() >= winDir.size() &&
                            cp.compare(0, winDir.size(), winDir) == 0;
            if (cp == parentLowerPath || (inRoot && !inWinDir && !isSharedRuntime(cp))) {
                uniqueExes.insert(cp);
            }
        }
        auto itCh = childrenMap.find(cur);
        if (itCh != childrenMap.end()) {
            for (DWORD ch : itCh->second) {
                stack.push_back(ch);
            }
        }
    }
    result.assign(uniqueExes.begin(), uniqueExes.end());
    return result;
}

static std::vector<std::wstring> CollectChildExePaths(const std::wstring& parentLowerPath) {
    auto snap = CaptureProcessSnapshot();
    return CollectChildExePathsFromSnap(parentLowerPath, snap);
}

static std::unordered_set<DWORD> GetPidsWithLiveNetwork() {
    std::unordered_set<DWORD> pids;
    DWORD size = 0;
    if (GetExtendedTcpTable(nullptr, &size, FALSE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0) == ERROR_INSUFFICIENT_BUFFER && size > 0) {
        std::vector<BYTE> buf(size);
        if (GetExtendedTcpTable(buf.data(), &size, FALSE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0) == NO_ERROR) {
            auto* tbl = reinterpret_cast<PMIB_TCPTABLE_OWNER_PID>(buf.data());
            for (DWORD i = 0; i < tbl->dwNumEntries; ++i) {
                pids.insert(tbl->table[i].dwOwningPid);
            }
        }
    }
    size = 0;
    if (GetExtendedTcpTable(nullptr, &size, FALSE, AF_INET6, TCP_TABLE_OWNER_PID_ALL, 0) == ERROR_INSUFFICIENT_BUFFER && size > 0) {
        std::vector<BYTE> buf(size);
        if (GetExtendedTcpTable(buf.data(), &size, FALSE, AF_INET6, TCP_TABLE_OWNER_PID_ALL, 0) == NO_ERROR) {
            auto* tbl = reinterpret_cast<PMIB_TCP6TABLE_OWNER_PID>(buf.data());
            for (DWORD i = 0; i < tbl->dwNumEntries; ++i) {
                pids.insert(tbl->table[i].dwOwningPid);
            }
        }
    }
    size = 0;
    if (GetExtendedUdpTable(nullptr, &size, FALSE, AF_INET, UDP_TABLE_OWNER_PID, 0) == ERROR_INSUFFICIENT_BUFFER && size > 0) {
        std::vector<BYTE> buf(size);
        if (GetExtendedUdpTable(buf.data(), &size, FALSE, AF_INET, UDP_TABLE_OWNER_PID, 0) == NO_ERROR) {
            auto* tbl = reinterpret_cast<PMIB_UDPTABLE_OWNER_PID>(buf.data());
            for (DWORD i = 0; i < tbl->dwNumEntries; ++i) {
                pids.insert(tbl->table[i].dwOwningPid);
            }
        }
    }
    size = 0;
    if (GetExtendedUdpTable(nullptr, &size, FALSE, AF_INET6, UDP_TABLE_OWNER_PID, 0) == ERROR_INSUFFICIENT_BUFFER && size > 0) {
        std::vector<BYTE> buf(size);
        if (GetExtendedUdpTable(buf.data(), &size, FALSE, AF_INET6, UDP_TABLE_OWNER_PID, 0) == NO_ERROR) {
            auto* tbl = reinterpret_cast<PMIB_UDP6TABLE_OWNER_PID>(buf.data());
            for (DWORD i = 0; i < tbl->dwNumEntries; ++i) {
                pids.insert(tbl->table[i].dwOwningPid);
            }
        }
    }
    return pids;
}

struct EnumWinCtx {
    std::unordered_set<DWORD> visiblePids;
};

static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    if (!IsWindowVisible(hwnd)) return TRUE;
    if (GetWindow(hwnd, GW_OWNER) != nullptr) return TRUE;
    LONG exStyle = GetWindowLongW(hwnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_TOOLWINDOW) return TRUE;
    RECT rc{};
    if (!GetWindowRect(hwnd, &rc) || (rc.right - rc.left) <= 0 || (rc.bottom - rc.top) <= 0) return TRUE;
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid) {
        reinterpret_cast<EnumWinCtx*>(lParam)->visiblePids.insert(pid);
    }
    return TRUE;
}

// ============================================================================
// WFP Dynamic Session Engine
// ============================================================================

static bool EnsureWfpEngineLocked() {
    if (g_wfpEngine) return true;
    FWPM_SESSION0 session{};
    session.flags = FWPM_SESSION_DYNAMIC;
    static wchar_t sName[] = L"ExclaimNet Dynamic Session";
    session.displayData.name = sName;
    DWORD err = FwpmEngineOpen0(nullptr, RPC_C_AUTHN_WINNT, nullptr, &session, &g_wfpEngine);
    if (err != ERROR_SUCCESS) {
        Wh_Log(L"FwpmEngineOpen0 failed: 0x%08X", err);
        g_wfpEngine = nullptr;
        return false;
    }

    if (!g_hasCustomSublayer) {
        if (UuidCreate(&g_sublayerGuid) == RPC_S_OK) {
            FWPM_SUBLAYER0 sub{};
            sub.subLayerKey = g_sublayerGuid;
            static wchar_t subName[] = L"ExclaimNet Dynamic Sublayer";
            sub.displayData.name = subName;
            sub.displayData.description = subName;
            sub.flags = 0;
            sub.weight = 0xFFFF;
            DWORD subErr = FwpmSubLayerAdd0(g_wfpEngine, &sub, nullptr);
            if (subErr == ERROR_SUCCESS || subErr == (DWORD)FWP_E_ALREADY_EXISTS) {
                g_hasCustomSublayer = true;
            } else {
                g_sublayerGuid = kWfpSublayerUniversal;
                g_hasCustomSublayer = true;
            }
        } else {
            g_sublayerGuid = kWfpSublayerUniversal;
            g_hasCustomSublayer = true;
        }
    }
    return true;
}

static void CloseWfpEngineLocked() {
    if (g_wfpEngine) {
        FwpmEngineClose0(g_wfpEngine);
        g_wfpEngine = nullptr;
        g_hasCustomSublayer = false;
    }
}

static bool AddWfpBlockForExeLocked(const std::wstring& exePath, Direction dir, std::vector<WfpFilterEntry>& outFilters, bool& outAccessDenied) {
    outAccessDenied = false;
    if (!EnsureWfpEngineLocked()) return false;

    FWP_BYTE_BLOB* appBlob = nullptr;
    PVOID oldRedir = nullptr;
    BOOL redirDisabled = Wow64DisableWow64FsRedirection(&oldRedir);
    DWORD err = FwpmGetAppIdFromFileName0(exePath.c_str(), &appBlob);
    if (redirDisabled) Wow64RevertWow64FsRedirection(oldRedir);

    if (err != ERROR_SUCCESS || !appBlob) {
        Wh_Log(L"FwpmGetAppIdFromFileName0 failed for %s: 0x%08X", exePath.c_str(), err);
        return false;
    }

    auto addLayer = [&](const GUID& layerKey) -> bool {
        FWPM_FILTER_CONDITION0 cond{};
        cond.fieldKey = kWfpCondAleAppId;
        cond.matchType = FWP_MATCH_EQUAL;
        cond.conditionValue.type = FWP_BYTE_BLOB_TYPE;
        cond.conditionValue.byteBlob = appBlob;

        FWPM_FILTER0 filter{};
        static wchar_t fName[] = L"ExclaimNet_SessionBlock";
        filter.displayData.name = fName;
        filter.layerKey = layerKey;
        filter.subLayerKey = g_sublayerGuid;
        filter.action.type = FWP_ACTION_BLOCK;
        filter.flags = 0;
        filter.weight.type = FWP_UINT8;
        filter.weight.uint8 = 15;
        filter.numFilterConditions = 1;
        filter.filterCondition = &cond;

        UINT64 filterId = 0;
        DWORD addErr = FwpmFilterAdd0(g_wfpEngine, &filter, nullptr, &filterId);
        if (addErr == ERROR_SUCCESS) {
            outFilters.push_back({filterId, exePath});
            return true;
        }
        if (addErr == ERROR_ACCESS_DENIED) outAccessDenied = true;
        Wh_Log(L"FwpmFilterAdd0 failed: 0x%08X", addErr);
        return false;
    };

    bool anyAdded = false;
    if (dir == Direction::FullyOffline || dir == Direction::CantReachInternet) {
        anyAdded |= addLayer(kWfpLayerAleAuthConnectV4);
        anyAdded |= addLayer(kWfpLayerAleAuthConnectV6);
    }
    if (dir == Direction::FullyOffline || dir == Direction::CantBeReached) {
        anyAdded |= addLayer(kWfpLayerAleAuthRecvAcceptV4);
        anyAdded |= addLayer(kWfpLayerAleAuthRecvAcceptV6);
    }

    FwpmFreeMemory0(reinterpret_cast<void**>(&appBlob));
    return anyAdded;
}

static void RemoveWfpFiltersLocked(std::vector<WfpFilterEntry>& filters) {
    if (!g_wfpEngine) {
        filters.clear();
        return;
    }
    for (const auto& f : filters) {
        FwpmFilterDeleteById0(g_wfpEngine, f.filterId);
    }
    filters.clear();
}

struct ExtFwInfo {
    int dirMask = 0;
    std::wstring ruleName;
};
static std::mutex g_extFwMtx;
static std::unordered_map<std::wstring, ExtFwInfo> g_extFwBlocks;
static ULONGLONG g_extFwScanTick = 0;

static std::unordered_map<std::wstring, ExtFwInfo> GetCachedExternalFirewallBlocks() {
    std::lock_guard<std::mutex> lk(g_extFwMtx);
    return g_extFwBlocks;
}

static std::unordered_map<std::wstring, ExtFwInfo> ScanExternalFirewallBlocks(bool force = false) {
    ULONGLONG now = GetTickCount64();
    {
        std::lock_guard<std::mutex> lk(g_extFwMtx);
        if (!force && g_extFwScanTick != 0 && (now - g_extFwScanTick) < 15000) {
            return g_extFwBlocks;
        }
    }

    std::unordered_map<std::wstring, ExtFwInfo> fresh;
    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    INetFwPolicy2* pPolicy = nullptr;
    if (SUCCEEDED(CoCreateInstance(__uuidof(NetFwPolicy2), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pPolicy))) && pPolicy) {
        long currentProfiles = NET_FW_PROFILE2_ALL;
        pPolicy->get_CurrentProfileTypes(&currentProfiles);

        INetFwRules* pRules = nullptr;
        if (SUCCEEDED(pPolicy->get_Rules(&pRules)) && pRules) {
            IUnknown* pUnk = nullptr;
            if (SUCCEEDED(pRules->get__NewEnum(&pUnk)) && pUnk) {
                IEnumVARIANT* pEnum = nullptr;
                if (SUCCEEDED(pUnk->QueryInterface(IID_PPV_ARGS(&pEnum))) && pEnum) {
                    VARIANT var{};
                    ULONG fetched = 0;
                    while (pEnum->Next(1, &var, &fetched) == S_OK && fetched == 1) {
                        if (var.vt == VT_DISPATCH && var.pdispVal) {
                            INetFwRule* pRule = nullptr;
                            if (SUCCEEDED(var.pdispVal->QueryInterface(IID_PPV_ARGS(&pRule))) && pRule) {
                                VARIANT_BOOL enabled = VARIANT_FALSE;
                                NET_FW_ACTION action = NET_FW_ACTION_ALLOW;
                                long profiles = 0;
                                if (SUCCEEDED(pRule->get_Enabled(&enabled)) && enabled == VARIANT_TRUE &&
                                    SUCCEEDED(pRule->get_Action(&action)) && action == NET_FW_ACTION_BLOCK &&
                                    SUCCEEDED(pRule->get_Profiles(&profiles)) &&
                                    ((profiles & currentProfiles) != 0 || profiles == NET_FW_PROFILE2_ALL)) {
                                    BSTR bName = nullptr;
                                    bool isOurs = false;
                                    std::wstring ruleNameStr;
                                    if (SUCCEEDED(pRule->get_Name(&bName)) && bName) {
                                        if (wcsncmp(bName, kRulePrefix, wcslen(kRulePrefix)) == 0) {
                                            isOurs = true;
                                        } else {
                                            ruleNameStr = bName;
                                        }
                                        SysFreeString(bName);
                                    }
                                    if (!isOurs) {
                                        auto isWildcardOrEmpty = [](BSTR b) {
                                            if (!b || !*b) return true;
                                            return wcscmp(b, L"*") == 0 || _wcsicmp(b, L"All") == 0;
                                        };
                                        long proto = NET_FW_IP_PROTOCOL_ANY;
                                        pRule->get_Protocol(&proto);
                                        BSTR bLP = nullptr, bRP = nullptr, bRA = nullptr, bLA = nullptr;
                                        pRule->get_LocalPorts(&bLP);
                                        pRule->get_RemotePorts(&bRP);
                                        pRule->get_RemoteAddresses(&bRA);
                                        pRule->get_LocalAddresses(&bLA);
                                        bool isBlanket = (proto == NET_FW_IP_PROTOCOL_ANY) &&
                                                         isWildcardOrEmpty(bLP) && isWildcardOrEmpty(bRP) &&
                                                         isWildcardOrEmpty(bRA) && isWildcardOrEmpty(bLA);
                                        if (bLP) SysFreeString(bLP);
                                        if (bRP) SysFreeString(bRP);
                                        if (bRA) SysFreeString(bRA);
                                        if (bLA) SysFreeString(bLA);

                                        if (isBlanket) {
                                            BSTR bApp = nullptr;
                                            if (SUCCEEDED(pRule->get_ApplicationName(&bApp)) && bApp) {
                                                if (*bApp) {
                                                    std::wstring rawApp(bApp);
                                                    static BOOL s_isWow64 = []() {
                                                        BOOL w = FALSE;
                                                        IsWow64Process(GetCurrentProcess(), &w);
                                                        return w;
                                                    }();
                                                    if (s_isWow64 && _wcsnicmp(rawApp.c_str(), L"%ProgramFiles%", 14) == 0) {
                                                        rawApp = L"%ProgramW6432%" + rawApp.substr(14);
                                                    }
                                                    wchar_t expanded[MAX_PATH * 2]{};
                                                    DWORD expLen = ExpandEnvironmentStringsW(rawApp.c_str(), expanded, ARRAYSIZE(expanded));
                                                    std::wstring appLow = ToLower((expLen > 0 && expLen < ARRAYSIZE(expanded)) ? expanded : rawApp);
                                                    NET_FW_RULE_DIRECTION fwDir = NET_FW_RULE_DIR_OUT;
                                                    pRule->get_Direction(&fwDir);
                                                    int bit = (fwDir == NET_FW_RULE_DIR_IN) ? 2 : 1;
                                                    auto& info = fresh[appLow];
                                                    info.dirMask |= bit;
                                                    if (info.ruleName.empty() && !ruleNameStr.empty()) {
                                                        info.ruleName = ruleNameStr;
                                                    }
                                                }
                                                SysFreeString(bApp);
                                            }
                                        }
                                    }
                                }
                                pRule->Release();
                            }
                        }
                        VariantClear(&var);
                    }
                    pEnum->Release();
                }
                pUnk->Release();
            }
            pRules->Release();
        }
        pPolicy->Release();
    }
    if (SUCCEEDED(hrCo)) CoUninitialize();

    {
        std::lock_guard<std::mutex> lk(g_extFwMtx);
        g_extFwBlocks = fresh;
        g_extFwScanTick = now;
    }
    return fresh;
}

// ============================================================================
// High-Level Block / Allow / Restore / Pulse Operations
// ============================================================================

static void TriggerPulse(PulseKind kind) {
    if (!g_hWnd) return;
    g_pillPulse.Trigger(kind, 1500.0);
    SetTimer(g_hWnd, TIMER_ANIM_VSYNC, 16, nullptr);
    RequestRepaint();
}

static bool IsDesktopOrShellWindow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return true;
    wchar_t cls[128]{};
    if (GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) > 0) {
        if (wcscmp(cls, L"Progman") == 0 ||
            wcscmp(cls, L"WorkerW") == 0 ||
            wcscmp(cls, L"Shell_TrayWnd") == 0 ||
            wcscmp(cls, L"Shell_SecondaryTrayWnd") == 0 ||
            wcscmp(cls, L"NotifyIconOverflowWindow") == 0 ||
            wcscmp(cls, L"Windows.UI.Core.CoreWindow") == 0 ||
            wcscmp(cls, L"TaskListThumbnailWnd") == 0) {
            return true;
        }
    }
    return false;
}

static void SyncForegroundStateAndFilter(bool allowFilterJump) {
    HWND fg = GetForegroundWindow();
    if (fg == g_hWnd) return;

    if (g_hCyclePopup && fg == g_hCyclePopup) return;
    if (g_hCycleWidget && fg == g_hCycleWidget) return;

    if (!fg || IsDesktopOrShellWindow(fg)) {
        g_lastFgExe.clear();
        NotifyCycleFocusChanged(L"");
        if (g_statusBanner.find(L"Windows Defender Firewall") != std::wstring::npos) {
            g_statusBanner.clear();
        }
        return;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(fg, &pid);
    std::wstring exe = pid ? QueryProcessExePath(pid) : L"";
    std::wstring low = ToLower(exe);
    if (GetFileNameOnly(low) == L"applicationframehost.exe") {
        struct UwpCtx { DWORD hostPid; std::wstring childExe; } uwp{pid, L""};
        struct UwpEnum {
            static BOOL CALLBACK Proc(HWND ch, LPARAM lp) {
                auto* ctx = reinterpret_cast<UwpCtx*>(lp);
                DWORD cpid = 0;
                GetWindowThreadProcessId(ch, &cpid);
                if (cpid && cpid != ctx->hostPid) {
                    std::wstring ce = QueryProcessExePath(cpid);
                    if (!ce.empty()) { ctx->childExe = ce; return FALSE; }
                }
                return TRUE;
            }
        };
        EnumChildWindows(fg, UwpEnum::Proc, reinterpret_cast<LPARAM>(&uwp));
        if (!uwp.childExe.empty()) { exe = uwp.childExe; low = ToLower(exe); }
    }
    if (exe.empty()) {
        g_lastFgExe.clear();
        NotifyCycleFocusChanged(L"");
        return;
    }
    g_lastFgExe = low;
    NotifyCycleFocusChanged(low);

    auto extFw = (g_extFwScanTick == 0) ? ScanExternalFirewallBlocks(false) : GetCachedExternalFirewallBlocks();
    auto itExt = extFw.find(low);
    bool isExtBlk = (itExt != extFw.end() && itExt->second.dirMask != 0);

    bool isNetBlk = false;
    {
        std::lock_guard<std::mutex> lk(g_engineMtx);
        auto it = g_blocks.find(low);
        isNetBlk = (it != g_blocks.end() && it->second.blocked);
    }

    if (allowFilterJump && !g_userPinnedFilter) {
        if (isExtBlk) {
            g_activeFilter = AppFilter::Blocked;
            g_blockedFirewallOnly = true;
            g_statusBanner = L"Blocked by Windows Firewall — edit rules in Windows Defender Firewall.";
        } else {
            if (g_activeFilter == AppFilter::Blocked && g_blockedFirewallOnly) {
                ModSettings defS;
                { std::lock_guard<std::mutex> lk(g_settingsMtx); defS = g_settings; }
                g_activeFilter = defS.defaultFilter;
                g_blockedFirewallOnly = false;
            } else if (isNetBlk && g_activeFilter == AppFilter::Blocked) {
                g_blockedFirewallOnly = false;
            }
            if (g_statusBanner.find(L"Windows Defender Firewall") != std::wstring::npos) {
                g_statusBanner.clear();
            }
        }
    }
}

static void TriggerForegroundPulse(const std::wstring& fgExe) {
    std::wstring low = ToLower(fgExe);
    if (low.empty() || g_excludedExes.count(low)) {
        g_pillPulse.active = false;
        g_pillPulse.kind = PulseKind::None;
        RequestRepaint();
        return;
    }

    bool isBlocked = false;
    {
        std::lock_guard<std::mutex> lk(g_engineMtx);
        auto it = g_blocks.find(low);
        isBlocked = (it != g_blocks.end() && it->second.blocked);
    }
    if (!isBlocked) {
        auto extFw = (g_extFwScanTick == 0) ? ScanExternalFirewallBlocks(false) : GetCachedExternalFirewallBlocks();
        auto itExt = extFw.find(low);
        isBlocked = (itExt != extFw.end() && itExt->second.dirMask != 0);
    }
    TriggerPulse(isBlocked ? PulseKind::BlockedRed : PulseKind::AllowedGreen);
}

static bool ApplyBlock(std::wstring exePath, Direction dir, bool includeChildren, bool askUser) {
    std::wstring low = ToLower(exePath);
    if (g_excludedExes.count(low)) {
        g_statusBanner = L"App is excluded from !Net.";
        return false;
    }
    bool lockCrit = true;
    bool warnVpn = true;
    {
        std::lock_guard<std::mutex> lk(g_settingsMtx);
        lockCrit = g_settings.lockSystemProcesses;
        warnVpn = g_settings.warnOnVpnOrRemote;
    }
    if (lockCrit && IsCriticalSystemExe(low)) {
        g_statusBanner = L"System-critical process is locked.";
        return false;
    }
    if (warnVpn && askUser && IsVpnOrRemoteExe(low)) {
        int res = MessageBoxW(g_hWnd,
                              L"Blocking this app may disconnect your active VPN or Remote Desktop session.\n\nContinue?",
                              L"!Net Safety Warning", MB_YESNO | MB_ICONWARNING | MB_TOPMOST);
        if (res != IDYES) return false;
    }

    std::vector<std::wstring> targets;
    if (includeChildren) {
        targets = CollectChildExePaths(low);
    }
    if (targets.empty()) targets.push_back(low);
    if (lockCrit) {
        targets.erase(std::remove_if(targets.begin(), targets.end(),
            [](const std::wstring& t) { return IsCriticalSystemExe(t); }), targets.end());
    }
    if (targets.empty()) return false;

    std::lock_guard<std::mutex> lk(g_engineMtx);
    std::vector<WfpFilterEntry> newFilters;
    bool anyOk = false;
    bool accessDenied = false;

    for (const auto& t : targets) {
        bool denied = false;
        anyOk |= AddWfpBlockForExeLocked(t, dir, newFilters, denied);
        accessDenied |= denied;
    }

    if (!anyOk) {
        RemoveWfpFiltersLocked(newFilters);
        if (accessDenied || !IsProcessElevated()) {
            g_needsElevation = true;
            g_statusBanner = L"Admin permission needed to block apps.";
        } else {
            g_statusBanner = L"Could not add WFP filter for this app.";
        }
        LogAudit(L"BLOCK FAILED: " + exePath);
        return false;
    }

    g_needsElevation = false;
    auto& state = g_blocks[low];
    RemoveWfpFiltersLocked(state.sessionFilters);
    state.blocked = true;
    state.includeChildProcesses = includeChildren;
    state.direction = dir;
    state.sessionFilters = std::move(newFilters);
    state.blockedChildren.clear();
    state.blockedChildPaths.clear();
    for (const auto& t : targets) {
        if (t != low) {
            state.blockedChildPaths.insert(t);
            state.blockedChildren.push_back(GetFileNameOnly(t));
        }
    }

    LogAudit(L"BLOCK [SESSION] " + exePath + L" (" + DirectionShortLabel(dir) + L")");
    g_statusBanner.clear();
    TriggerPulse(PulseKind::BlockedRed);
    return true;
}

static void SyncLateChildBlocks() {
    std::vector<std::pair<std::wstring, Direction>> toCheck;
    {
        std::lock_guard<std::mutex> lk(g_engineMtx);
        for (const auto& kv : g_blocks) {
            if (kv.second.blocked && kv.second.includeChildProcesses) {
                toCheck.push_back({kv.first, kv.second.direction});
            }
        }
    }
    if (toCheck.empty()) return;
    auto snap = CaptureProcessSnapshot();
    for (const auto& item : toCheck) {
        auto children = CollectChildExePathsFromSnap(item.first, snap);
        std::lock_guard<std::mutex> lk(g_engineMtx);
        auto it = g_blocks.find(item.first);
        if (it == g_blocks.end() || !it->second.blocked) continue;
        for (const auto& ch : children) {
            if (ch == item.first) continue;
            if (it->second.blockedChildPaths.count(ch)) continue;
            if (it->second.allowedChildPaths.count(ch)) continue;
            bool denied = false;
            if (AddWfpBlockForExeLocked(ch, item.second, it->second.sessionFilters, denied)) {
                it->second.blockedChildPaths.insert(ch);
                it->second.blockedChildren.push_back(GetFileNameOnly(ch));
            }
        }
    }
}

static bool ApplyAllow(std::wstring exePath) {
    std::wstring low = ToLower(exePath);
    {
        std::lock_guard<std::mutex> lk(g_engineMtx);
        auto it = g_blocks.find(low);
        if (it != g_blocks.end()) {
            RemoveWfpFiltersLocked(it->second.sessionFilters);
            g_blocks.erase(it);
        } else {
            for (auto& kv : g_blocks) {
                if (!kv.second.blockedChildPaths.count(low)) continue;
                std::wstring childFn = GetFileNameOnly(low);
                kv.second.blockedChildPaths.erase(low);
                kv.second.allowedChildPaths.insert(low);
                kv.second.blockedChildren.erase(
                    std::remove(kv.second.blockedChildren.begin(), kv.second.blockedChildren.end(), childFn),
                    kv.second.blockedChildren.end());
                std::vector<WfpFilterEntry> kept;
                for (const auto& f : kv.second.sessionFilters) {
                    if (ToLower(f.exePath) == low) {
                        if (g_wfpEngine) FwpmFilterDeleteById0(g_wfpEngine, f.filterId);
                    } else {
                        kept.push_back(f);
                    }
                }
                kv.second.sessionFilters = std::move(kept);
            }
        }
    }
    LogAudit(L"ALLOW " + exePath);
    g_statusBanner.clear();
    TriggerPulse(PulseKind::AllowedGreen);
    return true;
}


// ============================================================================
// Cycle Mode Scheduler
// ============================================================================

struct CycleRuntime {
    std::wstring exePath;     // normalized lowercase
    std::wstring displayPath; // original case for ApplyBlock/ApplyAllow
    CycleConfig cfg;
    CyclePhase phase = CyclePhase::Blocked;
    double phaseStartMs = 0.0;
    double phaseTargetMs = 0.0;
    double totalEndMs = 0.0;
    uint64_t completedCycles = 0;
    bool pausedByFocus = false;
    bool pausedByUser = false;
    double pauseStartMs = 0.0;
    double lastProcCheckMs = 0.0;
    bool includeChildren = true;
};

static std::mutex g_cycleMtx;
static std::condition_variable g_cycleCv;
static std::unordered_map<std::wstring, CycleRuntime> g_cycles;
static std::wstring g_cycleFgExe;
static std::wstring g_lastWidgetCycleExe;
static CycleConfig g_lastCycleConfig; // session-only memory, never saved to YAML
static bool g_hasLastCycleConfig = false;
struct CyclePendingAction {
    enum Kind { SwitchBlock, SwitchAllow, EndAllow, WipeExited } kind;
    std::wstring displayPath;
    Direction dir = Direction::FullyOffline;
    bool includeChildren = true;
};
static std::vector<CyclePendingAction> g_pendingCycleActions;
static HANDLE g_cycleThread = nullptr;
static std::atomic<bool> g_cycleStopping{false};

static uint64_t UtcNowMs() {
    FILETIME ft{};
    GetSystemTimeAsFileTime(&ft);
    ULARGE_INTEGER ui{};
    ui.LowPart = ft.dwLowDateTime;
    ui.HighPart = ft.dwHighDateTime;
    return ui.QuadPart / 10000ULL;
}

static double QpcNowMs() {
    static LARGE_INTEGER freq = []() {
        LARGE_INTEGER f{};
        QueryPerformanceFrequency(&f);
        return f;
    }();
    LARGE_INTEGER now{};
    QueryPerformanceCounter(&now);
    if (freq.QuadPart <= 0) return (double)GetTickCount64();
    return (double)now.QuadPart * 1000.0 / (double)freq.QuadPart;
}

static std::wstring FormatDurationCompact(uint64_t ms) {
    wchar_t buf[64]{};
    if (ms < 1000ULL) {
        swprintf_s(buf, L"%llums", (unsigned long long)ms);
    } else if (ms < 60000ULL) {
        uint64_t sec = (ms + 999ULL) / 1000ULL;
        swprintf_s(buf, L"%llus", (unsigned long long)sec);
    } else if (ms < 3600000ULL) {
        uint64_t totalSec = (ms + 999ULL) / 1000ULL;
        uint64_t m = totalSec / 60ULL;
        uint64_t s = totalSec % 60ULL;
        if (s == 0) swprintf_s(buf, L"%llum", (unsigned long long)m);
        else swprintf_s(buf, L"%llu:%02llu", (unsigned long long)m, (unsigned long long)s);
    } else {
        uint64_t totalSec = (ms + 999ULL) / 1000ULL;
        uint64_t h = totalSec / 3600ULL;
        uint64_t m = (totalSec % 3600ULL) / 60ULL;
        uint64_t s = totalSec % 60ULL;
        swprintf_s(buf, L"%llu:%02llu:%02llu", (unsigned long long)h, (unsigned long long)m, (unsigned long long)s);
    }
    return buf;
}

static bool IsAnyProcessAliveForExe(const std::wstring& lowerExePath) {
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return true; // fail-open on transient snapshot error
    PROCESSENTRY32W pe{};
    pe.dwSize = sizeof(pe);
    bool found = false;
    std::wstring targetName = GetFileNameOnly(lowerExePath);
    if (Process32FirstW(snap, &pe)) {
        do {
            if (_wcsicmp(pe.szExeFile, targetName.c_str()) == 0) {
                std::wstring full = ToLower(QueryProcessExePath(pe.th32ProcessID));
                if (full.empty() || full == lowerExePath) {
                    found = true;
                    break;
                }
            }
        } while (Process32NextW(snap, &pe));
    }
    CloseHandle(snap);
    return found;
}

static CycleStatusSnapshot BuildSnapshotLocked(const CycleRuntime& rt, double nowMs) {
    CycleStatusSnapshot snap;
    snap.active = true;
    snap.exePath = rt.exePath;
    snap.displayPath = rt.displayPath;
    snap.direction = rt.cfg.direction;
    snap.phase = rt.phase;
    snap.pausedByFocus = rt.pausedByFocus;
    snap.pausedByUser = rt.pausedByUser;
    snap.completedCycles = rt.completedCycles;
    snap.targetCycles = (rt.cfg.runMode == CycleRunMode::ForCycles) ? rt.cfg.runValue : 0;
    snap.runMode = rt.cfg.runMode;
    snap.focusedOnly = rt.cfg.focusedOnly;
    snap.allowWhenUnfocused = rt.cfg.allowWhenUnfocused;
    snap.blockMs = rt.cfg.blockMs;
    snap.allowMs = rt.cfg.allowMs;

    double effNow = (rt.pausedByFocus || rt.pausedByUser) ? rt.pauseStartMs : nowMs;
    double remPhase = std::max(0.0, rt.phaseTargetMs - effNow);
    snap.phaseRemainingMs = (uint64_t)std::llround(remPhase);

    if (rt.cfg.runMode == CycleRunMode::ForDuration) {
        double remTot = std::max(0.0, rt.totalEndMs - effNow);
        snap.totalRemainingMs = (uint64_t)std::llround(remTot);
    }
    return snap;
}

static void PostCycleUiSync() {
    if (g_hWnd) PostMessageW(g_hWnd, WM_APP_CYCLE_STATE, 0, 0);
}

static bool StartCycleForApp(const std::wstring& exePath, const CycleConfig& cfg, std::wstring& outErr) {
    std::wstring low = ToLower(Trim(exePath));
    if (low.empty()) {
        outErr = L"No application path provided.";
        return false;
    }
    if (cfg.blockMs < 100 || cfg.allowMs < 100) {
        outErr = L"Minimum phase duration is 100 ms.";
        return false;
    }
    if (cfg.runMode == CycleRunMode::ForCycles && cfg.runValue < 1) {
        outErr = L"Cycle count must be at least 1.";
        return false;
    }
    if (cfg.runMode == CycleRunMode::ForDuration && cfg.runValue < 100) {
        outErr = L"Total duration must be at least 100 ms.";
        return false;
    }
    if (g_excludedExes.count(low)) {
        outErr = L"App is excluded from !Net.";
        return false;
    }
    {
        std::lock_guard<std::mutex> lk(g_settingsMtx);
        if (!g_settings.enableCycleScheduler) {
            outErr = L"Cycle scheduler is off in Windhawk Settings.";
            return false;
        }
        if (g_settings.lockSystemProcesses && IsCriticalSystemExe(low)) {
            outErr = L"Not allowed on locked system processes.";
            return false;
        }
    }
    auto extFw = GetCachedExternalFirewallBlocks();
    auto itExt = extFw.find(low);
    if (itExt != extFw.end() && itExt->second.dirMask != 0) {
        outErr = L"Blocked by Windows Firewall — cannot cycle.";
        return false;
    }

    bool incChildren = true;
    {
        std::lock_guard<std::mutex> lk(g_settingsMtx);
        incChildren = g_settings.includeChildProcesses;
    }

    bool startUnfocusedAllowed = false;
    {
        std::lock_guard<std::mutex> lk(g_cycleMtx);
        startUnfocusedAllowed = (cfg.focusedOnly && cfg.allowWhenUnfocused && g_cycleFgExe != low);
    }

    // Always starts with Blocked unless focusedOnly + allowWhenUnfocused and app is currently unfocused
    if (!startUnfocusedAllowed) {
        if (!ApplyBlock(exePath, cfg.direction, incChildren, true)) {
            outErr = g_statusBanner.empty() ? L"Failed to apply initial block." : g_statusBanner;
            return false;
        }
    }

    double nowMs = QpcNowMs();
    {
        std::lock_guard<std::mutex> lk(g_cycleMtx);
        CycleRuntime rt;
        rt.exePath = low;
        rt.displayPath = exePath;
        rt.cfg = cfg;
        rt.phase = CyclePhase::Blocked;
        rt.phaseStartMs = nowMs;
        rt.phaseTargetMs = nowMs + (double)cfg.blockMs;
        rt.totalEndMs = (cfg.runMode == CycleRunMode::ForDuration) ? (nowMs + (double)cfg.runValue) : 0.0;
        rt.completedCycles = 0;
        rt.includeChildren = incChildren;
        rt.lastProcCheckMs = nowMs;
        rt.pausedByUser = false;
        rt.pausedByFocus = (cfg.focusedOnly && g_cycleFgExe != low);
        if (rt.pausedByFocus) rt.pauseStartMs = nowMs;

        g_cycles[low] = std::move(rt);
        g_lastWidgetCycleExe = low;
        g_lastCycleConfig = cfg;
        g_hasLastCycleConfig = true;
    }
    g_cycleCv.notify_all();
    LogAudit(L"CYCLE START " + exePath + L" (Block " + FormatDurationCompact(cfg.blockMs) +
             L" -> Allow " + FormatDurationCompact(cfg.allowMs) + L")");
    PostCycleUiSync();
    return true;
}

static bool StopCycleForApp(const std::wstring& exePath, bool setAllowed) {
    std::wstring low = ToLower(Trim(exePath));
    std::wstring disp;
    {
        std::lock_guard<std::mutex> lk(g_cycleMtx);
        auto it = g_cycles.find(low);
        if (it == g_cycles.end()) return false;
        disp = it->second.displayPath;
        g_cycles.erase(it);
        if (g_lastWidgetCycleExe == low) g_lastWidgetCycleExe.clear();
    }
    g_cycleCv.notify_all();
    if (setAllowed) {
        ApplyAllow(disp);
    }
    LogAudit(L"CYCLE STOP " + disp);
    PostCycleUiSync();
    return true;
}

static void StopAllCycles(bool setAllowed) {
    std::vector<std::wstring> toAllow;
    {
        std::lock_guard<std::mutex> lk(g_cycleMtx);
        if (g_cycles.empty()) return;
        for (const auto& kv : g_cycles) {
            toAllow.push_back(kv.second.displayPath);
        }
        g_cycles.clear();
        g_lastWidgetCycleExe.clear();
    }
    g_cycleCv.notify_all();
    if (setAllowed) {
        for (const auto& p : toAllow) ApplyAllow(p);
    }
    LogAudit(L"CYCLE STOP ALL");
    PostCycleUiSync();
}

static bool GetCycleSnapshot(const std::wstring& exePath, CycleStatusSnapshot& outSnap) {
    std::wstring low = ToLower(exePath);
    double nowMs = QpcNowMs();
    std::lock_guard<std::mutex> lk(g_cycleMtx);
    auto it = g_cycles.find(low);
    if (it == g_cycles.end()) return false;
    outSnap = BuildSnapshotLocked(it->second, nowMs);
    return true;
}

static bool HasAnyActiveCycle() {
    std::lock_guard<std::mutex> lk(g_cycleMtx);
    return !g_cycles.empty();
}

static bool ToggleCyclePauseForApp(const std::wstring& exePath, bool& outNowPaused) {
    std::wstring low = ToLower(Trim(exePath));
    double nowMs = QpcNowMs();
    std::wstring disp;
    {
        std::lock_guard<std::mutex> lk(g_cycleMtx);
        auto it = g_cycles.find(low);
        if (it == g_cycles.end()) return false;
        CycleRuntime& rt = it->second;
        bool wasPaused = rt.pausedByFocus || rt.pausedByUser;
        rt.pausedByUser = !rt.pausedByUser;
        bool nowPaused = rt.pausedByFocus || rt.pausedByUser;
        if (!wasPaused && nowPaused) {
            rt.pauseStartMs = nowMs;
        } else if (wasPaused && !nowPaused) {
            double pausedDelta = std::max(0.0, nowMs - rt.pauseStartMs);
            rt.phaseStartMs += pausedDelta;
            rt.phaseTargetMs += pausedDelta;
            if (rt.cfg.runMode == CycleRunMode::ForDuration) {
                rt.totalEndMs += pausedDelta;
            }
        }
        outNowPaused = rt.pausedByUser;
        disp = rt.displayPath;
    }
    g_cycleCv.notify_all();
    LogAudit(std::wstring(outNowPaused ? L"CYCLE PAUSE " : L"CYCLE RESUME ") + disp);
    PostCycleUiSync();
    return true;
}

static bool GetWidgetCycleTarget(CycleStatusSnapshot& outSnap) {
    double nowMs = QpcNowMs();
    std::lock_guard<std::mutex> lk(g_cycleMtx);
    if (g_cycles.empty()) return false;
    if (!g_cycleFgExe.empty()) {
        auto itFg = g_cycles.find(g_cycleFgExe);
        if (itFg != g_cycles.end()) {
            outSnap = BuildSnapshotLocked(itFg->second, nowMs);
            return true;
        }
    }
    if (!g_lastWidgetCycleExe.empty()) {
        auto itLast = g_cycles.find(g_lastWidgetCycleExe);
        if (itLast != g_cycles.end()) {
            outSnap = BuildSnapshotLocked(itLast->second, nowMs);
            return true;
        }
    }
    outSnap = BuildSnapshotLocked(g_cycles.begin()->second, nowMs);
    return true;
}

static void NotifyCycleFocusChanged(const std::wstring& fgExeLower) {
    double nowMs = QpcNowMs();
    bool changed = false;
    std::vector<CyclePendingAction> focusActions;
    {
        std::lock_guard<std::mutex> lk(g_cycleMtx);
        if (g_cycleFgExe == fgExeLower) return;
        g_cycleFgExe = fgExeLower;
        if (!fgExeLower.empty() && g_cycles.count(fgExeLower)) {
            g_lastWidgetCycleExe = fgExeLower;
            changed = true;
        }

        for (auto& kv : g_cycles) {
            CycleRuntime& rt = kv.second;
            if (!rt.cfg.focusedOnly) continue;
            bool shouldBeFocused = (rt.exePath == fgExeLower);
            if (!shouldBeFocused && !rt.pausedByFocus) {
                if (!rt.pausedByUser) rt.pauseStartMs = nowMs;
                rt.pausedByFocus = true;
                if (rt.cfg.allowWhenUnfocused && rt.phase == CyclePhase::Blocked) {
                    focusActions.push_back({CyclePendingAction::SwitchAllow, rt.displayPath, rt.cfg.direction, rt.includeChildren});
                }
                changed = true;
            } else if (shouldBeFocused && rt.pausedByFocus) {
                rt.pausedByFocus = false;
                if (!rt.pausedByUser) {
                    double pausedDelta = std::max(0.0, nowMs - rt.pauseStartMs);
                    rt.phaseStartMs += pausedDelta;
                    rt.phaseTargetMs += pausedDelta;
                    if (rt.cfg.runMode == CycleRunMode::ForDuration) {
                        rt.totalEndMs += pausedDelta;
                    }
                }
                if (rt.cfg.allowWhenUnfocused && rt.phase == CyclePhase::Blocked) {
                    focusActions.push_back({CyclePendingAction::SwitchBlock, rt.displayPath, rt.cfg.direction, rt.includeChildren});
                }
                changed = true;
            }
        }
    }
    for (const auto& act : focusActions) {
        if (act.kind == CyclePendingAction::SwitchAllow) {
            ApplyAllow(act.displayPath);
        } else if (act.kind == CyclePendingAction::SwitchBlock) {
            ApplyBlock(act.displayPath, act.dir, act.includeChildren, false);
        }
    }
    if (changed) {
        g_cycleCv.notify_all();
        PostCycleUiSync();
    }
}

static void NotifyCycleSystemResume() {
    double nowMs = QpcNowMs();
    {
        std::lock_guard<std::mutex> lk(g_cycleMtx);
        for (auto& kv : g_cycles) {
            CycleRuntime& rt = kv.second;
            if (rt.pausedByFocus || rt.pausedByUser) {
                double pausedDelta = std::max(0.0, nowMs - rt.pauseStartMs);
                rt.phaseStartMs += pausedDelta;
                rt.phaseTargetMs += pausedDelta;
                if (rt.cfg.runMode == CycleRunMode::ForDuration) rt.totalEndMs += pausedDelta;
                rt.pauseStartMs = nowMs;
            } else if (nowMs > rt.phaseTargetMs) {
                double dur = (double)(rt.phase == CyclePhase::Blocked ? rt.cfg.blockMs : rt.cfg.allowMs);
                if (nowMs - rt.phaseTargetMs >= dur) {
                    rt.phaseStartMs = nowMs;
                    rt.phaseTargetMs = nowMs + dur;
                }
            }
        }
    }
    g_cycleCv.notify_all();
    PostCycleUiSync();
}

static DWORD WINAPI CycleSchedulerThreadProc(LPVOID) {
    while (!g_cycleStopping.load()) {
        std::vector<CyclePendingAction> actions;
        double nextWakeMs = 250.0;

        {
            std::unique_lock<std::mutex> lk(g_cycleMtx);
            if (g_cycles.empty()) {
                g_cycleCv.wait(lk, []() { return g_cycleStopping.load() || !g_cycles.empty(); });
                if (g_cycleStopping.load()) break;
            }

            double nowMs = QpcNowMs();
            std::vector<std::wstring> toErase;

            for (auto& kv : g_cycles) {
                CycleRuntime& rt = kv.second;

                // Periodic process-exit / crash detection (every 500 ms)
                if (nowMs - rt.lastProcCheckMs >= 500.0) {
                    rt.lastProcCheckMs = nowMs;
                    if (!IsAnyProcessAliveForExe(rt.exePath)) {
                        actions.push_back({CyclePendingAction::WipeExited, rt.displayPath, rt.cfg.direction, rt.includeChildren});
                        toErase.push_back(kv.first);
                        continue;
                    }
                }

                if (rt.pausedByFocus || rt.pausedByUser) {
                    nextWakeMs = std::min(nextWakeMs, 250.0);
                    continue;
                }

                // Check total duration limit (monotonic OR Windows wall-clock target)
                if (rt.cfg.runMode == CycleRunMode::ForDuration &&
                    (nowMs >= rt.totalEndMs || (rt.cfg.targetEndUtcMs > 0 && UtcNowMs() >= rt.cfg.targetEndUtcMs))) {
                    actions.push_back({CyclePendingAction::EndAllow, rt.displayPath, rt.cfg.direction, rt.includeChildren});
                    toErase.push_back(kv.first);
                    continue;
                }

                // Check phase transition
                if (nowMs >= rt.phaseTargetMs) {
                    if (rt.phase == CyclePhase::Blocked) {
                        rt.phase = CyclePhase::Allowed;
                        double allowDur = (double)rt.cfg.allowMs;
                        rt.phaseStartMs = (nowMs - rt.phaseTargetMs < allowDur) ? rt.phaseTargetMs : nowMs;
                        rt.phaseTargetMs = rt.phaseStartMs + allowDur;
                        actions.push_back({CyclePendingAction::SwitchAllow, rt.displayPath, rt.cfg.direction, rt.includeChildren});
                    } else {
                        rt.completedCycles++;
                        if (rt.cfg.runMode == CycleRunMode::ForCycles && rt.completedCycles >= rt.cfg.runValue) {
                            actions.push_back({CyclePendingAction::EndAllow, rt.displayPath, rt.cfg.direction, rt.includeChildren});
                            toErase.push_back(kv.first);
                            continue;
                        }
                        rt.phase = CyclePhase::Blocked;
                        double blockDur = (double)rt.cfg.blockMs;
                        rt.phaseStartMs = (nowMs - rt.phaseTargetMs < blockDur) ? rt.phaseTargetMs : nowMs;
                        rt.phaseTargetMs = rt.phaseStartMs + blockDur;
                        actions.push_back({CyclePendingAction::SwitchBlock, rt.displayPath, rt.cfg.direction, rt.includeChildren});
                    }
                }

                double rem = std::max(5.0, rt.phaseTargetMs - nowMs);
                if (rt.cfg.runMode == CycleRunMode::ForDuration) {
                    rem = std::min(rem, std::max(5.0, rt.totalEndMs - nowMs));
                }
                nextWakeMs = std::min(nextWakeMs, rem);
            }

            for (const auto& k : toErase) {
                g_cycles.erase(k);
            }
        }

        if (!actions.empty()) {
            {
                std::lock_guard<std::mutex> lk(g_cycleMtx);
                g_pendingCycleActions.insert(g_pendingCycleActions.end(),
                                             std::make_move_iterator(actions.begin()),
                                             std::make_move_iterator(actions.end()));
            }
            if (g_hWnd) PostMessageW(g_hWnd, WM_APP_CYCLE_APPLY, 0, 0);
        }

        {
            std::unique_lock<std::mutex> lk(g_cycleMtx);
            if (!g_cycleStopping.load() && !g_cycles.empty()) {
                int waitMs = std::clamp((int)std::floor(nextWakeMs), 5, 250);
                g_cycleCv.wait_for(lk, std::chrono::milliseconds(waitMs), []() { return g_cycleStopping.load(); });
            }
        }
    }
    return 0;
}

static void StartCycleEngine() {
    g_cycleStopping = false;
    if (!g_cycleThread) {
        g_cycleThread = CreateThread(nullptr, 0, CycleSchedulerThreadProc, nullptr, 0, nullptr);
    }
}

static void ShutdownCycleEngine() {
    StopAllCycles(false);
    g_cycleStopping = true;
    g_cycleCv.notify_all();
    if (g_cycleThread) {
        WaitForSingleObject(g_cycleThread, 3000);
        CloseHandle(g_cycleThread);
        g_cycleThread = nullptr;
    }
}

static void RestoreAllSessionBlocks() {
    StopAllCycles(false);
    std::lock_guard<std::mutex> lk(g_engineMtx);
    for (auto it = g_blocks.begin(); it != g_blocks.end();) {
        RemoveWfpFiltersLocked(it->second.sessionFilters);
        it = g_blocks.erase(it);
    }
    LogAudit(L"RESTORE ALL session blocks");
    g_statusBanner = L"All session blocks cleared.";
    TriggerPulse(PulseKind::AllowedGreen);
}

static std::unordered_map<std::wstring, std::pair<Direction, bool>> g_lastAutoBlocked;

static void SyncAutoBlockedApps() {
    std::vector<AutoBlockRuleCfg> rules;
    {
        std::lock_guard<std::mutex> lk(g_settingsMtx);
        rules = g_settings.autoBlockedApps;
    }
    std::unordered_map<std::wstring, std::pair<Direction, bool>> nextMap;
    for (const auto& r : rules) {
        if (r.appPath.empty()) continue;
        std::wstring low = ToLower(r.appPath);
        if (g_excludedExes.count(low)) continue;
        if (FileExistsW(r.appPath)) {
            nextMap[low] = {r.direction, r.includeChildProcesses};
        }
    }
    for (const auto& prev : g_lastAutoBlocked) {
        if (!nextMap.count(prev.first)) {
            ApplyAllow(prev.first);
        }
    }
    for (const auto& cur : nextMap) {
        auto itPrev = g_lastAutoBlocked.find(cur.first);
        if (itPrev == g_lastAutoBlocked.end() || itPrev->second != cur.second) {
            ApplyBlock(cur.first, cur.second.first, cur.second.second, false);
        }
    }
    g_lastAutoBlocked = std::move(nextMap);
}

static int CountBlockedApps() {
    std::lock_guard<std::mutex> lk(g_engineMtx);
    int c = 0;
    for (const auto& kv : g_blocks) {
        if (kv.second.blocked) ++c;
    }
    return c;
}

// ============================================================================
// App List Builder
// ============================================================================

static void RefreshAppList() {
    HWND fgWnd = GetForegroundWindow();
    if (fgWnd != g_hWnd) {
        if (!fgWnd || IsDesktopOrShellWindow(fgWnd)) {
            g_lastFgExe.clear();
        } else {
            DWORD fgPid = 0;
            GetWindowThreadProcessId(fgWnd, &fgPid);
            std::wstring q = fgPid ? ToLower(QueryProcessExePath(fgPid)) : L"";
            if (!q.empty()) g_lastFgExe = q;
            else g_lastFgExe.clear();
        }
    }
    std::wstring fgExe = g_lastFgExe;

    EnumWinCtx winCtx;
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&winCtx));
    auto netPids = GetPidsWithLiveNetwork();
    auto snap = CaptureProcessSnapshot();
    auto extFw = ScanExternalFirewallBlocks(false);

    Direction defDir;
    bool lockCritSetting = true;
    bool showNonActionable = false;
    {
        std::lock_guard<std::mutex> lk(g_settingsMtx);
        defDir = g_settings.defaultDirection;
        lockCritSetting = g_settings.lockSystemProcesses;
        showNonActionable = g_settings.showNonActionableInRunning;
    }

    std::unordered_map<std::wstring, AppEntry> mapEntries;
    for (const auto& p : snap) {
        std::wstring low = ToLower(p.exePath);
        if (low.empty()) continue;
        auto& e = mapEntries[low];
        if (e.exePath.empty()) {
            e.exePath = low;
            e.displayPath = p.exePath;
            e.name = GetFileNameOnly(p.exePath);
            e.primaryPid = p.pid;
            e.isSystemApp = IsSystemPath(low);
            e.isCriticalLocked = IsCriticalSystemExe(low);
            e.isVpnOrRemote = IsVpnOrRemoteExe(low);
            e.isExcluded = g_excludedExes.count(low) > 0;
            e.exeExists = FileExistsW(p.exePath);
            auto itExt = extFw.find(low);
            if (itExt != extFw.end() && itExt->second.dirMask != 0) {
                e.isExternalBlocked = true;
                e.externalBlockMask = itExt->second.dirMask;
                e.externalRuleName = itExt->second.ruleName;
            }
            auto itDir = g_pendingDirs.find(low);
            e.block.direction = (itDir != g_pendingDirs.end()) ? itDir->second : defDir;
        }
        e.pids.push_back(p.pid);
        if (low == fgExe) e.isForeground = true;
        if (winCtx.visiblePids.count(p.pid)) e.hasVisibleWindow = true;
        if (netPids.count(p.pid)) e.hasLiveNetwork = true;
    }

    {
        std::lock_guard<std::mutex> lk(g_engineMtx);
        for (const auto& kv : g_blocks) {
            auto& e = mapEntries[kv.first];
            if (e.exePath.empty()) {
                e.exePath = kv.first;
                e.displayPath = kv.first;
                e.name = GetFileNameOnly(kv.first);
                e.isSystemApp = IsSystemPath(kv.first);
                e.isCriticalLocked = IsCriticalSystemExe(kv.first);
                e.isVpnOrRemote = IsVpnOrRemoteExe(kv.first);
                e.isExcluded = g_excludedExes.count(kv.first) > 0;
                e.exeExists = FileExistsW(kv.first);
            }
            e.block = kv.second;
            for (const auto& chLow : kv.second.blockedChildPaths) {
                auto itCh = mapEntries.find(chLow);
                if (itCh != mapEntries.end() && !itCh->second.block.blocked) {
                    itCh->second.block.blocked = true;
                    itCh->second.block.direction = kv.second.direction;
                }
            }
        }
    }

    if (g_activeFilter == AppFilter::Blocked && g_blockedFirewallOnly) {
        for (const auto& kv : extFw) {
            if ((kv.second.dirMask & 1) == 0) continue;
            if (!mapEntries.count(kv.first)) {
                AppEntry e;
                e.exePath = kv.first;
                e.displayPath = kv.first;
                e.name = GetFileNameOnly(kv.first);
                e.isSystemApp = IsSystemPath(kv.first);
                e.isCriticalLocked = IsCriticalSystemExe(kv.first);
                e.isVpnOrRemote = IsVpnOrRemoteExe(kv.first);
                e.isExcluded = g_excludedExes.count(kv.first) > 0;
                e.isExternalBlocked = true;
                e.externalBlockMask = kv.second.dirMask;
                e.externalRuleName = kv.second.ruleName;
                mapEntries[kv.first] = std::move(e);
            }
        }
    }

    for (const auto& ex : g_excludedExes) {
        if (!mapEntries.count(ex)) {
            AppEntry e;
            e.exePath = ex;
            e.displayPath = ex;
            e.name = GetFileNameOnly(ex);
            e.isExcluded = true;
            e.isSystemApp = IsSystemPath(ex);
            e.exeExists = FileExistsW(ex);
            mapEntries[ex] = std::move(e);
        }
    }

    std::vector<AppEntry> filtered;
    filtered.reserve(mapEntries.size());

    for (auto& kv : mapEntries) {
        AppEntry& e = kv.second;
        bool isRunning = !e.pids.empty();
        bool isOpenOrNet = e.isForeground || e.hasVisibleWindow || e.hasLiveNetwork;

        if (g_activeFilter == AppFilter::Excluded) {
            if (!e.isExcluded) continue;
        } else if (g_activeFilter == AppFilter::Blocked) {
            if (e.isExcluded) continue;
            if (g_blockedFirewallOnly) {
                if (!e.isExternalBlocked) continue;
            } else {
                if (!e.block.blocked) continue;
            }
        } else {
            if (e.isExcluded) continue;
            bool isNonActionable = e.isExternalBlocked || (!e.block.blocked && e.isCriticalLocked && lockCritSetting);
            if (g_activeFilter == AppFilter::Allowed) {
                if (isNonActionable || e.block.blocked || !isOpenOrNet) continue;
            }
            if (g_activeFilter == AppFilter::UserApps) {
                if (e.isSystemApp) continue;
                if (!showNonActionable && isNonActionable) continue;
                if (g_userRunningOnly && !isOpenOrNet && !e.block.blocked) continue;
                if (!g_userRunningOnly && !isRunning && !e.block.blocked) continue;
            }
            if (g_activeFilter == AppFilter::SystemApps) {
                if (e.isExternalBlocked) continue;
                if (!e.isSystemApp || (!isRunning && !e.block.blocked)) continue;
            }
        }

        filtered.push_back(std::move(e));
    }

    std::sort(filtered.begin(), filtered.end(), [lockCritSetting](const AppEntry& a, const AppEntry& b) {
        if (a.isForeground != b.isForeground) return a.isForeground > b.isForeground;
        bool aBlk = a.block.blocked || a.isExternalBlocked;
        bool bBlk = b.block.blocked || b.isExternalBlocked;
        if (aBlk != bBlk) return aBlk > bBlk;
        bool aLocked = !aBlk && a.isCriticalLocked && lockCritSetting;
        bool bLocked = !bBlk && b.isCriticalLocked && lockCritSetting;
        if (aLocked != bLocked) return aLocked < bLocked;
        if (a.hasVisibleWindow != b.hasVisibleWindow) return a.hasVisibleWindow > b.hasVisibleWindow;
        return ToLower(a.name) < ToLower(b.name);
    });

    g_appList = std::move(filtered);
    if (g_appList.empty()) g_kbSelectedIdx = -1;
    else if (g_kbSelectedIdx >= (int)g_appList.size()) g_kbSelectedIdx = (int)g_appList.size() - 1;
}

// ============================================================================
// Icon Cache
// ============================================================================

static HICON GetAppIconCached(const std::wstring& exePath, UINT dpi) {
    std::wstring key = exePath + L"@" + std::to_wstring(dpi);
    auto it = g_iconCache.find(key);
    if (it != g_iconCache.end()) return it->second;

    int sz = DpiScale(20, dpi);
    HICON hIcon = nullptr;
    if (FileExistsW(exePath)) {
        PVOID oldRedir = nullptr;
        BOOL redirDisabled = Wow64DisableWow64FsRedirection(&oldRedir);
        PrivateExtractIconsW(exePath.c_str(), 0, sz, sz, &hIcon, nullptr, 1, 0);
        if (redirDisabled) Wow64RevertWow64FsRedirection(oldRedir);
    }
    if (!hIcon) {
        SHSTOCKICONINFO sii{};
        sii.cbSize = sizeof(sii);
        if (SUCCEEDED(SHGetStockIconInfo(SIID_APPLICATION, SHGSI_ICON | SHGSI_SMALLICON, &sii))) {
            hIcon = sii.hIcon;
        }
    }
    g_iconCache[key] = hIcon;
    return hIcon;
}

static void ClearIconCache() {
    for (auto& kv : g_iconCache) {
        if (kv.second) DestroyIcon(kv.second);
    }
    g_iconCache.clear();
}

// ============================================================================
// Capture Guard & Fullscreen Gate
// ============================================================================

static void ApplyCaptureExclusionToWindow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return;
    bool hideCap = true;
    { std::lock_guard<std::mutex> lk(g_settingsMtx); hideCap = g_settings.hideDuringCapture; }
    SetWindowDisplayAffinity(hwnd, hideCap ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE);
}

static void ApplyCaptureExclusionAll() {
    ApplyCaptureExclusionToWindow(g_hWnd);
    ApplyCaptureExclusionToWindow(g_hCycleWidget);
    ApplyCaptureExclusionToWindow(g_hCyclePopup);
}

static bool IsFullscreenActive() {
    QUERY_USER_NOTIFICATION_STATE quns{};
    if (SUCCEEDED(SHQueryUserNotificationState(&quns))) {
        if (quns == QUNS_BUSY || quns == QUNS_RUNNING_D3D_FULL_SCREEN || quns == QUNS_PRESENTATION_MODE) {
            return true;
        }
    }
    HWND fg = GetForegroundWindow();
    if (!fg || fg == g_hWnd) return false;
    HMONITOR mon = MonitorFromWindow(fg, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{sizeof(mi)};
    if (GetMonitorInfoW(mon, &mi)) {
        RECT rc{};
        if (GetWindowRect(fg, &rc)) {
            if (rc.left <= mi.rcMonitor.left && rc.top <= mi.rcMonitor.top &&
                rc.right >= mi.rcMonitor.right && rc.bottom >= mi.rcMonitor.bottom) {
                wchar_t cls[64]{};
                GetClassNameW(fg, cls, 64);
                if (wcscmp(cls, L"Progman") != 0 && wcscmp(cls, L"WorkerW") != 0) {
                    return true;
                }
            }
        }
    }
    return false;
}

// ============================================================================
// Hotkeys & Settings UI
// ============================================================================

static HotkeyBinding ParseHotkeyString(PCWSTR str) {
    HotkeyBinding hb;
    if (!str || !*str) return hb;
    hb.raw = str;
    std::wstringstream ss(str);
    std::wstring tok;
    while (std::getline(ss, tok, L'+')) {
        tok = ToLower(Trim(tok));
        if (tok == L"ctrl" || tok == L"control") hb.modifiers |= MOD_CONTROL;
        else if (tok == L"alt") hb.modifiers |= MOD_ALT;
        else if (tok == L"shift") hb.modifiers |= MOD_SHIFT;
        else if (tok.size() == 1 && tok[0] >= L'a' && tok[0] <= L'z') hb.vk = (UINT)(L'A' + (tok[0] - L'a'));
        else if (tok.size() == 1 && tok[0] >= L'0' && tok[0] <= L'9') hb.vk = (UINT)tok[0];
        else if (tok.size() >= 2 && tok[0] == L'f') {
            int fnum = _wtoi(tok.c_str() + 1);
            if (fnum >= 1 && fnum <= 24) hb.vk = VK_F1 + (fnum - 1);
        }
    }
    hb.modifiers |= MOD_NOREPEAT;
    return hb;
}

static void RegisterAllHotkeys(HWND hwnd) {
    for (int id = 1; id <= 3; ++id) UnregisterHotKey(hwnd, id);
    ModSettings s;
    {
        std::lock_guard<std::mutex> lk(g_settingsMtx);
        s = g_settings;
    }
    auto reg = [&](int id, const HotkeyBinding& hb, const wchar_t* label) {
        if (hb.vk == 0) return;
        if (!RegisterHotKey(hwnd, id, hb.modifiers, hb.vk)) {
            Wh_Log(L"Hotkey conflict: %s (%s) already in use.", label, hb.raw.c_str());
        }
    };
    reg(HK_TOGGLE_FG, s.hkToggleFg, L"toggleForegroundApp");
    reg(HK_EXPAND_PILL, s.hkExpandPill, L"expandCollapsePill");
    reg(HK_RESTORE_ALL, s.hkRestoreAll, L"restoreAll");
}

static void OpenWindhawkSettingsUI() {
    HWND whWnd = FindWindowW(nullptr, L"Windhawk");
    if (whWnd) {
        ShowWindow(whWnd, SW_RESTORE);
        SetForegroundWindow(whWnd);
    } else {
        WCHAR exePath[MAX_PATH]{};
        GetModuleFileNameW(nullptr, exePath, ARRAYSIZE(exePath));
        std::wstring p(exePath);
        size_t sl = p.find_last_of(L"\\/");
        std::wstring dir = (sl != std::wstring::npos) ? p.substr(0, sl) : L"";
        std::wstring uiExe = dir + L"\\windhawk.exe";
        if (!FileExistsW(uiExe)) {
            size_t sl2 = dir.find_last_of(L"\\/");
            if (sl2 != std::wstring::npos) uiExe = dir.substr(0, sl2) + L"\\windhawk.exe";
        }
        if (FileExistsW(uiExe)) {
            ShellExecuteW(nullptr, L"open", uiExe.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
        }
    }
    g_statusBanner = L"Opened Windhawk — select !Net -> Settings.";
    RequestRepaint();
}

static void LoadSettings() {
    ModSettings next;

    auto sDir = WindhawkUtils::StringSetting::make(L"General.defaultDirection");
    next.defaultDirection = ParseDirection(sDir.get());

    next.includeChildProcesses = Wh_GetIntSetting(L"General.includeChildProcesses") != 0;

    auto sEdge = WindhawkUtils::StringSetting::make(L"Pill.dockEdge");
    {
        std::wstring e = ToLower(sEdge.get());
        if (e == L"left") next.dockEdge = DockEdge::Left;
        else if (e == L"top") next.dockEdge = DockEdge::Top;
        else if (e == L"bottom") next.dockEdge = DockEdge::Bottom;
        else if (e == L"top_left") next.dockEdge = DockEdge::TopLeft;
        else if (e == L"bottom_left") next.dockEdge = DockEdge::BottomLeft;
        else if (e == L"bottom_right") next.dockEdge = DockEdge::BottomRight;
        else next.dockEdge = DockEdge::Right;
    }

    auto sInd = WindhawkUtils::StringSetting::make(L"Pill.pillIndicatorMode");
    next.pillIndicatorMode = (ToLower(sInd.get()) == L"pulse") ? PillIndicatorMode::Pulse : PillIndicatorMode::Always;

    next.hoverExpand = Wh_GetIntSetting(L"Pill.hoverExpand") != 0;
    next.expandHoverHitArea = Wh_GetIntSetting(L"Pill.expandHoverHitArea") != 0;
    int rawMargin = Wh_GetIntSetting(L"Pill.hoverHitMarginPx");
    next.hoverHitMarginPx = (rawMargin > 0) ? std::clamp(rawMargin, 4, 40) : 12;
    int rawOffset = Wh_GetIntSetting(L"Pill.panelEdgeOffsetPx");
    next.panelEdgeOffsetPx = (rawOffset >= 0) ? std::clamp(rawOffset, 0, 12) : 8;

    auto sFilter = WindhawkUtils::StringSetting::make(L"Pill.defaultFilter");
    {
        std::wstring f = ToLower(sFilter.get());
        if (f == L"blocked") next.defaultFilter = AppFilter::Blocked;
        else if (f == L"allowed") next.defaultFilter = AppFilter::Allowed;
        else if (f == L"system") next.defaultFilter = AppFilter::SystemApps;
        else if (f == L"excluded") next.defaultFilter = AppFilter::Excluded;
        else next.defaultFilter = AppFilter::UserApps;
    }

    next.showNonActionableInRunning = Wh_GetIntSetting(L"Pill.showNonActionableInRunning") != 0;
    next.showTooltips = Wh_GetIntSetting(L"Pill.showTooltips") != 0;
    next.followCursorMonitor = Wh_GetIntSetting(L"Pill.followCursorMonitor") != 0;
    next.rememberWindowPosition = Wh_GetIntSetting(L"Pill.rememberWindowPosition") != 0;

    auto sPreset = WindhawkUtils::StringSetting::make(L"Pill.windowSizePreset");
    {
        std::wstring p = ToLower(sPreset.get());
        if (p == L"large") next.windowSizePreset = SizePreset::Large;
        else if (p == L"extralarge") next.windowSizePreset = SizePreset::ExtraLarge;
        else next.windowSizePreset = SizePreset::Default;
    }

    auto sFs = WindhawkUtils::StringSetting::make(L"Behavior.fullscreenBehavior");
    {
        std::wstring v = ToLower(sFs.get());
        next.fullscreenBehavior = (v == L"allow") ? FullscreenBehavior::Allow : FullscreenBehavior::Hide;
    }

    next.hideDuringCapture = Wh_GetIntSetting(L"Behavior.hideDuringCapture") != 0;
    next.enableCycleScheduler = Wh_GetIntSetting(L"Behavior.enableCycleScheduler") != 0;
    next.showCycleMs = Wh_GetIntSetting(L"Behavior.showCycleMs") != 0;
    next.showCycleWidget = Wh_GetIntSetting(L"Behavior.showCycleWidget") != 0;
    auto sCycAct = WindhawkUtils::StringSetting::make(L"Behavior.cycleWidgetAction");
    next.cycleWidgetAction = (_wcsicmp(sCycAct.get(), L"navigate_pill") == 0)
        ? CycleWidgetAction::NavigatePill
        : CycleWidgetAction::SchedulerOnly;

    next.lockSystemProcesses = Wh_GetIntSetting(L"Safety.lockSystemProcesses") != 0;
    next.warnOnVpnOrRemote = Wh_GetIntSetting(L"Safety.warnOnVpnOrRemote") != 0;

    auto readHk = [&](PCWSTR key, PCWSTR defVal) {
        auto raw = WindhawkUtils::StringSetting::make(key);
        return ParseHotkeyString(*raw.get() ? raw.get() : defVal);
    };
    next.hkToggleFg   = readHk(L"Hotkeys.toggleForegroundApp", L"Ctrl+Alt+Shift+1");
    next.hkExpandPill = readHk(L"Hotkeys.expandCollapsePill",  L"Ctrl+Alt+Shift+M");
    next.hkRestoreAll = readHk(L"Hotkeys.restoreAll",          L"Ctrl+Alt+Shift+R");

    for (int i = 0; i < 64; ++i) {
        auto app = WindhawkUtils::StringSetting::make(L"autoBlockedApps[%d].appPath", i);
        if (!*app.get()) break;
        AutoBlockRuleCfg r;
        r.appPath = app.get();
        auto dir = WindhawkUtils::StringSetting::make(L"autoBlockedApps[%d].direction", i);
        r.direction = ParseDirection(dir.get());
        r.includeChildProcesses = Wh_GetIntSetting(L"autoBlockedApps[%d].includeChildProcesses", i) != 0;
        next.autoBlockedApps.push_back(std::move(r));
    }

    int settingEdgeInt = (int)next.dockEdge;
    if (g_lastSettingDockEdge == -1) {
        int ov = Wh_GetIntValue(L"dockEdgeOverride", -1);
        if (ov >= 0 && ov <= 6) next.dockEdge = (DockEdge)ov;
    } else if (settingEdgeInt != g_lastSettingDockEdge) {
        Wh_SetIntValue(L"dockEdgeOverride", settingEdgeInt);
    } else {
        int ov = Wh_GetIntValue(L"dockEdgeOverride", -1);
        if (ov >= 0 && ov <= 6) next.dockEdge = (DockEdge)ov;
    }
    g_lastSettingDockEdge = settingEdgeInt;
    {
        std::lock_guard<std::mutex> lk(g_settingsMtx);
        g_settings = std::move(next);
        g_activeFilter = g_settings.defaultFilter;
        if (g_activeFilter == AppFilter::SystemApps) {
            g_statusBanner = L"Experts only — if you don't know what you're doing, skip System.";
        }
    }
}

// ============================================================================
// Rounded Geometry, Clean Pill Frame, Crisp Rendering
// ============================================================================

static void AddRoundedRectPath(Gdiplus::GraphicsPath& path, Gdiplus::REAL x, Gdiplus::REAL y,
                               Gdiplus::REAL w, Gdiplus::REAL h, Gdiplus::REAL r) {
    Gdiplus::REAL d = std::min(r * 2.0f, std::min(w, h));
    if (d <= 0.5f) {
        path.AddRectangle(Gdiplus::RectF(x, y, w, h));
        return;
    }
    path.AddArc(x, y, d, d, 180.0f, 90.0f);
    path.AddArc(x + w - d, y, d, d, 270.0f, 90.0f);
    path.AddArc(x + w - d, y + h - d, d, d, 0.0f, 90.0f);
    path.AddArc(x, y + h - d, d, d, 90.0f, 90.0f);
    path.CloseFigure();
}

static void FillRoundedRect(Gdiplus::Graphics& g, const Gdiplus::Brush& br,
                            Gdiplus::REAL x, Gdiplus::REAL y, Gdiplus::REAL w, Gdiplus::REAL h, Gdiplus::REAL r) {
    Gdiplus::GraphicsPath path;
    AddRoundedRectPath(path, x, y, w, h, r);
    g.FillPath(&br, &path);
}

static void DrawRoundedRect(Gdiplus::Graphics& g, const Gdiplus::Pen& pen,
                            Gdiplus::REAL x, Gdiplus::REAL y, Gdiplus::REAL w, Gdiplus::REAL h, Gdiplus::REAL r) {
    Gdiplus::GraphicsPath path;
    AddRoundedRectPath(path, x, y, w, h, r);
    g.DrawPath(&pen, &path);
}

static void SyncHoverSensorTimer(HWND hwnd) {
    if (!hwnd) return;
    bool needSensor = false;
    {
        std::lock_guard<std::mutex> lk(g_settingsMtx);
        needSensor = (g_uiState == UiState::Line) && g_settings.hoverExpand &&
                     g_settings.expandHoverHitArea && !g_hiddenByFullscreen;
    }
    if (needSensor) SetTimer(hwnd, TIMER_HOVER_SENSOR, 75, nullptr);
    else KillTimer(hwnd, TIMER_HOVER_SENSOR);
}

static void SetWindowFrameMode(HWND hwnd, bool roundedPanel) {
    if (roundedPanel && HasAnyActiveCycle()) SetTimer(hwnd, TIMER_CYCLE_UI, 250, nullptr);
    else if (!roundedPanel) KillTimer(hwnd, TIMER_CYCLE_UI);
    if (!hwnd) return;
    bool wantLayeredLine = !roundedPanel;
    LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    bool hasLayered = (ex & WS_EX_LAYERED) != 0;
    if (wantLayeredLine != hasLayered) {
        SetWindowLongPtrW(hwnd, GWL_EXSTYLE, wantLayeredLine ? (ex | WS_EX_LAYERED) : (ex & ~WS_EX_LAYERED));
    }
    if (g_lastRoundedFrame == roundedPanel && !wantLayeredLine) return;
    g_lastRoundedFrame = roundedPanel;
    DWORD pref = roundedPanel ? DWMWCP_ROUND : DWMWCP_DONOTROUND;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &pref, sizeof(pref));
    MARGINS m = roundedPanel ? MARGINS{-1, -1, -1, -1} : MARGINS{0, 0, 0, 0};
    DwmExtendFrameIntoClientArea(hwnd, &m);
}

static SIZE GetExpandedLogicalSize() {
    ModSettings s;
    {
        std::lock_guard<std::mutex> lk(g_settingsMtx);
        s = g_settings;
    }
    switch (s.windowSizePreset) {
        case SizePreset::Large:      return {680, 680};
        case SizePreset::ExtraLarge: return {840, 840};
        default:                     return {436, 480};
    }
}

static HMONITOR GetTargetMonitor(bool followCursor) {
    if (followCursor && !g_drag.dragging) {
        POINT cur{};
        if (GetCursorPos(&cur)) return MonitorFromPoint(cur, MONITOR_DEFAULTTONEAREST);
    }
    return g_hWnd ? MonitorFromWindow(g_hWnd, MONITOR_DEFAULTTONEAREST)
                  : MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY);
}

static void ComputeTargetRect(double progress, RECT& outRc) {
    UINT dpi = g_hWnd ? GetDpiForWindow(g_hWnd) : 96;
    ModSettings s;
    {
        std::lock_guard<std::mutex> lk(g_settingsMtx);
        s = g_settings;
    }

    HMONITOR mon = GetTargetMonitor(s.followCursorMonitor);
    MONITORINFO mi{sizeof(mi)};
    GetMonitorInfoW(mon, &mi);
    RECT wa = mi.rcWork;

    int lineThick = DpiScale(6, dpi);
    int lineLen   = DpiScale(64, dpi);
    int cornerLen = DpiScale(36, dpi);
    SIZE expLog   = GetExpandedLogicalSize();
    int expW      = DpiScale(expLog.cx, dpi);
    int expH      = DpiScale(expLog.cy, dpi);

    int curW = IsCornerDock(s.dockEdge)
                   ? (int)std::round(cornerLen + (expW - cornerLen) * progress)
                   : ((s.dockEdge == DockEdge::Left || s.dockEdge == DockEdge::Right)
                          ? (int)std::round(lineThick + (expW - lineThick) * progress)
                          : (int)std::round(lineLen + (expW - lineLen) * progress));
    int curH = IsCornerDock(s.dockEdge)
                   ? (int)std::round(cornerLen + (expH - cornerLen) * progress)
                   : ((s.dockEdge == DockEdge::Top || s.dockEdge == DockEdge::Bottom)
                          ? (int)std::round(lineThick + (expH - lineThick) * progress)
                          : (int)std::round(lineLen + (expH - lineLen) * progress));

    int panelOffsetPx = DpiScale(s.panelEdgeOffsetPx, dpi);
    int edgeInset = (int)std::round((double)panelOffsetPx * progress);

    if (s.rememberWindowPosition && !g_posLoaded) {
        g_posX = Wh_GetIntValue(L"posX", wa.left + (wa.right - wa.left - expW) / 2);
        g_posY = Wh_GetIntValue(L"posY", wa.top + DpiScale(240, dpi));
        g_posLoaded = true;
    }
    int savedY = s.rememberWindowPosition ? g_posY : (wa.top + DpiScale(240, dpi));
    int savedX = s.rememberWindowPosition ? g_posX : (wa.left + (wa.right - wa.left - expW) / 2);

    switch (s.dockEdge) {
        case DockEdge::Left:
            outRc.left = wa.left + edgeInset;
            outRc.right = outRc.left + curW;
            outRc.top = std::clamp(savedY, (int)wa.top, (int)wa.bottom - curH);
            outRc.bottom = outRc.top + curH;
            break;
        case DockEdge::Top:
            outRc.top = wa.top + edgeInset;
            outRc.bottom = outRc.top + curH;
            outRc.left = std::clamp(savedX, (int)wa.left, (int)wa.right - curW);
            outRc.right = outRc.left + curW;
            break;
        case DockEdge::Bottom:
            outRc.bottom = wa.bottom - edgeInset;
            outRc.top = outRc.bottom - curH;
            outRc.left = std::clamp(savedX, (int)wa.left, (int)wa.right - curW);
            outRc.right = outRc.left + curW;
            break;
        case DockEdge::TopLeft:
            outRc.left = wa.left + edgeInset;
            outRc.top = wa.top + edgeInset;
            outRc.right = outRc.left + curW;
            outRc.bottom = outRc.top + curH;
            break;
        case DockEdge::BottomLeft:
            outRc.left = wa.left + edgeInset;
            outRc.bottom = wa.bottom - edgeInset;
            outRc.right = outRc.left + curW;
            outRc.top = outRc.bottom - curH;
            break;
        case DockEdge::BottomRight:
            outRc.right = wa.right - edgeInset;
            outRc.bottom = wa.bottom - edgeInset;
            outRc.left = outRc.right - curW;
            outRc.top = outRc.bottom - curH;
            break;
        default:
            outRc.right = wa.right - edgeInset;
            outRc.left = outRc.right - curW;
            outRc.top = std::clamp(savedY, (int)wa.top, (int)wa.bottom - curH);
            outRc.bottom = outRc.top + curH;
            break;
    }
}

static void UpdateWindowGeometry(bool immediate) {
    if (!g_hWnd) return;
    double target = (g_uiState == UiState::Line) ? 0.0 : 1.0;
    if (immediate) {
        g_expandClock.currentVal = target;
        g_expandClock.active = false;
    } else {
        g_expandClock.Start(target, (target > 0.5) ? 240.0 : 200.0);
        SetTimer(g_hWnd, TIMER_ANIM_VSYNC, 16, nullptr);
    }
    SetWindowFrameMode(g_hWnd, (g_uiState == UiState::Panel) || g_expandClock.active);
    RECT rc{};
    ComputeTargetRect(g_expandClock.currentVal, rc);
    SetWindowPos(g_hWnd, HWND_TOPMOST, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
                 SWP_NOACTIVATE);
    SyncHoverSensorTimer(g_hWnd);
    if (g_hWnd) {
        if (g_uiState == UiState::Panel && !g_pinned) SetTimer(g_hWnd, TIMER_OUTSIDE_CLICK, 50, nullptr);
        else KillTimer(g_hWnd, TIMER_OUTSIDE_CLICK);
    }
    SyncCycleWidgetWindow(true);
    RequestRepaint();
}

static void RequestRepaint(const RECT* dirty) {
    if (g_hWnd) InvalidateRect(g_hWnd, dirty, FALSE);
}

static BYTE LerpByte(BYTE a, BYTE b, double t) {
    return (BYTE)std::clamp((int)std::round(a + (b - a) * t), 0, 255);
}

static void PaintFloatingWindow(HWND hwnd) {
    PAINTSTRUCT ps{};
    HDC hdcScreen = BeginPaint(hwnd, &ps);
    RECT rcClient{};
    GetClientRect(hwnd, &rcClient);
    int w = rcClient.right - rcClient.left;
    int h = rcClient.bottom - rcClient.top;
    if (w <= 0 || h <= 0) {
        EndPaint(hwnd, &ps);
        return;
    }

    UINT dpi = GetDpiForWindow(hwnd);
    HDC memDC = CreateCompatibleDC(hdcScreen);
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP hBmp = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    HGDIOBJ oldBmp = SelectObject(memDC, hBmp);

    if (bits) memset(bits, 0, (size_t)w * (size_t)h * 4);

    {
        Gdiplus::Graphics g(memDC);
        g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
        g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
        g.SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);

        int blockedCount = CountBlockedApps();

        // Collapsed Line Pill: drawn only when settled in Line state; Panel animates smoothly with scrim fade
        if (g_uiState == UiState::Line && !g_expandClock.active && g_expandClock.currentVal < 0.05) {
            PillIndicatorMode indMode;
            {
                std::lock_guard<std::mutex> lk(g_settingsMtx);
                indMode = g_settings.pillIndicatorMode;
            }

            BYTE nr = 72, ng = 74, nb = 82;
            BYTE tr = nr, tg = ng, tb = nb;
            double k = 0.0;

            if (indMode == PillIndicatorMode::Always) {
                std::wstring fgLow = g_lastFgExe;
                if (!fgLow.empty() && !g_excludedExes.count(fgLow)) {
                    bool fgBlocked = false;
                    {
                        std::lock_guard<std::mutex> lk(g_engineMtx);
                        auto it = g_blocks.find(fgLow);
                        fgBlocked = (it != g_blocks.end() && it->second.blocked);
                    }
                    if (!fgBlocked) {
                        std::lock_guard<std::mutex> lk(g_extFwMtx);
                        auto itExt = g_extFwBlocks.find(fgLow);
                        fgBlocked = (itExt != g_extFwBlocks.end() && itExt->second.dirMask != 0);
                    }
                    if (fgBlocked) { tr = 225; tg = 58; tb = 58; k = 1.0; }
                    else           { tr = 34;  tg = 175; tb = 85; k = 1.0; }
                }
            } else {
                k = g_pillPulse.active ? g_pillPulse.intensity : 0.0;
                if (g_pillPulse.kind == PulseKind::AllowedGreen)      { tr = 34;  tg = 175; tb = 85; }
                else if (g_pillPulse.kind == PulseKind::BlockedRed)   { tr = 225; tg = 58;  tb = 58; }
            }

            Gdiplus::Color pillCol(255, LerpByte(nr, tr, k), LerpByte(ng, tg, k), LerpByte(nb, tb, k));
            Gdiplus::SolidBrush br(pillCol);
            DockEdge dEdge;
            { std::lock_guard<std::mutex> lk(g_settingsMtx); dEdge = g_settings.dockEdge; }
            if (IsCornerDock(dEdge)) {
                Gdiplus::REAL t = (Gdiplus::REAL)std::min(DpiScale(6, dpi), std::min(w, h));
                Gdiplus::REAL r = t * 0.5f;
                if (dEdge == DockEdge::TopLeft) {
                    FillRoundedRect(g, br, 0.0f, 0.0f, (Gdiplus::REAL)w, t, r);
                    FillRoundedRect(g, br, 0.0f, 0.0f, t, (Gdiplus::REAL)h, r);
                } else if (dEdge == DockEdge::BottomLeft) {
                    FillRoundedRect(g, br, 0.0f, (Gdiplus::REAL)h - t, (Gdiplus::REAL)w, t, r);
                    FillRoundedRect(g, br, 0.0f, 0.0f, t, (Gdiplus::REAL)h, r);
                } else {
                    FillRoundedRect(g, br, 0.0f, (Gdiplus::REAL)h - t, (Gdiplus::REAL)w, t, r);
                    FillRoundedRect(g, br, (Gdiplus::REAL)w - t, 0.0f, t, (Gdiplus::REAL)h, r);
                }
            } else {
                Gdiplus::REAL capR = std::min((Gdiplus::REAL)w, (Gdiplus::REAL)h) * 0.5f;
                FillRoundedRect(g, br, 0.0f, 0.0f, (Gdiplus::REAL)w, (Gdiplus::REAL)h, capR);
                if (HasAnyActiveCycle()) {
                    Gdiplus::SolidBrush dotBr(Gdiplus::Color(255, 255, 224, 160));
                    int d = std::max(3, DpiScale(4, dpi));
                    g.FillEllipse(&dotBr, (Gdiplus::REAL)((w - d) / 2.0f), (Gdiplus::REAL)((h - d) / 2.0f), (Gdiplus::REAL)d, (Gdiplus::REAL)d);
                }
            }
        } else {
            Gdiplus::SolidBrush bgBrush(Gdiplus::Color(255, 0, 0, 0));
            g.FillRectangle(&bgBrush, 0, 0, w, h);

            Gdiplus::Pen topEdge(Gdiplus::Color(48, 255, 255, 255), 1.0f);
            g.DrawLine(&topEdge, 0, 0, w, 0);

            g_hitRects.clear();
            int pad = DpiScale(14, dpi);
            int y = pad;

            int hdrH = DpiScale(30, dpi);
            g_hitRects.push_back({{0, 0, w - DpiScale(120, dpi), hdrH + pad}, 1, -1, L"", HitRect::HeaderDrag});

            Gdiplus::Font titleFont(L"Segoe UI", (Gdiplus::REAL)DpiScale(15, dpi), Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
            Gdiplus::Font uiFont(L"Segoe UI", (Gdiplus::REAL)DpiScale(13, dpi), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
            Gdiplus::Font smallFont(L"Segoe UI", (Gdiplus::REAL)DpiScale(11, dpi), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
            Gdiplus::Font iconFont(L"Segoe MDL2 Assets", (Gdiplus::REAL)DpiScale(13, dpi), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

            Gdiplus::SolidBrush whiteBr(Gdiplus::Color(255, 245, 245, 247));
            Gdiplus::SolidBrush mutedBr(Gdiplus::Color(255, 160, 166, 178));
            Gdiplus::SolidBrush terracottaBr(Gdiplus::Color(255, 201, 100, 66));
            Gdiplus::SolidBrush tanBr(Gdiplus::Color(255, 210, 180, 140));
            Gdiplus::SolidBrush greenBr(Gdiplus::Color(255, 34, 175, 85));
            Gdiplus::SolidBrush redBr(Gdiplus::Color(255, 225, 58, 58));
            Gdiplus::Pen subtleBorder(Gdiplus::Color(36, 255, 255, 255), 1.0f);
            Gdiplus::Pen activeDirBorder(Gdiplus::Color(220, 225, 58, 58), 1.0f);

            Gdiplus::StringFormat sfLeft;
            sfLeft.SetAlignment(Gdiplus::StringAlignmentNear);
            sfLeft.SetLineAlignment(Gdiplus::StringAlignmentCenter);
            sfLeft.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
            sfLeft.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);

            Gdiplus::StringFormat sfCenter;
            sfCenter.SetAlignment(Gdiplus::StringAlignmentCenter);
            sfCenter.SetLineAlignment(Gdiplus::StringAlignmentCenter);
            sfCenter.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);

            g.FillEllipse(blockedCount > 0 ? &redBr : &greenBr,
                          (Gdiplus::REAL)pad, (Gdiplus::REAL)(y + DpiScale(11, dpi)),
                          (Gdiplus::REAL)DpiScale(8, dpi), (Gdiplus::REAL)DpiScale(8, dpi));
            Gdiplus::RectF titleRf((Gdiplus::REAL)(pad + DpiScale(14, dpi)), (Gdiplus::REAL)y,
                                   (Gdiplus::REAL)DpiScale(200, dpi), (Gdiplus::REAL)hdrH);
            wchar_t hdrTxt[64]{};
            swprintf_s(hdrTxt, L"!Net · %d blocked", blockedCount);
            g.DrawString(hdrTxt, -1, &titleFont, titleRf, &sfLeft, &whiteBr);

            int btnSz = DpiScale(28, dpi);
            int bx = w - pad - btnSz;
            Gdiplus::REAL btnRad = (Gdiplus::REAL)DpiScale(6, dpi);
            auto drawIconBtn = [&](int id, HitRect::Kind kind, const wchar_t* glyph, bool active, const wchar_t* tip) {
                RECT r{bx, y + DpiScale(1, dpi), bx + btnSz, y + DpiScale(1, dpi) + btnSz};
                Gdiplus::SolidBrush hb(active ? Gdiplus::Color(255, 201, 100, 66)
                                              : (g_hoverHitId == id ? Gdiplus::Color(255, 42, 42, 46) : Gdiplus::Color(255, 20, 20, 24)));
                FillRoundedRect(g, hb, (Gdiplus::REAL)r.left, (Gdiplus::REAL)r.top, (Gdiplus::REAL)btnSz, (Gdiplus::REAL)btnSz, btnRad);
                DrawRoundedRect(g, subtleBorder, (Gdiplus::REAL)r.left, (Gdiplus::REAL)r.top, (Gdiplus::REAL)btnSz, (Gdiplus::REAL)btnSz, btnRad);
                Gdiplus::RectF rf((Gdiplus::REAL)r.left, (Gdiplus::REAL)r.top, (Gdiplus::REAL)btnSz, (Gdiplus::REAL)btnSz);
                g.DrawString(glyph, -1, &iconFont, rf, &sfCenter, &whiteBr);
                g_hitRects.push_back({r, id, -1, tip, kind});
                bx -= (btnSz + DpiScale(6, dpi));
            };
            drawIconBtn(10, HitRect::SettingsBtn,   L"\xE713", false, L"Open Windhawk Settings for !Net");
            drawIconBtn(11, HitRect::RestoreAllBtn, L"\xE72C", false, L"Restore all — unblock all session apps");
            drawIconBtn(12, HitRect::PinBtn,        L"\xE718", g_pinned, g_pinned ? L"Pinned open — click to auto-hide" : L"Pin open — keep panel visible");

            y += hdrH + DpiScale(10, dpi);

            struct FilterItem { AppFilter f; const wchar_t* label; int w; int gapAfter; const wchar_t* tip; };
            const wchar_t* userLbl = g_userRunningOnly ? L"Running" : L"User";
            const wchar_t* blkLbl  = g_blockedFirewallOnly ? L"Blocked · FW" : L"Blocked";
            FilterItem chips[] = {
                {AppFilter::Allowed,    L"Allowed",  58, 6,  L"Show apps currently allowed online"},
                {AppFilter::Blocked,    blkLbl,      82, 10, L"Blocked apps (click again to switch between !Net and Windows Firewall)"},
                {AppFilter::UserApps,   userLbl,     78, 6,  L"User apps (click again to toggle Running vs All)"},
                {AppFilter::SystemApps, L"System",   56, 6,  L"Show Windows system processes"},
                {AppFilter::Excluded,   L"Excluded", 64, 0,  L"Apps excluded from !Net"}
            };
            int fx = pad;
            int chipH = DpiScale(26, dpi);
            Gdiplus::REAL chipRad = (Gdiplus::REAL)DpiScale(6, dpi);
            for (int i = 0; i < 5; ++i) {
                int cw = DpiScale(chips[i].w, dpi);
                RECT cr{fx, y, fx + cw, y + chipH};
                bool sel = (g_activeFilter == chips[i].f);
                bool hov = (g_hoverHitId == 20 + i);
                Gdiplus::SolidBrush cb(sel ? Gdiplus::Color(255, 201, 100, 66)
                                           : (hov ? Gdiplus::Color(255, 42, 42, 46) : Gdiplus::Color(255, 22, 22, 26)));
                FillRoundedRect(g, cb, (Gdiplus::REAL)cr.left, (Gdiplus::REAL)cr.top, (Gdiplus::REAL)cw, (Gdiplus::REAL)chipH, chipRad);
                DrawRoundedRect(g, subtleBorder, (Gdiplus::REAL)cr.left, (Gdiplus::REAL)cr.top, (Gdiplus::REAL)cw, (Gdiplus::REAL)chipH, chipRad);

                Gdiplus::RectF rf((Gdiplus::REAL)cr.left, (Gdiplus::REAL)cr.top, (Gdiplus::REAL)cw, (Gdiplus::REAL)chipH);
                g.DrawString(chips[i].label, -1, &smallFont, rf, &sfCenter, sel ? &whiteBr : &mutedBr);
                g_hitRects.push_back({cr, 20 + i, (int)chips[i].f, chips[i].tip, HitRect::FilterChip});
                fx += cw + DpiScale(chips[i].gapAfter, dpi);
            }
            y += chipH + DpiScale(10, dpi);

            if (!g_statusBanner.empty() || g_needsElevation) {
                int banH = DpiScale(26, dpi);
                if (g_needsElevation) {
                    int btnW = DpiScale(104, dpi);
                    RECT elRc{w - pad - btnW, y, w - pad, y + banH};
                    FillRoundedRect(g, terracottaBr, (Gdiplus::REAL)elRc.left, (Gdiplus::REAL)elRc.top, (Gdiplus::REAL)btnW, (Gdiplus::REAL)banH, chipRad);
                    Gdiplus::RectF elRf((Gdiplus::REAL)elRc.left, (Gdiplus::REAL)elRc.top, (Gdiplus::REAL)btnW, (Gdiplus::REAL)banH);
                    g.DrawString(L"Run as Admin", -1, &smallFont, elRf, &sfCenter, &whiteBr);
                    g_hitRects.push_back({elRc, 40, -1, L"Restart !Net as Administrator to enable blocking", HitRect::ElevateBtn});
                }
                Gdiplus::RectF stRf((Gdiplus::REAL)pad, (Gdiplus::REAL)y,
                                    (Gdiplus::REAL)(w - 2 * pad - (g_needsElevation ? DpiScale(112, dpi) : 0)),
                                    (Gdiplus::REAL)banH);
                g.DrawString(g_statusBanner.c_str(), -1, &smallFont, stRf, &sfLeft, &terracottaBr);
                y += banH + DpiScale(6, dpi);
            }

            int listTop = y;
            int listBottom = h - pad;
            int listH = std::max(1, listBottom - listTop);
            int rowStride = DpiScale(54, dpi);
            g_lastListH = listH;
            g_lastRowStride = rowStride;
            int rowInnerH = DpiScale(48, dpi);
            int totalH = (int)g_appList.size() * rowStride;
            g_maxScrollPx = std::max(0, totalH - listH);
            if (g_scrollClock.toVal > g_maxScrollPx) g_scrollClock.Start((double)g_maxScrollPx, 60.0);
            int scrollPx = (int)std::round(std::clamp(g_scrollClock.currentVal, 0.0, (double)g_maxScrollPx));

            g.SetClip(Gdiplus::Rect(pad, listTop, w - 2 * pad, listH));
            HRGN gdiClip = CreateRectRgn(pad, listTop, w - pad, listBottom);
            SelectClipRgn(memDC, gdiClip);

            Gdiplus::REAL rowRad = (Gdiplus::REAL)DpiScale(8, dpi);
            Gdiplus::REAL ctlRad = (Gdiplus::REAL)DpiScale(6, dpi);

            for (int idx = 0; idx < (int)g_appList.size(); ++idx) {
                int ry = listTop + idx * rowStride - scrollPx;
                if (ry + rowInnerH <= listTop || ry >= listBottom) continue;

                const AppEntry& app = g_appList[idx];
                RECT rowRc{pad, ry, w - pad, ry + rowInnerH};
                Gdiplus::REAL rw = (Gdiplus::REAL)(rowRc.right - rowRc.left);
                Gdiplus::REAL rh = (Gdiplus::REAL)(rowRc.bottom - rowRc.top);

                bool isKbSel = (idx == g_kbSelectedIdx);
                Gdiplus::SolidBrush rowBg(isKbSel
                    ? (app.isForeground ? Gdiplus::Color(255, 48, 38, 28) : Gdiplus::Color(255, 32, 32, 38))
                    : (app.isForeground ? Gdiplus::Color(255, 34, 28, 22) : Gdiplus::Color(255, 18, 18, 21)));
                FillRoundedRect(g, rowBg, (Gdiplus::REAL)rowRc.left, (Gdiplus::REAL)rowRc.top, rw, rh, rowRad);
                DrawRoundedRect(g, subtleBorder, (Gdiplus::REAL)rowRc.left, (Gdiplus::REAL)rowRc.top, rw, rh, rowRad);
                if (rowRc.top >= listTop && rowRc.bottom <= listBottom) {
                    g_hitRects.push_back({rowRc, 2000 + idx, idx, L"Double-click or right-click for Cycle Mode", HitRect::RowBody});
                }

                HICON hIcon = GetAppIconCached(app.displayPath, dpi);
                if (hIcon) {
                    DrawIconEx(memDC, rowRc.left + DpiScale(10, dpi), rowRc.top + DpiScale(14, dpi),
                               hIcon, DpiScale(20, dpi), DpiScale(20, dpi), 0, nullptr, DI_NORMAL);
                }

                int textX = rowRc.left + DpiScale(38, dpi);
                bool lockCrit = false;
                {
                    std::lock_guard<std::mutex> lk(g_settingsMtx);
                    lockCrit = g_settings.lockSystemProcesses;
                }
                bool critProtected = !app.block.blocked && !app.isExternalBlocked && app.isCriticalLocked && lockCrit;
                int rightCtrlW = (app.isExcluded || critProtected) ? DpiScale(90, dpi)
                               : (app.isExternalBlocked ? DpiScale(142, dpi) : DpiScale(182, dpi));
                int textW = std::max<int>(40, (int)rw - DpiScale(38, dpi) - rightCtrlW);

                CycleStatusSnapshot cyc;
                bool hasCyc = !app.isExcluded && !app.isExternalBlocked && GetCycleSnapshot(app.exePath, cyc);
                int nameX = textX;
                int nameW = textW;
                if (hasCyc) {
                    int icSz = DpiScale(15, dpi);
                    Gdiplus::RectF icRf((Gdiplus::REAL)nameX, (Gdiplus::REAL)(rowRc.top + DpiScale(7, dpi)),
                                        (Gdiplus::REAL)icSz, (Gdiplus::REAL)DpiScale(16, dpi));
                    g.DrawString(L"\xE916", -1, &iconFont, icRf, &sfCenter, &terracottaBr);
                    nameX += icSz + DpiScale(3, dpi);
                    nameW = std::max(20, nameW - icSz - DpiScale(3, dpi));
                }
                Gdiplus::RectF nameRf((Gdiplus::REAL)nameX, (Gdiplus::REAL)(rowRc.top + DpiScale(6, dpi)),
                                      (Gdiplus::REAL)nameW, (Gdiplus::REAL)DpiScale(18, dpi));
                g.DrawString(app.name.c_str(), -1, &uiFont, nameRf, &sfLeft, &whiteBr);

                std::wstring sub;
                const Gdiplus::SolidBrush* subBrush = app.isForeground ? &tanBr : &mutedBr;
                if (hasCyc) {
                    bool isCycPaused = cyc.pausedByFocus || cyc.pausedByUser;
                    if (isCycPaused) {
                        sub = L"Paused";
                    } else {
                        sub = std::wstring(cyc.phase == CyclePhase::Blocked ? L"Blocked · " : L"Allowed · ")
                            + FormatDurationCompact(cyc.phaseRemainingMs) + L" left";
                    }
                    if (cyc.runMode == CycleRunMode::ForCycles) {
                        sub += L" · Cycle " + std::to_wstring(cyc.completedCycles + 1) + L"/" + std::to_wstring(cyc.targetCycles);
                    } else if (cyc.runMode == CycleRunMode::ForDuration) {
                        sub += L" · " + FormatDurationCompact(cyc.totalRemainingMs) + L" left";
                    }
                    subBrush = isCycPaused ? &mutedBr : (cyc.phase == CyclePhase::Blocked ? &redBr : &greenBr);
                } else {
                    sub = app.isExcluded ? L"Excluded" : (app.isForeground ? L"Active now" : (app.hasVisibleWindow ? L"Running" : L"Background"));
                    if (app.isExternalBlocked) {
                        const wchar_t* dirTxt = (app.externalBlockMask == 1) ? L"Outbound" : ((app.externalBlockMask == 2) ? L"Inbound" : L"Offline");
                        sub = !app.externalRuleName.empty()
                            ? (std::wstring(L"Rule: ") + app.externalRuleName + L" (" + dirTxt + L")")
                            : (std::wstring(L"Blocked by Windows Firewall (") + dirTxt + L")");
                    } else if (critProtected) sub += L" · Protected";
                    else if (app.isVpnOrRemote) sub += L" · VPN / Remote";
                }
                Gdiplus::RectF subRf((Gdiplus::REAL)textX, (Gdiplus::REAL)(rowRc.top + DpiScale(24, dpi)),
                                     (Gdiplus::REAL)textW, (Gdiplus::REAL)DpiScale(16, dpi));
                g.DrawString(sub.c_str(), -1, &smallFont, subRf, &sfLeft, subBrush);

                int ctlH = DpiScale(26, dpi);
                int ctlY = rowRc.top + (rowInnerH - ctlH) / 2;

                if (app.isExcluded) {
                    int resW = DpiScale(72, dpi);
                    RECT resRc{rowRc.right - resW - DpiScale(10, dpi), ctlY, rowRc.right - DpiScale(10, dpi), ctlY + ctlH};
                    Gdiplus::SolidBrush resBg(Gdiplus::Color(255, 42, 42, 48));
                    FillRoundedRect(g, resBg, (Gdiplus::REAL)resRc.left, (Gdiplus::REAL)resRc.top, (Gdiplus::REAL)resW, (Gdiplus::REAL)ctlH, ctlRad);
                    DrawRoundedRect(g, subtleBorder, (Gdiplus::REAL)resRc.left, (Gdiplus::REAL)resRc.top, (Gdiplus::REAL)resW, (Gdiplus::REAL)ctlH, ctlRad);
                    Gdiplus::RectF resRf((Gdiplus::REAL)resRc.left, (Gdiplus::REAL)resRc.top, (Gdiplus::REAL)resW, (Gdiplus::REAL)ctlH);
                    g.DrawString(L"Restore", -1, &smallFont, resRf, &sfCenter, &whiteBr);
                    if (resRc.top >= listTop && resRc.bottom <= listBottom) {
                        g_hitRects.push_back({resRc, 1000 + idx * 4 + 2, idx, L"Remove from Excluded list", HitRect::RowExclude});
                    }
                } else if (app.isExternalBlocked) {
                    int lockW = DpiScale(124, dpi);
                    RECT lockRc{rowRc.right - lockW - DpiScale(10, dpi), ctlY, rowRc.right - DpiScale(10, dpi), ctlY + ctlH};
                    Gdiplus::SolidBrush lockBg(Gdiplus::Color(255, 32, 26, 26));
                    Gdiplus::Pen lockBorder(Gdiplus::Color(110, 201, 100, 66), 1.0f);
                    FillRoundedRect(g, lockBg, (Gdiplus::REAL)lockRc.left, (Gdiplus::REAL)lockRc.top, (Gdiplus::REAL)lockW, (Gdiplus::REAL)ctlH, ctlRad);
                    DrawRoundedRect(g, lockBorder, (Gdiplus::REAL)lockRc.left, (Gdiplus::REAL)lockRc.top, (Gdiplus::REAL)lockW, (Gdiplus::REAL)ctlH, ctlRad);
                    Gdiplus::RectF lockRf((Gdiplus::REAL)lockRc.left, (Gdiplus::REAL)lockRc.top, (Gdiplus::REAL)lockW, (Gdiplus::REAL)ctlH);
                    g.DrawString(L"Blocked · Firewall", -1, &smallFont, lockRf, &sfCenter, &tanBr);
                    if (lockRc.top >= listTop && lockRc.bottom <= listBottom) {
                        g_hitRects.push_back({lockRc, 1000 + idx * 4 + 3, idx, L"!Net can't manage this app — edit its rule in Windows Defender Firewall.", HitRect::RowExternalLocked});
                    }
                } else if (critProtected) {
                    int togW = DpiScale(64, dpi);
                    RECT togRc{rowRc.right - togW - DpiScale(10, dpi), ctlY,
                               rowRc.right - DpiScale(10, dpi), ctlY + ctlH};
                    Gdiplus::SolidBrush togBg(Gdiplus::Color(255, 38, 38, 44));
                    FillRoundedRect(g, togBg, (Gdiplus::REAL)togRc.left, (Gdiplus::REAL)togRc.top, (Gdiplus::REAL)togW, (Gdiplus::REAL)ctlH, ctlRad);
                    DrawRoundedRect(g, subtleBorder, (Gdiplus::REAL)togRc.left, (Gdiplus::REAL)togRc.top, (Gdiplus::REAL)togW, (Gdiplus::REAL)ctlH, ctlRad);
                    Gdiplus::RectF togRf((Gdiplus::REAL)togRc.left, (Gdiplus::REAL)togRc.top, (Gdiplus::REAL)togW, (Gdiplus::REAL)ctlH);
                    g.DrawString(L"Locked", -1, &smallFont, togRf, &sfCenter, &mutedBr);
                    if (togRc.top >= listTop && togRc.bottom <= listBottom) {
                        g_hitRects.push_back({togRc, 1000 + idx * 4, idx, L"System-critical process is locked (Safety settings)", HitRect::RowToggle});
                    }
                } else {
                    int exSz = DpiScale(24, dpi);
                    RECT exRc{rowRc.right - DpiScale(176, dpi), ctlY + DpiScale(1, dpi),
                              rowRc.right - DpiScale(176, dpi) + exSz, ctlY + DpiScale(1, dpi) + exSz};
                    Gdiplus::SolidBrush exBg(g_hoverHitId == (1000 + idx * 4 + 2) ? Gdiplus::Color(255, 52, 52, 58) : Gdiplus::Color(255, 26, 26, 30));
                    FillRoundedRect(g, exBg, (Gdiplus::REAL)exRc.left, (Gdiplus::REAL)exRc.top, (Gdiplus::REAL)exSz, (Gdiplus::REAL)exSz, ctlRad);
                    Gdiplus::RectF exRf((Gdiplus::REAL)exRc.left, (Gdiplus::REAL)exRc.top, (Gdiplus::REAL)exSz, (Gdiplus::REAL)exSz);
                    g.DrawString(L"×", -1, &smallFont, exRf, &sfCenter, &mutedBr);
                    if (exRc.top >= listTop && exRc.bottom <= listBottom) {
                        g_hitRects.push_back({exRc, 1000 + idx * 4 + 2, idx, L"Exclude this app from !Net", HitRect::RowExclude});
                    }

                    int dirW = DpiScale(72, dpi);
                    RECT dirRc{rowRc.right - DpiScale(146, dpi), ctlY,
                               rowRc.right - DpiScale(146, dpi) + dirW, ctlY + ctlH};
                    bool isBlocked = app.block.blocked;
                    Gdiplus::SolidBrush dirBg(isBlocked ? Gdiplus::Color(255, 54, 24, 24) : Gdiplus::Color(255, 28, 28, 32));
                    FillRoundedRect(g, dirBg, (Gdiplus::REAL)dirRc.left, (Gdiplus::REAL)dirRc.top, (Gdiplus::REAL)dirW, (Gdiplus::REAL)ctlH, ctlRad);
                    DrawRoundedRect(g, isBlocked ? activeDirBorder : subtleBorder,
                                    (Gdiplus::REAL)dirRc.left, (Gdiplus::REAL)dirRc.top, (Gdiplus::REAL)dirW, (Gdiplus::REAL)ctlH, ctlRad);
                    Gdiplus::RectF dirRf((Gdiplus::REAL)dirRc.left, (Gdiplus::REAL)dirRc.top, (Gdiplus::REAL)dirW, (Gdiplus::REAL)ctlH);
                    g.DrawString(DirectionShortLabel(app.block.direction), -1, &smallFont, dirRf, &sfCenter, isBlocked ? &redBr : &mutedBr);
                    if (dirRc.top >= listTop && dirRc.bottom <= listBottom) {
                        std::wstring dTip = isBlocked
                            ? (std::wstring(L"Active block: ") + DirectionLabel(app.block.direction))
                            : (std::wstring(L"Mode when blocked: ") + DirectionLabel(app.block.direction) + L" (click to change)");
                        g_hitRects.push_back({dirRc, 1000 + idx * 4 + 1, idx, dTip, HitRect::RowDirection});
                    }

                    int togW = DpiScale(60, dpi);
                    RECT togRc{rowRc.right - togW - DpiScale(10, dpi), ctlY,
                               rowRc.right - DpiScale(10, dpi), ctlY + ctlH};
                    bool allowed = !isBlocked;
                    Gdiplus::SolidBrush togBg(allowed ? Gdiplus::Color(255, 34, 175, 85)
                                                      : Gdiplus::Color(255, 225, 58, 58));
                    FillRoundedRect(g, togBg, (Gdiplus::REAL)togRc.left, (Gdiplus::REAL)togRc.top, (Gdiplus::REAL)togW, (Gdiplus::REAL)ctlH, ctlRad);
                    Gdiplus::RectF togRf((Gdiplus::REAL)togRc.left, (Gdiplus::REAL)togRc.top, (Gdiplus::REAL)togW, (Gdiplus::REAL)ctlH);
                    g.DrawString(allowed ? L"Allowed" : L"Blocked", -1, &smallFont, togRf, &sfCenter, &whiteBr);
                    if (togRc.top >= listTop && togRc.bottom <= listBottom) {
                        std::wstring togTip = allowed
                            ? L"Internet allowed — click to Block this app"
                            : L"Internet blocked — click to Allow this app";
                        if (!allowed && !app.block.blockedChildren.empty()) {
                            togTip += L" (also blocks: ";
                            for (size_t ci = 0; ci < app.block.blockedChildren.size(); ++ci) {
                                if (ci > 0) togTip += L", ";
                                togTip += app.block.blockedChildren[ci];
                            }
                            togTip += L")";
                        }
                        g_hitRects.push_back({togRc, 1000 + idx * 4, idx, togTip, HitRect::RowToggle});
                    }
                }
            }

            SelectClipRgn(memDC, nullptr);
            DeleteObject(gdiClip);
            g.ResetClip();

            if (g_expandClock.currentVal < 0.99) {
                double contentVis = std::clamp((g_expandClock.currentVal - 0.30) / 0.65, 0.0, 1.0);
                BYTE scrimA = (BYTE)std::clamp((int)std::round((1.0 - contentVis) * 255.0), 0, 255);
                if (scrimA > 0) {
                    Gdiplus::SolidBrush scrimBr(Gdiplus::Color(scrimA, 0, 0, 0));
                    g.FillRectangle(&scrimBr, 0, 1, w, std::max(0, h - 1));
                }
            }
        }
    }

    bool isLayered = (GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & WS_EX_LAYERED) != 0;
    if (isLayered) {
        RECT wr{};
        GetWindowRect(hwnd, &wr);
        POINT ptDst{wr.left, wr.top};
        SIZE sz{w, h};
        POINT ptSrc{0, 0};
        BLENDFUNCTION bf{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        UpdateLayeredWindow(hwnd, hdcScreen, &ptDst, &sz, memDC, &ptSrc, 0, &bf, ULW_ALPHA);
    } else {
        BitBlt(hdcScreen, 0, 0, w, h, memDC, 0, 0, SRCCOPY);
    }
    SelectObject(memDC, oldBmp);
    DeleteObject(hBmp);
    DeleteDC(memDC);
    EndPaint(hwnd, &ps);
}

// ============================================================================
// Tray Icon & Tooltip Helpers
// ============================================================================

static void AddTrayIcon(HWND hwnd) {
    if (g_trayAdded) return;
    g_nid.cbSize = sizeof(g_nid);
    g_nid.hWnd = hwnd;
    g_nid.uID = 1;
    g_nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    g_nid.uCallbackMessage = WM_APP_TRAY_CALLBACK;
    SHSTOCKICONINFO sii{sizeof(sii)};
    if (SUCCEEDED(SHGetStockIconInfo(SIID_INTERNET, SHGSI_ICON | SHGSI_SMALLICON, &sii))) {
        g_nid.hIcon = sii.hIcon;
    } else {
        g_nid.hIcon = LoadIconW(nullptr, IDI_SHIELD);
    }
    wcscpy_s(g_nid.szTip, kTrayTip);
    if (Shell_NotifyIconW(NIM_ADD, &g_nid)) {
        g_trayAdded = true;
    }
}

static void RemoveTrayIcon() {
    if (g_trayAdded) {
        Shell_NotifyIconW(NIM_DELETE, &g_nid);
        g_trayAdded = false;
    }
    if (g_nid.hIcon) {
        DestroyIcon(g_nid.hIcon);
        g_nid.hIcon = nullptr;
    }
}

static void ShowTrayContextMenu(HWND hwnd) {
    HMENU menu = CreatePopupMenu();
    AppendMenuW(menu, MF_STRING, 1, (g_uiState == UiState::Line) ? L"Expand !Net Panel" : L"Collapse !Net Panel");
    AppendMenuW(menu, MF_STRING, 2, L"Restore All Session Blocks");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, 3, g_pinned ? L"Unpin Panel" : L"Pin Panel Open");
    AppendMenuW(menu, MF_STRING, 4, L"Open Windhawk Settings");

    POINT pt{};
    GetCursorPos(&pt);
    SetForegroundWindow(hwnd);
    int cmd = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, nullptr);
    DestroyMenu(menu);

    if (cmd == 1) {
        g_uiState = (g_uiState == UiState::Line) ? UiState::Panel : UiState::Line;
        if (g_uiState == UiState::Panel) RefreshAppList();
        UpdateWindowGeometry(false);
    } else if (cmd == 2) {
        RestoreAllSessionBlocks();
        RefreshAppList();
        RequestRepaint();
    } else if (cmd == 3) {
        g_pinned = !g_pinned;
        if (g_pinned && g_uiState == UiState::Line) {
            g_uiState = UiState::Panel;
            RefreshAppList();
            UpdateWindowGeometry(false);
        }
        RequestRepaint();
    } else if (cmd == 4) {
        OpenWindhawkSettingsUI();
    }
}

static void InitTooltip(HWND hwnd) {
    INITCOMMONCONTROLSEX icc{sizeof(icc), ICC_WIN95_CLASSES};
    InitCommonControlsEx(&icc);
    g_hTooltip = CreateWindowExW(WS_EX_TOPMOST, TOOLTIPS_CLASSW, nullptr,
                                 WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
                                 CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                 hwnd, nullptr, GetModuleHandleW(nullptr), nullptr);
    if (g_hTooltip) {
        TOOLINFOW ti{sizeof(ti)};
        ti.uFlags = TTF_SUBCLASS | TTF_IDISHWND;
        ti.hwnd = hwnd;
        ti.uId = (UINT_PTR)hwnd;
        ti.lpszText = const_cast<LPWSTR>(L"");
        SendMessageW(g_hTooltip, TTM_ADDTOOLW, 0, (LPARAM)&ti);
        SendMessageW(g_hTooltip, TTM_SETMAXTIPWIDTH, 0, 320);
        SendMessageW(g_hTooltip, TTM_SETDELAYTIME, TTDT_INITIAL, 300);
    }
}

static void UpdateTooltipText(HWND hwnd, const wchar_t* text) {
    if (!g_hTooltip) return;
    bool showTips = false;
    {
        std::lock_guard<std::mutex> lk(g_settingsMtx);
        showTips = g_settings.showTooltips;
    }
    TOOLINFOW ti{sizeof(ti)};
    ti.hwnd = hwnd;
    ti.uId = (UINT_PTR)hwnd;
    ti.lpszText = const_cast<LPWSTR>((showTips && text) ? text : L"");
    SendMessageW(g_hTooltip, TTM_UPDATETIPTEXTW, 0, (LPARAM)&ti);
}

// ============================================================================
// Window Procedure & Interaction State Machine
// ============================================================================

static const HitRect* HitTestControl(POINT pt) {
    for (auto it = g_hitRects.rbegin(); it != g_hitRects.rend(); ++it) {
        if (PtInRect(&it->rc, pt)) return &(*it);
    }
    return nullptr;
}

static void HandleClickAction(const HitRect& hr) {
    if (hr.kind == HitRect::PinBtn) {
        g_pinned = !g_pinned;
        RequestRepaint();
    } else if (hr.kind == HitRect::RestoreAllBtn) {
        RestoreAllSessionBlocks();
        RefreshAppList();
        RequestRepaint();
    } else if (hr.kind == HitRect::ElevateBtn) {
        RelaunchElevated();
    } else if (hr.kind == HitRect::SettingsBtn) {
        OpenWindhawkSettingsUI();
    } else if (hr.kind == HitRect::FilterChip) {
        g_userPinnedFilter = true;
        AppFilter nextF = (AppFilter)hr.rowIndex;
        if (nextF == AppFilter::UserApps && g_activeFilter == AppFilter::UserApps) {
            g_userRunningOnly = !g_userRunningOnly;
        } else if (nextF == AppFilter::Blocked && g_activeFilter == AppFilter::Blocked) {
            g_blockedFirewallOnly = !g_blockedFirewallOnly;
        } else if (nextF == AppFilter::Blocked) {
            g_blockedFirewallOnly = false;
        }
        if (nextF == AppFilter::Blocked && g_blockedFirewallOnly) {
            g_statusBanner = L"Blocked by Windows Firewall — edit rules in Windows Defender Firewall.";
        } else if (nextF == AppFilter::SystemApps) {
            g_statusBanner = L"Experts only — if you don't know what you're doing, skip System.";
        } else {
            g_statusBanner.clear();
        }
        g_activeFilter = nextF;
        g_scrollClock.Start(0.0, 60.0);
        RefreshAppList();
        RequestRepaint();
    } else if (hr.kind == HitRect::RowExternalLocked) {
        g_statusBanner = L"!Net can't manage this app — edit its rule in Windows Defender Firewall.";
        RequestRepaint();
    } else if (hr.kind == HitRect::RowExclude && hr.rowIndex >= 0 && hr.rowIndex < (int)g_appList.size()) {
        AppEntry app = g_appList[hr.rowIndex];
        if (app.isExternalBlocked) {
            g_statusBanner = L"!Net can't manage this app — edit its rule in Windows Defender Firewall.";
            RequestRepaint();
            return;
        }
        std::wstring low = ToLower(app.displayPath);
        if (app.isExcluded) {
            g_excludedExes.erase(low);
            LogAudit(L"UNEXCLUDE " + app.displayPath);
        } else {
            ApplyAllow(app.displayPath);
            g_excludedExes.insert(low);
            LogAudit(L"EXCLUDE " + app.displayPath);
        }
        SaveExcludedApps();
        RefreshAppList();
        RequestRepaint();
    } else if (hr.kind == HitRect::RowToggle && hr.rowIndex >= 0 && hr.rowIndex < (int)g_appList.size()) {
        AppEntry app = g_appList[hr.rowIndex];
        if (app.isExternalBlocked) {
            g_statusBanner = L"!Net can't manage this app — edit its rule in Windows Defender Firewall.";
            RequestRepaint();
            return;
        }
        CycleStatusSnapshot cyc;
        if (GetCycleSnapshot(app.exePath, cyc)) {
            int res = MessageBoxW(g_hWnd,
                L"This app has an active Cycle Mode schedule.\n\nStop the cycle and toggle manually?",
                L"!Net · Stop Cycle?", MB_YESNO | MB_ICONQUESTION | MB_TOPMOST);
            if (res != IDYES) return;
            StopCycleForApp(app.displayPath, false);
        }
        bool blockedNow = false;
        {
            std::lock_guard<std::mutex> lk(g_engineMtx);
            auto it = g_blocks.find(app.exePath);
            if (it != g_blocks.end() && it->second.blocked) blockedNow = true;
            else {
                for (const auto& bkv : g_blocks) {
                    if (bkv.second.blocked && bkv.second.blockedChildPaths.count(app.exePath)) {
                        blockedNow = true;
                        break;
                    }
                }
            }
        }
        if (blockedNow) {
            ApplyAllow(app.displayPath);
        } else {
            ModSettings s;
            {
                std::lock_guard<std::mutex> lk(g_settingsMtx);
                s = g_settings;
            }
            std::wstring low = ToLower(app.displayPath);
            auto itDir = g_pendingDirs.find(low);
            Direction dir = (itDir != g_pendingDirs.end()) ? itDir->second : s.defaultDirection;
            ApplyBlock(app.displayPath, dir, s.includeChildProcesses, true);
        }
        RefreshAppList();
        RequestRepaint();
    } else if (hr.kind == HitRect::RowDirection && hr.rowIndex >= 0 && hr.rowIndex < (int)g_appList.size()) {
        AppEntry app = g_appList[hr.rowIndex];
        if (app.isExternalBlocked) {
            g_statusBanner = L"!Net can't manage this app — edit its rule in Windows Defender Firewall.";
            RequestRepaint();
            return;
        }
        std::wstring low = ToLower(app.displayPath);
        Direction nextDir = (Direction)(((int)app.block.direction + 1) % 3);
        g_pendingDirs[low] = nextDir;
        if (app.block.blocked) {
            bool isDirectRoot = false;
            {
                std::lock_guard<std::mutex> lk(g_engineMtx);
                isDirectRoot = (g_blocks.find(low) != g_blocks.end());
            }
            if (!isDirectRoot) ApplyAllow(app.displayPath);
            ApplyBlock(app.displayPath, nextDir, app.block.includeChildProcesses, true);
        }
        RefreshAppList();
        RequestRepaint();
    }
}


// ============================================================================
// Cycle Mode Popup Dialog (Pure GDI+ #000000 Inputs, Dirty Rects, Clock Sync)
// ============================================================================

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif
#ifndef DWMWA_SYSTEMBACKDROP_TYPE
#define DWMWA_SYSTEMBACKDROP_TYPE 38
#endif
#ifndef DWMSBT_TRANSIENTWINDOW
#define DWMSBT_TRANSIENTWINDOW 3
#endif

constexpr wchar_t kCycleRunDropClass[] = L"ExclaimNet_CycleRunDrop_v1";
constexpr UINT_PTR TIMER_CYCLE_POPUP_ANIM  = 77;
constexpr UINT_PTR TIMER_CYCLE_POPUP_CARET = 78;
constexpr UINT_PTR TIMER_CYCLE_POPUP_OPEN   = 79;
constexpr UINT_PTR TIMER_CYCLE_POPUP_RESIZE = 80;
constexpr UINT_PTR TIMER_CYCLE_CLOCK_COMMIT = 81;
constexpr uint64_t kCycleMaxMs = 99ULL * 3600000ULL;

enum CycleFieldId {
    CF_NONE = -1,
    CF_BLK_H = 0, CF_BLK_M, CF_BLK_S, CF_BLK_MS,
    CF_ALW_H,     CF_ALW_M, CF_ALW_S, CF_ALW_MS,
    CF_RUN_CYC,
    CF_RUN_H,     CF_RUN_M, CF_RUN_S, CF_RUN_MS,
    CF_CLK_H,     CF_CLK_M,
    CF_COUNT
};

struct DurationGroupState {
    std::wstring txtH = L"0";
    std::wstring txtM = L"00";
    std::wstring txtS = L"10";
    std::wstring txtMs = L"000";

    RECT rcH{};
    RECT rcM{};
    RECT rcS{};
    RECT rcMs{};

    bool showHours = false;
    uint64_t totalMs = 0;

    double animVal = 0.0;
    LARGE_INTEGER animStart{};
    bool animating = false;
};

struct CycleHitRect {
    RECT rc{};
    int id = 0;
    enum Kind {
                None = 0, InputBox, RunDropdown, SyncClockToggle, ClockFormat12h, FocusToggle, AllowUnfocusedToggle, CancelBtn, PrimaryBtn
    } kind = None;
    int fieldId = CF_NONE;
};

struct CyclePopupState {
    std::wstring exePath;
    std::wstring displayPath;
    std::wstring appName;
    Direction direction = Direction::FullyOffline;
    bool isRunning = false;
    bool hasBackdropBlur = false;
    bool showMs = false;

    HWND hwnd = nullptr;
    HWND hRunDrop = nullptr;
    int runDropHoverIdx = -1;
    bool openingRunDrop = false;

    DurationGroupState blockGrp;
    DurationGroupState allowGrp;
    DurationGroupState runTimeGrp;

    std::wstring runCyclesTxt = L"5";
    RECT rcRunCycles{};
    RECT rcRunDrop{};

    bool syncWithWindows = false;
    bool clock12Hour = true;
    bool clockIsPm = false;
    bool clockEditing = false;
    std::wstring clkHourTxt = L"12";
    std::wstring clkMinTxt  = L"00";
    RECT rcClkH{};
    RECT rcClkM{};
    RECT rcClkAmPm{};
    uint64_t computedClockMs = 0;
    uint64_t computedTargetUtcMs = 0;

    int runModeIdx = 0; // 0=Until stopped, 1=For N cycles, 2=For a set time
    bool focusedOnly = true;
    bool allowWhenUnfocused = true;

    int focusedField = CF_BLK_S;
    bool replaceOnType = false;
    bool caretVisible = true;

    bool valid = true;
    bool warnSub1s = false;
    std::wstring summaryText;

    int hoverId = -1;
    RECT hoverRc{};
    std::vector<CycleHitRect> hits;
    RECT rcSummary{};
    RECT rcBtnPrimary{};
    bool sideBySideBlockAllow = false;
    MotionClock openClock;
    MotionClock heightClock;
    int targetX = 0;
    int targetY = 0;
    int targetW = 0;
    int targetH = 0;
};

static CyclePopupState* g_cyclePopupState = nullptr;

static uint64_t ParseDigitsU64(const std::wstring& s) {
    if (s.empty()) return 0ULL;
    return (uint64_t)_wcstoui64(s.c_str(), nullptr, 10);
}

static void DecomposeMs(uint64_t ms, uint64_t& outH, uint64_t& outM, uint64_t& outS, uint64_t& outMs) {
    if (ms > kCycleMaxMs) ms = kCycleMaxMs;
    outH = ms / 3600000ULL;
    ms %= 3600000ULL;
    outM = ms / 60000ULL;
    ms %= 60000ULL;
    outS = ms / 1000ULL;
    outMs = ms % 1000ULL;
}

static uint64_t ComputeDurationGroupRawMs(const DurationGroupState& grp, bool showMs) {
    uint64_t h = grp.showHours ? ParseDigitsU64(grp.txtH) : 0ULL;
    uint64_t m = ParseDigitsU64(grp.txtM);
    uint64_t s = ParseDigitsU64(grp.txtS);
    uint64_t ms = showMs ? ParseDigitsU64(grp.txtMs) : 0ULL;
    uint64_t total = h * 3600000ULL + m * 60000ULL + s * 1000ULL + ms;
    return std::min<uint64_t>(total, kCycleMaxMs);
}

static void WriteDurationGroupMs(DurationGroupState& grp, uint64_t totalMs, bool animateMerge) {
    if (totalMs > kCycleMaxMs) totalMs = kCycleMaxMs;
    uint64_t nh = 0, nm = 0, ns = 0, nms = 0;
    DecomposeMs(totalMs, nh, nm, ns, nms);
    grp.totalMs = totalMs;
    grp.showHours = (nh > 0);

    wchar_t bufM[16]{}, bufS[16]{}, bufMs[16]{};
    swprintf_s(bufM, L"%02llu", (unsigned long long)nm);
    swprintf_s(bufS, L"%02llu", (unsigned long long)ns);
    swprintf_s(bufMs, L"%03llu", (unsigned long long)nms);

    grp.txtH = grp.showHours ? std::to_wstring(nh) : L"0";
    grp.txtM = bufM;
    grp.txtS = bufS;
    grp.txtMs = bufMs;

    if (animateMerge) {
        QueryPerformanceCounter(&grp.animStart);
        grp.animVal = 1.0;
        grp.animating = true;
    }
}

static int ClockTo24Hour(uint64_t hh, bool is12h, bool isPm) {
    if (!is12h) return (hh <= 23) ? (int)hh : -1;
    if (hh < 1 || hh > 12) return -1;
    if (hh == 12) return isPm ? 12 : 0;
    return isPm ? (int)(hh + 12) : (int)hh;
}

static void PopulateClockFromWindows(CyclePopupState* st) {
    if (!st) return;
    SYSTEMTIME lt{};
    GetLocalTime(&lt);
    wchar_t bH[8]{}, bM[8]{};
    if (st->clock12Hour) {
        st->clockIsPm = (lt.wHour >= 12);
        int h12 = lt.wHour % 12;
        if (h12 == 0) h12 = 12;
        swprintf_s(bH, L"%02d", h12);
    } else {
        swprintf_s(bH, L"%02u", (unsigned)lt.wHour);
    }
    swprintf_s(bM, L"%02u", (unsigned)lt.wMinute);
    st->clkHourTxt = bH;
    st->clkMinTxt = bM;
}

static void LayoutCyclePopup(CyclePopupState* st, UINT dpi, bool animateResize = false);

static bool AutoCarryDurationGroup(CyclePopupState* st, DurationGroupState& grp) {
    uint64_t h = grp.showHours ? ParseDigitsU64(grp.txtH) : 0ULL;
    uint64_t m = ParseDigitsU64(grp.txtM);
    uint64_t s = ParseDigitsU64(grp.txtS);
    uint64_t ms = (st && st->showMs) ? ParseDigitsU64(grp.txtMs) : 0ULL;

    uint64_t total = std::min<uint64_t>(kCycleMaxMs, h * 3600000ULL + m * 60000ULL + s * 1000ULL + ms);
    uint64_t nh = 0, nm = 0, ns = 0, nms = 0;
    DecomposeMs(total, nh, nm, ns, nms);

    bool needsCarry = (ms > 999 || s > 59 || m > 59 || h != nh || grp.showHours != (nh > 0));
    bool visChanged = (grp.showHours != (nh > 0));

    WriteDurationGroupMs(grp, total, needsCarry);
    if (needsCarry && st && st->hwnd) {
        SetTimer(st->hwnd, TIMER_CYCLE_POPUP_ANIM, 16, nullptr);
    }
    if (visChanged && st && st->hwnd) {
        LayoutCyclePopup(st, GetDpiForWindow(st->hwnd));
    }
    return needsCarry;
}

static std::wstring* GetFieldTextPtr(CyclePopupState* st, int fieldId, RECT* outRc = nullptr) {
    if (!st) return nullptr;
    auto ret = [&](std::wstring& s, const RECT& r) -> std::wstring* {
        if (outRc) *outRc = r;
        return &s;
    };
    switch (fieldId) {
        case CF_BLK_H:  return ret(st->blockGrp.txtH, st->blockGrp.rcH);
        case CF_BLK_M:  return ret(st->blockGrp.txtM, st->blockGrp.rcM);
        case CF_BLK_S:  return ret(st->blockGrp.txtS, st->blockGrp.rcS);
        case CF_BLK_MS: return ret(st->blockGrp.txtMs, st->blockGrp.rcMs);
        case CF_ALW_H:  return ret(st->allowGrp.txtH, st->allowGrp.rcH);
        case CF_ALW_M:  return ret(st->allowGrp.txtM, st->allowGrp.rcM);
        case CF_ALW_S:  return ret(st->allowGrp.txtS, st->allowGrp.rcS);
        case CF_ALW_MS: return ret(st->allowGrp.txtMs, st->allowGrp.rcMs);
        case CF_RUN_CYC: return ret(st->runCyclesTxt, st->rcRunCycles);
        case CF_RUN_H:  return ret(st->runTimeGrp.txtH, st->runTimeGrp.rcH);
        case CF_RUN_M:  return ret(st->runTimeGrp.txtM, st->runTimeGrp.rcM);
        case CF_RUN_S:  return ret(st->runTimeGrp.txtS, st->runTimeGrp.rcS);
        case CF_RUN_MS: return ret(st->runTimeGrp.txtMs, st->runTimeGrp.rcMs);
        case CF_CLK_H:  return ret(st->clkHourTxt, st->rcClkH);
        case CF_CLK_M:  return ret(st->clkMinTxt, st->rcClkM);
    }
    return nullptr;
}

static DurationGroupState* GetDurationGroupByField(CyclePopupState* st, int fieldId) {
    if (!st) return nullptr;
    if (fieldId >= CF_BLK_H && fieldId <= CF_BLK_MS) return &st->blockGrp;
    if (fieldId >= CF_ALW_H && fieldId <= CF_ALW_MS) return &st->allowGrp;
    if (fieldId >= CF_RUN_H && fieldId <= CF_RUN_MS) return &st->runTimeGrp;
    return nullptr;
}

static const wchar_t* RunModeDisplayLabel(int idx) {
    switch (idx) {
        case 1: return L"For N cycles";
        case 2: return L"For a set time";
        default: return L"Until stopped";
    }
}



static uint64_t ComputeNext12hDiffSeconds(uint64_t hh, uint64_t mm, int nowSec, bool& outIsPm) {
    int hAm = (hh == 12) ? 0 : (int)hh;
    int hPm = (hh == 12) ? 12 : (int)(hh + 12);
    int secAm = hAm * 3600 + (int)mm * 60;
    int secPm = hPm * 3600 + (int)mm * 60;
    uint64_t waitAm = (secAm > nowSec) ? (uint64_t)(secAm - nowSec) : (uint64_t)(secAm + 86400 - nowSec);
    uint64_t waitPm = (secPm > nowSec) ? (uint64_t)(secPm - nowSec) : (uint64_t)(secPm + 86400 - nowSec);
    if (waitAm <= waitPm) {
        outIsPm = false;
        return waitAm;
    }
    outIsPm = true;
    return waitPm;
}

static void AutoCalculateAmPm(CyclePopupState* st) {
    if (!st || !st->clock12Hour) return;
    uint64_t hh = ParseDigitsU64(st->clkHourTxt);
    uint64_t mm = ParseDigitsU64(st->clkMinTxt);
    if (hh < 1 || hh > 12 || mm > 59) return;
    SYSTEMTIME ltNow{};
    GetLocalTime(&ltNow);
    int nowSec = ltNow.wHour * 3600 + ltNow.wMinute * 60 + ltNow.wSecond;
    ComputeNext12hDiffSeconds(hh, mm, nowSec, st->clockIsPm);
}

static void CommitClockFields(CyclePopupState* st) {
    if (!st) return;
    st->clockEditing = false;
    if (st->hwnd) KillTimer(st->hwnd, TIMER_CYCLE_CLOCK_COMMIT);
    uint64_t hh = ParseDigitsU64(st->clkHourTxt);
    uint64_t mm = ParseDigitsU64(st->clkMinTxt);
    bool hValid = st->clock12Hour ? (hh >= 1 && hh <= 12) : (hh <= 23);
    bool mValid = (mm <= 59);
    if (hValid && !st->clkHourTxt.empty()) {
        wchar_t bH[8]{};
        swprintf_s(bH, L"%02llu", (unsigned long long)hh);
        st->clkHourTxt = bH;
    }
    if (mValid && !st->clkMinTxt.empty()) {
        wchar_t bM[8]{};
        swprintf_s(bM, L"%02llu", (unsigned long long)mm);
        st->clkMinTxt = bM;
    }
    AutoCalculateAmPm(st);
    if (st->hwnd) {
        if (st->rcClkH.right > st->rcClkH.left) InvalidateRect(st->hwnd, &st->rcClkH, FALSE);
        if (st->rcClkM.right > st->rcClkM.left) InvalidateRect(st->hwnd, &st->rcClkM, FALSE);
    }
}

static void UpdateCyclePopupSummary(CyclePopupState* st, bool repaintSummary = true) {
    if (!st) return;
    uint64_t bMs = ComputeDurationGroupRawMs(st->blockGrp, st->showMs);
    uint64_t aMs = ComputeDurationGroupRawMs(st->allowGrp, st->showMs);
    st->blockGrp.totalMs = bMs;
    st->allowGrp.totalMs = aMs;

    st->warnSub1s = false;
    st->computedClockMs = 0;
    st->computedTargetUtcMs = 0;

    bool prevValid = st->valid;
    if (bMs < 100 || aMs < 100) {
        st->valid = false;
        st->summaryText = L"Min 100 ms";
    } else if (st->runModeIdx == 1 && ParseDigitsU64(st->runCyclesTxt) < 1) {
        st->valid = false;
        st->summaryText = L"Min 1 cycle";
    } else if (st->runModeIdx == 2 && st->syncWithWindows) {
        uint64_t hh = ParseDigitsU64(st->clkHourTxt);
        uint64_t mm = ParseDigitsU64(st->clkMinTxt);
        if (st->clockEditing) {
            if (st->clock12Hour && hh >= 1 && hh <= 12 && mm <= 59) {
                bool prevPm = st->clockIsPm;
                AutoCalculateAmPm(st);
                if (prevPm != st->clockIsPm && st->hwnd && st->rcClkAmPm.right > st->rcClkAmPm.left) {
                    InvalidateRect(st->hwnd, &st->rcClkAmPm, FALSE);
                }
            }
            st->valid = true;
            st->summaryText = L"Editing target time...";
        } else if (st->clock12Hour) {
            if (hh < 1 || hh > 12 || mm > 59) {
                st->valid = false;
                st->summaryText = L"Use 01\u201312 : 00\u201359";
            } else {
                SYSTEMTIME ltNow{};
                GetLocalTime(&ltNow);
                int nowSec = ltNow.wHour * 3600 + ltNow.wMinute * 60 + ltNow.wSecond;
                uint64_t diffSec = ComputeNext12hDiffSeconds(hh, mm, nowSec, st->clockIsPm);
                uint64_t diffMs = diffSec * 1000ULL;
                st->computedClockMs = diffMs;
                st->computedTargetUtcMs = UtcNowMs() + diffMs;
                st->valid = true;
                if (bMs < 1000 || aMs < 1000) st->warnSub1s = true;
                wchar_t tBuf[24]{};
                swprintf_s(tBuf, L"%02llu:%02llu %s", (unsigned long long)hh, (unsigned long long)mm, st->clockIsPm ? L"PM" : L"AM");
                st->summaryText = L"Blocked " + FormatDurationCompact(bMs) + L" \u2192 Allowed " + FormatDurationCompact(aMs) + L" \u00b7 until " + tBuf;
                if (st->warnSub1s) st->summaryText += L"  (\u26a0 < 1s)";
                if (st->hwnd && st->rcClkAmPm.right > st->rcClkAmPm.left) InvalidateRect(st->hwnd, &st->rcClkAmPm, FALSE);
            }
        } else {
            if (hh > 23 || mm > 59) {
                st->valid = false;
                st->summaryText = L"Use 00\u201323 : 00\u201359";
            } else {
                SYSTEMTIME ltNow{};
                GetLocalTime(&ltNow);
                int nowSec = ltNow.wHour * 3600 + ltNow.wMinute * 60 + ltNow.wSecond;
                int tgtSec = (int)hh * 3600 + (int)mm * 60;
                if (tgtSec <= nowSec) {
                    st->valid = false;
                    st->summaryText = L"Enter a future time";
                } else {
                    uint64_t diffMs = (uint64_t)(tgtSec - nowSec) * 1000ULL;
                    st->computedClockMs = diffMs;
                    st->computedTargetUtcMs = UtcNowMs() + diffMs;
                    st->valid = true;
                    if (bMs < 1000 || aMs < 1000) st->warnSub1s = true;
                    wchar_t tBuf[24]{};
                    swprintf_s(tBuf, L"%02llu:%02llu", (unsigned long long)hh, (unsigned long long)mm);
                    st->summaryText = L"Blocked " + FormatDurationCompact(bMs) + L" \u2192 Allowed " + FormatDurationCompact(aMs) + L" \u00b7 until " + tBuf;
                    if (st->warnSub1s) st->summaryText += L"  (\u26a0 < 1s)";
                }
            }
        }
    } else if (st->runModeIdx == 2 && ComputeDurationGroupRawMs(st->runTimeGrp, st->showMs) < 100) {
        st->valid = false;
        st->summaryText = L"Min 100 ms";
    } else {
        st->valid = true;
        if (bMs < 1000 || aMs < 1000) st->warnSub1s = true;
        st->summaryText = L"Blocked " + FormatDurationCompact(bMs) + L" \u2192 Allowed " + FormatDurationCompact(aMs);
        if (st->runModeIdx == 1) {
            uint64_t cnt = ParseDigitsU64(st->runCyclesTxt);
            st->summaryText += L" \u00b7 " + std::to_wstring(cnt) + (cnt == 1 ? L" cycle" : L" cycles");
        } else if (st->runModeIdx == 2) {
            uint64_t rMs = ComputeDurationGroupRawMs(st->runTimeGrp, st->showMs);
            st->summaryText += L" \u00b7 for " + FormatDurationCompact(rMs);
        }
        if (st->warnSub1s) st->summaryText += L"  (\u26a0 < 1s)";
    }

    if (repaintSummary && st->hwnd) {
        InvalidateRect(st->hwnd, &st->rcSummary, FALSE);
        if (prevValid != st->valid) {
            InvalidateRect(st->hwnd, &st->rcBtnPrimary, FALSE);
        }
    }
}

static void LayoutCyclePopup(CyclePopupState* st, UINT dpi, bool animateResize) {
    if (!st || !st->hwnd) return;
    int pad = DpiScale(16, dpi);
    int boxH = DpiScale(36, dpi);
    int boxGap = DpiScale(10, dpi);
    int secGap = DpiScale(14, dpi);
    int lblH = DpiScale(18, dpi);
    int lblGap = DpiScale(6, dpi);
    int subLblH = DpiScale(14, dpi);
    int subLblGap = DpiScale(3, dpi);
    int titleH = DpiScale(22, dpi);
    int titleGap = DpiScale(14, dpi);

    int boxW = DpiScale(64, dpi);
    int msW  = DpiScale(70, dpi);
    int hrsW = DpiScale(48, dpi);

    st->sideBySideBlockAllow = (!st->showMs && !st->blockGrp.showHours && !st->allowGrp.showHours);
    int colGap = DpiScale(20, dpi);
    int twoBoxW = boxW * 2 + boxGap;
    bool anyHours = st->blockGrp.showHours || st->allowGrp.showHours ||
                    (st->runModeIdx == 2 && !st->syncWithWindows && st->runTimeGrp.showHours);
    int baseGroupW = st->sideBySideBlockAllow
        ? (twoBoxW * 2 + colGap)
        : (boxW + boxGap + boxW + (st->showMs ? (boxGap + msW) : 0));
    int maxGroupW = st->sideBySideBlockAllow
        ? std::max<int>(baseGroupW, (st->runModeIdx == 2 && !st->syncWithWindows && st->runTimeGrp.showHours) ? (hrsW + boxGap + twoBoxW) : baseGroupW)
        : (baseGroupW + (anyHours ? (hrsW + boxGap) : 0));
    int winW = pad * 2 + std::max<int>(DpiScale(240, dpi), maxGroupW);

    auto layoutDurationAt = [&](DurationGroupState& grp, int xStart, int yTop) -> int {
        int x = xStart;
        grp.rcH  = grp.showHours ? RECT{x, yTop, x + hrsW, yTop + boxH} : RECT{0, 0, 0, 0};
        if (grp.showHours) x += hrsW + boxGap;
        grp.rcM  = {x, yTop, x + boxW, yTop + boxH};
        x += boxW + boxGap;
        grp.rcS  = {x, yTop, x + boxW, yTop + boxH};
        x += boxW + boxGap;
        grp.rcMs = st->showMs ? RECT{x, yTop, x + msW, yTop + boxH} : RECT{0, 0, 0, 0};
        return yTop + boxH + subLblGap + subLblH;
    };
    auto layoutDuration = [&](DurationGroupState& grp, int yTop) -> int {
        return layoutDurationAt(grp, pad, yTop);
    };

    int y = pad + titleH + titleGap;
    if (st->sideBySideBlockAllow) {
        y += lblH + lblGap;
        int allowX = pad + twoBoxW + colGap;
        layoutDurationAt(st->blockGrp, pad, y);
        y = layoutDurationAt(st->allowGrp, allowX, y) + secGap;
    } else {
        y += lblH + lblGap;
        y = layoutDuration(st->blockGrp, y) + secGap;
        y += lblH + lblGap;
        y = layoutDuration(st->allowGrp, y) + secGap;
    }

    y += lblH + lblGap;
    st->rcRunDrop = {pad, y, winW - pad, y + boxH};
    y += boxH;

    if (st->runModeIdx == 1) {
        y += boxGap;
        st->rcRunCycles = {pad, y, pad + DpiScale(92, dpi), y + boxH};
        y += boxH;
    } else if (st->runModeIdx == 2) {
        y += DpiScale(10, dpi);
        y += DpiScale(22, dpi) + DpiScale(8, dpi); // Sync with Windows clock row
        if (st->syncWithWindows) {
            st->rcClkH = {pad, y, pad + boxW, y + boxH};
            st->rcClkM = {pad + boxW + DpiScale(16, dpi), y, pad + boxW * 2 + DpiScale(16, dpi), y + boxH};
            st->rcClkAmPm = st->clock12Hour
                ? RECT{st->rcClkM.right + DpiScale(10, dpi), y, st->rcClkM.right + DpiScale(60, dpi), y + boxH}
                : RECT{0, 0, 0, 0};
            y += boxH + subLblGap + subLblH;
        } else {
            y = layoutDuration(st->runTimeGrp, y);
        }
    }

    y += DpiScale(16, dpi);
    int chkH = DpiScale(22, dpi);
    y += chkH + DpiScale(8, dpi);
    if (st->focusedOnly) {
        y += chkH + DpiScale(8, dpi);
    }

    int sumH = DpiScale(16, dpi);
    st->rcSummary = {pad, y, winW - pad, y + sumH};
    y += sumH + DpiScale(14, dpi);

    int btnW = DpiScale(88, dpi);
    int btnH = DpiScale(32, dpi);
    st->rcBtnPrimary = {winW - pad - btnW, y, winW - pad, y + btnH};
    int totalH = y + btnH + pad;

    RECT curRc{};
    GetWindowRect(st->hwnd, &curRc);
    HMONITOR mon = MonitorFromWindow(st->hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{sizeof(mi)};
    GetMonitorInfoW(mon, &mi);
    int edgeOffPx = 0;
    {
        std::lock_guard<std::mutex> lk(g_settingsMtx);
        edgeOffPx = DpiScale(g_settings.panelEdgeOffsetPx, dpi);
    }
    int minX = (int)mi.rcWork.left + edgeOffPx;
    int maxX = std::max(minX, (int)mi.rcWork.right - edgeOffPx - winW);
    int minY = (int)mi.rcWork.top + edgeOffPx;
    int maxY = std::max(minY, (int)mi.rcWork.bottom - edgeOffPx - totalH);
    int nx = std::clamp((int)curRc.left, minX, maxX);
    int ny = std::clamp((int)curRc.top, minY, maxY);
    st->targetX = nx;
    st->targetY = ny;
    st->targetW = winW;
    st->targetH = totalH;
    if (animateResize && IsWindowVisible(st->hwnd)) {
        int prevH = curRc.bottom - curRc.top;
        if (std::abs(prevH - totalH) > 1) {
            st->heightClock.Init();
            st->heightClock.currentVal = (double)prevH;
            st->heightClock.Start((double)totalH, 160.0);
            SetTimer(st->hwnd, TIMER_CYCLE_POPUP_RESIZE, 10, nullptr);
        } else {
            SetWindowPos(st->hwnd, HWND_TOPMOST, nx, ny, winW, totalH, SWP_NOACTIVATE);
        }
    } else {
        SetWindowPos(st->hwnd, HWND_TOPMOST, nx, ny, winW, totalH, SWP_NOACTIVATE);
    }
    InvalidateRect(st->hwnd, nullptr, FALSE);
}

static void PaintCyclePopup(HWND hwnd, CyclePopupState* st) {
    PAINTSTRUCT ps{};
    HDC hdcScreen = BeginPaint(hwnd, &ps);
    RECT rc{};
    GetClientRect(hwnd, &rc);
    int w = rc.right - rc.left;
    int h = rc.bottom - rc.top;
    if (w <= 0 || h <= 0 || !st) {
        EndPaint(hwnd, &ps);
        return;
    }

    UINT dpi = GetDpiForWindow(hwnd);
    HDC memDC = CreateCompatibleDC(hdcScreen);
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP hBmp = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    HGDIOBJ oldBmp = SelectObject(memDC, hBmp);
    if (bits) memset(bits, 0, (size_t)w * (size_t)h * 4);

    {
        Gdiplus::Graphics g(memDC);
        g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
        g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
        g.SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);

        BYTE bgAlpha = st->hasBackdropBlur ? 222 : 255;
        Gdiplus::SolidBrush bgBr(Gdiplus::Color(bgAlpha, 12, 12, 16));
        g.FillRectangle(&bgBr, 0, 0, w, h);

        Gdiplus::Pen topEdge(Gdiplus::Color(52, 255, 255, 255), 1.0f);
        g.DrawLine(&topEdge, 0, 0, w, 0);

        Gdiplus::Font titleFont(L"Segoe UI", (Gdiplus::REAL)DpiScale(16, dpi), Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
        Gdiplus::Font secFont(L"Segoe UI", (Gdiplus::REAL)DpiScale(13, dpi), Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
        Gdiplus::Font numFont(L"Segoe UI", (Gdiplus::REAL)DpiScale(15, dpi), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::Font uiFont(L"Segoe UI", (Gdiplus::REAL)DpiScale(13, dpi), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::Font subFont(L"Segoe UI", (Gdiplus::REAL)DpiScale(11, dpi), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        Gdiplus::Font glyphFont(L"Segoe MDL2 Assets", (Gdiplus::REAL)DpiScale(10, dpi), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

        Gdiplus::SolidBrush whiteBr(Gdiplus::Color(255, 248, 248, 250));
        Gdiplus::SolidBrush labelBr(Gdiplus::Color(255, 205, 209, 218));
        Gdiplus::SolidBrush mutedBr(Gdiplus::Color(255, 138, 144, 158));
        Gdiplus::SolidBrush warnBr(Gdiplus::Color(255, 210, 180, 140));
        Gdiplus::SolidBrush terraBr(Gdiplus::Color(255, 201, 100, 66));

        // Pure OLED #000000 for time inputs (alpha=255 so DWM never bleeds grey through)
        Gdiplus::SolidBrush pureBlackBr(Gdiplus::Color(255, 0, 0, 0));
        Gdiplus::SolidBrush surfRestBr(Gdiplus::Color(255, 20, 20, 25));
        Gdiplus::SolidBrush surfHoverBr(Gdiplus::Color(255, 28, 28, 34));
        Gdiplus::Pen borderRestPen(Gdiplus::Color(40, 255, 255, 255), 1.0f);

        Gdiplus::StringFormat sfLeft;
        sfLeft.SetAlignment(Gdiplus::StringAlignmentNear);
        sfLeft.SetLineAlignment(Gdiplus::StringAlignmentCenter);
        sfLeft.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
        sfLeft.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);

        Gdiplus::StringFormat sfCenter;
        sfCenter.SetAlignment(Gdiplus::StringAlignmentCenter);
        sfCenter.SetLineAlignment(Gdiplus::StringAlignmentCenter);
        sfCenter.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);

        st->hits.clear();

        int pad = DpiScale(16, dpi);
        int titleH = DpiScale(22, dpi);
        int titleGap = DpiScale(14, dpi);
        int lblH = DpiScale(18, dpi);
        int lblGap = DpiScale(6, dpi);
        int subLblH = DpiScale(14, dpi);
        int subLblGap = DpiScale(3, dpi);
        int boxH = DpiScale(36, dpi);
        int boxGap = DpiScale(10, dpi);
        int secGap = DpiScale(14, dpi);
        Gdiplus::REAL rad = (Gdiplus::REAL)DpiScale(10, dpi);

        std::wstring title = L"Cycle \u00b7 " + st->appName;
        Gdiplus::RectF titleRf((Gdiplus::REAL)pad, (Gdiplus::REAL)pad, (Gdiplus::REAL)(w - 2 * pad), (Gdiplus::REAL)titleH);
        g.DrawString(title.c_str(), -1, &titleFont, titleRf, &sfLeft, &whiteBr);

        auto drawNumBox = [&](const RECT& r, int fieldId, const std::wstring& txt, double animPulse, const wchar_t* subLbl) {
            if (r.right <= r.left) return;
            FillRoundedRect(g, pureBlackBr, (Gdiplus::REAL)r.left, (Gdiplus::REAL)r.top,
                            (Gdiplus::REAL)(r.right - r.left), (Gdiplus::REAL)(r.bottom - r.top), rad);
            if (animPulse > 0.01) {
                Gdiplus::SolidBrush pulseBr(Gdiplus::Color((BYTE)std::round(animPulse * 56.0), 201, 100, 66));
                FillRoundedRect(g, pulseBr, (Gdiplus::REAL)r.left, (Gdiplus::REAL)r.top,
                                (Gdiplus::REAL)(r.right - r.left), (Gdiplus::REAL)(r.bottom - r.top), rad);
            }
            DrawRoundedRect(g, borderRestPen, (Gdiplus::REAL)r.left, (Gdiplus::REAL)r.top,
                            (Gdiplus::REAL)(r.right - r.left), (Gdiplus::REAL)(r.bottom - r.top), rad);

            Gdiplus::RectF tRf((Gdiplus::REAL)r.left, (Gdiplus::REAL)r.top,
                               (Gdiplus::REAL)(r.right - r.left), (Gdiplus::REAL)(r.bottom - r.top));
            bool isEmpty = txt.empty();
            const std::wstring& disp = isEmpty ? L"0" : txt;
            g.DrawString(disp.c_str(), -1, &numFont, tRf, &sfCenter, isEmpty ? &mutedBr : &whiteBr);

            if (st->focusedField == fieldId && st->caretVisible && GetFocus() == hwnd) {
                Gdiplus::REAL cx = (Gdiplus::REAL)r.left + (Gdiplus::REAL)(r.right - r.left) * 0.5f;
                if (!isEmpty) {
                    Gdiplus::RectF measured{};
                    g.MeasureString(disp.c_str(), -1, &numFont, Gdiplus::PointF(0, 0), &measured);
                    cx = (Gdiplus::REAL)r.left + ((Gdiplus::REAL)(r.right - r.left) + measured.Width) * 0.5f + 1.0f;
                }
                cx = std::min<Gdiplus::REAL>(cx, (Gdiplus::REAL)(r.right - DpiScale(6, dpi)));
                Gdiplus::Pen caretPen(Gdiplus::Color(235, 248, 248, 250), 1.5f);
                g.DrawLine(&caretPen, cx, (Gdiplus::REAL)(r.top + DpiScale(8, dpi)), cx, (Gdiplus::REAL)(r.bottom - DpiScale(8, dpi)));
            }

            if (subLbl) {
                Gdiplus::RectF subRf((Gdiplus::REAL)r.left, (Gdiplus::REAL)(r.bottom + subLblGap),
                                     (Gdiplus::REAL)(r.right - r.left), (Gdiplus::REAL)subLblH);
                g.DrawString(subLbl, -1, &subFont, subRf, &sfCenter, &mutedBr);
            }
            st->hits.push_back({r, 100 + fieldId, CycleHitRect::InputBox, fieldId});
        };

        auto drawDurationSection = [&](const wchar_t* secTitle, const DurationGroupState& grp, int baseId, int yStart) -> int {
            Gdiplus::RectF sRf((Gdiplus::REAL)pad, (Gdiplus::REAL)yStart, (Gdiplus::REAL)(w - 2 * pad), (Gdiplus::REAL)lblH);
            g.DrawString(secTitle, -1, &secFont, sRf, &sfLeft, &labelBr);
            double ap = grp.animating ? grp.animVal : 0.0;
            if (grp.showHours) drawNumBox(grp.rcH, baseId + 0, grp.txtH, ap, L"h");
            drawNumBox(grp.rcM, baseId + 1, grp.txtM, ap, L"min");
            drawNumBox(grp.rcS, baseId + 2, grp.txtS, ap, L"sec");
            if (st->showMs) drawNumBox(grp.rcMs, baseId + 3, grp.txtMs, ap, L"ms");
            return yStart + lblH + lblGap + boxH + subLblGap + subLblH;
        };

        int y = pad + titleH + titleGap;
        if (st->sideBySideBlockAllow) {
            int allowX = st->allowGrp.rcM.left;
            Gdiplus::RectF bRf((Gdiplus::REAL)pad, (Gdiplus::REAL)y, (Gdiplus::REAL)DpiScale(120, dpi), (Gdiplus::REAL)lblH);
            g.DrawString(L"Block", -1, &secFont, bRf, &sfLeft, &labelBr);
            Gdiplus::RectF aRf((Gdiplus::REAL)allowX, (Gdiplus::REAL)y, (Gdiplus::REAL)DpiScale(120, dpi), (Gdiplus::REAL)lblH);
            g.DrawString(L"Allow", -1, &secFont, aRf, &sfLeft, &labelBr);
            double apB = st->blockGrp.animating ? st->blockGrp.animVal : 0.0;
            double apA = st->allowGrp.animating ? st->allowGrp.animVal : 0.0;
            drawNumBox(st->blockGrp.rcM, CF_BLK_H + 1, st->blockGrp.txtM, apB, L"min");
            drawNumBox(st->blockGrp.rcS, CF_BLK_H + 2, st->blockGrp.txtS, apB, L"sec");
            drawNumBox(st->allowGrp.rcM, CF_ALW_H + 1, st->allowGrp.txtM, apA, L"min");
            drawNumBox(st->allowGrp.rcS, CF_ALW_H + 2, st->allowGrp.txtS, apA, L"sec");
            y += lblH + lblGap + boxH + subLblGap + subLblH + secGap;
        } else {
            y = drawDurationSection(L"Block", st->blockGrp, CF_BLK_H, y) + secGap;
            y = drawDurationSection(L"Allow", st->allowGrp, CF_ALW_H, y) + secGap;
        }

        // Run section
        Gdiplus::RectF runLblRf((Gdiplus::REAL)pad, (Gdiplus::REAL)y, (Gdiplus::REAL)(w - 2 * pad), (Gdiplus::REAL)lblH);
        g.DrawString(L"Run", -1, &secFont, runLblRf, &sfLeft, &labelBr);
        y += lblH + lblGap;

        bool dropOpen = (st->hRunDrop && IsWindow(st->hRunDrop));
        bool dropHov = dropOpen || (st->hoverId == 1);
        FillRoundedRect(g, dropHov ? surfHoverBr : surfRestBr,
                        (Gdiplus::REAL)st->rcRunDrop.left, (Gdiplus::REAL)st->rcRunDrop.top,
                        (Gdiplus::REAL)(st->rcRunDrop.right - st->rcRunDrop.left),
                        (Gdiplus::REAL)(st->rcRunDrop.bottom - st->rcRunDrop.top), rad);
        DrawRoundedRect(g, borderRestPen,
                        (Gdiplus::REAL)st->rcRunDrop.left, (Gdiplus::REAL)st->rcRunDrop.top,
                        (Gdiplus::REAL)(st->rcRunDrop.right - st->rcRunDrop.left),
                        (Gdiplus::REAL)(st->rcRunDrop.bottom - st->rcRunDrop.top), rad);
        int chevW = DpiScale(28, dpi);
        Gdiplus::RectF rTxtRf((Gdiplus::REAL)(st->rcRunDrop.left + DpiScale(12, dpi)),
                              (Gdiplus::REAL)st->rcRunDrop.top,
                              (Gdiplus::REAL)std::max<int>(20, (int)(st->rcRunDrop.right - st->rcRunDrop.left) - chevW - DpiScale(12, dpi)),
                              (Gdiplus::REAL)boxH);
        g.DrawString(RunModeDisplayLabel(st->runModeIdx), -1, &uiFont, rTxtRf, &sfLeft, &whiteBr);
        Gdiplus::RectF rChvRf((Gdiplus::REAL)(st->rcRunDrop.right - chevW - DpiScale(4, dpi)),
                              (Gdiplus::REAL)st->rcRunDrop.top, (Gdiplus::REAL)chevW, (Gdiplus::REAL)boxH);
        g.DrawString(dropOpen ? L"\xE70E" : L"\xE70D", -1, &glyphFont, rChvRf, &sfCenter, &mutedBr);
        st->hits.push_back({st->rcRunDrop, 1, CycleHitRect::RunDropdown, CF_NONE});
        y += boxH;

        if (st->runModeIdx == 1) {
            y += boxGap;
            drawNumBox(st->rcRunCycles, CF_RUN_CYC, st->runCyclesTxt, 0.0, nullptr);
            Gdiplus::RectF cycLbl((Gdiplus::REAL)(st->rcRunCycles.right + DpiScale(10, dpi)),
                                  (Gdiplus::REAL)y, (Gdiplus::REAL)DpiScale(90, dpi), (Gdiplus::REAL)boxH);
            g.DrawString(L"cycles", -1, &uiFont, cycLbl, &sfLeft, &mutedBr);
            y += boxH;
        } else if (st->runModeIdx == 2) {
            y += DpiScale(10, dpi);
            int syncH = DpiScale(22, dpi);
            int sBox = DpiScale(15, dpi);
            int sY = y + (syncH - sBox) / 2;
            RECT syncHit{pad, y, w - pad, y + syncH};
            Gdiplus::REAL chkRad = (Gdiplus::REAL)DpiScale(4, dpi);
            if (st->syncWithWindows) {
                FillRoundedRect(g, terraBr, (Gdiplus::REAL)pad, (Gdiplus::REAL)sY, (Gdiplus::REAL)sBox, (Gdiplus::REAL)sBox, chkRad);
                Gdiplus::RectF ckRf((Gdiplus::REAL)pad, (Gdiplus::REAL)sY, (Gdiplus::REAL)sBox, (Gdiplus::REAL)sBox);
                g.DrawString(L"\xE73E", -1, &glyphFont, ckRf, &sfCenter, &whiteBr);
            } else {
                FillRoundedRect(g, (st->hoverId == 5) ? surfHoverBr : surfRestBr,
                                (Gdiplus::REAL)pad, (Gdiplus::REAL)sY, (Gdiplus::REAL)sBox, (Gdiplus::REAL)sBox, chkRad);
                DrawRoundedRect(g, borderRestPen, (Gdiplus::REAL)pad, (Gdiplus::REAL)sY, (Gdiplus::REAL)sBox, (Gdiplus::REAL)sBox, chkRad);
            }
            int fmtW = DpiScale(42, dpi);
            syncHit.right = w - pad - fmtW - DpiScale(6, dpi);
            Gdiplus::RectF syncTxt((Gdiplus::REAL)(pad + sBox + DpiScale(8, dpi)), (Gdiplus::REAL)y,
                                   (Gdiplus::REAL)(syncHit.right - (pad + sBox + DpiScale(8, dpi))), (Gdiplus::REAL)syncH);
            g.DrawString(L"Sync with Windows clock", -1, &subFont, syncTxt, &sfLeft, &labelBr);
            st->hits.push_back({syncHit, 5, CycleHitRect::SyncClockToggle, CF_NONE});

            if (st->syncWithWindows) {
                RECT fmtRc{w - pad - fmtW, y, w - pad, y + syncH};
                FillRoundedRect(g, (st->hoverId == 6) ? surfHoverBr : surfRestBr,
                                (Gdiplus::REAL)fmtRc.left, (Gdiplus::REAL)fmtRc.top,
                                (Gdiplus::REAL)fmtW, (Gdiplus::REAL)syncH, (Gdiplus::REAL)DpiScale(6, dpi));
                DrawRoundedRect(g, borderRestPen, (Gdiplus::REAL)fmtRc.left, (Gdiplus::REAL)fmtRc.top,
                                (Gdiplus::REAL)fmtW, (Gdiplus::REAL)syncH, (Gdiplus::REAL)DpiScale(6, dpi));
                Gdiplus::RectF fmtRf((Gdiplus::REAL)fmtRc.left, (Gdiplus::REAL)fmtRc.top, (Gdiplus::REAL)fmtW, (Gdiplus::REAL)syncH);
                g.DrawString(st->clock12Hour ? L"12h" : L"24h", -1, &subFont, fmtRf, &sfCenter, &labelBr);
                st->hits.push_back({fmtRc, 6, CycleHitRect::ClockFormat12h, CF_NONE});
            }
            y += syncH + DpiScale(8, dpi);

            if (st->syncWithWindows) {
                drawNumBox(st->rcClkH, CF_CLK_H, st->clkHourTxt, 0.0, st->clock12Hour ? L"hr (12h)" : L"hr (24h)");
                Gdiplus::RectF colRf((Gdiplus::REAL)st->rcClkH.right, (Gdiplus::REAL)y,
                                     (Gdiplus::REAL)(st->rcClkM.left - st->rcClkH.right), (Gdiplus::REAL)boxH);
                g.DrawString(L":", -1, &secFont, colRf, &sfCenter, &mutedBr);
                drawNumBox(st->rcClkM, CF_CLK_M, st->clkMinTxt, 0.0, L"min");
                if (st->clock12Hour && st->rcClkAmPm.right > st->rcClkAmPm.left) {
                    FillRoundedRect(g, surfRestBr,
                                    (Gdiplus::REAL)st->rcClkAmPm.left, (Gdiplus::REAL)st->rcClkAmPm.top,
                                    (Gdiplus::REAL)(st->rcClkAmPm.right - st->rcClkAmPm.left),
                                    (Gdiplus::REAL)(st->rcClkAmPm.bottom - st->rcClkAmPm.top), rad);
                    DrawRoundedRect(g, borderRestPen,
                                    (Gdiplus::REAL)st->rcClkAmPm.left, (Gdiplus::REAL)st->rcClkAmPm.top,
                                    (Gdiplus::REAL)(st->rcClkAmPm.right - st->rcClkAmPm.left),
                                    (Gdiplus::REAL)(st->rcClkAmPm.bottom - st->rcClkAmPm.top), rad);
                    Gdiplus::RectF apRf((Gdiplus::REAL)st->rcClkAmPm.left, (Gdiplus::REAL)st->rcClkAmPm.top,
                                        (Gdiplus::REAL)(st->rcClkAmPm.right - st->rcClkAmPm.left),
                                        (Gdiplus::REAL)(st->rcClkAmPm.bottom - st->rcClkAmPm.top));
                    g.DrawString(st->clockIsPm ? L"PM" : L"AM", -1, &uiFont, apRf, &sfCenter, &labelBr);
                }
                y += boxH + subLblGap + subLblH;
            } else {
                double ap = st->runTimeGrp.animating ? st->runTimeGrp.animVal : 0.0;
                if (st->runTimeGrp.showHours) drawNumBox(st->runTimeGrp.rcH, CF_RUN_H, st->runTimeGrp.txtH, ap, L"h");
                drawNumBox(st->runTimeGrp.rcM, CF_RUN_M, st->runTimeGrp.txtM, ap, L"min");
                drawNumBox(st->runTimeGrp.rcS, CF_RUN_S, st->runTimeGrp.txtS, ap, L"sec");
                if (st->showMs) drawNumBox(st->runTimeGrp.rcMs, CF_RUN_MS, st->runTimeGrp.txtMs, ap, L"ms");
                y += boxH + subLblGap + subLblH;
            }
        }

        // Focus toggle
        y += DpiScale(16, dpi);
        int chkH = DpiScale(22, dpi);
        int boxSz = DpiScale(16, dpi);
        int boxY = y + (chkH - boxSz) / 2;
        RECT chkHit{pad, y, w - pad, y + chkH};
        Gdiplus::REAL chkRad = (Gdiplus::REAL)DpiScale(4, dpi);
        if (st->focusedOnly) {
            FillRoundedRect(g, terraBr, (Gdiplus::REAL)pad, (Gdiplus::REAL)boxY, (Gdiplus::REAL)boxSz, (Gdiplus::REAL)boxSz, chkRad);
            Gdiplus::RectF ckRf((Gdiplus::REAL)pad, (Gdiplus::REAL)boxY, (Gdiplus::REAL)boxSz, (Gdiplus::REAL)boxSz);
            g.DrawString(L"\xE73E", -1, &glyphFont, ckRf, &sfCenter, &whiteBr);
        } else {
            FillRoundedRect(g, (st->hoverId == 2) ? surfHoverBr : surfRestBr,
                            (Gdiplus::REAL)pad, (Gdiplus::REAL)boxY, (Gdiplus::REAL)boxSz, (Gdiplus::REAL)boxSz, chkRad);
            DrawRoundedRect(g, borderRestPen, (Gdiplus::REAL)pad, (Gdiplus::REAL)boxY, (Gdiplus::REAL)boxSz, (Gdiplus::REAL)boxSz, chkRad);
        }
        Gdiplus::RectF chkTxt((Gdiplus::REAL)(pad + boxSz + DpiScale(10, dpi)), (Gdiplus::REAL)y,
                              (Gdiplus::REAL)(w - 2 * pad - boxSz - DpiScale(10, dpi)), (Gdiplus::REAL)chkH);
        g.DrawString(L"Only while app is focused", -1, &uiFont, chkTxt, &sfLeft, &whiteBr);
        st->hits.push_back({chkHit, 2, CycleHitRect::FocusToggle, CF_NONE});
        y += chkH + DpiScale(8, dpi);

        if (st->focusedOnly) {
            int subPad = pad;
            int subBoxY = y + (chkH - boxSz) / 2;
            RECT subHit{subPad, y, w - pad, y + chkH};
            if (st->allowWhenUnfocused) {
                FillRoundedRect(g, terraBr, (Gdiplus::REAL)subPad, (Gdiplus::REAL)subBoxY, (Gdiplus::REAL)boxSz, (Gdiplus::REAL)boxSz, chkRad);
                Gdiplus::RectF subCkRf((Gdiplus::REAL)subPad, (Gdiplus::REAL)subBoxY, (Gdiplus::REAL)boxSz, (Gdiplus::REAL)boxSz);
                g.DrawString(L"\xE73E", -1, &glyphFont, subCkRf, &sfCenter, &whiteBr);
            } else {
                FillRoundedRect(g, (st->hoverId == 7) ? surfHoverBr : surfRestBr,
                                (Gdiplus::REAL)subPad, (Gdiplus::REAL)subBoxY, (Gdiplus::REAL)boxSz, (Gdiplus::REAL)boxSz, chkRad);
                DrawRoundedRect(g, borderRestPen, (Gdiplus::REAL)subPad, (Gdiplus::REAL)subBoxY, (Gdiplus::REAL)boxSz, (Gdiplus::REAL)boxSz, chkRad);
            }
            Gdiplus::RectF subTxt((Gdiplus::REAL)(subPad + boxSz + DpiScale(10, dpi)), (Gdiplus::REAL)y,
                                  (Gdiplus::REAL)(w - pad - (subPad + boxSz + DpiScale(10, dpi))), (Gdiplus::REAL)chkH);
            g.DrawString(L"Allow internet when out of focus", -1, &uiFont, subTxt, &sfLeft, &labelBr);
            st->hits.push_back({subHit, 7, CycleHitRect::AllowUnfocusedToggle, CF_NONE});
            y += chkH + DpiScale(8, dpi);
        }

        // Live summary
        Gdiplus::RectF sumRf((Gdiplus::REAL)st->rcSummary.left, (Gdiplus::REAL)st->rcSummary.top,
                             (Gdiplus::REAL)(st->rcSummary.right - st->rcSummary.left),
                             (Gdiplus::REAL)(st->rcSummary.bottom - st->rcSummary.top));
        g.DrawString(st->summaryText.c_str(), -1, &subFont, sumRf, &sfLeft,
                     (!st->valid || st->warnSub1s) ? &warnBr : &mutedBr);
        y = st->rcSummary.bottom + DpiScale(14, dpi);

        // Buttons
        int btnW = DpiScale(88, dpi);
        int btnH = DpiScale(32, dpi);
        RECT cancelRc{pad, y, pad + DpiScale(72, dpi), y + btnH};
        if (st->hoverId == 3) {
            FillRoundedRect(g, surfHoverBr, (Gdiplus::REAL)cancelRc.left, (Gdiplus::REAL)cancelRc.top,
                            (Gdiplus::REAL)(cancelRc.right - cancelRc.left), (Gdiplus::REAL)btnH, rad);
        }
        Gdiplus::RectF cRf((Gdiplus::REAL)cancelRc.left, (Gdiplus::REAL)cancelRc.top,
                           (Gdiplus::REAL)(cancelRc.right - cancelRc.left), (Gdiplus::REAL)btnH);
        g.DrawString(L"Cancel", -1, &uiFont, cRf, &sfCenter, (st->hoverId == 3) ? &whiteBr : &mutedBr);
        st->hits.push_back({cancelRc, 3, CycleHitRect::CancelBtn, CF_NONE});

        RECT primRc = st->rcBtnPrimary;
        bool pHov = (st->hoverId == 4);
        bool enabled = st->isRunning || st->valid;
        Gdiplus::SolidBrush primBr(
            !enabled ? Gdiplus::Color(255, 42, 42, 48)
            : (st->isRunning
                ? (pHov ? Gdiplus::Color(255, 235, 72, 72) : Gdiplus::Color(255, 215, 55, 55))
                : (pHov ? Gdiplus::Color(255, 218, 118, 82) : Gdiplus::Color(255, 201, 100, 66))));
        FillRoundedRect(g, primBr, (Gdiplus::REAL)primRc.left, (Gdiplus::REAL)primRc.top,
                        (Gdiplus::REAL)btnW, (Gdiplus::REAL)btnH, rad);
        Gdiplus::RectF pRf((Gdiplus::REAL)primRc.left, (Gdiplus::REAL)primRc.top, (Gdiplus::REAL)btnW, (Gdiplus::REAL)btnH);
        g.DrawString(st->isRunning ? L"Stop" : L"Start", -1, &secFont, pRf, &sfCenter, enabled ? &whiteBr : &mutedBr);
        st->hits.push_back({primRc, 4, CycleHitRect::PrimaryBtn, CF_NONE});
    }

    BitBlt(hdcScreen, 0, 0, w, h, memDC, 0, 0, SRCCOPY);
    SelectObject(memDC, oldBmp);
    DeleteObject(hBmp);
    DeleteDC(memDC);
    EndPaint(hwnd, &ps);
}

static void ExecuteCyclePrimaryAction(CyclePopupState* st) {
    if (!st) return;
    if (st->isRunning) {
        StopCycleForApp(st->displayPath, true);
        RefreshAppList();
        RequestRepaint();
        DestroyWindow(st->hwnd);
        return;
    }
    AutoCarryDurationGroup(st, st->blockGrp);
    AutoCarryDurationGroup(st, st->allowGrp);
    if (st->runModeIdx == 2 && !st->syncWithWindows) AutoCarryDurationGroup(st, st->runTimeGrp);
    if (st->runModeIdx == 2 && st->syncWithWindows) CommitClockFields(st);
    UpdateCyclePopupSummary(st);
    if (!st->valid) return;

    CycleConfig cfg;
    cfg.blockMs = st->blockGrp.totalMs;
    cfg.allowMs = st->allowGrp.totalMs;
    cfg.runMode = (CycleRunMode)st->runModeIdx;
    if (st->runModeIdx == 1) {
        cfg.runValue = ParseDigitsU64(st->runCyclesTxt);
    } else if (st->runModeIdx == 2) {
        if (st->syncWithWindows) {
            cfg.runValue = st->computedClockMs;
            cfg.targetEndUtcMs = st->computedTargetUtcMs;
        } else {
            cfg.runValue = st->runTimeGrp.totalMs;
        }
    }
    cfg.focusedOnly = st->focusedOnly;
    cfg.allowWhenUnfocused = st->focusedOnly && st->allowWhenUnfocused;
    cfg.direction = st->direction;

    std::wstring err;
    if (StartCycleForApp(st->displayPath, cfg, err)) {
        RefreshAppList();
        RequestRepaint();
        DestroyWindow(st->hwnd);
    } else {
        st->valid = false;
        st->summaryText = err;
        InvalidateRect(st->hwnd, &st->rcSummary, FALSE);
    }
}

static void CommitActiveDurationCarry(CyclePopupState* st) {
    if (!st) return;
    if (st->focusedField == CF_CLK_H || st->focusedField == CF_CLK_M) {
        CommitClockFields(st);
    }
    if (DurationGroupState* grp = GetDurationGroupByField(st, st->focusedField)) {
        AutoCarryDurationGroup(st, *grp);
    }
}

static void CycleStepField(CyclePopupState* st, bool reverse) {
    if (!st) return;
    std::vector<int> order;
    auto pushGrp = [&](const DurationGroupState& grp, int baseId) {
        if (grp.showHours) order.push_back(baseId + 0);
        order.push_back(baseId + 1);
        order.push_back(baseId + 2);
        if (st->showMs) order.push_back(baseId + 3);
    };
    pushGrp(st->blockGrp, CF_BLK_H);
    pushGrp(st->allowGrp, CF_ALW_H);
    if (st->runModeIdx == 1) order.push_back(CF_RUN_CYC);
    else if (st->runModeIdx == 2) {
        if (st->syncWithWindows) { order.push_back(CF_CLK_H); order.push_back(CF_CLK_M); }
        else pushGrp(st->runTimeGrp, CF_RUN_H);
    }
    if (order.empty()) return;

    int curPos = -1;
    for (int i = 0; i < (int)order.size(); ++i) {
        if (order[i] == st->focusedField) { curPos = i; break; }
    }
    int nextPos = reverse ? ((curPos <= 0) ? (int)order.size() - 1 : curPos - 1)
                          : ((curPos < 0 || curPos + 1 >= (int)order.size()) ? 0 : curPos + 1);

    CommitActiveDurationCarry(st);
    RECT oldRc{};
    GetFieldTextPtr(st, st->focusedField, &oldRc);
    st->focusedField = order[nextPos];
    st->replaceOnType = true;
    st->caretVisible = true;
    RECT newRc{};
    GetFieldTextPtr(st, st->focusedField, &newRc);
    if (oldRc.right > oldRc.left) InvalidateRect(st->hwnd, &oldRc, FALSE);
    if (newRc.right > newRc.left) InvalidateRect(st->hwnd, &newRc, FALSE);
    UpdateCyclePopupSummary(st);
}

static LRESULT CALLBACK CycleRunDropWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    CyclePopupState* st = g_cyclePopupState;
    switch (msg) {
        case WM_CREATE: {
            DWORD pref = DWMWCP_ROUND;
            DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &pref, sizeof(pref));
            MARGINS m{-1, -1, -1, -1};
            DwmExtendFrameIntoClientArea(hwnd, &m);
            ApplyCaptureExclusionToWindow(hwnd);
            return 0;
        }
        case WM_ACTIVATE: {
            if (LOWORD(wParam) == WA_INACTIVE) {
                DestroyWindow(hwnd);
            }
            return 0;
        }
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT: {
            PAINTSTRUCT ps{};
            HDC hdcScreen = BeginPaint(hwnd, &ps);
            RECT rc{};
            GetClientRect(hwnd, &rc);
            int w = rc.right - rc.left;
            int h = rc.bottom - rc.top;
            if (w > 0 && h > 0 && st) {
                UINT dpi = GetDpiForWindow(hwnd);
                HDC memDC = CreateCompatibleDC(hdcScreen);
                BITMAPINFO bmi{};
                bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                bmi.bmiHeader.biWidth = w;
                bmi.bmiHeader.biHeight = -h;
                bmi.bmiHeader.biPlanes = 1;
                bmi.bmiHeader.biBitCount = 32;
                bmi.bmiHeader.biCompression = BI_RGB;
                void* bits = nullptr;
                HBITMAP hBmp = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
                HGDIOBJ oldBmp = SelectObject(memDC, hBmp);
                if (bits) memset(bits, 0, (size_t)w * (size_t)h * 4);

                {
                    Gdiplus::Graphics g(memDC);
                    g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
                    g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
                    g.SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);

                    Gdiplus::SolidBrush bgBr(Gdiplus::Color(255, 20, 20, 25));
                    g.FillRectangle(&bgBr, 0, 0, w, h);
                    Gdiplus::Pen borderPen(Gdiplus::Color(48, 255, 255, 255), 1.0f);
                    g.DrawRectangle(&borderPen, 0.5f, 0.5f, (Gdiplus::REAL)(w - 1), (Gdiplus::REAL)(h - 1));

                    Gdiplus::Font uiFont(L"Segoe UI", (Gdiplus::REAL)DpiScale(13, dpi), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
                    Gdiplus::Font glyphFont(L"Segoe MDL2 Assets", (Gdiplus::REAL)DpiScale(11, dpi), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
                    Gdiplus::SolidBrush whiteBr(Gdiplus::Color(255, 245, 245, 247));
                    Gdiplus::SolidBrush terraBr(Gdiplus::Color(255, 201, 100, 66));
                    Gdiplus::SolidBrush hoverBr(Gdiplus::Color(48, 201, 100, 66));

                    Gdiplus::StringFormat sfLeft;
                    sfLeft.SetAlignment(Gdiplus::StringAlignmentNear);
                    sfLeft.SetLineAlignment(Gdiplus::StringAlignmentCenter);
                    Gdiplus::StringFormat sfCenter;
                    sfCenter.SetAlignment(Gdiplus::StringAlignmentCenter);
                    sfCenter.SetLineAlignment(Gdiplus::StringAlignmentCenter);

                    int pad = DpiScale(6, dpi);
                    int itemH = DpiScale(34, dpi);
                    Gdiplus::REAL rad = (Gdiplus::REAL)DpiScale(6, dpi);
                    for (int i = 0; i < 3; ++i) {
                        int iy = pad + i * itemH;
                        if (i == st->runDropHoverIdx) {
                            FillRoundedRect(g, hoverBr, (Gdiplus::REAL)pad, (Gdiplus::REAL)iy,
                                            (Gdiplus::REAL)(w - 2 * pad), (Gdiplus::REAL)itemH, rad);
                        }
                        if (i == st->runModeIdx) {
                            Gdiplus::RectF ckRf((Gdiplus::REAL)(pad + DpiScale(6, dpi)), (Gdiplus::REAL)iy,
                                                (Gdiplus::REAL)DpiScale(18, dpi), (Gdiplus::REAL)itemH);
                            g.DrawString(L"\xE73E", -1, &glyphFont, ckRf, &sfCenter, &terraBr);
                        }
                        Gdiplus::RectF txtRf((Gdiplus::REAL)(pad + DpiScale(28, dpi)), (Gdiplus::REAL)iy,
                                             (Gdiplus::REAL)(w - 2 * pad - DpiScale(32, dpi)), (Gdiplus::REAL)itemH);
                        g.DrawString(RunModeDisplayLabel(i), -1, &uiFont, txtRf, &sfLeft, &whiteBr);
                    }
                }

                BitBlt(hdcScreen, 0, 0, w, h, memDC, 0, 0, SRCCOPY);
                SelectObject(memDC, oldBmp);
                DeleteObject(hBmp);
                DeleteDC(memDC);
            }
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_MOUSEMOVE: {
            if (!st) break;
            UINT dpi = GetDpiForWindow(hwnd);
            int pad = DpiScale(6, dpi);
            int itemH = DpiScale(34, dpi);
            int y = GET_Y_LPARAM(lParam);
            int idx = (y >= pad) ? ((y - pad) / itemH) : -1;
            if (idx < 0 || idx > 2) idx = -1;
            if (idx != st->runDropHoverIdx) {
                st->runDropHoverIdx = idx;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }
        case WM_LBUTTONUP: {
            if (!st) break;
            UINT dpi = GetDpiForWindow(hwnd);
            int pad = DpiScale(6, dpi);
            int itemH = DpiScale(34, dpi);
            int y = GET_Y_LPARAM(lParam);
            int idx = (y >= pad) ? ((y - pad) / itemH) : -1;
            if (idx >= 0 && idx <= 2) {
                st->runModeIdx = idx;
                if (idx == 2 && st->syncWithWindows) PopulateClockFromWindows(st);
                LayoutCyclePopup(st, GetDpiForWindow(st->hwnd), true);
                UpdateCyclePopupSummary(st);
            }
            SetForegroundWindow(st->hwnd);
            DestroyWindow(hwnd);
            return 0;
        }
        case WM_KEYDOWN: {
            if (!st) break;
            if (wParam == VK_ESCAPE) {
                SetForegroundWindow(st->hwnd);
                DestroyWindow(hwnd);
                return 0;
            }
            if (wParam == VK_UP) {
                st->runDropHoverIdx = (st->runDropHoverIdx <= 0) ? 2 : st->runDropHoverIdx - 1;
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            }
            if (wParam == VK_DOWN) {
                st->runDropHoverIdx = (st->runDropHoverIdx < 0 || st->runDropHoverIdx >= 2) ? 0 : st->runDropHoverIdx + 1;
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            }
            if (wParam == VK_RETURN) {
                if (st->runDropHoverIdx >= 0 && st->runDropHoverIdx <= 2) {
                    st->runModeIdx = st->runDropHoverIdx;
                    if (st->runModeIdx == 2 && st->syncWithWindows) PopulateClockFromWindows(st);
                    LayoutCyclePopup(st, GetDpiForWindow(st->hwnd), true);
                    UpdateCyclePopupSummary(st);
                }
                SetForegroundWindow(st->hwnd);
                DestroyWindow(hwnd);
                return 0;
            }
            break;
        }
        case WM_DESTROY:
            if (st) {
                st->hRunDrop = nullptr;
                if (st->hwnd && IsWindow(st->hwnd)) {
                    InvalidateRect(st->hwnd, &st->rcRunDrop, FALSE);
                }
            }
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static void OpenCustomRunDropdown(CyclePopupState* st) {
    if (!st || !st->hwnd) return;
    if (st->hRunDrop && IsWindow(st->hRunDrop)) {
        DestroyWindow(st->hRunDrop);
        return;
    }

    static bool dropReg = false;
    if (!dropReg) {
        WNDCLASSEXW dwc{sizeof(dwc)};
        dwc.lpfnWndProc = CycleRunDropWndProc;
        dwc.hInstance = GetModuleHandleW(nullptr);
        dwc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        dwc.lpszClassName = kCycleRunDropClass;
        RegisterClassExW(&dwc);
        dropReg = true;
    }

    UINT dpi = GetDpiForWindow(st->hwnd);
    int pad = DpiScale(6, dpi);
    int itemH = DpiScale(34, dpi);
    int dropW = st->rcRunDrop.right - st->rcRunDrop.left;
    int dropH = pad * 2 + 3 * itemH;
    POINT pt{st->rcRunDrop.left, st->rcRunDrop.bottom + DpiScale(4, dpi)};
    ClientToScreen(st->hwnd, &pt);

    st->runDropHoverIdx = st->runModeIdx;
    st->openingRunDrop = true;
    st->hRunDrop = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        kCycleRunDropClass, L"", WS_POPUP,
        pt.x, pt.y, dropW, dropH,
        st->hwnd, nullptr, GetModuleHandleW(nullptr), nullptr);
    st->openingRunDrop = false;
    if (st->hRunDrop) {
        ShowWindow(st->hRunDrop, SW_SHOW);
        SetForegroundWindow(st->hRunDrop);
        SetFocus(st->hRunDrop);
        InvalidateRect(st->hwnd, &st->rcRunDrop, FALSE);
    }
}

static LRESULT CALLBACK CyclePopupWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    CyclePopupState* st = g_cyclePopupState;
    switch (msg) {
        case WM_CREATE: {
            BOOL dark = TRUE;
            DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));
            DWORD pref = DWMWCP_ROUND;
            DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &pref, sizeof(pref));
            MARGINS m{-1, -1, -1, -1};
            DwmExtendFrameIntoClientArea(hwnd, &m);
            int backdrop = DWMSBT_TRANSIENTWINDOW;
            HRESULT hrBd = DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdrop, sizeof(backdrop));
            if (st) st->hasBackdropBlur = SUCCEEDED(hrBd);
            SetTimer(hwnd, TIMER_CYCLE_POPUP_CARET, 520, nullptr);
            return 0;
        }
        case WM_NCACTIVATE:
            return TRUE;
        case WM_MOUSEACTIVATE:
            SetForegroundWindow(hwnd);
            SetFocus(hwnd);
            return MA_ACTIVATE;
        case WM_SETFOCUS:
        case WM_KILLFOCUS:
            if (st && st->focusedField != CF_NONE) {
                RECT fRc{};
                if (GetFieldTextPtr(st, st->focusedField, &fRc) && fRc.right > fRc.left) {
                    InvalidateRect(hwnd, &fRc, FALSE);
                }
            }
            return 0;
        case WM_ACTIVATE: {
            if (LOWORD(wParam) == WA_INACTIVE && st) {
                if (st->openingRunDrop || (st->hRunDrop && IsWindow(st->hRunDrop))) return 0;
                PostMessageW(hwnd, WM_CLOSE, 0, 0);
            }
            return 0;
        }
        case WM_DPICHANGED: {
            if (st) LayoutCyclePopup(st, HIWORD(wParam));
            return 0;
        }
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT:
            PaintCyclePopup(hwnd, st);
            return 0;
        case WM_TIMER: {
            if (!st) return 0;
            if (wParam == TIMER_CYCLE_POPUP_RESIZE) {
                bool more = st->heightClock.Step();
                int curH = (int)std::round(st->heightClock.currentVal);
                SetWindowPos(hwnd, HWND_TOPMOST, st->targetX, st->targetY, st->targetW, curH, SWP_NOACTIVATE);
                if (!more) {
                    KillTimer(hwnd, TIMER_CYCLE_POPUP_RESIZE);
                    SetWindowPos(hwnd, HWND_TOPMOST, st->targetX, st->targetY, st->targetW, st->targetH, SWP_NOACTIVATE);
                }
            } else if (wParam == TIMER_CYCLE_POPUP_OPEN) {
                bool more = st->openClock.Step();
                double p = st->openClock.currentVal;
                BYTE alpha = (BYTE)std::clamp((int)std::round(p * 255.0), 0, 255);
                int offsetY = (int)std::round((1.0 - p) * (double)DpiScale(10, GetDpiForWindow(hwnd)));
                SetLayeredWindowAttributes(hwnd, 0, alpha, LWA_ALPHA);
                SetWindowPos(hwnd, HWND_TOPMOST, st->targetX, st->targetY + offsetY, 0, 0,
                             SWP_NOSIZE | SWP_NOACTIVATE);
                if (!more) {
                    KillTimer(hwnd, TIMER_CYCLE_POPUP_OPEN);
                    SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
                    SetWindowPos(hwnd, HWND_TOPMOST, st->targetX, st->targetY, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
                }
            } else if (wParam == TIMER_CYCLE_CLOCK_COMMIT) {
                CommitClockFields(st);
                UpdateCyclePopupSummary(st);
            } else if (wParam == TIMER_CYCLE_POPUP_CARET) {
                if (st->focusedField != CF_NONE) {
                    st->caretVisible = !st->caretVisible;
                    RECT fRc{};
                    if (GetFieldTextPtr(st, st->focusedField, &fRc) && fRc.right > fRc.left) {
                        InvalidateRect(hwnd, &fRc, FALSE);
                    }
                }
            } else if (wParam == TIMER_CYCLE_POPUP_ANIM) {
                LARGE_INTEGER freq{}, now{};
                QueryPerformanceFrequency(&freq);
                QueryPerformanceCounter(&now);
                auto stepAnim = [&](DurationGroupState& grp) -> bool {
                    if (!grp.animating) return false;
                    double ms = (double)(now.QuadPart - grp.animStart.QuadPart) * 1000.0 / (double)freq.QuadPart;
                    if (ms >= 120.0) { grp.animVal = 0.0; grp.animating = false; return false; }
                    grp.animVal = 1.0 - (ms / 120.0);
                    return true;
                };
                bool m1 = stepAnim(st->blockGrp);
                bool m2 = stepAnim(st->allowGrp);
                bool m3 = stepAnim(st->runTimeGrp);
                auto invGrp = [&](const DurationGroupState& grp) {
                    if (grp.rcM.right <= grp.rcM.left) return;
                    RECT r{grp.showHours ? grp.rcH.left : grp.rcM.left, grp.rcM.top,
                           st->showMs ? grp.rcMs.right : grp.rcS.right, grp.rcS.bottom};
                    InvalidateRect(hwnd, &r, FALSE);
                };
                if (m1 || st->blockGrp.animVal == 0.0) invGrp(st->blockGrp);
                if (m2 || st->allowGrp.animVal == 0.0) invGrp(st->allowGrp);
                if (m3 || st->runTimeGrp.animVal == 0.0) invGrp(st->runTimeGrp);
                if (!(m1 || m2 || m3)) KillTimer(hwnd, TIMER_CYCLE_POPUP_ANIM);
            }
            return 0;
        }
        case WM_MOUSEMOVE: {
            if (!st) break;
            TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, hwnd, 0};
            TrackMouseEvent(&tme);
            POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            int newHover = -1;
            RECT newHoverRc{};
            for (const auto& h : st->hits) {
                if (h.kind != CycleHitRect::InputBox && PtInRect(&h.rc, pt)) {
                    newHover = h.id;
                    newHoverRc = h.rc;
                    break;
                }
            }
            if (newHover != st->hoverId) {
                if (st->hoverId != -1 && st->hoverRc.right > st->hoverRc.left) {
                    InvalidateRect(hwnd, &st->hoverRc, FALSE);
                }
                st->hoverId = newHover;
                st->hoverRc = newHoverRc;
                if (st->hoverId != -1 && st->hoverRc.right > st->hoverRc.left) {
                    InvalidateRect(hwnd, &st->hoverRc, FALSE);
                }
            }
            return 0;
        }
        case WM_MOUSELEAVE: {
            if (st && st->hoverId != -1) {
                if (st->hoverRc.right > st->hoverRc.left) InvalidateRect(hwnd, &st->hoverRc, FALSE);
                st->hoverId = -1;
                st->hoverRc = {};
            }
            return 0;
        }
        case WM_LBUTTONDOWN: {
            if (!st) break;
            SetForegroundWindow(hwnd);
            SetFocus(hwnd);
            POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            for (const auto& h : st->hits) {
                if (!PtInRect(&h.rc, pt)) continue;
                if (h.kind == CycleHitRect::InputBox) {
                    if (st->focusedField != h.fieldId) {
                        CommitActiveDurationCarry(st);
                        RECT oldRc{};
                        GetFieldTextPtr(st, st->focusedField, &oldRc);
                        st->focusedField = h.fieldId;
                        if (oldRc.right > oldRc.left) InvalidateRect(hwnd, &oldRc, FALSE);
                        UpdateCyclePopupSummary(st);
                    }
                    st->replaceOnType = true;
                    st->caretVisible = true;
                    InvalidateRect(hwnd, &h.rc, FALSE);
                    return 0;
                }
                CommitActiveDurationCarry(st);
                if (h.kind == CycleHitRect::RunDropdown) {
                    OpenCustomRunDropdown(st);
                } else if (h.kind == CycleHitRect::SyncClockToggle) {
                    st->syncWithWindows = !st->syncWithWindows;
                    st->clockEditing = false;
                    if (st->syncWithWindows) {
                        PopulateClockFromWindows(st);
                        st->focusedField = CF_CLK_H;
                        st->replaceOnType = true;
                    }
                    LayoutCyclePopup(st, GetDpiForWindow(hwnd), true);
                    UpdateCyclePopupSummary(st);
                } else if (h.kind == CycleHitRect::ClockFormat12h) {
                    int curH24 = ClockTo24Hour(ParseDigitsU64(st->clkHourTxt), st->clock12Hour, st->clockIsPm);
                    st->clock12Hour = !st->clock12Hour;
                    if (curH24 >= 0) {
                        wchar_t bH[8]{};
                        if (st->clock12Hour) {
                            st->clockIsPm = (curH24 >= 12);
                            int h12 = curH24 % 12; if (h12 == 0) h12 = 12;
                            swprintf_s(bH, L"%02d", h12);
                        } else {
                            swprintf_s(bH, L"%02d", curH24);
                        }
                        st->clkHourTxt = bH;
                    } else {
                        PopulateClockFromWindows(st);
                    }
                    st->clockEditing = false;
                    LayoutCyclePopup(st, GetDpiForWindow(hwnd), false);
                    UpdateCyclePopupSummary(st);
                } else if (h.kind == CycleHitRect::FocusToggle) {
                    st->focusedOnly = !st->focusedOnly;
                    LayoutCyclePopup(st, GetDpiForWindow(hwnd), true);
                    UpdateCyclePopupSummary(st);
                } else if (h.kind == CycleHitRect::AllowUnfocusedToggle) {
                    st->allowWhenUnfocused = !st->allowWhenUnfocused;
                    InvalidateRect(hwnd, &h.rc, FALSE);
                    UpdateCyclePopupSummary(st);
                } else if (h.kind == CycleHitRect::CancelBtn) {
                    DestroyWindow(hwnd);
                } else if (h.kind == CycleHitRect::PrimaryBtn) {
                    ExecuteCyclePrimaryAction(st);
                }
                return 0;
            }
            CommitActiveDurationCarry(st);
            UpdateCyclePopupSummary(st);
            return 0;
        }
        case WM_CHAR: {
            if (!st || st->focusedField == CF_NONE) break;
            if (wParam >= L'0' && wParam <= L'9') {
                RECT fRc{};
                if (std::wstring* pTxt = GetFieldTextPtr(st, st->focusedField, &fRc)) {
                    if (st->replaceOnType) {
                        pTxt->clear();
                        st->replaceOnType = false;
                    } else if (st->focusedField != CF_CLK_H && st->focusedField != CF_CLK_M && *pTxt == L"0") {
                        pTxt->clear();
                    }
                    size_t maxLen = (st->focusedField == CF_CLK_H || st->focusedField == CF_CLK_M) ? 2 : 6;
                    if (st->focusedField == CF_CLK_H || st->focusedField == CF_CLK_M) {
                        st->clockEditing = true;
                        SetTimer(hwnd, TIMER_CYCLE_CLOCK_COMMIT, 1500, nullptr);
                    }
                    if (pTxt->size() < maxLen) {
                        pTxt->push_back((wchar_t)wParam);
                        st->caretVisible = true;
                        InvalidateRect(hwnd, &fRc, FALSE);
                        UpdateCyclePopupSummary(st);
                        if (st->focusedField == CF_CLK_H && pTxt->size() == 2) {
                            CycleStepField(st, false);
                            if (st->focusedField == CF_CLK_M) {
                                st->clockEditing = true;
                                SetTimer(hwnd, TIMER_CYCLE_CLOCK_COMMIT, 1500, nullptr);
                            }
                        }
                    }
                }
                return 0;
            }
            return 0;
        }
        case WM_KEYDOWN: {
            if (!st) break;
            if (wParam == VK_ESCAPE) {
                DestroyWindow(hwnd);
                return 0;
            }
            if (wParam == VK_RETURN) {
                st->clockEditing = false;
                UpdateCyclePopupSummary(st);
                ExecuteCyclePrimaryAction(st);
                return 0;
            }
            if (wParam == VK_TAB || wParam == VK_LEFT || wParam == VK_RIGHT) {
                bool rev = (wParam == VK_LEFT) || (wParam == VK_TAB && (GetKeyState(VK_SHIFT) & 0x8000) != 0);
                CycleStepField(st, rev);
                return 0;
            }
            if (wParam == VK_DELETE && st->focusedField != CF_NONE) {
                RECT fRc{};
                if (std::wstring* pTxt = GetFieldTextPtr(st, st->focusedField, &fRc)) {
                    pTxt->clear();
                    st->replaceOnType = false;
                    if (st->focusedField == CF_CLK_H || st->focusedField == CF_CLK_M) {
                        st->clockEditing = true;
                        SetTimer(hwnd, TIMER_CYCLE_CLOCK_COMMIT, 1500, nullptr);
                    }
                    st->caretVisible = true;
                    InvalidateRect(hwnd, &fRc, FALSE);
                    UpdateCyclePopupSummary(st);
                }
                return 0;
            }
            if (wParam == VK_BACK && st->focusedField != CF_NONE) {
                RECT fRc{};
                if (std::wstring* pTxt = GetFieldTextPtr(st, st->focusedField, &fRc)) {
                    st->replaceOnType = false;
                    if (st->focusedField == CF_CLK_H || st->focusedField == CF_CLK_M) {
                        st->clockEditing = true;
                        SetTimer(hwnd, TIMER_CYCLE_CLOCK_COMMIT, 1500, nullptr);
                    }
                    if (!pTxt->empty()) pTxt->pop_back();
                    st->caretVisible = true;
                    InvalidateRect(hwnd, &fRc, FALSE);
                    UpdateCyclePopupSummary(st);
                }
                return 0;
            }
            if ((wParam == VK_UP || wParam == VK_DOWN) && st->focusedField != CF_NONE) {
                RECT fRc{};
                if (std::wstring* pTxt = GetFieldTextPtr(st, st->focusedField, &fRc)) {
                    int64_t val = (int64_t)ParseDigitsU64(*pTxt) + ((wParam == VK_UP) ? 1 : -1);
                    if (val < 0) val = 0;
                    if (st->focusedField == CF_CLK_H) {
                        val = st->clock12Hour ? std::clamp<int64_t>(val, 1, 12) : std::clamp<int64_t>(val, 0, 23);
                        st->clockEditing = false;
                    } else if (st->focusedField == CF_CLK_M) {
                        val = std::clamp<int64_t>(val, 0, 59);
                        st->clockEditing = false;
                    }
                    *pTxt = std::to_wstring(val);
                    st->replaceOnType = false;
                    st->caretVisible = true;
                    if (DurationGroupState* grp = GetDurationGroupByField(st, st->focusedField)) {
                        AutoCarryDurationGroup(st, *grp);
                    }
                    InvalidateRect(hwnd, &fRc, FALSE);
                    UpdateCyclePopupSummary(st);
                }
                return 0;
            }
            break;
        }
        case WM_DESTROY: {
            KillTimer(hwnd, TIMER_CYCLE_POPUP_ANIM);
            KillTimer(hwnd, TIMER_CYCLE_POPUP_CARET);
            KillTimer(hwnd, TIMER_CYCLE_POPUP_OPEN);
            KillTimer(hwnd, TIMER_CYCLE_POPUP_RESIZE);
            KillTimer(hwnd, TIMER_CYCLE_CLOCK_COMMIT);
            g_hCyclePopup = nullptr;
            if (st) {
                if (st->hRunDrop && IsWindow(st->hRunDrop)) DestroyWindow(st->hRunDrop);
                delete st;
                g_cyclePopupState = nullptr;
            }
            return 0;
        }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static void OpenCyclePopupForApp(const AppEntry& app) {
    bool schedEnabled = false;
    bool showMs = false;
    {
        std::lock_guard<std::mutex> lk(g_settingsMtx);
        schedEnabled = g_settings.enableCycleScheduler;
        showMs = g_settings.showCycleMs;
    }
    if (!schedEnabled) {
        g_statusBanner = L"Cycle scheduler is off \u2014 enable it in Windhawk Settings.";
        RequestRepaint();
        return;
    }
    if (app.isExcluded || app.isExternalBlocked) {
        g_statusBanner = app.isExcluded ? L"App is excluded from !Net." : L"Blocked by Windows Firewall \u2014 cannot cycle.";
        RequestRepaint();
        return;
    }
    {
        std::lock_guard<std::mutex> lk(g_settingsMtx);
        if (g_settings.lockSystemProcesses && app.isCriticalLocked) {
            g_statusBanner = L"Not allowed on locked system processes.";
            RequestRepaint();
            return;
        }
    }

    if (g_hCyclePopup && IsWindow(g_hCyclePopup)) {
        DestroyWindow(g_hCyclePopup);
    }

    static bool regDone = false;
    if (!regDone) {
        WNDCLASSEXW pwc{sizeof(pwc)};
        pwc.lpfnWndProc = CyclePopupWndProc;
        pwc.hInstance = GetModuleHandleW(nullptr);
        pwc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        pwc.lpszClassName = kCyclePopupClass;
        RegisterClassExW(&pwc);
        regDone = true;
    }

    auto* st = new CyclePopupState();
    st->exePath = app.exePath;
    st->displayPath = app.displayPath;
    st->appName = app.name;
    st->direction = app.block.direction;
    st->showMs = showMs;

    CycleStatusSnapshot activeSnap;
    st->isRunning = GetCycleSnapshot(app.exePath, activeSnap);

    CycleConfig defCfg;
    if (st->isRunning) {
        defCfg.blockMs = activeSnap.blockMs;
        defCfg.allowMs = activeSnap.allowMs;
        defCfg.runMode = activeSnap.runMode;
        defCfg.runValue = (activeSnap.runMode == CycleRunMode::ForCycles) ? activeSnap.targetCycles : activeSnap.totalRemainingMs;
        defCfg.focusedOnly = activeSnap.focusedOnly;
        defCfg.allowWhenUnfocused = activeSnap.allowWhenUnfocused;
    } else if (g_hasLastCycleConfig) {
        defCfg = g_lastCycleConfig;
    }

    WriteDurationGroupMs(st->blockGrp, defCfg.blockMs, false);
    WriteDurationGroupMs(st->allowGrp, defCfg.allowMs, false);
    st->runModeIdx = (int)defCfg.runMode;
    st->runCyclesTxt = std::to_wstring((defCfg.runMode == CycleRunMode::ForCycles && defCfg.runValue > 0) ? defCfg.runValue : 5ULL);
    WriteDurationGroupMs(st->runTimeGrp,
                         (defCfg.runMode == CycleRunMode::ForDuration && defCfg.runValue >= 100) ? defCfg.runValue : 300000ULL,
                         false);
    PopulateClockFromWindows(st);
    st->focusedOnly = defCfg.focusedOnly;
    st->allowWhenUnfocused = defCfg.allowWhenUnfocused;
    st->focusedField = CF_BLK_S;
    st->replaceOnType = true;

    g_cyclePopupState = st;

    POINT pt{};
    GetCursorPos(&pt);
    // Create hidden first, lay out to exact DPI size, then show without flash
    HWND hPopup = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED,
        kCyclePopupClass, L"Cycle Mode", WS_POPUP,
        pt.x, pt.y, 260, 260,
        g_hWnd, nullptr, GetModuleHandleW(nullptr), nullptr);
    if (!hPopup) {
        delete st;
        g_cyclePopupState = nullptr;
        return;
    }
    st->hwnd = hPopup;
    g_hCyclePopup = hPopup;

    LayoutCyclePopup(st, GetDpiForWindow(hPopup));
    UpdateCyclePopupSummary(st, false);
    st->openClock.Init();
    st->openClock.currentVal = 0.0;
    st->openClock.Start(1.0, 160.0);
    SetLayeredWindowAttributes(hPopup, 0, 0, LWA_ALPHA);
    SetWindowPos(hPopup, HWND_TOPMOST, st->targetX, st->targetY + DpiScale(10, GetDpiForWindow(hPopup)),
                 st->targetW, st->targetH, SWP_NOACTIVATE);
    ShowWindow(hPopup, SW_SHOW);
    SetForegroundWindow(hPopup);
    SetFocus(hPopup);
    SetTimer(hPopup, TIMER_CYCLE_POPUP_OPEN, 10, nullptr);
}


// ============================================================================
// Cycle Scheduler Clock Widget (Near Collapsed Pill)
// ============================================================================

constexpr wchar_t kCycleWidgetClass[] = L"ExclaimNet_CycleWidget_v1";
constexpr UINT_PTR TIMER_WIDGET_SINGLE_CLICK = 82;
constexpr UINT_PTR TIMER_WIDGET_TICK         = 83;
constexpr UINT_PTR TIMER_NAVIGATE_OPEN_CYCLE = 84;
constexpr UINT_PTR TIMER_WIDGET_REVEAL_ANIM  = 85;
static HWND g_hWidgetTooltip = nullptr;
static bool g_cycleWidgetHover = false;
static bool g_cycleWidgetPressed = false;
static bool g_widgetRevealed = false;
static MotionClock g_widgetRevealClock;
static std::wstring g_widgetClickExe;
static AppEntry g_pendingNavigateCycleApp;
static bool g_hasPendingNavigateCycle = false;

static bool IsExeEligibleForCycle(const std::wstring& exeLower) {
    if (exeLower.empty()) return false;
    if (g_excludedExes.count(exeLower)) return false;
    bool lockCrit = true;
    {
        std::lock_guard<std::mutex> lk(g_settingsMtx);
        lockCrit = g_settings.lockSystemProcesses;
    }
    if (lockCrit && IsCriticalSystemExe(exeLower)) return false;
    if (g_extFwScanTick == 0) ScanExternalFirewallBlocks(false);
    std::lock_guard<std::mutex> lk(g_extFwMtx);
    auto itExt = g_extFwBlocks.find(exeLower);
    if (itExt != g_extFwBlocks.end() && itExt->second.dirMask != 0) return false;
    return true;
}

static bool ShouldWidgetBeAvailable() {
    bool showWidget = true;
    bool schedEnabled = false;
    {
        std::lock_guard<std::mutex> lk(g_settingsMtx);
        showWidget = g_settings.showCycleWidget;
        schedEnabled = g_settings.enableCycleScheduler;
    }
    if (!showWidget || !schedEnabled || g_uiState != UiState::Line || g_hiddenByFullscreen) return false;
    return HasAnyActiveCycle() || IsExeEligibleForCycle(g_lastFgExe);
}

static void UpdateWidgetTooltipText(HWND hwnd) {
    if (!g_hWidgetTooltip) return;
    bool showTips = false;
    {
        std::lock_guard<std::mutex> lk(g_settingsMtx);
        showTips = g_settings.showTooltips;
    }
    std::wstring tip;
    if (showTips) {
        CycleStatusSnapshot snap;
        if (GetWidgetCycleTarget(snap)) {
            std::wstring appName = GetFileNameOnly(snap.displayPath);
            if (snap.pausedByFocus || snap.pausedByUser) {
                tip = appName + L" \u00b7 Paused \u00b7 Click to resume, double-click to open";
            } else {
                tip = appName + L" \u00b7 " + (snap.phase == CyclePhase::Blocked ? L"Blocked " : L"Allowed ") +
                      FormatDurationCompact(snap.phaseRemainingMs) + L" left \u00b7 Click to pause, double-click to open";
            }
        } else if (!g_lastFgExe.empty()) {
            tip = GetFileNameOnly(g_lastFgExe) + L" \u00b7 Click to schedule Cycle Mode";
        } else {
            tip = L"!Net Cycle Scheduler \u00b7 Focus an app or click to open";
        }
    }
    TOOLINFOW ti{sizeof(ti)};
    ti.hwnd = hwnd;
    ti.uId = (UINT_PTR)hwnd;
    ti.lpszText = const_cast<LPWSTR>(tip.c_str());
    SendMessageW(g_hWidgetTooltip, TTM_UPDATETIPTEXTW, 0, (LPARAM)&ti);
}

static void NavigatePillAndOpenCycle(const std::wstring& targetExeLower) {
    if (!g_hWnd) return;
    KillTimer(g_hWnd, TIMER_COLLAPSE);
    g_uiState = UiState::Panel;
    g_userPinnedFilter = false;
    SyncForegroundStateAndFilter(true);
    RefreshAppList();
    auto findIdx = [&]() -> int {
        for (int i = 0; i < (int)g_appList.size(); ++i) {
            if (g_appList[i].exePath == targetExeLower) return i;
        }
        return -1;
    };
    int foundIdx = findIdx();
    if (foundIdx < 0 && !targetExeLower.empty()) {
        g_activeFilter = IsSystemPath(targetExeLower) ? AppFilter::SystemApps : AppFilter::UserApps;
        g_userRunningOnly = false;
        g_statusBanner.clear();
        RefreshAppList();
        foundIdx = findIdx();
    }
    if (foundIdx >= 0) {
        g_kbSelectedIdx = foundIdx;
        UINT dpi = GetDpiForWindow(g_hWnd);
        int rowStride = (g_lastRowStride > 0) ? g_lastRowStride : DpiScale(54, dpi);
        int listH = (g_lastListH > 0) ? g_lastListH : DpiScale(GetExpandedLogicalSize().cy - 110, dpi);
        int topPx = foundIdx * rowStride;
        int botPx = topPx + rowStride;
        double curScroll = g_scrollClock.toVal;
        if (topPx < (int)curScroll) {
            g_scrollClock.Start((double)topPx, 90.0);
        } else if (botPx > (int)curScroll + listH) {
            double nextS = std::min((double)g_maxScrollPx, (double)(botPx - listH));
            g_scrollClock.Start(nextS, 90.0);
        }
        g_pendingNavigateCycleApp = g_appList[foundIdx];
        g_hasPendingNavigateCycle = true;
        SetTimer(g_hWnd, TIMER_NAVIGATE_OPEN_CYCLE, 250, nullptr);
    }
    UpdateWindowGeometry(false);
    SetForegroundWindow(g_hWnd);
}

static void OpenCycleTargetFromWidget(const CycleStatusSnapshot* activeSnap) {
    CycleWidgetAction action = CycleWidgetAction::SchedulerOnly;
    Direction defDir = Direction::FullyOffline;
    {
        std::lock_guard<std::mutex> lk(g_settingsMtx);
        action = g_settings.cycleWidgetAction;
        defDir = g_settings.defaultDirection;
    }
    AppEntry targetApp;
    if (activeSnap) {
        targetApp.exePath = activeSnap->exePath;
        targetApp.displayPath = activeSnap->displayPath;
        targetApp.name = GetFileNameOnly(activeSnap->displayPath);
        targetApp.block.direction = activeSnap->direction;
    } else if (!g_lastFgExe.empty()) {
        targetApp.exePath = g_lastFgExe;
        targetApp.displayPath = g_lastFgExe;
        targetApp.name = GetFileNameOnly(g_lastFgExe);
        targetApp.isExcluded = g_excludedExes.count(g_lastFgExe) > 0;
        targetApp.isCriticalLocked = IsCriticalSystemExe(g_lastFgExe);
        auto extFw = GetCachedExternalFirewallBlocks();
        auto itExt = extFw.find(g_lastFgExe);
        if (itExt != extFw.end() && itExt->second.dirMask != 0) targetApp.isExternalBlocked = true;
        auto itDir = g_pendingDirs.find(g_lastFgExe);
        targetApp.block.direction = (itDir != g_pendingDirs.end()) ? itDir->second : defDir;
    } else {
        if (g_hWnd) {
            g_uiState = UiState::Panel;
            g_userPinnedFilter = false;
            SyncForegroundStateAndFilter(true);
            RefreshAppList();
            UpdateWindowGeometry(false);
            SetForegroundWindow(g_hWnd);
        }
        return;
    }
    if (action == CycleWidgetAction::NavigatePill) {
        NavigatePillAndOpenCycle(targetApp.exePath);
    } else {
        OpenCyclePopupForApp(targetApp);
        if ((!g_hCyclePopup || !IsWindow(g_hCyclePopup)) && g_uiState == UiState::Line && !g_statusBanner.empty() && g_hWnd) {
            g_uiState = UiState::Panel;
            RefreshAppList();
            UpdateWindowGeometry(false);
        }
    }
}

static void ComputeCycleWidgetRect(RECT& outRc, double revealProgress = 1.0) {
    UINT dpi = g_hWnd ? GetDpiForWindow(g_hWnd) : 96;
    ModSettings s;
    {
        std::lock_guard<std::mutex> lk(g_settingsMtx);
        s = g_settings;
    }

    RECT pillRc{};
    ComputeTargetRect(0.0, pillRc);

    HMONITOR mon = GetTargetMonitor(s.followCursorMonitor);
    MONITORINFO mi{sizeof(mi)};
    GetMonitorInfoW(mon, &mi);
    RECT wa = mi.rcWork;

    int w = DpiScale(24, dpi);
    int h = DpiScale(20, dpi);
    int gap = DpiScale(4, dpi);
    int slideOff = (int)std::round((1.0 - std::clamp(revealProgress, 0.0, 1.0)) * (double)DpiScale(6, dpi));
    int x = 0;
    int y = 0;

    switch (s.dockEdge) {
        case DockEdge::Left:
            x = wa.left - slideOff;
            y = pillRc.bottom + gap;
            if (y + h > wa.bottom) y = pillRc.top - gap - h;
            break;
        case DockEdge::Top:
            x = pillRc.right + gap;
            if (x + w > wa.right) x = pillRc.left - gap - w;
            y = wa.top - slideOff;
            break;
        case DockEdge::Bottom:
            x = pillRc.right + gap;
            if (x + w > wa.right) x = pillRc.left - gap - w;
            y = wa.bottom - h + slideOff;
            break;
        case DockEdge::TopLeft: {
            int d = DpiScale(21, dpi);
            x = wa.left + d - w / 2;
            y = wa.top + d - h / 2 - slideOff;
            break;
        }
        case DockEdge::BottomLeft: {
            int d = DpiScale(21, dpi);
            x = wa.left + d - w / 2;
            y = wa.bottom - d - h / 2 + slideOff;
            break;
        }
        case DockEdge::BottomRight: {
            int d = DpiScale(21, dpi);
            x = wa.right - d - w / 2;
            y = wa.bottom - d - h / 2 + slideOff;
            break;
        }
        default:
            x = wa.right - w + slideOff;
            y = pillRc.bottom + gap;
            if (y + h > wa.bottom) y = pillRc.top - gap - h;
            break;
    }

    x = std::clamp(x, (int)wa.left - slideOff, (int)wa.right - w + slideOff);
    y = std::clamp(y, (int)wa.top - slideOff, (int)wa.bottom - h + slideOff);
    outRc = {x, y, x + w, y + h};
}

static void RenderCycleWidgetLayered(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return;
    RECT wrc{};
    ComputeCycleWidgetRect(wrc, g_widgetRevealClock.currentVal);
    int w = wrc.right - wrc.left;
    int h = wrc.bottom - wrc.top;
    if (w <= 0 || h <= 0) return;

    UINT dpi = GetDpiForWindow(hwnd);
    HDC hdcScreen = GetDC(nullptr);
    HDC memDC = CreateCompatibleDC(hdcScreen);
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP hBmp = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    HGDIOBJ oldBmp = SelectObject(memDC, hBmp);

    // 1/255 alpha keeps the entire hit rect clickable with zero visible border or fill
    if (bits) {
        uint32_t* px = static_cast<uint32_t*>(bits);
        size_t total = (size_t)w * (size_t)h;
        for (size_t i = 0; i < total; ++i) px[i] = 0x01000000u;
    }

    {
        Gdiplus::Graphics g(memDC);
        g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
        g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
        g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);

        CycleStatusSnapshot snap;
        bool hasSnap = GetWidgetCycleTarget(snap);
        bool isPaused = hasSnap && (snap.pausedByFocus || snap.pausedByUser);

        bool isLightMode = g_isLightTheme;
        BYTE r = isLightMode ? 0 : 255;
        BYTE gr = isLightMode ? 0 : 255;
        BYTE b = isLightMode ? 0 : 255;
        if (hasSnap) {
            if (isPaused) {
                r = isLightMode ? 90 : 178;
                gr = isLightMode ? 94 : 184;
                b = isLightMode ? 104 : 196;
            } else if (snap.phase == CyclePhase::Blocked) {
                r = 225; gr = 58; b = 58;
            } else {
                r = 34; gr = 175; b = 85;
            }
        }

        BYTE alpha = (BYTE)std::clamp((int)std::round(g_widgetRevealClock.currentVal * 255.0), 0, 255);
        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
        Gdiplus::Color col(alpha, r, gr, b);
        Gdiplus::REAL scale = (Gdiplus::REAL)dpi / 96.0f;
        Gdiplus::REAL cx = (Gdiplus::REAL)w * 0.5f;
        Gdiplus::REAL cy = (Gdiplus::REAL)h * 0.5f + (g_cycleWidgetPressed ? 1.0f * scale : 0.0f);
        Gdiplus::Pen pen(col, std::max(1.5f, 1.55f * scale));
        pen.SetStartCap(Gdiplus::LineCapRound);
        pen.SetEndCap(Gdiplus::LineCapRound);
        if (hasSnap && isPaused) {
            Gdiplus::REAL dx = 2.6f * scale;
            Gdiplus::REAL hy = 4.2f * scale;
            g.DrawLine(&pen, cx - dx, cy - hy, cx - dx, cy + hy);
            g.DrawLine(&pen, cx + dx, cy - hy, cx + dx, cy + hy);
        } else {
            Gdiplus::REAL rad = 5.4f * scale;
            g.DrawEllipse(&pen, cx - rad, cy - rad + 0.6f * scale, rad * 2.0f, rad * 2.0f);
            g.DrawLine(&pen, cx, cy - rad - 1.4f * scale, cx, cy - rad + 0.3f * scale);
            g.DrawLine(&pen, cx, cy + 0.6f * scale, cx, cy - 2.3f * scale);
            g.DrawLine(&pen, cx, cy + 0.6f * scale, cx + 2.6f * scale, cy + 1.8f * scale);
        }
    }

    POINT ptDst{wrc.left, wrc.top};
    SIZE sz{w, h};
    POINT ptSrc{0, 0};
    BLENDFUNCTION bf{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(hwnd, hdcScreen, &ptDst, &sz, memDC, &ptSrc, 0, &bf, ULW_ALPHA);

    SelectObject(memDC, oldBmp);
    DeleteObject(hBmp);
    DeleteDC(memDC);
    ReleaseDC(nullptr, hdcScreen);
}

static LRESULT CALLBACK CycleWidgetWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            ApplyCaptureExclusionToWindow(hwnd);
            g_hWidgetTooltip = CreateWindowExW(WS_EX_TOPMOST, TOOLTIPS_CLASSW, nullptr,
                                               WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
                                               CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                               hwnd, nullptr, GetModuleHandleW(nullptr), nullptr);
            if (g_hWidgetTooltip) {
                TOOLINFOW ti{sizeof(ti)};
                ti.uFlags = TTF_SUBCLASS | TTF_IDISHWND;
                ti.hwnd = hwnd;
                ti.uId = (UINT_PTR)hwnd;
                ti.lpszText = const_cast<LPWSTR>(L"");
                SendMessageW(g_hWidgetTooltip, TTM_ADDTOOLW, 0, (LPARAM)&ti);
                SendMessageW(g_hWidgetTooltip, TTM_SETMAXTIPWIDTH, 0, 320);
                SendMessageW(g_hWidgetTooltip, TTM_SETDELAYTIME, TTDT_INITIAL, 300);
            }
            return 0;
        }
        case WM_NCACTIVATE:
            return TRUE;
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT: {
            PAINTSTRUCT ps{};
            BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            RenderCycleWidgetLayered(hwnd);
            return 0;
        }
        case WM_DPICHANGED:
            SyncCycleWidgetWindow(true);
            return 0;
        case WM_MOUSEMOVE: {
            if (!g_cycleWidgetHover) {
                g_cycleWidgetHover = true;
                TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, hwnd, 0};
                TrackMouseEvent(&tme);
                UpdateWidgetTooltipText(hwnd);
                RenderCycleWidgetLayered(hwnd);
            }
            return 0;
        }
        case WM_MOUSELEAVE: {
            g_cycleWidgetHover = false;
            g_cycleWidgetPressed = false;
            if (g_hWidgetTooltip) {
                TOOLINFOW ti{sizeof(ti)};
                ti.hwnd = hwnd;
                ti.uId = (UINT_PTR)hwnd;
                ti.lpszText = const_cast<LPWSTR>(L"");
                SendMessageW(g_hWidgetTooltip, TTM_UPDATETIPTEXTW, 0, (LPARAM)&ti);
            }
            RenderCycleWidgetLayered(hwnd);
            return 0;
        }
        case WM_LBUTTONDOWN: {
            g_cycleWidgetPressed = true;
            RenderCycleWidgetLayered(hwnd);
            return 0;
        }
        case WM_LBUTTONUP: {
            g_cycleWidgetPressed = false;
            RenderCycleWidgetLayered(hwnd);
            CycleStatusSnapshot snap;
            if (GetWidgetCycleTarget(snap)) {
                g_widgetClickExe = snap.exePath;
                SetTimer(hwnd, TIMER_WIDGET_SINGLE_CLICK, GetDoubleClickTime(), nullptr);
            } else if (!g_hCyclePopup || !IsWindow(g_hCyclePopup)) {
                OpenCycleTargetFromWidget(nullptr);
            }
            return 0;
        }
        case WM_LBUTTONDBLCLK: {
            KillTimer(hwnd, TIMER_WIDGET_SINGLE_CLICK);
            g_cycleWidgetPressed = false;
            RenderCycleWidgetLayered(hwnd);
            CycleStatusSnapshot snap;
            if (GetWidgetCycleTarget(snap)) {
                OpenCycleTargetFromWidget(&snap);
            } else if (!g_hCyclePopup || !IsWindow(g_hCyclePopup)) {
                OpenCycleTargetFromWidget(nullptr);
            }
            return 0;
        }
        case WM_TIMER: {
            if (wParam == TIMER_WIDGET_SINGLE_CLICK) {
                KillTimer(hwnd, TIMER_WIDGET_SINGLE_CLICK);
                if (!g_widgetClickExe.empty()) {
                    bool nowPaused = false;
                    ToggleCyclePauseForApp(g_widgetClickExe, nowPaused);
                }
                RenderCycleWidgetLayered(hwnd);
            } else if (wParam == TIMER_WIDGET_REVEAL_ANIM) {
                bool more = g_widgetRevealClock.Step();
                RenderCycleWidgetLayered(hwnd);
                if (!more) {
                    KillTimer(hwnd, TIMER_WIDGET_REVEAL_ANIM);
                    if (!g_widgetRevealed && IsWindowVisible(hwnd)) {
                        ShowWindow(hwnd, SW_HIDE);
                    }
                }
            } else if (wParam == TIMER_WIDGET_TICK) {
                if (!HasAnyActiveCycle()) {
                    KillTimer(hwnd, TIMER_WIDGET_TICK);
                    if (IsWindowVisible(hwnd)) RenderCycleWidgetLayered(hwnd);
                } else if (IsWindowVisible(hwnd)) {
                    if (g_cycleWidgetHover) UpdateWidgetTooltipText(hwnd);
                    RenderCycleWidgetLayered(hwnd);
                }
            }
            return 0;
        }
        case WM_DESTROY: {
            KillTimer(hwnd, TIMER_WIDGET_SINGLE_CLICK);
            KillTimer(hwnd, TIMER_WIDGET_TICK);
            KillTimer(hwnd, TIMER_WIDGET_REVEAL_ANIM);
            g_widgetRevealed = false;
            if (g_hWidgetTooltip) { DestroyWindow(g_hWidgetTooltip); g_hWidgetTooltip = nullptr; }
            g_cycleWidgetHover = false;
            g_cycleWidgetPressed = false;
            g_hCycleWidget = nullptr;
            return 0;
        }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static void DestroyCycleWidgetWindow() {
    if (g_hCycleWidget && IsWindow(g_hCycleWidget)) {
        DestroyWindow(g_hCycleWidget);
    }
    g_hCycleWidget = nullptr;
}

static void SetCycleWidgetRevealed(bool reveal) {
    if (!ShouldWidgetBeAvailable()) reveal = false;
    if (reveal == g_widgetRevealed && (!reveal || (g_hCycleWidget && IsWindowVisible(g_hCycleWidget)))) {
        return;
    }
    g_widgetRevealed = reveal;
    if (reveal && (!g_hCycleWidget || !IsWindow(g_hCycleWidget))) {
        static bool widgetReg = false;
        if (!widgetReg) {
            WNDCLASSEXW wwc{sizeof(wwc)};
            wwc.style = CS_DBLCLKS;
            wwc.lpfnWndProc = CycleWidgetWndProc;
            wwc.hInstance = GetModuleHandleW(nullptr);
            wwc.hCursor = LoadCursorW(nullptr, IDC_HAND);
            wwc.lpszClassName = kCycleWidgetClass;
            RegisterClassExW(&wwc);
            widgetReg = true;
        }
        g_widgetRevealClock.Init();
        g_widgetRevealClock.currentVal = 0.0;
        RECT wrc{};
        ComputeCycleWidgetRect(wrc, 0.0);
        g_hCycleWidget = CreateWindowExW(
            WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED,
            kCycleWidgetClass, L"!Net Cycle Scheduler Widget", WS_POPUP,
            wrc.left, wrc.top, wrc.right - wrc.left, wrc.bottom - wrc.top,
            nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
        if (!g_hCycleWidget) return;
    }
    if (!g_hCycleWidget || !IsWindow(g_hCycleWidget)) return;

    if (reveal) {
        ShowWindow(g_hCycleWidget, SW_SHOWNOACTIVATE);
        g_widgetRevealClock.Start(1.0, 180.0);
        SetTimer(g_hCycleWidget, TIMER_WIDGET_REVEAL_ANIM, 10, nullptr);
        if (HasAnyActiveCycle()) SetTimer(g_hCycleWidget, TIMER_WIDGET_TICK, 500, nullptr);
        RenderCycleWidgetLayered(g_hCycleWidget);
    } else {
        KillTimer(g_hCycleWidget, TIMER_WIDGET_TICK);
        if (IsWindowVisible(g_hCycleWidget)) {
            g_widgetRevealClock.Start(0.0, 160.0);
            SetTimer(g_hCycleWidget, TIMER_WIDGET_REVEAL_ANIM, 10, nullptr);
        }
    }
}

static void SyncCycleWidgetWindow(bool reposition) {
    bool available = ShouldWidgetBeAvailable();
    if (g_hWnd) {
        if (available) SetTimer(g_hWnd, TIMER_WIDGET_PROXIMITY, 75, nullptr);
        else KillTimer(g_hWnd, TIMER_WIDGET_PROXIMITY);
    }
    if (!available) {
        if (g_hCycleWidget && IsWindow(g_hCycleWidget) && IsWindowVisible(g_hCycleWidget)) {
            if (g_uiState != UiState::Line || g_hiddenByFullscreen) {
                KillTimer(g_hCycleWidget, TIMER_WIDGET_REVEAL_ANIM);
                KillTimer(g_hCycleWidget, TIMER_WIDGET_TICK);
                g_widgetRevealed = false;
                g_widgetRevealClock.currentVal = 0.0;
                ShowWindow(g_hCycleWidget, SW_HIDE);
            } else {
                SetCycleWidgetRevealed(false);
            }
        }
        return;
    }
    if (g_hCycleWidget && IsWindow(g_hCycleWidget) && IsWindowVisible(g_hCycleWidget)) {
        if (HasAnyActiveCycle()) SetTimer(g_hCycleWidget, TIMER_WIDGET_TICK, 500, nullptr);
        else KillTimer(g_hCycleWidget, TIMER_WIDGET_TICK);
        if (reposition) RenderCycleWidgetLayered(g_hCycleWidget);
    }
}

static LRESULT CALLBACK FloatingWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (g_wmTaskbarCreated && msg == g_wmTaskbarCreated) {
        g_trayAdded = false;
        AddTrayIcon(hwnd);
        return 0;
    }
    switch (msg) {
        case WM_CREATE: {
            g_wmTaskbarCreated = RegisterWindowMessageW(L"TaskbarCreated");
            if (g_wmTaskbarCreated) ChangeWindowMessageFilterEx(hwnd, g_wmTaskbarCreated, MSGFLT_ALLOW, nullptr);
            ChangeWindowMessageFilterEx(hwnd, WM_APP_TRAY_CALLBACK, MSGFLT_ALLOW, nullptr);
            RefreshLightThemeCache();
            SetWindowFrameMode(hwnd, false);
            ApplyCaptureExclusionToWindow(hwnd);
            RegisterAllHotkeys(hwnd);
            AddTrayIcon(hwnd);
            InitTooltip(hwnd);
            if (!IsProcessElevated()) {
                g_needsElevation = true;
                g_statusBanner = L"Admin permission needed to block apps.";
            }
            SetTimer(hwnd, TIMER_FULLSCREEN, 1000, nullptr);
            SetTimer(hwnd, TIMER_LATE_CHILD_POLL, 3000, nullptr);
            SyncHoverSensorTimer(hwnd);
            return 0;
        }
        case WM_NCACTIVATE:
            return TRUE;
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT:
            PaintFloatingWindow(hwnd);
            return 0;
        case WM_DPICHANGED:
            ClearIconCache();
            UpdateWindowGeometry(true);
            return 0;
        case WM_MOUSEMOVE: {
            if (!g_mouseInside) {
                g_mouseInside = true;
                TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, hwnd, 0};
                TrackMouseEvent(&tme);
                KillTimer(hwnd, TIMER_COLLAPSE);
                bool hoverExp = true;
                {
                    std::lock_guard<std::mutex> lk(g_settingsMtx);
                    hoverExp = g_settings.hoverExpand;
                }
                if (hoverExp && g_uiState == UiState::Line) {
                    g_uiState = UiState::Panel;
                    g_userPinnedFilter = false;
                    SyncForegroundStateAndFilter(true);
                    RefreshAppList();
                    UpdateWindowGeometry(false);
                }
            }
            if (g_drag.dragging) {
                POINT cur{};
                GetCursorPos(&cur);
                RECT rc = g_drag.startRect;
                int w = rc.right - rc.left;
                int h = rc.bottom - rc.top;
                HMONITOR mon = MonitorFromPoint(cur, MONITOR_DEFAULTTONEAREST);
                MONITORINFO mi{sizeof(mi)};
                GetMonitorInfoW(mon, &mi);
                int nx = (g_drag.axisMode == 1) ? rc.left : std::clamp((int)(cur.x - g_drag.cursorAnchor.x), (int)mi.rcWork.left, (int)mi.rcWork.right - w);
                int ny = (g_drag.axisMode == 2) ? rc.top  : std::clamp((int)(cur.y - g_drag.cursorAnchor.y), (int)mi.rcWork.top, (int)mi.rcWork.bottom - h);
                SetWindowPos(hwnd, HWND_TOPMOST, nx, ny, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW);
                return 0;
            }
            POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            const HitRect* hr = HitTestControl(pt);
            int newHover = hr ? hr->id : -1;
            if (newHover != g_hoverHitId) {
                g_hoverHitId = newHover;
                UpdateTooltipText(hwnd, hr ? hr->tooltip.c_str() : L"");
                RequestRepaint();
            }
            return 0;
        }
        case WM_ACTIVATE: {
            if (LOWORD(wParam) == WA_INACTIVE && !g_pinned && g_uiState == UiState::Panel && !g_drag.dragging) {
                HWND newActive = reinterpret_cast<HWND>(lParam);
                if ((!g_hCyclePopup || !IsWindow(g_hCyclePopup)) && (!g_hCycleWidget || newActive != g_hCycleWidget)) {
                    g_hasPendingNavigateCycle = false;
                    KillTimer(hwnd, TIMER_NAVIGATE_OPEN_CYCLE);
                    KillTimer(hwnd, TIMER_OUTSIDE_CLICK);
                    g_uiState = UiState::Line;
                    g_kbSelectedIdx = -1;
                    g_userPinnedFilter = false;
                    UpdateWindowGeometry(false);
                }
            }
            return 0;
        }
        case WM_MOUSELEAVE: {
            g_mouseInside = false;
            g_hoverHitId = -1;
            UpdateTooltipText(hwnd, L"");
            return 0;
        }
        case WM_LBUTTONDOWN: {
            if (g_hCyclePopup && IsWindow(g_hCyclePopup)) DestroyWindow(g_hCyclePopup);
            POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            const HitRect* hr = HitTestControl(pt);
            if (hr && hr->kind == HitRect::HeaderDrag) {
                SetCapture(hwnd);
                g_drag.dragging = true;
                POINT cur{};
                GetCursorPos(&cur);
                GetWindowRect(hwnd, &g_drag.startRect);
                g_drag.cursorAnchor = {cur.x - g_drag.startRect.left, cur.y - g_drag.startRect.top};
                g_drag.axisMode = 0;
                return 0;
            }
            if (hr) {
                if (hr->rowIndex >= 0 && (hr->kind == HitRect::RowToggle || hr->kind == HitRect::RowDirection ||
                                          hr->kind == HitRect::RowExclude || hr->kind == HitRect::RowExternalLocked)) {
                    g_kbSelectedIdx = hr->rowIndex;
                }
                g_pressedHitId = hr->id;
                HandleClickAction(*hr);
            }
            if (g_uiState == UiState::Panel) SetForegroundWindow(hwnd);
            return 0;
        }
        case WM_LBUTTONDBLCLK: {
            if (g_uiState != UiState::Panel) break;
            POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            const HitRect* hr = HitTestControl(pt);
            if (hr && hr->rowIndex >= 0 && hr->rowIndex < (int)g_appList.size() &&
                (hr->kind == HitRect::RowBody || hr->kind == HitRect::RowDirection)) {
                OpenCyclePopupForApp(g_appList[hr->rowIndex]);
            }
            return 0;
        }
        case WM_RBUTTONUP: {
            if (g_uiState != UiState::Panel) break;
            POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            const HitRect* hr = HitTestControl(pt);
            if (hr && hr->rowIndex >= 0 && hr->rowIndex < (int)g_appList.size()) {
                AppEntry app = g_appList[hr->rowIndex];
                HMENU menu = CreatePopupMenu();
                CycleStatusSnapshot cyc;
                bool active = GetCycleSnapshot(app.exePath, cyc);
                AppendMenuW(menu, MF_STRING, 1, L"Cycle...");
                if (active) AppendMenuW(menu, MF_STRING, 2, L"Stop Cycle");
                POINT scr = pt;
                ClientToScreen(hwnd, &scr);
                SetForegroundWindow(hwnd);
                int cmd = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON, scr.x, scr.y, 0, hwnd, nullptr);
                DestroyMenu(menu);
                if (cmd == 1) OpenCyclePopupForApp(app);
                else if (cmd == 2) {
                    StopCycleForApp(app.displayPath, true);
                    RefreshAppList();
                    RequestRepaint();
                }
            }
            return 0;
        }
        case WM_LBUTTONUP: {
            if (g_drag.dragging) {
                g_drag.dragging = false;
                ReleaseCapture();
                RECT rc{};
                GetWindowRect(hwnd, &rc);
                HMONITOR mon = MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);
                MONITORINFO mi{sizeof(mi)};
                GetMonitorInfoW(mon, &mi);
                RECT wa = mi.rcWork;
                UINT dpi = GetDpiForWindow(hwnd);
                int cx = (rc.left + rc.right) / 2;
                int cy = (rc.top + rc.bottom) / 2;
                int dLeft   = std::max(0, (int)(cx - wa.left));
                int dRight  = std::max(0, (int)(wa.right - cx));
                int dTop    = std::max(0, (int)(cy - wa.top));
                int dBottom = std::max(0, (int)(wa.bottom - cy));
                int cornerZone = DpiScale(96, dpi);
                DockEdge snapped = DockEdge::Top;
                if (dTop < cornerZone && dLeft < cornerZone) {
                    snapped = DockEdge::TopLeft;
                } else if (dBottom < cornerZone && dLeft < cornerZone) {
                    snapped = DockEdge::BottomLeft;
                } else if (dBottom < cornerZone && dRight < cornerZone) {
                    snapped = DockEdge::BottomRight;
                } else {
                    int best = dTop; snapped = DockEdge::Top;
                    if (dBottom < best) { best = dBottom; snapped = DockEdge::Bottom; }
                    if (dLeft < best)   { best = dLeft;   snapped = DockEdge::Left; }
                    if (dRight < best)  { best = dRight;  snapped = DockEdge::Right; }
                }
                {
                    std::lock_guard<std::mutex> lk(g_settingsMtx);
                    g_settings.dockEdge = snapped;
                }
                Wh_SetIntValue(L"dockEdgeOverride", (int)snapped);
                g_posX = rc.left;
                g_posY = rc.top;
                g_posLoaded = true;
                Wh_SetIntValue(L"posX", rc.left);
                Wh_SetIntValue(L"posY", rc.top);
                UpdateWindowGeometry(false);
            }
            g_pressedHitId = -1;
            return 0;
        }
        case WM_CAPTURECHANGED: {
            if (g_drag.dragging) {
                g_drag.dragging = false;
                SetWindowPos(hwnd, HWND_TOPMOST, g_drag.startRect.left, g_drag.startRect.top,
                             0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
            }
            return 0;
        }
        case WM_MOUSEWHEEL: {
            UINT dpi = GetDpiForWindow(hwnd);
            int delta = GET_WHEEL_DELTA_WPARAM(wParam);
            double stepPx = (double)DpiScale(72, dpi) * ((double)delta / (double)WHEEL_DELTA);
            double nextTarget = std::clamp(g_scrollClock.toVal - stepPx, 0.0, (double)g_maxScrollPx);
            g_scrollClock.Start(nextTarget, 75.0);
            SetTimer(hwnd, TIMER_ANIM_VSYNC, 10, nullptr);
            return 0;
        }
        case WM_TIMER: {
            if (wParam == TIMER_COLLAPSE) {
                KillTimer(hwnd, TIMER_COLLAPSE);
                if (!g_mouseInside && !g_pinned && g_uiState != UiState::Line) {
                    g_hasPendingNavigateCycle = false;
                    KillTimer(hwnd, TIMER_NAVIGATE_OPEN_CYCLE);
                    g_uiState = UiState::Line;
                    g_kbSelectedIdx = -1;
                    g_userPinnedFilter = false;
                    UpdateWindowGeometry(false);
                }
            } else if (wParam == TIMER_ANIM_VSYNC) {
                bool wasExpandActive = g_expandClock.active;
                bool moreExpand = g_expandClock.Step();
                bool moreScroll = g_scrollClock.Step();
                bool morePulse  = g_pillPulse.Step();
                if (wasExpandActive || moreExpand) {
                    RECT rc{};
                    ComputeTargetRect(g_expandClock.currentVal, rc);
                    SetWindowPos(hwnd, HWND_TOPMOST, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
                                 SWP_NOACTIVATE | SWP_NOCOPYBITS);
                }
                if (wasExpandActive && !moreExpand && g_uiState == UiState::Line) {
                    SetWindowFrameMode(hwnd, false);
                    SyncCycleWidgetWindow(true);
                }
                RequestRepaint();
                if (!moreExpand && !moreScroll && !morePulse) {
                    KillTimer(hwnd, TIMER_ANIM_VSYNC);
                }
            } else if (wParam == TIMER_HOVER_SENSOR) {
                ModSettings s;
                {
                    std::lock_guard<std::mutex> lk(g_settingsMtx);
                    s = g_settings;
                }
                if (g_uiState == UiState::Line && s.hoverExpand && s.expandHoverHitArea &&
                    !g_hiddenByFullscreen && !g_drag.dragging) {
                    POINT cur{};
                    GetCursorPos(&cur);
                    RECT rc{};
                    GetWindowRect(hwnd, &rc);
                    UINT dpi = GetDpiForWindow(hwnd);
                    int m = DpiScale(s.hoverHitMarginPx, dpi);
                    InflateRect(&rc, m, m);
                    bool overWidget = false;
                    if (g_hCycleWidget && IsWindowVisible(g_hCycleWidget)) {
                        RECT wrc{};
                        GetWindowRect(g_hCycleWidget, &wrc);
                        overWidget = g_cycleWidgetHover || PtInRect(&wrc, cur);
                    }
                    if (!overWidget && PtInRect(&rc, cur)) {
                        KillTimer(hwnd, TIMER_HOVER_SENSOR);
                        KillTimer(hwnd, TIMER_COLLAPSE);
                        g_uiState = UiState::Panel;
                        g_userPinnedFilter = false;
                        SyncForegroundStateAndFilter(true);
                        RefreshAppList();
                        UpdateWindowGeometry(false);
                    }
                }
            } else if (wParam == TIMER_WIDGET_PROXIMITY) {
                bool followMon = false;
                { std::lock_guard<std::mutex> lk(g_settingsMtx); followMon = g_settings.followCursorMonitor; }
                if (followMon && g_uiState == UiState::Line && !g_drag.dragging && !g_hiddenByFullscreen) {
                    if (GetTargetMonitor(true) != MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST)) {
                        UpdateWindowGeometry(true);
                    }
                }
                if (!ShouldWidgetBeAvailable()) {
                    SetCycleWidgetRevealed(false);
                    KillTimer(hwnd, TIMER_WIDGET_PROXIMITY);
                } else {
                    POINT cur{};
                    GetCursorPos(&cur);
                    RECT pillRc{}, wrc{};
                    ComputeTargetRect(0.0, pillRc);
                    ComputeCycleWidgetRect(wrc, 1.0);
                    RECT zone{};
                    UnionRect(&zone, &pillRc, &wrc);
                    UINT dpi = GetDpiForWindow(hwnd);
                    int prox = DpiScale(42, dpi);
                    InflateRect(&zone, prox, prox);
                    bool inZone = g_cycleWidgetHover || PtInRect(&zone, cur);
                    SetCycleWidgetRevealed(inZone);
                }
            } else if (wParam == TIMER_LATE_CHILD_POLL) {
                SyncLateChildBlocks();
            } else if (wParam == TIMER_DEBOUNCE_REFRESH) {
                KillTimer(hwnd, TIMER_DEBOUNCE_REFRESH);
                SyncLateChildBlocks();
                if (g_uiState != UiState::Line) {
                    RefreshAppList();
                    RequestRepaint();
                }
            } else if (wParam == TIMER_OUTSIDE_CLICK) {
                if (g_uiState != UiState::Panel || g_pinned) {
                    KillTimer(hwnd, TIMER_OUTSIDE_CLICK);
                } else if (!g_drag.dragging && !(g_hCyclePopup && IsWindow(g_hCyclePopup))) {
                    bool btnDown = ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0) ||
                                   ((GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0);
                    if (btnDown) {
                        POINT cur{};
                        GetCursorPos(&cur);
                        RECT prc{};
                        GetWindowRect(hwnd, &prc);
                        bool overWidget = false;
                        if (g_hCycleWidget && IsWindowVisible(g_hCycleWidget)) {
                            RECT wrc{};
                            GetWindowRect(g_hCycleWidget, &wrc);
                            overWidget = PtInRect(&wrc, cur);
                        }
                        if (!PtInRect(&prc, cur) && !overWidget) {
                            KillTimer(hwnd, TIMER_OUTSIDE_CLICK);
                            g_hasPendingNavigateCycle = false;
                            KillTimer(hwnd, TIMER_NAVIGATE_OPEN_CYCLE);
                            g_uiState = UiState::Line;
                            g_kbSelectedIdx = -1;
                            g_userPinnedFilter = false;
                            UpdateWindowGeometry(false);
                        }
                    }
                }
            } else if (wParam == TIMER_NAVIGATE_OPEN_CYCLE) {
                KillTimer(hwnd, TIMER_NAVIGATE_OPEN_CYCLE);
                if (g_hasPendingNavigateCycle && g_uiState == UiState::Panel) {
                    g_hasPendingNavigateCycle = false;
                    OpenCyclePopupForApp(g_pendingNavigateCycleApp);
                } else {
                    g_hasPendingNavigateCycle = false;
                }
            } else if (wParam == TIMER_CYCLE_UI) {
                if (g_uiState == UiState::Panel && HasAnyActiveCycle()) {
                    RequestRepaint();
                } else {
                    KillTimer(hwnd, TIMER_CYCLE_UI);
                }
            } else if (wParam == TIMER_FULLSCREEN) {
                FullscreenBehavior fb = FullscreenBehavior::Hide;
                {
                    std::lock_guard<std::mutex> lk(g_settingsMtx);
                    fb = g_settings.fullscreenBehavior;
                }
                bool shouldHide = (fb == FullscreenBehavior::Hide) && IsFullscreenActive();
                if (shouldHide && g_hCyclePopup && IsWindow(g_hCyclePopup)) {
                    DestroyWindow(g_hCyclePopup);
                }
                if (shouldHide && IsWindowVisible(hwnd)) {
                    ShowWindow(hwnd, SW_HIDE);
                    g_hiddenByFullscreen = true;
                    SyncCycleWidgetWindow(false);
                } else if (!shouldHide && g_hiddenByFullscreen) {
                    ShowWindow(hwnd, SW_SHOWNOACTIVATE);
                    g_hiddenByFullscreen = false;
                    SyncCycleWidgetWindow(false);
                }
            }
            return 0;
        }
        case WM_KEYDOWN: {
            if (g_uiState != UiState::Panel) break;
            if (wParam == VK_ESCAPE) {
                g_hasPendingNavigateCycle = false;
                KillTimer(hwnd, TIMER_NAVIGATE_OPEN_CYCLE);
                g_uiState = UiState::Line;
                g_kbSelectedIdx = -1;
                g_userPinnedFilter = false;
                UpdateWindowGeometry(false);
                return 0;
            }
            if (g_appList.empty()) return 0;
            auto ensureVisible = [&](int idx) {
                int topPx = idx * g_lastRowStride;
                int botPx = topPx + g_lastRowStride;
                double curScroll = g_scrollClock.toVal;
                if (topPx < (int)curScroll) {
                    g_scrollClock.Start((double)topPx, 90.0);
                    SetTimer(hwnd, TIMER_ANIM_VSYNC, 10, nullptr);
                } else if (botPx > (int)curScroll + g_lastListH) {
                    double nextS = std::min((double)g_maxScrollPx, (double)(botPx - g_lastListH));
                    g_scrollClock.Start(nextS, 90.0);
                    SetTimer(hwnd, TIMER_ANIM_VSYNC, 10, nullptr);
                }
            };
            if (wParam == VK_DOWN) {
                g_kbSelectedIdx = (g_kbSelectedIdx < 0) ? 0 : std::min((int)g_appList.size() - 1, g_kbSelectedIdx + 1);
                ensureVisible(g_kbSelectedIdx);
                RequestRepaint();
                return 0;
            } else if (wParam == VK_UP) {
                g_kbSelectedIdx = (g_kbSelectedIdx <= 0) ? 0 : g_kbSelectedIdx - 1;
                ensureVisible(g_kbSelectedIdx);
                RequestRepaint();
                return 0;
            } else if ((wParam == VK_RETURN || wParam == VK_SPACE) && g_kbSelectedIdx >= 0 && g_kbSelectedIdx < (int)g_appList.size()) {
                HitRect syn{{}, 0, g_kbSelectedIdx, L"", HitRect::RowToggle};
                HandleClickAction(syn);
                return 0;
            } else if ((wParam == VK_LEFT || wParam == VK_RIGHT) && g_kbSelectedIdx >= 0 && g_kbSelectedIdx < (int)g_appList.size()) {
                HitRect syn{{}, 0, g_kbSelectedIdx, L"", HitRect::RowDirection};
                HandleClickAction(syn);
                return 0;
            }
            break;
        }
        case WM_HOTKEY: {
            int hkId = (int)wParam;
            if (hkId == HK_TOGGLE_FG) {
                HWND fg = GetForegroundWindow();
                DWORD pid = 0;
                if (fg && fg != hwnd) GetWindowThreadProcessId(fg, &pid);
                std::wstring exe = pid ? QueryProcessExePath(pid) : L"";
                if (!exe.empty()) {
                    std::wstring low = ToLower(exe);
                    auto extFw = ScanExternalFirewallBlocks();
                    auto itExt = extFw.find(low);
                    if (itExt != extFw.end() && itExt->second.dirMask != 0) {
                        g_activeFilter = AppFilter::Blocked;
                        g_blockedFirewallOnly = true;
                        g_statusBanner = L"Blocked by Windows Firewall — edit rules in Windows Defender Firewall.";
                        TriggerPulse(PulseKind::BlockedRed);
                        if (g_uiState == UiState::Line) {
                            g_uiState = UiState::Panel;
                            RefreshAppList();
                            UpdateWindowGeometry(false);
                        } else {
                            RefreshAppList();
                            RequestRepaint();
                        }
                        return 0;
                    }
                    bool isBlocked = false;
                    {
                        std::lock_guard<std::mutex> lk(g_engineMtx);
                        auto it = g_blocks.find(low);
                        isBlocked = (it != g_blocks.end() && it->second.blocked);
                    }
                    if (isBlocked) ApplyAllow(exe);
                    else {
                        ModSettings s;
                        {
                            std::lock_guard<std::mutex> lk(g_settingsMtx);
                            s = g_settings;
                        }
                        auto itDir = g_pendingDirs.find(low);
                        Direction dir = (itDir != g_pendingDirs.end()) ? itDir->second : s.defaultDirection;
                        ApplyBlock(exe, dir, s.includeChildProcesses, true);
                    }
                    RefreshAppList();
                    RequestRepaint();
                }
            } else if (hkId == HK_EXPAND_PILL) {
                g_uiState = (g_uiState == UiState::Line) ? UiState::Panel : UiState::Line;
                if (g_uiState == UiState::Panel) {
                    g_userPinnedFilter = false;
                    SyncForegroundStateAndFilter(true);
                    RefreshAppList();
                    g_kbSelectedIdx = g_appList.empty() ? -1 : 0;
                    SetForegroundWindow(hwnd);
                } else {
                    g_kbSelectedIdx = -1;
                    g_userPinnedFilter = false;
                }
                UpdateWindowGeometry(false);
            } else if (hkId == HK_RESTORE_ALL) {
                RestoreAllSessionBlocks();
                RefreshAppList();
                RequestRepaint();
            }
            return 0;
        }
        case WM_APP_TRAY_CALLBACK: {
            if (LOWORD(lParam) == WM_LBUTTONUP) {
                g_uiState = (g_uiState == UiState::Line) ? UiState::Panel : UiState::Line;
                if (g_uiState == UiState::Panel) {
                    g_userPinnedFilter = false;
                    SyncForegroundStateAndFilter(true);
                    RefreshAppList();
                } else {
                    g_kbSelectedIdx = -1;
                    g_userPinnedFilter = false;
                }
                UpdateWindowGeometry(false);
            } else if (LOWORD(lParam) == WM_RBUTTONUP) {
                ShowTrayContextMenu(hwnd);
            }
            return 0;
        }
        case WM_APP_FOREGROUND_CHG: {
            SyncForegroundStateAndFilter(g_uiState != UiState::Line);
            if (g_uiState == UiState::Line) {
                TriggerForegroundPulse(g_lastFgExe);
                SyncCycleWidgetWindow(false);
                RequestRepaint();
            } else {
                RefreshAppList();
                RequestRepaint();
            }
            return 0;
        }
        case WM_APP_REFRESH_APPS: {
            bool hasChildBlocks = false;
            if (g_uiState == UiState::Line) {
                std::lock_guard<std::mutex> lk(g_engineMtx);
                for (const auto& kv : g_blocks) {
                    if (kv.second.blocked && kv.second.includeChildProcesses) { hasChildBlocks = true; break; }
                }
            }
            if (g_uiState != UiState::Line || hasChildBlocks) {
                SetTimer(hwnd, TIMER_DEBOUNCE_REFRESH, 300, nullptr);
            }
            return 0;
        }
        case WM_SETTINGCHANGE: {
            RefreshLightThemeCache();
            if (g_hCycleWidget && IsWindow(g_hCycleWidget) && IsWindowVisible(g_hCycleWidget)) {
                RenderCycleWidgetLayered(g_hCycleWidget);
            }
            return 0;
        }
        case WM_APP_CYCLE_APPLY: {
            std::vector<CyclePendingAction> actions;
            {
                std::lock_guard<std::mutex> lk(g_cycleMtx);
                actions.swap(g_pendingCycleActions);
            }
            for (const auto& act : actions) {
                if (act.kind == CyclePendingAction::SwitchBlock || act.kind == CyclePendingAction::SwitchAllow) {
                    std::wstring low = ToLower(act.displayPath);
                    std::lock_guard<std::mutex> lk(g_cycleMtx);
                    if (!g_cycles.count(low)) continue;
                }
                if (act.kind == CyclePendingAction::SwitchBlock) {
                    ApplyBlock(act.displayPath, act.dir, act.includeChildren, false);
                } else if (act.kind == CyclePendingAction::SwitchAllow) {
                    ApplyAllow(act.displayPath);
                } else if (act.kind == CyclePendingAction::EndAllow) {
                    ApplyAllow(act.displayPath);
                    LogAudit(L"CYCLE COMPLETED " + act.displayPath);
                } else if (act.kind == CyclePendingAction::WipeExited) {
                    ApplyAllow(act.displayPath);
                    LogAudit(L"CYCLE WIPED (process exited) " + act.displayPath);
                }
            }
            if (!actions.empty()) {
                if (g_uiState != UiState::Line) RefreshAppList();
                SyncCycleWidgetWindow(true);
                RequestRepaint();
            }
            return 0;
        }
        case WM_APP_CYCLE_STATE: {
            if (HasAnyActiveCycle() && g_uiState == UiState::Panel) {
                SetTimer(hwnd, TIMER_CYCLE_UI, 250, nullptr);
            } else if (!HasAnyActiveCycle()) {
                KillTimer(hwnd, TIMER_CYCLE_UI);
            }
            if (g_uiState != UiState::Line) RefreshAppList();
            SyncCycleWidgetWindow(true);
            RequestRepaint();
            return 0;
        }
        case WM_POWERBROADCAST: {
            if (wParam == PBT_APMRESUMESUSPEND || wParam == PBT_APMRESUMEAUTOMATIC) {
                NotifyCycleSystemResume();
            }
            return TRUE;
        }
        case WM_DESTROY: {
            KillTimer(hwnd, TIMER_COLLAPSE);
            KillTimer(hwnd, TIMER_ANIM_VSYNC);
            KillTimer(hwnd, TIMER_FULLSCREEN);
            KillTimer(hwnd, TIMER_HOVER_SENSOR);
            KillTimer(hwnd, TIMER_CYCLE_UI);
            KillTimer(hwnd, TIMER_NAVIGATE_OPEN_CYCLE);
            KillTimer(hwnd, TIMER_LATE_CHILD_POLL);
            KillTimer(hwnd, TIMER_DEBOUNCE_REFRESH);
            KillTimer(hwnd, TIMER_WIDGET_PROXIMITY);
            KillTimer(hwnd, TIMER_OUTSIDE_CLICK);
            if (g_hCyclePopup && IsWindow(g_hCyclePopup)) DestroyWindow(g_hCyclePopup);
            DestroyCycleWidgetWindow();
            for (int id = 1; id <= 3; ++id) UnregisterHotKey(hwnd, id);
            if (g_hTooltip) { DestroyWindow(g_hTooltip); g_hTooltip = nullptr; }
            RemoveTrayIcon();
            PostQuitMessage(0);
            return 0;
        }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static LRESULT CALLBACK ControlWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_APP_RELOAD_SETTINGS) {
        LoadSettings();
        if (!g_settings.enableCycleScheduler) StopAllCycles(true);
        SyncAutoBlockedApps();
        if (g_hWnd) {
            ApplyCaptureExclusionAll();
            RegisterAllHotkeys(g_hWnd);
            UpdateWindowGeometry(true);
        }
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject, LONG, DWORD, DWORD) {
    if (!g_hWnd || !hwnd || hwnd == g_hWnd) return;
    if (event == EVENT_SYSTEM_FOREGROUND || event == EVENT_SYSTEM_SWITCHEND || event == EVENT_SYSTEM_MINIMIZEEND) {
        PostMessageW(g_hWnd, WM_APP_FOREGROUND_CHG, 0, 0);
    } else if (event == EVENT_OBJECT_DESTROY && idObject == OBJID_WINDOW) {
        PostMessageW(g_hWnd, WM_APP_FOREGROUND_CHG, 0, 0);
        PostMessageW(g_hWnd, WM_APP_REFRESH_APPS, 0, 0);
    } else if (event == EVENT_OBJECT_CREATE && idObject == OBJID_WINDOW) {
        PostMessageW(g_hWnd, WM_APP_REFRESH_APPS, 0, 0);
    }
}

// ============================================================================
// UI Thread & Windhawk Tool Mod Lifecycle
// ============================================================================

static DWORD WINAPI UiThreadProc(LPVOID) {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    Gdiplus::GdiplusStartupInput gpsi;
    Gdiplus::GdiplusStartup(&g_gdiplusToken, &gpsi, nullptr);
    g_expandClock.Init();
    g_scrollClock.Init();

    WNDCLASSEXW wc{sizeof(wc)};
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = FloatingWndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = kWndClass;
    RegisterClassExW(&wc);

    WNDCLASSEXW cwc{sizeof(cwc)};
    cwc.lpfnWndProc = ControlWndProc;
    cwc.hInstance = GetModuleHandleW(nullptr);
    cwc.lpszClassName = kCtrlWndClass;
    RegisterClassExW(&cwc);

    g_hCtrlWnd = CreateWindowExW(0, kCtrlWndClass, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, cwc.hInstance, nullptr);

    RECT initRc{};
    ComputeTargetRect(0.0, initRc);
    g_hWnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        kWndClass, L"!Net", WS_POPUP,
        initRc.left, initRc.top, initRc.right - initRc.left, initRc.bottom - initRc.top,
        nullptr, nullptr, wc.hInstance, nullptr);

    if (g_hWnd) {
        ShowWindow(g_hWnd, SW_SHOWNOACTIVATE);
        UpdateWindowGeometry(true);
        UpdateWindow(g_hWnd);
    }

    g_hookFg = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_MINIMIZEEND, nullptr,
                               WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    g_hookCreate = SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_DESTROY, nullptr,
                                   WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);

    if (g_uiReadyEvent) SetEvent(g_uiReadyEvent);

    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_hookFg) { UnhookWinEvent(g_hookFg); g_hookFg = nullptr; }
    if (g_hookCreate) { UnhookWinEvent(g_hookCreate); g_hookCreate = nullptr; }

    if (g_hCtrlWnd) { DestroyWindow(g_hCtrlWnd); g_hCtrlWnd = nullptr; }
    ClearIconCache();
    UnregisterClassW(kWndClass, wc.hInstance);
    UnregisterClassW(kCtrlWndClass, cwc.hInstance);

    if (g_gdiplusToken) { Gdiplus::GdiplusShutdown(g_gdiplusToken); g_gdiplusToken = 0; }
    CoUninitialize();
    return 0;
}

BOOL WhTool_ModInit() {
    LoadSettings();
    LoadExcludedApps();
    SyncAutoBlockedApps();

    g_uiReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_uiThread = CreateThread(nullptr, 0, UiThreadProc, nullptr, 0, &g_uiThreadId);
    if (!g_uiThread) {
        if (g_uiReadyEvent) { CloseHandle(g_uiReadyEvent); g_uiReadyEvent = nullptr; }
        return FALSE;
    }
    if (g_uiReadyEvent) {
        WaitForSingleObject(g_uiReadyEvent, 5000);
        CloseHandle(g_uiReadyEvent);
        g_uiReadyEvent = nullptr;
    }
    StartCycleEngine();
    LogAudit(L"!Net initialized.");
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    if (g_hCtrlWnd) {
        PostMessageW(g_hCtrlWnd, WM_APP_RELOAD_SETTINGS, 0, 0);
    }
}

void WhTool_ModUninit() {
    ShutdownCycleEngine();
    if (g_hWnd) {
        PostMessageW(g_hWnd, WM_CLOSE, 0, 0);
    } else if (g_uiThreadId) {
        PostThreadMessageW(g_uiThreadId, WM_QUIT, 0, 0);
    }
    if (g_uiThread) {
        WaitForSingleObject(g_uiThread, 5000);
        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
    }

    RestoreAllSessionBlocks();
    {
        std::lock_guard<std::mutex> lk(g_engineMtx);
        CloseWfpEngineLocked();
    }
    LogAudit(L"!Net uninitialized cleanly.");
}

// ============================================================================
// Windhawk Tool Mod Launcher Boilerplate (Windhawk 1.7.x / 2.0 compatible)
// ============================================================================

static void RelaunchElevated() {
    WCHAR exePath[MAX_PATH]{};
    GetModuleFileNameW(nullptr, exePath, ARRAYSIZE(exePath));
    std::wstring params = std::wstring(L"-tool-mod \"") + WH_MOD_ID + L"\"";

    if (g_toolModProcessMutex) {
        CloseHandle(g_toolModProcessMutex);
        g_toolModProcessMutex = nullptr;
    }

    SHELLEXECUTEINFOW sei{sizeof(sei)};
    sei.lpVerb = L"runas";
    sei.lpFile = exePath;
    sei.lpParameters = params.c_str();
    sei.nShow = SW_SHOWNORMAL;
    if (ShellExecuteExW(&sei)) {
        RemoveTrayIcon();
        ExitProcess(0);
    } else {
        g_toolModProcessMutex = CreateMutexW(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        g_statusBanner = L"Admin permission cancelled — needed to block apps.";
        RequestRepaint();
    }
}

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) && sessionId == 0) {
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

    WCHAR commandLine[MAX_PATH + 2 +
                      (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) -
                      1];
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
        LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation,
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
