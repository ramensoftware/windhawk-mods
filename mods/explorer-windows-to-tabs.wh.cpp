// ==WindhawkMod==
// @id              explorer-windows-to-tabs
// @name            Explorer: open new windows as tabs
// @name:ru-RU      Проводник: новые окна — во вкладки
// @description     A new File Explorer window opens as a tab in an already open window; a folder that is already open just becomes the active tab
// @description:ru-RU Новое окно Проводника открывается вкладкой в уже открытом окне; уже открытая папка просто становится активной вкладкой
// @version         1.7
// @author          limpompo
// @github          https://github.com/limpompo
// @homepage        https://github.com/limpompo/windhawk-mods-limpompo/tree/main/mods/explorer-windows-to-tabs
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lshell32 -ldwmapi -luser32 -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer: open new windows as tabs

When a new File Explorer window is opened (Win+E, "Open in new window",
"Show in folder" from another program, a folder from the desktop, a shortcut,
etc.) and another visible Explorer window already exists, the mod hides the new
window and:

- if the folder is already open in some tab - makes that tab active;
- otherwise - adds a new tab to the topmost Explorer window and opens the folder there.

Selected files ("Show in folder") are selected in the tab as well. The extra
window is closed. If anything goes wrong, the new window is shown as usual.

When the folder is already open, Explorer handles "Show in folder" by itself,
without a new window: it selects the file in the tab but does not switch to it,
and because of a mix-up in its own window list it sometimes picks the wrong tab
of the window (and then nothing happens). The mod switches to the right tab and,
when Explorer gets it wrong, finds the tab that contains the file.

New tabs are opened with the "Open in new tab" command, like the context menu does.

Not merged:
- a folder opened while **Shift** is held (to get a separate window on purpose);
- a tab dragged out into its own window, however fast the drag is (a tab
  leaves another window at the moment the new window appears);
- Control Panel (it also never gets other folders added as tabs);
- windows on another virtual desktop.

## Compared to Explorer Single Window Tabs

[Explorer Single Window Tabs](https://windhawk.net/mods/explorer-single-window-tabs)
also opens new Explorer windows as tabs. This mod additionally:
- switches to a tab that already shows the folder instead of opening it again;
- carries over the "Show in folder" selection to the new tab;
- fixes "Show in folder" into an already open folder (switches to the right tab);
- opens tabs with the "Open in new tab" verb, so they are registered correctly;
- protects a tab dragged out into its own window by the held mouse button
  (besides the Shift bypass that both mods have);
- restores the new window if the tab couldn't be opened.

**Don't enable both mods at the same time** - they would both handle every new
window.

Works only on Windows 11 with tabs in File Explorer (22H2 and later); on older
builds the mod doesn't load. The "new tab" command (WM_COMMAND 0xA21B) is
undocumented and may change in future builds. System files are not modified.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- reuseOpenTab: true
  $name: Do not open a folder twice
  $name:ru-RU: Не открывать папку повторно
  $description: If the folder is already open in a tab, switch to that tab instead of opening a new one.
  $description:ru-RU: Если папка уже открыта во вкладке, переключиться на эту вкладку вместо новой.
- activateSelectedTab: true
  $name: Switch to the tab where the program selected a file
  $name:ru-RU: Переключаться на вкладку, где программа выделила файл
  $description: When "Show in folder" targets an already open folder, Explorer selects the file in its tab but does not switch to it (and sometimes picks the wrong tab). The mod switches to the right one. It can't tell "Show in folder" from other programs that select files in an Explorer window (scripts, sync tools), so for them the tab is switched too, and the window is brought to the front if that option is on.
  $description:ru-RU: «Показать в папке» при уже открытой папке Проводник выделяет файл в её вкладке, но не переключается на неё (а иногда выбирает не ту вкладку). Мод переключает на нужную. Мод не отличает «Показать в папке» от других программ, которые выделяют файлы в окне Проводника (скрипты, программы синхронизации), поэтому для них вкладка тоже переключается, а окно выходит на передний план, если включена эта настройка.
- activateTarget: true
  $name: Bring the tabbed window to the foreground
  $name:ru-RU: Выводить окно со вкладками на передний план
  $description: After adding or selecting a tab, activate the Explorer window (and restore it if minimized). This includes a tab where a program selected a file (see the previous option).
  $description:ru-RU: После добавления или выбора вкладки активировать окно Проводника (и разворачивать, если оно свёрнуто). В том числе для вкладки, где программа выделила файл (см. предыдущую настройку).
- shiftBypass: true
  $name: Hold Shift to open a separate window
  $name:ru-RU: Shift — открыть отдельным окном
  $description: If Shift is held while a folder is being opened, the new window is left as a separate window.
  $description:ru-RU: Если при открытии папки зажат Shift, новое окно остаётся отдельным окном.
- ignoreMouseDown: true
  $name: Leave tabs dragged out into their own window
  $name:ru-RU: Не трогать вкладки, вытащенные в отдельное окно
  $description: A tab dragged out of a window becomes a separate window and is not merged back, however fast the drag is. A folder opened by double-click is still merged.
  $description:ru-RU: Вкладка, вытащенная из окна, становится отдельным окном и не сливается обратно, как бы быстро её ни вытащили. Папка, открытая двойным щелчком, по-прежнему сливается.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>
#include <dwmapi.h>
#include <exdisp.h>
#include <servprov.h>
#include <shlguid.h>
#include <shlobj.h>
#include <uiautomation.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

static const UINT kCmdNewTab = 0xA21B;                 // WM_COMMAND of the CabinetWClass window: "new tab"
static const UINT kCwmGetIShellBrowser = WM_USER + 7;  // the Explorer browser window returns its IShellBrowser

// Messages posted to the worker thread.
enum : UINT {
    kWorkerMerge = WM_APP + 1,  // wParam: new window; lParam: TabSnapshot* (owned by the worker)
    kWorkerSwitchTab,           // wParam: tab to switch to
};

static const int kMaxWindows = 64;
static const int kMaxSnapshotTabs = 512;     // tabs of all Explorer windows, for dragged-out tab detection
static const int kMaxSelection = 64;
static const DWORD kSelectionGraceMs = 300;  // how long to wait for a late selection after a tab is opened
static const DWORD kPollMs = 15;             // polling step for a new window

static bool g_reuseOpenTab = true;
static bool g_activateSelectedTab = true;
static bool g_activateTarget = true;
static bool g_ignoreMouseDown = true;
static bool g_shiftBypass = true;

// One long-lived worker thread does all merges and tab switches, so
// Wh_ModUninit can wait for it to exit.
static volatile LONG g_stop = 0;
static HANDLE g_stopEvent = nullptr;
static HANDLE g_workerReady = nullptr;
static HANDLE g_worker = nullptr;
static DWORD g_workerId = 0;

static UINT g_tabCommandMsg = 0;          // registered message for commands to a tab
static volatile LONG g_hookCalls = 0;     // TabHookProc calls in progress
static IUIAutomation* g_uia = nullptr;    // created and released on the worker thread

static SRWLOCK g_claimLock = SRWLOCK_INIT;  // protects g_claimed (taken on Explorer threads and the worker)
static HWND g_claimed[kMaxWindows];          // windows the mod already handled (in progress or left as is)
static int g_claimedCount = 0;

static void LoadSettings() {
    g_reuseOpenTab = Wh_GetIntSetting(L"reuseOpenTab") != 0;
    g_activateSelectedTab = Wh_GetIntSetting(L"activateSelectedTab") != 0;
    g_activateTarget = Wh_GetIntSetting(L"activateTarget") != 0;
    g_ignoreMouseDown = Wh_GetIntSetting(L"ignoreMouseDown") != 0;
    g_shiftBypass = Wh_GetIntSetting(L"shiftBypass") != 0;
}

// Waits for ms milliseconds; true if the mod is being unloaded.
static bool Wait(DWORD ms) {
    return WaitForSingleObject(g_stopEvent, ms) != WAIT_TIMEOUT;
}

static bool IsClass(HWND hwnd, PCWSTR cls) {
    WCHAR buf[64];
    return GetClassNameW(hwnd, buf, ARRAYSIZE(buf)) > 0 && wcscmp(buf, cls) == 0;
}

static bool IsOwnProcessWindow(HWND hwnd) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    return pid == GetCurrentProcessId();
}

static bool IsCloaked(HWND hwnd) {
    DWORD cloaked = 0;
    return SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) && cloaked;
}

static bool IsPrimaryButtonDown() {
    int vk = GetSystemMetrics(SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON;
    return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

// Returns true if the window was just added (not claimed before).
static bool Claim(HWND hwnd) {
    bool added = true;
    AcquireSRWLockExclusive(&g_claimLock);
    int n = 0;
    for (int i = 0; i < g_claimedCount; i++) {
        if (g_claimed[i] == hwnd) {
            added = false;
        }
        if (IsWindow(g_claimed[i])) {
            g_claimed[n++] = g_claimed[i];
        }
    }
    g_claimedCount = n;
    if (added && g_claimedCount < kMaxWindows) {
        g_claimed[g_claimedCount++] = hwnd;
    }
    ReleaseSRWLockExclusive(&g_claimLock);
    return added;
}

static void Unclaim(HWND hwnd) {
    AcquireSRWLockExclusive(&g_claimLock);
    for (int i = 0; i < g_claimedCount; i++) {
        if (g_claimed[i] == hwnd) {
            g_claimed[i] = g_claimed[--g_claimedCount];
            break;
        }
    }
    ReleaseSRWLockExclusive(&g_claimLock);
}

static void HideWindowAsync(HWND hwnd) {
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                 SWP_HIDEWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
}

// DWM cloaking: the window stays "visible" to Explorer but is not drawn on screen
// and does not appear on the taskbar. Only the window-owning process can set it.
static void CloakWindow(HWND hwnd, bool cloak) {
    BOOL value = cloak;
    DwmSetWindowAttribute(hwnd, DWMWA_CLOAK, &value, sizeof(value));
}

// Restore the window: uncloak a cloaked one (visibility is managed by Explorer),
// show one that the mod hid.
static void ShowWindowBack(HWND hwnd) {
    if (!IsWindow(hwnd)) {
        return;
    }
    if (IsCloaked(hwnd)) {
        CloakWindow(hwnd, false);
        return;
    }
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                 SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_ASYNCWINDOWPOS);
}

// An Explorer window of this process that can be merged into: visible, on the current
// desktop, not self.
static bool IsCandidateWindow(HWND hwnd, HWND self) {
    return hwnd != self && IsWindowVisible(hwnd) && IsClass(hwnd, L"CabinetWClass") && IsOwnProcessWindow(hwnd) &&
           !IsCloaked(hwnd);
}

struct WindowList {
    HWND items[kMaxWindows];
    int count;
    HWND self;  // for the window list: which window to skip

    bool Contains(HWND h) const {
        for (int i = 0; i < count; i++) {
            if (items[i] == h) {
                return true;
            }
        }
        return false;
    }
};

static BOOL CALLBACK CandidateWindowsProc(HWND hwnd, LPARAM lp) {
    auto* l = reinterpret_cast<WindowList*>(lp);
    if (l->count < kMaxWindows && IsCandidateWindow(hwnd, l->self)) {
        l->items[l->count++] = hwnd;
    }
    return TRUE;
}

// Explorer windows in Z-order, topmost first.
static void GetCandidateWindows(HWND self, WindowList* list) {
    list->count = 0;
    list->self = self;
    EnumWindows(CandidateWindowsProc, reinterpret_cast<LPARAM>(list));
}

static HWND FindTarget(HWND self) {
    WindowList list;
    GetCandidateWindows(self, &list);
    return list.count ? list.items[0] : nullptr;
}

static BOOL CALLBACK TabWindowsProc(HWND hwnd, LPARAM lp) {
    auto* l = reinterpret_cast<WindowList*>(lp);
    if (l->count < kMaxWindows && IsClass(hwnd, L"ShellTabWindowClass")) {
        l->items[l->count++] = hwnd;
    }
    return TRUE;
}

// Tab windows in Z-order: the active tab comes first.
static void GetTabWindows(HWND top, WindowList* list) {
    list->count = 0;
    list->self = nullptr;
    EnumChildWindows(top, TabWindowsProc, reinterpret_cast<LPARAM>(list));
}

static HWND GetActiveTab(HWND top) {
    WindowList tabs;
    GetTabWindows(top, &tabs);
    return tabs.count ? tabs.items[0] : nullptr;
}

// ---------------------------------------------------------------------------
// State of the new window: folder and selected items.

struct FolderState {
    PIDLIST_ABSOLUTE folder = nullptr;
    PITEMID_CHILD selection[kMaxSelection] = {};
    int selectionCount = 0;

    void Clear() {
        if (folder) {
            ILFree(folder);
            folder = nullptr;
        }
        for (int i = 0; i < selectionCount; i++) {
            ILFree(selection[i]);
        }
        selectionCount = 0;
    }
};

static PIDLIST_ABSOLUTE GetViewFolder(IFolderView* fv) {
    PIDLIST_ABSOLUTE pidl = nullptr;
    ComPtr<IPersistFolder2> pf;
    if (SUCCEEDED(fv->GetFolder(IID_PPV_ARGS(&pf))) && pf) {
        pf->GetCurFolder(&pidl);
    }
    return pidl;
}

// Current folder of the browser (absolute PIDL, free with ILFree). Optionally IFolderView.
static PIDLIST_ABSOLUTE GetBrowserFolder(IShellBrowser* sb, ComPtr<IFolderView>* folderView) {
    ComPtr<IShellView> sv;
    ComPtr<IFolderView> fv;
    if (FAILED(sb->QueryActiveShellView(&sv)) || !sv || FAILED(sv.As(&fv))) {
        return nullptr;
    }
    PIDLIST_ABSOLUTE pidl = GetViewFolder(fv.Get());
    if (folderView && pidl) {
        *folderView = fv;
    }
    return pidl;
}

static ComPtr<IShellBrowser> GetShellBrowser(IWebBrowser2* wb) {
    ComPtr<IServiceProvider> sp;
    ComPtr<IShellBrowser> sb;
    if (SUCCEEDED(wb->QueryInterface(IID_PPV_ARGS(&sp)))) {
        sp->QueryService(SID_STopLevelBrowser, IID_PPV_ARGS(&sb));
    }
    return sb;
}

static ComPtr<IShellWindows> CreateShellWindows() {
    ComPtr<IShellWindows> sw;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&sw)))) {
        sw.Reset();
    }
    return sw;
}

// Reads the new window's folder and selection via ShellWindows (the window is its own, one tab).
static bool ReadWindowState(IShellWindows* sw, HWND top, FolderState* state) {
    long count = 0;
    sw->get_Count(&count);
    for (long i = 0; i < count && !state->folder; i++) {
        VARIANT index;
        VariantInit(&index);
        index.vt = VT_I4;
        index.lVal = i;

        ComPtr<IDispatch> disp;
        ComPtr<IWebBrowser2> wb;
        SHANDLE_PTR h = 0;
        if (FAILED(sw->Item(index, &disp)) || !disp || FAILED(disp.As(&wb)) || FAILED(wb->get_HWND(&h)) ||
            reinterpret_cast<HWND>(h) != top) {
            continue;
        }
        ComPtr<IShellBrowser> sb = GetShellBrowser(wb.Get());
        if (!sb) {
            continue;
        }
        ComPtr<IFolderView> fv;
        state->folder = GetBrowserFolder(sb.Get(), &fv);
        ComPtr<IFolderView2> fv2;
        ComPtr<IShellItemArray> items;
        if (!fv || FAILED(fv.As(&fv2)) || fv2->GetSelection(FALSE, &items) != S_OK || !items) {
            continue;
        }
        DWORD n = 0;
        items->GetCount(&n);
        for (DWORD j = 0; j < n && state->selectionCount < kMaxSelection; j++) {
            ComPtr<IShellItem> item;
            PIDLIST_ABSOLUTE abs = nullptr;
            if (SUCCEEDED(items->GetItemAt(j, &item)) && item && SUCCEEDED(SHGetIDListFromObject(item.Get(), &abs)) &&
                abs) {
                state->selection[state->selectionCount++] = reinterpret_cast<PITEMID_CHILD>(ILClone(ILFindLastID(abs)));
                ILFree(abs);
            }
        }
    }
    return state->folder != nullptr;
}

// Tabs of the other Explorer windows at the moment a new window was created. A tab dragged out
// into its own window leaves its old window right then. Taken only if ignoreMouseDown is on.
struct TabSnapshot {
    bool mouseDown = false;
    int count = 0;
    HWND tabs[kMaxSnapshotTabs] = {};
    HWND owners[kMaxSnapshotTabs] = {};
    DWORD titles[kMaxSnapshotTabs] = {};  // TitleHash of each tab
};

// Hash of a tab's title (the folder name); 0 if it has none yet. InternalGetWindowText sends no
// messages, so it is safe for windows of other threads and right inside CreateWindowExW.
static DWORD TitleHash(HWND hwnd) {
    WCHAR title[260];
    int len = hwnd ? InternalGetWindowText(hwnd, title, ARRAYSIZE(title)) : 0;
    DWORD hash = 2166136261u;  // FNV-1a
    for (int i = 0; i < len; i++) {
        hash = (hash ^ title[i]) * 16777619u;
    }
    return len ? hash : 0;
}

static TabSnapshot* TakeTabSnapshot(HWND self, bool mouseDown) {
    auto* snapshot = new TabSnapshot;
    snapshot->mouseDown = mouseDown;
    if (!g_ignoreMouseDown) {
        return snapshot;
    }
    WindowList windows;
    GetCandidateWindows(self, &windows);
    for (int w = 0; w < windows.count; w++) {
        WindowList tabs;
        GetTabWindows(windows.items[w], &tabs);
        for (int t = 0; t < tabs.count && snapshot->count < kMaxSnapshotTabs; t++) {
            snapshot->tabs[snapshot->count] = tabs.items[t];
            snapshot->owners[snapshot->count] = windows.items[w];
            snapshot->titles[snapshot->count] = TitleHash(tabs.items[t]);
            snapshot->count++;
        }
    }
    return snapshot;
}

// Whether self is a tab dragged out of another window. Observed on Windows 11 build 26300:
// Explorer creates the new window ~100 ms after the mouse button is released, with a new tab, and
// ~60 ms later destroys the dragged tab in the old window, which stays open. So: a tab that existed
// when self was created has moved into self, or has left a window that is still open. A closed
// window doesn't count (its tabs leave with it), and windows being merged are cloaked and not in
// the snapshot. A tab that left must have the same title as the tab of self: otherwise it is a tab
// the user closed in another window at that moment. While the title of self (or of the tab) is
// unknown, the tab counts only if finalCheck is set - the last check before merging stays on the
// safe side.
static bool IsDraggedOutTab(HWND self, const TabSnapshot& snapshot, bool finalCheck = false) {
    DWORD selfTitle = 0;
    bool selfTitleRead = false;
    for (int i = 0; i < snapshot.count; i++) {
        HWND root = IsWindow(snapshot.tabs[i]) ? GetAncestor(snapshot.tabs[i], GA_ROOT) : nullptr;
        if (root == self) {
            return true;
        }
        if (root == snapshot.owners[i] || !IsWindow(snapshot.owners[i])) {
            continue;
        }
        if (!selfTitleRead) {
            selfTitle = TitleHash(GetActiveTab(self));
            selfTitleRead = true;
        }
        if (selfTitle && snapshot.titles[i] ? selfTitle == snapshot.titles[i] : finalCheck) {
            return true;
        }
    }
    return false;
}

// Control Panel opens in an Explorer window too, but it doesn't work well as a tab: such a
// window is never merged and never used as the window to add a tab to.
static bool IsControlPanelFolder(PCIDLIST_ABSOLUTE pidl) {
    PWSTR name = nullptr;
    if (!pidl || FAILED(SHGetNameFromIDList(pidl, SIGDN_DESKTOPABSOLUTEPARSING, &name)) || !name) {
        return false;
    }
    CharUpperW(name);
    bool controlPanel = wcsstr(name, L"::{26EE0668-A00A-44D7-9371-BEB064C98683}") ||  // Control Panel
                        wcsstr(name, L"::{21EC2020-3AEA-1069-A2DD-08002B30309D}");     // All Control Panel Items
    CoTaskMemFree(name);
    return controlPanel;
}

// Whether any tab of Explorer window top shows Control Panel.
static bool WindowShowsControlPanel(IShellWindows* sw, HWND top) {
    long count = 0;
    sw->get_Count(&count);
    for (long i = 0; i < count; i++) {
        VARIANT index;
        VariantInit(&index);
        index.vt = VT_I4;
        index.lVal = i;

        ComPtr<IDispatch> disp;
        ComPtr<IWebBrowser2> wb;
        SHANDLE_PTR h = 0;
        if (FAILED(sw->Item(index, &disp)) || !disp || FAILED(disp.As(&wb)) || FAILED(wb->get_HWND(&h)) ||
            reinterpret_cast<HWND>(h) != top) {
            continue;
        }
        ComPtr<IShellBrowser> sb = GetShellBrowser(wb.Get());
        PIDLIST_ABSOLUTE folder = sb ? GetBrowserFolder(sb.Get(), nullptr) : nullptr;
        bool controlPanel = IsControlPanelFolder(folder);
        if (folder) {
            ILFree(folder);
        }
        if (controlPanel) {
            return true;
        }
    }
    return false;
}

// The topmost Explorer window to add a tab to (not Control Panel).
static HWND FindMergeTarget(HWND self, IShellWindows* sw) {
    WindowList list;
    GetCandidateWindows(self, &list);
    for (int i = 0; i < list.count; i++) {
        if (!WindowShowsControlPanel(sw, list.items[i])) {
            return list.items[i];
        }
    }
    return nullptr;
}

// Waits until the folder is set in the new window (two identical checks in a row). The selection
// ("Show in folder") may arrive a bit later - CatchLateSelection picks it up after the tab is
// opened, so that it does not delay every merge. Stops early (*draggedOut) if self turns out to be a
// tab dragged out of another window.
static bool WaitForFolder(HWND self, IShellWindows* sw, const TabSnapshot& snapshot, FolderState* result,
                          DWORD timeoutMs, bool* draggedOut) {
    FolderState prev;
    DWORD start = GetTickCount();
    // A window caught at creation (cloaked) is merged only if Explorer showed it:
    // service invisible windows are left alone. A hidden window was already visible.
    bool seenVisible = !IsCloaked(self);
    *draggedOut = false;
    while (GetTickCount() - start < timeoutMs && !g_stop && IsWindow(self)) {
        if (g_ignoreMouseDown && IsDraggedOutTab(self, snapshot)) {
            *draggedOut = true;
            break;
        }
        if (IsWindowVisible(self)) {
            seenVisible = true;
            if (!IsCloaked(self)) {
                HideWindowAsync(self);  // Explorer showed the hidden window again
            }
        }
        FolderState cur;
        if (ReadWindowState(sw, self, &cur)) {
            if (seenVisible && prev.folder && ILIsEqual(prev.folder, cur.folder)) {
                Wh_Log(L"folder ready after %lu ms, %d selected", GetTickCount() - start, cur.selectionCount);
                prev.Clear();
                *result = cur;
                return true;
            }
            prev.Clear();
            prev = cur;
        }
        if (Wait(kPollMs)) {
            break;
        }
    }
    prev.Clear();
    return false;
}

// ---------------------------------------------------------------------------
// Commands to a tab. An empty new tab is not visible via ShellWindows, and inactive
// tabs are not always listed there, so everything is done inside the tab thread:
// a WH_CALLWNDPROC hook catches the registered message and works with the tab's
// IShellBrowser. Sender and receiver are in the same process, so the message carries
// a pointer to the command.

enum TabCommandType {
    kTabNavigate,    // navigate to the folder
    kTabMatch,       // succeed if the tab is open on the folder
    kTabSelect,      // the tab is open on the folder: select the items
    kTabOpenNewTab,  // "Open in new tab" for the folder from this tab
};

// Reference-counted: the sender and the hook each hold a reference, so the sender can stop
// waiting (timeout, unload) while the hook is still working on the command.
struct TabCommand {
    volatile LONG refs = 2;
    TabCommandType type = kTabMatch;
    PIDLIST_ABSOLUTE folder = nullptr;
    PITEMID_CHILD selection[kMaxSelection] = {};
    int selectionCount = 0;
    HANDLE done = nullptr;  // set by the hook when the command is finished
    volatile LONG result = 0;
};

static TabCommand* NewTabCommand(TabCommandType type, PCIDLIST_ABSOLUTE folder, const FolderState* state) {
    auto* cmd = new TabCommand;
    cmd->type = type;
    cmd->folder = ILCloneFull(folder);
    cmd->done = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (state) {
        for (int i = 0; i < state->selectionCount; i++) {
            if (PITEMID_CHILD child = reinterpret_cast<PITEMID_CHILD>(ILClone(state->selection[i]))) {
                cmd->selection[cmd->selectionCount++] = child;
            }
        }
    }
    if (!cmd->folder || !cmd->done) {
        cmd->refs = 1;
    }
    return cmd;
}

static void ReleaseTabCommand(TabCommand* cmd) {
    if (InterlockedDecrement(&cmd->refs) != 0) {
        return;
    }
    if (cmd->folder) {
        ILFree(cmd->folder);
    }
    for (int i = 0; i < cmd->selectionCount; i++) {
        ILFree(cmd->selection[i]);
    }
    if (cmd->done) {
        CloseHandle(cmd->done);
    }
    delete cmd;
}

// The folder command "Open in new tab" (opennewtab) with tab sb as the site - the same
// as from the context menu. A tab opened this way is registered correctly in ShellWindows;
// after BrowseObject in an empty tab the registration gets confused, and "Show in folder"
// then lands in the wrong tab.
static bool InvokeOpenNewTab(IShellBrowser* sb, HWND hwnd, PCIDLIST_ABSOLUTE folder) {
    ComPtr<IShellFolder> parent;
    PCUITEMID_CHILD child = nullptr;
    if (FAILED(SHBindToParent(folder, IID_PPV_ARGS(&parent), &child)) || !parent) {
        return false;
    }
    ComPtr<IContextMenu> menu;
    if (FAILED(parent->GetUIObjectOf(hwnd, 1, &child, __uuidof(IContextMenu), nullptr,
                                     reinterpret_cast<void**>(menu.GetAddressOf()))) ||
        !menu) {
        return false;
    }
    ComPtr<IObjectWithSite> site;
    if (SUCCEEDED(menu.As(&site))) {
        site->SetSite(sb);
    }
    bool ok = false;
    HMENU popup = CreatePopupMenu();
    if (popup && SUCCEEDED(menu->QueryContextMenu(popup, 0, 1, 0x7FFF, CMF_NORMAL))) {
        CMINVOKECOMMANDINFOEX ici = {};
        ici.cbSize = sizeof(ici);
        ici.fMask = CMIC_MASK_UNICODE | CMIC_MASK_FLAG_NO_UI;
        ici.hwnd = hwnd;
        ici.lpVerb = "opennewtab";
        ici.lpVerbW = L"opennewtab";
        ici.nShow = SW_SHOWNORMAL;
        HRESULT hr = menu->InvokeCommand(reinterpret_cast<CMINVOKECOMMANDINFO*>(&ici));
        Wh_Log(L"opennewtab: 0x%08X", static_cast<unsigned>(hr));
        ok = SUCCEEDED(hr);
    }
    if (popup) {
        DestroyMenu(popup);
    }
    if (site) {
        site->SetSite(nullptr);
    }
    return ok;
}

static bool RunTabCommand(IShellBrowser* sb, HWND hwnd, const TabCommand* cmd) {
    if (cmd->type == kTabNavigate) {
        return SUCCEEDED(sb->BrowseObject(cmd->folder, SBSP_SAMEBROWSER | SBSP_ABSOLUTE));
    }
    if (cmd->type == kTabOpenNewTab) {
        return InvokeOpenNewTab(sb, hwnd, cmd->folder);
    }

    ComPtr<IFolderView> fv;
    PIDLIST_ABSOLUTE cur = GetBrowserFolder(sb, cmd->type == kTabSelect ? &fv : nullptr);
    bool same = cur && ILIsEqual(cur, cmd->folder);
    if (cur) {
        ILFree(cur);
    }
    if (cmd->type == kTabMatch || !same) {
        return same;
    }

    // kTabSelect: the folder matches, select the items.
    ComPtr<IShellView> sv;
    if (FAILED(sb->QueryActiveShellView(&sv)) || !sv) {
        return false;
    }
    for (int i = 0; i < cmd->selectionCount; i++) {
        UINT flags = SVSI_SELECT;
        if (i == 0) {
            flags |= SVSI_DESELECTOTHERS | SVSI_ENSUREVISIBLE | SVSI_FOCUSED;
        }
        sv->SelectItem(cmd->selection[i], flags);
    }
    // Until the folder is enumerated, SelectItem selects nothing - retry then.
    int selected = 0;
    return fv && SUCCEEDED(fv->ItemCount(SVGIO_SELECTION, &selected)) && selected == cmd->selectionCount;
}

static LRESULT CALLBACK TabHookProc(int code, WPARAM wParam, LPARAM lParam) {
    InterlockedIncrement(&g_hookCalls);
    if (code == HC_ACTION) {
        auto* cwp = reinterpret_cast<CWPSTRUCT*>(lParam);
        if (cwp->message == g_tabCommandMsg && g_tabCommandMsg && cwp->lParam) {
            auto* cmd = reinterpret_cast<TabCommand*>(cwp->lParam);
            auto* sb = reinterpret_cast<IShellBrowser*>(SendMessageW(cwp->hwnd, kCwmGetIShellBrowser, 0, 0));
            if (sb && cmd->type == kTabOpenNewTab) {
                // The command makes outgoing COM calls, which are not allowed inside SendMessage
                // (RPC_E_CANTCALLOUT_ININPUTSYNCCALL): release the sender first.
                ReplyMessage(TRUE);
            }
            cmd->result = sb && RunTabCommand(sb, cwp->hwnd, cmd);
            SetEvent(cmd->done);
            ReleaseTabCommand(cmd);
        }
    }
    LRESULT result = CallNextHookEx(nullptr, code, wParam, lParam);
    InterlockedDecrement(&g_hookCalls);
    return result;
}

// Command hooks installed during one merge: one per tab-window thread, removed at the end of
// the merge (ReleaseTabHooks). Installing a hook per command is expensive - when searching for
// an open folder the command goes to every tab. Worker thread only.
static struct {
    DWORD thread;
    HHOOK hook;
} g_tabHooks[16];
static int g_tabHookCount = 0;

// Waits until no TabHookProc call is running (UnhookWindowsHookEx doesn't wait for them).
static void WaitForHookCalls() {
    while (g_hookCalls > 0) {
        Sleep(5);
    }
}

static void ReleaseTabHooks() {
    for (int i = 0; i < g_tabHookCount; i++) {
        UnhookWindowsHookEx(g_tabHooks[i].hook);
    }
    g_tabHookCount = 0;
    WaitForHookCalls();
}

// Hook for a thread of this process; *temporary - remove right after the command (cache overflow).
// hMod is null, so a thread of another process can't be hooked at all.
static HHOOK GetTabHook(DWORD thread, bool* temporary) {
    *temporary = false;
    for (int i = 0; i < g_tabHookCount; i++) {
        if (g_tabHooks[i].thread == thread) {
            return g_tabHooks[i].hook;
        }
    }
    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, TabHookProc, nullptr, thread);
    if (!hook) {
        Wh_Log(L"SetWindowsHookEx failed: %lu", GetLastError());
    } else if (g_tabHookCount < static_cast<int>(ARRAYSIZE(g_tabHooks))) {
        g_tabHooks[g_tabHookCount++] = {thread, hook};
    } else {
        *temporary = true;
    }
    return hook;
}

// Executes a command in tab "tab". true - the hook reported success (wait up to waitMs after the reply).
static bool SendTabCommand(HWND tab, TabCommandType type, PCIDLIST_ABSOLUTE folder, const FolderState* state = nullptr,
                           DWORD waitMs = 0) {
    DWORD pid = 0;
    DWORD thread = GetWindowThreadProcessId(tab, &pid);
    if (!thread || pid != GetCurrentProcessId() || g_stop) {
        return false;
    }
    bool temporary;
    HHOOK hook = GetTabHook(thread, &temporary);
    if (!hook) {
        return false;
    }
    TabCommand* cmd = NewTabCommand(type, folder, state);
    bool ok = false;
    if (cmd->refs == 2) {
        DWORD_PTR res = 0;
        if (SendMessageTimeoutW(tab, g_tabCommandMsg, 0, reinterpret_cast<LPARAM>(cmd), SMTO_ABORTIFHUNG, 3000,
                                &res)) {
            HANDLE handles[] = {cmd->done, g_stopEvent};
            ok = WaitForMultipleObjects(2, handles, FALSE, waitMs) == WAIT_OBJECT_0 && cmd->result;
        } else if (!IsWindow(tab)) {
            ReleaseTabCommand(cmd);  // the tab is gone: the hook will never see the command
        }
        // If the message timed out, the hook may still get it later, so its reference is kept; if
        // it never does, the command leaks (rare: the tab's thread was hung for 3 seconds).
    }
    if (temporary) {
        UnhookWindowsHookEx(hook);
        WaitForHookCalls();
    }
    ReleaseTabCommand(cmd);
    return ok;
}

// Selects items in a tab; waits until the tab loads the folder.
static bool ApplySelection(HWND tab, const FolderState& state) {
    if (!state.selectionCount) {
        return true;
    }
    DWORD start = GetTickCount();
    while (GetTickCount() - start < 3000 && !g_stop) {
        if (SendTabCommand(tab, kTabSelect, state.folder, &state)) {
            Wh_Log(L"selection applied: %d items", state.selectionCount);
            return true;
        }
        if (Wait(30)) {
            break;
        }
    }
    Wh_Log(L"selection not applied");
    return false;
}

// ---------------------------------------------------------------------------
// Switching a tab via UI Automation: the tab strip has no window command.

static IUIAutomation* GetUia() {
    if (!g_uia) {
        IUIAutomation* uia = nullptr;
        if (SUCCEEDED(CoCreateInstance(__uuidof(CUIAutomation), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&uia)))) {
            g_uia = uia;
        }
    }
    return g_uia;
}

static ComPtr<IUIAutomationCondition> ControlTypeCondition(IUIAutomation* uia, long type) {
    VARIANT v;
    VariantInit(&v);
    v.vt = VT_I4;
    v.lVal = type;
    ComPtr<IUIAutomationCondition> cond;
    if (FAILED(uia->CreatePropertyCondition(UIA_ControlTypePropertyId, v, &cond))) {
        cond.Reset();
    }
    return cond;
}

static ComPtr<IUIAutomationElementArray> FindTabItems(IUIAutomation* uia, HWND top) {
    ComPtr<IUIAutomationElement> root;
    ComPtr<IUIAutomationCondition> cond = ControlTypeCondition(uia, UIA_TabItemControlTypeId);
    if (!cond || FAILED(uia->ElementFromHandle(top, &root)) || !root) {
        return nullptr;
    }
    // Tab strip in the window title bar: look for tabs only there, not in the whole window tree.
    ComPtr<IUIAutomationElement> strip;
    ComPtr<IUIAutomationCondition> stripCond = ControlTypeCondition(uia, UIA_TabControlTypeId);
    if (!stripCond || FAILED(root->FindFirst(TreeScope_Descendants, stripCond.Get(), &strip))) {
        strip.Reset();
    }
    ComPtr<IUIAutomationElementArray> items;
    int found = 0;
    if (strip && SUCCEEDED(strip->FindAll(TreeScope_Descendants, cond.Get(), &items)) && items &&
        SUCCEEDED(items->get_Length(&found)) && found > 0) {
        return items;
    }
    // No strip, or the wrong one - search the whole window.
    items.Reset();
    if (FAILED(root->FindAll(TreeScope_Descendants, cond.Get(), &items))) {
        items.Reset();
    }
    return items;
}

// anyTitle = false: try only items with the same title as the tab window (identical titles are told
// apart by checking the result); true: try the others too (the title may not match) - this switches
// other tabs one after another, so it is a last resort.
static bool SelectTabOnce(HWND top, HWND tab, bool anyTitle) {
    if (GetActiveTab(top) == tab) {
        return true;
    }
    WCHAR title[256] = {};
    GetWindowTextW(tab, title, ARRAYSIZE(title));

    IUIAutomation* uia = GetUia();
    if (!uia) {
        return false;
    }
    ComPtr<IUIAutomationElementArray> items = FindTabItems(uia, top);
    if (!items) {
        return false;
    }
    int count = 0;
    items->get_Length(&count);
    for (int pass = 0; pass < (anyTitle ? 2 : 1); pass++) {
        for (int i = 0; i < count; i++) {
            ComPtr<IUIAutomationElement> item;
            if (FAILED(items->GetElement(i, &item)) || !item) {
                continue;
            }
            BSTR name = nullptr;
            item->get_CurrentName(&name);
            bool sameTitle = name && wcscmp(name, title) == 0;
            SysFreeString(name);
            if (sameTitle != (pass == 0)) {
                continue;
            }
            ComPtr<IUIAutomationSelectionItemPattern> sel;
            if (FAILED(item->GetCurrentPatternAs(UIA_SelectionItemPatternId, IID_PPV_ARGS(&sel))) || !sel) {
                continue;
            }
            sel->Select();
            for (int t = 0; t < 30; t++) {
                if (GetActiveTab(top) == tab) {
                    return true;
                }
                if (Wait(10)) {
                    return false;
                }
            }
        }
    }
    return false;
}

// A new tab gets its title and its tab-strip item not immediately - retry.
static bool SelectTab(HWND top, HWND tab) {
    // The first pass may select other tabs with the same title, so remember the original one now.
    HWND before = GetActiveTab(top);
    for (DWORD start = GetTickCount(); GetTickCount() - start < 2000 && !g_stop;) {
        if (!IsWindow(tab)) {
            return false;
        }
        if (SelectTabOnce(top, tab, false)) {
            return true;
        }
        if (Wait(50)) {
            return false;
        }
    }
    // Last resort: try tabs with other titles. If that fails too, switch back to the tab that
    // was active before, so the user doesn't end up on a random tab.
    if (g_stop) {
        return false;
    }
    if (SelectTabOnce(top, tab, true)) {
        return true;
    }
    if (before && IsWindow(before) && GetActiveTab(top) != before) {
        SelectTabOnce(top, before, false);
    }
    return false;
}

// ---------------------------------------------------------------------------

static void ActivateWindow(HWND hwnd) {
    if (IsIconic(hwnd)) {
        ShowWindow(hwnd, SW_RESTORE);
    }
    SetForegroundWindow(hwnd);
}

// Looks for a tab already open on the folder; *top is its window.
static HWND FindOpenTab(HWND self, PCIDLIST_ABSOLUTE folder, HWND* top) {
    WindowList windows;
    GetCandidateWindows(self, &windows);
    for (int w = 0; w < windows.count && !g_stop; w++) {
        WindowList tabs;
        GetTabWindows(windows.items[w], &tabs);
        for (int t = 0; t < tabs.count && !g_stop; t++) {
            if (SendTabCommand(tabs.items[t], kTabMatch, folder)) {
                *top = windows.items[w];
                return tabs.items[t];
            }
        }
    }
    return nullptr;
}

static HWND FindNewTab(HWND target, const WindowList& before) {
    WindowList after;
    GetTabWindows(target, &after);
    for (int i = 0; i < after.count; i++) {
        if (!before.Contains(after.items[i])) {
            return after.items[i];
        }
    }
    return nullptr;
}

// Opens the folder as a new tab in window target; returns the tab window.
static HWND OpenInNewTab(HWND target, PCIDLIST_ABSOLUTE folder) {
    WindowList before;
    GetTabWindows(target, &before);

    // Main way - "Open in new tab", as from the context menu.
    HWND active = GetActiveTab(target);
    if (active && SendTabCommand(active, kTabOpenNewTab, folder, nullptr, 3000)) {
        for (DWORD start = GetTickCount(); GetTickCount() - start < 3000;) {
            if (HWND newTab = FindNewTab(target, before)) {
                if (!SelectTab(target, newTab)) {
                    Wh_Log(L"new tab %p opened but not activated", newTab);
                }
                return newTab;
            }
            if (Wait(kPollMs)) {
                return nullptr;
            }
        }
    }
    if (g_stop) {
        return nullptr;
    }
    // The command may have succeeded after the wait above ran out: don't open a second tab then.
    for (DWORD start = GetTickCount(); GetTickCount() - start < 1000;) {
        if (HWND newTab = FindNewTab(target, before)) {
            Wh_Log(L"opennewtab finished late, new tab %p", newTab);
            if (!SelectTab(target, newTab)) {
                Wh_Log(L"new tab %p opened but not activated", newTab);
            }
            return newTab;
        }
        if (Wait(kPollMs)) {
            return nullptr;
        }
    }
    Wh_Log(L"opennewtab failed, falling back to new-tab command");

    // Fallback: an empty tab + navigation in it.
    GetTabWindows(target, &before);
    DWORD_PTR res = 0;
    if (!SendMessageTimeoutW(target, WM_COMMAND, kCmdNewTab, 0, SMTO_ABORTIFHUNG, 2000, &res)) {
        Wh_Log(L"new-tab command timed out");
        return nullptr;
    }

    HWND newTab = nullptr;
    for (DWORD start = GetTickCount(); GetTickCount() - start < 4000;) {
        if (!newTab) {
            newTab = FindNewTab(target, before);
        }
        if (newTab && SendTabCommand(newTab, kTabNavigate, folder)) {
            return newTab;
        }
        if (Wait(100)) {  // the tab is not ready to hand out its IShellBrowser yet
            break;
        }
    }
    return nullptr;
}

// "Show in folder" sometimes selects the file in the new window a bit later than the folder is set.
// The tab is already open and active by then; wait for the selection for another kSelectionGraceMs.
static void CatchLateSelection(HWND self, IShellWindows* sw, HWND tab) {
    for (DWORD start = GetTickCount(); GetTickCount() - start < kSelectionGraceMs && IsWindow(self);) {
        FolderState late;
        bool got = ReadWindowState(sw, self, &late) && late.selectionCount > 0;
        if (got) {
            Wh_Log(L"late selection after tab opened: %d items", late.selectionCount);
            ApplySelection(tab, late);
        }
        late.Clear();
        if (got || Wait(kPollMs)) {
            return;
        }
    }
}

// Moves the new window self into a tab. *target is the window where the tab ended up.
static bool MergeWindowWith(HWND self, IShellWindows* sw, const TabSnapshot& snapshot, HWND* target) {
    FolderState state;
    bool draggedOut = false;
    if (!WaitForFolder(self, sw, snapshot, &state, 4000, &draggedOut)) {
        if (draggedOut) {
            Wh_Log(L"merge: the window is a tab dragged out of another window, leaving it");
        } else {
            Wh_Log(L"merge: folder of new window unknown");
        }
        return false;
    }
    // Explorer destroys the dragged tab ~60 ms after creating the window, well before the folder
    // is ready (~250 ms); check once more right before merging.
    if (g_ignoreMouseDown && IsDraggedOutTab(self, snapshot, true)) {
        Wh_Log(L"merge: the window is a tab dragged out of another window, leaving it");
        state.Clear();
        return false;
    }
    if (IsControlPanelFolder(state.folder)) {
        Wh_Log(L"merge: Control Panel, leaving it as a window");
        state.Clear();
        return false;
    }

    bool ok = false;
    HWND tab = nullptr;
    if (g_reuseOpenTab && (tab = FindOpenTab(self, state.folder, target))) {
        ok = SelectTab(*target, tab);
        Wh_Log(L"merge: folder already open in tab %p, switched %d", tab, ok);
    }
    if (!ok && !g_stop) {
        *target = FindMergeTarget(self, sw);
        tab = *target ? OpenInNewTab(*target, state.folder) : nullptr;
        ok = tab != nullptr;
        Wh_Log(L"merge: new tab %p", tab);
    }
    if (ok) {
        if (g_activateTarget) {
            ActivateWindow(*target);
        }
        if (state.selectionCount) {
            ApplySelection(tab, state);
        } else {
            CatchLateSelection(self, sw, tab);
        }
    }
    state.Clear();
    return ok;
}

// A window created while the mouse button was held (a folder opened by double-click, or a tab being
// dragged out on Explorer builds that create the window during the drag). It is left visible while
// the button is held; a button still held after the double-click time means a drag. After the
// release the window is hidden and merged, and WaitForFolder still recognizes a dragged-out tab.
// Returns true to merge.
static bool CheckMouseDownWindow(HWND self, const TabSnapshot& snapshot) {
    DWORD limit = GetDoubleClickTime();
    for (DWORD start = GetTickCount(); IsPrimaryButtonDown();) {
        if (IsDraggedOutTab(self, snapshot)) {
            Wh_Log(L"window %p is a tab dragged out of another window, leaving it", self);
            return false;
        }
        if (GetTickCount() - start >= limit) {
            Wh_Log(L"mouse button still held: window %p is a dragged-out tab, leaving it", self);
            return false;
        }
        if (Wait(kPollMs)) {
            return false;
        }
    }
    if (!IsWindow(self)) {
        return false;
    }
    if (IsWindowVisible(self) && !IsCloaked(self)) {
        HideWindowAsync(self);
    } else {
        CloakWindow(self, true);
    }
    return true;
}

static void MergeNewWindow(HWND self, const TabSnapshot& snapshot) {
    if (snapshot.mouseDown && !CheckMouseDownWindow(self, snapshot)) {
        return;  // stays claimed: a regular window, leave it alone
    }
    DWORD start = GetTickCount();
    HWND target = nullptr;
    bool ok = false;
    if (IsWindow(self)) {
        if (ComPtr<IShellWindows> sw = CreateShellWindows()) {
            ok = MergeWindowWith(self, sw.Get(), snapshot, &target);
        }
        ReleaseTabHooks();
    }
    Wh_Log(L"merge %s in %lu ms", ok ? L"done" : L"failed", GetTickCount() - start);

    if (ok) {
        PostMessageW(self, WM_CLOSE, 0, 0);
        Unclaim(self);
    } else {
        ShowWindowBack(self);  // stays in g_claimed so that we do not try again
    }
}

// ---------------------------------------------------------------------------
// "Show in folder" when the folder is already open. The program (via SHOpenFolderAndSelectItems)
// finds the window in ShellWindows and calls IShellView::SelectItem on the tab - an incoming
// cross-process COM call. Explorer does not switch tabs then, and sometimes
// hands out the wrong tab of the window (the file is not selected at all). We intercept SelectItem:
// - the file was selected in an inactive tab - switch to it;
// - the file is not in this tab - find the window tab that has it, select it there and switch.

static const int kSelectItemSlot = 14;      // IShellView::SelectItem in the vtable
static const DWORD kRedirectKeepMs = 2000;  // the following items of the same call go to the same tab

using SelectItem_t = HRESULT STDMETHODCALLTYPE(IShellView*, PCUITEMID_CHILD, SVSIF);
static SelectItem_t* SelectItem_Original;

static volatile LONG g_switching = 0;

// Last redirect: from tab "from" to "to" (several items arrive as separate calls).
// Every Explorer window has its own UI thread and redirects only between its own tabs.
struct Redirect {
    HWND from;
    HWND to;
    DWORD at;
};
static thread_local Redirect g_redirect;

static HWND GetTabOf(HWND hwnd) {
    for (; hwnd; hwnd = GetParent(hwnd)) {
        if (IsClass(hwnd, L"ShellTabWindowClass")) {
            return hwnd;
        }
    }
    return nullptr;
}

static HWND GetViewTab(IShellView* sv) {
    HWND hwnd = nullptr;
    return SUCCEEDED(sv->GetWindow(&hwnd)) ? GetTabOf(hwnd) : nullptr;
}

static bool IsIncomingComCall() {
    ComPtr<IServerSecurity> security;
    return SUCCEEDED(CoGetCallContext(IID_PPV_ARGS(&security))) && security;
}

// Whether the view shows a file system folder. The item IDs that programs pass to SelectItem are
// file system IDs; handing them to another kind of folder (This PC, Control Panel, network, archives)
// makes that folder parse an ID of a foreign format, which some implementations don't survive.
// An archive (.zip, .cab) is a file on disk, so it also has SFGAO_FILESYSTEM, but with SFGAO_STREAM.
static bool IsFileSystemView(IShellView* sv) {
    ComPtr<IFolderView> fv;
    if (FAILED(sv->QueryInterface(IID_PPV_ARGS(&fv)))) {
        return false;
    }
    PIDLIST_ABSOLUTE pidl = GetViewFolder(fv.Get());
    if (!pidl) {
        return false;
    }
    ComPtr<IShellItem> item;
    SFGAOF attrs = 0;
    bool fileSystem = SUCCEEDED(SHCreateItemFromIDList(pidl, IID_PPV_ARGS(&item))) &&
                      SUCCEEDED(item->GetAttributes(SFGAO_FILESYSTEM | SFGAO_STREAM, &attrs)) &&
                      (attrs & (SFGAO_FILESYSTEM | SFGAO_STREAM)) == SFGAO_FILESYSTEM;
    ILFree(pidl);
    return fileSystem;
}

// Whether item child is selected in the view. Only for file system views (see IsFileSystemView).
static bool IsItemSelected(IShellView* sv, PCUITEMID_CHILD child) {
    ComPtr<IFolderView> fv;
    ComPtr<IShellFolder> folder;
    ComPtr<IEnumIDList> items;
    if (FAILED(sv->QueryInterface(IID_PPV_ARGS(&fv))) || FAILED(fv->GetFolder(IID_PPV_ARGS(&folder))) ||
        FAILED(fv->Items(SVGIO_SELECTION, IID_PPV_ARGS(&items))) || !items) {
        return false;
    }
    PITEMID_CHILD item = nullptr;
    ULONG fetched = 0;
    while (items->Next(1, &item, &fetched) == S_OK && item) {
        HRESULT hr = folder->CompareIDs(0, item, child);
        ILFree(item);
        item = nullptr;
        if (SUCCEEDED(hr) && HRESULT_CODE(hr) == 0) {
            return true;
        }
    }
    return false;
}

// Active view of the tab; the tab is in the same thread as the caller - a direct call.
static ComPtr<IShellView> GetTabView(HWND tab) {
    auto* sb = reinterpret_cast<IShellBrowser*>(SendMessageW(tab, kCwmGetIShellBrowser, 0, 0));
    ComPtr<IShellView> sv;
    if (sb && FAILED(sb->QueryActiveShellView(&sv))) {
        sv.Reset();
    }
    return sv;
}

// Switches to the tab where a program selected a file, and brings its window to the front: when the
// tab is already active but the window is behind the program, "Show in folder" would otherwise seem
// to do nothing.
static void SwitchTab(HWND tab) {
    if (Wait(50)) {  // let the SelectItem call finish
        return;
    }
    HWND top = GetAncestor(tab, GA_ROOT);
    if (!IsWindow(tab)) {
        return;
    }
    bool ok = true;
    if (GetActiveTab(top) != tab) {
        ok = SelectTab(top, tab);
        Wh_Log(L"show-in-folder: switched to tab %p: %d", tab, ok);
    }
    if (ok && g_activateTarget && (GetForegroundWindow() != top || IsIconic(top))) {
        ActivateWindow(top);
        Wh_Log(L"show-in-folder: window %p brought to the front", top);
    }
}

// UI Automation must not be called from the window thread in the middle of a COM call - switch
// from the worker thread.
static void SwitchToTabLater(HWND tab) {
    if (InterlockedCompareExchange(&g_switching, 1, 0) != 0) {
        return;
    }
    if (!PostThreadMessageW(g_workerId, kWorkerSwitchTab, reinterpret_cast<WPARAM>(tab), 0)) {
        InterlockedExchange(&g_switching, 0);
    }
}

static HRESULT STDMETHODCALLTYPE SelectItem_Hook(IShellView* self, PCUITEMID_CHILD child, SVSIF flags) {
    if (!g_activateSelectedTab || g_stop || !child || !(flags & SVSI_SELECT) || !IsIncomingComCall()) {
        return SelectItem_Original(self, child, flags);
    }
    HWND tab = GetViewTab(self);
    HWND top = tab ? GetAncestor(tab, GA_ROOT) : nullptr;
    // A cloaked window is a new window being merged: "Show in folder" selects the file there, and
    // the merge carries the selection over.
    if (!top || !IsClass(top, L"CabinetWClass") || IsCloaked(top)) {
        return SelectItem_Original(self, child, flags);
    }

    // Continuation of an already redirected call (several items being selected).
    if (!(flags & SVSI_DESELECTOTHERS) && g_redirect.from == tab && GetTickCount() - g_redirect.at < kRedirectKeepMs &&
        IsWindow(g_redirect.to)) {
        if (ComPtr<IShellView> sv = GetTabView(g_redirect.to)) {
            return SelectItem_Original(sv.Get(), child, flags);
        }
    }

    HRESULT hr = SelectItem_Original(self, child, flags);
    if (!IsFileSystemView(self)) {
        return hr;
    }
    if (IsItemSelected(self, child)) {
        Wh_Log(L"show-in-folder: item selected in tab %p", tab);
        SwitchToTabLater(tab);
        return hr;
    }

    // Wrong tab: look for the tab of this window that has the item. Only file system tabs are
    // probed. Probing selects the item without touching the rest of the selection, and is undone
    // in the tabs that aren't used. An item with the same name can exist in several folders; then
    // the target is ambiguous and nothing is redirected.
    WindowList tabs;
    GetTabWindows(top, &tabs);
    HWND match = nullptr;
    int matches = 0;
    for (int i = 0; i < tabs.count; i++) {
        if (tabs.items[i] == tab) {
            continue;
        }
        ComPtr<IShellView> sv = GetTabView(tabs.items[i]);
        if (!sv || !IsFileSystemView(sv.Get())) {
            continue;
        }
        bool found = IsItemSelected(sv.Get(), child);
        if (!found) {
            SelectItem_Original(sv.Get(), child, SVSI_SELECT);
            found = IsItemSelected(sv.Get(), child);
            if (found) {
                SelectItem_Original(sv.Get(), child, SVSI_DESELECT);
            }
        }
        if (found) {
            matches++;
            match = tabs.items[i];
        }
    }
    if (matches != 1) {
        if (matches > 1) {
            Wh_Log(L"show-in-folder: item found in %d tabs, not redirecting", matches);
        }
        return hr;
    }
    ComPtr<IShellView> sv = GetTabView(match);
    if (!sv) {
        return hr;
    }
    Wh_Log(L"show-in-folder: wrong tab %p, item found in tab %p", tab, match);
    hr = SelectItem_Original(sv.Get(), child, flags);
    g_redirect = {tab, match, GetTickCount()};
    SwitchToTabLater(match);
    return hr;
}

static bool HookSelectItem() {
    ComPtr<IShellFolder> desktop;
    if (FAILED(SHGetDesktopFolder(&desktop)) || !desktop) {
        return false;
    }
    SFV_CREATE create = {sizeof(create), desktop.Get(), nullptr, nullptr};
    ComPtr<IShellView> view;
    HRESULT hr = SHCreateShellFolderView(&create, &view);
    if (FAILED(hr) || !view) {
        Wh_Log(L"SHCreateShellFolderView failed: 0x%08X", static_cast<unsigned>(hr));
        return false;
    }
    // The vtable is shared by all Explorer views.
    void** vtable = *reinterpret_cast<void***>(view.Get());
    return WindhawkUtils::SetFunctionHook(reinterpret_cast<SelectItem_t*>(vtable[kSelectItemSlot]), SelectItem_Hook,
                                          &SelectItem_Original);
}

// ---------------------------------------------------------------------------
// New Explorer windows.

// Called on the thread that created the window, right after creation - before the first paint.
static void HandleNewWindow(HWND hwnd) {
    if (g_stop || GetAncestor(hwnd, GA_ROOT) != hwnd || !Claim(hwnd)) {
        return;
    }
    if (g_shiftBypass && (GetAsyncKeyState(VK_SHIFT) & 0x8000)) {
        Wh_Log(L"new Explorer window %p: Shift held, leaving it as a window", hwnd);
        return;  // stays claimed: a regular window, leave it alone
    }
    if (!FindTarget(hwnd)) {
        return;  // stays claimed: a regular window, leave it alone
    }
    bool mouseDown = g_ignoreMouseDown && IsPrimaryButtonDown();
    Wh_Log(L"new Explorer window %p, merging%s", hwnd, mouseDown ? L" (mouse button held)" : L"");
    TabSnapshot* snapshot = TakeTabSnapshot(hwnd, mouseDown);
    if (!mouseDown) {
        CloakWindow(hwnd, true);
    }
    if (!PostThreadMessageW(g_workerId, kWorkerMerge, reinterpret_cast<WPARAM>(hwnd),
                            reinterpret_cast<LPARAM>(snapshot))) {
        delete snapshot;
        if (!mouseDown) {
            CloakWindow(hwnd, false);
        }
    }
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
static CreateWindowExW_t CreateWindowExW_Original;

static HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle,
                                        int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu,
                                        HINSTANCE hInstance, LPVOID lpParam) {
    HWND hwnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);
    if (hwnd && !(dwStyle & WS_CHILD) &&
        (IS_INTRESOURCE(lpClassName) || (lpClassName && wcscmp(lpClassName, L"CabinetWClass") == 0)) &&
        IsClass(hwnd, L"CabinetWClass")) {
        HandleNewWindow(hwnd);
    }
    return hwnd;
}

// ---------------------------------------------------------------------------
// Worker thread: merges and tab switches, one at a time.

static DWORD WINAPI WorkerThread(LPVOID) {
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);  // create the message queue
    SetEvent(g_workerReady);
    bool coInit = SUCCEEDED(CoInitializeEx(nullptr, COINIT_MULTITHREADED));

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == kWorkerMerge) {
            HWND hwnd = reinterpret_cast<HWND>(msg.wParam);
            auto* snapshot = reinterpret_cast<TabSnapshot*>(msg.lParam);
            if (g_stop) {
                ShowWindowBack(hwnd);
            } else {
                MergeNewWindow(hwnd, *snapshot);
            }
            delete snapshot;
        } else if (msg.message == kWorkerSwitchTab) {
            if (!g_stop) {
                SwitchTab(reinterpret_cast<HWND>(msg.wParam));
            }
            InterlockedExchange(&g_switching, 0);
        }
    }
    // Windows still waiting for a merge are restored.
    while (PeekMessageW(&msg, nullptr, kWorkerMerge, kWorkerMerge, PM_REMOVE)) {
        ShowWindowBack(reinterpret_cast<HWND>(msg.wParam));
        delete reinterpret_cast<TabSnapshot*>(msg.lParam);
    }

    ReleaseTabHooks();
    if (g_uia) {
        g_uia->Release();
        g_uia = nullptr;
    }
    if (coInit) {
        CoUninitialize();
    }
    return 0;
}

// Explorer tabs exist on Windows 11 22H2 (build 22621) and later.
static bool IsSupportedBuild() {
    using RtlGetVersion_t = LONG(WINAPI*)(PRTL_OSVERSIONINFOW);
    auto rtlGetVersion = reinterpret_cast<RtlGetVersion_t>(
        reinterpret_cast<void*>(GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "RtlGetVersion")));
    RTL_OSVERSIONINFOW vi = {};
    vi.dwOSVersionInfoSize = sizeof(vi);
    if (!rtlGetVersion || rtlGetVersion(&vi) != 0) {
        return true;  // unknown - don't block
    }
    return vi.dwBuildNumber >= 22621;
}

BOOL Wh_ModInit() {
    if (!IsSupportedBuild()) {
        Wh_Log(L"Windows build without Explorer tabs, the mod is not loaded");
        return FALSE;
    }
    LoadSettings();

    g_tabCommandMsg = RegisterWindowMessageW(L"Windhawk_" WH_MOD_ID L"_TabCommand");
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_workerReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_tabCommandMsg || !g_stopEvent || !g_workerReady) {
        return FALSE;
    }
    g_stop = 0;
    g_worker = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, &g_workerId);
    if (!g_worker) {
        return FALSE;
    }
    // After this PostThreadMessage to the worker can't fail for lack of a message queue.
    WaitForSingleObject(g_workerReady, INFINITE);

    if (!HookSelectItem()) {
        Wh_Log(L"IShellView::SelectItem not hooked; show-in-folder tab switching disabled");
    }
    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook, &CreateWindowExW_Original);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModUninit() {
    // Windhawk has already removed the function hooks, so no new work can arrive.
    InterlockedExchange(&g_stop, 1);
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }
    if (g_worker) {
        PostThreadMessageW(g_workerId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_worker, INFINITE);  // releases its tab hooks and IUIAutomation before exiting
        CloseHandle(g_worker);
        g_worker = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
    if (g_workerReady) {
        CloseHandle(g_workerReady);
        g_workerReady = nullptr;
    }
}
