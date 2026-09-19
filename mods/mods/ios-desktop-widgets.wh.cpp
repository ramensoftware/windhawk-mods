// ==WindhawkMod==
// @id              ios-desktop-widgets
// @name            iOS Desktop Widgets
// @description     GII | Ios desktop widgets for windows 11
// @version         1.2
// @author          VOID - v0idofff (dc)
// @github https://github.com/RoShip-Interactive
// @include         explorer.exe
// @compilerOptions -lcomdlg32 -lgdiplus -lgdi32 -luser32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# iOS Desktop Widgets
Brings clean, customizable, anti-aliased widgets with smooth 30 FPS animations and circular progress rings to your Windows desktop.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- position: bottom-right
  $name: Widget Position
  $description: Choose desktop corner location
  $options:
    - bottom-right: Bottom Right
    - top-right: Top Right
    - bottom-left: Bottom Left
    - top-left: Top Left
- theme: system
  $name: Theme
  $description: Choose visual style
  $options:
    - dark: Dark (White Accents)
    - light: Light (Dark Grey Accents)
    - system: Windows System Match
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <gdiplus.h>
#include <thread>
#include <string>
#include <algorithm>

using namespace Gdiplus;

// --- Global State & Settings ---
std::wstring g_positionSetting = L"bottom-right";
std::wstring g_themeSetting = L"system";

HWND g_WidgetWindow = NULL;
ULONG_PTR g_gdiplusToken;
bool g_isRunning = true;

// Animated Values (Lerp interpolation)
struct AnimState {
    float currentCpu = 0.0f;
    float targetCpu = 0.0f;
    
    float currentRam = 0.0f;
    float targetRam = 0.0f;
    
    float currentBat1 = 0.0f;
    float targetBat1 = 0.0f;
} g_anim;

// --- Helper Functions ---

float Lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

void AddRoundRect(GraphicsPath& path, const Rect& r, int radius) {
    int d = radius * 2;
    path.AddArc(r.X, r.Y, d, d, 180, 90);
    path.AddArc(r.X + r.Width - d, r.Y, d, d, 270, 90);
    path.AddArc(r.X + r.Width - d, r.Y + r.Height - d, d, d, 0, 90);
    path.AddArc(r.X, r.Y + r.Height - d, d, d, 90, 90);
    path.CloseFigure();
}

void DrawProgressRing(Graphics& graphics, const RectF& rect, float percentage, Color bgRingColor, Color fgRingColor, float strokeWidth) {
    Pen bgPen(bgRingColor, strokeWidth);
    graphics.DrawEllipse(&bgPen, rect);

    if (percentage > 0.0f) {
        Pen fgPen(fgRingColor, strokeWidth);
        fgPen.SetStartCap(LineCapRound);
        fgPen.SetEndCap(LineCapRound);
        float sweepAngle = (std::min)(100.0f, (std::max)(0.0f, percentage)) * 3.6f;
        graphics.DrawArc(&fgPen, rect, -90.0f, sweepAngle);
    }
}

float CalculateCPULoad() {
    static FILETIME prevIdleTime = { 0 }, prevKernelTime = { 0 }, prevUserTime = { 0 };
    FILETIME idleTime, kernelTime, userTime;
    if (!GetSystemTimes(&idleTime, &kernelTime, &userTime)) return 0.0f;

    auto ft2ull = [](const FILETIME& ft) -> ULONGLONG {
        return ((ULONGLONG)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
    };

    ULONGLONG idle = ft2ull(idleTime) - ft2ull(prevIdleTime);
    ULONGLONG kernel = ft2ull(kernelTime) - ft2ull(prevKernelTime);
    ULONGLONG user = ft2ull(userTime) - ft2ull(prevUserTime);

    prevIdleTime = idleTime;
    prevKernelTime = kernelTime;
    prevUserTime = userTime;

    ULONGLONG sys = kernel + user;
    if (sys == 0) return 0.0f;
    return (float)((sys - idle) * 100.0 / sys);
}

void RepositionWindow(HWND hwnd) {
    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);

    int width = 340;
    int height = 580;
    int margin = 30;

    int x = 0, y = 0;
    if (g_positionSetting == L"top-right") {
        x = screenW - width - margin;
        y = margin;
    } else if (g_positionSetting == L"bottom-left") {
        x = margin;
        y = screenH - height - margin - 50;
    } else if (g_positionSetting == L"top-left") {
        x = margin;
        y = margin;
    } else { // bottom-right
        x = screenW - width - margin;
        y = screenH - height - margin - 50;
    }

    SetWindowPos(hwnd, HWND_BOTTOM, x, y, width, height, SWP_NOACTIVATE);
}

// --- Core Rendering Engine ---
void RenderWidgets(HWND hwnd) {
    RECT rc;
    GetWindowRect(hwnd, &rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;

    static int frameCounter = 0;
    frameCounter++;
    if (frameCounter >= 30) {
        frameCounter = 0;
        g_anim.targetCpu = CalculateCPULoad();
        
        MEMORYSTATUSEX memex = { sizeof(MEMORYSTATUSEX) };
        GlobalMemoryStatusEx(&memex);
        g_anim.targetRam = (float)memex.dwMemoryLoad;

        SYSTEM_POWER_STATUS sps;
        if (GetSystemPowerStatus(&sps) && sps.BatteryLifePercent != 255) {
            g_anim.targetBat1 = (float)sps.BatteryLifePercent;
        } else {
            g_anim.targetBat1 = -1.0f;
        }
    }

    g_anim.currentCpu = Lerp(g_anim.currentCpu, g_anim.targetCpu, 0.15f);
    g_anim.currentRam = Lerp(g_anim.currentRam, g_anim.targetRam, 0.15f);
    if (g_anim.targetBat1 >= 0) {
        g_anim.currentBat1 = Lerp(g_anim.currentBat1, g_anim.targetBat1, 0.15f);
    }

    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);

    BITMAPINFOHEADER bmi = { 0 };
    bmi.biSize = sizeof(BITMAPINFOHEADER);
    bmi.biWidth = width;
    bmi.biHeight = -height;
    bmi.biPlanes = 1;
    bmi.biBitCount = 32;
    bmi.biCompression = BI_RGB;

    void* pBits = nullptr;
    HBITMAP hBitmap = CreateDIBSection(hdcScreen, (BITMAPINFO*)&bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);

    {
        Graphics graphics(hdcMem);
        graphics.SetSmoothingMode(SmoothingModeAntiAlias);
        graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);

        graphics.Clear(Color(0, 0, 0, 0));

        bool isDark = true;
        if (g_themeSetting == L"light") {
            isDark = false;
        } else if (g_themeSetting == L"dark") {
            isDark = true;
        } else { // system
            DWORD useLight = 0;
            DWORD size = sizeof(useLight);
            if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &useLight, &size) == ERROR_SUCCESS) {
                isDark = (useLight == 0);
            }
        }

        // Monochrome Color Palette
        Color bgColor = isDark ? Color(190, 28, 28, 30) : Color(210, 245, 245, 247);
        Color textColor = isDark ? Color(255, 255, 255, 255) : Color(255, 20, 20, 20);
        Color subTextColor = isDark ? Color(255, 200, 200, 200) : Color(255, 80, 80, 80);
        Color trackRingColor = isDark ? Color(255, 55, 55, 60) : Color(255, 215, 215, 220);
        Color accentRingColor = isDark ? Color(255, 255, 255, 255) : Color(255, 40, 40, 40);

        SolidBrush bgBrush(bgColor);
        SolidBrush textBrush(textColor);
        SolidBrush subTextBrush(subTextColor);
        
        FontFamily fontFamily(L"Segoe UI");
        Font titleFont(&fontFamily, 13, FontStyleBold, UnitPixel);
        Font dataFont(&fontFamily, 28, FontStyleBold, UnitPixel);
        Font subFont(&fontFamily, 14, FontStyleRegular, UnitPixel);
        Font ringFont(&fontFamily, 12, FontStyleBold, UnitPixel);

        int padding = 15;
        int widgetHeight = 120;
        int widgetWidth = width - (padding * 2);

        auto drawWidgetFrame = [&](int index) -> Rect {
            Rect r(padding, padding + (index * (widgetHeight + padding)), widgetWidth, widgetHeight);
            GraphicsPath path;
            AddRoundRect(path, r, 20);
            graphics.FillPath(&bgBrush, &path);
            return r;
        };

        // 1. Clock & Date Widget
        Rect r1 = drawWidgetFrame(0);
        SYSTEMTIME st; GetLocalTime(&st);
        wchar_t timeStr[64], dateStr[64];
        GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, TIME_NOSECONDS, &st, NULL, timeStr, 64);
        GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, DATE_LONGDATE, &st, NULL, dateStr, 64, NULL);
        graphics.DrawString(timeStr, -1, &dataFont, PointF((REAL)r1.X + 20, (REAL)r1.Y + 22), &textBrush);
        graphics.DrawString(dateStr, -1, &subFont, PointF((REAL)r1.X + 20, (REAL)r1.Y + 68), &subTextBrush);

        // 2. Battery Widget
        Rect r2 = drawWidgetFrame(1);
        graphics.DrawString(L"Battery", -1, &titleFont, PointF((REAL)r2.X + 20, (REAL)r2.Y + 18), &subTextBrush);
        
        if (g_anim.targetBat1 >= 0) {
            std::wstring batStr = std::to_wstring((int)(g_anim.currentBat1 + 0.5f)) + L"%";
            graphics.DrawString(batStr.c_str(), -1, &dataFont, PointF((REAL)r2.X + 20, (REAL)r2.Y + 52), &textBrush);
            
            RectF ringRect((REAL)r2.X + widgetWidth - 75, (REAL)r2.Y + 25, 55.0f, 55.0f);
            DrawProgressRing(graphics, ringRect, g_anim.currentBat1, trackRingColor, accentRingColor, 6.0f);
        } else {
            graphics.DrawString(L"Desktop PC", -1, &dataFont, PointF((REAL)r2.X + 20, (REAL)r2.Y + 52), &textBrush);
        }

        // 3. System Monitor Widget
        Rect r3 = drawWidgetFrame(2);
        graphics.DrawString(L"System Monitor", -1, &titleFont, PointF((REAL)r3.X + 20, (REAL)r3.Y + 16), &subTextBrush);

        // CPU Ring
        RectF cpuRingRect((REAL)r3.X + 25, (REAL)r3.Y + 46, 52.0f, 52.0f);
        DrawProgressRing(graphics, cpuRingRect, g_anim.currentCpu, trackRingColor, accentRingColor, 6.0f);
        std::wstring cpuValStr = std::to_wstring((int)(g_anim.currentCpu + 0.5f)) + L"%";
        graphics.DrawString(L"CPU", -1, &subFont, PointF((REAL)r3.X + 88, (REAL)r3.Y + 50), &textBrush);
        graphics.DrawString(cpuValStr.c_str(), -1, &ringFont, PointF((REAL)r3.X + 88, (REAL)r3.Y + 70), &subTextBrush);

        // RAM Ring
        RectF ramRingRect((REAL)r3.X + 165, (REAL)r3.Y + 46, 52.0f, 52.0f);
        DrawProgressRing(graphics, ramRingRect, g_anim.currentRam, trackRingColor, accentRingColor, 6.0f);
        std::wstring ramValStr = std::to_wstring((int)(g_anim.currentRam + 0.5f)) + L"%";
        graphics.DrawString(L"RAM", -1, &subFont, PointF((REAL)r3.X + 228, (REAL)r3.Y + 50), &textBrush);
        graphics.DrawString(ramValStr.c_str(), -1, &ringFont, PointF((REAL)r3.X + 228, (REAL)r3.Y + 70), &subTextBrush);

        // 4. Notifications Widget
        Rect r4 = drawWidgetFrame(3);
        graphics.DrawString(L"Notifications", -1, &titleFont, PointF((REAL)r4.X + 20, (REAL)r4.Y + 20), &subTextBrush);
        graphics.DrawString(L"All caught up", -1, &subFont, PointF((REAL)r4.X + 20, (REAL)r4.Y + 58), &textBrush);
    }

    POINT ptSrc = { 0, 0 };
    SIZE size = { width, height };
    BLENDFUNCTION blend = { 0 };
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;

    UpdateLayeredWindow(hwnd, hdcScreen, NULL, &size, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);

    SelectObject(hdcMem, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
}

// --- Window Procedure ---
LRESULT CALLBACK WidgetWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_TIMER:
        RenderWidgets(hwnd);
        return 0;
    case WM_NCHITTEST:
        return HTTRANSPARENT;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// --- Thread for Window Loop ---
void WidgetThread() {
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WidgetWindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"iOSWidgetLayeredWindow";
    
    RegisterClass(&wc);

    g_WidgetWindow = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT,
        wc.lpszClassName,
        L"iOSWidgets",
        WS_POPUP | WS_VISIBLE,
        0, 0, 340, 580,
        NULL, NULL, wc.hInstance, NULL
    );

    RepositionWindow(g_WidgetWindow);
    RenderWidgets(g_WidgetWindow);

    SetTimer(g_WidgetWindow, 1, 33, NULL);

    MSG msg;
    while (g_isRunning && GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    DestroyWindow(g_WidgetWindow);
    UnregisterClass(wc.lpszClassName, wc.hInstance);
}

// --- Windhawk Hooks ---

void LoadSettings() {
    PCWSTR pos = Wh_GetStringSetting(L"position");
    if (pos) {
        g_positionSetting = pos;
        Wh_FreeStringSetting(pos);
    } else {
        g_positionSetting = L"bottom-right";
    }

    PCWSTR thm = Wh_GetStringSetting(L"theme");
    if (thm) {
        g_themeSetting = thm;
        Wh_FreeStringSetting(thm);
    } else {
        g_themeSetting = L"system";
    }
}

BOOL Wh_ModInit() {
    Wh_Log(L"iOS Widgets Mod Init");
    LoadSettings();

    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, NULL);

    g_isRunning = true;
    std::thread(WidgetThread).detach();

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"iOS Widgets Mod Uninit");
    g_isRunning = false;
    
    if (g_WidgetWindow) {
        PostMessage(g_WidgetWindow, WM_QUIT, 0, 0);
    }
    
    Sleep(200); 
    GdiplusShutdown(g_gdiplusToken);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    if (g_WidgetWindow) {
        RepositionWindow(g_WidgetWindow);
        RenderWidgets(g_WidgetWindow);
    }
}
