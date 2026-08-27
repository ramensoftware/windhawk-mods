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

// Design rationale, history, and edge-case reasoning behind the comments
// below that say "see RATIONALE.md":
// https://github.com/rycalvo/w11tb_centerer/blob/main/RATIONALE.md

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
// Never traverse the tree from inside a nested Arrange call instead (see
// RATIONALE.md).

// Taskbar window handle, resolved by EnsureTaskbarWnd. atomic: written on
// the taskbar thread (or Wh_ModAfterInit's thread at startup), read from
// the dedicated WinEventHook thread. Callers with more than one read
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

HWINEVENTHOOK g_locationChangeHook;
HWINEVENTHOOK g_showEventHook;
std::atomic<int> g_winEventRawCount;
std::atomic<int> g_winEventInvalidateCount;
std::atomic<int> g_invalidateSkippedReentrant;
std::atomic<int> g_invalidateExceptions;

// Only touched from the dedicated WinEventHook thread (WinEventProc,
// DragFollowTrailingTimerProc, ButtonHwndResolveTimerProc all run there
// exclusively).
UINT_PTR g_dragFollowTrailingTimerId;

// The HWND-resolve timer's own id, on the same dedicated thread.
UINT_PTR g_buttonHwndResolveTimerId;

// Last drag-follow invalidate tick. Also updated by
// DragFollowTrailingTimerProc, since a trailing-timer fire is itself an
// invalidate for WinEventProc's own throttle to see.
ULONGLONG g_lastDragFollowInvalidate;

// Leading-edge throttle for EVENT_OBJECT_SHOW, mirroring the
// LOCATIONCHANGE throttle above.
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
// user gesture), this mod probes unattended from a background timer - see
// RATIONALE.md for the failure-mode analysis.
//
// Latched per path (item vs. group), not one shared flag, since the two
// paths reach CTaskListWnd::HandleClick through different internal call
// chains and a Windows update could break just one. Requires
// kClickSentinelMissesBeforeBroken consecutive misses before latching
// "broken" (not just one - see NoteUnconfirmedClickSentinelMiss for why),
// and that bound only holds pre-confirmation - see RATIONALE.md.
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
        return S_OK;
    }

    return CTaskListWnd_HandleClick_Original(pThis, taskGroup, taskItem,
                                              launcherOptions);
}

// Called right after a real ReportClicked probe comes back with no
// capture - not proof the interception is broken by itself (the window
// could have closed mid-probe, etc.), hence requiring a few misses rather
// than latching on the first one.
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

HWND ResolveHwndFromIndividualTaskItem(FrameworkElement element) {
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
    // Same interception hook as ResolveHwndFromIndividualTaskItem
    // (CTaskListWnd::HandleClick handles both item and group paths).
    if (!CTaskListWnd_HandleClick_Original ||
        !TryGetItemFromContainer_TaskListGroupViewModel_Original ||
        !TaskListGroupViewModel_IsMultiWindow_Original ||
        !TaskGroup_ReportClicked_Original || !CTaskGroup_GetNumItems_Original) {
        return nullptr;
    }

    // Validated up front (memoized, side-effect-free) rather than only
    // after dispatching a click - see RATIONALE.md for why that ordering
    // matters.
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
    // Each path bails out once its own sentinel is known broken, rather
    // than dispatching more genuine clicks - see g_clickSentinelItemBroken/
    // g_clickSentinelGroupBroken.
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
};
std::unordered_map<void*, ButtonHwndCacheEntry> g_buttonHwndCache;

// Set by WinEventProc's EVENT_OBJECT_SHOW branch and the ArrangeOverride
// hook's count-change check to make the next resolve pass ignore a
// negatively-cached entry's backoff - but only up to
// kMaxForcedRetryFailures consecutive failures each (see RATIONALE.md for
// why unconditional forcing was a real bug).
std::atomic<bool> g_forceResolveUnresolved;
constexpr int kMaxForcedRetryFailures = 3;

// The HWNDs g_buttonHwndCache currently resolves to, rebuilt at the end of
// every successful ResolvePendingButtonHwnds pass and read by WinEventProc
// (a different thread) to filter drag-follow's LOCATIONCHANGE events.
// Needs its own mutex since it's read cross-thread outside a marshal.
std::mutex g_resolvedHwndsMutex;
std::unordered_set<HWND> g_resolvedHwnds;

// Runs the resolution chain and updates the cache. Deliberately ONLY ever
// called from the resolve timer, never from GetButtonHwnd or an active
// Arrange pass - running the click-sentinel technique while a button is
// mid-insertion/removal in an ItemsRepeater reaches STATUS_STOWED_EXCEPTION
// and crashes explorer.exe (confirmed via crash-dump analysis; see
// RATIONALE.md for the full trigger conditions).
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

// A minimized window's GetWindowRect returns a nonsense off-screen
// position, so this freezes at the last known classification from before
// it was minimized instead.
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

        // No prior classification - this window was minimized before we
        // ever saw it. GetWindowPlacement's rcNormalPosition still
        // reports the restored position while minimized (unlike
        // GetWindowRect).
        WINDOWPLACEMENT wp{.length = sizeof(wp)};
        if (!hwnd || !GetWindowPlacement(hwnd, &wp) ||
            !GetMonitorInfo(mon, &mi)) {
            return {ResolveUnresolvedAppsDefaultSide()};
        }
        wr = wp.rcNormalPosition;
        // rcNormalPosition is in workspace coordinates (relative to
        // rcWork), while screenCenterX below is computed from rcMonitor -
        // a left-docked appbar shifts these apart, so this offset keeps a
        // boundary window from classifying to the wrong side.
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
// window position, then the configured default. Skips the accessible-name
// lookup entirely when leftApps/rightApps are both empty (the default) -
// this runs for every task-list button on every ArrangeOverride pass.
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

// Search, Task View and Widgets use a negative-margin collapse trick when
// hidden/shown - ActualWidth() includes that margin, so it never drops
// below the collapsed width. Reads the content child's DesiredSize instead
// (matching taskbar-start-button-position.wh.cpp), falling back to
// ActualWidth() if no child is realized yet. NOT used for Start (never
// hidden/shown, and DesiredSize() understates its true width) or task list
// buttons (this file's hottest path, no evidence of the same bug) - see
// RATIONALE.md.
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
    // isn't, so entry->width alone isn't a reliable zero-width signal -
    // see RATIONALE.md); skipping its outPlan entry then avoids
    // stacking it on its neighbor for that one frame.
    double x = leftInnerX;
    for (auto* entry : left) {
        if (entry->element.ActualWidth() > 0) {
            outPlan[winrt::get_abi(entry->element)] = x - entry->width;
        }
        x -= entry->width * leftScale;
    }
    x = rightInnerX;
    for (auto* entry : right) {
        if (entry->element.ActualWidth() > 0) {
            outPlan[winrt::get_abi(entry->element)] = x;
        }
        x += entry->width * rightScale;
    }
}

// ============================================================================
// XAML hooks
// ============================================================================

// Defined later (Live drag-follow section). Only ever marks the taskbar's
// layout dirty - never forces a synchronous UpdateLayout() (see
// RATIONALE.md).
void InvalidateTaskbarLayout();

// Idle re-check cadence once every cached button is resolved - keeps
// identity re-checked periodically (an ItemsRepeater rebind can change
// which item an element represents without changing the button count).
constexpr DWORD kIdleResolveTickMs = 30000;

// Defined later (Mod lifecycle section).
void StartWinEventHook();

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

// Resolves g_hTaskbarWnd and installs the taskbar-window subclass,
// self-healing on every ArrangeOverride pass rather than a one-shot
// Wh_ModAfterInit lookup - see RATIONALE.md for why both parts need to
// keep retrying rather than running once.
HWND EnsureTaskbarWnd() {
    // Shell_TrayWnd can in principle be recreated without explorer.exe
    // restarting. Cheap enough to check unconditionally every pass.
    if (g_hTaskbarWnd && !IsWindow(g_hTaskbarWnd)) {
        g_hTaskbarWnd = nullptr;
        // comctl32 tears the old subclass down on WM_NCDESTROY already -
        // this just keeps the flag in step so a fresh install is attempted
        // on the new window.
        g_taskbarWndSubclassed = false;
    }

    if (!g_hTaskbarWnd) {
        if (g_unloading) {
            // Guards against a pass landing here after Wh_ModBeforeUninit
            // already ran, with no teardown left to stop a newly-created
            // WinEventHook thread.
            return nullptr;
        }
        g_hTaskbarWnd = FindCurrentProcessTaskbarWnd();
        if (!g_hTaskbarWnd) {
            return nullptr;
        }
        Wh_Log(L"Resolved taskbar window: %p", (HWND)g_hTaskbarWnd);
        StartWinEventHook();
    }

    // Retried on every call until it succeeds, not just the pass that
    // first resolves g_hTaskbarWnd above - see RATIONALE.md for why that
    // matters. Cheap: this runs on the taskbar's own thread, so
    // SetWindowSubclassFromAnyThread takes its same-thread fast path.
    if (!g_taskbarWndSubclassed && !g_unloading &&
        WindhawkUtils::SetWindowSubclassFromAnyThread(
            g_hTaskbarWnd, TaskbarWndSubclassProc, 0)) {
        g_taskbarWndSubclassed = true;

        // Undo immediately if Wh_ModBeforeUninit's removal pass already
        // ran with the subclass not yet installed - see RATIONALE.md for
        // the crash this closes.
        if (g_unloading && g_taskbarWndSubclassed.exchange(false)) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                g_hTaskbarWnd, TaskbarWndSubclassProc);
        } else {
            // Kicks the resolve timer and a relayout awake - needed since
            // nothing else restarts them once a subclass install has
            // failed for a while (see ButtonHwndResolveTimerProc).
            ArmButtonHwndResolveTimer(0);
            InvalidateTaskbarLayout();
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
// garbage pointer instead of being intercepted (see RATIONALE.md).
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
    // only trustworthy then - the destructor falls back to
    // kIdleResolveTickMs otherwise.
    bool enumerated = false;
    struct ScheduleNextResolveTick {
        bool* enumerated;
        ~ScheduleNextResolveTick() {
            DWORD delay =
                *enumerated ? NextResolveDelayMs() : kIdleResolveTickMs;
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
                    // its own comment.
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

        // Rebuild the published resolved-HWND set for WinEventProc's
        // drag-follow filter - unconditionally, since it's cheap and needs
        // to reflect this pass's pruning either way.
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
// element's ABI pointer. Written only by RecomputeLayoutPlan, so
// IUIElement_Arrange_Hook below never computes anything, only looks a
// value up. An element with no entry (secondary-monitor, or too new to be
// in this pass's plan) falls through to native positioning.
std::unordered_map<void*, double> g_lastArrangedX;

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
        // g_lastArrangedX, or the live child count doesn't match the last
        // recompute's, the plan is stale regardless of the dirty flag.
        // Hash lookup first, IsTaskListButton (a real class-name lookup)
        // second and short-circuited, so the common "nothing changed"
        // case never pays for it.
        bool planIsCurrent = true;
        try {
            if (FrameworkElement repeater = GetCachedTaskbarRepeater()) {
                int liveChildCount = 0;
                for (auto& child : GetRepeaterChildElements(repeater)) {
                    liveChildCount++;
                    if (!g_lastArrangedX.count(winrt::get_abi(child)) &&
                        IsTaskListButton(child)) {
                        // A live task list button the plan doesn't cover
                        // yet. Scoped to task list buttons since a
                        // brand-new system button can't reach this branch.
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

        // Classify each child's SystemButton status exactly once - see
        // ChildInfo's comment for why.
        std::vector<ChildInfo> childInfos;
        childInfos.reserve(children.size());
        for (auto& child : children) {
            childInfos.push_back({child, IdentifySystemButton(child)});
        }

        // Start first: ComputeSystemButtonX/PlanTaskListButtons below
        // both read g_lastStartWidth, so it must reflect this pass.
        for (auto& info : childInfos) {
            if (info.systemButton == SystemButton::Start) {
                // Bare ActualWidth(), NOT SystemButtonContentWidth - see
                // RATIONALE.md for why that fix doesn't apply to Start.
                // Only updates g_lastStartWidth when positive, so a
                // freshly (re)created Start keeps the last known-good
                // width instead of collapsing around a zero-width Start.
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

        // Task list buttons last - see PlanTaskListButtons for the
        // single-O(n)-pass reasoning.
        PlanTaskListButtons(children, startCenterX, leftBoundLocal,
                            rightBoundLocal, newPlan);

        g_lastArrangedX = std::move(newPlan);
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

// Guards against reentering this body on the taskbar's own thread - a
// nested WinRT/taskbar.dll call inside it can pump messages and let a
// second posted invalidate land reentrantly before the first returns. This
// body also never calls UpdateLayout() itself - see InvalidateTaskbarLayout
// below for why a forced synchronous layout pass here is unsafe regardless
// of this guard (see RATIONALE.md for more).
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
// (up to ~7/sec while EVENT_OBJECT_SHOW events are bursting).
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

// Installed on g_hTaskbarWnd by EnsureTaskbarWnd. Handles the three private
// messages above and forwards everything else to DefSubclassProc (which
// also lets comctl32 clean this subclass up via WM_NCDESTROY if the window
// is destroyed before Wh_ModBeforeUninit removes it). See RATIONALE.md for
// why this is the sole path onto the taskbar thread, with no fallback.
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
        // Marks the previous plan stale now that the settings it was built
        // from just changed. See RATIONALE.md.
        g_planDirty = true;
        return 0;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Schedules a relayout on the taskbar's own thread via PostMessage rather
// than forcing a synchronous UpdateLayout() call. MUST stay async: a forced
// UpdateLayout() from a nested callback (e.g. WinEventProc) reenters WinUI
// layout and raises STATUS_STOWED_EXCEPTION, a raw SEH exception no
// try/catch in this file can contain. Does not set g_planDirty itself -
// see RATIONALE.md; each real caller marks dirty at its own point instead.
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

// One-shot: fires once the throttle window in WinEventProc has gone quiet,
// applying whatever position a drag/move most recently landed on.
void CALLBACK DragFollowTrailingTimerProc(HWND hwnd,
                                          UINT uMsg,
                                          UINT_PTR idEvent,
                                          DWORD dwTime) {
    KillTimer(nullptr, idEvent);
    g_dragFollowTrailingTimerId = 0;

    // Cheap early-exit during teardown; InvalidateTaskbarLayout would
    // already no-op safely either way.
    if (g_unloading) {
        return;
    }

    // This fire is itself an invalidate - WinEventProc's throttle needs to
    // know about it too.
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

    // Only a window this mod has resolved to a taskbar button can affect
    // the layout on a move, so filter LOCATIONCHANGE against that set
    // before the cross-process USER32 calls below. Not applied to SHOW: a
    // newly-shown window can't be resolved yet by definition.
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
        // A newly-visible top-level window is what a pinned-but-not-running
        // app launching looks like - nudge the resolve timer to run now,
        // bypassing each entry's own backoff. Leading-edge throttled since
        // this event is unthrottled and can arrive in bursts; no trailing
        // timer needed since a single arm(0) picks up every pending button
        // regardless of which SHOW event triggered it. See RATIONALE.md.
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
    ULONGLONG now = GetTickCount64();
    if (now - g_lastDragFollowInvalidate < 150) {
        // Leading-edge throttle, so re-arm a trailing one-shot timer on
        // every throttled event to catch the drag/move's final position
        // once the burst goes quiet. See RATIONALE.md.
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
    // Which of these two is actually loaded depends on the Windows build -
    // see GetTaskbarViewModuleHandle. Most entries are optional so a single
    // missing symbol doesn't abort the whole batch - see RATIONALE.md.
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
        // return value - see RATIONALE.md for why a missing optional
        // symbol shouldn't discard hooks that did resolve.
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

// WinEvent hooks run on a dedicated mod-owned thread rather than the
// taskbar's own, to keep the high raw event volume off the shell's layout
// thread. See RATIONALE.md.
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

DWORD WINAPI WinEventHookThreadProc(LPVOID) {
    // Forces this thread's message queue into existence before
    // SetWinEventHook runs, so WINEVENT_OUTOFCONTEXT has somewhere to
    // deliver callbacks to immediately.
    MSG msg;
    PeekMessage(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    // Not WINEVENT_SKIPOWNPROCESS: File Explorer windows often run inside
    // explorer.exe's own process, and that flag would drop their
    // location-change events too. The taskbar's own windows are excluded
    // explicitly in WinEventProc instead.
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

    // Separate hook since the two event IDs aren't adjacent - a single
    // range spanning both would also pick up unrelated event types.
    g_showEventHook = SetWinEventHook(EVENT_OBJECT_SHOW, EVENT_OBJECT_SHOW,
                                      nullptr, WinEventProc, 0, 0,
                                      WINEVENT_OUTOFCONTEXT);
    if (!g_showEventHook) {
        Wh_Log(L"Failed to register show-event hook - a newly launched "
               L"pinned app's icon will only start following it once the "
               L"resolve timer's own backoff schedule catches up, rather "
               L"than immediately");
    }

    // Kicks off the first HWND-resolve attempt so buttons already present
    // at mod startup get picked up; NextResolveDelayMs/
    // ArmButtonHwndResolveTimer keep it armed afterward only as needed.
    g_buttonHwndResolveTimerId =
        SetTimer(nullptr, 0, 100, ButtonHwndResolveTimerProc);

    // WM_APP (posted by StopWinEventHook) is this thread's shutdown signal.
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
// Written under g_winEventThreadMutex, read without it (from the taskbar
// thread) in ArmButtonHwndResolveTimer - atomic makes that well-defined.
std::atomic<DWORD> g_winEventThreadId;

// Serializes StartWinEventHook/StopWinEventHook against each other - see
// RATIONALE.md for the race this prevents.
std::mutex g_winEventThreadMutex;
bool g_winEventThreadStopped;

void StartWinEventHook() {
    std::lock_guard<std::mutex> guard(g_winEventThreadMutex);
    if (g_winEventThreadStopped || g_winEventThread) {
        return;
    }

    // CreateThread needs a plain DWORD* out-param, published to the
    // atomic once it returns.
    DWORD threadId = 0;
    g_winEventThread = CreateThread(nullptr, 0, WinEventHookThreadProc,
                                    nullptr, 0, &threadId);
    if (!g_winEventThread) {
        Wh_Log(L"StartWinEventHook: CreateThread failed");
    } else {
        g_winEventThreadId = threadId;
    }
}

// Waits for the thread to fully exit, guaranteeing UnhookWinEvent and the
// resolve timer's KillTimer have run before Windhawk unmaps this module.
void StopWinEventHook() {
    HANDLE thread;
    DWORD threadId;
    {
        std::lock_guard<std::mutex> guard(g_winEventThreadMutex);
        // Set before releasing the lock, so a StartWinEventHook call
        // arriving after this point can't recreate a thread nobody would
        // tear down.
        g_winEventThreadStopped = true;
        thread = g_winEventThread;
        threadId = g_winEventThreadId;
        g_winEventThread = nullptr;
        g_winEventThreadId = 0;
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

// Capped exponential backoff for the click-sentinel probe, shared by
// ResolvePendingButtonHwnds and NextResolveDelayMs. Fallback safety net for
// launches that don't produce EVENT_OBJECT_SHOW; see RATIONALE.md.
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
        // A periodic re-check even once everything resolves, so a
        // drag-reorder rebind (see ButtonHwndCacheEntry) still gets
        // picked up. Only armed when something is actually resolved; goes
        // fully idle (INFINITE) only when the cache is empty.
        if (anyResolved) {
            return kIdleResolveTickMs;
        }
        return INFINITE;
    }

    DWORD pendingDelay = earliestDue > now ? (DWORD)(earliestDue - now) : 0;
    if (anyResolved) {
        // Mixed cache: cap at kIdleResolveTickMs so already-resolved
        // entries still get their rebind re-check on the normal cadence,
        // without waiting on a pending entry's own long backoff.
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

// The resolve timer lives on the dedicated WinEventHook thread, so arming
// it from the taskbar thread needs PostThreadMessage to ask that thread to
// do it on its own queue.
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
        // Not gated on HookTaskbarViewDllSymbols' own return value (which
        // reflects optional symbols too) - see RATIONALE.md. The one
        // symbol that must load, ArrangeOverride, is checked directly
        // below instead.
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

    // The WinEventHook thread is torn down later, in Wh_ModUninit - this
    // mod's hooks (and so StartWinEventHook's own call paths) stay live
    // until this function returns. Setting g_unloading first is what stops
    // the click-sentinel probe and makes IUIElement_Arrange_Hook fall
    // through to native positioning immediately.

    // Removes the subclass as early as it's safe to. This is the sole
    // marshal onto the taskbar thread (see g_taskbarWndSubclassed), so
    // once it's gone there's no way to force an immediate visual
    // snap-back on disable - native positions apply cosmetically late
    // instead, next time XAML re-runs Arrange on its own. See RATIONALE.md.
    HWND hTaskbarWnd = g_hTaskbarWnd;
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
    StopWinEventHook();
}

// Applies a freshly-loaded settings struct on the taskbar's own thread,
// since it reassigns leftApps/rightApps while a concurrent layout pass
// could be reading them. No fallback if the subclass isn't installed or
// PostMessage fails - logged loudly rather than silently dropped, since
// unlike InvalidateTaskbarLayout there's no periodic retry for a missed
// settings change. See RATIONALE.md.
void ApplyLoadedSettings(ModSettings settings) {
    // Snapshot so every read below agrees.
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

    // PostMessage is async, so ownership transfers to a heap allocation,
    // released only once PostMessage has queued it, and reclaimed by
    // TaskbarWndSubclassProc's SettingsChangedMsg case.
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

    // Reads every setting on this calling thread; only the final
    // assignment into g_settings needs the taskbar thread (see
    // ApplyLoadedSettings).
    ApplyLoadedSettings(LoadSettingsFromStore());

    InvalidateTaskbarLayout();
}
