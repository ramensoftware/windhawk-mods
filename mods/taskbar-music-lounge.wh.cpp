// ==WindhawkMod==
// @id              taskbar-music-lounge
// @name            Taskbar Music Lounge
// @description     A "Pill-shaped" scrolling music ticker with media controls.
// @version         1.0
// @author          Hashah2311
// @github          https://github.com/Hashah2311
// @include         explorer.exe
// @compilerOptions -lole32 -ldwmapi -lgdi32 -luser32 -lgdiplus
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Music Lounge

A sleek, pill-shaped music ticker for your Windows 11 taskbar. It sits unobtrusively in the corner, scrolling your current track and providing quick media controls.

## 🎵 Features
* **Aesthetics:** Modern "Acrylic" glass effect that blends with Windows 11 design.
* **Marquee Text:** Automatically scrolls long song titles.
* **Controls:** Dedicated **Prev / Play / Next** buttons on the widget itself.
* **Volume:** Hover over the widget and **scroll your mouse wheel** to adjust system volume.
* **Compatibility:** Supports Spotify, YouTube Music (PWA & Desktop Wrappers), and most players that update window titles.

## ⚠️ Important Requirement
This mod occupies the **left corner** of the taskbar. To prevent overlap, you must disable the Windows "Widgets" (Weather) icon:
1. Right-click your Taskbar.
2. Select **Taskbar Settings**.
3. Toggle **Widgets** to **Off**.

## ⚙️ Customization
Go to the **Settings** tab in Windhawk to adjust:
* **Panel Width/Height:** Make it wider or slimmer.
* **Font Size:** Adjust text readability.
* **Offsets:** Fine-tune the position (X/Y) to align perfectly with your Start button or screen edge.

## 🐛 Troubleshooting
* **Text not showing?** Ensure your music player is running and not fully minimized to the tray (some apps stop broadcasting titles when hidden).
* **Overlapping Start Button?** If you use Left-Aligned taskbar icons, increase the `X Offset` in settings to push the widget to the right of the Start button.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- PanelWidth: 260
  $name: Panel Width
  $description: Width of the pill widget in pixels (min 150, max 600).
- PanelHeight: 40
  $name: Panel Height
  $description: Height of the pill widget in pixels (min 24, max 60).
- FontSize: 11
  $name: Font Size
  $description: Size of the scrolling text font.
- OffsetX: 12
  $name: X Offset
  $description: Distance from the left edge of the screen/taskbar.
- OffsetY: 0
  $name: Y Offset
  $description: Vertical adjustment (positive moves down, negative moves up).
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <gdiplus.h>
#include <string>
#include <vector>
#include <atomic>
#include <thread>
#include <algorithm>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "ole32.lib")

using namespace Gdiplus;
using namespace std;

// --- Undocumented Acrylic API ---
typedef enum _WINDOWCOMPOSITIONATTRIB { WCA_ACCENT_POLICY = 19 } WINDOWCOMPOSITIONATTRIB;
typedef enum _ACCENT_STATE {
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_GRADIENT = 1,
    ACCENT_ENABLE_TRANSPARENTGRADIENT = 2,
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
typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);

// --- Configurable State ---
struct ModSettings {
    int width = 260;
    int height = 40;
    int fontSize = 11;
    int offsetX = 12;
    int offsetY = 0;
} g_Settings;

// Constants
const WCHAR* FONT_NAME = L"Segoe UI Variable Display"; 
const DWORD ACRYLIC_COLOR = 0x01101010; 

// --- Global State ---
std::atomic<bool> g_Running(true);
HWND g_hMediaWindow = NULL;
wstring g_CurrentTrack = L"Waiting for music...";
int g_ScrollOffset = 0;
int g_TextWidth = 0;
bool g_IsScrolling = false;
int g_HoverState = 0; // 0=None, 1=Prev, 2=Play, 3=Next

// --- Helper: Load Settings ---
void LoadSettings() {
    g_Settings.width = Wh_GetIntSetting(L"PanelWidth");
    g_Settings.height = Wh_GetIntSetting(L"PanelHeight");
    g_Settings.fontSize = Wh_GetIntSetting(L"FontSize");
    g_Settings.offsetX = Wh_GetIntSetting(L"OffsetX");
    g_Settings.offsetY = Wh_GetIntSetting(L"OffsetY");
    
    // Safety clamps
    if (g_Settings.width < 50) g_Settings.width = 260;
    if (g_Settings.height < 10) g_Settings.height = 40;
}

// --- Helper: Window Enumeration ---
BOOL CALLBACK EnumMusicWindowsProc(HWND hwnd, LPARAM lParam) {
    int len = GetWindowTextLength(hwnd);
    if (len == 0) return TRUE;
    vector<WCHAR> buf(len + 1);
    GetWindowText(hwnd, &buf[0], len + 1);
    wstring title(&buf[0]);
    wstring* pResult = (wstring*)lParam;

    size_t ytmPos = title.find(L" - YouTube Music");
    if (ytmPos != wstring::npos) {
        *pResult = title.substr(0, ytmPos);
        return FALSE;
    }
    if (title.find(L"YouTube Music") != wstring::npos && title.find(L" - ") != wstring::npos) {
        *pResult = title;
        return FALSE;
    }
    if (title.find(L"Spotify") != wstring::npos && title.find(L" - ") != wstring::npos) {
        if (title.length() > 16) { 
            *pResult = title;
            return FALSE;
        }
    }
    return TRUE;
}

wstring GetNowPlayingText() {
    wstring foundTitle = L"";
    EnumWindows(EnumMusicWindowsProc, (LPARAM)&foundTitle);
    return foundTitle;
}

void EnableBlur(HWND hwnd) {
    HMODULE hUser = GetModuleHandle(L"user32.dll");
    if (hUser) {
        auto SetComp = (pSetWindowCompositionAttribute)GetProcAddress(hUser, "SetWindowCompositionAttribute");
        if (SetComp) {
            ACCENT_POLICY policy = { ACCENT_ENABLE_ACRYLICBLURBEHIND, 0, ACRYLIC_COLOR, 0 };
            WINDOWCOMPOSITIONATTRIBDATA data = { WCA_ACCENT_POLICY, &policy, sizeof(ACCENT_POLICY) };
            SetComp(hwnd, &data);
        }
    }
}

// --- Drawing ---
void DrawMediaPanel(HDC hdc, int width, int height) {
    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
    graphics.Clear(Color(0, 0, 0, 0)); 

    // Pill Background
    Rect rect(0, 0, width - 1, height - 1);
    GraphicsPath path;
    int arcSize = height; 
    path.AddArc(rect.X, rect.Y, arcSize, arcSize, 180, 90);
    path.AddArc(rect.X + rect.Width - arcSize, rect.Y, arcSize, arcSize, 270, 90);
    path.AddArc(rect.X + rect.Width - arcSize, rect.Y + rect.Height - arcSize, arcSize, arcSize, 0, 90);
    path.AddArc(rect.X, rect.Y + rect.Height - arcSize, arcSize, arcSize, 90, 90);
    path.CloseFigure();

    SolidBrush bgBrush(Color(20, 255, 255, 255));
    graphics.FillPath(&bgBrush, &path);
    Pen borderPen(Color(40, 255, 255, 255), 1);
    graphics.DrawPath(&borderPen, &path);

    // Controls
    SolidBrush normalBrush(Color(180, 255, 255, 255));
    SolidBrush hoverBrush(Color(255, 255, 255, 255));
    SolidBrush activeBg(Color(40, 255, 255, 255));

    int centerY = height / 2;

    // Prev
    if (g_HoverState == 1) graphics.FillEllipse(&activeBg, 12, centerY - 12, 24, 24);
    SolidBrush* pBrush = (g_HoverState == 1) ? &hoverBrush : &normalBrush;
    Point prevPts[3] = { Point(28, centerY - 6), Point(28, centerY + 6), Point(18, centerY) };
    graphics.FillPolygon(pBrush, prevPts, 3);
    graphics.FillRectangle(pBrush, 16, centerY - 6, 2, 12); 

    // Play
    if (g_HoverState == 2) graphics.FillEllipse(&activeBg, 38, centerY - 12, 24, 24);
    SolidBrush* plBrush = (g_HoverState == 2) ? &hoverBrush : &normalBrush;
    Point playPts[3] = { Point(46, centerY - 8), Point(46, centerY + 8), Point(58, centerY) };
    graphics.FillPolygon(plBrush, playPts, 3);

    // Next
    if (g_HoverState == 3) graphics.FillEllipse(&activeBg, 64, centerY - 12, 24, 24);
    SolidBrush* nBrush = (g_HoverState == 3) ? &hoverBrush : &normalBrush;
    Point nextPts[3] = { Point(72, centerY - 6), Point(72, centerY + 6), Point(82, centerY) };
    graphics.FillPolygon(nBrush, nextPts, 3);
    graphics.FillRectangle(nBrush, 82, centerY - 6, 2, 12); 

    // Separator
    Pen sepPen(Color(30, 255, 255, 255), 1);
    graphics.DrawLine(&sepPen, 95, 10, 95, height - 10);

    // Text
    FontFamily fontFamily(FONT_NAME);
    Font font(&fontFamily, (REAL)g_Settings.fontSize, FontStyleBold, UnitPixel);
    SolidBrush textBrush(Color(240, 255, 255, 255));
    
    RectF layoutRect(0, 0, 1000, 100);
    RectF boundRect;
    graphics.MeasureString(g_CurrentTrack.c_str(), -1, &font, layoutRect, &boundRect);
    g_TextWidth = (int)boundRect.Width;

    int textAreaWidth = width - 110; 
    int textX = 100; 

    // Clipping Intersect
    graphics.SetClip(&path); 
    Rect textClipRect(96, 0, width - 96, height);
    graphics.SetClip(textClipRect, CombineModeIntersect); 

    if (g_TextWidth > textAreaWidth) {
        g_IsScrolling = true;
        float drawX = (float)(textX - g_ScrollOffset);
        float textY = (height - boundRect.Height) / 2.0f;
        graphics.DrawString(g_CurrentTrack.c_str(), -1, &font, PointF(drawX, textY), &textBrush);
        if (drawX + g_TextWidth < width) {
             graphics.DrawString(g_CurrentTrack.c_str(), -1, &font, PointF(drawX + g_TextWidth + 40, textY), &textBrush);
        }
    } else {
        g_IsScrolling = false;
        g_ScrollOffset = 0;
        float drawX = textX + (textAreaWidth - g_TextWidth) / 2.0f;
        float textY = (height - boundRect.Height) / 2.0f;
        graphics.DrawString(g_CurrentTrack.c_str(), -1, &font, PointF(drawX, textY), &textBrush);
    }
}

LRESULT CALLBACK MediaWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: EnableBlur(hwnd); break;
        case WM_ERASEBKGND: return 1;

        case WM_MOUSEMOVE: {
            int x = LOWORD(lParam);
            int newState = 0;
            if (x >= 10 && x < 36) newState = 1; 
            else if (x >= 36 && x < 62) newState = 2; 
            else if (x >= 62 && x < 90) newState = 3; 
            
            if (newState != g_HoverState) {
                g_HoverState = newState;
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
        }
        case WM_MOUSELEAVE:
            if (g_HoverState != 0) {
                g_HoverState = 0;
                InvalidateRect(hwnd, NULL, FALSE);
            }
            break;
        case WM_LBUTTONUP: {
            int x = LOWORD(lParam);
            if (x >= 10 && x < 36) {
                keybd_event(VK_MEDIA_PREV_TRACK, 0, 0, 0);
                keybd_event(VK_MEDIA_PREV_TRACK, 0, KEYEVENTF_KEYUP, 0);
            }
            else if (x >= 36 && x < 62) {
                keybd_event(VK_MEDIA_PLAY_PAUSE, 0, 0, 0);
                keybd_event(VK_MEDIA_PLAY_PAUSE, 0, KEYEVENTF_KEYUP, 0);
            }
            else if (x >= 62 && x < 90) {
                keybd_event(VK_MEDIA_NEXT_TRACK, 0, 0, 0);
                keybd_event(VK_MEDIA_NEXT_TRACK, 0, KEYEVENTF_KEYUP, 0);
            }
            return 0;
        }
        case WM_MOUSEWHEEL: {
            short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
            if (zDelta > 0) {
                keybd_event(VK_VOLUME_UP, 0, 0, 0);
                keybd_event(VK_VOLUME_UP, 0, KEYEVENTF_KEYUP, 0);
            } else {
                keybd_event(VK_VOLUME_DOWN, 0, 0, 0);
                keybd_event(VK_VOLUME_DOWN, 0, KEYEVENTF_KEYUP, 0);
            }
            return 0;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc; GetClientRect(hwnd, &rc);
            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP memBitmap = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
            HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);
            DrawMediaPanel(memDC, rc.right, rc.bottom);
            BitBlt(hdc, 0, 0, rc.right, rc.bottom, memDC, 0, 0, SRCCOPY);
            SelectObject(memDC, oldBitmap); DeleteObject(memBitmap); DeleteDC(memDC);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_NCHITTEST: return HTCLIENT;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void MediaThread() {
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    WNDCLASS wc = {0};
    wc.lpfnWndProc = MediaWndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = TEXT("WindhawkMusicLounge");
    wc.hCursor = LoadCursor(NULL, IDC_HAND);
    RegisterClass(&wc);

    g_hMediaWindow = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
        wc.lpszClassName, TEXT("MusicLounge"),
        WS_POPUP | WS_VISIBLE,
        0, 0, g_Settings.width, g_Settings.height,
        NULL, NULL, wc.hInstance, NULL
    );

    SetLayeredWindowAttributes(g_hMediaWindow, 0, 255, LWA_ALPHA);
    
    TRACKMOUSEEVENT tme;
    tme.cbSize = sizeof(TRACKMOUSEEVENT);
    tme.dwFlags = TME_LEAVE;
    tme.hwndTrack = g_hMediaWindow;
    TrackMouseEvent(&tme);

    HWND hTaskbar = FindWindow(TEXT("Shell_TrayWnd"), NULL);
    int frameCount = 0;
    int scrollWait = 0;
    
    // Previous state to prevent ghosting (only update if changed)
    RECT prevRect = {0};

    while (g_Running) {
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg); DispatchMessage(&msg);
        }

        if (!IsWindow(hTaskbar)) hTaskbar = FindWindow(TEXT("Shell_TrayWnd"), NULL);

        if (hTaskbar) {
            RECT rc;
            GetWindowRect(hTaskbar, &rc);
            int x = rc.left + g_Settings.offsetX; 
            int taskbarHeight = rc.bottom - rc.top;
            int y = rc.top + (taskbarHeight / 2) - (g_Settings.height / 2) + g_Settings.offsetY;

            // FIX 1: Geometry Logic
            // Only full repaint/resize if the coordinates actually change.
            if (x != prevRect.left || y != prevRect.top || 
                g_Settings.width != (prevRect.right - prevRect.left) || 
                g_Settings.height != (prevRect.bottom - prevRect.top)) {
                
                SetWindowPos(g_hMediaWindow, HWND_TOPMOST, x, y, g_Settings.width, g_Settings.height, SWP_NOACTIVATE);
                
                // Update our tracker
                prevRect.left = x;
                prevRect.top = y;
                prevRect.right = x + g_Settings.width;
                prevRect.bottom = y + g_Settings.height;
                
                // Force a full clean redraw when size changes
                InvalidateRect(g_hMediaWindow, NULL, TRUE);
            } 
            // FIX 2: Survival Logic
            // Every ~1 second (60 frames), gently re-assert TopMost status
            // WITHOUT triggering a move/resize redraw (SWP_NOMOVE | SWP_NOSIZE)
            else if (frameCount % 60 == 0) {
                 SetWindowPos(g_hMediaWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            }
        }

        TrackMouseEvent(&tme);

        if (frameCount % 30 == 0) {
            wstring newTrack = GetNowPlayingText();
            if (newTrack.empty()) newTrack = L"No Music Detected";
            if (newTrack != g_CurrentTrack) {
                g_CurrentTrack = newTrack;
                g_ScrollOffset = 0; 
                scrollWait = 60; 
                InvalidateRect(g_hMediaWindow, NULL, FALSE);
            }
        }

        if (g_IsScrolling) {
            if (scrollWait > 0) {
                scrollWait--;
            } else {
                g_ScrollOffset++;
                if (g_ScrollOffset > g_TextWidth + 40) {
                    g_ScrollOffset = 0;
                    scrollWait = 60; 
                }
                InvalidateRect(g_hMediaWindow, NULL, FALSE);
            }
        }
        frameCount++;
        Sleep(16); 
    }

    DestroyWindow(g_hMediaWindow);
    UnregisterClass(wc.lpszClassName, wc.hInstance);
    GdiplusShutdown(gdiplusToken);
}

std::thread* g_pMediaThread = nullptr;

BOOL Wh_ModInit() {
    LoadSettings(); // Load on init
    g_Running = true;
    g_pMediaThread = new std::thread(MediaThread);
    return TRUE;
}

void Wh_ModUninit() {
    g_Running = false;
    if (g_pMediaThread) {
        g_pMediaThread->join();
        delete g_pMediaThread;
        g_pMediaThread = nullptr;
    }
}

// Hook for settings change
void Wh_ModSettingsChanged() {
    LoadSettings();
}