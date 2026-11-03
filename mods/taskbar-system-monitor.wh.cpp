// ==WindhawkMod==
// @id              taskbar-system-monitor
// @name            Taskbar System Monitor
// @description     Fully customizable CPU / GPU / RAM / disk / network monitor with 2D icons, tooltips and sparklines, drawn inside the Windows 11 taskbar (all monitors)
// @version         1.0.0
// @author          exefile04
// @github          https://github.com/exefile04
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -DWIN32_LEAN_AND_MEAN -lgdiplus -liphlpapi -lpdh -lole32 -loleaut32 -lwbemuuid -lshell32 -lgdi32 -lcomctl32 -lpsapi -ldwmapi -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar System Monitor

![Screenshot](https://raw.githubusercontent.com/exefile04/taskbar-system-monitor/main/images/screenshot.png)

Live CPU, GPU, RAM, disk and network stats drawn **inside the Windows 11 taskbar**, to the left of the first
icon, with clean outline icons. Works with Taskbar Styler docks and on every monitor.

## Features
- **Fully configurable layout** - choose what is shown and where (see below).
- **Hover** a value for details: busiest apps, memory free, disk read/write, data used today.
- **Click** opens Task Manager (configurable); **right-click** shows a small menu.
- Smooth value animation, optional 60-second sparklines, optional warning colors (off by default).
- RAM icon fills up with usage. KiB/MiB/GiB or KB/MB/GB, bytes or bits for network.
- Follows the taskbar (auto-hide, Start menu, minimising), hides while a fullscreen window covers the screen.
- Every monitor's taskbar gets its own monitor.

## How this differs from similar mods
Other mods already show CPU/GPU/RAM in the taskbar (Taskbar System Info), a network speed (Taskbar Network Speed
Indicator) or put values into the clock (Taskbar Clock Customization). This mod puts **CPU, GPU, RAM, disk and
network in one compact widget** with per-item icons (the RAM icon fills with usage), a free-form layout string,
hover tooltips (busiest apps, disk read/write, data used today), click actions and a daily data counter.

## Layout string
Columns are separated by `|`, rows inside a column by `,`. Items: `cpu`, `gpu`, `ram`, `disk`, `down`, `up`
and `-` for an empty slot (up to 3 rows).

| Layout | Result |
|---|---|
| `cpu,gpu\|ram,disk\|down,up` | default, three columns, two rows |
| `cpu\|ram\|down` | a single row |
| `cpu,ram,disk\|down,up` | three rows |

## About temperatures
Windows does not give normal programs real CPU temperatures.
- **GPU temperature** comes from the graphics driver through the same Windows interface Task Manager uses
  (works for NVIDIA and for most AMD/Intel drivers that report one).
- For accurate **CPU** temperatures run
  [LibreHardwareMonitor](https://github.com/LibreHardwareMonitor/LibreHardwareMonitor) (as administrator,
  it can stay minimised in the tray). The mod reads its WMI data automatically, even if it is started later.
- Without it the CPU value comes from Windows' ACPI thermal zone, which is only approximate and missing on some
  PCs. An integrated GPU without its own sensor can show the CPU/APU temperature (they share one die) - this can
  be turned off.

## Notes
- The monitor needs free space left of the first taskbar icon (a centered taskbar, or a Taskbar Styler dock).
  Its position is found by measuring the first icon. With a dock, set *dock margin* to the dock's margin so the
  monitor stays in place when icons are added; with 0 it sits right next to the first icon.
- The mod runs only in `explorer.exe` and draws its own windows; it does not hook any functions.
- "Data used today" is stored in the mod's local storage so it survives restarts.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- layout: "cpu,gpu|ram,disk|down,up"
  $name: Layout
  $description: "Columns separated by |, rows by comma. Items: cpu gpu ram disk down up and - for an empty slot. Up to 3 rows."
- allMonitors: true
  $name: Show on every monitor's taskbar
- refreshMs: 1000
  $name: Update interval (ms, 500-5000)
- smoothing: 40
  $name: Number smoothing (0 = off, up to 90)
- fontFamily: "Segoe UI Variable Text"
  $name: Font
- fontSize: 12
  $name: Font size (px at 100% scale)
- fontBold: false
  $name: Bold text
- textColor: ""
  $name: Text and icon color (hex RRGGBB, empty = automatic black/white)
- opacity: 100
  $name: Opacity (%)
- showIcons: true
  $name: Show icons
- iconScale: 120
  $name: Icon size (% of font size)
- colorWarnings: false
  $name: Color values that get high (off = pure black and white)
- warnUsage: 90
  $name: Warning usage level (%)
- warnTemp: 85
  $name: Warning temperature (C)
- warnColor: "FF5A5A"
  $name: Warning color (hex RRGGBB)
- showCpuTemp: true
  $name: Show CPU temperature
- showGpuTemp: true
  $name: Show GPU temperature
- igpuTempFromCpu: true
  $name: GPU without a sensor - show the CPU/APU temperature
  $description: An integrated GPU shares one die with the CPU. If the driver reports no GPU temperature, the CPU temperature is shown for it.
- tempUnit: celsius
  $name: Temperature unit
  $options:
  - celsius: Celsius
  - fahrenheit: Fahrenheit
- ramShowTotal: false
  $name: RAM - show total (14.6/15.2 GiB instead of 14.6 GiB)
- ramShowPercent: false
  $name: RAM - also show percentage
- netUnit: bytes
  $name: Network unit
  $options:
  - bytes: KiB/s, MiB/s
  - bits: Kbps, Mbps
- netInlineTotal: false
  $name: Network - show data used today next to the speed
- sparklines: false
  $name: Sparklines (60 second history behind each value)
- sparkOpacity: 30
  $name: Sparkline opacity (%)
- tooltips: true
  $name: Hover tooltips with details
- tooltipLive: false
  $name: Tooltips keep updating while hovered (off = text stays still so it is easy to read)
- binaryUnits: true
  $name: Use binary unit names (KiB / MiB / GiB instead of KB / MB / GB)
- interactive: true
  $name: Mouse interaction (turn off to make the monitor fully click-through)
- clickAction: taskmgr
  $name: Left click action
  $options:
  - taskmgr: Task Manager
  - resmon: Resource Monitor
  - custom: Custom command
  - none: Nothing
- customCommand: ""
  $name: Custom command for left click (arguments allowed)
- autoPosition: true
  $name: Auto-position next to the first taskbar icon
- dockMargin: 0
  $name: Taskbar Styler dock margin (px at 100% scale, 0 = hug the first icon)
  $description: The monitor starts this far from the screen edge (plus the left padding), so it stays put when icons are added. It never overlaps the first icon.
- leftPadding: 16
  $name: Left padding inside the taskbar (px at 100% scale)
- gapToIcons: 16
  $name: Minimum gap to the first icon (px at 100% scale)
- columnGap: 20
  $name: Gap between columns (px at 100% scale)
- itemGap: 5
  $name: Gap between icon and text (px at 100% scale)
- leftOffset: 262
  $name: Manual position - left offset (px at 100% scale)
- topOffset: 8
  $name: Manual position - top offset (px at 100% scale)
- height: 56
  $name: Manual position - height (px at 100% scale)
*/
// ==/WindhawkModSettings==

// Threads:
//  * worker      - message loop, timers, drawing (GDI+), event hooks. Never touches the taskbar's windows.
//  * sampler     - all sensor access (PDH / WMI / D3DKMT / iphlpapi).
//  * UI Automation - measures the first taskbar icon, only when something may have moved it.
//  * the taskbar's own UI thread - hosts the overlay windows and their tooltips (so no cross-thread owner
//    relationship ties its input queue to a mod thread). It only applies finished frames and handles the mouse.
// Every thread is signalled and joined (without timeout) in Wh_ModUninit.

#define NOMINMAX
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <wtypes.h>
#include <ole2.h>
#include <algorithm>
using std::min;
using std::max;
#include <gdiplus.h>
#include <iphlpapi.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <wbemidl.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <uiautomation.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <atomic>
#include <cmath>
#include <cstdarg>
#include <cwchar>
#include <map>
#include <memory>
#include <optional>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

using namespace Gdiplus;

// ------------------------------------------------------------- settings ----

enum Item { IT_NONE = -1, IT_CPU = 0, IT_GPU, IT_RAM, IT_DISK, IT_DOWN, IT_UP, IT_COUNT };

struct Settings {
    std::vector<std::vector<int>> cols;
    bool allMonitors = true;
    int refreshMs = 1000, smoothing = 40;
    std::wstring fontFamily = L"Segoe UI Variable Text";
    int fontSize = 12;
    bool bold = false, hasColor = false;
    BYTE tr = 255, tg = 255, tb = 255;
    int opacity = 100;
    bool icons = true;
    int iconScale = 120;
    bool warn = false;
    int warnUsage = 90, warnTemp = 85;
    BYTE wr = 255, wg = 90, wb = 90;
    bool showCpuTemp = true, showGpuTemp = true, igpuFromCpu = true, fahrenheit = false;
    bool ramTotal = false, ramPct = false, bits = false, netTotal = false;
    bool spark = false;
    int sparkOp = 30;
    bool tips = true, interactive = true, tipLive = false, binary = true;
    int clickAction = 0;  // 0 taskmgr, 1 resmon, 2 custom, 3 none
    std::wstring customCmd;
    bool autoPos = true;
    int dockMargin = 0, leftPad = 16, gapIcons = 16, colGap = 20, itemGap = 5;
    int leftOffset = 262, topOffset = 8, height = 56;
};

// Settings are immutable snapshots: a change publishes a new snapshot, readers on any thread use the current one.
// Old snapshots stay alive until the mod unloads, so a reader never sees a half-written or freed object.
static std::atomic<const Settings*> g_cfgPtr{nullptr};
static std::mutex g_cfgMutex;
static std::vector<std::unique_ptr<Settings>> g_cfgAll;
#define g_s (*g_cfgPtr.load())

static std::wstring GetStr(const wchar_t* name) {
    PCWSTR v = Wh_GetStringSetting(name);
    std::wstring r = v;
    Wh_FreeStringSetting(v);
    return r;
}

static bool ParseHex(std::wstring s, BYTE* r, BYTE* g, BYTE* b) {
    if (!s.empty() && s[0] == L'#') s.erase(0, 1);
    if (s.size() != 6) return false;
    wchar_t* end = nullptr;
    unsigned long v = wcstoul(s.c_str(), &end, 16);
    if (!end || *end) return false;
    *r = (BYTE)(v >> 16); *g = (BYTE)(v >> 8); *b = (BYTE)v;
    return true;
}

static int ItemFromName(const std::wstring& s) {
    std::wstring t;
    for (wchar_t c : s) if (c != L' ' && c != L'\t') t += (wchar_t)towlower(c);
    if (t == L"cpu") return IT_CPU;
    if (t == L"gpu") return IT_GPU;
    if (t == L"ram" || t == L"mem" || t == L"memory") return IT_RAM;
    if (t == L"disk") return IT_DISK;
    if (t == L"down" || t == L"download" || t == L"dl") return IT_DOWN;
    if (t == L"up" || t == L"upload" || t == L"ul") return IT_UP;
    return IT_NONE;
}

static std::vector<std::vector<int>> ParseLayout(const std::wstring& layout) {
    std::vector<std::vector<int>> cols;
    size_t pos = 0;
    while (pos <= layout.size()) {
        size_t bar = layout.find(L'|', pos);
        std::wstring col = layout.substr(pos, bar == std::wstring::npos ? std::wstring::npos : bar - pos);
        std::vector<int> items;
        size_t p2 = 0;
        while (p2 <= col.size() && items.size() < 3) {
            size_t comma = col.find(L',', p2);
            items.push_back(ItemFromName(col.substr(p2, comma == std::wstring::npos ? std::wstring::npos : comma - p2)));
            if (comma == std::wstring::npos) break;
            p2 = comma + 1;
        }
        bool any = false;
        for (int i : items) if (i != IT_NONE) any = true;
        if (any) cols.push_back(items);
        if (bar == std::wstring::npos) break;
        pos = bar + 1;
    }
    if (cols.empty()) cols = {{IT_CPU, IT_GPU}, {IT_RAM, IT_DISK}, {IT_DOWN, IT_UP}};
    return cols;
}

static void LoadSettings() {
    auto sp = std::make_unique<Settings>();
    Settings& s = *sp;
    s.cols = ParseLayout(GetStr(L"layout"));
    s.allMonitors = Wh_GetIntSetting(L"allMonitors") != 0;
    s.refreshMs = std::clamp<int>(Wh_GetIntSetting(L"refreshMs"), 500, 5000);
    s.smoothing = std::clamp<int>(Wh_GetIntSetting(L"smoothing"), 0, 90);
    s.fontFamily = GetStr(L"fontFamily");
    if (s.fontFamily.empty()) s.fontFamily = L"Segoe UI Variable Text";
    s.fontSize = std::clamp<int>(Wh_GetIntSetting(L"fontSize"), 8, 30);
    s.bold = Wh_GetIntSetting(L"fontBold") != 0;
    s.hasColor = ParseHex(GetStr(L"textColor"), &s.tr, &s.tg, &s.tb);
    s.opacity = std::clamp<int>(Wh_GetIntSetting(L"opacity"), 10, 100);
    s.icons = Wh_GetIntSetting(L"showIcons") != 0;
    s.iconScale = std::clamp<int>(Wh_GetIntSetting(L"iconScale"), 60, 220);
    s.warn = Wh_GetIntSetting(L"colorWarnings") != 0;
    s.warnUsage = Wh_GetIntSetting(L"warnUsage");
    s.warnTemp = Wh_GetIntSetting(L"warnTemp");
    if (!ParseHex(GetStr(L"warnColor"), &s.wr, &s.wg, &s.wb)) { s.wr = 255; s.wg = 90; s.wb = 90; }
    s.showCpuTemp = Wh_GetIntSetting(L"showCpuTemp") != 0;
    s.showGpuTemp = Wh_GetIntSetting(L"showGpuTemp") != 0;
    s.igpuFromCpu = Wh_GetIntSetting(L"igpuTempFromCpu") != 0;
    s.fahrenheit = GetStr(L"tempUnit") == L"fahrenheit";
    s.ramTotal = Wh_GetIntSetting(L"ramShowTotal") != 0;
    s.ramPct = Wh_GetIntSetting(L"ramShowPercent") != 0;
    s.bits = GetStr(L"netUnit") == L"bits";
    s.netTotal = Wh_GetIntSetting(L"netInlineTotal") != 0;
    s.spark = Wh_GetIntSetting(L"sparklines") != 0;
    s.sparkOp = std::clamp<int>(Wh_GetIntSetting(L"sparkOpacity"), 5, 100);
    s.tips = Wh_GetIntSetting(L"tooltips") != 0;
    s.tipLive = Wh_GetIntSetting(L"tooltipLive") != 0;
    s.binary = Wh_GetIntSetting(L"binaryUnits") != 0;
    s.interactive = Wh_GetIntSetting(L"interactive") != 0;
    std::wstring ca = GetStr(L"clickAction");
    s.clickAction = ca == L"resmon" ? 1 : ca == L"custom" ? 2 : ca == L"none" ? 3 : 0;
    s.customCmd = GetStr(L"customCommand");
    s.autoPos = Wh_GetIntSetting(L"autoPosition") != 0;
    s.dockMargin = Wh_GetIntSetting(L"dockMargin");
    s.leftPad = Wh_GetIntSetting(L"leftPadding");
    s.gapIcons = Wh_GetIntSetting(L"gapToIcons");
    s.colGap = Wh_GetIntSetting(L"columnGap");
    s.itemGap = Wh_GetIntSetting(L"itemGap");
    s.leftOffset = Wh_GetIntSetting(L"leftOffset");
    s.topOffset = Wh_GetIntSetting(L"topOffset");
    s.height = Wh_GetIntSetting(L"height");

    std::lock_guard<std::mutex> lock(g_cfgMutex);
    g_cfgPtr.store(sp.get());
    g_cfgAll.push_back(std::move(sp));
}

// ---------------------------------------------------------------- state ----

struct Stats {
    double cpu = 0, ram = 0, ramUsedGB = 0, ramTotalGB = 0;
    double gpu = -1, cpuTemp = -1, gpuTemp = -1, disk = -1;
    double down = 0, up = 0, diskRead = 0, diskWrite = 0;
    double todayDown = 0, todayUp = 0;
    int cpuTempSrc = 0;  // 0 none, 1 LibreHardwareMonitor, 2 Windows thermal zone
};
static Stats g_st;  // private to the sampler thread
static Stats g_pub;  // published copy, read by everybody else
static SRWLOCK g_pubLock = SRWLOCK_INIT;

static Stats GetStats() {
    AcquireSRWLockShared(&g_pubLock);
    Stats s = g_pub;
    ReleaseSRWLockShared(&g_pubLock);
    return s;
}

struct Disp { double cpu = 0, gpu = -1, ram = 0, disk = -1, down = 0, up = 0; };
static Disp g_disp;  // worker thread only

static std::atomic<bool> g_stop{false};
static std::atomic<HWND> g_msgWnd{nullptr};
static std::atomic<bool> g_fullscreen{false};
static std::atomic<bool> g_lightTheme{false};
static HANDLE g_thread = nullptr;
static HANDLE g_stopEvent = nullptr;  // manual reset, set on unload
static HANDLE g_uiaEvent = nullptr;   // auto reset: "something may have moved the taskbar icons"
static UINT g_runMsg = 0;
static const wchar_t* kClass = L"WhTaskbarSysMon";
static const wchar_t* kMsgClass = L"WhTaskbarSysMonMsg";
static const UINT WM_SETTINGS = WM_APP + 1;
static const UINT WM_FRAME = WM_APP + 10;
static const UINT WM_UPDATEPOS = WM_APP + 11;
static const UINT WM_APPLYSTYLE = WM_APP + 12;

static HINSTANCE GetModuleInstance() {
    HINSTANCE h = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&GetModuleInstance, &h);
    return h;
}

// -------------------------------------------------------------- sensors ----

static ULONGLONG FtU(FILETIME f) { return ((ULONGLONG)f.dwHighDateTime << 32) | f.dwLowDateTime; }

static void SampleCpuRam() {
    static ULONGLONG pi = 0, pk = 0, pu = 0;
    FILETIME i, k, u;
    if (GetSystemTimes(&i, &k, &u)) {
        ULONGLONG I = FtU(i), K = FtU(k), U = FtU(u);
        if (pk || pu) {
            ULONGLONG total = (K - pk) + (U - pu);  // kernel time includes idle
            if (total) g_st.cpu = 100.0 * (double)(total - (I - pi)) / (double)total;
        }
        pi = I; pk = K; pu = U;
    }
    MEMORYSTATUSEX m = {sizeof(m)};
    if (GlobalMemoryStatusEx(&m)) {
        g_st.ram = m.dwMemoryLoad;
        g_st.ramTotalGB = m.ullTotalPhys / 1073741824.0;
        g_st.ramUsedGB = (m.ullTotalPhys - m.ullAvailPhys) / 1073741824.0;
    }
}

// persisted "data used today"
struct NetToday { int day; double down, up; };
static int g_dayKey = 0;

static void LoadNetToday() {
    NetToday nt = {};
    SYSTEMTIME t; GetLocalTime(&t);
    int key = t.wYear * 10000 + t.wMonth * 100 + t.wDay;
    if (Wh_GetBinaryValue(L"netToday", &nt, sizeof(nt)) == sizeof(nt) && nt.day == key) {
        g_st.todayDown = nt.down; g_st.todayUp = nt.up;
    }
    g_dayKey = key;
}

static void PersistNetToday(bool force) {
    static ULONGLONG last = 0;
    ULONGLONG now = GetTickCount64();
    if (!force && now - last < 30000) return;
    last = now;
    NetToday nt = {g_dayKey, g_st.todayDown, g_st.todayUp};
    Wh_SetBinaryValue(L"netToday", &nt, sizeof(nt));
}

// 64-bit interface counters (no wrap-around even at 10 Gbps); only real (hardware) adapters, so VPN / virtual
// switches do not count traffic twice
static void SampleNet() {
    static std::map<NET_IFINDEX, std::pair<ULONG64, ULONG64>> prev;
    static ULONGLONG pTick = 0;
    PMIB_IF_TABLE2 t = nullptr;
    if (GetIfTable2(&t) != NO_ERROR || !t) return;
    ULONGLONG now = GetTickCount64();
    double dIn = 0, dOut = 0;
    std::map<NET_IFINDEX, std::pair<ULONG64, ULONG64>> cur;
    for (ULONG i = 0; i < t->NumEntries; i++) {
        const MIB_IF_ROW2& r = t->Table[i];
        if (r.Type == IF_TYPE_SOFTWARE_LOOPBACK || r.OperStatus != IfOperStatusUp) continue;
        if (!r.InterfaceAndOperStatusFlags.HardwareInterface || r.InterfaceAndOperStatusFlags.FilterInterface) continue;
        cur[r.InterfaceIndex] = {r.InOctets, r.OutOctets};
        auto it = prev.find(r.InterfaceIndex);
        if (it != prev.end() && pTick) {
            if (r.InOctets >= it->second.first) dIn += (double)(r.InOctets - it->second.first);
            if (r.OutOctets >= it->second.second) dOut += (double)(r.OutOctets - it->second.second);
        }
    }
    FreeMibTable(t);
    prev = std::move(cur);
    if (pTick && now > pTick) {
        double dt = (now - pTick) / 1000.0;
        g_st.down = dIn / dt;
        g_st.up = dOut / dt;
        SYSTEMTIME st; GetLocalTime(&st);
        int key = st.wYear * 10000 + st.wMonth * 100 + st.wDay;
        if (key != g_dayKey) { g_dayKey = key; g_st.todayDown = g_st.todayUp = 0; }
        g_st.todayDown += dIn;
        g_st.todayUp += dOut;
    }
    pTick = now;
}

// ---- GPU usage via PDH (any vendor): the busiest adapter, like Task Manager shows each GPU ----
static PDH_HQUERY g_pdhQ = nullptr;
static PDH_HCOUNTER g_pdhC = nullptr;
static int g_pdhAge = 999;

static void SampleGpuPdh() {
    if (g_pdhAge >= 10) {  // re-expand the wildcard so new processes are picked up
        if (g_pdhQ) PdhCloseQuery(g_pdhQ);
        g_pdhQ = nullptr; g_pdhC = nullptr; g_pdhAge = 0;
        if (PdhOpenQueryW(nullptr, 0, &g_pdhQ) == ERROR_SUCCESS) {
            if (PdhAddEnglishCounterW(g_pdhQ, L"\\GPU Engine(*)\\Utilization Percentage", 0, &g_pdhC) == ERROR_SUCCESS)
                PdhCollectQueryData(g_pdhQ);
            else { PdhCloseQuery(g_pdhQ); g_pdhQ = nullptr; }
        }
        return;
    }
    g_pdhAge++;
    if (!g_pdhQ || PdhCollectQueryData(g_pdhQ) != ERROR_SUCCESS) return;
    DWORD size = 0, count = 0;
    PdhGetFormattedCounterArrayW(g_pdhC, PDH_FMT_DOUBLE, &size, &count, nullptr);
    if (!size) return;
    std::vector<BYTE> buf(size);
    auto* items = (PDH_FMT_COUNTERVALUE_ITEM_W*)buf.data();
    if (PdhGetFormattedCounterArrayW(g_pdhC, PDH_FMT_DOUBLE, &size, &count, items) != ERROR_SUCCESS) return;
    // adapter ("luid_..._phys_N") -> engine type -> summed utilization
    std::map<std::wstring, std::map<std::wstring, double>> perAdapter;
    for (DWORD i = 0; i < count; i++) {
        if (items[i].FmtValue.CStatus != PDH_CSTATUS_VALID_DATA && items[i].FmtValue.CStatus != PDH_CSTATUS_NEW_DATA) continue;
        std::wstring name = items[i].szName;
        size_t l = name.find(L"luid_"), e = name.find(L"_eng_"), t = name.find(L"engtype_");
        std::wstring adapter = (l != std::wstring::npos && e != std::wstring::npos && e > l) ? name.substr(l, e - l) : L"?";
        std::wstring type = t != std::wstring::npos ? name.substr(t + 8) : L"?";
        perAdapter[adapter][type] += items[i].FmtValue.doubleValue;
    }
    double best = 0;
    for (auto& a : perAdapter)
        for (auto& kv : a.second) best = std::max(best, kv.second);
    g_st.gpu = std::min(best, 100.0);
}

// ---- disk activity via PDH ----
static PDH_HQUERY g_diskQ = nullptr;
static PDH_HCOUNTER g_diskT = nullptr, g_diskR = nullptr, g_diskW = nullptr;
static bool g_diskTried = false;

static void SampleDisk() {
    if (!g_diskTried) {
        g_diskTried = true;
        if (PdhOpenQueryW(nullptr, 0, &g_diskQ) == ERROR_SUCCESS) {
            bool ok = PdhAddEnglishCounterW(g_diskQ, L"\\PhysicalDisk(_Total)\\% Disk Time", 0, &g_diskT) == ERROR_SUCCESS;
            PdhAddEnglishCounterW(g_diskQ, L"\\PhysicalDisk(_Total)\\Disk Read Bytes/sec", 0, &g_diskR);
            PdhAddEnglishCounterW(g_diskQ, L"\\PhysicalDisk(_Total)\\Disk Write Bytes/sec", 0, &g_diskW);
            if (ok) PdhCollectQueryData(g_diskQ); else { PdhCloseQuery(g_diskQ); g_diskQ = nullptr; }
        }
        return;
    }
    if (!g_diskQ || PdhCollectQueryData(g_diskQ) != ERROR_SUCCESS) return;
    PDH_FMT_COUNTERVALUE v;
    if (PdhGetFormattedCounterValue(g_diskT, PDH_FMT_DOUBLE, nullptr, &v) == ERROR_SUCCESS) g_st.disk = std::clamp(v.doubleValue, 0.0, 100.0);
    if (g_diskR && PdhGetFormattedCounterValue(g_diskR, PDH_FMT_DOUBLE, nullptr, &v) == ERROR_SUCCESS) g_st.diskRead = v.doubleValue;
    if (g_diskW && PdhGetFormattedCounterValue(g_diskW, PDH_FMT_DOUBLE, nullptr, &v) == ERROR_SUCCESS) g_st.diskWrite = v.doubleValue;
}

// ---- CPU temp fallback: Windows ACPI thermal zones via PDH (no admin needed) ----
static PDH_HQUERY g_thQ = nullptr;
static PDH_HCOUNTER g_thC = nullptr;
static bool g_thTried = false;
static double g_cpuTempThermal = -1;  // sampler-private partial results, merged in PublishStats
static double g_cpuTempLhm = -1;
static double g_gpuTempKmt = -1;
static double g_gpuTempLhm = -1;

static void SampleThermalPdh() {
    if (!g_thTried) {
        g_thTried = true;
        if (PdhOpenQueryW(nullptr, 0, &g_thQ) == ERROR_SUCCESS) {
            if (PdhAddEnglishCounterW(g_thQ, L"\\Thermal Zone Information(*)\\Temperature", 0, &g_thC) == ERROR_SUCCESS)
                PdhCollectQueryData(g_thQ);
            else { PdhCloseQuery(g_thQ); g_thQ = nullptr; }
        }
        return;
    }
    if (!g_thQ || PdhCollectQueryData(g_thQ) != ERROR_SUCCESS) return;
    DWORD size = 0, count = 0;
    PdhGetFormattedCounterArrayW(g_thC, PDH_FMT_DOUBLE, &size, &count, nullptr);
    if (!size) return;
    std::vector<BYTE> buf(size);
    auto* items = (PDH_FMT_COUNTERVALUE_ITEM_W*)buf.data();
    if (PdhGetFormattedCounterArrayW(g_thC, PDH_FMT_DOUBLE, &size, &count, items) != ERROR_SUCCESS) return;
    double best = -1;
    for (DWORD i = 0; i < count; i++) {
        if (items[i].FmtValue.CStatus != PDH_CSTATUS_VALID_DATA && items[i].FmtValue.CStatus != PDH_CSTATUS_NEW_DATA) continue;
        double c = items[i].FmtValue.doubleValue - 273.15;  // reported in Kelvin
        if (c > 10 && c < 120) best = std::max(best, c);
    }
    if (best > 0) g_cpuTempThermal = best;
}

// ---- GPU temperature from the graphics driver (D3DKMT, the interface Task Manager uses; any vendor) ----
// Declared here because d3dkmthk.h is not available in every build environment.
struct KmtAdapterInfo { UINT hAdapter; LUID adapterLuid; ULONG numOfSources; BOOL precisePresent; };
struct KmtEnumAdapters2 { ULONG numAdapters; KmtAdapterInfo* pAdapters; };
struct KmtCloseAdapter { UINT hAdapter; };
struct KmtQueryAdapterInfo { UINT hAdapter; UINT type; VOID* pPrivateDriverData; UINT privateDriverDataSize; };
struct KmtAdapterPerfData {
    UINT PhysicalAdapterIndex;
    ULONGLONG MemoryFrequency, MaxMemoryFrequency, MaxMemoryFrequencyOC, MemoryBandwidth, PCIEBandwidth;
    ULONG FanRPM, Power, Temperature;
    UCHAR PowerStateOverride;
};
static const UINT kKmtAdapterPerfData = 62;  // KMTQAITYPE_ADAPTERPERFDATA
typedef LONG(WINAPI* KmtEnumAdapters2_t)(KmtEnumAdapters2*);
typedef LONG(WINAPI* KmtQueryAdapterInfo_t)(KmtQueryAdapterInfo*);
typedef LONG(WINAPI* KmtCloseAdapter_t)(const KmtCloseAdapter*);
static KmtEnumAdapters2_t g_kmtEnum = nullptr;
static KmtQueryAdapterInfo_t g_kmtQuery = nullptr;
static KmtCloseAdapter_t g_kmtClose = nullptr;
static bool g_kmtTried = false;

static void SampleGpuTempKmt() {
    if (!g_kmtTried) {
        g_kmtTried = true;
        HMODULE gdi = GetModuleHandleW(L"gdi32.dll");  // always loaded in explorer
        if (gdi) {
            g_kmtEnum = (KmtEnumAdapters2_t)GetProcAddress(gdi, "D3DKMTEnumAdapters2");
            g_kmtQuery = (KmtQueryAdapterInfo_t)GetProcAddress(gdi, "D3DKMTQueryAdapterInfo");
            g_kmtClose = (KmtCloseAdapter_t)GetProcAddress(gdi, "D3DKMTCloseAdapter");
        }
    }
    if (!g_kmtEnum || !g_kmtQuery || !g_kmtClose) return;
    KmtEnumAdapters2 en = {};
    g_kmtEnum(&en);  // first call only reports the count
    if (!en.numAdapters || en.numAdapters > 16) return;
    std::vector<KmtAdapterInfo> adapters(en.numAdapters);
    en.pAdapters = adapters.data();
    if (g_kmtEnum(&en) != 0) return;
    double best = -1;
    for (ULONG i = 0; i < en.numAdapters; i++) {
        KmtAdapterPerfData perf = {};
        KmtQueryAdapterInfo q = {adapters[i].hAdapter, kKmtAdapterPerfData, &perf, sizeof(perf)};
        if (g_kmtQuery(&q) == 0 && perf.Temperature > 0) best = std::max(best, perf.Temperature / 10.0);  // tenths of a degree C
        KmtCloseAdapter c = {adapters[i].hAdapter};
        g_kmtClose(&c);
    }
    g_gpuTempKmt = best;
}

// ---- WMI: LibreHardwareMonitor (accurate CPU temperature, GPU temperature if the driver gives none) ----
static IWbemServices* g_lhm = nullptr;
static ULONGLONG g_lastConnect = 0;

static bool WmiConnect(const wchar_t* ns, IWbemServices** out) {
    IWbemLocator* loc = nullptr;
    if (FAILED(CoCreateInstance(CLSID_WbemLocator, nullptr, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&loc))) return false;
    BSTR b = SysAllocString(ns);
    HRESULT hr = loc->ConnectServer(b, nullptr, nullptr, nullptr, 0, nullptr, nullptr, out);
    SysFreeString(b);
    loc->Release();
    if (FAILED(hr)) { *out = nullptr; return false; }
    CoSetProxyBlanket(*out, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr, RPC_C_AUTHN_LEVEL_CALL,
                      RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);
    return true;
}

static double VarToDouble(VARIANT& v) {
    VARIANT d; VariantInit(&d);
    double r = -1;
    if (SUCCEEDED(VariantChangeType(&d, &v, 0, VT_R8))) r = d.dblVal;
    VariantClear(&d);
    return r;
}

static void SampleWmi() {
    ULONGLONG now = GetTickCount64();
    if (!g_lhm && now - g_lastConnect > 30000) {  // keeps retrying, so LibreHardwareMonitor started later is found
        g_lastConnect = now;
        WmiConnect(L"ROOT\\LibreHardwareMonitor", &g_lhm);
    }
    if (!g_lhm) { g_cpuTempLhm = g_gpuTempLhm = -1; return; }

    IEnumWbemClassObject* en = nullptr;
    BSTR lang = SysAllocString(L"WQL");
    BSTR query = SysAllocString(L"SELECT Name,Identifier,Value FROM Sensor WHERE SensorType='Temperature'");
    HRESULT hr = g_lhm->ExecQuery(lang, query, WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, nullptr, &en);
    SysFreeString(lang); SysFreeString(query);
    if (FAILED(hr)) {  // LibreHardwareMonitor was closed: drop the connection and look again later
        g_lhm->Release(); g_lhm = nullptr;
        g_cpuTempLhm = g_gpuTempLhm = -1;
        return;
    }
    int cpuPrio = 99, gpuPrio = 99;
    double cpu = -1, gpu = -1;
    IWbemClassObject* o = nullptr; ULONG n = 0;
    while (en->Next(2000, 1, &o, &n) == S_OK && n) {
        VARIANT name, id, val;
        VariantInit(&name); VariantInit(&id); VariantInit(&val);
        o->Get(L"Name", 0, &name, nullptr, nullptr);
        o->Get(L"Identifier", 0, &id, nullptr, nullptr);
        o->Get(L"Value", 0, &val, nullptr, nullptr);
        std::wstring nm = name.vt == VT_BSTR ? name.bstrVal : L"";
        std::wstring ident = id.vt == VT_BSTR ? id.bstrVal : L"";
        double v = VarToDouble(val);
        if (v > 0) {
            if (ident.rfind(L"/intelcpu/", 0) == 0 || ident.rfind(L"/amdcpu/", 0) == 0) {
                int p = (nm.find(L"Package") != std::wstring::npos || nm.find(L"Tctl") != std::wstring::npos ||
                         nm.find(L"Tdie") != std::wstring::npos) ? 0
                      : nm.find(L"Core Max") != std::wstring::npos ? 1
                      : nm.find(L"Core") != std::wstring::npos ? 2 : 3;
                if (p < cpuPrio) { cpuPrio = p; cpu = v; }
            } else if (ident.rfind(L"/gpu", 0) == 0) {
                int p = nm.find(L"GPU Core") != std::wstring::npos ? 0 : 1;
                if (p < gpuPrio) { gpuPrio = p; gpu = v; }
            }
        }
        VariantClear(&name); VariantClear(&id); VariantClear(&val);
        o->Release();
    }
    en->Release();
    g_cpuTempLhm = cpu;
    g_gpuTempLhm = gpu;
}

// ---- top processes (only sampled while a tooltip is being hovered) ----
struct ProcAgg { std::wstring name; double cpu = 0; ULONGLONG mem = 0; };
static std::vector<ProcAgg> g_topCpu, g_topMem;
static SRWLOCK g_procLock = SRWLOCK_INIT;
static std::atomic<ULONGLONG> g_procWanted{0};

static void SampleProcs() {
    static std::unordered_map<DWORD, ULONGLONG> prev;
    static ULONGLONG prevTick = 0;
    static int ncpu = 0;
    if (!ncpu) { SYSTEM_INFO si; GetSystemInfo(&si); ncpu = std::max<int>(1, (int)si.dwNumberOfProcessors); }
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return;
    std::unordered_map<DWORD, ULONGLONG> cur;
    std::unordered_map<std::wstring, ProcAgg> agg;
    ULONGLONG now = GetTickCount64();
    double dt = prevTick ? (now - prevTick) / 1000.0 : 0;
    PROCESSENTRY32W pe = {sizeof(pe)};
    if (Process32FirstW(snap, &pe)) {
        do {
            if (pe.th32ProcessID == 0 || pe.th32ProcessID == 4) continue;
            HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pe.th32ProcessID);
            if (!h) continue;
            FILETIME c, e, k, u;
            PROCESS_MEMORY_COUNTERS pmc = {sizeof(pmc)};
            bool ok = GetProcessTimes(h, &c, &e, &k, &u) != 0;
            GetProcessMemoryInfo(h, &pmc, sizeof(pmc));
            CloseHandle(h);
            if (!ok) continue;
            ULONGLONG t = FtU(k) + FtU(u);
            cur[pe.th32ProcessID] = t;
            double cpu = 0;
            auto it = prev.find(pe.th32ProcessID);
            if (dt > 0 && it != prev.end() && t >= it->second) cpu = (t - it->second) / 1e7 / dt / ncpu * 100.0;
            std::wstring name = pe.szExeFile;
            if (name.size() > 4 && _wcsicmp(name.c_str() + name.size() - 4, L".exe") == 0) name.resize(name.size() - 4);
            ProcAgg& a = agg[name];
            a.name = name; a.cpu += cpu; a.mem += pmc.WorkingSetSize;
        } while (Process32NextW(snap, &pe));
    }
    CloseHandle(snap);
    prev = std::move(cur);
    prevTick = now;

    std::vector<ProcAgg> all;
    for (auto& kv : agg) all.push_back(kv.second);
    std::vector<ProcAgg> byCpu = all, byMem = all;
    std::sort(byCpu.begin(), byCpu.end(), [](const ProcAgg& a, const ProcAgg& b) { return a.cpu > b.cpu; });
    std::sort(byMem.begin(), byMem.end(), [](const ProcAgg& a, const ProcAgg& b) { return a.mem > b.mem; });
    if (byCpu.size() > 3) byCpu.resize(3);
    if (byMem.size() > 3) byMem.resize(3);
    AcquireSRWLockExclusive(&g_procLock);
    g_topCpu = std::move(byCpu);
    g_topMem = std::move(byMem);
    ReleaseSRWLockExclusive(&g_procLock);
}

// ---- 60 second history for sparklines ----
static double g_hist[IT_COUNT][60];
static int g_histPos = 0;
static SRWLOCK g_histLock = SRWLOCK_INIT;

static void PushHistory() {
    AcquireSRWLockExclusive(&g_histLock);
    g_hist[IT_CPU][g_histPos] = g_st.cpu;
    g_hist[IT_GPU][g_histPos] = std::max(0.0, g_st.gpu);
    g_hist[IT_RAM][g_histPos] = g_st.ram;
    g_hist[IT_DISK][g_histPos] = std::max(0.0, g_st.disk);
    g_hist[IT_DOWN][g_histPos] = g_st.down;
    g_hist[IT_UP][g_histPos] = g_st.up;
    g_histPos = (g_histPos + 1) % 60;
    ReleaseSRWLockExclusive(&g_histLock);
}

static void CopyHistory(int item, double* out) {
    AcquireSRWLockShared(&g_histLock);
    for (int i = 0; i < 60; i++) out[i] = g_hist[item][(g_histPos + i) % 60];
    ReleaseSRWLockShared(&g_histLock);
}

static void PublishStats() {
    // merge the partial temperature sources: LibreHardwareMonitor beats the Windows thermal zone, the driver beats LHM
    g_st.cpuTemp = g_cpuTempLhm > 0 ? g_cpuTempLhm : g_cpuTempThermal;
    g_st.cpuTempSrc = g_cpuTempLhm > 0 ? 1 : g_cpuTempThermal > 0 ? 2 : 0;
    g_st.gpuTemp = g_gpuTempKmt > 0 ? g_gpuTempKmt : g_gpuTempLhm;
    AcquireSRWLockExclusive(&g_pubLock);
    g_pub = g_st;
    ReleaseSRWLockExclusive(&g_pubLock);
}

static void SampleAll() {
    static int tick = 0;
    SampleCpuRam();
    SampleNet();
    SampleGpuPdh();
    SampleGpuTempKmt();
    SampleDisk();
    if (tick++ % 2 == 0) SampleWmi();
    if (!g_lhm) SampleThermalPdh();
    if (GetTickCount64() - g_procWanted.load() < 4000) SampleProcs();
    PushHistory();
    PublishStats();
    PersistNetToday(false);
}

static DWORD WINAPI SamplerWorker(LPVOID) {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    while (!g_stop) {
        SampleAll();
        if (WaitForSingleObject(g_stopEvent, g_cfgPtr.load()->refreshMs) == WAIT_OBJECT_0) break;
    }
    PersistNetToday(true);
    if (g_pdhQ) { PdhCloseQuery(g_pdhQ); g_pdhQ = nullptr; }
    if (g_thQ) { PdhCloseQuery(g_thQ); g_thQ = nullptr; }
    if (g_diskQ) { PdhCloseQuery(g_diskQ); g_diskQ = nullptr; }
    if (g_lhm) { g_lhm->Release(); g_lhm = nullptr; }
    CoUninitialize();
    return 0;
}

// ---------------------------------------------------- taskbar discovery ----

struct BtnInfo {
    bool ok = false;
    LONG relLeft = 0, relTop = 0, h = 0;  // relative to the taskbar window's top-left
};
static std::unordered_map<HWND, BtnInfo> g_btns;
static std::vector<HWND> g_tbList;
static SRWLOCK g_btnLock = SRWLOCK_INIT;
static HANDLE g_uiaThread = nullptr;
static HANDLE g_samplerThread = nullptr;

static BtnInfo GetBtn(HWND tb) {
    BtnInfo b;
    AcquireSRWLockShared(&g_btnLock);
    auto it = g_btns.find(tb);
    if (it != g_btns.end()) b = it->second;
    ReleaseSRWLockShared(&g_btnLock);
    return b;
}

static void MeasureTaskbarButton(IUIAutomation* uia, IUIAutomationCacheRequest* cache, IUIAutomationCondition* cond, HWND tbWnd) {
    IUIAutomationElement* root = nullptr;
    if (FAILED(uia->ElementFromHandle(tbWnd, &root)) || !root) return;
    IUIAutomationElementArray* arr = nullptr;
    root->FindAllBuildCache(TreeScope_Descendants, cond, cache, &arr);  // all rectangles in one round trip

    RECT tb = {};
    GetWindowRect(tbWnd, &tb);
    bool have = false;
    RECT best = {};
    if (arr) {
        int len = 0;
        arr->get_Length(&len);
        for (int i = 0; i < len; i++) {
            IUIAutomationElement* el = nullptr;
            if (FAILED(arr->GetElement(i, &el)) || !el) continue;
            RECT r = {};
            if (SUCCEEDED(el->get_CachedBoundingRectangle(&r)) && r.right - r.left > 10 && r.bottom - r.top > 10 &&
                r.left >= tb.left && r.right <= tb.right && r.top >= tb.top - 4 && r.bottom <= tb.bottom + 4) {
                if (!have || r.left < best.left) { best = r; have = true; }
            }
            el->Release();
        }
        arr->Release();
    }
    root->Release();

    if (have) {
        AcquireSRWLockExclusive(&g_btnLock);
        BtnInfo& b = g_btns[tbWnd];
        b.ok = true;
        b.relLeft = best.left - tb.left;
        b.relTop = best.top - tb.top;
        b.h = best.bottom - best.top;
        ReleaseSRWLockExclusive(&g_btnLock);
    }
}

// Measures the first icon when g_uiaEvent says something may have moved it (taskbar moved, app focus / minimise
// changes, settings), debounced, plus a slow safety net. Quick retries only until the first measurement works.
static DWORD WINAPI UiaWorker(LPVOID) {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    IUIAutomation* uia = nullptr;
    IUIAutomationCacheRequest* cache = nullptr;
    IUIAutomationCondition* cond = nullptr;
    if (SUCCEEDED(CoCreateInstance(__uuidof(CUIAutomation), nullptr, CLSCTX_INPROC_SERVER, __uuidof(IUIAutomation), (void**)&uia)) && uia) {
        uia->CreateCacheRequest(&cache);
        if (cache) cache->AddProperty(UIA_BoundingRectanglePropertyId);
        VARIANT v; VariantInit(&v);
        v.vt = VT_I4; v.lVal = UIA_ButtonControlTypeId;
        uia->CreatePropertyCondition(UIA_ControlTypePropertyId, v, &cond);
    }
    HANDLE handles[2] = {g_stopEvent, g_uiaEvent};
    while (!g_stop) {
        std::vector<HWND> list;
        AcquireSRWLockShared(&g_btnLock);
        list = g_tbList;
        ReleaseSRWLockShared(&g_btnLock);
        bool allOk = !list.empty();
        for (HWND tb : list) {
            if (g_stop) break;
            if (uia && cache && cond) MeasureTaskbarButton(uia, cache, cond, tb);
            if (!GetBtn(tb).ok) allOk = false;
        }
        DWORD r = WaitForMultipleObjects(2, handles, FALSE, allOk ? 3000 : 400);
        if (r == WAIT_OBJECT_0) break;
        if (r == WAIT_OBJECT_0 + 1 && WaitForSingleObject(g_stopEvent, 150) == WAIT_OBJECT_0) break;  // debounce
    }
    if (cond) cond->Release();
    if (cache) cache->Release();
    if (uia) uia->Release();
    CoUninitialize();
    return 0;
}

// ---------------------------------------------- overlay windows (taskbar thread) ----

struct Cell { RECT r; int item; };

// A finished picture prepared by the worker thread; the taskbar thread only applies it.
struct Frame {
    HBITMAP dib = nullptr;
    int W = 0, H = 0;
    bool useAuto = false;
    std::vector<Cell> cells;
};

static void FreeFrame(Frame* f) {
    if (!f) return;
    if (f->dib) DeleteObject(f->dib);
    delete f;
}

// At most one finished picture waits for each overlay; a newer one replaces it.
struct FrameSlot {
    std::atomic<Frame*> pending{nullptr};
    ~FrameSlot() { FreeFrame(pending.exchange(nullptr)); }
};

// Lives on, and is only touched from, the thread of its taskbar window.
struct Overlay {
    std::shared_ptr<FrameSlot> slot;
    HWND taskbar = nullptr, hwnd = nullptr, tip = nullptr;
    int lastW = 0, lastH = 0;
    bool lastAuto = false, shown = false, noRoomLogged = false;
    LONG px = 0, py = 0;
    ULONGLONG hideSince = 0;
    ULONGLONG lastCoverCheck = 0;
    bool fsCovered = false;
    std::vector<Cell> cells;
    int hoverItem = -1;
    bool tracking = false;
    std::wstring tipText;
    bool tipVisible = false;
    ULONGLONG tipStart = 0;
};

// (taskbar, overlay window) pairs, so event callbacks on any thread can notify the right window
static std::mutex g_pairMutex;
static void RemoveTaskbarHook();
static std::vector<std::pair<HWND, HWND>> g_pairs;

typedef void(WINAPI* RunProc_t)(PVOID);

// Runs proc on the thread that owns hWnd, and waits for it.
static bool RunFromWindowThread(HWND hWnd, RunProc_t proc, PVOID procParam) {
    struct Param { RunProc_t proc; PVOID p; };
    DWORD tid = GetWindowThreadProcessId(hWnd, nullptr);
    if (tid == 0) return false;
    if (tid == GetCurrentThreadId()) { proc(procParam); return true; }
    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == g_runMsg) {
                    Param* param = (Param*)cwp->lParam;
                    param->proc(param->p);
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, tid);
    if (!hook) return false;
    Param param = {proc, procParam};
    SendMessageW(hWnd, g_runMsg, 0, (LPARAM)&param);
    UnhookWindowsHookEx(hook);
    return true;
}

// true when a window above the taskbar covers the whole monitor (fullscreen video, photo viewer, game ...)
static bool CoveredByFullscreen(Overlay* ov) {
    HMONITOR mon = MonitorFromWindow(ov->taskbar, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {sizeof(mi)};
    if (!GetMonitorInfoW(mon, &mi)) return false;
    int guard = 0;
    for (HWND w = GetTopWindow(nullptr); w && guard < 400; w = GetWindow(w, GW_HWNDNEXT), guard++) {
        if (w == ov->taskbar) return false;  // reached the taskbar: nothing above it covers the screen
        if (w == ov->hwnd || w == ov->tip) continue;
        if (!IsWindowVisible(w) || IsIconic(w)) continue;
        LONG_PTR ex = GetWindowLongPtrW(w, GWL_EXSTYLE);
        if (ex & WS_EX_TRANSPARENT) continue;  // click-through overlays (wallpaper tools etc.) do not count
        DWORD cloaked = 0;
        DwmGetWindowAttribute(w, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
        if (cloaked) continue;
        RECT r;
        if (!GetWindowRect(w, &r)) continue;
        if (r.left <= mi.rcMonitor.left && r.top <= mi.rcMonitor.top && r.right >= mi.rcMonitor.right && r.bottom >= mi.rcMonitor.bottom)
            return true;
    }
    return false;
}

static void UpdatePosition(Overlay* ov, bool force) {
    if (!ov->hwnd || !ov->taskbar || !ov->lastW) return;
    RECT trc;
    if (!GetWindowRect(ov->taskbar, &trc)) return;
    const Settings& s = g_s;

    ULONGLONG now = GetTickCount64();
    if (now - ov->lastCoverCheck >= 100) { ov->fsCovered = CoveredByFullscreen(ov); ov->lastCoverCheck = now; }

    float sc = GetDpiForWindow(ov->taskbar) / 96.0f;
    BtnInfo b = GetBtn(ov->taskbar);
    LONG x, y;
    bool noRoom = false;
    if (ov->lastAuto && b.ok) {
        LONG gap = (LONG)(s.gapIcons * sc);
        LONG maxX = trc.left + b.relLeft - gap - ov->lastW;  // never overlap the first icon
        if (maxX < trc.left) noRoom = true;                  // no free space left of the first icon
        LONG wantX = s.dockMargin > 0 ? trc.left + (LONG)((s.dockMargin + s.leftPad) * sc) : maxX;
        x = std::max<LONG>(trc.left, std::min(wantX, maxX));
        y = trc.top + b.relTop;
    } else {
        x = trc.left + (LONG)(s.leftOffset * sc);
        y = trc.top + (LONG)(s.topOffset * sc);
    }
    if (noRoom && !ov->noRoomLogged) { Wh_Log(L"No free space left of the first taskbar icon, monitor hidden"); ov->noRoomLogged = true; }
    if (!noRoom) ov->noRoomLogged = false;

    // hide while the taskbar is hidden, a fullscreen window covers the screen, or there is no room; debounced a
    // little so short flickers (apps minimising) do not make the monitor blink
    bool wantHide = noRoom || !IsWindowVisible(ov->taskbar) || g_fullscreen.load() || ov->fsCovered;
    if (wantHide) {
        if (!ov->hideSince) { ov->hideSince = now; SetTimer(ov->hwnd, 2, 90, nullptr); }
        if (now - ov->hideSince >= 80) {
            if (ov->shown) { ShowWindow(ov->hwnd, SW_HIDE); ov->shown = false; }
        }
        return;
    }
    ov->hideSince = 0;

    // the taskbar re-raises itself above us when apps minimise / activate
    bool covered = false;
    for (HWND w = GetWindow(ov->hwnd, GW_HWNDPREV); w; w = GetWindow(w, GW_HWNDPREV))
        if (w == ov->taskbar) { covered = true; break; }

    if (force || !ov->shown || x != ov->px || y != ov->py || covered) {
        UINT f = SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW;
        if (!covered) f |= SWP_NOZORDER;
        SetWindowPos(ov->hwnd, HWND_TOPMOST, x, y, 0, 0, f);
        ov->px = x; ov->py = y; ov->shown = true;
    }
}

static void ApplyFrame(Overlay* ov, Frame* f) {
    bool moved = f->W != ov->lastW || f->H != ov->lastH || f->useAuto != ov->lastAuto;
    HDC screen = GetDC(nullptr);
    HDC mem = CreateCompatibleDC(screen);
    HGDIOBJ old = SelectObject(mem, f->dib);
    SIZE size = {f->W, f->H};
    POINT src = {0, 0};
    BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    ov->lastW = f->W; ov->lastH = f->H; ov->lastAuto = f->useAuto;
    ov->cells = std::move(f->cells);
    if (moved) SetWindowPos(ov->hwnd, nullptr, 0, 0, f->W, f->H, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
    UpdateLayeredWindow(ov->hwnd, screen, nullptr, &size, mem, &src, 0, &bf, ULW_ALPHA);
    SelectObject(mem, old);
    DeleteDC(mem);
    ReleaseDC(nullptr, screen);
    if (moved) UpdatePosition(ov, true);
}

// ---- value formatting ----
static std::wstring Fmt(const wchar_t* f, ...) {
    wchar_t b[96];
    va_list ap; va_start(ap, f);
    _vsnwprintf(b, 95, f, ap);
    va_end(ap);
    b[95] = 0;
    return b;
}

static const wchar_t* UK() { return g_s.binary ? L"KiB" : L"KB"; }
static const wchar_t* UM() { return g_s.binary ? L"MiB" : L"MB"; }
static const wchar_t* UG() { return g_s.binary ? L"GiB" : L"GB"; }

static std::wstring FmtPct(double v) { return v < 0 ? L"--%" : Fmt(L"%.0f%%", v); }

static std::wstring FmtTemp(double c) {
    if (c <= 0) return g_s.fahrenheit ? L"--°F" : L"--°C";
    return g_s.fahrenheit ? Fmt(L"%.0f°F", c * 9.0 / 5.0 + 32.0) : Fmt(L"%.0f°C", c);
}

static std::wstring FmtSpeed(double bps) {
    if (g_s.bits) {
        double kb = bps * 8 / 1000.0;
        if (kb < 1000) return Fmt(L"%.0f Kbps", kb);
        double mb = kb / 1000.0;
        if (mb < 1000) return Fmt(L"%.1f Mbps", mb);
        return Fmt(L"%.2f Gbps", mb / 1000.0);
    }
    double kb = bps / 1024.0;
    if (kb < 1000) return Fmt(L"%.0f %s/s", kb, UK());
    double mb = kb / 1024.0;
    if (mb < 1000) return Fmt(L"%.1f %s/s", mb, UM());
    return Fmt(L"%.2f %s/s", mb / 1024.0, UG());
}

static std::wstring FmtBytes(double b) {
    double mb = b / 1048576.0;
    if (mb < 1000) return Fmt(L"%.0f %s", mb, UM());
    return Fmt(L"%.1f %s", mb / 1024.0, UG());
}

// -------------------------------------------------------------- tooltips ----

static std::wstring ProcLines(bool byCpu) {
    std::wstring s;
    AcquireSRWLockShared(&g_procLock);
    const auto& v = byCpu ? g_topCpu : g_topMem;
    for (const auto& p : v)
        s += byCpu ? Fmt(L"\r\n  %s  %.0f%%", p.name.c_str(), p.cpu)
                   : Fmt(L"\r\n  %s  %s", p.name.c_str(), FmtBytes((double)p.mem).c_str());
    ReleaseSRWLockShared(&g_procLock);
    return s;
}

static std::wstring BuildTip(int item) {
    g_procWanted.store(GetTickCount64());
    Stats st = GetStats();
    switch (item) {
        case IT_CPU: {
            std::wstring s = Fmt(L"CPU  %.0f%%", st.cpu);
            if (st.cpuTemp > 0) s += L"\r\nTemperature  " + FmtTemp(st.cpuTemp) + (st.cpuTempSrc == 2 ? L"  (Windows thermal zone, approximate)" : L"");
            else s += L"\r\nTemperature  not available (run LibreHardwareMonitor)";
            std::wstring p = ProcLines(true);
            if (!p.empty()) s += L"\r\nBusiest apps:" + p;
            return s;
        }
        case IT_GPU: {
            std::wstring s = L"GPU  " + FmtPct(st.gpu);
            if (st.gpuTemp > 0) s += L"\r\nTemperature  " + FmtTemp(st.gpuTemp);
            else if (g_s.igpuFromCpu && st.cpuTemp > 0) s += L"\r\nTemperature  " + FmtTemp(st.cpuTemp) + L"  (shared with the CPU)";
            else s += L"\r\nTemperature  not available";
            return s;
        }
        case IT_RAM: {
            std::wstring s = Fmt(L"Memory  %.1f of %.1f %s used  (%.0f%%)\r\nFree  %.1f %s", st.ramUsedGB, st.ramTotalGB, UG(), st.ram,
                                 st.ramTotalGB - st.ramUsedGB, UG());
            std::wstring p = ProcLines(false);
            if (!p.empty()) s += L"\r\nBiggest apps:" + p;
            return s;
        }
        case IT_DISK:
            return Fmt(L"Disk activity  %s\r\nRead  %s\r\nWrite  %s", FmtPct(st.disk).c_str(), FmtSpeed(st.diskRead).c_str(),
                       FmtSpeed(st.diskWrite).c_str());
        case IT_DOWN:
        case IT_UP:
            return Fmt(L"Download  %s\r\nUpload  %s\r\nToday  down %s, up %s", FmtSpeed(st.down).c_str(), FmtSpeed(st.up).c_str(),
                       FmtBytes(st.todayDown).c_str(), FmtBytes(st.todayUp).c_str());
    }
    return L"";
}

// puts the tooltip right above the overlay, using the tooltip window's real size
static void PositionTip(Overlay* ov, int item, int guessH) {
    RECT wr;
    GetWindowRect(ov->hwnd, &wr);
    int w = 300, h = guessH;
    if (ov->tipVisible) {
        RECT tr;
        if (GetWindowRect(ov->tip, &tr) && tr.bottom - tr.top > 4) { w = tr.right - tr.left; h = tr.bottom - tr.top; }
    }
    int cx = wr.left;
    for (const Cell& c : ov->cells) if (c.item == item) { cx = wr.left + c.r.left; break; }
    HMONITOR mon = MonitorFromWindow(ov->hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfoW(mon, &mi);
    int x = std::clamp<int>(cx, mi.rcMonitor.left, std::max<int>(mi.rcMonitor.left, mi.rcMonitor.right - w));
    int y = wr.top - h - 8;
    SendMessageW(ov->tip, TTM_TRACKPOSITION, 0, MAKELPARAM(x, y));
}

// refresh = true when called by the timer for an already visible tooltip
static void ShowTip(Overlay* ov, int item, bool refresh) {
    if (!ov->tip || !g_s.tips) return;
    if (refresh) {
        // not live: only the first moments are updated (so top-process data can arrive), then the text stays still
        if (!ov->tipVisible || (!g_s.tipLive && GetTickCount64() - ov->tipStart > 2500)) return;
    } else {
        ov->tipStart = GetTickCount64();
    }
    std::wstring text = BuildTip(item);
    if (refresh && text == ov->tipText) return;  // nothing changed: do not touch the tooltip at all

    TOOLINFOW ti = {sizeof(ti)};
    ti.hwnd = ov->hwnd;
    ti.uId = 1;
    ti.lpszText = (LPWSTR)text.c_str();
    SendMessageW(ov->tip, TTM_UPDATETIPTEXTW, 0, (LPARAM)&ti);
    ov->tipText = text;

    int lines = 1;
    for (wchar_t c : text) if (c == L'\n') lines++;
    int guessH = (int)(lines * g_s.fontSize * 1.5 + 18);
    PositionTip(ov, item, guessH);
    if (!ov->tipVisible) {  // activate only once; updates afterwards just change text and position, so nothing flashes
        SendMessageW(ov->tip, TTM_TRACKACTIVATE, TRUE, (LPARAM)&ti);
        ov->tipVisible = true;
    } else {
        SendMessageW(ov->tip, TTM_UPDATE, 0, 0);
    }
    PositionTip(ov, item, guessH);  // again, now that the window has its real size
}

static void HideTip(Overlay* ov) {
    if (!ov->tip) return;
    TOOLINFOW ti = {sizeof(ti)};
    ti.hwnd = ov->hwnd;
    ti.uId = 1;
    SendMessageW(ov->tip, TTM_TRACKACTIVATE, FALSE, (LPARAM)&ti);
    ov->tipVisible = false;
    ov->tipText.clear();
}

static void RunClickAction() {
    switch (g_s.clickAction) {
        case 0: ShellExecuteW(nullptr, L"open", L"taskmgr.exe", nullptr, nullptr, SW_SHOWNORMAL); break;
        case 1: ShellExecuteW(nullptr, L"open", L"resmon.exe", nullptr, nullptr, SW_SHOWNORMAL); break;
        case 2:
            if (!g_s.customCmd.empty()) {
                std::wstring cmd = g_s.customCmd;
                std::vector<wchar_t> file(cmd.begin(), cmd.end());
                file.push_back(0);
                std::wstring args = PathGetArgsW(file.data());  // split "program arguments"
                PathRemoveArgsW(file.data());
                PathUnquoteSpacesW(file.data());
                ShellExecuteW(nullptr, L"open", file.data(), args.empty() ? nullptr : args.c_str(), nullptr, SW_SHOWNORMAL);
            }
            break;
    }
}

static LRESULT CALLBACK OverlayProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    Overlay* ov = (Overlay*)GetWindowLongPtrW(h, GWLP_USERDATA);
    switch (m) {
        case WM_FRAME: {
            if (ov && ov->slot) {
                if (Frame* f = ov->slot->pending.exchange(nullptr)) {
                    ApplyFrame(ov, f);
                    FreeFrame(f);
                }
            }
            return 0;
        }
        case WM_UPDATEPOS:
            if (ov) UpdatePosition(ov, false);
            return 0;
        case WM_APPLYSTYLE: {
            LONG_PTR ex = GetWindowLongPtrW(h, GWL_EXSTYLE);
            LONG_PTR want = g_s.interactive ? (ex & ~WS_EX_TRANSPARENT) : (ex | WS_EX_TRANSPARENT);
            if (want != ex) SetWindowLongPtrW(h, GWL_EXSTYLE, want);
            return 0;
        }
        case WM_TIMER:
            if (ov) {
                if (w == 2) KillTimer(h, 2);
                UpdatePosition(ov, false);  // slow safety net next to the event hooks
                if (w == 1 && ov->hoverItem >= 0) ShowTip(ov, ov->hoverItem, true);
            }
            return 0;
        case WM_MOUSEMOVE: {
            if (!ov) break;
            if (!ov->tracking) {
                TRACKMOUSEEVENT t = {sizeof(t), TME_LEAVE, h, 0};
                TrackMouseEvent(&t);
                ov->tracking = true;
            }
            POINT pt = {(short)LOWORD(l), (short)HIWORD(l)};
            int item = -1;
            for (const Cell& c : ov->cells) if (PtInRect(&c.r, pt)) { item = c.item; break; }
            if (item != ov->hoverItem) {
                ov->hoverItem = item;
                if (item >= 0) ShowTip(ov, item, false); else HideTip(ov);
            }
            return 0;
        }
        case WM_MOUSELEAVE:
            if (ov) { ov->tracking = false; ov->hoverItem = -1; HideTip(ov); }
            return 0;
        case WM_LBUTTONUP:
            if (ov) HideTip(ov);
            RunClickAction();
            return 0;
        case WM_RBUTTONUP: {
            if (ov) HideTip(ov);
            HMENU menu = CreatePopupMenu();
            AppendMenuW(menu, MF_STRING, 1, L"Open Task Manager");
            AppendMenuW(menu, MF_STRING, 2, L"Open Resource Monitor");
            AppendMenuW(menu, MF_STRING, 3, L"Open Performance Monitor");
            POINT pt;
            GetCursorPos(&pt);
            SetForegroundWindow(h);
            int cmd = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, h, nullptr);
            PostMessageW(h, WM_NULL, 0, 0);
            DestroyMenu(menu);
            if (cmd == 1) ShellExecuteW(nullptr, L"open", L"taskmgr.exe", nullptr, nullptr, SW_SHOWNORMAL);
            else if (cmd == 2) ShellExecuteW(nullptr, L"open", L"resmon.exe", nullptr, nullptr, SW_SHOWNORMAL);
            else if (cmd == 3) ShellExecuteW(nullptr, L"open", L"perfmon.exe", nullptr, nullptr, SW_SHOWNORMAL);
            return 0;
        }
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_NCDESTROY: {  // also reached when the taskbar window dies and takes its owned overlay with it
            {
                std::lock_guard<std::mutex> lock(g_pairMutex);
                g_pairs.erase(std::remove_if(g_pairs.begin(), g_pairs.end(), [&](const auto& pr) { return pr.second == h; }), g_pairs.end());
            }
            SetWindowLongPtrW(h, GWLP_USERDATA, 0);
            if (ov) RemoveTaskbarHook();
            delete ov;
            break;
        }
    }
    return DefWindowProcW(h, m, w, l);
}

// The auto-hide slide runs inside the taskbar's own thread and does not pump posted messages meanwhile, so the overlay
// is moved from a hook that runs on that same thread right after each of the taskbar's own position changes.
static std::mutex g_hookMutex;
static std::map<DWORD, std::pair<HHOOK, int>> g_tbHooks;  // taskbar thread id -> hook, number of overlays using it

static LRESULT CALLBACK TaskbarRetProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        const CWPRETSTRUCT* c = (const CWPRETSTRUCT*)lParam;
        if (c->message == WM_WINDOWPOSCHANGED) {
            HWND ovh = nullptr;
            {
                std::lock_guard<std::mutex> lock(g_pairMutex);
                for (const auto& pr : g_pairs) if (pr.first == c->hwnd) { ovh = pr.second; break; }
            }
            if (ovh) {
                Overlay* ov = (Overlay*)GetWindowLongPtrW(ovh, GWLP_USERDATA);
                if (ov) UpdatePosition(ov, false);
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

static void AddTaskbarHook() {
    std::lock_guard<std::mutex> lock(g_hookMutex);
    auto& e = g_tbHooks[GetCurrentThreadId()];
    if (!e.first) e.first = SetWindowsHookExW(WH_CALLWNDPROCRET, TaskbarRetProc, nullptr, GetCurrentThreadId());
    e.second++;
}

static void RemoveTaskbarHook() {
    std::lock_guard<std::mutex> lock(g_hookMutex);
    auto it = g_tbHooks.find(GetCurrentThreadId());
    if (it == g_tbHooks.end()) return;
    if (--it->second.second <= 0) {
        if (it->second.first) UnhookWindowsHookEx(it->second.first);
        g_tbHooks.erase(it);
    }
}

// ---- create / destroy, run on the taskbar's thread ----
struct CreateParam { HWND taskbar; HWND result; std::shared_ptr<FrameSlot> slot; };

static void WINAPI CreateOverlayOnThread(PVOID p) {
    CreateParam* cp = (CreateParam*)p;
    cp->result = nullptr;
    DWORD ex = WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE;
    if (!g_s.interactive) ex |= WS_EX_TRANSPARENT;
    // Owned by the taskbar window on the SAME thread: the overlay then always stays above the taskbar, in every
    // z-band (e.g. while the Start menu is open), without linking two threads' input queues.
    HWND hwnd = CreateWindowExW(ex, kClass, L"", WS_POPUP, 0, 0, 10, 10, cp->taskbar, nullptr, GetModuleInstance(), nullptr);
    if (!hwnd) return;
    Overlay* ov = new Overlay();
    ov->taskbar = cp->taskbar;
    ov->hwnd = hwnd;
    ov->slot = cp->slot;
    SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)ov);

    ov->tip = CreateWindowExW(WS_EX_TOPMOST, TOOLTIPS_CLASSW, nullptr,
                              WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP | TTS_NOANIMATE | TTS_NOFADE, 0, 0, 0, 0, hwnd, nullptr,
                              GetModuleInstance(), nullptr);
    if (ov->tip) {
        TOOLINFOW ti = {sizeof(ti)};
        ti.uFlags = TTF_TRACK | TTF_ABSOLUTE;
        ti.hwnd = hwnd;
        ti.uId = 1;
        ti.lpszText = (LPWSTR)L"";
        SendMessageW(ov->tip, TTM_ADDTOOLW, 0, (LPARAM)&ti);
        SendMessageW(ov->tip, TTM_SETMAXTIPWIDTH, 0, 600);
    }
    SetTimer(hwnd, 1, 400, nullptr);
    {
        std::lock_guard<std::mutex> lock(g_pairMutex);
        g_pairs.push_back({cp->taskbar, hwnd});
    }
    AddTaskbarHook();
    cp->result = hwnd;
}

static void WINAPI DestroyOverlayOnThread(PVOID p) {
    DestroyWindow((HWND)p);  // also destroys the owned tooltip; WM_NCDESTROY frees the rest
}

// ------------------------------------------------------- worker thread ----

// W-side bookkeeping for each overlay; the Overlay object itself belongs to the taskbar thread
struct OvRef { HWND taskbar; HWND hwnd; ULONGLONG startTick; std::shared_ptr<FrameSlot> slot; std::wstring lastKey; };
static std::vector<OvRef> g_refs;  // worker thread only

static void NotifyOverlays(HWND onlyTaskbar) {
    std::lock_guard<std::mutex> lock(g_pairMutex);
    for (const auto& pr : g_pairs)
        if (!onlyTaskbar || pr.first == onlyTaskbar) PostMessageW(pr.second, WM_UPDATEPOS, 0, 0);
}

// instant reaction to taskbar movement (auto-hide animation), focus / minimise changes
static void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject, LONG, DWORD, DWORD) {
    if (event == EVENT_OBJECT_LOCATIONCHANGE) {
        if (idObject != OBJID_WINDOW) return;
        {
            std::lock_guard<std::mutex> lock(g_pairMutex);
            bool isTaskbar = false;
            for (const auto& pr : g_pairs) if (pr.first == hwnd) { PostMessageW(pr.second, WM_UPDATEPOS, 0, 0); isTaskbar = true; }
            if (!isTaskbar) return;
        }
        SetEvent(g_uiaEvent);
        return;
    }
    NotifyOverlays(nullptr);  // foreground / minimise: z-order or covering may have changed
    SetEvent(g_uiaEvent);     // ... and the taskbar icons may have moved
}

// ---- drawing (worker thread) ----
static void RoundRect(GraphicsPath& p, RectF r, float d) {
    p.AddArc(r.X, r.Y, d, d, 180, 90);
    p.AddArc(r.X + r.Width - d, r.Y, d, d, 270, 90);
    p.AddArc(r.X + r.Width - d, r.Y + r.Height - d, d, d, 0, 90);
    p.AddArc(r.X, r.Y + r.Height - d, d, d, 90, 90);
    p.CloseFigure();
}

static void IconCpu(Graphics& g, float x, float y, float s, Color c) {
    Pen pen(c, std::max(1.0f, s * 0.07f));
    float m = s * 0.22f;
    for (int i = 0; i < 3; i++) {
        float t = m + (s - 2 * m) * (i + 0.5f) / 3;
        g.DrawLine(&pen, x + t, y, x + t, y + m);
        g.DrawLine(&pen, x + t, y + s - m, x + t, y + s);
        g.DrawLine(&pen, x, y + t, x + m, y + t);
        g.DrawLine(&pen, x + s - m, y + t, x + s, y + t);
    }
    GraphicsPath p; RoundRect(p, RectF(x + m, y + m, s - 2 * m, s - 2 * m), s * 0.14f);
    g.DrawPath(&pen, &p);
    float k = s * 0.13f;
    g.DrawRectangle(&pen, x + s / 2 - k, y + s / 2 - k, 2 * k, 2 * k);
}

static void IconGpu(Graphics& g, float x, float y, float s, Color c) {
    Pen pen(c, std::max(1.0f, s * 0.07f));
    GraphicsPath p; RoundRect(p, RectF(x, y + s * 0.18f, s, s * 0.55f), s * 0.12f);
    g.DrawPath(&pen, &p);
    float cx = x + s * 0.64f, cy = y + s * 0.455f, r = s * 0.17f;
    g.DrawEllipse(&pen, cx - r, cy - r, 2 * r, 2 * r);
    g.DrawRectangle(&pen, x + s * 0.14f, y + s * 0.32f, s * 0.2f, s * 0.26f);
    g.DrawLine(&pen, x + s * 0.1f, y + s * 0.84f, x + s * 0.65f, y + s * 0.84f);
}

static void DrawRamShape(Graphics& g, float x, float y, float s, Color c) {
    Pen pen(c, std::max(1.0f, s * 0.07f));
    GraphicsPath p; RoundRect(p, RectF(x, y + s * 0.22f, s, s * 0.46f), s * 0.1f);
    g.DrawPath(&pen, &p);
    for (int i = 0; i < 3; i++)
        g.DrawRectangle(&pen, x + s * (0.14f + i * 0.28f), y + s * 0.34f, s * 0.2f, s * 0.22f);
    for (int i = 0; i < 5; i++) {
        float px = x + s * (0.1f + i * 0.2f);
        g.DrawLine(&pen, px, y + s * 0.68f, px, y + s * 0.84f);
    }
}

// the icon is a dim "background" outline; the part in use is drawn bright and filled in front of it, rising with usage
static void IconRam(Graphics& g, float x, float y, float s, Color c, double usedPct) {
    DrawRamShape(g, x, y, s, Color((BYTE)(c.GetA() * 80 / 255), c.GetR(), c.GetG(), c.GetB()));
    float ratio = (float)std::clamp(usedPct / 100.0, 0.0, 1.0);
    if (ratio < 0.02f) return;
    GraphicsState st = g.Save();
    g.SetClip(RectF(x - 2, y + s * (1.0f - ratio), s + 4, s * ratio + 2));
    SolidBrush fill(Color((BYTE)(c.GetA() * 120 / 255), c.GetR(), c.GetG(), c.GetB()));
    GraphicsPath p; RoundRect(p, RectF(x, y + s * 0.22f, s, s * 0.46f), s * 0.1f);
    g.FillPath(&fill, &p);
    DrawRamShape(g, x, y, s, c);
    g.Restore(st);
}

static void IconDisk(Graphics& g, float x, float y, float s, Color c) {
    Pen pen(c, std::max(1.0f, s * 0.07f));
    GraphicsPath p; RoundRect(p, RectF(x, y + s * 0.25f, s, s * 0.5f), s * 0.12f);
    g.DrawPath(&pen, &p);
    g.DrawLine(&pen, x + s * 0.08f, y + s * 0.55f, x + s * 0.92f, y + s * 0.55f);
    SolidBrush br(c);
    g.FillEllipse(&br, x + s * 0.72f, y + s * 0.6f, s * 0.12f, s * 0.12f);
}

static void IconArrow(Graphics& g, float x, float y, float s, Color c, bool down) {
    Pen pen(c, std::max(1.0f, s * 0.08f));
    pen.SetLineCap(LineCapRound, LineCapRound, DashCapRound);
    pen.SetLineJoin(LineJoinRound);
    float cx = x + s / 2;
    float top = y + s * 0.12f, bot = y + s * 0.72f;
    if (down) {
        g.DrawLine(&pen, cx, top, cx, bot);
        PointF h[3] = {PointF(cx - s * 0.26f, bot - s * 0.26f), PointF(cx, bot), PointF(cx + s * 0.26f, bot - s * 0.26f)};
        g.DrawLines(&pen, h, 3);
    } else {
        g.DrawLine(&pen, cx, top + s * 0.2f, cx, bot + s * 0.2f);
        PointF h[3] = {PointF(cx - s * 0.26f, top + s * 0.26f), PointF(cx, top), PointF(cx + s * 0.26f, top + s * 0.26f)};
        g.DrawLines(&pen, h, 3);
    }
    g.DrawLine(&pen, x + s * 0.15f, y + s * 0.92f, x + s * 0.85f, y + s * 0.92f);
}

// one text piece of an item: drawn at a fixed width (its template) so columns do not jitter
struct Seg { std::wstring text, tmpl; bool warn; };

static void BuildSegs(int item, const Stats& st, std::vector<Seg>& out) {
    const Settings& s = g_s;
    auto usageSeg = [&](double v) { return Seg{FmtPct(v), L"100%", v >= s.warnUsage}; };
    auto tempSeg = [&](double c) { return Seg{FmtTemp(c), s.fahrenheit ? L"100°F" : L"100°C", c >= s.warnTemp}; };
    switch (item) {
        case IT_CPU:
            out.push_back(usageSeg(g_disp.cpu));
            if (s.showCpuTemp) out.push_back(tempSeg(st.cpuTemp));
            break;
        case IT_GPU: {
            out.push_back(usageSeg(g_disp.gpu));
            if (s.showGpuTemp) {
                double t = st.gpuTemp > 0 ? st.gpuTemp : (s.igpuFromCpu && st.cpuTemp > 0 ? st.cpuTemp : -1);
                out.push_back(tempSeg(t));
            }
            break;
        }
        case IT_RAM:
            out.push_back(Seg{s.ramTotal ? Fmt(L"%.1f/%.1f %s", st.ramUsedGB, st.ramTotalGB, UG()) : Fmt(L"%.1f %s", st.ramUsedGB, UG()),
                              s.ramTotal ? Fmt(L"99.9/99.9 %s", UG()) : Fmt(L"99.9 %s", UG()), st.ram >= s.warnUsage});
            if (s.ramPct) out.push_back(usageSeg(st.ram));
            break;
        case IT_DISK:
            out.push_back(usageSeg(g_disp.disk));
            break;
        case IT_DOWN:
        case IT_UP:
            out.push_back(Seg{FmtSpeed(item == IT_DOWN ? g_disp.down : g_disp.up), s.bits ? std::wstring(L"999.9 Mbps") : Fmt(L"999.9 %s/s", UM()), false});
            if (s.netTotal) out.push_back(Seg{FmtBytes(item == IT_DOWN ? st.todayDown : st.todayUp), Fmt(L"999.9 %s", UG()), false});
            break;
    }
}

// font and measured text widths are cached per settings snapshot and monitor scale, not rebuilt every frame
struct RenderCache {
    std::unique_ptr<FontFamily> fam;
    std::unique_ptr<Font> font;
    std::unique_ptr<StringFormat> sf;
    std::unique_ptr<Bitmap> mbmp;
    std::unique_ptr<Graphics> mg;
    std::unordered_map<std::wstring, float> widths;
};
// Worker thread only. Never destroyed automatically: at process exit its GDI+ objects would be torn down under
// the loader lock after the worker was killed. The worker releases it explicitly before GdiplusShutdown.
[[clang::no_destroy]] static std::optional<std::map<UINT, std::unique_ptr<RenderCache>>> g_renderCaches;
static const Settings* g_renderCacheSettings = nullptr;

static RenderCache& GetRenderCache(UINT dpi, float fontPx) {
    if (g_renderCacheSettings != g_cfgPtr.load()) { g_renderCaches->clear(); g_renderCacheSettings = g_cfgPtr.load(); }
    auto& slot = (*g_renderCaches)[dpi];
    if (!slot) {
        slot = std::make_unique<RenderCache>();
        const Settings& s = g_s;
        slot->fam.reset(new FontFamily(s.fontFamily.c_str()));
        if (!slot->fam->IsAvailable()) slot->fam.reset(new FontFamily(L"Segoe UI Variable Text"));
        if (!slot->fam->IsAvailable()) slot->fam.reset(new FontFamily(L"Segoe UI"));
        slot->font.reset(new Font(slot->fam.get(), fontPx, s.bold ? FontStyleBold : FontStyleRegular, UnitPixel));
        slot->sf.reset(new StringFormat(StringFormat::GenericTypographic()));
        slot->sf->SetFormatFlags(slot->sf->GetFormatFlags() | StringFormatFlagsMeasureTrailingSpaces | StringFormatFlagsNoWrap);
        slot->sf->SetLineAlignment(StringAlignmentCenter);
        slot->mbmp.reset(new Bitmap(1, 1, PixelFormat32bppPARGB));
        slot->mg.reset(new Graphics(slot->mbmp.get()));
    }
    return *slot;
}

static void RenderOverlay(OvRef& ref) {
    const Settings& s = g_s;
    RECT trc;
    if (!GetWindowRect(ref.taskbar, &trc)) return;

    // wait (hidden) for the first measurement instead of flashing at a wrong spot
    BtnInfo binfo = GetBtn(ref.taskbar);
    bool useAuto = s.autoPos && binfo.ok && binfo.h >= 24;
    if (s.autoPos && !useAuto && GetTickCount64() - ref.startTick < 4000) return;

    UINT dpi = GetDpiForWindow(ref.taskbar);
    float sc = dpi / 96.0f;
    int H = s.height > 0 ? (int)(s.height * sc) : (trc.bottom - trc.top);
    if (useAuto) H = binfo.h;
    float fs = s.fontSize * sc;
    float icon = fs * s.iconScale / 100.0f;
    float gap = s.itemGap * sc, colGap = s.colGap * sc, pad = 4 * sc;

    BYTE alpha = (BYTE)(255 * s.opacity / 100);
    Color txt = s.hasColor ? Color(alpha, s.tr, s.tg, s.tb)
                           : (g_lightTheme.load() ? Color(alpha, 30, 30, 30) : Color(alpha, 240, 240, 240));
    Color warnCol(alpha, s.wr, s.wg, s.wb);

    RenderCache& rc = GetRenderCache(dpi, fs);
    auto tw = [&](const std::wstring& str) {
        auto it = rc.widths.find(str);
        if (it != rc.widths.end()) return it->second;
        RectF r;
        rc.mg->MeasureString(str.c_str(), -1, rc.font.get(), PointF(0, 0), rc.sf.get(), &r);
        rc.widths[str] = r.Width;
        return r.Width;
    };

    Stats st = GetStats();

    // layout: per column its items and segments
    struct ColLayout { std::vector<int> items; std::vector<std::vector<Seg>> segs; float w = 0; };
    std::vector<ColLayout> cols;
    int rows = 1;
    for (const auto& c : s.cols) {
        ColLayout cl;
        cl.items = c;
        rows = std::max<int>(rows, (int)c.size());
        for (int it : c) {
            std::vector<Seg> segs;
            if (it != IT_NONE) BuildSegs(it, st, segs);
            float w = s.icons ? icon + gap : 0;
            for (size_t i = 0; i < segs.size(); i++) w += tw(segs[i].tmpl) + (i ? gap : 0);
            cl.w = std::max(cl.w, w);
            cl.segs.push_back(std::move(segs));
        }
        cols.push_back(std::move(cl));
    }
    float totalW = pad * 2;
    for (size_t i = 0; i < cols.size(); i++) totalW += cols[i].w + (i ? colGap : 0);
    int W = std::max(1, (int)std::ceil(totalW));
    float rowH = (float)H / rows;

    // nothing visible changed since the last picture of this overlay: do not draw or post anything
    {
        std::wstring key = Fmt(L"%p|%d|%d|%d|%d|", (const void*)&s, W, H, (int)useAuto, (int)(g_disp.ram * icon / 100.0));
        for (auto& cl : cols)
            for (auto& sv : cl.segs)
                for (auto& sg : sv) key += sg.text + (sg.warn ? L"!" : L"") + L"\x1f";
        if (s.spark) key += Fmt(L"h%d", g_histPos);
        if (key == ref.lastKey) return;
        ref.lastKey = std::move(key);
    }

    // draw into a DIB so we can use per-pixel alpha; the finished bitmap is handed to the taskbar thread
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = W;
    bi.bmiHeader.biHeight = -H;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP dib = CreateDIBSection(nullptr, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!dib || !bits) { if (dib) DeleteObject(dib); return; }

    auto frame = std::make_unique<Frame>();
    {
        Bitmap bmp(W, H, W * 4, PixelFormat32bppPARGB, (BYTE*)bits);
        Graphics g(&bmp);
        // an almost invisible fill keeps the whole area clickable / hoverable
        g.Clear(s.interactive ? Color(2, 0, 0, 0) : Color(0, 0, 0, 0));
        g.SetSmoothingMode(SmoothingModeAntiAlias);
        g.SetTextRenderingHint(TextRenderingHintAntiAlias);

        float x = pad;
        for (auto& cl : cols) {
            int nItems = 0;
            for (int it : cl.items) if (it != IT_NONE) nItems++;
            for (size_t r = 0; r < cl.items.size(); r++) {
                int item = cl.items[r];
                if (item == IT_NONE) continue;
                float rowY = (nItems == 1 && rows > 1) ? (H - rowH) / 2 : r * rowH;
                frame->cells.push_back({{(LONG)x, (LONG)rowY, (LONG)(x + cl.w), (LONG)(rowY + rowH)}, item});

                // sparkline behind the item
                if (s.spark) {
                    double hist[60];
                    CopyHistory(item, hist);
                    double mx = (item == IT_DOWN || item == IT_UP) ? 100 * 1024.0 : 100.0;
                    for (double v : hist) mx = std::max(mx, v);
                    PointF pts[62];
                    float bottom = rowY + rowH * 0.92f, hgt = rowH * 0.8f;
                    for (int i = 0; i < 60; i++) pts[i + 1] = PointF(x + cl.w * i / 59.0f, bottom - (float)(hist[i] / mx) * hgt);
                    pts[0] = PointF(x, bottom);
                    pts[61] = PointF(x + cl.w, bottom);
                    BYTE sa = (BYTE)(alpha * s.sparkOp / 100);
                    SolidBrush fill(Color((BYTE)(sa * 0.45f), txt.GetR(), txt.GetG(), txt.GetB()));
                    g.FillPolygon(&fill, pts, 62);
                    Pen line(Color(sa, txt.GetR(), txt.GetG(), txt.GetB()), std::max(1.0f, sc * 0.9f));
                    g.DrawLines(&line, pts + 1, 60);
                }

                float tx = x;
                if (s.icons) {
                    float iy = rowY + (rowH - icon) / 2;
                    switch (item) {
                        case IT_CPU: IconCpu(g, x, iy, icon, txt); break;
                        case IT_GPU: IconGpu(g, x, iy, icon, txt); break;
                        case IT_RAM: IconRam(g, x, iy, icon, txt, g_disp.ram); break;
                        case IT_DISK: IconDisk(g, x, iy, icon, txt); break;
                        case IT_DOWN: IconArrow(g, x, iy, icon, txt, true); break;
                        case IT_UP: IconArrow(g, x, iy, icon, txt, false); break;
                    }
                    tx += icon + gap;
                }
                const auto& segs = cl.segs[r];
                for (size_t i = 0; i < segs.size(); i++) {
                    SolidBrush b(s.warn && segs[i].warn ? warnCol : txt);
                    g.DrawString(segs[i].text.c_str(), -1, rc.font.get(), RectF(tx, rowY, 400 * sc, rowH), rc.sf.get(), &b);
                    tx += tw(segs[i].tmpl) + gap;
                }
            }
            x += cl.w + colGap;
        }
    }

    frame->dib = dib;
    frame->W = W;
    frame->H = H;
    frame->useAuto = useAuto;
    // one pending frame per overlay: a newer one replaces an older one that was not applied yet
    Frame* raw = frame.release();
    if (Frame* old = ref.slot->pending.exchange(raw)) FreeFrame(old);  // a WM_FRAME is already queued
    else if (!PostMessageW(ref.hwnd, WM_FRAME, 0, 0)) FreeFrame(ref.slot->pending.exchange(nullptr));  // window is gone
}

static void RenderAll() {
    for (OvRef& ref : g_refs) RenderOverlay(ref);
}

// ---- keeping the overlay list in sync with the taskbars / monitors ----
static BOOL CALLBACK EnumTaskbars(HWND h, LPARAM lp) {
    wchar_t cls[64];
    if (!GetClassNameW(h, cls, 64)) return TRUE;
    bool prim = wcscmp(cls, L"Shell_TrayWnd") == 0, sec = wcscmp(cls, L"Shell_SecondaryTrayWnd") == 0;
    if (!prim && !sec) return TRUE;
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    if (pid != GetCurrentProcessId()) return TRUE;
    if (sec && !g_s.allMonitors) return TRUE;
    ((std::vector<HWND>*)lp)->push_back(h);
    return TRUE;
}

static bool LightTheme() {
    DWORD v = 0, sz = sizeof(v);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &v, &sz) == ERROR_SUCCESS)
        return v != 0;
    return false;
}

static void SyncOverlays() {
    std::vector<HWND> found;
    EnumWindows(EnumTaskbars, (LPARAM)&found);

    AcquireSRWLockExclusive(&g_btnLock);
    bool changed = g_tbList != found;
    g_tbList = found;
    ReleaseSRWLockExclusive(&g_btnLock);
    if (changed) SetEvent(g_uiaEvent);

    for (size_t i = 0; i < g_refs.size();) {
        if (std::find(found.begin(), found.end(), g_refs[i].taskbar) == found.end() || !IsWindow(g_refs[i].taskbar)) {
            RunFromWindowThread(g_refs[i].taskbar, DestroyOverlayOnThread, g_refs[i].hwnd);
            g_refs.erase(g_refs.begin() + i);
        } else i++;
    }
    for (HWND tb : found) {
        bool have = false;
        for (const OvRef& r : g_refs) if (r.taskbar == tb) have = true;
        if (have) continue;
        CreateParam cp = {tb, nullptr, std::make_shared<FrameSlot>()};
        if (RunFromWindowThread(tb, CreateOverlayOnThread, &cp) && cp.result)
            g_refs.push_back({tb, cp.result, GetTickCount64(), cp.slot, L""});
    }

    // hide in full-screen apps / games, but only if that state lasts a few seconds
    static int fsCount = 0;
    QUERY_USER_NOTIFICATION_STATE qs = QUNS_ACCEPTS_NOTIFICATIONS;
    SHQueryUserNotificationState(&qs);
    bool fsRaw = (qs == QUNS_BUSY || qs == QUNS_RUNNING_D3D_FULL_SCREEN || qs == QUNS_PRESENTATION_MODE);
    fsCount = fsRaw ? fsCount + 1 : 0;
    g_fullscreen = fsCount >= 3;
    g_lightTheme = LightTheme();
}

static bool EaseDisp() {
    Stats st = GetStats();
    double s = g_s.smoothing / 100.0;
    bool moving = false;
    auto ease = [&](double& d, double target) {
        if (s <= 0.0 || d < 0 || target < 0) { d = target; return; }
        double nd = d + (target - d) * (1.0 - s) * 0.35;
        if (std::fabs(target - nd) < std::max(0.05, std::fabs(target) * 0.004)) nd = target;
        if (nd != target) moving = true;
        d = nd;
    };
    ease(g_disp.cpu, st.cpu);
    ease(g_disp.gpu, st.gpu);
    ease(g_disp.ram, st.ram);
    ease(g_disp.disk, st.disk);
    ease(g_disp.down, st.down);
    ease(g_disp.up, st.up);
    return moving;
}

static LRESULT CALLBACK MsgProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    switch (m) {
        case WM_TIMER:
            if (w == 1) {  // a new sample is due
                if (g_s.smoothing > 0) {
                    SetTimer(h, 3, 33, nullptr);
                } else {
                    EaseDisp();
                    RenderAll();
                }
                // keep a visible tooltip fresh: its owner thread does the work
                NotifyOverlays(nullptr);
            } else if (w == 3) {  // easing animation, stops by itself once the values settled
                bool moving = EaseDisp();
                RenderAll();
                if (!moving) KillTimer(h, 3);
            } else if (w == 7) {
                // Cheap watcher for the taskbar rectangle (GetWindowRect only, nothing is drawn). The auto-hide slide
                // does not raise enough window events, so it polls slowly while idle and fast while the taskbar moves.
                static std::map<HWND, RECT> last;
                static ULONGLONG calmSince = 0;
                static bool fast = false;
                std::vector<std::pair<HWND, HWND>> pairs;
                {
                    std::lock_guard<std::mutex> lock(g_pairMutex);
                    pairs = g_pairs;
                }
                bool changed = false;
                for (const auto& pr : pairs) {
                    RECT r;
                    if (!GetWindowRect(pr.first, &r)) continue;
                    auto it = last.find(pr.first);
                    bool had = it != last.end();
                    if (!had || !EqualRect(&it->second, &r)) {
                        last[pr.first] = r;
                        if (had) { changed = true; PostMessageW(pr.second, WM_UPDATEPOS, 0, 0); }
                    }
                }
                ULONGLONG now = GetTickCount64();
                if (changed) {
                    calmSince = now;
                    if (!fast) { fast = true; SetTimer(h, 7, 8, nullptr); }
                } else if (fast && now - calmSince > 600) {
                    fast = false;
                    SetTimer(h, 7, 40, nullptr);
                    SetEvent(g_uiaEvent);  // the taskbar settled: re-measure the first icon
                }
            } else if (w == 4) {
                SyncOverlays();
            } else if (w == 6) {  // overlays that have not got their first picture yet
                static ULONGLONG t0 = GetTickCount64();
                if (GetTickCount64() - t0 > 6000) KillTimer(h, 6);
                for (OvRef& ref : g_refs) RenderOverlay(ref);
            }
            return 0;
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;
        case WM_SETTINGS: {
            SetTimer(h, 1, g_s.refreshMs, nullptr);
            SyncOverlays();
            for (const OvRef& ref : g_refs) PostMessageW(ref.hwnd, WM_APPLYSTYLE, 0, 0);
            SetEvent(g_uiaEvent);
            EaseDisp();
            RenderAll();
            return 0;
        }
    }
    return DefWindowProcW(h, m, w, l);
}

static DWORD WINAPI Worker(LPVOID) {
    // only the explorer.exe process that owns the taskbar draws the overlay; others stop right away
    bool isShell = false;
    for (;;) {
        if (g_stop) break;
        HWND tb = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (tb) {
            DWORD pid = 0;
            GetWindowThreadProcessId(tb, &pid);
            isShell = pid == GetCurrentProcessId();
            break;  // the taskbar exists: it either belongs to this process or this is a folder-window process
        }
        if (WaitForSingleObject(g_stopEvent, 500) == WAIT_OBJECT_0) break;  // shell still starting up
    }
    if (!isShell || g_stop) return 0;

    GdiplusStartupInput gin;
    ULONG_PTR token = 0;
    GdiplusStartup(&token, &gin, nullptr);
    g_renderCaches.emplace();
    INITCOMMONCONTROLSEX icc = {sizeof(icc), ICC_WIN95_CLASSES};
    InitCommonControlsEx(&icc);
    g_runMsg = RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    HINSTANCE inst = GetModuleInstance();
    WNDCLASSW wc = {};
    wc.lpfnWndProc = OverlayProc;
    wc.hInstance = inst;
    wc.lpszClassName = kClass;
    wc.hCursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512));
    bool overlayClass = RegisterClassW(&wc) != 0;
    WNDCLASSW mc = {};
    mc.lpfnWndProc = MsgProc;
    mc.hInstance = inst;
    mc.lpszClassName = kMsgClass;
    bool msgClass = RegisterClassW(&mc) != 0;

    HWND msgWnd = nullptr;
    if (overlayClass && msgClass) msgWnd = CreateWindowExW(0, kMsgClass, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, inst, nullptr);
    else Wh_Log(L"Window class registration failed");

    if (msgWnd) {
        g_msgWnd.store(msgWnd);
        if (g_stop) PostQuitMessage(0);  // Wh_ModUninit ran before the window existed

        LoadNetToday();
        g_lightTheme = LightTheme();
        g_samplerThread = CreateThread(nullptr, 0, SamplerWorker, nullptr, 0, nullptr);
        g_uiaThread = CreateThread(nullptr, 0, UiaWorker, nullptr, 0, nullptr);
        SyncOverlays();

        // event hooks (worker thread's message loop delivers them): taskbar movement, focus and minimise changes
        HWINEVENTHOOK hooks[3] = {
            SetWinEventHook(EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE, nullptr, WinEventProc, GetCurrentProcessId(), 0,
                            WINEVENT_OUTOFCONTEXT),
            SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT),
            SetWinEventHook(EVENT_SYSTEM_MINIMIZESTART, EVENT_SYSTEM_MINIMIZEEND, nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT),
        };

        SetTimer(msgWnd, 1, g_s.refreshMs, nullptr);
        SetTimer(msgWnd, 4, 1000, nullptr);
        SetTimer(msgWnd, 6, 250, nullptr);
        SetTimer(msgWnd, 7, 40, nullptr);

        MSG msg;
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        g_stop = true;
        SetEvent(g_stopEvent);
        for (HWINEVENTHOOK hk : hooks) if (hk) UnhookWinEvent(hk);
        KillTimer(msgWnd, 1); KillTimer(msgWnd, 3); KillTimer(msgWnd, 4); KillTimer(msgWnd, 6); KillTimer(msgWnd, 7);
        // the overlay windows live on the taskbar's thread: destroy them there
        for (const OvRef& ref : g_refs) RunFromWindowThread(ref.taskbar, DestroyOverlayOnThread, ref.hwnd);
        g_refs.clear();
        // no timeout: a thread still running inside the mod after it is unloaded would crash Explorer
        if (g_samplerThread) { WaitForSingleObject(g_samplerThread, INFINITE); CloseHandle(g_samplerThread); g_samplerThread = nullptr; }
        if (g_uiaThread) { WaitForSingleObject(g_uiaThread, INFINITE); CloseHandle(g_uiaThread); g_uiaThread = nullptr; }
        g_msgWnd.store(nullptr);
        DestroyWindow(msgWnd);
    }
    if (overlayClass) UnregisterClassW(kClass, inst);
    if (msgClass) UnregisterClassW(kMsgClass, inst);

    g_renderCaches.reset();  // GDI+ objects must be gone before GdiplusShutdown
    GdiplusShutdown(token);
    return 0;
}

// -------------------------------------------------------------- windhawk ---

BOOL Wh_ModInit() {
    LoadSettings();
    g_stop = false;
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_uiaEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_stopEvent || !g_uiaEvent) return FALSE;
    g_thread = CreateThread(nullptr, 0, Worker, nullptr, 0, nullptr);
    return g_thread != nullptr;
}

void Wh_ModSettingsChanged() {
    LoadSettings();  // thread-safe; the worker applies the change
    if (HWND wnd = g_msgWnd.load()) PostMessageW(wnd, WM_SETTINGS, 0, 0);
}

void Wh_ModUninit() {
    g_stop = true;
    if (g_stopEvent) SetEvent(g_stopEvent);
    if (HWND wnd = g_msgWnd.load()) PostMessageW(wnd, WM_CLOSE, 0, 0);
    if (g_thread) {
        WaitForSingleObject(g_thread, INFINITE);  // no timeout: see the note above
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_stopEvent) { CloseHandle(g_stopEvent); g_stopEvent = nullptr; }
    if (g_uiaEvent) { CloseHandle(g_uiaEvent); g_uiaEvent = nullptr; }
    std::lock_guard<std::mutex> lock(g_cfgMutex);
    g_cfgAll.clear();
    g_cfgPtr.store(nullptr);
}
