// ==WindhawkMod==
// @id           hanging-v-dice-customizable
// @name         Customizable Dual Hanging Dice
// @description  Hanging dice with physical reactions to window animations and auto-hide in fullscreen mode.
// @version      1.0
// @author       Jaali
// @github       https://github.com/alivca
// @include      explorer.exe
// @compilerOptions -lgdi32 -lgdiplus -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Add interactive hanging dice to your desktop with dynamic physics. They swing and react in real-time when you move, minimize, maximize, or restore windows, and automatically hide during fullscreen games or apps to stay out of your way.

![Preview](https://raw.githubusercontent.com/alivca/windhawk-mods/9f44d9cdda2cef3d7d5240971d32606518fa17c2/cubic.gif)

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- position: center
  $name: Position on Screen
  $description: "Choose where on the screen the hanging dice should be displayed."
  $options:
    - left: Left
    - center: Center
    - right: Right
- diceSize: 50
  $name: Dice Size (Pixels)
  $description: "Sets the base size of the dice in pixels (range: 20 to 120)."
- diceStyle: classic
  $name: Visual Style
  $description: "Color scheme and visual appearance of the dice and strings."
  $options:
    - classic: Classic (Red + Black)
    - neon: Neon (Cyan + Pink)
    - mono: Monochrome (White + Gray)
- randomOnClick: false
  $name: Random Value on Click
  $description: "If enabled, clicking a die changes its face to a random number from 1 to 6."
- leftDiceValue: 5
  $name: Left Die Fixed Value
  $description: "Fixed dot value (1 to 6) used when random on click is disabled."
- rightDiceValue: 3
  $name: Right Die Fixed Value
  $description: "Fixed dot value (1 to 6) used when random on click is disabled."
- hideOnFullscreen: false
  $name: Hide in Fullscreen Mode (F11 / Fullscreen)
  $description: "Automatically hides the overlay when the active application enters fullscreen mode."
*/
// ==/WindhawkModSettings==

#define WINVER 0x0601
#define _WIN32_WINNT 0x0601

#include <windows.h>
#include <gdiplus.h>
#include <math.h>
#include <string>
#include <stdlib.h>
#include <time.h>

#ifndef WINEVENT_OUTOFPROCESS
#define WINEVENT_OUTOFPROCESS 0x0000
#endif

using namespace Gdiplus;

#define PI 3.14159265358979323846f

struct Settings {
    std::wstring position = L"center";
    int diceSize = 40;
    std::wstring diceStyle = L"classic";
    bool randomOnClick = true;
    int leftDiceValue = 5;
    int rightDiceValue = 3;
    bool hideOnFullscreen = true;
} g_settings;

int g_winW = 350;
int g_winH = 350;

struct Dice {
    float x, y;
    float vx, vy;
    float size;
    float stringLen;
    bool isDragged;
    int dots;
    POINT dragStartPt;
};

PointF g_anchor(175.0f, 0.0f);
Dice g_d1 = { 130.0f, 110.0f, 0.0f, 0.0f, 40.0f, 110.0f, false, 5, {0, 0} };
Dice g_d2 = { 220.0f, 110.0f, 0.0f, 0.0f, 40.0f, 110.0f, false, 3, {0, 0} };

HWND g_hWnd = NULL;
HANDLE g_hThread = NULL;
HWINEVENTHOOK g_hEventHookMin = NULL;
HWINEVENTHOOK g_hEventHookUnmin = NULL;
HWINEVENTHOOK g_hEventHookMax = NULL;
HWINEVENTHOOK g_hEventHookPos = NULL;
bool g_running = false;
ULONG_PTR g_gdiToken = 0;
bool g_isHiddenByFullscreen = false;

int ClampDiceValue(int val) {
    if (val < 1) return 1;
    if (val > 6) return 6;
    return val;
}

bool IsWindowFullscreen(HWND hwnd) {
    if (!hwnd || hwnd == GetDesktopWindow() || hwnd == GetShellWindow()) return false;

    RECT appBounds;
    if (!GetWindowRect(hwnd, &appBounds)) return false;

    HMONITOR hMon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(MONITORINFO) };
    if (!GetMonitorInfo(hMon, &mi)) return false;

    return (appBounds.left <= mi.rcMonitor.left &&
            appBounds.top <= mi.rcMonitor.top &&
            appBounds.right >= mi.rcMonitor.right &&
            appBounds.bottom >= mi.rcMonitor.bottom);
}

void CheckFullscreenState() {
    if (!g_settings.hideOnFullscreen || !g_hWnd) {
        if (g_isHiddenByFullscreen) {
            ShowWindow(g_hWnd, SW_SHOWNOACTIVATE);
            SetWindowPos(g_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            g_isHiddenByFullscreen = false;
        }
        return;
    }

    HWND hForeground = GetForegroundWindow();
    if (hForeground && hForeground != g_hWnd) {
        bool fs = IsWindowFullscreen(hForeground);
        if (fs && !g_isHiddenByFullscreen) {
            ShowWindow(g_hWnd, SW_HIDE);
            g_isHiddenByFullscreen = true;
        } else if (!fs && g_isHiddenByFullscreen) {
            ShowWindow(g_hWnd, SW_SHOWNOACTIVATE);
            SetWindowPos(g_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            g_isHiddenByFullscreen = false;
        }
    }
}

void ApplySettings() {
    float size = (float)g_settings.diceSize;
    if (size < 20.0f) size = 20.0f;
    if (size > 120.0f) size = 120.0f;

    float strLen = size * 2.85f;

    g_d1.size = size;
    g_d1.stringLen = strLen;
    g_d2.size = size;
    g_d2.stringLen = strLen;

    if (!g_settings.randomOnClick) {
        g_d1.dots = ClampDiceValue(g_settings.leftDiceValue);
        g_d2.dots = ClampDiceValue(g_settings.rightDiceValue);
    }

    g_winW = (int)(size * 8.5f);
    g_winH = (int)(strLen + size * 3.0f);

    g_anchor = PointF((float)g_winW / 2.0f, 0.0f);

    if (!g_d1.isDragged) {
        g_d1.x = g_anchor.X - size * 0.9f;
        g_d1.y = strLen;
    }
    if (!g_d2.isDragged) {
        g_d2.x = g_anchor.X + size * 0.9f;
        g_d2.y = strLen;
    }

    if (g_hWnd) {
        int screenW = GetSystemMetrics(SM_CXSCREEN);
        int posX = 10;

        if (g_settings.position == L"center") {
            posX = (screenW - g_winW) / 2;
        } else if (g_settings.position == L"right") {
            posX = screenW - g_winW - 10;
        }

        SetWindowPos(g_hWnd, HWND_TOPMOST, posX, 10, g_winW, g_winH, SWP_NOACTIVATE | SWP_SHOWWINDOW);
        CheckFullscreenState();
    }
}

void CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
    if (!hwnd || hwnd == g_hWnd || idObject != OBJID_WINDOW) return;

    if (event == EVENT_SYSTEM_MINIMIZESTART) {
        g_d1.vy += 7.5f;
        g_d1.vx += (rand() % 2 == 0 ? 3.5f : -3.5f);

        g_d2.vy += 9.0f;
        g_d2.vx += (rand() % 2 == 0 ? 4.5f : -4.5f);
    }
    else if (event == EVENT_SYSTEM_MINIMIZEEND) {
        g_d1.vy -= 8.0f;
        g_d1.vx += (rand() % 2 == 0 ? 4.0f : -4.0f);

        g_d2.vy -= 9.5f;
        g_d2.vx += (rand() % 2 == 0 ? 5.0f : -5.0f);
    }
    else if (event == EVENT_SYSTEM_MOVESIZEEND) {
        if (IsZoomed(hwnd)) {
            g_d1.vy -= 6.0f;
            g_d1.vx += (rand() % 2 == 0 ? 3.0f : -3.0f);

            g_d2.vy -= 7.0f;
            g_d2.vx += (rand() % 2 == 0 ? 3.5f : -3.5f);
        }
    }

    CheckFullscreenState();
}

void ConstrainToBounds(Dice& d) {
    float half = d.size / 2.0f;

    if (d.x - half < 0.0f) {
        d.x = half;
        if (d.vx < 0) d.vx *= -0.3f;
    }
    if (d.x + half > (float)g_winW) {
        d.x = (float)g_winW - half;
        if (d.vx > 0) d.vx *= -0.3f;
    }
    if (d.y - half < 0.0f) {
        d.y = half;
        if (d.vy < 0) d.vy *= -0.3f;
    }
    if (d.y + half > (float)g_winH) {
        d.y = (float)g_winH - half;
        if (d.vy > 0) d.vy *= -0.3f;
    }
}

void UpdateDicePhysics(Dice& d) {
    POINT pt;
    GetCursorPos(&pt);
    if (g_hWnd) ScreenToClient(g_hWnd, &pt);

    if (d.isDragged) {
        d.vx = ((float)pt.x - d.x) * 0.35f;
        d.vy = ((float)pt.y - d.y) * 0.35f;
        d.x = (float)pt.x;
        d.y = (float)pt.y;
    } else {
        d.vy += 0.45f;
        d.x += d.vx;
        d.y += d.vy;

        float dx = d.x - g_anchor.X;
        float dy = d.y - g_anchor.Y;
        float currentDist = sqrtf(dx * dx + dy * dy);

        if (currentDist > 0.001f) {
            float nx = dx / currentDist;
            float ny = dy / currentDist;

            if (currentDist > d.stringLen) {
                float stretch = currentDist - d.stringLen;
                float springK = 0.18f;

                d.vx -= nx * stretch * springK;
                d.vy -= ny * stretch * springK;
            }
        }

        d.vx *= 0.982f;
        d.vy *= 0.982f;
    }

    ConstrainToBounds(d);
}

void ResolveCollision(Dice& d1, Dice& d2) {
    float dx = d2.x - d1.x;
    float dy = d2.y - d1.y;
    float dist = sqrtf(dx * dx + dy * dy);
    float minDist = (d1.size + d2.size) * 0.45f;

    if (dist < minDist && dist > 0.001f) {
        float nx = dx / dist;
        float ny = dy / dist;
        float overlap = minDist - dist;

        if (!d1.isDragged) {
            d1.x -= nx * overlap * 0.5f;
            d1.y -= ny * overlap * 0.5f;
        }
        if (!d2.isDragged) {
            d2.x += nx * overlap * 0.5f;
            d2.y += ny * overlap * 0.5f;
        }

        float kx = d1.vx - d2.vx;
        float ky = d1.vy - d2.vy;
        float p = (nx * kx + ny * ky);

        if (p > 0) {
            float restitution = 0.75f;
            d1.vx -= p * nx * restitution;
            d1.vy -= p * ny * restitution;
            d2.vx += p * nx * restitution;
            d2.vy += p * ny * restitution;
        }
    }
}

void PhysicsStep() {
    UpdateDicePhysics(g_d1);
    UpdateDicePhysics(g_d2);
    ResolveCollision(g_d1, g_d2);
}

void DrawScene(Graphics& gr) {
    gr.SetSmoothingMode(SmoothingModeAntiAlias);
    gr.Clear(Color(0, 0, 0, 0));

    Color stringColor(200, 220, 220, 220);
    if (g_settings.diceStyle == L"neon") stringColor = Color(220, 0, 255, 200);

    Pen stringPen(stringColor, 2.0f);
    gr.DrawLine(&stringPen, g_anchor.X, g_anchor.Y, g_d1.x, g_d1.y);
    gr.DrawLine(&stringPen, g_anchor.X, g_anchor.Y, g_d2.x, g_d2.y);

    auto DrawCube = [&](const Dice& d, Color bg, Color stroke, Color dotColor) {
        GraphicsState st = gr.Save();
        
        float angle = atan2f(d.x - g_anchor.X, d.y - g_anchor.Y) * (180.0f / PI);
        
        gr.TranslateTransform(d.x, d.y);
        gr.RotateTransform(-angle);

        float half = d.size / 2.0f;
        RectF r(-half, -half, d.size, d.size);

        SolidBrush b(bg);
        Pen p(stroke, 2.0f);
        SolidBrush dotB(dotColor);

        gr.FillRectangle(&b, r);
        gr.DrawRectangle(&p, r);

        float rDot = d.size * 0.065f;
        float offset = half * 0.5f;

        switch (d.dots) {
        case 1:
            gr.FillEllipse(&dotB, -rDot, -rDot, rDot * 2, rDot * 2);
            break;

        case 2:
            gr.FillEllipse(&dotB, -offset - rDot, -offset - rDot, rDot * 2, rDot * 2);
            gr.FillEllipse(&dotB, offset - rDot, offset - rDot, rDot * 2, rDot * 2);
            break;

        case 3:
            gr.FillEllipse(&dotB, -rDot, -rDot, rDot * 2, rDot * 2);
            gr.FillEllipse(&dotB, -offset - rDot, -offset - rDot, rDot * 2, rDot * 2);
            gr.FillEllipse(&dotB, offset - rDot, offset - rDot, rDot * 2, rDot * 2);
            break;

        case 4:
            gr.FillEllipse(&dotB, -offset - rDot, -offset - rDot, rDot * 2, rDot * 2);
            gr.FillEllipse(&dotB, offset - rDot, -offset - rDot, rDot * 2, rDot * 2);
            gr.FillEllipse(&dotB, -offset - rDot, offset - rDot, rDot * 2, rDot * 2);
            gr.FillEllipse(&dotB, offset - rDot, offset - rDot, rDot * 2, rDot * 2);
            break;

        case 5:
            gr.FillEllipse(&dotB, -rDot, -rDot, rDot * 2, rDot * 2);
            gr.FillEllipse(&dotB, -offset - rDot, -offset - rDot, rDot * 2, rDot * 2);
            gr.FillEllipse(&dotB, offset - rDot, -offset - rDot, rDot * 2, rDot * 2);
            gr.FillEllipse(&dotB, -offset - rDot, offset - rDot, rDot * 2, rDot * 2);
            gr.FillEllipse(&dotB, offset - rDot, offset - rDot, rDot * 2, rDot * 2);
            break;

        case 6:
            gr.FillEllipse(&dotB, -offset - rDot, -offset - rDot, rDot * 2, rDot * 2);
            gr.FillEllipse(&dotB, -offset - rDot, -rDot, rDot * 2, rDot * 2);
            gr.FillEllipse(&dotB, -offset - rDot, offset - rDot, rDot * 2, rDot * 2);
            gr.FillEllipse(&dotB, offset - rDot, -offset - rDot, rDot * 2, rDot * 2);
            gr.FillEllipse(&dotB, offset - rDot, -rDot, rDot * 2, rDot * 2);
            gr.FillEllipse(&dotB, offset - rDot, offset - rDot, rDot * 2, rDot * 2);
            break;
        }

        gr.Restore(st);
    };

    if (g_settings.diceStyle == L"neon") {
        DrawCube(g_d1, Color(220, 10, 200, 230), Color(255, 0, 255, 255), Color(255, 255, 255, 255));
        DrawCube(g_d2, Color(220, 230, 20, 120), Color(255, 255, 50, 180), Color(255, 255, 255, 255));
    } else if (g_settings.diceStyle == L"mono") {
        DrawCube(g_d1, Color(240, 240, 240, 240), Color(255, 50, 50, 50), Color(255, 20, 20, 20));
        DrawCube(g_d2, Color(240, 45, 45, 45), Color(255, 200, 200, 200), Color(255, 240, 240, 240));
    } else {
        DrawCube(g_d1, Color(230, 210, 40, 40), Color(255, 30, 30, 30), Color(255, 255, 255, 255));
        DrawCube(g_d2, Color(230, 30, 30, 30), Color(255, 100, 100, 100), Color(255, 255, 255, 255));
    }
}

void RedrawOverlay(HWND hWnd) {
    if (g_isHiddenByFullscreen) return;

    HDC hdcScr = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScr);
    HBITMAP hBmp = CreateCompatibleBitmap(hdcScr, g_winW, g_winH);
    HBITMAP hOld = (HBITMAP)SelectObject(hdcMem, hBmp);

    Graphics gr(hdcMem);
    DrawScene(gr);

    BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    POINT ptDst = { 0, 0 };
    RECT rc;
    GetWindowRect(hWnd, &rc);
    ptDst.x = rc.left;
    ptDst.y = rc.top;

    SIZE szDst = { g_winW, g_winH };
    POINT ptSrc = { 0, 0 };

    UpdateLayeredWindow(hWnd, hdcScr, &ptDst, &szDst, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);

    SelectObject(hdcMem, hOld);
    DeleteObject(hBmp);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScr);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_TIMER:
        PhysicsStep();
        RedrawOverlay(hWnd);
        break;

    case WM_LBUTTONDOWN: {
        int mx = (short)LOWORD(lParam);
        int my = (short)HIWORD(lParam);

        float dist2 = sqrtf((mx - g_d2.x)*(mx - g_d2.x) + (my - g_d2.y)*(my - g_d2.y));
        if (dist2 < g_d2.size) {
            g_d2.isDragged = true;
            g_d2.dragStartPt = { mx, my };
            SetCapture(hWnd);
            return 0;
        }

        float dist1 = sqrtf((mx - g_d1.x)*(mx - g_d1.x) + (my - g_d1.y)*(my - g_d1.y));
        if (dist1 < g_d1.size) {
            g_d1.isDragged = true;
            g_d1.dragStartPt = { mx, my };
            SetCapture(hWnd);
            return 0;
        }
        break;
    }

    case WM_LBUTTONUP: {
        int mx = (short)LOWORD(lParam);
        int my = (short)HIWORD(lParam);

        if (g_d1.isDragged) {
            int dx = abs(mx - g_d1.dragStartPt.x);
            int dy = abs(my - g_d1.dragStartPt.y);
            if (dx < 5 && dy < 5 && g_settings.randomOnClick) {
                g_d1.dots = (rand() % 6) + 1;
            }
            g_d1.isDragged = false;
        }

        if (g_d2.isDragged) {
            int dx = abs(mx - g_d2.dragStartPt.x);
            int dy = abs(my - g_d2.dragStartPt.y);
            if (dx < 5 && dy < 5 && g_settings.randomOnClick) {
                g_d2.dots = (rand() % 6) + 1;
            }
            g_d2.isDragged = false;
        }

        ReleaseCapture();
        break;
    }

    case WM_NCHITTEST: {
        POINT pt = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
        ScreenToClient(hWnd, &pt);

        float dist1 = sqrtf((pt.x - g_d1.x)*(pt.x - g_d1.x) + (pt.y - g_d1.y)*(pt.y - g_d1.y));
        float dist2 = sqrtf((pt.x - g_d2.x)*(pt.x - g_d2.x) + (pt.y - g_d2.y)*(pt.y - g_d2.y));

        if (dist1 < g_d1.size || dist2 < g_d2.size) {
            return HTCLIENT;
        }
        return HTTRANSPARENT;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

DWORD WINAPI StartThread(LPVOID) {
    srand((unsigned int)time(NULL));

    GdiplusStartupInput input;
    GdiplusStartup(&g_gdiToken, &input, NULL);

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"DiceCustomizableOverlay";
    RegisterClass(&wc);

    g_hWnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW,
        L"DiceCustomizableOverlay", L"Dice",
        WS_POPUP,
        10, 10, g_winW, g_winH,
        NULL, NULL, GetModuleHandle(NULL), NULL
    );

    if (g_hWnd) {
        ApplySettings();
        SetWindowPos(g_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
        ShowWindow(g_hWnd, SW_SHOW);
        SetTimer(g_hWnd, 1, 16, NULL);

        g_hEventHookMin = SetWinEventHook(
            EVENT_SYSTEM_MINIMIZESTART, EVENT_SYSTEM_MINIMIZESTART,
            NULL, WinEventProc, 0, 0, WINEVENT_OUTOFPROCESS
        );

        g_hEventHookUnmin = SetWinEventHook(
            EVENT_SYSTEM_MINIMIZEEND, EVENT_SYSTEM_MINIMIZEEND,
            NULL, WinEventProc, 0, 0, WINEVENT_OUTOFPROCESS
        );

        g_hEventHookMax = SetWinEventHook(
            EVENT_SYSTEM_MOVESIZEEND, EVENT_SYSTEM_MOVESIZEEND,
            NULL, WinEventProc, 0, 0, WINEVENT_OUTOFPROCESS
        );

        g_hEventHookPos = SetWinEventHook(
            EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE,
            NULL, WinEventProc, 0, 0, WINEVENT_OUTOFPROCESS
        );

        MSG msg;
        while (g_running && GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (g_hEventHookMin) UnhookWinEvent(g_hEventHookMin);
        if (g_hEventHookUnmin) UnhookWinEvent(g_hEventHookUnmin);
        if (g_hEventHookMax) UnhookWinEvent(g_hEventHookMax);
        if (g_hEventHookPos) UnhookWinEvent(g_hEventHookPos);

        KillTimer(g_hWnd, 1);
        DestroyWindow(g_hWnd);
    }

    GdiplusShutdown(g_gdiToken);
    UnregisterClass(L"DiceCustomizableOverlay", GetModuleHandle(NULL));
    return 0;
}

void LoadSettings() {
    PCWSTR pos = Wh_GetStringSetting(L"position");
    if (pos) {
        g_settings.position = pos;
        Wh_FreeStringSetting(pos);
    }

    g_settings.diceSize = Wh_GetIntSetting(L"diceSize");

    PCWSTR style = Wh_GetStringSetting(L"diceStyle");
    if (style) {
        g_settings.diceStyle = style;
        Wh_FreeStringSetting(style);
    }

    g_settings.randomOnClick = Wh_GetIntSetting(L"randomOnClick") != 0;
    g_settings.leftDiceValue = Wh_GetIntSetting(L"leftDiceValue");
    g_settings.rightDiceValue = Wh_GetIntSetting(L"rightDiceValue");
    g_settings.hideOnFullscreen = Wh_GetIntSetting(L"hideOnFullscreen") != 0;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    ApplySettings();
}

BOOL Wh_ModInit() {
    LoadSettings();
    g_running = true;
    g_hThread = CreateThread(NULL, 0, StartThread, NULL, 0, NULL);
    return TRUE;
}

void Wh_ModUninit() {
    g_running = false;
    if (g_hWnd) {
        PostMessage(g_hWnd, WM_CLOSE, 0, 0);
    }
    if (g_hThread) {
        WaitForSingleObject(g_hThread, 1000);
        CloseHandle(g_hThread);
    }
}