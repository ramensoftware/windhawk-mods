// ==WindhawkMod==
// @id              taskbar-spotify-widget
// @name            Taskbar Spotify Widget
// @description     A native-style Spotify widget for the taskbar. Windows 10/11 
// @version         1.0
// @author          memeri121
// @include         explorer.exe
// @compilerOptions -lole32 -ldwmapi -lgdi32 -luser32 -lwindowsapp -lshcore -lgdiplus -lshell32 -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
A modern Spotify widget directly on the Windows taskbar.

Shows the current track, album art, playback controls, and a smooth animated seekbar.
Supports track seeking, hover tooltips with timestamps, compact mode, customizable layout, and adjustable background transparency.

Designed to feel native to Windows with smooth animations and a clean UI.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- PanelWidth: 320
  $name: Panel Width
- PanelHeight: 52
  $name: Panel Height
- FontSize: 11
  $name: Font Size
- OffsetX: 12
  $name: X Offset
- OffsetY: 0
  $name: Y Offset
- AutoTheme: true
  $name: Auto Theme
- TextColor: 0xFFFFFF
  $name: Manual Text Color (Hex)
- UseBlackBackground: true
  $name: Use Black Background
- BgOpacity: 200
  $name: Background Opacity (0-255)
- ControlsSide: right
  $name: Controls Side
  $options:
    - left: Left
    - right: Right
- ShowAlbumArt: true
  $name: Show Album Art
- ShowSeekbar: true
  $name: Show Seekbar
- CompactMode: false
  $name: Compact Mode
- OpenSpotifyOnSingleClick: false
  $name: Open Spotify On Single Click
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <dwmapi.h>
#include <gdiplus.h>
#include <shcore.h>
#include <commctrl.h>
#include <string>
#include <vector>
#include <atomic>
#include <thread>
#include <mutex>
#include <cstdio>
#include <algorithm>
#include <cwctype>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>

using namespace Gdiplus;
using namespace std;
using namespace winrt;
using namespace Windows::Media::Control;
using namespace Windows::Storage::Streams;

const WCHAR* FONT_NAME = L"Segoe UI Variable Display";

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

struct ModSettings {
    int width = 320;
    int height = 52;
    int fontSize = 11;
    int offsetX = 12;
    int offsetY = 0;
    bool autoTheme = true;
    DWORD manualTextColor = 0xFFFFFFFF;
    bool useBlackBackground = true;
    int bgOpacity = 200;
    bool controlsLeft = false;
    bool showAlbumArt = true;
    bool showSeekbar = true;
    bool compactMode = false;
    bool openSpotifyOnSingleClick = false;
} g_Settings;

HWND g_hMediaWindow = NULL;
HWND g_hTooltip = NULL;
bool g_Running = true;
int g_HoverState = 0;
bool g_WindowVisible = false;
BYTE g_CurrentAlpha = 0;
BYTE g_TargetAlpha = 0;
bool g_FadeTimerRunning = false;
wstring g_LastRenderedText;
bool g_IsDraggingSeek = false;
bool g_IsSeekHover = false;
double g_DragSeekRatio = 0.0;
float g_SeekbarAnim = 0.0f;
bool g_SeekbarAnimTimerRunning = false;
wstring g_TooltipText;

struct MediaState {
    wstring title = L"";
    wstring artist = L"";
    bool isPlaying = false;
    bool hasMedia = false;
    bool canSeek = false;
    long long startTicks = 0;
    long long endTicks = 0;
    long long positionTicks = 0;
    long long minSeekTicks = 0;
    long long maxSeekTicks = 0;
    Bitmap* albumArt = nullptr;
    mutex lock;
} g_MediaState;

int g_ScrollOffset = 0;
int g_TextWidth = 0;
bool g_IsScrolling = false;
int g_ScrollWait = 75;

GlobalSystemMediaTransportControlsSessionManager g_SessionManager = nullptr;

wstring ToLowerString(wstring s) {
    transform(s.begin(), s.end(), s.begin(), [](wchar_t c) {
        return (wchar_t)towlower(c);
    });
    return s;
}

bool IsCursorOverWindow(HWND hwnd) {
    if (!hwnd) return false;

    POINT pt;
    if (!GetCursorPos(&pt)) return false;

    RECT rc;
    if (!GetWindowRect(hwnd, &rc)) return false;

    return PtInRect(&rc, pt) != 0;
}

void LoadSettings() {
    g_Settings.width = Wh_GetIntSetting(L"PanelWidth");
    g_Settings.height = Wh_GetIntSetting(L"PanelHeight");
    g_Settings.fontSize = Wh_GetIntSetting(L"FontSize");
    g_Settings.offsetX = Wh_GetIntSetting(L"OffsetX");
    g_Settings.offsetY = Wh_GetIntSetting(L"OffsetY");
    g_Settings.autoTheme = Wh_GetIntSetting(L"AutoTheme") != 0;

    PCWSTR textHex = Wh_GetStringSetting(L"TextColor");
    DWORD textRGB = 0xFFFFFF;
    if (textHex) {
        if (wcslen(textHex) > 0) textRGB = wcstoul(textHex, nullptr, 16);
        Wh_FreeStringSetting(textHex);
    }
    g_Settings.manualTextColor = 0xFF000000 | textRGB;

    g_Settings.useBlackBackground = Wh_GetIntSetting(L"UseBlackBackground") != 0;

    g_Settings.bgOpacity = Wh_GetIntSetting(L"BgOpacity");
    if (g_Settings.bgOpacity < 0) g_Settings.bgOpacity = 0;
    if (g_Settings.bgOpacity > 255) g_Settings.bgOpacity = 255;

    PCWSTR controlsSide = Wh_GetStringSetting(L"ControlsSide");
    g_Settings.controlsLeft = false;
    if (controlsSide) {
        wstring v = ToLowerString(controlsSide);
        g_Settings.controlsLeft = (v == L"left");
        Wh_FreeStringSetting(controlsSide);
    }

    g_Settings.showAlbumArt = Wh_GetIntSetting(L"ShowAlbumArt") != 0;
    g_Settings.showSeekbar = Wh_GetIntSetting(L"ShowSeekbar") != 0;
    g_Settings.compactMode = Wh_GetIntSetting(L"CompactMode") != 0;
    g_Settings.openSpotifyOnSingleClick = Wh_GetIntSetting(L"OpenSpotifyOnSingleClick") != 0;

    if (g_Settings.width < 180) g_Settings.width = 320;
    if (g_Settings.height < 40) g_Settings.height = 52;
}

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

bool IsSpotifySession(const GlobalSystemMediaTransportControlsSession& session) {
    if (!session) return false;
    try {
        wstring source = session.SourceAppUserModelId().c_str();
        source = ToLowerString(source);
        if (source.find(L"spotify") != wstring::npos) {
            return true;
        }
    } catch (...) {
    }
    return false;
}

GlobalSystemMediaTransportControlsSession GetSpotifySession() {
    if (!g_SessionManager) return nullptr;
    try {
        auto sessions = g_SessionManager.GetSessions();
        uint32_t count = sessions.Size();
        for (uint32_t i = 0; i < count; i++) {
            auto session = sessions.GetAt(i);
            if (IsSpotifySession(session)) {
                return session;
            }
        }
    } catch (...) {
    }
    return nullptr;
}

void FreeAlbumArtLocked() {
    if (g_MediaState.albumArt) {
        delete g_MediaState.albumArt;
        g_MediaState.albumArt = nullptr;
    }
}

void ResetScrollState() {
    g_IsScrolling = false;
    g_ScrollOffset = 0;
    g_ScrollWait = 75;
}

void ClearMediaState() {
    lock_guard<mutex> guard(g_MediaState.lock);
    g_MediaState.title = L"";
    g_MediaState.artist = L"";
    g_MediaState.isPlaying = false;
    g_MediaState.hasMedia = false;
    g_MediaState.canSeek = false;
    g_MediaState.startTicks = 0;
    g_MediaState.endTicks = 0;
    g_MediaState.positionTicks = 0;
    g_MediaState.minSeekTicks = 0;
    g_MediaState.maxSeekTicks = 0;
    FreeAlbumArtLocked();
}

void EnsureFadeTimer() {
    if (!g_hMediaWindow) return;
    if (g_FadeTimerRunning) return;
    SetTimer(g_hMediaWindow, 1003, 12, NULL);
    g_FadeTimerRunning = true;
}

void StopFadeTimer() {
    if (!g_hMediaWindow) return;
    if (!g_FadeTimerRunning) return;
    KillTimer(g_hMediaWindow, 1003);
    g_FadeTimerRunning = false;
}

void EnsureSeekbarAnimTimer() {
    if (!g_hMediaWindow) return;
    if (g_SeekbarAnimTimerRunning) return;
    SetTimer(g_hMediaWindow, 1004, 16, NULL);
    g_SeekbarAnimTimerRunning = true;
}

void StopSeekbarAnimTimer() {
    if (!g_hMediaWindow) return;
    if (!g_SeekbarAnimTimerRunning) return;
    KillTimer(g_hMediaWindow, 1004);
    g_SeekbarAnimTimerRunning = false;
}

void ApplyLayeredAlpha() {
    if (!g_hMediaWindow) return;
    SetLayeredWindowAttributes(g_hMediaWindow, 0, g_CurrentAlpha, LWA_ALPHA);
}

void SetWidgetVisible(bool visible) {
    if (!g_hMediaWindow) return;
    g_TargetAlpha = visible ? 255 : 0;
    if (visible) {
        if (!g_WindowVisible) {
            g_WindowVisible = true;
            ShowWindow(g_hMediaWindow, SW_SHOWNOACTIVATE);
        }
        EnsureFadeTimer();
    } else {
        EnsureFadeTimer();
    }
}

long long ClampI64(long long v, long long a, long long b) {
    if (v < a) return a;
    if (v > b) return b;
    return v;
}

double ClampDouble(double v, double a, double b) {
    if (v < a) return a;
    if (v > b) return b;
    return v;
}

wstring FormatTimeTicks(long long ticks) {
    if (ticks < 0) ticks = 0;
    long long totalSec = ticks / 10000000LL;
    long long m = totalSec / 60;
    long long s = totalSec % 60;
    wchar_t buf[64];
    swprintf_s(buf, L"%lld:%02lld", m, s);
    return buf;
}

void UpdateMediaInfo() {
    try {
        if (!g_SessionManager) {
            g_SessionManager = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
        }
        if (!g_SessionManager) {
            ClearMediaState();
            return;
        }

        auto session = GetSpotifySession();
        if (!session) {
            ClearMediaState();
            return;
        }

        auto props = session.TryGetMediaPropertiesAsync().get();
        auto info = session.GetPlaybackInfo();
        auto timeline = session.GetTimelineProperties();

        lock_guard<mutex> guard(g_MediaState.lock);

        wstring newTitle = props.Title().c_str();
        wstring newArtist = props.Artist().c_str();
        bool newHasMedia = !newTitle.empty() || !newArtist.empty();

        if (!newHasMedia) {
            g_MediaState.title = L"";
            g_MediaState.artist = L"";
            g_MediaState.isPlaying = false;
            g_MediaState.hasMedia = false;
            g_MediaState.canSeek = false;
            g_MediaState.startTicks = 0;
            g_MediaState.endTicks = 0;
            g_MediaState.positionTicks = 0;
            g_MediaState.minSeekTicks = 0;
            g_MediaState.maxSeekTicks = 0;
            FreeAlbumArtLocked();
            return;
        }

        if (newTitle != g_MediaState.title || newArtist != g_MediaState.artist || g_MediaState.albumArt == nullptr) {
            FreeAlbumArtLocked();
            auto thumbRef = props.Thumbnail();
            if (thumbRef) {
                auto stream = thumbRef.OpenReadAsync().get();
                g_MediaState.albumArt = StreamToBitmap(stream);
            }
        }

        g_MediaState.title = newTitle;
        g_MediaState.artist = newArtist;
        g_MediaState.isPlaying = (info.PlaybackStatus() == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing);
        g_MediaState.hasMedia = true;

        bool canSeek = false;
        long long startTicks = 0;
        long long endTicks = 0;
        long long positionTicks = 0;
        long long minSeekTicks = 0;
        long long maxSeekTicks = 0;

        try {
            canSeek = info.Controls().IsPlaybackPositionEnabled();
            startTicks = timeline.StartTime().count();
            endTicks = timeline.EndTime().count();
            positionTicks = timeline.Position().count();
            minSeekTicks = timeline.MinSeekTime().count();
            maxSeekTicks = timeline.MaxSeekTime().count();
        } catch (...) {
            canSeek = false;
        }

        if (endTicks <= startTicks) {
            canSeek = false;
        }

        if (canSeek) {
            positionTicks = ClampI64(positionTicks, minSeekTicks, maxSeekTicks);
        } else {
            startTicks = 0;
            endTicks = 0;
            positionTicks = 0;
            minSeekTicks = 0;
            maxSeekTicks = 0;
        }

        g_MediaState.canSeek = canSeek;
        g_MediaState.startTicks = startTicks;
        g_MediaState.endTicks = endTicks;
        g_MediaState.positionTicks = positionTicks;
        g_MediaState.minSeekTicks = minSeekTicks;
        g_MediaState.maxSeekTicks = maxSeekTicks;
    } catch (...) {
        ClearMediaState();
    }
}

void SendMediaCommand(int cmd) {
    try {
        if (!g_SessionManager) return;
        auto session = GetSpotifySession();
        if (session) {
            if (cmd == 1) session.TrySkipPreviousAsync();
            else if (cmd == 2) session.TryTogglePlayPauseAsync();
            else if (cmd == 3) session.TrySkipNextAsync();
        }
    } catch (...) {
    }
}

void OpenSpotify() {
    ShellExecuteW(NULL, L"open", L"spotify:", NULL, NULL, SW_SHOWNORMAL);
}

bool IsCompact() {
    return g_Settings.compactMode;
}

int GetVisualMargin() {
    return IsCompact() ? 5 : 6;
}

RECT GetArtRect() {
    RECT rc{};
    if (!g_Settings.showAlbumArt) return rc;

    int margin = GetVisualMargin();
    int artSize = g_Settings.height - margin * 2;
    rc.left = margin;
    rc.top = margin;
    rc.right = rc.left + artSize;
    rc.bottom = rc.top + artSize;
    return rc;
}

RECT GetControlsRect() {
    RECT rc{};
    int margin = IsCompact() ? 6 : 8;
    int blockW = IsCompact() ? 74 : 78;
    rc.top = 6;
    rc.bottom = g_Settings.height - 6;

    if (g_Settings.controlsLeft) {
        rc.left = margin;
        rc.right = rc.left + blockW;
    } else {
        rc.right = g_Settings.width - margin;
        rc.left = rc.right - blockW;
    }

    return rc;
}

RECT GetContentRect() {
    RECT rc{};
    int margin = IsCompact() ? 6 : 8;
    rc.left = margin;
    rc.top = 6;
    rc.right = g_Settings.width - margin;
    rc.bottom = g_Settings.height - 6;
    return rc;
}

RECT GetTextRect() {
    RECT content = GetContentRect();
    RECT controls = GetControlsRect();
    RECT art = GetArtRect();
    RECT rc = content;

    int leftGap = 8;
    int rightGap = 8;

    if (g_Settings.controlsLeft) {
        rc.left = controls.right + leftGap;
        if (g_Settings.showAlbumArt) {
            rc.left = max(rc.left, art.right + leftGap);
        }
    } else {
        rc.right = controls.left - rightGap;
        if (g_Settings.showAlbumArt) {
            rc.left = art.right + leftGap;
        }
    }

    if (!g_Settings.showAlbumArt && g_Settings.controlsLeft) {
        rc.left = controls.right + leftGap;
    }

    if (!g_Settings.showAlbumArt && !g_Settings.controlsLeft) {
        rc.left = content.left;
    }

    if (g_Settings.showAlbumArt && g_Settings.controlsLeft) {
        rc.right = content.right;
    }

    if (g_Settings.showAlbumArt && !g_Settings.controlsLeft) {
        rc.left = art.right + leftGap;
    }

    return rc;
}

RECT GetProgressBarRect() {
    RECT text = GetTextRect();
    RECT rc = text;

    if (!g_Settings.showSeekbar) {
        rc.top = rc.bottom = rc.left = rc.right = 0;
        return rc;
    }

    int h = (int)(4.0f + 2.0f * g_SeekbarAnim);
    rc.left = text.left;
    rc.right = text.right;
    rc.bottom = g_Settings.height - (IsCompact() ? 7 : 9);
    rc.top = rc.bottom - h;
    return rc;
}

RECT GetButtonRect(int index) {
    RECT block = GetControlsRect();
    int gap = IsCompact() ? 3 : 4;
    int btnW = IsCompact() ? 21 : 22;
    int btnH = IsCompact() ? 21 : 22;
    int totalW = btnW * 3 + gap * 2;
    int startX = block.left + ((block.right - block.left) - totalW) / 2;
    int y = block.top + ((block.bottom - block.top) - btnH) / 2;
    RECT rc{};
    rc.left = startX + index * (btnW + gap);
    rc.top = y;
    rc.right = rc.left + btnW;
    rc.bottom = rc.top + btnH;
    return rc;
}

double SeekRatioFromX(int x) {
    RECT bar = GetProgressBarRect();
    int w = bar.right - bar.left;
    if (w <= 0) return 0.0;
    return ClampDouble((double)(x - bar.left) / (double)w, 0.0, 1.0);
}

bool SeekToRatio(double ratio) {
    try {
        if (!g_SessionManager) return false;
        auto session = GetSpotifySession();
        if (!session) return false;

        auto info = session.GetPlaybackInfo();
        if (!info.Controls().IsPlaybackPositionEnabled()) return false;

        auto timeline = session.GetTimelineProperties();
        long long startTicks = timeline.StartTime().count();
        long long endTicks = timeline.EndTime().count();
        long long minSeekTicks = timeline.MinSeekTime().count();
        long long maxSeekTicks = timeline.MaxSeekTime().count();

        if (endTicks <= startTicks) return false;

        ratio = ClampDouble(ratio, 0.0, 1.0);

        long double range = (long double)(endTicks - startTicks);
        long long requested = startTicks + (long long)(range * ratio);
        requested = ClampI64(requested, minSeekTicks, maxSeekTicks);

        auto ok = session.TryChangePlaybackPositionAsync(requested).get();
        return ok;
    } catch (...) {
    }
    return false;
}

bool SeekBySeconds(int seconds) {
    try {
        if (!g_SessionManager) return false;
        auto session = GetSpotifySession();
        if (!session) return false;

        auto info = session.GetPlaybackInfo();
        if (!info.Controls().IsPlaybackPositionEnabled()) return false;

        auto timeline = session.GetTimelineProperties();
        long long current = timeline.Position().count();
        long long minSeekTicks = timeline.MinSeekTime().count();
        long long maxSeekTicks = timeline.MaxSeekTime().count();
        long long delta = (long long)seconds * 10000000LL;
        long long requested = ClampI64(current + delta, minSeekTicks, maxSeekTicks);

        auto ok = session.TryChangePlaybackPositionAsync(requested).get();
        return ok;
    } catch (...) {
    }
    return false;
}

bool IsPointInRectInt(int x, int y, const RECT& rc) {
    return x >= rc.left && x < rc.right && y >= rc.top && y < rc.bottom;
}

bool IsSystemLightMode() {
    DWORD value = 0;
    DWORD size = sizeof(value);
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
            if (g_Settings.useBlackBackground) {
                tint = ((DWORD)g_Settings.bgOpacity << 24) | 0x000000;
            } else if (g_Settings.autoTheme) {
                tint = IsSystemLightMode() ? ((DWORD)g_Settings.bgOpacity << 24) | 0xFFFFFF
                                           : ((DWORD)g_Settings.bgOpacity << 24) | 0x000000;
            } else {
                tint = ((DWORD)g_Settings.bgOpacity << 24) | 0x000000;
            }

            ACCENT_POLICY policy = { ACCENT_ENABLE_ACRYLICBLURBEHIND, 0, tint, 0 };
            WINDOWCOMPOSITIONATTRIBDATA data = { WCA_ACCENT_POLICY, &policy, sizeof(ACCENT_POLICY) };
            SetComp(hwnd, &data);
        }
    }
}

void FillRoundedRect(Graphics& graphics, Brush* brush, int x, int y, int w, int h, int r) {
    GraphicsPath path;
    path.AddArc((REAL)x, (REAL)y, (REAL)(r * 2), (REAL)(r * 2), 180.0f, 90.0f);
    path.AddArc((REAL)(x + w - r * 2), (REAL)y, (REAL)(r * 2), (REAL)(r * 2), 270.0f, 90.0f);
    path.AddArc((REAL)(x + w - r * 2), (REAL)(y + h - r * 2), (REAL)(r * 2), (REAL)(r * 2), 0.0f, 90.0f);
    path.AddArc((REAL)x, (REAL)(y + h - r * 2), (REAL)(r * 2), (REAL)(r * 2), 90.0f, 90.0f);
    path.CloseFigure();
    graphics.FillPath(brush, &path);
}

void DrawRoundedImage(Graphics& graphics, Bitmap* bmp, int x, int y, int size, int radius) {
    if (!bmp) return;

    GraphicsPath path;
    path.AddArc((REAL)x, (REAL)y, (REAL)(radius * 2), (REAL)(radius * 2), 180.0f, 90.0f);
    path.AddArc((REAL)(x + size - radius * 2), (REAL)y, (REAL)(radius * 2), (REAL)(radius * 2), 270.0f, 90.0f);
    path.AddArc((REAL)(x + size - radius * 2), (REAL)(y + size - radius * 2), (REAL)(radius * 2), (REAL)(radius * 2), 0.0f, 90.0f);
    path.AddArc((REAL)x, (REAL)(y + size - radius * 2), (REAL)(radius * 2), (REAL)(radius * 2), 90.0f, 90.0f);
    path.CloseFigure();

    GraphicsState state = graphics.Save();
    graphics.SetClip(&path);
    graphics.DrawImage(bmp, x, y, size, size);
    graphics.Restore(state);
}

void DrawButtonBg(Graphics& graphics, const RECT& rc, Color color) {
    SolidBrush brush(color);
    FillRoundedRect(graphics, &brush, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, 7);
}

void DrawPrevIcon(Graphics& graphics, const RECT& rc, Brush* brush) {
    int cx = (rc.left + rc.right) / 2;
    int cy = (rc.top + rc.bottom) / 2;
    Point pts[3] = {
        Point(cx + 3, cy - 5),
        Point(cx + 3, cy + 5),
        Point(cx - 3, cy)
    };
    graphics.FillPolygon(brush, pts, 3);
    graphics.FillRectangle(brush, cx - 5, cy - 5, 2, 10);
}

void DrawPlayPauseIcon(Graphics& graphics, const RECT& rc, Brush* brush, bool playing) {
    int cx = (rc.left + rc.right) / 2;
    int cy = (rc.top + rc.bottom) / 2;
    if (playing) {
        graphics.FillRectangle(brush, cx - 5, cy - 6, 3, 12);
        graphics.FillRectangle(brush, cx + 2, cy - 6, 3, 12);
    } else {
        Point pts[3] = {
            Point(cx - 3, cy - 7),
            Point(cx - 3, cy + 7),
            Point(cx + 6, cy)
        };
        graphics.FillPolygon(brush, pts, 3);
    }
}

void DrawNextIcon(Graphics& graphics, const RECT& rc, Brush* brush) {
    int cx = (rc.left + rc.right) / 2;
    int cy = (rc.top + rc.bottom) / 2;
    Point pts[3] = {
        Point(cx - 3, cy - 5),
        Point(cx - 3, cy + 5),
        Point(cx + 3, cy)
    };
    graphics.FillPolygon(brush, pts, 3);
    graphics.FillRectangle(brush, cx + 3, cy - 5, 2, 10);
}

void HideTooltip() {
    if (!g_hTooltip) return;

    TOOLINFOW ti{};
    ti.cbSize = sizeof(ti);
    ti.uFlags = TTF_TRACK | TTF_ABSOLUTE;
    ti.hwnd = g_hMediaWindow;
    ti.uId = 1;

    SendMessageW(g_hTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM)&ti);
    SendMessageW(g_hTooltip, TTM_POP, 0, 0);
    ShowWindow(g_hTooltip, SW_HIDE);
}

void UpdateTooltipText(int mouseX) {
    if (!g_hTooltip || !g_hMediaWindow) return;
    if (!g_Settings.showSeekbar) return;

    MediaState state;
    {
        lock_guard<mutex> guard(g_MediaState.lock);
        state.hasMedia = g_MediaState.hasMedia;
        state.canSeek = g_MediaState.canSeek;
        state.startTicks = g_MediaState.startTicks;
        state.endTicks = g_MediaState.endTicks;
        state.positionTicks = g_MediaState.positionTicks;
        state.minSeekTicks = g_MediaState.minSeekTicks;
        state.maxSeekTicks = g_MediaState.maxSeekTicks;
    }

    if (!state.hasMedia || !state.canSeek || state.endTicks <= state.startTicks) {
        HideTooltip();
        return;
    }

    RECT bar = GetProgressBarRect();
    if (bar.right <= bar.left) {
        HideTooltip();
        return;
    }

    double ratio = g_IsDraggingSeek ? g_DragSeekRatio : SeekRatioFromX(mouseX);
    ratio = ClampDouble(ratio, 0.0, 1.0);

    long long span = state.endTicks - state.startTicks;
    long long hoverPos = state.startTicks + (long long)((long double)span * ratio);
    hoverPos = ClampI64(hoverPos, state.minSeekTicks, state.maxSeekTicks);

    wstring a = FormatTimeTicks(hoverPos);
    wstring b = FormatTimeTicks(state.endTicks);
    g_TooltipText = a + L" / " + b;

    TOOLINFOW ti{};
    ti.cbSize = sizeof(ti);
    ti.uFlags = TTF_TRACK | TTF_ABSOLUTE;
    ti.hwnd = g_hMediaWindow;
    ti.uId = 1;
    ti.lpszText = (LPWSTR)g_TooltipText.c_str();

    SendMessageW(g_hTooltip, TTM_UPDATETIPTEXTW, 0, (LPARAM)&ti);

    POINT pt{ mouseX, bar.top };
    ClientToScreen(g_hMediaWindow, &pt);
    SendMessageW(g_hTooltip, TTM_TRACKPOSITION, 0, MAKELPARAM(pt.x, pt.y - 24));
    SendMessageW(g_hTooltip, TTM_TRACKACTIVATE, TRUE, (LPARAM)&ti);
}

void DrawMediaPanel(HDC hdc, int width, int height) {
    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);
    graphics.Clear(Color(0, 0, 0, 0));

    Color mainColor{GetCurrentTextColor()};
    Color hoverBg(36, mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue());
    Color seekBg(50, mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue());
    Color seekFill(190, mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue());

    MediaState state;
    {
        lock_guard<mutex> guard(g_MediaState.lock);
        state.title = g_MediaState.title;
        state.artist = g_MediaState.artist;
        state.albumArt = g_MediaState.albumArt ? g_MediaState.albumArt->Clone() : nullptr;
        state.hasMedia = g_MediaState.hasMedia;
        state.isPlaying = g_MediaState.isPlaying;
        state.canSeek = g_MediaState.canSeek;
        state.startTicks = g_MediaState.startTicks;
        state.endTicks = g_MediaState.endTicks;
        state.positionTicks = g_MediaState.positionTicks;
        state.minSeekTicks = g_MediaState.minSeekTicks;
        state.maxSeekTicks = g_MediaState.maxSeekTicks;
    }

    if (!state.hasMedia) {
        if (state.albumArt) delete state.albumArt;
        ResetScrollState();
        return;
    }

    if (g_Settings.showAlbumArt) {
        RECT art = GetArtRect();
        int artSize = art.right - art.left;

        if (state.albumArt) {
            DrawRoundedImage(graphics, state.albumArt, art.left, art.top, artSize, IsCompact() ? 8 : 9);
            delete state.albumArt;
        } else {
            SolidBrush placeBrush{Color(36, 128, 128, 128)};
            FillRoundedRect(graphics, &placeBrush, art.left, art.top, artSize, artSize, IsCompact() ? 8 : 9);
        }
    } else if (state.albumArt) {
        delete state.albumArt;
    }

    RECT btnPrev = GetButtonRect(0);
    RECT btnPlay = GetButtonRect(1);
    RECT btnNext = GetButtonRect(2);

    if (g_HoverState == 1) DrawButtonBg(graphics, btnPrev, hoverBg);
    if (g_HoverState == 2) DrawButtonBg(graphics, btnPlay, hoverBg);
    if (g_HoverState == 3) DrawButtonBg(graphics, btnNext, hoverBg);

    SolidBrush iconBrush(mainColor);

    DrawPrevIcon(graphics, btnPrev, &iconBrush);
    DrawPlayPauseIcon(graphics, btnPlay, &iconBrush, state.isPlaying);
    DrawNextIcon(graphics, btnNext, &iconBrush);

    RECT textRect = GetTextRect();

    wstring fullText = state.title;
    if (!state.artist.empty() && !IsCompact()) {
        fullText += L" • " + state.artist;
    }

    if (fullText != g_LastRenderedText) {
        g_LastRenderedText = fullText;
        ResetScrollState();
    }

    FontFamily fontFamily(FONT_NAME, nullptr);
    Font font(&fontFamily, (REAL)g_Settings.fontSize, FontStyleBold, UnitPixel);
    SolidBrush textBrush(mainColor);

    RectF layoutRect(0, 0, 2600, 100);
    RectF boundRect;
    graphics.MeasureString(fullText.c_str(), -1, &font, layoutRect, &boundRect);
    g_TextWidth = (int)boundRect.Width;

    LONG clipH = textRect.bottom - textRect.top;
    if (g_Settings.showSeekbar) {
        RECT bar = GetProgressBarRect();
        clipH = bar.top - textRect.top - 2;
    }
    if (clipH < 1) clipH = 1;

    Region textClip(Rect(textRect.left, textRect.top, textRect.right - textRect.left, clipH));
    graphics.SetClip(&textClip);

    float textY = (float)textRect.top + (IsCompact() ? 9.0f : 7.0f);
    if (g_TextWidth > (textRect.right - textRect.left)) {
        g_IsScrolling = true;
        float drawX = (float)(textRect.left - g_ScrollOffset);
        graphics.DrawString(fullText.c_str(), -1, &font, PointF(drawX, textY), &textBrush);
        graphics.DrawString(fullText.c_str(), -1, &font, PointF(drawX + g_TextWidth + 44.0f, textY), &textBrush);
    } else {
        g_IsScrolling = false;
        g_ScrollOffset = 0;
        graphics.DrawString(fullText.c_str(), -1, &font, PointF((float)textRect.left, textY), &textBrush);
    }

    graphics.ResetClip();

    if (g_Settings.showSeekbar) {
        RECT bar = GetProgressBarRect();
        SolidBrush barBg(seekBg);
        SolidBrush barFill(seekFill);
        LONG radiusLong = (bar.bottom - bar.top) / 2;
        if (radiusLong < 2) radiusLong = 2;
        int radius = (int)radiusLong;

        FillRoundedRect(graphics, &barBg, bar.left, bar.top, bar.right - bar.left, bar.bottom - bar.top, radius);

        double ratio = 0.0;
        if (g_IsDraggingSeek) {
            ratio = g_DragSeekRatio;
        } else if (state.canSeek && state.endTicks > state.startTicks) {
            long long span = state.endTicks - state.startTicks;
            long long pos = state.positionTicks - state.startTicks;
            if (pos < 0) pos = 0;
            if (pos > span) pos = span;
            ratio = (double)pos / (double)span;
        }

        ratio = ClampDouble(ratio, 0.0, 1.0);

        int fillW = (int)((bar.right - bar.left) * ratio);
        if (fillW > 0) {
            FillRoundedRect(graphics, &barFill, bar.left, bar.top, fillW, bar.bottom - bar.top, radius);
        }

        if ((state.canSeek || g_IsDraggingSeek) && (bar.right - bar.left) > 0) {
            int knobSize = (int)(6.0f + 2.0f * g_SeekbarAnim);
            int knobX = bar.left + fillW;
            if (knobX < bar.left) knobX = bar.left;
            if (knobX > bar.right) knobX = bar.right;
            SolidBrush knobBrush(Color(240, mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue()));
            graphics.FillEllipse(&knobBrush, knobX - knobSize / 2, ((bar.top + bar.bottom) / 2) - knobSize / 2, knobSize, knobSize);
        }
    }
}

#define IDT_POLL_MEDIA 1001
#define IDT_ANIMATION  1002
#define IDT_FADE       1003
#define IDT_SEEKBAR    1004
#define APP_WM_CLOSE   WM_APP

LRESULT CALLBACK MediaWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            UpdateAppearance(hwnd);
            SetTimer(hwnd, IDT_POLL_MEDIA, 900, NULL);
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
            HideTooltip();
            PostQuitMessage(0);
            return 0;

        case WM_SETTINGCHANGE:
            UpdateAppearance(hwnd);
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;

        case WM_TIMER:
            if (wParam == IDT_POLL_MEDIA) {
                if (!g_IsDraggingSeek) {
                    UpdateMediaInfo();
                }

                bool hasMedia = false;
                {
                    lock_guard<mutex> guard(g_MediaState.lock);
                    hasMedia = g_MediaState.hasMedia;
                }

                SetWidgetVisible(hasMedia);

                if (!g_IsDraggingSeek && !IsCursorOverWindow(hwnd)) {
                    g_HoverState = 0;
                    g_IsSeekHover = false;
                    HideTooltip();
                    EnsureSeekbarAnimTimer();
                }

                if (g_WindowVisible) {
                    HWND hTaskbar = FindWindow(TEXT("Shell_TrayWnd"), NULL);
                    if (hTaskbar) {
                        RECT rc;
                        GetWindowRect(hTaskbar, &rc);
                        int x = rc.left + g_Settings.offsetX;
                        int taskbarHeight = rc.bottom - rc.top;
                        int y = rc.top + (taskbarHeight / 2) - (g_Settings.height / 2) + g_Settings.offsetY;

                        RECT myRc;
                        GetWindowRect(hwnd, &myRc);
                        if (myRc.left != x || myRc.top != y ||
                            (myRc.right - myRc.left) != g_Settings.width ||
                            (myRc.bottom - myRc.top) != g_Settings.height) {
                            SetWindowPos(hwnd, HWND_TOPMOST, x, y, g_Settings.width, g_Settings.height, SWP_NOACTIVATE | SWP_SHOWWINDOW);
                        }
                    }

                    InvalidateRect(hwnd, NULL, FALSE);
                }
            } else if (wParam == IDT_ANIMATION) {
                if (g_IsScrolling && g_WindowVisible && g_CurrentAlpha > 0) {
                    if (g_ScrollWait > 0) {
                        g_ScrollWait--;
                    } else {
                        g_ScrollOffset++;
                        if (g_ScrollOffset > g_TextWidth + 44) {
                            g_ScrollOffset = 0;
                            g_ScrollWait = 75;
                        }
                        InvalidateRect(hwnd, NULL, FALSE);
                    }
                } else {
                    KillTimer(hwnd, IDT_ANIMATION);
                }
            } else if (wParam == IDT_FADE) {
                if (g_CurrentAlpha == g_TargetAlpha) {
                    StopFadeTimer();
                    if (g_CurrentAlpha == 0 && g_WindowVisible) {
                        ShowWindow(hwnd, SW_HIDE);
                        g_WindowVisible = false;
                        g_HoverState = 0;
                        g_IsSeekHover = false;
                        HideTooltip();
                        ResetScrollState();
                    }
                    return 0;
                }

                int step = 28;
                if (g_CurrentAlpha < g_TargetAlpha) {
                    int nextAlpha = (int)g_CurrentAlpha + step;
                    g_CurrentAlpha = (BYTE)(nextAlpha > g_TargetAlpha ? g_TargetAlpha : nextAlpha);
                } else {
                    int nextAlpha = (int)g_CurrentAlpha - step;
                    g_CurrentAlpha = (BYTE)(nextAlpha < g_TargetAlpha ? g_TargetAlpha : nextAlpha);
                }

                ApplyLayeredAlpha();
                InvalidateRect(hwnd, NULL, FALSE);
            } else if (wParam == IDT_SEEKBAR) {
                float target = (g_IsSeekHover || g_IsDraggingSeek) ? 1.0f : 0.0f;
                float speed = 0.22f;
                float diff = target - g_SeekbarAnim;
                if (diff < 0.0f) diff = -diff;

                if (diff < 0.01f) {
                    g_SeekbarAnim = target;
                    StopSeekbarAnimTimer();
                } else {
                    g_SeekbarAnim += (target - g_SeekbarAnim) * speed;
                }

                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;

        case WM_MOUSEMOVE: {
            if (!g_WindowVisible || g_CurrentAlpha == 0) return 0;

            int x = LOWORD(lParam);
            int y = HIWORD(lParam);

            RECT bar = GetProgressBarRect();
            bool seekHover = g_Settings.showSeekbar && IsPointInRectInt(x, y, bar);
            if (seekHover != g_IsSeekHover) {
                g_IsSeekHover = seekHover;
                EnsureSeekbarAnimTimer();
                InvalidateRect(hwnd, NULL, FALSE);
            }

            if (g_IsSeekHover || g_IsDraggingSeek) {
                UpdateTooltipText(x);
            } else {
                HideTooltip();
            }

            if (g_IsDraggingSeek) {
                g_DragSeekRatio = SeekRatioFromX(x);
                UpdateTooltipText(x);
                InvalidateRect(hwnd, NULL, FALSE);
                return 0;
            }

            int newState = 0;
            if (IsPointInRectInt(x, y, GetButtonRect(0))) newState = 1;
            else if (IsPointInRectInt(x, y, GetButtonRect(1))) newState = 2;
            else if (IsPointInRectInt(x, y, GetButtonRect(2))) newState = 3;

            if (newState != g_HoverState) {
                g_HoverState = newState;
                InvalidateRect(hwnd, NULL, FALSE);
            }

            TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, hwnd, 0 };
            TrackMouseEvent(&tme);
            return 0;
        }

        case WM_MOUSELEAVE:
            if (!g_IsDraggingSeek) {
                g_HoverState = 0;
                g_IsSeekHover = false;
                EnsureSeekbarAnimTimer();
                HideTooltip();
                if (g_WindowVisible && g_CurrentAlpha > 0) InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;

        case WM_LBUTTONDOWN: {
            if (!g_WindowVisible || g_CurrentAlpha == 0) return 0;

            int x = LOWORD(lParam);
            int y = HIWORD(lParam);

            if (g_Settings.showSeekbar) {
                RECT bar = GetProgressBarRect();
                if (IsPointInRectInt(x, y, bar)) {
                    g_IsDraggingSeek = true;
                    g_DragSeekRatio = SeekRatioFromX(x);
                    EnsureSeekbarAnimTimer();
                    SetCapture(hwnd);
                    UpdateTooltipText(x);
                    InvalidateRect(hwnd, NULL, FALSE);
                    return 0;
                }
            }

            if (g_Settings.openSpotifyOnSingleClick && g_Settings.showAlbumArt) {
                RECT art = GetArtRect();
                if (IsPointInRectInt(x, y, art)) {
                    OpenSpotify();
                    return 0;
                }
            }

            return 0;
        }

        case WM_LBUTTONUP: {
            if (!g_WindowVisible || g_CurrentAlpha == 0) return 0;

            if (g_IsDraggingSeek) {
                g_IsDraggingSeek = false;
                ReleaseCapture();
                SeekToRatio(g_DragSeekRatio);
                EnsureSeekbarAnimTimer();
                InvalidateRect(hwnd, NULL, FALSE);
                return 0;
            }

            int x = LOWORD(lParam);
            int y = HIWORD(lParam);

            if (g_Settings.showSeekbar) {
                RECT bar = GetProgressBarRect();
                if (IsPointInRectInt(x, y, bar)) {
                    SeekToRatio(SeekRatioFromX(x));
                    UpdateTooltipText(x);
                    return 0;
                }
            }

            if (g_HoverState > 0) {
                SendMediaCommand(g_HoverState);
            }
            return 0;
        }

        case WM_CAPTURECHANGED:
            if (g_IsDraggingSeek) {
                g_IsDraggingSeek = false;
                HideTooltip();
                EnsureSeekbarAnimTimer();
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;

        case WM_MBUTTONUP:
            if (g_WindowVisible && g_CurrentAlpha > 0) {
                SendMediaCommand(2);
            }
            return 0;

        case WM_LBUTTONDBLCLK:
            if (g_WindowVisible && g_CurrentAlpha > 0) {
                OpenSpotify();
            }
            return 0;

        case WM_MOUSEWHEEL: {
            if (!g_WindowVisible || g_CurrentAlpha == 0) return 0;

            short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

            if (GetKeyState(VK_SHIFT) & 0x8000) {
                SeekBySeconds(zDelta > 0 ? 5 : -5);
                return 0;
            }

            keybd_event(zDelta > 0 ? VK_VOLUME_UP : VK_VOLUME_DOWN, 0, 0, 0);
            keybd_event(zDelta > 0 ? VK_VOLUME_UP : VK_VOLUME_DOWN, 0, KEYEVENTF_KEYUP, 0);
            return 0;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc;
            GetClientRect(hwnd, &rc);
            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP memBitmap = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
            HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);

            DrawMediaPanel(memDC, rc.right, rc.bottom);

            if (g_IsScrolling && g_WindowVisible && g_CurrentAlpha > 0) {
                SetTimer(hwnd, IDT_ANIMATION, 16, NULL);
            }

            BitBlt(hdc, 0, 0, rc.right, rc.bottom, memDC, 0, 0, SRCCOPY);
            SelectObject(memDC, oldBitmap);
            DeleteObject(memBitmap);
            DeleteDC(memDC);
            EndPaint(hwnd, &ps);
            return 0;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void CreateTooltip() {
    if (!g_hMediaWindow || g_hTooltip) return;

    INITCOMMONCONTROLSEX icc{};
    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&icc);

    g_hTooltip = CreateWindowExW(
        WS_EX_TOPMOST,
        TOOLTIPS_CLASSW,
        NULL,
        WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        g_hMediaWindow,
        NULL,
        GetModuleHandle(NULL),
        NULL
    );

    if (!g_hTooltip) return;

    SetWindowPos(g_hTooltip, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    TOOLINFOW ti{};
    ti.cbSize = sizeof(ti);
    ti.uFlags = TTF_TRACK | TTF_ABSOLUTE;
    ti.hwnd = g_hMediaWindow;
    ti.uId = 1;
    ti.lpszText = (LPWSTR)L"";
    SendMessageW(g_hTooltip, TTM_ADDTOOLW, 0, (LPARAM)&ti);
    SendMessageW(g_hTooltip, TTM_SETMAXTIPWIDTH, 0, 300);
}

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
    wc.style = CS_DBLCLKS;
    RegisterClass(&wc);

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

    g_CurrentAlpha = 0;
    g_TargetAlpha = 0;
    SetLayeredWindowAttributes(g_hMediaWindow, 0, g_CurrentAlpha, LWA_ALPHA);
    ShowWindow(g_hMediaWindow, SW_HIDE);
    g_WindowVisible = false;

    CreateTooltip();

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (g_hTooltip && msg.hwnd == g_hTooltip) {
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_hTooltip) {
        DestroyWindow(g_hTooltip);
        g_hTooltip = NULL;
    }

    UnregisterClass(wc.lpszClassName, wc.hInstance);
    GdiplusShutdown(gdiplusToken);
    winrt::uninit_apartment();
}

std::thread* g_pMediaThread = nullptr;

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
    EnsureSeekbarAnimTimer();
    if (g_hMediaWindow) {
        SendMessage(g_hMediaWindow, WM_TIMER, IDT_POLL_MEDIA, 0);
        SendMessage(g_hMediaWindow, WM_SETTINGCHANGE, 0, 0);
    }
}

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    ExitThread(0);
}

BOOL Wh_ModInit() {
    bool isService = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
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
        g_toolModProcessMutex = CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders = (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

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
    switch (GetModuleFileName(nullptr, currentProcessPath, ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            return;
    }

    WCHAR commandLine[MAX_PATH + 2 + (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath, WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
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
        PHANDLE hRestrictedUserToken
    );

    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule, "CreateProcessInternalW");

    if (!pCreateProcessInternalW) {
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