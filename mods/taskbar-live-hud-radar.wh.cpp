// ==WindhawkMod==
// @id              taskbar-live-hud-radar
// @name            Taskbar Live HUD
// @description     Compact system monitoring widget for the taskbar: CPU, RAM, network speed, optional ping and a working set trimmer
// @version         1.1.1
// @author          Lemon
// @github          https://github.com/lemon-web-stack
// @homepage        https://github.com/lemon-web-stack
// @include         windhawk.exe
// @compilerOptions -luser32 -lgdi32 -liphlpapi -lpsapi -ldwmapi -lshell32 -lgdiplus -ladvapi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Live HUD

A compact system monitoring widget for the Windows 10 & 11 taskbar.
Displays real-time CPU usage, RAM load, network traffic (in Mbps/Kbps), and optional router latency,
with an optional process memory trimmer and real-time mini-graph.

This is a tool mod: it runs in its own dedicated process instead of being
injected into `explorer.exe`, so it cannot affect the shell's stability and
survives Explorer restarts.

![Screenshot](https://i.imgur.com/EAyRsCR.png)

### Features
- **Real-time monitoring:** CPU load, RAM usage and active network speed (Mbps/Kbps).
- **Windows 10 & 11 look:** blends with the taskbar, configurable corner style, custom/auto background color and per-pixel background alpha.
- **Zero-delay Auto-hide:** instantly and smoothly tracks taskbar animations with 0 latency.
- **Auto-hide support:** hides cleanly when the taskbar or desktop is idle/translucent/hidden.
- **Fullscreen detection:** hides itself when a fullscreen game or application is active.
- **Ping (opt-in, off by default):** measures ICMP latency to the local router (default gateway) roughly every 1.6 seconds.
- **Mini History Graph:** press **Ctrl + Right Click** to open a real-time CPU & RAM history graph.
- **Process Memory Trimmer (opt-in, off by default):** trims the working sets of user processes upon **Ctrl + Left Click**.

### Controls
- **Ctrl + Left Click:** trim memory (only if enabled in settings).
- **Ctrl + Right Click:** toggle real-time CPU & RAM mini-graph popup.
- **Alt + Left Click and drag:** adjust the horizontal position on the taskbar.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- updateInterval: 1000
  $name: "Update Interval (ms)"
  $description: "Sensor polling frequency in milliseconds (from 300 to 5000)"
- bgAlpha: 200
  $name: "Background Opacity (40-255)"
  $description: "Widget background transparency level (text stays crisp and fully opaque)"
- bgColorMode: auto
  $name: "Background Color Mode"
  $description: "Choose between following Windows taskbar theme or custom HEX color"
  $options:
    - auto: "Automatic (Follows Taskbar Theme)"
    - custom: "Custom Color"
- customBgColor: "#14151a"
  $name: "Custom Background Color (HEX)"
  $description: "RGB hex string used when Custom Color mode is selected, e.g. #14151a, #000000, #1e1e2e"
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
  $description: "Display active download and upload traffic speed in Mbps/Kbps"
- enableRamClean: false
  $name: "Enable Process Memory Trimmer (Ctrl + Click)"
  $description: "Allows pressing Ctrl + Left Click to trim working set memory of user processes"
- showPing: false
  $name: "Show Ping to Local Router (Opt-in)"
  $description: "Measure ICMP latency to the local default gateway (a request is sent about every 1.6 seconds)"
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
#include <cmath>
#include <string>
#include <vector>
#include <deque>
#include <atomic>
#include <algorithm>
#include <mutex>

#define WM_HUD_UPDATE_UI        (WM_USER + 101)
#define WM_HUD_CLEAN_DONE       (WM_USER + 102)
#define WM_HUD_APPLY_SETTINGS   (WM_USER + 103)
#define WM_HUD_UPDATE_GRAPH     (WM_USER + 104)
#define TIMER_FAST_CHECK        1

#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif

#ifndef NET_TYPE_LOOPBACK
#define NET_TYPE_LOOPBACK 24
#endif

static const int PING_ERROR   = -1;
static const int PING_PENDING = -2;

typedef struct _LOCAL_IF_ROW2 {
    ULONG64 InterfaceLuid;
    ULONG InterfaceIndex;
    GUID InterfaceGuid;
    WCHAR Alias[256 + 1];
    WCHAR Description[256 + 1];
    ULONG PhysicalAddressLength;
    UCHAR PhysicalAddress[32];
    UCHAR PermanentPhysicalAddress[32];
    ULONG Mtu;
    DWORD Type;
    DWORD TunnelType;
    DWORD MediaType;
    DWORD PhysicalMediumType;
    DWORD AccessType;
    DWORD DirectionType;
    struct {
        BOOLEAN HardwareInterface : 1;
        BOOLEAN FilterInterface : 1;
        BOOLEAN ConnectorPresent : 1;
        BOOLEAN NotAuthenticated : 1;
        BOOLEAN NotMediaConnected : 1;
        BOOLEAN Paused : 1;
        BOOLEAN LowPower : 1;
        BOOLEAN EndPointInterface : 1;
    } InterfaceAndOperStatusFlags;
    DWORD OperStatus;
    DWORD AdminStatus;
    DWORD MediaConnectState;
    GUID NetworkGuid;
    DWORD ConnectionType;
    ULONG64 TransmitLinkSpeed;
    ULONG64 ReceiveLinkSpeed;
    ULONG64 InOctets;
    ULONG64 InUcastPkts;
    ULONG64 InNUcastPkts;
    ULONG64 InDiscards;
    ULONG64 InErrors;
    ULONG64 InUnknownProtos;
    ULONG64 InUcastOctets;
    ULONG64 InMulticastOctets;
    ULONG64 InBroadcastOctets;
    ULONG64 OutOctets;
    ULONG64 OutUcastPkts;
    ULONG64 OutNUcastPkts;
    ULONG64 OutDiscards;
    ULONG64 OutErrors;
    ULONG64 OutUcastOctets;
    ULONG64 OutMulticastOctets;
    ULONG64 OutBroadcastOctets;
    ULONG64 OutQLen;
} LOCAL_IF_ROW2;

typedef struct _LOCAL_IF_TABLE2 {
    ULONG NumEntries;
    LOCAL_IF_ROW2 Table[1];
} LOCAL_IF_TABLE2;

typedef DWORD (WINAPI *GetIfTable2_t)(LOCAL_IF_TABLE2** Table);
typedef void  (WINAPI *FreeMibTable_t)(PVOID Memory);

static inline unsigned long MakeIpv4(unsigned char a, unsigned char b, unsigned char c, unsigned char d) {
    return (unsigned long)(a | (b << 8) | (c << 16) | (d << 24));
}

bool ParseHexColor(PCWSTR str, BYTE& r, BYTE& g, BYTE& b) {
    if (!str) return false;
    if (str[0] == L'#') str++;
    if (lstrlenW(str) != 6) return false;
    wchar_t* end = nullptr;
    unsigned long val = wcstoul(str, &end, 16);
    if (end && *end == L'\0') {
        r = (BYTE)((val >> 16) & 0xFF);
        g = (BYTE)((val >> 8) & 0xFF);
        b = (BYTE)(val & 0xFF);
        return true;
    }
    return false;
}

unsigned long GetDefaultGatewayIp() {
    MIB_IPFORWARDROW route = {};
    if (GetBestRoute(MakeIpv4(1, 1, 1, 1), 0, &route) == NO_ERROR) {
        if (route.dwForwardNextHop != 0 && route.dwForwardNextHop != 0xFFFFFFFF) {
            return route.dwForwardNextHop;
        }
    }
    return 0;
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
    bool customBgEnabled;
    BYTE customR, customG, customB;
};

ModSettings settings = {};
std::mutex g_settingsMutex;

std::atomic<HWND> g_hHudWnd(NULL);
std::atomic<HWND> g_hGraphWnd(NULL);
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
std::atomic<DWORD> g_netDownKbps(0);
std::atomic<DWORD> g_netUpKbps(0);

std::mutex g_historyMutex;
static const size_t MAX_GRAPH_POINTS = 50;
std::deque<int> g_cpuHistory;
std::deque<int> g_ramHistory;

std::wstring g_statusMessage = L"";
ULONGLONG g_statusMessageTime = 0;
ULONGLONG g_statusDuration = 1800;
Gdiplus::Color g_statusColor(255, 110, 230, 145);

bool g_isHovered = false;
bool g_isDragging = false;
bool g_isTaskbarLight = false;
POINT g_dragStartPt = {};
int g_dragStartOffset = 0;

Gdiplus::Font* g_pFontLabel = nullptr;
Gdiplus::Font* g_pFontValue = nullptr;
Gdiplus::Font* g_pFontSmall = nullptr;
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

bool ReadTaskbarLightTheme() {
    DWORD val = 0;
    DWORD sz = sizeof(val);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                    L"SystemUsesLightTheme", RRF_RT_REG_DWORD, NULL, &val, &sz) == ERROR_SUCCESS) {
        return val == 1;
    }
    return false;
}

void UpdateCachedFonts(UINT dpi) {
    if (g_pFontLabel) { delete g_pFontLabel; g_pFontLabel = nullptr; }
    if (g_pFontValue) { delete g_pFontValue; g_pFontValue = nullptr; }
    if (g_pFontSmall) { delete g_pFontSmall; g_pFontSmall = nullptr; }
    g_currentDpi = dpi;

    Gdiplus::REAL fontHeight = (Gdiplus::REAL)ScaleDpi(12, dpi);
    Gdiplus::REAL smallFontHeight = (Gdiplus::REAL)ScaleDpi(10, dpi);

    Gdiplus::FontFamily familyVar(L"Segoe UI Variable Text");
    Gdiplus::FontFamily familySegoe(L"Segoe UI");

    const Gdiplus::FontFamily* pFamily = familyVar.IsAvailable() ? &familyVar :
                                         (familySegoe.IsAvailable() ? &familySegoe : Gdiplus::FontFamily::GenericSansSerif());

    g_pFontLabel = new Gdiplus::Font(pFamily, fontHeight, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    g_pFontValue = new Gdiplus::Font(pFamily, fontHeight, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
    g_pFontSmall = new Gdiplus::Font(pFamily, smallFontHeight, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
}

void SyncWindowGeometry();
void FastRepositionOnly();
void ToggleGraphWindow();

void ShowTempStatus(const wchar_t* text, Gdiplus::Color color = Gdiplus::Color(255, 110, 230, 145), ULONGLONG duration = 1800) {
    g_statusMessage = text;
    g_statusColor = color;
    g_statusDuration = duration;
    g_statusMessageTime = GetTickCount64();
    SyncWindowGeometry();
}

void ApplyWindowStyle(HWND hWnd) {
    int cornerPref;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        cornerPref = settings.cornerPref;
    }
    DWORD corners = (DWORD)cornerPref;
    DwmSetWindowAttribute(hWnd, DWMWA_WINDOW_CORNER_PREFERENCE, &corners, sizeof(corners));
}

void LoadSettings(bool fromSettingsChanged = false) {
    ModSettings s = {};

    int interval = Wh_GetIntSetting(L"updateInterval");
    s.updateInterval = (std::max)(300, (std::min)(5000, interval));

    int rawAlpha = Wh_GetIntSetting(L"bgAlpha");
    s.bgAlpha = (std::max)(40, (std::min)(255, rawAlpha));

    PCWSTR bgModeStr = Wh_GetStringSetting(L"bgColorMode");
    s.customBgEnabled = (lstrcmpiW(bgModeStr, L"custom") == 0);
    Wh_FreeStringSetting(bgModeStr);

    s.customR = 20; s.customG = 21; s.customB = 26;
    PCWSTR hexStr = Wh_GetStringSetting(L"customBgColor");
    ParseHexColor(hexStr, s.customR, s.customG, s.customB);
    Wh_FreeStringSetting(hexStr);

    s.cornerPref = 2;
    PCWSTR cornerStr = Wh_GetStringSetting(L"cornerStyle");
    if (lstrcmpiW(cornerStr, L"square") == 0) s.cornerPref = 1;
    else if (lstrcmpiW(cornerStr, L"small") == 0) s.cornerPref = 3;
    Wh_FreeStringSetting(cornerStr);

    s.showCpu = (Wh_GetIntSetting(L"showCpu") != 0);
    s.showRam = (Wh_GetIntSetting(L"showRam") != 0);
    s.showMiniBars = (Wh_GetIntSetting(L"showMiniBars") != 0);
    s.showNetSpeed = (Wh_GetIntSetting(L"showNetSpeed") != 0);
    s.enableRamClean = (Wh_GetIntSetting(L"enableRamClean") != 0);
    s.showPing = (Wh_GetIntSetting(L"showPing") != 0);

    int settingOffsetX = Wh_GetIntSetting(L"offsetX");
    int lastSettingOffsetX = Wh_GetIntValue(L"lastSettingOffsetX", -9999);
    int customOffsetX = Wh_GetIntValue(L"customOffsetX", -9999);

    if (fromSettingsChanged && (lastSettingOffsetX != -9999) && (settingOffsetX != lastSettingOffsetX)) {
        Wh_SetIntValue(L"customOffsetX", -9999);
        Wh_SetIntValue(L"lastSettingOffsetX", settingOffsetX);
        s.offsetX = settingOffsetX;
    } else {
        Wh_SetIntValue(L"lastSettingOffsetX", settingOffsetX);
        s.offsetX = (customOffsetX != -9999) ? customOffsetX : settingOffsetX;
    }

    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        if (s.showPing && !settings.showPing) {
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
    ULONG64 inOctets;
    ULONG64 outOctets;
};
static std::vector<IfCounter> s_prevNetCounters;

void ClearNetHistory() {
    s_prevNetCounters.clear();
}

void UpdateNetSpeed(ULONGLONG elapsedMs) {
    if (elapsedMs == 0) return;

    static HMODULE hIphlpapi = GetModuleHandleW(L"iphlpapi.dll");
    static GetIfTable2_t pGetIfTable2 = hIphlpapi ? (GetIfTable2_t)GetProcAddress(hIphlpapi, "GetIfTable2") : nullptr;
    static FreeMibTable_t pFreeMibTable = hIphlpapi ? (FreeMibTable_t)GetProcAddress(hIphlpapi, "FreeMibTable") : nullptr;

    if (!pGetIfTable2 || !pFreeMibTable) return;

    LOCAL_IF_TABLE2* pTable = nullptr;
    if (pGetIfTable2(&pTable) != NO_ERROR || !pTable) return;

    std::vector<IfCounter> cur;
    cur.reserve(pTable->NumEntries);
    ULONG64 diffIn = 0, diffOut = 0;

    for (ULONG i = 0; i < pTable->NumEntries; i++) {
        const LOCAL_IF_ROW2& row = pTable->Table[i];
        if (row.Type == NET_TYPE_LOOPBACK) continue;
        if (!row.InterfaceAndOperStatusFlags.HardwareInterface) continue;
        if (row.InterfaceAndOperStatusFlags.FilterInterface) continue;
        if (row.OperStatus != 1) continue;

        IfCounter c = { (DWORD)row.InterfaceIndex, row.InOctets, row.OutOctets };
        cur.push_back(c);

        for (const IfCounter& p : s_prevNetCounters) {
            if (p.index == c.index) {
                if (c.inOctets >= p.inOctets)   diffIn  += (c.inOctets  - p.inOctets);
                if (c.outOctets >= p.outOctets) diffOut += (c.outOctets - p.outOctets);
                break;
            }
        }
    }

    pFreeMibTable(pTable);

    g_netDownKbps = (DWORD)((diffIn * 8ULL) / elapsedMs);
    g_netUpKbps   = (DWORD)((diffOut * 8ULL) / elapsedMs);
    s_prevNetCounters.swap(cur);
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
        int tenths = (int)(freedMB * 10.0 / 1024.0);
        wsprintfW(msg, L"+%d.%d GB Trimmed", tenths / 10, tenths % 10);
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
    ShowTempStatus(L"Trimming RAM...", Gdiplus::Color(255, 120, 200, 255), 5000);

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

void AddRoundedRect(Gdiplus::GraphicsPath& path, int x, int y, int width, int height, int radius) {
    if (radius <= 0) {
        path.AddRectangle(Gdiplus::Rect(x, y, width, height));
        return;
    }
    int d = radius * 2;
    if (d > width) d = width;
    if (d > height) d = height;
    path.AddArc(x, y, d, d, 180, 90);
    path.AddArc(x + width - d, y, d, d, 270, 90);
    path.AddArc(x + width - d, y + height - d, d, d, 0, 90);
    path.AddArc(x, y + height - d, d, d, 90, 90);
    path.CloseFigure();
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

    SolidBrush bgBrush(Color(90, 40, 44, 54));
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

int MeasureTextWidth(Gdiplus::Graphics& g, const wchar_t* text, Gdiplus::Font* font) {
    if (!text || !font) return 0;
    Gdiplus::RectF bound;
    Gdiplus::StringFormat format(Gdiplus::StringFormat::GenericTypographic());
    format.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap | Gdiplus::StringFormatFlagsMeasureTrailingSpaces);
    g.MeasureString(text, -1, font, Gdiplus::PointF(0, 0), &format, &bound);
    return (int)std::ceil(bound.Width);
}

void DrawTextSeg(Gdiplus::Graphics& g, int& x, int y, const wchar_t* text, Gdiplus::Color color, Gdiplus::Font* font) {
    if (!text || !font) return;
    Gdiplus::StringFormat format(Gdiplus::StringFormat::GenericTypographic());
    format.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap | Gdiplus::StringFormatFlagsMeasureTrailingSpaces);
    Gdiplus::SolidBrush brush(color);
    Gdiplus::RectF bound;
    g.MeasureString(text, -1, font, Gdiplus::PointF((Gdiplus::REAL)x, (Gdiplus::REAL)y), &format, &bound);
    g.DrawString(text, -1, font, Gdiplus::PointF((Gdiplus::REAL)x, (Gdiplus::REAL)y), &format, &brush);
    x += (int)std::ceil(bound.Width);
}

void FormatStrings(wchar_t* cpuVal, wchar_t* ramVal, wchar_t* pingVal, wchar_t* netVal) {
    wsprintfW(cpuVal, L"%d%%", g_cpuUsage.load());
    wsprintfW(ramVal, L"%d%%", g_ramUsage.load());

    int p = g_pingMs.load();
    if (p == PING_PENDING) wcscpy_s(pingVal, 24, L"...");
    else if (p < 0) wcscpy_s(pingVal, 24, L"err");
    else wsprintfW(pingVal, L"%dms", p);

    DWORD downKb = g_netDownKbps.load();
    DWORD upKb = g_netUpKbps.load();
    DWORD activeKb = (downKb >= upKb) ? downKb : upKb;
    const wchar_t* arrow = (downKb >= upKb) ? L"\u2193" : L"\u2191";

    if (activeKb >= 1000) {
        DWORD mb = activeKb / 1000;
        DWORD rem = (activeKb % 1000) / 100;
        wsprintfW(netVal, L"%s%u.%u Mbps", arrow, mb, rem);
    } else {
        wsprintfW(netVal, L"%s%u Kbps", arrow, activeKb);
    }
}

// Determines if the taskbar is retracted by auto-hide or hidden by third-party mods
bool IsTaskbarHiddenOrRetracted(HWND hTray) {
    if (!hTray || !IsWindowVisible(hTray)) return true;

    // Check if another mod made the taskbar fully transparent
    if (GetWindowLongPtrW(hTray, GWL_EXSTYLE) & WS_EX_LAYERED) {
        COLORREF crKey = 0;
        BYTE bAlpha = 255;
        DWORD dwFlags = 0;
        if (GetLayeredWindowAttributes(hTray, &crKey, &bAlpha, &dwFlags)) {
            if ((dwFlags & LWA_ALPHA) && bAlpha <= 5) return true;
        }
    }

    HMONITOR hMon = MonitorFromWindow(hTray, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi = { sizeof(mi) };
    if (!GetMonitorInfoW(hMon, &mi)) return false;

    RECT rcTray = {};
    if (!GetWindowRect(hTray, &rcTray)) return true;

    // Geometry check: Windows leaves a 2-4px edge when retracted via auto-hide
    const int THRESHOLD = 6;
    if (rcTray.top >= mi.rcMonitor.bottom - THRESHOLD) return true;
    if (rcTray.bottom <= mi.rcMonitor.top + THRESHOLD) return true;
    if (rcTray.left >= mi.rcMonitor.right - THRESHOLD) return true;
    if (rcTray.right <= mi.rcMonitor.left + THRESHOLD) return true;

    // TrayNotifyWnd check: many mods hide/collapse the system tray when inactive
    HWND hNotify = FindWindowExW(hTray, NULL, L"TrayNotifyWnd", NULL);
    if (hNotify) {
        if (!IsWindowVisible(hNotify)) return true;
        RECT rcNotify = {};
        if (GetWindowRect(hNotify, &rcNotify)) {
            if (rcNotify.right <= rcNotify.left || rcNotify.bottom <= rcNotify.top) return true;
            if (rcNotify.top >= mi.rcMonitor.bottom - THRESHOLD) return true;
            if (rcNotify.bottom <= mi.rcMonitor.top + THRESHOLD) return true;
            if (rcNotify.left >= mi.rcMonitor.right - THRESHOLD) return true;
            if (rcNotify.right <= mi.rcMonitor.left + THRESHOLD) return true;
        }
    }

    return false;
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

void RenderGraphPopup(HWND hWnd) {
    RECT rc;
    GetClientRect(hWnd, &rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;
    if (width <= 0 || height <= 0) return;

    HDC screenDC = GetDC(NULL);
    HDC memDC = CreateCompatibleDC(screenDC);
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = width;
    bi.bmiHeader.biHeight = -height;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* pBits = nullptr;
    HBITMAP memBmp = CreateDIBSection(memDC, &bi, DIB_RGB_COLORS, &pBits, NULL, 0);
    HGDIOBJ oldBmp = SelectObject(memDC, memBmp);

    if (pBits) {
        memset(pBits, 0, width * height * 4);
        Gdiplus::Bitmap bmp(width, height, width * 4, PixelFormat32bppPARGB, (BYTE*)pBits);
        Gdiplus::Graphics g(&bmp);
        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);

        int bgAlpha, cornerPref, interval;
        bool customBg;
        BYTE cr, cg, cb;
        {
            std::lock_guard<std::mutex> lock(g_settingsMutex);
            bgAlpha = settings.bgAlpha;
            cornerPref = settings.cornerPref;
            interval = settings.updateInterval;
            customBg = settings.customBgEnabled;
            cr = settings.customR; cg = settings.customG; cb = settings.customB;
        }

        bool isLight = g_isTaskbarLight;
        Gdiplus::Color bgColor, borderColor;
        if (customBg) {
            bgColor = Gdiplus::Color(bgAlpha, cr, cg, cb);
            borderColor = Gdiplus::Color(255, (std::min)(255, cr + 45), (std::min)(255, cg + 45), (std::min)(255, cb + 45));
        } else if (isLight) {
            bgColor = Gdiplus::Color(bgAlpha, 245, 246, 250);
            borderColor = Gdiplus::Color(255, 180, 185, 195);
        } else {
            bgColor = Gdiplus::Color(bgAlpha, 22, 24, 30);
            borderColor = Gdiplus::Color(255, 60, 65, 78);
        }

        int radius = (cornerPref == 1) ? 0 : (cornerPref == 3 ? ScaleDpi(3, g_currentDpi) : ScaleDpi(6, g_currentDpi));
        Gdiplus::GraphicsPath bgPath;
        AddRoundedRect(bgPath, 0, 0, width - 1, height - 1, radius);

        Gdiplus::SolidBrush bgBrush(bgColor);
        g.FillPath(&bgBrush, &bgPath);

        Gdiplus::Pen borderPen(borderColor, 1.0f);
        g.DrawPath(&borderPen, &bgPath);

        wchar_t titleBuf[48];
        int totalSec = (int)((MAX_GRAPH_POINTS * interval) / 1000);
        wsprintfW(titleBuf, L"Activity (Last %ds)", totalSec);

        Gdiplus::Color colText = isLight ? Gdiplus::Color(255, 30, 32, 38) : Gdiplus::Color(255, 240, 242, 246);
        Gdiplus::SolidBrush textBrush(colText);
        g.DrawString(titleBuf, -1, g_pFontSmall, Gdiplus::PointF((Gdiplus::REAL)ScaleDpi(10, g_currentDpi), (Gdiplus::REAL)ScaleDpi(6, g_currentDpi)), &textBrush);

        wchar_t legBuf[32];
        wsprintfW(legBuf, L"CPU %d%%", g_cpuUsage.load());
        Gdiplus::SolidBrush cpuBrush(Gdiplus::Color(255, 75, 175, 255));
        g.DrawString(legBuf, -1, g_pFontSmall, Gdiplus::PointF((Gdiplus::REAL)(width - ScaleDpi(115, g_currentDpi)), (Gdiplus::REAL)ScaleDpi(6, g_currentDpi)), &cpuBrush);

        wsprintfW(legBuf, L"RAM %d%%", g_ramUsage.load());
        Gdiplus::SolidBrush ramBrush(Gdiplus::Color(255, 175, 130, 255));
        g.DrawString(legBuf, -1, g_pFontSmall, Gdiplus::PointF((Gdiplus::REAL)(width - ScaleDpi(55, g_currentDpi)), (Gdiplus::REAL)ScaleDpi(6, g_currentDpi)), &ramBrush);

        int gx = ScaleDpi(10, g_currentDpi);
        int gy = ScaleDpi(26, g_currentDpi);
        int gw = width - gx * 2;
        int gh = height - gy - ScaleDpi(8, g_currentDpi);

        Gdiplus::Pen gridPen(isLight ? Gdiplus::Color(70, 180, 185, 195) : Gdiplus::Color(60, 255, 255, 255), 1.0f);
        g.DrawLine(&gridPen, gx, gy, gx + gw, gy);
        g.DrawLine(&gridPen, gx, gy + gh / 2, gx + gw, gy + gh / 2);
        g.DrawLine(&gridPen, gx, gy + gh, gx + gw, gy + gh);

        std::vector<int> cpuPts, ramPts;
        {
            std::lock_guard<std::mutex> lock(g_historyMutex);
            cpuPts.assign(g_cpuHistory.begin(), g_cpuHistory.end());
            ramPts.assign(g_ramHistory.begin(), g_ramHistory.end());
        }

        auto DrawGraphLine = [&](const std::vector<int>& pts, Gdiplus::Color lineColor, Gdiplus::Color areaColor) {
            if (pts.size() < 2) return;
            std::vector<Gdiplus::PointF> poly;
            poly.reserve(pts.size());
            for (size_t i = 0; i < pts.size(); i++) {
                float px = gx + ((float)i / (float)(MAX_GRAPH_POINTS - 1)) * gw;
                int val = (std::max)(0, (std::min)(100, pts[i]));
                float py = (gy + gh) - ((float)val / 100.0f) * gh;
                poly.emplace_back(px, py);
            }

            std::vector<Gdiplus::PointF> fillPoly = poly;
            fillPoly.emplace_back(poly.back().X, (Gdiplus::REAL)(gy + gh));
            fillPoly.emplace_back(poly.front().X, (Gdiplus::REAL)(gy + gh));
            Gdiplus::SolidBrush areaBrush(areaColor);
            g.FillPolygon(&areaBrush, fillPoly.data(), (INT)fillPoly.size());

            Gdiplus::Pen strokePen(lineColor, 1.6f);
            g.DrawLines(&strokePen, poly.data(), (INT)poly.size());
        };

        DrawGraphLine(ramPts, Gdiplus::Color(255, 175, 130, 255), Gdiplus::Color(45, 175, 130, 255));
        DrawGraphLine(cpuPts, Gdiplus::Color(255, 75, 175, 255), Gdiplus::Color(55, 75, 175, 255));
    }

    POINT ptSrc = { 0, 0 };
    SIZE szWindow = { width, height };
    BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    UpdateLayeredWindow(hWnd, screenDC, NULL, &szWindow, memDC, &ptSrc, 0, &blend, ULW_ALPHA);

    SelectObject(memDC, oldBmp);
    DeleteObject(memBmp);
    DeleteDC(memDC);
    ReleaseDC(NULL, screenDC);
}

LRESULT CALLBACK GraphWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_KILLFOCUS:
            ShowWindow(hWnd, SW_HIDE);
            return 0;

        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) {
                ShowWindow(hWnd, SW_HIDE);
                return 0;
            }
            break;

        case WM_HUD_UPDATE_GRAPH:
            if (IsWindowVisible(hWnd)) {
                RenderGraphPopup(hWnd);
            }
            return 0;
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

void ToggleGraphWindow() {
    HWND hGraph = g_hGraphWnd;
    HWND hHud = g_hHudWnd;
    if (!hGraph || !hHud) return;

    if (IsWindowVisible(hGraph)) {
        ShowWindow(hGraph, SW_HIDE);
        return;
    }

    RECT rcHud;
    GetWindowRect(hHud, &rcHud);
    UINT dpi = g_currentDpi;

    int gw = ScaleDpi(220, dpi);
    int gh = ScaleDpi(125, dpi);
    int gx = rcHud.left + (rcHud.right - rcHud.left - gw) / 2;
    int gy = rcHud.top - gh - ScaleDpi(8, dpi);

    HMONITOR hMon = MonitorFromWindow(hHud, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    if (!GetMonitorInfoW(hMon, &mi)) return;

    if (gx < mi.rcWork.left) gx = mi.rcWork.left;
    if (gx + gw > mi.rcWork.right) gx = mi.rcWork.right - gw;
    if (gy < mi.rcWork.top) gy = rcHud.bottom + ScaleDpi(8, dpi);

    SetWindowPos(hGraph, HWND_TOPMOST, gx, gy, gw, gh, SWP_NOACTIVATE);
    ShowWindow(hGraph, SW_SHOW);
    SetForegroundWindow(hGraph);
    SetFocus(hGraph);
    RenderGraphPopup(hGraph);
}

// Ultra-fast hardware window reposition without GDI+ CPU re-rendering (zero-lag tracking)
void FastRepositionOnly() {
    HWND hWnd = g_hHudWnd.load();
    if (!hWnd) return;

    HWND hTray = FindWindowW(L"Shell_TrayWnd", NULL);
    if (!hTray || IsTaskbarHiddenOrRetracted(hTray) || IsFullscreenAppActive(hTray)) {
        if (IsWindowVisible(hWnd)) ShowWindow(hWnd, SW_HIDE);
        HWND hGraph = g_hGraphWnd.load();
        if (hGraph && IsWindowVisible(hGraph)) ShowWindow(hGraph, SW_HIDE);
        return;
    }

    RECT rcHud = {};
    GetWindowRect(hWnd, &rcHud);
    int exactWidth = rcHud.right - rcHud.left;
    int height = rcHud.bottom - rcHud.top;
    if (exactWidth <= 0 || height <= 0) return;

    UINT dpi = SafeGetWindowDpi(hTray);
    int offsetX;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        offsetX = settings.offsetX;
    }

    RECT rcTray = {};
    GetWindowRect(hTray, &rcTray);
    int trayH = rcTray.bottom - rcTray.top;

    int posX = rcTray.right - exactWidth - ScaleDpi(200, dpi);
    int posY = rcTray.top + (trayH - height) / 2;

    HWND hNotify = FindWindowExW(hTray, NULL, L"TrayNotifyWnd", NULL);
    if (hNotify) {
        RECT rcNotify;
        if (GetWindowRect(hNotify, &rcNotify)) {
            posX = rcNotify.left - exactWidth - ScaleDpi(offsetX, dpi);
            int notifyH = rcNotify.bottom - rcNotify.top;
            if (notifyH > 0) {
                posY = rcNotify.top + (notifyH - height) / 2;
            }
        }
    }

    // Move window position instantly via DWM compositor
    SetWindowPos(hWnd, HWND_TOPMOST, posX, posY, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);

    if (!IsWindowVisible(hWnd)) {
        ShowWindow(hWnd, SW_SHOWNA);
    }
}

// Full sync: measures text, redraws layered GDI+ surface when sensor values change
void SyncWindowGeometry() {
    HWND hWnd = g_hHudWnd;
    if (!hWnd) return;

    HWND hTray = FindWindowW(L"Shell_TrayWnd", NULL);
    if (!hTray || IsTaskbarHiddenOrRetracted(hTray) || IsFullscreenAppActive(hTray)) {
        if (IsWindowVisible(hWnd)) ShowWindow(hWnd, SW_HIDE);
        HWND hGraph = g_hGraphWnd.load();
        if (hGraph && IsWindowVisible(hGraph)) ShowWindow(hGraph, SW_HIDE);
        return;
    }

    UINT dpi = SafeGetWindowDpi(hTray);
    if (dpi != g_currentDpi || !g_pFontLabel) {
        UpdateCachedFonts(dpi);
    }

    bool showCpu, showRam, showPing, showNet, showBars, customBg;
    int offsetX, bgAlpha, cornerPref;
    BYTE cr, cg, cb;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        showCpu = settings.showCpu;
        showRam = settings.showRam;
        showPing = settings.showPing;
        showNet = settings.showNetSpeed;
        showBars = settings.showMiniBars;
        offsetX = settings.offsetX;
        bgAlpha = settings.bgAlpha;
        cornerPref = settings.cornerPref;
        customBg = settings.customBgEnabled;
        cr = settings.customR; cg = settings.customG; cb = settings.customB;
    }

    if (!showCpu && !showRam && !showPing && !showNet) {
        showCpu = true;
    }

    wchar_t cpuVal[16], ramVal[16], pingVal[24], netVal[32];
    FormatStrings(cpuVal, ramVal, pingVal, netVal);

    HDC screenDC = GetDC(NULL);
    HDC tempDC = CreateCompatibleDC(screenDC);

    int textW = 0;
    int padX = ScaleDpi(11, dpi);
    bool inStatus = g_statusMessageTime && (GetTickCount64() - g_statusMessageTime < g_statusDuration);

    {
        Gdiplus::Graphics measureGraphics(tempDC);
        int sepW = MeasureTextWidth(measureGraphics, L"  ", g_pFontLabel);
        int items = 0;

        if (inStatus) {
            textW = MeasureTextWidth(measureGraphics, g_statusMessage.c_str(), g_pFontValue) + ScaleDpi(8, dpi);
        } else {
            if (showCpu) {
                if (items++ > 0) textW += sepW;
                textW += MeasureTextWidth(measureGraphics, L"CPU ", g_pFontLabel) + MeasureTextWidth(measureGraphics, cpuVal, g_pFontValue);
            }
            if (showRam) {
                if (items++ > 0) textW += sepW;
                textW += MeasureTextWidth(measureGraphics, L"RAM ", g_pFontLabel) + MeasureTextWidth(measureGraphics, ramVal, g_pFontValue);
            }
            if (showNet) {
                if (items++ > 0) textW += sepW;
                textW += MeasureTextWidth(measureGraphics, netVal, g_pFontValue);
            }
            if (showPing) {
                if (items++ > 0) textW += sepW;
                textW += MeasureTextWidth(measureGraphics, pingVal, g_pFontValue);
            }
        }
    }
    DeleteDC(tempDC);

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
            int notifyH = rcNotify.bottom - rcNotify.top;
            if (notifyH > 0) {
                posY = rcNotify.top + (notifyH - height) / 2;
            }
        }
    }

    HDC memDC = CreateCompatibleDC(screenDC);
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = exactWidth;
    bi.bmiHeader.biHeight = -height;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* pBits = nullptr;
    HBITMAP memBmp = CreateDIBSection(memDC, &bi, DIB_RGB_COLORS, &pBits, NULL, 0);
    HGDIOBJ oldBmp = SelectObject(memDC, memBmp);

    if (pBits) {
        memset(pBits, 0, exactWidth * height * 4);
        Gdiplus::Bitmap bmp(exactWidth, height, exactWidth * 4, PixelFormat32bppPARGB, (BYTE*)pBits);
        Gdiplus::Graphics g(&bmp);
        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);

        bool isLight = g_isTaskbarLight;
        Gdiplus::Color bgColor, borderColor;

        if (customBg) {
            bgColor = g_isHovered ? Gdiplus::Color(bgAlpha, (std::min)(255, cr + 15), (std::min)(255, cg + 15), (std::min)(255, cb + 15))
                                  : Gdiplus::Color(bgAlpha, cr, cg, cb);
            borderColor = g_isHovered ? Gdiplus::Color(255, 95, 150, 225)
                                      : Gdiplus::Color(180, (std::min)(255, cr + 35), (std::min)(255, cg + 35), (std::min)(255, cb + 35));
        } else if (isLight) {
            bgColor = g_isHovered ? Gdiplus::Color(bgAlpha, 225, 228, 235) : Gdiplus::Color(bgAlpha, 242, 243, 247);
            borderColor = g_isHovered ? Gdiplus::Color(255, 100, 145, 220) : Gdiplus::Color(180, 180, 185, 195);
        } else {
            bgColor = g_isHovered ? Gdiplus::Color(bgAlpha, 32, 34, 42) : Gdiplus::Color(bgAlpha, 20, 21, 26);
            borderColor = g_isHovered ? Gdiplus::Color(255, 95, 150, 225) : Gdiplus::Color(180, 50, 54, 64);
        }

        int radius = 0;
        if (cornerPref == 2) radius = ScaleDpi(6, dpi);
        else if (cornerPref == 3) radius = ScaleDpi(3, dpi);

        Gdiplus::GraphicsPath bgPath;
        AddRoundedRect(bgPath, 0, 0, exactWidth - 1, height - 1, radius);

        Gdiplus::SolidBrush bgBrush(bgColor);
        g.FillPath(&bgBrush, &bgPath);

        Gdiplus::Pen borderPen(borderColor, 1.0f);
        g.DrawPath(&borderPen, &bgPath);

        if (inStatus) {
            Gdiplus::StringFormat sf;
            sf.SetAlignment(Gdiplus::StringAlignmentCenter);
            sf.SetLineAlignment(Gdiplus::StringAlignmentCenter);
            Gdiplus::SolidBrush statusBrush(g_statusColor);
            Gdiplus::RectF layoutRect(0, 0, (Gdiplus::REAL)exactWidth, (Gdiplus::REAL)height);
            g.DrawString(g_statusMessage.c_str(), -1, g_pFontValue, layoutRect, &sf, &statusBrush);
        } else {
            int cpu = g_cpuUsage.load();
            int ram = g_ramUsage.load();
            int ping = g_pingMs.load();

            Gdiplus::Color colLabel = isLight ? (g_isHovered ? Gdiplus::Color(255, 90, 95, 105) : Gdiplus::Color(255, 115, 120, 130))
                                              : (g_isHovered ? Gdiplus::Color(255, 175, 180, 190) : Gdiplus::Color(255, 140, 145, 155));
            Gdiplus::Color colNormal = isLight ? Gdiplus::Color(255, 25, 27, 32) : Gdiplus::Color(255, 245, 246, 250);
            Gdiplus::Color colWarn   = Gdiplus::Color(255, 255, 190, 50);
            Gdiplus::Color colAlert  = Gdiplus::Color(255, 255, 85, 85);

            Gdiplus::Color cpuCol = (cpu > 80) ? colAlert : (cpu > 55 ? colWarn : colNormal);
            Gdiplus::Color ramCol = (ram > 85) ? colAlert : (ram > 70 ? colWarn : colNormal);
            Gdiplus::Color pingCol;
            if (ping == PING_PENDING) pingCol = colLabel;
            else if (ping < 0 || ping > 130) pingCol = colAlert;
            else if (ping > 75) pingCol = colWarn;
            else pingCol = isLight ? Gdiplus::Color(255, 30, 160, 70) : Gdiplus::Color(255, 125, 235, 155);

            bool drawBars = showBars && (showCpu || showRam);
            int barOffset = drawBars ? ScaleDpi(2, dpi) : 0;
            int yPos = (height - ScaleDpi(16, dpi)) / 2 - barOffset;
            int xPos = ScaleDpi(11, dpi);
            int curItem = 0;

            if (showCpu) {
                if (curItem++ > 0) DrawTextSeg(g, xPos, yPos, L"  ", colLabel, g_pFontLabel);
                int cpuStartX = xPos;
                DrawTextSeg(g, xPos, yPos, L"CPU ", colLabel, g_pFontLabel);
                DrawTextSeg(g, xPos, yPos, cpuVal, cpuCol, g_pFontValue);
                int cpuEndX = xPos;

                if (showBars) {
                    Gdiplus::Color barCol = (cpu > 80) ? Gdiplus::Color(255, 255, 85, 85) :
                                           ((cpu > 55) ? Gdiplus::Color(255, 255, 190, 50) : Gdiplus::Color(255, 75, 175, 255));
                    DrawRoundedMiniBarGDIPlus(g, cpuStartX, yPos + ScaleDpi(16, dpi), cpuEndX - cpuStartX, ScaleDpi(3, dpi), cpu, barCol);
                }
            }

            if (showRam) {
                if (curItem++ > 0) DrawTextSeg(g, xPos, yPos, L"  ", colLabel, g_pFontLabel);
                int ramStartX = xPos;
                DrawTextSeg(g, xPos, yPos, L"RAM ", colLabel, g_pFontLabel);
                DrawTextSeg(g, xPos, yPos, ramVal, ramCol, g_pFontValue);
                int ramEndX = xPos;

                if (showBars) {
                    Gdiplus::Color barCol = (ram > 85) ? Gdiplus::Color(255, 255, 85, 85) :
                                           ((ram > 70) ? Gdiplus::Color(255, 255, 190, 50) : Gdiplus::Color(255, 165, 130, 255));
                    DrawRoundedMiniBarGDIPlus(g, ramStartX, yPos + ScaleDpi(16, dpi), ramEndX - ramStartX, ScaleDpi(3, dpi), ram, barCol);
                }
            }

            if (showNet) {
                if (curItem++ > 0) DrawTextSeg(g, xPos, yPos, L"  ", colLabel, g_pFontLabel);
                Gdiplus::Color netCol = isLight ? Gdiplus::Color(255, 20, 140, 220) : Gdiplus::Color(255, 115, 215, 255);
                DrawTextSeg(g, xPos, yPos, netVal, netCol, g_pFontValue);
            }

            if (showPing) {
                if (curItem++ > 0) DrawTextSeg(g, xPos, yPos, L"  ", colLabel, g_pFontLabel);
                DrawTextSeg(g, xPos, yPos, pingVal, pingCol, g_pFontValue);
            }
        }
    }

    POINT ptDst = { posX, posY };
    SIZE szDst = { exactWidth, height };
    POINT ptSrc = { 0, 0 };
    BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

    UpdateLayeredWindow(hWnd, screenDC, &ptDst, &szDst, memDC, &ptSrc, 0, &blend, ULW_ALPHA);
    SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    if (!IsWindowVisible(hWnd)) {
        ShowWindow(hWnd, SW_SHOWNA);
    }

    SelectObject(memDC, oldBmp);
    DeleteObject(memBmp);
    DeleteDC(memDC);
    ReleaseDC(NULL, screenDC);
}

// WinEvent hook executes directly on the GUI thread to eliminate lag during taskbar sliding/animations
void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD, HWND hwnd, LONG idObject, LONG, DWORD, DWORD) {
    if (idObject != OBJID_WINDOW || !hwnd) return;
    HWND hTray = FindWindowW(L"Shell_TrayWnd", NULL);
    if (!hTray) return;

    if (hwnd == hTray || GetAncestor(hwnd, GA_ROOT) == hTray) {
        FastRepositionOnly();
    }
}

LRESULT CALLBACK HudWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;

        case WM_NCHITTEST:
            return HTCLIENT;

        case WM_SETTINGCHANGE: {
            g_isTaskbarLight = ReadTaskbarLightTheme();
            SyncWindowGeometry();
            return 0;
        }

        case WM_MOUSEMOVE: {
            if (!g_isHovered) {
                g_isHovered = true;
                TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hWnd, 0 };
                TrackMouseEvent(&tme);
                SyncWindowGeometry();
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
                FastRepositionOnly();
            }
            return 0;
        }

        case WM_MOUSELEAVE: {
            g_isHovered = false;
            SyncWindowGeometry();
            return 0;
        }

        case WM_RBUTTONUP: {
            if (GetKeyState(VK_CONTROL) & 0x8000) {
                ToggleGraphWindow();
            }
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
                Wh_SetIntValue(L"customOffsetX", finalOffset);
                ShowTempStatus(L"Position saved", Gdiplus::Color(255, 130, 210, 255), 1200);
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
                ShowTempStatus(msg, Gdiplus::Color(255, 110, 240, 150), 2200);
                delete[] msg;
            }
            return 0;
        }

        case WM_HUD_APPLY_SETTINGS: {
            ApplyWindowStyle(hWnd);
            SyncWindowGeometry();
            if (g_hGraphWnd && IsWindowVisible(g_hGraphWnd)) {
                RenderGraphPopup(g_hGraphWnd);
            }
            return 0;
        }

        case WM_HUD_UPDATE_UI: {
            SyncWindowGeometry();
            return 0;
        }

        case WM_TIMER: {
            if (wParam == TIMER_FAST_CHECK) {
                if (g_statusMessageTime && (GetTickCount64() - g_statusMessageTime >= g_statusDuration)) {
                    g_statusMessageTime = 0;
                    SyncWindowGeometry();
                } else {
                    FastRepositionOnly();
                }
            }
            return 0;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            BeginPaint(hWnd, &ps);
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
        {
            std::lock_guard<std::mutex> lock(g_settingsMutex);
            needPing = settings.showPing;
        }

        if (needPing) {
            unsigned long gw = GetDefaultGatewayIp();
            g_pingMs = (gw != 0) ? MeasurePing(gw) : PING_ERROR;
        }

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
        bool needNet;
        {
            std::lock_guard<std::mutex> lock(g_settingsMutex);
            interval = settings.updateInterval;
            needNet = settings.showNetSpeed;
        }

        int cpu = GetCpuUsage();
        g_cpuUsage = cpu;

        int ram = 0;
        MEMORYSTATUSEX memInfo = { sizeof(memInfo) };
        if (GlobalMemoryStatusEx(&memInfo)) {
            ram = (int)memInfo.dwMemoryLoad;
            g_ramUsage = ram;
        }

        {
            std::lock_guard<std::mutex> lock(g_historyMutex);
            g_cpuHistory.push_back(cpu);
            g_ramHistory.push_back(ram);
            while (g_cpuHistory.size() > MAX_GRAPH_POINTS) g_cpuHistory.pop_front();
            while (g_ramHistory.size() > MAX_GRAPH_POINTS) g_ramHistory.pop_front();
        }

        if (needNet) {
            UpdateNetSpeed(elapsed > 0 ? elapsed : 1000);
        } else {
            ClearNetHistory();
        }

        HWND hWnd = g_hHudWnd;
        if (hWnd) {
            PostMessageW(hWnd, WM_HUD_UPDATE_UI, 0, 0);
        }

        HWND hGraph = g_hGraphWnd;
        if (hGraph && IsWindowVisible(hGraph)) {
            PostMessageW(hGraph, WM_HUD_UPDATE_GRAPH, 0, 0);
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

    WNDCLASSW wcGraph = {};
    wcGraph.lpfnWndProc = GraphWndProc;
    wcGraph.hInstance = wc.hInstance;
    wcGraph.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcGraph.lpszClassName = L"WindhawkTaskbarLiveHudGraph";
    if (!RegisterClassW(&wcGraph)) {
        Wh_Log(L"RegisterClassW for graph failed: %u", GetLastError());
    }

    g_isTaskbarLight = ReadTaskbarLightTheme();
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
        UnregisterClassW(wcGraph.lpszClassName, wcGraph.hInstance);
        return 0;
    }
    g_hHudWnd = hWnd;

    HWND hGraph = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED,
        wcGraph.lpszClassName, L"TaskbarLiveHUDGraph",
        WS_POPUP,
        0, 0, 220, 125,
        hWnd, NULL, wc.hInstance, NULL
    );
    g_hGraphWnd = hGraph;

    ApplyWindowStyle(hWnd);
    if (hGraph) ApplyWindowStyle(hGraph);

    SyncWindowGeometry();
    SetTimer(hWnd, TIMER_FAST_CHECK, 80, NULL);

    // Install real-time hooks for 0ms latency tracking of taskbar animations
    HWINEVENTHOOK hHookLoc = SetWinEventHook(
        EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE,
        NULL, WinEventProc, 0, 0,
        WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS
    );
    HWINEVENTHOOK hHookVis = SetWinEventHook(
        EVENT_OBJECT_SHOW, EVENT_OBJECT_HIDE,
        NULL, WinEventProc, 0, 0,
        WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS
    );

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

    if (hHookLoc) UnhookWinEvent(hHookLoc);
    if (hHookVis) UnhookWinEvent(hHookVis);

    KillTimer(hWnd, TIMER_FAST_CHECK);
    g_hHudWnd = NULL;
    if (g_hGraphWnd) {
        DestroyWindow(g_hGraphWnd);
        g_hGraphWnd = NULL;
    }
    DestroyWindow(hWnd);

    if (g_pFontLabel) { delete g_pFontLabel; g_pFontLabel = nullptr; }
    if (g_pFontValue) { delete g_pFontValue; g_pFontValue = nullptr; }
    if (g_pFontSmall) { delete g_pFontSmall; g_pFontSmall = nullptr; }

    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    UnregisterClassW(wcGraph.lpszClassName, wcGraph.hInstance);
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

    for (HANDLE* h : { &g_hGuiThread, &g_hPingThread, &g_hStatsThread, &g_hCleanThread }) {
        if (*h) {
            WaitForSingleObject(*h, INFINITE);
            CloseHandle(*h);
            *h = NULL;
        }
    }

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
// Windhawk tool mod implementation for mods which don't need to inject to other processes
bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) && sessionId == 0) {
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
        g_toolModProcessMutex = CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
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

        IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders = (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

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
    switch (GetModuleFileName(nullptr, currentProcessPath, ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR commandLine[MAX_PATH + 2 + (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath, WH_MOD_ID);

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
        (CreateProcessInternalW_t)GetProcAddress(kernelModule, "CreateProcessInternalW");
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
