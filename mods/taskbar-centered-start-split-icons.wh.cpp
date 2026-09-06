// ==WindhawkMod==
// @id              taskbar-centered-start-split-icons
// @name            Taskbar Start Button Centered Origin
// @description     Pins the Start button to true screen-center and splits running-app taskbar buttons into two groups flanking it by which side of the screen each window is on (Windows 11 only; incompatible with "Start button always on the left")
// @version         1.0.0
// @author          rick
// @github          https://github.com/rycalvo
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject
// @license         GPL-3.0
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.

// ==WindhawkModReadme==
/*
# Taskbar Start Button Centered Origin

Pins the Windows/Start button to the exact horizontal center of the primary
monitor, regardless of how many taskbar icons are present. Running-app
taskbar buttons are split into two groups that flank the Start button:

- A button goes to the **left** group if its window's current on-screen
  position is left of screen-center.
- A button goes to the **right** group if its window is right of
  screen-center.
- Pinned-but-not-running apps have no window to check the position of, so
  they're classified by the `leftApps`/`rightApps` name lists below, falling
  back to `unresolvedAppsDefaultSide` if unlisted. Within whichever side
  they land on, `pinnedAppsAnchor` picks whether they sit at the far edge
  or right next to Start.

![Window left of screen-center](https://raw.githubusercontent.com/rycalvo/w11tb_centerer/main/screenshots/leftside.png) \
_The crosshair-marked window sits left of screen-center - its taskbar icon
follows, landing left of Start._

![Window right of screen-center](https://raw.githubusercontent.com/rycalvo/w11tb_centerer/main/screenshots/rightdefault.png) \
_The same window moved right of screen-center - its icon moves with it,
to the right of Start._

When you drag a window across the center line of the screen, its taskbar
button switches sides to follow it. Side-switching is driven by a periodic
poll of tracked windows' positions and is best-effort: it happens shortly
after a drag/move settles, not on every intermediate pixel of the drag.

Search, Task View and Widgets can either stay at the far left edge, or move
right next to Start on whichever side you prefer.

The window-tracking behind position-based splitting and drag-follow can be
turned off entirely with `trackWindowPositions`, if you'd rather keep just
the centered Start button and system-button placement with no background
probing of taskbar buttons and no periodic position polling - every app is
then classified the same way as a pinned-but-not-running one instead.

## Known limitations (please read before reporting issues)

- **Windows 11 only.** Windows 10's taskbar has no XAML layer to hook into.
- **Primary monitor only**, for both the Start button's position and
  window-side classification - and on a multi-monitor setup this is a
  bigger limitation than it might sound: a window's side is decided by
  comparing its position against the *primary* monitor's center line, so
  every window on a monitor entirely to one side of the primary reads as a
  fixed left/right regardless of where on that monitor it actually sits -
  only movement within the primary monitor (or across its center line) is
  tracked per-pixel. Taskbars on secondary displays aren't specially
  handled either (their icons keep Windows' default layout) - the mod's
  positioning plan is only ever computed on the primary taskbar's own
  thread, so secondary-monitor elements are simply never included in it
  and fall through to Windows' native positioning.
- **Don't enable alongside "Start button always on the left".** Both mods
  hook the same process-wide `IUIElement::Arrange` vtable slot, and both
  write their own X position for the Start button - with both enabled,
  they simply disagree, and whichever one's hook runs last for a given
  Arrange call wins for that pass. This mod's `systemButtonsPlacement:
  far-left` setting covers the same "keep everything else out of Start's
  way" goal that mod's `otherSystemButtonsOnTheLeft` option does, so
  there's no reason to run both together. This mod is intentionally scoped
  to true screen-center plus position-based icon splitting specifically -
  not a general Start-position picker - so this conflict is a documented
  incompatibility to avoid rather than something planned to be resolved by
  merging the two mods or their positioning logic.
- **The Start menu itself doesn't follow the repositioned Start button.**
  This mod only moves the Start *button* - nothing decides where the Start
  *menu* opens. With the taskbar's own "Taskbar alignment" set to
  "Center", the menu happens to open near screen-center anyway since
  that's where Windows expects it by default, but with alignment set to
  "Left", the button sits at true center while the menu still opens at the
  left edge. This is a solvable limitation rather than a permanent one -
  `taskbar-start-button-position.wh.cpp` repositions the menu itself by
  hooking `StartMenuExperienceHost.exe` - just not something this mod
  does today.
- **A very crowded side compresses instead of overflowing cleanly.** If
  enough app icons pile up on one side that they'd run into the system
  tray/clock on the right (or, in "far left" placement, into Search/Task
  View/Widgets on the left), icon spacing on that side is compressed just
  enough to keep the whole group within bounds - icons still render at
  full size, so once genuinely crowded they visually overlap each other
  rather than being hidden or scrolled the way the taskbar's own overflow
  handling would.
- **Windows' own "Combine taskbar buttons and hide labels: When taskbar
  is full" doesn't shrink icons under this mod** (icons stay full-size and
  compress/overlap instead, per the point above) - "Always" isn't
  affected and works normally. Likely because the "when full" decision is
  made by native layout logic (this mod only overrides each button's
  final X position afterward, it never touches sizing), evaluated against
  the taskbar's native, unsplit layout rather than this mod's split one -
  unconfirmed, not yet investigated further.
- **A grouped button (multiple windows combined under one icon) follows
  only its first window.** With "Combine taskbar buttons" set to "Always",
  a group's side and ordering are both decided by whichever of its windows
  happens to be first in the taskbar's own internal list - not necessarily
  the one you'd expect - so a group whose windows straddle the center line
  won't visually reflect all of them. There's no exposed way to pick a
  more meaningful "primary" window for a group, so this is a documented
  tradeoff rather than a bug.
- **The taskbar's own overflow button, when it appears on a crowded
  taskbar, keeps its native position** rather than being classified and
  placed like the buttons around it - this mod doesn't give it a slot in
  the split layout.
- **Undocumented internals.** This mod hooks private, unversioned classes
  inside `taskbar.dll` and `Taskbar.View.dll` (via symbols resolved from
  Microsoft's public symbol server at runtime, not hardcoded offsets). A
  Windows update can change these internals and break the mod until it's
  updated. If that happens, disable the mod rather than filing against
  explorer.exe crashing.
- The "resolve which HWND a taskbar button represents" step reuses a
  technique from other taskbar-reordering mods (synchronously reporting a
  sentinel "click" to the taskbar's internal click handler, which is
  intercepted before it does anything, to read back the window handle) -
  but unlike those mods, which only ever dispatch it from an actual user
  gesture on one specific button, this mod dispatches it unattended, on
  its own background timer, against every button it hasn't resolved yet.
  It runs on a periodic timer rather than inline during layout, and
  Arrange only ever reads whatever the timer has already cached - running
  it synchronously from inside the taskbar's own layout pass was the
  confirmed cause of an explorer.exe crash (specifically when Windows'
  "show taskbar apps on" setting is anything other than "All taskbars",
  since that's when a window moving across monitors structurally adds/
  removes taskbar buttons rather than just repositioning them). As a
  further safeguard, the very first such probe of a session is held back
  until a real (non-sentinel) click has been seen passing through the same
  interception point, and three misses on a path that has never once
  succeeded latch this mod's probing off entirely on that path (see
  `Wh_Log` for which) - so a running app's icon may briefly show on its
  default side, rather than by window position, until you click any
  taskbar button once. That gate
  only covers *before* interception is ever confirmed working, though: if
  a future Windows update breaks interception in a way that still lets a
  probe reach the taskbar's real click handler afterward, the probe's
  sentinel value gets delivered as that handler's real argument, and the
  realistic worst case is a stray window activate/minimize or an access
  violation in explorer.exe, not merely icons freezing on their default
  side.
- **Taskbar buttons can disappear when a display is deactivated** (via
  Settings, unplugging, or a third-party display on/off tool) - if
  "Taskbar behaviors > When using multiple displays, show my taskbar apps
  on" isn't set to "All taskbars", Windows itself can drop a window's
  taskbar button entirely until you switch to that window (e.g. via
  Alt+Tab), rather than migrating it to the remaining taskbar. Reactivating
  a display doesn't trigger this. This is native Explorer/taskbar.dll
  behavior upstream of anything the mod hooks into, not something this mod
  causes or can work around.

## Disclosures

I am not a software developer. The present mod was developed using the
Claude Code extension in VS Code. This mod was created for my own
interests and shared for targeted development by members of the Windhawk
community.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- gapPx: 12
  $name: Gap size (px)
  $description: >-
    Horizontal gap, in pixels, between the Start button and each flanking
    group of app icons (and between Start and Search/Task View/Widgets, if
    those are set to sit adjacent to Start below).
- systemButtonsPlacement: far-left
  $name: Search / Task View / Widgets placement
  $options:
    - far-left: Far left edge of the taskbar
    - adjacent-start: Right next to the Start button
  $description: >-
    Where the Search, Task View and Widgets buttons go. "Far left edge" keeps
    them out of the way of the centered layout entirely. "Right next to
    Start" tucks them against one side of the Start button instead (pick
    which side below).
- systemButtonsAdjacentSide: left
  $name: Side to place them on
  $options:
    - left: Left of Start
    - right: Right of Start
  $description: >-
    Only used when the placement above is "Right next to Start".
- leftApps: ""
  $name: Force these apps to the left
  $description: >-
    Comma-separated list of app name fragments (matched case-insensitively
    against each taskbar button's accessible name, e.g. "chrome, notepad")
    that should always be placed left of Start, regardless of where their
    window is. Mainly useful for pinned apps that aren't running, since
    those have no window position to classify by.
- rightApps: ""
  $name: Force these apps to the right
  $description: Same idea as leftApps, but for the right side.
- unresolvedAppsDefaultSide: contralateral-to-system-buttons
  $name: Default side for unclassified apps
  $options:
    - left: Left of Start
    - right: Right of Start
    - contralateral-to-system-buttons: Opposite side from Search/Task View/Widgets
  $description: >-
    Fallback for any taskbar button that isn't matched by leftApps/rightApps
    and whose window position can't be determined (mainly pinned-but-not-
    running apps you haven't listed above). "Opposite side from Search/Task
    View/Widgets" tracks the systemButtonsAdjacentSide setting above when
    those are placed next to Start; if they're instead left at the
    taskbar's far-left edge, that still counts as their "side" for this
    purpose, so it resolves to the right.
- taskListOrder: distance-from-center
  $name: App icon ordering within each side
  $options:
    - distance-from-center: Closer-to-center windows sit closer to Start
    - taskbar-order: Preserve the existing taskbar order
  $description: >-
    How same-side app icons are ordered relative to each other. "Closer to
    center" ranks each window by how close its own center is to the
    screen's center line, so the nearest one ends up right next to Start.
    Pinned/overridden apps with no live window position sort to whichever
    end pinnedAppsAnchor below picks. Minimized windows keep the position
    they last had before minimizing, rather than jumping around.
- pinnedAppsAnchor: outer-edge
  $name: Pinned (not running) app position within their side
  $options:
    - outer-edge: Far end, away from Start
    - adjacent-to-start: Right next to Start
  $description: >-
    Where a pinned-but-not-running app's icon sits relative to the running
    apps on the same side (leftApps/rightApps-forced apps included, since
    those also have no window to rank by distance). Only has an effect
    when "App icon ordering" above is "Closer to center" - with "Preserve
    existing taskbar order" there's no outer-edge/adjacent-to-Start
    distinction to begin with.
- trackWindowPositions: true
  $name: Track window positions
  $description: >-
    When on, each running app's taskbar button is matched to its window so
    it can be classified by live position and follow it if it's dragged
    across the screen - this involves probing the taskbar's internal click
    handler on a background timer and periodically polling tracked
    windows' positions. Turn off to disable all of that: every running app
    is then classified the same way as a pinned-but-not-running one, via
    leftApps/rightApps/
    "Default side for unclassified apps" above, with no drag-follow. Start
    centering and Search/Task View/Widgets placement are unaffected either
    way. Takes effect immediately, no need to reload the mod.
*/
// ==/WindhawkModSettings==

// Extended design rationale and round-by-round change history, for anyone
// who wants more background than the comments below carry on their own:
// https://github.com/rycalvo/w11tb_centerer/blob/main/RATIONALE.md

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <functional>
#include <iterator>
#include <limits>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <commctrl.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

#define WH_WINRT_WINUI2
#include <winrt/Microsoft.UI.Xaml.Controls.h>

using namespace winrt::Windows::UI::Xaml;

// ============================================================================
// Settings
// ============================================================================

enum class Side { Left, Right };

enum class SystemButtonsPlacement { FarLeft, AdjacentStart };

enum class TaskListOrder { DistanceFromCenter, TaskbarOrder };

enum class UnresolvedAppsDefaultSide {
    Left,
    Right,
    ContralateralToSystemButtons,
};

// Where a pinned-but-not-running app's icon sits within its side. Only
// meaningful when taskListOrder is DistanceFromCenter.
enum class PinnedAppsAnchor { OuterEdge, AdjacentToStart };

struct ModSettings {
    double gapPx;
    SystemButtonsPlacement systemButtonsPlacement;
    Side systemButtonsAdjacentSide;
    std::vector<std::wstring> leftApps;
    std::vector<std::wstring> rightApps;
    UnresolvedAppsDefaultSide unresolvedAppsDefaultSide;
    TaskListOrder taskListOrder;
    PinnedAppsAnchor pinnedAppsAnchor;
    bool trackWindowPositions;
};
ModSettings g_settings;

// CharLowerBuffW, not towlower per-character: towlower only maps ASCII in
// the default C locale, making leftApps/rightApps matching effectively
// case-sensitive for any non-ASCII app name.
std::wstring ToLower(std::wstring s) {
    if (!s.empty()) {
        CharLowerBuffW(s.data(), (DWORD)s.size());
    }
    return s;
}

// Splits a comma-separated settings string into trimmed, lowercased,
// non-empty fragments for case-insensitive substring matching later.
std::vector<std::wstring> ParseAppList(PCWSTR raw) {
    std::vector<std::wstring> result;
    std::wstring s(raw);
    size_t start = 0;
    while (start <= s.size()) {
        size_t comma = s.find(L',', start);
        size_t end = (comma == std::wstring::npos) ? s.size() : comma;

        size_t first = s.find_first_not_of(L" \t", start);
        size_t last = s.find_last_not_of(L" \t", end == start ? end : end - 1);
        if (first != std::wstring::npos && first < end && last != std::wstring::npos &&
            last >= first) {
            result.push_back(ToLower(s.substr(first, last - first + 1)));
        }

        if (comma == std::wstring::npos) {
            break;
        }
        start = comma + 1;
    }

    return result;
}

Side ParseSide(PCWSTR value, Side fallback) {
    if (wcscmp(value, L"right") == 0) {
        return Side::Right;
    }
    if (wcscmp(value, L"left") == 0) {
        return Side::Left;
    }
    return fallback;
}

UnresolvedAppsDefaultSide ParseUnresolvedAppsDefaultSide(PCWSTR value) {
    if (wcscmp(value, L"right") == 0) {
        return UnresolvedAppsDefaultSide::Right;
    }
    if (wcscmp(value, L"contralateral-to-system-buttons") == 0) {
        return UnresolvedAppsDefaultSide::ContralateralToSystemButtons;
    }
    return UnresolvedAppsDefaultSide::Left;
}

// Resolves the effective side for pinned-but-not-running apps not matched
// by leftApps/rightApps, per the unresolvedAppsDefaultSide setting.
Side ResolveUnresolvedAppsDefaultSide() {
    switch (g_settings.unresolvedAppsDefaultSide) {
        case UnresolvedAppsDefaultSide::Right:
            return Side::Right;
        case UnresolvedAppsDefaultSide::ContralateralToSystemButtons:
            if (g_settings.systemButtonsPlacement ==
                    SystemButtonsPlacement::AdjacentStart &&
                g_settings.systemButtonsAdjacentSide == Side::Right) {
                return Side::Left;
            }
            return Side::Right;
        case UnresolvedAppsDefaultSide::Left:
        default:
            return Side::Left;
    }
}

PinnedAppsAnchor ParsePinnedAppsAnchor(PCWSTR value) {
    if (wcscmp(value, L"adjacent-to-start") == 0) {
        return PinnedAppsAnchor::AdjacentToStart;
    }
    return PinnedAppsAnchor::OuterEdge;
}

// Order key for a pinned-but-not-running app (no window to measure a
// distance from). -infinity sorts innermost, +infinity sorts outermost.
double PinnedAppOrderKey() {
    return g_settings.pinnedAppsAnchor == PinnedAppsAnchor::AdjacentToStart
               ? -std::numeric_limits<double>::infinity()
               : std::numeric_limits<double>::infinity();
}

// Reads every setting into a fresh ModSettings. Doesn't touch XAML/COM, so
// it has no thread requirement - kept separate from g_settings itself so
// Wh_ModSettingsChanged can do this part on its own calling thread and
// marshal only the resulting struct's assignment (see ApplyLoadedSettings).
ModSettings LoadSettingsFromStore() {
    ModSettings s;
    // Clamped: a negative value would pull the icon groups toward Start
    // instead of away from it.
    s.gapPx = std::max(0, Wh_GetIntSetting(L"gapPx"));

    auto placement = WindhawkUtils::StringSetting::make(L"systemButtonsPlacement");
    s.systemButtonsPlacement =
        (wcscmp(placement, L"adjacent-start") == 0)
            ? SystemButtonsPlacement::AdjacentStart
            : SystemButtonsPlacement::FarLeft;

    auto adjacentSide =
        WindhawkUtils::StringSetting::make(L"systemButtonsAdjacentSide");
    s.systemButtonsAdjacentSide = ParseSide(adjacentSide, Side::Left);

    s.leftApps =
        ParseAppList(WindhawkUtils::StringSetting::make(L"leftApps"));
    s.rightApps =
        ParseAppList(WindhawkUtils::StringSetting::make(L"rightApps"));

    s.unresolvedAppsDefaultSide = ParseUnresolvedAppsDefaultSide(
        WindhawkUtils::StringSetting::make(L"unresolvedAppsDefaultSide"));

    auto taskListOrder = WindhawkUtils::StringSetting::make(L"taskListOrder");
    s.taskListOrder = (wcscmp(taskListOrder, L"taskbar-order") == 0)
                           ? TaskListOrder::TaskbarOrder
                           : TaskListOrder::DistanceFromCenter;

    s.pinnedAppsAnchor = ParsePinnedAppsAnchor(
        WindhawkUtils::StringSetting::make(L"pinnedAppsAnchor"));

    s.trackWindowPositions = Wh_GetIntSetting(L"trackWindowPositions") != 0;

    return s;
}

void LoadSettings() {
    g_settings = LoadSettingsFromStore();
}

// ============================================================================
// Globals
// ============================================================================

std::atomic<bool> g_taskbarViewDllLoaded;
std::atomic<bool> g_unloading;

thread_local bool g_inTaskbarArrangeOverride;

// RecomputeLayoutPlan does its entire XAML-tree traversal once per
// ArrangeOverride pass, up front, writing every element's target X into
// g_lastArrangedX - IUIElement_Arrange_Hook is then a pure map lookup.
// Never traverse the tree from inside a nested Arrange call instead -
// STATUS_STOWED_EXCEPTION was observed doing that when a window moves
// across monitors while Windows' "show taskbar apps on" setting isn't
// "All taskbars," since that structurally adds/removes taskbar buttons
// mid-traversal.

// Taskbar window handle, resolved by EnsureTaskbarWnd. atomic: written on
// the taskbar thread (or Wh_ModAfterInit's thread at startup), read from
// the dedicated background worker thread. Callers with more than one read
// snapshot it into a local first so all their reads agree on one value.
std::atomic<HWND> g_hTaskbarWnd;

// Whether TaskbarWndSubclassProc is installed on g_hTaskbarWnd - see
// EnsureTaskbarWnd (which retries the install every call until it
// succeeds) and InvalidateTaskbarLayout/ButtonHwndResolveTimerProc/
// ApplyLoadedSettings (which check it before posting to the taskbar
// thread; there is no fallback marshal). Core centering/splitting is
// unaffected either way, since RecomputeLayoutPlan computes that
// synchronously regardless.
std::atomic<bool> g_taskbarWndSubclassed;

// How often DragFollowPollTimerProc has ticked and actually found a
// tracked window's rect changed, respectively - see that function.
std::atomic<int> g_dragFollowPollCount;
std::atomic<int> g_dragFollowInvalidateCount;
std::atomic<int> g_invalidateSkippedReentrant;
std::atomic<int> g_invalidateExceptions;

// Only touched from the dedicated background thread (DragFollowPollTimerProc,
// ButtonHwndResolveTimerProc both run there exclusively).
UINT_PTR g_dragFollowPollTimerId;

// The HWND-resolve timer's own id, on the same dedicated thread.
UINT_PTR g_buttonHwndResolveTimerId;

// ============================================================================
// Generic taskbar/XAML helpers
// (traversal helpers adapted from the "Start button always on the left"
// Windhawk mod, which uses the same VisualTreeHelper/ItemsRepeater walking
// pattern to reach taskbar elements)
// ============================================================================

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcessId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

FrameworkElement EnumChildElements(
    FrameworkElement element,
    const std::function<bool(FrameworkElement)>& enumCallback) {
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

FrameworkElement FindChildByClassName(FrameworkElement element,
                                       PCWSTR className) {
    return EnumChildElements(element, [className](FrameworkElement child) {
        return winrt::get_class_name(child) == className;
    });
}

// Returns the ItemsRepeater's realized (non-virtualized) children in order.
std::vector<FrameworkElement> GetRepeaterChildElements(
    FrameworkElement repeaterElement) {
    std::vector<FrameworkElement> result;

    auto repeater =
        repeaterElement
            .try_as<winrt::Microsoft::UI::Xaml::Controls::ItemsRepeater>();
    if (!repeater) {
        return result;
    }

    auto itemsSourceView = repeater.ItemsSourceView();
    int count = itemsSourceView ? itemsSourceView.Count() : 0;

    for (int index = 0; index < count; index++) {
        auto element = repeater.TryGetElement(index);
        if (!element) {
            continue;  // Virtualized away, not currently realized.
        }

        auto child = element.try_as<FrameworkElement>();
        if (child) {
            result.push_back(child);
        }
    }

    return result;
}


// ============================================================================
// taskbar.dll: locating the taskbar's XamlRoot
// (verbatim technique from the "Start button always on the left" mod)
// ============================================================================

void* CTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

void* TaskbarHost_FrameHeight_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

XamlRoot XamlRootFromTaskbarHostSharedPtr(void* taskbarHostSharedPtr[2]) {
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        return nullptr;
    }

    // The shared_ptr's ref-count block must be released on every path
    // below, not just a fully successful call.
    struct DecrefGuard {
        void* ptr;
        ~DecrefGuard() { std__Ref_count_base__Decref_Original(ptr); }
    } decrefGuard{taskbarHostSharedPtr[1]};

    // Slot 0 can still be null even though the top-level check passed.
    if (!taskbarHostSharedPtr[0]) {
        return nullptr;
    }

    // The XAML element pointer's offset inside TaskbarHost isn't exposed
    // by any symbol, so it's read out of a neighboring function's
    // prologue instead. x86-64 and ARM64 need separate opcode patterns
    // (explorer.exe runs natively as ARM64 too). Bails out on a pattern
    // mismatch rather than proceeding with a guessed offset.
    size_t taskbarElementIUnknownOffset = 0;
    bool patternMatched = false;
#if defined(_M_X64)
    // 48:83EC 28 | sub rsp,28
    // 48:83C1 48 | add rcx,48
    const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
    if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
        b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F) {
        taskbarElementIUnknownOffset = b[7];
        patternMatched = true;
    }
#elif defined(_M_ARM64)
    // 7f2303d5 pacibsp
    // fd7bbfa9 stp     fp, lr, [sp, #-0x10]!
    // fd030091 mov     fp, sp
    // 080c41f8 ldr     x8, [x0, #0x10]!
    const DWORD* p = (const DWORD*)TaskbarHost_FrameHeight_Original;
    if (p[0] == 0xD503237F && (p[1] & 0xFFC07FFF) == 0xA9807BFD &&
        p[2] == 0x910003FD && (p[3] & 0xFFF00FE0) == 0xF8400C00) {
        taskbarElementIUnknownOffset = (p[3] >> 12) & 0xFF;
        patternMatched = true;
    }
#else
#error "Unsupported architecture"
#endif
    if (!patternMatched) {
        Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        return nullptr;
    }

    auto* taskbarElementIUnknown =
        *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] +
                      taskbarElementIUnknownOffset);
    if (!taskbarElementIUnknown) {
        return nullptr;
    }

    FrameworkElement taskbarElement = nullptr;
    taskbarElementIUnknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                            winrt::put_abi(taskbarElement));

    return taskbarElement ? taskbarElement.XamlRoot() : nullptr;
}

XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    if (!taskBand) {
        // Null while the taskband window exists but its extra data isn't
        // populated yet (e.g. mid-creation after a taskbar recreate) - a
        // null deref here is a raw access violation, not a catchable
        // C++ exception.
        return nullptr;
    }
    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite !=
                    CTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }

        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                      taskbarHostSharedPtr);

    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

// ============================================================================
// taskbar.dll: resolving the HWND behind a taskbar button
//
// Individual (ungrouped) button:
//   TaskListButton (XAML) -> TaskListWindowViewModel -> WindowsUdk ITaskItem
//   -> (sentinel click, intercepted before it does anything) -> native
//   ITaskItem -> HWND.
//
// Grouped button ("Combine taskbar buttons: Always" - not bound to a
// TaskListWindowViewModel, so the above doesn't apply):
//   TaskListButton (XAML) -> TaskListGroupViewModel -> (second sentinel, via
//   IsMultiWindow's internal ITaskGroup::IsRunning call) -> WindowsUdk
//   ITaskGroup -> (sentinel click) -> native ITaskGroup -> its task-items
//   array (see GetTaskItemsArray) -> first item's HWND, the group's
//   representative position.
//
// Read-only reuse of the click-sentinel technique from taskbar-reordering
// and per-app-volume-control mods, purely to find where a window is.
// ============================================================================

using TaskListButton_get_IsRunning_t = HRESULT(WINAPI*)(void* pThis,
                                                         bool* running);
TaskListButton_get_IsRunning_t TaskListButton_get_IsRunning_Original;

// Cheap, direct read of the XAML button's own running-state - lets
// ResolveHwndFromTaskListButton skip the whole click-sentinel chain
// (TryGetItemFromContainer/IsMultiWindow/ITaskGroup::IsRunning and friends)
// for a pinned-but-not-running button, rather than paying that chain's
// full cost every retry only to reach the same answer. Adapted from
// taskbar-labels.wh.cpp's TaskListButton_IsRunning, down to the
// element.as<IUnknown>() form used to get the ABI pointer (that mod is
// GPL-3.0, same as this one). Optional (see
// HookTaskbarViewDllSymbols); defaults to "assume running" - i.e. don't
// skip anything - both when the symbol hasn't resolved at all, and when a
// resolved call still fails (the failed call leaves isRunning at its
// initialized false, so treating that as confirmed-not-running would
// silently misclassify it instead of falling back the same way an
// unresolved symbol does), which is exactly this mod's behavior before
// this optimization existed.
bool TaskListButtonIsRunning(FrameworkElement element) {
    if (!TaskListButton_get_IsRunning_Original) {
        return true;
    }
    bool isRunning = false;
    if (FAILED(TaskListButton_get_IsRunning_Original(
            winrt::get_abi(element.as<winrt::Windows::Foundation::IUnknown>()),
            &isRunning))) {
        return true;
    }
    return isRunning;
}

void* CImmersiveTaskItem_vftable;

using CWindowTaskItem_GetWindow_t = HWND(WINAPI*)(void* pThis);
CWindowTaskItem_GetWindow_t CWindowTaskItem_GetWindow_Original;

using CImmersiveTaskItem_GetWindow_t = HWND(WINAPI*)(void* pThis);
CImmersiveTaskItem_GetWindow_t CImmersiveTaskItem_GetWindow_Original;

using CTaskListWnd_HandleClick_t = HRESULT(WINAPI*)(void* pThis,
                                                     void* taskGroup,
                                                     void* taskItem,
                                                     void** launcherOptions);
CTaskListWnd_HandleClick_t CTaskListWnd_HandleClick_Original;

WCHAR g_clickSentinel[] = L"click-sentinel";
thread_local void* g_clickSentinel_TaskItem;
thread_local void* g_clickSentinel_TaskGroup;

// Dispatches a REAL click through TaskItem::ReportClicked/
// TaskGroup::ReportClicked, relying entirely on CTaskListWnd_HandleClick_Hook
// below to recognize the sentinel and swallow it before the taskbar's real
// HandleClick runs. Unlike reference mods (which only probe on an actual
// user gesture), this mod probes unattended from a background timer. If
// interception ever stops recognizing the sentinel - the hook stays
// installed but a probe click reaches the taskbar's actual HandleClick
// unswallowed - &g_clickSentinel (a WCHAR[]) gets delivered as the real
// winrt::Windows::System::LauncherOptions const& argument, so the
// realistic worst case is a stray window activate/minimize or an access
// violation in explorer.exe, not merely a resolution failure.
//
// Latched per path (item vs. group), not one shared flag, since the two
// paths reach CTaskListWnd::HandleClick through different internal call
// chains and a Windows update could break just one. This latch only ever
// applies pre-confirmation (see NoteUnconfirmedClickSentinelMiss).
//
// The threshold is deliberately NOT 1, even though the asymmetry here
// (a false latch costs position tracking, a false negative
// activates/minimizes windows the user didn't touch) argues for failing
// closed early. By the time any probe runs at all,
// g_realTaskbarClickObserved is already true - and that flag is set
// inside CTaskListWnd_HandleClick_Hook itself, so the interception point
// is provably installed and provably reached by real clicks. A miss
// after that isn't strong evidence of a broken hook; it's most likely
// the same transient this file already calls "usually innocent" for the
// post-confirmation case (the window closed mid-probe, or a taskbar
// rebuild caught the item mid-teardown), and the first probes of a
// session are exactly when those transients are most likely, since
// buttons are still realizing. Latching on one of those would silently
// disable position tracking for the rest of the session - every running
// app piling onto one side of Start - recoverable only by reloading the
// mod. Three bounds the worst case at three stray clicks per path,
// still well short of a runaway, while making an accidental permanent
// latch on startup churn much less likely.
std::atomic<bool> g_clickSentinelItemConfirmed;
std::atomic<bool> g_clickSentinelItemBroken;
std::atomic<int> g_clickSentinelItemMisses;
std::atomic<bool> g_clickSentinelGroupConfirmed;
std::atomic<bool> g_clickSentinelGroupBroken;
std::atomic<int> g_clickSentinelGroupMisses;
constexpr int kClickSentinelMissesBeforeBroken = 3;

// Which path's probe is in flight on this thread - lets
// CTaskListWnd_HandleClick_Hook below credit the right path's *Confirmed
// flag, since it only sees the click itself, not which resolve function
// dispatched it.
thread_local bool g_clickSentinelProbingGroup;

// Set the first time a genuine (non-sentinel) click reaches this hook -
// gates the very first unattended sentinel probe of the session on proof
// that CTaskListWnd::HandleClick's hook is actually installed and
// dispatching real clicks through it correctly, before ever risking one
// becoming a real click if interception is broken.
std::atomic<bool> g_realTaskbarClickObserved;

// Forward declarations: both are defined later (Per-button HWND cache /
// Mod lifecycle sections), needed here so the first-real-click branch
// below can nudge the resolve timer the instant that click is observed,
// rather than waiting for the next backoff tick.
extern std::atomic<bool> g_forceResolveUnresolved;
void ArmButtonHwndResolveTimer(DWORD delayMs);

HRESULT WINAPI CTaskListWnd_HandleClick_Hook(void* pThis,
                                              void* taskGroup,
                                              void* taskItem,
                                              void** launcherOptions) {
    if (launcherOptions && *launcherOptions == (void*)&g_clickSentinel) {
        g_clickSentinel_TaskItem = taskItem;
        g_clickSentinel_TaskGroup = taskGroup;
        // .exchange so the log fires exactly once, the first time this
        // path is confirmed.
        bool alreadyConfirmed =
            g_clickSentinelProbingGroup
                ? g_clickSentinelGroupConfirmed.exchange(true)
                : g_clickSentinelItemConfirmed.exchange(true);
        if (!alreadyConfirmed) {
            Wh_Log(L"Click-sentinel interception confirmed working (%s path)",
                   g_clickSentinelProbingGroup ? L"group" : L"item");
        }
        // Clears this path's accumulated misses: a capture proves the
        // interception point works, so whatever missed earlier was a
        // transient rather than evidence against it. Belt-and-suspenders
        // as the code stands, since NoteUnconfirmedClickSentinelMiss
        // already stops counting once Confirmed is set (which just
        // happened above) - but it keeps "counts consecutive unconfirmed
        // misses" true of the counter on its own, rather than only of
        // these two flags read together.
        (g_clickSentinelProbingGroup ? g_clickSentinelGroupMisses
                                     : g_clickSentinelItemMisses) = 0;
        return S_OK;
    }

    // .exchange so the force-resolve nudge below only ever fires once,
    // the first time a real click confirms the interception point is
    // reachable - every button that was bailing out at
    // g_realTaskbarClickObserved's own check gets an immediate recheck
    // instead of waiting for its next 2s backoff tick.
    if (!g_realTaskbarClickObserved.exchange(true)) {
        g_forceResolveUnresolved = true;
        ArmButtonHwndResolveTimer(0);
    }
    return CTaskListWnd_HandleClick_Original(pThis, taskGroup, taskItem,
                                              launcherOptions);
}

// Called right after a real ReportClicked probe comes back with no
// capture. Latches "broken" once a path accumulates
// kClickSentinelMissesBeforeBroken misses without ever having captured
// (see that constant's own comment for the threshold's reasoning); a
// no-longer-pre-confirmation miss (the confirmed check below) is a no-op
// instead of ever latching, since a miss after interception has already
// proven itself once is far more likely innocent (window closed
// mid-probe, etc.) than evidence of a break.
void NoteUnconfirmedClickSentinelMiss(bool isGroupPath) {
    std::atomic<bool>& confirmed =
        isGroupPath ? g_clickSentinelGroupConfirmed : g_clickSentinelItemConfirmed;
    std::atomic<int>& misses =
        isGroupPath ? g_clickSentinelGroupMisses : g_clickSentinelItemMisses;
    std::atomic<bool>& broken =
        isGroupPath ? g_clickSentinelGroupBroken : g_clickSentinelItemBroken;
    if (confirmed) {
        return;
    }
    if (misses.fetch_add(1) + 1 >= kClickSentinelMissesBeforeBroken &&
        !broken.exchange(true)) {
        Wh_Log(L"Click-sentinel interception (%s path) never confirmed "
               L"working after %d unconfirmed probes - disabling further "
               L"HWND-resolution probes on this path to avoid dispatching "
               L"real clicks. This usually means a Windows update changed "
               L"CTaskListWnd::HandleClick's internal call path; consider "
               L"disabling this mod until it's updated.",
               isGroupPath ? L"group" : L"item", kClickSentinelMissesBeforeBroken);
    }
}

using TryGetItemFromContainer_TaskListWindowViewModel_t =
    void(WINAPI*)(void* resultPut, void* uiElement);
TryGetItemFromContainer_TaskListWindowViewModel_t
    TryGetItemFromContainer_TaskListWindowViewModel_Original;

using TaskListWindowViewModel_get_TaskItem_t = int(WINAPI*)(void* pThis,
                                                             void** result);
TaskListWindowViewModel_get_TaskItem_t
    TaskListWindowViewModel_get_TaskItem_Original;

using TaskItem_ReportClicked_t = int(WINAPI*)(void* pThis, void* param);
TaskItem_ReportClicked_t TaskItem_ReportClicked_Original;

using TryGetItemFromContainer_TaskListGroupViewModel_t =
    void(WINAPI*)(void* resultPut, void* uiElement);
TryGetItemFromContainer_TaskListGroupViewModel_t
    TryGetItemFromContainer_TaskListGroupViewModel_Original;

using TaskListGroupViewModel_IsMultiWindow_t = bool(WINAPI*)(void* pThis);
TaskListGroupViewModel_IsMultiWindow_t
    TaskListGroupViewModel_IsMultiWindow_Original;

thread_local bool g_captureTaskGroup;
thread_local void* g_capturedTaskGroup;

// RAII around g_captureTaskGroup's set/clear pair - a non-local exit
// between them would otherwise leave ITaskGroup_IsRunning_Hook permanently
// answering every real IsRunning call with a hardcoded false.
struct ScopedCaptureTaskGroup {
    ScopedCaptureTaskGroup() { g_captureTaskGroup = true; }
    ~ScopedCaptureTaskGroup() { g_captureTaskGroup = false; }
};

using ITaskGroup_IsRunning_t = bool(WINAPI*)(void* pThis);
ITaskGroup_IsRunning_t ITaskGroup_IsRunning_Original;
bool WINAPI ITaskGroup_IsRunning_Hook(void* pThis) {
    if (g_captureTaskGroup) {
        g_capturedTaskGroup = *(void**)pThis;
        return false;
    }

    return ITaskGroup_IsRunning_Original(pThis);
}

using CTaskGroup_GetNumItems_t = int(WINAPI*)(void* pThis);
CTaskGroup_GetNumItems_t CTaskGroup_GetNumItems_Original;

// CTaskGroup::GetNumItems' body is just `return
// DPA_GetPtrCount(this->taskItemsArray);` - calling it with a fake "this"
// that's actually an array of pointers-to-sequential-ints turns that read
// into a self-reporting probe for the real offset.
constexpr int kTaskItemsArrayProbeSize = 256;

size_t GetTaskItemsArrayOffset() {
    static size_t offset = [] {
        int arrayOfInts[kTaskItemsArrayProbeSize];
        int* arrayOfIntPtrs[kTaskItemsArrayProbeSize];
        for (int i = 0; i < kTaskItemsArrayProbeSize; i++) {
            arrayOfInts[i] = i;
            arrayOfIntPtrs[i] = &arrayOfInts[i];
        }
        return CTaskGroup_GetNumItems_Original
                   ? (size_t)CTaskGroup_GetNumItems_Original(arrayOfIntPtrs)
                   : 0;
    }();
    return offset;
}

HDPA GetTaskItemsArray(void* taskGroup) {
    if (!CTaskGroup_GetNumItems_Original) {
        return nullptr;
    }
    size_t offset = GetTaskItemsArrayOffset();
    // Offset 0 is the vtable slot, so a legitimate probe never lands there -
    // an out-of-range offset means GetNumItems isn't the trivial form this
    // relies on, and trusting it would be a wild read.
    if (offset == 0 || offset >= kTaskItemsArrayProbeSize) {
        return nullptr;
    }
    return (HDPA)((void**)taskGroup)[offset];
}

using TaskGroup_ReportClicked_t = int(WINAPI*)(void* pThis, void* param);
TaskGroup_ReportClicked_t TaskGroup_ReportClicked_Original;

// Diagnostics only: how often the HWND-resolution chain succeeds vs
// fails, across both paths combined.
struct ResolveStats {
    int success = 0;
    int failure = 0;
};
ResolveStats g_resolveStats;

HWND GetWindowFromNativeTaskItem(void* nativeTaskItem) {
    if (!nativeTaskItem) {
        return nullptr;
    }

    if (CImmersiveTaskItem_vftable &&
        *(void**)nativeTaskItem == CImmersiveTaskItem_vftable) {
        return CImmersiveTaskItem_GetWindow_Original
                   ? CImmersiveTaskItem_GetWindow_Original(nativeTaskItem)
                   : nullptr;
    }

    return CWindowTaskItem_GetWindow_Original
               ? CWindowTaskItem_GetWindow_Original(nativeTaskItem)
               : nullptr;
}

// outClickDispatched is set true iff this call actually reached
// TaskItem_ReportClicked_Original - see g_realTaskbarClickObserved's
// caller (ResolveAndCacheButtonHwnd) for why bail-outs before that point
// must be distinguished from a genuinely dispatched-and-missed click.
HWND ResolveHwndFromIndividualTaskItem(FrameworkElement element,
                                       bool& outClickDispatched) {
    outClickDispatched = false;

    // CTaskListWnd_HandleClick_Original proves the interception hook is
    // actually installed - it's optional (see HookTaskbarDllSymbols), so
    // on a build where it didn't resolve, the ReportClicked call below
    // would be a genuine click rather than an intercepted sentinel.
    if (!CTaskListWnd_HandleClick_Original ||
        !TryGetItemFromContainer_TaskListWindowViewModel_Original ||
        !TaskListWindowViewModel_get_TaskItem_Original ||
        !TaskItem_ReportClicked_Original) {
        return nullptr;
    }

    // g_realTaskbarClickObserved is checked once, centrally, by the sole
    // caller (ResolveHwndFromTaskListButton) before either path here ever
    // runs - see its own comment.

    IUnknown* elementAbi = (IUnknown*)winrt::get_abi(element);

    winrt::com_ptr<IUnknown> windowViewModel;
    TryGetItemFromContainer_TaskListWindowViewModel_Original(
        windowViewModel.put_void(), &elementAbi);
    if (!windowViewModel) {
        g_resolveStats.failure++;
        return nullptr;
    }

    winrt::com_ptr<IUnknown> windowsUdkTaskItem;
    if (FAILED(TaskListWindowViewModel_get_TaskItem_Original(
            windowViewModel.get(), windowsUdkTaskItem.put_void())) ||
        !windowsUdkTaskItem) {
        g_resolveStats.failure++;
        return nullptr;
    }

    g_clickSentinel_TaskItem = nullptr;
    g_clickSentinelProbingGroup = false;
    outClickDispatched = true;
    TaskItem_ReportClicked_Original(windowsUdkTaskItem.get(),
                                     &g_clickSentinel);

    void* nativeTaskItem = g_clickSentinel_TaskItem;
    g_clickSentinel_TaskItem = nullptr;
    if (!nativeTaskItem) {
        NoteUnconfirmedClickSentinelMiss(/*isGroupPath=*/false);
        g_resolveStats.failure++;
        return nullptr;
    }

    g_resolveStats.success++;
    return GetWindowFromNativeTaskItem(nativeTaskItem);
}

// Grouped button (all windows of one app collapsed under a single icon,
// e.g. "Combine taskbar buttons" set to Always) - see the resolution
// overview comment above this section for the full chain. outClickDispatched
// - see ResolveHwndFromIndividualTaskItem's own comment.
HWND ResolveHwndFromTaskGroup(FrameworkElement element,
                              bool& outClickDispatched) {
    outClickDispatched = false;

    // Same interception hook as ResolveHwndFromIndividualTaskItem
    // (CTaskListWnd::HandleClick handles both item and group paths).
    if (!CTaskListWnd_HandleClick_Original ||
        !TryGetItemFromContainer_TaskListGroupViewModel_Original ||
        !TaskListGroupViewModel_IsMultiWindow_Original ||
        !TaskGroup_ReportClicked_Original || !CTaskGroup_GetNumItems_Original) {
        return nullptr;
    }

    // g_realTaskbarClickObserved is checked once, centrally, by the sole
    // caller (ResolveHwndFromTaskListButton) before either path here ever
    // runs - see its own comment.

    // Validated up front (memoized, side-effect-free) rather than only
    // after dispatching a click: without this, a build where
    // CTaskGroup::GetNumItems stopped being the trivial form this offset
    // probe relies on would still dispatch a click on every single group
    // resolution attempt (the sentinel latch never trips, since
    // interception itself is unaffected - it's GetTaskItemsArray's own
    // bounds check further down that always fails instead), turning every
    // attempt into a pure-waste ReportClicked with nothing to bound it.
    size_t taskItemsOffset = GetTaskItemsArrayOffset();
    if (taskItemsOffset == 0 || taskItemsOffset >= kTaskItemsArrayProbeSize) {
        g_resolveStats.failure++;
        return nullptr;
    }

    IUnknown* elementAbi = (IUnknown*)winrt::get_abi(element);

    winrt::com_ptr<IUnknown> groupViewModel;
    TryGetItemFromContainer_TaskListGroupViewModel_Original(
        groupViewModel.put_void(), &elementAbi);
    if (!groupViewModel) {
        g_resolveStats.failure++;
        return nullptr;
    }

    g_capturedTaskGroup = nullptr;
    {
        // IsMultiWindow's implementation calls ITaskGroup::IsRunning
        // internally, hooked above to capture its `this` instead of
        // really answering. The -1 adjusts from the QueryInterface
        // pointer to the adjacent vtable IsMultiWindow needs (a fixed
        // ABI detail, not a magic number).
        ScopedCaptureTaskGroup scopedCapture;
        TaskListGroupViewModel_IsMultiWindow_Original(
            (void**)groupViewModel.get() - 1);
    }

    void* windowsUdkTaskGroup = g_capturedTaskGroup;
    g_capturedTaskGroup = nullptr;
    if (!windowsUdkTaskGroup) {
        g_resolveStats.failure++;
        return nullptr;
    }

    // A group with no running windows (pinned-but-not-running) can never
    // yield an HWND - bail out before dispatching a click at all, rather
    // than detecting the failure after the fact. &windowsUdkTaskGroup
    // (not the pointer itself) matches how ITaskGroup_IsRunning_Hook's
    // "consume" calling convention captured it.
    if (ITaskGroup_IsRunning_Original &&
        !ITaskGroup_IsRunning_Original(&windowsUdkTaskGroup)) {
        g_resolveStats.failure++;
        return nullptr;
    }

    g_clickSentinel_TaskGroup = nullptr;
    g_clickSentinelProbingGroup = true;
    outClickDispatched = true;
    TaskGroup_ReportClicked_Original(windowsUdkTaskGroup, &g_clickSentinel);
    void* nativeTaskGroup = g_clickSentinel_TaskGroup;
    g_clickSentinel_TaskGroup = nullptr;
    if (!nativeTaskGroup) {
        NoteUnconfirmedClickSentinelMiss(/*isGroupPath=*/true);
        g_resolveStats.failure++;
        return nullptr;
    }

    HDPA taskItemsArray = GetTaskItemsArray(nativeTaskGroup);
    if (!taskItemsArray || DPA_GetPtrCount(taskItemsArray) <= 0) {
        g_resolveStats.failure++;
        return nullptr;
    }

    // The group's first window stands in for the whole group's position -
    // Windows itself doesn't expose a more meaningful "primary" window.
    void* taskItem = DPA_GetPtr(taskItemsArray, 0);
    HWND hwnd = GetWindowFromNativeTaskItem(taskItem);
    if (hwnd) {
        g_resolveStats.success++;
    }
    return hwnd;
}

HWND ResolveHwndFromTaskListButton(FrameworkElement element,
                                   bool& outClickDispatched,
                                   bool& outNotRunning,
                                   bool& outAwaitingFirstClick) {
    outNotRunning = false;
    outAwaitingFirstClick = false;
    outClickDispatched = false;

    // Checked first, above even TaskListButtonIsRunning: neither path
    // below can do anything until the interception point has proven
    // reachable this session (see g_realTaskbarClickObserved's own
    // comment), so every resolve attempt for every button - running or
    // not - would otherwise bail here anyway. Centralizing it lets the
    // caller cache "awaiting first click" as its own idle-worthy state,
    // instead of every such button polling at the fast backoff-0 cadence
    // for as long as the session goes without a real taskbar click - which
    // could otherwise be indefinite for a user who launches everything
    // from Start/Alt+Tab rather than clicking a taskbar button directly.
    if (!g_realTaskbarClickObserved) {
        outAwaitingFirstClick = true;
        return nullptr;
    }

    // Cheapest possible check next: skips the entire click-sentinel chain
    // below (both paths) for a button that isn't running right now, rather
    // than discovering that only after running it. See
    // TaskListButtonIsRunning.
    if (!TaskListButtonIsRunning(element)) {
        outNotRunning = true;
        return nullptr;
    }

    // Each path bails out once its own sentinel is known broken, rather
    // than dispatching more genuine clicks - see g_clickSentinelItemBroken/
    // g_clickSentinelGroupBroken.
    bool itemDispatched = false;
    HWND hwnd = g_clickSentinelItemBroken
                    ? nullptr
                    : ResolveHwndFromIndividualTaskItem(element, itemDispatched);
    if (hwnd || itemDispatched) {
        // A dispatched-and-missed item-path click already proves this
        // button is bound to a TaskListWindowViewModel, not a group - the
        // group path would just dispatch a second, wholly redundant click
        // for the same button.
        outClickDispatched = itemDispatched;
        return hwnd;
    }

    if (g_clickSentinelGroupBroken) {
        outClickDispatched = false;
        return nullptr;
    }
    return ResolveHwndFromTaskGroup(element, outClickDispatched);
}

// Per-button HWND cache, keyed by the XAML element's ABI pointer. Avoids
// re-running the resolution chain every pass, and negatively caches
// failures. consecutiveFailures drives capped exponential backoff (see
// ResolveBackoffMs) rather than a fixed retry. identity catches
// ItemsRepeater rebinding an already-realized element to a different item
// (e.g. a drag-reorder) - ResolvePendingButtonHwnds forces a re-resolve on
// mismatch.
struct ButtonHwndCacheEntry {
    HWND hwnd = nullptr;
    std::wstring identity;
    ULONGLONG lastAttempt = 0;
    int consecutiveFailures = 0;
    // Set when the last resolve attempt bailed via TaskListButtonIsRunning's
    // cheap pre-check specifically, rather than any other bail-out reason.
    // NextResolveDelayMs treats this the same as a resolved or terminal
    // entry (idle cadence, not the fast backoff-0 schedule), since a
    // confirmed-not-running button won't change state until
    // TaskListButton::UpdateVisualStates says otherwise.
    bool notRunning = false;
    // Set when the last resolve attempt bailed specifically because
    // g_realTaskbarClickObserved was still false - every button gets this
    // until the session's first real taskbar click, which could otherwise
    // mean an indefinite backoff-0 poll for a user who never happens to
    // click one. Treated the same as notRunning by NextResolveDelayMs;
    // cleared the instant a real click is observed, via
    // CTaskListWnd_HandleClick_Hook's own force-resolve nudge.
    bool awaitingFirstClick = false;
};
std::unordered_map<void*, ButtonHwndCacheEntry> g_buttonHwndCache;

// Set by TaskListButton::UpdateVisualStates' hook and the ArrangeOverride
// hook's count-change check to make the next resolve pass ignore a
// negatively-cached entry's backoff - capped at kMaxForcedRetryFailures
// failures each. The cap is load-bearing: a running app's button can keep
// failing for reasons that never bail out early, and forcing every retry
// would dispatch a real ReportClicked on both paths every forced pass,
// indefinitely - the runaway the backoff schedule exists to bound. It
// still leaves the "pinned app just launched" fast path intact, since
// that case has 0 failures when the fast path first fires.
std::atomic<bool> g_forceResolveUnresolved;
constexpr int kMaxForcedRetryFailures = 3;

// Whether any live button still needs a recheck - true for anything not
// yet resolved that also hasn't hit the terminal kMaxResolveFailures cap
// (this includes notRunning/awaitingFirstClick entries, which are worth
// rechecking on an event even though they idle for polling purposes -
// see ButtonHwndCacheEntry). Recomputed at the end of every
// ResolvePendingButtonHwnds pass; read (same thread, no marshal needed)
// by TaskListButton_UpdateVisualStates_Hook to skip nudging the resolve
// timer once there's nothing left it could help - starts true so the
// very first UpdateVisualStates call, before any pass has run yet,
// doesn't get skipped.
bool g_anyButtonNeedsRecheck = true;

// Terminal cap on per-entry retries (distinct from the force-bypass cap
// above) - without it, a button whose interception path is permanently
// broken would keep dispatching a real ReportClicked every
// ResolveBackoffMs interval, forever. Only a genuinely dispatched-and-
// missed click counts toward it (see ResolveAndCacheButtonHwnd); a
// bail-out before dispatching risks nothing and must not count. A pruned
// and recreated button gets a fresh entry and so a fresh count, but an
// identity change alone does NOT reset it - ResolveAndCacheButtonHwnd
// seeds failures from the existing entry regardless of identity, clearing
// them only on a successful resolve. NextResolveDelayMs must treat a
// terminal entry exactly like a resolved one - see its own comment for
// the busy-loop that requires.
constexpr int kMaxResolveFailures = 8;

// The HWNDs g_buttonHwndCache currently resolves to, rebuilt at the end of
// every successful ResolvePendingButtonHwnds pass and read by
// DragFollowPollTimerProc (a different thread) as the small, known set of
// windows worth polling for drag-follow, instead of listening for a
// system-wide location-change event. Needs its own mutex since it's read
// cross-thread outside a marshal.
std::mutex g_resolvedHwndsMutex;
std::unordered_set<HWND> g_resolvedHwnds;

// Runs the resolution chain and updates the cache. Deliberately ONLY ever
// called from the resolve timer, never from GetButtonHwnd or an active
// Arrange pass - running the click-sentinel technique while a button is
// mid-insertion/removal in an ItemsRepeater crashes explorer.exe with
// STATUS_STOWED_EXCEPTION (confirmed via crash-dump analysis), under the
// same condition described at g_inTaskbarArrangeOverride above.
bool ResolveAndCacheButtonHwnd(FrameworkElement element,
                               const std::wstring& identity) {
    void* key = winrt::get_abi(element);
    HWND previous = nullptr;
    int failures = 0;
    auto it = g_buttonHwndCache.find(key);
    if (it != g_buttonHwndCache.end()) {
        previous = it->second.hwnd;
        failures = it->second.consecutiveFailures;
    }

    bool clickDispatched = false;
    bool notRunning = false;
    bool awaitingFirstClick = false;
    HWND hwnd = ResolveHwndFromTaskListButton(element, clickDispatched,
                                              notRunning, awaitingFirstClick);
    // Only a genuinely dispatched-and-missed click counts toward
    // kMaxResolveFailures' terminal cap - a bail-out before ever
    // dispatching one (not yet confirmed reachable, a view-model lookup
    // miss, a not-currently-running group) isn't a click-safety risk and
    // must not count against a button that may resolve fine later.
    // lastAttempt still advances either way, so a
    // persistently-bailing-out entry keeps retrying at ResolveBackoffMs(0) -
    // a cheap, indefinite cadence - except a confirmed-not-running or
    // awaiting-first-click one, which NextResolveDelayMs instead idles
    // (see ButtonHwndCacheEntry).
    if (hwnd) {
        failures = 0;
    } else if (clickDispatched) {
        failures = failures + 1;
    }
    g_buttonHwndCache[key] = {hwnd, identity, GetTickCount64(), failures,
                              notRunning, awaitingFirstClick};
    return hwnd != previous;
}

// Read-only: never triggers resolution itself, only reads whatever the
// timer has already cached (see ResolveAndCacheButtonHwnd for why). A
// button with no entry yet, or a closed cached window, falls back to the
// default-side classification until the next tick.
HWND GetButtonHwnd(FrameworkElement element) {
    void* key = winrt::get_abi(element);

    auto it = g_buttonHwndCache.find(key);
    if (it == g_buttonHwndCache.end()) {
        return nullptr;
    }
    if (it->second.hwnd && !IsWindow(it->second.hwnd)) {
        return nullptr;
    }
    return it->second.hwnd;
}

// Defined later (Mod lifecycle section) alongside NextResolveDelayMs,
// which shares this same backoff formula - forward-declared here so
// ResolvePendingButtonHwnds' own needsResolve check (below) can use it.
ULONGLONG ResolveBackoffMs(int consecutiveFailures);

// ============================================================================
// Screen-position math
// ============================================================================

struct WindowClassification {
    Side side;
    // Distance in screen px between the window's center and the screen's
    // center line. Only meaningful for ordering same-side icons relative
    // to each other; unresolved/overridden buttons get +infinity so they
    // sort to the outer edge.
    double distanceFromCenter = std::numeric_limits<double>::infinity();
};

// A minimized window's GetWindowRect returns a nonsense off-screen
// position, so this freezes at the last known classification from before
// it was minimized instead.
std::unordered_map<HWND, WindowClassification> g_lastKnownWindowClassification;

// mi is the primary monitor's info, resolved once per plan recompute by
// RecomputeLayoutPlan and threaded down rather than looked up here - this
// runs once per resolved task list button per pass, and every call would
// otherwise repeat the same MonitorFromWindow/GetMonitorInfo pair for the
// same monitor. Null if that lookup failed, handled exactly like the
// per-call failure it replaced: fall back to the default side.
WindowClassification ClassifyByWindowPositionCached(HWND hwnd,
                                                    const MONITORINFO* mi) {
    RECT wr;

    if (IsIconic(hwnd)) {
        auto it = g_lastKnownWindowClassification.find(hwnd);
        if (it != g_lastKnownWindowClassification.end()) {
            return it->second;
        }

        // No prior classification - this window was minimized before we
        // ever saw it. GetWindowPlacement's rcNormalPosition still
        // reports the restored position while minimized (unlike
        // GetWindowRect).
        WINDOWPLACEMENT wp{.length = sizeof(wp)};
        if (!mi || !GetWindowPlacement(hwnd, &wp)) {
            return {ResolveUnresolvedAppsDefaultSide()};
        }
        wr = wp.rcNormalPosition;
        // rcNormalPosition is in workspace coordinates (relative to
        // rcWork), while screenCenterX below is computed from rcMonitor -
        // a left-docked appbar shifts these apart, so this offset keeps a
        // boundary window from classifying to the wrong side.
        LONG workOffsetX = mi->rcWork.left - mi->rcMonitor.left;
        wr.left += workOffsetX;
        wr.right += workOffsetX;
    } else if (!mi || !GetWindowRect(hwnd, &wr)) {
        return {ResolveUnresolvedAppsDefaultSide()};
    }

    double windowCenterX = (wr.left + wr.right) / 2.0;
    double screenCenterX = (mi->rcMonitor.left + mi->rcMonitor.right) / 2.0;

    WindowClassification result;
    result.side = windowCenterX < screenCenterX ? Side::Left : Side::Right;
    result.distanceFromCenter = std::abs(windowCenterX - screenCenterX);

    g_lastKnownWindowClassification[hwnd] = result;
    return result;
}

// Taskbar buttons expose their app's display name as an accessibility
// property (used by screen readers); reused here as a stable identifier
// that works even for pinned-but-not-running apps, which have no HWND.
// Blind spot: with "Combine taskbar buttons" set to "Never", two windows
// of the same app produce two TaskListButtons with this same name, so an
// ItemsRepeater rebind between those two specifically wouldn't be caught
// by an identity mismatch - this only catches a rebind onto a
// differently-named button.
std::wstring GetButtonAccessibleName(FrameworkElement element) {
    auto name = Automation::AutomationProperties::GetName(element);
    if (!name.empty()) {
        return ToLower(std::wstring(name.c_str()));
    }

    auto elementName = element.Name();
    if (!elementName.empty()) {
        return ToLower(std::wstring(elementName.c_str()));
    }

    return L"";
}

bool ContainsAnyFragment(const std::wstring& haystack,
                         const std::vector<std::wstring>& needles) {
    if (haystack.empty()) {
        return false;
    }

    for (auto& needle : needles) {
        if (!needle.empty() && haystack.find(needle) != std::wstring::npos) {
            return true;
        }
    }

    return false;
}

struct ButtonClassification {
    Side side;
    double orderKey = std::numeric_limits<double>::infinity();
    bool hwndResolved = false;
};

// Classification priority: explicit user override by app name, then live
// window position, then the configured default. Skips the accessible-name
// lookup entirely when leftApps/rightApps are both empty (the default) -
// this runs for every task-list button on every ArrangeOverride pass.
ButtonClassification ClassifyTaskListButton(FrameworkElement element,
                                            const MONITORINFO* mi) {
    if (!g_settings.leftApps.empty() || !g_settings.rightApps.empty()) {
        std::wstring name = GetButtonAccessibleName(element);
        if (ContainsAnyFragment(name, g_settings.leftApps)) {
            return {Side::Left, PinnedAppOrderKey()};
        }
        if (ContainsAnyFragment(name, g_settings.rightApps)) {
            return {Side::Right, PinnedAppOrderKey()};
        }
    }

    HWND hwnd = GetButtonHwnd(element);
    if (hwnd) {
        WindowClassification wc = ClassifyByWindowPositionCached(hwnd, mi);
        return {wc.side, wc.distanceFromCenter, true};
    }

    return {ResolveUnresolvedAppsDefaultSide(), PinnedAppOrderKey()};
}

// X coordinate (in the taskbar repeater's local DIPs) of the primary
// monitor's horizontal center.
double GetMonitorCenterXLocal() {
    // Snapshot once, per g_hTaskbarWnd's own comment - three reads below
    // otherwise each independently re-read the atomic.
    HWND hTaskbarWnd = g_hTaskbarWnd;
    HMONITOR mon = MonitorFromWindow(hTaskbarWnd, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi{.cbSize = sizeof(mi)};
    if (!GetMonitorInfo(mon, &mi)) {
        return 0;
    }

    RECT taskbarRect;
    if (!GetWindowRect(hTaskbarWnd, &taskbarRect)) {
        return 0;
    }

    UINT dpi = GetDpiForWindow(hTaskbarWnd);
    double scale = dpi ? (96.0 / dpi) : 1.0;

    double centerScreenPx = (mi.rcMonitor.left + mi.rcMonitor.right) / 2.0;
    double localPx = centerScreenPx - taskbarRect.left;
    return localPx * scale;
}

// Width of the taskbar window itself, in the repeater's local DIPs. Only
// used as RecomputeLayoutPlan's fallback bound for the right-side task
// list group (see its own comment) when the system tray's own element
// can't be resolved - the taskbar's own frame is a much looser bound than
// the tray's actual left edge, since the tray only occupies the last
// portion of the taskbar's full width.
double GetTaskbarWidthLocal() {
    // Snapshot once, per g_hTaskbarWnd's own comment.
    HWND hTaskbarWnd = g_hTaskbarWnd;
    RECT taskbarRect;
    if (!GetWindowRect(hTaskbarWnd, &taskbarRect)) {
        return 0;
    }

    UINT dpi = GetDpiForWindow(hTaskbarWnd);
    double scale = dpi ? (96.0 / dpi) : 1.0;
    return (taskbarRect.right - taskbarRect.left) * scale;
}

// element's own left edge (top-left corner), mapped into relativeTo's
// coordinate space - e.g. the system tray frame's left edge in the same
// local DIP space startCenterX and friends already use, since content
// (the XamlRoot's own root element) shares that space's origin.
// TransformToVisual/TransformPoint is the same technique
// taskbar-reorder-right-drag.wh.cpp (already a technique source for this
// mod) uses to map between an element's own space and an ancestor's.
double GetElementLeftXLocal(FrameworkElement element,
                            FrameworkElement relativeTo) {
    auto transform = element.TransformToVisual(relativeTo);
    auto point =
        transform.TransformPoint(winrt::Windows::Foundation::Point{0, 0});
    return point.X;
}

double FullFootprintWidth(FrameworkElement element) {
    Thickness m = element.Margin();
    return m.Left + element.ActualWidth() + m.Right;
}

// Search, Task View and Widgets use a negative-margin collapse trick when
// hidden/shown - ActualWidth() includes that margin, so it never drops
// below the collapsed width. Reads the content child's DesiredSize instead
// (matching taskbar-start-button-position.wh.cpp), falling back to
// ActualWidth() if no child is realized yet. NOT used for Start (never
// hidden/shown, and DesiredSize() understates its true width) or task list
// buttons (this file's hottest path, no evidence of the same bug).
//
// Persists across calls rather than being folded into one of the per-pass
// plan maps elsewhere in this file, since it specifically needs to outlive
// a single pass to answer the question below - RecomputeLayoutPlan prunes
// it down to the current pass's live system-button keys, since the key is
// a recyclable ABI pointer and a stale entry could otherwise be served to
// an unrelated new element at the same address.
std::unordered_map<void*, double> g_lastSystemButtonContentWidth;
double SystemButtonContentWidth(FrameworkElement element) {
    double contentWidth = element.ActualWidth();
    if (Media::VisualTreeHelper::GetChildrenCount(element) > 0) {
        auto child = Media::VisualTreeHelper::GetChild(element, 0)
                         .try_as<FrameworkElement>();
        if (child) {
            contentWidth = child.DesiredSize().Width;
        }
    }

    // element.ActualWidth() (the OUTER element, not the content child
    // above) reflects the previous Arrange pass and is 0 for one pass
    // right after the element first appears - the same race Start and
    // task list buttons already guard against elsewhere in this file. In
    // that same window the content child usually hasn't been measured
    // yet either, so its DesiredSize reads 0 even though the button will
    // end up with real content - without this, the reserved gap next to
    // Start would collapse to 0 for that one frame. Once the outer
    // element has gone through a real Arrange pass (ActualWidth() > 0), a
    // content width of 0 is trustworthy - that's this function's only
    // signal for a genuine show/hide toggle (see this function's own
    // comment above), which has to keep working live, so it must NOT be
    // masked by a "last known good width" fallback the way this one is
    // for the unmeasured case.
    void* key = winrt::get_abi(element);
    if (contentWidth == 0 && element.ActualWidth() == 0) {
        auto it = g_lastSystemButtonContentWidth.find(key);
        return it != g_lastSystemButtonContentWidth.end() ? it->second : 0;
    }
    g_lastSystemButtonContentWidth[key] = contentWidth;
    return contentWidth;
}

// Same margin-inclusive shape as FullFootprintWidth, for Search/Task
// View/Widgets. Start doesn't use this - its width is read bare.
double SystemButtonFootprintWidth(FrameworkElement element) {
    Thickness m = element.Margin();
    return m.Left + SystemButtonContentWidth(element) + m.Right;
}

// ============================================================================
// Element identification
// ============================================================================

enum class SystemButton {
    None,
    Start,
    Widgets,
    Search,
    TaskView,
};

SystemButton IdentifySystemButton(FrameworkElement element) {
    auto className = winrt::get_class_name(element);

    if (className == L"Taskbar.ExperienceToggleButton") {
        auto automationId =
            Automation::AutomationProperties::GetAutomationId(element);
        if (automationId == L"StartButton") {
            return SystemButton::Start;
        }
        if (automationId == L"TaskViewButton") {
            return SystemButton::TaskView;
        }
    } else if (className == L"Taskbar.AugmentedEntryPointButton") {
        if (element.Name() == L"AugmentedEntryPointButton") {
            return SystemButton::Widgets;
        }
    } else if (className == L"Taskbar.TaskbarExtensionElement") {
        return SystemButton::Search;
    }

    return SystemButton::None;
}

bool IsTaskListButton(FrameworkElement element) {
    return winrt::get_class_name(element) == L"Taskbar.TaskListButton";
}

// ============================================================================
// Layout: computing target X positions
// ============================================================================

// Small breathing room between the taskbar's own left edge and the first
// system button in "far left" placement mode.
constexpr double kFarLeftSystemButtonMarginPx = 8;

// Small breathing room between the right-side task list group and the
// system tray's own left edge (see RecomputeLayoutPlan's rightBoundLocal).
constexpr double kTrayMarginPx = 8;

// Total footprint of Search+TaskView+Widgets together, used both to lay
// them out and to reserve room for them next to Start (adjacent mode).
// Recomputed every pass so a hidden cluster reads as genuinely 0.
double g_lastLeftSystemClusterWidth = 0;
double g_lastRightSystemClusterWidth = 0;

// A repeater child paired with its SystemButton classification, computed
// once per child per pass and reused across loops rather than
// re-deriving IdentifySystemButton each time.
struct ChildInfo {
    FrameworkElement element;
    SystemButton systemButton;
};

// Places Search/TaskView/Widgets at the taskbar's far left edge or
// adjacent to Start, per g_settings.systemButtonsPlacement. clusterWidth
// is the combined width of all three; childInfos is the repeater's
// already-classified children (avoids re-walking/re-classifying).
double ComputeSystemButtonX(const std::vector<ChildInfo>& childInfos,
                            FrameworkElement targetElement,
                            double startCenterX,
                            double startWidth,
                            double clusterWidth) {
    // Sums every other system button's footprint that appears earlier than
    // targetElement in taskbar order - tracks whatever order the taskbar
    // itself presents these in, rather than a fixed Search/TaskView/Widgets
    // rank table (this replaced one): a fixed table assumes at most one
    // instance of each SystemButton value ever exists, so two elements
    // Windows both classifies the same way (e.g. two
    // Taskbar.TaskbarExtensionElements) would get the same rank, compute
    // the same X, and render on top of each other. This degrades
    // gracefully instead if that assumption ever breaks.
    double widthBefore = 0;
    for (auto& info : childInfos) {
        if (info.element == targetElement) {
            break;
        }
        if (info.systemButton != SystemButton::None &&
            info.systemButton != SystemButton::Start) {
            widthBefore += SystemButtonFootprintWidth(info.element);
        }
    }

    if (g_settings.systemButtonsPlacement == SystemButtonsPlacement::FarLeft) {
        return kFarLeftSystemButtonMarginPx + widthBefore;
    }

    double gap = g_settings.gapPx;
    double ownWidth = SystemButtonFootprintWidth(targetElement);

    if (g_settings.systemButtonsAdjacentSide == Side::Left) {
        // Stack right-to-left outward from Start: taskbar-order-earliest
        // button ends up farthest from Start (the outermost slot), so
        // reading left-to-right still shows the same order as far-left
        // mode does.
        double widthAfter = clusterWidth - widthBefore - ownWidth;
        return startCenterX - startWidth / 2.0 - gap - widthAfter - ownWidth;
    }

    return startCenterX + startWidth / 2.0 + gap + widthBefore;
}

// Updated whenever the Start button's own Arrange runs. Read by task list
// button arrangement so it doesn't need to assume Start lives in the same
// container as the task buttons.
double g_lastStartWidth = 48;

// Diagnostics only, for RecomputeLayoutPlan's once-per-pass traversal.
struct LayoutPlanStats {
    int taskListTotal = 0;
    int taskListHwndResolved = 0;
    int taskListLeft = 0;
    int taskListRight = 0;
    int exceptions = 0;
};
// thread_local: RecomputeLayoutPlan only ever does real work on the
// primary taskbar's own thread, but this stays correct even if that ever
// changed - same reasoning as g_passStats.
thread_local LayoutPlanStats g_planStats;

// Total realized repeater children (every kind) as of the last successful
// RecomputeLayoutPlan pass - used by its own staleness-backstop check to
// detect any child appearing/disappearing without a per-child class-name
// lookup. Broader than g_planStats.taskListTotal so a system-button
// visibility change also invalidates the plan.
thread_local int g_planChildCount;

struct TaskListPlanEntry {
    FrameworkElement element;
    ButtonClassification info;
    double width;
    int index;  // Position within `children`, i.e. taskbar order.
};

// Computes every task list button's target X in one O(n) pass. Every
// entry is written into outPlan and stays in this mod's own coordinate
// system - an overflowing side compresses inter-icon spacing (icons stay
// full width) to fit within leftBoundLocal/rightBoundLocal instead of
// falling through to native Arrange, since Start is forced to true center
// regardless.
//
// Each side is walked innermost-first from Start's own edge outward,
// using each icon's *unscaled* width for its own placement (only the
// running reference point advances by the scaled amount) - this
// guarantees the innermost icon's edge never drifts into Start under
// compression. Do not scale the placement itself instead.
void PlanTaskListButtons(const std::vector<FrameworkElement>& children,
                         double startCenterX,
                         double leftBoundLocal,
                         double rightBoundLocal,
                         const MONITORINFO* mi,
                         std::unordered_map<void*, double>& outPlan,
                         std::unordered_map<void*, double>& outWidths) {
    std::vector<TaskListPlanEntry> entries;
    for (int i = 0; i < (int)children.size(); i++) {
        if (IsTaskListButton(children[i])) {
            entries.push_back({children[i],
                               ClassifyTaskListButton(children[i], mi),
                               FullFootprintWidth(children[i]), i});
        }
    }

    g_planStats.taskListTotal = (int)entries.size();
    for (auto& entry : entries) {
        if (entry.info.hwndResolved) {
            g_planStats.taskListHwndResolved++;
        }
        if (entry.info.side == Side::Left) {
            g_planStats.taskListLeft++;
        } else {
            g_planStats.taskListRight++;
        }
    }

    double gap = g_settings.gapPx;
    double startWidth = g_lastStartWidth;
    bool adjacent =
        g_settings.systemButtonsPlacement == SystemButtonsPlacement::AdjacentStart;
    // Only add the gap when the cluster actually has width - otherwise an
    // empty cluster (all three hidden) still reserved a bare gapPx of dead
    // space next to Start for no reason.
    double leftExtra = (adjacent && g_settings.systemButtonsAdjacentSide == Side::Left &&
                        g_lastLeftSystemClusterWidth > 0)
                            ? (g_lastLeftSystemClusterWidth + gap)
                            : 0;
    double rightExtra = (adjacent && g_settings.systemButtonsAdjacentSide == Side::Right &&
                         g_lastRightSystemClusterWidth > 0)
                             ? (g_lastRightSystemClusterWidth + gap)
                             : 0;

    // Innermost reference point for each side - every side's innermost
    // icon is placed exactly here, regardless of compression.
    double leftInnerX = startCenterX - startWidth / 2.0 - gap - leftExtra;
    double rightInnerX = startCenterX + startWidth / 2.0 + gap + rightExtra;

    std::vector<TaskListPlanEntry*> left, right;
    for (auto& entry : entries) {
        (entry.info.side == Side::Left ? left : right).push_back(&entry);
    }

    if (g_settings.taskListOrder == TaskListOrder::DistanceFromCenter) {
        // Sort each side closest-to-center-first (innermost-first). Ties
        // break on taskbar index for a stable, predictable order.
        auto byOrderKey = [](const TaskListPlanEntry* a,
                             const TaskListPlanEntry* b) {
            if (a->info.orderKey != b->info.orderKey) {
                return a->info.orderKey < b->info.orderKey;
            }
            return a->index < b->index;
        };
        std::sort(left.begin(), left.end(), byOrderKey);
        std::sort(right.begin(), right.end(), byOrderKey);
    } else {
        // Preserve taskbar order, reading left-to-right across the split
        // layout: left side's earliest entry at the outer edge, right
        // side's earliest entry innermost. `left` needs reversing first
        // to get that (it's built in taskbar order, which would otherwise
        // put the earliest entry innermost instead).
        std::reverse(left.begin(), left.end());
    }

    double leftTotal = 0;
    for (auto* entry : left) {
        leftTotal += entry->width;
    }
    double rightTotal = 0;
    for (auto* entry : right) {
        rightTotal += entry->width;
    }

    // Scale <1 compresses inter-icon spacing (not each icon's rendered
    // width) so the outermost icon's edge never passes
    // leftBoundLocal/rightBoundLocal. The outermost icon's own width
    // (left.back()/right.back()) is excluded from the compressible pool -
    // it has nothing beyond it to compress against, so including it would
    // let its edge overshoot the bound.
    double leftAvailable = leftInnerX - leftBoundLocal;
    double leftScale = 1.0;
    if (!left.empty() && leftTotal > leftAvailable) {
        double leftOuterWidth = left.back()->width;
        double compressibleTotal = leftTotal - leftOuterWidth;
        double compressibleAvailable = leftAvailable - leftOuterWidth;
        leftScale = compressibleTotal > 0
                        ? std::max(0.0, compressibleAvailable / compressibleTotal)
                        : 1.0;
    }
    double rightAvailable = rightBoundLocal - rightInnerX;
    double rightScale = 1.0;
    if (!right.empty() && rightTotal > rightAvailable) {
        double rightOuterWidth = right.back()->width;
        double compressibleTotal = rightTotal - rightOuterWidth;
        double compressibleAvailable = rightAvailable - rightOuterWidth;
        rightScale = compressibleTotal > 0
                         ? std::max(0.0, compressibleAvailable / compressibleTotal)
                         : 1.0;
    }

    // Each icon's own placement uses its unscaled width, so the innermost
    // icon's edge lands exactly at leftInnerX/rightInnerX regardless of
    // scale - only `x` itself advances by the scaled amount. A
    // just-realized button's ActualWidth() is 0 for one pass (Margin
    // isn't, so entry->width alone isn't a reliable zero-width signal);
    // skipping its outPlan entry then avoids stacking it on its neighbor
    // for that one frame, falling through to native positioning instead -
    // safe since it's then also missing from g_lastArrangedX's coverage,
    // so RecomputeLayoutPlan's own staleness check forces a real recompute
    // the very next pass once its real width is available.
    double x = leftInnerX;
    for (auto* entry : left) {
        if (entry->element.ActualWidth() > 0) {
            void* key = winrt::get_abi(entry->element);
            outPlan[key] = x - entry->width;
            outWidths[key] = entry->width;
        }
        x -= entry->width * leftScale;
    }
    x = rightInnerX;
    for (auto* entry : right) {
        if (entry->element.ActualWidth() > 0) {
            void* key = winrt::get_abi(entry->element);
            outPlan[key] = x;
            outWidths[key] = entry->width;
        }
        x += entry->width * rightScale;
    }
}

// ============================================================================
// XAML hooks
// ============================================================================

// Defined later (Live drag-follow section). Only ever marks the taskbar's
// layout dirty - never forces a synchronous UpdateLayout() (a forced call
// from a nested callback reenters WinUI layout and raises
// STATUS_STOWED_EXCEPTION; see its own definition below for detail).
void InvalidateTaskbarLayout();

// Idle re-check cadence once every cached button is resolved - keeps
// identity re-checked periodically (an ItemsRepeater rebind can change
// which item an element represents without changing the button count).
constexpr DWORD kIdleResolveTickMs = 30000;

// Retry cadence for a pass that couldn't enumerate the live button set at
// all (e.g. a taskbar rebuild transiently breaks GetCachedTaskbarRepeater).
// Much shorter than kIdleResolveTickMs, which is only meant for a pass that
// actually confirmed the live set - see ScheduleNextResolveTick.
constexpr DWORD kEnumerationFailedRetryMs = 1000;

// Defined later (Mod lifecycle section).
void StartBackgroundWorkerThread();

// Defined later (Mod lifecycle section); lets a live trackWindowPositions
// toggle stop the background worker thread reversibly. A separate function from
// StopBackgroundWorkerThread specifically because that one also sets a permanent,
// one-way latch meant only for mod teardown - reusing it here would have
// silently bricked the setting after the first toggle-off, since turning
// it back on would then refuse to start (caught during the user's own live
// testing).
void StopBackgroundWorkerThreadForToggle();

// Defined later (Mod lifecycle section); lets the ArrangeOverride hook
// request an immediate HWND-resolve attempt.
void ArmButtonHwndResolveTimer(DWORD delayMs);

// Defined later (Mod lifecycle section).
DWORD NextResolveDelayMs();

// Defined later (Live drag-follow section).
LRESULT CALLBACK TaskbarWndSubclassProc(HWND hWnd,
                                        UINT uMsg,
                                        WPARAM wParam,
                                        LPARAM lParam,
                                        DWORD_PTR dwRefData);

// Defined later (Mod lifecycle section); applies a settings change
// ApplyLoadedSettings couldn't hand off to the taskbar thread earlier.
void ApplyPendingSettingsIfAny();

// Minimum gap between FindCurrentProcessTaskbarWnd attempts while the
// taskbar window is still unresolved - see EnsureTaskbarWnd.
constexpr ULONGLONG kTaskbarWndEnumRetryMs = 1000;

// Resolves g_hTaskbarWnd and installs the taskbar-window subclass,
// self-healing on every ArrangeOverride pass rather than a one-shot
// Wh_ModAfterInit lookup. Two independent reasons both parts need to keep
// retrying rather than running once: a fresh-boot race where Windhawk
// injects before Shell_TrayWnd exists yet would otherwise leave
// g_hTaskbarWnd permanently null, and a rare SetWindowSubclassFromAnyThread
// failure would otherwise permanently disable drag-follow/HWND-resolution/
// live-settings for the rest of the process (core centering/splitting
// keeps working either way, since that's computed synchronously).
HWND EnsureTaskbarWnd() {
    // Snapshot once, per g_hTaskbarWnd's own comment - every read below,
    // including the final return, agrees with whichever writes this same
    // call already made, rather than each re-reading the atomic
    // independently (this function is g_hTaskbarWnd's sole writer, so
    // that's benign today, but the snapshot keeps the code matching the
    // contract the variable's own comment states).
    HWND hTaskbarWnd = g_hTaskbarWnd;

    // Shell_TrayWnd can in principle be recreated without explorer.exe
    // restarting. Cheap enough to check unconditionally every pass.
    if (hTaskbarWnd && !IsWindow(hTaskbarWnd)) {
        hTaskbarWnd = nullptr;
        g_hTaskbarWnd = nullptr;
        // comctl32 tears the old subclass down on WM_NCDESTROY already -
        // this just keeps the flag in step so a fresh install is attempted
        // on the new window.
        g_taskbarWndSubclassed = false;
    }

    if (!hTaskbarWnd) {
        if (g_unloading) {
            // Guards against a pass landing here after Wh_ModBeforeUninit
            // already ran, with no teardown left to stop a newly-created
            // background worker thread.
            return nullptr;
        }
        // FindCurrentProcessTaskbarWnd runs an EnumWindows over every
        // top-level window on the desktop. While g_hTaskbarWnd is
        // unresolved this branch is reached on every ArrangeOverride pass
        // - and a secondary taskbar's own passes can fire before
        // Shell_TrayWnd resolves - so the retry is throttled rather than
        // enumerating the whole desktop at layout frequency. Costs
        // nothing once resolved, since the branch is skipped entirely
        // then. atomic because this function also runs on
        // Wh_ModAfterInit's thread, not just the taskbar's.
        static std::atomic<ULONGLONG> lastEnumTick;
        ULONGLONG nowTick = GetTickCount64();
        ULONGLONG lastTick = lastEnumTick.load();
        if (lastTick != 0 && nowTick - lastTick < kTaskbarWndEnumRetryMs) {
            return nullptr;
        }
        lastEnumTick.store(nowTick);

        hTaskbarWnd = FindCurrentProcessTaskbarWnd();
        if (!hTaskbarWnd) {
            return nullptr;
        }
        g_hTaskbarWnd = hTaskbarWnd;
        Wh_Log(L"Resolved taskbar window: %p", hTaskbarWnd);
        // Starting this thread is what turns on window-position tracking
        // at all - see trackWindowPositions' own settings description.
        // Only checked here at initial resolve, not on every call; a live
        // settings change afterward is handled separately by
        // TaskbarWndSubclassProc's SettingsChangedMsg case, via
        // StartBackgroundWorkerThread/StopBackgroundWorkerThreadForToggle directly.
        if (g_settings.trackWindowPositions) {
            StartBackgroundWorkerThread();
        } else {
            Wh_Log(L"trackWindowPositions is off - skipping the background "
                   L"worker thread and all HWND resolution");
        }
    }

    // Retried on every call until it succeeds, not just the pass that
    // first resolves g_hTaskbarWnd above (see this function's own comment
    // for why). Cheap on the common ArrangeOverride call path, since that
    // already runs on the taskbar's own thread and
    // SetWindowSubclassFromAnyThread takes its same-thread fast path there
    // (the Wh_ModAfterInit call site runs on Windhawk's own thread instead,
    // so it takes the real cross-thread marshal, but only once at startup).
    if (!g_taskbarWndSubclassed && !g_unloading &&
        WindhawkUtils::SetWindowSubclassFromAnyThread(
            hTaskbarWnd, TaskbarWndSubclassProc, 0)) {
        g_taskbarWndSubclassed = true;

        // Undo immediately if Wh_ModBeforeUninit's removal pass already
        // ran with the subclass not yet installed (reachable right after a
        // Shell_TrayWnd recreate, or a fresh-boot race, catches
        // EnsureTaskbarWnd still resolving as unload begins) - that
        // removal pass only ever runs once, so a subclass installed here
        // afterward would stay wired into Shell_TrayWnd with no later
        // removal call, and Windhawk unmaps this module's code shortly
        // after Wh_ModUninit returns - a use-after-free the next time that
        // window procedure runs.
        if (g_unloading && g_taskbarWndSubclassed.exchange(false)) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                hTaskbarWnd, TaskbarWndSubclassProc);
        } else {
            // Kicks the resolve timer and a relayout awake - needed since
            // nothing else restarts them once a subclass install has
            // failed for a while (see ButtonHwndResolveTimerProc). Also
            // applies any settings change that arrived before this point
            // could hand it off to the taskbar thread.
            ArmButtonHwndResolveTimer(0);
            InvalidateTaskbarLayout();
            ApplyPendingSettingsIfAny();
        }
    }

    return hTaskbarWnd;
}

FrameworkElement FindTaskbarFrameRepeater(FrameworkElement anyDescendant) {
    FrameworkElement child = anyDescendant;
    if (child && (child = FindChildByClassName(child, L"Taskbar.TaskbarFrame")) &&
        (child = FindChildByName(child, L"RootGrid")) &&
        (child = FindChildByName(child, L"TaskbarFrameRepeater"))) {
        return child;
    }
    return nullptr;
}

// Caches the resolved repeater across calls instead of redoing
// GetTaskbarXamlRoot's resolution chain plus FindTaskbarFrameRepeater's
// tree walk from scratch every time. A weak_ref alone isn't a reliable
// "still live" signal (a taskbar recreate or DPI change can leave it
// resolving to a now-detached element), so this also checks XamlRoot().
// MUST only run on g_hTaskbarWnd's own thread - same constraint as
// GetTaskbarXamlRoot, which this wraps.
winrt::weak_ref<FrameworkElement> g_cachedTaskbarRepeaterWeak;

FrameworkElement GetCachedTaskbarRepeater() {
    if (FrameworkElement cached = g_cachedTaskbarRepeaterWeak.get()) {
        if (cached.XamlRoot()) {
            return cached;
        }
        // Detached - clear and fall through to a fresh resolution.
        g_cachedTaskbarRepeaterWeak = nullptr;
    }

    XamlRoot xamlRoot = GetTaskbarXamlRoot(g_hTaskbarWnd);
    if (!xamlRoot) {
        return nullptr;
    }

    FrameworkElement content = xamlRoot.Content().try_as<FrameworkElement>();
    FrameworkElement repeater = FindTaskbarFrameRepeater(content);
    if (repeater) {
        g_cachedTaskbarRepeaterWeak = repeater;
    }
    return repeater;
}

// Walks the primary taskbar's current TaskListButtons and (re)resolves any
// whose cache entry is missing, stale, or past the negative-cache TTL -
// see ResolveAndCacheButtonHwnd for why this runs on a timer instead of
// inline during Arrange.
//
// g_unloading gates this first - once the mod's hooks are gone, a click-
// sentinel probe here reaches the taskbar's real HandleClick with a
// garbage pointer instead of being intercepted.
//
// Reentrancy guard (RAII, since this has several early returns): calling
// into taskbar.dll/WinRT here can pump messages, letting a second posted
// ResolveButtonHwndsMsg reenter this same function and corrupt the
// erase-while-iterating prune loops below.
thread_local bool g_inResolvePendingButtonHwnds;
struct ScopedResolvePendingButtonHwndsFlag {
    ScopedResolvePendingButtonHwndsFlag() {
        g_inResolvePendingButtonHwnds = true;
    }
    ~ScopedResolvePendingButtonHwndsFlag() {
        g_inResolvePendingButtonHwnds = false;
    }
};

void ResolvePendingButtonHwnds() {
    if (g_inResolvePendingButtonHwnds) {
        return;
    }
    ScopedResolvePendingButtonHwndsFlag scopedReentrancyFlag;

    // Computes and arms this function's own next tick on destruction, on
    // every return path, right after this pass's own cache reads/writes
    // are done (doing it from ButtonHwndResolveTimerProc instead would
    // race g_buttonHwndCache). `enumerated` tracks whether the walk below
    // actually completed, since NextResolveDelayMs' INFINITE answer is
    // only trustworthy then - the destructor falls back to the much
    // shorter kEnumerationFailedRetryMs otherwise, so a transient failure
    // (e.g. a taskbar rebuild) recovers quickly rather than leaving every
    // button on its default-side classification for up to 30s.
    bool enumerated = false;
    struct ScheduleNextResolveTick {
        bool* enumerated;
        ~ScheduleNextResolveTick() {
            DWORD delay = *enumerated ? NextResolveDelayMs()
                                      : kEnumerationFailedRetryMs;
            if (delay != INFINITE) {
                ArmButtonHwndResolveTimer(delay);
            }
        }
    } scheduleNextTick{&enumerated};

    if (g_unloading || g_inTaskbarArrangeOverride || !g_hTaskbarWnd) {
        return;
    }

    // Raw Win32 callback boundary (dispatched via TaskbarWndSubclassProc) -
    // an uncaught WinRT exception here fail-fasts explorer.exe.
    try {
        FrameworkElement repeater = GetCachedTaskbarRepeater();
        if (!repeater) {
            return;
        }

        ULONGLONG now = GetTickCount64();
        bool anyChanged = false;
        bool forceResolve = g_forceResolveUnresolved.exchange(false);

        std::unordered_set<void*> liveTaskListButtons;

        for (auto& child : GetRepeaterChildElements(repeater)) {
            if (!IsTaskListButton(child)) {
                continue;
            }
            void* key = winrt::get_abi(child);
            liveTaskListButtons.insert(key);

            std::wstring identity = GetButtonAccessibleName(child);

            auto it = g_buttonHwndCache.find(key);
            bool needsResolve = it == g_buttonHwndCache.end();
            if (needsResolve) {
                // A brand-new button - its mere appearance means
                // g_lastArrangedX has no entry for it yet.
                anyChanged = true;
            }
            if (!needsResolve) {
                // Identity mismatch means ItemsRepeater rebound this element
                // to a different item since we last resolved it - checked
                // on both branches, since a rebind onto a negatively-cached
                // element is just as real.
                if (it->second.hwnd) {
                    // The old HWND is still valid, just no longer this
                    // element's, so IsWindow() alone can't catch a rebind.
                    needsResolve = !IsWindow(it->second.hwnd) ||
                                   identity != it->second.identity;
                } else {
                    // forceResolve: bounded to
                    // consecutiveFailures < kMaxForcedRetryFailures - see
                    // its own comment. backoffElapsed itself stops
                    // retrying past kMaxResolveFailures - see its comment.
                    bool backoffElapsed =
                        it->second.consecutiveFailures < kMaxResolveFailures &&
                        now - it->second.lastAttempt >=
                        ResolveBackoffMs(it->second.consecutiveFailures);
                    needsResolve =
                        identity != it->second.identity || backoffElapsed ||
                        (forceResolve && it->second.consecutiveFailures <
                                             kMaxForcedRetryFailures);
                }
            }

            if (needsResolve && ResolveAndCacheButtonHwnd(child, identity)) {
                anyChanged = true;
            }
        }
        enumerated = true;

        // Prune g_buttonHwndCache entries for buttons that no longer exist -
        // XAML recreates TaskListButtons and the allocator can reuse a
        // destroyed element's address, so a fresh enumeration is the only
        // way to detect that. g_lastArrangedX isn't pruned here - it's
        // rebuilt separately by RecomputeLayoutPlan (see g_planDirty).
        for (auto it = g_buttonHwndCache.begin(); it != g_buttonHwndCache.end();) {
            if (liveTaskListButtons.find(it->first) == liveTaskListButtons.end()) {
                it = g_buttonHwndCache.erase(it);
                anyChanged = true;
            } else {
                it = std::next(it);
            }
        }

        // Same idea for g_lastKnownWindowClassification, keyed by HWND
        // (which Windows also recycles).
        for (auto it = g_lastKnownWindowClassification.begin();
             it != g_lastKnownWindowClassification.end();) {
            it = IsWindow(it->first) ? std::next(it)
                                      : g_lastKnownWindowClassification.erase(it);
        }

        // Rebuild the published resolved-HWND set DragFollowPollTimerProc
        // polls - unconditionally, since it's cheap and needs to reflect
        // this pass's pruning either way.
        {
            std::unordered_set<HWND> resolvedNow;
            for (auto& kv : g_buttonHwndCache) {
                if (kv.second.hwnd) {
                    resolvedNow.insert(kv.second.hwnd);
                }
            }
            std::lock_guard<std::mutex> guard(g_resolvedHwndsMutex);
            g_resolvedHwnds = std::move(resolvedNow);
        }

        // Recomputed every pass for TaskListButton_UpdateVisualStates_Hook
        // (same thread, no marshal needed) to skip nudging the resolve
        // timer at all once there's nothing left it could possibly help -
        // true for any entry that isn't resolved and hasn't given up
        // (notRunning/awaitingFirstClick entries count as "still worth a
        // recheck" here even though NextResolveDelayMs treats them as
        // idle for polling-cadence purposes; those are two different
        // questions).
        g_anyButtonNeedsRecheck = false;
        for (auto& kv : g_buttonHwndCache) {
            if (!kv.second.hwnd &&
                kv.second.consecutiveFailures < kMaxResolveFailures) {
                g_anyButtonNeedsRecheck = true;
                break;
            }
        }

        if (anyChanged) {
            InvalidateTaskbarLayout();
        }
    } catch (...) {
        Wh_Log(L"ResolvePendingButtonHwnds: exception");
    }
}

using IUIElement_Arrange_t =
    HRESULT(WINAPI*)(void* pThis, winrt::Windows::Foundation::Rect rect);
IUIElement_Arrange_t IUIElement_Arrange_Original;

// Every Start/system-button/task-list-button's target X, keyed by the XAML
// element's ABI pointer. Written only by RecomputeLayoutPlan, so
// IUIElement_Arrange_Hook below never computes anything, only looks a
// value up. An element with no entry (secondary-monitor, or too new to be
// in this pass's plan) falls through to native positioning.
std::unordered_map<void*, double> g_lastArrangedX;

// Each task list button's own FullFootprintWidth() as of the same pass
// that computed g_lastArrangedX, keyed the same way. RecomputeLayoutPlan's
// cheap staleness check uses this to catch a button whose width changed
// without its count changing (e.g. a label populating shortly after a
// freshly-launched button first appears icon-only).
std::unordered_map<void*, double> g_lastArrangedTaskListWidth;

// Set whenever something might have changed that g_lastArrangedX doesn't
// reflect yet, at the point that change becomes visible on the taskbar
// thread. Starts true so the first pass always computes a real plan.
// RecomputeLayoutPlan clears it only after a genuinely successful
// recompute - left true on an exception so a later pass retries. Skips
// RecomputeLayoutPlan's full traversal on passes where nothing changed.
std::atomic<bool> g_planDirty{true};

// Staleness backstop for g_planDirty: some real triggers aren't
// guaranteed to call InvalidateTaskbarLayout. RecomputeLayoutPlan forces
// a real recompute at least this often regardless, bounding how long the
// plan can disagree with reality.
constexpr ULONGLONG kMaxPlanStalenessMs = 500;
ULONGLONG g_lastPlanRecomputeTick;

// Diagnostics only, for IUIElement_Arrange_Hook's own per-call work
// (distinct from LayoutPlanStats, which covers RecomputeLayoutPlan's
// once-per-pass traversal).
struct ArrangePassStats {
    int totalArrangeCalls = 0;
    int repositioned = 0;
    int qiFailures = 0;
    int exceptions = 0;
};
// thread_local: this hook runs once per taskbar instance's XAML tree,
// process-wide - in practice all on the same Explorer UI thread, but this
// stays correct even if that ever changed.
thread_local ArrangePassStats g_passStats;

HRESULT WINAPI IUIElement_Arrange_Hook(void* pThis,
                                       winrt::Windows::Foundation::Rect rect) {
    auto original = [=] { return IUIElement_Arrange_Original(pThis, rect); };

    if (!g_inTaskbarArrangeOverride || g_unloading || !g_hTaskbarWnd) {
        return original();
    }

    // Raw ABI boundary (process-wide vtable slot) - an uncaught throw
    // here fail-fasts the whole process.
    try {
        g_passStats.totalArrangeCalls++;

        FrameworkElement element = nullptr;
        ((IUnknown*)pThis)
            ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                             winrt::put_abi(element));
        if (!element) {
            g_passStats.qiFailures++;
            return original();
        }

        auto it = g_lastArrangedX.find(winrt::get_abi(element));
        if (it == g_lastArrangedX.end()) {
            return original();
        }

        g_passStats.repositioned++;
        winrt::Windows::Foundation::Rect newRect = rect;
        newRect.X = it->second;
        return IUIElement_Arrange_Original(pThis, newRect);
    } catch (...) {
        g_passStats.exceptions++;
        return original();
    }
}

// Builds the complete layout plan in a single top-down pass over the
// PRIMARY taskbar's own repeater, written into g_lastArrangedX - called
// BEFORE XAML's own ArrangeOverride so no nested Arrange call ever sees a
// partial plan. Rebuilds from scratch on every genuine recompute (bounded
// to kMaxPlanStalenessMs, not immediate - see g_planDirty).
//
// MUST only run confirmed on g_hTaskbarWnd's own thread - an unsafe
// unmarshaled cross-apartment call otherwise. Shell_TrayWnd and
// Shell_SecondaryTrayWnd share one thread, so this also passes for
// secondary-monitor passes (harmless, since those elements never end up
// in g_lastArrangedX anyway).
void RecomputeLayoutPlan() {
    // Snapshot once - see g_hTaskbarWnd's own comment.
    HWND hTaskbarWnd = g_hTaskbarWnd;
    if (!hTaskbarWnd) {
        return;
    }
    DWORD primaryThreadId = GetWindowThreadProcessId(hTaskbarWnd, nullptr);
    if (primaryThreadId == 0 || primaryThreadId != GetCurrentThreadId()) {
        return;
    }
    if (!g_planDirty &&
        GetTickCount64() - g_lastPlanRecomputeTick < kMaxPlanStalenessMs) {
        // Cheap staleness check: if any live task list button isn't in
        // g_lastArrangedX, an already-covered one's own width has since
        // changed, or the live child count doesn't match the last
        // recompute's, the plan is stale regardless of the dirty flag.
        // Hash lookup first; IsTaskListButton (a real class-name lookup)
        // only runs on that lookup's miss, so a non-task-list child never
        // pays for it. FullFootprintWidth is NOT similarly avoided for an
        // already-covered task list button specifically - the && short-
        // circuit only guards the g_lastArrangedX miss case above, not
        // this one, so it runs unconditionally for every task list button
        // on every cheap-path pass, two property reads each. Left as-is
        // rather than added complexity to dodge it: both reads are cheap,
        // and this is exactly the button this whole check exists to catch
        // a width change on.
        bool planIsCurrent = true;
        try {
            if (FrameworkElement repeater = GetCachedTaskbarRepeater()) {
                int liveChildCount = 0;
                for (auto& child : GetRepeaterChildElements(repeater)) {
                    liveChildCount++;
                    void* key = winrt::get_abi(child);
                    if (!g_lastArrangedX.count(key)) {
                        if (IsTaskListButton(child)) {
                            // A live task list button the plan doesn't
                            // cover yet. Scoped to task list buttons since
                            // a brand-new system button can't reach this
                            // branch.
                            planIsCurrent = false;
                            break;
                        }
                        continue;
                    }
                    // Already covered - but for a task list button
                    // specifically, its own width can still have changed
                    // since the plan was computed without the button
                    // count changing at all (e.g. a freshly-launched
                    // button first renders icon-only, then grows once its
                    // label populates a moment later) - that wouldn't
                    // otherwise mark the plan dirty, leaving a stale,
                    // too-narrow X in effect until something unrelated
                    // (a drag, a count change) forces a real recompute.
                    // g_lastArrangedTaskListWidth only has entries for
                    // task list buttons, so this naturally no-ops for
                    // Start/Search/TaskView/Widgets.
                    auto widthIt = g_lastArrangedTaskListWidth.find(key);
                    if (widthIt != g_lastArrangedTaskListWidth.end() &&
                        std::abs(FullFootprintWidth(child) - widthIt->second) >
                            0.5) {
                        planIsCurrent = false;
                        break;
                    }
                }
                // A child *disappearing* adds nothing new to
                // g_lastArrangedX's coverage, so the loop above alone
                // can't catch it - this count comparison against
                // g_planChildCount does, off the same walk.
                if (planIsCurrent && liveChildCount != g_planChildCount) {
                    planIsCurrent = false;
                }
            }
        } catch (...) {
            // Conservative default: fall through to the real recompute.
            planIsCurrent = false;
        }
        if (planIsCurrent) {
            return;
        }
    }
    g_lastPlanRecomputeTick = GetTickCount64();

    g_planStats = {};

    try {
        FrameworkElement repeater = GetCachedTaskbarRepeater();
        if (!repeater) {
            return;
        }
        FrameworkElement content =
            repeater.XamlRoot().Content().try_as<FrameworkElement>();
        if (!content) {
            return;
        }

        auto children = GetRepeaterChildElements(repeater);
        double startCenterX = GetMonitorCenterXLocal();
        std::unordered_map<void*, double> newPlan;
        std::unordered_map<void*, double> newTaskListWidths;

        // Classify each child's SystemButton status exactly once - see
        // ChildInfo's comment for why.
        std::vector<ChildInfo> childInfos;
        childInfos.reserve(children.size());
        for (auto& child : children) {
            childInfos.push_back({child, IdentifySystemButton(child)});
        }

        // Prune g_lastSystemButtonContentWidth down to this pass's live
        // system buttons - the map is keyed by raw ABI pointer with no
        // reference held, so a destroyed element's key can be reused by an
        // unrelated new element at a taskbar/XamlRoot recreate, which would
        // otherwise silently inherit the destroyed one's stale width.
        {
            std::unordered_set<void*> liveSystemButtonKeys;
            for (auto& info : childInfos) {
                if (info.systemButton != SystemButton::None &&
                    info.systemButton != SystemButton::Start) {
                    liveSystemButtonKeys.insert(winrt::get_abi(info.element));
                }
            }
            for (auto it = g_lastSystemButtonContentWidth.begin();
                 it != g_lastSystemButtonContentWidth.end();) {
                it = liveSystemButtonKeys.count(it->first)
                         ? std::next(it)
                         : g_lastSystemButtonContentWidth.erase(it);
            }
        }

        // Start first: ComputeSystemButtonX/PlanTaskListButtons below
        // both read g_lastStartWidth, so it must reflect this pass.
        for (auto& info : childInfos) {
            if (info.systemButton == SystemButton::Start) {
                // Bare ActualWidth(), NOT SystemButtonContentWidth: Start
                // is never hidden/shown the way Search/Task View/Widgets
                // are, so it was never exposed to the collapse-margin
                // problem that fix exists for, and swapping to the content
                // child's DesiredSize() instead would understate Start's
                // true width - its own visual-tree shape doesn't match
                // what that technique was validated against. Only updates
                // g_lastStartWidth when positive, so a freshly (re)created
                // Start keeps the last known-good width instead of
                // collapsing around a zero-width Start.
                double w = info.element.ActualWidth();
                if (w > 0) {
                    g_lastStartWidth = w;
                }
                // Subtract Margin.Left since XAML insets rendered content
                // by it - otherwise Start centers off by that amount.
                newPlan[winrt::get_abi(info.element)] =
                    startCenterX - info.element.Margin().Left -
                    g_lastStartWidth / 2.0;
                break;
            }
        }

        // Search/TaskView/Widgets next: PlanTaskListButtons reads
        // g_last{Left,Right}SystemClusterWidth, set unconditionally here
        // so an empty cluster reads as genuinely 0.
        double systemClusterWidth = 0;
        for (auto& info : childInfos) {
            if (info.systemButton != SystemButton::None &&
                info.systemButton != SystemButton::Start) {
                systemClusterWidth += SystemButtonFootprintWidth(info.element);
            }
        }
        bool systemButtonsAdjacent = g_settings.systemButtonsPlacement ==
                                     SystemButtonsPlacement::AdjacentStart;
        g_lastLeftSystemClusterWidth =
            (systemButtonsAdjacent &&
             g_settings.systemButtonsAdjacentSide == Side::Left)
                ? systemClusterWidth
                : 0;
        g_lastRightSystemClusterWidth =
            (systemButtonsAdjacent &&
             g_settings.systemButtonsAdjacentSide == Side::Right)
                ? systemClusterWidth
                : 0;

        for (auto& info : childInfos) {
            if (info.systemButton == SystemButton::None ||
                info.systemButton == SystemButton::Start) {
                continue;
            }
            newPlan[winrt::get_abi(info.element)] = ComputeSystemButtonX(
                childInfos, info.element, startCenterX,
                g_lastStartWidth, systemClusterWidth);
        }

        // Bounds each task list group compresses to fit within.
        double leftBoundLocal =
            (g_settings.systemButtonsPlacement == SystemButtonsPlacement::FarLeft)
                ? (kFarLeftSystemButtonMarginPx + systemClusterWidth)
                : 0;

        // SystemTray.SystemTrayFrame's own left edge is a real bound,
        // unlike the taskbar's outer edge (the tray only occupies the
        // last portion of the taskbar's width). Falls back to the
        // taskbar's own width if it can't be resolved this pass.
        FrameworkElement systemTrayFrame =
            FindChildByClassName(content, L"SystemTray.SystemTrayFrame");
        double rightBoundLocal =
            systemTrayFrame
                ? (GetElementLeftXLocal(systemTrayFrame, content) -
                   kTrayMarginPx)
                : GetTaskbarWidthLocal();

        // Resolved once here and threaded down rather than per button -
        // every task list button's classification needs the same primary
        // monitor's rect. See ClassifyByWindowPositionCached.
        MONITORINFO monitorInfo{.cbSize = sizeof(monitorInfo)};
        bool monitorInfoValid = GetMonitorInfo(
            MonitorFromWindow(hTaskbarWnd, MONITOR_DEFAULTTOPRIMARY),
            &monitorInfo);

        // Task list buttons last - see PlanTaskListButtons for the
        // single-O(n)-pass reasoning.
        PlanTaskListButtons(children, startCenterX, leftBoundLocal,
                            rightBoundLocal,
                            monitorInfoValid ? &monitorInfo : nullptr, newPlan,
                            newTaskListWidths);

        g_lastArrangedX = std::move(newPlan);
        g_lastArrangedTaskListWidth = std::move(newTaskListWidths);
        g_planChildCount = (int)children.size();
        g_planDirty = false;
    } catch (...) {
        g_planStats.exceptions++;
        // g_lastArrangedX is left as whatever the last successful pass
        // produced. g_planDirty deliberately stays true here (unlike the
        // success path) so a later pass retries instead of treating this
        // as up to date.
    }
}

using TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_t =
    HRESULT(WINAPI*)(void* pThis,
                     void* context,
                     winrt::Windows::Foundation::Size size,
                     winrt::Windows::Foundation::Size* resultSize);
TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_t
    TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Original;
HRESULT WINAPI TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Hook(
    void* pThis,
    void* context,
    winrt::Windows::Foundation::Size size,
    winrt::Windows::Foundation::Size* resultSize) {
    [[maybe_unused]] static bool hooked = [] {
        Shapes::Rectangle rectangle;
        IUIElement element = rectangle;

        void** vtable = *(void***)winrt::get_abi(element);
        auto arrange = (IUIElement_Arrange_t)vtable[92];

        WindhawkUtils::SetFunctionHook(arrange, IUIElement_Arrange_Hook,
                                       &IUIElement_Arrange_Original);
        Wh_ApplyHookOperations();
        return true;
    }();

    static bool loggedFirstCall;
    if (!loggedFirstCall) {
        loggedFirstCall = true;
        Wh_Log(L"ArrangeOverride hook is firing (first call)");
    }

    g_passStats = {};

    EnsureTaskbarWnd();

    // Builds this pass's whole plan up front - see RecomputeLayoutPlan.
    RecomputeLayoutPlan();

    // The button count can change without any window moving. Self-correct
    // by invalidating whenever the observed count changes, and arm an
    // immediate HWND-resolve attempt for a newly-inserted button.
    static thread_local int lastPlanTaskListCount = -1;
    int currentTaskListCount = g_planStats.taskListTotal;
    if (currentTaskListCount != lastPlanTaskListCount) {
        bool countChanged = lastPlanTaskListCount != -1;
        lastPlanTaskListCount = currentTaskListCount;
        if (countChanged) {
            InvalidateTaskbarLayout();
            g_forceResolveUnresolved = true;
            ArmButtonHwndResolveTimer(0);
        }
    }

    // RAII: a non-local exit from the original call would otherwise leave
    // this stuck true.
    struct ScopedArrangeOverrideFlag {
        ScopedArrangeOverrideFlag() { g_inTaskbarArrangeOverride = true; }
        ~ScopedArrangeOverrideFlag() { g_inTaskbarArrangeOverride = false; }
    };

    HRESULT ret;
    {
        ScopedArrangeOverrideFlag scopedFlag;
        ret = TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Original(
            pThis, context, size, resultSize);
    }

    static ULONGLONG lastStatsLog;
    ULONGLONG now = GetTickCount64();
    if (now - lastStatsLog > 2000) {
        lastStatsLog = now;
        Wh_Log(
            L"Arrange pass: arrangeCalls=%d repositioned=%d qiFail=%d "
            L"exceptions=%d | plan: taskList=%d (hwndResolved=%d left=%d "
            L"right=%d) planExceptions=%d | dragFollow: polls=%d "
            L"invalidated=%d skippedReentrant=%d invalidateExceptions=%d | "
            L"resolve: ok=%d fail=%d",
            g_passStats.totalArrangeCalls, g_passStats.repositioned,
            g_passStats.qiFailures, g_passStats.exceptions,
            g_planStats.taskListTotal, g_planStats.taskListHwndResolved,
            g_planStats.taskListLeft, g_planStats.taskListRight,
            g_planStats.exceptions,
            (int)g_dragFollowPollCount,
            (int)g_dragFollowInvalidateCount, (int)g_invalidateSkippedReentrant,
            (int)g_invalidateExceptions, g_resolveStats.success,
            g_resolveStats.failure);
        g_resolveStats = {};
    }

    return ret;
}

// ============================================================================
// Live drag-follow: force a taskbar relayout when a top-level window moves
// ============================================================================

// Guards against reentering this body on the taskbar's own thread - a
// nested WinRT/taskbar.dll call inside it can pump messages and let a
// second posted invalidate land reentrantly before the first returns. This
// body also never calls UpdateLayout() itself - see InvalidateTaskbarLayout
// below for why a forced synchronous layout pass here is unsafe regardless
// of this guard.
thread_local bool g_inInvalidateTaskbarLayout;

// MUST only run on g_hTaskbarWnd's own thread - same constraint as
// GetCachedTaskbarRepeater, which this uses.
void PerformTaskbarLayoutInvalidate() {
    if (g_inInvalidateTaskbarLayout) {
        g_invalidateSkippedReentrant++;
        return;
    }
    g_inInvalidateTaskbarLayout = true;

    // Set here (not in InvalidateTaskbarLayout) - dirty has to be marked
    // at the point the change becomes visible on the taskbar thread.
    g_planDirty = true;

    try {
        FrameworkElement repeater = GetCachedTaskbarRepeater();
        if (!repeater) {
            Wh_Log(L"InvalidateTaskbarLayout: GetCachedTaskbarRepeater failed");
        } else {
            repeater.InvalidateArrange();
            repeater.InvalidateMeasure();
        }
    } catch (...) {
        g_invalidateExceptions++;
    }

    g_inInvalidateTaskbarLayout = false;
}

// Private message InvalidateTaskbarLayout posts to run
// PerformTaskbarLayoutInvalidate on the taskbar's own thread without
// blocking the caller. Function-local static so RegisterWindowMessage
// runs lazily rather than from a dynamic initializer under DllMain's
// loader lock.
UINT InvalidateTaskbarLayoutMsg() {
    static const UINT msg =
        RegisterWindowMessage(L"Windhawk_InvalidateTaskbarLayout_" WH_MOD_ID);
    return msg;
}

// Same idea as InvalidateTaskbarLayoutMsg, for ButtonHwndResolveTimerProc
// (can arrive in a burst while TaskListButton::UpdateVisualStates fires
// repeatedly, e.g. several buttons changing state at once).
UINT ResolveButtonHwndsMsg() {
    static const UINT msg =
        RegisterWindowMessage(L"Windhawk_ResolveButtonHwnds_" WH_MOD_ID);
    return msg;
}

// Same idea again, for a settings change. Carries a payload: lParam is a
// heap-allocated ModSettings* the dispatch case below owns and deletes.
// See ApplyLoadedSettings.
UINT SettingsChangedMsg() {
    static const UINT msg =
        RegisterWindowMessage(L"Windhawk_SettingsChanged_" WH_MOD_ID);
    return msg;
}

// Sent (not posted) by Wh_ModBeforeUninit right before removing the
// subclass, to reclaim a SettingsChangedMsg that was posted but never got
// dispatched (e.g. settings changed immediately before disable) - without
// this, that message would fall through to Shell_TrayWnd's real WndProc
// once the subclass is gone, and its heap ModSettings would leak.
UINT DrainSettingsMsg() {
    static const UINT msg =
        RegisterWindowMessage(L"Windhawk_DrainSettings_" WH_MOD_ID);
    return msg;
}

// Installed on g_hTaskbarWnd by EnsureTaskbarWnd. A subclass proc only
// ever runs on the thread that owns the window, which is what lets
// InvalidateTaskbarLayout/ResolvePendingButtonHwnds/ApplyLoadedSettings
// use a plain, non-blocking PostMessage - no per-call SetWindowsHookEx/
// SendMessage/UnhookWindowsHookEx dance needed, unlike RunFromWindowThread,
// an earlier fallback design that did exactly that dance (a blocking
// WH_CALLWNDPROC hook) for when the subclass install failed; it was
// removed entirely since that failure is rare, the fallback added real
// complexity for little benefit, and EnsureTaskbarWnd already retries the
// subclass install on every call rather than just once. This is the SOLE
// way any of the four private messages below reach the taskbar thread,
// with no fallback if the subclass never installs. Everything else is
// forwarded to
// DefSubclassProc, which also lets comctl32 clean this subclass up via
// WM_NCDESTROY if the window is destroyed before Wh_ModBeforeUninit
// removes it.
LRESULT CALLBACK TaskbarWndSubclassProc(HWND hWnd,
                                        UINT uMsg,
                                        WPARAM wParam,
                                        LPARAM lParam,
                                        DWORD_PTR dwRefData) {
    if (uMsg == InvalidateTaskbarLayoutMsg()) {
        PerformTaskbarLayoutInvalidate();
        return 0;
    }
    if (uMsg == ResolveButtonHwndsMsg()) {
        ResolvePendingButtonHwnds();
        return 0;
    }
    if (uMsg == DrainSettingsMsg()) {
        // Runs on this thread (SendMessage from Wh_ModBeforeUninit), so
        // PeekMessage here scans this thread's own queue - the one
        // SettingsChangedMsg was posted to.
        MSG queuedMsg;
        while (PeekMessage(&queuedMsg, hWnd, SettingsChangedMsg(),
                           SettingsChangedMsg(), PM_REMOVE)) {
            delete reinterpret_cast<ModSettings*>(queuedMsg.lParam);
        }
        return 0;
    }
    if (uMsg == SettingsChangedMsg()) {
        std::unique_ptr<ModSettings> heapSettings(
            reinterpret_cast<ModSettings*>(lParam));
        bool wasTracking = g_settings.trackWindowPositions;
        g_settings = std::move(*heapSettings);
        // Marks the previous plan stale now that the settings it was built
        // from just changed - has to happen exactly here, not by the
        // separate InvalidateTaskbarLayout() call Wh_ModSettingsChanged
        // also makes, since that one can't see the settings swap above.
        g_planDirty = true;
        // Starts/stops the background worker thread live on a trackWindowPositions
        // change, rather than only at the next mod reload - StopBackgroundWorkerThreadForToggle,
        // not StopBackgroundWorkerThread, since the latter's own comment explains why
        // that one would permanently brick this toggle.
        if (g_settings.trackWindowPositions != wasTracking) {
            if (g_settings.trackWindowPositions) {
                StartBackgroundWorkerThread();
            } else {
                StopBackgroundWorkerThreadForToggle();
                // Without this, an already-resolved button keeps using its
                // last-known (now frozen) window position instead of
                // immediately falling back to leftApps/rightApps/default -
                // GetButtonHwnd would otherwise keep finding a stale entry
                // here. Safe to clear directly: every other access to this
                // cache also runs on this same taskbar thread.
                g_buttonHwndCache.clear();
            }
        }
        return 0;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Schedules a relayout on the taskbar's own thread via PostMessage rather
// than forcing a synchronous UpdateLayout() call. MUST stay async: a forced
// UpdateLayout() from a nested callback (e.g. DragFollowPollTimerProc)
// reenters WinUI layout and raises STATUS_STOWED_EXCEPTION, a raw SEH
// exception no try/catch in this file can contain. Does not set
// g_planDirty itself, since this marshal is asynchronous - a natural
// ArrangeOverride pass could land on the taskbar thread between this call
// setting the flag and the posted message being processed, see it already
// true, recompute with stale state, and clear the flag, leaving the
// posted invalidate with nothing left to do. Each real caller marks dirty
// at its own point instead, at the moment its own state change becomes
// visible on the taskbar thread.
void InvalidateTaskbarLayout() {
    // Snapshot so a concurrent EnsureTaskbarWnd write can't change this
    // mid-function.
    HWND hTaskbarWnd = g_hTaskbarWnd;
    // No fallback if the subclass never installed - see
    // g_taskbarWndSubclassed. Hot path (up to ~7/sec during a drag), so
    // PostMessage's non-blocking behavior matters here.
    if (!hTaskbarWnd || !g_taskbarWndSubclassed) {
        return;
    }
    if (!PostMessage(hTaskbarWnd, InvalidateTaskbarLayoutMsg(), 0, 0)) {
        Wh_Log(L"InvalidateTaskbarLayout: PostMessage failed, error=%lu",
               GetLastError());
    }
}

// Polls g_resolvedHwnds directly via GetWindowRect on a fixed interval,
// rather than listening for a system-wide EVENT_OBJECT_LOCATIONCHANGE
// WinEventHook. That hook used idProcess=0 - every process on the desktop
// - for one of the highest-volume WinEvents there is (cursor, caret,
// every window move/size/scroll), so simply having it registered made
// every unrelated process on the system generate and marshal a
// notification this mod discarded almost all of anyway (thousands of raw
// events/sec observed, per this file's own former diagnostics).
// g_resolvedHwnds is typically a handful of windows, and GetWindowRect is
// a direct win32k.sys read rather than a cross-process message send, so
// polling it here moves the entire cost onto this mod's own dedicated
// thread instead of externalizing it onto every other process. Only ever
// touched from that thread.
std::unordered_map<HWND, LONG> g_lastPolledCenterX;

// Poll cadence, switched between adaptively - see the tail of
// DragFollowPollTimerProc. Fast enough during an actual drag to keep
// icons following live, slow enough at rest that an idle desktop isn't
// paying for a sub-second timer in the shell process.
constexpr UINT kDragFollowPollActiveMs = 150;
constexpr UINT kDragFollowPollIdleMs = 1000;
// How long the fast cadence persists after the last detected change.
// Bridges short gaps - a brief pause mid-drag, or the moment between two
// drags of the same window - so those don't drop to the idle rate and
// straight back. A pause longer than this does fall back to idle, which
// costs up to one idle interval before the fast rate resumes.
constexpr ULONGLONG kDragFollowActiveWindowMs = 2000;

// Both worker-thread-exclusive, like g_lastPolledCenterX itself.
UINT g_dragFollowPollCurrentMs;
ULONGLONG g_lastDragFollowChangeTick;

// Defined just below DragFollowPollTimerProc, which calls it.
void SetDragFollowPollInterval(UINT intervalMs);

void CALLBACK DragFollowPollTimerProc(HWND hwnd,
                                      UINT uMsg,
                                      UINT_PTR idEvent,
                                      DWORD dwTime) {
    if (g_unloading) {
        return;
    }

    g_dragFollowPollCount++;

    std::unordered_set<HWND> tracked;
    {
        std::lock_guard<std::mutex> guard(g_resolvedHwndsMutex);
        tracked = g_resolvedHwnds;
    }

    // Drop entries for windows no longer tracked - keeps this bounded to
    // the actual live set rather than accumulating forever as buttons
    // resolve/unresolve across a session. Windows also recycles HWNDs, so
    // this also prevents a stale value from a closed window being compared
    // against a new, unrelated window that happens to reuse the handle.
    for (auto it = g_lastPolledCenterX.begin();
         it != g_lastPolledCenterX.end();) {
        it = tracked.count(it->first) ? std::next(it)
                                      : g_lastPolledCenterX.erase(it);
    }

    // Only a window's horizontal center can affect this mod's layout:
    // ClassifyByWindowPositionCached derives BOTH the side (center vs. the
    // screen's center line) and distanceFromCenter (which orders same-side
    // icons) from that one number and nothing else. So a vertical-only
    // move, or a resize that leaves the center where it was, provably
    // can't change any icon's placement - comparing whole rects here would
    // fire a relayout for those anyway, on every tick of such a drag.
    // Stored as (left + right), i.e. twice the true center: only equality
    // matters, and staying integral keeps the comparison exact.
    //
    // A minimized window is skipped outright - GetWindowRect reports a
    // nonsense off-screen position for one, and
    // ClassifyByWindowPositionCached deliberately freezes at its last
    // pre-minimize classification rather than using it, so polling a
    // minimized window could only ever produce relayouts that change
    // nothing (on minimize, and again on restore).
    //
    // A brand-new entry (nothing to compare against yet) is deliberately
    // not treated as a change: ResolveAndCacheButtonHwnd already
    // invalidates when a button first resolves to an HWND (see
    // ResolvePendingButtonHwnds' own anyChanged), so counting it here
    // would just be a redundant second invalidate for the same event.
    bool anyChanged = false;
    for (HWND trackedHwnd : tracked) {
        if (IsIconic(trackedHwnd)) {
            continue;
        }
        RECT rect;
        if (!GetWindowRect(trackedHwnd, &rect)) {
            continue;
        }
        LONG centerX = rect.left + rect.right;
        auto it = g_lastPolledCenterX.find(trackedHwnd);
        if (it == g_lastPolledCenterX.end()) {
            g_lastPolledCenterX[trackedHwnd] = centerX;
            continue;
        }
        if (it->second != centerX) {
            it->second = centerX;
            anyChanged = true;
        }
    }

    ULONGLONG now = GetTickCount64();
    if (anyChanged) {
        g_dragFollowInvalidateCount++;
        g_lastDragFollowChangeTick = now;
        InvalidateTaskbarLayout();
    }

    // Adaptive cadence: poll fast only while something is actually
    // moving. A drag produces a change on nearly every tick, so this
    // holds the fast rate for the whole drag plus
    // kDragFollowActiveWindowMs after it settles, then drops to the idle
    // rate - which is where an ordinary desktop sits essentially all of
    // the time. A fixed fast rate would instead keep explorer.exe waking
    // ~7x/second for the entire time the mod is enabled, including with
    // nothing tracked at all, with the click-sentinel path latched off,
    // and with the user away from the machine; a constant sub-second
    // timer in the shell process is a real cost to impose for that.
    // Nothing is missed at the idle rate - it still does the same
    // GetWindowRect comparison per tracked window, so a new drag is
    // picked up within one idle interval and snaps straight back to the
    // fast rate for the rest of it.
    UINT wanted = (now - g_lastDragFollowChangeTick < kDragFollowActiveWindowMs)
                      ? kDragFollowPollActiveMs
                      : kDragFollowPollIdleMs;
    if (wanted != g_dragFollowPollCurrentMs) {
        SetDragFollowPollInterval(wanted);
    }
}

// Switches the drag-follow poll to a new interval. Only ever called from
// the dedicated worker thread, which owns this timer - from
// BackgroundWorkerThreadProc at startup, and from DragFollowPollTimerProc
// itself (killing and re-arming a thread-owned timer from inside its own
// callback is fine, and ButtonHwndResolveTimerProc already does the same).
// A thread-owned SetTimer mints a fresh id, so g_dragFollowPollTimerId has
// to be updated alongside or teardown would kill the wrong one.
void SetDragFollowPollInterval(UINT intervalMs) {
    // Arm the replacement BEFORE killing the old one. Killing first and
    // then failing to re-arm would leave no timer at all, and since this
    // function's only repeat caller is the poll callback itself, nothing
    // would ever run to try again - drag-follow would be silently and
    // permanently dead for the session. Keeping the old timer on failure
    // means the poll keeps running at the previous cadence and simply
    // retries this switch on its next tick. The two timers overlap for
    // the instant between these calls, which at worst costs one extra
    // poll pass.
    UINT_PTR newTimerId =
        SetTimer(nullptr, 0, intervalMs, DragFollowPollTimerProc);
    if (!newTimerId) {
        Wh_Log(L"SetDragFollowPollInterval: SetTimer failed, error=%lu - "
               L"keeping the current %u ms cadence",
               GetLastError(), g_dragFollowPollCurrentMs);
        return;
    }

    if (g_dragFollowPollTimerId) {
        KillTimer(nullptr, g_dragFollowPollTimerId);
    }
    g_dragFollowPollTimerId = newTimerId;
    g_dragFollowPollCurrentMs = intervalMs;
}

// ============================================================================
// Module/symbol hooking plumbing
// ============================================================================

bool HookTaskbarDllSymbols() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
            &CTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
            &TaskbarHost_FrameHeight_Original,
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            &std__Ref_count_base__Decref_Original,
        },
        {
            // Optional, like everything below: these four only power the
            // button->HWND resolution chain (icons following windows), not
            // the mod's core centering/split positioning - a future Windows
            // build renaming one of them should degrade to
            // unresolvedAppsDefaultSide classification, not refuse to load
            // the whole mod. The null checks already present in
            // GetWindowFromNativeTaskItem/ResolveHwndFromIndividualTaskItem
            // exist specifically to handle that.
            {LR"(public: virtual long __cdecl CTaskListWnd::HandleClick(struct ITaskGroup *,struct ITaskItem *,struct winrt::Windows::System::LauncherOptions const &))"},
            &CTaskListWnd_HandleClick_Original,
            CTaskListWnd_HandleClick_Hook,
            true,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CWindowTaskItem::GetWindow(void))"},
            &CWindowTaskItem_GetWindow_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CImmersiveTaskItem::GetWindow(void))"},
            &CImmersiveTaskItem_GetWindow_Original,
            nullptr,
            true,
        },
        {
            {LR"(const CImmersiveTaskItem::`vftable'{for `ITaskItem'})"},
            &CImmersiveTaskItem_vftable,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::WindowsUdk::UI::Shell::implementation::TaskItem,struct winrt::WindowsUdk::UI::Shell::ITaskItem>::ReportClicked(void *))"},
            &TaskItem_ReportClicked_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual int __cdecl CTaskGroup::GetNumItems(void))"},
            &CTaskGroup_GetNumItems_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::WindowsUdk::UI::Shell::implementation::TaskGroup,struct winrt::WindowsUdk::UI::Shell::ITaskGroup>::ReportClicked(void *))"},
            &TaskGroup_ReportClicked_Original,
            nullptr,
            true,
        },
    };

    bool ok = HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
    Wh_Log(L"HookTaskbarDllSymbols: %s", ok ? L"OK" : L"FAILED");
    // Logged unconditionally rather than gated on ok: HookSymbols' return
    // value only reflects the four required symbols above, so a missing
    // optional symbol never affects it - gating this log on failure would
    // mean it can never fire in the one scenario it exists for (an
    // optional symbol quietly disappearing after a Windows update while
    // every required symbol still resolves fine).
    bool anyOptionalMissing = !CTaskListWnd_HandleClick_Original ||
                              !CWindowTaskItem_GetWindow_Original ||
                              !CImmersiveTaskItem_GetWindow_Original ||
                              !CImmersiveTaskItem_vftable ||
                              !TaskItem_ReportClicked_Original ||
                              !CTaskGroup_GetNumItems_Original ||
                              !TaskGroup_ReportClicked_Original;
    if (anyOptionalMissing) {
        Wh_Log(L"  Missing (optional, click-sentinel chain only): "
               L"%s%s%s%s%s%s%s",
               CTaskListWnd_HandleClick_Original ? L"" : L"CTaskListWnd::HandleClick ",
               CWindowTaskItem_GetWindow_Original ? L"" : L"CWindowTaskItem::GetWindow ",
               CImmersiveTaskItem_GetWindow_Original
                   ? L""
                   : L"CImmersiveTaskItem::GetWindow ",
               CImmersiveTaskItem_vftable ? L"" : L"CImmersiveTaskItem vftable ",
               TaskItem_ReportClicked_Original ? L"" : L"TaskItem::ReportClicked ",
               CTaskGroup_GetNumItems_Original ? L"" : L"CTaskGroup::GetNumItems ",
               TaskGroup_ReportClicked_Original ? L"" : L"TaskGroup::ReportClicked ");
    }
    return ok;
}

using TaskListButton_UpdateVisualStates_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateVisualStates_t TaskListButton_UpdateVisualStates_Original;

// Fires on every visual-state transition of a taskbar button, including a
// pinned app's running/not-running change - the fast "something changed,
// go re-check" nudge, scoped to this taskbar's own buttons (it replaced a
// desktop-wide EVENT_OBJECT_SHOW WinEventHook). Optional: if a future
// Windows build renames it, resolution falls back to ResolveBackoffMs'
// capped-backoff schedule with no fast path.
//
// Re-arms unconditionally rather than gating on elapsed time. This also
// fires for hover, press, focus and badge transitions, so a leading-edge
// throttle could silently drop the one call that mattered when it landed
// behind an irrelevant one, with nothing left to re-arm. Arming instead
// gives a lossless trailing-edge debounce for free, since
// ArmButtonHwndResolveTimer's own KillTimer/SetTimer resets the pending
// countdown: a burst collapses into one pass 150ms after it goes quiet.
// g_anyButtonNeedsRecheck skips the nudge when nothing could benefit,
// though one pinned-but-not-running button keeps it true indefinitely -
// so on a typical taskbar the skip rarely fires, and the pass it would
// have avoided is cheap anyway.
void WINAPI TaskListButton_UpdateVisualStates_Hook(void* pThis) {
    TaskListButton_UpdateVisualStates_Original(pThis);

    if (!g_anyButtonNeedsRecheck) {
        return;
    }
    g_forceResolveUnresolved = true;
    ArmButtonHwndResolveTimer(150);
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Which of these two is actually loaded depends on the Windows build -
    // see GetTaskbarViewModuleHandle. Most entries are optional so a single
    // missing symbol doesn't abort the whole batch - see HookSymbols'
    // optional-parameter doc comment.
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            // Not optional, unlike every entry below: this hook is the
            // mod's entire function. A build where it's missing must
            // surface as a real failure, not a silently-inert mod.
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarCollapsibleLayout,struct winrt::Microsoft::UI::Xaml::Controls::IVirtualizingLayoutOverrides>::ArrangeOverride(void *,struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))"},
            &TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Original,
            TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Hook,
        },
        {
            {LR"(struct winrt::Taskbar::TaskListWindowViewModel __cdecl TryGetItemFromContainer<struct winrt::Taskbar::TaskListWindowViewModel>(struct winrt::Windows::UI::Xaml::UIElement const &))"},
            &TryGetItemFromContainer_TaskListWindowViewModel_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListWindowViewModel,struct winrt::Taskbar::ITaskListWindowViewModel>::get_TaskItem(void * *))"},
            &TaskListWindowViewModel_get_TaskItem_Original,
            nullptr,
            true,
        },
        {
            {LR"(struct winrt::Taskbar::TaskListGroupViewModel __cdecl TryGetItemFromContainer<struct winrt::Taskbar::TaskListGroupViewModel>(struct winrt::Windows::UI::Xaml::UIElement const &))"},
            &TryGetItemFromContainer_TaskListGroupViewModel_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: bool __cdecl winrt::Taskbar::implementation::TaskListGroupViewModel::IsMultiWindow(void)const )"},
            &TaskListGroupViewModel_IsMultiWindow_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: __cdecl winrt::impl::consume_WindowsUdk_UI_Shell_ITaskGroup<struct winrt::WindowsUdk::UI::Shell::ITaskGroup>::IsRunning(void)const )"},
            &ITaskGroup_IsRunning_Original,
            ITaskGroup_IsRunning_Hook,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Taskbar::ITaskListButton>::get_IsRunning(bool *))"},
            &TaskListButton_get_IsRunning_Original,
            nullptr,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))"},
            &TaskListButton_UpdateVisualStates_Original,
            TaskListButton_UpdateVisualStates_Hook,
            true,
        },
    };

    bool ok = HookSymbols(module, hooks, ARRAYSIZE(hooks));
    Wh_Log(L"HookTaskbarViewDllSymbols: %s", ok ? L"OK" : L"FAILED");
    // ArrangeOverride is the one required symbol here, so it's always
    // worth confirming directly. The rest are optional (HWND-resolution
    // chain only) - named individually below whenever any of them is
    // actually missing, regardless of ArrangeOverride's own state.
    Wh_Log(L"  ArrangeOverride: %s",
           TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Original ? L"resolved"
                                                                        : L"MISSING");
    bool anyOptionalMissing = !TryGetItemFromContainer_TaskListWindowViewModel_Original ||
                              !TaskListWindowViewModel_get_TaskItem_Original ||
                              !TryGetItemFromContainer_TaskListGroupViewModel_Original ||
                              !TaskListGroupViewModel_IsMultiWindow_Original ||
                              !ITaskGroup_IsRunning_Original ||
                              !TaskListButton_get_IsRunning_Original ||
                              !TaskListButton_UpdateVisualStates_Original;
    if (anyOptionalMissing) {
        Wh_Log(L"  Missing (optional, HWND-resolution chain only): "
               L"%s%s%s%s%s%s%s",
               TryGetItemFromContainer_TaskListWindowViewModel_Original
                   ? L""
                   : L"TryGetItemFromContainer<TaskListWindowViewModel> ",
               TaskListWindowViewModel_get_TaskItem_Original
                   ? L""
                   : L"TaskListWindowViewModel::get_TaskItem ",
               TryGetItemFromContainer_TaskListGroupViewModel_Original
                   ? L""
                   : L"TryGetItemFromContainer<TaskListGroupViewModel> ",
               TaskListGroupViewModel_IsMultiWindow_Original
                   ? L""
                   : L"TaskListGroupViewModel::IsMultiWindow ",
               ITaskGroup_IsRunning_Original ? L"" : L"ITaskGroup::IsRunning ",
               TaskListButton_get_IsRunning_Original
                   ? L""
                   : L"TaskListButton::get_IsRunning ",
               TaskListButton_UpdateVisualStates_Original
                   ? L""
                   : L"TaskListButton::UpdateVisualStates ");
    }
    return ok;
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

        // Applied unconditionally, not gated on HookTaskbarViewDllSymbols'
        // own return value. That value only reflects whether
        // ArrangeOverride, the sole required symbol in its table, was
        // found - a missing optional HWND-resolution symbol never affects
        // it (see HookSymbols' own optional-parameter doc comment). And by
        // this point Wh_ModInit has already returned successfully - this
        // callback runs later, from the LoadLibraryExW hook, so there's no
        // "fail the mod's load" outcome available here regardless; hooks
        // apply best-effort from here on.
        HookTaskbarViewDllSymbols(module);
        Wh_ApplyHookOperations();
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

// ============================================================================
// Mod lifecycle
// ============================================================================

// Hosts the HWND-resolve timer and the drag-follow poll timer on a
// dedicated mod-owned thread rather than the taskbar's own, keeping both
// off the shell's layout thread. Originally also hosted the
// EVENT_OBJECT_SHOW and EVENT_OBJECT_LOCATIONCHANGE WinEventHooks this
// mod used to register here (removed in favor of
// TaskListButton::UpdateVisualStates and the drag-follow poll,
// respectively) - the name stuck around across both removals since this
// is still fundamentally "the dedicated background thread," just no
// longer one that happens to run any actual WinEventHook.
//
// Private message this thread's own queue uses to let ArmButtonHwndResolveTimer
// (called from the taskbar thread) re-arm the resolve timer, which lives
// on this thread.
constexpr UINT kArmResolveNowMsg = WM_APP + 1;

// Defined later in this section, alongside NextResolveDelayMs.
void CALLBACK ButtonHwndResolveTimerProc(HWND hwnd,
                                         UINT uMsg,
                                         UINT_PTR idEvent,
                                         DWORD dwTime);

DWORD WINAPI BackgroundWorkerThreadProc(LPVOID) {
    // Forces this thread's message queue into existence before SetTimer
    // runs below, so the timers this thread owns have somewhere to
    // deliver WM_TIMER messages to immediately.
    MSG msg;
    PeekMessage(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    Wh_Log(L"BackgroundWorkerThreadProc: dedicated thread %lu",
           GetCurrentThreadId());

    // Kicks off the first HWND-resolve attempt so buttons already present
    // at mod startup get picked up; NextResolveDelayMs/
    // ArmButtonHwndResolveTimer keep it armed afterward only as needed.
    g_buttonHwndResolveTimerId =
        SetTimer(nullptr, 0, 100, ButtonHwndResolveTimerProc);

    // Runs for as long as this thread does, switching between the fast
    // and idle cadences on its own - see DragFollowPollTimerProc's tail.
    // Starts fast, with the active window seeded from now, so a drag in
    // the first couple of seconds after the mod loads is followed live
    // rather than waiting out an idle interval first; it settles to the
    // idle rate on its own if nothing moves.
    g_lastDragFollowChangeTick = GetTickCount64();
    SetDragFollowPollInterval(kDragFollowPollActiveMs);

    // WM_APP (posted by StopBackgroundWorkerThread) is this thread's
    // shutdown signal.
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_APP) {
            break;
        }
        if (msg.message == kArmResolveNowMsg) {
            // Arm the replacement before killing the old one - the same
            // ordering, and for the same reason, as
            // SetDragFollowPollInterval: killing first and then failing to
            // arm would leave no resolve timer at all. Less severe here
            // than for the drag-follow poll, since the taskbar thread
            // re-arms this from several places (a real click, an
            // ArrangeOverride count change, a subclass install) so it
            // would recover eventually rather than never - but keeping the
            // old timer means it recovers on its own schedule instead of
            // waiting on an unrelated caller, and the failure no longer
            // passes silently. Killing the old one afterward is safe even
            // when it is the timer whose callback queued this message:
            // this thread's own message loop is what dispatches both.
            UINT_PTR newResolveTimerId = SetTimer(
                nullptr, 0, (UINT)msg.lParam, ButtonHwndResolveTimerProc);
            if (!newResolveTimerId) {
                Wh_Log(L"kArmResolveNowMsg: SetTimer failed, error=%lu - "
                       L"keeping the existing resolve timer",
                       GetLastError());
                continue;
            }
            if (g_buttonHwndResolveTimerId) {
                KillTimer(nullptr, g_buttonHwndResolveTimerId);
            }
            g_buttonHwndResolveTimerId = newResolveTimerId;
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_buttonHwndResolveTimerId) {
        KillTimer(nullptr, g_buttonHwndResolveTimerId);
        g_buttonHwndResolveTimerId = 0;
    }
    if (g_dragFollowPollTimerId) {
        KillTimer(nullptr, g_dragFollowPollTimerId);
        g_dragFollowPollTimerId = 0;
        // A live trackWindowPositions toggle can start this thread again
        // later; SetDragFollowPollInterval sets this unconditionally on
        // the way back up, but leaving a stale cadence behind here would
        // be misleading to read.
        g_dragFollowPollCurrentMs = 0;
    }

    return 0;
}

HANDLE g_backgroundWorkerThread;
// Written under g_backgroundWorkerThreadMutex, read without it (from the taskbar
// thread) in ArmButtonHwndResolveTimer - atomic makes that well-defined.
std::atomic<DWORD> g_backgroundWorkerThreadId;

// Serializes StartBackgroundWorkerThread/StopBackgroundWorkerThread against each other: a
// start call can still be inside CreateThread, before g_backgroundWorkerThread is
// written, when a concurrent stop call checks it - without this mutex
// making "is there a thread, and should one ever be created again" one
// atomic question both functions agree on, the stop call could miss the
// very thread it was meant to tear down, leaving it to crash the process
// when Windhawk unmaps the module later.
std::mutex g_backgroundWorkerThreadMutex;
bool g_backgroundWorkerThreadStopped;

void StartBackgroundWorkerThread() {
    std::lock_guard<std::mutex> guard(g_backgroundWorkerThreadMutex);
    if (g_backgroundWorkerThreadStopped || g_backgroundWorkerThread) {
        return;
    }

    // CreateThread needs a plain DWORD* out-param, published to the
    // atomic once it returns.
    DWORD threadId = 0;
    g_backgroundWorkerThread = CreateThread(nullptr, 0, BackgroundWorkerThreadProc,
                                    nullptr, 0, &threadId);
    if (!g_backgroundWorkerThread) {
        Wh_Log(L"StartBackgroundWorkerThread: CreateThread failed");
    } else {
        g_backgroundWorkerThreadId = threadId;
    }
}

// Waits for the thread to fully exit, guaranteeing both of its owned
// timers' KillTimer calls have run before returning. Shared by
// StopBackgroundWorkerThread (permanent, mod-teardown) and StopBackgroundWorkerThreadForToggle
// (reversible, live trackWindowPositions toggle) - see
// StopBackgroundWorkerThreadForToggle's own forward-declaration comment
// for why only the former sets g_backgroundWorkerThreadStopped.
void StopBackgroundWorkerThreadInternal(bool permanent) {
    // Serializes concurrent stop calls end-to-end, not just the
    // g_backgroundWorkerThreadMutex-guarded read/null section below -
    // without this, a second call arriving while an earlier one is already
    // past that section (waiting on the thread handle) would see
    // g_backgroundWorkerThread already null and return immediately,
    // letting its caller (e.g. Wh_ModUninit) proceed while the worker
    // thread is still executing mod code, right before Windhawk unmaps it.
    static std::mutex teardownMutex;
    std::lock_guard<std::mutex> teardownGuard(teardownMutex);

    HANDLE thread;
    DWORD threadId;
    {
        std::lock_guard<std::mutex> guard(g_backgroundWorkerThreadMutex);
        // Set before releasing the lock, so a StartBackgroundWorkerThread call
        // arriving after this point can't recreate a thread nobody would
        // tear down. Unconditional (even if thread turns out already null
        // below) so a concurrent StopBackgroundWorkerThreadForToggle racing this
        // permanent stop can't leave the latch unset.
        if (permanent) {
            g_backgroundWorkerThreadStopped = true;
        }
        thread = g_backgroundWorkerThread;
        threadId = g_backgroundWorkerThreadId;
        g_backgroundWorkerThread = nullptr;
        g_backgroundWorkerThreadId = 0;
    }
    if (!thread) {
        return;
    }

    // Retry until the thread's message queue exists (or it exits on its
    // own) rather than giving up after N tries - nothing else would wake
    // the unconditional wait below if the signal were never delivered.
    while (!PostThreadMessage(threadId, WM_APP, 0, 0)) {
        if (WaitForSingleObject(thread, 10) == WAIT_OBJECT_0) {
            break;  // Thread exited on its own before ever seeing WM_APP.
        }
    }

    // Unconditional wait: returning early would let Windhawk unmap this
    // module's code while the thread is still running inside it.
    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);
}

void StopBackgroundWorkerThread() {
    StopBackgroundWorkerThreadInternal(/*permanent=*/true);
}

// Lets trackWindowPositions be turned off live, without tripping the
// permanent g_backgroundWorkerThreadStopped latch StopBackgroundWorkerThread sets - a later
// StartBackgroundWorkerThread (from turning the setting back on) still works.
void StopBackgroundWorkerThreadForToggle() {
    StopBackgroundWorkerThreadInternal(/*permanent=*/false);
}

// Capped exponential backoff for the click-sentinel probe, shared by
// ResolvePendingButtonHwnds and NextResolveDelayMs. Fallback safety net for
// a launch that reaches TaskListButton::UpdateVisualStates through a path
// this mod doesn't expect, or on a build where that hook didn't resolve at
// all.
constexpr ULONGLONG kResolveBackoffCeilingMs = 30ULL * 60 * 1000;
ULONGLONG ResolveBackoffMs(int consecutiveFailures) {
    int shift = std::min(consecutiveFailures, 16);  // keep the shift itself from overflowing
    return std::min(2000ULL << shift, kResolveBackoffCeilingMs);
}

// How long until the next resolve attempt could do anything useful, based
// only on g_buttonHwndCache's recorded state - cheap enough to call after
// every timer tick.
DWORD NextResolveDelayMs() {
    ULONGLONG now = GetTickCount64();
    bool anyPending = false;
    // True for an entry that only the slow idle-rebind cadence still
    // applies to: resolved, terminal (consecutiveFailures at
    // kMaxResolveFailures), or confirmed-not-running/awaiting-first-click
    // (see ButtonHwndCacheEntry). The terminal case is load-bearing -
    // ResolvePendingButtonHwnds' backoffElapsed check structurally
    // evaluates false forever once the cap is reached, so if this didn't
    // independently stop chasing such an entry, its fixed dueAt would
    // fall further into the past every tick and pin the delay at 0
    // (SetTimer's 10ms floor) forever. notRunning/awaitingFirstClick
    // entries are only slowed, not excluded: their consecutiveFailures
    // never advances, so backoffElapsed does come true for them again in
    // time, which is what a not-running or still-unconfirmed button
    // wants.
    bool anyIdleWorthy = false;
    ULONGLONG earliestDue = 0;

    for (auto& kv : g_buttonHwndCache) {
        const ButtonHwndCacheEntry& entry = kv.second;
        if (entry.hwnd || entry.notRunning || entry.awaitingFirstClick ||
            entry.consecutiveFailures >= kMaxResolveFailures) {
            anyIdleWorthy = true;
            continue;
        }
        ULONGLONG dueAt =
            entry.lastAttempt + ResolveBackoffMs(entry.consecutiveFailures);
        if (!anyPending || dueAt < earliestDue) {
            earliestDue = dueAt;
            anyPending = true;
        }
    }

    if (!anyPending) {
        // A periodic re-check even once everything resolves, so a
        // drag-reorder rebind (see ButtonHwndCacheEntry) still gets
        // picked up. Only armed when something is actually resolved or
        // terminal; goes fully idle (INFINITE) only when the cache is
        // empty (or holds only genuinely-still-retryable entries, which
        // can't happen here since anyPending would then be true).
        if (anyIdleWorthy) {
            return kIdleResolveTickMs;
        }
        return INFINITE;
    }

    DWORD pendingDelay = earliestDue > now ? (DWORD)(earliestDue - now) : 0;
    if (anyIdleWorthy) {
        // Mixed cache: cap at kIdleResolveTickMs so already-resolved/
        // terminal entries still get their rebind re-check on the normal
        // cadence, without waiting on a pending entry's own long backoff.
        return std::min(pendingDelay, kIdleResolveTickMs);
    }
    return pendingDelay;
}

void CALLBACK ButtonHwndResolveTimerProc(HWND hwnd,
                                         UINT uMsg,
                                         UINT_PTR idEvent,
                                         DWORD dwTime) {
    // Self-managing one-shot: stop first, kick off the resolve pass, and
    // let it decide whether/when to re-arm (see ScheduleNextResolveTick).
    KillTimer(nullptr, idEvent);
    g_buttonHwndResolveTimerId = 0;

    if (g_unloading) {
        return;
    }

    // Snapshot so both branches below agree on one value.
    HWND hTaskbarWnd = g_hTaskbarWnd;
    if (!hTaskbarWnd) {
        // Taskbar window went away (recreate); EnsureTaskbarWnd will
        // re-resolve it. Retry later instead of letting null reach
        // PostMessage.
        ArmButtonHwndResolveTimer(kIdleResolveTickMs);
        return;
    }

    // No fallback if the subclass never installed - see
    // g_taskbarWndSubclassed. ResolvePendingButtonHwnds arms the next
    // delay itself once its pass completes; the only case needing
    // handling here is a PostMessage failure, since nothing else would
    // re-arm the timer if the message never arrives.
    if (g_taskbarWndSubclassed) {
        if (!PostMessage(hTaskbarWnd, ResolveButtonHwndsMsg(), 0, 0)) {
            Wh_Log(L"ButtonHwndResolveTimerProc: PostMessage failed, "
                   L"error=%lu", GetLastError());
            ArmButtonHwndResolveTimer(kIdleResolveTickMs);
        }
    }
}

// The resolve timer lives on the dedicated background worker thread, so
// arming it from the taskbar thread needs PostThreadMessage to ask that
// thread to do it on its own queue.
void ArmButtonHwndResolveTimer(DWORD delayMs) {
    if (!g_backgroundWorkerThreadId || g_unloading) {
        return;
    }
    if (!PostThreadMessage(g_backgroundWorkerThreadId, kArmResolveNowMsg, 0, delayMs)) {
        // ERROR_INVALID_THREAD_ID specifically is the benign startup race,
        // not a real failure: StartBackgroundWorkerThread publishes the
        // thread id as soon as CreateThread returns, but the thread only
        // gets a message queue once its own PeekMessage runs, and
        // EnsureTaskbarWnd reaches this call in the same pass that started
        // it. Nothing is lost - BackgroundWorkerThreadProc arms the first
        // resolve itself at startup, so this nudge was redundant anyway -
        // and logging it would put a scary-looking error in every session's
        // log for a condition that resolves itself microseconds later.
        DWORD error = GetLastError();
        if (error != ERROR_INVALID_THREAD_ID) {
            Wh_Log(L"ArmButtonHwndResolveTimer: PostThreadMessage failed, "
                   L"error=%lu", error);
        }
    }
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        Wh_Log(L"Taskbar view module already loaded at init time");
        g_taskbarViewDllLoaded = true;
        // Not gated on HookTaskbarViewDllSymbols' own return value - that
        // value only reflects whether ArrangeOverride, the sole required
        // symbol in its table, was found; a missing optional
        // HWND-resolution symbol never affects it (see HookSymbols' own
        // optional-parameter doc comment). ArrangeOverride itself is
        // checked directly below instead.
        HookTaskbarViewDllSymbols(taskbarViewModule);
        if (!TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Original) {
            return FALSE;
        }
    } else {
        Wh_Log(L"Taskbar view module not loaded yet, will hook on load");

        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            kernelBaseModule
                ? (decltype(&LoadLibraryExW))GetProcAddress(
                      kernelBaseModule, "LoadLibraryExW")
                : nullptr;
        if (!pKernelBaseLoadLibraryExW) {
            Wh_Log(L"Failed to resolve kernelbase.dll!LoadLibraryExW");
            return FALSE;
        }
        WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (!g_taskbarViewDllLoaded) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                // See HandleLoadedModuleIfTaskbarView - unconditional so a
                // missing optional symbol doesn't discard resolved hooks.
                HookTaskbarViewDllSymbols(taskbarViewModule);
                Wh_ApplyHookOperations();
            }
        }
    }

    EnsureTaskbarWnd();
    Wh_Log(L"g_hTaskbarWnd = %p, g_taskbarViewDllLoaded = %d",
           (HWND)g_hTaskbarWnd, (int)g_taskbarViewDllLoaded);

    InvalidateTaskbarLayout();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    // The background worker thread is torn down later, in Wh_ModUninit - this
    // mod's hooks (and so StartBackgroundWorkerThread's own call paths) stay live
    // until this function returns. Setting g_unloading first is what stops
    // the click-sentinel probe and makes IUIElement_Arrange_Hook fall
    // through to native positioning immediately.

    // Requests a relayout before removing the subclass below - not itself
    // an immediate snap-back (InvalidateArrange/InvalidateMeasure only
    // mark the tree dirty; the actual re-Arrange runs whenever XAML gets
    // around to its next layout pass), but by the time that pass runs,
    // IUIElement_Arrange_Hook already falls through to native positioning
    // since g_unloading is set, so the end result is the same. Must be
    // SendMessage, not PostMessage: RemoveWindowSubclassFromAnyThread
    // below is itself a SendMessage, and a merely posted invalidate would
    // arrive after the subclass is gone, with nothing left to dispatch it.
    HWND hTaskbarWnd = g_hTaskbarWnd;
    if (hTaskbarWnd && g_taskbarWndSubclassed) {
        SendMessage(hTaskbarWnd, InvalidateTaskbarLayoutMsg(), 0, 0);
        // Reclaims a SettingsChangedMsg posted but not yet dispatched -
        // see DrainSettingsMsg's own comment.
        SendMessage(hTaskbarWnd, DrainSettingsMsg(), 0, 0);
    }
    // Removes the subclass as early as it's safe to - the sole marshal
    // onto the taskbar thread (see g_taskbarWndSubclassed).
    if (hTaskbarWnd && g_taskbarWndSubclassed.exchange(false)) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            hTaskbarWnd, TaskbarWndSubclassProc);
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    // Runs here rather than in Wh_ModBeforeUninit - the mod's own hooks
    // are gone by this point, so nothing can reawaken the thread. Also
    // covers the HWND-resolve timer, which lives on the same thread.
    StopBackgroundWorkerThread();
}

// Holds a settings change ApplyLoadedSettings couldn't hand off to the
// taskbar thread yet (no taskbar window resolved, or its subclass isn't
// installed) - EnsureTaskbarWnd applies it via ApplyPendingSettingsIfAny
// once its own next successful subclass install makes that handoff
// possible. Guarded by a mutex: ApplyLoadedSettings runs on
// Wh_ModSettingsChanged's arbitrary calling thread, while
// ApplyPendingSettingsIfAny runs on whichever thread called
// EnsureTaskbarWnd.
std::mutex g_pendingSettingsMutex;
bool g_hasPendingSettings;
ModSettings g_pendingSettings;

// Applies a freshly-loaded settings struct on the taskbar's own thread,
// since it reassigns leftApps/rightApps while a concurrent layout pass
// could be reading them. If the taskbar window isn't resolved yet or its
// subclass isn't installed, stashes the settings above instead of
// assigning g_settings directly here - a foreign-thread write to it could
// otherwise race EnsureTaskbarWnd resolving the taskbar (and a layout pass
// starting to read g_settings) concurrently on another thread. No further
// fallback if PostMessage itself fails - logged loudly rather than
// silently dropped.
void ApplyLoadedSettings(ModSettings settings) {
    // Snapshot so both checks below agree.
    HWND hTaskbarWnd = g_hTaskbarWnd;
    if (!hTaskbarWnd || !g_taskbarWndSubclassed) {
        std::lock_guard<std::mutex> guard(g_pendingSettingsMutex);
        g_pendingSettings = std::move(settings);
        g_hasPendingSettings = true;
        return;
    }

    // PostMessage is async, so ownership transfers to a heap allocation,
    // released only once PostMessage has queued it, and reclaimed by
    // TaskbarWndSubclassProc's SettingsChangedMsg case.
    auto heapSettings = std::make_unique<ModSettings>(std::move(settings));
    if (PostMessage(hTaskbarWnd, SettingsChangedMsg(), 0,
                    (LPARAM)heapSettings.get())) {
        // The pointer itself was already captured via .get() above; this
        // call's own return value is the same pointer, discarded here on
        // purpose - release() is invoked purely so the unique_ptr's
        // destructor doesn't free what PostMessage's receiver now owns.
        (void)heapSettings.release();
        return;
    }
    Wh_Log(L"ApplyLoadedSettings: PostMessage failed, error=%lu, "
           L"new settings not applied", GetLastError());
}

void ApplyPendingSettingsIfAny() {
    ModSettings settings;
    {
        std::lock_guard<std::mutex> guard(g_pendingSettingsMutex);
        if (!g_hasPendingSettings) {
            return;
        }
        settings = std::move(g_pendingSettings);
        g_hasPendingSettings = false;
    }
    ApplyLoadedSettings(std::move(settings));
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    // Reads every setting on this calling thread; only the final
    // assignment into g_settings needs the taskbar thread (see
    // ApplyLoadedSettings).
    ApplyLoadedSettings(LoadSettingsFromStore());

    InvalidateTaskbarLayout();
}
