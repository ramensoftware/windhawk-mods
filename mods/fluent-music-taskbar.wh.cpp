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
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Fluent Music in the taskbar

Puts the currently playing track inside the taskbar itself — a real child of the
taskbar's own XAML tree, not a window floating over it.

![The strip in the taskbar](https://raw.githubusercontent.com/xabdelrahman/flyout-taskbar/main/docs/taskbar-strip.png)

![The player popover](https://raw.githubusercontent.com/xabdelrahman/flyout-taskbar/main/docs/popover.png)

The strip shows album artwork, the track and the artist, plus a waveform that
animates only while something is actually playing. Clicking it opens a popover
with previous / play-pause / next, shuffle and repeat.

Reads the Windows System Media Transport Controls, so it follows Spotify, a
YouTube tab in a browser, or anything else that publishes a media session - no
per-app setup.

## Settings

* **Distance from the left** moves the strip along the taskbar. The default sits
  it in the empty run between the widgets button and the centred Start icons; if
  your taskbar is arranged differently, change this rather than reinstalling.
* Artwork, the popover controls and the waveform can each be turned off.

## Compatibility

Developed and tested on Windows 11 25H2 (build 26200). It hooks
`TaskbarFrame::OnTaskbarLayoutChildBoundsChanged` in `Taskbar.View.dll`; if that
symbol is absent on your build the mod logs a failure and does nothing rather
than misbehaving.
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
  $name: Animated waveform
  $description: Animates only while something is playing
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

// winbase.h (pulled in by the Windhawk API header) defines GetCurrentTime as a
// macro, which collides with Storyboard::GetCurrentTime in the XAML animation
// projection and breaks the build. Drop the macro before the winrt headers.
#undef GetCurrentTime

#include <atomic>
#include <string>
#include <thread>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Core.h>
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
#include <winrt/base.h>

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

struct {
    int offsetX;
    bool showArtwork;
    bool showControls;
    bool showWaveform;
} g_settings;

/// Widest the bar is allowed to get; also what we reserve from the taskbar.
constexpr double kBarMaxWidth = 190;

/// Gap between the popover and the taskbar edge.
constexpr double kFlyoutGap = 10;

std::atomic<bool> g_unloading;
std::atomic<bool> g_taskbarViewDllLoaded;

WinMedia::GlobalSystemMediaTransportControlsSessionManager g_sessionManager{nullptr};
WinMedia::GlobalSystemMediaTransportControlsSession g_session{nullptr};
winrt::weak_ref<FrameworkElement> g_barElement;
winrt::weak_ref<FrameworkElement> g_taskbarFrame;

// Which track's cover each layer currently holds, so a cover that arrives after
// the title still gets written, and nothing is decoded twice.
std::wstring g_artKeyA;
std::wstring g_artKeyB;

/// Track the transition last ran for, so it plays once per song rather than on
/// every position update.
std::wstring g_animatedKey;



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

/// Uses the taskbar's own theme brushes so the bar follows light and dark without
/// us picking colours; falls back only if the resource is missing.
Media::Brush ThemeBrush(PCWSTR key, winrt::Windows::UI::Color fallback) {
    try {
        auto app = Application::Current();
        if (!app) {
            return Media::SolidColorBrush{fallback};
        }

        auto resources = app.Resources();
        auto boxedKey = winrt::box_value(winrt::hstring{key});
        if (resources.HasKey(boxedKey)) {
            if (auto brush = resources.Lookup(boxedKey).try_as<Media::Brush>()) {
                return brush;
            }
        }
    } catch (const winrt::hresult_error&) {
        // Fall through to the fallback colour.
    }

    return Media::SolidColorBrush{fallback};
}


/// Returns the first theme brush that exists, so the card can match the system
/// flyout surface without hardcoding a colour for one Windows build.
Media::Brush FirstThemeBrush(std::initializer_list<PCWSTR> keys,
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
        // Fall through.
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

Controls::Button MakeTransportButton(PCWSTR name, PCWSTR glyph) {
    Controls::Button button;
    button.Name(name);
    button.Content(MakeGlyph(glyph));
    button.Width(24);
    button.Height(24);
    button.Padding(Thickness{});
    button.BorderThickness(Thickness{});
    button.Background(Media::SolidColorBrush{winrt::Windows::UI::Colors::Transparent()});
    button.HorizontalContentAlignment(HorizontalAlignment::Center);
    button.VerticalContentAlignment(VerticalAlignment::Center);
    return button;
}


/// Wires a tap on a transport button. See the note on AddHandler above.
void OnTap(UIElement element, void (*action)()) {
    element.AddHandler(
        UIElement::PointerPressedEvent(),
        winrt::box_value(Input::PointerEventHandler(
            [action](winrt::Windows::Foundation::IInspectable const&,
                    Input::PointerRoutedEventArgs const&) {
                try {
                    if (g_session) {
                        action();
                    }
                } catch (const winrt::hresult_error&) {
                    // The media app went away between the tap and the call.
                }
            })),
        true);
}

/// Like OnTap, but runs even with no session - used to open the popover.
void OnTapAlways(UIElement element, void (*action)()) {
    element.AddHandler(
        UIElement::PointerPressedEvent(),
        winrt::box_value(Input::PointerEventHandler(
            [action](winrt::Windows::Foundation::IInspectable const&,
                    Input::PointerRoutedEventArgs const&) {
                try {
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

/// Animates ScaleY, which the compositor handles, rather than Height, which would
/// make the taskbar relayout on every frame.
void StartWaveform(Controls::StackPanel panel) {
    Media::Animation::Storyboard storyboard;

    unsigned seed = 7;
    auto nextRandom = [&seed]() {
        seed = seed * 1103515245 + 12345;
        return ((seed >> 16) & 0x7FFF) / 32767.0;
    };

    for (uint32_t ia = 0; ia < panel.Children().Size(); ia++) {
        auto bar = panel.Children().GetAt(ia).try_as<Shapes::Rectangle>();
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

    storyboard.Begin();
}



/// A snapshot handed from the reader thread to the UI thread.
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
NowPlaying g_lastState;

/// Raw artwork bytes for the current track. The popover decodes its own copy:
/// sharing one ImageBrush between the strip and the popover made the image slow
/// to swap when the track changed.
WinStreams::IRandomAccessStream g_artStream{nullptr};

/// Holds the running fade so its Completed handler is still alive when it fires.
Media::Animation::Storyboard g_swapStoryboard{nullptr};

/// Which of the two layers is currently the visible one.
bool g_frontIsB = false;

/// Delays hiding the strip. Apps report no session for a moment between tracks,
/// and collapsing the whole bar on that gap is what read as a flash.
DispatcherTimer g_hideTimer{nullptr};

/// Writes the track into one of the two layers. The layer may be the hidden one,
/// which is how the swap happens without anything blank appearing on screen.
void WriteLayer(FrameworkElement bar, NowPlaying state, bool useB) {
    if (auto title = FindDescendantByName(bar, useB ? kTitleB : kTitleA)
                         .try_as<Controls::TextBlock>()) {
        title.Text(state.title);
    }

    if (auto artist = FindDescendantByName(bar, useB ? kArtistB : kArtistA)
                          .try_as<Controls::TextBlock>()) {
        artist.Text(state.artist);
        artist.Visibility(state.artist.empty() ? Visibility::Collapsed
                                               : Visibility::Visible);
    }

    auto art = FindDescendantByName(bar, useB ? kArtB : kArtA)
                   .try_as<Controls::Border>();
    if (!art) {
        return;
    }

    art.Visibility(g_settings.showArtwork ? Visibility::Visible
                                          : Visibility::Collapsed);

    if (!g_settings.showArtwork || !state.artStream) {
        return;
    }

    auto& layerArtKey = useB ? g_artKeyB : g_artKeyA;
    if (layerArtKey == state.key) {
        return;
    }

    try {
        state.artStream.Seek(0);

        Media::Imaging::BitmapImage bitmap;
        bitmap.SetSource(state.artStream);

        Media::ImageBrush brush;
        brush.ImageSource(bitmap);
        brush.Stretch(Media::Stretch::UniformToFill);
        art.Background(brush);

        layerArtKey = state.key;
    } catch (const winrt::hresult_error& e) {
        Wh_Log(L"Artwork decode failed: %08X", e.code().value);
    }
}

/// Fades one layer up while the other fades down, both at once.
void CrossfadeLayers(FrameworkElement bar, bool toB) {
    auto incomingInfo = FindDescendantByName(bar, toB ? kInfoB : kInfoA);
    auto outgoingInfo = FindDescendantByName(bar, toB ? kInfoA : kInfoB);
    auto incomingArt = FindDescendantByName(bar, toB ? kArtB : kArtA);
    auto outgoingArt = FindDescendantByName(bar, toB ? kArtA : kArtB);

    // Stop any crossfade still in flight. A track change often reports twice in a
    // row (the artist usually arrives after the title, which changes the key), and
    // two storyboards animating the same opacities fight each other - which could
    // leave BOTH copies hidden, so only the waveform was left on screen.
    if (g_swapStoryboard) {
        g_swapStoryboard.Stop();
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

    g_swapStoryboard = storyboard;
    storyboard.Begin();
}

void ApplyToUi(FrameworkElement bar, NowPlaying state) {
    g_lastState = state;
    g_artStream = state.artStream;

    auto wave = FindDescendantByName(bar, kWaveName).try_as<Controls::StackPanel>();
    if (wave) {
        bool show = g_settings.showWaveform && state.playing;
        wave.Visibility(show ? Visibility::Visible : Visibility::Collapsed);
        if (show) {
            StartWaveform(wave);
        }
    }

    bar.Margin(Thickness{.Left = static_cast<double>(g_settings.offsetX)});

    bool haveTrack = state.hasSession && !state.title.empty();

    if (haveTrack) {
        if (g_hideTimer) {
            g_hideTimer.Stop();
        }
        bar.Visibility(Visibility::Visible);
    } else if (bar.Visibility() == Visibility::Visible) {
        // Do not hide yet: this is almost always the brief gap a player leaves
        // while switching tracks. Confirm it is really over first.
        if (!g_hideTimer) {
            g_hideTimer = DispatcherTimer();
            g_hideTimer.Interval(
                winrt::Windows::Foundation::TimeSpan{std::chrono::milliseconds{700}});
            g_hideTimer.Tick([bar](winrt::Windows::Foundation::IInspectable const&,
                                   winrt::Windows::Foundation::IInspectable const&) {
                if (g_hideTimer) {
                    g_hideTimer.Stop();
                }

                if (g_unloading) {
                    return;
                }

                if (!g_lastState.hasSession || g_lastState.title.empty()) {
                    bar.Visibility(Visibility::Collapsed);
                }
            });
        }

        g_hideTimer.Start();
    }

    bool trackChanged = !state.key.empty() && state.key != g_animatedKey;

    if (!trackChanged) {
        // Not a new song. Write into whichever layer is (or is becoming) the front
        // one - that is where this track belongs, transition running or not. This
        // is how a cover that arrives after the title still shows up.
        WriteLayer(bar, state, g_frontIsB);
        return;
    }

    g_animatedKey = state.key;

    // Fill the hidden layer, then cross-fade to it.
    bool toB = !g_frontIsB;
    WriteLayer(bar, state, toB);
    g_frontIsB = toB;

    CrossfadeLayers(bar, toB);
}

/// Reads the session off the UI thread. TryGetMediaPropertiesAsync and OpenReadAsync
/// both block, and blocking here would stall the whole taskbar.
void RefreshNowPlaying() {
    if (g_unloading) {
        return;
    }

    auto bar = g_barElement.get();
    if (!bar) {
        return;
    }

    std::thread([bar]() {
        NowPlaying state;
        auto session = g_session;

        try {
            if (session) {
                state.hasSession = true;

                auto playback = session.GetPlaybackInfo();
                state.playing =
                    playback &&
                    playback.PlaybackStatus() ==
                        WinMedia::GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing;

                if (playback) {
                    if (auto shuffle = playback.IsShuffleActive()) {
                        state.shuffle = shuffle.Value();
                    }
                    if (auto repeat = playback.AutoRepeatMode()) {
                        state.repeatOn =
                            repeat.Value() !=
                            winrt::Windows::Media::MediaPlaybackAutoRepeatMode::None;
                    }
                }

                if (auto properties = session.TryGetMediaPropertiesAsync().get()) {
                    state.title = properties.Title().c_str();
                    state.artist = properties.Artist().c_str();
                    state.key = state.title + L"|" + state.artist;

                    if (auto thumbnail = properties.Thumbnail()) {
                        auto source = thumbnail.OpenReadAsync().get();

                        // Own the bytes outright: decoding happens later, on the UI
                        // thread, by which time the app's stream may be gone.
                        WinStreams::InMemoryRandomAccessStream copy;
                        WinStreams::RandomAccessStream::CopyAsync(source, copy).get();
                        copy.Seek(0);
                        state.artStream = copy;
                    }
                }
            }
        } catch (const winrt::hresult_error&) {
            // The media app went away mid-read; show the empty state instead.
            state = NowPlaying{};
        }

        try {
            bar.Dispatcher().RunAsync(
                winrt::Windows::UI::Core::CoreDispatcherPriority::Normal,
                [bar, state]() {
                    if (!g_unloading) {
                        ApplyToUi(bar, state);
                    }
                });
        } catch (const winrt::hresult_error&) {
            // The taskbar went away; nothing to update.
        }
    }).detach();
}

// Defined below; OnSessionChanged subscribes the newly selected session to it.
void OnSessionDetailChanged(
    WinMedia::GlobalSystemMediaTransportControlsSession const&,
    winrt::Windows::Foundation::IInspectable const&);

void OnSessionChanged(WinMedia::GlobalSystemMediaTransportControlsSessionManager const&,
                    WinMedia::CurrentSessionChangedEventArgs const&) {
    try {
        g_session = g_sessionManager.GetCurrentSession();

        if (g_session) {
            g_session.MediaPropertiesChanged(OnSessionDetailChanged);
            g_session.PlaybackInfoChanged(OnSessionDetailChanged);
        }
    } catch (const winrt::hresult_error&) {
        g_session = nullptr;
    }

    RefreshNowPlaying();
}

void OnSessionDetailChanged(
    WinMedia::GlobalSystemMediaTransportControlsSession const&,
    winrt::Windows::Foundation::IInspectable const&) {
    RefreshNowPlaying();
}

void StartMediaWatch() {
    std::thread([]() {
        try {
            g_sessionManager = WinMedia::GlobalSystemMediaTransportControlsSessionManager::
                RequestAsync()
                    .get();
            g_session = g_sessionManager.GetCurrentSession();

            g_sessionManager.CurrentSessionChanged(OnSessionChanged);

            if (g_session) {
                g_session.MediaPropertiesChanged(OnSessionDetailChanged);
                g_session.PlaybackInfoChanged(OnSessionDetailChanged);
            }

            RefreshNowPlaying();
        } catch (const winrt::hresult_error& e) {
            Wh_Log(L"Media session watch failed: %08X", e.code().value);
        }
    }).detach();
}


/// A toggle-looking button for shuffle and repeat, tinted when the mode is on.
Controls::Button MakeToggleButton(PCWSTR glyph, bool active) {
    auto button = MakeTransportButton(L"", glyph);
    button.Width(24);
    button.Height(24);

    if (active) {
        button.Background(ThemeBrush(
            L"AccentFillColorDefaultBrush",
            winrt::Windows::UI::ColorHelper::FromArgb(0xFF, 0x00, 0x78, 0xD4)));
    }

    return button;
}

/// Opens the player popover: full artwork, track, transport, shuffle and repeat.
/// Built fresh each time from the last snapshot, so nothing needs syncing.
void ShowPlayerFlyout() {
    auto anchor = g_barElement.get();
    if (!anchor) {
        return;
    }

    const auto& state = g_lastState;

    // Landscape card: artwork on the left, everything else stacked beside it.
    // A tall column gets clipped when the flyout opens above the taskbar.
    Controls::StackPanel root;
    root.Orientation(Controls::Orientation::Horizontal);
    root.Spacing(10);
    root.Padding(Thickness{});

    Controls::Border art;
    art.Width(36);
    art.Height(36);
    art.CornerRadius(CornerRadius{5, 5, 5, 5});
    art.VerticalAlignment(VerticalAlignment::Center);
    if (g_artStream) {
        try {
            Media::Imaging::BitmapImage bitmap;
            g_artStream.Seek(0);
            bitmap.SetSource(g_artStream);

            Media::ImageBrush brush;
            brush.ImageSource(bitmap);
            brush.Stretch(Media::Stretch::UniformToFill);
            art.Background(brush);
        } catch (const winrt::hresult_error&) {
            art.Background(Media::SolidColorBrush{
                winrt::Windows::UI::ColorHelper::FromArgb(0x24, 0xFF, 0xFF, 0xFF)});
        }
    } else {
        art.Background(Media::SolidColorBrush{
            winrt::Windows::UI::ColorHelper::FromArgb(0x24, 0xFF, 0xFF, 0xFF)});
    }
    root.Children().Append(art);

    Controls::StackPanel details;
    details.Orientation(Controls::Orientation::Vertical);
    details.VerticalAlignment(VerticalAlignment::Center);
    details.Spacing(0);
    details.MinWidth(124);

    Controls::TextBlock title;
    title.Text(state.title.empty() ? L"Nothing playing" : state.title.c_str());
    title.FontSize(12);
    title.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    title.Foreground(ThemeBrush(L"TextFillColorPrimaryBrush",
                                winrt::Windows::UI::Colors::White()));
    title.TextTrimming(TextTrimming::CharacterEllipsis);
    title.TextWrapping(TextWrapping::NoWrap);
    title.MaxWidth(150);
    details.Children().Append(title);

    if (!state.artist.empty()) {
        Controls::TextBlock artist;
        artist.Text(state.artist.c_str());
        artist.FontSize(11);
        artist.Foreground(ThemeBrush(
            L"TextFillColorSecondaryBrush",
            winrt::Windows::UI::ColorHelper::FromArgb(0xA0, 0xFF, 0xFF, 0xFF)));
        artist.TextTrimming(TextTrimming::CharacterEllipsis);
        artist.TextWrapping(TextWrapping::NoWrap);
        artist.MaxWidth(150);
        details.Children().Append(artist);
    }

    if (g_settings.showControls) {
        Controls::StackPanel transport;
        transport.Orientation(Controls::Orientation::Horizontal);
        transport.HorizontalAlignment(HorizontalAlignment::Left);
        transport.Spacing(1);
        transport.VerticalAlignment(VerticalAlignment::Center);

        auto shuffle = MakeToggleButton(L"", state.shuffle);
        OnTap(shuffle, []() {
            g_session.TryChangeShuffleActiveAsync(!g_lastState.shuffle);
        });
        transport.Children().Append(shuffle);

        auto previous = MakeTransportButton(L"", L"");
        previous.Width(24);
        previous.Height(24);
        OnTap(previous, []() { g_session.TrySkipPreviousAsync(); });
        transport.Children().Append(previous);

        auto playPause =
            MakeTransportButton(L"", state.playing ? L"" : L"");
        playPause.Width(28);
        playPause.Height(28);
        OnTap(playPause, []() { g_session.TryTogglePlayPauseAsync(); });
        transport.Children().Append(playPause);

        auto next = MakeTransportButton(L"", L"");
        next.Width(24);
        next.Height(24);
        OnTap(next, []() { g_session.TrySkipNextAsync(); });
        transport.Children().Append(next);

        auto repeat = MakeToggleButton(L"", state.repeatOn);
        OnTap(repeat, []() {
            g_session.TryChangeAutoRepeatModeAsync(
                g_lastState.repeatOn
                    ? winrt::Windows::Media::MediaPlaybackAutoRepeatMode::None
                    : winrt::Windows::Media::MediaPlaybackAutoRepeatMode::List);
        });
        transport.Children().Append(repeat);

        root.Children().Append(details);
        root.Children().Append(transport);
    } else {
        root.Children().Append(details);
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
    card.Background(FirstThemeBrush(
        {L"AcrylicInAppFillColorDefaultBrush",
         L"AcrylicBackgroundFillColorDefaultBrush",
         L"SystemControlAcrylicElementBrush",
         L"FlyoutPresenterBackground"},
        winrt::Windows::UI::ColorHelper::FromArgb(0xFF, 0xF3, 0xF3, 0xF3)));
    card.BorderBrush(FirstThemeBrush(
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

    if (FindChildByName(containerGrid, kBarName)) {
        return;
    }


    auto primaryText = ThemeBrush(L"TextFillColorPrimaryBrush",
                                winrt::Windows::UI::Colors::White());
    auto secondaryText = ThemeBrush(L"TextFillColorSecondaryBrush",
                                    winrt::Windows::UI::ColorHelper::FromArgb(
                                        0xA0, 0xFF, 0xFF, 0xFF));


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


    // The whole strip opens the popover.
    bar.Background(Media::SolidColorBrush{winrt::Windows::UI::Colors::Transparent()});
    OnTapAlways(bar, ShowPlayerFlyout);

    // Hidden until we know something is playing, so it never flashes empty.
    bar.Visibility(Visibility::Collapsed);

    containerGrid.Children().Append(bar);
    g_barElement = bar;

    Wh_Log(L"Fluent Music bar added to the taskbar");

    RefreshNowPlaying();
}

void RemoveBar() {
    if (auto frame = g_taskbarFrame.get()) {
        auto margin = frame.Margin();
        if (margin.Left != 0) {
            margin.Left = 0;
            frame.Margin(margin);
        }
    }

    auto bar = g_barElement.get();
    if (!bar) {
        return;
    }

    auto parent = Media::VisualTreeHelper::GetParent(bar).try_as<Controls::Panel>();
    if (parent) {
        uint32_t index;
        if (parent.Children().IndexOf(bar, index)) {
            parent.Children().RemoveAt(index);
        }
    }

    g_barElement = nullptr;
    g_artKeyA.clear();
    g_artKeyB.clear();
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
        taskbarFrameElement.Dispatcher().RunAsync(
            winrt::Windows::UI::Core::CoreDispatcherPriority::Normal,
            [taskbarFrameElement]() {
                if (g_unloading) {
                    return;
                }

                try {
                    g_taskbarFrame = taskbarFrameElement;
                    EnsureBar(taskbarFrameElement);
                } catch (const winrt::hresult_error& e) {
                    Wh_Log(L"EnsureBar failed: %08X", e.code().value);
                } catch (...) {
                    Wh_Log(L"EnsureBar failed with an unknown exception");
                }
            });
    } catch (const winrt::hresult_error& e) {
        Wh_Log(L"Taskbar hook failed: %08X", e.code().value);
    } catch (const std::exception&) {
        Wh_Log(L"Taskbar hook failed with a std::exception");
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

    StartMediaWatch();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;
    RemoveBar();

    // Give the taskbar a moment to finish any in-flight layout pass before the
    // mod's code is unloaded from under it.
    Sleep(200);
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
    RefreshNowPlaying();
}
