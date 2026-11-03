// ==WindhawkMod==
// @id              taskbar-live-hud
// @name            Taskbar Live HUD
// @description     Compact system monitoring widget for the taskbar: CPU, RAM, network speed, optional ping and a working set trimmer
// @version         1.0
// @author          Lemon
// @github          https://github.com/lemon-web-stack
// @homepage        https://github.com/lemon-web-stack
// @include         windhawk.exe
// @compilerOptions -luser32 -lgdi32 -liphlpapi -lpsapi -ldwmapi -lshell32 -lgdiplus
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Live HUD

A compact system monitoring widget for the Windows 11 taskbar.
Displays real-time CPU usage, RAM load, network traffic, and optional latency,
with an optional process memory trimmer.

This is a tool mod: it runs in its own dedicated process instead of being
injected into `explorer.exe`, so it cannot affect the shell's stability and
survives Explorer restarts.

### Features
- **Real-time monitoring:** CPU load, RAM usage and active network speed.
- **Windows 11 look:** blends with the taskbar, configurable corner style.
- **Auto-hide:** hides itself when a fullscreen game or application is active.
- **Ping (opt-in, off by default):** when enabled, an ICMP echo request is sent
  to the selected public DNS resolver (Cloudflare 1.1.1.1, Google 8.8.8.8 or
  Quad9 9.9.9.9) roughly every 1.6 seconds. Nothing is sent while it is disabled.
- **Process Memory Trimmer (opt-in, off by default):** trims the working sets
  of all user processes that can be opened with the current privileges. Trimmed
  pages move to the standby list and programs page the data back in when they
  need it, so this does not free memory in a lasting way and may cause a short
  slowdown.

### Controls
- **Ctrl + Left Click:** trim memory (only if enabled in settings).
- **Alt + Left Click and drag:** adjust the horizontal position on the taskbar.

### Known limitations
- Only the primary taskbar is supported (no secondary monitors).
- Network speed is the sum of all active non-loopback adapters.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- updateInterval: 1000
  $name: "Update Interval (ms)"
  $description: "Sensor polling frequency in milliseconds (from 300 to 5000)"
- bgAlpha: 200
  $name: "Background Opacity (40-255)"
  $description: "Widget background transparency level"
- cornerStyle: round
  $name: "Corner Style"
  $description: "Window corner shape of the widget"
  $options:
    - round: "Rounded (Windows 11 Default)"
    - small: "Small Rounded"
    - square: "Square"
- offsetX: 6
  $name: "Horizontal Offset (px)"
  $description: "Distance from the system tray (adjustable via Alt + Drag)"
- showCpu: true
  $name: "Show CPU Usage"
  $description: "Display current processor load percentage"
- showRam: true
  $name: "Show RAM Usage"
  $description: "Display current memory load percentage"
- showMiniBars: true
  $name: "Show Mini-Bars"
  $description: "Display progress bars under CPU and RAM indicators"
- showNetSpeed: true
  $name: "Show Network Speed"
  $description: "Display active download and upload traffic speed"
- enableRamClean: false
  $name: "Enable Process Memory Trimmer (Ctrl + Click)"
  $description: "Allows pressing Ctrl + Left Click to trim working set memory of user processes"
- showPing: false
  $name: "Show Ping (Opt-in)"
  $description: "Measure ICMP latency to a remote DNS resolver (a request is sent about every 1.6 seconds)"
- pingTarget: cf
  $name: "Ping Target Server"
  $description: "Server used when ping display is enabled"
  $options:
    - cf: "Cloudflare (1.1.1.1)"
    - gg: "Google (8.8.8.8)"
    - q9: "Quad9 (9.9.9.9)"
*/
// ==/WindhawkModSettings==

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif

#include <windows.h>
#include <shellapi.h>
#include <dwmapi.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <gdiplus.h>
#include <string>
#include <vector>
#include <atomic>
#include <algorithm>
#include <mutex>

#define WM_HUD_UPDATE_UI        (WM_USER + 101)
#define WM_HUD_CLEAN_DONE       (WM_USER + 102)
#define WM_HUD_APPLY_SETTINGS   (WM_USER + 103)
#define TIMER_FAST_CHECK        1

#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif

// Values of MIB_IFROW::dwOperStatus and dwType, defined locally so the code
// does not depend on which constants the toolchain headers provide.
static const DWORD NET_OPER_CONNECTED   = 4;
static const DWORD NET_OPER_OPERATIONAL = 5;
static const DWORD NET_TYPE_LOOPBACK    = 24;

// Ping sentinel values: >= 0 is a real result in ms.
static const int PING_ERROR   = -1;
static const int PING_PENDING = -2;

static inline unsigned long MakeIpv4(unsigned char a, unsigned char b, unsigned char c, unsigned char d) {
    return (unsigned long)(a | (b << 8) | (c << 16) | (d << 24));
}

struct ModSettings {
    int updateInterval;
    int bgAlpha;
    int cornerPref;
    int offsetX;
    bool showCpu;
    bool showRam;
    bool showMiniBars;
    bool showNetSpeed;
    bool enableRamClean;
    bool showPing;
    unsigned long pingIp;
};

ModSettings settings = {};
std::mutex g_settingsMutex;

std::atomic<HWND> g_hHudWnd(NULL);
HANDLE g_hGuiThread = NULL;
HANDLE g_hStatsThread = NULL;
HANDLE g_hPingThread = NULL;
HANDLE g_hCleanThread = NULL;
HANDLE g_hStopEvent = NULL;
HANDLE g_hPingWake = NULL;
ULONG_PTR g_gdiplusToken = 0;

std::atomic<bool> g_bRunning(false);
std::atomic<bool> g_bCleaningRam(false);

std::atomic<int> g_cpuUsage(0);
std::atomic<int> g_ramUsage(0);
std::atomic<int> g_pingMs(PING_PENDING);
std::atomic<DWORD> g_netDownKBps(0);
std::atomic<DWORD> g_netUpKBps(0);

// The fields below are only touched from the GUI thread.
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
    static GetDpiForWindow_t pGetDpiForWindow =
        (GetDpiForWindow_t)GetProcAddress(GetModuleHandleW(L"user32.dll"), "GetDpiForWindow");
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

// GUI thread only.
void ShowTempStatus(const wchar_t* text, COLORREF color = RGB(110, 230, 145), ULONGLONG duration = 1800) {
    g_statusMessage = text;
    g_statusColor = color;
    g_statusDuration = duration;
    g_statusMessageTime = GetTickCount64();
    HWND hWnd = g_hHudWnd;
    if (hWnd) InvalidateRect(hWnd, NULL, FALSE);
}

// Applies window attributes that depend on settings. GUI thread only.
void ApplyWindowStyle(HWND hWnd) {
    int alpha, cornerPref;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        alpha = settings.bgAlpha;
        cornerPref = settings.cornerPref;
    }
    SetLayeredWindowAttributes(hWnd, 0, (BYTE)alpha, LWA_ALPHA);
    DWORD corners = (DWORD)cornerPref;
    DwmSetWindowAttribute(hWnd, DWMWA_WINDOW_CORNER_PREFERENCE, &corners, sizeof(corners));
}

void LoadSettings(bool fromSettingsChanged = false) {
    // Read everything into a local copy first. The lock is held only for the
    // final assignment, and no window calls are made while holding it.
    ModSettings s = {};

    int interval = Wh_GetIntSetting(L"updateInterval");
    s.updateInterval = (std::max)(300, (std::min)(5000, interval));

    int rawAlpha = Wh_GetIntSetting(L"bgAlpha");
    s.bgAlpha = (std::max)(40, (std::min)(255, rawAlpha));

    s.cornerPref = 2;
    PCWSTR cornerStr = Wh_GetStringSetting(L"cornerStyle");
    if (cornerStr) {
        if (lstrcmpiW(cornerStr, L"square") == 0) s.cornerPref = 1;
        else if (lstrcmpiW(cornerStr, L"small") == 0) s.cornerPref = 3;
        Wh_FreeStringSetting(cornerStr);
    }

    s.pingIp = MakeIpv4(1, 1, 1, 1);
    PCWSTR targetStr = Wh_GetStringSetting(L"pingTarget");
    if (targetStr) {
        if (lstrcmpiW(targetStr, L"gg") == 0) s.pingIp = MakeIpv4(8, 8, 8, 8);
        else if (lstrcmpiW(targetStr, L"q9") == 0) s.pingIp = MakeIpv4(9, 9, 9, 9);
        Wh_FreeStringSetting(targetStr);
    }

    s.showCpu = (Wh_GetIntSetting(L"showCpu") != 0);
    s.showRam = (Wh_GetIntSetting(L"showRam") != 0);
    s.showMiniBars = (Wh_GetIntSetting(L"showMiniBars") != 0);
    s.showNetSpeed = (Wh_GetIntSetting(L"showNetSpeed") != 0);
    s.enableRamClean = (Wh_GetIntSetting(L"enableRamClean") != 0);
    s.showPing = (Wh_GetIntSetting(L"showPing") != 0);

    if (fromSettingsChanged) {
        Wh_SetIntValue(L"customOffsetX", -9999);
        s.offsetX = Wh_GetIntSetting(L"offsetX");
    } else {
        int savedOffset = Wh_GetIntValue(L"customOffsetX", -9999);
        s.offsetX = (savedOffset != -9999) ? savedOffset : Wh_GetIntSetting(L"offsetX");
    }

    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        if (s.showPing && (!settings.showPing || settings.pingIp != s.pingIp)) {
            g_pingMs = PING_PENDING;
        }
        settings = s;
    }

    if (g_hPingWake) SetEvent(g_hPingWake);

    HWND hWnd = g_hHudWnd;
    if (hWnd) {
        PostMessageW(hWnd, WM_HUD_APPLY_SETTINGS, 0, 0);
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

struct IfCounter {
    DWORD index;
    DWORD inOctets;
    DWORD outOctets;
};

// GetIfTable has 32-bit counters. Each interface is tracked by its index, and
// the difference is computed with unsigned arithmetic, so a counter wrap-around
// is handled correctly and a newly appeared adapter does not cause a spike.
void UpdateNetSpeed(ULONGLONG elapsedMs) {
    if (elapsedMs == 0) return;
    static std::vector<IfCounter> prev;

    ULONG bufSize = 0;
    if (GetIfTable(NULL, &bufSize, FALSE) != ERROR_INSUFFICIENT_BUFFER) return;

    std::vector<BYTE> buf(bufSize);
    MIB_IFTABLE* table = (MIB_IFTABLE*)buf.data();
    if (GetIfTable(table, &bufSize, FALSE) != NO_ERROR) return;

    std::vector<IfCounter> cur;
    cur.reserve(table->dwNumEntries);
    ULONGLONG diffIn = 0, diffOut = 0;

    for (DWORD i = 0; i < table->dwNumEntries; i++) {
        const MIB_IFROW& row = table->table[i];
        if (row.dwType == NET_TYPE_LOOPBACK) continue;
        if (row.dwOperStatus != NET_OPER_OPERATIONAL && row.dwOperStatus != NET_OPER_CONNECTED) continue;

        IfCounter c = { row.dwIndex, row.dwInOctets, row.dwOutOctets };
        cur.push_back(c);

        for (const IfCounter& p : prev) {
            if (p.index == c.index) {
                diffIn  += (DWORD)(c.inOctets  - p.inOctets);   // wraps correctly
                diffOut += (DWORD)(c.outOctets - p.outOctets);
                break;
            }
        }
    }

    g_netDownKBps = (DWORD)((diffIn * 1000) / (elapsedMs * 1024));
    g_netUpKBps   = (DWORD)((diffOut * 1000) / (elapsedMs * 1024));
    prev.swap(cur);
}

int MeasurePing(unsigned long ip) {
    if (ip == 0 || ip == 0xFFFFFFFF) return PING_ERROR;

    HANDLE hIcmp = IcmpCreateFile();
    if (hIcmp == INVALID_HANDLE_VALUE) return PING_ERROR;

    char sendData[8] = "pinghud";
    BYTE replyBuf[sizeof(ICMP_ECHO_REPLY) + 32];
    DWORD ret = IcmpSendEcho(hIcmp, ip, sendData, sizeof(sendData), NULL, replyBuf, sizeof(replyBuf), 600);

    int ping = PING_ERROR;
    if (ret != 0) {
        PICMP_ECHO_REPLY reply = (PICMP_ECHO_REPLY)replyBuf;
        if (reply->Status == IP_SUCCESS) {
            ping = (int)reply->RoundTripTime;
        }
    }
    IcmpCloseHandle(hIcmp);
    return ping;
}

DWORD WINAPI CleanRamThreadProc(LPVOID) {
    MEMORYSTATUSEX memBefore = { sizeof(memBefore) };
    GlobalMemoryStatusEx(&memBefore);

    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W pe = { sizeof(pe) };
        if (Process32FirstW(hSnap, &pe)) {
            do {
                if (!g_bRunning.load()) break;
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

    g_ramUsage = (int)memAfter.dwMemoryLoad;

    wchar_t* msg = new wchar_t[48];
    if (freedMB >= 1024.0) {
        int gb = (int)(freedMB / 1024.0);
        int rem = (int)((freedMB - gb * 1024.0) / 102.4);
        wsprintfW(msg, L"+%d.%d GB Trimmed", gb, rem);
    } else if (freedMB >= 1.0) {
        wsprintfW(msg, L"+%d MB Trimmed", (int)freedMB);
    } else {
        wcscpy_s(msg, 48, L"Memory Trimmed");
    }

    g_bCleaningRam = false;
    HWND hWnd = g_hHudWnd;
    if (!hWnd || !PostMessageW(hWnd, WM_HUD_CLEAN_DONE, (WPARAM)msg, 0)) {
        delete[] msg;
    }
    return 0;
}

void TriggerAsyncRamClean() {
    bool enabled = false;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        enabled = settings.enableRamClean;
    }
    if (!enabled) return;

    if (g_bCleaningRam.exchange(true)) return;
    ShowTempStatus(L"Trimming RAM...", RGB(120, 200, 255), 5000);

    if (g_hCleanThread) {
        WaitForSingleObject(g_hCleanThread, INFINITE);
        CloseHandle(g_hCleanThread);
        g_hCleanThread = NULL;
    }

    g_hCleanThread = CreateThread(NULL, 0, CleanRamThreadProc, NULL, 0, NULL);
    if (!g_hCleanThread) {
        g_bCleaningRam = false;
        g_statusMessageTime = 0;
    }
}

void DrawRoundedMiniBarGDIPlus(Gdiplus::Graphics& g, int x, int y, int width, int height, int percent, Gdiplus::Color barColor) {
    using namespace Gdiplus;
    if (width <= 0 || height <= 0) return;
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
    if (p == PING_PENDING) wcscpy_s(pingVal, 24, L"...");
    else if (p < 0) wcscpy_s(pingVal, 24, L"err");
    else wsprintfW(pingVal, L"%dms", p);

    DWORD downKB = g_netDownKBps.load();
    DWORD upKB = g_netUpKBps.load();
    DWORD activeKB = (downKB >= upKB) ? downKB : upKB;
    const wchar_t* arrow = (downKB >= upKB) ? L"\u2193" : L"\u2191";

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
            HWND hHud = g_hHudWnd;
            HWND hFg = GetForegroundWindow();
            if (hFg && hFg != hTaskbar && hFg != hHud) {
                HMONITOR hMonTray = MonitorFromWindow(hTaskbar ? hTaskbar : hHud, MONITOR_DEFAULTTOPRIMARY);
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

// GUI thread only.
void SyncWindowGeometry() {
    HWND hWnd = g_hHudWnd;
    if (!hWnd) return;

    HWND hTray = FindWindowW(L"Shell_TrayWnd", NULL);
    if (!hTray || !IsWindowVisible(hTray) || IsFullscreenAppActive(hTray)) {
        if (IsWindowVisible(hWnd)) ShowWindow(hWnd, SW_HIDE);
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

    HDC hdc = GetDC(hWnd);
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
    ReleaseDC(hWnd, hdc);

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

    if (!IsWindowVisible(hWnd)) {
        ShowWindow(hWnd, SW_SHOWNA);
    }

    SetWindowPos(hWnd, HWND_TOPMOST, posX, posY, exactWidth, height, SWP_NOACTIVATE);
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
                {
                    std::lock_guard<std::mutex> lock(g_settingsMutex);
                    g_dragStartOffset = settings.offsetX;
                }
                SetCapture(hWnd);
            } else if (GetKeyState(VK_CONTROL) & 0x8000) {
                TriggerAsyncRamClean();
            }
            return 0;
        }

        case WM_LBUTTONUP: {
            if (g_isDragging) {
                g_isDragging = false;
                ReleaseCapture();

                int finalOffset;
                {
                    std::lock_guard<std::mutex> lock(g_settingsMutex);
                    finalOffset = settings.offsetX;
                }
                // Persist once at the end of the drag, not on every mouse move.
                Wh_SetIntValue(L"customOffsetX", finalOffset);

                ShowTempStatus(L"Position saved", RGB(130, 210, 255), 1200);
                SyncWindowGeometry();
            }
            return 0;
        }

        case WM_CAPTURECHANGED: {
            g_isDragging = false;
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

        case WM_HUD_APPLY_SETTINGS: {
            ApplyWindowStyle(hWnd);
            SyncWindowGeometry();
            InvalidateRect(hWnd, NULL, FALSE);
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
                COLORREF pingCol;
                if (ping == PING_PENDING) pingCol = colLabel;
                else if (ping < 0 || ping > 130) pingCol = colAlert;
                else if (ping > 75) pingCol = colWarn;
                else pingCol = RGB(125, 235, 155);

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
    HANDLE waits[2] = { g_hStopEvent, g_hPingWake };

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

        // With ping disabled the thread sleeps until settings change or the mod stops.
        DWORD r = WaitForMultipleObjects(2, waits, FALSE, needPing ? 1600 : INFINITE);
        if (r == WAIT_OBJECT_0 || r == WAIT_FAILED) {
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

        if (needCpu || (!needRam && !needNet)) {
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

        HWND hWnd = g_hHudWnd;
        if (hWnd) {
            PostMessageW(hWnd, WM_HUD_UPDATE_UI, 0, 0);
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
    wc.lpszClassName = L"WindhawkTaskbarLiveHud";
    if (!RegisterClassW(&wc)) {
        Wh_Log(L"RegisterClassW failed: %u", GetLastError());
        return 0;
    }

    UpdateCachedFonts(96);

    HWND hWnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_NOACTIVATE,
        wc.lpszClassName, L"TaskbarLiveHUD",
        WS_POPUP,
        0, 0, 160, 32,
        NULL, NULL, wc.hInstance, NULL
    );

    if (!hWnd) {
        Wh_Log(L"CreateWindowExW failed: %u", GetLastError());
        UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 0;
    }
    g_hHudWnd = hWnd;

    ApplyWindowStyle(hWnd);
    SyncWindowGeometry();
    SetTimer(hWnd, TIMER_FAST_CHECK, 250, NULL);

    for (;;) {
        DWORD r = MsgWaitForMultipleObjects(1, &g_hStopEvent, FALSE, INFINITE, QS_ALLINPUT);
        if (r == WAIT_OBJECT_0 || r == WAIT_FAILED) {
            break;
        }
        MSG msg;
        while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                goto end_loop;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
end_loop:

    KillTimer(hWnd, TIMER_FAST_CHECK);
    g_hHudWnd = NULL;
    DestroyWindow(hWnd);

    if (g_hFontLabel) { DeleteObject(g_hFontLabel); g_hFontLabel = NULL; }
    if (g_hFontValue) { DeleteObject(g_hFontValue); g_hFontValue = NULL; }

    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    return 0;
}

void WhTool_ModUninit();

BOOL WhTool_ModInit() {
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    if (Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, NULL) != Gdiplus::Ok) {
        g_gdiplusToken = 0;
        return FALSE;
    }

    g_hStopEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    g_hPingWake = CreateEventW(NULL, FALSE, FALSE, NULL);

    LoadSettings(false);
    g_bRunning = true;

    g_hGuiThread = CreateThread(NULL, 0, GuiWorkerThread, NULL, 0, NULL);
    g_hStatsThread = CreateThread(NULL, 0, StatsWorkerThread, NULL, 0, NULL);
    g_hPingThread = CreateThread(NULL, 0, PingWorkerThread, NULL, 0, NULL);

    if (!g_hStopEvent || !g_hPingWake || !g_hGuiThread || !g_hStatsThread || !g_hPingThread) {
        WhTool_ModUninit();
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModUninit() {
    g_bRunning = false;
    if (g_hStopEvent) {
        SetEvent(g_hStopEvent);
    }

    HANDLE threads[] = { g_hPingThread, g_hStatsThread, g_hGuiThread, g_hCleanThread };
    for (HANDLE& h : threads) {
        if (h) {
            WaitForSingleObject(h, INFINITE);
            CloseHandle(h);
            h = NULL;
        }
    }
    g_hPingThread = g_hStatsThread = g_hGuiThread = g_hCleanThread = NULL;

    if (g_hPingWake) {
        CloseHandle(g_hPingWake);
        g_hPingWake = NULL;
    }
    if (g_hStopEvent) {
        CloseHandle(g_hStopEvent);
        g_hStopEvent = NULL;
    }

    if (g_gdiplusToken) {
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
}

void WhTool_ModSettingsChanged() {
    LoadSettings(true);
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
