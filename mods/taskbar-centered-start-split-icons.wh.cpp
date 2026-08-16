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

When you drag a window across the center line of the screen, its taskbar
button switches sides to follow it. Side-switching is driven by a global
window-location-change listener and is best-effort: it happens shortly
after a drag/move settles, not on every intermediate pixel of the drag.

Search, Task View and Widgets can either stay at the far left edge, or move
right next to Start on whichever side you prefer.

## Known limitations (please read before reporting issues)

- **Windows 11 only.** Windows 10's taskbar has no XAML layer to hook into.
- **Primary monitor only.** Screen-center math and window-side classification
  use the primary monitor. Taskbars on secondary displays are not specially
  handled by this mod (their icons keep the default layout Windows gives
  them) - the positioning hook explicitly checks which taskbar's XAML tree
  an element belongs to and leaves anything outside the primary's alone.
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
    View/Widgets" tracks the systemButtonsAdjacentSide setting below when
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

struct {
    double gapPx;
    SystemButtonsPlacement systemButtonsPlacement;
    Side systemButtonsAdjacentSide;
    std::vector<std::wstring> leftApps;
    std::vector<std::wstring> rightApps;
    UnresolvedAppsDefaultSide unresolvedAppsDefaultSide;
    TaskListOrder taskListOrder;
    PinnedAppsAnchor pinnedAppsAnchor;
} g_settings;

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
    if (!raw) {
        return result;
    }

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
    if (value && wcscmp(value, L"right") == 0) {
        return Side::Right;
    }
    if (value && wcscmp(value, L"left") == 0) {
        return Side::Left;
    }
    return fallback;
}

UnresolvedAppsDefaultSide ParseUnresolvedAppsDefaultSide(PCWSTR value) {
    if (value && wcscmp(value, L"right") == 0) {
        return UnresolvedAppsDefaultSide::Right;
    }
    if (value && wcscmp(value, L"contralateral-to-system-buttons") == 0) {
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
    if (value && wcscmp(value, L"adjacent-to-start") == 0) {
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

void LoadSettings() {
    g_settings.gapPx = Wh_GetIntSetting(L"gapPx");

    PCWSTR placement = Wh_GetStringSetting(L"systemButtonsPlacement");
    g_settings.systemButtonsPlacement =
        (placement && wcscmp(placement, L"adjacent-start") == 0)
            ? SystemButtonsPlacement::AdjacentStart
            : SystemButtonsPlacement::FarLeft;
    Wh_FreeStringSetting(placement);

    PCWSTR adjacentSide = Wh_GetStringSetting(L"systemButtonsAdjacentSide");
    g_settings.systemButtonsAdjacentSide = ParseSide(adjacentSide, Side::Left);
    Wh_FreeStringSetting(adjacentSide);

    PCWSTR leftApps = Wh_GetStringSetting(L"leftApps");
    g_settings.leftApps = ParseAppList(leftApps);
    Wh_FreeStringSetting(leftApps);

    PCWSTR rightApps = Wh_GetStringSetting(L"rightApps");
    g_settings.rightApps = ParseAppList(rightApps);
    Wh_FreeStringSetting(rightApps);

    PCWSTR defaultSide = Wh_GetStringSetting(L"unresolvedAppsDefaultSide");
    g_settings.unresolvedAppsDefaultSide =
        ParseUnresolvedAppsDefaultSide(defaultSide);
    Wh_FreeStringSetting(defaultSide);

    PCWSTR taskListOrder = Wh_GetStringSetting(L"taskListOrder");
    g_settings.taskListOrder =
        (taskListOrder && wcscmp(taskListOrder, L"taskbar-order") == 0)
            ? TaskListOrder::TaskbarOrder
            : TaskListOrder::DistanceFromCenter;
    Wh_FreeStringSetting(taskListOrder);

    PCWSTR pinnedAppsAnchor = Wh_GetStringSetting(L"pinnedAppsAnchor");
    g_settings.pinnedAppsAnchor = ParsePinnedAppsAnchor(pinnedAppsAnchor);
    Wh_FreeStringSetting(pinnedAppsAnchor);
}

// ============================================================================
// Globals
// ============================================================================

std::atomic<bool> g_taskbarViewDllLoaded;
std::atomic<bool> g_unloading;

thread_local bool g_inTaskbarArrangeOverride;

// A recurring explorer.exe crash (STATUS_STOWED_EXCEPTION, confirmed via
// WinDbg crash-dump analysis across several sessions) traced back to every
// synchronous WinRT repeater-traversal call this file makes - reading
// sibling elements, classifying them, measuring widths - being made from
// *inside* IUIElement::Arrange, while XAML's own layout system can be
// mid-structural-mutation of that same repeater (only reproduces when
// Windows' "show taskbar apps on" setting causes a window moving across
// monitors to change a taskbar's button *set*, not just coordinates).
// Earlier attempts suppressed that traversal for a timing window after a
// detected change (a "settling window") rather than eliminating it - a
// real mitigation, but complex (needed its own recovery-guarantee and
// re-arm-loop bug fixes) and still not the actual fix.
//
// The actual fix: never call any of that traversal from inside a nested
// Arrange call at all. RecomputeLayoutPlan (below) does the entire
// traversal ONCE per ArrangeOverride pass, up front, before
// TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Original is called
// (i.e. before any nested Arrange calls happen), and writes every
// element's target X into g_lastArrangedX. IUIElement_Arrange_Hook then
// becomes a pure map lookup - no traversal, no WinRT calls beyond the one
// QueryInterface needed to identify the element, so there's nothing left
// for XAML's mid-mutation state to make unsafe.

HWND g_hTaskbarWnd;
HWINEVENTHOOK g_locationChangeHook;
std::atomic<int> g_winEventRawCount;
std::atomic<int> g_winEventInvalidateCount;
std::atomic<int> g_invalidateSkippedReentrant;
std::atomic<int> g_invalidateExceptions;

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
void* g_clickSentinel_TaskItem;
void* g_clickSentinel_TaskGroup;

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

// Diagnostics only: which stage of the WinRT -> native resolution chain
// last failed, and how often, so a systemic failure (e.g. a different
// view-model type when taskbar buttons are grouped/combined) is visible
// without guessing.
struct ResolveStats {
    int success = 0;
    int failViewModelNull = 0;
    int failGetTaskItem = 0;
    int failSentinelNoItem = 0;
    int groupSuccess = 0;
    int groupFailViewModelNull = 0;
    int groupFailSentinelNoGroup = 0;
    int groupFailNoItems = 0;
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
        g_resolveStats.failViewModelNull++;
        return nullptr;
    }

    winrt::com_ptr<IUnknown> windowsUdkTaskItem;
    if (FAILED(TaskListWindowViewModel_get_TaskItem_Original(
            windowViewModel.get(), windowsUdkTaskItem.put_void())) ||
        !windowsUdkTaskItem) {
        g_resolveStats.failGetTaskItem++;
        return nullptr;
    }

    g_clickSentinel_TaskItem = nullptr;
    TaskItem_ReportClicked_Original(windowsUdkTaskItem.get(),
                                     &g_clickSentinel);

    void* nativeTaskItem = g_clickSentinel_TaskItem;
    g_clickSentinel_TaskItem = nullptr;
    if (!nativeTaskItem) {
        g_resolveStats.failSentinelNoItem++;
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
        g_resolveStats.groupFailViewModelNull++;
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
        g_resolveStats.groupFailSentinelNoGroup++;
        return nullptr;
    }

    g_clickSentinel_TaskGroup = nullptr;
    TaskGroup_ReportClicked_Original(windowsUdkTaskGroup, &g_clickSentinel);
    void* nativeTaskGroup = g_clickSentinel_TaskGroup;
    g_clickSentinel_TaskGroup = nullptr;
    if (!nativeTaskGroup) {
        g_resolveStats.groupFailSentinelNoGroup++;
        return nullptr;
    }

    HDPA taskItemsArray = GetTaskItemsArray(nativeTaskGroup);
    if (!taskItemsArray || DPA_GetPtrCount(taskItemsArray) <= 0) {
        g_resolveStats.groupFailNoItems++;
        return nullptr;
    }

    // The group's first window stands in for the whole group's position -
    // Windows itself doesn't expose a more meaningful "primary" window.
    void* taskItem = DPA_GetPtr(taskItemsArray, 0);
    HWND hwnd = GetWindowFromNativeTaskItem(taskItem);
    if (hwnd) {
        g_resolveStats.groupSuccess++;
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
struct ButtonHwndCacheEntry {
    HWND hwnd = nullptr;
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
// GetButtonHwnd/an active Arrange pass - see the mod's own readme "Known
// limitations" section, which flagged this exact risk before it was ever
// hit in practice: the click-sentinel technique this chain uses
// interacts with the taskbar's own internal click-handling machinery, and
// running it synchronously while XAML is mid-layout - specifically while
// a button is being structurally inserted into or removed from an
// ItemsRepeater's data source, which happens whenever Windows' "show
// taskbar apps on" setting is anything other than "All taskbars" and a
// window moves across monitors - was confirmed (via WinDbg crash dump
// analysis across several explorer.exe crash-loop sessions) to be
// reachable in a way that fails fast with STATUS_STOWED_EXCEPTION. Only
// "All taskbars" mode was stable, because there a window moving between
// monitors never changes any taskbar's button set - no new/removed button
// ever needs fresh resolution inline during layout. Moving resolution to
// a timer restores the "normal" context this technique already assumed
// (a standalone event, not nested inside a live layout pass), matching
// where the pointer-release-based technique it's adapted from actually
// runs in the mods it's borrowed from.
bool ResolveAndCacheButtonHwnd(FrameworkElement element) {
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
    g_buttonHwndCache[key] = {hwnd, GetTickCount64(), failures};
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
// that aren't running and weren't listed in leftApps/rightApps).
ButtonClassification ClassifyTaskListButton(FrameworkElement element) {
    std::wstring name = GetButtonAccessibleName(element);

    if (ContainsAnyFragment(name, g_settings.leftApps)) {
        return {Side::Left, PinnedAppOrderKey()};
    }
    if (ContainsAnyFragment(name, g_settings.rightApps)) {
        return {Side::Right, PinnedAppOrderKey()};
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

double FullFootprintWidth(FrameworkElement element) {
    Thickness m = element.Margin();
    return m.Left + element.ActualWidth() + m.Right;
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

double ComputeStartButtonX(FrameworkElement startElement) {
    double centerX = GetMonitorCenterXLocal();
    return centerX - startElement.ActualWidth() / 2.0;
}

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
// task list buttons on that side don't overlap them. Updated whenever any
// of the three arranges; read by task list button placement.
double g_lastLeftSystemClusterWidth = 0;
double g_lastRightSystemClusterWidth = 0;

double SystemButtonClusterWidth(FrameworkElement repeater) {
    double total = 0;
    for (auto& child : GetRepeaterChildElements(repeater)) {
        if (SystemButtonRank(IdentifySystemButton(child)) >= 0) {
            total += FullFootprintWidth(child);
        }
    }
    return total;
}

// Places Search/TaskView/Widgets either at the taskbar's far left edge, or
// immediately adjacent to one side of the Start button, per
// g_settings.systemButtonsPlacement.
double ComputeSystemButtonX(FrameworkElement repeater,
                            FrameworkElement targetElement,
                            SystemButton target,
                            double startCenterX,
                            double startWidth) {
    int targetRank = SystemButtonRank(target);
    if (targetRank < 0) {
        return 0;
    }

    double widthBefore = 0;
    for (auto& child : GetRepeaterChildElements(repeater)) {
        int r = SystemButtonRank(IdentifySystemButton(child));
        if (r >= 0 && r < targetRank) {
            widthBefore += FullFootprintWidth(child);
        }
    }

    if (g_settings.systemButtonsPlacement == SystemButtonsPlacement::FarLeft) {
        g_lastLeftSystemClusterWidth = 0;
        g_lastRightSystemClusterWidth = 0;
        return 8 + widthBefore;  // small left margin
    }

    double clusterWidth = SystemButtonClusterWidth(repeater);
    double gap = g_settings.gapPx;
    double ownWidth = FullFootprintWidth(targetElement);

    if (g_settings.systemButtonsAdjacentSide == Side::Left) {
        g_lastLeftSystemClusterWidth = clusterWidth;
        g_lastRightSystemClusterWidth = 0;
        // Stack right-to-left outward from Start: lowest rank closest.
        double widthAfter = clusterWidth - widthBefore - ownWidth;
        return startCenterX - startWidth / 2.0 - gap - widthAfter - ownWidth;
    }

    g_lastLeftSystemClusterWidth = 0;
    g_lastRightSystemClusterWidth = clusterWidth;
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
// exactly once. The previous version (ComputeTaskListButtonX, called once
// per button from RecomputeLayoutPlan's loop) re-walked and re-classified
// every sibling for every single button it placed - an O(n^2) pass over
// the whole task list on every ArrangeOverride pass (~1600 classifications
// at 40 icons), on the shell's layout critical path during a drag. It also
// re-derived each target's sibling list via VisualTreeHelper::GetParent
// instead of reusing `children`, so it re-walked the repeater itself once
// per button too. This also closes a subtle consistency gap the O(n^2)
// version had: a button classified while being visited as someone else's
// *sibling* could in principle disagree with its own classification when
// later visited as the *target* (e.g. its HWND resolving mid-pass), which
// could produce two buttons at the same X or a gap - classifying once up
// front removes that possibility entirely.
void PlanTaskListButtons(const std::vector<FrameworkElement>& children,
                         double startCenterX,
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
    double leftExtra = (adjacent && g_settings.systemButtonsAdjacentSide == Side::Left)
                            ? (g_lastLeftSystemClusterWidth + gap)
                            : 0;
    double rightExtra = (adjacent && g_settings.systemButtonsAdjacentSide == Side::Right)
                             ? (g_lastRightSystemClusterWidth + gap)
                             : 0;

    if (g_settings.taskListOrder == TaskListOrder::DistanceFromCenter) {
        // Innermost (closest to Start) first on each side. Ties - e.g.
        // multiple pinned/overridden buttons, which all share the same
        // +/-infinity orderKey - now break on taskbar index instead of
        // the previous ABI-pointer value: equally stable frame to frame,
        // but a predictable order instead of an arbitrary one.
        std::vector<TaskListPlanEntry*> left, right;
        for (auto& entry : entries) {
            (entry.info.side == Side::Left ? left : right).push_back(&entry);
        }
        auto byOrderKey = [](const TaskListPlanEntry* a,
                             const TaskListPlanEntry* b) {
            if (a->info.orderKey != b->info.orderKey) {
                return a->info.orderKey < b->info.orderKey;
            }
            return a->index < b->index;
        };
        std::sort(left.begin(), left.end(), byOrderKey);
        std::sort(right.begin(), right.end(), byOrderKey);

        double widthBefore = 0;
        for (auto* entry : left) {
            outPlan[winrt::get_abi(entry->element)] =
                startCenterX - startWidth / 2.0 - gap - leftExtra -
                widthBefore - entry->width;
            widthBefore += entry->width;
        }
        widthBefore = 0;
        for (auto* entry : right) {
            outPlan[winrt::get_abi(entry->element)] = startCenterX +
                startWidth / 2.0 + gap + rightExtra + widthBefore;
            widthBefore += entry->width;
        }
    } else {
        // Preserve taskbar order: walk `entries` in original (taskbar)
        // order, accumulating each side's width independently. Unchanged
        // from before this rewrite: the left side ends up mirrored rather
        // than laid out outward-from-Start - a pre-existing ordering
        // quirk (tracked separately in the review) that this efficiency
        // rewrite deliberately preserves rather than also fixing.
        double leftWidthBefore = 0, rightWidthBefore = 0;
        for (auto& entry : entries) {
            if (entry.info.side == Side::Left) {
                outPlan[winrt::get_abi(entry.element)] =
                    startCenterX - startWidth / 2.0 - gap - leftExtra -
                    leftWidthBefore - entry.width;
                leftWidthBefore += entry.width;
            } else {
                outPlan[winrt::get_abi(entry.element)] = startCenterX +
                    startWidth / 2.0 + gap + rightExtra + rightWidthBefore;
                rightWidthBefore += entry.width;
            }
        }
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
    if (g_hTaskbarWnd) {
        return g_hTaskbarWnd;
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

        auto it = g_buttonHwndCache.find(key);
        bool needsResolve = it == g_buttonHwndCache.end();
        if (!needsResolve) {
            if (it->second.hwnd) {
                needsResolve = !IsWindow(it->second.hwnd);
            } else {
                int shift = std::min(it->second.consecutiveFailures, 4);
                ULONGLONG backoffMs = 2000ULL << shift;  // 2s..32s
                needsResolve = now - it->second.lastAttempt >= backoffMs;
            }
        }

        if (needsResolve && ResolveAndCacheButtonHwnd(child)) {
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
// scratch on every ArrangeOverride pass (see its own comment) - so
// IUIElement_Arrange_Hook below never needs to compute anything itself,
// only look a value up. An element with no entry (a secondary-monitor
// element, which RecomputeLayoutPlan never walks; or a primary element so
// new it wasn't realized yet when this pass's plan was built) falls
// through to Windows' own native positioning for that one pass, same as
// any element this mod doesn't touch at all.
std::unordered_map<void*, double> g_lastArrangedX;

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
// tree, process-wide - with a second monitor enabled that's the primary
// taskbar and secondary-monitor taskbars potentially interleaving on
// different threads. A single shared instance here was previously a data
// race (unsynchronized concurrent reset/read of a non-atomic struct).
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
// (and so IUIElement_Arrange_Hook above) run at all for this pass. That
// ordering is the actual fix for the crash class the old settling-window
// mechanism used to paper over - see g_inTaskbarArrangeOverride's comment.
//
// Rebuilds g_lastArrangedX from scratch every call rather than updating
// incrementally, so a destroyed element's stale entry can never outlive
// it - no separate pruning pass needed for this map, unlike
// g_buttonHwndCache/g_lastKnownWindowClassification (populated
// independently by the HWND-resolve timer - see ResolvePendingButtonHwnds
// for why those still need explicit pruning).
//
// MUST only run when confirmed to be on g_hTaskbarWnd's own thread:
// GetTaskbarXamlRoot(g_hTaskbarWnd) reaches across to the PRIMARY's XAML
// object specifically, which is an unsafe unmarshaled cross-apartment
// WinRT call from any other thread.
//
// Note this check does NOT scope the (relatively expensive) rebuild to
// primary-monitor passes only, despite an earlier version of this comment
// claiming that - Shell_TrayWnd and Shell_SecondaryTrayWnd both run on
// the SAME Explorer UI thread (the established pattern other taskbar mods
// use to reach every monitor's taskbar is a single EnumThreadWindows on
// that one thread), so this check passes for secondary-monitor
// ArrangeOverride passes too, and the whole plan gets rebuilt redundantly
// for each of them. It's still *correct* either way - secondary-monitor
// elements never end up in g_lastArrangedX regardless of how many times
// this runs, since it only ever walks the primary's own repeater - just
// not free. This check exists purely as the apartment-safety guard
// described above, not as a monitor-scoping mechanism. A real
// scoping/caching optimization (skip the rebuild when nothing relevant
// changed) is a separate, larger change left for its own review round
// given how much this exact function's correctness properties have
// already been fought for across several crash-debugging sessions.
void RecomputeLayoutPlan() {
    if (!g_hTaskbarWnd) {
        return;
    }
    DWORD primaryThreadId = GetWindowThreadProcessId(g_hTaskbarWnd, nullptr);
    if (primaryThreadId == 0 || primaryThreadId != GetCurrentThreadId()) {
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

        // Start first: ComputeSystemButtonX/PlanTaskListButtons below
        // both read g_lastStartWidth, so it needs to already reflect this
        // pass by the time they run, not the previous one. Inlined rather
        // than calling ComputeStartButtonX (same formula) purely to reuse
        // the startCenterX already computed once above, instead of that
        // function re-deriving it via another GetMonitorCenterXLocal()
        // call.
        for (auto& child : children) {
            if (IdentifySystemButton(child) == SystemButton::Start) {
                g_lastStartWidth = child.ActualWidth();
                newPlan[winrt::get_abi(child)] =
                    startCenterX - g_lastStartWidth / 2.0;
                break;
            }
        }

        // Search/TaskView/Widgets next: PlanTaskListButtons reads
        // g_lastLeftSystemClusterWidth/g_lastRightSystemClusterWidth
        // (updated inside ComputeSystemButtonX), so these need to run
        // before any task list button below for the same reason.
        for (auto& child : children) {
            SystemButton sb = IdentifySystemButton(child);
            if (sb == SystemButton::None || sb == SystemButton::Start) {
                continue;
            }
            newPlan[winrt::get_abi(child)] = ComputeSystemButtonX(
                repeater, child, sb, startCenterX, g_lastStartWidth);
        }

        // Task list buttons last - see PlanTaskListButtons' own comment
        // for why this is a single O(n) pass over `children` rather than
        // calling a per-button compute function in a loop here.
        PlanTaskListButtons(children, startCenterX, newPlan);

        g_lastArrangedX = std::move(newPlan);
    } catch (...) {
        g_planStats.exceptions++;
        // g_lastArrangedX is left as whatever the last successful pass
        // produced - IUIElement_Arrange_Hook's lookup-or-fall-through
        // handles a stale/incomplete plan exactly like it already handles
        // a brand-new not-yet-planned element.
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
    // for RecomputeLayoutPlan to pick it up on the next pass. Unlike the
    // old version of this same self-correction, there's nothing left to
    // suppress or re-arm here - RecomputeLayoutPlan is safe to call as
    // often as this triggers it - so this is a plain count comparison
    // with no gating logic alongside it.
    //
    // thread_local: this hook runs process-wide for every taskbar
    // instance's XAML tree (primary and, with a second monitor enabled,
    // secondary-monitor taskbars too), which can run on different threads
    // with independently-changing button counts - a shared global here
    // previously raced across instances.
    static thread_local int lastPlanTaskListCount = -1;
    int currentTaskListCount = g_planStats.taskListTotal;
    if (currentTaskListCount != lastPlanTaskListCount) {
        bool countChanged = lastPlanTaskListCount != -1;
        lastPlanTaskListCount = currentTaskListCount;
        if (countChanged) {
            InvalidateTaskbarLayout();
        }
    }

    g_inTaskbarArrangeOverride = true;

    HRESULT ret = TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Original(
        pThis, context, size, resultSize);

    g_inTaskbarArrangeOverride = false;

    static ULONGLONG lastStatsLog;
    ULONGLONG now = GetTickCount64();
    if (now - lastStatsLog > 2000) {
        lastStatsLog = now;
        Wh_Log(
            L"Arrange pass: arrangeCalls=%d repositioned=%d qiFail=%d "
            L"exceptions=%d | plan: taskList=%d (hwndResolved=%d left=%d "
            L"right=%d) planExceptions=%d | winEvents: raw=%d "
            L"invalidated=%d skippedReentrant=%d invalidateExceptions=%d | "
            L"resolve(individual): ok=%d viewModelNull=%d getTaskItemFail=%d "
            L"sentinelNoItem=%d | resolve(group): ok=%d viewModelNull=%d "
            L"sentinelNoGroup=%d noItems=%d",
            g_passStats.totalArrangeCalls, g_passStats.repositioned,
            g_passStats.qiFailures, g_passStats.exceptions,
            g_planStats.taskListTotal, g_planStats.taskListHwndResolved,
            g_planStats.taskListLeft, g_planStats.taskListRight,
            g_planStats.exceptions,
            (int)g_winEventRawCount,
            (int)g_winEventInvalidateCount, (int)g_invalidateSkippedReentrant,
            (int)g_invalidateExceptions, g_resolveStats.success,
            g_resolveStats.failViewModelNull, g_resolveStats.failGetTaskItem,
            g_resolveStats.failSentinelNoItem, g_resolveStats.groupSuccess,
            g_resolveStats.groupFailViewModelNull,
            g_resolveStats.groupFailSentinelNoGroup,
            g_resolveStats.groupFailNoItems);
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
// UpdateLayout() call. An earlier version forced it (for drag-follow
// snappiness), gated behind a reentrancy guard and a forceImmediate flag
// that was false only for one specific call site. That was not enough:
// this function's callers include a raw OS callback (WinEventProc) that
// can itself fire while the thread is already nested inside XAML-internal
// layout activity for reasons outside this mod's control (e.g. a taskbar
// button structurally moving between two different monitors' XAML trees,
// not just changing coordinates within one). A forced UpdateLayout() in
// that state is a reentrant "layout cycle" from WinUI's perspective, and
// it fails fast with STATUS_STOWED_EXCEPTION - the confirmed cause of
// three separate explorer.exe crash-loop incidents (identical fault offset
// in Windows.UI.Xaml.dll every time), each via a different trigger path,
// even after two of those paths were individually guarded. Since new
// trigger paths kept appearing, the only fix that actually closes the
// whole class is to never force it anywhere: InvalidateArrange() +
// InvalidateMeasure() alone are always safe to call from any context, and
// the resulting delay before the dispatcher's own pass runs is at most
// one composition frame - not perceptible for a drag-follow feature that
// was already documented as best-effort ("shortly after a drag/move
// settles, not on every intermediate pixel").
//
// Also important: that STATUS_STOWED_EXCEPTION is a raw SEH RaiseException,
// not a thrown C++ exception. This mod's toolchain (Windhawk's clang/MinGW
// build, no /EHa-style async exception handling) cannot catch it with
// `catch(...)` no matter where it's placed - confirmed by
// g_invalidateExceptions staying 0 across multiple full crash-loop
// sessions. The try/catch below only covers genuine C++ exceptions from
// the WinRT calls in this lambda (e.g. QueryInterface failures surfaced as
// hresult_error) - it was never a safety net for the layout-cycle failure,
// and no try/catch placement ever will be. Don't reintroduce a forced
// UpdateLayout() here without a fundamentally different mechanism (e.g. a
// genuinely async PostMessage-deferred call that's guaranteed to run only
// once the current call stack has fully unwound).
void InvalidateTaskbarLayout() {
    if (!g_hTaskbarWnd) {
        return;
    }

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

    static ULONGLONG lastInvalidate;
    ULONGLONG now = GetTickCount64();
    if (now - lastInvalidate < 150) {
        return;
    }
    lastInvalidate = now;

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
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarCollapsibleLayout,struct winrt::Microsoft::UI::Xaml::Controls::IVirtualizingLayoutOverrides>::ArrangeOverride(void *,struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))"},
            &TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Original,
            TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Hook,
            true,
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

// WINEVENT_OUTOFCONTEXT delivers its callback on whichever thread called
// SetWinEventHook, and only if that thread pumps messages. Wh_ModAfterInit
// doesn't reliably run on the taskbar's own message-pumping UI thread (the
// raw=0 counter in the logs confirms the callback never fired at all), so
// registration is marshaled onto g_hTaskbarWnd's thread via
// RunFromWindowThread, same technique already used for XAML tree access.
void StartWinEventHook() {
    if (g_locationChangeHook || !g_hTaskbarWnd) {
        return;
    }

    RunFromWindowThread(g_hTaskbarWnd, [] {
        if (g_locationChangeHook) {
            return;
        }

        // Deliberately NOT using WINEVENT_SKIPOWNPROCESS: File Explorer
        // windows often run inside explorer.exe's own process, and that
        // flag would silently drop their location-change events too. The
        // taskbar's own windows are already excluded explicitly in
        // WinEventProc.
        g_locationChangeHook = SetWinEventHook(
            EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE, nullptr,
            WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
        Wh_Log(L"StartWinEventHook: handle=%p (registered on taskbar thread "
               L"%lu)",
               g_locationChangeHook, GetCurrentThreadId());
        if (!g_locationChangeHook) {
            Wh_Log(L"Failed to register location-change hook - live "
                   L"drag-follow will not work, but everything else still "
                   L"will");
        }
    });
}

// RunFromWindowThread can only fail two ways: the taskbar's thread is
// already gone (GetWindowThreadProcessId returns 0 - in which case the
// OS already tore down any timer/hook that thread owned, so there's
// nothing left to clean up), or its own internal SetWindowsHookEx call
// failed (rare, e.g. transient resource exhaustion) while the thread is
// still very much alive. That second case is the dangerous one: nothing
// under mod control runs on the taskbar thread to unregister the real
// WinEventHook/timer, Wh_ModUninit returns anyway, Windhawk unmaps this
// module's code, and the still-alive thread's next tick calls into
// unmapped memory. A few retries gives that transient failure a chance to
// clear without risking an unbounded stall if the thread really is gone.
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

void StopWinEventHook() {
    if (!g_locationChangeHook) {
        return;
    }

    HWINEVENTHOOK hook = g_locationChangeHook;
    g_locationChangeHook = nullptr;

    if (g_hTaskbarWnd) {
        // See RunFromWindowThreadWithRetry's comment for why a bounded
        // retry, rather than falling back to an inline UnhookWinEvent
        // here - that's documented as unsafe from the wrong thread, so
        // it would just trade one crash class for another.
        if (!RunFromWindowThreadWithRetry(g_hTaskbarWnd,
                                          [hook] { UnhookWinEvent(hook); })) {
            Wh_Log(L"StopWinEventHook: RunFromWindowThread failed, "
                   L"location-change hook left registered");
        }
    } else {
        UnhookWinEvent(hook);
    }
}

// Not 1: this is a window (Shell_TrayWnd) the mod doesn't own, shared with
// Explorer's own timers and any other mod that also sets timers on it -
// SetTimer with an already-used ID silently replaces that timer, so a
// small/common value like 1 is a real collision risk. Arbitrary otherwise,
// same pattern other mods use for shell-window timers (e.g.
// classic-taskbar-properties.wh.cpp's kTimerIdMasterLayout).
constexpr UINT_PTR kButtonHwndResolveTimerId = 0x8C3F;

void CALLBACK ButtonHwndResolveTimerProc(HWND hwnd,
                                         UINT uMsg,
                                         UINT_PTR idEvent,
                                         DWORD dwTime) {
    ResolvePendingButtonHwnds();
}

// SetTimer's callback fires on whichever thread registered it, so this is
// marshaled onto g_hTaskbarWnd's own thread just like StartWinEventHook -
// both KillTimer and (for reliability) SetTimer itself are documented to
// need to run on the thread that owns the timer.
void StartButtonHwndResolveTimer() {
    if (!g_hTaskbarWnd) {
        return;
    }

    if (!RunFromWindowThread(g_hTaskbarWnd, [] {
            SetTimer(g_hTaskbarWnd, kButtonHwndResolveTimerId, 500,
                     ButtonHwndResolveTimerProc);
        })) {
        Wh_Log(L"StartButtonHwndResolveTimer: RunFromWindowThread failed, "
               L"HWND resolution will not run");
    }
}

void StopButtonHwndResolveTimer() {
    if (!g_hTaskbarWnd) {
        return;
    }

    // See StopWinEventHook's comment - same reasoning against an inline
    // fallback call applies to KillTimer from the wrong thread.
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

    StopWinEventHook();
    StopButtonHwndResolveTimer();

    InvalidateTaskbarLayout();
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    // LoadSettings() reassigns g_settings.leftApps/rightApps
    // (std::vector<std::wstring>) - marshaled onto the taskbar's own
    // thread so a concurrent ContainsAnyFragment call during a layout
    // pass on that thread can't read a vector mid-reassignment.
    if (g_hTaskbarWnd) {
        RunFromWindowThread(g_hTaskbarWnd, [] { LoadSettings(); });
    } else {
        LoadSettings();
    }

    InvalidateTaskbarLayout();
}
