// ==WindhawkMod==
// @id              topstatusbar
// @name            Top Status Bar
// @description     A smart, auto-hiding top bar for multi-monitor setups with system shortcuts and resource monitoring.
// @version         1.1.5
// @author          AlexanderOG
// @github          https://github.com/alexandr0g/topstatusbar
// @include         explorer.exe
// @compilerOptions -ldwmapi -luser32 -lgdi32 -lshell32 -lgdiplus
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- showClock: true
  $name: "Clock: Show Time"
- showDate: true
  $name: "Clock: Show Date"
- left_appMenu: true
  $name: "Left: Start Menu"
- left_activities: true
  $name: "Left: Task View"
- left_altTab: true
  $name: "Left: Alt-Tab"
- left_username: true
  $name: "Left: User Name"
- left_settings: true
  $name: "Left: Settings"
- left_explorer: true
  $name: "Left: File Explorer"
- left_browser: true
  $name: "Left: Web Browser"
- left_title: true
  $name: "Left: Window Title"
- right_power: true
  $name: "Right: Shutdown Dialog"
- right_notif: true
  $name: "Right: Notifications"
- right_battery: true
  $name: "Right: Battery"
- right_network: true
  $name: "Right: Network"
- right_bt: true
  $name: "Right: Bluetooth"
- right_volume: true
  $name: "Right: Volume"
- right_bright: true
  $name: "Right: Brightness"
- right_res: true
  $name: "Right: System Resources"
- right_usb: true
  $name: "Right: USB Devices"
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <shellapi.h>
#include <vector>
#include <algorithm>
#include <gdiplus.h>

using namespace Gdiplus;

#define BAR_HEIGHT 30
#define CONTENT_Y 3
#define CONTENT_H 24
#define HIDE_OFFSET -(BAR_HEIGHT + 20)
#define WM_CHECK_VISIBILITY (WM_USER + 100)
#define TIMER_MOUSE_CHECK 1
#define TIMER_ANIMATION 2
#define TIMER_CLOCK_TICK 3
#define MOUSE_TRIGGER_ZONE 2

struct SystemCache {
    bool darkMode;
    COLORREF backgroundColor;
    WCHAR userName[128];
    FILETIME preIdle, preKernel, preUser;
    int cpuUsage = 0;

    void Update() {
        DWORD v = 0, s = sizeof(v);
        RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", L"AppsUseLightTheme", RRF_RT_REG_DWORD, NULL, &v, &s);
        darkMode = (v == 0);
        backgroundColor = darkMode ? RGB(32, 32, 32) : RGB(240, 240, 240);
        DWORD unSize = 128; GetUserNameW(userName, &unSize);
        UpdateCPU();
    }

    void UpdateCPU() {
        FILETIME idle, kernel, user;
        if (GetSystemTimes(&idle, &kernel, &user)) {
            ULONGLONG i = (((ULONGLONG)idle.dwHighDateTime << 32) | idle.dwLowDateTime) - (((ULONGLONG)preIdle.dwHighDateTime << 32) | preIdle.dwLowDateTime);
            ULONGLONG k = (((ULONGLONG)kernel.dwHighDateTime << 32) | kernel.dwLowDateTime) - (((ULONGLONG)preKernel.dwHighDateTime << 32) | preKernel.dwLowDateTime);
            ULONGLONG u = (((ULONGLONG)user.dwHighDateTime << 32) | user.dwLowDateTime) - (((ULONGLONG)preUser.dwHighDateTime << 32) | preUser.dwLowDateTime);
            if (k + u > 0) cpuUsage = (int)((float)(k + u - i) * 100.0f / (float)(k + u));
            preIdle = idle; preKernel = kernel; preUser = user;
        }
    }
} g_cache;

struct BarModules {
    bool showClock, showDate;
    bool l_menu, l_act, l_tab, l_user, l_set, l_exp, l_view, l_title;
    bool r_power, r_notif, r_bat, r_net, r_bt, r_bright, r_vol, r_res, r_usb;
} g_modules;

struct TopBarInstance {
    HWND hwndTrigger, hwndBar;
    HMONITOR hMonitor;
    RECT monitorRect;
    int currentY, targetY;
    bool isAnimating, isLocked;
};

HINSTANCE g_hInstance = NULL;
UINT g_shellHookMsg = 0;
ULONG_PTR g_gdiplusToken;
std::vector<TopBarInstance> g_bars;

// Función de utilidad para detección de colisión
bool IsInRect(Rect r, POINT pt) {
    return (pt.x >= r.X && pt.x < r.X + r.Width && pt.y >= r.Y && pt.y < r.Y + r.Height);
}

void SendKeyStroke(std::vector<WORD> keys) {
    std::vector<INPUT> v;
    for (WORD k : keys) { INPUT i = { INPUT_KEYBOARD }; i.ki.wVk = k; v.push_back(i); }
    for (auto it = keys.rbegin(); it != keys.rend(); ++it) { INPUT i = { INPUT_KEYBOARD }; i.ki.wVk = *it; i.ki.dwFlags = KEYEVENTF_KEYUP; v.push_back(i); }
    SendInput((UINT)v.size(), v.data(), sizeof(INPUT));
}

Bitmap* HIconToBitmap(HICON hIcon) {
    if (!hIcon) return NULL;
    Bitmap* bmp = new Bitmap(24, 24, PixelFormat32bppARGB);
    Graphics* g = Graphics::FromImage(bmp);
    g->Clear(Color(0, 0, 0, 0));
    HDC hdc = g->GetHDC();
    DrawIconEx(hdc, 0, 0, hIcon, 24, 24, 0, NULL, DI_NORMAL);
    g->ReleaseHDC(hdc);
    delete g;
    return bmp;
}

void DrawButtonFrame(Graphics& g, Rect r, bool hover, bool press, bool dark, bool rL = true, bool rR = true) {
    int alpha = hover ? (press ? 25 : 45) : 0;
    if (alpha <= 0) return;
    Color c = dark ? Color(alpha, 255, 255, 255) : Color(alpha, 0, 0, 0);
    SolidBrush br(c);
    GraphicsPath path;
    REAL rad = 8.0f;
    RectF rf((REAL)r.X, (REAL)r.Y, (REAL)r.Width, (REAL)r.Height);
    if (rL) path.AddArc(rf.X, rf.Y, rad, rad, 180, 90);
    else { path.AddLine(rf.X, rf.Y + rf.Height, rf.X, rf.Y); path.AddLine(rf.X, rf.Y, rf.X + rad, rf.Y); }
    if (rR) path.AddArc(rf.X + rf.Width - rad, rf.Y, rad, rad, 270, 90);
    else { path.AddLine(rf.X + rf.Width - rad, rf.Y, rf.X + rf.Width, rf.Y); path.AddLine(rf.X + rf.Width, rf.Y, rf.X + rf.Width, rf.Y + rad); }
    if (rR) path.AddArc(rf.X + rf.Width - rad, rf.Y + rf.Height - rad, rad, rad, 0, 90);
    else { path.AddLine(rf.X + rf.Width, rf.Y + rf.Height - rad, rf.X + rf.Width, rf.Y + rf.Height); path.AddLine(rf.X + rf.Width, rf.Y + rf.Height, rf.X + rf.Width - rad, rf.Y + rf.Height); }
    if (rL) path.AddArc(rf.X, rf.Y + rf.Height - rad, rad, rad, 90, 90);
    else { path.AddLine(rf.X + rad, rf.Y + rf.Height, rf.X, rf.Y + rf.Height); path.AddLine(rf.X, rf.Y + rf.Height, rf.X, rf.Y + rf.Height - rad); }
    path.CloseFigure();
    g.FillPath(&br, &path);
}

struct ModuleManager {
    int hoveredID = -1, pressedID = -1, lastMinute = -1;
    struct ModuleRect { int id; Rect rect; };
    std::vector<ModuleRect> activeRects;

    void Render(Graphics& g, int barW, bool dark) {
        activeRects.clear();
        g.SetSmoothingMode(SmoothingModeAntiAlias);
        g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
        Gdiplus::Font iconFont(L"Segoe MDL2 Assets", 10, FontStyleRegular, UnitPoint);
        Gdiplus::Font textFont(L"Segoe UI", 9, FontStyleRegular, UnitPoint);
        Gdiplus::Font smallFont(L"Segoe UI", 7.5f, FontStyleRegular, UnitPoint);
        Gdiplus::Font boldFont(L"Segoe UI", 9, FontStyleBold, UnitPoint);
        SolidBrush textBr(dark ? Color(230, 230, 230) : Color(25, 25, 25));
        StringFormat sf; sf.SetAlignment(StringAlignmentCenter); sf.SetLineAlignment(StringAlignmentCenter);
        StringFormat sfNear; sfNear.SetAlignment(StringAlignmentNear); sfNear.SetLineAlignment(StringAlignmentCenter);
        sfNear.SetTrimming(StringTrimmingEllipsisCharacter); sfNear.SetFormatFlags(StringFormatFlagsNoWrap);

        int nextX = 4;
        auto AddLeft = [&](bool active, int id, int width, const wchar_t* label, bool isIcon = true) {
            if (!active) return;
            Rect r(nextX, CONTENT_Y, width, CONTENT_H);
            DrawButtonFrame(g, r, hoveredID == id, pressedID == id, dark);
            if (id == 10) { 
                g.SetSmoothingMode(SmoothingModeNone);
                REAL s = 6.0f, gp = 1.0f, xB = (REAL)r.X + (r.Width - 13.0f)/2.0f, yB = (REAL)r.Y + (r.Height - 13.0f)/2.0f;
                g.FillRectangle(&textBr, xB, yB, s, s); g.FillRectangle(&textBr, xB+s+gp, yB, s, s);
                g.FillRectangle(&textBr, xB, yB+s+gp, s, s); g.FillRectangle(&textBr, xB+s+gp, yB+s+gp, s, s);
                g.SetSmoothingMode(SmoothingModeAntiAlias);
            } else if (id == 11) {
                g.SetSmoothingMode(SmoothingModeNone);
                REAL xB = (REAL)r.X + (r.Width - 13.0f)/2.0f, yB = (REAL)r.Y + (r.Height - 13.0f)/2.0f;
                g.FillRectangle(&textBr, xB, yB, 6.0f, 6.0f); g.FillRectangle(&textBr, xB, yB+7.0f, 6.0f, 6.0f);
                g.FillRectangle(&textBr, xB+7.0f, yB, 6.0f, 13.0f);
                g.SetSmoothingMode(SmoothingModeAntiAlias);
            } else {
                RectF rf((REAL)r.X, (REAL)r.Y, (REAL)r.Width, (REAL)r.Height);
                if (isIcon) g.DrawString(label, -1, &iconFont, rf, &sf, &textBr);
                else g.DrawString(label, -1, &textFont, rf, &sf, &textBr);
            }
            activeRects.push_back({id, r}); nextX += width + 2;
        };

        AddLeft(g_modules.l_menu, 10, 38, L"");
        AddLeft(g_modules.l_act, 11, 38, L""); 
        AddLeft(g_modules.l_tab, 12, 38, L"\uE7C4"); 
        AddLeft(g_modules.l_user, 13, 110, g_cache.userName, false);
        AddLeft(g_modules.l_set, 14, 38, L"\uE713");
        AddLeft(g_modules.l_exp, 15, 38, L"\uE838"); 
        AddLeft(g_modules.l_view, 16, 38, L"\uE774");

        if (g_modules.l_title) {
            HWND fg = GetForegroundWindow(); WCHAR szT[256]; GetWindowTextW(fg, szT, 256);
            Rect r(nextX, CONTENT_Y, 160, CONTENT_H); DrawButtonFrame(g, r, hoveredID == 17, pressedID == 17, dark);
            HICON hI = (HICON)SendMessage(fg, WM_GETICON, ICON_SMALL2, 0); if(!hI) hI = (HICON)GetClassLongPtr(fg, GCLP_HICONSM);
            if(hI) { Bitmap* bmp = HIconToBitmap(hI); if (bmp) { g.SetInterpolationMode(InterpolationModeHighQualityBicubic); g.DrawImage(bmp, Rect(r.X + 6, r.Y + 4, 16, 16)); delete bmp; } }
            g.DrawString(szT, -1, &textFont, RectF((REAL)r.X+26, (REAL)r.Y, (REAL)r.Width-30, (REAL)r.Height), &sfNear, &textBr);
            activeRects.push_back({17, r}); nextX += 162;
        }

        int prevX = barW - 4;
        auto AddRight = [&](bool active, int id, int width, const wchar_t* label, bool isIcon = true) {
            if (!active) return;
            prevX -= width; Rect r(prevX, CONTENT_Y, width, CONTENT_H);
            DrawButtonFrame(g, r, hoveredID == id, pressedID == id, dark);
            RectF rf((REAL)r.X, (REAL)r.Y, (REAL)r.Width, (REAL)r.Height);
            if (id == 107) {
                SYSTEM_POWER_STATUS sps; GetSystemPowerStatus(&sps); int pct = std::min((int)sps.BatteryLifePercent, 100); 
                WCHAR btS[8]; wsprintfW(btS, L"%d%%", pct);
                float bW = 20, tW = 26, grp = bW + 4 + tW, sX = (float)r.X + (r.Width - grp)/2.0f;
                Pen p(&textBr, 1.2f); g.DrawRectangle(&p, (REAL)sX, (REAL)r.Y + 7, (REAL)bW, (REAL)10); g.FillRectangle(&textBr, (REAL)(sX+bW), (REAL)r.Y + 9, (REAL)2, (REAL)6);
                float fillW = (17.0f * (float)pct / 100.0f);
                if (pct > 0) { SolidBrush bF(dark ? Color(220, 255, 255, 255) : Color(220, 0, 0, 0)); g.FillRectangle(&bF, (REAL)(sX+1.5f), (REAL)r.Y + 8.5f, (REAL)fillW, (REAL)7); }
                if (sps.ACLineStatus == 1) {
                    PointF pts[] = { {(REAL)(sX+10), (REAL)r.Y+8}, {(REAL)(sX+7), (REAL)r.Y+13}, {(REAL)(sX+10), (REAL)r.Y+13}, {(REAL)(sX+9), (REAL)r.Y+18}, {(REAL)(sX+13), (REAL)r.Y+11}, {(REAL)(sX+10), (REAL)r.Y+11} };
                    Pen pO(dark ? Color(255,32,32,32) : Color(255,255,255,255), 1.5f); g.DrawPolygon(&pO, pts, 6);
                    SolidBrush yb(dark ? Color(255,255,255,255) : Color(255,30,30,30)); g.FillPolygon(&yb, pts, 6);
                }
                g.DrawString(btS, -1, &smallFont, RectF((REAL)(sX+bW+2), (REAL)r.Y, (REAL)(tW+10), (REAL)r.Height), &sfNear, &textBr);
            } else {
                if (isIcon) g.DrawString(label, -1, &iconFont, rf, &sf, &textBr);
                else g.DrawString(label, -1, &textFont, rf, &sf, &textBr);
            }
            activeRects.push_back({id, r}); prevX -= 2;
        };

        AddRight(g_modules.r_power, 108, 36, L"\uE7E8"); 
        AddRight(g_modules.r_notif, 100, 36, L"\uE7E7");
        AddRight(g_modules.r_bat, 107, 75, L"");
        AddRight(g_modules.r_net, 106, 36, L"\uE701");
        AddRight(g_modules.r_bt, 105, 36, L"\uE702");
        AddRight(g_modules.r_vol, 103, 36, L"\uE767");
        AddRight(g_modules.r_bright, 104, 36, L"\uE706");
        if (g_modules.r_res) {
            MEMORYSTATUSEX ms; ms.dwLength = sizeof(ms); GlobalMemoryStatusEx(&ms);
            ULARGE_INTEGER fA, tB, tF; GetDiskFreeSpaceExW(L"C:", &fA, &tB, &tF);
            int cpu = std::min(g_cache.cpuUsage, 99), ssd = (tB.QuadPart > 0) ? std::min((int)((tB.QuadPart - fA.QuadPart) * 100 / tB.QuadPart), 99) : 0;
            WCHAR rs[128]; wsprintfW(rs, L"Cpu %d%%   |   Ram %d%%   |   Ssd %d%%", cpu, (int)ms.dwMemoryLoad, ssd);
            AddRight(true, 102, 220, rs, false);
        }
        bool hU = false; for(int i=0;i<26;i++){ WCHAR d[]={ (WCHAR)(L'A'+i),L':',L'\\',L'\0' }; if(GetDriveTypeW(d)==DRIVE_REMOVABLE){ hU=true; break; } }
        if (g_modules.r_usb && hU) AddRight(true, 101, 36, L"\uE88E");

        if (g_modules.showClock || g_modules.showDate) {
            WCHAR tT[32], tD[64]; GetDateFormatW(LOCALE_USER_DEFAULT, 0, NULL, L"dd MMM yyyy", tD, 64); GetTimeFormatW(LOCALE_USER_DEFAULT, TIME_NOSECONDS, NULL, NULL, tT, 32);
            int mid = barW / 2; Rect rD(mid - 95, CONTENT_Y, 94, CONTENT_H), rT(mid + 1, CONTENT_Y, 70, CONTENT_H);
            bool cH = (hoveredID == 0 || hoveredID == 1);
            if (g_modules.showDate) { DrawButtonFrame(g, rD, cH, pressedID == 0, dark, true, false); g.DrawString(tD, -1, &boldFont, RectF((REAL)rD.X, (REAL)rD.Y, (REAL)rD.Width, (REAL)rD.Height), &sf, &textBr); activeRects.push_back({0, rD}); }
            if (g_modules.showClock) { DrawButtonFrame(g, rT, cH, pressedID == 1, dark, false, true); g.DrawString(tT, -1, &boldFont, RectF((REAL)rT.X, (REAL)rT.Y, (REAL)rT.Width, (REAL)rT.Height), &sf, &textBr); activeRects.push_back({1, rT}); }
        }
    }

    void HandleMouse(HWND hwnd, UINT msg, POINT pt) {
        int oldH = hoveredID; hoveredID = -1;
        for (auto& ar : activeRects) { if (IsInRect(ar.rect, pt)) { hoveredID = ar.id; break; } }
        if (msg == WM_LBUTTONDOWN) pressedID = hoveredID;
        if (msg == WM_LBUTTONUP) {
            if (pressedID != -1 && pressedID == hoveredID) {
                switch(pressedID) {
                    case 0: { HINSTANCE res = ShellExecuteW(NULL, L"open", L"outlookcal:", NULL, NULL, SW_SHOWNORMAL);
                              if ((INT_PTR)res <= 32) ShellExecuteW(NULL, L"open", L"https://calendar.google.com", NULL, NULL, SW_SHOWNORMAL); break; }
                    case 1: ShellExecuteW(NULL, L"open", L"ms-clock:", NULL, NULL, SW_SHOWNORMAL); break;
                    case 10: SendKeyStroke({VK_LWIN}); break;
                    case 11: SendKeyStroke({VK_LWIN, VK_TAB}); break;
                    case 12: SendKeyStroke({VK_CONTROL, VK_MENU, VK_TAB}); break;
                    case 13: ShellExecuteW(NULL, L"open", L"ms-settings:yourinfo", NULL, NULL, SW_SHOWNORMAL); break;
                    case 14: ShellExecuteW(NULL, L"open", L"ms-settings:home", NULL, NULL, SW_SHOWNORMAL); break;
                    case 15: ShellExecuteW(NULL, L"open", L"explorer.exe", NULL, NULL, SW_SHOWNORMAL); break;
                    case 16: ShellExecuteW(NULL, L"open", L"http://", NULL, NULL, SW_SHOWNORMAL); break;
                    case 17: PostMessage(GetForegroundWindow(), WM_SYSCOMMAND, SC_KEYMENU, 0); break;
                    case 100: SendKeyStroke({VK_LWIN, 'N'}); break;
                    case 102: ShellExecuteW(NULL, L"open", L"taskmgr.exe", NULL, NULL, SW_SHOWNORMAL); break;
                    case 103: ShellExecuteW(NULL, L"open", L"ms-settings:apps-volume", NULL, NULL, SW_SHOWNORMAL); break;
                    case 104: ShellExecuteW(NULL, L"open", L"ms-settings:display", NULL, NULL, SW_SHOWNORMAL); break;
                    case 105: ShellExecuteW(NULL, L"open", L"ms-settings:bluetooth", NULL, NULL, SW_SHOWNORMAL); break;
                    case 106: ShellExecuteW(NULL, L"open", L"ms-settings:network-wifi", NULL, NULL, SW_SHOWNORMAL); break;
                    case 107: ShellExecuteW(NULL, L"open", L"ms-settings:batterysaver", NULL, NULL, SW_SHOWNORMAL); break;
                    case 108: ShellExecuteW(NULL, L"open", L"powershell.exe", L"-Command \"(New-Object -ComObject Shell.Application).ShutdownWindows()\"", NULL, SW_HIDE); break;
                }
            }
            pressedID = -1;
        }
        if (oldH != hoveredID || msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP) InvalidateRect(hwnd, NULL, FALSE);
    }
} g_modManager;

TopBarInstance* GetBarInstance(HWND hwnd) { for (auto& bar : g_bars) if (bar.hwndTrigger == hwnd || bar.hwndBar == hwnd) return &bar; return nullptr; }

bool IsFullscreen(HWND hwnd, RECT mon) {
    if (!hwnd || !IsWindow(hwnd)) return false;
    WCHAR cls[64]; GetClassNameW(hwnd, cls, 64);
    if (wcscmp(cls, L"WorkerW") == 0 || wcscmp(cls, L"Progman") == 0) return false;
    if (GetWindowLong(hwnd, GWL_STYLE) & WS_CAPTION) return false;
    RECT rc; GetWindowRect(hwnd, &rc);
    return (rc.left <= mon.left && rc.top <= mon.top && rc.right >= mon.right && rc.bottom >= mon.bottom);
}

bool ShouldDodge(TopBarInstance* bar) {
    HWND fg = GetForegroundWindow();
    if (!fg || fg == bar->hwndBar || fg == bar->hwndTrigger) return false;
    if (MonitorFromWindow(fg, MONITOR_DEFAULTTONEAREST) != bar->hMonitor) return false;
    WCHAR cls[64]; GetClassNameW(fg, cls, 64);
    if (wcscmp(cls, L"WorkerW") == 0 || wcscmp(cls, L"Progman") == 0 || wcscmp(cls, L"Shell_TrayWnd") == 0) return false;
    WINDOWPLACEMENT wp = {sizeof(wp)};
    if (GetWindowPlacement(fg, &wp) && wp.showCmd == SW_SHOWMAXIMIZED) return true;
    RECT rc; GetWindowRect(fg, &rc); return (rc.top <= (bar->monitorRect.top + BAR_HEIGHT + 2));
}

void UpdateState(TopBarInstance* bar) {
    HWND fg = GetForegroundWindow();
    if (IsFullscreen(fg, bar->monitorRect)) {
        if (!bar->isLocked) { bar->isLocked = true; ShowWindow(bar->hwndTrigger, SW_HIDE); }
        bar->targetY = HIDE_OFFSET;
    } else {
        if (bar->isLocked) { bar->isLocked = false; ShowWindow(bar->hwndTrigger, SW_SHOWNOACTIVATE); }
        POINT pt; GetCursorPos(&pt); bool mouseIn = (pt.x >= bar->monitorRect.left && pt.x <= bar->monitorRect.right);
        if (bar->currentY <= HIDE_OFFSET + 5) mouseIn &= (pt.y >= bar->monitorRect.top && pt.y <= bar->monitorRect.top + MOUSE_TRIGGER_ZONE);
        else mouseIn &= (pt.y >= bar->monitorRect.top && pt.y <= bar->monitorRect.top + BAR_HEIGHT);
        bar->targetY = (!ShouldDodge(bar) || mouseIn) ? 2 : HIDE_OFFSET;
    }
    if (bar->targetY != bar->currentY && !bar->isAnimating) { bar->isAnimating = true; SetTimer(bar->hwndBar, TIMER_ANIMATION, 10, NULL); }
}

void LoadSettings() {
    g_modules.showClock = (bool)Wh_GetIntSetting(L"showClock");
    g_modules.showDate = (bool)Wh_GetIntSetting(L"showDate");
    g_modules.l_menu = (bool)Wh_GetIntSetting(L"left_appMenu");
    g_modules.l_act = (bool)Wh_GetIntSetting(L"left_activities");
    g_modules.l_tab = (bool)Wh_GetIntSetting(L"left_altTab");
    g_modules.l_user = (bool)Wh_GetIntSetting(L"left_username");
    g_modules.l_set = (bool)Wh_GetIntSetting(L"left_settings");
    g_modules.l_exp = (bool)Wh_GetIntSetting(L"left_explorer");
    g_modules.l_view = (bool)Wh_GetIntSetting(L"left_browser");
    g_modules.l_title = (bool)Wh_GetIntSetting(L"left_title");
    g_modules.r_power = (bool)Wh_GetIntSetting(L"right_power");
    g_modules.r_notif = (bool)Wh_GetIntSetting(L"right_notif");
    g_modules.r_bat = (bool)Wh_GetIntSetting(L"right_battery");
    g_modules.r_net = (bool)Wh_GetIntSetting(L"right_network");
    g_modules.r_bt = (bool)Wh_GetIntSetting(L"right_bt");
    g_modules.r_bright = (bool)Wh_GetIntSetting(L"right_bright");
    g_modules.r_vol = (bool)Wh_GetIntSetting(L"right_volume");
    g_modules.r_res = (bool)Wh_GetIntSetting(L"right_res");
    g_modules.r_usb = (bool)Wh_GetIntSetting(L"right_usb");
}

LRESULT CALLBACK BarWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    TopBarInstance* bar = GetBarInstance(hwnd);
    if (msg == g_shellHookMsg && (wParam == HSHELL_WINDOWACTIVATED || wParam == HSHELL_RUDEAPPACTIVATED)) {
        for (auto& b : g_bars) PostMessage(b.hwndBar, WM_CHECK_VISIBILITY, 0, 0);
        InvalidateRect(hwnd, NULL, FALSE);
    }
    switch (msg) {
        case WM_CREATE: SetTimer(hwnd, TIMER_MOUSE_CHECK, 400, NULL); SetTimer(hwnd, TIMER_CLOCK_TICK, 1000, NULL); return 0;
        case WM_TIMER:
            if (wParam == TIMER_MOUSE_CHECK && bar) UpdateState(bar);
            if (wParam == TIMER_CLOCK_TICK) { SYSTEMTIME st; GetLocalTime(&st); g_cache.Update(); if (st.wMinute != g_modManager.lastMinute) { g_modManager.lastMinute = st.wMinute; InvalidateRect(hwnd, NULL, FALSE); } }
            if (wParam == TIMER_ANIMATION && bar) {
                int dist = bar->targetY - bar->currentY;
                if (dist == 0) { bar->isAnimating = false; KillTimer(hwnd, TIMER_ANIMATION); }
                else { bar->currentY += (dist > 0) ? std::min(dist, 4) : std::max(dist, -4);
                       SetWindowPos(hwnd, HWND_TOPMOST, bar->monitorRect.left + 8, bar->monitorRect.top + bar->currentY, (bar->monitorRect.right - bar->monitorRect.left) - 16, 30, SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS); }
            } return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps; HDC hdc = BeginPaint(hwnd, &ps); RECT rc; GetClientRect(hwnd, &rc); g_cache.Update();
            HDC mDC = CreateCompatibleDC(hdc); HBITMAP mBM = CreateCompatibleBitmap(hdc, rc.right, rc.bottom); SelectObject(mDC, mBM);
            Graphics g(mDC); g.Clear(g_cache.darkMode ? Color(32,32,32) : Color(240,240,240));
            g_modManager.Render(g, rc.right, g_cache.darkMode); BitBlt(hdc, 0, 0, rc.right, rc.bottom, mDC, 0, 0, SRCCOPY);
            DeleteObject(mBM); DeleteDC(mDC); EndPaint(hwnd, &ps); return 0;
        }
        case WM_MOUSEMOVE: case WM_LBUTTONDOWN: case WM_LBUTTONUP: { POINT pt = { (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam) }; g_modManager.HandleMouse(hwnd, msg, pt); return 0; }
        case WM_ERASEBKGND: return 1;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

BOOL CALLBACK MonitorEnumProc(HMONITOR hM, HDC hdc, LPRECT lprc, LPARAM dw) {
    MONITORINFO mi = {sizeof(mi)}; GetMonitorInfo(hM, &mi);
    TopBarInstance bar; bar.hMonitor = hM; bar.monitorRect = mi.rcMonitor;
    bar.currentY = HIDE_OFFSET; bar.targetY = 2; bar.isAnimating = bar.isLocked = false;
    int fw = mi.rcMonitor.right - mi.rcMonitor.left;
    bar.hwndTrigger = CreateWindowExW(WS_EX_TOPMOST|WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE|WS_EX_LAYERED, L"TopBarT", NULL, WS_POPUP|WS_VISIBLE, mi.rcMonitor.left, mi.rcMonitor.top, fw, 2, NULL, NULL, g_hInstance, NULL);
    SetLayeredWindowAttributes(bar.hwndTrigger, 0, 1, LWA_ALPHA);
    bar.hwndBar = CreateWindowExW(WS_EX_TOPMOST|WS_EX_TOOLWINDOW|WS_EX_LAYERED|WS_EX_NOACTIVATE, L"TopBarC", NULL, WS_POPUP|WS_VISIBLE, mi.rcMonitor.left+8, mi.rcMonitor.top+HIDE_OFFSET, fw-16, 30, NULL, NULL, g_hInstance, NULL);
    SetLayeredWindowAttributes(bar.hwndBar, 0, 255, LWA_ALPHA);
    DWM_WINDOW_CORNER_PREFERENCE cp = DWMWCP_ROUND; DwmSetWindowAttribute(bar.hwndBar, 33, &cp, sizeof(cp));
    BOOL dark = TRUE; DwmSetWindowAttribute(bar.hwndBar, 20, &dark, sizeof(dark));
    g_bars.push_back(bar); return TRUE;
}

BOOL Wh_ModInit() {
    g_cache.Update(); g_hInstance = GetModuleHandle(NULL); LoadSettings();
    GdiplusStartupInput gsi; GdiplusStartup(&g_gdiplusToken, &gsi, NULL);
    g_shellHookMsg = RegisterWindowMessageW(L"SHELLHOOK");
    WNDCLASSEXW wcT = {sizeof(wcT), 0, DefWindowProc, 0, 0, g_hInstance, NULL, NULL, NULL, NULL, L"TopBarT", NULL}; RegisterClassExW(&wcT);
    WNDCLASSEXW wcB = {sizeof(wcB), CS_HREDRAW|CS_VREDRAW, BarWndProc, 0, 0, g_hInstance, NULL, NULL, NULL, NULL, L"TopBarC", NULL}; RegisterClassExW(&wcB);
    EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, 0);
    if (!g_bars.empty()) RegisterShellHookWindow(g_bars[0].hwndBar);
    return TRUE;
}

void Wh_ModUninit() {
    if (!g_bars.empty()) DeregisterShellHookWindow(g_bars[0].hwndBar);
    for (auto& b : g_bars) { DestroyWindow(b.hwndTrigger); DestroyWindow(b.hwndBar); }
    GdiplusShutdown(g_gdiplusToken);
}

void Wh_ModSettingsChanged() { LoadSettings(); for (auto& b : g_bars) InvalidateRect(b.hwndBar, NULL, FALSE); }
