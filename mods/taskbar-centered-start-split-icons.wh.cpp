// ==WindhawkMod==
// @id              taskbar-centered-start-split-icons
// @name            Taskbar Start Button Centered Origin
// @description     Pins the Start button to true screen-center and splits running-app taskbar buttons into two groups flanking it by which side of the screen each window is on (Windows 11 only; incompatible with "Start button always on the left")
// @version         0.1.0
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
# Centered Start button with position-split taskbar icons

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
button switches sides to follow it. Side-switching is driven by a global
window-location-change listener and is best-effort: it happens shortly
after a drag/move settles, not on every intermediate pixel of the drag.

Search, Task View and Widgets can either stay at the far left edge, or move
right next to Start on whichever side you prefer.

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
  there's no reason to run both together.
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
  unconfirmed, not yet investigated further. **Separately reported:** with
  "Always" enabled, multiple windows of the same app have been observed
  not combining into one button at all, appearing as separate icons
  regardless of count. This mod has no code path that could cause that -
  grouping is a native decision made before this mod's hooks ever see a
  button - but it's flagged here as a planned follow-up pending
  confirmation of whether it reproduces with the mod disabled.
- **A grouped button (multiple windows combined under one icon) follows
  only its first window.** With "Combine taskbar buttons" set to "Always",
  a group's side and ordering are both decided by whichever of its windows
  happens to be first in the taskbar's own internal list - not necessarily
  the one you'd expect - so a group whose windows straddle the center line
  won't visually reflect all of them. There's no exposed way to pick a
  more meaningful "primary" window for a group, so this is a documented
  tradeoff rather than a bug.
- **Undocumented internals.** This mod hooks private, unversioned classes
  inside `taskbar.dll` and `Taskbar.View.dll` (via symbols resolved from
  Microsoft's public symbol server at runtime, not hardcoded offsets). A
  Windows update can change these internals and break the mod until it's
  updated. If that happens, disable the mod rather than filing against
  explorer.exe crashing.
- The "resolve which HWND a taskbar button represents" step reuses a
  technique from other taskbar-reordering mods (synchronously reporting a
  sentinel "click" to the taskbar's internal click handler, which is
  intercepted before it does anything, to read back the window handle). It
  runs on a periodic timer rather than inline during layout, and Arrange
  only ever reads whatever the timer has already cached - running it
  synchronously from inside the taskbar's own layout pass was the
  confirmed cause of an explorer.exe crash (specifically when Windows'
  "show taskbar apps on" setting is anything other than "All taskbars",
  since that's when a window moving across monitors structurally adds/
  removes taskbar buttons rather than just repositioning them).
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
- unresolvedAppsDefaultSide: left
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
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cwctype>
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

// Where a pinned-but-not-running app's icon sits within its side's icon
// group, relative to the other (running, position-classified) icons on the
// same side. Only meaningful when taskListOrder is DistanceFromCenter -
// TaskbarOrder has no notion of "outer edge" vs "adjacent to Start" to
// begin with, since it doesn't rank icons by distance at all.
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

// Resolves the effective side for pinned-but-not-running apps that aren't
// matched by leftApps/rightApps, per the unresolvedAppsDefaultSide setting.
// The "contralateral" mode mirrors Search/Task View/Widgets' placement:
// opposite of systemButtonsAdjacentSide when they sit next to Start, or the
// right if they're left at the taskbar's far-left edge instead (still
// their "side" for this purpose, since that edge is unambiguously left of
// everything else).
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

// The distance-from-center order key for a pinned-but-not-running app (one
// with no window to measure a real distance from). Made -infinity instead
// of the default +infinity when pinnedAppsAnchor is AdjacentToStart, since
// PlanTaskListButtons' distance-from-center sort treats a smaller orderKey
// as closer to Start - nothing can beat -infinity, so these always end up
// innermost rather than outermost.
double PinnedAppOrderKey() {
    return g_settings.pinnedAppsAnchor == PinnedAppsAnchor::AdjacentToStart
               ? -std::numeric_limits<double>::infinity()
               : std::numeric_limits<double>::infinity();
}

// Reads every setting into a fresh ModSettings, with no dependency on
// which thread it runs on - Wh_Get/FreeStringSetting don't touch XAML or
// COM, unlike almost everything else in this file. Kept separate from
// LoadSettings/g_settings itself so Wh_ModSettingsChanged can do this part
// on its own calling thread and marshal only the resulting struct's
// assignment - see its comment for why.
ModSettings LoadSettingsFromStore() {
    ModSettings s;
    // Clamped rather than trusted as-is: a negative value from the
    // settings UI would otherwise pull the flanking icon groups into the
    // Start button instead of away from it.
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

// Never traverse the taskbar's XAML tree (siblings, classification,
// widths) from inside a nested IUIElement::Arrange call - XAML's layout
// system can be mid-structural-mutation of that same repeater there
// (STATUS_STOWED_EXCEPTION when a window moves across monitors while
// Windows' "show taskbar apps on" setting isn't "All taskbars", since
// that structurally adds/removes taskbar buttons). RecomputeLayoutPlan
// does the entire traversal once per ArrangeOverride pass, up front,
// writing every element's target X into g_lastArrangedX -
// IUIElement_Arrange_Hook then becomes a pure map lookup.

// atomic: written from EnsureTaskbarWnd (the taskbar thread, or
// Wh_ModAfterInit's thread at startup) and read from the dedicated
// WinEventHook thread (WinEventProc, InvalidateTaskbarLayout,
// ButtonHwndResolveTimerProc) - a plain HWND here was an oversight, not a
// decision, given g_taskbarWndSubclassed right below already needed the
// same treatment for the same reason. Beyond the formal race, a stale
// read passing a null-check right before EnsureTaskbarWnd clears it to
// nullptr has a real consequence: PostMessage(nullptr, ...) is not a
// no-op, it posts to the *calling* thread's own queue and returns TRUE -
// silently misdirecting the message instead of failing loudly. Callers
// with more than one read of this value snapshot it into a local first
// (see InvalidateTaskbarLayout, ButtonHwndResolveTimerProc,
// ApplyLoadedSettings, RecomputeLayoutPlan) so all their reads agree on
// one value even if a concurrent write lands mid-function.
std::atomic<HWND> g_hTaskbarWnd;

// Whether TaskbarWndSubclassProc is currently installed on g_hTaskbarWnd -
// see EnsureTaskbarWnd (where it's installed) and InvalidateTaskbarLayout/
// ButtonHwndResolveTimerProc/ApplyLoadedSettings (which all check it before
// posting to the taskbar thread). This is the SOLE way any of those reach
// the taskbar thread - there is no fallback marshal if the subclass never
// installed (SetWindowSubclassFromAnyThread failing is rare - see
// EnsureTaskbarWnd's own log line for that case): live drag-follow,
// HWND resolution, and live settings updates are simply unavailable until
// the taskbar recreates and a fresh subclass attempt succeeds. The mod's
// core centering/splitting is unaffected either way, since that's computed
// synchronously inside RecomputeLayoutPlan on whatever ArrangeOverride
// passes XAML triggers on its own. atomic: set from Explorer's UI thread
// (EnsureTaskbarWnd) or Wh_ModAfterInit's thread, read from
// InvalidateTaskbarLayout's callers, which include the dedicated
// WinEventHook thread.
std::atomic<bool> g_taskbarWndSubclassed;

HWINEVENTHOOK g_locationChangeHook;
HWINEVENTHOOK g_showEventHook;
std::atomic<int> g_winEventRawCount;
std::atomic<int> g_winEventInvalidateCount;
std::atomic<int> g_invalidateSkippedReentrant;
std::atomic<int> g_invalidateExceptions;

// Only ever touched from the dedicated WinEventHook thread (see
// StartWinEventHook) - WinEventProc, DragFollowTrailingTimerProc and
// ButtonHwndResolveTimerProc all run there exclusively, so none of this
// needs synchronization.
UINT_PTR g_dragFollowTrailingTimerId;

// The HWND-resolve timer's own id - lives on this same dedicated thread
// (see ButtonHwndResolveTimerProc) rather than on a window the mod
// doesn't own, so teardown is covered by the same thread join that
// already tears down the WinEvent hook, with no separate marshal that
// could fail during unload.
UINT_PTR g_buttonHwndResolveTimerId;

// Same thread-ownership as g_dragFollowTrailingTimerId above. Hoisted out
// of WinEventProc's own function-local static so DragFollowTrailingTimerProc
// can update it too - a trailing-timer fire is itself an invalidate, so
// the next WinEventProc event needs to see it as "now", not still throttle
// against whatever raw event last landed outside the 150ms window before
// the trailing timer took over.
ULONGLONG g_lastDragFollowInvalidate;

// Leading-edge throttle for EVENT_OBJECT_SHOW, mirroring the
// LOCATIONCHANGE throttle above - a top-level window becoming visible
// fires this event just as often system-wide, and a single arm(0) call is
// enough to catch every button that's currently pending regardless of how
// many SHOW events land in the same burst (unlike drag-follow, there's no
// "final position" that specifically needs the trailing event, so no
// trailing timer is needed here).
ULONGLONG g_lastShowEventArm;

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

    // The shared_ptr's ref-count block (taskbarHostSharedPtr[1]) must be
    // released on every path below, not just a fully successful call -
    // this runs on every ArrangeOverride pass, so a leaked reference here
    // is one per pass for the life of the process.
    struct DecrefGuard {
        void* ptr;
        ~DecrefGuard() { std__Ref_count_base__Decref_Original(ptr); }
    } decrefGuard{taskbarHostSharedPtr[1]};

    // The top-level null check above only rules out *both* slots being
    // null - slot 0 can still be null while slot 1 isn't, and it's
    // dereferenced below. Bail out here (after the guard above is
    // already constructed, so [1] still gets released).
    if (!taskbarHostSharedPtr[0]) {
        return nullptr;
    }

    // The offset of the XAML element pointer inside TaskbarHost isn't
    // exposed by any symbol, so it's read out of the prologue of a
    // neighboring function that's known to access it at a fixed offset.
    // @architecture x86-64 also covers ARM64 - explorer.exe is a predefined
    // shell process, so the mod is built and loaded natively as ARM64 there
    // too, and the two architectures' prologues need separate opcode
    // patterns. Bailing out on a pattern mismatch (rather than proceeding
    // with a guessed offset) since a wrong offset here means dereferencing
    // whatever happens to live there and calling a virtual method through
    // it.
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
        // populated yet (e.g. mid-creation after a taskbar recreate). This
        // runs on every dirty arrange pass and from a periodic timer, so
        // skipping here matters - a null deref is a raw access violation,
        // not a C++ exception, so no try/catch in any caller can contain it.
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
//   -> (sentinel "click" through the taskbar's own click handler, which we
//      intercept before it acts on it) -> native ITaskItem -> HWND.
//
// Grouped button (when "Combine taskbar buttons" is Always - each button can
// represent multiple windows of one app, and isn't bound to a
// TaskListWindowViewModel at all, so the above fails at the very first
// step):
//   TaskListButton (XAML) -> TaskListGroupViewModel -> (a second sentinel,
//   piggybacked on TaskListGroupViewModel::IsMultiWindow's internal call to
//   ITaskGroup::IsRunning, which we intercept the same way) -> WindowsUdk
//   ITaskGroup -> (sentinel "click" again, this time on the group) -> native
//   ITaskGroup -> its internal task-items array (located by exploiting
//   CTaskGroup::GetNumItems' known-trivial implementation - see
//   GetTaskItemsArray) -> first item's HWND, used as the group's
//   representative position.
//
// Both of these are the same techniques used by taskbar-reordering and
// per-app-volume-control mods to map a button back to a window/windows;
// here they're reused read-only, purely to find out where a window
// currently is on screen.
// ============================================================================

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

// The click-sentinel resolution technique (see the section comment above)
// dispatches a REAL click through TaskItem::ReportClicked/
// TaskGroup::ReportClicked, relying entirely on this hook recognizing the
// sentinel and swallowing it before the taskbar's real HandleClick ever
// runs. Reference mods using the same technique only fire it in response
// to an actual user gesture on one specific button, so a broken
// interception there just means the user's own click happens twice. This
// mod instead fires it unattended, from a background timer, against every
// button it hasn't resolved yet - if a future Windows update changes
// ReportClicked's internal call path so it stops reaching this hook, every
// one of those probes silently becomes a genuine click, which for the
// buttons actually retried (pinned-but-not-running apps) means Explorer
// spontaneously launching them on a timer, forever, with no user action
// and no visible sign anything is wrong (an unintercepted probe looks
// identical to an ordinary resolution failure).
//
// The interception either works on this Windows build or it doesn't - it
// isn't a per-button thing. Latched per path (item vs. group), not as one
// shared flag: the two paths reach CTaskListWnd::HandleClick through
// different internal call chains (TaskItem::ReportClicked vs.
// TaskGroup::ReportClicked), so a Windows update could break only one of
// them - a shared flag would let a still-working item-path confirmation
// mask a broken group path, which matters most because the group path is
// the one a pinned-but-not-running app's button keeps retrying (see
// ResolveHwndFromTaskGroup's IsRunning check for how that retry is now
// also avoided at the source). Each *Broken latch permanently gates its
// own path in ResolveHwndFromTaskListButton - once set, that path never
// calls ReportClicked again, rather than retrying a mechanism now known
// to dispatch real clicks.
//
// A single unconfirmed miss is NOT proof the interception is broken - a
// probe can also fail to reach HandleClick because the window closed
// between resolving the task item and dispatching the click, ReportClicked
// failed internally, or the item was mid-teardown during a taskbar
// rebuild. Latching a whole path dead on the first miss of a session
// risked exactly that: an early unlucky miss on the item path (the common
// path, used by every ungrouped button) would silently degrade every
// button on the taskbar to unresolvedAppsDefaultSide, for the rest of the
// session, with only a log line as the symptom. kClickSentinelMissesBeforeBroken
// requires a few misses (still a bounded cost if the mechanism really is
// broken) before actually latching - see NoteUnconfirmedClickSentinelMiss.
//
// This bound only holds pre-confirmation: NoteUnconfirmedClickSentinelMiss
// returns immediately once a path is confirmed, by design (a post-
// confirmation miss is routinely innocent - see above - so counting it
// would risk latching a working path dead over an unrelated timing
// hiccup). So "at most kClickSentinelMissesBeforeBroken real clicks per
// path, per session" is the guarantee before first confirmation, not a
// session-wide cap. Also note ResolveHwndFromTaskListButton tries the
// item path first and falls through to the group path on a miss, so one
// unresolvable button can burn a probe on BOTH paths in a single attempt
// - the worst case before both latches trip is
// 2 * kClickSentinelMissesBeforeBroken (6, at the current constant)
// dispatched clicks, not kClickSentinelMissesBeforeBroken.
std::atomic<bool> g_clickSentinelItemConfirmed;
std::atomic<bool> g_clickSentinelItemBroken;
std::atomic<int> g_clickSentinelItemMisses;
std::atomic<bool> g_clickSentinelGroupConfirmed;
std::atomic<bool> g_clickSentinelGroupBroken;
std::atomic<int> g_clickSentinelGroupMisses;
constexpr int kClickSentinelMissesBeforeBroken = 3;

// Which path's probe is in flight on this thread when a sentinel click
// might land in CTaskListWnd_HandleClick_Hook below - the hook only sees
// the click itself, not which resolve function dispatched it, so this is
// what lets it credit the right path's *Confirmed flag. Set immediately
// before dispatching a probe, alongside the existing reset-before/read-
// after g_clickSentinel_TaskItem/_TaskGroup pattern.
thread_local bool g_clickSentinelProbingGroup;

HRESULT WINAPI CTaskListWnd_HandleClick_Hook(void* pThis,
                                              void* taskGroup,
                                              void* taskItem,
                                              void** launcherOptions) {
    if (launcherOptions && *launcherOptions == (void*)&g_clickSentinel) {
        g_clickSentinel_TaskItem = taskItem;
        g_clickSentinel_TaskGroup = taskGroup;
        // .exchange rather than plain assignment purely so the log fires
        // exactly once, the first time this path is ever confirmed -
        // gives a concrete, observable answer (for the PR record and any
        // future debugging) to "does the group path actually get
        // exercised/intercepted on this build", which resolve-stats alone
        // can't distinguish from the item path.
        bool alreadyConfirmed =
            g_clickSentinelProbingGroup
                ? g_clickSentinelGroupConfirmed.exchange(true)
                : g_clickSentinelItemConfirmed.exchange(true);
        if (!alreadyConfirmed) {
            Wh_Log(L"Click-sentinel interception confirmed working (%s path)",
                   g_clickSentinelProbingGroup ? L"group" : L"item");
        }
        return S_OK;
    }

    return CTaskListWnd_HandleClick_Original(pThis, taskGroup, taskItem,
                                              launcherOptions);
}

// Called right after a real ReportClicked probe comes back with no
// capture - one of the ways "no capture" can happen, alongside the
// unrelated cases described above (closed window, internal failure,
// teardown timing), which is exactly why this requires a few misses
// rather than latching on the first one.
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

// RAII around g_captureTaskGroup's set/clear pair - if the IsMultiWindow
// call between them ever exited non-locally, a plain set/clear (the
// original form) would leave this stuck true, and ITaskGroup_IsRunning_Hook
// would then answer every subsequent real IsRunning call with a hardcoded
// false instead of forwarding to Original - a wrong answer to a question
// the taskbar itself uses for real decisions (running indicators, click
// behavior), for the rest of the session. Same reasoning as
// ScopedArrangeOverrideFlag elsewhere in this file.
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

// CTaskGroup::GetNumItems' entire body is just `return
// DPA_GetPtrCount(this->taskItemsArray);`, i.e. reading one int off `this`
// at a fixed-but-undocumented offset. Calling it with a fake "this" that's
// actually an array of pointers-to-sequential-ints turns that read into a
// self-reporting probe: whatever offset it reads becomes the returned
// value, revealing the real offset without needing the struct layout.
// Adapted from the per-app volume control mod, which needs the same task
// group -> item list access for an unrelated reason.
// Shared with GetTaskItemsArray's bounds check below - the probe can
// never legitimately return an offset at or past this size.
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
    // Offset 0 is the vtable slot, so the probe can never legitimately
    // land there - either GetNumItems' implementation stopped being the
    // trivial "return DPA_GetPtrCount(this->taskItemsArray)" form this
    // technique relies on, or the probe array (kTaskItemsArrayProbeSize
    // ints) was too small to reach the real offset. Either way, trusting
    // an out-of-range offset here means dereferencing whatever happens to
    // be there and handing it to DPA_GetPtrCount as if it were a real
    // array - a wild read on a background timer, not a user gesture.
    if (offset == 0 || offset >= kTaskItemsArrayProbeSize) {
        return nullptr;
    }
    return (HDPA)((void**)taskGroup)[offset];
}

using TaskGroup_ReportClicked_t = int(WINAPI*)(void* pThis, void* param);
TaskGroup_ReportClicked_t TaskGroup_ReportClicked_Original;

// Diagnostics only: how often the HWND-resolution chain succeeds vs
// fails, combined across both the individual and grouped-button paths -
// LayoutPlanStats' own resolved/total count already gives the
// steady-state health signal, this mainly confirms the chain is running
// at all. A stage-by-stage breakdown (which specific step failed) was
// useful while first bringing this technique up against a new Windows
// build; worth re-adding locally for that kind of investigation rather
// than carrying it permanently.
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

HWND ResolveHwndFromIndividualTaskItem(FrameworkElement element) {
    // CTaskListWnd_HandleClick_Original is what proves the interception
    // hook is actually installed - it's optional in HookTaskbarDllSymbols
    // (see that symbol table's comment), so on a build where it didn't
    // resolve, CTaskListWnd_HandleClick_Hook never gets installed and the
    // ReportClicked call below would be a genuine click on a live window
    // rather than an intercepted sentinel. Checked here specifically
    // (not just implied by the other symbols below) since this is the one
    // way the probe can be unsafe that's knowable up front, rather than
    // only discoverable via NoteUnconfirmedClickSentinelMiss's runtime
    // miss-counting.
    if (!CTaskListWnd_HandleClick_Original ||
        !TryGetItemFromContainer_TaskListWindowViewModel_Original ||
        !TaskListWindowViewModel_get_TaskItem_Original ||
        !TaskItem_ReportClicked_Original) {
        return nullptr;
    }

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
// overview comment above this section for the full chain.
HWND ResolveHwndFromTaskGroup(FrameworkElement element) {
    // See ResolveHwndFromIndividualTaskItem's comment - same reasoning,
    // same interception hook (CTaskListWnd::HandleClick handles both the
    // item and group ReportClicked paths).
    if (!CTaskListWnd_HandleClick_Original ||
        !TryGetItemFromContainer_TaskListGroupViewModel_Original ||
        !TaskListGroupViewModel_IsMultiWindow_Original ||
        !TaskGroup_ReportClicked_Original || !CTaskGroup_GetNumItems_Original) {
        return nullptr;
    }

    // GetTaskItemsArrayOffset() is a memoized, side-effect-free probe -
    // safe to validate up front rather than only after the fact. Without
    // this, a build where CTaskGroup::GetNumItems stopped being the
    // trivial form the offset probe relies on would still dispatch a
    // click below on every single group probe (the sentinel interception
    // itself is unaffected, so the click-sentinel latch never trips -
    // it's GetTaskItemsArray's own bounds check further down that always
    // fails instead), turning every group resolution attempt into a
    // pure-waste ReportClicked into the taskbar's click machinery,
    // forever, with nothing to bound it.
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
        // IsMultiWindow's implementation happens to call
        // ITaskGroup::IsRunning internally, which is hooked above to
        // capture its `this` (the native WindowsUdk task group) instead
        // of really answering the question. The -1 adjusts from the
        // interface pointer QueryInterface handed back to the adjacent
        // vtable IsMultiWindow actually needs - a fixed ABI detail of
        // this object, not a magic number specific to this mod.
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

    // A group with no running windows (a pinned-but-not-running app) can
    // never yield an HWND - its task items array is legitimately empty,
    // so GetTaskItemsArray's check below always fails for it anyway -
    // and it's exactly the group that would otherwise get re-probed
    // forever at the backoff ceiling for the life of the session. Bail
    // out before dispatching a click at it at all, rather than detecting
    // the failure after the fact. IsRunning is a sibling method on the
    // same interface already captured above; the "consume" calling
    // convention it was hooked under (see ITaskGroup_IsRunning_Hook's
    // *(void**)pThis capture) expects a pointer to a variable holding the
    // interface pointer, not the interface pointer itself - hence
    // &windowsUdkTaskGroup here, matching how it was captured.
    if (ITaskGroup_IsRunning_Original &&
        !ITaskGroup_IsRunning_Original(&windowsUdkTaskGroup)) {
        g_resolveStats.failure++;
        return nullptr;
    }

    g_clickSentinel_TaskGroup = nullptr;
    g_clickSentinelProbingGroup = true;
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

HWND ResolveHwndFromTaskListButton(FrameworkElement element) {
    // See g_clickSentinelItemBroken/g_clickSentinelGroupBroken's comment -
    // once a path's sentinel is known not to be intercepted on this
    // build, every further probe on that path would be a genuine click
    // rather than a resolution attempt, so each path bails out before
    // calling its own ReportClicked again. The other path is unaffected,
    // since a Windows update could break just one of the two call chains.
    HWND hwnd = g_clickSentinelItemBroken
                    ? nullptr
                    : ResolveHwndFromIndividualTaskItem(element);
    if (hwnd) {
        return hwnd;
    }

    if (g_clickSentinelGroupBroken) {
        return nullptr;
    }
    return ResolveHwndFromTaskGroup(element);
}

// Per-button HWND cache, keyed by the XAML element's ABI pointer. Avoids
// re-running the resolution chain every pass, and negatively caches
// failures (a pinned-but-not-running app's task group legitimately has
// zero windows, so resolution fails until it's actually launched).
//
// consecutiveFailures drives capped exponential backoff (2s up to the
// 30-minute kResolveBackoffCeilingMs) rather than a fixed retry: the
// resolution chain ends in a synthetic click against the taskbar's real
// click handler, and ItemsRepeater recycles the same element/cache entry
// for a given index rather than creating a new one, so a hard stop would
// permanently break side-following for a pinned app that's later
// launched. A negatively-cached element's failures accumulate for as
// long as its button exists, so a long-idle session's backoff can be
// close to the ceiling by the time the app is actually launched -
// g_forceResolveUnresolved exists specifically to bypass that when
// there's real evidence (a window just appeared) that a retry is worth
// trying regardless of the schedule.
//
// identity (the button's accessible name at resolve time) catches a
// different case: ItemsRepeater can rebind an already-realized element
// to a different item (e.g. a drag-reorder) without destroying it. The
// old HWND stays valid, just no longer this element's, so an IsWindow()
// check alone can't detect it - ResolvePendingButtonHwnds compares
// identity on every check (both the resolved and negatively-cached
// branches) and forces a re-resolve on mismatch.
struct ButtonHwndCacheEntry {
    HWND hwnd = nullptr;
    std::wstring identity;
    ULONGLONG lastAttempt = 0;
    int consecutiveFailures = 0;
};
std::unordered_map<void*, ButtonHwndCacheEntry> g_buttonHwndCache;

// Set by WinEventProc's EVENT_OBJECT_SHOW branch and the ArrangeOverride
// hook's button-count-change check - both call ArmButtonHwndResolveTimer(0)
// to make the next resolve pass run immediately, but arming the timer
// sooner doesn't by itself bypass a negatively-cached entry's own backoff
// gate (see ButtonHwndCacheEntry's comment for why that backoff can be
// long-lived). Consumed once per pass in ResolvePendingButtonHwnds to
// force a negatively-cached entry to retry regardless of backoff - but
// only up to kMaxForcedRetryFailures consecutive failures (see there for
// why this is capped, not unconditional).
std::atomic<bool> g_forceResolveUnresolved;

// A group with no running windows can never yield an HWND and bails at
// ResolveHwndFromTaskGroup's IsRunning check before ever dispatching a
// click - g_forceResolveUnresolved's whole justification for bypassing
// backoff assumes THAT is the only reason a negatively-cached entry keeps
// failing (a pinned app that just launched, still catching up to its own
// EVENT_OBJECT_SHOW). But a RUNNING app's button can also fail to resolve
// for reasons that don't bail out early - ReportClicked failing
// internally, a task item mid-teardown, GetTaskItemsArray coming back
// empty - and for those, forcing every retry unconditionally means
// dispatching a real ReportClicked on both paths on every forced pass,
// indefinitely, at up to the ~7Hz EVENT_OBJECT_SHOW can arm this at -
// exactly the runaway-real-clicks scenario the backoff schedule exists to
// bound. Capping the force-bypass to entries that have only failed a few
// times keeps the "pinned app just launched" fast path intact (that case
// is still failing 0 times when EVENT_OBJECT_SHOW first fires) while
// letting the normal backoff schedule take back over for an entry that's
// failed repeatedly, the same way it already would with no force at all.
constexpr int kMaxForcedRetryFailures = 3;

// The HWNDs g_buttonHwndCache currently resolves to, rebuilt at the end
// of every successful ResolvePendingButtonHwnds pass (taskbar thread) and
// read by WinEventProc (the dedicated WinEventHook thread) to filter drag
// -follow's EVENT_OBJECT_LOCATIONCHANGE events - a window that never
// resolved to a taskbar button can't change this mod's layout, so there's
// no reason to pay for a relayout on its account. Needs its own mutex
// (not covered by the arrange-hook thread confinement the rest of this
// cache enjoys) since it's the one piece of resolve state genuinely read
// cross-thread outside a marshal.
std::mutex g_resolvedHwndsMutex;
std::unordered_set<HWND> g_resolvedHwnds;

// Actually runs the resolution chain and updates the cache. Returns
// whether the cached HWND changed - used by the caller to decide whether
// a relayout is worth triggering.
//
// Deliberately ONLY ever called from the resolve timer, never from
// GetButtonHwnd or an active Arrange pass: the click-sentinel technique
// interacts with the taskbar's internal click-handling machinery, and
// running it while a button is being structurally inserted/removed from
// an ItemsRepeater's data source reaches STATUS_STOWED_EXCEPTION
// (confirmed via crash-dump analysis) - which happens whenever Windows'
// "show taskbar apps on" setting isn't "All taskbars" and a window moves
// across monitors.
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

    HWND hwnd = ResolveHwndFromTaskListButton(element);
    failures = hwnd ? 0 : failures + 1;
    g_buttonHwndCache[key] = {hwnd, identity, GetTickCount64(), failures};
    return hwnd != previous;
}

// Read-only: never triggers resolution itself, only ever reads whatever
// ResolveAndCacheButtonHwnd (via the timer) has already cached. A button
// with no cache entry yet, or one whose cached window has since closed,
// reads as unresolved here and falls back to the default-side
// classification until the next timer tick catches up - see
// ResolveAndCacheButtonHwnd's comment for why this can never call the
// resolution chain directly.
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

// ResolvePendingButtonHwnds (which actually walks TaskListButtons and
// calls ResolveAndCacheButtonHwnd above) is defined later, right after
// FindTaskbarFrameRepeater and IsTaskListButton - it depends on both.
// Forward-declared here so WinEventHookThreadProc (Mod lifecycle section)
// can reference it before that point.
void ResolvePendingButtonHwnds();

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

// A minimized window's GetWindowRect returns a nonsense off-screen position
// (classically around (-32000,-32000)), which would otherwise always
// classify it as "left". Instead, freeze at the last known classification
// from before it was minimized. Not pruned on window close - see note at
// GetButtonHwnd's cache for why that's an acceptable tradeoff here too.
std::unordered_map<HWND, WindowClassification> g_lastKnownWindowClassification;

WindowClassification ClassifyByWindowPositionCached(HWND hwnd) {
    RECT wr;
    HMONITOR mon = MonitorFromWindow(g_hTaskbarWnd, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi{.cbSize = sizeof(mi)};

    if (IsIconic(hwnd)) {
        auto it = g_lastKnownWindowClassification.find(hwnd);
        if (it != g_lastKnownWindowClassification.end()) {
            return it->second;
        }

        // No prior classification to freeze at - this window has been
        // minimized since before we ever saw it (e.g. an app that
        // auto-launches straight to a minimized/tray state, common among
        // startup apps right after login). GetWindowPlacement's
        // rcNormalPosition still reports the window's restored position
        // while minimized (unlike GetWindowRect, which is why the branch
        // above exists at all - it returns nonsense off-screen
        // coordinates for a minimized window), so this can classify it
        // correctly on first sight instead of permanently defaulting.
        WINDOWPLACEMENT wp{.length = sizeof(wp)};
        if (!hwnd || !GetWindowPlacement(hwnd, &wp) ||
            !GetMonitorInfo(mon, &mi)) {
            return {ResolveUnresolvedAppsDefaultSide()};
        }
        wr = wp.rcNormalPosition;
        // rcNormalPosition is in workspace coordinates (relative to
        // rcWork, which excludes the taskbar/appbars), while screenCenterX
        // below is computed from rcMonitor. Identical on a normal
        // bottom-docked taskbar (rcWork.left == rcMonitor.left), but a
        // left-docked appbar shifts rcWork.left right of rcMonitor.left -
        // without this offset a window near the boundary could classify
        // to the wrong side.
        LONG workOffsetX = mi.rcWork.left - mi.rcMonitor.left;
        wr.left += workOffsetX;
        wr.right += workOffsetX;
    } else if (!hwnd || !GetWindowRect(hwnd, &wr) || !GetMonitorInfo(mon, &mi)) {
        return {ResolveUnresolvedAppsDefaultSide()};
    }

    double windowCenterX = (wr.left + wr.right) / 2.0;
    double screenCenterX = (mi.rcMonitor.left + mi.rcMonitor.right) / 2.0;

    WindowClassification result;
    result.side = windowCenterX < screenCenterX ? Side::Left : Side::Right;
    result.distanceFromCenter = std::abs(windowCenterX - screenCenterX);

    g_lastKnownWindowClassification[hwnd] = result;
    return result;
}

// Taskbar buttons expose their app's display name as an accessibility
// property (used by screen readers); reused here as a stable identifier
// that works even for pinned-but-not-running apps, which have no HWND.
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
// window position, then the configured default (mainly hit by pinned apps
// that aren't running and weren't listed in leftApps/rightApps). Skips the
// accessible-name lookup (an AutomationProperties::GetName call plus a
// lowercasing string copy) entirely when there's nothing to match it
// against - leftApps/rightApps are both empty by default, and this runs
// for every task-list button on every single ArrangeOverride pass.
ButtonClassification ClassifyTaskListButton(FrameworkElement element) {
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
        WindowClassification wc = ClassifyByWindowPositionCached(hwnd);
        return {wc.side, wc.distanceFromCenter, true};
    }

    return {ResolveUnresolvedAppsDefaultSide(), PinnedAppOrderKey()};
}

// X coordinate (in the taskbar repeater's local DIPs) of the primary
// monitor's horizontal center.
double GetMonitorCenterXLocal() {
    HMONITOR mon = MonitorFromWindow(g_hTaskbarWnd, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi{.cbSize = sizeof(mi)};
    if (!GetMonitorInfo(mon, &mi)) {
        return 0;
    }

    RECT taskbarRect;
    if (!GetWindowRect(g_hTaskbarWnd, &taskbarRect)) {
        return 0;
    }

    UINT dpi = GetDpiForWindow(g_hTaskbarWnd);
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
    RECT taskbarRect;
    if (!GetWindowRect(g_hTaskbarWnd, &taskbarRect)) {
        return 0;
    }

    UINT dpi = GetDpiForWindow(g_hTaskbarWnd);
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

// Search, Task View and Widgets can each be individually hidden/shown
// using a negative-margin collapse trick - ActualWidth() includes that
// margin, so it never drops below the collapsed width and grows on every
// layout pass once a transition starts. Reads the content child's
// DesiredSize instead (matching taskbar-start-button-position.wh.cpp's
// fix for the same button types), falling back to ActualWidth() if no
// child is realized yet.
//
// Deliberately NOT used for Start or task list buttons: Start is never
// hidden/shown, so it was never exposed to this bug (and swapping its
// width source anyway understated it - see RecomputeLayoutPlan's
// Start-width comment); task list buttons have no evidence of the same
// mechanism and are this file's hottest path.
double SystemButtonContentWidth(FrameworkElement element) {
    if (Media::VisualTreeHelper::GetChildrenCount(element) > 0) {
        auto child = Media::VisualTreeHelper::GetChild(element, 0)
                         .try_as<FrameworkElement>();
        if (child) {
            return child.DesiredSize().Width;
        }
    }
    return element.ActualWidth();
}

// Same margin-inclusive shape as FullFootprintWidth, for the three
// system buttons (Search/Task View/Widgets) that need the
// SystemButtonContentWidth fix above. Start doesn't use this - its own
// width is read bare (no margin added), matching how it was already
// read before this fix, just with ActualWidth() swapped out.
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

int SystemButtonRank(SystemButton b) {
    switch (b) {
        case SystemButton::Search:
            return 0;
        case SystemButton::TaskView:
            return 1;
        case SystemButton::Widgets:
            return 2;
        default:
            return -1;
    }
}

// Total footprint of Search+TaskView+Widgets together, used both to lay
// them out and to reserve room for them next to Start (in adjacent mode).
// Computed unconditionally every RecomputeLayoutPlan pass so an empty
// cluster (all three hidden) reads as genuinely 0 rather than keeping a
// stale nonzero value that would permanently reserve dead space.
double g_lastLeftSystemClusterWidth = 0;
double g_lastRightSystemClusterWidth = 0;

// A repeater child paired with its SystemButton classification, computed
// once per child per RecomputeLayoutPlan pass and reused across the
// Start-finding, cluster-width, and system-button-placement loops rather
// than re-deriving IdentifySystemButton (a winrt::get_class_name call)
// separately in each.
struct ChildInfo {
    FrameworkElement element;
    SystemButton systemButton;
};

// Places Search/TaskView/Widgets either at the taskbar's far left edge, or
// immediately adjacent to one side of the Start button, per
// g_settings.systemButtonsPlacement. clusterWidth is the combined width of
// all three, computed once by the caller (see g_lastLeftSystemClusterWidth's
// comment for why it's no longer computed here as a side effect).
// childInfos is the whole repeater's already-classified children (see
// ChildInfo's comment) - avoids both re-walking the repeater and
// re-deriving each child's SystemButton classification.
double ComputeSystemButtonX(const std::vector<ChildInfo>& childInfos,
                            FrameworkElement targetElement,
                            SystemButton target,
                            double startCenterX,
                            double startWidth,
                            double clusterWidth) {
    int targetRank = SystemButtonRank(target);
    if (targetRank < 0) {
        return 0;
    }

    double widthBefore = 0;
    for (auto& info : childInfos) {
        int r = SystemButtonRank(info.systemButton);
        if (r >= 0 && r < targetRank) {
            widthBefore += SystemButtonFootprintWidth(info.element);
        }
    }

    if (g_settings.systemButtonsPlacement == SystemButtonsPlacement::FarLeft) {
        return kFarLeftSystemButtonMarginPx + widthBefore;
    }

    double gap = g_settings.gapPx;
    double ownWidth = SystemButtonFootprintWidth(targetElement);

    if (g_settings.systemButtonsAdjacentSide == Side::Left) {
        // Stack right-to-left outward from Start: highest rank closest to
        // Start, so reading left-to-right still shows the same low-to-high
        // rank order (Search, Task View, Widgets) as far-left mode does.
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

// Total realized repeater children (every kind - Start, system buttons,
// task list buttons) as of the last successful RecomputeLayoutPlan pass -
// used only by that function's own staleness-backstop check (see there)
// to detect ANY child appearing/disappearing via a single cheap count
// comparison, with no per-child class-name lookup needed. Deliberately
// broader than g_planStats.taskListTotal: a Search/Task View/Widgets
// visibility change also needs to invalidate the plan, not just a task
// list button count change.
thread_local int g_planChildCount;

struct TaskListPlanEntry {
    FrameworkElement element;
    ButtonClassification info;
    double width;
    int index;  // Position within `children`, i.e. taskbar order.
};

// Computes every task list button's target X in one O(n) pass, classifying
// each button exactly once. Every entry is written into outPlan and stays
// in this mod's own coordinate system - never falls through to native
// Arrange, since Start is forced to true center regardless and mixing
// forced- and native-position elements produces a visible overlap.
// Instead, an overflowing side's inter-icon spacing compresses (icons
// stay full width) to fit within leftBoundLocal/rightBoundLocal - the
// taskbar's left edge past the system-button cluster, and the system
// tray's left edge, respectively.
//
// Each side is walked innermost-first from Start's own edge outward,
// using each icon's *unscaled* width for its own placement (only the
// running reference point advances by the scaled amount) - this is what
// guarantees the innermost icon's edge never drifts into Start under
// compression. Scaling the placement itself looks equivalent at scale=1
// but breaks that guarantee - do not reintroduce it.
void PlanTaskListButtons(const std::vector<FrameworkElement>& children,
                         double startCenterX,
                         double leftBoundLocal,
                         double rightBoundLocal,
                         std::unordered_map<void*, double>& outPlan) {
    std::vector<TaskListPlanEntry> entries;
    for (int i = 0; i < (int)children.size(); i++) {
        if (IsTaskListButton(children[i])) {
            entries.push_back({children[i], ClassifyTaskListButton(children[i]),
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

    // Innermost reference point for each side - Start's own edge, minus
    // the gap. Every side's innermost icon is placed exactly here,
    // regardless of compression (see the function comment for why that's
    // the Start-overlap safety guarantee).
    double leftInnerX = startCenterX - startWidth / 2.0 - gap - leftExtra;
    double rightInnerX = startCenterX + startWidth / 2.0 + gap + rightExtra;

    std::vector<TaskListPlanEntry*> left, right;
    for (auto& entry : entries) {
        (entry.info.side == Side::Left ? left : right).push_back(&entry);
    }

    if (g_settings.taskListOrder == TaskListOrder::DistanceFromCenter) {
        // Sort each side closest-to-center-first, i.e. innermost-first -
        // exactly the walk order used below. Ties - e.g. multiple
        // pinned/overridden buttons, which all share the same
        // +/-infinity orderKey - break on taskbar index instead of an
        // ABI-pointer value: equally stable frame to frame, but a
        // predictable order instead of an arbitrary one.
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
        // Preserve taskbar order, read left-to-right across the whole
        // split layout the way native taskbar order reads: on the left
        // side, the earliest entry sits at the outer (leftmost) edge and
        // later entries sit progressively closer to Start; on the right
        // side it's the opposite - the earliest entry sits innermost
        // (right next to Start) and later entries sit progressively
        // farther out. Together these reproduce native left-to-right
        // order across the whole taskbar once Start is inserted in the
        // middle. `left`/`right` are already in taskbar order (entries
        // was built that way); the right side's walk below (accumulating
        // outward from Start) already puts its earliest entry innermost,
        // but the left side needs reversing first - walking it in
        // original taskbar order would put the *earliest* entry innermost
        // instead, backwards from what preserves reading order there
        // (earliest belongs at the outer edge on the left side).
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

    // Scale <1 compresses inter-icon spacing (not each icon's own
    // rendered width) so the outermost icon's own edge never passes
    // leftBoundLocal/rightBoundLocal, at the cost of icons overlapping
    // each other once a side is too full for natural spacing. A side
    // with plenty of room keeps scale at 1.
    //
    // The outermost icon's own width (left.back()/right.back() - the
    // walk loop below always processes them last) is excluded from both
    // the total and the available space fed into the ratio: that icon is
    // placed at full, unscaled width with nothing beyond it to compress
    // against, so folding its width into the compressible pool would
    // double-count it and let its own edge overshoot the bound by
    // width*(1-scale).
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

    // Each icon's own placement uses its unscaled width (x - entry->width,
    // not x - entry->width * leftScale), so the innermost icon's edge
    // lands exactly at leftInnerX/rightInnerX regardless of scale - only
    // x itself (the reference point carried to the next icon) advances by
    // the scaled amount. Scaling the placement itself instead would drift
    // the innermost icon into Start as compression increases.
    // A just-realized button reports ActualWidth()==0 for one pass (only
    // the *previous* arrange pass ever sets it), so entry->width can be 0
    // here for a genuinely brand-new button. Giving it a plan entry in
    // that state would place it exactly on top of its neighbor for one
    // frame. Leaving it out of outPlan entirely instead falls through to
    // native positioning for that one pass (same fallback every element
    // this mod doesn't plan gets) - and since it's then also missing from
    // g_lastArrangedX, RecomputeLayoutPlan's own staleness check (the
    // hash-lookup miss, not even the child-count backstop) forces a real
    // recompute on the very next pass once its real width is available,
    // so this is a one-frame artifact, not a persistent one. Doesn't
    // affect `x`'s advance either way - a zero-width entry doesn't move
    // the reference point regardless of whether it gets a plan entry.
    double x = leftInnerX;
    for (auto* entry : left) {
        if (entry->width > 0) {
            outPlan[winrt::get_abi(entry->element)] = x - entry->width;
        }
        x -= entry->width * leftScale;
    }
    x = rightInnerX;
    for (auto* entry : right) {
        if (entry->width > 0) {
            outPlan[winrt::get_abi(entry->element)] = x;
        }
        x += entry->width * rightScale;
    }
}

// ============================================================================
// XAML hooks
// ============================================================================

// Defined later (Live drag-follow section). Only ever marks the taskbar's
// layout dirty (InvalidateArrange/InvalidateMeasure) - never forces a
// synchronous UpdateLayout() call, which reenters WinUI layout while
// already nested inside it and fails fast with STATUS_STOWED_EXCEPTION.
void InvalidateTaskbarLayout();

// Idle re-check cadence once every cached button is already resolved - an
// ItemsRepeater rebind (e.g. a drag-reorder) can change which item an
// already-resolved element represents without changing the button count,
// so this keeps identity re-checked periodically even at steady state.
// Also used by ResolvePendingButtonHwnds as a fallback delay for passes
// that bail out before confirming the live button set.
constexpr DWORD kIdleResolveTickMs = 30000;

// Defined later (Mod lifecycle section).
void StartWinEventHook();

// Defined later (Mod lifecycle section); lets the ArrangeOverride hook
// request an immediate HWND-resolve attempt (delayMs = 0) as soon as it
// notices the task list button count changed, rather than waiting for the
// timer's own backoff schedule to catch up.
void ArmButtonHwndResolveTimer(DWORD delayMs);

// Defined later (Mod lifecycle section).
DWORD NextResolveDelayMs();

// Defined later (Live drag-follow section). WindhawkUtils::
// SetWindowSubclassFromAnyThread/RemoveWindowSubclassFromAnyThread key the
// subclass by this proc's own address, not by dwRefData (unused, passed
// as 0) - there's no separate id parameter the way raw SetWindowSubclass/
// RemoveWindowSubclass have.
LRESULT CALLBACK TaskbarWndSubclassProc(HWND hWnd,
                                        UINT uMsg,
                                        WPARAM wParam,
                                        LPARAM lParam,
                                        DWORD_PTR dwRefData);

// g_hTaskbarWnd is normally resolved once, in Wh_ModAfterInit. But if
// Windhawk injects into explorer.exe before Shell_TrayWnd has been created
// yet - observed after a fresh boot, never after a manual mod disable/
// re-enable since the taskbar is already fully up by then - that one-shot
// lookup fails and g_hTaskbarWnd stays null forever, which silently
// disables all positioning (IUIElement_Arrange_Hook and the position math
// below both require it). Retried once per ArrangeOverride pass instead
// (see call site) so the mod self-heals as soon as the taskbar exists,
// rather than needing a manual toggle to re-run Wh_ModAfterInit.
HWND EnsureTaskbarWnd() {
    // Shell_TrayWnd can in principle be recreated without explorer.exe
    // itself restarting (not something this mod's own code causes, but
    // not something it can rule out either) - without this check, a
    // stale g_hTaskbarWnd would silently stop all positioning forever
    // (every position-math function requires it) with no self-healing
    // path, since the early return below would never let a fresh
    // FindCurrentProcessTaskbarWnd() attempt happen again. Cheap enough
    // to check unconditionally every pass.
    if (g_hTaskbarWnd && !IsWindow(g_hTaskbarWnd)) {
        g_hTaskbarWnd = nullptr;
        // The old window's subclass chain goes with it (comctl32 tears it
        // down via WM_NCDESTROY, which TaskbarWndSubclassProc already
        // forwards to DefSubclassProc for every message it doesn't
        // otherwise handle) - this just keeps the flag in step so a fresh
        // resolve below knows to install a subclass on the new window
        // rather than assuming the old one still covers it.
        g_taskbarWndSubclassed = false;
    }

    if (g_hTaskbarWnd) {
        return g_hTaskbarWnd;
    }
    if (g_unloading) {
        // Guards against Wh_ModBeforeUninit's own InvalidateTaskbarLayout
        // call (or a naturally-timed pass) landing while the mod's hooks
        // are still installed but g_hTaskbarWnd was somehow never set -
        // without this, resolving it here would call StartWinEventHook()
        // and create a thread with no later teardown call left to stop it
        // (Wh_ModBeforeUninit already ran).
        return nullptr;
    }

    g_hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (g_hTaskbarWnd) {
        Wh_Log(L"Resolved taskbar window: %p", (HWND)g_hTaskbarWnd);
        StartWinEventHook();

        // Lets InvalidateTaskbarLayout/ButtonHwndResolveTimerProc/
        // ApplyLoadedSettings notify this window with a non-blocking
        // PostMessage - see g_taskbarWndSubclassed's own comment for why
        // there's no fallback if this fails. A one-time blocking install
        // here (via SetWindowSubclassFromAnyThread's own marshal, if this
        // isn't already running on the taskbar thread) is fine; it's the
        // per-event cost on the hot invalidate path this is meant to
        // avoid, not one-shot setup.
        if (WindhawkUtils::SetWindowSubclassFromAnyThread(
                g_hTaskbarWnd, TaskbarWndSubclassProc, 0)) {
            g_taskbarWndSubclassed = true;

            // Wh_ModBeforeUninit's own removal pass only runs once, gated
            // by this same g_taskbarWndSubclassed.exchange(false) latch -
            // if it already ran with g_hTaskbarWnd still null (reachable:
            // right after a Shell_TrayWnd recreate, or a fresh-boot
            // startup where the one-shot Wh_ModAfterInit lookup failed,
            // is exactly when EnsureTaskbarWnd can still be resolving
            // this for the first time as unload begins), it can never run
            // again - a subclass installed here after that point would
            // stay wired into Shell_TrayWnd with no later removal call,
            // and Windhawk unmaps this module's code shortly after
            // Wh_ModUninit returns. Re-checking g_unloading immediately
            // after install and undoing it right here closes that window;
            // StartWinEventHook above needs no equivalent recheck since
            // its own start/stop pair is already serialized by
            // g_winEventThreadMutex/g_winEventThreadStopped.
            if (g_unloading && g_taskbarWndSubclassed.exchange(false)) {
                WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                    g_hTaskbarWnd, TaskbarWndSubclassProc);
            } else {
                // With no fallback marshal, a subclass that installs
                // successfully on a LATER call (e.g. after an earlier
                // attempt failed and the taskbar has since recreated)
                // needs an explicit kick - ButtonHwndResolveTimerProc's
                // own self-rearming stopped entirely while unsubclassed
                // (see its comment), so nothing else would restart it.
                // Harmless to call unconditionally on the very first
                // successful resolve too, alongside Wh_ModAfterInit's own
                // InvalidateTaskbarLayout() call right after this returns.
                ArmButtonHwndResolveTimer(0);
                InvalidateTaskbarLayout();
            }
        } else {
            Wh_Log(L"EnsureTaskbarWnd: SetWindowSubclassFromAnyThread "
                   L"failed - live drag-follow, HWND resolution, and live "
                   L"settings updates will be unavailable until the "
                   L"taskbar recreates and a fresh attempt succeeds");
        }
    }

    return g_hTaskbarWnd;
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

// Caches the resolved repeater across calls instead of every caller
// redoing GetTaskbarXamlRoot's resolution chain (taskband HWND -> vtable
// scan -> TaskbarHost::FrameHeight prologue parse -> QueryInterface) plus
// FindTaskbarFrameRepeater's three-level tree walk from scratch - this used
// to run independently in RecomputeLayoutPlan, InvalidateTaskbarLayout, and
// ResolvePendingButtonHwnds on every single call.
//
// A weak_ref alone isn't a reliable "still live" signal: the repeater's
// own layout, ItemsSourceView and the animation machinery can each hold a
// reference of their own, so if any of those outlives the taskbar's tree
// even briefly (a taskbar recreate, a DPI/monitor change), the weak_ref
// still resolves to a now-detached element - one with a null XamlRoot,
// since that's only ever non-null while actually attached to a tree.
// GetCachedTaskbarRepeater checks that explicitly below rather than
// trusting the weak_ref by itself.
//
// MUST only run when confirmed to be on g_hTaskbarWnd's own thread - same
// constraint as GetTaskbarXamlRoot itself, which this wraps.
winrt::weak_ref<FrameworkElement> g_cachedTaskbarRepeaterWeak;

FrameworkElement GetCachedTaskbarRepeater() {
    if (FrameworkElement cached = g_cachedTaskbarRepeaterWeak.get()) {
        if (cached.XamlRoot()) {
            return cached;
        }
        // Detached - see g_cachedTaskbarRepeaterWeak's comment. Every
        // caller of this function eventually dereferences XamlRoot() (or
        // relies on the repeater actually being the live one), so
        // returning it anyway would be a null deref down the line rather
        // than here. Clear it and fall through to a fresh resolution.
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
// whose cache entry is missing, stale (window closed), or past the
// negative-cache TTL - see ResolveAndCacheButtonHwnd's comment for the
// full story on why this runs on a timer instead of inline during Arrange.
//
// g_unloading gates this before anything else: the click-sentinel probe
// only stays safe while CTaskListWnd_HandleClick_Hook is installed to
// intercept it, and that hook (like all of this mod's hooks) can be gone
// before Wh_ModUninit actually stops the timer that calls this - without
// this gate, a probe landing in that window reaches the taskbar's real
// HandleClick with a garbage LauncherOptions pointer (the sentinel
// string reinterpreted as a vtable), an access violation plus a
// genuinely dispatched click. g_unloading is set at the top of
// Wh_ModBeforeUninit, before the hooks are removed, so this closes the
// window regardless of exactly when the timer's next tick lands.
//
// Also guarded against running while nested inside an active Arrange
// pass on this thread - defense in depth, not the primary protection.
// Reentrancy guard: this function runs dispatched from the posted
// ResolveButtonHwndsMsg via TaskbarWndSubclassProc, and it mutates
// g_buttonHwndCache/g_lastKnownWindowClassification with
// erase-while-iterating prune loops. It calls into taskbar.dll and WinRT,
// either of which could pump messages - if another posted
// ResolveButtonHwndsMsg gets dispatched reentrantly while a call is
// already in progress here, a nested call's inserts would invalidate the
// outer loop's iterator - undefined behavior, not just wasted work. RAII rather than a
// plain set/clear, same reasoning as ScopedArrangeOverrideFlag: this
// function has several early returns a plain clear at the end would never
// reach.
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
    // every return path, always on this thread right after this pass's
    // own cache reads/writes are done - computing it from
    // ButtonHwndResolveTimerProc instead would read g_buttonHwndCache
    // concurrently with this function's own insert/erase calls on the
    // taskbar thread, a use-after-free.
    //
    // NextResolveDelayMs' INFINITE answer is only trustworthy once a pass
    // has actually confirmed the live button set - an empty
    // g_buttonHwndCache from a pass that bailed out before enumerating
    // anything (every early-return guard below, or no repeater yet) looks
    // identical to a genuinely empty taskbar otherwise, and the timer
    // would then never be armed again except by incidental luck.
    // `enumerated` tracks whether the walk below actually ran to
    // completion; the destructor falls back to kIdleResolveTickMs instead
    // of trusting INFINITE when it didn't.
    bool enumerated = false;
    struct ScheduleNextResolveTick {
        bool* enumerated;
        ~ScheduleNextResolveTick() {
            DWORD delay =
                *enumerated ? NextResolveDelayMs() : kIdleResolveTickMs;
            if (delay != INFINITE) {
                // Posts to the WinEventHook thread's own queue - safe to
                // call unconditionally, including during unload
                // (ArmButtonHwndResolveTimer itself no-ops on g_unloading).
                ArmButtonHwndResolveTimer(delay);
            }
        }
    } scheduleNextTick{&enumerated};

    if (g_unloading || g_inTaskbarArrangeOverride || !g_hTaskbarWnd) {
        return;
    }

    // Runs dispatched via TaskbarWndSubclassProc, a raw Win32 callback
    // boundary with no C++/WinRT exception translation on the other side,
    // same as IUIElement_Arrange_Hook/RecomputeLayoutPlan/
    // PerformTaskbarLayoutInvalidate. Every WinRT property access below
    // (Content(), ItemsSourceView(), get_class_name, GetName, XamlRoot())
    // can throw winrt::hresult_error, most likely exactly when the taskbar
    // tree is being recreated - an uncaught throw here crosses the boundary
    // and fail-fasts explorer.exe.
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
                // A brand-new button in the live set - even if it fails to
                // resolve an HWND right away (e.g. a just-pinned app with no
                // window yet), its mere appearance means g_lastArrangedX has
                // no entry for it and the plan needs rebuilding regardless of
                // whether ResolveAndCacheButtonHwnd's own HWND-changed check
                // fires below.
                anyChanged = true;
            }
            if (!needsResolve) {
                // Identity mismatch means ItemsRepeater rebound this element
                // to a different item since we last resolved it - see
                // ButtonHwndCacheEntry's comment. Checked on both branches
                // below, not just the resolved one: a rebind onto a
                // negatively-cached element (e.g. drag-reordering a pinned-
                // not-running app past a running one) is just as real a
                // rebind, and without this check here it would inherit the
                // old entry's accumulated backoff instead of resolving.
                if (it->second.hwnd) {
                    // The old HWND is still a perfectly valid window, just
                    // no longer this element's, so IsWindow() alone can't
                    // catch a rebind.
                    needsResolve = !IsWindow(it->second.hwnd) ||
                                   identity != it->second.identity;
                } else {
                    // forceResolve: a negatively-cached element's own
                    // consecutiveFailures keeps accumulating for as long as
                    // its button exists (which, for a pinned-but-not-running
                    // app, is the entire session) - by the time it's
                    // actually launched, the backoff could be minutes away,
                    // silently defeating EVENT_OBJECT_SHOW's whole purpose
                    // of triggering an immediate resolve. Bounded to
                    // consecutiveFailures < kMaxForcedRetryFailures - see
                    // its own comment for why an unconditional force here
                    // was a real bug, not just belt-and-suspenders.
                    bool backoffElapsed =
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

        // Prune g_buttonHwndCache entries for buttons that no longer exist.
        // XAML routinely destroys and recreates TaskListButtons (unpin, app
        // close, virtualization), and the allocator can reuse a destroyed
        // element's address for a later, unrelated one - without this, that
        // new element would silently inherit the destroyed one's cached HWND
        // (the cache is keyed by raw ABI pointer with no reference held, so
        // there's no way to detect this other than checking against a fresh
        // enumeration like this one). g_lastArrangedX doesn't need the same
        // treatment - RecomputeLayoutPlan already rebuilds it from scratch
        // every ArrangeOverride pass, so a stale entry there can never
        // outlive one pass regardless.
        for (auto it = g_buttonHwndCache.begin(); it != g_buttonHwndCache.end();) {
            if (liveTaskListButtons.find(it->first) == liveTaskListButtons.end()) {
                // A button disappeared (unpin, app close) - its old absolute X
                // in g_lastArrangedX would otherwise sit stale (nothing else
                // occupies that slot), leaving a hole where it used to be.
                it = g_buttonHwndCache.erase(it);
                anyChanged = true;
            } else {
                it = std::next(it);
            }
        }

        // Same idea one level up: g_lastKnownWindowClassification is keyed by
        // HWND, which Windows also recycles, and it's only ever consulted for
        // a minimized window's frozen side (ClassifyByWindowPositionCached) -
        // so a brand-new window could otherwise inherit a closed window's
        // stale classification.
        for (auto it = g_lastKnownWindowClassification.begin();
             it != g_lastKnownWindowClassification.end();) {
            it = IsWindow(it->first) ? std::next(it)
                                      : g_lastKnownWindowClassification.erase(it);
        }

        // Rebuild the published resolved-HWND set from g_buttonHwndCache's
        // just-finished state, for WinEventProc's drag-follow filter (see
        // g_resolvedHwnds' own comment) - unconditionally, not just when
        // anyChanged, since it's cheap and needs to reflect this pass's
        // pruning even when nothing else about the plan changed.
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
// element's ABI pointer (same identity technique as g_buttonHwndCache).
// Written only by RecomputeLayoutPlan, which rebuilds this map from
// scratch whenever g_planDirty says something might have changed since
// the last rebuild (see its own comment) - so IUIElement_Arrange_Hook
// below never needs to compute anything itself, only look a value up. An
// element with no entry (a secondary-monitor element, which
// RecomputeLayoutPlan never walks; or a primary element so new it wasn't
// realized yet when this pass's plan was built) falls through to
// Windows' own native positioning for that one pass, same as any element
// this mod doesn't touch at all.
std::unordered_map<void*, double> g_lastArrangedX;

// Set whenever something might have changed that g_lastArrangedX doesn't
// reflect yet - a window moving, a button's HWND/side resolving, the
// ArrangeOverride hook's button-count-change check, a settings change, or
// mod startup, each setting it at the point that specific change actually
// becomes visible on the taskbar thread (see InvalidateTaskbarLayout's own
// comment for why it can't be set any earlier, at the calling thread's
// call site). Starts true so the first ArrangeOverride pass always
// computes a real plan. RecomputeLayoutPlan clears it only after a
// genuinely successful recompute - left set on an exception so a later
// pass retries rather than freezing on a broken plan. Every read and write
// now happens on the taskbar thread only, so plain bool would be
// sufficient - kept atomic as low-risk insurance rather than downgrading
// it while touching this area for an unrelated fix.
//
// This flag skips RecomputeLayoutPlan's full traversal (a taskbar.dll
// vtable scan, a VisualTreeHelper walk, a classification per child) on
// the very common passes where nothing this mod cares about changed -
// a pure short-circuit around already-correct plan-computation logic,
// not a rewrite of it.
std::atomic<bool> g_planDirty{true};

// Staleness backstop for g_planDirty: some real triggers for a layout
// change (Search/Task View/Widgets visibility, taskbar geometry/DPI/
// monitor changes, a button appearing/disappearing between two dirty
// passes with nothing else invalidating in between) aren't guaranteed to
// call InvalidateTaskbarLayout. Rather than track every such trigger
// individually, RecomputeLayoutPlan forces a real recompute at least this
// often regardless of g_planDirty, bounding how long the plan can disagree
// with reality instead of only reacting to known triggers.
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

    // This hook replaces the process-wide IUIElement::Arrange vtable slot -
    // a raw ABI boundary with no C++/WinRT exception translation, so an
    // uncaught throw here fail-fasts the whole process. The QueryInterface
    // below is the only WinRT call left in this function; everything else
    // is a plain map lookup.
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
// PRIMARY taskbar's own repeater - every Start/system-button/task-list-
// button's target X, written into g_lastArrangedX - called from
// TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Hook BEFORE XAML's
// own ArrangeOverride, i.e. before any nested Arrange calls (and so
// IUIElement_Arrange_Hook) run at all this pass - see
// g_inTaskbarArrangeOverride's comment for why that ordering matters.
//
// Rebuilds g_lastArrangedX from scratch every recompute, so a destroyed
// element's stale entry can never outlive it - no pruning pass needed
// here, unlike g_buttonHwndCache/g_lastKnownWindowClassification (see
// ResolvePendingButtonHwnds for why those need it).
//
// MUST only run when confirmed to be on g_hTaskbarWnd's own thread:
// GetTaskbarXamlRoot reaches the PRIMARY's XAML object specifically, an
// unsafe unmarshaled cross-apartment call from any other thread.
// Shell_TrayWnd and Shell_SecondaryTrayWnd share one Explorer UI thread,
// so this check also passes for secondary-monitor passes - harmless,
// since secondary-monitor elements never end up in g_lastArrangedX
// regardless; the check is purely the apartment-safety guard, not a
// monitor-scoping mechanism.
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
        // This backstop only bounds staleness while ArrangeOverride keeps
        // getting called at least this often - a change landing in a
        // skipped pass on an otherwise-idle desktop (e.g. a pin/unpin)
        // could sit stale indefinitely otherwise. GetCachedTaskbarRepeater
        // makes checking the realized child set against what the plan
        // actually covers affordable every time this branch is taken - if
        // any live task list button isn't in g_lastArrangedX, or the live
        // child count doesn't match the last recompute's, the plan is
        // stale regardless of the dirty flag or the clock.
        //
        // Hash lookup first, IsTaskListButton (winrt::get_class_name - a
        // GetRuntimeClassName round trip plus an HSTRING allocation)
        // second, short-circuited via && - every element the plan covers
        // (Start and the system buttons too, not just task list buttons)
        // is already a key in g_lastArrangedX, so the common "nothing
        // changed" case never pays for a single class-name lookup here.
        // IsTaskListButton only runs at all on a hash-lookup miss, i.e.
        // a genuinely new child.
        bool planIsCurrent = true;
        try {
            if (FrameworkElement repeater = GetCachedTaskbarRepeater()) {
                int liveChildCount = 0;
                for (auto& child : GetRepeaterChildElements(repeater)) {
                    liveChildCount++;
                    if (!g_lastArrangedX.count(winrt::get_abi(child)) &&
                        IsTaskListButton(child)) {
                        // A live task list button the plan doesn't cover
                        // yet (just realized after the last recompute).
                        // Scoped to task list buttons specifically since
                        // a brand-new system button reaching this branch
                        // isn't otherwise possible - Search/Task View/
                        // Widgets/Start don't get created or destroyed
                        // the way task list buttons do - so there's
                        // nothing to gain from paying for the class-name
                        // check on a system-button miss too.
                        planIsCurrent = false;
                        break;
                    }
                }
                // A child *disappearing* (unpin, app closed, or a system
                // button's visibility changing) adds nothing new to
                // g_lastArrangedX's coverage - every remaining live child
                // is still a key in it - so the loop above alone can't
                // catch it, and a removed task list button would
                // otherwise sit at its old X (a visible hole) until the
                // resolve timer's own prune eventually invalidates, up to
                // kIdleResolveTickMs or the backoff schedule away. This
                // count comparison against g_planChildCount (every child
                // kind, not just task list buttons - see its own comment)
                // catches that direction too, for free, off the same walk.
                if (planIsCurrent && liveChildCount != g_planChildCount) {
                    planIsCurrent = false;
                }
            }
        } catch (...) {
            // Conservative default: treat an exception here as "can't
            // confirm the plan is current" and fall through to the real
            // recompute below, which has its own exception handling.
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

        // Classify each child's SystemButton status exactly once - see
        // ChildInfo's comment for why.
        std::vector<ChildInfo> childInfos;
        childInfos.reserve(children.size());
        for (auto& child : children) {
            childInfos.push_back({child, IdentifySystemButton(child)});
        }

        // Start first: ComputeSystemButtonX/PlanTaskListButtons below
        // both read g_lastStartWidth, so it needs to already reflect this
        // pass by the time they run, not the previous one.
        for (auto& info : childInfos) {
            if (info.systemButton == SystemButton::Start) {
                // Deliberately bare ActualWidth(), NOT
                // SystemButtonContentWidth: Start is never hidden/shown
                // the way Search/Task View/Widgets are, so it was never
                // exposed to ActualWidth()'s collapse-margin growth
                // problem (see SystemButtonContentWidth's comment) -
                // swapping to the content child's DesiredSize() here
                // instead understated Start's true width, since Start's
                // own visual-tree shape doesn't match what that technique
                // was validated against.
                //
                // ActualWidth() still reflects the previous arrange pass
                // and is 0 for a just-realized element, so
                // g_lastStartWidth only updates when it's actually
                // positive - a freshly (re)created Start button keeps the
                // last known-good width instead of collapsing the whole
                // taskbar around a zero-width Start for one pass.
                double w = info.element.ActualWidth();
                if (w > 0) {
                    g_lastStartWidth = w;
                }
                // The X handed to Arrange sets the layout slot's left
                // edge; XAML then insets the rendered content by the
                // element's own Margin.Left. Without subtracting it here,
                // Start's rendered icon centers at startCenterX +
                // Margin.Left instead of true screen-center - a no-op
                // when the margin happens to be 0, but wrong otherwise.
                newPlan[winrt::get_abi(info.element)] =
                    startCenterX - info.element.Margin().Left -
                    g_lastStartWidth / 2.0;
                break;
            }
        }

        // Search/TaskView/Widgets next: PlanTaskListButtons reads
        // g_lastLeftSystemClusterWidth/g_lastRightSystemClusterWidth, so
        // these need to be set - unconditionally, every pass, so a
        // cluster that's empty this pass reads as genuinely 0 rather than
        // keeping a stale value from an earlier one (see
        // g_lastLeftSystemClusterWidth's comment) - before any task list
        // button below runs for the same reason.
        double systemClusterWidth = 0;
        for (auto& info : childInfos) {
            if (SystemButtonRank(info.systemButton) >= 0) {
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
                childInfos, info.element, info.systemButton, startCenterX,
                g_lastStartWidth, systemClusterWidth);
        }

        // Bounds each task list group is compressed to fit within - see
        // PlanTaskListButtons' own comment for what these mean and why.
        double leftBoundLocal =
            (g_settings.systemButtonsPlacement == SystemButtonsPlacement::FarLeft)
                ? (kFarLeftSystemButtonMarginPx + systemClusterWidth)
                : 0;

        // The system tray/clock (SystemTray.SystemTrayFrame in this
        // taskbar's own XAML tree, a sibling of Taskbar.TaskbarFrame
        // under the same root content) is a real, measurable bound,
        // unlike the taskbar's own outer edge - the tray only occupies
        // the last portion of the taskbar's full width, so bounding
        // against the whole taskbar barely constrains anything in
        // practice (confirmed via live testing: icons still visibly ran
        // under the tray with that bound). Falls back to the taskbar's
        // own width only if the tray frame can't be resolved this pass.
        FrameworkElement systemTrayFrame =
            FindChildByClassName(content, L"SystemTray.SystemTrayFrame");
        double rightBoundLocal =
            systemTrayFrame
                ? (GetElementLeftXLocal(systemTrayFrame, content) -
                   kTrayMarginPx)
                : GetTaskbarWidthLocal();

        // Task list buttons last - see PlanTaskListButtons' own comment
        // for why this is a single O(n) pass over `children` rather than
        // calling a per-button compute function in a loop here.
        PlanTaskListButtons(children, startCenterX, leftBoundLocal,
                            rightBoundLocal, newPlan);

        g_lastArrangedX = std::move(newPlan);
        g_planChildCount = (int)children.size();
        g_planDirty = false;
    } catch (...) {
        g_planStats.exceptions++;
        // g_lastArrangedX is left as whatever the last successful pass
        // produced - IUIElement_Arrange_Hook's lookup-or-fall-through
        // handles a stale/incomplete plan exactly like it already handles
        // a brand-new not-yet-planned element. g_planDirty deliberately
        // stays true (not cleared) here, unlike the success path above -
        // this pass didn't actually produce a plan reflecting current
        // state, so a later pass should retry rather than treat this as
        // "up to date" and skip forever.
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

    // Builds this pass's whole plan up front - see RecomputeLayoutPlan's
    // comment for why this ordering (before the original ArrangeOverride,
    // so before any nested Arrange calls) is what keeps this safe.
    RecomputeLayoutPlan();

    // The button count can change without any window moving (new pin, app
    // launched/closed). If RecomputeLayoutPlan's repeater walk ran before
    // XAML had realized a just-inserted button yet, this pass's plan has
    // no entry for it and it renders at its native position for one pass.
    // Self-correct by invalidating whenever the observed count changes,
    // and arm an immediate HWND-resolve attempt, since a newly-inserted
    // button is exactly what ResolvePendingButtonHwnds' cache-only view
    // (NextResolveDelayMs) can't see coming on its own.
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

    // RAII rather than a plain set/clear pair around the original call:
    // if that call ever exited non-locally, a plain clear below it would
    // never run, leaving this stuck true and routing every later Arrange
    // on this thread through the plan lookup regardless of context.
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
            L"right=%d) planExceptions=%d | winEvents: raw=%d "
            L"invalidated=%d skippedReentrant=%d invalidateExceptions=%d | "
            L"resolve: ok=%d fail=%d",
            g_passStats.totalArrangeCalls, g_passStats.repositioned,
            g_passStats.qiFailures, g_passStats.exceptions,
            g_planStats.taskListTotal, g_planStats.taskListHwndResolved,
            g_planStats.taskListLeft, g_planStats.taskListRight,
            g_planStats.exceptions,
            (int)g_winEventRawCount,
            (int)g_winEventInvalidateCount, (int)g_invalidateSkippedReentrant,
            (int)g_invalidateExceptions, g_resolveStats.success,
            g_resolveStats.failure);
        g_resolveStats = {};
    }

    return ret;
}

// ============================================================================
// Live drag-follow: force a taskbar relayout when a top-level window moves
// ============================================================================

// Guards against reentering the invalidate body itself on the taskbar's
// own thread. WinEventProc can fire in rapid bursts (observed: thousands
// of raw events within seconds while something on screen is spamming
// EVENT_OBJECT_LOCATIONCHANGE), each one posting another
// InvalidateTaskbarLayoutMsg dispatched via TaskbarWndSubclassProc - a
// nested WinRT/taskbar.dll call inside this function's own body (GetCached
// TaskbarRepeater, InvalidateArrange/InvalidateMeasure) could pump
// messages and let a second posted message land reentrantly on this same
// thread before the first call returns.
//
// This deliberately never calls UpdateLayout() - see the note on
// InvalidateTaskbarLayout below for why forcing a synchronous layout pass
// here is unsafe regardless of this guard.
thread_local bool g_inInvalidateTaskbarLayout;

// The actual invalidate work, shared by both of InvalidateTaskbarLayout's
// call paths below. MUST only run on g_hTaskbarWnd's own thread - same
// constraint as GetCachedTaskbarRepeater, which this uses.
void PerformTaskbarLayoutInvalidate() {
    if (g_inInvalidateTaskbarLayout) {
        g_invalidateSkippedReentrant++;
        return;
    }
    g_inInvalidateTaskbarLayout = true;

    // Set here, not in InvalidateTaskbarLayout - see that function's own
    // comment for why marking dirty has to happen at the point a change
    // actually becomes visible on the taskbar thread, not at the calling
    // thread's call site.
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
// blocking the caller - see InvalidateTaskbarLayout's own comment for why
// that matters on this specific call path. Function-local static rather
// than a namespace-scope variable, so RegisterWindowMessage runs lazily on
// first real use instead of from a dynamic initializer under DllMain's
// loader lock.
UINT InvalidateTaskbarLayoutMsg() {
    static const UINT msg =
        RegisterWindowMessage(L"Windhawk_InvalidateTaskbarLayout_" WH_MOD_ID);
    return msg;
}

// Same idea as InvalidateTaskbarLayoutMsg, for ButtonHwndResolveTimerProc
// (up to ~7/sec while EVENT_OBJECT_SHOW events are bursting).
UINT ResolveButtonHwndsMsg() {
    static const UINT msg =
        RegisterWindowMessage(L"Windhawk_ResolveButtonHwnds_" WH_MOD_ID);
    return msg;
}

// Same idea again, for a settings change. Unlike the two above this one
// carries a payload: lParam is a heap-allocated ModSettings* that the
// dispatch case below takes ownership of and deletes - PostMessage is
// asynchronous, so the settings can't just live on the posting call's own
// stack the way a plain signal can. See ApplyLoadedSettings.
UINT SettingsChangedMsg() {
    static const UINT msg =
        RegisterWindowMessage(L"Windhawk_SettingsChanged_" WH_MOD_ID);
    return msg;
}

// Installed on g_hTaskbarWnd by EnsureTaskbarWnd via
// SetWindowSubclassFromAnyThread. A subclass proc only ever runs on the
// thread that owns the window, which is what lets these call paths use a
// plain, non-blocking PostMessage - no per-call SetWindowsHookEx/
// SendMessage/UnhookWindowsHookEx dance needed. This is also the SOLE way
// any of these three messages reach the taskbar thread; if this subclass
// never installs (see g_taskbarWndSubclassed's own comment), there is no
// fallback. Everything other than these private messages is forwarded to
// DefSubclassProc, which is also what lets comctl32 clean this subclass up
// automatically via WM_NCDESTROY if the window is ever destroyed out from
// under the mod without Wh_ModBeforeUninit's explicit removal call running
// first.
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
    if (uMsg == SettingsChangedMsg()) {
        std::unique_ptr<ModSettings> heapSettings(
            reinterpret_cast<ModSettings*>(lParam));
        g_settings = std::move(*heapSettings);
        // The plan the previous recompute produced was built from the
        // settings that just got replaced - see InvalidateTaskbarLayout's
        // comment for why this needs to be set exactly here, not by the
        // separate InvalidateTaskbarLayout() call Wh_ModSettingsChanged
        // also makes.
        g_planDirty = true;
        return 0;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Lets the XAML dispatcher pick up a relayout on its own next tick rather
// than forcing a synchronous UpdateLayout() call: callers include a raw
// OS callback (WinEventProc) that can fire while already nested inside
// XAML-internal layout activity, and a forced UpdateLayout() there
// reenters WinUI layout and fails fast with STATUS_STOWED_EXCEPTION - a
// raw SEH RaiseException, not a C++ exception, so no try/catch anywhere
// in this file can contain it. Don't reintroduce a forced call here
// without a fundamentally different (genuinely async, post-unwind-only)
// mechanism.
//
// Deliberately does NOT set g_planDirty itself: the marshal below is
// usually asynchronous (PostMessage), so a natural ArrangeOverride pass
// could land on the taskbar thread between this call setting the flag and
// the posted message being processed, see it already true, recompute
// with stale state, and clear the flag - leaving the posted invalidate
// with nothing to do. Each real caller marks dirty itself instead, at the
// point its own state change becomes visible on the taskbar thread:
// PerformTaskbarLayoutInvalidate and the SettingsChangedMsg dispatch case
// in TaskbarWndSubclassProc.
void InvalidateTaskbarLayout() {
    // Snapshot once so every read below agrees, even if EnsureTaskbarWnd
    // writes a new value on another thread mid-function - see
    // g_hTaskbarWnd's own comment for why that matters here specifically
    // (a stale nullptr reaching PostMessage is not a no-op).
    HWND hTaskbarWnd = g_hTaskbarWnd;
    // No fallback if the subclass never installed - see
    // g_taskbarWndSubclassed's own comment for why. Called from
    // WinEventProc's drag-follow throttle at up to ~7 times/sec while any
    // window on the system is being dragged, plus every resolve tick, so
    // this is a hot path - PostMessage doesn't block on Explorer's UI
    // thread pumping it.
    if (!hTaskbarWnd || !g_taskbarWndSubclassed) {
        return;
    }
    if (!PostMessage(hTaskbarWnd, InvalidateTaskbarLayoutMsg(), 0, 0)) {
        Wh_Log(L"InvalidateTaskbarLayout: PostMessage failed, error=%lu",
               GetLastError());
    }
}

// One-shot: fires once the throttle window below has gone quiet, applying
// whatever position a drag/move most recently landed on. See its arm site
// in WinEventProc for why this exists.
void CALLBACK DragFollowTrailingTimerProc(HWND hwnd,
                                          UINT uMsg,
                                          UINT_PTR idEvent,
                                          DWORD dwTime) {
    KillTimer(nullptr, idEvent);
    g_dragFollowTrailingTimerId = 0;

    // Same g_unloading check WinEventProc already has: on the fallback
    // path (subclass never installed), InvalidateTaskbarLayout blocks in
    // SendMessage until the taskbar thread pumps it, which
    // StopWinEventHook's now-unconditional wait (see its own comment)
    // would otherwise be stuck behind if this fired mid-teardown.
    if (g_unloading) {
        return;
    }

    // This fire is itself an invalidate - see g_lastDragFollowInvalidate's
    // comment for why WinEventProc's throttle needs to know about it too.
    g_lastDragFollowInvalidate = GetTickCount64();

    g_winEventInvalidateCount++;
    InvalidateTaskbarLayout();
}

void CALLBACK WinEventProc(HWINEVENTHOOK hook,
                           DWORD event,
                           HWND hwnd,
                           LONG idObject,
                           LONG idChild,
                           DWORD idEventThread,
                           DWORD dwmsEventTime) {
    g_winEventRawCount++;

    if (g_unloading || idObject != OBJID_WINDOW || idChild != CHILDID_SELF ||
        !hwnd || hwnd == g_hTaskbarWnd) {
        return;
    }

    // For LOCATIONCHANGE specifically, this is the cheapest and most
    // selective filter available - only a window this mod has actually
    // resolved to a taskbar button can change the layout, and checking a
    // hashset first skips the three cross-process USER32 calls below for
    // every other moving window on the system (there are a lot of them -
    // see this function's own comment on raw event volume further down).
    // Not applied to SHOW: a newly-shown window that's about to become a
    // taskbar button's target is, by definition, not resolved yet.
    if (event == EVENT_OBJECT_LOCATIONCHANGE) {
        std::lock_guard<std::mutex> guard(g_resolvedHwndsMutex);
        if (!g_resolvedHwnds.count(hwnd)) {
            return;
        }
    }

    if (!IsWindowVisible(hwnd) || GetAncestor(hwnd, GA_ROOT) != hwnd ||
        GetWindow(hwnd, GW_OWNER) != nullptr) {
        return;
    }

    if (event == EVENT_OBJECT_SHOW) {
        // A real top-level window (per the filtering above) becoming
        // visible is what a pinned-but-not-running app launching looks
        // like - nudge the resolve timer to run right away, and force it
        // to ignore each negatively-cached entry's own backoff (see
        // g_forceResolveUnresolved's comment for why arming the timer
        // alone isn't enough), instead of leaving it to the backoff
        // schedule. That schedule is kept as a fallback - this is a fast
        // path on top of it, not a replacement for it, in case a launch
        // is ever reached through a code path that legitimately doesn't
        // produce this event.
        //
        // Leading-edge throttled the same way the location-change branch
        // below is: this event fires for every top-level window becoming
        // visible anywhere on the system, unthrottled, so an app that
        // opens several windows at once (or a burst of unrelated launches)
        // would otherwise schedule a full resolve pass - a blocking
        // marshal onto Explorer's UI thread plus a repeater walk and an
        // AutomationProperties::GetName per button - for every single one.
        // No trailing timer is needed the way drag-follow has one: unlike
        // a window's final drop position, arm(0) just needs to run once to
        // pick up every currently-pending button, so missing the last
        // event in a burst costs nothing.
        ULONGLONG nowShow = GetTickCount64();
        if (nowShow - g_lastShowEventArm < 150) {
            return;
        }
        g_lastShowEventArm = nowShow;
        g_forceResolveUnresolved = true;
        ArmButtonHwndResolveTimer(0);
        return;
    }

    // event == EVENT_OBJECT_LOCATIONCHANGE from here on - drag-follow.
    // (Already filtered against g_resolvedHwnds above, before the
    // top-level-window checks.)
    ULONGLONG now = GetTickCount64();
    if (now - g_lastDragFollowInvalidate < 150) {
        // The throttle above is leading-edge only, so the final
        // location-change event of a drag/move - the one carrying its
        // actual release position - is routinely the one that lands
        // inside this window and gets dropped, since a drag generates a
        // continuous event stream right up to release. Without this, the
        // icon can be left classified by a stale mid-drag position until
        // some unrelated window happens to move. Re-arm a short one-shot
        // timer on every throttled event so it always fires once the
        // burst actually goes quiet, applying the final position. Runs on
        // this same dedicated WinEventHook thread (see
        // g_dragFollowTrailingTimerId's comment).
        if (g_dragFollowTrailingTimerId) {
            KillTimer(nullptr, g_dragFollowTrailingTimerId);
        }
        g_dragFollowTrailingTimerId =
            SetTimer(nullptr, 0, 200, DragFollowTrailingTimerProc);
        return;
    }
    g_lastDragFollowInvalidate = now;
    if (g_dragFollowTrailingTimerId) {
        KillTimer(nullptr, g_dragFollowTrailingTimerId);
        g_dragFollowTrailingTimerId = 0;
    }

    g_winEventInvalidateCount++;
    InvalidateTaskbarLayout();
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
    Wh_Log(L"  TaskItem::ReportClicked: %s",
           TaskItem_ReportClicked_Original ? L"resolved" : L"MISSING");
    Wh_Log(L"  CTaskGroup::GetNumItems: %s",
           CTaskGroup_GetNumItems_Original ? L"resolved" : L"MISSING");
    Wh_Log(L"  TaskGroup::ReportClicked: %s",
           TaskGroup_ReportClicked_Original ? L"resolved" : L"MISSING");
    return ok;
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // All marked optional so HookSymbols doesn't abort the whole batch on
    // the first miss - we want a per-symbol report while bringing this mod
    // up on a new Windows build, not just a single opaque FAILED. Which of
    // the two modules below is actually loaded depends on the Windows
    // build - see GetTaskbarViewModuleHandle.
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            // Deliberately NOT optional, unlike every other entry below:
            // this hook is the mod's entire function - if it can't be
            // resolved, positioning silently does nothing. Every other
            // symbol here is optional purely so HookSymbols doesn't abort
            // the whole batch on the first miss (a per-symbol report is
            // more useful than one opaque FAILED while bringing this mod
            // up on a new Windows build), but a build where this one is
            // missing should surface as a real failure in Windhawk, not a
            // mod that "loads successfully" and does nothing.
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
    };

    bool ok = HookSymbols(module, hooks, ARRAYSIZE(hooks));
    Wh_Log(L"HookTaskbarViewDllSymbols: %s", ok ? L"OK" : L"FAILED");
    Wh_Log(L"  ArrangeOverride: %s",
           TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Original ? L"resolved"
                                                                        : L"MISSING");
    Wh_Log(L"  TryGetItemFromContainer<TaskListWindowViewModel>: %s",
           TryGetItemFromContainer_TaskListWindowViewModel_Original ? L"resolved"
                                                                     : L"MISSING");
    Wh_Log(L"  TaskListWindowViewModel::get_TaskItem: %s",
           TaskListWindowViewModel_get_TaskItem_Original ? L"resolved"
                                                          : L"MISSING");
    Wh_Log(L"  TryGetItemFromContainer<TaskListGroupViewModel>: %s",
           TryGetItemFromContainer_TaskListGroupViewModel_Original ? L"resolved"
                                                                    : L"MISSING");
    Wh_Log(L"  TaskListGroupViewModel::IsMultiWindow: %s",
           TaskListGroupViewModel_IsMultiWindow_Original ? L"resolved"
                                                          : L"MISSING");
    Wh_Log(L"  ITaskGroup::IsRunning: %s",
           ITaskGroup_IsRunning_Original ? L"resolved" : L"MISSING");
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
        // own return value: that value reflects whether EVERY symbol in
        // its table resolved, optional ones included, so a single missing
        // optional HWND-resolution symbol - exactly the case that table's
        // `optional = true` entries exist to tolerate - would otherwise
        // skip applying hooks for every symbol that DID resolve, ArrangeOverride
        // included. The per-symbol resolved/MISSING logging already
        // reports what to investigate; there's nothing to gain from also
        // discarding the hooks that succeeded.
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

// EVENT_OBJECT_LOCATIONCHANGE fires for every window move on the entire
// system - "thousands of raw events within seconds" has been observed -
// and EVENT_OBJECT_SHOW (added for the resolve-timer fast path, see
// WinEventProc) is nearly as frequent. WINEVENT_OUTOFCONTEXT delivers
// callbacks on whichever thread called SetWinEventHook, and only if that
// thread pumps messages; registering on g_hTaskbarWnd's own thread would
// put all of that - plus WinEventProc's own filtering calls on each one -
// in direct contention with the shell's own layout work. This dedicated
// mod-owned thread (the pattern taskbar-background-helper.wh.cpp and
// taskbar-auto-hide-when-maximized.wh.cpp both use) keeps it off the
// shell's thread entirely.
//
// kArmResolveNowMsg (like WM_APP below) is a private signal on this
// thread's own message queue: the resolve timer lives here too (see
// g_buttonHwndResolveTimerId), but callers of ArmButtonHwndResolveTimer
// run on the taskbar thread, and a per-thread SetTimer/KillTimer can only
// be touched from the thread that owns it - PostThreadMessage is how a
// different thread asks this one to do that on its behalf.
constexpr UINT kArmResolveNowMsg = WM_APP + 1;

// Defined later in this section, alongside NextResolveDelayMs -
// forward-declared here so WinEventHookThreadProc (right below) can
// start/re-arm it directly on its own thread.
void CALLBACK ButtonHwndResolveTimerProc(HWND hwnd,
                                         UINT uMsg,
                                         UINT_PTR idEvent,
                                         DWORD dwTime);

DWORD WINAPI WinEventHookThreadProc(LPVOID) {
    // Forces this thread's message queue into existence before
    // SetWinEventHook runs, so WINEVENT_OUTOFCONTEXT has somewhere to
    // deliver callbacks to as soon as registration succeeds, rather than
    // racing the GetMessage loop below into existence.
    MSG msg;
    PeekMessage(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    // Deliberately NOT using WINEVENT_SKIPOWNPROCESS: File Explorer
    // windows often run inside explorer.exe's own process, and that flag
    // would silently drop their location-change events too. The taskbar's
    // own windows are already excluded explicitly in WinEventProc.
    g_locationChangeHook = SetWinEventHook(
        EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE, nullptr,
        WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    Wh_Log(L"WinEventHookThreadProc: handle=%p (dedicated thread %lu)",
           g_locationChangeHook, GetCurrentThreadId());
    if (!g_locationChangeHook) {
        Wh_Log(L"Failed to register location-change hook - live "
               L"drag-follow will not work, but everything else still "
               L"will");
    }

    // A second, separate hook rather than widening the range above: the
    // two event IDs aren't adjacent, and a single range spanning both
    // would also pick up several unrelated event types in between. See
    // WinEventProc's own EVENT_OBJECT_SHOW branch for what this is for.
    g_showEventHook = SetWinEventHook(EVENT_OBJECT_SHOW, EVENT_OBJECT_SHOW,
                                      nullptr, WinEventProc, 0, 0,
                                      WINEVENT_OUTOFCONTEXT);
    if (!g_showEventHook) {
        Wh_Log(L"Failed to register show-event hook - a newly launched "
               L"pinned app's icon will only start following it once the "
               L"resolve timer's own backoff schedule catches up, rather "
               L"than immediately");
    }

    // Kicks off the first HWND-resolve attempt shortly after this thread
    // (and so the taskbar window) is up, so buttons already present at
    // mod startup get picked up - after that, NextResolveDelayMs and
    // ArmButtonHwndResolveTimer keep it armed only for as long as
    // there's actually something to do.
    g_buttonHwndResolveTimerId =
        SetTimer(nullptr, 0, 100, ButtonHwndResolveTimerProc);

    // WM_APP (posted by StopWinEventHook) is this thread's shutdown
    // signal - it has no window to route to, so it's read directly out of
    // the queue rather than via a window procedure.
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_APP) {
            break;
        }
        if (msg.message == kArmResolveNowMsg) {
            KillTimer(nullptr, g_buttonHwndResolveTimerId);
            g_buttonHwndResolveTimerId =
                SetTimer(nullptr, 0, (UINT)msg.lParam, ButtonHwndResolveTimerProc);
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_locationChangeHook) {
        UnhookWinEvent(g_locationChangeHook);
        g_locationChangeHook = nullptr;
    }
    if (g_showEventHook) {
        UnhookWinEvent(g_showEventHook);
        g_showEventHook = nullptr;
    }
    if (g_buttonHwndResolveTimerId) {
        KillTimer(nullptr, g_buttonHwndResolveTimerId);
        g_buttonHwndResolveTimerId = 0;
    }
    if (g_dragFollowTrailingTimerId) {
        KillTimer(nullptr, g_dragFollowTrailingTimerId);
        g_dragFollowTrailingTimerId = 0;
    }

    return 0;
}

HANDLE g_winEventThread;
// Written under g_winEventThreadMutex (Start/StopWinEventHook), but read
// without it in ArmButtonHwndResolveTimer from the taskbar thread -
// atomic makes that well-defined, matching g_taskbarWndSubclassed's own
// treatment elsewhere in this file.
std::atomic<DWORD> g_winEventThreadId;

// Serializes StartWinEventHook/StopWinEventHook against each other.
// EnsureTaskbarWnd can call StartWinEventHook from either Wh_ModAfterInit's
// own thread or Explorer's UI thread (via the ArrangeOverride hook) once
// g_hTaskbarWnd first resolves, and that call can still be inside
// CreateThread - before g_winEventThread is written - when Wh_ModUninit
// calls StopWinEventHook concurrently: a plain check-then-act (even an
// atomic one guarding just the CreateThread call) can't stop
// StopWinEventHook from checking g_winEventThread before StartWinEventHook
// has finished writing it, missing the very thread it was meant to tear
// down and leaving it to crash the process when Windhawk unmaps the
// module later. The mutex makes "is there a thread, and should one ever
// be created again" one atomic question both functions agree on.
std::mutex g_winEventThreadMutex;
bool g_winEventThreadStopped;

void StartWinEventHook() {
    std::lock_guard<std::mutex> guard(g_winEventThreadMutex);
    if (g_winEventThreadStopped || g_winEventThread) {
        return;
    }

    // CreateThread's out-parameter needs a plain DWORD*, not
    // atomic<DWORD>* - written through the local and then published to
    // the atomic once CreateThread returns.
    DWORD threadId = 0;
    g_winEventThread = CreateThread(nullptr, 0, WinEventHookThreadProc,
                                    nullptr, 0, &threadId);
    if (!g_winEventThread) {
        Wh_Log(L"StartWinEventHook: CreateThread failed");
    } else {
        g_winEventThreadId = threadId;
    }
}

// This thread is entirely mod-owned: PostThreadMessage can't silently
// fail the way a marshaled call onto someone else's thread can, and
// waiting for the thread to actually exit guarantees UnhookWinEvent (and
// the resolve timer's own KillTimer, now that it lives here too) has
// already run before Windhawk unmaps this module's code.
void StopWinEventHook() {
    HANDLE thread;
    DWORD threadId;
    {
        std::lock_guard<std::mutex> guard(g_winEventThreadMutex);
        // Set before releasing the lock, not after the thread is confirmed
        // gone below - this is what stops a StartWinEventHook call that
        // arrives after this point (e.g. a late ArrangeOverride pass) from
        // recreating a thread nobody would be left to tear down.
        g_winEventThreadStopped = true;
        thread = g_winEventThread;
        threadId = g_winEventThreadId;
        g_winEventThread = nullptr;
        g_winEventThreadId = 0;
    }
    if (!thread) {
        return;
    }

    // PostThreadMessage fails with ERROR_INVALID_THREAD_ID if the thread
    // hasn't created its message queue yet (WinEventHookThreadProc does
    // that as its first action) - retry rather than give up, but tie the
    // retry to the thread's own liveness rather than a fixed attempt
    // count: giving up after N tries and falling through to the
    // unconditional wait below with the signal never delivered would hang
    // Wh_ModUninit forever, since nothing else can wake that wait.
    while (!PostThreadMessage(threadId, WM_APP, 0, 0)) {
        if (WaitForSingleObject(thread, 10) == WAIT_OBJECT_0) {
            break;  // Thread exited on its own before ever seeing WM_APP.
        }
    }

    // Unconditional wait, not a bounded timeout: giving up early would let
    // Wh_ModUninit return and Windhawk unmap this module's code while the
    // thread is still inside it - a deferred crash, not a mitigated one.
    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);
}

// Capped exponential backoff for the click-sentinel probe, shared by
// ResolvePendingButtonHwnds and NextResolveDelayMs: a fallback safety net
// for a pinned-but-not-running app's button, in case its launch is ever
// reached through a path that doesn't produce EVENT_OBJECT_SHOW
// (WinEventProc's fast path normally re-resolves it immediately - see its
// own comment). The 30-minute ceiling keeps this fallback from becoming a
// source of frequent synthetic clicks itself.
constexpr ULONGLONG kResolveBackoffCeilingMs = 30ULL * 60 * 1000;
ULONGLONG ResolveBackoffMs(int consecutiveFailures) {
    int shift = std::min(consecutiveFailures, 16);  // keep the shift itself from overflowing
    return std::min(2000ULL << shift, kResolveBackoffCeilingMs);
}

// How long until the next resolve attempt could possibly do anything
// useful, based only on g_buttonHwndCache's already-recorded state - no
// XAML/tree access, just a scan of an in-memory map, so this is cheap
// enough to call after every timer tick.
DWORD NextResolveDelayMs() {
    ULONGLONG now = GetTickCount64();
    bool anyPending = false;
    bool anyResolved = false;
    ULONGLONG earliestDue = 0;

    for (auto& kv : g_buttonHwndCache) {
        const ButtonHwndCacheEntry& entry = kv.second;
        if (entry.hwnd) {
            anyResolved = true;
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
        // Even once every cached button is resolved, ItemsRepeater can
        // rebind an already-realized element to a different item (a
        // drag-reorder) without changing the total count - the one other
        // thing that re-arms this timer. Without a periodic re-check
        // here, ResolvePendingButtonHwnds' identity comparison (see
        // ButtonHwndCacheEntry) would never run again once everything
        // first resolves. Only armed when something is actually resolved
        // - an all-pinned taskbar already gets its own tick from the
        // backoff loop above; this only truly goes idle (INFINITE) when
        // the cache is empty.
        if (anyResolved) {
            return kIdleResolveTickMs;
        }
        return INFINITE;
    }

    DWORD pendingDelay = earliestDue > now ? (DWORD)(earliestDue - now) : 0;
    if (anyResolved) {
        // Mixed cache: some buttons resolved (need the same periodic
        // rebind re-check as the anyPending==false branch above), others
        // still pending on their own up-to-30-minute backoff schedule,
        // which could otherwise silently starve rebind detection for
        // everyone else. Capping at kIdleResolveTickMs here doesn't
        // resolve the pending entry any earlier, it only ensures the
        // already-resolved entries keep getting their identity re-checked
        // on the normal idle cadence.
        return std::min(pendingDelay, kIdleResolveTickMs);
    }
    return pendingDelay;
}

void CALLBACK ButtonHwndResolveTimerProc(HWND hwnd,
                                         UINT uMsg,
                                         UINT_PTR idEvent,
                                         DWORD dwTime) {
    // Self-managing one-shot rather than a recurring interval - stop
    // first, kick off the resolve pass, and let that pass decide for
    // itself whether (and when) to re-arm (see
    // ResolvePendingButtonHwnds' ScheduleNextResolveTick), rather than
    // running forever even once everything is resolved and nothing has
    // changed. Runs on this thread directly (a per-thread SetTimer with
    // hWnd=nullptr), so KillTimer here needs no cross-thread marshal.
    KillTimer(nullptr, idEvent);
    g_buttonHwndResolveTimerId = 0;

    if (g_unloading) {
        return;
    }

    // Snapshot once so both branches below (and the fallback log message)
    // agree on one value - see g_hTaskbarWnd's own comment for why a
    // stale nullptr reaching PostMessage isn't a no-op. Named distinctly
    // from this callback's own `hwnd` parameter (the timer's owning
    // window, always nullptr for this thread-owned timer, unrelated to
    // the taskbar window).
    HWND hTaskbarWnd = g_hTaskbarWnd;
    if (!hTaskbarWnd) {
        // The taskbar window went away (recreate) between this timer
        // being armed and firing - EnsureTaskbarWnd will re-resolve it on
        // its own next pass. Retry later rather than let a null reach
        // PostMessage below.
        ArmButtonHwndResolveTimer(kIdleResolveTickMs);
        return;
    }

    // No fallback if the subclass never installed - see
    // g_taskbarWndSubclassed's own comment for why. In that case,
    // ResolvePendingButtonHwnds simply never runs and this timer stops
    // re-arming itself entirely; EnsureTaskbarWnd explicitly kicks both
    // this timer and InvalidateTaskbarLayout the moment a subclass
    // install eventually succeeds (its own comment explains why that
    // kick is needed with no fallback marshal to have kept this ticking
    // in the meantime).
    //
    // ResolvePendingButtonHwnds computes/arms the next delay itself, at
    // the very end of whichever pass this triggers (see its own
    // ScheduleNextResolveTick comment for why: computing it here, right
    // after an async PostMessage, would read g_buttonHwndCache
    // concurrently with that pass's own writes to it). The one case that
    // still needs handling here is a PostMessage failure - nothing will
    // ever call back to re-arm if the message never arrives, so retry
    // after a fixed fallback delay rather than leaving the timer silently
    // dead.
    if (g_taskbarWndSubclassed) {
        if (!PostMessage(hTaskbarWnd, ResolveButtonHwndsMsg(), 0, 0)) {
            Wh_Log(L"ButtonHwndResolveTimerProc: PostMessage failed, "
                   L"error=%lu", GetLastError());
            ArmButtonHwndResolveTimer(kIdleResolveTickMs);
        }
    }
}

// The resolve timer lives on the dedicated WinEventHook thread (see
// g_buttonHwndResolveTimerId), not on a window this mod doesn't own, so
// arming it from the taskbar thread (where every caller of this function
// runs) needs to ask that thread to do it on its own message queue -
// PostThreadMessage can't silently fail the way a window-based marshal
// can, and there's nothing left to leak if it does (the thread just
// doesn't get the nudge, and its own next scheduled tick catches up).
void ArmButtonHwndResolveTimer(DWORD delayMs) {
    if (!g_winEventThreadId || g_unloading) {
        return;
    }
    PostThreadMessage(g_winEventThreadId, kArmResolveNowMsg, 0, delayMs);
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
        // Deliberately NOT checking HookTaskbarViewDllSymbols' own return
        // value here: it reflects whether EVERY symbol in its table
        // resolved, optional ones included, so a single missing optional
        // HWND-resolution symbol would fail this ENTIRE mod's load -
        // exactly the outcome `optional = true` on those entries exists
        // to prevent (see that table's own comment, and
        // HandleLoadedModuleIfTaskbarView's identical reasoning for why
        // its hook-apply call isn't gated on this return value either).
        // ArrangeOverride is the one truly required symbol in that table
        // - the mod's entire function depends on it - so that's checked
        // directly below instead, matching HookTaskbarDllSymbols' own
        // check above (safe to trust as-is, since every symbol load-
        // gating that check is genuinely required, not optional).
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
                // See HandleLoadedModuleIfTaskbarView's identical comment -
                // applied unconditionally so a missing optional symbol
                // doesn't also discard the hooks that did resolve.
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

    // Deliberately NOT tearing down the WinEventHook thread here: this
    // mod's hooks stay installed until this function returns, and
    // ArrangeOverride isn't gated on g_unloading - a pass landing in that
    // window could still call StartWinEventHook via EnsureTaskbarWnd,
    // creating a thread nobody would tear down (Wh_ModUninit, which does
    // the actual teardown, runs after this). g_unloading being set first
    // is what keeps ResolvePendingButtonHwnds' click-sentinel probe from
    // running once its hooks are gone, and makes IUIElement_Arrange_Hook
    // fall through to native positioning immediately regardless of
    // whether anything below forces a relayout.

    // Comes off Shell_TrayWnd as early as it's safe to, rather than
    // staying wired in for the rest of teardown. With no fallback marshal
    // once this is gone (see g_taskbarWndSubclassed's own comment), there
    // is no reliable way left to force an immediate visual snap-back to
    // native positions on disable - g_unloading above already guarantees
    // the mod stops overriding anything, so this is a cosmetic timing
    // difference only: native positions apply as soon as anything next
    // triggers XAML to re-run Arrange on its own (opening/closing an app,
    // a resize, etc.), rather than instantly.
    HWND hTaskbarWnd = g_hTaskbarWnd;
    if (hTaskbarWnd && g_taskbarWndSubclassed.exchange(false)) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            hTaskbarWnd, TaskbarWndSubclassProc);
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    // See Wh_ModBeforeUninit's comment for why this doesn't run there -
    // the mod's own hooks are gone by the time Wh_ModUninit runs, so
    // nothing can reawaken the thread being torn down here. This one
    // join now also covers the HWND-resolve timer, which lives on the
    // same thread.
    StopWinEventHook();
}

// Applies a freshly-loaded settings struct on the taskbar's own thread -
// needed since it reassigns leftApps/rightApps (std::vector<std::wstring>),
// which a concurrent ContainsAnyFragment call during a layout pass on that
// thread could otherwise read mid-reassignment. No fallback if the
// subclass never installed or PostMessage itself fails - see
// g_taskbarWndSubclassed's own comment for why. Unlike
// InvalidateTaskbarLayout/the HWND-resolve tick, a dropped settings change
// here is a real loss rather than a delayed retry (there's no periodic
// mechanism that would naturally pick it up later), so it's logged loudly
// rather than silently swallowed - the new settings just don't take effect
// until the taskbar recreates and a fresh subclass attempt succeeds.
void ApplyLoadedSettings(ModSettings settings) {
    // Snapshot once so every read below agrees - see g_hTaskbarWnd's own
    // comment for why a stale nullptr reaching PostMessage isn't a no-op
    // (here, it would mean the heap ModSettings gets release()d and
    // leaked while the settings change is silently dropped).
    HWND hTaskbarWnd = g_hTaskbarWnd;
    if (!hTaskbarWnd) {
        g_settings = std::move(settings);
        return;
    }

    if (!g_taskbarWndSubclassed) {
        Wh_Log(L"ApplyLoadedSettings: no taskbar subclass installed, "
               L"new settings not applied");
        return;
    }

    // PostMessage is async, so the settings can't live on this function's
    // own stack - ownership transfers to the heap allocation, released via
    // .release() only once PostMessage has actually queued it, and
    // reclaimed by TaskbarWndSubclassProc's SettingsChangedMsg case on the
    // dispatch side.
    auto heapSettings = std::make_unique<ModSettings>(std::move(settings));
    if (PostMessage(hTaskbarWnd, SettingsChangedMsg(), 0,
                    (LPARAM)heapSettings.get())) {
        heapSettings.release();
        return;
    }
    Wh_Log(L"ApplyLoadedSettings: PostMessage failed, error=%lu, "
           L"new settings not applied", GetLastError());
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    // Read every setting on this calling thread - Wh_Get/FreeStringSetting
    // don't touch XAML/COM, so they don't need to run on the taskbar
    // thread at all, only the final assignment into g_settings does (see
    // ApplyLoadedSettings).
    ApplyLoadedSettings(LoadSettingsFromStore());

    InvalidateTaskbarLayout();
}
