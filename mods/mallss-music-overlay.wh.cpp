// ==WindhawkMod==
// @id              mallss-music-overlay
// @name            Mallss Music Overlay
// @description     Compact desktop music overlay with album art, clock, marquee title, progress and media controls.
// @version         1.5.1
// @author          Mallss
// @github          https://github.com/ItsMeMal
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -lshell32 -lgdiplus -lgdi32 -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Mallss Music Overlay

A compact desktop music overlay for Windows using Windows Global System
Media Transport Controls.

The overlay is designed specifically for the desktop rather than the
taskbar. It stays behind normal applications and becomes visible again
when the desktop is active.

## Features

- Album artwork
- Song title
- Artist
- Album
- Scrolling marquee for long titles
- Playback progress
- Elapsed and total duration
- Previous track
- Play / pause
- Next track
- Click-to-seek progress bar
- Desktop clock
- Current date
- Windows Media Session integration
- Local monotonic playback clock
- Safe media command queue
- Dedicated Windhawk tool process
- Double-buffered layered rendering
- DPI-aware rendering

## Desktop behavior

The overlay remains alive while media is available.

When another application is in the foreground, the overlay remains
alive but is moved behind normal application windows.

When the desktop becomes active, the overlay is moved above normal
desktop windows again.

The overlay is not injected into Explorer. It runs as a dedicated
Windhawk tool process.

## Playback timing

During playback, elapsed time is driven by a local monotonic clock.
Windows Media Session timeline data is used for initial positioning,
track changes and explicit seek operations.

Normal media polling does not continuously overwrite the local playback
clock, preventing the elapsed time from jumping backwards.

## Controls

- Previous button: previous track
- Center button: play / pause
- Next button: next track
- Progress bar: click to seek

## Compatibility

The mod uses Windows Global System Media Transport Controls.

It can work with media applications that expose their playback session
through Windows System Media Transport Controls.

## Difference from taskbar media mods

Mallss Music Overlay is a desktop-first floating overlay.

It combines media controls with a compact clock and date display and is
positioned near the bottom-right of the desktop.

It is not intended to replace taskbar media controls.

## Notes

A catalog screenshot or GIF should be added once hosted at an allowed
location such as raw.githubusercontent.com or i.imgur.com.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <gdiplus.h>

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <cstdint>
#include <cwctype>
#include <deque>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <vector>

using namespace Gdiplus;
using namespace winrt;

using namespace Windows::Foundation;
using namespace Windows::Media::Control;
using namespace Windows::Storage::Streams;

using namespace std::chrono_literals;

// ============================================================
// CONFIG
// ============================================================

static constexpr int BASE_WIDTH = 420;
static constexpr int BASE_HEIGHT = 164;

static constexpr int RIGHT_MARGIN = 18;
static constexpr int BOTTOM_MARGIN = 18;

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

static constexpr int MEDIA_POLL_MS = 250;
static constexpr int COMMAND_TIMEOUT_MS = 800;

// ============================================================
// FORWARD DECLARATIONS
// ============================================================

static void UIThreadProc();
static void MediaThreadProc();

static void RenderFrame();
static void PresentFrame();

static void PositionOverlay();
static void UpdateOverlayZOrder(bool force);

static bool UpdateCover(
    const GlobalSystemMediaTransportControlsSessionMediaProperties& props,
    const std::wstring& key
);

static bool LoadCoverFromThumbnail(
    const GlobalSystemMediaTransportControlsSessionMediaProperties& props,
    const std::wstring& key
);

static void QueuePrevious();
static void QueuePlayPause();
static void QueueNext();
static void QueueSeek(double position);

// ============================================================
// GLOBAL
// ============================================================

static HINSTANCE g_hInstance = nullptr;
static HWND g_hwnd = nullptr;
static HWINEVENTHOOK g_foregroundHook = nullptr;

static std::atomic<bool> g_running{true};
static std::atomic<bool> g_visible{false};
static std::atomic<bool> g_playing{false};
static std::atomic<bool> g_forceRedraw{true};

// ============================================================
// THREADS
// ============================================================

[[clang::no_destroy]]
static std::optional<std::thread> g_uiThread;

[[clang::no_destroy]]
static std::optional<std::thread> g_mediaThread;

// ============================================================
// COMMAND QUEUE
// ============================================================

enum class CommandType
{
    Previous,
    PlayPause,
    Next,
    Seek
};

struct MediaCommand
{
    CommandType type =
        CommandType::PlayPause;

    double position = 0.0;
};

[[clang::no_destroy]]
static std::mutex g_commandMutex;

[[clang::no_destroy]]
static std::condition_variable g_commandCv;

[[clang::no_destroy]]
static std::deque<MediaCommand> g_commandQueue;

// ============================================================
// MEDIA STATE
// ============================================================

static std::mutex g_stateMutex;

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
// WINRT
// ============================================================

[[clang::no_destroy]]
static GlobalSystemMediaTransportControlsSessionManager
    g_sessionManager{nullptr};

[[clang::no_destroy]]
static GlobalSystemMediaTransportControlsSession
    g_session{nullptr};

// ============================================================
// COVER
// ============================================================

[[clang::no_destroy]]
static Bitmap* g_coverBitmap = nullptr;

static std::wstring g_coverKey;

// ============================================================
// GDI+
// ============================================================

[[clang::no_destroy]]
static ULONG_PTR g_gdiplusToken = 0;

// ============================================================
// BACK BUFFER
// ============================================================

[[clang::no_destroy]]
static HDC g_backDC = nullptr;

[[clang::no_destroy]]
static HBITMAP g_backBitmap = nullptr;

[[clang::no_destroy]]
static HBITMAP g_backOldBitmap = nullptr;

[[clang::no_destroy]]
static void* g_backBits = nullptr;

static int g_bufferWidth = 0;
static int g_bufferHeight = 0;

// ============================================================
// DPI
// ============================================================

static UINT g_dpi = 96;
static float g_scale = 1.0f;

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
// Z ORDER STATE
// ============================================================

static bool g_lastDesktopActive = false;
static bool g_haveZOrderState = false;

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

static double QpcSecondsSince(
    int64_t start
)
{
    if (start == 0)
        return 0.0;

    static const double frequency = []()
    {
        LARGE_INTEGER value{};

        if (!QueryPerformanceFrequency(&value))
            return 1.0;

        return static_cast<double>(
            value.QuadPart
        );
    }();

    return static_cast<double>(
        QpcNow() - start
    ) / frequency;
}

// ============================================================
// MODULE HANDLE
// ============================================================

static HINSTANCE GetCurrentModuleHandle()
{
    HINSTANCE instance = nullptr;

    GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(
            &GetCurrentModuleHandle
        ),
        &instance
    );

    return instance;
}

// ============================================================
// STRING HELPERS
// ============================================================

static std::wstring TrimString(
    const std::wstring& value
)
{
    size_t first = 0;
    size_t last = value.size();

    while (
        first < last &&
        iswspace(value[first])
    )
    {
        ++first;
    }

    while (
        last > first &&
        iswspace(value[last - 1])
    )
    {
        --last;
    }

    return value.substr(
        first,
        last - first
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

// ============================================================
// LOCAL CLOCK
// ============================================================

static double GetLocalPlaybackPositionLocked()
{
    if (!g_clockRunning)
        return g_frozenPosition;

    const double elapsed =
        QpcSecondsSince(
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
// COVER
// ============================================================

static void DestroyCoverLocked()
{
    delete g_coverBitmap;

    g_coverBitmap =
        nullptr;

    g_coverKey.clear();
}

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

        auto operation =
            thumbnail.OpenReadAsync();

        if (
            operation.wait_for(
                1500ms
            ) != AsyncStatus::Completed
        )
        {
            return false;
        }

        auto stream =
            operation.get();

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

        auto loadOperation =
            reader.LoadAsync(
                size
            );

        if (
            loadOperation.wait_for(
                1500ms
            ) != AsyncStatus::Completed
        )
        {
            return false;
        }

        loadOperation.get();

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
// COMMAND QUEUE
// ============================================================

static void QueueCommand(
    const MediaCommand& command
)
{
    {
        std::scoped_lock lock(
            g_commandMutex
        );

        if (
            g_commandQueue.size() >= 8
        )
        {
            g_commandQueue.pop_front();
        }

        g_commandQueue.push_back(
            command
        );
    }

    g_commandCv.notify_one();
}

static void QueuePrevious()
{
    QueueCommand(
        MediaCommand{
            CommandType::Previous,
            0.0
        }
    );
}

static void QueuePlayPause()
{
    QueueCommand(
        MediaCommand{
            CommandType::PlayPause,
            0.0
        }
    );
}

static void QueueNext()
{
    QueueCommand(
        MediaCommand{
            CommandType::Next,
            0.0
        }
    );
}

static void QueueSeek(
    double position
)
{
    QueueCommand(
        MediaCommand{
            CommandType::Seek,
            position
        }
    );
}

// ============================================================
// COMMAND EXECUTION
// ============================================================

static void ExecuteMediaCommand(
    const MediaCommand& command
)
{
    try
    {
        auto session =
            g_session;

        if (!session)
            return;

        switch (command.type)
        {
            case CommandType::Previous:
            {
                auto operation =
                    session
                        .TrySkipPreviousAsync();

                if (
                    operation.wait_for(
                        std::chrono::milliseconds(
                            COMMAND_TIMEOUT_MS
                        )
                    ) ==
                    AsyncStatus::Completed
                )
                {
                    operation.get();
                }

                break;
            }

            case CommandType::PlayPause:
            {
                auto operation =
                    session
                        .TryTogglePlayPauseAsync();

                if (
                    operation.wait_for(
                        std::chrono::milliseconds(
                            COMMAND_TIMEOUT_MS
                        )
                    ) ==
                    AsyncStatus::Completed
                )
                {
                    operation.get();
                }

                break;
            }

            case CommandType::Next:
            {
                auto operation =
                    session
                        .TrySkipNextAsync();

                if (
                    operation.wait_for(
                        std::chrono::milliseconds(
                            COMMAND_TIMEOUT_MS
                        )
                    ) ==
                    AsyncStatus::Completed
                )
                {
                    operation.get();
                }

                break;
            }

            case CommandType::Seek:
            {
                double duration;

                {
                    std::scoped_lock lock(
                        g_stateMutex
                    );

                    duration =
                        g_duration;
                }

                double target =
                    command.position;

                if (target < 0.0)
                    target = 0.0;

                if (
                    duration > 0.0 &&
                    target > duration
                )
                {
                    target =
                        duration;
                }

                auto operation =
                    session
                        .TryChangePlaybackPositionAsync(
                            static_cast<int64_t>(
                                target *
                                10000000.0
                            )
                        );

                if (
                    operation.wait_for(
                        std::chrono::milliseconds(
                            COMMAND_TIMEOUT_MS
                        )
                    ) ==
                    AsyncStatus::Completed
                )
                {
                    operation.get();

                    std::scoped_lock lock(
                        g_stateMutex
                    );

                    if (g_playing)
                    {
                        StartLocalClockLocked(
                            target
                        );
                    }
                    else
                    {
                        FreezeLocalClockLocked(
                            target
                        );
                    }
                }

                break;
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

    GlobalSystemMediaTransportControlsSessionPlaybackStatus
        playbackStatus =
            GlobalSystemMediaTransportControlsSessionPlaybackStatus
                ::Closed;

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
            auto operation =
                GlobalSystemMediaTransportControlsSessionManager
                    ::RequestAsync();

            if (
                operation.wait_for(
                    1500ms
                ) != AsyncStatus::Completed
            )
            {
                return false;
            }

            g_sessionManager =
                operation.get();
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

        auto playback =
            session.GetPlaybackInfo();

        if (!playback)
        {
            snapshot.valid =
                false;

            return true;
        }

        snapshot.playbackStatus =
            playback.PlaybackStatus();

        snapshot.playing =
            snapshot.playbackStatus ==
            GlobalSystemMediaTransportControlsSessionPlaybackStatus
                ::Playing;

        //
        // Keep panel for Playing and Paused.
        //

        if (
            snapshot.playbackStatus ==
                GlobalSystemMediaTransportControlsSessionPlaybackStatus
                    ::Closed ||
            snapshot.playbackStatus ==
                GlobalSystemMediaTransportControlsSessionPlaybackStatus
                    ::Stopped
        )
        {
            snapshot.valid =
                false;

            return true;
        }

        auto propertiesOperation =
            session
                .TryGetMediaPropertiesAsync();

        if (
            propertiesOperation.wait_for(
                1500ms
            ) != AsyncStatus::Completed
        )
        {
            return false;
        }

        auto props =
            propertiesOperation.get();

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
                timeline.Position().count() /
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

        if (trackChanged)
        {
            g_duration =
                snapshot.duration;

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
        else if (
            snapshot.duration > 0.0
        )
        {
            g_duration =
                snapshot.duration;
        }

        //
        // Playing -> Paused
        //

        if (
            playbackChanged &&
            oldPlaying &&
            !snapshot.playing
        )
        {
            const double current =
                GetLocalPlaybackPositionLocked();

            FreezeLocalClockLocked(
                current
            );
        }

        //
        // Paused -> Playing
        //

        if (
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
        // Never continuously synchronize position
        // during normal polling.
        //

        g_playing =
            snapshot.playing;
    }

    //
    // IMPORTANT:
    //
    // Keep the panel alive while paused.
    //

    g_visible.store(
        true
    );

    if (needCover)
    {
        UpdateCover(
            snapshot.props,
            coverKey
        );
    }

    if (g_hwnd)
    {
        PostMessageW(
            g_hwnd,
            WM_APP + 20,
            0,
            0
        );
    }
}

// ============================================================
// NO MEDIA
// ============================================================

static void HandleNoMedia()
{
    bool changed =
        false;

    {
        std::scoped_lock lock(
            g_stateMutex
        );

        if (!g_mediaValid)
            return;

        if (g_clockRunning)
        {
            const double current =
                GetLocalPlaybackPositionLocked();

            FreezeLocalClockLocked(
                current
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

        changed =
            true;
    }

    if (changed)
    {
        g_visible.store(
            false
        );

        if (g_hwnd)
        {
            PostMessageW(
                g_hwnd,
                WM_APP + 20,
                0,
                0
            );
        }
    }
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
        MediaCommand command{};

        bool haveCommand =
            false;

        {
            std::unique_lock lock(
                g_commandMutex
            );

            g_commandCv.wait_for(
                lock,
                std::chrono::milliseconds(
                    MEDIA_POLL_MS
                ),
                []
                {
                    return
                        !g_running.load() ||
                        !g_commandQueue.empty();
                }
            );

            if (
                !g_commandQueue.empty()
            )
            {
                command =
                    g_commandQueue.front();

                g_commandQueue.pop_front();

                haveCommand =
                    true;
            }
        }

        if (!g_running.load())
            break;

        if (haveCommand)
        {
            ExecuteMediaCommand(
                command
            );
        }

        MediaSnapshot snapshot;

        if (
            QueryMedia(
                snapshot
            )
        )
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
    }

    {
        std::scoped_lock lock(
            g_commandMutex
        );

        g_commandQueue.clear();
    }

    g_session =
        nullptr;

    g_sessionManager =
        nullptr;

    try
    {
        winrt::uninit_apartment();
    }
    catch (...)
    {
    }
}

// ============================================================
// DPI
// ============================================================

static void UpdateDpiForWindow()
{
    if (!g_hwnd)
        return;

    UINT dpi =
        GetDpiForWindow(
            g_hwnd
        );

    if (dpi == 0)
        dpi = 96;

    g_dpi =
        dpi;

    g_scale =
        static_cast<float>(
            dpi
        ) / 96.0f;
}

static int ScaledInt(
    int value
)
{
    return static_cast<int>(
        std::round(
            value *
            g_scale
        )
    );
}

static float S(
    float value
)
{
    return value *
        g_scale;
}

// ============================================================
// BACK BUFFER
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

    g_bufferWidth =
        0;

    g_bufferHeight =
        0;
}

static bool CreateBackBuffer()
{
    const int width =
        ScaledInt(
            BASE_WIDTH
        );

    const int height =
        ScaledInt(
            BASE_HEIGHT
        );

    if (
        g_backDC &&
        g_bufferWidth ==
            width &&
        g_bufferHeight ==
            height
    )
    {
        return true;
    }

    DestroyBackBuffer();

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
        width;

    bi.bmiHeader.biHeight =
        -height;

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
        DestroyBackBuffer();

        return false;
    }

    g_backOldBitmap =
        static_cast<HBITMAP>(
            SelectObject(
                g_backDC,
                g_backBitmap
            )
        );

    g_bufferWidth =
        width;

    g_bufferHeight =
        height;

    return true;
}

// ============================================================
// DESKTOP STATE
// ============================================================

static bool IsDesktopWindow(
    HWND hwnd
)
{
    if (!hwnd)
        return false;

    wchar_t className[128]{};

    GetClassNameW(
        hwnd,
        className,
        ARRAYSIZE(className)
    );

    return
        lstrcmpW(
            className,
            L"Progman"
        ) == 0 ||
        lstrcmpW(
            className,
            L"WorkerW"
        ) == 0;
}

static bool IsDesktopActive()
{
    HWND foreground =
        GetForegroundWindow();

    if (!foreground)
        return false;

    if (
        foreground ==
        g_hwnd
    )
    {
        return true;
    }

    HWND shell =
        GetShellWindow();

    if (
        shell &&
        foreground == shell
    )
    {
        return true;
    }

    HWND root =
        GetAncestor(
            foreground,
            GA_ROOT
        );

    if (!root)
        root =
            foreground;

    return IsDesktopWindow(
        root
    );
}

// ============================================================
// POSITION
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

    const int width =
        ScaledInt(
            BASE_WIDTH
        );

    const int height =
        ScaledInt(
            BASE_HEIGHT
        );

    const int x =
        mi.rcWork.right -
        width -
        ScaledInt(
            RIGHT_MARGIN
        );

    const int y =
        mi.rcWork.bottom -
        height -
        ScaledInt(
            BOTTOM_MARGIN
        );

    SetWindowPos(
        g_hwnd,
        nullptr,
        x,
        y,
        width,
        height,
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

    SetWindowPos(
        g_hwnd,
        desktopActive
            ? HWND_TOP
            : HWND_BOTTOM,
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
}

// ============================================================
// ROUND RECT
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
// DRAW BACKGROUND
// ============================================================

static void DrawBackground(
    Graphics& graphics
)
{
    const float width =
        S(
            static_cast<float>(
                BASE_WIDTH
            )
        );

    const float height =
        S(
            static_cast<float>(
                BASE_HEIGHT
            )
        );

    GraphicsPath path;

    RoundedRectPath(
        path,
        RectF(
            S(0.5f),
            S(0.5f),
            width - S(1.0f),
            height - S(1.0f)
        ),
        S(18.0f)
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
        S(1.0f)
    );

    graphics.DrawPath(
        &border,
        &path
    );
}

// ============================================================
// DRAW COVER
// ============================================================

static void DrawCover(
    Graphics& graphics
)
{
    RectF destination(
        S(
            static_cast<float>(
                COVER_X
            )
        ),
        S(
            static_cast<float>(
                COVER_Y
            )
        ),
        S(
            static_cast<float>(
                COVER_SIZE
            )
        ),
        S(
            static_cast<float>(
                COVER_SIZE
            )
        )
    );

    GraphicsPath clip;

    RoundedRectPath(
        clip,
        destination,
        S(12.0f)
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
                S(5.0f)
            );

            graphics.DrawLine(
                &notePen,
                S(49.0f),
                S(35.0f),
                S(49.0f),
                S(64.0f)
            );

            graphics.DrawLine(
                &notePen,
                S(49.0f),
                S(35.0f),
                S(67.0f),
                S(30.0f)
            );

            graphics.FillEllipse(
                &noteBrush,
                S(38.0f),
                S(59.0f),
                S(16.0f),
                S(11.0f)
            );
        }
    }

    graphics.ResetClip();
}

// ============================================================
// DRAW CLOCK
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
            S(CLOCK_X),
            S(CLOCK_Y),
            S(CLOCK_W),
            S(CLOCK_H)
        ),
        S(10.0f),
        &panel
    );

    GraphicsPath borderPath;

    RoundedRectPath(
        borderPath,
        RectF(
            S(CLOCK_X + 0.5f),
            S(CLOCK_Y + 0.5f),
            S(CLOCK_W - 1.0f),
            S(CLOCK_H - 1.0f)
        ),
        S(10.0f)
    );

    Pen border(
        Color(
            55,
            255,
            255,
            255
        ),
        S(1.0f)
    );

    graphics.DrawPath(
        &border,
        &borderPath
    );

    const std::wstring time =
        CurrentClock();

    Font timeFont(
        L"Segoe UI",
        S(16.0f),
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
        time.c_str(),
        -1,
        &timeFont,
        RectF(
            S(CLOCK_X),
            S(CLOCK_Y + 1.0f),
            S(CLOCK_W),
            S(21.0f)
        ),
        &center,
        &white
    );

    const std::wstring date =
        CurrentDate();

    Font dateFont(
        L"Segoe UI",
        S(8.5f),
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
            S(CLOCK_X),
            S(CLOCK_Y + 25.0f),
            S(CLOCK_W),
            S(13.0f)
        ),
        &center,
        &dateBrush
    );
}

// ============================================================
// DRAW TITLE
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
        S(TITLE_FONT_SIZE),
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
        S(
            static_cast<float>(
                CONTENT_X
            )
        ),
        S(9.0f),
        S(
            static_cast<float>(
                TITLE_WIDTH
            )
        ),
        S(22.0f)
    );

    graphics.SetClip(
        area,
        CombineModeReplace
    );

    double offset = 0.0;

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
        const float firstX =
            area.X +
            S(
                static_cast<float>(
                    offset
                )
            );

        graphics.DrawString(
            title.c_str(),
            -1,
            &font,
            PointF(
                firstX,
                area.Y
            ),
            &brush
        );

        graphics.DrawString(
            title.c_str(),
            -1,
            &font,
            PointF(
                firstX +
                    S(
                        static_cast<float>(
                            g_cachedTitleWidth +
                            MARQUEE_GAP
                        )
                    ),
                area.Y
            ),
            &brush
        );
    }

    graphics.ResetClip();
}

// ============================================================
// DRAW ARTIST
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
        S(ARTIST_FONT_SIZE),
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
        S(
            static_cast<float>(
                CONTENT_X
            )
        ),
        S(34.0f),
        S(
            static_cast<float>(
                ARTIST_WIDTH
            )
        ),
        S(17.0f)
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
// DRAW ALBUM
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
        S(ALBUM_FONT_SIZE),
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
        S(
            static_cast<float>(
                CONTENT_X
            )
        ),
        S(49.0f),
        S(
            static_cast<float>(
                ALBUM_WIDTH
            )
        ),
        S(14.0f)
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
// DRAW PROGRESS
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
            S(PROGRESS_X),
            S(PROGRESS_Y),
            S(PROGRESS_W),
            S(PROGRESS_H)
        ),
        S(2.5f),
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
                S(PROGRESS_X),
                S(PROGRESS_Y),
                S(PROGRESS_W * ratio),
                S(PROGRESS_H)
            ),
            S(2.5f),
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
        S(knobX - 4.0f),
        S(PROGRESS_Y - 1.5f),
        S(8.0f),
        S(8.0f)
    );
}

// ============================================================
// DRAW DURATION
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
        S(8.5f),
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
            S(PROGRESS_X),
            S(72.0f)
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
            S(PROGRESS_X),
            S(72.0f),
            S(PROGRESS_W),
            S(15.0f)
        ),
        &alignRight,
        &brush
    );
}

// ============================================================
// VECTOR ICONS
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
        S(cx - radius),
        S(cy - radius),
        S(radius * 2.0f),
        S(radius * 2.0f)
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
                S(cx),
                S(cy),
                S(size),
                &icon
            );
            break;

        case 1:
            if (playing)
            {
                DrawPauseIcon(
                    graphics,
                    S(cx),
                    S(cy),
                    S(size),
                    &icon
                );
            }
            else
            {
                DrawPlayIcon(
                    graphics,
                    S(cx),
                    S(cy),
                    S(size),
                    &icon
                );
            }
            break;

        case 2:
            DrawNextIcon(
                graphics,
                S(cx),
                S(cy),
                S(size),
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
    const bool playing =
        g_playing.load();

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
        S(TITLE_FONT_SIZE),
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

    const int logicalWidth =
        static_cast<int>(
            std::ceil(
                measured.Width /
                g_scale
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
        logicalWidth;

    g_cachedTitleOverflow =
        logicalWidth >
        TITLE_WIDTH;
}

// ============================================================
// RENDER
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

    graphics.Clear(
        Color(
            0,
            0,
            0,
            0
        )
    );

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

    if (
        !IsWindowVisible(
            g_hwnd
        )
    )
    {
        return;
    }

    if (!g_backDC)
        return;

    HDC screen =
        GetDC(nullptr);

    if (!screen)
        return;

    RECT rect{};

    GetWindowRect(
        g_hwnd,
        &rect
    );

    POINT destination{
        rect.left,
        rect.top
    };

    POINT source{
        0,
        0
    };

    SIZE size{
        g_bufferWidth,
        g_bufferHeight
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
    int screenX,
    int screenY
)
{
    RECT rect{};

    GetWindowRect(
        g_hwnd,
        &rect
    );

    const float scale =
        g_scale <= 0.0f
            ? 1.0f
            : g_scale;

    const int x =
        static_cast<int>(
            (screenX - rect.left) /
            scale
        );

    const int y =
        static_cast<int>(
            (screenY - rect.top) /
            scale
        );

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
        y >= 60 &&
        y <= 79
    )
    {
        return HitArea::Progress;
    }

    return HitArea::None;
}

// ============================================================
// FOREGROUND EVENT
// ============================================================

static void CALLBACK ForegroundWinEventProc(
    HWINEVENTHOOK hook,
    DWORD event,
    HWND hwnd,
    LONG idObject,
    LONG idChild,
    DWORD eventThread,
    DWORD eventTime
)
{
    UNREFERENCED_PARAMETER(hook);
    UNREFERENCED_PARAMETER(event);
    UNREFERENCED_PARAMETER(idObject);
    UNREFERENCED_PARAMETER(idChild);
    UNREFERENCED_PARAMETER(eventThread);
    UNREFERENCED_PARAMETER(eventTime);

    if (!g_hwnd)
        return;

    if (hwnd == g_hwnd)
        return;

    UpdateOverlayZOrder(
        true
    );
}

// ============================================================
// WINDOW PROCEDURE
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
        case WM_NCHITTEST:
        {
            const HitArea area =
                HitTest(
                    GET_X_LPARAM(
                        lParam
                    ),
                    GET_Y_LPARAM(
                        lParam
                    )
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

        case WM_MOUSEACTIVATE:
        {
            return MA_NOACTIVATE;
        }

        case WM_LBUTTONDOWN:
        {
            const HitArea area =
                HitTest(
                    GET_X_LPARAM(
                        lParam
                    ),
                    GET_Y_LPARAM(
                        lParam
                    )
                );

            switch (area)
            {
                case HitArea::Previous:
                    QueuePrevious();
                    break;

                case HitArea::PlayPause:
                    QueuePlayPause();
                    break;

                case HitArea::Next:
                    QueueNext();
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
                        RECT rect{};

                        GetWindowRect(
                            hwnd,
                            &rect
                        );

                        const float scale =
                            g_scale <= 0.0f
                                ? 1.0f
                                : g_scale;

                        const double logicalX =
                            static_cast<double>(
                                GET_X_LPARAM(
                                    lParam
                                )
                            ) /
                            scale;

                        double ratio =
                            (
                                logicalX -
                                PROGRESS_X
                            ) /
                            PROGRESS_W;

                        ratio =
                            std::clamp(
                                ratio,
                                0.0,
                                1.0
                            );

                        QueueSeek(
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

        case WM_DPICHANGED:
        {
            const UINT newDpi =
                HIWORD(
                    wParam
                );

            g_dpi =
                newDpi == 0
                    ? 96
                    : newDpi;

            g_scale =
                static_cast<float>(
                    g_dpi
                ) /
                96.0f;

            const RECT* suggested =
                reinterpret_cast<
                    const RECT*
                >(
                    lParam
                );

            if (suggested)
            {
                SetWindowPos(
                    hwnd,
                    nullptr,
                    suggested->left,
                    suggested->top,
                    suggested->right -
                        suggested->left,
                    suggested->bottom -
                        suggested->top,
                    SWP_NOACTIVATE |
                    SWP_NOZORDER
                );
            }
            else
            {
                PositionOverlay();
            }

            DestroyBackBuffer();

            RenderFrame();

            PresentFrame();

            return 0;
        }

        case WM_DISPLAYCHANGE:
        case WM_SETTINGCHANGE:
        {
            PositionOverlay();

            g_haveZOrderState =
                false;

            if (g_visible.load())
            {
                RenderFrame();

                PresentFrame();
            }

            return 0;
        }

        case WM_APP + 20:
        {
            const bool shouldShow =
                g_visible.load();

            if (shouldShow)
            {
                if (
                    !IsWindowVisible(
                        hwnd
                    )
                )
                {
                    PositionOverlay();

                    ShowWindow(
                        hwnd,
                        SW_SHOWNOACTIVATE
                    );
                }

                UpdateOverlayZOrder(
                    true
                );

                RenderFrame();

                PresentFrame();
            }
            else
            {
                if (
                    IsWindowVisible(
                        hwnd
                    )
                )
                {
                    ShowWindow(
                        hwnd,
                        SW_HIDE
                    );
                }
            }

            return 0;
        }

        case WM_CLOSE:
        {
            DestroyWindow(
                hwnd
            );

            return 0;
        }

        case WM_NCDESTROY:
        {
            g_hwnd =
                nullptr;

            return DefWindowProcW(
                hwnd,
                message,
                wParam,
                lParam
            );
        }

        case WM_DESTROY:
        {
            if (g_foregroundHook)
            {
                UnhookWinEvent(
                    g_foregroundHook
                );

                g_foregroundHook =
                    nullptr;
            }

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
    SetThreadDpiAwarenessContext(
        DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
    );

    WNDCLASSEXW wc{};

    wc.cbSize =
        sizeof(wc);

    wc.lpfnWndProc =
        OverlayWndProc;

    wc.hInstance =
        g_hInstance;

    wc.lpszClassName =
        L"MallssMusicOverlay151";

    wc.hCursor =
        LoadCursorW(
            nullptr,
            IDC_ARROW
        );

    wc.hbrBackground =
        nullptr;

    if (
        !RegisterClassExW(
            &wc
        )
    )
    {
        Wh_Log(
            L"RegisterClassExW failed: %lu",
            GetLastError()
        );

        return;
    }

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
            BASE_WIDTH,
            BASE_HEIGHT,
            nullptr,
            nullptr,
            g_hInstance,
            nullptr
        );

    if (!g_hwnd)
    {
        Wh_Log(
            L"CreateWindowExW failed: %lu",
            GetLastError()
        );

        UnregisterClassW(
            wc.lpszClassName,
            g_hInstance
        );

        return;
    }

    UpdateDpiForWindow();

    PositionOverlay();

    CreateBackBuffer();

    RenderFrame();

    g_foregroundHook =
        SetWinEventHook(
            EVENT_SYSTEM_FOREGROUND,
            EVENT_SYSTEM_FOREGROUND,
            nullptr,
            ForegroundWinEventProc,
            0,
            0,
            WINEVENT_OUTOFCONTEXT |
                WINEVENT_SKIPOWNPROCESS
        );

    ShowWindow(
        g_hwnd,
        SW_HIDE
    );

    UpdateOverlayZOrder(
        true
    );

    Wh_Log(
        L"[UI] Overlay created hwnd=%p",
        g_hwnd
    );

    MSG msg{};

    int64_t lastFrameQpc =
        QpcNow();

    int lastMinute =
        -1;

    while (
        g_running.load()
    )
    {
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

        const bool visible =
            g_visible.load();

        const bool playing =
            g_playing.load();

        SYSTEMTIME st{};

        GetLocalTime(
            &st
        );

        const int minuteKey =
            st.wHour * 60 +
            st.wMinute;

        const bool minuteChanged =
            minuteKey !=
            lastMinute;

        if (minuteChanged)
        {
            lastMinute =
                minuteKey;
        }

        const int64_t nowQpc =
            QpcNow();

        double dt =
            QpcSecondsSince(
                lastFrameQpc
            );

        lastFrameQpc =
            nowQpc;

        if (dt > 0.1)
            dt = 0.1;

        bool titleOverflow =
            false;

        {
            std::scoped_lock lock(
                g_stateMutex
            );

            titleOverflow =
                g_cachedTitleOverflow;

            if (
                visible &&
                playing &&
                titleOverflow
            )
            {
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

                        const double cycleWidth =
                            g_cachedTitleWidth +
                            MARQUEE_GAP;

                        if (
                            -g_marqueeOffset >=
                            cycleWidth
                        )
                        {
                            g_marqueeOffset =
                                0.0;

                            g_marqueeTimer =
                                0.0;

                            g_marqueePhase =
                                MarqueePhase::HoldStart;
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
        }

        if (visible)
        {
            RenderFrame();

            PresentFrame();
        }

        //
        // Z-order is not touched every frame.
        //

        if (visible)
        {
            static int zOrderCounter =
                0;

            ++zOrderCounter;

            if (
                zOrderCounter >= 10
            )
            {
                zOrderCounter =
                    0;

                UpdateOverlayZOrder(
                    false
                );
            }
        }

        int sleepMs;

        if (
            visible &&
            playing &&
            titleOverflow
        )
        {
            sleepMs =
                16;
        }
        else if (
            visible &&
            playing
        )
        {
            sleepMs =
                33;
        }
        else if (visible)
        {
            sleepMs =
                200;
        }
        else
        {
            sleepMs =
                250;
        }

        Sleep(
            sleepMs
        );
    }

    if (g_foregroundHook)
    {
        UnhookWinEvent(
            g_foregroundHook
        );

        g_foregroundHook =
            nullptr;
    }

    if (g_hwnd)
    {
        DestroyWindow(
            g_hwnd
        );

        g_hwnd =
            nullptr;
    }

    DestroyBackBuffer();

    UnregisterClassW(
        wc.lpszClassName,
        g_hInstance
    );
}

// ============================================================
// TOOL INIT
// ============================================================

static bool InitGdiplus()
{
    GdiplusStartupInput input{};

    return
        GdiplusStartup(
            &g_gdiplusToken,
            &input,
            nullptr
        ) == Ok;
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

BOOL WhTool_ModInit()
{
    g_hInstance =
        GetCurrentModuleHandle();

    if (!g_hInstance)
    {
        Wh_Log(
            L"Failed to resolve mod module handle"
        );

        return FALSE;
    }

    g_running.store(
        true
    );

    g_visible.store(
        false
    );

    g_playing.store(
        false
    );

    g_forceRedraw.store(
        true
    );

    g_lastDesktopActive =
        false;

    g_haveZOrderState =
        false;

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

    if (!InitGdiplus())
    {
        Wh_Log(
            L"GDI+ initialization failed"
        );

        return FALSE;
    }

    try
    {
        g_uiThread.emplace(
            UIThreadProc
        );
    }
    catch (...)
    {
        ShutdownGdiplus();

        return FALSE;
    }

    try
    {
        g_mediaThread.emplace(
            MediaThreadProc
        );
    }
    catch (...)
    {
        g_running.store(
            false
        );

        g_commandCv.notify_all();

        if (g_hwnd)
        {
            PostMessageW(
                g_hwnd,
                WM_CLOSE,
                0,
                0
            );
        }

        if (
            g_uiThread &&
            g_uiThread->joinable()
        )
        {
            g_uiThread->join();
        }

        g_uiThread.reset();

        ShutdownGdiplus();

        return FALSE;
    }

    Wh_Log(
        L"Mallss Music Overlay 1.5.1 started as Windhawk tool"
    );

    return TRUE;
}

// ============================================================
// TOOL SETTINGS
// ============================================================

void WhTool_ModSettingsChanged()
{
    g_forceRedraw.store(
        true
    );

    if (g_hwnd)
    {
        PostMessageW(
            g_hwnd,
            WM_APP + 20,
            0,
            0
        );
    }
}

// ============================================================
// TOOL UNINIT
// ============================================================

void WhTool_ModUninit()
{
    Wh_Log(
        L"Mallss Music Overlay 1.5.1 shutting down"
    );

    g_running.store(
        false
    );

    g_commandCv.notify_all();

    if (g_hwnd)
    {
        PostMessageW(
            g_hwnd,
            WM_CLOSE,
            0,
            0
        );
    }

    if (
        g_mediaThread &&
        g_mediaThread->joinable()
    )
    {
        g_mediaThread->join();
    }

    g_mediaThread.reset();

    if (
        g_uiThread &&
        g_uiThread->joinable()
    )
    {
        g_uiThread->join();
    }

    g_uiThread.reset();

    {
        std::scoped_lock lock(
            g_stateMutex
        );

        DestroyCoverLocked();

        g_title =
            L"No music";

        g_artist.clear();

        g_album.clear();

        g_duration =
            0.0;

        g_mediaValid =
            false;

        g_playing =
            false;
    }

    {
        std::scoped_lock lock(
            g_commandMutex
        );

        g_commandQueue.clear();
    }

    ShutdownGdiplus();

    Wh_Log(
        L"Mallss Music Overlay 1.5.1 stopped"
    );
}

// ============================================================
// WINDHAWK TOOL LAUNCHER
// ============================================================

static bool g_isToolModProcessLauncher = false;

static HANDLE g_toolModProcessMutex = nullptr;

static void WINAPI EntryPoint_Hook()
{
    Wh_Log(
        L">"
    );

    ExitThread(
        0
    );
}

BOOL Wh_ModInit()
{
    bool isService =
        false;

    bool isToolModProcess =
        false;

    bool isCurrentToolModProcess =
        false;

    int argc = 0;

    LPWSTR* argv =
        CommandLineToArgvW(
            GetCommandLineW(),
            &argc
        );

    if (!argv)
    {
        Wh_Log(
            L"CommandLineToArgvW failed"
        );

        return FALSE;
    }

    for (
        int i = 1;
        i < argc;
        ++i
    )
    {
        if (
            wcscmp(
                argv[i],
                L"-service"
            ) == 0
        )
        {
            isService =
                true;

            break;
        }
    }

    for (
        int i = 1;
        i < argc - 1;
        ++i
    )
    {
        if (
            wcscmp(
                argv[i],
                L"-tool-mod"
            ) == 0
        )
        {
            isToolModProcess =
                true;

            if (
                wcscmp(
                    argv[i + 1],
                    WH_MOD_ID
                ) == 0
            )
            {
                isCurrentToolModProcess =
                    true;
            }

            break;
        }
    }

    LocalFree(
        argv
    );

    if (isService)
    {
        return FALSE;
    }

    if (isCurrentToolModProcess)
    {
        g_toolModProcessMutex =
            CreateMutexW(
                nullptr,
                TRUE,
                L"windhawk-tool-mod_" WH_MOD_ID
            );

        if (!g_toolModProcessMutex)
        {
            Wh_Log(
                L"CreateMutex failed"
            );

            ExitProcess(
                1
            );
        }

        if (
            GetLastError() ==
            ERROR_ALREADY_EXISTS
        )
        {
            Wh_Log(
                L"Tool mod already running (%s)",
                WH_MOD_ID
            );

            ExitProcess(
                1
            );
        }

        if (!WhTool_ModInit())
        {
            ExitProcess(
                1
            );
        }

        IMAGE_DOS_HEADER* dosHeader =
            reinterpret_cast<
                IMAGE_DOS_HEADER*
            >(
                GetModuleHandleW(
                    nullptr
                )
            );

        if (!dosHeader)
        {
            ExitProcess(
                1
            );
        }

        IMAGE_NT_HEADERS* ntHeaders =
            reinterpret_cast<
                IMAGE_NT_HEADERS*
            >(
                reinterpret_cast<BYTE*>(
                    dosHeader
                ) +
                dosHeader->e_lfanew
            );

        const DWORD entryPointRVA =
            ntHeaders
                ->OptionalHeader
                .AddressOfEntryPoint;

        void* entryPoint =
            reinterpret_cast<BYTE*>(
                dosHeader
            ) +
            entryPointRVA;

        Wh_SetFunctionHook(
            entryPoint,
            reinterpret_cast<void*>(
                EntryPoint_Hook
            ),
            nullptr
        );

        return TRUE;
    }

    if (isToolModProcess)
    {
        return FALSE;
    }

    g_isToolModProcessLauncher =
        true;

    return TRUE;
}

// ============================================================
// TOOL LAUNCHER
// ============================================================

void Wh_ModAfterInit()
{
    if (!g_isToolModProcessLauncher)
        return;

    WCHAR currentProcessPath[MAX_PATH]{};

    const DWORD length =
        GetModuleFileNameW(
            nullptr,
            currentProcessPath,
            ARRAYSIZE(
                currentProcessPath
            )
        );

    if (
        length == 0 ||
        length >= ARRAYSIZE(
            currentProcessPath
        )
    )
    {
        Wh_Log(
            L"GetModuleFileNameW failed"
        );

        return;
    }

    WCHAR commandLine[
        MAX_PATH + 128
    ]{};

    swprintf_s(
        commandLine,
        L"\"%s\" -tool-mod \"%s\"",
        currentProcessPath,
        WH_MOD_ID
    );

    HMODULE kernelModule =
        GetModuleHandleW(
            L"kernelbase.dll"
        );

    if (!kernelModule)
    {
        kernelModule =
            GetModuleHandleW(
                L"kernel32.dll"
            );
    }

    if (!kernelModule)
    {
        Wh_Log(
            L"No kernel module"
        );

        return;
    }

    using CreateProcessInternalW_t =
        BOOL(WINAPI*)(
            HANDLE,
            LPCWSTR,
            LPWSTR,
            LPSECURITY_ATTRIBUTES,
            LPSECURITY_ATTRIBUTES,
            WINBOOL,
            DWORD,
            LPVOID,
            LPCWSTR,
            LPSTARTUPINFOW,
            LPPROCESS_INFORMATION,
            PHANDLE
        );

    auto pCreateProcessInternalW =
        reinterpret_cast<
            CreateProcessInternalW_t
        >(
            GetProcAddress(
                kernelModule,
                "CreateProcessInternalW"
            )
        );

    if (!pCreateProcessInternalW)
    {
        Wh_Log(
            L"CreateProcessInternalW unavailable"
        );

        return;
    }

    STARTUPINFOW si{};

    si.cb =
        sizeof(si);

    si.dwFlags =
        STARTF_FORCEOFFFEEDBACK;

    PROCESS_INFORMATION pi{};

    if (
        !pCreateProcessInternalW(
            nullptr,
            currentProcessPath,
            commandLine,
            nullptr,
            nullptr,
            FALSE,
            NORMAL_PRIORITY_CLASS,
            nullptr,
            nullptr,
            &si,
            &pi,
            nullptr
        )
    )
    {
        Wh_Log(
            L"CreateProcessInternalW failed: %lu",
            GetLastError()
        );

        return;
    }

    CloseHandle(
        pi.hProcess
    );

    CloseHandle(
        pi.hThread
    );
}

// ============================================================
// WINDHAWK SETTINGS
// ============================================================

void Wh_ModSettingsChanged()
{
    if (
        g_isToolModProcessLauncher
    )
    {
        return;
    }

    WhTool_ModSettingsChanged();
}

// ============================================================
// WINDHAWK UNINIT
// ============================================================

void Wh_ModUninit()
{
    if (
        g_isToolModProcessLauncher
    )
    {
        return;
    }

    WhTool_ModUninit();

    if (g_toolModProcessMutex)
    {
        CloseHandle(
            g_toolModProcessMutex
        );

        g_toolModProcessMutex =
            nullptr;
    }

    ExitProcess(
        0
    );
}
