// ==WindhawkMod==
// @id              taskbar-media-bar
// @name            Taskbar Media Bar
// @description     A native-style media bar for the taskbar with lyrics, mini mode, and acrylic styling.
// @version         1.2
// @author          HibritTofas
// @github          https://github.com/HibritTofas
// @include         explorer.exe
// @compilerOptions -lole32 -ldwmapi -lgdi32 -luser32 -lwindowsapp -lshcore -lgdiplus -lpropsys -luuid -lshell32 -lwinhttp
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Media Bar

A compact media bar that sits on your taskbar, showing what's playing with synchronized lyrics, playback controls, and native Windows 11 acrylic styling.

> Based on the original **Taskbar Music Lounge** by **Hashah2311** (https://github.com/Hashah2311). Extended with lyrics, mini mode, custom context menu, and various UI improvements.

---

## Requirements
- **Windows 11** — required for rounded corners, acrylic blur, and native taskbar integration
- **Disable Taskbar Widgets** — Taskbar Settings → Widgets → Off (frees up space)

---

## Basic Usage

The bar appears on the left side of your taskbar as soon as media starts playing.

- **Left-click** the playback buttons (⏮ ⏯ ⏭) to control media
- **Double-click** anywhere on the bar (outside the buttons) to bring the media app to the foreground. If Spotify is minimized to tray, it will reopen automatically.
- **Scroll wheel** over the bar to adjust system volume (works when the bar or another non-exclusive window is focused)
- **Right-click** to open the context menu

---

## Mini Mode

Right-click → **Mini Mode** to switch to a compact square widget that shows only the album artwork with a circular progress bar around it.

- **Single-click** the artwork to play/pause
- **Double-click** the artwork to bring the media app to the foreground
- The circular progress bar traces clockwise from the top-left corner

---

## Media Source Priority

Windows allows multiple apps to report media at the same time (e.g. Spotify and a browser video playing simultaneously). The bar decides what to show based on the **Spotify Priority** setting.

**Spotify Priority ON (default):**
1. **Spotify actively playing** — Spotify always takes priority when it's playing
2. **Any other app actively playing** — if Spotify is paused or not open, the bar shows whatever is currently playing
3. **Spotify paused** — if nothing else is playing, the bar falls back to Spotify even if it's paused
4. **Current session** — last resort fallback

**Spotify Priority OFF:**
The bar follows standard Windows session order — whichever app Windows considers active is shown, with no special treatment for Spotify.

You can also **lock the bar to a specific source** via Right-click → Media Source regardless of which priority mode is active. The lock is released automatically if that source stops playing.

---

## Lyrics

Right-click → **Show Lyrics** to open the lyrics panel above the bar.

- Lyrics are fetched automatically when a new track starts playing
- Sources: **Musixmatch** (primary) → **LRCLIB** (fallback)
- While fetching, an animated loading indicator is shown. If no lyrics are found, "No lyrics found" is displayed and the panel fades out after 5 seconds.
- The lyrics panel shows the **current line** (bold) and the **next line** (dimmer) for context
- Long lines wrap to a second line instead of being cut off
- **Hover** over the lyrics panel to make it transparent and click-through, so it doesn't block anything underneath
- The lyrics panel **fades out** automatically after ~3.7 seconds of Spotify being paused, and **fades back in** as soon as playback resumes
- The lyrics panel only shows when **Spotify** is the active media source. If another app temporarily takes over (e.g. a browser video), the panel hides and returns automatically when Spotify resumes — without needing to reopen it from the menu. This behavior is independent of the Spotify Priority setting.
- Use the **Lyrics Offset** setting to fine-tune sync. Positive values delay the lyrics, negative values advance them. The typical range is 0–1500ms for Spotify depending on your system, but if lyrics feel off it may also be a sign of incorrect lyrics from the source rather than a sync issue.

> **Note:** Lyrics are only supported for **Spotify**. Other media players (browsers, Windows Media Player, etc.) are not supported for lyrics.

---

## Context Menu

Right-click the bar to open a custom acrylic context menu.

- **Media Source** — lists all active media sessions. Select one to lock the bar to that source.
- **Shuffle** — toggles shuffle on the active session
- **Repeat** — Off / All / This Song
- **Mini Mode** — toggles compact mode
- **Show Lyrics** — toggles the lyrics panel
- **Lyrics Source** *(visible when lyrics are open)* — choose between Musixmatch, Auto, or LRCLIB. Sources unavailable for the current track appear dimmed and cannot be selected.

The menu closes automatically after 2 seconds if the mouse leaves it, or immediately on a click outside. You can select multiple options without the menu closing.

---

## Display Mode

Choose how the bar is positioned relative to the taskbar via **Windhawk Settings → Display Mode**.

**Floating (default):** The bar hovers above the taskbar with rounded corners, a drop shadow, and configurable X/Y offsets. Looks like a separate widget.

**Native:** The bar docks flush against the taskbar — no rounded corners, no gap. Height is locked to the taskbar height and the bar starts from the exact left or right edge. X Offset and Y Offset are ignored in this mode. The bar slides in and out together with the taskbar during auto-hide.

---

## Fullscreen Behavior

The bar and lyrics panel hide automatically when a fullscreen window is detected, and reappear when you return to the desktop or taskbar.

---

## Settings

| Setting | Default | Description |
| :--- | :--- | :--- |
| Display Mode | floating | `floating` hovers above the taskbar with rounded corners. `native` docks flush inside the taskbar bounds — no gap, no rounded corners, offsets ignored. |
| Panel Width | 300 | Width of the bar in pixels |
| Panel Height | 48 | Height of the bar in pixels |
| Font Size | 11 | Size of the track title/artist text |
| Button Scale | 1.0 | Scale factor for playback buttons. Use 2.0 for 4K displays. |
| X Offset | 12 | Horizontal offset from the edge of the taskbar. Ignored in Native mode. |
| Y Offset | 0 | Vertical offset from the center of the taskbar. Ignored in Native mode. |
| Manual Text Color | 0xFFFFFF | Widget text color (hex RGB) |
| Acrylic Tint Opacity | 0 | Background tint strength. Keep at 0 for pure glass. |
| Lyrics Offset (ms) | 500 | Lyrics sync offset in milliseconds |
| Show Album Art | true | Show or hide the album artwork thumbnail |
| Show Playback Buttons | true | Show or hide the previous / play-pause / next buttons |
| Show Progress Bar | true | Show or hide the track progress bar |
| Progress Bar Position | bottom | Draw the progress bar at the top or bottom edge of the widget |
| Artwork Shape | rounded | Rounded or square corners on the album art thumbnail |
| Widget Side | left | Dock the widget to the left or right side of the taskbar |
| Spotify Priority | true | Give Spotify priority over other media sources while playing |
| Auto-hide When No Media | false | Hide the widget automatically when no media is active |
| Auto-hide Delay (ms) | 5000 | How many milliseconds to wait before hiding after media stops |
| Hide Lyrics When Taskbar Hides | true | Also hide the lyrics panel when the taskbar slides away (auto-hide) |

> **Note:** The default values listed above are what the mod is tested and optimized for. Using significantly different values — especially Panel Width, Height, or Button Scale — may cause layout issues or clipped elements. Adjust at your own risk.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- DisplayMode: floating
  $name: Display Mode
  $description: Choose 'Floating' for a traditional widget hovering above the taskbar, or 'Native' to dock seamlessly flush inside the taskbar bounds.
  $options:
  - floating: Floating
  - native: Native
- PanelWidth: 300
  $name: Panel Width
- PanelHeight: 48
  $name: Panel Height
- FontSize: 11
  $name: Font Size
- OffsetX: 12
  $name: X Offset
- OffsetY: 0
  $name: Y Offset
- TextColor: 0xFFFFFF
  $name: Manual Text Color (Hex)
- BgOpacity: 0
  $name: Acrylic Tint Opacity (0-255). Keep 0 for pure glass.
- ButtonScale: 1.0
  $name: Button Scale
  $description: Scale factor for playback buttons. Use 2.0 for 4K displays.
- LyricsOffset: 500
  $name: Lyrics Offset (ms)
  $description: Lyrics sync offset in milliseconds. Typical range is 0-1500 for Spotify depending on your system. Increase if lyrics are late, decrease if early. Note that sync issues can also be caused by incorrect or poorly timed lyrics from the source, not just system latency. Setting to 0 defaults to 500ms.
- ShowArtwork: true
  $name: Show Album Art
  $description: Show the album artwork on the left side of the bar.
- ShowButtons: true
  $name: Show Playback Buttons
  $description: Show the previous, play/pause, and next buttons.
- ShowProgressBar: true
  $name: Show Progress Bar
  $description: Show the track progress bar.
- ProgressBarPosition: bottom
  $name: Progress Bar Position
  $description: Where to draw the progress bar (top or bottom of the widget).
  $options:
  - bottom: Bottom
  - top: Top
- ArtworkShape: rounded
  $name: Artwork Shape
  $description: Corner style for the album art thumbnail.
  $options:
  - rounded: Rounded
  - square: Square
- WidgetSide: left
  $name: Widget Side
  $description: Which side of the taskbar the widget appears on.
  $options:
  - left: Left
  - right: Right
- SpotifyPriority: true
  $name: Spotify Priority
  $description: When enabled, Spotify always takes priority over other media sources while playing. Disable this to use standard Windows session order instead.
- AutoHideNoMedia: false
  $name: Auto-hide When No Media
  $description: Automatically hide the widget when no media is playing or detected.
- AutoHideDelay: 5000
  $name: Auto-hide Delay (ms)
  $description: How long to wait before hiding the widget after media stops. In milliseconds. Default is 5000 (5 seconds).
- HideLyricsWithTaskbar: true
  $name: Hide Lyrics When Taskbar Hides
  $description: When the taskbar is set to auto-hide, also hide the lyrics panel when the taskbar slides away.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <dwmapi.h>
#include <gdiplus.h>
#include <shcore.h>
#include <propsys.h>
#include <winhttp.h>

static const PROPERTYKEY MY_PKEY_AppUserModel_ID = {
    { 0x9F4C2855, 0x9F79, 0x4B39, {0xA8,0xD0,0xE1,0xD4,0x2D,0xE1,0xD5,0xF3} }, 5
};
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdio>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>

using namespace Gdiplus;
using namespace std;
using namespace winrt;
using namespace Windows::Media;
using namespace Windows::Media::Control;
using namespace Windows::Storage::Streams;

const WCHAR* FONT_NAME = L"Segoe UI Variable Display";

typedef enum _WINDOWCOMPOSITIONATTRIB { WCA_ACCENT_POLICY = 19 } WINDOWCOMPOSITIONATTRIB;
typedef enum _ACCENT_STATE {
    ACCENT_DISABLED = 0, ACCENT_ENABLE_BLURBEHIND = 3,
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4, ACCENT_INVALID_STATE = 5
} ACCENT_STATE;
typedef struct _ACCENT_POLICY { ACCENT_STATE AccentState; DWORD AccentFlags; DWORD GradientColor; DWORD AnimationId; } ACCENT_POLICY;
typedef struct _WINDOWCOMPOSITIONATTRIBDATA { WINDOWCOMPOSITIONATTRIB Attribute; PVOID Data; SIZE_T SizeOfData; } WINDOWCOMPOSITIONATTRIBDATA;
typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);

enum ZBID {
    ZBID_DEFAULT = 0, ZBID_DESKTOP = 1, ZBID_UIACCESS = 2,
    ZBID_IMMERSIVE_IHM = 3, ZBID_IMMERSIVE_NOTIFICATION = 4,
    ZBID_IMMERSIVE_APPCHROME = 5, ZBID_IMMERSIVE_MOGO = 6,
    ZBID_IMMERSIVE_EDGY = 7, ZBID_IMMERSIVE_INACTIVEMOBODY = 8,
    ZBID_IMMERSIVE_INACTIVEDOCK = 9, ZBID_IMMERSIVE_ACTIVEMOBODY = 10,
    ZBID_IMMERSIVE_ACTIVEDOCK = 11, ZBID_IMMERSIVE_BACKGROUND = 12,
    ZBID_IMMERSIVE_SEARCH = 13, ZBID_GENUINE_WINDOWS = 14,
    ZBID_IMMERSIVE_RESTRICTED = 15, ZBID_SYSTEM_TOOLS = 16,
    ZBID_LOCK = 17, ZBID_ABOVELOCK_UX = 18,
};

typedef HWND(WINAPI* pCreateWindowInBand)(DWORD,LPCWSTR,LPCWSTR,DWORD,int,int,int,int,HWND,HMENU,HINSTANCE,LPVOID,DWORD);

struct ModSettings {
    int width = 300, height = 48, fontSize = 11, offsetX = 12, offsetY = 0;
    DWORD manualTextColor = 0xFFFFFFFF;
    int bgOpacity = 0;
    double buttonScale = 1.0;
    int lyricsOffsetMs = 1000;
    // Customization
    bool showArtwork = true;
    bool showButtons = true;
    bool showProgressBar = true;
    bool progressBarOnTop = false;  // false=bottom, true=top
    bool artworkRounded = true;     // false=square
    bool widgetOnRight = false;     // false=left, true=right
    bool spotifyPriority = true;    // Give Spotify priority over other sources
    bool autoHideNoMedia = false;   // Hide widget when no media is active
    int  autoHideDelayMs = 5000;    // Milliseconds before hiding
    bool hideLyricsWithTaskbar = true; // Hide lyrics panel when taskbar slides away
    bool displayNative = false;    // false=floating, true=native
} g_Settings;

HWND g_hMediaWindow = NULL;
std::atomic<bool> g_Running{true};
int g_HoverState = 0;
bool g_IsHidden = false;           // hidden due to fullscreen
int  g_AutoHideAlpha  = 255;       // auto-hide fade alpha (0-255)
int  g_AutoHideTarget = 255;       // target alpha (0=hide, 255=show)

struct MediaState {
    wstring title = L"Waiting for media...";
    wstring artist = L"";
    bool isPlaying = false;
    bool hasMedia = false;
    Bitmap* albumArt = nullptr;
    double progressRatio = 0.0;
    bool hasProgress = false;
    int positionMs = 0;
    wstring sourceAppId = L"";
    bool isShuffle = false;
    int repeatMode = 0;
    mutex lock;
} g_MediaState;

int g_ScrollOffset = 0, g_TextWidth = 0, g_ScrollWait = 60;
bool g_IsScrolling = false;

HWND g_hPopupWindow = NULL;
bool g_IsHoveringArt = false;
bool g_UserHidden = false;
bool g_ManualSource = false;
wstring g_ManualSourceId = L"";
DWORD g_NoMediaSinceTick = 0;  // tick when media stopped (for auto-hide delay)
bool g_MiniMode = false;

// Lyrics slide animation (taskbar auto-hide)
int g_LyricsSlideX = 0;       // pixel offset (0 = normal position)
int g_LyricsSlideTarget = 0;  // target offset
HWND g_hContextMenu = NULL;
struct LyricLine { int ms; wstring text; };
vector<LyricLine> g_Lyrics;
wstring g_LyricsTitle = L"";
wstring g_LyricsArtist = L"";
bool g_LyricsLoading = false;
bool g_LyricsNotFound = false;
bool g_LyricsFetchDone = false; // whether first fetch has completed
// 0=auto(Mxm→LRCLIB), 1=Musixmatch only, 2=LRCLIB only
int  g_LyricsSourcePref = 0;
// Which sources returned results
bool g_LyricsMxmAvail   = false;
bool g_LyricsLrclibAvail = false;
mutex g_LyricsMutex;
HWND g_hLyricsWindow = NULL;
bool g_LyricsVisible = false;

// Lyric line cross-fade
wstring g_LyricCurLine = L"";
wstring g_LyricNxtLine = L"";
wstring g_LyricPrevLine = L"";
wstring g_LyricPrevNxtLine = L"";
float   g_LyricFadeT = 1.0f;

// Pre-lyric dot animation
DWORD g_DotStartTick[3] = {0, 0, 0};
DWORD g_DotDurationMs[3] = {0, 0, 0};
float g_DotGlow[3] = {0, 0, 0};
float g_DotAnimT[3] = {0, 0, 0}; // 0..1 animation progress


DWORD g_LyricsPosUpdateTick = 0;
int   g_LyricsLastPosMs = 0;
bool  g_LyricsIsPlaying = false;
int   g_LyricsLastIdx = -1;
int   g_LyricsDisplayIdx = -1; // for animation — triggers when index changes

// Lyrics fade variables
int   g_LyricsFadeAlpha = 255;
int   g_LyricsTargetAlpha = 255;
DWORD g_SpotifyLastPlayingTick = 0;
DWORD g_LyricsNotFoundTick = 0; // tick when 'not found' state was set

void FetchLyricsAsync(wstring artist, wstring title);
void UpdateLyricsWindowPos();
DWORD GetCurrentTextColor();

HWINEVENTHOOK g_hForegroundHook = NULL;
HWINEVENTHOOK g_hMoveSizeHook = NULL;
HWINEVENTHOOK g_hTaskbarMoveHook = NULL;

void LoadSettings() {
    g_Settings.width = Wh_GetIntSetting(L"PanelWidth");
    g_Settings.height = Wh_GetIntSetting(L"PanelHeight");
    g_Settings.fontSize = Wh_GetIntSetting(L"FontSize");
    g_Settings.offsetX = Wh_GetIntSetting(L"OffsetX");
    g_Settings.offsetY = Wh_GetIntSetting(L"OffsetY");
    PCWSTR textHex = Wh_GetStringSetting(L"TextColor");
    DWORD textRGB = 0xFFFFFF;
    if (textHex) { if (wcslen(textHex) > 0) textRGB = wcstoul(textHex, nullptr, 16); Wh_FreeStringSetting(textHex); }
    g_Settings.manualTextColor = 0xFF000000 | textRGB;
    g_Settings.bgOpacity = Wh_GetIntSetting(L"BgOpacity");
    if (g_Settings.bgOpacity < 0) g_Settings.bgOpacity = 0;
    if (g_Settings.bgOpacity > 255) g_Settings.bgOpacity = 255;
    g_Settings.lyricsOffsetMs = Wh_GetIntSetting(L"LyricsOffset");
    PCWSTR scaleStr = Wh_GetStringSetting(L"ButtonScale");
    g_Settings.buttonScale = scaleStr ? _wtof(scaleStr) : 1.0;
    if (scaleStr) Wh_FreeStringSetting(scaleStr);
    if (g_Settings.buttonScale < 0.5) g_Settings.buttonScale = 0.5;
    if (g_Settings.buttonScale > 4.0) g_Settings.buttonScale = 4.0;
    if (g_Settings.width < 100) g_Settings.width = 300;
    if (g_Settings.height < 24) g_Settings.height = 48;

    // Customization settings
    g_Settings.showArtwork     = Wh_GetIntSetting(L"ShowArtwork")     != 0;
    g_Settings.showButtons     = Wh_GetIntSetting(L"ShowButtons")     != 0;
    g_Settings.showProgressBar = Wh_GetIntSetting(L"ShowProgressBar") != 0;

    PCWSTR barPos = Wh_GetStringSetting(L"ProgressBarPosition");
    g_Settings.progressBarOnTop = barPos && wcscmp(barPos, L"top") == 0;
    if (barPos) Wh_FreeStringSetting(barPos);

    PCWSTR artShape = Wh_GetStringSetting(L"ArtworkShape");
    g_Settings.artworkRounded = !artShape || wcscmp(artShape, L"square") != 0;
    if (artShape) Wh_FreeStringSetting(artShape);

    PCWSTR widgetSide = Wh_GetStringSetting(L"WidgetSide");
    g_Settings.widgetOnRight = widgetSide && wcscmp(widgetSide, L"right") == 0;
    if (widgetSide) Wh_FreeStringSetting(widgetSide);

    g_Settings.spotifyPriority = Wh_GetIntSetting(L"SpotifyPriority") != 0;

    g_Settings.autoHideNoMedia  = Wh_GetIntSetting(L"AutoHideNoMedia") != 0;
    g_Settings.autoHideDelayMs  = Wh_GetIntSetting(L"AutoHideDelay");
    if (g_Settings.autoHideDelayMs < 0)    g_Settings.autoHideDelayMs = 0;
    if (g_Settings.autoHideDelayMs > 60000) g_Settings.autoHideDelayMs = 60000;
    g_Settings.hideLyricsWithTaskbar = Wh_GetIntSetting(L"HideLyricsWithTaskbar") != 0;

    PCWSTR dispModeStr = Wh_GetStringSetting(L"DisplayMode");
    g_Settings.displayNative = dispModeStr && wcscmp(dispModeStr, L"native") == 0;
    if (dispModeStr) Wh_FreeStringSetting(dispModeStr);
}

// UI state (mini mode, lyrics, source, offset) saved in registry
static const wchar_t* REG_KEY = L"Software\\taskbar-media-bar";

void SaveUIState() {
    HKEY hk;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, REG_KEY, 0, NULL, 0, KEY_SET_VALUE, NULL, &hk, NULL) != ERROR_SUCCESS) return;
    DWORD v;
    v = g_MiniMode ? 1 : 0;        RegSetValueExW(hk, L"MiniMode",     0, REG_DWORD, (BYTE*)&v, sizeof(v));
    v = g_LyricsVisible ? 1 : 0;   RegSetValueExW(hk, L"LyricsVisible",0, REG_DWORD, (BYTE*)&v, sizeof(v));
    v = (DWORD)g_LyricsSourcePref; RegSetValueExW(hk, L"LyricsSrc",    0, REG_DWORD, (BYTE*)&v, sizeof(v));
    // Offset is not saved here — loaded from Windhawk settings
    RegDeleteValueW(hk, L"LyricsOffset2");
    RegDeleteValueW(hk, L"LyricsOffsetSet");
    RegDeleteValueW(hk, L"LyricsOffsetVal");
    RegCloseKey(hk);
}

void LoadUIState() {
    HKEY hk;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, REG_KEY, 0, KEY_QUERY_VALUE, &hk) != ERROR_SUCCESS) return;
    DWORD v, sz = sizeof(v);
    if (RegQueryValueExW(hk, L"MiniMode",      NULL, NULL, (BYTE*)&v, &sz)==ERROR_SUCCESS) g_MiniMode = v != 0;
    sz=sizeof(v);
    if (RegQueryValueExW(hk, L"LyricsVisible", NULL, NULL, (BYTE*)&v, &sz)==ERROR_SUCCESS) g_LyricsVisible = v != 0;
    sz=sizeof(v);
    if (RegQueryValueExW(hk, L"LyricsSrc",     NULL, NULL, (BYTE*)&v, &sz)==ERROR_SUCCESS) g_LyricsSourcePref = (int)v;
    RegCloseKey(hk);
    Wh_Log(L"[State] Loaded: mini=%d lyrics=%d src=%d offset=%d", g_MiniMode, g_LyricsVisible, g_LyricsSourcePref, g_Settings.lyricsOffsetMs);
}

GlobalSystemMediaTransportControlsSessionManager g_SessionManager = nullptr;
winrt::event_token g_SessionsChangedToken{};
winrt::event_token g_CurrentSessionChangedToken{};

Bitmap* StreamToBitmap(IRandomAccessStreamWithContentType const& stream) {
    if (!stream) return nullptr;
    IStream* ns = nullptr;
    if (SUCCEEDED(CreateStreamOverRandomAccessStream(reinterpret_cast<IUnknown*>(winrt::get_abi(stream)), IID_PPV_ARGS(&ns)))) {
        Bitmap* bmp = Bitmap::FromStream(ns);
        ns->Release();
        if (bmp && bmp->GetLastStatus() == Ok) return bmp;
        delete bmp;
    }
    return nullptr;
}

bool IsForegroundFullscreen() {
    HWND hWnd = GetForegroundWindow();
    if (!hWnd) return false;
    if (hWnd == g_hMediaWindow) return false;
    WCHAR cls[64] = {};
    GetClassName(hWnd, cls, 64);
    if (wcscmp(cls, L"Shell_TrayWnd") == 0) return false;
    if (wcscmp(cls, L"WindhawkMusicLounge_GSMTC") == 0) return false;
    if (wcscmp(cls, L"WML_Lyrics") == 0) return false;
    if (wcscmp(cls, L"WML_ContextMenu") == 0) return false;
    if (wcscmp(cls, L"WML_SubMenu") == 0) return false;
    if (wcscmp(cls, L"WML_ArtPopup") == 0) return false;
    if (wcscmp(cls, L"Progman") == 0) return false;
    if (wcscmp(cls, L"WorkerW") == 0) return false;
    if (wcscmp(cls, L"Shell_SecondaryTrayWnd") == 0) return false;
    if (wcsncmp(cls, L"HwndWrapper", 11) == 0) return false;
    if (wcscmp(cls, L"WindhawkUI") == 0) return false;

    RECT appRect; GetWindowRect(hWnd, &appRect);
    HMONITOR hMon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi = { sizeof(mi) }; GetMonitorInfo(hMon, &mi);

    bool coversMonitor = (appRect.left  <= mi.rcMonitor.left   + 1 &&
                          appRect.top   <= mi.rcMonitor.top    + 1 &&
                          appRect.right  >= mi.rcMonitor.right  - 1 &&
                          appRect.bottom >= mi.rcMonitor.bottom - 1);
    if (!coversMonitor) return false;

    APPBARDATA abd = { sizeof(abd) };
    bool autoHideOn = (SHAppBarMessage(ABM_GETSTATE, &abd) & ABS_AUTOHIDE) != 0;
    if (autoHideOn) {
        HWND hTaskbar = FindWindow(L"Shell_TrayWnd", NULL);
        if (hTaskbar) {
            RECT trc; GetWindowRect(hTaskbar, &trc);
            int smallDim = min(abs(trc.bottom - trc.top), abs(trc.right - trc.left));
            if (smallDim > 4) return false;
        }
    }

    return true;
}

void SubscribeGSMTCEvents() {
    if (!g_SessionManager || !g_hMediaWindow) return;
    try {
        g_SessionsChangedToken = g_SessionManager.SessionsChanged(
            [](auto&&, auto&&) {
                if (g_hMediaWindow) PostMessage(g_hMediaWindow, WM_TIMER, 201 /*IDT_POLL_MEDIA*/, 0);
            });
        g_CurrentSessionChangedToken = g_SessionManager.CurrentSessionChanged(
            [](auto&&, auto&&) {
                if (g_hMediaWindow) PostMessage(g_hMediaWindow, WM_TIMER, 201 /*IDT_POLL_MEDIA*/, 0);
            });
    } catch (...) {}
}

void UnsubscribeGSMTCEvents() {
    if (!g_SessionManager) return;
    try {
        if (g_SessionsChangedToken) { g_SessionManager.SessionsChanged(g_SessionsChangedToken); g_SessionsChangedToken = {}; }
        if (g_CurrentSessionChangedToken) { g_SessionManager.CurrentSessionChanged(g_CurrentSessionChangedToken); g_CurrentSessionChangedToken = {}; }
    } catch (...) {}
}


// Applies window alpha combining fullscreen-hide and auto-hide states
void ApplyWindowAlpha() {
    if (!g_hMediaWindow) return;
    BYTE alpha = g_IsHidden ? 0 : (BYTE)g_AutoHideAlpha;
    SetLayeredWindowAttributes(g_hMediaWindow, 0, alpha, LWA_ALPHA);
}

// Returns the height to use for SetWindowPos — taskbar height in Native mode, settings height otherwise
int GetWidgetDrawHeight() {
    if (g_Settings.displayNative) {
        HWND hTb = FindWindow(TEXT("Shell_TrayWnd"), NULL);
        if (hTb) {
            RECT rc; GetWindowRect(hTb, &rc);
            int tbH = abs(rc.bottom - rc.top);
            if (tbH > 20 && tbH < 200) return tbH;
        }
    }
    return g_Settings.height;
}

void CheckAndApplyFullscreen() {
    if (!g_hMediaWindow) return;
    if (g_UserHidden) return;
    bool isFs = IsForegroundFullscreen();
    if (isFs && !g_IsHidden) {
        g_IsHidden = true;
        ApplyWindowAlpha();
        if (g_hLyricsWindow) ShowWindow(g_hLyricsWindow, SW_HIDE);
    } else if (!isFs && g_IsHidden) {
        g_IsHidden = false;
        ApplyWindowAlpha();
        if (g_hLyricsWindow && g_LyricsVisible && g_LyricsFadeAlpha > 0) ShowWindow(g_hLyricsWindow, SW_SHOWNOACTIVATE);
    }
}

void CALLBACK FullscreenEventProc(HWINEVENTHOOK, DWORD, HWND, LONG, LONG, DWORD, DWORD) { CheckAndApplyFullscreen(); }

void CALLBACK TaskbarMoveProc(HWINEVENTHOOK, DWORD, HWND hWnd, LONG idObj, LONG, DWORD, DWORD) {
    if (idObj != OBJID_WINDOW) return;
    WCHAR cls[32] = {};
    GetClassName(hWnd, cls, 32);
    if (wcscmp(cls, L"Shell_TrayWnd") != 0) return;
    CheckAndApplyFullscreen();
}

void UpdateMediaInfo() {
    try {
        if (!g_SessionManager) {
            g_SessionManager = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
            Wh_Log(L"[GSMTC] Session manager: %s", g_SessionManager ? L"OK" : L"FAILED");
            SubscribeGSMTCEvents();
        }
        if (!g_SessionManager) return;

        auto sessions = g_SessionManager.GetSessions();
        using S = GlobalSystemMediaTransportControlsSessionPlaybackStatus;
        GlobalSystemMediaTransportControlsSession session = nullptr;
        GlobalSystemMediaTransportControlsSession spotifySession = nullptr;
        GlobalSystemMediaTransportControlsSession manualSession = nullptr;
        GlobalSystemMediaTransportControlsSession anyPlayingSession = nullptr;
        bool spotifyPlaying = false, manualPlaying = false;

        // Track the most recently started source
        static vector<wstring> s_prevPlayingIds;
        static wstring s_lastStartedId;
        vector<wstring> curPlayingIds;

        for (auto const& s : sessions) {
            auto appId  = wstring(s.SourceAppUserModelId().c_str());
            auto status = s.GetPlaybackInfo().PlaybackStatus();
            bool playing = (status == S::Playing);
            if (appId.find(L"Spotify") != wstring::npos) { spotifySession = s; if (playing) spotifyPlaying = true; }
            if (g_ManualSource && appId == g_ManualSourceId) { manualSession = s; manualPlaying = playing; }
            if (playing) {
                curPlayingIds.push_back(appId);
                bool wasPlaying = false;
                for (auto& p : s_prevPlayingIds) if (p == appId) { wasPlaying = true; break; }
                if (!wasPlaying) s_lastStartedId = appId;
                if (!anyPlayingSession) anyPlayingSession = s;
            }
        }
        s_prevPlayingIds = curPlayingIds;

        if (g_ManualSource && manualPlaying) session = manualSession;
        else if (g_ManualSource && !manualPlaying) { g_ManualSource = false; g_ManualSourceId = L""; }

        if (!session) {
            if (g_Settings.spotifyPriority) {
                // Spotify priority mode
                if (spotifyPlaying)         session = spotifySession;
                else if (anyPlayingSession) session = anyPlayingSession;
                else if (spotifySession)    session = spotifySession;
                else                        session = g_SessionManager.GetCurrentSession();
            } else {
                // Standard mode: show the most recently started source
                if (!s_lastStartedId.empty()) {
                    for (auto const& s : sessions) {
                        if (wstring(s.SourceAppUserModelId().c_str()) == s_lastStartedId) {
                            if (s.GetPlaybackInfo().PlaybackStatus() == S::Playing)
                                { session = s; break; }
                        }
                    }
                }
                if (!session && anyPlayingSession) session = anyPlayingSession;
                if (!session) session = g_SessionManager.GetCurrentSession();
            }
        }

        if (session) {
            auto props    = session.TryGetMediaPropertiesAsync().get();
            auto info     = session.GetPlaybackInfo();
            auto timeline = session.GetTimelineProperties();

            double ratio = 0.0; bool hasProgress = false; int posMs = 0;
            try {
                auto pos = timeline.Position(); auto end = timeline.EndTime();
                auto endSec = chrono::duration_cast<chrono::duration<double>>(end).count();
                auto posSec = chrono::duration_cast<chrono::duration<double>>(pos).count();
                if (endSec > 0.0) { ratio = posSec / endSec; if (ratio > 1.0) ratio = 1.0; hasProgress = true; }
                posMs = (int)(posSec * 1000.0);
            } catch (...) {}

            lock_guard<mutex> guard(g_MediaState.lock);
            wstring newTitle = props.Title().c_str();
            if (newTitle != g_MediaState.title || g_MediaState.albumArt == nullptr) {
                if (g_MediaState.albumArt) { delete g_MediaState.albumArt; g_MediaState.albumArt = nullptr; }
                auto thumbRef = props.Thumbnail();
                if (thumbRef) { auto stream = thumbRef.OpenReadAsync().get(); g_MediaState.albumArt = StreamToBitmap(stream); }
            }
            g_MediaState.title       = newTitle;
            g_MediaState.artist      = props.Artist().c_str();
            g_MediaState.isPlaying   = (info.PlaybackStatus() == S::Playing);
            g_MediaState.hasMedia    = true;
            g_NoMediaSinceTick = 0;  // media present — reset the no-media timer
            g_MediaState.progressRatio = ratio;
            g_MediaState.hasProgress = hasProgress;
            g_MediaState.positionMs  = posMs;
            wstring newSourceId = session.SourceAppUserModelId().c_str();
            if (newSourceId != g_MediaState.sourceAppId)
                Wh_Log(L"[Media] Source: %s", newSourceId.c_str());
            g_MediaState.sourceAppId = newSourceId;

            
            if (posMs != g_LyricsLastPosMs) {
                g_LyricsLastPosMs     = posMs;
                g_LyricsPosUpdateTick = GetTickCount();
            }
            g_LyricsIsPlaying = g_MediaState.isPlaying;

            wstring artistStr = props.Artist().c_str();
            bool isSpotify = (g_MediaState.sourceAppId.find(L"Spotify") != wstring::npos);
            if (isSpotify) {
                bool titleChanged;
                { lock_guard<mutex> lg(g_LyricsMutex);
                  titleChanged = (newTitle != g_LyricsTitle || artistStr != g_LyricsArtist);
                  if (titleChanged) { g_LyricsTitle = newTitle; g_LyricsArtist = artistStr; g_Lyrics.clear(); g_LyricsLoading = false; g_LyricsNotFound = false; g_LyricsNotFoundTick = 0; g_LyricsFetchDone = false; g_LyricsMxmAvail = false; g_LyricsLrclibAvail = false; g_LyricsLastIdx = -1; g_LyricsDisplayIdx = -1; g_LyricsLastPosMs = 0; g_LyricsPosUpdateTick = 0;
                                  g_DotStartTick[0]=g_DotStartTick[1]=g_DotStartTick[2]=0;
                                  g_DotGlow[0]=g_DotGlow[1]=g_DotGlow[2]=0; } }
                if (titleChanged && !newTitle.empty()) FetchLyricsAsync(artistStr, newTitle);
            }

            try {
                auto shuffle = info.IsShuffleActive();
                g_MediaState.isShuffle = shuffle && shuffle.Value();
                auto repeat = info.AutoRepeatMode();
                if (repeat) {
                    using R = Windows::Media::MediaPlaybackAutoRepeatMode;
                    g_MediaState.repeatMode = repeat.Value() == R::List ? 1 : repeat.Value() == R::Track ? 2 : 0;
                }
            } catch (...) {}
        } else {
            lock_guard<mutex> guard(g_MediaState.lock);
            g_MediaState.hasMedia = false; g_MediaState.title = L"No Media"; g_MediaState.artist = L"";
            g_MediaState.hasProgress = false;
            if (g_MediaState.albumArt) { delete g_MediaState.albumArt; g_MediaState.albumArt = nullptr; }
            // First time media goes absent — record the tick
            if (g_NoMediaSinceTick == 0) g_NoMediaSinceTick = GetTickCount();
        }
    } catch (...) { lock_guard<mutex> guard(g_MediaState.lock); g_MediaState.hasMedia = false; if(g_NoMediaSinceTick==0) g_NoMediaSinceTick=GetTickCount(); }
}

void SendMediaCommand(int cmd) {
    const wchar_t* names[] = {L"?",L"Prev",L"PlayPause",L"Next"};
    Wh_Log(L"[Media] Command: %s", cmd>=1&&cmd<=3?names[cmd]:names[0]);
    try {
        if (!g_SessionManager) return;
        wstring targetId; { lock_guard<mutex> guard(g_MediaState.lock); targetId = g_MediaState.sourceAppId; }
        GlobalSystemMediaTransportControlsSession session = nullptr;
        auto sessions = g_SessionManager.GetSessions();
        for (auto const& s : sessions) { if (wstring(s.SourceAppUserModelId().c_str()) == targetId) { session = s; break; } }
        if (!session) session = g_SessionManager.GetCurrentSession();
        if (session) {
            if (cmd == 1) session.TrySkipPreviousAsync();
            else if (cmd == 2) session.TryTogglePlayPauseAsync();
            else if (cmd == 3) session.TrySkipNextAsync();
        }
    } catch (...) {}
}

static struct { wstring appId; wstring exeHint; HWND result; } g_FindData;

static wstring ExtractExeHint(const wstring& appId) {
    wstring id = appId;
    for (auto& c : id) c = (WCHAR)towlower(c);
    auto slash = id.rfind(L'\\'); if (slash != wstring::npos) id = id.substr(slash + 1);
    auto dot = id.rfind(L'.'); if (dot != wstring::npos && id.substr(dot) == L".exe") id = id.substr(0, dot);
    static const WCHAR* knownApps[] = { L"spotify", L"chrome", L"firefox", L"msedge", L"opera", L"brave", L"zen", L"vivaldi", L"arc", L"waterfox", L"librewolf", L"edge" };
    for (auto app : knownApps) if (id.find(app) != wstring::npos) return wstring(app);
    auto dotPos = id.rfind(L'.'); if (dotPos != wstring::npos) return id.substr(dotPos + 1);
    return id;
}

static BOOL CALLBACK FindWindowByAppIdProc(HWND hWnd, LPARAM) {
    bool visible = IsWindowVisible(hWnd) != FALSE;
    bool iconic  = IsIconic(hWnd) != FALSE;
    if (visible && !iconic) { RECT r; GetWindowRect(hWnd, &r); if ((r.right-r.left)<50||(r.bottom-r.top)<50) return TRUE; }

    IPropertyStore* pStore = nullptr;
    if (SUCCEEDED(SHGetPropertyStoreForWindow(hWnd, __uuidof(IPropertyStore), (void**)&pStore))) {
        PROPVARIANT pv; PropVariantInit(&pv);
        bool matched = false;
        if (SUCCEEDED(pStore->GetValue(MY_PKEY_AppUserModel_ID, &pv)) && pv.vt == VT_LPWSTR)
            if (wstring(pv.pwszVal) == g_FindData.appId) matched = true;
        PropVariantClear(&pv); pStore->Release();
        if (matched) { Wh_Log(L"[BringToFront] PKEY: appId=%s vis=%d iconic=%d", g_FindData.appId.c_str(), (int)visible, (int)iconic); g_FindData.result = hWnd; return FALSE; }
    }

    if (g_FindData.exeHint.empty()) return TRUE;
    DWORD pid = 0; GetWindowThreadProcessId(hWnd, &pid); if (!pid) return TRUE;
    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid); if (!hProc) return TRUE;
    WCHAR exePath[MAX_PATH] = {}; DWORD sz = MAX_PATH; QueryFullProcessImageNameW(hProc, 0, exePath, &sz); CloseHandle(hProc);
    WCHAR* exeName = wcsrchr(exePath, L'\\'); exeName = exeName ? exeName + 1 : exePath;
    wstring exe(exeName); for (auto& c : exe) c = (WCHAR)towlower(c);
    auto dot2 = exe.rfind(L'.'); wstring exeBase = (dot2 != wstring::npos) ? exe.substr(0, dot2) : exe;
    if (exeBase == g_FindData.exeHint || exeBase.find(g_FindData.exeHint) != wstring::npos || g_FindData.exeHint.find(exeBase) != wstring::npos) {
        WCHAR title[8] = {}; GetWindowText(hWnd, title, 8);
        if (wcslen(title) == 0 && !iconic) return TRUE;
        Wh_Log(L"[BringToFront] exe: %s hint=%s vis=%d iconic=%d", exeBase.c_str(), g_FindData.exeHint.c_str(), (int)visible, (int)iconic);
        g_FindData.result = hWnd; return FALSE;
    }
    return TRUE;
}

struct BestWndCtx { DWORD pid; HWND best; int score; };
static BOOL CALLBACK FindBestWindowProc(HWND h, LPARAM lp) {
    auto* c = (BestWndCtx*)lp;
    DWORD pid = 0; GetWindowThreadProcessId(h, &pid); if (pid != c->pid) return TRUE;
    RECT r = {}; GetWindowRect(h, &r); int w = r.right-r.left, ht = r.bottom-r.top;
    WCHAR title[64] = {}; GetWindowText(h, title, 64);
    bool hasTitle = wcslen(title)>0, visible = IsWindowVisible(h)!=FALSE, iconic = IsIconic(h)!=FALSE, goodSize = (w>100&&ht>100)||iconic;
    int score = (hasTitle?8:0)+(goodSize?4:0)+(visible?2:0)+(iconic?1:0);
    if (score > c->score) { c->score = score; c->best = h; }
    return TRUE;
}

void BringSourceAppToFront() {
    { lock_guard<mutex> guard(g_MediaState.lock); g_FindData.appId = g_MediaState.sourceAppId; }
    g_FindData.result = NULL; if (g_FindData.appId.empty()) return;
    g_FindData.exeHint = ExtractExeHint(g_FindData.appId);
    Wh_Log(L"[BringToFront] appId=%s exeHint=%s", g_FindData.appId.c_str(), g_FindData.exeHint.c_str());
    EnumWindows(FindWindowByAppIdProc, 0);
    if (!g_FindData.result) { Wh_Log(L"[BringToFront] NOT FOUND: %s", g_FindData.appId.c_str()); return; }

    DWORD targetPid = 0; GetWindowThreadProcessId(g_FindData.result, &targetPid);
    BestWndCtx ctx = { targetPid, g_FindData.result, -1 };
    EnumWindows(FindBestWindowProc, (LPARAM)&ctx);
    HWND hWnd = ctx.best;
    bool visible = IsWindowVisible(hWnd)!=FALSE, iconic = IsIconic(hWnd)!=FALSE;
    Wh_Log(L"[BringToFront] selected: hwnd=%p vis=%d iconic=%d", hWnd, (int)visible, (int)iconic);
    AllowSetForegroundWindow(ASFW_ANY);

    if (iconic) {
        SetWindowPos(g_hMediaWindow, HWND_NOTOPMOST, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
        ShowWindow(hWnd, SW_RESTORE); SetForegroundWindow(hWnd);
        SetWindowPos(g_hMediaWindow, HWND_TOPMOST, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
    } else if (visible) {
        SetWindowPos(g_hMediaWindow, HWND_NOTOPMOST, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
        BringWindowToTop(hWnd); SetForegroundWindow(hWnd);
        SetWindowPos(g_hMediaWindow, HWND_TOPMOST, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
    } else {
        if (g_FindData.appId.find(L"Spotify") != wstring::npos || g_FindData.exeHint == L"spotify") {
            ShellExecuteW(NULL, L"open", L"spotify:", NULL, NULL, SW_SHOW); return;
        }
        SetWindowPos(g_hMediaWindow, HWND_NOTOPMOST, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
        SendMessage(hWnd, WM_SYSCOMMAND, SC_RESTORE, 0); SetForegroundWindow(hWnd);
        SetWindowPos(g_hMediaWindow, HWND_TOPMOST, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
    }
}

void SendShuffleToggle() {
    try {
        if (!g_SessionManager) return;
        wstring targetId; { lock_guard<mutex> guard(g_MediaState.lock); targetId = g_MediaState.sourceAppId; }
        auto sessions = g_SessionManager.GetSessions();
        for (auto const& s : sessions) { if (wstring(s.SourceAppUserModelId().c_str()) == targetId) { s.TryChangeShuffleActiveAsync(!g_MediaState.isShuffle); break; } }
    } catch (...) {}
}

LRESULT CALLBACK PopupWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_PAINT) {
        PAINTSTRUCT ps; HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc; GetClientRect(hwnd, &rc);
        HDC memDC = CreateCompatibleDC(hdc); HBITMAP memBmp = CreateCompatibleBitmap(hdc, rc.right, rc.bottom); HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, memBmp);
        HBRUSH bg = CreateSolidBrush(RGB(20,20,20)); FillRect(memDC, &rc, bg); DeleteObject(bg);
        Bitmap* art = nullptr; { lock_guard<mutex> guard(g_MediaState.lock); art = g_MediaState.albumArt ? g_MediaState.albumArt->Clone() : nullptr; }
        if (art) { Graphics g(memDC); g.SetInterpolationMode(InterpolationModeHighQualityBicubic); g.DrawImage(art, 4, 4, rc.right-8, rc.bottom-8); delete art; }
        BitBlt(hdc, 0, 0, rc.right, rc.bottom, memDC, 0, 0, SRCCOPY);
        SelectObject(memDC, oldBmp); DeleteObject(memBmp); DeleteDC(memDC);
        EndPaint(hwnd, &ps); return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void ShowArtPopup(HWND parent) {
    if (g_hPopupWindow) return;
    const int SZ = 200;
    HWND hTaskbar = FindWindow(TEXT("Shell_TrayWnd"), NULL);
    RECT trc = {}; if (hTaskbar) GetWindowRect(hTaskbar, &trc);
    RECT prc = {}; GetWindowRect(parent, &prc);
    static bool reg = false;
    if (!reg) { WNDCLASS wc = {}; wc.lpfnWndProc = PopupWndProc; wc.hInstance = GetModuleHandle(NULL); wc.lpszClassName = TEXT("WML_ArtPopup"); RegisterClass(&wc); reg = true; }
    g_hPopupWindow = CreateWindowEx(WS_EX_LAYERED|WS_EX_TOOLWINDOW|WS_EX_TOPMOST|WS_EX_NOACTIVATE, TEXT("WML_ArtPopup"), NULL, WS_POPUP|WS_VISIBLE, prc.left, trc.top-SZ-8, SZ, SZ, parent, NULL, GetModuleHandle(NULL), NULL);
    if (g_hPopupWindow) { SetLayeredWindowAttributes(g_hPopupWindow, 0, 230, LWA_ALPHA); DWM_WINDOW_CORNER_PREFERENCE pref = DWMWCP_ROUND; DwmSetWindowAttribute(g_hPopupWindow, DWMWA_WINDOW_CORNER_PREFERENCE, &pref, sizeof(pref)); }
}
void HideArtPopup() { if (g_hPopupWindow) { DestroyWindow(g_hPopupWindow); g_hPopupWindow = NULL; } }

// --- Lyrics ---
vector<LyricLine> ParseLRC(const string& lrc) {
    vector<LyricLine> lines;
    size_t pos = 0;
    while (pos < lrc.size()) {
        size_t nl = lrc.find('\n', pos);
        string line = lrc.substr(pos, nl == string::npos ? string::npos : nl - pos);
        pos = (nl == string::npos) ? lrc.size() : nl + 1;
        if (line.empty() || line[0] != '[') continue;
        size_t cb = line.find(']'); if (cb == string::npos) continue;
        string ts = line.substr(1, cb - 1);
        string text = (cb + 1 < line.size()) ? line.substr(cb + 1) : "";
        if (!text.empty() && text.back() == '\r') text.pop_back();
        int mm = 0, ss = 0, xx = 0;
        if (sscanf(ts.c_str(), "%d:%d.%d", &mm, &ss, &xx) >= 2) {
            int ms = mm * 60000 + ss * 1000 + xx * 10;
            int wlen = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, NULL, 0);
            wstring wtext(wlen, 0); MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, &wtext[0], wlen);
            if (!wtext.empty() && wtext.back() == 0) wtext.pop_back();
            if (!wtext.empty()) lines.push_back({ms, wtext});
        }
    }
    return lines;
}

string UrlEncode(const wstring& input) {
    int utf8len = WideCharToMultiByte(CP_UTF8, 0, input.c_str(), -1, NULL, 0, NULL, NULL);
    string utf8(utf8len, 0); WideCharToMultiByte(CP_UTF8, 0, input.c_str(), -1, &utf8[0], utf8len, NULL, NULL);
    if (!utf8.empty() && utf8.back() == 0) utf8.pop_back();
    string out;
    for (unsigned char c : utf8) {
        if (isalnum(c) || c=='-' || c=='_' || c=='.' || c=='~') out += c;
        else { char hex[4]; sprintf(hex, "%%%02X", c); out += hex; }
    }
    return out;
}

static string g_MxmToken;
static mutex  g_MxmTokenMutex;

string HttpGet(const wchar_t* host, const wchar_t* path, bool https = true) {
    string result;
    HINTERNET hSession = WinHttpOpen(L"TaskbarMusicLounge/1.0", WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, NULL, NULL, 0); if (!hSession) return result;
    HINTERNET hConnect = WinHttpConnect(hSession, host, https ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_HTTP_PORT, 0);
    if (!hConnect) { WinHttpCloseHandle(hSession); return result; }
    DWORD flags = https ? WINHTTP_FLAG_SECURE : 0;
    HINTERNET hReq = WinHttpOpenRequest(hConnect, L"GET", path, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, flags);
    if (hReq) {
        WinHttpAddRequestHeaders(hReq, L"X-Requested-With: XMLHttpRequest\r\nAuthority: apic-desktop.musixmatch.com", -1L, WINHTTP_ADDREQ_FLAG_ADD);
        if (WinHttpSendRequest(hReq, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) && WinHttpReceiveResponse(hReq, NULL)) {
            DWORD bytesRead = 0; char buf[8192];
            while (WinHttpReadData(hReq, buf, sizeof(buf)-1, &bytesRead) && bytesRead > 0) { buf[bytesRead] = 0; result += buf; }
        }
        WinHttpCloseHandle(hReq);
    }
    WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession);
    return result;
}

string JsonField(const string& json, const string& key) {
    string search = "\"" + key + "\":";
    size_t p = json.find(search); if (p == string::npos) return "";
    p += search.size(); while (p < json.size() && (json[p]==' '||json[p]=='\t')) p++;
    if (p >= json.size()) return "";
    if (json[p] == '"') {
        string val; p++;
        while (p < json.size() && json[p] != '"') {
            if (json[p]=='\\' && p+1 < json.size()) { p++; if (json[p]=='n') val+='\n'; else if (json[p]=='"') val+='"'; else val+=json[p]; }
            else val+=json[p]; p++;
        }
        return val;
    }
    if (json.substr(p,4)=="null") return ""; return "";
}

string GetMxmToken() {
    lock_guard<mutex> guard(g_MxmTokenMutex);
    if (!g_MxmToken.empty()) return g_MxmToken;
    wstring tokenPath = wstring(L"/ws/1.1/token.get?app_id=web-desktop-app-v1.0&format=json&t=") + to_wstring(GetTickCount());
    string resp = HttpGet(L"apic-desktop.musixmatch.com", tokenPath.c_str());
    if (resp.empty()) return "";
    g_MxmToken = JsonField(resp, "user_token");
    Wh_Log(L"[MXM] Token: %hs", g_MxmToken.empty() ? "(none)" : g_MxmToken.substr(0,12).c_str());
    return g_MxmToken;
}

string FetchMxmLyrics(const wstring& artist, const wstring& title) {
    string token = GetMxmToken(); if (token.empty()) return "";
    string aEnc = UrlEncode(artist), tEnc = UrlEncode(title), tok = UrlEncode(wstring(token.begin(), token.end()));
    string pathA = "/ws/1.1/macro.subtitles.get?app_id=web-desktop-app-v1.0&format=json&namespace=lyrics_richsynched&optional_calls=track.lyrics.get&q_artist=" + aEnc + "&q_track=" + tEnc + "&usertoken=" + tok;
    int wlen = MultiByteToWideChar(CP_UTF8, 0, pathA.c_str(), -1, NULL, 0); wstring path(wlen, 0);
    MultiByteToWideChar(CP_UTF8, 0, pathA.c_str(), -1, &path[0], wlen); if (!path.empty() && path.back()==0) path.pop_back();
    string resp = HttpGet(L"apic-desktop.musixmatch.com", path.c_str()); if (resp.empty()) return "";
    string lrc = JsonField(resp, "subtitle_body");
    if (lrc.empty() && resp.find("\"status_code\":402") != string::npos) { lock_guard<mutex> guard(g_MxmTokenMutex); g_MxmToken.clear(); }
    return lrc;
}

string HttpGetLRCLIB(const wstring& artist, const wstring& title) {
    string result;
    HINTERNET hSession = WinHttpOpen(L"TaskbarMusicLounge/1.0", WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, NULL, NULL, 0); if (!hSession) return result;
    HINTERNET hConnect = WinHttpConnect(hSession, L"lrclib.net", INTERNET_DEFAULT_HTTPS_PORT, 0); if (!hConnect) { WinHttpCloseHandle(hSession); return result; }
    string pathA = "/api/get?artist_name=" + UrlEncode(artist) + "&track_name=" + UrlEncode(title);
    int wlen = MultiByteToWideChar(CP_UTF8, 0, pathA.c_str(), -1, NULL, 0); wstring path(wlen, 0);
    MultiByteToWideChar(CP_UTF8, 0, pathA.c_str(), -1, &path[0], wlen); if (!path.empty() && path.back()==0) path.pop_back();
    HINTERNET hReq = WinHttpOpenRequest(hConnect, L"GET", path.c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
    if (hReq) {
        if (WinHttpSendRequest(hReq, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) && WinHttpReceiveResponse(hReq, NULL)) {
            DWORD bytesRead = 0; char buf[4096];
            while (WinHttpReadData(hReq, buf, sizeof(buf)-1, &bytesRead) && bytesRead > 0) { buf[bytesRead] = 0; result += buf; }
        }
        WinHttpCloseHandle(hReq);
    }
    WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession);
    return JsonField(result, "syncedLyrics");
}

void FetchLyricsAsync(wstring artist, wstring title) {
    if (artist.empty() && title.find(L" - ") != wstring::npos) { size_t sep = title.find(L" - "); artist = title.substr(0, sep); title = title.substr(sep + 3); }
    Wh_Log(L"[Lyrics] Fetching: %s - %s", artist.c_str(), title.c_str());
    int pref = g_LyricsSourcePref;
    { lock_guard<mutex> guard(g_LyricsMutex); g_LyricsLoading = true; g_LyricsNotFound = false;
      g_LyricsFetchDone = false; } // availability is reset on track change, not on source pref change
    if (g_hLyricsWindow) InvalidateRect(g_hLyricsWindow, NULL, FALSE);
    thread([artist, title, pref]() {
        // Always try both sources to determine availability
        string mxm    = FetchMxmLyrics(artist, title);
        string lrclib = HttpGetLRCLIB(artist, title);

        bool mxmOk    = !mxm.empty();
        bool lrclibOk = !lrclib.empty();

        // Choose which source to use based on user preference
        string lrc;
        if      (pref == 1) lrc = mxm;
        else if (pref == 2) lrc = lrclib;
        else                lrc = mxmOk ? mxm : lrclib; // auto: Mxm first

        // If the mod was unloaded while fetching, do not touch the UI
        if (!g_Running) return;

        lock_guard<mutex> guard(g_LyricsMutex);
        g_LyricsLoading = false;
        g_LyricsFetchDone = true;
        g_LyricsMxmAvail    = mxmOk;
        g_LyricsLrclibAvail = lrclibOk;
        if (lrc.empty()) { Wh_Log(L"[Lyrics] Not found in any source"); g_Lyrics.clear(); g_LyricsNotFound = true; g_LyricsNotFoundTick = GetTickCount(); }
        else { auto parsed = ParseLRC(lrc); Wh_Log(L"[Lyrics] %d lines (src=%s)", (int)parsed.size(), mxmOk&&lrc==mxm?L"MXM":L"LRCLIB"); g_Lyrics = parsed; g_LyricsNotFound = false; }
        g_LyricsTitle = title; g_LyricsArtist = artist;
        if (g_Running && g_hMediaWindow) InvalidateRect(g_hMediaWindow, NULL, FALSE);
        if (g_Running && g_hLyricsWindow) InvalidateRect(g_hLyricsWindow, NULL, FALSE);
    }).detach();
}

void GetLyricLines(int posMs, wstring& current, wstring& next) {
    lock_guard<mutex> guard(g_LyricsMutex);
    current = L""; next = L"";
    if (g_Lyrics.empty()) return;

    int newIdx = -1;
    for (int i = 0; i < (int)g_Lyrics.size(); i++) {
        if (g_Lyrics[i].ms <= posMs) newIdx = i;
        else break;
    }

    // posMs has not yet reached the first line's timestamp — current is empty, next = first line
    if (newIdx < 0) {
        g_LyricsLastIdx = -1;
        next = g_Lyrics[0].text;
        return;
    }

    // Hysteresis: only go backward if clearly a real seek (>5s back)
    if (newIdx < g_LyricsLastIdx && g_LyricsLastIdx < (int)g_Lyrics.size()) {
        int curLineMs = g_Lyrics[g_LyricsLastIdx].ms;
        if (posMs >= curLineMs - 5000)
            newIdx = g_LyricsLastIdx;
    }

    if (newIdx != g_LyricsLastIdx)
        Wh_Log(L"[LyricIdx] %d -> %d  posMs=%d", g_LyricsLastIdx, newIdx, posMs);

    g_LyricsLastIdx = newIdx;
    current = g_Lyrics[newIdx].text;
    if (newIdx + 1 < (int)g_Lyrics.size()) next = g_Lyrics[newIdx + 1].text;
}

bool HasLyrics() { lock_guard<mutex> guard(g_LyricsMutex); return !g_Lyrics.empty(); }

LRESULT CALLBACK LyricsWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_PAINT) {
        PAINTSTRUCT ps; HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc; GetClientRect(hwnd, &rc); int w = rc.right, h = rc.bottom;
        HDC memDC = CreateCompatibleDC(hdc); HBITMAP memBmp = CreateCompatibleBitmap(hdc, w, h); HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, memBmp);
        Graphics g(memDC); g.SetSmoothingMode(SmoothingModeAntiAlias); g.SetTextRenderingHint(TextRenderingHintAntiAlias); g.Clear(Color(0,0,0,0));

        bool hasLyrics, isLoading;
        { lock_guard<mutex> lg(g_LyricsMutex); hasLyrics = !g_Lyrics.empty(); isLoading = g_LyricsLoading; }
        DWORD tc = GetCurrentTextColor(); Color mainColor{tc}; FontFamily ff(FONT_NAME, nullptr);

        if (isLoading) {
            // Continuous phase — full cycle in 1.5 seconds
            float phase = fmodf((float)(GetTickCount()) / 1500.0f, 1.0f) * 3.0f; // 0..3
            const int dotR = 3;
            const int spacing = 16;
            int totalW = 2 * spacing + dotR * 2;
            int startDotX = (w - totalW) / 2;
            int dotY = h / 2 - dotR;
            for (int i = 0; i < 3; i++) {
                float diff = fmodf(fabsf(phase - (float)i), 3.0f);
                if (diff > 1.5f) diff = 3.0f - diff;
                float t = 1.0f - (diff / 1.5f);
                float smooth = (sinf(t * 3.14159f - 1.5708f) + 1.0f) * 0.5f;
                int cx = startDotX + i * spacing;
                BYTE alpha     = (BYTE)(40  + smooth * 200);
                BYTE glowAlpha = (BYTE)(10  + smooth * 65);
                int  glowExtra = (int)(2    + smooth * 5.0f);
                SolidBrush glowBrush{Color(glowAlpha, mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue())};
                g.FillEllipse(&glowBrush, cx - glowExtra, dotY - glowExtra, (dotR + glowExtra) * 2, (dotR + glowExtra) * 2);
                SolidBrush dotBrush{Color(alpha, mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue())};
                g.FillEllipse(&dotBrush, cx, dotY, dotR * 2, dotR * 2);
            }
        } else if (!hasLyrics) {
            Font fi(&ff, (REAL)(g_Settings.fontSize+2), FontStyleRegular, UnitPixel);
            SolidBrush db{Color(80,mainColor.GetRed(),mainColor.GetGreen(),mainColor.GetBlue())};
            StringFormat sf; sf.SetAlignment(StringAlignmentCenter); sf.SetLineAlignment(StringAlignmentCenter);
            g.DrawString(L"♪  No lyrics found", -1, &fi, RectF(0,0,(float)w,(float)h), &sf, &db);
        } else if (g_LyricCurLine.empty()) {
            float curAreaH = (float)(h * 0.58f);
            float nxtAreaY = curAreaH;
            float nxtAreaH = (float)(h * 0.40f);

            const int dotR = 3, spacing = 14;
            int dotX = 8 + dotR;
            int dotBaseY = (int)curAreaH - dotR * 2 - 6;
            float bounceAmt = 5.0f;

            for (int i = 0; i < 3; i++) {
                float t  = g_DotAnimT[i];
                float gv = g_DotGlow[i];
                int cx = dotX + i * spacing;
                int cy = dotBaseY - (int)(sinf(t * 3.14159f) * bounceAmt);

                if (gv > 0) {
                    SolidBrush g1{Color((BYTE)(15*gv), mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue())};
                    SolidBrush g2{Color((BYTE)(45*gv), mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue())};
                    SolidBrush g3{Color((BYTE)(80*gv), mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue())};
                    g.FillEllipse(&g1, cx-5, cy-5, (dotR+5)*2, (dotR+5)*2);
                    g.FillEllipse(&g2, cx-3, cy-3, (dotR+3)*2, (dotR+3)*2);
                    g.FillEllipse(&g3, cx-1, cy-1, (dotR+1)*2, (dotR+1)*2);
                }
                BYTE da = (BYTE)(60 + gv * 180);
                SolidBrush dotBrush{Color(da, mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue())};
                g.FillEllipse(&dotBrush, cx, cy, dotR * 2, dotR * 2);
            }

            if (!g_LyricNxtLine.empty()) {
                Font fontNxt(&ff, (REAL)g_Settings.fontSize, FontStyleRegular, UnitPixel);
                SolidBrush nxtBrush{Color(110, mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue())};
                StringFormat sf; sf.SetAlignment(StringAlignmentNear); sf.SetTrimming(StringTrimmingNone); sf.SetLineAlignment(StringAlignmentNear);
                g.DrawString(g_LyricNxtLine.c_str(), -1, &fontNxt, RectF(8, nxtAreaY, (float)(w-16), nxtAreaH), &sf, &nxtBrush);
            }
        } else {
            Font fontCur(&ff, (REAL)(g_Settings.fontSize+1), FontStyleBold, UnitPixel);
            Font fontNxt(&ff, (REAL)g_Settings.fontSize, FontStyleRegular, UnitPixel);
            StringFormat sf; sf.SetAlignment(StringAlignmentNear); sf.SetTrimming(StringTrimmingNone);

            float ease = (sinf(g_LyricFadeT * 3.14159f - 1.5708f) + 1.0f) * 0.5f;
            float curAreaH = (float)(h * 0.58f);
            float nxtAreaY = curAreaH;
            float nxtAreaH = (float)(h * 0.40f);
            float slideAmt = 14.0f;

            g.SetClip(RectF(0, 0, (float)w, curAreaH));
            sf.SetLineAlignment(StringAlignmentFar);
            if (!g_LyricPrevLine.empty() && ease < 1.0f) {
                BYTE a = (BYTE)(240 * (1.0f - ease));
                SolidBrush prevBrush{Color(a, mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue())};
                g.DrawString(g_LyricPrevLine.c_str(), -1, &fontCur,
                    RectF(8, -slideAmt * ease, (float)(w-16), curAreaH), &sf, &prevBrush);
            }
            {
                BYTE a = (BYTE)(240 * ease);
                SolidBrush curBrush{Color(a, mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue())};
                g.DrawString(g_LyricCurLine.c_str(), -1, &fontCur,
                    RectF(8, slideAmt * (1.0f - ease), (float)(w-16), curAreaH), &sf, &curBrush);
            }

            g.SetClip(RectF(0, nxtAreaY, (float)w, nxtAreaH));
            sf.SetLineAlignment(StringAlignmentNear);
            if (!g_LyricPrevNxtLine.empty() && ease < 1.0f) {
                BYTE a = (BYTE)(110 * (1.0f - ease));
                SolidBrush prevNxtBrush{Color(a, mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue())};
                g.DrawString(g_LyricPrevNxtLine.c_str(), -1, &fontNxt,
                    RectF(8, nxtAreaY - slideAmt * ease, (float)(w-16), nxtAreaH), &sf, &prevNxtBrush);
            }
            if (!g_LyricNxtLine.empty()) {
                BYTE a = (BYTE)(110 * ease);
                SolidBrush nxtBrush{Color(a, mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue())};
                g.DrawString(g_LyricNxtLine.c_str(), -1, &fontNxt,
                    RectF(8, nxtAreaY + slideAmt * (1.0f - ease), (float)(w-16), nxtAreaH), &sf, &nxtBrush);
            }
            g.ResetClip();
        }

        BitBlt(hdc, 0, 0, w, h, memDC, 0, 0, SRCCOPY);
        SelectObject(memDC, oldBmp); DeleteObject(memBmp); DeleteDC(memDC);
        EndPaint(hwnd, &ps); return 0;
    }
    if (msg == WM_ERASEBKGND) return 1;
    if (msg == WM_TIMER) {
        // ★ SLIDE BLOCK REMOVED ★
        // Slide animation is now managed exclusively by IDT_TASKBAR (MediaWndProc).
        // The duplicate code here conflicted with g_LyricsSlideX — removed.

        // fade in quick, fade out slow
        if(g_LyricsFadeAlpha < g_LyricsTargetAlpha)      g_LyricsFadeAlpha = min(255, g_LyricsFadeAlpha + 25);
        else if(g_LyricsFadeAlpha > g_LyricsTargetAlpha) g_LyricsFadeAlpha = max(0,   g_LyricsFadeAlpha - 8);

        // Hover check
        POINT pt; GetCursorPos(&pt);
        RECT rc2; GetWindowRect(hwnd, &rc2);
        bool hover = (pt.x >= rc2.left && pt.x < rc2.right && pt.y >= rc2.top && pt.y < rc2.bottom);
        BYTE baseAlpha = (hover && !g_hContextMenu) ? 30 : 255;
        BYTE finalAlpha = (BYTE)((int)baseAlpha * g_LyricsFadeAlpha / 255);
        SetLayeredWindowAttributes(hwnd, 0, finalAlpha, LWA_ALPHA);

        // Detect line changes here — do not leave it to the paint handler
        {
            int posMs = 0;
            DWORD now = GetTickCount();
            DWORD elapsed = (g_LyricsPosUpdateTick > 0) ? (now - g_LyricsPosUpdateTick) : 0;
            if (elapsed > 6000) elapsed = 6000;
            int interp = g_LyricsLastPosMs + (g_LyricsIsPlaying ? (int)elapsed : 0);
            posMs = interp + g_Settings.lyricsOffsetMs;
            if (posMs < 0) posMs = 0;

            wstring newCur, newNxt;
            GetLyricLines(posMs, newCur, newNxt);

            if (newCur != g_LyricCurLine || g_LyricsLastIdx != g_LyricsDisplayIdx) {
                g_LyricFadeT       = 1.0f;
                g_LyricPrevLine    = g_LyricCurLine;
                g_LyricPrevNxtLine = g_LyricNxtLine;
                g_LyricCurLine     = newCur;
                g_LyricNxtLine     = newNxt;
                g_LyricFadeT       = 0.0f;
                g_LyricsDisplayIdx = g_LyricsLastIdx;
                if (newCur.empty()) {
                    float fLyricMs = 1.0f;
                    { lock_guard<mutex> lg(g_LyricsMutex); if (!g_Lyrics.empty()) fLyricMs = (float)g_Lyrics[0].ms; }
                    if (fLyricMs < 1.0f) fLyricMs = 1.0f;
                    float ph = ((float)posMs / fLyricMs) * 3.0f;
                    ph = max(0.0f, min(2.999f, ph));
                    for (int i = 0; i < 3; i++) {
                        float loc = ph - (float)i;
                        g_DotAnimT[i]      = (loc >= 1.0f) ? 1.0f : max(0.0f, loc);
                        g_DotGlow[i]       = (loc >= 1.0f) ? 1.0f : min(1.0f, g_DotAnimT[i] * 3.0f);
                        g_DotStartTick[i]  = 0;
                        g_DotDurationMs[i] = 0;
                    }
                }
            } else {
                g_LyricNxtLine = newNxt;
            }
            g_LyricFadeT = min(1.0f, g_LyricFadeT + 0.12f);

            if (g_LyricCurLine.empty()) {
                float fLyricMs = 1.0f;
                { lock_guard<mutex> lg(g_LyricsMutex); if (!g_Lyrics.empty()) fLyricMs = (float)g_Lyrics[0].ms; }
                if (fLyricMs < 1.0f) fLyricMs = 1.0f;
                float ph2 = ((float)posMs / fLyricMs) * 3.0f;
                ph2 = max(0.0f, min(2.999f, ph2));
                float dotDurMs2 = fLyricMs / 3.0f;

                for (int i = 0; i < 3; i++) {
                    float loc = ph2 - (float)i;
                    float expected = (loc >= 1.0f) ? 1.0f : max(0.0f, loc);
                    if (g_DotAnimT[i] > expected + 0.15f) {
                        g_DotAnimT[i] = expected;
                        g_DotGlow[i]  = min(1.0f, expected * 3.0f);
                    }
                }

                static DWORD s_lastDotTick = 0;
                DWORD nowDot = GetTickCount();
                float dtMs = (s_lastDotTick > 0) ? (float)(nowDot - s_lastDotTick) : 16.0f;
                s_lastDotTick = nowDot;
                for (int i = 0; i < 3; i++) {
                    float loc = ph2 - (float)i;
                    if (loc >= 1.0f) {
                        g_DotAnimT[i] = 1.0f;
                        g_DotGlow[i]  = 1.0f;
                    } else if (loc > 0 && g_LyricsIsPlaying && dotDurMs2 > 0) {
                        float step = dtMs / dotDurMs2;
                        g_DotAnimT[i] = min(1.0f, g_DotAnimT[i] + step);
                        g_DotGlow[i]  = min(1.0f, g_DotAnimT[i] * 3.0f);
                    }
                }
            }
        }

        bool needsFastTimer = (g_LyricFadeT < 1.0f) || !g_LyricCurLine.empty() || g_LyricsLoading;
        UINT newInterval = needsFastTimer ? 16 : 50;
        static UINT s_curInterval = 16;
        if (newInterval != s_curInterval) {
            s_curInterval = newInterval;
            SetTimer(hwnd, 302, newInterval, NULL);
        }

        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void UpdateLyricsWindowPos() {
    if (!g_hLyricsWindow || !g_hMediaWindow) return;
    RECT mrc; GetWindowRect(g_hMediaWindow, &mrc);
    int lx = mrc.left + g_LyricsSlideX;
    SetWindowPos(g_hLyricsWindow, HWND_TOPMOST, lx, mrc.top - g_Settings.height*2 - 2, g_Settings.width, g_Settings.height*2, SWP_NOACTIVATE);
}

void ShowLyricsWindow() {
    if (g_hLyricsWindow) return;
    static bool reg = false;
    if (!reg) { WNDCLASS wc = {}; wc.lpfnWndProc = LyricsWndProc; wc.hInstance = GetModuleHandle(NULL); wc.lpszClassName = TEXT("WML_Lyrics"); RegisterClass(&wc); reg = true; }
    g_hLyricsWindow = CreateWindowEx(WS_EX_LAYERED|WS_EX_TOOLWINDOW|WS_EX_TOPMOST|WS_EX_NOACTIVATE|WS_EX_TRANSPARENT, TEXT("WML_Lyrics"), NULL, WS_POPUP|WS_VISIBLE, 0, 0, g_Settings.width, g_Settings.height*2, g_hMediaWindow, NULL, GetModuleHandle(NULL), NULL);
    if (g_hLyricsWindow) {
        SetLayeredWindowAttributes(g_hLyricsWindow, 0, 255, LWA_ALPHA);
        HMODULE hUser = GetModuleHandle(L"user32.dll"); typedef BOOL(WINAPI* pSWCA)(HWND, void*); auto SetComp = hUser ? (pSWCA)GetProcAddress(hUser, "SetWindowCompositionAttribute") : nullptr;
        if (SetComp) { struct { int a; void* d; size_t s; } d; struct { int st; DWORD f; DWORD c; DWORD an; } p = {4,0,0x40000000,0}; d = {19,&p,sizeof(p)}; SetComp(g_hLyricsWindow, &d); }
        DWM_WINDOW_CORNER_PREFERENCE pref = DWMWCP_ROUND; DwmSetWindowAttribute(g_hLyricsWindow, DWMWA_WINDOW_CORNER_PREFERENCE, &pref, sizeof(pref));
        BOOL bNoAnim = TRUE; DwmSetWindowAttribute(g_hLyricsWindow, DWMWA_TRANSITIONS_FORCEDISABLED, &bNoAnim, sizeof(bNoAnim));
        SetTimer(g_hLyricsWindow, 302, 16, NULL);
        UpdateLyricsWindowPos();
    }
    g_LyricsVisible = true;
}

void HideLyricsWindow() { if (g_hLyricsWindow) { DestroyWindow(g_hLyricsWindow); g_hLyricsWindow = NULL; } g_LyricsVisible = false; }

bool IsSystemLightMode() {
    DWORD value = 0, size = sizeof(value);
    return RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", L"SystemUsesLightTheme", RRF_RT_DWORD, nullptr, &value, &size) == ERROR_SUCCESS && value != 0;
}

DWORD GetCurrentTextColor() {
    return IsSystemLightMode() ? 0xFF000000 : 0xFFFFFFFF;
}

void UpdateAppearance(HWND hwnd) {
    DWM_WINDOW_CORNER_PREFERENCE pref = g_Settings.displayNative ? DWMWCP_DONOTROUND : DWMWCP_ROUND;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &pref, sizeof(pref));
    BOOL bNoAnim = TRUE; DwmSetWindowAttribute(hwnd, DWMWA_TRANSITIONS_FORCEDISABLED, &bNoAnim, sizeof(bNoAnim));
    HMODULE hUser = GetModuleHandle(L"user32.dll");
    if (hUser) {
        auto SetComp = (pSetWindowCompositionAttribute)GetProcAddress(hUser, "SetWindowCompositionAttribute");
        if (SetComp) {
            bool light = IsSystemLightMode();
            DWORD tint = (g_Settings.bgOpacity > 0)
                ? ((g_Settings.bgOpacity << 24) | (light ? 0xFFFFFF : 0x000000))
                : (light ? 0x18FFFFFF : 0x18000000);
            ACCENT_POLICY policy = { ACCENT_ENABLE_ACRYLICBLURBEHIND, 0, tint, 0 };
            WINDOWCOMPOSITIONATTRIBDATA data = { WCA_ACCENT_POLICY, &policy, sizeof(policy) };
            SetComp(hwnd, &data);
        }
    }
}


// --- Context Menu ---

struct CMenuItem { int id; wstring text; bool checked; wstring appId; bool disabled = false; };
static vector<CMenuItem> g_MenuItems;
static vector<CMenuItem> g_SubMenuItems;
HWND g_hSubMenu = NULL;

static const int CM_W      = 220;
static const int CM_ITEM_H = 34;
static const int CM_LBL_H  = 24;
static const int CM_SEP_H  = 10;
static const int CM_VPAD   = 6;
static const int CM_PX     = 12;

static int CM_TotalH() {
    int h = CM_VPAD * 2;
    for (auto& it : g_MenuItems) h += (it.id==0)?CM_SEP_H:(it.id==-1)?CM_LBL_H:CM_ITEM_H;
    return h;
}


void ShowLyricsSubMenu(HWND parent) {
    if (g_hSubMenu) { DestroyWindow(g_hSubMenu); g_hSubMenu=NULL; }
    g_SubMenuItems.clear();

    bool fetchDone, mxmOk, lrclibOk;
    { lock_guard<mutex> lg(g_LyricsMutex);
      fetchDone  = g_LyricsFetchDone;
      mxmOk      = g_LyricsMxmAvail;
      lrclibOk   = g_LyricsLrclibAvail; }

    bool disableMxm    = fetchDone && !mxmOk;
    bool disableLrclib = fetchDone && !lrclibOk;

    wstring mxmLabel    = disableMxm    ? L"Musixmatch  \u2715" : L"Musixmatch";
    wstring lrclibLabel = disableLrclib ? L"LRCLIB  \u2715"     : L"LRCLIB";

    g_SubMenuItems.push_back({-1, L"Source"});
    g_SubMenuItems.push_back({206, mxmLabel,    g_LyricsSourcePref==1, L"", disableMxm});
    g_SubMenuItems.push_back({207, L"Auto",     g_LyricsSourcePref==0});
    g_SubMenuItems.push_back({208, lrclibLabel, g_LyricsSourcePref==2, L"", disableLrclib});

    int sh = CM_VPAD * 2;
    for (auto& it : g_SubMenuItems) sh += (it.id==0)?CM_SEP_H:(it.id==-1)?CM_LBL_H:CM_ITEM_H;

    RECT prc; GetWindowRect(parent, &prc);
    int sx = prc.right + 2;
    int sy = prc.bottom - sh;

    int screenW = GetSystemMetrics(SM_CXSCREEN);
    if (sx + CM_W > screenW) sx = prc.left - CM_W - 2;

    g_hSubMenu = CreateWindowEx(WS_EX_LAYERED|WS_EX_TOOLWINDOW|WS_EX_TOPMOST|WS_EX_NOACTIVATE,
        TEXT("WML_ContextMenu"), NULL, WS_POPUP|WS_VISIBLE, sx, sy, CM_W, sh,
        g_hContextMenu, NULL, GetModuleHandle(NULL), NULL);
    if (g_hSubMenu) {
        SetLayeredWindowAttributes(g_hSubMenu, 0, 0, LWA_ALPHA);
        HMODULE hU=GetModuleHandle(L"user32.dll"); typedef BOOL(WINAPI* pSWCA)(HWND,void*);
        auto SC=hU?(pSWCA)GetProcAddress(hU,"SetWindowCompositionAttribute"):nullptr;
        if (SC) { struct{int a;void*d;size_t s;}d2; struct{int st;DWORD f;DWORD c;DWORD an;}p={4,0,0x40000000,0}; d2={19,&p,sizeof(p)}; SC(g_hSubMenu,&d2); }
        DWM_WINDOW_CORNER_PREFERENCE cp=DWMWCP_ROUND; DwmSetWindowAttribute(g_hSubMenu,DWMWA_WINDOW_CORNER_PREFERENCE,&cp,sizeof(cp));
        BOOL bA=TRUE; DwmSetWindowAttribute(g_hSubMenu,DWMWA_TRANSITIONS_FORCEDISABLED,&bA,sizeof(bA));
        SetTimer(g_hSubMenu, 401, 16, NULL);
    }
}

void ExecuteMenuCmd(int cmd, const wstring& appId) {
    Wh_Log(L"[Menu] cmd=%d appId=%s", cmd, appId.empty()?L"(none)":appId.c_str());
    using R = Windows::Media::MediaPlaybackAutoRepeatMode;
    if (cmd >= 100 && cmd < 200) {
        g_ManualSource = true; g_ManualSourceId = appId;
        { lock_guard<mutex> gd(g_MediaState.lock); g_MediaState.sourceAppId = appId; }
    } else if (cmd == 200) { SendShuffleToggle(); }
    else if (cmd == 201) { try { auto ss=g_SessionManager.GetSessions(); for(auto const& s:ss){ wstring id; {lock_guard<mutex> gd(g_MediaState.lock);id=g_MediaState.sourceAppId;} if(wstring(s.SourceAppUserModelId().c_str())==id){s.TryChangeAutoRepeatModeAsync(R::None);break;} } } catch(...){} }
    else if (cmd == 202) { try { auto ss=g_SessionManager.GetSessions(); for(auto const& s:ss){ wstring id; {lock_guard<mutex> gd(g_MediaState.lock);id=g_MediaState.sourceAppId;} if(wstring(s.SourceAppUserModelId().c_str())==id){s.TryChangeAutoRepeatModeAsync(R::List);break;} } } catch(...){} }
    else if (cmd == 203) { try { auto ss=g_SessionManager.GetSessions(); for(auto const& s:ss){ wstring id; {lock_guard<mutex> gd(g_MediaState.lock);id=g_MediaState.sourceAppId;} if(wstring(s.SourceAppUserModelId().c_str())==id){s.TryChangeAutoRepeatModeAsync(R::Track);break;} } } catch(...){} }
    else if (cmd == 204) { g_MiniMode=!g_MiniMode; int nw=g_MiniMode?g_Settings.height:g_Settings.width; SetWindowPos(g_hMediaWindow,HWND_TOPMOST,0,0,nw,GetWidgetDrawHeight(),SWP_NOMOVE|SWP_NOACTIVATE); InvalidateRect(g_hMediaWindow,NULL,TRUE); SaveUIState(); }
    else if (cmd == 205) { if(g_LyricsVisible) HideLyricsWindow(); else ShowLyricsWindow(); SaveUIState(); }
    else if (cmd == 211) { ShowLyricsSubMenu(g_hContextMenu); }
    else if (cmd == 206) {
        g_LyricsSourcePref = (g_LyricsSourcePref == 1) ? 0 : 1;
        wstring a,t; {lock_guard<mutex> lg(g_LyricsMutex); a=g_LyricsArtist; t=g_LyricsTitle;}
        if (!t.empty()) FetchLyricsAsync(a, t);
        SaveUIState();
    }
    else if (cmd == 207) {
        g_LyricsSourcePref = 0;
        wstring a,t; {lock_guard<mutex> lg(g_LyricsMutex); a=g_LyricsArtist; t=g_LyricsTitle;}
        if (!t.empty()) FetchLyricsAsync(a, t);
        SaveUIState();
    }
    else if (cmd == 208) {
        g_LyricsSourcePref = (g_LyricsSourcePref == 2) ? 0 : 2;
        wstring a,t; {lock_guard<mutex> lg(g_LyricsMutex); a=g_LyricsArtist; t=g_LyricsTitle;}
        if (!t.empty()) FetchLyricsAsync(a, t);
        SaveUIState();
    }
}

LRESULT CALLBACK CustomMenuWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    auto& items = (hwnd == g_hSubMenu) ? g_SubMenuItems : g_MenuItems;
    struct CMS { int hover; DWORD leaveAt; int alpha; bool fadingOut; };
    auto GetS = [&]() -> CMS* { return (CMS*)GetWindowLongPtr(hwnd, GWLP_USERDATA); };

    switch (msg) {
    case WM_CREATE: {
        auto* s = new CMS{-1, 0, 0, false};
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)s);
        return 0;
    }
    case WM_PAINT: {
        auto* s = GetS();
        PAINTSTRUCT ps; HDC hdc = BeginPaint(hwnd, &ps);
        RECT rcl; GetClientRect(hwnd, &rcl); int W=rcl.right, H=rcl.bottom;
        HDC mem = CreateCompatibleDC(hdc); HBITMAP bmp = CreateCompatibleBitmap(hdc, W, H); HBITMAP obmp = (HBITMAP)SelectObject(mem, bmp);
        Graphics g(mem); g.SetSmoothingMode(SmoothingModeAntiAlias); g.SetTextRenderingHint(TextRenderingHintAntiAlias); g.Clear(Color(0,0,0,0));
        DWORD tc = GetCurrentTextColor(); Color mc{tc};
        FontFamily ff(FONT_NAME, nullptr);
        Font fN(&ff, (REAL)(g_Settings.fontSize+1), FontStyleBold, UnitPixel);
        Font fS(&ff, (REAL)g_Settings.fontSize, FontStyleRegular, UnitPixel);
        int cy = CM_VPAD;
        for (int i = 0; i < (int)items.size(); i++) {
            auto& it = items[i];
            if (it.id == 0) {
                SolidBrush sb{Color(55,mc.GetRed(),mc.GetGreen(),mc.GetBlue())};
                g.FillRectangle(&sb,(float)CM_PX,(float)cy+CM_SEP_H/2.0f,(float)(W-CM_PX*2),1.0f);
                cy += CM_SEP_H;
            } else if (it.id == -1) {
                SolidBrush lb{Color(130,mc.GetRed(),mc.GetGreen(),mc.GetBlue())};
                StringFormat sf; sf.SetAlignment(StringAlignmentNear); sf.SetLineAlignment(StringAlignmentCenter);
                g.DrawString(it.text.c_str(),-1,&fS,RectF((float)CM_PX,(float)cy,(float)(W-CM_PX*2),(float)CM_LBL_H),&sf,&lb);
                cy += CM_LBL_H;
            } else {
                if (s && i == s->hover && !it.disabled) { SolidBrush hb{Color(45,mc.GetRed(),mc.GetGreen(),mc.GetBlue())}; g.FillRectangle(&hb,CM_PX/2.0f,(float)(cy+1),(float)(W-CM_PX),(float)(CM_ITEM_H-2)); }
                if (it.checked) { SolidBrush cb{Color(230,mc.GetRed(),mc.GetGreen(),mc.GetBlue())}; StringFormat sf; sf.SetAlignment(StringAlignmentCenter); sf.SetLineAlignment(StringAlignmentCenter); g.DrawString(L"\u2713",-1,&fN,RectF((float)CM_PX,(float)cy,20.0f,(float)CM_ITEM_H),&sf,&cb); }
                BYTE textAlpha = it.disabled ? 70 : 220;
                SolidBrush tb{Color(textAlpha,mc.GetRed(),mc.GetGreen(),mc.GetBlue())};
                StringFormat sf; sf.SetAlignment(StringAlignmentNear); sf.SetLineAlignment(StringAlignmentCenter); sf.SetFormatFlags(StringFormatFlagsNoWrap); sf.SetTrimming(StringTrimmingEllipsisCharacter);
                g.DrawString(it.text.c_str(),-1,&fN,RectF((float)(CM_PX+22),(float)cy,(float)(W-CM_PX-22-CM_PX),(float)CM_ITEM_H),&sf,&tb);
                cy += CM_ITEM_H;
            }
        }
        BitBlt(hdc,0,0,W,H,mem,0,0,SRCCOPY); SelectObject(mem,obmp); DeleteObject(bmp); DeleteDC(mem);
        EndPaint(hwnd, &ps); return 0;
    }
    case WM_MOUSEMOVE: {
        auto* s = GetS(); if (!s) return 0;
        int y=HIWORD(lParam), cy=CM_VPAD, nh=-1;
        for (int i=0;i<(int)items.size();i++) {
            auto& it=items[i]; int ih=(it.id==0)?CM_SEP_H:(it.id==-1)?CM_LBL_H:CM_ITEM_H;
            if (it.id>0 && !it.disabled && y>=cy && y<cy+ih) { nh=i; break; }
            cy += ih;
        }
        if (nh!=s->hover) { s->hover=nh; InvalidateRect(hwnd,NULL,FALSE); }
        s->leaveAt = 0; s->fadingOut = false;
        TRACKMOUSEEVENT tme={sizeof(tme),TME_LEAVE,hwnd,0}; TrackMouseEvent(&tme);
        return 0;
    }
    case WM_MOUSELEAVE: {
        auto* s = GetS();
        if (s) {
            if (hwnd == g_hSubMenu) {
                s->hover=-1; s->leaveAt=GetTickCount();
            } else {
                s->hover=-1;
                if (!g_hSubMenu) s->leaveAt=GetTickCount();
            }
        }
        InvalidateRect(hwnd,NULL,FALSE); return 0;
    }
    case WM_LBUTTONUP: {
        int y=HIWORD(lParam), cy=CM_VPAD, cmd=-1; wstring aid;
        for (auto& it:items) {
            int ih=(it.id==0)?CM_SEP_H:(it.id==-1)?CM_LBL_H:CM_ITEM_H;
            if (it.id>0 && !it.disabled && y>=cy && y<cy+ih) { cmd=it.id; aid=it.appId; break; }
            cy += ih;
        }
        if (cmd > 0) {
            if (cmd == 211) {
                ExecuteMenuCmd(cmd, aid);
                InvalidateRect(hwnd, NULL, FALSE);
                return 0;
            }
            ExecuteMenuCmd(cmd, aid);
            if (cmd >= 100 && cmd < 200) UpdateMediaInfo();
            wstring actualSrc; { lock_guard<mutex> gd(g_MediaState.lock); actualSrc = g_MediaState.sourceAppId; }
            for (auto* list : {&g_MenuItems, &g_SubMenuItems}) {
                for (auto& it2 : *list) {
                    if (cmd == 200 && it2.id == 200) { it2.checked = !it2.checked; }
                    else if (cmd >= 201 && cmd <= 203 && it2.id >= 201 && it2.id <= 203) { it2.checked = (it2.id == cmd); }
                    else if (cmd == 204 && it2.id == 204) { it2.checked = g_MiniMode; }
                    else if (cmd == 205 && it2.id == 205) { it2.checked = g_LyricsVisible; }
                    else if (cmd >= 206 && cmd <= 208 && it2.id >= 206 && it2.id <= 208) { it2.checked = (it2.id == 206 && g_LyricsSourcePref==1) || (it2.id==207 && g_LyricsSourcePref==0) || (it2.id==208 && g_LyricsSourcePref==2); }
                    else if (it2.id >= 100 && it2.id < 200) { it2.checked = (it2.appId == actualSrc); }
                }
            }
            if (g_hSubMenu) InvalidateRect(g_hSubMenu, NULL, FALSE);
            InvalidateRect(hwnd, NULL, FALSE);
        }
        return 0;
    }
    case WM_TIMER: {
        auto* s = GetS(); if (!s) return 0;
        if (!s->fadingOut && s->alpha < 255) {
            s->alpha = min(255, s->alpha + 22);
            SetLayeredWindowAttributes(hwnd, 0, (BYTE)s->alpha, LWA_ALPHA);
            return 0;
        }
        if (!s->fadingOut) {
            bool mouseOnSubOrSelf = false;
            POINT pt2; GetCursorPos(&pt2);
            RECT rc2; GetWindowRect(hwnd, &rc2);
            if (pt2.x>=rc2.left&&pt2.x<rc2.right&&pt2.y>=rc2.top&&pt2.y<rc2.bottom)
                mouseOnSubOrSelf = true;
            if (!mouseOnSubOrSelf && g_hSubMenu && hwnd==g_hContextMenu) {
                RECT sr; GetWindowRect(g_hSubMenu, &sr);
                if (pt2.x>=sr.left&&pt2.x<sr.right&&pt2.y>=sr.top&&pt2.y<sr.bottom) {
                    mouseOnSubOrSelf = true;
                    s->leaveAt = 0;
                }
            }
            bool outsideClick = (GetAsyncKeyState(VK_LBUTTON)&0x8000) != 0;
            if (outsideClick && !mouseOnSubOrSelf) {
                POINT pt; GetCursorPos(&pt); RECT rc; GetWindowRect(hwnd,&rc);
                if (pt.x<rc.left||pt.x>rc.right||pt.y<rc.top||pt.y>rc.bottom) s->fadingOut = true;
            }
            if (s->leaveAt>0 && GetTickCount()-s->leaveAt>=2000 && !mouseOnSubOrSelf) s->fadingOut = true;
        }
        if (s->fadingOut) {
            s->alpha = max(0, s->alpha - 18);
            SetLayeredWindowAttributes(hwnd, 0, (BYTE)s->alpha, LWA_ALPHA);
            if (s->alpha <= 0) { DestroyWindow(hwnd); return 0; }
        }
        return 0;
    }
    case WM_DESTROY: {
        auto* s = GetS(); delete s; SetWindowLongPtr(hwnd, GWLP_USERDATA, 0);
        if (hwnd == g_hContextMenu) { if(g_hSubMenu){DestroyWindow(g_hSubMenu);g_hSubMenu=NULL;} g_hContextMenu=NULL; }
        else if (hwnd == g_hSubMenu) g_hSubMenu=NULL;
        return 0;
    }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void ShowCustomContextMenu(HWND parent) {
    if (g_hContextMenu) { DestroyWindow(g_hContextMenu); g_hContextMenu=NULL; }
    g_MenuItems.clear();
    g_MenuItems.push_back({-1, L"Media Source"});
    if (g_SessionManager) {
        try {
            wstring cid; { lock_guard<mutex> gd(g_MediaState.lock); cid = g_MediaState.sourceAppId; }
            auto sessions = g_SessionManager.GetSessions(); int idx=100;
            for (auto const& s:sessions) {
                wstring appId=s.SourceAppUserModelId().c_str();
                auto props=s.TryGetMediaPropertiesAsync().get();
                wstring title=props.Title().c_str(); if(title.empty()) title=appId;
                g_MenuItems.push_back({idx++, title, appId==cid, appId});
            }
        } catch (...) {}
    }
    g_MenuItems.push_back({0});
    bool isShuffle; int repeatMode;
    { lock_guard<mutex> gd(g_MediaState.lock); isShuffle=g_MediaState.isShuffle; repeatMode=g_MediaState.repeatMode; }
    g_MenuItems.push_back({200, L"Shuffle",  isShuffle});
    g_MenuItems.push_back({-1,  L"Repeat"});
    g_MenuItems.push_back({201, L"Off",    repeatMode==0});
    g_MenuItems.push_back({202, L"All",      repeatMode==1});
    g_MenuItems.push_back({203, L"This Song",  repeatMode==2});
    g_MenuItems.push_back({0});
    g_MenuItems.push_back({204, L"Mini Mode",      g_MiniMode});
    g_MenuItems.push_back({205, L"Show Lyrics",    g_LyricsVisible});
    g_MenuItems.push_back({211, L"Lyrics Settings \u203a"});
    int mh = CM_TotalH();
    RECT prc; GetWindowRect(parent, &prc);
    int mx, my;
    if (g_MiniMode && g_hLyricsWindow && g_LyricsVisible && IsWindowVisible(g_hLyricsWindow)) {
        RECT lrc; GetWindowRect(g_hLyricsWindow, &lrc);
        mx = lrc.left;
        my = lrc.top - mh - 2;
    } else {
        mx = prc.right + 2;
        my = prc.bottom - mh;
    }
    static bool reg = false;
    if (!reg) { WNDCLASS wc={}; wc.lpfnWndProc=CustomMenuWndProc; wc.hInstance=GetModuleHandle(NULL); wc.lpszClassName=TEXT("WML_ContextMenu"); wc.hCursor=LoadCursor(NULL,IDC_ARROW); RegisterClass(&wc); reg=true; }
    g_hContextMenu = CreateWindowEx(WS_EX_LAYERED|WS_EX_TOOLWINDOW|WS_EX_TOPMOST|WS_EX_NOACTIVATE,
        TEXT("WML_ContextMenu"), NULL, WS_POPUP|WS_VISIBLE, mx, my, CM_W, mh,
        parent, NULL, GetModuleHandle(NULL), NULL);
    if (g_hContextMenu) {
        SetLayeredWindowAttributes(g_hContextMenu, 0, 0, LWA_ALPHA);
        HMODULE hU=GetModuleHandle(L"user32.dll"); typedef BOOL(WINAPI* pSWCA)(HWND,void*);
        auto SC=hU?(pSWCA)GetProcAddress(hU,"SetWindowCompositionAttribute"):nullptr;
        if (SC) { struct{int a;void*d;size_t s;}d2; struct{int st;DWORD f;DWORD c;DWORD an;}p={4,0,0x40000000,0}; d2={19,&p,sizeof(p)}; SC(g_hContextMenu,&d2); }
        DWM_WINDOW_CORNER_PREFERENCE pref=DWMWCP_ROUND; DwmSetWindowAttribute(g_hContextMenu,DWMWA_WINDOW_CORNER_PREFERENCE,&pref,sizeof(pref));
        BOOL bA=TRUE; DwmSetWindowAttribute(g_hContextMenu,DWMWA_TRANSITIONS_FORCEDISABLED,&bA,sizeof(bA));
        SetTimer(g_hContextMenu, 401, 16, NULL);
    }
}
// ─────────────────────────────────────────────────────────────────────────────
void AddRoundedRect(GraphicsPath& path, int x, int y, int w, int h, int r) {
    int d = r * 2;
    path.AddArc(x, y, d, d, 180, 90);
    path.AddArc(x + w - d, y, d, d, 270, 90);
    path.AddArc(x + w - d, y + h - d, d, d, 0, 90);
    path.AddArc(x, y + h - d, d, d, 90, 90);
    path.CloseFigure();
}

void DrawMediaPanel(HDC hdc, int width, int height) {
    Graphics g(hdc); g.SetSmoothingMode(SmoothingModeAntiAlias); g.SetTextRenderingHint(TextRenderingHintAntiAlias); g.Clear(Color(0,0,0,0));
    Color mainColor{ GetCurrentTextColor() };
    MediaState state;
    { lock_guard<mutex> guard(g_MediaState.lock); state.title=g_MediaState.title; state.artist=g_MediaState.artist; state.albumArt=g_MediaState.albumArt?g_MediaState.albumArt->Clone():nullptr; state.hasMedia=g_MediaState.hasMedia; state.isPlaying=g_MediaState.isPlaying; state.progressRatio=g_MediaState.progressRatio; state.hasProgress=g_MediaState.hasProgress; }

    if (g_MiniMode) {
        const int border=4; const float penW=3.5f; const int artPad=border+(int)penW+1;
        const int side=min(width,height);
        {
            GraphicsPath artPath;
            AddRoundedRect(artPath, artPad, artPad, width-artPad*2, height-artPad*2, 6);
            if(state.albumArt){
                g.SetClip(&artPath);
                g.DrawImage(state.albumArt,artPad,artPad,width-artPad*2,height-artPad*2);
                g.ResetClip();
                delete state.albumArt;
            } else {
                SolidBrush pb{Color(40,128,128,128)};
                g.FillPath(&pb,&artPath);
            }
        }
        if(state.hasProgress&&state.progressRatio>0.0){
            float cr=8.0f;
            float o=penW/2.0f;
            float s=(float)side-penW;
            if(cr>s/2.0f)cr=s/2.0f;
            constexpr float PI=3.14159265f;

            GraphicsPath bgPath;
            bgPath.AddArc(o,       o,       2*cr,2*cr, 180,90);
            bgPath.AddArc(o+s-2*cr,o,       2*cr,2*cr, 270,90);
            bgPath.AddArc(o+s-2*cr,o+s-2*cr,2*cr,2*cr,   0,90);
            bgPath.AddArc(o,       o+s-2*cr,2*cr,2*cr,  90,90);
            bgPath.CloseFigure();
            Pen bgPen(Color(45,mainColor.GetRed(),mainColor.GetGreen(),mainColor.GetBlue()),penW);
            g.DrawPath(&bgPen,&bgPath);

            float arcL=PI*cr/2.0f;
            float seg=s-2*cr;
            float perim=4*seg+4*arcL;
            float fill=(float)(perim*state.progressRatio),rem=fill;

            Pen fgPen(Color(230,mainColor.GetRed(),mainColor.GetGreen(),mainColor.GetBlue()),penW);
            Pen glowPen(Color(70,mainColor.GetRed(),mainColor.GetGreen(),mainColor.GetBlue()),penW+5);

            auto drawSeg=[&](PointF a,PointF b){
                if(rem<=0)return;
                float segLen=sqrtf((b.X-a.X)*(b.X-a.X)+(b.Y-a.Y)*(b.Y-a.Y));
                if(rem>=segLen){g.DrawLine(&glowPen,a,b);g.DrawLine(&fgPen,a,b);rem-=segLen;}
                else{float t=rem/segLen;PointF end(a.X+(b.X-a.X)*t,a.Y+(b.Y-a.Y)*t);
                     g.DrawLine(&glowPen,a,end);g.DrawLine(&fgPen,a,end);
                     SolidBrush db{Color(255,mainColor.GetRed(),mainColor.GetGreen(),mainColor.GetBlue())};
                     float dr=penW;g.FillEllipse(&db,end.X-dr,end.Y-dr,dr*2,dr*2);rem=0;}
            };
            auto drawArc=[&](float cx,float cy,float startDeg,float sweepDeg){
                const int N=10;
                float prev_x=cx+cr*cosf(startDeg*PI/180.0f);
                float prev_y=cy+cr*sinf(startDeg*PI/180.0f);
                for(int i=1;i<=N&&rem>0;i++){
                    float ang=(startDeg+sweepDeg*i/N)*PI/180.0f;
                    float nx=cx+cr*cosf(ang),ny=cy+cr*sinf(ang);
                    drawSeg({prev_x,prev_y},{nx,ny});
                    prev_x=nx;prev_y=ny;
                }
            };
            drawSeg({o+cr,o},{o+s-cr,o});
            drawArc(o+s-cr,o+cr,   270,90);
            drawSeg({o+s,o+cr},{o+s,o+s-cr});
            drawArc(o+s-cr,o+s-cr,   0,90);
            drawSeg({o+s-cr,o+s},{o+cr,o+s});
            drawArc(o+cr,   o+s-cr,  90,90);
            drawSeg({o,o+s-cr},{o,o+cr});
            drawArc(o+cr,   o+cr,   180,90);
        }
        return;
    }

    int artSize = g_Settings.showArtwork ? height-12 : 0;
    int artX=6, artY=6;
    if (g_Settings.showArtwork) {
        GraphicsPath artPath;
        if (g_Settings.artworkRounded)
            AddRoundedRect(artPath, artX, artY, artSize, artSize, 6);
        else
            artPath.AddRectangle(RectF((REAL)artX,(REAL)artY,(REAL)artSize,(REAL)artSize));
        if(state.albumArt){
            g.SetClip(&artPath);
            g.DrawImage(state.albumArt,artX,artY,artSize,artSize);
            g.ResetClip();
            delete state.albumArt;
        } else {
            SolidBrush pb{Color(40,128,128,128)};
            g.FillPath(&pb,&artPath);
        }
    } else {
        if(state.albumArt){ delete state.albumArt; state.albumArt=nullptr; }
    }

    float sc=(float)g_Settings.buttonScale;
    int contentStart = g_Settings.showArtwork ? artX+artSize+(int)(8*sc) : 6;
    int cy=height/2;
    float circR=12.0f*sc, iconW=8.0f*sc, iconH=12.0f*sc, gap=28.0f*sc;
    SolidBrush iconBrush{mainColor},hoverBrush{Color(255,mainColor.GetRed(),mainColor.GetGreen(),mainColor.GetBlue())},activeBg{Color(40,mainColor.GetRed(),mainColor.GetGreen(),mainColor.GetBlue())};

    float nX = (float)contentStart;
    if (g_Settings.showButtons) {
        float pX=(float)(contentStart+(int)(gap/2));
        if(g_HoverState==1)g.FillEllipse(&activeBg,pX-circR,(float)cy-circR,circR*2,circR*2);
        { float hw=(iconW+2*sc)/2;
          PointF pp[3]={{pX-hw+iconW,(float)cy-iconH/2},{pX-hw+iconW,(float)cy+iconH/2},{pX-hw,(float)cy}};
          g.FillPolygon(g_HoverState==1?&hoverBrush:&iconBrush,pp,3);
          g.FillRectangle(g_HoverState==1?&hoverBrush:&iconBrush,pX+hw-2*sc,(float)cy-iconH/2,2.0f*sc,iconH); }

        float plX=pX+gap;
        if(g_HoverState==2)g.FillEllipse(&activeBg,plX-circR,(float)cy-circR,circR*2,circR*2);
        if(state.isPlaying){
            float bw=3.0f*sc,bh=14.0f*sc,gap2=2.0f*sc;
            float startBar=plX-(2*bw+gap2)/2;
            g.FillRectangle(g_HoverState==2?&hoverBrush:&iconBrush,startBar,(float)cy-bh/2,bw,bh);
            g.FillRectangle(g_HoverState==2?&hoverBrush:&iconBrush,startBar+bw+gap2,(float)cy-bh/2,bw,bh);
        } else {
            float pw=10.0f*sc,ph=16.0f*sc;
            PointF pp2[3]={{plX-pw/2,(float)cy-ph/2},{plX-pw/2,(float)cy+ph/2},{plX+pw/2,(float)cy}};
            g.FillPolygon(g_HoverState==2?&hoverBrush:&iconBrush,pp2,3);
        }

        nX=plX+gap;
        if(g_HoverState==3)g.FillEllipse(&activeBg,nX-circR,(float)cy-circR,circR*2,circR*2);
        { float hw=(iconW+2*sc)/2;
          PointF np[3]={{nX-hw,(float)cy-iconH/2},{nX-hw,(float)cy+iconH/2},{nX-hw+iconW,(float)cy}};
          g.FillPolygon(g_HoverState==3?&hoverBrush:&iconBrush,np,3);
          g.FillRectangle(g_HoverState==3?&hoverBrush:&iconBrush,nX+hw-2*sc,(float)cy-iconH/2,2.0f*sc,iconH); }
        nX += gap/2;
    }

    int textX=(int)nX,textMaxW=width-textX-10;
    wstring fullText=state.title;if(!state.artist.empty())fullText+=L" \u2022 "+state.artist;
    FontFamily ff(FONT_NAME,nullptr);Font font(&ff,(REAL)g_Settings.fontSize,FontStyleBold,UnitPixel);SolidBrush textBrush{mainColor};
    RectF layout(0,0,2000,100),bound;g.MeasureString(fullText.c_str(),-1,&font,layout,&bound);g_TextWidth=(int)bound.Width;
    Region clip(Rect(textX,0,textMaxW,height));g.SetClip(&clip);float textY=(height-bound.Height)/2.0f;
    if(g_TextWidth>textMaxW){g_IsScrolling=true;float drawX=(float)(textX-g_ScrollOffset);g.DrawString(fullText.c_str(),-1,&font,PointF(drawX,textY),&textBrush);if(drawX+g_TextWidth<width)g.DrawString(fullText.c_str(),-1,&font,PointF(drawX+g_TextWidth+40,textY),&textBrush);}
    else{g_IsScrolling=false;g_ScrollOffset=0;g.DrawString(fullText.c_str(),-1,&font,PointF((float)textX,textY),&textBrush);}

    if(g_Settings.showProgressBar && state.hasProgress){
        g.ResetClip();
        const int barH=4;
        const int barY = g_Settings.progressBarOnTop ? 0 : height-barH;
        const int glowDir = g_Settings.progressBarOnTop ? 1 : -1;
        const int fillW=(int)(width*state.progressRatio);
        SolidBrush bgBar{Color(40,mainColor.GetRed(),mainColor.GetGreen(),mainColor.GetBlue())};g.FillRectangle(&bgBar,0,barY,width,barH);
        if(fillW>0){
            SolidBrush g3{Color(25,mainColor.GetRed(),mainColor.GetGreen(),mainColor.GetBlue())},g2{Color(55,mainColor.GetRed(),mainColor.GetGreen(),mainColor.GetBlue())},g1{Color(90,mainColor.GetRed(),mainColor.GetGreen(),mainColor.GetBlue())};
            g.FillRectangle(&g3,0,barY+glowDir*(-3),fillW,barH+6);g.FillRectangle(&g2,0,barY+glowDir*(-2),fillW,barH+4);g.FillRectangle(&g1,0,barY+glowDir*(-1),fillW,barH+2);
            SolidBrush fgBar{Color(220,mainColor.GetRed(),mainColor.GetGreen(),mainColor.GetBlue())};g.FillRectangle(&fgBar,0,barY,fillW,barH);
            const int dotR=5,dotX=fillW-dotR,dotY=barY+barH/2-dotR;
            SolidBrush dotGlow{Color(60,mainColor.GetRed(),mainColor.GetGreen(),mainColor.GetBlue())};g.FillEllipse(&dotGlow,dotX-3,dotY-3,(dotR+3)*2,(dotR+3)*2);
            SolidBrush dotBrush{Color(255,mainColor.GetRed(),mainColor.GetGreen(),mainColor.GetBlue())};g.FillEllipse(&dotBrush,dotX,dotY,dotR*2,dotR*2);
        }
    }
}

#define IDT_POLL_MEDIA 201
#define IDT_ANIMATION  202
#define IDT_TASKBAR    203
#define IDT_LYRICS     204
#define APP_WM_CLOSE   WM_APP

LRESULT CALLBACK MediaWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
        { IPropertyStore* ps=nullptr;if(SUCCEEDED(SHGetPropertyStoreForWindow(hwnd,__uuidof(IPropertyStore),(void**)&ps))){PROPVARIANT pv={};pv.vt=VT_LPWSTR;pv.pwszVal=(LPWSTR)L"WindhawkMusicLounge.Widget";ps->SetValue(MY_PKEY_AppUserModel_ID,pv);ps->Commit();ps->Release();} }
        UpdateAppearance(hwnd);
        if (g_MiniMode) { int nw=g_Settings.height; SetWindowPos(hwnd,HWND_TOPMOST,0,0,nw,GetWidgetDrawHeight(),SWP_NOMOVE|SWP_NOACTIVATE); }
        SetTimer(hwnd, IDT_POLL_MEDIA, 1000, NULL);
        SetTimer(hwnd, IDT_TASKBAR, 16, NULL);
        SetTimer(hwnd, IDT_LYRICS, 50, NULL);
        g_hForegroundHook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, NULL, FullscreenEventProc, 0, 0, WINEVENT_OUTOFCONTEXT|WINEVENT_SKIPOWNPROCESS);
        g_hMoveSizeHook   = SetWinEventHook(EVENT_SYSTEM_MOVESIZEEND, EVENT_SYSTEM_MOVESIZEEND, NULL, FullscreenEventProc, 0, 0, WINEVENT_OUTOFCONTEXT|WINEVENT_SKIPOWNPROCESS);
        g_hTaskbarMoveHook = SetWinEventHook(EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE, NULL, TaskbarMoveProc, 0, 0, WINEVENT_OUTOFCONTEXT|WINEVENT_SKIPOWNPROCESS);
        if (g_LyricsVisible) SetTimer(hwnd, 212, 500, NULL);
        return 0;
    case WM_ERASEBKGND: return 1;
    case WM_CLOSE: return 0;
    case APP_WM_CLOSE: DestroyWindow(hwnd); return 0;
    case WM_DESTROY:
        if(g_hForegroundHook){UnhookWinEvent(g_hForegroundHook);g_hForegroundHook=NULL;}
        if(g_hMoveSizeHook){UnhookWinEvent(g_hMoveSizeHook);g_hMoveSizeHook=NULL;}
        if(g_hTaskbarMoveHook){UnhookWinEvent(g_hTaskbarMoveHook);g_hTaskbarMoveHook=NULL;}
        HideLyricsWindow();UnsubscribeGSMTCEvents();g_SessionManager=nullptr;PostQuitMessage(0);return 0;
    case WM_SETTINGCHANGE: UpdateAppearance(hwnd);InvalidateRect(hwnd,NULL,TRUE);return 0;
    case WM_TIMER:
        if(wParam==IDT_POLL_MEDIA){UpdateMediaInfo();InvalidateRect(hwnd,NULL,FALSE);if(g_hLyricsWindow&&IsWindowVisible(g_hLyricsWindow))InvalidateRect(g_hLyricsWindow,NULL,FALSE);}
        else if(wParam==IDT_LYRICS){
            if(!g_hLyricsWindow) return 0;
            if(g_SessionManager){
                try{
                    wstring targetId;{lock_guard<mutex> guard(g_MediaState.lock);targetId=g_MediaState.sourceAppId;}
                    auto sessions=g_SessionManager.GetSessions();
                    GlobalSystemMediaTransportControlsSession lyrSession=nullptr;
                    for(auto const& s:sessions){if(wstring(s.SourceAppUserModelId().c_str())==targetId){lyrSession=s;break;}}
                    if(!lyrSession)lyrSession=g_SessionManager.GetCurrentSession();
                    if(lyrSession){
                        auto tl=lyrSession.GetTimelineProperties();auto info=lyrSession.GetPlaybackInfo();
                        using S=GlobalSystemMediaTransportControlsSessionPlaybackStatus;
                        int posMs=(int)(chrono::duration_cast<chrono::duration<double>>(tl.Position()).count()*1000);
                        bool playing=(info.PlaybackStatus()==S::Playing);
                        DWORD now2=GetTickCount();

                        if(posMs != g_LyricsLastPosMs) {
                            DWORD el=(g_LyricsPosUpdateTick>0)?(now2-g_LyricsPosUpdateTick):0;
                            if(el>6000)el=6000;
                            int expected=g_LyricsLastPosMs+(g_LyricsIsPlaying?(int)el:0);
                            int drift=posMs-expected;
                            static DWORD s_lastPollLog=0;
                            if(abs(drift)>6000){
                                Wh_Log(L"[LyricPoll] SEEK drift=%d",drift);
                                g_LyricsLastIdx=-1;
                                s_lastPollLog=now2;
                            } else if(now2-s_lastPollLog>=5000){
                                Wh_Log(L"[LyricPoll] pos=%d drift=%d",posMs,drift);
                                s_lastPollLog=now2;
                            }
                            g_LyricsLastPosMs=posMs;
                            g_LyricsPosUpdateTick=now2;
                        }
                        g_LyricsIsPlaying=playing;
                        {lock_guard<mutex> guard(g_MediaState.lock);g_MediaState.positionMs=posMs;}
                    }
                }catch(...){}
                InvalidateRect(g_hLyricsWindow,NULL,FALSE);
            }
        }
        else if(wParam==IDT_TASKBAR){
            CheckAndApplyFullscreen();

            // Auto-hide fade animation
            if (g_Settings.autoHideNoMedia && !g_UserHidden) {
                bool hasMedia; { lock_guard<mutex> guard(g_MediaState.lock); hasMedia = g_MediaState.hasMedia; }
                if (!hasMedia && g_NoMediaSinceTick > 0) {
                    DWORD elapsed = GetTickCount() - g_NoMediaSinceTick;
                    if (elapsed >= (DWORD)g_Settings.autoHideDelayMs)
                        g_AutoHideTarget = 0;
                } else if (hasMedia) {
                    g_AutoHideTarget = 255;
                }
            } else {
                g_AutoHideTarget = 255;
            }

            if (g_AutoHideAlpha != g_AutoHideTarget) {
                if (g_AutoHideAlpha < g_AutoHideTarget)
                    g_AutoHideAlpha = min(255, g_AutoHideAlpha + 5);
                else
                    g_AutoHideAlpha = max(0, g_AutoHideAlpha - 5);
                ApplyWindowAlpha();
            }

            bool lyrTaskbarHidden = false;
            { HWND hTb = FindWindow(TEXT("Shell_TrayWnd"), NULL);
              if (hTb) {
                  RECT trc2; GetWindowRect(hTb, &trc2);
                  HMONITOR hMon = MonitorFromWindow(hTb, MONITOR_DEFAULTTOPRIMARY);
                  MONITORINFO mi = { sizeof(mi) }; GetMonitorInfo(hMon, &mi);
                  APPBARDATA abd3 = { sizeof(abd3) };
                  bool ah = (SHAppBarMessage(ABM_GETSTATE, &abd3) & ABS_AUTOHIDE) != 0;
                  
                  bool win11SlidDown = (trc2.top > mi.rcMonitor.bottom - 40);
                  
                  lyrTaskbarHidden = ah && win11SlidDown;
              }
            }

            // Lyrics window Spotify + fade control
            if(g_LyricsVisible && g_hLyricsWindow) {
                wstring srcId; bool isPlaying;
                {lock_guard<mutex> guard(g_MediaState.lock); srcId=g_MediaState.sourceAppId; isPlaying=g_MediaState.isPlaying;}
                bool isSpotify = srcId.find(L"Spotify") != wstring::npos;

                if(isSpotify && isPlaying) g_SpotifyLastPlayingTick = GetTickCount();

                if(!isSpotify) {
                    g_LyricsTargetAlpha = 0;
                } else {
                    DWORD silentMs = (g_SpotifyLastPlayingTick > 0) ? (GetTickCount() - g_SpotifyLastPlayingTick) : 0;
                    bool pausedTooLong = (silentMs >= 3700);

                    bool notFoundFade = false;
                    { lock_guard<mutex> lg(g_LyricsMutex);
                      if (g_LyricsNotFound && g_LyricsNotFoundTick > 0)
                          notFoundFade = (GetTickCount() - g_LyricsNotFoundTick >= 5000); }

                    g_LyricsTargetAlpha = (pausedTooLong || notFoundFade) ? 0 : 255;
                }

                bool wantLyricsHidden = g_Settings.hideLyricsWithTaskbar && lyrTaskbarHidden;

                // Update slide target only when state changes
                static bool s_lyrWasHidden = false;
                if (wantLyricsHidden && !s_lyrWasHidden) {
                    int slideDir = g_Settings.widgetOnRight ? 1 : -1;
                    g_LyricsSlideTarget = slideDir * (g_Settings.width + 20);
                    s_lyrWasHidden = true;
                    Wh_Log(L"[LyricsSlide] Hiding → target=%d", g_LyricsSlideTarget);
                } else if (!wantLyricsHidden && s_lyrWasHidden) {
                    g_LyricsSlideTarget = 0;
                    s_lyrWasHidden = false;
                    Wh_Log(L"[LyricsSlide] Showing → target=0");
                }

                // Advance slide animation using smooth framerate-independent float LERP
                if (g_hLyricsWindow && g_LyricsSlideX != g_LyricsSlideTarget) {
                    static DWORD s_lastTickLx = 0;
                    DWORD nowLx = GetTickCount();
                    float dtLx = (s_lastTickLx == 0) ? 0.016f : (nowLx - s_lastTickLx) / 1000.0f;
                    s_lastTickLx = nowLx;
                    if (dtLx > 0.05f) dtLx = 0.016f;

                    static float fX = 0;
                    if (abs(fX - g_LyricsSlideX) > 20.0f) fX = (float)g_LyricsSlideX;
                    
                    fX += (g_LyricsSlideTarget - fX) * (1.0f - std::exp(-15.0f * dtLx));
                    if (abs(fX - g_LyricsSlideTarget) < 1.0f) fX = (float)g_LyricsSlideTarget;
                    
                    g_LyricsSlideX = (int)fX;
                    UpdateLyricsWindowPos();
                }

                bool slideFinishedHidden = wantLyricsHidden && (g_LyricsSlideX == g_LyricsSlideTarget) && g_LyricsSlideTarget != 0;
                bool shouldBeVisible = !g_IsHidden && g_AutoHideAlpha > 0
                    && !slideFinishedHidden
                    && (g_LyricsSlideX != g_LyricsSlideTarget || g_LyricsFadeAlpha > 0 || g_LyricsTargetAlpha > 0);

                bool curVisible = IsWindowVisible(g_hLyricsWindow) != FALSE;
                if(shouldBeVisible && !curVisible) ShowWindow(g_hLyricsWindow, SW_SHOWNOACTIVATE);
                if(!shouldBeVisible && curVisible) ShowWindow(g_hLyricsWindow, SW_HIDE);
            }

            {
                HWND hTaskbar=FindWindow(TEXT("Shell_TrayWnd"),NULL);
                if(hTaskbar){
                    RECT rc;GetWindowRect(hTaskbar,&rc);
                    int curW=g_MiniMode?g_Settings.height:g_Settings.width;
                    APPBARDATA abd2={sizeof(abd2)};
                    bool ahOn=(SHAppBarMessage(ABM_GETSTATE,&abd2)&ABS_AUTOHIDE)!=0;

                    int targetX = 0, targetY = 0;
                    RECT myRc; GetWindowRect(hwnd, &myRc);
                    
                    HMONITOR hMon = MonitorFromWindow(hTaskbar, MONITOR_DEFAULTTOPRIMARY);
                    MONITORINFO mi = {sizeof(mi)}; GetMonitorInfo(hMon, &mi);

                    if (g_IsHidden || lyrTaskbarHidden) {
                        targetY = mi.rcMonitor.bottom + 10;
                        targetX = g_Settings.widgetOnRight ? (mi.rcMonitor.right - g_Settings.offsetX - curW) : (mi.rcMonitor.left + g_Settings.offsetX);
                    } else if (!g_IsHidden && g_AutoHideAlpha > 0) {
                        int normalTop = rc.top;
                        int tbActualH = abs(rc.bottom - rc.top);
                        if (tbActualH < 20 || tbActualH > 200) tbActualH = 48;  // fallback for invalid rects
                        if (normalTop > mi.rcMonitor.bottom - 30) normalTop = mi.rcMonitor.bottom - tbActualH; // Do not track the jumping hidden rect
                        int normalBottom = normalTop + tbActualH;

                        if (g_Settings.displayNative) {
                            targetY = normalTop;
                            targetX = g_Settings.widgetOnRight ? (mi.rcMonitor.right - curW) : mi.rcMonitor.left;
                        } else {
                            targetY = normalTop + (normalBottom - normalTop)/2 - g_Settings.height/2 + g_Settings.offsetY + 2;
                            targetX = g_Settings.widgetOnRight ? (mi.rcMonitor.right - g_Settings.offsetX - curW) : (mi.rcMonitor.left + g_Settings.offsetX);
                        }
                    } else {
                        targetY = myRc.top;
                        targetX = myRc.left;
                    }

                    static DWORD s_lastTickMod = 0;
                    DWORD nowMod = GetTickCount();
                    float dtMod = (s_lastTickMod == 0) ? 0.016f : (nowMod - s_lastTickMod) / 1000.0f;
                    s_lastTickMod = nowMod;
                    if (dtMod > 0.05f) dtMod = 0.016f;

                    static float fY = -1;
                    static float fX = -1;
                    if (fY == -1 || abs(fY - targetY) > 800) fY = (float)myRc.top;
                    if (fX == -1 || abs(fX - targetX) > 800) fX = (float)myRc.left;
                    
                    fY += (targetY - fY) * (1.0f - std::exp(-15.0f * dtMod));
                    fX += (targetX - fX) * (1.0f - std::exp(-15.0f * dtMod));
                    
                    if (abs(fY - targetY) < 1.0f) fY = (float)targetY;
                    if (abs(fX - targetX) < 1.0f) fX = (float)targetX;

                    int newY = (int)fY;
                    int newX = (int)fX;

                    // In Native mode, lock height to the actual taskbar height so the bar fills it perfectly
                    int drawH = g_Settings.height;
                    if (g_Settings.displayNative && !g_IsHidden) {
                        int tbH = abs(rc.bottom - rc.top);
                        if (tbH > 20 && tbH < 200) drawH = tbH;  // sanity-check: only use real taskbar heights
                    }

                    if (myRc.left != newX || myRc.top != newY || myRc.bottom - myRc.top != drawH) {
                        SetWindowPos(hwnd, HWND_TOPMOST, newX, newY, curW, drawH, SWP_NOACTIVATE);
                        if (g_hLyricsWindow && g_LyricsSlideX == 0) UpdateLyricsWindowPos();
                    }
                    if (g_hLyricsWindow && myRc.top != newY && g_LyricsSlideX != 0) {
                        UpdateLyricsWindowPos();
                    }
                }
            }
        }
        else if(wParam==IDT_ANIMATION){
            if(g_IsScrolling){if(g_ScrollWait>0){g_ScrollWait--;}else{g_ScrollOffset++;if(g_ScrollOffset>g_TextWidth+40){g_ScrollOffset=0;g_ScrollWait=60;}InvalidateRect(hwnd,NULL,FALSE);}}
            else KillTimer(hwnd,IDT_ANIMATION);
        }
        else if(wParam==206){
            KillTimer(hwnd,206);
            if(g_MiniMode) SendMediaCommand(2);
        }
        else if(wParam==212){
            KillTimer(hwnd,212);
            if(g_LyricsVisible) ShowLyricsWindow();
        }
        return 0;
    case WM_MOUSEMOVE:{
        int x=LOWORD(lParam),y=HIWORD(lParam);
        float sc=(float)g_Settings.buttonScale;
        int artSize = g_Settings.showArtwork ? g_Settings.height-12 : 0;
        int contentStart = g_Settings.showArtwork ? 6+artSize+(int)(8*sc) : 6;
        float pX=(float)(contentStart+(int)(28*sc/2)),plX=pX+28*sc,nX=plX+28*sc,circR=12*sc;
        int newState=0;
        if(g_Settings.showButtons && y>10&&y<g_Settings.height-10){
            if(x>=pX-circR&&x<=pX+circR)newState=1;
            else if(x>=plX-circR&&x<=plX+circR)newState=2;
            else if(x>=nX-circR&&x<=nX+circR)newState=3;
        }
        if(newState!=g_HoverState){g_HoverState=newState;InvalidateRect(hwnd,NULL,FALSE);}
        TRACKMOUSEEVENT tme={sizeof(tme),TME_LEAVE,hwnd,0};TrackMouseEvent(&tme);return 0;
    }
    case WM_MOUSELEAVE:g_HoverState=0;g_IsHoveringArt=false;HideArtPopup();InvalidateRect(hwnd,NULL,FALSE);break;
    case WM_LBUTTONDOWN:
        if(g_MiniMode) SetTimer(hwnd, 206, GetDoubleClickTime(), NULL);
        return 0;
    case WM_LBUTTONUP:
        if(!g_MiniMode && g_HoverState>0) SendMediaCommand(g_HoverState);
        return 0;
    case WM_LBUTTONDBLCLK:
        if(g_MiniMode){ KillTimer(hwnd,206); BringSourceAppToFront(); }
        else if(g_HoverState==0) BringSourceAppToFront();
        return 0;
    case WM_RBUTTONUP:ShowCustomContextMenu(hwnd);return 0;
    case WM_MOUSEWHEEL:{short zDelta=GET_WHEEL_DELTA_WPARAM(wParam);BYTE vk=zDelta>0?VK_VOLUME_UP:VK_VOLUME_DOWN;keybd_event(vk,0,0,0);keybd_event(vk,0,KEYEVENTF_KEYUP,0);return 0;}
    case WM_PAINT:{
        PAINTSTRUCT ps;HDC hdc=BeginPaint(hwnd,&ps);RECT rc;GetClientRect(hwnd,&rc);
        HDC memDC=CreateCompatibleDC(hdc);HBITMAP memBmp=CreateCompatibleBitmap(hdc,rc.right,rc.bottom);HBITMAP oldBmp=(HBITMAP)SelectObject(memDC,memBmp);
        DrawMediaPanel(memDC,rc.right,rc.bottom);if(g_IsScrolling)SetTimer(hwnd,IDT_ANIMATION,16,NULL);
        BitBlt(hdc,0,0,rc.right,rc.bottom,memDC,0,0,SRCCOPY);SelectObject(memDC,oldBmp);DeleteObject(memBmp);DeleteDC(memDC);
        EndPaint(hwnd,&ps);return 0;
    }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void MediaThread() {
    winrt::init_apartment();
    GdiplusStartupInput gsi;ULONG_PTR token;GdiplusStartup(&token,&gsi,NULL);
    WNDCLASS wc={0};wc.style=CS_DBLCLKS;wc.lpfnWndProc=MediaWndProc;wc.hInstance=GetModuleHandle(NULL);wc.lpszClassName=TEXT("WindhawkMusicLounge_GSMTC");wc.hCursor=LoadCursor(NULL,IDC_HAND);RegisterClass(&wc);
    HMODULE hUser32=GetModuleHandle(L"user32.dll");
    pCreateWindowInBand CreateWindowInBand=hUser32?(pCreateWindowInBand)GetProcAddress(hUser32,"CreateWindowInBand"):nullptr;
    if(CreateWindowInBand)g_hMediaWindow=CreateWindowInBand(WS_EX_LAYERED|WS_EX_TOOLWINDOW|WS_EX_TOPMOST,wc.lpszClassName,TEXT("MusicLounge"),WS_POPUP|WS_VISIBLE,0,0,g_Settings.width,g_Settings.height,NULL,NULL,wc.hInstance,NULL,ZBID_IMMERSIVE_NOTIFICATION);
    if(!g_hMediaWindow)g_hMediaWindow=CreateWindowEx(WS_EX_LAYERED|WS_EX_TOOLWINDOW|WS_EX_TOPMOST,wc.lpszClassName,TEXT("MusicLounge"),WS_POPUP|WS_VISIBLE,0,0,g_Settings.width,g_Settings.height,NULL,NULL,wc.hInstance,NULL);
    Wh_Log(L"[Init] Window: %s", g_hMediaWindow ? L"OK" : L"FAILED");
    ApplyWindowAlpha();
    MSG msg;while(GetMessage(&msg,NULL,0,0)){TranslateMessage(&msg);DispatchMessage(&msg);}
    UnregisterClass(wc.lpszClassName,wc.hInstance);GdiplusShutdown(token);winrt::uninit_apartment();
}

std::thread* g_pMediaThread = nullptr;

BOOL WhTool_ModInit(){
    Wh_Log(L"[Mod] Init — v1.0");
    LoadSettings();
    LoadUIState();
    if (g_Settings.lyricsOffsetMs == 0) g_Settings.lyricsOffsetMs = 500;
    g_Running=true;
    g_pMediaThread=new std::thread(MediaThread);
    return TRUE;
}

void WhTool_ModUninit(){
    Wh_Log(L"[Mod] Uninit");
    g_Running=false;
    if(g_hMediaWindow)SendMessage(g_hMediaWindow,APP_WM_CLOSE,0,0);
    if(g_pMediaThread){if(g_pMediaThread->joinable())g_pMediaThread->join();delete g_pMediaThread;g_pMediaThread=nullptr;}
}


void WhTool_ModSettingsChanged(){
    LoadSettings();
    if(g_hMediaWindow){
        UpdateAppearance(g_hMediaWindow);
        if(!g_Settings.showButtons) g_HoverState = 0;
        int curW = g_MiniMode ? g_Settings.height : g_Settings.width;
        SetWindowPos(g_hMediaWindow, HWND_TOPMOST, 0, 0, curW, GetWidgetDrawHeight(), SWP_NOMOVE|SWP_NOACTIVATE);
        InvalidateRect(g_hMediaWindow, NULL, TRUE);
        SendMessage(g_hMediaWindow, WM_TIMER, IDT_POLL_MEDIA, 0);
    }
}

////////////////////////////////////////////////////////////////////////////////
bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook(){Wh_Log(L">");ExitThread(0);}

BOOL Wh_ModInit(){
    bool isService=false,isToolModProcess=false,isCurrentToolModProcess=false;
    int argc;LPWSTR* argv=CommandLineToArgvW(GetCommandLine(),&argc);
    if(!argv){Wh_Log(L"CommandLineToArgvW failed");return FALSE;}
    for(int i=1;i<argc;i++){if(wcscmp(argv[i],L"-service")==0){isService=true;break;}}
    for(int i=1;i<argc-1;i++){if(wcscmp(argv[i],L"-tool-mod")==0){isToolModProcess=true;if(wcscmp(argv[i+1],WH_MOD_ID)==0)isCurrentToolModProcess=true;break;}}
    LocalFree(argv);
    if(isService)return FALSE;
    if(isCurrentToolModProcess){
        g_toolModProcessMutex=CreateMutex(nullptr,TRUE,L"windhawk-tool-mod_" WH_MOD_ID);
        if(!g_toolModProcessMutex){Wh_Log(L"CreateMutex failed");ExitProcess(1);}
        if(GetLastError()==ERROR_ALREADY_EXISTS){Wh_Log(L"Tool mod already running (%s)",WH_MOD_ID);ExitProcess(1);}
        if(!WhTool_ModInit())ExitProcess(1);
        IMAGE_DOS_HEADER* dos=(IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* nt=(IMAGE_NT_HEADERS*)((BYTE*)dos+dos->e_lfanew);
        void* ep=(BYTE*)dos+nt->OptionalHeader.AddressOfEntryPoint;
        Wh_SetFunctionHook(ep,(void*)EntryPoint_Hook,nullptr);
        return TRUE;
    }
    if(isToolModProcess)return FALSE;
    g_isToolModProcessLauncher=true;return TRUE;
}

void Wh_ModAfterInit(){
    if(!g_isToolModProcessLauncher)return;
    WCHAR path[MAX_PATH];if(!GetModuleFileName(nullptr,path,ARRAYSIZE(path)))return;
    WCHAR cmd[MAX_PATH+2+(sizeof(L" -tool-mod \"" WH_MOD_ID "\"")/sizeof(WCHAR))-1];
    swprintf_s(cmd,L"\"%s\" -tool-mod \"%s\"",path,WH_MOD_ID);
    HMODULE hK=GetModuleHandle(L"kernelbase.dll");if(!hK)hK=GetModuleHandle(L"kernel32.dll");if(!hK)return;
    using CPIW=BOOL(WINAPI*)(HANDLE,LPCWSTR,LPWSTR,LPSECURITY_ATTRIBUTES,LPSECURITY_ATTRIBUTES,WINBOOL,DWORD,LPVOID,LPCWSTR,LPSTARTUPINFOW,LPPROCESS_INFORMATION,PHANDLE);
    auto pCPIW=(CPIW)GetProcAddress(hK,"CreateProcessInternalW");if(!pCPIW)return;
    STARTUPINFO si{.cb=sizeof(si),.dwFlags=STARTF_FORCEOFFFEEDBACK};PROCESS_INFORMATION pi;
    if(pCPIW(nullptr,path,cmd,nullptr,nullptr,FALSE,NORMAL_PRIORITY_CLASS,nullptr,nullptr,&si,&pi,nullptr)){CloseHandle(pi.hProcess);CloseHandle(pi.hThread);}
}

void Wh_ModSettingsChanged(){if(!g_isToolModProcessLauncher)WhTool_ModSettingsChanged();}
void Wh_ModUninit(){if(!g_isToolModProcessLauncher){WhTool_ModUninit();ExitProcess(0);}}
