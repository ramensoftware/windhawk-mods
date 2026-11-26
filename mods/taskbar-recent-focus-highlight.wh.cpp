// ==WindhawkMod==
// @id              taskbar-recent-focus-highlight
// @name            Taskbar Recent Focus Highlight
// @description     Highlight the taskbar icons and window previews of the apps you used last
// @version         0.12.3
// @author          Jakub Vlášek
// @github          https://github.com/jvlasek
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -luuid -lshell32 -ladvapi32
// ==/WindhawkMod==
// -loleaut32 is required: WinRT hresult_error calls SysFreeString / SysStringLen.

// Source code is published under The GNU General Public License v3.0.
// Development repository, tests and investigation notes:
// https://github.com/jvlasek/taskbar-recent-focus-highlight
// (developed as parts in src/, joined into this file by tools/build_mod.py)

// ==WindhawkModReadme==
/*
# Taskbar Recent Focus Highlight

Highlights the taskbar icons of the few apps you work in at once, so you
find them again on a crowded taskbar. Inside a multi-window preview flyout,
it ranks that app's windows the same way.

![A crowded taskbar with the three most recently used apps marked, and the open flyout ranking that app's windows](https://raw.githubusercontent.com/jvlasek/taskbar-recent-focus-highlight/10ce8381849664bb9a73e1530bf889528e38b812/screenshots/0.12.1/hero.png)

## Styles

The most recently used app is marked strongest, the next ones fainter.
Three styles to choose from:

- **Side bar** (default): a bar beside the icon.

  ![Side bar](https://raw.githubusercontent.com/jvlasek/taskbar-recent-focus-highlight/10ce8381849664bb9a73e1530bf889528e38b812/screenshots/0.12.1/icons-side-bar.png)
- **Plate**: the button's own rounded plate, tinted in the highlight color.

  ![Plate](https://raw.githubusercontent.com/jvlasek/taskbar-recent-focus-highlight/10ce8381849664bb9a73e1530bf889528e38b812/screenshots/0.12.1/icons-plate.png)
- **Plate with accent**: the same plate, lighter, with an accent border.

  ![Plate with accent](https://raw.githubusercontent.com/jvlasek/taskbar-recent-focus-highlight/10ce8381849664bb9a73e1530bf889528e38b812/screenshots/0.12.1/icons-plate-with-accent.png)

On a button with several windows the plates also tint its stacked edge. An
app that asks for attention keeps Windows' red marks on top.

## How apps get ranked

- An app counts after **8 s** in front (Minimum focus time); Alt+Tab passing
  through does not.
- It drops out **30 min** after you last *used* it (typing or clicking), so
  the app you work in stays highlighted.
- Only running apps. One list per virtual desktop, the same on every
  monitor.
- Start, Alt+Tab, the taskbar, desktop and lock screen do not count.

## Previews

Flyout window previews are ranked by their last use, for each running app
independently (own timing: 1 s minimum focus, 15 min decay). Rank 1 and the
rest have separate styles: tinted card, tinted title (either with an accent
border), or a bar under the title.

![Default preview styles: whole plate for rank 1, title plates for ranks 2 and 3](https://raw.githubusercontent.com/jvlasek/taskbar-recent-focus-highlight/10ce8381849664bb9a73e1530bf889528e38b812/screenshots/0.12.1/flyout-plate-title.png)

![The "with accent" preview styles](https://raw.githubusercontent.com/jvlasek/taskbar-recent-focus-highlight/10ce8381849664bb9a73e1530bf889528e38b812/screenshots/0.12.1/flyout-plate-accent-title-accent.png)

Single-window flyouts only with **Also mark single-window previews**. Just
the active window marked? Use the much lighter
[taskbar-active-window-highlight](https://windhawk.net/mods/taskbar-active-window-highlight).

## Settings

- **Taskbar** (first four): how many apps, decay, minimum focus time,
  coming back to an app.
- **Taskbar icons**: style, color (system accent or your own), intensity per
  rank, side bar size, fill, accent border width.
- **Thumbnail previews**: on/off, how many windows, their own decay and
  minimum focus time, single-window previews, styles, intensity per rank.
- **Exclude list** (last): apps never highlighted, on icons or previews.

The exclude list takes process names, paths or app IDs. Two buttons of one
program (Total Commander and its Lister, for example) have different app
IDs; with the mod log on, each button's ID is shown as
`Button identity: ... appId=`.

## Compatibility and limitations

- Works with Windows 11 Taskbar Styler. The mod only adds its own elements:
  it changes nothing Windows draws, takes no space, stays visible on hover
  and follows themes that resize the buttons.
- Icons are matched by the app ID Explorer itself groups buttons by. A
  window without a taskbar button is never ranked.
- **SOLIDWORKS** and other apps that show each open document as its own
  preview card manage those previews themselves, so their previews are not
  marked. Their taskbar icon is ranked normally.
- "Use" is keyboard or mouse input anywhere while the app is in front.
  Watching a long video or reading without touching anything is not use,
  so such an app drops out after the decay time; mouse movement on another
  monitor still counts for the app in front.
- Enabled while Explorer is already running, the ranks appear after the
  first taskbar button changes (hover or a window switch).

## Credits

- [taskbar-active-window-highlight](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-active-window-highlight.wh.cpp):
  reading button windows and preview cards through Explorer's own task
  interfaces, and the "with accent" look.
- [taskbar-volume-control-per-app](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-volume-control-per-app.wh.cpp):
  finding a taskbar button's task group.
- [taskbar-thumbnail-reorder](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-thumbnail-reorder.wh.cpp):
  handling the preview flyout.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- highlightCount: 3
  $name: "Taskbar: number of highlighted apps"
  $description: How many recently used apps to highlight on the taskbar (1–6 recommended)
- decayMinutes: 30
  $name: "Taskbar: decay time (minutes)"
  $description: >-
    An app drops out this long after you last used it (keyboard or mouse
    input while it was in front), so the app you work in stays highlighted.
    Window previews have their own decay under Thumbnail previews.
- minFocusSeconds: 8
  $name: "Taskbar: minimum focus time (seconds)"
  $description: >-
    An app counts as used only after it stays in front this long, so passing
    through it with Alt+Tab does not count. Window previews have their own
    minimum under Thumbnail previews.
- promoteMode: immediateTracked
  $name: "Taskbar: coming back to an app"
  $description: >-
    Whether an app you switch back to becomes rank 1 at once or waits the
    minimum focus time again.

    At once if still in the list (default): at once when you used the app
    within the decay time, even if it is not highlighted right now.

    At once only if highlighted: at once only when the app is highlighted
    right now; others wait.

    Always wait: every switch waits the minimum focus time.
  $options:
  - immediateTracked: At once if still in the list (default)
  - immediateTopN: At once only if highlighted
  - alwaysWait: Always wait
- icons:
    - glowStyle: leftBar
      $name: Highlight style
      $description: >-
        How ranked apps look on the taskbar. Bars rotate with the taskbar edge
        (bottom / left / top / right). Side bar = beside the icon (left on a
        bottom or top taskbar, under the icon on a left or right taskbar).
        Plate = the button's own plate tinted (Fill opacity). Plate with
        accent = the same at half the tint with an accent border (Accent
        thickness).
      $options:
      - leftBar: Side bar
      - full: Plate
      - frame: Plate with accent
    - glowColor: accent
      $name: Highlight color
      $description: Base color for icon highlights (and previews)
      $options:
      - accent: System accent color
      - green: Green
      - blue: Blue
      - orange: Orange
      - white: White
      - custom: Custom (see custom highlight color)
    - customGlowColor: "#00C853"
      $name: Custom highlight color
      $description: Used when highlight color is Custom (hex, e.g. #00C853)
    - glowIntensityRank1: 100
      $name: Intensity rank 1
      $description: Overlay opacity for the most recent app (0–100). Linear — 60 is 60% of rank 1, not 36%.
    - glowIntensityRank2: 70
      $name: Intensity rank 2
      $description: Overlay opacity for the 2nd most recent app (0–100)
    - glowIntensityRank3: 45
      $name: Intensity rank 3
      $description: >-
        Overlay opacity for the 3rd most recent app (0–100). Ranks 4+ fade
        further from this value.
    - glowThickness: 3
      $name: Side bar thickness (px)
      $description: Side bar thickness (1–16).
    - glowSize: 92
      $name: Side bar length (%)
      $description: Side bar length (40–100).
    - glowFillOpacity: 40
      $name: Fill opacity
      $description: >-
        0–100. Side bar and Plate strength; half of it for Plate with accent.
        (Thumbnail tints use Previews → Tint opacity.)
    - glowAccentThickness: 2
      $name: Accent thickness (px)
      $description: Border width for Plate with accent (1–8).
  $name: Taskbar icons
- previews:
    - highlightEnabled: true
      $name: Highlight recent windows
      $description: >-
        When hovering a multi-window taskbar icon, rank that flyout’s thumbnails
        by when you last used them and mark the top N. Single-window flyouts:
        see "Also mark single-window previews".
    - highlightCount: 3
      $name: Number of highlighted windows
      $description: >-
        How many recent windows to mark in a multi-window flyout (1–6 recommended).
        Ranking is local to that flyout: the last focused window of this app is
        rank 1 even if other apps were used more recently. Set to 1 to mark only
        the latest window.
    - decayMinutes: 15
      $name: Decay time (minutes)
      $description: >-
        A window's preview drops out this long after you last used it
        (0 = never). Separate from the taskbar's decay time.
    - minFocusSeconds: 1
      $name: Minimum focus time (seconds)
      $description: >-
        How long a window must stay in front before its preview is ranked
        (0 = at once). Separate from the taskbar's minimum focus time.
        Clicking a thumbnail counts like any other switch.
    - singleWindow: false
      $name: Also mark single-window previews
      $description: >-
        Mark the preview of an app with only one window too, as rank 1 while
        that window is recent (the preview minimum focus and decay above), the same
        way the latest window of a multi-window flyout is marked. For a single
        window the mark therefore means "used within the preview decay";
        lower the decay to tighten it.
        For an active-window-only preview highlight, the much smaller
        taskbar-active-window-highlight mod does just that.
    - topStyle: plate
      $name: Rank 1 style
      $description: >-
        How to mark the most recent window. Whole plate = tint the whole
        card. Whole plate with accent = tinted card with an accent border (the
        look of Taskbar Active Window Highlight). Title = a tinted plate over
        the title row. Title with accent = the same with an accent border.
        Bar under title = thin line under the title (thickness follows Taskbar
        icons → Side bar thickness). The plates follow the card's rounded corners and stay
        visible on hover.
      $options:
      - plate: Whole plate
      - outline: Whole plate with accent
      - titleBg: Title
      - titleOutline: Title with accent
      - titleBar: Bar under title
    - restStyle: titleBg
      $name: Ranks 2+ style
      $description: >-
        How to mark the other ranked windows (2nd, 3rd …). Not marked = only
        rank 1 is shown.
      $options:
      - titleBg: Title
      - titleOutline: Title with accent
      - titleBar: Bar under title
      - plate: Whole plate
      - outline: Whole plate with accent
      - none: Not marked
    - outlineThickness: 2
      $name: Accent thickness
      $description: >-
        Border width in pixels for the "with accent" styles (1–8).
    - intensityRank1: 100
      $name: Intensity rank 1
      $description: Intensity for the most recent window in the flyout (0–100)
    - intensityRank2: 70
      $name: Intensity rank 2
      $description: Intensity for the 2nd most recent window in the flyout (0–100)
    - intensityRank3: 45
      $name: Intensity rank 3
      $description: >-
        Intensity for the 3rd most recent window in the flyout (0–100). Ranks
        4+ fade further from this value.
    - fillOpacity: 40
      $name: Tint opacity
      $description: >-
        0–100. Strength of the Whole plate and Title tint (half of it for the
        "with accent" styles, and for ranks 2+ when rank 1 uses one, so no
        lower rank is tinted more than rank 1). Bar under title uses the full
        accent and ignores this. Independent of Taskbar icons → Fill opacity. Per-rank intensity
        still scales the result.
  $name: Thumbnail previews
- excludedPrograms: [""]
  $name: Exclude list
  $description: >-
    Apps that should never be highlighted (icons or previews). Entries can be
    process names, paths or application IDs (Explorer's AppId of a taskbar
    button is shown in the mod log as "Button identity: ... appId=", which
    also tells apart two buttons of one exe), for example:

    mspaint.exe

    C:\Windows\System32\notepad.exe

    Microsoft.WindowsCalculator_8wekyb3d8bbwe!App
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <cmath>
#include <initguid.h>
#include <inspectable.h>
#include <propkey.h>
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
#include <winrt/Windows.UI.Xaml.Data.h>
// WinUI 2 ItemsRepeater (thumbnail flyout). Same as taskbar-thumbnail-reorder.
#define WH_WINRT_WINUI2
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/base.h>

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace winrt::Windows::UI::Xaml;

// IVirtualDesktopManager comes from <shobjidl.h>. The CLSID is spelled out
// so the build does not depend on a uuid library exporting it.
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

// Setting ids `frame` / `full` are older names of Plate with accent / Plate.
enum class GlowStyle {
    PlateAccent,  // id `frame`: native plate, half fill, accent border
    Plate,        // id `full`: native plate, full Fill opacity
    LeftBar,      // side bar: left on horizontal taskbar, bottom on vertical
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
    TitleBg,    // tinted plate over the title row
    Plate,      // tint whole preview card (hover-like plate)
    TitleBar,   // thin bar under the title, above the thumbnail image
    Outline,    // plate plus an accent border (card overlay)
    TitleOutline,  // title plate plus an accent border
    None,       // not marked (ranks 2+ only)
};

// Setting ids <-> enum values. The ids double as log names.
template <typename E>
struct EnumName {
    PCWSTR name;
    E value;
};
constexpr EnumName<GlowColorMode> kGlowColorNames[] = {
    {L"accent", GlowColorMode::Accent}, {L"green", GlowColorMode::Green},
    {L"blue", GlowColorMode::Blue},     {L"orange", GlowColorMode::Orange},
    {L"white", GlowColorMode::White},   {L"custom", GlowColorMode::Custom},
};
constexpr EnumName<GlowStyle> kGlowStyleNames[] = {
    {L"leftBar", GlowStyle::LeftBar},
    {L"frame", GlowStyle::PlateAccent},
    {L"full", GlowStyle::Plate},
};
constexpr EnumName<PromoteMode> kPromoteModeNames[] = {
    {L"immediateTracked", PromoteMode::ImmediateTracked},
    {L"immediateTopN", PromoteMode::ImmediateTopN},
    {L"alwaysWait", PromoteMode::AlwaysWait},
};
constexpr EnumName<PreviewStyle> kPreviewStyleNames[] = {
    {L"plate", PreviewStyle::Plate},       {L"outline", PreviewStyle::Outline},
    {L"titleBg", PreviewStyle::TitleBg},   {L"titleBar", PreviewStyle::TitleBar},
    {L"titleOutline", PreviewStyle::TitleOutline},
    {L"none", PreviewStyle::None},
};
constexpr EnumName<TaskbarEdge> kTaskbarEdgeNames[] = {
    {L"bottom", TaskbarEdge::Bottom}, {L"left", TaskbarEdge::Left},
    {L"top", TaskbarEdge::Top},       {L"right", TaskbarEdge::Right},
};
constexpr EnumName<BarSide> kBarSideNames[] = {
    {L"left", BarSide::Left}, {L"top", BarSide::Top},
    {L"right", BarSide::Right}, {L"bottom", BarSide::Bottom},
};

// Name of a value; the table's first entry for an unknown one.
template <typename E, size_t N>
PCWSTR EnumToName(const EnumName<E> (&table)[N], E value) {
    for (const auto& entry : table) {
        if (entry.value == value) {
            return entry.name;
        }
    }
    return table[0].name;
}

// Value for a name; false (out unchanged) for an unknown one.
template <typename E, size_t N>
bool NameToEnum(const EnumName<E> (&table)[N], PCWSTR name, E& out) {
    for (const auto& entry : table) {
        if (name && wcscmp(entry.name, name) == 0) {
            out = entry.value;
            return true;
        }
    }
    return false;
}

struct Settings {
    int highlightCount = 3;
    int minFocusSeconds = 8;
    PromoteMode promoteMode = PromoteMode::ImmediateTracked;
    GlowColorMode glowColor = GlowColorMode::Accent;
    std::wstring customGlowColor = L"#00C853";
    uint32_t customGlowPacked = 0xFF00C853;  // parsed once in LoadSettings
    int glowIntensity[3] = {100, 70, 45};
    GlowStyle glowStyle = GlowStyle::LeftBar;
    int glowThickness = 3;      // px, side bar
    int glowSize = 92;          // % side bar length (clamped to fit)
    int glowFillOpacity = 40;   // % side bar / plate (half for PlateAccent)
    int glowAccentThickness = 2;  // px, PlateAccent border
    int previewFillOpacity = 40;  // % for thumbnail plate / titleBg only
    int decayMinutes = 30;
    bool previewHighlightEnabled = true;
    int previewHighlightCount = 3;
    bool previewSingleWindow = false;  // also rank single-window flyouts
    int previewIntensity[3] = {100, 70, 45};
    int previewMinFocusSeconds = 1;
    int previewDecayMinutes = 15;
    // Rank 1 and ranks 2+ are styled separately.
    PreviewStyle previewTopStyle = PreviewStyle::Plate;
    PreviewStyle previewRestStyle = PreviewStyle::TitleBg;
    int previewOutlineThickness = 2;
    std::unordered_set<std::wstring> excludedPrograms;
    // Bumped in PublishSettings so UVS can skip a no-op repaint.
    uint32_t generation = 0;
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
    return g_settingsPtr;
}

void PublishSettings(Settings s) {
    static std::atomic<uint32_t> seq{0};
    s.generation = seq.fetch_add(1, std::memory_order_relaxed) + 1;
    auto next = std::make_shared<const Settings>(std::move(s));
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_settingsPtr = std::move(next);
}

// Accent is not inside Settings: an accent change must not republish a
// stale snapshot over a concurrent LoadSettings. Packed ARGB.
constexpr uint32_t kDefaultAccentPacked = 0xFF0078D7u;  // #0078D7
std::atomic<uint32_t> g_cachedAccent{kDefaultAccentPacked};

uint32_t PackColor(winrt::Windows::UI::Color c) {
    return (static_cast<uint32_t>(c.A) << 24) |
           (static_cast<uint32_t>(c.R) << 16) |
           (static_cast<uint32_t>(c.G) << 8) | static_cast<uint32_t>(c.B);
}

winrt::Windows::UI::Color UnpackColor(uint32_t v) {
    return {static_cast<uint8_t>(v >> 24), static_cast<uint8_t>(v >> 16),
            static_cast<uint8_t>(v >> 8), static_cast<uint8_t>(v)};
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

// Per-app record, derived from the window records at each rank recompute
// (DeriveAppsFromWindowsLocked). It outlives its windows until decay, so an
// app keeps its rank while another of its windows is open and promotes at
// once when reopened within the decay time.
struct AppFocusInfo {
    std::wstring key;          // APPID:<Explorer AppId>
    std::wstring displayName;  // e.g. WindowsTerminal.exe
    std::wstring lastWindowTitle;  // unmatched-rank logs only
    std::wstring pathUpper;    // process image path (exclusion sweeps)
    std::wstring appIdUpper;   // window AppUserModelID when present
    HWND lastHwnd = nullptr;
    DWORD lastPid = 0;  // reject recycled lastHwnd (preview already does this)
    ULONGLONG lastConfirmedFocusTick = 0;
    // Rank unit is one window (lastHwnd), not the app: set on the per-window
    // slots RecomputeRanks makes for uncombined buttons (Never Combine).
    bool perWindow = false;
    // Last AppAppearsOnTaskbar result in RecomputeRanks, true = no slot.
    // Only for logging the change.
    bool hiddenNoButton = false;
};

struct PendingFocus {
    HWND hwnd = nullptr;
    DWORD processId = 0;
    std::wstring key;
    std::wstring displayName;
    std::wstring windowTitle;
    std::wstring appIdUpper;  // window AUMID (exclusion only)
    std::wstring pathUpper;   // process image path (exclusion only)
    ULONGLONG focusStartTick = 0;
    ULONGLONG previewStartTick = 0;  // HWND-level; resets when instance changes
    // Post-deadline Alt-Tab / tray / IME wait. 0 = still inside min-focus.
    ULONGLONG transientRetryStartTick = 0;
    ULONGLONG previewTransientRetryStartTick = 0;
    GUID desktopId{};
    bool valid = false;
    bool appConfirmed = false;
    bool previewConfirmed = false;
};

// Per-window recency for multi-instance thumbnail previews (separate timers).
struct WindowFocusInfo {
    HWND hwnd = nullptr;
    DWORD pid = 0;              // reject recycled HWND with a new process
    std::wstring processKey;    // app key: APPID:<Explorer AppId>
    std::wstring pathUpper;     // process image path (exclusion sweeps)
    std::wstring windowTitle;   // log / exclude display; not an identity key
    std::wstring appIdUpper;    // Win32 window AUMID for exclusion sweeps
    ULONGLONG lastConfirmedTick = 0;
    ULONGLONG confirmSeq = 0;  // unique per confirm (breaks GetTickCount ties)
};

struct HwndHash {
    size_t operator()(HWND h) const noexcept {
        return std::hash<uintptr_t>{}(reinterpret_cast<uintptr_t>(h));
    }
};

// Explorer's AppId per taskbar window, from the button captures (UI
// thread); read by the focus worker to key app recency. Upper case. Stored
// with the window's PID so a recycled handle does not inherit an AppId.
// Pruned of dead windows when it grows, and on
// HSHELL_WINDOWDESTROYED.
struct WindowAppIdEntry {
    std::wstring appIdUpper;
    DWORD pid = 0;
};
std::mutex g_windowAppIdMutex;
std::unordered_map<HWND, WindowAppIdEntry, HwndHash> g_windowAppId;

DWORD WindowPid(HWND hwnd) {
    DWORD pid = 0;
    if (hwnd) {
        GetWindowThreadProcessId(hwnd, &pid);
    }
    return pid;
}

void PublishWindowAppIds(const std::vector<HWND>& windows,
                         const std::wstring& appIdUpper) {
    if (appIdUpper.empty() || windows.empty()) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_windowAppIdMutex);
    for (HWND h : windows) {
        // A window already gone has no PID to guard a recycled handle with.
        if (const DWORD pid = WindowPid(h)) {
            g_windowAppId[h] = WindowAppIdEntry{appIdUpper, pid};
        }
    }
    if (g_windowAppId.size() > 512) {
        for (auto it = g_windowAppId.begin(); it != g_windowAppId.end();) {
            it = IsWindow(it->first) ? std::next(it) : g_windowAppId.erase(it);
        }
    }
}

std::wstring LookupWindowAppId(HWND hwnd) {
    std::lock_guard<std::mutex> lock(g_windowAppIdMutex);
    auto it = g_windowAppId.find(hwnd);
    if (it == g_windowAppId.end()) {
        return {};
    }
    if (!it->second.pid || WindowPid(hwnd) != it->second.pid) {
        g_windowAppId.erase(it);  // handle recycled into another process
        return {};
    }
    return it->second.appIdUpper;
}

void ForgetWindowAppId(HWND hwnd) {
    std::lock_guard<std::mutex> lock(g_windowAppIdMutex);
    g_windowAppId.erase(hwnd);
}

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
// App-level (min-focus) confirmation of one window: the recency record.
// The app is not stored with it: each rank
// recompute looks the window up in Explorer's window -> AppId map, so a
// window no captured button lists yet is simply not ranked yet (no
// provisional keys), and a window whose AppId changes follows it. The same
// records give the per-window slots of uncombined buttons (Never Combine).
struct WindowAppFocus {
    std::wstring key;          // derived APPID:<AppId>; empty = not known yet
    std::wstring displayName;
    std::wstring title;
    std::wstring appIdUpper;   // window AUMID (exclude list)
    std::wstring pathUpper;    // process image path (exclude list)
    DWORD pid = 0;
    ULONGLONG tick = 0;
};

// Windows that own a taskbar button inside a multi-window group (an
// uncombined button). Written by the UI capture, read by RecomputeRanks.
// Lock order: g_stateMutex, then g_uncombinedMutex (never the reverse).
std::mutex g_uncombinedMutex;
// Per UI thread (one per taskbar dispatcher), so one taskbar's full pass
// cannot drop another taskbar's windows; readers use the union.
std::unordered_map<DWORD, std::unordered_set<HWND, HwndHash>>
    g_uncombinedWindowsByThread;

struct DesktopRecencyState {
    std::unordered_map<HWND, WindowAppFocus, HwndHash> windowAppFocus;
    std::unordered_map<std::wstring, AppFocusInfo> appFocusMap;
    std::vector<AppFocusInfo> rankedApps;
    std::unordered_map<HWND, WindowFocusInfo, HwndHash> windowFocusMap;
};

std::mutex g_stateMutex;

std::unordered_map<GUID, DesktopRecencyState, GuidHash, GuidEqual> g_desktopMaps;
GUID g_currentDesktopId{};
bool g_haveCurrentDesktop = false;
PendingFocus g_pendingFocus;

std::atomic<ULONGLONG> g_windowConfirmSeq{0};

// Public IVirtualDesktopManager (fallback when registry current-desktop fails).
IVirtualDesktopManager* g_vdm = nullptr;
std::mutex g_vdmMutex;

// HWND plus the PID captured with it. A recycled handle must not match.
struct CapturedWindow {
    HWND hwnd = nullptr;
    DWORD pid = 0;
};

// One synchronous snapshot of a taskbar button (UI thread). Only value data
// is kept; no XAML or task-item pointers outlive the capture.
struct ButtonCapture {
    void* buttonId = nullptr;  // opaque lookup key
    HWND hwnd = nullptr;       // first window of the button
    DWORD pid = 0;
    bool running = false;
    std::vector<CapturedWindow> windows;
    std::wstring explorerAppId;  // ITaskGroup/ITaskItem AppId (upper case)
    bool uncombined = false;     // Never Combine: stands for one window
};


struct ButtonState {
    winrt::weak_ref<FrameworkElement> button;
    // Explorer's AppId for this button (ITaskGroup / ITaskItem get_AppId,
    // upper case), refreshed synchronously on every full pass.
    std::wstring appIdUpper;
    HWND sampleHwnd = nullptr;  // first window of the button
    DWORD samplePid = 0;
    std::vector<CapturedWindow> groupWindows;
    bool uncombined = false;  // own-window button (Never Combine)
    ULONGLONG lastRunningTick = 0;  // last time IsRunning was true (Alt-Tab grace)
    bool observedRunning = false;   // last UI snapshot; not a 400ms heartbeat
    // No snapshot yet (new button) or reset by a desktop / lock switch: the
    // next read is counted in the pass summary, not logged per button.
    bool runningUnknown = true;
    // Last ApplyAllHighlights assignment: -1 unknown, 0 none, >0 1-based rank.
    int lastPaintRank = -1;
    uint32_t lastPaintSettingsGen = 0;
    uint32_t lastPaintAccent = 0;
    TaskbarEdge lastPaintEdge = TaskbarEdge::Bottom;
    int lastPaintBoxW = 0;
    int lastPaintBoxH = 0;
    uint64_t lastPaintPlateGeometry = 0;
};
std::mutex g_buttonStateMutex;
std::unordered_map<void*, ButtonState> g_buttonStates;
// Button identity lookups (Taskbar.View.dll symbols) are hooked and usable.
std::atomic<bool> g_taskbandResolveReady{false};

// Live thumbnail views for unload / re-apply while flyout is open.
std::mutex g_thumbViewsMutex;
std::vector<winrt::weak_ref<FrameworkElement>> g_trackedThumbViews;

std::atomic<bool> g_unloading{false};
std::atomic<bool> g_taskbarViewHookAttempted{false};
// Taskbar.View.dll symbols hooked: this process hosts the taskbar. Until then
// the focus thread tracks nothing (ActivateFocusTracking).
std::atomic<bool> g_taskbarViewHooked{false};
// After decay / empty ranks / desktop switch: ApplyAllHighlights must visit
// every button. UVS must not clear chrome just because this is set (flicker).
std::atomic<bool> g_pendingOverlaySweep{false};
std::atomic<bool> g_decayTimerArmed{false};
// Bumped when buttons the mod has not seen may exist (desktop switch, a
// focused window no captured button lists): the next full pass re-seeds
// every taskbar repeater (SeedButtonsFromRepeater_UIThread).
std::atomic<unsigned> g_buttonSeedGeneration{0};
// A desktop switch also re-seeds once more on the next debounced pass:
// Explorer realizes that desktop's buttons a moment after the switch.
std::atomic<bool> g_reseedOnDebouncedPass{false};
// Hook thread only. Skip re-arming so a hover storm cannot postpone forever.
bool g_fullRebindTimerArmed = false;

std::mutex g_winEventHookThreadMutex;
std::atomic<HANDLE> g_winEventHookThread{nullptr};
std::atomic<HWND> g_hookThreadHwnd{nullptr};
std::atomic<DWORD> g_hookThreadId{0};
HANDLE g_hookThreadReadyEvent = nullptr;

void SignalHookThreadReady() {
    if (g_hookThreadReadyEvent) {
        SetEvent(g_hookThreadReadyEvent);
    }
}

HWND HookThreadWindow() {
    return g_hookThreadHwnd.load(std::memory_order_acquire);
}

bool PostToHookThread(UINT msg, WPARAM wParam = 0, LPARAM lParam = 0) {
    HWND hwnd = HookThreadWindow();
    return hwnd && PostMessage(hwnd, msg, wParam, lParam);
}

bool OnHookThread() {
    DWORD id = g_hookThreadId.load(std::memory_order_acquire);
    return id != 0 && GetCurrentThreadId() == id;
}

// Canonical COM identity for hash maps. The weak_ref in the value is the
// liveness token; recycled pointers are dropped when the weak_ref dies.
void* InspectableIdentity(winrt::Windows::Foundation::IInspectable const& obj) {
    if (!obj) {
        return nullptr;
    }
    try {
        auto unk = obj.as<::IUnknown>();
        return winrt::get_abi(unk);
    } catch (...) {
        return nullptr;
    }
}

bool WeakIsSameElement(winrt::weak_ref<FrameworkElement> const& weak,
                       FrameworkElement expected) {
    if (!expected) {
        return false;
    }
    try {
        auto live = weak.get();
        return live && live == expected;
    } catch (...) {
        return false;
    }
}

// Requires g_buttonStateMutex. This button's cached state, or nullptr. An
// entry left at the same address by a destroyed element is dropped first;
// with create, a fresh entry is made for this button.
ButtonState* FindButtonStateLocked(FrameworkElement const& button,
                                            bool create) {
    void* id = button ? InspectableIdentity(button) : nullptr;
    if (!id) {
        return nullptr;
    }
    auto it = g_buttonStates.find(id);
    if (it != g_buttonStates.end() &&
        !WeakIsSameElement(it->second.button, button)) {
        g_buttonStates.erase(it);
        it = g_buttonStates.end();
    }
    if (it == g_buttonStates.end()) {
        if (!create) {
            return nullptr;
        }
        it = g_buttonStates.emplace(id, ButtonState{}).first;
        it->second.button = winrt::make_weak(button);
    }
    return &it->second;
}

// UI-thread tracking of task list buttons (weak refs, keyed by IUnknown*).
std::mutex g_buttonsMutex;
std::unordered_map<void*, winrt::weak_ref<FrameworkElement>> g_trackedButtons;

// Agile CoreDispatcher list — captured on the UI thread when we first see a
// button/thumb/panel. CollectUiDispatchers must not weak.get() XAML objects
// off the UI thread (last-ref ~FrameworkElement would run on the wrong thread).
// no_destroy: process teardown must not ~CoreDispatcher after XAML is gone.
std::mutex g_dispatchersMutex;
struct UiDispatcher {
    winrt::Windows::UI::Core::CoreDispatcher dispatcher{nullptr};
    winrt::handle thread;
    DWORD threadId;
};
[[clang::no_destroy]] std::optional<
    std::vector<std::shared_ptr<UiDispatcher>>>
    g_uiDispatchers{std::in_place};

bool RememberUiDispatcher(FrameworkElement el) {
    if (!el || g_unloading.load()) {
        return false;
    }
    try {
        auto dispatcher = el.Dispatcher();
        if (!dispatcher || !dispatcher.HasThreadAccess()) {
            return false;
        }
        std::lock_guard<std::mutex> lock(g_dispatchersMutex);
        if (!g_uiDispatchers) {
            return false;
        }
        for (const auto& existing : *g_uiDispatchers) {
            if (existing->dispatcher == dispatcher) {
                return true;
            }
        }
        // Capture a real handle while on its owning thread. An ID can be
        // reused after termination; failure to open it later proves nothing.
        HANDLE thread = nullptr;
        if (!DuplicateHandle(GetCurrentProcess(), GetCurrentThread(),
                             GetCurrentProcess(), &thread, SYNCHRONIZE,
                             FALSE, 0)) {
            const DWORD error = GetLastError();
            Wh_Log(L"Cannot capture UI thread: %u", error);
            return false;
        }
        winrt::handle ownedThread{thread};
        auto entry = std::make_shared<UiDispatcher>();
        entry->dispatcher = dispatcher;
        entry->thread = std::move(ownedThread);
        entry->threadId = GetCurrentThreadId();
        g_uiDispatchers->push_back(std::move(entry));
        return true;
    } catch (...) {
        return false;
    }
}

bool DispatcherTryRun(
    winrt::Windows::UI::Core::CoreDispatcher const& dispatcher,
    winrt::Windows::UI::Core::CoreDispatcherPriority prio,
    winrt::Windows::UI::Core::DispatchedHandler const& handler);

constexpr UINT WM_APP_FOREGROUND_CHANGED = WM_APP + 1;
constexpr UINT WM_APP_DESKTOP_SWITCHED = WM_APP + 2;
constexpr UINT WM_APP_SHUTDOWN = WM_APP + 3;
constexpr UINT WM_APP_REQUEST_APPLY_DEBOUNCED = WM_APP + 4;
constexpr UINT WM_APP_REFRESH_ACCENT = WM_APP + 5;
constexpr UINT WM_APP_SETTINGS_CHANGED = WM_APP + 6;
constexpr UINT WM_APP_TASKBAR_READY = WM_APP + 7;

constexpr UINT_PTR kMinFocusTimerId = 1;
constexpr UINT_PTR kDecayTimerId = 2;
constexpr UINT_PTR kPreviewMinFocusTimerId = 3;
constexpr UINT_PTR kFullRebindTimerId = 4;
// Windows sends several accent messages per change (and UISettings can lag
// the first one): read once, shortly after the last.
constexpr UINT_PTR kAccentRefreshTimerId = 5;
constexpr UINT kAccentRefreshDelayMs = 250;
// Change notification on Explorer's VirtualDesktops registry key (public
// RegNotifyChangeKeyValue). Windows writes CurrentVirtualDesktop there on a
// switch, sometimes after the foreground change the switch causes (clicking
// a flashing button of another desktop), so the write itself is
// the signal. Hook thread only. When the watch could not be set up, every
// foreground change re-reads the value instead.
bool g_desktopKeyWatchActive = false;
// Owned and accessed only by the focus thread.
UINT g_shellHookMessage = 0;
constexpr UINT kDecayCheckIntervalMs = 30 * 1000;
constexpr ULONGLONG kIsRunningGraceMs = 400;
// Full identity rebind (all buttons). UVS only re-paints the cached rank;
// siblings whose visuals Windows cleared without another UVS wait for this.
constexpr ULONGLONG kFullRebindDebounceMs = 300;
// After min-focus elapsed, poll this often while Alt-Tab / tray holds FG.
constexpr ULONGLONG kTransientRetryPollMs = 200;

// All glow layers live on our overlay (never BackgroundElement — hover/active
// storyboards own that and constantly wipe our styles). The mod writes no
// property of a native element.
constexpr PCWSTR kGlowElementName = L"WhRecentFocusGlow";
// The side bar.
constexpr PCWSTR kGlowBarName = L"WhRecentFocusGlowBar";
// Plate / Plate with accent: our copy of BackgroundElement, and of the
// MultiWindowElement sliver on a multi-window button.
constexpr PCWSTR kGlowContourName = L"WhRecentFocusContour";
constexpr PCWSTR kGlowStackName = L"WhRecentFocusContourStack";

// Intensity for a 0-based rank: the three configured values, then each
// further rank fades by a quarter (floor 10 %, never above rank 3) so ranks
// 4+ stay distinguishable.
int RankIntensity(const int (&perRank)[3], int rankIdx) {
    if (rankIdx < 3) {
        return perRank[rankIdx < 0 ? 0 : rankIdx];
    }
    double v = perRank[2];
    for (int r = 3; r <= rankIdx; ++r) {
        v *= 0.75;
    }
    const int floor = (std::min)(perRank[2], 10);
    return (std::max)(floor, static_cast<int>(v + 0.5));
}
constexpr PCWSTR kBackgroundElementName = L"BackgroundElement";
// Thumbnail preview glow (own named overlays on TaskItemThumbnailView).
constexpr PCWSTR kThumbGlowElementName = L"WhRecentFocusThumbGlow";
// Card overlay Border beside BackgroundBorder (14-preview-overlay).
constexpr wchar_t kThumbCardOverlayName[] = L"WhRecentFocusThumbCard";
constexpr PCWSTR kThumbGlowPlateName = L"WhRecentFocusThumbGlowL0";
// Title-area overlays (separate so plate host can sit full-card).
constexpr PCWSTR kThumbTitleBgName = L"WhRecentFocusThumbTitleBg";
constexpr PCWSTR kThumbTitleBarName = L"WhRecentFocusThumbTitleBar";
// ---------------------------------------------------------------------------
// Virtual desktops (public COM + explorer registry — no internal shell GUIDs)
// ---------------------------------------------------------------------------

void CancelMinFocusTimer();
void CancelPreviewMinFocusTimer();
void EnsurePendingAppTimer();
void EnsurePendingPreviewTimer();
void OnMinFocusTimerElapsed(MinFocusConfirmMode mode = MinFocusConfirmMode::FromTimer);
void OnPreviewMinFocusTimerElapsed(
    MinFocusConfirmMode mode = MinFocusConfirmMode::FromTimer);
void RequestApplyVisuals();
void RequestApplyVisualsDebounced();
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
std::unordered_map<void*, IconPanelLayoutWatch> g_layoutWatches;
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
    // CoCreate on the STA that will use it. UI threads must not create or
    // call this pointer — they read the cached desktop GUID instead.
    // Off that thread: no pointer at all (an STA pointer must not cross
    // apartments). No caller does this today.
    if (!OnHookThread()) {
        return nullptr;
    }
    std::lock_guard<std::mutex> lock(g_vdmMutex);
    if (g_vdm) {
        return g_vdm;
    }
    IVirtualDesktopManager* vdm = nullptr;
    HRESULT hr = CoCreateInstance(kClsidVirtualDesktopManager, nullptr,
                                  CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&vdm));
    if (FAILED(hr) || !vdm) {
        Wh_Log(L"IVirtualDesktopManager create failed %08X", hr);
        return nullptr;
    }
    g_vdm = vdm;
    return g_vdm;
}

void ReleaseVdm() {
    if (!OnHookThread() && g_hookThreadId.load() != 0) {
        return;
    }
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
    if (ReadCurrentDesktopFromRegistry(&fromReg)) {
        return fromReg;
    }
    // Registry missing (rare). VDM is worker-thread only.
    GUID fromWnd{};
    if (OnHookThread() &&
        TryGetWindowDesktopId(GetForegroundWindow(), &fromWnd)) {
        return fromWnd;
    }
    std::lock_guard<std::mutex> lock(g_stateMutex);
    return g_currentDesktopId;
}

// Desktop to credit a confirmed focus to. The pending
// record keeps the registry's current desktop from when focus arrived, and
// the registry lags a switch by a moment. At confirm (after min-focus) the
// registry has caught up: if the window is on the current desktop (public
// IVirtualDesktopManager, worker thread), credit that one. GUID_NULL keeps
// the caller's default (current desktop).
GUID ConfirmDesktopId(const GUID& pendingDesktop, HWND hwnd) {
    GUID current{};
    if (!ReadCurrentDesktopFromRegistry(&current) ||
        InlineIsEqualGUID(current, pendingDesktop)) {
        return pendingDesktop;
    }
    IVirtualDesktopManager* vdm = EnsureVdm();
    BOOL onCurrent = FALSE;
    if (hwnd && vdm &&
        SUCCEEDED(vdm->IsWindowOnCurrentVirtualDesktop(hwnd, &onCurrent)) &&
        onCurrent) {
        return current;
    }
    return pendingDesktop;
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
    std::lock_guard<std::mutex> lock(g_buttonStateMutex);
    for (auto& [id, e] : g_buttonStates) {
        e.lastRunningTick = 0;
        e.observedRunning = false;
        e.runningUnknown = true;
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

// reason: what noticed it, for the log. The input-desktop event (lock
// screen, UAC) runs the same steps without a virtual-desktop change.
void OnVirtualDesktopSwitched(PCWSTR reason) {
    if (g_unloading.load()) {
        return;
    }
    ClearButtonRunningGrace();
    RefreshCurrentDesktopId();
    bool droppedPending = false;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        droppedPending = DropPendingIfWrongDesktopLocked();
        // Do not recompute here: grace was just cleared, so every path would
        // fail AppAppearsOnTaskbar. ApplyAllHighlights snapshots IsRunning
        // on the UI thread, then recomputes.
        Wh_Log(L"Desktop switch (%s): current %s droppedPending=%d",
               reason, GuidToLogString(g_currentDesktopId).c_str(),
               droppedPending ? 1 : 0);
    }
    if (droppedPending) {
        CancelMinFocusTimer();
        CancelPreviewMinFocusTimer();
    }
    g_pendingOverlaySweep = true;
    ++g_buttonSeedGeneration;
    g_reseedOnDebouncedPass = true;
    RequestApplyVisualsDebounced();
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

// Extra wait after the min-focus deadline while Alt-Tab / tray / IME holds
// FG (the landed app often sends no second FOREGROUND). Same duration as
// the original wait, at least 2s, then drop the candidate.
ULONGLONG TransientGraceMs(int minFocusSeconds) {
    const ULONGLONG wait =
        static_cast<ULONGLONG>((std::max)(0, minFocusSeconds)) * 1000ULL;
    return (std::max)(2000ULL, wait);
}

// Remaining delay while a transient window holds FG. 0 = give up.
// Updates retryStartTick (0 until the original deadline has passed).
ULONGLONG TransientRetryDelayMs(ULONGLONG startTick,
                                int minFocusSeconds,
                                ULONGLONG now,
                                ULONGLONG& retryStartTick) {
    const ULONGLONG remaining =
        RemainingDeadlineMs(startTick, minFocusSeconds, now);
    if (remaining > 0) {
        retryStartTick = 0;
        return remaining;
    }
    if (retryStartTick == 0) {
        retryStartTick = now;
    }
    if (now - retryStartTick >= TransientGraceMs(minFocusSeconds)) {
        return 0;
    }
    return kTransientRetryPollMs;
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

void ArmHookTimer(UINT_PTR id, ULONGLONG ms) {
    HWND hwnd = HookThreadWindow();
    if (hwnd) {
        SetTimer(hwnd, id, ClampWinTimerMs(ms), nullptr);
    }
}

void DisarmHookTimer(UINT_PTR id) {
    HWND hwnd = HookThreadWindow();
    if (hwnd) {
        KillTimer(hwnd, id);
    }
}

// Requires g_stateMutex.
bool RecencyMapsEmptyLocked() {
    for (const auto& [id, desk] : g_desktopMaps) {
        (void)id;
        if (!desk.appFocusMap.empty() || !desk.windowAppFocus.empty() ||
            !desk.windowFocusMap.empty()) {
            return false;
        }
    }
    return true;
}

// GetTickCount64 of the last decay tick, or of arming (focus thread only):
// OnDecayTimer reports a tick far past its interval.
ULONGLONG g_lastDecayTick = 0;

void EnsureDecayTimerArmed() {
    if (!OnHookThread()) {
        return;
    }
    if (g_decayTimerArmed.exchange(true)) {
        return;
    }
    g_lastDecayTick = GetTickCount64();  // a stopped timer is not a late one
    ArmHookTimer(kDecayTimerId, kDecayCheckIntervalMs);
    Wh_Log(L"Decay timer armed");
}

void StopDecayTimerIfIdle() {
    if (!OnHookThread()) {
        return;
    }
    bool idle = false;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        idle = RecencyMapsEmptyLocked() && !g_pendingFocus.valid;
    }
    if (idle && g_decayTimerArmed.exchange(false)) {
        DisarmHookTimer(kDecayTimerId);
        Wh_Log(L"Decay timer stopped (no tracked apps/windows)");
    }
}

std::wstring ToUpper(std::wstring s) {
    if (!s.empty()) {
        LCMapStringEx(LOCALE_NAME_INVARIANT, LCMAP_UPPERCASE, s.data(),
                      static_cast<int>(s.length()), s.data(),
                      static_cast<int>(s.length()), nullptr, nullptr, 0);
    }
    return s;
}

thread_local DWORD g_imagePathCachePid = 0;
thread_local std::wstring g_imagePathCache;
thread_local int g_imagePathCacheScope = 0;

struct ProcessImagePathCacheScope {
    ProcessImagePathCacheScope() { ++g_imagePathCacheScope; }
    ~ProcessImagePathCacheScope() {
        if (--g_imagePathCacheScope <= 0) {
            g_imagePathCacheScope = 0;
            g_imagePathCachePid = 0;
            std::wstring().swap(g_imagePathCache);
        }
    }
};

std::wstring GetProcessImagePath(DWORD processId) {
    if (g_imagePathCacheScope > 0 && processId != 0 &&
        processId == g_imagePathCachePid) {
        return g_imagePathCache;
    }
    std::wstring path;
    HANDLE hProcess =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (!hProcess) {
        return path;
    }

    auto remember = [&]() {
        if (g_imagePathCacheScope > 0 && processId != 0) {
            g_imagePathCachePid = processId;
            g_imagePathCache = path;
        }
    };

    DWORD size = MAX_PATH;
    for (int attempt = 0; attempt < 8; ++attempt) {
        path.assign(size, L'\0');
        DWORD n = size;
        if (QueryFullProcessImageName(hProcess, 0, path.data(), &n)) {
            path.resize(n);
            CloseHandle(hProcess);
            remember();
            return path;
        }
        if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
            path.clear();
            break;
        }
        // Failure does not report the required size — double until the cap.
        if (size >= 32768) {
            path.clear();
            break;
        }
        size = (size > 16384) ? 32768 : size * 2;
    }
    CloseHandle(hProcess);
    remember();
    return path;
}

DWORD PidFromHwnd(HWND hwnd) {
    DWORD pid = 0;
    if (hwnd) {
        GetWindowThreadProcessId(hwnd, &pid);
    }
    return pid;
}

// False when the handle was recycled into a different process, or when no
// PID was stored (the window was already gone when it was recorded):
// unknown is not a match.
bool HwndMatchesStoredPid(HWND hwnd, DWORD storedPid) {
    if (!hwnd || !storedPid || !IsWindow(hwnd)) {
        return false;
    }
    const DWORD pid = PidFromHwnd(hwnd);
    // pid 0: window is gone / inaccessible — not a match (handle recycle).
    return pid != 0 && pid == storedPid;
}

std::wstring FileNameFromPath(const std::wstring& path) {
    size_t pos = path.find_last_of(L"\\/");
    if (pos == std::wstring::npos) {
        return path;
    }
    return path.substr(pos + 1);
}

bool IsOwnExplorerProcess(DWORD processId) {
    return processId == GetCurrentProcessId();
}

std::wstring AppIdFromAppKey(const std::wstring& key);
std::wstring CanonicalAppId(std::wstring id);

bool IsExcludedKey(const std::wstring& keyUpper,
                   const std::wstring& displayNameUpper,
                   const std::wstring& appIdUpper = {},
                   const std::wstring& pathUpper = {}) {
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
    // App keys are APPID:<Explorer AppId>; the process path comes separately.
    const std::wstring& path = pathUpper;
    if (!path.empty() && excluded.contains(path)) {
        return true;
    }
    if (!path.empty()) {
        std::wstring fileUpper = ToUpper(FileNameFromPath(path));
        if (!fileUpper.empty() && excluded.contains(fileUpper)) {
            return true;
        }
    }
    const std::wstring keyAppId = AppIdFromAppKey(keyUpper);
    if (!keyAppId.empty() && excluded.contains(keyAppId)) {
        return true;
    }
    // Win32 windows can have a path key and a separate window AUMID.
    if (!appIdUpper.empty()) {
        if (excluded.contains(appIdUpper)) {
            return true;
        }
        const std::wstring canonical = CanonicalAppId(appIdUpper);
        if (!canonical.empty() && excluded.contains(canonical)) {
            return true;
        }
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
            hWnd = root;
        }
    }
    // Owned windows (dialogs such as Lister's Find box) have no taskbar
    // button of their own; Explorer reports their owner as the active item
    // (a #32770 dialog owned by Lister). Credit the owner, or
    // a Lister dialog — no AUMID, so path-keyed — counted as Total Commander.
    // Same process only; WS_EX_APPWINDOW asks for its own button, keep it.
    // Climb GW_OWNER step by step and keep the highest *visible* owner:
    // GA_ROOTOWNER would end at a hidden application window (Delphi's
    // TApplication above Lister's TSEARCHTEXT find box), and
    // that hidden window has no button. Stop early at an owner that asks for
    // its own button (WS_EX_APPWINDOW).
    if (!(GetWindowLong(hWnd, GWL_EXSTYLE) & WS_EX_APPWINDOW)) {
        DWORD pid = 0;
        GetWindowThreadProcessId(hWnd, &pid);
        HWND best = nullptr;
        HWND owner = GetWindow(hWnd, GW_OWNER);
        for (int depth = 0; owner && depth < 8; ++depth) {
            DWORD ownerPid = 0;
            GetWindowThreadProcessId(owner, &ownerPid);
            if (!pid || ownerPid != pid) {
                break;
            }
            if (IsWindowVisible(owner)) {
                best = owner;
                if (GetWindowLong(owner, GWL_EXSTYLE) & WS_EX_APPWINDOW) {
                    break;
                }
            }
            owner = GetWindow(owner, GW_OWNER);
        }
        if (best && IsWindow(best)) {
            return best;
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

// The window AUMID is used only for the exclude list, and
// SHGetPropertyStoreForWindow is cross-process: skip it when the list is
// empty (the default).
std::wstring ExclusionAumid(HWND hWnd) {
    if (SettingsSnap()->excludedPrograms.empty()) {
        return {};
    }
    return ToUpper(GetWindowAppUserModelId(hWnd));
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

bool IsUwpHostFileName(const std::wstring& fileNameUpper) {
    return fileNameUpper == L"APPLICATIONFRAMEHOST.EXE" ||
           fileNameUpper == L"WWAHOST.EXE";
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

std::wstring AppIdFromAppKey(const std::wstring& key) {
    if (key.rfind(L"APPID:", 0) != 0) {
        return {};
    }
    return key.substr(6);
}

// Pending-focus identity of a window not (yet) listed by any captured
// taskbar button. Never stored in the recency maps.
std::wstring ProvisionalAppKey(HWND hwnd) {
    wchar_t buf[40];
    swprintf_s(buf, L"WINDOW:%p", hwnd);
    return buf;
}

std::wstring GetWindowTitle(HWND hWnd);

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
        if (GetWindowTitle(hWnd).empty()) {
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
    // ShellExperienceHost: jump lists, Action Center, clock/calendar flyout
    // (a Total Commander jump list used to cancel its minimum focus).
    return fileNameUpper == L"EXPLORER.EXE" ||
           fileNameUpper == L"SEARCHHOST.EXE" ||
           fileNameUpper == L"STARTMENUEXPERIENCEHOST.EXE" ||
           fileNameUpper == L"SHELLEXPERIENCEHOST.EXE" ||
           fileNameUpper == L"SHELLHOST.EXE" ||
           fileNameUpper == L"TEXTINPUTHOST.EXE" ||
           // Lock screen: shell UI, not an app (it was confirmed as focus
           // on wake).
           fileNameUpper == L"LOCKAPP.EXE";
}

// Ordinary File Explorer windows. Same process as the taskbar, but they are
// a real app with a taskbar button — not Alt-Tab / desktop / tray.
bool IsExplorerFolderWindowClass(const std::wstring& classUpper) {
    return classUpper == L"CABINETWCLASS" || classUpper == L"EXPLOREWCLASS";
}

// Alt-Tab UI, taskbar, desktop, IME: not a real app switch. Must not cancel
// an in-flight min-focus candidate — the landed app often sends no second
// FOREGROUND after these. Folder windows are a leave (and can be ranked).
bool IsTransientForeground(HWND hWnd) {
    hWnd = NormalizeFocusHwnd(hWnd);
    if (ShouldIgnoreHwnd(hWnd)) {
        return true;
    }
    DWORD processId = 0;
    GetWindowThreadProcessId(hWnd, &processId);
    if (!processId) {
        return true;
    }
    const std::wstring classUpper = ToUpper(GetWindowClassName(hWnd));
    if (IsExplorerFolderWindowClass(classUpper)) {
        return false;
    }
    if (IsOwnExplorerProcess(processId)) {
        return true;
    }
    return IsShellHostFileName(
        ToUpper(FileNameFromPath(GetProcessImagePath(processId))));
}

std::wstring GetWindowTitle(HWND hWnd) {
    // GetWindowTextW sends WM_GETTEXT to same-process windows, so a hung
    // Explorer folder can stall our worker and its shutdown join. Read stored
    // text without messaging; missing text is acceptable for this label.
    wchar_t buf[512]{};
    int n = InternalGetWindowText(hWnd, buf, ARRAYSIZE(buf));
    if (n <= 0) {
        return {};
    }
    return std::wstring(buf, static_cast<size_t>(n));
}

// App key = APPID:<Explorer AppId> of the taskbar button that lists this
// window. A window that no
// captured button lists yet (just opened, or the mod just loaded and
// Explorer has not reported its buttons) gets a per-window key WINDOW:<hwnd>
// (noButton set). That key only identifies the pending focus; recency is
// recorded per window and joins its app once a button lists the window
// (DeriveAppsFromWindowsLocked). A window that never gets a button (tray
// widget) is never ranked and decays.
struct AppIdentity {
    std::wstring key;          // APPID:<AppId>, or provisional WINDOW:<hwnd>
    std::wstring displayName;  // logs / exclude list
    std::wstring windowTitle;
    std::wstring appIdUpper;   // window AUMID (exclude list only)
    std::wstring pathUpper;    // process image path (exclude list only)
    DWORD processId = 0;
    bool noButton = false;     // provisional key
};

std::optional<AppIdentity> ResolveAppIdentity(HWND hWnd) {
    hWnd = NormalizeFocusHwnd(hWnd);
    if (ShouldIgnoreHwnd(hWnd)) {
        return std::nullopt;
    }

    DWORD processId = 0;
    GetWindowThreadProcessId(hWnd, &processId);
    if (!processId) {
        return std::nullopt;
    }

    const std::wstring classUpper = ToUpper(GetWindowClassName(hWnd));
    const bool explorerFolder = IsExplorerFolderWindowClass(classUpper);
    if (IsOwnExplorerProcess(processId) && !explorerFolder) {
        return std::nullopt;
    }

    std::wstring path = GetProcessImagePath(processId);
    if (path.empty()) {
        return std::nullopt;
    }

    std::wstring pathUpper = ToUpper(path);
    std::wstring fileName = FileNameFromPath(path);
    std::wstring fileNameUpper = ToUpper(fileName);

    if (IsShellHostFileName(fileNameUpper) && !explorerFolder) {
        return std::nullopt;
    }

    std::wstring appIdUpper = ExclusionAumid(hWnd);
    const std::wstring explorerAppId = LookupWindowAppId(hWnd);
    const std::wstring title = GetWindowTitle(hWnd);
    std::wstring displayName = fileName;
    if (IsUwpHostFileName(fileNameUpper)) {
        // Prefer "Settings" over WINDOWS.IMMERSIVECONTROLPANEL for logs /
        // exclude. Key remains APPID:… .
        if (!title.empty() && title.size() <= 80 &&
            title.find(L'\\') == std::wstring::npos &&
            title.find(L'/') == std::wstring::npos) {
            displayName = title;
        } else {
            displayName = DisplayNameFromAppId(explorerAppId);
        }
    }

    AppIdentity id;
    id.noButton = explorerAppId.empty();
    id.key = id.noButton ? ProvisionalAppKey(hWnd)
                         : std::wstring(L"APPID:") + explorerAppId;
    // Exclude list: exe name, path, window AUMID or Explorer AppId.
    if (IsExcludedKey(id.key, ToUpper(displayName), appIdUpper, pathUpper)) {
        Wh_Log(L"Excluded: %s", displayName.c_str());
        return std::nullopt;
    }
    id.displayName = std::move(displayName);
    id.windowTitle = title;
    id.appIdUpper = std::move(appIdUpper);
    id.pathUpper = std::move(pathUpper);
    id.processId = processId;
    return id;
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

// True if a running (or just-closing) taskbar button carries this app's
// Explorer AppId and still lists a live window of it. Ranks without one keep
// their history but take no slot. observedRunning is the last UI IsRunning
// snapshot; it can stay set for a button Explorer parked without a final
// state update, so a live window is required too.
// Grace covers Alt-Tab flicker, not a heartbeat. No weak.get() here.
bool AppAppearsOnTaskbar(const std::wstring& key) {
    const std::wstring wantAppId = AppIdFromAppKey(key);
    if (wantAppId.empty()) {
        return false;
    }
    const ULONGLONG now = GetTickCount64();
    std::lock_guard<std::mutex> lock(g_buttonStateMutex);
    for (const auto& [cacheKey, e] : g_buttonStates) {
        (void)cacheKey;
        const bool recentlyRunning =
            e.lastRunningTick != 0 &&
            now - e.lastRunningTick < kIsRunningGraceMs;
        if (e.appIdUpper != wantAppId) {
            continue;
        }
        if (recentlyRunning) {
            return true;
        }
        if (e.observedRunning) {
            for (const auto& w : e.groupWindows) {
                if (HwndMatchesStoredPid(w.hwnd, w.pid)) {
                    return true;
                }
            }
        }
    }
    return false;
}

// Requires g_stateMutex. Drops dead, recycled and decayed window records,
// then folds the others into their app's record by the window's current
// Explorer AppId (newest confirmation wins). A window without a known AppId
// keeps its record and is folded in once a button lists it.
void DeriveAppsFromWindowsLocked(DesktopRecencyState& desk,
                                 ULONGLONG decayMs,
                                 ULONGLONG now) {
    for (auto it = desk.windowAppFocus.begin();
         it != desk.windowAppFocus.end();) {
        HWND hwnd = it->first;
        WindowAppFocus& w = it->second;
        if (!IsWindow(hwnd) || !HwndMatchesStoredPid(hwnd, w.pid) ||
            IsTickDecayed(w.tick, decayMs, now)) {
            it = desk.windowAppFocus.erase(it);
            continue;
        }
        const std::wstring appId = LookupWindowAppId(hwnd);
        const std::wstring key =
            appId.empty() ? std::wstring{} : std::wstring(L"APPID:") + appId;
        if (key != w.key && !key.empty()) {
            Wh_Log(L"Window %p (%s) belongs to %s", hwnd,
                   w.displayName.c_str(), key.c_str());
        }
        w.key = key;
        if (!key.empty()) {
            AppFocusInfo& app = desk.appFocusMap[key];
            if (app.key.empty() || w.tick >= app.lastConfirmedFocusTick) {
                app.key = key;
                app.displayName = w.displayName;
                if (!w.title.empty()) {
                    app.lastWindowTitle = w.title;
                }
                app.appIdUpper = w.appIdUpper;
                app.pathUpper = w.pathUpper;
                app.lastHwnd = hwnd;
                app.lastPid = w.pid;
                app.lastConfirmedFocusTick = w.tick;
            }
        }
        ++it;
    }
}

void RecomputeRanksForDesktopLocked(DesktopRecencyState& desk) {
    desk.rankedApps.clear();

    auto settings = SettingsSnap();
    if (g_unloading.load()) {
        return;
    }

    const ULONGLONG now = GetTickCount64();
    const ULONGLONG decayMs = DecayMsFromMinutes(settings->decayMinutes);
    DeriveAppsFromWindowsLocked(desk, decayMs, now);

    std::vector<AppFocusInfo> candidates;
    candidates.reserve(desk.appFocusMap.size());

    for (auto it = desk.appFocusMap.begin(); it != desk.appFocusMap.end();) {
        auto& info = it->second;
        if (info.lastConfirmedFocusTick == 0) {
            ++it;
            continue;
        }
        if (IsTickDecayed(info.lastConfirmedFocusTick, decayMs, now)) {
            // Key: two apps can share a display name (Total Commander and
            // its Lister windows are both TOTALCMD64.EXE).
            const ULONGLONG idleTenths =
                (now - info.lastConfirmedFocusTick) / 6000;
            Wh_Log(L"Decayed: %s key=%s (last use %llu.%llu min ago)",
                   info.displayName.c_str(), info.key.c_str(),
                   static_cast<unsigned long long>(idleTenths / 10),
                   static_cast<unsigned long long>(idleTenths % 10));
            it = desk.appFocusMap.erase(it);
            continue;
        }
        // No running button with this AppId (closed, pinned-only, tray-only
        // window): keep history but do not occupy a visible slot. Logged on
        // change, which also explains a ladder that empties without a decay
        // (lock or desktop switch clears the running grace until the next
        // full pass re-reads IsRunning).
        const bool hidden = !AppAppearsOnTaskbar(info.key);
        if (hidden != info.hiddenNoButton) {
            info.hiddenNoButton = hidden;
            if (hidden) {
                Wh_Log(L"Hidden (no running button): %s key=%s",
                       info.displayName.c_str(), info.key.c_str());
            } else {
                Wh_Log(L"Shown again (running button): %s",
                       info.displayName.c_str());
            }
        }
        if (hidden) {
            ++it;
            continue;
        }
        candidates.push_back(info);
        ++it;
    }

    // Per-window slots (window records were pruned by the derive step).
    {
        std::unordered_set<HWND, HwndHash> uncombined;
        {
            std::lock_guard<std::mutex> lock(g_uncombinedMutex);
            for (auto& [thread, windows] : g_uncombinedWindowsByThread) {
                (void)thread;
                for (auto it = windows.begin(); it != windows.end();) {
                    it = IsWindow(*it) ? std::next(it) : windows.erase(it);
                }
                uncombined.insert(windows.begin(), windows.end());
            }
        }
        if (!uncombined.empty()) {
            // An app is split when any of its confirmed windows has its own
            // button; each confirmed window then is its own unit.
            std::vector<AppFocusInfo> expanded;
            for (auto& app : candidates) {
                bool split = false;
                for (const auto& [hwnd, w] : desk.windowAppFocus) {
                    if (w.key == app.key && uncombined.count(hwnd)) {
                        split = true;
                        break;
                    }
                }
                if (!split) {
                    expanded.push_back(std::move(app));
                    continue;
                }
                for (const auto& [hwnd, w] : desk.windowAppFocus) {
                    if (w.key != app.key) {
                        continue;
                    }
                    AppFocusInfo unit = app;
                    unit.perWindow = true;
                    unit.lastHwnd = hwnd;
                    unit.lastPid = w.pid;
                    unit.lastConfirmedFocusTick = w.tick;
                    if (!w.title.empty()) {
                        unit.lastWindowTitle = w.title;
                    }
                    expanded.push_back(std::move(unit));
                }
            }
            candidates = std::move(expanded);
        }
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

// ---------------------------------------------------------------------------
// Window-level recency (thumbnail previews)
// ---------------------------------------------------------------------------

void PruneWindowFocusMapLocked(DesktopRecencyState& desk);
void StampWindowRecencyLocked(DesktopRecencyState& desk,
                              HWND hwnd,
                              const std::wstring& processKey,
                              const std::wstring& windowTitle,
                              ULONGLONG now,
                              const std::wstring& appIdUpper = {},
                              const std::wstring& pathUpper = {});

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
                              ULONGLONG now,
                              const std::wstring& appIdUpper,
                              const std::wstring& pathUpper) {
    if (!hwnd) {
        return;
    }
    const DWORD pid = PidFromHwnd(hwnd);
    if (!pid) {
        return;  // gone already: nothing could check a recycled handle later
    }
    auto it = desk.windowFocusMap.find(hwnd);
    if (it != desk.windowFocusMap.end() && it->second.pid != 0 &&
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
    if (!pathUpper.empty()) {
        winfo.pathUpper = pathUpper;
    }
    if (!windowTitle.empty()) {
        winfo.windowTitle = windowTitle;
    }
    if (!appIdUpper.empty()) {
        winfo.appIdUpper = appIdUpper;
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

// Focus thread only, when settings load or Windows reports an accent change
// (AccentChangeMessage), never per paint.
winrt::Windows::UI::Color QuerySystemAccentColor() {
    try {
        winrt::Windows::UI::ViewManagement::UISettings uiSettings;
        auto c = uiSettings.GetColorValue(
            winrt::Windows::UI::ViewManagement::UIColorType::Accent);
        c.A = 255;
        return c;
    } catch (...) {
        return UnpackColor(kDefaultAccentPacked);
    }
}

void RefreshCachedAccent() {
    winrt::Windows::UI::Color accent = QuerySystemAccentColor();
    const uint32_t packed = PackColor(accent);
    const uint32_t prev =
        g_cachedAccent.exchange(packed, std::memory_order_relaxed);
    if (prev != packed) {
        Wh_Log(L"System accent updated: #%02X%02X%02X", accent.R, accent.G,
               accent.B);
    }
}

// Accent changes arrive as window messages on the focus worker's hidden
// top-level window. Not UISettings::ColorValuesChanged: its handler runs on
// a WinRT thread-pool thread, and revoking it on unload does not wait for a
// call already running, which could still be in the mod's code after
// Windhawk unloads it.
bool AccentChangeMessage(UINT msg, LPARAM lParam) {
    if (msg == WM_DWMCOLORIZATIONCOLORCHANGED) {
        return true;
    }
    // lParam is the changed area's name or null; Windows copies the string
    // for a sender in another process.
    return msg == WM_SETTINGCHANGE && lParam &&
           _wcsicmp(reinterpret_cast<PCWSTR>(lParam), L"ImmersiveColorSet") ==
               0;
}

winrt::Windows::UI::Color ResolveGlowBaseColor(const Settings& settings) {
    winrt::Windows::UI::Color c{255, 0, 200, 83};  // default green-ish

    switch (settings.glowColor) {
        case GlowColorMode::Accent:
            c = UnpackColor(g_cachedAccent.load(std::memory_order_relaxed));
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
            c = UnpackColor(settings.customGlowPacked);
            break;
    }
    return c;
}

struct ButtonIdentity {
    std::wstring appIdUpper;  // Explorer's AppId
    bool uncombined = false;  // own-window button (Never Combine)
    HWND sampleHwnd = nullptr;
    DWORD samplePid = 0;
    std::vector<CapturedWindow> groupWindows;
};
ButtonIdentity GetCachedButtonIdentity(FrameworkElement button);
struct PaintCacheState {
    int rank = -1;
    uint32_t settingsGen = 0;
    uint32_t accent = 0;
    TaskbarEdge edge = TaskbarEdge::Bottom;
    int boxW = 0;
    int boxH = 0;
    uint64_t plateGeometry = 0;  // ReadPlateShapes signature (plates only)
};
PaintCacheState GetCachedPaintState(FrameworkElement button);
void SetCachedPaintState(FrameworkElement button,
                         int rank,
                         uint32_t gen,
                         TaskbarEdge edge = TaskbarEdge::Bottom,
                         int boxW = 0,
                         int boxH = 0,
                         uint64_t plateGeometry = 0);
bool RunOnUiThread(const winrt::Windows::UI::Core::DispatchedHandler& handler);
void ApplyAllHighlights_UIThread();
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
// Running states read for the first time (new buttons) or again after a
// desktop / lock switch reset them. Summed per UI thread and logged once by
// the next full pass instead of one "Identity state" line per button.
struct RunningReread {
    unsigned running = 0;
    unsigned stopped = 0;
};
thread_local RunningReread t_runningReread;

// Virtual-desktop switches clear observedRunning so pinned-not-running icons
// on another desktop never keep a highlight. Call this on the UI thread
// before AppAppearsOnTaskbar / rank eligibility.
bool ButtonCountsAsRunning(FrameworkElement button) {
    const bool running = TaskListButton_IsRunning(button);
    const ULONGLONG now = GetTickCount64();
    void* id = InspectableIdentity(button);
    bool grace = false;
    {
        std::lock_guard<std::mutex> lock(g_buttonStateMutex);
        ButtonState* state = FindButtonStateLocked(button, false);
        if (!state) {
            return running;
        }
        auto& e = *state;
        if (e.runningUnknown) {
            e.runningUnknown = false;
            ++(running ? t_runningReread.running : t_runningReread.stopped);
        } else if (running != e.observedRunning) {
            Wh_Log(L"Identity state: button=%p running=%d->%d hwnd=%p pid=%u appId=%s",
                   id, e.observedRunning, running, e.sampleHwnd, e.samplePid,
                   e.appIdUpper.c_str());
        }
        if (running) {
            e.lastRunningTick = now;
            e.observedRunning = true;
            return true;
        }
        e.observedRunning = false;
        // Grace protects a transient state change, not a closed app. Check
        // the whole group: the sampled window might close before its siblings.
        bool liveWindow = HwndMatchesStoredPid(e.sampleHwnd, e.samplePid);
        for (const auto& window : e.groupWindows) {
            if (HwndMatchesStoredPid(window.hwnd, window.pid)) {
                liveWindow = true;
                break;
            }
        }
        if (!liveWindow) {
            e.lastRunningTick = 0;
        }
        grace = e.lastRunningTick != 0 &&
                now - e.lastRunningTick < kIsRunningGraceMs;
    }
    if (grace) {
        // A close animation can report not-running before HWND destruction.
        // Ensure another pass happens even if Explorer sends no further UVS.
        ScheduleRefreshAllHighlights(button);
    }
    // Grace preserves rank eligibility only. A stopped button must never
    // keep painting merely because its closing HWND is still alive.
    return false;
}

std::wstring GetButtonAutomationName(FrameworkElement button) {
    try {
        return std::wstring(
            Automation::AutomationProperties::GetName(button).c_str());
    } catch (...) {
        return {};
    }
}

// "Window: 0x31426" on an uncombined button (Never Combine) names its window,
// not an app; see GetButtonAutomationWindow.
bool IsWindowAutomationId(const std::wstring& id) {
    return id.size() > 7 && _wcsnicmp(id.c_str(), L"Window:", 7) == 0;
}

// Uncombined taskbar buttons (Never Combine / combine when full) carry their
// window in AutomationProperties.AutomationId: "Window: 0x<hwnd hex>"; group
// buttons carry "Appid: <AUMID>". Explorer's identifier, not a display
// name. Callers must still
// check the HWND against the group's windows.
HWND GetButtonAutomationWindow(FrameworkElement button) {
    if (!button) {
        return nullptr;
    }
    try {
        std::wstring id =
            Automation::AutomationProperties::GetAutomationId(button).c_str();
        if (!IsWindowAutomationId(id)) {
            return nullptr;
        }
        const wchar_t* p = id.c_str() + 7;
        while (*p == L' ' || *p == L'\t') {
            ++p;
        }
        wchar_t* end = nullptr;
        const unsigned long long value = wcstoull(p, &end, 16);
        if (!value || end == p) {
            return nullptr;
        }
        HWND hwnd = reinterpret_cast<HWND>(static_cast<uintptr_t>(value));
        return IsWindow(hwnd) ? hwnd : nullptr;
    } catch (...) {
        return nullptr;
    }
}

// Explorer AppId equality, plus per-window slots for uncombined buttons.
// Same rank may bind many buttons (secondary taskbar / Never Combine).
bool IdentityMatchesRank(const ButtonIdentity& ident,
                         const AppFocusInfo& info) {
    // Per-window slot: only the button that lists exactly this window (an
    // uncombined button lists just its own). No AppId match, or every
    // button of the app would light up.
    if (info.perWindow) {
        if (!info.lastHwnd || !info.lastPid ||
            !HwndMatchesStoredPid(info.lastHwnd, info.lastPid)) {
            return false;
        }
        for (const auto& g : ident.groupWindows) {
            if (g.hwnd == info.lastHwnd && g.pid == info.lastPid) {
                return true;
            }
        }
        return ident.sampleHwnd == info.lastHwnd &&
               ident.samplePid == info.lastPid;
    }
    // One taskbar button = one app = one Explorer AppId.
    if (ident.appIdUpper.empty() ||
        AppIdFromAppKey(info.key) != ident.appIdUpper) {
        return false;
    }
    // Uncombined buttons (Never Combine) each stand for one window. An
    // app-level rank (window not confirmed on its own yet) lights only the
    // button of the app's last focused window, not every window of the app.
    if (ident.uncombined) {
        return info.lastHwnd && info.lastPid &&
               HwndMatchesStoredPid(info.lastHwnd, info.lastPid) &&
               ident.sampleHwnd == info.lastHwnd;
    }
    return true;
}

// Best rank for a single button (1-based), or 0.
int FindRankForButton(FrameworkElement button,
                      const std::vector<AppFocusInfo>& ranks) {
    if (!button || ranks.empty()) {
        return 0;
    }
    if (!ButtonCountsAsRunning(button)) {
        return 0;
    }

    const ButtonIdentity ident = GetCachedButtonIdentity(button);
    for (size_t i = 0; i < ranks.size(); i++) {
        if (IdentityMatchesRank(ident, ranks[i])) {
            return static_cast<int>(i) + 1;
        }
    }
    return 0;
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
// under it.
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

// Position in the native-only child sequence. Never reorder native children.
struct GlowOrderChild {
    bool aboveHost;
};
uint32_t GlowHostInsertionIndex(const std::vector<GlowOrderChild>& children) {
    for (uint32_t i = 0; i < children.size(); ++i) {
        if (children[i].aboveHost) {
            return i;
        }
    }
    return static_cast<uint32_t>(children.size());
}

// Preserve the established style layering without changing native properties.
// Explicit InsertAt is required even at the end: Append omits XAML deferred
// index notifications while RemoveAt sends them.
void EnsureGlowHostZOrder(Controls::Panel panel, UIElement host) {
    if (!panel || !host)
        return;
    try {
        auto children = panel.Children();
        uint32_t current;
        if (!children.IndexOf(host, current))
            return;
        std::vector<GlowOrderChild> native;
        auto panelFe = panel.as<FrameworkElement>();
        for (auto child : children) {
            if (child == host)
                continue;
            auto fe = child.try_as<FrameworkElement>();
            auto name = fe ? fe.Name() : winrt::hstring{};
            bool running = name == L"RunningIndicator";
            bool above = name == L"OverlayIcon" ||
                         name == L"MultiWindowElement" ||
                         name == L"ProgressIndicator";
            // Every style sits behind the glyph.
            if (name == L"Icon" || name == L"DefaultIcon")
                above = true;
            if (running && !RunningIndicatorLooksLikeHoverPlate(fe, panelFe))
                above = true;
            native.push_back({above});
        }
        uint32_t target = GlowHostInsertionIndex(native);
        if (target == current)
            return;
        children.RemoveAt(current);
        children.InsertAt(target, host);
    } catch (...) {
        Wh_Log(L"Glow host placement failed: %08X", winrt::to_hresult());
    }
}

// iconPanel: GetIconPanel(button), when the caller already has it.
bool ButtonHasOurChromeIn(FrameworkElement button, FrameworkElement iconPanel) {
    if (!button) {
        return false;
    }
    if (!iconPanel) {
        return FindChildByName(button, kGlowElementName) != nullptr;
    }
    return FindChildByName(iconPanel, kGlowElementName) != nullptr;
}

bool ButtonHasOurChrome(FrameworkElement button) {
    return button && ButtonHasOurChromeIn(button, GetIconPanel(button));
}

void ClearButtonHighlight(FrameworkElement button) {
    if (!button) {
        return;
    }

    auto iconPanelEarly = GetIconPanel(button);

    // Skip no-op clears on every mouse-over, including overlay sweep.
    // Sweep only removes leftover WhRecentFocusGlow; do not reorder native
    // children on buttons we never painted.
    if (!ButtonHasOurChromeIn(button, iconPanelEarly)) {
        SetCachedPaintState(button, 0, SettingsSnap()->generation);
        return;
    }

    try {
        auto iconPanel = iconPanelEarly ? iconPanelEarly : GetIconPanel(button);
        if (!iconPanel) {
            // Still try to strip our named overlay from the button root.
            if (auto panel = button.try_as<Controls::Panel>()) {
                RemoveNamedChild(panel, kGlowElementName);
            }
            SetCachedPaintState(button, 0, SettingsSnap()->generation);
            return;
        }

        // Do NOT ClearValue BackgroundElement — we no longer style it; clearing
        // local values can leave a stuck pale hover plate after decay.

        // EnsureGlowHost adds exactly one host, as a direct child.
        if (auto panel = iconPanel.try_as<Controls::Panel>()) {
            RemoveNamedChild(panel, kGlowElementName);
        }

        SetCachedPaintState(button, 0, SettingsSnap()->generation);
    } catch (...) {
        HRESULT hr = winrt::to_hresult();
        Wh_Log(L"ClearButtonHighlight error %08X", hr);
    }
}

// Ensure glow host grid exists (side bar rectangle). Returns host or nullptr.
Controls::Grid EnsureGlowHost(Controls::Panel panel,
                              FrameworkElement iconPanel) {
    Controls::Grid host = nullptr;
    if (auto existing = FindChildByName(iconPanel, kGlowElementName)) {
        host = existing.try_as<Controls::Grid>();
        if (host && !FindChildByName(host, kGlowBarName)
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
                <Rectangle Name="WhRecentFocusGlowBar"
                           IsHitTestVisible="False"
                           Fill="Transparent"/>
            </Grid>
        )";
        host = Markup::XamlReader::Load(xaml).as<Controls::Grid>();
        // InsertAt(end), unlike Append, notifies XAML's deferred-element
        // index bookkeeping. Pair this with removal when unhighlighted.
        auto children = panel.Children();
        children.InsertAt(children.Size(), host);
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
    for (auto name : {kGlowContourName, kGlowStackName}) {
        if (auto el = FindChildByName(host, name)) {
            el.Visibility(Visibility::Collapsed);
        }
    }
    if (auto r = FindChildByName(host, kGlowBarName).try_as<Shapes::Rectangle>()) {
        r.Visibility(Visibility::Collapsed);
        r.Stroke(nullptr);
        r.Fill(Media::SolidColorBrush{winrt::Windows::UI::Color{0, 0, 0, 0}});
    }
}

// Reject malformed geometry before arithmetic or assigning XAML properties.
// Coordinates are DIPs relative to the native panel. Never divide by native dimensions.
bool SafeGlowMetric(double value) {
    return std::isfinite(value) && std::abs(value) <= 16384.0;
}
bool SafeGlowBounds(double x, double y, double w, double h) {
    return SafeGlowMetric(x) && SafeGlowMetric(y) && SafeGlowMetric(w) &&
           SafeGlowMetric(h) && w > 0.5 && h > 0.5;
}
double InsetGlowRadius(double radius, double inset, double w, double h) {
    if (!SafeGlowMetric(radius) || radius < 0 || !SafeGlowMetric(inset) ||
        !SafeGlowBounds(0, 0, w, h))
        return 0;
    return (std::max)(0.0, (std::min)(radius - inset, (std::min)(w, h) * 0.5));
}
struct NativeGlowShape {
    double x = 0, y = 0, w = 0, h = 0;
    CornerRadius corners{0, 0, 0, 0};
};
bool ReadNativeGlowShape(FrameworkElement element,
                         FrameworkElement reference,
                         NativeGlowShape& shape) {
    try {
        if (!element || element.Visibility() != Visibility::Visible)
            return false;
        double w = element.ActualWidth(), h = element.ActualHeight();
        if (!SafeGlowBounds(0, 0, w, h))
            return false;
        CornerRadius corners{0, 0, 0, 0};
        if (auto border = element.try_as<Controls::Border>()) {
            corners = border.CornerRadius();
        } else if (auto rect = element.try_as<Shapes::Rectangle>()) {
            double radius = (std::min)(rect.RadiusX(), rect.RadiusY());
            corners = CornerRadius{radius, radius, radius, radius};
        } else {
            return false;  // Unknown shape: use fallback, not a guessed native
                           // contour.
        }
        auto transform = element.TransformToVisual(reference);
        auto origin = transform.TransformPoint({0, 0});
        auto unitX = transform.TransformPoint({1, 0});
        auto unitY = transform.TransformPoint({0, 1});
        double sx = unitX.X - origin.X, sy = unitY.Y - origin.Y;
        // Border radii cannot reproduce rotations, skew or non-uniform scaling.
        if (!SafeGlowMetric(unitX.Y) || !SafeGlowMetric(unitY.X) ||
            !SafeGlowMetric(sx) || !SafeGlowMetric(sy) || sx <= 0 || sy <= 0 ||
            std::abs(unitX.Y - origin.Y) > 0.01 ||
            std::abs(unitY.X - origin.X) > 0.01 || std::abs(sx - sy) > 0.01)
            return false;
        if (!SafeGlowBounds(origin.X, origin.Y, w * sx, h * sy))
            return false;
        shape = {
            origin.X, origin.Y, w * sx, h * sy,
            CornerRadius{corners.TopLeft * sx, corners.TopRight * sx,
                         corners.BottomRight * sx, corners.BottomLeft * sx}};
        return true;
    } catch (...) {
        return false;  // Detached or changing native visual; use the safe
                       // fallback.
    }
}
// What the plates copy from Windows' button, read once per paint:
// the plate (a Styler hover plate or BackgroundElement, else the content
// box) and, on a multi-window button, the MultiWindowElement sliver. Also
// the signature of all of it for the paint cache.
// Margins that place an element of size w x h, centred in its slot and
// moved by (dx, dy), while its desired size is 0: overlay chrome must never
// make a native panel larger than Windows laid it out. Arrange is unchanged
// (the slot minus these margins is w x h larger, and centring adds it back);
// no layout clip, since the element fits.
Thickness ZeroDesiredMargin(double w, double h, double dx, double dy) {
    return Thickness{dx - w * 0.5, dy - h * 0.5, -dx - w * 0.5, -dy - h * 0.5};
}

struct PlateShapes {
    bool ok = false;
    double centerX = 0, centerY = 0;  // panel content centre
    NativeGlowShape main;
    bool fromBackground = false;
    bool hasStack = false;
    NativeGlowShape stack;
    uint64_t signature = 0;
};

PlateShapes ReadPlateShapes(FrameworkElement iconPanel) {
    PlateShapes out;
    // The host may not have been arranged yet. Use the native panel's content
    // center, not TransformToVisual(host), so first paint and later paints
    // agree.
    double panelW = iconPanel.ActualWidth(), panelH = iconPanel.ActualHeight();
    if (!SafeGlowBounds(0, 0, panelW, panelH))
        return out;
    Thickness padding{0, 0, 0, 0};
    if (auto grid = iconPanel.try_as<Controls::Grid>()) {
        auto p = grid.Padding();
        auto b = grid.BorderThickness();
        padding = {p.Left + b.Left, p.Top + b.Top, p.Right + b.Right,
                   p.Bottom + b.Bottom};
    }
    if (!SafeGlowMetric(padding.Left) || !SafeGlowMetric(padding.Top) ||
        !SafeGlowMetric(padding.Right) || !SafeGlowMetric(padding.Bottom))
        return out;
    double contentW = panelW - padding.Left - padding.Right;
    double contentH = panelH - padding.Top - padding.Bottom;
    if (!SafeGlowBounds(0, 0, contentW, contentH))
        return out;
    out.centerX = padding.Left + contentW * 0.5;
    out.centerY = padding.Top + contentH * 0.5;
    NativeGlowShape& shape = out.main;
    auto indicator = FindRunningIndicator(iconPanel);
    bool plate =
        indicator && RunningIndicatorLooksLikeHoverPlate(indicator, iconPanel);
    bool found = false;
    if (plate)
        found = ReadNativeGlowShape(indicator, iconPanel, shape);
    if (!found) {
        found = out.fromBackground = ReadNativeGlowShape(
            FindChildByName(iconPanel, L"BackgroundElement"), iconPanel, shape);
    }
    if (!found) {
        shape.x = padding.Left;
        shape.y = padding.Top;
        shape.w = contentW;
        shape.h = contentH;
        shape.corners = CornerRadius{4, 4, 4, 4};
    }
    if (!SafeGlowBounds(shape.x, shape.y, shape.w, shape.h))
        return out;
    out.hasStack =
        out.fromBackground &&
        ReadNativeGlowShape(FindChildByName(iconPanel, L"MultiWindowElement"),
                            iconPanel, out.stack);
    out.ok = true;

    // FNV-1a over the geometry in 1/16 DIP steps.
    uint64_t h = 1469598103934665603ull;
    auto mix = [&h](double v) {
        const int64_t q = static_cast<int64_t>(std::llround(v * 16.0));
        for (int i = 0; i < 8; ++i) {
            h ^= static_cast<uint64_t>(q >> (i * 8)) & 0xFF;
            h *= 1099511628211ull;
        }
    };
    auto mixShape = [&mix](NativeGlowShape const& s) {
        mix(s.x); mix(s.y); mix(s.w); mix(s.h);
        mix(s.corners.TopLeft); mix(s.corners.TopRight);
        mix(s.corners.BottomRight); mix(s.corners.BottomLeft);
    };
    mix(out.centerX);
    mix(out.centerY);
    mix(plate ? 1 : out.fromBackground ? 2 : 3);
    mixShape(shape);
    mix(out.hasStack ? 1 : 0);
    if (out.hasStack)
        mixShape(out.stack);
    out.signature = h ? h : 1;
    return out;
}

void PaintNativeContour(Controls::Grid host,
                        PlateShapes const& shapes,
                        GlowStyle style,
                        winrt::Windows::UI::Color color,
                        double accentThickness,
                        double opacity,
                        int fillOpacity) {
    if (!shapes.ok)
        return;
    const double centerX = shapes.centerX, centerY = shapes.centerY;
    NativeGlowShape const& shape = shapes.main;

    // Plate with accent: accent border (accent thickness) and half the fill,
    // as the preview "with accent" styles.
    const bool accent = style == GlowStyle::PlateAccent;
    auto strokeColor = color;
    strokeColor.A = 255;
    auto fillColor = color;
    fillColor.A = static_cast<uint8_t>(
        (std::clamp)(fillOpacity, 0, 100) * 2.55 * (accent ? 0.5 : 1.0) + 0.5);
    const double rankOpacity = (std::clamp)(opacity, 0.0, 1.0);

    // Our Border over a native shape: same size and corners, centred on it
    // through balanced margins relative to the panel content centre. Created
    // at `index` in the host (nullopt = on top). nullptr: bad geometry.
    auto paint = [&](PCWSTR name, NativeGlowShape const& s,
                     std::optional<uint32_t> index) -> Controls::Border {
        double w = s.w, h = s.h;
        double dx = s.x + w * 0.5 - centerX;
        double dy = s.y + h * 0.5 - centerY;
        if (!SafeGlowBounds(s.x, s.y, w, h) || !SafeGlowMetric(dx) ||
            !SafeGlowMetric(dy))
            return nullptr;
        Controls::Border b =
            FindChildByName(host, name).try_as<Controls::Border>();
        if (!b) {
            b = Controls::Border();
            b.Name(name);
            b.IsHitTestVisible(false);
            auto children = host.Children();
            children.InsertAt(index ? (std::min)(*index, children.Size())
                                    : children.Size(),
                              b);
        }
        b.HorizontalAlignment(HorizontalAlignment::Center);
        b.VerticalAlignment(VerticalAlignment::Center);
        b.Width(w);
        b.Height(h);
        // Same place as balanced margins {dx, dy, -dx, -dy}, but each pair
        // also subtracts the size, so the desired size is 0 and the plate
        // never sizes the button. With plain margins a plate kept the old
        // BackgroundElement size and held its button big after a Taskbar
        // Styler theme shrank the others; the next
        // repaint read the held size, so the plate cache kept it.
        b.Margin(ZeroDesiredMargin(w, h, dx, dy));
        b.CornerRadius({InsetGlowRadius(s.corners.TopLeft, 0, w, h),
                        InsetGlowRadius(s.corners.TopRight, 0, w, h),
                        InsetGlowRadius(s.corners.BottomRight, 0, w, h),
                        InsetGlowRadius(s.corners.BottomLeft, 0, w, h)});
        double stroke =
            accent ? (std::min)(accentThickness, (std::min)(w, h) * 0.5) : 0;
        b.BorderThickness({stroke, stroke, stroke, stroke});
        if (accent) {
            b.BorderBrush(Media::SolidColorBrush{strokeColor});
        } else {
            b.ClearValue(Controls::Border::BorderBrushProperty());
        }
        b.Background(Media::SolidColorBrush{fillColor});
        b.Opacity(rankOpacity);
        b.ClearValue(UIElement::ClipProperty());
        b.Visibility(Visibility::Visible);
        return b;
    };

    // Flush with the native plate, so it reads as the button's own
    // plate lit up.
    if (!paint(kGlowContourName, shape, std::nullopt))
        return;

    // Multi-window button: Windows narrows BackgroundElement by a
    // right margin of 4 and draws MultiWindowElement (Border, Width 16, right-
    // aligned, above the background) so a sliver sticks out: "(O))". Our
    // plate copies only the background, which left a faint native ")" next to
    // a tinted "(O)". Copy the sliver as well, under our plate, clipped to the
    // part right of the background so the tint does not double where they
    // overlap.
    bool stacked = false;
    if (shapes.hasStack) {
        NativeGlowShape const& stack = shapes.stack;
        const double clipX = shape.x + shape.w - stack.x;
        if (clipX > 0.0 && clipX < stack.w - 0.5) {
            if (auto b = paint(kGlowStackName, stack, 0u)) {
                Media::RectangleGeometry clip;
                clip.Rect({static_cast<float>(clipX), 0.f,
                           static_cast<float>(stack.w - clipX),
                           static_cast<float>(stack.h)});
                b.Clip(clip);
                stacked = true;
            }
        }
    }
    if (!stacked) {
        if (auto b = FindChildByName(host, kGlowStackName)) {
            b.Visibility(Visibility::Collapsed);
        }
    }
}
PCWSTR GlowStyleName(GlowStyle s) {
    return EnumToName(kGlowStyleNames, s);
}

PCWSTR PromoteModeName(PromoteMode m) {
    return EnumToName(kPromoteModeNames, m);
}

// True if this focus change may skip the app min-focus timer.
// Takes g_stateMutex itself; do not call with it held.
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
    return EnumToName(kPreviewStyleNames, s);
}

PCWSTR TaskbarEdgeName(TaskbarEdge e) {
    return EnumToName(kTaskbarEdgeNames, e);
}

PCWSTR BarSideName(BarSide s) {
    return EnumToName(kBarSideNames, s);
}

bool IsInVisualState(FrameworkElement root, PCWSTR stateName) {
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
// previous edge must not win (a left taskbar would get full-cell underlines).
// Do not use "wider than tall ⇒ horizontal": a left-edge button is 48×32.
TaskbarEdge DetectTaskbarEdge(FrameworkElement iconPanel) {
    bool verticalState = IsInVisualState(iconPanel, L"VerticalOrientation");
    bool horizontalState = IsInVisualState(iconPanel, L"HorizontalOrientation");
    try {
        auto parent = Media::VisualTreeHelper::GetParent(iconPanel)
                          .try_as<FrameworkElement>();
        if (parent) {
            verticalState =
                verticalState ||
                IsInVisualState(parent, L"VerticalOrientation");
            horizontalState =
                horizontalState ||
                IsInVisualState(parent, L"HorizontalOrientation");
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

    if (verticalState) {
        return pickVertical();
    }
    if (horizontalState) {
        return pickHorizontal();
    }
    if (panelLooksVertical || tallPill) {
        return pickVertical();
    }
    if (panelLooksHorizontal) {
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

BarSide SideBarForTaskbarEdge(TaskbarEdge edge) {
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
double BarLengthForSide(double boxW,
                        double boxH,
                        BarSide side,
                        double sizeFrac) {
    const bool horizontalBar =
        side == BarSide::Top || side == BarSide::Bottom;
    const double cellAlong = horizontalBar ? boxW : boxH;
    double barLen = cellAlong * sizeFrac;
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
                        double opacity) {
    const double t = thickness;
    const double len = length;
    const double edgePad =
        (side == BarSide::Bottom || side == BarSide::Top) ? 2.0 : 1.0;

    rect.Stroke(nullptr);
    rect.StrokeThickness(0);
    rect.Fill(Media::SolidColorBrush{fill});
    rect.RadiusX(t * 0.5);
    rect.RadiusY(t * 0.5);
    rect.Opacity(opacity);
    rect.IsHitTestVisible(false);
    rect.Visibility(Visibility::Visible);

    // Zero desired size, as the plates (ZeroDesiredMargin): the far margin
    // takes back the bar's thickness and its pad, the centred axis half the
    // length on each side. The placement is the same as before.
    const double back = -(t + edgePad);
    switch (side) {
        case BarSide::Left:
            rect.Width(t);
            rect.Height(len);
            rect.HorizontalAlignment(HorizontalAlignment::Left);
            rect.VerticalAlignment(VerticalAlignment::Center);
            rect.Margin(Thickness{edgePad, -len * 0.5, back, -len * 0.5});
            break;
        case BarSide::Right:
            rect.Width(t);
            rect.Height(len);
            rect.HorizontalAlignment(HorizontalAlignment::Right);
            rect.VerticalAlignment(VerticalAlignment::Center);
            rect.Margin(Thickness{back, -len * 0.5, edgePad, -len * 0.5});
            break;
        case BarSide::Top:
            rect.Width(len);
            rect.Height(t);
            rect.HorizontalAlignment(HorizontalAlignment::Center);
            rect.VerticalAlignment(VerticalAlignment::Top);
            rect.Margin(Thickness{-len * 0.5, edgePad, -len * 0.5, back});
            break;
        case BarSide::Bottom:
        default:
            rect.Width(len);
            rect.Height(t);
            rect.HorizontalAlignment(HorizontalAlignment::Center);
            rect.VerticalAlignment(VerticalAlignment::Bottom);
            rect.Margin(Thickness{-len * 0.5, back, -len * 0.5, edgePad});
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

// Relayout: restore the host placement relative to the native pill / Styler plate.
// Never mutate native collection order or geometry.
void RepositionGlowAfterRelayout(FrameworkElement iconPanel) {
    if (!iconPanel) {
        return;
    }

    auto panel = iconPanel.try_as<Controls::Panel>();
    if (!panel) {
        return;
    }
    try {
        if (auto host = FindChildByName(iconPanel, kGlowElementName)) {
            EnsureGlowHostZOrder(panel, host);
        }
    } catch (...) {
    }
}

// Must run on the panel's dispatcher. Pulls only watches for this thread so
// a primary-taskbar cleanup cannot revoke a secondary bar's tokens.
void RevokeIconPanelLayoutWatchesOnThisDispatcher() {
    // Dispatcher() is XAML. Do not call it while holding the watch mutex.
    std::unordered_map<void*, IconPanelLayoutWatch> taken;
    {
        std::lock_guard<std::mutex> lock(g_layoutWatchMutex);
        taken.swap(g_layoutWatches);
    }
    std::unordered_map<void*, IconPanelLayoutWatch> keep;
    std::vector<IconPanelLayoutWatch> mine;
    for (auto& [id, watch] : taken) {
        bool onThisThread = false;
        try {
            auto panel = watch.panel.get();
            if (!panel) {
                onThisThread = true;
            } else {
                auto dispatcher = panel.Dispatcher();
                onThisThread = dispatcher && dispatcher.HasThreadAccess();
            }
        } catch (...) {
            onThisThread = true;
        }
        if (onThisThread) {
            mine.push_back(std::move(watch));
        } else {
            keep.emplace(id, std::move(watch));
        }
    }
    if (!keep.empty()) {
        std::lock_guard<std::mutex> lock(g_layoutWatchMutex);
        for (auto& [id, watch] : keep) {
            auto panelRef = watch.panel;
            auto token = watch.sizeChanged;
            if (!g_layoutWatches.emplace(id, std::move(watch)).second) {
                try {
                    if (auto panel = panelRef.get()) {
                        if (token) {
                            panel.SizeChanged(token);
                        }
                    }
                } catch (...) {
                    Wh_Log(L"ERROR: SizeChanged revoke failed");
                }
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

TaskbarEdge CachedTaskbarEdge(FrameworkElement iconPanel) {
    void* id = InspectableIdentity(iconPanel);
    {
        std::lock_guard<std::mutex> lock(g_layoutWatchMutex);
        auto it = id ? g_layoutWatches.find(id) : g_layoutWatches.end();
        if (it != g_layoutWatches.end()) {
            if (!WeakIsSameElement(it->second.panel, iconPanel)) {
                g_layoutWatches.erase(it);
            } else if (it->second.haveEdge) {
                return it->second.lastEdge;
            }
        }
    }
    const TaskbarEdge edge = DetectTaskbarEdge(iconPanel);
    if (id) {
        std::lock_guard<std::mutex> lock(g_layoutWatchMutex);
        auto it = g_layoutWatches.find(id);
        if (it != g_layoutWatches.end() &&
            WeakIsSameElement(it->second.panel, iconPanel)) {
            it->second.lastEdge = edge;
            it->second.haveEdge = true;
        }
    }
    return edge;
}

// Stores the edge for this panel's watch. The edge belongs to the taskbar,
// not the button, so a full pass detects it once and passes it in.
void RefreshCachedTaskbarEdge(FrameworkElement iconPanel,
                              std::optional<TaskbarEdge>& passEdge) {
    if (!iconPanel) {
        return;
    }
    if (!passEdge) {
        passEdge = DetectTaskbarEdge(iconPanel);
    }
    const TaskbarEdge edge = *passEdge;
    void* id = InspectableIdentity(iconPanel);
    if (!id) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_layoutWatchMutex);
    auto it = g_layoutWatches.find(id);
    if (it != g_layoutWatches.end() &&
        WeakIsSameElement(it->second.panel, iconPanel)) {
        it->second.lastEdge = edge;
        it->second.haveEdge = true;
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
    if (!RememberUiDispatcher(iconPanel)) {
        return;
    }

    void* id = InspectableIdentity(iconPanel);
    if (!id) {
        return;
    }
    {
        std::lock_guard<std::mutex> lock(g_layoutWatchMutex);
        auto it = g_layoutWatches.find(id);
        if (it != g_layoutWatches.end()) {
            if (WeakIsSameElement(it->second.panel, iconPanel)) {
                return;
            }
            g_layoutWatches.erase(it);
        }
    }

    const TaskbarEdge edge = DetectTaskbarEdge(iconPanel);

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
                const TaskbarEdge edgeNow = DetectTaskbarEdge(panel);
                if (void* pid = InspectableIdentity(panel)) {
                    std::lock_guard<std::mutex> lock(g_layoutWatchMutex);
                    auto it = g_layoutWatches.find(pid);
                    if (it != g_layoutWatches.end()) {
                        it->second.lastEdge = edgeNow;
                        it->second.haveEdge = true;
                    }
                }
                ++g_iconPanelRelayoutDepth;
                try {
                    RepositionGlowAfterRelayout(panel);
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
    for (auto it = g_layoutWatches.begin(); it != g_layoutWatches.end();) {
        try {
            if (!it->second.panel.get()) {
                it = g_layoutWatches.erase(it);
                continue;
            }
        } catch (...) {
            it = g_layoutWatches.erase(it);
            continue;
        }
        ++it;
    }
    g_layoutWatches[id] = std::move(watch);
}

void ApplyButtonHighlight(FrameworkElement button, int rankOneBased) {
    if (!button) {
        return;
    }

    auto settings = SettingsSnap();
    if (rankOneBased <= 0 || g_unloading.load()) {
        ClearButtonHighlight(button);
        return;
    }

    try {
        auto iconPanel = GetIconPanel(button);
        if (!iconPanel) {
            return;
        }
        EnsureIconPanelLayoutWatch(button);

        auto panel = iconPanel.try_as<Controls::Panel>();
        if (!panel) {
            return;
        }

        double panelW = iconPanel.ActualWidth();
        double panelH = iconPanel.ActualHeight();
        if (!SafeGlowMetric(panelW) || !(panelW > 1.0)) {
            panelW = 44.0;
        }
        if (!SafeGlowMetric(panelH) || !(panelH > 1.0)) {
            panelH = 44.0;
        }
        const TaskbarEdge edge = CachedTaskbarEdge(iconPanel);
        auto existingHost = FindChildByName(iconPanel, kGlowElementName);
        double keyW = panelW;
        double keyH = panelH;
        GlowContentBoxSize(existingHost, iconPanel, panelW, panelH, keyW, keyH);
        if (!SafeGlowBounds(0, 0, keyW, keyH)) { keyW = panelW; keyH = panelH; }
        const int boxWi = static_cast<int>(keyW + 0.5);
        const int boxHi = static_cast<int>(keyH + 0.5);
        // Plates: Windows' button shapes, read once (cache key and paint).
        const bool plateStyle = settings->glowStyle == GlowStyle::Plate ||
                                settings->glowStyle == GlowStyle::PlateAccent;
        PlateShapes plateShapes;
        if (plateStyle) {
            plateShapes = ReadPlateShapes(iconPanel);
        }
        {
            // Skip an unchanged repaint. Explorer re-runs the button's
            // visual states on every title change (an image viewer paging
            // images: 8-16 a second). The plates also
            // compare Windows' shapes, which a window-count or hover change
            // alters without changing the panel size.
            auto painted = GetCachedPaintState(button);
            const bool sameGeometry =
                !plateStyle || (plateShapes.ok &&
                                painted.plateGeometry == plateShapes.signature);
            if (sameGeometry && painted.rank == rankOneBased &&
                painted.settingsGen == settings->generation &&
                painted.accent ==
                    g_cachedAccent.load(std::memory_order_relaxed) &&
                painted.edge == edge && painted.boxW == boxWi &&
                painted.boxH == boxHi && existingHost) {
                EnsureGlowHostZOrder(panel, existingHost);
                return;
            }
        }

        const int rankIdx = rankOneBased - 1;
        const int intensity = RankIntensity(settings->glowIntensity, rankIdx);
        const double t = intensity / 100.0;

        winrt::Windows::UI::Color base = ResolveGlowBaseColor(*settings);

        auto withAlpha = [](winrt::Windows::UI::Color c,
                            int a) -> winrt::Windows::UI::Color {
            c.A = static_cast<uint8_t>((std::max)(0, (std::min)(255, a)));
            return c;
        };

        const double thickness = static_cast<double>(
            (std::max)(1, (std::min)(16, settings->glowThickness)));
        const double sizeFrac =
            (std::max)(40, (std::min)(100, settings->glowSize)) / 100.0;
        const GlowStyle style = settings->glowStyle;
        const int fillOpacitySetting =
            (std::max)(0, (std::min)(100, settings->glowFillOpacity));

        Controls::Grid host = EnsureGlowHost(panel, iconPanel);
        if (!host) {
            return;
        }

        double boxW = panelW;
        double boxH = panelH;
        GlowContentBoxSize(host, iconPanel, panelW, panelH, boxW, boxH);

        const BarSide barSide = SideBarForTaskbarEdge(edge);

        // Position only our host; native children keep their relative order.
        EnsureGlowHostZOrder(panel, host);

        HideAllGlowLayers(host);

        if (t <= 0.0) {
            // With the plate signature, or Plate styles at intensity 0 missed
            // the cache on every UpdateVisualStates.
            SetCachedPaintState(button, rankOneBased, settings->generation,
                                edge, boxWi, boxHi,
                                plateStyle && plateShapes.ok
                                    ? plateShapes.signature
                                    : 0);
            return;
        }

        if (style == GlowStyle::Plate || style == GlowStyle::PlateAccent) {
            PaintNativeContour(host, plateShapes, style, base,
                               settings->glowAccentThickness, t,
                               fillOpacitySetting);
        } else {
            // Side bar: left of the icon on a bottom/top taskbar, under the
            // icon on a left/right taskbar — never the native-pill edge.
            const double barT = thickness + 1.0;
            const double barLen =
                BarLengthForSide(boxW, boxH, barSide, sizeFrac);
            const int fillA =
                static_cast<int>(fillOpacitySetting * 2.55 + 0.5);
            if (auto rect = FindChildByName(host, kGlowBarName)
                                .try_as<Shapes::Rectangle>()) {
                StyleGlowBarOnSide(rect, withAlpha(base, fillA), barT, barLen,
                                   barSide, t);
            }
        }

        Wh_Log(L"Glow rank %d \"%s\" style=%s edge=%s bar=%s box=%.0fx%.0f "
               L"th=%.0f size=%d%% intensity=%d",
               rankOneBased, GetButtonAutomationName(button).c_str(),
               GlowStyleName(style), TaskbarEdgeName(edge),
               BarSideName(barSide), boxW, boxH, thickness,
               settings->glowSize, intensity);
        SetCachedPaintState(button, rankOneBased, settings->generation, edge,
                            boxWi, boxHi,
                            plateStyle && plateShapes.ok
                                ? plateShapes.signature
                                : 0);
    } catch (...) {
        HRESULT hr = winrt::to_hresult();
        Wh_Log(L"ApplyButtonHighlight error %08X", hr);
    }
}

void PruneTrackedButtons_UIThread() {
    std::lock_guard<std::mutex> lock(g_buttonsMutex);
    for (auto it = g_trackedButtons.begin(); it != g_trackedButtons.end();) {
        try {
            if (!it->second.get()) {
                it = g_trackedButtons.erase(it);
            } else {
                ++it;
            }
        } catch (...) {
            it = g_trackedButtons.erase(it);
        }
    }
}

// A destroyed TaskListButton often never reports IsRunning=false.
// AppAppearsOnTaskbar runs off this thread and must not weak.get().
// Weak refs are resolved outside g_buttonStateMutex: resolving touches every
// cached XAML object, and after an idle period (pages trimmed) that once held
// the lock for 14 ms while the worker may need it.
void PruneDeadButtonStates_UIThread() {
    std::vector<std::pair<void*, winrt::weak_ref<FrameworkElement>>> entries;
    {
        std::lock_guard<std::mutex> lock(g_buttonStateMutex);
        entries.reserve(g_buttonStates.size());
        for (const auto& [id, e] : g_buttonStates) {
            entries.emplace_back(id, e.button);
        }
    }
    std::vector<void*> dead;
    for (const auto& [id, weak] : entries) {
        bool alive = false;
        try {
            alive = static_cast<bool>(weak.get());
        } catch (...) {
        }
        if (!alive) {
            dead.push_back(id);
        }
    }
    if (dead.empty()) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_buttonStateMutex);
    for (void* id : dead) {
        auto it = g_buttonStates.find(id);
        if (it == g_buttonStates.end()) {
            continue;
        }
        bool alive = false;
        try {
            alive = static_cast<bool>(it->second.button.get());
        } catch (...) {
        }
        if (!alive) {  // re-check: the slot may have been reused meanwhile
            g_buttonStates.erase(it);
        }
    }
}

void TrackButton_UIThread(FrameworkElement button) {
    if (!button) {
        return;
    }

    // Hover-hot (every UpdateVisualStates): a thread has one dispatcher, and
    // a known button keeps its weak ref unless Explorer reused the address.
    static thread_local bool dispatcherRemembered = false;
    if (!dispatcherRemembered) {
        dispatcherRemembered = RememberUiDispatcher(button);
    }
    if (void* id = InspectableIdentity(button)) {
        std::lock_guard<std::mutex> lock(g_buttonsMutex);
        auto it = g_trackedButtons.find(id);
        if (it == g_trackedButtons.end()) {
            g_trackedButtons.emplace(id, winrt::make_weak(button));
        } else if (!WeakIsSameElement(it->second, button)) {
            it->second = winrt::make_weak(button);
        }
    }
}

// ---------------------------------------------------------------------------
// Button → task item / task group — adapted from taskbar-volume-control-per-app
// ---------------------------------------------------------------------------

thread_local bool g_captureTaskGroup = false;
thread_local void* g_capturedTaskGroup = nullptr;

using TryGetItemFromContainer_TaskListWindowViewModel_t =
    void*(WINAPI*)(void** output, UIElement* container);
TryGetItemFromContainer_TaskListWindowViewModel_t
    TryGetItemFromContainer_TaskListWindowViewModel_Original;

// ---------------------------------------------------------------------------
// WindowsUdk.UI.Shell public interfaces: task groups and task items
// ---------------------------------------------------------------------------
// Layouts from windowsudk.winmd (C:\Windows\SystemApps\
// MicrosoftWindows.UndockedDevKit_cw5n1h2txyewy, build 26100-era). Only the
// slots up to the last method we call are declared;
// earlier slots are placeholders whose parameter types do not matter because
// we never call them. A changed interface has a new IID, so QueryInterface
// fails and callers fall back.

constexpr GUID IID_Wudk_ITaskItem = {
    0xB081D9D6, 0x9B45, 0x5363, {0x8A, 0x4D, 0x85, 0x4A, 0xDD, 0x6A, 0xBE, 0x7E}};
constexpr GUID IID_Wudk_ITaskGroup = {
    0xA152E779, 0x93DF, 0x5BE2, {0xAF, 0x2C, 0xD3, 0x42, 0x45, 0x2B, 0x0C, 0xE0}};

struct WudkITaskItemAbi : ::IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_AppId(HSTRING* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_UniqueId(UINT64* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_WindowId(UINT64* value) = 0;
};

struct WudkITaskGroupAbi : ::IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_AppId(HSTRING* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_UniqueId(void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DisplayName(void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_AccessibleName(void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Icon(void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_OverlayIcon(void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_OverlayIconDescription(void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Badge(void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_IsRequestingAttention(void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Progress(void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_VisualState(void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_VisualState(void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Bounds(void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Bounds(void*) = 0;
    // IVectorView<WindowsUdk.UI.Shell.TaskItem>
    virtual HRESULT STDMETHODCALLTYPE get_Items(::IInspectable** value) = 0;
};

// Windows.Foundation.Collections.IVectorView<T> for a runtime-class T: the
// element comes back as its default interface pointer. The parameterized IID
// is not needed: get_Items already returns this interface.
struct WudkIVectorViewAbi : ::IInspectable {
    virtual HRESULT STDMETHODCALLTYPE GetAt(UINT32 index, ::IInspectable** item) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Size(UINT32* size) = 0;
};

HWND HwndFromUdkTaskItem(::IUnknown* taskItem) {
    if (!taskItem) {
        return nullptr;
    }
    winrt::com_ptr<WudkITaskItemAbi> item;
    UINT64 windowId = 0;
    if (FAILED(taskItem->QueryInterface(IID_Wudk_ITaskItem, item.put_void())) ||
        FAILED(item->get_WindowId(&windowId))) {
        return nullptr;
    }
    HWND hwnd = reinterpret_cast<HWND>(windowId);
    return IsWindow(hwnd) ? hwnd : nullptr;
}

// Explorer's AppId (the key it groups taskbar buttons by): ITaskItem /
// ITaskGroup get_AppId, slot 0 of both public interfaces. Upper case.
std::wstring UdkTaskItemAppId(::IUnknown* taskItem) {
    if (!taskItem) {
        return {};
    }
    try {
        winrt::com_ptr<WudkITaskItemAbi> item;
        winrt::hstring id;
        if (FAILED(taskItem->QueryInterface(IID_Wudk_ITaskItem,
                                            item.put_void())) ||
            FAILED(item->get_AppId(reinterpret_cast<HSTRING*>(
                winrt::put_abi(id))))) {
            return {};
        }
        return ToUpper(std::wstring(id));
    } catch (...) {
        return {};
    }
}

std::wstring UdkTaskGroupAppId(void* udkTaskGroup) {
    if (!udkTaskGroup) {
        return {};
    }
    try {
        winrt::com_ptr<WudkITaskGroupAbi> group;
        winrt::hstring id;
        if (FAILED(static_cast<::IUnknown*>(udkTaskGroup)->QueryInterface(
                IID_Wudk_ITaskGroup, group.put_void())) ||
            FAILED(group->get_AppId(reinterpret_cast<HSTRING*>(
                winrt::put_abi(id))))) {
            return {};
        }
        return ToUpper(std::wstring(id));
    } catch (...) {
        return {};
    }
}

// Public replacement for CTaskGroup's private item array: the group's
// Items -> each item's WindowId. Returns false if the interfaces are not
// available (caller keeps the old path).
bool CollectUdkGroupWindows(void* udkTaskGroup, std::vector<HWND>& out) {
    out.clear();
    if (!udkTaskGroup) {
        return false;
    }
    try {
        winrt::com_ptr<WudkITaskGroupAbi> group;
        if (FAILED(static_cast<::IUnknown*>(udkTaskGroup)->QueryInterface(
                IID_Wudk_ITaskGroup, group.put_void()))) {
            return false;
        }
        winrt::com_ptr<::IInspectable> itemsObject;
        if (FAILED(group->get_Items(itemsObject.put())) || !itemsObject) {
            return false;
        }
        auto* items = reinterpret_cast<WudkIVectorViewAbi*>(itemsObject.get());
        UINT32 size = 0;
        if (FAILED(items->get_Size(&size))) {
            return false;
        }
        for (UINT32 i = 0; i < size && i < 256; ++i) {
            winrt::com_ptr<::IInspectable> element;
            if (FAILED(items->GetAt(i, element.put())) || !element) {
                continue;
            }
            if (HWND hwnd = HwndFromUdkTaskItem(element.get())) {
                if (std::find(out.begin(), out.end(), hwnd) == out.end()) {
                    out.push_back(hwnd);
                }
            }
        }
        return true;
    } catch (...) {
        out.clear();
        return false;
    }
}

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

// Returns the button's WindowsUdk TaskGroup with its own reference. The
// capture flag is reset by a scope guard, so an exception cannot leave
// ITaskGroup::IsRunning answering false for Explorer and other mods on this
// thread.
winrt::com_ptr<IUnknown> GetWindowsUdkTaskGroupFromTaskListButton(
    UIElement element) {
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
    struct CaptureScope {
        CaptureScope() {
            g_capturedTaskGroup = nullptr;
            g_captureTaskGroup = true;
        }
        ~CaptureScope() { g_captureTaskGroup = false; }
    };
    winrt::com_ptr<IUnknown> group;
    {
        CaptureScope capture;
        TaskListGroupViewModel_IsMultiWindow_Original(
            (void**)groupViewModel.get() - 1);
    }
    // AddRef while the view model (which holds the group) is still alive.
    group.copy_from(static_cast<IUnknown*>(g_capturedTaskGroup));
    g_capturedTaskGroup = nullptr;
    // If the IsMultiWindow -> ITaskGroup::IsRunning path changes in
    // Explorer, no group is ever captured and a grouped app's second window
    // is never ranked. Say so once. Single misses are
    // normal (some view models do not ask their group), so only
    // many misses with no capture at all count.
    static std::atomic<unsigned> captureHits{0}, captureMisses{0};
    static std::atomic<bool> loggedCaptureMiss{false};
    if (group) {
        ++captureHits;
    } else if (++captureMisses >= 20 && captureHits.load() == 0 &&
               !loggedCaptureMiss.exchange(true)) {
        Wh_Log(L"WARNING: task group capture never succeeded (%u misses): "
               L"IsMultiWindow / ITaskGroup::IsRunning changed?",
               captureMisses.load());
    }
    return group;
}

// Synchronous snapshot of the button's Explorer AppId and window list (UI
// thread, a few COM calls).
ButtonCapture CaptureButton(FrameworkElement button) {
    ButtonCapture data;
    data.buttonId = InspectableIdentity(button);
    data.running = TaskListButton_IsRunning(button);
    DWORD pid = 0;
    HWND hwnd = nullptr;
    std::vector<CapturedWindow> groupWindows;
    auto rememberWindow = [&](HWND h) {
        if (!h || !IsWindow(h)) {
            return;
        }
        for (const auto& g : groupWindows) {
            if (g.hwnd == h) {
                return;
            }
        }
        if (const DWORD windowPid = PidFromHwnd(h)) {
            groupWindows.push_back(CapturedWindow{h, windowPid});
        }
    };
    std::vector<HWND> udkWindows;
    try {
        UIElement el = button.as<UIElement>();
        // Public path: the button's TaskItem (single window) and its
        // TaskGroup's Items, each -> ITaskItem.WindowId. UWP gives the
        // ApplicationFrameWindow, the HWND focus tracking uses.
        auto udkItem = GetWindowsUdkTaskItemFromTaskListButton(el);
        HWND ownWindow = nullptr;
        std::wstring itemAppId = UdkTaskItemAppId(udkItem.get());
        std::wstring groupAppId;
        if (udkItem) {
            if (HWND h = HwndFromUdkTaskItem(udkItem.get())) {
                ownWindow = h;
                udkWindows.push_back(h);
            }
        }
        const winrt::com_ptr<IUnknown> udkGroup =
            GetWindowsUdkTaskGroupFromTaskListButton(el);
        if (void* windowsUdkTaskGroup = udkGroup.get()) {
            groupAppId = UdkTaskGroupAppId(windowsUdkTaskGroup);
            std::vector<HWND> groupHwnds;
            if (CollectUdkGroupWindows(windowsUdkTaskGroup, groupHwnds)) {
                for (HWND h : groupHwnds) {
                    if (std::find(udkWindows.begin(), udkWindows.end(), h) ==
                        udkWindows.end()) {
                        udkWindows.push_back(h);
                    }
                }
            }
        }
        // Uncombined button (Never combine / combine when full): it stands
        // for one window. Explorer marks it with AutomationId
        // "Window: 0x<hwnd>" (combined buttons: "Appid: …") and binds it to
        // that window's own TaskItem, without a group view model. Require
        // both to agree.
        bool uncombined = false;
        if (HWND idWindow = GetButtonAutomationWindow(button)) {
            if (ownWindow ? ownWindow == idWindow
                          : std::find(udkWindows.begin(), udkWindows.end(),
                                      idWindow) != udkWindows.end()) {
                ownWindow = idWindow;
                uncombined = true;
            }
        }
        {
            bool changed = false;
            std::lock_guard<std::mutex> lock(g_uncombinedMutex);
            if (uncombined) {
                changed = g_uncombinedWindowsByThread[GetCurrentThreadId()]
                              .insert(ownWindow)
                              .second;
            } else if (ownWindow) {
                changed = g_uncombinedWindowsByThread[GetCurrentThreadId()]
                              .erase(ownWindow) > 0;
            }
            if (changed) {
                Wh_Log(L"Button windows: button=%p own=%p -> %s",
                       data.buttonId, ownWindow,
                       uncombined ? L"uncombined (own slot)" : L"combined");
            }
        }
        if (uncombined) {
            udkWindows.assign(1, ownWindow);
        }
        data.uncombined = uncombined;
        data.explorerAppId = !groupAppId.empty() ? groupAppId : itemAppId;
        if (!groupAppId.empty() && !itemAppId.empty() &&
            groupAppId != itemAppId) {
            Wh_Log(L"AppId shadow: button=%p group AppId %s != item AppId %s",
                   data.buttonId, groupAppId.c_str(), itemAppId.c_str());
        }
        PublishWindowAppIds(udkWindows, data.explorerAppId);
        if (!udkWindows.empty()) {
            for (HWND h : udkWindows) {
                rememberWindow(h);
            }
            if (!groupWindows.empty()) {
                hwnd = groupWindows.front().hwnd;
                GetWindowThreadProcessId(hwnd, &pid);
            }
        }
    } catch (...) {
        pid = 0;
        hwnd = nullptr;
        groupWindows.clear();
    }

    data.hwnd = hwnd;
    data.pid = hwnd ? PidFromHwnd(hwnd) : pid;
    data.windows = std::move(groupWindows);
    return data;
}

// Button identity = Explorer's AppId (the key Explorer groups taskbar
// buttons by) plus the button's window set, read synchronously on the UI
// thread from the button's public WindowsUdk task item / group (a few COM
// calls, ~10 us). A
// changed AppId or window set invalidates the cached paint rank; a button
// Explorer recycles for another app simply reports the new AppId.
std::wstring EnsureButtonIdentity(FrameworkElement button) {
    if (!button || !g_taskbandResolveReady.load() || g_unloading.load()) {
        return {};
    }
    void* id = InspectableIdentity(button);
    if (!id) {
        return {};
    }
    ButtonCapture data = CaptureButton(button);
    std::lock_guard<std::mutex> lock(g_buttonStateMutex);
    ButtonState* state = FindButtonStateLocked(button, true);
    if (!state) {
        return {};
    }
    auto& e = *state;
    const bool windowsChanged =
        e.groupWindows.size() != data.windows.size() ||
        !std::equal(e.groupWindows.begin(), e.groupWindows.end(),
                    data.windows.begin(),
                    [](const CapturedWindow& a, const CapturedWindow& b) {
                        return a.hwnd == b.hwnd && a.pid == b.pid;
                    });
    if (e.appIdUpper != data.explorerAppId) {
        Wh_Log(L"Button identity: button=%p appId=%s (was %s) windows=%zu "
               L"running=%d",
               id, data.explorerAppId.c_str(), e.appIdUpper.c_str(),
               data.windows.size(), data.running);
        e.lastPaintRank = -1;
    } else if (windowsChanged) {
        e.lastPaintRank = -1;
    }
    if (e.uncombined != data.uncombined) {
        e.lastPaintRank = -1;
    }
    e.appIdUpper = data.explorerAppId;
    e.uncombined = data.uncombined;
    e.sampleHwnd = data.hwnd;
    e.samplePid = data.pid;
    e.groupWindows = std::move(data.windows);
    return e.appIdUpper;
}

ButtonIdentity GetCachedButtonIdentity(FrameworkElement button) {
    ButtonIdentity out;
    if (!button) {
        return out;
    }
    std::lock_guard<std::mutex> lock(g_buttonStateMutex);
    const ButtonState* state = FindButtonStateLocked(button, false);
    if (!state) {
        return out;
    }
    const auto& e = *state;
    out.appIdUpper = e.appIdUpper;
    out.uncombined = e.uncombined;
    out.sampleHwnd = e.sampleHwnd;
    out.samplePid = e.samplePid;
    out.groupWindows = e.groupWindows;
    return out;
}

PaintCacheState GetCachedPaintState(FrameworkElement button) {
    PaintCacheState out;
    if (!button) {
        return out;
    }
    std::lock_guard<std::mutex> lock(g_buttonStateMutex);
    const ButtonState* e = FindButtonStateLocked(button, false);
    if (!e) {
        return out;
    }
    out.rank = e->lastPaintRank;
    out.settingsGen = e->lastPaintSettingsGen;
    out.accent = e->lastPaintAccent;
    out.edge = e->lastPaintEdge;
    out.boxW = e->lastPaintBoxW;
    out.boxH = e->lastPaintBoxH;
    out.plateGeometry = e->lastPaintPlateGeometry;
    return out;
}

void SetCachedPaintState(FrameworkElement button,
                         int rank,
                         uint32_t gen,
                         TaskbarEdge edge,
                         int boxW,
                         int boxH,
                         uint64_t plateGeometry) {
    const uint32_t accent = g_cachedAccent.load(std::memory_order_relaxed);
    std::lock_guard<std::mutex> lock(g_buttonStateMutex);
    ButtonState* e = FindButtonStateLocked(button, true);
    if (!e) {
        return;
    }
    e->lastPaintRank = rank;
    e->lastPaintSettingsGen = gen;
    e->lastPaintAccent = accent;
    e->lastPaintEdge = edge;
    e->lastPaintBoxW = boxW;
    e->lastPaintBoxH = boxH;
    e->lastPaintPlateGeometry = plateGeometry;
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

// ItemsRepeater keeps recycled TaskListButtons parented, parked off-screen
// (about -10000,-10000), with their last AutomationId / name. Their cached
// identity can still match a rank and must not bind it. GetElementIndex is
// -1 for an element the repeater does not currently show. No repeater
// ancestor (unknown layout): treat as shown.
bool IsRealizedTaskbarButton(FrameworkElement const& button) {
    try {
        DependencyObject child = button;
        DependencyObject cur = Media::VisualTreeHelper::GetParent(button);
        for (int depth = 0; cur && depth < 8; ++depth) {
            if (auto repeater =
                    cur.try_as<winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater>()) {
                auto element = child.try_as<UIElement>();
                return element && repeater.GetElementIndex(element) >= 0;
            }
            child = cur;
            cur = Media::VisualTreeHelper::GetParent(cur);
        }
    } catch (...) {
    }
    return true;
}

// Explorer reports a TaskListButton to the mod only when that button's
// visual state changes, so after loading into a running Explorer most
// buttons were unknown until hovered. The first button
// seen on a taskbar seeds every button its ItemsRepeater currently shows.
// Once per repeater; buttons realized later
// report themselves. Returns true if new buttons were tracked.
winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater ButtonRepeater(
    FrameworkElement const& button) {
    try {
        DependencyObject cur = Media::VisualTreeHelper::GetParent(button);
        for (int depth = 0; cur && depth < 8; ++depth) {
            if (auto repeater = cur.try_as<
                    winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater>()) {
                return repeater;
            }
            cur = Media::VisualTreeHelper::GetParent(cur);
        }
    } catch (...) {
    }
    return nullptr;
}

// Tracks every TaskListButton the repeater currently shows. Returns the
// number tracked.
int SeedRepeater_UIThread(
    winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater const& repeater) {
    int tracked = 0;
    try {
        auto source = repeater.ItemsSourceView();
        const int count = source ? source.Count() : 0;
        for (int i = 0; i < count; ++i) {
            auto el = repeater.TryGetElement(i).try_as<FrameworkElement>();
            if (!el) {
                continue;
            }
            FrameworkElement btn =
                el.Name() == L"TaskListButton"
                    ? el
                    : FindDescendantByName(el, L"TaskListButton");
            if (btn) {
                TrackButton_UIThread(btn);
                ++tracked;
            }
        }
    } catch (...) {
    }
    return tracked;
}

// Per UI thread. Heap-owning thread_locals are emptied by the unload
// cleanup on their thread (ReleaseUiThreadLocals_UIThread): their
// destructors would run only at thread exit, after the module is gone.
thread_local std::unordered_set<void*> t_seededRepeaters;
thread_local std::wstring t_lastLadderLog;

bool SeedButtonsFromRepeater_UIThread(FrameworkElement const& button) {
    auto repeater = ButtonRepeater(button);
    if (!repeater ||
        !t_seededRepeaters.insert(InspectableIdentity(repeater)).second) {
        return false;
    }
    const int tracked = SeedRepeater_UIThread(repeater);
    Wh_Log(L"Seeded %d taskbar buttons from their repeater", tracked);
    return tracked > 0;
}

std::vector<FrameworkElement> CollectLiveButtonsOnThisDispatcher() {
    PruneTrackedButtons_UIThread();
    PruneDeadButtonStates_UIThread();
    std::vector<winrt::weak_ref<FrameworkElement>> buttons;
    {
        std::lock_guard<std::mutex> lock(g_buttonsMutex);
        buttons.reserve(g_trackedButtons.size());
        for (auto& [id, weak] : g_trackedButtons) {
            buttons.push_back(weak);
        }
    }
    std::vector<FrameworkElement> live;
    live.reserve(buttons.size());
    ForEachLiveElementOnThisDispatcher(
        buttons, [&](FrameworkElement b) { live.push_back(b); });
    return live;
}

// A button Explorer recycled (parked off-screen): it shows nothing, so it
// stops counting as running for ranking. Returns its last painted rank
// (0 = already clear) and marks it clear.
int ParkButtonState(FrameworkElement button) {
    std::lock_guard<std::mutex> lock(g_buttonStateMutex);
    ButtonState* state = FindButtonStateLocked(button, false);
    if (!state) {
        return -1;
    }
    auto& e = *state;
    e.observedRunning = false;
    e.lastRunningTick = 0;
    const int prior = e.lastPaintRank;
    e.lastPaintRank = 0;
    return prior;
}

void ApplyAllHighlights_UIThread() {
    g_lastFullRefreshTick = GetTickCount64();

    // Re-seed after a desktop switch or an unknown focused window: that
    // desktop's buttons may be newly realized and not have reported yet.
    {
        static thread_local unsigned seenGeneration = 0;
        const unsigned gen = g_buttonSeedGeneration.load();
        if (gen != seenGeneration) {
            seenGeneration = gen;
            std::unordered_set<void*> repeaters;  // one walk per taskbar
            for (auto& b : CollectLiveButtonsOnThisDispatcher()) {
                auto repeater = ButtonRepeater(b);
                if (repeater &&
                    repeaters.insert(InspectableIdentity(repeater)).second) {
                    SeedRepeater_UIThread(repeater);
                }
            }
        }
    }
    std::vector<FrameworkElement> live = CollectLiveButtonsOnThisDispatcher();
    size_t parked = 0;
    live.erase(std::remove_if(live.begin(), live.end(),
                              [&](FrameworkElement const& b) {
                                  if (IsRealizedTaskbarButton(b)) {
                                      return false;
                                  }
                                  // Parked: no longer running for ranking,
                                  // and cleared once (not every pass).
                                  if (ParkButtonState(b) != 0) {
                                      ClearButtonHighlight(b);
                                  }
                                  ++parked;
                                  return true;
                              }),
               live.end());

    // Snapshot IsRunning before eligibility, also with no ranks: otherwise
    // lastRunningTick / observedRunning go stale and idle decay and
    // desktop-switch clears cannot recover.
    std::vector<std::wstring> buttonAppIds(live.size());
    std::vector<ButtonIdentity> idents(live.size());
    std::vector<char> running(live.size(), 0);
    // Bottom <-> top moves keep the panel size (no SizeChanged), so the pass
    // refreshes the edge; once per pass, not per button.
    std::optional<TaskbarEdge> passEdge;
    for (size_t bi = 0; bi < live.size(); ++bi) {
        if (auto ip = GetIconPanel(live[bi])) {
            RefreshCachedTaskbarEdge(ip, passEdge);
        }
        buttonAppIds[bi] = EnsureButtonIdentity(live[bi]);
        running[bi] = ButtonCountsAsRunning(live[bi]) ? 1 : 0;
        if (running[bi]) {
            idents[bi] = GetCachedButtonIdentity(live[bi]);
        }
    }
    if (t_runningReread.running || t_runningReread.stopped) {
        Wh_Log(L"Running state read: %u running, %u not running (new "
               L"buttons, or after a desktop / lock switch)",
               t_runningReread.running, t_runningReread.stopped);
        t_runningReread = {};
    }
    // The shown buttons are authoritative for which windows have their own
    // slot: when Never Combine is switched off, no button is uncombined any
    // more and per-window ranks merge back into one app rank.
    // Captures add windows as they appear; this pass also drops them.
    {
        std::unordered_set<HWND, HwndHash> shownUncombined;
        for (size_t bi = 0; bi < live.size(); ++bi) {
            if (running[bi] && idents[bi].uncombined && idents[bi].sampleHwnd) {
                shownUncombined.insert(idents[bi].sampleHwnd);
            }
        }
        std::lock_guard<std::mutex> lock(g_uncombinedMutex);
        auto& mine = g_uncombinedWindowsByThread[GetCurrentThreadId()];
        if (shownUncombined != mine) {
            Wh_Log(L"Uncombined windows (this taskbar): %zu -> %zu",
                   mine.size(), shownUncombined.size());
            mine = std::move(shownUncombined);
        }
    }

    std::vector<AppFocusInfo> ranks;
    GUID deskId{};
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        RecomputeRanksForDesktopLocked(CurrentDeskLocked());
        ranks = CurrentDeskLocked().rankedApps;
        deskId = g_currentDesktopId;
    }

    // Ladder and bindings are logged only when they differ from this UI
    // thread's last pass (most passes repeat it). Pass 1 matches, pass 2
    // paints, so a
    // "Glow rank" line still follows its "bind rank" line.
    std::vector<int> buttonRank(live.size(), 0);
    std::vector<std::wstring> boundNames(live.size());
    std::vector<bool> rankTaken(ranks.size(), false);
    const bool painting = !g_unloading.load() && !ranks.empty();
    for (size_t bi = 0; painting && bi < live.size(); ++bi) {
        for (size_t ri = 0; running[bi] && ri < ranks.size(); ++ri) {
            if (IdentityMatchesRank(idents[bi], ranks[ri])) {
                buttonRank[bi] = static_cast<int>(ri) + 1;
                rankTaken[ri] = true;
                boundNames[bi] = GetButtonAutomationName(live[bi]);
                break;
            }
        }
    }
    std::wstring ladder = GuidToLogString(deskId);
    for (const auto& r : ranks) {
        ladder += L"|" + r.key + L"," + r.displayName;
        if (r.perWindow) {
            ladder += L"," + std::to_wstring(
                                 reinterpret_cast<uintptr_t>(r.lastHwnd));
        }
    }
    for (size_t bi = 0; bi < live.size(); ++bi) {
        if (buttonRank[bi] > 0) {
            ladder += L"#" + std::to_wstring(buttonRank[bi]) + L"," +
                      boundNames[bi] + L"," + buttonAppIds[bi];
        }
    }
    const bool ladderChanged = ladder != t_lastLadderLog;
    if (ladderChanged) {
        t_lastLadderLog = ladder;
    }

    Wh_Log(L"ApplyAllHighlights: %zu tracked buttons (+%zu recycled, "
           L"skipped), %zu ranks desktop=%s%s",
           live.size(), parked, ranks.size(), GuidToLogString(deskId).c_str(),
           ladderChanged ? L"" : L" (ladder unchanged)");
    for (size_t i = 0; ladderChanged && i < ranks.size(); ++i) {
        if (ranks[i].perWindow) {
            Wh_Log(L"  rank list[%zu]: %s window %p \"%s\"", i + 1,
                   ranks[i].displayName.c_str(), ranks[i].lastHwnd,
                   ranks[i].lastWindowTitle.c_str());
        } else {
            Wh_Log(L"  rank list[%zu]: %s", i + 1, ranks[i].displayName.c_str());
        }
    }

    if (!painting) {
        for (auto& button : live) {
            ClearButtonHighlight(button);
        }
        g_pendingOverlaySweep = false;
        if (ranks.empty() && ladderChanged) {
            Wh_Log(L"ApplyAllHighlights: no ranks — cleared overlays on %zu "
                   L"tracked buttons",
                   live.size());
        }
        return;
    }

    // Explorer AppId (plus per-window slots). One rank may bind many buttons
    // (secondary taskbar / Never Combine); a button takes its best rank.
    for (size_t bi = 0; bi < live.size(); ++bi) {
        const int rank = buttonRank[bi];
        if (rank > 0) {
            if (ladderChanged) {
                const auto& r = ranks[static_cast<size_t>(rank - 1)];
                Wh_Log(L"  bind rank %d %s -> \"%s\" (appId=%s)", rank,
                       r.displayName.c_str(), boundNames[bi].c_str(),
                       buttonAppIds[bi].c_str());
            }
            ApplyButtonHighlight(live[bi], rank);
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

    // A rank without a button is logged only: it is in the list because a
    // running button with its AppId existed a moment ago (AppAppearsOnTaskbar
    // in the recompute above), so this is a transient (closing, recycling).
    for (size_t ri = 0; ri < ranks.size(); ++ri) {
        if (!rankTaken[ri]) {
            Wh_Log(L"  UNMATCHED rank %zu: %s (title=\"%s\")", ri + 1,
                   ranks[ri].displayName.c_str(),
                   ranks[ri].lastWindowTitle.c_str());
        }
    }
}

void ClearAllHighlights_UIThread() {
    for (auto& button : CollectLiveButtonsOnThisDispatcher()) {
        ClearButtonHighlight(button);
    }
}

// ---------------------------------------------------------------------------
// Thumbnail preview glow (multi-window flyouts)
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// WindowsUdk.UI.Shell / Taskbar.View interfaces (Explorer's own bindings)
// ---------------------------------------------------------------------------
// IIDs and method order from windowsudk.winmd; Taskbar.* IIDs are guid_v
// constants from Taskbar.View.dll symbols. Same declarations as
// taskbar-active-window-highlight (EphremTil17, GPL-3.0). A changed interface
// changes its IID, the QueryInterface fails and the card stays unresolved.

constexpr GUID IID_Wudk_ITaskItemThumbnailView = {
    0xF093D5DE, 0xB516, 0x55A2, {0xB1, 0x37, 0x4E, 0x12, 0x5E, 0x37, 0xCA, 0xF4}};
constexpr GUID IID_Wudk_ITaskItemThumbnailViewModel = {
    0xA5D30F97, 0x5BC4, 0x58AE, {0xB4, 0x48, 0xB1, 0x54, 0x8A, 0x1A, 0x92, 0x9D}};
constexpr GUID IID_Wudk_ITaskItemThumbnail = {
    0xD7DC08AE, 0x776E, 0x5871, {0xBC, 0xEE, 0x25, 0x0D, 0xBC, 0x94, 0xD0, 0x37}};

// Leading members only; declared order matches windowsudk.winmd.
// (WudkITaskItemAbi is declared with the task-group interfaces above.)
struct WudkITaskItemThumbnailAbi : ::IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_DisplayName(HSTRING* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_AccessibleName(HSTRING* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ParentItem(WudkITaskItemAbi** value) = 0;
};

// Interface getters of Taskbar.View.dll types (optional symbols). Called with
// the interface pointer obtained by QueryInterface for that interface's IID.
using WudkGetObject_t = int32_t(WINAPI*)(void* pThis, void** value);
WudkGetObject_t TaskItemThumbnailView_get_ThumbnailSource;
WudkGetObject_t TaskItemThumbnailViewModel_get_TaskItemThumbnail;

// Strongest resolve: the window Explorer itself bound to this card.
// card -> ThumbnailSource (view model) -> TaskItemThumbnail -> ParentItem
// -> WindowId. Per card, no ctor timing, no index; for UWP the WindowId is
// the ApplicationFrameWindow, matching focus.
// Why a card did not resolve (diagnostics; 0 = resolved).
enum class CardResolveFail : int {
    None = 0,
    Unavailable,   // getters not found
    NoView,        // QI ITaskItemThumbnailView
    NoSource,      // ThumbnailSource
    NoViewModel,   // QI ITaskItemThumbnailViewModel
    NoThumbnail,   // TaskItemThumbnail
    NoThumbnailQi, // QI ITaskItemThumbnail
    NoParentItem,  // ParentItem null / failed
    NoWindowId,    // get_WindowId failed
    NotAWindow,    // WindowId is not a live HWND
    Exception,
};

PCWSTR CardResolveFailName(CardResolveFail f) {
    switch (f) {
        case CardResolveFail::None: return L"ok";
        case CardResolveFail::Unavailable: return L"unavailable";
        case CardResolveFail::NoView: return L"no-view";
        case CardResolveFail::NoSource: return L"no-source";
        case CardResolveFail::NoViewModel: return L"no-viewmodel";
        case CardResolveFail::NoThumbnail: return L"no-thumbnail";
        case CardResolveFail::NoThumbnailQi: return L"no-thumbnail-qi";
        case CardResolveFail::NoParentItem: return L"no-parent-item";
        case CardResolveFail::NoWindowId: return L"no-window-id";
        case CardResolveFail::NotAWindow: return L"not-a-window";
        case CardResolveFail::Exception: return L"exception";
    }
    return L"?";
}

HWND ResolveHwndFromCardViewModel(FrameworkElement const& card,
                                  CardResolveFail* fail = nullptr,
                                  UINT64* rawWindowId = nullptr) {
    auto failed = [&](CardResolveFail f) -> HWND {
        if (fail) {
            *fail = f;
        }
        return nullptr;
    };
    if (fail) {
        *fail = CardResolveFail::None;
    }
    if (rawWindowId) {
        *rawWindowId = 0;
    }
    if (!card || !TaskItemThumbnailView_get_ThumbnailSource ||
        !TaskItemThumbnailViewModel_get_TaskItemThumbnail) {
        return failed(CardResolveFail::Unavailable);
    }
    try {
        auto* unknown = static_cast<::IUnknown*>(winrt::get_abi(card));
        winrt::com_ptr<::IUnknown> view;
        if (FAILED(unknown->QueryInterface(IID_Wudk_ITaskItemThumbnailView,
                                           view.put_void()))) {
            return failed(CardResolveFail::NoView);
        }
        winrt::com_ptr<::IUnknown> source;
        if (TaskItemThumbnailView_get_ThumbnailSource(view.get(),
                                                      source.put_void()) < 0 ||
            !source) {
            return failed(CardResolveFail::NoSource);
        }
        winrt::com_ptr<::IUnknown> viewModel;
        if (FAILED(source->QueryInterface(IID_Wudk_ITaskItemThumbnailViewModel,
                                          viewModel.put_void()))) {
            return failed(CardResolveFail::NoViewModel);
        }
        winrt::com_ptr<::IUnknown> thumbnailObject;
        if (TaskItemThumbnailViewModel_get_TaskItemThumbnail(
                viewModel.get(), thumbnailObject.put_void()) < 0 ||
            !thumbnailObject) {
            return failed(CardResolveFail::NoThumbnail);
        }
        winrt::com_ptr<WudkITaskItemThumbnailAbi> thumbnail;
        if (FAILED(thumbnailObject->QueryInterface(IID_Wudk_ITaskItemThumbnail,
                                                   thumbnail.put_void()))) {
            return failed(CardResolveFail::NoThumbnailQi);
        }
        winrt::com_ptr<WudkITaskItemAbi> item;
        if (FAILED(thumbnail->get_ParentItem(item.put())) || !item) {
            return failed(CardResolveFail::NoParentItem);
        }
        UINT64 windowId = 0;
        if (FAILED(item->get_WindowId(&windowId))) {
            return failed(CardResolveFail::NoWindowId);
        }
        if (rawWindowId) {
            *rawWindowId = windowId;
        }
        HWND hwnd = reinterpret_cast<HWND>(windowId);
        return IsWindow(hwnd) ? hwnd : failed(CardResolveFail::NotAWindow);
    } catch (...) {
        return failed(CardResolveFail::Exception);
    }
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

// ---------------------------------------------------------------------------
// Flyout ItemsRepeater: repaint when a card is removed
// ---------------------------------------------------------------------------
// Closing a window from the flyout removes its card without rebinding the
// others (no put_ThumbnailSource, no OnApplyTemplate), so the remaining
// ranks kept their old paint until focus changed or the flyout reopened.
// ElementClearing
// fires when the repeater removes or recycles a card. Subscribed once per
// repeater on its UI thread; revoked there at unload.

namespace muxc = winrt::Microsoft::UI::Xaml::Controls;

void SchedulePreviewFlyoutRefresh(FrameworkElement dispatcherAnchor);

struct RepeaterClearingWatch {
    winrt::weak_ref<muxc::ItemsRepeater> repeater;
    winrt::event_token token{};
};
std::mutex g_repeaterWatchMutex;
std::unordered_map<void*, RepeaterClearingWatch> g_repeaterWatches;

void WatchRepeaterElementClearing_UIThread(muxc::ItemsRepeater const& repeater) {
    // Unload revokes the watch on its dispatcher: subscribe only on one the
    // drain knows.
    if (!repeater || g_unloading.load() ||
        !RememberUiDispatcher(repeater.try_as<FrameworkElement>())) {
        return;
    }
    void* id = InspectableIdentity(repeater);
    if (!id) {
        return;
    }
    {
        std::lock_guard<std::mutex> lock(g_repeaterWatchMutex);
        // A dead repeater's address can be reused by a new one: drop it.
        for (auto it = g_repeaterWatches.begin(); it != g_repeaterWatches.end();) {
            bool dead = true;
            try {
                dead = !it->second.repeater.get();
            } catch (...) {
            }
            it = dead ? g_repeaterWatches.erase(it) : std::next(it);
        }
        if (g_repeaterWatches.count(id)) {
            return;
        }
    }
    winrt::event_token token{};
    try {
        token = repeater.ElementClearing(
            [](muxc::ItemsRepeater const& sender,
               muxc::ItemsRepeaterElementClearingEventArgs const& args) {
                if (g_unloading.load() ||
                    !SettingsSnap()->previewHighlightEnabled) {
                    return;
                }
                try {
                    FrameworkElement anchor = nullptr;
                    if (auto el = args.Element()) {
                        anchor = el.try_as<FrameworkElement>();
                    }
                    if (!anchor) {
                        anchor = sender.try_as<FrameworkElement>();
                    }
                    // Low priority, after layout: the card set has changed by
                    // then. Coalesced with other refresh requests.
                    SchedulePreviewFlyoutRefresh(anchor);
                } catch (...) {
                }
            });
    } catch (...) {
        return;
    }
    if (!token) {
        return;
    }
    bool raced = false;
    {
        std::lock_guard<std::mutex> lock(g_repeaterWatchMutex);
        raced = !g_repeaterWatches
                     .emplace(id, RepeaterClearingWatch{winrt::make_weak(repeater),
                                                        token})
                     .second;
    }
    // Registration is not logged (205 lines a day); a watch left over at
    // unload is (Wh_ModUninit, "ElementClearing watches not revoked").
    if (raced) {
        try {
            repeater.ElementClearing(token);
        } catch (...) {
        }
    }
}

// Unload: revoke the watches owned by this dispatcher; keep the others for
// their own dispatcher's pass.
void RevokeRepeaterWatchesOnThisDispatcher() {
    std::unordered_map<void*, RepeaterClearingWatch> taken;
    {
        std::lock_guard<std::mutex> lock(g_repeaterWatchMutex);
        taken.swap(g_repeaterWatches);
    }
    std::unordered_map<void*, RepeaterClearingWatch> keep;
    for (auto& [id, watch] : taken) {
        muxc::ItemsRepeater repeater{nullptr};
        try {
            repeater = watch.repeater.get();
        } catch (...) {
        }
        if (!repeater) {
            continue;  // gone; nothing to revoke
        }
        bool mine = false;
        try {
            auto dispatcher = repeater.Dispatcher();
            mine = dispatcher && dispatcher.HasThreadAccess();
        } catch (...) {
        }
        if (!mine) {
            keep.emplace(id, std::move(watch));
            continue;
        }
        try {
            repeater.ElementClearing(watch.token);
        } catch (...) {
            Wh_Log(L"ERROR: ElementClearing revoke failed");
        }
    }
    if (!keep.empty()) {
        std::lock_guard<std::mutex> lock(g_repeaterWatchMutex);
        for (auto& [id, watch] : keep) {
            g_repeaterWatches.emplace(id, std::move(watch));
        }
    }
}

// The flyout's realized cards in repeater order, which is the flyout's
// visual order (unlike PositionInSet, which the automation peer may not
// refresh after a thumbnail reorder).
std::vector<FrameworkElement> CollectRepeaterThumbnailViews(
    FrameworkElement thumbView) {
    std::vector<FrameworkElement> out;
    auto repeaterEl = FindAncestorByClassName(
        thumbView, L"Microsoft.UI.Xaml.Controls.ItemsRepeater");
    if (!repeaterEl) {
        return out;
    }
    auto repeater =
        repeaterEl.try_as<winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater>();
    if (!repeater) {
        return out;
    }
    WatchRepeaterElementClearing_UIThread(repeater);
    try {
        auto itemsSourceView = repeater.ItemsSourceView();
        const int count = itemsSourceView ? itemsSourceView.Count() : 0;
        for (int i = 0; i < count; ++i) {
            auto element = repeater.TryGetElement(i);
            if (!element) {
                continue;
            }
            auto child = element.try_as<FrameworkElement>();
            if (!child) {
                continue;
            }
            if (winrt::get_class_name(child) ==
                L"Taskbar.TaskItemThumbnailView") {
                out.push_back(child);
            }
        }
    } catch (...) {
    }
    return out;
}

void TrackThumbView_UIThread(FrameworkElement thumbView) {
    // Untracked without a drainable dispatcher.
    if (!thumbView || !RememberUiDispatcher(thumbView)) {
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
            if (auto tb = FindDescendantByName(el, L"DisplayNameTextBlock")) {
                return tb;
            }
            return el;
        }
    }
    return nullptr;
}

// Snap-group card in the same flyout as the individual windows. Must not be
// treated as a window thumbnail. Language-independent: the group card hosts
// an IconsRepeater with 2+ icon children and has no window HWND.
bool IsSnapGroupThumbnailView(FrameworkElement view) {
    if (!view) {
        return false;
    }
    if (auto repeater = FindDescendantByName(view, L"IconsRepeater")) {
        try {
            if (Media::VisualTreeHelper::GetChildrenCount(repeater) >= 2) {
                // A real window card that gained a second icon still has an
                // HWND (from its view model). A snap-group card does not.
                if (ResolveHwndFromCardViewModel(view)) {
                    return false;
                }
                return true;
            }
        } catch (...) {
        }
    }
    return false;
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
            // Never our own card overlay.
            if (cn == L"Windows.UI.Xaml.Controls.Border" &&
                child.Name() != kThumbCardOverlayName) {
                return child;
            }
        }
    } catch (...) {
    }
    return nullptr;
}

// ---------------------------------------------------------------------------
// Card overlay (Whole plate / Whole plate with accent)
// ---------------------------------------------------------------------------
// Our own Border, inserted right above the card's BackgroundBorder and bound
// to its geometry (size, margin, alignment, rounded corners), so it follows
// Taskbar Styler themes without measuring. Same construction as
// taskbar-active-window-highlight (EphremTil17, GPL-3.0). Recolouring
// BackgroundBorder itself does not work: the card's hover state replaces
// that brush. The overlay sits
// above BackgroundBorder and below the title/thumbnail, so hover changes the
// background underneath and still shows through a partial fill.

void BindCardOverlay(FrameworkElement const& target,
                     DependencyObject const& source,
                     DependencyProperty const& property,
                     PCWSTR path) {
    Data::Binding binding;
    binding.Source(source);
    binding.Path(PropertyPath(path));
    binding.Mode(Data::BindingMode::OneWay);
    target.SetBinding(property, binding);
}

Controls::Border FindCardOverlay(FrameworkElement thumbView) {
    if (auto el = FindDescendantByName(thumbView, kThumbCardOverlayName)) {
        return el.try_as<Controls::Border>();
    }
    return nullptr;
}

void HideCardOverlay(FrameworkElement thumbView) {
    try {
        if (auto overlay = FindCardOverlay(thumbView)) {
            overlay.Visibility(Visibility::Collapsed);
        }
    } catch (...) {
    }
}

// Returns false when the card has no BackgroundBorder in a panel (unknown
// template); the caller then uses the old overlay-host plate.
bool PaintCardOverlay(FrameworkElement thumbView,
                      winrt::Windows::UI::Color fill,
                      winrt::Windows::UI::Color stroke,
                      double strokeThickness) {
    auto background = FindThumbnailBackgroundBorder(thumbView);
    if (!background) {
        return false;
    }
    auto panel =
        Media::VisualTreeHelper::GetParent(background).try_as<Controls::Panel>();
    if (!panel) {
        return false;
    }
    Controls::Border overlay = FindCardOverlay(thumbView);
    if (!overlay) {
        overlay = Controls::Border();
        overlay.Name(kThumbCardOverlayName);
        overlay.IsHitTestVisible(false);
        Controls::Grid::SetRow(overlay, Controls::Grid::GetRow(background));
        Controls::Grid::SetColumn(overlay, Controls::Grid::GetColumn(background));
        Controls::Grid::SetRowSpan(overlay, Controls::Grid::GetRowSpan(background));
        Controls::Grid::SetColumnSpan(overlay,
                                      Controls::Grid::GetColumnSpan(background));
        BindCardOverlay(overlay, background, FrameworkElement::WidthProperty(),
                        L"Width");
        BindCardOverlay(overlay, background, FrameworkElement::HeightProperty(),
                        L"Height");
        BindCardOverlay(overlay, background, FrameworkElement::MarginProperty(),
                        L"Margin");
        BindCardOverlay(overlay, background,
                        FrameworkElement::HorizontalAlignmentProperty(),
                        L"HorizontalAlignment");
        BindCardOverlay(overlay, background,
                        FrameworkElement::VerticalAlignmentProperty(),
                        L"VerticalAlignment");
        if (background.try_as<Controls::Border>()) {
            BindCardOverlay(overlay, background,
                            Controls::Border::CornerRadiusProperty(),
                            L"CornerRadius");
        }
        auto children = panel.Children();
        uint32_t index = 0;
        const bool found = children.IndexOf(background, index);
        // Explicit index: directly above the background, below the content.
        children.InsertAt(found ? index + 1 : 0, overlay);
    }
    overlay.Background(Media::SolidColorBrush{fill});
    if (strokeThickness > 0.0) {
        overlay.BorderBrush(Media::SolidColorBrush{stroke});
        overlay.BorderThickness(Thickness{strokeThickness, strokeThickness,
                                          strokeThickness, strokeThickness});
    } else {
        overlay.BorderBrush(nullptr);
        overlay.BorderThickness(Thickness{0, 0, 0, 0});
    }
    overlay.Visibility(Visibility::Visible);
    return true;
}

void ClearThumbnailHighlight(FrameworkElement thumbView) {
    if (!thumbView) {
        return;
    }
    try {
        if (!FindDescendantByName(thumbView, kThumbGlowElementName) &&
            !FindDescendantByName(thumbView, kThumbCardOverlayName)) {
            return;
        }
        RemoveNamedDescendant(thumbView, kThumbCardOverlayName);
        RemoveNamedDescendant(thumbView, kThumbGlowElementName);
        RemoveNamedDescendant(thumbView, kThumbTitleBgName);
        RemoveNamedDescendant(thumbView, kThumbTitleBarName);
    } catch (...) {
    }
}

// Preview panels use explicit insertion too: keep deferred-child index
// notifications balanced with removal, even when inserting at the end.
// Put el directly above the card's BackgroundBorder when that border is a
// child of this panel: above the card background, below the title text and
// thumbnail. Returns false if there is no such border here.
bool PlaceAboveCardBackground(Controls::Panel panel, UIElement el) {
    if (!panel || !el) {
        return false;
    }
    try {
        auto children = panel.Children();
        int bg = -1;
        for (uint32_t i = 0; i < children.Size(); ++i) {
            auto fe = children.GetAt(i).try_as<FrameworkElement>();
            if (fe && fe.Name() == L"BackgroundBorder") {
                bg = static_cast<int>(i);
                break;
            }
        }
        if (bg < 0) {
            return false;
        }
        uint32_t idx = 0;
        if (!children.IndexOf(el, idx)) {
            return false;
        }
        if (static_cast<int>(idx) == bg + 1) {
            return true;
        }
        children.RemoveAt(idx);
        if (static_cast<int>(idx) < bg) {
            --bg;
        }
        children.InsertAt(static_cast<uint32_t>(bg + 1), el);
        return true;
    } catch (...) {
        return false;
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
            children.InsertAt(children.Size(), el);
        }
    } catch (...) {
    }
}

// Overlay host that must NOT affect parent layout.
//
// Thumbnail cards often use a Grid with rows (title | image). A normal child
// lands in (0,0) — the title row — and expands that row (bar between icon and
// text + card grows sideways). Same fix as icon glow: span every row/column
// and Stretch to the arranged card size. Children use Margin (not
// RenderTransform — that clips the right end-cap).
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
                <Border Name="WhRecentFocusThumbTitleBg"
                        IsHitTestVisible="False" Visibility="Collapsed"/>
                <Rectangle Name="WhRecentFocusThumbTitleBar"
                           IsHitTestVisible="False" Visibility="Collapsed"/>
            </Grid>
        )";
        host = Markup::XamlReader::Load(xaml).as<Controls::Grid>();
        auto children = panel.Children();
        children.InsertAt(children.Size(), host);
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
    // Under the card content, right above its background: a title
    // wash on top of the title made the text look dim at higher tint
    // opacity. Front only if this template has no BackgroundBorder here.
    if (!PlaceAboveCardBackground(panel, host)) {
        BringElementToFront(panel, host);
    }
    return host;
}

// Position with Margin, not RenderTransform: the layout clip is the
// un-transformed slot, so a translate of `pad` clipped the right end-cap.
// The far margins take back the offset and the size, so the desired size is
// 0 and an overlay can never make the card larger than Windows laid it out
// (as the icon plates, ZeroDesiredMargin). Placement unchanged, and no
// layout clip: the slot minus these margins only grows.
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
        el.Margin(Thickness{x, y, width > 0 ? -(x + width) : 0,
                            height > 0 ? -(y + height) : 0});
        el.ClearValue(UIElement::RenderTransformProperty());
        if (width > 0) {
            el.Width(width);
            el.MaxWidth(width);
        } else {
            el.ClearValue(FrameworkElement::WidthProperty());
            el.ClearValue(FrameworkElement::MaxWidthProperty());
        }
        if (height > 0) {
            el.Height(height);
            el.MaxHeight(height);
        } else {
            el.ClearValue(FrameworkElement::HeightProperty());
            el.ClearValue(FrameworkElement::MaxHeightProperty());
        }
        el.Visibility(Visibility::Visible);
    } catch (...) {
    }
}

void HideThumbOverlayChildren(Controls::Grid host) {
    if (!host) {
        return;
    }
    for (auto name : {kThumbGlowPlateName, kThumbTitleBgName,
                      kThumbTitleBarName}) {
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
                    // Title outline: else a switch to titleBg keeps it.
                    b.BorderBrush(nullptr);
                    b.BorderThickness(Thickness{0, 0, 0, 0});
                }
                el.ClearValue(FrameworkElement::WidthProperty());
                el.ClearValue(FrameworkElement::HeightProperty());
                el.ClearValue(FrameworkElement::MaxWidthProperty());
                el.ClearValue(FrameworkElement::MaxHeightProperty());
                el.ClearValue(FrameworkElement::MarginProperty());
            } catch (...) {
            }
        }
    }
}

// Keep the overlay host; only restore plate + hide children (style switch /
// rank change). Full ClearThumbnailHighlight tears down the host and forces
// XamlReader::Load on the next paint.
void ResetThumbnailOverlayForRepaint(FrameworkElement thumbView) {
    HideCardOverlay(thumbView);
    try {
        auto panel = GetThumbnailHostPanel(thumbView);
        if (!panel) {
            return;
        }
        auto hostEl =
            FindChildByName(panel.as<FrameworkElement>(), kThumbGlowElementName);
        if (auto host = hostEl.try_as<Controls::Grid>()) {
            HideThumbOverlayChildren(host);
        }
    } catch (...) {
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

// One Low-priority flyout refresh per UI thread. Title remeasure, hover-switch
// (TargetItemKey), and OnApplyTemplate all coalesce here. The callback picks a
// card that still has an ItemsRepeater ancestor — do not reuse the weak_ref
// captured at schedule time (often the oldest tracked card, already recycled).
// At most one follow-up when the first pass still sees unmeasured cards.
thread_local bool g_previewFlyoutRefreshPending = false;
thread_local bool g_previewFlyoutRefreshNeeded = false;
thread_local int g_thumbRelayoutDepth = 0;
thread_local int g_previewFlyoutRefreshFollowUps = 0;

FrameworkElement PickLiveFlyoutThumbOnThisDispatcher() {
    FrameworkElement withRepeater = nullptr;
    FrameworkElement any = nullptr;
    std::vector<winrt::weak_ref<FrameworkElement>> thumbs;
    {
        std::lock_guard<std::mutex> lock(g_thumbViewsMutex);
        thumbs = g_trackedThumbViews;
    }
    ForEachLiveElementOnThisDispatcher(thumbs, [&](FrameworkElement el) {
        if (!any) {
            any = el;
        }
        if (!withRepeater &&
            FindAncestorByClassName(
                el, L"Microsoft.UI.Xaml.Controls.ItemsRepeater")) {
            withRepeater = el;
        }
    });
    return withRepeater ? withRepeater : any;
}

void SchedulePreviewFlyoutRefresh(FrameworkElement dispatcherAnchor) {
    // Only on this thread's dispatcher, and one the unload drain knows: the
    // pending flag below is per thread.
    if (!dispatcherAnchor || g_unloading.load() ||
        !RememberUiDispatcher(dispatcherAnchor)) {
        return;
    }
    if (g_thumbRelayoutDepth > 0) {
        g_previewFlyoutRefreshNeeded = true;
        return;
    }
    if (g_previewFlyoutRefreshPending) {
        return;
    }
    g_previewFlyoutRefreshPending = true;
    try {
        auto dispatcher = dispatcherAnchor.Dispatcher();
        if (!dispatcher) {
            g_previewFlyoutRefreshPending = false;
            return;
        }
        winrt::weak_ref<FrameworkElement> weak =
            winrt::make_weak(dispatcherAnchor);
        if (!DispatcherTryRun(
                dispatcher,
                winrt::Windows::UI::Core::CoreDispatcherPriority::Low,
                [weak]() {
                g_previewFlyoutRefreshPending = false;
                try {
                    if (g_unloading.load()) {
                        return;
                    }
                    FrameworkElement el = nullptr;
                    try {
                        el = weak.get();
                    } catch (...) {
                    }
                    if (el && !FindAncestorByClassName(
                                  el,
                                  L"Microsoft.UI.Xaml.Controls.ItemsRepeater")) {
                        el = nullptr;
                    }
                    if (!el) {
                        el = PickLiveFlyoutThumbOnThisDispatcher();
                    }
                    if (!el) {
                        return;
                    }
                    g_thumbRelayoutDepth = 1;
                    g_previewFlyoutRefreshNeeded = false;
                    RefreshThumbnailFlyout_UIThread(el);
                    g_thumbRelayoutDepth = 0;
                    const bool again = g_previewFlyoutRefreshNeeded &&
                                       g_previewFlyoutRefreshFollowUps < 1;
                    g_previewFlyoutRefreshNeeded = false;
                    if (again) {
                        ++g_previewFlyoutRefreshFollowUps;
                        SchedulePreviewFlyoutRefresh(el);
                    } else {
                        g_previewFlyoutRefreshFollowUps = 0;
                    }
                } catch (...) {
                    g_thumbRelayoutDepth = 0;
                    g_previewFlyoutRefreshFollowUps = 0;
                }
            })) {
            g_previewFlyoutRefreshPending = false;
        }
    } catch (...) {
        g_previewFlyoutRefreshPending = false;
    }
}

void ApplyThumbnailHighlight(FrameworkElement thumbView, int rankOneBased) {
    auto settings = SettingsSnap();
    if (!thumbView || rankOneBased <= 0 || g_unloading.load() ||
        !settings->previewHighlightEnabled) {
        ClearThumbnailHighlight(thumbView);
        return;
    }

    try {
        // Start clean so style switches don't leave mixed chrome. Keep the
        // overlay host — re-parsing the markup on every ranked paint is wasted.
        ResetThumbnailOverlayForRepaint(thumbView);

        auto panel = GetThumbnailHostPanel(thumbView);
        if (!panel) {
            return;
        }
        auto panelFe = panel.as<FrameworkElement>();

        const int rankIdx = rankOneBased - 1;
        const int intensity = RankIntensity(settings->previewIntensity, rankIdx);
        const double t = intensity / 100.0;
        winrt::Windows::UI::Color base = ResolveGlowBaseColor(*settings);
        auto withAlpha = [](winrt::Windows::UI::Color c, int a) {
            c.A = static_cast<uint8_t>((std::max)(0, (std::min)(255, a)));
            return c;
        };

        const double thickness = static_cast<double>(
            (std::max)(2, (std::min)(8, settings->glowThickness)));
        // Fallback plate (card without BackgroundBorder) corner roundness:
        // 28 %.
        constexpr double roundnessFrac = 0.28;
        // Preview plate / titleBg use dedicated opacity (not taskbar fill).
        const int fillOpacitySetting =
            (std::max)(0, (std::min)(100, settings->previewFillOpacity));
        const PreviewStyle style = rankOneBased <= 1
                                       ? settings->previewTopStyle
                                       : settings->previewRestStyle;
        if (t <= 0.0 || style == PreviewStyle::None) {
            ClearThumbnailHighlight(thumbView);
            return;
        }
        PreviewStyle paintStyle = style;

        // Tint base. An accent style halves it so its same-colour border
        // still reads. One tint level per flyout: with an
        // accent style at rank 1, ranks 2+ in a non-accent style are halved
        // too, so no lower rank is tinted more than rank 1.
        auto isAccent = [](PreviewStyle s) {
            return s == PreviewStyle::Outline ||
                   s == PreviewStyle::TitleOutline;
        };
        const bool halfTint =
            isAccent(style) ||
            (rankOneBased > 1 && isAccent(settings->previewTopStyle));
        const int tintSetting =
            halfTint ? fillOpacitySetting / 2 : fillOpacitySetting;

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
            SchedulePreviewFlyoutRefresh(thumbView);
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

        // Plate / Outline: our own Border above BackgroundBorder (see
        // PaintCardOverlay). Zero tint and no outline leaves the card alone.
        if (paintStyle == PreviewStyle::Plate ||
            paintStyle == PreviewStyle::Outline) {
            const bool outline = paintStyle == PreviewStyle::Outline;
            const int fillA =
                static_cast<int>(tintSetting * 2.55 * t + 0.5);
            const int strokeA = static_cast<int>(255 * t + 0.5);
            // painted: card overlay drawn, or nothing to draw (zero tint).
            bool painted = !outline && tintSetting <= 0;
            if (!painted) {
                try {
                    painted = PaintCardOverlay(
                        thumbView, withAlpha(base, fillA),
                        withAlpha(base, strokeA),
                        outline ? settings->previewOutlineThickness : 0.0);
                } catch (...) {
                    painted = false;
                }
            }
            if (!painted) {
                Controls::Grid host = EnsureThumbOverlayHost(panel);
                HideThumbOverlayChildren(host);
                if (auto plate = FindChildByName(host, kThumbGlowPlateName)
                                     .try_as<Shapes::Rectangle>()) {
                    const int plateA =
                        static_cast<int>(tintSetting * 2.55 + 0.5);
                    plate.Fill(Media::SolidColorBrush{withAlpha(base, plateA)});
                    plate.Stroke(nullptr);
                    plate.StrokeThickness(0);
                    plate.RadiusX(cardW * roundnessFrac * 0.12);
                    plate.RadiusY(cardH * roundnessFrac * 0.12);
                    plate.Opacity(t);
                    PlaceOverlayChild(plate, 0, 0, cardW, cardH);
                }
            }

            Wh_Log(L"Preview glow rank %d style=%s paint=%s intensity=%d "
                   L"on \"%s\"",
                   rankOneBased, PreviewStyleName(style),
                   PreviewStyleName(paintStyle), intensity,
                   Automation::AutomationProperties::GetName(thumbView)
                       .c_str());
            return;
        }

        Controls::Grid host = EnsureThumbOverlayHost(panel);
        HideThumbOverlayChildren(host);

        if (paintStyle == PreviewStyle::TitleBg ||
            paintStyle == PreviewStyle::TitleOutline) {
            // Title / title with accent: a plate over the title row,
            // 4px in from the card's BackgroundBorder (left, top, right) with
            // the card's corner radius, ending under the title with the gap
            // above the title mirrored below it. The accent variant adds an
            // accent border and halves the tint, as Outline does for the card.
            const bool outline = paintStyle == PreviewStyle::TitleOutline;
            const double stroke =
                outline ? settings->previewOutlineThickness : 0.0;
            constexpr double kEdgeInset = 4.0;
            double top = 4.0;
            double stripH = 28.0;
            double titleTop = 0, titleBottom = 0;
            bool laidOut = GetTitleBottomRelative(title, host, cardH, titleTop,
                                                  titleBottom);
            if (laidOut) {
                // Fallback strip (no BackgroundBorder). An outline is drawn
                // inside: grow by its width so the gap around the title stays.
                top = (std::max)(2.0, titleTop - 3.0 - stroke);
                stripH = (std::max)(20.0, titleBottom - top + 4.0 + stroke);
            } else {
                // Layout not ready — default strip + one deferred remeasure.
                SchedulePreviewFlyoutRefresh(thumbView);
            }
            auto chip =
                FindChildByName(host, kThumbTitleBgName).try_as<Controls::Border>();
            if (chip) {
                const double pad = 8.0;
                double x = pad;
                double w = (std::max)(24.0, cardW - 2.0 * pad);
                // Brace-init {4} only fills TopLeft (C++ struct). All four.
                CornerRadius radius{4, 4, 4, 4};
                bool fromCard = false;
                if (laidOut) {
                    try {
                        if (auto bb = FindThumbnailBackgroundBorder(thumbView)) {
                            const double bw = bb.ActualWidth();
                            auto origin =
                                bb.TransformToVisual(host).TransformPoint(
                                    winrt::Windows::Foundation::Point{0.f,
                                                                      0.f});
                            const double edgeTop = origin.Y + kEdgeInset;
                            // Gap below the title = gap above it, at most
                            // 4px + border.
                            const double gap = (std::min)(
                                titleTop - edgeTop, 8.0 - kEdgeInset + stroke);
                            if (bw > 2.0 * kEdgeInset + 24.0 && gap >= 0.0) {
                                x = origin.X + kEdgeInset;
                                w = bw - 2.0 * kEdgeInset;
                                top = edgeTop;
                                stripH = (std::max)(20.0,
                                                    titleBottom + gap - top);
                                // The card's own radius: radius - inset
                                // (concentric) looked too sharp.
                                if (auto b = bb.try_as<Controls::Border>()) {
                                    radius = b.CornerRadius();
                                }
                                fromCard = true;
                            }
                        }
                    } catch (...) {
                    }
                }
                if (!fromCard) {
                    const double bottom = top + stripH;
                    top = (std::max)(6.0, top);
                    if (outline) {
                        // Keep the bottom edge where it was, or the outline
                        // crowds the thumbnail when the top is pushed down.
                        stripH = (std::max)(20.0, bottom - top);
                    }
                }
                const int chipA =
                    static_cast<int>(tintSetting * 2.55 * t + 0.5);
                chip.Background(Media::SolidColorBrush{withAlpha(base, chipA)});
                if (outline) {
                    const int strokeA = static_cast<int>(255 * t + 0.5);
                    chip.BorderBrush(
                        Media::SolidColorBrush{withAlpha(base, strokeA)});
                    chip.BorderThickness(
                        Thickness{stroke, stroke, stroke, stroke});
                }
                chip.CornerRadius(radius);
                chip.Opacity(1.0);
                PlaceOverlayChild(chip, x, top, w, stripH);
            }
        } else {  // TitleBar — underline just under the title glyphs
            // Small gap under baseline — enough to avoid strikethrough, not
            // so much that the bar hugs the thumbnail image.
            constexpr double kGapBelowTitle = 2.0;
            double top = 30.0;  // fallback under a ~28px header
            double titleTop = 0, titleBottom = 0;
            bool laidOut = GetTitleBottomRelative(title, host, cardH, titleTop,
                                                  titleBottom);
            if (laidOut) {
                top = titleBottom + kGapBelowTitle;
            } else {
                SchedulePreviewFlyoutRefresh(thumbView);
            }
            top = (std::max)(18.0, (std::min)(top, cardH * 0.38));

            const double barH =
                (std::max)(2.0, (std::min)(5.0, thickness + 0.5));

            auto bar = FindChildByName(host, kThumbTitleBarName)
                           .try_as<Shapes::Rectangle>();
            if (bar) {
                // Wider alpha range than the old rank-1-only bar so flyout
                // ranks read as a ladder; t=1 stays fully opaque accent.
                // Intensity is Opacity only (do not also scale brush alpha).
                const double pad = 8.0;
                bar.Fill(Media::SolidColorBrush{withAlpha(base, 255)});
                bar.Stroke(nullptr);
                bar.StrokeThickness(0);
                bar.RadiusX(barH * 0.5);
                bar.RadiusY(barH * 0.5);
                bar.Opacity(t);
                PlaceOverlayChild(bar, pad, top,
                                  (std::max)(24.0, cardW - 2.0 * pad), barH);
            }
        }

        Wh_Log(L"Preview glow rank %d style=%s paint=%s intensity=%d "
               L"card=%.0fx%.0f on \"%s\"",
               rankOneBased, PreviewStyleName(style),
               PreviewStyleName(paintStyle), intensity, cardW, cardH,
               Automation::AutomationProperties::GetName(thumbView)
                   .c_str());
    } catch (...) {
        HRESULT hr = winrt::to_hresult();
        Wh_Log(L"ApplyThumbnailHighlight error %08X", hr);
    }
}

// Per UI thread; emptied by ReleaseUiThreadLocals_UIThread at unload.
thread_local std::wstring t_lastResolveLog;

void RefreshThumbnailFlyout_UIThread(FrameworkElement anyThumb) {
    if (!anyThumb) {
        return;
    }
    TrackThumbView_UIThread(anyThumb);

    auto repeaterViews = CollectRepeaterThumbnailViews(anyThumb);
    const bool usedRepeater = !repeaterViews.empty();
    auto allViews =
        usedRepeater ? std::move(repeaterViews)
                     : CollectSiblingThumbnailViews(anyThumb);
    if (!SettingsSnap()->previewHighlightEnabled || g_unloading.load()) {
        for (auto& s : allViews) {
            ClearThumbnailHighlight(s);
        }
        return;
    }

    // Snap-group cards sit in the same ItemsRepeater as the windows.
    // Never glow those; they are not a window HWND. Compute once — the
    // lookup walks IconsRepeater and resolves the card's view model.
    std::vector<char> isSnap(allViews.size(), 0);
    for (size_t i = 0; i < allViews.size(); ++i) {
        if (IsSnapGroupThumbnailView(allViews[i])) {
            isSnap[i] = 1;
        }
    }
    std::vector<FrameworkElement> siblings;
    siblings.reserve(allViews.size());
    for (size_t i = 0; i < allViews.size(); ++i) {
        if (isSnap[i]) {
            ClearThumbnailHighlight(allViews[i]);
        } else {
            siblings.push_back(allViews[i]);
        }
    }

    // Each flyout ranks its own cards (below). A single-window flyout is
    // ranked the same way only with previews.singleWindow: its one card is
    // rank 1 while its window is a confirmed recent window (preview min-focus
    // and decay), like the latest window of a multi-window flyout.
    if (siblings.empty() ||
        (siblings.size() == 1 && !SettingsSnap()->previewSingleWindow)) {
        for (auto& s : siblings) {
            ClearThumbnailHighlight(s);
        }
        return;
    }

    // HWND per card (this flyout only — not a global window ladder): the
    // card's own view model, ParentItem.WindowId (Explorer's binding). Per
    // card; cannot shift when cards are removed or recycled. A card without
    // one stays unmarked. Repeater order is no identity: it shifts when a card
    // is closed.
    struct Scored {
        FrameworkElement view{nullptr};
        HWND hwnd = nullptr;
        ULONGLONG tick = 0;
        ULONGLONG confirmSeq = 0;
        int rank = 0;  // 1-based flyout rank; 0 = not highlighted
        CardResolveFail fail = CardResolveFail::None;
        UINT64 rawWindowId = 0;
    };
    std::vector<Scored> scored(siblings.size());

    // Card view model -> ParentItem.WindowId (no lock held).
    for (size_t i = 0; i < siblings.size(); ++i) {
        TrackThumbView_UIThread(siblings[i]);
        scored[i].view = siblings[i];
        scored[i].hwnd = ResolveHwndFromCardViewModel(
            siblings[i], &scored[i].fail, &scored[i].rawWindowId);
    }

    // Recency for every card in one hold.
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        auto& desk = CurrentDeskLocked();
        for (auto& sc : scored) {
            if (!sc.hwnd) {
                continue;
            }
            IsWindowRecentForPreviewLocked(desk, sc.hwnd, &sc.tick);
            auto it = desk.windowFocusMap.find(sc.hwnd);
            if (it != desk.windowFocusMap.end()) {
                sc.confirmSeq = it->second.confirmSeq;
            }
        }
    }

    // Rank this flyout only (not a global window ladder). Sort siblings that
    // have a recency tick; ties: confirmSeq, then the live foreground HWND
    // (normalized: an owned dialog stands for its owner's card).
    HWND foreground = NormalizeFocusHwnd(GetForegroundWindow());
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
        if (scored[a].confirmSeq != scored[b].confirmSeq) {
            return scored[a].confirmSeq > scored[b].confirmSeq;
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

    {
        const size_t painted =
            (std::min)(order.size(), static_cast<size_t>(limit));
        // Siblings are listed only when the set, a rank or a resolve failure
        // differs from this UI thread's last resolve: reopening the same
        // flyout would repeat them every time.
        std::wstring sig = std::to_wstring(limit);
        for (const auto& s : scored) {
            sig += L"|" + std::to_wstring(reinterpret_cast<uintptr_t>(s.hwnd)) +
                   L"," + std::to_wstring(s.rank) + L"," +
                   std::to_wstring(static_cast<int>(s.fail));
        }
        const bool changed = sig != t_lastResolveLog;
        if (changed) {
            t_lastResolveLog = sig;
        }
        Wh_Log(L"Preview resolve: %zu siblings, highlightCount=%d "
               L"candidates=%zu painted=%zu%s",
               siblings.size(), limit, order.size(), painted,
               changed ? L"" : L" (unchanged)");
        for (size_t i = 0; changed && i < scored.size(); ++i) {
            std::wstring name;
            try {
                name = Automation::AutomationProperties::GetName(scored[i].view)
                           .c_str();
            } catch (...) {
            }
            Wh_Log(L"  sibling[%zu]: hwnd=%p tick=%llu rank=%d%s name=\"%s\"",
                   i, scored[i].hwnd,
                   static_cast<unsigned long long>(scored[i].tick),
                   scored[i].rank,
                   scored[i].rank > 0 ? L" [GLOW]" : L"", name.c_str());
            if (scored[i].fail != CardResolveFail::None) {
                HWND raw = reinterpret_cast<HWND>(scored[i].rawWindowId);
                wchar_t cls[96] = L"";
                DWORD pid = 0;
                if (raw && IsWindow(raw)) {
                    GetClassNameW(raw, cls, ARRAYSIZE(cls));
                    GetWindowThreadProcessId(raw, &pid);
                }
                Wh_Log(L"    unresolved: %s windowId=0x%llx class=%s pid=%u",
                       CardResolveFailName(scored[i].fail),
                       static_cast<unsigned long long>(scored[i].rawWindowId),
                       cls, pid);
            }
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
    // focus thread: a handler calling this again spins at 100 % CPU and
    // starves WM_TIMER.
    RunOnUiThread([]() {
        FrameworkElement el = PickLiveFlyoutThumbOnThisDispatcher();
        if (el) {
            SchedulePreviewFlyoutRefresh(el);
        }
    });
}

// Expected at startup before any TaskListButton is seen — log once, not per call.
std::atomic<bool> g_loggedNoDispatcherAnchor{false};

// TryRunAsync returns IAsyncOperation<bool> (GetResults = queued). Do not
// static_cast the operation object to bool (always true).
bool DispatcherOpWasQueued(
    winrt::Windows::Foundation::IAsyncOperation<bool> const& op) {
    if (!op) {
        return false;
    }
    if (op.Status() == winrt::Windows::Foundation::AsyncStatus::Completed) {
        try {
            return op.GetResults();
        } catch (...) {
            return false;
        }
    }
    if (op.Status() == winrt::Windows::Foundation::AsyncStatus::Error ||
        op.Status() == winrt::Windows::Foundation::AsyncStatus::Canceled) {
        return false;
    }
    return true;
}

bool DispatcherTryRun(
    winrt::Windows::UI::Core::CoreDispatcher const& dispatcher,
    winrt::Windows::UI::Core::CoreDispatcherPriority prio,
    winrt::Windows::UI::Core::DispatchedHandler const& handler) {
    try {
        return DispatcherOpWasQueued(dispatcher.TryRunAsync(prio, handler));
    } catch (...) {
        return false;
    }
}

// Unload handshake: Completed signals the waiter. The Low callback must not.
// Completed can be set only once; a failure here does not mean the op is idle.
template <typename Op>
bool SubscribeDrainCompleted(Op const& op, HANDLE done) {
    if (!op || !done) {
        return false;
    }
    try {
        op.Completed([done](auto&&, auto&&) { SetEvent(done); });
        return true;
    } catch (...) {
        return false;
    }
}

// Completion and thread termination are different safe boundaries. A failed
// API call, rejected post, or elapsed interval is neither one.
enum class DispatcherOpEnd { Ran, NotRun, ThreadExited };

bool UiThreadExited(const UiDispatcher& ui) {
    return WaitForSingleObject(ui.thread.get(), 0) == WAIT_OBJECT_0;
}

void PauseDispatcherAttempt(const UiDispatcher& ui) {
    if (GetCurrentThreadId() == ui.threadId) {
        try {
            ui.dispatcher.ProcessEvents(
                winrt::Windows::UI::Core::CoreProcessEventsOption::
                    ProcessOneIfPresent);
        } catch (...) {
            // A disconnected object is not proof its thread has terminated.
        }
    }
    // Also wakes immediately if the original owning thread exits.
    if (WaitForSingleObject(ui.thread.get(), 20) == WAIT_FAILED) {
        Sleep(20);  // Wait failure must not authorize unload or busy-spin.
    }
}

// Keep the operation until its result is known or its owning thread exits.
// Completed is the barrier, not a signal from inside the dispatched callback.
// If event creation/subscription fails, poll the same operation instead.
template <typename Op>
DispatcherOpEnd ObserveDispatcherOp(const UiDispatcher& ui, Op const& op,
                                   HANDLE done) {
    if (!op) {
        return DispatcherOpEnd::NotRun;
    }
    if (done) {
        ResetEvent(done);
    }
    const bool subscribed = done && SubscribeDrainCompleted(op, done);
    if (subscribed) {
        HANDLE waits[] = {done, ui.thread.get()};
        for (;;) {
            const DWORD result = WaitForMultipleObjects(2, waits, FALSE, INFINITE);
            if (result == WAIT_OBJECT_0 + 1) {
                return DispatcherOpEnd::ThreadExited;
            }
            if (result == WAIT_OBJECT_0) {
                break;
            }
            // Do not close the event while its completion delegate can run.
            PauseDispatcherAttempt(ui);
        }
    }
    for (;;) {
        if (UiThreadExited(ui)) {
            return DispatcherOpEnd::ThreadExited;
        }
        try {
            const auto status = op.Status();
            if (status == winrt::Windows::Foundation::AsyncStatus::Completed) {
                return op.GetResults() ? DispatcherOpEnd::Ran
                                       : DispatcherOpEnd::NotRun;
            }
            if (status == winrt::Windows::Foundation::AsyncStatus::Error ||
                status == winrt::Windows::Foundation::AsyncStatus::Canceled) {
                return DispatcherOpEnd::NotRun;
            }
        } catch (...) {
            // Retain this operation: inability to observe it is not completion.
        }
        PauseDispatcherAttempt(ui);
    }
}

std::vector<std::shared_ptr<UiDispatcher>> CollectUiDispatchers() {
    std::lock_guard<std::mutex> lock(g_dispatchersMutex);
    if (!g_uiDispatchers) {
        return {};
    }
    return *g_uiDispatchers;
}

enum class DrainProof { SentinelRan, ThreadExited };

BOOL CALLBACK TakeFirstThreadWindow(HWND hwnd, LPARAM out) {
    *reinterpret_cast<HWND*>(out) = hwnd;
    return FALSE;
}

// The sentinel ran, but its thread is still on its way out of mod code: the
// Completed delegate calls SetEvent (which wakes the waiter) before it
// returns, and the delegate and the sentinel are then released, all code in
// this module. A message sent to a window of that thread is handled only
// back in its message loop, after all of that. Unbounded like the rest of
// the drain, but ends if the thread exits.
void WaitUntilUiThreadLeftModCode(const UiDispatcher& ui) {
    if (GetCurrentThreadId() == ui.threadId) {
        return;  // drained on this thread: nothing of it is still running
    }
    bool loggedNoWindow = false;
    for (;;) {
        if (UiThreadExited(ui)) {
            return;
        }
        HWND hwnd = nullptr;
        EnumThreadWindows(ui.threadId, TakeFirstThreadWindow,
                          reinterpret_cast<LPARAM>(&hwnd));
        if (!hwnd) {
            // A XAML UI thread owns windows. If none is found, there is no
            // barrier to send; the brief wait below is all that is left.
            if (!loggedNoWindow) {
                Wh_Log(L"UI drain: thread %u has no window for the return "
                       L"barrier", ui.threadId);
                loggedNoWindow = true;
            }
            PauseDispatcherAttempt(ui);
            return;
        }
        DWORD_PTR result = 0;
        if (SendMessageTimeoutW(hwnd, WM_NULL, 0, 0, SMTO_NORMAL, 100,
                                &result)) {
            return;
        }
        // Timed out (thread busy) or the window just went away: try again.
    }
}

DrainProof DrainOneUiDispatcher(
    const UiDispatcher& ui,
    const winrt::Windows::UI::Core::DispatchedHandler& handler,
    HANDLE done) {
    using AsyncOp = winrt::Windows::Foundation::IAsyncOperation<bool>;
    using Pri = winrt::Windows::UI::Core::CoreDispatcherPriority;
    // Never wait for a dispatcher callback on its own thread without pumping.
    if (GetCurrentThreadId() == ui.threadId) {
        done = nullptr;
    }
    bool highRan = false;
    bool loggedRejection = false;
    for (;;) {
        if (UiThreadExited(ui)) {
            return DrainProof::ThreadExited;
        }
        AsyncOp op{nullptr};
        try {
            op = highRan ? ui.dispatcher.TryRunAsync(Pri::Low, []() {})
                         : ui.dispatcher.TryRunAsync(Pri::High, handler);
        } catch (...) {
            // Includes disconnected HRESULTs: wait for actual thread exit.
        }
        const auto end = ObserveDispatcherOp(ui, op, done);
        if (end == DispatcherOpEnd::ThreadExited) {
            return DrainProof::ThreadExited;
        }
        if (end == DispatcherOpEnd::Ran) {
            if (highRan) {
                WaitUntilUiThreadLeftModCode(ui);
                return DrainProof::SentinelRan;
            }
            highRan = true;
            continue;
        }
        if (!loggedRejection) {
            Wh_Log(L"UI drain: post rejected or failed; waiting for a sentinel "
                   L"or thread %u exit (no unsafe timeout)", ui.threadId);
            loggedRejection = true;
        }
        // A closed dispatcher on a still-live thread may remain here. Keeping
        // the mod loaded is intentional; rejection alone cannot prove safety.
        PauseDispatcherAttempt(ui);
    }
}

// Returns only after each Low sentinel completed or its owning thread exited.
bool RunOnEachUiDispatcherAndWait(
    const winrt::Windows::UI::Core::DispatchedHandler& handler) {
    auto dispatchers = CollectUiDispatchers();
    bool allOk = true;
    for (const auto& ui : dispatchers) {
        winrt::handle done{CreateEventW(nullptr, TRUE, FALSE, nullptr)};
        if (DrainOneUiDispatcher(*ui, handler, done.get()) ==
            DrainProof::ThreadExited) {
            Wh_Log(L"UI drain: original thread %u exited", ui->threadId);
            allOk = false;  // No claim that native visual cleanup ran.
        }
    }
    return allOk;
}

bool RunOnUiThread(const winrt::Windows::UI::Core::DispatchedHandler& handler) {
    if (g_unloading.load()) {
        return false;
    }
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
    for (const auto& ui : dispatchers) {
        if (UiThreadExited(*ui)) {
            continue;
        }
        const auto& dispatcher = ui->dispatcher;
        try {
            if (dispatcher.HasThreadAccess()) {
                handler();
                any = true;
                continue;
            }
            if (DispatcherTryRun(
                    dispatcher,
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


// One queued full pass per UI thread: worker requests (RequestApplyVisuals)
// and UI requests (ScheduleRefreshAllHighlights) coalesce.
// The flag is cleared when the pass starts, so a request made during a pass
// queues the next one.
std::mutex g_fullPassQueuedMutex;
std::unordered_set<DWORD> g_fullPassQueuedThreads;

void ClearFullPassQueued(DWORD threadId) {
    std::lock_guard<std::mutex> lock(g_fullPassQueuedMutex);
    g_fullPassQueuedThreads.erase(threadId);
}

// True if a pass is queued (now or already) on that thread.
bool QueueFullPass(winrt::Windows::UI::Core::CoreDispatcher const& dispatcher,
                   DWORD threadId,
                   winrt::Windows::UI::Core::CoreDispatcherPriority prio) {
    {
        std::lock_guard<std::mutex> lock(g_fullPassQueuedMutex);
        if (!g_fullPassQueuedThreads.insert(threadId).second) {
            return true;
        }
    }
    bool ok = false;
    try {
        ok = DispatcherTryRun(dispatcher, prio, []() {
            ClearFullPassQueued(GetCurrentThreadId());
            try {
                if (!g_unloading.load()) {
                    ApplyAllHighlights_UIThread();
                }
            } catch (...) {
            }
        });
    } catch (...) {
    }
    if (!ok) {
        ClearFullPassQueued(threadId);
    }
    return ok;
}

void RequestApplyVisuals() {
    if (g_unloading.load()) {
        return;
    }
    bool any = false;
    for (const auto& ui : CollectUiDispatchers()) {
        if (UiThreadExited(*ui)) {
            continue;
        }
        try {
            if (ui->dispatcher.HasThreadAccess()) {
                ApplyAllHighlights_UIThread();
                any = true;
            } else if (QueueFullPass(
                           ui->dispatcher, ui->threadId,
                           winrt::Windows::UI::Core::CoreDispatcherPriority::
                               Normal)) {
                any = true;
            }
        } catch (...) {
            HRESULT hr = winrt::to_hresult();
            Wh_Log(L"RequestApplyVisuals error %08X", hr);
        }
    }
    if (!any) {
        // Buttons not tracked yet — will apply on next UpdateVisualStates.
        std::lock_guard<std::mutex> lock(g_stateMutex);
        const auto& ranks = CurrentDeskLocked().rankedApps;
        Wh_Log(L"Ranks ready (%zu) — waiting for TaskListButton hooks",
               ranks.size());
        for (size_t i = 0; i < ranks.size(); i++) {
            Wh_Log(L"  Rank %zu: %s", i + 1, ranks[i].displayName.c_str());
        }
    }
}

void RequestApplyVisualsDebounced() {
    if (g_unloading.load()) {
        return;
    }
    PostToHookThread(WM_APP_REQUEST_APPLY_DEBOUNCED);
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
// Unranked button: an unknown identity needs a full pass; a known one is
// stored as rank 0 so hover repaints skip it.
void MarkButtonUnranked(FrameworkElement button) {
    if (GetCachedButtonIdentity(button).appIdUpper.empty()) {
        ScheduleRefreshAllHighlights(button);
    } else {
        SetCachedPaintState(button, 0, SettingsSnap()->generation);
    }
}

void RefreshButtonHighlight(FrameworkElement button) {
    if (!button || g_unloading.load()) {
        return;
    }

    // Explorer reuses a TaskListButton when the exe is replaced. The cached
    // paint rank still belongs to the old path if that HWND is gone or now
    // belongs to another process. PID only — do not OpenProcess from
    // UpdateVisualStates.
    // A grouped button whose sample window closed while siblings remain is
    // re-captured and re-ranked inline, without a visible off/on.
    auto sampleDead = [](const ButtonIdentity& ident) {
        return ident.sampleHwnd &&
               (!IsWindow(ident.sampleHwnd) ||
                (ident.samplePid &&
                 !HwndMatchesStoredPid(ident.sampleHwnd, ident.samplePid)));
    };
    if (TaskListButton_IsRunning(button) &&
        sampleDead(GetCachedButtonIdentity(button))) {
        EnsureButtonIdentity(button);
        if (sampleDead(GetCachedButtonIdentity(button))) {
            if (ButtonHasOurChrome(button)) {
                ClearButtonHighlight(button);
            }
            SetCachedPaintState(button, -1, SettingsSnap()->generation);
            ScheduleRefreshAllHighlights(button);
            return;
        }
    }

    // Closed / pinned-not-running: drop chrome here and rebind so the slot
    // frees. Read the rank first — clear stores 0.
    // Rank eligibility retains grace; visuals follow IsRunning immediately.
    if (!ButtonCountsAsRunning(button)) {
        const int cached = GetCachedPaintState(button).rank;
        if (ButtonHasOurChrome(button)) {
            ClearButtonHighlight(button);
        }
        if (cached > 0) {
            SetCachedPaintState(button, 0, SettingsSnap()->generation);
            ScheduleRefreshAllHighlights(button);
        } else if (cached < 0) {
            MarkButtonUnranked(button);
        }
        return;
    }

    // Sweep is ApplyAllHighlights. Clearing here blanks ranked icons until
    // that pass (desktop switch / decay flicker).
    int rank = GetCachedPaintState(button).rank;
    if (rank < 0) {
        std::vector<AppFocusInfo> ranks;
        {
            std::lock_guard<std::mutex> lock(g_stateMutex);
            ranks = CurrentDeskLocked().rankedApps;
        }
        if (ranks.empty()) {
            if (ButtonHasOurChrome(button)) {
                ClearButtonHighlight(button);
            } else {
                MarkButtonUnranked(button);
            }
            return;
        }
        rank = FindRankForButton(button, ranks);
    }

    if (rank > 0) {
        ApplyButtonHighlight(button, rank);
    } else if (ButtonHasOurChrome(button)) {
        ClearButtonHighlight(button);
    } else {
        MarkButtonUnranked(button);
    }
}

// Full identity rebind on the anchor's UI thread (Low priority, debounced).
// The anchor only selects the dispatcher. The queued bind still runs if
// that button is destroyed before Low priority.
void ScheduleRefreshAllHighlights(FrameworkElement dispatcherAnchor) {
    // Only on a dispatcher the unload drain knows.
    if (!dispatcherAnchor || g_unloading.load() ||
        !RememberUiDispatcher(dispatcherAnchor)) {
        return;
    }
    const ULONGLONG now = GetTickCount64();
    if (now - g_lastFullRefreshTick < kFullRebindDebounceMs &&
        g_lastFullRefreshTick != 0) {
        PostToHookThread(WM_APP_REQUEST_APPLY_DEBOUNCED);
        return;
    }
    try {
        auto dispatcher = dispatcherAnchor.Dispatcher();
        if (!dispatcher ||
            !QueueFullPass(
                dispatcher, GetCurrentThreadId(),
                winrt::Windows::UI::Core::CoreDispatcherPriority::Low)) {
            PostToHookThread(WM_APP_REQUEST_APPLY_DEBOUNCED);
            return;
        }
        g_lastFullRefreshTick = now;
    } catch (...) {
        PostToHookThread(WM_APP_REQUEST_APPLY_DEBOUNCED);
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

    // Nothing may throw into Explorer's frames.
    try {
        TrackButton_UIThread(button);
        const bool seeded = SeedButtonsFromRepeater_UIThread(button);
        RefreshButtonHighlight(button);
        // Only an unknown rank (-1) or a pending sweep needs a full pass. Our
        // overlays are own elements that Explorer's hover states do not
        // reset, so a ranked button does not re-bind every sibling.
        const int rank = GetCachedPaintState(button).rank;
        if (seeded || rank < 0 || g_pendingOverlaySweep.load()) {
            ScheduleRefreshAllHighlights(button);
        }
    } catch (...) {
    }
}

// XAML thumbnail view template — apply preview glow when flyout builds items.
using TaskItemThumbnailView_OnApplyTemplate_t = void(WINAPI*)(void* pThis);
TaskItemThumbnailView_OnApplyTemplate_t
    TaskItemThumbnailView_OnApplyTemplate_Original;
void WINAPI TaskItemThumbnailView_OnApplyTemplate_Hook(void* pThis) {
    if (TaskItemThumbnailView_OnApplyTemplate_Original) {
        TaskItemThumbnailView_OnApplyTemplate_Original(pThis);
    }
    if (g_unloading.load() || !SettingsSnap()->previewHighlightEnabled) {
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
            TrackThumbView_UIThread(element);
            SchedulePreviewFlyoutRefresh(element);
        }
    } catch (...) {
        HRESULT hr = winrt::to_hresult();
        Wh_Log(L"TaskItemThumbnailView_OnApplyTemplate error %08X", hr);
    }
}

// A card was bound to a (possibly different) window: realized, recycled by
// the ItemsRepeater, or rebound after another card was removed. OnApplyTemplate
// only runs on first realization, so without this a reused card kept the
// previous window's rank paint until the flyout reopened.
using TaskItemThumbnailView_put_ThumbnailSource_t = int32_t(WINAPI*)(void* pThis,
                                                                     void* source);
TaskItemThumbnailView_put_ThumbnailSource_t
    TaskItemThumbnailView_put_ThumbnailSource_Original;
int32_t WINAPI TaskItemThumbnailView_put_ThumbnailSource_Hook(void* pThis,
                                                              void* source) {
    const int32_t result =
        TaskItemThumbnailView_put_ThumbnailSource_Original(pThis, source);
    if (g_unloading.load() || !SettingsSnap()->previewHighlightEnabled) {
        return result;
    }
    try {
        FrameworkElement card = nullptr;
        static_cast<::IUnknown*>(pThis)->QueryInterface(
            winrt::guid_of<FrameworkElement>(), winrt::put_abi(card));
        if (card) {
            // Tracking records the card's dispatcher for the unload drain.
            TrackThumbView_UIThread(card);
            // Coalesced: a flyout binding N cards schedules one refresh.
            SchedulePreviewFlyoutRefresh(card);
        }
    } catch (...) {
    }
    return result;
}


using HoverFlyoutModel_TargetItemKey_t = void(WINAPI*)(void* pThis,
                                                       void* param1);
extern HoverFlyoutModel_TargetItemKey_t HoverFlyoutModel_TargetItemKey_Original;
void WINAPI HoverFlyoutModel_TargetItemKey_Hook(void* pThis, void* param1);

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
        {
            // Optional: flyout retargeted to another button; clear and
            // repaint its cards (symbol as in taskbar-thumbnail-reorder).
            {LR"(private: void __cdecl winrt::Taskbar::implementation::HoverFlyoutModel::TargetItemKey(struct winrt::hstring const &))"},
            &HoverFlyoutModel_TargetItemKey_Original,
            HoverFlyoutModel_TargetItemKey_Hook,
            true,
        },
        {
            // Optional: card view model -> window (preview resolve pass 0).
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskItemThumbnailView,struct winrt::Taskbar::ITaskItemThumbnailView>::get_ThumbnailSource(void * *))"},
            &TaskItemThumbnailView_get_ThumbnailSource,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskItemThumbnailViewModel,struct winrt::Taskbar::ITaskItemThumbnailViewModel>::get_TaskItemThumbnail(void * *))"},
            &TaskItemThumbnailViewModel_get_TaskItemThumbnail,
            nullptr,
            true,
        },
        {
            // Optional: repaint when a card is rebound to another window.
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskItemThumbnailView,struct winrt::Taskbar::ITaskItemThumbnailView>::put_ThumbnailSource(void *))"},
            &TaskItemThumbnailView_put_ThumbnailSource_Original,
            TaskItemThumbnailView_put_ThumbnailSource_Hook,
            true,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed for Taskbar.View.dll");
        return false;
    }
    // Button identity (TaskListButton -> view model -> WindowsUdk TaskItem /
    // TaskGroup) lives here.
    g_taskbandResolveReady = true;

    Wh_Log(L"Preview resolve: view model %s, rebind repaint %s",
           (TaskItemThumbnailView_get_ThumbnailSource &&
            TaskItemThumbnailViewModel_get_TaskItemThumbnail)
               ? L"on"
               : L"unavailable",
           TaskItemThumbnailView_put_ThumbnailSource_Original ? L"on"
                                                              : L"unavailable");

    if (TaskItemThumbnailView_OnApplyTemplate_Original) {
        Wh_Log(L"Hooked Taskbar.View.dll symbols (identity + paint + thumbnails)");
    } else {
        Wh_Log(L"Hooked Taskbar.View.dll symbols (identity + paint; "
               L"thumbnail OnApplyTemplate unavailable)");
    }
    return true;
}

// Per UI thread; emptied by ReleaseUiThreadLocals_UIThread at unload.
thread_local std::wstring t_lastFlyoutKey;
HoverFlyoutModel_TargetItemKey_t HoverFlyoutModel_TargetItemKey_Original;
void WINAPI HoverFlyoutModel_TargetItemKey_Hook(void* pThis, void* param1) {
    // Flyout retargeted to another button: clear and repaint (below).
    HoverFlyoutModel_TargetItemKey_Original(pThis, param1);
    if (g_unloading.load() || !SettingsSnap()->previewHighlightEnabled) {
        return;
    }
    // Repeater may not have swapped realized cards yet. Clear leftover chrome
    // now and paint at Low after layout so new HWNDs are not bound to the
    // previous app's views.
    try {
        // Explorer sets the same key repeatedly while one button's flyout
        // stays open; clearing every tracked card each time was the cost.
        // Only a different button needs the clear.
        std::wstring key;
        if (param1) {
            key = reinterpret_cast<const winrt::hstring*>(param1)->c_str();
        }
        // An empty key (flyout closing) clears once, and has no cards to
        // paint.
        const bool empty = key.empty();
        if (empty ? !t_lastFlyoutKey.empty() : key != t_lastFlyoutKey) {
            ClearAllThumbnailHighlights_UIThread();
        }
        t_lastFlyoutKey = std::move(key);
        if (empty) {
            return;
        }
        FrameworkElement el = PickLiveFlyoutThumbOnThisDispatcher();
        if (el) {
            SchedulePreviewFlyoutRefresh(el);
        }
    } catch (...) {
    }
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }
    return module;
}

void HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR lpLibFileName) {
    if (g_unloading.load() || g_taskbarViewHookAttempted.load() ||
        GetTaskbarViewModuleHandle() != module ||
        g_taskbarViewHookAttempted.exchange(true)) {
        return;
    }
    Wh_Log(L"Loaded %s", lpLibFileName);
    // Claim once before resolving symbols, including when resolution fails.
    if (HookTaskbarViewDllSymbols(module)) {
        Wh_ApplyHookOperations();
        g_taskbarViewHooked = true;
        PostToHookThread(WM_APP_TASKBAR_READY);
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
    DisarmHookTimer(kMinFocusTimerId);
}

void CancelPreviewMinFocusTimer() {
    DisarmHookTimer(kPreviewMinFocusTimerId);
}

// True if pending is still the focused app. May retarget to a new top-level
// window of the same PID. False if focus left the process.
bool StillPendingForeground(const PendingFocus& pending,
                            HWND* outHwnd,
                            std::wstring* outTitle) {
    // Same mapping as the focus path (child -> root, owned dialog -> owner),
    // so an owned dialog keeps its owner's pending focus alive. Raw HWND
    // here would make a Lister dialog look like another app, and its owner
    // would never be confirmed.
    HWND foreground = NormalizeFocusHwnd(GetForegroundWindow());
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
        // Desktop/tray share explorer.exe with folder windows. Same PID is
        // not enough — those HWNDs must take the transient-wait path.
        if (IsTransientForeground(foreground)) {
            return false;
        }
        // Same PID is not the same app when Explorer puts the windows on
        // different taskbar buttons (UWP hosts; TC main window vs Lister).
        {
            const std::wstring fgId = LookupWindowAppId(foreground);
            std::wstring pendingId = AppIdFromAppKey(pending.key);
            if (pendingId.empty()) {
                pendingId = LookupWindowAppId(pending.hwnd);
            }
            if (fgId.empty() || fgId != pendingId) {
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

// The app and preview min-focus confirmations are one state machine: the
// deadline, the still-foreground check with the bounded
// Alt-Tab grace, then a track-specific confirm. A track names its timer,
// its seconds, its start tick and its retry field.
struct MinFocusTrack {
    bool preview = false;
    UINT_PTR timerId = 0;
    int seconds = 0;
    ULONGLONG start = 0;
    ULONGLONG PendingFocus::*retryStart = nullptr;
    PCWSTR logName = L"";
};

MinFocusTrack AppTrack(const PendingFocus& p, const Settings& s) {
    return {false, kMinFocusTimerId, s.minFocusSeconds, p.focusStartTick,
            &PendingFocus::transientRetryStartTick, L"Min-focus timer"};
}

MinFocusTrack PreviewTrack(const PendingFocus& p, const Settings& s) {
    return {true, kPreviewMinFocusTimerId, s.previewMinFocusSeconds,
            p.previewStartTick ? p.previewStartTick : p.focusStartTick,
            &PendingFocus::previewTransientRetryStartTick,
            L"Preview min-focus"};
}

// Copies the pending focus. False if there is none, or if it is excluded
// (then it is dropped).
bool TakePendingForConfirm(PendingFocus& pending) {
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        if (!g_pendingFocus.valid) {
            return false;
        }
        pending = g_pendingFocus;
    }
    std::wstring pendingAppId = pending.appIdUpper;
    if (pendingAppId.empty() && pending.hwnd) {
        pendingAppId = ExclusionAumid(pending.hwnd);
    }
    if (IsExcludedKey(pending.key, ToUpper(pending.displayName), pendingAppId,
                      pending.pathUpper)) {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        if (g_pendingFocus.hwnd == pending.hwnd) {
            g_pendingFocus = {};
        }
        return false;
    }
    return true;
}

// Drops the pending focus (app track only: a preview track leaves the app
// timer running).
void DropPendingFor(const MinFocusTrack& track, const PendingFocus& pending) {
    if (track.preview) {
        return;
    }
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        if (g_pendingFocus.hwnd == pending.hwnd) {
            g_pendingFocus = {};
        }
    }
    StopDecayTimerIfIdle();
}

// True when the track may confirm now, with the window to confirm (it may
// be another top-level window of the same app). Otherwise the deadline was
// re-armed, Alt-Tab grace continues, or the candidate was dropped.
bool MinFocusDeadlineReached(const MinFocusTrack& track,
                             const PendingFocus& pending,
                             MinFocusConfirmMode mode,
                             HWND* confirmHwnd,
                             std::wstring* title) {
    if (mode == MinFocusConfirmMode::FromTimer) {
        // KillTimer does not dequeue a WM_TIMER already posted.
        const ULONGLONG remaining =
            RemainingDeadlineMs(track.start, track.seconds, GetTickCount64());
        if (remaining > 0) {
            ArmHookTimer(track.timerId, remaining);
            return false;
        }
    }
    if (StillPendingForeground(pending, confirmHwnd, title)) {
        return true;
    }
    if (mode == MinFocusConfirmMode::FromTimer &&
        IsTransientForeground(GetForegroundWindow())) {
        // Alt-Tab / taskbar holds FG briefly: keep the candidate through a
        // bounded grace, then drop.
        ULONGLONG retryStart = pending.*track.retryStart;
        const bool firstWait = retryStart == 0;
        const ULONGLONG delay = TransientRetryDelayMs(
            track.start, track.seconds, GetTickCount64(), retryStart);
        {
            std::lock_guard<std::mutex> lock(g_stateMutex);
            if (g_pendingFocus.valid && g_pendingFocus.hwnd == pending.hwnd) {
                g_pendingFocus.*track.retryStart = retryStart;
            }
        }
        if (delay == 0) {
            const ULONGLONG waited =
                retryStart ? GetTickCount64() - retryStart : 0;
            Wh_Log(L"%s: gave up waiting through transient FG for %s "
                   L"(after %llus)",
                   track.logName, pending.displayName.c_str(),
                   static_cast<unsigned long long>(waited / 1000));
            DropPendingFor(track, pending);
            return false;
        }
        ArmHookTimer(track.timerId, delay);
        // Only the start of a wait: it re-polls every 200 ms. It ends in a
        // confirmation or the "gave up" line.
        if (!track.preview && firstWait) {
            Wh_Log(L"Min-focus timer: transient FG, waiting on %s",
                   pending.displayName.c_str());
        }
        return false;
    }
    if (!track.preview) {
        Wh_Log(L"Min-focus timer: focus left %s before confirmation",
               pending.displayName.c_str());
    }
    DropPendingFor(track, pending);
    return false;
}

void OnPreviewMinFocusTimerElapsed(MinFocusConfirmMode mode) {
    auto settings = SettingsSnap();
    if (!settings->previewHighlightEnabled || g_unloading.load()) {
        return;
    }

    PendingFocus pending;
    if (!TakePendingForConfirm(pending)) {
        return;
    }
    const MinFocusTrack track = PreviewTrack(pending, *settings);
    HWND confirmHwnd = nullptr;
    std::wstring title;
    if (!MinFocusDeadlineReached(track, pending, mode, &confirmHwnd, &title)) {
        return;  // re-armed, waiting through Alt-Tab, or focus left
    }

    std::wstring pendingAppId = ExclusionAumid(confirmHwnd);
    if (IsExcludedKey(pending.key, ToUpper(pending.displayName), pendingAppId,
                      pending.pathUpper)) {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        if (g_pendingFocus.hwnd == pending.hwnd) {
            g_pendingFocus.previewConfirmed = true;
            if (g_pendingFocus.appConfirmed) {
                g_pendingFocus = {};
            }
        }
        return;
    }

    const ULONGLONG now = GetTickCount64();
    pending.desktopId = ConfirmDesktopId(pending.desktopId, confirmHwnd);
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        auto& desk =
            InlineIsEqualGUID(pending.desktopId, GUID_NULL)
                ? CurrentDeskLocked()
                : DeskStateLocked(pending.desktopId);
        StampWindowRecencyLocked(desk, confirmHwnd, pending.key, title, now,
                                 pendingAppId, pending.pathUpper);
        if (g_pendingFocus.valid && g_pendingFocus.hwnd == pending.hwnd) {
            g_pendingFocus.previewConfirmed = true;
            if (g_pendingFocus.appConfirmed) {
                g_pendingFocus = {};
            }
        }
        Wh_Log(L"Preview focus confirmed: hwnd=%p %s title=\"%s\" (map=%zu "
               L"desktop=%s)",
               confirmHwnd, pending.displayName.c_str(), title.c_str(),
               desk.windowFocusMap.size(),
               GuidToLogString(pending.desktopId).c_str());
    }

    EnsureDecayTimerArmed();
    StopDecayTimerIfIdle();
    RequestApplyPreviewVisuals();
}


void OnMinFocusTimerElapsed(MinFocusConfirmMode mode) {
    auto settings = SettingsSnap();
    if (g_unloading.load()) {
        return;
    }
    PendingFocus pending;
    if (!TakePendingForConfirm(pending)) {
        return;
    }
    const MinFocusTrack track = AppTrack(pending, *settings);
    HWND confirmHwnd = nullptr;
    std::wstring title;
    if (!MinFocusDeadlineReached(track, pending, mode, &confirmHwnd, &title)) {
        return;  // re-armed, waiting through Alt-Tab, or dropped
    }
    // The deadline check can retarget to a sibling window of the app; the
    // pending focus is still keyed by the original one.
    const HWND pendingHwnd = pending.hwnd;
    pending.hwnd = confirmHwnd;
    if (!title.empty()) {
        pending.windowTitle = std::move(title);
    }

    const std::wstring appIdUpper =
        ExclusionAumid(pending.hwnd);
    if (IsExcludedKey(pending.key, ToUpper(pending.displayName), appIdUpper,
                      pending.pathUpper)) {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        if (g_pendingFocus.hwnd == pendingHwnd) {
            g_pendingFocus = {};
        }
        return;
    }

    const ULONGLONG now = GetTickCount64();
    pending.desktopId = ConfirmDesktopId(pending.desktopId, pending.hwnd);
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        auto& desk =
            InlineIsEqualGUID(pending.desktopId, GUID_NULL)
                ? CurrentDeskLocked()
                : DeskStateLocked(pending.desktopId);
        // The window record is the recency; the recompute below folds it
        // into its app once Explorer's AppId for the window is known.
        WindowAppFocus& w = desk.windowAppFocus[pending.hwnd];
        w.displayName = pending.displayName;
        if (!pending.windowTitle.empty()) {
            w.title = pending.windowTitle;
        }
        w.appIdUpper = appIdUpper;
        w.pathUpper = pending.pathUpper;
        w.pid = pending.processId ? pending.processId
                                  : PidFromHwnd(pending.hwnd);
        w.tick = now;

        // Preview min-focus is independent: do not stamp the current HWND
        // here. A window switch just before the app deadline must wait out
        // previewStartTick (clicks too).
        if (g_pendingFocus.valid &&
            (g_pendingFocus.hwnd == pending.hwnd ||
             g_pendingFocus.processId == pending.processId)) {
            g_pendingFocus.appConfirmed = true;
            if (!settings->previewHighlightEnabled ||
                g_pendingFocus.previewConfirmed) {
                g_pendingFocus = {};
            }
        }

        RecomputeRanksForDesktopLocked(desk);
        auto rec = desk.windowAppFocus.find(pending.hwnd);
        const std::wstring derived =
            rec == desk.windowAppFocus.end() || rec->second.key.empty()
                ? std::wstring(L"(no button yet)")
                : rec->second.key;
        Wh_Log(L"Confirmed focus: %s window=%p app=%s (apps=%zu, "
               L"windows=%zu, ranks=%zu, title=\"%s\" desktop=%s)",
               pending.displayName.c_str(), pending.hwnd, derived.c_str(),
               desk.appFocusMap.size(), desk.windowAppFocus.size(),
               desk.rankedApps.size(),
               pending.windowTitle.c_str(),
               GuidToLogString(pending.desktopId).c_str());
    }

    EnsureDecayTimerArmed();

    // The preview track may have given up during a long Alt-Tab while the
    // app track kept waiting; the window is in front again now, so let it
    // confirm too (its deadline has passed: FromTimer re-checks the
    // foreground). Otherwise this visit left no preview recency.
    if (settings->previewHighlightEnabled) {
        bool previewPending = false;
        {
            std::lock_guard<std::mutex> lock(g_stateMutex);
            previewPending = g_pendingFocus.valid &&
                             g_pendingFocus.appConfirmed &&
                             !g_pendingFocus.previewConfirmed;
        }
        if (previewPending) {
            EnsurePendingPreviewTimer();
        }
    }

    // The UI pass re-binds ranks to buttons; a rank with no button paints
    // nothing but keeps its history.
    RequestApplyVisuals();
}

// Arms the track's timer for the rest of its deadline, or confirms now.
// An already due deadline confirms FromTimer, which still honors the
// Alt-Tab grace; Immediate would drop the candidate while the switcher
// holds foreground.
void ArmOrConfirmTrack(const MinFocusTrack& track,
                       void (*elapsed)(MinFocusConfirmMode)) {
    if (track.seconds <= 0) {
        elapsed(MinFocusConfirmMode::Immediate);
        return;
    }
    const ULONGLONG remaining =
        RemainingDeadlineMs(track.start, track.seconds, GetTickCount64());
    if (remaining == 0) {
        elapsed(MinFocusConfirmMode::FromTimer);
        return;
    }
    ArmHookTimer(track.timerId, remaining);
}

// Keep the app min-focus one-shot alive. A stale WM_TIMER KillTimer's the
// live timer, so same-app FOREGROUND cannot assume it is still running.
void EnsurePendingAppTimer() {
    PendingFocus pending;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        if (!g_pendingFocus.valid || g_pendingFocus.appConfirmed) {
            return;
        }
        pending = g_pendingFocus;
    }
    ArmOrConfirmTrack(AppTrack(pending, *SettingsSnap()),
                      OnMinFocusTimerElapsed);
}

void EnsurePendingPreviewTimer() {
    PendingFocus pending;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        if (!g_pendingFocus.valid || g_pendingFocus.previewConfirmed) {
            return;
        }
        pending = g_pendingFocus;
    }
    auto settings = SettingsSnap();
    if (!settings->previewHighlightEnabled) {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        if (g_pendingFocus.valid &&
            g_pendingFocus.hwnd == pending.hwnd) {
            g_pendingFocus.previewConfirmed = true;
            if (g_pendingFocus.appConfirmed) {
                g_pendingFocus = {};
            }
        }
        CancelPreviewMinFocusTimer();
        return;
    }
    ArmOrConfirmTrack(PreviewTrack(pending, *settings),
                      OnPreviewMinFocusTimerElapsed);
}

void SchedulePreviewConfirm(bool windowAlreadyTracked) {
    if (!SettingsSnap()->previewHighlightEnabled) {
        return;
    }
    CancelPreviewMinFocusTimer();
    const int previewMin = (std::max)(0, SettingsSnap()->previewMinFocusSeconds);
    if (previewMin <= 0 || windowAlreadyTracked) {
        OnPreviewMinFocusTimerElapsed(MinFocusConfirmMode::Immediate);
        return;
    }
    ArmHookTimer(kPreviewMinFocusTimerId,
                 static_cast<ULONGLONG>(previewMin) * 1000ULL);
}

// ---------------------------------------------------------------------------
// Last use: decay counts from when an app was last *used*, not from
// when it was switched to. While a confirmed window is in front, the decay
// tick moves its recency forward to the last keyboard / mouse input
// (GetLastInputInfo, session-wide); when it loses the foreground it is
// stamped with the last input before that. Walking away stops the input
// time, so everything still ages out from the last input.
// Only a stretch in front in which the window was confirmed counts: a visit
// shorter than the minimum focus refreshes nothing, so it cannot jump ranks.
// Focus thread only.
// ---------------------------------------------------------------------------
HWND g_inFrontHwnd = nullptr;
ULONGLONG g_inFrontSince = 0;

// GetTickCount64 time of the last input in this session, 0 if unknown.
ULONGLONG LastInputTick64(ULONGLONG now) {
    LASTINPUTINFO lii{};
    lii.cbSize = sizeof(lii);
    if (!GetLastInputInfo(&lii)) {
        return 0;
    }
    // dwTime is GetTickCount (32-bit); the difference survives wrap. Input
    // after `now` was read (a keystroke between the two calls) makes it
    // negative: that is "now", not 49.7 days ago.
    const int32_t ago =
        static_cast<int32_t>(static_cast<DWORD>(now) - lii.dwTime);
    if (ago <= 0) {
        return now;
    }
    return static_cast<ULONGLONG>(ago) < now ? now - ago : 0;
}

// What RefreshLastUseLocked moved (for the log).
constexpr unsigned kLastUseMovedApp = 1;
constexpr unsigned kLastUseMovedPreview = 2;

PCWSTR LastUseMovedName(unsigned moved) {
    switch (moved) {
        case kLastUseMovedApp:
            return L"app";
        case kLastUseMovedPreview:
            return L"preview";
        default:
            return L"app+preview";
    }
}

// Requires g_stateMutex. Moves hwnd's app and preview recency forward to
// `at`, only for records confirmed during the current stretch in front
// (tick >= since). Returns the kLastUseMoved* bits; 0 if nothing moved.
// The app record needs the app minimum focus in this stretch; the preview
// one only the preview minimum (often 0 s), so a short visit moves only
// "preview".
unsigned RefreshLastUseLocked(HWND hwnd, ULONGLONG since, ULONGLONG at) {
    unsigned moved = 0;
    for (auto& [id, desk] : g_desktopMaps) {
        (void)id;
        auto w = desk.windowAppFocus.find(hwnd);
        if (w != desk.windowAppFocus.end() && w->second.tick >= since &&
            w->second.tick < at && HwndMatchesStoredPid(hwnd, w->second.pid)) {
            w->second.tick = at;
            moved |= kLastUseMovedApp;
        }
        auto p = desk.windowFocusMap.find(hwnd);
        if (p != desk.windowFocusMap.end() &&
            p->second.lastConfirmedTick >= since &&
            p->second.lastConfirmedTick < at &&
            HwndMatchesStoredPid(hwnd, p->second.pid)) {
            p->second.lastConfirmedTick = at;
            moved |= kLastUseMovedPreview;
        }
    }
    return moved;
}

// A new real foreground: stamp the window that left with its last use, then
// start the new stretch. Not called for transient windows (Alt+Tab, Start,
// taskbar, desktop, lock screen): Windows often sends no second foreground
// event when the user lands back on the same window, and the stretch stayed
// on the switcher, so the app in use was never refreshed again and decayed
// while used. While a transient window is in front the
// decay tick refreshes nothing (it is not g_inFrontHwnd); leaving for
// another app stamps the last input, a switch's own keystrokes included.
void NoteForegroundForLastUse(HWND hwnd) {
    if (hwnd == g_inFrontHwnd) {
        return;
    }
    const ULONGLONG now = GetTickCount64();
    if (g_inFrontHwnd) {
        const ULONGLONG at = LastInputTick64(now);
        if (at) {
            std::lock_guard<std::mutex> lock(g_stateMutex);
            if (const unsigned moved = RefreshLastUseLocked(
                    g_inFrontHwnd, g_inFrontSince, at)) {
                Wh_Log(L"Last use: hwnd=%p left, used until %llus ago "
                       L"(%s)",
                       g_inFrontHwnd,
                       static_cast<unsigned long long>((now - at) / 1000),
                       LastUseMovedName(moved));
            }
        }
    }
    g_inFrontHwnd = hwnd;
    g_inFrontSince = now;
}

// Decay tick: the confirmed window in front was used up to the last input.
void RefreshInFrontLastUse() {
    HWND fg = NormalizeFocusHwnd(GetForegroundWindow());
    if (!fg || fg != g_inFrontHwnd) {
        return;  // a change the foreground handler has not seen yet
    }
    const ULONGLONG at = LastInputTick64(GetTickCount64());
    if (!at) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_stateMutex);
    RefreshLastUseLocked(fg, g_inFrontSince, at);
}

void HandleForegroundChanged(HWND hWnd) {
    if (g_unloading.load()) {
        return;
    }
    ProcessImagePathCacheScope pathCacheScope;

    hWnd = NormalizeFocusHwnd(hWnd);
    const bool transient = IsTransientForeground(hWnd);
    if (!transient) {
        NoteForegroundForLastUse(hWnd);
    }

    // Normally the registry watch on the focus thread reports switches.
    // Without it, read here, before the transient check: switching to an
    // empty desktop gives the desktop / taskbar foreground, which is
    // transient. Every reader that sees the GUID change runs the full
    // switch steps.
    if (!g_desktopKeyWatchActive && RefreshCurrentDesktopId()) {
        OnVirtualDesktopSwitched(L"foreground");
    }

    if (transient) {
        // Alt-Tab frame, taskbar, desktop, IME. Do not cancel min-focus.
        bool ranksNonEmpty = false;
        {
            std::lock_guard<std::mutex> lock(g_stateMutex);
            ranksNonEmpty = !CurrentDeskLocked().rankedApps.empty();
        }
        if (ranksNonEmpty) {
            RequestApplyVisualsDebounced();
        }
        return;
    }

    const std::optional<AppIdentity> resolved = ResolveAppIdentity(hWnd);
    if (!resolved) {
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
            RequestApplyVisualsDebounced();
        }
        return;
    }

    const std::wstring& key = resolved->key;
    const std::wstring& displayName = resolved->displayName;
    const std::wstring& windowTitle = resolved->windowTitle;
    const std::wstring& appIdUpper = resolved->appIdUpper;
    const std::wstring& pathUpper = resolved->pathUpper;
    const DWORD processId = resolved->processId;
    if (resolved->noButton) {
        // Provisional key until a captured button lists this window: ask
        // for a full pass (with a repeater re-seed) so a button that exists
        // is captured now.
        ++g_buttonSeedGeneration;
        RequestApplyVisuals();
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
        // Decay, not the 30 s prune, ends immediate re-focus. A tick that is
        // already past decayMinutes waits the minimum again.
        if (it != desk.appFocusMap.end() &&
            it->second.lastConfirmedFocusTick != 0) {
            const ULONGLONG decayMs =
                DecayMsFromMinutes(SettingsSnap()->decayMinutes);
            alreadyTracked = !IsTickDecayed(it->second.lastConfirmedFocusTick,
                                            decayMs, now);
        }
        // A window confirmed before whose button is not known yet is
        // tracked too (its record joins the app at the next recompute).
        if (!alreadyTracked) {
            auto w = desk.windowAppFocus.find(hWnd);
            if (w != desk.windowAppFocus.end() &&
                HwndMatchesStoredPid(hWnd, w->second.pid)) {
                alreadyTracked = !IsTickDecayed(
                    w->second.tick,
                    DecayMsFromMinutes(SettingsSnap()->decayMinutes), now);
            }
        }
        // Log title only. Identity is the AppId, not the window title.
        if (it != desk.appFocusMap.end() && !windowTitle.empty()) {
            it->second.lastWindowTitle = windowTitle;
        }
        // Same live-window rule as preview ranking: PID, liveness, decay.
        windowAlreadyTracked = IsWindowRecentForPreviewLocked(desk, hWnd);
        if (windowAlreadyTracked && !windowTitle.empty()) {
            desk.windowFocusMap[hWnd].windowTitle = windowTitle;
        }
    }

    // Always repaint existing ranks on any focus change (Alt-Tab must not
    // leave other ranked icons unstyled until the min-focus timer fires).
    // Debounce: transient FG + landed app would otherwise full-rebind twice.
    if (ranksNonEmpty || alreadyTracked) {
        RequestApplyVisualsDebounced();
    }

    bool sameAppPending = false;
    bool hwndChanged = false;
    GUID deskForLog{};
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);

        // Same app as the pending focus: same key, or the same window whose
        // button became known meanwhile (WINDOW:<hwnd> -> APPID:), which
        // must not restart the app wait.
        const bool sameApp =
            g_pendingFocus.key == key ||
            (g_pendingFocus.hwnd == hWnd &&
             g_pendingFocus.processId == processId &&
             g_pendingFocus.key.rfind(L"WINDOW:", 0) == 0);
        if (g_pendingFocus.valid && sameApp &&
            InlineIsEqualGUID(g_pendingFocus.desktopId, g_currentDesktopId)) {
            g_pendingFocus.key = key;
            hwndChanged = g_pendingFocus.hwnd != hWnd;
            g_pendingFocus.hwnd = hWnd;
            g_pendingFocus.processId = processId;
            g_pendingFocus.transientRetryStartTick = 0;
            g_pendingFocus.previewTransientRetryStartTick = 0;
            if (!windowTitle.empty()) {
                g_pendingFocus.windowTitle = windowTitle;
            }
            if (!appIdUpper.empty()) {
                g_pendingFocus.appIdUpper = appIdUpper;
            }
            if (!pathUpper.empty()) {
                g_pendingFocus.pathUpper = pathUpper;
            }
            if (hwndChanged) {
                g_pendingFocus.previewStartTick = now;
                g_pendingFocus.previewConfirmed = false;
                // The app confirm stamped the previous window only; confirm
                // again so this one gets its own record (last use, Never
                // Combine slot). Its deadline counts from the app's start,
                // usually already passed.
                g_pendingFocus.appConfirmed = false;
            }
            sameAppPending = true;
        } else {
            PendingFocus next;
            next.hwnd = hWnd;
            next.processId = processId;
            next.key = key;
            next.displayName = displayName;
            next.windowTitle = windowTitle;
            next.appIdUpper = appIdUpper;
            next.pathUpper = pathUpper;
            next.focusStartTick = now;
            next.previewStartTick = now;
            next.desktopId = g_currentDesktopId;
            next.valid = true;
            g_pendingFocus = std::move(next);
        }
        deskForLog = g_pendingFocus.desktopId;
    }

    if (sameAppPending) {
        // Same app: keep app min-focus timer; re-schedule preview if HWND moved
        // between instances (multi-window VS Code / Terminal).
        if (hwndChanged) {
            SchedulePreviewConfirm(windowAlreadyTracked);
        } else {
            // WinEvent and shell activation can describe the same episode.
            // Preserve its deadline rather than restarting the preview wait.
            EnsurePendingPreviewTimer();
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

    ArmHookTimer(kMinFocusTimerId, static_cast<ULONGLONG>(minSeconds) * 1000ULL);
}

void OnDecayTimer() {
    if (g_unloading.load()) {
        return;
    }
    // A tick far past its interval: the machine slept (GetTickCount64 counts
    // sleep, so everything idle that long decays now) or the thread stalled.
    // Logged, since nothing else would show it.
    {
        const ULONGLONG now = GetTickCount64();
        if (g_lastDecayTick &&
            now - g_lastDecayTick > kDecayCheckIntervalMs + 60000) {
            Wh_Log(L"Decay tick late by %llu min (sleep or stall)",
                   static_cast<unsigned long long>(
                       (now - g_lastDecayTick - kDecayCheckIntervalMs) /
                       60000));
        }
        g_lastDecayTick = now;
    }
    std::vector<std::wstring> keysBefore;
    std::vector<std::wstring> keysAfter;
    size_t windowsBefore = 0;
    size_t windowsAfter = 0;
    if (RefreshCurrentDesktopId()) {
        OnVirtualDesktopSwitched(L"decay tick");  // it saw the switch first
    }
    RefreshInFrontLastUse();  // before decay: the app in use is not idle
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        const auto& beforeApps = CurrentDeskLocked().rankedApps;
        keysBefore.reserve(beforeApps.size());
        for (const auto& app : beforeApps) {
            keysBefore.push_back(app.key);
        }
        windowsBefore = CurrentDeskLocked().windowFocusMap.size();
        // Only the current desktop affects paint; the others are pruned
        // below in separate short holds.
        RecomputeRanksForDesktopLocked(CurrentDeskLocked());
        PruneWindowFocusMapLocked(CurrentDeskLocked());
        const auto& afterApps = CurrentDeskLocked().rankedApps;
        keysAfter.reserve(afterApps.size());
        for (const auto& app : afterApps) {
            keysAfter.push_back(app.key);
        }
        windowsAfter = CurrentDeskLocked().windowFocusMap.size();
    }
    std::vector<GUID> otherDesktops;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        for (const auto& [id, desk] : g_desktopMaps) {
            (void)desk;
            if (!InlineIsEqualGUID(id, g_currentDesktopId)) {
                otherDesktops.push_back(id);
            }
        }
    }
    for (const GUID& id : otherDesktops) {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        auto it = g_desktopMaps.find(id);
        if (it == g_desktopMaps.end() ||
            InlineIsEqualGUID(id, g_currentDesktopId)) {
            continue;
        }
        RecomputeRanksForDesktopLocked(it->second);
        PruneWindowFocusMapLocked(it->second);
        if (it->second.appFocusMap.empty() &&
            it->second.windowAppFocus.empty() &&
            it->second.windowFocusMap.empty()) {
            g_desktopMaps.erase(it);
        }
    }
    // Same length can still be a different ladder ([A,B,C] -> [A,C,D]).
    // An empty list requests a few more sweeps so a missed clear can retry,
    // then stops, so an idle desktop does not run a full pass on every decay
    // tick. Focus thread only.
    static int emptySweepsLeft = 0;
    if (keysBefore != keysAfter) {
        emptySweepsLeft = 2;
    }
    const bool retrySweep = keysAfter.empty() && emptySweepsLeft > 0;
    if (retrySweep && keysBefore == keysAfter) {
        --emptySweepsLeft;
    }
    if (keysBefore != keysAfter || retrySweep) {
        if (keysBefore != keysAfter) {
            Wh_Log(L"Decay recompute: ranks %zu -> %zu", keysBefore.size(),
                   keysAfter.size());
        }
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
    StopDecayTimerIfIdle();
}

// HSHELL_WINDOWDESTROYED: forget the window at once
// instead of on the next decay tick, and repaint so a closed app frees its
// icon slot and preview mark without waiting. App recency itself stays
// (other windows of the app, or re-open within decay). The handle is
// already invalid here, so it is matched by value only.
void HandleWindowDestroyed(HWND hwnd) {
    if (g_unloading.load() || !hwnd) {
        return;
    }
    bool known = false;
    bool previewChanged = false;
    bool droppedPending = false;
    // The window in front closed: its last use is now.
    const bool wasInFront = hwnd == g_inFrontHwnd;
    const ULONGLONG lastUse =
        wasInFront ? LastInputTick64(GetTickCount64()) : 0;
    const ULONGLONG inFrontSince = g_inFrontSince;
    if (wasInFront) {
        g_inFrontHwnd = nullptr;
        g_inFrontSince = 0;
    }
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        for (auto& [id, desk] : g_desktopMaps) {
            (void)id;
            if (desk.windowFocusMap.erase(hwnd)) {
                known = true;
                previewChanged = true;
            }
            if (auto w = desk.windowAppFocus.find(hwnd);
                w != desk.windowAppFocus.end()) {
                // Hand the window's recency to its app before the record
                // goes: the recompute that folds it may not have run since
                // the last stamp. The window is already
                // destroyed, so no liveness check here.
                ULONGLONG tick = w->second.tick;
                // Same rule as RefreshLastUseLocked: confirmed in this
                // stretch in front.
                if (wasInFront && lastUse > tick &&
                    w->second.tick >= inFrontSince) {
                    tick = lastUse;
                }
                if (!w->second.key.empty()) {
                    auto app = desk.appFocusMap.find(w->second.key);
                    if (app != desk.appFocusMap.end() &&
                        tick > app->second.lastConfirmedFocusTick) {
                        app->second.lastConfirmedFocusTick = tick;
                    }
                }
                desk.windowAppFocus.erase(w);
                known = true;
            }
            for (const auto& [key, info] : desk.appFocusMap) {
                (void)key;
                if (info.lastHwnd == hwnd) {
                    known = true;
                }
            }
        }
        if (g_pendingFocus.valid && g_pendingFocus.hwnd == hwnd) {
            g_pendingFocus = {};
            droppedPending = true;
            known = true;
        }
    }
    ForgetWindowAppId(hwnd);
    if (droppedPending) {
        CancelMinFocusTimer();
        CancelPreviewMinFocusTimer();
    }
    if (!known) {
        return;
    }
    // Debounced: the button's own running state settles a moment later.
    RequestApplyVisualsDebounced();
    if (previewChanged) {
        RequestApplyPreviewVisuals();
    }
}
// ---------------------------------------------------------------------------
// WinEvent hook thread
// ---------------------------------------------------------------------------

// Focus thread only. False until the Taskbar.View hooks are in (see
// ActivateFocusTracking); a foreground posted before that is ignored.
bool g_focusTrackingActive = false;
void ActivateFocusTracking(HWND hwnd);

// Notification targets can already be stale when the focus worker receives
// them. Background creation/attention is identity information, not recency.
void HandleForegroundNotification(HWND target) {
    if (g_unloading.load() || !target || !g_focusTrackingActive) {
        return;
    }
    HWND foreground = NormalizeFocusHwnd(GetForegroundWindow());
    if (!foreground || NormalizeFocusHwnd(target) != foreground) {
        return;
    }
    HandleForegroundChanged(foreground);
}

// EVENT_SYSTEM_DESKTOPSWITCH fires on the way to the lock screen / UAC and
// back, and the old "lock / UAC" label read the same both ways. Names the
// input desktop now: "Default" means back on the user desktop; one this
// session may not open (the secure Winlogon desktop) is "not accessible".
std::wstring InputDesktopSwitchReason() {
    HDESK desk = OpenInputDesktop(0, FALSE, DESKTOP_READOBJECTS);
    if (!desk) {
        return L"input desktop: not accessible (lock / UAC)";
    }
    wchar_t name[64] = L"";
    if (!GetUserObjectInformationW(desk, UOI_NAME, name, sizeof(name),
                                   nullptr)) {
        name[0] = L'\0';
    }
    CloseDesktop(desk);
    return std::wstring(L"input desktop: ") + (name[0] ? name : L"?");
}

void HandleShellActivation(WPARAM code, HWND target) {
    if (code == HSHELL_WINDOWACTIVATED || code == HSHELL_RUDEAPPACTIVATED) {
        HandleForegroundNotification(target);
    } else if (code == HSHELL_WINDOWDESTROYED) {
        HandleWindowDestroyed(target);
    }
}

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
        PostToHookThread(WM_APP_DESKTOP_SWITCHED);
        return;
    }
    if (event != EVENT_SYSTEM_FOREGROUND) {
        return;
    }
    if (idObject != OBJID_WINDOW || !hWnd) {
        return;
    }

    PostToHookThread(WM_APP_FOREGROUND_CHANGED, reinterpret_cast<WPARAM>(hWnd));
}

// After a settings change: drop the records of newly excluded apps on every
// desktop. Focus thread (WM_APP_SETTINGS_CHANGED): it owns the maps and is an
// STA, which ExclusionAumid (SHGetPropertyStoreForWindow) needs. Windhawk's
// settings thread gives no COM guarantee; an AUMID read failing there
// would let a newly excluded app keep its highlight.
void DropExcludedRecords() {
    size_t dropped = 0;
    std::unordered_set<HWND, HwndHash> windowHwnds;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        for (auto& [id, desk] : g_desktopMaps) {
            (void)id;
            for (const auto& [hwnd, winfo] : desk.windowFocusMap) {
                (void)winfo;
                if (hwnd) {
                    windowHwnds.insert(hwnd);
                }
            }
            for (const auto& [hwnd, w] : desk.windowAppFocus) {
                (void)w;
                if (hwnd) {
                    windowHwnds.insert(hwnd);
                }
            }
            for (const auto& [key, app] : desk.appFocusMap) {
                (void)key;
                if (app.lastHwnd && app.appIdUpper.empty() &&
                    HwndMatchesStoredPid(app.lastHwnd, app.lastPid)) {
                    windowHwnds.insert(app.lastHwnd);
                }
            }
        }
    }
    std::unordered_map<HWND, std::wstring, HwndHash> windowAppIds;
    for (HWND hwnd : windowHwnds) {
        std::wstring id = ExclusionAumid(hwnd);
        if (!id.empty()) {
            windowAppIds[hwnd] = std::move(id);
        }
    }

    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        for (auto& [id, desk] : g_desktopMaps) {
            (void)id;
            for (auto it = desk.appFocusMap.begin();
                 it != desk.appFocusMap.end();) {
                std::wstring displayUpper = ToUpper(it->second.displayName);
                // No AUMID read at focus time (empty exclude list then):
                // the last window's, read now.
                std::wstring appIdUpper = it->second.appIdUpper;
                if (appIdUpper.empty()) {
                    auto found = windowAppIds.find(it->second.lastHwnd);
                    if (found != windowAppIds.end()) {
                        appIdUpper = found->second;
                    }
                }
                if (IsExcludedKey(it->first, displayUpper, appIdUpper,
                                  it->second.pathUpper)) {
                    it = desk.appFocusMap.erase(it);
                    ++dropped;
                } else {
                    ++it;
                }
            }
            // Window records too, or the next recompute would derive the
            // excluded app again.
            for (auto it = desk.windowAppFocus.begin();
                 it != desk.windowAppFocus.end();) {
                auto& w = it->second;
                if (w.appIdUpper.empty()) {
                    auto found = windowAppIds.find(it->first);
                    if (found != windowAppIds.end()) {
                        w.appIdUpper = found->second;
                    }
                }
                if (IsExcludedKey(w.key, ToUpper(w.displayName), w.appIdUpper,
                                  w.pathUpper)) {
                    it = desk.windowAppFocus.erase(it);
                    ++dropped;
                } else {
                    ++it;
                }
            }
            for (auto it = desk.windowFocusMap.begin();
                 it != desk.windowFocusMap.end();) {
                if (it->second.appIdUpper.empty()) {
                    auto found = windowAppIds.find(it->first);
                    if (found != windowAppIds.end()) {
                        it->second.appIdUpper = found->second;
                    }
                }
                std::wstring fileUpper =
                    ToUpper(FileNameFromPath(it->second.pathUpper));
                if (IsExcludedKey(it->second.processKey, fileUpper,
                                  it->second.appIdUpper,
                                  it->second.pathUpper)) {
                    it = desk.windowFocusMap.erase(it);
                    ++dropped;
                } else {
                    ++it;
                }
            }
            PruneWindowFocusMapLocked(desk);
            RecomputeRanksForDesktopLocked(desk);
        }
    }
    if (dropped) {
        Wh_Log(L"Settings: dropped %zu records of excluded apps", dropped);
        RequestApplyVisuals();
        RequestApplyPreviewVisuals();
    }
}

LRESULT CALLBACK HookThreadWndProc(HWND hWnd,
                                   UINT msg,
                                   WPARAM wParam,
                                   LPARAM lParam) {
    if (g_shellHookMessage && msg == g_shellHookMessage) {
        HandleShellActivation(wParam, reinterpret_cast<HWND>(lParam));
        return 0;
    }
    if (g_focusTrackingActive && !g_unloading.load() &&
        AccentChangeMessage(msg, lParam)) {
        SetTimer(hWnd, kAccentRefreshTimerId, kAccentRefreshDelayMs, nullptr);
        // Broadcasts: fall through to DefWindowProc.
    }
    switch (msg) {
        case WM_CLOSE:
            // This hidden notification window is owned by worker shutdown.
            return 0;
        case WM_APP_FOREGROUND_CHANGED:
            HandleForegroundNotification(reinterpret_cast<HWND>(wParam));
            return 0;
        case WM_APP_DESKTOP_SWITCHED:
            OnVirtualDesktopSwitched(InputDesktopSwitchReason().c_str());
            return 0;
        case WM_APP_SHUTDOWN:
            PostQuitMessage(0);
            return 0;
        case WM_APP_TASKBAR_READY:
            ActivateFocusTracking(hWnd);
            return 0;
        case WM_APP_REQUEST_APPLY_DEBOUNCED:
            if (!g_fullRebindTimerArmed) {
                g_fullRebindTimerArmed = true;
                SetTimer(hWnd, kFullRebindTimerId,
                         static_cast<UINT>(kFullRebindDebounceMs), nullptr);
            }
            return 0;
        case WM_APP_REFRESH_ACCENT:
            RefreshCachedAccent();
            RequestApplyVisuals();
            RequestApplyPreviewVisuals();
            return 0;
        case WM_APP_SETTINGS_CHANGED: {
            DropExcludedRecords();
            bool dropPending = false;
            bool keepPending = false;
            PendingFocus pendingSnap;
            {
                std::lock_guard<std::mutex> lock(g_stateMutex);
                pendingSnap = g_pendingFocus;
            }
            std::wstring pendingAppId = pendingSnap.appIdUpper;
            if (pendingSnap.valid && pendingAppId.empty() && pendingSnap.hwnd) {
                pendingAppId = ExclusionAumid(pendingSnap.hwnd);
            }
            {
                std::lock_guard<std::mutex> lock(g_stateMutex);
                if (g_pendingFocus.valid) {
                    const std::wstring aumid =
                        !g_pendingFocus.appIdUpper.empty()
                            ? g_pendingFocus.appIdUpper
                            : pendingAppId;
                    if (IsExcludedKey(g_pendingFocus.key,
                                      ToUpper(g_pendingFocus.displayName),
                                      aumid, g_pendingFocus.pathUpper)) {
                        g_pendingFocus = {};
                        dropPending = true;
                    } else {
                        keepPending = true;
                    }
                }
            }
            if (dropPending) {
                CancelMinFocusTimer();
                CancelPreviewMinFocusTimer();
            } else if (keepPending) {
                // Color/thickness/intensity must not disarm an allowed
                // candidate. Re-arm the time still left. A deadline that
                // has already elapsed resumes the timer path, including
                // Alt-Tab grace. KillTimer does not flush a queued WM_TIMER.
                EnsurePendingAppTimer();
                EnsurePendingPreviewTimer();
            }
            return 0;
        }
        case WM_TIMER:
            if (wParam == kMinFocusTimerId) {
                KillTimer(hWnd, kMinFocusTimerId);
                OnMinFocusTimerElapsed();
            } else if (wParam == kPreviewMinFocusTimerId) {
                KillTimer(hWnd, kPreviewMinFocusTimerId);
                OnPreviewMinFocusTimerElapsed();
            } else if (wParam == kDecayTimerId) {
                OnDecayTimer();

            } else if (wParam == kAccentRefreshTimerId) {
                KillTimer(hWnd, kAccentRefreshTimerId);
                PostMessage(hWnd, WM_APP_REFRESH_ACCENT, 0, 0);
            } else if (wParam == kFullRebindTimerId) {
                KillTimer(hWnd, kFullRebindTimerId);
                g_fullRebindTimerArmed = false;
                if (g_reseedOnDebouncedPass.exchange(false)) {
                    ++g_buttonSeedGeneration;
                }
                RequestApplyVisuals();
            }
            return 0;
        case WM_DESTROY:
            KillTimer(hWnd, kMinFocusTimerId);
            KillTimer(hWnd, kPreviewMinFocusTimerId);
            KillTimer(hWnd, kDecayTimerId);
            KillTimer(hWnd, kFullRebindTimerId);
            KillTimer(hWnd, kAccentRefreshTimerId);
            g_fullRebindTimerArmed = false;
            return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

// Registry watch for virtual-desktop switches (see g_desktopKeyWatchActive).
// One-shot: re-armed after every signal, before the value is read, so no
// write between the two is missed. Closing the key ends the registration.
struct DesktopKeyWatch {
    HKEY key = nullptr;
    HANDLE event = nullptr;

    bool Arm() {
        return key && event &&
               RegNotifyChangeKeyValue(key, FALSE, REG_NOTIFY_CHANGE_LAST_SET,
                                       event, TRUE) == ERROR_SUCCESS;
    }
    bool Open() {
        if (RegOpenKeyExW(HKEY_CURRENT_USER,
                          L"Software\\Microsoft\\Windows\\CurrentVersion\\"
                          L"Explorer\\VirtualDesktops",
                          0, KEY_NOTIFY, &key) != ERROR_SUCCESS) {
            key = nullptr;
            return false;
        }
        event = CreateEventW(nullptr, FALSE, FALSE, nullptr);
        return Arm();
    }
    void Close() {
        if (key) {
            RegCloseKey(key);
            key = nullptr;
        }
        if (event) {
            CloseHandle(event);
            event = nullptr;
        }
    }
};

// ---------------------------------------------------------------------------
// Focus tracking starts with the taskbar. Windhawk loads the mod into
// every explorer.exe, also folder-window and COM-server processes that never
// load Taskbar.View.dll. There the worker keeps only its hidden window and
// waits: no shell hook, no WinEvents, no accent refresh, no registry
// watch, and no focus logs (each such process would log the taskbar
// process's focus events again).
// Started by WM_APP_TASKBAR_READY once the Taskbar.View hooks are in, or at
// worker start when they already are. Focus thread only.
// ---------------------------------------------------------------------------
struct FocusTracking {
    bool shellHookRegistered = false;
    HWINEVENTHOOK foregroundHook = nullptr;
    HWINEVENTHOOK desktopHook = nullptr;
    DesktopKeyWatch desktopWatch;
};
FocusTracking g_focusTracking;

void ActivateFocusTracking(HWND hwnd) {
    if (g_focusTrackingActive || g_unloading.load()) {
        return;
    }
    g_focusTrackingActive = true;
    FocusTracking& t = g_focusTracking;

    // Shell notifications require a desktop window. Never show or activate it.
    g_shellHookMessage = RegisterWindowMessageW(L"SHELLHOOK");
    const bool shellHookRegistered =
        g_shellHookMessage && RegisterShellHookWindow(hwnd);
    if (!shellHookRegistered) {
        Wh_Log(L"Shell activation registration failed: %u; using foreground WinEvents only",
               GetLastError());
        g_shellHookMessage = 0;
    } else {
        Wh_Log(L"Shell activation notifications registered (no foreground polling)");
    }
    // Foreground WinEvents
    t.shellHookRegistered = shellHookRegistered;

    // Accent changes: AccentChangeMessage in HookThreadWndProc.
    RefreshCachedAccent();

    t.foregroundHook =
        SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
                        nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!t.foregroundHook) {
        // Init is long over: log it, do not fail.
        if (shellHookRegistered) {
            Wh_Log(L"ERROR: SetWinEventHook failed: %u; focus from shell "
                   L"activations only",
                   GetLastError());
        } else {
            Wh_Log(L"ERROR: SetWinEventHook failed: %u; no focus "
                   L"notifications, nothing will be ranked",
                   GetLastError());
        }
    } else {
        Wh_Log(L"EVENT_SYSTEM_FOREGROUND hook installed");
    }

    t.desktopHook =
        SetWinEventHook(EVENT_SYSTEM_DESKTOPSWITCH, EVENT_SYSTEM_DESKTOPSWITCH,
                        nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!t.desktopHook) {
        Wh_Log(L"EVENT_SYSTEM_DESKTOPSWITCH hook failed: %u", GetLastError());
    } else {
        Wh_Log(L"EVENT_SYSTEM_DESKTOPSWITCH hook installed");
    }

    g_desktopKeyWatchActive = t.desktopWatch.Open();
    if (g_desktopKeyWatchActive) {
        Wh_Log(L"Virtual desktop registry watch installed");
    } else {
        Wh_Log(L"Virtual desktop registry watch failed: %u; reading the "
               L"desktop on each foreground change",
               GetLastError());
        t.desktopWatch.Close();
    }

    RefreshCurrentDesktopId();
    Wh_Log(L"Focus tracking started");

    if (HWND fg = GetForegroundWindow()) {
        PostMessage(hwnd, WM_APP_FOREGROUND_CHANGED,
                    reinterpret_cast<WPARAM>(fg), 0);
    }
}

// Worker exit. Safe when tracking never started.
void DeactivateFocusTracking(HWND hwnd) {
    FocusTracking& t = g_focusTracking;
    t.desktopWatch.Close();
    g_desktopKeyWatchActive = false;
    if (t.foregroundHook) {
        UnhookWinEvent(t.foregroundHook);
        t.foregroundHook = nullptr;
    }
    if (t.desktopHook) {
        UnhookWinEvent(t.desktopHook);
        t.desktopHook = nullptr;
    }
    if (t.shellHookRegistered) {
        DeregisterShellHookWindow(hwnd);
        t.shellHookRegistered = false;
    }
    g_shellHookMessage = 0;
    g_focusTrackingActive = false;
}

DWORD WINAPI WinEventHookThread(LPVOID /*param*/) {
    g_hookThreadId.store(GetCurrentThreadId(), std::memory_order_release);
    g_decayTimerArmed.store(false);
    g_fullRebindTimerArmed = false;
    g_shellHookMessage = 0;

    HMODULE hMod = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            reinterpret_cast<LPCWSTR>(&HookThreadWndProc),
                            &hMod) ||
        !hMod) {
        Wh_Log(L"GetModuleHandleExW failed for message-window class: %u",
               GetLastError());
        g_hookThreadId.store(0, std::memory_order_release);
        SignalHookThreadReady();
        return 1;
    }

    WNDCLASSEXW wc{
        .cbSize = sizeof(WNDCLASSEXW),
        .lpfnWndProc = HookThreadWndProc,
        .hInstance = hMod,
        .lpszClassName = L"Windhawk_TaskbarRecentFocusHighlight_MsgWnd",
    };
    ATOM atom = RegisterClassExW(&wc);
    if (!atom) {
        Wh_Log(L"RegisterClassExW failed: %u", GetLastError());
        g_hookThreadId.store(0, std::memory_order_release);
        SignalHookThreadReady();
        return 1;
    }

    HWND hwnd =
        CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, wc.lpszClassName,
                        L"", WS_POPUP, 0, 0, 0, 0, nullptr,
                        nullptr, wc.hInstance, nullptr);
    if (!hwnd) {
        Wh_Log(L"Failed to create message window: %u", GetLastError());
        UnregisterClassW(wc.lpszClassName, wc.hInstance);
        g_hookThreadId.store(0, std::memory_order_release);
        SignalHookThreadReady();
        return 1;
    }
    g_hookThreadHwnd.store(hwnd, std::memory_order_release);

    HRESULT coHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(coHr) && coHr != RPC_E_CHANGED_MODE) {
        Wh_Log(L"Focus thread CoInitializeEx failed %08X", coHr);
    }
    SignalHookThreadReady();

    // The window is published above, so a WM_APP_TASKBAR_READY posted from
    // now on arrives; the flag covers hooks installed before this point.
    if (g_taskbarViewHooked.load()) {
        ActivateFocusTracking(hwnd);
    } else {
        Wh_Log(L"Focus tracking waits for the taskbar (Taskbar.View.dll not "
               L"hooked in this process)");
    }

    // Messages (WinEvents, shell hook, timers, posted work) plus the
    // desktop key event.
    DesktopKeyWatch& desktopWatch = g_focusTracking.desktopWatch;
    bool quit = false;
    while (!quit) {
        const DWORD handles = desktopWatch.event ? 1 : 0;
        const DWORD r = MsgWaitForMultipleObjectsEx(
            handles, handles ? &desktopWatch.event : nullptr, INFINITE,
            QS_ALLINPUT, MWMO_INPUTAVAILABLE);
        if (handles && r == WAIT_OBJECT_0) {
            if (!desktopWatch.Arm()) {
                Wh_Log(L"Virtual desktop registry watch lost; reading the "
                       L"desktop on each foreground change");
                desktopWatch.Close();
                g_desktopKeyWatchActive = false;
            }
            if (!g_unloading.load() && RefreshCurrentDesktopId()) {
                OnVirtualDesktopSwitched(L"registry");
            }
            // Fall through to the messages: a burst of registry writes must
            // not hold WM_APP_SHUTDOWN in the queue.
        } else if (r != WAIT_OBJECT_0 + handles) {
            Wh_Log(L"Focus thread wait failed: %u", GetLastError());
            break;
        }
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT ||
                (msg.message == WM_APP && msg.hwnd == nullptr)) {
                quit = true;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
    ReleaseVdm();
    g_decayTimerArmed.store(false);
    g_hookThreadHwnd.store(nullptr, std::memory_order_release);
    DeactivateFocusTracking(hwnd);
    DestroyWindow(hwnd);
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    g_hookThreadId.store(0, std::memory_order_release);
    if (SUCCEEDED(coHr)) {
        CoUninitialize();
    }

    Wh_Log(L"WinEvent hook thread exiting");
    return 0;
}

bool StartWinEventHookThread() {
    std::lock_guard<std::mutex> lock(g_winEventHookThreadMutex);
    if (g_winEventHookThread && HookThreadWindow()) {
        return true;
    }
    if (!g_hookThreadReadyEvent) {
        g_hookThreadReadyEvent =
            CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!g_hookThreadReadyEvent) {
            const DWORD error = GetLastError();
            Wh_Log(L"CreateEventW failed for hook-thread ready: %u",
                   error);
            return false;
        }
    } else {
        ResetEvent(g_hookThreadReadyEvent);
    }
    HANDLE hThread =
        CreateThread(nullptr, 0, WinEventHookThread, nullptr, 0, nullptr);
    if (!hThread) {
        const DWORD error = GetLastError();
        Wh_Log(L"CreateThread failed: %u", error);
        if (g_hookThreadReadyEvent) {
            CloseHandle(g_hookThreadReadyEvent);
            g_hookThreadReadyEvent = nullptr;
        }
        return false;
    }
    g_winEventHookThread = hThread;
    WaitForSingleObject(g_hookThreadReadyEvent, INFINITE);
    if (!HookThreadWindow()) {
        Wh_Log(L"Focus thread failed to create the message window");
        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);
        g_winEventHookThread = nullptr;
        if (g_hookThreadReadyEvent) {
            CloseHandle(g_hookThreadReadyEvent);
            g_hookThreadReadyEvent = nullptr;
        }
        return false;
    }
    Wh_Log(L"WinEvent hook thread started");
    return true;
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

    HWND hwnd = HookThreadWindow();
    if (hwnd) {
        PostMessage(hwnd, WM_APP_SHUTDOWN, 0, 0);
    }
    DWORD threadId = GetThreadId(hThread);
    if (threadId) {
        PostThreadMessage(threadId, WM_APP, 0, 0);
    }
    const DWORD w = WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    if (g_hookThreadReadyEvent) {
        CloseHandle(g_hookThreadReadyEvent);
        g_hookThreadReadyEvent = nullptr;
    }
    if (w != WAIT_OBJECT_0) {
        Wh_Log(L"ERROR: focus thread wait failed (%u)", w);
    } else {
        Wh_Log(L"WinEvent hook thread stopped");
    }
}

// ---------------------------------------------------------------------------
// Settings load
// ---------------------------------------------------------------------------

// Settings transition from 0.11.1 (remove in 1.0). 0.11.1 had one preview
// style for all ranks, `previews.style` (plateTitle = default hybrid,
// plate, titleBg, titleBar), now `previews.topStyle` +
// `previews.restStyle`. Windhawk keeps the old
// stored value and has no migration mechanism, so
// a user who had picked a non-default style would silently get the new
// defaults. When both new keys are unset or still at their defaults, use the
// old choice for both ranks until the user first saves settings: from then on
// the settings page is what they see. A mod cannot write its own settings, so
// the state lives in mod storage: 0 = not checked yet, 1 = legacy style in
// use, 2 = done. Other values and the old default map to nothing.
constexpr wchar_t kLegacyPreviewStyleState[] = L"legacyPreviewStyleState";

bool LegacyPreviewStyle(PreviewStyle& out) {
    // Old ids: plateTitle (= the new defaults), plate, titleBg, titleBar.
    auto legacy = WindhawkUtils::StringSetting::make(L"previews.style");
    PreviewStyle v = PreviewStyle::None;
    if (!NameToEnum(kPreviewStyleNames, legacy.get(), v) ||
        v == PreviewStyle::Outline || v == PreviewStyle::TitleOutline ||
        v == PreviewStyle::None) {
        return false;
    }
    out = v;
    return true;
}

void ApplyLegacyPreviewStyle(Settings& s, bool userSavedSettings) {
    int state = Wh_GetIntValue(kLegacyPreviewStyleState, 0);
    if (state == 2) {
        return;
    }
    if (userSavedSettings) {
        if (state == 1) {
            Wh_Log(L"Settings: legacy previews.style no longer applied "
                   L"(settings saved)");
        }
        Wh_SetIntValue(kLegacyPreviewStyleState, 2);
        return;
    }
    PreviewStyle legacy = PreviewStyle::Plate;
    const bool haveLegacy = LegacyPreviewStyle(legacy);
    if (state == 0) {
        auto top = WindhawkUtils::StringSetting::make(L"previews.topStyle");
        auto rest = WindhawkUtils::StringSetting::make(L"previews.restStyle");
        const bool newAtDefaults =
            (!*top.get() || wcscmp(top.get(), L"plate") == 0) &&
            (!*rest.get() || wcscmp(rest.get(), L"titleBg") == 0);
        state = (haveLegacy && newAtDefaults) ? 1 : 2;
        Wh_SetIntValue(kLegacyPreviewStyleState, state);
    }
    if (state == 1 && haveLegacy) {
        s.previewTopStyle = legacy;
        s.previewRestStyle = legacy;
        Wh_Log(L"Settings: using legacy previews.style=%s for all ranks until "
               L"settings are saved",
               PreviewStyleName(legacy));
    }
}

int ClampedIntSetting(PCWSTR name, int lo, int hi) {
    return (std::min)(hi, (std::max)(lo, Wh_GetIntSetting(L"%s", name)));
}

// Unknown or removed ids (e.g. the old bottomBar style) keep the fallback.
template <typename E, size_t N>
E EnumSetting(PCWSTR name, const EnumName<E> (&table)[N], E fallback) {
    auto value = WindhawkUtils::StringSetting::make(L"%s", name);
    NameToEnum(table, value.get(), fallback);
    return fallback;
}

void LoadSettings(bool userSavedSettings) {
    Settings s;
    constexpr int kNoMax = 1 << 30;

    s.highlightCount = ClampedIntSetting(L"highlightCount", 0, 16);
    s.minFocusSeconds = ClampedIntSetting(L"minFocusSeconds", 0, kNoMax);
    s.promoteMode = EnumSetting(L"promoteMode", kPromoteModeNames,
                                PromoteMode::ImmediateTracked);
    s.decayMinutes = ClampedIntSetting(L"decayMinutes", 0, kNoMax);

    s.glowStyle =
        EnumSetting(L"icons.glowStyle", kGlowStyleNames, GlowStyle::LeftBar);
    s.glowColor =
        EnumSetting(L"icons.glowColor", kGlowColorNames, GlowColorMode::Accent);
    auto customColor =
        WindhawkUtils::StringSetting::make(L"icons.customGlowColor");
    s.customGlowColor = *customColor.get() ? customColor.get() : L"#00C853";
    {
        winrt::Windows::UI::Color parsed{255, 0, 200, 83};
        if (!ParseHexColor(s.customGlowColor, parsed)) {
            parsed = {255, 0, 200, 83};
        }
        s.customGlowPacked = PackColor(parsed);
    }
    s.glowIntensity[0] = ClampedIntSetting(L"icons.glowIntensityRank1", 0, 100);
    s.glowIntensity[1] = ClampedIntSetting(L"icons.glowIntensityRank2", 0, 100);
    s.glowIntensity[2] = ClampedIntSetting(L"icons.glowIntensityRank3", 0, 100);
    s.glowThickness = ClampedIntSetting(L"icons.glowThickness", 1, 16);
    s.glowSize = ClampedIntSetting(L"icons.glowSize", 40, 100);
    s.glowFillOpacity = ClampedIntSetting(L"icons.glowFillOpacity", 0, 100);
    s.glowAccentThickness =
        ClampedIntSetting(L"icons.glowAccentThickness", 1, 8);

    s.previewHighlightEnabled =
        Wh_GetIntSetting(L"previews.highlightEnabled") != 0;
    s.previewHighlightCount = ClampedIntSetting(L"previews.highlightCount", 0, 16);
    s.previewSingleWindow = Wh_GetIntSetting(L"previews.singleWindow") != 0;
    s.previewIntensity[0] = ClampedIntSetting(L"previews.intensityRank1", 0, 100);
    s.previewIntensity[1] = ClampedIntSetting(L"previews.intensityRank2", 0, 100);
    s.previewIntensity[2] = ClampedIntSetting(L"previews.intensityRank3", 0, 100);
    s.previewFillOpacity = ClampedIntSetting(L"previews.fillOpacity", 0, 100);
    s.previewMinFocusSeconds =
        ClampedIntSetting(L"previews.minFocusSeconds", 0, kNoMax);
    s.previewDecayMinutes = ClampedIntSetting(L"previews.decayMinutes", 0, kNoMax);
    s.previewTopStyle =
        EnumSetting(L"previews.topStyle", kPreviewStyleNames, PreviewStyle::Plate);
    if (s.previewTopStyle == PreviewStyle::None) {  // not offered for rank 1
        s.previewTopStyle = PreviewStyle::Plate;
    }
    s.previewRestStyle = EnumSetting(L"previews.restStyle", kPreviewStyleNames,
                                     PreviewStyle::TitleBg);
    ApplyLegacyPreviewStyle(s, userSavedSettings);
    s.previewOutlineThickness =
        ClampedIntSetting(L"previews.outlineThickness", 1, 8);

    for (int i = 0;; i++) {
        auto program =
            WindhawkUtils::StringSetting::make(L"excludedPrograms[%d]", i);
        if (!*program.get()) {
            break;
        }
        s.excludedPrograms.insert(ToUpper(program.get()));
    }

    Wh_Log(L"Settings: count=%d style=%s color=%s th=%d size=%d%% fillOp=%d "
           L"previewFillOp=%d decay=%dmin minFocus=%ds promote=%s preview=%d "
           L"previewCount=%d single=%d previewI=%d/%d/%d previewStyle=%s/%s "
           L"previewMin=%ds previewDecay=%dmin excluded=%zu",
           s.highlightCount, GlowStyleName(s.glowStyle),
           EnumToName(kGlowColorNames, s.glowColor),
           s.glowThickness, s.glowSize, s.glowFillOpacity, s.previewFillOpacity,
           s.decayMinutes, s.minFocusSeconds, PromoteModeName(s.promoteMode),
           s.previewHighlightEnabled ? 1 : 0, s.previewHighlightCount,
           s.previewSingleWindow ? 1 : 0, s.previewIntensity[0],
           s.previewIntensity[1], s.previewIntensity[2],
           PreviewStyleName(s.previewTopStyle),
           PreviewStyleName(s.previewRestStyle), s.previewMinFocusSeconds,
           s.previewDecayMinutes, s.excludedPrograms.size());

    PublishSettings(std::move(s));
    PostToHookThread(WM_APP_REFRESH_ACCENT);
}

// ---------------------------------------------------------------------------
// Windhawk entry points
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"> init " WH_MOD_VERSION);

    g_unloading = false;
    LoadSettings(/*userSavedSettings=*/false);
    // Nothing to paint (highlightCount=0, previews off) is not a failed init:
    // Windhawk shows nothing for a failed init and then does not call
    // Wh_ModSettingsChanged, so turning a highlight back on would do nothing
    // until the mod was toggled.
    {
        auto settings = SettingsSnap();
        if (settings->highlightCount <= 0 &&
            !settings->previewHighlightEnabled) {
            Wh_Log(L"Nothing to paint yet (highlightCount=0, previews off)");
        }
    }

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelBaseModule) {
        Wh_Log(L"kernelbase.dll not loaded");
        return FALSE;
    }
    auto pKernelBaseLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(
        kernelBaseModule, "LoadLibraryExW");
    if (!pKernelBaseLoadLibraryExW) {
        Wh_Log(L"LoadLibraryExW not found in kernelbase.dll");
        return FALSE;
    }
    WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                   LoadLibraryExW_Hook,
                                   &LoadLibraryExW_Original);

    // No taskbar.dll hooks: button identity, windows, previews
    // and focus all come from Taskbar.View.dll and public WindowsUdk
    // interfaces.
    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
            Wh_Log(L"Taskbar.View hooks failed");
            return FALSE;
        }
        g_taskbarViewHookAttempted = true;
        g_taskbarViewHooked = true;  // the focus thread starts tracking
    } else {
        Wh_Log(L"Taskbar view module not loaded yet");
    }

    if (!StartWinEventHookThread()) {
        Wh_Log(L"Focus thread failed to start");
        return FALSE;
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        HandleLoadedModuleIfTaskbarView(taskbarViewModule, L"Taskbar view (after init)");
    }
    // The current foreground is posted by ActivateFocusTracking, once this
    // process is known to host the taskbar.
}

// Heap-owning thread_locals of a UI thread. Their destructors would run only
// at that thread's exit, long after the module is unloaded, so each disable
// or update leaked them. Runs in the unload cleanup on each UI thread.
void ReleaseUiThreadLocals_UIThread() {
    std::unordered_set<void*>().swap(t_seededRepeaters);
    std::wstring().swap(t_lastLadderLog);
    std::wstring().swap(t_lastResolveLog);
    std::wstring().swap(t_lastFlyoutKey);
    std::wstring().swap(g_imagePathCache);
}

void Wh_ModUninit() {
    Wh_Log(L">");
    g_unloading = true;
    g_taskbandResolveReady = false;

    // Stop the worker first so it cannot TryRunAsync after the UI drain.
    StopWinEventHookThread();

    if (!RunOnEachUiDispatcherAndWait([]() {
            ClearAllHighlights_UIThread();
            ClearAllThumbnailHighlights_UIThread();
            RevokeIconPanelLayoutWatchesOnThisDispatcher();
            RevokeRepeaterWatchesOnThisDispatcher();
            ReleaseUiThreadLocals_UIThread();
        })) {
        Wh_Log(L"ERROR: UI cleanup did not finish on every dispatcher");
    }
    size_t leftLayoutWatches = 0;
    size_t leftRepeaterWatches = 0;
    {
        std::lock_guard<std::mutex> lock(g_layoutWatchMutex);
        leftLayoutWatches = g_layoutWatches.size();
    }
    {
        std::lock_guard<std::mutex> lock(g_repeaterWatchMutex);
        // Entries left here belong to repeaters still alive on a dispatcher
        // the drain did not reach.
        leftRepeaterWatches = g_repeaterWatches.size();
    }
    if (leftLayoutWatches) {
        Wh_Log(L"ERROR: %zu IconPanel SizeChanged watches not revoked",
               leftLayoutWatches);
    }
    if (leftRepeaterWatches) {
        Wh_Log(L"ERROR: %zu flyout ElementClearing watches not revoked",
               leftRepeaterWatches);
    }

    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        g_desktopMaps.clear();
        g_currentDesktopId = {};
        g_haveCurrentDesktop = false;
        g_pendingFocus = {};
    }
    // The worker has exited (and released the VDM); its last-use stretch
    // goes with it.
    g_inFrontHwnd = nullptr;
    g_inFrontSince = 0;
    {
        std::lock_guard<std::mutex> lock(g_windowAppIdMutex);
        g_windowAppId.clear();
    }
    {
        std::lock_guard<std::mutex> lock(g_uncombinedMutex);
        g_uncombinedWindowsByThread.clear();
    }
    {
        std::lock_guard<std::mutex> lock(g_fullPassQueuedMutex);
        g_fullPassQueuedThreads.clear();
    }
    {
        std::lock_guard<std::mutex> lock(g_buttonsMutex);
        g_trackedButtons.clear();
    }
    {
        std::lock_guard<std::mutex> lock(g_buttonStateMutex);
        g_buttonStates.clear();
    }
    {
        std::lock_guard<std::mutex> lock(g_thumbViewsMutex);
        g_trackedThumbViews.clear();
    }
    {
        std::lock_guard<std::mutex> lock(g_dispatchersMutex);
        g_uiDispatchers.reset();
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings(/*userSavedSettings=*/true);

    // Newly excluded apps are dropped on the focus worker
    // (DropExcludedRecords): it owns the recency maps and is the STA
    // ExclusionAumid's SHGetPropertyStoreForWindow needs.
    PostToHookThread(WM_APP_SETTINGS_CHANGED);
    RequestApplyVisuals();
    RequestApplyPreviewVisuals();
}
