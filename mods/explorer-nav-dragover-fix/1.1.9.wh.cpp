// ==WindhawkMod==
// @id           explorer-nav-dragover-fix
// @name         Explorer Nav DragOver Fix
// @description  Stops File Explorer's nav pane from jumping when a folder auto-expands during a drag, and restores mouse-wheel scrolling to reach out-of-view drop targets
// @version      1.1.9
// @author       tonythethompson
// @github       https://github.com/tonythethompson
// @include      explorer.exe
// @architecture x86-64
// @compilerOptions -lole32 -lcomctl32 -luiautomationcore
// @license      MIT
// @donateUrl    https://ko-fi.com/tonythethompson
// ==/WindhawkMod==

// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues

// ==WindhawkModReadme==
/*
# Explorer Nav DragOver Fix

Two fixes for File Explorer's drag-over behavior:

1. **Stops the jump.** When you drag a file or folder onto a root-level entry
   in the nav pane (drives, pinned folders, the "This PC" grouping), Explorer
   auto-expands that entry after a short hover delay -- and the auto-expand
   often scrolls the pane, sending the row you were aiming at off-screen.
   This mod pins the TreeView's first-visible item to its pre-expand position
   and keeps the drop highlight on the original parent row. Protection
   releases as soon as the cursor leaves the captured row, so deeper folders
   continue to behave normally.

2. **Re-enables the mouse wheel.** Windows normally swallows wheel input
   during drag-and-drop. This mod intercepts the wheel and forwards it to
   the Explorer navigation pane (left tree) and the main file list on the
   right (Details / ItemsView on Win11 25H2, and classic Large icons / List
   views via SysListView32). Scrolling prefers UI Automation so wheel messages
   are not sent to SHELLDLL_DefView during a drag. Disable the corresponding
   setting to fall back to native behavior.

## Demo

![Without Mod vs With Mod](https://raw.githubusercontent.com/tonythethompson/windhawk-mods/main/mods_assets/explorer-nav-dragover-fix-demo.gif)

Left: default Explorer behavior. Right: with this mod enabled (stable hover
expand and mouse-wheel scrolling during drag).

The mod hooks `DoDragDrop` (to track drag generations across threads),
subclasses each nav-pane `SysTreeView32`, and watches `EVENT_OBJECT_CREATE`
on a dedicated helper thread to pick up new Explorer windows. No state is
persisted between drags; everything is scoped to the active drag loop.

## Troubleshooting

If you disable the mod and see a log line like **"WinEvent helper thread did
not exit within 10s"**, an Explorer thread was wedged mid-callback (for
example while subclassing a nav tree). The mod intentionally leaks that
helper handle rather than calling `TerminateThread`, which would be unsafe.
**Restart File Explorer** (or sign out) to clear the state. This is rare in
normal use.

## Compatibility

- File Explorer (`explorer.exe`) on Windows 11 (tested on 23H2 / 24H2).
- Architectures: x64, arm64.
- No effect on any other process.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- protectedMaxDepth: 3
  $name: Protected depth
  $description: >-
    How many top levels of the navigation pane are protected from scrolling
    while you drag items over them. Default 3 covers the four topmost
    depths: the hidden root (depth 0), the visible groupings like Home /
    This PC / Network (depth 1), their direct children like drives, pinned
    folders, OneDrive top entries (depth 2), and the 3rd-level groupings
    like Documents / Pictures / Downloads (depth 3). Items deeper than this
    still auto-expand on hover but without the anti-scroll anchoring. Lower
    the value to protect fewer levels; raise it to extend protection deeper.
    (0-8)
- releaseMoveY: 6
  $name: Cursor release threshold (pixels)
  $description: >-
    Vertical cursor travel from the protection start point that releases
    the anchor. Lower = quicker release; higher = stickier protection. (0-64)
- capturedRowMarginX: 8
  $name: Captured row horizontal margin (pixels)
  $description: >-
    Horizontal slack on the captured row's hit rect. Larger values tolerate
    sideways cursor wobble before the row is considered exited and
    protection releases. (0-64)
- capturedRowMarginY: 3
  $name: Captured row vertical margin (pixels)
  $description: >-
    Vertical slack on the captured row's hit rect, independent of
    releaseMoveY. Larger values tolerate vertical wobble; smaller values
    release sooner once the cursor leaves the row. (0-64)
- enableWheelScrollDuringDrag: true
  $name: Enable scroll-wheel during drag
  $description: >-
    Allow the mouse wheel to scroll Explorer's left navigation pane and
    right-hand file list while you drag a file or folder. Windows normally
    blocks the wheel during drag-and-drop; this re-enables it so you can
    scroll to reach drop targets that aren't in view without releasing the
    drag. On Win11 25H2 Details view the file list uses DirectUI ItemsView,
    not SysListView32.
    Disable to fall back to native (no-scroll) behavior.
*/
// ==/WindhawkModSettings==

// This mod targets a specific File Explorer drag/drop annoyance:
//
// 1. Explorer is allowed to auto-expand a navigation-pane folder while files
//    are being dragged over it.
// 2. If the hovered item is root-ish/top-level, the mod prevents the TreeView
//    from making that item or one of its new children the first visible item.
// 3. While the mouse has not moved out of the original parent row, the
//    TreeView drop highlight is kept on the parent item instead of a newly
//    exposed child.
//
// Implementation notes:
// - We subclass each File Explorer SysTreeView32 nav pane via
//   WindhawkUtils::SetWindowSubclassFromAnyThread, which marshals the install
//   to the window's owning thread.
// - New trees are discovered by a SetWinEventHook(EVENT_OBJECT_CREATE)
//   running on a dedicated helper thread with its own message pump (the
//   thread that calls SetWinEventHook must pump for WINEVENT_OUTOFCONTEXT
//   delivery).
// - Helper functions that call back into the subclassed tree set a
//   thread-local re-entry depth so the subclass proc passes them straight
//   through to DefSubclassProc.
// - DoDragDrop_Hook tracks drag loop depth with an atomic counter so target
//   trees on different threads from the dragging caller still engage the
//   protection. Each transition from 0 -> 1 bumps a global drag-generation
//   counter; per-thread hover state records the generation at capture time
//   so stale captures from a prior drag don't survive into a new one.

#include <windhawk_utils.h>

#include <windows.h>
#include <commctrl.h>
#include <ole2.h>
#include <initguid.h>  // must come before uiautomation.h
#include <uiautomation.h>
#include <wchar.h>

#include <atomic>
#include <cstdint>
#include <cstring>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

// Each field is std::atomic so concurrent reads from subclass procs (running
// on tree-owning threads), the WH_MOUSE hook (drag-owning threads), and any
// other consumer can observe the settings-thread's writes in LoadSettings
// without a C++-level data race. The settings are independent of each other
// and of every other global, so memory_order_relaxed is sufficient at every
// read and store site -- we just need atomicity per field, not cross-field
// ordering.
struct Settings {
    std::atomic<int> protectedMaxDepth{2};
    std::atomic<int> releaseMoveY{6};
    std::atomic<int> capturedRowMarginX{8};
    std::atomic<int> capturedRowMarginY{3};
    std::atomic<bool> enableWheelScrollDuringDrag{true};
};

Settings g_settings;

// DoDragDrop produces one nested modal loop per thread, but the dragging
// thread can differ from the thread that owns the target tree. Keep this
// global and atomic so any thread's subclass proc can observe it.
std::atomic<int> g_dragLoopDepth{0};

// Bumped on every fresh drag (0 -> 1 transition in g_dragLoopDepth). Each
// captured hover stores the generation at capture time; once the generation
// changes, the per-thread thread_local capture is treated as stale.
std::atomic<uint64_t> g_dragGeneration{0};

// Per-tree-thread hover capture. Each tree only ever runs on one thread, and
// each thread can have at most one captured row at a time.
thread_local struct ProtectedHoverState {
    bool active = false;
    HWND hwndTree = nullptr;
    HTREEITEM hItem = nullptr;
    POINT capturedCursorScreen = {};
    POINT lastSeenCursorScreen = {};
    RECT rowRectScreen = {};
    uint64_t generation = 0;
} g_protectedHover;

// While protection is active we suppress Explorer-initiated jumps
// (TVM_EXPAND restoration, TVM_SELECTITEM/TVGN_FIRSTVISIBLE,
// TVM_ENSUREVISIBLE for the captured row). Some Explorer builds also fire a
// follow-up line-based WM_VSCROLL as the scroll tail of those events, and we
// want to keep swallowing those. But edge-scroll-while-dragging (cursor pinned
// to the top/bottom of the window) produces an indistinguishable line-based
// WM_VSCROLL -- and that one is legitimate user input. The discriminator is
// time: stamp on every suppression event, then only treat WM_VSCROLL as part
// of a jump tail if it arrives within the window. Outside the window it must
// be user-driven edge-scroll, so let it through.
constexpr uint64_t kPostJumpScrollSuppressionMs = 200;
constexpr DWORD kWheelForwardTimeoutMs = 200;
constexpr DWORD kWinEventThreadReadyTimeoutMs = 5000;
constexpr DWORD kWinEventHelperJoinTimeoutMs = 10000;
// Cap UIA Scroll() calls per intercepted wheel message (fast/high-res wheels).
constexpr int kMaxUiScrollCallsPerWheelMessage = 24;

#ifndef WHEEL_PAGESCROLL
#define WHEEL_PAGESCROLL static_cast<UINT>(-1)
#endif
thread_local uint64_t g_lastJumpAttemptTickMs = 0;

// Set while one of our helpers calls back into the subclassed tree, so the
// subclass proc skips its filtering (and avoids recursion).
thread_local int g_internalCallDepth = 0;

// Per-thread WH_MOUSE hook installed for the duration of DoDragDrop on this
// thread. Used to rescue WM_MOUSEWHEEL events that OLE's drag loop would
// otherwise silently consume, so the nav pane can be wheel-scrolled while a
// drag is in flight. Nested drags on the same thread reuse this single hook.
thread_local HHOOK g_dragMouseHook = nullptr;

// Global registry of every WH_MOUSE hook handle currently installed by
// DoDragDrop_Hook (across all dragging threads). Required because the
// per-thread g_dragMouseHook is not reachable from Wh_ModUninit, which runs
// on the unload thread; without this registry, disabling the mod mid-drag
// would leave the system holding a hook handle into our soon-to-be-unmapped
// DragMouseHookProc. Inserted by the same thread that installs the hook,
// erased on the way out of DoDragDrop_Hook, and swept at the top of
// Wh_ModUninit. g_dragHookMutex serializes the install/uninstall path
// against the uninit sweep.
std::unordered_set<HHOOK> g_activeDragHooks;
std::mutex g_dragHookMutex;

// Tracked subclassed HWNDs for cleanup on uninit. Guarded by g_subclassMutex.
//
// Two-state map (not a flat set) because TrySubclassTree must release the
// mutex while WindhawkUtils::SetWindowSubclassFromAnyThread marshals across
// threads -- two concurrent discovery callbacks for the same hwnd would
// otherwise both pass a "not present" check and both install a subclass.
// With the state map, the first caller inserts {hwnd, Installing} and
// proceeds; any concurrent caller sees the existing entry and bails. On
// install success the state flips to Installed; on failure the entry is
// erased so a future retry can proceed cleanly.
enum class SubclassState { Installing, Installed };
std::unordered_map<HWND, SubclassState> g_subclassedTrees;
std::mutex g_subclassMutex;

// Count of TrySubclassTree calls that have passed the shutdown gate and are
// still mid-install (i.e. the marshalled SetWindowSubclassFromAnyThread has
// not returned yet, so the {hwnd, Installing} entry may still be visible to
// a concurrent uninit snapshot). Wh_ModUninit briefly waits for this to drain
// to zero before snapshotting g_subclassedTrees, so a Remove dispatched by
// uninit cannot land on the target thread before the corresponding Install
// (out-of-order between two SendMessage senders to one receiver), which would
// otherwise leave a live subclass pointer in an about-to-be-unmapped image.
// Bounded wait -- we'd rather risk the same microsecond race than block uninit
// indefinitely if an Explorer thread is wedged.
std::atomic<int> g_subclassOpsInFlight{0};

// During unload we ask each tree's subclass proc to remove itself.
std::atomic<bool> g_uninitInProgress{false};

// Helper thread that hosts the WinEvent hook with its own message pump.
// g_winEventThreadReady is signalled once the thread has provoked queue
// creation and finished its SetWinEventHook call; this closes the start-up
// race in which Wh_ModUninit could otherwise signal shutdown before the
// helper had even installed its hook.
//
// g_winEventHook is owned EXCLUSIVELY by the helper thread. The helper
// installs it under SetWinEventHook on its own thread, runs its message loop
// via MsgWaitForMultipleObjectsEx, and calls UnhookWinEvent on the same
// thread when the stop event fires. Cross-thread UnhookWinEvent is not
// supported by Win32 (it must be called on the thread that installed the
// hook), and touching the variable from anywhere else would also be a data
// race.
//
// g_helperStopEvent is the deterministic shutdown signal. Wh_ModUninit calls
// SetEvent on it; the helper's MsgWaitForMultipleObjectsEx returns
// WAIT_OBJECT_0 immediately (taking priority over any queued WinEvent
// callbacks), the helper unhooks, and returns. SetEvent never fails the way
// PostThreadMessageW can if the target thread has not built its queue yet,
// which eliminates the "could not post stop" fatal-leak path entirely.
HANDLE g_winEventThread = nullptr;
HANDLE g_winEventThreadReady = nullptr;
HANDLE g_helperStopEvent = nullptr;
HWINEVENTHOOK g_winEventHook = nullptr;

// Set in Wh_ModUninit BEFORE SetEvent(g_helperStopEvent). WinEventProc
// consults this flag and returns immediately when it is set, which prevents
// any in-flight callbacks from installing subclasses during shutdown.
// Monotonic (set once, never cleared) and only ever transitions false ->
// true, so readers cannot observe a torn or oscillating value. The flag and
// the event are complementary: the flag stops the per-callback work, the
// event stops the message loop.
std::atomic<bool> g_helperStopping{false};

using DoDragDrop_t = HRESULT(WINAPI*)(IDataObject*, IDropSource*, DWORD, DWORD*);
DoDragDrop_t DoDragDrop_Original = nullptr;

void ClearProtectedHover() {
    g_protectedHover = {};
}

bool IsDragLoopActive() {
    return g_dragLoopDepth.load(std::memory_order_relaxed) > 0;
}

void NoteJumpAttempt() {
    g_lastJumpAttemptTickMs = GetTickCount64();
}

bool InPostJumpScrollSuppressionWindow() {
    if (g_lastJumpAttemptTickMs == 0) {
        return false;
    }
    return (GetTickCount64() - g_lastJumpAttemptTickMs) <
           kPostJumpScrollSuppressionMs;
}

// RAII guard for g_internalCallDepth. Used by SendInternal so the depth
// counter can never get stuck at >0 even if a future edit adds an early
// return on a Windows API failure path or a C++ exception ever crosses the
// SendMessageW boundary. Neither happens today, but the cost is zero and
// the failure mode of a stuck counter is silent and very bad: every
// subsequent message would be treated as an internal re-entry, the subclass
// proc would stop filtering, and drag-time protection would silently
// disappear for the rest of the session.
struct InternalCallGuard {
    InternalCallGuard() { g_internalCallDepth++; }
    ~InternalCallGuard() { g_internalCallDepth--; } 
};

LRESULT SendInternal(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    InternalCallGuard guard;
    return SendMessageW(hwnd, msg, wParam, lParam);
}

bool WindowClassEquals(HWND hwnd, const wchar_t* expectedClass) {
    wchar_t className[64] = {};
    if (!GetClassNameW(hwnd, className, ARRAYSIZE(className))) {
        return false;
    }
    return wcscmp(className, expectedClass) == 0;
}

bool IsExplorerCabinetRoot(HWND hwnd) {
    return WindowClassEquals(hwnd, L"CabinetWClass") ||
           WindowClassEquals(hwnd, L"ExploreWClass");
}

bool IsExplorerNavigationTree(HWND hwnd) {
    if (!IsWindow(hwnd) || !WindowClassEquals(hwnd, L"SysTreeView32")) {
        return false;
    }
    HWND root = GetAncestor(hwnd, GA_ROOT);
    if (!root) {
        return false;
    }
    // CabinetWClass is the normal File Explorer frame; ExploreWClass is kept
    // for older/classic Explorer windows. Win11's tabbed explorer still uses
    // CabinetWClass at the root; revisit if a future build wraps the tree in
    // a different shell.
    return IsExplorerCabinetRoot(root);
}

bool IsExplorerFileContentHost(HWND hwnd) {
    // Win11 Details view (including 25H2) uses DirectUI ItemsView hosts
    // (UIItemsView / ItemsView / DirectUIHWND) instead of SysListView32.
    // Classic layouts and some view modes still expose SysListView32.
    if (!IsWindow(hwnd)) {
        return false;
    }
    if (!WindowClassEquals(hwnd, L"SysListView32") &&
        !WindowClassEquals(hwnd, L"UIItemsView") &&
        !WindowClassEquals(hwnd, L"ItemsView") &&
        !WindowClassEquals(hwnd, L"DirectUIHWND")) {
        return false;
    }

    HWND root = GetAncestor(hwnd, GA_ROOT);
    if (!root) {
        return false;
    }

    return IsExplorerCabinetRoot(root);
}

bool PointInWindow(HWND hwnd, POINT ptScreen) {
    RECT rc = {};
    return IsWindow(hwnd) && GetWindowRect(hwnd, &rc) && PtInRect(&rc, ptScreen);
}

struct FindDescendantClassContext {
    const wchar_t* className = nullptr;
    HWND found = nullptr;
};

BOOL CALLBACK FindDescendantClassEnumProc(HWND hwnd, LPARAM lParam) {
    auto* ctx = reinterpret_cast<FindDescendantClassContext*>(lParam);
    if (WindowClassEquals(hwnd, ctx->className)) {
        ctx->found = hwnd;
        return FALSE;
    }
    return TRUE;
}

HWND FindDescendantClass(HWND root, const wchar_t* className) {
    if (!IsWindow(root)) {
        return nullptr;
    }
    FindDescendantClassContext ctx{className};
    EnumChildWindows(root, FindDescendantClassEnumProc,
                     reinterpret_cast<LPARAM>(&ctx));
    return ctx.found;
}

HWND FindShellDefViewInCabinet(HWND cabinet) {
    // Win11's tabbed UI nests SHELLDLL_DefView under DirectUI hosts; a
    // recursive search covers both classic and modern Explorer layouts.
    return FindDescendantClass(cabinet, L"SHELLDLL_DefView");
}

struct CabinetAtPointContext {
    POINT pt = {};
    HWND found = nullptr;
};

BOOL CALLBACK CabinetAtPointEnumProc(HWND hwnd, LPARAM lParam) {
    auto* ctx = reinterpret_cast<CabinetAtPointContext*>(lParam);
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId() || !IsWindowVisible(hwnd) ||
        !IsExplorerCabinetRoot(hwnd)) {
        return TRUE;
    }
    RECT rc = {};
    if (!GetWindowRect(hwnd, &rc) || !PtInRect(&rc, ctx->pt)) {
        return TRUE;
    }
    ctx->found = hwnd;
    return FALSE;
}

HWND GetExplorerCabinetAtPoint(POINT pt) {
    HWND at = WindowFromPoint(pt);
    if (at) {
        HWND root = GetAncestor(at, GA_ROOT);
        if (root && IsExplorerCabinetRoot(root)) {
            return root;
        }
    }

    // During DoDragDrop, WindowFromPoint often hits a drag-feedback layer whose
    // root is not CabinetWClass even though the cursor is still over an open
    // Explorer frame. Fall back to a same-process, geometry-based cabinet hit
    // test so file-pane wheel forwarding still works mid-drag.
    CabinetAtPointContext ctx{pt};
    EnumWindows(CabinetAtPointEnumProc, reinterpret_cast<LPARAM>(&ctx));
    return ctx.found;
}

struct ExplorerFilePaneWheelTarget {
    HWND defView = nullptr;
    HWND contentHost = nullptr;
};

// Per-drag caches for file-pane wheel scrolling. Re-resolving the DefView /
// ItemsView tree and re-acquiring the UIA scroll pattern on every wheel tick
// was causing visible hitches during fast scrolling.
thread_local struct {
    uint64_t dragGeneration = 0;
    ExplorerFilePaneWheelTarget target{};
} g_filePaneTargetCache;

thread_local struct FilePaneUiScrollCache {
    uint64_t dragGeneration = 0;
    HWND scrollHost = nullptr;
    IUIAutomationScrollPattern* scroll = nullptr;
} g_filePaneUiScrollCache;

void ClearFilePaneUiScrollCache() {
    if (g_filePaneUiScrollCache.scroll) {
        g_filePaneUiScrollCache.scroll->Release();
    }
    g_filePaneUiScrollCache = {};
}

void ClearFilePaneWheelCaches() {
    ClearFilePaneUiScrollCache();
    g_filePaneTargetCache = {};
}

// COM must be initialized on the thread that creates UIA objects. Drag-time
// wheel forwarding runs on the DoDragDrop thread via WH_MOUSE; Explorer often
// already initialized COM there, but we probe explicitly so UIA is reliable.
thread_local struct ThreadComForUIA {
    bool probeDone = false;
    bool comUsable = false;
    bool weInitialized = false;
} g_threadComForUIA;

thread_local IUIAutomation* g_threadUiAutomation = nullptr;

bool EnsureComForUIA() {
    if (g_threadComForUIA.probeDone) {
        return g_threadComForUIA.comUsable;
    }
    g_threadComForUIA.probeDone = true;
    const HRESULT hr =
        CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (hr == RPC_E_CHANGED_MODE) {
        // COM is already active on this thread under a different model; UIA
        // may still work if Explorer initialized the apartment.
        g_threadComForUIA.comUsable = true;
        g_threadComForUIA.weInitialized = false;
    } else if (SUCCEEDED(hr)) {
        g_threadComForUIA.comUsable = true;
        g_threadComForUIA.weInitialized = (hr == S_OK);
    } else {
        g_threadComForUIA.comUsable = false;
        g_threadComForUIA.weInitialized = false;
        Wh_Log(L"CoInitializeEx for UIA failed (hr 0x%08lx)", hr);
    }
    return g_threadComForUIA.comUsable;
}

void TeardownDragThreadWheelState() {
    ClearFilePaneWheelCaches();

    if (g_threadUiAutomation) {
        g_threadUiAutomation->Release();
        g_threadUiAutomation = nullptr;
    }

    if (g_threadComForUIA.weInitialized) {
        CoUninitialize();
    }
    g_threadComForUIA = {};
}

struct ContentHostAtPointContext {
    POINT pt = {};
    HWND found = nullptr;
    LONG bestArea = LONG_MAX;
};

BOOL CALLBACK ContentHostAtPointEnumProc(HWND hwnd, LPARAM lParam) {
    auto* ctx = reinterpret_cast<ContentHostAtPointContext*>(lParam);
    if (IsExplorerFileContentHost(hwnd) && PointInWindow(hwnd, ctx->pt)) {
        RECT rc = {};
        if (GetWindowRect(hwnd, &rc)) {
            LONG area = (rc.right - rc.left) * (rc.bottom - rc.top);
            if (area > 0 && area < ctx->bestArea) {
                ctx->bestArea = area;
                ctx->found = hwnd;
            }
        }
    }
    return TRUE;
}

HWND FindSmallestContentHostUnderDefView(HWND defView, POINT pt) {
    ContentHostAtPointContext ctx{pt};
    EnumChildWindows(defView, ContentHostAtPointEnumProc,
                     reinterpret_cast<LPARAM>(&ctx));
    return ctx.found;
}

ExplorerFilePaneWheelTarget FindExplorerFilePaneWheelTargetAtPoint(POINT pt) {
    ExplorerFilePaneWheelTarget target{};
    HWND cabinet = GetExplorerCabinetAtPoint(pt);
    if (!cabinet) {
        return target;
    }

    HWND defView = FindShellDefViewInCabinet(cabinet);
    if (!defView || !PointInWindow(defView, pt)) {
        return target;
    }
    target.defView = defView;

    POINT clientPt = pt;
    if (!ScreenToClient(defView, &clientPt)) {
        return target;
    }

    // Geometry-based hit test under DefView: works during drag even when
    // WindowFromPoint lands on a drag overlay instead of the file pane.
    HWND hit = ChildWindowFromPointEx(
        defView, clientPt, CWP_SKIPINVISIBLE | CWP_SKIPTRANSPARENT);
    HWND content = nullptr;
    if (hit && hit != defView) {
        for (HWND w = hit; w && w != defView; w = GetParent(w)) {
            if (IsExplorerFileContentHost(w)) {
                content = w;
                break;
            }
        }
        if (!content && IsExplorerFileContentHost(hit)) {
            content = hit;
        }
    }

    if (!content) {
        HWND listView =
            FindWindowExW(defView, nullptr, L"SysListView32", nullptr);
        if (listView && IsExplorerFileContentHost(listView) &&
            PointInWindow(listView, pt)) {
            content = listView;
        }
    }

    if (!content) {
        content = FindSmallestContentHostUnderDefView(defView, pt);
    }

    target.contentHost = content;
    return target;
}

ExplorerFilePaneWheelTarget GetFilePaneWheelTargetCached(POINT pt) {
    const uint64_t generation =
        g_dragGeneration.load(std::memory_order_relaxed);
    const ExplorerFilePaneWheelTarget& cached = g_filePaneTargetCache.target;
    if (g_filePaneTargetCache.dragGeneration == generation && cached.defView &&
        IsWindow(cached.defView) && PointInWindow(cached.defView, pt)) {
        return cached;
    }

    ExplorerFilePaneWheelTarget target =
        FindExplorerFilePaneWheelTargetAtPoint(pt);
    if (target.defView || target.contentHost) {
        if (g_filePaneTargetCache.target.contentHost != target.contentHost) {
            ClearFilePaneUiScrollCache();
        }
        g_filePaneTargetCache.dragGeneration = generation;
        g_filePaneTargetCache.target = target;
    }
    return target;
}

IUIAutomation* GetUIAutomation() {
    if (!EnsureComForUIA()) {
        return nullptr;
    }
    // Per drag-thread instance: avoids a data race on a process-wide static
    // and keeps the automation object on the same thread that runs the WH_MOUSE
    // hook (wheel forwarding during DoDragDrop).
    if (g_threadUiAutomation) {
        return g_threadUiAutomation;
    }
    IUIAutomation* created = nullptr;
    if (FAILED(CoCreateInstance(CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(&created)))) {
        return nullptr;
    }
    g_threadUiAutomation = created;
    return g_threadUiAutomation;
}

// Map one wheel detent (|delta|/WHEEL_DELTA) to how many UIA Scroll() calls to
// issue, using the same SPI the shell honors for line scroll (typically 3).
int UiScrollCallCountForWheelSteps(bool verticalWheel,
                                     int steps,
                                     ScrollAmount* outAmount) {
    if (!outAmount || steps <= 0) {
        return 0;
    }

    UINT unitsPerDetent = 3;
    const UINT spi =
        verticalWheel ? SPI_GETWHEELSCROLLLINES : SPI_GETWHEELSCROLLCHARS;
    if (!SystemParametersInfoW(spi, 0, &unitsPerDetent, 0)) {
        unitsPerDetent = 3;
    }

    if (unitsPerDetent == WHEEL_PAGESCROLL) {
        *outAmount = ScrollAmount_LargeIncrement;
        int scrollCalls = steps;
        if (scrollCalls > kMaxUiScrollCallsPerWheelMessage) {
            scrollCalls = kMaxUiScrollCallsPerWheelMessage;
        }
        return scrollCalls;
    }

    if (unitsPerDetent == 0) {
        unitsPerDetent = 3;
    }
    if (unitsPerDetent > 20) {
        unitsPerDetent = 20;
    }

    *outAmount = ScrollAmount_SmallIncrement;
    int scrollCalls = steps * static_cast<int>(unitsPerDetent);
    if (scrollCalls > kMaxUiScrollCallsPerWheelMessage) {
        scrollCalls = kMaxUiScrollCallsPerWheelMessage;
    }
    if (scrollCalls < 1) {
        scrollCalls = 1;
    }
    return scrollCalls;
}

// Scroll via UIA so we never post wheel/scroll messages to SHELLDLL_DefView
// during DoDragDrop (those can cancel or complete the OLE drag).
bool ScrollExplorerPaneViaUIAutomation(HWND hwnd,
                                       int steps,
                                       bool verticalWheel,
                                       bool increment) {
    if (!hwnd || steps <= 0 || !IsWindow(hwnd)) {
        return false;
    }

    const uint64_t generation =
        g_dragGeneration.load(std::memory_order_relaxed);
    IUIAutomationScrollPattern* scroll = nullptr;
    if (g_filePaneUiScrollCache.scroll &&
        g_filePaneUiScrollCache.scrollHost == hwnd &&
        g_filePaneUiScrollCache.dragGeneration == generation) {
        scroll = g_filePaneUiScrollCache.scroll;
    } else {
        ClearFilePaneUiScrollCache();

        IUIAutomation* automation = GetUIAutomation();
        if (!automation) {
            return false;
        }

        IUIAutomationElement* element = nullptr;
        if (FAILED(automation->ElementFromHandle(hwnd, &element)) ||
            !element) {
            return false;
        }

        IUIAutomationScrollPattern* acquired = nullptr;
        const HRESULT patternHr = element->GetCurrentPatternAs(
            UIA_ScrollPatternId, IID_PPV_ARGS(&acquired));
        element->Release();
        if (FAILED(patternHr) || !acquired) {
            return false;
        }

        g_filePaneUiScrollCache.dragGeneration = generation;
        g_filePaneUiScrollCache.scrollHost = hwnd;
        g_filePaneUiScrollCache.scroll = acquired;
        scroll = acquired;
    }

    ScrollAmount primaryAmount = ScrollAmount_SmallIncrement;
    int scrollCalls =
        UiScrollCallCountForWheelSteps(verticalWheel, steps, &primaryAmount);
    if (scrollCalls <= 0) {
        return false;
    }

    if (!increment) {
        if (primaryAmount == ScrollAmount_LargeIncrement) {
            primaryAmount = ScrollAmount_LargeDecrement;
        } else {
            primaryAmount = ScrollAmount_SmallDecrement;
        }
    }

    const ScrollAmount noAmount = ScrollAmount_NoAmount;
    ScrollAmount horizontalAmount = noAmount;
    ScrollAmount verticalAmount = noAmount;
    if (verticalWheel) {
        verticalAmount = primaryAmount;
    } else {
        horizontalAmount = primaryAmount;
    }

    for (int i = 0; i < scrollCalls; i++) {
        scroll->Scroll(horizontalAmount, verticalAmount);
    }
    return true;
}

void ForwardWheelToExplorerFilePane(const ExplorerFilePaneWheelTarget& target,
                                    UINT wheelMsg,
                                    const MOUSEHOOKSTRUCTEX* info) {
    if (!target.defView && !target.contentHost) {
        return;
    }

    const short delta = static_cast<short>(HIWORD(info->mouseData));
    if (delta == 0) {
        return;
    }

    int steps = static_cast<int>(delta / WHEEL_DELTA);
    if (steps < 0) {
        steps = -steps;
    }
    if (steps == 0) {
        steps = 1;
    }

    const bool verticalWheel = (wheelMsg == WM_MOUSEWHEEL);
    const bool increment = (delta < 0);

    // UIA first for every resolved content host (ItemsView, DirectUI, and
    // SysListView32 in Large icons / List / etc.). Avoids posting wheel/scroll
    // messages to SHELLDLL_DefView during DoDragDrop when the pattern is available.
    if (target.contentHost) {
        if (ScrollExplorerPaneViaUIAutomation(target.contentHost, steps,
                                              verticalWheel, increment)) {
            return;
        }
    }

    HWND wheelTarget = target.contentHost;
    const bool listViewContentHost =
        wheelTarget && WindowClassEquals(wheelTarget, L"SysListView32");
    if (!wheelTarget) {
        wheelTarget = target.defView;
    }
    if (!wheelTarget) {
        return;
    }

    // Never message DefView when we already resolved a modern (non-ListView)
    // content host -- that was ending the drag in testing.
    if (target.contentHost && !listViewContentHost &&
        wheelTarget == target.defView) {
        return;
    }

    const WPARAM wheelWParam = MAKEWPARAM(0, HIWORD(info->mouseData));
    const LPARAM screenLParam = MAKELPARAM(info->pt.x, info->pt.y);
    DWORD_PTR replyDummy = 0;
    SendMessageTimeoutW(wheelTarget, wheelMsg, wheelWParam, screenLParam,
                        SMTO_NORMAL | SMTO_ABORTIFHUNG, kWheelForwardTimeoutMs,
                        &replyDummy);
}

HTREEITEM TreeParent(HWND hwndTree, HTREEITEM hItem) {
    return reinterpret_cast<HTREEITEM>(
        SendInternal(hwndTree, TVM_GETNEXTITEM, TVGN_PARENT,
                     reinterpret_cast<LPARAM>(hItem)));
}

int TreeItemDepth(HWND hwndTree, HTREEITEM hItem) {
    int depth = 0;
    int guard = 0;
    // Defensive limit in case the control returns unexpected handles while
    // it is mutating during lazy expansion.
    for (; hItem && guard < 32; guard++) {
        HTREEITEM parent = TreeParent(hwndTree, hItem);
        if (!parent) {
            break;
        }
        depth++;
        hItem = parent;
    }
    if (guard >= 32) {
        // The guard fires at the boundary, so the tree might be exactly 32
        // levels deep (correctly walked) or deeper (truncated). We can't
        // distinguish without another TreeParent call; the log just notes
        // that we capped.
        Wh_Log(L"TreeItemDepth capped at 32 levels; tree may be exactly 32 "
               L"deep or deeper");
    }
    return depth;
}

bool IsRootishItem(HWND hwndTree, HTREEITEM hItem) {
    return hItem && TreeItemDepth(hwndTree, hItem) <=
                        g_settings.protectedMaxDepth.load(
                            std::memory_order_relaxed);
}

bool IsSameOrDescendant(HWND hwndTree, HTREEITEM hItem,
                        HTREEITEM possibleAncestor) {
    if (!hItem || !possibleAncestor) {
        return false;
    }
    int guard = 0;
    for (; hItem && guard < 32; guard++) {
        if (hItem == possibleAncestor) {
            return true;
        }
        hItem = TreeParent(hwndTree, hItem);
    }
    if (guard >= 32) {
        Wh_Log(L"IsSameOrDescendant capped at 32 levels");
    }
    return false;
}

HTREEITEM GetFirstVisibleItem(HWND hwndTree) {
    return reinterpret_cast<HTREEITEM>(
        SendInternal(hwndTree, TVM_GETNEXTITEM, TVGN_FIRSTVISIBLE, 0));
}

bool GetTreeItemRowRectScreen(HWND hwndTree, HTREEITEM hItem,
                              RECT* rowRectScreen) {
    RECT rect = {};
    // TVM_GETITEMRECT's documented protocol: write the HTREEITEM into the
    // first bytes of the RECT (overlapping left/top); the control overwrites
    // the entire RECT with the row's bounding box on success. The reinterpret
    // pointer write that previous versions used was the literal Win32 idiom
    // but tripped strict-aliasing diagnostics; memcpy expresses the same
    // byte-wise overlay without the appearance of UB.
    //
    // sizeof(HTREEITEM) (the type) rather than sizeof(hItem) (the expression)
    // because clang-tidy's bugprone-sizeof-expression flags sizeof on a
    // pointer-typed expression -- usually a real bug, but here we genuinely
    // want the size of the handle itself per the TVM_GETITEMRECT contract.
    // The two forms are byte-identical; the type spelling makes intent
    // explicit and silences the lint.
    static_assert(sizeof(HTREEITEM) <= sizeof(LONG) * 2,
                  "HTREEITEM must fit in the first two LONG fields of RECT "
                  "per the TVM_GETITEMRECT contract");
    memcpy(&rect, &hItem, sizeof(HTREEITEM));

    if (!SendInternal(hwndTree, TVM_GETITEMRECT, FALSE,
                      reinterpret_cast<LPARAM>(&rect))) {
        return false;
    }
    MapWindowPoints(hwndTree, nullptr, reinterpret_cast<POINT*>(&rect), 2);
    *rowRectScreen = rect;
    return true;
}

bool HitTestCursorInTree(HWND hwndTree, TVHITTESTINFO* hit,
                         POINT* cursorScreen) {
    POINT screen = {};
    if (!GetCursorPos(&screen)) {
        return false;
    }
    POINT client = screen;
    if (!ScreenToClient(hwndTree, &client)) {
        return false;
    }
    RECT clientRect = {};
    GetClientRect(hwndTree, &clientRect);
    if (!PtInRect(&clientRect, client)) {
        return false;
    }
    TVHITTESTINFO localHit = {};
    localHit.pt = client;
    SendInternal(hwndTree, TVM_HITTEST, 0,
                 reinterpret_cast<LPARAM>(&localHit));
    if (!localHit.hItem) {
        return false;
    }
    // Ignore non-item zones. Dropping to the right of the label still maps
    // to the item row in Explorer, so hItem being set is what matters.
    if (localHit.flags & (TVHT_ABOVE | TVHT_BELOW | TVHT_NOWHERE |
                          TVHT_TOLEFT | TVHT_TORIGHT)) {
        return false;
    }
    *hit = localHit;
    *cursorScreen = screen;
    return true;
}

// If the TreeView's visible drop-highlight is still on the item we just
// released, clear it. Keeps the visual in sync when our capture moves on
// before Explorer sends its next TVM_SELECTITEM TVGN_DROPHILITE.
void ClearStaleDropHiliteOn(HWND hwndTree, HTREEITEM previousHItem) {
    if (!previousHItem) {
        return;
    }
    HTREEITEM currentHilite = reinterpret_cast<HTREEITEM>(
        SendInternal(hwndTree, TVM_GETNEXTITEM, TVGN_DROPHILITE, 0));
    if (currentHilite == previousHItem) {
        SendInternal(hwndTree, TVM_SELECTITEM, TVGN_DROPHILITE, 0);
    }
}

bool CursorStillOnCapturedRow(HWND hwndTree, const POINT& cursor) {
    if (!g_protectedHover.active || g_protectedHover.hwndTree != hwndTree) {
        return false;
    }
    int verticalMove = cursor.y - g_protectedHover.capturedCursorScreen.y;
    if (verticalMove < 0) {
        verticalMove = -verticalMove;
    }
    if (verticalMove >
        g_settings.releaseMoveY.load(std::memory_order_relaxed)) {
        return false;
    }
    RECT row = g_protectedHover.rowRectScreen;
    InflateRect(
        &row,
        g_settings.capturedRowMarginX.load(std::memory_order_relaxed),
        g_settings.capturedRowMarginY.load(std::memory_order_relaxed));
    return PtInRect(&row, cursor);
}

bool CursorStillOnCapturedRow(HWND hwndTree) {
    POINT cursor = {};
    if (!GetCursorPos(&cursor)) {
        return false;
    }
    return CursorStillOnCapturedRow(hwndTree, cursor);
}

void UpdateProtectedHoverFromCursor(HWND hwndTree) {
    POINT cursor = {};
    if (!GetCursorPos(&cursor)) {
        return;
    }

    // If the captured state was recorded during a previous drag generation,
    // discard it before any of the throttle / cursor-stable shortcuts can
    // re-use it. DoDragDrop_Hook only clears the dragging thread's
    // thread_local copy, so tree-owner threads must invalidate themselves.
    if (g_protectedHover.active &&
        g_protectedHover.generation !=
            g_dragGeneration.load(std::memory_order_acquire)) {
        ClearProtectedHover();
    }

    // Throttle: if the cursor screen position has not moved since the last
    // update for this tree, the prior capture decision is still valid and we
    // can avoid the hit-test / depth-walk work.
    if (g_protectedHover.active &&
        g_protectedHover.hwndTree == hwndTree &&
        cursor.x == g_protectedHover.lastSeenCursorScreen.x &&
        cursor.y == g_protectedHover.lastSeenCursorScreen.y) {
        return;
    }

    // If Explorer already scrolled the TreeView, a fresh hit test may now
    // point at a child under the stationary cursor. Keep the captured parent
    // until the cursor actually leaves that original screen row.
    if (CursorStillOnCapturedRow(hwndTree, cursor)) {
        g_protectedHover.lastSeenCursorScreen = cursor;
        return;
    }

    HTREEITEM previousHItem = nullptr;
    if (g_protectedHover.active && g_protectedHover.hwndTree == hwndTree) {
        previousHItem = g_protectedHover.hItem;
        ClearProtectedHover();
    }

    TVHITTESTINFO hit = {};
    POINT cursorScreen = {};
    if (!HitTestCursorInTree(hwndTree, &hit, &cursorScreen)) {
        // No new capture coming, but the released item may still be visibly
        // drop-highlighted; clear it so Explorer's next DROPHILITE message
        // paints fresh state instead of layering on top of stale highlight.
        ClearStaleDropHiliteOn(hwndTree, previousHItem);
        return;
    }
    if (!IsRootishItem(hwndTree, hit.hItem)) {
        // Same rationale as the no-hit branch above: clear lingering visible
        // drop-highlight on the released item so the non-root-ish row under
        // the cursor receives a clean DROPHILITE update from Explorer.
        ClearStaleDropHiliteOn(hwndTree, previousHItem);
        return;
    }
    RECT rowRectScreen = {};
    if (!GetTreeItemRowRectScreen(hwndTree, hit.hItem, &rowRectScreen)) {
        // We've already abandoned the previous capture but can't establish a
        // new one. Same rationale as the no-hit / non-root-ish branches above:
        // clear the released item's drop highlight so Explorer's next
        // DROPHILITE message paints fresh state.
        ClearStaleDropHiliteOn(hwndTree, previousHItem);
        return;
    }

    g_protectedHover.active = true;
    g_protectedHover.hwndTree = hwndTree;
    g_protectedHover.hItem = hit.hItem;
    g_protectedHover.capturedCursorScreen = cursorScreen;
    g_protectedHover.lastSeenCursorScreen = cursorScreen;
    g_protectedHover.rowRectScreen = rowRectScreen;
    g_protectedHover.generation =
        g_dragGeneration.load(std::memory_order_acquire);

    // Explorer doesn't always re-send TVM_SELECTITEM TVGN_DROPHILITE for every
    // cursor position (especially on fast moves between two root-ish rows),
    // which can leave the previous row's visible drop-highlight stuck while
    // our capture has already moved on. Proactively sync the TreeView's
    // current drop-highlight to the newly captured item.
    HTREEITEM currentHilite = reinterpret_cast<HTREEITEM>(
        SendInternal(hwndTree, TVM_GETNEXTITEM, TVGN_DROPHILITE, 0));
    if (currentHilite != hit.hItem) {
        SendInternal(hwndTree, TVM_SELECTITEM, TVGN_DROPHILITE,
                     reinterpret_cast<LPARAM>(hit.hItem));
    }
}

bool HasProtectedHover(HWND hwndTree) {
    return IsDragLoopActive() &&
           g_protectedHover.active &&
           g_protectedHover.hwndTree == hwndTree &&
           g_protectedHover.generation ==
               g_dragGeneration.load(std::memory_order_acquire) &&
           CursorStillOnCapturedRow(hwndTree);
}

void RestoreViewportAndDropTarget(HWND hwndTree, HTREEITEM hFirstVisible) {
    if (!HasProtectedHover(hwndTree)) {
        return;
    }
    // Only re-issue the viewport / drop-highlight messages if the TreeView's
    // current state has actually drifted -- expansion frequently leaves both
    // unchanged, and the no-op writes still trigger a layout / repaint pass.
    if (hFirstVisible) {
        HTREEITEM currentFirstVisible = GetFirstVisibleItem(hwndTree);
        if (currentFirstVisible != hFirstVisible) {
            SendInternal(hwndTree, TVM_SELECTITEM, TVGN_FIRSTVISIBLE,
                         reinterpret_cast<LPARAM>(hFirstVisible));
        }
    }
    // If Explorer retargeted the drop highlight to a child that appeared
    // under the stationary cursor, put it back on the parent row. Explorer
    // is free to retarget once the user physically moves to a child row.
    HTREEITEM currentHilite = reinterpret_cast<HTREEITEM>(
        SendInternal(hwndTree, TVM_GETNEXTITEM, TVGN_DROPHILITE, 0));
    if (currentHilite != g_protectedHover.hItem) {
        SendInternal(hwndTree, TVM_SELECTITEM, TVGN_DROPHILITE,
                     reinterpret_cast<LPARAM>(g_protectedHover.hItem));
    }
}

// Windhawk's WH_SUBCLASSPROC drops the standard SUBCLASSPROC's uIdSubclass
// parameter -- the utility manages the subclass ID internally and only passes
// dwRefData through.
LRESULT CALLBACK TreeSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                  LPARAM lParam, DWORD_PTR /*dwRefData*/) {
    if (uMsg == WM_NCDESTROY) {
        {
            std::lock_guard<std::mutex> lock(g_subclassMutex);
            g_subclassedTrees.erase(hWnd);
        }
        // Safe if Wh_ModUninit already removed the subclass on this HWND.
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd,
                                                         TreeSubclassProc);
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    if (g_uninitInProgress.load(std::memory_order_acquire)) {
        {
            std::lock_guard<std::mutex> lock(g_subclassMutex);
            g_subclassedTrees.erase(hWnd);
        }
        // Duplicate Remove is a no-op when uninit already ran.
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd,
                                                         TreeSubclassProc);
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    if (g_internalCallDepth > 0 || !IsDragLoopActive()) {
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    // Refresh on every drag-time message. UpdateProtectedHoverFromCursor is
    // cheap when the cursor hasn't moved (early return after a position
    // comparison) and is the entry point for the proactive drop-highlight
    // sync. Filtering to only the four decision messages below would skip
    // refresh on the high-frequency messages Explorer's drop target uses
    // (TVM_HITTEST in particular), which lets the visible highlight lag the
    // cursor on quick moves between root-ish rows.
    UpdateProtectedHoverFromCursor(hWnd);
    const bool hasProtectedHover = HasProtectedHover(hWnd);

    if (hasProtectedHover) {
        switch (uMsg) {
            case TVM_EXPAND: {
                HTREEITEM expandedItem = reinterpret_cast<HTREEITEM>(lParam);
                if ((wParam & TVE_EXPAND) &&
                    expandedItem == g_protectedHover.hItem) {
                    HTREEITEM firstVisibleBefore = GetFirstVisibleItem(hWnd);
                    // Stamp BEFORE DefSubclassProc: some Explorer builds emit
                    // a synchronous WM_VSCROLL during the expansion cascade,
                    // and we want that to land inside the suppression window
                    // so the WM_VSCROLL handler swallows it.
                    NoteJumpAttempt();
                    // Allow normal auto-expansion. The guard only prevents
                    // the viewport / drop target from jumping afterward.
                    LRESULT result =
                        DefSubclassProc(hWnd, uMsg, wParam, lParam);
                    RestoreViewportAndDropTarget(hWnd, firstVisibleBefore);
                    return result;
                }
                break;
            }

            case TVM_ENSUREVISIBLE: {
                HTREEITEM targetItem = reinterpret_cast<HTREEITEM>(lParam);
                if (IsSameOrDescendant(hWnd, targetItem,
                                       g_protectedHover.hItem)) {
                    // Cursor is already over the parent row. We pretend the
                    // scroll succeeded (TRUE = scrolled) to suppress
                    // Explorer's "ensure visible" jumping new children under
                    // the cursor. A small lie -- the row was already
                    // on-screen anyway.
                    NoteJumpAttempt();
                    return TRUE;
                }
                break;
            }

            case TVM_SELECTITEM:
                if (wParam == TVGN_FIRSTVISIBLE) {
                    // Aggressive viewport-jump path: Explorer asks the
                    // TreeView to place an item at the top of the viewport.
                    NoteJumpAttempt();
                    return TRUE;
                }
                if (wParam == TVGN_DROPHILITE && lParam != 0 &&
                    reinterpret_cast<HTREEITEM>(lParam) !=
                        g_protectedHover.hItem) {
                    SendInternal(hWnd, TVM_SELECTITEM, TVGN_DROPHILITE,
                                 reinterpret_cast<LPARAM>(
                                     g_protectedHover.hItem));
                    return TRUE;
                }
                break;

            case WM_VSCROLL: {
                // Two different things can fire a line-based WM_VSCROLL on a
                // protected tree:
                //   1. The scroll tail of an Explorer-initiated jump
                //      (TVM_EXPAND / TVM_SELECTITEM / TVM_ENSUREVISIBLE that
                //      we just suppressed). This is what we want to swallow.
                //   2. Edge-scroll-while-dragging: Explorer's built-in
                //      behavior where pinning the cursor to the top/bottom
                //      of the window auto-scrolls the pane during DoDragDrop.
                //      This is legitimate user input -- it must pass through.
                // The messages themselves are indistinguishable, so use the
                // suppression-window timestamp: a line-based scroll arriving
                // right after a stamped jump attempt is (1); anything outside
                // the window is (2).
                WORD scrollCode = LOWORD(wParam);
                if (scrollCode == SB_THUMBTRACK ||
                    scrollCode == SB_THUMBPOSITION ||
                    scrollCode == SB_ENDSCROLL) {
                    // Thumb interactions can't really fire during DoDragDrop
                    // (the LMB is captured by the drag), but keep the
                    // pass-through for defense in depth.
                    break;
                }
                if (InPostJumpScrollSuppressionWindow()) {
                    return 0;
                }
                break;
            }
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

void TrySubclassTree(HWND hwnd) {
    // Cheap shutdown fast-path. Both discovery paths (WinEventProc and
    // EnumWindows) flow through TrySubclassTree, so a single check here
    // covers them. g_helperStopping covers the WinEventProc path's race
    // window (it is set before the helper is signalled); g_uninitInProgress
    // covers the remaining time between the helper exiting and the subclass
    // sweep finishing. Either flag alone would leave a small gap.
    // Re-checked again below after the in-flight counter bump.
    if (g_uninitInProgress.load(std::memory_order_acquire) ||
        g_helperStopping.load(std::memory_order_acquire)) {
        return;
    }
    if (!IsExplorerNavigationTree(hwnd)) {
        return;
    }

    // Account for this in-flight install so Wh_ModUninit's bounded wait can
    // see us. RAII decrement at every return below. Without this counter,
    // SetWindowSubclassFromAnyThread (which is SendMessage-synchronous) and
    // an uninit-dispatched RemoveWindowSubclassFromAnyThread for the same
    // hwnd are two senders SendMessaging the same receiver thread; if the
    // target thread services the Remove first the Remove is a no-op against
    // an unsubclassed window, then the Install lands and survives DLL
    // unload. The bounded wait in Wh_ModUninit serialises against this.
    struct OpGuard {
        OpGuard() {
            g_subclassOpsInFlight.fetch_add(1, std::memory_order_acq_rel);
        }
        ~OpGuard() {
            g_subclassOpsInFlight.fetch_sub(1, std::memory_order_acq_rel);
        }
    } opGuard;

    // Re-check after the bump. If Wh_ModUninit flipped the flag between our
    // first check and our increment, it may have already observed the counter
    // as zero and decided to proceed; we must not continue past this point
    // or we'd insert/install behind its back.
    if (g_uninitInProgress.load(std::memory_order_acquire) ||
        g_helperStopping.load(std::memory_order_acquire)) {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(g_subclassMutex);
        auto [it, inserted] =
            g_subclassedTrees.try_emplace(hwnd, SubclassState::Installing);
        (void)it;
        if (!inserted) {
            // Another caller is mid-install or already finished. Either way
            // we do nothing -- they own the entry.
            return;
        }
    }
    // WindhawkUtils marshals the actual SetWindowSubclass call to the
    // window's owning thread, which is the supported pattern per Microsoft's
    // SetWindowSubclass docs. The map entry is in the Installing state
    // across this call so any concurrent TrySubclassTree on the same hwnd
    // sees us and bails.
    if (WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, TreeSubclassProc,
                                                       0)) {
        std::lock_guard<std::mutex> lock(g_subclassMutex);
        auto it = g_subclassedTrees.find(hwnd);
        if (it != g_subclassedTrees.end()) {
            it->second = SubclassState::Installed;
            Wh_Log(L"Subclassed nav tree %p", hwnd);
        } else {
            // WM_NCDESTROY erased our Installing entry while the cross-thread
            // install was in flight; drop the subclass so we don't leak an
            // untracked pointer into our image.
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                hwnd, TreeSubclassProc);
        }
    } else {
        // Roll back the Installing marker so a future retry on the same
        // hwnd can proceed instead of being stuck behind a phantom entry.
        std::lock_guard<std::mutex> lock(g_subclassMutex);
        g_subclassedTrees.erase(hwnd);
        Wh_Log(L"SetWindowSubclassFromAnyThread failed for %p", hwnd);
    }
}

void CALLBACK WinEventProc(HWINEVENTHOOK /*hook*/, DWORD event, HWND hwnd,
                           LONG idObject, LONG /*idChild*/,
                           DWORD /*eventThread*/, DWORD /*eventTime*/) {
    // Drop late callbacks during shutdown. Wh_ModUninit sets this flag
    // BEFORE signalling g_helperStopEvent, so any callbacks dispatched out
    // of the queue before the wait returns see it and bail out instead of
    // installing subclasses that would immediately have to be torn down.
    if (g_helperStopping.load(std::memory_order_acquire)) {
        return;
    }
    if (event != EVENT_OBJECT_CREATE || idObject != OBJID_WINDOW || !hwnd) {
        return;
    }
    TrySubclassTree(hwnd);
}

DWORD WINAPI WinEventHookThread(LPVOID /*param*/) {
    // WINEVENT_OUTOFCONTEXT delivers callbacks to the thread that installed
    // the hook; that thread must keep pumping messages. Explorer's main
    // threads pump; Windhawk's loader/init thread does not, so we host the
    // hook here on a dedicated thread with its own message loop.

    // Force message queue creation BEFORE signalling ready. Even though we
    // no longer rely on PostThreadMessageW for shutdown, SetWinEventHook
    // itself and various Win32 callbacks expect a thread queue to exist.
    // PeekMessageW with PM_NOREMOVE is the documented way to provoke queue
    // creation without disturbing pending messages.
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    g_winEventHook = SetWinEventHook(
        EVENT_OBJECT_CREATE, EVENT_OBJECT_CREATE, nullptr, WinEventProc,
        GetCurrentProcessId(), 0, WINEVENT_OUTOFCONTEXT);
    if (!g_winEventHook) {
        Wh_Log(L"SetWinEventHook failed; new trees will not be guarded");
    }

    // Signal ready unconditionally so Wh_ModInit doesn't block indefinitely
    // even on hook-install failure. After SetEvent, Wh_ModUninit is allowed
    // to observe the thread either still pumping (hook installed) or already
    // exited via the early return below (hook failed).
    if (g_winEventThreadReady) {
        SetEvent(g_winEventThreadReady);
    }

    if (!g_winEventHook) {
        return 1;
    }

    // Event-based message loop. MsgWaitForMultipleObjectsEx returns
    // WAIT_OBJECT_0 the moment g_helperStopEvent is signalled, even if there
    // are WinEvent callbacks queued -- the function returns the lowest-index
    // signalled handle, and the event sits at index 0 ahead of QS_ALLINPUT.
    // This makes shutdown strictly deterministic: SetEvent never fails the
    // way PostThreadMessageW could when the target thread's queue is wedged,
    // and we never have to drain queued callbacks before exiting.
    //
    // MWMO_INPUTAVAILABLE is required so that messages already in the queue
    // before the wait call are seen (without the flag, the function only
    // wakes for messages arriving AFTER the call).
    bool quit = false;
    while (!quit) {
        DWORD waitResult = MsgWaitForMultipleObjectsEx(
            1, &g_helperStopEvent, INFINITE, QS_ALLINPUT,
            MWMO_INPUTAVAILABLE);
        if (waitResult == WAIT_OBJECT_0) {
            quit = true;
            break;
        }
        if (waitResult == WAIT_OBJECT_0 + 1) {
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) {
                    // No code path in this mod posts WM_QUIT, but if
                    // anything ever does, treat it as a stop signal so we
                    // don't infinite-loop redispatching it.
                    quit = true;
                    break;
                }
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        } else {
            // Unexpected return code (WAIT_FAILED etc.); break out so the
            // helper can at least unhook cleanly instead of spinning.
            Wh_Log(L"MsgWaitForMultipleObjectsEx unexpected result %lu "
                   L"(err %lu); exiting helper",
                   waitResult, GetLastError());
            quit = true;
        }
    }

    // UnhookWinEvent must run on the thread that installed the hook. This
    // is the only safe place to do it.
    if (g_winEventHook) {
        UnhookWinEvent(g_winEventHook);
        g_winEventHook = nullptr;
    }
    return 0;
}

BOOL CALLBACK EnumChildSubclassProc(HWND hwnd, LPARAM /*lParam*/) {
    TrySubclassTree(hwnd);
    return TRUE;
}

BOOL CALLBACK EnumTopLevelExplorerWindowsProc(HWND hwnd, LPARAM /*lParam*/) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId()) {
        return TRUE;
    }
    if (WindowClassEquals(hwnd, L"CabinetWClass") ||
        WindowClassEquals(hwnd, L"ExploreWClass")) {
        EnumChildWindows(hwnd, EnumChildSubclassProc, 0);
    }
    return TRUE;
}

// WH_MOUSE hook proc. Fires for every mouse message on the installing
// thread's queue, including the WM_MOUSEWHEEL events OLE's drag loop would
// otherwise eat. We redirect those to whichever tracked nav tree the cursor
// is over and bump the drag generation so any captured hover state is
// invalidated -- if the user is wheel-scrolling, they're intentionally moving
// past the captured row.
LRESULT CALLBACK DragMouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode != HC_ACTION) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }
    if (wParam != WM_MOUSEWHEEL && wParam != WM_MOUSEHWHEEL) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }
    if (!g_settings.enableWheelScrollDuringDrag.load(
            std::memory_order_relaxed)) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }
    auto* info = reinterpret_cast<MOUSEHOOKSTRUCTEX*>(lParam);
    if (!info) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    POINT pt = info->pt;
    HWND tree = nullptr;
    for (HWND w = WindowFromPoint(pt); w; w = GetParent(w)) {
        // Prefer nav-tree routing when the cursor is over the tree.
        if (IsExplorerNavigationTree(w)) {
            tree = w;
            break;
        }
    }

    ExplorerFilePaneWheelTarget filePane{};
    if (!tree) {
        filePane = GetFilePaneWheelTargetCached(pt);
    }

    const bool hasFilePaneTarget =
        filePane.defView || filePane.contentHost;
    if (!tree && !hasFilePaneTarget) {
        // Cursor isn't over any tracked Explorer scroll target. Let normal
        // processing run (which means OLE will still eat wheel input, matching
        // native behavior for non-scroll targets during drag).
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    if (tree) {
        // SendMessage bypasses the message queue, so the dispatched
        // WM_MOUSEWHEEL does NOT re-enter this hook -- no recursion concern.
        DWORD_PTR replyDummy = 0;
        SendMessageTimeoutW(
            tree, static_cast<UINT>(wParam),
            MAKEWPARAM(0, HIWORD(info->mouseData)), MAKELPARAM(pt.x, pt.y),
            SMTO_NORMAL | SMTO_ABORTIFHUNG, kWheelForwardTimeoutMs,
            &replyDummy);
    } else {
        ForwardWheelToExplorerFilePane(filePane, static_cast<UINT>(wParam),
                                       info);
    }

    if (tree) {
        // Bump the drag generation AFTER the synthesized wheel has actually
        // scrolled the tree. Any capture established mid-dispatch by
        // UpdateProtectedHoverFromCursor would have cached pre-scroll item
        // rects (now stale); bumping here forces the next message on the
        // tree's thread to clear that capture and re-evaluate against post-
        // scroll geometry. Bumping BEFORE SendMessageTimeoutW would itself
        // create a mid-dispatch stale capture, since the tree hadn't yet
        // scrolled at hit-test time.
        g_dragGeneration.fetch_add(1, std::memory_order_acq_rel);
    }

    // Returning non-zero stops the system from passing the message through
    // to OLE's drag loop, which would otherwise eat it.
    return 1;
}

HRESULT WINAPI DoDragDrop_Hook(IDataObject* pDataObj, IDropSource* pDropSource,
                               DWORD dwOKEffects, DWORD* pdwEffect) {
    // Shutdown fast-path. If Wh_ModUninit has signalled teardown but a fresh
    // drag started before the hook engine restored the original function,
    // bypass all mod-side state: don't install a mouse hook the uninit sweep
    // already finished, don't mutate g_dragLoopDepth, don't alter drag
    // generation. The downstream gates in TreeSubclassProc and
    // DragMouseHookProc also check g_uninitInProgress, but checking it here
    // makes the boundary explicit.
    //
    // We rely on Windhawk's hook engine to keep this DLL's image mapped
    // until in-flight DoDragDrop_Hook frames return -- without that
    // guarantee, returning from DoDragDrop_Original would jump back into
    // freed code. The same is true of every Windhawk mod that hooks a
    // modal-loop API (DoDragDrop, MessageBox, DialogBoxIndirect, GetMessage,
    // ...), so the guarantee has to exist. Adding our own in-flight counter
    // here would be observable but not actionable: waiting for a drag to
    // complete in Wh_ModUninit could block for minutes, which is worse than
    // the current behaviour.
    if (g_uninitInProgress.load(std::memory_order_acquire)) {
        return DoDragDrop_Original(pDataObj, pDropSource, dwOKEffects,
                                   pdwEffect);
    }

    // On the 0 -> 1 edge, bump the generation so any leftover hover capture
    // on other (tree-owning) threads is invalidated before they consult it.
    if (g_dragLoopDepth.fetch_add(1, std::memory_order_acq_rel) == 0) {
        g_dragGeneration.fetch_add(1, std::memory_order_acq_rel);
    }

    // Install the per-thread mouse hook for the duration of the modal drag
    // loop. g_dragMouseHook is thread_local; g_dragHookMutex only serializes
    // updates to g_activeDragHooks (visible to Wh_ModUninit). The
    // SetWindowsHookExW call and the g_activeDragHooks insert run under that
    // mutex so an uninit sweep cannot snapshot an empty g_activeDragHooks in
    // the window between the hook being installed and the registry being
    // updated -- that gap would otherwise leave a live hook handle pointing
    // into our about-to-be-unmapped DragMouseHookProc.
    // We also re-check g_uninitInProgress while holding the lock for the
    // same reason. SetWindowsHookExW is a syscall that does not recurse into
    // our code, so calling it under the mutex is deadlock-safe.
    //
    // Nested drags on the same thread share one hook -- only the outermost
    // install removes it. On failure we log and silently degrade (no wheel
    // scroll); the rest of the mod still works.
    bool installedHook = false;
    {
        std::lock_guard<std::mutex> lock(g_dragHookMutex);
        if (!g_uninitInProgress.load(std::memory_order_acquire) &&
            g_settings.enableWheelScrollDuringDrag.load(
                std::memory_order_relaxed) &&
            !g_dragMouseHook) {
            g_dragMouseHook = SetWindowsHookExW(
                WH_MOUSE, DragMouseHookProc, nullptr, GetCurrentThreadId());
            if (!g_dragMouseHook) {
                Wh_Log(L"SetWindowsHookExW(WH_MOUSE) failed (err %lu); "
                       L"wheel-scroll-during-drag disabled for this drag",
                       GetLastError());
            } else {
                g_activeDragHooks.insert(g_dragMouseHook);
                installedHook = true;
            }
        }
    }

    HRESULT result =
        DoDragDrop_Original(pDataObj, pDropSource, dwOKEffects, pdwEffect);

    if (installedHook && g_dragMouseHook) {
        {
            std::lock_guard<std::mutex> lock(g_dragHookMutex);
            g_activeDragHooks.erase(g_dragMouseHook);
        }
        // If Wh_ModUninit already swept the hook out from under us, this
        // call is a no-op (returns FALSE, GetLastError() == ERROR_INVALID_HOOK_HANDLE).
        // Either way, drop our thread-local handle.
        UnhookWindowsHookEx(g_dragMouseHook);
        g_dragMouseHook = nullptr;
    }

    if (g_dragLoopDepth.fetch_sub(1, std::memory_order_acq_rel) == 1) {
        ClearProtectedHover();
        TeardownDragThreadWheelState();
    }
    return result;
}

int ClampInt(int value, int lo, int hi) {
    if (value < lo) return lo;
    if (value > hi) return hi;
    return value;
}

void LoadSettings() {
    int depth = static_cast<int>(Wh_GetIntSetting(L"protectedMaxDepth"));
    int releaseY = static_cast<int>(Wh_GetIntSetting(L"releaseMoveY"));
    int marginX = static_cast<int>(Wh_GetIntSetting(L"capturedRowMarginX"));
    int marginY = static_cast<int>(Wh_GetIntSetting(L"capturedRowMarginY"));

    // Upper bounds keep a typo'd setting from making the nav pane feel broken
    // (e.g. depth 999, release 500). The ranges are intentionally generous
    // relative to the defaults so power users still have headroom.
    g_settings.protectedMaxDepth.store(ClampInt(depth, 0, 8),
                                       std::memory_order_relaxed);
    g_settings.releaseMoveY.store(ClampInt(releaseY, 0, 64),
                                  std::memory_order_relaxed);
    g_settings.capturedRowMarginX.store(ClampInt(marginX, 0, 64),
                                        std::memory_order_relaxed);
    g_settings.capturedRowMarginY.store(ClampInt(marginY, 0, 64),
                                        std::memory_order_relaxed);
    g_settings.enableWheelScrollDuringDrag.store(
        Wh_GetIntSetting(L"enableWheelScrollDuringDrag") != 0,
        std::memory_order_relaxed);
}

}  // namespace

void Wh_ModSettingsChanged() {
    LoadSettings();
    Wh_Log(L"Settings reloaded: depth=%d releaseY=%d marginX=%d marginY=%d "
           L"wheelDuringDrag=%d",
           g_settings.protectedMaxDepth.load(std::memory_order_relaxed),
           g_settings.releaseMoveY.load(std::memory_order_relaxed),
           g_settings.capturedRowMarginX.load(std::memory_order_relaxed),
           g_settings.capturedRowMarginY.load(std::memory_order_relaxed),
           g_settings.enableWheelScrollDuringDrag.load(
               std::memory_order_relaxed)
               ? 1
               : 0);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    // Belt-and-suspenders for a failed/partial init where Wh_ModUninit may not
    // run. Windhawk unloads the module on disable, so a normal off/on toggle
    // already gets zero-initialised globals on the next load.
    g_uninitInProgress.store(false, std::memory_order_relaxed);
    g_helperStopping.store(false, std::memory_order_relaxed);
    g_dragLoopDepth.store(0, std::memory_order_relaxed);
    g_subclassOpsInFlight.store(0, std::memory_order_relaxed);

    LoadSettings();

    HMODULE ole32 = GetModuleHandleW(L"ole32.dll");
    if (!ole32) {
        ole32 = LoadLibraryW(L"ole32.dll");
    }
    if (!ole32) {
        Wh_Log(L"Failed to load ole32.dll");
        return FALSE;
    }
    void* doDragDrop =
        reinterpret_cast<void*>(GetProcAddress(ole32, "DoDragDrop"));
    if (!doDragDrop) {
        Wh_Log(L"Failed to locate DoDragDrop");
        return FALSE;
    }
    if (!Wh_SetFunctionHook(doDragDrop,
                            reinterpret_cast<void*>(DoDragDrop_Hook),
                            reinterpret_cast<void**>(&DoDragDrop_Original))) {
        Wh_Log(L"Failed to hook DoDragDrop");
        return FALSE;
    }

    g_winEventThreadReady =
        CreateEventW(nullptr, /*bManualReset=*/TRUE,
                     /*bInitialState=*/FALSE, nullptr);
    if (!g_winEventThreadReady) {
        Wh_Log(L"Failed to create WinEvent ready event (err %lu); "
               L"uninit may race the helper thread",
               GetLastError());
    }
    // Create the stop event BEFORE the helper thread so the helper can wait
    // on it from the moment it enters MsgWaitForMultipleObjectsEx. Manual
    // reset so SetEvent stays sticky -- the helper sees the signal even if
    // it observes it after a brief delay.
    g_helperStopEvent =
        CreateEventW(nullptr, /*bManualReset=*/TRUE,
                     /*bInitialState=*/FALSE, nullptr);
    if (!g_helperStopEvent) {
        Wh_Log(L"Failed to create WinEvent stop event (err %lu); "
               L"refusing to start helper thread because it would have "
               L"no signal path and could leak indefinitely on uninit",
               GetLastError());
        // Refuse to start the helper. Without the stop event the only way
        // to halt the helper's MsgWaitForMultipleObjectsEx wait is to
        // forcibly terminate the thread (which leaves Win32 internal locks
        // in an indeterminate state) or to leak it. EnumWindows below
        // still installs subclasses on already-open Explorer windows, so
        // the mod degrades gracefully -- the user just won't get new
        // navigation trees auto-subclassed for the rest of the session.
        if (g_winEventThreadReady) {
            CloseHandle(g_winEventThreadReady);
            g_winEventThreadReady = nullptr;
        }
        EnumWindows(EnumTopLevelExplorerWindowsProc, 0);
        return TRUE;
    }
    g_winEventThread = CreateThread(nullptr, 0, WinEventHookThread, nullptr,
                                     0, nullptr);
    if (!g_winEventThread) {
        Wh_Log(L"Failed to start WinEvent helper thread; "
               L"new trees will not be guarded");
        if (g_winEventThreadReady) {
            CloseHandle(g_winEventThreadReady);
            g_winEventThreadReady = nullptr;
        }
        if (g_helperStopEvent) {
            CloseHandle(g_helperStopEvent);
            g_helperStopEvent = nullptr;
        }
    } else if (g_winEventThreadReady) {
        // Block until the helper thread has finished SetWinEventHook (or
        // signalled failure). This guarantees subsequent SetEvent on
        // g_helperStopEvent will be observed by a fully-initialised helper.
        if (WaitForSingleObject(g_winEventThreadReady,
                                kWinEventThreadReadyTimeoutMs) ==
            WAIT_TIMEOUT) {
            Wh_Log(L"WinEvent helper thread did not signal ready within 5s; "
                   L"shutdown may rely on the helper exiting on its own");
        }
    }

    EnumWindows(EnumTopLevelExplorerWindowsProc, 0);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    // Set BOTH shutdown flags at the very top so every concurrent path
    // observes shutdown the moment teardown begins.
    //
    // * g_uninitInProgress gates TreeSubclassProc (self-removes on its next
    //   message) and TrySubclassTree (refuses new installs entirely).
    // * g_helperStopping gates WinEventProc (drops late callbacks) and is
    //   also re-checked at the top of TrySubclassTree as belt-and-suspenders.
    //
    // Setting them here closes the narrow window between Wh_ModUninit
    // entry and the point where the per-section teardown code would have
    // set them inline. A WinEvent dispatched out of the helper queue in
    // that window would otherwise reach TrySubclassTree and install a
    // subclass that the upcoming sweep has no record of.
    g_uninitInProgress.store(true, std::memory_order_release);
    g_helperStopping.store(true, std::memory_order_release);

    // Tear down any in-flight WH_MOUSE drag hooks BEFORE the rest of teardown.
    // g_dragMouseHook is thread_local, so we cannot reach it directly from
    // here; instead each install/uninstall registers itself in
    // g_activeDragHooks (see DoDragDrop_Hook). If we did not sweep here, a
    // drag in progress when the user disables the mod would leave the system
    // holding a hook handle into our about-to-be-unmapped DragMouseHookProc,
    // crashing on the next mouse event.
    //
    // UnhookWindowsHookEx on a thread-targeted hook installed from another
    // thread is not strictly documented to wait for an in-progress callback
    // to finish before returning. In practice we rely on Windhawk's hook
    // engine keeping this DLL's image mapped until in-flight hook callbacks
    // return -- the same platform-level guarantee DoDragDrop_Hook depends on
    // for its return frame. A bounded wait on a per-callback refcount here
    // would be observable but not actionable, because Wh_ModUninit's own
    // teardown budget is finite and waiting on a hook callback that is
    // itself blocked in SendMessageTimeoutW would just shift the failure.
    //
    // After this sweep the in-flight drag loses wheel-scroll-during-drag but
    // continues to function normally; its eventual DoDragDrop_Hook epilogue
    // will find the handle already removed from the set and the
    // UnhookWindowsHookEx call is harmlessly a no-op.
    std::vector<HHOOK> dragHooksToUnhook;
    {
        std::lock_guard<std::mutex> lock(g_dragHookMutex);
        dragHooksToUnhook.assign(g_activeDragHooks.begin(),
                                 g_activeDragHooks.end());
        g_activeDragHooks.clear();
    }
    for (HHOOK h : dragHooksToUnhook) {
        UnhookWindowsHookEx(h);
    }

    // Track whether the helper actually exited. If it leaks (10s timeout), it
    // may still be inside MsgWaitForMultipleObjectsEx on g_helperStopEvent,
    // and closing that handle would be UB. Default true so the no-helper case
    // and the fast-path (already-exited) case both close the event normally.
    bool helperExited = true;
    if (g_winEventThread) {
        // The helper thread owns g_winEventHook exclusively, including the
        // UnhookWinEvent call. Win32 documents that UnhookWinEvent must be
        // called on the thread that installed the hook; doing it here would
        // either silently fail or be undefined.
        //
        // Fast path: if SetWinEventHook failed during init, the helper
        // already returned and the thread object is signalled. Signalling
        // the stop event in that case is harmless but unnecessary -- skip it
        // and just close the handle.
        if (WaitForSingleObject(g_winEventThread, 0) == WAIT_OBJECT_0) {
            CloseHandle(g_winEventThread);
        } else {
            // g_helperStopping was already set at the top of Wh_ModUninit
            // so WinEventProc bails on any in-flight callback before we
            // even signal the stop event.

            // Signal the helper. SetEvent on a manual-reset event with a
            // valid handle does not fail in practice, which is why this
            // replaced the PostThreadMessage retry loop. The helper's
            // MsgWaitForMultipleObjectsEx returns WAIT_OBJECT_0 immediately
            // (priority over QS_ALLINPUT), it unhooks on its own thread,
            // and returns.
            if (g_helperStopEvent) {
                if (!SetEvent(g_helperStopEvent)) {
                    // Extremely unlikely; log once and fall back to the
                    // 10s wait so we at least observe whether the helper
                    // exits on its own.
                    Wh_Log(L"FATAL: SetEvent(g_helperStopEvent) failed "
                           L"(err %lu); helper may keep running",
                           GetLastError());
                }
            } else {
                Wh_Log(L"FATAL: stop event missing; helper has no signal "
                       L"path and may keep running");
            }

            if (WaitForSingleObject(g_winEventThread,
                                    kWinEventHelperJoinTimeoutMs) ==
                WAIT_TIMEOUT) {
                // The helper did not observe the event within 10s. The
                // only way this happens in practice is if a dispatched
                // callback is itself blocked (e.g. SetWindowSubclass
                // marshalling to a hung Explorer thread). Closing the
                // handle would let DLL unload race live code, so we leak
                // the handle instead. There is no safe sledgehammer:
                // TerminateThread would orphan whatever lock the helper
                // currently holds inside the OS.
                Wh_Log(L"FATAL: WinEvent helper thread did not exit within "
                       L"10s; leaking thread handle");
                helperExited = false;
            } else {
                CloseHandle(g_winEventThread);
            }
        }
        g_winEventThread = nullptr;
    }
    if (g_winEventThreadReady) {
        // The helper only touches this handle once near startup (SetEvent),
        // and by the time Wh_ModUninit runs the wait in Wh_ModInit has long
        // since observed the signal. Safe to close even if the helper later
        // leaked.
        CloseHandle(g_winEventThreadReady);
        g_winEventThreadReady = nullptr;
    }
    if (g_helperStopEvent && helperExited) {
        // Only close the stop event if the helper actually exited; otherwise
        // it may still be inside MsgWaitForMultipleObjectsEx on this handle.
        // The leaked HANDLE is the lesser evil compared to UB on a live wait.
        CloseHandle(g_helperStopEvent);
        g_helperStopEvent = nullptr;
    }

    // g_uninitInProgress and g_helperStopping were both set at the top of
    // this function, so any TrySubclassTree call that has not yet bumped
    // g_subclassOpsInFlight will see the gate and bail. But a call that
    // had already passed the gate before our stores landed could still be
    // mid-marshal inside SetWindowSubclassFromAnyThread. If we snapshot
    // now, that install can land on the target thread AFTER our Remove
    // (the two are independent SendMessage senders to one receiver, so
    // ordering is not guaranteed), leaving a live subclass pointer in an
    // about-to-be-unmapped image.
    //
    // Drain the in-flight counter with a short bounded wait. 100ms is
    // generous for a SendMessage round-trip on a healthy Explorer thread.
    // If we time out, the install is wedged and blocking longer makes the
    // user wait without improving safety -- the belt-and-suspenders
    // g_uninitInProgress check inside TreeSubclassProc still self-removes
    // any late landing on the target thread's next dispatched message.
    //
    // Wall-clock timing via GetTickCount64 rather than count-based
    // bookkeeping: Sleep on Windows rounds up to the system timer tick
    // (~15.6 ms by default), so a count-based loop with Sleep(2) would
    // block ~780 ms instead of 100 ms. Comparing actual elapsed ticks
    // keeps the bound honest regardless of timer resolution.
    {
        constexpr ULONGLONG kSubclassDrainTimeoutMs = 100;
        const ULONGLONG drainStart = GetTickCount64();
        while (g_subclassOpsInFlight.load(std::memory_order_acquire) > 0 &&
               (GetTickCount64() - drainStart) < kSubclassDrainTimeoutMs) {
            Sleep(5);
        }
        int remaining = g_subclassOpsInFlight.load(std::memory_order_acquire);
        if (remaining > 0) {
            ULONGLONG elapsed = GetTickCount64() - drainStart;
            Wh_Log(L"WARN: %d subclass install op(s) still in flight after "
                   L"~%llu ms; proceeding with sweep",
                   remaining, elapsed);
        }
    }

    // Snapshot the map keys so we can iterate without holding the mutex
    // while RemoveWindowSubclassFromAnyThread may block on other threads.
    // We process both Installed and Installing entries -- after the drain
    // above the Installing-state set should be empty in the common case,
    // but if any remain (drain timed out), RemoveWindowSubclassFromAnyThread
    // is a safe no-op against an unsubclassed window, and the late install
    // will be cleaned up by the TreeSubclassProc shutdown gate.
    std::vector<HWND> trees;
    {
        std::lock_guard<std::mutex> lock(g_subclassMutex);
        trees.reserve(g_subclassedTrees.size());
        for (const auto& kv : g_subclassedTrees) {
            trees.push_back(kv.first);
        }
    }
    // Remove each subclass directly via the utility that already does the
    // cross-thread marshalling. The previous WM_NULL-ping approach relied on
    // each tree's proc self-removing because g_uninitInProgress is set, but
    // if SendMessageTimeoutW timed out, the subclass would still be live
    // and the DLL could unload behind a callback pointer that still pointed
    // into our code. Calling the utility directly closes that gap: it owns
    // the marshalling, and we log only if it itself reports failure.
    //
    // The g_uninitInProgress check inside TreeSubclassProc remains as belt-
    // and-suspenders: if a message is dispatched on the owning thread
    // between this snapshot and the removal call landing, the proc bails
    // out of all drag logic and self-removes.
    for (HWND tree : trees) {
        if (!IsWindow(tree)) {
            continue;
        }
        // RemoveWindowSubclassFromAnyThread returns void in the real
        // Windhawk SDK -- it provides no failure indication. If marshalling
        // to the tree's owning thread fails internally (thread wedged), the
        // subclass proc still self-removes on its next dispatched message
        // via the g_uninitInProgress fallback.
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(tree,
                                                         TreeSubclassProc);
    }

    // Clear the map AFTER the remove loop. If Windhawk keeps the image
    // mapped across toggle-off / toggle-on without a host restart (the same
    // model the Wh_ModInit flag resets account for), leaving stale HWND
    // entries here would make every TrySubclassTree call on the next init
    // no-op via try_emplace, silently disabling the mod. Clearing here
    // gives the next init a clean slate.
    {
        std::lock_guard<std::mutex> lock(g_subclassMutex);
        g_subclassedTrees.clear();
    }

    // Intentionally do NOT call ClearProtectedHover() here. g_protectedHover
    // is thread_local, so this thread's slot is unrelated to any tree-owning
    // thread's slot, and the uninit thread's TLS is about to be torn down
    // with the DLL anyway. Generation invalidation at the next drag start
    // and the per-thread uninit cleanup in TreeSubclassProc handle the
    // tree-owning threads.
}
