// ==WindhawkMod==
// @id              mallss-music-overlay
// @name            Mallss Music Overlay
// @description     Compact desktop music overlay with album art, clock, date and playback controls.
// @version         1.5.1
// @author          Mallss
// @github          https://github.com/ItsMeMal
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -lgdiplus -lgdi32 -lshell32 -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Mallss Music Overlay

Compact desktop media overlay using Windows Global System Media
Transport Controls.

## Features

- Album artwork
- Song title
- Artist
- Album
- Smooth marquee
- Progress bar
- Elapsed time
- Total duration
- Previous / Play-Pause / Next
- Click progress bar to seek
- Clock and date
- Locale-aware time and date
- Monitor selection
- Four corner positions
- X/Y offset
- Dedicated Windhawk tool process

## Desktop behavior

The overlay is a non-activating top-level desktop widget.

It does not steal focus and does not use Explorer injection.

Normal application windows naturally cover it when they are opened.
When the desktop becomes visible again, the overlay remains available.

## Playback timing

Elapsed time is driven by a local monotonic clock while playing.
The media timeline is used when a track starts, playback changes,
or an explicit seek is performed.

Normal polling does not continuously overwrite the displayed position.

## Difference from other desktop media overlays

The focus of this mod is a compact corner card combining album artwork,
media controls, clock and date in one desktop-oriented widget.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- monitor: primary
  $name: Monitor
  $description: Display where the overlay is shown.
  $options:
  - primary: Primary monitor
  - display1: Display 1
  - display2: Display 2
  - display3: Display 3
  - display4: Display 4

- corner: bottom_right
  $name: Corner
  $description: Screen corner where the overlay is positioned.
  $options:
  - top_left: Top left
  - top_right: Top right
  - bottom_left: Bottom left
  - bottom_right: Bottom right

- offsetX: 18
  $name: Horizontal offset
  $description: Horizontal offset in pixels.

- offsetY: 18
  $name: Vertical offset
  $description: Vertical offset in pixels.

- timeFormat: auto
  $name: Clock format
  $description: Follow Windows locale or force 12/24 hour.
  $options:
  - auto: Automatic
  - 12h: 12-hour
  - 24h: 24-hour

- dateFormat: auto
  $name: Date format
  $description: Follow Windows locale or force short/long date.
  $options:
  - auto: Automatic
  - short: Short date
  - long: Long date
*/
// ==/WindhawkModSettings==

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
#include <condition_variable>
#include <cstdint>
#include <cwctype>
#include <deque>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <vector>
#include <cmath>
#include <cstring>

using namespace Gdiplus;
using namespace winrt;

using namespace Windows::Foundation;
using namespace Windows::Media::Control;
using namespace Windows::Storage::Streams;

using namespace std::chrono_literals;

// ============================================================
// CONFIG
// ============================================================

static constexpr int OVERLAY_W = 420;
static constexpr int OVERLAY_H = 164;

static constexpr int WINDOW_CORNER_RADIUS = 18;

static constexpr float COVER_X = 12.0f;
static constexpr float COVER_Y = 11.0f;
static constexpr float COVER_SIZE = 92.0f;

static constexpr float CONTENT_X = 116.0f;

static constexpr float TITLE_WIDTH = 178.0f;
static constexpr float ARTIST_WIDTH = 178.0f;
static constexpr float ALBUM_WIDTH = 178.0f;

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
static constexpr double MARQUEE_HOLD = 1.15;

static constexpr int MEDIA_POLL_MS = 250;
static constexpr int COMMAND_TIMEOUT_MS = 800;

static constexpr double DURATION_EPSILON = 0.25;

static constexpr UINT WM_MALSS_UPDATE = WM_APP + 20;

// ============================================================
// SETTINGS
// ============================================================

enum class Corner
{
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight
};

enum class TimeFormatMode
{
    Auto,
    TwelveHour,
    TwentyFourHour
};

enum class DateFormatMode
{
    Auto,
    Short,
    Long
};

struct Settings
{
    std::wstring monitor = L"primary";

    Corner corner = Corner::BottomRight;

    int offsetX = 18;
    int offsetY = 18;

    TimeFormatMode timeFormat =
        TimeFormatMode::Auto;

    DateFormatMode dateFormat =
        DateFormatMode::Auto;
};

static Settings g_settings;

// ============================================================
// GLOBALS
// ============================================================

static HINSTANCE g_hInstance = nullptr;
static HWND g_hwnd = nullptr;

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
// COMMANDS
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
    CommandType type = CommandType::PlayPause;
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

static std::wstring g_title = L"No music";
static std::wstring g_artist;
static std::wstring g_album;

static double g_duration = 0.0;

// ============================================================
// LOCAL CLOCK
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

static HDC g_backDC = nullptr;
static HBITMAP g_backBitmap = nullptr;
static HBITMAP g_backOldBitmap = nullptr;

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
    Hold,
    Scroll
};

static MarqueePhase g_marqueePhase = MarqueePhase::Hold;

static double g_marqueeTimer = 0.0;
static double g_marqueeOffset = 0.0;

static int g_cachedTitleWidth = 0;
static bool g_cachedTitleOverflow = false;

static std::wstring g_cachedTitle;

// ============================================================
// FORWARD
// ============================================================

static void UIThreadProc();
static void MediaThreadProc();

static void RenderFrame();
static void PresentFrame();

static void PositionOverlay();

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

static void UpdateWindowRegion();

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
    if (!start)
        return 0.0;

    LARGE_INTEGER frequency{};

    if (!QueryPerformanceFrequency(&frequency))
        return 0.0;

    return static_cast<double>(
        QpcNow() - start
    ) / static_cast<double>(
        frequency.QuadPart
    );
}

// ============================================================
// MODULE
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
// SETTINGS
// ============================================================

static std::wstring ReadStringSetting(
    PCWSTR name
)
{
    PCWSTR value =
        Wh_GetStringSetting(name);

    std::wstring result =
        value ? value : L"";

    Wh_FreeStringSetting(value);

    return result;
}

static void LoadSettings()
{
    Settings settings;

    settings.monitor =
        ReadStringSetting(L"monitor");

    if (settings.monitor.empty())
        settings.monitor = L"primary";

    const std::wstring corner =
        ReadStringSetting(L"corner");

    if (corner == L"top_left")
        settings.corner = Corner::TopLeft;
    else if (corner == L"top_right")
        settings.corner = Corner::TopRight;
    else if (corner == L"bottom_left")
        settings.corner = Corner::BottomLeft;
    else
        settings.corner = Corner::BottomRight;

    settings.offsetX =
        std::clamp(
            Wh_GetIntSetting(L"offsetX"),
            0,
            2000
        );

    settings.offsetY =
        std::clamp(
            Wh_GetIntSetting(L"offsetY"),
            0,
            2000
        );

    const std::wstring timeFormat =
        ReadStringSetting(L"timeFormat");

    if (timeFormat == L"12h")
        settings.timeFormat =
            TimeFormatMode::TwelveHour;
    else if (timeFormat == L"24h")
        settings.timeFormat =
            TimeFormatMode::TwentyFourHour;
    else
        settings.timeFormat =
            TimeFormatMode::Auto;

    const std::wstring dateFormat =
        ReadStringSetting(L"dateFormat");

    if (dateFormat == L"short")
        settings.dateFormat =
            DateFormatMode::Short;
    else if (dateFormat == L"long")
        settings.dateFormat =
            DateFormatMode::Long;
    else
        settings.dateFormat =
            DateFormatMode::Auto;

    g_settings =
        std::move(settings);
}

// ============================================================
// STRING
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

static bool NearlyEqual(
    double a,
    double b,
    double epsilon
)
{
    return std::abs(a - b) <= epsilon;
}

// ============================================================
// CLOCK / DATE
// ============================================================

static std::wstring CurrentClock()
{
    SYSTEMTIME st{};

    GetLocalTime(&st);

    wchar_t buffer[64]{};

    const wchar_t* format = nullptr;

    switch (g_settings.timeFormat)
    {
        case TimeFormatMode::TwelveHour:
            format = L"h:mm tt";
            break;

        case TimeFormatMode::TwentyFourHour:
            format = L"HH:mm";
            break;

        case TimeFormatMode::Auto:
        default:
            break;
    }

    if (
        GetTimeFormatEx(
            LOCALE_NAME_USER_DEFAULT,
            TIME_NOSECONDS,
            &st,
            format,
            buffer,
            ARRAYSIZE(buffer)
        ) > 0
    )
    {
        return buffer;
    }

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

    GetLocalTime(&st);

    wchar_t buffer[128]{};

    DWORD flags = 0;

    switch (g_settings.dateFormat)
    {
        case DateFormatMode::Short:
            flags = DATE_SHORTDATE;
            break;

        case DateFormatMode::Long:
            flags = DATE_LONGDATE;
            break;

        case DateFormatMode::Auto:
        default:
            flags = 0;
            break;
    }

    if (
        GetDateFormatEx(
            LOCALE_NAME_USER_DEFAULT,
            flags,
            &st,
            nullptr,
            buffer,
            ARRAYSIZE(buffer),
            nullptr
        ) > 0
    )
    {
        return buffer;
    }

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
// TIME FORMAT
// ============================================================

static std::wstring FormatTime(
    double seconds
)
{
    if (seconds < 0)
        seconds = 0;

    const int total =
        static_cast<int>(seconds);

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

// ============================================================
// LOCAL CLOCK
// ============================================================

static double GetLocalPlaybackPositionLocked()
{
    if (!g_clockRunning)
        return g_frozenPosition;

    double position =
        g_clockAnchorPosition +
        QpcSecondsSince(
            g_clockAnchorQpc
        );

    if (position < 0)
        position = 0;

    if (
        g_duration > 0 &&
        position > g_duration
    )
    {
        position = g_duration;
    }

    return position;
}

static void StartLocalClockLocked(
    double position
)
{
    position =
        std::max(
            0.0,
            position
        );

    if (
        g_duration > 0 &&
        position > g_duration
    )
    {
        position = g_duration;
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
    position =
        std::max(
            0.0,
            position
        );

    if (
        g_duration > 0 &&
        position > g_duration
    )
    {
        position = g_duration;
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
        MarqueePhase::Hold;

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

        auto openOperation =
            thumbnail.OpenReadAsync();

        if (
            openOperation.wait_for(
                1500ms
            ) !=
            AsyncStatus::Completed
        )
        {
            return false;
        }

        auto stream =
            openOperation.get();

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

        DataReader reader(stream);

        auto loadOperation =
            reader.LoadAsync(size);

        if (
            loadOperation.wait_for(
                1500ms
            ) !=
            AsyncStatus::Completed
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
                bytes.data() + bytes.size()
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
            GlobalLock(hGlobal);

        if (!memory)
        {
            GlobalFree(hGlobal);
            return false;
        }

        memcpy(
            memory,
            bytes.data(),
            bytes.size()
        );

        GlobalUnlock(hGlobal);

        IStream* streamObject =
            nullptr;

        HRESULT hr =
            CreateStreamOnHGlobal(
                hGlobal,
                TRUE,
                &streamObject
            );

        if (
            FAILED(hr) ||
            !streamObject
        )
        {
            GlobalFree(hGlobal);
            return false;
        }

        Bitmap* bitmap =
            Bitmap::FromStream(
                streamObject
            );

        streamObject->Release();

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
            0
        }
    );
}

static void QueuePlayPause()
{
    QueueCommand(
        MediaCommand{
            CommandType::PlayPause,
            0
        }
    );
}

static void QueueNext()
{
    QueueCommand(
        MediaCommand{
            CommandType::Next,
            0
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
                auto op =
                    session.TrySkipPreviousAsync();

                if (
                    op.wait_for(
                        std::chrono::milliseconds(
                            COMMAND_TIMEOUT_MS
                        )
                    ) ==
                    AsyncStatus::Completed
                )
                {
                    op.get();
                }

                break;
            }

            case CommandType::PlayPause:
            {
                auto op =
                    session.TryTogglePlayPauseAsync();

                if (
                    op.wait_for(
                        std::chrono::milliseconds(
                            COMMAND_TIMEOUT_MS
                        )
                    ) ==
                    AsyncStatus::Completed
                )
                {
                    op.get();
                }

                break;
            }

            case CommandType::Next:
            {
                auto op =
                    session.TrySkipNextAsync();

                if (
                    op.wait_for(
                        std::chrono::milliseconds(
                            COMMAND_TIMEOUT_MS
                        )
                    ) ==
                    AsyncStatus::Completed
                )
                {
                    op.get();
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
                    std::max(
                        0.0,
                        command.position
                    );

                if (
                    duration > 0 &&
                    target > duration
                )
                {
                    target =
                        duration;
                }

                auto op =
                    session.TryChangePlaybackPositionAsync(
                        static_cast<int64_t>(
                            target *
                            10000000.0
                        )
                    );

                if (
                    op.wait_for(
                        std::chrono::milliseconds(
                            COMMAND_TIMEOUT_MS
                        )
                    ) ==
                    AsyncStatus::Completed
                )
                {
                    op.get();

                    std::scoped_lock lock(
                        g_stateMutex
                    );

                    if (g_playing)
                        StartLocalClockLocked(target);
                    else
                        FreezeLocalClockLocked(target);
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

    double duration = 0;
    double position = 0;

    std::wstring coverKey;

    GlobalSystemMediaTransportControlsSessionMediaProperties
        props{nullptr};
};

// ============================================================
// SNAPSHOT CHANGE DETECTION
// ============================================================

static bool MediaSnapshotChanged(
    const MediaSnapshot& snapshot
)
{
    std::scoped_lock lock(
        g_stateMutex
    );

    if (!snapshot.valid)
        return false;

    if (!g_mediaValid)
        return true;

    if (
        !SameText(
            g_title,
            snapshot.title
        )
    )
    {
        return true;
    }

    if (
        !SameText(
            g_artist,
            snapshot.artist
        )
    )
    {
        return true;
    }

    if (
        !SameText(
            g_album,
            snapshot.album
        )
    )
    {
        return true;
    }

    if (
        g_playing.load() !=
        snapshot.playing
    )
    {
        return true;
    }

    if (
        g_duration <= 0.0 &&
        snapshot.duration > 0.0
    )
    {
        return true;
    }

    if (
        g_duration > 0.0 &&
        snapshot.duration <= 0.0
    )
    {
        return true;
    }

    if (
        g_duration > 0.0 &&
        snapshot.duration > 0.0 &&
        !NearlyEqual(
            g_duration,
            snapshot.duration,
            DURATION_EPSILON
        )
    )
    {
        return true;
    }

    return false;
}

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
            auto op =
                GlobalSystemMediaTransportControlsSessionManager
                    ::RequestAsync();

            if (
                op.wait_for(
                    1500ms
                ) !=
                AsyncStatus::Completed
            )
            {
                Wh_Log(
                    L"[Media] RequestAsync timeout"
                );

                return false;
            }

            g_sessionManager =
                op.get();

            Wh_Log(
                L"[Media] Session manager OK"
            );
        }

        if (!g_sessionManager)
            return false;

        auto session =
            g_sessionManager.GetCurrentSession();

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
            return false;

        snapshot.playbackStatus =
            playback.PlaybackStatus();

        snapshot.playing =
            snapshot.playbackStatus ==
            GlobalSystemMediaTransportControlsSessionPlaybackStatus
                ::Playing;

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

        auto propertiesOp =
            session.TryGetMediaPropertiesAsync();

        if (
            propertiesOp.wait_for(
                1500ms
            ) !=
            AsyncStatus::Completed
        )
        {
            return false;
        }

        auto props =
            propertiesOp.get();

        if (!props)
            return false;

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
            session.GetTimelineProperties();

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

            if (duration > 0)
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

    bool stateChanged =
        false;

    {
        std::scoped_lock lock(
            g_stateMutex
        );

        const bool hadMedia =
            g_mediaValid;

        const bool oldPlaying =
            g_playing.load();

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

        const bool durationChanged =
            (
                g_duration <= 0.0 &&
                snapshot.duration > 0.0
            ) ||
            (
                g_duration > 0.0 &&
                snapshot.duration <= 0.0
            ) ||
            (
                g_duration > 0.0 &&
                snapshot.duration > 0.0 &&
                !NearlyEqual(
                    g_duration,
                    snapshot.duration,
                    DURATION_EPSILON
                )
            );

        stateChanged =
            trackChanged ||
            playbackChanged ||
            durationChanged ||
            !hadMedia;

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

            const double position =
                snapshot.position;

            if (snapshot.playing)
                StartLocalClockLocked(position);
            else
                FreezeLocalClockLocked(position);

            DestroyCoverLocked();

            ResetMarqueeLocked();

            needCover =
                true;

            coverKey =
                snapshot.coverKey;
        }
        else if (
            snapshot.duration > 0
        )
        {
            g_duration =
                snapshot.duration;
        }

        if (
            playbackChanged &&
            oldPlaying &&
            !snapshot.playing
        )
        {
            FreezeLocalClockLocked(
                GetLocalPlaybackPositionLocked()
            );
        }

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

        g_playing =
            snapshot.playing;
    }

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

    if (
        g_hwnd &&
        stateChanged
    )
    {
        g_forceRedraw.store(
            true
        );

        PostMessageW(
            g_hwnd,
            WM_MALSS_UPDATE,
            0,
            0
        );
    }

    if (stateChanged)
    {
        Wh_Log(
            L"[UI] Media state applied"
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
            FreezeLocalClockLocked(
                GetLocalPlaybackPositionLocked()
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
            0;

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
                WM_MALSS_UPDATE,
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
                if (
                    MediaSnapshotChanged(
                        snapshot
                    )
                )
                {
                    ApplyMedia(
                        snapshot
                    );
                }
            }
            else
            {
                HandleNoMedia();
            }
        }
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

static void UpdateDpi()
{
    if (!g_hwnd)
        return;

    UINT dpi =
        GetDpiForWindow(
            g_hwnd
        );

    if (!dpi)
        dpi = 96;

    g_dpi =
        dpi;

    g_scale =
        static_cast<float>(dpi) /
        96.0f;
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
    return value * g_scale;
}

// ============================================================
// WINDOW REGION
// ============================================================

static void UpdateWindowRegion()
{
    if (!g_hwnd)
        return;

    const int width =
        ScaledInt(
            OVERLAY_W
        );

    const int height =
        ScaledInt(
            OVERLAY_H
        );

    const int radius =
        std::max(
            1,
            ScaledInt(
                WINDOW_CORNER_RADIUS
            )
        );

    HRGN region =
        CreateRoundRectRgn(
            0,
            0,
            width + 1,
            height + 1,
            radius * 2,
            radius * 2
        );

    if (!region)
    {
        Wh_Log(
            L"[UI] CreateRoundRectRgn failed: %lu",
            GetLastError()
        );

        return;
    }

    if (
        SetWindowRgn(
            g_hwnd,
            region,
            TRUE
        ) == 0
    )
    {
        DeleteObject(
            region
        );

        Wh_Log(
            L"[UI] SetWindowRgn failed: %lu",
            GetLastError()
        );
    }
}

// ============================================================
// MONITOR
// ============================================================

struct MonitorLookup
{
    std::wstring requested;

    HMONITOR monitor = nullptr;

    bool found = false;
};

static BOOL CALLBACK FindMonitorCallback(
    HMONITOR monitor,
    HDC,
    LPRECT,
    LPARAM lParam
)
{
    auto* lookup =
        reinterpret_cast<
            MonitorLookup*
        >(
            lParam
        );

    if (!lookup)
        return TRUE;

    MONITORINFOEXW info{};

    info.cbSize =
        sizeof(info);

    if (
        !GetMonitorInfoW(
            monitor,
            &info
        )
    )
    {
        return TRUE;
    }

    bool match =
        false;

    if (
        lookup->requested ==
        L"primary"
    )
    {
        match =
            (info.dwFlags &
                MONITORINFOF_PRIMARY) !=
            0;
    }
    else
    {
        const wchar_t* wanted =
            nullptr;

        if (
            lookup->requested ==
            L"display1"
        )
            wanted = L"\\\\.\\DISPLAY1";
        else if (
            lookup->requested ==
            L"display2"
        )
            wanted = L"\\\\.\\DISPLAY2";
        else if (
            lookup->requested ==
            L"display3"
        )
            wanted = L"\\\\.\\DISPLAY3";
        else if (
            lookup->requested ==
            L"display4"
        )
            wanted = L"\\\\.\\DISPLAY4";

        if (wanted)
        {
            match =
                lstrcmpW(
                    wanted,
                    info.szDevice
                ) == 0;
        }
    }

    if (match)
    {
        lookup->monitor =
            monitor;

        lookup->found =
            true;

        return FALSE;
    }

    return TRUE;
}

static HMONITOR GetSelectedMonitor()
{
    MonitorLookup lookup;

    lookup.requested =
        g_settings.monitor;

    EnumDisplayMonitors(
        nullptr,
        nullptr,
        FindMonitorCallback,
        reinterpret_cast<LPARAM>(
            &lookup
        )
    );

    if (
        lookup.found &&
        lookup.monitor
    )
    {
        return lookup.monitor;
    }

    return MonitorFromPoint(
        POINT{0, 0},
        MONITOR_DEFAULTTOPRIMARY
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
        GetSelectedMonitor();

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
            OVERLAY_W
        );

    const int height =
        ScaledInt(
            OVERLAY_H
        );

    const int offsetX =
        ScaledInt(
            g_settings.offsetX
        );

    const int offsetY =
        ScaledInt(
            g_settings.offsetY
        );

    int x = 0;
    int y = 0;

    switch (
        g_settings.corner
    )
    {
        case Corner::TopLeft:
            x =
                mi.rcWork.left +
                offsetX;

            y =
                mi.rcWork.top +
                offsetY;
            break;

        case Corner::TopRight:
            x =
                mi.rcWork.right -
                width -
                offsetX;

            y =
                mi.rcWork.top +
                offsetY;
            break;

        case Corner::BottomLeft:
            x =
                mi.rcWork.left +
                offsetX;

            y =
                mi.rcWork.bottom -
                height -
                offsetY;
            break;

        case Corner::BottomRight:
        default:
            x =
                mi.rcWork.right -
                width -
                offsetX;

            y =
                mi.rcWork.bottom -
                height -
                offsetY;
            break;
    }

    SetWindowPos(
        g_hwnd,
        HWND_TOP,
        x,
        y,
        width,
        height,
        SWP_NOACTIVATE |
        SWP_NOOWNERZORDER
    );

    UpdateWindowRegion();
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

    g_backDC = nullptr;
    g_backBitmap = nullptr;
    g_backOldBitmap = nullptr;

    g_bufferWidth = 0;
    g_bufferHeight = 0;
}

static bool CreateBackBuffer()
{
    const int width =
        ScaledInt(OVERLAY_W);

    const int height =
        ScaledInt(OVERLAY_H);

    if (
        g_backDC &&
        g_bufferWidth == width &&
        g_bufferHeight == height
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
        CreateCompatibleDC(screen);

    if (!g_backDC)
    {
        ReleaseDC(
            nullptr,
            screen
        );

        return false;
    }

    g_backBitmap =
        CreateCompatibleBitmap(
            screen,
            width,
            height
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
// ROUNDED RECT
// ============================================================

static void RoundedRectPath(
    GraphicsPath& path,
    const RectF& rect,
    float radius
)
{
    const float diameter =
        std::min(
            radius * 2.0f,
            std::min(
                rect.Width,
                rect.Height
            )
        );

    path.AddArc(
        rect.X,
        rect.Y,
        diameter,
        diameter,
        180,
        90
    );

    path.AddArc(
        rect.GetRight() - diameter,
        rect.Y,
        diameter,
        diameter,
        270,
        90
    );

    path.AddArc(
        rect.GetRight() - diameter,
        rect.GetBottom() - diameter,
        diameter,
        diameter,
        0,
        90
    );

    path.AddArc(
        rect.X,
        rect.GetBottom() - diameter,
        diameter,
        diameter,
        90,
        90
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
// DRAW
// ============================================================

static void DrawBackground(
    Graphics& graphics
)
{
    SolidBrush background(
        Color(
            255,
            16,
            16,
            21
        )
    );

    RectF rect(
        S(0.5f),
        S(0.5f),
        S(
            static_cast<float>(
                OVERLAY_W
            )
        ) - S(1.0f),
        S(
            static_cast<float>(
                OVERLAY_H
            )
        ) - S(1.0f)
    );

    FillRoundedRect(
        graphics,
        rect,
        S(
            static_cast<float>(
                WINDOW_CORNER_RADIUS
            )
        ),
        &background
    );

    GraphicsPath borderPath;

    RoundedRectPath(
        borderPath,
        rect,
        S(
            static_cast<float>(
                WINDOW_CORNER_RADIUS
            )
        )
    );

    Pen border(
        Color(
            80,
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
}

static void DrawCover(
    Graphics& graphics
)
{
    RectF rect(
        S(COVER_X),
        S(COVER_Y),
        S(COVER_SIZE),
        S(COVER_SIZE)
    );

    GraphicsPath clip;

    RoundedRectPath(
        clip,
        rect,
        S(11.0f)
    );

    graphics.SetClip(
        &clip,
        CombineModeIntersect
    );

    {
        std::scoped_lock lock(
            g_stateMutex
        );

        if (g_coverBitmap)
        {
            graphics.SetInterpolationMode(
                InterpolationModeHighQualityBicubic
            );

            graphics.DrawImage(
                g_coverBitmap,
                rect
            );
        }
        else
        {
            SolidBrush placeholder(
                Color(
                    255,
                    42,
                    42,
                    49
                )
            );

            graphics.FillPath(
                &placeholder,
                &clip
            );
        }
    }

    graphics.ResetClip();
}

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

    Font timeFont(
        L"Segoe UI",
        S(16.0f),
        FontStyleBold,
        UnitPixel
    );

    Font dateFont(
        L"Segoe UI",
        S(8.5f),
        FontStyleRegular,
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

    SolidBrush gray(
        Color(
            190,
            190,
            198,
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

    const std::wstring time =
        CurrentClock();

    graphics.DrawString(
        time.c_str(),
        -1,
        &timeFont,
        RectF(
            S(CLOCK_X),
            S(CLOCK_Y),
            S(CLOCK_W),
            S(21.0f)
        ),
        &center,
        &white
    );

    const std::wstring date =
        CurrentDate();

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
        &gray
    );
}

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
        S(CONTENT_X),
        S(9.0f),
        S(TITLE_WIDTH),
        S(22.0f)
    );

    graphics.SetClip(
        area,
        CombineModeReplace
    );

    double offset = 0;

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
            S(
                static_cast<float>(
                    offset
                )
            );

        const float cycle =
            S(
                static_cast<float>(
                    g_cachedTitleWidth +
                    MARQUEE_GAP
                )
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
                x + cycle,
                area.Y
            ),
            &brush
        );
    }

    graphics.ResetClip();
}

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

    StringFormat format;

    format.SetTrimming(
        StringTrimmingEllipsisCharacter
    );

    graphics.DrawString(
        artist.c_str(),
        -1,
        &font,
        RectF(
            S(CONTENT_X),
            S(34.0f),
            S(ARTIST_WIDTH),
            S(17.0f)
        ),
        &format,
        &brush
    );
}

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

    StringFormat format;

    format.SetTrimming(
        StringTrimmingEllipsisCharacter
    );

    graphics.DrawString(
        album.c_str(),
        -1,
        &font,
        RectF(
            S(CONTENT_X),
            S(49.0f),
            S(ALBUM_WIDTH),
            S(14.0f)
        ),
        &format,
        &brush
    );
}

static void DrawProgress(
    Graphics& graphics,
    double position,
    double duration
)
{
    float ratio = 0;

    if (duration > 0)
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
            65,
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

    if (ratio > 0)
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
                S(
                    PROGRESS_W *
                    ratio
                ),
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

static void DrawDuration(
    Graphics& graphics,
    double position,
    double duration
)
{
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

    const std::wstring left =
        FormatTime(position);

    const std::wstring right =
        FormatTime(duration);

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

    StringFormat format;

    format.SetAlignment(
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
        &format,
        &brush
    );
}

// ============================================================
// ICONS
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
        S(radius * 2),
        S(radius * 2)
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

    if (type == 0)
    {
        DrawPreviousIcon(
            graphics,
            S(cx),
            S(cy),
            S(size),
            &icon
        );
    }
    else if (type == 1)
    {
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
    }
    else
    {
        DrawNextIcon(
            graphics,
            S(cx),
            S(cy),
            S(size),
            &icon
        );
    }
}

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
        15,
        0,
        playing,
        false
    );

    DrawMediaButton(
        graphics,
        PLAY_X,
        CONTROL_Y,
        18,
        1,
        playing,
        true
    );

    DrawMediaButton(
        graphics,
        NEXT_X,
        CONTROL_Y,
        15,
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

    if (
        SameText(
            g_cachedTitle,
            title
        )
    )
    {
        return;
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
            0,
            0
        ),
        &measured
    );

    g_cachedTitleWidth =
        static_cast<int>(
            std::ceil(
                measured.Width /
                g_scale
            )
        );

    g_cachedTitleOverflow =
        g_cachedTitleWidth >
        static_cast<int>(
            TITLE_WIDTH
        );

    g_cachedTitle =
        title;

    std::scoped_lock lock(
        g_stateMutex
    );

    ResetMarqueeLocked();
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
        TextRenderingHintAntiAliasGridFit
    );

    graphics.SetInterpolationMode(
        InterpolationModeHighQualityBicubic
    );

    graphics.Clear(
        Color(
            255,
            16,
            16,
            21
        )
    );

    DrawBackground(graphics);

    DrawCover(graphics);

    UpdateTitleMetrics(graphics);

    DrawTitle(graphics);

    DrawArtist(graphics);

    DrawAlbum(graphics);

    DrawClock(graphics);

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

    DrawControls(graphics);
}

// ============================================================
// PRESENT
// ============================================================

static void PresentFrame()
{
    if (!g_hwnd)
        return;

    if (!g_backDC)
        return;

    HDC dc =
        GetDC(g_hwnd);

    if (!dc)
        return;

    BitBlt(
        dc,
        0,
        0,
        g_bufferWidth,
        g_bufferHeight,
        g_backDC,
        0,
        0,
        SRCCOPY
    );

    ReleaseDC(
        g_hwnd,
        dc
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
    int clientX,
    int clientY
)
{
    const float scale =
        g_scale <= 0
            ? 1.0f
            : g_scale;

    const int x =
        static_cast<int>(
            clientX / scale
        );

    const int y =
        static_cast<int>(
            clientY / scale
        );

    auto circleHit =
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
        circleHit(
            x,
            y,
            static_cast<int>(PREV_X),
            static_cast<int>(CONTROL_Y),
            22
        )
    )
    {
        return HitArea::Previous;
    }

    if (
        circleHit(
            x,
            y,
            static_cast<int>(PLAY_X),
            static_cast<int>(CONTROL_Y),
            25
        )
    )
    {
        return HitArea::PlayPause;
    }

    if (
        circleHit(
            x,
            y,
            static_cast<int>(NEXT_X),
            static_cast<int>(CONTROL_Y),
            22
        )
    )
    {
        return HitArea::Next;
    }

    if (
        x >= static_cast<int>(PROGRESS_X) &&
        x <= static_cast<int>(
            PROGRESS_X + PROGRESS_W
        ) &&
        y >= 58 &&
        y <= 82
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
            POINT point{
                GET_X_LPARAM(lParam),
                GET_Y_LPARAM(lParam)
            };

            ScreenToClient(
                hwnd,
                &point
            );

            return
                HitTest(
                    point.x,
                    point.y
                ) != HitArea::None
                    ? HTCLIENT
                    : HTTRANSPARENT;
        }

        case WM_LBUTTONDOWN:
        {
            const int x =
                GET_X_LPARAM(lParam);

            const int y =
                GET_Y_LPARAM(lParam);

            const HitArea area =
                HitTest(
                    x,
                    y
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

                    if (duration > 0)
                    {
                        const double logicalX =
                            static_cast<double>(x) /
                            g_scale;

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
                            duration * ratio
                        );
                    }

                    break;
                }

                default:
                    break;
            }

            return 0;
        }

        case WM_PAINT:
        {
            PAINTSTRUCT ps{};

            HDC dc =
                BeginPaint(
                    hwnd,
                    &ps
                );

            if (g_backDC)
            {
                BitBlt(
                    dc,
                    0,
                    0,
                    g_bufferWidth,
                    g_bufferHeight,
                    g_backDC,
                    0,
                    0,
                    SRCCOPY
                );
            }

            EndPaint(
                hwnd,
                &ps
            );

            return 0;
        }

        case WM_ERASEBKGND:
        {
            return 1;
        }

        case WM_MALSS_UPDATE:
        {
            if (g_visible.load())
            {
                PositionOverlay();

                RenderFrame();

                InvalidateRect(
                    hwnd,
                    nullptr,
                    FALSE
                );

                UpdateWindow(
                    hwnd
                );
            }
            else
            {
                ShowWindow(
                    hwnd,
                    SW_HIDE
                );
            }

            return 0;
        }

        case WM_DPICHANGED:
        {
            g_dpi =
                HIWORD(wParam);

            if (!g_dpi)
                g_dpi = 96;

            g_scale =
                static_cast<float>(
                    g_dpi
                ) /
                96.0f;

            DestroyBackBuffer();

            PositionOverlay();

            UpdateWindowRegion();

            RenderFrame();

            InvalidateRect(
                hwnd,
                nullptr,
                FALSE
            );

            return 0;
        }

        case WM_DISPLAYCHANGE:
        case WM_SETTINGCHANGE:
        {
            PositionOverlay();

            g_forceRedraw.store(
                true
            );

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

        case WM_NCDESTROY:
        {
            if (g_hwnd == hwnd)
                g_hwnd = nullptr;

            break;
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
        !RegisterClassExW(&wc)
    )
    {
        Wh_Log(
            L"[UI] RegisterClassExW failed: %lu",
            GetLastError()
        );

        return;
    }

    //
    // TOP-LEVEL WINDOW
    //
    // No SetParent.
    // No WorkerW.
    // No WS_CHILD.
    // No HWND_BOTTOM.
    // No UpdateLayeredWindow.
    //

    g_hwnd =
        CreateWindowExW(
            WS_EX_TOOLWINDOW |
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
            g_hInstance,
            nullptr
        );

    if (!g_hwnd)
    {
        Wh_Log(
            L"[UI] CreateWindowExW failed: %lu",
            GetLastError()
        );

        UnregisterClassW(
            wc.lpszClassName,
            g_hInstance
        );

        return;
    }

    UpdateDpi();

    PositionOverlay();

    UpdateWindowRegion();

    if (!CreateBackBuffer())
    {
        Wh_Log(
            L"[UI] CreateBackBuffer failed"
        );
    }

    RenderFrame();

    ShowWindow(
        g_hwnd,
        SW_SHOWNOACTIVATE
    );

    UpdateWindow(
        g_hwnd
    );

    InvalidateRect(
        g_hwnd,
        nullptr,
        FALSE
    );

    Wh_Log(
        L"[UI] Overlay created hwnd=%p",
        g_hwnd
    );

    MSG msg{};

    int64_t lastQpc =
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

        const int64_t currentQpc =
            QpcNow();

        double dt =
            QpcSecondsSince(
                lastQpc
            );

        lastQpc =
            currentQpc;

        if (dt > 0.1)
            dt = 0.1;

        const bool playing =
            g_playing.load();

        bool titleOverflow =
            false;

        {
            std::scoped_lock lock(
                g_stateMutex
            );

            titleOverflow =
                g_cachedTitleOverflow;

            if (
                playing &&
                titleOverflow
            )
            {
                g_marqueeTimer += dt;

                if (
                    g_marqueePhase ==
                    MarqueePhase::Hold
                )
                {
                    g_marqueeOffset =
                        0.0;

                    if (
                        g_marqueeTimer >=
                        MARQUEE_HOLD
                    )
                    {
                        g_marqueeTimer =
                            0.0;

                        g_marqueePhase =
                            MarqueePhase::Scroll;
                    }
                }
                else
                {
                    g_marqueeOffset -=
                        MARQUEE_SPEED *
                        dt;

                    const double cycle =
                        g_cachedTitleWidth +
                        MARQUEE_GAP;

                    if (
                        -g_marqueeOffset >=
                        cycle
                    )
                    {
                        g_marqueeOffset =
                            0.0;

                        g_marqueeTimer =
                            0.0;

                        g_marqueePhase =
                            MarqueePhase::Hold;
                    }
                }
            }
        }

        SYSTEMTIME st{};

        GetLocalTime(
            &st
        );

        const int minute =
            st.wHour * 60 +
            st.wMinute;

        const bool minuteChanged =
            minute !=
            lastMinute;

        if (minuteChanged)
            lastMinute = minute;

        if (
            playing ||
            minuteChanged ||
            g_forceRedraw.exchange(false)
        )
        {
            RenderFrame();

            InvalidateRect(
                g_hwnd,
                nullptr,
                FALSE
            );

            UpdateWindow(
                g_hwnd
            );
        }

        int sleepMs =
            playing && titleOverflow
                ? 16
                : playing
                    ? 33
                    : 250;

        Sleep(
            sleepMs
        );
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
// GDI+
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

// ============================================================
// TOOL INIT
// ============================================================

BOOL WhTool_ModInit()
{
    g_hInstance =
        GetCurrentModuleHandle();

    if (!g_hInstance)
        return FALSE;

    LoadSettings();

    g_running.store(true);
    g_visible.store(false);
    g_playing.store(false);
    g_forceRedraw.store(true);

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
            0;

        g_clockAnchorQpc =
            QpcNow();

        g_clockRunning =
            false;

        g_frozenPosition =
            0;

        g_cachedTitle.clear();

        g_cachedTitleWidth =
            0;

        g_cachedTitleOverflow =
            false;

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
        g_running.store(false);

        g_commandCv.notify_all();

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
        L"Mallss Music Overlay started"
    );

    return TRUE;
}

// ============================================================
// SETTINGS CHANGED
// ============================================================

void WhTool_ModSettingsChanged()
{
    LoadSettings();

    g_forceRedraw.store(
        true
    );

    if (g_hwnd)
    {
        PositionOverlay();

        UpdateWindowRegion();

        DestroyBackBuffer();

        RenderFrame();

        InvalidateRect(
            g_hwnd,
            nullptr,
            FALSE
        );

        UpdateWindow(
            g_hwnd
        );
    }
}

// ============================================================
// UNINIT
// ============================================================

void WhTool_ModUninit()
{
    Wh_Log(
        L"Mallss Music Overlay shutting down"
    );

    g_running.store(false);

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
            0;

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
        L"Mallss Music Overlay stopped"
    );
}

// ============================================================================
// Windhawk tool-mod launcher
// ============================================================================

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook()
{
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit()
{
    DWORD sessionId;

    if (
        ProcessIdToSessionId(
            GetCurrentProcessId(),
            &sessionId
        ) &&
        sessionId == 0
    )
    {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;

    int argc;

    LPWSTR* argv =
        CommandLineToArgvW(
            GetCommandLine(),
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
        i++
    )
    {
        if (
            wcscmp(
                argv[i],
                L"-service"
            ) == 0 ||
            wcscmp(
                argv[i],
                L"-service-start"
            ) == 0 ||
            wcscmp(
                argv[i],
                L"-service-stop"
            ) == 0
        )
        {
            isExcluded =
                true;

            break;
        }
    }

    for (
        int i = 1;
        i < argc - 1;
        i++
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

    LocalFree(argv);

    if (isExcluded)
        return FALSE;

    if (isCurrentToolModProcess)
    {
        g_toolModProcessMutex =
            CreateMutex(
                nullptr,
                TRUE,
                L"windhawk-tool-mod_" WH_MOD_ID
            );

        if (!g_toolModProcessMutex)
        {
            Wh_Log(
                L"CreateMutex failed"
            );

            ExitProcess(1);
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

            ExitProcess(1);
        }

        if (!WhTool_ModInit())
        {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)
                GetModuleHandle(nullptr);

        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)(
                (BYTE*)dosHeader +
                dosHeader->e_lfanew
            );

        DWORD entryPointRVA =
            ntHeaders->
                OptionalHeader.
                AddressOfEntryPoint;

        void* entryPoint =
            (BYTE*)dosHeader +
            entryPointRVA;

        Wh_SetFunctionHook(
            entryPoint,
            (void*)EntryPoint_Hook,
            nullptr
        );

        return TRUE;
    }

    if (isToolModProcess)
        return FALSE;

    g_isToolModProcessLauncher =
        true;

    return TRUE;
}

void Wh_ModAfterInit()
{
    if (!g_isToolModProcessLauncher)
        return;

    WCHAR currentProcessPath[MAX_PATH];

    if (
        GetModuleFileName(
            nullptr,
            currentProcessPath,
            ARRAYSIZE(currentProcessPath)
        ) == 0
    )
    {
        Wh_Log(
            L"GetModuleFileName failed"
        );

        return;
    }

    WCHAR commandLine[
        MAX_PATH +
        2 +
        (
            sizeof(
                L" -tool-mod \"" WH_MOD_ID "\""
            ) /
            sizeof(WCHAR)
        ) -
        1
    ];

    swprintf_s(
        commandLine,
        L"\"%s\" -tool-mod \"%s\"",
        currentProcessPath,
        WH_MOD_ID
    );

    HMODULE kernelModule =
        GetModuleHandle(
            L"kernelbase.dll"
        );

    if (!kernelModule)
    {
        kernelModule =
            GetModuleHandle(
                L"kernel32.dll"
            );
    }

    if (!kernelModule)
    {
        Wh_Log(
            L"No kernelbase.dll/kernel32.dll"
        );

        return;
    }

    //
    // Correct CreateProcessInternalW signature:
    //
    // 1  hToken
    // 2  applicationName
    // 3  commandLine
    // 4  processAttributes
    // 5  threadAttributes
    // 6  inheritHandles
    // 7  creationFlags
    // 8  environment
    // 9  currentDirectory
    // 10 startupInfo
    // 11 processInformation
    // 12 newToken
    //
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
            LPSTARTUPINFO,
            LPPROCESS_INFORMATION,
            PHANDLE
        );

    auto pCreateProcessInternalW =
        (CreateProcessInternalW_t)
            GetProcAddress(
                kernelModule,
                "CreateProcessInternalW"
            );

    if (!pCreateProcessInternalW)
    {
        Wh_Log(
            L"No CreateProcessInternalW"
        );

        return;
    }

    STARTUPINFO si{
        .cb =
            sizeof(STARTUPINFO),
        .dwFlags =
            STARTF_FORCEOFFFEEDBACK,
    };

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
            L"CreateProcess failed"
        );

        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged()
{
    if (g_isToolModProcessLauncher)
        return;

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit()
{
    if (g_isToolModProcessLauncher)
        return;

    WhTool_ModUninit();

    ExitProcess(0);
}
