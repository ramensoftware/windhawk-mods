// ==WindhawkMod==
// @id              simple-media-progress-bar
// @name            Simple Media Progress Bar
// @description     Adds a seekable progress bar with elapsed and total time to the media card in Windows 11 Quick Settings
// @version         1.0.0
// @author          n3thshan
// @github          https://github.com/n3thshan
// @include         ShellHost.exe
// @architecture    x86-64
// @compilerOptions -DWIN32_LEAN_AND_MEAN -lole32 -loleaut32 -lruntimeobject -luuid -luser32
// @license         GPL-3.0-only
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Adds a seek bar to the media card of the Windows 11 (24H2 and newer) Quick
Settings panel, so the playback position can be changed without opening the
media app.

![Simple Media Progress Bar in Quick Settings](https://raw.githubusercontent.com/n3thshan/wh-simple-media-progress-bar/main/assets/quick-settings.png)

The bar is inserted into the card itself, so it uses the real XAML control, the
real accent color, the real hover and press animations, and the real
accessibility tree. The native previous/play/next buttons are left untouched
and keep their exact look and position.

Dragging the thumb moves it immediately; the playback position is sent to the
media app once, when the thumb is released. The elapsed time on the left follows
the thumb while dragging.

Only media apps that report a seekable timeline get the extra row. Live
streams and apps that do not support seeking are shown without it, exactly as
before.

Notes:

* After enabling the mod, close and reopen Quick Settings once. Windows builds
  the panel on demand, so the bar appears the next time the panel opens.
* The card is about 36 DIP taller while the bar is shown. On short screens the
  rest of the panel moves down by that amount.
* Requires Windows 11 24H2 (build 26100) or newer.
* Source and issue reports:
  https://github.com/n3thshan/wh-simple-media-progress-bar
*/
// ==/WindhawkModReadme==

#include <windows.h>

// WIN32_LEAN_AND_MEAN (set in @compilerOptions) makes <windows.h> skip the
// traditional COM headers, which leaves IUnknown as a forward declaration and
// breaks the QueryInterface call used for element discovery. Include it
// explicitly; C++/WinRT keys off __IUnknown_INTERFACE_DEFINED__.
#include <unknwn.h>

#include <windhawk_utils.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.UI.Core.h>

// <windows.h> defines GetCurrentTime as a macro, which collides with the XAML
// animation interface.
#undef GetCurrentTime

#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Automation.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cwchar>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace X = winrt::Windows::UI::Xaml;
namespace C = X::Controls;
namespace P = C::Primitives;
namespace M = X::Media;
namespace GSMTC = winrt::Windows::Media::Control;

using winrt::Windows::Foundation::IInspectable;
using winrt::Windows::Foundation::TimeSpan;

namespace {

// ---------------------------------------------------------------------------
// XAML helpers
// ---------------------------------------------------------------------------

std::wstring ClassOf(IInspectable const& object) {
    return object ? std::wstring(winrt::get_class_name(object)) : std::wstring();
}

template <class F>
X::FrameworkElement Find(X::DependencyObject const& root, F const& match) {
    if (!root) {
        return nullptr;
    }
    if (auto element = root.try_as<X::FrameworkElement>(); element && match(element)) {
        return element;
    }
    int count = M::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        if (auto found = Find(M::VisualTreeHelper::GetChild(root, i), match)) {
            return found;
        }
    }
    return nullptr;
}

X::FrameworkElement ParentOf(X::DependencyObject const& element) {
    return M::VisualTreeHelper::GetParent(element).try_as<X::FrameworkElement>();
}

// mm:ss below one hour, h:mm:ss above it. A track that is an hour or longer
// always uses the h:mm:ss form, even while the elapsed label is still below
// the one-hour mark, so its width does not jump across that boundary.
winrt::hstring FormatTime(int64_t ticks, bool forceHours = false) {
    int64_t seconds = ticks / 10000000;
    if (seconds < 0) {
        seconds = 0;
    }
    int64_t hours = seconds / 3600;
    int64_t minutes = (seconds / 60) % 60;
    int64_t rest = seconds % 60;
    wchar_t buffer[32];
    if (forceHours || hours > 0) {
        swprintf(buffer, ARRAYSIZE(buffer), L"%lld:%02lld:%02lld", hours, minutes, rest);
    } else {
        swprintf(buffer, ARRAYSIZE(buffer), L"%lld:%02lld", minutes, rest);
    }
    return winrt::hstring(buffer);
}

// ---------------------------------------------------------------------------
// Media session state
//
// A dedicated MTA thread owns every GSMTC object. The XAML thread only reads a
// plain snapshot and posts seek requests through an atomic, so no COM call ever
// happens on the shell's UI thread.
// ---------------------------------------------------------------------------

}  // namespace

namespace media {

struct Snapshot {
    bool valid = false;    // A usable start/end range exists.
    bool canSeek = false;  // The session reports playback-position control.
    bool playing = false;
    int64_t start = 0;
    int64_t position = 0;
    int64_t end = 0;
};

std::mutex g_mutex;
Snapshot g_snapshot;

std::atomic<bool> g_active{false};
std::atomic<bool> g_watch{false};
std::atomic<bool> g_stop{false};
std::atomic<int64_t> g_seek{-1};

HANDLE g_thread = nullptr;
HANDLE g_stopEvent = nullptr;
HANDLE g_wakeEvent = nullptr;

Snapshot Get() {
    std::lock_guard<std::mutex> lock(g_mutex);
    return g_snapshot;
}

void Store(Snapshot const& snapshot) {
    std::lock_guard<std::mutex> lock(g_mutex);
    g_snapshot = snapshot;
}

void Wake() {
    if (g_wakeEvent) {
        SetEvent(g_wakeEvent);
    }
}

void RequestSeek(int64_t ticks) {
    g_seek.store(ticks);
    Wake();
}

// Position tracking. A session's timeline is a sample of the playhead taken at
// LastUpdatedTime, and players publish a new sample roughly once per second.
//
// Trusting a sample blindly (position + "now - LastUpdatedTime") breaks in two
// ways: a player that stops publishing while it keeps playing freezes the bar
// once the sample is old, and a player that applies a seek but never publishes
// the new timeline makes the bar extrapolate from the *old* position, leaving it
// minutes behind the audio. Both are fixed by keeping our own monotonic
// baseline: a new sample resynchronises it, silence does not.
//
// Everything below is touched by the media thread only.
int64_t g_sample = -1;    // LastUpdatedTime of the accepted sample, -1 if none.
int64_t g_reported = -1;  // Last raw Position, for players without a timestamp.
int64_t g_basePos = -1;   // Absolute position at g_baseTick.
int64_t g_baseTick = 0;   // GetTickCount64 at g_basePos, in 100 ns units.
int64_t g_start = 0;
int64_t g_end = 0;
bool g_wasPlaying = false;
// Set by a seek and cleared by the next published sample (or a timeout): the
// timeline still describes the position before the seek until then.
bool g_seekPending = false;
int64_t g_seekPendingUntil = 0;
int64_t g_seekTarget = -1;
int64_t g_preSeek = -1;  // Projection just before the seek, -1 if unknown.
int64_t g_lastPoll = 0;  // MonoNow of the previous read, to spot a wake-up gap.
// A timeline sample that is this close to where playback was before the seek is
// the pre-seek timeline, not the result of the seek.
constexpr int64_t kStaleWindow = 20000000;  // 2 seconds

int64_t MonoNow() {
    return static_cast<int64_t>(GetTickCount64()) * 10000;
}

int64_t WallNow() {
    return winrt::clock::now().time_since_epoch().count();
}

int64_t ClampPosition(int64_t value, int64_t start, int64_t end) {
    return std::clamp(value, start, std::max(start, end));
}

// Where the playhead should be right now, according to the last accepted sample.
int64_t Projected() {
    int64_t value = g_basePos;
    if (g_wasPlaying && g_baseTick > 0 && value >= 0) {
        value += MonoNow() - g_baseTick;
    }
    return value;
}

void ResetTracking() {
    g_sample = -1;
    g_reported = -1;
    g_basePos = -1;
    g_baseTick = 0;
    g_start = 0;
    g_end = 0;
    g_wasPlaying = false;
    g_seekPending = false;
    g_seekPendingUntil = 0;
    g_seekTarget = -1;
    g_preSeek = -1;
    g_lastPoll = 0;
}

void Refresh(GSMTC::GlobalSystemMediaTransportControlsSessionManager const& manager) {
    Snapshot next;
    try {
        auto session = manager.GetCurrentSession();
        if (!session) {
            ResetTracking();
            Store(next);
            return;
        }

        auto playback = session.GetPlaybackInfo();
        if (playback) {
            next.playing = playback.PlaybackStatus() ==
                           GSMTC::GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing;
            if (auto controls = playback.Controls()) {
                next.canSeek = controls.IsPlaybackPositionEnabled();
            }
        }

        auto timeline = session.GetTimelineProperties();
        int64_t start = timeline.StartTime().count();
        int64_t end = timeline.EndTime().count();
        int64_t reported = timeline.Position().count();
        int64_t sample = timeline.LastUpdatedTime().time_since_epoch().count();

        next.start = start;
        next.end = end;
        next.valid = end > start;
        if (!next.valid) {
            ResetTracking();
            Store(next);
            return;
        }

        int64_t nowMono = MonoNow();
        // A gap of more than ten seconds means the machine slept or the thread
        // was suspended. The projection is meaningless across that gap, so
        // resynchronise from the sample instead of counting it as elapsed time.
        bool woke = g_lastPoll > 0 && nowMono - g_lastPoll > 100000000LL;
        g_lastPoll = nowMono;
        reported = ClampPosition(reported, start, end);
        bool trackChanged = start != g_start || end != g_end;
        // Some players never set LastUpdatedTime; for those, any change of the
        // reported position is a new sample.
        bool newSample = sample > 0 ? sample != g_sample : reported != g_reported;
        bool resumed = next.playing && !g_wasPlaying;

        if (trackChanged) {
            // A different track makes the pre-seek timeline meaningless: the
            // seek context is over.
            g_seekPending = false;
            g_seekTarget = -1;
        }
        if (woke) {
            // The projection is meaningless across the gap; resync from the
            // sample. The pre-seek position may still be stale, so the seek
            // target is kept.
            g_seekPending = false;
        }
        // A seek the player has not confirmed yet is still outstanding.
        // While paused this holds even after the pending timeout, because
        // many players only publish the post-seek timeline once playback
        // resumes (and some never publish it at all).
        bool seekOutstanding = g_seekPending || (g_seekTarget >= 0 && !next.playing);
        if (seekOutstanding && newSample) {
            // The player may publish one last old timeline right after the seek
            // is applied. Applying it would drag the bar back to where playback
            // was before the seek, so recognise it by comparing it with the
            // pre-seek position and ignore it while continuing to project from
            // the requested target. A sample that is not near the old position
            // (a keyframe seek, a different target) is accepted normally.
            int64_t position = reported;
            if (sample > 0) {
                int64_t age = WallNow() - sample;
                if (age > 0 && age < 36000000000LL) {
                    position += age;
                }
            }
            position = ClampPosition(position, start, end);
            bool stale = g_preSeek >= 0 && g_seekTarget >= 0 &&
                         std::llabs(position - g_preSeek) <= kStaleWindow &&
                         std::llabs(g_seekTarget - g_preSeek) > 2 * kStaleWindow;
            if (stale) {
                // Remember it as seen so it is not applied later, but keep the
                // seek baseline.
                g_sample = sample;
                g_reported = reported;
                newSample = false;
            } else {
                // A genuinely new sample, distinct from the pre-seek
                // position: the player has confirmed the seek.
                g_seekPending = false;
                g_seekTarget = -1;
            }
        }
        if (nowMono > g_seekPendingUntil) {
            // The timeout only ends the "just seeked" urgency. While paused,
            // the position stays parked on the seek target until the player
            // actually publishes a post-seek sample.
            g_seekPending = false;
        }

        bool rebase;
        if (g_seekPending) {
            rebase = false;
        } else if (resumed && g_basePos >= 0) {
            // Came back from a pause. The frozen sample does not describe the
            // paused time, so do not add its age: just start counting from the
            // position where playback stopped and let the next sample correct it.
            g_baseTick = nowMono;
            rebase = false;
        } else {
            // Paused with an unconfirmed seek: the reported timeline is
            // still the pre-seek one. Park on the seek target instead of
            // rebasing backwards; a genuinely new sample (handled above)
            // clears g_seekTarget and re-enables the normal path.
            bool holdForSeek = !next.playing && g_seekTarget >= 0 && g_basePos >= 0;
            rebase = !holdForSeek &&
                     (newSample || !next.playing || trackChanged || g_basePos < 0 || woke);
        }
        if (rebase) {
            // Rebase on the sample. It describes the playhead at
            // LastUpdatedTime, so its age is added to get where it is now.
            // That is known to be wrong for a sample published before a pause
            // and then resumed, so only add the age when:
            //
            // * we have already seen this session playing (with no track
            //   change), or
            // * this is the first accepted sample while the session is playing.
            //   An initial zero baseline is not a real track change.
            //
            // If the first sample's age would move past the end, the sample
            // almost certainly includes time that was spent paused; fall back
            // to the raw reported position instead of pretending playback is
            // finished.
            int64_t position = reported;
            bool hadBaseline = g_basePos >= 0;
            bool initialPlayingBaseline = !hadBaseline && next.playing;
            // Only add the sample's age if the session is actively playing
            // right now. A sample published before a pause must not be
            // aged across it.
            bool knownPlayingBaseline =
                hadBaseline && g_wasPlaying && next.playing && !trackChanged;
            if (sample > 0 && (knownPlayingBaseline || initialPlayingBaseline)) {
                int64_t age = WallNow() - sample;
                if (age > 0 && age < 36000000000LL) {  // Ignore ages above 1 hour.
                    int64_t withAge = reported + age;
                    if (withAge <= end) {
                        position = withAge;
                    }
                }
            }
            g_basePos = ClampPosition(position, start, end);
            g_baseTick = nowMono;
            g_sample = sample;
        }
        // Otherwise no new information arrived: keep projecting from the last
        // accepted position instead of freezing or jumping backwards.

        g_reported = reported;
        g_start = start;
        g_end = end;
        g_wasPlaying = next.playing;

        int64_t projected = g_basePos;
        if (next.playing) {
            projected += nowMono - g_baseTick;
        }
        next.position = ClampPosition(projected, start, end);
    } catch (...) {
        // A session that disappears mid-read is not an error.
    }
    Store(next);
}

void ApplySeek(GSMTC::GlobalSystemMediaTransportControlsSessionManager const& manager, int64_t ticks) {
    try {
        auto session = manager.GetCurrentSession();
        if (!session) {
            return;
        }
        if (!session.TryChangePlaybackPositionAsync(ticks).get()) {
            return;
        }
        // The player has moved, but its timeline may still describe the old
        // position (and some players never publish the post-seek position at
        // all). Move our own baseline to the target so the bar cannot drift
        // back to where playback was before the seek.
        int64_t start = 0;
        int64_t end = 0;
        int64_t target = ticks;
        {
            std::lock_guard<std::mutex> lock(g_mutex);
            start = g_snapshot.start;
            end = g_snapshot.end;
            target = end > start ? std::clamp(ticks, start, end) : ticks;
            g_snapshot.position = target;
        }
        g_preSeek = Projected();
        g_seekTarget = target;
        g_basePos = target;
        g_baseTick = MonoNow();
        g_seekPending = true;
        g_seekPendingUntil = g_baseTick + 100000000LL;  // 10 seconds
    } catch (...) {
    }
}

DWORD WINAPI ThreadProc(void*) {
    try {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
    } catch (...) {
        Wh_Log(L"Media thread could not join an apartment: %08X", winrt::to_hresult());
        return 0;
    }
    try {
        auto operation = GSMTC::GlobalSystemMediaTransportControlsSessionManager::RequestAsync();
        while (operation.Status() == winrt::Windows::Foundation::AsyncStatus::Started) {
            if (WaitForSingleObject(g_stopEvent, 20) == WAIT_OBJECT_0) {
                winrt::uninit_apartment();
                return 0;
            }
        }
        auto manager = operation.GetResults();
        if (!manager) {
            Wh_Log(L"No media session manager available");
            winrt::uninit_apartment();
            return 0;
        }

        HANDLE handles[] = {g_stopEvent, g_wakeEvent};
        while (!g_stop.load(std::memory_order_relaxed)) {
            bool active = g_active.load(std::memory_order_relaxed);
            DWORD wait = WaitForMultipleObjects(ARRAYSIZE(handles), handles, FALSE,
                                                active ? 200 : 1000);
            if (wait == WAIT_OBJECT_0) {
                break;
            }

            // A seek is applied even when the panel was just closed.
            int64_t target = g_seek.exchange(-1);
            if (target >= 0) {
                ApplySeek(manager, target);
            }
            // Re-read the flag: the wake event may have been set because the
            // panel just opened, after the value above was sampled.
            if (!g_watch.load(std::memory_order_relaxed)) {
                continue;
            }
            Refresh(manager);
        }
    } catch (...) {
        Wh_Log(L"Media thread failed: %08X", winrt::to_hresult());
    }
    winrt::uninit_apartment();
    return 0;
}

void Start() {
    if (g_thread) {
        return;
    }
    g_stop.store(false);
    g_seek.store(-1);
    ResetTracking();
    g_active.store(false);
    g_watch.store(false);
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_wakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_stopEvent || !g_wakeEvent) {
        if (g_stopEvent) {
            CloseHandle(g_stopEvent);
            g_stopEvent = nullptr;
        }
        if (g_wakeEvent) {
            CloseHandle(g_wakeEvent);
            g_wakeEvent = nullptr;
        }
        return;
    }
    g_thread = CreateThread(nullptr, 0, ThreadProc, nullptr, 0, nullptr);
    if (!g_thread) {
        CloseHandle(g_stopEvent);
        CloseHandle(g_wakeEvent);
        g_stopEvent = g_wakeEvent = nullptr;
    }
}

void Stop() {
    g_stop.store(true);
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }
    if (g_wakeEvent) {
        SetEvent(g_wakeEvent);
    }
    if (g_thread) {
        WaitForSingleObject(g_thread, 5000);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
    if (g_wakeEvent) {
        CloseHandle(g_wakeEvent);
        g_wakeEvent = nullptr;
    }
}

}  // namespace media

namespace {

// ---------------------------------------------------------------------------
// The seek row
// ---------------------------------------------------------------------------

constexpr PCWSTR kViewClass = L"ControlCenter.ControlCenterView";
constexpr PCWSTR kRegionName = L"MediaTransportControlsRegion";
constexpr PCWSTR kRootName = L"MediaTransportControlsRoot";
constexpr PCWSTR kButtonsName = L"MediaButtonsListView";

// 32 DIP bar plus a 4 DIP gap above it, so the card grows by exactly 36 DIP and
// the button row keeps its original 48 DIP.
constexpr double kBarHeight = 32.0;
constexpr double kRowGap = 4.0;
constexpr double kCardGrowth = kBarHeight + kRowGap;

std::atomic<bool> g_stopping{false};

// ---------------------------------------------------------------------------
// Cross-thread dispatch
//
// Everything XAML-related is released on the UI thread that owns it. A
// CoreDispatcher.RunAsync hop is enough and does not depend on the window
// still existing.
// ---------------------------------------------------------------------------

std::mutex g_threadsMutex;
// Weak so that a dispatcher for a torn-down XAML island cannot keep the island
// (and the flyout it belongs to) alive. If it no longer resolves there is
// nothing left to clean up, because the row died with the island.
std::map<DWORD, winrt::weak_ref<winrt::Windows::UI::Core::CoreDispatcher>> g_threads;

void RememberThread(DWORD id, winrt::Windows::UI::Core::CoreDispatcher const& dispatcher) {
    if (!dispatcher) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_threadsMutex);
    auto it = g_threads.find(id);
    if (it == g_threads.end()) {
        g_threads.emplace(id, winrt::make_weak(dispatcher));
    } else {
        it->second = winrt::make_weak(dispatcher);
    }
}

class Patcher;

}  // namespace

// Defined after Patcher.
namespace {
void StopOnCurrentThread();
}

namespace {

// The win32 timer callback runs on the thread that owns the message queue, so a
// thread-local pointer is the right way to get back to the patcher from it.
thread_local Patcher* g_timerOwner = nullptr;

class Patcher {
   public:
    Patcher() : threadId_(GetCurrentThreadId()) {}

    ~Patcher() { Restore(); }

    // Drives the injected row. A Win32 thread timer is used instead of
    // Windows.UI.Xaml.DispatcherTimer: the default DispatcherTimer constructor
    // binds to the CoreWindow dispatcher, and ShellHost hosts this XAML in an
    // island without a CoreWindow, so the timer was created but never ticked.
    // A thread timer only needs a pumped message queue, which a UI thread has.
    void StartTimer() {
        // Starting the timer means the panel is opening again. The countdown
        // that parked the timer must not carry over: on the opening frame DWM
        // still reports the flyout as cloaked, and a stale count of two would
        // stop the freshly started timer on its very first update.
        hiddenTicks_ = 0;
        if (timerId_) {
            return;
        }
        g_timerOwner = this;
        timerId_ = SetTimer(nullptr, 0, 200, &Patcher::OnTimer);
        if (!timerId_) {
            Wh_Log(L"Update timer could not be created: %u", GetLastError());
            g_timerOwner = nullptr;
            return;
        }
        // The timer is restarted every time the panel opens, so log only the
        // first time to keep the log readable.
        static std::atomic<bool> logged{false};
        if (!logged.exchange(true)) {
            Wh_Log(L"Update timer started");
        }
    }

    void StopTimer() {
        if (timerId_) {
            KillTimer(nullptr, timerId_);
            timerId_ = 0;
        }
        if (g_timerOwner == this) {
            g_timerOwner = nullptr;
        }
    }

   private:
    static void CALLBACK OnTimer(HWND, UINT, UINT_PTR, DWORD) {
        // Nothing may escape a timer callback: it runs inside user32's message
        // dispatch, where an exception can take the shell down or wedge the
        // thread's message loop.
        try {
            if (auto patcher = g_timerOwner) {
                patcher->Tick();
            }
        } catch (...) {
            Wh_Log(L"Seek bar tick failed: %08X", winrt::to_hresult());
        }
    }

    void Tick() {
        if (g_stopping || restored_) {
            return;
        }
        ULONGLONG started = GetTickCount64();
        // Nothing on screen: leave the XAML tree alone. The hidden-state
        // bookkeeping (counter, timer shutdown) happens in UpdateCore.
        if (HostVisible()) {
            ProcessPending();
        }
        Update();
        // If the panel ever fails to open, a slow tick is a likely reason: this
        // runs on the shell's UI thread, and a XAML or window query that blocks
        // there can stall the whole flyout.
        ULONGLONG took = GetTickCount64() - started;
        if (took > 50 && slowTicks_ < 10) {
            slowTicks_++;
            Wh_Log(L"Slow tick: %llu ms", took);
        }
    }

   public:

    void Offer(X::FrameworkElement const& element, bool deferred) {
        if (g_stopping || !element) {
            return;
        }
        if (deferred) {
            // Connect runs in the middle of InitializeComponent, so this element
            // is still half constructed. Do not touch it at all beyond keeping a
            // weak reference: querying its Dispatcher or its place in the tree
            // here is what can make the shell fail to open the panel at all.
            // The focus hook fires after construction and paints the bar.
            try {
                if (pending_.size() < 32) {
                    pending_.push_back(winrt::make_weak(element));
                }
            } catch (...) {
            }
            // A plain user32 call and an event signal, both safe here and
            // independent of XAML. It lets the timer pick the element up even
            // on a build where the focus hook never runs. The flags are armed
            // first so the media thread polls as soon as it wakes up; the
            // hidden ticks stop it again if the panel never actually shows.
            media::g_watch.store(true, std::memory_order_relaxed);
            media::g_active.store(true, std::memory_order_relaxed);
            hiddenTicks_ = 0;
            StartTimer();
            media::Wake();
            return;
        }
        // OnGotFocus is a normal, completed view: working on it here is what
        // the published Control Center mods do as well.
        Discover(element);
        WakeUp();
    }

    // The panel is (re)opening. Make sure the 200 ms timer is running, nudge
    // the media thread, and paint one frame synchronously so the bar never
    // shows a stale value while the panel animates in. The flags are armed
    // before the wake so the media thread refreshes on its first wakeup.
    void WakeUp() {
        // The window is still cloaked while the flyout animates in. Restart the
        // hidden-tick countdown and force one paint, so the first frame shows
        // the live position instead of the 0:00 placeholder.
        hiddenTicks_ = 0;
        forceUpdate_ = true;
        StartTimer();
        media::g_watch.store(true, std::memory_order_relaxed);
        media::g_active.store(true, std::memory_order_relaxed);
        media::Wake();
        Update();
    }

    void Restore() {
        if (restored_) {
            return;
        }
        restored_ = true;

        StopTimer();
        pending_.clear();

        if (auto view = view_.get()) {
            try {
                view.LayoutUpdated(layoutToken_);
            } catch (...) {
            }
        }
        if (auto row = row_.get()) {
            try {
                if (auto slider = slider_.get()) {
                    slider.ValueChanged(valueToken_);
                    slider.RemoveHandler(X::UIElement::PointerPressedEvent(), pressedHandler_);
                    slider.RemoveHandler(X::UIElement::PointerReleasedEvent(), releasedHandler_);
                    slider.RemoveHandler(X::UIElement::PointerCaptureLostEvent(), releasedHandler_);
                    slider.RemoveHandler(X::UIElement::PointerCanceledEvent(), releasedHandler_);
                }
            } catch (...) {
            }
        }

        if (auto root = root_.get()) {
            try {
                if (auto row = row_.get()) {
                    uint32_t index = 0;
                    if (root.Children().IndexOf(row, index)) {
                        root.Children().RemoveAt(index);
                    }
                }
                for (auto it = shifted_.rbegin(); it != shifted_.rend(); ++it) {
                    if (auto element = it->first.get()) {
                        C::Grid::SetRow(element, it->second);
                    }
                }
                if (insertedRow_ >= 0 &&
                    static_cast<uint32_t>(insertedRow_) < root.RowDefinitions().Size()) {
                    root.RowDefinitions().RemoveAt(insertedRow_);
                }
            } catch (...) {
                Wh_Log(L"Seek bar restore failed: %08X", winrt::to_hresult());
            }
        }
        if (auto region = region_.get()) {
            try {
                // Give the card back its natural auto-sizing; a fixed height
                // written by the mod must not survive the restore.
                region.Height(std::numeric_limits<double>::quiet_NaN());
            } catch (...) {
            }
        }

        row_ = nullptr;
        slider_ = nullptr;
        view_ = nullptr;
        island_ = nullptr;
        region_ = nullptr;
        root_ = nullptr;
        buttons_ = nullptr;
        pressedHandler_ = nullptr;
        releasedHandler_ = nullptr;
        shifted_.clear();
        insertedRow_ = -1;
        baseHeight_ = NAN;
        subtitle_ = nullptr;
        timeBrush_ = nullptr;
        fallbackBrush_ = nullptr;
        elapsedTime_ = nullptr;
        totalTime_ = nullptr;
        showRow_ = false;
        dragPointer_ = nullptr;
        attempts_ = 0;
        needsReconcile_ = false;
        hostActive_ = false;
        hiddenTicks_ = 0;
        forceUpdate_ = false;
        forceHoursClock_ = false;
        hasLastColor_ = false;
        unsupported_ = false;
        unsupportedAt_ = 0;
        suppressUntil_ = 0;
        commitPending_ = false;
        lastSeekTarget_ = -1;
        lastSeekEnd_ = 0;
        missLogs_ = 0;
        lastMissLog_ = 0;
        media::g_active.store(false);
        media::g_watch.store(false);
    }

   private:
    bool Discover(X::FrameworkElement const& element) {
        if (g_stopping || !element) {
            return false;
        }
        X::FrameworkElement root = element;
        try {
            if (auto xamlRoot = element.XamlRoot()) {
                if (auto content = xamlRoot.Content().try_as<X::FrameworkElement>()) {
                    root = content;
                }
            }
        } catch (...) {
        }
        if (root == element) {
            // Bounded: a half-built island must not be able to spin here.
            for (int depth = 0; depth < 64; depth++) {
                auto parent = ParentOf(root);
                if (!parent) {
                    break;
                }
                root = parent;
            }
        }

        // Prefer the element we were handed: it is the view that is being shown,
        // while Find() could return an older instance the shell keeps loaded.
        X::FrameworkElement view = nullptr;
        if (ClassOf(element) == kViewClass) {
            view = element;
        } else if (ClassOf(root) == kViewClass) {
            view = root;
        } else {
            view = Find(root, [](X::FrameworkElement const& candidate) {
                return ClassOf(candidate) == kViewClass;
            });
        }
        if (!view) {
            return false;
        }

        if (auto current = view_.get(); current && current == view) {
            Reconcile();
            return true;
        }
        Restore();
        restored_ = false;
        Base(root, view);
        return true;
    }

    // The media card is a sibling of the Quick Settings view, not a child of it:
    // the panel lays out RootContent as [media card, ControlCenterRegion]. The
    // scan therefore starts at the XAML island root, while the view is kept for
    // its dispatcher and for layout notifications.
    void Base(X::FrameworkElement const& island, X::FrameworkElement const& view) {
        island_ = winrt::make_weak(island);
        view_ = winrt::make_weak(view);
        try {
            RememberThread(threadId_, view.Dispatcher());
        } catch (...) {
        }
        try {
            layoutToken_ = view.LayoutUpdated([this](auto&&, auto&&) {
                // Runs inside the shell's layout pass: never let anything throw
                // from here.
                try {
                    // Runs inside the shell's layout pass: only record that the
                    // tree changed. Touching the tree from here (adding rows,
                    // changing heights) can abort the flyout's first layout, so
                    // the timer does all of that instead.
                    needsReconcile_ = true;
                    // Insurance: if the thread timer could not be created, keep
                    // the row alive from layout notifications instead. Throttled
                    // so a layout pass storm cannot turn into a callback storm.
                    if (!timerId_ && GetTickCount64() - lastFallback_ >= 200) {
                        lastFallback_ = GetTickCount64();
                        Update();
                    }
                } catch (...) {
                }
            });
        } catch (...) {
        }
        StartTimer();
        Reconcile();
        Wh_Log(L"Control Center view attached");
    }

    void ProcessPending() {
        if (g_stopping) {
            pending_.clear();
            return;
        }
        if (pending_.empty()) {
            return;
        }
        auto pending = std::move(pending_);
        pending_.clear();
        bool found = false;
        bool waiting = false;
        for (auto const& weak : pending) {
            auto element = weak.get();
            if (!element) {
                continue;
            }
            // Connect hands over children that are still being constructed. Wait
            // until XAML has loaded them before touching the tree: patching a
            // tree that is still loading is exactly what can make the flyout fail
            // to appear. The focus hook normally gets there first; this path is
            // the fallback for builds where it does not fire.
            bool loaded = false;
            try {
                loaded = element.IsLoaded();
            } catch (...) {
            }
            if (!loaded) {
                waiting = true;
                continue;
            }
            if (Discover(element)) {
                found = true;
                break;
            }
        }
        if (found) {
            attempts_ = 0;
            return;
        }
        if (waiting) {
            attempts_ = 0;
            pending_ = std::move(pending);
            return;
        }
        // Loaded but useless (not part of a Control Center view): retry a few
        // times in case the view appears around it, then forget it.
        if (++attempts_ < 8) {
            pending_ = std::move(pending);
        } else {
            attempts_ = 0;
        }
    }

    void Reconcile() {
        // Never let a XAML failure escape into the shell's layout or timer
        // callbacks: an uncaught exception there would take ShellHost down.
        try {
            ReconcileCore();
        } catch (...) {
            Wh_Log(L"Seek row update failed: %08X", winrt::to_hresult());
        }
    }

    void ReconcileCore() {
        lastReconcile_ = GetTickCount64();
        needsReconcile_ = false;
        if (g_stopping || restored_) {
            return;
        }
        auto view = view_.get();
        if (!view) {
            return;
        }

        // The common case is "our row is still in place": one IndexOf call, no
        // tree walk. A full walk only runs when the card was rebuilt, or every
        // two seconds to notice a card that was replaced in place.
        if (GetTickCount64() - lastFullScan_ <= 2000) {
            if (auto row = row_.get()) {
                if (auto root = root_.get()) {
                    uint32_t index = 0;
                    if (root.Children().IndexOf(row, index)) {
                        if (insertedRow_ >= 0 && C::Grid::GetRow(row) != insertedRow_) {
                            C::Grid::SetRow(row, insertedRow_);
                        }
                        ApplyHeight();
                        return;
                    }
                }
            }
        }

        if (unsupported_) {
            // A transient layout while the shell rebuilds the card must not
            // disable the row until the process ends: retry after a few seconds.
            if (GetTickCount64() - unsupportedAt_ < 5000) {
                return;
            }
            unsupported_ = false;
        }

        lastFullScan_ = GetTickCount64();
        // The card lives next to the view, not inside it, so the island root is
        // the only correct place to start. If the island root was replaced (the
        // shell does this on display or session changes) while the view object
        // survived, re-derive it from the view instead of giving up.
        X::FrameworkElement scan = island_.get();
        if (!scan) {
            try {
                if (auto xamlRoot = view.XamlRoot()) {
                    scan = xamlRoot.Content().try_as<X::FrameworkElement>();
                }
            } catch (...) {
            }
            if (!scan) {
                scan = view;
                auto parent = ParentOf(scan);
                for (int depth = 0; parent && depth < 64; depth++) {
                    scan = parent;
                    parent = ParentOf(scan);
                }
            }
            if (scan) {
                island_ = winrt::make_weak(scan);
            }
        }
        if (!scan) {
            scan = view;
        }
        X::FrameworkElement region = nullptr;
        C::Grid root = nullptr;
        X::FrameworkElement buttons = nullptr;
        FindCard(scan, region, root, buttons);
        if (!region || !root || !buttons) {
            // The card is still being built; the next layout pass retries.
            ReportMissing(region, root, buttons);
            return;
        }

        if (!std::isfinite(baseHeight_) || baseHeight_ <= 0) {
            double height = region.Height();
            if (!std::isfinite(height) || height <= 0) {
                height = region.ActualHeight();
            }
            if (!std::isfinite(height) || height <= 0) {
                return;
            }
            baseHeight_ = height;
            Wh_Log(L"Media card base height: %.1f", baseHeight_);
        }

        region_ = winrt::make_weak(region);
        root_ = winrt::make_weak(root);
        buttons_ = winrt::make_weak(buttons);

        if (auto row = row_.get()) {
            uint32_t index = 0;
            if (root.Children().IndexOf(row, index)) {
                ApplyHeight();
                return;
            }
            // The shell rebuilt the card: the old row is gone with the old tree.
            row_ = nullptr;
            shifted_.clear();
            insertedRow_ = -1;
        }
        Insert(region, root, buttons);
    }

    static void FindCard(X::DependencyObject const& node, X::FrameworkElement& region,
                         C::Grid& root, X::FrameworkElement& buttons) {
        if (!node || (region && root && buttons)) {
            return;
        }
        if (auto element = node.try_as<X::FrameworkElement>()) {
            auto name = element.Name();
            if (!region && name == kRegionName) {
                region = element;
            } else if (!root && name == kRootName) {
                root = element.try_as<C::Grid>();
            } else if (!buttons && name == kButtonsName) {
                buttons = element;
            }
        }
        int count = M::VisualTreeHelper::GetChildrenCount(node);
        for (int i = 0; i < count && !(region && root && buttons); i++) {
            FindCard(M::VisualTreeHelper::GetChild(node, i), region, root, buttons);
        }
    }

    // A silent "not found" is impossible to diagnose from a log, so say which
    // piece is missing: at most five times, five seconds apart.
    void ReportMissing(X::FrameworkElement const& region, C::Grid const& root,
                       X::FrameworkElement const& buttons) {
        if (missLogs_ >= 5) {
            return;
        }
        ULONGLONG now = GetTickCount64();
        if (now - lastMissLog_ < 5000) {
            return;
        }
        lastMissLog_ = now;
        missLogs_++;
        Wh_Log(L"Media card not ready: region=%d root=%d buttons=%d", region ? 1 : 0,
               root ? 1 : 0, buttons ? 1 : 0);
    }

    static void AddColumn(C::Grid const& grid, X::GridLength const& width) {
        C::ColumnDefinition column;
        column.Width(width);
        grid.ColumnDefinitions().Append(column);
    }

    C::Grid BuildRow() {
        C::Grid row;
        row.Name(L"WindhawkSeekRow");
        row.Height(kBarHeight);
        row.Margin({0, kRowGap, 0, 0});
        row.HorizontalAlignment(X::HorizontalAlignment::Stretch);
        row.VerticalAlignment(X::VerticalAlignment::Top);

        AddColumn(row, X::GridLengthHelper::Auto());
        AddColumn(row, X::GridLengthHelper::FromValueAndType(8, X::GridUnitType::Pixel));
        AddColumn(row, X::GridLengthHelper::FromValueAndType(1, X::GridUnitType::Star));
        AddColumn(row, X::GridLengthHelper::FromValueAndType(8, X::GridUnitType::Pixel));
        AddColumn(row, X::GridLengthHelper::Auto());

        // The brush is applied by SyncTimeBrush(): the card's own secondary text
        // brush, which follows light/dark/high contrast correctly. A
        // TextFillColorSecondaryBrush resource lookup resolves against the
        // shell's application theme, not the card's, which produced white labels
        // in light mode. The opacity below is only a placeholder for the first
        // frame and is dropped as soon as the real brush is set.
        auto makeTime = [](X::TextAlignment alignment) {
            C::TextBlock text;
            text.FontSize(12);
            text.MinWidth(34);
            text.VerticalAlignment(X::VerticalAlignment::Center);
            text.TextAlignment(alignment);
            text.Opacity(0.6);
            return text;
        };

        C::TextBlock elapsed = makeTime(X::TextAlignment::Right);
        C::TextBlock total = makeTime(X::TextAlignment::Left);
        elapsed.Name(L"WindhawkSeekElapsed");
        total.Name(L"WindhawkSeekTotal");
        elapsed.Text(FormatTime(0));
        total.Text(FormatTime(0));
        elapsedTime_ = elapsed;
        totalTime_ = total;

        C::Slider slider;
        slider.Name(L"WindhawkSeekSlider");
        slider.Minimum(0);
        slider.Maximum(1);
        slider.Value(0);
        // The value is in seconds, so a step of one second gives a thumb that
        // moves in whole seconds and a seek target that is always exact.
        slider.StepFrequency(1);
        slider.SmallChange(5);
        slider.LargeChange(15);
        slider.IsThumbToolTipEnabled(false);
        slider.MinHeight(0);
        slider.Height(kBarHeight);
        slider.HorizontalAlignment(X::HorizontalAlignment::Stretch);
        slider.VerticalAlignment(X::VerticalAlignment::Center);
        X::Automation::AutomationProperties::SetName(slider, L"Playback position");
        slider_ = winrt::make_weak<C::Slider>(slider);

        valueToken_ = slider.ValueChanged([this](auto&&, P::RangeBaseValueChangedEventArgs const& args) {
            try {
                if (sync_) {
                    return;
                }
                pendingValue_ = args.NewValue();
                if (dragPointer_) {
                    // The elapsed label follows the thumb while dragging, so the
                    // user sees the position being selected instead of the
                    // position that is still playing.
                    if (elapsedTime_) {
                        elapsedTime_.Text(FormatTime(
                            static_cast<int64_t>(std::llround(args.NewValue() * 10000000.0)),
                            forceHoursClock_));
                    }
                } else {
                    // Deliberately not committed here. Pressing the track
                    // changes the value before the press handler runs, and a
                    // second commit on release would seek twice: the player
                    // then replays a fraction of the audio. The change is sent
                    // from the next tick unless a drag starts, and the release
                    // handler sends it at once.
                    commitPending_ = true;
                }
            } catch (...) {
            }
        });

        auto press = winrt::box_value(X::Input::PointerEventHandler([this](auto&&, auto&&) {
            try {
                dragPointer_ = reinterpret_cast<void*>(1);
                commitPending_ = false;
            } catch (...) {
            }
        }));
        auto release = winrt::box_value(X::Input::PointerEventHandler([this](auto&&, auto&&) {
            try {
                bool commit = dragPointer_ != nullptr || commitPending_;
                dragPointer_ = nullptr;
                commitPending_ = false;
                if (commit) {
                    Commit();
                }
            } catch (...) {
            }
        }));
        pressedHandler_ = press;
        releasedHandler_ = release;
        slider.AddHandler(X::UIElement::PointerPressedEvent(), press, true);
        for (auto const& event : {X::UIElement::PointerReleasedEvent(),
                                  X::UIElement::PointerCaptureLostEvent(),
                                  X::UIElement::PointerCanceledEvent()}) {
            slider.AddHandler(event, release, true);
        }

        C::Grid::SetColumn(elapsed, 0);
        C::Grid::SetColumn(slider, 2);
        C::Grid::SetColumn(total, 4);
        row.Children().Append(elapsed);
        row.Children().Append(slider);
        row.Children().Append(total);
        return row;
    }

    void Insert(X::FrameworkElement const& region, C::Grid const& root,
                X::FrameworkElement const& buttons) {
        int buttonsRow = C::Grid::GetRow(buttons);
        uint32_t definitions = root.RowDefinitions().Size();
        if (definitions == 0) {
            Wh_Log(L"The media card has no row definitions");
            return;
        }
        // The button row is expected to be row 2 of [Auto, Auto, *]. Refuse to
        // touch the card at all if it looks different, rather than reorder a
        // layout that is not understood.
        if (buttonsRow <= 0 || static_cast<uint32_t>(buttonsRow) >= definitions) {
            Wh_Log(L"Unexpected media card layout (button row %d of %u), leaving it alone",
                   buttonsRow, definitions);
            unsupported_ = true;
            unsupportedAt_ = GetTickCount64();
            return;
        }

        // Built before anything is changed: if the control cannot be created,
        // the card is left exactly as it was. The card's artist line (or the
        // title, if the artist line is missing) supplies the text brush.
        subtitle_ = nullptr;
        for (PCWSTR wanted : {L"SubtitleText", L"TitleText"}) {
            if (auto element = Find(region, [wanted](X::FrameworkElement const& candidate) {
                    return candidate.Name() == wanted;
                })) {
                if (auto text = element.try_as<C::TextBlock>()) {
                    subtitle_ = text;
                    break;
                }
            }
        }
        auto row = BuildRow();

        shifted_.clear();
        for (auto const& child : root.Children()) {
            auto element = child.try_as<X::FrameworkElement>();
            if (!element) {
                continue;
            }
            int childRow = C::Grid::GetRow(element);
            if (childRow >= buttonsRow) {
                shifted_.push_back({winrt::make_weak(element), childRow});
                C::Grid::SetRow(element, childRow + 1);
            }
        }

        C::RowDefinition definition;
        definition.Height(X::GridLengthHelper::Auto());
        root.RowDefinitions().InsertAt(buttonsRow, definition);
        insertedRow_ = buttonsRow;

        C::Grid::SetRow(row, buttonsRow);
        root.Children().Append(row);
        row_ = winrt::make_weak<C::Grid>(row);
        SyncTimeBrush();

        // The row starts collapsed and the card stays at its natural height:
        // UpdateCore shows the row and grows the card only after the session is
        // verified seekable, so a live stream never flashes an empty 36 DIP
        // band that collapses again a moment later.
        showRow_ = false;
        row.Visibility(X::Visibility::Collapsed);
        Wh_Log(L"Seek row inserted in row %d of %u", buttonsRow, root.RowDefinitions().Size());
    }

    void ApplyHeight() {
        auto region = region_.get();
        if (!region) {
            return;
        }
        if (!showRow_) {
            // The row is collapsed: the card must size itself. Drop any fixed
            // height written earlier and re-learn the natural height, so a
            // taller card (a title wrapped to two lines, a different app) is
            // not stuck at the first measurement.
            double current = region.Height();
            if (std::isfinite(current)) {
                // The write relayouts; ActualHeight settles on the next pass.
                region.Height(std::numeric_limits<double>::quiet_NaN());
                return;
            }
            double actual = region.ActualHeight();
            if (std::isfinite(actual) && actual > 0) {
                baseHeight_ = actual;
            }
            return;
        }
        if (!std::isfinite(baseHeight_) || baseHeight_ <= 0) {
            return;
        }
        double wanted = baseHeight_ + kCardGrowth;
        double current = region.Height();
        if (std::isfinite(current) && std::fabs(current - wanted) < 0.01) {
            return;
        }
        region.Height(wanted);
    }

    void SetRowVisible(bool visible) {
        if (visible == showRow_) {
            return;
        }
        showRow_ = visible;
        if (auto row = row_.get()) {
            row.Visibility(visible ? X::Visibility::Visible : X::Visibility::Collapsed);
        }
        ApplyHeight();
    }

    // Sends the pending thumb position to the player. The range is taken from
    // the current snapshot (not from the value used while dragging), so a track
    // change during a drag cannot produce an out-of-range seek.
    void Commit() {
        auto snapshot = media::Get();
        int64_t duration = snapshot.end - snapshot.start;
        if (!snapshot.valid || duration <= 0) {
            return;
        }
        double maximum = static_cast<double>(duration) / 10000000.0;
        double value = std::clamp(pendingValue_, 0.0, maximum);
        int64_t target =
            snapshot.start + static_cast<int64_t>(std::llround(value * 10000000.0));
        target = std::clamp(target, snapshot.start, snapshot.end);

        ULONGLONG now = GetTickCount64();
        // A press and its release can resolve to the same position. Sending the
        // same seek twice makes the player replay a split second of audio.
        if (target == lastSeekTarget_ && snapshot.end == lastSeekEnd_ &&
            now - lastSeekAt_ < 500) {
            return;
        }
        lastSeekTarget_ = target;
        lastSeekEnd_ = snapshot.end;
        lastSeekAt_ = now;
        media::RequestSeek(target);
        suppressUntil_ = now + 1000;
        if (elapsedTime_) {
            elapsedTime_.Text(FormatTime(target - snapshot.start, forceHoursClock_));
        }
    }

    void Update() {
        if (g_stopping || restored_) {
            return;
        }
        // This runs from the thread timer and from LayoutUpdated, i.e. inside
        // the shell's own message and layout dispatch. Nothing may escape.
        try {
            UpdateCore();
        } catch (...) {
            static int failures = 0;
            if (failures++ < 5) {
                Wh_Log(L"Seek bar update failed: %08X", winrt::to_hresult());
            }
        }
    }

    void UpdateCore() {
        // Zero idle drain: while the panel is closed (or DWM-cloaked) none of
        // the XAML tree is touched and, after two consecutive hidden ticks,
        // the timer is stopped entirely and the media thread is parked. The
        // focus hooks and Offer() start everything again on the next open.
        //
        // The wake-up frame is exempt: HostVisible() is still false while the
        // flyout animates in, and returning here would leave the bar frozen at
        // the 0:00 placeholder even though the timer was just started.
        bool force = forceUpdate_;
        forceUpdate_ = false;
        if (!HostVisible() && !force) {
            // A drag in progress is meaningless once the panel is gone; its
            // release event may never arrive.
            dragPointer_ = nullptr;
            commitPending_ = false;
            hiddenTicks_++;
            if (hiddenTicks_ >= 2) {
                media::g_active.store(false, std::memory_order_relaxed);
                hostActive_ = false;
                StopTimer();
            }
            return;
        }
        hiddenTicks_ = 0;

        if (needsReconcile_) {
            needsReconcile_ = false;
            Reconcile();
        }

        auto row = row_.get();
        if (!row) {
            // The row is gone (card rebuilt): try to put it back.
            Reconcile();
            return;
        }

        // A pending value change (keyboard or a click on the track) is sent once
        // no pointer gesture is in progress. While a drag is active the release
        // handler commits instead, so a press-then-drag never seeks twice.
        if (commitPending_ && !dragPointer_) {
            commitPending_ = false;
            Commit();
        }

        bool cardShown = false;
        double cardHeight = 0;
        if (auto region = region_.get()) {
            cardShown = region.Visibility() == X::Visibility::Visible;
            cardHeight = region.ActualHeight();
        }
        bool visible = cardShown && cardHeight > 0;
        if (visible && !hostActive_) {
            // First paint after the panel opens: nudge the media thread so the
            // bar is filled immediately instead of up to one poll later. Any
            // drag that was in flight when the panel closed is meaningless now.
            media::Wake();
            dragPointer_ = nullptr;
            commitPending_ = false;
            // Bounded trace: if the panel ever opens without the bar, this shows
            // whether the window was seen and whether the row still existed.
            if (openLogs_ < 20) {
                openLogs_++;
                Wh_Log(L"Panel opened: row=1, height=%.0f", cardHeight);
            }
        } else if (!visible) {
            // The card is not on screen: drop any stale drag state.
            dragPointer_ = nullptr;
            commitPending_ = false;
        }
        hostActive_ = visible;
        // The window test only chooses the poll rate. The UI itself is always
        // brought up to date while the panel is on screen.
        media::g_active.store(visible, std::memory_order_relaxed);
        media::g_watch.store(true, std::memory_order_relaxed);

        auto snapshot = media::Get();
        bool seekable = snapshot.valid && snapshot.canSeek;
        SetRowVisible(seekable);

        // Keeps the labels correct across a live light/dark theme switch even
        // while the row is hidden.
        SyncTimeBrush();
        if (!seekable) {
            return;
        }

        double maximum = static_cast<double>(snapshot.end - snapshot.start) / 10000000.0;
        if (!(maximum > 0)) {
            SetRowVisible(false);
            return;
        }

        forceHoursClock_ = (snapshot.end - snapshot.start) >= 36000000000LL;
        winrt::hstring elapsedText =
            FormatTime(snapshot.position - snapshot.start, forceHoursClock_);
        winrt::hstring totalText = FormatTime(snapshot.end - snapshot.start, forceHoursClock_);
        bool follow = !dragPointer_ && GetTickCount64() >= suppressUntil_;
        sync_ = true;
        if (auto slider = slider_.get()) {
            if (std::abs(slider.Maximum() - maximum) > 0.01) {
                slider.Maximum(maximum);
            }
            if (follow) {
                double value = static_cast<double>(snapshot.position - snapshot.start) / 10000000.0;
                value = std::clamp(value, 0.0, maximum);
                if (std::abs(slider.Value() - value) > 0.05) {
                    slider.Value(value);
                }
                pendingValue_ = value;
            }
        }
        // The labels do not depend on the slider: they must keep working even if
        // that one control could not be reached.
        if (follow && elapsedTime_) {
            elapsedTime_.Text(elapsedText);
        }
        if (totalTime_) {
            totalTime_.Text(totalText);
        }
        sync_ = false;
    }

    // The card's own secondary text brush is the only reliable colour source:
    // a TextFillColorSecondaryBrush lookup resolves against the shell's
    // application theme, which is not the theme the card is rendered in, and
    // produced white labels in light mode. Mirroring the native subtitle keeps
    // the exact colour in light, dark and high contrast, and re-checking it each
    // tick picks up a live theme switch.
    void SyncTimeBrush() {
        if (!elapsedTime_ && !totalTime_) {
            return;
        }
        M::Brush brush{nullptr};
        if (subtitle_) {
            try {
                brush = subtitle_.Foreground();
            } catch (...) {
            }
        }
        if (!brush) {
            try {
                brush = FallbackBrush();
            } catch (...) {
            }
        }
        if (!brush) {
            return;
        }
        // Compare the A, R, G, B values, not the brush instances: the shell
        // can swap in a new brush object on every theme switch, and can also
        // mutate a shared brush in place. Only a real colour change is worth
        // a relayout of the labels.
        if (auto solid = brush.try_as<M::SolidColorBrush>()) {
            auto color = solid.Color();
            if (hasLastColor_ && color.A == lastColor_.A && color.R == lastColor_.R &&
                color.G == lastColor_.G && color.B == lastColor_.B) {
                return;
            }
            lastColor_ = color;
            hasLastColor_ = true;
        } else if (timeBrush_) {
            if (winrt::get_abi(timeBrush_) == winrt::get_abi(brush)) {
                return;
            }
        }
        timeBrush_ = brush;
        if (elapsedTime_) {
            elapsedTime_.Foreground(brush);
            elapsedTime_.Opacity(1);
        }
        if (totalTime_) {
            totalTime_.Foreground(brush);
            totalTime_.Opacity(1);
        }
    }

    M::Brush FallbackBrush() {
        // 0 = light, 1 = dark, -1 = not known yet.
        int theme = -1;
        if (auto view = view_.get()) {
            try {
                auto actual = view.ActualTheme();
                if (actual == X::ElementTheme::Dark) {
                    theme = 1;
                } else if (actual == X::ElementTheme::Light) {
                    theme = 0;
                }
            } catch (...) {
            }
        }
        if (theme < 0) {
            try {
                theme = X::Application::Current().RequestedTheme() == X::ApplicationTheme::Dark
                            ? 1
                            : 0;
            } catch (...) {
                theme = 0;
            }
        }
        if (fallbackBrush_ && fallbackTheme_ == theme) {
            return fallbackBrush_;
        }
        // TextFillColorSecondary: 60 % black in light mode, 60 % white in dark.
        M::SolidColorBrush brush;
        brush.Color(theme == 1 ? winrt::Windows::UI::Color{0x99, 0xFF, 0xFF, 0xFF}
                               : winrt::Windows::UI::Color{0x99, 0x00, 0x00, 0x00});
        fallbackBrush_ = brush;
        fallbackTheme_ = theme;
        return brush;
    }

    // `IsWindowVisible` is not enough for the shell's flyouts: the window stays
    // "visible" and the panel is hidden by DWM cloaking, so the cloaked state is
    // what actually says whether it is on screen.
    static bool WindowShown(HWND window) {
        if (!window || !IsWindowVisible(window)) {
            return false;
        }
        using DwmGetWindowAttributeFn = HRESULT(WINAPI*)(HWND, DWORD, void*, DWORD);
        static auto get = [] {
            auto dll = LoadLibraryExW(L"dwmapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
            return dll ? reinterpret_cast<DwmGetWindowAttributeFn>(
                             GetProcAddress(dll, "DwmGetWindowAttribute"))
                       : nullptr;
        }();
        DWORD cloaked = 0;
        return !(get && SUCCEEDED(get(window, 14 /*DWMWA_CLOAKED*/, &cloaked,
                                    sizeof(cloaked))) &&
                 cloaked);
    }

    struct HostWindowState {
        bool found = false;
        bool shown = false;
    };

    static HostWindowState QueryHostWindows() {
        struct Context {
            HostWindowState state;
        } context;
        EnumThreadWindows(
            GetCurrentThreadId(),
            [](HWND window, LPARAM parameter) -> BOOL {
                auto context = reinterpret_cast<Context*>(parameter);
                WCHAR name[64]{};
                if (!GetClassNameW(window, name, ARRAYSIZE(name))) {
                    return TRUE;
                }
                if (_wcsicmp(name, L"ControlCenterWindow") &&
                    _wcsicmp(name, L"Windows.UI.Core.CoreWindow")) {
                    return TRUE;
                }
                context->state.found = true;
                if (WindowShown(window)) {
                    context->state.shown = true;
                }
                return TRUE;
            },
            reinterpret_cast<LPARAM>(&context));
        return context.state;
    }

    // True when the panel is on screen. If no panel window can be identified at
    // all (an unknown shell build), fail open: a bar that keeps polling costs a
    // little CPU, a bar that never updates is simply broken.
    bool HostVisible() {
        HostWindowState state = QueryHostWindows();
        if (!state.found) {
            if (!noHostWindow_) {
                noHostWindow_ = true;
                Wh_Log(L"No panel window recognised on this build, polling continuously");
            }
            return true;
        }
        return state.shown;
    }

    DWORD threadId_ = 0;
    bool restored_ = false;
    bool showRow_ = false;
    bool sync_ = false;
    bool needsReconcile_ = false;
    bool hostActive_ = false;
    bool unsupported_ = false;
    bool noHostWindow_ = false;
    bool commitPending_ = false;
    int openLogs_ = 0;
    int hiddenTicks_ = 0;
    // Set when the panel opens. The first update is then allowed to paint while
    // the window is still DWM-cloaked, which is what makes the bar show the
    // current position on the opening frame.
    bool forceUpdate_ = false;
    bool forceHoursClock_ = false;
    double baseHeight_ = NAN;
    double pendingValue_ = 0;
    int insertedRow_ = -1;
    ULONGLONG lastReconcile_ = 0;
    ULONGLONG lastFullScan_ = 0;
    ULONGLONG lastSeekAt_ = 0;
    ULONGLONG suppressUntil_ = 0;
    ULONGLONG lastMissLog_ = 0;
    ULONGLONG lastFallback_ = 0;
    ULONGLONG unsupportedAt_ = 0;
    int64_t lastSeekTarget_ = -1;
    int64_t lastSeekEnd_ = 0;
    int attempts_ = 0;
    int missLogs_ = 0;
    void* dragPointer_ = nullptr;

    UINT_PTR timerId_ = 0;
    int slowTicks_ = 0;

    winrt::event_token layoutToken_{};
    winrt::event_token valueToken_{};
    IInspectable pressedHandler_{nullptr};
    IInspectable releasedHandler_{nullptr};

    winrt::weak_ref<X::FrameworkElement> view_{nullptr};
    winrt::weak_ref<X::FrameworkElement> island_{nullptr};
    winrt::weak_ref<X::FrameworkElement> region_{nullptr};
    winrt::weak_ref<C::Grid> root_{nullptr};
    winrt::weak_ref<X::FrameworkElement> buttons_{nullptr};
    winrt::weak_ref<C::Grid> row_{nullptr};
    winrt::weak_ref<C::Slider> slider_{nullptr};
    // Strong references on purpose. These three are text blocks we either own or
    // only read, and on this shell a weak reference to a TextBlock created in
    // code does not resolve: the labels silently kept their placeholder text
    // while the bar itself worked. Holding the element keeps the label usable;
    // Restore() drops it again with the row.
    C::TextBlock elapsedTime_{nullptr};
    C::TextBlock totalTime_{nullptr};
    // The card's own secondary text, used as the colour source for the labels.
    C::TextBlock subtitle_{nullptr};
    M::Brush timeBrush_{nullptr};
    M::Brush fallbackBrush_{nullptr};
    int fallbackTheme_ = -1;
    // Last ARGB applied to the labels, so a live theme switch is detected even
    // when the shell mutates a shared brush in place.
    winrt::Windows::UI::Color lastColor_{0x00, 0x00, 0x00, 0x00};
    bool hasLastColor_ = false;

    std::vector<winrt::weak_ref<X::FrameworkElement>> pending_;
    std::vector<std::pair<winrt::weak_ref<X::FrameworkElement>, int>> shifted_;
};

// One patcher per XAML UI thread. Deliberately never destroyed by the CRT: the
// shell can tear its threads down in any order at process exit.
[[clang::no_destroy]] thread_local std::unique_ptr<Patcher> g_patcher;

void StopOnCurrentThread() {
    if (g_patcher) {
        g_patcher->Restore();
        g_patcher.reset();
    }
}

}  // namespace

// ---------------------------------------------------------------------------
// Discovery
//
// ControlCenter.dll builds the Quick Settings view on demand. Hooking two of
// its WinRT interface entry points hands us the live XAML element on the UI
// thread without polling and without XAML diagnostics.
// ---------------------------------------------------------------------------

namespace {

using FocusFn = int(WINAPI*)(void*, void*);
using ConnectFn = int(WINAPI*)(void*, int, void*);

FocusFn g_originalViewFocus = nullptr;
FocusFn g_originalPageFocus = nullptr;
ConnectFn g_originalViewConnect = nullptr;
ConnectFn g_originalPageConnect = nullptr;

std::atomic<int> g_discovery{0};  // 0 waiting, 1 installing, 2 ready, 3 failed
std::mutex g_installMutex;
std::mutex g_creationMutex;
thread_local bool g_insideHook = false;

// ControlCenter.dll is hooked from a thread of our own that watches for the
// module with GetModuleHandleW. ShellHost aborted a few milliseconds into every
// cold start while this mod was loaded, inside ControlCenter's own start-up and
// with none of our frames on the stack, so the whole start-up window is now
// kept free of us: no LoadLibraryExW hook (our code was in the path of every
// library the shell loaded), no COM, and a full second of quiet after the
// module first appears before its code is touched.
HANDLE g_installThread = nullptr;

// Set once the media thread has been asked for. Started from the first real
// discovery instead of Wh_ModInit for the same reason as above.
std::once_flag g_mediaStartFlag;

void Discovered(void* object, bool deferred) noexcept {
    if (!object || g_stopping || g_insideHook) {
        return;
    }
    g_insideHook = true;
    // Bounded trace: tells a later log apart "the shell never reached our hooks"
    // from "the hook ran and something below it failed".
    static std::atomic<int> hookLogs{0};
    try {
        X::FrameworkElement element{nullptr};
        HRESULT hr = static_cast<::IUnknown*>(object)->QueryInterface(
            winrt::guid_of<X::FrameworkElement>(), winrt::put_abi(element));
        if (hookLogs.fetch_add(1) < 6) {
            Wh_Log(L"Hook ran: deferred=%d element=%d qr=%08X", deferred ? 1 : 0,
                   element ? 1 : 0, static_cast<unsigned>(hr));
        }
        if (SUCCEEDED(hr) && element) {
            // The shell is up and its panel exists at this point, so COM and
            // GSMTC can be brought up without sharing the start-up window that
            // ControlCenter's own initialisation used to die in.
            std::call_once(g_mediaStartFlag, [] { media::Start(); });
            {
                std::lock_guard<std::mutex> lock(g_creationMutex);
                if (!g_stopping && !g_patcher) {
                    g_patcher = std::make_unique<Patcher>();
                }
            }
            if (g_patcher) {
                g_patcher->Offer(element, deferred);
            }
        }
    } catch (...) {
        static std::atomic<int> logs{0};
        if (logs.fetch_add(1) < 5) {
            Wh_Log(L"Discovery failed: %08X", winrt::to_hresult());
        }
    }
    g_insideHook = false;
}

// Also bounded: only interesting when the panel misbehaves, and a per-open line
// would otherwise flood the log.
void NoteSlowDiscovery(ULONGLONG started) {
    ULONGLONG took = GetTickCount64() - started;
    if (took > 50) {
        static std::atomic<int> logs{0};
        if (logs.fetch_add(1) < 10) {
            Wh_Log(L"Slow panel handling: %llu ms", took);
        }
    }
}

int WINAPI HookViewFocus(void* self, void* args) noexcept {
    int result = g_originalViewFocus ? g_originalViewFocus(self, args) : 0;
    try {
        ULONGLONG started = GetTickCount64();
        Discovered(self, false);
        NoteSlowDiscovery(started);
    } catch (...) {
    }
    return result;
}

int WINAPI HookPageFocus(void* self, void* args) noexcept {
    int result = g_originalPageFocus ? g_originalPageFocus(self, args) : 0;
    try {
        ULONGLONG started = GetTickCount64();
        Discovered(self, false);
        NoteSlowDiscovery(started);
    } catch (...) {
    }
    return result;
}

int WINAPI HookViewConnect(void* self, int id, void* target) noexcept {
    int result = g_originalViewConnect ? g_originalViewConnect(self, id, target) : 0;
    try {
        Discovered(target, true);
    } catch (...) {
    }
    return result;
}

int WINAPI HookPageConnect(void* self, int id, void* target) noexcept {
    int result = g_originalPageConnect ? g_originalPageConnect(self, id, target) : 0;
    try {
        Discovered(target, true);
    } catch (...) {
    }
    return result;
}

bool HookControlCenter(HMODULE module, bool applyNow) {
    std::lock_guard<std::mutex> lock(g_installMutex);
    if (g_stopping) {
        return false;
    }
    int expected = 0;
    if (!g_discovery.compare_exchange_strong(expected, 1)) {
        return expected != 3;
    }

    WindhawkUtils::SYMBOL_HOOK controlCenterDllHooks[] = {
        // OnGotFocus is the reliable trigger: it fires every time the panel is
        // shown. Connect is a bonus that fires earlier while the tree is built,
        // so it is optional, exactly like in the published mod this follows.
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::ControlCenter::implementation::ControlCenterView,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnGotFocus(void *))"},
         &g_originalViewFocus,
         HookViewFocus},
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::ControlCenter::implementation::ControlCenterView,struct winrt::Windows::UI::Xaml::Markup::IComponentConnector>::Connect(int,void *))"},
         &g_originalViewConnect,
         HookViewConnect,
         true},
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::ControlCenter::implementation::FullScreenPage,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnGotFocus(void *))"},
         &g_originalPageFocus,
         HookPageFocus,
         true},
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::ControlCenter::implementation::FullScreenPage,struct winrt::Windows::UI::Xaml::Markup::IComponentConnector>::Connect(int,void *))"},
         &g_originalPageConnect,
         HookPageConnect,
         true},
    };

    bool ok = WindhawkUtils::HookSymbols(module, controlCenterDllHooks, ARRAYSIZE(controlCenterDllHooks));
    if (ok && applyNow) {
        ok = Wh_ApplyHookOperations();
    }
    g_discovery = ok ? 2 : 3;
    Wh_Log(L"ControlCenter discovery: %s", ok ? L"ready" : L"failed");
    return ok;
}

// Watches for ControlCenter.dll and installs the hooks once its own start-up is
// over. GetModuleHandleW is the gate, not just a lookup: it needs the loader
// lock, which the loading thread holds for the whole of a load, so a module
// handle here always means a module that has finished loading. The extra second
// then covers whatever ControlCenter does immediately after loading itself,
// which is where the shell used to abort.
DWORD WINAPI InstallProc(void*) {
    while (!g_stopping && g_discovery == 0) {
        if (auto module = GetModuleHandleW(L"ControlCenter.dll")) {
            Sleep(1000);
            if (g_stopping) {
                break;
            }
            HookControlCenter(module, true);
            return 0;
        }
        Sleep(250);
    }
    return 0;
}

}  // namespace

// ---------------------------------------------------------------------------
// Entry points
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing (process %u)", static_cast<unsigned>(GetCurrentProcessId()));
    g_stopping.store(false);

    // Deliberately nothing else here. The media thread starts on the first real
    // discovery, and ControlCenter is found by InstallProc, so the shell's own
    // start-up runs without this mod in its way.
    g_installThread = CreateThread(nullptr, 0, InstallProc, nullptr, 0, nullptr);
    if (!g_installThread) {
        Wh_Log(L"Installer thread could not be created: %u", GetLastError());
    } else {
        Wh_Log(L"Waiting for ControlCenter.dll");
    }
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitializing");
    g_stopping.store(true);

    // The installer thread must be gone before the module unloads; it checks
    // g_stopping every 250 ms, but may already be inside the one-second delay.
    if (g_installThread) {
        WaitForSingleObject(g_installThread, 5000);
        CloseHandle(g_installThread);
        g_installThread = nullptr;
    }

    // Drain a symbol install that was already running when the thread was
    // joined, so the module cannot be unloaded underneath it.
    {
        std::lock_guard<std::mutex> lock(g_installMutex);
    }
    {
        std::lock_guard<std::mutex> lock(g_creationMutex);
    }

    media::Stop();

    std::map<DWORD, winrt::weak_ref<winrt::Windows::UI::Core::CoreDispatcher>> threads;
    {
        std::lock_guard<std::mutex> lock(g_threadsMutex);
        threads = g_threads;
        g_threads.clear();
    }

    for (auto const& [id, weak] : threads) {
        if (id == GetCurrentThreadId()) {
            StopOnCurrentThread();
            continue;
        }
        winrt::Windows::UI::Core::CoreDispatcher dispatcher{nullptr};
        try {
            dispatcher = weak.get();
        } catch (...) {
        }
        if (!dispatcher) {
            // The island is gone, so the row went with it.
            continue;
        }
        try {
            auto operation = dispatcher.RunAsync(
                winrt::Windows::UI::Core::CoreDispatcherPriority::High,
                [] { StopOnCurrentThread(); });
            // The delegate releases XAML objects on its own thread; wait for it
            // so the module is not unloaded underneath it.
            for (int i = 0; i < 2000 &&
                            operation.Status() == winrt::Windows::Foundation::AsyncStatus::Started;
                 i++) {
                Sleep(1);
            }
        } catch (...) {
            Wh_Log(L"UI thread %u is gone, retaining its XAML state", id);
        }
    }
}
