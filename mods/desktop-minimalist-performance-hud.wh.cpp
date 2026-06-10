// ==WindhawkMod==
// @id           desktop-minimalist-performance-hud
// @name         Minimalist System Performance HUD
// @description  A premium desktop hardware monitor overlay. Supports all local and Google Fonts with optimized ClearType rendering.
// @version      1.0.0
// @author       Random Author
// @github       https://github.com/agarwaladvit96-coder
// @homepage     https://github.com/agarwaladvit96-coder
// @include      explorer.exe
// @compilerOptions -lpdh -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Minimalist System Performance HUD
A sleek, elegant desktop overlay displaying real-time hardware metrics. 

### 🌐 Using Google Fonts
This mod is fully optimized to display any font from the Google Fonts library! To use your favorite web font:
1. Go to [fonts.google.com](https://fonts.google.com).
2. Download your favorite font family (e.g., *Inter*, *Roboto*, *Poppins*, or *Montserrat*).
3. Open the downloaded `.ttf` or `.otf` file and click **Install**.
4. Open this mod's settings, set the Preset to **Custom**, and type the exact name into the **Custom Font Name Override** box.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- FontPreset: "modern"
  $name: Font Style Preset
  $description: Choose a curated design style, or select "Custom" to use a downloaded Google Font.
  $options:
    - modern: "Windows 11 Fluent (Segoe UI)"
    - tech: "Terminal Hacker (Consolas)"
    - retro: "Classic Bit-Style (Courier New)"
    - custom: "Custom Google Font / System Font (Type below)"
- CustomFontName: "Inter"
  $name: Custom Font Name Override
  $description: Type the exact name of any installed Google Font or system font (e.g., Roboto, Poppins, Inter, Arial Black).
- ShowCpu: true
  $name: Show CPU
  $description: Toggle visibility of the CPU tracking meter.
- ShowRam: true
  $name: Show RAM
  $description: Toggle visibility of the RAM tracking meter.
- ShowGpu: true
  $name: Show GPU
  $description: Toggle visibility of the GPU tracking meter.
- ShowStorage: true
  $name: Show Storage
  $description: Toggle visibility of the storage capacity metric.
- ShowDisk: true
  $name: Show Disk Activity
  $description: Toggle visibility of the hard drive active response metric.
- ShowNetworkDown: true
  $name: Show Download Speed
  $description: Toggle visibility of network download traffic.
- ShowNetworkUp: true
  $name: Show Upload Speed
  $description: Toggle visibility of network upload traffic.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <string>
#include <pdh.h>
#include <pdhmsg.h>

// Global Window and Timer variables
HWND g_hWndOverlay = NULL;
UINT_PTR g_timerId = 0;

// PDH Variables for advanced queries
HQUERY   g_hQuery = NULL;
HCOUNTER g_hCpuCounter = NULL, g_hGpuCounter = NULL, g_hDiskCounter = NULL;
HCOUNTER g_hWifiRxCounter = NULL, g_hWifiTxCounter = NULL;

// Metric storage variables
int g_cpuUsage = 0, g_ramUsage = 0, g_gpuUsage = 0, g_diskActive = 0, g_storageUsedPct = 0;
double g_wifiRxMbps = 0.0, g_wifiTxMbps = 0.0;

// Layout Grid Constants
const int HUD_WIDTH = 310;
const int HUD_HEIGHT = 300; 
const int HUD_PADDING = 20;
const int BAR_HEIGHT = 6;
const int BAR_WIDTH = 130;

// Design Palette
#define COLOR_BG            RGB(26, 26, 30)     
#define COLOR_BORDER        RGB(55, 55, 62)     
#define COLOR_TEXT_MAIN     RGB(243, 243, 243)  
#define COLOR_TEXT_MUTED    RGB(155, 155, 165)  
#define COLOR_ACCENT_BLUE   RGB(60, 140, 250)   
#define COLOR_ACCENT_WARN   RGB(239, 68, 68)    
#define COLOR_TRACK_EMPTY   RGB(48, 48, 54)     

// System Data Gathering Pipelines
int FetchRamUsage() {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    if (GlobalMemoryStatusEx(&memInfo)) return memInfo.dwMemoryLoad;
    return 0;
}

int FetchStorageUsagePercentage() {
    ULARGE_INTEGER totalFreeBytes, totalBytes;
    if (GetDiskFreeSpaceExW(L"C:\\", &totalFreeBytes, &totalBytes, NULL)) {
        double used = (double)(totalBytes.QuadPart - totalFreeBytes.QuadPart);
        double total = (double)totalBytes.QuadPart;
        return (int)((used / total) * 100.0);
    }
    return 0;
}

double GetWildcardCounterSumDouble(HCOUNTER hCounter) {
    DWORD dwBufferSize = 0, dwItemCount = 0;
    PdhGetFormattedCounterArrayW(hCounter, PDH_FMT_DOUBLE, &dwBufferSize, &dwItemCount, NULL);
    if (dwBufferSize > 0) {
        BYTE* pBuffer = new BYTE[dwBufferSize];
        PPDH_FMT_COUNTERVALUE_ITEM_W pItems = (PPDH_FMT_COUNTERVALUE_ITEM_W)pBuffer;
        if (PdhGetFormattedCounterArrayW(hCounter, PDH_FMT_DOUBLE, &dwBufferSize, &dwItemCount, pItems) == ERROR_SUCCESS) {
            double total = 0.0;
            for (DWORD i = 0; i < dwItemCount; i++) {
                if (pItems[i].FmtValue.CStatus == PDH_CSTATUS_VALID_DATA) total += pItems[i].FmtValue.doubleValue;
            }
            delete[] pBuffer;
            return total;
        }
        delete[] pBuffer;
    }
    return 0.0;
}

HWND GetDesktopShellHandle() {
    HWND hwndShell = FindWindowEx(NULL, NULL, L"WorkerW", NULL);
    while (hwndShell) {
        if (FindWindowEx(hwndShell, NULL, L"SHELLDLL_DefView", NULL)) return hwndShell;
        hwndShell = FindWindowEx(NULL, hwndShell, L"WorkerW", NULL);
    }
    return FindWindow(L"Progman", L"Program Manager");
}

void InitializePdhCounters() {
    if (PdhOpenQuery(NULL, NULL, &g_hQuery) == ERROR_SUCCESS) {
        PdhAddEnglishCounter(g_hQuery, L"\\Processor(_Total)\\% Processor Time", 0, &g_hCpuCounter);
        PdhAddEnglishCounter(g_hQuery, L"\\GPU Engine(*)\\Utilization Percentage", 0, &g_hGpuCounter);
        PdhAddEnglishCounter(g_hQuery, L"\\PhysicalDisk(_Total)\\% Disk Time", 0, &g_hDiskCounter);
        PdhAddEnglishCounter(g_hQuery, L"\\Network Interface(*)\\Bytes Received/sec", 0, &g_hWifiRxCounter);
        PdhAddEnglishCounter(g_hQuery, L"\\Network Interface(*)\\Bytes Sent/sec", 0, &g_hWifiTxCounter);
        PdhCollectQueryData(g_hQuery);
    }
}

void UpdateMetrics() {
    if (!g_hQuery) return;
    PdhCollectQueryData(g_hQuery);
    PDH_FMT_COUNTERVALUE counterVal;

    if (PdhGetFormattedCounterValue(g_hCpuCounter, PDH_FMT_LONG, NULL, &counterVal) == ERROR_SUCCESS) g_cpuUsage = (int)counterVal.longValue;
    g_ramUsage = FetchRamUsage();
    g_storageUsedPct = FetchStorageUsagePercentage();
    g_gpuUsage = (int)GetWildcardCounterSumDouble(g_hGpuCounter);
    if (g_gpuUsage > 100) g_gpuUsage = 100;

    if (PdhGetFormattedCounterValue(g_hDiskCounter, PDH_FMT_LONG, NULL, &counterVal) == ERROR_SUCCESS) {
        g_diskActive = (int)counterVal.longValue;
        if (g_diskActive > 100) g_diskActive = 100;
    }

    g_wifiRxMbps = (GetWildcardCounterSumDouble(g_hWifiRxCounter) * 8.0) / (1024.0 * 1024.0);
    g_wifiTxMbps = (GetWildcardCounterSumDouble(g_hWifiTxCounter) * 8.0) / (1024.0 * 1024.0);
}

void DrawPremiumProgressLine(HDC hdc, int x, int y, int width, int height, int usagePct) {
    RECT trackRect = {x, y, x + width, y + height};
    HBRUSH hEmptyBrush = CreateSolidBrush(COLOR_TRACK_EMPTY);
    FillRect(hdc, &trackRect, hEmptyBrush);
    DeleteObject(hEmptyBrush);

    int fillWidth = (int)((double)width * (usagePct / 100.0));
    if (fillWidth > 0) {
        RECT fillRect = {x, y, x + fillWidth, y + height};
        COLORREF barColor = (usagePct > 90) ? COLOR_ACCENT_WARN : COLOR_ACCENT_BLUE;
        HBRUSH hFillBrush = CreateSolidBrush(barColor);
        FillRect(hdc, &fillRect, hFillBrush);
        DeleteObject(hFillBrush);
    }
}

// Adaptive Rendering Engine
LRESULT CALLBACK DashboardWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_TIMER: {
            InvalidateRect(hWnd, NULL, FALSE); 
            return 0;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            UpdateMetrics();

            // Fetch settings configuration
            bool showCpu       = Wh_GetIntSetting(L"ShowCpu") != 0;
            bool showRam       = Wh_GetIntSetting(L"ShowRam") != 0;
            bool showGpu       = Wh_GetIntSetting(L"ShowGpu") != 0;
            bool showStorage   = Wh_GetIntSetting(L"ShowStorage") != 0;
            bool showDisk      = Wh_GetIntSetting(L"ShowDisk") != 0;
            bool showNetDown   = Wh_GetIntSetting(L"ShowNetworkDown") != 0;
            bool showNetUp     = Wh_GetIntSetting(L"ShowNetworkUp") != 0;

            // Process Curated Presets vs Custom Google Fonts
            wchar_t targetFont[LF_FACESIZE] = L"Segoe UI";
            PCWSTR presetSetting = Wh_GetStringSetting(L"FontPreset");
            if (presetSetting) {
                if (wcscmp(presetSetting, L"modern") == 0) {
                    wcsncpy_s(targetFont, L"Segoe UI", _TRUNCATE);
                } else if (wcscmp(presetSetting, L"tech") == 0) {
                    wcsncpy_s(targetFont, L"Consolas", _TRUNCATE);
                } else if (wcscmp(presetSetting, L"retro") == 0) {
                    wcsncpy_s(targetFont, L"Courier New", _TRUNCATE);
                } else if (wcscmp(presetSetting, L"custom") == 0) {
                    PCWSTR customSetting = Wh_GetStringSetting(L"CustomFontName");
                    if (customSetting) {
                        wcsncpy_s(targetFont, customSetting, _TRUNCATE);
                        Wh_FreeStringSetting(customSetting);
                    }
                }
                Wh_FreeStringSetting(presetSetting);
            }

            RECT rect;
            GetClientRect(hWnd, &rect);
            HDC hdcMem = CreateCompatibleDC(hdc);
            HBITMAP hbmMem = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
            HBITMAP hOldBm = (HBITMAP)SelectObject(hdcMem, hbmMem);

            HBRUSH hBlackBrush = CreateSolidBrush(RGB(0, 0, 0));
            FillRect(hdcMem, &rect, hBlackBrush);
            DeleteObject(hBlackBrush);

            int coreCount = (showCpu ? 1 : 0) + (showRam ? 1 : 0) + (showGpu ? 1 : 0);
            int secondaryCount = (showStorage ? 1 : 0) + (showDisk ? 1 : 0) + (showNetDown ? 1 : 0) + (showNetUp ? 1 : 0);

            int dynamicCardHeight = HUD_PADDING + 35 + (coreCount * 28);
            if (coreCount > 0 && secondaryCount > 0) {
                dynamicCardHeight += 15; 
            }
            dynamicCardHeight += (secondaryCount * 24) + HUD_PADDING;

            RECT cardRect = {0, 0, HUD_WIDTH, dynamicCardHeight};

            HBRUSH hBgBrush = CreateSolidBrush(COLOR_BG);
            FillRect(hdcMem, &cardRect, hBgBrush);
            DeleteObject(hBgBrush);

            HBRUSH hBorderBrush = CreateSolidBrush(COLOR_BORDER); 
            FrameRect(hdcMem, &cardRect, hBorderBrush);
            DeleteObject(hBorderBrush);

            SetBkMode(hdcMem, TRANSPARENT);

            // GDI Object Generation utilizing CLEARTYPE_QUALITY for sharp rendering of Google Fonts
            HFONT hFontHeader = CreateFont(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH | FF_SWISS, targetFont);
            HFONT hFontLabel  = CreateFont(15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH | FF_SWISS, targetFont);
            HFONT hFontValue  = CreateFont(15, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH | FF_SWISS, targetFont);

            SelectObject(hdcMem, hFontHeader);
            SetTextColor(hdcMem, COLOR_TEXT_MAIN);
            RECT headerRect = {HUD_PADDING, HUD_PADDING, HUD_WIDTH - HUD_PADDING, HUD_PADDING + 20};
            DrawText(hdcMem, L"SYSTEM PERFORMANCE", -1, &headerRect, DT_LEFT | DT_TOP);

            int currentY = HUD_PADDING + 35;

            auto RenderCoreRow = [&](const wchar_t* label, int value) {
                SelectObject(hdcMem, hFontLabel);
                SetTextColor(hdcMem, COLOR_TEXT_MUTED);
                RECT lblRect = {HUD_PADDING, currentY, HUD_PADDING + 45, currentY + 20};
                DrawText(hdcMem, label, -1, &lblRect, DT_LEFT | DT_TOP);

                int barX = HUD_PADDING + 45 + 5;
                DrawPremiumProgressLine(hdcMem, barX, currentY + 6, BAR_WIDTH, BAR_HEIGHT, value);

                SelectObject(hdcMem, hFontValue);
                SetTextColor(hdcMem, COLOR_TEXT_MAIN);
                wchar_t valString[12]; swprintf_s(valString, 12, L"%d%%", value);
                RECT valRect = {barX + BAR_WIDTH + 12, currentY, HUD_WIDTH - HUD_PADDING, currentY + 20};
                DrawText(hdcMem, valString, -1, &valRect, DT_RIGHT | DT_TOP);

                currentY += 28;
            };

            if (showCpu) RenderCoreRow(L"CPU", g_cpuUsage);
            if (showRam) RenderCoreRow(L"RAM", g_ramUsage);
            if (showGpu) RenderCoreRow(L"GPU", g_gpuUsage);

            if (coreCount > 0 && secondaryCount > 0) {
                HPEN hPen = CreatePen(PS_SOLID, 1, RGB(52, 52, 58));
                HPEN hOldPen = (HPEN)SelectObject(hdcMem, hPen);
                MoveToEx(hdcMem, HUD_PADDING, currentY + 6, NULL);
                LineTo(hdcMem, HUD_WIDTH - HUD_PADDING, currentY + 6);
                SelectObject(hdcMem, hOldPen);
                DeleteObject(hPen);
                currentY += 15;
            }

            auto RenderSecondaryRow = [&](const wchar_t* label, const wchar_t* valueStr) {
                SelectObject(hdcMem, hFontLabel);
                SetTextColor(hdcMem, COLOR_TEXT_MUTED);
                RECT lblRect = {HUD_PADDING, currentY, HUD_PADDING + 120, currentY + 20};
                DrawText(hdcMem, label, -1, &lblRect, DT_LEFT | DT_TOP);

                SetTextColor(hdcMem, COLOR_TEXT_MAIN);
                RECT valRect = {HUD_PADDING + 120, currentY, HUD_WIDTH - HUD_PADDING, currentY + 20};
                DrawText(hdcMem, valueStr, -1, &valRect, DT_RIGHT | DT_TOP);

                currentY += 24;
            };

            wchar_t capBuf[16], actBuf[16], dlBuf[32], ulBuf[32];
            swprintf_s(capBuf, 16, L"%d%%", g_storageUsedPct);
            swprintf_s(actBuf, 16, L"%d%%", g_diskActive);
            swprintf_s(dlBuf, 32, L"%.1f Mbps", g_wifiRxMbps);
            swprintf_s(ulBuf, 32, L"%.1f Mbps", g_wifiTxMbps);

            if (showStorage) RenderSecondaryRow(L"Storage (C:)", capBuf);
            if (showDisk)    RenderSecondaryRow(L"Disk Activity", actBuf);
            if (showNetDown) RenderSecondaryRow(L"Network Down", dlBuf);
            if (showNetUp)   RenderSecondaryRow(L"Network Up", ulBuf);

            BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdcMem, 0, 0, SRCCOPY);

            DeleteObject(hFontHeader); DeleteObject(hFontLabel); DeleteObject(hFontValue);
            SelectObject(hdcMem, hOldBm);
            DeleteObject(hbmMem);
            DeleteDC(hdcMem);

            EndPaint(hWnd, &ps);
            return 0;
        }
        case WM_ERASEBKGND:
            return TRUE; 
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

DWORD WINAPI DashboardThreadProc(LPVOID lpParam) {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    WNDCLASS wc = {0}; wc.lpfnWndProc = DashboardWndProc; wc.hInstance = hInstance;
    wc.lpszClassName = L"WindhawkConfigurableMinimalistHUD";
    RegisterClass(&wc);

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int xPos = screenWidth - HUD_WIDTH - 30; 
    int yPos = 45;                             

    HWND hwndDesktopParent = GetDesktopShellHandle();

    g_hWndOverlay = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE, wc.lpszClassName, 
        L"Windhawk System Dashboard V11", WS_POPUP | WS_CHILD,
        xPos, yPos, HUD_WIDTH, HUD_HEIGHT, hwndDesktopParent, NULL, hInstance, NULL
    );

    if (!g_hWndOverlay) return 0;

    SetParent(g_hWndOverlay, hwndDesktopParent);
    SetWindowPos(g_hWndOverlay, HWND_BOTTOM, xPos, yPos, HUD_WIDTH, HUD_HEIGHT, SWP_ASYNCWINDOWPOS);

    SetLayeredWindowAttributes(g_hWndOverlay, RGB(0, 0, 0), 0, LWA_COLORKEY);
    ShowWindow(g_hWndOverlay, SW_SHOW);
    UpdateWindow(g_hWndOverlay);

    InitializePdhCounters();
    g_timerId = SetTimer(g_hWndOverlay, 1, 1000, NULL);

    MSG msg; while (GetMessage(&msg, NULL, 0, 0)) { TranslateMessage(&msg); DispatchMessage(&msg); }
    return 0;
}

BOOL Wh_ModInit() { HANDLE hThread = CreateThread(NULL, 0, DashboardThreadProc, NULL, 0, NULL); if (hThread) CloseHandle(hThread); return TRUE; }
void Wh_ModUninit() { if (g_timerId && g_hWndOverlay) KillTimer(g_hWndOverlay, g_timerId); if (g_hWndOverlay) DestroyWindow(g_hWndOverlay); if (g_hQuery) PdhCloseQuery(g_hQuery); }
