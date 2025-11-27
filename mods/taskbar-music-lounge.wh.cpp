// ==WindhawkMod==
// @id              taskbar-music-lounge
// @name            Taskbar Music Lounge
// @description     A "Pill-shaped" scrolling music ticker with media controls.
// @version         2.0
// @author          Hashah2311
// @github          https://github.com/Hashah2311
// @include         windhawk.exe
// @compilerOptions -lole32 -ldwmapi -lgdi32 -luser32 -lgdiplus -lshell32 -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Music Lounge (Refined Glass)

A sleek, pill-shaped music ticker for your Windows 11 taskbar.

## 🎵 Features
* **Refined Visuals:** Improved text rendering (smoother edges) and tighter window shaping to reduce artifacts.
* **Auto-Theming:** Automatically adapts to Windows Light/Dark mode.
* **Marquee Text:** Automatically scrolls long song titles.
* **Controls:** Dedicated **Prev / Play / Next** buttons.
* **Volume:** Scroll mouse wheel over the widget to adjust volume.
* **Architecture:** Runs as a standalone process (Safe Mode).

## ⚠️ Important Requirement
This mod occupies the **left corner** of the taskbar. To prevent overlap, you must disable the Windows "Widgets" (Weather) icon:
1. Right-click your Taskbar.
2. Select **Taskbar Settings**.
3. Toggle **Widgets** to **Off**.

## ⚙️ Customization
Go to the **Settings** tab in Windhawk to adjust:
* **Panel Width/Height:** Make it wider or slimmer.
* **Font Size:** Adjust text readability.
* **Offsets:** Fine-tune X/Y offsets.
* **Manual Colors:** You can disable "Auto Theme" to enforce specific colors.

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
- AutoTheme: true
  $name: Auto Theme
  $description: Automatically switch colors based on Windows System Theme (Overrides manual colors).
- TextColor: 0xFFFFFF
  $name: Manual Text Color (Hex)
  $description: Used only if Auto Theme is OFF.
- BgColor: 0x000000
  $name: Manual Background Tint (Hex)
  $description: Used only if Auto Theme is OFF.
- BgOpacity: 10
  $name: Manual Background Opacity (0-255)
  $description: Used only if Auto Theme is OFF. Lower = More Translucent.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <dwmapi.h>
#include <gdiplus.h>
#include <string>
#include <vector>
#include <atomic>
#include <thread>
#include <algorithm>
#include <cstdio>

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

// --- Constants & IDs ---
#define IDT_CHECK_MUSIC 1001
#define IDT_SCROLL      1002
#define IDT_ZORDER      1003

const WCHAR* FONT_NAME = L"Segoe UI Variable Display"; 

// --- Configurable State ---
struct ModSettings {
    int width = 260;
    int height = 40;
    int fontSize = 11;
    int offsetX = 12;
    int offsetY = 0;
    bool autoTheme = true;
    DWORD manualTextColor = 0xFFFFFFFF; 
    DWORD manualBgColor = 0x0A000000;   
} g_Settings;

// --- Global State ---
HWND g_hMediaWindow = NULL;
wstring g_CurrentTrack = L"Waiting for music...";
int g_ScrollOffset = 0;
int g_TextWidth = 0;
bool g_IsScrolling = false;
int g_HoverState = 0; 
int g_ScrollWait = 0;
bool g_Running = true; 

// --- Helper: System Theme Detection ---
bool IsSystemLightMode() {
    DWORD value = 0;
    DWORD size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", L"AppsUseLightTheme", RRF_RT_DWORD, nullptr, &value, &size) == ERROR_SUCCESS) {
        return value != 0;
    }
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", L"SystemUsesLightTheme", RRF_RT_DWORD, nullptr, &value, &size) == ERROR_SUCCESS) {
        return value != 0;
    }
    return false; 
}

// --- Helper: Load Settings ---
void LoadSettings() {
    g_Settings.width = Wh_GetIntSetting(L"PanelWidth");
    g_Settings.height = Wh_GetIntSetting(L"PanelHeight");
    g_Settings.fontSize = Wh_GetIntSetting(L"FontSize");
    g_Settings.offsetX = Wh_GetIntSetting(L"OffsetX");
    g_Settings.offsetY = Wh_GetIntSetting(L"OffsetY");
    g_Settings.autoTheme = Wh_GetIntSetting(L"AutoTheme") != 0;
    
    PCWSTR textHex = Wh_GetStringSetting(L"TextColor");
    DWORD textRGB = 0xFFFFFF;
    if (textHex && wcslen(textHex) > 0) textRGB = wcstoul(textHex, nullptr, 16);
    g_Settings.manualTextColor = 0xFF000000 | textRGB;

    PCWSTR bgHex = Wh_GetStringSetting(L"BgColor");
    DWORD bgRGB = 0x000000;
    if (bgHex && wcslen(bgHex) > 0) bgRGB = wcstoul(bgHex, nullptr, 16);
    int opacity = Wh_GetIntSetting(L"BgOpacity");
    if (opacity < 0) opacity = 0;
    if (opacity > 255) opacity = 255;
    
    g_Settings.manualBgColor = (opacity << 24) | (bgRGB & 0xFFFFFF);

    if (g_Settings.width < 50) g_Settings.width = 260;
    if (g_Settings.height < 10) g_Settings.height = 40;
}

// --- Helper: Window Enumeration ---
BOOL CALLBACK EnumMusicWindowsProc(HWND hwnd, LPARAM lParam) {
    if (!IsWindowVisible(hwnd)) { } 

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

// --- Color Logic ---
DWORD GetCurrentBgColor() {
    if (g_Settings.autoTheme) {
        // Use very low opacity (0x05 = 5) for nearly-clear glassy look
        if (IsSystemLightMode()) return 0x05FFFFFF; 
        else return 0x05000000; 
    }
    return g_Settings.manualBgColor;
}

DWORD GetCurrentTextColor() {
    if (g_Settings.autoTheme) {
        if (IsSystemLightMode()) return 0xFF000000; 
        else return 0xFFFFFFFF; 
    }
    return g_Settings.manualTextColor;
}

void UpdateAcrylic(HWND hwnd) {
    HMODULE hUser = GetModuleHandle(L"user32.dll");
    if (hUser) {
        auto SetComp = (pSetWindowCompositionAttribute)GetProcAddress(hUser, "SetWindowCompositionAttribute");
        if (SetComp) {
            ACCENT_POLICY policy = { ACCENT_ENABLE_ACRYLICBLURBEHIND, 0, GetCurrentBgColor(), 0 };
            WINDOWCOMPOSITIONATTRIBDATA data = { WCA_ACCENT_POLICY, &policy, sizeof(ACCENT_POLICY) };
            SetComp(hwnd, &data);
        }
    }
}

// --- Region Clipping ---
void UpdateWindowShape(HWND hwnd, int width, int height) {
    // Create region with slightly inset to avoid white border artifacts on some systems
    HRGN hRgn = CreateRoundRectRgn(0, 0, width + 1, height + 1, height, height);
    SetWindowRgn(hwnd, hRgn, TRUE);
}

// --- Drawing ---
void DrawMediaPanel(HDC hdc, int width, int height) {
    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    // FIX: Use AntiAlias for text instead of ClearType.
    // ClearType (GridFit) often produces ugly fringes on transparent/layered windows.
    graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);
    graphics.Clear(Color(0, 0, 0, 0)); 

    Rect rect(0, 0, width - 1, height - 1);
    GraphicsPath path;
    int arcSize = height; 
    path.AddArc(rect.X, rect.Y, arcSize, arcSize, 180, 90);
    path.AddArc(rect.X + rect.Width - arcSize, rect.Y, arcSize, arcSize, 270, 90);
    path.AddArc(rect.X + rect.Width - arcSize, rect.Y + rect.Height - arcSize, arcSize, arcSize, 0, 90);
    path.AddArc(rect.X, rect.Y + rect.Height - arcSize, arcSize, arcSize, 90, 90);
    path.CloseFigure();

    SolidBrush bgBrush((Color(GetCurrentBgColor())));
    graphics.FillPath(&bgBrush, &path);
    
    Color mainColor((GetCurrentTextColor()));
    // Thinner, more subtle border
    Pen borderPen(Color(25, mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue()), 1);
    graphics.DrawPath(&borderPen, &path);

    SolidBrush iconBrush(mainColor);
    SolidBrush hoverBrush(Color(255, mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue()));
    SolidBrush activeBg(Color(40, mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue()));

    int centerY = height / 2;

    // Prev
    if (g_HoverState == 1) graphics.FillEllipse(&activeBg, 12, centerY - 12, 24, 24);
    SolidBrush* pBrush = (g_HoverState == 1) ? &hoverBrush : &iconBrush;
    Point prevPts[3] = { Point(28, centerY - 6), Point(28, centerY + 6), Point(18, centerY) };
    graphics.FillPolygon(pBrush, prevPts, 3);
    graphics.FillRectangle(pBrush, 16, centerY - 6, 2, 12); 

    // Play
    if (g_HoverState == 2) graphics.FillEllipse(&activeBg, 38, centerY - 12, 24, 24);
    SolidBrush* plBrush = (g_HoverState == 2) ? &hoverBrush : &iconBrush;
    Point playPts[3] = { Point(46, centerY - 8), Point(46, centerY + 8), Point(58, centerY) };
    graphics.FillPolygon(plBrush, playPts, 3);

    // Next
    if (g_HoverState == 3) graphics.FillEllipse(&activeBg, 64, centerY - 12, 24, 24);
    SolidBrush* nBrush = (g_HoverState == 3) ? &hoverBrush : &iconBrush;
    Point nextPts[3] = { Point(72, centerY - 6), Point(72, centerY + 6), Point(82, centerY) };
    graphics.FillPolygon(nBrush, nextPts, 3);
    graphics.FillRectangle(nBrush, 82, centerY - 6, 2, 12); 

    Pen sepPen(Color(30, mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue()), 1);
    graphics.DrawLine(&sepPen, 95, 10, 95, height - 10);

    FontFamily fontFamily(FONT_NAME);
    Font font(&fontFamily, (REAL)g_Settings.fontSize, FontStyleBold, UnitPixel);
    SolidBrush textBrush(mainColor);
    
    RectF layoutRect(0, 0, 1000, 100);
    RectF boundRect;
    graphics.MeasureString(g_CurrentTrack.c_str(), -1, &font, layoutRect, &boundRect);
    g_TextWidth = (int)boundRect.Width;

    int textAreaWidth = width - 110; 
    int textX = 100; 

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

// --- Window Procedure ---
LRESULT CALLBACK MediaWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: 
            UpdateAcrylic(hwnd);
            UpdateWindowShape(hwnd, g_Settings.width, g_Settings.height); 
            SetTimer(hwnd, IDT_CHECK_MUSIC, 1000, NULL); 
            SetTimer(hwnd, IDT_ZORDER, 2000, NULL);      
            return 0;

        case WM_ERASEBKGND: 
            return 1;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_SETTINGCHANGE:
            UpdateAcrylic(hwnd);
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;

        case WM_WINDOWPOSCHANGED: {
            WINDOWPOS* wp = (WINDOWPOS*)lParam;
            if (!(wp->flags & SWP_NOSIZE)) {
                UpdateWindowShape(hwnd, wp->cx, wp->cy);
            }
            break;
        }

        case WM_TIMER:
            if (wParam == IDT_CHECK_MUSIC) {
                wstring newTrack = GetNowPlayingText();
                if (newTrack.empty()) newTrack = L"No Music Detected";
                
                if (newTrack != g_CurrentTrack) {
                    g_CurrentTrack = newTrack;
                    g_ScrollOffset = 0; 
                    g_ScrollWait = 60; 
                    InvalidateRect(hwnd, NULL, FALSE);
                }

                HWND hTaskbar = FindWindow(TEXT("Shell_TrayWnd"), NULL);
                if (hTaskbar) {
                    RECT rc; GetWindowRect(hTaskbar, &rc);
                    int x = rc.left + g_Settings.offsetX; 
                    int taskbarHeight = rc.bottom - rc.top;
                    int y = rc.top + (taskbarHeight / 2) - (g_Settings.height / 2) + g_Settings.offsetY;
                    
                    RECT myRc; GetWindowRect(hwnd, &myRc);
                    if (myRc.left != x || myRc.top != y || 
                        (myRc.right - myRc.left) != g_Settings.width || 
                        (myRc.bottom - myRc.top) != g_Settings.height) {
                         SetWindowPos(hwnd, HWND_TOPMOST, x, y, g_Settings.width, g_Settings.height, SWP_NOACTIVATE);
                         InvalidateRect(hwnd, NULL, TRUE);
                    }
                }
            }
            else if (wParam == IDT_SCROLL) {
                if (g_IsScrolling) {
                    if (g_ScrollWait > 0) {
                        g_ScrollWait--;
                    } else {
                        g_ScrollOffset++;
                        if (g_ScrollOffset > g_TextWidth + 40) {
                            g_ScrollOffset = 0;
                            g_ScrollWait = 60; 
                        }
                        InvalidateRect(hwnd, NULL, FALSE);
                    }
                } else {
                    KillTimer(hwnd, IDT_SCROLL); 
                }
            }
            else if (wParam == IDT_ZORDER) {
                SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            }
            return 0;

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
            
            TRACKMOUSEEVENT tme;
            tme.cbSize = sizeof(TRACKMOUSEEVENT);
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd;
            TrackMouseEvent(&tme);
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
            
            if (g_IsScrolling) {
                SetTimer(hwnd, IDT_SCROLL, 16, NULL);
            }

            BitBlt(hdc, 0, 0, rc.right, rc.bottom, memDC, 0, 0, SRCCOPY);
            SelectObject(memDC, oldBitmap); DeleteObject(memBitmap); DeleteDC(memDC);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_NCHITTEST: return HTCLIENT;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// --- Main Thread ---
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
    
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnregisterClass(wc.lpszClassName, wc.hInstance);
    GdiplusShutdown(gdiplusToken);
}

std::thread* g_pMediaThread = nullptr;

// --- RENAMED CALLBACKS ---
BOOL WhTool_ModInit() {
    Wh_Log(L"[MusicLounge] Tool Init - Starting Media Thread");
    LoadSettings(); 
    g_Running = true;
    g_pMediaThread = new std::thread(MediaThread);
    return TRUE;
}

void WhTool_ModUninit() {
    Wh_Log(L"[MusicLounge] Tool Uninit");
    g_Running = false;
    if (g_hMediaWindow) {
        SendMessage(g_hMediaWindow, WM_CLOSE, 0, 0); 
    }
    if (g_pMediaThread) {
        if (g_pMediaThread->joinable()) g_pMediaThread->join();
        delete g_pMediaThread;
        g_pMediaThread = nullptr;
    }
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"[MusicLounge] Settings Changed");
    LoadSettings();
    if (g_hMediaWindow) {
         UpdateAcrylic(g_hMediaWindow);
         // Re-apply clipping in case size changed
         UpdateWindowShape(g_hMediaWindow, g_Settings.width, g_Settings.height);
         InvalidateRect(g_hMediaWindow, NULL, TRUE);
         SendMessage(g_hMediaWindow, WM_TIMER, IDT_CHECK_MUSIC, 0);
    }
}

// --- Windhawk Tool Mod Launcher Code ---
bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L"[MusicLounge] EntryPoint Hook - Thread exiting to prevent UI");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    bool isService = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0) {
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

    if (isService) {
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
    Wh_Log(L"[MusicLounge] Launcher Init");
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    if (GetModuleFileName(nullptr, currentProcessPath, ARRAYSIZE(currentProcessPath)) == 0) {
        Wh_Log(L"GetModuleFileName failed");
        return;
    }

    WCHAR commandLine[MAX_PATH * 2];
    _snwprintf(commandLine, ARRAYSIZE(commandLine), L"\"%s\" -tool-mod \"%s\"", currentProcessPath, WH_MOD_ID);

    STARTUPINFO si = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi;
    
    if (!CreateProcess(nullptr, commandLine, nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS, nullptr, nullptr, &si, &pi)) {
        Wh_Log(L"CreateProcess failed, error: %d", GetLastError());
        return;
    }

    Wh_Log(L"[MusicLounge] Launched Tool Process ID: %d", pi.dwProcessId);
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
