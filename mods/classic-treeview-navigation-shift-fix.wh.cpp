// ==WindhawkMod==
// @id              classic-treeview-navigation-shift-fix
// @name            Classic Explorer Treeview Navigation Shift Fix
// @description     Keeps the classic folder pane of Classic Explorer Treeview from sliding a couple of pixels aside after navigation and from flickering while it is resized, and keeps it in a narrow window the way Windows 2000 did
// @name:ru         Исправление сдвига классического дерева папок
// @description:ru  Классическая панель папок мода Classic Explorer Treeview больше не съезжает на пару пикселей после перехода в другую папку, не мерцает при растягивании и, как в Windows 2000, не уступает место списку файлов в узком окне
// @version         1.3
// @author          appEW
// @github          https://github.com/appEW
// @include         explorer.exe
// @compilerOptions -lcomctl32
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Classic Explorer Treeview Navigation Shift Fix

> **Tested only on Windows 11 24H2 (build 26100).** It has not been tried on any other version of Windows and may not work there.

A companion fix for the
[Classic Explorer Treeview](https://windhawk.net/mods/classic-explorer-treeview)
mod. It fixes three things about the folder pane that mod draws:

* with it enabled, the folder pane sits flush against the left edge of the
  window in a freshly opened Explorer window, but as soon as you navigate into
  any folder or drive, the whole pane - the Folders band and the tree inside
  it - slides two pixels to the side and stays there until the window is
  resized;
* while the pane is being resized with the splitter, the Folders band, the
  edges of the pane and the scroll bars of the tree and of the file list
  flicker;
* in a narrow window the pane shrinks and then disappears while the file list
  keeps its room - the other way round from Windows 2000, which kept the tree.

## Why the pane shifts

The folder pane lives in a `CtrlNotifySink` window that hosts the
`NamespaceTreeControl`. Explorer's DirectUI layout puts that host two pixels
away from the window edge; Classic Explorer Treeview moves it back to the edge,
but it does so from its `WM_SIZE` handler.

On navigation DirectUI re-runs its layout and moves the host back to its own
position **without changing its size**, so no `WM_SIZE` is sent and the
correction is never re-applied. Resizing the window does send `WM_SIZE`, which
is why resizing puts the pane back where it belongs.

Measured on Windows 11 24H2, host window rectangle inside its parent:

| state               | x | width |
|---------------------|---|-------|
| freshly opened      | 0 | 316   |
| after window resize | 0 | 316   |
| after navigation    | 2 | 316   |

## Why the pane flickers while it is resized

Every step of a splitter drag goes through the same tug of war: DirectUI puts
the host at x=2 with the new width, then Classic Explorer Treeview pulls it back
to x=0 and widens it by three pixels. Sampled on a real drag:

| step     | x | width |
|----------|---|-------|
| DirectUI | 2 | 421   |
| fixed up | 0 | 424   |

So both edges of the pane jump back and forth on every step. On top of that,
one step is not one repaint. DirectUI first paints the window at its new
layout - the splitter at its new place, and its own white background where the
pane is about to grow into - and only then resizes the pane; Classic Explorer
Treeview then widens it once more and resizes the tree inside. Along the way
Windows paints what it paints synchronously: the pane's background (white, the
class brush of the host and the `NamespaceTreeControl`) and the tree's frame,
with its scroll bar at the new place, while the inside of the tree - still
showing the old scroll bar - only repaints at the very end. A screen refresh
landing in that sequence shows a white strip at the right edge of the pane, a
Folders band without its close button, or two scroll bars side by side.

## Why the pane gives way in a narrow window

Windows 2000 kept the folder tree when an Explorer window got narrow and let
the file list go. Explorer on Windows 11 does the opposite: the splitter of the
pane, a `CDUISizerElement` in `shell32`, bounds the pane in
`_ComputeBoundedSize` to the width of the window minus a `ReservedSpace` kept
for the list, so the tree shrinks first, and once what is left for it drops
below its minimum it is hidden altogether.

## What the mod does

It subclasses the same host window and keeps it where it already is:

* in `WM_WINDOWPOSCHANGING` a `SetWindowPos` that moves the host horizontally
  without changing its size gets its x replaced by the x the host already has,
  so the pane simply stays where it is. `WM_WINDOWPOSCHANGED` is checked as
  well, to also catch moves made with `SWP_NOSENDCHANGING`;
* a `SetWindowPos` that resizes the host *and* moves it back to the right is
  applied at the x the host already has, keeping the right edge DirectUI asked
  for. Classic Explorer Treeview then only has the width to adjust, which ends
  in exactly the rectangle it would have produced anyway - just without the
  intermediate one;
* while a step of a splitter drag is handled (the `WM_MOUSEMOVE` of the
  `DirectUIHWND` that has captured the mouse), the host, the
  `NamespaceTreeControl` and the tree view swallow `WM_PAINT`, `WM_NCPAINT` and
  `WM_ERASEBKGND`, and DirectUI's own `WM_PAINT` is held back. The step then
  ends with the whole pane painted at once - with `WS_EX_COMPOSITED` set on the
  host just for that, so it is drawn off screen and shown in one piece - then
  DirectUI's own paint, which is quick and puts the splitter at its new place,
  and only then the windows next to the pane, the file list taking longest.
  Hiding the pane with `WM_SETREDRAW` instead does not work: DirectUI notices
  and shows it again in the middle of the step; neither does compositing the
  whole `DirectUIHWND`, which leaves the splitter black;
* `SetScrollInfo` and `SetScrollPos` asked to redraw draw the scroll bar
  straight onto the screen, past any held `WM_NCPAINT`: during a step they
  are told not to, for every window in the `DirectUIHWND`, and the repaint
  after the step draws the scroll bars with everything else. Otherwise the
  tree's thumb - whose length changes whenever its horizontal scroll bar comes
  or goes - shows up next to the old one;
* the other windows next to the pane - the file list, and any other pane - go
  through the same: during such a step they are moved without carrying their
  old picture along (`SWP_NOCOPYBITS`) and paint nothing, and they are
  repainted right after it. Otherwise the list's scroll bar shows up twice as
  well. (No `WS_EX_COMPOSITED` there: composited, the list leaves its scroll
  bar unpainted.)
* the host and the `NamespaceTreeControl` erase in the 3D face colour instead of
  white;
* when the window is resized, the splitter of the pane reports no reserved
  space (`CDUISizerElement::GetReservedSpace` returns 0 while
  `CDUISizerElement::_Render` runs for it), so the pane keeps its width and the
  file list gets what is left, down to nothing. Dragging the splitter is bounded
  as before - that goes through `CDUISizerElement::OnInput` - so the pane still
  cannot be dragged over the whole window.

Frames showing anything but the finished picture at the edge of the pane -
two scroll bars, a white or missing splitter, the pane two pixels off - out of
150 filmed on the same real mouse drag of the splitter, two runs each:

| variant           | 120 Hz monitor | 60 Hz monitor |
|-------------------|----------------|---------------|
| without this mod  | 39, 39         |               |
| version 1.2       | 27, 23         |               |
| version 1.3       | 0, 1           | 0, 0          |

The file list's scroll bar, filmed the same way: doubled or cut in 29 and 9
frames with version 1.2, in none with 1.3.

Anything that resizes the pane is otherwise passed through untouched, which
matters: Classic Explorer Treeview compensates the width of the pane by the
offset it finds it at (`rect.right + xOffset + 1`). Forcing the pane to the
window edge before that handler runs, without handing it the width that goes
with it, takes those pixels away and the pane ends up too narrow, with a gap on
the right against the splitter.

The resize, painting and narrow-window handling only apply to a pane that has
been seen being pulled to the left, or that carries the Folders band above its
tree - both are what Classic Explorer Treeview does; the band is what lets a
window that was already open when the mod was loaded be recognised before it is
next resized. The mod has no effect at all when Classic Explorer Treeview is not
installed. The DirectUI id of the splitter is looked up every time rather than
kept: it is an atom that DirectUI deletes when the last element carrying it
goes, and it can come back under another number.

Existing Explorer windows are picked up when the mod is enabled, so there is no
need to reopen them.

---

## По-русски

Дополнение к моду
[Classic Explorer Treeview](https://windhawk.net/mods/classic-explorer-treeview),
без него этот мод ничего не делает. Исправляет три недостатка панели папок,
которую рисует Classic Explorer Treeview:

* в только что открытом окне панель прилегает к левому краю, но после перехода
  в любую папку или на диск вся панель - полоса «Папки» и дерево в ней -
  сдвигается на два пикселя в сторону и остаётся там до изменения размера окна;
* при перетаскивании разделителя панели полоса «Папки», края панели и полосы
  прокрутки дерева и списка файлов мерцают;
* в узком окне панель сжимается, а затем пропадает, тогда как список файлов
  сохраняет своё место, - наоборот по сравнению с Windows 2000, где оставалось
  дерево.

Мод удерживает окно панели на месте, когда Проводник пытается сдвинуть его без
изменения размера, перерисовывает каждый шаг перетаскивания разделителя
целиком за один раз, а в узком окне отдаёт место дереву, а не списку файлов.
Уже открытые окна Проводника подхватываются сразу, открывать их заново не нужно.

> **Проверено только на Windows 11 24H2 (сборка 26100).** На других версиях Windows мод не проверялся и может не работать.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct SinkGeometry {
    int x;
    int y;
    int cx;
    int cy;
};

struct SinkState {
    // The last position the folder pane host was seen at, so that a move that
    // arrives without a WM_WINDOWPOSCHANGING of its own can still be undone.
    SinkGeometry geometry;
    // The NamespaceTreeControl inside it, subclassed for its erase and to hold
    // its painting during a splitter drag step.
    HWND hTree;
    // The DirectUIHWND it sits in, subclassed to order the painting of a
    // splitter drag step.
    HWND hDui;
    // The tree view inside the NamespaceTreeControl, subclassed for the same.
    HWND hTreeView;
    // Set once something has pulled the host to the left of where DirectUI put
    // it, i.e. once Classic Explorer Treeview is known to be managing it.
    bool pulledLeft;
};

std::mutex g_sinksMutex;
std::unordered_map<HWND, SinkState> g_sinks;

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;

bool IsWindowClass(HWND hWnd, PCWSTR className) {
    WCHAR buffer[64];
    if (!GetClassNameW(hWnd, buffer, ARRAYSIZE(buffer))) {
        return false;
    }

    return wcscmp(buffer, className) == 0;
}

bool GetSinkGeometry(HWND hSink, SinkGeometry* geometry) {
    HWND hParent = GetParent(hSink);
    if (!hParent) {
        return false;
    }

    RECT rect;
    if (!GetWindowRect(hSink, &rect)) {
        return false;
    }

    MapWindowPoints(nullptr, hParent, (POINT*)&rect, 2);

    geometry->x = rect.left;
    geometry->y = rect.top;
    geometry->cx = rect.right - rect.left;
    geometry->cy = rect.bottom - rect.top;
    return true;
}

bool LookupSink(HWND hSink, SinkState* state) {
    std::lock_guard<std::mutex> guard(g_sinksMutex);

    auto it = g_sinks.find(hSink);
    if (it == g_sinks.end()) {
        return false;
    }

    *state = it->second;
    return true;
}

bool IsSinkPulledLeft(HWND hSink) {
    SinkState state;
    return LookupSink(hSink, &state) && state.pulledLeft;
}

// Records where the host is now, and notes whether it got there by being pulled
// to the left of where it was.
void RememberSink(HWND hSink, const SinkGeometry& geometry) {
    std::lock_guard<std::mutex> guard(g_sinksMutex);

    auto it = g_sinks.find(hSink);
    if (it == g_sinks.end()) {
        return;
    }

    if (geometry.x < it->second.geometry.x) {
        it->second.pulledLeft = true;
    }

    it->second.geometry = geometry;
}

// The DirectUIHWND the folder pane host sits in, while it handles a step of a
// splitter drag, the host whose painting is held back meanwhile, and what
// happened to it.
thread_local HWND g_busyDui;
thread_local HWND g_heldSink;
thread_local bool g_sinkPaintHeld;
thread_local bool g_sinkResized;
thread_local bool g_duiPaintDeferred;
thread_local bool g_neighbourMoved;

// The other windows in such a DirectUIHWND - the file list, any other pane -
// subclassed to be moved without their old picture during a step.
std::mutex g_neighboursMutex;
std::unordered_set<HWND> g_neighbours;

// The file list goes through the same as the folder pane: it is first moved,
// then resized, and Windows paints its frame - the scroll bar - at each of
// those places right away, while its old picture is carried along with the
// move. Either way an old scroll bar shows up next to the real one until the
// list repaints. So during a step these windows (and everything inside them)
// neither carry their picture along nor paint; whatever was on screen stays in
// place - the scroll bar included, as the right edge of the list does not
// move - and they are repainted as soon as the step is over.
LRESULT CALLBACK NeighbourSubclassProc(HWND hWnd,
                                       UINT uMsg,
                                       WPARAM wParam,
                                       LPARAM lParam,
                                       DWORD_PTR dwRefData) {
    bool busy = g_busyDui && g_busyDui == (HWND)dwRefData;
    switch (uMsg) {
        case WM_WINDOWPOSCHANGING: {
            WINDOWPOS* pos = (WINDOWPOS*)lParam;
            if (busy && pos &&
                (pos->flags & (SWP_NOMOVE | SWP_NOSIZE)) !=
                    (SWP_NOMOVE | SWP_NOSIZE)) {
                pos->flags |= SWP_NOCOPYBITS;
                g_neighbourMoved = true;
            }
            break;
        }

        case WM_PAINT:
        case WM_NCPAINT:
        case WM_ERASEBKGND:
            if (busy) {
                // Left invalid, see DuiSubclassProc.
                g_neighbourMoved = true;
                return 0;
            }
            break;

        case WM_NCDESTROY: {
            std::lock_guard<std::mutex> guard(g_neighboursMutex);
            g_neighbours.erase(hWnd);
            break;
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

void SubclassNeighbour(HWND hDui, HWND hChild) {
    {
        std::lock_guard<std::mutex> guard(g_neighboursMutex);
        if (!g_neighbours.insert(hChild).second) {
            return;
        }
    }

    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(
            hChild, NeighbourSubclassProc, (DWORD_PTR)hDui)) {
        std::lock_guard<std::mutex> guard(g_neighboursMutex);
        g_neighbours.erase(hChild);
    }
}

struct NeighbourEnum {
    HWND hDui;
};

BOOL CALLBACK SubclassNeighbourProc(HWND hWnd, LPARAM lParam) {
    SubclassNeighbour(((NeighbourEnum*)lParam)->hDui, hWnd);
    return TRUE;
}

// Windows come and go with navigation, so this runs at every step.
void SubclassNeighbours(HWND hDui, HWND hSink) {
    NeighbourEnum data{hDui};
    for (HWND hChild = GetWindow(hDui, GW_CHILD); hChild;
         hChild = GetWindow(hChild, GW_HWNDNEXT)) {
        if (hChild == hSink) {
            continue;
        }

        SubclassNeighbour(hDui, hChild);
        EnumChildWindows(hChild, SubclassNeighbourProc, (LPARAM)&data);
    }
}

// Swallows the painting of the folder pane windows during a splitter drag
// step, see DuiSubclassProc. Whatever is swallowed stays invalid and gets
// painted once the step is over.
bool HoldPaint(HWND hSink, UINT uMsg, LRESULT* result) {
    if (!g_heldSink || g_heldSink != hSink) {
        return false;
    }

    switch (uMsg) {
        case WM_PAINT:
        case WM_NCPAINT:
        case WM_ERASEBKGND:
            g_sinkPaintHeld = true;
            *result = 0;
            return true;
    }

    return false;
}

// Both folder pane windows have a white class background, while everything
// Classic Explorer Treeview paints around the tree is the 3D face colour.
LRESULT EraseWithFaceColor(HWND hWnd, HDC hdc) {
    RECT rect;
    GetClientRect(hWnd, &rect);
    FillRect(hdc, &rect, GetSysColorBrush(COLOR_BTNFACE));
    return 1;
}

LRESULT CALLBACK TreeSubclassProc(HWND hWnd,
                                  UINT uMsg,
                                  WPARAM wParam,
                                  LPARAM lParam,
                                  DWORD_PTR dwRefData) {
    LRESULT held;
    if (HoldPaint(GetParent(hWnd), uMsg, &held)) {
        return held;
    }

    switch (uMsg) {
        case WM_ERASEBKGND:
            // Classic Explorer Treeview paints the control grey in WM_PAINT,
            // erasing it white first only shows up as a flash.
            if (IsSinkPulledLeft(GetParent(hWnd))) {
                return EraseWithFaceColor(hWnd, (HDC)wParam);
            }
            break;

        case WM_NCDESTROY: {
            HWND hSink = GetParent(hWnd);
            std::lock_guard<std::mutex> guard(g_sinksMutex);
            auto it = g_sinks.find(hSink);
            if (it != g_sinks.end() && it->second.hTree == hWnd) {
                it->second.hTree = nullptr;
            }
            break;
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK TreeViewSubclassProc(HWND hWnd,
                                      UINT uMsg,
                                      WPARAM wParam,
                                      LPARAM lParam,
                                      DWORD_PTR dwRefData) {
    LRESULT held;
    if (HoldPaint(GetParent(GetParent(hWnd)), uMsg, &held)) {
        return held;
    }

    if (uMsg == WM_NCDESTROY) {
        HWND hSink = GetParent(GetParent(hWnd));
        std::lock_guard<std::mutex> guard(g_sinksMutex);
        auto it = g_sinks.find(hSink);
        if (it != g_sinks.end() && it->second.hTreeView == hWnd) {
            it->second.hTreeView = nullptr;
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// The tree view is created after the NamespaceTreeControl, so it is picked up
// the first time it is needed.
void SubclassTreeView(HWND hSink) {
    HWND hTreeView = nullptr;
    {
        std::lock_guard<std::mutex> guard(g_sinksMutex);
        auto it = g_sinks.find(hSink);
        if (it == g_sinks.end() || it->second.hTreeView || !it->second.hTree) {
            return;
        }

        hTreeView = FindWindowExW(it->second.hTree, nullptr, L"SysTreeView32",
                                  nullptr);
        if (!hTreeView) {
            return;
        }

        it->second.hTreeView = hTreeView;
    }

    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(
            hTreeView, TreeViewSubclassProc, 0)) {
        std::lock_guard<std::mutex> guard(g_sinksMutex);
        auto it = g_sinks.find(hSink);
        if (it != g_sinks.end() && it->second.hTreeView == hTreeView) {
            it->second.hTreeView = nullptr;
        }
    }
}

// Whether this DirectUIHWND has a folder pane managed by Classic Explorer
// Treeview in it, showing or not.
// Classic Explorer Treeview draws its Folders band above the tree, so the tree
// view no longer starts at the top of the NamespaceTreeControl. Unlike
// pulledLeft, this can be told at any moment - also in a window that was open
// before the mod was loaded and has not been resized since.
bool HasFoldersBand(HWND hTree) {
    HWND hTreeView =
        hTree ? FindWindowExW(hTree, nullptr, L"SysTreeView32", nullptr)
              : nullptr;
    RECT treeRect;
    RECT treeViewRect;
    if (!hTreeView || !GetWindowRect(hTree, &treeRect) ||
        !GetWindowRect(hTreeView, &treeViewRect)) {
        return false;
    }

    return treeViewRect.top - treeRect.top >= 10;
}

bool HasManagedSink(HWND hDui) {
    std::vector<HWND> trees;
    {
        std::lock_guard<std::mutex> guard(g_sinksMutex);
        for (const auto& entry : g_sinks) {
            if (entry.second.hDui != hDui) {
                continue;
            }
            if (entry.second.pulledLeft) {
                return true;
            }
            trees.push_back(entry.second.hTree);
        }
    }

    for (HWND hTree : trees) {
        if (HasFoldersBand(hTree)) {
            return true;
        }
    }

    return false;
}

// The folder pane host managed by Classic Explorer Treeview in this
// DirectUIHWND, if it is showing.
HWND FindPulledLeftSink(HWND hDui) {
    std::lock_guard<std::mutex> guard(g_sinksMutex);
    for (const auto& entry : g_sinks) {
        if (entry.second.hDui == hDui && entry.second.pulledLeft &&
            (GetWindowLongPtrW(entry.first, GWL_STYLE) & WS_VISIBLE)) {
            return entry.first;
        }
    }

    return nullptr;
}

// A splitter drag step goes: DirectUI lays the window out and paints it
// straight away - the splitter at its new place, and its own white background
// where the pane is about to grow into - and only then resizes the pane. The
// pane in turn is finished in several steps (Classic Explorer Treeview widens it
// once more and resizes the tree), and along the way Windows paints the parts
// it paints synchronously: the background of the widened pane, and the tree's
// frame with its scroll bar at the new place, while the inside of the tree,
// with the old scroll bar still in it, only repaints at the end. A screen
// refresh landing anywhere in that sequence shows a white strip, a band without
// its close button, or a doubled scroll bar.
//
// So for the duration of the step nothing of the pane is painted and
// DirectUI's own paint is held back. Once the step is over, the pane is
// painted in one go, then DirectUI. Hiding the pane with WM_SETREDRAW instead
// does not work: DirectUI notices and shows it again in the middle of the
// step.
LRESULT CALLBACK DuiSubclassProc(HWND hWnd,
                                 UINT uMsg,
                                 WPARAM wParam,
                                 LPARAM lParam,
                                 DWORD_PTR dwRefData) {
    switch (uMsg) {
        case WM_PAINT:
            if (g_busyDui == hWnd) {
                // Left invalid, so it is painted after all, see below.
                g_duiPaintDeferred = true;
                return 0;
            }
            break;

        case WM_MOUSEMOVE: {
            // The splitter captures the mouse while it is dragged.
            if (!(wParam & MK_LBUTTON) || GetCapture() != hWnd || g_busyDui) {
                break;
            }

            HWND hSink = FindPulledLeftSink(hWnd);
            if (hSink) {
                SubclassTreeView(hSink);
                SubclassNeighbours(hWnd, hSink);
            }

            g_busyDui = hWnd;
            g_heldSink = hSink;
            g_sinkPaintHeld = false;
            g_sinkResized = false;
            g_duiPaintDeferred = false;
            g_neighbourMoved = false;
            LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            g_busyDui = nullptr;
            g_heldSink = nullptr;

            if (hSink && (g_sinkPaintHeld || g_sinkResized) &&
                IsWindow(hSink)) {
                // Painted piece by piece, the tree's frame - the scroll bar at
                // its new place - would land before its inside, which still
                // shows the old one. Composited, the whole pane is drawn off
                // screen and shown at once.
                LONG_PTR exStyle = GetWindowLongPtrW(hSink, GWL_EXSTYLE);
                if (!(exStyle & WS_EX_COMPOSITED)) {
                    SetWindowLongPtrW(hSink, GWL_EXSTYLE,
                                      exStyle | WS_EX_COMPOSITED);
                }

                RedrawWindow(hSink, nullptr, nullptr,
                             RDW_INVALIDATE | RDW_ERASE | RDW_FRAME |
                                 RDW_ALLCHILDREN | RDW_UPDATENOW);

                if (!(exStyle & WS_EX_COMPOSITED)) {
                    SetWindowLongPtrW(hSink, GWL_EXSTYLE, exStyle);
                }
            }

            // DirectUI's own paint - the splitter above all - right after the
            // pane, before the windows next to it: until it is done, the
            // splitter's new place still shows what was there before, and
            // repainting the file list takes a while.
            if (g_duiPaintDeferred) {
                g_duiPaintDeferred = false;
                UpdateWindow(hWnd);
            }

            if (hSink && g_neighbourMoved) {
                g_neighbourMoved = false;
                for (HWND hChild = GetWindow(hWnd, GW_CHILD); hChild;
                     hChild = GetWindow(hChild, GW_HWNDNEXT)) {
                    if (hChild != hSink) {
                        RedrawWindow(hChild, nullptr, nullptr,
                                     RDW_INVALIDATE | RDW_ERASE | RDW_FRAME |
                                         RDW_ALLCHILDREN | RDW_UPDATENOW);
                    }
                }
            }

            return result;
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Scroll bars are drawn straight onto the screen by SetScrollInfo and
// SetScrollPos when asked to redraw, without a WM_NCPAINT to hold back. The
// tree and the file list update theirs as they are resized, so during a
// splitter drag step the scroll bar at its new place showed up next to the old
// picture of the window, which still has the old one in it. During a step they
// are updated without drawing; the repaint after the step draws them.
using SetScrollInfo_t = decltype(&SetScrollInfo);
SetScrollInfo_t SetScrollInfo_Original;
using SetScrollPos_t = decltype(&SetScrollPos);
SetScrollPos_t SetScrollPos_Original;

bool IsHeldDuringStep(HWND hWnd) {
    return g_busyDui && hWnd && IsChild(g_busyDui, hWnd);
}

int WINAPI SetScrollInfo_Hook(HWND hWnd,
                              int nBar,
                              LPCSCROLLINFO lpsi,
                              BOOL redraw) {
    if (redraw && IsHeldDuringStep(hWnd)) {
        redraw = FALSE;
        g_sinkPaintHeld = true;
        g_neighbourMoved = true;
    }

    return SetScrollInfo_Original(hWnd, nBar, lpsi, redraw);
}

int WINAPI SetScrollPos_Hook(HWND hWnd, int nBar, int nPos, BOOL redraw) {
    if (redraw && IsHeldDuringStep(hWnd)) {
        redraw = FALSE;
        g_sinkPaintHeld = true;
        g_neighbourMoved = true;
    }

    return SetScrollPos_Original(hWnd, nBar, nPos, redraw);
}

LRESULT CALLBACK SinkSubclassProc(HWND hWnd,
                                  UINT uMsg,
                                  WPARAM wParam,
                                  LPARAM lParam,
                                  DWORD_PTR dwRefData) {
    LRESULT held;
    if (HoldPaint(hWnd, uMsg, &held)) {
        return held;
    }

    switch (uMsg) {
        case WM_WINDOWPOSCHANGING: {
            WINDOWPOS* pos = (WINDOWPOS*)lParam;
            SinkGeometry current;
            if (pos && !(pos->flags & SWP_NOMOVE) &&
                GetSinkGeometry(hWnd, &current) && pos->x != current.x) {
                bool resizing = !(pos->flags & SWP_NOSIZE) &&
                                (pos->cx != current.cx || pos->cy != current.cy);
                if (!resizing) {
                    // The regular case: a move-only layout pass is turned into
                    // a no-op before it happens.
                    pos->x = current.x;
                } else if (pos->x > current.x && IsSinkPulledLeft(hWnd)) {
                    // A resize that also moves the pane back right, e.g. every
                    // step of a splitter drag. Keep the left edge and the right
                    // edge that was asked for, Classic Explorer Treeview's
                    // WM_SIZE then only has the width left to adjust.
                    pos->cx += pos->x - current.x;
                    pos->x = current.x;
                }
            }
            break;
        }

        case WM_WINDOWPOSCHANGED: {
            LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);

            // Resized in the middle of a splitter drag step: have the pane
            // painted as soon as the step is done, see DuiSubclassProc.
            WINDOWPOS* pos = (WINDOWPOS*)lParam;
            if (g_heldSink == hWnd && pos && !(pos->flags & SWP_NOSIZE)) {
                g_sinkResized = true;
            }

            // A move made with SWP_NOSENDCHANGING never reaches the handler
            // above, so compare against the last known geometry as well.
            SinkGeometry current;
            SinkState last;
            if (GetSinkGeometry(hWnd, &current)) {
                if (LookupSink(hWnd, &last) &&
                    current.cx == last.geometry.cx &&
                    current.cy == last.geometry.cy &&
                    current.x != last.geometry.x) {
                    SetWindowPos(hWnd, nullptr, last.geometry.x, current.y, 0,
                                 0,
                                 SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE |
                                     SWP_NOOWNERZORDER | SWP_NOSENDCHANGING);
                } else {
                    RememberSink(hWnd, current);
                }
            }

            return result;
        }

        case WM_ERASEBKGND:
            // Only ever visible as the strip a growing pane uncovers before the
            // NamespaceTreeControl inside it catches up.
            if (IsSinkPulledLeft(hWnd)) {
                return EraseWithFaceColor(hWnd, (HDC)wParam);
            }
            break;

        case WM_NCDESTROY: {
            LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);

            HWND hDui = nullptr;
            {
                std::lock_guard<std::mutex> guard(g_sinksMutex);
                auto it = g_sinks.find(hWnd);
                if (it != g_sinks.end()) {
                    hDui = it->second.hDui;
                    g_sinks.erase(it);
                }
            }

            // The DirectUIHWND usually goes down together with the host, but
            // if it outlives it, it must not keep a subclass nobody removes.
            if (hDui && IsWindow(hDui)) {
                WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                    hDui, DuiSubclassProc);
            }

            return result;
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

void SubclassTree(HWND hSink, HWND hTree) {
    {
        std::lock_guard<std::mutex> guard(g_sinksMutex);
        auto it = g_sinks.find(hSink);
        if (it == g_sinks.end() || it->second.hTree == hTree) {
            return;
        }

        it->second.hTree = hTree;
    }

    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(hTree, TreeSubclassProc,
                                                       0)) {
        Wh_Log(L"Failed to subclass folder pane %p", hTree);
        std::lock_guard<std::mutex> guard(g_sinksMutex);
        auto it = g_sinks.find(hSink);
        if (it != g_sinks.end() && it->second.hTree == hTree) {
            it->second.hTree = nullptr;
        }
    }
}

void SubclassSink(HWND hSink, HWND hTree) {
    SinkState state;
    if (!LookupSink(hSink, &state)) {
        if (!GetSinkGeometry(hSink, &state.geometry)) {
            return;
        }

        state.hTree = nullptr;
        state.hDui = GetParent(hSink);
        state.hTreeView = nullptr;
        state.pulledLeft = false;

        {
            std::lock_guard<std::mutex> guard(g_sinksMutex);
            g_sinks[hSink] = state;
        }

        // Not called with the lock held: subclassing from another thread sends
        // a message to the window's thread.
        if (!WindhawkUtils::SetWindowSubclassFromAnyThread(
                hSink, SinkSubclassProc, 0)) {
            Wh_Log(L"Failed to subclass folder pane host %p", hSink);
            std::lock_guard<std::mutex> guard(g_sinksMutex);
            g_sinks.erase(hSink);
            return;
        }

        if (!WindhawkUtils::SetWindowSubclassFromAnyThread(
                state.hDui, DuiSubclassProc, 0)) {
            Wh_Log(L"Failed to subclass %p", state.hDui);
            std::lock_guard<std::mutex> guard(g_sinksMutex);
            auto it = g_sinks.find(hSink);
            if (it != g_sinks.end()) {
                it->second.hDui = nullptr;
            }
        }

        Wh_Log(L"Watching folder pane host %p at x=%d", hSink,
               state.geometry.x);
    }

    SubclassTree(hSink, hTree);
}

HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,
                                 LPCWSTR lpClassName,
                                 LPCWSTR lpWindowName,
                                 DWORD dwStyle,
                                 int X,
                                 int Y,
                                 int nWidth,
                                 int nHeight,
                                 HWND hWndParent,
                                 HMENU hMenu,
                                 HINSTANCE hInstance,
                                 LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);

    // The folder pane of a new Explorer window or tab. The class name is read
    // back from the window instead of lpClassName, which may be an atom.
    if (hWnd && hWndParent && IsWindowClass(hWnd, L"NamespaceTreeControl") &&
        IsWindowClass(hWndParent, L"CtrlNotifySink")) {
        SubclassSink(hWndParent, hWnd);
    }

    return hWnd;
}

BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM lParam) {
    if (IsWindowClass(hWnd, L"NamespaceTreeControl")) {
        HWND hParent = GetParent(hWnd);
        if (hParent && IsWindowClass(hParent, L"CtrlNotifySink")) {
            ((std::vector<HWND>*)lParam)->push_back(hWnd);
        }
    }

    return TRUE;
}

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam) {
    DWORD processId = 0;
    GetWindowThreadProcessId(hWnd, &processId);
    if (processId == GetCurrentProcessId()) {
        EnumChildWindows(hWnd, EnumChildProc, lParam);
    }

    return TRUE;
}

// Picks up the Explorer windows that are already open, so that enabling the mod
// takes effect without reopening them.
void SubclassExistingSinks() {
    std::vector<HWND> trees;
    EnumWindows(EnumWindowsProc, (LPARAM)&trees);

    for (HWND hTree : trees) {
        HWND hSink = GetParent(hTree);
        SubclassSink(hSink, hTree);

        // Already pulled to the window edge by Classic Explorer Treeview, which
        // the mod only notices when it happens: take it as seen, or the first
        // resize of a window that was open before the mod was loaded goes by
        // without the pane being held (its width, its painting).
        SinkGeometry geometry;
        if (HasFoldersBand(hTree) && GetSinkGeometry(hSink, &geometry) &&
            geometry.x == 0) {
            std::lock_guard<std::mutex> guard(g_sinksMutex);
            auto it = g_sinks.find(hSink);
            if (it != g_sinks.end()) {
                it->second.pulledLeft = true;
            }
        }
    }
}

// -----------------------------------------------------------------------------
// The pane in a narrow window
// -----------------------------------------------------------------------------

// dui70.dll
using Element_GetID_t = ATOM(__cdecl*)(void* element);
using Element_GetRoot_t = void*(__cdecl*)(void* element);
using HWNDElement_GetHWND_t = HWND(__cdecl*)(void* hwndElement);
using StrToID_t = ATOM(WINAPI*)(PCWSTR);
Element_GetID_t Element_GetID;
Element_GetRoot_t Element_GetRoot;
HWNDElement_GetHWND_t HWNDElement_GetHWND;
StrToID_t StrToID;

// shell32.dll
using CDUISizerElement_Render_t = HRESULT(__cdecl*)(void* pThis);
CDUISizerElement_Render_t CDUISizerElement_Render_Original;
using CDUISizerElement_GetReservedSpace_t = int(__cdecl*)(void* pThis);
CDUISizerElement_GetReservedSpace_t CDUISizerElement_GetReservedSpace_Original;

// Set while a sizer lays its target out, as opposed to being dragged.
thread_local int g_sizerRenderDepth;

HRESULT __cdecl CDUISizerElement_Render_Hook(void* pThis) {
    g_sizerRenderDepth++;
    HRESULT result = CDUISizerElement_Render_Original(pThis);
    g_sizerRenderDepth--;
    return result;
}

// The splitter of the folder pane, in a window whose pane Classic Explorer
// Treeview manages.
bool IsManagedPaneSizer(void* sizer) {
    // Looked up every time, never kept: StrToID is FindAtomW, and DirectUI
    // adds the atom while elements carry the id and deletes it when the last
    // of them goes - there is no such id before the first Explorer window of
    // the process has its splitter, and it may come back under another number
    // after all of them have been closed.
    ATOM paneSizerAtom = StrToID(L"PageSpaceControlSizer");
    if (!paneSizerAtom || Element_GetID(sizer) != paneSizerAtom) {
        return false;
    }

    void* root = Element_GetRoot(sizer);
    if (!root) {
        return false;
    }

    HWND hDui = HWNDElement_GetHWND(root);
    return hDui && HasManagedSink(hDui);
}

// The width kept for the file list, which the pane is bounded by. Reported as
// none while the window is being laid out, so the pane keeps its width and the
// list gives way, as in Windows 2000; still reported while the splitter is
// dragged, which it keeps from covering the whole window.
int __cdecl CDUISizerElement_GetReservedSpace_Hook(void* pThis) {
    if (g_sizerRenderDepth > 0 && IsManagedPaneSizer(pThis)) {
        return 0;
    }

    return CDUISizerElement_GetReservedSpace_Original(pThis);
}

bool HookPaneSizer() {
    HMODULE dui70 = LoadLibraryW(L"dui70.dll");
    HMODULE shell32 = LoadLibraryW(L"shell32.dll");
    if (!dui70 || !shell32) {
        return false;
    }

    Element_GetID = (Element_GetID_t)GetProcAddress(
        dui70, "?GetID@Element@DirectUI@@QEAAGXZ");
    Element_GetRoot = (Element_GetRoot_t)GetProcAddress(
        dui70, "?GetRoot@Element@DirectUI@@QEAAPEAV12@XZ");
    HWNDElement_GetHWND = (HWNDElement_GetHWND_t)GetProcAddress(
        dui70, "?GetHWND@HWNDElement@DirectUI@@UEAAPEAUHWND__@@XZ");
    StrToID = (StrToID_t)GetProcAddress(dui70, "StrToID");
    if (!Element_GetID || !Element_GetRoot || !HWNDElement_GetHWND ||
        !StrToID) {
        return false;
    }

    // shell32.dll
    WindhawkUtils::SYMBOL_HOOK shell32Hooks[] = {
        {{L"private: long __cdecl CDUISizerElement::_Render(void)"},
         (void**)&CDUISizerElement_Render_Original,
         (void*)CDUISizerElement_Render_Hook,
         false},
        {{L"public: int __cdecl CDUISizerElement::GetReservedSpace(void)"},
         (void**)&CDUISizerElement_GetReservedSpace_Original,
         (void*)CDUISizerElement_GetReservedSpace_Hook,
         false},
    };

    return WindhawkUtils::HookSymbols(shell32, shell32Hooks,
                                      ARRAYSIZE(shell32Hooks));
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    // Not fatal: the rest of the mod does not depend on it.
    if (!HookPaneSizer()) {
        Wh_Log(L"Failed to hook the folder pane splitter, the pane will give "
               L"way in narrow windows as usual");
    }

    if (!Wh_SetFunctionHook((void*)CreateWindowExW,
                            (void*)CreateWindowExW_Hook,
                            (void**)&CreateWindowExW_Original)) {
        Wh_Log(L"Failed to hook CreateWindowExW");
        return FALSE;
    }

    Wh_SetFunctionHook((void*)SetScrollInfo, (void*)SetScrollInfo_Hook,
                       (void**)&SetScrollInfo_Original);
    Wh_SetFunctionHook((void*)SetScrollPos, (void*)SetScrollPos_Hook,
                       (void**)&SetScrollPos_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    SubclassExistingSinks();
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    std::vector<HWND> sinks;
    std::vector<HWND> trees;
    std::vector<HWND> duis;
    std::vector<HWND> treeViews;
    {
        std::lock_guard<std::mutex> guard(g_sinksMutex);
        for (const auto& entry : g_sinks) {
            sinks.push_back(entry.first);
            if (entry.second.hTree) {
                trees.push_back(entry.second.hTree);
            }
            if (entry.second.hDui) {
                duis.push_back(entry.second.hDui);
            }
            if (entry.second.hTreeView) {
                treeViews.push_back(entry.second.hTreeView);
            }
        }
        g_sinks.clear();
    }

    for (HWND hTree : trees) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hTree,
                                                         TreeSubclassProc);
    }

    for (HWND hSink : sinks) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hSink,
                                                         SinkSubclassProc);
    }

    std::vector<HWND> neighbours;
    {
        std::lock_guard<std::mutex> guard(g_neighboursMutex);
        neighbours.assign(g_neighbours.begin(), g_neighbours.end());
        g_neighbours.clear();
    }

    for (HWND hNeighbour : neighbours) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hNeighbour,
                                                         NeighbourSubclassProc);
    }

    for (HWND hTreeView : treeViews) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hTreeView,
                                                         TreeViewSubclassProc);
    }

    for (HWND hDui : duis) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hDui,
                                                         DuiSubclassProc);
    }
}
