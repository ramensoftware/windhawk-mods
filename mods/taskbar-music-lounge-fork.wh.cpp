// ==WindhawkMod==
// @id              taskbar-music-lounge-fork
// @name            Taskbar Music Lounge - Fork (V4 Merged & Adaptive)
// @description     A native-style music ticker with media controls, adaptive colors, and Caps Lock notify. (Original by Hashah2311)
// @version         1.0.0
// @author          Uiisland
// @github          https://github.com/Uiisland
// @include         explorer.exe
// @compilerOptions -lole32 -ldwmapi -lgdi32 -luser32 -lwindowsapp -lshcore -lgdiplus -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Music Lounge

这是一个使用 Windows 11 原生 DWM 样式实现无缝外观的媒体控制器。包含了从 V4.0.1 合并的功能以及自定义的 UI 增强。

✨ 功能特性

通用支持：通过 GSMTC 支持任何媒体播放器。可靠地寻找活动会话。

专辑封面背景：模糊当前的曲目封面艺术并将其用作背景。

自适应颜色：自动提取封面亮度，将 UI 元素切换为浅色或深色模式。

迷你 Logo 模式：在没有媒体播放时，平滑折叠成一个紧凑的音符徽标。

封面模式：图像始终无失真地填充窗口（如有需要会进行裁剪）。

遮罩叠加（可选）：深色/浅色半透明图层，确保文本和控件清晰可见。

原生外观：Windows 11 圆角和亚克力风格的模糊效果。

控件：播放/暂停、下一曲、上一曲。支持自定义缩放。

音量调节：在小部件上滚动鼠标滚轮即可调节音量。

大写锁定通知：切换大写锁定时，在底部居中显示弹出窗口。（对游戏友好，不抢占焦点）

自动隐藏：支持在全屏应用处于活动状态或空闲时隐藏。

A media controller that uses Windows 11 native DWM styling for a seamless look.
Includes merged features from V4.0.1 and custom UI enhancements.

## ✨ Features
* **Universal Support:** Works with any media player via GSMTC. Finds active session reliably.
* **Album Art Background:** Current track cover art is blurred and used as background.
* **Adaptive Colors:** Automatically extracts cover brightness to switch UI elements to light or dark mode.
* **Mini Logo Mode:** Smoothly collapses into a compact music note logo when no media is playing.
* **Cover Mode:** Image always fills the window without distortion (cropped if needed).
* **Mask Overlay (Optional):** Dark/Light semi-transparent layer ensures text and controls are visible.
* **Native Look:** Windows 11 rounded corners and acrylic-style blur.
* **Controls:** Play/Pause, Next, Previous. Supports custom scaling.
* **Volume:** Scroll over widget to adjust volume.
* **Caps Lock Notification:** Shows a popup at the bottom center when Caps Lock toggles. (Game-friendly, no focus stealing)
* **Auto Hide:** Option to hide when full-screen apps are active or when idle.
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
- ButtonScale: "1.0"
  $name: UI Scale
  $description: Scale for controls and album art. Default is 1.0. Minimum 0.5, Maximum 4.0.
- HideFullscreen: false
  $name: Auto-hide in Fullscreen Apps
  $description: Automatically hide the widget when a fullscreen application (like a game) is active.
- IdleTimeout: 0
  $name: Idle Timeout (Seconds)
  $description: Hide the widget if media has been paused for this many seconds. Set to 0 to disable.
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
- BlurStrength: 8
  $name: Blur Strength (1-10, higher = more blur)
  $description: Adjusts how strong the background blur appears.
- EnableMask: true
  $name: Enable Dark/Light Mask
  $description: If enabled, an overlay is applied to improve text contrast. Automatically adapts to cover brightness.
- MaskOpacity: 180
  $name: Mask Opacity (0-255)
  $description: Darkness of the overlay layer (0=transparent, 255=opaque). Only active if mask is enabled.
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
#include <cmath>

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
const WCHAR* FONT_NAME = L"Microsoft YaHei UI"; 

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
    int blurStrength = 8;
    bool enableMask = true;
    int maskOpacity = 180;
} g_Settings;

// --- Global State ---
HWND g_hMediaWindow = NULL;
bool g_Running = true; 
int g_HoverState = 0; 
HWINEVENTHOOK g_TaskbarHook = nullptr; 
UINT g_TaskbarCreatedMsg = RegisterWindowMessage(L"TaskbarCreated");

#define WM_UPDATE_BLUR (WM_APP + 1)

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
    
    Bitmap* blurredBg = nullptr;
    int bgWidth = 0;
    int bgHeight = 0;
    
    int artVersion = 0;
    int lastArtVersion = -1;

    // 自适应颜色：封面是否为深色调
    bool isDarkCover = true;
} g_MediaState;

// Caps Lock Notification
HWND g_hCapsWindow = NULL;
int g_capsAlpha = 0; 
bool g_LastCapsState = false;
#define IDT_CAPSLOCK_POLL 1003
#define IDT_CAPS_HIDE 1004

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

    g_Settings.blurStrength = Wh_GetIntSetting(L"BlurStrength");
    if (g_Settings.blurStrength < 1) g_Settings.blurStrength = 1;
    if (g_Settings.blurStrength > 10) g_Settings.blurStrength = 10;

    g_Settings.enableMask = Wh_GetIntSetting(L"EnableMask") != 0;

    g_Settings.maskOpacity = Wh_GetIntSetting(L"MaskOpacity");
    if (g_Settings.maskOpacity < 0) g_Settings.maskOpacity = 0;
    if (g_Settings.maskOpacity > 255) g_Settings.maskOpacity = 255;

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
            bool artChanged = false;
            if (newTitle != g_MediaState.title || g_MediaState.albumArt == nullptr) {
                if (g_MediaState.albumArt) { delete g_MediaState.albumArt; g_MediaState.albumArt = nullptr; }
                auto thumbRef = props.Thumbnail();
                if (thumbRef) {
                    auto stream = thumbRef.OpenReadAsync().get();
                    g_MediaState.albumArt = StreamToBitmap(stream);
                    artChanged = true;
                    g_MediaState.artVersion++;

                    // 极速提取平均颜色以判断亮暗色调
                    if (g_MediaState.albumArt) {
                        Bitmap bmp1x1(1, 1, PixelFormat32bppARGB);
                        Graphics g(&bmp1x1);
                        g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
                        // 将整个封面缩放到 1x1 像素，GDI+ 会自动计算出平均色
                        g.DrawImage(g_MediaState.albumArt, 0, 0, 1, 1);
                        Color avgColor;
                        bmp1x1.GetPixel(0, 0, &avgColor);
                        
                        // 计算亮度 Luminance = 0.299*R + 0.587*G + 0.114*B
                        double lum = 0.299 * avgColor.GetR() + 0.587 * avgColor.GetG() + 0.114 * avgColor.GetB();
                        g_MediaState.isDarkCover = (lum < 135.0); // 以 135 作为分界线
                    }
                }
            }
            g_MediaState.title = newTitle;
            g_MediaState.artist = props.Artist().c_str();
            g_MediaState.isPlaying = (info.PlaybackStatus() == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing);
            g_MediaState.hasMedia = true;

            if (artChanged && g_hMediaWindow) {
                PostMessage(g_hMediaWindow, WM_UPDATE_BLUR, 0, 0);
            }
        } else {
            lock_guard<mutex> guard(g_MediaState.lock);
            g_MediaState.hasMedia = false;
            g_MediaState.title = L"暂无媒体播放";
            g_MediaState.artist = L"";
            if (g_MediaState.albumArt) { delete g_MediaState.albumArt; g_MediaState.albumArt = nullptr; }
            g_MediaState.artVersion++;
            g_MediaState.isDarkCover = true; 
            if (g_MediaState.blurredBg) {
                delete g_MediaState.blurredBg;
                g_MediaState.blurredBg = nullptr;
            }
            g_MediaState.bgWidth = g_MediaState.bgHeight = 0;
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

// --- 更新模糊背景 ---
void UpdateBlurredBackground(HWND hwnd) {
    RECT rc;
    GetClientRect(hwnd, &rc);
    int w = rc.right, h = rc.bottom;

    lock_guard<mutex> guard(g_MediaState.lock);

    if (!g_MediaState.albumArt) {
        if (g_MediaState.blurredBg) {
            delete g_MediaState.blurredBg;
            g_MediaState.blurredBg = nullptr;
        }
        g_MediaState.bgWidth = 0;
        g_MediaState.bgHeight = 0;
        g_MediaState.lastArtVersion = -1;
        return;
    }

    if (g_MediaState.blurredBg && 
        g_MediaState.bgWidth == w && 
        g_MediaState.bgHeight == h &&
        g_MediaState.lastArtVersion == g_MediaState.artVersion) {
        return;
    }

    int srcW = g_MediaState.albumArt->GetWidth();
    int srcH = g_MediaState.albumArt->GetHeight();

    int scaleFactor = max(1, g_Settings.blurStrength * g_Settings.blurStrength); 
    int smallW = max(1, srcW / scaleFactor);
    int smallH = max(1, srcH / scaleFactor);

    Bitmap smallBmp(smallW, smallH, PixelFormat32bppPARGB);
    Graphics smallG(&smallBmp);
    smallG.SetInterpolationMode(InterpolationModeBilinear); 
    smallG.DrawImage(g_MediaState.albumArt, 0, 0, smallW, smallH);

    Bitmap* bg = new Bitmap(w, h, PixelFormat32bppPARGB);
    if (bg && bg->GetLastStatus() == Ok) {
        Graphics g(bg);
        g.Clear(Color(0, 0, 0, 0));
        
        g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
        ImageAttributes attr;
        attr.SetWrapMode(WrapModeTileFlipXY); 

        float scaleW = (float)w / (float)smallW;
        float scaleH = (float)h / (float)smallH;
        float scale = max(scaleW, scaleH);
        
        scale *= 1.02f; 

        float drawW = (float)smallW * scale;
        float drawH = (float)smallH * scale;
        
        float offsetX = ((float)w - drawW) / 2.0f;
        float offsetY = ((float)h - drawH) / 2.0f;

        RectF destRect(offsetX, offsetY, drawW, drawH);
        g.DrawImage(&smallBmp, destRect, 0.0f, 0.0f, (REAL)smallW, (REAL)smallH, UnitPixel, &attr);

        if (g_MediaState.blurredBg) {
            delete g_MediaState.blurredBg;
        }
        g_MediaState.blurredBg = bg;
        g_MediaState.bgWidth = w;
        g_MediaState.bgHeight = h;
        g_MediaState.lastArtVersion = g_MediaState.artVersion;
    } else {
        delete bg;
    }
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

void DrawLockIcon(Graphics& graphics, int iconX, int iconY, int iconSize, bool isOn, COLORREF color) {
    GraphicsState state = graphics.Save();
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.TranslateTransform((REAL)iconX, (REAL)iconY);
    graphics.ScaleTransform((REAL)iconSize / 1024.0f, (REAL)iconSize / 1024.0f);

    Color gdiColor(GetRValue(color), GetGValue(color), GetBValue(color));
    Pen pen(gdiColor, 60.0f);
    pen.SetLineCap(LineCapRound, LineCapRound, DashCapRound);
    pen.SetLineJoin(LineJoinRound);

    float bodyW = 602.0f;
    float bodyH = 490.0f; 
    float bodyLeft = 512.0f - bodyW / 2.0f; 
    float bodyTop = 415.0f; 
    float cornerR = 40.0f;  

    GraphicsPath bodyPath;
    bodyPath.AddArc(bodyLeft, bodyTop, cornerR*2, cornerR*2, 180, 90);
    bodyPath.AddArc(bodyLeft + bodyW - cornerR*2, bodyTop, cornerR*2, cornerR*2, 270, 90);
    bodyPath.AddArc(bodyLeft + bodyW - cornerR*2, bodyTop + bodyH - cornerR*2, cornerR*2, cornerR*2, 0, 90);
    bodyPath.AddArc(bodyLeft, bodyTop + bodyH - cornerR*2, cornerR*2, cornerR*2, 90, 90);
    bodyPath.CloseFigure();
    graphics.DrawPath(&pen, &bodyPath);

    graphics.DrawLine(&pen, 512.0f, bodyTop + 180.0f, 512.0f, bodyTop + 180.0f + 120.0f);

    float shackleR = 192.0f; 
    float centerX = 512.0f;
    float shackleTop = bodyTop - 280.0f; 

    GraphicsState shackleState = graphics.Save();
    
    if (isOn) {
        GraphicsPath lockPath;
        lockPath.AddLine(centerX - shackleR, bodyTop, centerX - shackleR, shackleTop + shackleR);
        lockPath.AddArc(centerX - shackleR, shackleTop, shackleR * 2, shackleR * 2, 180, 180);
        lockPath.AddLine(centerX + shackleR, shackleTop + shackleR, centerX + shackleR, bodyTop);
        graphics.DrawPath(&pen, &lockPath);
    } else {
        float popUpHeight = 50.0f;
        graphics.TranslateTransform(0, -popUpHeight);

        GraphicsPath unlockPath;
        unlockPath.AddLine(centerX - shackleR, bodyTop + popUpHeight, centerX - shackleR, shackleTop + shackleR);
        unlockPath.AddArc(centerX - shackleR, shackleTop, shackleR * 2, shackleR * 2, 180, 180);
        unlockPath.AddLine(centerX + shackleR, shackleTop + shackleR, centerX + shackleR, shackleTop + shackleR + 00.0f);
        
        graphics.DrawPath(&pen, &unlockPath);
    }
    
    graphics.Restore(shackleState);
    graphics.Restore(state);
}

// --- 绘制音乐图标 ---
void DrawMusicIcon(Graphics& graphics, float x, float y, float size, Color color) {
    GraphicsState state = graphics.Save();
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);

    SolidBrush brush(color);
    Pen pen(color, size * 0.08f);
    pen.SetLineCap(LineCapRound, LineCapRound, DashCapRound);
    pen.SetLineJoin(LineJoinRound);

    float headW = size * 0.30f;
    float headH = size * 0.22f;
    
    float leftX = x + size * 0.20f;
    float leftY = y + size * 0.65f;
    
    float rightX = x + size * 0.60f;
    float rightY = y + size * 0.45f;

    GraphicsState st1 = graphics.Save();
    graphics.TranslateTransform(leftX + headW/2, leftY + headH/2);
    graphics.RotateTransform(-20.0f);
    graphics.FillEllipse(&brush, -headW/2, -headH/2, headW, headH);
    graphics.Restore(st1);

    GraphicsState st2 = graphics.Save();
    graphics.TranslateTransform(rightX + headW/2, rightY + headH/2);
    graphics.RotateTransform(-20.0f);
    graphics.FillEllipse(&brush, -headW/2, -headH/2, headW, headH);
    graphics.Restore(st2);

    float stemX1 = leftX + headW - size * 0.03f;
    float stemY1 = leftY + headH / 2.0f;
    float stemTop1 = y + size * 0.25f;
    
    float stemX2 = rightX + headW - size * 0.03f;
    float stemY2 = rightY + headH / 2.0f;
    float stemTop2 = y + size * 0.05f;

    graphics.DrawLine(&pen, stemX1, stemY1, stemX1, stemTop1);
    graphics.DrawLine(&pen, stemX2, stemY2, stemX2, stemTop2);

    PointF beamPts[4] = {
        PointF(stemX1 - size*0.04f, stemTop1),
        PointF(stemX1 - size*0.04f, stemTop1 + size*0.14f),
        PointF(stemX2 + size*0.04f, stemTop2 + size*0.14f),
        PointF(stemX2 + size*0.04f, stemTop2)
    };
    graphics.FillPolygon(&brush, beamPts, 4);

    graphics.Restore(state);
}

// --- Caps Lock Notification Window ---
LRESULT CALLBACK CapsWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            Graphics graphics(hdc);
            graphics.SetSmoothingMode(SmoothingModeAntiAlias);
            graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);
            graphics.Clear(Color(0, 0, 0, 0));

            RECT rc;
            GetClientRect(hwnd, &rc);
            int width = rc.right;
            int height = rc.bottom;

            wchar_t text[32];
            GetWindowTextW(hwnd, text, 32);
            bool isOn = (wcsstr(text, L"开") != NULL);

            bool lightMode = IsSystemLightMode();
            COLORREF foregroundColor = lightMode ? RGB(0, 0, 0) : RGB(255, 255, 255);
            
            FontFamily textFontFamily(L"Microsoft YaHei");
            Font textFont(&textFontFamily, 16, FontStyleRegular, UnitPixel);

            RectF textBounds;
            graphics.MeasureString(text, -1, &textFont, PointF(0, 0), &textBounds);

            int iconSize = 30;
            int spacing = 12; 
            int totalWidth = iconSize + spacing + (int)textBounds.Width;

            int startX = (width - totalWidth) / 2;
            int iconX = startX; 
            int iconY = (height - iconSize) / 2 - 2; 

            DrawLockIcon(graphics, iconX, iconY, iconSize, isOn, foregroundColor);

            float textX = (float)(iconX + iconSize + spacing) - 7.0f; 
            float textY = (iconY + iconSize / 2.0f) - (textBounds.Height / 2.0f) + 1.0f;

            SolidBrush textBrush(Color(255, GetRValue(foregroundColor), GetGValue(foregroundColor), GetBValue(foregroundColor)));
            
            graphics.DrawString(text, -1, &textFont, PointF(textX, textY), &textBrush);
            graphics.DrawString(text, -1, &textFont, PointF(textX + 0.5f, textY), &textBrush);

            int barHeight = 4;
            int barWidth = width - 110;
            int barX = (width - barWidth) / 2;
            int barY = height - 10;
            
            COLORREF barColor = isOn ? RGB(0x00, 0x88, 0xC7) : RGB(0x88, 0x88, 0x88);
            SolidBrush barBrush(Color(255, GetRValue(barColor), GetGValue(barColor), GetBValue(barColor)));

            GraphicsPath barPath;
            int cornerR = barHeight / 2;
            barPath.AddEllipse(barX, barY, cornerR * 2, barHeight);
            barPath.AddEllipse(barX + barWidth - cornerR * 2, barY, cornerR * 2, barHeight);
            graphics.FillPath(&barBrush, &barPath);
            graphics.FillRectangle(&barBrush, (REAL)barX + cornerR, (REAL)barY, (REAL)barWidth - cornerR * 2, (REAL)barHeight);

            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_ERASEBKGND:
            return 1; 

        case WM_TIMER: {
            if (wParam == 101) { 
                g_capsAlpha += 32; 
                if (g_capsAlpha >= 255) {
                    g_capsAlpha = 255;
                    KillTimer(hwnd, 101);
                }
                SetLayeredWindowAttributes(hwnd, 0, (BYTE)g_capsAlpha, LWA_ALPHA);
            }
            else if (wParam == 102) { 
                g_capsAlpha -= 32;
                if (g_capsAlpha <= 0) {
                    g_capsAlpha = 0;
                    KillTimer(hwnd, 102);
                    ShowWindow(hwnd, SW_HIDE); 
                }
                SetLayeredWindowAttributes(hwnd, 0, (BYTE)g_capsAlpha, LWA_ALPHA);
            }
            else if (wParam == IDT_CAPS_HIDE) {
                KillTimer(hwnd, IDT_CAPS_HIDE);
                KillTimer(hwnd, 101);
                SetTimer(hwnd, 102, 15, NULL);
            }
            return 0;
        }
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

void UpdateCapsWindowAppearance(HWND hwnd) {
    if (!hwnd) return;
    
    bool lightMode = IsSystemLightMode();
    DWORD accentColor = lightMode ? 0x9EFFFFFF : 0xB3000000;
    
    HMODULE hUser = GetModuleHandle(L"user32.dll");
    if (hUser) {
        auto SetComp = (pSetWindowCompositionAttribute)GetProcAddress(hUser, "SetWindowCompositionAttribute");
        if (SetComp) {
            ACCENT_POLICY policy = { ACCENT_ENABLE_ACRYLICBLURBEHIND, 0, accentColor, 0 };
            WINDOWCOMPOSITIONATTRIBDATA data = { WCA_ACCENT_POLICY, &policy, sizeof(ACCENT_POLICY) };
            SetComp(hwnd, &data);
        }
    }
}

void ShowCapsLockNotify(bool isOn) {
    if (!g_hCapsWindow) return;

    UpdateCapsWindowAppearance(g_hCapsWindow);

    const wchar_t* text = isOn ? L"大写锁定 开启" : L"大写锁定 关闭";
    SetWindowTextW(g_hCapsWindow, text);

    POINT pt;
    GetCursorPos(&pt);
    HMONITOR hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfo(hMonitor, &mi);
    RECT workArea = mi.rcWork;

    int capsWidth = 190; 
    int capsHeight = 60;
    int capsX = workArea.left + (workArea.right - workArea.left - capsWidth) / 2;
    int capsY = workArea.bottom - capsHeight - 50;

    KillTimer(g_hCapsWindow, 101); 
    KillTimer(g_hCapsWindow, 102); 
    KillTimer(g_hCapsWindow, IDT_CAPS_HIDE); 
    if (!IsWindowVisible(g_hCapsWindow) || g_capsAlpha <= 0) {
        g_capsAlpha = 0;
        SetLayeredWindowAttributes(g_hCapsWindow, 0, 0, LWA_ALPHA);
    }
    // 使用 SWP_NOACTIVATE 确保即使调用 ShowWindow 也不会抢占游戏焦点
    SetWindowPos(g_hCapsWindow, HWND_TOPMOST, capsX, capsY, capsWidth, capsHeight, SWP_SHOWWINDOW | SWP_NOACTIVATE);
    InvalidateRect(g_hCapsWindow, NULL, TRUE);
    UpdateWindow(g_hCapsWindow);

    SetTimer(g_hCapsWindow, 101, 15, NULL);
    SetTimer(g_hCapsWindow, IDT_CAPS_HIDE, 2000, NULL);
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

    MediaState state;
    {
        lock_guard<mutex> guard(g_MediaState.lock);
        state.title = g_MediaState.title;
        state.artist = g_MediaState.artist;
        state.albumArt = g_MediaState.albumArt ? g_MediaState.albumArt->Clone() : nullptr;
        state.hasMedia = g_MediaState.hasMedia;
        state.isPlaying = g_MediaState.isPlaying;
        state.isDarkCover = g_MediaState.isDarkCover;
    }

    // --- Blurred Background ---
    {
        lock_guard<mutex> guard(g_MediaState.lock);
        if (g_MediaState.blurredBg) {
            graphics.DrawImage(g_MediaState.blurredBg, 0, 0);
        }
    }

    // --- 动态 Mask 适应 ---
    // 如果开启了 Mask，为了保证浅色封面时的深色字可见，当判定为浅色封面时，Mask 会变成白色半透明
    if (g_Settings.enableMask) {
        BYTE maskTone = (!state.hasMedia || state.isDarkCover) ? 20 : 235;
        SolidBrush maskBrush(Color((BYTE)g_Settings.maskOpacity, maskTone, maskTone, maskTone));
        graphics.FillRectangle(&maskBrush, 0, 0, width, height);
    }

    // --- 动态字体与控件颜色 ---
    Color mainColor;
    if (state.hasMedia && state.albumArt) {
        // 如果是深色封面，用白色；如果是浅色封面，用接近黑色的深灰，增加对比度
        mainColor = state.isDarkCover ? Color(255, 255, 255, 255) : Color(255, 30, 30, 30);
    } else {
        // 没播放媒体时恢复系统默认或设置颜色
        mainColor = Color(GetCurrentTextColor());
    }

    // 1. Album Art (Rounded) OR Music Icon
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

        Color logoColor = Color(200, mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue());
        float padding = artSize * 0.22f;
        DrawMusicIcon(graphics, (float)artX + padding, (float)artY + padding, (float)artSize - padding * 2.0f, logoColor);
    }

    // 2. Controls and Text
    if (width > height + 10) {
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
    DWORD, DWORD) {
    if (!IsTaskbarWindow(hwnd) || !g_hMediaWindow) return;
    PostMessage(g_hMediaWindow, WM_APP + 10, 0, 0);
}

// Register Event Hook scoped to Taskbar Thread
void RegisterTaskbarHook(HWND hwnd){
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

// --- Window Procedure ---
#define IDT_POLL_MEDIA 1001
#define IDT_ANIMATION  1002
#define APP_WM_CLOSE   WM_APP

LRESULT CALLBACK MediaWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: 
            UpdateAppearance(hwnd); 
            SetTimer(hwnd, IDT_POLL_MEDIA, 1000, NULL); 
            SetTimer(hwnd, IDT_CAPSLOCK_POLL, 200, NULL);
            RegisterTaskbarHook(hwnd);
            UpdateBlurredBackground(hwnd);
            return 0;

        case WM_SIZE:
        case WM_UPDATE_BLUR:
            UpdateBlurredBackground(hwnd);
            InvalidateRect(hwnd, NULL, FALSE);
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
            {
                lock_guard<mutex> guard(g_MediaState.lock);
                if (g_MediaState.blurredBg) {
                    delete g_MediaState.blurredBg;
                    g_MediaState.blurredBg = nullptr;
                }
            }
            g_SessionManager = nullptr;
            PostQuitMessage(0);
            return 0;

        case WM_SETTINGCHANGE:
            UpdateAppearance(hwnd);
            UpdateBlurredBackground(hwnd);
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;

        case WM_TIMER:
            if (wParam == IDT_POLL_MEDIA) {
                UpdateMediaInfo();
                
                bool shouldHide = false;

                if (g_Settings.hideFullscreen) {
                    QUERY_USER_NOTIFICATION_STATE state;
                    if (SUCCEEDED(SHQueryUserNotificationState(&state))) {
                        if (state == QUNS_BUSY || state == QUNS_RUNNING_D3D_FULL_SCREEN || state == QUNS_PRESENTATION_MODE) {
                            shouldHide = true;
                        }
                    }
                }

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

                if (shouldHide && IsWindowVisible(hwnd)) {
                    ShowWindow(hwnd, SW_HIDE);
                } else if (!shouldHide && !IsWindowVisible(hwnd)) {
                    HWND hTaskbar = FindWindow(L"Shell_TrayWnd", nullptr);
                    if (hTaskbar && IsWindowVisible(hTaskbar)) {
                        ShowWindow(hwnd, SW_SHOWNOACTIVATE);
                    }
                }
                
                PostMessage(hwnd, WM_APP + 10, 0, 0);

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
            else if (wParam == IDT_CAPSLOCK_POLL) {
                bool currentState = (GetKeyState(VK_CAPITAL) & 1) != 0;
                if (currentState != g_LastCapsState) {
                    g_LastCapsState = currentState;
                    ShowCapsLockNotify(currentState);
                }
            }
            return 0;

        case WM_APP + 10: {
            HWND hTaskbar = FindWindow(TEXT("Shell_TrayWnd"), nullptr);
            if (!hTaskbar) break;

            if (!IsWindowVisible(hTaskbar)) {
                if (IsWindowVisible(hwnd)) ShowWindow(hwnd, SW_HIDE);
                return 0;
            }

            if (!g_IsHiddenByIdle && !IsWindowVisible(hwnd)) {
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

            int targetWidth = g_Settings.width;
            {
                lock_guard<mutex> guard(g_MediaState.lock);
                if (!g_MediaState.hasMedia) {
                    targetWidth = g_Settings.height;
                }
            }

            int x = rc.left + g_Settings.offsetX;
            int taskbarHeight = rc.bottom - rc.top;
            int y = rc.top + (taskbarHeight / 2) - (g_Settings.height / 2) + g_Settings.offsetY;
            
            RECT myRc; GetWindowRect(hwnd, &myRc);
            if (myRc.left != x || myRc.top != y || 
                (myRc.right - myRc.left) != targetWidth || 
                (myRc.bottom - myRc.top) != g_Settings.height) {
                    SetWindowPos(
                        hwnd,
                        HWND_TOPMOST,
                        x, y,
                        targetWidth,
                        g_Settings.height,
                        SWP_NOACTIVATE
                    );
            }
            return 0;
        }

        case WM_MOUSEMOVE: {
            RECT rcClient;
            GetClientRect(hwnd, &rcClient);
            int newState = 0;

            if (rcClient.right > g_Settings.height + 10) {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                int artSize = g_Settings.height - 12;
                double scale = g_Settings.buttonScale;
                
                int startControlX = 6 + artSize + (int)(12 * scale);
                float gap = 28.0f * (float)scale;
                float pX = (float)startControlX;
                float plX = pX + gap;
                float nX = plX + gap;
                float radius = 12.0f * (float)scale;

                if (y > 10 && y < g_Settings.height - 10) {
                    if (x >= pX - radius && x <= pX + radius) newState = 1;
                    else if (x >= plX - radius && x <= plX + radius) newState = 2;
                    else if (x >= nX - radius && x <= nX + radius) newState = 3;
                }
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
        default:
            if (msg == g_TaskbarCreatedMsg) {
                if (g_TaskbarHook) {
                    UnhookWinEvent(g_TaskbarHook);
                    g_TaskbarHook = nullptr;
                }
                RegisterTaskbarHook(hwnd);
                return 0;
            }
            break;
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

    WNDCLASS wcCaps = {0};
    wcCaps.lpfnWndProc = CapsWndProc;
    wcCaps.hInstance = wc.hInstance;
    wcCaps.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcCaps.lpszClassName = L"CapsLockNotify";
    RegisterClass(&wcCaps);

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
    }

    if (!g_hMediaWindow) {
        g_hMediaWindow = CreateWindowEx(
            WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
            wc.lpszClassName, TEXT("MusicLounge"),
            WS_POPUP | WS_VISIBLE,
            0, 0, g_Settings.width, g_Settings.height,
            NULL, NULL, wc.hInstance, NULL
        );
    }

    SetLayeredWindowAttributes(g_hMediaWindow, 0, 255, LWA_ALPHA);
    
    int capsWidth = 300;
    int capsHeight = 100;
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int capsX = (screenWidth - capsWidth) / 2;
    int capsY = screenHeight - capsHeight - 50;

    // 添加了 WS_EX_NOACTIVATE (不夺取焦点) 和 WS_EX_TRANSPARENT (点击穿透)，游戏体验拉满！
    g_hCapsWindow = CreateWindowEx(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT,
        L"CapsLockNotify", L"",
        WS_POPUP,
        capsX, capsY, capsWidth, capsHeight,
        NULL, NULL, wc.hInstance, NULL
    );
    
    UpdateCapsWindowAppearance(g_hCapsWindow);

    if (g_hCapsWindow) {
        HMODULE hUser = GetModuleHandle(L"user32.dll");
        if (hUser) {
            auto SetComp = (pSetWindowCompositionAttribute)GetProcAddress(hUser, "SetWindowCompositionAttribute");
            if (SetComp) {
                ACCENT_POLICY policy = { ACCENT_ENABLE_ACRYLICBLURBEHIND, 0, 0xCC000000, 0 };
                WINDOWCOMPOSITIONATTRIBDATA data = { WCA_ACCENT_POLICY, &policy, sizeof(ACCENT_POLICY) };
                SetComp(g_hCapsWindow, &data);
            }
        }
        DWM_WINDOW_CORNER_PREFERENCE preference = DWMWCP_ROUND;
        DwmSetWindowAttribute(g_hCapsWindow, DWMWA_WINDOW_CORNER_PREFERENCE, &preference, sizeof(preference));
        ShowWindow(g_hCapsWindow, SW_HIDE);
    }
    
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnregisterClass(L"CapsLockNotify", wc.hInstance);
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
    if (g_hCapsWindow) DestroyWindow(g_hCapsWindow);
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
// Windhawk tool mod implementation

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
