// ==WindhawkMod==
// @id              taskbar-live-hud-radar
// @name            Taskbar Live HUD
// @description     Compact system monitoring widget for the taskbar: CPU, RAM, Ping, and Standby RAM cleaner
// @version         1.0
// @author          Lemon
// @github          https://github.com/lemon-web-stack
// @homepage        https://github.com/lemon-web-stack
// @include         explorer.exe
// @compilerOptions -luser32 -lgdi32 -liphlpapi -lws2_32 -lpsapi -ldwmapi -lshell32 -lgdiplus
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Live HUD

A compact system monitoring widget for the Windows 11 taskbar that displays real-time CPU usage, RAM load, network speed, and ping, with built-in one-click memory cleaning.

![Screenshot](https://i.imgur.com/dDy0BoL.png)

### Features
- **Real-time monitoring:** Tracks CPU, RAM, network traffic, and ping.
- **Windows 11 design:** Blends into the taskbar using native Acrylic blur and customizable corner styles.
- **Auto-hide:** Automatically hides when fullscreen applications or games are running.

### Controls
- **Left Click:** Clean RAM (purges Standby List and background process memory).
- **Alt + Left Click & Drag:** Adjust the horizontal position on the taskbar.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- updateInterval: 1000
  $name: "Update Interval (ms)"
  $description: "Sensor polling frequency in milliseconds (from 300 to 5000)"
- bgAlpha: 200
  $name: "Background Opacity (40-255)"
  $description: "Widget background transparency level (higher value is more opaque)"
- cornerStyle: round
  $name: "Corner Style"
  $description: "Window corner shape of the widget"
  $options:
    - round: "Rounded (Windows 11 Default)"
    - small: "Small Rounded"
    - square: "Square"
- offsetX: 6
  $name: "Horizontal Offset (px)"
  $description: "Distance from the system tray (can also be adjusted via Alt + Left Click drag)"
- showCpu: true
  $name: "Show CPU Usage"
  $description: "Display current processor load percentage"
- showRam: true
  $name: "Show RAM Usage"
  $description: "Display current memory load percentage"
- showMiniBars: true
  $name: "Show Mini-Bars"
  $description: "Display smooth progress bars under CPU and RAM indicators"
- showNetSpeed: false
  $name: "Show Network Speed"
  $description: "Display active download and upload traffic speed"
- showPing: true
  $name: "Show Ping"
  $description: "Display network latency in milliseconds"
- pingTarget: cf
  $name: "Ping Target Server"
  $description: "Public DNS server used to measure network latency"
  $options:
    - cf: "Cloudflare (1.1.1.1)"
    - gg: "Google (8.8.8.8)"
    - q9: "Quad9 (9.9.9.9)"
*/
// ==/WindhawkModSettings==

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <shellapi.h>
#include <dwmapi.h>
#include <iphlpapi.h>
#include <netioapi.h>
#include <icmpapi.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <gdiplus.h>
#include <string>
#include <atomic>
#include <algorithm>
#include <mutex>

#define WM_HUD_UPDATE_UI   (WM_USER + 101)
#define WM_HUD_CLEAN_DONE  (WM_USER + 102)
#define TIMER_FAST_CHECK   1

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif
#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif
#ifndef DWMWA_SYSTEMBACKDROP_TYPE
#define DWMWA_SYSTEMBACKDROP_TYPE 38
#endif

struct ModSettings {
    int updateInterval;
    int bgAlpha;
    int cornerPref;
    int offsetX;
    bool showCpu;
    bool showRam;
    bool showMiniBars;
    bool showNetSpeed;
    bool showPing;
    unsigned long pingIp;
} settings;

std::mutex g_settingsMutex;

HWND g_hHudWnd = NULL;
HANDLE g_hGuiThread = NULL;
HANDLE g_hStatsThread = NULL;
HANDLE g_hPingThread = NULL;
HANDLE g_hStopEvent = NULL;
ULONG_PTR g_gdiplusToken = 0;

std::atomic<bool> g_bRunning(false);
std::atomic<bool> g_bCleaningRam(false);

std::atomic<int> g_cpuUsage(0);
std::atomic<int> g_ramUsage(0);
std::atomic<int> g_pingMs(0);
std::atomic<DWORD> g_netDownKBps(0);
std::atomic<DWORD> g_netUpKBps(0);

std::wstring g_statusMessage = L"";
ULONGLONG g_statusMessageTime = 0;
ULONGLONG g_statusDuration = 1800;
COLORREF g_statusColor = RGB(110, 230, 145);

bool g_isHovered = false;
bool g_isDragging = false;
POINT g_dragStartPt = {};
int g_dragStartOffset = 0;

HFONT g_hFontLabel = NULL;
HFONT g_hFontValue = NULL;
UINT g_currentDpi = 96;

UINT SafeGetWindowDpi(HWND hWnd) {
    typedef UINT(WINAPI* GetDpiForWindow_t)(HWND);
    static GetDpiForWindow_t pGetDpiForWindow = (GetDpiForWindow_t)GetProcAddress(GetModuleHandleW(L"user32.dll"), "GetDpiForWindow");
    if (pGetDpiForWindow && hWnd) {
        UINT dpi = pGetDpiForWindow(hWnd);
        if (dpi > 0) return dpi;
    }
    return 96;
}

int ScaleDpi(int val, UINT dpi) {
    return MulDiv(val, (int)dpi, 96);
}

void UpdateCachedFonts(UINT dpi) {
    if (g_hFontLabel) DeleteObject(g_hFontLabel);
    if (g_hFontValue) DeleteObject(g_hFontValue);
    g_currentDpi = dpi;

    int fontHeight = -ScaleDpi(12, dpi);
    g_hFontLabel = CreateFontW(fontHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                               OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                               DEFAULT_PITCH | FF_SWISS, L"Segoe UI Variable Text");
    g_hFontValue = CreateFontW(fontHeight, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                               OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                               DEFAULT_PITCH | FF_SWISS, L"Segoe UI Variable Text");
}

void ShowTempStatus(const wchar_t* text, COLORREF color = RGB(110, 230, 145), ULONGLONG duration = 1800) {
    g_statusMessage = text;
    g_statusColor = color;
    g_statusDuration = duration;
    g_statusMessageTime = GetTickCount64();
    if (g_hHudWnd) InvalidateRect(g_hHudWnd, NULL, FALSE);
}

void LoadSettings(bool fromSettingsChanged = false) {
    std::lock_guard<std::mutex> lock(g_settingsMutex);

    int interval = Wh_GetIntSetting(L"updateInterval");
    settings.updateInterval = (std::max)(300, (std::min)(5000, interval));

    int rawAlpha = Wh_GetIntSetting(L"bgAlpha");
    settings.bgAlpha = (std::max)(40, (std::min)(255, rawAlpha));

    PCWSTR cornerStr = Wh_GetStringSetting(L"cornerStyle");
    if (cornerStr) {
        if (lstrcmpiW(cornerStr, L"square") == 0) settings.cornerPref = 1;
        else if (lstrcmpiW(cornerStr, L"small") == 0) settings.cornerPref = 3;
        else settings.cornerPref = 2;
    } else {
        settings.cornerPref = 2;
    }
    Wh_FreeStringSetting(cornerStr);

    PCWSTR targetStr = Wh_GetStringSetting(L"pingTarget");
    if (targetStr) {
        if (lstrcmpiW(targetStr, L"gg") == 0) settings.pingIp = inet_addr("8.8.8.8");
        else if (lstrcmpiW(targetStr, L"q9") == 0) settings.pingIp = inet_addr("9.9.9.9");
        else settings.pingIp = inet_addr("1.1.1.1");
    } else {
        settings.pingIp = inet_addr("1.1.1.1");
    }
    Wh_FreeStringSetting(targetStr);

    settings.showCpu = (Wh_GetIntSetting(L"showCpu") != 0);
    settings.showRam = (Wh_GetIntSetting(L"showRam") != 0);
    settings.showMiniBars = (Wh_GetIntSetting(L"showMiniBars") != 0);
    settings.showNetSpeed = (Wh_GetIntSetting(L"showNetSpeed") != 0);
    settings.showPing = (Wh_GetIntSetting(L"showPing") != 0);

    if (fromSettingsChanged) {
        Wh_SetIntValue(L"customOffsetX", -9999);
        settings.offsetX = Wh_GetIntSetting(L"offsetX");
    } else {
        int savedOffset = Wh_GetIntValue(L"customOffsetX", -9999);
        settings.offsetX = (savedOffset != -9999) ? savedOffset : Wh_GetIntSetting(L"offsetX");
    }

    if (g_hHudWnd) {
        SetLayeredWindowAttributes(g_hHudWnd, 0, (BYTE)settings.bgAlpha, LWA_ALPHA);
        DWORD corners = (DWORD)settings.cornerPref;
        DwmSetWindowAttribute(g_hHudWnd, DWMWA_WINDOW_CORNER_PREFERENCE, &corners, sizeof(corners));
        PostMessageW(g_hHudWnd, WM_HUD_UPDATE_UI, 0, 0);
    }
}

ULONGLONG SubtractTimes(const FILETIME& ftA, const FILETIME& ftB) {
    ULARGE_INTEGER a, b;
    a.LowPart = ftA.dwLowDateTime; a.HighPart = ftA.dwHighDateTime;
    b.LowPart = ftB.dwLowDateTime; b.HighPart = ftB.dwHighDateTime;
    return a.QuadPart - b.QuadPart;
}

int GetCpuUsage() {
    static FILETIME prevIdle = {}, prevKernel = {}, prevUser = {};
    FILETIME idle, kernel, user;
    if (!GetSystemTimes(&idle, &kernel, &user)) return 0;

    int usage = 0;
    if (prevIdle.dwLowDateTime != 0 || prevIdle.dwHighDateTime != 0) {
        ULONGLONG idleDiff = SubtractTimes(idle, prevIdle);
        ULONGLONG totalDiff = SubtractTimes(kernel, prevKernel) + SubtractTimes(user, prevUser);
        if (totalDiff > 0) {
            usage = (int)(((totalDiff - idleDiff) * 100) / totalDiff);
        }
    }
    prevIdle = idle; prevKernel = kernel; prevUser = user;
    return (std::max)(0, (std::min)(100, usage));
}

typedef NETIOAPI_API (*GetIfTable2_t)(PMIB_IF_TABLE2*);
typedef VOID (*FreeMibTable_t)(PVOID);

void UpdateNetSpeed(ULONGLONG elapsedMs) {
    if (elapsedMs == 0) return;
    static ULONG64 prevIn = 0, prevOut = 0;
    static bool firstRun = true;

    static HMODULE hIphlp = GetModuleHandleW(L"iphlpapi.dll");
    static GetIfTable2_t pGetIfTable2 = hIphlp ? (GetIfTable2_t)GetProcAddress(hIphlp, "GetIfTable2") : NULL;
    static FreeMibTable_t pFreeMibTable = hIphlp ? (FreeMibTable_t)GetProcAddress(hIphlp, "FreeMibTable") : NULL;

    if (pGetIfTable2 && pFreeMibTable) {
        PMIB_IF_TABLE2 pTable = NULL;
        if (pGetIfTable2(&pTable) == NO_ERROR && pTable) {
            ULONG64 totalIn = 0, totalOut = 0;
            for (ULONG i = 0; i < pTable->NumEntries; i++) {
                const MIB_IF_ROW2& row = pTable->Table[i];
                if (row.Type != IF_TYPE_SOFTWARE_LOOPBACK && (row.OperStatus == IfOperStatusUp)) {
                    totalIn += row.InOctets;
                    totalOut += row.OutOctets;
                }
            }

            if (!firstRun) {
                ULONG64 diffIn = (totalIn >= prevIn) ? (totalIn - prevIn) : 0;
                ULONG64 diffOut = (totalOut >= prevOut) ? (totalOut - prevOut) : 0;
                g_netDownKBps = (DWORD)((diffIn * 1000) / (elapsedMs * 1024));
                g_netUpKBps = (DWORD)((diffOut * 1000) / (elapsedMs * 1024));
            }

            prevIn = totalIn;
            prevOut = totalOut;
            firstRun = false;
            pFreeMibTable(pTable);
            return;
        }
    }

    ULONG bufSize = 16384;
    MIB_IFTABLE* pIfTable = (MIB_IFTABLE*)malloc(bufSize);
    if (!pIfTable) return;
    if (GetIfTable(pIfTable, &bufSize, FALSE) == ERROR_INSUFFICIENT_BUFFER) {
        free(pIfTable);
        pIfTable = (MIB_IFTABLE*)malloc(bufSize);
        if (!pIfTable) return;
    }
    if (GetIfTable(pIfTable, &bufSize, FALSE) == NO_ERROR) {
        DWORD totalIn = 0, totalOut = 0;
        for (DWORD i = 0; i < pIfTable->dwNumEntries; i++) {
            MIB_IFROW& row = pIfTable->table[i];
            if (row.dwType != MIB_IF_TYPE_LOOPBACK &&
               (row.dwOperStatus == MIB_IF_OPER_STATUS_OPERATIONAL || row.dwOperStatus == MIB_IF_OPER_STATUS_CONNECTED)) {
                totalIn += row.dwInOctets;
                totalOut += row.dwOutOctets;
            }
        }
        if (!firstRun) {
            DWORD diffIn = (totalIn >= (DWORD)prevIn) ? (totalIn - (DWORD)prevIn) : 0;
            DWORD diffOut = (totalOut >= (DWORD)prevOut) ? (totalOut - (DWORD)prevOut) : 0;
            g_netDownKBps = (DWORD)(((ULONGLONG)diffIn * 1000) / (elapsedMs * 1024));
            g_netUpKBps = (DWORD)(((ULONGLONG)diffOut * 1000) / (elapsedMs * 1024));
        }
        prevIn = totalIn;
        prevOut = totalOut;
        firstRun = false;
    }
    free(pIfTable);
}

int MeasurePing(unsigned long ip) {
    if (ip == INADDR_NONE) return -1;

    HANDLE hIcmp = IcmpCreateFile();
    if (hIcmp == INVALID_HANDLE_VALUE) return -1;

    char sendData[8] = "pinghud";
    BYTE replyBuf[sizeof(ICMP_ECHO_REPLY) + 32];
    DWORD ret = IcmpSendEcho(hIcmp, ip, sendData, sizeof(sendData), NULL, replyBuf, sizeof(replyBuf), 600);

    int ping = -1;
    if (ret != 0) {
        PICMP_ECHO_REPLY reply = (PICMP_ECHO_REPLY)replyBuf;
        if (reply->Status == IP_SUCCESS) {
            ping = (int)reply->RoundTripTime;
        }
    }
    IcmpCloseHandle(hIcmp);
    return ping;
}

typedef enum _SYSTEM_INFORMATION_CLASS {
    SystemMemoryListInformation = 80
} SYSTEM_INFORMATION_CLASS;

typedef enum _SYSTEM_MEMORY_LIST_COMMAND {
    MemoryPurgeStandbyList = 4
} SYSTEM_MEMORY_LIST_COMMAND;

typedef NTSTATUS(NTAPI* pfnNtSetSystemInformation)(
    SYSTEM_INFORMATION_CLASS SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength
);

void EnablePrivilege(LPCWSTR privName) {
    HANDLE hToken;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        TOKEN_PRIVILEGES tp = {};
        if (LookupPrivilegeValueW(NULL, privName, &tp.Privileges[0].Luid)) {
            tp.PrivilegeCount = 1;
            tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
            AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
        }
        CloseHandle(hToken);
    }
}

DWORD WINAPI CleanRamThreadProc(LPVOID) {
    EnablePrivilege(SE_DEBUG_NAME);
    EnablePrivilege(L"SeProfileSingleProcessPrivilege");
    EnablePrivilege(L"SeIncreaseQuotaPrivilege");

    MEMORYSTATUSEX memBefore = { sizeof(memBefore) };
    GlobalMemoryStatusEx(&memBefore);

    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (hNtdll) {
        pfnNtSetSystemInformation NtSetSystemInfo = (pfnNtSetSystemInformation)GetProcAddress(hNtdll, "NtSetSystemInformation");
        if (NtSetSystemInfo) {
            SYSTEM_MEMORY_LIST_COMMAND command = MemoryPurgeStandbyList;
            NtSetSystemInfo(SystemMemoryListInformation, &command, sizeof(command));
        }
    }

    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W pe = { sizeof(pe) };
        if (Process32FirstW(hSnap, &pe)) {
            do {
                if (pe.th32ProcessID <= 4) continue;
                HANDLE hProc = OpenProcess(PROCESS_SET_QUOTA | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pe.th32ProcessID);
                if (hProc) {
                    EmptyWorkingSet(hProc);
                    CloseHandle(hProc);
                }
            } while (Process32NextW(hSnap, &pe));
        }
        CloseHandle(hSnap);
    }

    MEMORYSTATUSEX memAfter = { sizeof(memAfter) };
    GlobalMemoryStatusEx(&memAfter);

    long long diffBytes = (long long)memAfter.ullAvailPhys - (long long)memBefore.ullAvailPhys;
    double freedMB = (double)diffBytes / (1024.0 * 1024.0);
    int diffPercent = (int)memBefore.dwMemoryLoad - (int)memAfter.dwMemoryLoad;

    g_ramUsage = (int)memAfter.dwMemoryLoad;

    wchar_t* msg = new wchar_t[48];
    if (freedMB >= 1024.0) {
        int gb = (int)(freedMB / 1024.0);
        int rem = (int)((freedMB - gb * 1024.0) / 102.4);
        wsprintfW(msg, L"+%d.%d GB RAM", gb, rem);
    } else if (freedMB >= 1.0) {
        wsprintfW(msg, L"+%d MB RAM", (int)freedMB);
    } else if (diffPercent > 0) {
        wsprintfW(msg, L"-%d%% RAM", diffPercent);
    } else {
        wcscpy_s(msg, 48, L"RAM Cleaned");
    }

    g_bCleaningRam = false;
    if (g_hHudWnd) {
        PostMessageW(g_hHudWnd, WM_HUD_CLEAN_DONE, (WPARAM)msg, 0);
    } else {
        delete[] msg;
    }
    return 0;
}

void TriggerAsyncRamClean() {
    if (g_bCleaningRam.exchange(true)) return;
    ShowTempStatus(L"Cleaning RAM...", RGB(120, 200, 255), 5000);
    HANDLE hThread = CreateThread(NULL, 0, CleanRamThreadProc, NULL, 0, NULL);
    if (hThread) CloseHandle(hThread);
    else g_bCleaningRam = false;
}

void DrawRoundedMiniBarGDIPlus(Gdiplus::Graphics& g, int x, int y, int width, int height, int percent, Gdiplus::Color barColor) {
    using namespace Gdiplus;
    int clamped = (std::max)(0, (std::min)(100, percent));
    int fillW = (width * clamped) / 100;

    int r = height;
    GraphicsPath bgPath;
    bgPath.AddArc(x, y, r, r, 90, 180);
    bgPath.AddArc(x + width - r, y, r, r, 270, 180);
    bgPath.CloseFigure();

    SolidBrush bgBrush(Color(70, 25, 27, 32));
    g.FillPath(&bgBrush, &bgPath);

    if (fillW >= r) {
        GraphicsPath fillPath;
        fillPath.AddArc(x, y, r, r, 90, 180);
        fillPath.AddArc(x + fillW - r, y, r, r, 270, 180);
        fillPath.CloseFigure();

        SolidBrush fillBrush(barColor);
        g.FillPath(&fillBrush, &fillPath);
    } else if (fillW > 0) {
        SolidBrush fillBrush(barColor);
        g.FillEllipse(&fillBrush, x, y, r, r);
    }
}

int GetTextWidthW(HDC hdc, const wchar_t* text, HFONT font) {
    HGDIOBJ old = SelectObject(hdc, font);
    SIZE sz = {};
    GetTextExtentPoint32W(hdc, text, lstrlenW(text), &sz);
    SelectObject(hdc, old);
    return sz.cx;
}

void DrawSeg(HDC hdc, int& x, int y, const wchar_t* text, COLORREF color, HFONT font) {
    HGDIOBJ old = SelectObject(hdc, font);
    SetTextColor(hdc, color);
    SIZE sz = {};
    int len = lstrlenW(text);
    GetTextExtentPoint32W(hdc, text, len, &sz);
    TextOutW(hdc, x, y, text, len);
    x += sz.cx;
    SelectObject(hdc, old);
}

void FormatStrings(wchar_t* cpuVal, wchar_t* ramVal, wchar_t* pingVal, wchar_t* netVal) {
    wsprintfW(cpuVal, L"%d%%", g_cpuUsage.load());
    wsprintfW(ramVal, L"%d%%", g_ramUsage.load());

    int p = g_pingMs.load();
    if (p < 0) wcscpy_s(pingVal, 24, L"err");
    else wsprintfW(pingVal, L"%dms", p);

    DWORD downKB = g_netDownKBps.load();
    DWORD upKB = g_netUpKBps.load();
    DWORD activeKB = (downKB >= upKB) ? downKB : upKB;
    const wchar_t* arrow = (downKB >= upKB) ? L"↓" : L"↑";

    if (activeKB >= 1024) {
        DWORD mb = activeKB / 1024;
        DWORD rem = (activeKB % 1024) / 102;
        wsprintfW(netVal, L"%s%u.%uM", arrow, mb, rem);
    } else {
        wsprintfW(netVal, L"%s%uK", arrow, activeKB);
    }
}

bool IsFullscreenAppActive(HWND hTaskbar) {
    QUERY_USER_NOTIFICATION_STATE quns;
    if (SUCCEEDED(SHQueryUserNotificationState(&quns))) {
        if (quns == QUNS_BUSY || quns == QUNS_RUNNING_D3D_FULL_SCREEN || quns == QUNS_PRESENTATION_MODE) {
            HWND hFg = GetForegroundWindow();
            if (hFg && hFg != hTaskbar && hFg != g_hHudWnd) {
                HMONITOR hMonTray = MonitorFromWindow(hTaskbar ? hTaskbar : g_hHudWnd, MONITOR_DEFAULTTOPRIMARY);
                HMONITOR hMonFg = MonitorFromWindow(hFg, MONITOR_DEFAULTTONEAREST);
                if (hMonTray == hMonFg) {
                    RECT rcFg;
                    MONITORINFO mi = { sizeof(mi) };
                    if (GetWindowRect(hFg, &rcFg) && GetMonitorInfoW(hMonTray, &mi)) {
                        if (rcFg.left <= mi.rcMonitor.left && rcFg.top <= mi.rcMonitor.top &&
                            rcFg.right >= mi.rcMonitor.right && rcFg.bottom >= mi.rcMonitor.bottom) {
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

void SyncWindowGeometry() {
    if (!g_hHudWnd) return;

    HWND hTray = FindWindowW(L"Shell_TrayWnd", NULL);
    if (!hTray || !IsWindowVisible(hTray) || IsFullscreenAppActive(hTray)) {
        if (IsWindowVisible(g_hHudWnd)) ShowWindow(g_hHudWnd, SW_HIDE);
        return;
    }

    UINT dpi = SafeGetWindowDpi(hTray);
    if (dpi != g_currentDpi || !g_hFontLabel) {
        UpdateCachedFonts(dpi);
    }

    bool showCpu, showRam, showPing, showNet;
    int offsetX;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        showCpu = settings.showCpu;
        showRam = settings.showRam;
        showPing = settings.showPing;
        showNet = settings.showNetSpeed;
        offsetX = settings.offsetX;
    }

    if (!showCpu && !showRam && !showPing && !showNet) {
        showCpu = true;
    }

    wchar_t cpuVal[16], ramVal[16], pingVal[24], netVal[32];
    FormatStrings(cpuVal, ramVal, pingVal, netVal);

    HDC hdc = GetDC(g_hHudWnd);
    int padX = ScaleDpi(11, dpi);
    int sepW = GetTextWidthW(hdc, L"  ", g_hFontLabel);
    int textW = 0;
    int items = 0;

    if (g_statusMessageTime && (GetTickCount64() - g_statusMessageTime < g_statusDuration)) {
        textW = GetTextWidthW(hdc, g_statusMessage.c_str(), g_hFontValue) + ScaleDpi(8, dpi);
    } else {
        if (showCpu) {
            if (items++ > 0) textW += sepW;
            textW += GetTextWidthW(hdc, L"CPU ", g_hFontLabel) + GetTextWidthW(hdc, cpuVal, g_hFontValue);
        }
        if (showRam) {
            if (items++ > 0) textW += sepW;
            textW += GetTextWidthW(hdc, L"RAM ", g_hFontLabel) + GetTextWidthW(hdc, ramVal, g_hFontValue);
        }
        if (showNet) {
            if (items++ > 0) textW += sepW;
            textW += GetTextWidthW(hdc, netVal, g_hFontValue);
        }
        if (showPing) {
            if (items++ > 0) textW += sepW;
            textW += GetTextWidthW(hdc, pingVal, g_hFontValue);
        }
    }
    ReleaseDC(g_hHudWnd, hdc);

    int minW = ScaleDpi(54, dpi);
    int exactWidth = (std::max)(minW, textW + padX * 2);
    int height = ScaleDpi(32, dpi);

    RECT rcTray = {};
    GetWindowRect(hTray, &rcTray);
    int trayH = rcTray.bottom - rcTray.top;
    if (trayH >= ScaleDpi(28, dpi) && trayH <= ScaleDpi(64, dpi)) {
        height = trayH - ScaleDpi(10, dpi);
    }

    int posX = rcTray.right - exactWidth - ScaleDpi(200, dpi);
    int posY = rcTray.top + (trayH - height) / 2;

    HWND hNotify = FindWindowExW(hTray, NULL, L"TrayNotifyWnd", NULL);
    if (hNotify) {
        RECT rcNotify;
        if (GetWindowRect(hNotify, &rcNotify)) {
            posX = rcNotify.left - exactWidth - ScaleDpi(offsetX, dpi);
        }
    }

    if (!IsWindowVisible(g_hHudWnd)) {
        ShowWindow(g_hHudWnd, SW_SHOWNA);
    }

    SetWindowPos(g_hHudWnd, HWND_TOPMOST, posX, posY, exactWidth, height, SWP_NOACTIVATE);
}

LRESULT CALLBACK HudWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;

        case WM_MOUSEMOVE: {
            if (!g_isHovered) {
                g_isHovered = true;
                TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hWnd, 0 };
                TrackMouseEvent(&tme);
                InvalidateRect(hWnd, NULL, FALSE);
            }
            if (g_isDragging) {
                POINT pt;
                GetCursorPos(&pt);
                int deltaX = g_dragStartPt.x - pt.x;
                int deltaUnscaled = MulDiv(deltaX, 96, (int)g_currentDpi);
                int newOffset = g_dragStartOffset + deltaUnscaled;
                newOffset = (std::max)(-400, (std::min)(1500, newOffset));
                {
                    std::lock_guard<std::mutex> lock(g_settingsMutex);
                    settings.offsetX = newOffset;
                }
                Wh_SetIntValue(L"customOffsetX", newOffset);
                SyncWindowGeometry();
            }
            return 0;
        }

        case WM_MOUSELEAVE: {
            g_isHovered = false;
            InvalidateRect(hWnd, NULL, FALSE);
            return 0;
        }

        case WM_LBUTTONDOWN: {
            if (GetKeyState(VK_MENU) & 0x8000) {
                g_isDragging = true;
                GetCursorPos(&g_dragStartPt);
                std::lock_guard<std::mutex> lock(g_settingsMutex);
                g_dragStartOffset = settings.offsetX;
                SetCapture(hWnd);
            } else {
                TriggerAsyncRamClean();
            }
            return 0;
        }

        case WM_LBUTTONUP: {
            if (g_isDragging) {
                g_isDragging = false;
                ReleaseCapture();
                ShowTempStatus(L"Position saved", RGB(130, 210, 255), 1200);
                SyncWindowGeometry();
            }
            return 0;
        }

        case WM_HUD_CLEAN_DONE: {
            wchar_t* msg = (wchar_t*)wParam;
            if (msg) {
                ShowTempStatus(msg, RGB(110, 240, 150), 2200);
                delete[] msg;
                SyncWindowGeometry();
            }
            return 0;
        }

        case WM_HUD_UPDATE_UI: {
            SyncWindowGeometry();
            if (IsWindowVisible(hWnd)) {
                InvalidateRect(hWnd, NULL, FALSE);
            }
            return 0;
        }

        case WM_TIMER: {
            if (wParam == TIMER_FAST_CHECK) {
                if (g_statusMessageTime && (GetTickCount64() - g_statusMessageTime >= g_statusDuration)) {
                    g_statusMessageTime = 0;
                    SyncWindowGeometry();
                    InvalidateRect(hWnd, NULL, FALSE);
                } else {
                    HWND hTaskbar = FindWindowW(L"Shell_TrayWnd", NULL);
                    bool shouldHide = !hTaskbar || !IsWindowVisible(hTaskbar) || IsFullscreenAppActive(hTaskbar);
                    if (shouldHide && IsWindowVisible(hWnd)) {
                        ShowWindow(hWnd, SW_HIDE);
                    } else if (!shouldHide && !IsWindowVisible(hWnd)) {
                        SyncWindowGeometry();
                    }
                }
            }
            return 0;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            RECT rc;
            GetClientRect(hWnd, &rc);

            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP memBmp = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
            HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, memBmp);

            COLORREF bgColor = g_isHovered ? RGB(32, 34, 42) : RGB(20, 21, 26);
            COLORREF borderColor = g_isHovered ? RGB(95, 150, 225) : RGB(50, 54, 64);

            HBRUSH bgBrush = CreateSolidBrush(bgColor);
            FillRect(memDC, &rc, bgBrush);
            DeleteObject(bgBrush);

            HBRUSH borderBrush = CreateSolidBrush(borderColor);
            FrameRect(memDC, &rc, borderBrush);
            DeleteObject(borderBrush);

            SetBkMode(memDC, TRANSPARENT);

            if (!g_hFontLabel || !g_hFontValue) {
                UpdateCachedFonts(g_currentDpi);
            }

            if (g_statusMessageTime && (GetTickCount64() - g_statusMessageTime < g_statusDuration)) {
                HGDIOBJ oldF = SelectObject(memDC, g_hFontValue);
                SetTextColor(memDC, g_statusColor);
                DrawTextW(memDC, g_statusMessage.c_str(), -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                SelectObject(memDC, oldF);
            } else {
                bool showCpu, showRam, showPing, showNet, showBars;
                {
                    std::lock_guard<std::mutex> lock(g_settingsMutex);
                    showCpu = settings.showCpu;
                    showRam = settings.showRam;
                    showPing = settings.showPing;
                    showNet = settings.showNetSpeed;
                    showBars = settings.showMiniBars;
                }

                if (!showCpu && !showRam && !showPing && !showNet) {
                    showCpu = true;
                }

                wchar_t cpuVal[16], ramVal[16], pingVal[24], netVal[32];
                FormatStrings(cpuVal, ramVal, pingVal, netVal);

                int cpu = g_cpuUsage.load();
                int ram = g_ramUsage.load();
                int ping = g_pingMs.load();

                COLORREF colLabel = g_isHovered ? RGB(175, 180, 190) : RGB(140, 145, 155);
                COLORREF colWhite = RGB(245, 246, 250);
                COLORREF colWarn  = RGB(255, 205, 75);
                COLORREF colAlert = RGB(255, 95, 95);

                COLORREF cpuCol  = (cpu > 80) ? colAlert : (cpu > 55 ? colWarn : colWhite);
                COLORREF ramCol  = (ram > 85) ? colAlert : (ram > 70 ? colWarn : colWhite);
                COLORREF pingCol = (ping < 0 || ping > 130) ? colAlert : (ping > 75 ? colWarn : RGB(125, 235, 155));

                SIZE szSample = {};
                SelectObject(memDC, g_hFontValue);
                GetTextExtentPoint32W(memDC, L"A", 1, &szSample);

                bool drawBars = showBars && (showCpu || showRam);
                int barOffset = drawBars ? ScaleDpi(2, g_currentDpi) : 0;
                int yPos = (rc.bottom - szSample.cy) / 2 - barOffset;
                int xPos = ScaleDpi(11, g_currentDpi);
                int items = 0;

                Gdiplus::Graphics graphics(memDC);
                graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

                if (showCpu) {
                    if (items++ > 0) DrawSeg(memDC, xPos, yPos, L"  ", colLabel, g_hFontLabel);
                    int cpuStartX = xPos;
                    DrawSeg(memDC, xPos, yPos, L"CPU ", colLabel, g_hFontLabel);
                    DrawSeg(memDC, xPos, yPos, cpuVal, cpuCol, g_hFontValue);
                    int cpuEndX = xPos;

                    if (showBars) {
                        Gdiplus::Color barCol = (cpu > 80) ? Gdiplus::Color(255, 95, 95) : 
                                               ((cpu > 55) ? Gdiplus::Color(255, 205, 75) : Gdiplus::Color(80, 185, 255));
                        DrawRoundedMiniBarGDIPlus(graphics, cpuStartX, yPos + szSample.cy + 1, cpuEndX - cpuStartX, ScaleDpi(3, g_currentDpi), cpu, barCol);
                    }
                }

                if (showRam) {
                    if (items++ > 0) DrawSeg(memDC, xPos, yPos, L"  ", colLabel, g_hFontLabel);
                    int ramStartX = xPos;
                    DrawSeg(memDC, xPos, yPos, L"RAM ", colLabel, g_hFontLabel);
                    DrawSeg(memDC, xPos, yPos, ramVal, ramCol, g_hFontValue);
                    int ramEndX = xPos;

                    if (showBars) {
                        Gdiplus::Color barCol = (ram > 85) ? Gdiplus::Color(255, 95, 95) : 
                                               ((ram > 70) ? Gdiplus::Color(255, 205, 75) : Gdiplus::Color(165, 130, 255));
                        DrawRoundedMiniBarGDIPlus(graphics, ramStartX, yPos + szSample.cy + 1, ramEndX - ramStartX, ScaleDpi(3, g_currentDpi), ram, barCol);
                    }
                }

                if (showNet) {
                    if (items++ > 0) DrawSeg(memDC, xPos, yPos, L"  ", colLabel, g_hFontLabel);
                    DrawSeg(memDC, xPos, yPos, netVal, RGB(115, 215, 255), g_hFontValue);
                }

                if (showPing) {
                    if (items++ > 0) DrawSeg(memDC, xPos, yPos, L"  ", colLabel, g_hFontLabel);
                    DrawSeg(memDC, xPos, yPos, pingVal, pingCol, g_hFontValue);
                }
            }

            BitBlt(hdc, 0, 0, rc.right, rc.bottom, memDC, 0, 0, SRCCOPY);

            SelectObject(memDC, oldBmp);
            DeleteObject(memBmp);
            DeleteDC(memDC);

            EndPaint(hWnd, &ps);
            return 0;
        }
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

DWORD WINAPI PingWorkerThread(LPVOID) {
    while (g_bRunning) {
        bool needPing;
        unsigned long targetIp;
        {
            std::lock_guard<std::mutex> lock(g_settingsMutex);
            needPing = settings.showPing;
            targetIp = settings.pingIp;
        }

        if (needPing) {
            g_pingMs = MeasurePing(targetIp);
        }

        if (WaitForSingleObject(g_hStopEvent, 1600) == WAIT_OBJECT_0) {
            break;
        }
    }
    return 0;
}

DWORD WINAPI StatsWorkerThread(LPVOID) {
    ULONGLONG lastTick = GetTickCount64();

    while (g_bRunning) {
        ULONGLONG now = GetTickCount64();
        ULONGLONG elapsed = now - lastTick;
        lastTick = now;

        int interval;
        bool needCpu, needRam, needNet;
        {
            std::lock_guard<std::mutex> lock(g_settingsMutex);
            interval = settings.updateInterval;
            needCpu = settings.showCpu;
            needRam = settings.showRam;
            needNet = settings.showNetSpeed;
        }

        if (needCpu || (!needCpu && !needRam && !needNet)) {
            g_cpuUsage = GetCpuUsage();
        }

        if (needRam) {
            MEMORYSTATUSEX memInfo = { sizeof(memInfo) };
            if (GlobalMemoryStatusEx(&memInfo)) {
                g_ramUsage = (int)memInfo.dwMemoryLoad;
            }
        }

        if (needNet) {
            UpdateNetSpeed(elapsed > 0 ? elapsed : 1000);
        }

        if (g_hHudWnd) {
            PostMessageW(g_hHudWnd, WM_HUD_UPDATE_UI, 0, 0);
        }

        if (WaitForSingleObject(g_hStopEvent, (DWORD)interval) == WAIT_OBJECT_0) {
            break;
        }
    }
    return 0;
}

DWORD WINAPI GuiWorkerThread(LPVOID) {
    WNDCLASSW wc = {};
    wc.lpfnWndProc = HudWndProc;
    wc.hInstance = GetModuleHandleW(NULL);
    wc.hCursor = LoadCursor(NULL, IDC_HAND);
    wc.lpszClassName = L"WindhawkMinimalIslandHud10";
    RegisterClassW(&wc);

    UpdateCachedFonts(96);

    g_hHudWnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_NOACTIVATE,
        wc.lpszClassName, L"TaskbarLiveHUD",
        WS_POPUP,
        0, 0, 160, 32,
        NULL, NULL, wc.hInstance, NULL
    );

    int alpha, cornerPref;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        alpha = settings.bgAlpha;
        cornerPref = settings.cornerPref;
    }
    SetLayeredWindowAttributes(g_hHudWnd, 0, (BYTE)alpha, LWA_ALPHA);

    DWORD dark = 1;
    DwmSetWindowAttribute(g_hHudWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));

    DWORD corners = (DWORD)cornerPref;
    DwmSetWindowAttribute(g_hHudWnd, DWMWA_WINDOW_CORNER_PREFERENCE, &corners, sizeof(corners));

    DWORD backdrop = 3;
    DwmSetWindowAttribute(g_hHudWnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdrop, sizeof(backdrop));

    SyncWindowGeometry();
    SetTimer(g_hHudWnd, TIMER_FAST_CHECK, 250, NULL);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    KillTimer(g_hHudWnd, TIMER_FAST_CHECK);
    if (g_hHudWnd) {
        DestroyWindow(g_hHudWnd);
        g_hHudWnd = NULL;
    }
    if (g_hFontLabel) { DeleteObject(g_hFontLabel); g_hFontLabel = NULL; }
    if (g_hFontValue) { DeleteObject(g_hFontValue); g_hFontValue = NULL; }

    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    return 0;
}

BOOL Wh_ModInit() {
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, NULL);

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    LoadSettings(false);
    g_bRunning = true;
    g_hStopEvent = CreateEventW(NULL, TRUE, FALSE, NULL);

    g_hGuiThread = CreateThread(NULL, 0, GuiWorkerThread, NULL, 0, NULL);
    g_hStatsThread = CreateThread(NULL, 0, StatsWorkerThread, NULL, 0, NULL);
    g_hPingThread = CreateThread(NULL, 0, PingWorkerThread, NULL, 0, NULL);

    return TRUE;
}

void Wh_ModUninit() {
    g_bRunning = false;
    if (g_hStopEvent) {
        SetEvent(g_hStopEvent);
    }
    if (g_hHudWnd) {
        PostMessageW(g_hHudWnd, WM_QUIT, 0, 0);
    }

    if (g_hPingThread) {
        WaitForSingleObject(g_hPingThread, 1500);
        CloseHandle(g_hPingThread);
        g_hPingThread = NULL;
    }
    if (g_hStatsThread) {
        WaitForSingleObject(g_hStatsThread, 1500);
        CloseHandle(g_hStatsThread);
        g_hStatsThread = NULL;
    }
    if (g_hGuiThread) {
        WaitForSingleObject(g_hGuiThread, 1500);
        CloseHandle(g_hGuiThread);
        g_hGuiThread = NULL;
    }
    if (g_hStopEvent) {
        CloseHandle(g_hStopEvent);
        g_hStopEvent = NULL;
    }

    WSACleanup();

    if (g_gdiplusToken) {
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings(true);
}
