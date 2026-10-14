// ==WindhawkMod==
// @id              fluent-music-taskbar
// @name            Fluent Music in the taskbar
// @description     Now playing inside the taskbar, with a player popover
// @version         1.0.0
// @author          Abdelrahman Tarek
// @github          https://github.com/xabdelrahman
// @include         explorer.exe
// @architecture    x86-64
// @license         MIT
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Fluent Music in the taskbar

Puts the currently playing track inside the taskbar itself — a real child of the
taskbar's own XAML tree, not a window floating over it.

![The strip in the taskbar](https://raw.githubusercontent.com/xabdelrahman/flyout-taskbar/main/docs/taskbar-strip.png)

![The player popover](https://raw.githubusercontent.com/xabdelrahman/flyout-taskbar/main/docs/popover.png)

The strip shows album artwork, the track and the artist, plus an animation that
runs only while something is actually playing. Clicking it opens a popover with
previous / play-pause / next, shuffle and repeat.

Reads the Windows System Media Transport Controls, so it follows Spotify, a
YouTube tab in a browser, or anything else that publishes a media session - no
per-app setup.

## How this compares to the other media mods

The catalog already has **Taskbar Fluent Media Player**, **Island Media
Controls** and **Taskbar Music Lounge**, and the first two also inject into the
taskbar's XAML tree rather than floating a window over it. So "really inside the
taskbar" is not what sets this one apart.

What is different is the scope. This mod is deliberately small: one hooked
symbol, one fixed layout, four settings. Taskbar Fluent Media Player is the
full-featured option - 20+ placements, auto-hide, theme-aware materials - and if
you want those, use it instead. Pick this one if you want a minimal strip with a
cross-fade between tracks and a decorative playing indicator, and nothing else
to configure.

## Settings

* **Distance from the left** moves the strip along the taskbar. The strip is
  drawn *over* the taskbar rather than reserving space in it, so it needs a clear
  run: anything underneath it cannot be clicked. The default sits it in the empty
  gap between the widgets button and the centred Start icons. With a left-aligned
  taskbar, or with many windows open and a centred taskbar, the task buttons can
  grow under it - move the strip if that happens.
* Album artwork and the popover controls can each be turned off.
* **Playing animation** is a decorative indicator, not an audio visualizer: it
  is not driven by the audio signal, it only tells you something is playing.

## Compatibility

Developed and tested on Windows 11 25H2 (build 26200). It hooks
`TaskbarFrame::OnTaskbarLayoutChildBoundsChanged` in `Taskbar.View.dll`; if that
symbol is absent on your build the mod logs a failure and does nothing rather
than misbehaving.

Secondary monitors each get their own strip, and all of them update together.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- offsetX: 120
  $name: Distance from the left
  $description: How far along the taskbar the bar sits, in pixels
- showArtwork: true
  $name: Album artwork
- showControls: true
  $name: Controls in the popover
  $description: Previous, play/pause, next, shuffle and repeat
- showWaveform: true
  $name: Playing animation
  $description: A decorative indicator, not driven by the audio
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

// winbase.h (pulled in by the Windhawk API header) defines GetCurrentTime as a
// macro, which collides with Storyboard::GetCurrentTime in the XAML animation
// projection and breaks the build. Drop the macro before the winrt headers.
#undef GetCurrentTime

#include <atomic>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <vector>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>

using namespace winrt::Windows::UI::Xaml;
namespace WinMedia = winrt::Windows::Media::Control;
namespace WinStreams = winrt::Windows::Storage::Streams;

// Named so the elements can be found again rather than tracked in globals, and so
// the bar is never added twice to the same taskbar.
constexpr PCWSTR kBarName = L"FluentMusicBar";
constexpr PCWSTR kWaveName = L"FluentMusicWave";
constexpr PCWSTR kTrackInfoName = L"FluentMusicTrackInfo";

// Two overlapping copies of the artwork and the text. A track change writes into
// whichever copy is hidden, then the two are cross-faded, so the new song is
// already on screen as the old one leaves and there is never a blank frame.
constexpr PCWSTR kArtA = L"FluentMusicArtA";
constexpr PCWSTR kArtB = L"FluentMusicArtB";
constexpr PCWSTR kInfoA = L"FluentMusicInfoA";
constexpr PCWSTR kInfoB = L"FluentMusicInfoB";
constexpr PCWSTR kTitleA = L"FluentMusicTitleA";
constexpr PCWSTR kTitleB = L"FluentMusicTitleB";
constexpr PCWSTR kArtistA = L"FluentMusicArtistA";
constexpr PCWSTR kArtistB = L"FluentMusicArtistB";

constexpr int kWaveformBars = 8;
constexpr double kBarWidth = 3;
constexpr double kBarHeight = 16;
constexpr double kArtSize = 28;
constexpr double kFlyoutArtSize = 36;

/// Widest the bar is allowed to get, so a long title never grows into the
/// centred Start icons.
constexpr double kBarMaxWidth = 190;

/// Gap between the popover and the taskbar edge.
constexpr double kFlyoutGap = 10;

struct {
    int offsetX;
    bool showArtwork;
    bool showControls;
    bool showWaveform;
} g_settings;

std::atomic<bool> g_unloading;
std::atomic<bool> g_taskbarViewDllLoaded;

// The session objects are touched from the worker thread, the WinRT callback
// threads and the UI thread, so everything about them lives behind this mutex.
// They are [[clang::no_destroy]] because releasing an out-of-process WinRT object
// at process exit marshals into an apartment whose thread may already be gone:
// https://github.com/ramensoftware/windhawk/wiki/Global-objects-and-process-shutdown
std::mutex g_sessionMutex;
[[clang::no_destroy]] WinMedia::GlobalSystemMediaTransportControlsSessionManager
    g_sessionManager{nullptr};
[[clang::no_destroy]] WinMedia::GlobalSystemMediaTransportControlsSession g_session{
    nullptr};
winrt::event_token g_currentSessionChangedToken{};
winrt::event_token g_mediaPropertiesChangedToken{};
winrt::event_token g_playbackInfoChangedToken{};

/// Everything belonging to one taskbar's strip. Secondary monitors have their own
/// taskbar and their own TaskbarFrame, so they get their own entry here; keeping
/// this per-bar is what stops one monitor's strip from being the only live one.
/// Only ever touched on the XAML UI thread.
struct BarState {
    winrt::weak_ref<FrameworkElement> bar;
    Media::Animation::Storyboard swapStoryboard{nullptr};
    Media::Animation::Storyboard waveStoryboard{nullptr};
    DispatcherTimer hideTimer{nullptr};

    // Which track's cover each layer currently holds, so a cover that arrives
    // after the title still gets written, and nothing is decoded twice.
    std::wstring artKeyA;
    std::wstring artKeyB;

    /// The track the transition last ran for, so it plays once per song rather
    /// than on every playback update.
    std::wstring animatedKey;

    /// Which of the two layers is currently the visible one.
    bool frontIsB = false;
};

// Holds XAML objects, so it must not be destroyed during process shutdown, when
// the UI thread is already gone.
[[clang::no_destroy]] std::optional<std::vector<BarState>> g_bars;

/// The taskbar's dispatcher, kept so unload can marshal the teardown back onto
/// the UI thread. Every XAML call below has thread affinity. Written from the UI
/// thread and read from the worker, so it is behind a lock for the same reason
/// the session is: copying a projected type is not atomic.
std::mutex g_dispatcherMutex;
[[clang::no_destroy]] winrt::Windows::UI::Core::CoreDispatcher g_dispatcher{nullptr};

winrt::Windows::UI::Core::CoreDispatcher GetDispatcher() {
    std::lock_guard<std::mutex> guard(g_dispatcherMutex);
    return g_dispatcher;
}

void SetDispatcherOnce(winrt::Windows::UI::Core::CoreDispatcher dispatcher) {
    std::lock_guard<std::mutex> guard(g_dispatcherMutex);
    if (!g_dispatcher) {
        g_dispatcher = dispatcher;
    }
}

std::vector<BarState>& Bars() {
    if (!g_bars) {
        g_bars.emplace();
    }

    return *g_bars;
}

void LoadSettings() {
    g_settings.offsetX = Wh_GetIntSetting(L"offsetX");
    g_settings.showArtwork = Wh_GetIntSetting(L"showArtwork");
    g_settings.showControls = Wh_GetIntSetting(L"showControls");
    g_settings.showWaveform = Wh_GetIntSetting(L"showWaveform");
}

FrameworkElement FindChildByName(FrameworkElement element, PCWSTR name) {
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < childrenCount; i++) {
        auto child =
            Media::VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
        if (child && child.Name() == name) {
            return child;
        }
    }

    return nullptr;
}

/// Recursive, because our controls sit inside nested panels.
FrameworkElement FindDescendantByName(FrameworkElement element, PCWSTR name) {
    if (!element) {
        return nullptr;
    }

    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < childrenCount; i++) {
        auto child =
            Media::VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
        if (!child) {
            continue;
        }

        if (child.Name() == name) {
            return child;
        }

        if (auto found = FindDescendantByName(child, name)) {
            return found;
        }
    }

    return nullptr;
}

/// Returns the first theme brush that exists, so the bar follows light and dark
/// without us picking colours, and the popover can match the system flyout
/// surface without hardcoding a colour for one Windows build.
Media::Brush ThemeBrush(std::initializer_list<PCWSTR> keys,
                        winrt::Windows::UI::Color fallback) {
    try {
        if (auto app = Application::Current()) {
            auto resources = app.Resources();
            for (PCWSTR key : keys) {
                auto boxed = winrt::box_value(winrt::hstring{key});
                if (resources.HasKey(boxed)) {
                    if (auto brush = resources.Lookup(boxed).try_as<Media::Brush>()) {
                        return brush;
                    }
                }
            }
        }
    } catch (const winrt::hresult_error&) {
        // Fall through to the fallback colour.
    }

    return Media::SolidColorBrush{fallback};
}

Controls::FontIcon MakeGlyph(PCWSTR glyph) {
    Controls::FontIcon icon;
    icon.FontFamily(Media::FontFamily{L"Segoe Fluent Icons"});
    icon.Glyph(glyph);
    icon.FontSize(12);
    return icon;
}

Controls::Button MakeTransportButton(PCWSTR glyph, double size) {
    Controls::Button button;
    button.Content(MakeGlyph(glyph));
    button.Width(size);
    button.Height(size);
    button.Padding(Thickness{});
    button.BorderThickness(Thickness{});
    button.Background(Media::SolidColorBrush{winrt::Windows::UI::Colors::Transparent()});
    button.HorizontalContentAlignment(HorizontalAlignment::Center);
    button.VerticalContentAlignment(VerticalAlignment::Center);
    return button;
}

/// Wires a left-click. PointerPressed rather than Click because Click is not
/// reliably hookable from here, but it fires for every button, so a right-click
/// would otherwise open the popover and swallow the taskbar context menu.
template <typename TAction>
void OnLeftTap(UIElement element, TAction action) {
    element.AddHandler(
        UIElement::PointerPressedEvent(),
        winrt::box_value(Input::PointerEventHandler(
            [action](winrt::Windows::Foundation::IInspectable const& sender,
                     Input::PointerRoutedEventArgs const& args) {
                try {
                    auto element = sender.try_as<UIElement>();
                    if (!element) {
                        return;
                    }

                    auto point = args.GetCurrentPoint(element);
                    if (!point || !point.Properties().IsLeftButtonPressed()) {
                        return;
                    }

                    action();
                } catch (const winrt::hresult_error&) {
                    // Never let a click take the taskbar down with it.
                }
            })),
        true);
}

/// Bars scale about their centre line, so they grow symmetrically the way an audio
/// waveform reads, rather than up from a baseline.
Controls::StackPanel CreateWaveform() {
    Controls::StackPanel panel;
    panel.Name(kWaveName);
    panel.Orientation(Controls::Orientation::Horizontal);
    panel.VerticalAlignment(VerticalAlignment::Center);
    panel.Spacing(2);

    Media::SolidColorBrush amber{
        winrt::Windows::UI::ColorHelper::FromArgb(0xFF, 0xF0, 0xA8, 0x68)};

    for (int i = 0; i < kWaveformBars; i++) {
        Shapes::Rectangle bar;
        bar.Width(kBarWidth);
        bar.Height(kBarHeight);
        bar.RadiusX(kBarWidth / 2);
        bar.RadiusY(kBarWidth / 2);
        bar.Fill(amber);
        bar.VerticalAlignment(VerticalAlignment::Center);

        Media::ScaleTransform transform;
        transform.ScaleX(1);
        transform.ScaleY(0.28);
        transform.CenterY(kBarHeight / 2);
        bar.RenderTransform(transform);

        panel.Children().Append(bar);
    }

    return panel;
}

/// Builds the forever-running animation. Animates ScaleY, which the compositor
/// handles, rather than Height, which would relayout the taskbar every frame.
Media::Animation::Storyboard BuildWaveformStoryboard(Controls::StackPanel panel) {
    Media::Animation::Storyboard storyboard;

    unsigned seed = 7;
    auto nextRandom = [&seed]() {
        seed = seed * 1103515245 + 12345;
        return ((seed >> 16) & 0x7FFF) / 32767.0;
    };

    for (uint32_t i = 0; i < panel.Children().Size(); i++) {
        auto bar = panel.Children().GetAt(i).try_as<Shapes::Rectangle>();
        if (!bar) {
            continue;
        }

        auto transform = bar.RenderTransform().try_as<Media::ScaleTransform>();
        if (!transform) {
            continue;
        }

        Media::Animation::DoubleAnimation animation;
        animation.From(0.22);
        animation.To(0.55 + nextRandom() * 0.45);
        animation.Duration(Duration{std::chrono::milliseconds{
            static_cast<int>(420 + nextRandom() * 460)}});
        animation.BeginTime(winrt::Windows::Foundation::TimeSpan{
            std::chrono::milliseconds{static_cast<int>(nextRandom() * 380)}});
        animation.AutoReverse(true);

        Media::Animation::RepeatBehavior repeat{};
        repeat.Type = Media::Animation::RepeatBehaviorType::Forever;
        animation.RepeatBehavior(repeat);

        Media::Animation::SineEase ease;
        ease.EasingMode(Media::Animation::EasingMode::EaseInOut);
        animation.EasingFunction(ease);

        Media::Animation::Storyboard::SetTarget(animation, transform);
        Media::Animation::Storyboard::SetTargetProperty(animation, L"ScaleY");
        storyboard.Children().Append(animation);
    }

    return storyboard;
}

/// One storyboard per bar, started once. Rebuilding it on every playback update
/// would stack another eight Forever animations on the same transforms, and
/// nothing would ever stop them.
void EnsureWaveformRunning(BarState& state, Controls::StackPanel panel) {
    if (!state.waveStoryboard) {
        state.waveStoryboard = BuildWaveformStoryboard(panel);
    }

    if (state.waveStoryboard.GetCurrentState() !=
        Media::Animation::ClockState::Active) {
        state.waveStoryboard.Begin();
    }
}

void StopWaveform(BarState& state) {
    if (state.waveStoryboard) {
        state.waveStoryboard.Stop();
    }
}

/// A snapshot handed from the worker thread to the UI thread.
struct NowPlaying {
    std::wstring title;
    std::wstring artist;
    std::wstring key;
    bool playing = false;
    bool hasSession = false;
    bool shuffle = false;
    bool repeatOn = false;
    WinStreams::IRandomAccessStream artStream{nullptr};
};

/// Last state applied to the UI, so the popover can render without a fresh read.
/// UI thread only. Holds a WinRT stream, so it must outlive process shutdown.
[[clang::no_destroy]] NowPlaying g_lastState;

/// Raw artwork bytes for the current track. The popover decodes its own copy:
/// sharing one ImageBrush between the strip and the popover made the image slow
/// to swap when the track changed.
[[clang::no_destroy]] WinStreams::IRandomAccessStream g_artStream{nullptr};

/// Copies the session under the lock. Callers must use the returned local rather
/// than touching the global again: a projected type is not atomic, so a
/// concurrent assignment could otherwise release the object under the reader.
WinMedia::GlobalSystemMediaTransportControlsSession CurrentSession() {
    std::lock_guard<std::mutex> guard(g_sessionMutex);
    return g_session;
}

/// Decodes artwork no larger than the tile it goes into. Without this a 640x640
/// cover is decoded at full size into every layer, on the UI thread.
Media::ImageBrush MakeArtBrush(WinStreams::IRandomAccessStream stream, double size) {
    stream.Seek(0);

    Media::Imaging::BitmapImage bitmap;
    bitmap.DecodePixelType(Media::Imaging::DecodePixelType::Logical);
    bitmap.DecodePixelWidth(static_cast<int32_t>(size));
    bitmap.DecodePixelHeight(static_cast<int32_t>(size));
    bitmap.SetSource(stream);

    Media::ImageBrush brush;
    brush.ImageSource(bitmap);
    brush.Stretch(Media::Stretch::UniformToFill);
    return brush;
}

/// Writes the track into one of the two layers. The layer may be the hidden one,
/// which is how the swap happens without anything blank appearing on screen.
void WriteLayer(BarState& state, FrameworkElement bar, NowPlaying const& track,
                bool useB) {
    if (auto title = FindDescendantByName(bar, useB ? kTitleB : kTitleA)
                         .try_as<Controls::TextBlock>()) {
        title.Text(track.title);
    }

    if (auto artist = FindDescendantByName(bar, useB ? kArtistB : kArtistA)
                          .try_as<Controls::TextBlock>()) {
        artist.Text(track.artist);
        artist.Visibility(track.artist.empty() ? Visibility::Collapsed
                                               : Visibility::Visible);
    }

    auto art = FindDescendantByName(bar, useB ? kArtB : kArtA)
                   .try_as<Controls::Border>();
    if (!art) {
        return;
    }

    art.Visibility(g_settings.showArtwork ? Visibility::Visible
                                          : Visibility::Collapsed);

    if (!g_settings.showArtwork || !track.artStream) {
        return;
    }

    auto& layerArtKey = useB ? state.artKeyB : state.artKeyA;
    if (layerArtKey == track.key) {
        return;
    }

    try {
        art.Background(MakeArtBrush(track.artStream, kArtSize));
        layerArtKey = track.key;
    } catch (const winrt::hresult_error& e) {
        Wh_Log(L"Artwork decode failed: %08X", e.code().value);
    }
}

/// Fades one layer up while the other fades down, both at once.
void CrossfadeLayers(BarState& state, FrameworkElement bar, bool toB) {
    auto incomingInfo = FindDescendantByName(bar, toB ? kInfoB : kInfoA);
    auto outgoingInfo = FindDescendantByName(bar, toB ? kInfoA : kInfoB);
    auto incomingArt = FindDescendantByName(bar, toB ? kArtB : kArtA);
    auto outgoingArt = FindDescendantByName(bar, toB ? kArtA : kArtB);

    // Stop any crossfade still in flight. A track change often reports twice in a
    // row (the artist usually arrives after the title, which changes the key), and
    // two storyboards animating the same opacities fight each other - which could
    // leave BOTH copies hidden, so only the waveform was left on screen.
    if (state.swapStoryboard) {
        state.swapStoryboard.Stop();
    }

    // A stopped storyboard releases its hold on Opacity, so put the layers into a
    // known state before animating rather than trusting whatever they were left at.
    if (incomingInfo) {
        incomingInfo.Opacity(0.0);
    }
    if (outgoingInfo) {
        outgoingInfo.Opacity(1.0);
    }
    if (incomingArt) {
        incomingArt.Opacity(0.0);
    }
    if (outgoingArt) {
        outgoingArt.Opacity(1.0);
    }

    Media::Animation::Storyboard storyboard;
    Media::Animation::CubicEase ease;
    ease.EasingMode(Media::Animation::EasingMode::EaseInOut);

    auto duration = Duration{std::chrono::milliseconds{150}};

    auto fade = [&](FrameworkElement element, double to) {
        if (!element) {
            return;
        }

        Media::Animation::DoubleAnimation animation;
        animation.To(to);
        animation.Duration(duration);
        animation.EasingFunction(ease);
        Media::Animation::Storyboard::SetTarget(animation, element);
        Media::Animation::Storyboard::SetTargetProperty(animation, L"Opacity");
        storyboard.Children().Append(animation);
    };

    fade(incomingInfo, 1.0);
    fade(outgoingInfo, 0.0);
    fade(incomingArt, 1.0);
    fade(outgoingArt, 0.0);

    // A small lift on the incoming text so the change reads as a swap rather than
    // a dissolve. The outgoing copy stays put and just fades.
    if (incomingInfo) {
        if (auto transform =
                incomingInfo.RenderTransform().try_as<Media::TranslateTransform>()) {
            Media::Animation::DoubleAnimation slide;
            slide.From(5.0);
            slide.To(0.0);
            slide.Duration(duration);
            slide.EasingFunction(ease);
            Media::Animation::Storyboard::SetTarget(slide, transform);
            Media::Animation::Storyboard::SetTargetProperty(slide, L"Y");
            storyboard.Children().Append(slide);
        }
    }

    state.swapStoryboard = storyboard;
    storyboard.Begin();
}

void ApplyToBar(BarState& state, FrameworkElement bar, NowPlaying const& track) {
    auto wave = FindDescendantByName(bar, kWaveName).try_as<Controls::StackPanel>();
    if (wave) {
        bool show = g_settings.showWaveform && track.playing;
        wave.Visibility(show ? Visibility::Visible : Visibility::Collapsed);
        if (show) {
            EnsureWaveformRunning(state, wave);
        } else {
            StopWaveform(state);
        }
    }

    bar.Margin(Thickness{.Left = static_cast<double>(g_settings.offsetX)});

    bool haveTrack = track.hasSession && !track.title.empty();

    if (haveTrack) {
        if (state.hideTimer) {
            state.hideTimer.Stop();
        }
        bar.Visibility(Visibility::Visible);
    } else if (bar.Visibility() == Visibility::Visible) {
        // Do not hide yet: this is almost always the brief gap a player leaves
        // while switching tracks. Confirm it is really over first.
        if (!state.hideTimer) {
            state.hideTimer = DispatcherTimer();
            state.hideTimer.Interval(
                winrt::Windows::Foundation::TimeSpan{std::chrono::milliseconds{700}});

            winrt::weak_ref<FrameworkElement> weakBar = bar;
            auto timer = state.hideTimer;
            state.hideTimer.Tick([weakBar, timer](
                                     winrt::Windows::Foundation::IInspectable const&,
                                     winrt::Windows::Foundation::IInspectable const&) {
                timer.Stop();

                if (g_unloading) {
                    return;
                }

                if (!g_lastState.hasSession || g_lastState.title.empty()) {
                    if (auto target = weakBar.get()) {
                        target.Visibility(Visibility::Collapsed);
                    }
                }
            });
        }

        state.hideTimer.Start();
    }

    bool trackChanged = !track.key.empty() && track.key != state.animatedKey;

    if (!trackChanged) {
        // Not a new song. Write into whichever layer is (or is becoming) the front
        // one - that is where this track belongs, transition running or not. This
        // is how a cover that arrives after the title still shows up.
        WriteLayer(state, bar, track, state.frontIsB);
        return;
    }

    state.animatedKey = track.key;

    // Fill the hidden layer, then cross-fade to it.
    bool toB = !state.frontIsB;
    WriteLayer(state, bar, track, toB);
    state.frontIsB = toB;

    CrossfadeLayers(state, bar, toB);
}

/// Applies a snapshot to every taskbar's strip. UI thread only.
void ApplyToUi(NowPlaying track) {
    g_lastState = track;
    g_artStream = track.artStream;

    auto& bars = Bars();
    for (auto it = bars.begin(); it != bars.end();) {
        auto bar = it->bar.get();
        if (!bar) {
            // That taskbar is gone; drop its state rather than leaking it.
            it = bars.erase(it);
            continue;
        }

        try {
            ApplyToBar(*it, bar, track);
        } catch (const winrt::hresult_error& e) {
            Wh_Log(L"Updating a bar failed: %08X", e.code().value);
        }

        ++it;
    }
}

// ---------------------------------------------------------------------------
// Media session reading
//
// One long-lived worker, woken by an event, rather than a detached thread per
// notification. A detached thread blocked in TryGetMediaPropertiesAsync().get()
// can still be running mod code when the DLL is unmapped, and there is no handle
// to wait on; this one is joined on unload. It also coalesces the burst of
// notifications a single track change produces.
// ---------------------------------------------------------------------------

[[clang::no_destroy]] std::optional<std::thread> g_refreshThread;
HANDLE g_refreshEvent;
HANDLE g_stopEvent;

// Worker thread only. The UI thread's g_lastState cannot be read from here -
// that would be the same unsynchronized cross-thread access flagged elsewhere -
// so the worker keeps its own record of what it last fetched.
[[clang::no_destroy]] std::wstring g_workerLastKey;
[[clang::no_destroy]] WinStreams::IRandomAccessStream g_workerLastArt{nullptr};

void RequestRefresh() {
    if (g_refreshEvent) {
        SetEvent(g_refreshEvent);
    }
}

NowPlaying ReadNowPlaying() {
    NowPlaying track;
    auto session = CurrentSession();

    try {
        if (!session) {
            return track;
        }

        track.hasSession = true;

        auto playback = session.GetPlaybackInfo();
        track.playing =
            playback &&
            playback.PlaybackStatus() ==
                WinMedia::GlobalSystemMediaTransportControlsSessionPlaybackStatus::
                    Playing;

        if (playback) {
            if (auto shuffle = playback.IsShuffleActive()) {
                track.shuffle = shuffle.Value();
            }
            if (auto repeat = playback.AutoRepeatMode()) {
                track.repeatOn =
                    repeat.Value() !=
                    winrt::Windows::Media::MediaPlaybackAutoRepeatMode::None;
            }
        }

        auto properties = session.TryGetMediaPropertiesAsync().get();
        if (!properties) {
            return track;
        }

        track.title = properties.Title().c_str();
        track.artist = properties.Artist().c_str();

        // The album is part of the key so that a different version of a track
        // with the same title and artist still counts as a change, and so its
        // artwork is refreshed rather than short-circuited in WriteLayer.
        track.key = track.title + L"|" + track.artist + L"|" +
                    std::wstring{properties.AlbumTitle().c_str()};

        // Re-opening and copying the cover on every playback update is wasted
        // work; only the track changing can change the artwork.
        if (track.key == g_workerLastKey && g_workerLastArt) {
            track.artStream = g_workerLastArt;
            return track;
        }

        if (auto thumbnail = properties.Thumbnail()) {
            auto source = thumbnail.OpenReadAsync().get();

            // Own the bytes outright: decoding happens later, on the UI thread,
            // by which time the app's stream may be gone.
            WinStreams::InMemoryRandomAccessStream copy;
            WinStreams::RandomAccessStream::CopyAsync(source, copy).get();
            copy.Seek(0);
            track.artStream = copy;
        }

        g_workerLastKey = track.key;
        g_workerLastArt = track.artStream;
    } catch (const winrt::hresult_error&) {
        // The media app went away mid-read; show the empty state instead.
        g_workerLastKey.clear();
        g_workerLastArt = nullptr;
        return NowPlaying{};
    }

    return track;
}

void OnSessionDetailChanged(
    WinMedia::GlobalSystemMediaTransportControlsSession const&,
    winrt::Windows::Foundation::IInspectable const&) {
    RequestRefresh();
}

/// Drops the handlers on the session we are leaving. Without this every session
/// that was ever current keeps its handlers and keeps waking the worker - and
/// after unload those delegates point into an unmapped image.
void UnsubscribeFromSessionNoLock() {
    if (!g_session) {
        return;
    }

    try {
        g_session.MediaPropertiesChanged(g_mediaPropertiesChangedToken);
        g_session.PlaybackInfoChanged(g_playbackInfoChangedToken);
    } catch (const winrt::hresult_error& e) {
        Wh_Log(L"Unsubscribing from the session failed: %08X", e.code().value);
    }

    g_mediaPropertiesChangedToken = {};
    g_playbackInfoChangedToken = {};
    g_session = nullptr;
}

void SubscribeToCurrentSessionNoLock() {
    UnsubscribeFromSessionNoLock();

    if (!g_sessionManager || g_unloading) {
        return;
    }

    try {
        auto session = g_sessionManager.GetCurrentSession();
        if (!session) {
            return;
        }

        g_session = session;
        g_mediaPropertiesChangedToken =
            session.MediaPropertiesChanged(OnSessionDetailChanged);
        g_playbackInfoChangedToken =
            session.PlaybackInfoChanged(OnSessionDetailChanged);
    } catch (const winrt::hresult_error& e) {
        Wh_Log(L"Subscribing to the session failed: %08X", e.code().value);
        g_session = nullptr;
    }
}

void OnCurrentSessionChanged(
    WinMedia::GlobalSystemMediaTransportControlsSessionManager const&,
    WinMedia::CurrentSessionChangedEventArgs const&) {
    {
        std::lock_guard<std::mutex> guard(g_sessionMutex);
        SubscribeToCurrentSessionNoLock();
    }

    RequestRefresh();
}

void MediaSessionInit() {
    try {
        auto manager =
            WinMedia::GlobalSystemMediaTransportControlsSessionManager::RequestAsync()
                .get();

        if (g_unloading) {
            return;
        }

        std::lock_guard<std::mutex> guard(g_sessionMutex);
        g_sessionManager = manager;
        g_currentSessionChangedToken =
            manager.CurrentSessionChanged(OnCurrentSessionChanged);
        SubscribeToCurrentSessionNoLock();
    } catch (const winrt::hresult_error& e) {
        Wh_Log(L"Media session init failed: %08X", e.code().value);
    }
}

/// Revokes every handler, so nothing can call into the mod image after unload.
void MediaSessionUninit() {
    WinMedia::GlobalSystemMediaTransportControlsSessionManager manager{nullptr};
    winrt::event_token currentSessionChangedToken{};

    {
        std::lock_guard<std::mutex> guard(g_sessionMutex);

        UnsubscribeFromSessionNoLock();

        manager = std::move(g_sessionManager);
        g_sessionManager = nullptr;
        currentSessionChangedToken = g_currentSessionChangedToken;
        g_currentSessionChangedToken = {};
    }

    // Detach outside the lock: this waits for an in-flight CurrentSessionChanged,
    // which takes the lock itself.
    if (manager) {
        try {
            manager.CurrentSessionChanged(currentSessionChangedToken);
        } catch (const winrt::hresult_error& e) {
            Wh_Log(L"Revoking the session manager handler failed: %08X",
                   e.code().value);
        }
    }
}

void RefreshThreadProc() {
    MediaSessionInit();

    HANDLE waits[] = {g_stopEvent, g_refreshEvent};

    for (;;) {
        if (g_unloading) {
            return;
        }

        auto track = ReadNowPlaying();

        auto dispatcher = GetDispatcher();
        if (dispatcher && !g_unloading) {
            try {
                dispatcher.RunAsync(
                    winrt::Windows::UI::Core::CoreDispatcherPriority::Normal,
                    [track]() {
                        if (!g_unloading) {
                            ApplyToUi(track);
                        }
                    });
            } catch (const winrt::hresult_error&) {
                // The taskbar went away; nothing to update.
            }
        }

        DWORD waited = WaitForMultipleObjects(ARRAYSIZE(waits), waits, FALSE, INFINITE);
        if (waited != WAIT_OBJECT_0 + 1) {
            // Stop requested, or the wait failed; either way we are done.
            return;
        }
    }
}

/// Opens the player popover: full artwork, track, transport, shuffle and repeat.
/// Built fresh each time from the last snapshot, so nothing needs syncing.
void ShowPlayerFlyout(FrameworkElement anchor) {
    if (!anchor) {
        return;
    }

    const auto& track = g_lastState;

    // Landscape card: artwork on the left, everything else stacked beside it.
    // A tall column gets clipped when the flyout opens above the taskbar.
    Controls::StackPanel root;
    root.Orientation(Controls::Orientation::Horizontal);
    root.Spacing(10);
    root.Padding(Thickness{});

    auto placeholder = Media::SolidColorBrush{
        winrt::Windows::UI::ColorHelper::FromArgb(0x24, 0xFF, 0xFF, 0xFF)};

    Controls::Border art;
    art.Width(kFlyoutArtSize);
    art.Height(kFlyoutArtSize);
    art.CornerRadius(CornerRadius{5, 5, 5, 5});
    art.VerticalAlignment(VerticalAlignment::Center);
    art.Background(placeholder);
    if (g_artStream) {
        try {
            art.Background(MakeArtBrush(g_artStream, kFlyoutArtSize));
        } catch (const winrt::hresult_error&) {
            // Keep the placeholder.
        }
    }
    root.Children().Append(art);

    Controls::StackPanel details;
    details.Orientation(Controls::Orientation::Vertical);
    details.VerticalAlignment(VerticalAlignment::Center);
    details.Spacing(0);
    details.MinWidth(124);

    Controls::TextBlock title;
    title.Text(track.title.empty() ? L"Nothing playing" : track.title.c_str());
    title.FontSize(12);
    title.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    title.Foreground(ThemeBrush({L"TextFillColorPrimaryBrush"},
                                winrt::Windows::UI::Colors::White()));
    title.TextTrimming(TextTrimming::CharacterEllipsis);
    title.TextWrapping(TextWrapping::NoWrap);
    title.MaxWidth(150);
    details.Children().Append(title);

    if (!track.artist.empty()) {
        Controls::TextBlock artist;
        artist.Text(track.artist.c_str());
        artist.FontSize(11);
        artist.Foreground(ThemeBrush(
            {L"TextFillColorSecondaryBrush"},
            winrt::Windows::UI::ColorHelper::FromArgb(0xA0, 0xFF, 0xFF, 0xFF)));
        artist.TextTrimming(TextTrimming::CharacterEllipsis);
        artist.TextWrapping(TextWrapping::NoWrap);
        artist.MaxWidth(150);
        details.Children().Append(artist);
    }

    root.Children().Append(details);

    if (g_settings.showControls) {
        Controls::StackPanel transport;
        transport.Orientation(Controls::Orientation::Horizontal);
        transport.HorizontalAlignment(HorizontalAlignment::Left);
        transport.Spacing(1);
        transport.VerticalAlignment(VerticalAlignment::Center);

        auto shuffle = MakeTransportButton(L"", 24);
        if (track.shuffle) {
            shuffle.Background(ThemeBrush(
                {L"AccentFillColorDefaultBrush"},
                winrt::Windows::UI::ColorHelper::FromArgb(0xFF, 0x00, 0x78, 0xD4)));
        }
        OnLeftTap(shuffle, []() {
            if (auto session = CurrentSession()) {
                session.TryChangeShuffleActiveAsync(!g_lastState.shuffle);
            }
        });
        transport.Children().Append(shuffle);

        auto previous = MakeTransportButton(L"", 24);
        OnLeftTap(previous, []() {
            if (auto session = CurrentSession()) {
                session.TrySkipPreviousAsync();
            }
        });
        transport.Children().Append(previous);

        auto playPause =
            MakeTransportButton(track.playing ? L"" : L"", 28);
        OnLeftTap(playPause, []() {
            if (auto session = CurrentSession()) {
                session.TryTogglePlayPauseAsync();
            }
        });
        transport.Children().Append(playPause);

        auto next = MakeTransportButton(L"", 24);
        OnLeftTap(next, []() {
            if (auto session = CurrentSession()) {
                session.TrySkipNextAsync();
            }
        });
        transport.Children().Append(next);

        auto repeat = MakeTransportButton(L"", 24);
        if (track.repeatOn) {
            repeat.Background(ThemeBrush(
                {L"AccentFillColorDefaultBrush"},
                winrt::Windows::UI::ColorHelper::FromArgb(0xFF, 0x00, 0x78, 0xD4)));
        }
        OnLeftTap(repeat, []() {
            if (auto session = CurrentSession()) {
                session.TryChangeAutoRepeatModeAsync(
                    g_lastState.repeatOn
                        ? winrt::Windows::Media::MediaPlaybackAutoRepeatMode::None
                        : winrt::Windows::Media::MediaPlaybackAutoRepeatMode::List);
            }
        });
        transport.Children().Append(repeat);

        root.Children().Append(transport);
    }

    // The presenter is made invisible and this Border is the visible popover, so
    // the bottom margin becomes a real gap above the taskbar and the corner radius
    // matches the system flyouts rather than whatever the presenter defaults to.
    Controls::Border card;
    card.Child(root);
    card.CornerRadius(CornerRadius{8, 8, 8, 8});
    card.Padding(Thickness{10, 8, 10, 8});
    card.Margin(Thickness{0, 0, 0, kFlyoutGap});
    card.BorderThickness(Thickness{1, 1, 1, 1});
    card.Background(ThemeBrush(
        {L"AcrylicInAppFillColorDefaultBrush",
         L"AcrylicBackgroundFillColorDefaultBrush",
         L"SystemControlAcrylicElementBrush",
         L"FlyoutPresenterBackground"},
        winrt::Windows::UI::ColorHelper::FromArgb(0xFF, 0xF3, 0xF3, 0xF3)));
    card.BorderBrush(ThemeBrush(
        {L"SurfaceStrokeColorFlyoutBrush", L"FlyoutBorderThemeBrush"},
        winrt::Windows::UI::ColorHelper::FromArgb(0x20, 0x00, 0x00, 0x00)));

    Controls::Flyout flyout;
    flyout.Content(card);
    flyout.Placement(Controls::Primitives::FlyoutPlacementMode::Top);

    // Leave the presenter's own background alone: the system flyout style is
    // already acrylic and follows light/dark. Only nudge the geometry - a bottom
    // margin to sit clear of the taskbar, and tighter padding.
    try {
        Style presenterStyle{winrt::xaml_typename<Controls::FlyoutPresenter>()};

        presenterStyle.Setters().Append(Setter{
            Controls::Control::BackgroundProperty(),
            Media::SolidColorBrush{winrt::Windows::UI::Colors::Transparent()}});
        presenterStyle.Setters().Append(Setter{
            Controls::Control::BorderThicknessProperty(),
            winrt::box_value(Thickness{})});
        presenterStyle.Setters().Append(Setter{
            Controls::Control::PaddingProperty(), winrt::box_value(Thickness{})});
        presenterStyle.Setters().Append(Setter{
            Controls::Control::CornerRadiusProperty(),
            winrt::box_value(CornerRadius{})});
        presenterStyle.Setters().Append(Setter{
            FrameworkElement::MinWidthProperty(), winrt::box_value(0.0)});
        presenterStyle.Setters().Append(Setter{
            FrameworkElement::MinHeightProperty(), winrt::box_value(0.0)});

        flyout.FlyoutPresenterStyle(presenterStyle);
    } catch (const winrt::hresult_error&) {
        // Styling failed: the default presenter still works.
    }

    // The taskbar is a XAML island only as tall as the taskbar itself, and a flyout
    // is clipped to its island by default - which is why the popover appeared to
    // open "inside" the taskbar. This lets it render in its own window, above it.
    flyout.ShouldConstrainToRootBounds(false);

    // A presenter margin alone did not lift the card off the taskbar. ExclusionRect
    // is the documented way to keep a flyout off a region: extend the excluded area
    // above the bar and the flyout is pushed up by that much, leaving a real gap.
    Controls::Primitives::FlyoutShowOptions options;
    options.Placement(Controls::Primitives::FlyoutPlacementMode::Top);
    options.ExclusionRect(winrt::Windows::Foundation::Rect{
        0.0f,
        -static_cast<float>(kFlyoutGap),
        static_cast<float>(anchor.ActualWidth() > 0 ? anchor.ActualWidth() : 1.0),
        static_cast<float>(anchor.ActualHeight() + kFlyoutGap)});

    flyout.ShowAt(anchor, options);
}

/// Adds the bar to the taskbar's container Grid, a sibling of TaskbarFrame and the
/// system tray, so it is genuinely part of the taskbar's visual tree.
void EnsureBar(FrameworkElement taskbarFrameElement) {
    // Walk up until we find a Panel we can add a child to. The immediate parent is
    // a Grid on current builds, but the shape is not guaranteed across versions.
    Controls::Panel containerGrid{nullptr};
    FrameworkElement current = taskbarFrameElement;

    for (int depth = 0; depth < 4 && current; depth++) {
        auto parent =
            Media::VisualTreeHelper::GetParent(current).try_as<FrameworkElement>();
        if (!parent) {
            break;
        }

        if (auto panel = parent.try_as<Controls::Panel>()) {
            containerGrid = panel;
            break;
        }

        current = parent;
    }

    if (!containerGrid) {
        Wh_Log(L"No Panel ancestor for the taskbar frame; not adding the bar");
        return;
    }

    auto& bars = Bars();

    if (auto existing = FindChildByName(containerGrid, kBarName)) {
        for (auto const& state : bars) {
            if (state.bar.get() == existing) {
                // Already ours and already tracked; nothing to do.
                return;
            }
        }

        // Left behind by a previous load of the mod. Its click handler points into
        // an unmapped image, so replace it rather than leaving it in the tree.
        uint32_t index;
        if (containerGrid.Children().IndexOf(existing, index)) {
            containerGrid.Children().RemoveAt(index);
        }
    }

    auto primaryText =
        ThemeBrush({L"TextFillColorPrimaryBrush"}, winrt::Windows::UI::Colors::White());
    auto secondaryText = ThemeBrush(
        {L"TextFillColorSecondaryBrush"},
        winrt::Windows::UI::ColorHelper::FromArgb(0xA0, 0xFF, 0xFF, 0xFF));

    Controls::StackPanel bar;
    bar.Name(kBarName);
    bar.Orientation(Controls::Orientation::Horizontal);
    bar.Spacing(8);
    bar.VerticalAlignment(VerticalAlignment::Center);
    bar.HorizontalAlignment(HorizontalAlignment::Left);
    bar.Margin(Thickness{.Left = static_cast<double>(g_settings.offsetX)});

    // Never let a long title grow the bar into the centred Start icons.
    bar.MaxWidth(kBarMaxWidth);

    // Artwork, twice, stacked in a Grid. A Border with an ImageBrush background
    // rounds the corners, which a plain Image cannot do on its own.
    Controls::Grid artHost;
    artHost.Width(kArtSize);
    artHost.Height(kArtSize);
    artHost.VerticalAlignment(VerticalAlignment::Center);

    for (bool second : {false, true}) {
        Controls::Border layer;
        layer.Name(second ? kArtB : kArtA);
        layer.CornerRadius(CornerRadius{4, 4, 4, 4});
        layer.Opacity(second ? 0.0 : 1.0);
        layer.Background(Media::SolidColorBrush{
            winrt::Windows::UI::ColorHelper::FromArgb(0x24, 0xFF, 0xFF, 0xFF)});
        artHost.Children().Append(layer);
    }

    bar.Children().Append(artHost);

    // Track over artist, twice, also stacked. Same treatment as the popover:
    // bold title, dimmer artist.
    Controls::Grid infoHost;
    infoHost.Name(kTrackInfoName);
    infoHost.VerticalAlignment(VerticalAlignment::Center);

    for (bool second : {false, true}) {
        Controls::StackPanel layer;
        layer.Name(second ? kInfoB : kInfoA);
        layer.Orientation(Controls::Orientation::Vertical);
        layer.VerticalAlignment(VerticalAlignment::Center);
        layer.Spacing(0);
        layer.Opacity(second ? 0.0 : 1.0);
        layer.RenderTransform(Media::TranslateTransform{});

        Controls::TextBlock title;
        title.Name(second ? kTitleB : kTitleA);
        title.FontSize(12);
        title.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
        title.Foreground(primaryText);
        title.TextTrimming(TextTrimming::CharacterEllipsis);
        title.TextWrapping(TextWrapping::NoWrap);
        title.MaxWidth(104);
        layer.Children().Append(title);

        Controls::TextBlock artist;
        artist.Name(second ? kArtistB : kArtistA);
        artist.FontSize(11);
        artist.Foreground(secondaryText);
        artist.TextTrimming(TextTrimming::CharacterEllipsis);
        artist.TextWrapping(TextWrapping::NoWrap);
        artist.MaxWidth(104);
        layer.Children().Append(artist);

        infoHost.Children().Append(layer);
    }

    bar.Children().Append(infoHost);
    bar.Children().Append(CreateWaveform());

    // The whole strip opens the popover. Weak, so the handler does not keep the
    // bar it is attached to alive.
    bar.Background(Media::SolidColorBrush{winrt::Windows::UI::Colors::Transparent()});
    winrt::weak_ref<FrameworkElement> weakBar = bar;
    OnLeftTap(bar, [weakBar]() {
        if (!g_unloading) {
            ShowPlayerFlyout(weakBar.get());
        }
    });

    // Hidden until we know something is playing, so it never flashes empty.
    bar.Visibility(Visibility::Collapsed);

    containerGrid.Children().Append(bar);

    BarState state;
    state.bar = bar;
    bars.push_back(std::move(state));

    Wh_Log(L"Fluent Music bar added to a taskbar (%zu total)", bars.size());

    RequestRefresh();
}

/// Tears down every strip. Must run on the UI thread: all of this has thread
/// affinity and would throw RPC_E_WRONG_THREAD from Windhawk's callback thread.
void RemoveBarsOnUiThread() {
    if (!g_bars) {
        return;
    }

    for (auto& state : *g_bars) {
        try {
            if (state.hideTimer) {
                state.hideTimer.Stop();
                state.hideTimer = nullptr;
            }

            if (state.swapStoryboard) {
                state.swapStoryboard.Stop();
                state.swapStoryboard = nullptr;
            }

            if (state.waveStoryboard) {
                state.waveStoryboard.Stop();
                state.waveStoryboard = nullptr;
            }

            if (auto bar = state.bar.get()) {
                auto parent =
                    Media::VisualTreeHelper::GetParent(bar).try_as<Controls::Panel>();
                if (parent) {
                    uint32_t index;
                    if (parent.Children().IndexOf(bar, index)) {
                        parent.Children().RemoveAt(index);
                    }
                }
            }
        } catch (const winrt::hresult_error& e) {
            Wh_Log(L"Removing a bar failed: %08X", e.code().value);
        }

        state.bar = nullptr;
    }

    g_bars->clear();

    g_artStream = nullptr;
    g_lastState = NowPlaying{};
}

using TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_t = void(WINAPI*)(void* pThis);
TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_t
    TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Original;

void WINAPI TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Hook(void* pThis) {
    TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Original(pThis);

    if (g_unloading) {
        return;
    }

    try {
        // pThis is the C++/WinRT implementation object, which carries several
        // vtables; the XAML-side IUnknown sits three pointers in. This is the same
        // offset the established taskbar mods use.
        void* taskbarFrameIUnknownPtr = (void**)pThis + 3;
        winrt::Windows::Foundation::IUnknown taskbarFrameIUnknown;
        winrt::copy_from_abi(taskbarFrameIUnknown, taskbarFrameIUnknownPtr);

        auto taskbarFrameElement = taskbarFrameIUnknown.try_as<FrameworkElement>();
        if (!taskbarFrameElement) {
            Wh_Log(L"Could not recover the TaskbarFrame element");
            return;
        }

        // This hook runs *inside* a layout pass, where walking or mutating the
        // visual tree is an invalid operation (XAML reports 0x800F1000). Defer to
        // the dispatcher so the work happens once layout has finished.
        auto dispatcher = taskbarFrameElement.Dispatcher();
        SetDispatcherOnce(dispatcher);

        dispatcher.RunAsync(
            winrt::Windows::UI::Core::CoreDispatcherPriority::Normal,
            [taskbarFrameElement]() {
                if (g_unloading) {
                    return;
                }

                try {
                    EnsureBar(taskbarFrameElement);
                } catch (const winrt::hresult_error& e) {
                    Wh_Log(L"EnsureBar failed: %08X", e.code().value);
                } catch (...) {
                    Wh_Log(L"EnsureBar failed with an unknown exception");
                }
            });
    } catch (const winrt::hresult_error& e) {
        Wh_Log(L"Taskbar hook failed: %08X", e.code().value);
    } catch (...) {
        Wh_Log(L"Taskbar hook failed with an unknown exception");
    }
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }

    return module;
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarFrame::OnTaskbarLayoutChildBoundsChanged(void))"},
            &TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Original,
            TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Hook,
        },
    };

    return HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks));
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                HANDLE hFile,
                                DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (!module) {
        return module;
    }

    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Taskbar.View.dll loaded");
        if (HookTaskbarViewDllSymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }

    return module;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    // Auto-reset, initially clear: the worker does a read before its first wait.
    g_refreshEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    g_stopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_refreshEvent || !g_stopEvent) {
        Wh_Log(L"CreateEvent failed");
        return FALSE;
    }

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
            Wh_Log(L"HookSymbols failed");
            return FALSE;
        }
    } else {
        Wh_Log(L"Taskbar view module not loaded yet");

        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(
            kernelBaseModule, "LoadLibraryExW");
        WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                    LoadLibraryExW_Hook,
                                    &LoadLibraryExW_Original);
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    g_refreshThread.emplace(RefreshThreadProc);
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    // Revoke first, so nothing can wake the worker or call into the mod image
    // while the rest of the teardown runs.
    MediaSessionUninit();
}

void Wh_ModUninit() {
    Wh_Log(L">");

    // Stop and join the worker before touching anything it uses. A detached
    // thread still running mod code when the DLL is unmapped crashes Explorer.
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }

    if (g_refreshThread && g_refreshThread->joinable()) {
        g_refreshThread->join();
    }
    g_refreshThread.reset();

    // Now the UI teardown, on the UI thread and synchronously. Low priority so it
    // runs after every Normal item the hook and the worker already queued.
    auto dispatcher = GetDispatcher();
    if (dispatcher) {
        try {
            dispatcher
                .RunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::Low,
                          []() { RemoveBarsOnUiThread(); })
                .get();
        } catch (const winrt::hresult_error& e) {
            Wh_Log(L"UI teardown failed: %08X", e.code().value);
        }
    }

    {
        std::lock_guard<std::mutex> guard(g_dispatcherMutex);
        g_dispatcher = nullptr;
    }

    if (g_refreshEvent) {
        CloseHandle(g_refreshEvent);
        g_refreshEvent = nullptr;
    }

    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
    RequestRefresh();
}
