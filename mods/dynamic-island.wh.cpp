// ==WindhawkMod==
// @id              dynamic-island-shweps
// @name            Dynamic Island
// @name:ru-RU      Динамический Остров
// @description     A macOS-style Dynamic Island for Windows 11. Hover, click, auto-compact when apps open, with time, date, weather, battery and keyboard layout.
// @description:ru-RU Динамический остров в стиле macOS для Windows 11. Наведение, клик, авто-сворачивание при открытии приложений, с временем, датой, погодой, батареей и раскладкой.
// @version         1.0
// @author          shweps
// @github          https://github.com/shweps
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -lgdi32 -luser32 -lwinhttp
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Dynamic Island

## English
A lightweight, animated overlay inspired by the iPhone Dynamic Island.

**Features:**
- **Hover 3s** — island expands to show more info
- **Left click** — expands horizontally with date and temperature
- **Auto-compact** — shrinks to a dot when any app is focused (except games/fullscreen)
- **Click the dot** — expands back instantly
- **Battery icon** — live level, updates once per minute
- **Weather** — real outdoor temperature via wttr.in (updates every 10 min)
- **Keyboard layout** — shows current input language
- **Time & Date** — centered, updates every second

**Warning:** Setting animation interval below 16 ms or hover delay below 500 ms may cause high CPU usage or DWM instability.

## Русский
Лёгкий анимированный оверлей в стиле Dynamic Island с iPhone.

**Возможности:**
- **Наведение 3 сек** — остров расширяется
- **ЛКМ** — расширяется по горизонтали с датой и температурой
- **Авто-сворачивание** — сжимается в точку, когда открыто любое приложение
- **Клик по точке** — мгновенно восстанавливает размер
- **Батарея** — уровень заряда, обновляется раз в минуту
- **Погода** — реальная температура через wttr.in (обновление каждые 10 мин)
- **Раскладка клавиатуры** — текущий язык ввода
- **Время и дата** — по центру, обновление каждую секунду

**Предупреждение:** Значения интервала анимации ниже 16 мс или задержки наведения ниже 500 мс могут вызвать высокую нагрузку на CPU или нестабильность DWM.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- HoverDelayMs: 3000
  $name: Hover delay (ms)
  $name:ru-RU: Задержка наведения (мс)
  $description: Time before expanding on hover. Values below 1000 may cause instability.
  $description:ru-RU: Время перед расширением при наведении. Значения ниже 1000 могут вызвать нестабильность.
- AnimationIntervalMs: 33
  $name: Animation interval (ms)
  $name:ru-RU: Интервал анимации (мс)
  $description: Lower = smoother, but higher CPU usage. Below 16 may crash DWM.
  $description:ru-RU: Меньше = плавнее, но больше нагрузка. Ниже 16 может привести к падению DWM.
- WeatherUpdateMinutes: 10
  $name: Weather update interval (min)
  $name:ru-RU: Интервал обновления погоды (мин)
  $description: How often to fetch temperature. Very low values may trigger rate limits.
  $description:ru-RU: Как часто запрашивать температуру. Слишком часто — риск блокировки по rate limit.
- ShowTime: true
  $name: Show time
  $name:ru-RU: Показывать время
- ShowDate: true
  $name: Show date on click
  $name:ru-RU: Показывать дату при клике
- ShowWeather: true
  $name: Show temperature
  $name:ru-RU: Показывать температуру
- ShowBattery: true
  $name: Show battery
  $name:ru-RU: Показывать батарею
- ShowLanguage: true
  $name: Show keyboard layout
  $name:ru-RU: Показывать раскладку клавиатуры
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <winhttp.h>
#include <dwmapi.h>

#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

// ------------------------------------------------------------------
// Constants
// ------------------------------------------------------------------
#define ISLAND_W_NORMAL        180
#define ISLAND_H_NORMAL        40
#define ISLAND_W_HOVER_EXPAND  320
#define ISLAND_H_HOVER_EXPAND  80
#define ISLAND_W_CLICK_EXPAND  480
#define ISLAND_H_CLICK_EXPAND  ISLAND_H_NORMAL
#define ISLAND_W_COMPACT       36
#define ISLAND_H_COMPACT       36
#define TIMER_ID               1
#define TIMER_ID_TIME          2
#define TIMER_INTERVAL_TIME    500
#define LERP_FACTOR            0.22f

static HWND   g_hWndIsland     = NULL;
static UINT   g_TimerId        = 0;
static UINT   g_TimerIdTime    = 0;
static BOOL   g_bHovered       = FALSE;
static BOOL   g_bTrackingMouse = FALSE;
static BOOL   g_bInTimer       = FALSE;
static BOOL   g_bClickExpanded = FALSE;
static BOOL   g_bCompactMode   = FALSE;
static DWORD  g_HoverStartTime = 0;
static float  g_CurW           = ISLAND_W_NORMAL;
static float  g_CurH           = ISLAND_H_NORMAL;
static HFONT  g_hFont          = NULL;
static HFONT  g_hFontSmall     = NULL;

// Weather thread
static WCHAR        g_szTemp[32]          = L"--\u00B0";
static HANDLE       g_hWeatherThread      = NULL;
static volatile BOOL g_bWeatherRunning    = FALSE;

// Battery cache (update once per minute)
static int   g_BatteryPct        = -1;
static DWORD g_LastBatteryCheck  = 0;

// Settings
static int   g_SetHoverDelay       = 3000;
static int   g_SetAnimInterval     = 33;
static int   g_SetWeatherInterval  = 10;
static BOOL  g_SetShowTime         = TRUE;
static BOOL  g_SetShowDate         = TRUE;
static BOOL  g_SetShowWeather      = TRUE;
static BOOL  g_SetShowBattery      = TRUE;
static BOOL  g_SetShowLanguage     = TRUE;

// ------------------------------------------------------------------
// Helpers
// ------------------------------------------------------------------
static void GetPrimaryMonitorWorkArea(RECT* rc)
{
    HMONITOR hMon = MonitorFromWindow(NULL, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi = { sizeof(mi) };
    if (GetMonitorInfoW(hMon, &mi)) {
        *rc = mi.rcWork;
    } else {
        SystemParametersInfoW(SPI_GETWORKAREA, 0, rc, 0);
    }
}

static void ApplyIslandGeometry(HWND hWnd, float w, float h)
{
    RECT rcWork;
    GetPrimaryMonitorWorkArea(&rcWork);

    int iw = (int)(w + 0.5f);
    int ih = (int)(h + 0.5f);
    int x  = rcWork.left + ((rcWork.right - rcWork.left) - iw) / 2;
    int y  = rcWork.top + 8;

    // SWP_NOSENDCHANGING: don't generate WM_WINDOWPOSCHANGING/CHANGED
    // SWP_NOZORDER: don't touch z-order during animation (set once at creation)
    SetWindowPos(hWnd, NULL, x, y, iw, ih,
                 SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOSENDCHANGING | SWP_NOZORDER);
}

static void StartAnimationTimer(HWND hWnd)
{
    if (g_TimerId == 0) {
        g_TimerId = SetTimer(hWnd, TIMER_ID, g_SetAnimInterval, NULL);
    }
}

static void KillAnimationTimer(HWND hWnd)
{
    if (g_TimerId != 0) {
        KillTimer(hWnd, g_TimerId);
        g_TimerId = 0;
    }
}

// ------------------------------------------------------------------
// Content helpers
// ------------------------------------------------------------------
static void GetCurrentTimeString(WCHAR* buf, int bufChars)
{
    SYSTEMTIME st;
    GetLocalTime(&st);
    GetTimeFormatW(LOCALE_USER_DEFAULT, TIME_NOSECONDS, &st, NULL, buf, bufChars);
}

static void GetCurrentLangString(WCHAR* buf, int bufChars)
{
    // Get layout of the foreground window, not explorer's thread
    HWND hwndFore = GetForegroundWindow();
    DWORD dwThreadId = GetWindowThreadProcessId(hwndFore, NULL);
    HKL hkl = GetKeyboardLayout(dwThreadId);
    LANGID langId = LOWORD(hkl);
    // Try ISO 639-1 two-letter code first (en, ru, etc.)
    if (GetLocaleInfoW(langId, LOCALE_SISO639LANGNAME, buf, bufChars) == 0) {
        // fallback to abbreviated name
        GetLocaleInfoW(langId, LOCALE_SABBREVLANGNAME, buf, bufChars);
    }
}

static void DrawBattery(HDC hdc, int x, int y, int w, int h)
{
    // Cache battery level, refresh once per minute
    DWORD now = GetTickCount();
    if (g_BatteryPct < 0 || (now - g_LastBatteryCheck) > 60000) {
        SYSTEM_POWER_STATUS ps;
        if (GetSystemPowerStatus(&ps)) {
            g_BatteryPct = ps.BatteryLifePercent;
            if (g_BatteryPct > 100) g_BatteryPct = 100;
            if (g_BatteryPct < 0)   g_BatteryPct = 0;
        } else {
            g_BatteryPct = 0;
        }
        g_LastBatteryCheck = now;
    }

    int pct = g_BatteryPct;

    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    // Main body
    RoundRect(hdc, x, y, x + w - 3, y + h, 2, 2);

    // Tip
    RECT rcTip = { x + w - 3, y + 3, x + w, y + h - 3 };
    FillRect(hdc, &rcTip, (HBRUSH)GetStockObject(WHITE_BRUSH));

    // Fill level
    if (pct > 0) {
        int fillW = ((w - 7) * pct) / 100;
        if (fillW < 2) fillW = 2;
        RECT rcFill = { x + 2, y + 2, x + 2 + fillW, y + h - 2 };
        HBRUSH hFill = CreateSolidBrush(RGB(255, 255, 255));
        FillRect(hdc, &rcFill, hFill);
        DeleteObject(hFill);
    }

    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hPen);
}

static void GetCurrentDateString(WCHAR* buf, int bufChars)
{
    SYSTEMTIME st;
    GetLocalTime(&st);
    GetDateFormatW(LOCALE_USER_DEFAULT, 0, &st, L"dd.MM.yy", buf, bufChars);
}

static void LoadSettings()
{
    int val;

    val = Wh_GetIntSetting(L"HoverDelayMs");
    if (val >= 500 && val <= 10000) g_SetHoverDelay = val;

    val = Wh_GetIntSetting(L"AnimationIntervalMs");
    if (val >= 16 && val <= 100) {
        if (val != g_SetAnimInterval) {
            g_SetAnimInterval = val;
            // Restart animation timer with new interval if running
            if (g_hWndIsland && IsWindow(g_hWndIsland) && g_TimerId != 0) {
                KillTimer(g_hWndIsland, g_TimerId);
                g_TimerId = SetTimer(g_hWndIsland, TIMER_ID, g_SetAnimInterval, NULL);
            }
        }
    }

    val = Wh_GetIntSetting(L"WeatherUpdateMinutes");
    if (val >= 5 && val <= 60) g_SetWeatherInterval = val;

    g_SetShowTime     = Wh_GetIntSetting(L"ShowTime")     != 0;
    g_SetShowDate     = Wh_GetIntSetting(L"ShowDate")     != 0;
    g_SetShowWeather  = Wh_GetIntSetting(L"ShowWeather")  != 0;
    g_SetShowBattery  = Wh_GetIntSetting(L"ShowBattery")  != 0;
    g_SetShowLanguage = Wh_GetIntSetting(L"ShowLanguage") != 0;
}

// ------------------------------------------------------------------
// Weather thread
// ------------------------------------------------------------------
static DWORD WINAPI WeatherThreadProc(LPVOID lpParam)
{
    (void)lpParam;

    while (g_bWeatherRunning) {
        HINTERNET hSession = WinHttpOpen(
            L"DynamicIsland/1.0",
            WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
            WINHTTP_NO_PROXY_NAME,
            WINHTTP_NO_PROXY_BYPASS,
            0);

        if (hSession) {
            WinHttpSetTimeouts(hSession, 5000, 5000, 5000, 5000);

            HINTERNET hConnect = WinHttpConnect(
                hSession, L"wttr.in", INTERNET_DEFAULT_HTTPS_PORT, 0);

            if (hConnect) {
                HINTERNET hRequest = WinHttpOpenRequest(
                    hConnect, L"GET", L"/?format=%t&M",
                    NULL, WINHTTP_NO_REFERER,
                    WINHTTP_DEFAULT_ACCEPT_TYPES,
                    WINHTTP_FLAG_SECURE);

                if (hRequest) {
                    if (WinHttpSendRequest(hRequest,
                        WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                        WINHTTP_NO_REQUEST_DATA, 0,
                        0, 0)) {
                        if (WinHttpReceiveResponse(hRequest, NULL)) {
                            char buf[256] = {0};
                            DWORD dwRead = 0;
                            if (WinHttpReadData(hRequest, buf, sizeof(buf)-1, &dwRead)) {
                                // Trim whitespace
                                int start = 0;
                                while (start < (int)dwRead &&
                                       (buf[start] == ' ' || buf[start] == '\n' ||
                                        buf[start] == '\r' || buf[start] == '\t'))
                                    start++;
                                int end = (int)dwRead - 1;
                                while (end > start &&
                                       (buf[end] == ' ' || buf[end] == '\n' ||
                                        buf[end] == '\r' || buf[end] == '\t'))
                                    end--;
                                int len = end - start + 1;
                                if (len > 0 && len < 30) {
                                    WCHAR wbuf[32];
                                    int wlen = MultiByteToWideChar(CP_UTF8, 0,
                                        buf + start, len, wbuf, 32);
                                    if (wlen > 0 && wlen < 32) {
                                        wbuf[wlen] = L'\0';
                                        // Append degree symbol if missing
                                        if (wcschr(wbuf, L'\u00B0') == NULL &&
                                            wcschr(wbuf, L'C') == NULL &&
                                            wcschr(wbuf, L'F') == NULL) {
                                            wcscat_s(wbuf, 32, L"\u00B0");
                                        }
                                        wcscpy_s(g_szTemp, 32, wbuf);
                                    }
                                }
                            }
                        }
                    }
                    WinHttpCloseHandle(hRequest);
                }
                WinHttpCloseHandle(hConnect);
            }
            WinHttpCloseHandle(hSession);
        }

        // Sleep based on setting (check g_bWeatherRunning every second)
        int sleepSeconds = g_SetWeatherInterval * 60;
        for (int i = 0; i < sleepSeconds && g_bWeatherRunning; i++)
            Sleep(1000);
    }
    return 0;
}

// ------------------------------------------------------------------
// Window procedure
// ------------------------------------------------------------------
static LRESULT CALLBACK IslandWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_MOUSEMOVE:
        if (!g_bHovered) {
            g_bHovered = TRUE;
            g_HoverStartTime = GetTickCount();
            StartAnimationTimer(hWnd);
        }
        if (!g_bTrackingMouse) {
            g_bTrackingMouse = TRUE;
            TRACKMOUSEEVENT tme = { sizeof(tme) };
            tme.dwFlags   = TME_LEAVE;
            tme.hwndTrack = hWnd;
            TrackMouseEvent(&tme);
        }
        break;

    case WM_MOUSELEAVE:
        g_bHovered       = FALSE;
        g_bTrackingMouse = FALSE;
        g_bClickExpanded = FALSE; // leave always collapses
        StartAnimationTimer(hWnd);
        break;

    case WM_LBUTTONDOWN:
        if (g_bCompactMode) {
            g_bCompactMode = FALSE;
            StartAnimationTimer(hWnd);
        } else {
            g_bClickExpanded = !g_bClickExpanded;
            g_HoverStartTime = GetTickCount();
            StartAnimationTimer(hWnd);
        }
        break;

    case WM_TIMER:
    {
        if (wParam == TIMER_ID_TIME) {
            if (!IsWindow(g_hWndIsland))
                break;

            // Auto compact: if foreground belongs to another process, shrink
            // BUT only if mouse is NOT hovering the island (user is interacting)
            HWND hwndFore = GetForegroundWindow();
            if (hwndFore != NULL && hwndFore != g_hWndIsland && !g_bHovered) {
                DWORD pid = 0;
                GetWindowThreadProcessId(hwndFore, &pid);
                if (pid != 0 && pid != GetCurrentProcessId()) {
                    if (!g_bCompactMode) {
                        g_bCompactMode = TRUE;
                        g_bClickExpanded = FALSE;
                        StartAnimationTimer(g_hWndIsland);
                    }
                } else {
                    if (g_bCompactMode) {
                        g_bCompactMode = FALSE;
                        StartAnimationTimer(g_hWndIsland);
                    }
                }
            } else if (g_bCompactMode && (hwndFore == NULL || hwndFore == g_hWndIsland || g_bHovered)) {
                // Foreground is explorer/desktop OR mouse is over island — expand back
                g_bCompactMode = FALSE;
                StartAnimationTimer(g_hWndIsland);
            }

            InvalidateRect(g_hWndIsland, NULL, FALSE);
            UpdateWindow(g_hWndIsland);
            SetWindowPos(g_hWndIsland, NULL, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                         SWP_NOACTIVATE | SWP_NOSENDCHANGING | SWP_FRAMECHANGED);
            break;
        }

        if (wParam != TIMER_ID)
            break;
        if (g_bInTimer)
            break;
        g_bInTimer = TRUE;

        if (!IsWindow(g_hWndIsland)) {
            g_TimerId = 0;
            g_bInTimer = FALSE;
            break;
        }

        float targetW = ISLAND_W_NORMAL;
        float targetH = ISLAND_H_NORMAL;

        if (g_bCompactMode) {
            targetW = ISLAND_W_COMPACT;
            targetH = ISLAND_H_COMPACT;
        } else if (g_bClickExpanded) {
            targetW = ISLAND_W_CLICK_EXPAND;
            targetH = ISLAND_H_CLICK_EXPAND;
        } else if (g_bHovered) {
            DWORD elapsed = GetTickCount() - g_HoverStartTime;
            if (elapsed >= (DWORD)g_SetHoverDelay) {
                targetW = ISLAND_W_HOVER_EXPAND;
                targetH = ISLAND_H_HOVER_EXPAND;
            }
        }

        float diffW = targetW - g_CurW;
        float diffH = targetH - g_CurH;
        float adiffW = diffW < 0 ? -diffW : diffW;
        float adiffH = diffH < 0 ? -diffH : diffH;

        if (adiffW < 0.5f && adiffH < 0.5f) {
            g_CurW = targetW;
            g_CurH = targetH;
            ApplyIslandGeometry(hWnd, g_CurW, g_CurH);

            if (!g_bHovered && !g_bClickExpanded && !g_bCompactMode) {
                KillAnimationTimer(hWnd);
            }
        } else {
            g_CurW += diffW * LERP_FACTOR;
            g_CurH += diffH * LERP_FACTOR;
            ApplyIslandGeometry(hWnd, g_CurW, g_CurH);
        }

        g_bInTimer = FALSE;
        break;
    }

    case WM_ERASEBKGND:
        {
            HDC hdc = (HDC)wParam;
            RECT rc;
            GetClientRect(hWnd, &rc);
            HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
            FillRect(hdc, &rc, hBrush);
            DeleteObject(hBrush);
        }
        return 1;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        RECT rc;
        GetClientRect(hWnd, &rc);

        HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
        FillRect(hdc, &rc, hBrush);
        DeleteObject(hBrush);

        int iw = rc.right - rc.left;
        int ih = rc.bottom - rc.top;

        // Compact: draw nothing (DWM rounded corners make it a black dot)
        if (g_bCompactMode || iw < 100 || ih < 20) {
            EndPaint(hWnd, &ps);
            return 0;
        }

        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(255, 255, 255));

        BOOL bExpanded = g_bClickExpanded && iw >= 400;

        int contentLeft  = 8;
        int contentRight = iw - 8;
        int centerX      = iw / 2;

        // --- Battery (left) ---
        if (g_SetShowBattery) {
            DrawBattery(hdc, 6, (ih - 8) / 2, 18, 8);
            contentLeft = 36;
        }

        // --- Language (right) ---
        if (g_SetShowLanguage) {
            WCHAR szLang[16];
            GetCurrentLangString(szLang, 16);
            HFONT hOldFont = (HFONT)SelectObject(hdc, g_hFontSmall);
            RECT rcLang = rc;
            rcLang.left  = iw - 44;
            rcLang.right = iw - 10;
            DrawTextW(hdc, szLang, -1, &rcLang, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
            SelectObject(hdc, hOldFont);
            contentRight = iw - 48;
        }

        // --- Time / Date / Weather ---
        WCHAR szTime[32];
        if (g_SetShowTime)
            GetCurrentTimeString(szTime, 32);
        else
            szTime[0] = L'\0';

        HFONT hOldFont = (HFONT)SelectObject(hdc, g_hFont);

        if (bExpanded && (g_SetShowDate || g_SetShowWeather)) {
            // Expanded layout
            int usableW = contentRight - contentLeft;
            int slotW   = usableW / 3;

            // Date (left slot)
            if (g_SetShowDate) {
                WCHAR szDate[32];
                GetCurrentDateString(szDate, 32);
                RECT rcDate = rc;
                rcDate.left  = contentLeft;
                rcDate.right = contentLeft + slotW;
                DrawTextW(hdc, szDate, -1, &rcDate, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
            }

            // Time (center slot)
            if (g_SetShowTime) {
                RECT rcTime = rc;
                rcTime.left  = contentLeft + slotW;
                rcTime.right = contentLeft + slotW * 2;
                DrawTextW(hdc, szTime, -1, &rcTime, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            }

            // Weather (right slot)
            if (g_SetShowWeather) {
                RECT rcTemp = rc;
                rcTemp.left  = contentLeft + slotW * 2;
                rcTemp.right = contentRight;
                DrawTextW(hdc, g_szTemp, -1, &rcTemp, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
            }
        } else {
            // Normal layout: just time centered
            if (g_SetShowTime) {
                RECT rcTime = rc;
                rcTime.left  = contentLeft;
                rcTime.right = contentRight;
                DrawTextW(hdc, szTime, -1, &rcTime, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            }
        }

        SelectObject(hdc, hOldFont);

        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        g_hWndIsland     = NULL;
        g_TimerId        = 0;
        g_bHovered       = FALSE;
        g_bTrackingMouse = FALSE;
        g_bInTimer       = FALSE;
        g_bClickExpanded = FALSE;
        g_bCompactMode   = FALSE;
        g_HoverStartTime = 0;
        g_BatteryPct     = -1;
        g_LastBatteryCheck = 0;
        return 0;
    }

    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

// ------------------------------------------------------------------
// Create / Destroy
// ------------------------------------------------------------------
static BOOL CreateIslandWindow()
{
    WNDCLASSEXW wc = { sizeof(wc) };
    wc.lpfnWndProc   = IslandWndProc;
    wc.hInstance     = GetModuleHandleW(NULL);
    wc.hCursor       = LoadCursorW(NULL, IDC_ARROW);
    wc.lpszClassName = L"WindhawkDynamicIsland";

    if (!RegisterClassExW(&wc)) {
        if (GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
            Wh_Log(L"RegisterClassExW failed: %lu", GetLastError());
            return FALSE;
        }
    }

    DWORD dwExStyle = WS_EX_TOOLWINDOW |
                      WS_EX_LAYERED |
                      WS_EX_NOACTIVATE;

    g_hWndIsland = CreateWindowExW(
        dwExStyle,
        L"WindhawkDynamicIsland",
        L"Dynamic Island",
        WS_POPUP,
        0, 0, ISLAND_W_NORMAL, ISLAND_H_NORMAL,
        NULL, NULL, wc.hInstance, NULL);

    if (!g_hWndIsland) {
        Wh_Log(L"CreateWindowExW failed: %lu", GetLastError());
        return FALSE;
    }

    g_CurW           = ISLAND_W_NORMAL;
    g_CurH           = ISLAND_H_NORMAL;
    g_bHovered       = FALSE;
    g_bTrackingMouse = FALSE;
    g_bInTimer       = FALSE;
    g_bClickExpanded = FALSE;
    g_bCompactMode   = FALSE;
    g_HoverStartTime = 0;
    g_TimerId        = 0;

    DWM_WINDOW_CORNER_PREFERENCE cornerPref = DWMWCP_ROUND;
    DwmSetWindowAttribute(g_hWndIsland, DWMWA_WINDOW_CORNER_PREFERENCE,
                          &cornerPref, sizeof(cornerPref));

    MARGINS margins = { 0, 0, 0, 0 };
    DwmExtendFrameIntoClientArea(g_hWndIsland, &margins);

    SetLayeredWindowAttributes(g_hWndIsland, 0, 255, LWA_ALPHA);

    // Create fonts — DEFAULT_QUALITY to avoid first-render delay in explorer
    g_hFont = CreateFontW(
        14, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS,
        L"Segoe UI");
    if (!g_hFont) g_hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

    g_hFontSmall = CreateFontW(
        14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS,
        L"Segoe UI");
    if (!g_hFontSmall) g_hFontSmall = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

    ApplyIslandGeometry(g_hWndIsland, g_CurW, g_CurH);
    // One-time: set topmost and show. Never use SWP_SHOWWINDOW in animation.
    SetWindowPos(g_hWndIsland, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    ShowWindow(g_hWndIsland, SW_SHOWNOACTIVATE);

    // Start clock timer (always running)
    g_TimerIdTime = SetTimer(g_hWndIsland, TIMER_ID_TIME, TIMER_INTERVAL_TIME, NULL);

    // Start weather thread
    g_bWeatherRunning = TRUE;
    g_hWeatherThread = CreateThread(NULL, 0, WeatherThreadProc, NULL, 0, NULL);

    Wh_Log(L"Dynamic Island created successfully");
    return TRUE;
}

static void DestroyIslandWindow()
{
    // Stop weather thread first
    g_bWeatherRunning = FALSE;
    if (g_hWeatherThread) {
        WaitForSingleObject(g_hWeatherThread, 2000);
        CloseHandle(g_hWeatherThread);
        g_hWeatherThread = NULL;
    }

    if (g_hWndIsland && IsWindow(g_hWndIsland)) {
        if (g_TimerIdTime != 0) {
            KillTimer(g_hWndIsland, g_TimerIdTime);
            g_TimerIdTime = 0;
        }
        KillAnimationTimer(g_hWndIsland);
        DestroyWindow(g_hWndIsland);
    }
    if (g_hFont) {
        DeleteObject(g_hFont);
        g_hFont = NULL;
    }
    if (g_hFontSmall) {
        DeleteObject(g_hFontSmall);
        g_hFontSmall = NULL;
    }
    g_hWndIsland     = NULL;
    g_TimerId        = 0;
    g_bHovered       = FALSE;
    g_bTrackingMouse = FALSE;
    g_bInTimer       = FALSE;
    g_bClickExpanded = FALSE;
    g_bCompactMode   = FALSE;
    g_HoverStartTime = 0;
    g_BatteryPct     = -1;
    g_LastBatteryCheck = 0;
}

// ------------------------------------------------------------------
// Windhawk callbacks
// ------------------------------------------------------------------
BOOL Wh_ModInit()
{
    Wh_Log(L"Dynamic Island mod initializing...");
    LoadSettings();
    return CreateIslandWindow();
}

void Wh_ModUninit()
{
    Wh_Log(L"Dynamic Island mod unloading...");
    DestroyIslandWindow();
}

BOOL Wh_ModSettingsChanged(BOOL* bReload)
{
    Wh_Log(L"Settings changed, applying...");
    LoadSettings();
    *bReload = FALSE; // apply on-the-fly without mod reload
    return TRUE;
}
