// ==WindhawkMod==
// @id              micromanager
// @name            MicroManager
// @description     Mini task manager tray icon showing CPU, GPU and RAM usage with top consumers.
// @version         1.0.0
// @author          BlackPaw
// @github          https://github.com/BlackPaw21
// @donateUrl       https://ko-fi.com/blackpaw21
// @include         windhawk.exe
// @compilerOptions -lpdh -lshell32 -lgdi32 -luser32 -lole32 -luuid -ladvapi32 -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# MicroManager

![Screenshot](https://i.imgur.com/yDHoq9N.png)

A lightweight tray icon that shows a mini task manager popup with live CPU, GPU
and RAM usage, plus the single top-consuming process for each.

## How to Use

1. **Left-click** the tray icon to open the popup showing:
   - Total CPU usage and the top CPU-consuming process
   - Total GPU usage and the top GPU-consuming process
   - Total RAM usage and the top RAM-consuming process
2. **Click anywhere outside** the popup (or press **Esc**) to close it
3. **Hover** the tray icon to see a live `CPU / GPU / RAM` summary tooltip
4. **Right-click** the tray icon for options

## Configuration

Right-click the tray icon to change the refresh rate (0.3s / 0.5s / 1s / 3s).

## Changelog

### v1.0.0
- Initial release.
- Left-click to see live CPU and GPU usage with the top process for each.
- Right-click for options. Update interval adjustable in Settings.
*/
// ==/WindhawkModReadme==

#define NOMINMAX
#include <windows.h>
#include <shellapi.h>
#include <shobjidl.h>
#include <propkey.h>
#include <dwmapi.h>
#include <pdh.h>
#include <tlhelp32.h>
#include <stdlib.h>
#include <stdio.h>
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

// Base (96-DPI) popup geometry — scaled at runtime via Sc().
#define POPUP_WIDTH         360
#define POPUP_HEIGHT        112
#define POPUP_ROWS          3

#define MAX_PROCESSES       1024
#define PROCESS_BUF_SIZE    (512 * 1024)

// Re-attempt GPU (PDH) initialization roughly every this many ticks if it fails,
// instead of disabling GPU stats permanently for the session.
#define GPU_RETRY_TICKS     30

#define MENU_OPEN_WINDHAWK   9000
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
static volatile HWND       g_trayHwnd     = nullptr;
static HWND                g_popupHwnd    = nullptr;
static ULONGLONG           g_lastPopupCloseTime = 0;
static HINSTANCE           g_hInstance    = nullptr;
static WCHAR               g_windhawkPath[MAX_PATH] = {};
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

    ULONG bufSize = PROCESS_BUF_SIZE;
    *outBuf = (MY_SYSTEM_PROCESS_INFO*)malloc(bufSize);
    if (!*outBuf) return 0;

    NTSTATUS status = NtQuery(SystemProcessInformation, *outBuf, bufSize, &bufSize);
    if (status == STATUS_INFO_LENGTH_MISMATCH) {
        free(*outBuf);
        bufSize *= 2;
        *outBuf = (MY_SYSTEM_PROCESS_INFO*)malloc(bufSize);
        if (!*outBuf) return 0;
        status = NtQuery(SystemProcessInformation, *outBuf, bufSize, &bufSize);
    }
    if (status < 0) {
        free(*outBuf);
        *outBuf = nullptr;
        return 0;
    }
    return 1;
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

static void CollectGpuStats(int* outTotal, int* outTopPct, WCHAR* outTopName, int nameLen) {
    *outTotal = -1;
    *outTopPct = 0;
    outTopName[0] = L'\0';

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

        for (int i = 0; i < g_prevProcCount; i++) {
            if (g_prevProcs[i].pid == g_gpuProcs[topIdx].pid) {
                wcscpy_s(outTopName, nameLen, g_prevProcs[i].name);
                break;
            }
        }
    }
}

// ─── Data Refresh ─────────────────────────────────────────────────────────────

static void RefreshData() {
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
    FILETIME nowIdle, nowKernel, nowUser;
    GetSystemTimes(&nowIdle, &nowKernel, &nowUser);

    ULARGE_INTEGER ui, uk, uu, pi, pk, pu;
    ui.LowPart = nowIdle.dwLowDateTime;     ui.HighPart = nowIdle.dwHighDateTime;
    uk.LowPart = nowKernel.dwLowDateTime;   uk.HighPart = nowKernel.dwHighDateTime;
    uu.LowPart = nowUser.dwLowDateTime;     uu.HighPart = nowUser.dwHighDateTime;
    pi.LowPart = g_prevIdle.dwLowDateTime;  pi.HighPart = g_prevIdle.dwHighDateTime;
    pk.LowPart = g_prevKernel.dwLowDateTime; pk.HighPart = g_prevKernel.dwHighDateTime;
    pu.LowPart = g_prevUser.dwLowDateTime;  pu.HighPart = g_prevUser.dwHighDateTime;

    double totalDelta = (double)(uk.QuadPart - pk.QuadPart) + (double)(uu.QuadPart - pu.QuadPart);
    double idleDelta = (double)(ui.QuadPart - pi.QuadPart);

    if (g_hasPrevSample && totalDelta > 0) {
        newTotalCpu = (int)(100.0 - 100.0 * idleDelta / totalDelta + 0.5);

        MY_SYSTEM_PROCESS_INFO* buf = nullptr;
        if (CollectProcessInfo(&buf) && buf) {
            MY_SYSTEM_PROCESS_INFO* p = buf;
            LONGLONG bestTime = 0;
            WCHAR bestCpuName[64] = {};
            ULONGLONG bestWs = 0;
            WCHAR bestRamName[64] = {};
            int count = 0;

            while (true) {
                LONGLONG curTime = p->KernelTime.QuadPart + p->UserTime.QuadPart;

                LONGLONG prevTime = 0;
                BOOL foundInPrev = FALSE;
                for (int i = 0; i < g_prevProcCount; i++) {
                    if (g_prevProcs[i].pid == (DWORD)(ULONG_PTR)p->UniqueProcessId) {
                        prevTime = g_prevProcs[i].time;
                        foundInPrev = TRUE;
                        break;
                    }
                }
                // Only count delta for processes seen last tick; new/overflow processes
                // would otherwise show their entire boot-time CPU as a single-tick spike.
                LONGLONG delta = foundInPrev ? (curTime - prevTime) : 0;
                if (delta > bestTime && p->ImageName.Buffer) {
                    bestTime = delta;
                    wcsncpy_s(bestCpuName, p->ImageName.Buffer,
                        MIN(p->ImageName.Length / sizeof(WCHAR), 63));
                    bestCpuName[63] = L'\0';
                }

                // Top RAM consumer by working set (absolute — no prior sample needed).
                if (p->ImageName.Buffer && (ULONGLONG)p->WorkingSetSize > bestWs) {
                    bestWs = (ULONGLONG)p->WorkingSetSize;
                    wcsncpy_s(bestRamName, p->ImageName.Buffer,
                        MIN(p->ImageName.Length / sizeof(WCHAR), 63));
                    bestRamName[63] = L'\0';
                }

                if (count < MAX_PROCESSES) {
                    g_curProcs[count].pid = (DWORD)(ULONG_PTR)p->UniqueProcessId;
                    g_curProcs[count].time = curTime;
                    if (p->ImageName.Buffer) {
                        wcsncpy_s(g_curProcs[count].name, p->ImageName.Buffer,
                            MIN(p->ImageName.Length / sizeof(WCHAR), 63));
                        g_curProcs[count].name[63] = L'\0';
                    } else {
                        g_curProcs[count].name[0] = L'\0';
                    }
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
                }
            }

            if (bestWs > 0 && g_totalPhys > 0) {
                int pct = (int)((bestWs * 100ULL) / g_totalPhys);
                newTopRamPct = pct < 1 ? 1 : pct;  // the top consumer is always shown
                wcscpy_s(newTopRamName, bestRamName);
            }

            free(buf);
        }
    }

    g_prevIdle = nowIdle; g_prevKernel = nowKernel; g_prevUser = nowUser;
    g_hasPrevSample = TRUE;

    CollectGpuStats(&newTotalGpu, &newTopGpuPct, newTopGpuName, 64);

    g_totalCpu = newTotalCpu;
    g_topCpuPct = newTopCpuPct;
    wcscpy_s(g_topCpuName, newTopCpuName);
    g_totalGpu = newTotalGpu;
    g_topGpuPct = newTopGpuPct;
    wcscpy_s(g_topGpuName, newTopGpuName);
    g_totalRam = newTotalRam;
    g_topRamPct = newTopRamPct;
    wcscpy_s(g_topRamName, newTopRamName);

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
            nid.uFlags   = NIF_TIP | NIF_GUID;
            nid.guidItem = MICROMANAGER_TRAY_GUID;
            lstrcpynW(nid.szTip, tip, 128);
            Shell_NotifyIconW(NIM_MODIFY, &nid);
        }
    }
}

// ─── Popup Window Procedure ───────────────────────────────────────────────────

static LRESULT CALLBACK PopupWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_ACTIVATE:
            if (LOWORD(wParam) == WA_INACTIVE) {
                ShowWindow(hWnd, SW_HIDE);
                g_lastPopupCloseTime = GetTickCount64();
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
            SelectObject(hdc, g_hPopupFont);

            for (int row = 0; row < POPUP_ROWS; row++) {
                int y = Sc(12) + row * Sc(32);
                int totalVal = totals[row];
                int topPctVal = topPcts[row];
                PCWSTR topName = topNames[row];

                SetTextColor(hdc, RGB(140, 140, 140));
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

                SetTextColor(hdc, RGB(220, 220, 220));
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

            EndPaint(hWnd, &ps);
            return 0;
        }

        case WM_NCHITTEST: {
            LRESULT hit = DefWindowProcW(hWnd, msg, wParam, lParam);
            if (hit == HTCLIENT) return HTCAPTION;
            return hit;
        }

        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) ShowWindow(hWnd, SW_HIDE);
            break;

        case WM_DESTROY:
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
    nii.hWnd = hTrayWnd;
    nii.uID = TRAY_ICON_ID;
    nii.guidItem = MICROMANAGER_TRAY_GUID;  // icon is registered with NIF_GUID
    RECT iconRect;
    int w = Sc(POPUP_WIDTH), h = Sc(POPUP_HEIGHT);
    int x, y;

    if (SUCCEEDED(Shell_NotifyIconGetRect(&nii, &iconRect))) {
        x = iconRect.left - w + (iconRect.right - iconRect.left) / 2;
        y = iconRect.top - h - Sc(4);
    } else {
        POINT pt;
        GetCursorPos(&pt);
        x = pt.x - w / 2;
        y = pt.y - h - Sc(4);
    }

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

static bool IsSystemDarkMode() {
    DWORD value = 1, size = sizeof(value);
    RegGetValueW(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &value, &size);
    return value == 0;
}

static void ApplyContextMenuTheme(HWND hWnd, bool dark) {
    HMODULE ux = GetModuleHandleW(L"uxtheme.dll");
    if (!ux) return;
    using Fn135 = int(WINAPI*)(int);
    using Fn133 = bool(WINAPI*)(HWND, bool);
    using Fn136 = void(WINAPI*)();
    if (auto f = (Fn135)GetProcAddress(ux, MAKEINTRESOURCEA(135))) f(dark ? 2 : 0);
    if (auto f = (Fn133)GetProcAddress(ux, MAKEINTRESOURCEA(133))) f(hWnd, dark);
    if (auto f = (Fn136)GetProcAddress(ux, MAKEINTRESOURCEA(136))) f();
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
                    if (g_popupHwnd && IsWindowVisible(g_popupHwnd)) {
                        ShowWindow(g_popupHwnd, SW_HIDE);
                        g_lastPopupCloseTime = GetTickCount64();
                    } else {
                        if (GetTickCount64() - g_lastPopupCloseTime > 200) {
                            ShowPopup(hWnd);
                        }
                    }
                    break;

                case WM_RBUTTONUP: {
                    HMENU hMenu = CreatePopupMenu();

                    WCHAR statusText[128];
                    int cpu = g_totalCpu;
                    int gpu = g_totalGpu;
                    int ram = g_totalRam;

                    if (cpu >= 0 && gpu >= 0 && ram >= 0)
                        swprintf_s(statusText, L"CPU: %d%%  GPU: %d%%  RAM: %d%%", cpu, gpu, ram);
                    else
                        lstrcpyW(statusText, L"Collecting...");
                    AppendMenuW(hMenu, MF_STRING | MF_GRAYED, 0, statusText);
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
                    AppendMenuW(hMenu, MF_STRING, MENU_OPEN_WINDHAWK, L"Open Windhawk");

                    POINT pt;
                    GetCursorPos(&pt);
                    bool dark = IsSystemDarkMode();
                    ApplyContextMenuTheme(hWnd, dark);
                    SetForegroundWindow(hWnd);
                    int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_RIGHTBUTTON |
                        TPM_BOTTOMALIGN | TPM_RIGHTALIGN,
                        pt.x, pt.y, 0, hWnd, nullptr);
                    PostMessageW(hWnd, WM_NULL, 0, 0);
                    DestroyMenu(hMenu);

                    DWORD newMs = 0;
                    if      (cmd == MENU_INTERVAL_300MS) newMs = 300;
                    else if (cmd == MENU_INTERVAL_500MS) newMs = 500;
                    else if (cmd == MENU_INTERVAL_1S)    newMs = 1000;
                    else if (cmd == MENU_INTERVAL_3S)    newMs = 3000;
                    else if (cmd == MENU_OPEN_WINDHAWK) {
                        SHELLEXECUTEINFOW sei = {sizeof(sei)};
                        sei.lpFile = g_windhawkPath;
                        sei.nShow  = SW_SHOWNORMAL;
                        ShellExecuteExW(&sei);
                    }

                    if (newMs > 0 && newMs != g_updateMs) {
                        g_updateMs = newMs;
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
        nid.uFlags          = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_GUID;
        nid.guidItem        = MICROMANAGER_TRAY_GUID;
        nid.uCallbackMessage = WM_TRAY_CALLBACK;
        lstrcpynW(nid.szTip, L"MicroManager", 128);
        if (!g_iconEnabled)
            ExtractIconExW(g_ddoresDllPath, 28, nullptr, &g_iconEnabled, 1);
        nid.hIcon = g_iconEnabled ? g_iconEnabled : LoadIconW(nullptr, IDI_APPLICATION);
        if (Shell_NotifyIconW(NIM_ADD, &nid)) {
            NOTIFYICONDATAW nidVer = {sizeof(nidVer)};
            nidVer.hWnd     = hWnd;
            nidVer.uID      = TRAY_ICON_ID;
            nidVer.uFlags   = NIF_GUID;
            nidVer.guidItem = MICROMANAGER_TRAY_GUID;
            nidVer.uVersion = NOTIFYICON_VERSION_4;
            Shell_NotifyIconW(NIM_SETVERSION, &nidVer);
        }
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

    g_trayHwnd = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        wc.lpszClassName, L"MicroManager",
        WS_POPUP,
        0, 0, 1, 1, nullptr, nullptr, g_hInstance, nullptr);
    if (!g_trayHwnd) {
        if (SUCCEEDED(hrCo)) CoUninitialize();
        return 1;
    }

    // Unique AUMID so the OS doesn't group this icon with Windhawk's main window
    IPropertyStore* pps = nullptr;
    if (SUCCEEDED(SHGetPropertyStoreForWindow(g_trayHwnd, IID_PPV_ARGS(&pps)))) {
        PROPVARIANT var;
        PropVariantInit(&var);
        var.vt      = VT_LPWSTR;
        var.pwszVal = (LPWSTR)CoTaskMemAlloc(MAX_PATH * sizeof(WCHAR));
        if (var.pwszVal) {
            lstrcpyW(var.pwszVal, L"BlackPaw.MicroManager");
            pps->SetValue(PKEY_AppUserModel_ID, var);
            CoTaskMemFree(var.pwszVal);
        }
        pps->Commit();
        pps->Release();
    }

    NOTIFYICONDATAW nid = {sizeof(nid)};
    nid.hWnd            = g_trayHwnd;
    nid.uID             = TRAY_ICON_ID;
    nid.uFlags          = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_GUID;
    nid.guidItem        = MICROMANAGER_TRAY_GUID;
    nid.uCallbackMessage = WM_TRAY_CALLBACK;
    lstrcpynW(nid.szTip, L"MicroManager", 128);
    nid.hIcon = g_iconEnabled ? g_iconEnabled : LoadIconW(nullptr, IDI_APPLICATION);
    if (Shell_NotifyIconW(NIM_ADD, &nid)) {
        NOTIFYICONDATAW nidVer = {sizeof(nidVer)};
        nidVer.hWnd     = g_trayHwnd;
        nidVer.uID      = TRAY_ICON_ID;
        nidVer.uFlags   = NIF_GUID;
        nidVer.guidItem = MICROMANAGER_TRAY_GUID;
        nidVer.uVersion = NOTIFYICON_VERSION_4;
        Shell_NotifyIconW(NIM_SETVERSION, &nidVer);
    }

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    g_trayHwnd = nullptr;
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
    GetModuleFileNameW(g_hInstance, g_windhawkPath, MAX_PATH);

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
            g_prevProcs[count].time = p->KernelTime.QuadPart + p->UserTime.QuadPart;
            if (p->ImageName.Buffer) {
                wcsncpy_s(g_prevProcs[count].name, p->ImageName.Buffer,
                    MIN(p->ImageName.Length / sizeof(WCHAR), 63));
                g_prevProcs[count].name[63] = L'\0';
            } else {
                g_prevProcs[count].name[0] = L'\0';
            }
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
    if (g_trayHwnd) PostMessageW((HWND)g_trayHwnd, WM_CLOSE, 0, 0);
    if (g_trayThread) {
        WaitForSingleObject(g_trayThread, 3000);
        CloseHandle(g_trayThread);
        g_trayThread = nullptr;
    }

    // Unregister popup class after the tray thread has fully exited so a
    // subsequent mod reload can re-register it cleanly
    UnregisterClassW(L"MicroManagerPopupClass", g_hInstance);

    if (g_iconEnabled) { DestroyIcon(g_iconEnabled); g_iconEnabled = nullptr; }
    if (g_hPopupFont) { DeleteObject(g_hPopupFont); g_hPopupFont = nullptr; }
    if (g_gpuQuery) { PdhCloseQuery(g_gpuQuery); g_gpuQuery = nullptr; g_gpuCounter = nullptr; }
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
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
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
            CreateMutexW(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
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
            (IMAGE_DOS_HEADER*)GetModuleHandleW(nullptr);
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
    switch (GetModuleFileNameW(nullptr, currentProcessPath,
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

    HMODULE kernelModule = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandleW(L"kernel32.dll");
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

    STARTUPINFOW si{
        .cb = sizeof(STARTUPINFOW),
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
