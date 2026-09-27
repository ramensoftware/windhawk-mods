// ==WindhawkMod==
// @id              taskbar-independent-per-monitor
// @name            Independent taskbar per monitor
// @name:de-DE      Eigenständige Taskleiste pro Monitor
// @description     Every taskbar acts like its own main taskbar: only the windows of its monitor, its own pins (pin/unpin per taskbar via right-click), per-taskbar pin properties
// @description:de-DE Jede Taskleiste wie eine eigene Hauptleiste: nur die Fenster ihres Monitors, eigene Pins (per Rechtsklick pro Leiste anheften/lösen), eigene Pin-Eigenschaften
// @version         1.4.0
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

With several monitors, **every taskbar acts like its own main taskbar** (Windows 11):

- **Only its own windows:** each taskbar shows only the windows that are on its monitor. When a window
  is dragged to another monitor, its button slides in sideways on the new taskbar.
- **Own pins per taskbar:** right-click on the taskbar you want → *Pin to taskbar* / *Unpin from
  taskbar* only applies to that taskbar. No Explorer restart, the pin order stays unchanged.
- **Own pin properties per taskbar:** right-click a pin → right-click the app name → *Properties*
  only changes the pin of that taskbar (e.g. Steam with `steam://open/friends` on one monitor and the
  normal Steam window on another).
- **Bring running apps to the front:** clicking the pin of an app that is already running brings its
  window to the front instead of starting a second instance (Shift+click or middle-click still starts
  a new one).
- **Fixes a Windows bug:** when an app shows a window again that it had only hidden (e.g. Steam), the
  taskbar button could stay narrow with a cut-off label.

## Settings
Windows can be shown on the taskbar of their own monitor only, additionally on the primary taskbar, or
on all taskbars - with a list of programs whose windows always appear on all taskbars. Pins can be per
taskbar or the same everywhere.

## Required Windows settings
The mod sets these itself (setting *Configure the taskbar settings automatically*) and restores the
previous values when it's disabled or removed:
- *Show my taskbar on all displays* = **on**
- *When using multiple displays, show my taskbar apps on* = **All taskbars**

## Notes
- Windows 11 taskbar only (tested with 24H2/25H2 and *Combine taskbar buttons: Never*).
- Not compatible with the Windows 10 taskbar (ExplorerPatcher, "Windows 10 taskbar on Windows 11").
- *Disable grouping on the taskbar* changes the same parts of the taskbar and isn't needed on
  Windows 11 (use *Combine taskbar buttons: Never*). Using both at the same time isn't supported.
- Pins that aren't assigned to a taskbar yet (e.g. from before the mod) appear on the primary taskbar -
  this can be changed in the settings.
- After switching the *Pins* setting, newly shown pins are added at the end; Windows restores the
  usual order on the next Explorer restart.
- On the first start Windhawk downloads symbols from Microsoft, which can take a minute or two.

## Deutsch
Jede Taskleiste verhält sich wie eine eigene Hauptleiste: Sie zeigt nur die Fenster ihres Monitors
und hat eigene Pins. „An Taskleiste anheften“ / „Von Taskleiste lösen“ per Rechtsklick gilt nur für
die Leiste, auf der man klickt. „Eigenschaften“ eines Pins (Rechtsklick auf den App-Namen in der Jump
List) gelten ebenfalls nur für diese Leiste. Die nötigen Windows-Einstellungen setzt die Mod selbst
und stellt beim Entfernen die vorherigen Werte wieder her.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- windowMode: monitor
  $name: Windows on the taskbar
  $name:de-DE: Fenster auf der Taskleiste
  $description: Which windows each taskbar shows
  $description:de-DE: Welche Fenster jede Taskleiste zeigt
  $options:
  - monitor: Only the windows on its own monitor
  - primaryAll: Primary taskbar shows all windows, the others only their own
  - all: Every taskbar shows all windows
  $options:de-DE:
  - monitor: Nur die Fenster des eigenen Monitors
  - primaryAll: Hauptleiste zeigt alle Fenster, die anderen nur ihre eigenen
  - all: Jede Leiste zeigt alle Fenster
- alwaysOnAllTaskbars: [""]
  $name: Programs on all taskbars
  $name:de-DE: Programme auf allen Taskleisten
  $description: Windows of these programs appear on every taskbar, e.g. discord.exe
  $description:de-DE: Fenster dieser Programme erscheinen auf jeder Taskleiste, z.B. discord.exe
- pinMode: perTaskbar
  $name: Pins
  $name:de-DE: Pins
  $description: Own pins per taskbar (pin/unpin via right-click applies only to that taskbar), or the same pins on every taskbar like Windows does
  $description:de-DE: Eigene Pins pro Taskleiste (Anheften/Lösen per Rechtsklick gilt nur für diese Leiste) oder überall dieselben Pins wie bei Windows
  $options:
  - perTaskbar: Own pins per taskbar
  - everywhere: Same pins on every taskbar
  $options:de-DE:
  - perTaskbar: Eigene Pins pro Taskleiste
  - everywhere: Überall dieselben Pins
- unassignedPins: primary
  $name: Unassigned pins
  $name:de-DE: Pins ohne Zuordnung
  $description: Where pins appear that aren't assigned to a taskbar yet (e.g. pinned before the mod or by Windows itself)
  $description:de-DE: Wo Pins erscheinen, die noch keiner Leiste zugeordnet sind (z.B. aus der Zeit vor der Mod oder von Windows selbst angeheftet)
  $options:
  - primary: Only on the primary taskbar
  - all: On all taskbars
  $options:de-DE:
  - primary: Nur auf der Hauptleiste
  - all: Auf allen Leisten
- foregroundFix: true
  $name: Bring running apps to the front
  $name:de-DE: Laufende App nach vorn holen
  $description: Clicking the pin of an app that is already running brings its window to the front instead of starting it again. Shift+click or middle-click still starts a new instance.
  $description:de-DE: Klick auf einen Pin, dessen App schon läuft, holt das Fenster nach vorn statt die App neu zu starten. Shift+Klick oder Mittelklick startet trotzdem neu.
- focusScope: anyMonitor
  $name: Which windows are brought to the front
  $name:de-DE: Welche Fenster nach vorn geholt werden
  $options:
  - anyMonitor: From any monitor (also hidden in the tray)
  - thisMonitor: Only windows on the monitor of the clicked taskbar
  $options:de-DE:
  - anyMonitor: Von jedem Monitor (auch versteckt im Infobereich)
  - thisMonitor: Nur Fenster auf dem Monitor der angeklickten Leiste
- slideAnimation: true
  $name: Slide in sideways
  $name:de-DE: Seitlich hereingleiten
  $description: When a window is dragged to another monitor, its button slides in from the side the window came from
  $description:de-DE: Wird ein Fenster auf einen anderen Monitor gezogen, gleitet sein Button von der Seite herein, von der das Fenster kommt
- ctrlClickMenu: true
  $name: Ctrl+click menu
  $name:de-DE: Strg+Klick-Menü
  $description: Ctrl+left-click on a pin opens a menu to choose its monitors. Usually not needed - just pin/unpin via right-click on the taskbar you want.
  $description:de-DE: Strg+Linksklick auf einen Pin öffnet ein Menü zur Monitor-Zuordnung. Normalerweise nicht nötig - einfach per Rechtsklick auf der gewünschten Leiste anheften oder lösen.
- autoConfigure: true
  $name: Configure the taskbar settings automatically
  $name:de-DE: Taskleisten-Einstellungen automatisch setzen
  $description: The mod needs "Show my taskbar on all displays" = on and "When using multiple displays, show my taskbar apps on" = All taskbars. If enabled, the mod sets these and restores the previous values when it's disabled or removed (or this option is turned off).
  $description:de-DE: Die Mod braucht "Taskleiste auf allen Anzeigen anzeigen" = an und "Beim Verwenden mehrerer Anzeigen Apps anzeigen auf" = Alle Taskleisten. Ist das an, stellt die Mod das selbst ein und stellt beim Deaktivieren/Entfernen (oder Ausschalten dieser Option) die vorherigen Werte wieder her.
- traceLog: false
  $name: Diagnostic log
  $name:de-DE: Diagnose-Log
  $description: Writes every decision of the mod to the Windhawk log (for troubleshooting)
  $description:de-DE: Schreibt jede Entscheidung der Mod ins Windhawk-Log (für Fehlersuche)
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <propsys.h>
#include <psapi.h>
#include <cstdarg>
#include <cstdio>
#include <cwchar>
#include <cwctype>
#include <map>
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
    bool ctrlClickMenu;
    bool autoConfigure;
    bool traceLog;
    std::vector<PinRule> rules;
} g_s;

static std::mutex g_rulesMx;
static bool g_isExplorer = false;

#define TLOG(...)                          \
    do {                                   \
        if (g_s.traceLog) Wh_Log(__VA_ARGS__); \
    } while (0)

// ---------- Callable taskbar functions (resolved only, not hooked)

static PCWSTR (*pGroupGetAppID)(void*);
static int (*pGroupGetNumItems)(void*);
static DWORD (*pGroupGetFlags)(void*);
static PCIDLIST_ABSOLUTE (*pGroupGetShortcutIDList)(void*);
static HRESULT (*pGroupUpdateFlags)(void*, DWORD, DWORD);
static HWND (*pItemGetWindow)(void*);   // CWindowTaskItem::GetWindow (resolved only)
static void* pWindowItemVft = nullptr;  // class identity of CWindowTaskItem
static HRESULT (*pGroupItemFromWindow)(void*, HWND, void**) = nullptr;  // CTaskGroup::GetItemFromWindow

// ---------- Helpers

static std::wstring Lower(std::wstring s) {
    for (auto& c : s) c = (wchar_t)towlower(c);
    return s;
}

// ---------- Monitor numbers: sorted by X position, 1 = leftmost

static BOOL CALLBACK MonEnum(HMONITOR m, HDC, LPRECT r, LPARAM lp) {
    auto* v = (std::vector<std::pair<LONG, HMONITOR>>*)lp;
    v->push_back({r->left, m});
    return TRUE;
}

static int MonNumber(HMONITOR m) {
    std::vector<std::pair<LONG, HMONITOR>> v;
    EnumDisplayMonitors(nullptr, nullptr, MonEnum, (LPARAM)&v);
    std::sort(v.begin(), v.end(),
              [](auto& a, auto& b) { return a.first < b.first; });
    for (size_t i = 0; i < v.size(); i++) {
        if (v[i].second == m) return (int)i + 1;
    }
    return 0;
}

static bool MonIsPrimary(HMONITOR m) {
    MONITORINFO mi{sizeof(mi)};
    return m && GetMonitorInfoW(m, &mi) && (mi.dwFlags & MONITORINFOF_PRIMARY);
}


static bool InMask(unsigned mask, int mon) {
    return mon >= 1 && mon <= 8 && (mask & (1u << (mon - 1)));
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
    using Fn = LONG(WINAPI*)(HANDLE, UINT32*, PWSTR);
    static Fn fn = (Fn)GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "GetApplicationUserModelId");
    std::wstring r;
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    if (!fn || !pid) return r;
    HANDLE p = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!p) return r;
    wchar_t buf[512];
    UINT32 len = ARRAYSIZE(buf);
    if (fn(p, &len, buf) == ERROR_SUCCESS) r = buf;
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
    HWND hidden = nullptr;
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
    if (GetWindow(h, GW_OWNER)) return TRUE;
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
    if (!match) return TRUE;

    if (IsWindowVisible(h)) {
        if (c->onlyMon && MonitorFromWindow(h, MONITOR_DEFAULTTONEAREST) != c->onlyMon) return TRUE;
        if (!c->visible) c->visible = h;
    } else if (!c->onlyMon && !c->hidden) {  // hidden: prefer a normal launch
        RECT r{};
        GetWindowRect(h, &r);
        if (r.right - r.left >= 300 && r.bottom - r.top >= 200) c->hidden = h;
    }
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
        TLOG(L"  Pin has launch arguments - launching normally");
        return false;
    }
    EnumWindows(EnumProc, (LPARAM)&ctx);
    HWND h = ctx.visible ? ctx.visible : ctx.hidden;
    TLOG(L"  Search: app=%ls lnk=%ls exe=%ls name=%ls -> visible=%p hidden=%p",
         ctx.appId.c_str(), path, ctx.tgt.exactPath.c_str(),
         ctx.tgt.name.c_str(), (void*)ctx.visible, (void*)ctx.hidden);
    if (!h) return false;

    if (!IsWindowVisible(h)) ShowWindow(h, SW_SHOW);
    if (IsIconic(h)) ShowWindow(h, SW_RESTORE);
    if (!SetForegroundWindow(h)) SwitchToThisWindow(h, TRUE);
    TLOG(L"  -> brought existing window %p to the front", (void*)h);
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
static std::map<void*, void*> g_itemGroup;     // window button -> group
static std::map<HWND, HMONITOR> g_winMon;      // window -> last known monitor
static std::map<void*, std::set<HMONITOR>> g_itemBars; // window button -> taskbars (monitors) it is on
static HWINEVENTHOOK g_winEvent = nullptr;
// Message window on the taskbar thread. The WinEvent hook and the timers belong to this
// thread and must be removed there too - otherwise Windows calls WinEventProc in the
// unloaded DLL (crash).
static HWND g_msgWnd = nullptr;
static const wchar_t kMsgClass[] = L"TaskbarIndependentPerMonitorMsg";
static const UINT kMsgCleanup = WM_APP + 0x51;
static const UINT_PTR kReevalTimerId = 1;
static const UINT_PTR kApplyTimerId = 2;
static const UINT_PTR kSyncTimerId = 3;
static const UINT_PTR kStableTimerId = 4;
static void SyncTimerProc();
static const ULONG_PTR kCopyUnpinOne = 0x534B5031;  // WM_COPYDATA from the jump list ("SKP1")
static void ApplyTimerProc();
static void ReleasePending();
static void SetFakeGroup(void* g);
static void SetJumpGroup(void* g);
static LRESULT HandleUnpinOne(const wchar_t* app);
static bool HandleExtraPin(const wchar_t* app);
extern void* g_fakeGroup;
extern DWORD g_jumpTick;
extern std::wstring g_swallowAddKey;
extern DWORD g_swallowAddTick;

// Window of a button - only if it is definitely a CWindowTaskItem
static HWND ItemWindow(void* item) {
    if (!item || !pItemGetWindow || !pWindowItemVft) return nullptr;
    if (*(void**)item != pWindowItemVft) return nullptr;
    HWND h = pItemGetWindow(item);
    return (h && IsWindow(h)) ? h : nullptr;
}

static HMONITOR ItemMonitor(void* item) {
    HWND h = ItemWindow(item);
    return h ? MonitorFromWindow(h, MONITOR_DEFAULTTONEAREST) : nullptr;
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
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    std::wstring exe = FileName(ProcPath(pid));
    bool r = false;
    {
        std::lock_guard<std::mutex> l(g_rulesMx);
        for (auto& a : g_s.alwaysAll) {
            if (a == exe) r = true;
        }
    }
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

static void QueueReeval(void* item);
static void EnsureWinEvent();
extern int g_moveDir;
extern DWORD g_moveTick;
extern DWORD g_swapTick;
extern DWORD g_swapHideTick;

static HRESULT TaskCreated_hook(void* self, void* g, void* item) {
    EnsureWinEvent();  // runs on the taskbar thread (has a message loop)
    {
        std::lock_guard<std::mutex> l(g_mx);
        g_bars.insert(self);
        if (item && g) {
            g_itemGroup[item] = g;
            if (HWND h = ItemWindow(item))
                g_winMon[h] = MonitorFromWindow(h, MONITOR_DEFAULTTONEAREST);
        }
    }
    // Don't pass on a pin without a window that isn't allowed here at all.
    // Otherwise TaskCreated creates a button group itself and reports it to the XAML taskbar
    // (ghost pin on the primary taskbar), even if _TaskCreated is filtered.
    if (!item && g && pGroupGetFlags && (pGroupGetFlags(g) & 0x1) &&
        !ShouldShow(self, g, nullptr)) {
        PCWSTR id = pGroupGetAppID ? pGroupGetAppID(g) : nullptr;
        TLOG(L"-> Pin skipped (monitor %d): %.40ls",
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
static int g_itemRefMode = -1;  // -1 unknown, 1 = GetItemFromWindow adds a reference, 0 = doesn't

// Window button of a group for a window (or nullptr); free with ReleaseItem
static void* GroupItem(void* g, HWND h) {
    void* item = nullptr;
    if (!pGroupItemFromWindow || FAILED(pGroupItemFromWindow(g, h, &item)) || !item) return nullptr;
    if (g_itemRefMode < 0) {  // measure once
        ULONG a = ((IUnknown*)item)->AddRef();
        ((IUnknown*)item)->Release();
        void* item2 = nullptr;
        pGroupItemFromWindow(g, h, &item2);
        ULONG b = ((IUnknown*)item)->AddRef();
        ((IUnknown*)item)->Release();
        g_itemRefMode = (b == a + 1) ? 1 : 0;
        if (g_itemRefMode == 1 && item2) ((IUnknown*)item2)->Release();
    }
    return item;
}
static void ReleaseItem(void* item) {
    if (item && g_itemRefMode == 1) ((IUnknown*)item)->Release();
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
static int (*pBtnGetType)(void*) = nullptr;             // CTaskBtnGroup::GetGroupType

// All button groups of a taskbar: CDPA<ITaskBtnGroup> in CTaskListWnd. The offset is taken
// from the code of GetButtonGroupCount ("mov rdx,[rcx+disp32]") and read at runtime.
// With "never combine" every window has its own group - _GetTBGroupFromGroup only finds
// the first one.
static HMODULE g_tb = nullptr;
static bool g_safeMode = false;  // after a crash loop: risky parts disabled
static void* pGetButtonGroupCount = nullptr;
static int g_dpaOffset = -1;                    // from tl; -1 = unknown/disabled

static void InitDpaOffset() {
    const unsigned char* p = (const unsigned char*)pGetButtonGroupCount;
    if (p && Readable(p, 8) && p[0] == 0x48 && p[1] == 0x8B && p[2] == 0x91) {
        int d = *(const int*)(p + 3);
        if (d > 0 && d < 0x1000) g_dpaOffset = d;
    }
    Wh_Log(L"-> Button group list %ls (0x%X)", g_dpaOffset > 0 ? L"found" : L"NOT found", g_dpaOffset);
}

static std::vector<void*> BtnGroupsOf(void* tl, void* g) {
    std::vector<void*> r;
    if (g_dpaOffset <= 0 || !g_tb || g_safeMode) return r;
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

// Does the taskbar already have a button for window h of group g?
static bool ItemOnBar(void* tl, void* g, HWND h) {
    if (!pBtnGetNumItems || !pBtnGetTaskItem || g_dpaOffset <= 0 || g_safeMode) {
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
    std::vector<void*> foreign;
    int cnt = 0;
    for (void* tbg : BtnGroupsOf(tl, g)) {
        int c = pBtnGetNumItems(tbg);
        cnt += c;
        for (int k = 0; k < c; k++) {
            void* it = pBtnGetTaskItem(tbg, k);
            HWND h = ItemWindow(it);
            HMONITOR m = h ? MonitorFromWindow(h, MONITOR_DEFAULTTONEAREST) : nullptr;
            if (it && m && !WinAllowed(h, m, bar)) foreign.push_back(it);
        }
    }
    for (void* it : foreign) {
        if (Pinned(g) && ShouldShow(tl, g, nullptr)) g_swapTick = g_swapHideTick = GetTickCount();  // pin stays here
        TaskDestroyed_orig(tl, g, it);
        std::lock_guard<std::mutex> l(g_mx);
        auto f = g_itemBars.find(it);
        if (f != g_itemBars.end()) f->second.erase(bar);
    }
    if (!foreign.empty())
        TLOG(L"-> Monitor %d: removed %d foreign window(s) (of %d) %.40ls", MonNumber(bar), (int)foreign.size(),
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

static void PruneTimerProc() {
    KillTimer(g_msgWnd, kPruneTimerId);
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
static int g_allowDiag = 0;
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
    if (!ok && g_s.traceLog && g_allowDiag++ < 20)
        TLOG(L"-> Window %p not allowed on the taskbar of monitor %d", (void*)ItemWindow(item), MonNumber(GetMonitor_orig(tl)));
    return ok;
}
static HRESULT TaskCreatedInt_hook(void* self, void* g, void* item, int a) {
    void* tl = (char*)self + 0x28;
    if (!ShouldShow(tl, g, item)) {
        // window is elsewhere: show the pin if it's allowed on this taskbar
        if (item && pGroupGetFlags && (pGroupGetFlags(g) & 0x1) && ShouldShow(tl, g, nullptr))
            return CreateOnBar(self, g, nullptr, a);
        return S_OK;
    }
    if (item && GetMonitor_orig) {
        std::lock_guard<std::mutex> l(g_mx);
        g_itemBars[item].insert(GetMonitor_orig(tl));
    }
    if (item && PinOnlyOnBar(self, g)) g_swapTick = g_swapHideTick = GetTickCount();
    return CreateOnBar(self, g, item, a);
}

static HRESULT TaskIncl_hook(void* self, void* g, void* item) {
    // Pins are rebuilt (pin/unpin) through here - skip pins that aren't allowed
    if (!item && g && pGroupGetFlags && (pGroupGetFlags(g) & 0x1) &&
        !ShouldShow(self, g, nullptr))
        return S_OK;
    return TaskIncl_orig(self, g, item);
}

static bool Pinned(void* g);
static void RemovePinButton(void* tl, void* g);
static bool OtherWindowOnBar(void* tl, void* g, void* item);

static HRESULT TaskDestroyed_hook(void* self, void* g, void* item) {
    if (item) {
        std::lock_guard<std::mutex> l(g_mx);
        g_itemGroup.erase(item);
        g_itemBars.erase(item);
    }
    if (item && g && Pinned(g) && ShouldShow(self, g, nullptr)) g_swapTick = g_swapHideTick = GetTickCount();
    HRESULT hr = TaskDestroyed_orig(self, g, item);
    // Last window of a pinned app closed -> Windows turns it into a pin.
    // Remove it again on taskbars where the pin isn't allowed.
    if (item && g && Pinned(g) && !ShouldShow(self, g, nullptr) &&
        !OtherWindowOnBar(self, g, item)) {
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

static std::set<void*> g_dirty;
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
    if (was && pGroupUpdateFlags) {
        pGroupUpdateFlags(g, 0x1, 0);
        if (Pinned(g)) pGroupUpdateFlags(g, 0, 0x1);  // parameter order unknown
    }
    TaskDestroyed_orig(tl, g, nullptr);
    if (was && !Pinned(g) && pGroupUpdateFlags) pGroupUpdateFlags(g, 0x1, 0x1);
    if (was && !Pinned(g)) Wh_Log(L"-> Pin flag NOT restored!");
}

// Is another window of the same group on this taskbar?
static bool OtherWindowOnBar(void* tl, void* g, void* item) {
    HMONITOR bar = GetMonitor_orig(tl);
    std::lock_guard<std::mutex> l(g_mx);
    for (auto& kv : g_itemGroup)
        if (kv.second == g && kv.first != item && ItemMonitor(kv.first) == bar) return true;
    return false;
}

// Only touch affected taskbars (touching everything everywhere caused flicker).
// Windows usually creates the window button on the new taskbar itself. The mod only
// removes it from the old taskbars - a pinned group stays there as a pin;
// only where the pin isn't allowed, it is removed as well.
static void ReevalItem(void* item, void* g, const std::set<void*>& bars) {
    bool pinned = Pinned(g);
    HMONITOR now = ItemMonitor(item);
    if (!now) return;
    std::set<HMONITOR> on;
    bool known = false;
    {
        std::lock_guard<std::mutex> l(g_mx);
        auto it = g_itemBars.find(item);
        if (it != g_itemBars.end()) { on = it->second; known = true; }
    }
    int removed = 0, added = 0;
    HWND h = ItemWindow(item);
    for (void* tl : bars) {
        HMONITOR bm = GetMonitor_orig(tl);
        if (WinAllowed(h, now, bm)) {
            if (!on.count(bm)) { CreateOnBar((char*)tl - 0x28, g, item, 1); added++; }
        } else if (!known || on.count(bm)) {
            if (pinned && ShouldShow(tl, g, nullptr)) g_swapTick = g_swapHideTick = GetTickCount();
            TaskDestroyed_orig(tl, g, item);
            if (pinned && !ShouldShow(tl, g, nullptr) && !OtherWindowOnBar(tl, g, item))
                RemovePinButton(tl, g);
            removed++;
        }
    }
    TLOG(L"-> Window on monitor %d: removed from %d taskbar(s), created on %d", MonNumber(now), removed, added);
    std::lock_guard<std::mutex> l(g_mx);
    g_itemBars[item] = {now};
}

static void ReevalTimerProc() {
    KillTimer(g_msgWnd, kReevalTimerId);
    g_reevalTimer = false;
    std::set<void*> bars;
    std::vector<std::pair<void*, void*>> todo;
    {
        std::lock_guard<std::mutex> l(g_mx);
        bars = g_bars;
        for (auto& kv : g_itemGroup)
            if (g_reevalAll || g_dirty.count(kv.first)) todo.push_back(kv);
        g_dirty.clear();
        g_reevalAll = false;
    }
    for (auto& t : todo) ReevalItem(t.first, t.second, bars);
    TLOG(L"-> Redistributed %d window(s)", (int)todo.size());
}

static void QueueReeval(void* item) {
    {
        std::lock_guard<std::mutex> l(g_mx);
        if (item) g_dirty.insert(item); else g_reevalAll = true;
    }
    if (!g_reevalTimer && g_msgWnd)
        g_reevalTimer = SetTimer(g_msgWnd, kReevalTimerId, 150, nullptr) != 0;
}


// Windows reports a monitor change itself before it shows the button on the new taskbar -
// remember the direction for the sideways slide-in here
using MonChanged_t = void (*)(void*, HWND);
static MonChanged_t MonChanged_orig;
static void MonChanged_hook(void* self, HWND h) {
    HMONITOR now = MonitorFromWindow(h, MONITOR_DEFAULTTONEAREST);
    HMONITOR old = nullptr;
    {
        std::lock_guard<std::mutex> l(g_mx);
        auto it = g_winMon.find(h);
        if (it != g_winMon.end()) old = it->second;
    }
    MONITORINFO a{sizeof(a)}, b{sizeof(b)};
    if (old && now && old != now && GetMonitorInfoW(old, &a) && GetMonitorInfoW(now, &b)) {
        g_moveDir = b.rcMonitor.left > a.rcMonitor.left ? 1 : -1;
        g_moveTick = GetTickCount();
    }
    MonChanged_orig(self, h);
}
static void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD, HWND h, LONG idObj, LONG idChild,
                                  DWORD, DWORD) {
    if (!h || idObj != OBJID_WINDOW || idChild != CHILDID_SELF) return;
    HMONITOR now = MonitorFromWindow(h, MONITOR_DEFAULTTONEAREST);
    HMONITOR old = nullptr;
    std::vector<void*> moved;
    {
        std::lock_guard<std::mutex> l(g_mx);
        auto it = g_winMon.find(h);
        if (it == g_winMon.end() || it->second == now) return;
        old = it->second;
        it->second = now;
        for (auto& kv : g_itemGroup)
            if (ItemWindow(kv.first) == h) moved.push_back(kv.first);
    }
    if (!moved.empty() && old && now) {  // direction for the entrance animation
        MONITORINFO a{sizeof(a)}, b{sizeof(b)};
        if (GetMonitorInfoW(old, &a) && GetMonitorInfoW(now, &b)) {
            g_moveDir = b.rcMonitor.left > a.rcMonitor.left ? 1 : -1;
            g_moveTick = GetTickCount();
        }
    }
    for (void* item : moved) QueueReeval(item);
    if (!moved.empty()) TLOG(L"-> Window %p moved to monitor %d", (void*)h, MonNumber(now));
}

// DIAG: dump all button groups of all taskbars to the log (via WM_COPYDATA "SKD1")
static void DumpBars() {
    std::set<void*> bars;
    {
        std::lock_guard<std::mutex> l(g_mx);
        bars = g_bars;
    }
    for (void* tl : bars) {
        if (g_dpaOffset <= 0) break;
        char* dpa = *(char**)((char*)tl + g_dpaOffset);
        if (!dpa || !Readable(dpa, 0x10)) continue;
        int cnt = *(int*)dpa;
        void** pp = *(void***)(dpa + 0x08);
        Wh_Log(L"== Taskbar monitor %d: %d groups", MonNumber(GetMonitor_orig(tl)), cnt);
        for (int k = 0; k < cnt && k < 200; k++) {
            void* tbg = pp[k];
            if (!tbg || !Readable(tbg, 8)) continue;
            void** vt = *(void***)tbg;
            void* g = ((void* (*)(void*))vt[6])(tbg);
            int ni = pBtnGetNumItems(tbg);
            std::wstring items;
            for (int j = 0; j < ni; j++) {
                void* it = pBtnGetTaskItem(tbg, j);
                HWND h = ItemWindow(it);
                wchar_t b[96];
                swprintf(b, 96, L" [%p M%d]", (void*)h, h ? MonNumber(MonitorFromWindow(h, MONITOR_DEFAULTTONEAREST)) : 0);
                items += b;
            }
            Wh_Log(L"   #%d type=%d items=%d%ls %.30ls", k, pBtnGetType ? pBtnGetType(tbg) : -1, ni, items.c_str(), AppOf(g));
        }
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
    if (msg == WM_TIMER && wp == kStableTimerId) {  // stable for 60 s: reset the counter
        KillTimer(h, kStableTimerId);
        Wh_SetIntValue(L"bootCount", 0);
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
    if (msg == WM_COPYDATA) {  // jump list (ShellExperienceHost) asks before unpinning
        auto* cd = (COPYDATASTRUCT*)lp;
        TLOG(L"-> Request from jump list: %lX", cd ? (unsigned long)cd->dwData : 0);
        if (cd && cd->dwData == 0x534B4431) { DumpBars(); return 1; }  // "SKD1"
        if (cd && cd->dwData == 0x534B5032)  // "SKP2": PinManager wants to pin
            return (g_fakeGroup && GetTickCount() - g_jumpTick < 10000 && HandleExtraPin(nullptr)) ? 1 : 0;
        if (cd && cd->dwData == 0x534B4131) {  // "SKA1": re-append after HandleExtraPin?
            const wchar_t* app = cd->cbData >= 2 ? (const wchar_t*)cd->lpData : L"";
            return !g_swallowAddKey.empty() && GetTickCount() - g_swallowAddTick < 3000 && Lower(app) == g_swallowAddKey
                       ? (g_swallowAddKey.clear(), 1) : 0;
        }
        if (cd && cd->dwData == kCopyUnpinOne)
            return HandleUnpinOne(cd->cbData >= 2 ? (const wchar_t*)cd->lpData : L"");
        return 0;
    }
    if (msg == kMsgCleanup) {  // runs on the taskbar thread
        if (g_winEvent) UnhookWinEvent(g_winEvent);
        g_winEvent = nullptr;
        KillTimer(h, kReevalTimerId);
        KillTimer(h, kApplyTimerId);
        KillTimer(h, kSyncTimerId);
        KillTimer(h, kStableTimerId);
        KillTimer(h, kPruneTimerId);
        ReleasePrune();
        ReleasePending();
        SetFakeGroup(nullptr);
        SetJumpGroup(nullptr);
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

static void EnsureWinEvent() {
    if (g_msgWnd) return;
    WNDCLASSW wc{};
    wc.lpfnWndProc = MsgWndProc;
    wc.hInstance = ModInstance();
    wc.lpszClassName = kMsgClass;
    RegisterClassW(&wc);
    g_msgWnd = CreateWindowExW(0, kMsgClass, L"", 0, 0, 0, 0, 0, HWND_MESSAGE,
                               nullptr, wc.hInstance, nullptr);
    if (!g_msgWnd) {
        Wh_Log(L"-> Message window FAILED (%lu)", GetLastError());
        return;
    }
    // The jump list runs in an AppContainer (lower integrity) - allow its request
    ChangeWindowMessageFilterEx(g_msgWnd, WM_COPYDATA, MSGFLT_ALLOW, nullptr);
    g_winEvent = SetWinEventHook(EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE,
                                 nullptr, WinEventProc, 0, 0,
                                 WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    Wh_Log(L"-> Window monitoring %ls", g_winEvent ? L"active" : L"FAILED");
    SetTimer(g_msgWnd, kSyncTimerId, 3000, nullptr);  // sync once startup is done
    SetTimer(g_msgWnd, kStableTimerId, 60000, nullptr);
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
    DWORD_PTR r = 0;
    SendMessageTimeoutW(tray, g_runMsg, (WPARAM)fn, 0x534B, SMTO_BLOCK, 3000, &r);
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
    {
        std::lock_guard<std::mutex> l(g_mx);
        g_bars.insert(self);
    }
    // Mod loaded late / reloaded: start even without new buttons
    if (!g_msgWnd && OnTrayThread()) EnsureWinEvent();
    return GetMonitor_orig(self);
}

// ---------- Menu rules (set via right-click or Ctrl+click, stored in the mod storage)
// Format: "appid|mask;appid|mask;"

static std::map<std::wstring, unsigned> LoadMenuRules() {
    std::map<std::wstring, unsigned> r;
    static wchar_t buf[16384];
    buf[0] = 0;
    Wh_GetStringValue(L"menuRules", buf, ARRAYSIZE(buf));
    std::wstring all = buf, part;
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

static void SaveMenuRules(const std::map<std::wstring, unsigned>& r) {
    std::wstring out;
    wchar_t hex[16];
    for (auto& kv : r) {
        swprintf(hex, 16, L"%X", kv.second);
        out += kv.first + L"|" + hex + L";";
    }
    Wh_SetStringValue(L"menuRules", out.c_str());
}

static void LoadSettings();

// ---------- Apply rules live - no Explorer restart

static void SetRule(const std::wstring& key, unsigned m) {
    auto rules = LoadMenuRules();
    if (m) rules[key] = m; else rules.erase(key);
    SaveMenuRules(rules);
    LoadSettings();
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

struct MsvcVec { void** first; void** last; void** end; };  // std::vector<ComPtr<ITaskGroup>>
using GetGroups_t = MsvcVec* (*)(void* self, MsvcVec* ret);
static GetGroups_t pGetGroups = nullptr;
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

static BOOL CALLBACK FindListWnd(HWND h, LPARAM) {
    wchar_t cls[64];
    if (!GetClassNameW(h, cls, 64) || wcscmp(cls, L"MSTaskListWClass") != 0) return TRUE;
    char* p = (char*)GetWindowLongPtrW(h, 0);
    if (!p || !Readable(p, 8) || *(void**)p != pTlVftWnd) {
        TLOG(L"-> Taskbar search: window %p without a matching vtable (%p)", h, p ? *(void**)p : nullptr);
        return TRUE;
    }
    // take the nearest match - the taskbar objects are located close to each other
    for (int d = 0; d <= 0x400; d += 8) {
        for (int sgn = 0; sgn < 2; sgn++) {
            int off = sgn ? -d : d;
            char* q = p + off;
            if (Readable(q, 8) && *(void**)q == pTlVftUI) {
                std::lock_guard<std::mutex> l(g_mx);
                g_bars.insert(q);
                    return TRUE;
            }
        }
    }
    TLOG(L"-> Taskbar search: window %p - tl not found", h);
    return TRUE;
}

static BOOL CALLBACK FindTrays(HWND h, LPARAM) {
    wchar_t cls[64];
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    if (pid == GetCurrentProcessId() && GetClassNameW(h, cls, 64) &&
        (!wcscmp(cls, L"Shell_TrayWnd") || !wcscmp(cls, L"Shell_SecondaryTrayWnd")))
        EnumChildWindows(h, FindListWnd, 0);
    return TRUE;
}

static void FindBars() {
    if (pTlVftUI && pTlVftWnd) EnumWindows(FindTrays, 0);
}


static void SyncAll() {
    if (!pGetGroups || !pGroupVft || !pGroupItemFromWindow || !GetMonitor_orig) return;
    FindBars();
    std::set<void*> bars;
    {
        std::lock_guard<std::mutex> l(g_mx);
        bars = g_bars;
    }
    // 1. collect all groups (every taskbar returns its own)
    std::set<void*> groups;
    for (void* tl : bars) {
        MsvcVec v{};
        pGetGroups(tl, &v);
        for (void** p = v.first; p && p < v.last; p++) {
            void* g = *p;
            if (!g) continue;
            if (*(void**)g != pGroupVft) {  // unknown object: don't touch
                ((IUnknown*)g)->Release();
                continue;
            }
            if (groups.insert(g).second) continue;  // keep the reference from GetGroups
            ((IUnknown*)g)->Release();              // duplicate: drop the extra reference
        }
        // the vector's memory belongs to the taskbar's CRT - intentionally not freed (small, rare)
    }
    // 2. windows: each window button gets its taskbar
    std::vector<HWND> wins;
    EnumWindows(CollectWnd, (LPARAM)&wins);
    int moved = 0, pinsOff = 0, pinsOn = 0;
    for (void* g : groups) {
        for (HWND h : wins) {
            void* item = GroupItem(g, h);
            if (!item) continue;
            {
                std::lock_guard<std::mutex> l(g_mx);
                g_itemGroup[item] = g;
                g_winMon[h] = MonitorFromWindow(h, MONITOR_DEFAULTTONEAREST);
            }
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
                g_itemBars[item] = allowed;
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

static void InitAnimationHooks();
extern void* Entrance_origPtr();

static void SyncTimerProc() {
    KillTimer(g_msgWnd, kSyncTimerId);
    // Taskbar.View.dll may not have been loaded yet when the mod loaded (Explorer startup): do it now
    if (!Entrance_origPtr()) {
        InitAnimationHooks();
        Wh_ApplyHookOperations();
    }
    SyncAll();
    if (++g_syncRuns < 2) SetTimer(g_msgWnd, kSyncTimerId, 5000, nullptr);  // second pass
}
enum { ID_MON1 = 1, ID_ALL = 20, ID_REMOVE = 21, ID_APPLY = 22 };

static bool ShowPinMenu(void* g) {
    PCWSTR id = pGroupGetAppID ? pGroupGetAppID(g) : nullptr;
    if (!id) return false;
    std::wstring key = Lower(id);
    bool pinned = pGroupGetFlags && (pGroupGetFlags(g) & 0x1);

    std::vector<std::pair<LONG, HMONITOR>> mons;
    EnumDisplayMonitors(nullptr, nullptr, MonEnum, (LPARAM)&mons);
    int count = (int)std::min<size_t>(mons.size(), 8);

    auto rules = LoadMenuRules();
    unsigned orig = rules.count(key) ? rules[key] : AssignedMonitor(g);
    unsigned m = orig;
    POINT pt;
    GetCursorPos(&pt);

    // Menu texts in German for a German UI, English otherwise
    bool de = PRIMARYLANGID(GetUserDefaultUILanguage()) == LANG_GERMAN;
    for (;;) {
        HMENU menu = CreatePopupMenu();
        if (!pinned) {
            AppendMenuW(menu, MF_STRING | MF_GRAYED, 0,
                        de ? L"Erst normal an Taskleiste anheften" : L"Pin to taskbar first");
        } else {
            for (int i = 1; i <= count; i++) {
                wchar_t t[64];
                swprintf(t, 64, de ? L"Pin auf Monitor %d%ls" : L"Pin on monitor %d%ls", i,
                         i == 1 ? (de ? L" (links)" : L" (left)")
                                : (i == count ? (de ? L" (rechts)" : L" (right)") : L""));
                AppendMenuW(menu, MF_STRING | (InMask(m, i) ? MF_CHECKED : 0),
                            ID_MON1 + i - 1, t);
            }
            AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
            AppendMenuW(menu, MF_STRING, ID_ALL, de ? L"Auf allen Monitoren" : L"On all monitors");
            AppendMenuW(menu, MF_STRING, ID_REMOVE,
                        de ? L"Regel entfernen (Windows-Standard)" : L"Remove rule (Windows default)");
            AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
            AppendMenuW(menu, MF_STRING | (m == orig ? MF_GRAYED : 0), ID_APPLY,
                        m == orig ? (de ? L"Keine Aenderung" : L"No change")
                                  : (de ? L"Fertig - anwenden" : L"Done - apply"));
        }
        HWND owner = CreateWindowExW(WS_EX_TOOLWINDOW, L"STATIC", L"", WS_POPUP, 0, 0,
                                     0, 0, nullptr, nullptr, nullptr, nullptr);
        SetForegroundWindow(owner);
        int cmd = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_BOTTOMALIGN,
                                 pt.x, pt.y, 0, owner, nullptr);
        DestroyMenu(menu);
        DestroyWindow(owner);
        if (!pinned) return true;

        if (cmd >= ID_MON1 && cmd < ID_MON1 + 8) { m ^= 1u << (cmd - ID_MON1); continue; }
        if (cmd == ID_ALL) { m = (1u << count) - 1; continue; }
        if (cmd == ID_REMOVE) { m = 0; continue; }
        break;  // done or menu closed
    }

    if (m == orig) return true;
    SetRule(key, m);
    Wh_Log(L"Menu: %ls -> mask 0x%X", key.c_str(), m);
    ApplyPinRule(g);
    return true;
}

using HandleClick_t = HRESULT (*)(void*, void*, void*, void*);
static HandleClick_t HandleClick_orig;
static HRESULT HandleClick_hook(void* self, void* g, void* item, void* opts) {
    bool ctrl = g_s.ctrlClickMenu && (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
    if (g && ctrl && ShowPinMenu(g)) return S_OK;
    return HandleClick_orig(self, g, item, opts);
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
    if (mon < 1 || mon > 8) return L"";
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

static bool FilesEqual(const std::wstring& a, const std::wstring& b) {
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
using Launch_t = HRESULT (*)(void*, void*, const POINT*, int);
static Launch_t Launch_orig;
static HRESULT Launch_hook(void* self, void* g, const POINT* pt, int opt) {
    // App is running somewhere (also on another monitor or in the tray): bring the window instead of relaunching
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
        bool anyOwn = false;
        int cnt = std::min(GetSystemMetrics(SM_CMONITORS), 8);
        for (int m = 1; m <= cnt && !orig.empty(); m++) {
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
            TLOG(L"-> Launched own shortcut of monitor %d: %ls", mon, copy.c_str());
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
    if (g_s.pinsOnAll) {
        int c = GetSystemMetrics(SM_CMONITORS);
        return c >= 8 ? 0xFF : (1u << c) - 1;
    }
    return 1u << (PrimaryNumber() - 1);
}

// Real user actions only: not right after startup, not during our own flag toggle,
// only if mouse/keyboard was just used. Multiple calls (one per taskbar) count once.
static std::wstring g_lastEvt;
static DWORD g_lastEvtTick = 0;
static bool TakePinEvent(void* g, bool pin, std::wstring& key) {
    if (g_selfFlag || !g || !pGroupGetAppID) return false;
    DWORD now = GetTickCount();
    LASTINPUTINFO li{sizeof(li)};
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
DWORD g_jumpTick = 0;
static std::wstring g_jumpKey;
void* g_fakeGroup = nullptr;  // jump list showed "Pin" although pinned globally (AddRef)

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
    if (mon >= 1 && mon <= 8) {
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
std::wstring g_swallowAddKey;
DWORD g_swallowAddTick = 0;

static bool HandleExtraPin(const wchar_t* app) {
    void* g = g_fakeGroup;
    if (!g || GetTickCount() - g_jumpTick > 20000 || !Pinned(g)) return false;
    if (app && *app && _wcsicmp(app, AppOf(g)) != 0) return false;
    int mon = MonNumber(g_jumpMon);
    if (mon < 1 || mon > 8) return false;
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
    if (HandleExtraPin(app)) return 1;
    void* g = g_jumpGroup;
    TLOG(L"-> Unpin request app=%ls jump=%.40ls age=%lu ms pinned=%d mask=0x%X mon=%d", app ? app : L"?",
         g ? AppOf(g) : L"-", GetTickCount() - g_jumpTick, g ? Pinned(g) : 0, g ? MaskOf(g) : 0, MonNumber(g_jumpMon));
    if (!g || GetTickCount() - g_jumpTick > 20000 || !Pinned(g)) return 0;
    int mon = MonNumber(g_jumpMon);
    unsigned mask = MaskOf(g);
    if (mon < 1 || mon > 8 || !InMask(mask, mon)) return 0;
    unsigned rest = mask & ~(1u << (mon - 1));
    if (!rest) return 0;  // last taskbar: let it unpin normally
    if (app && *app && _wcsicmp(app, AppOf(g)) != 0)
        TLOG(L"-> Note: jump list app %ls != %ls", app, AppOf(g));
    SetRule(KeyOf(g), rest);
    Wh_Log(L"-> Unpinned only from monitor %d, stays on mask 0x%X: %ls", mon, rest, AppOf(g));
    QueueApply(g);
    return 1;
}
// Jump list: show as "not pinned" on taskbars where the pin isn't allowed
using ShowJump_t = HRESULT (*)(void*, void*, void*, bool);
static ShowJump_t ShowJump_orig;
static HRESULT ShowJump_hook(void* self, void* g, void* item, bool b) {
    void* tl = BarOf(self);
    bool fake = false;
    if (tl && g && Pinned(g) && !ShouldShow(tl, g, nullptr) && pGroupUpdateFlags) {
        g_selfFlag = true;
        pGroupUpdateFlags(g, 0x1, 0);
        if (Pinned(g)) pGroupUpdateFlags(g, 0, 0x1);
        fake = !Pinned(g);
    }
    // this taskbar's jump list points to the taskbar's own shortcut (for "Properties")
    bool swapped = false;
    PIDLIST_ABSOLUTE saved = nullptr;
    if (tl && g && Pinned(g) && pGroupSetShortcut && pGroupGetShortcutIDList) {
        std::wstring copy = BarLinkPath(g, MonNumber(GetMonitor_orig(tl)), true);
        PCIDLIST_ABSOLUTE cur = pGroupGetShortcutIDList(g);
        PIDLIST_ABSOLUTE sw = copy.empty() ? nullptr : ILCreateFromPathW(copy.c_str());
        if (cur && sw) {
            saved = ILCloneFull(cur);  // intentionally not freed (unknown whether Windows takes ownership)
            swapped = SUCCEEDED(pGroupSetShortcut(g, sw));
        }
    }
    HRESULT hr = ShowJump_orig(self, g, item, b);
    if (swapped && saved) pGroupSetShortcut(g, saved);
    if (fake) pGroupUpdateFlags(g, 0x1, 0x1);
    g_selfFlag = false;
    if (tl) {
        g_jumpMon = GetMonitor_orig(tl);
        g_jumpTick = GetTickCount();
        g_jumpKey = KeyOf(g);
        SetFakeGroup(fake ? g : nullptr);
        SetJumpGroup(g);
    }
    TLOG(L"-> Jump list on monitor %d%ls: %.40ls", tl ? MonNumber(GetMonitor_orig(tl)) : 0,
         fake ? L" (as not pinned)" : L"", AppOf(g));
    return hr;
}

// The pin list was reread: was this the click on "Pin" in a trick jump list?
using EnumPinned_t = HRESULT (*)(void*, bool, bool, bool);
static EnumPinned_t EnumPinned_orig;
static HRESULT EnumPinned_hook(void* self, bool a, bool b, bool c) {
    HRESULT hr = EnumPinned_orig(self, a, b, c);
    void* g = g_fakeGroup;
    LASTINPUTINFO li{sizeof(li)};
    DWORD now = GetTickCount();
    if (g && now - g_jumpTick < 20000 && GetLastInputInfo(&li) && now - li.dwTime < 8000 && Pinned(g)) {
        int mon = MonNumber(g_jumpMon);
        if (mon >= 1 && mon <= 8) {
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

static void LoadSettings() {
    g_s.foregroundFix = Wh_GetIntSetting(L"foregroundFix") != 0;
    g_s.slideAnimation = Wh_GetIntSetting(L"slideAnimation") != 0;
    g_s.ctrlClickMenu = Wh_GetIntSetting(L"ctrlClickMenu") != 0;
    g_s.autoConfigure = Wh_GetIntSetting(L"autoConfigure") != 0;
    g_s.traceLog = Wh_GetIntSetting(L"traceLog") != 0;
    PCWSTR up = Wh_GetStringSetting(L"unassignedPins");
    g_s.pinsOnAll = up && _wcsicmp(up, L"all") == 0;
    if (up) Wh_FreeStringSetting(up);
    PCWSTR pm = Wh_GetStringSetting(L"pinMode");
    g_s.pinsEverywhere = pm && _wcsicmp(pm, L"everywhere") == 0;
    if (pm) Wh_FreeStringSetting(pm);
    PCWSTR wm = Wh_GetStringSetting(L"windowMode");
    g_s.windowMode = !wm ? 0 : !_wcsicmp(wm, L"all") ? 2 : !_wcsicmp(wm, L"primaryAll") ? 1 : 0;
    if (wm) Wh_FreeStringSetting(wm);
    PCWSTR fs = Wh_GetStringSetting(L"focusScope");
    g_s.focusThisMonitor = fs && _wcsicmp(fs, L"thisMonitor") == 0;
    if (fs) Wh_FreeStringSetting(fs);
    std::vector<std::wstring> always;
    for (int i = 0; i < 100; i++) {
        PCWSTR a = Wh_GetStringSetting(L"alwaysOnAllTaskbars[%d]", i);
        std::wstring s = a ? a : L"";
        if (a) Wh_FreeStringSetting(a);
        if (s.empty()) break;
        s = Lower(FileName(s));  // accepts "discord.exe" or a full path
        if (s.find(L'.') == std::wstring::npos) s += L".exe";
        always.push_back(s);
    }
    {
        std::lock_guard<std::mutex> l(g_exemptMx);
        g_exemptCache.clear();
    }

    // Pin assignments (right-click / Ctrl+click), exact app ID
    std::vector<PinRule> rules;
    for (auto& kv : LoadMenuRules()) rules.push_back({kv.first, kv.second});
    std::lock_guard<std::mutex> l(g_rulesMx);
    g_s.rules = rules;
    g_s.alwaysAll = always;
}
// ---------- Check / set / restore the taskbar settings
// The mod needs: taskbar on all displays (MMTaskbarEnabled=1) and apps on all
// taskbars (MMTaskbarMode=0) - Windows then creates everything everywhere and the mod filters.

static const wchar_t kAdvKey[] = L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced";

static DWORD RegGetDw(const wchar_t* name, DWORD def) {
    DWORD v = def, cb = sizeof(v);
    if (RegGetValueW(HKEY_CURRENT_USER, kAdvKey, name, RRF_RT_REG_DWORD, nullptr, &v, &cb) != ERROR_SUCCESS)
        return def;
    return v;
}
static void RegSetDw(const wchar_t* name, DWORD v) {
    RegSetKeyValueW(HKEY_CURRENT_USER, kAdvKey, name, REG_DWORD, &v, sizeof(v));
}
static void BroadcastTraySettings() {
    SendNotifyMessageW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"TraySettings");
}

// Original values are also kept in memory: on "Remove", Windhawk deletes the mod storage
// BEFORE the mod is unloaded - Wh_ModUninit couldn't read them otherwise.
static bool g_cfgSaved = false;
static DWORD g_cfgOrigEn = 1, g_cfgOrigMode = 0;
static void LoadCfgCache() {
    g_cfgSaved = Wh_GetIntValue(L"cfgSaved", 0) != 0;
    g_cfgOrigEn = (DWORD)Wh_GetIntValue(L"cfgOrigEnabled", 1);
    g_cfgOrigMode = (DWORD)Wh_GetIntValue(L"cfgOrigMode", 0);
}

static void CheckTaskbarConfig() {
    DWORD en = RegGetDw(L"MMTaskbarEnabled", 0), mode = RegGetDw(L"MMTaskbarMode", 0);
    if (en == 1 && mode == 0) {
        Wh_Log(L"-> Taskbar settings are fine");
        return;
    }
    if (!g_s.autoConfigure) {
        Wh_Log(L"-> NOTE: please set: Settings > Personalization > Taskbar > Taskbar behaviors > "
               L"'Show my taskbar on all displays' = on, 'When using multiple displays, show my taskbar "
               L"apps on' = All taskbars (current: on=%lu, mode=%lu)", en, mode);
        return;
    }
    if (!g_cfgSaved) {  // remember the original values only the first time
        Wh_SetIntValue(L"cfgOrigEnabled", (int)en);
        Wh_SetIntValue(L"cfgOrigMode", (int)mode);
        Wh_SetIntValue(L"cfgSaved", 1);
        g_cfgSaved = true;
        g_cfgOrigEn = en;
        g_cfgOrigMode = mode;
    }
    RegSetDw(L"MMTaskbarEnabled", 1);
    RegSetDw(L"MMTaskbarMode", 0);
    BroadcastTraySettings();
    Wh_Log(L"-> Taskbar settings applied (before: on=%lu, mode=%lu - restored when the mod is removed)",
           en, mode);
}

// When the mod is disabled/removed: restore the previous values (only if the mod changed them)
static void RestoreTaskbarConfig(bool removed) {
    if (!g_cfgSaved) return;
    RegSetDw(L"MMTaskbarEnabled", g_cfgOrigEn);
    RegSetDw(L"MMTaskbarMode", g_cfgOrigMode);
    if (!removed) Wh_SetIntValue(L"cfgSaved", 0);
    g_cfgSaved = false;
    BroadcastTraySettings();
    Wh_Log(L"-> Taskbar settings restored (on=%lu, mode=%lu)", g_cfgOrigEn, g_cfgOrigMode);
}

// Mod removed? On startup a marker is written to the mod storage. If it's missing on unload,
// Windhawk deleted the storage = the mod was removed (not just disabled/updated).
static bool ModWasRemoved() {
    return Wh_GetIntValue(L"installed", 0) == 0;
}
static void DeleteTree(const std::wstring& dir) {
    WIN32_FIND_DATAW fd;
    HANDLE f = FindFirstFileW((dir + L"\\*").c_str(), &fd);
    if (f != INVALID_HANDLE_VALUE) {
        do {
            if (!wcscmp(fd.cFileName, L".") || !wcscmp(fd.cFileName, L"..")) continue;
            std::wstring p = dir + L"\\" + fd.cFileName;
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) DeleteTree(p);
            else DeleteFileW(p.c_str());
        } while (FindNextFileW(f, &fd));
        FindClose(f);
    }
    RemoveDirectoryW(dir.c_str());
}
static void RemoveBarLinkCopies() {
    if (g_linkRoot.empty()) return;
    DeleteTree(g_linkRoot);  // Windhawk normally removes the storage folder itself - just in case
    Wh_Log(L"-> Shortcut copies deleted");
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
        TLOG(L"-> Pin list: unpin %ls (caller %d): %ls", app ? app : L"?", caller,
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
        TLOG(L"-> Pin list: append %ls (caller %d)%ls", app ? app : L"?", caller, swallow ? L" - intercepted" : L"");
        if (app) CoTaskMemFree(app);
        if (swallow) return S_OK;
    }
    return PinModify_orig(self, from, to, caller);
}

// ---------- Animation (Taskbar.View.dll)
using Entrance_t = void (*)(void*, int, double, float*, long long, bool);
static Entrance_t Entrance_orig;
// Window moved to another taskbar: instead of "appearing from below", slide in
// sideways - from the side the window came from
int g_moveDir = 0;        // -1 = came from the right, +1 = came from the left
DWORD g_swapTick = 0;      // a pin <-> window button swap on the same taskbar follows
DWORD g_swapHideTick = 0;  // plus: old button without exit animation
DWORD g_moveTick = 0;
static void Entrance_hook(void* self, int kind, double d, float* off, long long dur, bool b) {
    if (g_s.slideAnimation && g_moveDir && GetTickCount() - g_moveTick < 400 && off) {
        kind = 1;  // kind 1 = slide by the offset (kind 0 = from below, kind 2 = zoom)
        off[0] = -48.0f * g_moveDir;
        off[1] = 0.0f;
        d = 0.0;  // no part from below - purely sideways
        g_moveDir = 0;
        TLOG(L"-> Slide in sideways (x=%.0f)", off[0]);
    } else if (g_s.slideAnimation && g_swapTick && GetTickCount() - g_swapTick < 350) {
        // Pin turns into a window button (or vice versa) on the same taskbar: the icon stays
        // still instead of appearing from below (otherwise two icons overlap briefly)
        d = 0.0;
        g_swapTick = 0;
        TLOG(L"-> Pin/window swap without entrance animation");
    }
    Entrance_orig(self, kind, d, off, dur, b);
}

void* Entrance_origPtr() { return (void*)Entrance_orig; }

using ExitAnim_t = void (*)(void*);
static ExitAnim_t ExitAnim_orig;
static void ExitAnim_hook(void* self) {
    // Window gone, the app's pin stays on this taskbar: don't slide down
    // (otherwise two icons overlap briefly)
    if (g_s.slideAnimation && g_swapHideTick && GetTickCount() - g_swapHideTick < 400) {
        g_swapHideTick = 0;
        TLOG(L"-> Window/pin swap: without exit animation");
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
        HMODULE tv = GetModuleHandleW(L"Taskbar.View.dll");
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

static void InitAnimationHooks() {
    HMODULE tv = GetModuleHandleW(L"Taskbar.View.dll");
    if (!tv || Entrance_orig || g_safeMode) return;
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {{L"public: void __cdecl winrt::Taskbar::implementation::TaskListButton::PlayEntranceAnimation(enum winrt::Taskbar::implementation::TaskListButtonEntranceAnimationKind,double,struct winrt::Windows::Foundation::Numerics::float3,class std::chrono::duration<__int64,struct std::ratio<1,10000000> >,bool)"},
         (void**)&Entrance_orig, (void*)Entrance_hook, true},
        {{L"public: void __cdecl winrt::Taskbar::implementation::TaskListButton::StartExitAnimation(void)"},
         (void**)&ExitAnim_orig, (void*)ExitAnim_hook, true},
    };
    WindhawkUtils::HookSymbols(tv, hooks, ARRAYSIZE(hooks));
    Wh_Log(L"-> Animation %ls", (Entrance_orig && ExitAnim_orig) ? L"active" : L"partial");
    WindhawkUtils::SYMBOL_HOOK labelFix[] = {
        {{L"public: void __cdecl winrt::Taskbar::implementation::TaskListButton::HasLabel(bool)"},
         (void**)&BtnHasLabel_orig, (void*)BtnHasLabel_hook, true},
        {{L"public: void __cdecl winrt::Taskbar::implementation::TaskListButton::Prepared(bool)"},
         (void**)&BtnPrepared_orig, (void*)BtnPrepared_hook, true},
    };
    WindhawkUtils::HookSymbols(tv, labelFix, ARRAYSIZE(labelFix));
    Wh_Log(L"-> Label fix %ls", (BtnHasLabel_orig && BtnPrepared_orig) ? L"active" : L"partial");
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
        COPYDATASTRUCT cd{0x534B5032, sizeof(wchar_t), (PVOID)L""};  // "SKP2"
        extra = SendMessageTimeoutW(w, WM_COPYDATA, 0, (LPARAM)&cd, SMTO_ABORTIFHUNG, 1500, &r) && r == 1;
    }
    TLOG(L"-> PinManager: pin (caller %d)%ls", caller, extra ? L" - additional taskbar only" : L"");
    if (extra) return S_OK;
    return PinTrusted_orig(self, pidl, caller);
}
static bool InitPinListHooks() {
    HMODULE tw = GetModuleHandleW(L"twinui.pcshell.dll");
    if (!tw) return false;
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {{L"public: virtual long __cdecl CPinnedList::GetAppIDForPinnedItem(struct _ITEMIDLIST const __unaligned *,unsigned short * *)"},
         (void**)&pGetAppIDForPinned, nullptr, true},
        {{L"public: virtual long __cdecl CPinnedList::Modify(struct _ITEMIDLIST const __unaligned *,struct _ITEMIDLIST const __unaligned *,enum PINNEDLISTMODIFYCALLER)"},
         (void**)&PinModify_orig, (void*)PinModify_hook, true},
        {{L"public: virtual long __cdecl winrt::Windows::Internal::Shell::implementation::PinManager::PinItemFromTrustedCaller(struct _ITEMIDLIST const __unaligned *,enum PINNEDLISTMODIFYCALLER)"},
         (void**)&PinTrusted_orig, (void*)PinTrusted_hook, true},
    };
    WindhawkUtils::HookSymbols(tw, hooks, ARRAYSIZE(hooks));
    Wh_Log(L"-> Pin list (%ls) %ls, PinManager %ls", FileName(GetCommandLineW()).c_str(),
           PinModify_orig ? L"monitored" : L"NOT found", PinTrusted_orig ? L"yes" : L"no");
    return PinModify_orig != nullptr;
}
BOOL Wh_ModInit() {
    wchar_t exe[MAX_PATH];
    GetModuleFileNameW(nullptr, exe, MAX_PATH);
    if (_wcsicmp(FileName(exe).c_str(), L"explorer.exe") != 0) {  // sihost: pin list only
        LoadSettings();
        return InitPinListHooks();
    }
    g_isExplorer = true;
    LoadSettings();
    Wh_SetIntValue(L"installed", 1);  // marker for "mod removed" (see ModWasRemoved)
    LoadCfgCache();
    InitLinkRoot();
    // Crash loop protection: if Explorer restarts several times in a row before the mod
    // ran stable for 60 s, risky parts (group cleanup, animation) stay off until the
    // next mod version.
    {
        // only count if Explorer has just started (not on mod updates)
        FILETIME c, x, k, u, now;
        GetProcessTimes(GetCurrentProcess(), &c, &x, &k, &u);
        GetSystemTimeAsFileTime(&now);
        ULONGLONG age = (((ULONGLONG)now.dwHighDateTime << 32 | now.dwLowDateTime) -
                         ((ULONGLONG)c.dwHighDateTime << 32 | c.dwLowDateTime)) / 10000000ULL;
        int boots = Wh_GetIntValue(L"bootCount", 0);
        if (age < 30) Wh_SetIntValue(L"bootCount", ++boots);
        wchar_t sv[32] = {};
        Wh_GetStringValue(L"safeVersion", sv, 32);
        if (boots >= 4) {
            Wh_SetStringValue(L"safeVersion", WH_MOD_VERSION);
            wcscpy(sv, WH_MOD_VERSION);
        }
        g_safeMode = wcscmp(sv, WH_MOD_VERSION) == 0;
        if (g_safeMode)
            Wh_Log(L"-> SAFE MODE: Explorer restarted repeatedly - group cleanup and "
                   L"animation are off until the next update");
    }
    HMODULE tb = GetModuleHandleW(L"taskbar.dll");
    if (!tb) tb = LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!tb) {
        Wh_Log(L"taskbar.dll not found");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {{L"public: virtual unsigned short const * __cdecl CTaskGroup::GetAppID(void)"},
         (void**)&pGroupGetAppID},
        {{L"public: virtual int __cdecl CTaskGroup::GetNumItems(void)"},
         (void**)&pGroupGetNumItems},
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
        {{L"protected: void __cdecl CTaskBand::_HandleMonitorChanged(struct HWND__ *)"},
         (void**)&MonChanged_orig, (void*)MonChanged_hook, true},
        {{L"public: virtual bool __cdecl CTaskListWnd::IsTaskAllowed(struct ITaskItem *)"},
         (void**)&IsAllowed_orig, (void*)IsAllowed_hook},
        {{L"public: virtual long __cdecl CTaskListWnd::HandleClick(struct ITaskGroup *,struct ITaskItem *,struct winrt::Windows::System::LauncherOptions const &)"},
         (void**)&HandleClick_orig, (void*)HandleClick_hook},
        {{L"public: virtual long __cdecl CTaskBand::Launch(struct ITaskGroup *,struct tagPOINT const &,enum LaunchFromTaskbarOptions)"},
         (void**)&Launch_orig, (void*)Launch_hook},
    };

    if (!WindhawkUtils::HookSymbols(tb, hooks, ARRAYSIZE(hooks))) {
        Wh_Log(L"HookSymbols failed");
        return FALSE;
    }

    // Optional, lookup only - nothing is called
    WindhawkUtils::SYMBOL_HOOK probe[] = {
        {{L"public: virtual struct HWND__ * __cdecl CWindowTaskItem::GetWindow(void)"},
         (void**)&pItemGetWindow, nullptr, true},
        {{L"const CWindowTaskItem::`vftable'{for `ITaskItem'}",
          L"const CWindowTaskItem::`vftable'"},
         (void**)&pWindowItemVft, nullptr, true},
    };
    WindhawkUtils::HookSymbols(tb, probe, ARRAYSIZE(probe));

    g_tb = tb;
    WindhawkUtils::SYMBOL_HOOK upd[] = {
        {{L"public: virtual long __cdecl CTaskGroup::UpdateFlags(unsigned long,unsigned long)"},
         (void**)&pGroupUpdateFlags, nullptr, true},
    };
    WindhawkUtils::HookSymbols(tb, upd, ARRAYSIZE(upd));

    WindhawkUtils::SYMBOL_HOOK jump[] = {
        {{L"public: virtual long __cdecl CTaskGroup::SetShortcutIDList(struct _ITEMIDLIST_ABSOLUTE const *)"},
         (void**)&pGroupSetShortcut, nullptr, true},
        {{L"public: virtual long __cdecl CTaskListWnd::ShowJumpView(struct ITaskGroup *,struct ITaskItem *,bool)"},
         (void**)&ShowJump_orig, (void*)ShowJump_hook, true},
        {{L"protected: long __cdecl CTaskBand::_EnumPinnedItems(bool,bool,bool)"},
         (void**)&EnumPinned_orig, (void*)EnumPinned_hook, true},
    };
    WindhawkUtils::HookSymbols(tb, jump, ARRAYSIZE(jump));
    Wh_Log(L"-> Jump list pins %ls", (pGroupUpdateFlags && ShowJump_orig && EnumPinned_orig)
                                        ? L"active" : L"INCOMPLETE");

    WindhawkUtils::SYMBOL_HOOK tbg[] = {
        {{L"protected: struct ITaskBtnGroup * __cdecl CTaskListWnd::_GetTBGroupFromGroup(struct ITaskGroup *,int *)"},
         (void**)&pGetTBGroup, nullptr, true},
    };
    WindhawkUtils::HookSymbols(tb, tbg, ARRAYSIZE(tbg));

    WindhawkUtils::SYMBOL_HOOK sync[] = {
        {{L"public: virtual class std::vector<class Microsoft::WRL::ComPtr<struct ITaskGroup>,class std::allocator<class Microsoft::WRL::ComPtr<struct ITaskGroup> > > __cdecl CTaskListWnd::GetGroups(void)"},
         (void**)&pGetGroups, nullptr, true},
        {{L"const CTaskGroup::`vftable'{for `ITaskGroup'}"}, (void**)&pGroupVft, nullptr, true},
        {{L"const CTaskListWnd::`vftable'{for `ITaskListUI'}"}, (void**)&pTlVftUI, nullptr, true},
        {{L"const CTaskListWnd::`vftable'{for `CImpWndProc'}"}, (void**)&pTlVftWnd, nullptr, true},
        {{L"public: virtual long __cdecl CTaskGroup::GetItemFromWindow(struct HWND__ *,struct ITaskItem * *)"},
         (void**)&pGroupItemFromWindow, nullptr, true},
        {{L"public: virtual int __cdecl CTaskBtnGroup::GetNumItems(void)"}, (void**)&pBtnGetNumItems, nullptr, true},
        {{L"public: virtual struct ITaskItem * __cdecl CTaskBtnGroup::GetTaskItem(int)"}, (void**)&pBtnGetTaskItem, nullptr, true},
        {{L"public: virtual int __cdecl CTaskListWnd::GetButtonGroupCount(void)"}, (void**)&pGetButtonGroupCount, nullptr, true},
        {{L"public: virtual enum eTBGROUPTYPE __cdecl CTaskBtnGroup::GetGroupType(void)"}, (void**)&pBtnGetType, nullptr, true},
    };
    WindhawkUtils::HookSymbols(tb, sync, ARRAYSIZE(sync));
    InitDpaOffset();
    Wh_Log(L"-> Startup sync %ls", (pGetGroups && pGroupVft && pGroupItemFromWindow && pBtnGetNumItems && pBtnGetTaskItem) ? L"active" : L"MISSING");
    if (pGetTBGroup) {
        WindhawkUtils::SYMBOL_HOOK order[] = {
            {{L"public: virtual void __cdecl CTaskListWnd::SetRelativeTaskOrder(struct ITaskGroup *,int)"},
             (void**)&SetOrder_orig, (void*)SetOrder_hook, true},
        };
        WindhawkUtils::HookSymbols(tb, order, ARRAYSIZE(order));
    }
    WindhawkUtils::SYMBOL_HOOK pinEvt[] = {
        {{L"public: virtual void __cdecl CTaskListWnd::HandleTaskGroupPinned(struct ITaskGroup *)"},
         (void**)&PinnedEvt_orig, (void*)PinnedEvt_hook, true},
        {{L"public: virtual void __cdecl CTaskListWnd::HandleTaskGroupUnpinned(struct ITaskGroup *)"},
         (void**)&UnpinnedEvt_orig, (void*)UnpinnedEvt_hook, true},
    };
    WindhawkUtils::HookSymbols(tb, pinEvt, ARRAYSIZE(pinEvt));
    Wh_Log(L"-> Pin events %ls", (PinnedEvt_orig && UnpinnedEvt_orig) ? L"active" : L"MISSING");
    Wh_Log(L"-> Order filter %ls", (pGetTBGroup && SetOrder_orig) ? L"active" : L"MISSING");


    if (false) {
        Wh_Log(L"HookSymbols failed");
        return FALSE;
    }
    InitPinListHooks();
    InitAnimationHooks();
    CheckTaskbarConfig();
    g_initTick = GetTickCount();
    // Taskbar is already up (mod update / loaded late): start right away, sync follows
    if (RunOnTrayThread(EnsureWinEvent)) Wh_Log(L"-> Taskbar already running - sync scheduled");
    Wh_Log(L"v%ls active (foregroundFix=%d traceLog=%d)", WH_MOD_VERSION, g_s.foregroundFix,
           g_s.traceLog);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    Wh_Log(L"Settings reloaded");
    if (!g_isExplorer) return;
    if (g_s.autoConfigure) CheckTaskbarConfig();
    else RestoreTaskbarConfig(false);  // automatic configuration turned off: restore previous values
    RunOnTrayThread(SyncAll);  // e.g. apply "Unassigned pins" right away
}

void Wh_ModUninit() {
    if (g_msgWnd) {
        DWORD_PTR r = 0;
        if (!SendMessageTimeoutW(g_msgWnd, kMsgCleanup, 0, 0, SMTO_BLOCK, 3000, &r))
            Wh_Log(L"-> Cleanup on the taskbar thread FAILED");
    }
    UnregisterClassW(kMsgClass, ModInstance());
    if (g_isExplorer) {
        bool removed = ModWasRemoved();
        RestoreTaskbarConfig(removed);  // mod disabled/removed: restore previous values
        if (removed) RemoveBarLinkCopies();
        Wh_Log(L"Uninit%ls", removed ? L" (mod removed)" : L"");
        return;
    }
    Wh_Log(L"Uninit");
}
