// ==WindhawkMod==
// @id              taskbar-live-hud-radar
// @name            Taskbar Live HUD
// @description     Compact system monitoring widget for the taskbar: CPU, RAM, GPU, temperatures, disk speed, network speed, optional ping and a working set trimmer
// @version         1.2
// @author          Lemon
// @github          https://github.com/its-lemon
// @homepage        https://github.com/its-lemon
// @include         windhawk.exe
// @compilerOptions -luser32 -lgdi32 -liphlpapi -lpsapi -ldwmapi -lshell32 -lgdiplus -ladvapi32 -lpdh -lole32 -loleaut32 -lwbemuuid
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Live HUD

A compact system monitoring widget for the Windows 10 & 11 taskbar.
Displays real-time CPU usage, RAM load, GPU usage & temperature, CPU temperature,
disk read/write speed, network traffic (in Mbps/Kbps), and optional router latency,
with an optional process memory trimmer and real-time mini-graph.

This is a tool mod: it runs in its own dedicated process instead of being
injected into `explorer.exe`, so it cannot affect the shell's stability and
survives Explorer restarts.

![Screenshot](https://i.imgur.com/m7OiS0O.png)

### Features
- **Real-time monitoring:** CPU load, RAM usage, GPU usage & temperature, CPU temperature, disk R/W/total speed, and active network speed (Mbps/Kbps).
- **Temperature units:** Choose between Celsius (°C) and Fahrenheit (°F).
- **Windows 10 & 11 look:** blends with the taskbar, configurable corner style, custom/auto background, border, and metric colors with full RGBA / HEX opacity support.
- **Configurable sizes & font:** customize font size, widget min/max width, widget height, and mini-graph popup dimensions.
- **Customizable shortcuts & hover:** remap or disable Left/Right/Ctrl+Click actions, drag modifier, and optional mouse-hover activation with custom delay.
- **Auto-hide support:** smoothly follows taskbar auto-hide animations and hides when the taskbar is retracted.
- **Instant Fullscreen detection:** hides instantly without delay when any app goes fullscreen (e.g. F11 in browsers, video players, or games).
- **Ping (opt-in, off by default):** measures ICMP latency to the local router (default gateway) roughly every 1.6 seconds. Nothing is sent while it is disabled.
- **Mini History Graph:** press **Ctrl + Right Click** (default) to open a real-time CPU, RAM & GPU history graph.
- **Process Memory Trimmer (opt-in, off by default):** trims the working sets of user processes upon **Ctrl + Left Click** (default). Trimmed pages move to the standby list and programs page the data back in when they need it, so this does not free memory in a lasting way and may cause a short slowdown.

### Default Controls (Customizable in Settings)
- **Ctrl + Left Click:** trim memory (only if enabled in settings).
- **Ctrl + Right Click:** toggle real-time mini-graph popup.
- **Alt + Left Click and drag:** adjust the horizontal position on the taskbar.

### Known limitations
- Only the primary taskbar is supported (no secondary monitors).
- Network speed is the sum of all active hardware adapters.
- CPU temperature on Windows without a kernel driver relies on LibreHardwareMonitor/OpenHardwareMonitor WMI if running, or Windows PDH Thermal Zone counters (which on some desktops report a motherboard/ambient sensor or a static value); displays `N/A` if no sensor is exposed.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- updateInterval: 1000
  $name: "Update Interval (ms)"
  $description: "Sensor polling frequency in milliseconds (from 300 to 5000)"
- bgAlpha: 200
  $name: "Background Opacity (40-255)"
  $description: "Widget background transparency level when using Auto or #RRGGBB color (text stays crisp)"
- bgColorMode: auto
  $name: "Background Color Mode"
  $description: "Choose between following Windows taskbar theme or custom color"
  $options:
    - auto: "Automatic (Follows Taskbar Theme)"
    - custom: "Custom Color"
- customBgColor: "#14151a"
  $name: "Custom Background Color (HEX / RGBA)"
  $description: "Supports #RRGGBB, #RRGGBBAA, rgb(r,g,b), or rgba(r,g,b,a), e.g. #14151a, rgba(20, 21, 26, 0.8)"
- borderColorMode: auto
  $name: "Border Color Mode"
  $description: "Choose between automatic theme border or custom border color"
  $options:
    - auto: "Automatic (Follows Theme)"
    - custom: "Custom Color"
- customBorderColor: "rgba(95, 150, 225, 0.7)"
  $name: "Custom Border Color (HEX / RGBA)"
  $description: "Supports #RRGGBB, #RRGGBBAA, rgb(r,g,b), or rgba(r,g,b,a)"
- metricsColorMode: auto
  $name: "Metrics Color Mode"
  $description: "Automatic uses dynamic load colors (normal/warn/alert); Custom lets you set each color"
  $options:
    - auto: "Automatic (Dynamic Load Colors)"
    - custom: "Custom Colors"
- customLabelColor: "#8c919b"
  $name: "Custom Label Color (HEX / RGBA)"
  $description: "Color for metric labels (e.g. CPU, RAM, GPU)"
- customCpuColor: "#f5f6fa"
  $name: "Custom CPU Color (HEX / RGBA)"
- customRamColor: "#f5f6fa"
  $name: "Custom RAM Color (HEX / RGBA)"
- customGpuColor: "#6ee691"
  $name: "Custom GPU Color (HEX / RGBA)"
- customTempColor: "#ffbe32"
  $name: "Custom Temperature Color (HEX / RGBA)"
- customNetColor: "#73d7ff"
  $name: "Custom Network Speed Color (HEX / RGBA)"
- customDiskColor: "#c8a0ff"
  $name: "Custom Disk Speed Color (HEX / RGBA)"
- customPingColor: "#7deb9b"
  $name: "Custom Ping Color (HEX / RGBA)"
- cornerStyle: round
  $name: "Corner Style"
  $description: "Window corner shape of the widget"
  $options:
    - round: "Rounded (Windows 11 Default)"
    - small: "Small Rounded"
    - square: "Square"
- offsetX: 6
  $name: "Horizontal Offset (px)"
  $description: "Distance from the system tray (adjustable via Drag)"
- fontSize: 12
  $name: "Font Size (px)"
  $description: "Text size for HUD metrics (8 to 20, default 12)"
- widgetMinWidth: 54
  $name: "Widget Minimum Width (px)"
  $description: "Minimum width of the taskbar widget in px (24-800, 0 = auto 54 px)"
- widgetMaxWidth: 0
  $name: "Widget Maximum Width (px)"
  $description: "Maximum width of the taskbar widget (0 = unlimited)"
- widgetHeight: 0
  $name: "Widget Height (px)"
  $description: "Custom height of the taskbar widget in px (16-64, 0 = auto match taskbar)"
- graphWidth: 250
  $name: "Mini-Graph Popup Width (px)"
  $description: "Width of the history graph popup window (140 to 600)"
- graphHeight: 125
  $name: "Mini-Graph Popup Height (px)"
  $description: "Height of the history graph popup window (80 to 400)"
- showCpu: true
  $name: "Show CPU Usage"
  $description: "Display current processor load percentage"
- showCpuTemp: false
  $name: "Show CPU Temperature"
  $description: "Display CPU temperature (via ACPI/WMI or LibreHardwareMonitor)"
- showRam: true
  $name: "Show RAM Usage"
  $description: "Display current memory load percentage"
- showGpu: false
  $name: "Show GPU Usage"
  $description: "Display current GPU utilization percentage"
- showGpuTemp: false
  $name: "Show GPU Temperature"
  $description: "Display GPU temperature (NVIDIA NVAPI, AMD ADL, or LibreHardwareMonitor)"
- tempUnit: celsius
  $name: "Temperature Unit"
  $description: "Unit for CPU and GPU temperatures"
  $options:
    - celsius: "Celsius (°C)"
    - fahrenheit: "Fahrenheit (°F)"
- showMiniBars: true
  $name: "Show Mini-Bars"
  $description: "Display progress bars under CPU, RAM, and GPU indicators"
- diskSpeedMode: off
  $name: "Show Disk Speed"
  $description: "Display physical disk Read, Write, or Total speed"
  $options:
    - off: "Off"
    - total: "Total Speed (Read + Write)"
    - rw: "Read & Write Separately"
    - read: "Read Speed Only"
    - write: "Write Speed Only"
- showNetSpeed: true
  $name: "Show Network Speed"
  $description: "Display active download and upload traffic speed in Mbps/Kbps"
- enableRamClean: false
  $name: "Enable Process Memory Trimmer"
  $description: "Allows triggering working set memory trim via the configured shortcut"
- showPing: false
  $name: "Show Ping to Local Router (Opt-in)"
  $description: "Measure ICMP latency to the local default gateway (a request is sent about every 1.6 seconds)"
- leftClickAction: none
  $name: "Left Click Action"
  $options:
    - none: "Disabled"
    - toggleGraph: "Toggle Mini-Graph"
    - trimRam: "Trim Memory"
- ctrlLeftClickAction: trimRam
  $name: "Ctrl + Left Click Action"
  $options:
    - trimRam: "Trim Memory"
    - toggleGraph: "Toggle Mini-Graph"
    - none: "Disabled"
- rightClickAction: none
  $name: "Right Click Action"
  $options:
    - none: "Disabled"
    - toggleGraph: "Toggle Mini-Graph"
    - trimRam: "Trim Memory"
- ctrlRightClickAction: toggleGraph
  $name: "Ctrl + Right Click Action"
  $options:
    - toggleGraph: "Toggle Mini-Graph"
    - trimRam: "Trim Memory"
    - none: "Disabled"
- dragModifier: alt
  $name: "Drag Position Modifier (with Left Click)"
  $options:
    - alt: "Alt + Left Drag (Default)"
    - ctrl: "Ctrl + Left Drag"
    - shift: "Shift + Left Drag"
    - none: "Left Drag (No Modifier)"
    - disabled: "Disabled"
- hoverAction: none
  $name: "Mouse Hover Action"
  $description: "Automatically trigger an action when hovering over the widget"
  $options:
    - none: "Disabled"
    - toggleGraph: "Show Mini-Graph on Hover"
- hoverDelayMs: 400
  $name: "Mouse Hover Delay (ms)"
  $description: "Delay in milliseconds before the hover action activates (100 to 3000)"
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
#include <pdh.h>
#include <pdhmsg.h>
#include <wbemidl.h>
#include <oleauto.h>
#include <cmath>
#include <cstdlib>
#include <cwchar>
#include <cwctype>
#include <string>
#include <vector>
#include <deque>
#include <map>
#include <atomic>
#include <algorithm>
#include <mutex>

#define WM_HUD_UPDATE_UI        (WM_USER + 101)
#define WM_HUD_CLEAN_DONE       (WM_USER + 102)
#define WM_HUD_APPLY_SETTINGS   (WM_USER + 103)
#define WM_HUD_UPDATE_GRAPH     (WM_USER + 104)
#define TIMER_FAST_CHECK        1
#define TIMER_HOVER_ACTIVATE    2

#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif

#ifndef NET_TYPE_LOOPBACK
#define NET_TYPE_LOOPBACK 24
#endif

static const int PING_ERROR       = -1;
static const int PING_PENDING     = -2;
static const int TEMP_UNAVAILABLE = -999;

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

// Parses #RRGGBB, #RRGGBBAA, rgb(r,g,b), or rgba(r,g,b,a)
bool ParseColorString(PCWSTR str, Gdiplus::Color& outColor, BYTE defaultAlpha = 255, bool* outHasExplicitAlpha = nullptr) {
    if (outHasExplicitAlpha) *outHasExplicitAlpha = false;
    if (!str) return false;

    std::wstring s(str);
    while (!s.empty() && iswspace(s.front())) s.erase(s.begin());
    while (!s.empty() && iswspace(s.back())) s.pop_back();
    if (s.empty()) return false;

    if (s[0] == L'#') {
        std::wstring hex = s.substr(1);
        if (hex.size() == 6 || hex.size() == 8) {
            wchar_t* end = nullptr;
            unsigned long long val = wcstoull(hex.c_str(), &end, 16);
            if (end && *end == L'\0') {
                if (hex.size() == 6) {
                    BYTE r = (BYTE)((val >> 16) & 0xFF);
                    BYTE g = (BYTE)((val >> 8) & 0xFF);
                    BYTE b = (BYTE)(val & 0xFF);
                    outColor = Gdiplus::Color(defaultAlpha, r, g, b);
                } else {
                    BYTE r = (BYTE)((val >> 24) & 0xFF);
                    BYTE g = (BYTE)((val >> 16) & 0xFF);
                    BYTE b = (BYTE)((val >> 8) & 0xFF);
                    BYTE a = (BYTE)(val & 0xFF);
                    outColor = Gdiplus::Color(a, r, g, b);
                    if (outHasExplicitAlpha) *outHasExplicitAlpha = true;
                }
                return true;
            }
        }
        return false;
    }

    // Check rgb(...) or rgba(...)
    std::wstring lower = s;
    for (wchar_t& ch : lower) ch = (wchar_t)towlower(ch);
    bool isRgba = (lower.rfind(L"rgba(", 0) == 0);
    bool isRgb = !isRgba && (lower.rfind(L"rgb(", 0) == 0);
    if ((isRgba || isRgb) && lower.back() == L')') {
        size_t start = isRgba ? 5 : 4;
        std::wstring inner = s.substr(start, s.size() - start - 1);
        double vals[4] = { 0, 0, 0, (double)defaultAlpha };
        int count = 0;
        size_t pos = 0;
        while (pos < inner.size() && count < 4) {
            size_t comma = inner.find(L',', pos);
            std::wstring part = (comma == std::wstring::npos) ? inner.substr(pos) : inner.substr(pos, comma - pos);
            wchar_t* end = nullptr;
            double v = wcstod(part.c_str(), &end);
            if (end == part.c_str()) return false;
            vals[count++] = v;
            if (comma == std::wstring::npos) break;
            pos = comma + 1;
        }
        if ((isRgb && count == 3) || (isRgba && count == 4)) {
            BYTE r = (BYTE)(std::max)(0, (std::min)(255, (int)std::lround(vals[0])));
            BYTE g = (BYTE)(std::max)(0, (std::min)(255, (int)std::lround(vals[1])));
            BYTE b = (BYTE)(std::max)(0, (std::min)(255, (int)std::lround(vals[2])));
            BYTE a = defaultAlpha;
            if (isRgba) {
                if (vals[3] <= 1.0 && vals[3] >= 0.0) {
                    a = (BYTE)(std::max)(0, (std::min)(255, (int)std::lround(vals[3] * 255.0)));
                } else {
                    a = (BYTE)(std::max)(0, (std::min)(255, (int)std::lround(vals[3])));
                }
                if (outHasExplicitAlpha) *outHasExplicitAlpha = true;
            }
            outColor = Gdiplus::Color(a, r, g, b);
            return true;
        }
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

enum class DiskSpeedMode {
    Off = 0,
    Total,
    RW,
    ReadOnly,
    WriteOnly
};

enum class TempUnit {
    Celsius = 0,
    Fahrenheit
};

enum class HudAction {
    None = 0,
    TrimRam,
    ToggleGraph
};

enum class DragModifier {
    Alt = 0,
    Ctrl,
    Shift,
    None,
    Disabled
};

struct ModSettings {
    int updateInterval;
    int bgAlpha;
    int cornerPref;
    int offsetX;
    int fontSize;
    int widgetMinWidth;
    int widgetMaxWidth;
    int widgetHeight;
    int graphWidth;
    int graphHeight;

    bool showCpu;
    bool showCpuTemp;
    bool showRam;
    bool showGpu;
    bool showGpuTemp;
    TempUnit tempUnit;
    bool showMiniBars;
    DiskSpeedMode diskSpeedMode;
    bool showNetSpeed;
    bool enableRamClean;
    bool showPing;

    bool customBgEnabled;
    Gdiplus::Color customBgColor;
    bool customBgHasAlpha;

    bool customBorderEnabled;
    Gdiplus::Color customBorderColor;

    bool customMetricsEnabled;
    Gdiplus::Color customLabelColor;
    Gdiplus::Color customCpuColor;
    Gdiplus::Color customRamColor;
    Gdiplus::Color customGpuColor;
    Gdiplus::Color customTempColor;
    Gdiplus::Color customNetColor;
    Gdiplus::Color customDiskColor;
    Gdiplus::Color customPingColor;

    HudAction leftClickAction;
    HudAction ctrlLeftClickAction;
    HudAction rightClickAction;
    HudAction ctrlRightClickAction;
    DragModifier dragModifier;
    HudAction hoverAction;
    int hoverDelayMs;
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

HWND g_hookedTray = NULL;
HWINEVENTHOOK g_hHookLoc = NULL;
HWINEVENTHOOK g_hHookVis = NULL;

std::atomic<bool> g_bRunning(false);
std::atomic<bool> g_bCleaningRam(false);

std::atomic<int> g_cpuUsage(0);
std::atomic<int> g_cpuTempC(TEMP_UNAVAILABLE);
std::atomic<int> g_ramUsage(0);
std::atomic<int> g_gpuUsage(0);
std::atomic<int> g_gpuTempC(TEMP_UNAVAILABLE);
std::atomic<int> g_pingMs(PING_PENDING);
std::atomic<DWORD> g_netDownKbps(0);
std::atomic<DWORD> g_netUpKbps(0);
std::atomic<ULONGLONG> g_diskReadBps(0);
std::atomic<ULONGLONG> g_diskWriteBps(0);

// History buffer for mini-graph
std::mutex g_historyMutex;
static const size_t MAX_GRAPH_POINTS = 50;
std::deque<int> g_cpuHistory;
std::deque<int> g_ramHistory;
std::deque<int> g_gpuHistory;

std::wstring g_statusMessage = L"";
ULONGLONG g_statusMessageTime = 0;
ULONGLONG g_statusDuration = 1800;
Gdiplus::Color g_statusColor(255, 110, 230, 145);

bool g_isHovered = false;
bool g_isDragging = false;
bool g_dragPending = false;
bool g_graphOpenedByHover = false;
bool g_isTaskbarLight = false;
POINT g_dragStartPt = {};
int g_dragStartOffset = 0;
HudAction g_pendingClickAction = HudAction::None;

Gdiplus::Font* g_pFontLabel = nullptr;
Gdiplus::Font* g_pFontValue = nullptr;
Gdiplus::Font* g_pFontSmall = nullptr;
UINT g_currentDpi = 96;
int g_currentFontSize = 12;

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

void UpdateCachedFonts(UINT dpi, int fontSize) {
    if (g_pFontLabel) { delete g_pFontLabel; g_pFontLabel = nullptr; }
    if (g_pFontValue) { delete g_pFontValue; g_pFontValue = nullptr; }
    if (g_pFontSmall) { delete g_pFontSmall; g_pFontSmall = nullptr; }
    g_currentDpi = dpi;
    g_currentFontSize = fontSize;

    Gdiplus::REAL fontHeight = (Gdiplus::REAL)ScaleDpi(fontSize, dpi);
    Gdiplus::REAL smallFontHeight = (Gdiplus::REAL)ScaleDpi((std::max)(8, fontSize - 2), dpi);

    // Support Windows 11 (Segoe UI Variable) and Windows 10 (Segoe UI fallback)
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
void ShowGraphWindow(bool activate);
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

HudAction ParseHudAction(PCWSTR str, HudAction def) {
    if (!str) return def;
    if (lstrcmpiW(str, L"trimRam") == 0) return HudAction::TrimRam;
    if (lstrcmpiW(str, L"toggleGraph") == 0) return HudAction::ToggleGraph;
    if (lstrcmpiW(str, L"none") == 0) return HudAction::None;
    return def;
}

Gdiplus::Color LoadColorSetting(PCWSTR name, Gdiplus::Color fallback, BYTE defaultAlpha = 255, bool* outExplicitAlpha = nullptr) {
    PCWSTR str = Wh_GetStringSetting(name);
    Gdiplus::Color col = fallback;
    ParseColorString(str, col, defaultAlpha, outExplicitAlpha);
    Wh_FreeStringSetting(str);
    return col;
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

    s.customBgColor = LoadColorSetting(L"customBgColor", Gdiplus::Color((BYTE)s.bgAlpha, 20, 21, 26), (BYTE)s.bgAlpha, &s.customBgHasAlpha);

    PCWSTR borderModeStr = Wh_GetStringSetting(L"borderColorMode");
    s.customBorderEnabled = (lstrcmpiW(borderModeStr, L"custom") == 0);
    Wh_FreeStringSetting(borderModeStr);
    s.customBorderColor = LoadColorSetting(L"customBorderColor", Gdiplus::Color(180, 95, 150, 225), 255);

    PCWSTR metricsModeStr = Wh_GetStringSetting(L"metricsColorMode");
    s.customMetricsEnabled = (lstrcmpiW(metricsModeStr, L"custom") == 0);
    Wh_FreeStringSetting(metricsModeStr);
    s.customLabelColor = LoadColorSetting(L"customLabelColor", Gdiplus::Color(255, 140, 145, 155), 255);
    s.customCpuColor   = LoadColorSetting(L"customCpuColor",   Gdiplus::Color(255, 245, 246, 250), 255);
    s.customRamColor   = LoadColorSetting(L"customRamColor",   Gdiplus::Color(255, 245, 246, 250), 255);
    s.customGpuColor   = LoadColorSetting(L"customGpuColor",   Gdiplus::Color(255, 110, 230, 145), 255);
    s.customTempColor  = LoadColorSetting(L"customTempColor",  Gdiplus::Color(255, 255, 190, 50),  255);
    s.customNetColor   = LoadColorSetting(L"customNetColor",   Gdiplus::Color(255, 115, 215, 255), 255);
    s.customDiskColor  = LoadColorSetting(L"customDiskColor",  Gdiplus::Color(255, 200, 160, 255), 255);
    s.customPingColor  = LoadColorSetting(L"customPingColor",  Gdiplus::Color(255, 125, 235, 155), 255);

    s.cornerPref = 2;
    PCWSTR cornerStr = Wh_GetStringSetting(L"cornerStyle");
    if (lstrcmpiW(cornerStr, L"square") == 0) s.cornerPref = 1;
    else if (lstrcmpiW(cornerStr, L"small") == 0) s.cornerPref = 3;
    Wh_FreeStringSetting(cornerStr);

    int rawFontSize = Wh_GetIntSetting(L"fontSize");
    if (rawFontSize <= 0) rawFontSize = 12;
    s.fontSize = (std::max)(8, (std::min)(20, rawFontSize));

    int rawMinW = Wh_GetIntSetting(L"widgetMinWidth");
    s.widgetMinWidth = (rawMinW <= 0) ? 54 : (std::max)(24, (std::min)(800, rawMinW));

    int rawMaxW = Wh_GetIntSetting(L"widgetMaxWidth");
    s.widgetMaxWidth = (rawMaxW <= 0) ? 0 : (std::max)(s.widgetMinWidth, (std::min)(1200, rawMaxW));

    int rawH = Wh_GetIntSetting(L"widgetHeight");
    s.widgetHeight = (rawH <= 0) ? 0 : (std::max)(16, (std::min)(64, rawH));

    int rawGw = Wh_GetIntSetting(L"graphWidth");
    if (rawGw <= 0) rawGw = 250;
    s.graphWidth = (std::max)(140, (std::min)(600, rawGw));

    int rawGh = Wh_GetIntSetting(L"graphHeight");
    if (rawGh <= 0) rawGh = 125;
    s.graphHeight = (std::max)(80, (std::min)(400, rawGh));

    s.showCpu = (Wh_GetIntSetting(L"showCpu") != 0);
    s.showCpuTemp = (Wh_GetIntSetting(L"showCpuTemp") != 0);
    s.showRam = (Wh_GetIntSetting(L"showRam") != 0);
    s.showGpu = (Wh_GetIntSetting(L"showGpu") != 0);
    s.showGpuTemp = (Wh_GetIntSetting(L"showGpuTemp") != 0);

    PCWSTR tempUnitStr = Wh_GetStringSetting(L"tempUnit");
    s.tempUnit = (lstrcmpiW(tempUnitStr, L"fahrenheit") == 0) ? TempUnit::Fahrenheit : TempUnit::Celsius;
    Wh_FreeStringSetting(tempUnitStr);

    s.showMiniBars = (Wh_GetIntSetting(L"showMiniBars") != 0);

    PCWSTR diskModeStr = Wh_GetStringSetting(L"diskSpeedMode");
    s.diskSpeedMode = DiskSpeedMode::Off;
    if (lstrcmpiW(diskModeStr, L"total") == 0) s.diskSpeedMode = DiskSpeedMode::Total;
    else if (lstrcmpiW(diskModeStr, L"rw") == 0) s.diskSpeedMode = DiskSpeedMode::RW;
    else if (lstrcmpiW(diskModeStr, L"read") == 0) s.diskSpeedMode = DiskSpeedMode::ReadOnly;
    else if (lstrcmpiW(diskModeStr, L"write") == 0) s.diskSpeedMode = DiskSpeedMode::WriteOnly;
    Wh_FreeStringSetting(diskModeStr);

    s.showNetSpeed = (Wh_GetIntSetting(L"showNetSpeed") != 0);
    s.enableRamClean = (Wh_GetIntSetting(L"enableRamClean") != 0);
    s.showPing = (Wh_GetIntSetting(L"showPing") != 0);

    PCWSTR actStr = Wh_GetStringSetting(L"leftClickAction");
    s.leftClickAction = ParseHudAction(actStr, HudAction::None);
    Wh_FreeStringSetting(actStr);

    actStr = Wh_GetStringSetting(L"ctrlLeftClickAction");
    s.ctrlLeftClickAction = ParseHudAction(actStr, HudAction::TrimRam);
    Wh_FreeStringSetting(actStr);

    actStr = Wh_GetStringSetting(L"rightClickAction");
    s.rightClickAction = ParseHudAction(actStr, HudAction::None);
    Wh_FreeStringSetting(actStr);

    actStr = Wh_GetStringSetting(L"ctrlRightClickAction");
    s.ctrlRightClickAction = ParseHudAction(actStr, HudAction::ToggleGraph);
    Wh_FreeStringSetting(actStr);

    PCWSTR dragStr = Wh_GetStringSetting(L"dragModifier");
    s.dragModifier = DragModifier::Alt;
    if (lstrcmpiW(dragStr, L"ctrl") == 0) s.dragModifier = DragModifier::Ctrl;
    else if (lstrcmpiW(dragStr, L"shift") == 0) s.dragModifier = DragModifier::Shift;
    else if (lstrcmpiW(dragStr, L"none") == 0) s.dragModifier = DragModifier::None;
    else if (lstrcmpiW(dragStr, L"disabled") == 0) s.dragModifier = DragModifier::Disabled;
    Wh_FreeStringSetting(dragStr);

    actStr = Wh_GetStringSetting(L"hoverAction");
    s.hoverAction = ParseHudAction(actStr, HudAction::None);
    Wh_FreeStringSetting(actStr);

    int rawHoverDelay = Wh_GetIntSetting(L"hoverDelayMs");
    if (rawHoverDelay <= 0) rawHoverDelay = 400;
    s.hoverDelayMs = (std::max)(100, (std::min)(3000, rawHoverDelay));

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
        if (row.OperStatus != 1) continue; // IfOperStatusUp == 1

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

    // Decimal kilobits per second: (bytes * 8 * 1000) / (elapsedMs * 1000) = (bytes * 8) / elapsedMs
    g_netDownKbps = (DWORD)((diffIn * 8ULL) / elapsedMs);
    g_netUpKbps   = (DWORD)((diffOut * 8ULL) / elapsedMs);
    s_prevNetCounters.swap(cur);
}

// PDH Monitor for Disk Read/Write, GPU Engine Utilization, and Thermal Zone Temperature (StatsWorkerThread only)
class PdhMonitor {
public:
    ~PdhMonitor() { Shutdown(); }

    void Update(bool needDisk, bool needGpu, bool needThermalCounter, bool readThermal,
                ULONGLONG& outReadBps, ULONGLONG& outWriteBps, int& outGpuPct, int& outThermalTempC) {
        if (!needDisk && !needGpu && !needThermalCounter) {
            if (m_initialized) Shutdown();
            return;
        }
        if (m_initialized && (needDisk != m_disk || needGpu != m_gpu || needThermalCounter != m_thermal)) {
            Shutdown();
        }
        if (!m_initialized) {
            if (!Init(needDisk, needGpu, needThermalCounter)) return;
        }

        if (PdhCollectQueryData(m_query) != ERROR_SUCCESS) {
            return;
        }

        if (needDisk) {
            PDH_FMT_COUNTERVALUE valRead = {}, valWrite = {};
            if (m_hDiskRead && PdhGetFormattedCounterValue(m_hDiskRead, PDH_FMT_LARGE, NULL, &valRead) == ERROR_SUCCESS) {
                outReadBps = (valRead.largeValue > 0) ? (ULONGLONG)valRead.largeValue : 0;
            }
            if (m_hDiskWrite && PdhGetFormattedCounterValue(m_hDiskWrite, PDH_FMT_LARGE, NULL, &valWrite) == ERROR_SUCCESS) {
                outWriteBps = (valWrite.largeValue > 0) ? (ULONGLONG)valWrite.largeValue : 0;
            }
        }

        if (needGpu && m_hGpuEngine) {
            DWORD bufSize = 0;
            DWORD itemCount = 0;
            PDH_STATUS status = PdhGetFormattedCounterArrayW(m_hGpuEngine, PDH_FMT_DOUBLE, &bufSize, &itemCount, NULL);
            if (status == (PDH_STATUS)PDH_MORE_DATA && bufSize > 0) {
                std::vector<BYTE> buf(bufSize);
                PPDH_FMT_COUNTERVALUE_ITEM_W items = (PPDH_FMT_COUNTERVALUE_ITEM_W)buf.data();
                if (PdhGetFormattedCounterArrayW(m_hGpuEngine, PDH_FMT_DOUBLE, &bufSize, &itemCount, items) == ERROR_SUCCESS) {
                    std::map<std::wstring, double> engineTotals;
                    for (DWORD i = 0; i < itemCount; i++) {
                        if (items[i].FmtValue.CStatus != PDH_CSTATUS_VALID_DATA &&
                            items[i].FmtValue.CStatus != PDH_CSTATUS_NEW_DATA) {
                            continue;
                        }
                        std::wstring engKey = L"default";
                        if (items[i].szName) {
                            // Instance format: pid_XXXX_luid_0x..._phys_N_eng_M_engtype_...
                            // Group per individual engine instance (from luid_ onward) to match Task Manager.
                            const wchar_t* p = wcsstr(items[i].szName, L"luid_");
                            engKey = p ? std::wstring(p) : std::wstring(items[i].szName);
                        }
                        engineTotals[engKey] += items[i].FmtValue.doubleValue;
                    }
                    double maxEngine = 0.0;
                    for (const auto& kv : engineTotals) {
                        if (kv.second > maxEngine) maxEngine = kv.second;
                    }
                    outGpuPct = (std::max)(0, (std::min)(100, (int)std::lround(maxEngine)));
                }
            }
        }

        if (readThermal && m_hThermalZone) {
            DWORD bufSize = 0;
            DWORD itemCount = 0;
            PDH_STATUS status = PdhGetFormattedCounterArrayW(m_hThermalZone, PDH_FMT_DOUBLE, &bufSize, &itemCount, NULL);
            if (status == (PDH_STATUS)PDH_MORE_DATA && bufSize > 0) {
                std::vector<BYTE> buf(bufSize);
                PPDH_FMT_COUNTERVALUE_ITEM_W items = (PPDH_FMT_COUNTERVALUE_ITEM_W)buf.data();
                if (PdhGetFormattedCounterArrayW(m_hThermalZone, PDH_FMT_DOUBLE, &bufSize, &itemCount, items) == ERROR_SUCCESS) {
                    int bestTemp = TEMP_UNAVAILABLE;
                    for (DWORD i = 0; i < itemCount; i++) {
                        if (items[i].FmtValue.CStatus != PDH_CSTATUS_VALID_DATA &&
                            items[i].FmtValue.CStatus != PDH_CSTATUS_NEW_DATA) {
                            continue;
                        }
                        double raw = items[i].FmtValue.doubleValue;
                        double c = -999.0;
                        if (raw > 2000.0 && raw < 4500.0) {
                            c = (raw / 10.0) - 273.15;
                        } else if (raw > 200.0 && raw < 450.0) {
                            c = raw - 273.15;
                        }
                        if (c > 0.0 && c < 150.0) {
                            int ic = (int)std::lround(c);
                            if (bestTemp == TEMP_UNAVAILABLE || ic > bestTemp) bestTemp = ic;
                        }
                    }
                    if (bestTemp != TEMP_UNAVAILABLE) {
                        outThermalTempC = bestTemp;
                    }
                }
            }
        }
    }

    void Shutdown() {
        if (m_query) {
            PdhCloseQuery(m_query);
            m_query = NULL;
        }
        m_hDiskRead = NULL;
        m_hDiskWrite = NULL;
        m_hGpuEngine = NULL;
        m_hThermalZone = NULL;
        m_disk = false;
        m_gpu = false;
        m_thermal = false;
        m_initialized = false;
    }

private:
    bool Init(bool needDisk, bool needGpu, bool needThermal) {
        if (PdhOpenQueryW(NULL, 0, &m_query) != ERROR_SUCCESS) {
            m_query = NULL;
            return false;
        }
        if (needDisk) {
            PdhAddEnglishCounterW(m_query, L"\\PhysicalDisk(_Total)\\Disk Read Bytes/sec", 0, &m_hDiskRead);
            PdhAddEnglishCounterW(m_query, L"\\PhysicalDisk(_Total)\\Disk Write Bytes/sec", 0, &m_hDiskWrite);
        }
        if (needGpu) {
            PdhAddEnglishCounterW(m_query, L"\\GPU Engine(*)\\Utilization Percentage", 0, &m_hGpuEngine);
        }
        if (needThermal) {
            PdhAddEnglishCounterW(m_query, L"\\Thermal Zone Information(*)\\Temperature", 0, &m_hThermalZone);
        }
        m_disk = needDisk;
        m_gpu = needGpu;
        m_thermal = needThermal;
        PdhCollectQueryData(m_query);
        m_initialized = true;
        return true;
    }

    PDH_HQUERY m_query = NULL;
    PDH_HCOUNTER m_hDiskRead = NULL;
    PDH_HCOUNTER m_hDiskWrite = NULL;
    PDH_HCOUNTER m_hGpuEngine = NULL;
    PDH_HCOUNTER m_hThermalZone = NULL;
    bool m_disk = false;
    bool m_gpu = false;
    bool m_thermal = false;
    bool m_initialized = false;
};

// NVIDIA NVAPI dynamic loader for GPU Temperature
class NvApiMonitor {
public:
    ~NvApiMonitor() { Shutdown(); }

    int GetGpuTempC() {
        if (!EnsureInit()) return TEMP_UNAVAILABLE;
        if (!m_pGetThermal || m_gpuCount == 0) return TEMP_UNAVAILABLE;

        NV_GPU_THERMAL_SETTINGS_V2 thermal = {};
        thermal.version = (2U << 16) | (unsigned int)sizeof(NV_GPU_THERMAL_SETTINGS_V2);
        if (m_pGetThermal(m_gpus[0], 15 /* ALL */, &thermal) == 0 && thermal.count > 0) {
            int t = thermal.sensor[0].currentTemp;
            if (t > 0 && t < 150) return t;
        }
        return TEMP_UNAVAILABLE;
    }

    void Shutdown() {
        if (m_initialized && m_pUnload) {
            m_pUnload();
        }
        if (m_hMod) {
            FreeLibrary(m_hMod);
            m_hMod = NULL;
        }
        m_triedInit = false;
        m_initialized = false;
        m_ready = false;
        m_gpuCount = 0;
        m_pUnload = nullptr;
        m_pGetThermal = nullptr;
    }

private:
    struct NV_GPU_THERMAL_SENSOR {
        int controller;
        int defaultMinTemp;
        int defaultMaxTemp;
        int currentTemp;
        int target;
    };
    struct NV_GPU_THERMAL_SETTINGS_V2 {
        unsigned int version;
        unsigned int count;
        NV_GPU_THERMAL_SENSOR sensor[3];
    };

    typedef void* (*NvAPI_QueryInterface_t)(unsigned int offset);
    typedef int (*NvAPI_Initialize_t)();
    typedef int (*NvAPI_Unload_t)();
    typedef int (*NvAPI_EnumPhysicalGPUs_t)(void* handles[64], unsigned int* count);
    typedef int (*NvAPI_GPU_GetThermalSettings_t)(void* handle, unsigned int sensorIndex, NV_GPU_THERMAL_SETTINGS_V2* thermal);

    bool EnsureInit() {
        if (m_triedInit) return m_ready;
        m_triedInit = true;

#ifdef _WIN64
        m_hMod = LoadLibraryExW(L"nvapi64.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
#else
        m_hMod = LoadLibraryExW(L"nvapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
#endif
        if (!m_hMod) return false;

        auto query = (NvAPI_QueryInterface_t)GetProcAddress(m_hMod, "nvapi_QueryInterface");
        if (!query) return false;

        auto pInit = (NvAPI_Initialize_t)query(0x0150E828);
        m_pUnload = (NvAPI_Unload_t)query(0xD22BDD7E);
        auto pEnum = (NvAPI_EnumPhysicalGPUs_t)query(0xE5AC921F);
        m_pGetThermal = (NvAPI_GPU_GetThermalSettings_t)query(0xE3640A56);

        if (!pInit || !pEnum || pInit() != 0) return false;
        m_initialized = true;

        if (pEnum(m_gpus, &m_gpuCount) != 0 || m_gpuCount == 0) return false;

        m_ready = true;
        return true;
    }

    HMODULE m_hMod = NULL;
    bool m_triedInit = false;
    bool m_initialized = false;
    bool m_ready = false;
    void* m_gpus[64] = {};
    unsigned int m_gpuCount = 0;
    NvAPI_Unload_t m_pUnload = nullptr;
    NvAPI_GPU_GetThermalSettings_t m_pGetThermal = nullptr;
};

// AMD ADL dynamic loader for GPU Temperature
class AdlMonitor {
public:
    ~AdlMonitor() { Shutdown(); }

    int GetGpuTempC() {
        if (!EnsureInit()) return TEMP_UNAVAILABLE;
        if (!m_pOverdrive5Temp) return TEMP_UNAVAILABLE;

        ADLTemperature temp = {};
        temp.iSize = sizeof(ADLTemperature);
        if (m_pOverdrive5Temp(m_adapterIndex, 0, &temp) == 0) {
            int c = temp.iTemperature / 1000;
            if (c > 0 && c < 150) return c;
        }
        return TEMP_UNAVAILABLE;
    }

    void Shutdown() {
        if (m_pDestroy && m_created) {
            m_pDestroy();
        }
        if (m_hMod) {
            FreeLibrary(m_hMod);
            m_hMod = NULL;
        }
        m_triedInit = false;
        m_created = false;
        m_ready = false;
        m_pDestroy = nullptr;
        m_pOverdrive5Temp = nullptr;
    }

private:
    struct ADLTemperature {
        int iSize;
        int iTemperature;
    };
    typedef void* (__stdcall* ADL_MAIN_MALLOC_CALLBACK)(int);
    typedef int (*ADL_Main_Control_Create_t)(ADL_MAIN_MALLOC_CALLBACK, int);
    typedef int (*ADL_Main_Control_Destroy_t)();
    typedef int (*ADL_Adapter_NumberOfAdapters_Get_t)(int*);
    typedef int (*ADL_Overdrive5_Temperature_Get_t)(int, int, ADLTemperature*);

    static void* __stdcall AdlAlloc(int iSize) {
        return malloc((size_t)iSize);
    }

    bool EnsureInit() {
        if (m_triedInit) return m_ready;
        m_triedInit = true;

        m_hMod = LoadLibraryExW(L"atiadlxx.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!m_hMod) m_hMod = LoadLibraryExW(L"atiadlxy.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!m_hMod) return false;

        auto pCreate = (ADL_Main_Control_Create_t)GetProcAddress(m_hMod, "ADL_Main_Control_Create");
        m_pDestroy = (ADL_Main_Control_Destroy_t)GetProcAddress(m_hMod, "ADL_Main_Control_Destroy");
        auto pNumAdapters = (ADL_Adapter_NumberOfAdapters_Get_t)GetProcAddress(m_hMod, "ADL_Adapter_NumberOfAdapters_Get");
        m_pOverdrive5Temp = (ADL_Overdrive5_Temperature_Get_t)GetProcAddress(m_hMod, "ADL_Overdrive5_Temperature_Get");

        if (!pCreate || !m_pDestroy || !pNumAdapters || pCreate(AdlAlloc, 1) != 0) return false;
        m_created = true;

        int num = 0;
        if (pNumAdapters(&num) != 0 || num <= 0) return false;

        m_ready = true;
        return true;
    }

    HMODULE m_hMod = NULL;
    bool m_triedInit = false;
    bool m_created = false;
    bool m_ready = false;
    int m_adapterIndex = 0;
    ADL_Main_Control_Destroy_t m_pDestroy = nullptr;
    ADL_Overdrive5_Temperature_Get_t m_pOverdrive5Temp = nullptr;
};

// WMI Thermal Monitor (LibreHardwareMonitor / OpenHardwareMonitor)
class WmiThermalMonitor {
public:
    ~WmiThermalMonitor() { Shutdown(); }

    void QueryTemps(bool needCpu, bool needGpu, int& outCpuTempC, int& outGpuTempC) {
        if (!needCpu && !needGpu) return;

        IWbemServices* pSvc = GetNamespace(L"root\\LibreHardwareMonitor", m_pLhm, m_lastLhmTry);
        bool usingLhm = (pSvc != nullptr);
        if (!pSvc) {
            pSvc = GetNamespace(L"root\\OpenHardwareMonitor", m_pOhm, m_lastOhmTry);
        }
        if (pSvc) {
            if (!QueryHardwareMonitor(pSvc, needCpu, needGpu, outCpuTempC, outGpuTempC)) {
                if (usingLhm && m_pLhm) { m_pLhm->Release(); m_pLhm = nullptr; }
                else if (!usingLhm && m_pOhm) { m_pOhm->Release(); m_pOhm = nullptr; }
            }
        }
    }

    void Shutdown() {
        if (m_pLhm) { m_pLhm->Release(); m_pLhm = nullptr; }
        if (m_pOhm) { m_pOhm->Release(); m_pOhm = nullptr; }
        if (m_pLocator) { m_pLocator->Release(); m_pLocator = nullptr; }
        m_lastLhmTry = 0;
        m_lastOhmTry = 0;
    }

private:
    IWbemServices* GetNamespace(const wchar_t* ns, IWbemServices*& cache, ULONGLONG& lastTry) {
        if (cache) return cache;
        ULONGLONG now = GetTickCount64();
        if (lastTry != 0 && (now - lastTry) < 30000ULL) return nullptr;
        lastTry = now;

        if (!m_pLocator) {
            HRESULT hr = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&m_pLocator);
            if (FAILED(hr) || !m_pLocator) return nullptr;
        }

        BSTR bstrNs = SysAllocString(ns);
        if (!bstrNs) return nullptr;
        HRESULT hr = m_pLocator->ConnectServer(bstrNs, NULL, NULL, 0, 0, 0, 0, &cache);
        SysFreeString(bstrNs);

        if (FAILED(hr) || !cache) {
            cache = nullptr;
            return nullptr;
        }

        CoSetProxyBlanket(cache, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
                          RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
        return cache;
    }

    bool QueryHardwareMonitor(IWbemServices* pSvc, bool needCpu, bool needGpu, int& outCpuTempC, int& outGpuTempC) {
        BSTR bstrWql = SysAllocString(L"WQL");
        BSTR bstrQuery = SysAllocString(L"SELECT Name, Identifier, Value FROM Sensor WHERE SensorType='Temperature'");
        IEnumWbemClassObject* pEnum = nullptr;
        HRESULT hr = pSvc->ExecQuery(bstrWql, bstrQuery, WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnum);
        SysFreeString(bstrWql);
        SysFreeString(bstrQuery);
        if (FAILED(hr) || !pEnum) return false;

        IWbemClassObject* pObj = nullptr;
        ULONG ret = 0;
        while (pEnum->Next(1000, 1, &pObj, &ret) == S_OK) {
            VARIANT vtId, vtName, vtVal;
            VariantInit(&vtId); VariantInit(&vtName); VariantInit(&vtVal);
            pObj->Get(L"Identifier", 0, &vtId, 0, 0);
            pObj->Get(L"Name", 0, &vtName, 0, 0);
            pObj->Get(L"Value", 0, &vtVal, 0, 0);

            double valC = -999.0;
            if (vtVal.vt == VT_R4) valC = vtVal.fltVal;
            else if (vtVal.vt == VT_R8) valC = vtVal.dblVal;

            if (valC > 0.0 && valC < 150.0) {
                std::wstring id = (vtId.vt == VT_BSTR && vtId.bstrVal) ? vtId.bstrVal : L"";
                std::wstring name = (vtName.vt == VT_BSTR && vtName.bstrVal) ? vtName.bstrVal : L"";
                for (wchar_t& c : id) c = (wchar_t)towlower(c);
                for (wchar_t& c : name) c = (wchar_t)towlower(c);

                if (needCpu && outCpuTempC == TEMP_UNAVAILABLE &&
                    (id.find(L"cpu") != std::wstring::npos || name.find(L"cpu") != std::wstring::npos ||
                     name.find(L"package") != std::wstring::npos || name.find(L"tctl") != std::wstring::npos)) {
                    outCpuTempC = (int)std::lround(valC);
                }
                if (needGpu && outGpuTempC == TEMP_UNAVAILABLE &&
                    (id.find(L"gpu") != std::wstring::npos || name.find(L"gpu") != std::wstring::npos)) {
                    outGpuTempC = (int)std::lround(valC);
                }
            }

            VariantClear(&vtId); VariantClear(&vtName); VariantClear(&vtVal);
            pObj->Release();
        }
        pEnum->Release();
        return true;
    }

    IWbemLocator* m_pLocator = nullptr;
    IWbemServices* m_pLhm = nullptr;
    IWbemServices* m_pOhm = nullptr;
    ULONGLONG m_lastLhmTry = 0;
    ULONGLONG m_lastOhmTry = 0;
};

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

void ExecuteHudAction(HudAction action) {
    switch (action) {
        case HudAction::TrimRam:
            TriggerAsyncRamClean();
            break;
        case HudAction::ToggleGraph:
            ToggleGraphWindow();
            break;
        case HudAction::None:
        default:
            break;
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

void FormatTempString(int tempC, TempUnit unit, wchar_t* buf, size_t bufLen) {
    if (tempC == TEMP_UNAVAILABLE) {
        wcscpy_s(buf, bufLen, L"N/A");
        return;
    }
    if (unit == TempUnit::Fahrenheit) {
        int f = (int)std::lround(tempC * 9.0 / 5.0 + 32.0);
        swprintf_s(buf, bufLen, L"%d\u00B0F", f);
    } else {
        swprintf_s(buf, bufLen, L"%d\u00B0C", tempC);
    }
}

void FormatBytesRate(ULONGLONG bps, wchar_t* buf, size_t bufLen) {
    if (bps >= 1024ULL * 1024ULL) {
        ULONGLONG tenths = (bps * 10ULL) / (1024ULL * 1024ULL);
        swprintf_s(buf, bufLen, L"%llu.%llu MB/s", tenths / 10ULL, tenths % 10ULL);
    } else {
        ULONGLONG kb = bps / 1024ULL;
        swprintf_s(buf, bufLen, L"%llu KB/s", kb);
    }
}

void FormatDiskString(DiskSpeedMode mode, ULONGLONG readBps, ULONGLONG writeBps, wchar_t* buf, size_t bufLen) {
    wchar_t rBuf[24], wBuf[24];
    switch (mode) {
        case DiskSpeedMode::Total:
            FormatBytesRate(readBps + writeBps, rBuf, 24);
            swprintf_s(buf, bufLen, L"D:%s", rBuf);
            break;
        case DiskSpeedMode::ReadOnly:
            FormatBytesRate(readBps, rBuf, 24);
            swprintf_s(buf, bufLen, L"R:%s", rBuf);
            break;
        case DiskSpeedMode::WriteOnly:
            FormatBytesRate(writeBps, wBuf, 24);
            swprintf_s(buf, bufLen, L"W:%s", wBuf);
            break;
        case DiskSpeedMode::RW:
            FormatBytesRate(readBps, rBuf, 24);
            FormatBytesRate(writeBps, wBuf, 24);
            swprintf_s(buf, bufLen, L"R:%s W:%s", rBuf, wBuf);
            break;
        default:
            buf[0] = L'\0';
            break;
    }
}

void FormatStrings(wchar_t* cpuVal, wchar_t* ramVal, wchar_t* gpuVal, wchar_t* pingVal, wchar_t* netVal) {
    wsprintfW(cpuVal, L"%d%%", g_cpuUsage.load());
    wsprintfW(ramVal, L"%d%%", g_ramUsage.load());
    wsprintfW(gpuVal, L"%d%%", g_gpuUsage.load());

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

// Determines if the taskbar is retracted by auto-hide or hidden by styling mods
bool IsTaskbarHiddenOrRetracted(HWND hTray) {
    if (!hTray || !IsWindowVisible(hTray)) return true;

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

    int trayHeight = rcTray.bottom - rcTray.top;

    // Detect if the taskbar is sliding down or retracted
    if (rcTray.top > mi.rcMonitor.bottom - (trayHeight > 30 ? (trayHeight - 8) : 15)) return true;
    if (rcTray.bottom < mi.rcMonitor.top + (trayHeight > 30 ? (trayHeight - 8) : 15)) return true;
    if (rcTray.left > mi.rcMonitor.right - 15) return true;
    if (rcTray.right < mi.rcMonitor.left + 15) return true;

    return false;
}

// Instantly detects if any window is running fullscreen (e.g. F11 in browsers, games, media players)
bool IsFullscreenAppActive(HWND hTaskbar) {
    HWND hFg = GetForegroundWindow();
    if (!hFg) return false;

    HWND hHud = g_hHudWnd.load();
    if (hFg == hTaskbar || hFg == hHud) return false;

    // Do not hide when the desktop itself is focused
    WCHAR className[32] = {};
    if (GetClassNameW(hFg, className, 32)) {
        if (wcscmp(className, L"Progman") == 0 || wcscmp(className, L"WorkerW") == 0) {
            return false;
        }
    }

    // With an auto-hide taskbar the work area is the whole monitor, so a
    // maximized window's rect (including its invisible resize borders) covers
    // the monitor too. A maximized window that still has a caption isn't
    // fullscreen.
    if (IsZoomed(hFg) &&
        (GetWindowLongPtrW(hFg, GWL_STYLE) & WS_CAPTION) == WS_CAPTION) {
        return false;
    }

    HMONITOR hMonTray = MonitorFromWindow(hTaskbar ? hTaskbar : hHud, MONITOR_DEFAULTTOPRIMARY);
    HMONITOR hMonFg = MonitorFromWindow(hFg, MONITOR_DEFAULTTONEAREST);
    if (hMonTray != hMonFg) return false;

    RECT rcFg;
    MONITORINFO mi = { sizeof(mi) };
    if (GetWindowRect(hFg, &rcFg) && GetMonitorInfoW(hMonTray, &mi)) {
        // Fast direct geometry check: window covers or exceeds full monitor bounds (F11)
        if (rcFg.left <= mi.rcMonitor.left && rcFg.top <= mi.rcMonitor.top &&
            rcFg.right >= mi.rcMonitor.right && rcFg.bottom >= mi.rcMonitor.bottom) {
            return true;
        }
    }

    return false;
}

// Renders the mini-graph popup on Ctrl + Right Click (or configured shortcut/hover)
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
        bool customBg, customBgHasAlpha, customBorder, showGpu;
        Gdiplus::Color cBg, cBorder;
        {
            std::lock_guard<std::mutex> lock(g_settingsMutex);
            bgAlpha = settings.bgAlpha;
            cornerPref = settings.cornerPref;
            interval = settings.updateInterval;
            customBg = settings.customBgEnabled;
            customBgHasAlpha = settings.customBgHasAlpha;
            cBg = settings.customBgColor;
            customBorder = settings.customBorderEnabled;
            cBorder = settings.customBorderColor;
            showGpu = settings.showGpu;
        }

        bool isLight = g_isTaskbarLight;
        Gdiplus::Color bgColor, borderColor;
        if (customBg) {
            BYTE a = customBgHasAlpha ? (std::max)((BYTE)40, cBg.GetAlpha()) : (BYTE)bgAlpha;
            bgColor = Gdiplus::Color(a, cBg.GetRed(), cBg.GetGreen(), cBg.GetBlue());
            borderColor = Gdiplus::Color(255, (std::min)(255, cBg.GetRed() + 45), (std::min)(255, cBg.GetGreen() + 45), (std::min)(255, cBg.GetBlue() + 45));
        } else if (isLight) {
            bgColor = Gdiplus::Color(bgAlpha, 245, 246, 250);
            borderColor = Gdiplus::Color(255, 180, 185, 195);
        } else {
            bgColor = Gdiplus::Color(bgAlpha, 22, 24, 30);
            borderColor = Gdiplus::Color(255, 60, 65, 78);
        }

        if (customBorder) {
            borderColor = cBorder;
        }

        int radius = (cornerPref == 1) ? 0 : (cornerPref == 3 ? ScaleDpi(3, g_currentDpi) : ScaleDpi(6, g_currentDpi));
        Gdiplus::GraphicsPath bgPath;
        AddRoundedRect(bgPath, 0, 0, width - 1, height - 1, radius);

        Gdiplus::SolidBrush bgBrush(bgColor);
        g.FillPath(&bgBrush, &bgPath);

        Gdiplus::Pen borderPen(borderColor, 1.0f);
        g.DrawPath(&borderPen, &bgPath);

        // Header with dynamically calculated duration and right-to-left measured legends
        int headerY = ScaleDpi(6, g_currentDpi);
        int pad = ScaleDpi(10, g_currentDpi);
        int legGap = ScaleDpi(8, g_currentDpi);
        int curRight = width - pad;

        wchar_t ramLeg[24], cpuLeg[24], gpuLeg[24];
        wsprintfW(ramLeg, L"RAM %d%%", g_ramUsage.load());
        wsprintfW(cpuLeg, L"CPU %d%%", g_cpuUsage.load());
        wsprintfW(gpuLeg, L"GPU %d%%", g_gpuUsage.load());

        int ramW = MeasureTextWidth(g, ramLeg, g_pFontSmall);
        int cpuW = MeasureTextWidth(g, cpuLeg, g_pFontSmall);
        int gpuW = showGpu ? MeasureTextWidth(g, gpuLeg, g_pFontSmall) : 0;

        Gdiplus::StringFormat legFmt(Gdiplus::StringFormat::GenericTypographic());
        legFmt.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);

        curRight -= ramW;
        if (curRight > pad) {
            Gdiplus::SolidBrush ramBrush(Gdiplus::Color(255, 175, 130, 255));
            g.DrawString(ramLeg, -1, g_pFontSmall, Gdiplus::PointF((Gdiplus::REAL)curRight, (Gdiplus::REAL)headerY), &legFmt, &ramBrush);
        }

        curRight -= (legGap + cpuW);
        if (curRight > pad) {
            Gdiplus::SolidBrush cpuBrush(Gdiplus::Color(255, 75, 175, 255));
            g.DrawString(cpuLeg, -1, g_pFontSmall, Gdiplus::PointF((Gdiplus::REAL)curRight, (Gdiplus::REAL)headerY), &legFmt, &cpuBrush);
        }

        if (showGpu) {
            int nextRight = curRight - (legGap + gpuW);
            if (nextRight > pad) {
                curRight = nextRight;
                Gdiplus::SolidBrush gpuBrush(Gdiplus::Color(255, 110, 230, 145));
                g.DrawString(gpuLeg, -1, g_pFontSmall, Gdiplus::PointF((Gdiplus::REAL)curRight, (Gdiplus::REAL)headerY), &legFmt, &gpuBrush);
            }
        }

        int totalSec = (int)((MAX_GRAPH_POINTS * interval) / 1000);
        wchar_t titleBuf[48];
        wsprintfW(titleBuf, L"Activity (Last %ds)", totalSec);
        int availTitleW = curRight - pad - legGap;
        if (MeasureTextWidth(g, titleBuf, g_pFontSmall) > availTitleW) {
            wsprintfW(titleBuf, L"%ds", totalSec);
        }
        if (MeasureTextWidth(g, titleBuf, g_pFontSmall) <= availTitleW) {
            Gdiplus::Color colText = isLight ? Gdiplus::Color(255, 30, 32, 38) : Gdiplus::Color(255, 240, 242, 246);
            Gdiplus::SolidBrush textBrush(colText);
            g.DrawString(titleBuf, -1, g_pFontSmall, Gdiplus::PointF((Gdiplus::REAL)pad, (Gdiplus::REAL)headerY), &legFmt, &textBrush);
        }

        int gx = pad;
        int gy = headerY + ScaleDpi((std::max)(18, g_currentFontSize + 6), g_currentDpi);
        int gw = width - gx * 2;
        int gh = height - gy - ScaleDpi(8, g_currentDpi);

        if (gw > 10 && gh > 10) {
            Gdiplus::Pen gridPen(isLight ? Gdiplus::Color(70, 180, 185, 195) : Gdiplus::Color(60, 255, 255, 255), 1.0f);
            g.DrawLine(&gridPen, gx, gy, gx + gw, gy);
            g.DrawLine(&gridPen, gx, gy + gh / 2, gx + gw, gy + gh / 2);
            g.DrawLine(&gridPen, gx, gy + gh, gx + gw, gy + gh);

            std::vector<int> cpuPts, ramPts, gpuPts;
            {
                std::lock_guard<std::mutex> lock(g_historyMutex);
                cpuPts.assign(g_cpuHistory.begin(), g_cpuHistory.end());
                ramPts.assign(g_ramHistory.begin(), g_ramHistory.end());
                gpuPts.assign(g_gpuHistory.begin(), g_gpuHistory.end());
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

            if (showGpu) {
                DrawGraphLine(gpuPts, Gdiplus::Color(255, 110, 230, 145), Gdiplus::Color(35, 110, 230, 145));
            }
            DrawGraphLine(ramPts, Gdiplus::Color(255, 175, 130, 255), Gdiplus::Color(45, 175, 130, 255));
            DrawGraphLine(cpuPts, Gdiplus::Color(255, 75, 175, 255), Gdiplus::Color(55, 75, 175, 255));
        }
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
            g_graphOpenedByHover = false;
            ShowWindow(hWnd, SW_HIDE);
            return 0;

        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) {
                g_graphOpenedByHover = false;
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

void ShowGraphWindow(bool activate) {
    HWND hGraph = g_hGraphWnd;
    HWND hHud = g_hHudWnd;
    if (!hGraph || !hHud) return;

    int cfgW, cfgH;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        cfgW = settings.graphWidth;
        cfgH = settings.graphHeight;
    }

    RECT rcHud;
    GetWindowRect(hHud, &rcHud);
    UINT dpi = g_currentDpi;

    int gw = ScaleDpi(cfgW, dpi);
    int gh = ScaleDpi(cfgH, dpi);
    int gx = rcHud.left + (rcHud.right - rcHud.left - gw) / 2;
    int gy = rcHud.top - gh - ScaleDpi(8, dpi);

    HMONITOR hMon = MonitorFromWindow(hHud, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    if (!GetMonitorInfoW(hMon, &mi)) return;

    if (gx < mi.rcWork.left) gx = mi.rcWork.left;
    if (gx + gw > mi.rcWork.right) gx = mi.rcWork.right - gw;
    if (gy < mi.rcWork.top) gy = rcHud.bottom + ScaleDpi(8, dpi);

    SetWindowPos(hGraph, HWND_TOPMOST, gx, gy, gw, gh, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    if (activate) {
        SetForegroundWindow(hGraph);
        SetFocus(hGraph);
    }
    RenderGraphPopup(hGraph);
}

void ToggleGraphWindow() {
    HWND hGraph = g_hGraphWnd;
    if (!hGraph) return;

    if (IsWindowVisible(hGraph)) {
        g_graphOpenedByHover = false;
        ShowWindow(hGraph, SW_HIDE);
        return;
    }

    g_graphOpenedByHover = false;
    ShowGraphWindow(true);
}

bool CalculateHudPosition(HWND hTray, int exactWidth, int height, int& posX, int& posY) {
    if (!hTray) return false;
    RECT rcTray = {};
    if (!GetWindowRect(hTray, &rcTray)) return false;

    UINT dpi = SafeGetWindowDpi(hTray);
    int trayH = rcTray.bottom - rcTray.top;

    int offsetX;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        offsetX = settings.offsetX;
    }

    posX = rcTray.right - exactWidth - ScaleDpi(200, dpi);
    posY = rcTray.top + (trayH - height) / 2;

    HWND hNotify = FindWindowExW(hTray, NULL, L"TrayNotifyWnd", NULL);
    if (hNotify && IsWindowVisible(hNotify)) {
        RECT rcNotify;
        if (GetWindowRect(hNotify, &rcNotify) && rcNotify.right > rcNotify.left) {
            posX = rcNotify.left - exactWidth - ScaleDpi(offsetX, dpi);
            int notifyH = rcNotify.bottom - rcNotify.top;
            if (notifyH > 0) {
                posY = rcNotify.top + (notifyH - height) / 2;
            }
        }
    }
    return true;
}

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

    int posX = 0, posY = 0;
    if (!CalculateHudPosition(hTray, exactWidth, height, posX, posY)) return;

    // Early return: if already at the target position, avoid unnecessary SetWindowPos
    if (IsWindowVisible(hWnd) && rcHud.left == posX && rcHud.top == posY) {
        return;
    }

    // Reassert topmost when showing from hidden state so it never gets obscured by taskbar
    if (!IsWindowVisible(hWnd)) {
        SetWindowPos(hWnd, HWND_TOPMOST, posX, posY, 0, 0,
                     SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
        return;
    }

    SetWindowPos(hWnd, NULL, posX, posY, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

// GUI thread only: computes dimensions and paints with per-pixel alpha via UpdateLayeredWindow
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

    ModSettings s;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        s = settings;
    }

    UINT dpi = SafeGetWindowDpi(hTray);
    if (dpi != g_currentDpi || s.fontSize != g_currentFontSize || !g_pFontLabel) {
        UpdateCachedFonts(dpi, s.fontSize);
    }

    bool showCpu = s.showCpu;
    bool showCpuTemp = s.showCpuTemp;
    bool showRam = s.showRam;
    bool showGpu = s.showGpu;
    bool showGpuTemp = s.showGpuTemp;
    bool showDisk = (s.diskSpeedMode != DiskSpeedMode::Off);
    bool showPing = s.showPing;
    bool showNet = s.showNetSpeed;
    bool showBars = s.showMiniBars;

    if (!showCpu && !showCpuTemp && !showRam && !showGpu && !showGpuTemp && !showDisk && !showPing && !showNet) {
        showCpu = true;
    }

    wchar_t cpuVal[16], ramVal[16], gpuVal[16], pingVal[24], netVal[32];
    FormatStrings(cpuVal, ramVal, gpuVal, pingVal, netVal);

    wchar_t cpuTempVal[16], gpuTempVal[16], diskVal[48];
    FormatTempString(g_cpuTempC.load(), s.tempUnit, cpuTempVal, 16);
    FormatTempString(g_gpuTempC.load(), s.tempUnit, gpuTempVal, 16);
    FormatDiskString(s.diskSpeedMode, g_diskReadBps.load(), g_diskWriteBps.load(), diskVal, 48);

    HDC screenDC = GetDC(NULL);
    HDC tempDC = CreateCompatibleDC(screenDC);

    int textW = 0;
    int padX = ScaleDpi(11, dpi);
    bool inStatus = g_statusMessageTime && (GetTickCount64() - g_statusMessageTime < g_statusDuration);

    {
        Gdiplus::Graphics measureGraphics(tempDC);
        int sepW = MeasureTextWidth(measureGraphics, L"  ", g_pFontLabel);
        int spaceW = MeasureTextWidth(measureGraphics, L" ", g_pFontLabel);
        int items = 0;

        if (inStatus) {
            textW = MeasureTextWidth(measureGraphics, g_statusMessage.c_str(), g_pFontValue) + ScaleDpi(8, dpi);
        } else {
            if (showCpu || showCpuTemp) {
                if (items++ > 0) textW += sepW;
                textW += MeasureTextWidth(measureGraphics, L"CPU ", g_pFontLabel);
                if (showCpu) textW += MeasureTextWidth(measureGraphics, cpuVal, g_pFontValue);
                if (showCpu && showCpuTemp) textW += spaceW;
                if (showCpuTemp) textW += MeasureTextWidth(measureGraphics, cpuTempVal, g_pFontValue);
            }
            if (showRam) {
                if (items++ > 0) textW += sepW;
                textW += MeasureTextWidth(measureGraphics, L"RAM ", g_pFontLabel) + MeasureTextWidth(measureGraphics, ramVal, g_pFontValue);
            }
            if (showGpu || showGpuTemp) {
                if (items++ > 0) textW += sepW;
                textW += MeasureTextWidth(measureGraphics, L"GPU ", g_pFontLabel);
                if (showGpu) textW += MeasureTextWidth(measureGraphics, gpuVal, g_pFontValue);
                if (showGpu && showGpuTemp) textW += spaceW;
                if (showGpuTemp) textW += MeasureTextWidth(measureGraphics, gpuTempVal, g_pFontValue);
            }
            if (showDisk) {
                if (items++ > 0) textW += sepW;
                textW += MeasureTextWidth(measureGraphics, diskVal, g_pFontValue);
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

    int minW = ScaleDpi(s.widgetMinWidth, dpi);
    int exactWidth = (std::max)(minW, textW + padX * 2);
    if (s.widgetMaxWidth > 0) {
        int maxW = ScaleDpi(s.widgetMaxWidth, dpi);
        exactWidth = (std::min)(exactWidth, maxW);
    }

    int height = ScaleDpi(32, dpi);
    if (s.widgetHeight > 0) {
        height = ScaleDpi(s.widgetHeight, dpi);
    } else {
        RECT rcTray = {};
        GetWindowRect(hTray, &rcTray);
        int trayH = rcTray.bottom - rcTray.top;
        if (trayH >= ScaleDpi(28, dpi) && trayH <= ScaleDpi(64, dpi)) {
            height = trayH - ScaleDpi(10, dpi);
        }
    }

    int posX = 0, posY = 0;
    CalculateHudPosition(hTray, exactWidth, height, posX, posY);

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

        if (s.customBgEnabled) {
            BYTE a = s.customBgHasAlpha ? (std::max)((BYTE)40, s.customBgColor.GetAlpha()) : (BYTE)s.bgAlpha;
            BYTE r = s.customBgColor.GetRed();
            BYTE gr = s.customBgColor.GetGreen();
            BYTE b = s.customBgColor.GetBlue();
            bgColor = g_isHovered ? Gdiplus::Color(a, (std::min)(255, r + 15), (std::min)(255, gr + 15), (std::min)(255, b + 15))
                                  : Gdiplus::Color(a, r, gr, b);
            borderColor = g_isHovered ? Gdiplus::Color(255, 95, 150, 225)
                                      : Gdiplus::Color(180, (std::min)(255, r + 35), (std::min)(255, gr + 35), (std::min)(255, b + 35));
        } else if (isLight) {
            bgColor = g_isHovered ? Gdiplus::Color(s.bgAlpha, 225, 228, 235) : Gdiplus::Color(s.bgAlpha, 242, 243, 247);
            borderColor = g_isHovered ? Gdiplus::Color(255, 100, 145, 220) : Gdiplus::Color(180, 180, 185, 195);
        } else {
            bgColor = g_isHovered ? Gdiplus::Color(s.bgAlpha, 32, 34, 42) : Gdiplus::Color(s.bgAlpha, 20, 21, 26);
            borderColor = g_isHovered ? Gdiplus::Color(255, 95, 150, 225) : Gdiplus::Color(180, 50, 54, 64);
        }

        if (s.customBorderEnabled) {
            borderColor = s.customBorderColor;
        }

        int radius = 0;
        if (s.cornerPref == 2) radius = ScaleDpi(6, dpi);
        else if (s.cornerPref == 3) radius = ScaleDpi(3, dpi);

        Gdiplus::GraphicsPath bgPath;
        AddRoundedRect(bgPath, 0, 0, exactWidth - 1, height - 1, radius);

        Gdiplus::SolidBrush bgBrush(bgColor);
        g.FillPath(&bgBrush, &bgPath);

        Gdiplus::Pen borderPen(borderColor, 1.0f);
        g.DrawPath(&borderPen, &bgPath);

        // Clip inner content so custom max width never overflows the rounded border
        g.SetClip(Gdiplus::Rect(ScaleDpi(4, dpi), 0, (std::max)(0, exactWidth - ScaleDpi(8, dpi)), height));

        if (inStatus) {
            Gdiplus::StringFormat sf;
            sf.SetAlignment(Gdiplus::StringAlignmentCenter);
            sf.SetLineAlignment(Gdiplus::StringAlignmentCenter);
            Gdiplus::SolidBrush statusBrush(g_statusColor);
            Gdiplus::RectF layoutRect(0, 0, (Gdiplus::REAL)exactWidth, (Gdiplus::REAL)height);
            g.DrawString(g_statusMessage.c_str(), -1, g_pFontValue, layoutRect, &sf, &statusBrush);
        } else {
            int cpu = g_cpuUsage.load();
            int cpuTemp = g_cpuTempC.load();
            int ram = g_ramUsage.load();
            int gpu = g_gpuUsage.load();
            int gpuTemp = g_gpuTempC.load();
            int ping = g_pingMs.load();

            Gdiplus::Color colLabel = isLight ? (g_isHovered ? Gdiplus::Color(255, 90, 95, 105) : Gdiplus::Color(255, 115, 120, 130))
                                              : (g_isHovered ? Gdiplus::Color(255, 175, 180, 190) : Gdiplus::Color(255, 140, 145, 155));
            Gdiplus::Color colNormal = isLight ? Gdiplus::Color(255, 25, 27, 32) : Gdiplus::Color(255, 245, 246, 250);
            Gdiplus::Color colWarn   = Gdiplus::Color(255, 255, 190, 50);
            Gdiplus::Color colAlert  = Gdiplus::Color(255, 255, 85, 85);

            Gdiplus::Color cpuCol = (cpu > 80) ? colAlert : (cpu > 55 ? colWarn : colNormal);
            Gdiplus::Color ramCol = (ram > 85) ? colAlert : (ram > 70 ? colWarn : colNormal);
            Gdiplus::Color gpuCol = (gpu > 85) ? colAlert : (gpu > 60 ? colWarn : (isLight ? Gdiplus::Color(255, 30, 160, 70) : Gdiplus::Color(255, 110, 230, 145)));
            Gdiplus::Color cpuTempCol = (cpuTemp == TEMP_UNAVAILABLE) ? colLabel : ((cpuTemp >= 85) ? colAlert : ((cpuTemp >= 70) ? colWarn : colNormal));
            Gdiplus::Color gpuTempCol = (gpuTemp == TEMP_UNAVAILABLE) ? colLabel : ((gpuTemp >= 85) ? colAlert : ((gpuTemp >= 70) ? colWarn : colNormal));
            Gdiplus::Color netCol = isLight ? Gdiplus::Color(255, 20, 140, 220) : Gdiplus::Color(255, 115, 215, 255);
            Gdiplus::Color diskCol = isLight ? Gdiplus::Color(255, 130, 70, 210) : Gdiplus::Color(255, 200, 160, 255);

            Gdiplus::Color pingCol;
            if (ping == PING_PENDING) pingCol = colLabel;
            else if (ping < 0 || ping > 130) pingCol = colAlert;
            else if (ping > 75) pingCol = colWarn;
            else pingCol = isLight ? Gdiplus::Color(255, 30, 160, 70) : Gdiplus::Color(255, 125, 235, 155);

            if (s.customMetricsEnabled) {
                colLabel   = s.customLabelColor;
                cpuCol     = s.customCpuColor;
                ramCol     = s.customRamColor;
                gpuCol     = s.customGpuColor;
                cpuTempCol = s.customTempColor;
                gpuTempCol = s.customTempColor;
                netCol     = s.customNetColor;
                diskCol    = s.customDiskColor;
                pingCol    = s.customPingColor;
            }

            bool drawBars = showBars && (showCpu || showRam || showGpu);
            int lineH = ScaleDpi(s.fontSize + 4, dpi);
            int barOffset = drawBars ? ScaleDpi(2, dpi) : 0;
            int yPos = (height - lineH) / 2 - barOffset;
            int barY = yPos + lineH;
            int xPos = ScaleDpi(11, dpi);
            int curItem = 0;

            if (showCpu || showCpuTemp) {
                if (curItem++ > 0) DrawTextSeg(g, xPos, yPos, L"  ", colLabel, g_pFontLabel);
                int cpuStartX = xPos;
                DrawTextSeg(g, xPos, yPos, L"CPU ", colLabel, g_pFontLabel);
                if (showCpu) {
                    DrawTextSeg(g, xPos, yPos, cpuVal, cpuCol, g_pFontValue);
                }
                int cpuEndX = xPos;
                if (showCpu && showCpuTemp) DrawTextSeg(g, xPos, yPos, L" ", colLabel, g_pFontLabel);
                if (showCpuTemp) {
                    DrawTextSeg(g, xPos, yPos, cpuTempVal, cpuTempCol, g_pFontValue);
                }

                if (showBars && showCpu) {
                    Gdiplus::Color barCol = s.customMetricsEnabled ? cpuCol :
                                           ((cpu > 80) ? colAlert : ((cpu > 55) ? colWarn : Gdiplus::Color(255, 75, 175, 255)));
                    DrawRoundedMiniBarGDIPlus(g, cpuStartX, barY, cpuEndX - cpuStartX, ScaleDpi(3, dpi), cpu, barCol);
                }
            }

            if (showRam) {
                if (curItem++ > 0) DrawTextSeg(g, xPos, yPos, L"  ", colLabel, g_pFontLabel);
                int ramStartX = xPos;
                DrawTextSeg(g, xPos, yPos, L"RAM ", colLabel, g_pFontLabel);
                DrawTextSeg(g, xPos, yPos, ramVal, ramCol, g_pFontValue);
                int ramEndX = xPos;

                if (showBars) {
                    Gdiplus::Color barCol = s.customMetricsEnabled ? ramCol :
                                           ((ram > 85) ? colAlert : ((ram > 70) ? colWarn : Gdiplus::Color(255, 165, 130, 255)));
                    DrawRoundedMiniBarGDIPlus(g, ramStartX, barY, ramEndX - ramStartX, ScaleDpi(3, dpi), ram, barCol);
                }
            }

            if (showGpu || showGpuTemp) {
                if (curItem++ > 0) DrawTextSeg(g, xPos, yPos, L"  ", colLabel, g_pFontLabel);
                int gpuStartX = xPos;
                DrawTextSeg(g, xPos, yPos, L"GPU ", colLabel, g_pFontLabel);
                if (showGpu) {
                    DrawTextSeg(g, xPos, yPos, gpuVal, gpuCol, g_pFontValue);
                }
                int gpuEndX = xPos;
                if (showGpu && showGpuTemp) DrawTextSeg(g, xPos, yPos, L" ", colLabel, g_pFontLabel);
                if (showGpuTemp) {
                    DrawTextSeg(g, xPos, yPos, gpuTempVal, gpuTempCol, g_pFontValue);
                }

                if (showBars && showGpu) {
                    Gdiplus::Color barCol = s.customMetricsEnabled ? gpuCol :
                                           ((gpu > 85) ? colAlert : ((gpu > 60) ? colWarn : Gdiplus::Color(255, 110, 230, 145)));
                    DrawRoundedMiniBarGDIPlus(g, gpuStartX, barY, gpuEndX - gpuStartX, ScaleDpi(3, dpi), gpu, barCol);
                }
            }

            if (showDisk) {
                if (curItem++ > 0) DrawTextSeg(g, xPos, yPos, L"  ", colLabel, g_pFontLabel);
                DrawTextSeg(g, xPos, yPos, diskVal, diskCol, g_pFontValue);
            }

            if (showNet) {
                if (curItem++ > 0) DrawTextSeg(g, xPos, yPos, L"  ", colLabel, g_pFontLabel);
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
    // Keep widget above the taskbar in the topmost band
    SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    if (!IsWindowVisible(hWnd)) {
        ShowWindow(hWnd, SW_SHOWNA);
    }

    SelectObject(memDC, oldBmp);
    DeleteObject(memBmp);
    DeleteDC(memDC);
    ReleaseDC(NULL, screenDC);
}

// WinEvent hook to follow taskbar slide smoothly
void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD, HWND hwnd, LONG idObject, LONG, DWORD, DWORD) {
    if (idObject != OBJID_WINDOW || !hwnd) return;
    HWND hTray = g_hookedTray;
    if (!hTray) return;

    if (hwnd == hTray || GetAncestor(hwnd, GA_ROOT) == hTray) {
        FastRepositionOnly();
    }
}

// GUI thread only. Hooks Explorer's process so animations from any Explorer thread/mod are caught
void UpdateTaskbarHooks() {
    HWND hTray = FindWindowW(L"Shell_TrayWnd", NULL);
    if (hTray == g_hookedTray) return;
    if (g_hHookLoc) { UnhookWinEvent(g_hHookLoc); g_hHookLoc = NULL; }
    if (g_hHookVis) { UnhookWinEvent(g_hHookVis); g_hHookVis = NULL; }
    g_hookedTray = hTray;
    if (!hTray) return;
    DWORD pid = 0;
    GetWindowThreadProcessId(hTray, &pid);
    if (pid != 0) {
        g_hHookLoc = SetWinEventHook(EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE,
                                     NULL, WinEventProc, pid, 0, WINEVENT_OUTOFCONTEXT);
        g_hHookVis = SetWinEventHook(EVENT_OBJECT_SHOW, EVENT_OBJECT_HIDE,
                                     NULL, WinEventProc, pid, 0, WINEVENT_OUTOFCONTEXT);
    }
}

bool IsDragModifierActive(DragModifier mod) {
    switch (mod) {
        case DragModifier::Alt:      return (GetKeyState(VK_MENU) & 0x8000) != 0;
        case DragModifier::Ctrl:     return (GetKeyState(VK_CONTROL) & 0x8000) != 0;
        case DragModifier::Shift:    return (GetKeyState(VK_SHIFT) & 0x8000) != 0;
        case DragModifier::None:     return true;
        case DragModifier::Disabled:
        default:                     return false;
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

                HudAction hoverAct;
                int hoverDelay;
                {
                    std::lock_guard<std::mutex> lock(g_settingsMutex);
                    hoverAct = settings.hoverAction;
                    hoverDelay = settings.hoverDelayMs;
                }
                if (hoverAct != HudAction::None) {
                    SetTimer(hWnd, TIMER_HOVER_ACTIVATE, (UINT)hoverDelay, NULL);
                }

                SyncWindowGeometry();
            }
            if (g_dragPending && !g_isDragging) {
                POINT pt;
                GetCursorPos(&pt);
                int cxDrag = (std::max)(4, GetSystemMetrics(SM_CXDRAG));
                int cyDrag = (std::max)(4, GetSystemMetrics(SM_CYDRAG));
                if (std::abs(pt.x - g_dragStartPt.x) >= cxDrag || std::abs(pt.y - g_dragStartPt.y) >= cyDrag) {
                    g_isDragging = true;
                    KillTimer(hWnd, TIMER_HOVER_ACTIVATE);
                }
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
            KillTimer(hWnd, TIMER_HOVER_ACTIVATE);
            if (g_graphOpenedByHover) {
                g_graphOpenedByHover = false;
                HWND hGraph = g_hGraphWnd.load();
                if (hGraph && IsWindowVisible(hGraph)) {
                    ShowWindow(hGraph, SW_HIDE);
                }
            }
            SyncWindowGeometry();
            return 0;
        }

        case WM_RBUTTONUP: {
            bool ctrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
            HudAction act;
            {
                std::lock_guard<std::mutex> lock(g_settingsMutex);
                act = ctrl ? settings.ctrlRightClickAction : settings.rightClickAction;
            }
            ExecuteHudAction(act);
            return 0;
        }

        case WM_LBUTTONDOWN: {
            DragModifier dragMod;
            HudAction clickAct;
            bool ctrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
            {
                std::lock_guard<std::mutex> lock(g_settingsMutex);
                dragMod = settings.dragModifier;
                clickAct = ctrl ? settings.ctrlLeftClickAction : settings.leftClickAction;
            }

            if (IsDragModifierActive(dragMod)) {
                g_dragPending = true;
                g_isDragging = false;
                g_pendingClickAction = clickAct;
                GetCursorPos(&g_dragStartPt);
                {
                    std::lock_guard<std::mutex> lock(g_settingsMutex);
                    g_dragStartOffset = settings.offsetX;
                }
                SetCapture(hWnd);
            } else {
                ExecuteHudAction(clickAct);
            }
            return 0;
        }

        case WM_LBUTTONUP: {
            if (g_dragPending) {
                bool wasDragging = g_isDragging;
                HudAction clickAct = g_pendingClickAction;
                g_dragPending = false;
                g_isDragging = false;
                g_pendingClickAction = HudAction::None;
                ReleaseCapture();

                if (wasDragging) {
                    int finalOffset;
                    {
                        std::lock_guard<std::mutex> lock(g_settingsMutex);
                        finalOffset = settings.offsetX;
                    }
                    Wh_SetIntValue(L"customOffsetX", finalOffset);
                    ShowTempStatus(L"Position saved", Gdiplus::Color(255, 130, 210, 255), 1200);
                } else {
                    ExecuteHudAction(clickAct);
                }
            }
            return 0;
        }

        case WM_CAPTURECHANGED: {
            g_dragPending = false;
            g_isDragging = false;
            g_pendingClickAction = HudAction::None;
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
                UpdateTaskbarHooks();

                if (g_statusMessageTime && (GetTickCount64() - g_statusMessageTime >= g_statusDuration)) {
                    g_statusMessageTime = 0;
                    SyncWindowGeometry();
                } else {
                    FastRepositionOnly();
                }
            } else if (wParam == TIMER_HOVER_ACTIVATE) {
                KillTimer(hWnd, TIMER_HOVER_ACTIVATE);
                if (g_isHovered && !g_isDragging) {
                    HudAction hoverAct;
                    {
                        std::lock_guard<std::mutex> lock(g_settingsMutex);
                        hoverAct = settings.hoverAction;
                    }
                    if (hoverAct == HudAction::ToggleGraph) {
                        HWND hGraph = g_hGraphWnd.load();
                        if (hGraph && !IsWindowVisible(hGraph)) {
                            g_graphOpenedByHover = true;
                            ShowGraphWindow(false);
                        }
                    } else {
                        ExecuteHudAction(hoverAct);
                    }
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
    HRESULT hrCom = CoInitializeEx(NULL, COINIT_MULTITHREADED);

    PdhMonitor pdh;
    NvApiMonitor nvapi;
    AdlMonitor adl;
    WmiThermalMonitor wmi;

    ULONGLONG lastTick = GetTickCount64();

    while (g_bRunning) {
        ULONGLONG now = GetTickCount64();
        ULONGLONG elapsed = now - lastTick;
        lastTick = now;

        int interval;
        bool needNet, needGpu, needGpuTemp, needCpuTemp, needDisk;
        {
            std::lock_guard<std::mutex> lock(g_settingsMutex);
            interval = settings.updateInterval;
            needNet = settings.showNetSpeed;
            needGpu = settings.showGpu;
            needGpuTemp = settings.showGpuTemp;
            needCpuTemp = settings.showCpuTemp;
            needDisk = (settings.diskSpeedMode != DiskSpeedMode::Off);
        }

        int cpu = GetCpuUsage();
        g_cpuUsage = cpu;

        int ram = 0;
        MEMORYSTATUSEX memInfo = { sizeof(memInfo) };
        if (GlobalMemoryStatusEx(&memInfo)) {
            ram = (int)memInfo.dwMemoryLoad;
            g_ramUsage = ram;
        }

        int cpuTemp = TEMP_UNAVAILABLE;
        int gpuTemp = TEMP_UNAVAILABLE;
        if (needGpuTemp) {
            gpuTemp = nvapi.GetGpuTempC();
            if (gpuTemp == TEMP_UNAVAILABLE) {
                gpuTemp = adl.GetGpuTempC();
            }
        }
        if (needCpuTemp || (needGpuTemp && gpuTemp == TEMP_UNAVAILABLE)) {
            wmi.QueryTemps(needCpuTemp, needGpuTemp && (gpuTemp == TEMP_UNAVAILABLE), cpuTemp, gpuTemp);
        }

        ULONGLONG diskReadBps = 0, diskWriteBps = 0;
        int gpuPct = 0;
        int pdhThermalTemp = TEMP_UNAVAILABLE;
        bool readPdhThermal = needCpuTemp && (cpuTemp == TEMP_UNAVAILABLE);
        pdh.Update(needDisk, needGpu, needCpuTemp, readPdhThermal, diskReadBps, diskWriteBps, gpuPct, pdhThermalTemp);
        if (needDisk) {
            g_diskReadBps = diskReadBps;
            g_diskWriteBps = diskWriteBps;
        }
        if (needGpu) {
            g_gpuUsage = gpuPct;
        }
        if (needCpuTemp) {
            g_cpuTempC = (cpuTemp != TEMP_UNAVAILABLE) ? cpuTemp : pdhThermalTemp;
        }
        if (needGpuTemp) {
            g_gpuTempC = gpuTemp;
        }

        {
            std::lock_guard<std::mutex> lock(g_historyMutex);
            g_cpuHistory.push_back(cpu);
            g_ramHistory.push_back(ram);
            g_gpuHistory.push_back(gpuPct);
            while (g_cpuHistory.size() > MAX_GRAPH_POINTS) g_cpuHistory.pop_front();
            while (g_ramHistory.size() > MAX_GRAPH_POINTS) g_ramHistory.pop_front();
            while (g_gpuHistory.size() > MAX_GRAPH_POINTS) g_gpuHistory.pop_front();
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

    wmi.Shutdown();
    adl.Shutdown();
    nvapi.Shutdown();
    pdh.Shutdown();

    if (SUCCEEDED(hrCom)) {
        CoUninitialize();
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
    UpdateCachedFonts(96, 12);

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
    UpdateTaskbarHooks();
    SetTimer(hWnd, TIMER_FAST_CHECK, 100, NULL);

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

    if (g_hHookLoc) { UnhookWinEvent(g_hHookLoc); g_hHookLoc = NULL; }
    if (g_hHookVis) { UnhookWinEvent(g_hHookVis); g_hHookVis = NULL; }
    g_hookedTray = NULL;

    KillTimer(hWnd, TIMER_FAST_CHECK);
    KillTimer(hWnd, TIMER_HOVER_ACTIVATE);
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

    // Join GUI thread first so no UI actions can spawn new clean threads
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
