// ==WindhawkMod==
// @id              taskbar-centered-start-split-icons
// @name            Taskbar Start Button Centered Origin
// @description     Pins the Start button to the true horizontal center of the screen, and splits running-app taskbar buttons into two groups flanking it based on which side of the screen each window is currently on (Windows 11 only)
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
  hook the same process-wide `IUIElement::Arrange` vtable slot to force
  the Start button's own X position; with both enabled, whichever one
  installed its hook second wins, and the result is undefined. This mod's
  `systemButtonsPlacement: far-left` setting covers the same "keep
  everything else out of Start's way" goal that mod's
  `otherSystemButtonsOnTheLeft` option does, so there's no reason to run
  both together.
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
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <commctrl.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
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
// widths) from inside a nested IUIElement::Arrange call - XAML's own
// layout system can be mid-structural-mutation of that same repeater
// there (reproducible: explorer.exe crash, STATUS_STOWED_EXCEPTION, when
// Windows' "show taskbar apps on" setting causes a window moving across
// monitors to change a taskbar's button *set*, not just coordinates).
// RecomputeLayoutPlan (below) does the entire traversal ONCE per
// ArrangeOverride pass, up front, before
// TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Original is called -
// i.e. before any nested Arrange calls happen - and writes every
// element's target X into g_lastArrangedX. IUIElement_Arrange_Hook then
// becomes a pure map lookup, with nothing left for XAML's mid-mutation
// state to make unsafe.

HWND g_hTaskbarWnd;
HWINEVENTHOOK g_locationChangeHook;
std::atomic<int> g_winEventRawCount;
std::atomic<int> g_winEventInvalidateCount;
std::atomic<int> g_invalidateSkippedReentrant;
std::atomic<int> g_invalidateExceptions;

// Only ever touched from the dedicated WinEventHook thread (see
// StartWinEventHook) - both WinEventProc and DragFollowTrailingTimerProc
// run there exclusively, so this needs no synchronization.
UINT_PTR g_dragFollowTrailingTimerId;

// Same thread-ownership as g_dragFollowTrailingTimerId above. Hoisted out
// of WinEventProc's own function-local static so DragFollowTrailingTimerProc
// can update it too - a trailing-timer fire is itself an invalidate, so
// the next WinEventProc event needs to see it as "now", not still throttle
// against whatever raw event last landed outside the 150ms window before
// the trailing timer took over.
ULONGLONG g_lastDragFollowInvalidate;

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

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    auto* proc = (RunFromWindowThreadProc_t*)cwp->lParam;
                    (*proc)();
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&proc);

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

HRESULT WINAPI CTaskListWnd_HandleClick_Hook(void* pThis,
                                              void* taskGroup,
                                              void* taskItem,
                                              void** launcherOptions) {
    if (launcherOptions && *launcherOptions == (void*)&g_clickSentinel) {
        g_clickSentinel_TaskItem = taskItem;
        g_clickSentinel_TaskGroup = taskGroup;
        return S_OK;
    }

    return CTaskListWnd_HandleClick_Original(pThis, taskGroup, taskItem,
                                              launcherOptions);
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
size_t GetTaskItemsArrayOffset() {
    static size_t offset = [] {
        constexpr int kIntArraySize = 256;
        int arrayOfInts[kIntArraySize];
        int* arrayOfIntPtrs[kIntArraySize];
        for (int i = 0; i < kIntArraySize; i++) {
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
    return (HDPA)((void**)taskGroup)[GetTaskItemsArrayOffset()];
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
    HWND hwnd = ResolveHwndFromIndividualTaskItem(element);
    if (hwnd) {
        return hwnd;
    }

    return ResolveHwndFromTaskGroup(element);
}

// Per-button HWND cache, keyed by the XAML element's ABI pointer. Avoids
// running the (relatively expensive) resolution chain above on every single
// layout pass for every button - only when we don't have a cached handle, or
// the cached one has stopped being a valid window.
//
// Also negatively caches failures with a short TTL: a pinned-but-not-running
// app's task group legitimately has zero windows, so its resolution will
// fail forever until it's actually launched - without this, that button's
// full multi-sentinel resolution chain gets retried on every single arrange
// pass (confirmed via g_resolveStats climbing into the hundreds within a
// couple of seconds for just one such button).
//
// consecutiveFailures drives a capped exponential backoff (2s, 4s, 8s,
// 16s, then holding at 32s) instead of retrying at a fixed 2s interval
// forever: the resolution chain ends with a synthetic ReportClicked call
// against the taskbar's real internal click handler (intercepted before
// it acts on it - see CTaskListWnd_HandleClick_Hook), and a button that
// can genuinely never resolve (the common case, exactly the one this
// negative cache exists for) would otherwise hammer that at a fixed 2s
// cadence indefinitely. A hard stop after a few failures was tried first,
// but ItemsRepeater typically recycles the same realized element (and so
// the same cache entry) for a given index rather than creating a new one
// - so a pinned-but-not-running app that fails a few times and is *then*
// actually launched would never get its HWND resolved for as long as
// that element stays realized, silently breaking the mod's own headline
// feature (side-following) for that button. Backing off instead of
// stopping keeps retrying, just less often, so a later launch is still
// picked up (worst case within 32s) without hammering the click handler
// at a fixed rate forever. A live element that's simply slow to resolve
// still gets the fast early retries, since the counter only advances on
// an actual failure and resets to 0 on the first success.
//
// identity (the button's accessible name at the time it was last resolved)
// guards against a different staleness case entirely: ItemsRepeater can
// rebind an already-realized element to a *different* item (e.g. the user
// drag-reorders two running apps' buttons) rather than destroying and
// recreating it. Without a way to notice that, a cache entry keyed by the
// element's ABI pointer would keep reporting the OLD item's HWND for the
// element's new identity indefinitely - the "hwnd is still a valid window"
// check alone can't catch this, since the old HWND is still alive, just no
// longer what this element represents. ResolvePendingButtonHwnds compares
// the live accessible name against this on every check and forces a
// re-resolve on a mismatch.
struct ButtonHwndCacheEntry {
    HWND hwnd = nullptr;
    std::wstring identity;
    ULONGLONG lastAttempt = 0;
    int consecutiveFailures = 0;
};
std::unordered_map<void*, ButtonHwndCacheEntry> g_buttonHwndCache;

// Actually runs the resolution chain and updates the cache. Returns
// whether the cached HWND changed (added, removed, or replaced) - used by
// the timer that calls this (see ResolvePendingButtonHwnds) to decide
// whether a relayout is worth triggering.
//
// Deliberately ONLY ever called from that timer, never from inside
// GetButtonHwnd/an active Arrange pass: the click-sentinel technique this
// chain uses interacts with the taskbar's own internal click-handling
// machinery, and running it while a button is being structurally
// inserted into or removed from an ItemsRepeater's data source - which
// happens whenever Windows' "show taskbar apps on" setting is anything
// other than "All taskbars" and a window moves across monitors - reaches
// STATUS_STOWED_EXCEPTION (confirmed via crash-dump analysis). Running
// it from a timer instead restores the standalone-event context this
// technique already assumes, matching where it runs in the mods it's
// adapted from.
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
// Forward-declared here so StartButtonHwndResolveTimer (Mod lifecycle
// section) can reference it before that point.
void ResolvePendingButtonHwnds();

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
    if (IsIconic(hwnd)) {
        auto it = g_lastKnownWindowClassification.find(hwnd);
        if (it != g_lastKnownWindowClassification.end()) {
            return it->second;
        }
        return {ResolveUnresolvedAppsDefaultSide()};
    }

    RECT wr;
    HMONITOR mon = MonitorFromWindow(g_hTaskbarWnd, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi{.cbSize = sizeof(mi)};
    if (!hwnd || !GetWindowRect(hwnd, &wr) || !GetMonitorInfo(mon, &mi)) {
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
// (settings, or Windows' own adaptive behavior), using a negative-margin
// collapse trick - ActualWidth() includes that margin, so it never drops
// below the collapsed width and grows on every layout pass once a
// transition starts. Confirmed against taskbar-start-button-position.wh.cpp,
// which hits the identical problem for these same button types and fixes
// it the same way: read the content child's DesiredSize instead, which
// doesn't depend on the button's own margin (falls back to ActualWidth()
// if there's no realized child yet, matching that mod exactly).
//
// Deliberately NOT used for Start (see g_lastStartWidth's own read site)
// or task list buttons (ordinary app icons, see FullFootprintWidth
// above): Start is never hidden/shown the way these three can be, so it
// was never actually exposed to the collapse-margin bug this exists to
// fix, and applying this technique to it anyway was confirmed via live
// testing to understate Start's true width - its own visual-tree
// structure isn't necessarily the same shape this technique was
// validated against. Task list buttons have no evidence of the same
// collapse-margin mechanism either, and changing their much more heavily
// used width source without that evidence isn't worth the risk.
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
// them out and to reserve room for them next to Start (in adjacent mode) so
// task list buttons on that side don't overlap them. Computed once per
// pass in RecomputeLayoutPlan (from the already-enumerated `children`) and
// written here unconditionally on every pass - not as a side effect of
// ComputeSystemButtonX running for a realized system button, which was the
// previous approach and had a real bug: if none of the three are currently
// realized in the repeater (all hidden, or a session that starts with them
// hidden), ComputeSystemButtonX never ran and these kept whatever value an
// earlier configuration left behind, permanently reserving that much dead
// space next to Start in "adjacent" mode. Zero when the cluster is
// genuinely empty now, always.
double g_lastLeftSystemClusterWidth = 0;
double g_lastRightSystemClusterWidth = 0;

// A repeater child element paired with its SystemButton classification,
// computed once per child per RecomputeLayoutPlan pass instead of
// re-deriving IdentifySystemButton
// (a winrt::get_class_name call - IInspectable::GetRuntimeClassName plus
// an HSTRING allocation) every time it's needed. Previously recomputed up
// to 4 times per child per pass: once each in the Start-finding, cluster-
// width, and system-button-placement loops, plus once per child *again*
// inside ComputeSystemButtonX's own widthBefore loop for every realized
// system button (itself also re-walking the repeater via
// GetRepeaterChildElements instead of reusing what RecomputeLayoutPlan
// already enumerated).
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
        // Stack right-to-left outward from Start: lowest rank closest.
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

// Computes every task list button's target X in a single O(n) pass over
// the repeater's already-enumerated children, classifying each button
// exactly once - avoids both re-walking the repeater and re-classifying
// every sibling for every single button placed, and the same up-front
// classification means a button's side/order can't disagree with itself
// depending on whether it's being visited as a target or as someone
// else's sibling.
//
// Every entry is always written into outPlan and stays in this mod's own
// coordinate system, never falling through to native Arrange even when a
// side overflows: Start's position is forced to true screen-center by
// this mod's own hook regardless of what any individual task list button
// does, and native's layout algorithm has no knowledge of that - mixing
// forced-position elements with native-position elements in the same
// pass produces a visually incoherent overlap. Instead, a side that
// doesn't fit gets its inter-icon spacing proportionally compressed
// (icons still render at natural width, so they visually overlap each
// other once genuinely crowded). leftBoundLocal/rightBoundLocal are the
// outer edges (local DIPs, same space as startCenterX) each side's group
// is compressed to fit within - leftBoundLocal is the taskbar's left
// edge, pushed right by the Search/Task View/Widgets cluster's width in
// "far left" placement mode; rightBoundLocal is the system tray's own
// left edge (see RecomputeLayoutPlan's comment), falling back to the
// taskbar's own outer edge if the tray element can't be found this pass.
//
// Every side is walked innermost-first, starting exactly at Start's own
// edge (minus gap) and moving outward. The Start-overlap guarantee: the
// innermost icon's edge *facing Start* must sit at that fixed,
// bound-independent reference point regardless of scale, so each icon's
// own placement uses its unscaled width - only the running reference
// point handed to the *next* icon advances by the scaled amount. Scaling
// the placement itself (not just the step to the next icon) looks
// equivalent at scale=1 but drifts the innermost icon into Start as
// compression increases - do not reintroduce that.
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
        // Preserve taskbar order, read left-to-right the same way the
        // native taskbar does: the earliest entry in taskbar order on a
        // side sits at that side's outer edge, and later entries sit
        // progressively closer to Start - i.e. innermost-last. `left`/
        // `right` are already in taskbar order (entries was built that
        // way); the right side's innermost-last order already matches
        // the walk order used below (accumulating outward from Start),
        // but the left side needs reversing to innermost-first - walking
        // it in original taskbar order here would put the *earliest*
        // entry innermost instead, mirroring the group relative to
        // native order.
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
    // rendered width, which this mod never touches) so the outermost
    // icon's own position never passes leftBoundLocal/rightBoundLocal,
    // at the cost of consecutive icons visually overlapping each other
    // once a side is genuinely too full to fit at natural spacing. A
    // side with plenty of room keeps scale at 1 (identical to natural,
    // uncompressed spacing).
    //
    // The outermost icon's own width (left.back()/right.back() - the
    // walk loop below always processes them last, regardless of sort
    // order) is deliberately excluded from both the total and the
    // available space fed into the ratio. That icon is placed at its
    // own full, unscaled width with nothing beyond it to compress
    // against (mirroring the innermost icon's exact placement against
    // Start - see the walk loop's own comment), so folding its width
    // into the same pool as the compressible pitches double-counted it:
    // once inside the ratio, again in full at the final placement. That
    // under-compressed every pitch just enough for the outermost icon's
    // own edge to overshoot the bound by width*(1-scale) - confirmed
    // live as running-app icons overlapping the far-left system buttons
    // (Task View), while Start itself was never overlapped since the
    // innermost-icon side of this class of bug was already fixed.
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

    // The innermost icon's position is placed using its own *unscaled*
    // width (x - entry->width, not x - entry->width * leftScale) so its
    // right edge lands exactly at leftInnerX regardless of scale - only
    // x itself (the reference point carried to the *next* icon) advances
    // by the scaled amount. An earlier version scaled the position too
    // (x -= entry->width * leftScale; outPlan[...] = x), which is exactly
    // right at scale=1 but drifts the innermost icon's right edge into
    // Start by width*(1-scale) as compression increases - the exact
    // Start-overlap this whole mechanism exists to prevent, caught by
    // review rather than by testing since the two live-tested overflow
    // scenarios both happened to stress the right side (which never had
    // this bug - its innermost icon is placed at the unscaled rightInnerX
    // directly, with no equivalent subtraction to get wrong).
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

// Defined later (Mod lifecycle section); forward-declared here so
// EnsureTaskbarWnd (below) can start the drag-follow WinEventHook as soon
// as the taskbar window resolves, whether that happens at normal startup
// or late (see EnsureTaskbarWnd's comment).
void StartWinEventHook();

// Defined later (Mod lifecycle section); forward-declared here for the
// same reason as StartWinEventHook above - starts the timer that drives
// ResolvePendingButtonHwnds.
void StartButtonHwndResolveTimer();

// Defined later (Mod lifecycle section); forward-declared here so the
// ArrangeOverride hook below can request an immediate HWND-resolve attempt
// (delayMs = 0) as soon as it notices the task list button count changed,
// rather than waiting for whatever delay the timer last computed for
// itself - see NextResolveDelayMs' comment for why it can't see a
// brand-new button coming on its own.
void ArmButtonHwndResolveTimer(DWORD delayMs);

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
    }

    if (g_hTaskbarWnd) {
        return g_hTaskbarWnd;
    }
    if (g_unloading) {
        // Guards against Wh_ModBeforeUninit's own InvalidateTaskbarLayout
        // call (or a naturally-timed pass) landing while the mod's hooks
        // are still installed but g_hTaskbarWnd was somehow never set -
        // without this, resolving it here would call
        // StartWinEventHook()/StartButtonHwndResolveTimer() and create a
        // thread/timer with no later Wh_ModUninit call left to tear it
        // down (Wh_ModBeforeUninit already ran).
        return nullptr;
    }

    g_hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (g_hTaskbarWnd) {
        Wh_Log(L"Resolved taskbar window: %p", g_hTaskbarWnd);
        StartWinEventHook();
        StartButtonHwndResolveTimer();
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

// Walks the primary taskbar's current TaskListButtons and (re)resolves any
// whose cache entry is missing, stale (window closed), or past the
// negative-cache TTL - see ResolveAndCacheButtonHwnd's comment for the
// full story on why this runs on a timer (StartButtonHwndResolveTimer,
// Mod lifecycle section) instead of inline during Arrange.
//
// Guarded against running while nested inside an active Arrange pass on
// this thread - defense in depth, not the primary protection. Windows
// generally doesn't deliver WM_TIMER re-entrantly mid-callback on the
// same thread, but there's no hard guarantee of that documented, and the
// check is nearly free.
void ResolvePendingButtonHwnds() {
    if (g_inTaskbarArrangeOverride || !g_hTaskbarWnd) {
        return;
    }

    XamlRoot xamlRoot = GetTaskbarXamlRoot(g_hTaskbarWnd);
    if (!xamlRoot) {
        return;
    }

    FrameworkElement content = xamlRoot.Content().try_as<FrameworkElement>();
    FrameworkElement repeater = FindTaskbarFrameRepeater(content);
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
                int shift = std::min(it->second.consecutiveFailures, 4);
                ULONGLONG backoffMs = 2000ULL << shift;  // 2s..32s
                needsResolve = now - it->second.lastAttempt >= backoffMs;
            }
        }

        if (needsResolve && ResolveAndCacheButtonHwnd(child, identity)) {
            anyChanged = true;
        }
    }

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
        it = liveTaskListButtons.find(it->first) == liveTaskListButtons.end()
                 ? g_buttonHwndCache.erase(it)
                 : std::next(it);
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

    if (anyChanged) {
        InvalidateTaskbarLayout();
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
// reflect yet - every InvalidateTaskbarLayout call covers every real
// trigger this mod has (see its own top-of-function write): a window
// moving, a button's HWND/side resolving, the ArrangeOverride hook's own
// button-count-change check, a settings change, and mod startup. Starts
// true so the very first ArrangeOverride pass always computes a real
// plan rather than reusing an empty one. RecomputeLayoutPlan clears it
// only after a genuinely successful recompute - left set on an exception
// so a later pass retries rather than silently freezing on a broken or
// incomplete plan. atomic: InvalidateTaskbarLayout can be called from
// threads other than the taskbar's own (the dedicated WinEventHook
// thread registers WinEventProc there, for one), while
// RecomputeLayoutPlan only ever reads/clears this from the taskbar
// thread itself (guarded the same way the rest of that function already
// is).
//
// Previously this whole function - a taskbar.dll vtable scan and
// TaskbarHost::FrameHeight prologue parse to reach the XamlRoot, a
// VisualTreeHelper walk down to the repeater, a TryGetElement and
// winrt::get_class_name per child - ran unconditionally on every single
// ArrangeOverride pass, for every taskbar (primary and any
// secondary-monitor ones, since they share the primary's own thread and
// so pass the apartment-safety check this function already needed
// regardless). This flag removes nearly all of that cost on the (very
// common) passes where nothing this mod cares about actually changed,
// without touching the plan-computation logic itself - it's a pure
// short-circuit around code whose correctness properties have already
// been fought for across several crash-debugging sessions, not a rewrite
// of it.
std::atomic<bool> g_planDirty{true};

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
    // so it's invoked directly by XAML's own native call sites for every
    // UIElement being arranged anywhere in explorer.exe - a raw ABI
    // boundary with no C++/WinRT exception translation on the other side.
    // A WinRT call throwing here (observed via a real crash: explorer.exe
    // faulting in Windows.UI.Xaml.dll with STATUS_STOWED_EXCEPTION,
    // 0xC000027B, at enabling a second monitor - a fresh secondary-taskbar
    // XAML tree briefly has elements with no parent yet mid-construction,
    // and .as<T>() throws on a null source where .try_as<T>() would just
    // return null) crosses that boundary uncaught and fail-fasts the whole
    // process. Catch anything and fall back to the real implementation
    // rather than let that happen again. The QueryInterface below is the
    // only WinRT call left in this function - everything else is a plain
    // map lookup - so this net is mostly a leftover safety margin at this
    // point rather than something expected to actually catch anything.
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
// TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Hook BEFORE it calls
// into XAML's own ArrangeOverride, i.e. before any nested Arrange calls
// (and so IUIElement_Arrange_Hook above) run at all for this pass - see
// g_inTaskbarArrangeOverride's comment for why that ordering matters.
//
// Rebuilds g_lastArrangedX from scratch every real recompute rather than
// updating incrementally, so a destroyed element's stale entry can never
// outlive it - no separate pruning pass needed for this map, unlike
// g_buttonHwndCache/g_lastKnownWindowClassification (populated
// independently by the HWND-resolve timer - see ResolvePendingButtonHwnds
// for why those still need explicit pruning).
//
// MUST only run when confirmed to be on g_hTaskbarWnd's own thread:
// GetTaskbarXamlRoot(g_hTaskbarWnd) reaches across to the PRIMARY's XAML
// object specifically, an unsafe unmarshaled cross-apartment WinRT call
// from any other thread. Shell_TrayWnd and Shell_SecondaryTrayWnd both
// run on the SAME Explorer UI thread, so this check passes for
// secondary-monitor ArrangeOverride passes too - harmless either way,
// since secondary-monitor elements never end up in g_lastArrangedX
// regardless (this function only ever walks the primary's own
// repeater); the check exists purely as the apartment-safety guard, not
// a monitor-scoping mechanism.
void RecomputeLayoutPlan() {
    if (!g_hTaskbarWnd) {
        return;
    }
    DWORD primaryThreadId = GetWindowThreadProcessId(g_hTaskbarWnd, nullptr);
    if (primaryThreadId == 0 || primaryThreadId != GetCurrentThreadId()) {
        return;
    }
    if (!g_planDirty) {
        return;
    }

    g_planStats = {};

    try {
        XamlRoot xamlRoot = GetTaskbarXamlRoot(g_hTaskbarWnd);
        if (!xamlRoot) {
            return;
        }
        FrameworkElement content = xamlRoot.Content().try_as<FrameworkElement>();
        FrameworkElement repeater = FindTaskbarFrameRepeater(content);
        if (!repeater) {
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
                // Deliberately bare ActualWidth() here, NOT
                // SystemButtonContentWidth - confirmed via live testing
                // that swapping this specific read to the content-child's
                // DesiredSize() (done for Search/Task View/Widgets below,
                // where it's genuinely needed) caused task list icons to
                // visibly render on top of Start's own icon on the side
                // that pushes toward it, meaning the content child's
                // DesiredSize understates Start's true width - Start's own
                // visual-tree structure isn't necessarily the same shape
                // that technique was validated against. ActualWidth()'s
                // collapse-margin growth problem (see
                // SystemButtonContentWidth's comment) is specific to
                // elements that actually get hidden/shown - Start never
                // is, unlike Search/Task View/Widgets, so it was never
                // actually exposed to that bug in the first place. Four
                // full rounds of live testing never reported a Start-width
                // problem before this specific swap was tried.
                //
                // ActualWidth() still reflects the previous arrange pass,
                // and is 0 for a just-realized element - a freshly
                // (re)created Start button would otherwise briefly clobber
                // g_lastStartWidth to 0 and lay out the entire taskbar
                // around a zero-width Start for that pass. Keep the last
                // known-good width instead; it'll catch up to the real one
                // within a pass or two.
                double w = info.element.ActualWidth();
                if (w > 0) {
                    g_lastStartWidth = w;
                }
                newPlan[winrt::get_abi(info.element)] =
                    startCenterX - g_lastStartWidth / 2.0;
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

// Guards against reentering InvalidateTaskbarLayout itself on the
// taskbar's own thread. WinEventProc can fire in rapid bursts (observed:
// thousands of raw events within seconds while something on screen is
// spamming EVENT_OBJECT_LOCATIONCHANGE).
//
// This lambda deliberately never calls UpdateLayout() - see the note on
// InvalidateTaskbarLayout below for why forcing a synchronous layout pass
// here is unsafe regardless of this guard.
thread_local bool g_inInvalidateTaskbarLayout;

// Marks the taskbar's layout dirty and lets the XAML dispatcher pick it up
// on its own next tick, rather than forcing an immediate synchronous
// UpdateLayout() call. This function's callers include a raw OS callback
// (WinEventProc) that can itself fire while the thread is already nested
// inside XAML-internal layout activity for reasons outside this mod's
// control (e.g. a taskbar button structurally moving between two
// different monitors' XAML trees, not just changing coordinates within
// one). A forced UpdateLayout() in that state is a reentrant "layout
// cycle" from WinUI's perspective, and fails fast with
// STATUS_STOWED_EXCEPTION - confirmed as the cause of several
// explorer.exe crash-loop incidents, each via a different trigger path,
// so the only fix that actually closes the whole class is to never force
// it anywhere. InvalidateArrange() + InvalidateMeasure() alone are always
// safe to call from any context; the resulting delay before the
// dispatcher's own pass runs is at most one composition frame - not
// perceptible for a drag-follow feature already documented as
// best-effort.
//
// STATUS_STOWED_EXCEPTION is a raw SEH RaiseException, not a thrown C++
// exception - this mod's toolchain (Windhawk's clang/MinGW build, no
// /EHa-style async exception handling) cannot catch it with `catch(...)`
// no matter where it's placed. The try/catch below only covers genuine
// C++ exceptions from the WinRT calls in this lambda (e.g. QueryInterface
// failures) - it is not, and never will be, a safety net for the
// layout-cycle failure. Don't reintroduce a forced UpdateLayout() here
// without a fundamentally different mechanism (e.g. a genuinely async
// PostMessage-deferred call guaranteed to run only once the current call
// stack has fully unwound).
void InvalidateTaskbarLayout() {
    if (!g_hTaskbarWnd) {
        return;
    }

    // Every real trigger for a layout change goes through this function
    // (see g_planDirty's own comment for the full list), so marking the
    // plan dirty here - unconditionally, before the marshal below, so it
    // still happens even if the marshal itself fails - is the one place
    // that needs to know about all of them.
    g_planDirty = true;

    bool posted = RunFromWindowThread(g_hTaskbarWnd, [] {
        if (g_inInvalidateTaskbarLayout) {
            g_invalidateSkippedReentrant++;
            return;
        }
        g_inInvalidateTaskbarLayout = true;

        try {
            XamlRoot xamlRoot = GetTaskbarXamlRoot(g_hTaskbarWnd);
            if (!xamlRoot) {
                Wh_Log(L"InvalidateTaskbarLayout: GetTaskbarXamlRoot failed");
                g_inInvalidateTaskbarLayout = false;
                return;
            }

            FrameworkElement content =
                xamlRoot.Content().try_as<FrameworkElement>();
            FrameworkElement repeater = FindTaskbarFrameRepeater(content);
            if (!repeater) {
                Wh_Log(
                    L"InvalidateTaskbarLayout: FindTaskbarFrameRepeater "
                    L"failed");
                g_inInvalidateTaskbarLayout = false;
                return;
            }

            repeater.InvalidateArrange();
            repeater.InvalidateMeasure();
        } catch (...) {
            g_invalidateExceptions++;
        }

        g_inInvalidateTaskbarLayout = false;
    });
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

    // Same g_unloading check WinEventProc already has: InvalidateTaskbarLayout
    // blocks in SendMessage until the taskbar thread pumps it, which
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

    if (!IsWindowVisible(hwnd) || GetAncestor(hwnd, GA_ROOT) != hwnd ||
        GetWindow(hwnd, GW_OWNER) != nullptr) {
        return;
    }

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
// system, not just windows this mod cares about - "thousands of raw
// events within seconds" has been observed from something on the system
// spamming it. WINEVENT_OUTOFCONTEXT delivers those callbacks on whichever
// thread called SetWinEventHook, and only if that thread pumps messages.
// Registering on g_hTaskbarWnd's own thread (as an earlier version of this
// function did, via RunFromWindowThread) put every one of those events -
// plus WinEventProc's IsWindowVisible/GetAncestor/GetWindow calls on each
// one, all before the 150ms throttle even applies - in direct contention
// with the shell's own layout work on the one thread responsible for it.
// A dedicated mod-owned thread with its own message loop (the pattern
// taskbar-background-helper.wh.cpp and
// taskbar-auto-hide-when-maximized.wh.cpp both use for the same kind of
// global WinEventHook) keeps all of that off the shell's thread entirely -
// InvalidateTaskbarLayout already marshals onto the taskbar thread for the
// one thing that actually needs to run there.
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

    // WM_APP (posted by StopWinEventHook) is this thread's shutdown
    // signal - it has no window to route to, so it's read directly out of
    // the queue rather than via a window procedure.
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_APP) {
            break;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_locationChangeHook) {
        UnhookWinEvent(g_locationChangeHook);
        g_locationChangeHook = nullptr;
    }

    return 0;
}

HANDLE g_winEventThread;
DWORD g_winEventThreadId;

void StartWinEventHook() {
    if (g_winEventThread) {
        return;
    }

    g_winEventThread = CreateThread(nullptr, 0, WinEventHookThreadProc,
                                    nullptr, 0, &g_winEventThreadId);
    if (!g_winEventThread) {
        Wh_Log(L"StartWinEventHook: CreateThread failed");
    }
}

// RunFromWindowThread can only fail two ways: the taskbar's thread is
// already gone (GetWindowThreadProcessId returns 0 - in which case the
// OS already tore down any timer that thread owned, so there's nothing
// left to clean up), or its own internal SetWindowsHookEx call failed
// (rare, e.g. transient resource exhaustion) while the thread is still
// very much alive. That second case is the dangerous one: nothing under
// mod control runs on the taskbar thread to unregister the real
// HWND-resolve timer, Wh_ModUninit returns anyway, Windhawk unmaps this
// module's code, and the still-alive thread's next tick calls into
// unmapped memory. A few retries gives that transient failure a chance to
// clear without risking an unbounded stall if the thread really is gone.
// (The WinEventHook itself no longer goes through this - see
// StopWinEventHook, which owns its thread outright instead.)
bool RunFromWindowThreadWithRetry(HWND hWnd, RunFromWindowThreadProc_t proc) {
    for (int attempt = 0; attempt < 3; attempt++) {
        if (RunFromWindowThread(hWnd, proc)) {
            return true;
        }
        if (GetWindowThreadProcessId(hWnd, nullptr) == 0) {
            break;
        }
        Sleep(20);
    }
    return false;
}

// Unlike the RunFromWindowThread-based teardowns below (which depend on
// g_hTaskbarWnd's thread still being alive and responsive), this thread is
// entirely mod-owned: PostThreadMessage can't silently fail the way a
// marshaled call onto someone else's thread can, and waiting for the
// thread to actually exit (rather than just requesting it) guarantees
// UnhookWinEvent has already run, on the thread that registered it, by
// the time this returns - before Windhawk unmaps this module's code.
void StopWinEventHook() {
    if (!g_winEventThread) {
        return;
    }

    // WinEventHookThreadProc's own first action creates its message
    // queue via PeekMessage before doing anything else (see its
    // comment), but there's still a small window right after
    // CreateThread returns where the new thread hasn't run that yet -
    // PostThreadMessage fails with ERROR_INVALID_THREAD_ID in that
    // window, and StartWinEventHook is called lazily from
    // EnsureTaskbarWnd, so thread creation isn't necessarily far from
    // teardown. Retry briefly to bridge that narrow gap.
    for (int attempt = 0; attempt < 50; attempt++) {
        if (PostThreadMessage(g_winEventThreadId, WM_APP, 0, 0)) {
            break;
        }
        Sleep(10);
    }

    // Unconditional wait, not a bounded timeout - an earlier version
    // gave up after 2s and returned anyway, which meant Wh_ModUninit
    // could return, Windhawk unmaps this module's code via FreeLibrary
    // right after, and the still-running thread's next instruction
    // (its own return address, still inside this module's
    // WinEventHookThreadProc) is now unmapped memory - a deferred
    // crash, not a mitigated one. Matches the established teardown
    // shape in taskbar-background-helper.wh.cpp.
    WaitForSingleObject(g_winEventThread, INFINITE);

    CloseHandle(g_winEventThread);
    g_winEventThread = nullptr;
    g_winEventThreadId = 0;
}

// Not 1: this is a window (Shell_TrayWnd) the mod doesn't own, shared with
// Explorer's own timers and any other mod that also sets timers on it -
// SetTimer with an already-used ID silently replaces that timer, so a
// small/common value like 1 is a real collision risk. Arbitrary otherwise,
// same pattern other mods use for shell-window timers (e.g.
// classic-taskbar-properties.wh.cpp's kTimerIdMasterLayout).
constexpr UINT_PTR kButtonHwndResolveTimerId = 0x8C3F;

// Idle re-check cadence once every cached button is already resolved -
// see NextResolveDelayMs' comment for what this specifically exists to
// catch. Deliberately not as tight as the 2s..32s backoff used for
// buttons that still need resolving - this only ever runs
// ResolvePendingButtonHwnds' comparison against already-known-good
// state, not the (much more expensive, and the one actually worth
// keeping rare) click-sentinel resolution chain.
constexpr DWORD kIdleResolveTickMs = 8000;

// How long until the next resolve attempt could possibly do anything
// useful, based only on g_buttonHwndCache's already-recorded state - no
// XAML/tree access, just a scan of an in-memory map, so this is cheap
// enough to call after every timer tick. Mirrors the same backoff formula
// ResolvePendingButtonHwnds uses to decide needsResolve for an existing
// entry.
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
        int shift = std::min(entry.consecutiveFailures, 4);
        ULONGLONG dueAt = entry.lastAttempt + (2000ULL << shift);
        if (!anyPending || dueAt < earliestDue) {
            earliestDue = dueAt;
            anyPending = true;
        }
    }

    if (!anyPending) {
        // Even once every cached button is resolved, ItemsRepeater can
        // rebind an already-realized element to a *different* item (a
        // drag-reorder of two running apps) without changing the total
        // count - the one thing that otherwise re-arms this timer, via
        // the ArrangeOverride hook's count-change check. Without some
        // periodic re-check here, ResolvePendingButtonHwnds' identity
        // comparison (the mechanism that exists specifically to catch a
        // rebind - see ButtonHwndCacheEntry's comment) would never
        // actually run again once everything first resolves, so a
        // rebind could go undetected indefinitely. Only armed when at
        // least one button is genuinely resolved - an all-pinned/
        // not-running taskbar already gets its own periodic tick from
        // the backoff loop above, no need to add a second one; the
        // timer only truly goes idle (returns INFINITE, no re-arm at
        // all) when the cache is completely empty.
        if (anyResolved) {
            return kIdleResolveTickMs;
        }
        return INFINITE;
    }

    return earliestDue > now ? (DWORD)(earliestDue - now) : 0;
}

void CALLBACK ButtonHwndResolveTimerProc(HWND hwnd,
                                         UINT uMsg,
                                         UINT_PTR idEvent,
                                         DWORD dwTime) {
    // Self-managing one-shot rather than a recurring interval: stop first,
    // do the actual work, then only re-arm if there's a concrete reason
    // to. Without this, the timer would fire at a fixed cadence forever -
    // running the full resolve pass (XamlRoot lookup, a visual-tree walk,
    // a click-sentinel probe per due button) even while everything is
    // already resolved and nothing has changed, which was flagged as a
    // real objection on submissions here.
    KillTimer(hwnd, kButtonHwndResolveTimerId);

    ResolvePendingButtonHwnds();

    DWORD delay = NextResolveDelayMs();
    if (delay != INFINITE) {
        SetTimer(hwnd, kButtonHwndResolveTimerId, delay,
                 ButtonHwndResolveTimerProc);
    }
}

// SetTimer's callback fires on whichever thread registered it, so this is
// marshaled onto g_hTaskbarWnd's own thread - both KillTimer and (for
// reliability) SetTimer itself are documented to need to run on the
// thread that owns the timer.
void ArmButtonHwndResolveTimer(DWORD delayMs) {
    if (!g_hTaskbarWnd || g_unloading) {
        // See EnsureTaskbarWnd's g_unloading check - same reasoning: once
        // Wh_ModBeforeUninit sets g_unloading, don't create a new
        // SetTimer registration (whose TimerProc is in this module's own
        // code) even though the Arrange hook that could still trigger
        // this (the count-change branch) stays installed until
        // Wh_ModBeforeUninit returns - StopButtonHwndResolveTimer doesn't
        // run until the later Wh_ModUninit call, by which point the hook
        // is gone and nothing could reach here to recreate it anyway.
        return;
    }

    if (!RunFromWindowThread(g_hTaskbarWnd, [delayMs] {
            SetTimer(g_hTaskbarWnd, kButtonHwndResolveTimerId, delayMs,
                     ButtonHwndResolveTimerProc);
        })) {
        Wh_Log(L"ArmButtonHwndResolveTimer: RunFromWindowThread failed, "
               L"HWND resolution will not run");
    }
}

// Kicks off an initial resolve attempt shortly after the taskbar window
// resolves, so buttons already present at mod startup get picked up -
// after that, NextResolveDelayMs and the ArrangeOverride hook's
// button-count-change check keep the timer armed only for as long as
// there's actually something to do.
void StartButtonHwndResolveTimer() {
    ArmButtonHwndResolveTimer(100);
}

void StopButtonHwndResolveTimer() {
    if (!g_hTaskbarWnd) {
        return;
    }

    // See RunFromWindowThreadWithRetry's own comment for why a bounded
    // retry rather than an inline KillTimer from the wrong thread here.
    if (!RunFromWindowThreadWithRetry(g_hTaskbarWnd, [] {
            KillTimer(g_hTaskbarWnd, kButtonHwndResolveTimerId);
        })) {
        Wh_Log(L"StopButtonHwndResolveTimer: RunFromWindowThread failed, "
               L"timer left running");
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
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"Taskbar view module not loaded yet, will hook on load");

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

    // Deliberately NOT tearing down the WinEventHook thread/timer here -
    // this mod's own hooks (IUIElement_Arrange_Hook,
    // TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Hook) stay
    // installed until this function *returns*, and ArrangeOverride isn't
    // itself gated on g_unloading. If StopWinEventHook/
    // StopButtonHwndResolveTimer ran here and a pass landed in the
    // window before this function returns, EnsureTaskbarWnd could still
    // call StartWinEventHook/StartButtonHwndResolveTimer again (if
    // g_hTaskbarWnd somehow wasn't set yet) or the count-change branch
    // could call ArmButtonHwndResolveTimer(0) - either creates a brand
    // new thread/timer nobody would ever tear down, since Wh_ModUninit
    // runs after this. This call still needs the Arrange hook alive
    // (it's what restores native positioning), so it has to happen while
    // the hooks are still installed - the actual teardown of the
    // thread/timer that could otherwise be reawakened waits for
    // Wh_ModUninit, after Windhawk has removed the hooks.
    InvalidateTaskbarLayout();
}

void Wh_ModUninit() {
    Wh_Log(L">");

    // See Wh_ModBeforeUninit's comment for why this doesn't run there -
    // the mod's own hooks are gone by the time Wh_ModUninit runs, so
    // nothing can reawaken the thread/timer being torn down here.
    StopWinEventHook();
    StopButtonHwndResolveTimer();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    // Read every setting on this calling thread first -
    // Wh_Get/FreeStringSetting don't touch XAML/COM, so they don't need
    // to run on the taskbar thread at all. Only the final assignment into
    // g_settings is marshaled: it reassigns leftApps/rightApps
    // (std::vector<std::wstring>), which a concurrent ContainsAnyFragment
    // call during a layout pass on the taskbar thread could otherwise
    // read mid-reassignment. Doing the reads here means the RunFromWindowThread
    // call below blocks the taskbar thread (which may be in the middle of
    // handling this via WH_CALLWNDPROC/SendMessage) for a plain struct
    // move instead of several setting reads plus string work.
    ModSettings settings = LoadSettingsFromStore();

    if (g_hTaskbarWnd) {
        if (!RunFromWindowThread(g_hTaskbarWnd,
                                 [settings = std::move(settings)]() mutable {
                                     g_settings = std::move(settings);
                                 })) {
            Wh_Log(L"Wh_ModSettingsChanged: RunFromWindowThread failed, "
                   L"new settings not applied");
        }
    } else {
        g_settings = std::move(settings);
    }

    InvalidateTaskbarLayout();
}
