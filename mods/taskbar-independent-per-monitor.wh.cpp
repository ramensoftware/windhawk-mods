// ==WindhawkMod==
// @id              taskbar-independent-per-monitor
// @name            Independent taskbar per monitor
// @name:de-DE      Eigenständige Taskleiste pro Monitor
// @description     Each taskbar shows only the windows of its own monitor and has its own pinned items (Windows 11 only)
// @description:de-DE Jede Taskleiste zeigt nur die Fenster ihres Monitors und hat eigene angeheftete Apps (nur Windows 11)
// @version         1.5.0
// @author          2ndSky95
// @github          https://github.com/2ndSky95
// @license         MIT
// @include         explorer.exe
// @include         sihost.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lshell32 -luuid -lpsapi -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Independent taskbar per monitor

Each taskbar behaves like its own main taskbar.

![Each taskbar shows only the windows of its own monitor](https://raw.githubusercontent.com/2ndSky95/taskbar-independent-per-monitor/ea374053f26cfbfe3865461bdd4adf86dce6b782/images/windows-per-monitor.png) \
*Each taskbar shows only the windows of its own monitor*

![Separate pinned items for each taskbar](https://raw.githubusercontent.com/2ndSky95/taskbar-independent-per-monitor/ea374053f26cfbfe3865461bdd4adf86dce6b782/images/pins-per-taskbar.png) \
*Separate pinned items for each taskbar*

* Pin/unpin from the jump list applies to that taskbar only.
* Pin *Properties* can differ per taskbar.
* Clicking the pin of a running app brings its window to the front (Shift+click
  or middle-click starts a new instance).

Requires Windows 11 and *Show my taskbar on all displays*. Tested with *Combine
taskbar buttons: Never*. Not compatible with *Disable grouping on the taskbar*.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- windowMode: monitor
  $name: Windows on each taskbar
  $name:de-DE: Fenster auf jeder Taskleiste
  $options:
  - monitor: Only the windows of its own monitor
  - primaryAll: Primary taskbar shows all windows
  - all: All windows on all taskbars
  $options:de-DE:
  - monitor: Nur die Fenster des eigenen Monitors
  - primaryAll: Hauptleiste zeigt alle Fenster
  - all: Alle Fenster auf allen Taskleisten
- alwaysOnAllTaskbars: [""]
  $name: Programs shown on all taskbars
  $name:de-DE: Programme auf allen Taskleisten
  $description: Process name or path, e.g. mspaint.exe
  $description:de-DE: Prozessname oder Pfad, z.B. mspaint.exe
- pinMode: perTaskbar
  $name: Pinned items
  $name:de-DE: Angeheftete Apps
  $options:
  - perTaskbar: Separate for each taskbar
  - everywhere: Same on all taskbars
  $options:de-DE:
  - perTaskbar: Eigene pro Taskleiste
  - everywhere: Überall dieselben
- unassignedPins: primary
  $name: Pinned items without an assigned taskbar
  $name:de-DE: Angeheftete Apps ohne Zuordnung
  $description: Pinned before the mod was installed, or by Windows or an installer
  $description:de-DE: Vor der Mod oder von Windows bzw. einem Installer angeheftet
  $options:
  - primary: Primary taskbar only
  - all: All taskbars
  $options:de-DE:
  - primary: Nur Hauptleiste
  - all: Alle Taskleisten
- foregroundFix: true
  $name: Bring running apps to the front
  $name:de-DE: Laufende Apps nach vorn holen
- focusScope: anyMonitor
  $name: Windows that are brought to the front
  $name:de-DE: Fenster, die nach vorn geholt werden
  $options:
  - anyMonitor: Windows on any monitor
  - thisMonitor: Only windows on the clicked taskbar's monitor
  $options:de-DE:
  - anyMonitor: Fenster auf jedem Monitor
  - thisMonitor: Nur Fenster auf dem Monitor der Taskleiste
- slideAnimation: true
  $name: Slide animation when a window moves to another monitor
  $name:de-DE: Gleit-Animation beim Monitorwechsel
- showAppsOnAllTaskbars: true
  $name: Show taskbar apps on all taskbars
  $name:de-DE: Taskleisten-Apps auf allen Taskleisten anzeigen
  $description: Required for separate pinned items per taskbar. Applied while the mod runs, the Windows setting isn't changed. If off, set it to "All taskbars" manually
  $description:de-DE: Nötig für eigene angeheftete Apps pro Taskleiste. Gilt, solange die Mod läuft, die Windows-Einstellung bleibt unverändert. Wenn aus, von Hand auf „Alle Taskleisten“ stellen
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <propsys.h>
#include <psapi.h>
#include <appmodel.h>
#include <cstdio>
#include <cwchar>
#include <cwctype>
#include <map>
#include <atomic>
#include <mutex>
#include <set>
#include <string>
#include <vector>
#include <algorithm>

#undef GetCurrentTime  // macro from windows.h collides with WinRT
#include <winrt/Windows.UI.Xaml.h>

struct PinRule {
    std::wstring app;  // lowercase
    unsigned mask;  // Bit 0 = Monitor 1, Bit 1 = Monitor 2, ...
};

struct {
    bool foregroundFix;
    bool slideAnimation;
    bool pinsOnAll;       // unassigned pins on all taskbars
    bool pinsEverywhere;  // every pin on every taskbar (no per-taskbar pins)
    int windowMode;       // 0 = own monitor only, 1 = primary shows all, 2 = all taskbars
    bool focusThisMonitor;  // bring to front only windows on the clicked taskbar's monitor
    std::vector<std::wstring> alwaysAll;  // lowercase exe names shown on all taskbars
    bool showAppsOnAllTaskbars;  // report MMTaskbarMode=0 to taskbar.dll (see RegGetValueW_hook)
    std::vector<PinRule> rules;
} g_s;

static std::mutex g_rulesMx;
static bool g_isExplorer = false;

// ---------- Callable taskbar functions (resolved only, not hooked)

static PCWSTR (*pGroupGetAppID)(void*);
static DWORD (*pGroupGetFlags)(void*);
static PCIDLIST_ABSOLUTE (*pGroupGetShortcutIDList)(void*);
static HRESULT (*pGroupUpdateFlags)(void*, DWORD, DWORD);
static HWND (*pItemGetWindow)(void*);   // CWindowTaskItem::GetWindow (resolved only)
static void* pWindowItemVft = nullptr;  // class identity of CWindowTaskItem
static HWND (*pImmersiveItemGetWindow)(void*);  // CImmersiveTaskItem::GetWindow (resolved only)
static void* pImmersiveItemVft = nullptr;       // class identity of CImmersiveTaskItem
static HRESULT (*pGroupItemFromWindow)(void*, HWND, void**) = nullptr;  // CTaskGroup::GetItemFromWindow

// ---------- Helpers

static std::wstring Lower(std::wstring s) {
    for (auto& c : s) c = (wchar_t)towlower(c);
    return s;
}

// ---------- Monitor numbers

static BOOL CALLBACK MonEnum(HMONITOR m, HDC, LPRECT r, LPARAM lp) {
    auto* v = (std::vector<std::pair<LONG, HMONITOR>>*)lp;
    v->push_back({r->left, m});
    return TRUE;
}

// Taskbar number (1..kMaxMon) per physical monitor, kept in "monitorSlots" so that pins stay on
// their monitor when the layout changes. First run: numbered from left to right. When all
// numbers are taken, the monitor that has been disconnected longest gives up its number.
constexpr int kMaxMon = 32;
struct MonSlot {
    int n;
    unsigned seq;  // last seen (higher = more recent)
};
static std::mutex g_slotMx;
static std::map<std::wstring, MonSlot> g_slots;
static std::set<std::wstring> g_connected;  // monitors connected at the last check
static unsigned g_slotSeq = 0;
static bool g_slotsLoaded = false;
static std::map<HMONITOR, std::pair<RECT, int>> g_slotCache;
static DWORD g_slotCacheTick = 0;

static std::wstring MonitorId(HMONITOR m) {
    MONITORINFOEXW mi{};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(m, &mi)) return L"";
    DISPLAY_DEVICEW dd{.cb = sizeof(dd)};
    if (EnumDisplayDevicesW(mi.szDevice, 0, &dd, EDD_GET_DEVICE_INTERFACE_NAME) && dd.DeviceID[0])
        return Lower(dd.DeviceID);
    return Lower(mi.szDevice);
}

// "id|number|seq;..."
static void LoadSlots() {
    std::vector<wchar_t> buf(16384);
    Wh_GetStringValue(L"monitorSlots", buf.data(), buf.size());
    std::wstring s = buf.data();
    for (size_t pos = 0; pos < s.size();) {
        size_t end = s.find(L';', pos);
        if (end == std::wstring::npos) end = s.size();
        std::wstring e = s.substr(pos, end - pos);
        pos = end + 1;
        size_t b2 = e.rfind(L'|');
        if (b2 == std::wstring::npos || b2 == 0) continue;
        size_t b1 = e.rfind(L'|', b2 - 1);
        if (b1 == std::wstring::npos) continue;
        int n = _wtoi(e.c_str() + b1 + 1);
        unsigned seq = wcstoul(e.c_str() + b2 + 1, nullptr, 10);
        if (n < 1 || n > kMaxMon) continue;
        g_slots[e.substr(0, b1)] = {n, seq};
        g_slotSeq = std::max(g_slotSeq, seq + 1);
    }
}

static std::set<std::wstring> ConnectedIds() {
    std::vector<std::pair<LONG, HMONITOR>> v;
    EnumDisplayMonitors(nullptr, nullptr, MonEnum, (LPARAM)&v);
    std::set<std::wstring> r;
    for (auto& [x, hm] : v) r.insert(MonitorId(hm));
    return r;
}

static void TouchSlots(const std::set<std::wstring>& connected) {
    for (auto& id : connected) {
        auto i = g_slots.find(id);
        if (i != g_slots.end()) i->second.seq = g_slotSeq++;
    }
}

static void SaveSlots() {
    std::wstring out;
    for (auto& [id, s] : g_slots)
        out += id + L"|" + std::to_wstring(s.n) + L"|" + std::to_wstring(s.seq) + L";";
    Wh_SetStringValue(L"monitorSlots", out.c_str());
}

// Monitors connected or disconnected since the last check: both count as seen now
static void UpdateConnected() {
    std::set<std::wstring> cur = ConnectedIds();
    if (cur == g_connected) return;
    TouchSlots(g_connected);
    TouchSlots(cur);
    g_connected = std::move(cur);
    SaveSlots();
}

static void ForgetMonitor(int n);

static int MonNumberLocked(HMONITOR m, const MONITORINFO& mi, std::vector<int>& freed) {
    DWORD now = GetTickCount();
    if (!g_slotsLoaded) {
        g_slotsLoaded = true;
        LoadSlots();
    }
    if (now - g_slotCacheTick > 2000) {
        g_slotCache.clear();
        g_slotCacheTick = now;
        UpdateConnected();
    }
    auto c = g_slotCache.find(m);
    if (c != g_slotCache.end() && EqualRect(&c->second.first, &mi.rcMonitor)) return c->second.second;

    std::wstring id = MonitorId(m);
    auto it = id.empty() ? g_slots.end() : g_slots.find(id);
    if (!id.empty() && it == g_slots.end()) {
        std::vector<std::pair<LONG, HMONITOR>> v;
        EnumDisplayMonitors(nullptr, nullptr, MonEnum, (LPARAM)&v);
        std::sort(v.begin(), v.end(), [](auto& a, auto& b) { return a.first < b.first; });
        UpdateConnected();
        for (auto& [x, hm] : v) {  // new monitors get the lowest free numbers, left to right
            std::wstring vid = MonitorId(hm);
            if (vid.empty() || g_slots.count(vid)) continue;
            std::set<int> used;
            for (auto& [k, s] : g_slots) used.insert(s.n);
            int n = 1;
            while (n <= kMaxMon && used.count(n)) n++;
            if (n > kMaxMon) {
                auto old = g_slots.end();
                for (auto i = g_slots.begin(); i != g_slots.end(); ++i) {
                    if (!g_connected.count(i->first) && (old == g_slots.end() || i->second.seq < old->second.seq))
                        old = i;
                }
                if (old == g_slots.end()) break;
                n = old->second.n;
                g_slots.erase(old);
                freed.push_back(n);
            }
            g_slots[vid] = {n, g_slotSeq++};
        }
        SaveSlots();
        it = g_slots.find(id);
    }
    int n = it != g_slots.end() ? it->second.n : 0;
    g_slotCache[m] = {mi.rcMonitor, n};  // also a miss, so it isn't retried on every call
    return n;
}

static int MonNumber(HMONITOR m) {
    if (!m) return 0;
    MONITORINFO mi{.cbSize = sizeof(mi)};
    if (!GetMonitorInfoW(m, &mi)) return 0;
    std::vector<int> freed;
    int n;
    {
        std::lock_guard<std::mutex> l(g_slotMx);
        n = MonNumberLocked(m, mi, freed);
    }
    for (int f : freed) ForgetMonitor(f);
    return n;
}

static bool MonIsPrimary(HMONITOR m) {
    MONITORINFO mi{.cbSize = sizeof(mi)};
    return m && GetMonitorInfoW(m, &mi) && (mi.dwFlags & MONITORINFOF_PRIMARY);
}


static bool InMask(unsigned mask, int mon) {
    return mon >= 1 && mon <= kMaxMon && (mask & (1u << (mon - 1)));
}

// 0 = no rule (default Windows behavior), otherwise a monitor bit mask
static unsigned AssignedMonitor(void* g) {
    if (!g || !pGroupGetFlags || !pGroupGetAppID) return 0;
    if (!(pGroupGetFlags(g) & 0x1)) return 0;  // not pinned
    PCWSTR id = pGroupGetAppID(g);
    if (!id) return 0;
    std::wstring lid;
    for (const wchar_t* p = id; *p; p++) lid += (wchar_t)towlower(*p);
    std::lock_guard<std::mutex> l(g_rulesMx);
    for (auto& r : g_s.rules) {
        if (r.app == lid) return r.mask;
    }
    return 0;
}

static std::wstring ProcPath(DWORD pid) {
    std::wstring r;
    HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (h) {
        wchar_t p[MAX_PATH * 2];
        DWORD len = ARRAYSIZE(p);
        if (QueryFullProcessImageNameW(h, 0, p, &len)) r.assign(p, len);
        CloseHandle(h);
    }
    return Lower(r);
}

static const PROPERTYKEY kAumid = {
    {0x9F4C2855, 0x9F79, 0x4B39, {0xA8, 0xD0, 0xE1, 0xD4, 0x2D, 0xE1, 0xD5, 0xF3}},
    5};

static std::wstring WindowAppId(HWND h) {
    std::wstring r;
    IPropertyStore* ps = nullptr;
    if (SUCCEEDED(SHGetPropertyStoreForWindow(h, IID_PPV_ARGS(&ps))) && ps) {
        PROPVARIANT pv;
        PropVariantInit(&pv);
        if (SUCCEEDED(ps->GetValue(kAumid, &pv)) && pv.vt == VT_LPWSTR &&
            pv.pwszVal) {
            r = pv.pwszVal;
        }
        PropVariantClear(&pv);
        ps->Release();
    }
    return r;
}

// App ID of a packaged app (e.g. Windows Notepad) via its process
static std::wstring PackageAppId(HWND h) {
    std::wstring r;
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    if (!pid) return r;
    HANDLE p = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!p) return r;
    wchar_t buf[512];
    UINT32 len = ARRAYSIZE(buf);
    if (GetApplicationUserModelId(p, &len, buf) == ERROR_SUCCESS) r = buf;
    CloseHandle(p);
    return r;
}

struct Target {
    std::wstring exactPath;  // lowercase, full path
    std::wstring name;       // file name only (Squirrel/Update.exe cases)
    std::wstring dirPrefix;  // folder the EXE must be located in
    bool hasArgs = false;    // shortcut with its own launch arguments (e.g. steam://open/friends)
};

static std::wstring FileName(const std::wstring& p) {
    size_t i = p.find_last_of(L"\\/");
    return i == std::wstring::npos ? p : p.substr(i + 1);
}

static std::wstring DirOf(const std::wstring& p) {
    size_t i = p.find_last_of(L"\\/");
    return i == std::wstring::npos ? L"" : p.substr(0, i + 1);
}

static void ResolveTarget(const std::wstring& path, Target& t) {
    std::wstring lp = Lower(path);
    if (lp.size() < 4 || lp.substr(lp.size() - 4) != L".lnk") {
        t.exactPath = lp;
        return;
    }
    IShellLinkW* sl = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(&sl))) ||
        !sl) {
        return;
    }
    IPersistFile* pf = nullptr;
    if (SUCCEEDED(sl->QueryInterface(IID_PPV_ARGS(&pf))) && pf) {
        if (SUCCEEDED(pf->Load(path.c_str(), STGM_READ))) {
            wchar_t raw[MAX_PATH] = {}, exp[MAX_PATH * 2] = {},
                    args[1024] = {};
            sl->GetPath(raw, MAX_PATH, nullptr, SLGP_RAWPATH);
            ExpandEnvironmentStringsW(raw, exp, ARRAYSIZE(exp));
            sl->GetArguments(args, ARRAYSIZE(args));
            std::wstring target = Lower(exp);
            std::wstring a = args;
            size_t k = a.find(L"--processStart ");
            if (FileName(target) == L"update.exe" && k != std::wstring::npos) {
                std::wstring rest = a.substr(k + 15);
                if (!rest.empty() && rest[0] == L'"') {
                    rest = rest.substr(1);
                    rest = rest.substr(0, rest.find(L'"'));
                } else {
                    rest = rest.substr(0, rest.find(L' '));
                }
                t.name = Lower(rest);
                t.dirPrefix = DirOf(target);
            } else {
                t.exactPath = target;
                t.hasArgs = args[0] != 0;
            }
        }
        pf->Release();
    }
    sl->Release();
}

struct FindCtx {
    void* group = nullptr;  // taskbar group: knows its own windows
    HMONITOR onlyMon = nullptr;  // only visible windows on this monitor (app with per-taskbar pins)
    std::wstring appId;
    Target tgt;
    std::map<DWORD, std::wstring> pidPath;
    HWND visible = nullptr;
};

static bool PathMatches(FindCtx* c, DWORD pid) {
    auto it = c->pidPath.find(pid);
    if (it == c->pidPath.end()) it = c->pidPath.emplace(pid, ProcPath(pid)).first;
    const std::wstring& p = it->second;
    if (p.empty()) return false;
    if (!c->tgt.exactPath.empty() && p == c->tgt.exactPath) return true;
    if (!c->tgt.name.empty() && FileName(p) == c->tgt.name &&
        p.compare(0, c->tgt.dirPrefix.size(), c->tgt.dirPrefix) == 0) {
        return true;
    }
    return false;
}

static BOOL CALLBACK EnumProc(HWND h, LPARAM lp) {
    auto* c = (FindCtx*)lp;
    // Only visible windows: hidden ones (e.g. of tray apps) are left to the app itself, a normal
    // launch of a single-instance app shows them
    if (c->visible || !IsWindowVisible(h) || GetWindow(h, GW_OWNER)) return TRUE;
    if (c->onlyMon && MonitorFromWindow(h, MONITOR_DEFAULTTONEAREST) != c->onlyMon) return TRUE;
    LONG ex = GetWindowLongW(h, GWL_EXSTYLE);
    LONG st = GetWindowLongW(h, GWL_STYLE);
    if (ex & WS_EX_TOOLWINDOW) return TRUE;
    if ((st & WS_CAPTION) != WS_CAPTION) return TRUE;
    if (GetWindowTextLengthW(h) == 0) return TRUE;

    bool match = false;
    // 1. ask the group itself (reliable, also for packaged apps such as Notepad)
    if (c->group && pGroupItemFromWindow) {
        void* item = nullptr;
        if (SUCCEEDED(pGroupItemFromWindow(c->group, h, &item)) && item) {
            ((IUnknown*)item)->Release();
            match = true;
        }
    }
    // 2. app ID of the window or of the packaged app
    if (!match && !c->appId.empty()) {
        std::wstring w = WindowAppId(h);
        if (w.empty()) w = PackageAppId(h);
        if (!w.empty() && _wcsicmp(w.c_str(), c->appId.c_str()) == 0) match = true;
    }
    if (!match && (!c->tgt.exactPath.empty() || !c->tgt.name.empty())) {
        DWORD pid = 0;
        GetWindowThreadProcessId(h, &pid);
        match = PathMatches(c, pid);
    }
    if (match) c->visible = h;
    return TRUE;
}

static bool TryActivateExisting(void* g, HMONITOR onlyMon = nullptr) {
    FindCtx ctx;
    ctx.group = g;
    ctx.onlyMon = onlyMon;
    PCWSTR id = pGroupGetAppID ? pGroupGetAppID(g) : nullptr;
    if (id) ctx.appId = id;

    PCIDLIST_ABSOLUTE pidl =
        pGroupGetShortcutIDList ? pGroupGetShortcutIDList(g) : nullptr;
    wchar_t path[MAX_PATH] = {};
    if (pidl && SHGetPathFromIDListW(pidl, path)) ResolveTarget(path, ctx.tgt);

    // Pin with its own launch command: always launch normally (the command should run)
    if (ctx.tgt.hasArgs) {
        Wh_Log(L"  Pin has launch arguments - launching normally");
        return false;
    }
    EnumWindows(EnumProc, (LPARAM)&ctx);
    HWND h = ctx.visible;
    Wh_Log(L"  Search: app=%ls lnk=%ls exe=%ls name=%ls -> window=%p", ctx.appId.c_str(), path,
           ctx.tgt.exactPath.c_str(), ctx.tgt.name.c_str(), (void*)h);
    if (!h) return false;

    if (IsIconic(h)) ShowWindow(h, SW_RESTORE);
    if (!SetForegroundWindow(h)) SwitchToThisWindow(h, TRUE);
    Wh_Log(L"  -> brought existing window %p to the front", (void*)h);
    return true;
}

// ---------- Hooks
// Windows runs in the "All taskbars" mode (MMTaskbarMode=0) and creates every button on
// every taskbar. The mod only filters:
//  - window buttons: only on the taskbar of the monitor the window is on
//  - pins with a rule: only on the chosen monitors; pins without a rule: primary taskbar only

using TaskCreated_t = HRESULT (*)(void*, void*, void*);
static TaskCreated_t TaskCreated_orig;
using MonSelf_t = HMONITOR (*)(void*);
static MonSelf_t GetMonitor_orig;
using TaskCreatedInt_t = HRESULT (*)(void*, void*, void*, int);
static TaskCreatedInt_t TaskCreatedInt_orig;
static TaskCreated_t TaskIncl_orig;
static TaskCreated_t TaskDestroyed_orig;

static std::mutex g_mx;
static std::set<void*> g_bars;                 // tl pointers of all taskbars
// Windows with a taskbar button, by window handle. The group is held with a reference so it
// stays valid; the task item itself is fetched from the group when it's needed (GroupItem).
struct WinInfo {
    void* g = nullptr;
    HMONITOR mon = nullptr;    // last known monitor
    std::set<HMONITOR> bars;   // taskbars (by monitor) the button is on
    bool frame = false;        // UWP frame window (ApplicationFrameWindow)
};
static std::map<HWND, WinInfo> g_wins;
// Message window on the taskbar thread. The timers belong to this thread and must be removed
// there too - otherwise Windows calls the timer procedures in the unloaded DLL (crash).
static HWND g_msgWnd = nullptr;
static bool g_msgWndFailed = false;  // creating it failed, don't retry on every call
static std::atomic<bool> g_unloading{false};  // set in Wh_ModBeforeUninit: don't create it again
static const wchar_t kMsgClass[] = L"TaskbarIndependentPerMonitorMsg";
static const UINT kMsgCleanup = WM_APP + 0x51;
static const UINT kMsgSync = WM_APP + 0x52;  // posted: re-apply all pins on the taskbar thread
static const UINT_PTR kReevalTimerId = 1;
static const UINT_PTR kApplyTimerId = 2;
static const UINT_PTR kSyncTimerId = 3;
static void SyncTimerProc();
static void SyncAll();
static const ULONG_PTR kCopyUnpinOne = 0x534B5031;  // WM_COPYDATA from the jump list ("SKP1")
static void ApplyTimerProc();
static void ReleasePending();
static void SetFakeGroup(void* g);
static void SetJumpGroup(void* g);
static LRESULT HandleUnpinOne(const wchar_t* app);
static bool HandleExtraPin(const wchar_t* app, const wchar_t* path);
static DWORD g_jumpTick = 0;
static void* g_fakeGroup = nullptr;  // jump list showed "Pin" although pinned globally (AddRef)
static std::wstring g_swallowAddKey;
static DWORD g_swallowAddTick = 0;

// Window handle of a button even if the window is already destroyed (for cleanup) - only if it
// is definitely a CWindowTaskItem or a CImmersiveTaskItem (UWP apps such as Settings)
static HWND ItemWindowRaw(void* item) {
    if (!item) return nullptr;
    void* vft = *(void**)item;
    if (vft && vft == pWindowItemVft && pItemGetWindow) return pItemGetWindow(item);
    if (vft && vft == pImmersiveItemVft && pImmersiveItemGetWindow) return pImmersiveItemGetWindow(item);
    return nullptr;
}

// Window of a button
static HWND ItemWindow(void* item) {
    HWND h = ItemWindowRaw(item);
    return (h && IsWindow(h)) ? h : nullptr;
}

static HMONITOR ItemMonitor(void* item) {
    HWND h = ItemWindow(item);
    return h ? MonitorFromWindow(h, MONITOR_DEFAULTTONEAREST) : nullptr;
}

// Windows doesn't report monitor changes of UWP frame windows to the taskbar: while one is
// tracked, a timer checks their monitor
static const UINT_PTR kFrameTimerId = 6;
static bool g_frameTimer = false;

static bool IsFrameWindow(HWND h) {
    wchar_t cls[32];
    return GetClassNameW(h, cls, ARRAYSIZE(cls)) && !wcscmp(cls, L"ApplicationFrameWindow");
}

static void TrackWindow(HWND h, void* g) {
    bool frame = IsFrameWindow(h);
    void* old = nullptr;
    {
        std::lock_guard<std::mutex> l(g_mx);
        WinInfo& w = g_wins[h];
        if (w.g != g) {
            old = w.g;
            w.g = g;
            ((IUnknown*)g)->AddRef();
        }
        w.mon = MonitorFromWindow(h, MONITOR_DEFAULTTONEAREST);
        w.frame = frame;
    }
    if (old) ((IUnknown*)old)->Release();
    // SetTimer only succeeds on the taskbar thread, which owns the message window
    if (frame && g_msgWnd && !g_frameTimer && !g_unloading)
        g_frameTimer = SetTimer(g_msgWnd, kFrameTimerId, 500, nullptr) != 0;
}

static void UntrackWindow(HWND h) {
    void* old = nullptr;
    {
        std::lock_guard<std::mutex> l(g_mx);
        auto it = g_wins.find(h);
        if (it == g_wins.end()) return;
        old = it->second.g;
        g_wins.erase(it);
    }
    if (old) ((IUnknown*)old)->Release();
}

// Drop windows that no longer exist (all = drop everything, on unload)
static void ReleaseWindows(bool all) {
    std::vector<void*> groups;
    {
        std::lock_guard<std::mutex> l(g_mx);
        for (auto it = g_wins.begin(); it != g_wins.end();) {
            if (!all && IsWindow(it->first)) {
                ++it;
                continue;
            }
            groups.push_back(it->second.g);
            it = g_wins.erase(it);
        }
    }
    for (void* g : groups) ((IUnknown*)g)->Release();
}

static bool IsNewPin(void* g);
static bool Pinned(void* g);

// Windows of the apps in the "always on all taskbars" list (cached per window)
static std::mutex g_exemptMx;
static std::map<HWND, bool> g_exemptCache;
static bool IsExemptWindow(HWND h) {
    if (!h) return false;
    {
        std::lock_guard<std::mutex> l(g_rulesMx);
        if (g_s.alwaysAll.empty()) return false;
    }
    {
        std::lock_guard<std::mutex> l(g_exemptMx);
        auto it = g_exemptCache.find(h);
        if (it != g_exemptCache.end()) return it->second;
    }
    // UWP frame windows belong to ApplicationFrameHost.exe: use the app's CoreWindow instead. It's
    // detached while the app is minimized or suspended - then don't cache the result.
    DWORD pid = 0;
    bool cache = true;
    if (IsFrameWindow(h)) {
        if (HWND core = FindWindowExW(h, nullptr, L"Windows.UI.Core.CoreWindow", nullptr))
            GetWindowThreadProcessId(core, &pid);
        else
            cache = false;
    }
    if (!pid) GetWindowThreadProcessId(h, &pid);
    std::wstring path = ProcPath(pid), exe = FileName(path);
    bool r = false;
    {
        std::lock_guard<std::mutex> l(g_rulesMx);
        for (auto& a : g_s.alwaysAll) {
            if (a == (a.find(L'\\') != std::wstring::npos ? path : exe)) r = true;
        }
    }
    if (!cache) return r;
    std::lock_guard<std::mutex> l(g_exemptMx);
    if (g_exemptCache.size() > 1000) g_exemptCache.clear();
    g_exemptCache[h] = r;
    return r;
}

// May the window h (on monitor wm) be shown on the taskbar of monitor bar?
static bool WinAllowed(HWND h, HMONITOR wm, HMONITOR bar) {
    if (!wm || wm == bar) return true;  // monitor unknown -> don't filter
    if (g_s.windowMode == 2) return true;
    if (g_s.windowMode == 1 && MonIsPrimary(bar)) return true;
    return IsExemptWindow(h);
}

// true = show on this taskbar
static bool ShouldShow(void* tl, void* g, void* item) {
    if (!g || !GetMonitor_orig) return true;
    HMONITOR bar = GetMonitor_orig(tl);
    if (item) {
        HWND h = ItemWindow(item);
        return WinAllowed(h, h ? MonitorFromWindow(h, MONITOR_DEFAULTTONEAREST) : nullptr, bar);
    }
    if (!(pGroupGetFlags && (pGroupGetFlags(g) & 0x1))) return true;  // not a pin
    if (g_s.pinsEverywhere) return true;
    unsigned mask = AssignedMonitor(g);
    if (!mask) return g_s.pinsOnAll || MonIsPrimary(bar);
    // freshly pinned: the primary taskbar must play along briefly, otherwise Windows unpins it again
    if (IsNewPin(g) && MonIsPrimary(bar)) return true;
    return InMask(mask, MonNumber(bar));
}

static void QueueReeval(HWND h);
static void EnsureMsgWnd();
static bool OnTrayThread();
static int g_moveDir = 0;        // -1 = came from the right, +1 = came from the left
static DWORD g_moveTick = 0;
static DWORD g_swapTick = 0;      // a pin <-> window button swap on the same taskbar follows
static DWORD g_swapHideTick = 0;  // plus: old button without exit animation

static bool AddBar(void* tl);

static HRESULT TaskCreated_hook(void* self, void* g, void* item) {
    if (!g_unloading && !g_msgWnd && !g_msgWndFailed && OnTrayThread()) EnsureMsgWnd();  // taskbar thread only
    if (!AddBar(self)) return TaskCreated_orig(self, g, item);  // unknown layout: don't filter
    if (item && g) {
        if (HWND h = ItemWindow(item)) TrackWindow(h, g);
    }
    // Don't pass on a pin without a window that isn't allowed here at all.
    // Otherwise TaskCreated creates a button group itself and reports it to the XAML taskbar
    // (ghost pin on the primary taskbar), even if _TaskCreated is filtered.
    if (!item && g && pGroupGetFlags && (pGroupGetFlags(g) & 0x1) &&
        !ShouldShow(self, g, nullptr)) {
        PCWSTR id = pGroupGetAppID ? pGroupGetAppID(g) : nullptr;
        Wh_Log(L"-> Pin skipped (monitor %d): %.40ls",
             MonNumber(GetMonitor_orig ? GetMonitor_orig(self) : nullptr), id ? id : L"?");
        return S_OK;
    }
    return TaskCreated_orig(self, g, item);
}

// ---------- Restrict the windows of a group to one taskbar
// When an app gets a button group on a taskbar for the first time, Windows puts ALL
// windows of this app into it (e.g. the Steam main window next to the friends list,
// WhatsApp next to a call, windows next to the replacement pin). Right after that -
// before drawing - take out all windows that are on another monitor again.

static void* (*pGetTBGroup)(void*, void*, int*);  // CTaskListWnd::_GetTBGroupFromGroup
// Window button of a group for a window (or nullptr). CTaskGroup::GetItemFromWindow returns
// the item with a reference (COM out parameter), free it with ReleaseItem.
static void* GroupItem(void* g, HWND h) {
    void* item = nullptr;
    if (!pGroupItemFromWindow || FAILED(pGroupItemFromWindow(g, h, &item))) return nullptr;
    return item;
}
static void ReleaseItem(void* item) {
    if (item) ((IUnknown*)item)->Release();
}

static const wchar_t* AppOf(void* g);
static BOOL CALLBACK CollectWnd(HWND h, LPARAM lp) {
    if (IsWindowVisible(h) || IsIconic(h)) ((std::vector<HWND>*)lp)->push_back(h);
    return TRUE;
}

// Walk the entries of g's button group on the taskbar and remove windows that are on another
// monitor. Directly via the taskbar's button group - GetItemFromWindow doesn't always
// return the same object that is on the taskbar.
static bool Readable(const void* p, size_t n);
static int (*pBtnGetNumItems)(void*) = nullptr;       // CTaskBtnGroup::GetNumItems
static void* (*pBtnGetTaskItem)(void*, int) = nullptr;  // CTaskBtnGroup::GetTaskItem

// All button groups of a taskbar: CDPA<ITaskBtnGroup> in CTaskListWnd. The offset is taken
// from the code of GetButtonGroupCount ("mov rdx,[rcx+disp32]" / "ldr xN,[x0,#imm]") at runtime.
// With "never combine" every window has its own group - _GetTBGroupFromGroup only finds
// the first one.
static HMODULE g_tb = nullptr;
static void* pGetButtonGroupCount = nullptr;
static int g_dpaOffset = -1;                    // from tl; -1 = unknown/disabled

static void InitDpaOffset() {
#if defined(__aarch64__)
    const DWORD* p = (const DWORD*)pGetButtonGroupCount;
    for (int i = 0; p && i < 4 && Readable(p + i, 4); i++) {
        if ((p[i] & 0xFFC003E0) == 0xF9400000) {  // ldr xN,[x0,#imm]
            int d = ((p[i] >> 10) & 0xFFF) * 8;
            if (d > 0 && d < 0x1000) g_dpaOffset = d;
            break;
        }
    }
#else
    const unsigned char* p = (const unsigned char*)pGetButtonGroupCount;
    if (p && Readable(p, 8) && p[0] == 0x48 && p[1] == 0x8B && p[2] == 0x91) {
        int d = *(const int*)(p + 3);
        if (d > 0 && d < 0x1000) g_dpaOffset = d;
    }
#endif
    Wh_Log(L"-> Button group list %ls (0x%X)", g_dpaOffset > 0 ? L"found" : L"NOT found", g_dpaOffset);
}

static std::vector<void*> BtnGroupsOf(void* tl, void* g) {
    std::vector<void*> r;
    if (g_dpaOffset <= 0 || !g_tb) return r;
    // DPA: { int cp; void** pp; ... } - as in _GetTBGroupFromGroup ("mov rcx,[rax+8]")
    char* dpa = *(char**)((char*)tl + g_dpaOffset);
    if (!dpa || !Readable(dpa, 0x10)) return r;
    int cnt = *(int*)dpa;
    void** pp = *(void***)(dpa + 0x08);
    if (cnt <= 0 || cnt > 2000 || !pp || !Readable(pp, cnt * sizeof(void*))) return r;
    MODULEINFO mi{};
    GetModuleInformation(GetCurrentProcess(), g_tb, &mi, sizeof(mi));
    char* lo = (char*)mi.lpBaseOfDll;
    char* hi = lo + mi.SizeOfImage;
    for (int k = 0; k < cnt; k++) {
        void* tbg = pp[k];
        if (!tbg || !Readable(tbg, 8)) continue;
        void** vt = *(void***)tbg;  // vtable must be inside taskbar.dll
        if ((char*)vt < lo || (char*)vt >= hi || !Readable(vt, 7 * sizeof(void*))) continue;
        auto getGroup = (void* (*)(void*))vt[6];  // ITaskBtnGroup::GetGroup (as Windows itself does)
        if ((char*)getGroup < lo || (char*)getGroup >= hi) continue;
        if (getGroup(tbg) == g) r.push_back(tbg);
    }
    return r;
}

// The items of window h that are on the taskbar (from its button groups, no reference)
static std::vector<void*> ItemsOfWindowOnBar(void* tl, void* g, HWND h) {
    std::vector<void*> r;
    if (!pBtnGetNumItems || !pBtnGetTaskItem) return r;
    for (void* tbg : BtnGroupsOf(tl, g)) {
        int c = pBtnGetNumItems(tbg);
        for (int k = 0; k < c; k++) {
            void* it = pBtnGetTaskItem(tbg, k);
            if (it && ItemWindow(it) == h) r.push_back(it);
        }
    }
    return r;
}

// Does the taskbar already have a button for window h of group g?
static bool ItemOnBar(void* tl, void* g, HWND h) {
    if (!pBtnGetNumItems || !pBtnGetTaskItem || g_dpaOffset <= 0) {
        int i = -1;
        return pGetTBGroup && pGetTBGroup((char*)tl - 0x28, g, &i);  // fallback: group exists
    }
    for (void* tbg : BtnGroupsOf(tl, g)) {
        int c = pBtnGetNumItems(tbg);
        for (int k = 0; k < c; k++) {
            if (ItemWindow(pBtnGetTaskItem(tbg, k)) == h) return true;
        }
    }
    return false;
}

static int PruneForeignItems(void* tl, void* g) {
    if (!pBtnGetNumItems || !pBtnGetTaskItem) return 0;
    HMONITOR bar = GetMonitor_orig(tl);
    std::vector<std::pair<void*, HWND>> foreign;
    int cnt = 0;
    for (void* tbg : BtnGroupsOf(tl, g)) {
        int c = pBtnGetNumItems(tbg);
        cnt += c;
        for (int k = 0; k < c; k++) {
            void* it = pBtnGetTaskItem(tbg, k);
            HWND h = ItemWindow(it);
            HMONITOR m = h ? MonitorFromWindow(h, MONITOR_DEFAULTTONEAREST) : nullptr;
            if (it && m && !WinAllowed(h, m, bar)) foreign.push_back({it, h});
        }
    }
    for (auto& [it, h] : foreign) {
        if (Pinned(g) && ShouldShow(tl, g, nullptr)) g_swapTick = g_swapHideTick = GetTickCount();  // pin stays here
        TaskDestroyed_orig(tl, g, it);
        std::lock_guard<std::mutex> l(g_mx);
        auto f = g_wins.find(h);
        if (f != g_wins.end()) f->second.bars.erase(bar);
    }
    if (!foreign.empty())
        Wh_Log(L"-> Monitor %d: removed %d foreign window(s) (of %d) %.40ls", MonNumber(bar), (int)foreign.size(),
             cnt, AppOf(g));
    return (int)foreign.size();
}
// Check deferred: the visible taskbar (XAML) processes the insertion a bit later - removing
// it immediately would have no effect there and leave ghost buttons behind.
static const UINT_PTR kPruneTimerId = 5;
static std::set<std::pair<void*, void*>> g_prune;  // (tl, group) with AddRef on the group

static void QueuePrune(void* tl, void* g) {
    if (!g_msgWnd || !g) return;
    if (g_prune.insert({tl, g}).second) ((IUnknown*)g)->AddRef();
    SetTimer(g_msgWnd, kPruneTimerId, 60, nullptr);
}

static void RefreshBars();
static void PruneTimerProc() {
    KillTimer(g_msgWnd, kPruneTimerId);
    RefreshBars();  // drops requests for taskbars that no longer exist
    auto v = std::move(g_prune);
    g_prune.clear();
    for (auto& p : v) {
        PruneForeignItems(p.first, p.second);
        ((IUnknown*)p.second)->Release();
    }
}

static void ReleasePrune() {
    for (auto& p : g_prune) ((IUnknown*)p.second)->Release();
    g_prune.clear();
}
static void ReleasePruneFor(const std::set<void*>& live) {
    for (auto it = g_prune.begin(); it != g_prune.end();) {
        if (live.count(it->first)) {
            ++it;
            continue;
        }
        ((IUnknown*)it->second)->Release();
        it = g_prune.erase(it);
    }
}
// Is the app currently shown on this taskbar only as a pin (without a window)?
static bool PinOnlyOnBar(void* self, void* g) {
    if (!g || !Pinned(g) || !pBtnGetNumItems) return false;
    auto v = BtnGroupsOf((char*)self + 0x28, g);
    if (v.empty()) return false;
    for (void* tb : v) if (pBtnGetNumItems(tb) > 0) return false;
    return true;
}

// Create a button (window or pin); if the group is newly created on this taskbar,
// take foreign windows out again
static HRESULT CreateOnBar(void* self, void* g, void* item, int a) {
    HRESULT hr = TaskCreatedInt_orig(self, g, item, a);
    QueuePrune((char*)self + 0x28, g);  // remove windows of other monitors that slipped in (deferred)
    return hr;
}
// The visible taskbar (XAML) builds its buttons from the WHOLE app group and asks
// CTaskListWnd::IsTaskAllowed for each window - this is how Windows filters in the "taskbar
// where the window is open" mode. Here: only windows on this taskbar's monitor are allowed.
using IsAllowed_t = bool (*)(void*, void*);
static IsAllowed_t IsAllowed_orig;
static bool IsAllowed_hook(void* self, void* item) {
    bool r = IsAllowed_orig(self, item);
    if (!r || !item || !GetMonitor_orig) return r;
    HMONITOR im = ItemMonitor(item);
    if (!im) return r;
    void* tl = nullptr;
    {
        std::lock_guard<std::mutex> l(g_mx);
        if (g_bars.count(self)) tl = self;
        else if (g_bars.count((char*)self + 0x28)) tl = (char*)self + 0x28;
    }
    if (!tl) return r;
    bool ok = WinAllowed(ItemWindow(item), im, GetMonitor_orig(tl));
    return ok;
}
static void* BarOf(void* self);
static HRESULT TaskCreatedInt_hook(void* self, void* g, void* item, int a) {
    void* tl = BarOf(self);
    if (!tl) return TaskCreatedInt_orig(self, g, item, a);  // taskbar not known (yet): don't filter
    if (!ShouldShow(tl, g, item)) {
        // window is elsewhere: show the pin if it's allowed on this taskbar
        if (item && pGroupGetFlags && (pGroupGetFlags(g) & 0x1) && ShouldShow(tl, g, nullptr))
            return CreateOnBar(self, g, nullptr, a);
        return S_OK;
    }
    if (HWND h = item ? ItemWindow(item) : nullptr) {
        HMONITOR bar = GetMonitor_orig(tl);
        std::lock_guard<std::mutex> l(g_mx);
        auto it = g_wins.find(h);
        if (it != g_wins.end()) it->second.bars.insert(bar);
    }
    if (item && PinOnlyOnBar(self, g)) g_swapTick = g_swapHideTick = GetTickCount();
    return CreateOnBar(self, g, item, a);
}

static HRESULT TaskIncl_hook(void* self, void* g, void* item) {
    // Pins are rebuilt (pin/unpin) through here - skip pins that aren't allowed
    if (!item && g && pGroupGetFlags && (pGroupGetFlags(g) & 0x1) &&
        !ShouldShow(self, g, nullptr))
        return S_OK;
    HRESULT hr = TaskIncl_orig(self, g, item);
    // UWP frame windows get their buttons through here, bypassing IsTaskAllowed on other taskbars
    HWND h = ItemWindow(item);
    if (h && g && IsFrameWindow(h)) {
        TrackWindow(h, g);
        QueuePrune(self, g);
    }
    return hr;
}

static bool Pinned(void* g);
static void RemovePinButton(void* tl, void* g);
static bool OtherWindowOnBar(void* tl, void* g, HWND exclude);

static HRESULT TaskDestroyed_hook(void* self, void* g, void* item) {
    HWND h = ItemWindowRaw(item);  // the window may already be destroyed
    if (h) UntrackWindow(h);
    if (item && g && Pinned(g) && ShouldShow(self, g, nullptr)) g_swapTick = g_swapHideTick = GetTickCount();
    HRESULT hr = TaskDestroyed_orig(self, g, item);
    // Last window of a pinned app closed -> Windows turns it into a pin.
    // Remove it again on taskbars where the pin isn't allowed.
    if (item && g && Pinned(g) && !ShouldShow(self, g, nullptr) &&
        !OtherWindowOnBar(self, g, h)) {
        RemovePinButton(self, g);
    }
    return hr;
}

// On startup the primary taskbar restores the pin order via SetRelativeTaskOrder - and
// creates missing button groups itself while doing so (bypassing the filter, ghost pin).
// If the group is missing here and the pin isn't allowed: don't pass it on.
// Exception: a freshly pinned app on the primary taskbar - Windows must be allowed to
// create the group there while reading the pins, otherwise it unpins it immediately
// (_EnumPinnedItems). The follow-up (ApplyPinRule) removes the button afterwards.
using SetOrder_t = void (*)(void*, void*, int);
static SetOrder_t SetOrder_orig;
static void SetOrder_hook(void* self, void* g, int pos) {
    if (g && pGetTBGroup && pGroupGetFlags && (pGroupGetFlags(g) & 0x1)) {
        void* tl = nullptr;
        {
            std::lock_guard<std::mutex> l(g_mx);
            if (g_bars.count(self)) tl = self;
            else if (g_bars.count((char*)self + 0x28)) tl = (char*)self + 0x28;
        }
        if (tl && !ShouldShow(tl, g, nullptr)) {
            int i = -1;
            if (!pGetTBGroup((char*)tl - 0x28, g, &i)) return;
        }
    }
    SetOrder_orig(self, g, pos);
}

// ---------- Redistribute (window moved / host seen for the first time)

static std::set<HWND> g_dirty;
static bool g_reevalTimer = false;
static bool g_reevalAll = false;

static HRESULT TaskCreatedInt_hook(void* self, void* g, void* item, int a);

// Remove the pin button of a pinned group from ONE taskbar.
// Secondary taskbars ignore TaskDestroyed(g, nullptr) as long as the group counts as
// pinned; the primary taskbar clears the pin flag of the whole group while doing so.
// Therefore: clear the flag briefly, remove the button, restore the flag right away.
static bool Pinned(void* g) { return pGroupGetFlags && (pGroupGetFlags(g) & 0x1); }

static bool g_selfFlag = false;  // own flag toggle in progress -> ignore pin events

static void RemovePinButton(void* tl, void* g) {
    int i = -1;
    if (pGetTBGroup && !pGetTBGroup((char*)tl - 0x28, g, &i)) return;  // nothing there
    struct Guard { Guard() { g_selfFlag = true; } ~Guard() { g_selfFlag = false; } } guard;
    bool was = Pinned(g);
    if (was && pGroupUpdateFlags) pGroupUpdateFlags(g, 0x1, 0);  // UpdateFlags(mask, value)
    TaskDestroyed_orig(tl, g, nullptr);
    if (was && !Pinned(g) && pGroupUpdateFlags) pGroupUpdateFlags(g, 0x1, 0x1);
}

// Is another window of the same group shown on this taskbar?
static bool OtherWindowOnBar(void* tl, void* g, HWND exclude) {
    HMONITOR bar = GetMonitor_orig(tl);
    std::vector<HWND> wins;
    {
        std::lock_guard<std::mutex> l(g_mx);
        for (auto& [h, w] : g_wins)
            if (w.g == g && h != exclude) wins.push_back(h);
    }
    for (HWND h : wins) {
        if (IsWindow(h) && WinAllowed(h, MonitorFromWindow(h, MONITOR_DEFAULTTONEAREST), bar)) return true;
    }
    return false;
}

// Only touch affected taskbars (touching everything everywhere caused flicker).
// Windows usually creates the window button on the new taskbar itself. The mod only
// removes it from the old taskbars - a pinned group stays there as a pin;
// only where the pin isn't allowed, it is removed as well.
static void ReevalWindow(HWND h, void* g, const std::set<void*>& bars) {
    void* item = GroupItem(g, h);
    if (!item) return;
    bool pinned = Pinned(g);
    HMONITOR now = MonitorFromWindow(h, MONITOR_DEFAULTTONEAREST);
    std::set<HMONITOR> on, allowed;
    bool known = false;
    {
        std::lock_guard<std::mutex> l(g_mx);
        auto it = g_wins.find(h);
        if (it != g_wins.end()) { on = it->second.bars; known = true; }
    }
    int removed = 0, added = 0;
    for (void* tl : bars) {
        HMONITOR bm = GetMonitor_orig(tl);
        if (WinAllowed(h, now, bm)) {
            allowed.insert(bm);
            if (!on.count(bm)) { CreateOnBar((char*)tl - 0x28, g, item, 1); added++; }
        } else if (!known || on.count(bm)) {
            if (pinned && ShouldShow(tl, g, nullptr)) g_swapTick = g_swapHideTick = GetTickCount();
            // remove the item that's actually on this taskbar (GetItemFromWindow may return
            // another object); without the button group list, fall back to the group's item
            std::vector<void*> onBar = ItemsOfWindowOnBar(tl, g, h);
            if (onBar.empty() && (g_dpaOffset <= 0 || !pBtnGetNumItems)) onBar.push_back(item);
            for (void* it : onBar) TaskDestroyed_orig(tl, g, it);
            if (pinned && !ShouldShow(tl, g, nullptr) && !OtherWindowOnBar(tl, g, h))
                RemovePinButton(tl, g);
            removed++;
        }
    }
    ReleaseItem(item);
    Wh_Log(L"-> Window on monitor %d: removed from %d taskbar(s), created on %d", MonNumber(now), removed, added);
    std::lock_guard<std::mutex> l(g_mx);
    auto it = g_wins.find(h);
    if (it != g_wins.end()) it->second.bars = allowed;
}

static void ReevalTimerProc() {
    KillTimer(g_msgWnd, kReevalTimerId);
    g_reevalTimer = false;
    RefreshBars();
    ReleaseWindows(false);  // windows that no longer exist
    std::set<void*> bars;
    std::vector<std::pair<HWND, void*>> todo;  // with a reference on the group
    {
        std::lock_guard<std::mutex> l(g_mx);
        bars = g_bars;
        for (auto& [h, w] : g_wins) {
            if (g_reevalAll || g_dirty.count(h)) {
                ((IUnknown*)w.g)->AddRef();
                todo.push_back({h, w.g});
            }
        }
        g_dirty.clear();
        g_reevalAll = false;
    }
    for (auto& [h, g] : todo) {
        ReevalWindow(h, g, bars);
        ((IUnknown*)g)->Release();
    }
    Wh_Log(L"-> Redistributed %d window(s)", (int)todo.size());
}

static void QueueReeval(HWND h) {
    {
        std::lock_guard<std::mutex> l(g_mx);
        if (h) g_dirty.insert(h); else g_reevalAll = true;
    }
    if (!g_reevalTimer && g_msgWnd)
        g_reevalTimer = SetTimer(g_msgWnd, kReevalTimerId, 150, nullptr) != 0;
}

static void FrameTimerProc() {
    std::vector<HWND> moved;
    bool any = false;
    {
        std::lock_guard<std::mutex> l(g_mx);
        for (auto& [h, w] : g_wins) {
            if (!w.frame) continue;
            any = true;
            HMONITOR now = MonitorFromWindow(h, MONITOR_DEFAULTTONEAREST);
            if (now == w.mon) continue;
            MONITORINFO a{.cbSize = sizeof(a)}, b{.cbSize = sizeof(b)};
            if (w.mon && GetMonitorInfoW(w.mon, &a) && GetMonitorInfoW(now, &b)) {
                g_moveDir = b.rcMonitor.left > a.rcMonitor.left ? 1 : -1;
                g_moveTick = GetTickCount();
            }
            w.mon = now;
            moved.push_back(h);
        }
    }
    if (!any) {
        KillTimer(g_msgWnd, kFrameTimerId);
        g_frameTimer = false;
    }
    for (HWND h : moved) QueueReeval(h);
}


// Windows reports a monitor change itself before it shows the button on the new taskbar:
// remember the direction for the sideways slide-in and re-evaluate the window
using MonChanged_t = void (*)(void*, HWND);
static MonChanged_t MonChanged_orig;
static void MonChanged_hook(void* self, HWND h) {
    HMONITOR now = h ? MonitorFromWindow(h, MONITOR_DEFAULTTONEAREST) : nullptr;
    HMONITOR old = nullptr;
    bool moved = false;
    if (now) {
        std::lock_guard<std::mutex> l(g_mx);
        auto it = g_wins.find(h);
        if (it != g_wins.end() && it->second.mon != now) {
            old = it->second.mon;
            it->second.mon = now;
            moved = true;
        }
    }
    MONITORINFO a{.cbSize = sizeof(a)}, b{.cbSize = sizeof(b)};
    if (old && GetMonitorInfoW(old, &a) && GetMonitorInfoW(now, &b)) {
        g_moveDir = b.rcMonitor.left > a.rcMonitor.left ? 1 : -1;
        g_moveTick = GetTickCount();
    }
    MonChanged_orig(self, h);
    if (moved && !g_unloading) {
        QueueReeval(h);
        Wh_Log(L"-> Window %p moved to monitor %d", (void*)h, MonNumber(now));
    }
}

static LRESULT CALLBACK MsgWndProc(HWND h, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_TIMER && wp == kReevalTimerId) {
        ReevalTimerProc();
        return 0;
    }
    if (msg == WM_TIMER && wp == kPruneTimerId) {
        PruneTimerProc();
        return 0;
    }
    if (msg == WM_TIMER && wp == kFrameTimerId) {
        FrameTimerProc();
        return 0;
    }
    if (msg == WM_TIMER && wp == kSyncTimerId) {
        SyncTimerProc();
        return 0;
    }
    if (msg == WM_TIMER && wp == kApplyTimerId) {
        ApplyTimerProc();
        return 0;
    }
    if (msg == WM_COPYDATA) {  // the pin list hooks (explorer.exe, sihost.exe) ask before changing
        auto* cd = (COPYDATASTRUCT*)lp;
        if (!cd) return 0;
        // The app ID is read only within the sent size (the sender isn't trusted to terminate it)
        std::wstring app;
        if (cd->lpData && cd->cbData >= sizeof(wchar_t)) {
            app.assign((const wchar_t*)cd->lpData, cd->cbData / sizeof(wchar_t));
            app.resize(wcsnlen(app.c_str(), app.size()));
        }
        Wh_Log(L"-> Request from the pin list: %lX", (unsigned long)cd->dwData);
        if (cd->dwData == 0x534B5032) {  // "SKP2": PinManager wants to pin ("path|aumid")
            size_t sep = app.find(L'|');
            std::wstring path = app.substr(0, sep);
            std::wstring aumid = sep == std::wstring::npos ? L"" : app.substr(sep + 1);
            return (g_fakeGroup && GetTickCount() - g_jumpTick < 10000 &&
                    HandleExtraPin(aumid.c_str(), path.c_str())) ? 1 : 0;
        }
        if (cd->dwData == 0x534B4131) {  // "SKA1": re-append after HandleExtraPin?
            if (g_fakeGroup && _wcsicmp(app.c_str(), AppOf(g_fakeGroup)) != 0) SetFakeGroup(nullptr);  // another app was pinned
            return !g_swallowAddKey.empty() && GetTickCount() - g_swallowAddTick < 3000 && Lower(app) == g_swallowAddKey
                       ? (g_swallowAddKey.clear(), 1) : 0;
        }
        if (cd->dwData == kCopyUnpinOne) return HandleUnpinOne(app.c_str());
        return 0;
    }
    if (msg == kMsgSync) {
        if (!g_unloading) SyncAll();
        return 0;
    }
    if (msg == kMsgCleanup) {  // runs on the taskbar thread
        KillTimer(h, kReevalTimerId);
        KillTimer(h, kApplyTimerId);
        KillTimer(h, kSyncTimerId);
        KillTimer(h, kPruneTimerId);
        KillTimer(h, kFrameTimerId);
        g_frameTimer = false;
        ReleasePrune();
        ReleasePending();
        SetFakeGroup(nullptr);
        SetJumpGroup(nullptr);
        ReleaseWindows(true);
        g_reevalTimer = false;
        DestroyWindow(h);
        g_msgWnd = nullptr;
        return 0;
    }
    return DefWindowProcW(h, msg, wp, lp);
}

static HINSTANCE ModInstance() {
    HMODULE m = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&MsgWndProc, &m);
    return m;
}

static void EnsureMsgWnd() {
    if (g_msgWnd || g_msgWndFailed || g_unloading) return;
    WNDCLASSW wc{};
    wc.lpfnWndProc = MsgWndProc;
    wc.hInstance = ModInstance();
    wc.lpszClassName = kMsgClass;
    if (!RegisterClassW(&wc)) {  // an existing class could belong to an unloaded version
        g_msgWndFailed = true;
        Wh_Log(L"-> Message window class FAILED (%lu)", GetLastError());
        return;
    }
    g_msgWnd = CreateWindowExW(0, kMsgClass, L"", 0, 0, 0, 0, 0, HWND_MESSAGE,
                               nullptr, wc.hInstance, nullptr);
    if (!g_msgWnd) {
        g_msgWndFailed = true;  // don't retry on every call
        Wh_Log(L"-> Message window FAILED (%lu)", GetLastError());
        return;
    }
    SetTimer(g_msgWnd, kSyncTimerId, 3000, nullptr);  // sync once startup is done
}

// Run a function on the taskbar thread (mod loaded late / update): a short
// WH_CALLWNDPROC hook on the thread, then a message to the taskbar
static UINT g_runMsg = 0;
static LRESULT CALLBACK RunProc(int code, WPARAM wp, LPARAM lp) {
    if (code == HC_ACTION) {
        auto* c = (CWPSTRUCT*)lp;
        if (c->message == g_runMsg && c->lParam == 0x534B) ((void (*)())c->wParam)();
    }
    return CallNextHookEx(nullptr, code, wp, lp);
}
static bool RunOnTrayThread(void (*fn)()) {
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    DWORD pid = 0;
    DWORD tid = tray ? GetWindowThreadProcessId(tray, &pid) : 0;
    if (!tid || pid != GetCurrentProcessId()) return false;
    if (tid == GetCurrentThreadId()) { fn(); return true; }
    if (!g_runMsg) g_runMsg = RegisterWindowMessageW(L"TaskbarIndependentPerMonitorRun");
    HHOOK hk = SetWindowsHookExW(WH_CALLWNDPROC, RunProc, ModInstance(), tid);
    if (!hk) return false;
    SendMessageW(tray, g_runMsg, (WPARAM)fn, 0x534B);
    UnhookWindowsHookEx(hk);
    return true;
}

// Is this call running on the taskbar thread?
static bool OnTrayThread() {
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    DWORD pid = 0;
    return tray && GetWindowThreadProcessId(tray, &pid) == GetCurrentThreadId() && pid == GetCurrentProcessId();
}

static HMONITOR GetMonitor_hook(void* self) {
    AddBar(self);
    // Mod loaded late / reloaded: start even without new buttons
    if (!g_unloading && !g_msgWnd && !g_msgWndFailed && OnTrayThread()) EnsureMsgWnd();
    return GetMonitor_orig(self);
}

// ---------- Pin rules (set via the jump list, stored in the mod storage)
// Format: "appid|mask;appid|mask;" - appid in lowercase, mask bit 0 = monitor 1

static std::map<std::wstring, unsigned> LoadPinRules() {
    std::map<std::wstring, unsigned> r;
    std::vector<wchar_t> buf(16384);
    Wh_GetStringValue(L"pinRules", buf.data(), buf.size());
    std::wstring all = buf.data(), part;
    size_t pos = 0;
    while (pos < all.size()) {
        size_t e = all.find(L';', pos);
        if (e == std::wstring::npos) e = all.size();
        part = all.substr(pos, e - pos);
        size_t bar = part.find(L'|');
        if (bar != std::wstring::npos) {
            unsigned m = wcstoul(part.c_str() + bar + 1, nullptr, 16);
            if (m) r[part.substr(0, bar)] = m;
        }
        pos = e + 1;
    }
    return r;
}

static void SavePinRules(const std::map<std::wstring, unsigned>& r) {
    std::wstring out;
    wchar_t hex[16];
    for (auto& kv : r) {
        swprintf(hex, 16, L"%X", kv.second);
        out += kv.first + L"|" + hex + L";";
    }
    Wh_SetStringValue(L"pinRules", out.c_str());
}

static void LoadSettings();
static void LoadRules();

// ---------- Apply rules live - no Explorer restart

static void SetRule(const std::wstring& key, unsigned m) {
    auto rules = LoadPinRules();
    if (m) rules[key] = m; else rules.erase(key);
    SavePinRules(rules);
    LoadRules();
}

// Align the pin buttons of a group on all taskbars with its rule
static void ApplyPinRule(void* g) {
    if (!Pinned(g)) return;
    std::set<void*> bars;
    {
        std::lock_guard<std::mutex> l(g_mx);
        bars = g_bars;
    }
    for (void* tl : bars) {
        int i = -1;
        bool has = pGetTBGroup && pGetTBGroup((char*)tl - 0x28, g, &i);
        if (ShouldShow(tl, g, nullptr)) {
            if (!has) CreateOnBar((char*)tl - 0x28, g, nullptr, 1);
        } else if (has && !OtherWindowOnBar(tl, g, nullptr)) {
            RemovePinButton(tl, g);
        }
    }
}

// Follow-up: Windows still builds buttons itself after pinning - align again afterwards
static std::vector<void*> g_pendingApply;  // held with AddRef

static void QueueApply(void* g) {
    if (!g_msgWnd) return;
    ((IUnknown*)g)->AddRef();
    g_pendingApply.push_back(g);
    SetTimer(g_msgWnd, kApplyTimerId, 400, nullptr);
}

static std::wstring g_newPinKey;

static void ApplyTimerProc() {
    KillTimer(g_msgWnd, kApplyTimerId);
    RefreshBars();
    g_newPinKey.clear();  // exception for the fresh pin ends - clean up now
    std::vector<void*> v;
    v.swap(g_pendingApply);
    for (void* g : v) {
        ApplyPinRule(g);
        ((IUnknown*)g)->Release();
    }
}

static void ReleasePending() {
    for (void* g : g_pendingApply) ((IUnknown*)g)->Release();
    g_pendingApply.clear();
}

// ---------- Sync after startup (and after a mod update)
// On startup Windows partly builds the taskbars in ways the filter doesn't see, or the
// mod is loaded only when everything is already in place. The sync walks all groups of all
// taskbars: window buttons only on their monitor, pins only where allowed.

static void* pGroupVft = nullptr;  // CTaskGroup identity (ITaskGroup)
static int g_syncRuns = 0;

// Find the taskbars ourselves (after a mod update the mod doesn't know them yet): every
// MSTaskListWClass window stores its CImpWndProc part in window data 0; the ITaskListUI
// part (= tl) is nearby, recognizable by its vtable.
static void* pTlVftUI = nullptr;
static void* pTlVftWnd = nullptr;

static bool Readable(const void* p, size_t n) {
    MEMORY_BASIC_INFORMATION mi;
    if (!VirtualQuery(p, &mi, sizeof(mi)) || mi.State != MEM_COMMIT) return false;
    if (mi.Protect & (PAGE_GUARD | PAGE_NOACCESS)) return false;
    return (char*)p + n <= (char*)mi.BaseAddress + mi.RegionSize;
}

// The mod converts between the ITaskListUI part of CTaskListWnd (tl, the "this" of the virtual
// ITaskListUI methods) and the object itself (tl - 0x28, the "this" of _TaskCreated and
// _GetTBGroupFromGroup). Before a taskbar is used, both are checked against the CTaskListWnd
// vtables from the symbols, so a changed class layout disables the mod for that taskbar
// instead of crashing Explorer.
static void* pTlVftOther[6];

static bool ValidBar(void* tl) {
    if (!tl || !pTlVftUI || !Readable((char*)tl - 0x28, 0x28 + sizeof(void*)) ||
        *(void**)tl != pTlVftUI) {
        return false;
    }
    void* first = *(void**)((char*)tl - 0x28);
    bool ok = pTlVftWnd && first == pTlVftWnd;
    for (void* v : pTlVftOther) {
        if (v && v == first) ok = true;
    }
    if (!ok) {
        static bool logged = false;
        if (!logged) Wh_Log(L"-> Unexpected CTaskListWnd layout - taskbar ignored");
        logged = true;
    }
    return ok;
}

static bool AddBar(void* tl) {
    {
        std::lock_guard<std::mutex> l(g_mx);
        if (g_bars.count(tl)) return true;
    }
    if (!ValidBar(tl)) return false;
    std::lock_guard<std::mutex> l(g_mx);
    g_bars.insert(tl);
    return true;
}

static BOOL CALLBACK FindListWnd(HWND h, LPARAM lp) {
    auto* found = (std::set<void*>*)lp;
    wchar_t cls[64];
    if (!GetClassNameW(h, cls, 64) || wcscmp(cls, L"MSTaskListWClass") != 0) return TRUE;
    char* p = (char*)GetWindowLongPtrW(h, 0);
    if (!p || !Readable(p, 8) || *(void**)p != pTlVftWnd) {
        Wh_Log(L"-> Taskbar search: window %p without a matching vtable (%p)", h, p ? *(void**)p : nullptr);
        return TRUE;
    }
    // take the nearest match - the taskbar objects are located close to each other
    for (int d = 0; d <= 0x400; d += 8) {
        for (int sgn = 0; sgn < 2; sgn++) {
            int off = sgn ? -d : d;
            char* q = p + off;
            if (Readable(q, 8) && *(void**)q == pTlVftUI) {
                if (ValidBar(q)) found->insert(q);
                return TRUE;
            }
        }
    }
    Wh_Log(L"-> Taskbar search: window %p - tl not found", h);
    return TRUE;
}

static BOOL CALLBACK FindTrays(HWND h, LPARAM lp) {
    wchar_t cls[64];
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    if (pid == GetCurrentProcessId() && GetClassNameW(h, cls, 64) &&
        (!wcscmp(cls, L"Shell_TrayWnd") || !wcscmp(cls, L"Shell_SecondaryTrayWnd")))
        EnumChildWindows(h, FindListWnd, lp);
    return TRUE;
}

// Taskbars come and go with monitors. Before the list is used outside of a taskbar call, it's
// rebuilt from the taskbar windows that currently exist, so the object of a removed taskbar is
// never touched. Pending prune requests for removed taskbars are dropped.
static void RefreshBars() {
    if (!pTlVftUI || !pTlVftWnd) return;
    std::set<void*> live;
    EnumWindows(FindTrays, (LPARAM)&live);
    {
        std::lock_guard<std::mutex> l(g_mx);
        g_bars = live;
    }
    ReleasePruneFor(live);
}

// All groups that have a button on the taskbar, with a reference each (release with Release).
// ITaskBtnGroup::GetGroup returns the group without a reference.
static void GroupsOfBar(void* tl, std::set<void*>& groups) {
    if (g_dpaOffset <= 0 || !g_tb) return;
    char* dpa = *(char**)((char*)tl + g_dpaOffset);
    if (!dpa || !Readable(dpa, 0x10)) return;
    int cnt = *(int*)dpa;
    void** pp = *(void***)(dpa + 0x08);
    if (cnt <= 0 || cnt > 2000 || !pp || !Readable(pp, cnt * sizeof(void*))) return;
    MODULEINFO mi{};
    GetModuleInformation(GetCurrentProcess(), g_tb, &mi, sizeof(mi));
    char* lo = (char*)mi.lpBaseOfDll;
    char* hi = lo + mi.SizeOfImage;
    for (int k = 0; k < cnt; k++) {
        void* tbg = pp[k];
        if (!tbg || !Readable(tbg, 8)) continue;
        void** vt = *(void***)tbg;
        if ((char*)vt < lo || (char*)vt >= hi || !Readable(vt, 7 * sizeof(void*))) continue;
        auto getGroup = (void* (*)(void*))vt[6];
        if ((char*)getGroup < lo || (char*)getGroup >= hi) continue;
        void* g = getGroup(tbg);
        if (g && Readable(g, sizeof(void*)) && *(void**)g == pGroupVft && groups.insert(g).second)
            ((IUnknown*)g)->AddRef();
    }
}


static void SyncAll() {
    if (!pGroupVft || !pGroupItemFromWindow || !GetMonitor_orig) return;
    RefreshBars();
    std::set<void*> bars;
    {
        std::lock_guard<std::mutex> l(g_mx);
        bars = g_bars;
    }
    // 1. collect the groups of all taskbars (with a reference each)
    std::set<void*> groups;
    for (void* tl : bars) GroupsOfBar(tl, groups);
    // 2. windows: each window button gets its taskbar
    std::vector<HWND> wins;
    EnumWindows(CollectWnd, (LPARAM)&wins);
    int moved = 0, pinsOff = 0, pinsOn = 0;
    for (void* g : groups) {
        for (HWND h : wins) {
            void* item = GroupItem(g, h);
            if (!item) continue;
            TrackWindow(h, g);
            HMONITOR now = MonitorFromWindow(h, MONITOR_DEFAULTTONEAREST);
            std::set<HMONITOR> allowed;
            for (void* tl : bars) {
                HMONITOR bm = GetMonitor_orig(tl);
                if (WinAllowed(h, now, bm)) {
                    allowed.insert(bm);
                    if (!ItemOnBar(tl, g, h)) CreateOnBar((char*)tl - 0x28, g, item, 1);
                }
            }
            {
                std::lock_guard<std::mutex> l(g_mx);
                auto it = g_wins.find(h);
                if (it != g_wins.end()) it->second.bars = allowed;
            }
            ReleaseItem(item);
        }
    }
    // remove windows that are stuck on foreign taskbars
    for (void* g : groups)
        for (void* tl : bars) moved += PruneForeignItems(tl, g);
    // 3. pins: only where allowed (or where a window of the app is)
    for (void* g : groups) {
        if (!Pinned(g)) continue;
        for (void* tl : bars) {
            int i = -1;
            bool has = pGetTBGroup((char*)tl - 0x28, g, &i) != nullptr;
            if (ShouldShow(tl, g, nullptr)) {
                if (!has) { CreateOnBar((char*)tl - 0x28, g, nullptr, 1); pinsOn++; }
            } else if (has && !OtherWindowOnBar(tl, g, nullptr)) {
                RemovePinButton(tl, g);
                pinsOff++;
            }
        }
    }
    for (void* g : groups) ((IUnknown*)g)->Release();
    Wh_Log(L"-> Sync: %d groups, %d taskbars, window fixes %d, pins removed %d, pins added %d",
           (int)groups.size(), (int)bars.size(), moved, pinsOff, pinsOn);
}

static void SyncTimerProc() {
    KillTimer(g_msgWnd, kSyncTimerId);
    SyncAll();
    if (++g_syncRuns < 2) SetTimer(g_msgWnd, kSyncTimerId, 5000, nullptr);  // second pass
}
// ---------- Own shortcut per taskbar
// Windows has only ONE pin (one .lnk) per app. So that "Properties" (right-click on the app entry
// in the jump list) only applies to one taskbar, each taskbar gets a copy of the shortcut the first
// time its jump list is opened (<mod storage>\TaskbarLinks\M<n>\). While the jump list is being
// built, the group points to this copy; clicking the pin launches the copy if it was modified.
static HRESULT (*pGroupSetShortcut)(void*, PCIDLIST_ABSOLUTE) = nullptr;  // CTaskGroup::SetShortcutIDList
static std::wstring g_linkRoot;  // set in Wh_ModInit; Windhawk deletes the folder when the mod is removed

static std::wstring BarLinkDir(int mon) {
    if (g_linkRoot.empty()) return L"";
    wchar_t d[40];
    swprintf(d, 40, L"\\M%d", mon);
    return g_linkRoot + d;
}

// A number that goes to a new monitor: drop the old monitor's pins and shortcut copies
static void ForgetMonitor(int n) {
    auto rules = LoadPinRules();
    for (auto& [key, mask] : rules) mask &= ~(1u << (n - 1));
    std::erase_if(rules, [](auto& kv) { return kv.second == 0; });
    SavePinRules(rules);
    LoadRules();
    std::wstring dir = BarLinkDir(n);
    if (!dir.empty()) {
        WIN32_FIND_DATAW fd;
        HANDLE h = FindFirstFileW((dir + L"\\*.lnk").c_str(), &fd);
        if (h != INVALID_HANDLE_VALUE) {
            do DeleteFileW((dir + L"\\" + fd.cFileName).c_str()); while (FindNextFileW(h, &fd));
            FindClose(h);
        }
        RemoveDirectoryW(dir.c_str());
    }
    if (g_msgWnd) PostMessageW(g_msgWnd, kMsgSync, 0, 0);  // pins that lost their taskbar
    Wh_Log(L"-> Taskbar number %d reused for a new monitor", n);
}

static void InitLinkRoot() {
    wchar_t b[MAX_PATH] = {};
    if (!Wh_GetModStoragePath(b, MAX_PATH) || !*b) return;
    g_linkRoot = std::wstring(b) + L"\\TaskbarLinks";
}

// .lnk file behind the pin (empty for packaged apps without a shortcut)
static std::wstring GroupLinkPath(void* g) {
    PCIDLIST_ABSOLUTE p = pGroupGetShortcutIDList ? pGroupGetShortcutIDList(g) : nullptr;
    wchar_t path[MAX_PATH] = {};
    if (!p || !SHGetPathFromIDListW(p, path)) return L"";
    std::wstring s = path;
    if (s.size() < 4 || _wcsicmp(s.c_str() + s.size() - 4, L".lnk") != 0) return L"";
    return s;
}

static std::wstring BarLinkPath(void* g, int mon, bool create) {
    if (mon < 1 || mon > kMaxMon) return L"";
    std::wstring orig = GroupLinkPath(g);
    std::wstring dir = BarLinkDir(mon);
    if (orig.empty() || dir.empty() || Lower(orig).find(Lower(g_linkRoot)) == 0) return L"";
    std::wstring copy = dir + L"\\" + FileName(orig);
    if (create && GetFileAttributesW(copy.c_str()) == INVALID_FILE_ATTRIBUTES) {
        SHCreateDirectoryExW(nullptr, dir.c_str(), nullptr);
        CopyFileW(orig.c_str(), copy.c_str(), TRUE);
    }
    return GetFileAttributesW(copy.c_str()) != INVALID_FILE_ATTRIBUTES ? copy : L"";
}

// Shortcut files are small: compare the sizes first, then the content
static bool FilesEqual(const std::wstring& a, const std::wstring& b) {
    WIN32_FILE_ATTRIBUTE_DATA fa, fb;
    if (!GetFileAttributesExW(a.c_str(), GetFileExInfoStandard, &fa) ||
        !GetFileAttributesExW(b.c_str(), GetFileExInfoStandard, &fb) ||
        fa.nFileSizeHigh || fb.nFileSizeHigh || fa.nFileSizeLow != fb.nFileSizeLow ||
        fa.nFileSizeLow > 1024 * 1024) {
        return false;
    }
    auto read = [](const std::wstring& p, std::string& out) {
        HANDLE h = CreateFileW(p.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
        if (h == INVALID_HANDLE_VALUE) return false;
        char buf[4096];
        DWORD r;
        while (ReadFile(h, buf, sizeof(buf), &r, nullptr) && r) out.append(buf, r);
        CloseHandle(h);
        return true;
    };
    std::string x, y;
    return read(a, x) && read(b, y) && x == y;
}
// The copies are made from the pinned shortcut. When the shortcut itself changes (for example
// an app update rewrites it), the copies that weren't changed by the user are updated. A snapshot
// of the shortcut at copy time (<storage>\TaskbarLinks\Original) tells them apart.
static void RefreshLinkCopies(const std::wstring& orig) {
    if (g_linkRoot.empty() || orig.empty()) return;
    std::wstring name = FileName(orig);
    std::wstring baseDir = g_linkRoot + L"\\Original", base = baseDir + L"\\" + name;
    if (GetFileAttributesW(base.c_str()) == INVALID_FILE_ATTRIBUTES) {
        SHCreateDirectoryExW(nullptr, baseDir.c_str(), nullptr);
        CopyFileW(orig.c_str(), base.c_str(), TRUE);
        return;
    }
    if (FilesEqual(orig, base)) return;
    for (int mon = 1; mon <= kMaxMon; mon++) {
        std::wstring copy = BarLinkDir(mon) + L"\\" + name;
        if (GetFileAttributesW(copy.c_str()) != INVALID_FILE_ATTRIBUTES && FilesEqual(copy, base))
            CopyFileW(orig.c_str(), copy.c_str(), FALSE);
    }
    CopyFileW(orig.c_str(), base.c_str(), FALSE);
    Wh_Log(L"-> Shortcut changed, unchanged per-taskbar copies updated: %ls", name.c_str());
}

using Launch_t = HRESULT (*)(void*, void*, const POINT*, int);
static Launch_t Launch_orig;
static HRESULT Launch_hook(void* self, void* g, const POINT* pt, int opt) {
    // App has a visible window: bring it to the front instead of relaunching
    bool wantNew = opt != 0 || (GetAsyncKeyState(VK_SHIFT) & 0x8000) ||
                   (GetAsyncKeyState(VK_MBUTTON) & 0x8000);
    // This taskbar's pin has its own (modified) shortcut -> launch it. If the app has its own
    // settings on any taskbar, focusing only brings windows from the own monitor
    // (otherwise e.g. Steam on M2 would bring the friends list from M1 instead of opening Steam).
    POINT p{};
    if (pt) p = *pt;
    if (!pt || (p.x == 0 && p.y == 0)) GetCursorPos(&p);
    HMONITOR clickMon = MonitorFromPoint(p, MONITOR_DEFAULTTONEAREST);
    HMONITOR focusMon = g_s.focusThisMonitor ? clickMon : nullptr;
    if (g && Pinned(g) && pGroupSetShortcut) {
        int mon = MonNumber(clickMon);
        std::wstring orig = GroupLinkPath(g);
        RefreshLinkCopies(orig);
        bool anyOwn = false;
        for (int m = 1; m <= kMaxMon && !orig.empty(); m++) {
            std::wstring c = BarLinkPath(g, m, false);
            if (!c.empty() && !FilesEqual(c, orig)) anyOwn = true;
        }
        if (anyOwn) focusMon = clickMon;
        std::wstring copy = BarLinkPath(g, mon, false);
        if (!copy.empty() && !orig.empty() && !FilesEqual(copy, orig)) {
            Target tc;
            ResolveTarget(copy, tc);
            if (g_s.foregroundFix && !wantNew && !tc.hasArgs && TryActivateExisting(g, focusMon)) return S_OK;
            ShellExecuteW(nullptr, L"open", copy.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
            Wh_Log(L"-> Launched own shortcut of monitor %d: %ls", mon, copy.c_str());
            return S_OK;
        }
    }
    if (g_s.foregroundFix && g && !wantNew) {
        if (TryActivateExisting(g, focusMon)) return S_OK;
    }
    return Launch_orig(self, g, pt, opt);
}

// ---------- Pin/unpin via the normal right-click menu - per taskbar
// The jump list runs in ShellExperienceHost and changes the global pin list directly.
// The mod turns this into per-taskbar pins:
//  1. Pin on taskbar X (app not pinned anywhere): Windows pins -> rule = X only
//  2. Pin on taskbar Y (app already pinned on X): the jump list on Y shows "Pin" (pin flag
//     removed briefly during ShowJumpView). Click -> Windows rereads the pin list -> add Y
//  3. Unpin on X (only pinned there): real unpin, rule removed
//  4. Unpin on X (also on Y): Windows would unpin globally -> skipped, only X is removed

static DWORD g_initTick = 0;
static const wchar_t* AppOf(void* g) {
    PCWSTR id = (g && pGroupGetAppID) ? pGroupGetAppID(g) : nullptr;
    return id ? id : L"?";
}
static std::wstring KeyOf(void* g) { return Lower(AppOf(g)); }

// this of a CTaskListWnd call -> known tl pointer (or nullptr)
static void* BarOf(void* self) {
    std::lock_guard<std::mutex> l(g_mx);
    if (g_bars.count(self)) return self;
    if (g_bars.count((char*)self + 0x28)) return (char*)self + 0x28;
    return nullptr;
}

static int PrimaryNumber() {
    POINT pt{0, 0};
    return MonNumber(MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY));
}

// Current mask of a pinned app (without a rule: primary taskbar only)
static unsigned MaskOf(void* g) {
    unsigned m = AssignedMonitor(g);
    if (m) return m;
    if (g_s.pinsOnAll) {  // all connected taskbars
        std::vector<std::pair<LONG, HMONITOR>> v;
        EnumDisplayMonitors(nullptr, nullptr, MonEnum, (LPARAM)&v);
        unsigned all = 0;
        for (auto& [x, hm] : v) {
            int n = MonNumber(hm);
            if (n >= 1) all |= 1u << (n - 1);
        }
        return all;
    }
    int p = PrimaryNumber();
    return p >= 1 ? 1u << (p - 1) : 0;
}

// Real user actions only: not right after startup, not during our own flag toggle,
// only if mouse/keyboard was just used. Multiple calls (one per taskbar) count once.
static std::wstring g_lastEvt;
static DWORD g_lastEvtTick = 0;
static bool TakePinEvent(void* g, bool pin, std::wstring& key) {
    if (g_selfFlag || !g || !pGroupGetAppID) return false;
    DWORD now = GetTickCount();
    LASTINPUTINFO li{.cbSize = sizeof(li)};
    if (now - g_initTick < 10000 || !GetLastInputInfo(&li) || now - li.dwTime > 8000) return false;
    key = KeyOf(g);
    std::wstring tag = (pin ? L"+" : L"-") + key;
    if (tag == g_lastEvt && now - g_lastEvtTick < 3000) return false;
    g_lastEvt = tag;
    g_lastEvtTick = now;
    return true;
}

// Most recently opened jump list (taskbar + group)
static HMONITOR g_jumpMon = nullptr;

// Monitor of the action: jump list of the last 20 s, otherwise the mouse cursor (e.g. Start menu)
static int ActionMonitor() {
    if (g_jumpMon && GetTickCount() - g_jumpTick < 20000) return MonNumber(g_jumpMon);
    POINT pt;
    GetCursorPos(&pt);
    return MonNumber(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST));
}

static void SetFakeGroup(void* g) {
    if (g) ((IUnknown*)g)->AddRef();
    if (g_fakeGroup) ((IUnknown*)g_fakeGroup)->Release();
    g_fakeGroup = g;
}

// Freshly pinned: the primary taskbar may play along briefly while reading (otherwise Windows
// unpins it right away), the follow-up removes it there again
static DWORD g_newPinTick = 0;
static bool IsNewPin(void* g) {
    return !g_newPinKey.empty() && GetTickCount() - g_newPinTick < 3000 && KeyOf(g) == g_newPinKey;
}

using GroupEvt_t = void (*)(void*, void*);
static GroupEvt_t PinnedEvt_orig;
static GroupEvt_t UnpinnedEvt_orig;

static void PinnedEvt_hook(void* self, void* g) {
    std::wstring key;
    if (!TakePinEvent(g, true, key)) {
        PinnedEvt_orig(self, g);
        return;
    }
    int mon = ActionMonitor();
    if (mon >= 1 && mon <= kMaxMon) {
        SetRule(key, 1u << (mon - 1));  // before Windows rebuilds, so that the filter already applies
        Wh_Log(L"-> Pinned on monitor %d: %ls", mon, key.c_str());
    }
    g_newPinKey = key;
    g_newPinTick = GetTickCount();
    PinnedEvt_orig(self, g);
    QueueApply(g);
}

// Really unpinned globally (last taskbar, Start menu, ...): rule removed
static void UnpinnedEvt_hook(void* self, void* g) {
    UnpinnedEvt_orig(self, g);
    std::wstring key;
    if (TakePinEvent(g, false, key)) {
        SetRule(key, 0);
        Wh_Log(L"-> Unpinned: %ls", key.c_str());
    }
}

// Most recently opened jump list: keep the group (AddRef)
static void* g_jumpGroup = nullptr;
static void SetJumpGroup(void* g) {
    if (g) ((IUnknown*)g)->AddRef();
    if (g_jumpGroup) ((IUnknown*)g_jumpGroup)->Release();
    g_jumpGroup = g;
}

// Request from the jump list (ShellExperienceHost) before it unpins globally:
// if the app is pinned on other taskbars too, only remove it from this taskbar -> 1
// "Pin to taskbar" in a trick jump list (app already pinned on another taskbar):
// Windows' PinManager removes the app from the pin list and appends it again - which moves
// it to the end on all taskbars. Instead: just add this taskbar, list unchanged.

static bool SamePinTarget(const std::wstring& path, void* g) {
    std::wstring link = GroupLinkPath(g);
    if (link.empty() || !_wcsicmp(path.c_str(), link.c_str())) return true;
    Target a, b;
    ResolveTarget(path, a);
    ResolveTarget(link, b);
    if (a.exactPath.empty() || b.exactPath.empty()) return true;  // can't tell (e.g. Squirrel apps)
    return a.exactPath == b.exactPath;
}

static bool HandleExtraPin(const wchar_t* app, const wchar_t* path) {
    void* g = g_fakeGroup;
    if (!g || GetTickCount() - g_jumpTick > 20000 || !Pinned(g)) return false;
    if ((app && *app && _wcsicmp(app, AppOf(g)) != 0) ||
        (path && *path && !SamePinTarget(path, g))) {  // another app was pinned
        SetFakeGroup(nullptr);
        return false;
    }
    int mon = MonNumber(g_jumpMon);
    if (mon < 1 || mon > kMaxMon) return false;
    unsigned m = MaskOf(g) | (1u << (mon - 1));
    SetRule(KeyOf(g), m);
    Wh_Log(L"-> Additionally pinned on monitor %d (mask 0x%X, order kept): %ls", mon, m, AppOf(g));
    g_swallowAddKey = KeyOf(g);
    g_swallowAddTick = GetTickCount();
    QueueApply(g);
    SetFakeGroup(nullptr);
    return true;
}

static LRESULT HandleUnpinOne(const wchar_t* app) {
    if (g_s.pinsEverywhere) return 0;  // pins are global: unpin normally
    if (HandleExtraPin(app, nullptr)) return 1;
    void* g = g_jumpGroup;
    Wh_Log(L"-> Unpin request app=%ls jump=%.40ls age=%lu ms pinned=%d mask=0x%X mon=%d", app ? app : L"?",
         g ? AppOf(g) : L"-", GetTickCount() - g_jumpTick, g ? Pinned(g) : 0, g ? MaskOf(g) : 0, MonNumber(g_jumpMon));
    if (!g || GetTickCount() - g_jumpTick > 20000 || !Pinned(g)) return 0;
    int mon = MonNumber(g_jumpMon);
    unsigned mask = MaskOf(g);
    if (mon < 1 || mon > kMaxMon || !InMask(mask, mon)) return 0;
    unsigned rest = mask & ~(1u << (mon - 1));
    if (!rest) return 0;  // last taskbar: let it unpin normally
    SetRule(KeyOf(g), rest);
    Wh_Log(L"-> Unpinned only from monitor %d, stays on mask 0x%X: %ls", mon, rest, AppOf(g));
    QueueApply(g);
    return 1;
}
// Set the shortcut of a group. The pidl is freed unless the group kept this exact pointer
// (CTaskGroup::SetShortcutIDList either copies it or takes ownership - checked, not assumed).
static bool SetGroupShortcut(void* g, PIDLIST_ABSOLUTE pidl) {
    bool ok = SUCCEEDED(pGroupSetShortcut(g, pidl));
    if (!ok || pGroupGetShortcutIDList(g) != pidl) ILFree(pidl);
    return ok;
}

// Jump list: show as "not pinned" on taskbars where the pin isn't allowed
using ShowJump_t = HRESULT (*)(void*, void*, void*, bool);
static ShowJump_t ShowJump_orig;
static HRESULT ShowJump_hook(void* self, void* g, void* item, bool b) {
    void* tl = BarOf(self);
    bool fake = false;
    if (tl && g && Pinned(g) && !ShouldShow(tl, g, nullptr) && pGroupUpdateFlags) {
        g_selfFlag = true;
        pGroupUpdateFlags(g, 0x1, 0);  // UpdateFlags(mask, value)
        fake = !Pinned(g);
    }
    // this taskbar's jump list points to the taskbar's own shortcut (for "Properties")
    bool swapped = false;
    PIDLIST_ABSOLUTE saved = nullptr;
    if (tl && g && Pinned(g) && pGroupSetShortcut && pGroupGetShortcutIDList) {
        RefreshLinkCopies(GroupLinkPath(g));
        std::wstring copy = BarLinkPath(g, MonNumber(GetMonitor_orig(tl)), true);
        PCIDLIST_ABSOLUTE cur = pGroupGetShortcutIDList(g);
        PIDLIST_ABSOLUTE sw = copy.empty() ? nullptr : ILCreateFromPathW(copy.c_str());
        if (cur && sw) {
            saved = ILCloneFull(cur);
            swapped = saved && SetGroupShortcut(g, sw);
        } else if (sw) {
            ILFree(sw);
        }
    }
    HRESULT hr = ShowJump_orig(self, g, item, b);
    if (swapped) SetGroupShortcut(g, saved);
    else if (saved) ILFree(saved);
    if (fake) pGroupUpdateFlags(g, 0x1, 0x1);
    g_selfFlag = false;
    if (tl) {
        g_jumpMon = GetMonitor_orig(tl);
        g_jumpTick = GetTickCount();
        SetFakeGroup(fake ? g : nullptr);
        SetJumpGroup(g);
    }
    Wh_Log(L"-> Jump list on monitor %d%ls: %.40ls", tl ? MonNumber(GetMonitor_orig(tl)) : 0,
         fake ? L" (as not pinned)" : L"", AppOf(g));
    return hr;
}

// The pin list was reread: was this the click on "Pin" in a trick jump list?
using EnumPinned_t = HRESULT (*)(void*, bool, bool, bool);
static EnumPinned_t EnumPinned_orig;
static HRESULT EnumPinned_hook(void* self, bool a, bool b, bool c) {
    HRESULT hr = EnumPinned_orig(self, a, b, c);
    void* g = g_fakeGroup;
    LASTINPUTINFO li{.cbSize = sizeof(li)};
    DWORD now = GetTickCount();
    if (g && now - g_jumpTick < 20000 && GetLastInputInfo(&li) && now - li.dwTime < 8000 && Pinned(g)) {
        int mon = MonNumber(g_jumpMon);
        if (mon >= 1 && mon <= kMaxMon) {
            unsigned m = MaskOf(g) | (1u << (mon - 1));
            SetRule(KeyOf(g), m);
            Wh_Log(L"-> Additionally pinned on monitor %d (mask 0x%X): %ls", mon, m, AppOf(g));
            QueueApply(g);
        }
        SetFakeGroup(nullptr);
    }
    return hr;
}

// ---------- Init

// Pin assignments (pin/unpin via the jump list), exact app ID
static void LoadRules() {
    std::vector<PinRule> rules;
    for (auto& kv : LoadPinRules()) rules.push_back({kv.first, kv.second});
    std::lock_guard<std::mutex> l(g_rulesMx);
    g_s.rules = rules;
}

static void LoadSettings() {
    g_s.foregroundFix = Wh_GetIntSetting(L"foregroundFix") != 0;
    g_s.slideAnimation = Wh_GetIntSetting(L"slideAnimation") != 0;
    g_s.showAppsOnAllTaskbars = Wh_GetIntSetting(L"showAppsOnAllTaskbars") != 0;
    g_s.pinsOnAll = _wcsicmp(WindhawkUtils::StringSetting::make(L"unassignedPins"), L"all") == 0;
    g_s.pinsEverywhere = _wcsicmp(WindhawkUtils::StringSetting::make(L"pinMode"), L"everywhere") == 0;
    auto windowMode = WindhawkUtils::StringSetting::make(L"windowMode");
    g_s.windowMode = !_wcsicmp(windowMode, L"all") ? 2 : !_wcsicmp(windowMode, L"primaryAll") ? 1 : 0;
    g_s.focusThisMonitor = _wcsicmp(WindhawkUtils::StringSetting::make(L"focusScope"), L"thisMonitor") == 0;
    // "notepad.exe" matches the file name, a path matches the full path
    std::vector<std::wstring> always;
    for (int i = 0; i < 100; i++) {
        std::wstring s = Lower(WindhawkUtils::StringSetting::make(L"alwaysOnAllTaskbars[%d]", i).get());
        if (s.empty()) break;
        if (s.find(L'.') == std::wstring::npos) s += L".exe";
        always.push_back(s);
    }
    {
        std::lock_guard<std::mutex> l(g_exemptMx);
        g_exemptCache.clear();
    }
    {
        std::lock_guard<std::mutex> l(g_rulesMx);
        g_s.alwaysAll = always;
    }
    LoadRules();
}
// ---------- "Show my taskbar apps on: All taskbars" while the mod runs
// The mod needs Windows to create every button on every taskbar (MMTaskbarMode = 0) and filters
// them itself. Instead of changing the user's setting, taskbar.dll is told that the value is 0
// while the mod is loaded. taskbar.dll reads it with RegGetValueW, and reads it again when the
// "TraySettings" change is broadcast, so the change applies right away and nothing is written.

static const wchar_t kAdvKey[] = L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced";

using RegGetValueW_t = decltype(&RegGetValueW);
static RegGetValueW_t RegGetValueW_orig;

static bool IsTaskbarDllAddress(void* p) {
    MODULEINFO mi{};
    return g_tb && GetModuleInformation(GetCurrentProcess(), g_tb, &mi, sizeof(mi)) &&
           (char*)p >= (char*)mi.lpBaseOfDll && (char*)p < (char*)mi.lpBaseOfDll + mi.SizeOfImage;
}

// -1: report 0 if the setting is on. Otherwise: the value to report (used while unloading).
static std::atomic<int> g_reportedAppsMode{-1};

static LSTATUS WINAPI RegGetValueW_hook(HKEY hkey, LPCWSTR subKey, LPCWSTR value, DWORD flags,
                                        LPDWORD type, PVOID data, LPDWORD size) {
    void* ret = __builtin_return_address(0);
    LSTATUS r = RegGetValueW_orig(hkey, subKey, value, flags, type, data, size);
    if (r == ERROR_SUCCESS && data && size && *size == sizeof(DWORD) && value &&
        _wcsicmp(value, L"MMTaskbarMode") == 0 && (!type || *type == REG_DWORD) &&
        IsTaskbarDllAddress(ret)) {
        int forced = g_reportedAppsMode;
        if (forced >= 0) *(DWORD*)data = (DWORD)forced;
        else if (g_s.showAppsOnAllTaskbars) *(DWORD*)data = 0;
    }
    return r;
}

// The real value of a setting (the hook only changes reads from taskbar.dll)
static DWORD ReadAdvancedDword(const wchar_t* name, DWORD def) {
    DWORD v = def, cb = sizeof(v);
    if (RegGetValueW(HKEY_CURRENT_USER, kAdvKey, name, RRF_RT_REG_DWORD, nullptr, &v, &cb) != ERROR_SUCCESS)
        return def;
    return v;
}

// Make the taskbar read its settings again. Like the Settings app
// does with a broadcast, but only for the top-level windows of Explorer - the setting change is
// handled by one of them (not by the taskbar windows themselves).
// Sent synchronously: the string lives in the mod, it must not be read after an unload.
static BOOL CALLBACK NotifyExplorerWnd(HWND h, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    if (pid != GetCurrentProcessId()) return TRUE;
    DWORD_PTR r = 0;
    SendMessageTimeoutW(h, WM_SETTINGCHANGE, 0, (LPARAM)L"TraySettings",
                        SMTO_BLOCK | SMTO_ABORTIFHUNG, 2000, &r);
    return TRUE;
}
static void NotifyTaskbar() {
    EnumWindows(NotifyExplorerWnd, 0);
}

static void ApplyAppsMode() {
    NotifyTaskbar();  // also if the real value is 0: the taskbar may still use another mode
    if (!ReadAdvancedDword(L"MMTaskbarEnabled", 0))
        Wh_Log(L"-> Note: \"Show my taskbar on all displays\" is off, there's only one taskbar");
}

static void InitAppsModeHook() {
    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    void* target = kernelBase ? (void*)GetProcAddress(kernelBase, "RegGetValueW") : nullptr;
    if (!target || !WindhawkUtils::SetFunctionHook((RegGetValueW_t)target, RegGetValueW_hook, &RegGetValueW_orig))
        Wh_Log(L"-> RegGetValueW hook FAILED - set \"Show my taskbar apps on\" to \"All taskbars\" manually");
}

// ---------- Pin list (twinui.pcshell.dll, in Explorer and in sihost.exe)
// "Unpin from taskbar" in the jump list ends up in the PinManager, which changes the pin list
// via CPinnedList::Modify(old, nullptr). Before that the mod asks the taskbar thread in
// Explorer: if it only applies to one taskbar, the global unpin is skipped.

static HRESULT (*pGetAppIDForPinned)(void*, PCIDLIST_ABSOLUTE, PWSTR*);
using PinModify_t = HRESULT (*)(void*, PCIDLIST_ABSOLUTE, PCIDLIST_ABSOLUTE, int);
static PinModify_t PinModify_orig;

static HRESULT PinModify_hook(void* self, PCIDLIST_ABSOLUTE from, PCIDLIST_ABSOLUTE to, int caller) {
    if (from && !to) {
        PWSTR app = nullptr;
        if (pGetAppIDForPinned) pGetAppIDForPinned(self, from, &app);
        HWND w = FindWindowExW(HWND_MESSAGE, nullptr, kMsgClass, nullptr);
        DWORD_PTR r = 0;
        bool onlyHere = false;
        if (w) {
            const wchar_t* s = app ? app : L"";
            COPYDATASTRUCT cd{kCopyUnpinOne, (DWORD)((wcslen(s) + 1) * sizeof(wchar_t)), (PVOID)s};
            onlyHere = SendMessageTimeoutW(w, WM_COPYDATA, 0, (LPARAM)&cd, SMTO_ABORTIFHUNG, 1500, &r) && r == 1;
        }
        Wh_Log(L"-> Pin list: unpin %ls (caller %d): %ls", app ? app : L"?", caller,
             !w ? L"taskbar not reachable" : onlyHere ? L"one taskbar only" : L"everywhere");
        if (app) CoTaskMemFree(app);
        if (onlyHere) return S_OK;
    }
    if (!from && to) {  // adding: does it belong to an intercepted "additionally pin"?
        PWSTR app = nullptr;
        if (pGetAppIDForPinned) pGetAppIDForPinned(self, to, &app);
        HWND w = FindWindowExW(HWND_MESSAGE, nullptr, kMsgClass, nullptr);
        DWORD_PTR r = 0;
        bool swallow = false;
        if (w && app) {
            COPYDATASTRUCT cd{0x534B4131, (DWORD)((wcslen(app) + 1) * sizeof(wchar_t)), (PVOID)app};
            swallow = SendMessageTimeoutW(w, WM_COPYDATA, 0, (LPARAM)&cd, SMTO_ABORTIFHUNG, 1500, &r) && r == 1;
        }
        Wh_Log(L"-> Pin list: append %ls (caller %d)%ls", app ? app : L"?", caller, swallow ? L" - intercepted" : L"");
        if (app) CoTaskMemFree(app);
        if (swallow) return S_OK;
    }
    return PinModify_orig(self, from, to, caller);
}

// ---------- Animation (Taskbar.View.dll)
// Taskbar.View.dll (ExplorerExtensions.dll in some builds) is loaded after Explorer starts
static HMODULE TaskbarViewModule() {
    HMODULE m = GetModuleHandleW(L"Taskbar.View.dll");
    return m ? m : GetModuleHandleW(L"ExplorerExtensions.dll");
}

struct Float3 {  // same layout as winrt::Windows::Foundation::Numerics::float3
    float x, y, z;
};
// The float3 is passed by value, so the compiler uses the right convention for the architecture
using Entrance_t = void (*)(void*, int, double, Float3, long long, bool);
static Entrance_t Entrance_orig;
// Window moved to another taskbar: instead of "appearing from below", slide in
// sideways - from the side the window came from
static void Entrance_hook(void* self, int kind, double d, Float3 off, long long dur, bool b) {
    if (g_s.slideAnimation && g_moveDir && GetTickCount() - g_moveTick < 400) {
        kind = 1;  // kind 1 = slide by the offset (kind 0 = from below, kind 2 = zoom)
        off.x = -48.0f * g_moveDir;
        off.y = 0.0f;
        d = 0.0;  // no part from below - purely sideways
        g_moveDir = 0;
        Wh_Log(L"-> Slide in sideways (x=%.0f)", off.x);
    } else if (g_s.slideAnimation && g_swapTick && GetTickCount() - g_swapTick < 350) {
        // Pin turns into a window button (or vice versa) on the same taskbar: the icon stays
        // still instead of appearing from below (otherwise two icons overlap briefly)
        d = 0.0;
        g_swapTick = 0;
        Wh_Log(L"-> Pin/window swap without entrance animation");
    }
    Entrance_orig(self, kind, d, off, dur, b);
}

using ExitAnim_t = void (*)(void*);
static ExitAnim_t ExitAnim_orig;
static void ExitAnim_hook(void* self) {
    // Window gone, the app's pin stays on this taskbar: don't slide down
    // (otherwise two icons overlap briefly)
    if (g_s.slideAnimation && g_swapHideTick && GetTickCount() - g_swapHideTick < 400) {
        g_swapHideTick = 0;
        Wh_Log(L"-> Window/pin swap: without exit animation");
        return;
    }
    ExitAnim_orig(self);
}
// Windows bug: the XAML taskbar recycles button elements. If a former pin (44 px, no label)
// is reused for a window, it gets HasLabel=1 but isn't measured again -> the label is cut off
// (e.g. Steam, whose window is only hidden when closed).
// Workaround: have the width recalculated when the label changes or the element is reused.
// The XAML element of a TaskListButton is located 3 pointers after "this" (as in other
// Windhawk taskbar mods). Check first that a vtable from Taskbar.View.dll is there.
static IUnknown* ButtonUnknown(void* btn) {
    static uintptr_t lo = 0, hi = 0;
    if (!lo) {
        HMODULE tv = TaskbarViewModule();
        MODULEINFO mi{};
        if (!tv || !GetModuleInformation(GetCurrentProcess(), tv, &mi, sizeof(mi))) return nullptr;
        lo = (uintptr_t)mi.lpBaseOfDll;
        hi = lo + mi.SizeOfImage;
    }
    void** p = (void**)btn + 3;
    uintptr_t vt = (uintptr_t)*p;
    if (vt < lo || vt >= hi) return nullptr;
    return (IUnknown*)p;
}
static void RemeasureButton(void* btn) {
    try {
        winrt::Windows::UI::Xaml::UIElement el{nullptr};
        IUnknown* u = ButtonUnknown(btn);
        if (u && SUCCEEDED(u->QueryInterface(
                winrt::guid_of<winrt::Windows::UI::Xaml::UIElement>(), winrt::put_abi(el))) && el)
            el.InvalidateMeasure();
    } catch (...) {
    }
}
using BtnHasLabel_t = void (*)(void*, bool);
static BtnHasLabel_t BtnHasLabel_orig;
static void BtnHasLabel_hook(void* s, bool v) {
    BtnHasLabel_orig(s, v);
    RemeasureButton(s);
}
using BtnPrepared_t = void (*)(void*, bool);
static BtnPrepared_t BtnPrepared_orig;
static void BtnPrepared_hook(void* s, bool v) {
    BtnPrepared_orig(s, v);
    if (v) RemeasureButton(s);
}

static std::atomic<bool> g_taskbarViewHooked{false};
static std::atomic<bool> g_pinListHooked{false};

static void InitAnimationHooks() {
    HMODULE tv = TaskbarViewModule();
    if (!tv || g_taskbarViewHooked.exchange(true)) return;
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK taskbarViewHooks[] = {
        {{L"public: void __cdecl winrt::Taskbar::implementation::TaskListButton::PlayEntranceAnimation(enum winrt::Taskbar::implementation::TaskListButtonEntranceAnimationKind,double,struct winrt::Windows::Foundation::Numerics::float3,class std::chrono::duration<__int64,struct std::ratio<1,10000000> >,bool)"},
         (void**)&Entrance_orig, (void*)Entrance_hook, true},
        {{L"public: void __cdecl winrt::Taskbar::implementation::TaskListButton::StartExitAnimation(void)"},
         (void**)&ExitAnim_orig, (void*)ExitAnim_hook, true},
        {{L"public: void __cdecl winrt::Taskbar::implementation::TaskListButton::HasLabel(bool)"},
         (void**)&BtnHasLabel_orig, (void*)BtnHasLabel_hook, true},
        {{L"public: void __cdecl winrt::Taskbar::implementation::TaskListButton::Prepared(bool)"},
         (void**)&BtnPrepared_orig, (void*)BtnPrepared_hook, true},
    };
    WindhawkUtils::HookSymbols(tv, taskbarViewHooks, ARRAYSIZE(taskbarViewHooks));
    Wh_Log(L"-> Animation %ls, label fix %ls", (Entrance_orig && ExitAnim_orig) ? L"active" : L"partial",
           (BtnHasLabel_orig && BtnPrepared_orig) ? L"active" : L"partial");
}

// Pinning from the jump list (PinManager): if it's only "additionally on this taskbar", do it
// ourselves and don't touch the pin list at all - otherwise the app moves to the end
using PinTrusted_t = HRESULT (*)(void*, PCIDLIST_ABSOLUTE, int);
static PinTrusted_t PinTrusted_orig;
static HRESULT PinTrusted_hook(void* self, PCIDLIST_ABSOLUTE pidl, int caller) {
    HWND w = FindWindowExW(HWND_MESSAGE, nullptr, kMsgClass, nullptr);
    DWORD_PTR r = 0;
    bool extra = false;
    if (w) {
        wchar_t path[MAX_PATH] = {};
        std::wstring data;
        if (pidl) {
            SHGetPathFromIDListW(pidl, path);
            data = path;
            IPropertyStore* ps = nullptr;
            if (SUCCEEDED(SHGetPropertyStoreFromIDList(pidl, GPS_DEFAULT, IID_PPV_ARGS(&ps))) && ps) {
                PROPVARIANT pv;
                PropVariantInit(&pv);
                if (SUCCEEDED(ps->GetValue(kAumid, &pv)) && pv.vt == VT_LPWSTR && pv.pwszVal) {
                    data += L'|';
                    data += pv.pwszVal;
                }
                PropVariantClear(&pv);
                ps->Release();
            }
        }
        COPYDATASTRUCT cd{0x534B5032, (DWORD)((data.size() + 1) * sizeof(wchar_t)), (void*)data.c_str()};  // "SKP2"
        extra = SendMessageTimeoutW(w, WM_COPYDATA, 0, (LPARAM)&cd, SMTO_ABORTIFHUNG, 1500, &r) && r == 1;
    }
    Wh_Log(L"-> PinManager: pin (caller %d)%ls", caller, extra ? L" - additional taskbar only" : L"");
    if (extra) return S_OK;
    return PinTrusted_orig(self, pidl, caller);
}
static bool InitPinListHooks() {
    HMODULE tw = GetModuleHandleW(L"twinui.pcshell.dll");
    if (!tw || g_pinListHooked.exchange(true)) return false;
    // twinui.pcshell.dll
    WindhawkUtils::SYMBOL_HOOK pinListHooks[] = {
        {{L"public: virtual long __cdecl CPinnedList::GetAppIDForPinnedItem(struct _ITEMIDLIST const __unaligned *,unsigned short * *)"},
         (void**)&pGetAppIDForPinned, nullptr, true},
        {{L"public: virtual long __cdecl CPinnedList::Modify(struct _ITEMIDLIST const __unaligned *,struct _ITEMIDLIST const __unaligned *,enum PINNEDLISTMODIFYCALLER)"},
         (void**)&PinModify_orig, (void*)PinModify_hook, true},
        {{L"public: virtual long __cdecl winrt::Windows::Internal::Shell::implementation::PinManager::PinItemFromTrustedCaller(struct _ITEMIDLIST const __unaligned *,enum PINNEDLISTMODIFYCALLER)"},
         (void**)&PinTrusted_orig, (void*)PinTrusted_hook, true},
    };
    WindhawkUtils::HookSymbols(tw, pinListHooks, ARRAYSIZE(pinListHooks));
    Wh_Log(L"-> Pin list (%ls) %ls, PinManager %ls", FileName(GetCommandLineW()).c_str(),
           PinModify_orig ? L"monitored" : L"NOT found", PinTrusted_orig ? L"yes" : L"no");
    return PinModify_orig != nullptr;
}

// Modules that are loaded after the mod: hook them as soon as they're loaded
static void HookLateModules() {
    bool changed = InitPinListHooks();
    if (g_isExplorer && !g_taskbarViewHooked && TaskbarViewModule()) {
        InitAnimationHooks();
        changed = true;
    }
    if (changed) Wh_ApplyHookOperations();
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
static LoadLibraryExW_t LoadLibraryExW_orig;
static HMODULE WINAPI LoadLibraryExW_hook(LPCWSTR name, HANDLE file, DWORD flags) {
    HMODULE module = LoadLibraryExW_orig(name, file, flags);
    if (module && (!g_pinListHooked || (g_isExplorer && !g_taskbarViewHooked))) HookLateModules();
    return module;
}

static void InitLoadLibraryHook() {
    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    void* target = kernelBase ? (void*)GetProcAddress(kernelBase, "LoadLibraryExW") : nullptr;
    if (target) WindhawkUtils::SetFunctionHook((LoadLibraryExW_t)target, LoadLibraryExW_hook, &LoadLibraryExW_orig);
}

BOOL Wh_ModInit() {
    wchar_t exe[MAX_PATH];
    GetModuleFileNameW(nullptr, exe, MAX_PATH);
    if (_wcsicmp(FileName(exe).c_str(), L"explorer.exe") != 0) {  // sihost: pin list only
        LoadSettings();
        if (!InitPinListHooks()) InitLoadLibraryHook();
        return TRUE;
    }
    g_isExplorer = true;
    LoadSettings();
    InitLinkRoot();
    HMODULE tb = GetModuleHandleW(L"taskbar.dll");
    if (!tb) tb = LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!tb) {
        Wh_Log(L"taskbar.dll not found");
        return FALSE;
    }

    g_tb = tb;
    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        // required
        {{L"public: virtual unsigned short const * __cdecl CTaskGroup::GetAppID(void)"},
         (void**)&pGroupGetAppID},
        {{L"public: virtual unsigned long __cdecl CTaskGroup::GetFlags(void)const "},
         (void**)&pGroupGetFlags},
        {{L"public: virtual struct _ITEMIDLIST_ABSOLUTE const * __cdecl CTaskGroup::GetShortcutIDList(void)"},
         (void**)&pGroupGetShortcutIDList},
        {{L"public: virtual long __cdecl CTaskListWnd::TaskCreated(struct ITaskGroup *,struct ITaskItem *)"},
         (void**)&TaskCreated_orig, (void*)TaskCreated_hook},
        {{L"protected: long __cdecl CTaskListWnd::_TaskCreated(struct ITaskGroup *,struct ITaskItem *,int)"},
         (void**)&TaskCreatedInt_orig, (void*)TaskCreatedInt_hook},
        {{L"public: virtual long __cdecl CTaskListWnd::TaskInclusionChanged(struct ITaskGroup *,struct ITaskItem *)"},
         (void**)&TaskIncl_orig, (void*)TaskIncl_hook},
        {{L"public: virtual long __cdecl CTaskListWnd::TaskDestroyed(struct ITaskGroup *,struct ITaskItem *)"},
         (void**)&TaskDestroyed_orig, (void*)TaskDestroyed_hook},
        {{L"public: virtual struct HMONITOR__ * __cdecl CTaskListWnd::GetMonitor(void)"},
         (void**)&GetMonitor_orig, (void*)GetMonitor_hook},
        {{L"public: virtual bool __cdecl CTaskListWnd::IsTaskAllowed(struct ITaskItem *)"},
         (void**)&IsAllowed_orig, (void*)IsAllowed_hook},
        {{L"public: virtual long __cdecl CTaskBand::Launch(struct ITaskGroup *,struct tagPOINT const &,enum LaunchFromTaskbarOptions)"},
         (void**)&Launch_orig, (void*)Launch_hook},
        {{L"protected: void __cdecl CTaskBand::_HandleMonitorChanged(struct HWND__ *)"},
         (void**)&MonChanged_orig, (void*)MonChanged_hook},
        // optional
        {{L"public: virtual struct HWND__ * __cdecl CWindowTaskItem::GetWindow(void)"},
         (void**)&pItemGetWindow, nullptr, true},
        {{L"const CWindowTaskItem::`vftable'{for `ITaskItem'}",
          L"const CWindowTaskItem::`vftable'"},
         (void**)&pWindowItemVft, nullptr, true},
        {{L"public: virtual struct HWND__ * __cdecl CImmersiveTaskItem::GetWindow(void)"},
         (void**)&pImmersiveItemGetWindow, nullptr, true},
        {{L"const CImmersiveTaskItem::`vftable'{for `ITaskItem'}"}, (void**)&pImmersiveItemVft, nullptr, true},
        {{L"public: virtual long __cdecl CTaskGroup::UpdateFlags(unsigned long,unsigned long)"},
         (void**)&pGroupUpdateFlags, nullptr, true},
        {{L"public: virtual long __cdecl CTaskGroup::SetShortcutIDList(struct _ITEMIDLIST_ABSOLUTE const *)"},
         (void**)&pGroupSetShortcut, nullptr, true},
        {{L"public: virtual long __cdecl CTaskListWnd::ShowJumpView(struct ITaskGroup *,struct ITaskItem *,bool)"},
         (void**)&ShowJump_orig, (void*)ShowJump_hook, true},
        {{L"protected: long __cdecl CTaskBand::_EnumPinnedItems(bool,bool,bool)"},
         (void**)&EnumPinned_orig, (void*)EnumPinned_hook, true},
        {{L"protected: struct ITaskBtnGroup * __cdecl CTaskListWnd::_GetTBGroupFromGroup(struct ITaskGroup *,int *)"},
         (void**)&pGetTBGroup, nullptr, true},
        {{L"const CTaskGroup::`vftable'{for `ITaskGroup'}"}, (void**)&pGroupVft, nullptr, true},
        {{L"const CTaskListWnd::`vftable'{for `ITaskListUI'}"}, (void**)&pTlVftUI, nullptr, true},
        {{L"const CTaskListWnd::`vftable'{for `CImpWndProc'}"}, (void**)&pTlVftWnd, nullptr, true},
        {{L"const CTaskListWnd::`vftable'{for `ITaskListSite'}"}, (void**)&pTlVftOther[0], nullptr, true},
        {{L"const CTaskListWnd::`vftable'{for `IDropTarget'}"}, (void**)&pTlVftOther[1], nullptr, true},
        {{L"const CTaskListWnd::`vftable'{for `IStateCapture'}"}, (void**)&pTlVftOther[2], nullptr, true},
        {{L"const CTaskListWnd::`vftable'{for `IObjectWithSite'}"}, (void**)&pTlVftOther[3], nullptr, true},
        {{L"const CTaskListWnd::`vftable'{for `CTaskUnknown'}"}, (void**)&pTlVftOther[4], nullptr, true},
        {{L"public: virtual long __cdecl CTaskGroup::GetItemFromWindow(struct HWND__ *,struct ITaskItem * *)"},
         (void**)&pGroupItemFromWindow, nullptr, true},
        {{L"public: virtual int __cdecl CTaskBtnGroup::GetNumItems(void)"}, (void**)&pBtnGetNumItems, nullptr, true},
        {{L"public: virtual struct ITaskItem * __cdecl CTaskBtnGroup::GetTaskItem(int)"}, (void**)&pBtnGetTaskItem, nullptr, true},
        {{L"public: virtual int __cdecl CTaskListWnd::GetButtonGroupCount(void)"}, (void**)&pGetButtonGroupCount, nullptr, true},
        // SetOrder_hook only acts if _GetTBGroupFromGroup was found
        {{L"public: virtual void __cdecl CTaskListWnd::SetRelativeTaskOrder(struct ITaskGroup *,int)"},
         (void**)&SetOrder_orig, (void*)SetOrder_hook, true},
        {{L"public: virtual void __cdecl CTaskListWnd::HandleTaskGroupPinned(struct ITaskGroup *)"},
         (void**)&PinnedEvt_orig, (void*)PinnedEvt_hook, true},
        {{L"public: virtual void __cdecl CTaskListWnd::HandleTaskGroupUnpinned(struct ITaskGroup *)"},
         (void**)&UnpinnedEvt_orig, (void*)UnpinnedEvt_hook, true},
    };
    if (!WindhawkUtils::HookSymbols(tb, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookSymbols failed");
        return FALSE;
    }
    InitDpaOffset();
    Wh_Log(L"-> Jump list pins %ls, startup sync %ls, pin events %ls",
           (pGroupUpdateFlags && ShowJump_orig && EnumPinned_orig) ? L"active" : L"INCOMPLETE",
           (g_dpaOffset > 0 && pGroupVft && pGroupItemFromWindow && pBtnGetNumItems && pBtnGetTaskItem) ? L"active" : L"MISSING",
           (PinnedEvt_orig && UnpinnedEvt_orig) ? L"active" : L"MISSING");

    InitPinListHooks();
    InitAnimationHooks();
    if (!g_pinListHooked || !g_taskbarViewHooked) InitLoadLibraryHook();
    InitAppsModeHook();
    g_initTick = GetTickCount();
    // Taskbar is already up (mod update / loaded late): start right away, sync follows
    if (RunOnTrayThread(EnsureMsgWnd)) Wh_Log(L"-> Taskbar already running - sync scheduled");
    Wh_Log(L"v%ls active", WH_MOD_VERSION);
    return TRUE;
}

void Wh_ModAfterInit() {
    HookLateModules();  // loaded while the mod was initializing
    if (g_isExplorer) ApplyAppsMode();  // hooks are active now
}

void Wh_ModSettingsChanged() {
    bool appsMode = g_s.showAppsOnAllTaskbars;
    LoadSettings();
    Wh_Log(L"Settings reloaded");
    if (!g_isExplorer) return;
    if (appsMode != g_s.showAppsOnAllTaskbars) ApplyAppsMode();
    RunOnTrayThread(SyncAll);  // e.g. apply "Unassigned pins" right away
}

// Give the taskbar its normal buttons back: a change of the "show taskbar apps on" mode makes
// it recreate all buttons. The hooks are still active here, so a different mode is reported and
// the taskbar reads it; after the hooks are removed (Wh_ModUninit) it reads the real value and
// rebuilds everything without the mod's filtering.
void Wh_ModBeforeUninit() {
    // Remove the message window with its timers first, while the hooks are
    // still installed - its work calls the original functions of the hooks
    g_unloading = true;
    if (g_msgWnd) SendMessageW(g_msgWnd, kMsgCleanup, 0, 0);
    if (!g_isExplorer) return;
    DWORD real = ReadAdvancedDword(L"MMTaskbarMode", 0);
    DWORD reported = g_s.showAppsOnAllTaskbars ? 0 : real;
    if (real == reported) {
        g_reportedAppsMode = real == 1 ? 2 : 1;
        NotifyTaskbar();
    }
}

void Wh_ModUninit() {
    if (g_msgWnd) {
        SendMessageW(g_msgWnd, kMsgCleanup, 0, 0);  // runs the cleanup on the taskbar thread
    } else {
        SetFakeGroup(nullptr);
        SetJumpGroup(nullptr);
        ReleaseWindows(true);
    }
    UnregisterClassW(kMsgClass, ModInstance());
    // The hooks are removed at this point: taskbar.dll reads the real setting again
    if (g_isExplorer) NotifyTaskbar();
    Wh_Log(L"Uninit");
}
