// ==WindhawkMod==
// @id              micromanager
// @name            MicroManager
// @description     Mini task manager tray icon showing CPU, GPU and RAM usage with top consumers.
// @version         1.2.0
// @author          BlackPaw
// @github          https://github.com/BlackPaw21
// @donateUrl       https://ko-fi.com/blackpaw21
// @include         windhawk.exe
// @compilerOptions -lpdh -lshell32 -lgdi32 -luser32 -lole32 -luuid -ladvapi32 -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# MicroManager

![Screenshot](https://i.imgur.com/TaUELAP.png)

A lightweight tray icon that shows a mini task manager popup with live CPU, GPU
and RAM usage, plus the single top-consuming process for each.

## How to Use

1. **Left-click** the tray icon to open the popup showing:
   - Total CPU, GPU, and RAM usage percentages
   - Top-consuming process for each resource
   - 16-sample activity history graph for the selected resource
2. **Click any row or the graph canvas** (or press **Up** / **Down** arrow keys) to cycle the history graph between CPU, GPU, and RAM
3. **Right-click any process row** (or press the **Menu** key / **Shift+F10**) to access process actions:
   - **End task:** Terminates only the selected process
   - **End process tree...:** Safely terminates the application root and all its related processes (with process count confirmation)
   - **Open file location:** Selects the executable file in File Explorer
4. **Click anywhere outside** the popup (or press **Esc**) to close it
5. **Hover** the tray icon to see a live `CPU / GPU / RAM` summary tooltip
6. **Right-click** the tray icon to change refresh rate, choose the active graph metric, or open Windhawk

## Configuration

Right-click the tray icon to change the refresh rate (0.3s / 0.5s / 1s / 3s).

## Changelog

# 1.2.0
- **Added:** Live activity graphs — added a 16-sample history graph to the popup. Click any row or the graph canvas to cycle between live CPU, GPU, and RAM trends.
- **Added:** Process management actions — right-click any top-consumer row to access "End task", "End process tree...", and "Open file location".
- **Added:** Safe process tree termination — resolved application root by ancestor timestamps to eliminate PID reuse issues, added shell (explorer.exe) protection, and added confirmation showing the exact number of related processes.
- **Added:** Open file location — added a quick action to reveal and select the top consumer's executable in File Explorer.
- **Added:** Safety & identity protection — confirms termination targets, validates process creation times against PID reuse, and blocks critical Windows system processes and Windhawk itself.
- **Improved:** High-contrast readability — pure white text across popup labels, graph titles, and context menus for excellent readability with dark and custom themes.
- **Improved:** Top taskbar placement — automatically detects top-aligned taskbars using the icon's monitor rect and flips the popup below the tray icon so it never renders off-screen.
- **Improved:** System process display — correctly identifies and labels "System Idle Process" and the NT Kernel (PID 4) when they consume resources.
- **Fixed:** Tray & navigation fixes — handled WM_CONTEXTMENU for tray right-clicks, added keyboard navigation (Up / Down / Esc), and improved popup reliability.

# 1.1.0
- **Fixed:** Tooltip now displays correctly when hovering the tray icon.
- **Fixed:** Ghost window prevention — popup no longer flickers on rapid open/close.
- **Fixed:** Removed stale exponential backoff in process enumeration retry — STATUS_INFO_LENGTH_MISMATCH only needs a larger buffer.
- **Fixed:** Safe mod reload — icon and window clean up properly without crashing.
- **Improved:** Tray tooltip updates only when values change, reducing unnecessary CPU work.
- **Fixed:** Popup no longer leaves a ghost window behind.

# 1.0.0
- Initial release.
- Left-click to see live CPU and GPU usage with the top process for each.
- Right-click for options. Update interval adjustable in Settings.
*/
// ==/WindhawkModReadme==

#define NOMINMAX
#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <shobjidl.h>
#include <shlobj.h>
#include <propkey.h>
#include <dwmapi.h>
#include <pdh.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
// PDH constants that may not be defined in all SDK versions
#ifndef PDH_MORE_DATA
#define PDH_MORE_DATA ((PDH_STATUS)0x800007D2L)
#endif
#ifndef PDH_CSTATUS_VALID_DATA
#define PDH_CSTATUS_VALID_DATA ((LONG)0x00000000L)
#endif

// DWM window corner preference (Windows 11). Declared here so the mod builds
// against older SDK headers; the call silently no-ops on Windows 10.
#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif
#ifndef DWMWCP_ROUND
#define DWMWCP_ROUND 2
#endif

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

// ─── Constants ────────────────────────────────────────────────────────────────

#define TRAY_ICON_ID        1
#define WM_TRAY_CALLBACK    (WM_USER + 1)
#define WM_TIMER_ID         1

#ifndef NIN_SELECT
#define NIN_SELECT          (WM_USER + 0)
#endif
#ifndef NIN_KEYSELECT
#define NIN_KEYSELECT       (WM_USER + 1)
#endif

// Base (96-DPI) popup geometry — scaled at runtime via Sc().
#define POPUP_WIDTH         360
#define POPUP_HEIGHT        164
#define POPUP_ROWS          3

#define MAX_PROCESSES       1024
#define PROCESS_BUF_SIZE    (512 * 1024)

// Re-attempt GPU (PDH) initialization roughly every this many ticks if it fails,
// instead of disabling GPU stats permanently for the session.
#define GPU_RETRY_TICKS     30

#define MENU_OPEN_WINDHAWK   9000
#define MENU_GRAPH_CPU       9001
#define MENU_GRAPH_GPU       9002
#define MENU_GRAPH_RAM       9003
#define MENU_PROC_END        9200
#define MENU_PROC_END_TREE   9201
#define MENU_PROC_LOCATION   9202
#define MENU_INTERVAL_300MS  9100
#define MENU_INTERVAL_500MS  9101
#define MENU_INTERVAL_1S     9102
#define MENU_INTERVAL_3S     9103

// Stable GUID that gives our tray icon a process-independent identity.
static const GUID MICROMANAGER_TRAY_GUID =
    {0xFA6DAD73, 0xD350, 0x4BA8, {0x97, 0x3F, 0x5F, 0xA6, 0x0B, 0x15, 0x7B, 0x18}};

// ─── NtQuerySystemInformation via dynamic load ────────────────────────────────

#ifndef UNICODE_STRING
typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
} UNICODE_STRING;
#endif

typedef LONG NTSTATUS;

#define STATUS_INFO_LENGTH_MISMATCH ((NTSTATUS)0xC0000004L)
#define SystemProcessInformation 5

typedef struct {
    ULONG NextEntryOffset;
    ULONG NumberOfThreads;
    LARGE_INTEGER WorkingSetPrivateSize;
    ULONG HardFaultCount;
    ULONG NumberOfThreadsHighWatermark;
    LARGE_INTEGER CycleTime;
    LARGE_INTEGER CreateTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER KernelTime;
    UNICODE_STRING ImageName;
    LONG BasePriority;
    HANDLE UniqueProcessId;
    HANDLE InheritedFromUniqueProcessId;
    ULONG HandleCount;
    ULONG SessionId;
    ULONG_PTR Reserved1;
    SIZE_T PeakVirtualSize;
    SIZE_T VirtualSize;
    ULONG PageFaultCount;
    SIZE_T PeakWorkingSetSize;
    SIZE_T WorkingSetSize;
    SIZE_T QuotaPeakPagedPoolUsage;
    SIZE_T QuotaPagedPoolUsage;
    SIZE_T QuotaPeakNonPagedPoolUsage;
    SIZE_T QuotaNonPagedPoolUsage;
    SIZE_T PagefileUsage;
    SIZE_T PeakPagefileUsage;
    SIZE_T PrivatePageCount;
    LARGE_INTEGER ReadOperationCount;
    LARGE_INTEGER WriteOperationCount;
    LARGE_INTEGER OtherOperationCount;
    LARGE_INTEGER ReadTransferCount;
    LARGE_INTEGER WriteTransferCount;
    LARGE_INTEGER OtherTransferCount;
} MY_SYSTEM_PROCESS_INFO;

typedef NTSTATUS (WINAPI *NtQuerySystemInformation_t)(ULONG, PVOID, ULONG, PULONG);

// ─── Globals ──────────────────────────────────────────────────────────────────

static HANDLE              g_trayThread   = nullptr;
static HWND       g_trayHwnd     = nullptr;
static HWND                g_popupHwnd    = nullptr;
static ULONGLONG           g_lastPopupCloseTime = 0;
static ULONGLONG           g_lastPopupOpenTime  = 0;
static HINSTANCE           g_hInstance    = nullptr;
static WCHAR               g_windhawkPath[MAX_PATH] = {};
static WCHAR               g_windhawkModPath[MAX_PATH] = {};
static WCHAR               g_ddoresDllPath[MAX_PATH] = {};

static DWORD               g_updateMs     = 1000;
static int                 g_dpi          = 96;   // popup DPI, refreshed per show
static ULONGLONG           g_totalPhys    = 0;    // total physical RAM, bytes

// Cached stats (updated on timer, read on popup paint)
static int                 g_totalCpu     = -1;
static int                 g_topCpuPct    = 0;
static WCHAR               g_topCpuName[64] = {};
static int                 g_totalGpu     = -1;
static int                 g_topGpuPct    = 0;
static WCHAR               g_topGpuName[64] = {};
static int                 g_totalRam     = -1;
static int                 g_topRamPct    = 0;
static WCHAR               g_topRamName[64] = {};
struct ProcessTarget { DWORD pid = 0; LONGLONG created = 0; WCHAR name[64] = {}; };
static ProcessTarget g_topCpuTarget, g_topGpuTarget, g_topRamTarget;
static int g_actionRow = 0; // Row selected in popup (0: CPU, 1: GPU, 2: RAM)
// The popup keeps short histories for CPU, GPU, and RAM. -1 is a failed read.
static int g_graphMetric = 0;  // 0: CPU, 1: GPU, 2: RAM
static int g_graphSamples[POPUP_ROWS][16] = {};
static int g_graphCount[POPUP_ROWS] = {};
static int g_graphNext[POPUP_ROWS] = {};

static void AppendGraphSample(int metric, int value) {
    if (metric < 0 || metric >= POPUP_ROWS) return;
    g_graphSamples[metric][g_graphNext[metric]] = value;
    g_graphNext[metric] = (g_graphNext[metric] + 1) % 16;
    if (g_graphCount[metric] < 16) g_graphCount[metric]++;
}

// GPU PDH handles
static PDH_HQUERY          g_gpuQuery     = nullptr;
static PDH_HCOUNTER        g_gpuCounter   = nullptr;
static int                 g_gpuRetryIn   = 0;     // ticks until next init attempt

// Cached NtQuerySystemInformation pointer (resolved once)
static NtQuerySystemInformation_t g_ntQuery = nullptr;

// Per-PID GPU aggregation scratch (tray thread only — hoisted off the stack)
struct GpuProc { DWORD pid; double total; };
static GpuProc             g_gpuProcs[MAX_PROCESSES];

// Previous sample for CPU delta
static FILETIME            g_prevIdle     = {};
static FILETIME            g_prevKernel   = {};
static FILETIME            g_prevUser     = {};
static struct {
    DWORD  pid;
    LONGLONG created;
    LONGLONG time;
    WCHAR  name[64];
} g_prevProcs[MAX_PROCESSES], g_curProcs[MAX_PROCESSES];
static int                 g_prevProcCount = 0;
static BOOL                g_hasPrevSample = FALSE;

static HICON               g_iconEnabled  = nullptr;
static HFONT               g_hPopupFont   = nullptr;
static int                 g_fontDpi      = 0;

static UINT                g_taskbarCreatedMsg = 0;
static WCHAR               g_lastTip[128] = {};

// ─── DPI Helpers ──────────────────────────────────────────────────────────────

static int Sc(int v) { return MulDiv(v, g_dpi, 96); }

// ─── Font Helpers ─────────────────────────────────────────────────────────────

static void EnsureFont() {
    if (g_hPopupFont && g_fontDpi == g_dpi) return;
    if (g_hPopupFont) { DeleteObject(g_hPopupFont); g_hPopupFont = nullptr; }
    g_hPopupFont = CreateFontW(-Sc(13), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    g_fontDpi = g_dpi;
}

// ─── CPU Sampling ─────────────────────────────────────────────────────────────

static NtQuerySystemInformation_t GetNtQuery() {
    if (!g_ntQuery) {
        HMODULE h = GetModuleHandleW(L"ntdll.dll");
        if (h)
            g_ntQuery = (NtQuerySystemInformation_t)GetProcAddress(
                h, "NtQuerySystemInformation");
    }
    return g_ntQuery;
}

static int CollectProcessInfo(MY_SYSTEM_PROCESS_INFO** outBuf) {
    NtQuerySystemInformation_t NtQuery = GetNtQuery();
    if (!NtQuery) return 0;

    *outBuf = nullptr;

    for (int retry = 0; retry < 5; retry++) {
        ULONG bufSize = PROCESS_BUF_SIZE * (retry + 2);
        MY_SYSTEM_PROCESS_INFO* newBuf = (MY_SYSTEM_PROCESS_INFO*)realloc(*outBuf, bufSize);
        if (!newBuf) { free(*outBuf); *outBuf = nullptr; return 0; }
        *outBuf = newBuf;
        NTSTATUS status = NtQuery(SystemProcessInformation, *outBuf, bufSize, nullptr);
        if (status != STATUS_INFO_LENGTH_MISMATCH) {
            if (status < 0) {
                free(*outBuf);
                *outBuf = nullptr;
                return 0;
            }
            return 1;
        }
    }
    // All 5 attempts failed with STATUS_INFO_LENGTH_MISMATCH
    free(*outBuf);
    *outBuf = nullptr;
    return 0;
}

// Helper to retrieve display name for a process, gracefully handling Idle and System PIDs
static void GetProcessDisplayName(MY_SYSTEM_PROCESS_INFO* p, WCHAR* outBuf, size_t maxLen) {
    if (p->ImageName.Buffer && p->ImageName.Length > 0) {
        wcsncpy_s(outBuf, maxLen, p->ImageName.Buffer,
            MIN(p->ImageName.Length / sizeof(WCHAR), maxLen - 1));
        outBuf[maxLen - 1] = L'\0';
    } else {
        DWORD pid = (DWORD)(ULONG_PTR)p->UniqueProcessId;
        if (pid == 0)
            wcscpy_s(outBuf, maxLen, L"System Idle Process");
        else if (pid == 4)
            wcscpy_s(outBuf, maxLen, L"System");
        else
            swprintf_s(outBuf, maxLen, L"PID %u", pid);
    }
}

// ─── GPU Sampling (PDH) ───────────────────────────────────────────────────────

static void InitGpuQuery() {
    if (g_gpuQuery) return;

    PDH_STATUS ps = PdhOpenQueryW(nullptr, 0, &g_gpuQuery);
    if (ps != ERROR_SUCCESS) {
        g_gpuQuery = nullptr;
        g_gpuRetryIn = GPU_RETRY_TICKS;
        return;
    }

    ps = PdhAddEnglishCounterW(g_gpuQuery,
        L"\\GPU Engine(*)\\Utilization Percentage", 0, &g_gpuCounter);
    if (ps != ERROR_SUCCESS) {
        PdhCloseQuery(g_gpuQuery);
        g_gpuQuery = nullptr;
        g_gpuCounter = nullptr;
        g_gpuRetryIn = GPU_RETRY_TICKS;
        return;
    }

    // Prime the baseline — PDH rate counters return 0 on the very first collection
    // without a prior sample to delta against. One collection here means the first
    // real tick in CollectGpuStats produces accurate data instead of 0%.
    PdhCollectQueryData(g_gpuQuery);
}

static void CollectGpuStats(int* outTotal, int* outTopPct, WCHAR* outTopName, int nameLen, ProcessTarget* outTopTarget = nullptr) {
    *outTotal = -1;
    *outTopPct = 0;
    outTopName[0] = L'\0';
    if (outTopTarget) *outTopTarget = {};

    // Lazy init with bounded retry (no permanent "GPU disabled" latch).
    if (!g_gpuQuery) {
        if (g_gpuRetryIn > 0) { g_gpuRetryIn--; return; }
        InitGpuQuery();
        if (!g_gpuQuery) return;  // still failing — try again after the backoff
    }

    PdhCollectQueryData(g_gpuQuery);

    DWORD bufSize = 0;
    DWORD itemCount = 0;
    PDH_STATUS ps = PdhGetFormattedCounterArrayW(g_gpuCounter, PDH_FMT_DOUBLE,
        &bufSize, &itemCount, nullptr);
    if (ps != PDH_MORE_DATA || bufSize == 0 || itemCount == 0) return;

    PDH_FMT_COUNTERVALUE_ITEM_W* items = (PDH_FMT_COUNTERVALUE_ITEM_W*)malloc(bufSize);
    if (!items) return;

    ps = PdhGetFormattedCounterArrayW(g_gpuCounter, PDH_FMT_DOUBLE,
        &bufSize, &itemCount, items);
    if (ps != ERROR_SUCCESS) { free(items); return; }

    // Aggregate per PID
    int procCount = 0;
    double grandTotal = 0;

    for (DWORD i = 0; i < itemCount; i++) {
        if (items[i].FmtValue.CStatus != PDH_CSTATUS_VALID_DATA) continue;

        double val = items[i].FmtValue.doubleValue;
        grandTotal += val;

        // Parse instance: "pid_1234_luid_0x00000000_phys_0_eng_0_enum_1"
        PCWSTR s = items[i].szName;
        if (wcsncmp(s, L"pid_", 4) != 0) continue;
        DWORD pid = 0;
        for (s += 4; *s >= L'0' && *s <= L'9'; s++)
            pid = pid * 10 + (*s - L'0');

        int j;
        for (j = 0; j < procCount; j++) {
            if (g_gpuProcs[j].pid == pid) { g_gpuProcs[j].total += val; break; }
        }
        if (j >= procCount && procCount < MAX_PROCESSES) {
            g_gpuProcs[procCount].pid = pid;
            g_gpuProcs[procCount].total = val;
            procCount++;
        }
    }
    free(items);

    if (grandTotal > 100.0) grandTotal = 100.0;
    *outTotal = (int)(grandTotal + 0.5);

    // Find top GPU process
    int topIdx = -1;
    double topVal = 0;
    for (int i = 0; i < procCount; i++) {
        if (g_gpuProcs[i].total > topVal) {
            topVal = g_gpuProcs[i].total;
            topIdx = i;
        }
    }
    if (topVal > 100.0) topVal = 100.0;
    if (topIdx >= 0) {
        *outTopPct = (int)(topVal + 0.5);
        DWORD topPid = g_gpuProcs[topIdx].pid;
        if (outTopTarget) outTopTarget->pid = topPid;

        for (int i = 0; i < g_prevProcCount; i++) {
            if (g_prevProcs[i].pid == topPid) {
                wcscpy_s(outTopName, nameLen, g_prevProcs[i].name);
                if (outTopTarget) {
                    outTopTarget->created = g_prevProcs[i].created;
                    wcscpy_s(outTopTarget->name, g_prevProcs[i].name);
                }
                break;
            }
        }
        if (outTopName[0] == L'\0') {
            swprintf_s(outTopName, nameLen, L"PID %u", topPid);
            if (outTopTarget) {
                wcscpy_s(outTopTarget->name, outTopName);
            }
        }
        if (outTopTarget && outTopTarget->created == 0 && topPid != 0) {
            HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, topPid);
            if (hProc) {
                FILETIME ct, et, kt, ut;
                if (GetProcessTimes(hProc, &ct, &et, &kt, &ut)) {
                    ULARGE_INTEGER stamp;
                    stamp.LowPart = ct.dwLowDateTime;
                    stamp.HighPart = ct.dwHighDateTime;
                    outTopTarget->created = (LONGLONG)stamp.QuadPart;
                }
                CloseHandle(hProc);
            }
        }
    }
}

// ─── Data Refresh ─────────────────────────────────────────────────────────────

static void RefreshData() {
    g_topCpuTarget = {};
    g_topGpuTarget = {};
    g_topRamTarget = {};
    int newTotalCpu = -1, newTopCpuPct = 0;
    WCHAR newTopCpuName[64] = {};
    int newTotalGpu = -1, newTopGpuPct = 0;
    WCHAR newTopGpuName[64] = {};
    int newTotalRam = -1, newTopRamPct = 0;
    WCHAR newTopRamName[64] = {};

    // Total RAM load is cheap and needs no prior sample — read it every tick.
    MEMORYSTATUSEX mem = {sizeof(mem)};
    if (GlobalMemoryStatusEx(&mem)) newTotalRam = (int)mem.dwMemoryLoad;

    // Collect CPU
    FILETIME nowIdle = {}, nowKernel = {}, nowUser = {};
    BOOL systemTimesValid = GetSystemTimes(&nowIdle, &nowKernel, &nowUser);

    ULARGE_INTEGER ui, uk, uu, pi, pk, pu;
    ui.LowPart = nowIdle.dwLowDateTime;     ui.HighPart = nowIdle.dwHighDateTime;
    uk.LowPart = nowKernel.dwLowDateTime;   uk.HighPart = nowKernel.dwHighDateTime;
    uu.LowPart = nowUser.dwLowDateTime;     uu.HighPart = nowUser.dwHighDateTime;
    pi.LowPart = g_prevIdle.dwLowDateTime;  pi.HighPart = g_prevIdle.dwHighDateTime;
    pk.LowPart = g_prevKernel.dwLowDateTime; pk.HighPart = g_prevKernel.dwHighDateTime;
    pu.LowPart = g_prevUser.dwLowDateTime;  pu.HighPart = g_prevUser.dwHighDateTime;

    double totalDelta = (double)(uk.QuadPart - pk.QuadPart) + (double)(uu.QuadPart - pu.QuadPart);
    double idleDelta = (double)(ui.QuadPart - pi.QuadPart);

    if (systemTimesValid && g_hasPrevSample &&
        uk.QuadPart >= pk.QuadPart && uu.QuadPart >= pu.QuadPart &&
        ui.QuadPart >= pi.QuadPart && totalDelta > 0 && idleDelta <= totalDelta) {
        newTotalCpu = (int)(100.0 - 100.0 * idleDelta / totalDelta + 0.5);

        MY_SYSTEM_PROCESS_INFO* buf = nullptr;
        if (CollectProcessInfo(&buf) && buf) {
            MY_SYSTEM_PROCESS_INFO* p = buf;
            LONGLONG bestTime = 0;
            WCHAR bestCpuName[64] = {};
            ProcessTarget bestCpuTarget;
            ULONGLONG bestWs = 0;
            WCHAR bestRamName[64] = {};
            ProcessTarget bestRamTarget;
            int count = 0;

            while (true) {
                DWORD pid = (DWORD)(ULONG_PTR)p->UniqueProcessId;
                LONGLONG curTime = p->KernelTime.QuadPart + p->UserTime.QuadPart;

                LONGLONG prevTime = 0;
                BOOL foundInPrev = FALSE;
                for (int i = 0; i < g_prevProcCount; i++) {
                    if (g_prevProcs[i].pid == pid && g_prevProcs[i].created == p->CreateTime.QuadPart) {
                        prevTime = g_prevProcs[i].time;
                        foundInPrev = TRUE;
                        break;
                    }
                }
                // Only count delta for processes seen last tick; new/overflow processes
                // would otherwise show their entire boot-time CPU as a single-tick spike.
                // Also skip System Idle Process (PID 0) so idle CPU is not treated as consumer.
                LONGLONG delta = (foundInPrev && pid != 0 && curTime >= prevTime) ? (curTime - prevTime) : 0;
                if (delta > bestTime) {
                    bestTime = delta;
                    GetProcessDisplayName(p, bestCpuName, 64);
                    bestCpuTarget.pid = pid;
                    bestCpuTarget.created = p->CreateTime.QuadPart;
                    wcscpy_s(bestCpuTarget.name, bestCpuName);
                }

                // Top RAM consumer by working set (absolute — skip PID 0).
                if (pid != 0 && (ULONGLONG)p->WorkingSetSize > bestWs) {
                    bestWs = (ULONGLONG)p->WorkingSetSize;
                    GetProcessDisplayName(p, bestRamName, 64);
                    bestRamTarget.pid = pid;
                    bestRamTarget.created = p->CreateTime.QuadPart;
                    wcscpy_s(bestRamTarget.name, bestRamName);
                }

                if (count < MAX_PROCESSES) {
                    g_curProcs[count].pid = pid;
                    g_curProcs[count].created = p->CreateTime.QuadPart;
                    g_curProcs[count].time = curTime;
                    GetProcessDisplayName(p, g_curProcs[count].name, 64);
                    count++;
                }

                if (p->NextEntryOffset == 0) break;
                p = (MY_SYSTEM_PROCESS_INFO*)((BYTE*)p + p->NextEntryOffset);
            }
            memcpy(g_prevProcs, g_curProcs, count * sizeof(g_prevProcs[0]));
            g_prevProcCount = count;

            if (bestTime > 0) {
                double pct = 100.0 * (double)bestTime / totalDelta;
                if (pct >= 0.5) {
                    newTopCpuPct = (int)(pct + 0.5);
                    wcscpy_s(newTopCpuName, bestCpuName);
                    g_topCpuTarget = bestCpuTarget;
                }
            }

            if (bestWs > 0 && g_totalPhys > 0) {
                int pct = (int)((bestWs * 100ULL) / g_totalPhys);
                newTopRamPct = pct < 1 ? 1 : pct;  // the top consumer is always shown
                wcscpy_s(newTopRamName, bestRamName);
                g_topRamTarget = bestRamTarget;
            }

            free(buf);
        }
    }

    if (systemTimesValid) {
        g_prevIdle = nowIdle; g_prevKernel = nowKernel; g_prevUser = nowUser;
        g_hasPrevSample = TRUE;
    } else {
        g_hasPrevSample = FALSE;
    }

    ProcessTarget newTopGpuTarget = {};
    CollectGpuStats(&newTotalGpu, &newTopGpuPct, newTopGpuName, 64, &newTopGpuTarget);

    g_totalCpu = newTotalCpu;
    g_topCpuPct = newTopCpuPct;
    wcscpy_s(g_topCpuName, newTopCpuName);
    g_totalGpu = newTotalGpu;
    g_topGpuPct = newTopGpuPct;
    wcscpy_s(g_topGpuName, newTopGpuName);
    g_topGpuTarget = newTopGpuTarget;
    g_totalRam = newTotalRam;
    g_topRamPct = newTopRamPct;
    wcscpy_s(g_topRamName, newTopRamName);
    AppendGraphSample(0, newTotalCpu);
    AppendGraphSample(1, newTotalGpu);
    AppendGraphSample(2, newTotalRam);

    if (g_popupHwnd && IsWindowVisible(g_popupHwnd)) {
        InvalidateRect(g_popupHwnd, nullptr, TRUE);
    }

    // Live summary tooltip — refresh only when the displayed numbers change.
    if (g_trayHwnd) {
        WCHAR cpuS[8], gpuS[8], ramS[8], tip[128];
        if (newTotalCpu < 0) wcscpy_s(cpuS, L"--"); else swprintf_s(cpuS, L"%d%%", newTotalCpu);
        if (newTotalGpu < 0) wcscpy_s(gpuS, L"--"); else swprintf_s(gpuS, L"%d%%", newTotalGpu);
        if (newTotalRam < 0) wcscpy_s(ramS, L"--"); else swprintf_s(ramS, L"%d%%", newTotalRam);
        swprintf_s(tip, L"CPU %s   GPU %s   RAM %s", cpuS, gpuS, ramS);

        if (wcscmp(tip, g_lastTip) != 0) {
            wcscpy_s(g_lastTip, tip);
            NOTIFYICONDATAW nid = {sizeof(nid)};
            nid.hWnd     = (HWND)g_trayHwnd;
            nid.uID      = TRAY_ICON_ID;
            nid.uFlags   = NIF_TIP | NIF_GUID | NIF_SHOWTIP;
            nid.guidItem = MICROMANAGER_TRAY_GUID;
            lstrcpynW(nid.szTip, tip, 128);
            if (!Shell_NotifyIconW(NIM_MODIFY, &nid)) {
                g_lastTip[0] = L'\0';
            }
        }
    }
}

// ─── Popup Window Procedure ───────────────────────────────────────────────────

static HANDLE OpenSampledProcess(const ProcessTarget& target, DWORD rights) {
    if (!target.pid || !target.created || target.pid == GetCurrentProcessId()) return nullptr;
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | rights, FALSE, target.pid);
    if (!process) return nullptr;
    FILETIME created, exited, kernel, user;
    ULARGE_INTEGER stamp;
    if (!GetProcessTimes(process, &created, &exited, &kernel, &user)) {
        CloseHandle(process);
        return nullptr;
    }
    stamp.LowPart = created.dwLowDateTime;
    stamp.HighPart = created.dwHighDateTime;
    if (stamp.QuadPart != (ULONGLONG)target.created ||
        WaitForSingleObject(process, 0) != WAIT_TIMEOUT) {
        CloseHandle(process);
        return nullptr;
    }
    return process;
}

static bool IsProtectedProcess(HANDLE process) {
    if (g_windhawkPath[0] != L'\0' || g_windhawkModPath[0] != L'\0') {
        WCHAR path[MAX_PATH];
        DWORD len = ARRAYSIZE(path);
        if (QueryFullProcessImageNameW(process, 0, path, &len)) {
            if (g_windhawkPath[0] != L'\0' && _wcsicmp(path, g_windhawkPath) == 0) return true;
            if (g_windhawkModPath[0] != L'\0' && _wcsicmp(path, g_windhawkModPath) == 0) return true;
        }
    }
    BOOL critical = FALSE;
    if (!IsProcessCritical(process, &critical) || critical) return true;
    HANDLE token = nullptr;
    if (!OpenProcessToken(process, TOKEN_QUERY, &token)) return true;
    BYTE storage[512];
    DWORD needed = 0;
    bool protectedAccount = true;
    if (GetTokenInformation(token, TokenUser, storage, sizeof(storage), &needed)) {
        PSID sid = reinterpret_cast<TOKEN_USER*>(storage)->User.Sid;
        protectedAccount = IsWellKnownSid(sid, WinLocalSystemSid) ||
            IsWellKnownSid(sid, WinLocalServiceSid) ||
            IsWellKnownSid(sid, WinNetworkServiceSid);
    }
    CloseHandle(token);
    return protectedAccount;
}

static void RunProcessAction(HWND popup, const ProcessTarget& target, UINT action);

static void TerminateProcessTree(HWND popup, const ProcessTarget& target) {
    DWORD shellPid = 0;
    HWND hShell = GetShellWindow();
    if (hShell) GetWindowThreadProcessId(hShell, &shellPid);
    if (target.pid == shellPid || _wcsicmp(target.name, L"explorer.exe") == 0) {
        MessageBoxW(popup,
            L"Ending the process tree for Windows Explorer is not supported because it would terminate the shell and all running desktop applications. Use \"End task\" instead.",
            L"MicroManager", MB_OK | MB_ICONWARNING);
        return;
    }

    MY_SYSTEM_PROCESS_INFO* buf = nullptr;
    if (!CollectProcessInfo(&buf) || !buf) {
        RunProcessAction(popup, target, MENU_PROC_END);
        return;
    }

    struct ProcItem {
        DWORD pid;
        DWORD parentPid;
        LONGLONG created;
        WCHAR name[64];
    };
    std::vector<ProcItem> allProcs;
    MY_SYSTEM_PROCESS_INFO* p = buf;
    while (p) {
        ProcItem item = {};
        item.pid = (DWORD)(ULONG_PTR)p->UniqueProcessId;
        item.parentPid = (DWORD)(ULONG_PTR)p->InheritedFromUniqueProcessId;
        item.created = p->CreateTime.QuadPart;
        GetProcessDisplayName(p, item.name, ARRAYSIZE(item.name));
        allProcs.push_back(item);
        if (p->NextEntryOffset == 0) break;
        p = (MY_SYSTEM_PROCESS_INFO*)((BYTE*)p + p->NextEntryOffset);
    }
    free(buf);

    auto canEnd = [&](const ProcItem& item) {
        if (item.pid == 0 || item.pid == 4 || item.pid == shellPid || item.pid == GetCurrentProcessId()) return false;
        if (_wcsicmp(item.name, L"explorer.exe") == 0) return false;
        ProcessTarget t = {};
        t.pid = item.pid;
        t.created = item.created;
        wcscpy_s(t.name, item.name);
        HANDLE h = OpenSampledProcess(t, PROCESS_TERMINATE);
        if (!h) return false;
        bool ok = !IsProtectedProcess(h);
        CloseHandle(h);
        return ok;
    };

    auto isParentOf = [](const ProcItem& parent, const ProcItem& child) {
        return child.parentPid == parent.pid && child.pid != parent.pid &&
               parent.created < child.created;
    };

    const ProcItem* targetItem = nullptr;
    for (const auto& item : allProcs) {
        if (item.pid == target.pid && item.created == target.created) {
            targetItem = &item;
            break;
        }
    }
    if (!targetItem) {
        MessageBoxW(popup, L"The selected process has exited, changed identity, or denied access.",
                    L"MicroManager", MB_OK | MB_ICONINFORMATION);
        return;
    }

    if (!canEnd(*targetItem)) {
        MessageBoxW(popup, L"This process is protected or its protection status is unavailable.",
                    L"MicroManager", MB_OK | MB_ICONWARNING);
        return;
    }

    // 1. Walk up the parent chain while parent shares the same executable name and can be ended
    ProcItem root = *targetItem;
    bool movedUp = true;
    while (movedUp) {
        movedUp = false;
        for (const auto& parent : allProcs) {
            if (isParentOf(parent, root) && _wcsicmp(parent.name, root.name) == 0 && canEnd(parent)) {
                root = parent;
                movedUp = true;
                break;
            }
        }
    }

    // 2. Collect all descendants under root that can be ended
    std::vector<ProcItem> treeItems;
    treeItems.push_back(root);
    size_t qHead = 0;
    while (qHead < treeItems.size()) {
        const ProcItem parent = treeItems[qHead++];
        for (const auto& child : allProcs) {
            if (isParentOf(parent, child) && canEnd(child)) {
                bool already = false;
                for (const auto& existing : treeItems) {
                    if (existing.pid == child.pid) { already = true; break; }
                }
                if (!already) {
                    treeItems.push_back(child);
                }
            }
        }
    }

    // 3. Confirm with user
    WCHAR prompt[300];
    if (treeItems.size() <= 1) {
        swprintf_s(prompt, L"End %s (PID %lu)?\nUnsaved work may be lost.", root.name, root.pid);
    } else {
        swprintf_s(prompt, L"End %s (PID %lu) and %u related processes?\nUnsaved work may be lost.",
                   root.name, root.pid, (UINT)(treeItems.size() - 1));
    }

    if (MessageBoxW(popup, prompt, L"Confirm End Process Tree", MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2) != IDYES) {
        return;
    }

    // 4. Terminate bottom-up (descendants first, root last)
    for (auto it = treeItems.rbegin(); it != treeItems.rend(); ++it) {
        ProcessTarget t = {};
        t.pid = it->pid;
        t.created = it->created;
        wcscpy_s(t.name, it->name);
        HANDLE h = OpenSampledProcess(t, PROCESS_TERMINATE | SYNCHRONIZE);
        if (h) {
            if (!IsProtectedProcess(h)) {
                TerminateProcess(h, 1);
            }
            CloseHandle(h);
        }
    }
}

static void RunProcessAction(HWND popup, const ProcessTarget& target, UINT action) {
    if (action == MENU_PROC_END_TREE) {
        TerminateProcessTree(popup, target);
        return;
    }

    DWORD rights = (action == MENU_PROC_END) ? (PROCESS_TERMINATE | SYNCHRONIZE) : SYNCHRONIZE;
    HANDLE process = OpenSampledProcess(target, rights);
    if (!process) {
        MessageBoxW(popup, L"The selected process has exited, changed identity, or denied access.",
                    L"MicroManager", MB_OK | MB_ICONINFORMATION);
        return;
    }

    if (action == MENU_PROC_END) {
        if (IsProtectedProcess(process)) {
            MessageBoxW(popup, L"This process is protected or its protection status is unavailable.",
                        L"MicroManager", MB_OK | MB_ICONWARNING);
        } else {
            WCHAR prompt[256];
            swprintf_s(prompt, L"End %s (PID %lu)?\nUnsaved work may be lost.", target.name, target.pid);
            if (MessageBoxW(popup, prompt, L"Confirm End Task", MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2) == IDYES &&
                WaitForSingleObject(process, 0) == WAIT_TIMEOUT) {
                TerminateProcess(process, 1);
            }
        }
    } else if (action == MENU_PROC_LOCATION) {
        WCHAR path[32768];
        DWORD length = ARRAYSIZE(path);
        PIDLIST_ABSOLUTE item = nullptr;
        if (!QueryFullProcessImageNameW(process, 0, path, &length) ||
            FAILED(SHParseDisplayName(path, nullptr, &item, 0, nullptr)) || !item ||
            FAILED(SHOpenFolderAndSelectItems(item, 0, nullptr, 0)))
            MessageBoxW(popup, L"The executable location is unavailable.", L"MicroManager", MB_OK | MB_ICONINFORMATION);
        if (item) CoTaskMemFree(item);
    }
    CloseHandle(process);
}

static void ShowProcessMenu(HWND popup, int row, POINT point) {
    const ProcessTarget target = (row == 2) ? g_topRamTarget : (row == 1) ? g_topGpuTarget : g_topCpuTarget;
    if (!target.pid || !target.created) return;
    HMENU menu = CreatePopupMenu();
    if (!menu) return;
    WCHAR heading[128];
    swprintf_s(heading, L"%s (PID %lu)", target.name, target.pid);
    AppendMenuW(menu, MF_STRING, 0, heading);
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, MENU_PROC_END, L"End task");
    AppendMenuW(menu, MF_STRING, MENU_PROC_END_TREE, L"End process tree...");
    AppendMenuW(menu, MF_STRING, MENU_PROC_LOCATION, L"Open file location");
    SetForegroundWindow(popup);
    UINT action = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON, point.x, point.y, 0, popup, nullptr);
    PostMessageW(popup, WM_NULL, 0, 0);
    DestroyMenu(menu);
    if (action) RunProcessAction(popup, target, action);
}

static LRESULT CALLBACK PopupWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_ACTIVATE:
            if (LOWORD(wParam) == WA_INACTIVE) {
                static DWORD lastDeactivateTick = 0;
                DWORD now = GetTickCount();
                if (now - lastDeactivateTick > 200) {
                    lastDeactivateTick = now;
                    ShowWindow(hWnd, SW_HIDE);
                    g_lastPopupCloseTime = GetTickCount64();
                }
            }
            return 0;

        case WM_PAINT: {
            // Snapshot stats under the lock, then paint without holding it.
            int totals[POPUP_ROWS], topPcts[POPUP_ROWS];
            WCHAR topNames[POPUP_ROWS][64];
            totals[0] = g_totalCpu; topPcts[0] = g_topCpuPct; wcscpy_s(topNames[0], g_topCpuName);
            totals[1] = g_totalGpu; topPcts[1] = g_topGpuPct; wcscpy_s(topNames[1], g_topGpuName);
            totals[2] = g_totalRam; topPcts[2] = g_topRamPct; wcscpy_s(topNames[2], g_topRamName);

            static const PCWSTR kLabels[POPUP_ROWS]    = { L"CPU", L"GPU", L"RAM" };
            static const PCWSTR kEmptyText[POPUP_ROWS] = { L"Idle", L"Idle", L"\x2014" };
            const COLORREF kTotalColors[POPUP_ROWS] = {
                RGB(0, 200, 255), RGB(0, 220, 100), RGB(255, 180, 70) };

            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            HBRUSH bgBrush = CreateSolidBrush(RGB(32, 32, 32));
            RECT rc;
            GetClientRect(hWnd, &rc);
            FillRect(hdc, &rc, bgBrush);
            DeleteObject(bgBrush);

            HPEN borderPen = CreatePen(PS_SOLID, 1, RGB(64, 64, 64));
            HPEN oldPen = (HPEN)SelectObject(hdc, borderPen);
            HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
            Rectangle(hdc, 0, 0, rc.right, rc.bottom);
            SelectObject(hdc, oldPen);
            SelectObject(hdc, oldBrush);
            DeleteObject(borderPen);

            SetBkMode(hdc, TRANSPARENT);
            EnsureFont();
            HFONT oldFont = (HFONT)SelectObject(hdc, g_hPopupFont);

            for (int row = 0; row < POPUP_ROWS; row++) {
                int y = Sc(12) + row * Sc(32);
                if (row == g_actionRow) {
                    RECT selected = {Sc(8), y - Sc(3), rc.right - Sc(8), y + Sc(26)};
                    HBRUSH outline = CreateSolidBrush(RGB(78, 100, 120));
                    FrameRect(hdc, &selected, outline);
                    DeleteObject(outline);
                }
                int totalVal = totals[row];
                int topPctVal = topPcts[row];
                PCWSTR topName = topNames[row];

                SetTextColor(hdc, RGB(255, 255, 255));
                WCHAR labelBuf[16];
                swprintf_s(labelBuf, L"%s:", kLabels[row]);
                TextOutW(hdc, Sc(12), y, labelBuf, (int)wcslen(labelBuf));

                SetTextColor(hdc, kTotalColors[row]);
                WCHAR totalBuf[16];
                if (totalVal < 0)
                    swprintf_s(totalBuf, L"..%%");
                else
                    swprintf_s(totalBuf, L"%d%%", totalVal);
                TextOutW(hdc, Sc(60), y, totalBuf, (int)wcslen(totalBuf));

                SetTextColor(hdc, RGB(255, 255, 255));
                if (topName[0] != L'\0' && topPctVal > 0) {
                    WCHAR lineBuf[256];
                    swprintf_s(lineBuf, L"%s  %d%%", topName, topPctVal);
                    TextOutW(hdc, Sc(130), y, lineBuf, (int)wcslen(lineBuf));
                } else if (totalVal >= 0) {
                    TextOutW(hdc, Sc(130), y, kEmptyText[row], (int)wcslen(kEmptyText[row]));
                } else {
                    PCWSTR collecting = L"Collecting...";
                    TextOutW(hdc, Sc(130), y, collecting, (int)wcslen(collecting));
                }
            }

            static const PCWSTR kGraphLabels[POPUP_ROWS] = { L"CPU history", L"GPU history", L"RAM history" };
            PCWSTR graphLabel = (g_graphMetric >= 0 && g_graphMetric < POPUP_ROWS) ? kGraphLabels[g_graphMetric] : L"";
            SetTextColor(hdc, RGB(255, 255, 255));
            TextOutW(hdc, Sc(12), Sc(118), graphLabel, (int)wcslen(graphLabel));
            const int graphX = Sc(110), graphY = Sc(112);
            const int graphWidth = Sc(235), graphHeight = Sc(40);
            HBRUSH graphBg = CreateSolidBrush(RGB(44, 44, 44));
            RECT graphRect = {graphX, graphY, graphX + graphWidth, graphY + graphHeight};
            FillRect(hdc, &graphRect, graphBg);
            DeleteObject(graphBg);
            COLORREF barColor = (g_graphMetric >= 0 && g_graphMetric < POPUP_ROWS) ? kTotalColors[g_graphMetric] : RGB(0, 200, 255);
            HBRUSH barBrush = CreateSolidBrush(barColor);
            int m = g_graphMetric;
            if (m >= 0 && m < POPUP_ROWS) {
                int count = g_graphCount[m];
                int next = g_graphNext[m];
                for (int i = 0; i < count; i++) {
                    int value = g_graphSamples[m][(next + 16 - count + i) % 16];
                    if (value < 0) continue;  // failed reads remain visible gaps
                    int x1 = graphX + i * graphWidth / 16;
                    int x2 = graphX + (i + 1) * graphWidth / 16 - Sc(2);
                    int height = std::max(1, std::min(100, value) * graphHeight / 100);
                    RECT bar = {x1, graphY + graphHeight - height, x2, graphY + graphHeight};
                    FillRect(hdc, &bar, barBrush);
                }
            }
            DeleteObject(barBrush);

            SelectObject(hdc, oldFont);
            EndPaint(hWnd, &ps);
            return 0;
        }

        case WM_LBUTTONDOWN: {
            int y = (short)HIWORD(lParam);
            if (y < Sc(12)) return 0;
            int row = (y - Sc(12)) / Sc(32);
            if (row >= 0 && row < POPUP_ROWS) {
                g_actionRow = row;
                g_graphMetric = row;
                InvalidateRect(hWnd, nullptr, FALSE);
            } else if (y >= Sc(110) && y <= Sc(155)) {
                g_graphMetric = (g_graphMetric + 1) % POPUP_ROWS;
                g_actionRow = g_graphMetric;
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            return 0;
        }

        case WM_CONTEXTMENU: {
            POINT point = {(short)LOWORD(lParam), (short)HIWORD(lParam)};
            int row = g_actionRow;
            if (GET_X_LPARAM(lParam) == -1 && GET_Y_LPARAM(lParam) == -1) {
                RECT rect; GetWindowRect(hWnd, &rect);
                point.x = rect.left + Sc(160);
                point.y = rect.top + Sc(12 + row * 32);
            } else {
                POINT client = point; ScreenToClient(hWnd, &client);
                if (client.y < Sc(12)) return 0;
                int hit = (client.y - Sc(12)) / Sc(32);
                if (hit < 0 || hit >= POPUP_ROWS) return 0;
                row = hit;
            }
            g_actionRow = row;
            g_graphMetric = row;
            InvalidateRect(hWnd, nullptr, FALSE);
            ShowProcessMenu(hWnd, row, point);
            return 0;
        }

        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) ShowWindow(hWnd, SW_HIDE);
            if (wParam == VK_UP) {
                g_actionRow = (g_actionRow + POPUP_ROWS - 1) % POPUP_ROWS;
                g_graphMetric = g_actionRow;
                InvalidateRect(hWnd, nullptr, FALSE);
            } else if (wParam == VK_DOWN) {
                g_actionRow = (g_actionRow + 1) % POPUP_ROWS;
                g_graphMetric = g_actionRow;
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            break;

        case WM_DESTROY:
            InterlockedExchangePointer((PVOID*)&g_popupHwnd, nullptr);
            break;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

// ─── Show/Hide Popup ──────────────────────────────────────────────────────────

static void ShowPopup(HWND hTrayWnd) {
    if (!g_popupHwnd) {
        WNDCLASSW wc = {0};
        wc.lpfnWndProc = PopupWndProc;
        wc.hInstance = g_hInstance;
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.lpszClassName = L"MicroManagerPopupClass";
        wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
        if (!RegisterClassW(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
            Wh_Log(L"Popup RegisterClassW failed (%u)", GetLastError());
            return;
        }

        g_popupHwnd = CreateWindowExW(
            WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
            wc.lpszClassName, L"MicroManager",
            WS_POPUP | WS_BORDER,
            0, 0, Sc(POPUP_WIDTH), Sc(POPUP_HEIGHT),
            nullptr, nullptr, g_hInstance, nullptr);
        if (!g_popupHwnd) {
            UnregisterClassW(L"MicroManagerPopupClass", g_hInstance);
            return;
        }
    }

    if (!g_popupHwnd) return;

    // Refresh DPI for the monitor the icon lives on, then rebuild the font.
    UINT dpi = GetDpiForWindow(g_popupHwnd);
    g_dpi = dpi ? (int)dpi : 96;
    EnsureFont();

    NOTIFYICONIDENTIFIER nii = {sizeof(nii)};
    nii.hWnd = nullptr;
    nii.uID = 0;
    nii.guidItem = MICROMANAGER_TRAY_GUID;  // icon is registered with NIF_GUID
    RECT iconRect = {};
    int w = Sc(POPUP_WIDTH), h = Sc(POPUP_HEIGHT);
    int x = 0, y = 0;

    bool hasIconRect = SUCCEEDED(Shell_NotifyIconGetRect(&nii, &iconRect));
    if (hasIconRect) {
        x = iconRect.left - w + (iconRect.right - iconRect.left) / 2;
        y = iconRect.top - h - Sc(4);
    } else {
        POINT pt;
        GetCursorPos(&pt);
        x = pt.x - w / 2;
        y = pt.y - h - Sc(4);
    }

    // Clamp to monitor work area and flip below icon if taskbar is at the top
    HMONITOR hm = hasIconRect ? MonitorFromRect(&iconRect, MONITOR_DEFAULTTONEAREST)
                              : MonitorFromPoint(POINT{ x + w / 2, y + h / 2 }, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    if (GetMonitorInfoW(hm, &mi)) {
        const RECT& wa = mi.rcWork;
        if (x < wa.left)       x = wa.left + Sc(4);
        if (x + w > wa.right)  x = wa.right - w - Sc(4);
        if (y < wa.top) {
            y = hasIconRect ? (iconRect.bottom + Sc(4)) : (wa.top + Sc(4));
        }
        if (y + h > wa.bottom) y = wa.bottom - h - Sc(4);
    }

    g_actionRow = g_graphMetric;
    SetWindowPos(g_popupHwnd, HWND_TOPMOST, x, y, w, h, SWP_SHOWWINDOW);

    // Rounded corners on Windows 11 (silently ignored on Windows 10).
    int corner = DWMWCP_ROUND;
    DwmSetWindowAttribute(g_popupHwnd, DWMWA_WINDOW_CORNER_PREFERENCE,
        &corner, sizeof(corner));

    // Take foreground activation so a click anywhere outside the popup fires
    // WM_ACTIVATE/WA_INACTIVE and auto-hides it. SetFocus alone does not cross
    // the foreground boundary from the tray thread, so the popup would never
    // become "active" and would only close after being clicked first. This
    // mirrors AudioSwap's VolumePopup::Show.
    SetForegroundWindow(g_popupHwnd);
}

// ─── System Theme + Context Menu ─────────────────────────────────────────────

static HMODULE g_hUxTheme = nullptr;

static bool IsSystemDarkMode() {
    DWORD value = 1, size = sizeof(value);
    RegGetValueW(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &value, &size);
    return value == 0;
}

static void ApplyContextMenuTheme(HWND hWnd, bool dark) {
    if (!g_hUxTheme) {
        g_hUxTheme = LoadLibraryExW(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    }
    if (!g_hUxTheme) return;
    using Fn135 = int(WINAPI*)(int);
    using Fn133 = bool(WINAPI*)(HWND, bool);
    using Fn136 = void(WINAPI*)();
    if (auto f = (Fn135)GetProcAddress(g_hUxTheme, MAKEINTRESOURCEA(135))) f(dark ? 2 : 0);
    if (auto f = (Fn133)GetProcAddress(g_hUxTheme, MAKEINTRESOURCEA(133))) f(hWnd, dark);
    if (auto f = (Fn136)GetProcAddress(g_hUxTheme, MAKEINTRESOURCEA(136))) f();
}

static void SaveIntervalMs(DWORD ms) {
    Wh_SetIntValue(L"UpdateIntervalMs", (int)ms);
}

static DWORD LoadIntervalMs() {
    DWORD ms = (DWORD)Wh_GetIntValue(L"UpdateIntervalMs", 1000);
    if (ms != 300 && ms != 500 && ms != 1000 && ms != 3000) ms = 1000;
    return ms;
}

// ─── Tray Window Procedure ────────────────────────────────────────────────────

static LRESULT CALLBACK TrayWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            RefreshData();
            SetTimer(hWnd, WM_TIMER_ID, g_updateMs, nullptr);
            return 0;

        case WM_TIMER:
            if (wParam == WM_TIMER_ID) {
                RefreshData();
            }
            return 0;

        case WM_TRAY_CALLBACK:
            switch (LOWORD(lParam)) {
                case WM_LBUTTONUP:
                case NIN_SELECT:
                case NIN_KEYSELECT: {
                    ULONGLONG now = GetTickCount64();
                    if (g_popupHwnd && IsWindowVisible(g_popupHwnd)) {
                        // A version-4 tray click can deliver both mouse-up and selection.
                        if (now - g_lastPopupOpenTime < 250) break;
                        ShowWindow(g_popupHwnd, SW_HIDE);
                        g_lastPopupCloseTime = now;
                    } else if (now - g_lastPopupCloseTime > 200) {
                        ShowPopup(hWnd);
                        if (g_popupHwnd && IsWindowVisible(g_popupHwnd))
                            g_lastPopupOpenTime = now;
                    }
                    break;
                }

                case WM_CONTEXTMENU: {
                    HMENU hMenu = CreatePopupMenu();

                    WCHAR statusText[128];
                    int cpu = g_totalCpu;
                    int gpu = g_totalGpu;
                    int ram = g_totalRam;

                    if (cpu >= 0 && gpu >= 0 && ram >= 0)
                        swprintf_s(statusText, L"CPU: %d%%  GPU: %d%%  RAM: %d%%", cpu, gpu, ram);
                    else
                        lstrcpyW(statusText, L"Collecting...");
                    AppendMenuW(hMenu, MF_STRING, 0, statusText);
                    AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);

                    UINT chk300 = (g_updateMs == 300)  ? (MF_STRING | MF_CHECKED) : MF_STRING;
                    UINT chk500 = (g_updateMs == 500)  ? (MF_STRING | MF_CHECKED) : MF_STRING;
                    UINT chk1s  = (g_updateMs == 1000) ? (MF_STRING | MF_CHECKED) : MF_STRING;
                    UINT chk3s  = (g_updateMs == 3000) ? (MF_STRING | MF_CHECKED) : MF_STRING;
                    AppendMenuW(hMenu, chk300, MENU_INTERVAL_300MS, L"Refresh: 0.3s");
                    AppendMenuW(hMenu, chk500, MENU_INTERVAL_500MS, L"Refresh: 0.5s");
                    AppendMenuW(hMenu, chk1s,  MENU_INTERVAL_1S,    L"Refresh: 1s");
                    AppendMenuW(hMenu, chk3s,  MENU_INTERVAL_3S,    L"Refresh: 3s");
                    AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
                    AppendMenuW(hMenu, MF_STRING | (g_graphMetric == 0 ? MF_CHECKED : 0), MENU_GRAPH_CPU, L"Graph: CPU");
                    AppendMenuW(hMenu, MF_STRING | (g_graphMetric == 1 ? MF_CHECKED : 0), MENU_GRAPH_GPU, L"Graph: GPU");
                    AppendMenuW(hMenu, MF_STRING | (g_graphMetric == 2 ? MF_CHECKED : 0), MENU_GRAPH_RAM, L"Graph: RAM");
                    AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
                    AppendMenuW(hMenu, MF_STRING, MENU_OPEN_WINDHAWK, L"Open Windhawk");

                    POINT pt = { (short)LOWORD(wParam), (short)HIWORD(wParam) };
                    if (pt.x == 0 && pt.y == 0) {
                        GetCursorPos(&pt);
                    }
                    bool dark = IsSystemDarkMode();
                    ApplyContextMenuTheme(hWnd, dark);
                    SetForegroundWindow(hWnd);
                    int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_RIGHTBUTTON |
                        TPM_BOTTOMALIGN | TPM_RIGHTALIGN,
                        pt.x, pt.y, 0, hWnd, nullptr);
                    PostMessageW(hWnd, WM_NULL, 0, 0);
                    DestroyMenu(hMenu);

                    if (cmd == MENU_GRAPH_CPU || cmd == MENU_GRAPH_GPU || cmd == MENU_GRAPH_RAM) {
                        int metric = (cmd == MENU_GRAPH_GPU) ? 1 : (cmd == MENU_GRAPH_RAM) ? 2 : 0;
                        if (metric != g_graphMetric) {
                            g_graphMetric = metric;
                            g_actionRow = metric;
                            if (g_popupHwnd) InvalidateRect(g_popupHwnd, nullptr, FALSE);
                        }
                    }

                    DWORD newMs = 0;
                    if      (cmd == MENU_INTERVAL_300MS) newMs = 300;
                    else if (cmd == MENU_INTERVAL_500MS) newMs = 500;
                    else if (cmd == MENU_INTERVAL_1S)    newMs = 1000;
                    else if (cmd == MENU_INTERVAL_3S)    newMs = 3000;
                    else if (cmd == MENU_OPEN_WINDHAWK) {
                        if (g_windhawkPath[0] != L'\0') {
                            SHELLEXECUTEINFOW sei = {sizeof(sei)};
                            sei.lpFile = g_windhawkPath;
                            sei.nShow  = SW_SHOWNORMAL;
                            ShellExecuteExW(&sei);
                        }
                    }

                    if (newMs > 0 && newMs != g_updateMs) {
                        g_updateMs = newMs;
                        memset(g_graphCount, 0, sizeof(g_graphCount));
                        memset(g_graphNext, 0, sizeof(g_graphNext));
                        KillTimer(hWnd, WM_TIMER_ID);
                        SetTimer(hWnd, WM_TIMER_ID, newMs, nullptr);
                        SaveIntervalMs(newMs);
                    }
                    break;
                }
            }
            return 0;

        case WM_CLOSE:
            // Destroy popup on the tray thread (its owner) before destroying the tray window
            if (g_popupHwnd) { DestroyWindow(g_popupHwnd); g_popupHwnd = nullptr; }
            DestroyWindow(hWnd);
            return 0;

        case WM_DESTROY:
            KillTimer(hWnd, WM_TIMER_ID);
            {
                NOTIFYICONDATAW nid = {sizeof(nid)};
                nid.hWnd     = hWnd;
                nid.uID      = TRAY_ICON_ID;
                nid.uFlags   = NIF_GUID;
                nid.guidItem = MICROMANAGER_TRAY_GUID;
                Shell_NotifyIconW(NIM_DELETE, &nid);
            }
            PostQuitMessage(0);
            return 0;
    }

    // Re-add tray icon after Explorer restarts
    if (msg == g_taskbarCreatedMsg && g_taskbarCreatedMsg != 0) {
        g_lastTip[0] = L'\0';  // force the tooltip to be re-pushed on next refresh
        NOTIFYICONDATAW nid = {sizeof(nid)};
        nid.hWnd            = hWnd;
        nid.uID             = TRAY_ICON_ID;
        nid.uFlags          = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_GUID | NIF_SHOWTIP;
        nid.guidItem        = MICROMANAGER_TRAY_GUID;
        nid.uCallbackMessage = WM_TRAY_CALLBACK;
        lstrcpynW(nid.szTip, L"MicroManager", 128);
        if (!g_iconEnabled)
            ExtractIconExW(g_ddoresDllPath, 28, nullptr, &g_iconEnabled, 1);
        nid.hIcon = g_iconEnabled ? g_iconEnabled : LoadIconW(nullptr, IDI_APPLICATION);
        if (!Shell_NotifyIconW(NIM_ADD, &nid)) {
            Shell_NotifyIconW(NIM_DELETE, &nid);
            Shell_NotifyIconW(NIM_ADD, &nid);
        }
        NOTIFYICONDATAW nidVer = {sizeof(nidVer)};
        nidVer.hWnd     = hWnd;
        nidVer.uID      = TRAY_ICON_ID;
        nidVer.uFlags   = NIF_GUID;
        nidVer.guidItem = MICROMANAGER_TRAY_GUID;
        nidVer.uVersion = NOTIFYICON_VERSION_4;
        Shell_NotifyIconW(NIM_SETVERSION, &nidVer);
        return 0;
    }

    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

// ─── Tray Thread ──────────────────────────────────────────────────────────────

static DWORD WINAPI TrayThreadProc(LPVOID) {
    // Per-monitor DPI awareness so the popup renders crisply on scaled displays.
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hrCo) && hrCo != RPC_E_CHANGED_MODE) {
        Wh_Log(L"TrayThread: CoInitializeEx failed (0x%X)", hrCo);
        return 1;
    }

    g_taskbarCreatedMsg = RegisterWindowMessageW(L"TaskbarCreated");

    WNDCLASSW wc = {0};
    wc.lpfnWndProc = TrayWndProc;
    wc.hInstance = g_hInstance;
    wc.lpszClassName = L"MicroManagerTrayClass";
    if (!RegisterClassW(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        Wh_Log(L"Tray RegisterClassW failed (%u)", GetLastError());
        if (SUCCEEDED(hrCo)) CoUninitialize();
        return 1;
    }

    InterlockedExchangePointer((PVOID*)&g_trayHwnd, CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        wc.lpszClassName, L"MicroManager",
        WS_POPUP,
        0, 0, 1, 1, nullptr, nullptr, g_hInstance, nullptr));
    if (!g_trayHwnd) {
        if (SUCCEEDED(hrCo)) CoUninitialize();
        return 1;
    }

    // Unique AUMID so the OS doesn't group this icon with Windhawk's main window
    IPropertyStore* pps = nullptr;
    if (SUCCEEDED(SHGetPropertyStoreForWindow(g_trayHwnd, IID_PPV_ARGS(&pps)))) {
        wchar_t aumid[] = L"BlackPaw.MicroManager";
        PROPVARIANT var;
        PropVariantInit(&var);
        var.vt = VT_LPWSTR;
        var.pwszVal = aumid;
        if (SUCCEEDED(pps->SetValue(PKEY_AppUserModel_ID, var))) {
            pps->Commit();
        }
        pps->Release();
    }

    NOTIFYICONDATAW nid = {sizeof(nid)};
    nid.hWnd            = g_trayHwnd;
    nid.uID             = TRAY_ICON_ID;
    nid.uFlags          = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_GUID | NIF_SHOWTIP;
    nid.guidItem        = MICROMANAGER_TRAY_GUID;
    nid.uCallbackMessage = WM_TRAY_CALLBACK;
    lstrcpynW(nid.szTip, L"MicroManager", 128);
    nid.hIcon = g_iconEnabled ? g_iconEnabled : LoadIconW(nullptr, IDI_APPLICATION);
    if (!Shell_NotifyIconW(NIM_ADD, &nid)) {
        Shell_NotifyIconW(NIM_DELETE, &nid);
        Shell_NotifyIconW(NIM_ADD, &nid);
    }
    NOTIFYICONDATAW nidVer = {sizeof(nidVer)};
    nidVer.hWnd     = g_trayHwnd;
    nidVer.uID      = TRAY_ICON_ID;
    nidVer.uFlags   = NIF_GUID;
    nidVer.guidItem = MICROMANAGER_TRAY_GUID;
    nidVer.uVersion = NOTIFYICON_VERSION_4;
    Shell_NotifyIconW(NIM_SETVERSION, &nidVer);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    InterlockedExchangePointer((PVOID*)&g_trayHwnd, nullptr);
    if (SUCCEEDED(hrCo)) CoUninitialize();
    return 0;
}

// ─── Windhawk Callbacks ───────────────────────────────────────────────────────

BOOL WhTool_ModInit() {
    Wh_Log(L"MicroManager Init");

    g_updateMs = LoadIntervalMs();

    MEMORYSTATUSEX mem = {sizeof(mem)};
    if (GlobalMemoryStatusEx(&mem)) g_totalPhys = mem.ullTotalPhys;

    g_hInstance = GetModuleHandleW(nullptr);
    WCHAR modulePath[MAX_PATH];
    switch (GetModuleFileNameW(g_hInstance, modulePath, ARRAYSIZE(modulePath))) {
        case 0:
        case ARRAYSIZE(modulePath):
            Wh_Log(L"GetModuleFileNameW failed");
            g_windhawkPath[0] = L'\0';
            g_windhawkModPath[0] = L'\0';
            break;
        default: {
            WCHAR* lastSlash = wcsrchr(modulePath, L'\\');
            if (lastSlash) {
                *lastSlash = L'\0';
                swprintf_s(g_windhawkPath, L"%s\\windhawk.exe", modulePath);
                swprintf_s(g_windhawkModPath, L"%s\\windhawk-mod.exe", modulePath);
            } else {
                wcscpy_s(g_windhawkPath, L"windhawk.exe");
                wcscpy_s(g_windhawkModPath, L"windhawk-mod.exe");
            }
            break;
        }
    }

    // Full path for ddores.dll — ExtractIconExW handles the .mun redirect on Win11
    UINT sysLen = GetSystemDirectoryW(g_ddoresDllPath, MAX_PATH);
    if (sysLen > 0 && sysLen < MAX_PATH - 12)
        lstrcatW(g_ddoresDllPath, L"\\ddores.dll");
    else
        lstrcpyW(g_ddoresDllPath, L"ddores.dll");

    ExtractIconExW(g_ddoresDllPath, 28, nullptr, &g_iconEnabled, 1);

    InitGpuQuery();

    // Baseline CPU sample so the first timer tick has a delta to work from
    GetSystemTimes(&g_prevIdle, &g_prevKernel, &g_prevUser);
    MY_SYSTEM_PROCESS_INFO* initBuf = nullptr;
    if (CollectProcessInfo(&initBuf) && initBuf) {
        MY_SYSTEM_PROCESS_INFO* p = initBuf;
        int count = 0;
        while (count < MAX_PROCESSES) {
            g_prevProcs[count].pid = (DWORD)(ULONG_PTR)p->UniqueProcessId;
            g_prevProcs[count].created = p->CreateTime.QuadPart;
            g_prevProcs[count].time = p->KernelTime.QuadPart + p->UserTime.QuadPart;
            GetProcessDisplayName(p, g_prevProcs[count].name, ARRAYSIZE(g_prevProcs[count].name));
            count++;
            if (p->NextEntryOffset == 0) break;
            p = (MY_SYSTEM_PROCESS_INFO*)((BYTE*)p + p->NextEntryOffset);
        }
        g_prevProcCount = count;
        free(initBuf);
    }

    g_trayThread = CreateThread(nullptr, 0, TrayThreadProc, nullptr, 0, nullptr);
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    // Refresh rate is managed via the right-click menu; nothing to read here.
}

void WhTool_ModUninit() {
    Wh_Log(L"MicroManager Mod Uninit");

    // WM_CLOSE handler on the tray thread destroys g_popupHwnd before the tray
    // window itself — no cross-thread DestroyWindow needed here
    HWND hwndClose = (HWND)InterlockedCompareExchangePointer((PVOID*)&g_trayHwnd, nullptr, nullptr);
    if (hwndClose && IsWindow(hwndClose)) PostMessageW(hwndClose, WM_CLOSE, 0, 0);
    if (g_trayThread) {
        WaitForSingleObject(g_trayThread, 3000);
        CloseHandle(g_trayThread);
        g_trayThread = nullptr;
    }

    // Unregister popup class after the tray thread has fully exited so a
    // subsequent mod reload can re-register it cleanly
    UnregisterClassW(L"MicroManagerPopupClass", g_hInstance);
    UnregisterClassW(L"MicroManagerTrayClass", g_hInstance);

    if (g_iconEnabled) { DestroyIcon(g_iconEnabled); g_iconEnabled = nullptr; }
    if (g_hPopupFont) { DeleteObject(g_hPopupFont); g_hPopupFont = nullptr; }
    if (g_gpuQuery) { PdhCloseQuery(g_gpuQuery); g_gpuQuery = nullptr; g_gpuCounter = nullptr; }
    if (g_hUxTheme) { FreeLibrary(g_hUxTheme); g_hUxTheme = nullptr; }
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
