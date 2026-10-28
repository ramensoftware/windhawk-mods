// ==WindhawkMod==
// @id              quick-settings-media-progress-bar
// @name            Quick Settings Media Progress Bar
// @description     Adds a modern, interactive Fluent seek bar and live timestamps to the Windows 11 Quick Settings media player card.
// @version         1.2.12
// @author          Peter2oo1
// @github          https://github.com/Peter2oo1
// @include         ShellHost.exe
// @include         ShellExperienceHost.exe
// @architecture    x86-64
// @compilerOptions -DWIN32_LEAN_AND_MEAN -lole32 -loleaut32 -lruntimeobject -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Quick Settings Media Progress Bar for Windows 11

Adds a modern, interactive seek bar and live playback timestamps directly to the default Windows 11 Quick Settings (`Win + A`) media player widget.

![Preview](https://i.imgur.com/DGY7Rtj.png)

### Features
- **Interactive Seeking & Scrubbing:** Click or drag the progress bar thumb to seek smoothly within any playing track.
- **Mouse Wheel Scrubbing:** Hover anywhere over the progress bar and scroll the mouse wheel to seek forward or backward.
- **Accurate Timestamps:** Displays current playback time and total track duration (or remaining time).
- **Smooth Playback Sync:** Automatically syncs with Windows System Media Transport Controls (SMTC) in real time across Spotify, Microsoft Edge, Google Chrome, Apple Music, VLC, Windows Media Player, and more.
- **Accidental Click Protection:** Interacting with the progress bar or timestamps will not trigger the underlying card button that opens the media application.
- **Native Fluent Styling:** Seamlessly matches Windows 11 Fluent design, with optional system accent color tracking and zero clipping of the media card.
- **Multi-Session Support:** Seamlessly switches timeline and position when switching between active media apps using the Quick Settings arrows.
- **Zero XAML Diagnostics Conflicts:** Uses direct COM-interface symbol hooks in `ControlCenter.dll`, completely avoiding XAML Diagnostics. Fully compatible with Windows 11 Taskbar Styler, Notification Center Styler, and File Explorer Styler.
- **Auto-Restoring:** Cleanly restores the native media player card when the mod is disabled or unloaded.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- position: aboveButtons
  $name: Progress Bar Position
  $description: Where to place the progress bar relative to the playback control buttons.
  $options:
    - aboveButtons: Above playback buttons (Recommended, full width, centered)
    - belowButtons: Below playback buttons (Compact bottom position)
- verticalOffsetAbove: 33
  $name: Vertical Offset - Above Buttons (DIPs)
  $description: Distance of the seek bar above the bottom of the card when positioned Above Buttons. Default is 33.
- verticalOffsetBelow: -12
  $name: Vertical Offset - Below Buttons (DIPs)
  $description: Distance of the seek bar above the bottom of the card when positioned Below Buttons. Default is -12.
- showTimestamps: true
  $name: Show Timestamps
  $description: Show current time and total duration next to the progress bar.
- remainingTime: false
  $name: Show Remaining Time
  $description: Display remaining time (e.g. -2:45) instead of total duration on the right side.
- accentColorTrack: true
  $name: Use System Accent Color
  $description: Color the played portion of the progress bar with your Windows accent color.
- seekStepSeconds: 5
  $name: Mouse Wheel Seek Step (seconds)
  $description: Number of seconds to jump per mouse wheel scroll notch.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <unknwn.h>
#ifdef GetCurrentTime
#undef GetCurrentTime
#endif

#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <mutex>
#include <string>
#include <string_view>
#include <vector>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Documents.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.h>

namespace wux = winrt::Windows::UI::Xaml;
namespace wmc = winrt::Windows::Media::Control;

// Forward declarations
void SetupProgressBarUI(wux::FrameworkElement const& mtc);
void RestoreUI();
void InitSessionManager();
void RefreshCurrentMediaSession();
void UpdateProgressFromMedia(bool force = false);
void StartTimer();
void SeekTo(double positionSeconds);

enum class ProgressBarPosition {
    BelowButtons,
    AboveButtons,
};

struct Settings {
    ProgressBarPosition progressBarPosition{ProgressBarPosition::AboveButtons};
    int verticalOffsetAbove{33};
    int verticalOffsetBelow{-12};
    bool showTimestamps{true};
    bool remainingTime{false};
    bool accentColorTrack{true};
    int seekStepSeconds{5};
    bool autoRestartShellHost{true};
} g_settings;

void LoadSettings() {
    PCWSTR positionStr = Wh_GetStringSetting(L"position");
    if (positionStr && wcscmp(positionStr, L"belowButtons") == 0) {
        g_settings.progressBarPosition = ProgressBarPosition::BelowButtons;
    } else {
        g_settings.progressBarPosition = ProgressBarPosition::AboveButtons;
    }
    Wh_FreeStringSetting(positionStr);

    PCWSTR strAbove = Wh_GetStringSetting(L"verticalOffsetAbove");
    if (strAbove && *strAbove) {
        g_settings.verticalOffsetAbove = _wtoi(strAbove);
        Wh_FreeStringSetting(strAbove);
    } else {
        // Fallback for previous verticalOffset setting
        PCWSTR strLegacy = Wh_GetStringSetting(L"verticalOffset");
        if (strLegacy && *strLegacy) {
            g_settings.verticalOffsetAbove = _wtoi(strLegacy);
            Wh_FreeStringSetting(strLegacy);
        } else {
            g_settings.verticalOffsetAbove = 33;
        }
    }

    PCWSTR strBelow = Wh_GetStringSetting(L"verticalOffsetBelow");
    if (strBelow && *strBelow) {
        g_settings.verticalOffsetBelow = _wtoi(strBelow);
        Wh_FreeStringSetting(strBelow);
    } else {
        g_settings.verticalOffsetBelow = -12;
    }

    PCWSTR showTimestampsVal = Wh_GetStringSetting(L"showTimestamps");
    if (showTimestampsVal) {
        g_settings.showTimestamps = (wcscmp(showTimestampsVal, L"0") != 0 && _wcsicmp(showTimestampsVal, L"false") != 0);
        Wh_FreeStringSetting(showTimestampsVal);
    } else {
        g_settings.showTimestamps = true;
    }

    PCWSTR remainingTimeVal = Wh_GetStringSetting(L"remainingTime");
    if (remainingTimeVal) {
        g_settings.remainingTime = (wcscmp(remainingTimeVal, L"1") == 0 || _wcsicmp(remainingTimeVal, L"true") == 0);
        Wh_FreeStringSetting(remainingTimeVal);
    } else {
        g_settings.remainingTime = false;
    }

    PCWSTR accentVal = Wh_GetStringSetting(L"accentColorTrack");
    if (accentVal) {
        g_settings.accentColorTrack = (wcscmp(accentVal, L"0") != 0 && _wcsicmp(accentVal, L"false") != 0);
        Wh_FreeStringSetting(accentVal);
    } else {
        g_settings.accentColorTrack = true;
    }

    PCWSTR seekStepStr = Wh_GetStringSetting(L"seekStepSeconds");
    if (seekStepStr && *seekStepStr) {
        g_settings.seekStepSeconds = _wtoi(seekStepStr);
        if (g_settings.seekStepSeconds <= 0) g_settings.seekStepSeconds = 5;
        Wh_FreeStringSetting(seekStepStr);
    } else {
        g_settings.seekStepSeconds = 5;
    }

    PCWSTR autoRestartVal = Wh_GetStringSetting(L"autoRestartShellHost");
    if (autoRestartVal) {
        g_settings.autoRestartShellHost = (wcscmp(autoRestartVal, L"0") != 0 && _wcsicmp(autoRestartVal, L"false") != 0);
        Wh_FreeStringSetting(autoRestartVal);
    } else {
        g_settings.autoRestartShellHost = true;
    }
}

// Media Session and UI State
std::atomic<bool> g_unloading{false};
wmc::GlobalSystemMediaTransportControlsSessionManager g_sessionManager{nullptr};
wmc::GlobalSystemMediaTransportControlsSession g_currentSession{nullptr};
winrt::event_token g_timelineToken{};
winrt::event_token g_playbackToken{};
winrt::event_token g_sessionToken{};

wux::DispatcherTimer g_updateTimer{nullptr};
bool g_isUserScrubbing = false;
bool g_isInternalValueUpdate = false;
double g_currentDurationSeconds = 0.0;
double g_currentPositionSeconds = 0.0;
std::chrono::steady_clock::time_point g_lastSeekTime{};
double g_targetSeekPosition = -1.0;

winrt::weak_ref<wux::FrameworkElement> g_weakMtc{nullptr};
winrt::weak_ref<wux::Controls::Grid> g_attachedProgressGrid{nullptr};
winrt::weak_ref<wux::Controls::Slider> g_attachedSlider{nullptr};
winrt::weak_ref<wux::Controls::TextBlock> g_attachedElapsedText{nullptr};
winrt::weak_ref<wux::Controls::TextBlock> g_attachedTotalText{nullptr};
winrt::weak_ref<wux::FrameworkElement> g_weakRoot{nullptr};
winrt::Windows::System::DispatcherQueue g_uiDispatcherQueue{nullptr};

std::wstring FormatTime(double totalSeconds) {
    if (totalSeconds < 0.0) totalSeconds = 0.0;
    int secInt = static_cast<int>(std::round(totalSeconds));
    int hours = secInt / 3600;
    int mins = (secInt % 3600) / 60;
    int secs = secInt % 60;

    WCHAR buf[64];
    if (hours > 0) {
        wsprintfW(buf, L"%d:%02d:%02d", hours, mins, secs);
    } else {
        wsprintfW(buf, L"%d:%02d", mins, secs);
    }
    return std::wstring(buf);
}

void SeekTo(double positionSeconds) {
    if (g_unloading || !g_currentSession) {
        Wh_Log(L"SeekTo: session is null or mod unloading");
        return;
    }

    try {
        if (g_currentDurationSeconds > 0.0) {
            positionSeconds = std::clamp(positionSeconds, 0.0, g_currentDurationSeconds);
        } else if (positionSeconds < 0.0) {
            positionSeconds = 0.0;
        }

        int64_t ticks = static_cast<int64_t>(positionSeconds * 10'000'000.0);
        g_currentPositionSeconds = positionSeconds;
        g_lastSeekTime = std::chrono::steady_clock::now();
        g_targetSeekPosition = positionSeconds;

        Wh_Log(L"SeekTo: Requesting seek to %.2f seconds (%lld ticks)", positionSeconds, ticks);

        auto asyncOp = g_currentSession.TryChangePlaybackPositionAsync(ticks);
        asyncOp.Completed([](winrt::Windows::Foundation::IAsyncOperation<bool> const& op, winrt::Windows::Foundation::AsyncStatus status) {
            if (status == winrt::Windows::Foundation::AsyncStatus::Completed) {
                bool res = op.GetResults();
                Wh_Log(L"SeekTo: TryChangePlaybackPositionAsync result: %s", res ? L"TRUE (accepted)" : L"FALSE (rejected)");
            } else {
                Wh_Log(L"SeekTo: TryChangePlaybackPositionAsync status: %d", (int)status);
            }
        });
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"SeekTo: HRESULT error 0x%08X: %ls", (uint32_t)ex.code(), ex.message().c_str());
    } catch (...) {
        Wh_Log(L"SeekTo: TryChangePlaybackPositionAsync exception");
    }
}

void UpdateProgressFromMedia(bool force) {
    if (g_unloading || g_isUserScrubbing) return;

    auto slider = g_attachedSlider.get();
    auto elapsed = g_attachedElapsedText.get();
    auto total = g_attachedTotalText.get();
    if (!slider || !elapsed || !total) return;

    if (!g_currentSession) {
        g_isInternalValueUpdate = true;
        slider.Value(0);
        g_isInternalValueUpdate = false;
        elapsed.Text(L"0:00");
        total.Text(L"0:00");
        return;
    }

    try {
        auto timeline = g_currentSession.GetTimelineProperties();
        if (timeline) {
            auto endTicks = timeline.EndTime().count();
            auto posTicks = timeline.Position().count();
            auto lastUpdated = timeline.LastUpdatedTime();

            double durationSec = static_cast<double>(endTicks) / 10'000'000.0;
            double positionSec = static_cast<double>(posTicks) / 10'000'000.0;

            auto playbackInfo = g_currentSession.GetPlaybackInfo();
            if (playbackInfo &&
                playbackInfo.PlaybackStatus() == wmc::GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing) {
                auto now = winrt::clock::now();
                auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdated).count();
                if (diff > 0 && diff < 30000) {
                    positionSec += static_cast<double>(diff) / 1000.0;
                }
            }

            if (positionSec > durationSec) positionSec = durationSec;
            if (positionSec < 0.0) positionSec = 0.0;

            g_currentDurationSeconds = durationSec;

            // Post-seek grace period: protect the user-selected seek position from being overwritten
            // by stale SMTC timeline data while the media player processes the asynchronous seek request
            if (g_targetSeekPosition >= 0.0) {
                auto nowSteady = std::chrono::steady_clock::now();
                auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(nowSteady - g_lastSeekTime).count();
                if (elapsedMs < 1500) {
                    // If the player's timeline has updated and caught up to within 2 seconds of our target,
                    // release the grace period and resume normal live tracking
                    if (std::abs(positionSec - g_targetSeekPosition) < 2.0) {
                        g_targetSeekPosition = -1.0;
                    } else {
                        // Player hasn't caught up yet: keep user's seek position and skip overwrite
                        return;
                    }
                } else {
                    // Grace period expired (1.5 seconds)
                    g_targetSeekPosition = -1.0;
                }
            }

            g_currentPositionSeconds = positionSec;

            if (durationSec > 0.0) {
                slider.Maximum(durationSec);
                g_isInternalValueUpdate = true;
                slider.Value(positionSec);
                g_isInternalValueUpdate = false;

                elapsed.Text(FormatTime(positionSec));
                if (g_settings.remainingTime) {
                    double rem = (std::max)(0.0, durationSec - positionSec);
                    total.Text(L"-" + FormatTime(rem));
                } else {
                    total.Text(FormatTime(durationSec));
                }
            } else {
                g_isInternalValueUpdate = true;
                slider.Value(0);
                g_isInternalValueUpdate = false;
                elapsed.Text(L"0:00");
                total.Text(L"0:00");
            }
        }
    } catch (...) {
        Wh_Log(L"UpdateProgressFromMedia exception");
    }
}

void StartTimer() {
    if (g_unloading) return;
    if (!g_sessionManager) InitSessionManager();

    if (!g_updateTimer) {
        try {
            g_updateTimer = wux::DispatcherTimer();
            g_updateTimer.Interval(winrt::Windows::Foundation::TimeSpan{std::chrono::milliseconds(250)});
            g_updateTimer.Tick([](winrt::Windows::Foundation::IInspectable const&, winrt::Windows::Foundation::IInspectable const&) {
                UpdateProgressFromMedia(false);
            });
        } catch (...) {
            Wh_Log(L"Failed to create DispatcherTimer");
        }
    }

    if (g_updateTimer && !g_updateTimer.IsEnabled()) {
        g_updateTimer.Start();
    }
}

void RefreshCurrentMediaSession() {
    if (g_unloading) return;
    if (!g_sessionManager) {
        InitSessionManager();
        return;
    }

    try {
        auto session = g_sessionManager.GetCurrentSession();
        if (session != g_currentSession) {
            if (g_currentSession) {
                try {
                    if (g_timelineToken.value) g_currentSession.TimelinePropertiesChanged(g_timelineToken);
                    if (g_playbackToken.value) g_currentSession.PlaybackInfoChanged(g_playbackToken);
                } catch (...) {}
                g_timelineToken = {};
                g_playbackToken = {};
            }

            g_currentSession = session;

            if (g_currentSession) {
                g_timelineToken = g_currentSession.TimelinePropertiesChanged(
                    [](wmc::GlobalSystemMediaTransportControlsSession const&, wmc::TimelinePropertiesChangedEventArgs const&) {
                        if (g_uiDispatcherQueue) {
                            g_uiDispatcherQueue.TryEnqueue([]() {
                                UpdateProgressFromMedia(true);
                            });
                        }
                    });

                g_playbackToken = g_currentSession.PlaybackInfoChanged(
                    [](wmc::GlobalSystemMediaTransportControlsSession const&, wmc::PlaybackInfoChangedEventArgs const&) {
                        if (g_uiDispatcherQueue) {
                            g_uiDispatcherQueue.TryEnqueue([]() {
                                UpdateProgressFromMedia(true);
                            });
                        }
                    });

                if (g_uiDispatcherQueue) {
                    g_uiDispatcherQueue.TryEnqueue([]() {
                        UpdateProgressFromMedia(true);
                    });
                }
            }
        } else if (g_currentSession) {
            if (g_uiDispatcherQueue) {
                g_uiDispatcherQueue.TryEnqueue([]() {
                    UpdateProgressFromMedia(true);
                });
            }
        }
    } catch (...) {
        Wh_Log(L"RefreshCurrentMediaSession exception");
    }
}

static std::atomic<bool> g_sessionManagerInitializing{false};

void InitSessionManager() {
    if (g_unloading || g_sessionManager) return;

    bool expected = false;
    if (!g_sessionManagerInitializing.compare_exchange_strong(expected, true)) {
        return;
    }

    try {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
    } catch (...) {}

    try {
        auto asyncOp = wmc::GlobalSystemMediaTransportControlsSessionManager::RequestAsync();
        asyncOp.Completed([](auto const& asyncInfo, auto status) {
            g_sessionManagerInitializing = false;
            if (g_unloading) return;

            if (status == winrt::Windows::Foundation::AsyncStatus::Completed) {
                try {
                    g_sessionManager = asyncInfo.GetResults();
                    if (g_sessionManager) {
                        g_sessionToken = g_sessionManager.CurrentSessionChanged(
                            [](auto const&, auto const&) {
                                RefreshCurrentMediaSession();
                            });
                        Wh_Log(L"SMTC SessionManager successfully connected");
                        RefreshCurrentMediaSession();
                    }
                } catch (...) {
                    Wh_Log(L"GetResults exception in SessionManager");
                }
            } else {
                Wh_Log(L"SMTC SessionManager RequestAsync status not completed, will retry on demand");
            }
        });
    } catch (...) {
        g_sessionManagerInitializing = false;
        Wh_Log(L"InitSessionManager RequestAsync exception, will retry on demand");
    }
}

// Visual Tree Helpers
wux::FrameworkElement FindChildByName(wux::DependencyObject const& parent, std::wstring_view name) {
    if (!parent) return nullptr;

    int count = wux::Media::VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; ++i) {
        auto child = wux::Media::VisualTreeHelper::GetChild(parent, i);
        if (auto fe = child.try_as<wux::FrameworkElement>()) {
            if (fe.Name() == name) return fe;
        }
        auto found = FindChildByName(child, name);
        if (found) return found;
    }
    return nullptr;
}

wux::FrameworkElement FindChildByType(wux::DependencyObject const& parent, std::wstring_view typeSubstr) {
    if (!parent) return nullptr;

    int count = wux::Media::VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; ++i) {
        auto child = wux::Media::VisualTreeHelper::GetChild(parent, i);
        if (child) {
            std::wstring typeName{winrt::get_class_name(child)};
            if (typeName.find(typeSubstr) != std::wstring_view::npos) {
                if (auto fe = child.try_as<wux::FrameworkElement>()) return fe;
            }
        }
        auto found = FindChildByType(child, typeSubstr);
        if (found) return found;
    }
    return nullptr;
}

wux::Style FindStyleInResources(wux::FrameworkElement const& element, std::wstring_view key) {
    if (!element) return nullptr;
    auto boxedKey = winrt::box_value(key);

    // 1. Walk up the visual tree checking each ancestor's Resources()
    wux::FrameworkElement curr = element;
    while (curr) {
        try {
            if (auto res = curr.Resources()) {
                if (res.HasKey(boxedKey)) {
                    if (auto s = res.Lookup(boxedKey).try_as<wux::Style>()) {
                        return s;
                    }
                }
            }
        } catch (...) {}
        try {
            curr = wux::Media::VisualTreeHelper::GetParent(curr).try_as<wux::FrameworkElement>();
        } catch (...) {
            break;
        }
    }

    // 2. Check Application-level Resources()
    try {
        if (auto app = wux::Application::Current()) {
            if (auto res = app.Resources()) {
                if (res.HasKey(boxedKey)) {
                    if (auto s = res.Lookup(boxedKey).try_as<wux::Style>()) {
                        return s;
                    }
                }
            }
        }
    } catch (...) {}
    return nullptr;
}

wux::Controls::Slider FindNativeSlider(wux::DependencyObject const& parent) {
    if (!parent) return nullptr;
    int count = wux::Media::VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; ++i) {
        auto child = wux::Media::VisualTreeHelper::GetChild(parent, i);
        if (auto s = child.try_as<wux::Controls::Slider>()) {
            if (s.Name() != L"WindhawkMediaProgressSlider") {
                return s;
            }
        }
        auto found = FindNativeSlider(child);
        if (found) return found;
    }
    return nullptr;
}

void SetupProgressBarUI(wux::FrameworkElement const& mtc) {
    if (g_unloading || !mtc) return;
    if (!g_sessionManager) InitSessionManager();

    try {
        auto existing = FindChildByName(mtc, L"WindhawkMediaProgressBarGrid");
        if (existing) {
            RefreshCurrentMediaSession();
            StartTimer();
            return;
        }

        auto rootGrid = FindChildByName(mtc, L"MediaTransportControlsRoot").try_as<wux::Controls::Grid>();
        if (!rootGrid) {
            rootGrid = FindChildByType(mtc, L"MediaTransportControlsRoot").try_as<wux::Controls::Grid>();
        }
        if (!rootGrid) {
            rootGrid = mtc.try_as<wux::Controls::Grid>();
        }
        if (!rootGrid) {
            int childCount = wux::Media::VisualTreeHelper::GetChildrenCount(mtc);
            for (int i = 0; i < childCount; ++i) {
                if (auto g = wux::Media::VisualTreeHelper::GetChild(mtc, i).try_as<wux::Controls::Grid>()) {
                    rootGrid = g;
                    break;
                }
            }
        }

        if (!rootGrid) {
            if (g_uiDispatcherQueue) {
                g_uiDispatcherQueue.TryEnqueue(winrt::Windows::System::DispatcherQueuePriority::Low, [weak = winrt::make_weak(mtc)]() {
                    try {
                        if (auto m = weak.get()) {
                            SetupProgressBarUI(m);
                        }
                    } catch (...) {}
                });
            }
            return;
        }

        Wh_Log(L"SetupProgressBarUI creating controls in %s", rootGrid.Name().c_str());

        wux::Controls::Grid progressGrid;
        progressGrid.Name(L"WindhawkMediaProgressBarGrid");
        progressGrid.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
        progressGrid.Height(32);
        progressGrid.Visibility(wux::Visibility::Visible);
        // Transparent background enables pointer hit-testing and prevents click-through to card button
        progressGrid.Background(wux::Media::SolidColorBrush{winrt::Windows::UI::Colors::Transparent()});

        // Make progressGrid span all rows and columns so it overlays the entire card rather than just Row 0
        wux::Controls::Grid::SetRow(progressGrid, 0);
        wux::Controls::Grid::SetRowSpan(progressGrid, 99);
        wux::Controls::Grid::SetColumn(progressGrid, 0);
        wux::Controls::Grid::SetColumnSpan(progressGrid, 99);
        wux::Controls::Canvas::SetZIndex(progressGrid, 99);

        if (g_settings.progressBarPosition == ProgressBarPosition::AboveButtons) {
            progressGrid.VerticalAlignment(wux::VerticalAlignment::Bottom);
            // Positioned cleanly above buttons; spans full width symmetrically
            progressGrid.Margin(wux::Thickness{0, 0, 0, static_cast<double>(g_settings.verticalOffsetAbove)});
        } else {
            progressGrid.VerticalAlignment(wux::VerticalAlignment::Bottom);
            // Uses the natural empty space below the playback buttons with full symmetric width
            progressGrid.Margin(wux::Thickness{0, 0, 0, static_cast<double>(g_settings.verticalOffsetBelow)});
        }

        // Intercept clicks on the container background to protect the underlying card button
        progressGrid.PointerPressed([](auto const&, wux::Input::PointerRoutedEventArgs const& e) {
            e.Handled(true);
        });
        progressGrid.PointerReleased([](auto const&, wux::Input::PointerRoutedEventArgs const& e) {
            e.Handled(true);
        });

        wux::Controls::ColumnDefinition col0;
        col0.Width(wux::GridLength{0, wux::GridUnitType::Auto});
        wux::Controls::ColumnDefinition col1;
        col1.Width(wux::GridLength{1, wux::GridUnitType::Star});
        wux::Controls::ColumnDefinition col2;
        col2.Width(wux::GridLength{0, wux::GridUnitType::Auto});

        progressGrid.ColumnDefinitions().Append(col0);
        progressGrid.ColumnDefinitions().Append(col1);
        progressGrid.ColumnDefinitions().Append(col2);

        wux::Controls::TextBlock elapsedText;
        elapsedText.Name(L"WindhawkMediaElapsedText");
        elapsedText.Text(L"0:00");
        elapsedText.FontSize(11);
        elapsedText.VerticalAlignment(wux::VerticalAlignment::Center);
        elapsedText.HorizontalAlignment(wux::HorizontalAlignment::Left);
        elapsedText.TextAlignment(wux::TextAlignment::Left);
        // Enable OpenType Tabular Numerals (tnum) so all digits (including '1') have identical fixed widths,
        // preventing the progress bar from shifting or jittering horizontally every second as numbers change
        wux::Documents::Typography::SetNumeralAlignment(elapsedText, wux::FontNumeralAlignment::Tabular);
        // Top margin of 3.3 DIPs (~4 pixels) perfectly lowers the text baseline
        // so the progress bar track cuts directly through the vertical center of the digits.
        elapsedText.Margin(wux::Thickness{0, 3.3, 4, 0});
        elapsedText.Opacity(0.75);
        if (!g_settings.showTimestamps) {
            elapsedText.Visibility(wux::Visibility::Collapsed);
        }
        wux::Controls::Grid::SetColumn(elapsedText, 0);
        progressGrid.Children().Append(elapsedText);

        wux::Controls::Slider progressSlider;
        progressSlider.Name(L"WindhawkMediaProgressSlider");
        progressSlider.Minimum(0);
        progressSlider.Maximum(100);
        progressSlider.Value(0);
        progressSlider.MinHeight(32);
        progressSlider.Height(32);
        progressSlider.IsThumbToolTipEnabled(false);
        progressSlider.VerticalAlignment(wux::VerticalAlignment::Center);
        progressSlider.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
        progressSlider.Margin(wux::Thickness{0, 0, 0, 0});
        // 12 DIPs horizontal padding insets the track so the thumb (radius ~9-10 DIPs)
        // stays completely inside the slider's layout slot at 0:00 and at the end of the track,
        // eliminating edge clipping forever.
        progressSlider.Padding(wux::Thickness{12, 0, 12, 0});

        // Look up native Quick Settings volume slider style or MediaSliderStyle to ensure
        // compact thumb dimensions (~20 DIPs) and native track styling
        try {
            wux::Style resolvedStyle{nullptr};
            if (auto root = g_weakRoot.get()) {
                if (auto nativeSlider = FindNativeSlider(root)) {
                    if (auto style = nativeSlider.Style()) {
                        resolvedStyle = style;
                        Wh_Log(L"Found and applied native Quick Settings volume slider Style");
                    }
                }
            }
            if (!resolvedStyle) {
                resolvedStyle = FindStyleInResources(mtc, L"MediaSliderStyle");
                if (resolvedStyle) {
                    Wh_Log(L"Found and applied native MediaSliderStyle");
                }
            }
            if (resolvedStyle) {
                progressSlider.Style(resolvedStyle);
            } else {
                Wh_Log(L"Native slider style not found, using default style with custom sizing");
            }
        } catch (...) {
            Wh_Log(L"Exception resolving slider style");
        }

        // Re-apply padding and alignment to guarantee local property precedence
        progressSlider.Padding(wux::Thickness{12, 0, 12, 0});
        progressSlider.Height(32);
        progressSlider.MinHeight(32);
        progressSlider.VerticalAlignment(wux::VerticalAlignment::Center);

        // Hook Loaded to ensure template container clipping does not cut off thumb edges,
        // and match the thumb size to the native Quick Settings volume slider.
        progressSlider.Loaded([](winrt::Windows::Foundation::IInspectable const& sender, wux::RoutedEventArgs const&) {
            try {
                auto slider = sender.try_as<wux::Controls::Slider>();
                if (!slider) return;

                Wh_Log(L"progressSlider Loaded. Size: %.1fx%.1f", slider.ActualWidth(), slider.ActualHeight());

                // Find native Quick Settings slider thumb size if available
                double targetThumbWidth = 20.0;
                double targetThumbHeight = 20.0;
                if (auto root = g_weakRoot.get()) {
                    if (auto nativeSlider = FindNativeSlider(root)) {
                        if (auto nativeThumb = FindChildByName(nativeSlider, L"HorizontalThumb")) {
                            if (nativeThumb.ActualWidth() > 0 && nativeThumb.ActualHeight() > 0) {
                                targetThumbWidth = nativeThumb.ActualWidth();
                                targetThumbHeight = nativeThumb.ActualHeight();
                                Wh_Log(L"Found native slider thumb size: %.1fx%.1f", targetThumbWidth, targetThumbHeight);
                            }
                        }
                    }
                }

                // Adjust HorizontalThumb size so the outer halo perfectly matches Quick Settings,
                // and hook native Thumb DragStarted/DragCompleted for robust scrubbing
                if (auto thumb = FindChildByName(slider, L"HorizontalThumb").try_as<wux::Controls::Primitives::Thumb>()) {
                    thumb.Width(targetThumbWidth);
                    thumb.Height(targetThumbHeight);
                    thumb.VerticalAlignment(wux::VerticalAlignment::Center);
                    thumb.Margin(wux::Thickness{0, 0, 0, 0});
                    if (thumb.Clip()) {
                        thumb.Clip(nullptr);
                    }
                    Wh_Log(L"Applied thumb size %.1fx%.1f to HorizontalThumb", targetThumbWidth, targetThumbHeight);

                    thumb.DragStarted([](winrt::Windows::Foundation::IInspectable const&, wux::Controls::Primitives::DragStartedEventArgs const&) {
                        Wh_Log(L"HorizontalThumb: DragStarted (user scrubbing)");
                        g_isUserScrubbing = true;
                    });

                    thumb.DragCompleted([slider](winrt::Windows::Foundation::IInspectable const&, wux::Controls::Primitives::DragCompletedEventArgs const&) {
                        Wh_Log(L"HorizontalThumb: DragCompleted");
                        if (g_isUserScrubbing) {
                            g_isUserScrubbing = false;
                            double target = slider.Value();
                            Wh_Log(L"HorizontalThumb: Committing seek to %.2f seconds", target);
                            SeekTo(target);
                        }
                    });
                }

                // Collapse HeaderContentPresenter and tick bars so they don't offset the track downwards
                if (auto header = FindChildByName(slider, L"HeaderContentPresenter")) {
                    header.Visibility(wux::Visibility::Collapsed);
                    if (auto fe = header.try_as<wux::FrameworkElement>()) {
                        fe.Margin(wux::Thickness{0, 0, 0, 0});
                        fe.Height(0);
                        fe.MaxHeight(0);
                    }
                }
                if (auto topTick = FindChildByName(slider, L"TopTickBar")) {
                    topTick.Visibility(wux::Visibility::Collapsed);
                }
                if (auto bottomTick = FindChildByName(slider, L"BottomTickBar")) {
                    bottomTick.Visibility(wux::Visibility::Collapsed);
                }

                // Span HorizontalTemplate across all rows and center it symmetrically
                if (auto horiz = FindChildByName(slider, L"HorizontalTemplate")) {
                    wux::Controls::Grid::SetRow(horiz, 0);
                    wux::Controls::Grid::SetRowSpan(horiz, 99);
                    horiz.VerticalAlignment(wux::VerticalAlignment::Center);
                    horiz.Margin(wux::Thickness{0, 0, 0, 0});
                    if (horiz.Clip()) {
                        horiz.Clip(nullptr);
                        Wh_Log(L"Cleared Clip on HorizontalTemplate");
                    }
                }
                if (auto rootGrid = FindChildByName(slider, L"RootGrid")) {
                    rootGrid.VerticalAlignment(wux::VerticalAlignment::Center);
                    rootGrid.Margin(wux::Thickness{0, 0, 0, 0});
                    if (rootGrid.Clip()) {
                        rootGrid.Clip(nullptr);
                        Wh_Log(L"Cleared Clip on RootGrid");
                    }
                }
                if (slider.Clip()) {
                    slider.Clip(nullptr);
                    Wh_Log(L"Cleared Clip on progressSlider");
                }
            } catch (...) {}
        });

        if (g_settings.accentColorTrack) {
            try {
                winrt::Windows::UI::ViewManagement::UISettings uiSettings;
                winrt::Windows::UI::Color lightAccent;
                try {
                    // AccentLight2 is the vibrant lighter accent color used by native Windows 11 Quick Settings sliders
                    lightAccent = uiSettings.GetColorValue(winrt::Windows::UI::ViewManagement::UIColorType::AccentLight2);
                } catch (...) {
                    lightAccent = uiSettings.GetColorValue(winrt::Windows::UI::ViewManagement::UIColorType::Accent);
                }
                auto lightBrush = wux::Media::SolidColorBrush{lightAccent};
                progressSlider.Foreground(lightBrush);

                // Override Slider theme resources so the active track and thumb always use the vibrant
                // lighter accent color across all visual states (Normal, PointerOver, Pressed),
                // eliminating the darker unhovered state and perfectly matching Quick Settings sliders.
                auto res = progressSlider.Resources();
                res.Insert(winrt::box_value(L"SliderTrackValueFill"), lightBrush);
                res.Insert(winrt::box_value(L"SliderTrackValueFillPointerOver"), lightBrush);
                res.Insert(winrt::box_value(L"SliderTrackValueFillPressed"), lightBrush);
                res.Insert(winrt::box_value(L"SliderThumbBackground"), lightBrush);
                res.Insert(winrt::box_value(L"SliderThumbBackgroundPointerOver"), lightBrush);
                res.Insert(winrt::box_value(L"SliderThumbBackgroundPressed"), lightBrush);
            } catch (...) {}
        }

        wux::Controls::Grid::SetColumn(progressSlider, 1);
        progressGrid.Children().Append(progressSlider);

        wux::Controls::TextBlock totalText;
        totalText.Name(L"WindhawkMediaTotalText");
        totalText.Text(L"0:00");
        totalText.FontSize(11);
        totalText.VerticalAlignment(wux::VerticalAlignment::Center);
        totalText.HorizontalAlignment(wux::HorizontalAlignment::Right);
        totalText.TextAlignment(wux::TextAlignment::Right);
        // Enable OpenType Tabular Numerals (tnum) so all digits (including '1') have identical fixed widths,
        // preventing the progress bar from shifting or jittering horizontally every second as numbers change
        wux::Documents::Typography::SetNumeralAlignment(totalText, wux::FontNumeralAlignment::Tabular);
        // Top margin of 3.3 DIPs (~4 pixels) perfectly lowers the text baseline
        // so the progress bar track cuts directly through the vertical center of the digits.
        totalText.Margin(wux::Thickness{4, 3.3, 0, 0});
        totalText.Opacity(0.75);
        if (!g_settings.showTimestamps) {
            totalText.Visibility(wux::Visibility::Collapsed);
        }
        wux::Controls::Grid::SetColumn(totalText, 2);
        progressGrid.Children().Append(totalText);

    auto commitSeek = [progressSlider]() {
        if (g_isUserScrubbing) {
            g_isUserScrubbing = false;
            double target = progressSlider.Value();
            Wh_Log(L"commitSeek: Committing seek to %.2f seconds", target);
            SeekTo(target);
        }
    };

    // Listen to PointerPressed to detect scrubbing and track clicks.
    // CRITICAL: DO NOT mark e.Handled(true)! WinUI Slider and Thumb MUST receive PointerPressed
    // naturally to calculate click coordinates, update Value, and initiate dragging.
    auto pressedHandler = wux::Input::PointerEventHandler([](winrt::Windows::Foundation::IInspectable const&, wux::Input::PointerRoutedEventArgs const&) {
        Wh_Log(L"progressSlider: PointerPressed (interaction started)");
        g_isUserScrubbing = true;
    });

    auto releasedHandler = wux::Input::PointerEventHandler([commitSeek](winrt::Windows::Foundation::IInspectable const&, wux::Input::PointerRoutedEventArgs const&) {
        Wh_Log(L"progressSlider: PointerReleased");
        commitSeek();
    });

    auto captureLostHandler = wux::Input::PointerEventHandler([commitSeek](winrt::Windows::Foundation::IInspectable const&, wux::Input::PointerRoutedEventArgs const&) {
        Wh_Log(L"progressSlider: PointerCaptureLost");
        commitSeek();
    });

    try {
        progressSlider.AddHandler(
            wux::UIElement::PointerPressedEvent(),
            pressedHandler.as<winrt::Windows::Foundation::IInspectable>(),
            true);
        progressSlider.AddHandler(
            wux::UIElement::PointerReleasedEvent(),
            releasedHandler.as<winrt::Windows::Foundation::IInspectable>(),
            true);
        progressSlider.AddHandler(
            wux::UIElement::PointerCaptureLostEvent(),
            captureLostHandler.as<winrt::Windows::Foundation::IInspectable>(),
            true);
    } catch (...) {
        Wh_Log(L"progressSlider: AddHandler fallback");
        progressSlider.PointerPressed([](auto const&, wux::Input::PointerRoutedEventArgs const&) {
            g_isUserScrubbing = true;
        });
        progressSlider.PointerReleased([commitSeek](auto const&, wux::Input::PointerRoutedEventArgs const&) {
            commitSeek();
        });
        progressSlider.PointerCaptureLost([commitSeek](auto const&, wux::Input::PointerRoutedEventArgs const&) {
            commitSeek();
        });
    }

    progressSlider.ValueChanged([elapsedText, totalText](winrt::Windows::Foundation::IInspectable const&, wux::Controls::Primitives::RangeBaseValueChangedEventArgs const& e) {
        if (g_isInternalValueUpdate) return;

        double val = e.NewValue();
        elapsedText.Text(FormatTime(val));
        if (g_settings.remainingTime) {
            double rem = (std::max)(0.0, g_currentDurationSeconds - val);
            totalText.Text(L"-" + FormatTime(rem));
        } else {
            totalText.Text(FormatTime(g_currentDurationSeconds));
        }
    });

    // Mouse wheel seek support across both progressSlider and progressGrid
    auto wheelHandler = wux::Input::PointerEventHandler([progressSlider, elapsedText, totalText](winrt::Windows::Foundation::IInspectable const&, wux::Input::PointerRoutedEventArgs const& e) {
        if (e.Handled() || g_isUserScrubbing) return;
        e.Handled(true);

        auto pointerPoint = e.GetCurrentPoint(progressSlider);
        int delta = pointerPoint.Properties().MouseWheelDelta();
        if (delta != 0) {
            double total = g_currentDurationSeconds;
            if (total <= 0.0) return;

            double step = (delta > 0) ? g_settings.seekStepSeconds : -g_settings.seekStepSeconds;
            double current = (g_targetSeekPosition >= 0.0) ? g_targetSeekPosition : progressSlider.Value();
            double target = std::clamp(current + step, 0.0, total);

            g_isInternalValueUpdate = true;
            progressSlider.Value(target);
            g_isInternalValueUpdate = false;

            elapsedText.Text(FormatTime(target));
            if (g_settings.remainingTime) {
                double rem = (std::max)(0.0, total - target);
                totalText.Text(L"-" + FormatTime(rem));
            } else {
                totalText.Text(FormatTime(total));
            }

            Wh_Log(L"PointerWheelChanged: seek to %.2f (step %+.1f)", target, step);
            SeekTo(target);
        }
    });

    try {
        progressSlider.AddHandler(
            wux::UIElement::PointerWheelChangedEvent(),
            wheelHandler.as<winrt::Windows::Foundation::IInspectable>(),
            true);
        progressGrid.AddHandler(
            wux::UIElement::PointerWheelChangedEvent(),
            wheelHandler.as<winrt::Windows::Foundation::IInspectable>(),
            true);
    } catch (...) {
        progressSlider.PointerWheelChanged([wheelHandler](auto const& sender, auto const& e) {
            wheelHandler(sender, e);
        });
        progressGrid.PointerWheelChanged([wheelHandler](auto const& sender, auto const& e) {
            wheelHandler(sender, e);
        });
    }

    rootGrid.Children().Append(progressGrid);

    g_attachedProgressGrid = winrt::make_weak(progressGrid);
    g_attachedSlider = winrt::make_weak(progressSlider);
    g_attachedElapsedText = winrt::make_weak(elapsedText);
    g_attachedTotalText = winrt::make_weak(totalText);

    StartTimer();
    RefreshCurrentMediaSession();
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"SetupProgressBarUI HRESULT error: 0x%08X: %ls", (uint32_t)ex.code(), ex.message().c_str());
    } catch (...) {
        Wh_Log(L"SetupProgressBarUI unknown exception");
    }
}

void RestoreUI() {
    if (auto mtc = g_weakMtc.get()) {
        if (auto grid = g_attachedProgressGrid.get()) {
            if (auto rootGrid = FindChildByName(mtc, L"MediaTransportControlsRoot").try_as<wux::Controls::Grid>()) {
                uint32_t index = 0;
                if (rootGrid.Children().IndexOf(grid, index)) {
                    rootGrid.Children().RemoveAt(index);
                }
            } else if (auto root = mtc.try_as<wux::Controls::Grid>()) {
                uint32_t index = 0;
                if (root.Children().IndexOf(grid, index)) {
                    root.Children().RemoveAt(index);
                }
            }
        }
    }

    g_attachedProgressGrid = nullptr;
    g_attachedSlider = nullptr;
    g_attachedElapsedText = nullptr;
    g_attachedTotalText = nullptr;
    g_weakMtc = nullptr;
    g_weakRoot = nullptr;
}

// ControlCenter.dll Discovery Hooks (Zero XAML Diagnostics)
void OnControlCenterDiscovered(void* object) noexcept {
    if (!object || g_unloading) return;

    try {
        winrt::Windows::Foundation::IUnknown unk{nullptr};
        winrt::copy_from_abi(unk, object);
        if (!unk) return;

        auto element = unk.try_as<wux::FrameworkElement>();
        if (!element) return;

        if (!g_uiDispatcherQueue) {
            g_uiDispatcherQueue = winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
        }

        if (!g_sessionManager) {
            InitSessionManager();
        }

        auto attachMedia = [](wux::FrameworkElement const& container) {
            if (!container || g_unloading) return;
            auto mtc = FindChildByName(container, L"MediaTransportControls");
            if (!mtc) mtc = FindChildByType(container, L"MediaTransportControls");
            if (mtc) {
                g_weakMtc = winrt::make_weak(mtc);
                SetupProgressBarUI(mtc);
            }
        };

        // Walk to the top root of the visual tree
        wux::FrameworkElement root = element;
        if (auto xamlRoot = element.XamlRoot()) {
            if (auto content = xamlRoot.Content().try_as<wux::FrameworkElement>()) {
                root = content;
            }
        } else {
            for (int depth = 0; depth < 64; ++depth) {
                auto parent = wux::Media::VisualTreeHelper::GetParent(root).try_as<wux::FrameworkElement>();
                if (!parent) break;
                root = parent;
            }
        }

        g_weakRoot = winrt::make_weak(root);

        attachMedia(root);

        // Ensure attachment if MediaTransportControls is constructed with a brief delay
        if (g_uiDispatcherQueue) {
            g_uiDispatcherQueue.TryEnqueue(winrt::Windows::System::DispatcherQueuePriority::Low, [weakRoot = winrt::make_weak(root), attachMedia]() {
                if (auto r = weakRoot.get()) {
                    attachMedia(r);
                }
            });
        }
    } catch (...) {
        Wh_Log(L"OnControlCenterDiscovered exception");
    }
}

using Focus_t = int(WINAPI*)(void*, void*);
static Focus_t originalViewFocus = nullptr;
static Focus_t originalPageFocus = nullptr;

static int WINAPI ViewFocus_Hook(void* self, void* args) {
    int res = originalViewFocus ? originalViewFocus(self, args) : 0;
    OnControlCenterDiscovered(self);
    return res;
}

static int WINAPI PageFocus_Hook(void* self, void* args) {
    int res = originalPageFocus ? originalPageFocus(self, args) : 0;
    OnControlCenterDiscovered(self);
    return res;
}

static std::atomic<int> g_discoveryState{0}; // 0 = waiting, 1 = installing, 2 = ready, 3 = failed
static std::mutex g_discoveryInstallMutex;
static HANDLE g_hWatchdogThread = nullptr;

static bool HookControlCenterInternal(HMODULE controlCenter, bool applyNow) {
    std::lock_guard installLock(g_discoveryInstallMutex);
    if (g_unloading) return false;

    WindhawkUtils::SYMBOL_HOOK controlCenterDllHooks[] = {
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::ControlCenter::implementation::ControlCenterView,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnGotFocus(void *))"},
         &originalViewFocus,
         ViewFocus_Hook},
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::ControlCenter::implementation::FullScreenPage,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnGotFocus(void *))"},
         &originalPageFocus,
         PageFocus_Hook,
         true},
    };

    bool ok = WindhawkUtils::HookSymbols(controlCenter, controlCenterDllHooks,
                                         ARRAYSIZE(controlCenterDllHooks));
    if (ok && applyNow) {
        ok = Wh_ApplyHookOperations();
    }
    g_discoveryState = ok ? (applyNow ? 2 : 1) : 3;
    Wh_Log(L"ControlCenter.dll discovery hooks: %s", ok ? L"ready" : L"failed");
    return ok;
}

static bool HookControlCenter(HMODULE controlCenter, bool applyNow) {
    int expected = 0;
    if (!g_discoveryState.compare_exchange_strong(expected, 1)) {
        return expected != 3;
    }
    return HookControlCenterInternal(controlCenter, applyNow);
}

// Watchdog thread: polls until ControlCenter.dll is loaded, with zero early timeout
static DWORD WINAPI WatchdogThreadProc(LPVOID) {
    while (!g_unloading && g_discoveryState != 2) {
        Sleep(200);
        if (g_unloading) break;

        if (auto controlCenter = GetModuleHandleW(L"ControlCenter.dll")) {
            int expected = 0;
            if (g_discoveryState.compare_exchange_strong(expected, 1)) {
                Wh_Log(L"ControlCenter.dll discovered by watchdog thread");
                HookControlCenterInternal(controlCenter, true);
                break;
            } else if (g_discoveryState == 2) {
                break;
            }
        }
    }
    return 0;
}

// Process Helpers
static bool IsShellProcess() {
    WCHAR modulePath[MAX_PATH];
    if (GetModuleFileNameW(NULL, modulePath, ARRAYSIZE(modulePath))) {
        PCWSTR exeName = wcsrchr(modulePath, L'\\');
        if (exeName) exeName++;
        else exeName = modulePath;

        return (_wcsicmp(exeName, L"ShellHost.exe") == 0 ||
                _wcsicmp(exeName, L"ShellExperienceHost.exe") == 0);
    }
    return false;
}

static bool IsShellHost() {
    WCHAR modulePath[MAX_PATH];
    if (GetModuleFileNameW(NULL, modulePath, ARRAYSIZE(modulePath))) {
        PCWSTR exeName = wcsrchr(modulePath, L'\\');
        if (exeName) exeName++;
        else exeName = modulePath;

        return (_wcsicmp(exeName, L"ShellHost.exe") == 0);
    }
    return false;
}

// Mod Lifecycle
BOOL Wh_ModInit() {
    Wh_Log(L"Quick Settings Media Progress Bar: Wh_ModInit v1.2.12");

    if (!IsShellProcess()) {
        Wh_Log(L"Not ShellHost or ShellExperienceHost, skipping mod init");
        return FALSE;
    }

    LoadSettings();

    if (auto controlCenter = GetModuleHandleW(L"ControlCenter.dll")) {
        HookControlCenter(controlCenter, false);
    } else {
        Wh_Log(L"Waiting for ControlCenter.dll to load...");
        g_hWatchdogThread = CreateThread(nullptr, 0, WatchdogThreadProc, nullptr, 0, nullptr);
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"Wh_ModAfterInit");
    if (!g_unloading) {
        if (g_discoveryState == 1) {
            bool ok = Wh_ApplyHookOperations();
            g_discoveryState = ok ? 2 : 3;
            Wh_Log(L"ControlCenter.dll discovery hooks applied post-init: %s", ok ? L"ready" : L"failed");
        }
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed");
    LoadSettings();

    // Auto-restart ShellHost.exe if enabled to guarantee a 100% clean state
    if (g_settings.autoRestartShellHost && IsShellHost()) {
        Wh_Log(L"Auto-restarting ShellHost.exe on settings change");
        TerminateProcess(GetCurrentProcess(), 0);
        return;
    }

    if (g_uiDispatcherQueue) {
        g_uiDispatcherQueue.TryEnqueue([]() {
            if (auto mtc = g_weakMtc.get()) {
                RestoreUI();
                SetupProgressBarUI(mtc);
            }
        });
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"BeforeUninit");
    g_unloading = true;

    if (g_hWatchdogThread) {
        WaitForSingleObject(g_hWatchdogThread, 500);
        CloseHandle(g_hWatchdogThread);
        g_hWatchdogThread = nullptr;
    }

    if (g_updateTimer) {
        try {
            g_updateTimer.Stop();
        } catch (...) {}
        g_updateTimer = nullptr;
    }

    if (g_currentSession) {
        try {
            if (g_timelineToken.value) g_currentSession.TimelinePropertiesChanged(g_timelineToken);
            if (g_playbackToken.value) g_currentSession.PlaybackInfoChanged(g_playbackToken);
        } catch (...) {}
        g_timelineToken = {};
        g_playbackToken = {};
        g_currentSession = nullptr;
    }

    if (g_sessionManager) {
        try {
            if (g_sessionToken.value) {
                g_sessionManager.CurrentSessionChanged(g_sessionToken);
            }
        } catch (...) {}
        g_sessionToken = {};
        g_sessionManager = nullptr;
    }

    if (g_uiDispatcherQueue) {
        HANDLE hDone = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        bool enqueued = g_uiDispatcherQueue.TryEnqueue([hDone]() {
            try {
                RestoreUI();
            } catch (...) {}
            if (hDone) SetEvent(hDone);
        });
        if (enqueued && hDone) {
            WaitForSingleObject(hDone, 1000);
        }
        if (hDone) CloseHandle(hDone);
    }

    // Auto-restart ShellHost.exe if enabled so Quick Settings is always clean and responsive without needing Task Manager
    if (g_settings.autoRestartShellHost && IsShellHost()) {
        Wh_Log(L"Auto-restarting ShellHost.exe on uninit");
        TerminateProcess(GetCurrentProcess(), 0);
    }
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}
