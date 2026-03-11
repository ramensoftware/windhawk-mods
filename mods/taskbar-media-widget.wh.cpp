// ==WindhawkMod==
// @id              taskbar-media-widget
// @name            Taskbar Media Widget
// @description     A customizable taskbar media widget with transparency, alignment, and media controls.
// @version         1.1
// @author          kevinoe
// @github          https://github.com/kevinoes
// @include         explorer.exe
// @compilerOptions -lole32 -ldwmapi -lgdi32 -luser32 -lwindowsapp -lshcore -lgdiplus
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Media Widget

A customizable taskbar media widget for Windows that shows the current track and lets you control playback directly from the taskbar.

This mod is based on the original **Taskbar Music Lounge** by **Hashah2311**, but has been extensively redesigned.

Unlike the original version, this widget uses custom layered rendering instead of Windows acrylic styling, allowing configurable transparency, alignment, corner radius, font weight, album art visibility, and scrolling behavior.

## ✨ Features
* **Universal Support:** Works with any media player that exposes Windows media sessions through GSMTC.
* **Media Controls:** Previous, Play/Pause, and Next buttons directly in the widget.
* **Volume Control:** Scroll over the widget to adjust system volume.
* **Custom Appearance:** Configure panel size, font size, font weight, corner radius, text color, and background color.
* **Real Transparency:** Uses layered window rendering for proper alpha transparency.
* **Flexible Layout:** Supports left, center, or right taskbar alignment with adjustable X/Y offsets.
* **Album Art Toggle:** Show or hide album artwork.
* **Scrolling Text Toggle:** Enable or disable scrolling for long track titles.
* **Auto Hide:** Hides automatically when no active media session is available.

## ⚠️ Notes
* **Disable Widgets:** Taskbar Settings -> Widgets -> Off.
* **Best on Windows 11:** Designed for the Windows 11 taskbar environment.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- PanelWidth: 300
  $name: Panel Width
- PanelHeight: 36
  $name: Panel Height
- FontSize: 11
  $name: Font Size
- FontWeight: 0
  $name: Font Weight
  $description: "0 = Regular, 1 = Semibold, 2 = Bold"
- ScrollLongText: true
  $name: Scroll Long Text
- HorizontalAlignment: 1
  $name: Horizontal Alignment
  $description: "0 = Left, 1 = Center, 2 = Right"
- OffsetX: 12
  $name: X Offset
- OffsetY: 0
  $name: Y Offset
- CornerRadius: 0
  $name: Corner Radius
  $description: "0 = square corners. Maximum useful value is half the panel height."
- TextColor: "255,255,255,1"
  $name: Text Color (RGBA)
  $description: "Format R,G,B,A. Example: 255,255,255,0.5 for 50% transparent white."
- BackgroundColor: "32,32,32,0.85"
  $name: Background Color (RGBA)
  $description: "Format R,G,B,A. Example: 32,32,32,0.85"
- ShowAlbumArt: true
  $name: Show Album Art
*/
// ==/WindhawkModSettings==

#include <windows.h>
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
#include <stdint.h>

// WinRT
#include <winrt/Windows.Foundation.h>
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

typedef BOOL(WINAPI* pSetWindowBand)(HWND hWnd, HWND hwndInsertAfter, DWORD dwBand);
typedef BOOL(WINAPI* pGetWindowBand)(HWND hWnd, PDWORD pdwBand);

// --- Configurable State ---
struct ModSettings {
    int width = 300;
    int height = 36;
    int fontSize = 10;
    int offsetX = 12;
    int offsetY = 0;
    int fontWeight = 0;
    int cornerRadius = 0;
    int horizontalAlignment = 1;
    bool scrollLongText = true;
    DWORD textColor = 0xFFFFFFFF;
    DWORD backgroundColor = 0xD8202020;
    bool showAlbumArt = true;
} g_Settings;

// --- Global State ---
HWND g_hMediaWindow = NULL;
bool g_Running = true; 
int g_HoverState = 0; 

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

// Read RGBA
DWORD ParseRgbaColorSetting(PCWSTR value, DWORD fallback) {
    if (!value || !*value) {
        return fallback;
    }

    int r = 255, g = 255, b = 255;
    float aFloat = 1.0f;

    // Accept format: R,G,B,A
    // Example: 255,255,255,0.5
    if (swscanf_s(value, L"%d,%d,%d,%f", &r, &g, &b, &aFloat) == 4) {
        if (r < 0) r = 0; if (r > 255) r = 255;
        if (g < 0) g = 0; if (g > 255) g = 255;
        if (b < 0) b = 0; if (b > 255) b = 255;

        int a = 255;

        // Support both 0-1 and 0-255 alpha
        if (aFloat <= 1.0f) {
            if (aFloat < 0.0f) aFloat = 0.0f;
            a = (int)(aFloat * 255.0f + 0.5f);
        } else {
            if (aFloat < 0.0f) aFloat = 0.0f;
            if (aFloat > 255.0f) aFloat = 255.0f;
            a = (int)(aFloat + 0.5f);
        }

        return ((DWORD)a << 24) | ((DWORD)r << 16) | ((DWORD)g << 8) | (DWORD)b;
    }

    return fallback;
}

// --- Settings ---
void LoadSettings() {
    g_Settings.width = Wh_GetIntSetting(L"PanelWidth");
    g_Settings.height = Wh_GetIntSetting(L"PanelHeight");
    g_Settings.fontSize = Wh_GetIntSetting(L"FontSize");
    g_Settings.offsetX = Wh_GetIntSetting(L"OffsetX");
    g_Settings.offsetY = Wh_GetIntSetting(L"OffsetY");
    g_Settings.scrollLongText = Wh_GetIntSetting(L"ScrollLongText") != 0;
    g_Settings.cornerRadius = Wh_GetIntSetting(L"CornerRadius");
    g_Settings.showAlbumArt = Wh_GetIntSetting(L"ShowAlbumArt") != 0;
    g_Settings.fontWeight = Wh_GetIntSetting(L"FontWeight");
    if (g_Settings.fontWeight < 0) g_Settings.fontWeight = 0;
    if (g_Settings.fontWeight > 2) g_Settings.fontWeight = 2;
    g_Settings.horizontalAlignment = Wh_GetIntSetting(L"HorizontalAlignment");
    if (g_Settings.horizontalAlignment < 0) g_Settings.horizontalAlignment = 0;
    if (g_Settings.horizontalAlignment > 2) g_Settings.horizontalAlignment = 2;

    PCWSTR textColorStr = Wh_GetStringSetting(L"TextColor");
    g_Settings.textColor = ParseRgbaColorSetting(textColorStr, 0xFFFFFFFF);
    if (textColorStr) {
        Wh_FreeStringSetting(textColorStr);
    }

    PCWSTR backgroundColorStr = Wh_GetStringSetting(L"BackgroundColor");
    g_Settings.backgroundColor = ParseRgbaColorSetting(backgroundColorStr, 0xD8202020);
    if (backgroundColorStr) {
        Wh_FreeStringSetting(backgroundColorStr);
    }

    if (g_Settings.width < 100) g_Settings.width = 300;
    if (g_Settings.height < 24) g_Settings.height = 36;
    if (g_Settings.fontSize < 6) g_Settings.fontSize = 11;
    if (g_Settings.cornerRadius < 0) g_Settings.cornerRadius = 0;
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

bool UpdateMediaInfo() {
    bool changed = false;

    try {
        if (!g_SessionManager) {
            g_SessionManager = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
        }
        if (!g_SessionManager) {
            return false;
        }

        auto session = g_SessionManager.GetCurrentSession();
        if (session) {
            auto props = session.TryGetMediaPropertiesAsync().get();
            auto info = session.GetPlaybackInfo();

            wstring newTitle = props.Title().c_str();
            wstring newArtist = props.Artist().c_str();
            bool newIsPlaying =
                (info.PlaybackStatus() == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing);

            auto thumbRef = props.Thumbnail();
            bool newHasThumbnail = (thumbRef != nullptr);

            lock_guard<mutex> guard(g_MediaState.lock);

            bool oldHasMedia = g_MediaState.hasMedia;
            bool oldIsPlaying = g_MediaState.isPlaying;
            bool oldHasThumbnail = (g_MediaState.albumArt != nullptr);

            changed =
                !oldHasMedia ||
                (g_MediaState.title != newTitle) ||
                (g_MediaState.artist != newArtist) ||
                (oldIsPlaying != newIsPlaying) ||
                (oldHasThumbnail != newHasThumbnail);

            if ((g_MediaState.title != newTitle) || (oldHasThumbnail != newHasThumbnail)) {
                if (g_MediaState.albumArt) {
                    delete g_MediaState.albumArt;
                    g_MediaState.albumArt = nullptr;
                }

                if (thumbRef) {
                    auto stream = thumbRef.OpenReadAsync().get();
                    g_MediaState.albumArt = StreamToBitmap(stream);
                }
            }

            if (g_MediaState.title != newTitle) {
                g_ScrollOffset = 0;
                g_ScrollWait = 60;
            }

            g_MediaState.title = newTitle;
            g_MediaState.artist = newArtist;
            g_MediaState.isPlaying = newIsPlaying;
            g_MediaState.hasMedia = true;
        } else {
            lock_guard<mutex> guard(g_MediaState.lock);

            changed =
                g_MediaState.hasMedia ||
                (g_MediaState.title != L"No Media") ||
                !g_MediaState.artist.empty() ||
                (g_MediaState.albumArt != nullptr);

            g_MediaState.hasMedia = false;
            g_MediaState.title = L"No Media";
            g_MediaState.artist = L"";
            if (g_MediaState.albumArt) {
                delete g_MediaState.albumArt;
                g_MediaState.albumArt = nullptr;
            }
        }
    } catch (...) {
        lock_guard<mutex> guard(g_MediaState.lock);

        changed =
            g_MediaState.hasMedia ||
            (g_MediaState.title != L"No Media") ||
            !g_MediaState.artist.empty() ||
            (g_MediaState.albumArt != nullptr);

        g_MediaState.hasMedia = false;
        g_MediaState.title = L"No Media";
        g_MediaState.artist = L"";
        if (g_MediaState.albumArt) {
            delete g_MediaState.albumArt;
            g_MediaState.albumArt = nullptr;
        }
    }

    return changed;
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

DWORD GetCurrentTextColor() {
    return g_Settings.textColor;
}

void UpdateAppearance(HWND hwnd) {
    DWM_WINDOW_CORNER_PREFERENCE preference = DWMWCP_DONOTROUND;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &preference, sizeof(preference));

    HMODULE hUser = GetModuleHandle(L"user32.dll");
    if (hUser) {
        auto SetComp = (pSetWindowCompositionAttribute)GetProcAddress(hUser, "SetWindowCompositionAttribute");
        if (SetComp) {
            ACCENT_POLICY policy = { ACCENT_DISABLED, 0, 0, 0 };
            WINDOWCOMPOSITIONATTRIBDATA data = { WCA_ACCENT_POLICY, &policy, sizeof(ACCENT_POLICY) };
            SetComp(hwnd, &data);
        }
    }
}

void CreateRoundedRectPath(GraphicsPath& path, int x, int y, int width, int height, int radius) {
    path.Reset();

    if (radius <= 0) {
        path.AddRectangle(Rect(x, y, width, height));
        return;
    }

    int maxRadius = min(width, height) / 2;
    if (radius > maxRadius) radius = maxRadius;

    int d = radius * 2;

    path.AddArc(x, y, d, d, 180.0f, 90.0f);
    path.AddArc(x + width - d, y, d, d, 270.0f, 90.0f);
    path.AddArc(x + width - d, y + height - d, d, d, 0.0f, 90.0f);
    path.AddArc(x, y + height - d, d, d, 90.0f, 90.0f);
    path.CloseFigure();
}

void DrawMediaPanel(Graphics& graphics, int width, int height) {
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);
    graphics.Clear(Color(0, 0, 0, 0));

    int dynamicSidePadding = max(0, min(6, (height - 36) / 8));
    int leftPaddingNoArt = 11 + dynamicSidePadding;
    int rightPadding = 10 + dynamicSidePadding;

    BYTE bgA = (g_Settings.backgroundColor >> 24) & 0xFF;
    BYTE bgR = (g_Settings.backgroundColor >> 16) & 0xFF;
    BYTE bgG = (g_Settings.backgroundColor >> 8) & 0xFF;
    BYTE bgB = g_Settings.backgroundColor & 0xFF;

    int radius = g_Settings.cornerRadius;
    int maxRadius = min(width, height) / 2;
    if (radius < 0) radius = 0;
    if (radius > maxRadius) radius = maxRadius;

    GraphicsPath panelPath;
    CreateRoundedRectPath(panelPath, 0, 0, width, height, radius);

    graphics.SetClip(&panelPath);
    SolidBrush bgBrush(Color(bgA, bgR, bgG, bgB));
    graphics.FillPath(&bgBrush, &panelPath);

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

    int contentLeft = leftPaddingNoArt;

    if (g_Settings.showAlbumArt) {
        int artSize = height - 12;
        int artX = 6;
        int artY = 6;
        int artRadius = max(0, g_Settings.cornerRadius - artX);

        GraphicsPath artPath;
        CreateRoundedRectPath(artPath, artX, artY, artSize, artSize, artRadius);

        GraphicsState artState = graphics.Save();
        graphics.SetClip(&artPath);

        if (state.albumArt) {
            graphics.DrawImage(state.albumArt, artX, artY, artSize, artSize);
        } else {
            SolidBrush placeBrush{Color(40, 128, 128, 128)};
            graphics.FillPath(&placeBrush, &artPath);
        }

        graphics.Restore(artState);

        contentLeft = artX + artSize + 12;
    } else {
        contentLeft = leftPaddingNoArt;
    }

    if (state.albumArt) {
        delete state.albumArt;
        state.albumArt = nullptr;
    }

    int startControlX = contentLeft;
    int controlY = height / 2;

    SolidBrush iconBrush{mainColor};
    SolidBrush hoverBrush{Color(
        (BYTE)((g_Settings.textColor >> 24) & 0xFF),
        mainColor.GetRed(),
        mainColor.GetGreen(),
        mainColor.GetBlue()
    )};
    SolidBrush activeBg{Color(40, mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue())};

    // Prev
    int pX = startControlX;
    if (g_HoverState == 1)
        graphics.FillEllipse(&activeBg, pX - 8, controlY - 12, 24, 24);

    Point prevPts[3] = {
        Point(pX + 8, controlY - 6),
        Point(pX + 8, controlY + 6),
        Point(pX, controlY)
    };
    graphics.FillPolygon(g_HoverState == 1 ? &hoverBrush : &iconBrush, prevPts, 3);
    graphics.FillRectangle(g_HoverState == 1 ? &hoverBrush : &iconBrush, pX, controlY - 6, 2, 12);

    // Play/Pause
    int plX = startControlX + 28;
    if (g_HoverState == 2)
        graphics.FillEllipse(&activeBg, plX - 8, controlY - 12, 24, 24);

    if (state.isPlaying) {
        graphics.FillRectangle(g_HoverState == 2 ? &hoverBrush : &iconBrush, plX, controlY - 7, 3, 14);
        graphics.FillRectangle(g_HoverState == 2 ? &hoverBrush : &iconBrush, plX + 6, controlY - 7, 3, 14);
    } else {
        Point playPts[3] = {
            Point(plX, controlY - 8),
            Point(plX, controlY + 8),
            Point(plX + 10, controlY)
        };
        graphics.FillPolygon(g_HoverState == 2 ? &hoverBrush : &iconBrush, playPts, 3);
    }

    // Next
    int nX = startControlX + 56;
    if (g_HoverState == 3)
        graphics.FillEllipse(&activeBg, nX - 8, controlY - 12, 24, 24);

    Point nextPts[3] = {
        Point(nX, controlY - 6),
        Point(nX, controlY + 6),
        Point(nX + 8, controlY)
    };
    graphics.FillPolygon(g_HoverState == 3 ? &hoverBrush : &iconBrush, nextPts, 3);
    graphics.FillRectangle(g_HoverState == 3 ? &hoverBrush : &iconBrush, nX + 8, controlY - 6, 2, 12);

    // Text
    int textX = nX + 20;
    int textMaxW = width - textX - rightPadding;

    wstring fullText = state.title;
    if (!state.artist.empty()) {
        fullText += L" • " + state.artist;
    }

    const WCHAR* fontName = L"Segoe UI";
    INT fontStyle = FontStyleRegular;

    if (g_Settings.fontWeight == 1) {
        fontName = L"Segoe UI Semibold";
    }
    else if (g_Settings.fontWeight == 2) {
        fontStyle = FontStyleBold;
    }

    FontFamily fontFamily(fontName, nullptr);
    Font font(&fontFamily, (REAL)g_Settings.fontSize, fontStyle, UnitPixel);
    SolidBrush textBrush{mainColor};

    RectF layoutRect(0, 0, 2000, 100);
    RectF boundRect;
    graphics.MeasureString(fullText.c_str(), -1, &font, layoutRect, &boundRect);
    g_TextWidth = (int)boundRect.Width;

    GraphicsState saved = graphics.Save();
    Region textClip(Rect(textX, 0, textMaxW, height));
    graphics.SetClip(&textClip, CombineModeIntersect);

    float textY = (height - boundRect.Height) / 2.0f;

    if (g_TextWidth > textMaxW && g_Settings.scrollLongText) {
        g_IsScrolling = true;
        float drawX = (float)(textX - g_ScrollOffset);
        graphics.DrawString(fullText.c_str(), -1, &font, PointF(drawX, textY), &textBrush);

        if (drawX + g_TextWidth < width) {
            graphics.DrawString(
                fullText.c_str(),
                -1,
                &font,
                PointF(drawX + g_TextWidth + 40, textY),
                &textBrush
            );
        }
    } else {
        g_IsScrolling = false;
        g_ScrollOffset = 0;
        graphics.DrawString(fullText.c_str(), -1, &font, PointF((float)textX, textY), &textBrush);
    }

    graphics.Restore(saved);
}

// --- Window Procedure ---
#define IDT_POLL_MEDIA 1001
#define IDT_ANIMATION  1002
#define APP_WM_CLOSE   WM_APP

void RenderLayeredWindow(HWND hwnd) {
    RECT rc;
    GetWindowRect(hwnd, &rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;

    if (width <= 0 || height <= 0) {
        return;
    }

    HDC screenDC = GetDC(nullptr);
    HDC memDC = CreateCompatibleDC(screenDC);

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height; // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP hBitmap = CreateDIBSection(screenDC, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!hBitmap || !bits) {
        if (hBitmap) DeleteObject(hBitmap);
        DeleteDC(memDC);
        ReleaseDC(nullptr, screenDC);
        return;
    }

    HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, hBitmap);

    {
        Graphics graphics(memDC);
        DrawMediaPanel(graphics, width, height);
    }

    // Start/stop scrolling timer based on latest measured text state
    if (g_IsScrolling) {
        SetTimer(hwnd, IDT_ANIMATION, 16, NULL);
    } else {
        KillTimer(hwnd, IDT_ANIMATION);
        g_ScrollOffset = 0;
        g_ScrollWait = 60;
    }

    POINT ptSrc = { 0, 0 };
    SIZE sizeWnd = { width, height };
    POINT ptDst = { rc.left, rc.top };

    BLENDFUNCTION blend = {};
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;

    UpdateLayeredWindow(hwnd, screenDC, &ptDst, &sizeWnd, memDC, &ptSrc, 0, &blend, ULW_ALPHA);

    SelectObject(memDC, oldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(memDC);
    ReleaseDC(nullptr, screenDC);
}

LRESULT CALLBACK MediaWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: 
            UpdateAppearance(hwnd);
            ShowWindow(hwnd, SW_HIDE);
            SetTimer(hwnd, IDT_POLL_MEDIA, 1000, NULL);
            return 0;

        case WM_ERASEBKGND: 
            return 1;

        case WM_CLOSE:
            return 0;

        case APP_WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            g_SessionManager = nullptr;
            PostQuitMessage(0);
            return 0;

        case WM_SETTINGCHANGE:
            UpdateAppearance(hwnd);
            if (IsWindowVisible(hwnd)) {
                RenderLayeredWindow(hwnd);
            }
            return 0;

        case WM_TIMER:
            if (wParam == IDT_POLL_MEDIA) {
                bool mediaChanged = UpdateMediaInfo();

                bool hasMedia = false;
                {
                    lock_guard<mutex> guard(g_MediaState.lock);
                    hasMedia = g_MediaState.hasMedia;
                }

                bool isVisible = IsWindowVisible(hwnd) != FALSE;
                bool needsRender = mediaChanged;

                if (!hasMedia) {
                    if (isVisible) {
                        ShowWindow(hwnd, SW_HIDE);
                    }

                    if (g_IsScrolling) {
                        KillTimer(hwnd, IDT_ANIMATION);
                        g_IsScrolling = false;
                        g_ScrollOffset = 0;
                        g_ScrollWait = 60;
                    }

                    return 0;
                }

                if (!isVisible) {
                    ShowWindow(hwnd, SW_SHOWNOACTIVATE);
                    needsRender = true;
                }

                HWND hTaskbar = FindWindow(TEXT("Shell_TrayWnd"), NULL);
                if (hTaskbar) {
                    RECT rc;
                    GetWindowRect(hTaskbar, &rc);

                    int taskbarWidth = rc.right - rc.left;
                    int taskbarHeight = rc.bottom - rc.top;

                    int x = rc.left + g_Settings.offsetX;
                    if (g_Settings.horizontalAlignment == 1) {
                        x = rc.left + (taskbarWidth - g_Settings.width) / 2 + g_Settings.offsetX;
                    } else if (g_Settings.horizontalAlignment == 2) {
                        x = rc.right - g_Settings.width - g_Settings.offsetX;
                    }

                    int y = rc.top + (taskbarHeight / 2) - (g_Settings.height / 2) + g_Settings.offsetY;

                    RECT myRc;
                    GetWindowRect(hwnd, &myRc);

                    if (myRc.left != x || myRc.top != y ||
                        (myRc.right - myRc.left) != g_Settings.width ||
                        (myRc.bottom - myRc.top) != g_Settings.height) {
                        SetWindowPos(hwnd, HWND_TOPMOST, x, y, g_Settings.width, g_Settings.height, SWP_NOACTIVATE);
                        needsRender = true;
                    }
                }

                if (needsRender) {
                    RenderLayeredWindow(hwnd);
                }
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
                        RenderLayeredWindow(hwnd);
                    }
                } else {
                    KillTimer(hwnd, IDT_ANIMATION); 
                }
            }
            return 0;

        case WM_MOUSEMOVE: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            int startControlX = 11;
            if (g_Settings.showAlbumArt) {
                int artSize = g_Settings.height - 12;
                startControlX = 6 + artSize + 12;
            }
            int newState = 0;
            
            if (y > 10 && y < g_Settings.height - 10) {
                if (x >= startControlX - 10 && x < startControlX + 14) newState = 1;
                else if (x >= startControlX + 14 && x < startControlX + 42) newState = 2;
                else if (x >= startControlX + 42 && x < startControlX + 66) newState = 3;
            }
            
            if (newState != g_HoverState) {
                g_HoverState = newState;
                RenderLayeredWindow(hwnd);
            }
            
            TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, hwnd, 0 };
            TrackMouseEvent(&tme);
            return 0;
        }
        case WM_MOUSELEAVE:
            if (g_HoverState != 0) {
                g_HoverState = 0;
                RenderLayeredWindow(hwnd);
            }
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
            BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            RenderLayeredWindow(hwnd);
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

    // Try to use CreateWindowInBand for proper z-ordering
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

    // Fallback to CreateWindowEx if CreateWindowInBand failed or unavailable
    if (!g_hMediaWindow) {
        Wh_Log(L"CreateWindowInBand failed or unavailable, falling back to CreateWindowEx");
        g_hMediaWindow = CreateWindowEx(
            WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
            wc.lpszClassName, TEXT("MusicLounge"),
            WS_POPUP | WS_VISIBLE,
            0, 0, g_Settings.width, g_Settings.height,
            NULL, NULL, wc.hInstance, NULL
        );
    }

    //SetLayeredWindowAttributes(g_hMediaWindow, 0, 255, LWA_ALPHA);
    
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
