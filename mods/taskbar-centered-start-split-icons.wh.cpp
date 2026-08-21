// ==WindhawkMod==
// @id              taskbar-centered-start-split-icons
// @name            Taskbar Start Button Centered Origin
// @description     Pins the Start button to the true horizontal center of the screen, and splits running-app taskbar buttons into two groups flanking it based on which side of the screen each window is currently on (Windows 11 only, incompatible with "Start button always on the left")
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
  unconfirmed, not yet investigated further.
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

std::wstring ToLower(std::wstring s) {
    for (auto& c : s) {
        c = towlower(c);
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
    s.gapPx = Wh_GetIntSetting(L"gapPx");

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
// (reproducible crash: STATUS_STOWED_EXCEPTION, when Windows' "show
// taskbar apps on" setting causes a window moving across monitors to
// change a taskbar's button set, not just coordinates). RecomputeLayoutPlan
// does the entire traversal once per ArrangeOverride pass, up front,
// before any nested Arrange calls happen, writing every element's target
// X into g_lastArrangedX - IUIElement_Arrange_Hook then becomes a pure
// map lookup, with nothing left for XAML's mid-mutation state to make
// unsafe.

HWND g_hTaskbarWnd;

// Whether TaskbarWndSubclassProc is currently installed on g_hTaskbarWnd -
// see EnsureTaskbarWnd (where it's installed) and InvalidateTaskbarLayout
// (which checks it to pick PostMessage vs. the RunFromWindowThread
// fallback). atomic: set from Explorer's UI thread (EnsureTaskbarWnd) or
// Wh_ModAfterInit's thread, read from InvalidateTaskbarLayout's callers,
// which include the dedicated WinEventHook thread.
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


using RunFromWindowThreadProc_t = std::function<void()>;

// A second, concurrent RunFromWindowThread call targeting the same thread
// installs a second WH_CALLWNDPROC hook in the same chain. Both hooks
// only ever check the message id (there's exactly one, shared by every
// call), so when either call's SendMessage lands, every currently-
// installed hook for this thread fires and blindly casts cwp->lParam -
// which points at whichever call actually sent that specific message - to
// its own proc pointer. That means an unrelated concurrent call's proc can
// run in place of (or in addition to) the intended one. The `done` flag
// caps this at exactly one real invocation per SendMessage regardless of
// how many hooks are chained and firing for it: whichever hook proc
// happens to run first for a given message wins, the rest see done==true
// and no-op. Plain bool, not atomic - all hooks that fire for one message
// do so synchronously, one after another, on the single thread receiving
// that message, never concurrently with each other.
struct RunFromWindowThreadParam {
    RunFromWindowThreadProc_t* proc;
    bool done = false;
};

bool RunFromWindowThread(HWND hWnd, RunFromWindowThreadProc_t proc) {
    static const UINT runFromWindowThreadRegisteredMsg = RegisterWindowMessage(
        L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc();
        return true;
    }

    RunFromWindowThreadParam param{&proc};

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    auto* param = (RunFromWindowThreadParam*)cwp->lParam;
                    if (!param->done) {
                        param->done = true;
                        (*param->proc)();
                    }
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);

    return true;
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
    // released on every path below, not just the one at the end of a
    // fully successful call - every early return past this point used to
    // leak a reference. That's worse than it looks now that
    // RecomputeLayoutPlan calls this on every single ArrangeOverride
    // pass: one leaked reference per pass, for the life of the process,
    // on any Windows build where the prologue pattern below stops
    // matching.
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
// isn't a per-button thing - so one confirmed capture is proof it works,
// and one probe that reaches ReportClicked with zero capture, before any
// capture has ever been confirmed, is proof it doesn't. g_clickSentinelBroken
// latches that verdict permanently: ResolveHwndFromTaskListButton bails
// out before ever calling ReportClicked again once it's set, rather than
// retrying a mechanism now known to dispatch real clicks.
std::atomic<bool> g_clickSentinelConfirmed;
std::atomic<bool> g_clickSentinelBroken;

HRESULT WINAPI CTaskListWnd_HandleClick_Hook(void* pThis,
                                              void* taskGroup,
                                              void* taskItem,
                                              void** launcherOptions) {
    if (launcherOptions && *launcherOptions == (void*)&g_clickSentinel) {
        g_clickSentinel_TaskItem = taskItem;
        g_clickSentinel_TaskGroup = taskGroup;
        g_clickSentinelConfirmed = true;
        return S_OK;
    }

    return CTaskListWnd_HandleClick_Original(pThis, taskGroup, taskItem,
                                              launcherOptions);
}

// Called right after a real ReportClicked probe comes back with no
// capture - the one point where "no capture" is actually evidence about
// the interception itself, as opposed to an earlier, unrelated failure
// (missing view-model, no task item) that never reached ReportClicked at
// all. See g_clickSentinelBroken's own comment for the full reasoning.
void NoteUnconfirmedClickSentinelMiss() {
    if (!g_clickSentinelConfirmed && !g_clickSentinelBroken.exchange(true)) {
        Wh_Log(L"Click-sentinel interception never confirmed working - "
               L"disabling further HWND-resolution probes to avoid "
               L"dispatching real clicks. This usually means a Windows "
               L"update changed CTaskListWnd::HandleClick's internal call "
               L"path; consider disabling this mod until it's updated.");
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
    if (!TryGetItemFromContainer_TaskListWindowViewModel_Original ||
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
    TaskItem_ReportClicked_Original(windowsUdkTaskItem.get(),
                                     &g_clickSentinel);

    void* nativeTaskItem = g_clickSentinel_TaskItem;
    g_clickSentinel_TaskItem = nullptr;
    if (!nativeTaskItem) {
        NoteUnconfirmedClickSentinelMiss();
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
    if (!TryGetItemFromContainer_TaskListGroupViewModel_Original ||
        !TaskListGroupViewModel_IsMultiWindow_Original ||
        !TaskGroup_ReportClicked_Original || !CTaskGroup_GetNumItems_Original) {
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
    g_captureTaskGroup = true;
    // IsMultiWindow's implementation happens to call ITaskGroup::IsRunning
    // internally, which is hooked above to capture its `this` (the native
    // WindowsUdk task group) instead of really answering the question. The
    // -1 adjusts from the interface pointer QueryInterface handed back to
    // the adjacent vtable IsMultiWindow actually needs - a fixed ABI detail
    // of this object, not a magic number specific to this mod.
    TaskListGroupViewModel_IsMultiWindow_Original(
        (void**)groupViewModel.get() - 1);
    g_captureTaskGroup = false;

    void* windowsUdkTaskGroup = g_capturedTaskGroup;
    g_capturedTaskGroup = nullptr;
    if (!windowsUdkTaskGroup) {
        g_resolveStats.failure++;
        return nullptr;
    }

    g_clickSentinel_TaskGroup = nullptr;
    TaskGroup_ReportClicked_Original(windowsUdkTaskGroup, &g_clickSentinel);
    void* nativeTaskGroup = g_clickSentinel_TaskGroup;
    g_clickSentinel_TaskGroup = nullptr;
    if (!nativeTaskGroup) {
        NoteUnconfirmedClickSentinelMiss();
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
    // See g_clickSentinelBroken's comment - once the sentinel is known not
    // to be intercepted on this build, every further probe would be a
    // genuine click rather than a resolution attempt, so this bails out
    // before either resolution path can call ReportClicked again.
    if (g_clickSentinelBroken) {
        return nullptr;
    }

    HWND hwnd = ResolveHwndFromIndividualTaskItem(element);
    if (hwnd) {
        return hwnd;
    }

    return ResolveHwndFromTaskGroup(element);
}

// Per-button HWND cache, keyed by the XAML element's ABI pointer. Avoids
// re-running the resolution chain every pass, and negatively caches
// failures (a pinned-but-not-running app's task group legitimately has
// zero windows, so resolution fails until it's actually launched).
//
// consecutiveFailures drives capped exponential backoff (2s..32s) rather
// than a fixed retry: the resolution chain ends in a synthetic click
// against the taskbar's real click handler, and ItemsRepeater recycles
// the same element/cache entry for a given index rather than creating a
// new one, so a hard stop would permanently break side-following for a
// pinned app that's later launched. Backing off keeps retrying (worst
// case within 32s) without hammering the click handler forever.
//
// identity (the button's accessible name at resolve time) catches a
// different case: ItemsRepeater can rebind an already-realized element
// to a different item (e.g. a drag-reorder) without destroying it. The
// old HWND stays valid, just no longer this element's, so an IsWindow()
// check alone can't detect it - ResolvePendingButtonHwnds compares
// identity on every check and forces a re-resolve on mismatch.
struct ButtonHwndCacheEntry {
    HWND hwnd = nullptr;
    std::wstring identity;
    ULONGLONG lastAttempt = 0;
    int consecutiveFailures = 0;
};
std::unordered_map<void*, ButtonHwndCacheEntry> g_buttonHwndCache;

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
// thread_local since RecomputeLayoutPlan only ever does real work on the
// primary taskbar's own thread, but stays thread_local rather than a plain
// global on general principle for anything read alongside a per-pass
// reset - see g_passStats' comment for the concrete bug that caused
// (unrelated struct, same underlying risk).
thread_local LayoutPlanStats g_planStats;

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
    double x = leftInnerX;
    for (auto* entry : left) {
        outPlan[winrt::get_abi(entry->element)] = x - entry->width;
        x -= entry->width * leftScale;
    }
    x = rightInnerX;
    for (auto* entry : right) {
        outPlan[winrt::get_abi(entry->element)] = x;
        x += entry->width * rightScale;
    }
}

// ============================================================================
// XAML hooks
// ============================================================================

// Defined later (Live drag-follow section); forward-declared here because
// the ArrangeOverride hook below uses it to self-correct when the button
// set changes (new pin, app launched/closed) - see the comment at its call
// site for why that's needed.
//
// Only ever marks the taskbar's layout dirty (InvalidateArrange/
// InvalidateMeasure) - never forces a synchronous UpdateLayout() call. See
// the comment on the definition for why: a forced synchronous pass here
// was the confirmed cause of three separate explorer.exe crash-loop
// incidents, each via a different call path landing while the thread was
// already nested inside XAML-internal layout activity.
void InvalidateTaskbarLayout();

// Idle re-check cadence once every cached button is already resolved -
// see NextResolveDelayMs' comment (Mod lifecycle section) for what this
// specifically exists to catch (a drag-reorder rebind, which isn't
// latency-sensitive). Kept well above the 2s..32s backoff used for
// buttons that still need resolving: even at idle, each tick still
// resolves the taskbar's XamlRoot and walks its visual tree on Explorer's
// UI thread, and this runs indefinitely for the life of the session. Also
// used by ResolvePendingButtonHwnds (below) as a "something's not right
// yet, check back soon" fallback delay for passes that bail out before
// confirming the live button set - moved up here (out of the Mod
// lifecycle section it's otherwise grouped with) so it's declared before
// that use.
constexpr DWORD kIdleResolveTickMs = 30000;

// Defined later (Mod lifecycle section); forward-declared here so
// EnsureTaskbarWnd (below) can start the drag-follow WinEventHook as soon
// as the taskbar window resolves, whether that happens at normal startup
// or late (see EnsureTaskbarWnd's comment). This also starts the
// HWND-resolve timer now that it lives on the same dedicated thread - see
// WinEventHookThreadProc.
void StartWinEventHook();

// Defined later (Mod lifecycle section); forward-declared here so the
// ArrangeOverride hook below can request an immediate HWND-resolve attempt
// (delayMs = 0) as soon as it notices the task list button count changed,
// rather than waiting for whatever delay the timer last computed for
// itself - see NextResolveDelayMs' comment for why it can't see a
// brand-new button coming on its own.
void ArmButtonHwndResolveTimer(DWORD delayMs);

// Defined later (Mod lifecycle section, right before ButtonHwndResolveTimerProc);
// forward-declared here so ResolvePendingButtonHwnds (below) can compute
// and arm its own next tick at the end of a pass, on whichever thread that
// pass actually ran on - see ResolvePendingButtonHwnds' own
// ScheduleNextResolveTick comment for why it can't be computed by the
// timer callback itself anymore.
DWORD NextResolveDelayMs();

// Defined later (Live drag-follow section, alongside InvalidateTaskbarLayout
// itself); forward-declared here so EnsureTaskbarWnd (below) can install it
// on g_hTaskbarWnd as soon as the window resolves - see
// InvalidateTaskbarLayout's own comment for why a subclass is what lets it
// use a non-blocking PostMessage instead of RunFromWindowThread's per-call
// blocking marshal. WindhawkUtils::SetWindowSubclassFromAnyThread/
// RemoveWindowSubclassFromAnyThread key the subclass by this proc's own
// address, not by dwRefData (unused, passed as 0) - there's no separate id
// parameter the way raw SetWindowSubclass/RemoveWindowSubclass have.
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
        Wh_Log(L"Resolved taskbar window: %p", g_hTaskbarWnd);
        StartWinEventHook();

        // Lets InvalidateTaskbarLayout notify this window with a
        // non-blocking PostMessage instead of RunFromWindowThread's
        // per-call SetWindowsHookEx/SendMessage/UnhookWindowsHookEx dance -
        // see its own comment. A one-time blocking install here (via
        // SetWindowSubclassFromAnyThread's own marshal, if this isn't
        // already running on the taskbar thread) is fine; it's the
        // per-event cost on the hot invalidate path this is meant to
        // avoid, not one-shot setup.
        if (WindhawkUtils::SetWindowSubclassFromAnyThread(
                g_hTaskbarWnd, TaskbarWndSubclassProc, 0)) {
            g_taskbarWndSubclassed = true;
        } else {
            Wh_Log(L"EnsureTaskbarWnd: SetWindowSubclassFromAnyThread "
                   L"failed, InvalidateTaskbarLayout will use the blocking "
                   L"fallback instead");
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
void ResolvePendingButtonHwnds() {
    // Computes and arms this function's own next tick on destruction -
    // i.e. whenever this function returns, by any path (the early-return
    // guards below included, not just a full successful pass), always on
    // this same thread right after this pass's own cache reads/writes are
    // actually done. ButtonHwndResolveTimerProc used to do this itself
    // immediately after kicking off the resolve call, which was safe only
    // because that call used to be a blocking RunFromWindowThread marshal
    // - now that the common path is an async PostMessage to the taskbar's
    // subclass (see ButtonHwndResolveTimerProc), computing the delay right
    // after posting would read g_buttonHwndCache concurrently with this
    // function's own insert/erase calls on the taskbar thread - a real
    // use-after-free, not just stale data. Moving the computation here
    // closes that regardless of which path (post or the RunFromWindowThread
    // fallback) invoked this pass.
    //
    // NextResolveDelayMs' INFINITE answer is only trustworthy once a pass
    // has actually confirmed the live button set - an empty
    // g_buttonHwndCache from a pass that bailed out before enumerating
    // anything (every early-return guard below, or no repeater yet) looks
    // identical to a genuinely empty taskbar otherwise, and the timer
    // would then never be armed again except by incidental luck (some
    // unrelated EVENT_OBJECT_SHOW or count-change nudging
    // ArmButtonHwndResolveTimer). `enumerated` tracks whether the walk
    // below actually ran to completion; the destructor falls back to
    // kIdleResolveTickMs instead of trusting INFINITE when it didn't.
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

    // Runs marshaled through RunFromWindowThread's WH_CALLWNDPROC hook - a
    // raw Win32 callback boundary with no C++/WinRT exception translation
    // on the other side, same as IUIElement_Arrange_Hook/RecomputeLayoutPlan/
    // InvalidateTaskbarLayout's lambda. Every WinRT property access below
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
                if (it->second.hwnd) {
                    // Identity mismatch means ItemsRepeater rebound this
                    // element to a different item since we last resolved it -
                    // see ButtonHwndCacheEntry's comment. The old HWND is
                    // still a perfectly valid window, just no longer this
                    // element's, so IsWindow() alone can't catch this case.
                    needsResolve = !IsWindow(it->second.hwnd) ||
                                   identity != it->second.identity;
                } else {
                    needsResolve = now - it->second.lastAttempt >=
                                   ResolveBackoffMs(it->second.consecutiveFailures);
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
// once-per-pass traversal - two different phases now that the plan is
// built up front rather than computed inline here).
struct ArrangePassStats {
    int totalArrangeCalls = 0;
    int repositioned = 0;
    int qiFailures = 0;
    int exceptions = 0;
};
// thread_local: the ArrangeOverride hook (and so this Arrange hook, which
// only ever runs nested inside it) runs once per taskbar instance's XAML
// tree, process-wide. In practice all of them - primary and any
// secondary-monitor taskbars - run on the same Explorer UI thread (see
// RecomputeLayoutPlan's comment for how that was confirmed), so a plain
// global would work too; thread_local costs nothing extra here and stays
// correct even if that ever stopped being true, so it's kept as cheap
// insurance rather than because it's currently load-bearing.
thread_local ArrangePassStats g_passStats;

HRESULT WINAPI IUIElement_Arrange_Hook(void* pThis,
                                       winrt::Windows::Foundation::Rect rect) {
    auto original = [=] { return IUIElement_Arrange_Original(pThis, rect); };

    if (!g_inTaskbarArrangeOverride || g_unloading || !g_hTaskbarWnd) {
        return original();
    }

    // This hook replaces the process-wide IUIElement::Arrange vtable slot,
    // invoked by XAML's own native call sites for every UIElement in
    // explorer.exe - a raw ABI boundary with no C++/WinRT exception
    // translation on the other side. A WinRT call throwing here crosses
    // that boundary uncaught and fail-fasts the whole process (previously
    // hit via .as<T>() throwing on a null source during a fresh
    // secondary-monitor tree's construction). The QueryInterface below is
    // the only WinRT call left in this function - everything else is a
    // plain map lookup - so this net is mostly a leftover safety margin.
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
    if (!g_hTaskbarWnd) {
        return;
    }
    DWORD primaryThreadId = GetWindowThreadProcessId(g_hTaskbarWnd, nullptr);
    if (primaryThreadId == 0 || primaryThreadId != GetCurrentThreadId()) {
        return;
    }
    if (!g_planDirty &&
        GetTickCount64() - g_lastPlanRecomputeTick < kMaxPlanStalenessMs) {
        // This backstop only bounds staleness while ArrangeOverride keeps
        // getting called at least this often - once XAML stops producing
        // layout passes, this early return would never get reevaluated
        // again, so a change that landed in a skipped pass (see
        // g_planDirty's own comment) could then sit stale indefinitely
        // instead of just kMaxPlanStalenessMs (concretely: a pin/unpin on
        // an otherwise-idle desktop). The repeater is cached now (see
        // GetCachedTaskbarRepeater), so checking the realized task-list-
        // button set against what the plan actually covers is affordable
        // every time this branch is taken - if any live button isn't in
        // g_lastArrangedX, the plan is stale regardless of the dirty flag
        // or the clock, and this pass needs to fall through to a real
        // recompute rather than trust the backstop.
        bool planCoversLiveTaskListButtons = true;
        try {
            if (FrameworkElement repeater = GetCachedTaskbarRepeater()) {
                for (auto& child : GetRepeaterChildElements(repeater)) {
                    // Map lookup first: every element the plan covers is
                    // already a key in g_lastArrangedX, so this is a plain
                    // hash lookup for the common case. IsTaskListButton
                    // (winrt::get_class_name - a GetRuntimeClassName round
                    // trip plus an HSTRING allocation) then only runs for
                    // the rare child that isn't in the plan yet, which is
                    // exactly the case this check exists to catch.
                    if (!g_lastArrangedX.count(winrt::get_abi(child)) &&
                        IsTaskListButton(child)) {
                        planCoversLiveTaskListButtons = false;
                        break;
                    }
                }
            }
        } catch (...) {
            // Conservative default: treat an exception here as "can't
            // confirm the plan is current" and fall through to the real
            // recompute below, which has its own exception handling.
            planCoversLiveTaskListButtons = false;
        }
        if (planCoversLiveTaskListButtons) {
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
        // ChildInfo's comment for why (was up to 4 redundant
        // winrt::get_class_name calls per child per pass beforehand).
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
    // and so before any nested Arrange calls) is what actually closes the
    // crash class the old settling-window mechanism used to work around.
    RecomputeLayoutPlan();

    // The button count can change without any window moving (new pin, app
    // launched/closed). If RecomputeLayoutPlan's repeater walk ran before
    // XAML had realized a just-inserted button yet, this pass's plan
    // simply has no entry for it (see g_lastArrangedX's comment) and it
    // renders at its native position for one pass. Self-correct by
    // invalidating whenever the observed count changes, which is enough
    // for RecomputeLayoutPlan to pick it up on the next pass; also arm an
    // immediate HWND-resolve attempt, since a newly-inserted button is
    // exactly the case ResolvePendingButtonHwnds' own cache-only view
    // (NextResolveDelayMs) can't see coming on its own. There's nothing
    // left to suppress or re-arm beyond that - RecomputeLayoutPlan is safe
    // to call as often as this triggers it - so this is a plain count
    // comparison with no gating logic alongside it.
    //
    // thread_local: this hook runs process-wide for every taskbar
    // instance's XAML tree. In practice they all run on the same Explorer
    // UI thread (see RecomputeLayoutPlan's comment), so a plain global
    // would work too - kept thread_local as cheap insurance, same
    // reasoning as g_passStats above.
    static thread_local int lastPlanTaskListCount = -1;
    int currentTaskListCount = g_planStats.taskListTotal;
    if (currentTaskListCount != lastPlanTaskListCount) {
        bool countChanged = lastPlanTaskListCount != -1;
        lastPlanTaskListCount = currentTaskListCount;
        if (countChanged) {
            InvalidateTaskbarLayout();
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
// EVENT_OBJECT_LOCATIONCHANGE), and both call paths below
// (TaskbarWndSubclassProc's dispatch and RunFromWindowThread's fallback)
// land on that same thread, so one thread_local flag covers either.
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
// that matters on this specific call path. Function-local static (like
// RunFromWindowThread's own registered message) rather than a
// namespace-scope variable, so RegisterWindowMessage runs lazily on first
// real use instead of from a dynamic initializer under DllMain's loader
// lock.
UINT InvalidateTaskbarLayoutMsg() {
    static const UINT msg =
        RegisterWindowMessage(L"Windhawk_InvalidateTaskbarLayout_" WH_MOD_ID);
    return msg;
}

// Same idea as InvalidateTaskbarLayoutMsg, for ButtonHwndResolveTimerProc
// - lets it post instead of going through RunFromWindowThread on every
// tick (up to ~7/sec while EVENT_OBJECT_SHOW events are bursting).
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
// plain PostMessage here instead of RunFromWindowThread's
// SetWindowsHookEx/SendMessage/UnhookWindowsHookEx dance on every single
// call - and, since a subclass proc is dispatched directly by that
// thread's own message loop rather than through a possibly-multiply-
// chained hook, it can't suffer the double-invocation risk RunFromWindowThread
// has to guard against instead. Everything other than these private
// messages is forwarded to DefSubclassProc, which is also what lets
// comctl32 clean this subclass up automatically via WM_NCDESTROY if the
// window is ever destroyed out from under the mod without
// Wh_ModBeforeUninit's explicit removal call running first.
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
        auto* heapSettings = reinterpret_cast<ModSettings*>(lParam);
        g_settings = std::move(*heapSettings);
        delete heapSettings;
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

// Lets the XAML dispatcher pick up a relayout on its own next tick,
// rather than forcing a synchronous UpdateLayout() call: this function's
// callers include a raw OS callback (WinEventProc) that can fire while
// already nested inside XAML-internal layout activity, and a forced
// UpdateLayout() there reenters WinUI layout and fails fast with
// STATUS_STOWED_EXCEPTION - a raw SEH RaiseException, not a C++
// exception, so no try/catch anywhere in this file can contain it. Don't
// reintroduce a forced call here without a fundamentally different
// mechanism (e.g. a genuinely async, post-unwind-only deferred call).
//
// Deliberately does NOT set g_planDirty itself, even though every real
// trigger for a layout change goes through here - marking dirty on this
// function's OWN calling thread would be premature when the marshal below
// is asynchronous (the common PostMessage case): a natural ArrangeOverride
// pass can land on the taskbar thread between this call setting the flag
// and the posted message actually being processed, see it already true,
// recompute with whatever state existed before the real change (settings
// not yet applied, an HWND not yet re-cached), and clear the flag - so the
// posted invalidate that finally runs afterward finds g_planDirty already
// false and skips the recompute the change actually needed. Each real
// caller marks dirty itself, at the point its own state change becomes
// visible on the taskbar thread: PerformTaskbarLayoutInvalidate (the
// PostMessage/RunFromWindowThread call this function makes) and the
// SettingsChangedMsg dispatch case in TaskbarWndSubclassProc.
void InvalidateTaskbarLayout() {
    if (!g_hTaskbarWnd) {
        return;
    }

    if (g_taskbarWndSubclassed) {
        // The hot path: called from WinEventProc's drag-follow throttle
        // at up to ~7 times/sec while any window on the system is being
        // dragged, plus every resolve tick. PostMessage doesn't block on
        // Explorer's UI thread pumping it, unlike RunFromWindowThread
        // below, which installs a WH_CALLWNDPROC hook and blocks in
        // SendMessage until that thread processes it - a real cost at
        // this frequency, even though it's fine for a one-shot call.
        if (!PostMessage(g_hTaskbarWnd, InvalidateTaskbarLayoutMsg(), 0, 0)) {
            Wh_Log(L"InvalidateTaskbarLayout: PostMessage failed, error=%lu",
                   GetLastError());
        }
        return;
    }

    // Fallback for the (should stay rare) case where the taskbar window
    // was never successfully subclassed - still gets the job done, just
    // without the non-blocking benefit above.
    bool posted =
        RunFromWindowThread(g_hTaskbarWnd, PerformTaskbarLayoutInvalidate);
    if (!posted) {
        Wh_Log(L"InvalidateTaskbarLayout: RunFromWindowThread failed");
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
        // like - nudge the resolve timer to run right away instead of
        // leaving it to its own backoff schedule (which, for a button
        // that's failed before, can be up to kResolveBackoffCeilingMs
        // away). The backoff schedule itself is kept as a fallback -
        // this is a fast path on top of it, not a replacement for it, in
        // case a launch is ever reached through a code path that
        // legitimately doesn't produce this event.
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
            {LR"(public: virtual long __cdecl CTaskListWnd::HandleClick(struct ITaskGroup *,struct ITaskItem *,struct winrt::Windows::System::LauncherOptions const &))"},
            &CTaskListWnd_HandleClick_Original,
            CTaskListWnd_HandleClick_Hook,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CWindowTaskItem::GetWindow(void))"},
            &CWindowTaskItem_GetWindow_Original,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CImmersiveTaskItem::GetWindow(void))"},
            &CImmersiveTaskItem_GetWindow_Original,
        },
        {
            {LR"(const CImmersiveTaskItem::`vftable'{for `ITaskItem'})"},
            &CImmersiveTaskItem_vftable,
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
DWORD g_winEventThreadId;

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

    g_winEventThread = CreateThread(nullptr, 0, WinEventHookThreadProc,
                                    nullptr, 0, &g_winEventThreadId);
    if (!g_winEventThread) {
        Wh_Log(L"StartWinEventHook: CreateThread failed");
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
        // still pending on their own backoff schedule - which, since
        // that ceiling was extended to 30 minutes to cut down on
        // synthetic clicks against pinned-not-running apps, could
        // otherwise silently starve rebind detection for everyone else
        // for just as long. Capping at kIdleResolveTickMs here doesn't
        // resolve the pending entry any earlier (ResolvePendingButtonHwnds
        // still checks its own backoff before actually retrying it), it
        // only ensures the already-resolved entries keep getting their
        // identity re-checked on the normal idle cadence.
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

    // Prefer the subclass's non-blocking PostMessage (see
    // ResolveButtonHwndsMsg) - this can fire several times a second
    // during an EVENT_OBJECT_SHOW burst, the same hot-path cost
    // InvalidateTaskbarLayout avoids via PostMessage instead of
    // RunFromWindowThread's per-call hook install. Only fall back to the
    // blocking marshal in the (should stay rare) case the subclass was
    // never successfully installed.
    //
    // Neither branch computes/arms the next delay here anymore -
    // ResolvePendingButtonHwnds does that itself now, at the very end of
    // whichever pass this triggers (see its own ScheduleNextResolveTick
    // comment for why: computing it here, right after an async
    // PostMessage, would read g_buttonHwndCache concurrently with that
    // pass's own writes to it). The one case that still needs handling
    // here is a PostMessage failure - nothing will ever call back to
    // re-arm if the message never arrives, so retry after a fixed
    // fallback delay rather than leaving the timer silently dead.
    if (g_taskbarWndSubclassed) {
        if (!PostMessage(g_hTaskbarWnd, ResolveButtonHwndsMsg(), 0, 0)) {
            Wh_Log(L"ButtonHwndResolveTimerProc: PostMessage failed, "
                   L"error=%lu", GetLastError());
            ArmButtonHwndResolveTimer(kIdleResolveTickMs);
        }
    } else if (!RunFromWindowThread(g_hTaskbarWnd, ResolvePendingButtonHwnds)) {
        // Same "nothing will call back" concern as the PostMessage failure
        // above - a marshal failure here (SetWindowsHookEx failing, or
        // g_hTaskbarWnd going stale/null mid-taskbar-recreate) means
        // ResolvePendingButtonHwnds never ran at all, so its own
        // ScheduleNextResolveTick guard never gets a chance to re-arm.
        Wh_Log(L"ButtonHwndResolveTimerProc: RunFromWindowThread failed");
        ArmButtonHwndResolveTimer(kIdleResolveTickMs);
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
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
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
                if (HookTaskbarViewDllSymbols(taskbarViewModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    EnsureTaskbarWnd();
    Wh_Log(L"g_hTaskbarWnd = %p, g_taskbarViewDllLoaded = %d", g_hTaskbarWnd,
           (int)g_taskbarViewDllLoaded);

    InvalidateTaskbarLayout();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    // Deliberately NOT tearing down the WinEventHook thread here: this
    // mod's hooks stay installed until this function returns, and
    // ArrangeOverride isn't gated on g_unloading - a pass landing in that
    // window could still call StartWinEventHook via EnsureTaskbarWnd,
    // creating a thread nobody would tear down since Wh_ModUninit runs
    // after this (StartWinEventHook's own atomic guard prevents a second
    // thread if one already exists, but not a first one if none does
    // yet). This call still needs the Arrange hook alive to restore
    // native positioning, so it has to run while the hooks are
    // installed - actual teardown waits for Wh_ModUninit, after Windhawk
    // has removed them. g_unloading being set here (before that) is what
    // keeps ResolvePendingButtonHwnds' click-sentinel probe from running
    // once the hooks it depends on are gone - see its own comment.

    // Removed here, before the final InvalidateTaskbarLayout() call below,
    // for two reasons: it makes that call fall back to RunFromWindowThread's
    // blocking marshal instead of the normally non-blocking PostMessage
    // path, restoring this function's "runs before Wh_ModBeforeUninit
    // returns" guarantee for this last invalidate exactly as it was before
    // the subclass existed; and it gets TaskbarWndSubclassProc off
    // Shell_TrayWnd as early as it's safe to, rather than leaving it wired
    // in for the rest of teardown. RemoveWindowSubclassFromAnyThread (like
    // SetWindowSubclassFromAnyThread) already marshals itself onto the
    // owning thread internally, so this doesn't need RunFromWindowThread's
    // own marshal wrapped around it.
    if (g_hTaskbarWnd && g_taskbarWndSubclassed.exchange(false)) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            g_hTaskbarWnd, TaskbarWndSubclassProc);
    }

    InvalidateTaskbarLayout();
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
// thread could otherwise read mid-reassignment. Prefers the subclass's
// PostMessage over RunFromWindowThread for the same non-blocking reason as
// InvalidateTaskbarLayout/the HWND-resolve tick, but unlike those two a
// dropped settings change is a real loss (not just a delayed retry), so
// this - unlike them - does fall back to the blocking RunFromWindowThread
// marshal if PostMessage itself fails, not only when the subclass was
// never installed at all.
void ApplyLoadedSettings(ModSettings settings) {
    if (!g_hTaskbarWnd) {
        g_settings = std::move(settings);
        return;
    }

    if (g_taskbarWndSubclassed) {
        // PostMessage is async, so the settings can't live on this
        // function's own stack - ownership transfers to the heap pointer,
        // and TaskbarWndSubclassProc's SettingsChangedMsg case deletes
        // it after moving out of it.
        auto* heapSettings = new ModSettings(std::move(settings));
        if (PostMessage(g_hTaskbarWnd, SettingsChangedMsg(), 0,
                        (LPARAM)heapSettings)) {
            return;
        }
        Wh_Log(L"ApplyLoadedSettings: PostMessage failed, error=%lu, "
               L"falling back to RunFromWindowThread", GetLastError());
        // Reclaim ownership before falling through to the blocking path.
        settings = std::move(*heapSettings);
        delete heapSettings;
    }

    if (!RunFromWindowThread(g_hTaskbarWnd,
                             [settings = std::move(settings)]() mutable {
                                 g_settings = std::move(settings);
                             })) {
        Wh_Log(L"ApplyLoadedSettings: RunFromWindowThread failed, "
               L"new settings not applied");
    }
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
