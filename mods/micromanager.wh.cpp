// ==WindhawkMod==
// @id              micromanager
// @name            MicroManager
// @description     Mini task manager tray icon showing CPU/GPU usage and top consumers.
// @version         1.1.0
// @author          BlackPaw
// @github          https://github.com/BlackPaw21
// @include         windhawk.exe
// @compilerOptions -lpdh -lshell32 -lgdi32 -luser32 -lole32 -luuid -DUNICODE -D_UNICODE
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- updateInterval: 2
  $name: Update Interval (s)
  $description: How often to refresh CPU/GPU stats
  $options:
    - 1: Fast (1s)
    - 2: Normal (2s)
    - 3: Relaxed (3s)
    - 5: Slow (5s)
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
# MicroManager

A lightweight tray icon that shows a mini task manager popup with live CPU and GPU usage stats.

## How to Use

1. **Left-click** the tray icon to open the popup showing:
   - Total CPU usage and the top CPU-consuming process
   - Total GPU usage and the top GPU-consuming process
2. **Click again** to close it
3. **Right-click** the tray icon for options

## Configuration

The update interval can be adjusted in the Settings tab.

## Changelog

### v1.1.0
- Fixed: boilerplate corrected so the mod actually runs
- Fixed: "Open WindHawk" right-click item now works
- Fixed: popup destroyed on the correct thread during shutdown
- Fixed: CoInitialize/CoUninitialize now properly paired on tray thread
- Fixed: tray icon reappears after Explorer restart (TaskbarCreated)
- Fixed: tray window style changed to WS_POPUP / WS_EX_TOOLWINDOW
- Fixed: AUMID set so icon doesn't group with Windhawk's own icon
- Fixed: popup class unregistered on mod uninit for clean reload
- Fixed: ddores.dll loaded from full system32 path
- Fixed: GPU PDH counter primed on init so first tick shows real data

### v1.0.0
- Initial release.
*/
// ==/WindhawkModReadme==

#define NOMINMAX
#include <windows.h>
#include <shellapi.h>
#include <shobjidl.h>
#include <propkey.h>
#include <pdh.h>
#include <tlhelp32.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

// PDH constants that may not be defined in all SDK versions
#ifndef PDH_MORE_DATA
#define PDH_MORE_DATA ((PDH_STATUS)0x800007D2L)
#endif
#ifndef PDH_CSTATUS_VALID_DATA
#define PDH_CSTATUS_VALID_DATA ((LONG)0x00000000L)
#endif

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

// ─── Constants ────────────────────────────────────────────────────────────────

#define TRAY_ICON_ID        1
#define WM_TRAY_CALLBACK    (WM_USER + 1)
#define WM_REFRESH_DATA     (WM_USER + 2)
#define WM_UPDATE_POPUP     (WM_USER + 3)
#define WM_TIMER_ID         1

#define POPUP_WIDTH         360
#define POPUP_HEIGHT        80

#define MAX_PROCESSES       1024
#define PROCESS_BUF_SIZE    (512 * 1024)

#define MENU_OPEN_WINDHAWK  9000

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
static HINSTANCE           g_hInstance    = nullptr;
static WCHAR               g_windhawkPath[MAX_PATH] = {};
static WCHAR               g_ddoresDllPath[MAX_PATH] = {};

static DWORD               g_updateMs     = 2000;

// Cached stats (updated on timer, read on popup paint)
static CRITICAL_SECTION    g_statsLock;
static int                 g_totalCpu     = -1;
static int                 g_topCpuPct    = 0;
static WCHAR               g_topCpuName[64] = {};
static int                 g_totalGpu     = -1;
static int                 g_topGpuPct    = 0;
static WCHAR               g_topGpuName[64] = {};

// GPU PDH handles
static PDH_HQUERY          g_gpuQuery     = nullptr;
static PDH_HCOUNTER        g_gpuCounter   = nullptr;
static BOOL                g_gpuFailed    = FALSE;

// Previous sample for CPU delta
static FILETIME            g_prevIdle     = {};
static FILETIME            g_prevKernel   = {};
static FILETIME            g_prevUser     = {};
static struct {
    DWORD  pid;
    LONGLONG time;
    WCHAR  name[64];
} g_prevProcs[MAX_PROCESSES];
static int                 g_prevProcCount = 0;
static BOOL                g_hasPrevSample = FALSE;

static HICON               g_iconEnabled  = nullptr;
static HFONT               g_hPopupFont   = nullptr;

static UINT                g_taskbarCreatedMsg = 0;

// ─── Font Helpers ─────────────────────────────────────────────────────────────

static void EnsureFont() {
    if (!g_hPopupFont) {
        g_hPopupFont = CreateFontW(-13, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    }
}

// ─── CPU Sampling ─────────────────────────────────────────────────────────────

static NtQuerySystemInformation_t LoadNtQuery() {
    HMODULE h = GetModuleHandleW(L"ntdll.dll");
    if (!h) return nullptr;
    return (NtQuerySystemInformation_t)GetProcAddress(h, "NtQuerySystemInformation");
}

static int CollectProcessInfo(MY_SYSTEM_PROCESS_INFO** outBuf) {
    NtQuerySystemInformation_t NtQuery = LoadNtQuery();
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
    if (g_gpuFailed) return;

    PDH_STATUS ps = PdhOpenQueryW(nullptr, 0, &g_gpuQuery);
    if (ps != ERROR_SUCCESS) { g_gpuFailed = TRUE; return; }

    ps = PdhAddEnglishCounterW(g_gpuQuery,
        L"\\GPU Engine(*)\\Utilization Percentage", 0, &g_gpuCounter);
    if (ps != ERROR_SUCCESS) {
        PdhCloseQuery(g_gpuQuery);
        g_gpuQuery = nullptr;
        g_gpuFailed = TRUE;
        return;
    }

    // Prime the baseline — PDH rate counters return 0 on the very first collection
    // without a prior sample to delta against. One collection here means the first
    // real tick in RefreshData produces accurate data instead of 0%.
    PdhCollectQueryData(g_gpuQuery);
}

static void CollectGpuStats(int* outTotal, int* outTopPct, WCHAR* outTopName, int nameLen) {
    *outTotal = -1;
    *outTopPct = 0;
    outTopName[0] = L'\0';

    if (!g_gpuQuery || g_gpuFailed) return;

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
    struct GpuProc { DWORD pid; double total; };
    GpuProc procs[MAX_PROCESSES];
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
            if (procs[j].pid == pid) { procs[j].total += val; break; }
        }
        if (j >= procCount && procCount < MAX_PROCESSES) {
            procs[procCount].pid = pid;
            procs[procCount].total = val;
            procCount++;
        }
    }
    free(items);

    *outTotal = (int)(grandTotal + 0.5);

    // Find top GPU process
    int topIdx = -1;
    double topVal = 0;
    for (int i = 0; i < procCount; i++) {
        if (procs[i].total > topVal) {
            topVal = procs[i].total;
            topIdx = i;
        }
    }
    if (topIdx >= 0) {
        *outTopPct = (int)(topVal + 0.5);

        HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snap != INVALID_HANDLE_VALUE) {
            PROCESSENTRY32W pe = {sizeof(pe)};
            if (Process32FirstW(snap, &pe)) {
                do {
                    if (pe.th32ProcessID == procs[topIdx].pid) {
                        wcscpy_s(outTopName, nameLen, pe.szExeFile);
                        break;
                    }
                } while (Process32NextW(snap, &pe));
            }
            CloseHandle(snap);
        }
    }
}

// ─── Data Refresh ─────────────────────────────────────────────────────────────

static void RefreshData() {
    int newTotalCpu = -1, newTopCpuPct = 0;
    WCHAR newTopCpuName[64] = {};
    int newTotalGpu = -1, newTopGpuPct = 0;
    WCHAR newTopGpuName[64] = {};

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
            NtQuerySystemInformation_t NtQuery = LoadNtQuery();
            if (NtQuery) {
                MY_SYSTEM_PROCESS_INFO* p = buf;
                LONGLONG bestTime = 0;
                WCHAR bestName[64] = {};
                int count = 0;

                while (true) {
                    LONGLONG curTime = p->KernelTime.QuadPart + p->UserTime.QuadPart;

                    LONGLONG prevTime = 0;
                    for (int i = 0; i < g_prevProcCount; i++) {
                        if (g_prevProcs[i].pid == (DWORD)(ULONG_PTR)p->UniqueProcessId) {
                            prevTime = g_prevProcs[i].time;
                            break;
                        }
                    }
                    LONGLONG delta = curTime - prevTime;
                    if (delta > bestTime && p->ImageName.Buffer) {
                        bestTime = delta;
                        wcsncpy_s(bestName, p->ImageName.Buffer,
                            MIN(p->ImageName.Length / sizeof(WCHAR), 63));
                        bestName[63] = L'\0';
                    }

                    if (count < MAX_PROCESSES) {
                        g_prevProcs[count].pid = (DWORD)(ULONG_PTR)p->UniqueProcessId;
                        g_prevProcs[count].time = curTime;
                        if (p->ImageName.Buffer) {
                            wcsncpy_s(g_prevProcs[count].name, p->ImageName.Buffer,
                                MIN(p->ImageName.Length / sizeof(WCHAR), 63));
                            g_prevProcs[count].name[63] = L'\0';
                        } else {
                            g_prevProcs[count].name[0] = L'\0';
                        }
                        count++;
                    }

                    if (p->NextEntryOffset == 0) break;
                    p = (MY_SYSTEM_PROCESS_INFO*)((BYTE*)p + p->NextEntryOffset);
                }
                g_prevProcCount = count;

                if (bestTime > 0) {
                    double pct = 100.0 * (double)bestTime / totalDelta;
                    if (pct >= 0.5) {
                        newTopCpuPct = (int)(pct + 0.5);
                        wcscpy_s(newTopCpuName, bestName);
                    }
                }
            }
            free(buf);
        }
    }

    g_prevIdle = nowIdle; g_prevKernel = nowKernel; g_prevUser = nowUser;
    g_hasPrevSample = TRUE;

    CollectGpuStats(&newTotalGpu, &newTopGpuPct, newTopGpuName, 64);

    EnterCriticalSection(&g_statsLock);
    g_totalCpu = newTotalCpu;
    g_topCpuPct = newTopCpuPct;
    wcscpy_s(g_topCpuName, newTopCpuName);
    g_totalGpu = newTotalGpu;
    g_topGpuPct = newTopGpuPct;
    wcscpy_s(g_topGpuName, newTopGpuName);
    LeaveCriticalSection(&g_statsLock);

    if (g_popupHwnd && IsWindowVisible(g_popupHwnd)) {
        InvalidateRect(g_popupHwnd, nullptr, TRUE);
    }
}

// ─── Popup Window Procedure ───────────────────────────────────────────────────

static LRESULT CALLBACK PopupWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_ACTIVATE:
            if (LOWORD(wParam) == WA_INACTIVE) {
                ShowWindow(hWnd, SW_HIDE);
            }
            return 0;

        case WM_PAINT: {
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

            EnterCriticalSection(&g_statsLock);

            for (int row = 0; row < 2; row++) {
                int y = 12 + row * 32;
                BOOL isCpu = (row == 0);

                PCWSTR label = isCpu ? L"CPU" : L"GPU";
                int totalVal = isCpu ? g_totalCpu : g_totalGpu;
                int topPctVal = isCpu ? g_topCpuPct : g_topGpuPct;
                PCWSTR topName = isCpu ? g_topCpuName : g_topGpuName;

                COLORREF labelColor = RGB(140, 140, 140);
                COLORREF totalColor = isCpu ? RGB(0, 200, 255) : RGB(0, 220, 100);
                COLORREF valueColor = RGB(220, 220, 220);

                SetTextColor(hdc, labelColor);
                WCHAR labelBuf[16];
                swprintf_s(labelBuf, L"%s:", label);
                TextOutW(hdc, 12, y, labelBuf, (int)wcslen(labelBuf));

                SetTextColor(hdc, totalColor);
                WCHAR totalBuf[16];
                if (totalVal < 0)
                    swprintf_s(totalBuf, L"..%%");
                else
                    swprintf_s(totalBuf, L"%d%%", totalVal);
                TextOutW(hdc, 60, y, totalBuf, (int)wcslen(totalBuf));

                SetTextColor(hdc, valueColor);
                if (topName[0] != L'\0' && topPctVal > 0) {
                    WCHAR lineBuf[256];
                    swprintf_s(lineBuf, L"%s  %d%%", topName, topPctVal);
                    TextOutW(hdc, 130, y, lineBuf, (int)wcslen(lineBuf));
                } else if (totalVal >= 0) {
                    PCWSTR idle = L"Idle";
                    TextOutW(hdc, 130, y, idle, (int)wcslen(idle));
                } else {
                    PCWSTR collecting = L"Collecting...";
                    TextOutW(hdc, 130, y, collecting, (int)wcslen(collecting));
                }
            }

            LeaveCriticalSection(&g_statsLock);

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
        RegisterClassW(&wc);

        g_popupHwnd = CreateWindowExW(
            WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
            wc.lpszClassName, L"MicroManager",
            WS_POPUP | WS_BORDER,
            0, 0, POPUP_WIDTH, POPUP_HEIGHT,
            nullptr, nullptr, g_hInstance, nullptr);
    }

    if (!g_popupHwnd) return;

    NOTIFYICONIDENTIFIER nii = {sizeof(nii)};
    nii.hWnd = hTrayWnd;
    nii.uID = TRAY_ICON_ID;
    RECT iconRect;
    int x, y;

    if (SUCCEEDED(Shell_NotifyIconGetRect(&nii, &iconRect))) {
        x = iconRect.left - POPUP_WIDTH + (iconRect.right - iconRect.left) / 2;
        y = iconRect.top - POPUP_HEIGHT - 4;
    } else {
        POINT pt;
        GetCursorPos(&pt);
        x = pt.x - POPUP_WIDTH / 2;
        y = pt.y - POPUP_HEIGHT - 4;
    }

    SetWindowPos(g_popupHwnd, HWND_TOPMOST, x, y, POPUP_WIDTH, POPUP_HEIGHT,
        SWP_SHOWWINDOW);
    SetFocus(g_popupHwnd);
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
                    } else {
                        ShowPopup(hWnd);
                    }
                    break;

                case WM_RBUTTONUP: {
                    HMENU hMenu = CreatePopupMenu();
                    AppendMenuW(hMenu, MF_STRING, MENU_OPEN_WINDHAWK, L"Open WindHawk");
                    AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);

                    WCHAR statusText[128];
                    EnterCriticalSection(&g_statsLock);
                    int cpu = g_totalCpu;
                    int gpu = g_totalGpu;
                    LeaveCriticalSection(&g_statsLock);

                    if (cpu >= 0 && gpu >= 0)
                        swprintf_s(statusText, L"CPU: %d%%  GPU: %d%%", cpu, gpu);
                    else
                        lstrcpyW(statusText, L"Collecting...");
                    AppendMenuW(hMenu, MF_STRING | MF_GRAYED, 0, statusText);

                    POINT pt;
                    GetCursorPos(&pt);
                    SetForegroundWindow(hWnd);
                    // TPM_RETURNCMD returns the selected ID directly — no WM_COMMAND is posted
                    int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_RIGHTBUTTON |
                        TPM_BOTTOMALIGN | TPM_RIGHTALIGN,
                        pt.x, pt.y, 0, hWnd, nullptr);
                    PostMessageW(hWnd, WM_NULL, 0, 0);
                    DestroyMenu(hMenu);

                    if (cmd == MENU_OPEN_WINDHAWK) {
                        SHELLEXECUTEINFOW sei = {sizeof(sei)};
                        sei.lpFile = g_windhawkPath;
                        sei.nShow  = SW_SHOWNORMAL;
                        ShellExecuteExW(&sei);
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
                nid.hWnd = hWnd;
                nid.uID = TRAY_ICON_ID;
                Shell_NotifyIconW(NIM_DELETE, &nid);
            }
            PostQuitMessage(0);
            return 0;
    }

    // Re-add tray icon after Explorer restarts
    if (msg == g_taskbarCreatedMsg && g_taskbarCreatedMsg != 0) {
        NOTIFYICONDATAW nid = {sizeof(nid)};
        nid.hWnd = hWnd;
        nid.uID = TRAY_ICON_ID;
        nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        nid.uCallbackMessage = WM_TRAY_CALLBACK;
        lstrcpynW(nid.szTip, L"MicroManager", 128);
        nid.hIcon = g_iconEnabled ? g_iconEnabled : LoadIconW(nullptr, IDI_APPLICATION);
        Shell_NotifyIconW(NIM_ADD, &nid);
        return 0;
    }

    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

// ─── Tray Thread ──────────────────────────────────────────────────────────────

static DWORD WINAPI TrayThreadProc(LPVOID) {
    CoInitialize(nullptr);
    g_taskbarCreatedMsg = RegisterWindowMessageW(L"TaskbarCreated");

    WNDCLASSW wc = {0};
    wc.lpfnWndProc = TrayWndProc;
    wc.hInstance = g_hInstance;
    wc.lpszClassName = L"MicroManagerTrayClass";
    RegisterClassW(&wc);

    g_trayHwnd = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        wc.lpszClassName, L"MicroManager",
        WS_POPUP,
        0, 0, 1, 1, nullptr, nullptr, g_hInstance, nullptr);
    if (!g_trayHwnd) {
        CoUninitialize();
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
    nid.hWnd = g_trayHwnd;
    nid.uID = TRAY_ICON_ID;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAY_CALLBACK;
    lstrcpynW(nid.szTip, L"MicroManager", 128);
    nid.hIcon = g_iconEnabled ? g_iconEnabled : LoadIconW(nullptr, IDI_APPLICATION);
    Shell_NotifyIconW(NIM_ADD, &nid);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    g_trayHwnd = nullptr;
    CoUninitialize();
    return 0;
}

// ─── Windhawk Callbacks ───────────────────────────────────────────────────────

BOOL WhTool_ModInit() {
    Wh_Log(L"MicroManager Init");

    InitializeCriticalSection(&g_statsLock);

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
        NtQuerySystemInformation_t NtQuery = LoadNtQuery();
        if (NtQuery) {
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
        }
        free(initBuf);
    }

    g_trayThread = CreateThread(nullptr, 0, TrayThreadProc, nullptr, 0, nullptr);
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    PCWSTR s = Wh_GetStringSetting(L"updateInterval");
    if (s) {
        DWORD newSec = _wtoi(s);
        Wh_FreeStringSetting(s);
        DWORD newMs = newSec * 1000;
        if (newMs >= 100 && newMs <= 60000) {
            g_updateMs = newMs;
            HWND hwnd = g_trayHwnd;
            if (hwnd) {
                KillTimer(hwnd, WM_TIMER_ID);
                SetTimer(hwnd, WM_TIMER_ID, newMs, nullptr);
            }
        }
    }
}

void WhTool_ModUninit() {
    Wh_Log(L"MicroManager Mod Uninit");

    // WM_CLOSE handler on the tray thread destroys g_popupHwnd before the tray
    // window itself — no cross-thread DestroyWindow needed here
    if (g_trayHwnd) PostMessageW(g_trayHwnd, WM_CLOSE, 0, 0);
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

    DeleteCriticalSection(&g_statsLock);
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
