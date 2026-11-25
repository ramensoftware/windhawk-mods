// ==WindhawkMod==
// @id              radial-alt-tab
// @name            Radial Alt+Tab
// @description     Replaces the Alt+Tab switcher with a radial wheel of app icons and a large live preview, steered with the mouse like a joystick
// @version         1.1
// @author          khorkovenko
// @github          https://github.com/khorkovenko
// @include         windhawk.exe
// @compilerOptions -ldwmapi -lshcore -lgdi32 -lgdiplus -lshell32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Radial Alt+Tab
Replaces the Alt+Tab switcher with a full-screen view split into two halves:

- **Top half:** a large live preview of the selected window, with its number
  and full title, so two windows of the same app are easy to tell apart.
- **Bottom half:** a wheel with the icon and number of each open window. A
  pointer in the center shows where the mouse joystick points.

## Usage
- Hold **Alt** and press **Tab**. If you release Alt quickly, you switch to the
  previous window without seeing the wheel, like the default Alt+Tab.
- Keep holding Alt and the view opens. While it's open, the mouse cursor is
  hidden and doesn't move. Mouse movement steers the pointer: push the mouse
  toward a window to select it.
- Release **Alt** to switch to the selected window. No click is needed.
- Tab / Shift+Tab, the arrow keys and the mouse wheel also change the selection.
- Left click switches right away. Right click closes the selected window; if
  it has unsaved changes, the app asks first. **Esc** cancels.

The view opens and closes instantly, without animations.

## Limitations
When a program running as administrator is in the foreground, Windows may not
pass the keyboard to this mod, and the default Alt+Tab appears instead.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- openDelay: 150
  $name: Delay before the view opens (ms)
  $description: >-
    Releasing Alt before this delay switches to the previous window without
    showing the view, like the default Alt+Tab. 0 opens the view immediately.
- maxWindows: 12
  $name: Maximum number of windows on the wheel
*/
// ==/WindhawkModSettings==

#include <atomic>
#include <dwmapi.h>
#include <math.h>
#include <stdio.h>
#include <shellscalingapi.h>
#include <gdiplus.h>

constexpr int MAXWIN = 16;
constexpr float PI = 3.14159265f;
constexpr WORD VK_MASK = 0xE8;
constexpr UINT WM_APP_START = WM_APP + 1;
constexpr UINT WM_APP_STEP = WM_APP + 2;
constexpr UINT WM_APP_END = WM_APP + 3;
constexpr UINT WM_APP_JOY = WM_APP + 4;
constexpr UINT WM_APP_MOUSEHOOK = WM_APP + 5;

struct {
    int delay;
    int maxWindows;
} settings;

HANDLE g_thread, g_hookThread;
DWORD g_threadId, g_hookThreadId;
HINSTANCE g_inst;
HWND g_wnd;
std::atomic<bool> g_session, g_open;
int g_count, g_sel, g_dpi = 96;
HWND g_list[MAXWIN];
HICON g_icons[MAXWIN];
HTHUMBNAIL g_thumb;
RECT g_mon, g_preview;
POINT g_center, g_savedPos;
float g_ro, g_ri, g_wx, g_wy, g_jx, g_jy, g_lastAngle;

float Sc(float v) {
    return v * g_dpi / 96.f;
}

void SendKey(WORD vk, bool up) {
    INPUT in{};
    in.type = INPUT_KEYBOARD;
    in.ki.wVk = vk;
    in.ki.dwFlags = up ? KEYEVENTF_KEYUP : 0;
    SendInput(1, &in, sizeof(INPUT));
}

BOOL CALLBACK EnumProc(HWND hwnd, LPARAM) {
    WCHAR cls[32] = L"";
    DWORD cloaked = 0;
    LONG_PTR ex = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    GetClassName(hwnd, cls, 32);
    DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
    if (hwnd == g_wnd || !IsWindowVisible(hwnd) || cloaked || !GetWindowTextLength(hwnd) ||
        (!(ex & WS_EX_APPWINDOW) && (GetWindow(hwnd, GW_OWNER) || (ex & WS_EX_TOOLWINDOW))) ||
        !lstrcmpW(cls, L"Progman"))
        return TRUE;
    DWORD_PTR icon = 0;
    SendMessageTimeout(hwnd, WM_GETICON, ICON_BIG, 0, SMTO_ABORTIFHUNG, 20, &icon);
    if (!icon)
        icon = GetClassLongPtr(hwnd, GCLP_HICON);
    g_icons[g_count] = (HICON)icon;
    g_list[g_count++] = hwnd;
    return g_count < settings.maxWindows;
}

void Activate(HWND hwnd) {
    if (IsIconic(hwnd))
        ShowWindowAsync(hwnd, SW_RESTORE);
    if (SetForegroundWindow(hwnd))
        return;
    SendKey(VK_MENU, false);
    SetForegroundWindow(hwnd);
    SendKey(VK_MASK, false);
    SendKey(VK_MASK, true);
    SendKey(VK_MENU, true);
}

void UpdatePreview() {
    if (g_thumb)
        DwmUnregisterThumbnail(g_thumb);
    g_thumb = nullptr;
    int w = g_mon.right - g_mon.left, h = g_mon.bottom - g_mon.top;
    float m = Sc(40), titleH = Sc(48);
    float bx = m, by = m + titleH, bw = w - 2 * m, bh = h / 2.f - by - Sc(24);
    float tw = bw, th = bh;
    if (SUCCEEDED(DwmRegisterThumbnail(g_wnd, g_list[g_sel], &g_thumb))) {
        SIZE s{};
        DwmQueryThumbnailSourceSize(g_thumb, &s);
        if (s.cx > 0 && s.cy > 0) {
            if (tw * s.cy > th * s.cx)
                tw = th * s.cx / s.cy;
            else
                th = tw * s.cy / s.cx;
        }
    } else {
        g_thumb = nullptr;
    }
    g_preview = {(LONG)(bx + (bw - tw) / 2), (LONG)by, (LONG)(bx + (bw + tw) / 2), (LONG)(by + th)};
    if (!g_thumb)
        return;
    DWM_THUMBNAIL_PROPERTIES tp{};
    tp.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE;
    tp.rcDestination = g_preview;
    tp.fVisible = TRUE;
    DwmUpdateThumbnailProperties(g_thumb, &tp);
}

void Select(int sel) {
    if (sel == g_sel)
        return;
    g_sel = sel;
    if (g_open)
        UpdatePreview();
    InvalidateRect(g_wnd, nullptr, FALSE);
}

void Open() {
    GetCursorPos(&g_savedPos);
    HMONITOR mon = MonitorFromPoint(g_savedPos, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{sizeof(mi)};
    GetMonitorInfo(mon, &mi);
    UINT dx, dy;
    g_dpi = SUCCEEDED(GetDpiForMonitor(mon, MDT_EFFECTIVE_DPI, &dx, &dy)) ? dx : 96;
    g_mon = mi.rcMonitor;
    int w = g_mon.right - g_mon.left, h = g_mon.bottom - g_mon.top;
    g_center = {g_mon.left + w / 2, g_mon.top + h / 2};
    g_ro = fminf(w, h / 2.f) * 0.36f;
    g_ri = g_ro * 0.42f;
    g_wx = w / 2.f;
    g_wy = h * 0.75f;
    g_jx = g_jy = 0;
    g_lastAngle = 1000;

    SetWindowPos(g_wnd, HWND_TOPMOST, g_mon.left, g_mon.top, w, h, SWP_NOACTIVATE);
    g_open = true;
    UpdatePreview();
    ShowWindow(g_wnd, SW_SHOWNOACTIVATE);
    UpdateWindow(g_wnd);
    SetCursorPos(g_center.x, g_center.y);
    SetCursor(nullptr);
    PostThreadMessage(g_hookThreadId, WM_APP_MOUSEHOOK, 1, 0);
}

void Close() {
    if (!g_open)
        return;
    g_open = false;
    PostThreadMessage(g_hookThreadId, WM_APP_MOUSEHOOK, 0, 0);
    if (g_thumb)
        DwmUnregisterThumbnail(g_thumb);
    g_thumb = nullptr;
    ShowWindow(g_wnd, SW_HIDE);
    SetCursorPos(g_savedPos.x, g_savedPos.y);
}

void Paint(HDC dc, int w, int h) {
    using namespace Gdiplus;
    float span = 360.f / (g_count ? g_count : 1), rm = (g_ri + g_ro) / 2, rh = g_ri - Sc(14);
    {
        Graphics g(dc);
        g.SetSmoothingMode(SmoothingModeAntiAlias);
        g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
        SolidBrush top(Color(255, 22, 22, 22)), bottom(Color(255, 16, 16, 16));
        g.FillRectangle(&top, 0, 0, w, h / 2);
        g.FillRectangle(&bottom, 0, h / 2, w, h - h / 2);
        Pen divider(Color(255, 54, 54, 54), Sc(1));
        g.DrawLine(&divider, 0.f, h / 2.f, (float)w, h / 2.f);
        if (!g_count)
            return;

        SolidBrush white(Color(255, 255, 255, 255)), dim(Color(255, 190, 190, 190));
        Color accent(255, 0, 150, 255);
        StringFormat center;
        center.SetAlignment(StringAlignmentCenter);
        center.SetLineAlignment(StringAlignmentCenter);
        center.SetTrimming(StringTrimmingEllipsisCharacter);
        center.SetFormatFlags(StringFormatFlagsNoWrap);

        WCHAR title[300], text[320];
        GetWindowText(g_list[g_sel], title, 300);
        swprintf_s(text, L"%d  \u00B7  %s", g_sel + 1, title);
        Font titleFont(L"Segoe UI Semibold", Sc(20), FontStyleRegular, UnitPixel);
        g.DrawString(text, -1, &titleFont, RectF(Sc(40), Sc(40), w - Sc(80), Sc(40)), &center,
                     &white);
        Pen frame(accent, Sc(3));
        g.DrawRectangle(&frame, g_preview.left - Sc(4), g_preview.top - Sc(4),
                        g_preview.right - g_preview.left + Sc(8),
                        g_preview.bottom - g_preview.top + Sc(8));

        RectF outer(g_wx - g_ro, g_wy - g_ro, 2 * g_ro, 2 * g_ro);
        RectF inner(g_wx - g_ri, g_wy - g_ri, 2 * g_ri, 2 * g_ri);
        float gap = Sc(3), go = gap / g_ro * 180 / PI, gi = gap / g_ri * 180 / PI;
        SolidBrush slice(Color(255, 38, 38, 38)), selFill(Color(255, 74, 74, 74));
        SolidBrush hub(Color(255, 28, 28, 28));
        Pen border(Color(255, 60, 60, 60), Sc(1)), selPen(Color(255, 255, 255, 255), Sc(2));
        Font numFont(L"Segoe UI", Sc(12), FontStyleRegular, UnitPixel);

        for (int i = 0; i < g_count; i++) {
            GraphicsPath p;
            float a0 = -90 + i * span - span / 2;
            if (g_count == 1) {
                p.AddEllipse(outer);
                p.AddEllipse(inner);
            } else {
                p.AddArc(outer, a0 + go, span - 2 * go);
                p.AddArc(inner, a0 + span - gi, -(span - 2 * gi));
                p.CloseFigure();
            }
            g.FillPath(i == g_sel ? &selFill : &slice, &p);
            g.DrawPath(i == g_sel ? &selPen : &border, &p);

            float a = (a0 + span / 2) * PI / 180;
            WCHAR num[8];
            swprintf_s(num, L"%d", i + 1);
            g.DrawString(num, -1, &numFont,
                         RectF(g_wx + rm * cosf(a) - Sc(20), g_wy + rm * sinf(a) + Sc(14), Sc(40),
                               Sc(16)),
                         &center, i == g_sel ? &white : &dim);
        }

        g.FillEllipse(&hub, g_wx - rh, g_wy - rh, 2 * rh, 2 * rh);
        g.DrawEllipse(&border, g_wx - rh, g_wy - rh, 2 * rh, 2 * rh);
        Pen arc(accent, Sc(5));
        arc.SetStartCap(LineCapRound);
        arc.SetEndCap(LineCapRound);
        float ra = rh - Sc(5);
        g.DrawArc(&arc, g_wx - ra, g_wy - ra, 2 * ra, 2 * ra, -90 + g_sel * span - span / 2 + 4,
                  span - 8 > 1 ? span - 8 : 1);

        if (g_jx * g_jx + g_jy * g_jy >= Sc(20) * Sc(20)) {
            float a = atan2f(g_jy, g_jx), c = cosf(a), s = sinf(a);
            float tip = g_ri - Sc(1), base = rh - Sc(14), half = Sc(11);
            PointF pts[3] = {
                PointF(g_wx + tip * c, g_wy + tip * s),
                PointF(g_wx + base * c - half * s, g_wy + base * s + half * c),
                PointF(g_wx + base * c + half * s, g_wy + base * s - half * c),
            };
            SolidBrush pointer(accent);
            Pen outline(Color(255, 255, 255, 255), Sc(1.5f));
            g.FillPolygon(&pointer, pts, 3);
            g.DrawPolygon(&outline, pts, 3);
        }
    }

    if (!g_count)
        return;
    int isz = (int)Sc(32);
    for (int i = 0; i < g_count; i++) {
        float a = (-90 + i * span) * PI / 180;
        if (g_icons[i])
            DrawIconEx(dc, (int)(g_wx + rm * cosf(a)) - isz / 2,
                       (int)(g_wy + rm * sinf(a)) - isz / 2 - (int)Sc(4), g_icons[i], isz, isz, 0,
                       nullptr, DI_NORMAL);
    }
    int hsz = (int)Sc(44);
    if (g_icons[g_sel])
        DrawIconEx(dc, (int)g_wx - hsz / 2, (int)g_wy - hsz / 2, g_icons[g_sel], hsz, hsz, 0,
                   nullptr, DI_NORMAL);
}

LRESULT CALLBACK KeyboardProc(int code, WPARAM wp, LPARAM lp) {
    auto* k = (KBDLLHOOKSTRUCT*)lp;
    if (code != HC_ACTION || (k->flags & LLKHF_INJECTED))
        return CallNextHookEx(nullptr, code, wp, lp);

    bool down = wp == WM_KEYDOWN || wp == WM_SYSKEYDOWN;
    DWORD vk = k->vkCode;
    if (vk == VK_TAB && ((k->flags & LLKHF_ALTDOWN) || g_session)) {
        bool shift = GetAsyncKeyState(VK_SHIFT) < 0;
        if (down && !g_session) {
            g_session = true;
            PostMessage(g_wnd, WM_APP_START, shift, 0);
        } else if (down) {
            PostMessage(g_wnd, WM_APP_STEP, shift ? -1 : 1, 0);
        }
        return 1;
    }
    if (!g_session)
        return CallNextHookEx(nullptr, code, wp, lp);

    if (!down && (vk == VK_LMENU || vk == VK_RMENU)) {
        g_session = false;
        PostMessage(g_wnd, WM_APP_END, 1, 0);
    } else if (down && vk == VK_ESCAPE) {
        g_session = false;
        PostMessage(g_wnd, WM_APP_END, 0, 0);
        return 1;
    } else if (down && (vk == VK_LEFT || vk == VK_UP || vk == VK_RIGHT || vk == VK_DOWN)) {
        PostMessage(g_wnd, WM_APP_STEP, vk == VK_LEFT || vk == VK_UP ? -1 : 1, 0);
        return 1;
    }
    return CallNextHookEx(nullptr, code, wp, lp);
}

LRESULT CALLBACK MouseProc(int code, WPARAM wp, LPARAM lp) {
    if (code != HC_ACTION || !g_open)
        return CallNextHookEx(nullptr, code, wp, lp);
    auto* m = (MSLLHOOKSTRUCT*)lp;
    int dx = m->pt.x - g_center.x, dy = m->pt.y - g_center.y;
    if (wp == WM_MOUSEMOVE && !dx && !dy)
        return CallNextHookEx(nullptr, code, wp, lp);
    if (wp == WM_MOUSEMOVE) {
        PostMessage(g_wnd, WM_APP_JOY, (WPARAM)dx, (LPARAM)dy);
    } else if (wp == WM_LBUTTONDOWN || wp == WM_RBUTTONDOWN) {
        g_session = false;
        PostMessage(g_wnd, WM_APP_END, wp == WM_LBUTTONDOWN ? 1 : 2, 0);
    } else if (wp == WM_MOUSEWHEEL) {
        PostMessage(g_wnd, WM_APP_STEP, (short)HIWORD(m->mouseData) > 0 ? -1 : 1, 0);
    }
    return 1;
}

DWORD WINAPI HookThreadProc(LPVOID) {
    HHOOK kb = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, g_inst, 0), mouse = nullptr;
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        if (msg.message != WM_APP_MOUSEHOOK)
            continue;
        if (msg.wParam && !mouse) {
            mouse = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, g_inst, 0);
        } else if (!msg.wParam && mouse) {
            UnhookWindowsHookEx(mouse);
            mouse = nullptr;
        }
    }
    if (mouse)
        UnhookWindowsHookEx(mouse);
    UnhookWindowsHookEx(kb);
    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_APP_START:
            SendKey(VK_MASK, false);
            SendKey(VK_MASK, true);
            g_count = 0;
            EnumWindows(EnumProc, 0);
            if (!g_count) {
                g_session = false;
                return 0;
            }
            g_sel = g_count < 2 ? 0 : wp ? g_count - 1 : 1;
            if (settings.delay > 0)
                SetTimer(hwnd, 1, settings.delay, nullptr);
            else
                Open();
            return 0;
        case WM_TIMER:
            KillTimer(hwnd, 1);
            if (g_session)
                Open();
            return 0;
        case WM_APP_STEP:
            if (g_count)
                Select((g_sel + (int)(INT_PTR)wp + g_count) % g_count);
            return 0;
        case WM_APP_JOY: {
            if (!g_open)
                return 0;
            g_jx += (int)(INT_PTR)wp;
            g_jy += (int)(INT_PTR)lp;
            float len = sqrtf(g_jx * g_jx + g_jy * g_jy), r = Sc(80);
            if (len > r) {
                g_jx *= r / len;
                g_jy *= r / len;
            }
            if (len < Sc(20))
                return 0;
            float deg = atan2f(g_jy, g_jx) * 180 / PI;
            Select(((int)lroundf((deg + 90) / (360.f / g_count)) % g_count + g_count) % g_count);
            if (fabsf(deg - g_lastAngle) > 2) {
                g_lastAngle = deg;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        }
        case WM_APP_END:
            KillTimer(hwnd, 1);
            Close();
            if (wp && g_sel < g_count) {
                Activate(g_list[g_sel]);
                if (wp == 2)
                    PostMessage(g_list[g_sel], WM_CLOSE, 0, 0);
            }
            g_count = 0;
            return 0;
        case WM_SETCURSOR:
            SetCursor(nullptr);
            return TRUE;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC dc = BeginPaint(hwnd, &ps);
            RECT rc;
            GetClientRect(hwnd, &rc);
            HDC mem = CreateCompatibleDC(dc);
            HBITMAP bmp = CreateCompatibleBitmap(dc, rc.right, rc.bottom);
            HGDIOBJ old = SelectObject(mem, bmp);
            Paint(mem, rc.right, rc.bottom);
            BitBlt(dc, 0, 0, rc.right, rc.bottom, mem, 0, 0, SRCCOPY);
            SelectObject(mem, old);
            DeleteObject(bmp);
            DeleteDC(mem);
            EndPaint(hwnd, &ps);
            return 0;
        }
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

DWORD WINAPI ThreadProc(LPVOID) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartupInput gsi;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gsi, nullptr);
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                          GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      (LPCWSTR)ThreadProc, &g_inst);

    WNDCLASS wc{0, WndProc, 0, 0, g_inst, nullptr, nullptr, nullptr, nullptr, L"RadialAltTab"};
    RegisterClass(&wc);
    g_wnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, wc.lpszClassName,
                           nullptr, WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, g_inst, nullptr);
    BOOL noAnim = TRUE;
    DwmSetWindowAttribute(g_wnd, DWMWA_TRANSITIONS_FORCEDISABLED, &noAnim, sizeof(noAnim));
    g_hookThread = CreateThread(nullptr, 0, HookThreadProc, nullptr, 0, &g_hookThreadId);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) > 0)
        DispatchMessage(&msg);

    Close();
    while (!PostThreadMessage(g_hookThreadId, WM_QUIT, 0, 0) &&
           WaitForSingleObject(g_hookThread, 10) == WAIT_TIMEOUT) {
    }
    WaitForSingleObject(g_hookThread, INFINITE);
    CloseHandle(g_hookThread);
    DestroyWindow(g_wnd);
    UnregisterClass(wc.lpszClassName, g_inst);
    Gdiplus::GdiplusShutdown(gdiplusToken);
    return 0;
}

void LoadSettings() {
    settings.delay = Wh_GetIntSetting(L"openDelay");
    if (settings.delay < 0)
        settings.delay = 0;
    settings.maxWindows = Wh_GetIntSetting(L"maxWindows");
    if (settings.maxWindows < 2 || settings.maxWindows > MAXWIN)
        settings.maxWindows = 12;
}

BOOL WhTool_ModInit() {
    LoadSettings();
    g_thread = CreateThread(nullptr, 0, ThreadProc, nullptr, 0, &g_threadId);
    return g_thread != nullptr;
}

void WhTool_ModUninit() {
    while (!PostThreadMessage(g_threadId, WM_QUIT, 0, 0) &&
           WaitForSingleObject(g_thread, 10) == WAIT_TIMEOUT) {
    }
    WaitForSingleObject(g_thread, INFINITE);
    CloseHandle(g_thread);
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
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
