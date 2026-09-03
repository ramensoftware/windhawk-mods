// ==WindhawkMod==
// @id              taskbar-recent-focus-highlight
// @name            Taskbar Recent Focus Highlight
// @description     Visually highlight the most recently focused running apps on the taskbar
// @version         0.9.0
// @author          Jakub Vlášek
// @github          https://github.com/jvlasek
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject -lpropsys -luuid -lshell32 -ladvapi32
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.

// ==WindhawkModReadme==
/*
# Taskbar Recent Focus Highlight

Visually highlights the most recently used running applications on the taskbar
for faster context switching. Optionally ranks **windows** inside multi-window
thumbnail previews (e.g. several VS Code or Terminal instances) with the same
kind of intensity ladder used on the icons.

## How it works

The mod tracks window focus (`EVENT_SYSTEM_FOREGROUND`) and keeps a ranked list
of the apps you actually stayed on (after a configurable minimum focus time),
**per virtual desktop**. The top N **running** taskbar buttons on the current
desktop receive a highlight (frame, full plate, side bar, or edge bar — bars
rotate with the taskbar’s screen edge) and optional icon scale. App↔button
binding uses a process-path cache resolved from the taskband (same approach
as taskbar-volume-control-per-app),
with name matching as fallback. Pinned icons that are not running on this
desktop are never highlighted.

Separately, it tracks **per-window** recency (own min-focus / decay). When you
hover a combined taskbar icon and the flyout shows 2+ thumbnails, that flyout
gets its **own** recency list: the top N windows in it are marked with rank
intensities (title bar, soft title tint, whole plate, hybrid plate+title, or
ring — configurable). Single-window flyouts are left alone.

## Tips for testing

1. Compile and enable the mod in Windhawk (injects into `explorer.exe`).
2. Optionally enable **Debug logging**.
3. Focus apps for at least the minimum focus time (default 8s).
4. Ranked apps should show a highlight on their taskbar buttons (default: side
   bar — left on a bottom taskbar, under the icon on a left/right taskbar);
   rank 1 is strongest. Moving the taskbar to another edge should keep running
   dots on unranked icons and rotate the bar.
5. Brief Alt+Tab under the minimum focus time should not change ranks.
6. Open 3+ windows of one app, focus them in turn, hover the icon — previews
   show ranks 1 > 2 > 3 (strongest on the last focused window of that app).
7. Disable the mod or toggle Enabled off to clear highlights.
8. Multi-monitor: the same rank should appear on every taskbar that shows
   that app. Combined-icon flyouts should mark the recent window even when
   titles differ (Total Commander Lister, etc.). Lister has its own taskbar
   icon — focusing it should highlight Lister, not Total Commander.
9. Virtual desktops: each desktop has its own recency list. A pinned icon
   that is not running on this desktop must not glow. Switching back restores
   that desktop’s ranks.
10. UWP: Calculator and Settings get separate ranks. Windows Security must
    not copy Settings’ glow. Packaged apps with their own exe (Terminal) are
    still path-keyed.
11. Windows 11 Taskbar Styler: hover must not hide the side bar; preview
    plate should restore the Styler tint when cleared.

See the repo README.md for full settings and architecture notes.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
# Windhawk shows settings in this list order (no section headers — $group is
# not allowed by the settings schema). Names use [General] / [Icons] /
# [Previews] / [Advanced] prefixes so groups stay obvious in a flat list.

# --- General ---
- enabled: true
  $name: "[General] Enabled"
  $description: Master toggle for all highlighting (icons and previews)
- highlightCount: 3
  $name: "[General] Number of highlighted apps"
  $description: How many recent apps to boost on the taskbar (1–6 recommended)
- minFocusSeconds: 8
  $name: "[General] Minimum focus time (seconds)"
  $description: >-
    Only count an app as recent if it stays focused at least this long.
    Filters Alt+Tab noise. (Preview windows use a separate timer under Previews.)
    See “When to skip min-focus” for re-focus of apps already in the list.
- promoteMode: immediateTracked
  $name: "[General] When to skip min-focus"
  $description: >-
    Controls instant promotion when you re-focus an app (confirmed apps still
    become rank 1 once promoted — this only skips the wait timer).

    Immediate if in recency map (default): any app still in the map (even if
    not currently highlighted) promotes immediately.

    Immediate only if highlighted: instant only when the app is already in the
    top-N glow set; rank 4+ and new apps wait the full min-focus time.

    Always wait: every app focus (including re-focus) waits min-focus seconds.
  $options:
  - immediateTracked: Immediate if still in recency map (default)
  - immediateTopN: Immediate only if already highlighted (top N)
  - alwaysWait: Always wait min-focus time
- decayMinutes: 30
  $name: "[General] Decay time (minutes)"
  $description: >-
    Time after last focus before an app drops out of the taskbar highlight
    list. (Preview windows use a separate decay under Previews.)
- requireTaskbarButton: true
  $name: "[General] Only apps on the taskbar"
  $description: >-
    Ignore tray-only / tool windows that take focus but have no taskbar button
    (e.g. desktop widgets that open a popup then hide to the tray).
- excludedPrograms: [""]
  $name: "[General] Exclude list"
  $description: >-
    Apps that should never be highlighted (icons or previews). Entries can be
    process names, paths or application IDs, for example:

    mspaint.exe

    C:\Windows\System32\notepad.exe

    Microsoft.WindowsCalculator_8wekyb3d8bbwe!App

# --- Taskbar icons ---
- glowStyle: leftBar
  $name: "[Icons] Highlight style"
  $description: >-
    How ranked apps look on the taskbar. Bars rotate with the taskbar edge
    (bottom / left / top / right). Side bar = beside the icon (left on a
    bottom or top taskbar, under the icon on a left or right taskbar). Edge
    bar = same side as the native running indicator (screen edge). Frame/Full
    = rounded rectangle. Edge bar paints our own pill and does not restyle
    the native running indicator permanently.
  $options:
  - leftBar: Side bar (left on bottom/top, under icon on left/right)
  - frame: Frame (hollow rounded rectangle)
  - full: Full (filled rounded rectangle)
  - bottomBar: Edge bar (follows the screen edge)
- glowColor: accent
  $name: "[Icons] Glow color"
  $description: Base color for icon highlights (and previews)
  $options:
  - accent: System accent color
  - green: Green
  - blue: Blue
  - orange: Orange
  - white: White
  - custom: Custom (see custom glow color)
- customGlowColor: "#00C853"
  $name: "[Icons] Custom glow color"
  $description: Used when glow color is Custom (hex, e.g. #00C853)
- glowIntensityRank1: 100
  $name: "[Icons] Intensity rank 1"
  $description: Strength for the most recent app (0–100)
- glowIntensityRank2: 70
  $name: "[Icons] Intensity rank 2"
  $description: Strength for the 2nd most recent app (0–100)
- glowIntensityRank3: 45
  $name: "[Icons] Intensity rank 3"
  $description: Strength for the 3rd most recent app (0–100); also used for ranks 4+
- glowThickness: 3
  $name: "[Icons] Thickness (px)"
  $description: >-
    Frame/Full border width, or bar thickness (1–16). For side/edge bars this
    is the bar’s short dimension.
- glowRoundness: 28
  $name: "[Icons] Roundness (%)"
  $description: >-
    Corner radius for Frame/Full (0 = square, ~25–35 = Win11, 50 ≈ pill).
    Side bar uses this for pill rounding; edge bar ignores it.
- glowSize: 92
  $name: "[Icons] Size (%)"
  $description: >-
    Frame/Full: box size vs icon panel (≤100). Side bar: bar length along the
    icon. Edge bar: pill length % of the icon’s long side (try 70–100).
- glowLayers: 2
  $name: "[Icons] Layers"
  $description: >-
    Frame/Full: nested frames (1–3). Side bar: soft outer glow layers. Edge
    bar: ignored.
- glowFillOpacity: 40
  $name: "[Icons] Fill opacity"
  $description: >-
    0–100. Plate fill for Full; solid bar opacity for Side/Edge. Frame uses
    stroke only. (Thumbnail tints use [Previews] Tint opacity.)
- sizeBoostRank1: 10
  $name: "[Icons] Size boost rank 1 (%)"
  $description: Subtle icon scale for rank 1 (0 = disabled)
- sizeBoostRank2: 6
  $name: "[Icons] Size boost rank 2 (%)"
  $description: Subtle icon scale for rank 2 (0 = disabled)
- sizeBoostRank3: 3
  $name: "[Icons] Size boost rank 3 (%)"
  $description: Subtle icon scale for rank 3 (0 = disabled)

# --- Thumbnail previews ---
- previewHighlightEnabled: true
  $name: "[Previews] Highlight recent windows"
  $description: >-
    When hovering a multi-window taskbar icon, rank that flyout’s thumbnails
    by window recency and mark the top N. Single-window flyouts are never
    highlighted.
- previewHighlightCount: 3
  $name: "[Previews] Number of highlighted windows"
  $description: >-
    How many recent windows to mark in a multi-window flyout (1–6 recommended).
    Ranking is local to that flyout: the last focused window of this app is
    rank 1 even if other apps were used more recently. Set to 1 to mark only
    the latest window.
- previewStyle: titleBar
  $name: "[Previews] Highlight style"
  $description: >-
    How to mark ranked windows. Title bar = thin line under the title.
    Title background = soft wash behind the title. Plate = tint the whole card.
    Hybrid = whole plate for rank 1, title wash for ranks 2+.
    Ring = hollow border around the card.
  $options:
  - titleBar: Bar under window title
  - titleBg: Title background tint
  - plate: Whole preview plate
  - plateTitle: Hybrid (plate rank 1, title tint 2+)
  - ring: Ring / frame
- previewIntensityRank1: 100
  $name: "[Previews] Intensity rank 1"
  $description: Strength for the most recent window in the flyout (0–100)
- previewIntensityRank2: 70
  $name: "[Previews] Intensity rank 2"
  $description: Strength for the 2nd most recent window in the flyout (0–100)
- previewIntensityRank3: 45
  $name: "[Previews] Intensity rank 3"
  $description: >-
    Strength for the 3rd most recent window in the flyout (0–100); also used
    for ranks 4+
- previewFillOpacity: 40
  $name: "[Previews] Tint opacity"
  $description: >-
    0–100. Strength of title-background wash and whole-preview plate. Title bar
    line uses full accent and ignores this. Independent of [Icons] Fill opacity.
    Per-rank intensity still scales the result.
- previewMinFocusSeconds: 1
  $name: "[Previews] Minimum focus (seconds)"
  $description: >-
    How long a window must stay focused before it enters that flyout’s
    recency list. Separate from app ranking min focus (0 = immediate).
- previewDecayMinutes: 15
  $name: "[Previews] Decay (minutes)"
  $description: >-
    Drop a window from preview recency after this idle time (0 = never).
    Separate from app ranking decay.

# --- Advanced ---
- glowDebugLog: false
  $name: "[Advanced] Debug log (verbose)"
  $description: >-
    Logs glow metrics, path binds, and preview resolve details. Leave off for
    normal use; turn on when diagnosing matches.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <commctrl.h>
#include <initguid.h>
#include <propkey.h>
#include <propsys.h>
#include <psapi.h>
#include <objbase.h>
#include <shellapi.h>
#include <shobjidl.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/base.h>

#include <algorithm>
#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace winrt::Windows::UI::Xaml;

// Public IVirtualDesktopManager (Win10+). Defined here so we do not depend on
// a particular SDK header / uuid.lib export of the CLSID.
#ifndef __IVirtualDesktopManager_INTERFACE_DEFINED__
#define __IVirtualDesktopManager_INTERFACE_DEFINED__
MIDL_INTERFACE("a5cd92ff-29be-454c-8d04-d82879fb3f1b")
IVirtualDesktopManager : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE IsWindowOnCurrentVirtualDesktop(
        HWND topLevelWindow,
        BOOL* onCurrentDesktop) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetWindowDesktopId(HWND topLevelWindow,
                                                         GUID* desktopId) = 0;
    virtual HRESULT STDMETHODCALLTYPE MoveWindowToDesktop(HWND topLevelWindow,
                                                          REFGUID desktopId) = 0;
};
#endif

// AA509086-5CA9-4C25-8F95-589D3C07B48A
static const CLSID kClsidVirtualDesktopManager = {
    0xaa509086,
    0x5ca9,
    0x4c25,
    {0x8f, 0x95, 0x58, 0x9d, 0x3c, 0x07, 0xb4, 0x8a}};

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

enum class GlowColorMode {
    Accent,
    Green,
    Blue,
    Orange,
    White,
    Custom,
};

enum class GlowStyle {
    Frame,      // hollow rounded rectangle
    Full,       // filled rounded rectangle
    LeftBar,    // side bar: left on horizontal taskbar, bottom on vertical
    BottomBar,  // edge bar: same side as the native RunningIndicator
};

// Physical screen edge the taskbar is on (Win11 24H2/25H2 can use all four).
enum class TaskbarEdge {
    Bottom,
    Left,
    Top,
    Right,
};

// Where a bar highlight is painted on the icon button.
enum class BarSide {
    Left,
    Top,
    Right,
    Bottom,
};

// When re-focus may skip the app min-focus timer (see HandleForegroundChanged).
enum class PromoteMode {
    ImmediateTracked,  // any app still in this desktop's recency map
    ImmediateTopN,     // only if currently highlighted on this desktop
    AlwaysWait,        // always use minFocusSeconds
};

// Thumbnail flyout highlight (independent of icon GlowStyle).
enum class PreviewStyle {
    Ring,       // hollow frame around the whole preview (placeholder)
    TitleBg,    // tint behind the window title text
    Plate,      // tint whole preview card (hover-like plate)
    TitleBar,   // thin bar under the title, above the thumbnail image
    PlateTitle, // rank 1 = plate; ranks 2+ = title background tint
};

struct Settings {
    bool enabled = true;
    int highlightCount = 3;
    int minFocusSeconds = 8;
    PromoteMode promoteMode = PromoteMode::ImmediateTracked;
    GlowColorMode glowColor = GlowColorMode::Accent;
    std::wstring customGlowColor = L"#00C853";
    int glowIntensity[3] = {100, 70, 45};
    int sizeBoostPercent[3] = {10, 6, 3};
    GlowStyle glowStyle = GlowStyle::LeftBar;
    int glowThickness = 3;      // px
    int glowRoundness = 28;     // % of glow box
    int glowSize = 92;          // % of icon panel (clamped to fit)
    int glowLayers = 2;         // 1–3
    int glowFillOpacity = 40;   // % for Full / left / bottom icon styles
    int previewFillOpacity = 40;  // % for thumbnail plate / titleBg only
    bool glowDebugLog = false;
    int decayMinutes = 30;
    bool requireTaskbarButton = true;  // skip tray-only focus targets
    bool previewHighlightEnabled = true;
    int previewHighlightCount = 3;
    int previewIntensity[3] = {100, 70, 45};
    int previewMinFocusSeconds = 1;
    int previewDecayMinutes = 15;
    PreviewStyle previewStyle = PreviewStyle::TitleBar;
    std::unordered_set<std::wstring> excludedPrograms;
};

// Published as a whole. LoadSettings builds a local Settings, then stores it
// here so the focus thread and UI dispatchers never iterate a set mid-rehash.
// Windhawk's libc++ has no std::atomic<shared_ptr> specialization (T must be
// trivially copyable), so the pointer is swapped under a mutex.
std::mutex g_settingsMutex;
std::shared_ptr<const Settings> g_settingsPtr =
    std::make_shared<const Settings>();

std::shared_ptr<const Settings> SettingsSnap() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settingsPtr ? g_settingsPtr : std::make_shared<const Settings>();
}

void PublishSettings(Settings s) {
    auto next = std::make_shared<const Settings>(std::move(s));
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_settingsPtr = std::move(next);
}

// WM_TIMER may be a leftover from a previous candidate (KillTimer does not
// flush a message already queued). Immediate confirm skips the deadline.
enum class MinFocusConfirmMode {
    FromTimer,
    Immediate,
};

// ---------------------------------------------------------------------------
// Focus / recency state
// ---------------------------------------------------------------------------

struct AppFocusInfo {
    std::wstring key;          // APPID:… or PATH|CLS:class or bare path
    std::wstring displayName;  // e.g. WindowsTerminal.exe
    std::wstring lastWindowTitle;  // from focused HWND (helps button match)
    std::wstring classUpper;   // focused window class (TLister vs TTOTAL_CMD)
    std::wstring appIdUpper;   // window AppUserModelID when present
    HWND lastHwnd = nullptr;
    ULONGLONG lastConfirmedFocusTick = 0;
    // True once we saw a TaskListButton for this path (path cache / bind).
    bool seenOnTaskbar = false;
};

struct PendingFocus {
    HWND hwnd = nullptr;
    DWORD processId = 0;
    std::wstring key;
    std::wstring displayName;
    std::wstring windowTitle;
    ULONGLONG focusStartTick = 0;
    ULONGLONG previewStartTick = 0;  // HWND-level; resets when instance changes
    GUID desktopId{};
    bool valid = false;
};

// Per-window recency for multi-instance thumbnail previews (separate timers).
struct WindowFocusInfo {
    HWND hwnd = nullptr;
    DWORD pid = 0;              // reject recycled HWND with a new process
    std::wstring processKey;    // UPPER path
    std::wstring windowTitle;   // fallback match
    ULONGLONG lastConfirmedTick = 0;
    ULONGLONG confirmSeq = 0;  // unique per confirm (breaks GetTickCount ties)
};

struct HwndHash {
    size_t operator()(HWND h) const noexcept {
        return std::hash<uintptr_t>{}(reinterpret_cast<uintptr_t>(h));
    }
};

struct GuidHash {
    size_t operator()(const GUID& g) const noexcept {
        size_t h = 0;
        const unsigned char* p = reinterpret_cast<const unsigned char*>(&g);
        for (int i = 0; i < 16; ++i) {
            h = h * 131u + p[i];
        }
        return h;
    }
};

struct GuidEqual {
    bool operator()(const GUID& a, const GUID& b) const noexcept {
        return InlineIsEqualGUID(a, b) != 0;
    }
};

// App + window recency for one virtual desktop (session-wide, not per-monitor).
struct DesktopRecencyState {
    std::unordered_map<std::wstring, AppFocusInfo> appFocusMap;
    std::vector<AppFocusInfo> rankedApps;
    std::unordered_map<HWND, WindowFocusInfo, HwndHash> windowFocusMap;
};

std::mutex g_stateMutex;
std::unordered_map<GUID, DesktopRecencyState, GuidHash, GuidEqual> g_desktopMaps;
GUID g_currentDesktopId{};
bool g_haveCurrentDesktop = false;
PendingFocus g_pendingFocus;

// process key (UPPER path) -> last seen AutomationProperties.Name of its button
std::unordered_map<std::wstring, std::wstring> g_keyToAutomationName;

std::atomic<ULONGLONG> g_windowConfirmSeq{0};

// Public IVirtualDesktopManager (fallback when registry current-desktop fails).
IVirtualDesktopManager* g_vdm = nullptr;
std::mutex g_vdmMutex;

// Option C: long-lived button → process path cache (resolve rarely, paint often).
struct ButtonPathCacheEntry {
    winrt::weak_ref<FrameworkElement> button;
    std::wstring pathUpper;  // empty if resolve failed / not yet tried
    std::wstring appIdUpper;
    std::wstring classUpper;
    std::wstring autoIdUpper;  // AutomationId, often "APPID: …"
    DWORD pid = 0;
    HWND sampleHwnd = nullptr;  // sample from resolve; preview uses window map
    std::vector<HWND> groupHwnds;
    bool resolveAttempted = false;
    ULONGLONG lastResolveTick = 0;
    ULONGLONG lastRunningTick = 0;  // IsRunning grace (Alt-Tab flicker)
    // Last ApplyAllHighlights assignment: -1 unknown, 0 none, >0 1-based rank.
    int lastPaintRank = -1;
};
std::mutex g_buttonPathMutex;
std::vector<ButtonPathCacheEntry> g_buttonPathCache;
std::atomic<bool> g_taskbandResolveReady{false};

// XAML TaskItemThumbnail (model) → native task item (optional hooks).
struct ThumbnailTaskItemMapping {
    winrt::weak_ref<winrt::Windows::Foundation::IInspectable> thumbnail;
    void* taskGroup = nullptr;
    void* taskItem = nullptr;
    HWND hwnd = nullptr;  // resolved at map time (stable for same-title windows)
};
std::mutex g_thumbnailMapMutex;
std::vector<ThumbnailTaskItemMapping> g_thumbnailTaskItemMapping;
std::atomic<bool> g_previewHooksReady{false};

// Live thumbnail views for unload / re-apply while flyout is open.
std::mutex g_thumbViewsMutex;
std::vector<winrt::weak_ref<FrameworkElement>> g_trackedThumbViews;

std::atomic<bool> g_unloading{false};
std::atomic<bool> g_taskbarViewDllLoaded{false};
std::atomic<bool> g_taskbarDllHooked{false};
// After decay / empty ranks, force-clear overlays on next button touch if
// RequestApplyVisuals couldn't run (sleep/wake, no dispatcher yet).
std::atomic<bool> g_pendingOverlaySweep{false};

std::mutex g_winEventHookThreadMutex;
std::atomic<HANDLE> g_winEventHookThread{nullptr};
HWND g_hookThreadHwnd = nullptr;

// UI-thread tracking of task list buttons (weak refs).
std::mutex g_buttonsMutex;
std::vector<winrt::weak_ref<FrameworkElement>> g_trackedButtons;
winrt::weak_ref<FrameworkElement> g_dispatcherAnchor;

constexpr UINT WM_APP_FOREGROUND_CHANGED = WM_APP + 1;
constexpr UINT WM_APP_REQUEST_APPLY = WM_APP + 2;
constexpr UINT WM_APP_REQUEST_PREVIEW_APPLY = WM_APP + 3;
constexpr UINT WM_APP_DESKTOP_SWITCHED = WM_APP + 4;
constexpr UINT WM_APP_SHUTDOWN = WM_APP + 5;

// Identity scores. Only exact identity (and name-cache) may bind the same
// rank to many buttons (secondary taskbar / Never Combine). Score 900 is
// same filename, different folder — 1:1 so two python.exe installs stay
// distinct.
constexpr int kScoreExactIdentity = 1000;
constexpr int kScoreSameFileDifferentPath = 900;
constexpr int kScoreNameCache = 96;
constexpr int kScoreMinBind = 70;
constexpr UINT_PTR kMinFocusTimerId = 1;
constexpr UINT_PTR kDecayTimerId = 2;
constexpr UINT_PTR kPreviewMinFocusTimerId = 3;
constexpr UINT_PTR kFullRebindTimerId = 4;
constexpr UINT kDecayCheckIntervalMs = 30 * 1000;
constexpr ULONGLONG kIsRunningGraceMs = 400;
// Full identity rebind (all buttons). UVS only re-paints the cached rank;
// siblings whose visuals Windows cleared without another UVS wait for this.
constexpr ULONGLONG kFullRebindDebounceMs = 300;

// All glow layers live on our overlay (never BackgroundElement — hover/active
// storyboards own that and constantly wipe our styles).
constexpr PCWSTR kGlowElementName = L"WhRecentFocusGlow";
constexpr PCWSTR kGlowLayerNames[] = {
    L"WhRecentFocusGlowL0",
    L"WhRecentFocusGlowL1",
    L"WhRecentFocusGlowL2",
};
constexpr int kGlowMaxLayers = 3;
constexpr PCWSTR kBackgroundElementName = L"BackgroundElement";
// Present while edge-bar (bottomBar) style is applied — we hid RunningIndicator.
constexpr PCWSTR kBottomBarMarkerName = L"WhRecentFocusBottomBar";
// Thumbnail preview glow (own named overlays on TaskItemThumbnailView).
constexpr PCWSTR kThumbGlowElementName = L"WhRecentFocusThumbGlow";
constexpr PCWSTR kThumbGlowLayerNames[] = {
    L"WhRecentFocusThumbGlowL0",
    L"WhRecentFocusThumbGlowL1",
};
// Title-area overlays (separate so plate/ring host can sit full-card).
constexpr PCWSTR kThumbTitleBgName = L"WhRecentFocusThumbTitleBg";
constexpr PCWSTR kThumbTitleBarName = L"WhRecentFocusThumbTitleBar";
// Marker: we tinted native BackgroundBorder. Tag holds the previous Brush
// (or is empty if the local value was unset) so Taskbar Styler survives clear.
constexpr PCWSTR kThumbNativeStyleMarker = L"WhRecentFocusThumbNative";

// ---------------------------------------------------------------------------
// Virtual desktops (public COM + explorer registry — no internal shell GUIDs)
// ---------------------------------------------------------------------------

void CancelMinFocusTimer();
void CancelPreviewMinFocusTimer();
void OnMinFocusTimerElapsed(MinFocusConfirmMode mode = MinFocusConfirmMode::FromTimer);
void OnPreviewMinFocusTimerElapsed(
    MinFocusConfirmMode mode = MinFocusConfirmMode::FromTimer);
void RequestApplyVisuals();
void RequestApplyPreviewVisuals();
void RefreshButtonHighlight(FrameworkElement button);
void ScheduleRefreshAllHighlights(FrameworkElement dispatcherAnchor);
void RecomputeRanksForDesktopLocked(DesktopRecencyState& desk);

struct IconPanelLayoutWatch {
    winrt::weak_ref<FrameworkElement> panel;
    winrt::event_token sizeChanged{};
    TaskbarEdge lastEdge = TaskbarEdge::Bottom;
    bool haveEdge = false;
};
std::mutex g_layoutWatchMutex;
std::vector<IconPanelLayoutWatch> g_layoutWatches;
thread_local int g_iconPanelRelayoutDepth = 0;
thread_local ULONGLONG g_lastFullRefreshTick = 0;

std::wstring GuidToLogString(const GUID& id) {
    wchar_t buf[64]{};
    if (StringFromGUID2(id, buf, ARRAYSIZE(buf)) > 0) {
        return buf;
    }
    return L"{?}";
}

IVirtualDesktopManager* EnsureVdm() {
    std::lock_guard<std::mutex> lock(g_vdmMutex);
    if (g_vdm) {
        return g_vdm;
    }
    IVirtualDesktopManager* vdm = nullptr;
    HRESULT hr = CoCreateInstance(kClsidVirtualDesktopManager, nullptr,
                                  CLSCTX_ALL, IID_PPV_ARGS(&vdm));
    if (FAILED(hr) || !vdm) {
        Wh_Log(L"IVirtualDesktopManager create failed %08X", hr);
        return nullptr;
    }
    g_vdm = vdm;
    return g_vdm;
}

void ReleaseVdm() {
    std::lock_guard<std::mutex> lock(g_vdmMutex);
    if (g_vdm) {
        g_vdm->Release();
        g_vdm = nullptr;
    }
}

bool TryGetWindowDesktopId(HWND hwnd, GUID* outId) {
    if (!hwnd || !outId) {
        return false;
    }
    IVirtualDesktopManager* vdm = EnsureVdm();
    if (!vdm) {
        return false;
    }
    GUID id{};
    HRESULT hr = vdm->GetWindowDesktopId(hwnd, &id);
    if (FAILED(hr) || InlineIsEqualGUID(id, GUID_NULL)) {
        return false;
    }
    *outId = id;
    return true;
}

bool ReadCurrentDesktopFromRegistry(GUID* outId) {
    if (!outId) {
        return false;
    }
    HKEY key = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\"
                      L"VirtualDesktops",
                      0, KEY_READ, &key) != ERROR_SUCCESS) {
        return false;
    }
    GUID id{};
    DWORD size = sizeof(id);
    DWORD type = 0;
    LONG rc = RegQueryValueExW(key, L"CurrentVirtualDesktop", nullptr, &type,
                               reinterpret_cast<LPBYTE>(&id), &size);
    RegCloseKey(key);
    if (rc != ERROR_SUCCESS || type != REG_BINARY || size != sizeof(GUID) ||
        InlineIsEqualGUID(id, GUID_NULL)) {
        return false;
    }
    *outId = id;
    return true;
}

GUID ResolveCurrentDesktopId() {
    GUID fromReg{};
    const bool haveReg = ReadCurrentDesktopFromRegistry(&fromReg);
    GUID fromWnd{};
    const bool haveWnd =
        TryGetWindowDesktopId(GetForegroundWindow(), &fromWnd);
    if (haveReg && haveWnd && !InlineIsEqualGUID(fromReg, fromWnd) &&
        SettingsSnap()->glowDebugLog) {
        Wh_Log(L"Desktop id mismatch registry=%s fgWindow=%s — using registry",
               GuidToLogString(fromReg).c_str(),
               GuidToLogString(fromWnd).c_str());
    }
    if (haveReg) {
        return fromReg;
    }
    if (haveWnd) {
        return fromWnd;
    }
    std::lock_guard<std::mutex> lock(g_stateMutex);
    return g_currentDesktopId;
}

// Returns true when the current-desktop id changed after the first successful
// read (i.e. a real switch, not startup). Safe to call without g_stateMutex.
bool RefreshCurrentDesktopId() {
    const GUID id = ResolveCurrentDesktopId();
    std::lock_guard<std::mutex> lock(g_stateMutex);
    const bool first = !g_haveCurrentDesktop;
    const bool changed = !InlineIsEqualGUID(id, g_currentDesktopId);
    g_currentDesktopId = id;
    g_haveCurrentDesktop = true;
    g_desktopMaps[id];
    if (changed) {
        Wh_Log(L"Current virtual desktop: %s (%zu tracked)",
               GuidToLogString(id).c_str(), g_desktopMaps.size());
    }
    return !first && changed;
}

// Requires g_stateMutex.
DesktopRecencyState& DeskStateLocked(const GUID& id) {
    return g_desktopMaps[id];
}

DesktopRecencyState& CurrentDeskLocked() {
    return g_desktopMaps[g_currentDesktopId];
}

void ClearButtonRunningGrace() {
    std::lock_guard<std::mutex> lock(g_buttonPathMutex);
    for (auto& e : g_buttonPathCache) {
        e.lastRunningTick = 0;
    }
}

// Requires g_stateMutex. True if pending was dropped.
bool DropPendingIfWrongDesktopLocked() {
    if (g_pendingFocus.valid &&
        !InlineIsEqualGUID(g_pendingFocus.desktopId, g_currentDesktopId)) {
        g_pendingFocus = {};
        return true;
    }
    return false;
}

void OnVirtualDesktopSwitched() {
    ClearButtonRunningGrace();
    RefreshCurrentDesktopId();
    bool droppedPending = false;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        droppedPending = DropPendingIfWrongDesktopLocked();
        RecomputeRanksForDesktopLocked(CurrentDeskLocked());
        Wh_Log(L"Virtual desktop switch: %s ranks=%zu droppedPending=%d",
               GuidToLogString(g_currentDesktopId).c_str(),
               CurrentDeskLocked().rankedApps.size(),
               droppedPending ? 1 : 0);
    }
    if (droppedPending) {
        CancelMinFocusTimer();
        CancelPreviewMinFocusTimer();
    }
    g_pendingOverlaySweep = true;
    RequestApplyVisuals();
    RequestApplyPreviewVisuals();
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

ULONGLONG DecayMsFromMinutes(int minutes) {
    if (minutes <= 0) {
        return 0;
    }
    return static_cast<ULONGLONG>(minutes) * 60ULL * 1000ULL;
}

bool IsTickDecayed(ULONGLONG tick, ULONGLONG decayMs, ULONGLONG now) {
    return decayMs > 0 && tick != 0 && now - tick > decayMs;
}

// KillTimer does not dequeue a WM_TIMER already posted. Re-check this
// candidate's start tick and arm the remainder instead of confirming early.
ULONGLONG RemainingDeadlineMs(ULONGLONG startTick, int seconds, ULONGLONG now) {
    if (seconds <= 0) {
        return 0;
    }
    const ULONGLONG need = static_cast<ULONGLONG>(seconds) * 1000ULL;
    if (now < startTick) {
        return need;
    }
    const ULONGLONG elapsed = now - startTick;
    if (elapsed >= need) {
        return 0;
    }
    return need - elapsed;
}

UINT ClampWinTimerMs(ULONGLONG ms) {
    constexpr ULONGLONG kMax = 0x7FFFFFFFULL;
    if (ms == 0) {
        return 1;
    }
    if (ms > kMax) {
        return static_cast<UINT>(kMax);
    }
    return static_cast<UINT>(ms);
}

std::wstring ToUpper(std::wstring s) {
    if (!s.empty()) {
        LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE, s.data(),
                      static_cast<int>(s.length()), s.data(),
                      static_cast<int>(s.length()), nullptr, nullptr, 0);
    }
    return s;
}

// Keep A–Z / 0–9 only, uppercased — for fuzzy exe ↔ automation-name match.
std::wstring AlnumUpper(std::wstring_view s) {
    std::wstring out;
    out.reserve(s.size());
    for (wchar_t ch : s) {
        if (ch >= L'a' && ch <= L'z') {
            out.push_back(static_cast<wchar_t>(ch - L'a' + L'A'));
        } else if ((ch >= L'A' && ch <= L'Z') || (ch >= L'0' && ch <= L'9')) {
            out.push_back(ch);
        }
    }
    return out;
}

std::wstring GetProcessImagePath(DWORD processId) {
    std::wstring path;
    HANDLE hProcess =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (!hProcess) {
        return path;
    }

    DWORD size = MAX_PATH;
    for (int attempt = 0; attempt < 5; ++attempt) {
        path.assign(size, L'\0');
        DWORD n = size;
        if (QueryFullProcessImageName(hProcess, 0, path.data(), &n)) {
            path.resize(n);
            CloseHandle(hProcess);
            return path;
        }
        const DWORD err = GetLastError();
        if (err != ERROR_INSUFFICIENT_BUFFER || n <= size) {
            path.clear();
            break;
        }
        size = n;
        if (size > 32768) {
            path.clear();
            break;
        }
    }
    CloseHandle(hProcess);
    return path;
}

DWORD PidFromHwnd(HWND hwnd) {
    DWORD pid = 0;
    if (hwnd) {
        GetWindowThreadProcessId(hwnd, &pid);
    }
    return pid;
}

// False when the handle was recycled into a different process.
bool HwndMatchesStoredPid(HWND hwnd, DWORD storedPid) {
    if (!hwnd || !IsWindow(hwnd)) {
        return false;
    }
    if (!storedPid) {
        return true;
    }
    const DWORD pid = PidFromHwnd(hwnd);
    return pid == 0 || pid == storedPid;
}

std::wstring FileNameFromPath(const std::wstring& path) {
    size_t pos = path.find_last_of(L"\\/");
    if (pos == std::wstring::npos) {
        return path;
    }
    return path.substr(pos + 1);
}

std::wstring StripExtension(std::wstring name) {
    size_t pos = name.find_last_of(L'.');
    if (pos != std::wstring::npos && pos > 0) {
        name.resize(pos);
    }
    return name;
}

bool IsOwnExplorerProcess(DWORD processId) {
    return processId == GetCurrentProcessId();
}

std::wstring PathFromAppKey(const std::wstring& key);
std::wstring AppIdFromAppKey(const std::wstring& key);
std::wstring ClassFromAppKey(const std::wstring& key);

bool IsExcludedKey(const std::wstring& keyUpper,
                   const std::wstring& displayNameUpper) {
    auto settings = SettingsSnap();
    const auto& excluded = settings->excludedPrograms;
    if (excluded.empty()) {
        return false;
    }
    if (excluded.contains(keyUpper)) {
        return true;
    }
    if (!displayNameUpper.empty() && excluded.contains(displayNameUpper)) {
        return true;
    }
    const std::wstring path = PathFromAppKey(keyUpper);
    if (!path.empty() && excluded.contains(path)) {
        return true;
    }
    if (!path.empty()) {
        std::wstring fileUpper = ToUpper(FileNameFromPath(path));
        if (!fileUpper.empty() && excluded.contains(fileUpper)) {
            return true;
        }
    }
    const std::wstring appId = AppIdFromAppKey(keyUpper);
    if (!appId.empty() && excluded.contains(appId)) {
        return true;
    }
    return false;
}

HWND NormalizeFocusHwnd(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) {
        return nullptr;
    }
    if (GetWindowLong(hWnd, GWL_STYLE) & WS_CHILD) {
        HWND root = GetAncestor(hWnd, GA_ROOT);
        if (root && IsWindow(root)) {
            return root;
        }
    }
    return hWnd;
}

std::wstring GetWindowClassName(HWND hWnd) {
    WCHAR buf[256]{};
    if (!hWnd || !GetClassNameW(hWnd, buf, ARRAYSIZE(buf))) {
        return {};
    }
    return buf;
}

std::wstring GetWindowAppUserModelId(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) {
        return {};
    }
    IPropertyStore* store = nullptr;
    HRESULT hr = SHGetPropertyStoreForWindow(hWnd, IID_IPropertyStore,
                                             reinterpret_cast<void**>(&store));
    if (FAILED(hr) || !store) {
        return {};
    }
    PROPVARIANT pv;
    PropVariantInit(&pv);
    hr = store->GetValue(PKEY_AppUserModel_ID, &pv);
    std::wstring id;
    if (SUCCEEDED(hr) && pv.vt == VT_LPWSTR && pv.pwszVal && pv.pwszVal[0]) {
        id = pv.pwszVal;
    }
    PropVariantClear(&pv);
    store->Release();
    return id;
}

std::wstring StripAppIdPrefix(std::wstring id) {
    // AutomationId looks like "Appid: com.squirrel.Discord.Discord"
    constexpr wchar_t kPref[] = L"Appid:";
    if (id.size() > 6 && _wcsnicmp(id.c_str(), kPref, 6) == 0) {
        id.erase(0, 6);
        while (!id.empty() && (id.front() == L' ' || id.front() == L'\t')) {
            id.erase(id.begin());
        }
    }
    return id;
}

std::wstring CanonicalAppId(std::wstring id) {
    return ToUpper(StripAppIdPrefix(std::move(id)));
}

bool IsAppIdKey(const std::wstring& key) {
    return key.rfind(L"APPID:", 0) == 0;
}

bool IsUwpHostFileName(const std::wstring& fileNameUpper) {
    return fileNameUpper == L"APPLICATIONFRAMEHOST.EXE" ||
           fileNameUpper == L"WWAHOST.EXE";
}

bool IsUwpHostPath(const std::wstring& pathUpper) {
    if (pathUpper.empty()) {
        return false;
    }
    return IsUwpHostFileName(ToUpper(FileNameFromPath(pathUpper)));
}

// Microsoft.WindowsCalculator_8wekyb3d8bbwe!App → Microsoft.WindowsCalculator
std::wstring DisplayNameFromAppId(const std::wstring& appIdUpper) {
    std::wstring s = CanonicalAppId(appIdUpper);
    auto bang = s.find(L'!');
    if (bang != std::wstring::npos) {
        s.resize(bang);
    }
    auto us = s.rfind(L'_');
    if (us != std::wstring::npos && us > 0) {
        s.resize(us);
    }
    return s.empty() ? appIdUpper : s;
}

std::wstring MakeAppKey(const std::wstring& pathUpper,
                        const std::wstring& appIdUpper,
                        const std::wstring& /*classUpper*/) {
    // Win32: path only. Windhawk's editor is VSCodium.exe with AppId
    // RAMENSOFTWARE.WINDHAWK — that is still one taskbar button.
    // UWP hosts (ApplicationFrameHost / WWAHost) share one exe for many
    // apps; key those by AppUserModelID so Calculator ≠ Settings.
    if (IsUwpHostPath(pathUpper) && !appIdUpper.empty()) {
        return std::wstring(L"APPID:") + CanonicalAppId(appIdUpper);
    }
    return pathUpper;
}

std::wstring PathFromAppKey(const std::wstring& key) {
    if (key.rfind(L"APPID:", 0) == 0) {
        return {};
    }
    auto pos = key.find(L"|CLS:");
    if (pos != std::wstring::npos) {
        return key.substr(0, pos);
    }
    return key;
}

std::wstring ClassFromAppKey(const std::wstring& key) {
    auto pos = key.find(L"|CLS:");
    if (pos == std::wstring::npos) {
        return {};
    }
    return key.substr(pos + 5);
}

std::wstring AppIdFromAppKey(const std::wstring& key) {
    if (key.rfind(L"APPID:", 0) != 0) {
        return {};
    }
    std::wstring rest = key.substr(6);
    auto pos = rest.find(L"|CLS:");
    if (pos != std::wstring::npos) {
        rest.resize(pos);
    }
    return rest;
}

bool SamePidAndClass(HWND a, HWND b) {
    if (!a || !b || !IsWindow(a) || !IsWindow(b)) {
        return false;
    }
    DWORD pa = 0, pb = 0;
    GetWindowThreadProcessId(a, &pa);
    GetWindowThreadProcessId(b, &pb);
    if (!pa || pa != pb) {
        return false;
    }
    return ToUpper(GetWindowClassName(a)) == ToUpper(GetWindowClassName(b));
}

bool ShouldIgnoreHwnd(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) {
        return true;
    }
    if (GetWindowLong(hWnd, GWL_STYLE) & WS_CHILD) {
        return true;
    }
    if (!IsWindowVisible(hWnd)) {
        return true;
    }

    WCHAR className[256]{};
    if (!GetClassName(hWnd, className, ARRAYSIZE(className))) {
        return true;
    }

    static const PCWSTR kIgnoredClasses[] = {
        L"Shell_TrayWnd",
        L"Shell_SecondaryTrayWnd",
        L"Shell_TrayWndDummy",
        L"Progman",
        L"WorkerW",
        L"XamlExplorerHostIslandWindow",
        L"ForegroundStaging",
        L"MultitaskingViewFrame",
        L"TaskListThumbnailWnd",
        L"Windows.Internal.Shell.TabProxyWindow",
        L"NotifyIconOverflowWindow",
        L"tooltips_class32",
    };

    for (auto ignored : kIgnoredClasses) {
        if (_wcsicmp(className, ignored) == 0) {
            return true;
        }
    }

    LONG exStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_TOOLWINDOW) {
        // Ignore typical tool popups, but keep sizable titled windows.
        // Total Commander Lister (and similar viewers) can be tool-styled
        // yet still appear as grouped taskbar thumbnails.
        if (GetWindowTextLengthW(hWnd) <= 0) {
            return true;
        }
        RECT rc{};
        if (!GetWindowRect(hWnd, &rc) || (rc.right - rc.left) < 200 ||
            (rc.bottom - rc.top) < 150) {
            return true;
        }
    }

    return false;
}

bool IsShellHostFileName(const std::wstring& fileNameUpper) {
    return fileNameUpper == L"EXPLORER.EXE" ||
           fileNameUpper == L"SEARCHHOST.EXE" ||
           fileNameUpper == L"STARTMENUXPERIENCEHOST.EXE" ||
           fileNameUpper == L"SHELLHOST.EXE" ||
           fileNameUpper == L"TEXTINPUTHOST.EXE";
}

// Alt-Tab UI, taskbar, desktop, IME: not a real app switch. Must not cancel
// an in-flight min-focus candidate — the landed app often sends no second
// FOREGROUND after these.
bool IsTransientForeground(HWND hWnd) {
    hWnd = NormalizeFocusHwnd(hWnd);
    if (ShouldIgnoreHwnd(hWnd)) {
        return true;
    }
    DWORD processId = 0;
    GetWindowThreadProcessId(hWnd, &processId);
    if (!processId || IsOwnExplorerProcess(processId)) {
        return true;
    }
    return IsShellHostFileName(
        ToUpper(FileNameFromPath(GetProcessImagePath(processId))));
}

std::wstring GetWindowTitle(HWND hWnd) {
    wchar_t buf[512];
    int n = GetWindowTextW(hWnd, buf, ARRAYSIZE(buf));
    if (n <= 0) {
        return {};
    }
    return std::wstring(buf, static_cast<size_t>(n));
}

bool ResolveAppIdentity(HWND hWnd,
                        std::wstring& outKey,
                        std::wstring& outDisplayName,
                        DWORD& outProcessId,
                        std::wstring* outWindowTitle = nullptr) {
    outKey.clear();
    outDisplayName.clear();
    outProcessId = 0;
    if (outWindowTitle) {
        outWindowTitle->clear();
    }

    hWnd = NormalizeFocusHwnd(hWnd);
    if (ShouldIgnoreHwnd(hWnd)) {
        return false;
    }

    DWORD processId = 0;
    GetWindowThreadProcessId(hWnd, &processId);
    if (!processId || IsOwnExplorerProcess(processId)) {
        return false;
    }

    std::wstring path = GetProcessImagePath(processId);
    if (path.empty()) {
        return false;
    }

    std::wstring pathUpper = ToUpper(path);
    std::wstring fileName = FileNameFromPath(path);
    std::wstring fileNameUpper = ToUpper(fileName);

    if (IsShellHostFileName(fileNameUpper)) {
        return false;
    }

    std::wstring appIdUpper = ToUpper(GetWindowAppUserModelId(hWnd));
    std::wstring classUpper = ToUpper(GetWindowClassName(hWnd));

    if (IsUwpHostFileName(fileNameUpper) && appIdUpper.empty()) {
        return false;
    }

    const std::wstring key = MakeAppKey(pathUpper, appIdUpper, classUpper);
    const std::wstring title = GetWindowTitle(hWnd);
    std::wstring displayName = fileName;
    if (IsUwpHostFileName(fileNameUpper)) {
        // Prefer "Settings" over WINDOWS.IMMERSIVECONTROLPANEL for logs /
        // exclude / fuzzy. Key remains APPID:… .
        if (!title.empty() && title.size() <= 80 &&
            title.find(L'\\') == std::wstring::npos &&
            title.find(L'/') == std::wstring::npos) {
            displayName = title;
        } else {
            displayName = DisplayNameFromAppId(appIdUpper);
        }
    }

    if (IsExcludedKey(key, ToUpper(displayName)) ||
        (!appIdUpper.empty() &&
         SettingsSnap()->excludedPrograms.contains(appIdUpper))) {
        Wh_Log(L"Excluded: %s", displayName.c_str());
        return false;
    }

    outKey = key;
    outDisplayName = displayName;
    outProcessId = processId;
    if (outWindowTitle) {
        *outWindowTitle = title;
    }
    return true;
}

FrameworkElement FindChildByName(FrameworkElement element, PCWSTR name) {
    if (!element) {
        return nullptr;
    }

    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            continue;
        }
        if (child.Name() == name) {
            return child;
        }
    }
    return nullptr;
}

FrameworkElement FindDescendantByName(FrameworkElement element, PCWSTR name) {
    if (!element) {
        return nullptr;
    }
    if (element.Name() == name) {
        return element;
    }

    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            continue;
        }
        if (auto found = FindDescendantByName(child, name)) {
            return found;
        }
    }
    return nullptr;
}

// ---------------------------------------------------------------------------
// Ranking
// ---------------------------------------------------------------------------

// True if any cached TaskListButton resolved to this process path (or same
// file name). Call from UI thread after EnsureButtonPathCached, or any thread
// if only reading the path cache.
bool PathAppearsOnTaskbar(const std::wstring& keyOrPath,
                          const std::wstring& displayName) {
    const std::wstring pathUpper = PathFromAppKey(keyOrPath);
    const std::wstring wantAppId = CanonicalAppId(AppIdFromAppKey(keyOrPath));
    std::wstring fileUpper = ToUpper(displayName);
    if (fileUpper.empty() && !pathUpper.empty()) {
        fileUpper = ToUpper(FileNameFromPath(pathUpper));
    }
    if (pathUpper.empty() && fileUpper.empty() && wantAppId.empty()) {
        return false;
    }

    std::lock_guard<std::mutex> lock(g_buttonPathMutex);
    for (const auto& e : g_buttonPathCache) {
        if (!wantAppId.empty()) {
            std::wstring id = e.appIdUpper.empty() ? e.autoIdUpper : e.appIdUpper;
            if (CanonicalAppId(id) == wantAppId) {
                return true;
            }
        }
        if (e.pathUpper.empty()) {
            continue;
        }
        // Class is for *which* button to highlight, not whether the app
        // exists on the taskbar (TC + Lister share a path, different class).
        if (!pathUpper.empty() && e.pathUpper == pathUpper) {
            return true;
        }
        if (!fileUpper.empty() &&
            ToUpper(FileNameFromPath(e.pathUpper)) == fileUpper) {
            return true;
        }
    }
    return false;
}

// Same process path, two taskbar icons (TOTALCMD64 → Total Commander + Lister).
bool PathHasSplitTaskbarButtons(const std::wstring& pathUpper) {
    if (pathUpper.empty()) {
        return false;
    }
    const std::wstring fileUpper = ToUpper(FileNameFromPath(pathUpper));
    std::wstring firstClass;
    bool sawLister = false;
    bool sawOther = false;
    std::lock_guard<std::mutex> lock(g_buttonPathMutex);
    for (const auto& e : g_buttonPathCache) {
        if (e.pathUpper.empty()) {
            continue;
        }
        if (e.pathUpper != pathUpper &&
            ToUpper(FileNameFromPath(e.pathUpper)) != fileUpper) {
            continue;
        }
        if (e.classUpper.find(L"LISTER") != std::wstring::npos) {
            sawLister = true;
        } else if (!e.classUpper.empty()) {
            sawOther = true;
        }
        if (firstClass.empty()) {
            firstClass = e.classUpper;
        } else if (!e.classUpper.empty() && e.classUpper != firstClass) {
            return true;
        }
    }
    return sawLister && sawOther;
}

void RecomputeRanksForDesktopLocked(DesktopRecencyState& desk) {
    desk.rankedApps.clear();

    auto settings = SettingsSnap();
    if (!settings->enabled || g_unloading.load()) {
        return;
    }

    const ULONGLONG now = GetTickCount64();
    const ULONGLONG decayMs = DecayMsFromMinutes(settings->decayMinutes);

    std::vector<AppFocusInfo> candidates;
    candidates.reserve(desk.appFocusMap.size());

    for (auto it = desk.appFocusMap.begin(); it != desk.appFocusMap.end();) {
        auto& info = it->second;
        if (info.lastConfirmedFocusTick == 0) {
            ++it;
            continue;
        }
        if (IsTickDecayed(info.lastConfirmedFocusTick, decayMs, now)) {
            Wh_Log(L"Decayed: %s", info.displayName.c_str());
            const std::wstring decayedKey = it->first;
            it = desk.appFocusMap.erase(it);
            bool stillUsed = false;
            for (const auto& [oid, other] : g_desktopMaps) {
                if (other.appFocusMap.contains(decayedKey)) {
                    stillUsed = true;
                    break;
                }
            }
            if (!stillUsed) {
                g_keyToAutomationName.erase(decayedKey);
            }
            continue;
        }
        // Tray-only / no taskbar button: keep optional history but never rank.
        if (settings->requireTaskbarButton && !info.seenOnTaskbar) {
            // Refresh from path cache if buttons resolved since last time.
            if (PathAppearsOnTaskbar(info.key, info.displayName)) {
                info.seenOnTaskbar = true;
            } else {
                ++it;
                continue;
            }
        }
        candidates.push_back(info);
        ++it;
    }

    std::sort(candidates.begin(), candidates.end(),
              [](const AppFocusInfo& a, const AppFocusInfo& b) {
                  return a.lastConfirmedFocusTick > b.lastConfirmedFocusTick;
              });

    const int limit = (std::max)(0, settings->highlightCount);
    if (static_cast<int>(candidates.size()) > limit) {
        candidates.resize(static_cast<size_t>(limit));
    }

    desk.rankedApps = std::move(candidates);
}

void RecomputeRanksLocked() {
    RecomputeRanksForDesktopLocked(CurrentDeskLocked());
}

// ---------------------------------------------------------------------------
// Window-level recency (thumbnail previews)
// ---------------------------------------------------------------------------

void PruneWindowFocusMapLocked(DesktopRecencyState& desk);
void StampWindowRecencyLocked(DesktopRecencyState& desk,
                              HWND hwnd,
                              const std::wstring& processKey,
                              const std::wstring& windowTitle,
                              ULONGLONG now);

void ConfirmPreviewFocusNow(HWND hwnd) {
    if (!hwnd || g_unloading.load() || !SettingsSnap()->enabled ||
        !SettingsSnap()->previewHighlightEnabled) {
        return;
    }
    hwnd = NormalizeFocusHwnd(hwnd);
    if (ShouldIgnoreHwnd(hwnd)) {
        return;
    }

    std::wstring key;
    std::wstring displayName;
    std::wstring windowTitle;
    DWORD processId = 0;
    if (!ResolveAppIdentity(hwnd, key, displayName, processId, &windowTitle)) {
        return;
    }
    if (IsExcludedKey(key, ToUpper(displayName))) {
        return;
    }

    const ULONGLONG now = GetTickCount64();
    RefreshCurrentDesktopId();
    std::wstring processKey = PathFromAppKey(key);
    if (processKey.empty()) {
        processKey = ToUpper(GetProcessImagePath(processId));
    }
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        auto& desk = CurrentDeskLocked();
        StampWindowRecencyLocked(desk, hwnd, processKey, windowTitle, now);
        Wh_Log(L"Preview click confirmed: hwnd=%p %s title=\"%s\" (map=%zu "
               L"desktop=%s)",
               hwnd, displayName.c_str(), windowTitle.c_str(),
               desk.windowFocusMap.size(),
               GuidToLogString(g_currentDesktopId).c_str());
    }
    RequestApplyPreviewVisuals();
}

void PruneWindowFocusMapLocked(DesktopRecencyState& desk) {
    const ULONGLONG now = GetTickCount64();
    const ULONGLONG decayMs =
        DecayMsFromMinutes(SettingsSnap()->previewDecayMinutes);

    for (auto it = desk.windowFocusMap.begin();
         it != desk.windowFocusMap.end();) {
        if (!it->first || !IsWindow(it->first) ||
            !HwndMatchesStoredPid(it->first, it->second.pid)) {
            it = desk.windowFocusMap.erase(it);
            continue;
        }
        if (IsTickDecayed(it->second.lastConfirmedTick, decayMs, now)) {
            it = desk.windowFocusMap.erase(it);
            continue;
        }
        ++it;
    }
}

// Requires g_stateMutex.
void StampWindowRecencyLocked(DesktopRecencyState& desk,
                              HWND hwnd,
                              const std::wstring& processKey,
                              const std::wstring& windowTitle,
                              ULONGLONG now) {
    if (!hwnd) {
        return;
    }
    const DWORD pid = PidFromHwnd(hwnd);
    auto it = desk.windowFocusMap.find(hwnd);
    if (it != desk.windowFocusMap.end() && it->second.pid != 0 && pid != 0 &&
        it->second.pid != pid) {
        Wh_Log(L"HWND recycled, dropping stale preview recency: hwnd=%p "
               L"oldPid=%u newPid=%u",
               hwnd, it->second.pid, pid);
        desk.windowFocusMap.erase(it);
    }
    WindowFocusInfo& winfo = desk.windowFocusMap[hwnd];
    winfo.hwnd = hwnd;
    winfo.pid = pid;
    if (!processKey.empty()) {
        winfo.processKey = processKey;
    }
    if (!windowTitle.empty()) {
        winfo.windowTitle = windowTitle;
    }
    winfo.lastConfirmedTick = now;
    winfo.confirmSeq = g_windowConfirmSeq.fetch_add(1) + 1;
    PruneWindowFocusMapLocked(desk);
}

// True if hwnd is still a non-decayed confirmed recent window on this desktop.
bool IsWindowRecentForPreviewLocked(DesktopRecencyState& desk,
                                    HWND hwnd,
                                    ULONGLONG* outTick = nullptr) {
    if (!hwnd) {
        return false;
    }
    auto it = desk.windowFocusMap.find(hwnd);
    if (it == desk.windowFocusMap.end() || it->second.lastConfirmedTick == 0) {
        return false;
    }
    if (!IsWindow(hwnd) || !HwndMatchesStoredPid(hwnd, it->second.pid)) {
        return false;
    }
    const ULONGLONG now = GetTickCount64();
    const ULONGLONG decayMs =
        DecayMsFromMinutes(SettingsSnap()->previewDecayMinutes);
    if (IsTickDecayed(it->second.lastConfirmedTick, decayMs, now)) {
        return false;
    }
    if (outTick) {
        *outTick = it->second.lastConfirmedTick;
    }
    return true;
}

// Snapshot of recent windows for UI matching (copy under lock).
std::vector<WindowFocusInfo> CopyRecentWindowsForPreview() {
    RefreshCurrentDesktopId();
    std::lock_guard<std::mutex> lock(g_stateMutex);
    auto& desk = CurrentDeskLocked();
    PruneWindowFocusMapLocked(desk);
    std::vector<WindowFocusInfo> out;
    out.reserve(desk.windowFocusMap.size());
    for (const auto& [hwnd, info] : desk.windowFocusMap) {
        if (IsWindowRecentForPreviewLocked(desk, hwnd)) {
            out.push_back(info);
        }
    }
    std::sort(out.begin(), out.end(),
              [](const WindowFocusInfo& a, const WindowFocusInfo& b) {
                  return a.lastConfirmedTick > b.lastConfirmedTick;
              });
    return out;
}

// ---------------------------------------------------------------------------
// Color / style helpers (UI thread)
// ---------------------------------------------------------------------------

bool ParseHexColor(const std::wstring& text, winrt::Windows::UI::Color& out) {
    std::wstring s = text;
    if (!s.empty() && s[0] == L'#') {
        s.erase(0, 1);
    }
    // Allow 0x prefix
    if (s.size() >= 2 && s[0] == L'0' && (s[1] == L'x' || s[1] == L'X')) {
        s.erase(0, 2);
    }

    auto hexVal = [](wchar_t c) -> int {
        if (c >= L'0' && c <= L'9') {
            return c - L'0';
        }
        if (c >= L'a' && c <= L'f') {
            return c - L'a' + 10;
        }
        if (c >= L'A' && c <= L'F') {
            return c - L'A' + 10;
        }
        return -1;
    };

    auto readByte = [&](size_t i) -> int {
        int hi = hexVal(s[i]);
        int lo = hexVal(s[i + 1]);
        if (hi < 0 || lo < 0) {
            return -1;
        }
        return (hi << 4) | lo;
    };

    out.A = 255;
    if (s.size() == 8) {
        int a = readByte(0), r = readByte(2), g = readByte(4), b = readByte(6);
        if (a < 0 || r < 0 || g < 0 || b < 0) {
            return false;
        }
        out.A = static_cast<uint8_t>(a);
        out.R = static_cast<uint8_t>(r);
        out.G = static_cast<uint8_t>(g);
        out.B = static_cast<uint8_t>(b);
        return true;
    }
    if (s.size() == 6) {
        int r = readByte(0), g = readByte(2), b = readByte(4);
        if (r < 0 || g < 0 || b < 0) {
            return false;
        }
        out.R = static_cast<uint8_t>(r);
        out.G = static_cast<uint8_t>(g);
        out.B = static_cast<uint8_t>(b);
        return true;
    }
    return false;
}

winrt::Windows::UI::Color ResolveGlowBaseColor() {
    winrt::Windows::UI::Color c{255, 0, 200, 83};  // default green-ish

    switch (SettingsSnap()->glowColor) {
        case GlowColorMode::Accent:
            try {
                winrt::Windows::UI::ViewManagement::UISettings uiSettings;
                c = uiSettings.GetColorValue(
                    winrt::Windows::UI::ViewManagement::UIColorType::Accent);
                c.A = 255;
            } catch (...) {
                c = {255, 0, 120, 215};
            }
            break;
        case GlowColorMode::Green:
            c = {255, 0, 200, 83};
            break;
        case GlowColorMode::Blue:
            c = {255, 30, 144, 255};
            break;
        case GlowColorMode::Orange:
            c = {255, 255, 140, 0};
            break;
        case GlowColorMode::White:
            c = {255, 255, 255, 255};
            break;
        case GlowColorMode::Custom:
            if (!ParseHexColor(SettingsSnap()->customGlowColor, c)) {
                c = {255, 0, 200, 83};
            }
            break;
    }
    return c;
}

int RankIntensity(int rankZeroBased) {
    int idx = rankZeroBased < 3 ? rankZeroBased : 2;
    return SettingsSnap()->glowIntensity[idx];
}

int PreviewRankIntensity(int rankZeroBased) {
    int idx = rankZeroBased < 3 ? rankZeroBased : 2;
    return SettingsSnap()->previewIntensity[idx];
}

int RankSizeBoost(int rankZeroBased) {
    int idx = rankZeroBased < 3 ? rankZeroBased : 2;
    return SettingsSnap()->sizeBoostPercent[idx];
}

// Defined with option-C resolve stack (button → process path).
std::wstring EnsureButtonPathCached(FrameworkElement button, bool force);
std::wstring GetCachedButtonPath(FrameworkElement button);
struct ButtonIdentity {
    std::wstring pathUpper;
    std::wstring appIdUpper;
    std::wstring classUpper;
    std::wstring autoIdUpper;
    DWORD pid = 0;
    HWND sampleHwnd = nullptr;
    std::vector<HWND> groupHwnds;
};
ButtonIdentity GetCachedButtonIdentity(FrameworkElement button);
bool RunOnUiThread(const winrt::Windows::UI::Core::DispatchedHandler& handler);
void RefreshThumbnailFlyout_UIThread(FrameworkElement anyThumb);

// ---------------------------------------------------------------------------
// Button identity matching
// ---------------------------------------------------------------------------

using TaskListButton_get_IsRunning_t = HRESULT(WINAPI*)(void* pThis,
                                                        bool* running);
TaskListButton_get_IsRunning_t TaskListButton_get_IsRunning_Original;

bool TaskListButton_IsRunning(FrameworkElement taskListButtonElement) {
    if (!TaskListButton_get_IsRunning_Original || !taskListButtonElement) {
        return false;
    }
    bool isRunning = false;
    HRESULT hr = TaskListButton_get_IsRunning_Original(
        winrt::get_abi(
            taskListButtonElement.as<winrt::Windows::Foundation::IUnknown>()),
        &isRunning);
    return SUCCEEDED(hr) && isRunning;
}

// IsRunning, plus a short grace so Alt-Tab flicker does not drop glows.
// Virtual-desktop switches clear lastRunningTick so pinned-not-running icons
// on another desktop never keep a highlight.
bool ButtonCountsAsRunning(FrameworkElement button) {
    const bool running = TaskListButton_IsRunning(button);
    const ULONGLONG now = GetTickCount64();
    std::lock_guard<std::mutex> lock(g_buttonPathMutex);
    for (auto& e : g_buttonPathCache) {
        try {
            if (e.button.get() != button) {
                continue;
            }
            if (running) {
                e.lastRunningTick = now;
                return true;
            }
            return e.lastRunningTick != 0 &&
                   now - e.lastRunningTick < kIsRunningGraceMs;
        } catch (...) {
        }
    }
    return running;
}

std::wstring GetButtonAutomationName(FrameworkElement button) {
    try {
        return std::wstring(
            Automation::AutomationProperties::GetName(button).c_str());
    } catch (...) {
        return {};
    }
}

std::wstring GetButtonAutomationAppId(FrameworkElement button) {
    if (!button) {
        return {};
    }
    try {
        std::wstring id =
            Automation::AutomationProperties::GetAutomationId(button).c_str();
        return ToUpper(StripAppIdPrefix(std::move(id)));
    } catch (...) {
        return {};
    }
}

bool IsVisualStateActive(FrameworkElement root) {
    if (!root) {
        return false;
    }

    try {
        auto groups = VisualStateManager::GetVisualStateGroups(root);
        for (auto group : groups) {
            auto current = group.CurrentState();
            if (!current) {
                continue;
            }
            std::wstring name(current.Name().c_str());
            // "InactivePointerOver".find("Active") is a hit — that made every
            // hovered running button look focused (TC stole Lister's glow).
            if (name.rfind(L"Inactive", 0) == 0) {
                continue;
            }
            if (name.rfind(L"Active", 0) == 0) {
                return true;
            }
        }
    } catch (...) {
    }

    // Also check children that host state groups (IconPanel).
    try {
        int n = Media::VisualTreeHelper::GetChildrenCount(root);
        for (int i = 0; i < n; i++) {
            auto child = Media::VisualTreeHelper::GetChild(root, i)
                             .try_as<FrameworkElement>();
            if (child && IsVisualStateActive(child)) {
                return true;
            }
        }
    } catch (...) {
    }

    return false;
}

// Taskbar names look like "App - 2 running windows pinned" — strip that noise.
// Do NOT cut at the first " - ": Lister titles are
// "Lister - [C:\\path\\file.txt] - 3 running windows and 1 group".
std::wstring NormalizeAutomationName(std::wstring name) {
    auto isRunningCountSuffix = [](std::wstring_view tail) -> bool {
        size_t i = 0;
        while (i < tail.size() && tail[i] == L' ') {
            ++i;
        }
        if (i >= tail.size() || tail[i] < L'0' || tail[i] > L'9') {
            return false;
        }
        while (i < tail.size() && tail[i] >= L'0' && tail[i] <= L'9') {
            ++i;
        }
        if (i >= tail.size() || tail[i] != L' ') {
            return false;
        }
        ++i;
        constexpr wchar_t kRun[] = L"running";
        if (i + 7 > tail.size()) {
            return false;
        }
        for (int k = 0; k < 7; ++k) {
            wchar_t c = tail[i + static_cast<size_t>(k)];
            if (c >= L'A' && c <= L'Z') {
                c = static_cast<wchar_t>(c - L'A' + L'a');
            }
            if (c != kRun[k]) {
                return false;
            }
        }
        return true;
    };

    size_t cut = std::wstring::npos;
    for (size_t search = 0; search + 3 < name.size();) {
        const auto pos = name.find(L" - ", search);
        if (pos == std::wstring::npos) {
            break;
        }
        if (isRunningCountSuffix(std::wstring_view(name).substr(pos + 3))) {
            cut = pos;
            break;
        }
        search = pos + 3;
    }
    if (cut != std::wstring::npos) {
        name.resize(cut);
    }
    // Trailing " pinned"
    constexpr wchar_t kPinned[] = L" pinned";
    if (name.size() > 7) {
        auto off = name.size() - 7;
        if (_wcsicmp(name.c_str() + off, kPinned) == 0) {
            name.resize(off);
        }
    }
    // Trim spaces
    while (!name.empty() && name.back() == L' ') {
        name.pop_back();
    }
    return name;
}

// True for Lister-style "[c:\temp\file.txt]" / "[book.epub]", not Calibre's
// format tag "[EPUB]" (same on every book — must not be an identity key).
bool LooksLikeFilePath(const std::wstring& s) {
    if (s.empty()) {
        return false;
    }
    if (s.find(L'\\') != std::wstring::npos ||
        s.find(L'/') != std::wstring::npos) {
        return true;
    }
    if (s.size() >= 2 && ((s[0] >= L'A' && s[0] <= L'Z') ||
                          (s[0] >= L'a' && s[0] <= L'z')) &&
        s[1] == L':') {
        return true;
    }
    const auto dot = s.rfind(L'.');
    return dot != std::wstring::npos && dot > 0 && dot + 1 < s.size();
}

std::wstring ExtractBracketedPath(const std::wstring& s) {
    const auto open = s.find(L'[');
    const auto close = s.rfind(L']');
    if (open == std::wstring::npos || close == std::wstring::npos ||
        close <= open + 1) {
        return {};
    }
    std::wstring inner = s.substr(open + 1, close - open - 1);
    if (!LooksLikeFilePath(inner)) {
        return {};
    }
    return inner;
}

// Strip marketing suffixes so WINDOWSTERMINAL ≈ TERMINALPREVIEW → TERMINAL.
std::wstring StripProductNoise(std::wstring alnumUpper) {
    static const wchar_t* kNoise[] = {L"PREVIEW", L"BETA",   L"PORTABLE",
                                      L"CANARY",  L"INSIDER", L"NIGHTLY"};
    for (auto noise : kNoise) {
        for (;;) {
            auto pos = alnumUpper.find(noise);
            if (pos == std::wstring::npos) {
                break;
            }
            alnumUpper.erase(pos, wcslen(noise));
        }
    }
    return alnumUpper;
}

// True if every char of `needle` appears in order inside `haystack`
// (not necessarily contiguous). TOTALCMD64 ⊂ TOTALCOMMANDER64BIT.
bool IsSubsequence(const std::wstring& needle, const std::wstring& haystack) {
    if (needle.empty()) {
        return false;
    }
    size_t j = 0;
    for (size_t i = 0; i < haystack.size() && j < needle.size(); ++i) {
        if (haystack[i] == needle[j]) {
            ++j;
        }
    }
    return j == needle.size();
}

// Initials of title words: "Total Commander 64 bit" → TC64B
std::wstring InitialsAlnum(const std::wstring& automationName) {
    std::wstring title = NormalizeAutomationName(automationName);
    std::wstring out;
    bool atWord = true;
    for (wchar_t ch : title) {
        if (ch == L' ' || ch == L'-' || ch == L'_' || ch == L'.') {
            atWord = true;
            continue;
        }
        if (atWord) {
            if ((ch >= L'a' && ch <= L'z') || (ch >= L'A' && ch <= L'Z') ||
                (ch >= L'0' && ch <= L'9')) {
                if (ch >= L'a' && ch <= L'z') {
                    ch = static_cast<wchar_t>(ch - L'a' + L'A');
                }
                out.push_back(ch);
            }
            atWord = false;
        }
    }
    return out;
}

// Higher = better. 0 = no match.
// Important: VSCodium must NOT match "Visual Studio Code" (initials VSC alone
// used to fire). Prefer longer/more specific matches; callers assign 1:1.
int ScoreExeToAutomationName(const std::wstring& displayName,
                             const std::wstring& automationName) {
    if (displayName.empty() || automationName.empty()) {
        return 0;
    }

    std::wstring exeAlnum = AlnumUpper(StripExtension(displayName));
    std::wstring autoAlnum = AlnumUpper(NormalizeAutomationName(automationName));
    if (exeAlnum.empty() || autoAlnum.empty()) {
        return 0;
    }

    if (exeAlnum == autoAlnum) {
        return 100;
    }

    // Contiguous containment (CODE ⊂ VISUALSTUDIOCODE, STEAM ⊂ STEAM…).
    if (exeAlnum.size() >= 4 &&
        autoAlnum.find(exeAlnum) != std::wstring::npos) {
        return 92;
    }
    if (autoAlnum.size() >= 4 &&
        exeAlnum.find(autoAlnum) != std::wstring::npos) {
        // Prefer when title is a large part of the exe (STEAM vs STEAMWEBHELPER).
        int cover = static_cast<int>(autoAlnum.size() * 100 / exeAlnum.size());
        return 80 + (std::min)(12, cover / 10);
    }

    std::wstring exeN = StripProductNoise(exeAlnum);
    std::wstring autoN = StripProductNoise(autoAlnum);
    if (!exeN.empty() && exeN == autoN) {
        return 90;
    }
    if (exeN.size() >= 5 && autoN.find(exeN) != std::wstring::npos) {
        return 88;
    }
    if (autoN.size() >= 5 && exeN.find(autoN) != std::wstring::npos) {
        return 84;
    }

    // Subsequence: TOTALCMD64 ⊂ TOTALCOMMANDER64BIT (not short initials).
    std::wstring exeLetters;
    for (wchar_t c : exeN) {
        if (c < L'0' || c > L'9') {
            exeLetters.push_back(c);
        }
    }
    if (exeN.size() >= 5 && IsSubsequence(exeN, autoN)) {
        return 72;
    }
    if (exeLetters.size() >= 5 && IsSubsequence(exeLetters, autoN)) {
        return 70;
    }

    // Initials only when they are a *substantial* prefix of the exe.
    // VSC vs VSCODIUM (3/8) → reject; avoids VS Code button stealing VSCodium.
    // TC64 vs TOTALCMD64 — initials may be TC64B; require prefix of exe length
    // and initials covering ≥ half the exe stem.
    std::wstring initials = InitialsAlnum(automationName);
    if (initials.size() >= 3) {
        if (exeN.rfind(initials, 0) == 0 &&
            initials.size() * 2 >= exeN.size()) {
            return 55;
        }
        // Exe is essentially the initials (rare).
        if (exeN == initials) {
            return 95;
        }
    }

    return 0;
}

bool ExeMatchesAutomationName(const std::wstring& displayName,
                              const std::wstring& automationName) {
    return ScoreExeToAutomationName(displayName, automationName) > 0;
}

// Window title ↔ taskbar button title (both normalized).
int ScoreTitleToAutomationName(const std::wstring& windowTitle,
                               const std::wstring& automationName) {
    if (windowTitle.empty() || automationName.empty()) {
        return 0;
    }
    std::wstring t = AlnumUpper(NormalizeAutomationName(windowTitle));
    std::wstring a = AlnumUpper(NormalizeAutomationName(automationName));
    if (t.empty() || a.empty()) {
        return 0;
    }
    if (t == a) {
        return 98;
    }
    // Button title contained in window title or vice versa (min length 4).
    if (t.size() >= 4 && a.find(t) != std::wstring::npos) {
        return 93;
    }
    if (a.size() >= 4 && t.find(a) != std::wstring::npos) {
        return 91;
    }
    // Significant shared prefix (e.g. WINDHAWK…).
    // Do not use this when both sides have distinct [bracket] paths —
    // "Lister - [c:\tmp\a.txt]" vs "Lister - [c:\tmp\c.txt]" share LISTERCTMP.
    const std::wstring tPath =
        ToUpper(ExtractBracketedPath(NormalizeAutomationName(windowTitle)));
    const std::wstring aPath =
        ToUpper(ExtractBracketedPath(NormalizeAutomationName(automationName)));
    if (!tPath.empty() && !aPath.empty() && tPath != aPath) {
        return 0;
    }
    size_t pref = 0;
    while (pref < t.size() && pref < a.size() && t[pref] == a[pref]) {
        ++pref;
    }
    if (pref >= 6) {
        // Shared prefix only: "HarryPotter1" vs "HarryPotter2" (or two
        // "EbookReader…" books) must not count as a match.
        if (t.size() > pref && a.size() > pref) {
            return 0;
        }
        return 85;
    }
    return 0;
}

// True if this button's automation name is a legitimate label for the rank.
// Class-qualified ranks (TLISTER) must not bind to "Total Commander".
bool AutomationNameFitsRank(const AppFocusInfo& info,
                            const std::wstring& autoName) {
    if (autoName.empty()) {
        return false;
    }
    const std::wstring cls = !info.classUpper.empty()
                                 ? info.classUpper
                                 : ClassFromAppKey(info.key);
    if (!cls.empty() && cls.find(L"LISTER") != std::wstring::npos) {
        return AlnumUpper(NormalizeAutomationName(autoName)).find(L"LISTER") !=
               std::wstring::npos;
    }
    return ExeMatchesAutomationName(info.displayName, autoName);
}

void StoreAutomationNameIfFits(const AppFocusInfo& info,
                               const std::wstring& autoName) {
    if (!AutomationNameFitsRank(info, autoName)) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_stateMutex);
    g_keyToAutomationName[info.key] = autoName;
}

void StoreAutomationName(const AppFocusInfo& info,
                         const std::wstring& autoName) {
    if (info.key.empty() || autoName.empty()) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_stateMutex);
    g_keyToAutomationName[info.key] = autoName;
}

// Trusted writes only: path/exe binds, or Active-button associate (Windhawk
// button for VSCodium.exe). Name-equals is then enough, except we refuse
// a non-fitting name when this exe has two icons (TC vs Lister).
bool CacheEntryValid(const AppFocusInfo& info,
                     const std::wstring& cachedAutoName,
                     const std::wstring& buttonAutoName) {
    if (cachedAutoName.empty() || buttonAutoName.empty()) {
        return false;
    }
    const bool nameEquals =
        _wcsicmp(cachedAutoName.c_str(), buttonAutoName.c_str()) == 0 ||
        _wcsicmp(NormalizeAutomationName(cachedAutoName).c_str(),
                 NormalizeAutomationName(buttonAutoName).c_str()) == 0;
    if (!nameEquals) {
        return false;
    }
    if (AutomationNameFitsRank(info, buttonAutoName)) {
        return true;
    }
    const std::wstring path = PathFromAppKey(info.key);
    return !PathHasSplitTaskbarButtons(path);
}

// Score this button against one ranked app. Higher is better; 0 = no match.
int ScoreButtonForRank(FrameworkElement button,
                       const AppFocusInfo& info,
                       bool requireRunning) {
    if (!button) {
        return 0;
    }
    if (requireRunning && !ButtonCountsAsRunning(button)) {
        return 0;
    }

    const ButtonIdentity ident = GetCachedButtonIdentity(button);
    const std::wstring rankPath = PathFromAppKey(info.key);
    const std::wstring rankCls = !info.classUpper.empty()
                                     ? info.classUpper
                                     : ClassFromAppKey(info.key);
    const std::wstring rankAppId = !info.appIdUpper.empty()
                                       ? info.appIdUpper
                                       : AppIdFromAppKey(info.key);

    auto hwndOnButton = [&](HWND h) -> bool {
        if (!h) {
            return false;
        }
        if (ident.sampleHwnd == h) {
            return true;
        }
        for (HWND g : ident.groupHwnds) {
            if (g == h) {
                return true;
            }
        }
        if (ident.sampleHwnd && SamePidAndClass(h, ident.sampleHwnd)) {
            // ApplicationFrameHost hosts many apps; PID+class is not identity.
            if (!IsAppIdKey(info.key) && !IsUwpHostPath(ident.pathUpper)) {
                return true;
            }
        }
        return false;
    };

    if (hwndOnButton(info.lastHwnd)) {
        return kScoreExactIdentity;
    }

    if (IsAppIdKey(info.key)) {
        const std::wstring want = AppIdFromAppKey(info.key);
        std::wstring got =
            ident.appIdUpper.empty() ? ident.autoIdUpper : ident.appIdUpper;
        got = CanonicalAppId(got);
        if (!want.empty() && got == want) {
            return kScoreExactIdentity;
        }
        // Hosted UWP ranks are AUMID-only. Do not fuzzy "WINDOWS.IMMERSIVE…"
        // onto Windows Security, or reuse a name-cache replica.
        return 0;
    }

    const std::wstring pathKey =
        !rankPath.empty() ? rankPath : PathFromAppKey(info.key);
    const bool exactPath =
        !ident.pathUpper.empty() &&
        (ident.pathUpper == info.key || ident.pathUpper == pathKey);
    const bool sameFileName =
        !ident.pathUpper.empty() && !info.displayName.empty() &&
        ToUpper(FileNameFromPath(ident.pathUpper)) ==
            ToUpper(info.displayName);
    // Both sides resolved, folders differ: not a replica (portable python).
    const bool pathConflict =
        sameFileName && !exactPath && !pathKey.empty() &&
        ident.pathUpper != pathKey && ident.pathUpper != info.key;
    const bool pathOk = exactPath || (sameFileName && !pathConflict);
    const bool split = PathHasSplitTaskbarButtons(
        !pathKey.empty() ? pathKey : ident.pathUpper);

    // Path cache hit: skip fuzzy names (VS Code vs VSCodium stay distinct
    // via full path). Fuzzy runs only when path is missing or this exe has
    // two taskbar icons (TOTALCMD64 → Commander + Lister).
    if (pathOk) {
        if (!split) {
            return exactPath ? kScoreExactIdentity
                             : kScoreSameFileDifferentPath;
        }
        if (!rankCls.empty() && ident.classUpper == rankCls) {
            return kScoreExactIdentity;
        }
        if (!rankCls.empty() && !ident.classUpper.empty() &&
            ident.classUpper != rankCls) {
            // other half of a split pair
        } else if (rankCls.empty() || ident.classUpper.empty()) {
            return kScoreSameFileDifferentPath;
        }
    }

    const bool identityConflict =
        pathConflict ||
        (split && !rankCls.empty() && !ident.classUpper.empty() &&
         ident.classUpper != rankCls);

    std::wstring autoName = GetButtonAutomationName(button);
    if (autoName.empty()) {
        return 0;
    }

    // Name fallback when path cache missed: "Lister" button vs TLister focus.
    if (!rankCls.empty() &&
        rankCls.find(L"LISTER") != std::wstring::npos) {
        std::wstring n = AlnumUpper(NormalizeAutomationName(autoName));
        if (n.find(L"LISTER") != std::wstring::npos) {
            return kScoreExactIdentity;
        }
    }

    int score = 0;
    if (!identityConflict) {
        score = ScoreExeToAutomationName(info.displayName, autoName);
    }

    // Do not score last window/tab title against taskbar buttons.
    // Terminal tabs (and VSCodium editing a Windhawk mod) rename the HWND
    // to the document — that is preview identity, not which icon to glow.

    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        auto it = g_keyToAutomationName.find(info.key);
        if (!identityConflict && it != g_keyToAutomationName.end() &&
            !it->second.empty()) {
            if (CacheEntryValid(info, it->second, autoName)) {
                score = (std::max)(score, kScoreNameCache);
            } else if (_wcsicmp(it->second.c_str(), autoName.c_str()) == 0 ||
                       _wcsicmp(NormalizeAutomationName(it->second).c_str(),
                                NormalizeAutomationName(autoName).c_str()) ==
                           0) {
                Wh_Log(L"Dropping bad cache: %s was \"%s\" (exe/class mismatch)",
                       info.displayName.c_str(), it->second.c_str());
                g_keyToAutomationName.erase(it);
            }
        }
    }
    return score;
}

// Best rank for a single button (1-based), or 0. Prefer highest score.
// requireRunning uses ButtonCountsAsRunning (short Alt-Tab grace).
int FindRankForButton(FrameworkElement button,
                      const std::vector<AppFocusInfo>& ranks,
                      bool requireRunning = true) {
    if (!button || ranks.empty()) {
        return 0;
    }

    int bestRank = 0;
    int bestScore = 0;
    for (size_t i = 0; i < ranks.size(); i++) {
        int s = ScoreButtonForRank(button, ranks[i], requireRunning);
        if (s > bestScore) {
            bestScore = s;
            bestRank = static_cast<int>(i) + 1;
        }
    }
    // Minimum confidence: reject weak initials-only if ever reintroduced.
    if (bestScore < kScoreMinBind) {
        return 0;
    }

    if (bestRank > 0) {
        StoreAutomationNameIfFits(
            ranks[static_cast<size_t>(bestRank - 1)],
            GetButtonAutomationName(button));
    }
    return bestRank;
}

void AssociateActiveButtonWithKey(const std::wstring& key) {
    if (key.empty()) {
        return;
    }

    std::wstring displayName;
    std::wstring windowTitle;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        auto& map = CurrentDeskLocked().appFocusMap;
        auto it = map.find(key);
        if (it != map.end()) {
            displayName = it->second.displayName;
            windowTitle = it->second.lastWindowTitle;
        }
    }
    if (displayName.empty()) {
        displayName = FileNameFromPath(key);
    }

    std::vector<winrt::weak_ref<FrameworkElement>> buttons;
    {
        std::lock_guard<std::mutex> lock(g_buttonsMutex);
        buttons = g_trackedButtons;
    }

    // Prefer active button; fall back to best-scoring running button for this
    // app (Windhawk / TC sometimes fail IsVisualStateActive timing).
    int bestScore = 0;
    std::wstring bestName;

    for (auto& weak : buttons) {
        FrameworkElement button = nullptr;
        try {
            button = weak.get();
        } catch (...) {
            continue;
        }
        if (!button || !TaskListButton_IsRunning(button)) {
            continue;
        }

        std::wstring autoName = GetButtonAutomationName(button);
        if (autoName.empty()) {
            continue;
        }

        int score = ScoreExeToAutomationName(displayName, autoName);
        // Identity from path cache (Terminal, VSCodium) beats a window
        // title that happens to mention another app ("Discord icon…").
        const ButtonIdentity ident = GetCachedButtonIdentity(button);
        const std::wstring rankPath = PathFromAppKey(key);
        if (IsAppIdKey(key)) {
            const std::wstring want = AppIdFromAppKey(key);
            std::wstring got = ident.appIdUpper.empty() ? ident.autoIdUpper
                                                        : ident.appIdUpper;
            if (!want.empty() && CanonicalAppId(got) == want) {
                score = (std::max)(score, 1000);
            } else if (!want.empty()) {
                continue;
            }
        } else if (!ident.pathUpper.empty() && !rankPath.empty() &&
                   ident.pathUpper == rankPath) {
            score = (std::max)(score, 1000);
        } else if (!ident.pathUpper.empty() &&
                   ToUpper(FileNameFromPath(ident.pathUpper)) ==
                       ToUpper(displayName)) {
            score = (std::max)(score, 900);
        }
        const bool active = IsVisualStateActive(button);
        if (active) {
            score += 5;
            // Real Active* only (Inactive* is not Active). Windhawk's button
            // is named "Windhawk" while the process is VSCodium.exe.
            if (score < 70) {
                score = 70;
            }
        }

        if (score > bestScore) {
            bestScore = score;
            bestName = autoName;
        }

        if (active && score < 70) {
            Wh_Log(L"Skip associate: active \"%s\" weak match for %s (score=%d)",
                   autoName.c_str(), displayName.c_str(), score);
        }
    }

    AppFocusInfo assocInfo;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        auto& map = CurrentDeskLocked().appFocusMap;
        auto it = map.find(key);
        if (it != map.end()) {
            assocInfo = it->second;
        }
    }
    if (assocInfo.key.empty()) {
        assocInfo.key = key;
        assocInfo.displayName = displayName;
    }

    if (bestScore >= kScoreMinBind && !bestName.empty()) {
        // Always remember the Active button, even when the label is the host
        // ("Windhawk") and the process is VSCodium.exe. Scoring trusts this
        // cache; other write sites still use StoreAutomationNameIfFits.
        StoreAutomationName(assocInfo, bestName);
        Wh_Log(L"Associated %s -> \"%s\" (score=%d, fits=%d, title=\"%s\")",
               displayName.c_str(), bestName.c_str(), bestScore,
               AutomationNameFitsRank(assocInfo, bestName) ? 1 : 0,
               windowTitle.c_str());
    } else {
        Wh_Log(L"Associate failed for %s (bestScore=%d, title=\"%s\")",
               displayName.c_str(), bestScore, windowTitle.c_str());
    }
}

// ---------------------------------------------------------------------------
// Apply / clear visual highlight on one button (UI thread)
// ---------------------------------------------------------------------------

FrameworkElement GetIconPanel(FrameworkElement button) {
    auto iconPanel = FindChildByName(button, L"IconPanel");
    if (!iconPanel) {
        iconPanel = FindDescendantByName(button, L"IconPanel");
    }
    return iconPanel;
}

void RemoveNamedChild(Controls::Panel panel, PCWSTR name) {
    if (!panel) {
        return;
    }
    if (auto child = FindChildByName(panel, name)) {
        uint32_t idx = 0;
        if (panel.Children().IndexOf(child, idx)) {
            panel.Children().RemoveAt(idx);
        }
    }
}

void SpanHostOverPanel(FrameworkElement host, Controls::Panel panel) {
    if (!host || !panel) {
        return;
    }
    try {
        if (auto grid = panel.try_as<Controls::Grid>()) {
            auto cols = grid.ColumnDefinitions().Size();
            auto rows = grid.RowDefinitions().Size();
            Controls::Grid::SetColumn(host, 0);
            Controls::Grid::SetRow(host, 0);
            if (cols > 0) {
                Controls::Grid::SetColumnSpan(host, cols);
            }
            if (rows > 0) {
                Controls::Grid::SetRowSpan(host, rows);
            }
        }
    } catch (...) {
    }
}

// Only clear clip on our own host — walking ancestors with Clip(nullptr) can
// leave native BackgroundElement / RunningIndicator stuck after long idle.
void ClearOurHostClip(FrameworkElement host) {
    if (!host) {
        return;
    }
    try {
        host.Clip(nullptr);
    } catch (...) {
    }
}

// One glow layer: stroked rounded rect; optional fill for Full style.
void StyleGlowRectangle(Shapes::Rectangle rect,
                        const winrt::Windows::UI::Color& stroke,
                        const winrt::Windows::UI::Color& fill,
                        double strokeThickness,
                        double corner,
                        double inset,
                        double opacity) {
    rect.Stroke(Media::SolidColorBrush{stroke});
    rect.StrokeThickness(strokeThickness);
    rect.Fill(Media::SolidColorBrush{fill});
    rect.RadiusX(corner);
    rect.RadiusY(corner);
    rect.Opacity(opacity);
    rect.HorizontalAlignment(HorizontalAlignment::Stretch);
    rect.VerticalAlignment(VerticalAlignment::Stretch);
    rect.Margin(Thickness{inset, inset, inset, inset});
    rect.ClearValue(FrameworkElement::WidthProperty());
    rect.ClearValue(FrameworkElement::HeightProperty());
    rect.IsHitTestVisible(false);
    rect.Visibility(Visibility::Visible);
}

FrameworkElement FindRunningIndicator(FrameworkElement iconPanel) {
    if (!iconPanel) {
        return nullptr;
    }
    auto indicator = FindChildByName(iconPanel, L"RunningIndicator");
    if (!indicator) {
        indicator = FindDescendantByName(iconPanel, L"RunningIndicator");
    }
    return indicator;
}

// Native pill is a 2–4px strip. Taskbar Styler themes restyle RunningIndicator
// into a full-cell acrylic hover plate that would cover a side bar sitting
// under it (UWPSpy: 39×38 acrylic on PointerOver vs #00FFFFFF at rest).
bool RunningIndicatorLooksLikeHoverPlate(FrameworkElement ri,
                                         FrameworkElement iconPanel) {
    if (!ri || !iconPanel) {
        return false;
    }
    try {
        double rw = ri.ActualWidth();
        double rh = ri.ActualHeight();
        double pw = iconPanel.ActualWidth();
        double ph = iconPanel.ActualHeight();
        if (!(rw > 1.0) || !(rh > 1.0) || !(pw > 1.0) || !(ph > 1.0)) {
            return false;
        }
        return rw > pw * 0.45 && rh > ph * 0.45;
    } catch (...) {
        return false;
    }
}

std::wstring CurrentRunningIndicatorStateName(FrameworkElement iconPanel,
                                              FrameworkElement button) {
    auto from = [](FrameworkElement root) -> std::wstring {
        if (!root) {
            return {};
        }
        try {
            for (auto group : VisualStateManager::GetVisualStateGroups(root)) {
                if (group.Name() != L"RunningIndicatorStates") {
                    continue;
                }
                auto current = group.CurrentState();
                if (current) {
                    return std::wstring(current.Name().c_str());
                }
            }
        } catch (...) {
        }
        return {};
    };
    std::wstring name = from(iconPanel);
    if (name.empty()) {
        name = from(button);
    }
    return name;
}

// Only undo Visibility=Collapsed that *we* set. Visual-state setters store
// Visible as a local value — ClearValue drops it to the template default
// (Collapsed), and GoToState(InactiveRunningIndicator) is a no-op if already
// in that state, so the short inactive pill never comes back.
void RestoreNativeRunningIndicator(FrameworkElement iconPanel,
                                   FrameworkElement button) {
    auto indicator = FindRunningIndicator(iconPanel);
    if (!indicator) {
        return;
    }
    try {
        if (indicator.ReadLocalValue(UIElement::VisibilityProperty()) ==
            DependencyProperty::UnsetValue()) {
            return;
        }
        if (indicator.Visibility() != Visibility::Collapsed) {
            return;
        }
        const std::wstring state =
            CurrentRunningIndicatorStateName(iconPanel, button);
        if (state == L"NoRunningIndicator") {
            return;
        }
        indicator.Visibility(Visibility::Visible);
    } catch (...) {
    }
}

bool RunningIndicatorHasLocalCollapsed(FrameworkElement iconPanel) {
    auto indicator = FindRunningIndicator(iconPanel);
    if (!indicator) {
        return false;
    }
    try {
        if (indicator.ReadLocalValue(UIElement::VisibilityProperty()) ==
            DependencyProperty::UnsetValue()) {
            return false;
        }
        return indicator.Visibility() == Visibility::Collapsed;
    } catch (...) {
        return false;
    }
}

void EnsureOverlayIconAboveGlyph(FrameworkElement iconPanel);

// Place glow host in the IconPanel child list without thrashing native chrome.
//
// Moving RunningIndicator every paint (mouse-over UpdateVisualStates) causes
// the short/long underline to flicker. Instead, put our host *under* native
// RunningIndicator / MultiWindowElement / ProgressIndicator so they always
// paint on top — only reorder when the host is in the wrong place.
void EnsureGlowHostZOrder(Controls::Panel panel,
                          UIElement host,
                          GlowStyle style) {
    if (!panel || !host) {
        return;
    }
    try {
        auto children = panel.Children();
        uint32_t hostIdx = 0;
        if (!children.IndexOf(host, hostIdx)) {
            return;
        }

        if (style == GlowStyle::Full) {
            // Plate behind icon and indicators.
            if (hostIdx != 0) {
                children.RemoveAt(hostIdx);
                children.InsertAt(0, host);
            }
            return;
        }

        if (style == GlowStyle::BottomBar) {
            // Cover the native pill without Visibility=Collapsed: host after
            // RunningIndicator. OverlayIcon stays last (badges).
            uint32_t riIdx = UINT32_MAX;
            if (auto ri = FindChildByName(panel.as<FrameworkElement>(),
                                          L"RunningIndicator")) {
                uint32_t i = 0;
                if (children.IndexOf(ri, i)) {
                    riIdx = i;
                }
            }
            if (!children.IndexOf(host, hostIdx)) {
                return;
            }
            if (riIdx != UINT32_MAX) {
                if (hostIdx != riIdx + 1) {
                    children.RemoveAt(hostIdx);
                    if (hostIdx < riIdx) {
                        children.InsertAt(riIdx, host);
                    } else {
                        children.InsertAt(riIdx + 1, host);
                    }
                }
            } else if (hostIdx + 1 != children.Size()) {
                children.RemoveAt(hostIdx);
                children.Append(host);
            }
            EnsureOverlayIconAboveGlyph(panel.as<FrameworkElement>());
            return;
        }

        // Frame / side bar: stay under OverlayIcon, progress, a native thin
        // running pill, and (for side bar) the glyph. Do *not* stay under a
        // Styler hover plate (large RunningIndicator) — that covers the bar.
        auto panelFe = panel.as<FrameworkElement>();
        uint32_t insertBefore = children.Size();
        bool foundTop = false;
        auto considerUnder = [&](PCWSTR name) {
            auto el = FindChildByName(panelFe, name);
            if (!el) {
                return;
            }
            uint32_t idx = 0;
            if (!children.IndexOf(el, idx)) {
                return;
            }
            if (!foundTop || idx < insertBefore) {
                insertBefore = idx;
                foundTop = true;
            }
        };
        considerUnder(L"OverlayIcon");
        considerUnder(L"MultiWindowElement");
        considerUnder(L"ProgressIndicator");
        if (style == GlowStyle::LeftBar) {
            considerUnder(L"Icon");
            considerUnder(L"DefaultIcon");
        }
        if (auto ri = FindRunningIndicator(panelFe)) {
            if (!RunningIndicatorLooksLikeHoverPlate(ri, panelFe)) {
                considerUnder(L"RunningIndicator");
            }
        }

        if (!children.IndexOf(host, hostIdx)) {
            return;
        }

        if (foundTop) {
            if (hostIdx < insertBefore) {
                if (hostIdx + 1 == insertBefore) {
                    // Already immediately under the first element that must
                    // stay on top.
                    EnsureOverlayIconAboveGlyph(panelFe);
                    return;
                }
                // Gap (Styler plate between host and icon) — raise the bar.
                children.RemoveAt(hostIdx);
                children.InsertAt(insertBefore - 1, host);
            } else {
                children.RemoveAt(hostIdx);
                children.InsertAt(insertBefore, host);
            }
        } else if (hostIdx + 1 != children.Size()) {
            children.RemoveAt(hostIdx);
            children.Append(host);
        }

        // Moving the host can leave OverlayIcon behind Icon when Windows later
        // re-appends the glyph (Discord/Thunderbird ping). Heal after we settle.
        EnsureOverlayIconAboveGlyph(panel.as<FrameworkElement>());
    } catch (...) {
    }
}

// TaskListLabeledButtonPanel paints later children on top. Native template
// order is BackgroundElement (back) → Icon → OverlayIcon → RunningIndicator
// (front). Inserting/removing our host can leave BackgroundElement in front
// of the running underscore, or leave OverlayIcon *behind* Icon (Discord /
// Thunderbird / WhatsApp badge clipped by the glyph after a ping + UVS).
void EnsureOverlayIconAboveGlyph(FrameworkElement iconPanel) {
    if (!iconPanel) {
        return;
    }
    auto panel = iconPanel.try_as<Controls::Panel>();
    if (!panel) {
        return;
    }
    try {
        auto overlay = FindChildByName(iconPanel, L"OverlayIcon");
        if (!overlay) {
            return;
        }
        auto children = panel.Children();
        uint32_t oIdx = 0;
        if (!children.IndexOf(overlay, oIdx)) {
            return;
        }

        uint32_t glyphLast = 0;
        bool haveGlyph = false;
        for (PCWSTR name : {L"Icon", L"DefaultIcon"}) {
            auto el = FindChildByName(iconPanel, name);
            if (!el) {
                continue;
            }
            uint32_t idx = 0;
            if (!children.IndexOf(el, idx)) {
                continue;
            }
            if (!haveGlyph || idx > glyphLast) {
                glyphLast = idx;
                haveGlyph = true;
            }
        }
        if (!haveGlyph || oIdx > glyphLast) {
            return;
        }

        children.RemoveAt(oIdx);
        uint32_t dest = glyphLast;
        if (dest > children.Size()) {
            dest = children.Size();
        }
        children.InsertAt(dest, overlay);
        Wh_Log(L"Raised OverlayIcon above Icon (notification badge was behind "
               L"the glyph)");
    } catch (...) {
    }
}

void RestoreIconPanelNativeZOrder(FrameworkElement iconPanel) {
    if (!iconPanel) {
        return;
    }
    auto panel = iconPanel.try_as<Controls::Panel>();
    if (!panel) {
        return;
    }
    try {
        auto children = panel.Children();
        auto indexOfName = [&](PCWSTR name) -> int {
            auto el = FindChildByName(iconPanel, name);
            if (!el) {
                return -1;
            }
            uint32_t idx = 0;
            if (!children.IndexOf(el, idx)) {
                return -1;
            }
            return static_cast<int>(idx);
        };

        const int bg = indexOfName(kBackgroundElementName);
        const int run = indexOfName(L"RunningIndicator");
        if (bg >= 0 && run >= 0 && bg > run) {
            auto bgEl = FindChildByName(iconPanel, kBackgroundElementName);
            if (bgEl) {
                uint32_t bgIdx = 0;
                if (children.IndexOf(bgEl, bgIdx)) {
                    children.RemoveAt(bgIdx);
                    children.InsertAt(0, bgEl);
                    Wh_Log(
                        L"Restored IconPanel z-order (BackgroundElement was "
                        L"in front of RunningIndicator)");
                }
            }
        }

        EnsureOverlayIconAboveGlyph(iconPanel);
    } catch (...) {
    }
}

bool ButtonHasOurChrome(FrameworkElement button) {
    if (!button) {
        return false;
    }
    auto iconPanel = GetIconPanel(button);
    if (!iconPanel) {
        return FindDescendantByName(button, kGlowElementName) != nullptr ||
               FindDescendantByName(button, kBottomBarMarkerName) != nullptr;
    }
    return FindChildByName(iconPanel, kGlowElementName) != nullptr ||
           FindDescendantByName(iconPanel, kGlowElementName) != nullptr ||
           FindChildByName(iconPanel, kBottomBarMarkerName) != nullptr ||
           FindDescendantByName(iconPanel, kBottomBarMarkerName) != nullptr;
}

void ClearButtonHighlight(FrameworkElement button) {
    if (!button) {
        return;
    }

    auto iconPanelEarly = GetIconPanel(button);

    // Skip no-op clears on every mouse-over (UpdateVisualStates storms).
    // Still heal z-order: after a timed-out glow, our host is gone but
    // BackgroundElement can remain in front of RunningIndicator (Discord
    // ping + decay left a red plate and no underscore).
    if (!ButtonHasOurChrome(button) && !g_pendingOverlaySweep.load()) {
        RestoreIconPanelNativeZOrder(iconPanelEarly);
        return;
    }

    try {
        auto iconPanel = iconPanelEarly ? iconPanelEarly : GetIconPanel(button);
        if (!iconPanel) {
            // Still try to strip our named overlay from the button root.
            if (auto panel = button.try_as<Controls::Panel>()) {
                RemoveNamedChild(panel, kGlowElementName);
                RemoveNamedChild(panel, kBottomBarMarkerName);
            }
            return;
        }

        // Do NOT ClearValue BackgroundElement — we no longer style it; clearing
        // local values can leave a stuck pale hover plate after decay.

        if (auto panel = iconPanel.try_as<Controls::Panel>()) {
            // Remove by name even if multiple generations of hosts exist.
            for (int guard = 0; guard < 4; ++guard) {
                if (!FindChildByName(panel, kGlowElementName) &&
                    !FindDescendantByName(iconPanel, kGlowElementName)) {
                    break;
                }
                RemoveNamedChild(panel, kGlowElementName);
                // Descendant host (unexpected nesting): walk children once.
                if (auto orphan =
                        FindDescendantByName(iconPanel, kGlowElementName)) {
                    try {
                        if (auto parent =
                                Media::VisualTreeHelper::GetParent(orphan)
                                    .try_as<Controls::Panel>()) {
                            uint32_t idx = 0;
                            if (parent.Children().IndexOf(orphan, idx)) {
                                parent.Children().RemoveAt(idx);
                            }
                        }
                    } catch (...) {
                    }
                }
            }
            RemoveNamedChild(panel, kBottomBarMarkerName);
        }

        if (RunningIndicatorHasLocalCollapsed(iconPanel)) {
            RestoreNativeRunningIndicator(iconPanel, button);
        }
        // Do not heal Visibility on every clear — that ClearValue thrashing
        // also flickers the native underline during hover storms.

        if (auto icon = FindChildByName(iconPanel, L"Icon")) {
            auto localTf =
                icon.ReadLocalValue(UIElement::RenderTransformProperty());
            if (localTf != DependencyProperty::UnsetValue()) {
                if (icon.RenderTransform().try_as<Media::ScaleTransform>()) {
                    icon.ClearValue(UIElement::RenderTransformProperty());
                    icon.ClearValue(UIElement::RenderTransformOriginProperty());
                }
            }
        }

        RestoreIconPanelNativeZOrder(iconPanel);
    } catch (...) {
        HRESULT hr = winrt::to_hresult();
        Wh_Log(L"ClearButtonHighlight error %08X", hr);
    }
}

// Ensure glow host grid exists (L0–L2 rectangles). Returns host or nullptr.
Controls::Grid EnsureGlowHost(Controls::Panel panel,
                              FrameworkElement iconPanel) {
    Controls::Grid host = nullptr;
    if (auto existing = FindChildByName(iconPanel, kGlowElementName)) {
        host = existing.try_as<Controls::Grid>();
        if (host && !FindChildByName(host, kGlowLayerNames[0])
                         .try_as<Shapes::Rectangle>()) {
            RemoveNamedChild(panel, kGlowElementName);
            host = nullptr;
        } else if (!host) {
            RemoveNamedChild(panel, kGlowElementName);
        }
    }

    if (!host) {
        PCWSTR xaml =
            LR"(
            <Grid
                xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
                xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
                Name="WhRecentFocusGlow"
                IsHitTestVisible="False"
                HorizontalAlignment="Stretch"
                VerticalAlignment="Stretch">
                <Rectangle Name="WhRecentFocusGlowL0"
                           IsHitTestVisible="False"
                           Fill="Transparent"/>
                <Rectangle Name="WhRecentFocusGlowL1"
                           IsHitTestVisible="False"
                           Fill="Transparent"/>
                <Rectangle Name="WhRecentFocusGlowL2"
                           IsHitTestVisible="False"
                           Fill="Transparent"/>
            </Grid>
        )";
        host = Markup::XamlReader::Load(xaml).as<Controls::Grid>();
        panel.Children().Append(host);
    }

    SpanHostOverPanel(host, panel);
    host.Visibility(Visibility::Visible);
    ClearOurHostClip(host);
    return host;
}

void HideAllGlowLayers(Controls::Grid host) {
    if (!host) {
        return;
    }
    for (int i = 0; i < kGlowMaxLayers; ++i) {
        if (auto r =
                FindChildByName(host, kGlowLayerNames[i]).try_as<Shapes::Rectangle>()) {
            r.Visibility(Visibility::Collapsed);
            r.Stroke(nullptr);
            r.Fill(Media::SolidColorBrush{
                winrt::Windows::UI::Color{0, 0, 0, 0}});
        }
    }
}

PCWSTR GlowStyleName(GlowStyle s) {
    switch (s) {
        case GlowStyle::Full:
            return L"full";
        case GlowStyle::LeftBar:
            return L"leftBar";
        case GlowStyle::BottomBar:
            return L"bottomBar";
        case GlowStyle::Frame:
        default:
            return L"frame";
    }
}

PCWSTR PromoteModeName(PromoteMode m) {
    switch (m) {
        case PromoteMode::ImmediateTopN:
            return L"immediateTopN";
        case PromoteMode::AlwaysWait:
            return L"alwaysWait";
        case PromoteMode::ImmediateTracked:
        default:
            return L"immediateTracked";
    }
}

// True if this focus change may skip the app min-focus timer.
// Caller must hold g_stateMutex when checking ranked list (or we take it).
bool ShouldSkipAppMinFocus(const std::wstring& key, bool alreadyTracked) {
    auto settings = SettingsSnap();
    if (settings->minFocusSeconds <= 0) {
        return true;
    }
    switch (settings->promoteMode) {
        case PromoteMode::AlwaysWait:
            return false;
        case PromoteMode::ImmediateTopN: {
            if (!alreadyTracked) {
                return false;
            }
            std::lock_guard<std::mutex> lock(g_stateMutex);
            for (const auto& r : CurrentDeskLocked().rankedApps) {
                if (r.key == key) {
                    return true;
                }
            }
            return false;
        }
        case PromoteMode::ImmediateTracked:
        default:
            return alreadyTracked;
    }
}

PCWSTR PreviewStyleName(PreviewStyle s) {
    switch (s) {
        case PreviewStyle::TitleBg:
            return L"titleBg";
        case PreviewStyle::Plate:
            return L"plate";
        case PreviewStyle::TitleBar:
            return L"titleBar";
        case PreviewStyle::PlateTitle:
            return L"plateTitle";
        case PreviewStyle::Ring:
        default:
            return L"ring";
    }
}

PCWSTR TaskbarEdgeName(TaskbarEdge e) {
    switch (e) {
        case TaskbarEdge::Left:
            return L"left";
        case TaskbarEdge::Top:
            return L"top";
        case TaskbarEdge::Right:
            return L"right";
        case TaskbarEdge::Bottom:
        default:
            return L"bottom";
    }
}

PCWSTR BarSideName(BarSide s) {
    switch (s) {
        case BarSide::Left:
            return L"left";
        case BarSide::Top:
            return L"top";
        case BarSide::Right:
            return L"right";
        case BarSide::Bottom:
        default:
            return L"bottom";
    }
}

bool HasVisualState(FrameworkElement root, PCWSTR stateName) {
    if (!root || !stateName) {
        return false;
    }
    try {
        auto groups = VisualStateManager::GetVisualStateGroups(root);
        for (auto group : groups) {
            auto current = group.CurrentState();
            if (current && current.Name() == stateName) {
                return true;
            }
        }
    } catch (...) {
    }
    return false;
}

TaskbarEdge TaskbarEdgeFromAppBar() {
    APPBARDATA abd{};
    abd.cbSize = sizeof(APPBARDATA);
    if (!SHAppBarMessage(ABM_GETTASKBARPOS, &abd)) {
        return TaskbarEdge::Bottom;
    }
    switch (abd.uEdge) {
        case ABE_LEFT:
            return TaskbarEdge::Left;
        case ABE_TOP:
            return TaskbarEdge::Top;
        case ABE_RIGHT:
            return TaskbarEdge::Right;
        default:
            return TaskbarEdge::Bottom;
    }
}

// Prefer OrientationStates. A leftover RunningIndicator VA=Bottom from the
// previous edge must not win: we used to treat a left taskbar as Bottom and
// paint full-cell underlines of rank-dependent length.
// Do not use "wider than tall ⇒ horizontal": a left-edge button is 48×32.
TaskbarEdge DetectTaskbarEdge(FrameworkElement iconPanel) {
    bool verticalState = HasVisualState(iconPanel, L"VerticalOrientation");
    bool horizontalState = HasVisualState(iconPanel, L"HorizontalOrientation");
    try {
        auto parent = Media::VisualTreeHelper::GetParent(iconPanel)
                          .try_as<FrameworkElement>();
        if (parent) {
            verticalState =
                verticalState ||
                HasVisualState(parent, L"VerticalOrientation");
            horizontalState =
                horizontalState ||
                HasVisualState(parent, L"HorizontalOrientation");
        }
    } catch (...) {
    }

    HorizontalAlignment ha = HorizontalAlignment::Stretch;
    VerticalAlignment va = VerticalAlignment::Stretch;
    double riW = 0;
    double riH = 0;
    if (auto ri = FindRunningIndicator(iconPanel)) {
        try {
            ha = ri.HorizontalAlignment();
            va = ri.VerticalAlignment();
            riW = ri.ActualWidth();
            riH = ri.ActualHeight();
        } catch (...) {
        }
    }

    double pw = 0;
    double ph = 0;
    try {
        pw = iconPanel.ActualWidth();
        ph = iconPanel.ActualHeight();
    } catch (...) {
    }
    // Vertical bar: each button is short along the bar (≈32) and as wide as
    // the bar (≈48). Horizontal bar: roughly square / taller (≈44×48).
    const bool panelLooksVertical = pw > ph + 4.0;
    const bool panelLooksHorizontal = ph > pw + 4.0;
    const bool tallPill = riH > riW + 0.5 && riW > 0.5;

    auto pickVertical = [&]() -> TaskbarEdge {
        if (ha == HorizontalAlignment::Right) {
            return TaskbarEdge::Right;
        }
        if (ha == HorizontalAlignment::Left) {
            return TaskbarEdge::Left;
        }
        return TaskbarEdgeFromAppBar() == TaskbarEdge::Right
                   ? TaskbarEdge::Right
                   : TaskbarEdge::Left;
    };
    auto pickHorizontal = [&]() -> TaskbarEdge {
        if (va == VerticalAlignment::Top) {
            return TaskbarEdge::Top;
        }
        if (va == VerticalAlignment::Bottom) {
            return TaskbarEdge::Bottom;
        }
        return TaskbarEdgeFromAppBar() == TaskbarEdge::Top ? TaskbarEdge::Top
                                                           : TaskbarEdge::Bottom;
    };

    if (verticalState || panelLooksVertical || tallPill) {
        return pickVertical();
    }
    if (horizontalState || panelLooksHorizontal) {
        return pickHorizontal();
    }
    if (ha == HorizontalAlignment::Right) {
        return TaskbarEdge::Right;
    }
    if (ha == HorizontalAlignment::Left) {
        return TaskbarEdge::Left;
    }
    if (va == VerticalAlignment::Top) {
        return TaskbarEdge::Top;
    }
    if (va == VerticalAlignment::Bottom) {
        return TaskbarEdge::Bottom;
    }
    return TaskbarEdgeFromAppBar();
}

BarSide BarSideForGlowStyle(GlowStyle style, TaskbarEdge edge) {
    if (style == GlowStyle::BottomBar) {
        switch (edge) {
            case TaskbarEdge::Left:
                return BarSide::Left;
            case TaskbarEdge::Top:
                return BarSide::Top;
            case TaskbarEdge::Right:
                return BarSide::Right;
            case TaskbarEdge::Bottom:
            default:
                return BarSide::Bottom;
        }
    }
    // Side bar: perpendicular to the taskbar so it does not cover the native
    // running pill (left on bottom/top, under the icon on left/right).
    switch (edge) {
        case TaskbarEdge::Left:
        case TaskbarEdge::Right:
            return BarSide::Bottom;
        case TaskbarEdge::Top:
        case TaskbarEdge::Bottom:
        default:
            return BarSide::Left;
    }
}

void GlowContentBoxSize(FrameworkElement host,
                        FrameworkElement iconPanel,
                        double panelW,
                        double panelH,
                        double& boxW,
                        double& boxH) {
    boxW = 0;
    boxH = 0;
    if (host) {
        try {
            boxW = host.ActualWidth();
            boxH = host.ActualHeight();
        } catch (...) {
        }
    }
    if (!(boxW > 1.0) || !(boxH > 1.0)) {
        if (auto bg = FindChildByName(iconPanel, kBackgroundElementName)) {
            try {
                boxW = bg.ActualWidth();
                boxH = bg.ActualHeight();
            } catch (...) {
            }
        }
    }
    if (!(boxW > 1.0)) {
        boxW = panelW;
    }
    if (!(boxH > 1.0)) {
        boxH = panelH;
    }
}

// Length follows the glow host (padded cell), same for every rank — rank is
// opacity. Icon-sized underlines on a left taskbar were too short to scan
// (Windows uses a very small glyph there). Size % still scales the bar.
double BarLengthForSide(FrameworkElement iconPanel,
                        double boxW,
                        double boxH,
                        BarSide side,
                        TaskbarEdge edge,
                        double sizeFrac,
                        double rankLenScale) {
    (void)iconPanel;
    (void)edge;
    const bool horizontalBar =
        side == BarSide::Top || side == BarSide::Bottom;
    const double cellAlong = horizontalBar ? boxW : boxH;
    double barLen = cellAlong * sizeFrac * rankLenScale;
    const double maxLen = (std::max)(4.0, cellAlong - 2.0);
    if (barLen > maxLen) {
        barLen = maxLen;
    }
    if (barLen < 4.0) {
        barLen = 4.0;
    }
    return barLen;
}

void StyleGlowBarOnSide(Shapes::Rectangle rect,
                        const winrt::Windows::UI::Color& fill,
                        double thickness,
                        double length,
                        BarSide side,
                        int layer,
                        double opacity) {
    const double t = thickness + static_cast<double>(layer) * 2.0;
    const double len = length + static_cast<double>(layer) * 2.0;
    const double edgePad = (side == BarSide::Bottom || side == BarSide::Top)
                               ? (2.0 + static_cast<double>(layer))
                               : (1.0 + static_cast<double>(layer));

    rect.Stroke(nullptr);
    rect.StrokeThickness(0);
    rect.Fill(Media::SolidColorBrush{fill});
    rect.RadiusX(t * 0.5);
    rect.RadiusY(t * 0.5);
    rect.Opacity(opacity);
    rect.IsHitTestVisible(false);
    rect.Visibility(Visibility::Visible);

    switch (side) {
        case BarSide::Left:
            rect.Width(t);
            rect.Height(len);
            rect.HorizontalAlignment(HorizontalAlignment::Left);
            rect.VerticalAlignment(VerticalAlignment::Center);
            rect.Margin(Thickness{edgePad, 0, 0, 0});
            break;
        case BarSide::Right:
            rect.Width(t);
            rect.Height(len);
            rect.HorizontalAlignment(HorizontalAlignment::Right);
            rect.VerticalAlignment(VerticalAlignment::Center);
            rect.Margin(Thickness{0, 0, edgePad, 0});
            break;
        case BarSide::Top:
            rect.Width(len);
            rect.Height(t);
            rect.HorizontalAlignment(HorizontalAlignment::Center);
            rect.VerticalAlignment(VerticalAlignment::Top);
            rect.Margin(Thickness{0, edgePad, 0, 0});
            break;
        case BarSide::Bottom:
        default:
            rect.Width(len);
            rect.Height(t);
            rect.HorizontalAlignment(HorizontalAlignment::Center);
            rect.VerticalAlignment(VerticalAlignment::Bottom);
            rect.Margin(Thickness{0, 0, 0, edgePad});
            break;
    }
}

FrameworkElement TaskListButtonFromDescendant(FrameworkElement start) {
    FrameworkElement cur = start;
    for (int i = 0; i < 12 && cur; ++i) {
        try {
            if (cur.Name() == L"TaskListButton") {
                return cur;
            }
            cur = Media::VisualTreeHelper::GetParent(cur)
                      .try_as<FrameworkElement>();
        } catch (...) {
            return nullptr;
        }
    }
    return nullptr;
}

// Relayout (taskbar moved to another screen edge): put RunningIndicator back
// in front of BackgroundElement / our host. Do not ClearValue Width/Height —
// OrientationStates owns those. Only call on size/edge change, not every UVS.
void HealRunningIndicatorAfterRelayout(FrameworkElement iconPanel) {
    if (!iconPanel) {
        return;
    }
    RestoreIconPanelNativeZOrder(iconPanel);

    auto panel = iconPanel.try_as<Controls::Panel>();
    if (!panel) {
        return;
    }
    try {
        if (auto host = FindChildByName(iconPanel, kGlowElementName)) {
            EnsureGlowHostZOrder(panel, host, SettingsSnap()->glowStyle);
        }
    } catch (...) {
    }
}

// Must run on the panel's dispatcher. Pulls only watches for this thread so
// a primary-taskbar cleanup cannot revoke a secondary bar's tokens.
void RevokeIconPanelLayoutWatchesOnThisDispatcher() {
    std::vector<IconPanelLayoutWatch> mine;
    {
        std::lock_guard<std::mutex> lock(g_layoutWatchMutex);
        for (auto it = g_layoutWatches.begin(); it != g_layoutWatches.end();) {
            bool take = false;
            try {
                auto panel = it->panel.get();
                if (!panel) {
                    take = true;
                } else {
                    auto dispatcher = panel.Dispatcher();
                    take = dispatcher && dispatcher.HasThreadAccess();
                }
            } catch (...) {
                take = true;
            }
            if (take) {
                mine.push_back(std::move(*it));
                it = g_layoutWatches.erase(it);
            } else {
                ++it;
            }
        }
    }
    for (auto& w : mine) {
        try {
            if (auto panel = w.panel.get()) {
                if (w.sizeChanged) {
                    panel.SizeChanged(w.sizeChanged);
                    w.sizeChanged = {};
                }
            }
        } catch (...) {
            Wh_Log(L"ERROR: SizeChanged revoke failed");
        }
    }
}

void EnsureIconPanelLayoutWatch(FrameworkElement button) {
    if (!button || g_unloading.load()) {
        return;
    }
    auto iconPanel = GetIconPanel(button);
    if (!iconPanel) {
        return;
    }

    const TaskbarEdge edge = DetectTaskbarEdge(iconPanel);
    bool alreadyWatched = false;
    bool edgeChanged = false;
    TaskbarEdge previousEdge = TaskbarEdge::Bottom;
    {
        std::lock_guard<std::mutex> lock(g_layoutWatchMutex);
        for (auto& w : g_layoutWatches) {
            try {
                if (w.panel.get() != iconPanel) {
                    continue;
                }
                alreadyWatched = true;
                if (w.haveEdge && w.lastEdge != edge) {
                    edgeChanged = true;
                    previousEdge = w.lastEdge;
                }
                w.lastEdge = edge;
                w.haveEdge = true;
                break;
            } catch (...) {
            }
        }
    }
    if (alreadyWatched) {
        if (edgeChanged) {
            Wh_Log(L"Taskbar edge %s -> %s (%.0fx%.0f)",
                   TaskbarEdgeName(previousEdge), TaskbarEdgeName(edge),
                   iconPanel.ActualWidth(), iconPanel.ActualHeight());
            HealRunningIndicatorAfterRelayout(iconPanel);
        }
        return;
    }

    IconPanelLayoutWatch watch;
    watch.panel = winrt::make_weak(iconPanel);
    watch.lastEdge = edge;
    watch.haveEdge = true;
    winrt::weak_ref<FrameworkElement> weakPanel = watch.panel;
    try {
        watch.sizeChanged = iconPanel.SizeChanged(
            [weakPanel](winrt::Windows::Foundation::IInspectable const&,
                        SizeChangedEventArgs const& args) {
                if (g_unloading.load()) {
                    return;
                }
                if (g_iconPanelRelayoutDepth > 0) {
                    return;
                }
                FrameworkElement panel = nullptr;
                try {
                    panel = weakPanel.get();
                } catch (...) {
                    return;
                }
                if (!panel) {
                    return;
                }
                const double nw = args.NewSize().Width;
                const double nh = args.NewSize().Height;
                const double ow = args.PreviousSize().Width;
                const double oh = args.PreviousSize().Height;
                if (!(nw > 1.0 && nh > 1.0)) {
                    return;
                }
                const bool relayout =
                    ow > 1.0 && oh > 1.0 &&
                    ((nw > ow + 1.5 || ow > nw + 1.5) ||
                     (nh > oh + 1.5 || oh > nh + 1.5));
                const bool firstMeasure = !(ow > 1.0 && oh > 1.0);
                if (!relayout && !firstMeasure) {
                    return;
                }
                if (relayout) {
                    Wh_Log(L"IconPanel relayout: %.0fx%.0f -> %.0fx%.0f", ow,
                           oh, nw, nh);
                }
                ++g_iconPanelRelayoutDepth;
                try {
                    HealRunningIndicatorAfterRelayout(panel);
                    if (auto btn = TaskListButtonFromDescendant(panel)) {
                        RefreshButtonHighlight(btn);
                        ScheduleRefreshAllHighlights(btn);
                    }
                } catch (...) {
                }
                --g_iconPanelRelayoutDepth;
            });
    } catch (...) {
        return;
    }

    std::lock_guard<std::mutex> lock(g_layoutWatchMutex);
    g_layoutWatches.erase(
        std::remove_if(g_layoutWatches.begin(), g_layoutWatches.end(),
                       [](IconPanelLayoutWatch& w) {
                           try {
                               return !w.panel.get();
                           } catch (...) {
                               return true;
                           }
                       }),
        g_layoutWatches.end());
    g_layoutWatches.push_back(std::move(watch));
}

void ApplyButtonHighlight(FrameworkElement button, int rankOneBased) {
    if (!button) {
        return;
    }

    if (rankOneBased <= 0 || g_unloading.load() || !SettingsSnap()->enabled) {
        ClearButtonHighlight(button);
        return;
    }

    try {
        auto iconPanel = GetIconPanel(button);
        if (!iconPanel) {
            return;
        }

        auto panel = iconPanel.try_as<Controls::Panel>();
        if (!panel) {
            return;
        }

        const int rankIdx = rankOneBased - 1;
        const int intensity = RankIntensity(rankIdx);
        const int sizeBoost = RankSizeBoost(rankIdx);
        const double t = intensity / 100.0;

        winrt::Windows::UI::Color base = ResolveGlowBaseColor();

        auto withAlpha = [](winrt::Windows::UI::Color c,
                            int a) -> winrt::Windows::UI::Color {
            c.A = static_cast<uint8_t>((std::max)(0, (std::min)(255, a)));
            return c;
        };

        const int layers =
            (std::max)(1, (std::min)(kGlowMaxLayers, SettingsSnap()->glowLayers));
        const double thickness = static_cast<double>(
            (std::max)(1, (std::min)(16, SettingsSnap()->glowThickness)));
        const double roundnessFrac =
            (std::max)(0, (std::min)(50, SettingsSnap()->glowRoundness)) / 100.0;
        const double sizeFrac =
            (std::max)(40, (std::min)(100, SettingsSnap()->glowSize)) / 100.0;
        const GlowStyle style = SettingsSnap()->glowStyle;
        const int fillOpacitySetting =
            (std::max)(0, (std::min)(100, SettingsSnap()->glowFillOpacity));

        double panelW = iconPanel.ActualWidth();
        double panelH = iconPanel.ActualHeight();
        if (!(panelW > 1.0)) {
            panelW = 44.0;
        }
        if (!(panelH > 1.0)) {
            panelH = 44.0;
        }

        Controls::Grid host = EnsureGlowHost(panel, iconPanel);
        if (!host) {
            return;
        }

        double boxW = panelW;
        double boxH = panelH;
        GlowContentBoxSize(host, iconPanel, panelW, panelH, boxW, boxH);

        const TaskbarEdge edge = DetectTaskbarEdge(iconPanel);
        const BarSide barSide = BarSideForGlowStyle(style, edge);

        // Heal native stacking first (Discord overlay / leftover attention
        // plate), then place our host (under a native pill; above a Styler
        // hover plate so the side bar is not covered on PointerOver).
        RestoreIconPanelNativeZOrder(iconPanel);

        // Z-order once when wrong — never yank RunningIndicator every paint
        // (that caused short/long underline flicker on mouse-over).
        EnsureGlowHostZOrder(panel, host, style);

        HideAllGlowLayers(host);

        // Strip leftover Edge-bar marker from older builds. Do not touch
        // RunningIndicator unless we actually left Visibility=Collapsed —
        // ClearValue(Visible) is what killed the short inactive pills.
        if (style != GlowStyle::BottomBar) {
            if (FindChildByName(iconPanel, kBottomBarMarkerName)) {
                if (auto p = iconPanel.try_as<Controls::Panel>()) {
                    RemoveNamedChild(p, kBottomBarMarkerName);
                }
            }
        }
        if (RunningIndicatorHasLocalCollapsed(iconPanel)) {
            RestoreNativeRunningIndicator(iconPanel, button);
        }

        if (style == GlowStyle::Frame || style == GlowStyle::Full) {
            const double baseInset =
                (std::min)(boxW, boxH) * (1.0 - sizeFrac) * 0.5;
            const bool isFrame = style == GlowStyle::Frame;

            for (int i = 0; i < kGlowMaxLayers; ++i) {
                auto rect = FindChildByName(host, kGlowLayerNames[i])
                                .try_as<Shapes::Rectangle>();
                if (!rect) {
                    continue;
                }
                if (i >= layers) {
                    continue;
                }

                const double step =
                    (i == 0) ? 0.0 : (3.0 + thickness * 0.55) * i;
                const double inset = baseInset + step;
                const double inner =
                    (std::max)(8.0, (std::min)(boxW, boxH) - 2.0 * inset);
                const double corner = inner * roundnessFrac;
                const double layerT = t * (1.0 - 0.15 * i);
                const double th =
                    (std::max)(1.0, thickness * (1.0 - 0.1 * i));
                const int strokeA =
                    static_cast<int>((100 + 130 * layerT) * (1.0 - 0.12 * i));
                const double opacity = 0.70 + 0.30 * layerT;

                winrt::Windows::UI::Color fill{0, 0, 0, 0};
                if (!isFrame && i == 0) {
                    int fillA = static_cast<int>(fillOpacitySetting * 2.55 *
                                                 (0.45 + 0.55 * layerT));
                    fill = withAlpha(base, fillA);
                }

                StyleGlowRectangle(rect, withAlpha(base, strokeA), fill, th,
                                   corner, inset, opacity);
            }
        } else if (style == GlowStyle::LeftBar) {
            // Side bar: left of the icon on a bottom/top taskbar, under the
            // icon on a left/right taskbar — never the native-pill edge.
            const double barT = thickness + 1.0;
            const double barLen = BarLengthForSide(
                iconPanel, boxW, boxH, barSide, edge, sizeFrac, 1.0);
            const int fillBase =
                static_cast<int>(fillOpacitySetting * 2.55 * (0.55 + 0.45 * t));
            const int nLeft = (std::max)(1, (std::min)(layers, 2));

            for (int i = 0; i < nLeft; ++i) {
                auto rect = FindChildByName(host, kGlowLayerNames[i])
                                .try_as<Shapes::Rectangle>();
                if (!rect) {
                    continue;
                }
                const int fillA =
                    i == 0 ? fillBase : static_cast<int>(fillBase * 0.35);
                const double opacity = i == 0 ? (0.85 + 0.15 * t) : 0.45;
                StyleGlowBarOnSide(rect, withAlpha(base, fillA), barT, barLen,
                                   barSide, i, opacity);
            }
        } else if (style == GlowStyle::BottomBar) {
            // Edge bar on the native RunningIndicator side. Cover the pill
            // by z-order (host after RI). Never set Visibility/Width/Height
            // on the native indicator.
            if (SettingsSnap()->glowDebugLog && !FindRunningIndicator(iconPanel)) {
                Wh_Log(L"EdgeBar: RunningIndicator not found on \"%s\"",
                       GetButtonAutomationName(button).c_str());
            }

            const int fillA =
                static_cast<int>(fillOpacitySetting * 2.55 * (0.6 + 0.4 * t));
            const double barT =
                (std::max)(2.0, (std::min)(6.0, thickness));
            const double barLen = BarLengthForSide(
                iconPanel, boxW, boxH, barSide, edge, sizeFrac, 1.0);

            if (auto rect = FindChildByName(host, kGlowLayerNames[0])
                                .try_as<Shapes::Rectangle>()) {
                StyleGlowBarOnSide(rect, withAlpha(base, fillA), barT, barLen,
                                   barSide, 0, 0.9 + 0.1 * t);
            }

        }

        if (auto icon = FindChildByName(iconPanel, L"Icon")) {
            if (sizeBoost > 0) {
                double s = 1.0 + static_cast<double>(sizeBoost) / 100.0;
                Media::ScaleTransform scale;
                scale.ScaleX(s);
                scale.ScaleY(s);
                icon.RenderTransformOrigin(
                    winrt::Windows::Foundation::Point{0.5f, 0.5f});
                icon.RenderTransform(scale);
            } else {
                if (icon.RenderTransform().try_as<Media::ScaleTransform>()) {
                    icon.ClearValue(UIElement::RenderTransformProperty());
                    icon.ClearValue(UIElement::RenderTransformOriginProperty());
                }
            }
        }

        if (SettingsSnap()->glowDebugLog) {
            Wh_Log(L"Glow rank %d \"%s\" style=%s edge=%s bar=%s box=%.0fx%.0f "
                   L"th=%.0f round=%d%% size=%d%% layers=%d intensity=%d",
                   rankOneBased, GetButtonAutomationName(button).c_str(),
                   GlowStyleName(style), TaskbarEdgeName(edge),
                   BarSideName(barSide), boxW, boxH, thickness,
                   SettingsSnap()->glowRoundness, SettingsSnap()->glowSize, layers,
                   intensity);
        }
    } catch (...) {
        HRESULT hr = winrt::to_hresult();
        Wh_Log(L"ApplyButtonHighlight error %08X", hr);
    }
}

void PruneTrackedButtons_UIThread() {
    std::lock_guard<std::mutex> lock(g_buttonsMutex);
    g_trackedButtons.erase(
        std::remove_if(g_trackedButtons.begin(), g_trackedButtons.end(),
                       [](winrt::weak_ref<FrameworkElement>& weak) {
                           try {
                               return !weak.get();
                           } catch (...) {
                               return true;
                           }
                       }),
        g_trackedButtons.end());
}

void TrackButton_UIThread(FrameworkElement button) {
    if (!button) {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(g_buttonsMutex);
        g_dispatcherAnchor = winrt::make_weak(button);

        bool tracked = false;
        for (auto& weak : g_trackedButtons) {
            try {
                if (weak.get() == button) {
                    tracked = true;
                    break;
                }
            } catch (...) {
            }
        }
        if (!tracked) {
            g_trackedButtons.push_back(winrt::make_weak(button));
        }
    }
    EnsureButtonPathCached(button, /*force=*/false);
    EnsureIconPanelLayoutWatch(button);
}

// ---------------------------------------------------------------------------
// Button → process path (option C) — adapted from taskbar-volume-control-per-app
// ---------------------------------------------------------------------------

thread_local bool g_captureTaskGroup = false;
thread_local void* g_capturedTaskGroup = nullptr;

WCHAR g_clickSentinel[] = L"wh-rfh-click-sentinel";
thread_local void* g_clickSentinel_TaskGroup = nullptr;
thread_local void* g_clickSentinel_TaskItem = nullptr;

constexpr int kMaxVtableProbeSlots = 32;
constexpr size_t kMaxTaskItemsArrayOffset = 64;

void* QueryViaVtable(void* object, void* vtable) {
    if (!object || !vtable) {
        return nullptr;
    }
    void** ptr = static_cast<void**>(object);
    for (int i = 0; i < kMaxVtableProbeSlots; ++i) {
        if (ptr[i] == vtable) {
            return ptr + i;
        }
    }
    return nullptr;
}

using CWindowTaskItem_GetWindow_t = HWND(WINAPI*)(void* pThis);
CWindowTaskItem_GetWindow_t CWindowTaskItem_GetWindow;

using CImmersiveTaskItem_GetAppWindow_t = HWND(WINAPI*)(void* pThis);
CImmersiveTaskItem_GetAppWindow_t CImmersiveTaskItem_GetAppWindow;

void* CImmersiveTaskItem_vftable = nullptr;
void* CImmersiveTaskItem_vftable_ITaskItem = nullptr;
void* CWindowTaskItem_vftable = nullptr;
void* CWindowTaskItem_vftable_ITaskItem = nullptr;

using TryGetItemFromContainer_TaskListWindowViewModel_t =
    void*(WINAPI*)(void** output, UIElement* container);
TryGetItemFromContainer_TaskListWindowViewModel_t
    TryGetItemFromContainer_TaskListWindowViewModel_Original;

using TaskListWindowViewModel_get_TaskItem_t = int(WINAPI*)(void* pThis,
                                                            void** taskItem);
TaskListWindowViewModel_get_TaskItem_t
    TaskListWindowViewModel_get_TaskItem_Original;

using TryGetItemFromContainer_TaskListGroupViewModel_t =
    void*(WINAPI*)(void** output, UIElement* container);
TryGetItemFromContainer_TaskListGroupViewModel_t
    TryGetItemFromContainer_TaskListGroupViewModel_Original;

using TaskListGroupViewModel_IsMultiWindow_t = bool(WINAPI*)(void* pThis);
TaskListGroupViewModel_IsMultiWindow_t
    TaskListGroupViewModel_IsMultiWindow_Original;

using ITaskGroup_IsRunning_t = bool(WINAPI*)(void* pThis);
ITaskGroup_IsRunning_t ITaskGroup_IsRunning_Original;
bool WINAPI ITaskGroup_IsRunning_Hook(void* pThis) {
    if (g_captureTaskGroup) {
        g_capturedTaskGroup = *(void**)pThis;
        return false;
    }
    return ITaskGroup_IsRunning_Original
               ? ITaskGroup_IsRunning_Original(pThis)
               : false;
}

using CTaskGroup_GetNumItems_t = int(WINAPI*)(void* pThis);
CTaskGroup_GetNumItems_t CTaskGroup_GetNumItems;

HDPA GetTaskItemsArray(void* taskGroup) {
    if (!CTaskGroup_GetNumItems || !taskGroup) {
        return nullptr;
    }
    static size_t offset = []() -> size_t {
        constexpr int kIntArraySize = 256;
        int arrayOfInts[kIntArraySize];
        int* arrayOfIntPtrs[kIntArraySize];
        for (int i = 0; i < kIntArraySize; i++) {
            arrayOfInts[i] = i;
            arrayOfIntPtrs[i] = &arrayOfInts[i];
        }
        const int raw = CTaskGroup_GetNumItems(arrayOfIntPtrs);
        if (raw < 0 || raw >= kIntArraySize) {
            return static_cast<size_t>(-1);
        }
        return static_cast<size_t>(raw);
    }();
    if (offset >= kMaxTaskItemsArrayOffset) {
        return nullptr;
    }
    return (HDPA)((void**)taskGroup)[offset];
}

using CTaskListWnd_HandleClick_t = HRESULT(WINAPI*)(void* pThis,
                                                    void* taskGroup,
                                                    void* taskItem,
                                                    void** launcherOptions);
CTaskListWnd_HandleClick_t CTaskListWnd_HandleClick_Original;
HWND GetWindowFromTaskItem(void* taskItem);

HRESULT WINAPI CTaskListWnd_HandleClick_Hook(void* pThis,
                                             void* taskGroup,
                                             void* taskItem,
                                             void** launcherOptions) {
    if (launcherOptions && *launcherOptions == &g_clickSentinel) {
        g_clickSentinel_TaskGroup = taskGroup;
        g_clickSentinel_TaskItem = taskItem;
        return S_OK;
    }
    const HRESULT hr = CTaskListWnd_HandleClick_Original
                           ? CTaskListWnd_HandleClick_Original(
                                 pThis, taskGroup, taskItem, launcherOptions)
                           : E_FAIL;
    // Thumbnail (or grouped-icon) click is an explicit "this window".
    // Confirm it immediately so decay + click does not paint the sibling.
    if (SUCCEEDED(hr) && taskItem) {
        if (HWND clicked = GetWindowFromTaskItem(taskItem)) {
            ConfirmPreviewFocusNow(clicked);
        }
    }
    return hr;
}

using TaskItem_ReportClicked_t = int(WINAPI*)(void* pThis, void* param);
TaskItem_ReportClicked_t TaskItem_ReportClicked_Original;

using TaskGroup_ReportClicked_t = int(WINAPI*)(void* pThis, void* param);
TaskGroup_ReportClicked_t TaskGroup_ReportClicked_Original;

void* GetNativeTaskItemFromWindowsUdkTaskItem(void* windowsUdkTaskItem) {
    if (!TaskItem_ReportClicked_Original || !windowsUdkTaskItem) {
        return nullptr;
    }
    void* prev = g_clickSentinel_TaskItem;
    g_clickSentinel_TaskItem = nullptr;
    TaskItem_ReportClicked_Original(windowsUdkTaskItem, &g_clickSentinel);
    void* result = g_clickSentinel_TaskItem;
    g_clickSentinel_TaskItem = prev;
    return result;
}

void* GetNativeTaskGroupFromWindowsUdkTaskGroup(void* windowsUdkTaskGroup) {
    if (!TaskGroup_ReportClicked_Original || !windowsUdkTaskGroup) {
        return nullptr;
    }
    void* prev = g_clickSentinel_TaskGroup;
    g_clickSentinel_TaskGroup = nullptr;
    TaskGroup_ReportClicked_Original(windowsUdkTaskGroup, &g_clickSentinel);
    void* result = g_clickSentinel_TaskGroup;
    g_clickSentinel_TaskGroup = prev;
    return result;
}

winrt::com_ptr<IUnknown> GetWindowsUdkTaskItemFromTaskListButton(
    UIElement element) {
    if (!TryGetItemFromContainer_TaskListWindowViewModel_Original ||
        !TaskListWindowViewModel_get_TaskItem_Original) {
        return nullptr;
    }
    winrt::com_ptr<IUnknown> windowViewModel;
    TryGetItemFromContainer_TaskListWindowViewModel_Original(
        windowViewModel.put_void(), &element);
    if (!windowViewModel) {
        return nullptr;
    }
    winrt::com_ptr<IUnknown> windowsUdkTaskItem;
    TaskListWindowViewModel_get_TaskItem_Original(
        windowViewModel.get(), windowsUdkTaskItem.put_void());
    return windowsUdkTaskItem;
}

void* GetWindowsUdkTaskGroupFromTaskListButton(UIElement element) {
    if (!TryGetItemFromContainer_TaskListGroupViewModel_Original ||
        !TaskListGroupViewModel_IsMultiWindow_Original) {
        return nullptr;
    }
    winrt::com_ptr<IUnknown> groupViewModel;
    TryGetItemFromContainer_TaskListGroupViewModel_Original(
        groupViewModel.put_void(), &element);
    if (!groupViewModel) {
        return nullptr;
    }
    g_capturedTaskGroup = nullptr;
    g_captureTaskGroup = true;
    TaskListGroupViewModel_IsMultiWindow_Original((void**)groupViewModel.get() -
                                                  1);
    g_captureTaskGroup = false;
    return g_capturedTaskGroup;
}

HWND GetWindowFromTaskItem(void* taskItem) {
    if (!taskItem) {
        return nullptr;
    }
    if (CImmersiveTaskItem_vftable_ITaskItem &&
        *(void**)taskItem == CImmersiveTaskItem_vftable_ITaskItem &&
        CImmersiveTaskItem_GetAppWindow && CImmersiveTaskItem_vftable) {
        void* immersiveTaskItem =
            QueryViaVtable(taskItem, CImmersiveTaskItem_vftable);
        if (!immersiveTaskItem) {
            return nullptr;
        }
        return CImmersiveTaskItem_GetAppWindow(immersiveTaskItem);
    }
    if (CWindowTaskItem_GetWindow) {
        void* windowTaskItem = taskItem;
        if (CWindowTaskItem_vftable_ITaskItem && CWindowTaskItem_vftable &&
            *(void**)taskItem == CWindowTaskItem_vftable_ITaskItem) {
            windowTaskItem = QueryViaVtable(taskItem, CWindowTaskItem_vftable);
            if (!windowTaskItem) {
                return nullptr;
            }
        }
        return CWindowTaskItem_GetWindow(windowTaskItem);
    }
    return nullptr;
}

// Button → PID via taskband (volume-mod approach).
DWORD GetProcessIdFromTaskListButton(UIElement element) {
    try {
        auto windowsUdkTaskItem =
            GetWindowsUdkTaskItemFromTaskListButton(element);
        if (windowsUdkTaskItem) {
            void* nativeTaskItem = GetNativeTaskItemFromWindowsUdkTaskItem(
                windowsUdkTaskItem.get());
            if (nativeTaskItem) {
                HWND hWnd = GetWindowFromTaskItem(nativeTaskItem);
                if (hWnd) {
                    DWORD processId = 0;
                    GetWindowThreadProcessId(hWnd, &processId);
                    return processId;
                }
            }
        }

        void* windowsUdkTaskGroup =
            GetWindowsUdkTaskGroupFromTaskListButton(element);
        if (windowsUdkTaskGroup) {
            void* nativeTaskGroup =
                GetNativeTaskGroupFromWindowsUdkTaskGroup(windowsUdkTaskGroup);
            if (nativeTaskGroup) {
                HDPA taskItemsArray = GetTaskItemsArray(nativeTaskGroup);
                if (taskItemsArray && DPA_GetPtrCount(taskItemsArray) > 0) {
                    void* taskItem = DPA_GetPtr(taskItemsArray, 0);
                    HWND hWnd = GetWindowFromTaskItem(taskItem);
                    if (hWnd) {
                        DWORD processId = 0;
                        GetWindowThreadProcessId(hWnd, &processId);
                        return processId;
                    }
                }
            }
        }
    } catch (...) {
    }
    return 0;
}

// Resolve button → path; force=true on click. Returns path upper or empty.
std::wstring EnsureButtonPathCached(FrameworkElement button, bool force) {
    if (!button || !g_taskbandResolveReady.load()) {
        return {};
    }

    const ULONGLONG now = GetTickCount64();
    {
        std::lock_guard<std::mutex> lock(g_buttonPathMutex);
        for (auto& e : g_buttonPathCache) {
            FrameworkElement b = nullptr;
            try {
                b = e.button.get();
            } catch (...) {
                continue;
            }
            if (b != button) {
                continue;
            }
            const ULONGLONG debounceMs = e.pathUpper.empty() ? 250 : 2000;
            if (!force && e.resolveAttempted &&
                (now - e.lastResolveTick) < debounceMs) {
                return e.pathUpper;
            }
            // fall through to re-resolve below using this entry
            break;
        }
    }

    DWORD pid = 0;
    HWND hwnd = nullptr;
    std::vector<HWND> groupHwnds;
    try {
        UIElement el = button.as<UIElement>();
        // Prefer full resolve (also captures HWND via item path).
        auto udkItem = GetWindowsUdkTaskItemFromTaskListButton(el);
        if (udkItem) {
            void* native =
                GetNativeTaskItemFromWindowsUdkTaskItem(udkItem.get());
            hwnd = GetWindowFromTaskItem(native);
            if (hwnd) {
                GetWindowThreadProcessId(hwnd, &pid);
                groupHwnds.push_back(hwnd);
            }
        }
        void* windowsUdkTaskGroup =
            GetWindowsUdkTaskGroupFromTaskListButton(el);
        if (windowsUdkTaskGroup) {
            void* nativeTaskGroup =
                GetNativeTaskGroupFromWindowsUdkTaskGroup(windowsUdkTaskGroup);
            if (nativeTaskGroup) {
                HDPA taskItemsArray = GetTaskItemsArray(nativeTaskGroup);
                if (taskItemsArray) {
                    const int n = DPA_GetPtrCount(taskItemsArray);
                    for (int i = 0; i < n; ++i) {
                        HWND h = GetWindowFromTaskItem(
                            DPA_GetPtr(taskItemsArray, i));
                        if (!h || !IsWindow(h)) {
                            continue;
                        }
                        if (std::find(groupHwnds.begin(), groupHwnds.end(),
                                      h) == groupHwnds.end()) {
                            groupHwnds.push_back(h);
                        }
                        if (!hwnd) {
                            hwnd = h;
                        }
                        if (!pid) {
                            GetWindowThreadProcessId(h, &pid);
                        }
                    }
                }
            }
        }
        if (!pid) {
            pid = GetProcessIdFromTaskListButton(el);
        }
    } catch (...) {
        pid = 0;
    }

    std::wstring pathUpper;
    if (pid) {
        std::wstring path = GetProcessImagePath(pid);
        if (!path.empty()) {
            pathUpper = ToUpper(path);
        }
    }

    std::wstring classUpper =
        hwnd ? ToUpper(GetWindowClassName(hwnd)) : std::wstring{};
    std::wstring appIdUpper =
        hwnd ? ToUpper(GetWindowAppUserModelId(hwnd)) : std::wstring{};
    std::wstring autoIdUpper = GetButtonAutomationAppId(button);
    if (appIdUpper.empty()) {
        appIdUpper = autoIdUpper;
    }

    {
        std::lock_guard<std::mutex> lock(g_buttonPathMutex);
        bool found = false;
        for (auto& e : g_buttonPathCache) {
            FrameworkElement b = nullptr;
            try {
                b = e.button.get();
            } catch (...) {
                continue;
            }
            if (b != button) {
                continue;
            }
            e.pathUpper = pathUpper;
            e.appIdUpper = appIdUpper;
            e.classUpper = classUpper;
            e.autoIdUpper = autoIdUpper;
            e.pid = pid;
            e.sampleHwnd = hwnd;
            e.groupHwnds = groupHwnds;
            e.resolveAttempted = true;
            e.lastResolveTick = now;
            found = true;
            break;
        }
        if (!found) {
            ButtonPathCacheEntry e;
            e.button = winrt::make_weak(button);
            e.pathUpper = pathUpper;
            e.appIdUpper = appIdUpper;
            e.classUpper = classUpper;
            e.autoIdUpper = autoIdUpper;
            e.pid = pid;
            e.sampleHwnd = hwnd;
            e.groupHwnds = std::move(groupHwnds);
            e.resolveAttempted = true;
            e.lastResolveTick = now;
            g_buttonPathCache.push_back(std::move(e));
        }
        // Prune dead weaks occasionally.
        if (g_buttonPathCache.size() > 128) {
            g_buttonPathCache.erase(
                std::remove_if(
                    g_buttonPathCache.begin(), g_buttonPathCache.end(),
                    [](ButtonPathCacheEntry& e) {
                        try {
                            return !e.button.get();
                        } catch (...) {
                            return true;
                        }
                    }),
                g_buttonPathCache.end());
        }
    }

    if (SettingsSnap()->glowDebugLog) {
        Wh_Log(L"Button path cache: pid=%u path=%s class=%s appId=%s force=%d "
               L"name=\"%s\"",
               pid, pathUpper.empty() ? L"(none)" : pathUpper.c_str(),
               classUpper.empty() ? L"?" : classUpper.c_str(),
               appIdUpper.empty() ? L"?" : appIdUpper.c_str(), force ? 1 : 0,
               GetButtonAutomationName(button).c_str());
    }
    return pathUpper;
}

// Lookup only (no resolve).
std::wstring GetCachedButtonPath(FrameworkElement button) {
    return GetCachedButtonIdentity(button).pathUpper;
}

ButtonIdentity GetCachedButtonIdentity(FrameworkElement button) {
    ButtonIdentity out;
    if (!button) {
        return out;
    }
    std::lock_guard<std::mutex> lock(g_buttonPathMutex);
    for (auto& e : g_buttonPathCache) {
        try {
            if (e.button.get() == button) {
                out.pathUpper = e.pathUpper;
                out.appIdUpper = e.appIdUpper;
                out.classUpper = e.classUpper;
                out.autoIdUpper = e.autoIdUpper;
                out.pid = e.pid;
                out.sampleHwnd = e.sampleHwnd;
                out.groupHwnds = e.groupHwnds;
                return out;
            }
        } catch (...) {
        }
    }
    return out;
}

int GetCachedPaintRank(FrameworkElement button) {
    if (!button) {
        return -1;
    }
    std::lock_guard<std::mutex> lock(g_buttonPathMutex);
    for (auto& e : g_buttonPathCache) {
        try {
            if (e.button.get() == button) {
                return e.lastPaintRank;
            }
        } catch (...) {
        }
    }
    return -1;
}

void SetCachedPaintRank(FrameworkElement button, int rank) {
    if (!button) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_buttonPathMutex);
    for (auto& e : g_buttonPathCache) {
        try {
            if (e.button.get() == button) {
                e.lastPaintRank = rank;
                return;
            }
        } catch (...) {
        }
    }
    ButtonPathCacheEntry stub;
    stub.button = winrt::make_weak(button);
    stub.lastPaintRank = rank;
    g_buttonPathCache.push_back(std::move(stub));
}

template <typename Fn>
void ForEachLiveElementOnThisDispatcher(
    const std::vector<winrt::weak_ref<FrameworkElement>>& weaks,
    Fn&& fn) {
    for (auto& weak : weaks) {
        FrameworkElement el = nullptr;
        try {
            el = weak.get();
        } catch (...) {
            continue;
        }
        if (!el) {
            continue;
        }
        try {
            auto dispatcher = el.Dispatcher();
            if (!dispatcher || !dispatcher.HasThreadAccess()) {
                continue;
            }
        } catch (...) {
            continue;
        }
        fn(el);
    }
}

std::vector<FrameworkElement> CollectLiveButtonsOnThisDispatcher() {
    PruneTrackedButtons_UIThread();
    std::vector<winrt::weak_ref<FrameworkElement>> buttons;
    {
        std::lock_guard<std::mutex> lock(g_buttonsMutex);
        buttons = g_trackedButtons;
    }
    std::vector<FrameworkElement> live;
    live.reserve(buttons.size());
    ForEachLiveElementOnThisDispatcher(
        buttons, [&](FrameworkElement b) { live.push_back(b); });
    return live;
}

void ApplyAllHighlights_UIThread() {
    g_lastFullRefreshTick = GetTickCount64();
    RefreshCurrentDesktopId();

    std::vector<AppFocusInfo> ranks;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        ranks = CurrentDeskLocked().rankedApps;
    }

    std::vector<FrameworkElement> live = CollectLiveButtonsOnThisDispatcher();

    if (SettingsSnap()->glowDebugLog) {
        for (size_t i = 0; i < ranks.size(); ++i) {
            Wh_Log(L"  rank list[%zu]: %s", i + 1, ranks[i].displayName.c_str());
        }
        Wh_Log(L"ApplyAllHighlights: %zu tracked buttons, %zu ranks, enabled=%d "
               L"desktop=%s",
               live.size(), ranks.size(), SettingsSnap()->enabled ? 1 : 0,
               GuidToLogString(g_currentDesktopId).c_str());
    }

    if (!SettingsSnap()->enabled || g_unloading.load() || ranks.empty()) {
        for (auto& button : live) {
            SetCachedPaintRank(button, 0);
            ClearButtonHighlight(button);
        }
        g_pendingOverlaySweep = false;
        if (SettingsSnap()->glowDebugLog && ranks.empty()) {
            Wh_Log(L"ApplyAllHighlights: no ranks — cleared overlays on %zu "
                   L"tracked buttons",
                   live.size());
        }
        return;
    }

    if (SettingsSnap()->glowDebugLog) {
        int dumped = 0;
        for (auto& button : live) {
            Wh_Log(L"  button[%d]: running=%d name=\"%s\"", dumped,
                   TaskListButton_IsRunning(button) ? 1 : 0,
                   GetButtonAutomationName(button).c_str());
            if (++dumped >= 24) {
                break;
            }
        }
    }

    // Prefer process-path cache (option C), then fuzzy name scores.
    // 1:1 assignment, highest score wins.
    struct Cand {
        int score;
        size_t rankIdx;
        size_t buttonIdx;
    };
    std::vector<Cand> cands;
    std::vector<std::wstring> buttonPaths(live.size());
    for (size_t bi = 0; bi < live.size(); ++bi) {
        // Resolve once if missing (cheap if cached).
        buttonPaths[bi] = EnsureButtonPathCached(live[bi], /*force=*/false);
        for (size_t ri = 0; ri < ranks.size(); ++ri) {
            int s = ScoreButtonForRank(live[bi], ranks[ri], true);
            if (s >= kScoreMinBind) {
                cands.push_back({s, ri, bi});
            }
        }
    }
    std::sort(cands.begin(), cands.end(),
              [](const Cand& a, const Cand& b) { return a.score > b.score; });

    std::vector<int> buttonRank(live.size(), 0);  // 1-based rank or 0
    std::vector<bool> rankTaken(ranks.size(), false);
    std::vector<bool> buttonTaken(live.size(), false);

    // Replicas of one identity may share a rank (secondary taskbar / Never
    // Combine). Only exact identity (1000: path / HWND / AUMID). Name-cache
    // 96 is 1:1 — it used to copy Settings' rank onto Windows Security.
    for (const auto& c : cands) {
        if (buttonTaken[c.buttonIdx]) {
            continue;
        }
        const bool replica = c.score >= kScoreExactIdentity;
        if (replica) {
            buttonTaken[c.buttonIdx] = true;
            rankTaken[c.rankIdx] = true;
            buttonRank[c.buttonIdx] = static_cast<int>(c.rankIdx) + 1;
        } else if (!rankTaken[c.rankIdx]) {
            rankTaken[c.rankIdx] = true;
            buttonTaken[c.buttonIdx] = true;
            buttonRank[c.buttonIdx] = static_cast<int>(c.rankIdx) + 1;
        } else {
            continue;
        }

        std::wstring autoName = GetButtonAutomationName(live[c.buttonIdx]);
        if (c.score >= kScoreExactIdentity) {
            StoreAutomationName(ranks[c.rankIdx], autoName);
        } else {
            StoreAutomationNameIfFits(ranks[c.rankIdx], autoName);
        }
        // Successful bind ⇒ this process has a taskbar presence.
        {
            std::lock_guard<std::mutex> lock(g_stateMutex);
            auto it = CurrentDeskLocked().appFocusMap.find(ranks[c.rankIdx].key);
            if (it != CurrentDeskLocked().appFocusMap.end()) {
                it->second.seenOnTaskbar = true;
            }
        }
        if (SettingsSnap()->glowDebugLog) {
            Wh_Log(L"  bind rank %zu %s -> \"%s\" (score=%d path=%s)",
                   c.rankIdx + 1, ranks[c.rankIdx].displayName.c_str(),
                   autoName.c_str(), c.score,
                   buttonPaths[c.buttonIdx].empty()
                       ? L"?"
                       : buttonPaths[c.buttonIdx].c_str());
        }
    }

    for (size_t bi = 0; bi < live.size(); ++bi) {
        SetCachedPaintRank(live[bi], buttonRank[bi]);
        if (buttonRank[bi] > 0) {
            ApplyButtonHighlight(live[bi], buttonRank[bi]);
        } else {
            ClearButtonHighlight(live[bi]);
        }
    }

    // Sweep completed for all currently tracked buttons.
    if (g_pendingOverlaySweep.load()) {
        g_pendingOverlaySweep = false;
        Wh_Log(L"Overlay sweep finished (%zu buttons cleared/updated)",
               live.size());
    }

// Second pass: unmatched ranks — pick best free button with lower bar
    // (uses window title). Helps Windhawk and odd product names.
    std::vector<std::wstring> demoteKeys;
    for (size_t ri = 0; ri < ranks.size(); ++ri) {
        if (rankTaken[ri]) {
            continue;
        }
        int bestScore = 0;
        size_t bestBi = SIZE_MAX;
        for (size_t bi = 0; bi < live.size(); ++bi) {
            if (buttonTaken[bi]) {
                continue;
            }
            int s = ScoreButtonForRank(live[bi], ranks[ri], true);
            if (s > bestScore) {
                bestScore = s;
                bestBi = bi;
            }
        }
        std::wstring autoName = (bestBi != SIZE_MAX)
                                    ? GetButtonAutomationName(live[bestBi])
                                    : std::wstring{};
        if (bestBi != SIZE_MAX && bestScore >= kScoreMinBind &&
            AutomationNameFitsRank(ranks[ri], autoName)) {
            rankTaken[ri] = true;
            buttonTaken[bestBi] = true;
            buttonRank[bestBi] = static_cast<int>(ri) + 1;
            StoreAutomationNameIfFits(ranks[ri], autoName);
            {
                std::lock_guard<std::mutex> lock(g_stateMutex);
                auto& map = CurrentDeskLocked().appFocusMap;
                auto it = map.find(ranks[ri].key);
                if (it != map.end()) {
                    it->second.seenOnTaskbar = true;
                }
            }
            if (SettingsSnap()->glowDebugLog) {
                Wh_Log(L"  bind rank %zu %s -> \"%s\" (score=%d, fallback)",
                       ri + 1, ranks[ri].displayName.c_str(), autoName.c_str(),
                       bestScore);
            }
            SetCachedPaintRank(live[bestBi], buttonRank[bestBi]);
            ApplyButtonHighlight(live[bestBi], buttonRank[bestBi]);
        } else {
            if (SettingsSnap()->glowDebugLog) {
                Wh_Log(L"  UNMATCHED rank %zu: %s (bestScore=%d title=\"%s\")",
                       ri + 1, ranks[ri].displayName.c_str(), bestScore,
                       ranks[ri].lastWindowTitle.c_str());
                if (bestBi != SIZE_MAX) {
                    Wh_Log(L"    closest button was \"%s\"",
                           GetButtonAutomationName(live[bestBi]).c_str());
                }
            }
            // Tray-only / no button: drop from ranks so it stops occupying a
            // slot (e.g. HA Desktop Widget). Only demote once we already know
            // several real taskbar paths (avoid false demote at explorer start).
            size_t resolvedButtons = 0;
            {
                std::lock_guard<std::mutex> lock(g_buttonPathMutex);
                for (const auto& e : g_buttonPathCache) {
                    if (!e.pathUpper.empty()) {
                        ++resolvedButtons;
                    }
                }
            }
            if (SettingsSnap()->requireTaskbarButton && bestScore == 0 &&
                resolvedButtons >= 2) {
                demoteKeys.push_back(ranks[ri].key);
            }
        }
    }
    // Demote after the loop so rank list stays stable while matching.
    if (!demoteKeys.empty()) {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        auto& map = CurrentDeskLocked().appFocusMap;
        for (const auto& key : demoteKeys) {
            auto it = map.find(key);
            if (it != map.end() && !it->second.seenOnTaskbar) {
                Wh_Log(L"  demoting non-taskbar app from ranks: %s",
                       it->second.displayName.c_str());
                it->second.lastConfirmedFocusTick = 0;
                it->second.seenOnTaskbar = false;
            }
        }
        RecomputeRanksLocked();
    }
}

void ClearAllHighlights_UIThread() {
    for (auto& button : CollectLiveButtonsOnThisDispatcher()) {
        SetCachedPaintRank(button, 0);
        ClearButtonHighlight(button);
    }
}

// ---------------------------------------------------------------------------
// Thumbnail preview glow (multi-window flyouts)
// ---------------------------------------------------------------------------

void AddThumbnailTaskItemMapping(
    winrt::Windows::Foundation::IInspectable thumbnail,
    void* taskGroup,
    void* taskItem) {
    if (!thumbnail || !taskItem) {
        return;
    }
    HWND hwnd = GetWindowFromTaskItem(taskItem);
    std::lock_guard<std::mutex> lock(g_thumbnailMapMutex);
    std::erase_if(g_thumbnailTaskItemMapping, [&](const ThumbnailTaskItemMapping& item) {
        try {
            auto t = item.thumbnail.get();
            if (!t || t == thumbnail) {
                return true;
            }
        } catch (...) {
            return true;
        }
        return item.taskGroup == taskGroup && item.taskItem == taskItem;
    });
    ThumbnailTaskItemMapping entry;
    entry.thumbnail = winrt::make_weak(thumbnail);
    entry.taskGroup = taskGroup;
    entry.taskItem = taskItem;
    entry.hwnd = hwnd;
    g_thumbnailTaskItemMapping.push_back(std::move(entry));
    // Soft cap
    if (g_thumbnailTaskItemMapping.size() > 128) {
        std::erase_if(g_thumbnailTaskItemMapping, [](const ThumbnailTaskItemMapping& item) {
            try {
                return !item.thumbnail.get();
            } catch (...) {
                return true;
            }
        });
    }
}

HWND HwndFromMappingEntry(const ThumbnailTaskItemMapping& item) {
    if (item.hwnd && IsWindow(item.hwnd)) {
        return item.hwnd;
    }
    if (item.taskItem) {
        HWND h = GetWindowFromTaskItem(item.taskItem);
        if (h && IsWindow(h)) {
            return h;
        }
    }
    return nullptr;
}

// Closed flyouts leave dead weaks; their HWNDs/groups must not be reused to
// resolve a later app's sibling count (TC Lister vs an earlier Chrome hover).
bool ThumbnailMappingLive(const ThumbnailTaskItemMapping& item) {
    try {
        return item.thumbnail.get() != nullptr;
    } catch (...) {
        return false;
    }
}

// True if two WinRT objects are the same COM identity (different projections
// of the same TaskItemThumbnail still match).
bool SameInspectableIdentity(winrt::Windows::Foundation::IInspectable const& a,
                             winrt::Windows::Foundation::IInspectable const& b) {
    if (!a || !b) {
        return false;
    }
    try {
        if (a == b) {
            return true;
        }
        if (winrt::get_abi(a) == winrt::get_abi(b)) {
            return true;
        }
        // Canonical COM identity (QI to IUnknown).
        auto ua = a.as<::IUnknown>();
        auto ub = b.as<::IUnknown>();
        return winrt::get_abi(ua) == winrt::get_abi(ub);
    } catch (...) {
        return false;
    }
}

// Strong resolve: TaskItemThumbnail model (DataContext) → HWND.
// Returns also taskGroup when found (for index-order fill of siblings).
HWND ResolveHwndFromThumbnailModel(
    winrt::Windows::Foundation::IInspectable thumbnail,
    void** outTaskGroup = nullptr) {
    if (outTaskGroup) {
        *outTaskGroup = nullptr;
    }
    if (!thumbnail) {
        return nullptr;
    }
    std::lock_guard<std::mutex> lock(g_thumbnailMapMutex);
    for (const auto& item : g_thumbnailTaskItemMapping) {
        try {
            auto t = item.thumbnail.get();
            if (!t) {
                continue;
            }
            if (!SameInspectableIdentity(t, thumbnail)) {
                continue;
            }
            if (outTaskGroup) {
                *outTaskGroup = item.taskGroup;
            }
            return HwndFromMappingEntry(item);
        } catch (...) {
        }
    }
    return nullptr;
}

// HWNDs for one task group in the *current* flyout's construction order.
// Closed-flyout maps stay in the vector; first-seen order is the previous
// hover and is often reversed after a click (Windows rebuilds MRU).
std::vector<HWND> HwndsForTaskGroupInOrder(void* taskGroup) {
    std::vector<HWND> newestFirst;
    if (!taskGroup) {
        return newestFirst;
    }
    std::lock_guard<std::mutex> lock(g_thumbnailMapMutex);
    for (auto it = g_thumbnailTaskItemMapping.rbegin();
         it != g_thumbnailTaskItemMapping.rend(); ++it) {
        if (it->taskGroup != taskGroup || !ThumbnailMappingLive(*it)) {
            continue;
        }
        HWND h = HwndFromMappingEntry(*it);
        if (!h) {
            continue;
        }
        if (std::find(newestFirst.begin(), newestFirst.end(), h) ==
            newestFirst.end()) {
            newestFirst.push_back(h);
        }
    }
    std::reverse(newestFirst.begin(), newestFirst.end());
    return newestFirst;
}

// Pick a taskGroup that has exactly `siblingCount` unique HWNDs (current flyout).
void* FindTaskGroupForSiblingCount(size_t siblingCount) {
    if (siblingCount == 0) {
        return nullptr;
    }
    std::lock_guard<std::mutex> lock(g_thumbnailMapMutex);
    // group -> ordered unique hwnds
    struct GroupAcc {
        void* group = nullptr;
        std::vector<HWND> hwnds;
    };
    std::vector<GroupAcc> groups;
    for (const auto& item : g_thumbnailTaskItemMapping) {
        if (!item.taskGroup || !ThumbnailMappingLive(item)) {
            continue;
        }
        HWND h = HwndFromMappingEntry(item);
        if (!h) {
            continue;
        }
        GroupAcc* acc = nullptr;
        for (auto& g : groups) {
            if (g.group == item.taskGroup) {
                acc = &g;
                break;
            }
        }
        if (!acc) {
            groups.push_back({item.taskGroup, {}});
            acc = &groups.back();
        }
        if (std::find(acc->hwnds.begin(), acc->hwnds.end(), h) ==
            acc->hwnds.end()) {
            acc->hwnds.push_back(h);
        }
    }
    // Last exact match wins — maps are append-only, so the newest live
    // flyout is preferred over an older group with the same window count.
    void* exact = nullptr;
    for (const auto& g : groups) {
        if (g.hwnds.size() == siblingCount) {
            exact = g.group;
        }
    }
    if (exact) {
        return exact;
    }
    // Prefer largest group that is at least siblingCount (partial flyout).
    void* best = nullptr;
    size_t bestN = 0;
    for (const auto& g : groups) {
        if (g.hwnds.size() >= siblingCount && g.hwnds.size() > bestN) {
            bestN = g.hwnds.size();
            best = g.group;
        }
    }
    return best;
}

// Last N unique HWNDs from the mapping vector (most recently constructed models
// — typically the open flyout when maps were just created on hover).
std::vector<HWND> LastMappedHwnds(size_t n) {
    std::vector<HWND> out;
    if (n == 0) {
        return out;
    }
    std::lock_guard<std::mutex> lock(g_thumbnailMapMutex);
    for (auto it = g_thumbnailTaskItemMapping.rbegin();
         it != g_thumbnailTaskItemMapping.rend() && out.size() < n; ++it) {
        if (!ThumbnailMappingLive(*it)) {
            continue;
        }
        HWND h = HwndFromMappingEntry(*it);
        if (!h) {
            continue;
        }
        if (std::find(out.begin(), out.end(), h) == out.end()) {
            out.push_back(h);
        }
    }
    // We walked newest→oldest; reverse to construction/display order.
    std::reverse(out.begin(), out.end());
    return out;
}

HWND ResolveHwndForThumbnailView(FrameworkElement thumbView,
                                 void** outTaskGroup = nullptr) {
    if (outTaskGroup) {
        *outTaskGroup = nullptr;
    }
    if (!thumbView) {
        return nullptr;
    }
    try {
        // DataContext is often the TaskItemThumbnail model object.
        auto dc = thumbView.DataContext();
        if (dc) {
            if (HWND h = ResolveHwndFromThumbnailModel(dc, outTaskGroup)) {
                return h;
            }
        }
    } catch (...) {
    }
    // No title fallback here — identical titles (Calibre 2× same file) would
    // all bind to the same HWND. Callers do unique assignment separately.
    return nullptr;
}

// Title → HWND only for unique assignment (each HWND at most once).
HWND MatchTitleToUnusedRecent(const std::wstring& autoName,
                              const std::vector<WindowFocusInfo>& recent,
                              const std::unordered_set<HWND>& used) {
    if (autoName.empty()) {
        return nullptr;
    }
    const std::wstring cardPath = ToUpper(ExtractBracketedPath(autoName));
    const std::wstring cardFile =
        cardPath.empty() ? std::wstring{}
                         : ToUpper(FileNameFromPath(cardPath));

    int bestScore = 0;
    HWND bestHwnd = nullptr;
    ULONGLONG bestTick = 0;
    for (const auto& info : recent) {
        if (!info.hwnd || used.count(info.hwnd)) {
            continue;
        }
        int s = ScoreTitleToAutomationName(info.windowTitle, autoName);
        if (IsWindow(info.hwnd)) {
            const std::wstring liveTitle = GetWindowTitle(info.hwnd);
            s = (std::max)(s, ScoreTitleToAutomationName(liveTitle, autoName));
            if (!cardPath.empty()) {
                const std::wstring winPath =
                    ToUpper(ExtractBracketedPath(liveTitle));
                const std::wstring infoPath =
                    ToUpper(ExtractBracketedPath(info.windowTitle));
                if (winPath == cardPath || infoPath == cardPath) {
                    s = (std::max)(s, 100);
                } else if (!cardFile.empty()) {
                    if (ToUpper(FileNameFromPath(winPath)) == cardFile ||
                        ToUpper(FileNameFromPath(infoPath)) == cardFile ||
                        ToUpper(FileNameFromPath(liveTitle)) == cardFile ||
                        ToUpper(FileNameFromPath(info.windowTitle)) ==
                            cardFile) {
                        s = (std::max)(s, 96);
                    }
                }
            }
        }
        if (s > bestScore ||
            (s == bestScore && s >= 70 && info.lastConfirmedTick > bestTick)) {
            bestScore = s;
            bestHwnd = info.hwnd;
            bestTick = info.lastConfirmedTick;
        }
    }
    return bestScore >= 70 ? bestHwnd : nullptr;
}

void CollectThumbnailViewsUnder(FrameworkElement root,
                                std::vector<FrameworkElement>& out) {
    if (!root) {
        return;
    }
    try {
        if (winrt::get_class_name(root) == L"Taskbar.TaskItemThumbnailView") {
            out.push_back(root);
        }
        int n = Media::VisualTreeHelper::GetChildrenCount(root);
        for (int i = 0; i < n; ++i) {
            auto child = Media::VisualTreeHelper::GetChild(root, i)
                             .try_as<FrameworkElement>();
            if (child) {
                CollectThumbnailViewsUnder(child, out);
            }
        }
    } catch (...) {
    }
}

FrameworkElement FindAncestorByClassName(FrameworkElement element,
                                         PCWSTR className) {
    FrameworkElement cur = element;
    for (int guard = 0; guard < 32 && cur; ++guard) {
        try {
            if (winrt::get_class_name(cur) == className) {
                return cur;
            }
            cur = Media::VisualTreeHelper::GetParent(cur)
                      .try_as<FrameworkElement>();
        } catch (...) {
            break;
        }
    }
    return nullptr;
}

std::vector<FrameworkElement> CollectSiblingThumbnailViews(
    FrameworkElement thumbView) {
    std::vector<FrameworkElement> out;
    if (!thumbView) {
        return out;
    }

    // Prefer known list hosts from the XAML thumbnail flyout.
    static const PCWSTR kHosts[] = {
        L"Taskbar.TaskItemThumbnailList",
        L"Taskbar.TaskItemThumbnailScrollableList",
        L"Taskbar.FlyoutFrame",
    };
    FrameworkElement host = nullptr;
    for (auto name : kHosts) {
        host = FindAncestorByClassName(thumbView, name);
        if (host) {
            break;
        }
    }
    if (!host) {
        // Walk up a few parents and collect from the highest with 2+ thumbs.
        FrameworkElement cur = thumbView;
        for (int i = 0; i < 8 && cur; ++i) {
            try {
                cur = Media::VisualTreeHelper::GetParent(cur)
                          .try_as<FrameworkElement>();
            } catch (...) {
                cur = nullptr;
            }
            if (cur) {
                host = cur;
            }
        }
    }
    if (host) {
        CollectThumbnailViewsUnder(host, out);
    }
    if (out.empty()) {
        out.push_back(thumbView);
    }
    return out;
}

void TrackThumbView_UIThread(FrameworkElement thumbView) {
    if (!thumbView) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_thumbViewsMutex);
    for (auto& weak : g_trackedThumbViews) {
        try {
            if (weak.get() == thumbView) {
                return;
            }
        } catch (...) {
        }
    }
    g_trackedThumbViews.push_back(winrt::make_weak(thumbView));
    if (g_trackedThumbViews.size() > 64) {
        g_trackedThumbViews.erase(
            std::remove_if(
                g_trackedThumbViews.begin(), g_trackedThumbViews.end(),
                [](winrt::weak_ref<FrameworkElement>& weak) {
                    try {
                        return !weak.get();
                    } catch (...) {
                        return true;
                    }
                }),
            g_trackedThumbViews.end());
    }
}

void RemoveNamedDescendant(FrameworkElement root, PCWSTR name) {
    if (!root || !name) {
        return;
    }
    for (int guard = 0; guard < 4; ++guard) {
        auto existing = FindDescendantByName(root, name);
        if (!existing) {
            return;
        }
        try {
            if (auto parent = Media::VisualTreeHelper::GetParent(existing)
                                  .try_as<Controls::Panel>()) {
                uint32_t idx = 0;
                if (parent.Children().IndexOf(existing, idx)) {
                    parent.Children().RemoveAt(idx);
                    continue;
                }
            }
        } catch (...) {
        }
        break;
    }
}

// Find a Panel to host overlays (root Grid under TaskItemThumbnailView).
Controls::Panel GetThumbnailHostPanel(FrameworkElement thumbView) {
    if (!thumbView) {
        return nullptr;
    }
    try {
        if (auto p = thumbView.try_as<Controls::Panel>()) {
            return p;
        }
        int n = Media::VisualTreeHelper::GetChildrenCount(thumbView);
        for (int i = 0; i < n; ++i) {
            auto child = Media::VisualTreeHelper::GetChild(thumbView, i)
                             .try_as<FrameworkElement>();
            if (!child) {
                continue;
            }
            if (winrt::get_class_name(child) ==
                L"Windows.UI.Xaml.Controls.Grid") {
                if (auto p = child.try_as<Controls::Panel>()) {
                    return p;
                }
            }
            if (auto p = child.try_as<Controls::Panel>()) {
                return p;
            }
        }
    } catch (...) {
    }
    return nullptr;
}

FrameworkElement FindThumbnailTitleElement(FrameworkElement thumbView) {
    if (!thumbView) {
        return nullptr;
    }
    static const PCWSTR kNames[] = {
        L"DisplayNameTextBlock",
        L"DisplayName",
        L"TitleTextBlock",
        L"Title",
    };
    for (auto name : kNames) {
        if (auto el = FindDescendantByName(thumbView, name)) {
            if (winrt::get_class_name(el) == L"Windows.UI.Xaml.Controls.TextBlock" ||
                el.try_as<Controls::TextBlock>()) {
                return el;
            }
            // Name matched a wrapper — prefer a TextBlock inside.
            if (auto tb = FindDescendantByName(el, L"DisplayNameTextBlock")) {
                return tb;
            }
            return el;
        }
    }
    // First TextBlock in the tree (title is usually the only one besides close).
    try {
        std::function<FrameworkElement(FrameworkElement)> walk;
        walk = [&](FrameworkElement root) -> FrameworkElement {
            if (!root) {
                return nullptr;
            }
            auto cn = winrt::get_class_name(root);
            if (cn == L"Windows.UI.Xaml.Controls.TextBlock") {
                // Skip close-button glyphs (often single-char / Segoe icons).
                try {
                    if (auto tb = root.try_as<Controls::TextBlock>()) {
                        auto text = tb.Text();
                        if (text.size() >= 2) {
                            return root;
                        }
                    }
                } catch (...) {
                    return root;
                }
            }
            int n = Media::VisualTreeHelper::GetChildrenCount(root);
            for (int i = 0; i < n; ++i) {
                auto child = Media::VisualTreeHelper::GetChild(root, i)
                                 .try_as<FrameworkElement>();
                if (auto found = walk(child)) {
                    return found;
                }
            }
            return nullptr;
        };
        return walk(thumbView);
    } catch (...) {
    }
    return nullptr;
}

std::wstring GetThumbnailAutomationName(FrameworkElement thumbView) {
    try {
        return NormalizeAutomationName(
            Automation::AutomationProperties::GetName(thumbView).c_str());
    } catch (...) {
        return {};
    }
}

std::wstring GetThumbnailDisplayText(FrameworkElement thumbView) {
    try {
        if (auto titleEl = FindThumbnailTitleElement(thumbView)) {
            if (auto tb = titleEl.try_as<Controls::TextBlock>()) {
                return NormalizeAutomationName(tb.Text().c_str());
            }
        }
    } catch (...) {
    }
    return {};
}

std::wstring TitleMatchKey(const std::wstring& raw) {
    std::wstring n = NormalizeAutomationName(raw);
    std::wstring path = ToUpper(ExtractBracketedPath(n));
    if (!path.empty()) {
        return path;
    }
    return AlnumUpper(n);
}

bool TitleKeysAreDistinct(const std::vector<std::wstring>& titles) {
    if (titles.size() < 2) {
        return false;
    }
    std::unordered_set<std::wstring> seen;
    for (const auto& t : titles) {
        std::wstring key = TitleMatchKey(t);
        if (key.empty() || !seen.insert(key).second) {
            return false;
        }
    }
    return true;
}

std::wstring GetThumbnailMatchTitle(FrameworkElement thumbView) {
    std::wstring autoName = GetThumbnailAutomationName(thumbView);
    std::wstring text = GetThumbnailDisplayText(thumbView);

    // Prefer the more specific string (file name vs generic "Lister").
    if (text.size() > autoName.size()) {
        return text;
    }
    if (autoName.size() > text.size()) {
        return autoName;
    }
    return !text.empty() ? text : autoName;
}

// Per-flyout: pick the title source that actually distinguishes cards.
// Ebook readers often put the book name on DisplayNameTextBlock while
// Automation Name is the shared "App - 2 running windows" (same on every
// sibling) — preferring the longer string then made titlesDistinct fail
// and we assigned HWNDs by stale group-order (swap after click).
std::vector<std::wstring> PickFlyoutCardTitles(
    const std::vector<FrameworkElement>& siblings) {
    std::vector<std::wstring> autos;
    std::vector<std::wstring> texts;
    std::vector<std::wstring> mixed;
    autos.reserve(siblings.size());
    texts.reserve(siblings.size());
    mixed.reserve(siblings.size());
    for (const auto& v : siblings) {
        autos.push_back(GetThumbnailAutomationName(v));
        texts.push_back(GetThumbnailDisplayText(v));
        mixed.push_back(GetThumbnailMatchTitle(v));
    }
    if (TitleKeysAreDistinct(texts)) {
        return texts;
    }
    if (TitleKeysAreDistinct(autos)) {
        return autos;
    }
    return mixed;
}

int ThumbnailPositionInSet(FrameworkElement view) {
    try {
        return Automation::AutomationProperties::GetPositionInSet(view);
    } catch (...) {
        return -1;
    }
}

void SortThumbnailViewsVisualOrder(std::vector<FrameworkElement>& views) {
    bool anyPos = false;
    for (const auto& v : views) {
        if (ThumbnailPositionInSet(v) >= 1) {
            anyPos = true;
            break;
        }
    }
    if (!anyPos) {
        return;
    }
    std::stable_sort(
        views.begin(), views.end(),
        [](const FrameworkElement& a, const FrameworkElement& b) {
            int pa = ThumbnailPositionInSet(a);
            int pb = ThumbnailPositionInSet(b);
            if (pa < 1) {
                pa = 100000;
            }
            if (pb < 1) {
                pb = 100000;
            }
            return pa < pb;
        });
}

// Snap-group card in the same flyout as the individual windows
// ("Group | Lister - [file] and 1 other window"). Must not be treated as a
// window thumbnail — it steals group-order HWND 0 and the recent glow.
bool IsSnapGroupThumbnailView(FrameworkElement view) {
    if (!view) {
        return false;
    }
    auto looksLikeGroup = [](const std::wstring& s) -> bool {
        if (s.empty()) {
            return false;
        }
        if (s.size() >= 5 && _wcsnicmp(s.c_str(), L"Group", 5) == 0 &&
            (s.size() == 5 || s[5] == L' ' || s[5] == L'|' || s[5] == L'-')) {
            return true;
        }
        if (s.find(L" other window") != std::wstring::npos) {
            return true;
        }
        return false;
    };

    try {
        std::wstring name =
            Automation::AutomationProperties::GetName(view).c_str();
        if (looksLikeGroup(name) ||
            looksLikeGroup(NormalizeAutomationName(name))) {
            return true;
        }
    } catch (...) {
    }

    try {
        if (auto title = FindThumbnailTitleElement(view)) {
            if (auto tb = title.try_as<Controls::TextBlock>()) {
                std::wstring text = tb.Text().c_str();
                if (looksLikeGroup(text)) {
                    return true;
                }
            }
        }
    } catch (...) {
    }

    if (auto repeater = FindDescendantByName(view, L"IconsRepeater")) {
        try {
            if (Media::VisualTreeHelper::GetChildrenCount(repeater) >= 2) {
                return true;
            }
        } catch (...) {
        }
    }
    return false;
}

struct EnumSameClassCtx {
    DWORD pid = 0;
    std::wstring classUpper;
    std::vector<HWND>* out = nullptr;
};

BOOL CALLBACK EnumSameClassWndProc(HWND hWnd, LPARAM lParam) {
    auto* ctx = reinterpret_cast<EnumSameClassCtx*>(lParam);
    if (!ctx || !ctx->out) {
        return FALSE;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid != ctx->pid || !IsWindowVisible(hWnd)) {
        return TRUE;
    }
    if (GetWindowLong(hWnd, GWL_STYLE) & WS_CHILD) {
        return TRUE;
    }
    if (ToUpper(GetWindowClassName(hWnd)) != ctx->classUpper) {
        return TRUE;
    }
    if (std::find(ctx->out->begin(), ctx->out->end(), hWnd) ==
        ctx->out->end()) {
        ctx->out->push_back(hWnd);
    }
    return TRUE;
}

std::vector<HWND> ExpandSameClassWindows(const std::vector<HWND>& seeds) {
    std::vector<HWND> out;
    for (HWND seed : seeds) {
        if (!seed || !IsWindow(seed)) {
            continue;
        }
        if (std::find(out.begin(), out.end(), seed) == out.end()) {
            out.push_back(seed);
        }
        DWORD pid = 0;
        GetWindowThreadProcessId(seed, &pid);
        std::wstring cls = ToUpper(GetWindowClassName(seed));
        if (!pid || cls.empty()) {
            continue;
        }
        EnumSameClassCtx ctx{pid, std::move(cls), &out};
        EnumWindows(EnumSameClassWndProc, reinterpret_cast<LPARAM>(&ctx));
    }
    return out;
}

FrameworkElement FindThumbnailBackgroundBorder(FrameworkElement thumbView) {
    if (!thumbView) {
        return nullptr;
    }
    if (auto b = FindDescendantByName(thumbView, L"BackgroundBorder")) {
        return b;
    }
    // First Border under the root grid (styler targets this).
    try {
        auto panel = GetThumbnailHostPanel(thumbView);
        if (!panel) {
            return nullptr;
        }
        auto fe = panel.as<FrameworkElement>();
        int n = Media::VisualTreeHelper::GetChildrenCount(fe);
        for (int i = 0; i < n; ++i) {
            auto child = Media::VisualTreeHelper::GetChild(fe, i)
                             .try_as<FrameworkElement>();
            if (!child) {
                continue;
            }
            auto cn = winrt::get_class_name(child);
            if (cn == L"Windows.UI.Xaml.Controls.Border") {
                return child;
            }
        }
    } catch (...) {
    }
    return nullptr;
}

FrameworkElement MakeThumbNativeMarker(
    winrt::Windows::Foundation::IInspectable const& savedBackground) {
    PCWSTR markerXaml = LR"(
        <Border xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
                Name="WhRecentFocusThumbNative"
                Width="0" Height="0" Opacity="0"
                IsHitTestVisible="False" Visibility="Collapsed"/>
    )";
    try {
        auto marker =
            Markup::XamlReader::Load(markerXaml).as<FrameworkElement>();
        if (savedBackground) {
            marker.Tag(savedBackground);
        }
        return marker;
    } catch (...) {
        return nullptr;
    }
}

void ClearThumbnailNativeStyles(FrameworkElement thumbView) {
    if (!thumbView) {
        return;
    }
    auto markerEl = FindDescendantByName(thumbView, kThumbNativeStyleMarker);
    if (!markerEl) {
        return;
    }
    winrt::Windows::Foundation::IInspectable saved;
    try {
        saved = markerEl.Tag();
    } catch (...) {
    }
    try {
        // Restore the brush Taskbar Styler (or the template) had, if any.
        if (auto border = FindThumbnailBackgroundBorder(thumbView)) {
            if (auto b = border.try_as<Controls::Border>()) {
                if (auto brush = saved.try_as<Media::Brush>()) {
                    b.Background(brush);
                } else {
                    b.ClearValue(Controls::Border::BackgroundProperty());
                }
            }
        }
    } catch (...) {
    }
    RemoveNamedDescendant(thumbView, kThumbNativeStyleMarker);
}

void ClearThumbnailHighlight(FrameworkElement thumbView) {
    if (!thumbView) {
        return;
    }
    try {
        RemoveNamedDescendant(thumbView, kThumbGlowElementName);
        RemoveNamedDescendant(thumbView, kThumbTitleBgName);
        RemoveNamedDescendant(thumbView, kThumbTitleBarName);
        ClearThumbnailNativeStyles(thumbView);
        if (auto panel = thumbView.try_as<Controls::Panel>()) {
            RemoveNamedChild(panel, kThumbGlowElementName);
            RemoveNamedChild(panel, kThumbTitleBgName);
            RemoveNamedChild(panel, kThumbTitleBarName);
            RemoveNamedChild(panel, kThumbNativeStyleMarker);
        }
        auto hostPanel = GetThumbnailHostPanel(thumbView);
        if (hostPanel) {
            RemoveNamedChild(hostPanel, kThumbGlowElementName);
            RemoveNamedChild(hostPanel, kThumbTitleBgName);
            RemoveNamedChild(hostPanel, kThumbTitleBarName);
            RemoveNamedChild(hostPanel, kThumbNativeStyleMarker);
        }
    } catch (...) {
    }
}

void BringElementToFront(Controls::Panel panel, UIElement el) {
    if (!panel || !el) {
        return;
    }
    try {
        auto children = panel.Children();
        uint32_t idx = 0;
        if (children.IndexOf(el, idx) && idx + 1 != children.Size()) {
            children.RemoveAt(idx);
            children.Append(el);
        }
    } catch (...) {
    }
}

// Overlay host that must NOT affect parent layout.
//
// Thumbnail cards often use a Grid with rows (title | image). A normal child
// lands in (0,0) — the title row — and expands that row (bar between icon and
// text + card grows sideways). Same fix as icon glow: span every row/column
// and Stretch to the arranged card size. Children are positioned with
// RenderTransform so their layout slot stays tiny (Width×Height of the bar
// only, transform ignored by measure).
Controls::Grid EnsureThumbOverlayHost(Controls::Panel panel) {
    FrameworkElement hostEl =
        FindChildByName(panel.as<FrameworkElement>(), kThumbGlowElementName);
    Controls::Grid host = hostEl ? hostEl.try_as<Controls::Grid>() : nullptr;
    if (!host) {
        PCWSTR xaml = LR"(
            <Grid xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
                  Name="WhRecentFocusThumbGlow"
                  IsHitTestVisible="False"
                  HorizontalAlignment="Stretch"
                  VerticalAlignment="Stretch">
                <Rectangle Name="WhRecentFocusThumbGlowL0"
                           IsHitTestVisible="False" Visibility="Collapsed"/>
                <Rectangle Name="WhRecentFocusThumbGlowL1"
                           IsHitTestVisible="False" Visibility="Collapsed"/>
                <Border Name="WhRecentFocusThumbTitleBg"
                        IsHitTestVisible="False" Visibility="Collapsed"/>
                <Rectangle Name="WhRecentFocusThumbTitleBar"
                           IsHitTestVisible="False" Visibility="Collapsed"/>
            </Grid>
        )";
        host = Markup::XamlReader::Load(xaml).as<Controls::Grid>();
        panel.Children().Append(host);
    }

    // Critical: cover title+image rows, not just row 0 (title).
    SpanHostOverPanel(host, panel);

    host.ClearValue(FrameworkElement::WidthProperty());
    host.ClearValue(FrameworkElement::HeightProperty());
    host.ClearValue(FrameworkElement::MaxWidthProperty());
    host.ClearValue(FrameworkElement::MaxHeightProperty());
    host.MinWidth(0);
    host.MinHeight(0);
    host.Margin(Thickness{0, 0, 0, 0});
    host.HorizontalAlignment(HorizontalAlignment::Stretch);
    host.VerticalAlignment(VerticalAlignment::Stretch);
    host.IsHitTestVisible(false);
    ClearOurHostClip(host);
    BringElementToFront(panel, host);
    return host;
}

// Layout slot is width×height at (0,0); visual position is (x,y) via transform
// so measure does not include the offset (avoids expanding the title row).
void PlaceOverlayChild(FrameworkElement el,
                       double x,
                       double y,
                       double width,
                       double height) {
    if (!el) {
        return;
    }
    try {
        el.HorizontalAlignment(HorizontalAlignment::Left);
        el.VerticalAlignment(VerticalAlignment::Top);
        el.Margin(Thickness{0, 0, 0, 0});
        if (width > 0) {
            el.Width(width);
        } else {
            el.ClearValue(FrameworkElement::WidthProperty());
        }
        if (height > 0) {
            el.Height(height);
        } else {
            el.ClearValue(FrameworkElement::HeightProperty());
        }
        Media::TranslateTransform tf;
        tf.X(x);
        tf.Y(y);
        el.RenderTransform(tf);
        el.Visibility(Visibility::Visible);
    } catch (...) {
    }
}

void HideThumbOverlayChildren(Controls::Grid host) {
    if (!host) {
        return;
    }
    for (auto name : {kThumbGlowLayerNames[0], kThumbGlowLayerNames[1],
                      kThumbTitleBgName, kThumbTitleBarName}) {
        if (auto el = FindChildByName(host, name)) {
            try {
                el.Visibility(Visibility::Collapsed);
                el.ClearValue(UIElement::RenderTransformProperty());
                if (auto r = el.try_as<Shapes::Rectangle>()) {
                    r.Stroke(nullptr);
                    r.Fill(Media::SolidColorBrush{
                        winrt::Windows::UI::Color{0, 0, 0, 0}});
                }
                if (auto b = el.try_as<Controls::Border>()) {
                    b.Background(nullptr);
                }
                el.ClearValue(FrameworkElement::WidthProperty());
                el.ClearValue(FrameworkElement::HeightProperty());
                el.ClearValue(FrameworkElement::MarginProperty());
            } catch (...) {
            }
        }
    }
}

// Title bottom Y relative to `relativeTo` (prefer overlay host).
// Returns false if transform looks unusable (layout not ready / wrong ancestor).
bool GetTitleBottomRelative(FrameworkElement title,
                            FrameworkElement relativeTo,
                            double cardH,
                            double& outTop,
                            double& outBottom) {
    outTop = outBottom = 0;
    if (!title || !relativeTo) {
        return false;
    }
    try {
        try {
            title.UpdateLayout();
        } catch (...) {
        }
        double th = title.ActualHeight();
        if (!(th > 1.0)) {
            th = title.DesiredSize().Height;
        }
        if (!(th > 1.0)) {
            return false;  // not laid out yet — caller should defer
        }
        auto xform = title.TransformToVisual(relativeTo);
        auto topLeft = xform.TransformPoint(
            winrt::Windows::Foundation::Point{0.f, 0.f});
        auto bottomLeft = xform.TransformPoint(
            winrt::Windows::Foundation::Point{0.f, static_cast<float>(th)});
        // Reject nonsense (wrong ancestor / mid-layout).
        if (topLeft.Y < -5.0f || topLeft.Y > cardH * 0.55) {
            return false;
        }
        if (bottomLeft.Y < topLeft.Y + 4.0f || bottomLeft.Y > cardH * 0.60) {
            return false;
        }
        outTop = static_cast<double>(topLeft.Y);
        outBottom = static_cast<double>(bottomLeft.Y);
        return true;
    } catch (...) {
        return false;
    }
}

// Deferred re-layout for titleBar/titleBg after the flyout finishes measuring.
// At most one nested pass (avoids infinite Low-priority loops when title never
// reports a size).
std::atomic<bool> g_thumbRelayoutPending{false};
std::atomic<int> g_thumbRelayoutDepth{0};

void ScheduleThumbnailRelayout(FrameworkElement thumbView) {
    if (!thumbView || g_thumbRelayoutDepth.load() > 0) {
        return;  // already inside the deferred pass
    }
    if (g_thumbRelayoutPending.exchange(true)) {
        return;
    }
    try {
        auto dispatcher = thumbView.Dispatcher();
        if (!dispatcher) {
            g_thumbRelayoutPending = false;
            return;
        }
        winrt::weak_ref<FrameworkElement> weak =
            winrt::make_weak(thumbView);
        // Low priority so it runs after the current layout pass.
        dispatcher.RunAsync(
            winrt::Windows::UI::Core::CoreDispatcherPriority::Low,
            [weak]() {
                g_thumbRelayoutPending = false;
                try {
                    FrameworkElement el = weak.get();
                    if (!el || g_unloading.load()) {
                        return;
                    }
                    g_thumbRelayoutDepth = 1;
                    RefreshThumbnailFlyout_UIThread(el);
                    g_thumbRelayoutDepth = 0;
                } catch (...) {
                    g_thumbRelayoutDepth = 0;
                }
            });
    } catch (...) {
        g_thumbRelayoutPending = false;
    }
}

void ApplyThumbnailHighlight(FrameworkElement thumbView, int rankOneBased) {
    if (!thumbView || rankOneBased <= 0 || g_unloading.load() ||
        !SettingsSnap()->enabled || !SettingsSnap()->previewHighlightEnabled) {
        ClearThumbnailHighlight(thumbView);
        return;
    }

    try {
        // Start clean so style switches don't leave mixed chrome.
        ClearThumbnailHighlight(thumbView);

        auto panel = GetThumbnailHostPanel(thumbView);
        if (!panel) {
            return;
        }
        auto panelFe = panel.as<FrameworkElement>();

        const int intensity = PreviewRankIntensity(rankOneBased - 1);
        const double t = intensity / 100.0;
        winrt::Windows::UI::Color base = ResolveGlowBaseColor();
        auto withAlpha = [](winrt::Windows::UI::Color c, int a) {
            c.A = static_cast<uint8_t>((std::max)(0, (std::min)(255, a)));
            return c;
        };

        const double thickness = static_cast<double>(
            (std::max)(2, (std::min)(8, SettingsSnap()->glowThickness)));
        const double roundnessFrac =
            (std::max)(0, (std::min)(50, SettingsSnap()->glowRoundness)) / 100.0;
        // Preview plate / titleBg use dedicated opacity (not taskbar fill).
        const int fillOpacitySetting =
            (std::max)(0, (std::min)(100, SettingsSnap()->previewFillOpacity));
        const PreviewStyle style = SettingsSnap()->previewStyle;
        // Hybrid: plate is the rank-1 “this one” signal; title wash is enough
        // for 2+ (whole-plate 50/5 looks like leftover hover, not a ladder).
        PreviewStyle paintStyle = style;
        if (style == PreviewStyle::PlateTitle) {
            paintStyle = (rankOneBased <= 1) ? PreviewStyle::Plate
                                             : PreviewStyle::TitleBg;
        }

        // Measure card BEFORE injecting any overlay (critical for layout).
        double cardW = 0, cardH = 0;
        try {
            cardW = panelFe.ActualWidth();
            cardH = panelFe.ActualHeight();
            if (!(cardW > 1.0)) {
                cardW = thumbView.ActualWidth();
            }
            if (!(cardH > 1.0)) {
                cardH = thumbView.ActualHeight();
            }
        } catch (...) {
        }
        const bool cardSizeFallback = !(cardW > 1.0) || !(cardH > 1.0);
        if (!(cardW > 1.0)) {
            cardW = 180.0;
        }
        if (!(cardH > 1.0)) {
            cardH = 120.0;
        }
        // 180×120 means layout not ready — bar placement will be wrong until
        // the deferred remeasure runs.
        if (cardSizeFallback) {
            ScheduleThumbnailRelayout(thumbView);
        }

        auto title = FindThumbnailTitleElement(thumbView);

        // Prefer BackgroundBorder size — more stable mid-layout.
        if (auto bb = FindThumbnailBackgroundBorder(thumbView)) {
            try {
                double bw = bb.ActualWidth();
                double bh = bb.ActualHeight();
                if (bw > 1.0) {
                    cardW = bw;
                }
                if (bh > 1.0) {
                    cardH = bh;
                }
            } catch (...) {
            }
        }

        // Plate prefers native BackgroundBorder (no layout child).
        if (paintStyle == PreviewStyle::Plate) {
            bool usedNative = false;
            if (auto borderEl = FindThumbnailBackgroundBorder(thumbView)) {
                try {
                    if (auto border = borderEl.try_as<Controls::Border>()) {
                        winrt::Windows::Foundation::IInspectable savedBg;
                        auto local = border.ReadLocalValue(
                            Controls::Border::BackgroundProperty());
                        if (local != DependencyProperty::UnsetValue()) {
                            savedBg = border.Background();
                        }
                        auto marker = MakeThumbNativeMarker(savedBg);
                        if (marker) {
                            const int fillA = static_cast<int>(
                                fillOpacitySetting * 2.55 *
                                (0.45 + 0.55 * t));
                            border.Background(Media::SolidColorBrush{
                                withAlpha(base, fillA)});
                            panel.Children().Append(marker);
                            usedNative = true;
                        }
                    }
                } catch (...) {
                }
            }
            if (!usedNative) {
                Controls::Grid host = EnsureThumbOverlayHost(panel);
                HideThumbOverlayChildren(host);
                if (auto plate = FindChildByName(host, kThumbGlowLayerNames[0])
                                     .try_as<Shapes::Rectangle>()) {
                    const int fillA = static_cast<int>(
                        fillOpacitySetting * 2.55 * (0.45 + 0.55 * t));
                    plate.Fill(Media::SolidColorBrush{withAlpha(base, fillA)});
                    plate.Stroke(nullptr);
                    plate.StrokeThickness(0);
                    plate.RadiusX(cardW * roundnessFrac * 0.12);
                    plate.RadiusY(cardH * roundnessFrac * 0.12);
                    plate.Opacity(0.85 + 0.15 * t);
                    PlaceOverlayChild(plate, 0, 0, cardW, cardH);
                }
            }

            if (SettingsSnap()->glowDebugLog) {
                Wh_Log(L"Preview glow rank %d style=%s paint=%s intensity=%d "
                       L"on \"%s\"",
                       rankOneBased, PreviewStyleName(style),
                       PreviewStyleName(paintStyle), intensity,
                       Automation::AutomationProperties::GetName(thumbView)
                           .c_str());
            }
            return;
        }

        Controls::Grid host = EnsureThumbOverlayHost(panel);
        HideThumbOverlayChildren(host);

        if (paintStyle == PreviewStyle::Ring) {
            const double inset = 3.0;
            const double corner =
                (std::max)(12.0, (std::min)(cardW, cardH) - 2.0 * inset) *
                roundnessFrac;

            for (int i = 0; i < 2; ++i) {
                auto rect = FindChildByName(host, kThumbGlowLayerNames[i])
                                .try_as<Shapes::Rectangle>();
                if (!rect) {
                    continue;
                }
                const double step = i == 0 ? 0.0 : 3.0;
                const double layerInset = inset + step;
                const int strokeA =
                    static_cast<int>((140 + 100 * t) * (1.0 - 0.2 * i));
                const double opacity = 0.75 + 0.25 * t;
                const double th =
                    (std::max)(1.5, thickness * (1.0 - 0.15 * i));
                const double size = (std::max)(
                    8.0, (std::min)(cardW, cardH) - 2.0 * layerInset);
                rect.Stroke(Media::SolidColorBrush{withAlpha(base, strokeA)});
                rect.StrokeThickness(th);
                rect.Fill(Media::SolidColorBrush{
                    winrt::Windows::UI::Color{0, 0, 0, 0}});
                rect.RadiusX(corner + step);
                rect.RadiusY(corner + step);
                rect.Opacity(opacity);
                PlaceOverlayChild(rect, layerInset, layerInset, size, size);
            }
        } else if (paintStyle == PreviewStyle::TitleBg) {
            double top = 4.0;
            double stripH = 28.0;
            double titleTop = 0, titleBottom = 0;
            bool laidOut = GetTitleBottomRelative(title, host, cardH, titleTop,
                                                  titleBottom);
            if (laidOut) {
                top = (std::max)(2.0, titleTop - 3.0);
                stripH = (std::max)(20.0, titleBottom - top + 4.0);
            } else {
                // Layout not ready — default strip + one deferred remeasure.
                ScheduleThumbnailRelayout(thumbView);
            }
            const double hPad = 8.0;
            const double stripW = (std::max)(24.0, cardW - 2.0 * hPad);

            auto chip =
                FindChildByName(host, kThumbTitleBgName).try_as<Controls::Border>();
            if (chip) {
                // Soft wash above title glyphs. Rank-1 alpha follows tint
                // opacity; lower ranks scale linearly with intensity. The old
                // (0.55+0.45*t) floor made 100 vs 5 look almost the same.
                const int maxA = (std::max)(
                    16, (std::min)(140, static_cast<int>(
                                            14 + fillOpacitySetting * 1.15)));
                const int chipA = (std::max)(8, static_cast<int>(maxA * t));
                chip.Background(Media::SolidColorBrush{withAlpha(base, chipA)});
                chip.CornerRadius(CornerRadius{stripH * 0.35});
                chip.Opacity(1.0);
                PlaceOverlayChild(chip, hPad, top, stripW, stripH);
            }
        } else {  // TitleBar — underline just under the title glyphs
            // Prefer transform relative to host (same coordinate space as
            // PlaceOverlayChild). Small gap under baseline — enough to avoid
            // strikethrough, not so much that the bar hugs the thumbnail image.
            constexpr double kGapBelowTitle = 2.0;
            double top = 30.0;  // fallback under a ~28px header
            double titleTop = 0, titleBottom = 0;
            bool laidOut = GetTitleBottomRelative(title, host, cardH, titleTop,
                                                  titleBottom);
            if (laidOut) {
                top = titleBottom + kGapBelowTitle;
            } else {
                ScheduleThumbnailRelayout(thumbView);
            }
            top = (std::max)(18.0, (std::min)(top, cardH * 0.38));

            const double barH =
                (std::max)(2.0, (std::min)(5.0, thickness + 0.5));
            const double hPad = 10.0;
            const double barW = (std::max)(24.0, cardW - 2.0 * hPad);

            auto bar = FindChildByName(host, kThumbTitleBarName)
                           .try_as<Shapes::Rectangle>();
            if (bar) {
                // Wider alpha range than the old rank-1-only bar so flyout
                // ranks read as a ladder; t=1 stays fully opaque accent.
                const int fillA = static_cast<int>(90 + 165 * t);
                bar.Fill(Media::SolidColorBrush{withAlpha(base, fillA)});
                bar.Stroke(nullptr);
                bar.StrokeThickness(0);
                bar.RadiusX(barH * 0.5);
                bar.RadiusY(barH * 0.5);
                bar.Opacity(0.50 + 0.50 * t);
                PlaceOverlayChild(bar, hPad, top, barW, barH);
            }
        }

        if (SettingsSnap()->glowDebugLog) {
            Wh_Log(L"Preview glow rank %d style=%s paint=%s intensity=%d "
                   L"card=%.0fx%.0f on \"%s\"",
                   rankOneBased, PreviewStyleName(style),
                   PreviewStyleName(paintStyle), intensity, cardW, cardH,
                   Automation::AutomationProperties::GetName(thumbView)
                       .c_str());
        }
    } catch (...) {
        HRESULT hr = winrt::to_hresult();
        Wh_Log(L"ApplyThumbnailHighlight error %08X", hr);
    }
}

void RefreshThumbnailFlyout_UIThread(FrameworkElement anyThumb) {
    if (!anyThumb) {
        return;
    }
    TrackThumbView_UIThread(anyThumb);

    auto allViews = CollectSiblingThumbnailViews(anyThumb);
    if (!SettingsSnap()->enabled || !SettingsSnap()->previewHighlightEnabled ||
        g_unloading.load()) {
        for (auto& s : allViews) {
            ClearThumbnailHighlight(s);
        }
        return;
    }

    // Snap-group cards sit in the same ItemsRepeater as the windows
    // (UWPSpy: PositionInSet 1/4 = "Group | Lister - [file] and 1 other
    // window"). Never glow those; they are not a window HWND.
    std::vector<FrameworkElement> siblings;
    siblings.reserve(allViews.size());
    for (auto& v : allViews) {
        if (IsSnapGroupThumbnailView(v)) {
            ClearThumbnailHighlight(v);
        } else {
            siblings.push_back(v);
        }
    }

    // Product rule: only multi-window flyouts (group card does not count).
    if (siblings.size() <= 1) {
        for (auto& s : siblings) {
            ClearThumbnailHighlight(s);
        }
        return;
    }

    SortThumbnailViewsVisualOrder(siblings);
    const std::vector<std::wstring> cardTitles = PickFlyoutCardTitles(siblings);

    // HWND pipeline (this flyout only — not a global window ladder):
    //   1. TaskItem — DataContext ↔ ctor map (COM identity).
    //   2. Group-order — live mappings for this hover, not the previous one.
    //      Dropped when card titles are distinct (index ≠ visual after a click).
    //   3. Title unique — DisplayNameTextBlock when texts differ; each HWND once.
    // Then sort by recency tick / confirmSeq and paint top N.
    enum class ResolveHow : int { None = 0, TaskItem, GroupOrder, Title };
    struct Scored {
        FrameworkElement view{nullptr};
        HWND hwnd = nullptr;
        ULONGLONG tick = 0;
        ResolveHow how = ResolveHow::None;
        int rank = 0;  // 1-based flyout rank; 0 = not highlighted
    };
    std::vector<Scored> scored(siblings.size());
    std::unordered_set<HWND> usedHwnds;
    void* sharedGroup = nullptr;

    auto recent = CopyRecentWindowsForPreview();

    auto tickFor = [](HWND hwnd) -> ULONGLONG {
        ULONGLONG tick = 0;
        if (!hwnd) {
            return 0;
        }
        std::lock_guard<std::mutex> lock(g_stateMutex);
        IsWindowRecentForPreviewLocked(CurrentDeskLocked(), hwnd, &tick);
        return tick;
    };
    auto seqFor = [](HWND hwnd) -> ULONGLONG {
        if (!hwnd) {
            return 0;
        }
        std::lock_guard<std::mutex> lock(g_stateMutex);
        auto& map = CurrentDeskLocked().windowFocusMap;
        auto it = map.find(hwnd);
        if (it == map.end()) {
            return 0;
        }
        return it->second.confirmSeq;
    };

    // Pass 1: strong identity (TaskItemThumbnail model → HWND). Never use bare
    // title here — two Calibre windows with the same file name would both bind
    // to the same HWND.
    for (size_t i = 0; i < siblings.size(); ++i) {
        TrackThumbView_UIThread(siblings[i]);
        scored[i].view = siblings[i];
        void* group = nullptr;
        HWND hwnd = ResolveHwndForThumbnailView(siblings[i], &group);
        if (group && !sharedGroup) {
            sharedGroup = group;
        }
        if (hwnd && IsWindow(hwnd) && !usedHwnds.count(hwnd)) {
            scored[i].hwnd = hwnd;
            scored[i].tick = tickFor(hwnd);
            scored[i].how = ResolveHow::TaskItem;
            usedHwnds.insert(hwnd);
        }
    }

    // Pass 1b: fill from TaskItem maps by group / construction order.
    // DataContext often does NOT match our ctor IInspectable (logs showed
    // "2 taskitem maps" but how=title/none) — so discover the group even
    // when pass 1 resolved nothing.
    if (usedHwnds.size() < siblings.size()) {
        void* group = sharedGroup;
        if (!group) {
            std::lock_guard<std::mutex> lock(g_thumbnailMapMutex);
            for (const auto& item : g_thumbnailTaskItemMapping) {
                HWND h = HwndFromMappingEntry(item);
                if (h && usedHwnds.count(h) && item.taskGroup) {
                    group = item.taskGroup;
                    break;
                }
            }
        }
        if (!group) {
            group = FindTaskGroupForSiblingCount(siblings.size());
        }

        std::vector<HWND> groupHwnds = HwndsForTaskGroupInOrder(group);
        // Fallback: last N mapped HWNDs (just-created flyout models).
        if (groupHwnds.size() != siblings.size()) {
            auto last = LastMappedHwnds(siblings.size());
            if (last.size() == siblings.size()) {
                groupHwnds = std::move(last);
            }
        }
        // Do NOT EnumWindows-expand here and then assign by index.
        // Z-order (focused window first) is not left-to-right flyout order —
        // that pinned the recent Lister HWND onto card 0 every time.

        if (groupHwnds.size() == siblings.size()) {
            // Prefer map order for ALL siblings when counts match — more
            // reliable than leaving a half-filled title match in place.
            // Only overwrite title-resolved slots when they have no taskitem id.
            bool anyTaskItem = false;
            for (const auto& s : scored) {
                if (s.how == ResolveHow::TaskItem) {
                    anyTaskItem = true;
                    break;
                }
            }
            if (!anyTaskItem) {
                usedHwnds.clear();
                for (size_t i = 0; i < siblings.size(); ++i) {
                    HWND h = groupHwnds[i];
                    scored[i].hwnd = h;
                    scored[i].tick = tickFor(h);
                    scored[i].how = ResolveHow::GroupOrder;
                    if (h) {
                        usedHwnds.insert(h);
                    }
                }
            } else {
                for (size_t i = 0; i < siblings.size(); ++i) {
                    if (scored[i].hwnd) {
                        continue;
                    }
                    HWND h = groupHwnds[i];
                    if (!h || usedHwnds.count(h)) {
                        continue;
                    }
                    scored[i].hwnd = h;
                    scored[i].tick = tickFor(h);
                    scored[i].how = ResolveHow::GroupOrder;
                    usedHwnds.insert(h);
                }
            }
        } else if (!groupHwnds.empty()) {
            size_t gi = 0;
            for (size_t i = 0; i < siblings.size() && gi < groupHwnds.size();
                 ++i) {
                if (scored[i].hwnd) {
                    continue;
                }
                while (gi < groupHwnds.size() &&
                       usedHwnds.count(groupHwnds[gi])) {
                    ++gi;
                }
                if (gi >= groupHwnds.size()) {
                    break;
                }
                HWND h = groupHwnds[gi++];
                scored[i].hwnd = h;
                scored[i].tick = tickFor(h);
                scored[i].how = ResolveHow::GroupOrder;
                usedHwnds.insert(h);
            }
        }
    }

    // Title pool: recency map plus live same-class windows (TLister a/b/c).
    // Enumerated HWNDs are candidates only — never assigned by index.
    std::vector<HWND> titleSeeds;
    for (const auto& r : recent) {
        if (r.hwnd) {
            titleSeeds.push_back(r.hwnd);
        }
    }
    for (const auto& s : scored) {
        if (s.hwnd) {
            titleSeeds.push_back(s.hwnd);
        }
    }
    std::vector<WindowFocusInfo> titlePool = recent;
    {
        std::unordered_set<HWND> have;
        for (const auto& r : titlePool) {
            if (r.hwnd) {
                have.insert(r.hwnd);
            }
        }
        for (HWND h : ExpandSameClassWindows(titleSeeds)) {
            if (!h || have.count(h)) {
                continue;
            }
            WindowFocusInfo extra;
            extra.hwnd = h;
            extra.windowTitle = GetWindowTitle(h);
            extra.lastConfirmedTick = tickFor(h);
            titlePool.push_back(std::move(extra));
            have.insert(h);
        }
    }

    const bool titlesDistinct = TitleKeysAreDistinct(cardTitles);

    auto hwndTitle = [](HWND hwnd) -> std::wstring {
        return hwnd && IsWindow(hwnd) ? GetWindowTitle(hwnd) : std::wstring{};
    };

    // Unique titles beat index assignment. Also drop a TaskItem HWND that
    // clearly belongs to a *different* card (ebook reader: DataContext /
    // stale map points at the other book in the same process).
    if (titlesDistinct) {
        for (size_t i = 0; i < scored.size(); ++i) {
            if (!scored[i].hwnd) {
                continue;
            }
            const bool dropGroup = scored[i].how == ResolveHow::GroupOrder;
            int selfScore = ScoreTitleToAutomationName(hwndTitle(scored[i].hwnd),
                                                       cardTitles[i]);
            int bestOther = 0;
            for (size_t j = 0; j < cardTitles.size(); ++j) {
                if (j == i) {
                    continue;
                }
                bestOther = (std::max)(
                    bestOther, ScoreTitleToAutomationName(
                                   hwndTitle(scored[i].hwnd), cardTitles[j]));
            }
            const bool belongsElsewhere =
                bestOther >= 70 && bestOther > selfScore;
            if (dropGroup || belongsElsewhere) {
                usedHwnds.erase(scored[i].hwnd);
                scored[i].hwnd = nullptr;
                scored[i].tick = 0;
                scored[i].how = ResolveHow::None;
            }
        }
    }

    // Pass 2: title fallback with unique HWND assignment only.
    for (size_t i = 0; i < siblings.size(); ++i) {
        if (scored[i].hwnd) {
            continue;
        }
        std::wstring autoName = cardTitles[i];
        HWND h = MatchTitleToUnusedRecent(autoName, titlePool, usedHwnds);
        if (h) {
            scored[i].hwnd = h;
            scored[i].tick = tickFor(h);
            if (scored[i].tick == 0) {
                // Live window not yet in recency map — still use its title
                // tick from the pool entry if we recorded one.
                for (const auto& info : titlePool) {
                    if (info.hwnd == h && info.lastConfirmedTick > 0) {
                        scored[i].tick = info.lastConfirmedTick;
                        break;
                    }
                }
            }
            scored[i].how = ResolveHow::Title;
            usedHwnds.insert(h);
        }
    }

    // Pass 3: ITaskItem HWND and EVENT_SYSTEM_FOREGROUND HWND can differ
    // (owned Lister windows, tab proxies). Copy recency from a same-PID
    // recent window when the card's HWND itself has tick 0.
    for (size_t i = 0; i < scored.size(); ++i) {
        if (!scored[i].hwnd || scored[i].tick > 0) {
            continue;
        }
        DWORD cardPid = 0;
        GetWindowThreadProcessId(scored[i].hwnd, &cardPid);
        if (!cardPid) {
            continue;
        }

        std::wstring autoName = cardTitles[i];

        int bestScore = 0;
        ULONGLONG bestTick = 0;
        for (const auto& info : recent) {
            if (!info.hwnd) {
                continue;
            }
            DWORD rpid = 0;
            GetWindowThreadProcessId(info.hwnd, &rpid);
            if (rpid != cardPid) {
                continue;
            }
            int s = 0;
            if (!autoName.empty()) {
                s = ScoreTitleToAutomationName(info.windowTitle, autoName);
                if (s < 70) {
                    s = (std::max)(
                        s, ScoreTitleToAutomationName(GetWindowTitle(info.hwnd),
                                                      autoName));
                }
            }
            if (s > bestScore) {
                bestScore = s;
                bestTick = info.lastConfirmedTick;
            }
        }
        // Require a unique title/path (96+). Score 85 prefix would copy the
        // latest Lister tick onto a.txt AND c.txt, then card 0 always wins.
        if (bestScore >= 96) {
            scored[i].tick = bestTick;
        }
    }

    // Rank this flyout only (not a global window ladder). Sort siblings that
    // have a recency tick; ties: confirmSeq, then the live foreground HWND.
    HWND foreground = GetForegroundWindow();
    std::vector<size_t> order;
    order.reserve(scored.size());
    for (size_t i = 0; i < scored.size(); ++i) {
        if (scored[i].hwnd && scored[i].tick > 0) {
            order.push_back(i);
        }
    }
    std::sort(order.begin(), order.end(), [&](size_t a, size_t b) {
        if (scored[a].tick != scored[b].tick) {
            return scored[a].tick > scored[b].tick;
        }
        const ULONGLONG sa = seqFor(scored[a].hwnd);
        const ULONGLONG sb = seqFor(scored[b].hwnd);
        if (sa != sb) {
            return sa > sb;
        }
        const bool aFg = scored[a].hwnd == foreground;
        const bool bFg = scored[b].hwnd == foreground;
        if (aFg != bFg) {
            return aFg;
        }
        return a < b;
    });

    const int limit = (std::max)(0, SettingsSnap()->previewHighlightCount);
    for (size_t r = 0; r < order.size() && static_cast<int>(r) < limit; ++r) {
        scored[order[r]].rank = static_cast<int>(r) + 1;
    }

    if (SettingsSnap()->glowDebugLog) {
        size_t mapCount = 0;
        {
            std::lock_guard<std::mutex> lock(g_thumbnailMapMutex);
            mapCount = g_thumbnailTaskItemMapping.size();
        }
        const size_t painted =
            (std::min)(order.size(), static_cast<size_t>(limit));
        Wh_Log(L"Preview resolve: %zu siblings, %zu taskitem maps, "
               L"highlightCount=%d candidates=%zu painted=%zu",
               siblings.size(), mapCount, limit, order.size(), painted);
        for (size_t i = 0; i < scored.size(); ++i) {
            PCWSTR how = L"none";
            switch (scored[i].how) {
                case ResolveHow::TaskItem:
                    how = L"taskitem";
                    break;
                case ResolveHow::GroupOrder:
                    how = L"group-order";
                    break;
                case ResolveHow::Title:
                    how = L"title";
                    break;
                default:
                    break;
            }
            std::wstring name;
            try {
                name = Automation::AutomationProperties::GetName(scored[i].view)
                           .c_str();
            } catch (...) {
            }
            Wh_Log(L"  sibling[%zu]: hwnd=%p tick=%llu how=%s rank=%d%s "
                   L"name=\"%s\" card=\"%s\"",
                   i, scored[i].hwnd,
                   static_cast<unsigned long long>(scored[i].tick), how,
                   scored[i].rank,
                   scored[i].rank > 0 ? L" [GLOW]" : L"", name.c_str(),
                   (i < cardTitles.size()) ? cardTitles[i].c_str() : L"");
        }
    }

    for (size_t i = 0; i < scored.size(); ++i) {
        if (scored[i].rank > 0) {
            ApplyThumbnailHighlight(scored[i].view, scored[i].rank);
        } else {
            ClearThumbnailHighlight(scored[i].view);
        }
    }
}

void ClearAllThumbnailHighlights_UIThread() {
    std::vector<winrt::weak_ref<FrameworkElement>> thumbs;
    {
        std::lock_guard<std::mutex> lock(g_thumbViewsMutex);
        thumbs = g_trackedThumbViews;
    }
    ForEachLiveElementOnThisDispatcher(
        thumbs, [](FrameworkElement el) { ClearThumbnailHighlight(el); });
}

void RequestApplyPreviewVisuals() {
    if (g_unloading.load()) {
        return;
    }
    // Same as RequestApplyVisuals: if no dispatcher yet, wait for the first
    // TaskListButton / thumbnail OnApplyTemplate. Do not PostMessage to this
    // focus thread — the WM_APP_REQUEST_PREVIEW_APPLY handler used to call
    // this again, which spun at 100% CPU and starved WM_TIMER.
    RunOnUiThread([]() {
        std::vector<winrt::weak_ref<FrameworkElement>> thumbs;
        {
            std::lock_guard<std::mutex> lock(g_thumbViewsMutex);
            thumbs = g_trackedThumbViews;
        }
        ForEachLiveElementOnThisDispatcher(thumbs, [](FrameworkElement el) {
            RefreshThumbnailFlyout_UIThread(el);
        });
    });
}

// Expected at startup before any TaskListButton is seen — log once, not per call.
std::atomic<bool> g_loggedNoDispatcherAnchor{false};

std::vector<winrt::Windows::UI::Core::CoreDispatcher> CollectUiDispatchers() {
    std::vector<winrt::Windows::UI::Core::CoreDispatcher> dispatchers;
    auto addDispatcher = [&](FrameworkElement el) {
        if (!el) {
            return;
        }
        try {
            auto dispatcher = el.Dispatcher();
            if (!dispatcher) {
                return;
            }
            for (const auto& existing : dispatchers) {
                if (existing == dispatcher) {
                    return;
                }
            }
            dispatchers.push_back(dispatcher);
        } catch (...) {
        }
    };

    {
        std::lock_guard<std::mutex> lock(g_buttonsMutex);
        try {
            addDispatcher(g_dispatcherAnchor.get());
        } catch (...) {
        }
        for (auto& weak : g_trackedButtons) {
            try {
                addDispatcher(weak.get());
            } catch (...) {
            }
        }
    }
    {
        std::lock_guard<std::mutex> lock(g_thumbViewsMutex);
        for (auto& weak : g_trackedThumbViews) {
            try {
                addDispatcher(weak.get());
            } catch (...) {
            }
        }
    }
    std::vector<winrt::weak_ref<FrameworkElement>> layoutPanels;
    {
        std::lock_guard<std::mutex> lock(g_layoutWatchMutex);
        for (auto& w : g_layoutWatches) {
            layoutPanels.push_back(w.panel);
        }
    }
    for (auto& weak : layoutPanels) {
        try {
            addDispatcher(weak.get());
        } catch (...) {
        }
    }
    return dispatchers;
}

// Uninit: run handler at High, then wait for a Low sentinel so earlier
// Normal TryRunAsync work (which no-ops on g_unloading) has drained.
bool RunOnEachUiDispatcherAndWait(
    const winrt::Windows::UI::Core::DispatchedHandler& handler,
    DWORD timeoutMs) {
    auto dispatchers = CollectUiDispatchers();
    if (dispatchers.empty()) {
        Wh_Log(L"UI cleanup: no dispatcher");
        return true;
    }
    bool allOk = true;
    for (auto& dispatcher : dispatchers) {
        try {
            if (dispatcher.HasThreadAccess()) {
                handler();
                continue;
            }
            HANDLE done = CreateEventW(nullptr, TRUE, FALSE, nullptr);
            if (!done) {
                allOk = false;
                continue;
            }
            bool posted = false;
            bool drainPosted = false;
            try {
                posted = static_cast<bool>(dispatcher.TryRunAsync(
                    winrt::Windows::UI::Core::CoreDispatcherPriority::High,
                    handler));
                drainPosted = static_cast<bool>(dispatcher.TryRunAsync(
                    winrt::Windows::UI::Core::CoreDispatcherPriority::Low,
                    [done]() { SetEvent(done); }));
            } catch (...) {
            }
            if (!posted) {
                allOk = false;
            }
            if (!drainPosted) {
                SetEvent(done);
                allOk = false;
            }
            const DWORD w = WaitForSingleObject(done, timeoutMs);
            if (w == WAIT_OBJECT_0) {
                CloseHandle(done);
            } else {
                // Low sentinel may still run — do not CloseHandle.
                Wh_Log(L"ERROR: UI dispatcher cleanup timed out (%ums)",
                       timeoutMs);
                allOk = false;
            }
        } catch (...) {
            allOk = false;
            Wh_Log(L"ERROR: UI dispatcher cleanup failed");
        }
    }
    return allOk;
}

bool RunOnUiThread(const winrt::Windows::UI::Core::DispatchedHandler& handler) {
    auto dispatchers = CollectUiDispatchers();

    if (dispatchers.empty()) {
        // App + preview apply both hit this before the first button hook.
        if (!g_loggedNoDispatcherAnchor.exchange(true)) {
            Wh_Log(L"RunOnUiThread: no dispatcher anchor yet (no buttons seen) "
                   L"— will apply on first TaskListButton");
        }
        return false;
    }

    bool any = false;
    for (auto& dispatcher : dispatchers) {
        try {
            if (dispatcher.HasThreadAccess()) {
                handler();
                any = true;
                continue;
            }
            if (dispatcher.TryRunAsync(
                    winrt::Windows::UI::Core::CoreDispatcherPriority::Normal,
                    handler)) {
                any = true;
            }
        } catch (...) {
            HRESULT hr = winrt::to_hresult();
            Wh_Log(L"RunOnUiThread error %08X", hr);
        }
    }
    return any;
}

void RequestApplyVisuals() {
    // Prefer UI-thread apply; also keep logging from caller context.
    if (!RunOnUiThread([]() { ApplyAllHighlights_UIThread(); })) {
        // Buttons not tracked yet — will apply on next UpdateVisualStates.
        // Avoid rank dump spam at startup (preview + app both request apply).
        if (SettingsSnap()->glowDebugLog) {
            std::lock_guard<std::mutex> lock(g_stateMutex);
            const auto& ranks = CurrentDeskLocked().rankedApps;
            Wh_Log(L"Ranks ready (%zu) — waiting for TaskListButton hooks",
                   ranks.size());
            for (size_t i = 0; i < ranks.size(); i++) {
                Wh_Log(L"  Rank %zu: %s", i + 1, ranks[i].displayName.c_str());
            }
        }
    }
}

void ApplyVisualHighlights() {
    // Legacy entry used by timers / focus path.
    RequestApplyVisuals();
}

// ---------------------------------------------------------------------------
// Taskbar.View.dll hooks
// ---------------------------------------------------------------------------

using TaskListButton_UpdateVisualStates_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateVisualStates_t TaskListButton_UpdateVisualStates_Original;

FrameworkElement TaskListButtonElementFromThis(void* pThis) {
    FrameworkElement element = nullptr;
    // implementation* layout: IUnknown at pThis+3 (same as other taskbar mods).
    ((IUnknown*)pThis + 3)
        ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                         winrt::put_abi(element));
    return element;
}

// Re-paint one button from the last full bind. Does not re-score identity —
// hover UpdateVisualStates would otherwise O(buttons×ranks) every mouse-over.
void RefreshButtonHighlight(FrameworkElement button) {
    if (!button || g_unloading.load()) {
        return;
    }

    if (g_pendingOverlaySweep.load() || !SettingsSnap()->enabled) {
        ClearButtonHighlight(button);
        return;
    }

    int rank = GetCachedPaintRank(button);
    if (rank < 0) {
        std::vector<AppFocusInfo> ranks;
        {
            std::lock_guard<std::mutex> lock(g_stateMutex);
            ranks = CurrentDeskLocked().rankedApps;
        }
        if (ranks.empty()) {
            SetCachedPaintRank(button, 0);
            ClearButtonHighlight(button);
            return;
        }
        if (TaskListButton_IsRunning(button) && IsVisualStateActive(button)) {
            StoreAutomationNameIfFits(ranks[0], GetButtonAutomationName(button));
        }
        rank = FindRankForButton(button, ranks, /*requireRunning=*/true);
        SetCachedPaintRank(button, rank);
    }

    if (rank > 0) {
        ApplyButtonHighlight(button, rank);
    } else {
        ClearButtonHighlight(button);
    }
}

// Full identity rebind. Hover UVS paints the cached rank immediately; this
// pass restores siblings whose visuals Windows reset without another UVS.
// Per dispatcher so primary and secondary taskbars do not throttle each other.
void ScheduleRefreshAllHighlights(FrameworkElement dispatcherAnchor) {
    if (!dispatcherAnchor) {
        return;
    }
    try {
        winrt::weak_ref<FrameworkElement> weak =
            winrt::make_weak(dispatcherAnchor);
        dispatcherAnchor.Dispatcher().TryRunAsync(
            winrt::Windows::UI::Core::CoreDispatcherPriority::Low,
            [weak]() {
                try {
                    const ULONGLONG now = GetTickCount64();
                    if (now - g_lastFullRefreshTick < kFullRebindDebounceMs &&
                        g_lastFullRefreshTick != 0) {
                        // Coalesce a trailing full bind after the hover storm
                        // so siblings Windows reset without UVS get restored.
                        if (g_hookThreadHwnd) {
                            SetTimer(g_hookThreadHwnd, kFullRebindTimerId,
                                     static_cast<UINT>(kFullRebindDebounceMs),
                                     nullptr);
                        }
                        return;
                    }
                    g_lastFullRefreshTick = now;
                    if (!weak.get()) {
                        return;
                    }
                    ApplyAllHighlights_UIThread();
                } catch (...) {
                }
            });
    } catch (...) {
    }
}

void WINAPI TaskListButton_UpdateVisualStates_Hook(void* pThis) {
    TaskListButton_UpdateVisualStates_Original(pThis);

    if (g_unloading.load()) {
        return;
    }

    FrameworkElement button = TaskListButtonElementFromThis(pThis);
    if (!button) {
        return;
    }

    try {
        if (button.Name() != L"TaskListButton") {
            return;
        }
    } catch (...) {
        return;
    }

    TrackButton_UIThread(button);
    RefreshButtonHighlight(button);
    ScheduleRefreshAllHighlights(button);
}

// XAML thumbnail view template — apply preview glow when flyout builds items.
using TaskItemThumbnailView_OnApplyTemplate_t = void(WINAPI*)(void* pThis);
TaskItemThumbnailView_OnApplyTemplate_t
    TaskItemThumbnailView_OnApplyTemplate_Original;
void WINAPI TaskItemThumbnailView_OnApplyTemplate_Hook(void* pThis) {
    if (TaskItemThumbnailView_OnApplyTemplate_Original) {
        TaskItemThumbnailView_OnApplyTemplate_Original(pThis);
    }
    if (g_unloading.load() || !SettingsSnap()->enabled ||
        !SettingsSnap()->previewHighlightEnabled) {
        return;
    }
    try {
        IUnknown* unknownPtr = *((IUnknown**)pThis + 1);
        if (!unknownPtr) {
            return;
        }
        FrameworkElement element = nullptr;
        unknownPtr->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                   winrt::put_abi(element));
        if (element) {
            RefreshThumbnailFlyout_UIThread(element);
        }
    } catch (...) {
        HRESULT hr = winrt::to_hresult();
        Wh_Log(L"TaskItemThumbnailView_OnApplyTemplate error %08X", hr);
    }
}

// Option C: re-resolve button → path on click.
using TaskListButton_OnPointerPressed_t = int(WINAPI*)(void* pThis, void* pArgs);
TaskListButton_OnPointerPressed_t TaskListButton_OnPointerPressed_Original;
int WINAPI TaskListButton_OnPointerPressed_Hook(void* pThis, void* pArgs) {
    int ret = TaskListButton_OnPointerPressed_Original
                  ? TaskListButton_OnPointerPressed_Original(pThis, pArgs)
                  : 0;
    if (g_unloading.load() || !g_taskbandResolveReady.load()) {
        return ret;
    }
    try {
        UIElement element = nullptr;
        ((IUnknown*)pThis)
            ->QueryInterface(winrt::guid_of<UIElement>(),
                             winrt::put_abi(element));
        if (!element) {
            return ret;
        }
        if (winrt::get_class_name(element) != L"Taskbar.TaskListButton") {
            return ret;
        }
        auto button = element.try_as<FrameworkElement>();
        if (button) {
            EnsureButtonPathCached(button, /*force=*/true);
        }
    } catch (...) {
    }
    return ret;
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Taskbar::ITaskListButton>::get_IsRunning(bool *))"},
            &TaskListButton_get_IsRunning_Original,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))"},
            &TaskListButton_UpdateVisualStates_Original,
            TaskListButton_UpdateVisualStates_Hook,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerPressed(void *))"},
            &TaskListButton_OnPointerPressed_Original,
            TaskListButton_OnPointerPressed_Hook,
        },
        {
            {LR"(struct winrt::Taskbar::TaskListWindowViewModel __cdecl TryGetItemFromContainer<struct winrt::Taskbar::TaskListWindowViewModel>(struct winrt::Windows::UI::Xaml::UIElement const &))"},
            &TryGetItemFromContainer_TaskListWindowViewModel_Original,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListWindowViewModel,struct winrt::Taskbar::ITaskListWindowViewModel>::get_TaskItem(void * *))"},
            &TaskListWindowViewModel_get_TaskItem_Original,
        },
        {
            {LR"(struct winrt::Taskbar::TaskListGroupViewModel __cdecl TryGetItemFromContainer<struct winrt::Taskbar::TaskListGroupViewModel>(struct winrt::Windows::UI::Xaml::UIElement const &))"},
            &TryGetItemFromContainer_TaskListGroupViewModel_Original,
        },
        {
            {LR"(public: bool __cdecl winrt::Taskbar::implementation::TaskListGroupViewModel::IsMultiWindow(void)const )"},
            &TaskListGroupViewModel_IsMultiWindow_Original,
        },
        {
            {LR"(public: __cdecl winrt::impl::consume_WindowsUdk_UI_Shell_ITaskGroup<struct winrt::WindowsUdk::UI::Shell::ITaskGroup>::IsRunning(void)const )"},
            &ITaskGroup_IsRunning_Original,
            ITaskGroup_IsRunning_Hook,
        },
        {
            // Optional: XAML thumbnail flyout (Win11). Fail soft if missing.
            {LR"(public: void __cdecl winrt::Taskbar::implementation::TaskItemThumbnailView::OnApplyTemplate(void))"},
            &TaskItemThumbnailView_OnApplyTemplate_Original,
            TaskItemThumbnailView_OnApplyTemplate_Hook,
            true,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed for Taskbar.View.dll");
        return false;
    }

    if (TaskItemThumbnailView_OnApplyTemplate_Original) {
        Wh_Log(L"Hooked Taskbar.View.dll symbols (identity + paint + thumbnails)");
    } else {
        Wh_Log(L"Hooked Taskbar.View.dll symbols (identity + paint; "
               L"thumbnail OnApplyTemplate unavailable)");
    }
    return true;
}

// Capture TaskItemThumbnail model → ITaskItem for HWND resolve (optional).
using TaskItemThumbnail_TaskItemThumbnail_t = void*(WINAPI*)(void* param1,
                                                             void* param2,
                                                             void* taskGroup,
                                                             void* taskItem,
                                                             void* taskListUi,
                                                             void* param6,
                                                             void* param7,
                                                             bool param8);
TaskItemThumbnail_TaskItemThumbnail_t
    TaskItemThumbnail_TaskItemThumbnail_Original;
void* WINAPI TaskItemThumbnail_TaskItemThumbnail_Hook(void* param1,
                                                      void* param2,
                                                      void* taskGroup,
                                                      void* taskItem,
                                                      void* taskListUi,
                                                      void* param6,
                                                      void* param7,
                                                      bool param8) {
    void* result = TaskItemThumbnail_TaskItemThumbnail_Original(
        param1, param2, taskGroup, taskItem, taskListUi, param6, param7,
        param8);
    if (result) {
        try {
            winrt::Windows::Foundation::IInspectable obj = nullptr;
            ((IUnknown*)result + 2)
                ->QueryInterface(
                    winrt::guid_of<winrt::Windows::Foundation::IInspectable>(),
                    winrt::put_abi(obj));
            AddThumbnailTaskItemMapping(obj, taskGroup, taskItem);
        } catch (...) {
        }
    }
    return result;
}

using TaskItemThumbnail_TaskItemThumbnail_2_t =
    void*(WINAPI*)(void* param1,
                   void* param2,
                   void* taskGroup,
                   void* taskItem,
                   void* taskListUi,
                   void* param6,
                   bool param7);
TaskItemThumbnail_TaskItemThumbnail_2_t
    TaskItemThumbnail_TaskItemThumbnail_2_Original;
void* WINAPI TaskItemThumbnail_TaskItemThumbnail_2_Hook(void* param1,
                                                        void* param2,
                                                        void* taskGroup,
                                                        void* taskItem,
                                                        void* taskListUi,
                                                        void* param6,
                                                        bool param7) {
    void* result = TaskItemThumbnail_TaskItemThumbnail_2_Original(
        param1, param2, taskGroup, taskItem, taskListUi, param6, param7);
    if (result) {
        try {
            winrt::Windows::Foundation::IInspectable obj = nullptr;
            ((IUnknown*)result + 2)
                ->QueryInterface(
                    winrt::guid_of<winrt::Windows::Foundation::IInspectable>(),
                    winrt::put_abi(obj));
            AddThumbnailTaskItemMapping(obj, taskGroup, taskItem);
        } catch (...) {
        }
    }
    return result;
}

bool HookTaskbarDllSymbols() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Could not load taskbar.dll — path cache unavailable");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(public: virtual int __cdecl CTaskGroup::GetNumItems(void))"},
            &CTaskGroup_GetNumItems,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CWindowTaskItem::GetWindow(void))"},
            &CWindowTaskItem_GetWindow,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CImmersiveTaskItem::GetAppWindow(void))"},
            &CImmersiveTaskItem_GetAppWindow,
        },
        {
            {LR"(const CImmersiveTaskItem::`vftable')"},
            &CImmersiveTaskItem_vftable,
        },
        {
            {LR"(const CImmersiveTaskItem::`vftable'{for `ITaskItem'})"},
            &CImmersiveTaskItem_vftable_ITaskItem,
        },
        {
            {LR"(const CWindowTaskItem::`vftable')"},
            &CWindowTaskItem_vftable,
            nullptr,
            true,
        },
        {
            {LR"(const CWindowTaskItem::`vftable'{for `ITaskItem'})"},
            &CWindowTaskItem_vftable_ITaskItem,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual long __cdecl CTaskListWnd::HandleClick(struct ITaskGroup *,struct ITaskItem *,struct winrt::Windows::System::LauncherOptions const &))"},
            &CTaskListWnd_HandleClick_Original,
            CTaskListWnd_HandleClick_Hook,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::WindowsUdk::UI::Shell::implementation::TaskItem,struct winrt::WindowsUdk::UI::Shell::ITaskItem>::ReportClicked(void *))"},
            &TaskItem_ReportClicked_Original,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::WindowsUdk::UI::Shell::implementation::TaskGroup,struct winrt::WindowsUdk::UI::Shell::ITaskGroup>::ReportClicked(void *))"},
            &TaskGroup_ReportClicked_Original,
        },
        {
            // Optional: older XAML thumbnail model ctor.
            {LR"(public: __cdecl winrt::WindowsUdk::UI::Shell::implementation::TaskItemThumbnail::TaskItemThumbnail(struct winrt::WindowsUdk::UI::Shell::TaskItem const &,struct ITaskGroup *,struct ITaskItem *,struct ITaskListUI *,struct IWICImagingFactory *,struct ITaskListAcc *,bool))"},
            &TaskItemThumbnail_TaskItemThumbnail_Original,
            TaskItemThumbnail_TaskItemThumbnail_Hook,
            true,
        },
        {
            // Optional: newer ctor (e.g. 10.0.26100.8328+).
            {LR"(public: __cdecl winrt::WindowsUdk::UI::Shell::implementation::TaskItemThumbnail::TaskItemThumbnail(struct winrt::WindowsUdk::UI::Shell::TaskItem const &,struct ITaskGroup *,struct ITaskItem *,struct ITaskListUI *,struct IWICImagingFactory *,bool))"},
            &TaskItemThumbnail_TaskItemThumbnail_2_Original,
            TaskItemThumbnail_TaskItemThumbnail_2_Hook,
            true,
        },
    };

    if (!HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookSymbols failed for taskbar.dll");
        return false;
    }

    g_taskbarDllHooked = true;
    g_taskbandResolveReady = true;
    g_previewHooksReady =
        TaskItemThumbnail_TaskItemThumbnail_Original != nullptr ||
        TaskItemThumbnail_TaskItemThumbnail_2_Original != nullptr;
    if (g_previewHooksReady) {
        Wh_Log(L"Hooked taskbar.dll identity + thumbnail model symbols");
    } else {
        Wh_Log(L"Hooked taskbar.dll identity symbols (preview HWND mapping "
               L"unavailable — title fallback only)");
    }
    return true;
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }
    return module;
}

void HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR lpLibFileName) {
    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);
        if (HookTaskbarViewDllSymbols(module)) {
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
    if (module) {
        HandleLoadedModuleIfTaskbarView(module, lpLibFileName);
    }
    return module;
}

// ---------------------------------------------------------------------------
// Focus promotion (min focus time)
// ---------------------------------------------------------------------------

void CancelMinFocusTimer() {
    if (g_hookThreadHwnd) {
        KillTimer(g_hookThreadHwnd, kMinFocusTimerId);
    }
}

void CancelPreviewMinFocusTimer() {
    if (g_hookThreadHwnd) {
        KillTimer(g_hookThreadHwnd, kPreviewMinFocusTimerId);
    }
}

// True if pending is still the focused app. May retarget to a new top-level
// window of the same PID. False if focus left the process.
bool StillPendingForeground(const PendingFocus& pending,
                            HWND* outHwnd,
                            std::wstring* outTitle) {
    HWND foreground = GetForegroundWindow();
    HWND confirmHwnd = pending.hwnd;
    std::wstring title = pending.windowTitle;
    if (!foreground || foreground != pending.hwnd) {
        DWORD fgPid = 0;
        if (foreground) {
            GetWindowThreadProcessId(foreground, &fgPid);
        }
        if (!foreground || fgPid != pending.processId) {
            return false;
        }
        // ApplicationFrameHost: same PID is not the same app.
        if (IsAppIdKey(pending.key)) {
            std::wstring fgId =
                CanonicalAppId(ToUpper(GetWindowAppUserModelId(foreground)));
            if (fgId != AppIdFromAppKey(pending.key)) {
                return false;
            }
        }
        confirmHwnd = foreground;
        std::wstring t = GetWindowTitle(foreground);
        if (!t.empty()) {
            title = std::move(t);
        }
    }
    if (!confirmHwnd || !IsWindow(confirmHwnd)) {
        return false;
    }
    if (outHwnd) {
        *outHwnd = confirmHwnd;
    }
    if (outTitle) {
        *outTitle = std::move(title);
    }
    return true;
}

void OnPreviewMinFocusTimerElapsed(MinFocusConfirmMode mode) {
    auto settings = SettingsSnap();
    if (!settings->enabled || !settings->previewHighlightEnabled ||
        g_unloading.load()) {
        return;
    }

    PendingFocus pending;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        if (!g_pendingFocus.valid) {
            return;
        }
        pending = g_pendingFocus;
    }

    if (mode == MinFocusConfirmMode::FromTimer) {
        const ULONGLONG now = GetTickCount64();
        const ULONGLONG start = pending.previewStartTick
                                    ? pending.previewStartTick
                                    : pending.focusStartTick;
        const ULONGLONG remaining = RemainingDeadlineMs(
            start, settings->previewMinFocusSeconds, now);
        if (remaining > 0) {
            if (g_hookThreadHwnd) {
                SetTimer(g_hookThreadHwnd, kPreviewMinFocusTimerId,
                         ClampWinTimerMs(remaining), nullptr);
            }
            return;
        }
    }

    HWND confirmHwnd = nullptr;
    std::wstring title;
    if (!StillPendingForeground(pending, &confirmHwnd, &title)) {
        if (mode == MinFocusConfirmMode::FromTimer &&
            IsTransientForeground(GetForegroundWindow())) {
            const ULONGLONG start = pending.previewStartTick
                                        ? pending.previewStartTick
                                        : pending.focusStartTick;
            ULONGLONG remaining = RemainingDeadlineMs(
                start, settings->previewMinFocusSeconds, GetTickCount64());
            if (remaining == 0) {
                remaining = 200;
            }
            if (g_hookThreadHwnd) {
                SetTimer(g_hookThreadHwnd, kPreviewMinFocusTimerId,
                         ClampWinTimerMs(remaining), nullptr);
            }
        }
        // Focus left the app — drop only preview; app timer may still be pending.
        return;
    }

    std::wstring processKey = PathFromAppKey(pending.key);
    if (processKey.empty()) {
        processKey = ToUpper(GetProcessImagePath(pending.processId));
    }
    const ULONGLONG now = GetTickCount64();
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        auto& desk =
            InlineIsEqualGUID(pending.desktopId, GUID_NULL)
                ? CurrentDeskLocked()
                : DeskStateLocked(pending.desktopId);
        StampWindowRecencyLocked(desk, confirmHwnd, processKey, title, now);
        Wh_Log(L"Preview focus confirmed: hwnd=%p %s title=\"%s\" (map=%zu "
               L"desktop=%s)",
               confirmHwnd, pending.displayName.c_str(), title.c_str(),
               desk.windowFocusMap.size(),
               GuidToLogString(pending.desktopId).c_str());
    }

    RequestApplyPreviewVisuals();
}

void OnMinFocusTimerElapsed(MinFocusConfirmMode mode) {
    auto settings = SettingsSnap();
    PendingFocus pending;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        if (!g_pendingFocus.valid) {
            return;
        }
        pending = g_pendingFocus;
    }

    if (mode == MinFocusConfirmMode::FromTimer) {
        const ULONGLONG now = GetTickCount64();
        const ULONGLONG remaining = RemainingDeadlineMs(
            pending.focusStartTick, settings->minFocusSeconds, now);
        if (remaining > 0) {
            if (g_hookThreadHwnd) {
                SetTimer(g_hookThreadHwnd, kMinFocusTimerId,
                         ClampWinTimerMs(remaining), nullptr);
            }
            return;
        }
    }

    HWND confirmHwnd = nullptr;
    std::wstring title;
    if (!StillPendingForeground(pending, &confirmHwnd, &title)) {
        if (mode == MinFocusConfirmMode::FromTimer &&
            IsTransientForeground(GetForegroundWindow())) {
            // Alt-Tab / taskbar stole FG briefly. WndProc already KillTimer'd;
            // keep the candidate and wait out the rest of min-focus.
            ULONGLONG remaining = RemainingDeadlineMs(
                pending.focusStartTick, settings->minFocusSeconds,
                GetTickCount64());
            if (remaining == 0) {
                remaining = 200;
            }
            if (g_hookThreadHwnd) {
                SetTimer(g_hookThreadHwnd, kMinFocusTimerId,
                         ClampWinTimerMs(remaining), nullptr);
            }
            if (settings->glowDebugLog) {
                Wh_Log(L"Min-focus timer: transient FG, still waiting on %s "
                       L"(%llums left)",
                       pending.displayName.c_str(),
                       static_cast<unsigned long long>(remaining));
            }
            return;
        }
        Wh_Log(L"Min-focus timer: focus left %s before confirmation",
               pending.displayName.c_str());
        std::lock_guard<std::mutex> lock(g_stateMutex);
        if (g_pendingFocus.hwnd == pending.hwnd) {
            g_pendingFocus = {};
        }
        return;
    }
    pending.hwnd = confirmHwnd;
    if (!title.empty()) {
        pending.windowTitle = std::move(title);
    }

    const ULONGLONG now = GetTickCount64();
    bool alsoConfirmPreviewWindow = false;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        auto& desk =
            InlineIsEqualGUID(pending.desktopId, GUID_NULL)
                ? CurrentDeskLocked()
                : DeskStateLocked(pending.desktopId);
        AppFocusInfo& info = desk.appFocusMap[pending.key];
        info.key = pending.key;
        info.displayName = pending.displayName;
        if (!pending.windowTitle.empty()) {
            info.lastWindowTitle = pending.windowTitle;
        }
        info.lastHwnd = pending.hwnd;
        info.classUpper = ToUpper(GetWindowClassName(pending.hwnd));
        info.appIdUpper = ToUpper(GetWindowAppUserModelId(pending.hwnd));
        info.lastConfirmedFocusTick = now;

        // If preview min-focus is not longer than app min-focus, promote the
        // window here too (covers minFocus=0 / already-tracked immediate path
        // without waiting for a separate preview timer). When preview min is
        // longer, leave pending so the preview timer can still fire.
        alsoConfirmPreviewWindow =
            settings->previewHighlightEnabled &&
            settings->previewMinFocusSeconds <=
                (std::max)(0, settings->minFocusSeconds);

        if (alsoConfirmPreviewWindow && pending.hwnd &&
            IsWindow(pending.hwnd)) {
            std::wstring processKey = PathFromAppKey(pending.key);
            if (processKey.empty()) {
                processKey = ToUpper(GetProcessImagePath(pending.processId));
            }
            StampWindowRecencyLocked(desk, pending.hwnd, processKey,
                                     pending.windowTitle, now);
        }

        // Keep pending alive while a longer preview timer may still need it.
        const bool previewTimerMayRemain =
            settings->previewHighlightEnabled &&
            settings->previewMinFocusSeconds >
                (std::max)(0, settings->minFocusSeconds);
        if (!previewTimerMayRemain &&
            (g_pendingFocus.hwnd == pending.hwnd ||
             g_pendingFocus.processId == pending.processId)) {
            g_pendingFocus = {};
        }

        RecomputeRanksForDesktopLocked(desk);
        Wh_Log(L"Confirmed focus: %s key=%s (map size=%zu, ranks=%zu, "
               L"title=\"%s\" desktop=%s)",
               pending.displayName.c_str(), pending.key.c_str(),
               desk.appFocusMap.size(), desk.rankedApps.size(),
               pending.windowTitle.c_str(),
               GuidToLogString(pending.desktopId).c_str());
    }

    if (alsoConfirmPreviewWindow) {
        RequestApplyPreviewVisuals();
    }

    // Learn button mapping on UI thread, then apply. Drop tray-only apps that
    // never show a TaskListButton (HA widget, etc.).
    RunOnUiThread([key = pending.key, displayName = pending.displayName,
                   desktopId = pending.desktopId]() {
        AssociateActiveButtonWithKey(key);

        auto live = CollectLiveButtonsOnThisDispatcher();
        for (auto& b : live) {
            EnsureButtonPathCached(b, /*force=*/false);
        }

        bool hasNameCache = false;
        {
            std::lock_guard<std::mutex> lock(g_stateMutex);
            hasNameCache = g_keyToAutomationName.contains(key);
        }
        bool exeNameOnAButton = false;
        for (auto& b : live) {
            if (ScoreExeToAutomationName(displayName,
                                         GetButtonAutomationName(b)) >=
                kScoreMinBind) {
                exeNameOnAButton = true;
                break;
            }
        }
        const bool appears = PathAppearsOnTaskbar(key, displayName) ||
                             hasNameCache || exeNameOnAButton;

        size_t resolvedButtons = 0;
        {
            std::lock_guard<std::mutex> lock(g_buttonPathMutex);
            for (const auto& e : g_buttonPathCache) {
                if (!e.pathUpper.empty()) {
                    ++resolvedButtons;
                }
            }
        }

        {
            std::lock_guard<std::mutex> lock(g_stateMutex);
            auto& desk = InlineIsEqualGUID(desktopId, GUID_NULL)
                             ? CurrentDeskLocked()
                             : DeskStateLocked(desktopId);
            auto it = desk.appFocusMap.find(key);
            if (it != desk.appFocusMap.end()) {
                if (appears) {
                    it->second.seenOnTaskbar = true;
                } else if (SettingsSnap()->requireTaskbarButton &&
                           resolvedButtons >= 2) {
                    Wh_Log(L"Ignoring non-taskbar app: %s (title=\"%s\")",
                           displayName.c_str(),
                           it->second.lastWindowTitle.c_str());
                    it->second.lastConfirmedFocusTick = 0;
                    it->second.seenOnTaskbar = false;
                } else {
                    // Path cache not ready — keep the rank, name-match later.
                    it->second.seenOnTaskbar = true;
                }
            }
            RecomputeRanksForDesktopLocked(desk);
        }

        ApplyAllHighlights_UIThread();
    });
}

// Keep the app min-focus one-shot alive. A stale WM_TIMER KillTimer's the
// live timer; same-app FOREGROUND used to assume it was still running.
void EnsurePendingAppTimer() {
    auto settings = SettingsSnap();
    if (settings->minFocusSeconds <= 0) {
        return;
    }
    PendingFocus pending;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        if (!g_pendingFocus.valid) {
            return;
        }
        pending = g_pendingFocus;
    }
    const ULONGLONG remaining = RemainingDeadlineMs(
        pending.focusStartTick, settings->minFocusSeconds, GetTickCount64());
    if (remaining == 0) {
        OnMinFocusTimerElapsed(MinFocusConfirmMode::Immediate);
        return;
    }
    if (g_hookThreadHwnd) {
        SetTimer(g_hookThreadHwnd, kMinFocusTimerId,
                 ClampWinTimerMs(remaining), nullptr);
    }
}

void SchedulePreviewConfirm(bool windowAlreadyTracked) {
    if (!SettingsSnap()->previewHighlightEnabled || !SettingsSnap()->enabled) {
        return;
    }
    CancelPreviewMinFocusTimer();
    const int previewMin = (std::max)(0, SettingsSnap()->previewMinFocusSeconds);
    if (previewMin <= 0 || windowAlreadyTracked) {
        OnPreviewMinFocusTimerElapsed(MinFocusConfirmMode::Immediate);
        return;
    }
    if (g_hookThreadHwnd) {
        SetTimer(g_hookThreadHwnd, kPreviewMinFocusTimerId,
                 static_cast<UINT>(previewMin) * 1000U, nullptr);
    }
}

void HandleForegroundChanged(HWND hWnd) {
    if (g_unloading.load() || !SettingsSnap()->enabled) {
        return;
    }

    hWnd = NormalizeFocusHwnd(hWnd);

    if (IsTransientForeground(hWnd)) {
        // Alt-Tab frame, taskbar, desktop, IME. Do not cancel min-focus.
        bool ranksNonEmpty = false;
        {
            std::lock_guard<std::mutex> lock(g_stateMutex);
            ranksNonEmpty = !CurrentDeskLocked().rankedApps.empty();
        }
        if (ranksNonEmpty) {
            RequestApplyVisuals();
        }
        return;
    }

    const bool desktopChanged = RefreshCurrentDesktopId();
    if (desktopChanged) {
        ClearButtonRunningGrace();
        bool droppedPending = false;
        {
            std::lock_guard<std::mutex> lock(g_stateMutex);
            droppedPending = DropPendingIfWrongDesktopLocked();
            RecomputeRanksForDesktopLocked(CurrentDeskLocked());
        }
        if (droppedPending) {
            CancelMinFocusTimer();
            CancelPreviewMinFocusTimer();
        }
        g_pendingOverlaySweep = true;
        RequestApplyVisuals();
        RequestApplyPreviewVisuals();
    }

    std::wstring key;
    std::wstring displayName;
    std::wstring windowTitle;
    DWORD processId = 0;
    if (!ResolveAppIdentity(hWnd, key, displayName, processId, &windowTitle)) {
        CancelMinFocusTimer();
        CancelPreviewMinFocusTimer();
        bool ranksNonEmpty = false;
        {
            std::lock_guard<std::mutex> lock(g_stateMutex);
            g_pendingFocus = {};
            ranksNonEmpty = !CurrentDeskLocked().rankedApps.empty();
        }
        // Still repaint ranks — taskbar active states changed.
        if (ranksNonEmpty) {
            RequestApplyVisuals();
        }
        return;
    }

    const ULONGLONG now = GetTickCount64();
    const int minSeconds = (std::max)(0, SettingsSnap()->minFocusSeconds);

    bool alreadyTracked = false;
    bool windowAlreadyTracked = false;
    bool ranksNonEmpty = false;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        auto& desk = CurrentDeskLocked();
        ranksNonEmpty = !desk.rankedApps.empty();
        auto it = desk.appFocusMap.find(key);
        alreadyTracked =
            it != desk.appFocusMap.end() && it->second.lastConfirmedFocusTick > 0;
        // Keep title fresh for matching even before min-focus confirms.
        if (it != desk.appFocusMap.end() && !windowTitle.empty()) {
            it->second.lastWindowTitle = windowTitle;
        }
        auto wit = desk.windowFocusMap.find(hWnd);
        windowAlreadyTracked =
            wit != desk.windowFocusMap.end() &&
            wit->second.lastConfirmedTick > 0;
        if (wit != desk.windowFocusMap.end() && !windowTitle.empty()) {
            wit->second.windowTitle = windowTitle;
        }
    }

    // Always repaint existing ranks on any focus change (Alt-Tab must not
    // leave other ranked icons unstyled until the min-focus timer fires).
    if (ranksNonEmpty || alreadyTracked) {
        RequestApplyVisuals();
    }

    bool sameAppPending = false;
    bool hwndChanged = false;
    GUID deskForLog{};
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);

        if (g_pendingFocus.valid && g_pendingFocus.key == key &&
            InlineIsEqualGUID(g_pendingFocus.desktopId, g_currentDesktopId)) {
            hwndChanged = g_pendingFocus.hwnd != hWnd;
            g_pendingFocus.hwnd = hWnd;
            g_pendingFocus.processId = processId;
            if (!windowTitle.empty()) {
                g_pendingFocus.windowTitle = windowTitle;
            }
            if (hwndChanged) {
                g_pendingFocus.previewStartTick = now;
            }
            sameAppPending = true;
        } else {
            g_pendingFocus.hwnd = hWnd;
            g_pendingFocus.processId = processId;
            g_pendingFocus.key = key;
            g_pendingFocus.displayName = displayName;
            g_pendingFocus.windowTitle = windowTitle;
            g_pendingFocus.focusStartTick = now;
            g_pendingFocus.previewStartTick = now;
            g_pendingFocus.desktopId = g_currentDesktopId;
            g_pendingFocus.valid = true;
        }
        deskForLog = g_pendingFocus.desktopId;
    }

    if (sameAppPending) {
        // Same app: keep app min-focus timer; re-schedule preview if HWND moved
        // between instances (multi-window VS Code / Terminal).
        if (hwndChanged || !windowAlreadyTracked) {
            SchedulePreviewConfirm(windowAlreadyTracked);
        }
        EnsurePendingAppTimer();
        return;
    }

    CancelMinFocusTimer();
    CancelPreviewMinFocusTimer();

    const bool skipMinFocus = ShouldSkipAppMinFocus(key, alreadyTracked);

    Wh_Log(L"Focus candidate: %s key=%s (minFocus=%ds, tracked=%d, skipMin=%d, "
           L"promote=%s, previewTracked=%d desktop=%s)",
           displayName.c_str(), key.c_str(), minSeconds,
           alreadyTracked ? 1 : 0, skipMinFocus ? 1 : 0,
           PromoteModeName(SettingsSnap()->promoteMode),
           windowAlreadyTracked ? 1 : 0, GuidToLogString(deskForLog).c_str());

    SchedulePreviewConfirm(windowAlreadyTracked);

    if (skipMinFocus) {
        // minFocus=0, or promoteMode allows instant re-focus (tracked map
        // and/or currently highlighted top-N).
        OnMinFocusTimerElapsed(MinFocusConfirmMode::Immediate);
        return;
    }

    if (g_hookThreadHwnd) {
        SetTimer(g_hookThreadHwnd, kMinFocusTimerId,
                 static_cast<UINT>(minSeconds) * 1000U, nullptr);
    }
}

void OnDecayTimer() {
    size_t before = 0;
    size_t after = 0;
    size_t windowsBefore = 0;
    size_t windowsAfter = 0;
    RefreshCurrentDesktopId();
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        before = CurrentDeskLocked().rankedApps.size();
        windowsBefore = CurrentDeskLocked().windowFocusMap.size();
        for (auto it = g_desktopMaps.begin(); it != g_desktopMaps.end();) {
            RecomputeRanksForDesktopLocked(it->second);
            PruneWindowFocusMapLocked(it->second);
            if (it->second.appFocusMap.empty() &&
                it->second.windowFocusMap.empty() &&
                !InlineIsEqualGUID(it->first, g_currentDesktopId)) {
                it = g_desktopMaps.erase(it);
            } else {
                ++it;
            }
        }
        after = CurrentDeskLocked().rankedApps.size();
        windowsAfter = CurrentDeskLocked().windowFocusMap.size();
    }
    if (after != before || after == 0) {
        Wh_Log(L"Decay recompute: ranks %zu -> %zu", before, after);
        // Ensure overlays are stripped even if some weak refs are stale after
        // sleep — every live button will clear on next UpdateVisualStates too.
        g_pendingOverlaySweep = true;
        RequestApplyVisuals();
    }
    if (windowsAfter != windowsBefore) {
        Wh_Log(L"Preview decay: windows %zu -> %zu", windowsBefore,
               windowsAfter);
        RequestApplyPreviewVisuals();
    }
}

// ---------------------------------------------------------------------------
// WinEvent hook thread
// ---------------------------------------------------------------------------

void CALLBACK WinEventProc(HWINEVENTHOOK /*hWinEventHook*/,
                           DWORD event,
                           HWND hWnd,
                           LONG idObject,
                           LONG /*idChild*/,
                           DWORD /*dwEventThread*/,
                           DWORD /*dwmsEventTime*/) {
    if (g_unloading.load()) {
        return;
    }
    if (event == EVENT_SYSTEM_DESKTOPSWITCH) {
        if (g_hookThreadHwnd) {
            PostMessage(g_hookThreadHwnd, WM_APP_DESKTOP_SWITCHED, 0, 0);
        }
        return;
    }
    if (event != EVENT_SYSTEM_FOREGROUND) {
        return;
    }
    if (idObject != OBJID_WINDOW || !hWnd) {
        return;
    }

    if (g_hookThreadHwnd) {
        PostMessage(g_hookThreadHwnd, WM_APP_FOREGROUND_CHANGED,
                    reinterpret_cast<WPARAM>(hWnd), 0);
    }
}

LRESULT CALLBACK HookThreadWndProc(HWND hWnd,
                                   UINT msg,
                                   WPARAM wParam,
                                   LPARAM lParam) {
    switch (msg) {
        case WM_APP_FOREGROUND_CHANGED:
            HandleForegroundChanged(reinterpret_cast<HWND>(wParam));
            return 0;
        case WM_APP_DESKTOP_SWITCHED:
            OnVirtualDesktopSwitched();
            return 0;
        case WM_APP_REQUEST_APPLY:
            RequestApplyVisuals();
            return 0;
        case WM_APP_REQUEST_PREVIEW_APPLY:
            RequestApplyPreviewVisuals();
            return 0;
        case WM_APP_SHUTDOWN:
            PostQuitMessage(0);
            return 0;
        case WM_TIMER:
            if (wParam == kMinFocusTimerId) {
                KillTimer(hWnd, kMinFocusTimerId);
                OnMinFocusTimerElapsed();
            } else if (wParam == kPreviewMinFocusTimerId) {
                KillTimer(hWnd, kPreviewMinFocusTimerId);
                OnPreviewMinFocusTimerElapsed();
            } else if (wParam == kDecayTimerId) {
                OnDecayTimer();
            } else if (wParam == kFullRebindTimerId) {
                KillTimer(hWnd, kFullRebindTimerId);
                RequestApplyVisuals();
            }
            return 0;
        case WM_DESTROY:
            KillTimer(hWnd, kMinFocusTimerId);
            KillTimer(hWnd, kPreviewMinFocusTimerId);
            KillTimer(hWnd, kDecayTimerId);
            KillTimer(hWnd, kFullRebindTimerId);
            return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

DWORD WINAPI WinEventHookThread(LPVOID /*param*/) {
    WNDCLASSEXW wc{
        .cbSize = sizeof(WNDCLASSEXW),
        .lpfnWndProc = HookThreadWndProc,
        .hInstance = GetModuleHandle(nullptr),
        .lpszClassName = L"Windhawk_TaskbarRecentFocusHighlight_MsgWnd",
    };
    RegisterClassExW(&wc);

    g_hookThreadHwnd =
        CreateWindowExW(0, wc.lpszClassName, L"", 0, 0, 0, 0, 0, HWND_MESSAGE,
                        nullptr, wc.hInstance, nullptr);
    if (!g_hookThreadHwnd) {
        Wh_Log(L"Failed to create message window: %u", GetLastError());
        return 1;
    }

    HRESULT coHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(coHr) && coHr != RPC_E_CHANGED_MODE) {
        Wh_Log(L"Focus thread CoInitializeEx failed %08X", coHr);
    }

    HWINEVENTHOOK hook =
        SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
                        nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!hook) {
        Wh_Log(L"SetWinEventHook failed: %u", GetLastError());
    } else {
        Wh_Log(L"EVENT_SYSTEM_FOREGROUND hook installed");
    }

    HWINEVENTHOOK deskHook =
        SetWinEventHook(EVENT_SYSTEM_DESKTOPSWITCH, EVENT_SYSTEM_DESKTOPSWITCH,
                        nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!deskHook) {
        Wh_Log(L"EVENT_SYSTEM_DESKTOPSWITCH hook failed: %u", GetLastError());
    } else {
        Wh_Log(L"EVENT_SYSTEM_DESKTOPSWITCH hook installed");
    }

    SetTimer(g_hookThreadHwnd, kDecayTimerId, kDecayCheckIntervalMs, nullptr);
    RefreshCurrentDesktopId();

    if (HWND fg = GetForegroundWindow()) {
        PostMessage(g_hookThreadHwnd, WM_APP_FOREGROUND_CHANGED,
                    reinterpret_cast<WPARAM>(fg), 0);
    }

    MSG msg;
    BOOL bRet;
    while ((bRet = GetMessage(&msg, nullptr, 0, 0)) != 0) {
        if (bRet == -1) {
            break;
        }
        if (msg.message == WM_APP && msg.hwnd == nullptr) {
            break;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (hook) {
        UnhookWinEvent(hook);
    }
    if (deskHook) {
        UnhookWinEvent(deskHook);
    }

    ReleaseVdm();

    if (g_hookThreadHwnd) {
        DestroyWindow(g_hookThreadHwnd);
        g_hookThreadHwnd = nullptr;
    }
    UnregisterClassW(wc.lpszClassName, wc.hInstance);

    if (SUCCEEDED(coHr)) {
        CoUninitialize();
    }

    Wh_Log(L"WinEvent hook thread exiting");
    return 0;
}

void StartWinEventHookThread() {
    std::lock_guard<std::mutex> lock(g_winEventHookThreadMutex);
    if (g_winEventHookThread) {
        return;
    }
    HANDLE hThread =
        CreateThread(nullptr, 0, WinEventHookThread, nullptr, 0, nullptr);
    if (hThread) {
        g_winEventHookThread = hThread;
        Wh_Log(L"WinEvent hook thread started");
    } else {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
    }
}

void StopWinEventHookThread() {
    HANDLE hThread = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_winEventHookThreadMutex);
        hThread = g_winEventHookThread.exchange(nullptr);
    }
    if (!hThread) {
        return;
    }

    HWND hwnd = g_hookThreadHwnd;
    if (hwnd) {
        PostMessage(hwnd, WM_APP_SHUTDOWN, 0, 0);
    }
    DWORD threadId = GetThreadId(hThread);
    if (threadId) {
        PostThreadMessage(threadId, WM_APP, 0, 0);
    }
    const DWORD w = WaitForSingleObject(hThread, 8000);
    CloseHandle(hThread);
    if (w != WAIT_OBJECT_0) {
        Wh_Log(L"ERROR: focus thread still running after 8s (wait=%u) — "
               L"unload may crash explorer",
               w);
    } else {
        Wh_Log(L"WinEvent hook thread stopped");
    }
}

// ---------------------------------------------------------------------------
// Settings load
// ---------------------------------------------------------------------------

void LoadSettings() {
    Settings s;

    s.enabled = Wh_GetIntSetting(L"enabled") != 0;
    s.highlightCount = Wh_GetIntSetting(L"highlightCount");
    if (s.highlightCount < 0) {
        s.highlightCount = 0;
    }
    if (s.highlightCount > 16) {
        s.highlightCount = 16;
    }

    s.minFocusSeconds = Wh_GetIntSetting(L"minFocusSeconds");
    if (s.minFocusSeconds < 0) {
        s.minFocusSeconds = 0;
    }

    PCWSTR promoteMode = Wh_GetStringSetting(L"promoteMode");
    s.promoteMode = PromoteMode::ImmediateTracked;
    if (promoteMode) {
        if (wcscmp(promoteMode, L"immediateTopN") == 0) {
            s.promoteMode = PromoteMode::ImmediateTopN;
        } else if (wcscmp(promoteMode, L"alwaysWait") == 0) {
            s.promoteMode = PromoteMode::AlwaysWait;
        } else if (wcscmp(promoteMode, L"immediateTracked") == 0) {
            s.promoteMode = PromoteMode::ImmediateTracked;
        }
    }
    Wh_FreeStringSetting(promoteMode);

    PCWSTR glowColor = Wh_GetStringSetting(L"glowColor");
    s.glowColor = GlowColorMode::Accent;
    if (glowColor && *glowColor) {
        if (wcscmp(glowColor, L"green") == 0) {
            s.glowColor = GlowColorMode::Green;
        } else if (wcscmp(glowColor, L"blue") == 0) {
            s.glowColor = GlowColorMode::Blue;
        } else if (wcscmp(glowColor, L"orange") == 0) {
            s.glowColor = GlowColorMode::Orange;
        } else if (wcscmp(glowColor, L"white") == 0) {
            s.glowColor = GlowColorMode::White;
        } else if (wcscmp(glowColor, L"custom") == 0) {
            s.glowColor = GlowColorMode::Custom;
        }
    }
    Wh_FreeStringSetting(glowColor);

    PCWSTR customColor = Wh_GetStringSetting(L"customGlowColor");
    s.customGlowColor = customColor ? customColor : L"#00C853";
    Wh_FreeStringSetting(customColor);

    s.glowIntensity[0] = Wh_GetIntSetting(L"glowIntensityRank1");
    s.glowIntensity[1] = Wh_GetIntSetting(L"glowIntensityRank2");
    s.glowIntensity[2] = Wh_GetIntSetting(L"glowIntensityRank3");
    for (int& v : s.glowIntensity) {
        if (v < 0) {
            v = 0;
        }
        if (v > 100) {
            v = 100;
        }
    }

    s.sizeBoostPercent[0] = Wh_GetIntSetting(L"sizeBoostRank1");
    s.sizeBoostPercent[1] = Wh_GetIntSetting(L"sizeBoostRank2");
    s.sizeBoostPercent[2] = Wh_GetIntSetting(L"sizeBoostRank3");
    for (int& v : s.sizeBoostPercent) {
        if (v < 0) {
            v = 0;
        }
        if (v > 50) {
            v = 50;
        }
    }

    PCWSTR glowStyle = Wh_GetStringSetting(L"glowStyle");
    s.glowStyle = GlowStyle::LeftBar;
    if (glowStyle) {
        if (wcscmp(glowStyle, L"full") == 0) {
            s.glowStyle = GlowStyle::Full;
        } else if (wcscmp(glowStyle, L"frame") == 0) {
            s.glowStyle = GlowStyle::Frame;
        } else if (wcscmp(glowStyle, L"leftBar") == 0) {
            s.glowStyle = GlowStyle::LeftBar;
        } else if (wcscmp(glowStyle, L"bottomBar") == 0) {
            s.glowStyle = GlowStyle::BottomBar;
        }
    }
    Wh_FreeStringSetting(glowStyle);

    s.glowThickness = Wh_GetIntSetting(L"glowThickness");
    if (s.glowThickness < 1) {
        s.glowThickness = 1;
    }
    if (s.glowThickness > 16) {
        s.glowThickness = 16;
    }

    s.glowRoundness = Wh_GetIntSetting(L"glowRoundness");
    if (s.glowRoundness < 0) {
        s.glowRoundness = 0;
    }
    if (s.glowRoundness > 50) {
        s.glowRoundness = 50;
    }

    s.glowSize = Wh_GetIntSetting(L"glowSize");
    if (s.glowSize < 40) {
        s.glowSize = 40;
    }
    if (s.glowSize > 100) {
        s.glowSize = 100;
    }

    s.glowLayers = Wh_GetIntSetting(L"glowLayers");
    if (s.glowLayers < 1) {
        s.glowLayers = 1;
    }
    if (s.glowLayers > 3) {
        s.glowLayers = 3;
    }

    s.glowFillOpacity = Wh_GetIntSetting(L"glowFillOpacity");
    if (s.glowFillOpacity < 0) {
        s.glowFillOpacity = 0;
    }
    if (s.glowFillOpacity > 100) {
        s.glowFillOpacity = 100;
    }

    s.previewFillOpacity = Wh_GetIntSetting(L"previewFillOpacity");
    if (s.previewFillOpacity < 0) {
        s.previewFillOpacity = 0;
    }
    if (s.previewFillOpacity > 100) {
        s.previewFillOpacity = 100;
    }

    s.glowDebugLog = Wh_GetIntSetting(L"glowDebugLog") != 0;

    s.decayMinutes = Wh_GetIntSetting(L"decayMinutes");
    if (s.decayMinutes < 0) {
        s.decayMinutes = 0;
    }

    s.requireTaskbarButton = Wh_GetIntSetting(L"requireTaskbarButton") != 0;

    s.previewHighlightEnabled =
        Wh_GetIntSetting(L"previewHighlightEnabled") != 0;

    s.previewHighlightCount = Wh_GetIntSetting(L"previewHighlightCount");
    if (s.previewHighlightCount < 0) {
        s.previewHighlightCount = 0;
    }
    if (s.previewHighlightCount > 16) {
        s.previewHighlightCount = 16;
    }

    s.previewIntensity[0] = Wh_GetIntSetting(L"previewIntensityRank1");
    s.previewIntensity[1] = Wh_GetIntSetting(L"previewIntensityRank2");
    s.previewIntensity[2] = Wh_GetIntSetting(L"previewIntensityRank3");
    for (int& v : s.previewIntensity) {
        if (v < 0) {
            v = 0;
        }
        if (v > 100) {
            v = 100;
        }
    }

    // New keys are blank after an in-place recompile. Wh_GetIntSetting
    // returns 0 for a missing value, which would paint nothing. Treat the
    // all-zero cluster as unset and apply the YAML defaults (3 / 100/70/45).
    // An explicit count of 0 with any intensity set is still honored.
    const bool previewRanksUnset = s.previewIntensity[0] == 0 &&
                                   s.previewIntensity[1] == 0 &&
                                   s.previewIntensity[2] == 0;
    if (previewRanksUnset) {
        s.previewIntensity[0] = 100;
        s.previewIntensity[1] = 70;
        s.previewIntensity[2] = 45;
        if (s.previewHighlightCount == 0) {
            s.previewHighlightCount = 3;
        }
    }

    s.previewMinFocusSeconds = Wh_GetIntSetting(L"previewMinFocusSeconds");
    if (s.previewMinFocusSeconds < 0) {
        s.previewMinFocusSeconds = 0;
    }
    s.previewDecayMinutes = Wh_GetIntSetting(L"previewDecayMinutes");
    if (s.previewDecayMinutes < 0) {
        s.previewDecayMinutes = 0;
    }

    PCWSTR previewStyle = Wh_GetStringSetting(L"previewStyle");
    s.previewStyle = PreviewStyle::TitleBar;
    if (previewStyle) {
        if (wcscmp(previewStyle, L"ring") == 0) {
            s.previewStyle = PreviewStyle::Ring;
        } else if (wcscmp(previewStyle, L"titleBg") == 0) {
            s.previewStyle = PreviewStyle::TitleBg;
        } else if (wcscmp(previewStyle, L"plate") == 0) {
            s.previewStyle = PreviewStyle::Plate;
        } else if (wcscmp(previewStyle, L"plateTitle") == 0) {
            s.previewStyle = PreviewStyle::PlateTitle;
        } else if (wcscmp(previewStyle, L"titleBar") == 0) {
            s.previewStyle = PreviewStyle::TitleBar;
        }
    }
    Wh_FreeStringSetting(previewStyle);

    s.excludedPrograms.clear();
    for (int i = 0;; i++) {
        PCWSTR program = Wh_GetStringSetting(L"excludedPrograms[%d]", i);
        bool hasProgram = program && *program;
        if (hasProgram) {
            s.excludedPrograms.insert(ToUpper(program));
        }
        Wh_FreeStringSetting(program);
        if (!hasProgram) {
            break;
        }
    }

    Wh_Log(L"Settings: enabled=%d style=%s th=%d round=%d%% size=%d%% "
           L"layers=%d fillOp=%d previewFillOp=%d debug=%d decay=%dmin "
           L"minFocus=%ds promote=%s preview=%d previewCount=%d "
           L"previewI=%d/%d/%d previewStyle=%s previewMin=%ds previewDecay=%dmin",
           s.enabled ? 1 : 0, GlowStyleName(s.glowStyle), s.glowThickness,
           s.glowRoundness, s.glowSize, s.glowLayers, s.glowFillOpacity,
           s.previewFillOpacity, s.glowDebugLog ? 1 : 0, s.decayMinutes,
           s.minFocusSeconds, PromoteModeName(s.promoteMode),
           s.previewHighlightEnabled ? 1 : 0, s.previewHighlightCount,
           s.previewIntensity[0], s.previewIntensity[1], s.previewIntensity[2],
           PreviewStyleName(s.previewStyle), s.previewMinFocusSeconds,
           s.previewDecayMinutes);

    PublishSettings(std::move(s));
}

// ---------------------------------------------------------------------------
// Windhawk entry points
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"> Taskbar Recent Focus Highlight init v0.9.0");

    g_unloading = false;
    LoadSettings();

    // Identity resolve (taskband) — optional; fuzzy names remain as fallback.
    if (!HookTaskbarDllSymbols()) {
        Wh_Log(L"Warning: taskbar.dll identity hooks failed — fuzzy match only");
    }

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
            Wh_Log(L"Warning: Taskbar.View hooks failed — visuals unavailable");
        }
    } else {
        Wh_Log(L"Taskbar view module not loaded yet");
    }

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    auto pKernelBaseLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(
        kernelBaseModule, "LoadLibraryExW");
    WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                   LoadLibraryExW_Hook,
                                   &LoadLibraryExW_Original);

    StartWinEventHookThread();
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (!g_taskbarViewDllLoaded) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                Wh_Log(L"Got Taskbar.View.dll");
                if (HookTaskbarViewDllSymbols(taskbarViewModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    if (g_hookThreadHwnd) {
        if (HWND fg = GetForegroundWindow()) {
            PostMessage(g_hookThreadHwnd, WM_APP_FOREGROUND_CHANGED,
                        reinterpret_cast<WPARAM>(fg), 0);
        }
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");
    g_unloading = true;
    g_taskbandResolveReady = false;
    g_previewHooksReady = false;

    // Clear visuals and revoke SizeChanged on each dispatcher, then drain
    // already-queued Normal work (those lambdas no-op on g_unloading).
    if (!RunOnEachUiDispatcherAndWait(
            []() {
                ClearAllHighlights_UIThread();
                ClearAllThumbnailHighlights_UIThread();
                RevokeIconPanelLayoutWatchesOnThisDispatcher();
            },
            2000)) {
        Wh_Log(L"ERROR: UI cleanup did not finish on every dispatcher");
    }
    {
        std::lock_guard<std::mutex> lock(g_layoutWatchMutex);
        if (!g_layoutWatches.empty()) {
            Wh_Log(L"ERROR: %zu IconPanel SizeChanged watches not revoked",
                   g_layoutWatches.size());
        }
    }

    StopWinEventHookThread();

    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        g_desktopMaps.clear();
        g_currentDesktopId = {};
        g_haveCurrentDesktop = false;
        g_pendingFocus = {};
        g_keyToAutomationName.clear();
    }
    ReleaseVdm();
    {
        std::lock_guard<std::mutex> lock(g_buttonsMutex);
        g_trackedButtons.clear();
        g_dispatcherAnchor = {};
    }
    {
        std::lock_guard<std::mutex> lock(g_buttonPathMutex);
        g_buttonPathCache.clear();
    }
    {
        std::lock_guard<std::mutex> lock(g_thumbnailMapMutex);
        g_thumbnailTaskItemMapping.clear();
    }
    {
        std::lock_guard<std::mutex> lock(g_thumbViewsMutex);
        g_trackedThumbViews.clear();
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");
    if (bReload) {
        *bReload = FALSE;
    }

    LoadSettings();

    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        for (auto& [id, desk] : g_desktopMaps) {
            for (auto it = desk.appFocusMap.begin();
                 it != desk.appFocusMap.end();) {
                std::wstring displayUpper = ToUpper(it->second.displayName);
                if (IsExcludedKey(it->first, displayUpper)) {
                    g_keyToAutomationName.erase(it->first);
                    it = desk.appFocusMap.erase(it);
                } else {
                    ++it;
                }
            }
            for (auto it = desk.windowFocusMap.begin();
                 it != desk.windowFocusMap.end();) {
                std::wstring fileUpper =
                    ToUpper(FileNameFromPath(it->second.processKey));
                if (IsExcludedKey(it->second.processKey, fileUpper)) {
                    it = desk.windowFocusMap.erase(it);
                } else {
                    ++it;
                }
            }
            PruneWindowFocusMapLocked(desk);
            RecomputeRanksForDesktopLocked(desk);
        }
    }

    RequestApplyVisuals();
    RequestApplyPreviewVisuals();
    return TRUE;
}
