// ==WindhawkMod==
// @id              taskbar-spatial-order
// @name            Taskbar Spatial Order
// @description     Keeps each app's windows on the taskbar in the order they sit on screen, left to right, in thumbnail previews and uncombined buttons
// @version         1.0
// @author          Jori Huisman
// @github          https://github.com/SixFive7
// @homepage        https://github.com/SixFive7/taskbar-spatial-order
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi
// @license         GPL-3.0
// ==/WindhawkMod==

// Taskbar Spatial Order
// Copyright (C) 2026 Jori Huisman
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License version 3 as published by
// the Free Software Foundation. This program is distributed WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE. See https://www.gnu.org/licenses/gpl-3.0.html for the
// full licence.
//
// Parts of this mod are adapted from GPL-3.0 mods by Michael Maltsev (m417z):
// - Taskbar Thumbnail Reorder: the move-and-refresh technique (reorder the
//   group's item list, then take each moved item off the taskbar and put it
//   back, so the button list and the thumbnail list follow), the lookup of
//   each taskbar and of a group's item list, and, from his development
//   version, repositioning several items in one move instead of one slot at a
//   time.
//   https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-thumbnail-reorder.wh.cpp
//   https://github.com/m417z/my-windhawk-mods/commit/935c372e3de33982b3c0dfb2c6871ef88158545e
// - Cycle taskbar buttons with mouse wheel: the lookup of a taskbar's button
//   groups.
//   https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-wheel-cycle.wh.cpp
// - Taskbar Labels for Windows 11: running code on the taskbar's thread.
//   https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-labels.wh.cpp

// ==WindhawkModReadme==
/*
# Taskbar Spatial Order

Keeps each app's windows on the taskbar in the order they sit on your screens,
left to right, then top to bottom, rather than roughly the order they were
opened. Hover over an app's taskbar button and its leftmost window is the
leftmost thumbnail; uncombined buttons follow the same order.

The order follows as windows move, are minimised or restored, open or close.
The mod waits until things have been still for a moment, and while a window
is dragged or the Start menu, search, notifications or a jump list is open.

By default, minimised windows go last and a group whose windows are all
minimised is left alone. The settings can change both and exclude apps.

## Limitations

- Windows 11's current taskbar only. Tested on build 26300. The mod relies on
  internal taskbar functions found through Windows debug symbols, which an
  update can change; if one is missing or looks unexpected, the mod logs it
  and does nothing.
- A button that moves is taken off the taskbar and put back, which can show
  as a brief animation. Only buttons that are out of place move, and they keep
  their progress bar, overlay badge and thumbnail toolbar buttons.
- The mod's order wins: a window moved to another place in its group, by hand
  or by another mod, goes back at the next sort.

## Credits

The move-and-refresh technique and the code that finds the taskbar's lists
are adapted from [Taskbar Thumbnail Reorder][reorder] by [m417z][m417z], with
smaller parts from his [Cycle taskbar buttons with mouse wheel][wheel] and
[Taskbar Labels for Windows 11][labels] mods.

[reorder]: https://windhawk.net/mods/taskbar-thumbnail-reorder
[m417z]: https://github.com/m417z
[wheel]: https://windhawk.net/mods/taskbar-wheel-cycle
[labels]: https://windhawk.net/mods/taskbar-labels
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- minimisedLast: true
  $name: Minimised windows last
  $description: >-
    Put minimised windows after the visible ones. Either way, a minimised
    window is placed by its restored position: where it sits when not
    minimised, maximised or snapped.
- skipFullyMinimisedGroups: true
  $name: Skip groups where every window is minimised
- excludedApps: [""]
  $name: Excluded apps
  $description: >-
    Executable names, for example mspaint.exe, whose groups are never
    reordered. Store apps whose windows belong to ApplicationFrameHost.exe can
    only be excluded together, by that name.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <dwmapi.h>

#include <algorithm>
#include <atomic>
#include <optional>
#include <span>
#include <string>
#include <tuple>
#include <unordered_set>
#include <vector>

struct {
    bool minimisedLast;
    bool skipFullyMinimisedGroups;
    std::vector<std::wstring> excludedApps;
} g_settings;

////////////////////////////////////////////////////////////////////////////////
// Taskbar structures
//
// Explorer keeps one item list per app (a CTaskGroup) and, on every taskbar, a
// button group (CTaskBtnGroup) whose buttons are a subset of that list in the
// same order. Everything is reached through functions found by symbol name,
// and every object is checked against its class's vtable before it is used.

// https://www.geoffchappell.com/studies/windows/shell/comctl32/api/da/dpa/dpa.htm
typedef struct _DPA {
    int cpItems;
    PVOID* pArray;
    HANDLE hHeap;
    int cpCapacity;
    int cpGrow;
} DPA, *HDPA;

void* CTaskListWnd_ITaskListUI_vftable;
void* CTaskBtnGroup_ITaskBtnGroup_vftable;
void* CTaskGroup_ITaskGroup_vftable;
void* CWindowTaskItem_ITaskItem_vftable;
void* CImmersiveTaskItem_ITaskItem_vftable;

using GetInt_t = int(WINAPI*)(void* pThis);
GetInt_t CTaskListWnd_GetButtonGroupCount;
GetInt_t CTaskBtnGroup_GetGroupType;
GetInt_t CTaskBtnGroup_GetNumItems;
GetInt_t CTaskGroup_GetNumItems;

using CTaskBtnGroup_GetGroup_t = void*(WINAPI*)(void* pThis);
CTaskBtnGroup_GetGroup_t CTaskBtnGroup_GetGroup;

using CTaskBtnGroup_GetTaskItem_t = void*(WINAPI*)(void* pThis, int index);
CTaskBtnGroup_GetTaskItem_t CTaskBtnGroup_GetTaskItem;

using TaskItem_GetWindow_t = HWND(WINAPI*)(void* pThis);
TaskItem_GetWindow_t CWindowTaskItem_GetWindow;
TaskItem_GetWindow_t CImmersiveTaskItem_GetWindow;

// Positions, in pointer-sized slots, of the DPA of button groups in a
// taskbar's ITaskListUI and of the DPA of items in a CTaskGroup.
int g_buttonGroupsSlot;
int g_itemsSlot;

// Finds which slot of an object a getter reads its DPA from. The getters are
// `return m_dpa ? DPA_GetPtrCount(m_dpa) : 0;`, so called on an array of
// pointers to the numbers 0, 1, 2, ..., one returns the slot it read. This is
// m417z's technique; it takes the offset from Windows' own code instead of
// guessing it.
int FindDpaSlot(GetInt_t getCount) {
    constexpr int kSlots = 256;
    int numbers[kSlots];
    int* pointers[kSlots];
    for (int i = 0; i < kSlots; i++) {
        numbers[i] = i;
        pointers[i] = &numbers[i];
    }

    int slot = getCount(pointers);
    return slot > 0 && slot < kSlots ? slot : -1;
}

bool HasVtable(void* object, void* vtable) {
    return object && *(void**)object == vtable;
}

void* FindInterface(void* object, void* vtable) {
    for (int i = 0; i < 16; i++) {
        if (((void**)object)[i] == vtable) {
            return (void**)object + i;
        }
    }

    return nullptr;
}

// The ITaskListUI of every taskbar, one per monitor. All taskbars belong to
// the calling thread.
std::vector<void*> FindTaskLists() {
    std::vector<void*> taskLists;

    auto proc = [](HWND hWnd, LPARAM lParam) -> BOOL {
        WCHAR className[32];
        if (!GetClassName(hWnd, className, ARRAYSIZE(className))) {
            return TRUE;
        }

        HWND hTaskSwWnd;
        if (_wcsicmp(className, L"Shell_TrayWnd") == 0) {
            hTaskSwWnd = (HWND)GetProp(hWnd, L"TaskbandHWND");
        } else if (_wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0) {
            hTaskSwWnd = FindWindowEx(hWnd, nullptr, L"WorkerW", nullptr);
        } else {
            return TRUE;
        }

        HWND hTaskListWnd = hTaskSwWnd
                                ? FindWindowEx(hTaskSwWnd, nullptr,
                                               L"MSTaskListWClass", nullptr)
                                : nullptr;
        void* taskListWnd =
            hTaskListWnd ? (void*)GetWindowLongPtr(hTaskListWnd, 0) : nullptr;
        void* taskList =
            taskListWnd
                ? FindInterface(taskListWnd, CTaskListWnd_ITaskListUI_vftable)
                : nullptr;
        if (taskList) {
            ((std::vector<void*>*)lParam)->push_back(taskList);
        } else {
            Wh_Log(L"No task list found for taskbar %08X",
                   (DWORD)(DWORD_PTR)hWnd);
        }

        return TRUE;
    };

    EnumThreadWindows(GetCurrentThreadId(), proc, (LPARAM)&taskLists);
    return taskLists;
}

// A taskbar that shows a group: its ITaskListUI and the group's button group
// there.
struct Taskbar {
    void* taskList;
    void* buttonGroup;
};

struct Group {
    void* taskGroup;
    std::vector<Taskbar> taskbars;
};

// Every group that has window buttons on a taskbar. Returns false if
// something doesn't look as expected.
bool FindGroups(std::vector<Group>& groups) {
    for (void* taskList : FindTaskLists()) {
        HDPA buttonGroups = (HDPA)((void**)taskList)[g_buttonGroupsSlot];
        for (int i = 0; buttonGroups && i < buttonGroups->cpItems; i++) {
            void* buttonGroup = buttonGroups->pArray[i];
            if (!HasVtable(buttonGroup, CTaskBtnGroup_ITaskBtnGroup_vftable)) {
                Wh_Log(L"Unexpected object in the button group list");
                return false;
            }

            // 1: one button, or several uncombined ones. 2: a pinned app
            // without windows. 3: several combined buttons.
            int type = CTaskBtnGroup_GetGroupType(buttonGroup);
            if (type != 1 && type != 3) {
                continue;
            }

            void* taskGroup = CTaskBtnGroup_GetGroup(buttonGroup);
            if (!HasVtable(taskGroup, CTaskGroup_ITaskGroup_vftable)) {
                Wh_Log(L"Unexpected task group object");
                return false;
            }

            auto it = std::ranges::find(groups, taskGroup, &Group::taskGroup);
            if (it == groups.end()) {
                it = groups.insert(it, {taskGroup, {}});
            }

            it->taskbars.push_back({taskList, buttonGroup});
        }
    }

    return true;
}

// The window of an item, for the two kinds of items that have one: desktop
// windows and Store app windows.
HWND ItemWindow(void* item) {
    if (HasVtable(item, CWindowTaskItem_ITaskItem_vftable)) {
        return CWindowTaskItem_GetWindow(item);
    }

    if (HasVtable(item, CImmersiveTaskItem_ITaskItem_vftable)) {
        return CImmersiveTaskItem_GetWindow(item);
    }

    return nullptr;
}

int IndexOfItem(HDPA items, void* item) {
    for (int i = 0; i < items->cpItems; i++) {
        if (items->pArray[i] == item) {
            return i;
        }
    }

    return -1;
}

// The rank of each button of a button group: the index of its item in the item
// list. Nothing if an item isn't there.
std::optional<std::vector<int>> ButtonRanks(void* buttonGroup, HDPA items) {
    std::vector<int> ranks(CTaskBtnGroup_GetNumItems(buttonGroup));
    for (int i = 0; i < (int)ranks.size(); i++) {
        ranks[i] =
            IndexOfItem(items, CTaskBtnGroup_GetTaskItem(buttonGroup, i));
        if (ranks[i] == -1) {
            Wh_Log(L"Button without an item in its group's item list");
            return std::nullopt;
        }
    }

    return ranks;
}

////////////////////////////////////////////////////////////////////////////////
// Moving buttons
//
// Adapted from m417z's Taskbar Thumbnail Reorder. Once the group's item list
// is in the new order, a button that is out of place is taken off its taskbar
// and put back. The taskbar adds it right after the closest item before it in
// the item list that has a button there, and inserts its thumbnail at the
// same index, so both end up in the order of the item list. m417z takes the
// button off with TaskInclusionChanged while a hook makes the taskbar's filter
// reject the item; on the current taskbar that path is a call to
// TaskDestroyed, which is called directly here instead.

using CTaskListWnd_TaskNotify_t = HRESULT(WINAPI*)(void* pThis,
                                                   void* taskGroup,
                                                   void* taskItem);
CTaskListWnd_TaskNotify_t CTaskListWnd_TaskDestroyed;
CTaskListWnd_TaskNotify_t CTaskListWnd_TaskInclusionChanged;

// The ranks of the buttons to move, in increasing order: all but a longest
// increasing subsequence, so that as few buttons as possible move.
std::vector<int> RanksToMove(std::vector<int> ranks) {
    int count = (int)ranks.size();
    std::vector<int> length(count, 1);
    std::vector<int> previous(count, -1);
    int last = -1;
    for (int i = 0; i < count; i++) {
        for (int j = 0; j < i; j++) {
            if (ranks[j] < ranks[i] && length[j] + 1 > length[i]) {
                length[i] = length[j] + 1;
                previous[i] = j;
            }
        }

        if (last == -1 || length[i] > length[last]) {
            last = i;
        }
    }

    for (int i = last; i != -1; i = previous[i]) {
        ranks[i] = -1;
    }

    std::erase(ranks, -1);
    std::ranges::sort(ranks);
    return ranks;
}

// Brings one taskbar's buttons of a group into the order of the group's item
// list. Buttons are moved one at a time and in the order of the list, so each
// lands next to a button that is already in place, and the group always keeps
// at least one button: a group that loses all its buttons is rebuilt at the
// end of the taskbar.
int FixButtonOrder(const Taskbar& taskbar, void* taskGroup, HDPA items) {
    std::optional<std::vector<int>> ranks =
        ButtonRanks(taskbar.buttonGroup, items);
    if (!ranks) {
        return 0;
    }

    int moved = 0;
    for (int rank : RanksToMove(*ranks)) {
        // Explorer could change the lists while it handles the calls below.
        if (CTaskBtnGroup_GetNumItems(taskbar.buttonGroup) < 2 ||
            rank >= items->cpItems) {
            Wh_Log(L"Lists changed while moving buttons");
            break;
        }

        void* item = items->pArray[rank];
        CTaskListWnd_TaskDestroyed(taskbar.taskList, taskGroup, item);
        CTaskListWnd_TaskInclusionChanged(taskbar.taskList, taskGroup, item);
        moved++;
    }

    return moved;
}

////////////////////////////////////////////////////////////////////////////////
// Sorting

struct Window {
    void* item;
    HWND hWnd;
    bool minimised;
    POINT position;
};

// The top-left corner of a window's visible frame or, if it's minimised or
// parked off screen, of its restored rectangle: where it sits when not
// minimised, maximised or snapped. That rectangle is in workspace coordinates,
// off from screen coordinates only by a taskbar or app bar at the top or left.
POINT WindowPosition(HWND hWnd, bool minimised) {
    RECT rect{};
    if (!minimised &&
        (SUCCEEDED(DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS,
                                         &rect, sizeof(rect))) ||
         GetWindowRect(hWnd, &rect)) &&
        MonitorFromRect(&rect, MONITOR_DEFAULTTONULL)) {
        return {rect.left, rect.top};
    }

    WINDOWPLACEMENT placement{};
    placement.length = sizeof(placement);
    if (GetWindowPlacement(hWnd, &placement)) {
        return {placement.rcNormalPosition.left,
                placement.rcNormalPosition.top};
    }

    return {rect.left, rect.top};
}

// Minimised windows last if so set, then the left edge, the top edge and the
// window handle, which makes the order total, so it never flips between passes.
auto SortKey(const Window& window) {
    return std::tuple(g_settings.minimisedLast && window.minimised,
                      window.position.x, window.position.y,
                      (UINT_PTR)window.hWnd);
}

// The file name of the process that owns a window, such as mspaint.exe.
std::wstring ProcessName(HWND hWnd) {
    DWORD processId = 0;
    GetWindowThreadProcessId(hWnd, &processId);
    HANDLE process =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (!process) {
        return {};
    }

    WCHAR path[MAX_PATH * 2];
    DWORD size = ARRAYSIZE(path);
    BOOL succeeded = QueryFullProcessImageName(process, 0, path, &size);
    CloseHandle(process);
    if (!succeeded) {
        return {};
    }

    PCWSTR name = wcsrchr(path, L'\\');
    return name ? name + 1 : path;
}

// Whether a window's process has one of the names. False if the process can't
// be identified.
bool IsProcessOneOf(HWND hWnd, std::span<const std::wstring> names) {
    if (names.empty()) {
        return false;
    }

    std::wstring name = ProcessName(hWnd);
    return std::ranges::any_of(names, [&](auto& other) {
        return _wcsicmp(name.c_str(), other.c_str()) == 0;
    });
}

bool IsExcluded(HWND hWnd) {
    return IsProcessOneOf(hWnd, g_settings.excludedApps);
}

std::wstring WindowList(const std::vector<Window>& windows) {
    std::wstring list;
    for (const Window& window : windows) {
        WCHAR handle[20];
        swprintf_s(handle, ARRAYSIZE(handle), L"%s%X",
                   list.empty() ? L"" : L" ", (DWORD)(DWORD_PTR)window.hWnd);
        list += handle;
    }

    return list;
}

// Windows whose moves, minimising and restoring can change the order of a
// group: those in groups of two or more. Rebuilt by every pass.
std::unordered_set<HWND> g_trackedWindows;

void SortGroup(const Group& group) {
    HDPA items = (HDPA)((void**)group.taskGroup)[g_itemsSlot];
    int count = items ? items->cpItems : 0;
    if (count < 2) {
        return;
    }

    std::vector<Window> windows(count);
    for (int i = 0; i < count; i++) {
        Window& window = windows[i];
        window.item = items->pArray[i];
        window.hWnd = ItemWindow(window.item);
        if (!window.hWnd || !IsWindow(window.hWnd)) {
            Wh_Log(L"Skipping a group with an item that has no window");
            return;
        }

        g_trackedWindows.insert(window.hWnd);
        window.minimised = IsIconic(window.hWnd);
        window.position = WindowPosition(window.hWnd, window.minimised);
    }

    // Every button must belong to an item of the group, or the item list
    // isn't what it's taken for.
    for (const Taskbar& taskbar : group.taskbars) {
        if (!ButtonRanks(taskbar.buttonGroup, items)) {
            return;
        }
    }

    if ((g_settings.skipFullyMinimisedGroups &&
         std::ranges::all_of(windows, &Window::minimised)) ||
        std::ranges::any_of(windows, IsExcluded, &Window::hWnd)) {
        return;
    }

    if (!std::ranges::is_sorted(windows, {}, SortKey)) {
        std::vector<Window> sorted = windows;
        std::ranges::sort(sorted, {}, SortKey);
        Wh_Log(L"%s: %s -> %s", ProcessName(windows[0].hWnd).c_str(),
               WindowList(windows).c_str(), WindowList(sorted).c_str());

        for (int i = 0; i < count; i++) {
            items->pArray[i] = sorted[i].item;
        }
    }

    for (const Taskbar& taskbar : group.taskbars) {
        if (int moved = FixButtonOrder(taskbar, group.taskGroup, items)) {
            Wh_Log(L"%s: moved %d buttons on taskbar %p",
                   ProcessName(windows[0].hWnd).c_str(), moved,
                   taskbar.taskList);
        }
    }
}

void SortAllGroups() {
    std::vector<Group> groups;
    if (!FindGroups(groups)) {
        return;
    }

    g_trackedWindows.clear();
    for (const Group& group : groups) {
        SortGroup(group);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Triggers
//
// Sorting happens on the taskbar's thread, which owns the lists: the WinEvent
// hooks and the timer are set up there, so their callbacks run there too.

// How long windows have to be still before the groups are sorted again.
constexpr UINT kSettleMs = 400;

std::atomic<DWORD> g_taskbarThreadId;
std::atomic<HWND> g_taskbarWnd;
std::atomic<bool> g_unloading;
std::atomic<bool> g_sorting;
UINT_PTR g_sortTimer;
std::vector<HWINEVENTHOOK> g_winEventHooks;

// Whether the user is in the middle of something the sort shouldn't cut into:
// dragging or resizing a window, holding a mouse button down (which also
// covers dragging a taskbar button or pressing a thumbnail), a menu of the
// taskbar, or one of the shell's popups.
bool IsUserBusy() {
    if (GetAsyncKeyState(VK_LBUTTON) < 0 || GetAsyncKeyState(VK_RBUTTON) < 0) {
        return true;
    }

    GUITHREADINFO info{};
    info.cbSize = sizeof(info);
    if (GetGUIThreadInfo(0, &info) && (info.flags & GUI_INMOVESIZE)) {
        return true;
    }

    info = {};
    info.cbSize = sizeof(info);
    if (GetGUIThreadInfo(GetCurrentThreadId(), &info) &&
        (info.hwndCapture || (info.flags & (GUI_INMENUMODE | GUI_INMOVESIZE |
                                            GUI_POPUPMENUMODE)))) {
        return true;
    }

    // Jump lists, notifications and the calendar, the Start menu, and search
    // are drawn by these processes, so the taskbar's thread is idle while
    // they're open, and moving a button closes an open jump list.
    static const std::wstring kShellHosts[] = {L"ShellExperienceHost.exe",
                                               L"StartMenuExperienceHost.exe",
                                               L"SearchHost.exe"};
    return IsProcessOneOf(GetForegroundWindow(), kShellHosts);
}

void CALLBACK SortTimerProc(HWND, UINT, UINT_PTR, DWORD) {
    // While the user is busy the timer keeps running, so the sort is retried.
    if (g_sorting || IsUserBusy()) {
        return;
    }

    KillTimer(nullptr, g_sortTimer);
    g_sortTimer = 0;

    g_sorting = true;
    SortAllGroups();
    g_sorting = false;
}

// Sorts once things have been still for kSettleMs: every call restarts the
// countdown, so a burst of events ends in one pass.
void ScheduleSort() {
    if (!g_unloading) {
        g_sortTimer = SetTimer(nullptr, g_sortTimer, kSettleMs, SortTimerProc);
    }
}

void CALLBACK WinEventProc(HWINEVENTHOOK,
                           DWORD event,
                           HWND hWnd,
                           LONG idObject,
                           LONG idChild,
                           DWORD,
                           DWORD) {
    if (!hWnd || idObject != OBJID_WINDOW || idChild != CHILDID_SELF) {
        return;
    }

    // A window that's uncloaked may belong to a group without buttons so far,
    // for example after switching to another virtual desktop.
    if (event == EVENT_OBJECT_UNCLOAKED || g_trackedWindows.contains(hWnd)) {
        ScheduleSort();
    }
}

void WINAPI InitOnTaskbarThread(PVOID taskbarWnd) {
    if (g_taskbarThreadId || g_unloading) {
        return;
    }

    g_taskbarThreadId = GetCurrentThreadId();
    g_taskbarWnd = (HWND)taskbarWnd;

    // Moves and resizes ending, minimising and restoring; window moves of any
    // kind; and windows being uncloaked.
    constexpr DWORD kEventRanges[][2] = {
        {EVENT_SYSTEM_MOVESIZEEND, EVENT_SYSTEM_MINIMIZEEND},
        {EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE},
        {EVENT_OBJECT_UNCLOAKED, EVENT_OBJECT_UNCLOAKED},
    };
    for (auto [first, last] : kEventRanges) {
        if (HWINEVENTHOOK hook =
                SetWinEventHook(first, last, nullptr, WinEventProc, 0, 0,
                                WINEVENT_OUTOFCONTEXT)) {
            g_winEventHooks.push_back(hook);
        } else {
            Wh_Log(L"SetWinEventHook failed: %u", GetLastError());
        }
    }

    ScheduleSort();
}

void WINAPI UninitOnTaskbarThread(PVOID) {
    g_unloading = true;

    for (HWINEVENTHOOK hook : g_winEventHooks) {
        UnhookWinEvent(hook);
    }
    g_winEventHooks.clear();

    if (g_sortTimer) {
        KillTimer(nullptr, g_sortTimer);
        g_sortTimer = 0;
    }
}

HWND FindTaskbarWindow() {
    HWND hWnd = nullptr;
    while ((hWnd = FindWindowEx(nullptr, hWnd, L"Shell_TrayWnd", nullptr))) {
        DWORD processId = 0;
        GetWindowThreadProcessId(hWnd, &processId);
        if (processId == GetCurrentProcessId()) {
            return hWnd;
        }
    }

    return nullptr;
}

// Adapted from m417z's Taskbar Labels for Windows 11.
using RunFromWindowThreadProc_t = void(WINAPI*)(PVOID parameter);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         PVOID procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        PVOID procParam;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    RUN_FROM_WINDOW_THREAD_PARAM* param =
                        (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
                    param->proc(param->procParam);
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);

    return true;
}

// Called by the hooks below when a window joins or leaves a group. Explorer
// does that on the taskbar's thread, which is also how the mod finds that
// thread when it's loaded while Explorer starts, before the taskbar exists.
void OnGroupChanged() {
    if (!g_taskbarThreadId) {
        HWND hTaskbarWnd = FindTaskbarWindow();
        if (!hTaskbarWnd || GetWindowThreadProcessId(hTaskbarWnd, nullptr) !=
                                GetCurrentThreadId()) {
            return;
        }

        InitOnTaskbarThread(hTaskbarWnd);
    }

    if (GetCurrentThreadId() == g_taskbarThreadId) {
        ScheduleSort();
    }
}

using CTaskGroup_AddTaskItem_t = HRESULT(WINAPI*)(void* pThis,
                                                  void* taskItem,
                                                  void* insertBefore);
CTaskGroup_AddTaskItem_t CTaskGroup_AddTaskItem_Original;
HRESULT WINAPI CTaskGroup_AddTaskItem_Hook(void* pThis,
                                           void* taskItem,
                                           void* insertBefore) {
    HRESULT result =
        CTaskGroup_AddTaskItem_Original(pThis, taskItem, insertBefore);
    OnGroupChanged();
    return result;
}

using CTaskGroup_RemoveTaskItem_t = HRESULT(WINAPI*)(void* pThis,
                                                     void* taskItem);
CTaskGroup_RemoveTaskItem_t CTaskGroup_RemoveTaskItem_Original;
HRESULT WINAPI CTaskGroup_RemoveTaskItem_Hook(void* pThis, void* taskItem) {
    HRESULT result = CTaskGroup_RemoveTaskItem_Original(pThis, taskItem);
    OnGroupChanged();
    return result;
}

////////////////////////////////////////////////////////////////////////////////
// Mod

void LoadSettings() {
    g_settings.minimisedLast = Wh_GetIntSetting(L"minimisedLast");
    g_settings.skipFullyMinimisedGroups =
        Wh_GetIntSetting(L"skipFullyMinimisedGroups");

    g_settings.excludedApps.clear();
    for (int i = 0;; i++) {
        auto app = WindhawkUtils::StringSetting::make(L"excludedApps[%d]", i);
        std::wstring name = app.get();
        name.erase(0, name.find_first_not_of(L" \t"));
        name.erase(name.find_last_not_of(L" \t") + 1);
        if (name.empty()) {
            break;
        }

        g_settings.excludedApps.push_back(name);
    }
}

bool HookTaskbarSymbols() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Couldn't load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(const CTaskListWnd::`vftable'{for `ITaskListUI'})"},
            &CTaskListWnd_ITaskListUI_vftable,
        },
        {
            {LR"(public: virtual int __cdecl CTaskListWnd::GetButtonGroupCount(void))"},
            &CTaskListWnd_GetButtonGroupCount,
        },
        {
            {LR"(public: virtual long __cdecl CTaskListWnd::TaskDestroyed(struct ITaskGroup *,struct ITaskItem *))"},
            &CTaskListWnd_TaskDestroyed,
        },
        {
            {LR"(public: virtual long __cdecl CTaskListWnd::TaskInclusionChanged(struct ITaskGroup *,struct ITaskItem *))"},
            &CTaskListWnd_TaskInclusionChanged,
        },
        {
            {LR"(const CTaskBtnGroup::`vftable'{for `ITaskBtnGroup'})"},
            &CTaskBtnGroup_ITaskBtnGroup_vftable,
        },
        {
            {LR"(public: virtual enum eTBGROUPTYPE __cdecl CTaskBtnGroup::GetGroupType(void))"},
            &CTaskBtnGroup_GetGroupType,
        },
        {
            {LR"(public: virtual struct ITaskGroup * __cdecl CTaskBtnGroup::GetGroup(void))"},
            &CTaskBtnGroup_GetGroup,
        },
        {
            {LR"(public: virtual int __cdecl CTaskBtnGroup::GetNumItems(void))"},
            &CTaskBtnGroup_GetNumItems,
        },
        {
            {LR"(public: virtual struct ITaskItem * __cdecl CTaskBtnGroup::GetTaskItem(int))"},
            &CTaskBtnGroup_GetTaskItem,
        },
        {
            {LR"(const CTaskGroup::`vftable'{for `ITaskGroup'})"},
            &CTaskGroup_ITaskGroup_vftable,
        },
        {
            {LR"(public: virtual int __cdecl CTaskGroup::GetNumItems(void))"},
            &CTaskGroup_GetNumItems,
        },
        {
            {LR"(public: virtual long __cdecl CTaskGroup::AddTaskItem(struct ITaskItem *,struct ITaskItem *))"},
            &CTaskGroup_AddTaskItem_Original,
            CTaskGroup_AddTaskItem_Hook,
        },
        {
            {LR"(public: virtual long __cdecl CTaskGroup::RemoveTaskItem(struct ITaskItem *))"},
            &CTaskGroup_RemoveTaskItem_Original,
            CTaskGroup_RemoveTaskItem_Hook,
        },
        {
            {LR"(const CWindowTaskItem::`vftable'{for `ITaskItem'})"},
            &CWindowTaskItem_ITaskItem_vftable,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CWindowTaskItem::GetWindow(void))"},
            &CWindowTaskItem_GetWindow,
        },
        {
            {LR"(const CImmersiveTaskItem::`vftable'{for `ITaskItem'})"},
            &CImmersiveTaskItem_ITaskItem_vftable,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CImmersiveTaskItem::GetWindow(void))"},
            &CImmersiveTaskItem_GetWindow,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, taskbarDllHooks,
                                    ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    g_buttonGroupsSlot = FindDpaSlot(CTaskListWnd_GetButtonGroupCount);
    g_itemsSlot = FindDpaSlot(CTaskGroup_GetNumItems);
    Wh_Log(L"Button group list in slot %d, item list in slot %d",
           g_buttonGroupsSlot, g_itemsSlot);
    if (g_buttonGroupsSlot == -1 || g_itemsSlot == -1) {
        Wh_Log(L"Unexpected taskbar code");
        return false;
    }

    return true;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();
    return HookTaskbarSymbols();
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    // If Explorer is still starting, the first group change initialises.
    if (HWND hTaskbarWnd = FindTaskbarWindow()) {
        RunFromWindowThread(hTaskbarWnd, InitOnTaskbarThread, hTaskbarWnd);
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    HWND hTaskbarWnd = g_taskbarWnd;
    if (!hTaskbarWnd) {
        hTaskbarWnd = FindTaskbarWindow();
    }

    if (hTaskbarWnd) {
        RunFromWindowThread(hTaskbarWnd, UninitOnTaskbarThread, nullptr);
    }

    // If the taskbar pumped messages mid-sort, the call above ran inside that
    // sort, which must end before the mod is unloaded.
    while (g_sorting) {
        Sleep(10);
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    *bReload = TRUE;
    return TRUE;
}
