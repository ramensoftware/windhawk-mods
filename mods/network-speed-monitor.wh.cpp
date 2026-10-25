// ==WindhawkMod==
// @id              network-speed-monitor
// @name            Network Speed Monitor
// @description     Shows real-time download and upload speeds in the system tray
// @version         1.4
// @author          SHARIAR
// @github          https://github.com/shariarcode
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -liphlpapi -lgdi32 -lshell32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Network Speed Monitor
This mod places an icon in the Windows system tray displaying live download and upload speeds.
* **Top Line:** Download speed (green by default)
* **Bottom Line:** Upload speed (orange by default)

Hover over the icon to view the precise speeds in a tooltip. Right-click or adjust settings in Windhawk to customize colors, update intervals, and unit scales.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- updateIntervalMs: 1000
  $name: Update Interval (ms)
  $description: How often to update the network speed (min 200ms).
- downloadColor: "green"
  $name: Download Text Color
  $description: Color of the download speed text (top line).
  $options:
    - white: White
    - green: Green
    - cyan: Cyan
    - yellow: Yellow
    - orange: Orange
    - red: Red
    - pink: Pink
    - black: Black
- uploadColor: "orange"
  $name: Upload Text Color
  $description: Color of the upload speed text (bottom line).
  $options:
    - white: White
    - green: Green
    - cyan: Cyan
    - yellow: Yellow
    - orange: Orange
    - red: Red
    - pink: Pink
    - black: Black
- useBitsPerSecond: false
  $name: Use Bits per Second (bps)
  $description: Show speed in bits per second (e.g. Mbps/Kbps) instead of bytes per second (MB/s/KB/s).
- fontSize: 13
  $name: Font Size (Height)
  $description: Height of the numbers (recommended 10-15 for two lines, 20-28 for single line).
- fontWeight: "bold"
  $name: Font Weight (Thickness)
  $description: Boldness / weight of the numbers.
  $options:
    - normal: Normal
    - bold: Bold
    - extrabold: Extra Bold
    - heavy: Heavy
- singleLineLayout: false
  $name: Single Line (Download Only)
  $description: Shows only the download speed in a single large line (ignores upload).
*/
// ==/WindhawkModSettings==

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <iphlpapi.h>
#include <cwchar>
#include <cmath>

#ifndef IF_OPER_STATUS_OPERATIONAL
#define IF_OPER_STATUS_OPERATIONAL 5
#endif

namespace {

constexpr UINT WM_TRAYICON = WM_APP + 1;
constexpr UINT_PTR TIMER_ID = 1;

HWND g_hWnd = nullptr;
NOTIFYICONDATAW g_nid = {};
ULONG64 g_lastBytesIn = 0;
ULONG64 g_lastBytesOut = 0;
DWORD g_lastTime = 0;
bool g_firstSample = true;
HANDLE g_thread = nullptr;
volatile bool g_running = false;

UINT g_updateIntervalMs = 1000;
COLORREF g_downColor = RGB(80, 220, 80);
COLORREF g_upColor = RGB(255, 160, 60);
bool g_useBitsPerSecond = false;
int g_fontSize = 13;
int g_fontWeight = FW_BOLD;
bool g_singleLineLayout = false;
UINT g_msgTaskbarCreated = 0;

COLORREF ParseColorSetting(const wchar_t* settingName, COLORREF defaultColor) {
    COLORREF color = defaultColor;
    PCWSTR value = Wh_GetStringSetting(settingName);
    if (value) {
        if (wcscmp(value, L"white") == 0) color = RGB(255, 255, 255);
        else if (wcscmp(value, L"black") == 0) color = RGB(0, 0, 0);
        else if (wcscmp(value, L"red") == 0) color = RGB(255, 70, 70);
        else if (wcscmp(value, L"green") == 0) color = RGB(80, 220, 80);
        else if (wcscmp(value, L"yellow") == 0) color = RGB(255, 220, 60);
        else if (wcscmp(value, L"cyan") == 0) color = RGB(70, 220, 255);
        else if (wcscmp(value, L"orange") == 0) color = RGB(255, 160, 60);
        else if (wcscmp(value, L"pink") == 0) color = RGB(255, 105, 180);
        Wh_FreeStringSetting(value);
    }
    return color;
}

void LoadSettings() {
    g_updateIntervalMs = static_cast<UINT>(Wh_GetIntSetting(L"updateIntervalMs"));
    if (g_updateIntervalMs < 200) {
        g_updateIntervalMs = 1000;
    }
    g_downColor = ParseColorSetting(L"downloadColor", RGB(80, 220, 80));
    g_upColor = ParseColorSetting(L"uploadColor", RGB(255, 160, 60));
    g_useBitsPerSecond = Wh_GetIntSetting(L"useBitsPerSecond") != 0;
    
    g_fontSize = static_cast<int>(Wh_GetIntSetting(L"fontSize"));
    if (g_fontSize < 6 || g_fontSize > 32) {
        g_fontSize = 13;
    }

    g_singleLineLayout = Wh_GetIntSetting(L"singleLineLayout") != 0;

    PCWSTR weightVal = Wh_GetStringSetting(L"fontWeight");
    if (weightVal) {
        if (wcscmp(weightVal, L"normal") == 0) g_fontWeight = FW_NORMAL;
        else if (wcscmp(weightVal, L"bold") == 0) g_fontWeight = FW_BOLD;
        else if (wcscmp(weightVal, L"extrabold") == 0) g_fontWeight = FW_EXTRABOLD;
        else if (wcscmp(weightVal, L"heavy") == 0) g_fontWeight = FW_HEAVY;
        Wh_FreeStringSetting(weightVal);
    } else {
        g_fontWeight = FW_BOLD;
    }
}

// Measures bytes using backward-compatible Win32 API structures
void GetNetworkBytes(ULONG64* pBytesIn, ULONG64* pBytesOut) {
    *pBytesIn = 0;
    *pBytesOut = 0;

    DWORD ifIndex = 0;
    if (GetBestInterface(0x08080808, &ifIndex) == NO_ERROR) {
        MIB_IFROW row = {};
        row.dwIndex = ifIndex;
        if (GetIfEntry(&row) == NO_ERROR) {
            *pBytesIn = row.dwInOctets;
            *pBytesOut = row.dwOutOctets;
            return;
        }
    }

    // Fallback: Loopback/tunnel filtered sum
    ULONG size = 0;
    if (GetIfTable(nullptr, &size, FALSE) == ERROR_INSUFFICIENT_BUFFER) {
        MIB_IFTABLE* pIfTable = (MIB_IFTABLE*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
        if (pIfTable) {
            if (GetIfTable(pIfTable, &size, FALSE) == NO_ERROR) {
                for (DWORD i = 0; i < pIfTable->dwNumEntries; i++) {
                    const MIB_IFROW& row = pIfTable->table[i];
                    // 24 = loopback, 131 = tunnel
                    if (row.dwOperStatus == IF_OPER_STATUS_OPERATIONAL && 
                        row.dwType != 24 && 
                        row.dwType != 131) {
                        *pBytesIn += row.dwInOctets;
                        *pBytesOut += row.dwOutOctets;
                    }
                }
            }
            HeapFree(GetProcessHeap(), 0, pIfTable);
        }
    }
}

void FormatSpeed(double bytesPerSec, wchar_t* buf, size_t bufSize, bool useBits) {
    double val = bytesPerSec;
    if (useBits) {
        val *= 8.0;
    }

    if (val < 100.0 * 1024.0) {
        swprintf_s(buf, bufSize, L"%.0fK", val / 1024.0);
    } else if (val < 1024.0 * 1024.0) {
        swprintf_s(buf, bufSize, L"%.0fK", val / 1024.0);
    } else if (val < 10.0 * 1024.0 * 1024.0) {
        swprintf_s(buf, bufSize, L"%.1fM", val / (1024.0 * 1024.0));
    } else if (val < 100.0 * 1024.0 * 1024.0) {
        swprintf_s(buf, bufSize, L"%.1fM", val / (1024.0 * 1024.0));
    } else {
        swprintf_s(buf, bufSize, L"%.0fM", val / (1024.0 * 1024.0));
    }
}

HICON CreateSpeedIcon(double downBytes, double upBytes, COLORREF downColor, COLORREF upColor) {
    const int size = 32;

    HDC screenDc = GetDC(nullptr);
    HDC dc = CreateCompatibleDC(screenDc);
    if (!dc) {
        ReleaseDC(nullptr, screenDc);
        return nullptr;
    }

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = size;
    bmi.bmiHeader.biHeight = -size;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP colorBmp = CreateDIBSection(dc, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!colorBmp) {
        DeleteDC(dc);
        ReleaseDC(nullptr, screenDc);
        return nullptr;
    }
    HBITMAP oldBmp = (HBITMAP)SelectObject(dc, colorBmp);

    RECT rect = {0, 0, size, size};
    HBRUSH bg = (HBRUSH)GetStockObject(BLACK_BRUSH);
    FillRect(dc, &rect, bg);

    wchar_t downBuf[16];
    wchar_t upBuf[16];
    FormatSpeed(downBytes, downBuf, 16, g_useBitsPerSecond);
    FormatSpeed(upBytes, upBuf, 16, g_useBitsPerSecond);

    SetBkMode(dc, TRANSPARENT);

    HFONT font = CreateFontW(
        g_fontSize, 0, 0, 0, g_fontWeight, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
    HFONT oldFont = (HFONT)SelectObject(dc, font);

    if (g_singleLineLayout) {
        RECT middleRect = {0, 0, size, size};
        SetTextColor(dc, RGB(255, 255, 255));
        DrawTextW(dc, downBuf, -1, &middleRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    } else {
        RECT topRect = {0, 0, size, size / 2};
        SetTextColor(dc, RGB(255, 255, 255));
        DrawTextW(dc, downBuf, -1, &topRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        RECT bottomRect = {0, size / 2, size, size};
        DrawTextW(dc, upBuf, -1, &bottomRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }

    SelectObject(dc, oldFont);
    DeleteObject(font);

    BYTE dr = GetRValue(downColor);
    BYTE dg = GetGValue(downColor);
    BYTE db = GetBValue(downColor);

    BYTE ur = GetRValue(upColor);
    BYTE ug = GetGValue(upColor);
    BYTE ub = GetBValue(upColor);

    DWORD* pixels = static_cast<DWORD*>(bits);
    if (pixels) {
        for (int y = 0; y < size; y++) {
            bool isTop = (y < size / 2);
            BYTE targetR = (g_singleLineLayout || isTop) ? dr : ur;
            BYTE targetG = (g_singleLineLayout || isTop) ? dg : ug;
            BYTE targetB = (g_singleLineLayout || isTop) ? db : ub;

            for (int x = 0; x < size; x++) {
                int idx = y * size + x;
                BYTE coverage = GetRValue(pixels[idx]);
                BYTE a = coverage;
                BYTE r = (BYTE)((targetR * a) / 255);
                BYTE g = (BYTE)((targetG * a) / 255);
                BYTE b = (BYTE)((targetB * a) / 255);
                pixels[idx] = (static_cast<DWORD>(a) << 24) | (r << 16) | (g << 8) | b;
            }
        }
    }

    SelectObject(dc, oldBmp);

    BYTE maskBits[128] = {0};
    HBITMAP maskBmp = CreateBitmap(size, size, 1, 1, maskBits);
    if (!maskBmp) {
        DeleteObject(colorBmp);
        DeleteDC(dc);
        ReleaseDC(nullptr, screenDc);
        return nullptr;
    }

    ICONINFO iconInfo = {};
    iconInfo.fIcon = TRUE;
    iconInfo.hbmColor = colorBmp;
    iconInfo.hbmMask = maskBmp;

    HICON icon = CreateIconIndirect(&iconInfo);

    DeleteObject(colorBmp);
    DeleteObject(maskBmp);
    DeleteDC(dc);
    ReleaseDC(nullptr, screenDc);

    return icon;
}

void UpdateTrayIcon() {
    ULONG64 currentBytesIn = 0;
    ULONG64 currentBytesOut = 0;
    GetNetworkBytes(&currentBytesIn, &currentBytesOut);

    DWORD currentTime = GetTickCount();
    double timeDeltaSec = (currentTime - g_lastTime) / 1000.0;
    if (timeDeltaSec <= 0.0) timeDeltaSec = g_updateIntervalMs / 1000.0;

    double downSpeed = 0.0;
    double upSpeed = 0.0;

    if (!g_firstSample) {
        ULONG64 deltaIn = (currentBytesIn >= g_lastBytesIn) ? (currentBytesIn - g_lastBytesIn) : 0;
        ULONG64 deltaOut = (currentBytesOut >= g_lastBytesOut) ? (currentBytesOut - g_lastBytesOut) : 0;
        downSpeed = deltaIn / timeDeltaSec;
        upSpeed = deltaOut / timeDeltaSec;
    }

    g_firstSample = false;
    g_lastBytesIn = currentBytesIn;
    g_lastBytesOut = currentBytesOut;
    g_lastTime = currentTime;

    HICON newIcon = CreateSpeedIcon(downSpeed, upSpeed, g_downColor, g_upColor);
    if (!newIcon) {
        Wh_Log(L"CreateSpeedIcon failed!");
        return;
    }

    HICON oldIcon = g_nid.hIcon;
    g_nid.hIcon = newIcon;

    wchar_t tip[128];
    double downVal = downSpeed;
    double upVal = upSpeed;
    if (g_useBitsPerSecond) {
        downVal *= 8.0;
        upVal *= 8.0;
        
        wchar_t downStr[32];
        if (downVal < 1024.0) swprintf_s(downStr, L"%.0f bps", downVal);
        else if (downVal < 1024.0 * 1024.0) swprintf_s(downStr, L"%.1f Kbps", downVal / 1024.0);
        else swprintf_s(downStr, L"%.1f Mbps", downVal / (1024.0 * 1024.0));

        wchar_t upStr[32];
        if (upVal < 1024.0) swprintf_s(upStr, L"%.0f bps", upVal);
        else if (upVal < 1024.0 * 1024.0) swprintf_s(upStr, L"%.1f Kbps", upVal / 1024.0);
        else swprintf_s(upStr, L"%.1f Mbps", upVal / (1024.0 * 1024.0));

        swprintf_s(tip, L"Download: %s\nUpload: %s", downStr, upStr);
    } else {
        wchar_t downStr[32];
        if (downVal < 1024.0) swprintf_s(downStr, L"%.0f B/s", downVal);
        else if (downVal < 1024.0 * 1024.0) swprintf_s(downStr, L"%.1f KB/s", downVal / 1024.0);
        else swprintf_s(downStr, L"%.1f MB/s", downVal / (1024.0 * 1024.0));

        wchar_t upStr[32];
        if (upVal < 1024.0) swprintf_s(upStr, L"%.0f B/s", upVal);
        else if (upVal < 1024.0 * 1024.0) swprintf_s(upStr, L"%.1f KB/s", upVal / 1024.0);
        else swprintf_s(upStr, L"%.1f MB/s", upVal / (1024.0 * 1024.0));

        swprintf_s(tip, L"Download: %s\nUpload: %s", downStr, upStr);
    }
    wcscpy_s(g_nid.szTip, tip);

    BOOL res = Shell_NotifyIconW(NIM_MODIFY, &g_nid);
    if (!res) {
        Wh_Log(L"Shell_NotifyIconW NIM_MODIFY failed!");
    }
    if (oldIcon) DestroyIcon(oldIcon);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_TIMER && wParam == TIMER_ID) {
        UpdateTrayIcon();
        return 0;
    }
    if (msg == g_msgTaskbarCreated) {
        Wh_Log(L"TaskbarCreated message received, re-adding tray icon");
        Shell_NotifyIconW(NIM_ADD, &g_nid);
        return 0;
    }
    switch (msg) {
        case WM_CLOSE:
            DestroyWindow(hWnd);
            return 0;
        case WM_DESTROY:
            KillTimer(hWnd, TIMER_ID);
            Shell_NotifyIconW(NIM_DELETE, &g_nid);
            if (g_nid.hIcon) {
                DestroyIcon(g_nid.hIcon);
                g_nid.hIcon = nullptr;
            }
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

DWORD WINAPI ThreadProc(LPVOID) {
    Wh_Log(L"ThreadProc started");
    const wchar_t* className = L"WindhawkNetSpeedTrayWndClass";

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = className;
    
    if (!RegisterClassW(&wc)) {
        Wh_Log(L"RegisterClassW returned false (this might be fine if class is already registered)");
    }

    // Created as a top-level invisible window to receive broadcast messages (like TaskbarCreated)
    g_hWnd = CreateWindowExW(0, className, L"NetSpeedTray", 0, 0, 0, 0, 0,
                              nullptr, nullptr, wc.hInstance, nullptr);
    if (!g_hWnd) {
        Wh_Log(L"CreateWindowExW failed! LastError: %d", GetLastError());
        return 1;
    }
    Wh_Log(L"CreateWindowExW succeeded, HWND: %p", g_hWnd);

    g_msgTaskbarCreated = RegisterWindowMessageW(L"TaskbarCreated");

    g_nid.cbSize = sizeof(NOTIFYICONDATAW);
    g_nid.hWnd = g_hWnd;
    g_nid.uID = 1;
    g_nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    g_nid.uCallbackMessage = WM_TRAYICON;
    g_nid.hIcon = CreateSpeedIcon(0.0, 0.0, g_downColor, g_upColor);
    wcscpy_s(g_nid.szTip, L"Speed: --");

    BOOL res = Shell_NotifyIconW(NIM_ADD, &g_nid);
    Wh_Log(L"Shell_NotifyIconW NIM_ADD returned: %d", res);

    GetNetworkBytes(&g_lastBytesIn, &g_lastBytesOut);
    g_lastTime = GetTickCount();
    g_firstSample = false;

    SetTimer(g_hWnd, TIMER_ID, g_updateIntervalMs, nullptr);
    Wh_Log(L"Timer initialized. Entering message loop...");

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    UnregisterClassW(className, wc.hInstance);
    Wh_Log(L"ThreadProc exiting");
    return 0;
}

}  // namespace

BOOL Wh_ModInit() {
    Wh_Log(L"Wh_ModInit called");

    // Avoid loading the mod thread in folder windows/subprocesses of explorer.exe
    PCWSTR cmdLine = GetCommandLineW();
    if (wcsstr(cmdLine, L"/factory") != nullptr) {
        Wh_Log(L"Running in explorer.exe subprocess, skipping init");
        return FALSE;
    }

    LoadSettings();

    g_running = true;
    g_thread = CreateThread(nullptr, 0, ThreadProc, nullptr, 0, nullptr);
    if (!g_thread) {
        Wh_Log(L"CreateThread failed!");
    } else {
        Wh_Log(L"CreateThread succeeded");
    }
    return g_thread != nullptr;
}

void Wh_ModUninit() {
    Wh_Log(L"Wh_ModUninit called");
    g_running = false;
    if (g_hWnd) {
        PostMessageW(g_hWnd, WM_CLOSE, 0, 0);
    }
    if (g_thread) {
        WaitForSingleObject(g_thread, 3000);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Wh_ModSettingsChanged called");
    LoadSettings();
    if (g_hWnd) {
        SetTimer(g_hWnd, TIMER_ID, g_updateIntervalMs, nullptr);
        UpdateTrayIcon();
    }
}
