// ==WindhawkMod==
// @id              taskbar-music-lounge
// @name            Taskbar Music Lounge
// @description     A native-style music ticker with media controls.
// @version         4.0
// @author          Hashah2311
// @github          https://github.com/Hashah2311
// @include         explorer.exe
// @compilerOptions -lole32 -ldwmapi -lgdi32 -luser32 -lwindowsapp -lshcore -lgdiplus -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Music Lounge

A media controller that uses Windows 11 native DWM styling for a seamless look.

## 🚀 v3 vs v4: Major Architecture Shift
| Feature | v3 (Legacy) | v4 (Current) |
| :--- | :--- | :--- |
| **Performance** | Polling Loop (High CPU wakeups) | **Event-Driven** (0% CPU when idle) |
| **Smoothness** | 1-second delay on moves | **Instant** sync with Taskbar animations |
| **Compatibility** | Chrome/Spotify only | **Universal** supports all media players |
| **Visuals** | Square Art, Fixed Size | **Rounded Art**, 4K Scalable Icons, Acrylic Blur, Rounded Art |
| **Behavior** | Always visible (Clunky) | **Smart Auto-Hide** Hides on fullscreen (Idle, Games, Taskbar Hide) |

## ✨ Features
* **Universal Support:** Smart scanning detects active playback from any app, not just the "focused" one.
* **Album Art:** Displays current track cover art.
* **Fullscreen Mode:** Hides automatically when running full-screen applications.
* **Native Look:** Uses Windows 11 hardware-accelerated rounding and acrylic blur.
* **Idle Timeout:** Optional setting to fade out the widget when music is paused for X seconds.
* **Controls:** Play/Pause, Next, Previous.
* **Volume:** Scroll over widget to adjust volume.


## ⚠️ Requirements
* **Disable Widgets:** Taskbar Settings -> Widgets -> Off.
* **Windows 11:** Required for rounded corners.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- PanelWidth: 300
  $name: Panel Width
- PanelHeight: 48
  $name: Panel Height
- FontSize: 11
  $name: Font Size
- ButtonScale: 1.0
  $name: Button Scale (1.0 = Normal, 2.0 = 4K)
- HideFullscreen: false
  $name: Hide when Fullscreen
- IdleTimeout: 0
  $name: Auto-hide when paused (Seconds). Set 0 to disable.
- OffsetX: 12
  $name: X Offset
- OffsetY: 0
  $name: Y Offset
- AutoTheme: true
  $name: Auto Theme
- TextColor: 0xFFFFFF
  $name: Manual Text Color (Hex)
- BgOpacity: 0
  $name: Acrylic Tint Opacity (0-255). Keep 0 for pure glass.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shobjidl.h> 
#include <shellapi.h>
#include <dwmapi.h>
#include <gdiplus.h>
#include <shcore.h> 
#include <string>
#include <vector>
#include <atomic>
#include <thread>
#include <mutex>
#include <cstdio>

// WinRT
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>

using namespace Gdiplus;
using namespace std;
using namespace winrt;
using namespace Windows::Media::Control;
using namespace Windows::Storage::Streams;

// --- Constants ---
const WCHAR* FONT_NAME = L"Segoe UI Variable Display"; 

// --- DWM API ---
typedef enum _WINDOWCOMPOSITIONATTRIB { WCA_ACCENT_POLICY = 19 } WINDOWCOMPOSITIONATTRIB;
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
typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);

// --- Z-Band API ---
enum ZBID {
    ZBID_DEFAULT = 0,
    ZBID_DESKTOP = 1,
    ZBID_UIACCESS = 2,
    ZBID_IMMERSIVE_IHM = 3,
    ZBID_IMMERSIVE_NOTIFICATION = 4,
    ZBID_IMMERSIVE_APPCHROME = 5,
    ZBID_IMMERSIVE_MOGO = 6,
    ZBID_IMMERSIVE_EDGY = 7,
    ZBID_IMMERSIVE_INACTIVEMOBODY = 8,
    ZBID_IMMERSIVE_INACTIVEDOCK = 9,
    ZBID_IMMERSIVE_ACTIVEMOBODY = 10,
    ZBID_IMMERSIVE_ACTIVEDOCK = 11,
    ZBID_IMMERSIVE_BACKGROUND = 12,
    ZBID_IMMERSIVE_SEARCH = 13,
    ZBID_GENUINE_WINDOWS = 14,
    ZBID_IMMERSIVE_RESTRICTED = 15,
    ZBID_SYSTEM_TOOLS = 16,
    ZBID_LOCK = 17,
    ZBID_ABOVELOCK_UX = 18,
};

typedef HWND(WINAPI* pCreateWindowInBand)(
    DWORD dwExStyle,
    LPCWSTR lpClassName,
    LPCWSTR lpWindowName,
    DWORD dwStyle,
    int x,
    int y,
    int nWidth,
    int nHeight,
    HWND hWndParent,
    HMENU hMenu,
    HINSTANCE hInstance,
    LPVOID lpParam,
    DWORD dwBand
);

// --- Configurable State ---
struct ModSettings {
    int width = 300;
    int height = 48;
    int fontSize = 11;
    double buttonScale = 1.0; 
    bool hideFullscreen = false;
    int idleTimeout = 0; 
    int offsetX = 12;
    int offsetY = 0;
    bool autoTheme = true;
    DWORD manualTextColor = 0xFFFFFFFF; 
    int bgOpacity = 0;   
} g_Settings;

// --- Global State ---
HWND g_hMediaWindow = NULL;
bool g_Running = true; 
int g_HoverState = 0; 
HWINEVENTHOOK g_TaskbarHook = nullptr; 

// Idle Tracking
int g_IdleSecondsCounter = 0;
bool g_IsHiddenByIdle = false;

// Data Model
struct MediaState {
    wstring title = L"Waiting for media...";
    wstring artist = L"";
    bool isPlaying = false;
    bool hasMedia = false;
    Bitmap* albumArt = nullptr;
    mutex lock;
} g_MediaState;

// Animation
int g_ScrollOffset = 0;
int g_TextWidth = 0;
bool g_IsScrolling = false;
int g_ScrollWait = 60;

// --- Settings ---
void LoadSettings() {
    g_Settings.width = Wh_GetIntSetting(L"PanelWidth");
    g_Settings.height = Wh_GetIntSetting(L"PanelHeight");
    g_Settings.fontSize = Wh_GetIntSetting(L"FontSize");
    g_Settings.offsetX = Wh_GetIntSetting(L"OffsetX");
    g_Settings.offsetY = Wh_GetIntSetting(L"OffsetY");
    g_Settings.autoTheme = Wh_GetIntSetting(L"AutoTheme") != 0;
    
    PCWSTR scaleStr = Wh_GetStringSetting(L"ButtonScale");
    if (scaleStr) {
        g_Settings.buttonScale = _wtof(scaleStr);
        Wh_FreeStringSetting(scaleStr);
    } else {
        g_Settings.buttonScale = 1.0;
    }
    if (g_Settings.buttonScale < 0.5) g_Settings.buttonScale = 0.5;
    if (g_Settings.buttonScale > 4.0) g_Settings.buttonScale = 4.0;

    g_Settings.hideFullscreen = Wh_GetIntSetting(L"HideFullscreen") != 0;
    g_Settings.idleTimeout = Wh_GetIntSetting(L"IdleTimeout");

    PCWSTR textHex = Wh_GetStringSetting(L"TextColor");
    DWORD textRGB = 0xFFFFFF;
    if (textHex) {
        if (wcslen(textHex) > 0) textRGB = wcstoul(textHex, nullptr, 16);
        Wh_FreeStringSetting(textHex);
    }
    g_Settings.manualTextColor = 0xFF000000 | textRGB;
    
    g_Settings.bgOpacity = Wh_GetIntSetting(L"BgOpacity");
    if (g_Settings.bgOpacity < 0) g_Settings.bgOpacity = 0;
    if (g_Settings.bgOpacity > 255) g_Settings.bgOpacity = 255;

    if (g_Settings.width < 100) g_Settings.width = 300;
    if (g_Settings.height < 24) g_Settings.height = 48;
}

// --- WinRT / GSMTC ---
GlobalSystemMediaTransportControlsSessionManager g_SessionManager = nullptr;

Bitmap* StreamToBitmap(IRandomAccessStreamWithContentType const& stream) {
    if (!stream) return nullptr;
    IStream* nativeStream = nullptr;
    if (SUCCEEDED(CreateStreamOverRandomAccessStream(reinterpret_cast<IUnknown*>(winrt::get_abi(stream)), IID_PPV_ARGS(&nativeStream)))) {
        Bitmap* bmp = Bitmap::FromStream(nativeStream);
        nativeStream->Release();
        if (bmp && bmp->GetLastStatus() == Ok) return bmp;
        delete bmp;
    }
    return nullptr;
}

void UpdateMediaInfo() {
    try {
        if (!g_SessionManager) {
            g_SessionManager = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
        }
        if (!g_SessionManager) return;

        // Iterate ALL sessions to find one that is actively PLAYING.
        GlobalSystemMediaTransportControlsSession session = nullptr;
        bool foundActive = false;

        auto sessionsList = g_SessionManager.GetSessions();
        for (auto const& s : sessionsList) {
            auto pb = s.GetPlaybackInfo();
            if (pb && pb.PlaybackStatus() == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing) {
                session = s;
                foundActive = true;
                break;
            }
        }

        if (!foundActive) {
            session = g_SessionManager.GetCurrentSession();
        }

        if (session) {
            auto props = session.TryGetMediaPropertiesAsync().get();
            auto info = session.GetPlaybackInfo();

            lock_guard<mutex> guard(g_MediaState.lock);
            
            wstring newTitle = props.Title().c_str();
            if (newTitle != g_MediaState.title || g_MediaState.albumArt == nullptr) {
                if (g_MediaState.albumArt) { delete g_MediaState.albumArt; g_MediaState.albumArt = nullptr; }
                auto thumbRef = props.Thumbnail();
                if (thumbRef) {
                    auto stream = thumbRef.OpenReadAsync().get();
                    g_MediaState.albumArt = StreamToBitmap(stream);
                }
            }
            g_MediaState.title = newTitle;
            g_MediaState.artist = props.Artist().c_str();
            g_MediaState.isPlaying = (info.PlaybackStatus() == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing);
            g_MediaState.hasMedia = true;
        } else {
            lock_guard<mutex> guard(g_MediaState.lock);
            g_MediaState.hasMedia = false;
            g_MediaState.title = L"No Media";
            g_MediaState.artist = L"";
            if (g_MediaState.albumArt) { delete g_MediaState.albumArt; g_MediaState.albumArt = nullptr; }
        }
    } catch (...) {
        lock_guard<mutex> guard(g_MediaState.lock);
        g_MediaState.hasMedia = false;
    }
}

void SendMediaCommand(int cmd) {
    try {
        if (!g_SessionManager) return;
        auto session = g_SessionManager.GetCurrentSession();
        if (session) {
            if (cmd == 1) session.TrySkipPreviousAsync();
            else if (cmd == 2) session.TryTogglePlayPauseAsync();
            else if (cmd == 3) session.TrySkipNextAsync();
        }
    } catch (...) {}
}

// --- Visuals ---
bool IsSystemLightMode() {
    DWORD value = 0; DWORD size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", L"SystemUsesLightTheme", RRF_RT_DWORD, nullptr, &value, &size) == ERROR_SUCCESS) {
        return value != 0;
    }
    return false;
}

DWORD GetCurrentTextColor() {
    if (g_Settings.autoTheme) return IsSystemLightMode() ? 0xFF000000 : 0xFFFFFFFF;
    return g_Settings.manualTextColor;
}

void UpdateAppearance(HWND hwnd) {
    DWM_WINDOW_CORNER_PREFERENCE preference = DWMWCP_ROUND;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &preference, sizeof(preference));

    HMODULE hUser = GetModuleHandle(L"user32.dll");
    if (hUser) {
        auto SetComp = (pSetWindowCompositionAttribute)GetProcAddress(hUser, "SetWindowCompositionAttribute");
        if (SetComp) {
            DWORD tint = 0; 
            if (g_Settings.autoTheme) {
                tint = IsSystemLightMode() ? 0x40FFFFFF : 0x40000000;
            } else {
                tint = (g_Settings.bgOpacity << 24) | (0xFFFFFF); 
            }
            ACCENT_POLICY policy = { ACCENT_ENABLE_ACRYLICBLURBEHIND, 0, tint, 0 };
            WINDOWCOMPOSITIONATTRIBDATA data = { WCA_ACCENT_POLICY, &policy, sizeof(ACCENT_POLICY) };
            SetComp(hwnd, &data);
        }
    }
}

void AddRoundedRect(GraphicsPath& path, int x, int y, int w, int h, int r) {
    int d = r * 2;
    path.AddArc(x, y, d, d, 180, 90);
    path.AddArc(x + w - d, y, d, d, 270, 90);
    path.AddArc(x + w - d, y + h - d, d, d, 0, 90);
    path.AddArc(x, y + h - d, d, d, 90, 90);
    path.CloseFigure();
}

void DrawMediaPanel(HDC hdc, int width, int height) {
    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);
    graphics.Clear(Color(0, 0, 0, 0)); 

    Color mainColor{GetCurrentTextColor()};
    
    MediaState state;
    {
        lock_guard<mutex> guard(g_MediaState.lock);
        state.title = g_MediaState.title;
        state.artist = g_MediaState.artist;
        state.albumArt = g_MediaState.albumArt ? g_MediaState.albumArt->Clone() : nullptr;
        state.hasMedia = g_MediaState.hasMedia;
        state.isPlaying = g_MediaState.isPlaying;
    }

    // 1. Album Art (Rounded)
    int artSize = height - 12;
    int artX = 6;
    int artY = 6;
    
    GraphicsPath path;
    AddRoundedRect(path, artX, artY, artSize, artSize, 8); 

    if (state.albumArt) {
        graphics.SetClip(&path);
        graphics.DrawImage(state.albumArt, artX, artY, artSize, artSize);
        graphics.ResetClip();
        delete state.albumArt;
    } else {
        SolidBrush placeBrush{Color(40, 128, 128, 128)};
        graphics.FillPath(&placeBrush, &path);
    }

    // 2. Controls (Scaled)
    double scale = g_Settings.buttonScale;
    int startControlX = artX + artSize + (int)(12 * scale);
    int controlY = height / 2;

    SolidBrush iconBrush{mainColor};
    SolidBrush hoverBrush{Color(255, mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue())};
    SolidBrush activeBg{Color(40, mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue())};

    float circleR = 12.0f * (float)scale; 
    float iconW = 8.0f * (float)scale;
    float iconH = 12.0f * (float)scale; 
    float gap = 28.0f * (float)scale;
    
    // Prev
    float pX = (float)startControlX;
    if (g_HoverState == 1) graphics.FillEllipse(&activeBg, pX - circleR, (float)controlY - circleR, circleR*2, circleR*2);
    PointF prevPts[3] = { PointF(pX + iconW, (float)controlY - (iconH/2)), PointF(pX + iconW, (float)controlY + (iconH/2)), PointF(pX, (float)controlY) };
    graphics.FillPolygon(g_HoverState == 1 ? &hoverBrush : &iconBrush, prevPts, 3);
    graphics.FillRectangle(g_HoverState == 1 ? &hoverBrush : &iconBrush, pX, (float)controlY - (iconH/2), 2.0f * (float)scale, iconH);

    // Play/Pause
    float plX = pX + gap;
    if (g_HoverState == 2) graphics.FillEllipse(&activeBg, plX - circleR, (float)controlY - circleR, circleR*2, circleR*2);
    if (state.isPlaying) {
        float barW = 3.0f * (float)scale;
        float barH = 14.0f * (float)scale;
        graphics.FillRectangle(g_HoverState == 2 ? &hoverBrush : &iconBrush, plX - (barW + 1), (float)controlY - (barH/2), barW, barH);
        graphics.FillRectangle(g_HoverState == 2 ? &hoverBrush : &iconBrush, plX + 1, (float)controlY - (barH/2), barW, barH);
    } else {
        float playW = 10.0f * (float)scale;
        float playH = 16.0f * (float)scale;
        PointF playPts[3] = { PointF(plX - (playW/2), (float)controlY - (playH/2)), PointF(plX - (playW/2), (float)controlY + (playH/2)), PointF(plX + (playW/2), (float)controlY) };
        graphics.FillPolygon(g_HoverState == 2 ? &hoverBrush : &iconBrush, playPts, 3);
    }

    // Next
    float nX = plX + gap;
    if (g_HoverState == 3) graphics.FillEllipse(&activeBg, nX - circleR, (float)controlY - circleR, circleR*2, circleR*2);
    PointF nextPts[3] = { PointF(nX - iconW, (float)controlY - (iconH/2)), PointF(nX - iconW, (float)controlY + (iconH/2)), PointF(nX, (float)controlY) };
    graphics.FillPolygon(g_HoverState == 3 ? &hoverBrush : &iconBrush, nextPts, 3);
    graphics.FillRectangle(g_HoverState == 3 ? &hoverBrush : &iconBrush, nX, (float)controlY - (iconH/2), 2.0f * (float)scale, iconH);

    // 3. Text
    int textX = (int)(nX + (20 * scale));
    int textMaxW = width - textX - 10;
    
    wstring fullText = state.title;
    if (!state.artist.empty()) fullText += L" • " + state.artist;

    FontFamily fontFamily(FONT_NAME, nullptr);
    Font font(&fontFamily, (REAL)g_Settings.fontSize, FontStyleBold, UnitPixel);
    SolidBrush textBrush{mainColor};
    
    RectF layoutRect(0, 0, 2000, 100); 
    RectF boundRect;
    graphics.MeasureString(fullText.c_str(), -1, &font, layoutRect, &boundRect);
    g_TextWidth = (int)boundRect.Width;

    Region textClip(Rect(textX, 0, textMaxW, height));
    graphics.SetClip(&textClip);

    float textY = (height - boundRect.Height) / 2.0f;

    if (g_TextWidth > textMaxW) {
        g_IsScrolling = true;
        float drawX = (float)(textX - g_ScrollOffset);
        graphics.DrawString(fullText.c_str(), -1, &font, PointF(drawX, textY), &textBrush);
        if (drawX + g_TextWidth < width) {
             graphics.DrawString(fullText.c_str(), -1, &font, PointF(drawX + g_TextWidth + 40, textY), &textBrush);
        }
    } else {
        g_IsScrolling = false;
        g_ScrollOffset = 0;
        graphics.DrawString(fullText.c_str(), -1, &font, PointF((float)textX, textY), &textBrush);
    }
}

// --- Event Hook ---
bool IsTaskbarWindow(HWND hwnd) {
    WCHAR cls[64];
    if (!hwnd) return false;
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    return wcscmp(cls, L"Shell_TrayWnd") == 0;
}

void CALLBACK TaskbarEventProc(
    HWINEVENTHOOK,
    DWORD event,
    HWND hwnd,
    LONG, LONG,
    DWORD, DWORD
) {
    if (!IsTaskbarWindow(hwnd) || !g_hMediaWindow) return;
    PostMessage(g_hMediaWindow, WM_APP + 10, 0, 0);
}

// --- Window Procedure ---
#define IDT_POLL_MEDIA 1001
#define IDT_ANIMATION  1002
#define APP_WM_CLOSE   WM_APP

LRESULT CALLBACK MediaWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: 
            UpdateAppearance(hwnd); 
            SetTimer(hwnd, IDT_POLL_MEDIA, 1000, NULL); 
            
            // Register Event Hook scoped to Taskbar Thread
            {
                HWND hTaskbar = FindWindow(L"Shell_TrayWnd", nullptr);
                if (hTaskbar) {
                    DWORD pid = 0;
                    DWORD tid = GetWindowThreadProcessId(hTaskbar, &pid);
                    if (tid != 0) {
                        g_TaskbarHook = SetWinEventHook(
                            EVENT_OBJECT_LOCATIONCHANGE,
                            EVENT_OBJECT_LOCATIONCHANGE,
                            nullptr,
                            TaskbarEventProc,
                            pid, tid,
                            WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS
                        );
                    }
                }
                PostMessage(hwnd, WM_APP + 10, 0, 0);
            }
            return 0;

        case WM_ERASEBKGND: 
            return 1;

        case WM_CLOSE:
            return 0;

        case APP_WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            if (g_TaskbarHook) {
                UnhookWinEvent(g_TaskbarHook);
                g_TaskbarHook = nullptr;
            }
            g_SessionManager = nullptr;
            PostQuitMessage(0);
            return 0;

        case WM_SETTINGCHANGE:
            UpdateAppearance(hwnd);
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;

        case WM_TIMER:
            if (wParam == IDT_POLL_MEDIA) {
                UpdateMediaInfo();
                
                bool shouldHide = false;

                // 1. Check Fullscreen
                if (g_Settings.hideFullscreen) {
                    QUERY_USER_NOTIFICATION_STATE state;
                    if (SUCCEEDED(SHQueryUserNotificationState(&state))) {
                        if (state == QUNS_BUSY || state == QUNS_RUNNING_D3D_FULL_SCREEN || state == QUNS_PRESENTATION_MODE) {
                            shouldHide = true;
                        }
                    }
                }

                // 2. Check Idle Timeout
                bool isPlaying = false;
                {
                    lock_guard<mutex> guard(g_MediaState.lock);
                    isPlaying = g_MediaState.isPlaying;
                }

                if (g_Settings.idleTimeout > 0) {
                    if (isPlaying) {
                        g_IdleSecondsCounter = 0;
                        g_IsHiddenByIdle = false;
                    } else {
                        g_IdleSecondsCounter++;
                        if (g_IdleSecondsCounter >= g_Settings.idleTimeout) {
                            g_IsHiddenByIdle = true;
                        }
                    }
                } else {
                    g_IsHiddenByIdle = false;
                }

                if (g_IsHiddenByIdle) shouldHide = true;

                // 3. Final Visibility Check
                if (shouldHide && IsWindowVisible(hwnd)) {
                    ShowWindow(hwnd, SW_HIDE);
                } else if (!shouldHide && !IsWindowVisible(hwnd)) {
                    // Only restore if Taskbar is also visible
                    HWND hTaskbar = FindWindow(L"Shell_TrayWnd", nullptr);
                    if (hTaskbar && IsWindowVisible(hTaskbar)) {
                        ShowWindow(hwnd, SW_SHOWNOACTIVATE);
                    }
                }
                
                InvalidateRect(hwnd, NULL, FALSE);
            }
            else if (wParam == IDT_ANIMATION) {
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
                    KillTimer(hwnd, IDT_ANIMATION); 
                }
            }
            return 0;

        case WM_APP + 10: {
            HWND hTaskbar = FindWindow(TEXT("Shell_TrayWnd"), nullptr);
            if (!hTaskbar) break;

            // Merged Logic: Check visibility first
            if (!IsWindowVisible(hTaskbar)) {
                if (IsWindowVisible(hwnd)) ShowWindow(hwnd, SW_HIDE);
                return 0;
            }

            // Restore visibility if we aren't hidden by fullscreen or Idle modes
            // (The Timer loop handles fullscreen/idle hiding, this handles Taskbar hiding)
            if (!g_IsHiddenByIdle && !IsWindowVisible(hwnd)) {
                // Double check fullscreen mode isn't forcing hide
                bool gameModeHide = false;
                if (g_Settings.hideFullscreen) {
                     QUERY_USER_NOTIFICATION_STATE state;
                     if (SUCCEEDED(SHQueryUserNotificationState(&state))) {
                        if (state == QUNS_BUSY || state == QUNS_RUNNING_D3D_FULL_SCREEN || state == QUNS_PRESENTATION_MODE) gameModeHide = true;
                     }
                }
                if (!gameModeHide) ShowWindow(hwnd, SW_SHOWNOACTIVATE);
            }

            RECT rc;
            GetWindowRect(hTaskbar, &rc);

            int x = rc.left + g_Settings.offsetX;
            int taskbarHeight = rc.bottom - rc.top;
            int y = rc.top + (taskbarHeight / 2) -
            (g_Settings.height / 2) + g_Settings.offsetY;
            
            RECT myRc; GetWindowRect(hwnd, &myRc);
            if (myRc.left != x || myRc.top != y || 
                (myRc.right - myRc.left) != g_Settings.width || 
                (myRc.bottom - myRc.top) != g_Settings.height) {
                    SetWindowPos(
                        hwnd,
                        HWND_TOPMOST,
                        x, y,
                        g_Settings.width,
                        g_Settings.height,
                        SWP_NOACTIVATE
                    );
            }
            return 0;
        }

        case WM_MOUSEMOVE: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            int artSize = g_Settings.height - 12;
            double scale = g_Settings.buttonScale;
            
            // Re-calculate hit targets based on scale
            int startControlX = 6 + artSize + (int)(12 * scale);
            float gap = 28.0f * (float)scale;
            float pX = (float)startControlX;
            float plX = pX + gap;
            float nX = plX + gap;
            float radius = 12.0f * (float)scale;

            int newState = 0;
            if (y > 10 && y < g_Settings.height - 10) {
                if (x >= pX - radius && x <= pX + radius) newState = 1;
                else if (x >= plX - radius && x <= plX + radius) newState = 2;
                else if (x >= nX - radius && x <= nX + radius) newState = 3;
            }
            
            if (newState != g_HoverState) {
                g_HoverState = newState;
                InvalidateRect(hwnd, NULL, FALSE);
            }
            
            TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, hwnd, 0 };
            TrackMouseEvent(&tme);
            return 0;
        }
        case WM_MOUSELEAVE:
            g_HoverState = 0;
            InvalidateRect(hwnd, NULL, FALSE);
            break;
        case WM_LBUTTONUP:
            if (g_HoverState > 0) SendMediaCommand(g_HoverState);
            return 0;
        case WM_MOUSEWHEEL: {
            short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
            keybd_event(zDelta > 0 ? VK_VOLUME_UP : VK_VOLUME_DOWN, 0, 0, 0);
            keybd_event(zDelta > 0 ? VK_VOLUME_UP : VK_VOLUME_DOWN, 0, KEYEVENTF_KEYUP, 0);
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
            
            if (g_IsScrolling) SetTimer(hwnd, IDT_ANIMATION, 16, NULL);

            BitBlt(hdc, 0, 0, rc.right, rc.bottom, memDC, 0, 0, SRCCOPY);
            SelectObject(memDC, oldBitmap); DeleteObject(memBitmap); DeleteDC(memDC);
            EndPaint(hwnd, &ps);
            return 0;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// --- Main Thread ---
void MediaThread() {
    winrt::init_apartment();

    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    WNDCLASS wc = {0};
    wc.lpfnWndProc = MediaWndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = TEXT("WindhawkMusicLounge_GSMTC");
    wc.hCursor = LoadCursor(NULL, IDC_HAND);
    RegisterClass(&wc);

    // Try to use CreateWindowInBand
    HMODULE hUser32 = GetModuleHandle(L"user32.dll");
    pCreateWindowInBand CreateWindowInBand = nullptr;
    if (hUser32) {
        CreateWindowInBand = (pCreateWindowInBand)GetProcAddress(hUser32, "CreateWindowInBand");
    }

    if (CreateWindowInBand) {
        g_hMediaWindow = CreateWindowInBand(
            WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
            wc.lpszClassName, TEXT("MusicLounge"),
            WS_POPUP | WS_VISIBLE,
            0, 0, g_Settings.width, g_Settings.height,
            NULL, NULL, wc.hInstance, NULL,
            ZBID_IMMERSIVE_NOTIFICATION
        );
        if (g_hMediaWindow) {
            Wh_Log(L"Created window in ZBID_IMMERSIVE_NOTIFICATION band");
        }
    }

    if (!g_hMediaWindow) {
        Wh_Log(L"Falling back to CreateWindowEx");
        g_hMediaWindow = CreateWindowEx(
            WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
            wc.lpszClassName, TEXT("MusicLounge"),
            WS_POPUP | WS_VISIBLE,
            0, 0, g_Settings.width, g_Settings.height,
            NULL, NULL, wc.hInstance, NULL
        );
    }

    SetLayeredWindowAttributes(g_hMediaWindow, 0, 255, LWA_ALPHA);
    
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnregisterClass(wc.lpszClassName, wc.hInstance);
    GdiplusShutdown(gdiplusToken);
    winrt::uninit_apartment();
}

std::thread* g_pMediaThread = nullptr;

// --- CALLBACKS ---
BOOL WhTool_ModInit() {
    SetCurrentProcessExplicitAppUserModelID(L"taskbar-music-lounge");
    LoadSettings(); 
    g_Running = true;
    g_pMediaThread = new std::thread(MediaThread);
    return TRUE;
}

void WhTool_ModUninit() {
    g_Running = false;
    if (g_hMediaWindow) SendMessage(g_hMediaWindow, APP_WM_CLOSE, 0, 0);
    if (g_pMediaThread) {
        if (g_pMediaThread->joinable()) g_pMediaThread->join();
        delete g_pMediaThread;
        g_pMediaThread = nullptr;
    }
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    if (g_hMediaWindow) {
         SendMessage(g_hMediaWindow, WM_TIMER, IDT_POLL_MEDIA, 0);
         SendMessage(g_hMediaWindow, WM_SETTINGCHANGE, 0, 0); 
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk-mods/pull/1916
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
