// ==WindhawkMod==
// @id              mutealert
// @name            MuteAlert - Microphone Activity Taskbar Widget
// @description     Shows live microphone activity, call mute state, volume controls, and headset mute synchronization in the Windows 11 taskbar.
// @version         0.9.1
// @author          Nikolay
// @github          https://github.com/Nikolay1243
// @homepage        https://github.com/MuteAlert/windhawk
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lcomctl32 -luuid -lsetupapi -lhid
// @license         MIT
// ==/WindhawkMod==

// Licensed under the MIT License.

// ==WindhawkModReadme==
/*
# MuteAlert for Windhawk

Adds a microphone button to the Windows 11 system tray area.

* The microphone fills from bottom to top as the default input device detects
  sound.
* Hover to see the default microphone name, input volume, mute state, and
  available mouse controls.
* Scroll over the button to change the input volume.
* Optionally lock the microphone at a chosen volume. Scrolling updates the
  locked target when this is enabled.
* Left-click the button to mute or unmute the microphone.
* A secondary Slack, Teams, or Zoom logo shows the active call microphone
  state and focuses the call window when left-clicked.
* Headset mute synchronization supports Windows hardware mute reporting,
  standard USB HID mute controls, and vendor-specific adapters. SteelSeries
  Arctis Nova Pro Wireless is the first high-confidence vendor adapter.
* If you speak while muted in a Slack huddle, Microsoft Teams call, or Zoom
  meeting, the widget warns you and can play an optional audio cue.
* Right-click the widget to mute or unmute active call apps when that action
  is shown in the tooltip.

The widget follows the default Windows capture endpoint. Changing the default
input device in Windows automatically moves the widget to the new device.

Headset detection reports one of four methods in the hover tooltip: Windows
hardware mute, Standard HID mute button, SteelSeries device state, or
Unsupported/no observable state. Standard HID buttons are events and do not
prove the position of a latched switch. Purely mechanical disconnect switches
cannot be observed, and a zero audio level is never interpreted as mute.

For unsupported headsets, set **Export sanitized headset diagnostics** to a
full `.txt` path and apply Settings. The report includes device IDs, HID
descriptors, provider slots, and changed byte offsets, but excludes device
paths, serial numbers, and raw report values.

Windows 11 is required. The initial release supports x64 Explorer.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- position: beforeClock
  $name: Position
  $description: Where to place the microphone widget in the system tray.
  $options:
  - beforeIcons: Before notification icons
  - beforeOmni: Before Wi-Fi, volume, and battery
  - beforeClock: Before the clock
  - afterClock: After the clock
  - afterShowDesktop: After the Show Desktop strip
- deviceRole: console
  $name: Default microphone role
  $description: Which Windows default capture endpoint the widget follows.
  $options:
  - console: General/default input
  - communications: Communications input
  - multimedia: Multimedia input
- volumeStep: 2
  $name: Volume change per wheel notch (%)
  $description: Scroll up to increase and scroll down to decrease the microphone volume.
- forceVolume: false
  $name: Keep microphone at the selected volume
  $description: Restores the target input volume if Windows or another application changes it. Scrolling the widget changes the target.
- forcedVolume: 100
  $name: Locked microphone volume (%)
  $description: "Target volume while volume lock is enabled. Range: 0-100%."
- updateInterval: 50
  $name: Meter update interval (ms)
  $description: "Lower values make the meter smoother but wake Explorer more often. Range: 25-500 ms."
- peakSensitivity: 150
  $name: Meter sensitivity (%)
  $description: Scales the displayed peak without changing the microphone volume. 100% is the raw Windows peak value.
- iconSize: 18
  $name: Microphone icon size (px)
- buttonWidth: 32
  $name: Widget width (px)
- showCallStateIcon: true
  $name: Show active-call app icon
  $description: Shows a secondary Slack, Teams, or Zoom logo while a supported call is active. Left-clicking it focuses the call window.
- headsetSyncMode: full
  $name: Headset mute synchronization
  $description: Uses Windows hardware mute, standard HID mute controls, or a supported vendor adapter. The taskbar tooltip shows the current detection method and confidence. Silence is never interpreted as physical mute.
  $options:
  - full: Sync physical mute and unmute changes
  - muteOnly: Sync only physical mute changes
  - statusOnly: Show the physical state without changing software mute states
  - off: Disabled
- headsetSyncWindows: true
  $name: Synchronize headset mute with Windows input
- headsetSyncCalls: true
  $name: Synchronize headset mute with active calls
- headsetPollInterval: 500
  $name: Headset status interval (ms)
  $description: "How often to query vendor device state. Range: 200-2000 ms."
- headsetDiagnosticsPath: ""
  $name: Export sanitized headset diagnostics
  $description: Optional full .txt path. Applying settings exports device IDs, HID descriptors, provider slots, and sanitized report changes without paths, serial numbers, or raw report values.
- slackWarning: true
  $name: Warn when speaking while Slack is muted
  $description: Uses Windows UI Automation to detect a visible muted Slack huddle. No Slack credentials or network access are used.
- slackAudioCue: true
  $name: Play Slack muted audio cue
  $description: Plays the Windows exclamation sound once when the speaking-while-muted warning begins.
- slackRightClickUnmute: true
  $name: Right-click to toggle Slack microphone
  $description: Invokes Slack's accessible Mute or Unmute button when the taskbar microphone is right-clicked during a huddle.
- slackMutedButtonText: "unmute"
  $name: Slack muted-button text
  $description: Case-insensitive text expected in Slack's button while your huddle microphone is muted. Change this for a localized Slack interface.
- slackCallButtonText: "leave"
  $name: Slack in-call button text
  $description: Case-insensitive text expected in Slack's Leave huddle button. Change this for a localized Slack interface.
- slackSpeechThreshold: 8
  $name: Slack warning speech threshold (%)
  $description: Minimum displayed microphone peak considered speech. Raise this if background noise triggers warnings.
- slackSpeechDelay: 500
  $name: Slack warning delay (ms)
  $description: "How long sound must remain above the threshold before showing the warning. Range: 100-3000 ms."
- teamsWarning: true
  $name: Warn when speaking while Microsoft Teams is muted
  $description: Uses Windows UI Automation to detect a visible muted Teams call. No Teams credentials or network access are used.
- teamsAudioCue: true
  $name: Play Teams muted audio cue
  $description: Plays the Windows exclamation sound once when the Teams speaking-while-muted warning begins.
- teamsRightClickUnmute: true
  $name: Right-click to toggle Teams microphone
  $description: Invokes Teams' accessible Mute or Unmute button when the taskbar microphone is right-clicked during a call.
- teamsMutedButtonText: "unmute"
  $name: Teams muted-button text
  $description: Case-insensitive text expected in Teams' button while your call microphone is muted. Change this for a localized Teams interface.
- teamsCallButtonText: "hang up|leave"
  $name: Teams in-call button text
  $description: Case-insensitive text expected in Teams' Hang up or Leave button. Separate alternatives with a vertical bar.
- teamsSpeechThreshold: 8
  $name: Teams warning speech threshold (%)
  $description: Minimum displayed microphone peak considered speech. Raise this if background noise triggers warnings.
- teamsSpeechDelay: 500
  $name: Teams warning delay (ms)
  $description: "How long sound must remain above the threshold before showing the warning. Range: 100-3000 ms."
- zoomWarning: true
  $name: Warn when speaking while Zoom is muted
  $description: Uses Windows UI Automation to detect a visible muted Zoom meeting. No Zoom credentials or network access are used.
- zoomAudioCue: true
  $name: Play Zoom muted audio cue
  $description: Plays the Windows exclamation sound once when the Zoom speaking-while-muted warning begins.
- zoomRightClickUnmute: true
  $name: Right-click to toggle Zoom microphone
  $description: Invokes Zoom's accessible Mute or Unmute button when the taskbar microphone is right-clicked during a meeting.
- zoomMutedButtonText: "unmute"
  $name: Zoom muted-button text
  $description: Case-insensitive text expected in Zoom's button while your meeting microphone is muted. Change this for a localized Zoom interface.
- zoomCallButtonText: "leave|end"
  $name: Zoom in-meeting button text
  $description: Case-insensitive text expected in Zoom's Leave or End meeting button. Separate alternatives with a vertical bar.
- zoomSpeechThreshold: 8
  $name: Zoom warning speech threshold (%)
  $description: Minimum displayed microphone peak considered speech. Raise this if background noise triggers warnings.
- zoomSpeechDelay: 500
  $name: Zoom warning delay (ms)
  $description: "How long sound must remain above the threshold before showing the warning. Range: 100-3000 ms."
*/
// ==/WindhawkModSettings==

#undef GetCurrentTime

#include <windows.h>
#include <commctrl.h>
#include <endpointvolume.h>
#include <functiondiscoverykeys_devpkey.h>
#include <hidsdi.h>
#include <hidpi.h>
#include <mmdeviceapi.h>
#include <propsys.h>
#include <setupapi.h>
#include <tlhelp32.h>
#include <UIAutomation.h>

// MinGW's endpointvolume.h currently only forward-declares this Windows Core
// Audio interface. Keep the SDK-compatible declaration local to the mod.
#ifndef __IAudioMeterInformation_INTERFACE_DEFINED__
#define __IAudioMeterInformation_INTERFACE_DEFINED__
MIDL_INTERFACE("c02216f6-8c67-4b5b-9d00-d008e73e0064")
IAudioMeterInformation : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetPeakValue(float* peak) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMeteringChannelCount(
        UINT* channelCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetChannelsPeakValues(
        UINT channelCount,
        float* peakValues) = 0;
    virtual HRESULT STDMETHODCALLTYPE QueryHardwareSupport(DWORD* mask) = 0;
};
#ifdef __CRT_UUID_DECL
__CRT_UUID_DECL(IAudioMeterInformation, 0xc02216f6, 0x8c67, 0x4b5b, 0x9d,
                0x00, 0xd0, 0x08, 0xe7, 0x3e, 0x00, 0x64)
#endif
#endif

#include <windhawk_utils.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cwctype>
#include <functional>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Input;
using namespace winrt::Windows::UI::Xaml::Media;

namespace Shapes = winrt::Windows::UI::Xaml::Shapes;

enum class HeadsetDetectionMethod {
    Unsupported,
    WindowsHardwareMute,
    StandardHidButton,
    SteelSeriesDeviceState,
};

enum class HeadsetConfidence { None, Medium, High };

struct HeadsetStatus {
    bool detected = false;
    bool stateKnown = false;
    bool muted = false;
    HeadsetDetectionMethod method = HeadsetDetectionMethod::Unsupported;
    HeadsetConfidence confidence = HeadsetConfidence::None;
    std::wstring deviceName;
    std::wstring detail;
};

struct HeadsetAdapterObservation {
    bool available = false;
    bool stateKnown = false;
    bool muted = false;
    std::wstring deviceName;
    std::wstring detail;
};

class IHeadsetMuteAdapter {
public:
    virtual ~IHeadsetMuteAdapter() = default;
    virtual PCWSTR Id() const = 0;
    virtual PCWSTR DisplayName() const = 0;
    virtual bool TryConnect() = 0;
    virtual bool Poll(HeadsetAdapterObservation& observation) = 0;
    virtual void Disconnect() = 0;
};

struct VendorAdapterSlot {
    USHORT vendorId;
    PCWSTR vendorName;
    PCWSTR adapterId;
    bool implemented;
};

static const std::vector<VendorAdapterSlot>& GetVendorAdapterSlots() {
    static const std::vector<VendorAdapterSlot> slots = {
        {0x1038, L"SteelSeries", L"steelseries-nova-pro", true},
        {0x046D, L"Logitech", L"logitech", false},
        {0x0B0E, L"Jabra", L"jabra", false},
        {0x047F, L"Poly / Plantronics", L"poly", false},
        {0x1B1C, L"Corsair", L"corsair", false},
    };
    return slots;
}

static const VendorAdapterSlot* FindVendorAdapterSlot(USHORT vendorId) {
    for (const auto& slot : GetVendorAdapterSlots()) {
        if (slot.vendorId == vendorId) return &slot;
    }
    return nullptr;
}

// -----------------------------------------------------------------------------
// Settings
// -----------------------------------------------------------------------------

struct ModSettings {
    std::wstring position = L"beforeClock";
    ERole deviceRole = eConsole;
    int volumeStep = 2;
    bool forceVolume = false;
    int forcedVolume = 100;
    int updateInterval = 50;
    int peakSensitivity = 150;
    int iconSize = 18;
    int buttonWidth = 32;
    bool showCallStateIcon = true;
    std::wstring headsetSyncMode = L"full";
    bool headsetSyncWindows = true;
    bool headsetSyncCalls = true;
    int headsetPollInterval = 500;
    std::wstring headsetDiagnosticsPath;
    bool slackWarning = true;
    bool slackAudioCue = true;
    bool slackRightClickToggle = true;
    std::wstring slackMutedButtonText = L"unmute";
    std::wstring slackCallButtonText = L"leave";
    int slackSpeechThreshold = 8;
    int slackSpeechDelay = 500;
    bool teamsWarning = true;
    bool teamsAudioCue = true;
    bool teamsRightClickToggle = true;
    std::wstring teamsMutedButtonText = L"unmute";
    std::wstring teamsCallButtonText = L"hang up|leave";
    int teamsSpeechThreshold = 8;
    int teamsSpeechDelay = 500;
    bool zoomWarning = true;
    bool zoomAudioCue = true;
    bool zoomRightClickToggle = true;
    std::wstring zoomMutedButtonText = L"unmute";
    std::wstring zoomCallButtonText = L"leave|end";
    int zoomSpeechThreshold = 8;
    int zoomSpeechDelay = 500;
};

static ModSettings g_settings;
static std::atomic<int> g_audioRole{eConsole};
static std::atomic<int> g_updateInterval{50};
static std::atomic<int> g_peakSensitivity{150};
static std::atomic<bool> g_forceVolume{false};
static std::atomic<int> g_forcedVolume{100};

static std::wstring GetStringSetting(PCWSTR name) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result = value ? value : L"";
    Wh_FreeStringSetting(value);
    return result;
}

static void LoadSettings() {
    g_settings.position = GetStringSetting(L"position");
    if (g_settings.position.empty()) {
        g_settings.position = L"beforeClock";
    }

    std::wstring role = GetStringSetting(L"deviceRole");
    if (role == L"communications") {
        g_settings.deviceRole = eCommunications;
    } else if (role == L"multimedia") {
        g_settings.deviceRole = eMultimedia;
    } else {
        g_settings.deviceRole = eConsole;
    }

    g_settings.volumeStep =
        std::clamp(Wh_GetIntSetting(L"volumeStep"), 1, 20);
    g_settings.forceVolume = Wh_GetIntSetting(L"forceVolume") != 0;
    int configuredForcedVolume =
        std::clamp(Wh_GetIntSetting(L"forcedVolume"), 0, 100);
    int previousConfiguredVolume =
        Wh_GetIntValue(L"forcedVolumeSettingBaseline", -1);
    if (previousConfiguredVolume != configuredForcedVolume) {
        Wh_SetIntValue(L"forcedVolumeSettingBaseline", configuredForcedVolume);
        Wh_SetIntValue(L"forcedVolumeTarget", configuredForcedVolume);
    }
    g_settings.forcedVolume = std::clamp(
        Wh_GetIntValue(L"forcedVolumeTarget", configuredForcedVolume), 0, 100);
    g_settings.updateInterval =
        std::clamp(Wh_GetIntSetting(L"updateInterval"), 25, 500);
    g_settings.peakSensitivity =
        std::clamp(Wh_GetIntSetting(L"peakSensitivity"), 25, 500);
    g_settings.iconSize =
        std::clamp(Wh_GetIntSetting(L"iconSize"), 12, 32);
    g_settings.buttonWidth =
        std::clamp(Wh_GetIntSetting(L"buttonWidth"), 20, 64);
    g_settings.showCallStateIcon =
        Wh_GetIntSetting(L"showCallStateIcon") != 0;
    g_settings.headsetSyncMode = GetStringSetting(L"headsetSyncMode");
    if (g_settings.headsetSyncMode.empty()) {
        g_settings.headsetSyncMode = L"full";
    }
    g_settings.headsetSyncWindows =
        Wh_GetIntSetting(L"headsetSyncWindows") != 0;
    g_settings.headsetSyncCalls =
        Wh_GetIntSetting(L"headsetSyncCalls") != 0;
    g_settings.headsetPollInterval =
        std::clamp(Wh_GetIntSetting(L"headsetPollInterval"), 200, 2000);
    g_settings.headsetDiagnosticsPath =
        GetStringSetting(L"headsetDiagnosticsPath");
    g_settings.slackWarning = Wh_GetIntSetting(L"slackWarning") != 0;
    g_settings.slackAudioCue = Wh_GetIntSetting(L"slackAudioCue") != 0;
    g_settings.slackRightClickToggle =
        Wh_GetIntSetting(L"slackRightClickUnmute") != 0;
    g_settings.slackMutedButtonText =
        GetStringSetting(L"slackMutedButtonText");
    g_settings.slackCallButtonText =
        GetStringSetting(L"slackCallButtonText");
    if (g_settings.slackMutedButtonText.empty()) {
        g_settings.slackMutedButtonText = L"unmute";
    }
    if (g_settings.slackCallButtonText.empty()) {
        g_settings.slackCallButtonText = L"leave";
    }
    g_settings.slackSpeechThreshold =
        std::clamp(Wh_GetIntSetting(L"slackSpeechThreshold"), 1, 100);
    g_settings.slackSpeechDelay =
        std::clamp(Wh_GetIntSetting(L"slackSpeechDelay"), 100, 3000);
    g_settings.teamsWarning = Wh_GetIntSetting(L"teamsWarning") != 0;
    g_settings.teamsAudioCue = Wh_GetIntSetting(L"teamsAudioCue") != 0;
    g_settings.teamsRightClickToggle =
        Wh_GetIntSetting(L"teamsRightClickUnmute") != 0;
    g_settings.teamsMutedButtonText =
        GetStringSetting(L"teamsMutedButtonText");
    g_settings.teamsCallButtonText =
        GetStringSetting(L"teamsCallButtonText");
    if (g_settings.teamsMutedButtonText.empty()) {
        g_settings.teamsMutedButtonText = L"unmute";
    }
    if (g_settings.teamsCallButtonText.empty()) {
        g_settings.teamsCallButtonText = L"hang up|leave";
    }
    g_settings.teamsSpeechThreshold =
        std::clamp(Wh_GetIntSetting(L"teamsSpeechThreshold"), 1, 100);
    g_settings.teamsSpeechDelay =
        std::clamp(Wh_GetIntSetting(L"teamsSpeechDelay"), 100, 3000);
    g_settings.zoomWarning = Wh_GetIntSetting(L"zoomWarning") != 0;
    g_settings.zoomAudioCue = Wh_GetIntSetting(L"zoomAudioCue") != 0;
    g_settings.zoomRightClickToggle =
        Wh_GetIntSetting(L"zoomRightClickUnmute") != 0;
    g_settings.zoomMutedButtonText =
        GetStringSetting(L"zoomMutedButtonText");
    g_settings.zoomCallButtonText =
        GetStringSetting(L"zoomCallButtonText");
    if (g_settings.zoomMutedButtonText.empty()) {
        g_settings.zoomMutedButtonText = L"unmute";
    }
    if (g_settings.zoomCallButtonText.empty()) {
        g_settings.zoomCallButtonText = L"leave|end";
    }
    g_settings.zoomSpeechThreshold =
        std::clamp(Wh_GetIntSetting(L"zoomSpeechThreshold"), 1, 100);
    g_settings.zoomSpeechDelay =
        std::clamp(Wh_GetIntSetting(L"zoomSpeechDelay"), 100, 3000);

    g_audioRole.store(static_cast<int>(g_settings.deviceRole));
    g_updateInterval.store(g_settings.updateInterval);
    g_peakSensitivity.store(g_settings.peakSensitivity);
    g_forceVolume.store(g_settings.forceVolume);
    g_forcedVolume.store(g_settings.forcedVolume);
}

// -----------------------------------------------------------------------------
// Shared audio snapshot and commands
// -----------------------------------------------------------------------------

static std::atomic<bool> g_unloading{false};
static std::atomic<bool> g_audioAvailable{false};
static std::atomic<bool> g_audioMuted{false};
static std::atomic<int> g_audioVolume{0};
static std::atomic<float> g_audioPeak{0.0f};
static std::atomic<float> g_audioLinearPeak{0.0f};
static std::atomic<int> g_pendingVolumeNotches{0};
static std::atomic<int> g_pendingVolumeSet{-1};
static std::atomic<unsigned int> g_pendingMuteToggles{0};
static std::atomic<int> g_pendingMuteSet{-1};
static std::atomic<int> g_pendingSlackCommand{-1};
static std::atomic<int> g_pendingTeamsCommand{-1};
static std::atomic<int> g_pendingZoomCommand{-1};
static std::atomic<bool> g_slackCallActive{false};
static std::atomic<bool> g_slackMuted{false};
static std::atomic<bool> g_slackWarningActive{false};
static std::atomic<HWND> g_slackCallWindow{nullptr};
static std::atomic<bool> g_teamsCallActive{false};
static std::atomic<bool> g_teamsMuted{false};
static std::atomic<bool> g_teamsWarningActive{false};
static std::atomic<HWND> g_teamsCallWindow{nullptr};
static std::atomic<bool> g_zoomCallActive{false};
static std::atomic<bool> g_zoomMuted{false};
static std::atomic<bool> g_zoomStateKnown{false};
static std::atomic<bool> g_zoomWarningActive{false};
static std::atomic<HWND> g_zoomCallWindow{nullptr};
static std::atomic<bool> g_headsetAvailable{false};
static std::atomic<bool> g_headsetMuted{false};
static SRWLOCK g_headsetStatusLock = SRWLOCK_INIT;
static HeadsetStatus g_headsetStatus;
static bool g_windowsHardwareMuteSupported = false;
static bool g_windowsHardwareMuted = false;
static std::wstring g_windowsHardwareDevice;
static bool g_standardHidDetected = false;
static std::wstring g_standardHidDevice;
static std::wstring g_standardHidDetail;
static bool g_steelSeriesDetected = false;
static bool g_steelSeriesMuted = false;
static std::wstring g_steelSeriesDevice;
static std::wstring g_steelSeriesDetail;
static SRWLOCK g_diagnosticLock = SRWLOCK_INIT;
static std::vector<std::wstring> g_diagnosticEvents;
static ULONGLONG g_diagnosticStartTime = 0;
static HWND g_headsetMessageWindow = nullptr;
static HINSTANCE g_headsetWindowInstance = nullptr;
static constexpr wchar_t kHeadsetWindowClass[] =
    L"MuteAlert.Windhawk.HeadsetInput";
static SRWLOCK g_audioNameLock = SRWLOCK_INIT;
static std::wstring g_audioDeviceName = L"No microphone available";

static HANDLE g_audioThread = nullptr;
static HANDLE g_callAppsThread = nullptr;
static HANDLE g_headsetThread = nullptr;
static HANDLE g_audioStopEvent = nullptr;
static HANDLE g_audioWakeEvent = nullptr;

static std::atomic<HWND> g_mainTaskbarWnd{nullptr};

static UINT AudioUpdateMessage() {
    static const UINT message = RegisterWindowMessageW(
        L"Windhawk_MicrophoneActivityUpdate_" WH_MOD_ID);
    return message;
}

static void SetSharedDeviceName(std::wstring name) {
    AcquireSRWLockExclusive(&g_audioNameLock);
    g_audioDeviceName = std::move(name);
    ReleaseSRWLockExclusive(&g_audioNameLock);
}

static std::wstring GetSharedDeviceName() {
    AcquireSRWLockShared(&g_audioNameLock);
    std::wstring name = g_audioDeviceName;
    ReleaseSRWLockShared(&g_audioNameLock);
    return name;
}

static void NotifyTaskbar() {
    if (HWND hWnd = g_mainTaskbarWnd.load()) {
        PostMessageW(hWnd, AudioUpdateMessage(), 0, 0);
    }
}

static PCWSTR HeadsetMethodName(HeadsetDetectionMethod method) {
    switch (method) {
        case HeadsetDetectionMethod::WindowsHardwareMute:
            return L"Windows hardware mute";
        case HeadsetDetectionMethod::StandardHidButton:
            return L"Standard HID mute button";
        case HeadsetDetectionMethod::SteelSeriesDeviceState:
            return L"SteelSeries device state";
        default:
            return L"Unsupported/no observable state";
    }
}

static PCWSTR HeadsetConfidenceName(HeadsetConfidence confidence) {
    switch (confidence) {
        case HeadsetConfidence::High:
            return L"High - latched mute state is observable";
        case HeadsetConfidence::Medium:
            return L"Medium - button events are observable";
        default:
            return L"None - no physical mute signal is observable";
    }
}

static HeadsetStatus GetHeadsetStatus() {
    AcquireSRWLockShared(&g_headsetStatusLock);
    HeadsetStatus status = g_headsetStatus;
    ReleaseSRWLockShared(&g_headsetStatusLock);
    return status;
}

static bool SameHeadsetStatus(const HeadsetStatus& left,
                              const HeadsetStatus& right) {
    return left.detected == right.detected &&
           left.stateKnown == right.stateKnown && left.muted == right.muted &&
           left.method == right.method && left.confidence == right.confidence &&
           left.deviceName == right.deviceName && left.detail == right.detail;
}

static void RecomputeHeadsetStatus() {
    bool changed = false;
    AcquireSRWLockExclusive(&g_headsetStatusLock);
    HeadsetStatus next;
    if (g_steelSeriesDetected) {
        next.detected = true;
        next.stateKnown = true;
        next.muted = g_steelSeriesMuted;
        next.method = HeadsetDetectionMethod::SteelSeriesDeviceState;
        next.confidence = HeadsetConfidence::High;
        next.deviceName = g_steelSeriesDevice;
        next.detail = g_steelSeriesDetail;
    } else if (g_standardHidDetected) {
        next.detected = true;
        next.stateKnown = false;
        next.method = HeadsetDetectionMethod::StandardHidButton;
        next.confidence = HeadsetConfidence::Medium;
        next.deviceName = g_standardHidDevice;
        next.detail = g_standardHidDetail;
    } else if (g_windowsHardwareMuteSupported) {
        next.detected = true;
        next.stateKnown = true;
        next.muted = g_windowsHardwareMuted;
        next.method = HeadsetDetectionMethod::WindowsHardwareMute;
        next.confidence = HeadsetConfidence::High;
        next.deviceName = g_windowsHardwareDevice;
        next.detail = L"The active capture driver exposes hardware mute.";
    } else {
        next.detail =
            L"Silence is never treated as proof that a headset is muted.";
    }
    changed = !SameHeadsetStatus(g_headsetStatus, next);
    g_headsetStatus = std::move(next);
    g_headsetAvailable.store(g_headsetStatus.detected &&
                             g_headsetStatus.stateKnown);
    g_headsetMuted.store(g_headsetStatus.stateKnown &&
                         g_headsetStatus.muted);
    ReleaseSRWLockExclusive(&g_headsetStatusLock);
    if (changed) NotifyTaskbar();
}

static void UpdateWindowsHardwareSource(bool supported, bool muted,
                                        const std::wstring& deviceName) {
    AcquireSRWLockExclusive(&g_headsetStatusLock);
    g_windowsHardwareMuteSupported = supported;
    g_windowsHardwareMuted = muted;
    g_windowsHardwareDevice = supported ? deviceName : L"";
    ReleaseSRWLockExclusive(&g_headsetStatusLock);
    RecomputeHeadsetStatus();
}

static void UpdateStandardHidSource(bool detected,
                                    const std::wstring& deviceName,
                                    const std::wstring& detail) {
    AcquireSRWLockExclusive(&g_headsetStatusLock);
    g_standardHidDetected = detected;
    g_standardHidDevice = detected ? deviceName : L"";
    g_standardHidDetail = detected ? detail : L"";
    ReleaseSRWLockExclusive(&g_headsetStatusLock);
    RecomputeHeadsetStatus();
}

static void UpdateSteelSeriesSource(bool detected, bool muted,
                                    const std::wstring& deviceName,
                                    const std::wstring& detail) {
    AcquireSRWLockExclusive(&g_headsetStatusLock);
    g_steelSeriesDetected = detected;
    g_steelSeriesMuted = muted;
    g_steelSeriesDevice = detected ? deviceName : L"";
    g_steelSeriesDetail = detected ? detail : L"";
    ReleaseSRWLockExclusive(&g_headsetStatusLock);
    RecomputeHeadsetStatus();
}

static void RecordDiagnosticEvent(const std::wstring& event) {
    ULONGLONG elapsed = GetTickCount64() - g_diagnosticStartTime;
    std::wstring line = L"+" + std::to_wstring(elapsed) + L" ms: " + event;
    AcquireSRWLockExclusive(&g_diagnosticLock);
    if (g_diagnosticEvents.size() >= 256) {
        g_diagnosticEvents.erase(g_diagnosticEvents.begin(),
                                 g_diagnosticEvents.begin() + 64);
    }
    g_diagnosticEvents.push_back(std::move(line));
    ReleaseSRWLockExclusive(&g_diagnosticLock);
}

static void QueueVolumeNotches(int notches) {
    if (notches == 0) {
        return;
    }

    if (g_forceVolume.load()) {
        int target = std::clamp(
            g_forcedVolume.load() + notches * g_settings.volumeStep, 0, 100);
        g_forcedVolume.store(target);
        g_pendingVolumeSet.store(target);
        Wh_SetIntValue(L"forcedVolumeTarget", target);
    } else {
        g_pendingVolumeNotches.fetch_add(notches);
    }
    if (g_audioWakeEvent) {
        SetEvent(g_audioWakeEvent);
    }
}

static void QueueMuteToggle() {
    g_pendingMuteToggles.fetch_add(1);
    if (g_audioWakeEvent) {
        SetEvent(g_audioWakeEvent);
    }
}

static void QueueMuteSet(bool muted) {
    g_pendingMuteSet.store(muted ? 1 : 0);
    if (g_audioWakeEvent) SetEvent(g_audioWakeEvent);
}

static constexpr int kCallCommandNone = -1;
static constexpr int kCallCommandUnmute = 0;
static constexpr int kCallCommandMute = 1;
static constexpr int kCallCommandToggle = 2;

static void QueueSlackToggle() {
    g_pendingSlackCommand.store(kCallCommandToggle);
}

static void QueueTeamsToggle() {
    g_pendingTeamsCommand.store(kCallCommandToggle);
}

static void QueueZoomToggle() {
    g_pendingZoomCommand.store(kCallCommandToggle);
}

static void QueueActiveCallMuteState(bool muted) {
    int command = muted ? kCallCommandMute : kCallCommandUnmute;
    if (g_slackCallActive.load() && g_slackMuted.load() != muted) {
        g_pendingSlackCommand.store(command);
    }
    if (g_teamsCallActive.load() && g_teamsMuted.load() != muted) {
        g_pendingTeamsCommand.store(command);
    }
    if (g_zoomCallActive.load() && g_zoomMuted.load() != muted) {
        g_pendingZoomCommand.store(command);
    }
}

static bool QueueActiveCallToggles() {
    bool queued = false;
    if (g_settings.slackRightClickToggle && g_slackCallActive.load()) {
        QueueSlackToggle();
        queued = true;
    }
    if (g_settings.teamsRightClickToggle && g_teamsCallActive.load()) {
        QueueTeamsToggle();
        queued = true;
    }
    if (g_settings.zoomRightClickToggle && g_zoomCallActive.load() &&
        g_zoomStateKnown.load()) {
        QueueZoomToggle();
        queued = true;
    }
    return queued;
}

static int GetSelectedActiveCallIndex() {
    bool slackActive = g_slackCallActive.load();
    bool teamsActive = g_teamsCallActive.load();
    bool zoomActive = g_zoomCallActive.load();
    if (slackActive && g_slackMuted.load()) return 0;
    if (teamsActive && g_teamsMuted.load()) return 1;
    if (zoomActive && g_zoomMuted.load()) return 2;
    if (slackActive) return 0;
    if (teamsActive) return 1;
    if (zoomActive) return 2;
    return -1;
}

static bool FocusSelectedCallWindow() {
    int selected = GetSelectedActiveCallIndex();
    HWND callWindow = selected == 0   ? g_slackCallWindow.load()
                      : selected == 1 ? g_teamsCallWindow.load()
                      : selected == 2 ? g_zoomCallWindow.load()
                                      : nullptr;
    if (!callWindow || !IsWindow(callWindow)) return false;

    if (IsIconic(callWindow)) {
        ShowWindowAsync(callWindow, SW_RESTORE);
    } else if (!IsWindowVisible(callWindow)) {
        ShowWindowAsync(callWindow, SW_SHOW);
    }
    BringWindowToTop(callWindow);
    bool focused = SetForegroundWindow(callWindow) != FALSE;
    Wh_Log(L"[Call apps] Focus %s for window %p",
           focused ? L"requested" : L"was denied by Windows", callWindow);
    return true;
}

struct AudioEndpoint {
    winrt::com_ptr<IMMDevice> device;
    winrt::com_ptr<IAudioMeterInformation> meter;
    winrt::com_ptr<IAudioEndpointVolume> volume;
    std::wstring id;
    std::wstring name;
    bool hardwareMute = false;

    void Reset() {
        volume = nullptr;
        meter = nullptr;
        device = nullptr;
        id.clear();
        name.clear();
        hardwareMute = false;
    }
};

static std::wstring ReadDeviceName(IMMDevice* device) {
    winrt::com_ptr<IPropertyStore> properties;
    if (FAILED(device->OpenPropertyStore(STGM_READ, properties.put()))) {
        return L"Microphone";
    }

    PROPVARIANT value;
    PropVariantInit(&value);
    std::wstring result = L"Microphone";
    if (SUCCEEDED(properties->GetValue(PKEY_Device_FriendlyName, &value)) &&
        value.vt == VT_LPWSTR && value.pwszVal) {
        result = value.pwszVal;
    }
    PropVariantClear(&value);
    return result;
}

static bool OpenDefaultEndpoint(IMMDeviceEnumerator* enumerator,
                                AudioEndpoint* endpoint) {
    winrt::com_ptr<IMMDevice> device;
    ERole role = static_cast<ERole>(g_audioRole.load());
    if (FAILED(enumerator->GetDefaultAudioEndpoint(eCapture, role,
                                                   device.put()))) {
        endpoint->Reset();
        return false;
    }

    LPWSTR rawId = nullptr;
    if (FAILED(device->GetId(&rawId)) || !rawId) {
        endpoint->Reset();
        return false;
    }
    std::wstring id = rawId;
    CoTaskMemFree(rawId);

    if (endpoint->device && endpoint->id == id) {
        return true;
    }

    AudioEndpoint replacement;
    replacement.device = std::move(device);
    replacement.id = std::move(id);
    replacement.name = ReadDeviceName(replacement.device.get());

    if (FAILED(replacement.device->Activate(
            __uuidof(IAudioMeterInformation), CLSCTX_ALL, nullptr,
            replacement.meter.put_void())) ||
        FAILED(replacement.device->Activate(
            __uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr,
            replacement.volume.put_void()))) {
        endpoint->Reset();
        return false;
    }

    DWORD hardwareSupport = 0;
    replacement.hardwareMute =
        SUCCEEDED(replacement.volume->QueryHardwareSupport(
            &hardwareSupport)) &&
        (hardwareSupport & ENDPOINT_HARDWARE_SUPPORT_MUTE) != 0;

    *endpoint = std::move(replacement);
    Wh_Log(L"[Audio] Following: %s (hardware mute: %s)",
           endpoint->name.c_str(), endpoint->hardwareMute ? L"yes" : L"no");
    return true;
}

static void PublishUnavailableAudio() {
    g_audioAvailable.store(false);
    g_audioMuted.store(false);
    g_audioVolume.store(0);
    g_audioPeak.store(0.0f);
    g_audioLinearPeak.store(0.0f);
    g_slackWarningActive.store(false);
    g_teamsWarningActive.store(false);
    g_zoomWarningActive.store(false);
    SetSharedDeviceName(L"No microphone available");
    UpdateWindowsHardwareSource(false, false, L"");
    NotifyTaskbar();
}

enum class CallApp { Slack, Teams, Zoom };
enum class CallState { NotInCall, Unknown, Unmuted, Muted };

static std::wstring Lowercase(std::wstring text) {
    std::transform(text.begin(), text.end(), text.begin(), [](wchar_t value) {
        return static_cast<wchar_t>(std::towlower(value));
    });
    return text;
}

static PCWSTR CallAppName(CallApp app) {
    switch (app) {
        case CallApp::Slack:
            return L"Slack";
        case CallApp::Teams:
            return L"Microsoft Teams";
        case CallApp::Zoom:
            return L"Zoom";
    }
    return L"Call app";
}

static bool IsCallAppWindow(HWND hWnd, CallApp app) {
    if (!IsWindowVisible(hWnd)) return false;

    DWORD processId = 0;
    GetWindowThreadProcessId(hWnd, &processId);
    if (!processId) return false;

    HANDLE process =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (!process) return false;

    WCHAR path[32768];
    DWORD pathLength = ARRAYSIZE(path);
    bool matches = false;
    if (QueryFullProcessImageNameW(process, 0, path, &pathLength)) {
        const wchar_t* fileName = path;
        for (DWORD i = 0; i < pathLength; i++) {
            if (path[i] == L'\\' || path[i] == L'/') {
                fileName = path + i + 1;
            }
        }
        switch (app) {
            case CallApp::Slack:
                matches = _wcsicmp(fileName, L"slack.exe") == 0;
                break;
            case CallApp::Teams:
                matches = _wcsicmp(fileName, L"ms-teams.exe") == 0 ||
                          _wcsicmp(fileName, L"teams.exe") == 0;
                break;
            case CallApp::Zoom:
                matches = _wcsicmp(fileName, L"zoom.exe") == 0 ||
                          _wcsicmp(fileName, L"cpthost.exe") == 0;
                break;
        }
    }
    CloseHandle(process);
    return matches;
}

static bool IsZoomMeetingHostRunning() {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return false;

    bool found = false;
    PROCESSENTRY32W entry{};
    entry.dwSize = sizeof(entry);
    if (Process32FirstW(snapshot, &entry)) {
        do {
            if (_wcsicmp(entry.szExeFile, L"cpthost.exe") != 0) continue;

            HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,
                                         FALSE, entry.th32ProcessID);
            if (!process) continue;
            WCHAR path[32768];
            DWORD length = ARRAYSIZE(path);
            if (QueryFullProcessImageNameW(process, 0, path, &length)) {
                std::wstring lowered = Lowercase(std::wstring(path, length));
                if (lowered.find(L"\\zoom\\") != std::wstring::npos) {
                    found = true;
                }
            }
            CloseHandle(process);
            if (found) break;
        } while (Process32NextW(snapshot, &entry));
    }
    CloseHandle(snapshot);
    return found;
}

static bool RequestForegroundWindow(HWND hWnd) {
    hWnd = GetAncestor(hWnd, GA_ROOT);
    if (!hWnd || !IsWindow(hWnd)) return false;

    DWORD currentThread = GetCurrentThreadId();
    DWORD targetThread = GetWindowThreadProcessId(hWnd, nullptr);
    HWND previousForeground = GetForegroundWindow();
    DWORD foregroundThread = previousForeground
                                 ? GetWindowThreadProcessId(previousForeground,
                                                            nullptr)
                                 : 0;
    bool attachedTarget =
        targetThread && targetThread != currentThread &&
        AttachThreadInput(currentThread, targetThread, TRUE);
    bool attachedForeground =
        foregroundThread && foregroundThread != currentThread &&
        foregroundThread != targetThread &&
        AttachThreadInput(currentThread, foregroundThread, TRUE);

    if (IsIconic(hWnd)) ShowWindowAsync(hWnd, SW_RESTORE);
    BringWindowToTop(hWnd);
    SetForegroundWindow(hWnd);
    SetFocus(hWnd);

    HWND actualForeground = GetForegroundWindow();
    DWORD requestedProcess = 0;
    DWORD actualProcess = 0;
    GetWindowThreadProcessId(hWnd, &requestedProcess);
    if (actualForeground) {
        GetWindowThreadProcessId(actualForeground, &actualProcess);
    }

    if (attachedForeground) {
        AttachThreadInput(currentThread, foregroundThread, FALSE);
    }
    if (attachedTarget) {
        AttachThreadInput(currentThread, targetThread, FALSE);
    }
    return requestedProcess && requestedProcess == actualProcess;
}

static bool SendZoomMuteShortcut(HWND callWindow) {
    if (!callWindow || !IsWindow(callWindow) ||
        !IsCallAppWindow(callWindow, CallApp::Zoom)) {
        return false;
    }

    HWND zoomWindow = GetAncestor(callWindow, GA_ROOT);
    HWND previousForeground = GetForegroundWindow();
    if (!RequestForegroundWindow(zoomWindow)) {
        Wh_Log(L"[Zoom] Couldn't focus the meeting window for Alt+A");
        return false;
    }

    Sleep(20);
    INPUT input[4]{};
    input[0].type = INPUT_KEYBOARD;
    input[0].ki.wVk = VK_MENU;
    input[1].type = INPUT_KEYBOARD;
    input[1].ki.wVk = L'A';
    input[2].type = INPUT_KEYBOARD;
    input[2].ki.wVk = L'A';
    input[2].ki.dwFlags = KEYEVENTF_KEYUP;
    input[3].type = INPUT_KEYBOARD;
    input[3].ki.wVk = VK_MENU;
    input[3].ki.dwFlags = KEYEVENTF_KEYUP;
    UINT sent = SendInput(ARRAYSIZE(input), input, sizeof(INPUT));
    Sleep(40);

    if (previousForeground && previousForeground != zoomWindow &&
        IsWindow(previousForeground)) {
        RequestForegroundWindow(previousForeground);
    }

    if (sent != ARRAYSIZE(input)) {
        Wh_Log(L"[Zoom] Alt+A input failed after %u of %u events: %u", sent,
               static_cast<UINT>(ARRAYSIZE(input)), GetLastError());
        return false;
    }
    Wh_Log(L"[Zoom] Alt+A invoked while the meeting controls were hidden");
    return true;
}

static CallState ApplyCallCommandToState(CallState state, int command) {
    if (command == kCallCommandMute) return CallState::Muted;
    if (command == kCallCommandUnmute) return CallState::Unmuted;
    if (command == kCallCommandToggle) {
        if (state == CallState::Muted) return CallState::Unmuted;
        if (state == CallState::Unmuted) return CallState::Muted;
    }
    return state;
}

static bool ContainsConfiguredText(const std::wstring& name,
                                   const std::wstring& configuredText) {
    size_t start = 0;
    while (start <= configuredText.size()) {
        size_t end = configuredText.find(L'|', start);
        std::wstring token = configuredText.substr(
            start, end == std::wstring::npos ? end : end - start);
        size_t first = token.find_first_not_of(L" \t");
        size_t last = token.find_last_not_of(L" \t");
        if (first != std::wstring::npos) {
            token = token.substr(first, last - first + 1);
            size_t match = 0;
            while (!token.empty() &&
                   (match = name.find(token, match)) != std::wstring::npos) {
                bool startsAtBoundary =
                    match == 0 || !std::iswalnum(name[match - 1]);
                size_t after = match + token.size();
                bool endsAtBoundary =
                    after == name.size() || !std::iswalnum(name[after]);
                if (startsAtBoundary && endsAtBoundary) return true;
                match++;
            }
        }
        if (end == std::wstring::npos) break;
        start = end + 1;
    }
    return false;
}

struct CallWindowSearch {
    CallApp app;
    std::vector<HWND>* windows;
};

static CallState ReadCallState(IUIAutomation* automation, CallApp app,
                               int command, HWND* activeWindow) {
    if (activeWindow) *activeWindow = nullptr;
    if (!automation) return CallState::NotInCall;

    std::vector<HWND> appWindows;
    CallWindowSearch search{app, &appWindows};
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto* search = reinterpret_cast<CallWindowSearch*>(lParam);
            if (IsCallAppWindow(hWnd, search->app)) {
                search->windows->push_back(hWnd);
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&search));

    if (appWindows.empty()) return CallState::NotInCall;

    // Zoom's meeting toolbar can disappear from the automation tree while it
    // is auto-hidden. Keep a usable window target for the app badge even when
    // the mute button itself isn't currently exposed.
    if (activeWindow && app == CallApp::Zoom) {
        *activeWindow = appWindows.front();
    }

    VARIANT buttonType;
    VariantInit(&buttonType);
    buttonType.vt = VT_I4;
    buttonType.lVal = UIA_ButtonControlTypeId;
    winrt::com_ptr<IUIAutomationCondition> buttonCondition;
    if (FAILED(automation->CreatePropertyCondition(
            UIA_ControlTypePropertyId, buttonType, buttonCondition.put()))) {
        return CallState::NotInCall;
    }

    const std::wstring* mutedButtonText = nullptr;
    const std::wstring* callButtonText = nullptr;
    switch (app) {
        case CallApp::Slack:
            mutedButtonText = &g_settings.slackMutedButtonText;
            callButtonText = &g_settings.slackCallButtonText;
            break;
        case CallApp::Teams:
            mutedButtonText = &g_settings.teamsMutedButtonText;
            callButtonText = &g_settings.teamsCallButtonText;
            break;
        case CallApp::Zoom:
            mutedButtonText = &g_settings.zoomMutedButtonText;
            callButtonText = &g_settings.zoomCallButtonText;
            break;
    }
    std::wstring mutedText = Lowercase(*mutedButtonText);
    std::wstring callText = Lowercase(*callButtonText);

    for (HWND hWnd : appWindows) {
        winrt::com_ptr<IUIAutomationElement> root;
        if (FAILED(automation->ElementFromHandle(hWnd, root.put())) || !root) {
            continue;
        }

        winrt::com_ptr<IUIAutomationElementArray> buttons;
        if (FAILED(root->FindAll(TreeScope_Descendants, buttonCondition.get(),
                                 buttons.put())) ||
            !buttons) {
            continue;
        }

        bool hasCallMarker = false;
        bool hasMutedAction = false;
        bool hasUnmutedAction = false;
        winrt::com_ptr<IUIAutomationElement> mutedButton;
        winrt::com_ptr<IUIAutomationElement> unmutedButton;
        int count = 0;
        buttons->get_Length(&count);
        for (int i = 0; i < count; i++) {
            winrt::com_ptr<IUIAutomationElement> button;
            if (FAILED(buttons->GetElement(i, button.put())) || !button) {
                continue;
            }

            BOOL offscreen = TRUE;
            if (FAILED(button->get_CurrentIsOffscreen(&offscreen)) ||
                (offscreen && app != CallApp::Zoom)) {
                continue;
            }

            BSTR rawName = nullptr;
            if (FAILED(button->get_CurrentName(&rawName)) || !rawName) {
                continue;
            }
            std::wstring name = Lowercase(rawName);
            SysFreeString(rawName);

            if (ContainsConfiguredText(name, callText)) {
                hasCallMarker = true;
            }
            if (ContainsConfiguredText(name, mutedText)) {
                hasMutedAction = true;
                mutedButton.copy_from(button.get());
            } else if (ContainsConfiguredText(name, L"mute")) {
                hasUnmutedAction = true;
                unmutedButton.copy_from(button.get());
            }
        }

        if (hasCallMarker && (hasMutedAction || hasUnmutedAction)) {
            if (activeWindow) *activeWindow = hWnd;
            CallState currentState = hasMutedAction ? CallState::Muted
                                                    : CallState::Unmuted;
            auto actionButton = hasMutedAction ? mutedButton : unmutedButton;
            PCWSTR actionName = hasMutedAction ? L"Unmute" : L"Mute";
            bool shouldInvoke =
                command == kCallCommandToggle ||
                (command == kCallCommandMute &&
                 currentState == CallState::Unmuted) ||
                (command == kCallCommandUnmute &&
                 currentState == CallState::Muted);
            if (shouldInvoke && actionButton) {
                winrt::com_ptr<IUIAutomationInvokePattern> invokePattern;
                HRESULT patternResult = actionButton->GetCurrentPatternAs(
                    UIA_InvokePatternId, IID_IUIAutomationInvokePattern,
                    invokePattern.put_void());
                HRESULT actionResult = patternResult;
                if (SUCCEEDED(patternResult) && invokePattern) {
                    actionResult = invokePattern->Invoke();
                    if (SUCCEEDED(actionResult)) {
                        Wh_Log(L"[%s] %s invoked from taskbar widget",
                               CallAppName(app), actionName);
                        return currentState == CallState::Muted
                                   ? CallState::Unmuted
                                   : CallState::Muted;
                    }
                }
                Wh_Log(L"[%s] Accessible %s action failed: 0x%08X",
                       CallAppName(app), actionName, actionResult);
            }
            return currentState;
        }
    }

    return CallState::NotInCall;
}

static DWORD WINAPI CallAppsThreadProc(void*) {
    HRESULT coInit = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(coInit)) {
        Wh_Log(L"[Call apps] COM initialization failed: 0x%08X", coInit);
        return 0;
    }

    winrt::com_ptr<IUIAutomation> automation;
    HRESULT automationResult = CoCreateInstance(
        CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER,
        IID_IUIAutomation, automation.put_void());
    if (FAILED(automationResult)) {
        Wh_Log(L"[Call apps] UI Automation unavailable: 0x%08X",
               automationResult);
        CoUninitialize();
        return 0;
    }

    ULONGLONG lastSlackCheck = 0;
    ULONGLONG lastTeamsCheck = 0;
    ULONGLONG lastZoomCheck = 0;
    ULONGLONG slackSpeakingSince = 0;
    ULONGLONG slackWarningUntil = 0;
    ULONGLONG teamsSpeakingSince = 0;
    ULONGLONG teamsWarningUntil = 0;
    ULONGLONG zoomSpeakingSince = 0;
    ULONGLONG zoomWarningUntil = 0;
    CallState slackState = CallState::NotInCall;
    CallState teamsState = CallState::NotInCall;
    CallState zoomState = CallState::NotInCall;
    int deferredZoomCommand = kCallCommandNone;
    bool syncCallsFromHeadset =
        g_settings.headsetSyncCalls &&
        (g_settings.headsetSyncMode == L"full" ||
         g_settings.headsetSyncMode == L"muteOnly");
    bool monitorSlack =
        g_settings.showCallStateIcon || g_settings.slackWarning ||
        g_settings.slackRightClickToggle || syncCallsFromHeadset;
    bool monitorTeams =
        g_settings.showCallStateIcon || g_settings.teamsWarning ||
        g_settings.teamsRightClickToggle || syncCallsFromHeadset;
    bool monitorZoom =
        g_settings.showCallStateIcon || g_settings.zoomWarning ||
        g_settings.zoomRightClickToggle || syncCallsFromHeadset;

    while (WaitForSingleObject(g_audioStopEvent, 50) == WAIT_TIMEOUT &&
           !g_unloading.load()) {
        ULONGLONG now = GetTickCount64();
        int slackCommand =
            g_pendingSlackCommand.exchange(kCallCommandNone);
        if (lastSlackCheck == 0 || now - lastSlackCheck >= 750 ||
            slackCommand != kCallCommandNone) {
            HWND callWindow = nullptr;
            slackState = monitorSlack
                             ? ReadCallState(automation.get(), CallApp::Slack,
                                             slackCommand, &callWindow)
                             : CallState::NotInCall;
            now = GetTickCount64();
            lastSlackCheck = now;
            g_slackCallActive.store(slackState != CallState::NotInCall);
            g_slackMuted.store(slackState == CallState::Muted);
            g_slackCallWindow.store(callWindow);
        }

        int teamsCommand =
            g_pendingTeamsCommand.exchange(kCallCommandNone);
        if (lastTeamsCheck == 0 || now - lastTeamsCheck >= 750 ||
            teamsCommand != kCallCommandNone) {
            HWND callWindow = nullptr;
            teamsState = monitorTeams
                             ? ReadCallState(automation.get(), CallApp::Teams,
                                             teamsCommand, &callWindow)
                             : CallState::NotInCall;
            now = GetTickCount64();
            lastTeamsCheck = now;
            g_teamsCallActive.store(teamsState != CallState::NotInCall);
            g_teamsMuted.store(teamsState == CallState::Muted);
            g_teamsCallWindow.store(callWindow);
        }

        int requestedZoomCommand =
            g_pendingZoomCommand.exchange(kCallCommandNone);
        if (requestedZoomCommand != kCallCommandNone) {
            deferredZoomCommand = requestedZoomCommand;
        }
        if (lastZoomCheck == 0 || now - lastZoomCheck >= 750 ||
            requestedZoomCommand != kCallCommandNone) {
            HWND callWindow = nullptr;
            CallState detectedState =
                monitorZoom
                    ? ReadCallState(automation.get(), CallApp::Zoom,
                                    deferredZoomCommand, &callWindow)
                    : CallState::NotInCall;
            bool meetingHostRunning =
                monitorZoom && IsZoomMeetingHostRunning();
            if (detectedState == CallState::NotInCall &&
                meetingHostRunning) {
                if (zoomState == CallState::NotInCall) {
                    zoomState = CallState::Unknown;
                }
                HWND previousWindow = g_zoomCallWindow.load();
                if (!callWindow && IsWindow(previousWindow)) {
                    callWindow = previousWindow;
                }
                if (deferredZoomCommand != kCallCommandNone &&
                    SendZoomMuteShortcut(callWindow)) {
                    zoomState = ApplyCallCommandToState(
                        zoomState, deferredZoomCommand);
                    deferredZoomCommand = kCallCommandNone;
                }
            } else {
                zoomState = detectedState;
                deferredZoomCommand = kCallCommandNone;
            }
            now = GetTickCount64();
            lastZoomCheck = now;
            g_zoomCallActive.store(zoomState != CallState::NotInCall);
            g_zoomMuted.store(zoomState == CallState::Muted);
            g_zoomStateKnown.store(zoomState == CallState::Muted ||
                                   zoomState == CallState::Unmuted);
            g_zoomCallWindow.store(callWindow);
        }

        bool windowsCanHear =
            g_audioAvailable.load() && !g_audioMuted.load();
        float currentPeak = g_audioLinearPeak.load();
        bool playCue = false;
        bool notify = false;

        auto updateWarning = [&](CallState state, bool warningEnabled,
                                 bool audioCue, int speechThreshold,
                                 int speechDelay, ULONGLONG& speakingSince,
                                 ULONGLONG& warningUntil,
                                 std::atomic<bool>& warningActive) {
            bool appIsMuted = state == CallState::Muted;
            if (!appIsMuted || !windowsCanHear) {
                speakingSince = 0;
                warningUntil = 0;
            } else if (currentPeak >= speechThreshold / 100.0f) {
                if (speakingSince == 0) speakingSince = now;
                if (now - speakingSince >=
                    static_cast<ULONGLONG>(speechDelay)) {
                    warningUntil = now + 2000;
                }
            } else {
                speakingSince = 0;
            }

            bool warning = warningEnabled && appIsMuted &&
                           windowsCanHear && now < warningUntil;
            bool previousWarning = warningActive.exchange(warning);
            if (previousWarning != warning) {
                notify = true;
                if (warning && audioCue) playCue = true;
            }
        };

        updateWarning(slackState, g_settings.slackWarning,
                      g_settings.slackAudioCue,
                      g_settings.slackSpeechThreshold,
                      g_settings.slackSpeechDelay, slackSpeakingSince,
                      slackWarningUntil, g_slackWarningActive);
        updateWarning(teamsState, g_settings.teamsWarning,
                      g_settings.teamsAudioCue,
                      g_settings.teamsSpeechThreshold,
                      g_settings.teamsSpeechDelay, teamsSpeakingSince,
                      teamsWarningUntil, g_teamsWarningActive);
        updateWarning(zoomState, g_settings.zoomWarning,
                      g_settings.zoomAudioCue,
                      g_settings.zoomSpeechThreshold,
                      g_settings.zoomSpeechDelay, zoomSpeakingSince,
                      zoomWarningUntil, g_zoomWarningActive);
        if (playCue) MessageBeep(MB_ICONEXCLAMATION);
        if (notify) NotifyTaskbar();
    }

    g_slackCallActive.store(false);
    g_slackMuted.store(false);
    g_slackWarningActive.store(false);
    g_slackCallWindow.store(nullptr);
    g_teamsCallActive.store(false);
    g_teamsMuted.store(false);
    g_teamsWarningActive.store(false);
    g_teamsCallWindow.store(nullptr);
    g_zoomCallActive.store(false);
    g_zoomMuted.store(false);
    g_zoomStateKnown.store(false);
    g_zoomWarningActive.store(false);
    g_zoomCallWindow.store(nullptr);
    NotifyTaskbar();
    automation = nullptr;
    CoUninitialize();
    return 0;
}

static DWORD WINAPI AudioThreadProc(void*) {
    HRESULT coInit = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(coInit)) {
        Wh_Log(L"[Audio] COM initialization failed: 0x%08X", coInit);
        PublishUnavailableAudio();
        return 0;
    }

    winrt::com_ptr<IMMDeviceEnumerator> enumerator;
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                                  CLSCTX_ALL,
                                  __uuidof(IMMDeviceEnumerator),
                                  enumerator.put_void());
    if (FAILED(hr)) {
        Wh_Log(L"[Audio] MMDeviceEnumerator failed: 0x%08X", hr);
        PublishUnavailableAudio();
        CoUninitialize();
        return 0;
    }

    AudioEndpoint endpoint;
    float smoothedPeak = 0.0f;
    ULONGLONG lastEndpointCheck = 0;
    ULONGLONG lastVolumeForce = 0;
    std::wstring observedEndpointId;
    bool observedHardwareMuteKnown = false;
    bool observedHardwareMuted = false;
    HANDLE waits[] = {g_audioStopEvent, g_audioWakeEvent};

    for (;;) {
        DWORD waitResult = WaitForMultipleObjects(
            ARRAYSIZE(waits), waits, FALSE,
            static_cast<DWORD>(g_updateInterval.load()));
        if (waitResult == WAIT_OBJECT_0 || g_unloading.load()) {
            break;
        }

        ULONGLONG now = GetTickCount64();
        if (lastEndpointCheck == 0 || now - lastEndpointCheck >= 1000) {
            lastEndpointCheck = now;
            if (!OpenDefaultEndpoint(enumerator.get(), &endpoint)) {
                observedEndpointId.clear();
                observedHardwareMuteKnown = false;
                PublishUnavailableAudio();
                continue;
            }
            if (observedEndpointId != endpoint.id) {
                observedEndpointId = endpoint.id;
                observedHardwareMuteKnown = false;
            }
        }
        if (!endpoint.device) {
            continue;
        }

        int requestedVolume = g_pendingVolumeSet.exchange(-1);
        if (requestedVolume >= 0) {
            endpoint.volume->SetMasterVolumeLevelScalar(
                requestedVolume / 100.0f, nullptr);
            lastVolumeForce = now;
        }

        int notches = g_pendingVolumeNotches.exchange(0);
        if (notches != 0) {
            float current = 0.0f;
            if (SUCCEEDED(endpoint.volume->GetMasterVolumeLevelScalar(
                    &current))) {
                float next = std::clamp(
                    current + notches * g_settings.volumeStep / 100.0f,
                    0.0f, 1.0f);
                endpoint.volume->SetMasterVolumeLevelScalar(next, nullptr);
            }
        }

        int requestedMute = g_pendingMuteSet.exchange(-1);
        if (requestedMute >= 0) {
            g_pendingMuteToggles.exchange(0);
            endpoint.volume->SetMute(requestedMute != 0, nullptr);
        } else {
            unsigned int toggles = g_pendingMuteToggles.exchange(0);
            if ((toggles & 1U) != 0) {
                BOOL muted = FALSE;
                if (SUCCEEDED(endpoint.volume->GetMute(&muted))) {
                    endpoint.volume->SetMute(!muted, nullptr);
                }
            }
        }

        float peak = 0.0f;
        float volume = 0.0f;
        BOOL muted = FALSE;
        if (FAILED(endpoint.meter->GetPeakValue(&peak)) ||
            FAILED(endpoint.volume->GetMasterVolumeLevelScalar(&volume)) ||
            FAILED(endpoint.volume->GetMute(&muted))) {
            endpoint.Reset();
            PublishUnavailableAudio();
            continue;
        }

        if (g_forceVolume.load()) {
            float targetVolume = g_forcedVolume.load() / 100.0f;
            if (std::fabs(volume - targetVolume) > 0.01f &&
                (lastVolumeForce == 0 || now - lastVolumeForce >= 250)) {
                if (SUCCEEDED(endpoint.volume->SetMasterVolumeLevelScalar(
                        targetVolume, nullptr))) {
                    volume = targetVolume;
                    lastVolumeForce = now;
                }
            }
        }

        float scaledPeak = std::clamp(
            peak * g_peakSensitivity.load() / 100.0f, 0.0f, 1.0f);
        float displayPeak = 0.0f;
        if (scaledPeak > 0.001f) {
            float decibels = 20.0f * std::log10(scaledPeak);
            displayPeak = std::clamp((decibels + 60.0f) / 60.0f,
                                     0.0f, 1.0f);
        }
        float smoothing = displayPeak > smoothedPeak ? 0.65f : 0.18f;
        smoothedPeak += (displayPeak - smoothedPeak) * smoothing;
        if (smoothedPeak < 0.005f) {
            smoothedPeak = 0.0f;
        }

        g_audioAvailable.store(true);
        g_audioMuted.store(muted != FALSE);
        g_audioVolume.store(static_cast<int>(std::lround(volume * 100.0f)));
        g_audioPeak.store(muted ? 0.0f : smoothedPeak);
        g_audioLinearPeak.store(muted ? 0.0f : scaledPeak);
        SetSharedDeviceName(endpoint.name);
        UpdateWindowsHardwareSource(endpoint.hardwareMute, muted != FALSE,
                                    endpoint.name);
        if (endpoint.hardwareMute) {
            bool changed = observedHardwareMuteKnown &&
                           observedHardwareMuted != (muted != FALSE);
            bool initialMuted = !observedHardwareMuteKnown && muted != FALSE;
            bool syncMute = g_settings.headsetSyncMode == L"full" ||
                            g_settings.headsetSyncMode == L"muteOnly";
            bool syncUnmute = g_settings.headsetSyncMode == L"full";
            if (g_settings.headsetSyncCalls &&
                ((muted && (changed || initialMuted) && syncMute) ||
                 (!muted && changed && syncUnmute))) {
                QueueActiveCallMuteState(muted != FALSE);
                RecordDiagnosticEvent(
                    std::wstring(L"Windows hardware mute changed to ") +
                    (muted ? L"muted" : L"unmuted"));
            }
        }
        observedHardwareMuteKnown = true;
        observedHardwareMuted = muted != FALSE;
        NotifyTaskbar();
    }

    endpoint.Reset();
    enumerator = nullptr;
    CoUninitialize();
    return 0;
}

static bool IsSupportedArctisProduct(USHORT productId) {
    return productId == 0x12E0 || productId == 0x12E5 ||
           productId == 0x225D;
}

static HANDLE OpenArctisStatusDevice() {
    GUID hidGuid{};
    HidD_GetHidGuid(&hidGuid);
    HDEVINFO devices = SetupDiGetClassDevsW(
        &hidGuid, nullptr, nullptr, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (devices == INVALID_HANDLE_VALUE) return INVALID_HANDLE_VALUE;

    HANDLE result = INVALID_HANDLE_VALUE;
    SP_DEVICE_INTERFACE_DATA interfaceData{};
    interfaceData.cbSize = sizeof(interfaceData);
    for (DWORD index = 0; SetupDiEnumDeviceInterfaces(
             devices, nullptr, &hidGuid, index, &interfaceData);
         index++) {
        DWORD required = 0;
        SetupDiGetDeviceInterfaceDetailW(
            devices, &interfaceData, nullptr, 0, &required, nullptr);
        if (required < sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W)) continue;

        std::vector<BYTE> detailStorage(required);
        auto* detail = reinterpret_cast<SP_DEVICE_INTERFACE_DETAIL_DATA_W*>(
            detailStorage.data());
        detail->cbSize = sizeof(*detail);
        if (!SetupDiGetDeviceInterfaceDetailW(
                devices, &interfaceData, detail, required, nullptr, nullptr)) {
            continue;
        }

        HANDLE device = CreateFileW(
            detail->DevicePath, GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING,
            FILE_FLAG_OVERLAPPED, nullptr);
        if (device == INVALID_HANDLE_VALUE) continue;

        HIDD_ATTRIBUTES attributes{};
        attributes.Size = sizeof(attributes);
        PHIDP_PREPARSED_DATA preparsed = nullptr;
        HIDP_CAPS caps{};
        bool matches =
            HidD_GetAttributes(device, &attributes) &&
            attributes.VendorID == 0x1038 &&
            IsSupportedArctisProduct(attributes.ProductID) &&
            HidD_GetPreparsedData(device, &preparsed) &&
            HidP_GetCaps(preparsed, &caps) == HIDP_STATUS_SUCCESS &&
            caps.UsagePage == 0xFFC0 && caps.InputReportByteLength == 64 &&
            caps.OutputReportByteLength == 64;
        if (preparsed) HidD_FreePreparsedData(preparsed);
        if (matches) {
            result = device;
            Wh_Log(L"[Arctis] Connected to Nova Pro Wireless HID status "
                   L"interface (PID %04X)",
                   attributes.ProductID);
            break;
        }
        CloseHandle(device);
    }

    SetupDiDestroyDeviceInfoList(devices);
    return result;
}

static bool RunArctisIo(HANDLE device, bool write, BYTE* buffer,
                        DWORD length, DWORD timeout, DWORD* transferred) {
    OVERLAPPED overlapped{};
    overlapped.hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!overlapped.hEvent) return false;

    DWORD bytes = 0;
    BOOL started = write ? WriteFile(device, buffer, length, &bytes,
                                     &overlapped)
                         : ReadFile(device, buffer, length, &bytes,
                                    &overlapped);
    bool success = started != FALSE;
    if (!started && GetLastError() == ERROR_IO_PENDING) {
        if (WaitForSingleObject(overlapped.hEvent, timeout) == WAIT_OBJECT_0) {
            success = GetOverlappedResult(device, &overlapped, &bytes,
                                          FALSE) != FALSE;
        } else {
            CancelIoEx(device, &overlapped);
            WaitForSingleObject(overlapped.hEvent, INFINITE);
            success = false;
        }
    }

    CloseHandle(overlapped.hEvent);
    if (transferred) *transferred = bytes;
    return success;
}

static bool ReadArctisMuteState(HANDLE device, bool* muted, bool* online) {
    BYTE request[64]{};
    request[0] = 0x06;
    request[1] = 0xB0;
    DWORD written = 0;
    if (!RunArctisIo(device, true, request, sizeof(request), 250,
                     &written) ||
        written != sizeof(request)) {
        return false;
    }

    for (int attempt = 0; attempt < 6; attempt++) {
        BYTE response[64]{};
        DWORD read = 0;
        if (!RunArctisIo(device, false, response, sizeof(response), 125,
                         &read)) {
            return false;
        }
        if (read >= 16 && response[0] == 0x06 && response[1] == 0xB0) {
            if (response[9] > 1) return false;
            *muted = response[9] == 1;
            *online = response[15] == 0x08;
            return true;
        }
    }
    return false;
}

class SteelSeriesHeadsetAdapter final : public IHeadsetMuteAdapter {
public:
    ~SteelSeriesHeadsetAdapter() override { Disconnect(); }

    PCWSTR Id() const override { return L"steelseries-nova-pro"; }
    PCWSTR DisplayName() const override { return L"SteelSeries device state"; }

    bool TryConnect() override {
        Disconnect();
        device_ = OpenArctisStatusDevice();
        return device_ != INVALID_HANDLE_VALUE;
    }

    bool Poll(HeadsetAdapterObservation& observation) override {
        observation = {};
        if (device_ == INVALID_HANDLE_VALUE) return false;
        bool muted = false;
        bool online = false;
        if (!ReadArctisMuteState(device_, &muted, &online)) return false;
        observation.available = online;
        observation.stateKnown = online;
        observation.muted = muted;
        observation.deviceName = L"SteelSeries Arctis Nova Pro Wireless";
        observation.detail = L"SteelSeries vendor status report";
        return true;
    }

    void Disconnect() override {
        if (device_ != INVALID_HANDLE_VALUE) CloseHandle(device_);
        device_ = INVALID_HANDLE_VALUE;
    }

private:
    HANDLE device_ = INVALID_HANDLE_VALUE;
};

static std::unique_ptr<IHeadsetMuteAdapter>
CreateSteelSeriesHeadsetAdapter() {
    return std::make_unique<SteelSeriesHeadsetAdapter>();
}

static constexpr USHORT kHidUsagePageGeneric = 0x01;
static constexpr USHORT kHidUsageSystemControl = 0x80;
static constexpr USHORT kHidUsageSystemMicrophoneMute = 0xA9;
static constexpr USHORT kHidUsagePageTelephony = 0x0B;
static constexpr USHORT kHidUsagePhoneMute = 0x2F;
static constexpr USHORT kHidUsageCallMuteToggle = 0xE1;
static constexpr unsigned kHidSystemMuteMask = 1;
static constexpr unsigned kHidPhoneMuteMask = 2;
static constexpr unsigned kHidCallMuteMask = 4;
static constexpr UINT_PTR kStandardHidTimer = 1;

struct StandardHidMetadata {
    USHORT vendorId = 0;
    USHORT productId = 0;
    USHORT usagePage = 0;
    USHORT usage = 0;
    USHORT inputReportLength = 0;
    USHORT outputReportLength = 0;
    USHORT featureReportLength = 0;
    bool systemMicrophoneMute = false;
    bool phoneMute = false;
    bool callMuteToggle = false;
    std::wstring manufacturer;
    std::wstring product;

    bool HasStandardMuteUsage() const {
        return systemMicrophoneMute || phoneMute || callMuteToggle;
    }
};

struct StandardHidRuntime {
    std::unordered_map<unsigned, unsigned> activeMasks;
    std::unordered_map<unsigned, std::vector<BYTE>> previousReports;
};

struct PendingStandardHidAction {
    bool valid = false;
    bool beforeAudioAvailable = false;
    bool beforeAudioMuted = false;
    std::optional<bool> beforeCallMuted;
    std::optional<bool> explicitTargetMuted;
    unsigned usageMask = 0;
};

static std::unordered_map<HANDLE, StandardHidRuntime> g_standardHidRuntime;
static PendingStandardHidAction g_pendingStandardHidAction;

static std::wstring Hex4(USHORT value) {
    wchar_t text[5]{};
    swprintf(text, ARRAYSIZE(text), L"%04X", value);
    return text;
}

static bool GetRawHidPreparsedData(HANDLE device, std::vector<BYTE>& storage,
                                   HIDP_CAPS& caps) {
    UINT size = 0;
    GetRawInputDeviceInfoW(device, RIDI_PREPARSEDDATA, nullptr, &size);
    if (!size) return false;
    storage.resize(size);
    if (GetRawInputDeviceInfoW(device, RIDI_PREPARSEDDATA, storage.data(),
                               &size) == static_cast<UINT>(-1)) {
        return false;
    }
    auto* preparsed = reinterpret_cast<PHIDP_PREPARSED_DATA>(storage.data());
    return HidP_GetCaps(preparsed, &caps) == HIDP_STATUS_SUCCESS;
}

static bool ButtonCapsContains(const HIDP_BUTTON_CAPS& caps, USHORT page,
                               USHORT usage) {
    if (caps.UsagePage != page) return false;
    return caps.IsRange
               ? usage >= caps.Range.UsageMin && usage <= caps.Range.UsageMax
               : caps.NotRange.Usage == usage;
}

static bool ValueCapsContains(const HIDP_VALUE_CAPS& caps, USHORT page,
                              USHORT usage) {
    if (caps.UsagePage != page) return false;
    return caps.IsRange
               ? usage >= caps.Range.UsageMin && usage <= caps.Range.UsageMax
               : caps.NotRange.Usage == usage;
}

static bool InputCapsContainUsage(PHIDP_PREPARSED_DATA preparsed,
                                  const HIDP_CAPS& caps, USHORT page,
                                  USHORT usage) {
    USHORT buttonCount = caps.NumberInputButtonCaps;
    if (buttonCount) {
        std::vector<HIDP_BUTTON_CAPS> buttons(buttonCount);
        if (HidP_GetButtonCaps(HidP_Input, buttons.data(), &buttonCount,
                               preparsed) == HIDP_STATUS_SUCCESS) {
            for (USHORT index = 0; index < buttonCount; index++) {
                if (ButtonCapsContains(buttons[index], page, usage))
                    return true;
            }
        }
    }
    USHORT valueCount = caps.NumberInputValueCaps;
    if (valueCount) {
        std::vector<HIDP_VALUE_CAPS> values(valueCount);
        if (HidP_GetValueCaps(HidP_Input, values.data(), &valueCount,
                              preparsed) == HIDP_STATUS_SUCCESS) {
            for (USHORT index = 0; index < valueCount; index++) {
                if (ValueCapsContains(values[index], page, usage)) return true;
            }
        }
    }
    return false;
}

static std::wstring RawHidString(HANDLE rawDevice, bool product) {
    UINT characters = 0;
    GetRawInputDeviceInfoW(rawDevice, RIDI_DEVICENAME, nullptr, &characters);
    if (!characters) return L"";
    std::vector<wchar_t> path(characters + 1);
    if (GetRawInputDeviceInfoW(rawDevice, RIDI_DEVICENAME, path.data(),
                               &characters) == static_cast<UINT>(-1)) {
        return L"";
    }
    HANDLE device = CreateFileW(path.data(), 0,
                                FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                                OPEN_EXISTING, 0, nullptr);
    if (device == INVALID_HANDLE_VALUE) return L"";
    wchar_t text[256]{};
    BOOLEAN read = product ? HidD_GetProductString(device, text, sizeof(text))
                           : HidD_GetManufacturerString(device, text,
                                                       sizeof(text));
    CloseHandle(device);
    return read ? text : L"";
}

static bool ReadStandardHidMetadata(HANDLE device,
                                    StandardHidMetadata& metadata,
                                    std::vector<BYTE>* preparsedStorage =
                                        nullptr) {
    RID_DEVICE_INFO info{};
    info.cbSize = sizeof(info);
    UINT infoSize = sizeof(info);
    if (GetRawInputDeviceInfoW(device, RIDI_DEVICEINFO, &info, &infoSize) ==
            static_cast<UINT>(-1) ||
        info.dwType != RIM_TYPEHID) {
        return false;
    }

    std::vector<BYTE> localStorage;
    std::vector<BYTE>& storage = preparsedStorage ? *preparsedStorage
                                                  : localStorage;
    HIDP_CAPS caps{};
    if (!GetRawHidPreparsedData(device, storage, caps)) return false;
    auto* preparsed = reinterpret_cast<PHIDP_PREPARSED_DATA>(storage.data());
    metadata = {};
    metadata.vendorId = static_cast<USHORT>(info.hid.dwVendorId);
    metadata.productId = static_cast<USHORT>(info.hid.dwProductId);
    metadata.usagePage = info.hid.usUsagePage;
    metadata.usage = info.hid.usUsage;
    metadata.inputReportLength = caps.InputReportByteLength;
    metadata.outputReportLength = caps.OutputReportByteLength;
    metadata.featureReportLength = caps.FeatureReportByteLength;
    metadata.systemMicrophoneMute = InputCapsContainUsage(
        preparsed, caps, kHidUsagePageGeneric,
        kHidUsageSystemMicrophoneMute);
    metadata.phoneMute = InputCapsContainUsage(
        preparsed, caps, kHidUsagePageTelephony, kHidUsagePhoneMute);
    metadata.callMuteToggle = InputCapsContainUsage(
        preparsed, caps, kHidUsagePageTelephony, kHidUsageCallMuteToggle);
    metadata.manufacturer = RawHidString(device, false);
    metadata.product = RawHidString(device, true);
    return true;
}

static bool UsageAsserted(PHIDP_PREPARSED_DATA preparsed, USHORT page,
                          USHORT usage, const BYTE* report,
                          ULONG reportLength) {
    std::vector<USAGE> usages(128);
    ULONG usageCount = static_cast<ULONG>(usages.size());
    NTSTATUS status = HidP_GetUsages(
        HidP_Input, page, 0, usages.data(), &usageCount, preparsed,
        reinterpret_cast<PCHAR>(const_cast<BYTE*>(report)), reportLength);
    if (status == HIDP_STATUS_SUCCESS) {
        for (ULONG index = 0; index < usageCount; index++) {
            if (usages[index] == usage) return true;
        }
    }
    ULONG value = 0;
    return HidP_GetUsageValue(
               HidP_Input, page, 0, usage, &value, preparsed,
               reinterpret_cast<PCHAR>(const_cast<BYTE*>(report)),
               reportLength) == HIDP_STATUS_SUCCESS &&
           value != 0;
}

static unsigned StandardMuteMask(PHIDP_PREPARSED_DATA preparsed,
                                 const StandardHidMetadata& metadata,
                                 const BYTE* report, ULONG reportLength) {
    unsigned mask = 0;
    if (metadata.systemMicrophoneMute &&
        UsageAsserted(preparsed, kHidUsagePageGeneric,
                      kHidUsageSystemMicrophoneMute, report, reportLength)) {
        mask |= kHidSystemMuteMask;
    }
    if (metadata.phoneMute &&
        UsageAsserted(preparsed, kHidUsagePageTelephony, kHidUsagePhoneMute,
                      report, reportLength)) {
        mask |= kHidPhoneMuteMask;
    }
    if (metadata.callMuteToggle &&
        UsageAsserted(preparsed, kHidUsagePageTelephony,
                      kHidUsageCallMuteToggle, report, reportLength)) {
        mask |= kHidCallMuteMask;
    }
    return mask;
}

static std::wstring StandardUsageDescription(
    const StandardHidMetadata& metadata) {
    std::wstring result;
    auto append = [&](bool present, PCWSTR name) {
        if (!present) return;
        if (!result.empty()) result += L", ";
        result += name;
    };
    append(metadata.systemMicrophoneMute, L"System Microphone Mute");
    append(metadata.phoneMute, L"Phone Mute");
    append(metadata.callMuteToggle, L"Call Mute Toggle");
    return result;
}

static std::optional<bool> CurrentCallMuteState() {
    int selected = GetSelectedActiveCallIndex();
    if (selected == 0) return g_slackMuted.load();
    if (selected == 1) return g_teamsMuted.load();
    if (selected == 2 && g_zoomStateKnown.load()) return g_zoomMuted.load();
    return std::nullopt;
}

static void BeginStandardHidAction(
    unsigned usageMask,
    std::optional<bool> explicitTargetMuted = std::nullopt) {
    g_pendingStandardHidAction.valid = true;
    g_pendingStandardHidAction.beforeAudioAvailable = g_audioAvailable.load();
    g_pendingStandardHidAction.beforeAudioMuted = g_audioMuted.load();
    g_pendingStandardHidAction.beforeCallMuted = CurrentCallMuteState();
    g_pendingStandardHidAction.explicitTargetMuted = explicitTargetMuted;
    g_pendingStandardHidAction.usageMask = usageMask;
    if (g_headsetMessageWindow) {
        KillTimer(g_headsetMessageWindow, kStandardHidTimer);
        SetTimer(g_headsetMessageWindow, kStandardHidTimer, 300, nullptr);
    }
}

static void ResolveStandardHidAction() {
    PendingStandardHidAction action = g_pendingStandardHidAction;
    g_pendingStandardHidAction = {};
    if (!action.valid || g_settings.headsetSyncMode == L"off" ||
        g_settings.headsetSyncMode == L"statusOnly") {
        return;
    }

    bool currentAudioAvailable = g_audioAvailable.load();
    bool currentAudioMuted = g_audioMuted.load();
    std::optional<bool> currentCallMuted = CurrentCallMuteState();
    bool audioChanged = action.beforeAudioAvailable && currentAudioAvailable &&
                        action.beforeAudioMuted != currentAudioMuted;
    bool callChanged = action.beforeCallMuted && currentCallMuted &&
                       *action.beforeCallMuted != *currentCallMuted;
    bool systemFirst = (action.usageMask & kHidSystemMuteMask) != 0;
    bool targetMuted = false;
    if (action.explicitTargetMuted) {
        targetMuted = *action.explicitTargetMuted;
    } else if (systemFirst && audioChanged) {
        targetMuted = currentAudioMuted;
    } else if (!systemFirst && callChanged) {
        targetMuted = *currentCallMuted;
    } else if (audioChanged) {
        targetMuted = currentAudioMuted;
    } else if (callChanged) {
        targetMuted = *currentCallMuted;
    } else if (systemFirst || !action.beforeCallMuted) {
        targetMuted = !action.beforeAudioMuted;
    } else {
        targetMuted = !*action.beforeCallMuted;
    }

    if (!targetMuted && g_settings.headsetSyncMode != L"full") return;
    if (g_settings.headsetSyncWindows &&
        (!currentAudioAvailable || currentAudioMuted != targetMuted)) {
        QueueMuteSet(targetMuted);
    }
    if (g_settings.headsetSyncCalls) {
        QueueActiveCallMuteState(targetMuted);
    }
    RecordDiagnosticEvent(
        std::wstring(L"Standard HID action resolved to ") +
        (targetMuted ? L"muted" : L"unmuted"));
}

static void RefreshStandardHidDevices() {
    UINT count = 0;
    if (GetRawInputDeviceList(nullptr, &count, sizeof(RAWINPUTDEVICELIST)) ==
            static_cast<UINT>(-1) ||
        !count) {
        UpdateStandardHidSource(false, L"", L"");
        return;
    }
    std::vector<RAWINPUTDEVICELIST> devices(count);
    if (GetRawInputDeviceList(devices.data(), &count,
                              sizeof(RAWINPUTDEVICELIST)) ==
        static_cast<UINT>(-1)) {
        UpdateStandardHidSource(false, L"", L"");
        return;
    }
    for (UINT index = 0; index < count; index++) {
        if (devices[index].dwType != RIM_TYPEHID) continue;
        StandardHidMetadata metadata;
        if (!ReadStandardHidMetadata(devices[index].hDevice, metadata) ||
            !metadata.HasStandardMuteUsage()) {
            continue;
        }
        std::wstring name = metadata.product.empty()
                                ? L"HID headset VID " + Hex4(metadata.vendorId) +
                                      L" / PID " + Hex4(metadata.productId)
                                : metadata.product;
        UpdateStandardHidSource(true, name,
                                StandardUsageDescription(metadata));
        return;
    }
    UpdateStandardHidSource(false, L"", L"");
}

static void RecordSanitizedReportChange(
    const StandardHidMetadata& metadata, StandardHidRuntime& runtime,
    const BYTE* report, DWORD reportLength, unsigned reportKey,
    unsigned muteMask) {
    auto& previous = runtime.previousReports[reportKey];
    std::wstring event = L"HID VID " + Hex4(metadata.vendorId) + L" / PID " +
                         Hex4(metadata.productId) + L", report " +
                         std::to_wstring(reportKey) + L", length " +
                         std::to_wstring(reportLength);
    if (previous.size() == reportLength) {
        std::wstring offsets;
        for (DWORD index = 0; index < reportLength; index++) {
            if (previous[index] == report[index]) continue;
            if (!offsets.empty()) offsets += L",";
            offsets += std::to_wstring(index);
        }
        if (offsets.empty()) return;
        event += L", changed byte offsets [" + offsets + L"]";
    } else {
        event += L", initial sanitized report";
    }
    if (muteMask) event += L", standard mute usage asserted";
    previous.assign(report, report + reportLength);
    RecordDiagnosticEvent(event);
}

static void ProcessStandardHidRawInput(HRAWINPUT inputHandle) {
    UINT size = 0;
    GetRawInputData(inputHandle, RID_INPUT, nullptr, &size,
                    sizeof(RAWINPUTHEADER));
    if (!size) return;
    std::vector<BYTE> inputStorage(size);
    if (GetRawInputData(inputHandle, RID_INPUT, inputStorage.data(), &size,
                        sizeof(RAWINPUTHEADER)) == static_cast<UINT>(-1)) {
        return;
    }
    auto* input = reinterpret_cast<RAWINPUT*>(inputStorage.data());
    if (input->header.dwType != RIM_TYPEHID) return;

    std::vector<BYTE> preparsedStorage;
    StandardHidMetadata metadata;
    if (!ReadStandardHidMetadata(input->header.hDevice, metadata,
                                 &preparsedStorage) ||
        !metadata.HasStandardMuteUsage()) {
        return;
    }
    auto* preparsed =
        reinterpret_cast<PHIDP_PREPARSED_DATA>(preparsedStorage.data());
    auto& runtime = g_standardHidRuntime[input->header.hDevice];
    for (DWORD index = 0; index < input->data.hid.dwCount; index++) {
        const BYTE* report = input->data.hid.bRawData +
                             index * input->data.hid.dwSizeHid;
        DWORD reportLength = input->data.hid.dwSizeHid;
        unsigned reportKey = reportLength ? report[0] : 0;
        unsigned mask = StandardMuteMask(preparsed, metadata, report,
                                         reportLength);
        RecordSanitizedReportChange(metadata, runtime, report, reportLength,
                                    reportKey, mask);
        unsigned previousMask = runtime.activeMasks[reportKey];
        unsigned asserted =
            (mask & ~previousMask) &
            (kHidSystemMuteMask | kHidCallMuteMask);
        unsigned phoneChanged =
            (mask ^ previousMask) & kHidPhoneMuteMask;
        runtime.activeMasks[reportKey] = mask;
        if (!asserted && !phoneChanged) continue;
        std::wstring name = metadata.product.empty()
                                ? L"HID headset VID " + Hex4(metadata.vendorId) +
                                      L" / PID " + Hex4(metadata.productId)
                                : metadata.product;
        UpdateStandardHidSource(true, name,
                                StandardUsageDescription(metadata));
        RecordDiagnosticEvent(L"Standard HID mute control changed: " +
                              StandardUsageDescription(metadata));
        std::optional<bool> explicitTarget;
        if (phoneChanged) {
            explicitTarget = (mask & kHidPhoneMuteMask) != 0;
        }
        BeginStandardHidAction(asserted | phoneChanged, explicitTarget);
    }
}

static bool RegisterStandardHidInput(HWND target) {
    RAWINPUTDEVICE devices[2]{};
    devices[0].usUsagePage = kHidUsagePageGeneric;
    devices[0].usUsage = kHidUsageSystemControl;
    devices[0].dwFlags = RIDEV_INPUTSINK | RIDEV_DEVNOTIFY;
    devices[0].hwndTarget = target;
    devices[1].usUsagePage = kHidUsagePageTelephony;
    devices[1].usUsage = 0;
    devices[1].dwFlags =
        RIDEV_INPUTSINK | RIDEV_DEVNOTIFY | RIDEV_PAGEONLY;
    devices[1].hwndTarget = target;
    return RegisterRawInputDevices(devices, ARRAYSIZE(devices),
                                   sizeof(devices[0])) != FALSE;
}

static LRESULT CALLBACK HeadsetMessageWindowProc(HWND window, UINT message,
                                                 WPARAM wParam,
                                                 LPARAM lParam) {
    switch (message) {
        case WM_INPUT:
            ProcessStandardHidRawInput(reinterpret_cast<HRAWINPUT>(lParam));
            return DefWindowProcW(window, message, wParam, lParam);
        case WM_INPUT_DEVICE_CHANGE:
            g_standardHidRuntime.clear();
            RefreshStandardHidDevices();
            return 0;
        case WM_TIMER:
            if (wParam == kStandardHidTimer) {
                KillTimer(window, kStandardHidTimer);
                ResolveStandardHidAction();
                return 0;
            }
            break;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

static HWND CreateHeadsetMessageWindow() {
    GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(&HeadsetMessageWindowProc),
        &g_headsetWindowInstance);
    if (!g_headsetWindowInstance) return nullptr;
    WNDCLASSW windowClass{};
    windowClass.lpfnWndProc = HeadsetMessageWindowProc;
    windowClass.hInstance = g_headsetWindowInstance;
    windowClass.lpszClassName = kHeadsetWindowClass;
    RegisterClassW(&windowClass);
    return CreateWindowExW(0, kHeadsetWindowClass, L"", 0, 0, 0, 0, 0,
                           HWND_MESSAGE, nullptr, windowClass.hInstance,
                           nullptr);
}

static std::wstring BuildHeadsetDiagnostics() {
    SYSTEMTIME now{};
    GetLocalTime(&now);
    std::wstringstream output;
    output << L"MuteAlert Windhawk - headset diagnostics\r\n";
    output << L"Generated: " << now.wYear << L"-" << now.wMonth << L"-"
           << now.wDay << L" " << now.wHour << L":" << now.wMinute << L":"
           << now.wSecond << L"\r\n";
    output << L"Privacy: device paths, serial numbers, and raw HID values are "
              L"not included.\r\n\r\n";

    HeadsetStatus status = GetHeadsetStatus();
    output << L"Current detection\r\n";
    output << L"  Method: " << HeadsetMethodName(status.method) << L"\r\n";
    output << L"  Confidence: " << HeadsetConfidenceName(status.confidence)
           << L"\r\n";
    output << L"  Device: "
           << (status.deviceName.empty() ? L"(none)" : status.deviceName)
           << L"\r\n";
    output << L"  State: "
           << (!status.stateKnown ? L"unknown"
                                  : status.muted ? L"muted" : L"unmuted")
           << L"\r\n";
    output << L"  Detail: " << status.detail << L"\r\n\r\n";

    output << L"Vendor adapter registry\r\n";
    for (const auto& slot : GetVendorAdapterSlots()) {
        output << L"  VID " << Hex4(slot.vendorId) << L": "
               << slot.vendorName << L" - "
               << (slot.implemented ? L"implemented" : L"extension slot")
               << L" (" << slot.adapterId << L")\r\n";
    }
    output << L"  Other vendors can implement IHeadsetMuteAdapter.\r\n\r\n";

    output << L"Sanitized HID descriptor summaries\r\n";
    UINT count = 0;
    if (GetRawInputDeviceList(nullptr, &count, sizeof(RAWINPUTDEVICELIST)) !=
            static_cast<UINT>(-1) &&
        count) {
        std::vector<RAWINPUTDEVICELIST> devices(count);
        if (GetRawInputDeviceList(devices.data(), &count,
                                  sizeof(RAWINPUTDEVICELIST)) !=
            static_cast<UINT>(-1)) {
            unsigned written = 0;
            for (UINT index = 0; index < count; index++) {
                if (devices[index].dwType != RIM_TYPEHID) continue;
                StandardHidMetadata metadata;
                if (!ReadStandardHidMetadata(devices[index].hDevice,
                                             metadata)) {
                    continue;
                }
                const VendorAdapterSlot* slot =
                    FindVendorAdapterSlot(metadata.vendorId);
                output << L"  [" << ++written << L"] VID "
                       << Hex4(metadata.vendorId) << L" / PID "
                       << Hex4(metadata.productId) << L"\r\n";
                output << L"      Manufacturer: "
                       << (metadata.manufacturer.empty()
                               ? L"(not reported)"
                               : metadata.manufacturer)
                       << L"\r\n";
                output << L"      Product: "
                       << (metadata.product.empty() ? L"(not reported)"
                                                    : metadata.product)
                       << L"\r\n";
                output << L"      Top-level usage: page 0x"
                       << Hex4(metadata.usagePage) << L", usage 0x"
                       << Hex4(metadata.usage) << L"\r\n";
                output << L"      Reports: input "
                       << metadata.inputReportLength << L", output "
                       << metadata.outputReportLength << L", feature "
                       << metadata.featureReportLength << L" bytes\r\n";
                output << L"      Standard mute usages: "
                       << (metadata.HasStandardMuteUsage()
                               ? StandardUsageDescription(metadata)
                               : L"none")
                       << L"\r\n";
                output << L"      Vendor adapter: "
                       << (slot ? slot->vendorName : L"unregistered")
                       << (slot && slot->implemented ? L" (implemented)" : L"")
                       << L"\r\n";
            }
            if (!written) output << L"  No HID devices were readable.\r\n";
        }
    } else {
        output << L"  No HID devices were enumerated.\r\n";
    }

    output << L"\r\nSanitized report-change log\r\n";
    AcquireSRWLockShared(&g_diagnosticLock);
    if (g_diagnosticEvents.empty()) {
        output << L"  No report changes observed during this run.\r\n";
    } else {
        for (const auto& event : g_diagnosticEvents) {
            output << L"  " << event << L"\r\n";
        }
    }
    ReleaseSRWLockShared(&g_diagnosticLock);
    return output.str();
}

static bool WriteUtf8File(const std::wstring& path,
                          const std::wstring& content) {
    int required = WideCharToMultiByte(
        CP_UTF8, 0, content.c_str(), static_cast<int>(content.size()),
        nullptr, 0, nullptr, nullptr);
    if (required <= 0) return false;
    std::vector<char> bytes(static_cast<size_t>(required));
    WideCharToMultiByte(CP_UTF8, 0, content.c_str(),
                        static_cast<int>(content.size()), bytes.data(),
                        required, nullptr, nullptr);
    HANDLE file = CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr,
                              CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;
    DWORD written = 0;
    bool success = WriteFile(file, bytes.data(),
                             static_cast<DWORD>(bytes.size()), &written,
                             nullptr) != FALSE &&
                   written == bytes.size();
    CloseHandle(file);
    return success;
}

static void ExportHeadsetDiagnosticsIfRequested() {
    if (g_settings.headsetDiagnosticsPath.empty()) return;
    bool success = WriteUtf8File(g_settings.headsetDiagnosticsPath,
                                 BuildHeadsetDiagnostics());
    Wh_Log(L"[Headset] Diagnostics export to %s %s",
           g_settings.headsetDiagnosticsPath.c_str(),
           success ? L"completed" : L"failed");
}

static DWORD WINAPI HeadsetThreadProc(void*) {
    g_headsetMessageWindow = CreateHeadsetMessageWindow();
    if (g_headsetMessageWindow) {
        if (!RegisterStandardHidInput(g_headsetMessageWindow)) {
            Wh_Log(L"[Headset] Standard HID Raw Input registration failed");
        }
        RefreshStandardHidDevices();
    } else {
        Wh_Log(L"[Headset] Could not create the HID message window");
    }

    auto adapter = CreateSteelSeriesHeadsetAdapter();
    bool connected = false;
    bool stateKnown = false;
    bool previousMuted = false;
    ULONGLONG nextVendorPoll = 0;
    bool diagnosticsExported = false;

    for (;;) {
        if (g_unloading.load() ||
            WaitForSingleObject(g_audioStopEvent, 0) == WAIT_OBJECT_0) {
            break;
        }

        ULONGLONG now = GetTickCount64();
        if (now >= nextVendorPoll) {
            if (!connected) {
                connected = adapter->TryConnect();
                stateKnown = false;
                if (connected) {
                    RecordDiagnosticEvent(
                        L"SteelSeries vendor adapter connected");
                } else {
                    UpdateSteelSeriesSource(false, false, L"", L"");
                }
            }
            if (connected) {
                HeadsetAdapterObservation observation;
                if (!adapter->Poll(observation)) {
                    adapter->Disconnect();
                    connected = false;
                    stateKnown = false;
                    UpdateSteelSeriesSource(false, false, L"", L"");
                    RecordDiagnosticEvent(
                        L"SteelSeries adapter disconnected after read failure");
                    nextVendorPoll = now + 500;
                } else if (!observation.available ||
                           !observation.stateKnown) {
                    stateKnown = false;
                    UpdateSteelSeriesSource(false, false, L"", L"");
                    nextVendorPoll = now + g_settings.headsetPollInterval;
                } else {
                    bool stateChanged =
                        !stateKnown || observation.muted != previousMuted;
                    UpdateSteelSeriesSource(
                        true, observation.muted, observation.deviceName,
                        observation.detail);
                    if (stateChanged) {
                        Wh_Log(L"[Headset] SteelSeries microphone is %s",
                               observation.muted ? L"muted" : L"unmuted");
                        RecordDiagnosticEvent(
                            std::wstring(L"SteelSeries physical state: ") +
                            (observation.muted ? L"muted" : L"unmuted"));
                    }
                    bool syncMute = g_settings.headsetSyncMode == L"full" ||
                                    g_settings.headsetSyncMode == L"muteOnly";
                    bool syncUnmute = g_settings.headsetSyncMode == L"full";
                    if (observation.muted && syncMute) {
                        if (g_settings.headsetSyncWindows &&
                            !g_audioMuted.load()) {
                            QueueMuteSet(true);
                        }
                        if (g_settings.headsetSyncCalls) {
                            QueueActiveCallMuteState(true);
                        }
                    } else if (!observation.muted && syncUnmute &&
                               stateKnown && previousMuted) {
                        if (g_settings.headsetSyncWindows) QueueMuteSet(false);
                        if (g_settings.headsetSyncCalls) {
                            QueueActiveCallMuteState(false);
                        }
                    }
                    stateKnown = true;
                    previousMuted = observation.muted;
                    nextVendorPoll = now + g_settings.headsetPollInterval;
                }
            } else {
                nextVendorPoll = now + 2000;
            }
        }

        if (!diagnosticsExported) {
            ExportHeadsetDiagnosticsIfRequested();
            diagnosticsExported = true;
        }

        DWORD wait = MsgWaitForMultipleObjects(
            1, &g_audioStopEvent, FALSE, 50, QS_ALLINPUT);
        if (wait == WAIT_OBJECT_0) break;
        if (wait == WAIT_OBJECT_0 + 1) {
            MSG message;
            while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
        }
    }

    adapter->Disconnect();
    UpdateSteelSeriesSource(false, false, L"", L"");
    UpdateStandardHidSource(false, L"", L"");
    g_standardHidRuntime.clear();
    if (g_headsetMessageWindow) {
        DestroyWindow(g_headsetMessageWindow);
        g_headsetMessageWindow = nullptr;
    }
    if (g_headsetWindowInstance) {
        UnregisterClassW(kHeadsetWindowClass, g_headsetWindowInstance);
        g_headsetWindowInstance = nullptr;
    }
    return 0;
}

static bool StartAudioThread() {
    if (g_audioThread) {
        return true;
    }

    g_audioStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_audioWakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_audioStopEvent || !g_audioWakeEvent) {
        if (g_audioStopEvent) CloseHandle(g_audioStopEvent);
        if (g_audioWakeEvent) CloseHandle(g_audioWakeEvent);
        g_audioStopEvent = nullptr;
        g_audioWakeEvent = nullptr;
        return false;
    }

    g_audioThread = CreateThread(nullptr, 0, AudioThreadProc, nullptr, 0,
                                 nullptr);
    if (!g_audioThread) {
        CloseHandle(g_audioStopEvent);
        CloseHandle(g_audioWakeEvent);
        g_audioStopEvent = nullptr;
        g_audioWakeEvent = nullptr;
        return false;
    }
    bool headsetSyncsCalls =
        g_settings.headsetSyncCalls &&
        (g_settings.headsetSyncMode == L"full" ||
         g_settings.headsetSyncMode == L"muteOnly");
    if (g_settings.showCallStateIcon || headsetSyncsCalls ||
        g_settings.slackWarning ||
        g_settings.slackRightClickToggle ||
        g_settings.teamsWarning || g_settings.teamsRightClickToggle ||
        g_settings.zoomWarning || g_settings.zoomRightClickToggle) {
        g_callAppsThread = CreateThread(
            nullptr, 0, CallAppsThreadProc, nullptr, 0, nullptr);
        if (!g_callAppsThread) {
            Wh_Log(L"[Call apps] Failed to start monitor thread");
        }
    }
    if (g_settings.headsetSyncMode != L"off" ||
        !g_settings.headsetDiagnosticsPath.empty()) {
        g_headsetThread = CreateThread(
            nullptr, 0, HeadsetThreadProc, nullptr, 0, nullptr);
        if (!g_headsetThread) {
            Wh_Log(L"[Headset] Failed to start headset monitor thread");
        }
    }
    return true;
}

static void StopAudioThread() {
    if (g_audioStopEvent) {
        SetEvent(g_audioStopEvent);
    }
    if (g_headsetThread) {
        WaitForSingleObject(g_headsetThread, INFINITE);
        CloseHandle(g_headsetThread);
    }
    if (g_callAppsThread) {
        WaitForSingleObject(g_callAppsThread, INFINITE);
        CloseHandle(g_callAppsThread);
    }
    if (g_audioThread) {
        WaitForSingleObject(g_audioThread, INFINITE);
        CloseHandle(g_audioThread);
    }
    if (g_audioStopEvent) CloseHandle(g_audioStopEvent);
    if (g_audioWakeEvent) CloseHandle(g_audioWakeEvent);
    g_audioThread = nullptr;
    g_callAppsThread = nullptr;
    g_headsetThread = nullptr;
    g_audioStopEvent = nullptr;
    g_audioWakeEvent = nullptr;
}

// -----------------------------------------------------------------------------
// Taskbar XAML access
// -----------------------------------------------------------------------------

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void*, void**);
using CSecondaryTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void*, void**);
using std__Ref_count_base__Decref_t = void(WINAPI*)(void*);

static void* CTaskBand_ITaskListWndSite_vftable = nullptr;
static void* CSecondaryTaskBand_ITaskListWndSite_vftable = nullptr;
static CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original = nullptr;
static CSecondaryTaskBand_GetTaskbarHost_t
    CSecondaryTaskBand_GetTaskbarHost_Original = nullptr;
static void* TaskbarHost_FrameHeight_Original = nullptr;
static std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original =
    nullptr;

static XamlRoot XamlRootFromTaskbarHostSharedPtr(void* sharedPtr[2]) {
    if (!sharedPtr[0] || !sharedPtr[1] || !TaskbarHost_FrameHeight_Original ||
        !std__Ref_count_base__Decref_Original) {
        if (sharedPtr[1] && std__Ref_count_base__Decref_Original) {
            std__Ref_count_base__Decref_Original(sharedPtr[1]);
        }
        return nullptr;
    }

    size_t elementOffset = 0x10;
    const BYTE* bytes = static_cast<const BYTE*>(TaskbarHost_FrameHeight_Original);
    if (bytes[0] == 0x48 && bytes[1] == 0x83 && bytes[2] == 0xEC &&
        bytes[4] == 0x48 && bytes[5] == 0x83 && bytes[6] == 0xC1 &&
        bytes[7] <= 0x7F) {
        elementOffset = bytes[7];
    } else {
        Wh_Log(L"[XAML] Unsupported TaskbarHost::FrameHeight prologue");
    }

    auto* unknown =
        *reinterpret_cast<IUnknown**>(static_cast<BYTE*>(sharedPtr[0]) +
                                      elementOffset);
    FrameworkElement taskbarElement = nullptr;
    if (unknown) {
        unknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                winrt::put_abi(taskbarElement));
    }
    XamlRoot result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;
    std__Ref_count_base__Decref_Original(sharedPtr[1]);
    return result;
}

static XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    HWND taskSwitch =
        reinterpret_cast<HWND>(GetPropW(hTaskbarWnd, L"TaskbandHWND"));
    if (!taskSwitch || !CTaskBand_ITaskListWndSite_vftable ||
        !CTaskBand_GetTaskbarHost_Original) {
        return nullptr;
    }

    void* taskBand = reinterpret_cast<void*>(GetWindowLongPtrW(taskSwitch, 0));
    void* site = taskBand;
    for (int i = 0; site &&
                    *reinterpret_cast<void**>(site) !=
                        CTaskBand_ITaskListWndSite_vftable;
         i++, site = reinterpret_cast<void**>(site) + 1) {
        if (i == 20) return nullptr;
    }
    if (!site) return nullptr;

    void* sharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(site, sharedPtr);
    return XamlRootFromTaskbarHostSharedPtr(sharedPtr);
}

static XamlRoot GetSecondaryTaskbarXamlRoot(HWND hTaskbarWnd) {
    HWND taskSwitch =
        FindWindowExW(hTaskbarWnd, nullptr, L"WorkerW", nullptr);
    if (!taskSwitch || !CSecondaryTaskBand_ITaskListWndSite_vftable ||
        !CSecondaryTaskBand_GetTaskbarHost_Original) {
        return nullptr;
    }

    void* taskBand = reinterpret_cast<void*>(GetWindowLongPtrW(taskSwitch, 0));
    void* site = taskBand;
    for (int i = 0; site &&
                    *reinterpret_cast<void**>(site) !=
                        CSecondaryTaskBand_ITaskListWndSite_vftable;
         i++, site = reinterpret_cast<void**>(site) + 1) {
        if (i == 20) return nullptr;
    }
    if (!site) return nullptr;

    void* sharedPtr[2]{};
    CSecondaryTaskBand_GetTaskbarHost_Original(site, sharedPtr);
    return XamlRootFromTaskbarHostSharedPtr(sharedPtr);
}

using RunFromWindowThreadProc_t = void(WINAPI*)(void*);

static bool RunFromWindowThread(HWND hWnd, RunFromWindowThreadProc_t proc,
                                void* parameter) {
    static const UINT message = RegisterWindowMessageW(
        L"Windhawk_RunFromTaskbarThread_" WH_MOD_ID);
    struct Call {
        RunFromWindowThreadProc_t proc;
        void* parameter;
    };

    DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (!threadId) return false;
    if (threadId == GetCurrentThreadId()) {
        proc(parameter);
        return true;
    }

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int code, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (code == HC_ACTION) {
                const CWPSTRUCT* messageData =
                    reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (messageData->message == message) {
                    auto* call = reinterpret_cast<Call*>(messageData->lParam);
                    call->proc(call->parameter);
                }
            }
            return CallNextHookEx(nullptr, code, wParam, lParam);
        },
        nullptr, threadId);
    if (!hook) return false;

    Call call{proc, parameter};
    SendMessageW(hWnd, message, 0, reinterpret_cast<LPARAM>(&call));
    UnhookWindowsHookEx(hook);
    return true;
}

static HWND FindMainTaskbarWindow() {
    HWND result = nullptr;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD processId = 0;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &processId) &&
                processId == GetCurrentProcessId() &&
                GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&result));
    return result;
}

static FrameworkElement FindChildRecursive(
    FrameworkElement const& element,
    std::function<bool(FrameworkElement)> const& predicate,
    int maxDepth = 20) {
    int count = VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < count && maxDepth > 0; i++) {
        auto child = VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child) continue;
        if (predicate(child)) return child;
        if (auto found = FindChildRecursive(child, predicate, maxDepth - 1)) {
            return found;
        }
    }
    return nullptr;
}

// -----------------------------------------------------------------------------
// Widget construction, updates, and cleanup
// -----------------------------------------------------------------------------

struct WidgetState {
    HWND taskbarWnd = nullptr;
    Grid parent{nullptr};
    Button button{nullptr};
    Grid fillLayer{nullptr};
    RectangleGeometry fillClip{nullptr};
    Shapes::Line muteLine{nullptr};
    TextBlock tooltipText{nullptr};
    ToolTip tooltip{nullptr};
    Button callButton{nullptr};
    Grid callLogoHost{nullptr};
    Grid callSlackLogo{nullptr};
    Grid callTeamsLogo{nullptr};
    Grid callZoomLogo{nullptr};
    Grid callMultipleBadge{nullptr};
    Shapes::Line callMuteLine{nullptr};
    TextBlock callTooltipText{nullptr};
    ToolTip callTooltip{nullptr};
    int column = -1;
    winrt::event_token clickToken{};
    winrt::event_token wheelToken{};
    winrt::event_token rightTappedToken{};
    winrt::event_token callClickToken{};
    winrt::event_token callRightTappedToken{};
    bool warningOpen = false;
};

[[clang::no_destroy]] static std::optional<std::vector<WidgetState>>
    g_widgets{std::in_place};
static std::atomic<bool> g_widgetsLive{false};

static SolidColorBrush MakeBrush(winrt::Windows::UI::Color color) {
    return SolidColorBrush(color);
}

static winrt::Windows::UI::Color AccentColor() {
    try {
        return winrt::Windows::UI::ViewManagement::UISettings().GetColorValue(
            winrt::Windows::UI::ViewManagement::UIColorType::Accent);
    } catch (...) {
        return {255, 0, 120, 215};
    }
}

static Grid BuildMicrophoneLayer(double size,
                                 SolidColorBrush const& brush) {
    double scale = size / 18.0;
    Grid layer;
    layer.Width(size);
    layer.Height(size);
    layer.IsHitTestVisible(false);

    Shapes::Rectangle capsule;
    capsule.Width(7.0 * scale);
    capsule.Height(10.5 * scale);
    capsule.RadiusX(3.5 * scale);
    capsule.RadiusY(3.5 * scale);
    capsule.Fill(brush);
    capsule.HorizontalAlignment(HorizontalAlignment::Center);
    capsule.VerticalAlignment(VerticalAlignment::Top);
    capsule.Margin({0.0, 0.5 * scale, 0.0, 0.0});

    Border cradle;
    cradle.Width(13.0 * scale);
    cradle.Height(8.0 * scale);
    cradle.BorderBrush(brush);
    cradle.BorderThickness(
        {2.0 * scale, 0.0, 2.0 * scale, 2.0 * scale});
    cradle.CornerRadius({6.5 * scale});
    cradle.Background(MakeBrush({0, 0, 0, 0}));
    cradle.HorizontalAlignment(HorizontalAlignment::Center);
    cradle.VerticalAlignment(VerticalAlignment::Top);
    cradle.Margin({0.0, 6.5 * scale, 0.0, 0.0});

    Shapes::Rectangle stem;
    stem.Width(2.0 * scale);
    stem.Height(4.0 * scale);
    stem.Fill(brush);
    stem.HorizontalAlignment(HorizontalAlignment::Center);
    stem.VerticalAlignment(VerticalAlignment::Bottom);
    stem.Margin({0.0, 0.0, 0.0, 1.5 * scale});

    Shapes::Rectangle base;
    base.Width(8.0 * scale);
    base.Height(2.0 * scale);
    base.RadiusX(1.0 * scale);
    base.RadiusY(1.0 * scale);
    base.Fill(brush);
    base.HorizontalAlignment(HorizontalAlignment::Center);
    base.VerticalAlignment(VerticalAlignment::Bottom);

    layer.Children().Append(capsule);
    layer.Children().Append(cradle);
    layer.Children().Append(stem);
    layer.Children().Append(base);
    return layer;
}

static Grid BuildSlackLogo(double size) {
    double scale = size / 18.0;
    Grid logo;
    logo.Width(size);
    logo.Height(size);
    logo.IsHitTestVisible(false);

    Canvas canvas;
    canvas.Width(size);
    canvas.Height(size);
    auto addBar = [&](double left, double top, double width, double height,
                      winrt::Windows::UI::Color color) {
        Shapes::Rectangle bar;
        bar.Width(width * scale);
        bar.Height(height * scale);
        bar.RadiusX(1.5 * scale);
        bar.RadiusY(1.5 * scale);
        bar.Fill(MakeBrush(color));
        Canvas::SetLeft(bar, left * scale);
        Canvas::SetTop(bar, top * scale);
        canvas.Children().Append(bar);
    };
    addBar(7.5, 0.5, 3.0, 8.0, {255, 54, 197, 240});
    addBar(9.5, 7.5, 8.0, 3.0, {255, 46, 182, 125});
    addBar(7.5, 9.5, 3.0, 8.0, {255, 236, 178, 46});
    addBar(0.5, 7.5, 8.0, 3.0, {255, 224, 30, 90});
    logo.Children().Append(canvas);
    return logo;
}

static Grid BuildTeamsLogo(double size) {
    double scale = size / 18.0;
    Grid logo;
    logo.Width(size);
    logo.Height(size);
    logo.IsHitTestVisible(false);

    Canvas canvas;
    canvas.Width(size);
    canvas.Height(size);

    Shapes::Ellipse person;
    person.Width(5.0 * scale);
    person.Height(5.0 * scale);
    person.Fill(MakeBrush({255, 123, 131, 235}));
    Canvas::SetLeft(person, 11.5 * scale);
    Canvas::SetTop(person, 1.0 * scale);
    canvas.Children().Append(person);

    Shapes::Rectangle group;
    group.Width(7.5 * scale);
    group.Height(9.0 * scale);
    group.RadiusX(2.0 * scale);
    group.RadiusY(2.0 * scale);
    group.Fill(MakeBrush({255, 123, 131, 235}));
    Canvas::SetLeft(group, 9.5 * scale);
    Canvas::SetTop(group, 7.0 * scale);
    canvas.Children().Append(group);

    Shapes::Rectangle tile;
    tile.Width(11.5 * scale);
    tile.Height(12.5 * scale);
    tile.RadiusX(2.0 * scale);
    tile.RadiusY(2.0 * scale);
    tile.Fill(MakeBrush({255, 98, 100, 167}));
    Canvas::SetLeft(tile, 0.5 * scale);
    Canvas::SetTop(tile, 3.5 * scale);
    canvas.Children().Append(tile);

    TextBlock letter;
    letter.Text(L"T");
    letter.FontSize(10.0 * scale);
    letter.Foreground(MakeBrush({255, 255, 255, 255}));
    Canvas::SetLeft(letter, 3.0 * scale);
    Canvas::SetTop(letter, 2.7 * scale);
    canvas.Children().Append(letter);

    logo.Children().Append(canvas);
    return logo;
}

static Grid BuildZoomLogo(double size) {
    double scale = size / 18.0;
    Grid logo;
    logo.Width(size);
    logo.Height(size);
    logo.IsHitTestVisible(false);

    Canvas canvas;
    canvas.Width(size);
    canvas.Height(size);

    Shapes::Rectangle background;
    background.Width(17.0 * scale);
    background.Height(13.0 * scale);
    background.RadiusX(4.0 * scale);
    background.RadiusY(4.0 * scale);
    background.Fill(MakeBrush({255, 45, 140, 255}));
    Canvas::SetLeft(background, 0.5 * scale);
    Canvas::SetTop(background, 2.5 * scale);
    canvas.Children().Append(background);

    Shapes::Rectangle cameraBody;
    cameraBody.Width(8.0 * scale);
    cameraBody.Height(6.0 * scale);
    cameraBody.RadiusX(1.4 * scale);
    cameraBody.RadiusY(1.4 * scale);
    cameraBody.Fill(MakeBrush({255, 255, 255, 255}));
    Canvas::SetLeft(cameraBody, 3.0 * scale);
    Canvas::SetTop(cameraBody, 6.0 * scale);
    canvas.Children().Append(cameraBody);

    Shapes::Polygon cameraLens;
    cameraLens.Fill(MakeBrush({255, 255, 255, 255}));
    cameraLens.Points().Append({11.0f * static_cast<float>(scale),
                                7.0f * static_cast<float>(scale)});
    cameraLens.Points().Append({15.0f * static_cast<float>(scale),
                                5.0f * static_cast<float>(scale)});
    cameraLens.Points().Append({15.0f * static_cast<float>(scale),
                                13.0f * static_cast<float>(scale)});
    cameraLens.Points().Append({11.0f * static_cast<float>(scale),
                                11.0f * static_cast<float>(scale)});
    canvas.Children().Append(cameraLens);

    logo.Children().Append(canvas);
    return logo;
}

static Grid BuildMultipleCallsBadge(double size) {
    double badgeSize = std::max(7.0, size * 0.44);
    Grid badge;
    badge.Width(badgeSize);
    badge.Height(badgeSize);
    badge.HorizontalAlignment(HorizontalAlignment::Right);
    badge.VerticalAlignment(VerticalAlignment::Bottom);
    badge.IsHitTestVisible(false);

    Shapes::Ellipse background;
    background.Fill(MakeBrush({255, 32, 32, 32}));
    background.Stroke(MakeBrush({255, 255, 255, 255}));
    background.StrokeThickness(1.0);

    Shapes::Line horizontal;
    horizontal.X1(badgeSize * 0.25);
    horizontal.Y1(badgeSize * 0.5);
    horizontal.X2(badgeSize * 0.75);
    horizontal.Y2(badgeSize * 0.5);
    horizontal.Stroke(MakeBrush({255, 255, 255, 255}));
    horizontal.StrokeThickness(1.2);

    Shapes::Line vertical;
    vertical.X1(badgeSize * 0.5);
    vertical.Y1(badgeSize * 0.25);
    vertical.X2(badgeSize * 0.5);
    vertical.Y2(badgeSize * 0.75);
    vertical.Stroke(MakeBrush({255, 255, 255, 255}));
    vertical.StrokeThickness(1.2);

    badge.Children().Append(background);
    badge.Children().Append(horizontal);
    badge.Children().Append(vertical);
    return badge;
}

static std::wstring JoinAppNames(const std::vector<std::wstring>& names) {
    if (names.empty()) return L"";
    if (names.size() == 1) return names.front();
    if (names.size() == 2) return names[0] + L" and " + names[1];

    std::wstring result;
    for (size_t i = 0; i < names.size(); i++) {
        if (i != 0) {
            result += i + 1 == names.size() ? L", and " : L", ";
        }
        result += names[i];
    }
    return result;
}

static void UpdateWidgets() {
    if (!g_widgets) return;

    bool available = g_audioAvailable.load();
    bool muted = g_audioMuted.load();
    bool headsetAvailable = g_headsetAvailable.load();
    bool headsetMuted = g_headsetMuted.load();
    int volume = std::clamp(g_audioVolume.load(), 0, 100);
    bool slackWarning = g_slackWarningActive.load();
    bool teamsWarning = g_teamsWarningActive.load();
    bool zoomWarning = g_zoomWarningActive.load();
    bool slackActive = g_slackCallActive.load();
    bool teamsActive = g_teamsCallActive.load();
    bool zoomActive = g_zoomCallActive.load();
    bool slackMuted = g_slackMuted.load();
    bool teamsMuted = g_teamsMuted.load();
    bool zoomMuted = g_zoomMuted.load();
    bool zoomStateKnown = g_zoomStateKnown.load();
    float peak =
        available && !muted && !(headsetAvailable && headsetMuted)
            ? std::clamp(g_audioPeak.load(), 0.0f, 1.0f)
            : 0.0f;
    std::wstring name = GetSharedDeviceName();
    std::wstring tooltip;
    std::vector<std::wstring> warningApps;
    if (slackWarning) warningApps.push_back(L"Slack");
    if (teamsWarning) warningApps.push_back(L"Microsoft Teams");
    if (zoomWarning) warningApps.push_back(L"Zoom");
    if (!warningApps.empty()) {
        tooltip = L"You're speaking, but " + JoinAppNames(warningApps) +
                  (warningApps.size() == 1 ? L" is muted.\n\n"
                                           : L" are muted.\n\n");
    }
    tooltip += name + L"\nVolume: " +
                           std::to_wstring(volume) + L"%";
    if (g_forceVolume.load()) {
        tooltip += L" (locked at " +
                   std::to_wstring(g_forcedVolume.load()) + L"%)";
    }
    if (!available) {
        tooltip += L"\nUnavailable";
    } else if (muted) {
        tooltip += L" (Muted)";
    }
    HeadsetStatus headset = GetHeadsetStatus();
    tooltip += L"\nHeadset: ";
    tooltip += HeadsetMethodName(headset.method);
    tooltip += L"\nConfidence: ";
    tooltip += HeadsetConfidenceName(headset.confidence);
    if (headset.detected) {
        tooltip += L"\nHeadset state: ";
        tooltip += headset.stateKnown
                       ? (headset.muted ? L"Muted" : L"Unmuted")
                       : L"Button detected; latched state unknown";
    }
    tooltip += L"\n\nLeft-click: mute/unmute Windows input";
    std::vector<std::wstring> toggleApps;
    if (g_settings.slackRightClickToggle && slackActive) {
        toggleApps.push_back(L"Slack");
    }
    if (g_settings.teamsRightClickToggle && teamsActive) {
        toggleApps.push_back(L"Microsoft Teams");
    }
    if (g_settings.zoomRightClickToggle && zoomActive && zoomStateKnown) {
        toggleApps.push_back(L"Zoom");
    }
    if (!toggleApps.empty()) {
        tooltip += L"\nRight-click: mute/unmute " +
                   JoinAppNames(toggleApps);
    }

    std::vector<std::wstring> activeApps;
    std::wstring callTooltip = L"Call microphone";
    auto addCallState = [&](bool active, bool stateKnown, bool appMuted,
                            PCWSTR appName) {
        if (!active) return;
        activeApps.push_back(appName);
        callTooltip += L"\n" + std::wstring(appName) + L": " +
                       (!stateKnown ? L"Unknown"
                                    : appMuted ? L"Muted" : L"Unmuted");
    };
    addCallState(slackActive, true, slackMuted, L"Slack");
    addCallState(teamsActive, true, teamsMuted, L"Microsoft Teams");
    addCallState(zoomActive, zoomStateKnown, zoomMuted, L"Zoom");
    if (zoomActive && !zoomStateKnown) {
        callTooltip += L"\nShow Zoom's controls once to read its mute state.";
    }
    int selectedCallLogo = GetSelectedActiveCallIndex();
    PCWSTR selectedCallName =
        selectedCallLogo == 0   ? L"Slack"
        : selectedCallLogo == 1 ? L"Microsoft Teams"
        : selectedCallLogo == 2 ? L"Zoom"
                                : L"active call";
    if (!activeApps.empty()) {
        callTooltip += L"\n\nLeft-click: focus " +
                       std::wstring(selectedCallName);
    }
    if (!toggleApps.empty()) {
        callTooltip += L"\nRight-click: mute/unmute " +
                       JoinAppNames(toggleApps);
    }
    bool anyCallMuted = (slackActive && slackMuted) ||
                        (teamsActive && teamsMuted) ||
                        (zoomActive && zoomMuted);

    for (auto& widget : *g_widgets) {
        try {
            double size = static_cast<double>(g_settings.iconSize);
            double filledHeight = size * peak;
            widget.fillClip.Rect(
                {0.0f, static_cast<float>(size - filledHeight),
                 static_cast<float>(size), static_cast<float>(filledHeight)});
            widget.muteLine.Visibility(
                (!available || muted ||
                 (headsetAvailable && headsetMuted))
                    ? Visibility::Visible
                    : Visibility::Collapsed);
            widget.tooltipText.Text(tooltip);
            bool showCallIcon =
                g_settings.showCallStateIcon && !activeApps.empty();
            widget.callButton.Visibility(
                showCallIcon ? Visibility::Visible : Visibility::Collapsed);
            widget.callSlackLogo.Visibility(
                selectedCallLogo == 0 ? Visibility::Visible
                                      : Visibility::Collapsed);
            widget.callTeamsLogo.Visibility(
                selectedCallLogo == 1 ? Visibility::Visible
                                      : Visibility::Collapsed);
            widget.callZoomLogo.Visibility(
                selectedCallLogo == 2 ? Visibility::Visible
                                      : Visibility::Collapsed);
            widget.callLogoHost.Opacity(anyCallMuted ? 0.58 : 1.0);
            widget.callMultipleBadge.Visibility(
                activeApps.size() > 1 ? Visibility::Visible
                                      : Visibility::Collapsed);
            widget.callMuteLine.Visibility(
                anyCallMuted ? Visibility::Visible : Visibility::Collapsed);
            widget.callTooltipText.Text(callTooltip);
            bool shouldOpen = !warningApps.empty() &&
                              widget.taskbarWnd == g_mainTaskbarWnd.load();
            if (widget.warningOpen != shouldOpen) {
                widget.tooltip.IsOpen(shouldOpen);
                widget.warningOpen = shouldOpen;
            }
        } catch (...) {
            // The taskbar can replace its visual tree during resume or display
            // changes. The retry path will rebuild stale controls.
        }
    }
}

static Button BuildWidgetButton(WidgetState* state) {
    const double iconSize = static_cast<double>(g_settings.iconSize);
    auto accent = AccentColor();
    auto dim = winrt::Windows::UI::Color{150, 128, 128, 128};
    auto red = winrt::Windows::UI::Color{255, 232, 17, 35};
    auto transparent = winrt::Windows::UI::Color{0, 0, 0, 0};

    Grid iconGrid;
    iconGrid.Width(iconSize);
    iconGrid.Height(iconSize);

    auto baseLayer = BuildMicrophoneLayer(iconSize, MakeBrush(dim));
    auto fillLayer = BuildMicrophoneLayer(iconSize, MakeBrush(accent));
    RectangleGeometry fillClip;
    fillClip.Rect({0.0f, static_cast<float>(iconSize),
                   static_cast<float>(iconSize), 0.0f});
    fillLayer.Clip(fillClip);

    Shapes::Line muteLine;
    muteLine.X1(2.0);
    muteLine.Y1(2.0);
    muteLine.X2(iconSize - 2.0);
    muteLine.Y2(iconSize - 2.0);
    muteLine.Stroke(MakeBrush(red));
    muteLine.StrokeThickness(2.0);
    muteLine.Visibility(Visibility::Collapsed);

    iconGrid.Children().Append(baseLayer);
    iconGrid.Children().Append(fillLayer);
    iconGrid.Children().Append(muteLine);

    Button button;
    button.Name(L"MicrophoneActivityButton");
    button.Content(iconGrid);
    button.Width(static_cast<double>(g_settings.buttonWidth));
    button.Padding({0.0, 0.0, 0.0, 0.0});
    button.HorizontalAlignment(HorizontalAlignment::Center);
    button.VerticalAlignment(VerticalAlignment::Stretch);
    button.Background(MakeBrush(transparent));
    button.BorderThickness({0.0, 0.0, 0.0, 0.0});
    winrt::Windows::UI::Xaml::Automation::AutomationProperties::SetName(
        button, L"Microphone activity and input volume");

    TextBlock tooltipText;
    tooltipText.Text(L"Detecting microphone...");
    ToolTip tooltip;
    tooltip.Content(tooltipText);
    tooltip.PlacementTarget(button);
    tooltip.Placement(
        winrt::Windows::UI::Xaml::Controls::Primitives::PlacementMode::Top);
    ToolTipService::SetToolTip(button, tooltip);

    state->fillLayer = fillLayer;
    state->fillClip = fillClip;
    state->muteLine = muteLine;
    state->tooltipText = tooltipText;
    state->tooltip = tooltip;

    state->clickToken = button.Click([](auto const&, auto const&) {
        if (!g_unloading.load()) QueueMuteToggle();
    });
    state->wheelToken = button.PointerWheelChanged(
        [](auto const&, PointerRoutedEventArgs const& args) {
            if (g_unloading.load()) return;
            int delta =
                args.GetCurrentPoint(nullptr).Properties().MouseWheelDelta();
            if (delta != 0) {
                QueueVolumeNotches(delta / WHEEL_DELTA);
                args.Handled(true);
            }
        });
    state->rightTappedToken = button.RightTapped(
        [](auto const&, RightTappedRoutedEventArgs const& args) {
            if (g_unloading.load()) return;
            if (QueueActiveCallToggles()) args.Handled(true);
        });
    state->button = button;
    return button;
}

static Button BuildCallStateButton(WidgetState* state) {
    const double iconSize = static_cast<double>(g_settings.iconSize);
    auto red = winrt::Windows::UI::Color{255, 232, 17, 35};
    auto transparent = winrt::Windows::UI::Color{0, 0, 0, 0};

    Grid iconGrid;
    iconGrid.Width(iconSize);
    iconGrid.Height(iconSize);

    Grid logoHost;
    logoHost.Width(iconSize);
    logoHost.Height(iconSize);
    auto slackLogo = BuildSlackLogo(iconSize);
    auto teamsLogo = BuildTeamsLogo(iconSize);
    auto zoomLogo = BuildZoomLogo(iconSize);
    auto multipleBadge = BuildMultipleCallsBadge(iconSize);
    slackLogo.Visibility(Visibility::Collapsed);
    teamsLogo.Visibility(Visibility::Collapsed);
    zoomLogo.Visibility(Visibility::Collapsed);
    multipleBadge.Visibility(Visibility::Collapsed);
    logoHost.Children().Append(slackLogo);
    logoHost.Children().Append(teamsLogo);
    logoHost.Children().Append(zoomLogo);
    logoHost.Children().Append(multipleBadge);

    Shapes::Line muteLine;
    muteLine.X1(2.0);
    muteLine.Y1(2.0);
    muteLine.X2(iconSize - 2.0);
    muteLine.Y2(iconSize - 2.0);
    muteLine.Stroke(MakeBrush(red));
    muteLine.StrokeThickness(2.0);
    muteLine.Visibility(Visibility::Collapsed);

    iconGrid.Children().Append(logoHost);
    iconGrid.Children().Append(muteLine);

    Button button;
    button.Name(L"CallMicrophoneStateButton");
    button.Content(iconGrid);
    button.Width(static_cast<double>(g_settings.buttonWidth));
    button.Padding({0.0, 0.0, 0.0, 0.0});
    button.HorizontalAlignment(HorizontalAlignment::Center);
    button.VerticalAlignment(VerticalAlignment::Stretch);
    button.Background(MakeBrush(transparent));
    button.BorderThickness({0.0, 0.0, 0.0, 0.0});
    button.Visibility(Visibility::Collapsed);
    winrt::Windows::UI::Xaml::Automation::AutomationProperties::SetName(
        button, L"Active call microphone state");

    TextBlock tooltipText;
    tooltipText.Text(L"Detecting active calls...");
    ToolTip tooltip;
    tooltip.Content(tooltipText);
    tooltip.PlacementTarget(button);
    tooltip.Placement(
        winrt::Windows::UI::Xaml::Controls::Primitives::PlacementMode::Top);
    ToolTipService::SetToolTip(button, tooltip);

    state->callLogoHost = logoHost;
    state->callSlackLogo = slackLogo;
    state->callTeamsLogo = teamsLogo;
    state->callZoomLogo = zoomLogo;
    state->callMultipleBadge = multipleBadge;
    state->callMuteLine = muteLine;
    state->callTooltipText = tooltipText;
    state->callTooltip = tooltip;
    state->callClickToken = button.Click([](auto const&, auto const&) {
        if (!g_unloading.load()) FocusSelectedCallWindow();
    });
    state->callRightTappedToken = button.RightTapped(
        [](auto const&, RightTappedRoutedEventArgs const& args) {
            if (g_unloading.load()) return;
            if (QueueActiveCallToggles()) args.Handled(true);
        });
    state->callButton = button;
    return button;
}

static FrameworkElement FindDirectChild(Grid const& parent, PCWSTR name) {
    for (auto const& child : parent.Children()) {
        auto element = child.try_as<FrameworkElement>();
        if (element && element.Name() == name) return element;
    }
    return nullptr;
}

static bool InjectWidget(XamlRoot const& xamlRoot, HWND taskbarWnd) {
    auto root = xamlRoot.Content().try_as<FrameworkElement>();
    if (!root) return false;

    auto parent = FindChildRecursive(
                      root, [](FrameworkElement const& element) {
                          return element.Name() == L"SystemTrayFrameGrid";
                      })
                      .try_as<Grid>();
    if (!parent || parent.ActualHeight() <= 0.0) return false;

    if (FindDirectChild(parent, L"MicrophoneActivityWidget")) {
        return true;
    }

    FrameworkElement anchor = nullptr;
    bool afterAnchor = false;
    if (g_settings.position == L"beforeOmni") {
        anchor = FindDirectChild(parent, L"ControlCenterButton");
    } else if (g_settings.position == L"beforeClock") {
        anchor = FindDirectChild(parent, L"NotificationCenterButton");
    } else if (g_settings.position == L"afterClock") {
        anchor = FindDirectChild(parent, L"ShowDesktopStack");
    } else if (g_settings.position == L"afterShowDesktop") {
        anchor = FindDirectChild(parent, L"ShowDesktopStack");
        afterAnchor = true;
    } else if (g_settings.position != L"beforeIcons") {
        return false;
    }

    if (g_settings.position != L"beforeIcons" && !anchor) return false;

    int insertColumn = 0;
    if (anchor) insertColumn = Grid::GetColumn(anchor);
    if (afterAnchor) insertColumn++;

    ColumnDefinition definition;
    definition.Width({1.0, GridUnitType::Auto});
    if (static_cast<uint32_t>(insertColumn) <
        parent.ColumnDefinitions().Size()) {
        parent.ColumnDefinitions().InsertAt(insertColumn, definition);
    } else {
        parent.ColumnDefinitions().Append(definition);
    }

    for (auto const& child : parent.Children()) {
        auto element = child.try_as<FrameworkElement>();
        if (!element) continue;
        int column = Grid::GetColumn(element);
        int span = Grid::GetColumnSpan(element);
        if (column >= insertColumn) {
            Grid::SetColumn(element, column + 1);
        } else if (column + span > insertColumn) {
            Grid::SetColumnSpan(element, span + 1);
        }
    }

    WidgetState state;
    state.taskbarWnd = taskbarWnd;
    state.parent = parent;
    state.column = insertColumn;

    StackPanel widget;
    widget.Name(L"MicrophoneActivityWidget");
    widget.Orientation(Orientation::Horizontal);
    widget.VerticalAlignment(VerticalAlignment::Stretch);
    auto button = BuildWidgetButton(&state);
    auto callButton = BuildCallStateButton(&state);
    widget.Children().Append(button);
    widget.Children().Append(callButton);
    Grid::SetColumn(widget, insertColumn);
    parent.Children().Append(widget);

    g_widgets->push_back(std::move(state));
    Wh_Log(L"[XAML] Widget injected into taskbar %p at column %d",
           taskbarWnd, insertColumn);
    return true;
}

static void RemoveWidgetState(WidgetState& state) {
    try {
        state.button.Click(state.clickToken);
        state.button.PointerWheelChanged(state.wheelToken);
        state.button.RightTapped(state.rightTappedToken);
        state.callButton.Click(state.callClickToken);
        state.callButton.RightTapped(state.callRightTappedToken);
        state.tooltip.IsOpen(false);
        state.callTooltip.IsOpen(false);
        ToolTipService::SetToolTip(
            state.button, winrt::Windows::Foundation::IInspectable{nullptr});
        state.tooltip.Content(nullptr);
        state.button.Content(nullptr);
        ToolTipService::SetToolTip(
            state.callButton,
            winrt::Windows::Foundation::IInspectable{nullptr});
        state.callTooltip.Content(nullptr);
        state.callButton.Content(nullptr);

        int liveColumn = state.column;
        for (uint32_t i = 0; i < state.parent.Children().Size(); i++) {
            auto element = state.parent.Children().GetAt(i)
                               .try_as<FrameworkElement>();
            if (element && element.Name() == L"MicrophoneActivityWidget") {
                liveColumn = Grid::GetColumn(element);
                state.parent.Children().RemoveAt(i);
                break;
            }
        }

        if (liveColumn >= 0 &&
            static_cast<uint32_t>(liveColumn) <
                state.parent.ColumnDefinitions().Size()) {
            state.parent.ColumnDefinitions().RemoveAt(liveColumn);
            for (auto const& child : state.parent.Children()) {
                auto element = child.try_as<FrameworkElement>();
                if (!element) continue;
                int column = Grid::GetColumn(element);
                int span = Grid::GetColumnSpan(element);
                if (column > liveColumn) {
                    Grid::SetColumn(element, column - 1);
                } else if (column < liveColumn &&
                           column + span > liveColumn) {
                    Grid::SetColumnSpan(element, span - 1);
                }
            }
        }
    } catch (...) {
        Wh_Log(L"[XAML] Widget tree was already replaced");
    }
}

static void RemoveAllWidgets() {
    g_widgetsLive.store(false);
    if (!g_widgets) return;
    for (auto& state : *g_widgets) {
        RemoveWidgetState(state);
    }
    g_widgets->clear();
}

static constexpr UINT_PTR kTaskbarSubclassId = 0x4D494357;

static LRESULT CALLBACK TaskbarSubclassProc(HWND hWnd, UINT message,
                                            WPARAM wParam, LPARAM lParam,
                                            UINT_PTR subclassId,
                                            DWORD_PTR referenceData) {
    if (message == AudioUpdateMessage()) {
        UpdateWidgets();
        return 0;
    }
    if (message == WM_NCDESTROY) {
        RemoveWindowSubclass(hWnd, TaskbarSubclassProc, kTaskbarSubclassId);
    }
    return DefSubclassProc(hWnd, message, wParam, lParam);
}

static bool ApplyWidgetsFromTaskbarThread() {
    RemoveAllWidgets();
    bool injected = false;

    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            WCHAR className[32];
            if (!GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
                return TRUE;
            }

            XamlRoot root = nullptr;
            if (_wcsicmp(className, L"Shell_TrayWnd") == 0) {
                root = GetTaskbarXamlRoot(hWnd);
                g_mainTaskbarWnd.store(hWnd);
                SetWindowSubclass(hWnd, TaskbarSubclassProc,
                                  kTaskbarSubclassId, 0);
            } else if (_wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0) {
                root = GetSecondaryTaskbarXamlRoot(hWnd);
            } else {
                return TRUE;
            }

            if (root && InjectWidget(root, hWnd)) {
                *reinterpret_cast<bool*>(lParam) = true;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&injected));

    UpdateWidgets();
    g_widgetsLive.store(injected);
    return injected;
}

static void ApplyWidgetsOnWindowThread() {
    HWND hWnd = FindMainTaskbarWindow();
    if (!hWnd) return;
    g_mainTaskbarWnd.store(hWnd);
    RunFromWindowThread(hWnd,
                        [](void*) { ApplyWidgetsFromTaskbarThread(); },
                        nullptr);
}

static bool RemoveWidgetsOnWindowThread() {
    HWND hWnd = FindMainTaskbarWindow();
    if (!hWnd) return false;
    return RunFromWindowThread(
        hWnd,
        [](void*) {
            RemoveAllWidgets();
            HWND mainWindow = g_mainTaskbarWnd.exchange(nullptr);
            if (mainWindow) {
                RemoveWindowSubclass(mainWindow, TaskbarSubclassProc,
                                     kTaskbarSubclassId);
            }
        },
        nullptr);
}

// -----------------------------------------------------------------------------
// Taskbar restart hooks and bounded retry
// -----------------------------------------------------------------------------

static HANDLE g_retryThread = nullptr;
static HANDLE g_retryStopEvent = nullptr;
static SRWLOCK g_retryLock = SRWLOCK_INIT;

static void StopRetryThread() {
    AcquireSRWLockExclusive(&g_retryLock);
    HANDLE thread = g_retryThread;
    HANDLE stopEvent = g_retryStopEvent;
    g_retryThread = nullptr;
    g_retryStopEvent = nullptr;
    if (stopEvent) SetEvent(stopEvent);
    ReleaseSRWLockExclusive(&g_retryLock);

    if (thread) {
        DWORD result;
        do {
            result = MsgWaitForMultipleObjects(1, &thread, FALSE, INFINITE,
                                               QS_SENDMESSAGE);
            if (result == WAIT_OBJECT_0 + 1) {
                MSG message;
                PeekMessageW(&message, nullptr, 0, 0, PM_NOREMOVE);
            }
        } while (result == WAIT_OBJECT_0 + 1);
        CloseHandle(thread);
    }
    if (stopEvent) CloseHandle(stopEvent);
}

static void StartRetryThread() {
    StopRetryThread();
    AcquireSRWLockExclusive(&g_retryLock);
    if (g_unloading.load()) {
        ReleaseSRWLockExclusive(&g_retryLock);
        return;
    }

    g_retryStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_retryStopEvent) {
        ReleaseSRWLockExclusive(&g_retryLock);
        return;
    }

    HANDLE stopEvent = g_retryStopEvent;
    g_retryThread = CreateThread(
        nullptr, 0,
        [](void* parameter) -> DWORD {
            HANDLE stopEvent = static_cast<HANDLE>(parameter);
            static constexpr DWORD delays[] = {0,    500,  1000, 2000,
                                                4000, 8000, 15000, 30000};
            for (DWORD delay : delays) {
                if (delay &&
                    WaitForSingleObject(stopEvent, delay) != WAIT_TIMEOUT) {
                    break;
                }
                if (g_unloading.load()) break;
                ApplyWidgetsOnWindowThread();
                if (g_widgetsLive.load()) break;
            }
            return 0;
        },
        stopEvent, 0, nullptr);

    if (!g_retryThread) {
        CloseHandle(g_retryStopEvent);
        g_retryStopEvent = nullptr;
    }
    ReleaseSRWLockExclusive(&g_retryLock);
}

using TrayUI_StartTaskbar_t = void(WINAPI*)(void*);
using CSecondaryTray_GetTrayWindow_t = HWND(WINAPI*)(void*);
using CSecondaryTray_InitModelAndHost_t = void(WINAPI*)(void*, void*);

static TrayUI_StartTaskbar_t TrayUI_StartTaskbar_Original = nullptr;
static CSecondaryTray_GetTrayWindow_t CSecondaryTray_GetTrayWindow_Original =
    nullptr;
static CSecondaryTray_InitModelAndHost_t
    CSecondaryTray_InitModelAndHost_Original = nullptr;

static void WINAPI TrayUI_StartTaskbar_Hook(void* self) {
    TrayUI_StartTaskbar_Original(self);
    if (!g_unloading.load()) StartRetryThread();
}

static void WINAPI CSecondaryTray_InitModelAndHost_Hook(void* self,
                                                        void* taskbarModel) {
    CSecondaryTray_InitModelAndHost_Original(self, taskbarModel);
    if (!g_unloading.load()) StartRetryThread();
}

static bool HookTaskbarSymbols() {
    HMODULE module =
        LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) return false;

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {{LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
         &CTaskBand_ITaskListWndSite_vftable},
        {{LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"},
         &CSecondaryTaskBand_ITaskListWndSite_vftable},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
         &CTaskBand_GetTaskbarHost_Original},
        {{LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
         &TaskbarHost_FrameHeight_Original},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
         &CSecondaryTaskBand_GetTaskbarHost_Original},
        {{LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
         &std__Ref_count_base__Decref_Original},
        {{LR"(public: virtual void __cdecl TrayUI::StartTaskbar(void))"},
         &TrayUI_StartTaskbar_Original, TrayUI_StartTaskbar_Hook},
        {{LR"(public: virtual struct HWND__ * __cdecl CSecondaryTray::GetTrayWindow(void))"},
         &CSecondaryTray_GetTrayWindow_Original},
        {{LR"(public: virtual void __cdecl CSecondaryTray::InitModelAndHost(struct winrt::WindowsUdk::UI::Shell::TaskbarModel))"},
         &CSecondaryTray_InitModelAndHost_Original,
         CSecondaryTray_InitModelAndHost_Hook},
    };
    return WindhawkUtils::HookSymbols(module, taskbarDllHooks,
                                      ARRAYSIZE(taskbarDllHooks));
}

// -----------------------------------------------------------------------------
// Windhawk lifecycle
// -----------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"[Init] Microphone Activity Taskbar Widget 0.9.0");
    g_diagnosticStartTime = GetTickCount64();
    LoadSettings();
    if (!HookTaskbarSymbols()) {
        Wh_Log(L"[Init] Failed to resolve taskbar.dll symbols");
        return FALSE;
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    StartAudioThread();
    ApplyWidgetsOnWindowThread();
    if (!g_widgetsLive.load()) StartRetryThread();
}

void Wh_ModUninit() {
    g_unloading.store(true);
    StopRetryThread();
    StopAudioThread();
    if (RemoveWidgetsOnWindowThread() && g_widgets) {
        g_widgets.reset();
    } else {
        // The vector has no_destroy storage. If Explorer has already torn down
        // its UI thread, retain stale WinRT handles instead of releasing XAML
        // objects from Windhawk's callback thread after the DLL unloads.
        Wh_Log(L"[Uninit] Taskbar UI unavailable; retaining XAML handles");
    }
}

void Wh_ModSettingsChanged() {
    StopRetryThread();
    RemoveWidgetsOnWindowThread();
    StopAudioThread();
    LoadSettings();
    StartAudioThread();
    ApplyWidgetsOnWindowThread();
    if (!g_widgetsLive.load()) StartRetryThread();
}
