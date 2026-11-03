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
// @compilerOptions -lgdiplus -liphlpapi -lpdh -lole32 -loleaut32 -lwbemuuid -lshell32 -lgdi32 -lcomctl32 -lpsapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar System Monitor

Live CPU, GPU, RAM, disk and network stats drawn **inside the Windows 11 taskbar**, to the left of the first
icon, with clean outline icons. Works with Taskbar Styler docks and on every monitor.

## Features
- **Fully configurable layout** - choose what is shown and where (see below).
- **Hover** a value for details: busiest apps, memory free, disk read/write, data used today.
- **Click** opens Task Manager (configurable); **right-click** shows a small menu.
- Smooth value animation, optional 60-second sparklines, optional warning colors (off by default).
- RAM icon fills up with usage. KiB/MiB/GiB or KB/MB/GB, bytes or bits for network.
- Follows the taskbar (auto-hide, Start menu, minimising) and stays above it.
- Every monitor's taskbar gets its own monitor.

## Layout string
Columns are separated by `|`, rows inside a column by `,`. Items: `cpu`, `gpu`, `ram`, `disk`, `down`, `up`
and `-` for an empty slot (up to 3 rows).

| Layout | Result |
|---|---|
| `cpu,gpu\|ram,disk\|down,up` | default, three columns, two rows |
| `cpu\|ram\|down` | a single row |
| `cpu,ram,disk\|down,up` | three rows |

## About temperatures
Windows does not give normal programs real CPU or most GPU temperatures.
- **NVIDIA GPUs** work out of the box.
- For accurate **CPU / AMD / Intel GPU** temperatures run
  [LibreHardwareMonitor](https://github.com/LibreHardwareMonitor/LibreHardwareMonitor) (as administrator,
  it can stay minimised in the tray). The mod reads its WMI data automatically.
- Without it the CPU value comes from Windows' ACPI thermal zone, which is only approximate and missing on some
  PCs. An integrated GPU can show the CPU/APU temperature (they share one die) - this can be turned off.

## Notes
- Position is found automatically by measuring the first taskbar icon. If you use a Taskbar Styler dock, set
  *dock margin* to its margin so the monitor stays put when icons are added.
- The mod only needs to run in `explorer.exe`. It draws its own windows and does not hook any functions.
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
  $name: Integrated GPU - show the CPU/APU temperature
  $description: An integrated Radeon shares one die with the CPU, and Windows can't read its own sensor, so the CPU temperature is shown for it.
- tempUnit: celsius
  $name: Temperature unit
  $options:
  - celsius: Celsius
  - fahrenheit: Fahrenheit
- ramShowTotal: false
  $name: RAM - show total (14.6/15.2 GB instead of 14.6 GB)
- ramShowPercent: false
  $name: RAM - also show percentage
- netUnit: bytes
  $name: Network unit
  $options:
  - bytes: KB/s, MB/s
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
  $name: Custom command for left click
- autoPosition: true
  $name: Auto-position next to the first taskbar icon
- dockMargin: 200
  $name: Taskbar Styler dock margin (px at 100% scale, 0 = hug the first icon)
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

#define NOMINMAX
#include <windows.h>
#include <algorithm>
using std::min;
using std::max;
#include <gdiplus.h>
#include <iphlpapi.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <wbemidl.h>
#include <shellapi.h>
#include <uiautomation.h>
#include <commctrl.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <cmath>
#include <cstdarg>
#include <cwchar>
#include <map>
#include <memory>
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
    int dockMargin = 200, leftPad = 16, gapIcons = 16, colGap = 20, itemGap = 5;
    int leftOffset = 262, topOffset = 8, height = 56;
};
static Settings g_s;

static std::wstring GetStr(const wchar_t* name) {
    PCWSTR v = Wh_GetStringSetting(name);
    std::wstring r = v ? v : L"";
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

static int ItemFromName(std::wstring s) {
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

static void ParseLayout(const std::wstring& layout) {
    g_s.cols.clear();
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
        if (any) g_s.cols.push_back(items);
        if (bar == std::wstring::npos) break;
        pos = bar + 1;
    }
    if (g_s.cols.empty()) g_s.cols = {{IT_CPU, IT_GPU}, {IT_RAM, IT_DISK}, {IT_DOWN, IT_UP}};
}

static void LoadSettings() {
    ParseLayout(GetStr(L"layout"));
    g_s.allMonitors = Wh_GetIntSetting(L"allMonitors") != 0;
    g_s.refreshMs = std::clamp(Wh_GetIntSetting(L"refreshMs"), 500, 5000);
    g_s.smoothing = std::clamp(Wh_GetIntSetting(L"smoothing"), 0, 90);
    g_s.fontFamily = GetStr(L"fontFamily");
    if (g_s.fontFamily.empty()) g_s.fontFamily = L"Segoe UI Variable Text";
    g_s.fontSize = std::clamp(Wh_GetIntSetting(L"fontSize"), 8, 30);
    g_s.bold = Wh_GetIntSetting(L"fontBold") != 0;
    g_s.hasColor = ParseHex(GetStr(L"textColor"), &g_s.tr, &g_s.tg, &g_s.tb);
    g_s.opacity = std::clamp(Wh_GetIntSetting(L"opacity"), 10, 100);
    g_s.icons = Wh_GetIntSetting(L"showIcons") != 0;
    g_s.iconScale = std::clamp(Wh_GetIntSetting(L"iconScale"), 60, 220);
    g_s.warn = Wh_GetIntSetting(L"colorWarnings") != 0;
    g_s.warnUsage = Wh_GetIntSetting(L"warnUsage");
    g_s.warnTemp = Wh_GetIntSetting(L"warnTemp");
    if (!ParseHex(GetStr(L"warnColor"), &g_s.wr, &g_s.wg, &g_s.wb)) { g_s.wr = 255; g_s.wg = 90; g_s.wb = 90; }
    g_s.showCpuTemp = Wh_GetIntSetting(L"showCpuTemp") != 0;
    g_s.showGpuTemp = Wh_GetIntSetting(L"showGpuTemp") != 0;
    g_s.igpuFromCpu = Wh_GetIntSetting(L"igpuTempFromCpu") != 0;
    g_s.fahrenheit = GetStr(L"tempUnit") == L"fahrenheit";
    g_s.ramTotal = Wh_GetIntSetting(L"ramShowTotal") != 0;
    g_s.ramPct = Wh_GetIntSetting(L"ramShowPercent") != 0;
    g_s.bits = GetStr(L"netUnit") == L"bits";
    g_s.netTotal = Wh_GetIntSetting(L"netInlineTotal") != 0;
    g_s.spark = Wh_GetIntSetting(L"sparklines") != 0;
    g_s.sparkOp = std::clamp(Wh_GetIntSetting(L"sparkOpacity"), 5, 100);
    g_s.tips = Wh_GetIntSetting(L"tooltips") != 0;
    g_s.tipLive = Wh_GetIntSetting(L"tooltipLive") != 0;
    g_s.binary = Wh_GetIntSetting(L"binaryUnits") != 0;
    g_s.interactive = Wh_GetIntSetting(L"interactive") != 0;
    std::wstring ca = GetStr(L"clickAction");
    g_s.clickAction = ca == L"resmon" ? 1 : ca == L"custom" ? 2 : ca == L"none" ? 3 : 0;
    g_s.customCmd = GetStr(L"customCommand");
    g_s.autoPos = Wh_GetIntSetting(L"autoPosition") != 0;
    g_s.dockMargin = Wh_GetIntSetting(L"dockMargin");
    g_s.leftPad = Wh_GetIntSetting(L"leftPadding");
    g_s.gapIcons = Wh_GetIntSetting(L"gapToIcons");
    g_s.colGap = Wh_GetIntSetting(L"columnGap");
    g_s.itemGap = Wh_GetIntSetting(L"itemGap");
    g_s.leftOffset = Wh_GetIntSetting(L"leftOffset");
    g_s.topOffset = Wh_GetIntSetting(L"topOffset");
    g_s.height = Wh_GetIntSetting(L"height");
}

// ---------------------------------------------------------------- state ----

struct Stats {
    double cpu = 0, ram = 0, ramUsedGB = 0, ramTotalGB = 0;
    double gpu = -1, cpuTemp = -1, gpuTemp = -1, disk = -1;
    double down = 0, up = 0, diskRead = 0, diskWrite = 0;
    double todayDown = 0, todayUp = 0;
};
static Stats g_st;
static int g_cpuTempSrc = 0;  // 0 none, 1 LibreHardwareMonitor, 2 Windows thermal zone
static int g_gpuTempSrc = 0;  // 0 none, 1 sensor

struct Disp { double cpu = 0, gpu = -1, ram = 0, disk = -1, down = 0, up = 0; };
static Disp g_disp;

static volatile bool g_stop = false;
static HANDLE g_thread = nullptr;
static HWND g_msgWnd = nullptr;
static bool g_fullscreen = false;
static bool g_lightTheme = false;
static const wchar_t* kClass = L"WhTaskbarSysMon";
static const wchar_t* kMsgClass = L"WhTaskbarSysMonMsg";
static const UINT WM_SETTINGS = WM_APP + 1;

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

static void SampleNet() {
    static std::map<DWORD, std::pair<DWORD, DWORD>> prev;  // per interface, 32-bit counters wrap
    static ULONGLONG pTick = 0;
    ULONG size = 0;
    GetIfTable(nullptr, &size, FALSE);
    if (!size) return;
    std::vector<BYTE> buf(size);
    auto* t = (PMIB_IFTABLE)buf.data();
    if (GetIfTable(t, &size, FALSE) != NO_ERROR) return;
    ULONGLONG now = GetTickCount64();
    double dIn = 0, dOut = 0;
    for (DWORD i = 0; i < t->dwNumEntries; i++) {
        const MIB_IFROW& r = t->table[i];
        if (r.dwType != IF_TYPE_ETHERNET_CSMACD && r.dwType != IF_TYPE_IEEE80211) continue;
        if (r.dwOperStatus != IF_OPER_STATUS_OPERATIONAL && r.dwOperStatus != IF_OPER_STATUS_CONNECTED) continue;
        std::string d((const char*)r.bDescr);
        if (d.find("Virtual") != std::string::npos || d.find("VMware") != std::string::npos ||
            d.find("TAP") != std::string::npos || d.find("VPN") != std::string::npos ||
            d.find("Hyper-V") != std::string::npos)
            continue;  // avoid double counting
        auto it = prev.find(r.dwIndex);
        if (it != prev.end() && pTick) {
            dIn += (DWORD)(r.dwInOctets - it->second.first);
            dOut += (DWORD)(r.dwOutOctets - it->second.second);
        }
        prev[r.dwIndex] = {r.dwInOctets, r.dwOutOctets};
    }
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

// ---- GPU usage via PDH (any vendor) ----
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
    std::map<std::wstring, double> perType;  // sum per engine type, like Task Manager
    for (DWORD i = 0; i < count; i++) {
        if (items[i].FmtValue.CStatus != PDH_CSTATUS_VALID_DATA && items[i].FmtValue.CStatus != PDH_CSTATUS_NEW_DATA) continue;
        const wchar_t* e = wcsstr(items[i].szName, L"engtype_");
        perType[e ? e + 8 : L"?"] += items[i].FmtValue.doubleValue;
    }
    double best = 0;
    for (auto& kv : perType) best = std::max(best, kv.second);
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
    if (best > 0) { g_st.cpuTemp = best; g_cpuTempSrc = 2; }
}

// ---- NVIDIA via NVML (usage + temperature) ----
typedef int (*PFN_Init)();
typedef int (*PFN_GetHandle)(unsigned, void**);
typedef int (*PFN_GetTemp)(void*, int, unsigned*);
struct NvmlUtil { unsigned gpu, mem; };
typedef int (*PFN_GetUtil)(void*, NvmlUtil*);
static PFN_GetTemp g_nvTemp = nullptr;
static PFN_GetUtil g_nvUtil = nullptr;
static void* g_nvDev = nullptr;
static bool g_nvTried = false;

static void InitNvml() {
    g_nvTried = true;
    HMODULE h = LoadLibraryW(L"nvml.dll");
    if (!h) h = LoadLibraryW(L"C:\\Program Files\\NVIDIA Corporation\\NVSMI\\nvml.dll");
    if (!h) return;
    auto init = (PFN_Init)GetProcAddress(h, "nvmlInit_v2");
    auto get = (PFN_GetHandle)GetProcAddress(h, "nvmlDeviceGetHandleByIndex_v2");
    auto temp = (PFN_GetTemp)GetProcAddress(h, "nvmlDeviceGetTemperature");
    auto util = (PFN_GetUtil)GetProcAddress(h, "nvmlDeviceGetUtilizationRates");
    if (!init || !get || !temp || !util) return;
    if (init() != 0 || get(0, &g_nvDev) != 0) return;
    g_nvTemp = temp; g_nvUtil = util;
}

static bool SampleNvml() {
    if (!g_nvTried) InitNvml();
    if (!g_nvDev) return false;
    unsigned t = 0; NvmlUtil u = {};
    if (g_nvTemp(g_nvDev, 0, &t) == 0) { g_st.gpuTemp = t; g_gpuTempSrc = 1; }
    if (g_nvUtil(g_nvDev, &u) == 0) g_st.gpu = u.gpu;
    return true;
}

// ---- WMI: LibreHardwareMonitor (CPU temp, non-NVIDIA GPU temp) ----
static IWbemServices* g_lhm = nullptr;
static IWbemServices* g_acpi = nullptr;
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

static bool WmiQuery(IWbemServices* svc, const wchar_t* q, IEnumWbemClassObject** en) {
    BSTR lang = SysAllocString(L"WQL"), query = SysAllocString(q);
    HRESULT hr = svc->ExecQuery(lang, query, WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, nullptr, en);
    SysFreeString(lang); SysFreeString(query);
    return SUCCEEDED(hr);
}

static void SampleWmi() {
    ULONGLONG now = GetTickCount64();
    if (!g_lhm && !g_acpi && now - g_lastConnect > 30000) {
        g_lastConnect = now;
        if (!WmiConnect(L"ROOT\\LibreHardwareMonitor", &g_lhm)) WmiConnect(L"ROOT\\WMI", &g_acpi);
    }
    if (g_lhm) {
        IEnumWbemClassObject* en = nullptr;
        if (!WmiQuery(g_lhm, L"SELECT Name,Identifier,Value FROM Sensor WHERE SensorType='Temperature'", &en)) {
            g_lhm->Release(); g_lhm = nullptr; return;
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
        if (cpu > 0) { g_st.cpuTemp = cpu; g_cpuTempSrc = 1; }
        if (gpu > 0 && !g_nvTemp) { g_st.gpuTemp = gpu; g_gpuTempSrc = 1; }
    } else if (g_acpi) {  // usually needs admin; best effort
        IEnumWbemClassObject* en = nullptr;
        if (!WmiQuery(g_acpi, L"SELECT CurrentTemperature FROM MSAcpi_ThermalZoneTemperature", &en)) {
            g_acpi->Release(); g_acpi = nullptr; return;
        }
        IWbemClassObject* o = nullptr; ULONG n = 0;
        if (en->Next(2000, 1, &o, &n) == S_OK && n) {
            VARIANT v; VariantInit(&v);
            o->Get(L"CurrentTemperature", 0, &v, nullptr, nullptr);
            double c = VarToDouble(v) / 10.0 - 273.15;
            if (c > 10 && c < 120) { g_st.cpuTemp = c; g_cpuTempSrc = 2; }
            VariantClear(&v);
            o->Release();
        }
        en->Release();
    }
}

// ---- top processes (only sampled while a tooltip is being hovered) ----
struct ProcAgg { std::wstring name; double cpu = 0; ULONGLONG mem = 0; };
static std::vector<ProcAgg> g_topCpu, g_topMem;
static SRWLOCK g_procLock = SRWLOCK_INIT;
static volatile ULONGLONG g_procWanted = 0;

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

static void SampleAll() {
    static int tick = 0;
    SampleCpuRam();
    SampleNet();
    if (!SampleNvml()) SampleGpuPdh();
    SampleDisk();
    if (tick++ % 2 == 0) SampleWmi();
    if (!g_lhm) SampleThermalPdh();
    if (GetTickCount64() - g_procWanted < 4000) SampleProcs();
    PushHistory();
    PersistNetToday(false);
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

static void MeasureTaskbarButton(IUIAutomation* uia, HWND tbWnd) {
    IUIAutomationElement* root = nullptr;
    if (FAILED(uia->ElementFromHandle(tbWnd, &root)) || !root) return;

    VARIANT v; VariantInit(&v);
    v.vt = VT_I4; v.lVal = UIA_ButtonControlTypeId;
    IUIAutomationCondition* cond = nullptr;
    uia->CreatePropertyCondition(UIA_ControlTypePropertyId, v, &cond);

    IUIAutomationElementArray* arr = nullptr;
    if (cond) root->FindAll(TreeScope_Descendants, cond, &arr);

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
            if (SUCCEEDED(el->get_CurrentBoundingRectangle(&r)) && r.right - r.left > 10 && r.bottom - r.top > 10 &&
                r.left >= tb.left && r.right <= tb.right && r.top >= tb.top - 4 && r.bottom <= tb.bottom + 4) {
                if (!have || r.left < best.left) { best = r; have = true; }
            }
            el->Release();
        }
        arr->Release();
    }
    if (cond) cond->Release();
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

static DWORD WINAPI UiaWorker(LPVOID) {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    static const GUID kClsidUia = {0xff48dba4, 0x60ef, 0x4201, {0xaa, 0x87, 0x54, 0x10, 0x3e, 0xef, 0x59, 0x4e}};
    static const GUID kIidUia = {0x30cbe57d, 0xd9d0, 0x452a, {0xab, 0x13, 0x7a, 0xc5, 0xac, 0x48, 0x25, 0xee}};
    IUIAutomation* uia = nullptr;
    CoCreateInstance(kClsidUia, nullptr, CLSCTX_INPROC_SERVER, kIidUia, (void**)&uia);
    while (!g_stop) {
        std::vector<HWND> list;
        AcquireSRWLockShared(&g_btnLock);
        list = g_tbList;
        ReleaseSRWLockShared(&g_btnLock);
        bool allOk = !list.empty();
        for (HWND tb : list) {
            if (g_stop) break;
            if (uia) MeasureTaskbarButton(uia, tb);
            if (!GetBtn(tb).ok) allOk = false;
        }
        int waits = allOk ? 5 : 2;  // quick retries until found, then follow icon changes twice a second
        for (int i = 0; i < waits && !g_stop; i++) Sleep(100);
    }
    if (uia) uia->Release();
    CoUninitialize();
    return 0;
}

static DWORD WINAPI SamplerWorker(LPVOID) {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    while (!g_stop) {
        SampleAll();
        int ms = g_s.refreshMs;
        for (int i = 0; i < ms / 50 && !g_stop; i++) Sleep(50);
    }
    PersistNetToday(true);
    if (g_pdhQ) { PdhCloseQuery(g_pdhQ); g_pdhQ = nullptr; }
    if (g_thQ) { PdhCloseQuery(g_thQ); g_thQ = nullptr; }
    if (g_diskQ) { PdhCloseQuery(g_diskQ); g_diskQ = nullptr; }
    if (g_lhm) { g_lhm->Release(); g_lhm = nullptr; }
    if (g_acpi) { g_acpi->Release(); g_acpi = nullptr; }
    CoUninitialize();
    return 0;
}

// ------------------------------------------------------------- overlays ----

struct Cell { RECT r; int item; };

struct Overlay {
    HWND taskbar = nullptr, hwnd = nullptr, tip = nullptr;
    int lastW = 0, lastH = 0;
    bool lastAuto = false, shown = false;
    LONG px = 0, py = 0;
    int hideCount = 0;
    ULONGLONG startTick = 0;
    std::vector<Cell> cells;
    int hoverItem = -1;
    bool tracking = false;
    std::wstring tipText;
    bool tipVisible = false;
    ULONGLONG tipStart = 0;
};
static std::vector<Overlay*> g_overlays;  // UI thread only

typedef BOOL(WINAPI* GetWindowBand_t)(HWND, PDWORD);
typedef BOOL(WINAPI* SetWindowBand_t)(HWND, HWND, DWORD);
static GetWindowBand_t g_getBand = nullptr;
static SetWindowBand_t g_setBand = nullptr;

static void UpdatePosition(Overlay* ov, bool force) {
    if (!ov->hwnd || !ov->taskbar || !ov->lastW) return;
    RECT trc;
    if (!GetWindowRect(ov->taskbar, &trc)) return;

    // hide only when the taskbar has been gone for a few ticks (avoids flicker while apps minimise)
    if (!IsWindowVisible(ov->taskbar) || g_fullscreen) ov->hideCount++; else ov->hideCount = 0;
    if (ov->hideCount >= 4) {
        if (ov->shown) { ShowWindow(ov->hwnd, SW_HIDE); ov->shown = false; }
        return;
    }
    if (ov->hideCount > 0) return;

    float sc = GetDpiForWindow(ov->taskbar) / 96.0f;
    BtnInfo b = GetBtn(ov->taskbar);
    LONG x, y;
    LONG wantManualX = trc.left + (LONG)((g_s.dockMargin > 0 ? g_s.dockMargin + g_s.leftPad : g_s.leftOffset) * sc);
    if (ov->lastAuto && b.ok) {
        LONG gap = (LONG)(g_s.gapIcons * sc);
        LONG maxX = trc.left + b.relLeft - gap - ov->lastW;  // never overlap the first icon
        LONG wantX = g_s.dockMargin > 0 ? trc.left + (LONG)((g_s.dockMargin + g_s.leftPad) * sc) : maxX;
        x = std::max<LONG>(trc.left, std::min(wantX, maxX));
        y = trc.top + b.relTop;
    } else {
        x = wantManualX;
        y = trc.top + (LONG)(g_s.topOffset * sc);
    }

    // when the taskbar changes z-band (e.g. Start menu opens) follow it; the owner link normally does this already
    if (g_getBand && ov == g_overlays.front()) {
        DWORD tbBand = 0, myBand = 0;
        g_getBand(ov->taskbar, &tbBand);
        g_getBand(ov->hwnd, &myBand);
        static DWORD lastTb = 0, lastMy = 0;
        if (tbBand != lastTb || myBand != lastMy) {
            Wh_Log(L"band taskbar=%u overlay=%u", tbBand, myBand);
            lastTb = tbBand; lastMy = myBand;
        }
        if (tbBand && myBand && tbBand != myBand && g_setBand) {
            BOOL ok = g_setBand(ov->hwnd, nullptr, tbBand);
            static int tries = 0;
            if (tries++ < 6) Wh_Log(L"SetWindowBand(%u) -> %d err=%lu", tbBand, ok, ok ? 0 : GetLastError());
        }
    }

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

static HWINEVENTHOOK g_locHook = nullptr;
static void CALLBACK LocationEventProc(HWINEVENTHOOK, DWORD, HWND hwnd, LONG idObject, LONG, DWORD, DWORD) {
    if (idObject != OBJID_WINDOW) return;
    for (Overlay* ov : g_overlays)
        if (ov->taskbar == hwnd) { UpdatePosition(ov, false); return; }  // follow the taskbar the instant it moves
}

// -------------------------------------------------------------- drawing ----

static bool LightTheme() {
    DWORD v = 0, sz = sizeof(v);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &v, &sz) == ERROR_SUCCESS)
        return v != 0;
    return false;
}

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

// ---- value formatting ----
static const wchar_t* UK() { return g_s.binary ? L"KiB" : L"KB"; }
static const wchar_t* UM() { return g_s.binary ? L"MiB" : L"MB"; }
static const wchar_t* UG() { return g_s.binary ? L"GiB" : L"GB"; }
static std::wstring Fmt(const wchar_t* f, ...) {
    wchar_t b[96];
    va_list ap; va_start(ap, f);
    _vsnwprintf(b, 95, f, ap);
    va_end(ap);
    b[95] = 0;
    return b;
}

static std::wstring FmtPct(double v) { return v < 0 ? L"--%" : Fmt(L"%.0f%%", v); }

static std::wstring FmtTemp(double c) {
    if (c <= 0) return g_s.fahrenheit ? L"--\u00B0F" : L"--\u00B0C";
    return g_s.fahrenheit ? Fmt(L"%.0f\u00B0F", c * 9.0 / 5.0 + 32.0) : Fmt(L"%.0f\u00B0C", c);
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

// ---- one text piece of an item: drawn at a fixed width (template) so columns don't jitter ----
struct Seg { std::wstring text, tmpl; bool warn; };

static void BuildSegs(int item, std::vector<Seg>& out) {
    auto usageSeg = [&](double v) { return Seg{FmtPct(v), L"100%", v >= g_s.warnUsage}; };
    auto tempSeg = [&](double c) { return Seg{FmtTemp(c), g_s.fahrenheit ? L"100\u00B0F" : L"100\u00B0C", c >= g_s.warnTemp}; };
    switch (item) {
        case IT_CPU:
            out.push_back(usageSeg(g_disp.cpu));
            if (g_s.showCpuTemp) out.push_back(tempSeg(g_st.cpuTemp));
            break;
        case IT_GPU: {
            out.push_back(usageSeg(g_disp.gpu));
            if (g_s.showGpuTemp) {
                double t = g_st.gpuTemp > 0 ? g_st.gpuTemp : (g_s.igpuFromCpu && g_st.cpuTemp > 0 ? g_st.cpuTemp : -1);
                out.push_back(tempSeg(t));
            }
            break;
        }
        case IT_RAM:
            out.push_back(Seg{g_s.ramTotal ? Fmt(L"%.1f/%.1f %s", g_st.ramUsedGB, g_st.ramTotalGB, UG()) : Fmt(L"%.1f %s", g_st.ramUsedGB, UG()),
                              g_s.ramTotal ? Fmt(L"99.9/99.9 %s", UG()) : Fmt(L"99.9 %s", UG()), g_st.ram >= g_s.warnUsage});
            if (g_s.ramPct) out.push_back(usageSeg(g_st.ram));
            break;
        case IT_DISK:
            out.push_back(usageSeg(g_disp.disk));
            break;
        case IT_DOWN:
        case IT_UP:
            out.push_back(Seg{FmtSpeed(item == IT_DOWN ? g_disp.down : g_disp.up), g_s.bits ? std::wstring(L"999.9 Mbps") : Fmt(L"999.9 %s/s", UM()), false});
            if (g_s.netTotal) out.push_back(Seg{FmtBytes(item == IT_DOWN ? g_st.todayDown : g_st.todayUp), Fmt(L"999.9 %s", UG()), false});
            break;
    }
}

static void Render(Overlay* ov) {
    if (!ov->taskbar || !ov->hwnd) return;
    RECT trc;
    if (!GetWindowRect(ov->taskbar, &trc)) return;

    // wait (hidden) for the first measurement instead of flashing at a wrong spot
    BtnInfo binfo = GetBtn(ov->taskbar);
    bool useAuto = g_s.autoPos && binfo.ok && binfo.h >= 24;
    if (g_s.autoPos && !useAuto && GetTickCount64() - ov->startTick < 4000) return;
    if (g_fullscreen) { UpdatePosition(ov, false); return; }

    float sc = GetDpiForWindow(ov->taskbar) / 96.0f;
    int H = g_s.height > 0 ? (int)(g_s.height * sc) : (trc.bottom - trc.top);
    if (useAuto) H = binfo.h;
    float fs = g_s.fontSize * sc;
    float icon = fs * g_s.iconScale / 100.0f;
    float gap = g_s.itemGap * sc, colGap = g_s.colGap * sc, pad = 4 * sc;

    BYTE alpha = (BYTE)(255 * g_s.opacity / 100);
    Color txt = g_s.hasColor ? Color(alpha, g_s.tr, g_s.tg, g_s.tb)
                             : (g_lightTheme ? Color(alpha, 30, 30, 30) : Color(alpha, 240, 240, 240));
    Color warnCol(alpha, g_s.wr, g_s.wg, g_s.wb);

    std::unique_ptr<FontFamily> fam(new FontFamily(g_s.fontFamily.c_str()));
    if (!fam->IsAvailable()) fam.reset(new FontFamily(L"Segoe UI Variable Text"));
    if (!fam->IsAvailable()) fam.reset(new FontFamily(L"Segoe UI"));
    Font font(fam.get(), fs, g_s.bold ? FontStyleBold : FontStyleRegular, UnitPixel);
    StringFormat sf(StringFormat::GenericTypographic());
    sf.SetFormatFlags(sf.GetFormatFlags() | StringFormatFlagsMeasureTrailingSpaces | StringFormatFlagsNoWrap);
    sf.SetLineAlignment(StringAlignmentCenter);

    Bitmap mbmp(1, 1, PixelFormat32bppPARGB);
    Graphics mg(&mbmp);
    auto tw = [&](const std::wstring& s) { RectF r; mg.MeasureString(s.c_str(), -1, &font, PointF(0, 0), &sf, &r); return r.Width; };

    // layout: per column its items and segments
    struct ColLayout { std::vector<int> items; std::vector<std::vector<Seg>> segs; float w = 0; };
    std::vector<ColLayout> cols;
    int rows = 1;
    for (const auto& c : g_s.cols) {
        ColLayout cl;
        cl.items = c;
        rows = std::max<int>(rows, (int)c.size());
        for (int it : c) {
            std::vector<Seg> segs;
            if (it != IT_NONE) BuildSegs(it, segs);
            float w = g_s.icons ? icon + gap : 0;
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

    // draw into a DIB so we can use per-pixel alpha
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = W;
    bi.bmiHeader.biHeight = -H;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    HDC screen = GetDC(nullptr);
    HDC mem = CreateCompatibleDC(screen);
    void* bits = nullptr;
    HBITMAP dib = CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!dib || !bits) { DeleteDC(mem); ReleaseDC(nullptr, screen); return; }
    HGDIOBJ old = SelectObject(mem, dib);
    ov->cells.clear();
    {
        Bitmap bmp(W, H, W * 4, PixelFormat32bppPARGB, (BYTE*)bits);
        Graphics g(&bmp);
        // an almost invisible fill keeps the whole area clickable / hoverable
        g.Clear(g_s.interactive ? Color(2, 0, 0, 0) : Color(0, 0, 0, 0));
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
                ov->cells.push_back({{(LONG)x, (LONG)rowY, (LONG)(x + cl.w), (LONG)(rowY + rowH)}, item});

                // sparkline behind the item
                if (g_s.spark) {
                    double hist[60];
                    CopyHistory(item, hist);
                    double mx = (item == IT_DOWN || item == IT_UP) ? 100 * 1024.0 : 100.0;
                    for (double v : hist) mx = std::max(mx, v);
                    PointF pts[62];
                    float bottom = rowY + rowH * 0.92f, hgt = rowH * 0.8f;
                    for (int i = 0; i < 60; i++) pts[i + 1] = PointF(x + cl.w * i / 59.0f, bottom - (float)(hist[i] / mx) * hgt);
                    pts[0] = PointF(x, bottom);
                    pts[61] = PointF(x + cl.w, bottom);
                    BYTE sa = (BYTE)(alpha * g_s.sparkOp / 100);
                    SolidBrush fill(Color((BYTE)(sa * 0.45f), txt.GetR(), txt.GetG(), txt.GetB()));
                    g.FillPolygon(&fill, pts, 62);
                    Pen line(Color(sa, txt.GetR(), txt.GetG(), txt.GetB()), std::max(1.0f, sc * 0.9f));
                    g.DrawLines(&line, pts + 1, 60);
                }

                float tx = x;
                if (g_s.icons) {
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
                    SolidBrush b(g_s.warn && segs[i].warn ? warnCol : txt);
                    g.DrawString(segs[i].text.c_str(), -1, &font, RectF(tx, rowY, 400 * sc, rowH), &sf, &b);
                    tx += tw(segs[i].tmpl) + gap;
                }
            }
            x += cl.w + colGap;
        }
    }

    SIZE size = {W, H};
    POINT src = {0, 0};
    BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    ov->lastW = W; ov->lastH = H; ov->lastAuto = useAuto;
    SetWindowPos(ov->hwnd, nullptr, 0, 0, W, H, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
    UpdateLayeredWindow(ov->hwnd, screen, nullptr, &size, mem, &src, 0, &bf, ULW_ALPHA);

    SelectObject(mem, old);
    DeleteObject(dib);
    DeleteDC(mem);
    ReleaseDC(nullptr, screen);

    UpdatePosition(ov, true);
}

static void RenderAll() {
    for (Overlay* ov : g_overlays) Render(ov);
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
    g_procWanted = GetTickCount64();
    switch (item) {
        case IT_CPU: {
            std::wstring s = Fmt(L"CPU  %.0f%%", g_st.cpu);
            if (g_st.cpuTemp > 0) s += L"\r\nTemperature  " + FmtTemp(g_st.cpuTemp) + (g_cpuTempSrc == 2 ? L"  (Windows thermal zone, approximate)" : L"");
            else s += L"\r\nTemperature  not available (run LibreHardwareMonitor)";
            std::wstring p = ProcLines(true);
            if (!p.empty()) s += L"\r\nBusiest apps:" + p;
            return s;
        }
        case IT_GPU: {
            std::wstring s = L"GPU  " + FmtPct(g_st.gpu);
            if (g_st.gpuTemp > 0) s += L"\r\nTemperature  " + FmtTemp(g_st.gpuTemp);
            else if (g_s.igpuFromCpu && g_st.cpuTemp > 0) s += L"\r\nTemperature  " + FmtTemp(g_st.cpuTemp) + L"  (shared with the CPU)";
            else s += L"\r\nTemperature  not available";
            return s;
        }
        case IT_RAM: {
            std::wstring s = Fmt(L"Memory  %.1f of %.1f %s used  (%.0f%%)\r\nFree  %.1f %s", g_st.ramUsedGB, g_st.ramTotalGB, UG(), g_st.ram,
                                 g_st.ramTotalGB - g_st.ramUsedGB, UG());
            std::wstring p = ProcLines(false);
            if (!p.empty()) s += L"\r\nBiggest apps:" + p;
            return s;
        }
        case IT_DISK:
            return Fmt(L"Disk activity  %s\r\nRead  %s\r\nWrite  %s", FmtPct(g_st.disk).c_str(), FmtSpeed(g_st.diskRead).c_str(),
                       FmtSpeed(g_st.diskWrite).c_str());
        case IT_DOWN:
        case IT_UP:
            return Fmt(L"Download  %s\r\nUpload  %s\r\nToday  down %s, up %s", FmtSpeed(g_st.down).c_str(), FmtSpeed(g_st.up).c_str(),
                       FmtBytes(g_st.todayDown).c_str(), FmtBytes(g_st.todayUp).c_str());
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
            if (!g_s.customCmd.empty()) ShellExecuteW(nullptr, L"open", g_s.customCmd.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
            break;
    }
}

static LRESULT CALLBACK OverlayProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    Overlay* ov = (Overlay*)GetWindowLongPtrW(h, GWLP_USERDATA);
    switch (m) {
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
    }
    return DefWindowProcW(h, m, w, l);
}

static Overlay* CreateOverlay(HWND taskbar) {
    DWORD ex = WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE;
    if (!g_s.interactive) ex |= WS_EX_TRANSPARENT;
    // owner = taskbar: the overlay stays above it in every z-band (e.g. while the Start menu is open)
    HWND hwnd = CreateWindowExW(ex, kClass, L"", WS_POPUP, 0, 0, 10, 10, taskbar, nullptr, GetModuleHandleW(nullptr), nullptr);
    if (!hwnd) return nullptr;
    Overlay* ov = new Overlay();
    ov->taskbar = taskbar;
    ov->hwnd = hwnd;
    ov->startTick = GetTickCount64();
    SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)ov);

    ov->tip = CreateWindowExW(WS_EX_TOPMOST, TOOLTIPS_CLASSW, nullptr, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP | TTS_NOANIMATE | TTS_NOFADE, 0, 0, 0, 0, hwnd,
                              nullptr, GetModuleHandleW(nullptr), nullptr);
    if (ov->tip) {
        TOOLINFOW ti = {sizeof(ti)};
        ti.uFlags = TTF_TRACK | TTF_ABSOLUTE;
        ti.hwnd = hwnd;
        ti.uId = 1;
        ti.lpszText = (LPWSTR)L"";
        SendMessageW(ov->tip, TTM_ADDTOOLW, 0, (LPARAM)&ti);
        SendMessageW(ov->tip, TTM_SETMAXTIPWIDTH, 0, 600);
    }
    Wh_Log(L"overlay created for taskbar %p owner=%p", (void*)taskbar, (void*)GetWindow(hwnd, GW_OWNER));
    return ov;
}

static void DestroyOverlay(Overlay* ov) {
    if (ov->hwnd) DestroyWindow(ov->hwnd);
    delete ov;
}

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

// adds overlays for new taskbars (monitors), removes those of gone ones, refreshes shared flags
static void SyncOverlays() {
    std::vector<HWND> found;
    EnumWindows(EnumTaskbars, (LPARAM)&found);

    AcquireSRWLockExclusive(&g_btnLock);
    g_tbList = found;
    ReleaseSRWLockExclusive(&g_btnLock);

    for (size_t i = 0; i < g_overlays.size();) {
        if (std::find(found.begin(), found.end(), g_overlays[i]->taskbar) == found.end() || !IsWindow(g_overlays[i]->taskbar)) {
            DestroyOverlay(g_overlays[i]);
            g_overlays.erase(g_overlays.begin() + i);
        } else i++;
    }
    for (HWND tb : found) {
        bool have = false;
        for (Overlay* ov : g_overlays) if (ov->taskbar == tb) have = true;
        if (!have) if (Overlay* ov = CreateOverlay(tb)) g_overlays.push_back(ov);
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

// ---------------------------------------------------------- main window ----

static bool EaseDisp() {
    double s = g_s.smoothing / 100.0;
    bool moving = false;
    auto ease = [&](double& d, double target) {
        if (s <= 0.0 || d < 0 || target < 0) { if (d != target) moving = moving || false; d = target; return; }
        double nd = d + (target - d) * (1.0 - s) * 0.35;
        if (std::fabs(target - nd) < std::max(0.05, std::fabs(target) * 0.004)) nd = target;
        if (nd != target) moving = true;
        d = nd;
    };
    ease(g_disp.cpu, g_st.cpu);
    ease(g_disp.gpu, g_st.gpu);
    ease(g_disp.ram, g_st.ram);
    ease(g_disp.disk, g_st.disk);
    ease(g_disp.down, g_st.down);
    ease(g_disp.up, g_st.up);
    return moving;
}

static void ApplyInteractiveStyle() {
    for (Overlay* ov : g_overlays) {
        LONG_PTR ex = GetWindowLongPtrW(ov->hwnd, GWL_EXSTYLE);
        LONG_PTR want = g_s.interactive ? (ex & ~WS_EX_TRANSPARENT) : (ex | WS_EX_TRANSPARENT);
        if (want != ex) SetWindowLongPtrW(ov->hwnd, GWL_EXSTYLE, want);
    }
}

static LRESULT CALLBACK MsgProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    switch (m) {
        case WM_TIMER:
            if (w == 2) {  // fast tick: follow the taskbar
                for (Overlay* ov : g_overlays) {
                    UpdatePosition(ov, false);
                    if (!ov->lastW) Render(ov);
                    else if (ov->lastAuto) {
                        BtnInfo b = GetBtn(ov->taskbar);
                        if (b.ok && b.h != ov->lastH) Render(ov);
                    }
                }
            } else if (w == 3) {  // easing animation
                bool moving = EaseDisp();
                RenderAll();
                if (!moving) KillTimer(h, 3);
            } else if (w == 4) {
                SyncOverlays();
            } else if (w == 1) {  // new sample available
                if (g_s.smoothing > 0) {
                    SetTimer(h, 3, 33, nullptr);
                } else {
                    EaseDisp();
                    RenderAll();
                }
                for (Overlay* ov : g_overlays) if (ov->hoverItem >= 0) ShowTip(ov, ov->hoverItem, true);  // quiet refresh
            }
            return 0;
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;
        case WM_SETTINGS:
            LoadSettings();
            SetTimer(h, 1, g_s.refreshMs, nullptr);
            SyncOverlays();
            ApplyInteractiveStyle();
            EaseDisp();
            RenderAll();
            return 0;
    }
    return DefWindowProcW(h, m, w, l);
}

static DWORD WINAPI Worker(LPVOID) {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    GdiplusStartupInput gin;
    ULONG_PTR token = 0;
    GdiplusStartup(&token, &gin, nullptr);
    INITCOMMONCONTROLSEX icc = {sizeof(icc), ICC_WIN95_CLASSES};
    InitCommonControlsEx(&icc);

    // only the explorer.exe process that owns the taskbar draws the overlay
    bool found = false;
    for (int i = 0; i < 240 && !g_stop && !found; i++) {
        HWND tb = FindWindowW(L"Shell_TrayWnd", nullptr);
        DWORD pid = 0;
        if (tb) GetWindowThreadProcessId(tb, &pid);
        if (tb && pid == GetCurrentProcessId()) found = true; else Sleep(500);
    }

    if (found && !g_stop) {
        HINSTANCE inst = GetModuleHandleW(nullptr);
        WNDCLASSW wc = {};
        wc.lpfnWndProc = OverlayProc;
        wc.hInstance = inst;
        wc.lpszClassName = kClass;
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        RegisterClassW(&wc);
        WNDCLASSW mc = {};
        mc.lpfnWndProc = MsgProc;
        mc.hInstance = inst;
        mc.lpszClassName = kMsgClass;
        RegisterClassW(&mc);
        g_msgWnd = CreateWindowExW(0, kMsgClass, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, inst, nullptr);

        if (g_msgWnd) {
            HMODULE u32 = GetModuleHandleW(L"user32.dll");
            g_getBand = (GetWindowBand_t)GetProcAddress(u32, "GetWindowBand");
            g_setBand = (SetWindowBand_t)GetProcAddress(u32, "SetWindowBand");
            Wh_Log(L"started; GetWindowBand=%d SetWindowBand=%d", g_getBand != nullptr, g_setBand != nullptr);

            LoadNetToday();
            g_lightTheme = LightTheme();
            SyncOverlays();
            g_samplerThread = CreateThread(nullptr, 0, SamplerWorker, nullptr, 0, nullptr);
            g_uiaThread = CreateThread(nullptr, 0, UiaWorker, nullptr, 0, nullptr);
            g_locHook = SetWinEventHook(EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE, nullptr, LocationEventProc,
                                            GetCurrentProcessId(), 0, WINEVENT_OUTOFCONTEXT);
            SetTimer(g_msgWnd, 1, g_s.refreshMs, nullptr);
            SetTimer(g_msgWnd, 2, 10, nullptr);
            SetTimer(g_msgWnd, 4, 1000, nullptr);

            MSG msg;
            while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }

            g_stop = true;
            if (g_locHook) { UnhookWinEvent(g_locHook); g_locHook = nullptr; }
            for (Overlay* ov : g_overlays) DestroyOverlay(ov);
            g_overlays.clear();
            if (g_samplerThread) { WaitForSingleObject(g_samplerThread, 4000); CloseHandle(g_samplerThread); g_samplerThread = nullptr; }
            if (g_uiaThread) { WaitForSingleObject(g_uiaThread, 3000); CloseHandle(g_uiaThread); g_uiaThread = nullptr; }
            DestroyWindow(g_msgWnd);
            g_msgWnd = nullptr;
        }
        UnregisterClassW(kClass, inst);
        UnregisterClassW(kMsgClass, inst);
    }

    GdiplusShutdown(token);
    CoUninitialize();
    return 0;
}

// -------------------------------------------------------------- windhawk ---

BOOL Wh_ModInit() {
    LoadSettings();
    g_stop = false;
    g_thread = CreateThread(nullptr, 0, Worker, nullptr, 0, nullptr);
    return g_thread != nullptr;
}

void Wh_ModSettingsChanged() {
    // settings are re-read on the UI thread, which owns them
    if (g_msgWnd) PostMessageW(g_msgWnd, WM_SETTINGS, 0, 0);
}

void Wh_ModUninit() {
    g_stop = true;
    if (g_msgWnd) PostMessageW(g_msgWnd, WM_CLOSE, 0, 0);
    if (g_thread) {
        WaitForSingleObject(g_thread, 8000);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
}
