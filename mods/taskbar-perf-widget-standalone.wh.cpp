// ==WindhawkMod==
// @id              taskbar-perf-widget-standalone
// @name            Taskbar Performance Widget
// @description     Native Acrylic, Absolute Vectors, Asymmetric Width, and Free Positioning (Drag & Drop).
// @version         16.0
// @author          Agarciao10
// @include         windhawk.exe
// @compilerOptions -lpdh -ldwmapi -lgdi32 -luser32 -lgdiplus -lshell32
// @github          https://github.com/AngelGarciaODiana
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Performance Widget

![Screenshot](https://i.imgur.com/rlqNbbg.png)

A high-precision performance monitoring widget, designed to integrate natively
and symmetrically into the Windows 11 taskbar.

## Key Features
- **Real-time monitoring:** Instant display of CPU, Memory (RAM), Network
(Down/Up), GPU, and NPU.
- **Flawless Acrylic:** Uses pure GDI+ rendering combined with DWM frame
extension to maintain the floating Acrylic/Mica illusion transparently.
- **Absolute Vector Graphics:** Custom mathematical rendering for hardware
components (RAM, GPU, and NPU) bypassing all font dependencies.
- **Free Positioning (Drag & Drop):** Unlock the widget to drag it freely to any
location on your screen.
- **Dual Anchoring:** When locked, choose between placing the widget on the left
or right side of the taskbar.
- **Dynamic Asymmetric Matrix:** Network metrics automatically calculate and
request wider coordinate spaces to prevent textual overlap.
- **Isolated Architecture:** Uses an independent process to guarantee the full
stability of the Explorer.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- freeMove: false
  $name: "Unlock Free Positioning"
  $description: "Enable to drag the widget anywhere on the screen with the
mouse. (Disables click-through while active)"
- alignLeft: false
  $name: "Align to Left"
  $description: "Places the widget on the left side of the taskbar instead of
the right (only applies when Free Positioning is disabled)."
- showCpu: true
  $name: "Show CPU"
- showRam: true
  $name: "Show Memory (RAM)"
- showNetRecv: true
  $name: "Show Network (Download)"
- showNetSend: true
  $name: "Show Network (Upload)"
- showGpu: true
  $name: "Show GPU"
- showNpu: true
  $name: "Show NPU"
- offsetRight: 150
  $name: "Horizontal Offset"
  $description: "Distance in pixels from the edge of the screen."
- height: 48
  $name: "Widget Height"
  $description: "Total height in pixels of the widget."
- colWidth: 84
  $name: "Base Width per Indicator"
  $description: "Horizontal space allocated to standard metrics. Network metrics
dynamically request 40% more space."
- bgOpacity: 64
  $name: "Acrylic Opacity (Tint)"
  $description: "Controls the background density. Lower is more transparent
(0-255)."
*/
// ==/WindhawkModSettings==

#include <dwmapi.h>
#include <gdiplus.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <windhawk_api.h>
#include <windows.h>
#include <cwchar>
#include <mutex>
#include <thread>

using namespace Gdiplus;

typedef enum _WINDOWCOMPOSITIONATTRIB {
    WCA_ACCENT_POLICY = 19
} WINDOWCOMPOSITIONATTRIB;
typedef enum _ACCENT_STATE {
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_BLURBEHIND = 3,
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,
    ACCENT_INVALID_STATE = 5
} ACCENT_STATE;

typedef struct _ACCENT_POLICY {
    ACCENT_STATE AccentState;
    DWORD AccentFlags;
    DWORD GradientColor;
    DWORD AnimationId;
} ACCENT_POLICY;

typedef struct _WINDOWCOMPOSITIONATTRIBDATA {
    WINDOWCOMPOSITIONATTRIB Attribute;
    PVOID Data;
    SIZE_T SizeOfData;
} WINDOWCOMPOSITIONATTRIBDATA;

typedef BOOL(
    WINAPI* pSetWindowCompositionAttribute)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);

#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif

struct ModSettings {
    bool freeMove = false;
    bool alignLeft = false;
    int colWidth = 84;
    int height = 48;
    int offsetRight = 150;
    bool showCpu = true;
    bool showRam = true;
    bool showNetRecv = true;
    bool showNetSend = true;
    bool showGpu = true;
    bool showNpu = true;
    int bgOpacity = 64;
} g_Settings;

std::mutex g_SettingsMutex;

HWND g_hWidgetWnd = NULL;
DWORD g_WidgetThreadId = 0;
HWINEVENTHOOK g_TaskbarHook = nullptr;
UINT g_TaskbarCreatedMsg = RegisterWindowMessage(L"TaskbarCreated");

PDH_HQUERY pdhQuery = NULL;
PDH_HCOUNTER pdhCpuTotal = NULL;
PDH_HCOUNTER pdhRamUsage = NULL;
PDH_HCOUNTER pdhNetRecv = NULL;
PDH_HCOUNTER pdhNetSend = NULL;
PDH_HCOUNTER pdhGpuUsage = NULL;
PDH_HCOUNTER pdhNpuUsage = NULL;

double currentCpu = 0.0;
double currentRam = 0.0;
double currentNetRecv = 0.0;
double currentNetSend = 0.0;
double currentGpu = 0.0;
double currentNpu = 0.0;

void UpdateAppearance(HWND hwnd) {
    if (!hwnd)
        return;

    DWM_WINDOW_CORNER_PREFERENCE preference = DWMWCP_ROUND;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &preference,
                          sizeof(preference));

    HMODULE hUser = GetModuleHandle(L"user32.dll");
    if (hUser) {
        auto SetComp = (pSetWindowCompositionAttribute)GetProcAddress(
            hUser, "SetWindowCompositionAttribute");
        if (SetComp) {
            ModSettings localSettings;
            {
                std::lock_guard<std::mutex> lock(g_SettingsMutex);
                localSettings = g_Settings;
            }

            DWORD alpha = localSettings.bgOpacity & 0xFF;
            DWORD tint = (alpha << 24) | 0x000000;

            ACCENT_POLICY policy = {ACCENT_ENABLE_ACRYLICBLURBEHIND, 2, tint,
                                    0};
            WINDOWCOMPOSITIONATTRIBDATA data = {WCA_ACCENT_POLICY, &policy,
                                                sizeof(ACCENT_POLICY)};
            SetComp(hwnd, &data);
        }
    }

    MARGINS margins = {-1, -1, -1, -1};
    DwmExtendFrameIntoClientArea(hwnd, &margins);
}

void LoadSettings() {
    std::lock_guard<std::mutex> lock(g_SettingsMutex);
    g_Settings.freeMove = Wh_GetIntSetting(L"freeMove") != 0;
    g_Settings.alignLeft = Wh_GetIntSetting(L"alignLeft") != 0;
    g_Settings.showCpu = Wh_GetIntSetting(L"showCpu") != 0;
    g_Settings.showRam = Wh_GetIntSetting(L"showRam") != 0;
    g_Settings.showNetRecv = Wh_GetIntSetting(L"showNetRecv") != 0;
    g_Settings.showNetSend = Wh_GetIntSetting(L"showNetSend") != 0;
    g_Settings.showGpu = Wh_GetIntSetting(L"showGpu") != 0;
    g_Settings.showNpu = Wh_GetIntSetting(L"showNpu") != 0;
    g_Settings.offsetRight = Wh_GetIntSetting(L"offsetRight");

    g_Settings.height = Wh_GetIntSetting(L"height");
    if (g_Settings.height < 20)
        g_Settings.height = 48;

    g_Settings.colWidth = Wh_GetIntSetting(L"colWidth");
    if (g_Settings.colWidth < 50)
        g_Settings.colWidth = 84;

    g_Settings.bgOpacity = Wh_GetIntSetting(L"bgOpacity");
    if (g_Settings.bgOpacity < 0)
        g_Settings.bgOpacity = 0;
    if (g_Settings.bgOpacity > 255)
        g_Settings.bgOpacity = 255;
}

int GetTotalWidgetWidth(const ModSettings& s) {
    float width = 0.0f;
    if (s.showCpu)
        width += s.colWidth;
    if (s.showRam)
        width += s.colWidth;
    if (s.showNetRecv)
        width += s.colWidth * 1.4f;
    if (s.showNetSend)
        width += s.colWidth * 1.4f;
    if (s.showGpu)
        width += s.colWidth;
    if (s.showNpu)
        width += s.colWidth;
    return (int)width;
}

double GetTotalCounterSum(PDH_HCOUNTER counter) {
    if (!counter)
        return 0.0;
    DWORD bufferSize = 0, itemCount = 0;
    PdhGetFormattedCounterArray(counter, PDH_FMT_DOUBLE, &bufferSize,
                                &itemCount, NULL);

    if (bufferSize > 0) {
        PDH_FMT_COUNTERVALUE_ITEM* items =
            (PDH_FMT_COUNTERVALUE_ITEM*)malloc(bufferSize);
        if (items) {
            if (PdhGetFormattedCounterArray(counter, PDH_FMT_DOUBLE,
                                            &bufferSize, &itemCount,
                                            items) == ERROR_SUCCESS) {
                double sum = 0.0;
                for (DWORD i = 0; i < itemCount; i++)
                    sum += items[i].FmtValue.doubleValue;
                free(items);
                return sum;
            }
            free(items);
        }
    }
    return 0.0;
}

void InitCounters() {
    if (PdhOpenQuery(NULL, NULL, &pdhQuery) != ERROR_SUCCESS)
        return;
    PdhAddEnglishCounter(pdhQuery, L"\\Processor(_Total)\\% Processor Time",
                         NULL, &pdhCpuTotal);
    PdhAddEnglishCounter(pdhQuery, L"\\Memory\\% Committed Bytes In Use", NULL,
                         &pdhRamUsage);
    PdhAddEnglishCounter(pdhQuery,
                         L"\\Network Interface(*)\\Bytes Received/sec", NULL,
                         &pdhNetRecv);
    PdhAddEnglishCounter(pdhQuery, L"\\Network Interface(*)\\Bytes Sent/sec",
                         NULL, &pdhNetSend);
    PdhAddEnglishCounter(pdhQuery, L"\\GPU Engine(*)\\Utilization Percentage",
                         NULL, &pdhGpuUsage);
    PdhAddEnglishCounter(pdhQuery, L"\\NPU Engine(*)\\Utilization Percentage",
                         NULL, &pdhNpuUsage);
    PdhCollectQueryData(pdhQuery);
}

void UpdateMetrics() {
    if (!pdhQuery)
        return;
    PDH_FMT_COUNTERVALUE counterVal;
    PdhCollectQueryData(pdhQuery);

    if (PdhGetFormattedCounterValue(pdhCpuTotal, PDH_FMT_DOUBLE, NULL,
                                    &counterVal) == ERROR_SUCCESS)
        currentCpu = counterVal.doubleValue;
    if (PdhGetFormattedCounterValue(pdhRamUsage, PDH_FMT_DOUBLE, NULL,
                                    &counterVal) == ERROR_SUCCESS)
        currentRam = counterVal.doubleValue;

    currentNetRecv = GetTotalCounterSum(pdhNetRecv);
    currentNetSend = GetTotalCounterSum(pdhNetSend);
    currentGpu = GetTotalCounterSum(pdhGpuUsage);
    currentNpu = GetTotalCounterSum(pdhNpuUsage);

    if (currentGpu > 100.0)
        currentGpu = 100.0;
    if (currentNpu > 100.0)
        currentNpu = 100.0;
}

void FormatNetSpeed(double bytesSec, wchar_t* buffer, size_t maxLen) {
    double kbps = (bytesSec * 8.0) / 1000.0;
    if (kbps >= 1000.0)
        swprintf(buffer, maxLen, L"%.1f Mbps", kbps / 1000.0);
    else
        swprintf(buffer, maxLen, L"%.1f Kbps", kbps);
}

void DrawPerformancePanel(HWND hwnd, HDC hdc, int width, int height) {
    ModSettings localSettings;
    {
        std::lock_guard<std::mutex> lock(g_SettingsMutex);
        localSettings = g_Settings;
    }

    UINT dpi = GetDpiForWindow(hwnd);
    float scale = dpi / 96.0f;

    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);
    graphics.ScaleTransform(scale, scale);

    graphics.Clear(Color(255, 0, 0, 0));

    FontFamily iconFamily(L"Segoe MDL2 Assets", nullptr);
    Font iconFont(&iconFamily, 15, FontStyleRegular, UnitPixel);

    FontFamily textFamily(L"Segoe UI", nullptr);
    Font valFont(&textFamily, 13, FontStyleBold, UnitPixel);
    Font lblFont(&textFamily, 10, FontStyleRegular, UnitPixel);

    Color primaryTextColor = Color(255, 255, 255, 255);
    Color secondaryTextColor = Color(200, 255, 255, 255);

    SolidBrush iconBrush(primaryTextColor);
    SolidBrush valBrush(primaryTextColor);
    SolidBrush lblBrush(secondaryTextColor);

    Pen iconPen(primaryTextColor, 1.5f);
    iconPen.SetLineCap(LineCapRound, LineCapRound, DashCapRound);
    iconPen.SetLineJoin(LineJoinRound);

    StringFormat formatLeft;
    formatLeft.SetAlignment(StringAlignmentNear);
    formatLeft.SetLineAlignment(StringAlignmentNear);

    float currentXOffset = 0.0f;
    float logicalHeight = height / scale;
    float centerY = logicalHeight / 2.0f;
    float iconY = centerY - 8.0f;
    float valY = centerY - 16.0f;
    float lblY = centerY;

    auto drawMetric = [&](const wchar_t* icon, const wchar_t* value,
                          const wchar_t* label, int customType,
                          float allocatedWidth) {
        float iconX = currentXOffset + 6.0f;

        PointF iconPos(iconX, iconY);
        PointF valPos(iconX + 24.0f, valY);
        PointF lblPos(iconX + 24.0f, lblY);

        if (customType == 0) {
            graphics.DrawString(icon, -1, &iconFont, iconPos, &formatLeft,
                                &iconBrush);
        } else if (customType == 1) {
            graphics.DrawRectangle(&iconPen, iconX + 3.0f, iconY + 5.0f, 14.0f,
                                   8.0f);
            graphics.DrawLine(&iconPen, iconX + 5.0f, iconY + 3.0f,
                              iconX + 5.0f, iconY + 5.0f);
            graphics.DrawLine(&iconPen, iconX + 8.0f, iconY + 3.0f,
                              iconX + 8.0f, iconY + 5.0f);
            graphics.DrawLine(&iconPen, iconX + 12.0f, iconY + 3.0f,
                              iconX + 12.0f, iconY + 5.0f);
            graphics.DrawLine(&iconPen, iconX + 15.0f, iconY + 3.0f,
                              iconX + 15.0f, iconY + 5.0f);
            graphics.DrawLine(&iconPen, iconX + 5.0f, iconY + 13.0f,
                              iconX + 5.0f, iconY + 15.0f);
            graphics.DrawLine(&iconPen, iconX + 8.0f, iconY + 13.0f,
                              iconX + 8.0f, iconY + 15.0f);
            graphics.DrawLine(&iconPen, iconX + 12.0f, iconY + 13.0f,
                              iconX + 12.0f, iconY + 15.0f);
            graphics.DrawLine(&iconPen, iconX + 15.0f, iconY + 13.0f,
                              iconX + 15.0f, iconY + 15.0f);
        } else if (customType == 2) {
            graphics.DrawRectangle(&iconPen, iconX + 2.0f, iconY + 3.0f, 16.0f,
                                   10.0f);
            graphics.DrawEllipse(&iconPen, iconX + 6.0f, iconY + 4.0f, 8.0f,
                                 8.0f);
            graphics.FillEllipse(&iconBrush, iconX + 9.0f, iconY + 7.0f, 2.0f,
                                 2.0f);
            graphics.FillRectangle(&iconBrush, iconX + 4.0f, iconY + 13.0f,
                                   5.0f, 2.0f);
            graphics.FillRectangle(&iconBrush, iconX + 11.0f, iconY + 13.0f,
                                   5.0f, 2.0f);
        } else if (customType == 3) {
            graphics.DrawLine(&iconPen, iconX + 3.0f, iconY + 9.0f,
                              iconX + 10.0f, iconY + 3.0f);
            graphics.DrawLine(&iconPen, iconX + 3.0f, iconY + 9.0f,
                              iconX + 10.0f, iconY + 15.0f);
            graphics.DrawLine(&iconPen, iconX + 10.0f, iconY + 3.0f,
                              iconX + 17.0f, iconY + 9.0f);
            graphics.DrawLine(&iconPen, iconX + 10.0f, iconY + 15.0f,
                              iconX + 17.0f, iconY + 9.0f);
            graphics.DrawLine(&iconPen, iconX + 10.0f, iconY + 3.0f,
                              iconX + 10.0f, iconY + 15.0f);

            graphics.FillEllipse(&iconBrush, iconX + 1.0f, iconY + 7.0f, 4.0f,
                                 4.0f);
            graphics.FillEllipse(&iconBrush, iconX + 8.0f, iconY + 1.0f, 4.0f,
                                 4.0f);
            graphics.FillEllipse(&iconBrush, iconX + 8.0f, iconY + 13.0f, 4.0f,
                                 4.0f);
            graphics.FillEllipse(&iconBrush, iconX + 15.0f, iconY + 7.0f, 4.0f,
                                 4.0f);
        }

        graphics.DrawString(value, -1, &valFont, valPos, &formatLeft,
                            &valBrush);
        graphics.DrawString(label, -1, &lblFont, lblPos, &formatLeft,
                            &lblBrush);
        currentXOffset += allocatedWidth;
    };

    if (localSettings.showCpu) {
        wchar_t cpuStr[16];
        swprintf(cpuStr, 16, L"%d%%", (int)currentCpu);
        drawMetric(L"\xE9D9", cpuStr, L"CPU", 0, (float)localSettings.colWidth);
    }
    if (localSettings.showRam) {
        wchar_t ramStr[16];
        swprintf(ramStr, 16, L"%d%%", (int)currentRam);
        drawMetric(L"", ramStr, L"Memory", 1, (float)localSettings.colWidth);
    }
    if (localSettings.showNetRecv) {
        wchar_t recvStr[32];
        FormatNetSpeed(currentNetRecv, recvStr, 32);
        drawMetric(L"\xEC27", recvStr, L"Down \x2193", 0,
                   localSettings.colWidth * 1.4f);
    }
    if (localSettings.showNetSend) {
        wchar_t sendStr[32];
        FormatNetSpeed(currentNetSend, sendStr, 32);
        drawMetric(L"\xEC27", sendStr, L"Up \x2191", 0,
                   localSettings.colWidth * 1.4f);
    }
    if (localSettings.showGpu) {
        wchar_t gpuStr[16];
        swprintf(gpuStr, 16, L"%d%%", (int)currentGpu);
        drawMetric(L"", gpuStr, L"GPU", 2, (float)localSettings.colWidth);
    }
    if (localSettings.showNpu) {
        wchar_t npuStr[16];
        swprintf(npuStr, 16, L"%d%%", (int)currentNpu);
        drawMetric(L"", npuStr, L"NPU", 3, (float)localSettings.colWidth);
    }
}

bool IsTaskbarWindow(HWND hwnd) {
    WCHAR cls[64];
    if (!hwnd)
        return false;
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    return wcscmp(cls, L"Shell_TrayWnd") == 0;
}

void CALLBACK
TaskbarEventProc(HWINEVENTHOOK, DWORD, HWND hwnd, LONG, LONG, DWORD, DWORD) {
    if (!IsTaskbarWindow(hwnd) || !g_hWidgetWnd)
        return;
    PostMessage(g_hWidgetWnd, WM_APP + 10, 0, 0);
}

void RegisterTaskbarHook(HWND hwnd) {
    HWND hTaskbar = FindWindow(L"Shell_TrayWnd", nullptr);
    if (hTaskbar) {
        DWORD pid = 0, tid = GetWindowThreadProcessId(hTaskbar, &pid);
        if (tid != 0)
            g_TaskbarHook = SetWinEventHook(
                EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE,
                nullptr, TaskbarEventProc, pid, tid,
                WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    }
    PostMessage(hwnd, WM_APP + 10, 0, 0);
}

#define IDT_POLL_HARDWARE 1001

LRESULT CALLBACK WidgetWndProc(HWND hwnd,
                               UINT msg,
                               WPARAM wParam,
                               LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            InitCounters();
            UpdateAppearance(hwnd);
            SetTimer(hwnd, IDT_POLL_HARDWARE, 1000, NULL);
            RegisterTaskbarHook(hwnd);
            return 0;
        case WM_ERASEBKGND:
            return 1;
        case WM_NCHITTEST: {
            ModSettings localSettings;
            {
                std::lock_guard<std::mutex> lock(g_SettingsMutex);
                localSettings = g_Settings;
            }
            // Retornar HTCAPTION habilita el Drag&Drop nativo de Win32.
            // Retornar HTTRANSPARENT lo hace click-through.
            return localSettings.freeMove ? HTCAPTION : HTTRANSPARENT;
        }
        case WM_NCLBUTTONDBLCLK:
            return 0;  // Previene que un doble clic maximice el widget de forma
                       // irregular
        case WM_SETTINGCHANGE:
            UpdateAppearance(hwnd);
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        case WM_DESTROY:
            if (g_TaskbarHook)
                UnhookWinEvent(g_TaskbarHook);
            if (pdhQuery)
                PdhCloseQuery(pdhQuery);
            PostQuitMessage(0);
            return 0;
        case WM_TIMER:
            if (wParam == IDT_POLL_HARDWARE) {
                UpdateMetrics();
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
        case WM_APP + 10: {
            HWND hTaskbar = FindWindow(TEXT("Shell_TrayWnd"), nullptr);
            if (!hTaskbar)
                break;

            ModSettings localSettings;
            {
                std::lock_guard<std::mutex> lock(g_SettingsMutex);
                localSettings = g_Settings;
            }

            int totalWidth = GetTotalWidgetWidth(localSettings);
            if (totalWidth == 0 || !IsWindowVisible(hTaskbar)) {
                if (IsWindowVisible(hwnd))
                    ShowWindow(hwnd, SW_HIDE);
                return 0;
            } else {
                if (!IsWindowVisible(hwnd))
                    ShowWindow(hwnd, SW_SHOWNOACTIVATE);
            }

            UINT dpi = GetDpiForWindow(hwnd);
            float scale = dpi / 96.0f;

            int scaledWidth = (int)(totalWidth * scale);
            int scaledHeight = (int)(localSettings.height * scale);
            int scaledOffset = (int)(localSettings.offsetRight * scale);

            RECT rc;
            GetWindowRect(hTaskbar, &rc);
            int taskbarHeight = rc.bottom - rc.top;

            int x, y;
            static bool hasSpawned = false;
            bool isInitialSpawn = !hasSpawned;
            hasSpawned = true;

            // Matriz dinámica de coordenadas
            if (localSettings.freeMove && !isInitialSpawn) {
                // Si el widget ya ha sido colocado y Free Move está activo,
                // mantener posición arrastrada
                RECT myRc;
                GetWindowRect(hwnd, &myRc);
                x = myRc.left;
                y = myRc.top;
            } else {
                // Anclaje matemático guiado
                if (localSettings.alignLeft) {
                    x = rc.left + scaledOffset;
                } else {
                    x = rc.right - scaledWidth - scaledOffset;
                }
                y = rc.top + (taskbarHeight / 2) - (scaledHeight / 2);
            }

            RECT myRc;
            GetWindowRect(hwnd, &myRc);
            if (myRc.left != x || myRc.top != y ||
                (myRc.right - myRc.left) != scaledWidth ||
                (myRc.bottom - myRc.top) != scaledHeight) {
                SetWindowPos(hwnd, HWND_TOPMOST, x, y, scaledWidth,
                             scaledHeight, SWP_NOACTIVATE);
            }
            return 0;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc;
            GetClientRect(hwnd, &rc);

            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP memBitmap =
                CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
            HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);

            DrawPerformancePanel(hwnd, memDC, rc.right, rc.bottom);

            BitBlt(hdc, 0, 0, rc.right, rc.bottom, memDC, 0, 0, SRCCOPY);

            SelectObject(memDC, oldBitmap);
            DeleteObject(memBitmap);
            DeleteDC(memDC);
            EndPaint(hwnd, &ps);
            return 0;
        }
        default:
            if (msg == g_TaskbarCreatedMsg) {
                if (g_TaskbarHook)
                    UnhookWinEvent(g_TaskbarHook);
                RegisterTaskbarHook(hwnd);
                return 0;
            }
            break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void WidgetThread() {
    g_WidgetThreadId = GetCurrentThreadId();
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WidgetWndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = TEXT("PsiNetStandalonePerfWidget");
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    RegisterClass(&wc);

    ModSettings localSettings;
    {
        std::lock_guard<std::mutex> lock(g_SettingsMutex);
        localSettings = g_Settings;
    }

    int initialWidth = GetTotalWidgetWidth(localSettings);

    g_hWidgetWnd = CreateWindowEx(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST, wc.lpszClassName,
        TEXT("PerformanceWidget"), WS_POPUP | WS_VISIBLE, 0, 0, initialWidth,
        localSettings.height, NULL, NULL, wc.hInstance, NULL);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    UnregisterClass(wc.lpszClassName, wc.hInstance);
    GdiplusShutdown(gdiplusToken);
}

std::thread* g_pWidgetThread = nullptr;

BOOL WhTool_ModInit() {
    LoadSettings();
    g_pWidgetThread = new std::thread(WidgetThread);
    return TRUE;
}

void WhTool_ModUninit() {
    if (g_WidgetThreadId != 0) {
        PostThreadMessage(g_WidgetThreadId, WM_QUIT, 0, 0);
    }
    if (g_pWidgetThread) {
        if (g_pWidgetThread->joinable())
            g_pWidgetThread->join();
        delete g_pWidgetThread;
        g_pWidgetThread = nullptr;
    }
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    if (g_hWidgetWnd) {
        UpdateAppearance(g_hWidgetWnd);
        SendMessage(g_hWidgetWnd, WM_APP + 10, 0, 0);
        InvalidateRect(g_hWidgetWnd, NULL, TRUE);
    }
}

// === Windhawk Canonical Tool-Mod Boilerplate ===
bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L"");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    bool isService = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;

    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (argv) {
        for (int i = 1; i < argc; i++) {
            if (wcscmp(argv[i], L"-service") == 0 ||
                wcscmp(argv[i], L"-service-start") == 0 ||
                wcscmp(argv[i], L"-service-stop") == 0) {
                isService = true;
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
    }

    if (isService) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex || GetLastError() == ERROR_ALREADY_EXISTS) {
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);
        void* entryPoint =
            (BYTE*)dosHeader + ntHeaders->OptionalHeader.AddressOfEntryPoint;
        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);

        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher)
        return;

    WCHAR currentProcessPath[MAX_PATH];
    if (GetModuleFileName(nullptr, currentProcessPath,
                          ARRAYSIZE(currentProcessPath)) == 0)
        return;

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule)
        kernelModule = GetModuleHandle(L"kernel32.dll");
    if (!kernelModule)
        return;

    using CreateProcessInternalW_t =
        BOOL(WINAPI*)(HANDLE, LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES,
                      LPSECURITY_ATTRIBUTES, WINBOOL, DWORD, LPVOID, LPCWSTR,
                      LPSTARTUPINFOW, LPPROCESS_INFORMATION, PHANDLE);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW)
        return;

    STARTUPINFO si = {0};
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_FORCEOFFFEEDBACK;
    PROCESS_INFORMATION pi;
    if (pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                nullptr, nullptr, &si, &pi, nullptr)) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}

void Wh_ModSettingsChanged() {
    if (!g_isToolModProcessLauncher) {
        WhTool_ModSettingsChanged();
    }
}

void Wh_ModUninit() {
    if (!g_isToolModProcessLauncher) {
        WhTool_ModUninit();
        ExitProcess(0);
    }
}