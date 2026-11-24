// ==WindhawkMod==
// @id              taskbar-edge-stop
// @name            Taskbar Edge Stop
// @description     On multi-monitor setups, the mouse stops at the taskbar instead of slipping onto the other monitor
// @version         1.0
// @author          voide61
// @github          https://github.com/voide61
// @include         windhawk.exe
// @compilerOptions -luser32 -lole32 -lshcore
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Edge Stop

**The problem:** with two monitors, you move the mouse to the taskbar and it slips
right past it onto the other monitor. With a small or auto-hidden taskbar this
happens all the time.

**The fix:** the taskbar becomes a "stopper". The mouse stops on it, so you can
click your icons without overshooting.

![Demo](https://raw.githubusercontent.com/voide61/taskbar-edge-stop/main/demo.gif)

## How it works

- Moving to the taskbar, the mouse **stops on it** (and an auto-hidden taskbar shows up).
- Want to go to the other monitor? **Keep pushing** the mouse a little more.
- Over the taskbar **icons**, you can make it **never cross**, no matter how hard
  you push (option below). Cross through the empty part of the taskbar instead.
- Coming from the other monitor straight onto the taskbar, the way is **free**.
- Moving **sideways** into the taskbar strip of a neighboring monitor (monitors side
  by side) is also stopped, so the mouse doesn't slide onto the wrong taskbar.
- Nothing changes anywhere else on the screen.

## Works with

- Monitors stacked vertically or side by side, any size (ultrawide included).
- Small taskbars (other Windhawk mods), auto-hide, and taskbars on any screen edge.
- Plugging in, unplugging or rearranging monitors, and Explorer restarts: the mod
  adapts within a second.

## Limitations

- Injected mouse input is ignored, so the mod does nothing in Remote Desktop
  sessions, in virtual machines with guest tools, and with some tablets or
  touchpads that report their movement as injected input.
- Other mods that also act on the mouse crossing between monitors with a low-level
  mouse hook (for example *Proportional cursor between monitors*) may conflict:
  whichever hook was installed last decides.

## Settings

- **Extra height above the taskbar:** increase it if the mouse still slips through.
  It follows each monitor's scale, so it looks the same on a 100% and a 200% monitor.
- **Push force to cross:** how much to push against the taskbar to cross (0 = never).
- **Never cross over the icons:** over the app icons, Start and the system tray icons,
  the mouse never crosses, even when pushing.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- margin: 40
  $name: Extra height above the taskbar (px)
  $description: Strip above the taskbar that also stops the mouse, in pixels at 100% scale (scaled with each monitor's DPI). Increase it if the mouse still slips through.
- force: 400
  $name: Push force to cross to the other monitor (px)
  $description: How much to keep pushing against the taskbar to cross over. 0 = never crosses through the taskbar.
- blockIcons: true
  $name: Never cross over the icons
  $description: Over the taskbar icons (Start, apps, system tray) the mouse never crosses, even when pushing. The push force still works on the empty parts of the taskbar.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellscalingapi.h>
#include <uiautomationclient.h>

#include <algorithm>
#include <atomic>
#include <climits>
#include <cstdlib>
#include <utility>
#include <vector>

using Span = std::pair<int, int>;  // [start, end) along the taskbar

struct Mon {
    RECT rc;
    RECT zone;
    bool hasZone;
    bool flat;               // horizontal taskbar (top/bottom)
    int dpi;                 // monitor DPI (96 = 100% scale)
    std::vector<Span> icons; // taskbar ranges that have icons
};

struct BarIcons {
    HWND bar;
    std::vector<Span> spans;
};

static std::atomic<int> g_margin{40};
static std::atomic<int> g_force{400};
static std::atomic<bool> g_blockIcons{true};
static std::atomic<bool> g_stop{false};

static HANDLE g_hookThread = nullptr;
static DWORD g_hookThreadId = 0;
static HANDLE g_queueReadyEvent = nullptr;
static HANDLE g_iconThread = nullptr;
static HANDLE g_stopEvent = nullptr;
static HANDLE g_wakeEvent = nullptr; // hook thread -> icon thread: "the cursor is near a taskbar"

// icon ranges found by the UI Automation thread
static SRWLOCK g_iconLock = SRWLOCK_INIT;
static std::vector<BarIcons> g_barIcons;

// used only by the hook thread
static std::vector<Mon> g_mons;
static HHOOK g_hook = nullptr;
static int g_accum = 0;
static DWORD g_lastBlock = 0;
static DWORD g_lastWake = 0;

static bool InRect(const RECT& r, int x, int y) {
    return x >= r.left && x < r.right && y >= r.top && y < r.bottom;
}

static int Clamp(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }

static int IndexOf(const std::vector<Mon>& mons, int x, int y) {
    for (size_t i = 0; i < mons.size(); i++)
        if (InRect(mons[i].rc, x, y)) return (int)i;
    return -1;
}

static int Nearest(const std::vector<Mon>& mons, int x, int y) {
    int idx = -1;
    long long best = LLONG_MAX;
    for (size_t i = 0; i < mons.size(); i++) {
        const RECT& m = mons[i].rc;
        long long dx = x < m.left ? m.left - x : (x >= m.right ? x - m.right + 1 : 0);
        long long dy = y < m.top ? m.top - y : (y >= m.bottom ? y - m.bottom + 1 : 0);
        long long d = dx * dx + dy * dy;
        if (d < best) { best = d; idx = (int)i; }
    }
    return idx;
}

static std::vector<HWND> TaskbarWindows() {
    std::vector<HWND> v;
    const wchar_t* classes[] = {L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd"};
    for (const wchar_t* cls : classes) {
        HWND h = nullptr;
        while ((h = FindWindowExW(nullptr, h, cls, nullptr)) != nullptr) v.push_back(h);
    }
    return v;
}

// ---------------------------------------------------------------- icons (UI Automation)
// Runs on its own thread, so a slow taskbar never makes the mouse hook lag.

struct SpanCollector {
    RECT bar;
    bool flat;
    std::vector<Span> spans;
    void Add(const RECT& r) {
        if (r.right <= r.left || r.bottom <= r.top) return;
        int a = flat ? r.left : r.top, b = flat ? r.right : r.bottom;
        int lo = flat ? bar.left : bar.top, hi = flat ? bar.right : bar.bottom;
        // skip anything spanning the whole taskbar (containers)
        if (b - a < (hi - lo) * 3 / 4) spans.push_back({a, b});
    }
};

// Classic (Windows 10 style) taskbar: icons are child windows.
static BOOL CALLBACK ChildProc(HWND h, LPARAM lp) {
    auto* sc = reinterpret_cast<SpanCollector*>(lp);
    wchar_t cls[64];
    if (!IsWindowVisible(h) || !GetClassNameW(h, cls, 64)) return TRUE;
    if (!lstrcmpW(cls, L"MSTaskListWClass") || !lstrcmpW(cls, L"MSTaskSwWClass") ||
        !lstrcmpW(cls, L"Start") || !lstrcmpW(cls, L"TrayNotifyWnd")) {
        RECT r;
        if (GetWindowRect(h, &r)) sc->Add(r);
    }
    return TRUE;
}

// Windows 11 taskbar: the buttons only show up when walking the raw UI Automation tree.
static void WalkButtons(IUIAutomationTreeWalker* w, IUIAutomationCacheRequest* cache,
                        IUIAutomationElement* e, int depth, SpanCollector& sc) {
    if (depth > 8 || g_stop) return;
    IUIAutomationElement* c = nullptr;
    w->GetFirstChildElementBuildCache(e, cache, &c);
    while (c) {
        CONTROLTYPEID ct = 0;
        c->get_CachedControlType(&ct);
        if (ct == UIA_ButtonControlTypeId) {
            RECT r = {};
            if (SUCCEEDED(c->get_CachedBoundingRectangle(&r))) sc.Add(r);
        } else {
            WalkButtons(w, cache, c, depth + 1, sc);
        }
        IUIAutomationElement* next = nullptr;
        if (!g_stop) w->GetNextSiblingElementBuildCache(c, cache, &next);
        c->Release();
        c = next;
    }
}

// With auto-hide, the XAML buttons disappear from the tree while the taskbar is hidden.
// Keep the last ones found for each taskbar and use them meanwhile.
static std::vector<BarIcons> g_lastXaml; // touched only by the icon thread

static std::vector<Span> ReadIconSpans(IUIAutomation* uia, IUIAutomationTreeWalker* walker,
                                       IUIAutomationCacheRequest* cache, HWND bar) {
    SpanCollector sc;
    if (!GetWindowRect(bar, &sc.bar)) return {};
    sc.flat = (sc.bar.right - sc.bar.left) >= (sc.bar.bottom - sc.bar.top);

    SpanCollector xaml = sc;
    IUIAutomationElement* root = nullptr;
    if (SUCCEEDED(uia->ElementFromHandle(bar, &root)) && root) {
        WalkButtons(walker, cache, root, 0, xaml);
        root->Release();
    }
    BarIcons* last = nullptr;
    for (BarIcons& bi : g_lastXaml) if (bi.bar == bar) last = &bi;
    if (!xaml.spans.empty()) {
        if (last) last->spans = xaml.spans;
        else g_lastXaml.push_back({bar, xaml.spans});
    } else if (last) {
        xaml.spans = last->spans;
    }

    EnumChildWindows(bar, ChildProc, reinterpret_cast<LPARAM>(&sc));
    sc.spans.insert(sc.spans.end(), xaml.spans.begin(), xaml.spans.end());
    std::vector<Span>& spans = sc.spans;

    // merge neighboring icons into one block (no gaps between them)
    std::sort(spans.begin(), spans.end());
    std::vector<Span> merged;
    for (const Span& s : spans) {
        if (!merged.empty() && s.first <= merged.back().second + 8) {
            if (s.second > merged.back().second) merged.back().second = s.second;
        } else {
            merged.push_back(s);
        }
    }
    return merged;
}

static DWORD WINAPI IconThread(LPVOID) {
    if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED))) return 0;
    IUIAutomation* uia = nullptr;
    IUIAutomationTreeWalker* walker = nullptr;
    IUIAutomationCondition* rawView = nullptr;
    IUIAutomationCacheRequest* cache = nullptr;
    if (SUCCEEDED(CoCreateInstance(__uuidof(CUIAutomation), nullptr, CLSCTX_INPROC_SERVER,
                                   __uuidof(IUIAutomation), reinterpret_cast<void**>(&uia))) && uia) {
        // keep each UIA call short, so stopping the mod never waits long
        IUIAutomation2* uia2 = nullptr;
        if (SUCCEEDED(uia->QueryInterface(__uuidof(IUIAutomation2), reinterpret_cast<void**>(&uia2)))) {
            uia2->put_ConnectionTimeout(1000);
            uia2->put_TransactionTimeout(1000);
            uia2->Release();
        }
        uia->get_RawViewWalker(&walker);
        uia->CreateTrueCondition(&rawView);
        if (SUCCEEDED(uia->CreateCacheRequest(&cache)) && cache) {
            cache->AddProperty(UIA_BoundingRectanglePropertyId);
            cache->AddProperty(UIA_ControlTypePropertyId);
            if (rawView) cache->put_TreeFilter(rawView);
        }
    }
    if (!uia || !walker || !cache) {
        Wh_Log(L"UI Automation unavailable, icon blocking disabled");
    } else {
        // Read once at start, then only when the hook thread reports the cursor near a
        // taskbar (at most every 1.5 s). Meanwhile the last known spans are used.
        HANDLE waits[] = {g_stopEvent, g_wakeEvent};
        bool first = true;
        while (!g_stop) {
            if (!first && WaitForMultipleObjects(2, waits, FALSE, INFINITE) != WAIT_OBJECT_0 + 1) break;
            first = false;
            if (g_blockIcons) {
                std::vector<HWND> bars = TaskbarWindows();
                std::vector<BarIcons> found;
                for (HWND bar : bars) {
                    if (g_stop) break;
                    found.push_back({bar, ReadIconSpans(uia, walker, cache, bar)});
                }
                // forget taskbars that no longer exist (monitor unplugged, Explorer restarted)
                g_lastXaml.erase(std::remove_if(g_lastXaml.begin(), g_lastXaml.end(),
                                                [&](const BarIcons& bi) {
                                                    return std::find(bars.begin(), bars.end(), bi.bar) == bars.end();
                                                }),
                                 g_lastXaml.end());
                AcquireSRWLockExclusive(&g_iconLock);
                g_barIcons.swap(found);
                ReleaseSRWLockExclusive(&g_iconLock);
            }
            if (WaitForSingleObject(g_stopEvent, 1500) != WAIT_TIMEOUT) break; // rate limit
        }
    }
    if (cache) cache->Release();
    if (rawView) rawView->Release();
    if (walker) walker->Release();
    if (uia) uia->Release();
    CoUninitialize();
    return 0;
}

// ---------------------------------------------------------------- monitors and taskbars

static BOOL CALLBACK EnumMonProc(HMONITOR hmon, HDC, LPRECT rc, LPARAM lp) {
    auto* v = reinterpret_cast<std::vector<Mon>*>(lp);
    Mon m = {};
    m.rc = *rc;
    UINT dx = 96, dy = 96;
    m.dpi = SUCCEEDED(GetDpiForMonitor(hmon, MDT_EFFECTIVE_DPI, &dx, &dy)) && dy ? (int)dy : 96;
    v->push_back(m);
    return TRUE;
}

static void Refresh() {
    std::vector<Mon> mons;
    EnumDisplayMonitors(nullptr, nullptr, EnumMonProc, reinterpret_cast<LPARAM>(&mons));
    int margin = g_margin;

    std::vector<BarIcons> icons;
    AcquireSRWLockShared(&g_iconLock);
    icons = g_barIcons;
    ReleaseSRWLockShared(&g_iconLock);

    for (HWND h : TaskbarWindows()) {
        RECT r;
        if (!GetWindowRect(h, &r) || r.right <= r.left || r.bottom <= r.top) continue;
        int w = r.right - r.left, hgt = r.bottom - r.top;
        bool flat = w >= hgt;

        // An auto-hidden taskbar sits almost entirely outside its monitor (sometimes over
        // the neighboring one), so match it by width and edge, not by overlapping area.
        int idx = -1, side = -1; // 0 bottom, 1 top, 2 right, 3 left
        for (size_t i = 0; i < mons.size() && side != 0 && side != 2; i++) {
            const RECT& m = mons[i].rc;
            if (flat && abs(r.left - m.left) <= 2 && abs(r.right - m.right) <= 2) {
                if (r.top < m.bottom && m.bottom <= r.bottom) { idx = (int)i; side = 0; }
                else if (side < 0 && r.top <= m.top && m.top < r.bottom) { idx = (int)i; side = 1; }
            }
            if (!flat && abs(r.top - m.top) <= 2 && abs(r.bottom - m.bottom) <= 2) {
                if (r.left < m.right && m.right <= r.right) { idx = (int)i; side = 2; }
                else if (side < 0 && r.left <= m.left && m.left < r.right) { idx = (int)i; side = 3; }
            }
        }
        if (idx < 0) { // fallback: largest overlapping area
            long long best = -1;
            for (size_t i = 0; i < mons.size(); i++) {
                RECT in;
                long long a = 0;
                if (IntersectRect(&in, &mons[i].rc, &r))
                    a = (long long)(in.right - in.left) * (in.bottom - in.top);
                if (a > best) { best = a; idx = (int)i; }
            }
            if (best <= 0) idx = Nearest(mons, (r.left + r.right) / 2, (r.top + r.bottom) / 2);
            if (idx < 0) continue;
            const RECT& m = mons[idx].rc;
            side = flat ? (((r.top + r.bottom) / 2 > (m.top + m.bottom) / 2) ? 0 : 1)
                        : (((r.left + r.right) / 2 > (m.left + m.right) / 2) ? 2 : 3);
        }

        Mon& mon = mons[idx];
        const RECT& m = mon.rc;
        int mw = m.right - m.left, mh = m.bottom - m.top;
        // margin is in pixels at 100% scale, so it covers the same visual size on every monitor
        int thick = flat ? hgt : w;
        if (thick < 8 || thick > (flat ? mh : mw) / 2) thick = MulDiv(48, mon.dpi, 96);
        int band = thick + MulDiv(margin, mon.dpi, 96);
        int maxBand = (mw < mh ? mw : mh) / 2;
        if (band > maxBand) band = maxBand;

        RECT z;
        if (side == 0)      z = {m.left, m.bottom - band, m.right, m.bottom};
        else if (side == 1) z = {m.left, m.top, m.right, m.top + band};
        else if (side == 2) z = {m.right - band, m.top, m.right, m.bottom};
        else                z = {m.left, m.top, m.left + band, m.bottom};

        if (mon.hasZone) UnionRect(&mon.zone, &mon.zone, &z);
        else { mon.zone = z; mon.hasZone = true; }
        mon.flat = flat;

        for (const BarIcons& bi : icons)
            if (bi.bar == h) mon.icons.insert(mon.icons.end(), bi.spans.begin(), bi.spans.end());
    }
    g_mons.swap(mons);
}

// true = block the move from (cx,cy) to (x,y)
static bool ShouldBlock(int cx, int cy, int x, int y) {
    int src = IndexOf(g_mons, cx, cy);
    if (src < 0) return false;
    int dst = IndexOf(g_mons, x, y);
    if (dst < 0) dst = Nearest(g_mons, x, y);
    if (dst < 0 || dst == src) return false;

    const Mon& s = g_mons[src];
    const Mon& d = g_mons[dst];
    bool crossSide = x < s.rc.left || x >= s.rc.right; // otherwise it crosses vertically

    // leaving the monitor while near its taskbar
    bool leavingBar = s.hasZone && InRect(s.zone, cx, cy);
    bool wall = leavingBar;
    // entering the other monitor's taskbar strip sideways
    if (!wall && d.hasZone && InRect(d.zone, x, y) && crossSide == d.flat) wall = true;
    if (!wall) return false;

    int nx = Clamp(x, s.rc.left, s.rc.right - 1);
    int ny = Clamp(y, s.rc.top, s.rc.bottom - 1);

    // over the icons: never cross
    bool overIcons = false;
    if (leavingBar && g_blockIcons) {
        int along = s.flat ? nx : ny;
        for (const Span& sp : s.icons)
            if (along >= sp.first && along < sp.second) { overIcons = true; break; }
    }

    DWORD now = GetTickCount();
    if (now - g_lastBlock > 400) g_accum = 0;
    g_lastBlock = now;
    if (overIcons) {
        g_accum = 0;
    } else {
        g_accum += abs(x - nx) + abs(y - ny);
        int force = g_force;
        if (force > 0 && g_accum >= force) { g_accum = 0; return false; }
    }

    SetCursorPos(nx, ny);
    return true;
}

// Ask the icon thread to re-read the icons when the cursor gets near a taskbar.
static void WakeIconThreadIfNearBar(int x, int y) {
    if (!g_blockIcons || !g_wakeEvent) return;
    DWORD now = GetTickCount();
    if (now - g_lastWake < 1000) return;
    for (const Mon& m : g_mons) {
        if (m.hasZone && InRect(m.zone, x, y)) {
            g_lastWake = now;
            SetEvent(g_wakeEvent);
            return;
        }
    }
}

static LRESULT CALLBACK LowLevelMouseProc(int code, WPARAM wp, LPARAM lp) {
    if (code == HC_ACTION && wp == WM_MOUSEMOVE) {
        auto* info = reinterpret_cast<MSLLHOOKSTRUCT*>(lp);
        if (!(info->flags & LLMHF_INJECTED)) {
            WakeIconThreadIfNearBar(info->pt.x, info->pt.y);
            POINT c;
            if (GetCursorPos(&c) && ShouldBlock(c.x, c.y, info->pt.x, info->pt.y)) return 1;
        }
    }
    return CallNextHookEx(nullptr, code, wp, lp);
}

static DWORD WINAPI HookThread(LPVOID) {
    // Force the message queue to be created, and only then let WhTool_ModUninit
    // post to this thread.
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    SetEvent(g_queueReadyEvent);

    Refresh();
    HMODULE self = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&LowLevelMouseProc), &self);
    g_hook = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, self, 0);
    if (!g_hook) {
        Wh_Log(L"SetWindowsHookEx failed: %u", GetLastError());
    } else {
        Wh_Log(L"Mouse hook installed");
        // icons are only needed while the hook is running
        g_iconThread = CreateThread(nullptr, 0, IconThread, nullptr, 0, nullptr);
        if (!g_iconThread) Wh_Log(L"CreateThread (icons) failed: %u", GetLastError());
    }

    // re-read monitors, taskbars and icons every second (also survives Explorer restarts)
    UINT_PTR timer = SetTimer(nullptr, 0, 1000, nullptr);
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_TIMER || msg.message == WM_APP) {
            if (g_hook) Refresh();
            continue;
        }
        DispatchMessageW(&msg);
    }
    KillTimer(nullptr, timer);
    if (g_hook) {
        UnhookWindowsHookEx(g_hook);
        g_hook = nullptr;
    }
    return 0;
}

// ---------------------------------------------------------------- mod entry points

static void LoadSettings() {
    int m = Wh_GetIntSetting(L"margin");
    int f = Wh_GetIntSetting(L"force");
    int b = Wh_GetIntSetting(L"blockIcons");
    if (m < 0) m = 0;
    if (m > 400) m = 400;
    if (f < 0) f = 0;
    g_margin = m;
    g_force = f;
    g_blockIcons = b != 0;
}

static void CloseEvents() {
    for (HANDLE* h : {&g_stopEvent, &g_queueReadyEvent, &g_wakeEvent}) {
        if (*h) {
            CloseHandle(*h);
            *h = nullptr;
        }
    }
}

BOOL WhTool_ModInit() {
    LoadSettings();
    g_stop = false;

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_queueReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_wakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr); // auto-reset
    if (!g_stopEvent || !g_queueReadyEvent || !g_wakeEvent) {
        Wh_Log(L"CreateEvent failed: %u", GetLastError());
        CloseEvents();
        return FALSE;
    }

    g_hookThread = CreateThread(nullptr, 0, HookThread, nullptr, 0, &g_hookThreadId);
    if (!g_hookThread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
        CloseEvents();
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    if (g_hookThread) PostThreadMessageW(g_hookThreadId, WM_APP, 0, 0);
    if (g_wakeEvent && g_blockIcons) SetEvent(g_wakeEvent); // icons may have just been turned on
}

void WhTool_ModUninit() {
    g_stop = true;
    if (g_stopEvent) SetEvent(g_stopEvent);

    if (g_hookThread) {
        // The queue is ready by now, so WM_QUIT can't be lost and the wait
        // below is guaranteed to return.
        WaitForSingleObject(g_queueReadyEvent, INFINITE);
        PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_hookThread, INFINITE);
        CloseHandle(g_hookThread);
        g_hookThread = nullptr;
        g_hookThreadId = 0;
    }
    // created by the hook thread, which has finished by now
    if (g_iconThread) {
        WaitForSingleObject(g_iconThread, INFINITE);
        CloseHandle(g_iconThread);
        g_iconThread = nullptr;
    }
    CloseEvents();
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
