// ==WindhawkMod==
// @id              explorer-windows-to-tabs
// @name            Explorer: open new windows as tabs
// @name:ru-RU      Проводник: новые окна — во вкладки
// @description     A new File Explorer window opens as a tab in an already open window; a folder that is already open just becomes the active tab
// @description:ru-RU Новое окно Проводника открывается вкладкой в уже открытом окне; уже открытая папка просто становится активной вкладкой
// @version         1.5
// @author          limpompo
// @github          https://github.com/limpompo
// @homepage        https://github.com/limpompo/windhawk-mods-limpompo/tree/main/mods/explorer-windows-to-tabs
// @license         MIT
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -lshell32 -ldwmapi -luser32
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
- a window created while the left mouse button is held (a tab dragged out into its own window);
- windows on another virtual desktop.

Works only on Windows 11 with tabs in File Explorer (22H2 and later). The "new tab"
command (WM_COMMAND 0xA21B) is undocumented and may change in future builds.
System files are not modified.
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
  $description: When "Show in folder" targets an already open folder, Explorer selects the file in its tab but does not switch to it (and sometimes picks the wrong tab). The mod switches to the right one.
  $description:ru-RU: «Показать в папке» при уже открытой папке Проводник выделяет файл в её вкладке, но не переключается на неё (а иногда выбирает не ту вкладку). Мод переключает на нужную.
- activateTarget: true
  $name: Bring the tabbed window to the foreground
  $name:ru-RU: Выводить окно со вкладками на передний план
  $description: After adding or selecting a tab, activate the Explorer window (and restore it if minimized).
  $description:ru-RU: После добавления или выбора вкладки активировать окно Проводника (и разворачивать, если оно свёрнуто).
- ignoreMouseDown: true
  $name: Leave windows created with a mouse button held
  $name:ru-RU: Не трогать окна, созданные при зажатой кнопке мыши
  $description: Protects against merging back a tab that was dragged out into its own window.
  $description:ru-RU: Защита от «сливания» обратно вкладки, которую вытащили в отдельное окно.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shlobj.h>
#include <exdisp.h>
#include <servprov.h>
#include <dwmapi.h>
#include <uiautomation.h>

static const UINT kCmdNewTab = 0xA21B;                 // WM_COMMAND of the CabinetWClass window: "new tab"
static const UINT kCwmGetIShellBrowser = WM_USER + 7;  // the Explorer browser window returns its IShellBrowser
static const WCHAR kTabDoneEvent[] = L"Local\\ExplorerWindowsToTabs_TabDone";

// Commands to a tab via WM_COPYDATA (executed by a hook on the tab thread).
enum : ULONG_PTR {
    kTabNavigate = 0x4E545745,  // data: folder PIDL; navigate to it
    kTabMatch = 0x4D545745,     // data: folder PIDL; "yes" if the tab is open on it
    kTabSelect = 0x53545745,    // data: folder + child PIDLs; select the items
    kTabOpenNewTab = 0x4F545745,  // data: folder PIDL; "Open in new tab" from this tab
};

static const GUID kClsidShellWindows = {0x9BA05972, 0xF6A8, 0x11CF, {0xA4, 0x42, 0x00, 0xA0, 0xC9, 0x0A, 0x8F, 0x39}};
static const GUID kIidShellWindows = {0x85CB6900, 0x4D95, 0x11CF, {0x96, 0x0C, 0x00, 0x80, 0xC7, 0xF4, 0xEE, 0x85}};
static const GUID kIidWebBrowser2 = {0xD30C1661, 0xCDAF, 0x11D0, {0x8A, 0x3E, 0x00, 0xC0, 0x4F, 0xC9, 0xE2, 0x6E}};
static const GUID kIidServiceProvider = {0x6D5140C1, 0x7436, 0x11CE, {0x80, 0x34, 0x00, 0xAA, 0x00, 0x60, 0x09, 0xFA}};
static const GUID kSidTopLevelBrowser = {0x4C96BE40, 0x915C, 0x11CF, {0x99, 0xD3, 0x00, 0xAA, 0x00, 0x4A, 0xE8, 0x37}};
static const GUID kIidShellBrowser = {0x000214E2, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
static const GUID kIidFolderView = {0xCDE725B0, 0xCCC9, 0x4519, {0x91, 0x7E, 0x32, 0x5D, 0x72, 0xFA, 0xB4, 0xCE}};
static const GUID kIidShellFolder = {0x000214E6, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
static const GUID kIidContextMenu = {0x000214E4, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
static const GUID kIidObjectWithSite = {0xFC4801A3, 0x2BA9, 0x11CF, {0xA2, 0x29, 0x00, 0xAA, 0x00, 0x3D, 0x73, 0x52}};
static const GUID kIidFolderView2 ={0x1AF3A467, 0x214F, 0x4298, {0x90, 0x8E, 0x06, 0xB0, 0x3E, 0x0B, 0x39, 0xF9}};
static const GUID kIidPersistFolder2 ={0x1AC3D9F0, 0x175C, 0x11D1, {0x95, 0xBE, 0x00, 0x60, 0x97, 0x97, 0xEA, 0x4F}};
static const GUID kClsidCUIAutomation = {0xFF48DBA4, 0x60EF, 0x4201, {0xAA, 0x87, 0x54, 0x10, 0x3E, 0xEF, 0x59, 0x4E}};
static const GUID kIidUIAutomation = {0x30CBE57D, 0xD9D0, 0x452A, {0xAB, 0x13, 0x7A, 0xC5, 0xAC, 0x48, 0x25, 0xEE}};
static const GUID kIidUIASelectionItem = {0xA8EFA66A, 0x0FDA, 0x421A, {0x91, 0x94, 0x38, 0x02, 0x1F, 0x35, 0x78, 0xEA}};

static const int kMaxWindows = 64;
static const int kMaxSelection = 64;
static const DWORD kSelectionGraceMs = 300;  // how long to wait for a late selection after a tab is opened
static const DWORD kPollMs = 15;             // polling step for a new window

static bool g_reuseOpenTab = true;
static bool g_activateSelectedTab = true;
static bool g_activateTarget = true;
static bool g_ignoreMouseDown = true;

static volatile LONG g_stop = 0;
static volatile LONG g_activeMerges = 0;
static HANDLE g_hookThread = nullptr;
static DWORD g_hookThreadId = 0;
static HMODULE g_module = nullptr;

static CRITICAL_SECTION g_cs;         // serializes merges (held for seconds)
static CRITICAL_SECTION g_claimLock;  // protects g_claimed (also taken from the window-creating thread)
static HWND g_claimed[kMaxWindows];  // windows the mod already handled (in progress or left as is)
static int g_claimedCount = 0;

static void LoadSettings() {
    g_reuseOpenTab = Wh_GetIntSetting(L"reuseOpenTab") != 0;
    g_activateSelectedTab = Wh_GetIntSetting(L"activateSelectedTab") != 0;
    g_activateTarget = Wh_GetIntSetting(L"activateTarget") != 0;
    g_ignoreMouseDown = Wh_GetIntSetting(L"ignoreMouseDown") != 0;
}

static bool IsClass(HWND hwnd, PCWSTR cls) {
    WCHAR buf[64];
    return GetClassNameW(hwnd, buf, ARRAYSIZE(buf)) > 0 && wcscmp(buf, cls) == 0;
}

static bool IsCloaked(HWND hwnd) {
    DWORD cloaked = 0;
    return SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) && cloaked;
}

// Returns true if the window was just added (not claimed before).
static bool Claim(HWND hwnd) {
    bool added = true;
    EnterCriticalSection(&g_claimLock);
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
    LeaveCriticalSection(&g_claimLock);
    return added;
}

static void Unclaim(HWND hwnd) {
    EnterCriticalSection(&g_claimLock);
    for (int i = 0; i < g_claimedCount; i++) {
        if (g_claimed[i] == hwnd) {
            g_claimed[i] = g_claimed[--g_claimedCount];
            break;
        }
    }
    LeaveCriticalSection(&g_claimLock);
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

// An Explorer window that can be merged into: visible, on the current desktop, not self.
static bool IsCandidateWindow(HWND hwnd, HWND self) {
#ifdef WH_TEST_ALL_PROCESSES
    if (TestIgnoresWindow(hwnd)) {
        return false;
    }
#endif
    return hwnd != self && IsWindowVisible(hwnd) && IsClass(hwnd, L"CabinetWClass") && !IsCloaked(hwnd);
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
    IPersistFolder2* pf = nullptr;
    if (SUCCEEDED(fv->GetFolder(kIidPersistFolder2, reinterpret_cast<void**>(&pf))) && pf) {
        pf->GetCurFolder(&pidl);
        pf->Release();
    }
    return pidl;
}

// Current folder of the browser (absolute PIDL, free with ILFree). Optionally IFolderView.
static PIDLIST_ABSOLUTE GetBrowserFolder(IShellBrowser* sb, IFolderView** folderView) {
    PIDLIST_ABSOLUTE pidl = nullptr;
    IShellView* sv = nullptr;
    if (SUCCEEDED(sb->QueryActiveShellView(&sv)) && sv) {
        IFolderView* fv = nullptr;
        if (SUCCEEDED(sv->QueryInterface(kIidFolderView, reinterpret_cast<void**>(&fv))) && fv) {
            pidl = GetViewFolder(fv);
            if (folderView && pidl) {
                *folderView = fv;
                fv = nullptr;
            }
            if (fv) {
                fv->Release();
            }
        }
        sv->Release();
    }
    return pidl;
}

static IShellBrowser* GetShellBrowser(IWebBrowser2* wb) {
    IServiceProvider* sp = nullptr;
    IShellBrowser* sb = nullptr;
    if (SUCCEEDED(wb->QueryInterface(kIidServiceProvider, reinterpret_cast<void**>(&sp))) && sp) {
        sp->QueryService(kSidTopLevelBrowser, kIidShellBrowser, reinterpret_cast<void**>(&sb));
        sp->Release();
    }
    return sb;
}

// Reads the new window's folder and selection via ShellWindows (the window is its own, one tab).
static IShellWindows* CreateShellWindows() {
    IShellWindows* sw = nullptr;
    if (FAILED(CoCreateInstance(kClsidShellWindows, nullptr, CLSCTX_ALL, kIidShellWindows,
                                reinterpret_cast<void**>(&sw)))) {
        sw = nullptr;
    }
    return sw;
}

static bool ReadWindowState(IShellWindows* sw, HWND top, FolderState* state) {
    long count = 0;
    sw->get_Count(&count);
    for (long i = 0; i < count && !state->folder; i++) {
        VARIANT index;
        VariantInit(&index);
        index.vt = VT_I4;
        index.lVal = i;

        IDispatch* disp = nullptr;
        if (FAILED(sw->Item(index, &disp)) || !disp) {
            continue;
        }
        IWebBrowser2* wb = nullptr;
        SHANDLE_PTR h = 0;
        if (SUCCEEDED(disp->QueryInterface(kIidWebBrowser2, reinterpret_cast<void**>(&wb))) && wb &&
            SUCCEEDED(wb->get_HWND(&h)) && reinterpret_cast<HWND>(h) == top) {
            if (IShellBrowser* sb = GetShellBrowser(wb)) {
                IFolderView* fv = nullptr;
                state->folder = GetBrowserFolder(sb, &fv);
                IFolderView2* fv2 = nullptr;
                if (fv && SUCCEEDED(fv->QueryInterface(kIidFolderView2, reinterpret_cast<void**>(&fv2))) && fv2) {
                    IShellItemArray* items = nullptr;
                    if (fv2->GetSelection(FALSE, &items) == S_OK && items) {
                        DWORD n = 0;
                        items->GetCount(&n);
                        for (DWORD i = 0; i < n && state->selectionCount < kMaxSelection; i++) {
                            IShellItem* item = nullptr;
                            PIDLIST_ABSOLUTE abs = nullptr;
                            if (SUCCEEDED(items->GetItemAt(i, &item)) && item &&
                                SUCCEEDED(SHGetIDListFromObject(item, &abs)) && abs) {
                                state->selection[state->selectionCount++] =
                                    reinterpret_cast<PITEMID_CHILD>(ILClone(ILFindLastID(abs)));
                                ILFree(abs);
                            }
                            if (item) {
                                item->Release();
                            }
                        }
                        items->Release();
                    }
                    fv2->Release();
                }
                if (fv) {
                    fv->Release();
                }
                sb->Release();
            }
        }
        if (wb) {
            wb->Release();
        }
        disp->Release();
    }
    return state->folder != nullptr;
}

// Waits until the folder is set in the new window (two identical checks in a row). The selection
// ("Show in folder") may arrive a bit later - CatchLateSelection picks it up after the tab is
// opened, so that it does not delay every merge.
static bool WaitForFolder(HWND self, IShellWindows* sw, FolderState* result, DWORD timeoutMs) {
    FolderState prev;
    DWORD start = GetTickCount();
    // A window caught at creation (cloaked) is merged only if Explorer showed it:
    // service invisible windows are left alone. A window caught on show was already visible.
    bool seenVisible = !IsCloaked(self);
    while (GetTickCount() - start < timeoutMs && !g_stop && IsWindow(self)) {
        if (IsWindowVisible(self)) {
            seenVisible = true;
            if (!IsCloaked(self)) {
                HideWindowAsync(self);  // caught on show, and Explorer showed it again
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
        Sleep(kPollMs);
    }
    prev.Clear();
    return false;
}

// ---------------------------------------------------------------------------
// Commands to a tab. An empty new tab is not visible via ShellWindows, and inactive
// tabs are not always listed there, so everything is done inside the tab thread:
// a WH_CALLWNDPROC hook catches WM_COPYDATA and works with the tab's IShellBrowser.
// The hook reports success via the kTabDoneEvent event.

struct Buffer {
    BYTE* data = nullptr;
    UINT size = 0;

    void Append(const void* p, UINT n) {
        BYTE* d = static_cast<BYTE*>(data ? HeapReAlloc(GetProcessHeap(), 0, data, size + n)
                                          : HeapAlloc(GetProcessHeap(), 0, n));
        if (!d) {
            return;
        }
        memcpy(d + size, p, n);
        data = d;
        size += n;
    }
    void AppendIdList(LPCITEMIDLIST pidl) {
        UINT n = ILGetSize(pidl);
        Append(&n, sizeof(n));
        Append(pidl, n);
    }
    ~Buffer() {
        if (data) {
            HeapFree(GetProcessHeap(), 0, data);
        }
    }
};

// Reads a PIDL written by AppendIdList; nullptr when the data ends.
static LPCITEMIDLIST ReadIdList(const BYTE*& p, const BYTE* end) {
    UINT n = 0;
    if (end - p < static_cast<ptrdiff_t>(sizeof(n))) {
        return nullptr;
    }
    memcpy(&n, p, sizeof(n));
    p += sizeof(n);
    if (n < sizeof(USHORT) || end - p < static_cast<ptrdiff_t>(n)) {
        return nullptr;
    }
    auto pidl = reinterpret_cast<LPCITEMIDLIST>(p);
    p += n;
    return pidl;
}

// The folder command "Open in new tab" (opennewtab) with tab sb as the site - the same
// as from the context menu. A tab opened this way is registered correctly in ShellWindows;
// after BrowseObject in an empty tab the registration gets confused, and "Show in folder"
// then lands in the wrong tab.
static bool InvokeOpenNewTab(IShellBrowser* sb, HWND hwnd, PCIDLIST_ABSOLUTE folder) {
    IShellFolder* parent = nullptr;
    PCUITEMID_CHILD child = nullptr;
    if (FAILED(SHBindToParent(folder, kIidShellFolder, reinterpret_cast<void**>(&parent), &child)) || !parent) {
        return false;
    }
    bool ok = false;
    IContextMenu* menu = nullptr;
    if (SUCCEEDED(parent->GetUIObjectOf(hwnd, 1, &child, kIidContextMenu, nullptr, reinterpret_cast<void**>(&menu))) &&
        menu) {
        IObjectWithSite* site = nullptr;
        if (SUCCEEDED(menu->QueryInterface(kIidObjectWithSite, reinterpret_cast<void**>(&site))) && site) {
            site->SetSite(sb);
        }
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
            site->Release();
        }
        menu->Release();
    }
    parent->Release();
    return ok;
}

static bool RunTabCommand(IShellBrowser* sb, HWND hwnd, ULONG_PTR command, const BYTE* data, UINT size) {
    const BYTE* p = data;
    const BYTE* end = data + size;
    auto folder = reinterpret_cast<PCIDLIST_ABSOLUTE>(ReadIdList(p, end));
    if (!folder) {
        return false;
    }

    if (command == kTabNavigate) {
        return SUCCEEDED(sb->BrowseObject(folder, SBSP_SAMEBROWSER | SBSP_ABSOLUTE));
    }
    if (command == kTabOpenNewTab) {
        return InvokeOpenNewTab(sb, hwnd, folder);
    }

    IFolderView* fv = nullptr;
    PIDLIST_ABSOLUTE cur = GetBrowserFolder(sb, command == kTabSelect ? &fv : nullptr);
    bool same = cur && ILIsEqual(cur, folder);
    if (cur) {
        ILFree(cur);
    }
    if (command == kTabMatch || !same) {
        if (fv) {
            fv->Release();
        }
        return same;
    }

    // kTabSelect: the folder matches, select the items.
    IShellView* sv = nullptr;
    bool done = false;
    if (SUCCEEDED(sb->QueryActiveShellView(&sv)) && sv) {
        int requested = 0;
        while (auto child = reinterpret_cast<PCUITEMID_CHILD>(ReadIdList(p, end))) {
            UINT flags = SVSI_SELECT;
            if (!requested) {
                flags |= SVSI_DESELECTOTHERS | SVSI_ENSUREVISIBLE | SVSI_FOCUSED;
            }
            sv->SelectItem(child, flags);
            requested++;
        }
        // Until the folder is enumerated, SelectItem selects nothing - retry then.
        int selected = 0;
        done = fv && SUCCEEDED(fv->ItemCount(SVGIO_SELECTION, &selected)) && selected == requested;
        sv->Release();
    }
    if (fv) {
        fv->Release();
    }
    return done;
}

static LRESULT CALLBACK TabHookProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION) {
        auto* cwp = reinterpret_cast<CWPSTRUCT*>(lParam);
        auto* cds = cwp->message == WM_COPYDATA ? reinterpret_cast<COPYDATASTRUCT*>(cwp->lParam) : nullptr;
        if (cds &&
            (cds->dwData == kTabNavigate || cds->dwData == kTabMatch || cds->dwData == kTabSelect ||
             cds->dwData == kTabOpenNewTab) &&
            cds->lpData) {
            auto* sb = reinterpret_cast<IShellBrowser*>(SendMessageW(cwp->hwnd, kCwmGetIShellBrowser, 0, 0));
            const BYTE* data = static_cast<const BYTE*>(cds->lpData);
            BYTE* copy = nullptr;
            if (sb && cds->dwData == kTabOpenNewTab) {
                // The command makes outgoing COM calls, which are not allowed inside SendMessage
                // (RPC_E_CANTCALLOUT_ININPUTSYNCCALL): copy the data and release the sender.
                copy = static_cast<BYTE*>(HeapAlloc(GetProcessHeap(), 0, cds->cbData));
                if (copy) {
                    memcpy(copy, data, cds->cbData);
                    data = copy;
                    ReplyMessage(TRUE);
                } else {
                    sb = nullptr;
                }
            }
            if (sb && RunTabCommand(sb, cwp->hwnd, cds->dwData, data, cds->cbData)) {
                if (HANDLE ev = OpenEventW(EVENT_MODIFY_STATE, FALSE, kTabDoneEvent)) {
                    SetEvent(ev);
                    CloseHandle(ev);
                }
            }
            if (copy) {
                HeapFree(GetProcessHeap(), 0, copy);
            }
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

// Command hooks installed during one merge: one per tab-window thread, removed at the end of
// the merge (ReleaseTabHooks). Installing a hook per command is expensive - when searching for
// an open folder the command goes to every tab. Only under g_cs.
static struct {
    DWORD thread;
    HHOOK hook;
} g_tabHooks[16];
static int g_tabHookCount = 0;
static HANDLE g_tabDone = nullptr;  // the kTabDoneEvent event, created in Wh_ModInit

static void ReleaseTabHooks() {
    for (int i = 0; i < g_tabHookCount; i++) {
        UnhookWindowsHookEx(g_tabHooks[i].hook);
    }
    g_tabHookCount = 0;
}

// Hook for a thread; *temporary - remove right after the command (cache overflow).
static HHOOK GetTabHook(DWORD thread, bool* temporary) {
    *temporary = false;
    for (int i = 0; i < g_tabHookCount; i++) {
        if (g_tabHooks[i].thread == thread) {
            return g_tabHooks[i].hook;
        }
    }
    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, TabHookProc, g_module, thread);
    if (!hook) {
        Wh_Log(L"SetWindowsHookEx failed: %lu", GetLastError());
    } else if (g_tabHookCount < ARRAYSIZE(g_tabHooks)) {
        g_tabHooks[g_tabHookCount++] = {thread, hook};
    } else {
        *temporary = true;
    }
    return hook;
}

// Executes a command in tab "tab". true - the hook reported success (wait up to waitMs after the reply).
static bool SendTabCommand(HWND tab, ULONG_PTR command, const Buffer& payload, DWORD waitMs = 0) {
    if (!g_tabDone) {
        return false;
    }
    ResetEvent(g_tabDone);

    bool ok = false;
    bool temporary;
    HHOOK hook = GetTabHook(GetWindowThreadProcessId(tab, nullptr), &temporary);
    if (hook) {
        COPYDATASTRUCT cds = {command, payload.size, payload.data};
        DWORD_PTR res = 0;
        if (SendMessageTimeoutW(tab, WM_COPYDATA, 0, reinterpret_cast<LPARAM>(&cds), SMTO_ABORTIFHUNG, 3000, &res)) {
            ok = WaitForSingleObject(g_tabDone, waitMs) == WAIT_OBJECT_0;
        }
        if (temporary) {
            UnhookWindowsHookEx(hook);
        }
    }
    return ok;
}

// Selects items in a tab; waits until the tab loads the folder.
static bool ApplySelection(HWND tab, const FolderState& state) {
    if (!state.selectionCount) {
        return true;
    }
    Buffer payload;
    payload.AppendIdList(state.folder);
    for (int i = 0; i < state.selectionCount; i++) {
        payload.AppendIdList(state.selection[i]);
    }
    DWORD start = GetTickCount();
    while (GetTickCount() - start < 3000 && !g_stop) {
        if (SendTabCommand(tab, kTabSelect, payload)) {
            Wh_Log(L"selection applied: %d items", state.selectionCount);
            return true;
        }
        Sleep(30);
    }
    Wh_Log(L"selection not applied");
    return false;
}

// ---------------------------------------------------------------------------
// Switching a tab via UI Automation: the tab strip has no window command.

// anyTitle = false: try only items with the same title (identical ones are told apart by checking the result);
// true: try the others too (the title may not match) - switches other tabs, a last resort.
// The UI Automation object is created once (it is thread-safe); released in Wh_ModUninit.
static IUIAutomation* volatile g_uia = nullptr;

static IUIAutomation* GetUia() {
    if (!g_uia) {
        IUIAutomation* uia = nullptr;
        if (SUCCEEDED(CoCreateInstance(kClsidCUIAutomation, nullptr, CLSCTX_INPROC_SERVER, kIidUIAutomation,
                                       reinterpret_cast<void**>(&uia))) &&
            uia && InterlockedCompareExchangePointer(reinterpret_cast<void* volatile*>(&g_uia), uia, nullptr)) {
            uia->Release();  // another thread was first
        }
    }
    return g_uia;
}

static IUIAutomationCondition* ControlTypeCondition(IUIAutomation* uia, long type) {
    VARIANT v;
    VariantInit(&v);
    v.vt = VT_I4;
    v.lVal = type;
    IUIAutomationCondition* cond = nullptr;
    return SUCCEEDED(uia->CreatePropertyCondition(UIA_ControlTypePropertyId, v, &cond)) ? cond : nullptr;
}

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

    bool ok = false;
    IUIAutomationElement* root = nullptr;
    IUIAutomationElement* strip = nullptr;
    IUIAutomationCondition* stripCond = ControlTypeCondition(uia, UIA_TabControlTypeId);
    IUIAutomationCondition* cond = ControlTypeCondition(uia, UIA_TabItemControlTypeId);
    IUIAutomationElementArray* items = nullptr;
    if (SUCCEEDED(uia->ElementFromHandle(top, &root)) && root && cond) {
        // Tab strip in the window title bar: look for tabs only there, not in the whole window tree.
        if (stripCond && FAILED(root->FindFirst(TreeScope_Descendants, stripCond, &strip))) {
            strip = nullptr;
        }
        if (FAILED((strip ? strip : root)->FindAll(TreeScope_Descendants, cond, &items))) {
            items = nullptr;
        }
        int found = 0;
        if (strip && (!items || FAILED(items->get_Length(&found)) || found == 0)) {
            // Found the wrong strip - search the whole window.
            if (items) {
                items->Release();
            }
            if (FAILED(root->FindAll(TreeScope_Descendants, cond, &items))) {
                items = nullptr;
            }
        }
    }
    if (items) {
        int count = 0;
        items->get_Length(&count);
        // First tabs with the same title; identical titles are told apart by checking the result.
        for (int pass = 0; pass < (anyTitle ? 2 : 1) && !ok; pass++) {
            for (int i = 0; i < count && !ok; i++) {
                IUIAutomationElement* item = nullptr;
                if (FAILED(items->GetElement(i, &item)) || !item) {
                    continue;
                }
                BSTR name = nullptr;
                item->get_CurrentName(&name);
                bool sameTitle = name && wcscmp(name, title) == 0;
                SysFreeString(name);
                if (sameTitle == (pass == 0)) {
                    IUIAutomationSelectionItemPattern* sel = nullptr;
                    if (SUCCEEDED(item->GetCurrentPatternAs(UIA_SelectionItemPatternId, kIidUIASelectionItem,
                                                            reinterpret_cast<void**>(&sel))) &&
                        sel) {
                        sel->Select();
                        sel->Release();
                        for (int t = 0; t < 30 && !ok; t++) {
                            ok = GetActiveTab(top) == tab;
                            if (!ok) {
                                Sleep(10);
                            }
                        }
                    }
                }
                item->Release();
            }
        }
    }
    if (items) {
        items->Release();
    }
    if (cond) {
        cond->Release();
    }
    if (stripCond) {
        stripCond->Release();
    }
    if (strip) {
        strip->Release();
    }
    if (root) {
        root->Release();
    }
    return ok;
}

// A new tab gets its title and its tab-strip item not immediately - retry.
static bool SelectTab(HWND top, HWND tab) {
    for (DWORD start = GetTickCount(); GetTickCount() - start < 2000 && !g_stop; Sleep(50)) {
        if (!IsWindow(tab)) {
            return false;
        }
        if (SelectTabOnce(top, tab, false)) {
            return true;
        }
    }
    return SelectTabOnce(top, tab, true);
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
    Buffer payload;
    payload.AppendIdList(folder);
    WindowList windows;
    GetCandidateWindows(self, &windows);
    for (int w = 0; w < windows.count; w++) {
        WindowList tabs;
        GetTabWindows(windows.items[w], &tabs);
        for (int t = 0; t < tabs.count; t++) {
            if (SendTabCommand(tabs.items[t], kTabMatch, payload)) {
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
    Buffer payload;
    payload.AppendIdList(folder);
    HWND active = GetActiveTab(target);
    if (active && SendTabCommand(active, kTabOpenNewTab, payload, 3000)) {
        for (DWORD start = GetTickCount(); GetTickCount() - start < 3000 && !g_stop; Sleep(kPollMs)) {
            if (HWND newTab = FindNewTab(target, before)) {
                if (!SelectTab(target, newTab)) {
                    Wh_Log(L"new tab %p opened but not activated", newTab);
                }
                return newTab;
            }
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
    DWORD start = GetTickCount();
    while (GetTickCount() - start < 4000 && !g_stop) {
        if (!newTab) {
            newTab = FindNewTab(target, before);
        }
        if (newTab && SendTabCommand(newTab, kTabNavigate, payload)) {
            return newTab;
        }
        Sleep(100);  // the tab is not ready to hand out its IShellBrowser yet
    }
    return nullptr;
}

// "Show in folder" sometimes selects the file in the new window a bit later than the folder is set.
// The tab is already open and active by then; wait for the selection for another kSelectionGraceMs.
static void CatchLateSelection(HWND self, IShellWindows* sw, HWND tab) {
    for (DWORD start = GetTickCount(); GetTickCount() - start < kSelectionGraceMs && !g_stop && IsWindow(self);
         Sleep(kPollMs)) {
        FolderState late;
        bool got = ReadWindowState(sw, self, &late) && late.selectionCount > 0;
        if (got) {
            Wh_Log(L"late selection after tab opened: %d items", late.selectionCount);
            ApplySelection(tab, late);
        }
        late.Clear();
        if (got) {
            return;
        }
    }
}

// Moves the new window self into a tab. *target is the window where the tab ended up.
static bool MergeWindowWith(HWND self, IShellWindows* sw, HWND* target) {
    FolderState state;
    if (!WaitForFolder(self, sw, &state, 4000)) {
        Wh_Log(L"merge: folder of new window unknown");
        return false;
    }

    bool ok = false;
    HWND tab = nullptr;
    if (g_reuseOpenTab && (tab = FindOpenTab(self, state.folder, target))) {
        ok = SelectTab(*target, tab);
        Wh_Log(L"merge: folder already open in tab %p, switched %d", tab, ok);
    }
    if (!ok) {
        *target = FindTarget(self);
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

static bool MergeWindow(HWND self, HWND* target) {
    IShellWindows* sw = CreateShellWindows();
    if (!sw) {
        return false;
    }
    bool ok = MergeWindowWith(self, sw, target);
    sw->Release();
    ReleaseTabHooks();
    return ok;
}

static DWORD WINAPI MergeThread(LPVOID param) {
    HWND self = static_cast<HWND>(param);
    bool coInit = SUCCEEDED(CoInitializeEx(nullptr, COINIT_MULTITHREADED));
    DWORD start = GetTickCount();

    EnterCriticalSection(&g_cs);
    HWND target = nullptr;
    bool ok = IsWindow(self) && MergeWindow(self, &target);
    LeaveCriticalSection(&g_cs);
    Wh_Log(L"merge %s in %lu ms", ok ? L"done" : L"failed", GetTickCount() - start);

    if (ok) {
        PostMessageW(self, WM_CLOSE, 0, 0);
        Unclaim(self);
    } else {
        ShowWindowBack(self);  // stays in g_claimed so that we do not try again
    }

    if (coInit) {
        CoUninitialize();
    }
    InterlockedDecrement(&g_activeMerges);
    return 0;
}

// ---------------------------------------------------------------------------
// "Show in folder" when the folder is already open. The program (via SHOpenFolderAndSelectItems)
// finds the window in ShellWindows and calls IShellView::SelectItem on the tab - an incoming
// cross-process COM call. Explorer does not switch tabs then, and sometimes
// hands out the wrong tab of the window (the file is not selected at all). We intercept SelectItem:
// - the file was selected in an inactive tab - switch to it;
// - the file is not in this tab - find the window tab that has it, select it there and switch.

static const GUID kIidServerSecurity = {0x0000013E, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
static const int kSelectItemSlot = 14;      // IShellView::SelectItem in the vtable
static const DWORD kRedirectKeepMs = 2000;  // the following items of the same call go to the same tab

using SelectItem_t = HRESULT(STDMETHODCALLTYPE*)(IShellView*, PCUITEMID_CHILD, SVSIF);
static SelectItem_t SelectItem_Original;

static volatile LONG g_switching = 0;

// Last redirect: from tab "from" to "to" (several items arrive as separate calls).
static struct {
    HWND from;
    HWND to;
    DWORD at;
} g_redirect;

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
    IUnknown* security = nullptr;
    if (FAILED(CoGetCallContext(kIidServerSecurity, reinterpret_cast<void**>(&security))) || !security) {
        return false;
    }
    security->Release();
    return true;
}

// Whether the view focus is on the item child (SelectItem with SVSI_FOCUSED puts it there).
static bool IsFocusedItem(IShellView* sv, PCUITEMID_CHILD child) {
    IFolderView* fv = nullptr;
    if (FAILED(sv->QueryInterface(kIidFolderView, reinterpret_cast<void**>(&fv))) || !fv) {
        return false;
    }
    bool equal = false;
    int index = -1;
    PITEMID_CHILD item = nullptr;
    IShellFolder* folder = nullptr;
    if (fv->GetFocusedItem(&index) == S_OK && index >= 0 && SUCCEEDED(fv->Item(index, &item)) && item &&
        SUCCEEDED(fv->GetFolder(kIidShellFolder, reinterpret_cast<void**>(&folder))) && folder) {
        HRESULT hr = folder->CompareIDs(0, item, child);
        equal = SUCCEEDED(hr) && HRESULT_CODE(hr) == 0;
    }
    if (folder) {
        folder->Release();
    }
    if (item) {
        ILFree(item);
    }
    fv->Release();
    return equal;
}

// Active view of the tab; the tab is in the same thread as the caller - a direct call.
static IShellView* GetTabView(HWND tab) {
    auto* sb = reinterpret_cast<IShellBrowser*>(SendMessageW(tab, kCwmGetIShellBrowser, 0, 0));
    IShellView* sv = nullptr;
    if (sb && FAILED(sb->QueryActiveShellView(&sv))) {
        sv = nullptr;
    }
    return sv;
}

static DWORD WINAPI SwitchTabThread(LPVOID param) {
    HWND tab = static_cast<HWND>(param);
    HWND top = GetAncestor(tab, GA_ROOT);
    bool coInit = SUCCEEDED(CoInitializeEx(nullptr, COINIT_MULTITHREADED));

    Sleep(50);  // let the SelectItem call finish
    if (IsWindow(tab) && GetActiveTab(top) != tab) {
        bool ok = SelectTab(top, tab);
        Wh_Log(L"show-in-folder: switched to tab %p: %d", tab, ok);
        if (ok && g_activateTarget) {
            ActivateWindow(top);
        }
    }

    if (coInit) {
        CoUninitialize();
    }
    InterlockedExchange(&g_switching, 0);
    InterlockedDecrement(&g_activeMerges);
    return 0;
}

// UI Automation must not be called from the window thread in the middle of a COM call - switch from our own thread.
static void SwitchToTabLater(HWND tab) {
    HWND top = GetAncestor(tab, GA_ROOT);
    if (GetActiveTab(top) == tab || InterlockedCompareExchange(&g_switching, 1, 0) != 0) {
        return;
    }
    InterlockedIncrement(&g_activeMerges);
    HANDLE t = CreateThread(nullptr, 0, SwitchTabThread, tab, 0, nullptr);
    if (t) {
        CloseHandle(t);
    } else {
        InterlockedDecrement(&g_activeMerges);
        InterlockedExchange(&g_switching, 0);
    }
}

static HRESULT STDMETHODCALLTYPE SelectItem_Hook(IShellView* self, PCUITEMID_CHILD child, SVSIF flags) {
    if (!SelectItem_Original) {
        return E_UNEXPECTED;
    }
    if (!g_activateSelectedTab || g_stop || !child || !(flags & SVSI_SELECT) || !IsIncomingComCall()) {
        return SelectItem_Original(self, child, flags);
    }
    HWND tab = GetViewTab(self);
    HWND top = tab ? GetAncestor(tab, GA_ROOT) : nullptr;
    if (!top || !IsClass(top, L"CabinetWClass")) {
        return SelectItem_Original(self, child, flags);
    }

    // Continuation of an already redirected call (several items being selected).
    if (!(flags & SVSI_DESELECTOTHERS) && g_redirect.from == tab && GetTickCount() - g_redirect.at < kRedirectKeepMs &&
        IsWindow(g_redirect.to)) {
        if (IShellView* sv = GetTabView(g_redirect.to)) {
            HRESULT hr = SelectItem_Original(sv, child, flags);
            sv->Release();
            return hr;
        }
    }

    HRESULT hr = SelectItem_Original(self, child, flags | SVSI_FOCUSED);
    if (IsFocusedItem(self, child)) {
        Wh_Log(L"show-in-folder: item selected in tab %p", tab);
        SwitchToTabLater(tab);
        return hr;
    }

    // Wrong tab: look for a tab of this window that has the item.
    WindowList tabs;
    GetTabWindows(top, &tabs);
    for (int i = 0; i < tabs.count; i++) {
        if (tabs.items[i] == tab) {
            continue;
        }
        IShellView* sv = GetTabView(tabs.items[i]);
        if (!sv) {
            continue;
        }
        HRESULT hr2 = SelectItem_Original(sv, child, flags | SVSI_FOCUSED);
        bool found = IsFocusedItem(sv, child);
        sv->Release();
        if (found) {
            Wh_Log(L"show-in-folder: wrong tab %p, item found in tab %p", tab, tabs.items[i]);
            g_redirect = {tab, tabs.items[i], GetTickCount()};
            SwitchToTabLater(tabs.items[i]);
            return hr2;
        }
    }
    return hr;
}

static bool HookSelectItem() {
    IShellFolder* desktop = nullptr;
    if (FAILED(SHGetDesktopFolder(&desktop)) || !desktop) {
        return false;
    }
    SFV_CREATE create = {sizeof(create), desktop, nullptr, nullptr};
    IShellView* view = nullptr;
    HRESULT hr = SHCreateShellFolderView(&create, &view);
    desktop->Release();
    if (FAILED(hr) || !view) {
        Wh_Log(L"SHCreateShellFolderView failed: 0x%08X", static_cast<unsigned>(hr));
        return false;
    }
    void** vtable = *reinterpret_cast<void***>(view);
    bool ok;
#ifdef WH_TEST_ALL_PROCESSES
    // Test bench without Windhawk: swap the vtable slot (shared by all Explorer views), removed in Uninit.
    ok = TestPatchSlot(&vtable[kSelectItemSlot], reinterpret_cast<void*>(SelectItem_Hook),
                       reinterpret_cast<void**>(&SelectItem_Original));
#else
    ok = Wh_SetFunctionHook(vtable[kSelectItemSlot], reinterpret_cast<void*>(SelectItem_Hook),
                            reinterpret_cast<void**>(&SelectItem_Original));
#endif
    view->Release();
    return ok;
}

// ---------------------------------------------------------------------------

// New Explorer window: decide whether to merge it, and hide it. atCreate - caught at creation
// (before the first paint, cloaked); otherwise on show (hidden, the window managed to flash).
static void HandleNewWindow(HWND hwnd, bool atCreate) {
    if (GetAncestor(hwnd, GA_ROOT) != hwnd || !IsClass(hwnd, L"CabinetWClass")) {
        return;
    }
    if (!Claim(hwnd)) {
        return;
    }
    if ((g_ignoreMouseDown && (GetAsyncKeyState(VK_LBUTTON) & 0x8000)) || !FindTarget(hwnd)) {
        return;  // stays claimed: a regular window, leave it alone
    }

    Wh_Log(L"new Explorer window %p, merging (caught at %s)", hwnd, atCreate ? L"create" : L"show");
    if (atCreate) {
        CloakWindow(hwnd, true);
    } else {
        HideWindowAsync(hwnd);
    }
    InterlockedIncrement(&g_activeMerges);
    HANDLE t = CreateThread(nullptr, 0, MergeThread, hwnd, 0, nullptr);
    if (t) {
        CloseHandle(t);
    } else {
        InterlockedDecrement(&g_activeMerges);
        ShowWindowBack(hwnd);
    }
}

// Window creation: synchronous, on the thread that creates it - before the first paint.
static void CALLBACK CreateEventProc(HWINEVENTHOOK, DWORD, HWND hwnd, LONG idObject, LONG idChild, DWORD, DWORD) {
    if (hwnd && idObject == OBJID_WINDOW && idChild == CHILDID_SELF && !g_stop) {
        HandleNewWindow(hwnd, true);
    }
}

// Window show: a fallback if creation was not caught.
static void CALLBACK ShowEventProc(HWINEVENTHOOK, DWORD, HWND hwnd, LONG idObject, LONG idChild, DWORD, DWORD) {
    if (hwnd && idObject == OBJID_WINDOW && idChild == CHILDID_SELF && !g_stop) {
        HandleNewWindow(hwnd, false);
    }
}

static DWORD WINAPI HookThread(LPVOID) {
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);  // create the message queue

    HWINEVENTHOOK createHook = SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_CREATE, g_module, CreateEventProc,
                                               GetCurrentProcessId(), 0, WINEVENT_INCONTEXT);
    HWINEVENTHOOK showHook = SetWinEventHook(EVENT_OBJECT_SHOW, EVENT_OBJECT_SHOW, nullptr, ShowEventProc,
                                             GetCurrentProcessId(), 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNTHREAD);
    if (!createHook || !showHook) {
        Wh_Log(L"SetWinEventHook failed: create %p show %p, error %lu", createHook, showHook, GetLastError());
    }
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    if (createHook) {
        UnhookWinEvent(createHook);
    }
    if (showHook) {
        UnhookWinEvent(showHook);
    }
    return 0;
}

BOOL Wh_ModInit() {
    LoadSettings();
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&TabHookProc), &g_module);
    if (!HookSelectItem()) {
        Wh_Log(L"IShellView::SelectItem not hooked; show-in-folder tab switching disabled");
    }
    InitializeCriticalSection(&g_cs);
    InitializeCriticalSection(&g_claimLock);
    g_tabDone = CreateEventW(nullptr, TRUE, FALSE, kTabDoneEvent);
    g_stop = 0;
    g_hookThread = CreateThread(nullptr, 0, HookThread, nullptr, 0, &g_hookThreadId);
    return g_hookThread != nullptr;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModUninit() {
    g_stop = 1;
    if (g_hookThread) {
        PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_hookThread, 3000);
        CloseHandle(g_hookThread);
        g_hookThread = nullptr;
    }
    for (int i = 0; i < 100 && g_activeMerges > 0; i++) {
        Sleep(50);
    }
    DeleteCriticalSection(&g_cs);
    DeleteCriticalSection(&g_claimLock);
    if (g_tabDone) {
        CloseHandle(g_tabDone);
        g_tabDone = nullptr;
    }
    if (g_uia) {
        g_uia->Release();
        g_uia = nullptr;
    }
#ifdef WH_TEST_ALL_PROCESSES
    TestUnpatchSlot();
#endif
}
