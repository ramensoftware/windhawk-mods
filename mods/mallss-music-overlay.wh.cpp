// ==WindhawkMod==
// @id              mallss-music-overlay
// @name            Mallss Music Overlay
// @description     Compact desktop music overlay with album art, clock, marquee title, progress and media controls.
// @version         1.4.1
// @author          Mallss
// @github          https://github.com/ItsMeMal
// @include         explorer.exe
// @compilerOptions -lgdiplus -lgdi32 -lshlwapi -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
# Mallss Music Overlay

A compact Windows desktop music overlay powered by Windows System Media
Transport Controls.

## Features

- Album artwork
- Song title and artist
- Scrolling marquee for long titles
- Playback progress bar
- Elapsed and total duration
- Previous, play/pause and next controls
- Click the progress bar to seek
- Desktop clock and date
- Automatic display while media is playing
- Local monotonic playback clock to prevent elapsed-time jitter

## Supported media

The mod uses Windows Global System Media Transport Controls, so it can
work with media applications that expose their playback session through
Windows.

## Usage

Enable the mod in Windhawk and start playing supported media.

The overlay appears near the bottom-right corner of the desktop.

When another application is active, the overlay remains alive but stays
behind the active application. It becomes visible again when returning
to the desktop.

## Controls

- Previous: skip to the previous track
- Play/Pause: toggle playback
- Next: skip to the next track
- Progress bar: click to seek

## Notes

The elapsed playback position is driven by a local monotonic timer during
normal playback. Media Session timeline data is used for initial
positioning, track changes and explicit seeks.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <windowsx.h>
#include <shlwapi.h>
#include <gdiplus.h>

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>

#include <atomic>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cwctype>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib, "runtimeobject.lib")

using namespace Gdiplus;
using namespace winrt;

using namespace Windows::Media::Control;
using namespace Windows::Storage::Streams;

// ============================================================
// CONFIG
// ============================================================

static constexpr int OVERLAY_W = 420;
static constexpr int OVERLAY_H = 164;

static constexpr int SCREEN_MARGIN_RIGHT = 18;
static constexpr int SCREEN_MARGIN_BOTTOM = 18;

static constexpr int COVER_X = 12;
static constexpr int COVER_Y = 11;
static constexpr int COVER_SIZE = 92;

static constexpr int CONTENT_X = 116;

static constexpr int TITLE_WIDTH = 178;
static constexpr int ARTIST_WIDTH = 178;
static constexpr int ALBUM_WIDTH = 178;

static constexpr float CLOCK_X = 302.0f;
static constexpr float CLOCK_Y = 8.0f;
static constexpr float CLOCK_W = 96.0f;
static constexpr float CLOCK_H = 42.0f;

static constexpr float PROGRESS_X = 116.0f;
static constexpr float PROGRESS_Y = 66.0f;
static constexpr float PROGRESS_W = 286.0f;
static constexpr float PROGRESS_H = 5.0f;

static constexpr float PREV_X = 205.0f;
static constexpr float PLAY_X = 259.0f;
static constexpr float NEXT_X = 313.0f;
static constexpr float CONTROL_Y = 136.0f;

static constexpr float TITLE_FONT_SIZE = 14.0f;
static constexpr float ARTIST_FONT_SIZE = 10.5f;
static constexpr float ALBUM_FONT_SIZE = 8.5f;

static constexpr double MARQUEE_SPEED = 62.0;
static constexpr double MARQUEE_GAP = 42.0;
static constexpr double MARQUEE_START_HOLD = 1.15;
static constexpr double MARQUEE_END_HOLD = 0.80;

static constexpr int MEDIA_POLL_MS = 500;

static constexpr int FRAME_MS = 16;

static constexpr int ZORDER_CHECK_MS = 120;

// ============================================================
// FORWARD DECLARATIONS
// ============================================================

static void UIThreadProc();
static void MediaThreadProc();

static void PositionOverlay();
static void UpdateOverlayZOrder(bool force);

static void MediaPrevious();
static void MediaNext();
static void MediaTogglePlayPause();
static void MediaSeek(double position);

// ============================================================
// GLOBAL
// ============================================================

static bool g_activeMod = false;

static HANDLE g_singletonMutex = nullptr;

static HWND g_hwnd = nullptr;

// ============================================================
// THREAD STATE
// ============================================================

static std::atomic<bool> g_running{true};

static std::thread g_mediaThread;
static std::thread g_uiThread;

static std::mutex g_stateMutex;

// ============================================================
// PLAYBACK STATE
// ============================================================

static std::atomic<bool> g_playing{false};
static std::atomic<bool> g_visible{false};

// ============================================================
// GDI+
// ============================================================

static ULONG_PTR g_gdiplusToken = 0;

// ============================================================
// BACK BUFFER
// ============================================================

static HDC g_backDC = nullptr;
static HBITMAP g_backBitmap = nullptr;
static HBITMAP g_backOldBitmap = nullptr;
static void* g_backBits = nullptr;

static bool g_backBufferReady = false;

// ============================================================
// MEDIA
// ============================================================

static bool g_mediaValid = false;

static std::wstring g_title =
    L"No music";

static std::wstring g_artist;

static std::wstring g_album;

static double g_duration = 0.0;

// ============================================================
// LOCAL PLAYBACK CLOCK
// ============================================================

static double g_clockAnchorPosition = 0.0;

static int64_t g_clockAnchorQpc = 0;

static bool g_clockRunning = false;

static double g_frozenPosition = 0.0;

// ============================================================
// MEDIA SESSION
// ============================================================

static GlobalSystemMediaTransportControlsSessionManager
    g_sessionManager{nullptr};

static GlobalSystemMediaTransportControlsSession
    g_session{nullptr};

// ============================================================
// COVER
// ============================================================

static Bitmap* g_coverBitmap = nullptr;

static std::wstring g_coverKey;

// ============================================================
// MARQUEE
// ============================================================

enum class MarqueePhase
{
    HoldStart,
    Scrolling,
    HoldEnd
};

static MarqueePhase g_marqueePhase =
    MarqueePhase::HoldStart;

static double g_marqueeTimer = 0.0;

static double g_marqueeOffset = 0.0;

static int g_cachedTitleWidth = 0;

static bool g_cachedTitleOverflow = false;

static std::wstring g_cachedTitle;

// ============================================================
// CLOCK CACHE
// ============================================================

static int g_cachedMinuteKey = -1;

// ============================================================
// Z ORDER CACHE
// ============================================================

static bool g_lastDesktopActive = false;

static bool g_haveZOrderState = false;

static int64_t g_lastZOrderQpc = 0;

// ============================================================
// QPC
// ============================================================

static int64_t QpcNow()
{
    LARGE_INTEGER value{};

    if (!QueryPerformanceCounter(&value))
        return 0;

    return value.QuadPart;
}

static double SecondsSince(
    int64_t start
)
{
    if (start == 0)
        return 0.0;

    LARGE_INTEGER frequency{};

    if (!QueryPerformanceFrequency(&frequency))
        return 0.0;

    const int64_t now =
        QpcNow();

    return static_cast<double>(
        now - start
    ) /
    static_cast<double>(
        frequency.QuadPart
    );
}

// ============================================================
// STRING HELPERS
// ============================================================

static std::wstring TrimString(
    const std::wstring& value
)
{
    size_t start = 0;
    size_t end = value.size();

    while (
        start < end &&
        iswspace(value[start])
    )
    {
        ++start;
    }

    while (
        end > start &&
        iswspace(value[end - 1])
    )
    {
        --end;
    }

    return value.substr(
        start,
        end - start
    );
}

static std::wstring SafeString(
    hstring value
)
{
    if (value.empty())
        return L"";

    return value.c_str();
}

static bool SameText(
    const std::wstring& a,
    const std::wstring& b
)
{
    return
        _wcsicmp(
            a.c_str(),
            b.c_str()
        ) == 0;
}

// ============================================================
// TIME
// ============================================================

static std::wstring FormatTime(
    double seconds
)
{
    if (seconds < 0.0)
        seconds = 0.0;

    const int total =
        static_cast<int>(
            seconds
        );

    const int hours =
        total / 3600;

    const int minutes =
        (total % 3600) / 60;

    const int secs =
        total % 60;

    wchar_t buffer[64]{};

    if (hours > 0)
    {
        swprintf_s(
            buffer,
            L"%d:%02d:%02d",
            hours,
            minutes,
            secs
        );
    }
    else
    {
        swprintf_s(
            buffer,
            L"%d:%02d",
            minutes,
            secs
        );
    }

    return buffer;
}

static std::wstring CurrentClock()
{
    SYSTEMTIME st{};

    GetLocalTime(
        &st
    );

    wchar_t buffer[32]{};

    swprintf_s(
        buffer,
        L"%02d:%02d",
        st.wHour,
        st.wMinute
    );

    return buffer;
}

static std::wstring CurrentDate()
{
    SYSTEMTIME st{};

    GetLocalTime(
        &st
    );

    wchar_t buffer[64]{};

    swprintf_s(
        buffer,
        L"%02d/%02d/%04d",
        st.wDay,
        st.wMonth,
        st.wYear
    );

    return buffer;
}

static int CurrentMinuteKey()
{
    SYSTEMTIME st{};

    GetLocalTime(
        &st
    );

    return
        st.wHour * 60 +
        st.wMinute;
}

// ============================================================
// LOCAL PLAYBACK CLOCK
// ============================================================

static double GetLocalPlaybackPositionLocked()
{
    if (!g_clockRunning)
        return g_frozenPosition;

    const double elapsed =
        SecondsSince(
            g_clockAnchorQpc
        );

    double position =
        g_clockAnchorPosition +
        elapsed;

    if (position < 0.0)
        position = 0.0;

    if (
        g_duration > 0.0 &&
        position > g_duration
    )
    {
        position =
            g_duration;
    }

    return position;
}

static void StartLocalClockLocked(
    double position
)
{
    if (position < 0.0)
        position = 0.0;

    if (
        g_duration > 0.0 &&
        position > g_duration
    )
    {
        position =
            g_duration;
    }

    g_clockAnchorPosition =
        position;

    g_clockAnchorQpc =
        QpcNow();

    g_frozenPosition =
        position;

    g_clockRunning =
        true;
}

static void FreezeLocalClockLocked(
    double position
)
{
    if (position < 0.0)
        position = 0.0;

    if (
        g_duration > 0.0 &&
        position > g_duration
    )
    {
        position =
            g_duration;
    }

    g_frozenPosition =
        position;

    g_clockAnchorPosition =
        position;

    g_clockAnchorQpc =
        QpcNow();

    g_clockRunning =
        false;
}

// ============================================================
// MARQUEE
// ============================================================

static void ResetMarqueeLocked()
{
    g_marqueePhase =
        MarqueePhase::HoldStart;

    g_marqueeTimer =
        0.0;

    g_marqueeOffset =
        0.0;
}

// ============================================================
// COVER CLEANUP
// ============================================================

static void DestroyCoverLocked()
{
    delete g_coverBitmap;

    g_coverBitmap =
        nullptr;

    g_coverKey.clear();
}

// ============================================================
// LOAD COVER
// ============================================================

static bool LoadCoverFromThumbnail(
    const GlobalSystemMediaTransportControlsSessionMediaProperties& props,
    const std::wstring& key
)
{
    try
    {
        auto thumbnail =
            props.Thumbnail();

        if (!thumbnail)
            return false;

        auto stream =
            thumbnail
                .OpenReadAsync()
                .get();

        if (!stream)
            return false;

        const uint32_t size =
            static_cast<uint32_t>(
                stream.Size()
            );

        if (
            size == 0 ||
            size > 20 * 1024 * 1024
        )
        {
            return false;
        }

        DataReader reader(
            stream
        );

        reader.LoadAsync(
            size
        ).get();

        std::vector<uint8_t> bytes(
            size
        );

        reader.ReadBytes(
            array_view<uint8_t>(
                bytes.data(),
                bytes.data() +
                    bytes.size()
            )
        );

        reader.Close();

        HGLOBAL hGlobal =
            GlobalAlloc(
                GMEM_MOVEABLE,
                bytes.size()
            );

        if (!hGlobal)
            return false;

        void* memory =
            GlobalLock(
                hGlobal
            );

        if (!memory)
        {
            GlobalFree(
                hGlobal
            );

            return false;
        }

        memcpy(
            memory,
            bytes.data(),
            bytes.size()
        );

        GlobalUnlock(
            hGlobal
        );

        IStream* pStream =
            nullptr;

        HRESULT hr =
            CreateStreamOnHGlobal(
                hGlobal,
                TRUE,
                &pStream
            );

        if (
            FAILED(hr) ||
            !pStream
        )
        {
            GlobalFree(
                hGlobal
            );

            return false;
        }

        Bitmap* bitmap =
            Bitmap::FromStream(
                pStream
            );

        pStream->Release();

        if (
            !bitmap ||
            bitmap->GetLastStatus() != Ok
        )
        {
            delete bitmap;

            return false;
        }

        {
            std::scoped_lock lock(
                g_stateMutex
            );

            DestroyCoverLocked();

            g_coverBitmap =
                bitmap;

            g_coverKey =
                key;
        }

        return true;
    }
    catch (...)
    {
        return false;
    }
}

// ============================================================
// UPDATE COVER
// ============================================================

static bool UpdateCover(
    const GlobalSystemMediaTransportControlsSessionMediaProperties& props,
    const std::wstring& key
)
{
    {
        std::scoped_lock lock(
            g_stateMutex
        );

        if (
            g_coverBitmap &&
            SameText(
                g_coverKey,
                key
            )
        )
        {
            return true;
        }
    }

    return LoadCoverFromThumbnail(
        props,
        key
    );
}

// ============================================================
// MEDIA COMMANDS
// ============================================================

static void MediaPrevious()
{
    try
    {
        if (g_session)
        {
            g_session
                .TrySkipPreviousAsync()
                .get();
        }
    }
    catch (...)
    {
    }
}

static void MediaNext()
{
    try
    {
        if (g_session)
        {
            g_session
                .TrySkipNextAsync()
                .get();
        }
    }
    catch (...)
    {
    }
}

static void MediaTogglePlayPause()
{
    try
    {
        if (g_session)
        {
            g_session
                .TryTogglePlayPauseAsync()
                .get();
        }
    }
    catch (...)
    {
    }
}

// ============================================================
// SEEK
// ============================================================

static void MediaSeek(
    double position
)
{
    try
    {
        if (!g_session)
            return;

        double duration;

        {
            std::scoped_lock lock(
                g_stateMutex
            );

            duration =
                g_duration;
        }

        if (position < 0.0)
            position = 0.0;

        if (
            duration > 0.0 &&
            position > duration
        )
        {
            position =
                duration;
        }

        g_session
            .TryChangePlaybackPositionAsync(
                static_cast<int64_t>(
                    position *
                    10000000.0
                )
            )
            .get();

        {
            std::scoped_lock lock(
                g_stateMutex
            );

            if (g_playing)
            {
                StartLocalClockLocked(
                    position
                );
            }
            else
            {
                FreezeLocalClockLocked(
                    position
                );
            }
        }
    }
    catch (...)
    {
    }
}

// ============================================================
// MEDIA SNAPSHOT
// ============================================================

struct MediaSnapshot
{
    bool valid = false;

    bool playing = false;

    std::wstring title;

    std::wstring artist;

    std::wstring album;

    double duration = 0.0;

    double position = 0.0;

    std::wstring coverKey;

    GlobalSystemMediaTransportControlsSessionMediaProperties
        props{nullptr};
};

// ============================================================
// QUERY MEDIA
// ============================================================

static bool QueryMedia(
    MediaSnapshot& snapshot
)
{
    try
    {
        if (!g_sessionManager)
        {
            g_sessionManager =
                GlobalSystemMediaTransportControlsSessionManager
                    ::RequestAsync()
                    .get();
        }

        if (!g_sessionManager)
            return false;

        auto session =
            g_sessionManager
                .GetCurrentSession();

        if (!session)
        {
            g_session =
                nullptr;

            snapshot.valid =
                false;

            return true;
        }

        g_session =
            session;

        auto playbackInfo =
            session
                .GetPlaybackInfo();

        if (!playbackInfo)
        {
            snapshot.valid =
                false;

            return true;
        }

        snapshot.playing =
            playbackInfo.PlaybackStatus() ==
            GlobalSystemMediaTransportControlsSessionPlaybackStatus
                ::Playing;

        auto props =
            session
                .TryGetMediaPropertiesAsync()
                .get();

        if (!props)
        {
            snapshot.valid =
                false;

            return true;
        }

        snapshot.props =
            props;

        snapshot.title =
            TrimString(
                SafeString(
                    props.Title()
                )
            );

        snapshot.artist =
            TrimString(
                SafeString(
                    props.Artist()
                )
            );

        snapshot.album =
            TrimString(
                SafeString(
                    props.AlbumTitle()
                )
            );

        if (snapshot.title.empty())
            snapshot.title =
                L"Unknown title";

        if (snapshot.artist.empty())
            snapshot.artist =
                L"Unknown artist";

        auto timeline =
            session
                .GetTimelineProperties();

        if (timeline)
        {
            snapshot.position =
                timeline
                    .Position()
                    .count() /
                10000000.0;

            const auto start =
                timeline.StartTime();

            const auto end =
                timeline.EndTime();

            const double duration =
                (end - start).count() /
                10000000.0;

            if (duration > 0.0)
            {
                snapshot.duration =
                    duration;
            }
        }

        snapshot.coverKey =
            snapshot.title +
            L"\n" +
            snapshot.artist +
            L"\n" +
            snapshot.album;

        snapshot.valid =
            true;

        return true;
    }
    catch (...)
    {
        return false;
    }
}

// ============================================================
// APPLY MEDIA
// ============================================================

static void ApplyMedia(
    const MediaSnapshot& snapshot
)
{
    bool needCover =
        false;

    std::wstring coverKey;

    {
        std::scoped_lock lock(
            g_stateMutex
        );

        const bool hadMedia =
            g_mediaValid;

        const bool oldPlaying =
            g_playing;

        const bool trackChanged =
            !hadMedia ||
            !SameText(
                g_title,
                snapshot.title
            ) ||
            !SameText(
                g_artist,
                snapshot.artist
            ) ||
            !SameText(
                g_album,
                snapshot.album
            );

        const bool playbackChanged =
            !hadMedia ||
            oldPlaying !=
                snapshot.playing;

        g_mediaValid =
            true;

        g_title =
            snapshot.title;

        g_artist =
            snapshot.artist;

        g_album =
            snapshot.album;

        if (snapshot.duration > 0.0)
        {
            g_duration =
                snapshot.duration;
        }

        //
        // Track changed.
        //

        if (trackChanged)
        {
            double position =
                snapshot.position;

            if (position < 0.0)
                position = 0.0;

            if (
                g_duration > 0.0 &&
                position > g_duration
            )
            {
                position =
                    g_duration;
            }

            if (snapshot.playing)
            {
                StartLocalClockLocked(
                    position
                );
            }
            else
            {
                FreezeLocalClockLocked(
                    position
                );
            }

            DestroyCoverLocked();

            ResetMarqueeLocked();

            needCover =
                true;

            coverKey =
                snapshot.coverKey;
        }

        //
        // Playing -> Paused.
        //

        else if (
            playbackChanged &&
            oldPlaying &&
            !snapshot.playing
        )
        {
            const double position =
                GetLocalPlaybackPositionLocked();

            FreezeLocalClockLocked(
                position
            );
        }

        //
        // Paused -> Playing.
        //

        else if (
            playbackChanged &&
            !oldPlaying &&
            snapshot.playing
        )
        {
            StartLocalClockLocked(
                g_frozenPosition
            );
        }

        //
        // IMPORTANT:
        //
        // Do not synchronize normal position polling.
        //

        g_playing =
            snapshot.playing;
    }

    g_visible.store(
        snapshot.playing
    );

    if (needCover)
    {
        UpdateCover(
            snapshot.props,
            coverKey
        );
    }
}

// ============================================================
// HANDLE NO MEDIA
// ============================================================

static void HandleNoMedia()
{
    {
        std::scoped_lock lock(
            g_stateMutex
        );

        if (!g_mediaValid)
            return;

        if (g_clockRunning)
        {
            const double position =
                GetLocalPlaybackPositionLocked();

            FreezeLocalClockLocked(
                position
            );
        }

        g_mediaValid =
            false;

        g_playing =
            false;

        g_title =
            L"No music";

        g_artist.clear();

        g_album.clear();

        g_duration =
            0.0;

        DestroyCoverLocked();

        ResetMarqueeLocked();
    }

    g_visible.store(
        false
    );
}

// ============================================================
// MEDIA THREAD
// ============================================================

static void MediaThreadProc()
{
    try
    {
        winrt::init_apartment(
            winrt::apartment_type::multi_threaded
        );
    }
    catch (...)
    {
    }

    while (
        g_running.load()
    )
    {
        MediaSnapshot snapshot;

        if (QueryMedia(snapshot))
        {
            if (snapshot.valid)
            {
                ApplyMedia(
                    snapshot
                );
            }
            else
            {
                HandleNoMedia();
            }
        }

        int waited =
            0;

        while (
            waited <
                MEDIA_POLL_MS &&
            g_running.load()
        )
        {
            Sleep(50);

            waited += 50;
        }
    }

    try
    {
        winrt::uninit_apartment();
    }
    catch (...)
    {
    }
}

// ============================================================
// DESKTOP STATE
// ============================================================

static bool IsDesktopActive()
{
    HWND foreground =
        GetForegroundWindow();

    if (!foreground)
        return false;

    //
    // Our overlay.
    //

    if (
        foreground ==
        g_hwnd
    )
    {
        return true;
    }

    //
    // Shell.
    //

    HWND shell =
        GetShellWindow();

    if (
        shell &&
        foreground == shell
    )
    {
        return true;
    }

    //
    // Root window.
    //

    HWND root =
        GetAncestor(
            foreground,
            GA_ROOT
        );

    if (!root)
        root =
            foreground;

    wchar_t className[128]{};

    GetClassNameW(
        root,
        className,
        ARRAYSIZE(className)
    );

    if (
        lstrcmpW(
            className,
            L"Progman"
        ) == 0
    )
    {
        return true;
    }

    if (
        lstrcmpW(
            className,
            L"WorkerW"
        ) == 0
    )
    {
        return true;
    }

    return false;
}

// ============================================================
// POSITION OVERLAY
// ============================================================

static void PositionOverlay()
{
    if (!g_hwnd)
        return;

    HMONITOR monitor =
        MonitorFromPoint(
            POINT{
                0,
                0
            },
            MONITOR_DEFAULTTOPRIMARY
        );

    if (!monitor)
        return;

    MONITORINFO mi{};

    mi.cbSize =
        sizeof(mi);

    if (
        !GetMonitorInfoW(
            monitor,
            &mi
        )
    )
    {
        return;
    }

    const int x =
        mi.rcWork.right -
        OVERLAY_W -
        SCREEN_MARGIN_RIGHT;

    const int y =
        mi.rcWork.bottom -
        OVERLAY_H -
        SCREEN_MARGIN_BOTTOM;

    SetWindowPos(
        g_hwnd,
        nullptr,
        x,
        y,
        OVERLAY_W,
        OVERLAY_H,
        SWP_NOACTIVATE |
        SWP_NOZORDER |
        SWP_NOSENDCHANGING
    );
}

// ============================================================
// Z ORDER
// ============================================================

static void UpdateOverlayZOrder(
    bool force
)
{
    if (!g_hwnd)
        return;

    const bool desktopActive =
        IsDesktopActive();

    if (
        !force &&
        g_haveZOrderState &&
        desktopActive ==
            g_lastDesktopActive
    )
    {
        return;
    }

    HWND insertAfter =
        desktopActive
            ? HWND_TOP
            : HWND_BOTTOM;

    SetWindowPos(
        g_hwnd,
        insertAfter,
        0,
        0,
        0,
        0,
        SWP_NOMOVE |
        SWP_NOSIZE |
        SWP_NOACTIVATE |
        SWP_NOOWNERZORDER |
        SWP_NOSENDCHANGING
    );

    g_lastDesktopActive =
        desktopActive;

    g_haveZOrderState =
        true;

    g_lastZOrderQpc =
        QpcNow();
}

// ============================================================
// ROUND RECT PATH
// ============================================================

static void RoundedRectPath(
    GraphicsPath& path,
    const RectF& rect,
    float radius
)
{
    float diameter =
        radius * 2.0f;

    if (diameter > rect.Width)
        diameter = rect.Width;

    if (diameter > rect.Height)
        diameter = rect.Height;

    path.AddArc(
        rect.X,
        rect.Y,
        diameter,
        diameter,
        180.0f,
        90.0f
    );

    path.AddArc(
        rect.GetRight() -
            diameter,
        rect.Y,
        diameter,
        diameter,
        270.0f,
        90.0f
    );

    path.AddArc(
        rect.GetRight() -
            diameter,
        rect.GetBottom() -
            diameter,
        diameter,
        diameter,
        0.0f,
        90.0f
    );

    path.AddArc(
        rect.X,
        rect.GetBottom() -
            diameter,
        diameter,
        diameter,
        90.0f,
        90.0f
    );

    path.CloseFigure();
}

// ============================================================
// FILL ROUND RECT
// ============================================================

static void FillRoundedRect(
    Graphics& graphics,
    const RectF& rect,
    float radius,
    Brush* brush
)
{
    if (!brush)
        return;

    GraphicsPath path;

    RoundedRectPath(
        path,
        rect,
        radius
    );

    graphics.FillPath(
        brush,
        &path
    );
}

// ============================================================
// BACKGROUND
// ============================================================

static void DrawBackground(
    Graphics& graphics
)
{
    graphics.SetSmoothingMode(
        SmoothingModeAntiAlias
    );

    GraphicsPath path;

    RoundedRectPath(
        path,
        RectF(
            0.5f,
            0.5f,
            OVERLAY_W - 1.0f,
            OVERLAY_H - 1.0f
        ),
        18.0f
    );

    SolidBrush background(
        Color(
            248,
            16,
            16,
            21
        )
    );

    graphics.FillPath(
        &background,
        &path
    );

    Pen border(
        Color(
            85,
            255,
            255,
            255
        ),
        1.0f
    );

    graphics.DrawPath(
        &border,
        &path
    );
}

// ============================================================
// COVER
// ============================================================

static void DrawCover(
    Graphics& graphics
)
{
    const RectF destination(
        static_cast<float>(
            COVER_X
        ),
        static_cast<float>(
            COVER_Y
        ),
        static_cast<float>(
            COVER_SIZE
        ),
        static_cast<float>(
            COVER_SIZE
        )
    );

    GraphicsPath clip;

    RoundedRectPath(
        clip,
        destination,
        12.0f
    );

    graphics.SetClip(
        &clip,
        CombineModeIntersect
    );

    graphics.SetInterpolationMode(
        InterpolationModeHighQualityBicubic
    );

    {
        std::scoped_lock lock(
            g_stateMutex
        );

        if (g_coverBitmap)
        {
            graphics.DrawImage(
                g_coverBitmap,
                destination
            );
        }
        else
        {
            SolidBrush placeholder(
                Color(
                    255,
                    43,
                    43,
                    49
                )
            );

            graphics.FillPath(
                &placeholder,
                &clip
            );

            SolidBrush noteBrush(
                Color(
                    220,
                    255,
                    255,
                    255
                )
            );

            Pen notePen(
                Color(
                    230,
                    255,
                    255,
                    255
                ),
                5.0f
            );

            graphics.DrawLine(
                &notePen,
                49.0f,
                35.0f,
                49.0f,
                64.0f
            );

            graphics.DrawLine(
                &notePen,
                49.0f,
                35.0f,
                67.0f,
                30.0f
            );

            graphics.FillEllipse(
                &noteBrush,
                38.0f,
                59.0f,
                16.0f,
                11.0f
            );
        }
    }

    graphics.ResetClip();
}

// ============================================================
// CLOCK
// ============================================================

static void DrawClock(
    Graphics& graphics
)
{
    SolidBrush panel(
        Color(
            255,
            28,
            28,
            35
        )
    );

    FillRoundedRect(
        graphics,
        RectF(
            CLOCK_X,
            CLOCK_Y,
            CLOCK_W,
            CLOCK_H
        ),
        10.0f,
        &panel
    );

    GraphicsPath path;

    RoundedRectPath(
        path,
        RectF(
            CLOCK_X + 0.5f,
            CLOCK_Y + 0.5f,
            CLOCK_W - 1.0f,
            CLOCK_H - 1.0f
        ),
        10.0f
    );

    Pen border(
        Color(
            55,
            255,
            255,
            255
        ),
        1.0f
    );

    graphics.DrawPath(
        &border,
        &path
    );

    const std::wstring clock =
        CurrentClock();

    Font clockFont(
        L"Segoe UI",
        16.0f,
        FontStyleBold,
        UnitPixel
    );

    SolidBrush white(
        Color(
            255,
            255,
            255,
            255
        )
    );

    StringFormat center;

    center.SetAlignment(
        StringAlignmentCenter
    );

    center.SetLineAlignment(
        StringAlignmentCenter
    );

    graphics.DrawString(
        clock.c_str(),
        -1,
        &clockFont,
        RectF(
            CLOCK_X,
            CLOCK_Y + 1.0f,
            CLOCK_W,
            21.0f
        ),
        &center,
        &white
    );

    const std::wstring date =
        CurrentDate();

    Font dateFont(
        L"Segoe UI",
        8.5f,
        FontStyleRegular,
        UnitPixel
    );

    SolidBrush dateBrush(
        Color(
            190,
            190,
            198,
            255
        )
    );

    graphics.DrawString(
        date.c_str(),
        -1,
        &dateFont,
        RectF(
            CLOCK_X,
            CLOCK_Y + 25.0f,
            CLOCK_W,
            13.0f
        ),
        &center,
        &dateBrush
    );
}

// ============================================================
// TITLE
// ============================================================

static void DrawTitle(
    Graphics& graphics
)
{
    std::wstring title;

    {
        std::scoped_lock lock(
            g_stateMutex
        );

        title =
            g_title;
    }

    Font font(
        L"Segoe UI",
        TITLE_FONT_SIZE,
        FontStyleBold,
        UnitPixel
    );

    SolidBrush brush(
        Color(
            255,
            255,
            255,
            255
        )
    );

    RectF area(
        static_cast<float>(
            CONTENT_X
        ),
        9.0f,
        static_cast<float>(
            TITLE_WIDTH
        ),
        22.0f
    );

    graphics.SetClip(
        area,
        CombineModeReplace
    );

    double offset;

    {
        std::scoped_lock lock(
            g_stateMutex
        );

        offset =
            g_marqueeOffset;
    }

    if (!g_cachedTitleOverflow)
    {
        graphics.DrawString(
            title.c_str(),
            -1,
            &font,
            PointF(
                area.X,
                area.Y
            ),
            &brush
        );
    }
    else
    {
        const float x =
            area.X +
            static_cast<float>(
                offset
            );

        graphics.DrawString(
            title.c_str(),
            -1,
            &font,
            PointF(
                x,
                area.Y
            ),
            &brush
        );

        graphics.DrawString(
            title.c_str(),
            -1,
            &font,
            PointF(
                x +
                static_cast<float>(
                    g_cachedTitleWidth +
                    MARQUEE_GAP
                ),
                area.Y
            ),
            &brush
        );
    }

    graphics.ResetClip();
}

// ============================================================
// ARTIST
// ============================================================

static void DrawArtist(
    Graphics& graphics
)
{
    std::wstring artist;

    {
        std::scoped_lock lock(
            g_stateMutex
        );

        artist =
            g_artist;
    }

    if (artist.empty())
        artist =
            L"Unknown artist";

    Font font(
        L"Segoe UI",
        ARTIST_FONT_SIZE,
        FontStyleRegular,
        UnitPixel
    );

    SolidBrush brush(
        Color(
            205,
            190,
            190,
            200
        )
    );

    RectF area(
        static_cast<float>(
            CONTENT_X
        ),
        34.0f,
        static_cast<float>(
            ARTIST_WIDTH
        ),
        17.0f
    );

    StringFormat format;

    format.SetTrimming(
        StringTrimmingEllipsisCharacter
    );

    graphics.DrawString(
        artist.c_str(),
        -1,
        &font,
        area,
        &format,
        &brush
    );
}

// ============================================================
// ALBUM
// ============================================================

static void DrawAlbum(
    Graphics& graphics
)
{
    std::wstring album;

    {
        std::scoped_lock lock(
            g_stateMutex
        );

        album =
            g_album;
    }

    if (album.empty())
        return;

    Font font(
        L"Segoe UI",
        ALBUM_FONT_SIZE,
        FontStyleRegular,
        UnitPixel
    );

    SolidBrush brush(
        Color(
            165,
            165,
            172,
            185
        )
    );

    RectF area(
        static_cast<float>(
            CONTENT_X
        ),
        49.0f,
        static_cast<float>(
            ALBUM_WIDTH
        ),
        14.0f
    );

    StringFormat format;

    format.SetTrimming(
        StringTrimmingEllipsisCharacter
    );

    graphics.DrawString(
        album.c_str(),
        -1,
        &font,
        area,
        &format,
        &brush
    );
}

// ============================================================
// PROGRESS
// ============================================================

static void DrawProgress(
    Graphics& graphics,
    double position,
    double duration
)
{
    float ratio =
        0.0f;

    if (duration > 0.0)
    {
        ratio =
            static_cast<float>(
                position /
                duration
            );

        ratio =
            std::clamp(
                ratio,
                0.0f,
                1.0f
            );
    }

    SolidBrush track(
        Color(
            70,
            255,
            255,
            255
        )
    );

    FillRoundedRect(
        graphics,
        RectF(
            PROGRESS_X,
            PROGRESS_Y,
            PROGRESS_W,
            PROGRESS_H
        ),
        2.5f,
        &track
    );

    if (ratio > 0.0f)
    {
        SolidBrush fill(
            Color(
                240,
                255,
                255,
                255
            )
        );

        FillRoundedRect(
            graphics,
            RectF(
                PROGRESS_X,
                PROGRESS_Y,
                PROGRESS_W *
                    ratio,
                PROGRESS_H
            ),
            2.5f,
            &fill
        );
    }

    const float knobX =
        PROGRESS_X +
        PROGRESS_W *
            ratio;

    SolidBrush knob(
        Color(
            255,
            255,
            255,
            255
        )
    );

    graphics.FillEllipse(
        &knob,
        knobX - 4.0f,
        PROGRESS_Y - 1.5f,
        8.0f,
        8.0f
    );
}

// ============================================================
// DURATION
// ============================================================

static void DrawDuration(
    Graphics& graphics,
    double position,
    double duration
)
{
    const std::wstring left =
        FormatTime(
            position
        );

    const std::wstring right =
        FormatTime(
            duration
        );

    Font font(
        L"Segoe UI",
        8.5f,
        FontStyleRegular,
        UnitPixel
    );

    SolidBrush brush(
        Color(
            185,
            180,
            180,
            190
        )
    );

    graphics.DrawString(
        left.c_str(),
        -1,
        &font,
        PointF(
            PROGRESS_X,
            72.0f
        ),
        &brush
    );

    StringFormat alignRight;

    alignRight.SetAlignment(
        StringAlignmentFar
    );

    graphics.DrawString(
        right.c_str(),
        -1,
        &font,
        RectF(
            PROGRESS_X,
            72.0f,
            PROGRESS_W,
            15.0f
        ),
        &alignRight,
        &brush
    );
}

// ============================================================
// VECTOR PLAY
// ============================================================

static void DrawPlayIcon(
    Graphics& graphics,
    float cx,
    float cy,
    float size,
    Brush* brush
)
{
    PointF points[3] = {
        PointF(
            cx - size * 0.30f,
            cy - size * 0.50f
        ),
        PointF(
            cx + size * 0.48f,
            cy
        ),
        PointF(
            cx - size * 0.30f,
            cy + size * 0.50f
        )
    };

    graphics.FillPolygon(
        brush,
        points,
        3
    );
}

// ============================================================
// VECTOR PAUSE
// ============================================================

static void DrawPauseIcon(
    Graphics& graphics,
    float cx,
    float cy,
    float size,
    Brush* brush
)
{
    const float width =
        size * 0.18f;

    const float height =
        size * 0.60f;

    graphics.FillRectangle(
        brush,
        cx - size * 0.28f,
        cy - height * 0.5f,
        width,
        height
    );

    graphics.FillRectangle(
        brush,
        cx + size * 0.10f,
        cy - height * 0.5f,
        width,
        height
    );
}

// ============================================================
// VECTOR PREVIOUS
// ============================================================

static void DrawPreviousIcon(
    Graphics& graphics,
    float cx,
    float cy,
    float size,
    Brush* brush
)
{
    const float width =
        size * 0.12f;

    const float height =
        size * 0.62f;

    graphics.FillRectangle(
        brush,
        cx + size * 0.20f,
        cy - height * 0.5f,
        width,
        height
    );

    PointF points[3] = {
        PointF(
            cx + size * 0.12f,
            cy - size * 0.42f
        ),
        PointF(
            cx - size * 0.42f,
            cy
        ),
        PointF(
            cx + size * 0.12f,
            cy + size * 0.42f
        )
    };

    graphics.FillPolygon(
        brush,
        points,
        3
    );
}

// ============================================================
// VECTOR NEXT
// ============================================================

static void DrawNextIcon(
    Graphics& graphics,
    float cx,
    float cy,
    float size,
    Brush* brush
)
{
    const float width =
        size * 0.12f;

    const float height =
        size * 0.62f;

    graphics.FillRectangle(
        brush,
        cx - size * 0.32f,
        cy - height * 0.5f,
        width,
        height
    );

    PointF points[3] = {
        PointF(
            cx - size * 0.10f,
            cy - size * 0.42f
        ),
        PointF(
            cx + size * 0.45f,
            cy
        ),
        PointF(
            cx - size * 0.10f,
            cy + size * 0.42f
        )
    };

    graphics.FillPolygon(
        brush,
        points,
        3
    );
}

// ============================================================
// MEDIA BUTTON
// ============================================================

static void DrawMediaButton(
    Graphics& graphics,
    float cx,
    float cy,
    float radius,
    int type,
    bool playing,
    bool active
)
{
    SolidBrush circle(
        Color(
            active ? 245 : 48,
            255,
            255,
            255
        )
    );

    graphics.FillEllipse(
        &circle,
        cx - radius,
        cy - radius,
        radius * 2.0f,
        radius * 2.0f
    );

    SolidBrush icon(
        Color(
            active ? 20 : 235,
            active ? 20 : 235,
            active ? 20 : 235,
            255
        )
    );

    const float size =
        radius * 1.35f;

    switch (type)
    {
        case 0:
            DrawPreviousIcon(
                graphics,
                cx,
                cy,
                size,
                &icon
            );
            break;

        case 1:
            if (playing)
            {
                DrawPauseIcon(
                    graphics,
                    cx,
                    cy,
                    size,
                    &icon
                );
            }
            else
            {
                DrawPlayIcon(
                    graphics,
                    cx,
                    cy,
                    size,
                    &icon
                );
            }
            break;

        case 2:
            DrawNextIcon(
                graphics,
                cx,
                cy,
                size,
                &icon
            );
            break;
    }
}

// ============================================================
// CONTROLS
// ============================================================

static void DrawControls(
    Graphics& graphics
)
{
    bool playing;

    {
        std::scoped_lock lock(
            g_stateMutex
        );

        playing =
            g_playing;
    }

    DrawMediaButton(
        graphics,
        PREV_X,
        CONTROL_Y,
        15.0f,
        0,
        playing,
        false
    );

    DrawMediaButton(
        graphics,
        PLAY_X,
        CONTROL_Y,
        18.0f,
        1,
        playing,
        true
    );

    DrawMediaButton(
        graphics,
        NEXT_X,
        CONTROL_Y,
        15.0f,
        2,
        playing,
        false
    );
}

// ============================================================
// TITLE METRICS
// ============================================================

static void UpdateTitleMetrics(
    Graphics& graphics
)
{
    std::wstring title;

    {
        std::scoped_lock lock(
            g_stateMutex
        );

        title =
            g_title;
    }

    Font font(
        L"Segoe UI",
        TITLE_FONT_SIZE,
        FontStyleBold,
        UnitPixel
    );

    RectF measured{};

    graphics.MeasureString(
        title.c_str(),
        -1,
        &font,
        PointF(
            0.0f,
            0.0f
        ),
        &measured
    );

    const int width =
        static_cast<int>(
            std::ceil(
                measured.Width
            )
        );

    if (
        !SameText(
            g_cachedTitle,
            title
        )
    )
    {
        g_cachedTitle =
            title;

        std::scoped_lock lock(
            g_stateMutex
        );

        ResetMarqueeLocked();
    }

    g_cachedTitleWidth =
        width;

    g_cachedTitleOverflow =
        width >
        TITLE_WIDTH;
}

// ============================================================
// BACK BUFFER
// ============================================================

static bool CreateBackBuffer()
{
    if (g_backBufferReady)
        return true;

    HDC screen =
        GetDC(nullptr);

    if (!screen)
        return false;

    g_backDC =
        CreateCompatibleDC(
            screen
        );

    if (!g_backDC)
    {
        ReleaseDC(
            nullptr,
            screen
        );

        return false;
    }

    BITMAPINFO bi{};

    bi.bmiHeader.biSize =
        sizeof(
            BITMAPINFOHEADER
        );

    bi.bmiHeader.biWidth =
        OVERLAY_W;

    bi.bmiHeader.biHeight =
        -OVERLAY_H;

    bi.bmiHeader.biPlanes =
        1;

    bi.bmiHeader.biBitCount =
        32;

    bi.bmiHeader.biCompression =
        BI_RGB;

    g_backBitmap =
        CreateDIBSection(
            screen,
            &bi,
            DIB_RGB_COLORS,
            &g_backBits,
            nullptr,
            0
        );

    ReleaseDC(
        nullptr,
        screen
    );

    if (!g_backBitmap)
    {
        DeleteDC(
            g_backDC
        );

        g_backDC =
            nullptr;

        return false;
    }

    g_backOldBitmap =
        static_cast<HBITMAP>(
            SelectObject(
                g_backDC,
                g_backBitmap
            )
        );

    g_backBufferReady =
        true;

    return true;
}

// ============================================================
// DESTROY BACK BUFFER
// ============================================================

static void DestroyBackBuffer()
{
    if (g_backDC)
    {
        if (g_backOldBitmap)
        {
            SelectObject(
                g_backDC,
                g_backOldBitmap
            );
        }

        if (g_backBitmap)
        {
            DeleteObject(
                g_backBitmap
            );
        }

        DeleteDC(
            g_backDC
        );
    }

    g_backDC =
        nullptr;

    g_backBitmap =
        nullptr;

    g_backOldBitmap =
        nullptr;

    g_backBits =
        nullptr;

    g_backBufferReady =
        false;
}

// ============================================================
// RENDER FRAME
// ============================================================

static void RenderFrame()
{
    if (!CreateBackBuffer())
        return;

    Graphics graphics(
        g_backDC
    );

    graphics.SetSmoothingMode(
        SmoothingModeAntiAlias
    );

    graphics.SetTextRenderingHint(
        TextRenderingHintClearTypeGridFit
    );

    graphics.SetInterpolationMode(
        InterpolationModeHighQualityBicubic
    );

    //
    // Clear transparent.
    //

    graphics.Clear(
        Color(
            0,
            0,
            0,
            0
        )
    );

    //
    // Draw complete UI into one buffer.
    //

    DrawBackground(
        graphics
    );

    DrawCover(
        graphics
    );

    UpdateTitleMetrics(
        graphics
    );

    DrawTitle(
        graphics
    );

    DrawArtist(
        graphics
    );

    DrawAlbum(
        graphics
    );

    DrawClock(
        graphics
    );

    double position;
    double duration;

    {
        std::scoped_lock lock(
            g_stateMutex
        );

        position =
            GetLocalPlaybackPositionLocked();

        duration =
            g_duration;
    }

    DrawProgress(
        graphics,
        position,
        duration
    );

    DrawDuration(
        graphics,
        position,
        duration
    );

    DrawControls(
        graphics
    );
}

// ============================================================
// PRESENT
// ============================================================

static void PresentFrame()
{
    if (!g_hwnd)
        return;

    if (!IsWindowVisible(
            g_hwnd
        ))
    {
        return;
    }

    if (!CreateBackBuffer())
        return;

    HDC screen =
        GetDC(nullptr);

    if (!screen)
        return;

    RECT windowRect{};

    GetWindowRect(
        g_hwnd,
        &windowRect
    );

    POINT destination{
        windowRect.left,
        windowRect.top
    };

    POINT source{
        0,
        0
    };

    SIZE size{
        OVERLAY_W,
        OVERLAY_H
    };

    BLENDFUNCTION blend{};

    blend.BlendOp =
        AC_SRC_OVER;

    blend.BlendFlags =
        0;

    blend.SourceConstantAlpha =
        255;

    blend.AlphaFormat =
        AC_SRC_ALPHA;

    UpdateLayeredWindow(
        g_hwnd,
        screen,
        &destination,
        &size,
        g_backDC,
        &source,
        0,
        &blend,
        ULW_ALPHA
    );

    ReleaseDC(
        nullptr,
        screen
    );
}

// ============================================================
// HIT TEST
// ============================================================

enum class HitArea
{
    None,
    Previous,
    PlayPause,
    Next,
    Progress
};

static HitArea HitTest(
    int x,
    int y
)
{
    auto insideCircle =
        [](
            int px,
            int py,
            int cx,
            int cy,
            int radius
        )
        {
            const int dx =
                px - cx;

            const int dy =
                py - cy;

            return
                dx * dx +
                dy * dy <=
                radius * radius;
        };

    if (
        insideCircle(
            x,
            y,
            static_cast<int>(
                PREV_X
            ),
            static_cast<int>(
                CONTROL_Y
            ),
            22
        )
    )
    {
        return HitArea::Previous;
    }

    if (
        insideCircle(
            x,
            y,
            static_cast<int>(
                PLAY_X
            ),
            static_cast<int>(
                CONTROL_Y
            ),
            25
        )
    )
    {
        return HitArea::PlayPause;
    }

    if (
        insideCircle(
            x,
            y,
            static_cast<int>(
                NEXT_X
            ),
            static_cast<int>(
                CONTROL_Y
            ),
            22
        )
    )
    {
        return HitArea::Next;
    }

    if (
        x >= static_cast<int>(
            PROGRESS_X
        ) &&
        x <= static_cast<int>(
            PROGRESS_X +
            PROGRESS_W
        ) &&
        y >= 52 &&
        y <= 80
    )
    {
        return HitArea::Progress;
    }

    return HitArea::None;
}

// ============================================================
// WINDOW PROC
// ============================================================

static LRESULT CALLBACK OverlayWndProc(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
)
{
    switch (message)
    {
        case WM_MOUSEACTIVATE:
        {
            return MA_NOACTIVATE;
        }

        case WM_NCHITTEST:
        {
            const int screenX =
                GET_X_LPARAM(
                    lParam
                );

            const int screenY =
                GET_Y_LPARAM(
                    lParam
                );

            RECT rect{};

            GetWindowRect(
                hwnd,
                &rect
            );

            const int x =
                screenX -
                rect.left;

            const int y =
                screenY -
                rect.top;

            const HitArea area =
                HitTest(
                    x,
                    y
                );

            if (
                area !=
                HitArea::None
            )
            {
                return HTCLIENT;
            }

            return HTTRANSPARENT;
        }

        case WM_LBUTTONDOWN:
        {
            const int x =
                GET_X_LPARAM(
                    lParam
                );

            const int y =
                GET_Y_LPARAM(
                    lParam
                );

            const HitArea area =
                HitTest(
                    x,
                    y
                );

            switch (area)
            {
                case HitArea::Previous:
                    MediaPrevious();
                    break;

                case HitArea::PlayPause:
                    MediaTogglePlayPause();
                    break;

                case HitArea::Next:
                    MediaNext();
                    break;

                case HitArea::Progress:
                {
                    double duration;

                    {
                        std::scoped_lock lock(
                            g_stateMutex
                        );

                        duration =
                            g_duration;
                    }

                    if (duration > 0.0)
                    {
                        double ratio =
                            static_cast<double>(
                                x -
                                static_cast<int>(
                                    PROGRESS_X
                                )
                            ) /
                            PROGRESS_W;

                        ratio =
                            std::clamp(
                                ratio,
                                0.0,
                                1.0
                            );

                        MediaSeek(
                            duration *
                            ratio
                        );
                    }

                    break;
                }

                default:
                    break;
            }

            return 0;
        }

        case WM_DISPLAYCHANGE:
        case WM_SETTINGCHANGE:
        {
            PositionOverlay();

            g_haveZOrderState =
                false;

            RenderFrame();

            PresentFrame();

            return 0;
        }

        case WM_CLOSE:
        {
            DestroyWindow(
                hwnd
            );

            return 0;
        }

        case WM_DESTROY:
        {
            PostQuitMessage(
                0
            );

            return 0;
        }

        default:
            break;
    }

    return DefWindowProcW(
        hwnd,
        message,
        wParam,
        lParam
    );
}

// ============================================================
// UI THREAD
// ============================================================

static void UIThreadProc()
{
    try
    {
        winrt::init_apartment(
            winrt::apartment_type::multi_threaded
        );
    }
    catch (...)
    {
    }

    //
    // Register class.
    //

    WNDCLASSEXW wc{};

    wc.cbSize =
        sizeof(wc);

    wc.lpfnWndProc =
        OverlayWndProc;

    wc.hInstance =
        GetModuleHandleW(
            nullptr
        );

    wc.lpszClassName =
        L"MallssMusicOverlay141";

    wc.hCursor =
        LoadCursorW(
            nullptr,
            IDC_ARROW
        );

    wc.hbrBackground =
        nullptr;

    ATOM atom =
        RegisterClassExW(
            &wc
        );

    if (
        atom == 0 &&
        GetLastError() !=
            ERROR_CLASS_ALREADY_EXISTS
    )
    {
        Wh_Log(
            L"[UI] RegisterClassEx failed: %lu",
            GetLastError()
        );

        try
        {
            winrt::uninit_apartment();
        }
        catch (...)
        {
        }

        return;
    }

    //
    // Normal popup.
    //
    // No WS_CHILD.
    // No HWND_TOPMOST.
    //

    g_hwnd =
        CreateWindowExW(
            WS_EX_TOOLWINDOW |
            WS_EX_LAYERED |
            WS_EX_NOACTIVATE |
            WS_EX_NOREDIRECTIONBITMAP,
            wc.lpszClassName,
            L"",
            WS_POPUP,
            0,
            0,
            OVERLAY_W,
            OVERLAY_H,
            nullptr,
            nullptr,
            wc.hInstance,
            nullptr
        );

    if (!g_hwnd)
    {
        //
        // Retry without WS_EX_NOREDIRECTIONBITMAP.
        //

        g_hwnd =
            CreateWindowExW(
                WS_EX_TOOLWINDOW |
                WS_EX_LAYERED |
                WS_EX_NOACTIVATE,
                wc.lpszClassName,
                L"",
                WS_POPUP,
                0,
                0,
                OVERLAY_W,
                OVERLAY_H,
                nullptr,
                nullptr,
                wc.hInstance,
                nullptr
            );
    }

    if (!g_hwnd)
    {
        Wh_Log(
            L"[UI] CreateWindowEx failed: %lu",
            GetLastError()
        );

        try
        {
            winrt::uninit_apartment();
        }
        catch (...)
        {
        }

        return;
    }

    //
    // Rounded region.
    //

    HRGN region =
        CreateRoundRectRgn(
            0,
            0,
            OVERLAY_W + 1,
            OVERLAY_H + 1,
            36,
            36
        );

    if (region)
    {
        SetWindowRgn(
            g_hwnd,
            region,
            TRUE
        );
    }

    //
    // Create buffer.
    //

    if (!CreateBackBuffer())
    {
        Wh_Log(
            L"[UI] CreateBackBuffer failed."
        );
    }

    //
    // Initial render.
    //

    RenderFrame();

    //
    // Initial position.
    //

    PositionOverlay();

    //
    // Hidden until music plays.
    //

    ShowWindow(
        g_hwnd,
        SW_HIDE
    );

    Wh_Log(
        L"[UI] Overlay created hwnd=%p",
        g_hwnd
    );

    MSG msg{};

    int64_t lastFrame =
        QpcNow();

    int64_t lastZOrder =
        QpcNow();

    while (
        g_running.load()
    )
    {
        //
        // Process pending messages.
        //

        while (
            PeekMessageW(
                &msg,
                nullptr,
                0,
                0,
                PM_REMOVE
            )
        )
        {
            if (
                msg.message ==
                WM_QUIT
            )
            {
                g_running.store(
                    false
                );

                break;
            }

            TranslateMessage(
                &msg
            );

            DispatchMessageW(
                &msg
            );
        }

        if (!g_running.load())
            break;

        //
        // Music visibility only.
        //

        const bool shouldShow =
            g_visible.load();

        if (shouldShow)
        {
            if (
                !IsWindowVisible(
                    g_hwnd
                )
            )
            {
                PositionOverlay();

                ShowWindow(
                    g_hwnd,
                    SW_SHOWNOACTIVATE
                );

                g_haveZOrderState =
                    false;
            }
        }
        else
        {
            if (
                IsWindowVisible(
                    g_hwnd
                )
            )
            {
                ShowWindow(
                    g_hwnd,
                    SW_HIDE
                );
            }
        }

        //
        // Z order only when necessary.
        //

        const int64_t now =
            QpcNow();

        if (
            SecondsSince(
                lastZOrder
            ) >=
            (
                static_cast<double>(
                    ZORDER_CHECK_MS
                ) /
                1000.0
            )
        )
        {
            lastZOrder =
                now;

            if (shouldShow)
            {
                UpdateOverlayZOrder(
                    false
                );
            }
        }

        //
        // Current minute.
        //

        const int minuteKey =
            CurrentMinuteKey();

        bool clockChanged =
            minuteKey !=
            g_cachedMinuteKey;

        if (clockChanged)
        {
            g_cachedMinuteKey =
                minuteKey;
        }

        //
        // Frame delta.
        //

        double dt =
            SecondsSince(
                lastFrame
            );

        lastFrame =
            now;

        if (dt > 0.1)
            dt = 0.1;

        //
        // Marquee.
        //

        if (
            shouldShow &&
            g_cachedTitleOverflow
        )
        {
            std::scoped_lock lock(
                g_stateMutex
            );

            g_marqueeTimer +=
                dt;

            switch (
                g_marqueePhase
            )
            {
                case MarqueePhase::HoldStart:
                {
                    g_marqueeOffset =
                        0.0;

                    if (
                        g_marqueeTimer >=
                        MARQUEE_START_HOLD
                    )
                    {
                        g_marqueeTimer =
                            0.0;

                        g_marqueePhase =
                            MarqueePhase::Scrolling;
                    }

                    break;
                }

                case MarqueePhase::Scrolling:
                {
                    g_marqueeOffset -=
                        MARQUEE_SPEED *
                        dt;

                    const double maxScroll =
                        static_cast<double>(
                            g_cachedTitleWidth -
                            TITLE_WIDTH
                        );

                    if (
                        -g_marqueeOffset >=
                        maxScroll
                    )
                    {
                        g_marqueeOffset =
                            -maxScroll;

                        g_marqueeTimer =
                            0.0;

                        g_marqueePhase =
                            MarqueePhase::HoldEnd;
                    }

                    break;
                }

                case MarqueePhase::HoldEnd:
                {
                    if (
                        g_marqueeTimer >=
                        MARQUEE_END_HOLD
                    )
                    {
                        g_marqueeTimer =
                            0.0;

                        g_marqueeOffset =
                            0.0;

                        g_marqueePhase =
                            MarqueePhase::HoldStart;
                    }

                    break;
                }
            }
        }

        //
        // Re-render.
        //

        if (
            shouldShow ||
            clockChanged
        )
        {
            if (shouldShow)
            {
                RenderFrame();

                PresentFrame();
            }
        }

        Sleep(
            FRAME_MS
        );
    }

    //
    // Cleanup.
    //

    if (g_hwnd)
    {
        DestroyWindow(
            g_hwnd
        );

        g_hwnd =
            nullptr;
    }

    DestroyBackBuffer();

    try
    {
        winrt::uninit_apartment();
    }
    catch (...)
    {
    }
}

// ============================================================
// SHELL PROCESS SEARCH
// ============================================================

struct ShellSearch
{
    DWORD pid = 0;
};

static BOOL CALLBACK FindShellProcessProc(
    HWND hwnd,
    LPARAM lParam
)
{
    ShellSearch* search =
        reinterpret_cast<
            ShellSearch*
        >(
            lParam
        );

    if (!search)
        return TRUE;

    //
    // Look for actual desktop view.
    //

    HWND desktopView =
        FindWindowExW(
            hwnd,
            nullptr,
            L"SHELLDLL_DefView",
            nullptr
        );

    if (!desktopView)
        return TRUE;

    DWORD pid =
        0;

    GetWindowThreadProcessId(
        hwnd,
        &pid
    );

    if (pid == 0)
        return TRUE;

    search->pid =
        pid;

    return FALSE;
}

// ============================================================
// GET DESKTOP SHELL PID
// ============================================================

static DWORD GetDesktopShellPid()
{
    ShellSearch search{};

    EnumWindows(
        FindShellProcessProc,
        reinterpret_cast<LPARAM>(
            &search
        )
    );

    //
    // Fallback 1.
    //

    if (search.pid == 0)
    {
        HWND shell =
            GetShellWindow();

        if (shell)
        {
            GetWindowThreadProcessId(
                shell,
                &search.pid
            );
        }
    }

    //
    // Fallback 2.
    //

    if (search.pid == 0)
    {
        HWND progman =
            FindWindowW(
                L"Progman",
                nullptr
            );

        if (progman)
        {
            GetWindowThreadProcessId(
                progman,
                &search.pid
            );
        }
    }

    return search.pid;
}

// ============================================================
// SINGLETON
// ============================================================

static bool AcquireSingleton()
{
    g_singletonMutex =
        CreateMutexW(
            nullptr,
            TRUE,
            L"Local\\MallssMusicOverlay.Singleton"
        );

    if (!g_singletonMutex)
    {
        Wh_Log(
            L"[Guard] CreateMutex failed: %lu",
            GetLastError()
        );

        return false;
    }

    if (
        GetLastError() ==
        ERROR_ALREADY_EXISTS
    )
    {
        Wh_Log(
            L"[Guard] Another active Mallss Music Overlay exists."
        );

        CloseHandle(
            g_singletonMutex
        );

        g_singletonMutex =
            nullptr;

        return false;
    }

    Wh_Log(
        L"[Guard] Singleton acquired."
    );

    return true;
}

// ============================================================
// GDI+
// ============================================================

static bool InitGdiplus()
{
    GdiplusStartupInput input{};

    const Status status =
        GdiplusStartup(
            &g_gdiplusToken,
            &input,
            nullptr
        );

    return status == Ok;
}

static void ShutdownGdiplus()
{
    if (g_gdiplusToken)
    {
        GdiplusShutdown(
            g_gdiplusToken
        );

        g_gdiplusToken =
            0;
    }
}

// ============================================================
// WINDHAWK INIT
// ============================================================

BOOL Wh_ModInit()
{
    //
    // Determine actual desktop Shell Explorer.
    //

    const DWORD currentPid =
        GetCurrentProcessId();

    const DWORD desktopShellPid =
        GetDesktopShellPid();

    Wh_Log(
        L"[Guard] Current PID=%lu | Desktop Shell PID=%lu",
        currentPid,
        desktopShellPid
    );

    //
    // Only the actual desktop Explorer.
    //

    if (
        desktopShellPid == 0 ||
        currentPid != desktopShellPid
    )
    {
        Wh_Log(
            L"[Guard] Not desktop Shell Explorer. Skipping."
        );

        g_activeMod =
            false;

        return FALSE;
    }

    //
    // One active instance.
    //

    if (!AcquireSingleton())
    {
        g_activeMod =
            false;

        return FALSE;
    }

    g_activeMod =
        true;

    //
    // Reset.
    //

    g_running.store(
        true
    );

    g_playing.store(
        false
    );

    g_visible.store(
        false
    );

    g_cachedTitleWidth =
        0;

    g_cachedTitleOverflow =
        false;

    g_cachedTitle.clear();

    g_cachedMinuteKey =
        -1;

    g_haveZOrderState =
        false;

    //
    // GDI+.
    //

    if (!InitGdiplus())
    {
        Wh_Log(
            L"[Init] GDI+ initialization failed."
        );

        if (g_singletonMutex)
        {
            ReleaseMutex(
                g_singletonMutex
            );

            CloseHandle(
                g_singletonMutex
            );

            g_singletonMutex =
                nullptr;
        }

        g_activeMod =
            false;

        return FALSE;
    }

    //
    // Reset media state.
    //

    {
        std::scoped_lock lock(
            g_stateMutex
        );

        g_mediaValid =
            false;

        g_title =
            L"No music";

        g_artist.clear();

        g_album.clear();

        g_duration =
            0.0;

        g_clockAnchorPosition =
            0.0;

        g_clockAnchorQpc =
            QpcNow();

        g_clockRunning =
            false;

        g_frozenPosition =
            0.0;

        ResetMarqueeLocked();

        DestroyCoverLocked();
    }

    //
    // Start UI.
    //

    g_uiThread =
        std::thread(
            UIThreadProc
        );

    //
    // Start media worker.
    //

    g_mediaThread =
        std::thread(
            MediaThreadProc
        );

    Wh_Log(
        L"Mallss Music Overlay 1.4.1 initialized."
    );

    return TRUE;
}

// ============================================================
// WINDHAWK UNINIT
// ============================================================

void Wh_ModUninit()
{
    if (!g_activeMod)
        return;

    Wh_Log(
        L"Mallss Music Overlay 1.4.1 shutting down."
    );

    //
    // Stop.
    //

    g_running.store(
        false
    );

    //
    // Wake UI.
    //

    if (g_hwnd)
    {
        PostMessageW(
            g_hwnd,
            WM_CLOSE,
            0,
            0
        );
    }

    //
    // Media thread.
    //

    if (
        g_mediaThread.joinable()
    )
    {
        g_mediaThread.join();
    }

    //
    // UI thread.
    //

    if (
        g_uiThread.joinable()
    )
    {
        g_uiThread.join();
    }

    //
    // Cover.
    //

    {
        std::scoped_lock lock(
            g_stateMutex
        );

        DestroyCoverLocked();
    }

    //
    // Media.
    //

    g_session =
        nullptr;

    g_sessionManager =
        nullptr;

    //
    // Buffer.
    //

    DestroyBackBuffer();

    //
    // GDI+.
    //

    ShutdownGdiplus();

    //
    // Singleton.
    //

    if (g_singletonMutex)
    {
        ReleaseMutex(
            g_singletonMutex
        );

        CloseHandle(
            g_singletonMutex
        );

        g_singletonMutex =
            nullptr;
    }

    g_activeMod =
        false;

    Wh_Log(
        L"Mallss Music Overlay 1.4.1 stopped."
    );
}
