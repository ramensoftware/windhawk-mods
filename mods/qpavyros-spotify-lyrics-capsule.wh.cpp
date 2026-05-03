// ==WindhawkMod==
// @id            qpavyros-spotify-lyrics-capsule
// @name          Spotify Lyrics Capsule
// @description     Fast Spotify-focused taskbar widget with synced lyrics, smart collapse, timeline ring, and runtime style controls.
// @version         1.0.0
// @author         qpavyros
// @github         https://github.com/qpavyros
// @include         explorer.exe
// @compilerOptions -lole32 -ldwmapi -lgdi32 -luser32 -lwindowsapp -lshcore -lgdiplus -lshell32 -lwinhttp
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Spotify Lyrics Capsule

Spotify Lyrics Capsule is a Spotify-focused taskbar media widget with synced lyrics,
timeline controls, smart collapse behavior, and runtime customization from the right-click menu.

## What This Mod Does
* Displays album art, playback controls, and track title/artist in a compact taskbar widget.
* Shows synchronized lyrics (LRC) when available.
* Fetches lyrics online from LRCLIB and caches them locally for faster repeat playback.
* Supports smart collapse animation (controls/text collapse toward disc).
* Draws a circular progress ring around album art.
* Supports runtime style switching (theme, shape, ring style, control style).

## Quick Start
* Install and enable the mod in Windhawk.
* Start Spotify playback.
* Right-click the widget to access runtime options.
* Enable `Enable Lyrics` in the right-click menu if lyrics are hidden.
* Optional: adjust lyrics timing from `Lyrics Calibration`.

## Interaction Guide
* Left click playback icons: `Previous`, `Play / Pause`, `Next`.
* Drag widget: available when `EnableDragMove` is enabled.
* Shift + Mouse Wheel: seek timeline by small steps.
* Right click widget: open full runtime menu.

## Right-Click Menu (Runtime Controls)
* `Previous`
* `Play / Pause`
* `Next`
* `Collapse To Disc On Mouse Leave`
* `Pin Always Visible`
* `Pause Animations`
* `Keep Controls Visible`
* `Enable Lyrics`
* `Lyrics Calibration`
* `Quick Theme` -> `Auto`, `Light`, `Dark`
* `Theme Preset` -> `Glass Clean`, `Minimal Mono`, `Neon Accent`, `Retro Vinyl`, `High Contrast`, `Legacy Classic`
* `Widget Shape` -> `Pill`, `Capsule`, `Compact`
* `Ring Style` -> `Single Ring`, `Double Ring`, `Dotted Ring`, `Segmented Ring`
* `Control Style` -> `Classic Controls`, `Outline Controls`, `Soft Controls`
* `Enable Fullscreen Hide`
* `Use Setting Value for Fullscreen Hide`
* `Reset All Runtime Toggles`
* `Reset Position`
* `Refresh`

## Lyrics Behavior
* Tries local cache first for instant lyrics on known tracks.
* If not cached, tries local `.lrc` files from `LyricsFolder`.
* If still missing, fetches online from LRCLIB.
* Uses a fast fetch strategy with retry/backoff and automatic mode probing.
* Caches fetched lyrics into a single local cache file (optional).
* Supports live timing calibration with `+1s`, `+2s`, `-1s`, `-2s`, `Reset`.

## Settings Reference
All settings below are available in Windhawk settings panel.

### Layout and Size
* `PanelWidth`: Widget width.
* `PanelHeight`: Widget height.
* `FontSize`: Text size.
* `ButtonScale`: Scale playback icons.
* `DiscScale`: Scale album art disc.
* `ControlGap`: Horizontal spacing between controls.
* `HitboxScale`: Click area scale for controls.
* `OffsetX`: X offset from anchor.
* `OffsetY`: Y offset from anchor.

### Motion and Animation
* `SpinSpeed`: Disc rotation speed.
* `FadeDurationMs`: Fade duration.
* `CollapseDelayMs`: Delay before collapse starts.
* `CollapseSpeed`: Collapse animation speed.
* `CollapseEasing`: Collapse easing function.
* `CollapseFadeStrength`: Text/control fade strength during collapse.
* `AnimationProfile`: Motion tuning profile (`Smooth`, `Snappy`, `Calm`).
* `TextOpacity`: Text alpha.

### Timeline and Seeking
* `ShowTimelineTime`: Show elapsed/total time label.
* `RingStyle`: Default ring rendering style.

### Lyrics
* `EnableLyrics`: Enable lyrics rendering.
* `LyricsFolder`: Folder for local `.lrc` files.
* `EnableOnlineLyrics`: Allow LRCLIB online fetch.
* `OnlineLyricsTimeoutMs`: Per-request timeout for online lyrics.
* `EnableLyricsDiskCache`: Save and reuse fetched lyrics.
* `LyricsCacheFile`: Path for single cache file.
* `PrefetchTopLyricsOnFirstEnable`: Prefetch popular tracks once.
* `PrefetchTopLyricsCount`: Prefetch target size (max 2000).

### Theme and Visual Style
* `ThemePreset`: Default preset (`Glass`, `Minimal`, `Neon`, `Retro`, `Contrast`, `Legacy`).
* `AutoTheme`: Auto follow light/dark mode.
* `AccentColor`: Accent/ring color.
* `BackgroundOpacity`: Widget background alpha.
* `BlurStrength`: Acrylic/blur strength.
* `CornerStyle`: Corner radius style.
* `WidgetShape`: Default widget shape.
* `ControlStyle`: Default playback control style.
* `TextColor`: Manual text color when needed.

### Behavior and Visibility
* `EnableDragMove`: Allow drag-to-move.
* `HideFullscreen`: Hide widget in fullscreen apps.

### Debugging
* `EnableDebugLogging`: Enable verbose logs.
* `DebugLogVerbosity`: `0=Basic`, `1=Verbose`, `2=Trace`.

## Requirements
* Windows 11 recommended for best native visual behavior.
* Spotify must be running with an active media session.
* For best taskbar compatibility, keep Windows Widgets disabled if layout conflicts appear.

## License
MIT
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- PanelWidth: 430
  $name: Panel Width
- PanelHeight: 48
  $name: Panel Height
- FontSize: 11
  $name: Font Size
- ButtonScale: 1.0
  $name: Button Scale (1.0 = Normal, 2.0 = 4K)
- DiscScale: 1.0
  $name: Disc Size Scale
- SpinSpeed: 2.5
  $name: Disc Spin Speed (Degrees/Frame)
- ControlGap: 28
  $name: Control Gap
- HitboxScale: 1.35
  $name: Button Hitbox Scale
- TextOpacity: 255
  $name: Text Opacity (0-255)
- FadeDurationMs: 180
  $name: Fade Duration (ms)
- CollapseDelayMs: 2000
  $name: Collapse Delay (ms)
- CollapseSpeed: 6.5
  $name: Collapse Speed
- CollapseEasing: 2
  $name: Collapse Easing (0=Linear, 1=EaseOut, 2=Smoothstep)
- CollapseFadeStrength: 1.0
  $name: Collapse Fade Strength (0.0-1.0)
- ShowTimelineTime: false
  $name: Show Timeline Time Label
- EnableLyrics: true
  $name: Enable Lyrics (LRC)
- LyricsFolder: ""
  $name: Lyrics Folder (optional - %USERPROFILE%\\Music\\Lyrics)
- EnableOnlineLyrics: true
  $name: Enable Online Lyrics (LRCLIB)
- OnlineLyricsTimeoutMs: 900
  $name: Online Lyrics Timeout (ms)
- EnableLyricsDiskCache: true
  $name: Save Lyrics On Device
- LyricsCacheFile: ""
  $name: Lyrics Cache File (single file, optional)
- PrefetchTopLyricsOnFirstEnable: true
  $name: Prefetch Top Lyrics On First Enable
- PrefetchTopLyricsCount: 2000
  $name: Prefetch Top Lyrics Count (max 2000)
- ThemePreset: 0
  $name: Theme Preset (0=Glass, 1=Minimal, 2=Neon, 3=Retro, 4=Contrast, 5=Legacy)
- AccentColor: 0x3EA6FF
  $name: Accent Color (Hex)
- BackgroundOpacity: 120
  $name: Background Opacity (0-255)
- BlurStrength: 40
  $name: Blur Strength (0-100)
- CornerStyle: 2
  $name: Corner Style (0=Sharp, 1=Rounded, 2=Pill, 3=Soft)
- RingStyle: 0
  $name: Timeline Ring Style (0=Single, 1=Double, 2=Dotted, 3=Segmented)
- ControlStyle: 0
  $name: Control Style (0=Classic, 1=Outline, 2=Soft)
- WidgetShape: 0
  $name: Widget Shape (0=Pill, 1=Capsule, 2=Compact)
- AnimationProfile: 0
  $name: Animation Profile (0=Smooth, 1=Snappy, 2=Calm)
- EnableDragMove: true
  $name: Allow Drag to Move
- HideFullscreen: false
  $name: Hide when Fullscreen
- OffsetX: 12
  $name: X Offset
- OffsetY: 0
  $name: Y Offset
- AutoTheme: true
  $name: Auto Theme
- TextColor: 0xFFFFFF
  $name: Manual Text Color (Hex)
- EnableDebugLogging: false
  $name: Enable Debug Logs
- DebugLogVerbosity: 1
  $name: Debug Log Verbosity (0=Basic, 1=Verbose, 2=Trace)
*/
// ==/WindhawkModSettings==

#include <winsock2.h>
#include <windows.h>
#include <windowsx.h>
#include <shobjidl.h> 
#include <shellapi.h>
#include <dwmapi.h>
#include <gdiplus.h>
#include <shcore.h> 
#include <winhttp.h>
#include <string>
#include <thread>
#include <mutex>
#include <memory>
#include <cstdio>
#include <cstdarg>
#include <algorithm>
#include <cwctype>
#include <cstdint>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <vector>
#include <unordered_map>
#include <unordered_set>

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
const COLORREF TRANSPARENT_COLORKEY = RGB(1, 0, 1);
const WCHAR* RUNTIME_COLLAPSE_TO_DISC_VALUE = L"RuntimeCollapseToDiscEnabled";
const WCHAR* RUNTIME_PIN_ALWAYS_VISIBLE_VALUE = L"RuntimePinAlwaysVisible";
const WCHAR* RUNTIME_PAUSE_ANIMATIONS_VALUE = L"RuntimePauseAnimations";
const WCHAR* RUNTIME_LYRICS_ENABLED_VALUE = L"RuntimeLyricsEnabled";
const WCHAR* RUNTIME_KEEP_CONTROLS_VISIBLE_VALUE = L"RuntimeKeepControlsVisible";
const WCHAR* RUNTIME_LYRICS_OFFSET_MS_VALUE = L"RuntimeLyricsOffsetMs";
const WCHAR* RUNTIME_LYRICS_HTTP_MODE_VALUE = L"RuntimeLyricsHttpAccessMode";
const WCHAR* RUNTIME_LYRICS_BOOTSTRAP_DONE_VALUE = L"RuntimeLyricsBootstrapDone";
const WCHAR* RUNTIME_THEME_MODE_VALUE = L"RuntimeThemeMode";
const WCHAR* RUNTIME_HIDE_FULLSCREEN_OVERRIDE_VALUE = L"RuntimeHideFullscreenOverride";
const WCHAR* RUNTIME_THEME_PRESET_VALUE = L"RuntimeThemePreset";
const WCHAR* RUNTIME_WIDGET_SHAPE_VALUE = L"RuntimeWidgetShape";
const WCHAR* RUNTIME_RING_STYLE_VALUE = L"RuntimeRingStyle";
const WCHAR* RUNTIME_CONTROL_STYLE_VALUE = L"RuntimeControlStyle";

constexpr UINT TIMER_ID_POLL_MEDIA = 1001;
constexpr UINT TIMER_ID_ANIMATION = 1002;
constexpr UINT TIMER_ID_FADE = 1003;
constexpr UINT MSG_APP_CLOSE = WM_APP;

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

#ifndef DWMWA_BORDER_COLOR
#define DWMWA_BORDER_COLOR 34
#endif

#ifndef DWMWA_COLOR_NONE
#define DWMWA_COLOR_NONE 0xFFFFFFFE
#endif

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

// --- Settings ---
struct ModSettings {
    int width = 300;
    int height = 48;
    int fontSize = 11;
    double buttonScale = 1.0; 
    double discScale = 1.0;
    double spinSpeed = 2.5;
    int controlGap = 28;
    double hitboxScale = 1.35;
    int textOpacity = 255;
    int fadeDurationMs = 180;
    int collapseDelayMs = 2000;
    double collapseSpeed = 6.5;
    int collapseEasing = 2;
    double collapseFadeStrength = 1.0;
    bool showTimelineTime = false;
    bool enableLyrics = true;
    wstring lyricsFolder;
    bool enableOnlineLyrics = true;
    int onlineLyricsTimeoutMs = 1800;
    bool enableLyricsDiskCache = true;
    wstring lyricsCacheFile;
    bool prefetchTopLyricsOnFirstEnable = true;
    int prefetchTopLyricsCount = 2000;
    int themePreset = 0;
    DWORD accentColor = 0xFF3EA6FF;
    int backgroundOpacity = 120;
    int blurStrength = 40;
    int cornerStyle = 2;
    int ringStyle = 0;
    int controlStyle = 0;
    int widgetShape = 0;
    int animationProfile = 0;
    bool enableDragMove = true;
    bool hideFullscreen = false;
    int offsetX = 12;
    int offsetY = 0;
    bool autoTheme = true;
    DWORD manualTextColor = 0xFFFFFFFF;
    bool enableDebugLogging = false;
    int debugLogVerbosity = 1;
} g_Settings;

// --- Runtime State ---
HWND g_hMediaWindow = NULL;
int g_HoverState = 0; 
HWINEVENTHOOK g_TaskbarHook = nullptr; 
UINT g_TaskbarCreatedMsg = RegisterWindowMessage(L"TaskbarCreated");
bool g_CollapseToDiscOnMouseLeave = false;
bool g_IsMouseInsideWidget = false;
bool g_RuntimePinAlwaysVisible = false;
bool g_RuntimePauseAnimations = false;
bool g_RuntimeLyricsEnabled = true;
bool g_RuntimeKeepControlsVisible = false;
int g_RuntimeLyricsOffsetMs = 0;
int g_RuntimeThemeMode = 0; // 0=Auto, 1=Light, 2=Dark
int g_LyricsHttpPreferredAccessMode = -1; // -1=unknown, 0=no proxy, 1=default proxy
mutex g_LyricsHttpAccessModeLock;
bool g_LyricsBootstrapInProgress = false;
mutex g_LyricsBootstrapLock;
mutex g_LyricsDiskCacheFileIoLock;

// Idle Tracking
bool g_IsHiddenByIdle = false;
int g_DefaultOffsetX = 12;
int g_DefaultOffsetY = 0;

// Dragging
bool g_IsDraggingWidget = false;
bool g_DragMoved = false;
POINT g_DragStartCursor = {0, 0};
RECT g_DragStartWindowRect = {0, 0, 0, 0};

// Fade animation
BYTE g_CurrentAlpha = 255;
BYTE g_TargetAlpha = 255;
bool g_FadePendingHide = false;
bool g_FadeTimerRunning = false;
ULONGLONG g_LastFadeTick = 0;
bool g_AnimationTimerRunning = false;

// Data Model
struct MediaState {
    wstring title = L"Waiting for Spotify...";
    wstring artist = L"";
    bool isPlaying = false;
    bool hasMedia = false;
    shared_ptr<Bitmap> albumArt;
    wstring albumArtCacheKey;
    int64_t timelinePosition100ns = 0;
    int64_t timelineDuration100ns = 0;
    bool canSeek = false;
    mutex lock;
 } g_MediaState;

struct LyricLine {
    int64_t timestamp100ns = 0;
    wstring text;
};

struct LyricsState {
    wstring trackKey;
    wstring sourcePath;
    vector<LyricLine> lines;
    ULONGLONG lastLoadAttemptTick = 0;
    ULONGLONG retryBlockedUntilTick = 0;
    int consecutiveOnlineFailures = 0;
    bool onlineFetchInProgress = false;
    mutex lock;
} g_LyricsState;

struct LyricsCacheEntry {
    wstring sourcePath;
    vector<LyricLine> lines;
};

struct LyricsDiskCacheState {
    unordered_map<wstring, LyricsCacheEntry> entries;
    bool loaded = false;
    bool dirty = false;
    mutex lock;
} g_LyricsDiskCache;

// Animation
float g_ScrollOffset = 0.0f;
int g_TextWidth = 0;
bool g_IsScrolling = false;
float g_ScrollPauseMs = 900.0f;
float g_DiscAngle = 0.0f;
float g_DiscVelocity = 0.0f;
float g_CollapseProgress = 0.0f;
float g_CollapseTarget = 0.0f;
ULONGLONG g_CollapseDelayUntilTick = 0;
bool g_CollapseDelayPending = false;
ULONGLONG g_LastAnimationTick = 0;
wstring g_LastAnimatedLyricLine;
wstring g_PreviousAnimatedLyricLine;
ULONGLONG g_LastLyricLineChangeTick = 0;

const float kScrollSpeedPxPerSec = 55.0f;
const float kScrollGapPx = 44.0f;
const float kScrollPauseMsDefault = 900.0f;
const float kDiscSmoothingPerSec = 8.0f;
const float kCollapseSpeedDefaultPerSec = 6.5f;
const float kLyricLineTransitionMs = 320.0f;
const float kLyricLineShiftPx = 12.0f;
const int kLyricsOffsetStepMs = 1000;
const int kLyricsOffsetMaxMs = 15000;
const int kLyricsHoldWindowMs = 350;
const int kLyricsRetryCooldownMs = 900;
const int kLyricsFetchStallMs = 25000;
const int kLyricsRetryBackoffBaseMs = 1000;
const int kLyricsRetryBackoffMaxMs = 10000;
const int kLyricsRetryBackoffJitterPct = 10;
const int kOnlineLyricsRequestTimeoutCapMs = 1100;
const int kLrclibFetchRoundDeadlineMs = 2400;
const int kLrclibFetchHardDeadlineMs = 6000;
const size_t kLyricsDiskCacheMaxEntries = 2000;
const int kLyricsPreferredWidth = 430;
const int kSeekStepSeconds = 5;
const int kThemePresetCount = 6;
const int kWidgetShapeCount = 3;
const int kRingStyleCount = 4;
const int kControlStyleCount = 3;
const int kAnimationProfileCount = 3;
const float kMaxFrameDeltaSec = 0.05f;
const float kFadeMinStep = 2.0f;
const char* kLyricsCacheFileHeader = "WH_LYRICS_CACHE_V1";
const WCHAR* kFixedDebugLogPath =
    L"%USERPROFILE%\\Documents\\Windhawk\\tml.log";

double ReadDoubleSetting(PCWSTR settingName, double defaultValue) {
    double value = defaultValue;
    PCWSTR settingStr = Wh_GetStringSetting(settingName);
    if (settingStr) {
        if (wcslen(settingStr) > 0) {
            value = _wtof(settingStr);
        }
        Wh_FreeStringSetting(settingStr);
    }
    return value;
}

DWORD ReadRgbSetting(PCWSTR settingName, DWORD defaultRgb) {
    DWORD value = defaultRgb;
    PCWSTR settingStr = Wh_GetStringSetting(settingName);
    if (settingStr) {
        if (wcslen(settingStr) > 0) {
            value = wcstoul(settingStr, nullptr, 16);
        }
        Wh_FreeStringSetting(settingStr);
    }
    return value & 0x00FFFFFF;
}

enum DebugLogLevel {
    LOG_BASIC = 0,
    LOG_VERBOSE = 1,
    LOG_TRACE = 2,
};

static string WideToUtf8(const wstring& value);
void EnsureDirectoryForFilePath(const wstring& path);

mutex g_DebugLogFileLock;

static wstring ResolveFixedDebugLogFilePath() {
    static wstring resolvedPath;
    static bool initialized = false;
    if (initialized) {
        return resolvedPath;
    }

    WCHAR expanded[32767] = {};
    DWORD expandedLen = ExpandEnvironmentStringsW(kFixedDebugLogPath, expanded,
                                                   ARRAYSIZE(expanded));
    if (expandedLen > 0 && expandedLen < ARRAYSIZE(expanded)) {
        resolvedPath = expanded;
    } else {
        resolvedPath = L"C:\\Users\\Public\\Documents\\Windhawk\\tml.log";
    }

    initialized = true;
    return resolvedPath;
}

static const wchar_t* DebugLogLevelTag(int level) {
    if (level <= LOG_BASIC) return L"BASIC";
    if (level == LOG_VERBOSE) return L"VERBOSE";
    return L"TRACE";
}

static wstring TruncateForLog(const wstring& value, size_t maxChars = 72) {
    if (value.size() <= maxChars) {
        return value;
    }

    if (maxChars < 4) {
        return value.substr(0, maxChars);
    }

    wstring out = value.substr(0, maxChars - 3);
    out += L"...";
    return out;
}

static const wchar_t* HttpAccessTypeName(DWORD accessType) {
    return (accessType == WINHTTP_ACCESS_TYPE_DEFAULT_PROXY) ? L"DEFAULT_PROXY"
                                                              : L"NO_PROXY";
}

static ULONGLONG ApplyLyricsBackoffJitterMs(ULONGLONG baseMs) {
    if (baseMs == 0) {
        return 0;
    }

    // Lightweight jitter (+/- configured percent) to avoid synchronized retries.
    ULONGLONG span = (ULONGLONG)(kLyricsRetryBackoffJitterPct * 2 + 1);
    long long jitterSeed =
        (long long)(GetTickCount64() % span) -
        (long long)kLyricsRetryBackoffJitterPct;
    long long adjusted =
        (long long)baseMs +
        (((long long)baseMs * jitterSeed) / (long long)100);
    if (adjusted < 250) {
        adjusted = 250;
    }
    return (ULONGLONG)adjusted;
}

static ULONGLONG ComputeLyricsRetryBackoffMs(int consecutiveFailures) {
    if (consecutiveFailures <= 0) {
        return 0;
    }

    // 1s, 2s, 4s, 8s, then cap at 10s.
    int shift = consecutiveFailures - 1;
    if (shift < 0) shift = 0;
    if (shift > 3) shift = 3;

    ULONGLONG backoffMs = (ULONGLONG)kLyricsRetryBackoffBaseMs << shift;
    if (consecutiveFailures >= 5) {
        backoffMs = (ULONGLONG)kLyricsRetryBackoffMaxMs;
    }
    if (backoffMs > (ULONGLONG)kLyricsRetryBackoffMaxMs) {
        backoffMs = (ULONGLONG)kLyricsRetryBackoffMaxMs;
    }
    return ApplyLyricsBackoffJitterMs(backoffMs);
}

static void AppendDebugLogToFixedFile(int level, const wchar_t* body) {
    if (!body) {
        return;
    }

    wstring logPath = ResolveFixedDebugLogFilePath();
    if (logPath.empty()) {
        return;
    }

    SYSTEMTIME st = {};
    GetLocalTime(&st);

    wchar_t line[1400];
    _snwprintf_s(line, _countof(line), _TRUNCATE,
                 L"[%04u-%02u-%02u %02u:%02u:%02u.%03u][%ls] %ls\r\n",
                 st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond,
                 st.wMilliseconds, DebugLogLevelTag(level), body);

    lock_guard<mutex> guard(g_DebugLogFileLock);
    EnsureDirectoryForFilePath(logPath);

    HANDLE hFile = CreateFileW(logPath.c_str(), FILE_APPEND_DATA,
                               FILE_SHARE_READ | FILE_SHARE_WRITE |
                                   FILE_SHARE_DELETE,
                               nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        return;
    }

    LARGE_INTEGER size = {};
    if (GetFileSizeEx(hFile, &size) && size.QuadPart == 0) {
        static const BYTE kUtf8Bom[3] = {0xEF, 0xBB, 0xBF};
        DWORD written = 0;
        WriteFile(hFile, kUtf8Bom, sizeof(kUtf8Bom), &written, nullptr);
    }

    string utf8 = WideToUtf8(line);
    if (!utf8.empty()) {
        DWORD written = 0;
        WriteFile(hFile, utf8.data(), (DWORD)utf8.size(), &written, nullptr);
    }
    CloseHandle(hFile);
}

static void DebugLog(int level, const wchar_t* format, ...) {
    if (!g_Settings.enableDebugLogging) {
        return;
    }
    if (level > g_Settings.debugLogVerbosity) {
        return;
    }

    wchar_t body[1024];
    va_list args;
    va_start(args, format);
    _vsnwprintf_s(body, _countof(body), _TRUNCATE, format, args);
    va_end(args);

    Wh_Log(L"[TML][%ls] %ls", DebugLogLevelTag(level), body);
    AppendDebugLogToFixedFile(level, body);
}

// --- Settings ---
void RefreshMediaStateAndVisibility(HWND hwnd);
void EnsureAnimationTimer(HWND hwnd);
void SyncCollapseAnimationState(HWND hwnd);
void ResetCollapseDelayState();
void ResetRuntimeToggles();
float ApplyCollapseEasing(float t);
float CalculateCollapseVisibleFactor(float collapseEase);
float GetAnimationSpeedMultiplier();
float GetScrollSpeedMultiplier();
float GetDiscSmoothingMultiplier();
wstring FormatTimelineTime(int64_t ticks100ns);
void SeekSpotifyBySeconds(int deltaSeconds);
void ResetLyricsState();
void ResetLyricsAnimationState();
void ResetLyricsDiskCacheState();
void StartOnlineLyricsFetchAsync(const wstring& trackKey, const wstring& title,
                                 const wstring& artist, int durationSeconds);
bool LoadLyricsForTrack(const wstring& title, const wstring& artist,
                        int durationSeconds);
bool TryGetCurrentLyricsLine(int64_t position100ns, wstring* outLine,
                             int64_t* outTimestamp100ns);
wstring GetCurrentLyricsLine(int64_t position100ns);
bool TryGetLyricsFromDiskCache(const wstring& trackKey, vector<LyricLine>* outLines,
                               wstring* outSourcePath);
void SaveLyricsToDiskCache(const wstring& trackKey, const wstring& sourcePath,
                           const vector<LyricLine>& lines);
void SaveLyricsBatchToDiskCache(
    const vector<pair<wstring, LyricsCacheEntry>>& batchItems);
void StartLyricsBootstrapIfNeeded();
int GetResolvedThemeMode();
struct ThemeTokens;
void BuildRoundedRectPath(GraphicsPath* path, const RectF& rect, float radius);
ThemeTokens BuildThemeTokens(int width, int height, BYTE textAlpha);
void UpdateAppearance(HWND hwnd);

void LoadSettings() {
    g_Settings.width = Wh_GetIntSetting(L"PanelWidth");
    g_Settings.height = Wh_GetIntSetting(L"PanelHeight");
    g_Settings.fontSize = Wh_GetIntSetting(L"FontSize");
    g_Settings.offsetX = Wh_GetIntSetting(L"OffsetX");
    g_Settings.offsetY = Wh_GetIntSetting(L"OffsetY");
    g_DefaultOffsetX = g_Settings.offsetX;
    g_DefaultOffsetY = g_Settings.offsetY;
    g_Settings.autoTheme = Wh_GetIntSetting(L"AutoTheme") != 0;

    g_Settings.buttonScale = ReadDoubleSetting(L"ButtonScale", 1.0);
    g_Settings.discScale = ReadDoubleSetting(L"DiscScale", 1.0);
    g_Settings.spinSpeed = ReadDoubleSetting(L"SpinSpeed", 2.5);
    g_Settings.hitboxScale = ReadDoubleSetting(L"HitboxScale", 1.35);
    g_Settings.controlGap = Wh_GetIntSetting(L"ControlGap");
    g_Settings.textOpacity = Wh_GetIntSetting(L"TextOpacity");
    g_Settings.fadeDurationMs = Wh_GetIntSetting(L"FadeDurationMs");
    g_Settings.collapseDelayMs = Wh_GetIntSetting(L"CollapseDelayMs");
    g_Settings.collapseSpeed = ReadDoubleSetting(L"CollapseSpeed", kCollapseSpeedDefaultPerSec);
    g_Settings.collapseEasing = Wh_GetIntSetting(L"CollapseEasing");
    g_Settings.collapseFadeStrength = ReadDoubleSetting(L"CollapseFadeStrength", 1.0);
    g_Settings.showTimelineTime = Wh_GetIntSetting(L"ShowTimelineTime") != 0;
    g_Settings.enableLyrics = Wh_GetIntSetting(L"EnableLyrics") != 0;
    g_Settings.lyricsFolder.clear();
    if (PCWSTR lyricsFolder = Wh_GetStringSetting(L"LyricsFolder")) {
        g_Settings.lyricsFolder = lyricsFolder;
        Wh_FreeStringSetting(lyricsFolder);
    }
    g_Settings.enableOnlineLyrics =
        Wh_GetIntSetting(L"EnableOnlineLyrics") != 0;
    g_Settings.onlineLyricsTimeoutMs =
        Wh_GetIntSetting(L"OnlineLyricsTimeoutMs");
    g_Settings.enableLyricsDiskCache =
        Wh_GetIntSetting(L"EnableLyricsDiskCache") != 0;
    g_Settings.lyricsCacheFile.clear();
    if (PCWSTR lyricsCacheFile = Wh_GetStringSetting(L"LyricsCacheFile")) {
        g_Settings.lyricsCacheFile = lyricsCacheFile;
        Wh_FreeStringSetting(lyricsCacheFile);
    }
    g_Settings.prefetchTopLyricsOnFirstEnable =
        Wh_GetIntSetting(L"PrefetchTopLyricsOnFirstEnable") != 0;
    g_Settings.prefetchTopLyricsCount =
        Wh_GetIntSetting(L"PrefetchTopLyricsCount");
    g_Settings.themePreset = Wh_GetIntSetting(L"ThemePreset");
    g_Settings.accentColor = 0xFF000000 | ReadRgbSetting(L"AccentColor", 0x3EA6FF);
    g_Settings.backgroundOpacity = Wh_GetIntSetting(L"BackgroundOpacity");
    g_Settings.blurStrength = Wh_GetIntSetting(L"BlurStrength");
    g_Settings.cornerStyle = Wh_GetIntSetting(L"CornerStyle");
    g_Settings.ringStyle = Wh_GetIntSetting(L"RingStyle");
    g_Settings.controlStyle = Wh_GetIntSetting(L"ControlStyle");
    g_Settings.widgetShape = Wh_GetIntSetting(L"WidgetShape");
    g_Settings.animationProfile = Wh_GetIntSetting(L"AnimationProfile");
    g_Settings.enableDragMove = Wh_GetIntSetting(L"EnableDragMove") != 0;
    g_Settings.enableDebugLogging =
        Wh_GetIntSetting(L"EnableDebugLogging") != 0;
    g_Settings.debugLogVerbosity = Wh_GetIntSetting(L"DebugLogVerbosity");

    if (g_Settings.buttonScale < 0.5) g_Settings.buttonScale = 0.5;
    if (g_Settings.buttonScale > 4.0) g_Settings.buttonScale = 4.0;
    if (g_Settings.discScale < 0.7) g_Settings.discScale = 0.7;
    if (g_Settings.discScale > 1.8) g_Settings.discScale = 1.8;
    if (g_Settings.spinSpeed < 0.0) g_Settings.spinSpeed = 0.0;
    if (g_Settings.spinSpeed > 12.0) g_Settings.spinSpeed = 12.0;
    if (g_Settings.controlGap < 16) g_Settings.controlGap = 16;
    if (g_Settings.controlGap > 72) g_Settings.controlGap = 72;
    if (g_Settings.hitboxScale < 1.0) g_Settings.hitboxScale = 1.0;
    if (g_Settings.hitboxScale > 2.5) g_Settings.hitboxScale = 2.5;
    if (g_Settings.textOpacity < 0) g_Settings.textOpacity = 0;
    if (g_Settings.textOpacity > 255) g_Settings.textOpacity = 255;
    if (g_Settings.fadeDurationMs < 0) g_Settings.fadeDurationMs = 0;
    if (g_Settings.fadeDurationMs > 1200) g_Settings.fadeDurationMs = 1200;
    if (g_Settings.collapseDelayMs < 0) g_Settings.collapseDelayMs = 0;
    if (g_Settings.collapseDelayMs > 10000) g_Settings.collapseDelayMs = 10000;
    if (g_Settings.collapseSpeed < 0.2) g_Settings.collapseSpeed = 0.2;
    if (g_Settings.collapseSpeed > 25.0) g_Settings.collapseSpeed = 25.0;
    if (g_Settings.collapseEasing < 0) g_Settings.collapseEasing = 0;
    if (g_Settings.collapseEasing > 2) g_Settings.collapseEasing = 2;
    if (g_Settings.collapseFadeStrength < 0.0) g_Settings.collapseFadeStrength = 0.0;
    if (g_Settings.collapseFadeStrength > 1.0) g_Settings.collapseFadeStrength = 1.0;
    if (g_Settings.onlineLyricsTimeoutMs < 650) g_Settings.onlineLyricsTimeoutMs = 650;
    if (g_Settings.onlineLyricsTimeoutMs > 1100) g_Settings.onlineLyricsTimeoutMs = 1100;
    if (g_Settings.prefetchTopLyricsCount < 0) g_Settings.prefetchTopLyricsCount = 0;
    if (g_Settings.prefetchTopLyricsCount > 2000) g_Settings.prefetchTopLyricsCount = 2000;
    if (g_Settings.themePreset < 0) g_Settings.themePreset = 0;
    if (g_Settings.themePreset >= kThemePresetCount) g_Settings.themePreset = kThemePresetCount - 1;
    if (g_Settings.backgroundOpacity < 0) g_Settings.backgroundOpacity = 0;
    if (g_Settings.backgroundOpacity > 255) g_Settings.backgroundOpacity = 255;
    if (g_Settings.blurStrength < 0) g_Settings.blurStrength = 0;
    if (g_Settings.blurStrength > 100) g_Settings.blurStrength = 100;
    if (g_Settings.cornerStyle < 0) g_Settings.cornerStyle = 0;
    if (g_Settings.cornerStyle > 3) g_Settings.cornerStyle = 3;
    if (g_Settings.ringStyle < 0) g_Settings.ringStyle = 0;
    if (g_Settings.ringStyle >= kRingStyleCount) g_Settings.ringStyle = kRingStyleCount - 1;
    if (g_Settings.controlStyle < 0) g_Settings.controlStyle = 0;
    if (g_Settings.controlStyle >= kControlStyleCount) g_Settings.controlStyle = kControlStyleCount - 1;
    if (g_Settings.widgetShape < 0) g_Settings.widgetShape = 0;
    if (g_Settings.widgetShape >= kWidgetShapeCount) g_Settings.widgetShape = kWidgetShapeCount - 1;
    if (g_Settings.animationProfile < 0) g_Settings.animationProfile = 0;
    if (g_Settings.animationProfile >= kAnimationProfileCount) g_Settings.animationProfile = kAnimationProfileCount - 1;
    if (g_Settings.debugLogVerbosity < 0) g_Settings.debugLogVerbosity = 0;
    if (g_Settings.debugLogVerbosity > 2) g_Settings.debugLogVerbosity = 2;

    g_Settings.hideFullscreen = Wh_GetIntSetting(L"HideFullscreen") != 0;
    int hideFullscreenOverride = Wh_GetIntValue(RUNTIME_HIDE_FULLSCREEN_OVERRIDE_VALUE, -1);
    if (hideFullscreenOverride == 0 || hideFullscreenOverride == 1) {
        g_Settings.hideFullscreen = hideFullscreenOverride != 0;
    }
    g_CollapseToDiscOnMouseLeave =
        Wh_GetIntValue(RUNTIME_COLLAPSE_TO_DISC_VALUE, 0) != 0;
    g_RuntimePinAlwaysVisible =
        Wh_GetIntValue(RUNTIME_PIN_ALWAYS_VISIBLE_VALUE, 0) != 0;
    g_RuntimePauseAnimations =
        Wh_GetIntValue(RUNTIME_PAUSE_ANIMATIONS_VALUE, 0) != 0;
    int runtimeLyricsEnabled = Wh_GetIntValue(RUNTIME_LYRICS_ENABLED_VALUE, -1);
    if (runtimeLyricsEnabled == 0 || runtimeLyricsEnabled == 1) {
        g_RuntimeLyricsEnabled = runtimeLyricsEnabled != 0;
    } else {
        g_RuntimeLyricsEnabled = g_Settings.enableLyrics;
    }
    g_RuntimeKeepControlsVisible =
        Wh_GetIntValue(RUNTIME_KEEP_CONTROLS_VISIBLE_VALUE, 0) != 0;
    g_RuntimeLyricsOffsetMs = Wh_GetIntValue(RUNTIME_LYRICS_OFFSET_MS_VALUE, 0);
    if (g_RuntimeLyricsOffsetMs < -kLyricsOffsetMaxMs) {
        g_RuntimeLyricsOffsetMs = -kLyricsOffsetMaxMs;
    }
    if (g_RuntimeLyricsOffsetMs > kLyricsOffsetMaxMs) {
        g_RuntimeLyricsOffsetMs = kLyricsOffsetMaxMs;
    }
    g_RuntimeThemeMode = Wh_GetIntValue(RUNTIME_THEME_MODE_VALUE, 0);
    if (g_RuntimeThemeMode < 0 || g_RuntimeThemeMode > 2) {
        g_RuntimeThemeMode = 0;
    }
    {
        int runtimeLyricsHttpMode =
            Wh_GetIntValue(RUNTIME_LYRICS_HTTP_MODE_VALUE, -1);
        lock_guard<mutex> guard(g_LyricsHttpAccessModeLock);
        if (runtimeLyricsHttpMode == 0 || runtimeLyricsHttpMode == 1) {
            g_LyricsHttpPreferredAccessMode = runtimeLyricsHttpMode;
        } else {
            g_LyricsHttpPreferredAccessMode = -1;
        }
    }

    int runtimeThemePreset = Wh_GetIntValue(RUNTIME_THEME_PRESET_VALUE, -1);
    if (runtimeThemePreset >= 0 && runtimeThemePreset < kThemePresetCount) {
        g_Settings.themePreset = runtimeThemePreset;
    }

    int runtimeWidgetShape = Wh_GetIntValue(RUNTIME_WIDGET_SHAPE_VALUE, -1);
    if (runtimeWidgetShape >= 0 && runtimeWidgetShape < kWidgetShapeCount) {
        g_Settings.widgetShape = runtimeWidgetShape;
    }

    int runtimeRingStyle = Wh_GetIntValue(RUNTIME_RING_STYLE_VALUE, -1);
    if (runtimeRingStyle >= 0 && runtimeRingStyle < kRingStyleCount) {
        g_Settings.ringStyle = runtimeRingStyle;
    }

    int runtimeControlStyle = Wh_GetIntValue(RUNTIME_CONTROL_STYLE_VALUE, -1);
    if (runtimeControlStyle >= 0 && runtimeControlStyle < kControlStyleCount) {
        g_Settings.controlStyle = runtimeControlStyle;
    }

    if (!g_CollapseToDiscOnMouseLeave) {
        g_CollapseProgress = 0.0f;
        g_CollapseTarget = 0.0f;
        ResetCollapseDelayState();
    }

    PCWSTR textHex = Wh_GetStringSetting(L"TextColor");
    DWORD textRGB = 0xFFFFFF;
    if (textHex) {
        if (wcslen(textHex) > 0) textRGB = wcstoul(textHex, nullptr, 16);
        Wh_FreeStringSetting(textHex);
    }
    g_Settings.manualTextColor = 0xFF000000 | textRGB;
    

    if (g_Settings.width < 100) g_Settings.width = kLyricsPreferredWidth;
    if (g_RuntimeLyricsEnabled && g_Settings.width < kLyricsPreferredWidth) {
        g_Settings.width = kLyricsPreferredWidth;
    }
    if (g_Settings.height < 24) g_Settings.height = 48;

    // A persisted drag position has priority over static settings.
    if (Wh_GetIntValue(L"HasSavedPosition", 0) != 0) {
        g_Settings.offsetX = Wh_GetIntValue(L"SavedOffsetX", g_Settings.offsetX);
        g_Settings.offsetY = Wh_GetIntValue(L"SavedOffsetY", g_Settings.offsetY);
    }

    DebugLog(LOG_BASIC,
             L"Settings loaded: lyrics=%d online=%d diskCache=%d prefetch=%d timeout=%dms log=%d verbosity=%d",
             g_RuntimeLyricsEnabled ? 1 : 0, g_Settings.enableOnlineLyrics ? 1 : 0,
             g_Settings.enableLyricsDiskCache ? 1 : 0,
             g_Settings.prefetchTopLyricsOnFirstEnable ? 1 : 0,
             g_Settings.onlineLyricsTimeoutMs,
             g_Settings.enableDebugLogging ? 1 : 0,
             g_Settings.debugLogVerbosity);
}

// --- WinRT / GSMTC ---
GlobalSystemMediaTransportControlsSessionManager g_SessionManager = nullptr;
GlobalSystemMediaTransportControlsSession g_SpotifySessionForEvents = nullptr;
winrt::event_token g_PlaybackChangedToken{};
winrt::event_token g_MediaPropertiesChangedToken{};
winrt::event_token g_SessionsChangedToken{};
winrt::event_token g_CurrentSessionChangedToken{};
bool g_ManagerEventsSubscribed = false;

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

wstring ToLowerCopy(wstring value) {
    transform(value.begin(), value.end(), value.begin(), [](wchar_t c) {
        return (wchar_t)towlower(c);
    });
    return value;
}

wstring TrimCopy(const wstring& value) {
    size_t start = 0;
    while (start < value.size() && iswspace(value[start])) {
        start++;
    }

    size_t end = value.size();
    while (end > start && iswspace(value[end - 1])) {
        end--;
    }

    return value.substr(start, end - start);
}

wstring CollapseWhitespace(const wstring& value) {
    wstring out;
    out.reserve(value.size());
    bool lastWasSpace = false;
    for (wchar_t ch : value) {
        if (iswspace(ch)) {
            if (!lastWasSpace) {
                out.push_back(L' ');
                lastWasSpace = true;
            }
            continue;
        }

        lastWasSpace = false;
        out.push_back(ch);
    }
    return TrimCopy(out);
}

wstring NormalizeLyricsLookupPart(const wstring& value) {
    wstring normalized;
    normalized.reserve(value.size());

    for (wchar_t ch : value) {
        // Drop common invisible separators that can make keys unstable.
        if (ch == 0x200B || ch == 0x200C || ch == 0x200D || ch == 0xFEFF) {
            continue;
        }
        normalized.push_back(ch);
    }

    return CollapseWhitespace(TrimCopy(normalized));
}

wstring BuildLyricsTrackKey(const wstring& title, const wstring& artist) {
    wstring normalizedTitle = ToLowerCopy(NormalizeLyricsLookupPart(title));
    wstring normalizedArtist = ToLowerCopy(NormalizeLyricsLookupPart(artist));
    return normalizedTitle + L"|" + normalizedArtist;
}

wstring SimplifyTrackTitleForLyricsLookup(const wstring& title) {
    wstring simplified = NormalizeLyricsLookupPart(title);
    if (simplified.empty()) {
        return simplified;
    }

    wstring lower = ToLowerCopy(simplified);
    vector<wstring> cutMarkers = {
        L" (feat", L" [feat", L" feat.", L" ft.", L" - ", L" | ",
        L" (from ", L" (remaster", L" - remaster", L" - live", L" - acoustic"
    };

    size_t cutPos = wstring::npos;
    for (const auto& marker : cutMarkers) {
        size_t pos = lower.find(marker);
        if (pos == wstring::npos || pos == 0) {
            continue;
        }
        if (cutPos == wstring::npos || pos < cutPos) {
            cutPos = pos;
        }
    }

    if (cutPos != wstring::npos) {
        simplified = simplified.substr(0, cutPos);
    }

    return CollapseWhitespace(TrimCopy(simplified));
}

wstring SanitizePathPart(const wstring& value) {
    wstring out;
    out.reserve(value.size());

    for (wchar_t ch : value) {
        if (ch < 32 || wcschr(L"<>:\"/\\|?*", ch) != nullptr) {
            out.push_back(L'_');
        } else {
            out.push_back(ch);
        }
    }

    out = TrimCopy(out);
    while (!out.empty() && (out.back() == L'.' || out.back() == L' ')) {
        out.pop_back();
    }

    return out;
}

wstring ResolveLyricsFolderPath() {
    wstring folder = TrimCopy(g_Settings.lyricsFolder);

    if (folder.empty()) {
        WCHAR userProfile[512] = {};
        DWORD len = GetEnvironmentVariableW(L"USERPROFILE", userProfile,
                                            ARRAYSIZE(userProfile));
        if (len > 0 && len < ARRAYSIZE(userProfile)) {
            folder = wstring(userProfile) + L"\\Music\\Lyrics";
        } else {
            folder = L".\\Lyrics";
        }
    }

    WCHAR expanded[32767] = {};
    DWORD expandedLen = ExpandEnvironmentStringsW(folder.c_str(), expanded,
                                                   ARRAYSIZE(expanded));
    if (expandedLen > 0 && expandedLen < ARRAYSIZE(expanded)) {
        return expanded;
    }

    return folder;
}

wstring ResolveLyricsCacheFilePath() {
    wstring cachePath = TrimCopy(g_Settings.lyricsCacheFile);
    if (cachePath.empty()) {
        wstring folder = ResolveLyricsFolderPath();
        if (!folder.empty() && folder.back() != L'\\' && folder.back() != L'/') {
            folder += L'\\';
        }
        cachePath = folder + L"_lyrics_cache_all.whlrcdb";
    }

    WCHAR expanded[32767] = {};
    DWORD expandedLen = ExpandEnvironmentStringsW(cachePath.c_str(), expanded,
                                                   ARRAYSIZE(expanded));
    if (expandedLen > 0 && expandedLen < ARRAYSIZE(expanded)) {
        return expanded;
    }

    return cachePath;
}

void EnsureDirectoryForFilePath(const wstring& path) {
    size_t split = path.find_last_of(L"\\/");
    if (split == wstring::npos) {
        return;
    }

    wstring dir = path.substr(0, split);
    if (dir.empty()) {
        return;
    }

    replace(dir.begin(), dir.end(), L'/', L'\\');

    size_t start = 1;
    if (dir.size() >= 3 && dir[1] == L':' && dir[2] == L'\\') {
        start = 3;
    }

    for (size_t i = start; i <= dir.size(); i++) {
        if (i != dir.size() && dir[i] != L'\\') {
            continue;
        }

        wstring partial = dir.substr(0, i);
        if (partial.empty()) {
            continue;
        }
        if (partial.size() == 2 && partial[1] == L':') {
            continue;
        }

        CreateDirectoryW(partial.c_str(), nullptr);
    }
}

bool ReadFileBytesRaw(const wstring& path, string* outBytes) {
    if (!outBytes) return false;
    outBytes->clear();

    HANDLE hFile = CreateFileW(path.c_str(), GENERIC_READ,
                               FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                               nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        return false;
    }

    LARGE_INTEGER size = {};
    if (!GetFileSizeEx(hFile, &size) || size.QuadPart < 0 ||
        size.QuadPart > 32 * 1024 * 1024) {
        CloseHandle(hFile);
        return false;
    }

    if (size.QuadPart == 0) {
        CloseHandle(hFile);
        return true;
    }

    outBytes->assign((size_t)size.QuadPart, '\0');
    DWORD bytesRead = 0;
    bool ok = ReadFile(hFile, outBytes->data(), (DWORD)outBytes->size(), &bytesRead,
                       nullptr) &&
              bytesRead == outBytes->size();
    CloseHandle(hFile);
    if (!ok) {
        outBytes->clear();
        return false;
    }

    return true;
}

bool WriteFileBytesRaw(const wstring& path, const string& bytes) {
    EnsureDirectoryForFilePath(path);

    HANDLE hFile = CreateFileW(path.c_str(), GENERIC_WRITE,
                               FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                               nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        return false;
    }

    DWORD bytesWritten = 0;
    bool ok = true;
    if (!bytes.empty()) {
        ok = WriteFile(hFile, bytes.data(), (DWORD)bytes.size(), &bytesWritten,
                       nullptr) &&
             bytesWritten == bytes.size();
    }
    CloseHandle(hFile);
    return ok;
}

bool ReadLyricsTextFile(const wstring& path, wstring* outContent) {
    if (!outContent) return false;

    HANDLE hFile = CreateFileW(path.c_str(), GENERIC_READ,
                               FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                               nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        return false;
    }

    LARGE_INTEGER size = {};
    if (!GetFileSizeEx(hFile, &size) || size.QuadPart <= 0 ||
        size.QuadPart > 16 * 1024 * 1024) {
        CloseHandle(hFile);
        return false;
    }

    vector<char> bytes((size_t)size.QuadPart);
    DWORD bytesRead = 0;
    bool ok = ReadFile(hFile, bytes.data(), (DWORD)bytes.size(), &bytesRead, nullptr) &&
              bytesRead == bytes.size();
    CloseHandle(hFile);
    if (!ok) {
        return false;
    }

    if (bytes.size() >= 2 &&
        (BYTE)bytes[0] == 0xFF && (BYTE)bytes[1] == 0xFE) {
        size_t wcharCount = (bytes.size() - 2) / sizeof(wchar_t);
        const wchar_t* utf16 = reinterpret_cast<const wchar_t*>(bytes.data() + 2);
        outContent->assign(utf16, utf16 + wcharCount);
        return true;
    }

    if (bytes.size() >= 2 &&
        (BYTE)bytes[0] == 0xFE && (BYTE)bytes[1] == 0xFF) {
        size_t wcharCount = (bytes.size() - 2) / 2;
        outContent->clear();
        outContent->reserve(wcharCount);
        for (size_t i = 0; i < wcharCount; i++) {
            unsigned char hi = (unsigned char)bytes[2 + (i * 2)];
            unsigned char lo = (unsigned char)bytes[2 + (i * 2) + 1];
            wchar_t wc = (wchar_t)((hi << 8) | lo);
            outContent->push_back(wc);
        }
        return true;
    }

    int offset = 0;
    if (bytes.size() >= 3 &&
        (BYTE)bytes[0] == 0xEF && (BYTE)bytes[1] == 0xBB && (BYTE)bytes[2] == 0xBF) {
        offset = 3;
    }

    const char* raw = bytes.data() + offset;
    int rawLen = (int)bytes.size() - offset;

    int wideLen = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, raw, rawLen,
                                      nullptr, 0);
    UINT codePage = CP_UTF8;
    DWORD flags = MB_ERR_INVALID_CHARS;
    if (wideLen <= 0) {
        codePage = CP_ACP;
        flags = 0;
        wideLen = MultiByteToWideChar(codePage, flags, raw, rawLen, nullptr, 0);
    }

    if (wideLen <= 0) {
        return false;
    }

    outContent->assign((size_t)wideLen, L'\0');
    MultiByteToWideChar(codePage, flags, raw, rawLen, outContent->data(), wideLen);
    return true;
}

bool ParseLrcTimestampTag(const wstring& tag, int64_t* outTimestamp100ns) {
    if (!outTimestamp100ns) return false;

    int minutes = 0;
    double seconds = 0.0;
    if (swscanf(tag.c_str(), L"%d:%lf", &minutes, &seconds) != 2) {
        return false;
    }

    if (minutes < 0 || seconds < 0.0) {
        return false;
    }

    double totalSeconds = (double)minutes * 60.0 + seconds;
    *outTimestamp100ns = (int64_t)(totalSeconds * 10000000.0 + 0.5);
    return true;
}

bool ParseLrcContent(const wstring& content, vector<LyricLine>* outLines) {
    if (!outLines) return false;
    outLines->clear();

    size_t cursor = 0;
    while (cursor <= content.size()) {
        size_t lineEnd = content.find(L'\n', cursor);
        if (lineEnd == wstring::npos) lineEnd = content.size();

        wstring line = content.substr(cursor, lineEnd - cursor);
        if (!line.empty() && line.back() == L'\r') {
            line.pop_back();
        }

        cursor = (lineEnd < content.size()) ? (lineEnd + 1) : (content.size() + 1);

        vector<int64_t> timestamps;
        size_t scan = 0;
        while (scan < line.size() && line[scan] == L'[') {
            size_t close = line.find(L']', scan + 1);
            if (close == wstring::npos) {
                break;
            }

            wstring tag = TrimCopy(line.substr(scan + 1, close - scan - 1));
            int64_t ts100ns = 0;
            if (ParseLrcTimestampTag(tag, &ts100ns)) {
                timestamps.push_back(ts100ns);
            }

            scan = close + 1;
        }

        if (timestamps.empty()) {
            continue;
        }

        wstring lyricText = TrimCopy(line.substr(scan));
        for (int64_t ts : timestamps) {
            outLines->push_back({ts, lyricText});
        }
    }

    sort(outLines->begin(), outLines->end(),
         [](const LyricLine& a, const LyricLine& b) {
             return a.timestamp100ns < b.timestamp100ns;
         });

    return !outLines->empty();
}

static string WideToUtf8(const wstring& value) {
    if (value.empty()) return string();

    int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, value.c_str(),
                                         (int)value.size(), nullptr, 0, nullptr,
                                         nullptr);
    if (sizeNeeded <= 0) return string();

    string out((size_t)sizeNeeded, '\0');
    WideCharToMultiByte(CP_UTF8, 0, value.c_str(), (int)value.size(), out.data(),
                        sizeNeeded, nullptr, nullptr);
    return out;
}

static wstring Utf8ToWide(const string& value) {
    if (value.empty()) return wstring();

    int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, value.data(),
                                         (int)value.size(), nullptr, 0);
    if (sizeNeeded <= 0) return wstring();

    wstring out((size_t)sizeNeeded, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, value.data(), (int)value.size(), out.data(),
                        sizeNeeded);
    return out;
}

static wstring UrlEncodeUtf8(const wstring& value) {
    string utf8 = WideToUtf8(value);
    wstring out;
    WCHAR hex[4] = {};

    for (unsigned char c : utf8) {
        bool unreserved = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
                          (c >= '0' && c <= '9') || c == '-' || c == '_' ||
                          c == '.' || c == '~';
        if (unreserved) {
            out.push_back((wchar_t)c);
        } else {
            swprintf_s(hex, L"%%%02X", (unsigned int)c);
            out += hex;
        }
    }

    return out;
}

static wstring SerializeLyricsToLrc(const vector<LyricLine>& lines) {
    wstring lrc;
    lrc.reserve(lines.size() * 28);

    for (const auto& line : lines) {
        int64_t ts = line.timestamp100ns;
        if (ts < 0) ts = 0;
        int64_t totalMs = (ts + 5000LL) / 10000LL;
        int64_t minutes = totalMs / 60000LL;
        int64_t seconds = (totalMs / 1000LL) % 60LL;
        int64_t centis = (totalMs % 1000LL) / 10LL;

        wchar_t tag[40] = {};
        swprintf_s(tag, L"[%02lld:%02lld.%02lld]", (long long)minutes,
                   (long long)seconds, (long long)centis);
        lrc += tag;
        lrc += line.text;
        lrc += L"\n";
    }

    return lrc;
}

static bool AreLyricLinesEqual(const vector<LyricLine>& a,
                               const vector<LyricLine>& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); i++) {
        if (a[i].timestamp100ns != b[i].timestamp100ns) return false;
        if (a[i].text != b[i].text) return false;
    }
    return true;
}

static string Base64Encode(const string& input) {
    static const char kTable[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    string out;
    out.reserve(((input.size() + 2) / 3) * 4);

    size_t i = 0;
    while (i + 3 <= input.size()) {
        unsigned int b0 = (unsigned char)input[i++];
        unsigned int b1 = (unsigned char)input[i++];
        unsigned int b2 = (unsigned char)input[i++];
        unsigned int n = (b0 << 16) | (b1 << 8) | b2;
        out.push_back(kTable[(n >> 18) & 0x3F]);
        out.push_back(kTable[(n >> 12) & 0x3F]);
        out.push_back(kTable[(n >> 6) & 0x3F]);
        out.push_back(kTable[n & 0x3F]);
    }

    size_t rem = input.size() - i;
    if (rem == 1) {
        unsigned int b0 = (unsigned char)input[i];
        unsigned int n = (b0 << 16);
        out.push_back(kTable[(n >> 18) & 0x3F]);
        out.push_back(kTable[(n >> 12) & 0x3F]);
        out.push_back('=');
        out.push_back('=');
    } else if (rem == 2) {
        unsigned int b0 = (unsigned char)input[i++];
        unsigned int b1 = (unsigned char)input[i];
        unsigned int n = (b0 << 16) | (b1 << 8);
        out.push_back(kTable[(n >> 18) & 0x3F]);
        out.push_back(kTable[(n >> 12) & 0x3F]);
        out.push_back(kTable[(n >> 6) & 0x3F]);
        out.push_back('=');
    }

    return out;
}

static int Base64CharToValue(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    if (c >= '0' && c <= '9') return c - '0' + 52;
    if (c == '+') return 62;
    if (c == '/') return 63;
    return -1;
}

static bool Base64Decode(const string& input, string* out) {
    if (!out) return false;
    out->clear();
    if (input.empty()) return true;
    if ((input.size() % 4) != 0) return false;

    out->reserve((input.size() / 4) * 3);
    for (size_t i = 0; i < input.size(); i += 4) {
        char c0 = input[i];
        char c1 = input[i + 1];
        char c2 = input[i + 2];
        char c3 = input[i + 3];

        int v0 = Base64CharToValue(c0);
        int v1 = Base64CharToValue(c1);
        if (v0 < 0 || v1 < 0) {
            out->clear();
            return false;
        }

        int v2 = (c2 == '=') ? -1 : Base64CharToValue(c2);
        int v3 = (c3 == '=') ? -1 : Base64CharToValue(c3);
        if ((v2 < 0 && c2 != '=') || (v3 < 0 && c3 != '=')) {
            out->clear();
            return false;
        }
        if (c2 == '=' && c3 != '=') {
            out->clear();
            return false;
        }

        unsigned int n = ((unsigned int)v0 << 18) | ((unsigned int)v1 << 12);
        if (v2 >= 0) n |= ((unsigned int)v2 << 6);
        if (v3 >= 0) n |= (unsigned int)v3;

        out->push_back((char)((n >> 16) & 0xFF));
        if (v2 >= 0) out->push_back((char)((n >> 8) & 0xFF));
        if (v3 >= 0) out->push_back((char)(n & 0xFF));
    }

    return true;
}

void ResetLyricsDiskCacheState() {
    lock_guard<mutex> guard(g_LyricsDiskCache.lock);
    g_LyricsDiskCache.entries.clear();
    g_LyricsDiskCache.loaded = false;
    g_LyricsDiskCache.dirty = false;
}

static void EnsureLyricsDiskCacheLoaded() {
    if (!g_Settings.enableLyricsDiskCache) {
        return;
    }

    {
        lock_guard<mutex> guard(g_LyricsDiskCache.lock);
        if (g_LyricsDiskCache.loaded) {
            return;
        }
        g_LyricsDiskCache.loaded = true;
    }

    unordered_map<wstring, LyricsCacheEntry> loadedEntries;
    string bytes;
    if (ReadFileBytesRaw(ResolveLyricsCacheFilePath(), &bytes) && !bytes.empty()) {
        if (bytes.size() >= 3 && (BYTE)bytes[0] == 0xEF && (BYTE)bytes[1] == 0xBB &&
            (BYTE)bytes[2] == 0xBF) {
            bytes.erase(0, 3);
        }

        size_t cursor = 0;
        bool headerChecked = false;
        while (cursor <= bytes.size()) {
            size_t lineEnd = bytes.find('\n', cursor);
            if (lineEnd == string::npos) lineEnd = bytes.size();
            string line = bytes.substr(cursor, lineEnd - cursor);
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            cursor = (lineEnd < bytes.size()) ? (lineEnd + 1) : (bytes.size() + 1);

            if (!headerChecked) {
                headerChecked = true;
                if (line != kLyricsCacheFileHeader) {
                    break;
                }
                continue;
            }

            if (line.empty()) continue;

            size_t tab1 = line.find('\t');
            if (tab1 == string::npos) continue;
            size_t tab2 = line.find('\t', tab1 + 1);
            if (tab2 == string::npos) continue;

            string keyB64 = line.substr(0, tab1);
            string sourceB64 = line.substr(tab1 + 1, tab2 - tab1 - 1);
            string lrcB64 = line.substr(tab2 + 1);

            string keyUtf8;
            string sourceUtf8;
            string lrcUtf8;
            if (!Base64Decode(keyB64, &keyUtf8) ||
                !Base64Decode(sourceB64, &sourceUtf8) ||
                !Base64Decode(lrcB64, &lrcUtf8)) {
                continue;
            }

            wstring trackKey = Utf8ToWide(keyUtf8);
            wstring sourcePath = Utf8ToWide(sourceUtf8);
            wstring lrcContent = Utf8ToWide(lrcUtf8);
            if (trackKey.empty() || lrcContent.empty()) {
                continue;
            }

            vector<LyricLine> parsed;
            if (!ParseLrcContent(lrcContent, &parsed) || parsed.empty()) {
                continue;
            }

            loadedEntries[trackKey] = {sourcePath, std::move(parsed)};
        }
    }

    if (loadedEntries.empty()) {
        return;
    }

    lock_guard<mutex> guard(g_LyricsDiskCache.lock);
    for (auto& entry : loadedEntries) {
        if (g_LyricsDiskCache.entries.find(entry.first) ==
            g_LyricsDiskCache.entries.end()) {
            g_LyricsDiskCache.entries.emplace(entry.first, std::move(entry.second));
        }
    }
}

static bool WriteLyricsDiskCacheSnapshot(
    const unordered_map<wstring, LyricsCacheEntry>& snapshot,
    const wstring& cachePath) {
    string output;
    output.reserve(snapshot.size() * 128);
    output += kLyricsCacheFileHeader;
    output += "\n";

    for (const auto& item : snapshot) {
        if (item.first.empty() || item.second.lines.empty()) {
            continue;
        }

        string keyB64 = Base64Encode(WideToUtf8(item.first));
        string sourceB64 = Base64Encode(WideToUtf8(item.second.sourcePath));
        wstring lrcWide = SerializeLyricsToLrc(item.second.lines);
        string lrcB64 = Base64Encode(WideToUtf8(lrcWide));

        output += keyB64;
        output += "\t";
        output += sourceB64;
        output += "\t";
        output += lrcB64;
        output += "\n";
    }

    return WriteFileBytesRaw(cachePath, output);
}

static bool BuildLyricsDiskCacheEncodedLine(const wstring& trackKey,
                                            const wstring& sourcePath,
                                            const vector<LyricLine>& lines,
                                            string* outLine) {
    if (!outLine) return false;
    outLine->clear();
    if (trackKey.empty() || lines.empty()) {
        return false;
    }

    string keyB64 = Base64Encode(WideToUtf8(trackKey));
    string sourceB64 = Base64Encode(WideToUtf8(sourcePath));
    string lrcB64 = Base64Encode(WideToUtf8(SerializeLyricsToLrc(lines)));
    if (keyB64.empty() || lrcB64.empty()) {
        return false;
    }

    outLine->reserve(keyB64.size() + sourceB64.size() + lrcB64.size() + 3);
    *outLine += keyB64;
    *outLine += "\t";
    *outLine += sourceB64;
    *outLine += "\t";
    *outLine += lrcB64;
    *outLine += "\n";
    return true;
}

static bool AppendLyricsDiskCacheLines(const wstring& cachePath,
                                       const vector<string>& encodedLines) {
    if (cachePath.empty() || encodedLines.empty()) {
        return false;
    }

    lock_guard<mutex> ioGuard(g_LyricsDiskCacheFileIoLock);
    EnsureDirectoryForFilePath(cachePath);

    HANDLE hFile = CreateFileW(cachePath.c_str(), FILE_APPEND_DATA,
                               FILE_SHARE_READ | FILE_SHARE_WRITE |
                                   FILE_SHARE_DELETE,
                               nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL,
                               nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        return false;
    }

    LARGE_INTEGER fileSize = {};
    if (!GetFileSizeEx(hFile, &fileSize)) {
        CloseHandle(hFile);
        return false;
    }

    auto writeAll = [&](const char* bytes, size_t length) -> bool {
        if (!bytes || length == 0) {
            return true;
        }
        if (length > MAXDWORD) {
            return false;
        }

        DWORD written = 0;
        return WriteFile(hFile, bytes, (DWORD)length, &written, nullptr) != 0 &&
               written == (DWORD)length;
    };

    bool ok = true;
    if (fileSize.QuadPart == 0) {
        string header = string(kLyricsCacheFileHeader) + "\n";
        ok = writeAll(header.data(), header.size());
    }

    if (ok) {
        for (const string& line : encodedLines) {
            if (line.empty()) {
                continue;
            }
            if (!writeAll(line.data(), line.size())) {
                ok = false;
                break;
            }
        }
    }

    CloseHandle(hFile);
    DebugLog(ok ? LOG_TRACE : LOG_VERBOSE,
             L"Lyrics disk append %ls path=%ls lines=%zu",
             ok ? L"ok" : L"failed", TruncateForLog(cachePath, 70).c_str(),
             encodedLines.size());
    return ok;
}

bool TryGetLyricsFromDiskCache(const wstring& trackKey, vector<LyricLine>* outLines,
                               wstring* outSourcePath) {
    if (!outLines || !g_Settings.enableLyricsDiskCache || trackKey.empty()) {
        return false;
    }

    EnsureLyricsDiskCacheLoaded();

    lock_guard<mutex> guard(g_LyricsDiskCache.lock);
    auto it = g_LyricsDiskCache.entries.find(trackKey);
    if (it == g_LyricsDiskCache.entries.end() || it->second.lines.empty()) {
        return false;
    }

    *outLines = it->second.lines;
    if (outSourcePath) {
        *outSourcePath = it->second.sourcePath.empty() ? L"CACHE" : it->second.sourcePath;
    }
    return true;
}

void SaveLyricsToDiskCache(const wstring& trackKey, const wstring& sourcePath,
                           const vector<LyricLine>& lines) {
    if (!g_Settings.enableLyricsDiskCache || trackKey.empty() || lines.empty()) {
        return;
    }

    EnsureLyricsDiskCacheLoaded();

    wstring cachePath;
    bool changed = true;
    {
        lock_guard<mutex> guard(g_LyricsDiskCache.lock);

        auto it = g_LyricsDiskCache.entries.find(trackKey);
        if (it != g_LyricsDiskCache.entries.end() &&
            it->second.sourcePath == sourcePath &&
            AreLyricLinesEqual(it->second.lines, lines)) {
            changed = false;
        }

        if (!changed) {
            return;
        }

        if (g_LyricsDiskCache.entries.find(trackKey) ==
                g_LyricsDiskCache.entries.end() &&
            g_LyricsDiskCache.entries.size() >= kLyricsDiskCacheMaxEntries) {
            g_LyricsDiskCache.entries.erase(g_LyricsDiskCache.entries.begin());
        }

        g_LyricsDiskCache.entries[trackKey] = {sourcePath, lines};
        g_LyricsDiskCache.dirty = true;
        cachePath = ResolveLyricsCacheFilePath();
    }

    if (!changed) {
        return;
    }

    string encodedLine;
    if (!BuildLyricsDiskCacheEncodedLine(trackKey, sourcePath, lines,
                                         &encodedLine)) {
        return;
    }

    vector<string> encodedLines;
    encodedLines.push_back(std::move(encodedLine));
    bool ok = AppendLyricsDiskCacheLines(cachePath, encodedLines);
    if (ok) {
        lock_guard<mutex> guard(g_LyricsDiskCache.lock);
        g_LyricsDiskCache.dirty = false;
        DebugLog(LOG_TRACE, L"Lyrics cache persist single key=%ls",
                 TruncateForLog(trackKey, 64).c_str());
    } else {
        DebugLog(LOG_VERBOSE, L"Lyrics cache persist single failed key=%ls",
                 TruncateForLog(trackKey, 64).c_str());
    }
}

void SaveLyricsBatchToDiskCache(
    const vector<pair<wstring, LyricsCacheEntry>>& batchItems) {
    if (!g_Settings.enableLyricsDiskCache || batchItems.empty()) {
        return;
    }

    EnsureLyricsDiskCacheLoaded();

    vector<size_t> changedIndices;
    changedIndices.reserve(batchItems.size());
    wstring cachePath;
    {
        lock_guard<mutex> guard(g_LyricsDiskCache.lock);

        for (size_t i = 0; i < batchItems.size(); i++) {
            const auto& item = batchItems[i];
            const wstring& trackKey = item.first;
            const LyricsCacheEntry& entry = item.second;
            if (trackKey.empty() || entry.lines.empty()) {
                continue;
            }

            auto it = g_LyricsDiskCache.entries.find(trackKey);
            if (it != g_LyricsDiskCache.entries.end() &&
                it->second.sourcePath == entry.sourcePath &&
                AreLyricLinesEqual(it->second.lines, entry.lines)) {
                continue;
            }

            if (it == g_LyricsDiskCache.entries.end() &&
                g_LyricsDiskCache.entries.size() >= kLyricsDiskCacheMaxEntries) {
                g_LyricsDiskCache.entries.erase(g_LyricsDiskCache.entries.begin());
            }

            g_LyricsDiskCache.entries[trackKey] = entry;
            changedIndices.push_back(i);
        }

        if (changedIndices.empty()) {
            return;
        }

        g_LyricsDiskCache.dirty = true;
        cachePath = ResolveLyricsCacheFilePath();
    }

    vector<string> encodedLines;
    encodedLines.reserve(changedIndices.size());
    for (size_t index : changedIndices) {
        const auto& item = batchItems[index];
        string encodedLine;
        if (BuildLyricsDiskCacheEncodedLine(item.first, item.second.sourcePath,
                                            item.second.lines, &encodedLine)) {
            encodedLines.push_back(std::move(encodedLine));
        }
    }

    if (encodedLines.empty()) {
        return;
    }

    bool ok = AppendLyricsDiskCacheLines(cachePath, encodedLines);
    if (ok) {
        lock_guard<mutex> guard(g_LyricsDiskCache.lock);
        g_LyricsDiskCache.dirty = false;
        DebugLog(LOG_VERBOSE, L"Lyrics cache persist batch ok entries=%zu",
                 encodedLines.size());
    } else {
        DebugLog(LOG_VERBOSE, L"Lyrics cache persist batch failed entries=%zu",
                 encodedLines.size());
    }
}

static bool WinHttpGetUtf8(const wstring& host, const wstring& pathAndQuery,
                           int timeoutMs, string* outResponse,
                           bool allowModeMutation = true) {
    if (!outResponse) return false;
    outResponse->clear();

    int safeTimeout = timeoutMs;
    if (safeTimeout < 400) safeTimeout = 400;
    if (safeTimeout > 5000) safeTimeout = 5000;

    auto modeToAccessType = [](int mode) -> DWORD {
        if (mode == 0) {
            return WINHTTP_ACCESS_TYPE_NO_PROXY;
        }
        // Unknown/auto mode probes DEFAULT_PROXY first, then flips on later tries.
        return WINHTTP_ACCESS_TYPE_DEFAULT_PROXY;
    };

    auto accessTypeToMode = [](DWORD accessType) -> int {
        return (accessType == WINHTTP_ACCESS_TYPE_DEFAULT_PROXY) ? 1 : 0;
    };

    int preferredMode = -1;
    {
        lock_guard<mutex> guard(g_LyricsHttpAccessModeLock);
        preferredMode = g_LyricsHttpPreferredAccessMode;
    }

    ULONGLONG requestStartTick = GetTickCount64();
    wstring logPath = TruncateForLog(pathAndQuery, 92);

    auto tryWithAccessType = [&](DWORD accessType) -> bool {
        HINTERNET hSession = WinHttpOpen(L"WindhawkLyrics/1.0", accessType,
                                         WINHTTP_NO_PROXY_NAME,
                                         WINHTTP_NO_PROXY_BYPASS, 0);
        if (!hSession) return false;

        DWORD connectRetries = 1;
        WinHttpSetOption(hSession, WINHTTP_OPTION_CONNECT_RETRIES, &connectRetries,
                         sizeof(connectRetries));
        WinHttpSetTimeouts(hSession, safeTimeout, safeTimeout, safeTimeout,
                           safeTimeout);

        HINTERNET hConnect = WinHttpConnect(hSession, host.c_str(),
                                            INTERNET_DEFAULT_HTTPS_PORT, 0);
        if (!hConnect) {
            WinHttpCloseHandle(hSession);
            return false;
        }

        HINTERNET hRequest =
            WinHttpOpenRequest(hConnect, L"GET", pathAndQuery.c_str(), nullptr,
                               WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
                               WINHTTP_FLAG_SECURE);
        if (!hRequest) {
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return false;
        }

        DWORD decompression = WINHTTP_DECOMPRESSION_FLAG_GZIP |
                              WINHTTP_DECOMPRESSION_FLAG_DEFLATE;
        WinHttpSetOption(hRequest, WINHTTP_OPTION_DECOMPRESSION, &decompression,
                         sizeof(decompression));

        BOOL ok = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                                     WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
        if (!ok) {
            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return false;
        }

        ok = WinHttpReceiveResponse(hRequest, nullptr);
        if (!ok) {
            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return false;
        }

        DWORD statusCode = 0;
        DWORD statusSize = sizeof(statusCode);
        if (!WinHttpQueryHeaders(hRequest,
                                 WINHTTP_QUERY_STATUS_CODE |
                                     WINHTTP_QUERY_FLAG_NUMBER,
                                 WINHTTP_HEADER_NAME_BY_INDEX, &statusCode,
                                 &statusSize, WINHTTP_NO_HEADER_INDEX) ||
            statusCode != 200) {
            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return false;
        }

        string body;
        for (;;) {
            DWORD available = 0;
            if (!WinHttpQueryDataAvailable(hRequest, &available)) {
                body.clear();
                break;
            }

            if (available == 0) {
                break;
            }

            size_t oldSize = body.size();
            body.resize(oldSize + available);

            DWORD bytesRead = 0;
            if (!WinHttpReadData(hRequest, body.data() + oldSize, available,
                                 &bytesRead)) {
                body.clear();
                break;
            }

            body.resize(oldSize + bytesRead);

            if (bytesRead == 0) {
                break;
            }
        }

        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);

        if (body.empty()) {
            return false;
        }

        *outResponse = std::move(body);
        return true;
    };

    DWORD selectedAccess = modeToAccessType(preferredMode);
    ULONGLONG attemptStartTick = GetTickCount64();
    if (tryWithAccessType(selectedAccess)) {
        int resolvedMode = accessTypeToMode(selectedAccess);
        if (allowModeMutation) {
            {
                lock_guard<mutex> guard(g_LyricsHttpAccessModeLock);
                g_LyricsHttpPreferredAccessMode = resolvedMode;
            }
            Wh_SetIntValue(RUNTIME_LYRICS_HTTP_MODE_VALUE, resolvedMode);
        }
        DebugLog(LOG_VERBOSE,
                 L"HTTP ok host=%ls path=%ls mode=%ls attemptMs=%llu totalMs=%llu",
                 host.c_str(), logPath.c_str(),
                 HttpAccessTypeName(selectedAccess),
                 (unsigned long long)(GetTickCount64() - attemptStartTick),
                 (unsigned long long)(GetTickCount64() - requestStartTick));
        return true;
    }

    DebugLog(LOG_TRACE, L"HTTP attempt failed host=%ls path=%ls mode=%ls attemptMs=%llu",
             host.c_str(), logPath.c_str(), HttpAccessTypeName(selectedAccess),
             (unsigned long long)(GetTickCount64() - attemptStartTick));

    if (allowModeMutation && preferredMode == -1) {
        int nextProbeMode =
            (selectedAccess == WINHTTP_ACCESS_TYPE_DEFAULT_PROXY) ? 0 : 1;
        {
            lock_guard<mutex> guard(g_LyricsHttpAccessModeLock);
            g_LyricsHttpPreferredAccessMode = nextProbeMode;
        }
        Wh_SetIntValue(RUNTIME_LYRICS_HTTP_MODE_VALUE, nextProbeMode);
        DebugLog(LOG_TRACE, L"HTTP probe switched nextMode=%ls host=%ls",
                 HttpAccessTypeName(modeToAccessType(nextProbeMode)),
                 host.c_str());
    }

    if (preferredMode == -1) {
        DebugLog(LOG_VERBOSE,
                 L"HTTP failed host=%ls path=%ls mode=%ls totalMs=%llu (auto probe single mode)",
                 host.c_str(), logPath.c_str(),
                 HttpAccessTypeName(selectedAccess),
                 (unsigned long long)(GetTickCount64() - requestStartTick));
    } else {
        DebugLog(LOG_VERBOSE,
                 L"HTTP failed host=%ls path=%ls mode=%ls totalMs=%llu (known mode, no fallback)",
                 host.c_str(), logPath.c_str(),
                 HttpAccessTypeName(selectedAccess),
                 (unsigned long long)(GetTickCount64() - requestStartTick));
    }
    return false;
}

static bool ParseJsonStringLiteral(const string& json, size_t quotePos,
                                   wstring* outValue, size_t* outEndPos) {
    if (!outValue || quotePos >= json.size() || json[quotePos] != '"') {
        return false;
    }

    string utf8;
    size_t i = quotePos + 1;

    auto appendUtf8Codepoint = [&utf8](unsigned int cp) {
        if (cp <= 0x7F) {
            utf8.push_back((char)cp);
        } else if (cp <= 0x7FF) {
            utf8.push_back((char)(0xC0 | ((cp >> 6) & 0x1F)));
            utf8.push_back((char)(0x80 | (cp & 0x3F)));
        } else if (cp <= 0xFFFF) {
            utf8.push_back((char)(0xE0 | ((cp >> 12) & 0x0F)));
            utf8.push_back((char)(0x80 | ((cp >> 6) & 0x3F)));
            utf8.push_back((char)(0x80 | (cp & 0x3F)));
        } else {
            utf8.push_back((char)(0xF0 | ((cp >> 18) & 0x07)));
            utf8.push_back((char)(0x80 | ((cp >> 12) & 0x3F)));
            utf8.push_back((char)(0x80 | ((cp >> 6) & 0x3F)));
            utf8.push_back((char)(0x80 | (cp & 0x3F)));
        }
    };

    while (i < json.size()) {
        char c = json[i++];
        if (c == '"') {
            *outValue = Utf8ToWide(utf8);
            if (outEndPos) *outEndPos = i;
            return true;
        }

        if (c != '\\') {
            utf8.push_back(c);
            continue;
        }

        if (i >= json.size()) return false;
        char esc = json[i++];
        switch (esc) {
            case '"': utf8.push_back('\"'); break;
            case '\\': utf8.push_back('\\'); break;
            case '/': utf8.push_back('/'); break;
            case 'b': utf8.push_back('\b'); break;
            case 'f': utf8.push_back('\f'); break;
            case 'n': utf8.push_back('\n'); break;
            case 'r': utf8.push_back('\r'); break;
            case 't': utf8.push_back('\t'); break;
            case 'u': {
                if (i + 4 > json.size()) return false;
                unsigned int code = 0;
                for (int j = 0; j < 4; j++) {
                    char h = json[i + j];
                    unsigned int v = 0;
                    if (h >= '0' && h <= '9') v = (unsigned int)(h - '0');
                    else if (h >= 'a' && h <= 'f') v = (unsigned int)(h - 'a' + 10);
                    else if (h >= 'A' && h <= 'F') v = (unsigned int)(h - 'A' + 10);
                    else return false;
                    code = (code << 4) | v;
                }
                i += 4;

                unsigned int cp = code;
                if (code >= 0xD800 && code <= 0xDBFF) {
                    if (i + 6 <= json.size() && json[i] == '\\' &&
                        json[i + 1] == 'u') {
                        unsigned int low = 0;
                        bool okLow = true;
                        for (int j = 0; j < 4; j++) {
                            char h = json[i + 2 + j];
                            unsigned int v = 0;
                            if (h >= '0' && h <= '9') v = (unsigned int)(h - '0');
                            else if (h >= 'a' && h <= 'f') v = (unsigned int)(h - 'a' + 10);
                            else if (h >= 'A' && h <= 'F') v = (unsigned int)(h - 'A' + 10);
                            else { okLow = false; break; }
                            low = (low << 4) | v;
                        }

                        if (okLow && low >= 0xDC00 && low <= 0xDFFF) {
                            cp = 0x10000 + (((code - 0xD800) << 10) |
                                            (low - 0xDC00));
                            i += 6;
                        }
                    }
                }

                appendUtf8Codepoint(cp);
                break;
            }
            default:
                return false;
        }
    }

    return false;
}

static bool ExtractJsonStringField(const string& json, const string& fieldName,
                                   wstring* outValue) {
    if (!outValue) return false;

    string key = string("\"") + fieldName + "\"";
    size_t searchPos = 0;
    while (searchPos < json.size()) {
        size_t keyPos = json.find(key, searchPos);
        if (keyPos == string::npos) {
            break;
        }

        size_t colonPos = json.find(':', keyPos + key.size());
        if (colonPos == string::npos) {
            break;
        }

        size_t valuePos = colonPos + 1;
        while (valuePos < json.size() &&
               (json[valuePos] == ' ' || json[valuePos] == '\t' ||
                json[valuePos] == '\r' || json[valuePos] == '\n')) {
            valuePos++;
        }

        // LRCLIB can return null for some results in /search arrays.
        // Keep scanning until we find an actual JSON string value.
        if (valuePos < json.size() && json[valuePos] == '"') {
            size_t endPos = 0;
            wstring parsedValue;
            if (ParseJsonStringLiteral(json, valuePos, &parsedValue, &endPos)) {
                if (!parsedValue.empty()) {
                    *outValue = std::move(parsedValue);
                    return true;
                }
            }
        }

        searchPos = keyPos + key.size();
    }

    return false;
}

static bool TryExtractLyricsFromLrclibResponse(const string& json,
                                               vector<LyricLine>* outLines) {
    if (!outLines) return false;
    outLines->clear();

    wstring syncedLyrics;
    if (ExtractJsonStringField(json, "syncedLyrics", &syncedLyrics)) {
        vector<LyricLine> parsed;
        if (ParseLrcContent(syncedLyrics, &parsed) && !parsed.empty()) {
            *outLines = std::move(parsed);
            return true;
        }
    }

    wstring plainLyrics;
    if (!ExtractJsonStringField(json, "plainLyrics", &plainLyrics)) {
        return false;
    }

    plainLyrics = TrimCopy(plainLyrics);
    if (plainLyrics.empty()) {
        return false;
    }

    size_t lineBreak = plainLyrics.find_first_of(L"\r\n");
    if (lineBreak != wstring::npos) {
        plainLyrics = TrimCopy(plainLyrics.substr(0, lineBreak));
    }

    if (plainLyrics.empty()) {
        return false;
    }

    outLines->push_back({0, plainLyrics});
    return true;
}

static wstring BuildLrclibGetPath(const wstring& title, const wstring& artist,
                                  int durationSeconds) {
    wstring query = L"/api/get?track_name=" + UrlEncodeUtf8(title);
    if (!artist.empty()) {
        query += L"&artist_name=" + UrlEncodeUtf8(artist);
    }
    if (durationSeconds > 0) {
        query += L"&duration=" + to_wstring(durationSeconds);
    }
    return query;
}

static bool FetchLyricsFromLrclib(const wstring& title, const wstring& artist,
                                  int durationSeconds,
                                  vector<LyricLine>* outLines,
                                  wstring* outSourcePath,
                                  bool allowHttpModeMutation = true) {
    if (!outLines || !outSourcePath) return false;
    (void)durationSeconds;
    outLines->clear();
    outSourcePath->clear();
    ULONGLONG fetchStartTick = GetTickCount64();

    wstring cleanTitle = NormalizeLyricsLookupPart(title);
    wstring cleanArtist = NormalizeLyricsLookupPart(artist);
    if (cleanTitle.empty()) return false;

    int requestTimeoutMs = g_Settings.onlineLyricsTimeoutMs;
    if (requestTimeoutMs < 650) {
        requestTimeoutMs = 650;
    }
    if (requestTimeoutMs > kOnlineLyricsRequestTimeoutCapMs) {
        requestTimeoutMs = kOnlineLyricsRequestTimeoutCapMs;
    }

    struct LrclibCandidate {
        wstring kind;
        wstring path;
    };

    vector<LrclibCandidate> candidates;
    candidates.push_back(
        {L"exact", BuildLrclibGetPath(cleanTitle, cleanArtist, 0)});

    wstring simplifiedTitle = SimplifyTrackTitleForLyricsLookup(cleanTitle);
    if (!simplifiedTitle.empty() && simplifiedTitle != cleanTitle) {
        candidates.push_back(
            {L"simplified", BuildLrclibGetPath(simplifiedTitle, cleanArtist, 0)});
    }

    struct LrclibRoundState {
        mutex lock;
        condition_variable cv;
        bool anySuccess = false;
        int completedAttempts = 0;
        int totalAttempts = 0;
        vector<LyricLine> winningLines;
        wstring winningSourcePath;
        wstring winningKind;
    };

    auto roundState = make_shared<LrclibRoundState>();
    roundState->totalAttempts = (int)candidates.size();

    for (const auto& candidate : candidates) {
        wstring workerTitle = cleanTitle;
        wstring workerArtist = cleanArtist;
        thread([candidate, workerTitle, workerArtist, requestTimeoutMs,
                allowHttpModeMutation, roundState]() {
            ULONGLONG attemptStartTick = GetTickCount64();
            string json;
            bool httpOk =
                WinHttpGetUtf8(L"lrclib.net", candidate.path, requestTimeoutMs,
                               &json, allowHttpModeMutation);

            vector<LyricLine> parsedLines;
            bool parsedOk = false;
            if (httpOk) {
                parsedOk = TryExtractLyricsFromLrclibResponse(json, &parsedLines) &&
                           !parsedLines.empty();
            }

            ULONGLONG attemptMs = GetTickCount64() - attemptStartTick;
            DebugLog(LOG_VERBOSE,
                     L"LRCLIB attempt kind=%ls title=%ls artist=%ls http=%d parsed=%d lines=%zu ms=%llu",
                     candidate.kind.c_str(),
                     TruncateForLog(workerTitle, 48).c_str(),
                     TruncateForLog(workerArtist, 40).c_str(),
                     httpOk ? 1 : 0, parsedOk ? 1 : 0, parsedLines.size(),
                     (unsigned long long)attemptMs);

            {
                lock_guard<mutex> guard(roundState->lock);
                if (parsedOk && !roundState->anySuccess) {
                    roundState->anySuccess = true;
                    roundState->winningLines = std::move(parsedLines);
                    roundState->winningSourcePath = L"LRCLIB";
                    roundState->winningKind = candidate.kind;
                }
                roundState->completedAttempts++;
            }
            roundState->cv.notify_one();
        }).detach();
    }

    ULONGLONG softDeadlineTick =
        fetchStartTick + (ULONGLONG)kLrclibFetchRoundDeadlineMs;
    ULONGLONG hardDeadlineTick =
        fetchStartTick + (ULONGLONG)kLrclibFetchHardDeadlineMs;
    bool deadlineHit = false;
    bool anySuccess = false;
    int completedAttempts = 0;
    vector<LyricLine> winningLines;
    wstring winningSourcePath;
    wstring winningKind;

    {
        unique_lock<mutex> lock(roundState->lock);
        while (!roundState->anySuccess &&
               roundState->completedAttempts < roundState->totalAttempts) {
            ULONGLONG nowTick = GetTickCount64();
            if (nowTick >= hardDeadlineTick) {
                deadlineHit = true;
                break;
            }

            if (nowTick >= softDeadlineTick &&
                roundState->completedAttempts > 0) {
                deadlineHit = true;
                break;
            }

            ULONGLONG remainingMs = hardDeadlineTick - nowTick;
            if (remainingMs > 50ULL) {
                remainingMs = 50ULL;
            }
            roundState->cv.wait_for(
                lock, chrono::milliseconds((long long)remainingMs));
        }

        anySuccess = roundState->anySuccess;
        completedAttempts = roundState->completedAttempts;
        if (anySuccess) {
            winningLines = roundState->winningLines;
            winningSourcePath = roundState->winningSourcePath;
            winningKind = roundState->winningKind;
        }
    }

    if (anySuccess && !winningLines.empty()) {
        *outLines = std::move(winningLines);
        *outSourcePath = winningSourcePath.empty() ? L"LRCLIB" : winningSourcePath;
        DebugLog(LOG_BASIC,
                 L"LRCLIB hit kind=%ls title=%ls artist=%ls lines=%zu ms=%llu",
                 winningKind.empty() ? L"unknown" : winningKind.c_str(),
                 TruncateForLog(cleanTitle, 48).c_str(),
                 TruncateForLog(cleanArtist, 40).c_str(), outLines->size(),
                 (unsigned long long)(GetTickCount64() - fetchStartTick));
        return true;
    }

    int pendingAttempts = (int)candidates.size() - completedAttempts;
    if (pendingAttempts < 0) {
        pendingAttempts = 0;
    }
    DebugLog(
        LOG_VERBOSE,
        L"LRCLIB miss title=%ls artist=%ls attempts=%d pending=%d deadlineHit=%d ms=%llu",
        TruncateForLog(cleanTitle, 48).c_str(),
        TruncateForLog(cleanArtist, 40).c_str(), completedAttempts,
        pendingAttempts, deadlineHit ? 1 : 0,
        (unsigned long long)(GetTickCount64() - fetchStartTick));
    return false;
}

static bool ParseJsonLabelStringNear(const string& json, size_t anchorPos,
                                     wstring* outValue, size_t* outEndPos) {
    if (!outValue) return false;

    size_t labelPos = json.find("\"label\"", anchorPos);
    if (labelPos == string::npos) return false;

    size_t colonPos = json.find(':', labelPos + 7);
    if (colonPos == string::npos) return false;

    size_t valuePos = colonPos + 1;
    while (valuePos < json.size() &&
           (json[valuePos] == ' ' || json[valuePos] == '\t' ||
            json[valuePos] == '\r' || json[valuePos] == '\n')) {
        valuePos++;
    }
    if (valuePos >= json.size() || json[valuePos] != '"') {
        return false;
    }

    return ParseJsonStringLiteral(json, valuePos, outValue, outEndPos);
}

static void ExtractTopTracksFromItunesFeedJson(
    const string& json, size_t maxCount,
    vector<pair<wstring, wstring>>* outTracks) {
    if (!outTracks) return;
    outTracks->clear();
    if (json.empty() || maxCount == 0) return;

    unordered_set<wstring> seen;
    size_t cursor = 0;

    while (outTracks->size() < maxCount) {
        size_t nameAnchor = json.find("\"im:name\"", cursor);
        if (nameAnchor == string::npos) {
            break;
        }

        wstring title;
        size_t titleEnd = 0;
        if (!ParseJsonLabelStringNear(json, nameAnchor, &title, &titleEnd)) {
            cursor = nameAnchor + 9;
            continue;
        }

        size_t artistAnchor = json.find("\"im:artist\"", titleEnd);
        if (artistAnchor == string::npos) {
            break;
        }
        if (artistAnchor > nameAnchor + 3200) {
            cursor = titleEnd;
            continue;
        }

        wstring artist;
        size_t artistEnd = 0;
        if (!ParseJsonLabelStringNear(json, artistAnchor, &artist, &artistEnd)) {
            cursor = artistAnchor + 11;
            continue;
        }

        title = TrimCopy(title);
        artist = TrimCopy(artist);
        if (!title.empty() && !artist.empty()) {
            wstring dedupeKey = BuildLyricsTrackKey(title, artist);
            if (seen.insert(dedupeKey).second) {
                outTracks->push_back({title, artist});
            }
        }

        cursor = artistEnd;
    }
}

static bool ShouldYieldBootstrapToActiveLyricsFetch() {
    if (!g_RuntimeLyricsEnabled) {
        return false;
    }

    lock_guard<mutex> guard(g_LyricsState.lock);
    if (g_LyricsState.trackKey.empty()) {
        return false;
    }

    if (g_LyricsState.onlineFetchInProgress) {
        return true;
    }

    if (!g_LyricsState.lines.empty() || g_LyricsState.lastLoadAttemptTick == 0) {
        return false;
    }

    ULONGLONG now = GetTickCount64();
    if (now < g_LyricsState.lastLoadAttemptTick) {
        return false;
    }

    // Keep bootstrap passive for a short window while current song fetch settles.
    return (now - g_LyricsState.lastLoadAttemptTick) < 3000ULL;
}

void StartLyricsBootstrapIfNeeded() {
    if (!g_RuntimeLyricsEnabled || !g_Settings.enableLyricsDiskCache ||
        !g_Settings.prefetchTopLyricsOnFirstEnable ||
        g_Settings.prefetchTopLyricsCount <= 0) {
        DebugLog(LOG_TRACE,
                 L"Lyrics bootstrap skipped: enabled=%d diskCache=%d prefetch=%d count=%d",
                 g_RuntimeLyricsEnabled ? 1 : 0,
                 g_Settings.enableLyricsDiskCache ? 1 : 0,
                 g_Settings.prefetchTopLyricsOnFirstEnable ? 1 : 0,
                 g_Settings.prefetchTopLyricsCount);
        return;
    }

    if (Wh_GetIntValue(RUNTIME_LYRICS_BOOTSTRAP_DONE_VALUE, 0) != 0) {
        DebugLog(LOG_TRACE, L"Lyrics bootstrap skipped: already completed");
        return;
    }

    {
        lock_guard<mutex> guard(g_LyricsBootstrapLock);
        if (g_LyricsBootstrapInProgress) {
            DebugLog(LOG_TRACE, L"Lyrics bootstrap skipped: already running");
            return;
        }
        g_LyricsBootstrapInProgress = true;
    }

    int targetCount = g_Settings.prefetchTopLyricsCount;
    if (targetCount < 1) targetCount = 1;
    if (targetCount > 2000) targetCount = 2000;

    thread([targetCount]() {
        size_t cachedCount = 0;
        ULONGLONG bootstrapStartTick = GetTickCount64();
        DebugLog(LOG_BASIC, L"Lyrics bootstrap started target=%d", targetCount);
        try {
            // Let the active track load first so bootstrap doesn't compete immediately.
            Sleep(12000);
            bool bootstrapAllowed =
                g_RuntimeLyricsEnabled && g_Settings.enableLyricsDiskCache;

            if (bootstrapAllowed) {
                vector<wstring> regions = {
                    L"us", L"gb", L"de", L"fr", L"ca", L"au", L"jp", L"br",
                    L"mx", L"es", L"it", L"nl", L"se", L"ch", L"tr", L"pl",
                    L"kr", L"in", L"id", L"za"};

                vector<pair<wstring, wstring>> candidates;
                candidates.reserve((size_t)targetCount);
                unordered_set<wstring> seen;

                for (const auto& region : regions) {
                    if (candidates.size() >= (size_t)targetCount) {
                        break;
                    }

                    wstring path =
                        L"/" + region + L"/rss/topsongs/limit=200/json";
                    string json;
                    if (!WinHttpGetUtf8(L"itunes.apple.com", path, 1100, &json,
                                        false)) {
                        continue;
                    }

                    vector<pair<wstring, wstring>> regionTracks;
                    ExtractTopTracksFromItunesFeedJson(
                        json, (size_t)targetCount, &regionTracks);

                    for (const auto& track : regionTracks) {
                        if (candidates.size() >= (size_t)targetCount) {
                            break;
                        }
                        wstring key = BuildLyricsTrackKey(track.first, track.second);
                        if (seen.insert(key).second) {
                            candidates.push_back(track);
                        }
                    }
                }

                DebugLog(LOG_VERBOSE, L"Lyrics bootstrap candidates=%zu", candidates.size());

                vector<pair<wstring, LyricsCacheEntry>> batch;
                batch.reserve(32);

                for (const auto& track : candidates) {
                    if (cachedCount >= (size_t)targetCount) {
                        break;
                    }

                    if (ShouldYieldBootstrapToActiveLyricsFetch()) {
                        Sleep(90);
                        continue;
                    }

                    wstring trackKey = BuildLyricsTrackKey(track.first, track.second);
                    vector<LyricLine> existing;
                    if (TryGetLyricsFromDiskCache(trackKey, &existing, nullptr) &&
                        !existing.empty()) {
                        continue;
                    }

                    vector<LyricLine> lines;
                    wstring sourcePath;
                    if (!FetchLyricsFromLrclib(track.first, track.second, 0, &lines,
                                               &sourcePath, false) ||
                        lines.empty()) {
                        continue;
                    }

                    LyricsCacheEntry entry;
                    entry.sourcePath =
                        sourcePath.empty() ? L"LRCLIB_BOOTSTRAP" : sourcePath;
                    entry.lines = std::move(lines);
                    batch.push_back({trackKey, std::move(entry)});
                    cachedCount++;

                    if (batch.size() >= 32) {
                        SaveLyricsBatchToDiskCache(batch);
                        batch.clear();
                    }

                    Sleep(8);
                }

                if (!batch.empty()) {
                    SaveLyricsBatchToDiskCache(batch);
                }
            }
        } catch (...) {
        }

        if (cachedCount > 0) {
            Wh_SetIntValue(RUNTIME_LYRICS_BOOTSTRAP_DONE_VALUE, 1);
        }
        DebugLog(LOG_BASIC,
                 L"Lyrics bootstrap finished cached=%zu doneFlag=%d ms=%llu",
                 cachedCount, cachedCount > 0 ? 1 : 0,
                 (unsigned long long)(GetTickCount64() - bootstrapStartTick));

        {
            lock_guard<mutex> guard(g_LyricsBootstrapLock);
            g_LyricsBootstrapInProgress = false;
        }
    }).detach();
}

vector<wstring> BuildLyricsCandidatePaths(const wstring& title, const wstring& artist) {
    vector<wstring> paths;

    wstring safeTitle = SanitizePathPart(title);
    wstring safeArtist = SanitizePathPart(artist);
    if (safeTitle.empty()) {
        return paths;
    }

    wstring folder = ResolveLyricsFolderPath();
    if (!folder.empty() && folder.back() != L'\\' && folder.back() != L'/') {
        folder += L'\\';
    }

    if (!safeArtist.empty()) {
        paths.push_back(folder + safeArtist + L" - " + safeTitle + L".lrc");
        paths.push_back(folder + safeTitle + L" - " + safeArtist + L".lrc");
    }
    paths.push_back(folder + safeTitle + L".lrc");

    return paths;
}

void ResetLyricsState() {
    lock_guard<mutex> guard(g_LyricsState.lock);
    g_LyricsState.trackKey.clear();
    g_LyricsState.sourcePath.clear();
    g_LyricsState.lines.clear();
    g_LyricsState.lastLoadAttemptTick = 0;
    g_LyricsState.retryBlockedUntilTick = 0;
    g_LyricsState.consecutiveOnlineFailures = 0;
    g_LyricsState.onlineFetchInProgress = false;
    ResetLyricsAnimationState();
}

void ResetLyricsAnimationState() {
    g_LastAnimatedLyricLine.clear();
    g_PreviousAnimatedLyricLine.clear();
    g_LastLyricLineChangeTick = 0;
}

void StartOnlineLyricsFetchAsync(const wstring& trackKey, const wstring& title,
                                 const wstring& artist, int durationSeconds) {
    thread([trackKey, title, artist, durationSeconds]() {
        ULONGLONG fetchStartTick = GetTickCount64();
        DebugLog(LOG_VERBOSE, L"Lyrics async start key=%ls title=%ls artist=%ls",
                 TruncateForLog(trackKey, 64).c_str(),
                 TruncateForLog(title, 44).c_str(),
                 TruncateForLog(artist, 36).c_str());

        vector<LyricLine> loadedLines;
        wstring sourcePath;
        bool ok = FetchLyricsFromLrclib(title, artist, durationSeconds, &loadedLines,
                                        &sourcePath);
        ULONGLONG completedTick = GetTickCount64();
        bool notify = false;
        bool shouldPersist = false;
        bool forceHttpModeProbe = false;

        {
            lock_guard<mutex> guard(g_LyricsState.lock);
            if (g_LyricsState.trackKey == trackKey) {
                if (ok && !loadedLines.empty()) {
                    g_LyricsState.lines = loadedLines;
                    g_LyricsState.sourcePath = sourcePath;
                    g_LyricsState.consecutiveOnlineFailures = 0;
                    g_LyricsState.retryBlockedUntilTick = 0;
                    shouldPersist = true;
                } else {
                    if (g_LyricsState.consecutiveOnlineFailures < 8) {
                        g_LyricsState.consecutiveOnlineFailures++;
                    }
                    ULONGLONG backoffMs = ComputeLyricsRetryBackoffMs(
                        g_LyricsState.consecutiveOnlineFailures);
                    g_LyricsState.retryBlockedUntilTick = completedTick + backoffMs;
                    if (g_LyricsState.consecutiveOnlineFailures >= 2 &&
                        (g_LyricsState.consecutiveOnlineFailures % 2) == 0) {
                        forceHttpModeProbe = true;
                    }
                }
                g_LyricsState.lastLoadAttemptTick = completedTick;
                g_LyricsState.onlineFetchInProgress = false;
                notify = true;
            }
        }

        if (forceHttpModeProbe) {
            {
                lock_guard<mutex> guard(g_LyricsHttpAccessModeLock);
                g_LyricsHttpPreferredAccessMode = -1;
            }
            Wh_SetIntValue(RUNTIME_LYRICS_HTTP_MODE_VALUE, -1);
            DebugLog(LOG_VERBOSE,
                     L"Lyrics HTTP mode reset to auto-probe after repeated failures key=%ls",
                     TruncateForLog(trackKey, 64).c_str());
        }

        if (!notify) {
            DebugLog(LOG_TRACE, L"Lyrics async ignored stale key=%ls",
                     TruncateForLog(trackKey, 64).c_str());
        }

        if (ok && !loadedLines.empty()) {
            DebugLog(LOG_BASIC,
                     L"Lyrics async success key=%ls lines=%zu source=%ls ms=%llu",
                     TruncateForLog(trackKey, 64).c_str(), loadedLines.size(),
                     TruncateForLog(sourcePath, 52).c_str(),
                     (unsigned long long)(completedTick - fetchStartTick));
        } else {
            int failureCount = 0;
            ULONGLONG retryWaitMs = 0;
            {
                lock_guard<mutex> guard(g_LyricsState.lock);
                if (g_LyricsState.trackKey == trackKey) {
                    failureCount = g_LyricsState.consecutiveOnlineFailures;
                    if (g_LyricsState.retryBlockedUntilTick > completedTick) {
                        retryWaitMs =
                            g_LyricsState.retryBlockedUntilTick - completedTick;
                    }
                }
            }
            DebugLog(LOG_VERBOSE,
                     L"Lyrics async miss key=%ls ms=%llu failures=%d backoffMs=%llu",
                     TruncateForLog(trackKey, 64).c_str(),
                     (unsigned long long)(completedTick - fetchStartTick),
                     failureCount, (unsigned long long)retryWaitMs);
        }

        if (shouldPersist) {
            SaveLyricsToDiskCache(trackKey, sourcePath, loadedLines);
        }

        if (notify && g_hMediaWindow && IsWindow(g_hMediaWindow)) {
            PostMessage(g_hMediaWindow, WM_APP + 20, 0, 0);
        }
    }).detach();
}

bool LoadLyricsForTrack(const wstring& title, const wstring& artist,
                        int durationSeconds) {
    if (!g_RuntimeLyricsEnabled) {
        return false;
    }

    ULONGLONG loadStartTick = GetTickCount64();
    wstring lookupTitle = NormalizeLyricsLookupPart(title);
    wstring lookupArtist = NormalizeLyricsLookupPart(artist);
    if (lookupTitle.empty()) {
        return false;
    }
    wstring trackKey = BuildLyricsTrackKey(lookupTitle, lookupArtist);

    bool trackChanged = false;
    {
        lock_guard<mutex> guard(g_LyricsState.lock);
        trackChanged = (g_LyricsState.trackKey != trackKey);
        if (trackChanged) {
            g_LyricsState.trackKey = trackKey;
            g_LyricsState.sourcePath.clear();
            g_LyricsState.lines.clear();
            g_LyricsState.onlineFetchInProgress = false;
            g_LyricsState.lastLoadAttemptTick = 0;
            g_LyricsState.retryBlockedUntilTick = 0;
            g_LyricsState.consecutiveOnlineFailures = 0;
            ResetLyricsAnimationState();
            DebugLog(LOG_BASIC,
                     L"Lyrics track changed key=%ls title=%ls artist=%ls",
                     TruncateForLog(trackKey, 64).c_str(),
                     TruncateForLog(lookupTitle, 44).c_str(),
                     TruncateForLog(lookupArtist, 36).c_str());
        }

        if (g_LyricsState.trackKey == trackKey) {
            if (!g_LyricsState.lines.empty()) {
                return true;
            }
            ULONGLONG nowTick = GetTickCount64();
            if (!trackChanged && g_LyricsState.retryBlockedUntilTick > nowTick) {
                return false;
            }
            if (g_LyricsState.onlineFetchInProgress) {
                bool stalled = false;
                if (g_LyricsState.lastLoadAttemptTick != 0 &&
                    nowTick >= g_LyricsState.lastLoadAttemptTick) {
                    ULONGLONG elapsedMs = nowTick - g_LyricsState.lastLoadAttemptTick;
                    ULONGLONG stallLimitMs = (ULONGLONG)kLyricsFetchStallMs;
                    if (elapsedMs >= stallLimitMs) {
                        stalled = true;
                    }
                }

                if (!stalled) {
                    return false;
                }

                // Recover from a stuck fetch thread and allow retry.
                DebugLog(LOG_BASIC,
                         L"Lyrics fetch stalled key=%ls -> retry",
                         TruncateForLog(trackKey, 64).c_str());
                g_LyricsState.onlineFetchInProgress = false;
                g_LyricsState.lastLoadAttemptTick = nowTick;
                if (g_LyricsState.consecutiveOnlineFailures < 8) {
                    g_LyricsState.consecutiveOnlineFailures++;
                }
                ULONGLONG backoffMs = ComputeLyricsRetryBackoffMs(
                    g_LyricsState.consecutiveOnlineFailures);
                g_LyricsState.retryBlockedUntilTick = nowTick + backoffMs;
                return false;
            }
            if (g_LyricsState.lastLoadAttemptTick != 0 &&
                nowTick >= g_LyricsState.lastLoadAttemptTick) {
                ULONGLONG elapsedMs = nowTick - g_LyricsState.lastLoadAttemptTick;
                if (!trackChanged && elapsedMs < (ULONGLONG)kLyricsRetryCooldownMs) {
                    return false;
                }
            }
        }
    }

    vector<LyricLine> loadedLines;
    wstring sourcePath;

    if (TryGetLyricsFromDiskCache(trackKey, &loadedLines, &sourcePath) &&
        !loadedLines.empty()) {
        ULONGLONG completedTick = GetTickCount64();
        DebugLog(LOG_BASIC,
                 L"Lyrics cache hit key=%ls lines=%zu source=%ls ms=%llu",
                 TruncateForLog(trackKey, 64).c_str(), loadedLines.size(),
                 TruncateForLog(sourcePath, 52).c_str(),
                 (unsigned long long)(completedTick - loadStartTick));
        lock_guard<mutex> guard(g_LyricsState.lock);
        if (g_LyricsState.trackKey != trackKey) {
            return false;
        }
        g_LyricsState.sourcePath = sourcePath;
        g_LyricsState.lines = std::move(loadedLines);
        g_LyricsState.lastLoadAttemptTick = completedTick;
        g_LyricsState.retryBlockedUntilTick = 0;
        g_LyricsState.consecutiveOnlineFailures = 0;
        g_LyricsState.onlineFetchInProgress = false;
        return true;
    }

    vector<wstring> candidates = BuildLyricsCandidatePaths(lookupTitle, lookupArtist);
    for (const auto& candidate : candidates) {
        wstring content;
        if (!ReadLyricsTextFile(candidate, &content)) {
            continue;
        }

        if (ParseLrcContent(content, &loadedLines)) {
            sourcePath = candidate;
            break;
        }
    }

    if (!loadedLines.empty()) {
        ULONGLONG completedTick = GetTickCount64();
        DebugLog(LOG_BASIC, L"Lyrics local file hit key=%ls file=%ls lines=%zu ms=%llu",
                 TruncateForLog(trackKey, 64).c_str(),
                 TruncateForLog(sourcePath, 60).c_str(), loadedLines.size(),
                 (unsigned long long)(completedTick - loadStartTick));
        SaveLyricsToDiskCache(trackKey, sourcePath, loadedLines);
        lock_guard<mutex> guard(g_LyricsState.lock);
        if (g_LyricsState.trackKey != trackKey) {
            return false;
        }
        g_LyricsState.sourcePath = sourcePath;
        g_LyricsState.lines = std::move(loadedLines);
        g_LyricsState.lastLoadAttemptTick = completedTick;
        g_LyricsState.retryBlockedUntilTick = 0;
        g_LyricsState.consecutiveOnlineFailures = 0;
        g_LyricsState.onlineFetchInProgress = false;
        return true;
    }

    if (!g_Settings.enableOnlineLyrics) {
        if (trackChanged) {
            DebugLog(LOG_VERBOSE,
                     L"Lyrics online disabled key=%ls (no cache/local match)",
                     TruncateForLog(trackKey, 64).c_str());
        }
        return false;
    }

    bool startFetch = false;
    {
        lock_guard<mutex> guard(g_LyricsState.lock);
        if (g_LyricsState.trackKey == trackKey &&
            g_LyricsState.lines.empty() &&
            !g_LyricsState.onlineFetchInProgress) {
            ULONGLONG nowTick = GetTickCount64();
            if (!trackChanged && g_LyricsState.retryBlockedUntilTick > nowTick) {
                return false;
            }
            if (g_LyricsState.lastLoadAttemptTick == 0 ||
                nowTick < g_LyricsState.lastLoadAttemptTick ||
                (nowTick - g_LyricsState.lastLoadAttemptTick) >=
                    (ULONGLONG)kLyricsRetryCooldownMs ||
                trackChanged) {
                g_LyricsState.lastLoadAttemptTick = nowTick;
                g_LyricsState.onlineFetchInProgress = true;
                startFetch = true;
            }
        }
    }

    if (startFetch) {
        DebugLog(LOG_BASIC,
                 L"Lyrics async scheduled key=%ls title=%ls artist=%ls",
                 TruncateForLog(trackKey, 64).c_str(),
                 TruncateForLog(lookupTitle, 44).c_str(),
                 TruncateForLog(lookupArtist, 36).c_str());
        StartOnlineLyricsFetchAsync(trackKey, lookupTitle, lookupArtist,
                                    durationSeconds);
    } else if (trackChanged) {
        DebugLog(LOG_TRACE, L"Lyrics async not scheduled yet key=%ls",
                 TruncateForLog(trackKey, 64).c_str());
    }

    return false;
}

bool TryGetCurrentLyricsLine(int64_t position100ns, wstring* outLine,
                             int64_t* outTimestamp100ns) {
    if (outLine) {
        outLine->clear();
    }
    if (outTimestamp100ns) {
        *outTimestamp100ns = 0;
    }

    if (!g_RuntimeLyricsEnabled) {
        return false;
    }

    lock_guard<mutex> guard(g_LyricsState.lock);
    if (g_LyricsState.lines.empty()) {
        return false;
    }

    auto setIfNonEmpty = [&](const LyricLine& line) -> bool {
        wstring lineText = TrimCopy(line.text);
        if (lineText.empty()) {
            return false;
        }
        if (outLine) {
            *outLine = lineText;
        }
        if (outTimestamp100ns) {
            *outTimestamp100ns = line.timestamp100ns;
        }
        return true;
    };

    int64_t effectivePosition100ns =
        position100ns - (int64_t)kLyricsHoldWindowMs * 10000LL;
    if (effectivePosition100ns < 0) {
        effectivePosition100ns = 0;
    }

    auto begin = g_LyricsState.lines.begin();
    auto end = g_LyricsState.lines.end();

    auto it = upper_bound(
        begin, end, effectivePosition100ns,
        [](int64_t ts, const LyricLine& line) {
            return ts < line.timestamp100ns;
        });

    // Before the first timestamp, show the first available lyric immediately.
    // This avoids fixed intro-delay perception on many tracks.
    if (it == begin) {
        for (auto jt = begin; jt != end; ++jt) {
            if (setIfNonEmpty(*jt)) {
                return true;
            }
        }
        return false;
    }

    // Walk backwards until we find the latest non-empty lyric line.
    do {
        --it;
        if (setIfNonEmpty(*it)) {
            return true;
        }
    } while (it != begin);

    return false;
}

wstring GetCurrentLyricsLine(int64_t position100ns) {
    wstring lineText;
    if (TryGetCurrentLyricsLine(position100ns, &lineText, nullptr)) {
        return lineText;
    }
    return L"";
}

bool IsSpotifyAumid(const wstring& appId) {
    // Spotify store and desktop builds expose an AUMID containing "spotify".
    return ToLowerCopy(appId).find(L"spotify") != wstring::npos;
}

bool IsSpotifySession(GlobalSystemMediaTransportControlsSession const& session) {
    if (!session) return false;
    try {
        return IsSpotifyAumid(session.SourceAppUserModelId().c_str());
    } catch (...) {
        return false;
    }
}

GlobalSystemMediaTransportControlsSession GetSpotifySession() {
    if (!g_SessionManager) return nullptr;

    GlobalSystemMediaTransportControlsSession spotifyFallback = nullptr;
    GlobalSystemMediaTransportControlsSession anyPlaying = nullptr;
    GlobalSystemMediaTransportControlsSession anyFallback = nullptr;

    auto sessionsList = g_SessionManager.GetSessions();
    for (auto const& s : sessionsList) {
        if (!anyFallback) {
            anyFallback = s;
        }

        bool isPlaying = false;
        try {
            auto pb = s.GetPlaybackInfo();
            isPlaying = pb &&
                        pb.PlaybackStatus() ==
                            GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing;
        } catch (...) {
            isPlaying = false;
        }

        if (isPlaying && !anyPlaying) {
            anyPlaying = s;
        }

        if (!IsSpotifySession(s)) {
            continue;
        }

        if (isPlaying) {
            return s;
        }

        if (!spotifyFallback) {
            spotifyFallback = s;
        }
    }

    if (spotifyFallback) {
        return spotifyFallback;
    }

    try {
        auto current = g_SessionManager.GetCurrentSession();
        if (current) {
            return current;
        }
    } catch (...) {
    }

    if (anyPlaying) {
        return anyPlaying;
    }

    return anyFallback;
}

void ClearMediaState() {
    DebugLog(LOG_TRACE, L"ClearMediaState");
    {
        lock_guard<mutex> guard(g_MediaState.lock);
        g_MediaState.hasMedia = false;
        g_MediaState.isPlaying = false;
        g_MediaState.title = L"Waiting for Spotify...";
        g_MediaState.artist = L"";
        g_MediaState.albumArt.reset();
        g_MediaState.albumArtCacheKey.clear();
        g_MediaState.timelinePosition100ns = 0;
        g_MediaState.timelineDuration100ns = 0;
        g_MediaState.canSeek = false;
    }

    ResetLyricsState();
}

void UnsubscribeSpotifySessionEvents() {

    if (!g_SpotifySessionForEvents) {
        return;
    }

    if (g_PlaybackChangedToken.value != 0) {
        g_SpotifySessionForEvents.PlaybackInfoChanged(g_PlaybackChangedToken);
    }
    if (g_MediaPropertiesChangedToken.value != 0) {
        g_SpotifySessionForEvents.MediaPropertiesChanged(g_MediaPropertiesChangedToken);
    }

    g_PlaybackChangedToken = {};
    g_MediaPropertiesChangedToken = {};
    g_SpotifySessionForEvents = nullptr;
}

void EnsureSessionManagerEvents() {
    if (!g_SessionManager || g_ManagerEventsSubscribed) {
        return;
    }

    g_SessionsChangedToken = g_SessionManager.SessionsChanged([](auto const&, auto const&) {
        if (g_hMediaWindow) {
            PostMessage(g_hMediaWindow, WM_APP + 20, 0, 0);
        }
    });

    g_CurrentSessionChangedToken = g_SessionManager.CurrentSessionChanged([](auto const&, auto const&) {
        if (g_hMediaWindow) {
            PostMessage(g_hMediaWindow, WM_APP + 20, 0, 0);
        }
    });

    g_ManagerEventsSubscribed = true;
}

void SubscribeSpotifySessionEvents(GlobalSystemMediaTransportControlsSession const& session) {
    if (g_SpotifySessionForEvents == session) {
        return;
    }

    UnsubscribeSpotifySessionEvents();

    if (!session) {
        return;
    }

    g_PlaybackChangedToken = session.PlaybackInfoChanged([](auto const&, auto const&) {
        if (g_hMediaWindow) {
            PostMessage(g_hMediaWindow, WM_APP + 20, 0, 0);
        }
    });

    g_MediaPropertiesChangedToken = session.MediaPropertiesChanged([](auto const&, auto const&) {
        if (g_hMediaWindow) {
            PostMessage(g_hMediaWindow, WM_APP + 20, 0, 0);
        }
    });

    g_SpotifySessionForEvents = session;
}

void RefreshSpotifyMediaState() {
    try {
        if (!g_SessionManager) {
            g_SessionManager = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
        }
        if (!g_SessionManager) {
            ClearMediaState();
            return;
        }

        EnsureSessionManagerEvents();
        auto session = GetSpotifySession();
        SubscribeSpotifySessionEvents(session);

        if (!session) {
            ClearMediaState();
            return;
        }

        auto props = session.TryGetMediaPropertiesAsync().get();
        auto info = session.GetPlaybackInfo();
        auto timeline = session.GetTimelineProperties();

        wstring newTitle = props.Title().c_str();
        wstring newArtist = props.Artist().c_str();
        wstring newAlbumArtKey = newTitle + L"|" + newArtist;

        bool shouldReloadAlbumArt = false;
        bool trackChangedForLog = false;
        {
            lock_guard<mutex> guard(g_MediaState.lock);
            shouldReloadAlbumArt = (newAlbumArtKey != g_MediaState.albumArtCacheKey);
            trackChangedForLog =
                (newTitle != g_MediaState.title || newArtist != g_MediaState.artist);
        }

        if (trackChangedForLog) {
            DebugLog(LOG_VERBOSE, L"Media track changed title=%ls artist=%ls",
                     TruncateForLog(newTitle, 44).c_str(),
                     TruncateForLog(newArtist, 36).c_str());
        }

        shared_ptr<Bitmap> newAlbumArt;
        bool thumbRefAvailable = false;
        bool albumArtDecoded = false;
        if (shouldReloadAlbumArt) {
            auto thumbRef = props.Thumbnail();
            if (thumbRef) {
                thumbRefAvailable = true;
                try {
                    auto stream = thumbRef.OpenReadAsync().get();
                    if (Bitmap* bmp = StreamToBitmap(stream)) {
                        newAlbumArt = shared_ptr<Bitmap>(bmp, [](Bitmap* p) { delete p; });
                        albumArtDecoded = true;
                    }
                } catch (...) {
                }
            }
        }

        int64_t start100ns = timeline.StartTime().count();
        int64_t end100ns = timeline.EndTime().count();
        int64_t position100ns = timeline.Position().count();
        int64_t duration100ns = 0;
        int64_t relativePosition100ns = 0;

        if (end100ns > start100ns) {
            duration100ns = end100ns - start100ns;
            relativePosition100ns = position100ns - start100ns;
            if (relativePosition100ns < 0) relativePosition100ns = 0;
            if (relativePosition100ns > duration100ns) {
                relativePosition100ns = duration100ns;
            }
        }

        int durationSecondsForLyrics =
            (duration100ns > 0) ? (int)(duration100ns / 10000000LL) : 0;
        LoadLyricsForTrack(newTitle, newArtist, durationSecondsForLyrics);

        bool canSeek = false;
        try {
            auto controls = info.Controls();
            canSeek = controls && controls.IsPlaybackPositionEnabled();
        } catch (...) {
            canSeek = false;
        }

        lock_guard<mutex> guard(g_MediaState.lock);
        if (shouldReloadAlbumArt) {
            if (albumArtDecoded && newAlbumArt) {
                // Success: update art and lock key.
                g_MediaState.albumArt = newAlbumArt;
                g_MediaState.albumArtCacheKey = newAlbumArtKey;
                DebugLog(LOG_TRACE, L"Album art updated for track=%ls",
                         TruncateForLog(newAlbumArtKey, 64).c_str());
            } else if (!thumbRefAvailable) {
                // Track has no thumbnail: clear art and lock key to avoid useless retries.
                g_MediaState.albumArt.reset();
                g_MediaState.albumArtCacheKey = newAlbumArtKey;
                DebugLog(LOG_TRACE, L"Album art missing thumbnail track=%ls",
                         TruncateForLog(newAlbumArtKey, 64).c_str());
            } else {
                // Temporary decode/open failure: keep previous key so we retry next refresh.
                DebugLog(LOG_VERBOSE, L"Album art decode failed (will retry) track=%ls",
                         TruncateForLog(newAlbumArtKey, 64).c_str());
            }
        }
        g_MediaState.title = newTitle;
        g_MediaState.artist = newArtist;
        g_MediaState.isPlaying =
            (info.PlaybackStatus() ==
             GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing);
        g_MediaState.hasMedia = true;
        g_MediaState.timelinePosition100ns = relativePosition100ns;
        g_MediaState.timelineDuration100ns = duration100ns;
        g_MediaState.canSeek = canSeek;
    } catch (...) {
        DebugLog(LOG_BASIC, L"RefreshSpotifyMediaState exception -> clear media state");
        ClearMediaState();
    }
}

void SendSpotifyMediaCommand(int cmd) {
    try {
        if (!g_SessionManager) {
            g_SessionManager = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
        }
        if (!g_SessionManager) return;

        auto session = GetSpotifySession();
        if (session) {
            if (cmd == 1) session.TrySkipPreviousAsync();
            else if (cmd == 2) session.TryTogglePlayPauseAsync();
            else if (cmd == 3) session.TrySkipNextAsync();
        }
    } catch (...) {}
}

void SeekSpotifyBySeconds(int deltaSeconds) {
    if (deltaSeconds == 0) return;

    try {
        if (!g_SessionManager) {
            g_SessionManager =
                GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
        }
        if (!g_SessionManager) return;

        auto session = GetSpotifySession();
        if (!session) return;

        auto info = session.GetPlaybackInfo();
        auto controls = info.Controls();
        if (!controls || !controls.IsPlaybackPositionEnabled()) return;

        auto timeline = session.GetTimelineProperties();
        int64_t start100ns = timeline.StartTime().count();
        int64_t end100ns = timeline.EndTime().count();
        int64_t position100ns = timeline.Position().count();
        if (end100ns <= start100ns) return;

        int64_t delta100ns = (int64_t)deltaSeconds * 10000000LL;
        int64_t target100ns = position100ns + delta100ns;
        if (target100ns < start100ns) target100ns = start100ns;
        if (target100ns > end100ns) target100ns = end100ns;

        session.TryChangePlaybackPositionAsync(target100ns);

        lock_guard<mutex> guard(g_MediaState.lock);
        int64_t duration100ns = end100ns - start100ns;
        int64_t relativePos = target100ns - start100ns;
        if (relativePos < 0) relativePos = 0;
        if (relativePos > duration100ns) relativePos = duration100ns;
        g_MediaState.timelinePosition100ns = relativePos;
        g_MediaState.timelineDuration100ns = duration100ns;
    } catch (...) {
    }
}

void ResetRuntimeToggles() {
    g_CollapseToDiscOnMouseLeave = false;
    g_RuntimePinAlwaysVisible = false;
    g_RuntimePauseAnimations = false;
    g_RuntimeLyricsEnabled = g_Settings.enableLyrics;
    g_RuntimeKeepControlsVisible = false;
    g_RuntimeLyricsOffsetMs = 0;
    g_RuntimeThemeMode = 0;
    ResetLyricsState();
    ResetLyricsAnimationState();

    Wh_SetIntValue(RUNTIME_COLLAPSE_TO_DISC_VALUE, 0);
    Wh_SetIntValue(RUNTIME_PIN_ALWAYS_VISIBLE_VALUE, 0);
    Wh_SetIntValue(RUNTIME_PAUSE_ANIMATIONS_VALUE, 0);
    Wh_SetIntValue(RUNTIME_LYRICS_ENABLED_VALUE, -1);
    Wh_SetIntValue(RUNTIME_KEEP_CONTROLS_VISIBLE_VALUE, 0);
    Wh_SetIntValue(RUNTIME_LYRICS_OFFSET_MS_VALUE, 0);
    Wh_SetIntValue(RUNTIME_THEME_MODE_VALUE, 0);
    Wh_SetIntValue(RUNTIME_HIDE_FULLSCREEN_OVERRIDE_VALUE, -1);
    Wh_SetIntValue(RUNTIME_THEME_PRESET_VALUE, -1);
    Wh_SetIntValue(RUNTIME_WIDGET_SHAPE_VALUE, -1);
    Wh_SetIntValue(RUNTIME_RING_STYLE_VALUE, -1);
    Wh_SetIntValue(RUNTIME_CONTROL_STYLE_VALUE, -1);
}

void SaveWidgetPosition() {
    Wh_SetIntValue(L"SavedOffsetX", g_Settings.offsetX);
    Wh_SetIntValue(L"SavedOffsetY", g_Settings.offsetY);
    Wh_SetIntValue(L"HasSavedPosition", 1);
}

void ClearSavedWidgetPosition() {
    Wh_SetIntValue(L"HasSavedPosition", 0);
}

void ApplyWidgetAlpha(HWND hwnd, BYTE alpha) {
    g_CurrentAlpha = alpha;
    SetLayeredWindowAttributes(hwnd, TRANSPARENT_COLORKEY, g_CurrentAlpha,
                               LWA_ALPHA | LWA_COLORKEY);
}

void RequestWidgetVisibility(HWND hwnd, bool visible) {
    if (!hwnd) return;

    if (visible) {
        bool alreadyVisible = IsWindowVisible(hwnd) && g_TargetAlpha == 255 &&
                              (!g_FadeTimerRunning || g_CurrentAlpha == 255);
        if (alreadyVisible) return;

        if (!IsWindowVisible(hwnd)) {
            ShowWindow(hwnd, SW_SHOWNOACTIVATE);
            ApplyWidgetAlpha(hwnd, 0);
        }

        g_TargetAlpha = 255;
        g_FadePendingHide = false;
    } else {
        bool alreadyHidden = (!IsWindowVisible(hwnd) && g_CurrentAlpha == 0) ||
                             (g_TargetAlpha == 0 && g_FadeTimerRunning);
        if (alreadyHidden) return;

        g_TargetAlpha = 0;
        g_FadePendingHide = true;
    }

    if (g_Settings.fadeDurationMs <= 0) {
        ApplyWidgetAlpha(hwnd, g_TargetAlpha);
        if (g_TargetAlpha == 0 && IsWindowVisible(hwnd)) {
            ShowWindow(hwnd, SW_HIDE);
        }
        g_FadePendingHide = false;
        g_FadeTimerRunning = false;
        KillTimer(hwnd, TIMER_ID_FADE);
        return;
    }

    g_LastFadeTick = GetTickCount64();
    g_FadeTimerRunning = true;
    SetTimer(hwnd, TIMER_ID_FADE, 16, NULL);
}

void UpdateWidgetFade(HWND hwnd) {
    if (!g_FadeTimerRunning) return;

    ULONGLONG now = GetTickCount64();
    float dt = (g_LastFadeTick == 0) ? (1.0f / 60.0f)
                                     : (float)(now - g_LastFadeTick) / 1000.0f;
    g_LastFadeTick = now;
    if (dt < 0.0f) dt = 1.0f / 60.0f;
    if (dt > 0.05f) dt = 0.05f;

    float speedPerSec = 255.0f / ((float)g_Settings.fadeDurationMs / 1000.0f);
    float step = speedPerSec * dt;
    if (step < kFadeMinStep) step = kFadeMinStep;

    int nextAlpha = (int)g_CurrentAlpha;
    if (g_CurrentAlpha < g_TargetAlpha) {
        nextAlpha = min((int)g_TargetAlpha, (int)(g_CurrentAlpha + step));
    } else if (g_CurrentAlpha > g_TargetAlpha) {
        nextAlpha = max((int)g_TargetAlpha, (int)(g_CurrentAlpha - step));
    }

    ApplyWidgetAlpha(hwnd, (BYTE)nextAlpha);

    if (g_CurrentAlpha == g_TargetAlpha) {
        g_FadeTimerRunning = false;
        g_LastFadeTick = 0;
        KillTimer(hwnd, TIMER_ID_FADE);

        if (g_TargetAlpha == 0 && g_FadePendingHide && IsWindowVisible(hwnd)) {
            ShowWindow(hwnd, SW_HIDE);
        }
        g_FadePendingHide = false;
    }
}

enum ContextMenuCommandId {
    IDM_CTX_PREV = 30001,
    IDM_CTX_PLAY_PAUSE = 30002,
    IDM_CTX_NEXT = 30003,
    IDM_CTX_TOGGLE_FULLSCREEN = 30010,
    IDM_CTX_RESET_POSITION = 30011,
    IDM_CTX_REFRESH_NOW = 30012,
    IDM_CTX_CLEAR_FULLSCREEN_OVERRIDE = 30013,
    IDM_CTX_TOGGLE_COLLAPSE_TO_DISC = 30014,
    IDM_CTX_PIN_ALWAYS_VISIBLE = 30020,
    IDM_CTX_PAUSE_ANIMATIONS = 30021,
    IDM_CTX_RESET_RUNTIME_TOGGLES = 30022,
    IDM_CTX_THEME_AUTO = 30023,
    IDM_CTX_THEME_LIGHT = 30024,
    IDM_CTX_THEME_DARK = 30025,
    IDM_CTX_TOGGLE_LYRICS = 30026,
    IDM_CTX_KEEP_CONTROLS_VISIBLE = 30027,
    IDM_CTX_LYRICS_DELAY_1000 = 30070,
    IDM_CTX_LYRICS_DELAY_2000 = 30071,
    IDM_CTX_LYRICS_ADVANCE_1000 = 30072,
    IDM_CTX_LYRICS_ADVANCE_2000 = 30073,
    IDM_CTX_LYRICS_CALIBRATION_RESET = 30074,
    IDM_CTX_THEME_PRESET_GLASS = 30030,
    IDM_CTX_THEME_PRESET_MINIMAL = 30031,
    IDM_CTX_THEME_PRESET_NEON = 30032,
    IDM_CTX_THEME_PRESET_RETRO = 30033,
    IDM_CTX_THEME_PRESET_CONTRAST = 30034,
    IDM_CTX_THEME_PRESET_LEGACY = 30035,
    IDM_CTX_SHAPE_PILL = 30040,
    IDM_CTX_SHAPE_CAPSULE = 30041,
    IDM_CTX_SHAPE_COMPACT = 30042,
    IDM_CTX_RING_SINGLE = 30050,
    IDM_CTX_RING_DOUBLE = 30051,
    IDM_CTX_RING_DOTTED = 30052,
    IDM_CTX_RING_SEGMENTED = 30053,
    IDM_CTX_CONTROL_CLASSIC = 30060,
    IDM_CTX_CONTROL_OUTLINE = 30061,
    IDM_CTX_CONTROL_SOFT = 30062,
};

void ShowMediaWidgetContextMenu(HWND hwnd, POINT screenPt) {
    HMENU hMenu = CreatePopupMenu();
    if (!hMenu) return;

    AppendMenuW(hMenu, MF_STRING, IDM_CTX_PREV, L"Previous");
    AppendMenuW(hMenu, MF_STRING, IDM_CTX_PLAY_PAUSE, L"Play / Pause");
    AppendMenuW(hMenu, MF_STRING, IDM_CTX_NEXT, L"Next");
    AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);

    AppendMenuW(hMenu,
                MF_STRING | (g_CollapseToDiscOnMouseLeave ? MF_CHECKED : 0),
                IDM_CTX_TOGGLE_COLLAPSE_TO_DISC,
                L"Collapse To Disc On Mouse Leave");
    AppendMenuW(hMenu,
                MF_STRING | (g_RuntimePinAlwaysVisible ? MF_CHECKED : 0),
                IDM_CTX_PIN_ALWAYS_VISIBLE,
                L"Pin Always Visible");
    AppendMenuW(hMenu,
                MF_STRING | (g_RuntimePauseAnimations ? MF_CHECKED : 0),
                IDM_CTX_PAUSE_ANIMATIONS,
                L"Pause Animations");
    AppendMenuW(hMenu,
                MF_STRING | (g_RuntimeKeepControlsVisible ? MF_CHECKED : 0),
                IDM_CTX_KEEP_CONTROLS_VISIBLE,
                L"Keep Controls Visible");
    AppendMenuW(hMenu,
                MF_STRING | (g_RuntimeLyricsEnabled ? MF_CHECKED : 0),
                IDM_CTX_TOGGLE_LYRICS,
                L"Enable Lyrics");
    HMENU hLyricsCalibrationMenu = CreatePopupMenu();
    if (hLyricsCalibrationMenu) {
        WCHAR offsetLabel[64];
        swprintf_s(offsetLabel, L"Current Offset: %+0.1fs",
                   (double)g_RuntimeLyricsOffsetMs / 1000.0);
        AppendMenuW(hLyricsCalibrationMenu, MF_STRING | MF_DISABLED, 0,
                    offsetLabel);
        AppendMenuW(hLyricsCalibrationMenu, MF_SEPARATOR, 0, nullptr);
        AppendMenuW(hLyricsCalibrationMenu, MF_STRING,
                    IDM_CTX_LYRICS_DELAY_1000, L"Delay Lyrics +1s");
        AppendMenuW(hLyricsCalibrationMenu, MF_STRING,
                    IDM_CTX_LYRICS_DELAY_2000, L"Delay Lyrics +2s");
        AppendMenuW(hLyricsCalibrationMenu, MF_STRING,
                    IDM_CTX_LYRICS_ADVANCE_1000, L"Advance Lyrics -1s");
        AppendMenuW(hLyricsCalibrationMenu, MF_STRING,
                    IDM_CTX_LYRICS_ADVANCE_2000, L"Advance Lyrics -2s");
        AppendMenuW(hLyricsCalibrationMenu, MF_SEPARATOR, 0, nullptr);
        AppendMenuW(hLyricsCalibrationMenu,
                    MF_STRING |
                        (g_RuntimeLyricsOffsetMs == 0 ? MF_CHECKED : 0),
                    IDM_CTX_LYRICS_CALIBRATION_RESET,
                    L"Reset Lyrics Offset");
        AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hLyricsCalibrationMenu,
                    L"Lyrics Calibration");
    }

    HMENU hThemeModeMenu = CreatePopupMenu();
    if (hThemeModeMenu) {
        AppendMenuW(hThemeModeMenu,
                    MF_STRING | ((g_RuntimeThemeMode == 0) ? MF_CHECKED : 0),
                    IDM_CTX_THEME_AUTO, L"Auto");
        AppendMenuW(hThemeModeMenu,
                    MF_STRING | ((g_RuntimeThemeMode == 1) ? MF_CHECKED : 0),
                    IDM_CTX_THEME_LIGHT, L"Light");
        AppendMenuW(hThemeModeMenu,
                    MF_STRING | ((g_RuntimeThemeMode == 2) ? MF_CHECKED : 0),
                    IDM_CTX_THEME_DARK, L"Dark");
        AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hThemeModeMenu, L"Quick Theme");
    }

    HMENU hThemePresetMenu = CreatePopupMenu();
    if (hThemePresetMenu) {
        AppendMenuW(hThemePresetMenu,
                    MF_STRING | ((g_Settings.themePreset == 0) ? MF_CHECKED : 0),
                    IDM_CTX_THEME_PRESET_GLASS, L"Glass Clean");
        AppendMenuW(hThemePresetMenu,
                    MF_STRING | ((g_Settings.themePreset == 1) ? MF_CHECKED : 0),
                    IDM_CTX_THEME_PRESET_MINIMAL, L"Minimal Mono");
        AppendMenuW(hThemePresetMenu,
                    MF_STRING | ((g_Settings.themePreset == 2) ? MF_CHECKED : 0),
                    IDM_CTX_THEME_PRESET_NEON, L"Neon Accent");
        AppendMenuW(hThemePresetMenu,
                    MF_STRING | ((g_Settings.themePreset == 3) ? MF_CHECKED : 0),
                    IDM_CTX_THEME_PRESET_RETRO, L"Retro Vinyl");
        AppendMenuW(hThemePresetMenu,
                    MF_STRING | ((g_Settings.themePreset == 4) ? MF_CHECKED : 0),
                    IDM_CTX_THEME_PRESET_CONTRAST, L"High Contrast");
        AppendMenuW(hThemePresetMenu,
                    MF_STRING | ((g_Settings.themePreset == 5) ? MF_CHECKED : 0),
                    IDM_CTX_THEME_PRESET_LEGACY, L"Legacy Classic");
        AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hThemePresetMenu,
                    L"Theme Preset");
    }

    HMENU hShapeMenu = CreatePopupMenu();
    if (hShapeMenu) {
        AppendMenuW(hShapeMenu,
                    MF_STRING | ((g_Settings.widgetShape == 0) ? MF_CHECKED : 0),
                    IDM_CTX_SHAPE_PILL, L"Pill");
        AppendMenuW(hShapeMenu,
                    MF_STRING | ((g_Settings.widgetShape == 1) ? MF_CHECKED : 0),
                    IDM_CTX_SHAPE_CAPSULE, L"Capsule");
        AppendMenuW(hShapeMenu,
                    MF_STRING | ((g_Settings.widgetShape == 2) ? MF_CHECKED : 0),
                    IDM_CTX_SHAPE_COMPACT, L"Compact");
        AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hShapeMenu, L"Widget Shape");
    }

    HMENU hRingMenu = CreatePopupMenu();
    if (hRingMenu) {
        AppendMenuW(hRingMenu,
                    MF_STRING | ((g_Settings.ringStyle == 0) ? MF_CHECKED : 0),
                    IDM_CTX_RING_SINGLE, L"Single Ring");
        AppendMenuW(hRingMenu,
                    MF_STRING | ((g_Settings.ringStyle == 1) ? MF_CHECKED : 0),
                    IDM_CTX_RING_DOUBLE, L"Double Ring");
        AppendMenuW(hRingMenu,
                    MF_STRING | ((g_Settings.ringStyle == 2) ? MF_CHECKED : 0),
                    IDM_CTX_RING_DOTTED, L"Dotted Ring");
        AppendMenuW(hRingMenu,
                    MF_STRING | ((g_Settings.ringStyle == 3) ? MF_CHECKED : 0),
                    IDM_CTX_RING_SEGMENTED, L"Segmented Ring");
        AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hRingMenu, L"Ring Style");
    }

    HMENU hControlMenu = CreatePopupMenu();
    if (hControlMenu) {
        AppendMenuW(hControlMenu,
                    MF_STRING | ((g_Settings.controlStyle == 0) ? MF_CHECKED : 0),
                    IDM_CTX_CONTROL_CLASSIC, L"Classic Controls");
        AppendMenuW(hControlMenu,
                    MF_STRING | ((g_Settings.controlStyle == 1) ? MF_CHECKED : 0),
                    IDM_CTX_CONTROL_OUTLINE, L"Outline Controls");
        AppendMenuW(hControlMenu,
                    MF_STRING | ((g_Settings.controlStyle == 2) ? MF_CHECKED : 0),
                    IDM_CTX_CONTROL_SOFT, L"Soft Controls");
        AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hControlMenu, L"Control Style");
    }

    AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hMenu, MF_STRING, IDM_CTX_TOGGLE_FULLSCREEN,
                g_Settings.hideFullscreen ? L"Disable Fullscreen Hide"
                                          : L"Enable Fullscreen Hide");
    if (Wh_GetIntValue(RUNTIME_HIDE_FULLSCREEN_OVERRIDE_VALUE, -1) != -1) {
        AppendMenuW(hMenu, MF_STRING, IDM_CTX_CLEAR_FULLSCREEN_OVERRIDE,
                    L"Use Setting Value for Fullscreen Hide");
    }

    AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hMenu, MF_STRING, IDM_CTX_RESET_RUNTIME_TOGGLES,
                L"Reset All Runtime Toggles");
    AppendMenuW(hMenu, MF_STRING, IDM_CTX_RESET_POSITION, L"Reset Position");
    AppendMenuW(hMenu, MF_STRING, IDM_CTX_REFRESH_NOW, L"Refresh");

    SetForegroundWindow(hwnd);
    UINT cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_RIGHTBUTTON,
                              screenPt.x, screenPt.y, 0, hwnd, nullptr);
    DestroyMenu(hMenu);

    switch (cmd) {
        case IDM_CTX_PREV:
            SendSpotifyMediaCommand(1);
            break;
        case IDM_CTX_PLAY_PAUSE:
            SendSpotifyMediaCommand(2);
            break;
        case IDM_CTX_NEXT:
            SendSpotifyMediaCommand(3);
            break;
        case IDM_CTX_TOGGLE_FULLSCREEN:
            g_Settings.hideFullscreen = !g_Settings.hideFullscreen;
            Wh_SetIntValue(RUNTIME_HIDE_FULLSCREEN_OVERRIDE_VALUE,
                           g_Settings.hideFullscreen ? 1 : 0);
            RefreshMediaStateAndVisibility(hwnd);
            break;
        case IDM_CTX_TOGGLE_COLLAPSE_TO_DISC:
            g_CollapseToDiscOnMouseLeave = !g_CollapseToDiscOnMouseLeave;
            Wh_SetIntValue(RUNTIME_COLLAPSE_TO_DISC_VALUE,
                           g_CollapseToDiscOnMouseLeave ? 1 : 0);
            g_HoverState = 0;
            ResetCollapseDelayState();
            SyncCollapseAnimationState(hwnd);
            InvalidateRect(hwnd, NULL, FALSE);
            break;
        case IDM_CTX_PIN_ALWAYS_VISIBLE:
            g_RuntimePinAlwaysVisible = !g_RuntimePinAlwaysVisible;
            Wh_SetIntValue(RUNTIME_PIN_ALWAYS_VISIBLE_VALUE,
                           g_RuntimePinAlwaysVisible ? 1 : 0);
            RefreshMediaStateAndVisibility(hwnd);
            break;
        case IDM_CTX_PAUSE_ANIMATIONS:
            g_RuntimePauseAnimations = !g_RuntimePauseAnimations;
            Wh_SetIntValue(RUNTIME_PAUSE_ANIMATIONS_VALUE,
                           g_RuntimePauseAnimations ? 1 : 0);
            DebugLog(LOG_BASIC, L"Menu: Pause Animations -> %d",
                     g_RuntimePauseAnimations ? 1 : 0);
            if (!g_RuntimePauseAnimations) {
                EnsureAnimationTimer(hwnd);
            } else if (!g_CollapseDelayPending) {
                g_LastAnimationTick = 0;
                g_AnimationTimerRunning = false;
                KillTimer(hwnd, TIMER_ID_ANIMATION);
            }
            InvalidateRect(hwnd, NULL, FALSE);
            break;
        case IDM_CTX_KEEP_CONTROLS_VISIBLE:
            g_RuntimeKeepControlsVisible = !g_RuntimeKeepControlsVisible;
            Wh_SetIntValue(RUNTIME_KEEP_CONTROLS_VISIBLE_VALUE,
                           g_RuntimeKeepControlsVisible ? 1 : 0);
            DebugLog(LOG_BASIC, L"Menu: Keep Controls Visible -> %d",
                     g_RuntimeKeepControlsVisible ? 1 : 0);
            g_HoverState = 0;
            if (!g_RuntimeKeepControlsVisible && g_CollapseToDiscOnMouseLeave) {
                POINT cursor{};
                if (GetCursorPos(&cursor)) {
                    RECT rc{};
                    if (GetWindowRect(hwnd, &rc)) {
                        g_IsMouseInsideWidget = PtInRect(&rc, cursor) != FALSE;
                    }
                }
                if (g_IsMouseInsideWidget) {
                    g_CollapseTarget = 0.0f;
                    ResetCollapseDelayState();
                } else if (g_Settings.collapseDelayMs <= 0) {
                    g_CollapseTarget = 1.0f;
                    g_CollapseDelayPending = false;
                    g_CollapseDelayUntilTick = 0;
                } else {
                    g_CollapseDelayPending = true;
                    g_CollapseDelayUntilTick =
                        GetTickCount64() + (ULONGLONG)g_Settings.collapseDelayMs;
                    g_CollapseTarget = 0.0f;
                }
                EnsureAnimationTimer(hwnd);
            } else {
                SyncCollapseAnimationState(hwnd);
            }
            InvalidateRect(hwnd, NULL, FALSE);
            break;
        case IDM_CTX_TOGGLE_LYRICS:
            g_RuntimeLyricsEnabled = !g_RuntimeLyricsEnabled;
            Wh_SetIntValue(RUNTIME_LYRICS_ENABLED_VALUE,
                           g_RuntimeLyricsEnabled ? 1 : 0);
            DebugLog(LOG_BASIC, L"Menu: Enable Lyrics -> %d",
                     g_RuntimeLyricsEnabled ? 1 : 0);
            {
                lock_guard<mutex> guard(g_LyricsState.lock);
                g_LyricsState.trackKey.clear();
                g_LyricsState.sourcePath.clear();
                g_LyricsState.lines.clear();
                g_LyricsState.lastLoadAttemptTick = 0;
                g_LyricsState.onlineFetchInProgress = false;
            }
            ResetLyricsAnimationState();
            if (g_RuntimeLyricsEnabled) {
                if (g_Settings.width < kLyricsPreferredWidth) {
                    g_Settings.width = kLyricsPreferredWidth;
                    PostMessage(hwnd, WM_APP + 10, 0, 0);
                }

                wstring currentTitle;
                wstring currentArtist;
                int currentDurationSeconds = 0;
                {
                    lock_guard<mutex> guard(g_MediaState.lock);
                    if (g_MediaState.hasMedia) {
                        currentTitle = g_MediaState.title;
                        currentArtist = g_MediaState.artist;
                        if (g_MediaState.timelineDuration100ns > 0) {
                            currentDurationSeconds = (int)(
                                g_MediaState.timelineDuration100ns / 10000000LL);
                        }
                    }
                }

                if (!currentTitle.empty()) {
                    LoadLyricsForTrack(currentTitle, currentArtist,
                                       currentDurationSeconds);
                }
                StartLyricsBootstrapIfNeeded();
            }
            RefreshMediaStateAndVisibility(hwnd);
            InvalidateRect(hwnd, NULL, FALSE);
            break;
        case IDM_CTX_LYRICS_DELAY_1000:
        case IDM_CTX_LYRICS_DELAY_2000:
        case IDM_CTX_LYRICS_ADVANCE_1000:
        case IDM_CTX_LYRICS_ADVANCE_2000:
        case IDM_CTX_LYRICS_CALIBRATION_RESET: {
            if (cmd == IDM_CTX_LYRICS_CALIBRATION_RESET) {
                g_RuntimeLyricsOffsetMs = 0;
            } else {
                int deltaMs = 0;
                if (cmd == IDM_CTX_LYRICS_DELAY_1000) deltaMs = kLyricsOffsetStepMs;
                else if (cmd == IDM_CTX_LYRICS_DELAY_2000) deltaMs = 2 * kLyricsOffsetStepMs;
                else if (cmd == IDM_CTX_LYRICS_ADVANCE_1000) deltaMs = -kLyricsOffsetStepMs;
                else if (cmd == IDM_CTX_LYRICS_ADVANCE_2000) deltaMs = -2 * kLyricsOffsetStepMs;

                g_RuntimeLyricsOffsetMs += deltaMs;
                if (g_RuntimeLyricsOffsetMs < -kLyricsOffsetMaxMs) {
                    g_RuntimeLyricsOffsetMs = -kLyricsOffsetMaxMs;
                }
                if (g_RuntimeLyricsOffsetMs > kLyricsOffsetMaxMs) {
                    g_RuntimeLyricsOffsetMs = kLyricsOffsetMaxMs;
                }
            }

            Wh_SetIntValue(RUNTIME_LYRICS_OFFSET_MS_VALUE, g_RuntimeLyricsOffsetMs);
            DebugLog(LOG_BASIC, L"Menu: Lyrics calibration offset=%dms",
                     g_RuntimeLyricsOffsetMs);
            InvalidateRect(hwnd, NULL, FALSE);
            break;
        }
        case IDM_CTX_THEME_AUTO:
            g_RuntimeThemeMode = 0;
            Wh_SetIntValue(RUNTIME_THEME_MODE_VALUE, g_RuntimeThemeMode);
            UpdateAppearance(hwnd);
            InvalidateRect(hwnd, NULL, FALSE);
            break;
        case IDM_CTX_THEME_LIGHT:
            g_RuntimeThemeMode = 1;
            Wh_SetIntValue(RUNTIME_THEME_MODE_VALUE, g_RuntimeThemeMode);
            UpdateAppearance(hwnd);
            InvalidateRect(hwnd, NULL, FALSE);
            break;
        case IDM_CTX_THEME_DARK:
            g_RuntimeThemeMode = 2;
            Wh_SetIntValue(RUNTIME_THEME_MODE_VALUE, g_RuntimeThemeMode);
            UpdateAppearance(hwnd);
            InvalidateRect(hwnd, NULL, FALSE);
            break;
        case IDM_CTX_THEME_PRESET_GLASS:
        case IDM_CTX_THEME_PRESET_MINIMAL:
        case IDM_CTX_THEME_PRESET_NEON:
        case IDM_CTX_THEME_PRESET_RETRO:
        case IDM_CTX_THEME_PRESET_CONTRAST:
        case IDM_CTX_THEME_PRESET_LEGACY: {
            int preset = 0;
            if (cmd == IDM_CTX_THEME_PRESET_MINIMAL) preset = 1;
            else if (cmd == IDM_CTX_THEME_PRESET_NEON) preset = 2;
            else if (cmd == IDM_CTX_THEME_PRESET_RETRO) preset = 3;
            else if (cmd == IDM_CTX_THEME_PRESET_CONTRAST) preset = 4;
            else if (cmd == IDM_CTX_THEME_PRESET_LEGACY) preset = 5;
            g_Settings.themePreset = preset;
            Wh_SetIntValue(RUNTIME_THEME_PRESET_VALUE, preset);

            if (preset == 5) {
                g_Settings.widgetShape = 1;
                g_Settings.ringStyle = 2;
                g_Settings.controlStyle = 0;
                Wh_SetIntValue(RUNTIME_WIDGET_SHAPE_VALUE, g_Settings.widgetShape);
                Wh_SetIntValue(RUNTIME_RING_STYLE_VALUE, g_Settings.ringStyle);
                Wh_SetIntValue(RUNTIME_CONTROL_STYLE_VALUE, g_Settings.controlStyle);
            }

            UpdateAppearance(hwnd);
            InvalidateRect(hwnd, NULL, FALSE);
            break;
        }
        case IDM_CTX_SHAPE_PILL:
        case IDM_CTX_SHAPE_CAPSULE:
        case IDM_CTX_SHAPE_COMPACT: {
            int shape = (cmd == IDM_CTX_SHAPE_CAPSULE) ? 1 :
                        (cmd == IDM_CTX_SHAPE_COMPACT) ? 2 : 0;
            g_Settings.widgetShape = shape;
            Wh_SetIntValue(RUNTIME_WIDGET_SHAPE_VALUE, shape);
            UpdateAppearance(hwnd);
            InvalidateRect(hwnd, NULL, FALSE);
            break;
        }
        case IDM_CTX_RING_SINGLE:
        case IDM_CTX_RING_DOUBLE:
        case IDM_CTX_RING_DOTTED:
        case IDM_CTX_RING_SEGMENTED: {
            int ringStyle = 0;
            if (cmd == IDM_CTX_RING_DOUBLE) ringStyle = 1;
            else if (cmd == IDM_CTX_RING_DOTTED) ringStyle = 2;
            else if (cmd == IDM_CTX_RING_SEGMENTED) ringStyle = 3;
            g_Settings.ringStyle = ringStyle;
            Wh_SetIntValue(RUNTIME_RING_STYLE_VALUE, ringStyle);
            UpdateAppearance(hwnd);
            InvalidateRect(hwnd, NULL, FALSE);
            break;
        }
        case IDM_CTX_CONTROL_CLASSIC:
        case IDM_CTX_CONTROL_OUTLINE:
        case IDM_CTX_CONTROL_SOFT: {
            int controlStyle = (cmd == IDM_CTX_CONTROL_OUTLINE) ? 1 :
                               (cmd == IDM_CTX_CONTROL_SOFT) ? 2 : 0;
            g_Settings.controlStyle = controlStyle;
            Wh_SetIntValue(RUNTIME_CONTROL_STYLE_VALUE, controlStyle);
            UpdateAppearance(hwnd);
            InvalidateRect(hwnd, NULL, FALSE);
            break;
        }
        case IDM_CTX_RESET_RUNTIME_TOGGLES:
            ResetRuntimeToggles();
            LoadSettings();
            DebugLog(LOG_BASIC, L"Menu: Reset all runtime toggles");
            g_HoverState = 0;
            ResetCollapseDelayState();
            SyncCollapseAnimationState(hwnd);
            UpdateAppearance(hwnd);
            PostMessage(hwnd, WM_APP + 10, 0, 0);
            RefreshMediaStateAndVisibility(hwnd);
            InvalidateRect(hwnd, NULL, FALSE);
            break;
        case IDM_CTX_CLEAR_FULLSCREEN_OVERRIDE:
            Wh_SetIntValue(RUNTIME_HIDE_FULLSCREEN_OVERRIDE_VALUE, -1);
            g_Settings.hideFullscreen = Wh_GetIntSetting(L"HideFullscreen") != 0;
            RefreshMediaStateAndVisibility(hwnd);
            break;
        case IDM_CTX_RESET_POSITION:
            g_Settings.offsetX = g_DefaultOffsetX;
            g_Settings.offsetY = g_DefaultOffsetY;
            ClearSavedWidgetPosition();
            PostMessage(hwnd, WM_APP + 10, 0, 0);
            break;
        case IDM_CTX_REFRESH_NOW:
            RefreshMediaStateAndVisibility(hwnd);
            break;
        default:
            break;
    }
}

// --- Rendering ---
bool IsSystemLightMode() {
    DWORD value = 0; DWORD size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", L"SystemUsesLightTheme", RRF_RT_DWORD, nullptr, &value, &size) == ERROR_SUCCESS) {
        return value != 0;
    }
    return false;
}

DWORD GetCurrentTextColor() {
    if (g_RuntimeThemeMode == 1) return 0xFF101010;
    if (g_RuntimeThemeMode == 2) return 0xFFF2F2F2;

    if (g_Settings.autoTheme) return IsSystemLightMode() ? 0xFF000000 : 0xFFFFFFFF;
    return g_Settings.manualTextColor;
}

int GetResolvedThemeMode() {
    if (g_RuntimeThemeMode == 1) return 1;
    if (g_RuntimeThemeMode == 2) return 2;

    if (g_Settings.autoTheme) {
        return IsSystemLightMode() ? 1 : 2;
    }

    DWORD textColor = g_Settings.manualTextColor & 0x00FFFFFF;
    BYTE r = (BYTE)((textColor >> 16) & 0xFF);
    BYTE g = (BYTE)((textColor >> 8) & 0xFF);
    BYTE b = (BYTE)(textColor & 0xFF);
    int luminance = (int)r * 3 + (int)g * 6 + (int)b;
    return (luminance >= (128 * 10)) ? 1 : 2;
}

float GetAnimationSpeedMultiplier() {
    if (g_Settings.animationProfile == 1) return 1.45f; // Snappy
    if (g_Settings.animationProfile == 2) return 0.72f; // Calm
    return 1.0f; // Smooth
}

float GetScrollSpeedMultiplier() {
    if (g_Settings.animationProfile == 1) return 1.30f;
    if (g_Settings.animationProfile == 2) return 0.78f;
    return 1.0f;
}

float GetDiscSmoothingMultiplier() {
    if (g_Settings.animationProfile == 1) return 1.18f;
    if (g_Settings.animationProfile == 2) return 0.82f;
    return 1.0f;
}

float ApplyCollapseEasing(float t) {
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    if (g_Settings.collapseEasing == 0) {
        return t; // Linear
    }

    if (g_Settings.collapseEasing == 1) {
        float inv = 1.0f - t;
        return 1.0f - (inv * inv * inv); // Ease-out cubic
    }

    return t * t * (3.0f - (2.0f * t)); // Smoothstep
}

float CalculateCollapseVisibleFactor(float collapseEase) {
    float fadeStrength = (float)g_Settings.collapseFadeStrength;
    if (fadeStrength < 0.0f) fadeStrength = 0.0f;
    if (fadeStrength > 1.0f) fadeStrength = 1.0f;

    float visible = 1.0f - (collapseEase * fadeStrength);
    if (visible < 0.0f) visible = 0.0f;
    if (visible > 1.0f) visible = 1.0f;
    return visible;
}

wstring FormatTimelineTime(int64_t ticks100ns) {
    if (ticks100ns < 0) ticks100ns = 0;
    int totalSeconds = (int)(ticks100ns / 10000000LL);
    int hours = totalSeconds / 3600;
    int minutes = (totalSeconds / 60) % 60;
    int seconds = totalSeconds % 60;

    wchar_t buffer[32] = {};
    if (hours > 0) {
        swprintf_s(buffer, L"%d:%02d:%02d", hours, minutes, seconds);
    } else {
        swprintf_s(buffer, L"%d:%02d", minutes, seconds);
    }
    return buffer;
}

struct ThemeTokens {
    Color background = Color(120, 20, 20, 24);
    Color border = Color(140, 255, 255, 255);
    Color text = Color(255, 255, 255, 255);
    Color textShadow = Color(180, 0, 0, 0);
    Color control = Color(230, 255, 255, 255);
    Color controlHover = Color(255, 255, 255, 255);
    Color controlActiveBg = Color(62, 255, 255, 255);
    Color ringBg = Color(130, 255, 255, 255);
    Color ringFg = Color(240, 80, 190, 255);
    Color timeText = Color(220, 255, 255, 255);
    float cornerRadius = 14.0f;
    float panelInsetX = 1.0f;
    float panelInsetY = 1.0f;
    float ringThicknessBg = 2.0f;
    float ringThicknessFg = 2.6f;
};

void BuildRoundedRectPath(GraphicsPath* path, const RectF& rect, float radius) {
    if (!path) return;
    path->Reset();

    float maxRadius = min(rect.Width, rect.Height) / 2.0f;
    if (radius < 0.0f) radius = 0.0f;
    if (radius > maxRadius) radius = maxRadius;

    if (radius <= 0.001f) {
        path->AddRectangle(rect);
        return;
    }

    float d = radius * 2.0f;
    path->AddArc(rect.X, rect.Y, d, d, 180.0f, 90.0f);
    path->AddArc(rect.X + rect.Width - d, rect.Y, d, d, 270.0f, 90.0f);
    path->AddArc(rect.X + rect.Width - d, rect.Y + rect.Height - d, d, d, 0.0f,
                 90.0f);
    path->AddArc(rect.X, rect.Y + rect.Height - d, d, d, 90.0f, 90.0f);
    path->CloseFigure();
}

ThemeTokens BuildThemeTokens(int width, int height, BYTE textAlpha) {
    ThemeTokens tokens;

    int resolvedMode = GetResolvedThemeMode();
    bool isLight = (resolvedMode == 1);

    BYTE accentR = (BYTE)((g_Settings.accentColor >> 16) & 0xFF);
    BYTE accentG = (BYTE)((g_Settings.accentColor >> 8) & 0xFF);
    BYTE accentB = (BYTE)(g_Settings.accentColor & 0xFF);

    DWORD fallbackTextArgb = GetCurrentTextColor();
    BYTE fallbackTextR = (BYTE)((fallbackTextArgb >> 16) & 0xFF);
    BYTE fallbackTextG = (BYTE)((fallbackTextArgb >> 8) & 0xFF);
    BYTE fallbackTextB = (BYTE)(fallbackTextArgb & 0xFF);

    BYTE bgOpacity = (BYTE)g_Settings.backgroundOpacity;

    if (g_Settings.themePreset == 0) {
        if (isLight) {
            tokens.background = Color(bgOpacity, 248, 248, 252);
            tokens.border = Color((BYTE)min(255, (int)bgOpacity + 70), 210, 212, 220);
            tokens.control = Color(textAlpha, 30, 34, 44);
            tokens.controlHover = Color(textAlpha, 20, 24, 34);
            tokens.controlActiveBg = Color(68, accentR, accentG, accentB);
            tokens.ringBg = Color(120, 140, 150, 170);
            tokens.ringFg = Color(245, accentR, accentG, accentB);
            tokens.timeText = Color((BYTE)min(255, (int)textAlpha), 42, 48, 60);
            tokens.textShadow = Color(120, 255, 255, 255);
        } else {
            tokens.background = Color(bgOpacity, 18, 20, 28);
            tokens.border = Color((BYTE)min(255, (int)bgOpacity + 50), 70, 78, 96);
            tokens.control = Color(textAlpha, 238, 240, 248);
            tokens.controlHover = Color(255, 255, 255, 255);
            tokens.controlActiveBg = Color(70, accentR, accentG, accentB);
            tokens.ringBg = Color(120, 100, 112, 140);
            tokens.ringFg = Color(245, accentR, accentG, accentB);
            tokens.timeText = Color((BYTE)min(255, (int)textAlpha), 220, 226, 238);
            tokens.textShadow = Color(180, 0, 0, 0);
        }
    } else if (g_Settings.themePreset == 1) {
        if (isLight) {
            tokens.background = Color(bgOpacity, 250, 250, 250);
            tokens.border = Color((BYTE)min(255, (int)bgOpacity + 55), 198, 198, 198);
            tokens.control = Color(textAlpha, 24, 24, 24);
            tokens.controlHover = Color(textAlpha, 8, 8, 8);
            tokens.controlActiveBg = Color(62, 60, 60, 60);
            tokens.ringBg = Color(115, 140, 140, 140);
            tokens.ringFg = Color(235, 40, 40, 40);
            tokens.timeText = Color((BYTE)min(255, (int)textAlpha), 24, 24, 24);
            tokens.textShadow = Color(110, 255, 255, 255);
        } else {
            tokens.background = Color(bgOpacity, 18, 18, 18);
            tokens.border = Color((BYTE)min(255, (int)bgOpacity + 45), 92, 92, 92);
            tokens.control = Color(textAlpha, 232, 232, 232);
            tokens.controlHover = Color(255, 255, 255, 255);
            tokens.controlActiveBg = Color(58, 224, 224, 224);
            tokens.ringBg = Color(120, 118, 118, 118);
            tokens.ringFg = Color(235, 235, 235, 235);
            tokens.timeText = Color((BYTE)min(255, (int)textAlpha), 232, 232, 232);
            tokens.textShadow = Color(175, 0, 0, 0);
        }
    } else if (g_Settings.themePreset == 2) {
        tokens.background = Color(bgOpacity, 10, 10, 18);
        tokens.border = Color((BYTE)min(255, (int)bgOpacity + 55), accentR, accentG,
                              accentB);
        tokens.control = Color(textAlpha, 220, 240, 255);
        tokens.controlHover = Color(255, 255, 255, 255);
        tokens.controlActiveBg = Color(84, accentR, accentG, accentB);
        tokens.ringBg = Color(125, 80, 115, 150);
        tokens.ringFg = Color(250, accentR, accentG, accentB);
        tokens.timeText = Color((BYTE)min(255, (int)textAlpha), 210, 236, 255);
        tokens.textShadow = Color(185, 0, 0, 0);
    } else if (g_Settings.themePreset == 3) {
        tokens.background = Color(bgOpacity, 45, 29, 24);
        tokens.border = Color((BYTE)min(255, (int)bgOpacity + 55), 158, 112, 82);
        tokens.control = Color(textAlpha, 255, 228, 200);
        tokens.controlHover = Color(255, 255, 244, 230);
        tokens.controlActiveBg = Color(76, 205, 136, 82);
        tokens.ringBg = Color(120, 114, 82, 66);
        tokens.ringFg = Color(240, 245, 180, 102);
        tokens.timeText = Color((BYTE)min(255, (int)textAlpha), 248, 218, 184);
        tokens.textShadow = Color(185, 20, 10, 8);
    } else if (g_Settings.themePreset == 4) {
        tokens.background = Color((BYTE)max(200, (int)bgOpacity), 0, 0, 0);
        tokens.border = Color(250, 255, 255, 255);
        tokens.control = Color(255, 255, 255, 255);
        tokens.controlHover = Color(255, 255, 255, 0);
        tokens.controlActiveBg = Color(130, 255, 255, 255);
        tokens.ringBg = Color(160, 190, 190, 190);
        tokens.ringFg = Color(255, 255, 255, 0);
        tokens.timeText = Color(245, 255, 255, 255);
        tokens.textShadow = Color(220, 0, 0, 0);
    } else {
        tokens.background = Color((BYTE)max(150, (int)bgOpacity), 132, 132, 136);
        tokens.border = Color((BYTE)min(255, (int)bgOpacity + 85), 220, 220, 224);
        tokens.control = Color(textAlpha, 22, 24, 28);
        tokens.controlHover = Color(255, 8, 10, 12);
        tokens.controlActiveBg = Color(70, 245, 245, 245);
        tokens.ringBg = Color(130, 78, 122, 150);
        tokens.ringFg = Color(245, 102, 210, 245);
        tokens.timeText = Color((BYTE)min(255, (int)textAlpha), 28, 30, 35);
        tokens.textShadow = Color(110, 255, 255, 255);
    }

    if (g_Settings.themePreset == 0 && g_RuntimeThemeMode == 0 && !g_Settings.autoTheme) {
        tokens.text = Color(textAlpha, fallbackTextR, fallbackTextG, fallbackTextB);
    } else if (g_Settings.themePreset == 0 && g_RuntimeThemeMode == 0) {
        if (isLight) {
            tokens.text = Color(textAlpha, 18, 22, 32);
        } else {
            tokens.text = Color(textAlpha, 238, 242, 250);
        }
    } else {
        tokens.text = Color(textAlpha, tokens.control.GetRed(), tokens.control.GetGreen(),
                            tokens.control.GetBlue());
    }

    float h = (float)height;
    float baseRadius = 0.0f;
    if (g_Settings.cornerStyle == 1) {
        baseRadius = 9.0f;
    } else if (g_Settings.cornerStyle == 2) {
        baseRadius = max(8.0f, h * 0.46f);
    } else if (g_Settings.cornerStyle == 3) {
        baseRadius = 14.0f;
    }

    if (g_Settings.widgetShape == 0) {
        baseRadius = max(baseRadius, h * 0.45f);
    } else if (g_Settings.widgetShape == 1) {
        tokens.panelInsetY = 2.0f;
        baseRadius = max(baseRadius, h * 0.33f);
    } else {
        tokens.panelInsetY = 4.0f;
        tokens.panelInsetX = 2.0f;
        baseRadius = min(max(baseRadius, 8.0f), 11.0f);
    }

    if (g_Settings.themePreset == 5) {
        tokens.panelInsetX = 1.0f;
        tokens.panelInsetY = 1.0f;
        baseRadius = max(baseRadius, h * 0.40f);
    }

    float maxRadius = max(0.0f, (h - (tokens.panelInsetY * 2.0f)) * 0.5f - 1.0f);
    if (baseRadius > maxRadius) baseRadius = maxRadius;
    tokens.cornerRadius = baseRadius;

    if (g_Settings.ringStyle == 1) {
        tokens.ringThicknessBg = 1.8f;
        tokens.ringThicknessFg = 2.2f;
    } else if (g_Settings.ringStyle == 2) {
        tokens.ringThicknessBg = 2.0f;
        tokens.ringThicknessFg = 2.4f;
    } else if (g_Settings.ringStyle == 3) {
        tokens.ringThicknessBg = 2.0f;
        tokens.ringThicknessFg = 2.7f;
    }

    if (g_Settings.themePreset == 5) {
        tokens.ringThicknessBg = 1.7f;
        tokens.ringThicknessFg = 2.1f;
    }

    return tokens;
}

void UpdateAppearance(HWND hwnd) {
    DWM_WINDOW_CORNER_PREFERENCE preference = DWMWCP_DONOTROUND;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &preference,
                          sizeof(preference));

    DWMNCRENDERINGPOLICY ncRendering = DWMNCRP_DISABLED;
    DwmSetWindowAttribute(hwnd, DWMWA_NCRENDERING_POLICY, &ncRendering,
                          sizeof(ncRendering));

    COLORREF borderColor = (COLORREF)DWMWA_COLOR_NONE;
    DwmSetWindowAttribute(hwnd, (DWMWINDOWATTRIBUTE)DWMWA_BORDER_COLOR, &borderColor,
                          sizeof(borderColor));

    HMODULE hUser = GetModuleHandle(L"user32.dll");
    if (hUser) {
        auto SetComp = (pSetWindowCompositionAttribute)GetProcAddress(
            hUser, "SetWindowCompositionAttribute");
        if (SetComp) {
            ACCENT_POLICY policy = {};
            // Force true transparency: disable acrylic/blur composition entirely.
            policy.AccentState = ACCENT_DISABLED;
            policy.AccentFlags = 0;
            policy.GradientColor = 0;


            WINDOWCOMPOSITIONATTRIBDATA data = {
                WCA_ACCENT_POLICY,
                &policy,
                sizeof(ACCENT_POLICY),
            };
            SetComp(hwnd, &data);
        }
    }
}

void RenderMediaWidgetFrame(HDC hdc, int width, int height) {
    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);
    graphics.Clear(Color(255, 1, 0, 1));

    float collapseT = g_CollapseToDiscOnMouseLeave ? g_CollapseProgress : 0.0f;
    if (collapseT < 0.0f) collapseT = 0.0f;
    if (collapseT > 1.0f) collapseT = 1.0f;
    float collapseEase = ApplyCollapseEasing(collapseT);
    float visibleFactor = CalculateCollapseVisibleFactor(collapseEase);
    float textCollapseEase = g_RuntimeLyricsEnabled ? 0.0f : collapseEase;
    float textVisibleFactor = g_RuntimeLyricsEnabled ? 1.0f : visibleFactor;

    BYTE textAlpha = (BYTE)g_Settings.textOpacity;
    ThemeTokens theme = BuildThemeTokens(width, height, textAlpha);

    // Transparent panel mode: keep only media content, skip background and border.

    MediaState state;
    {
        lock_guard<mutex> guard(g_MediaState.lock);
        state.title = g_MediaState.title;
        state.artist = g_MediaState.artist;
        state.albumArt = g_MediaState.albumArt;
        state.hasMedia = g_MediaState.hasMedia;
        state.isPlaying = g_MediaState.isPlaying;
        state.timelinePosition100ns = g_MediaState.timelinePosition100ns;
        state.timelineDuration100ns = g_MediaState.timelineDuration100ns;
        state.canSeek = g_MediaState.canSeek;
    }

    int baseArtSize = height - 12;
    int artSize = (int)(baseArtSize * g_Settings.discScale);
    int maxArtSize = max(12, height - 4);
    if (artSize < 12) artSize = 12;
    if (artSize > maxArtSize) artSize = maxArtSize;
    int artX = 6;
    int artY = (height - artSize) / 2;

    float discCenterX = artX + (artSize / 2.0f);
    float discCenterY = artY + (artSize / 2.0f);
    float discSizeF = (float)artSize;
    GraphicsPath discPath;
    discPath.AddEllipse((REAL)artX, (REAL)artY, discSizeF, discSizeF);

    if (state.albumArt) {
        GraphicsState saved = graphics.Save();
        graphics.SetClip(&discPath);
        graphics.TranslateTransform(discCenterX, discCenterY);
        graphics.RotateTransform(g_DiscAngle);
        graphics.TranslateTransform(-discCenterX, -discCenterY);
        graphics.DrawImage(state.albumArt.get(), artX, artY, artSize, artSize);
        graphics.Restore(saved);
    } else {
        SolidBrush placeBrush{Color(70, 120, 120, 128)};
        graphics.FillPath(&placeBrush, &discPath);
    }

    Pen ringPen(theme.border, 1.2f);
    graphics.DrawEllipse(&ringPen, (REAL)artX + 0.6f, (REAL)artY + 0.6f,
                         discSizeF - 1.2f, discSizeF - 1.2f);

    int holeSize = max(5, artSize / 5);
    float holeX = discCenterX - (holeSize / 2.0f);
    float holeY = discCenterY - (holeSize / 2.0f);
    SolidBrush holeBrush(Color(220, 18, 18, 18));
    Pen holeRing(Color(140, 230, 230, 230), 1.0f);
    graphics.FillEllipse(&holeBrush, holeX, holeY, (REAL)holeSize, (REAL)holeSize);
    graphics.DrawEllipse(&holeRing, holeX, holeY, (REAL)holeSize, (REAL)holeSize);

    double scale = g_Settings.buttonScale;
    int startControlX = artX + artSize + (int)(12 * scale);
    int controlY = height / 2;

    float gap = (float)g_Settings.controlGap * (float)scale;
    float minGap = 18.0f * (float)scale;
    if (gap < minGap) gap = minGap;

    float collapseControlTargetX = discCenterX + ((float)holeSize * 0.2f);
    float pBaseX = (float)startControlX;
    float plBaseX = pBaseX + gap;
    float nBaseX = plBaseX + gap;

    float controlCollapseEase = g_RuntimeKeepControlsVisible ? 0.0f : collapseEase;
    float controlVisibleFactor = g_RuntimeKeepControlsVisible ? 1.0f : visibleFactor;

    float pX = pBaseX + (collapseControlTargetX - pBaseX) * controlCollapseEase;
    float plX = plBaseX + (collapseControlTargetX - plBaseX) * controlCollapseEase;
    float nX = nBaseX + (collapseControlTargetX - nBaseX) * controlCollapseEase;

    float controlScale = 1.0f - (0.30f * controlCollapseEase);
    if (controlScale < 0.65f) controlScale = 0.65f;

    float circleR = (12.0f * (float)scale) * controlScale;
    float iconW = (8.0f * (float)scale) * controlScale;
    float iconH = (12.0f * (float)scale) * controlScale;

    BYTE controlsAlpha = (BYTE)(theme.control.GetA() * controlVisibleFactor);
    int hoverState = (controlVisibleFactor > 0.35f) ? g_HoverState : 0;
    bool outlineControls = (g_Settings.controlStyle == 1);
    bool softControls = (g_Settings.controlStyle == 2);

    if (controlsAlpha > 1) {
        Color controlColor(controlsAlpha, theme.control.GetRed(), theme.control.GetGreen(),
                           theme.control.GetBlue());
        BYTE hoverAlpha = (BYTE)min(255, (int)controlsAlpha + 44);
        Color hoverColor(hoverAlpha, theme.controlHover.GetRed(),
                         theme.controlHover.GetGreen(),
                         theme.controlHover.GetBlue());
        BYTE activeBgAlpha = (BYTE)min(255, (int)(theme.controlActiveBg.GetA() * controlVisibleFactor));
        Color activeBgColor(activeBgAlpha, theme.controlActiveBg.GetRed(),
                            theme.controlActiveBg.GetGreen(),
                            theme.controlActiveBg.GetBlue());

        SolidBrush iconBrush{controlColor};
        SolidBrush hoverBrush{hoverColor};
        SolidBrush activeBg{activeBgColor};
        Pen hoverOutline(Color((BYTE)min(255, (int)hoverAlpha + 20),
                               theme.controlHover.GetRed(), theme.controlHover.GetGreen(),
                               theme.controlHover.GetBlue()), 1.2f);

        if (softControls) {
            BYTE softAlpha = (BYTE)min(140, (int)(controlsAlpha * 0.38f));
            SolidBrush softBg(Color(softAlpha, theme.controlActiveBg.GetRed(),
                                    theme.controlActiveBg.GetGreen(),
                                    theme.controlActiveBg.GetBlue()));
            float controlYf = (float)controlY;
            graphics.FillEllipse(&softBg, pX - circleR * 0.9f, controlYf - circleR * 0.9f,
                                 circleR * 1.8f, circleR * 1.8f);
            graphics.FillEllipse(&softBg, plX - circleR * 0.9f, controlYf - circleR * 0.9f,
                                 circleR * 1.8f, circleR * 1.8f);
            graphics.FillEllipse(&softBg, nX - circleR * 0.9f, controlYf - circleR * 0.9f,
                                 circleR * 1.8f, circleR * 1.8f);
        }

        float controlYf = (float)controlY;

        if (hoverState == 1) {
            if (outlineControls) {
                graphics.DrawEllipse(&hoverOutline, pX - circleR, controlYf - circleR,
                                     circleR * 2.0f, circleR * 2.0f);
            } else {
                graphics.FillEllipse(&activeBg, pX - circleR, controlYf - circleR,
                                     circleR * 2.0f, circleR * 2.0f);
            }
        }
        PointF prevPts[3] = {
            PointF(pX + iconW, controlYf - (iconH / 2.0f)),
            PointF(pX + iconW, controlYf + (iconH / 2.0f)),
            PointF(pX, controlYf)
        };
        graphics.FillPolygon(hoverState == 1 ? &hoverBrush : &iconBrush, prevPts, 3);
        graphics.FillRectangle(hoverState == 1 ? &hoverBrush : &iconBrush,
                               pX, controlYf - (iconH / 2.0f),
                               2.0f * (float)scale * controlScale, iconH);

        if (hoverState == 2) {
            if (outlineControls) {
                graphics.DrawEllipse(&hoverOutline, plX - circleR, controlYf - circleR,
                                     circleR * 2.0f, circleR * 2.0f);
            } else {
                graphics.FillEllipse(&activeBg, plX - circleR, controlYf - circleR,
                                     circleR * 2.0f, circleR * 2.0f);
            }
        }
        if (state.isPlaying) {
            float barW = (3.0f * (float)scale) * controlScale;
            float barH = (14.0f * (float)scale) * controlScale;
            graphics.FillRectangle(hoverState == 2 ? &hoverBrush : &iconBrush,
                                   plX - (barW + 1.0f), controlYf - (barH / 2.0f),
                                   barW, barH);
            graphics.FillRectangle(hoverState == 2 ? &hoverBrush : &iconBrush,
                                   plX + 1.0f, controlYf - (barH / 2.0f),
                                   barW, barH);
        } else {
            float playW = (10.0f * (float)scale) * controlScale;
            float playH = (16.0f * (float)scale) * controlScale;
            PointF playPts[3] = {
                PointF(plX - (playW / 2.0f), controlYf - (playH / 2.0f)),
                PointF(plX - (playW / 2.0f), controlYf + (playH / 2.0f)),
                PointF(plX + (playW / 2.0f), controlYf)
            };
            graphics.FillPolygon(hoverState == 2 ? &hoverBrush : &iconBrush,
                                 playPts, 3);
        }

        if (hoverState == 3) {
            if (outlineControls) {
                graphics.DrawEllipse(&hoverOutline, nX - circleR, controlYf - circleR,
                                     circleR * 2.0f, circleR * 2.0f);
            } else {
                graphics.FillEllipse(&activeBg, nX - circleR, controlYf - circleR,
                                     circleR * 2.0f, circleR * 2.0f);
            }
        }
        PointF nextPts[3] = {
            PointF(nX - iconW, controlYf - (iconH / 2.0f)),
            PointF(nX - iconW, controlYf + (iconH / 2.0f)),
            PointF(nX, controlYf)
        };
        graphics.FillPolygon(hoverState == 3 ? &hoverBrush : &iconBrush, nextPts, 3);
        graphics.FillRectangle(hoverState == 3 ? &hoverBrush : &iconBrush,
                               nX, controlYf - (iconH / 2.0f),
                               2.0f * (float)scale * controlScale, iconH);
    }

    float textAnchorX = discCenterX + ((float)artSize * 0.22f);
    float textXF = nX + max(16.0f * (float)scale, gap * 0.7f);
    textXF += (textAnchorX - textXF) * textCollapseEase;
    int textX = (int)textXF;

    int textMaxWBase = width - textX - 10;
    if (textMaxWBase < 1) textMaxWBase = 1;
    float widthFactor = textVisibleFactor;
    if (widthFactor < 0.08f) widthFactor = 0.08f;
    int textMaxW = (int)(textMaxWBase * widthFactor);
    if (textMaxW < 1) textMaxW = 1;

    wstring fullText = state.title;
    if (!state.artist.empty()) fullText += L" • " + state.artist;

    bool showingLyricLine = false;
    int64_t lyricsQueryPosition100ns = state.timelinePosition100ns;
    if (g_RuntimeLyricsOffsetMs != 0) {
        lyricsQueryPosition100ns -=
            (int64_t)g_RuntimeLyricsOffsetMs * 10000LL;
        if (lyricsQueryPosition100ns < 0) {
            lyricsQueryPosition100ns = 0;
        }
    }

    if (g_RuntimeLyricsEnabled) {
        wstring lyricLine;
        if (TryGetCurrentLyricsLine(lyricsQueryPosition100ns, &lyricLine,
                                    nullptr)) {
            fullText = lyricLine;
            showingLyricLine = true;
        } else {
            bool keepLastLyricVisible = false;
            {
                lock_guard<mutex> lyricsGuard(g_LyricsState.lock);
                keepLastLyricVisible =
                    g_LyricsState.onlineFetchInProgress &&
                    g_LyricsState.lines.empty();
            }

            if (keepLastLyricVisible && !g_LastAnimatedLyricLine.empty()) {
                fullText = g_LastAnimatedLyricLine;
                showingLyricLine = true;
            }
        }
    }

    FontFamily fontFamily(FONT_NAME, nullptr);
    Font font(&fontFamily, (REAL)g_Settings.fontSize, FontStyleBold, UnitPixel);

    RectF layoutRect(0, 0, 2000, 100);
    RectF boundRect;
    graphics.MeasureString(fullText.c_str(), -1, &font, layoutRect, &boundRect);
    g_TextWidth = (int)boundRect.Width;

    bool showProgress = state.timelineDuration100ns > 0 && visibleFactor > 0.10f;
    bool showTimeLabel = showProgress && g_Settings.showTimelineTime;
    int timeFontSize = max(8, g_Settings.fontSize - 2);
    float timeLabelSpace = showTimeLabel ? ((float)timeFontSize + 4.0f) : 0.0f;
    float textBottom = (float)height - timeLabelSpace;
    if (textBottom < 8.0f) textBottom = 8.0f;

    int baseTextAlphaInt = (int)(theme.text.GetA() * textVisibleFactor);
    if (baseTextAlphaInt < 0) baseTextAlphaInt = 0;
    if (baseTextAlphaInt > 255) baseTextAlphaInt = 255;
    if (baseTextAlphaInt <= 1) {
        g_IsScrolling = false;
        g_ScrollOffset = 0.0f;
        g_ScrollPauseMs = kScrollPauseMsDefault;
        return;
    }

    BYTE baseShadowAlpha =
        (BYTE)min((int)theme.textShadow.GetA(), baseTextAlphaInt);

    Region textClip(Rect(textX, 0, textMaxW, (int)textBottom));
    graphics.SetClip(&textClip);

    float textY = (textBottom - boundRect.Height) / 2.0f;
    if (textY < 0.0f) textY = 0.0f;

    if (showingLyricLine) {
        g_IsScrolling = false;
        g_ScrollOffset = 0.0f;
        g_ScrollPauseMs = kScrollPauseMsDefault;

        ULONGLONG nowTick = GetTickCount64();
        if (fullText != g_LastAnimatedLyricLine) {
            g_PreviousAnimatedLyricLine = g_LastAnimatedLyricLine;
            g_LastAnimatedLyricLine = fullText;
            g_LastLyricLineChangeTick = nowTick;
            if (g_hMediaWindow && IsWindow(g_hMediaWindow)) {
                EnsureAnimationTimer(g_hMediaWindow);
            }
        }

        float t = 1.0f;
        if (g_LastLyricLineChangeTick != 0 && kLyricLineTransitionMs > 1.0f) {
            ULONGLONG elapsedMs =
                (nowTick >= g_LastLyricLineChangeTick)
                    ? (nowTick - g_LastLyricLineChangeTick)
                    : (ULONGLONG)kLyricLineTransitionMs;
            t = (float)elapsedMs / kLyricLineTransitionMs;
            if (t < 0.0f) t = 0.0f;
            if (t > 1.0f) t = 1.0f;
        }

        float eased = t * t * (3.0f - 2.0f * t);
        float shiftPx = max(kLyricLineShiftPx, (float)g_Settings.fontSize * 0.95f);

        if (!g_PreviousAnimatedLyricLine.empty() && t < 1.0f) {
            int oldAlphaInt = (int)(baseTextAlphaInt * (1.0f - eased));
            if (oldAlphaInt > 1) {
                if (oldAlphaInt > 255) oldAlphaInt = 255;
                BYTE oldAlpha = (BYTE)oldAlphaInt;
                BYTE oldShadowAlpha = (BYTE)min((int)baseShadowAlpha, oldAlphaInt);
                SolidBrush oldTextBrush(
                    Color(oldAlpha, theme.text.GetRed(), theme.text.GetGreen(),
                          theme.text.GetBlue()));
                SolidBrush oldShadowBrush(
                    Color(oldShadowAlpha, theme.textShadow.GetRed(),
                          theme.textShadow.GetGreen(),
                          theme.textShadow.GetBlue()));
                float oldY = textY - (shiftPx * eased);
                graphics.DrawString(g_PreviousAnimatedLyricLine.c_str(), -1, &font,
                                    PointF((float)textX + 1.0f, oldY + 1.0f),
                                    &oldShadowBrush);
                graphics.DrawString(g_PreviousAnimatedLyricLine.c_str(), -1, &font,
                                    PointF((float)textX, oldY), &oldTextBrush);
            }
        } else if (t >= 1.0f && !g_PreviousAnimatedLyricLine.empty()) {
            g_PreviousAnimatedLyricLine.clear();
        }

        int newAlphaInt = (int)(baseTextAlphaInt * eased);
        if (newAlphaInt > 1) {
            if (newAlphaInt > 255) newAlphaInt = 255;
            BYTE newAlpha = (BYTE)newAlphaInt;
            BYTE newShadowAlpha = (BYTE)min((int)baseShadowAlpha, newAlphaInt);
            SolidBrush newTextBrush(
                Color(newAlpha, theme.text.GetRed(), theme.text.GetGreen(),
                      theme.text.GetBlue()));
            SolidBrush newShadowBrush(
                Color(newShadowAlpha, theme.textShadow.GetRed(),
                      theme.textShadow.GetGreen(),
                      theme.textShadow.GetBlue()));
            float newY = textY + (shiftPx * (1.0f - eased));
            graphics.DrawString(fullText.c_str(), -1, &font,
                                PointF((float)textX + 1.0f, newY + 1.0f),
                                &newShadowBrush);
            graphics.DrawString(fullText.c_str(), -1, &font,
                                PointF((float)textX, newY), &newTextBrush);
        }
    } else {
        if (!g_LastAnimatedLyricLine.empty() || !g_PreviousAnimatedLyricLine.empty() ||
            g_LastLyricLineChangeTick != 0) {
            ResetLyricsAnimationState();
        }

        SolidBrush textBrush(
            Color((BYTE)baseTextAlphaInt, theme.text.GetRed(), theme.text.GetGreen(),
                  theme.text.GetBlue()));
        SolidBrush shadowBrush(
            Color(baseShadowAlpha, theme.textShadow.GetRed(),
                  theme.textShadow.GetGreen(), theme.textShadow.GetBlue()));
        bool allowScroll = textVisibleFactor > 0.92f;

        if (allowScroll && g_TextWidth > textMaxW) {
            g_IsScrolling = true;
            float drawX = (float)textX - g_ScrollOffset;
            graphics.DrawString(fullText.c_str(), -1, &font,
                                PointF(drawX + 1.0f, textY + 1.0f), &shadowBrush);
            graphics.DrawString(fullText.c_str(), -1, &font,
                                PointF(drawX, textY), &textBrush);
            if (drawX + g_TextWidth < width) {
                graphics.DrawString(
                    fullText.c_str(), -1, &font,
                    PointF(drawX + g_TextWidth + kScrollGapPx + 1.0f, textY + 1.0f),
                    &shadowBrush);
                graphics.DrawString(fullText.c_str(), -1, &font,
                                    PointF(drawX + g_TextWidth + kScrollGapPx, textY),
                                    &textBrush);
            }
        } else {
            g_IsScrolling = false;
            g_ScrollOffset = 0.0f;
            g_ScrollPauseMs = kScrollPauseMsDefault;
            graphics.DrawString(fullText.c_str(), -1, &font,
                                PointF((float)textX + 1.0f, textY + 1.0f),
                                &shadowBrush);
            graphics.DrawString(fullText.c_str(), -1, &font,
                                PointF((float)textX, textY), &textBrush);
        }
    }

    graphics.ResetClip();

    if (showProgress) {
        float progress =
            (state.timelineDuration100ns > 0)
                ? ((float)state.timelinePosition100ns /
                   (float)state.timelineDuration100ns)
                : 0.0f;
        if (progress < 0.0f) progress = 0.0f;
        if (progress > 1.0f) progress = 1.0f;

        float ringPadding = 2.8f;
        float ringX = (float)artX - ringPadding;
        float ringY = (float)artY - ringPadding;
        float ringDiameter = discSizeF + (ringPadding * 2.0f);

        Pen ringBgPen(theme.ringBg, theme.ringThicknessBg);
        Pen ringFgPen(theme.ringFg, theme.ringThicknessFg);
        ringBgPen.SetLineJoin(LineJoinRound);
        ringFgPen.SetStartCap(LineCapRound);
        ringFgPen.SetEndCap(LineCapRound);
        ringFgPen.SetLineJoin(LineJoinRound);

        if (g_Settings.ringStyle == 2) {
            ringBgPen.SetDashStyle(DashStyleDot);
            ringFgPen.SetDashStyle(DashStyleDot);
        } else if (g_Settings.ringStyle == 3) {
            REAL dashPattern[2] = {5.0f, 3.0f};
            ringBgPen.SetDashStyle(DashStyleDash);
            ringFgPen.SetDashStyle(DashStyleDash);
            ringBgPen.SetDashPattern(dashPattern, 2);
            ringFgPen.SetDashPattern(dashPattern, 2);
        }

        graphics.DrawArc(&ringBgPen, ringX, ringY, ringDiameter, ringDiameter,
                         -90.0f, 359.9f);
        if (progress > 0.001f) {
            graphics.DrawArc(&ringFgPen, ringX, ringY, ringDiameter, ringDiameter,
                             -90.0f, 360.0f * progress);
        }

        if (g_Settings.ringStyle == 1) {
            float innerPad = ringPadding + 1.9f;
            float innerX = (float)artX - innerPad;
            float innerY = (float)artY - innerPad;
            float innerD = discSizeF + (innerPad * 2.0f);
            Pen innerBg(theme.ringBg, 1.0f);
            Pen innerFg(theme.ringFg, 1.3f);
            innerFg.SetStartCap(LineCapRound);
            innerFg.SetEndCap(LineCapRound);
            graphics.DrawArc(&innerBg, innerX, innerY, innerD, innerD, -90.0f, 359.9f);
            if (progress > 0.001f) {
                graphics.DrawArc(&innerFg, innerX, innerY, innerD, innerD,
                                 -90.0f, 360.0f * progress);
            }
        }

        if (showTimeLabel) {
            wstring timeText = FormatTimelineTime(state.timelinePosition100ns) +
                               L" / " +
                               FormatTimelineTime(state.timelineDuration100ns);
            Font timeFont(&fontFamily, (REAL)timeFontSize, FontStyleRegular,
                          UnitPixel);
            SolidBrush timeBrush(Color((BYTE)min(255, (int)(theme.timeText.GetA() * visibleFactor)),
                                      theme.timeText.GetRed(),
                                      theme.timeText.GetGreen(),
                                      theme.timeText.GetBlue()));
            StringFormat rightAlign;
            rightAlign.SetAlignment(StringAlignmentFar);
            RectF timeRect((REAL)textX,
                           (REAL)(height - (timeFontSize + 2)),
                           (REAL)(width - textX - 8),
                           (REAL)(timeFontSize + 2));
            graphics.DrawString(timeText.c_str(), -1, &timeFont, timeRect,
                                &rightAlign, &timeBrush);
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
    DWORD, DWORD
) {
    if (!IsTaskbarWindow(hwnd) || !g_hMediaWindow) return;
    PostMessage(g_hMediaWindow, WM_APP + 10, 0, 0);
}

// Register Event Hook scoped to Taskbar Thread
void RegisterTaskbarHook(HWND hwnd)
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

// --- Input & Window Messages ---

void EnsureAnimationTimer(HWND hwnd) {
    if (!hwnd) return;

    if (g_RuntimePauseAnimations && !g_CollapseDelayPending) {
        return;
    }

    if (g_LastAnimationTick == 0) {
        g_LastAnimationTick = GetTickCount64();
    }

    if (!g_AnimationTimerRunning) {
        SetTimer(hwnd, TIMER_ID_ANIMATION, 16, NULL);
        g_AnimationTimerRunning = true;
    }
}

void ResetCollapseDelayState() {
    g_CollapseDelayPending = false;
    g_CollapseDelayUntilTick = 0;
}

void SyncCollapseAnimationState(HWND hwnd) {
    if (!hwnd) return;

    if (!g_CollapseToDiscOnMouseLeave) {
        g_CollapseTarget = 0.0f;
        ResetCollapseDelayState();
        if (g_CollapseProgress != 0.0f) {
            g_CollapseProgress = 0.0f;
            InvalidateRect(hwnd, NULL, FALSE);
        }
        return;
    }

    POINT cursor;
    if (GetCursorPos(&cursor)) {
        RECT rc{};
        if (GetWindowRect(hwnd, &rc)) {
            g_IsMouseInsideWidget = PtInRect(&rc, cursor) != FALSE;
        }
    }

    if (g_IsMouseInsideWidget) {
        ResetCollapseDelayState();
        g_CollapseTarget = 0.0f;
        if (g_RuntimePauseAnimations && g_CollapseProgress != 0.0f) {
            g_CollapseProgress = 0.0f;
            InvalidateRect(hwnd, NULL, FALSE);
        }
    } else {
        if (g_CollapseProgress >= 0.999f || g_CollapseTarget >= 1.0f) {
            ResetCollapseDelayState();
            g_CollapseTarget = 1.0f;
            if (g_RuntimePauseAnimations && g_CollapseProgress != 1.0f) {
                g_CollapseProgress = 1.0f;
                InvalidateRect(hwnd, NULL, FALSE);
            }
        } else {
            ULONGLONG delayMs =
                (g_Settings.collapseDelayMs > 0) ? (ULONGLONG)g_Settings.collapseDelayMs : 0;
            if (delayMs == 0) {
                ResetCollapseDelayState();
                g_CollapseTarget = 1.0f;
                if (g_RuntimePauseAnimations) {
                    g_CollapseProgress = 1.0f;
                    InvalidateRect(hwnd, NULL, FALSE);
                }
            } else {
                if (!g_CollapseDelayPending) {
                    g_CollapseDelayPending = true;
                    g_CollapseDelayUntilTick = GetTickCount64() + delayMs;
                }
                g_CollapseTarget = 0.0f;
            }
        }
    }

    if (g_CollapseTarget != g_CollapseProgress || g_CollapseDelayPending) {
        EnsureAnimationTimer(hwnd);
    }
}

void RefreshMediaStateAndVisibility(HWND hwnd) {
    RefreshSpotifyMediaState();

    bool shouldHide = false;

    if (g_Settings.hideFullscreen) {
        QUERY_USER_NOTIFICATION_STATE state;
        if (SUCCEEDED(SHQueryUserNotificationState(&state))) {
            if (state == QUNS_BUSY || state == QUNS_RUNNING_D3D_FULL_SCREEN ||
                state == QUNS_PRESENTATION_MODE) {
                shouldHide = true;
            }
        }
    }

    bool hasMedia = false;
    {
        lock_guard<mutex> guard(g_MediaState.lock);
        hasMedia = g_MediaState.hasMedia;
    }

    if (!hasMedia) {
        g_IsHiddenByIdle = true;
        shouldHide = true;
    } else {
        g_IsHiddenByIdle = false;
    }

    if (g_RuntimeLyricsEnabled && hasMedia) {
        shouldHide = false;
        g_IsHiddenByIdle = false;
    }

    if (g_RuntimePinAlwaysVisible) {
        shouldHide = false;
        g_IsHiddenByIdle = false;
    }

    RequestWidgetVisibility(hwnd, !shouldHide);

    SyncCollapseAnimationState(hwnd);
    if (IsWindowVisible(hwnd)) {
        InvalidateRect(hwnd, NULL, FALSE);
    }
}

LRESULT CALLBACK MediaWidgetWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: 
            UpdateAppearance(hwnd); 
            SetTimer(hwnd, TIMER_ID_POLL_MEDIA, 1000, NULL); 
            g_LastAnimationTick = 0;
            g_AnimationTimerRunning = false;
            g_IsMouseInsideWidget = true;
            g_CollapseProgress = 0.0f;
            g_CollapseTarget = 0.0f;
            g_CollapseDelayUntilTick = 0;
            g_CollapseDelayPending = false;
            RegisterTaskbarHook(hwnd);
            return 0;

        case WM_ERASEBKGND: 
            return 1;

        case WM_CLOSE:
            return 0;

        case MSG_APP_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            if (g_TaskbarHook) {
                UnhookWinEvent(g_TaskbarHook);
                g_TaskbarHook = nullptr;
            }
            KillTimer(hwnd, TIMER_ID_POLL_MEDIA);
            KillTimer(hwnd, TIMER_ID_ANIMATION);
            g_AnimationTimerRunning = false;
            KillTimer(hwnd, TIMER_ID_FADE);
            g_FadeTimerRunning = false;
            UnsubscribeSpotifySessionEvents();
            if (g_SessionManager && g_ManagerEventsSubscribed) {
                if (g_SessionsChangedToken.value != 0) {
                    g_SessionManager.SessionsChanged(g_SessionsChangedToken);
                }
                if (g_CurrentSessionChangedToken.value != 0) {
                    g_SessionManager.CurrentSessionChanged(g_CurrentSessionChangedToken);
                }
            }
            g_SessionsChangedToken = {};
            g_CurrentSessionChangedToken = {};
            g_ManagerEventsSubscribed = false;
            g_SessionManager = nullptr;
            PostQuitMessage(0);
            return 0;

        case WM_SETTINGCHANGE:
            UpdateAppearance(hwnd);
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;

        case WM_TIMER:
            if (wParam == TIMER_ID_POLL_MEDIA) {
                RefreshMediaStateAndVisibility(hwnd);
            }
            else if (wParam == TIMER_ID_ANIMATION) {
                if (!IsWindowVisible(hwnd)) {
                    g_LastAnimationTick = 0;
                    g_AnimationTimerRunning = false;
                    KillTimer(hwnd, TIMER_ID_ANIMATION);
                    return 0;
                }

                ULONGLONG now = GetTickCount64();
                float dt = (g_LastAnimationTick == 0)
                               ? (1.0f / 60.0f)
                               : (float)(now - g_LastAnimationTick) / 1000.0f;
                g_LastAnimationTick = now;
                if (dt < 0.0f) dt = 1.0f / 60.0f;
                if (dt > kMaxFrameDeltaSec) dt = kMaxFrameDeltaSec;

                bool keepAnimationTimer = false;
                bool needInvalidate = false;

                if (g_CollapseToDiscOnMouseLeave && g_CollapseDelayPending) {
                    if (g_IsMouseInsideWidget) {
                        ResetCollapseDelayState();
                    } else if (now >= g_CollapseDelayUntilTick) {
                        g_CollapseDelayPending = false;
                        g_CollapseTarget = 1.0f;
                        if (g_RuntimePauseAnimations) {
                            g_CollapseProgress = 1.0f;
                            needInvalidate = true;
                        }
                    } else {
                        keepAnimationTimer = true;
                    }
                }

                if (g_RuntimePauseAnimations) {
                    if (needInvalidate) {
                        InvalidateRect(hwnd, NULL, FALSE);
                    }
                    if (!keepAnimationTimer) {
                        g_LastAnimationTick = 0;
                        g_AnimationTimerRunning = false;
                        KillTimer(hwnd, TIMER_ID_ANIMATION);
                    }
                    return 0;
                }

                if (g_CollapseToDiscOnMouseLeave) {
                    float deltaCollapse = g_CollapseTarget - g_CollapseProgress;
                    if (deltaCollapse > 0.001f || deltaCollapse < -0.001f) {
                        keepAnimationTimer = true;
                        float collapseStep = (float)g_Settings.collapseSpeed *
                                             GetAnimationSpeedMultiplier() * dt;
                        if (deltaCollapse > 0.0f) {
                            g_CollapseProgress += collapseStep;
                            if (g_CollapseProgress > g_CollapseTarget) {
                                g_CollapseProgress = g_CollapseTarget;
                            }
                        } else {
                            g_CollapseProgress -= collapseStep;
                            if (g_CollapseProgress < g_CollapseTarget) {
                                g_CollapseProgress = g_CollapseTarget;
                            }
                        }
                        needInvalidate = true;
                    }
                } else if (g_CollapseProgress != 0.0f || g_CollapseTarget != 0.0f ||
                           g_CollapseDelayPending) {
                    g_CollapseProgress = 0.0f;
                    g_CollapseTarget = 0.0f;
                    ResetCollapseDelayState();
                    needInvalidate = true;
                }

                if (g_IsScrolling) {
                    keepAnimationTimer = true;
                    if (g_ScrollPauseMs > 0.0f) {
                        g_ScrollPauseMs -= dt * 1000.0f;
                    } else {
                        g_ScrollOffset +=
                            (kScrollSpeedPxPerSec * GetScrollSpeedMultiplier()) * dt;
                        float loopWidth = (float)g_TextWidth + kScrollGapPx;
                        if (g_ScrollOffset >= loopWidth) {
                            g_ScrollOffset -= loopWidth;
                            g_ScrollPauseMs =
                                kScrollPauseMsDefault / max(0.5f, GetScrollSpeedMultiplier());
                        }
                        needInvalidate = true;
                    }
                } else {
                    g_ScrollOffset = 0.0f;
                    g_ScrollPauseMs =
                        kScrollPauseMsDefault / max(0.5f, GetScrollSpeedMultiplier());
                }

                if (g_LastLyricLineChangeTick != 0 && g_RuntimeLyricsEnabled) {
                    ULONGLONG elapsedMs =
                        (now >= g_LastLyricLineChangeTick)
                            ? (now - g_LastLyricLineChangeTick)
                            : (ULONGLONG)(kLyricLineTransitionMs + 1.0f);
                    if (elapsedMs < (ULONGLONG)(kLyricLineTransitionMs + 40.0f)) {
                        keepAnimationTimer = true;
                        needInvalidate = true;
                    }
                }

                bool shouldSpin = false;
                {
                    lock_guard<mutex> guard(g_MediaState.lock);
                    shouldSpin = g_MediaState.hasMedia && g_MediaState.isPlaying;
                }

                float targetVelocity =
                    shouldSpin ? ((float)g_Settings.spinSpeed * 60.0f) : 0.0f;
                float blend = dt * kDiscSmoothingPerSec * GetDiscSmoothingMultiplier();
                if (blend > 1.0f) blend = 1.0f;
                g_DiscVelocity += (targetVelocity - g_DiscVelocity) * blend;

                if (!shouldSpin && g_DiscVelocity < 0.01f) {
                    g_DiscVelocity = 0.0f;
                }

                if (g_DiscVelocity > 0.0f) {
                    keepAnimationTimer = true;
                    g_DiscAngle += g_DiscVelocity * dt;
                    if (g_DiscAngle >= 360.0f) {
                        g_DiscAngle -= 360.0f;
                    }
                    needInvalidate = true;
                }

                if (needInvalidate) {
                    InvalidateRect(hwnd, NULL, FALSE);
                }

                if (!keepAnimationTimer) {
                    g_LastAnimationTick = 0;
                    g_AnimationTimerRunning = false;
                    KillTimer(hwnd, TIMER_ID_ANIMATION);
                }
            }
            else if (wParam == TIMER_ID_FADE) {
                UpdateWidgetFade(hwnd);
            }
            return 0;

        case WM_APP + 20:
            RefreshMediaStateAndVisibility(hwnd);
            return 0;

        case WM_APP + 10: {
            HWND hTaskbar = FindWindow(TEXT("Shell_TrayWnd"), nullptr);
            if (!hTaskbar) break;

            if (!IsWindowVisible(hTaskbar)) {
                RequestWidgetVisibility(hwnd, false);
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
                if (!gameModeHide) RequestWidgetVisibility(hwnd, true);
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
            if (g_CollapseToDiscOnMouseLeave) {
                g_IsMouseInsideWidget = true;
                ResetCollapseDelayState();
                if (g_CollapseTarget != 0.0f) {
                    g_CollapseTarget = 0.0f;
                    if (g_RuntimePauseAnimations) {
                        g_CollapseProgress = 0.0f;
                        InvalidateRect(hwnd, NULL, FALSE);
                    }
                    EnsureAnimationTimer(hwnd);
                }
            }

            if (g_IsDraggingWidget) {
                POINT cursor;
                if (GetCursorPos(&cursor)) {
                    int dx = cursor.x - g_DragStartCursor.x;
                    int dy = cursor.y - g_DragStartCursor.y;
                    if (abs(dx) > 2 || abs(dy) > 2) {
                        g_DragMoved = true;
                    }

                    int newX = g_DragStartWindowRect.left + dx;
                    int newY = g_DragStartWindowRect.top + dy;
                    SetWindowPos(hwnd, HWND_TOPMOST, newX, newY, 0, 0,
                                 SWP_NOSIZE | SWP_NOACTIVATE);

                    HWND hTaskbar = FindWindow(L"Shell_TrayWnd", nullptr);
                    if (hTaskbar) {
                        RECT rc;
                        if (GetWindowRect(hTaskbar, &rc)) {
                            int taskbarHeight = rc.bottom - rc.top;
                            g_Settings.offsetX = newX - rc.left;
                            g_Settings.offsetY =
                                newY - (rc.top + (taskbarHeight / 2) -
                                        (g_Settings.height / 2));
                        }
                    }

                    InvalidateRect(hwnd, NULL, FALSE);
                }
                return 0;
            }

            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            int baseArtSize = g_Settings.height - 12;
            int artSize = (int)(baseArtSize * g_Settings.discScale);
            int maxArtSize = max(12, g_Settings.height - 4);
            if (artSize < 12) artSize = 12;
            if (artSize > maxArtSize) artSize = maxArtSize;
            double scale = g_Settings.buttonScale;

            float collapseT = g_CollapseToDiscOnMouseLeave ? g_CollapseProgress : 0.0f;
            if (collapseT < 0.0f) collapseT = 0.0f;
            if (collapseT > 1.0f) collapseT = 1.0f;
            float collapseEase = ApplyCollapseEasing(collapseT);
            float visibleFactor = CalculateCollapseVisibleFactor(collapseEase);
            float controlCollapseEase =
                g_RuntimeKeepControlsVisible ? 0.0f : collapseEase;
            float controlVisibleFactor =
                g_RuntimeKeepControlsVisible ? 1.0f : visibleFactor;

            int startControlX = 6 + artSize + (int)(12 * scale);
            float gap = (float)g_Settings.controlGap * (float)scale;
            float minGap = 18.0f * (float)scale;
            if (gap < minGap) gap = minGap;

            float discCenterX = 6.0f + ((float)artSize / 2.0f);
            float holeSize = (float)max(5, artSize / 5);
            float controlTargetX = discCenterX + (holeSize * 0.2f);

            float pBaseX = (float)startControlX;
            float plBaseX = pBaseX + gap;
            float nBaseX = plBaseX + gap;
            float pX = pBaseX + (controlTargetX - pBaseX) * controlCollapseEase;
            float plX = plBaseX + (controlTargetX - plBaseX) * controlCollapseEase;
            float nX = nBaseX + (controlTargetX - nBaseX) * controlCollapseEase;

            int controlY = g_Settings.height / 2;
            float controlScale = 1.0f - (0.30f * controlCollapseEase);
            if (controlScale < 0.65f) controlScale = 0.65f;
            float visualRadius = (12.0f * (float)scale) * controlScale;
            float hitRadius = visualRadius * (float)g_Settings.hitboxScale;
            if (hitRadius < visualRadius) hitRadius = visualRadius;

            int newState = 0;
            bool controlsInteractable =
                g_RuntimeKeepControlsVisible ||
                !(g_CollapseToDiscOnMouseLeave && controlVisibleFactor < 0.30f);
            if (controlsInteractable &&
                y >= controlY - hitRadius && y <= controlY + hitRadius) {
                if (x >= pX - hitRadius && x <= pX + hitRadius) newState = 1;
                else if (x >= plX - hitRadius && x <= plX + hitRadius) newState = 2;
                else if (x >= nX - hitRadius && x <= nX + hitRadius) newState = 3;
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
            g_IsMouseInsideWidget = false;
            if (g_CollapseToDiscOnMouseLeave) {
                if (g_Settings.collapseDelayMs <= 0) {
                    g_CollapseTarget = 1.0f;
                    g_CollapseDelayPending = false;
                    g_CollapseDelayUntilTick = 0;
                    if (g_RuntimePauseAnimations) {
                        g_CollapseProgress = 1.0f;
                    }
                } else {
                    g_CollapseDelayPending = true;
                    g_CollapseDelayUntilTick =
                        GetTickCount64() + (ULONGLONG)g_Settings.collapseDelayMs;
                    g_CollapseTarget = 0.0f;
                }
                EnsureAnimationTimer(hwnd);
            }
            InvalidateRect(hwnd, NULL, FALSE);
            break;
        case WM_LBUTTONDOWN:
            if (g_Settings.enableDragMove && g_HoverState == 0) {
                g_IsDraggingWidget = true;
                g_DragMoved = false;
                GetCursorPos(&g_DragStartCursor);
                GetWindowRect(hwnd, &g_DragStartWindowRect);
                SetCapture(hwnd);
                return 0;
            }
            return 0;
        case WM_CAPTURECHANGED:
            g_IsDraggingWidget = false;
            return 0;
        case WM_LBUTTONUP:
            if (g_IsDraggingWidget) {
                g_IsDraggingWidget = false;
                ReleaseCapture();
                if (g_DragMoved) {
                    SaveWidgetPosition();
                    PostMessage(hwnd, WM_APP + 10, 0, 0);
                }
                return 0;
            }

            if (g_HoverState > 0) SendSpotifyMediaCommand(g_HoverState);
            return 0;
        case WM_RBUTTONUP: {
            POINT screenPt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            ClientToScreen(hwnd, &screenPt);
            ShowMediaWidgetContextMenu(hwnd, screenPt);
            return 0;
        }
        case WM_CONTEXTMENU: {
            POINT screenPt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            if (screenPt.x == -1 && screenPt.y == -1) {
                GetCursorPos(&screenPt);
            }
            ShowMediaWidgetContextMenu(hwnd, screenPt);
            return 0;
        }
        case WM_MOUSEWHEEL: {
            short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
            if ((GET_KEYSTATE_WPARAM(wParam) & MK_SHIFT) != 0) {
                int steps = zDelta / WHEEL_DELTA;
                if (steps == 0) {
                    steps = (zDelta > 0) ? 1 : -1;
                }
                SeekSpotifyBySeconds(steps * kSeekStepSeconds);
                InvalidateRect(hwnd, NULL, FALSE);
                return 0;
            }

            keybd_event(zDelta > 0 ? VK_VOLUME_UP : VK_VOLUME_DOWN, 0, 0, 0);
            keybd_event(zDelta > 0 ? VK_VOLUME_UP : VK_VOLUME_DOWN, 0,
                        KEYEVENTF_KEYUP, 0);
            return 0;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc; GetClientRect(hwnd, &rc);
            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP memBitmap = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
            HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);
            
            RenderMediaWidgetFrame(memDC, rc.right, rc.bottom);

            bool isPlaying = false;
            {
                lock_guard<mutex> guard(g_MediaState.lock);
                isPlaying = g_MediaState.hasMedia && g_MediaState.isPlaying;
            }
            bool shouldAnimateSpin = (isPlaying && g_Settings.spinSpeed > 0.0) || g_DiscVelocity > 0.0f;
            bool shouldAnimateCollapse = false;
            if (g_CollapseToDiscOnMouseLeave) {
                float collapseDelta = g_CollapseTarget - g_CollapseProgress;
                shouldAnimateCollapse = (collapseDelta > 0.001f || collapseDelta < -0.001f);
            }
            if (!g_RuntimePauseAnimations) {
                if (g_IsScrolling || shouldAnimateSpin || shouldAnimateCollapse) {
                    EnsureAnimationTimer(hwnd);
                }
            } else if (g_CollapseDelayPending) {
                EnsureAnimationTimer(hwnd);
            }

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
    wc.lpfnWndProc = MediaWidgetWindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = TEXT("WindhawkMusicLounge_GSMTC");
    wc.hCursor = LoadCursor(NULL, IDC_HAND);
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

    SetLayeredWindowAttributes(g_hMediaWindow, TRANSPARENT_COLORKEY, 255, LWA_ALPHA | LWA_COLORKEY);
    
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
    DebugLog(LOG_BASIC, L"WhTool_ModInit");
    StartLyricsBootstrapIfNeeded();
    g_pMediaThread = new std::thread(MediaThread);
    return TRUE;
}

void WhTool_ModUninit() {
    DebugLog(LOG_BASIC, L"WhTool_ModUninit");
    if (g_hMediaWindow) SendMessage(g_hMediaWindow, MSG_APP_CLOSE, 0, 0);
    if (g_pMediaThread) {
        if (g_pMediaThread->joinable()) g_pMediaThread->join();
        delete g_pMediaThread;
        g_pMediaThread = nullptr;
    }
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    DebugLog(LOG_BASIC, L"WhTool_ModSettingsChanged");
    ResetLyricsDiskCacheState();
    ResetLyricsState();
    StartLyricsBootstrapIfNeeded();
    if (g_hMediaWindow) {
         SendMessage(g_hMediaWindow, WM_TIMER, TIMER_ID_POLL_MEDIA, 0);
         SendMessage(g_hMediaWindow, WM_SETTINGCHANGE, 0, 0); 
         SyncCollapseAnimationState(g_hMediaWindow);
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
