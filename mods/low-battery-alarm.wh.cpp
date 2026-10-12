// ==WindhawkMod==
// @id              low-battery-alarm
// @name            Low Battery Alarm
// @description     Plays a customizable alarm when a laptop battery reaches a low or critical charge level
// @version         1.0.0
// @author          communism420
// @github          https://github.com/communism420
// @homepage        https://github.com/communism420
// @include         windhawk.exe
// @compilerOptions -lwinmm -lole32 -luuid -ladvapi32 -lshell32 -lgdi32 -lcomdlg32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Low Battery Alarm

Low Battery Alarm plays a repeating, clearly audible alarm when a laptop is
discharging and reaches a configured low or critical battery level. It is useful
when the laptop is left performing a long-running task in another room and a
normal desktop notification might not be noticed.

By default, low-battery alerts begin at 10% and repeat every 120 seconds.
Critical alerts begin at 5% and repeat every 30 seconds. Critical alerts continue
below 5%; 5% is the entry threshold, not a lower cutoff. Alerts stop when external
power is connected and can start again during a later discharge cycle. AC/DC
changes are handled through Windows power notifications, so an active waveform is
stopped immediately instead of waiting for the next polling interval.

## Custom sound and fallback

Enter the complete WAV path manually in **WAV file path**, or use **Browse...**
in the custom configuration window to select it with Windows Explorer. A manual
path change saved in Windhawk replaces a path previously selected in the custom
window. To open the window, enable **Open alarm configuration** and save the
settings. The topmost window also lists the Windows default output roles and
every active Core Audio output device. It stores the selected device by its
stable endpoint ID. To reopen the window, disable the option and save, then
enable it and save again. An enabled request is handled only once, so saving
unrelated settings does not reopen the window. It also opens automatically on
the first run, before an output selection has been stored.

WAV is the guaranteed supported custom format. If no file has been selected, or
if the selected file is later removed, becomes inaccessible, is not a WAV file,
or cannot be played, the mod logs the reason. When the fallback option is
enabled, the Windows critical system sound is used instead. A short, clearly
audible, and recognizable WAV file is recommended.

## Volume and testing

The mod can temporarily change the selected output device's master volume and can
temporarily unmute it while an alert is playing. The previous volume and mute
state are normally restored afterward. If the user changes the volume during
playback, the new value is preserved when the change can be detected by comparing
it with the exact value applied by the mod.

Enable **Test sound after settings change** and save the settings to request one
test alert. The test uses the current sound and volume settings without changing
the battery state machine. The option is evaluated only when settings are saved;
it does not run during normal polling.

Desktop computers without a system battery are ignored. The mod creates no
persistent UI, console, tray icon, or external player. It runs in a dedicated
Windhawk tool process and uses low-overhead battery-monitoring, power-notification,
and controlled sound-playback threads.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- LowBatteryThreshold: 10
  $name: Low battery threshold
  $description: Battery percentage at which low-battery alerts begin.
- CriticalBatteryThreshold: 5
  $name: Critical battery threshold
  $description: Battery percentage at which critical alerts begin.
- PollingIntervalSeconds: 15
  $name: Battery polling interval (seconds)
  $description: How often the battery status is checked, in seconds.
- LowRepeatIntervalSeconds: 120
  $name: Low-battery repeat interval (seconds)
  $description: Delay between repeated alerts in the low-battery state, in seconds.
- CriticalRepeatIntervalSeconds: 30
  $name: Critical-battery repeat interval (seconds)
  $description: Delay between repeated alerts in the critical-battery state, in seconds.
- SoundFilePath: ""
  $name: WAV file path
  $description: Full path to the WAV audio file. Saving a manual change replaces a file selected in the custom configuration window. Leave empty to use the Windows fallback sound.
- ConfigureAudioOutput: false
  $name: Open alarm configuration
  $description: Enable and save to open a topmost window for selecting a WAV file and an active Windows output device. The request is handled only once; to reopen the window, disable and save, then enable and save again.
- RepetitionsPerAlert: 3
  $name: Repetitions per alert
  $description: Number of times the sound is played for each alert.
- PauseBetweenRepetitionsMilliseconds: 500
  $name: Pause between repetitions (milliseconds)
  $description: Pause between sound repetitions in milliseconds.
- UseSystemSoundIfFileInvalid: true
  $name: Use Windows fallback sound
  $description: Use a Windows critical system sound if the custom WAV file cannot be played.
- TestSoundAfterSettingsChange: false
  $name: Test sound after settings change
  $description: Play one test alert after settings are saved.
- TemporarilyIncreaseVolume: true
  $name: Temporarily increase volume
  $description: Temporarily increase the selected output-device volume while the alarm is playing.
- AlarmVolumePercent: 100
  $name: Alarm volume
  $description: Temporary volume level used for the alarm.
- TemporarilyUnmute: true
  $name: Temporarily unmute
  $description: Temporarily unmute the selected output device while the alarm is playing.
- RestorePreviousVolume: true
  $name: Restore previous audio state
  $description: Restore the previous volume and mute state after the alarm finishes.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <audioclient.h>
#include <commdlg.h>
#include <endpointvolume.h>
#include <functiondiscoverykeys_devpkey.h>
#include <mmdeviceapi.h>
#include <mmsystem.h>
#include <objbase.h>
#include <propsys.h>
#include <shellapi.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstring>
#include <cwctype>
#include <limits>
#include <string>
#include <utility>
#include <vector>

namespace {

constexpr DWORD kShutdownDiagnosticTimeoutMilliseconds = 10000;
constexpr DWORD kInitialEvaluationTimeoutMilliseconds = 5000;
constexpr DWORD kFallbackSoundDurationMilliseconds = 2000;
constexpr DWORD kMaximumWaveFileBytes = 64 * 1024 * 1024;
constexpr ULONGLONG kActivePlaybackPowerCheckMilliseconds = 500;
constexpr float kVolumeComparisonTolerance = 0.005f;
constexpr ULONGLONG kNoDeadline = std::numeric_limits<ULONGLONG>::max();
constexpr wchar_t kPowerNotificationWindowClass[] =
    L"WindhawkLowBatteryAlarmPowerNotification";
constexpr wchar_t kStoredAudioOutputMode[] = L"SelectedAudioOutputMode";
constexpr wchar_t kStoredAudioEndpointId[] = L"SelectedAudioEndpointId";
constexpr wchar_t kStoredSoundFilePath[] = L"SelectedSoundFilePath";
constexpr wchar_t kStoredConfigurationTriggerState[] =
    L"ConfigurationTriggerState";

enum class BatteryAlertState {
    Normal,
    Low,
    Critical
};

enum class PlaybackKind {
    Low,
    Critical,
    Test
};

enum class AudioOutputMode {
    WindowsDefault = 0,
    Multimedia = 1,
    Communications = 2,
    EndpointId = 3
};

struct Settings {
    int lowBatteryThreshold = 10;
    int criticalBatteryThreshold = 5;
    int pollingIntervalSeconds = 15;
    int lowRepeatIntervalSeconds = 120;
    int criticalRepeatIntervalSeconds = 30;
    std::wstring configuredSoundFilePath;
    std::wstring pickerSoundFilePath;
    std::wstring soundFilePath;
    bool configureAudioOutput = false;
    bool hasStoredAudioOutputSelection = false;
    AudioOutputMode audioOutputMode = AudioOutputMode::WindowsDefault;
    std::wstring audioOutputDevice;
    int repetitionsPerAlert = 3;
    int pauseBetweenRepetitionsMilliseconds = 500;
    bool useSystemSoundIfFileInvalid = true;
    bool testSoundAfterSettingsChange = false;
    bool temporarilyIncreaseVolume = true;
    int alarmVolumePercent = 100;
    bool temporarilyUnmute = true;
    bool restorePreviousVolume = true;
};

struct PlaybackRequest {
    PlaybackKind kind = PlaybackKind::Test;
    Settings settings;
};

SRWLOCK g_settingsLock = SRWLOCK_INIT;
Settings g_settings;

SRWLOCK g_playbackLock = SRWLOCK_INIT;
// This lock serializes cancellation control calls with request replacement.
SRWLOCK g_playbackControlLock = SRWLOCK_INIT;
PlaybackRequest g_pendingPlayback;
bool g_hasPendingPlayback = false;
bool g_playbackActive = false;

HANDLE g_powerNotificationThread = nullptr;
HANDLE g_powerNotificationReadyEvent = nullptr;
HANDLE g_powerSourceChangedEvent = nullptr;
std::atomic<HWND> g_powerNotificationWindow{nullptr};
std::atomic<bool> g_powerNotificationInitialized{false};
std::atomic<bool> g_externalPowerConnected{false};
std::atomic<int> g_lastNotifiedPowerCondition{-1};

HANDLE g_shutdownEvent = nullptr;
// Settings changes and playback requests are auto-reset wake-up events.
HANDLE g_settingsChangedEvent = nullptr;
// Playback cancellation stays signaled until the playback thread accepts a new request.
HANDLE g_playbackCancelEvent = nullptr;
HANDLE g_playbackRequestEvent = nullptr;
HANDLE g_playbackFinishedEvent = nullptr;
// Initialization waits for the monitor's first state-machine evaluation.
HANDLE g_initialEvaluationEvent = nullptr;
HANDLE g_monitorThread = nullptr;
HANDLE g_playbackThread = nullptr;
std::atomic<bool> g_stopping{false};
std::atomic<bool> g_initialEvaluationCompleted{false};
bool g_workersJoined = false;

SRWLOCK g_audioPickerLock = SRWLOCK_INIT;
HANDLE g_audioPickerThread = nullptr;
HWND g_audioPickerWindow = nullptr;

constexpr wchar_t kAudioPickerWindowClass[] =
    L"WindhawkLowBatteryAlarmAudioPicker";
constexpr UINT kAudioPickerBringToFrontMessage = WM_APP + 1;
constexpr int kAudioPickerComboId = 1001;
constexpr int kAudioPickerHintId = 1002;
constexpr int kAudioPickerBrowseId = 1003;
constexpr int kAudioPickerSoundPathId = 1004;

template <typename T>
class ComPtr {
public:
    ComPtr() = default;

    ~ComPtr() {
        Reset();
    }

    ComPtr(const ComPtr&) = delete;
    ComPtr& operator=(const ComPtr&) = delete;

    T* Get() const {
        return pointer_;
    }

    T** Put() {
        Reset();
        return &pointer_;
    }

    T* operator->() const {
        return pointer_;
    }

    explicit operator bool() const {
        return pointer_ != nullptr;
    }

    void Reset() {
        if (pointer_) {
            pointer_->Release();
            pointer_ = nullptr;
        }
    }

private:
    T* pointer_ = nullptr;
};

void LogWin32Failure(PCWSTR operation, DWORD error) {
    Wh_Log(L"%s failed. GetLastError=%lu.", operation, error);
}

void LogHResultFailure(PCWSTR operation, HRESULT result) {
    Wh_Log(L"%s failed. HRESULT=0x%08lX.", operation,
           static_cast<unsigned long>(result));
}

int ClampAndLog(PCWSTR settingName, int value, int minimum, int maximum) {
    int clamped = value;
    if (clamped < minimum) {
        clamped = minimum;
    } else if (clamped > maximum) {
        clamped = maximum;
    }

    if (clamped != value) {
        Wh_Log(L"Setting %s was clamped from %d to %d.", settingName, value,
               clamped);
    }

    return clamped;
}

bool LoadStringSetting(PCWSTR settingName, std::wstring* value) {
    if (!settingName || !value) {
        return false;
    }

    PCWSTR settingValue = Wh_GetStringSetting(settingName);
    bool succeeded = false;
    try {
        value->assign(settingValue ? settingValue : L"");
        succeeded = true;
    } catch (...) {
        Wh_Log(L"An exception occurred while loading string setting %s.",
               settingName);
    }
    Wh_FreeStringSetting(settingValue);
    return succeeded;
}

AudioOutputMode LoadStoredAudioOutputMode(bool* hasStoredSelection) {
    if (hasStoredSelection) {
        *hasStoredSelection = false;
    }
    int storedMode = Wh_GetIntValue(
        kStoredAudioOutputMode, -1);
    if (storedMode == -1) {
        return AudioOutputMode::WindowsDefault;
    }
    if (storedMode < static_cast<int>(AudioOutputMode::WindowsDefault) ||
        storedMode > static_cast<int>(AudioOutputMode::EndpointId)) {
        Wh_Log(L"The stored audio output mode is invalid; using the Windows "
               L"default output.");
        return AudioOutputMode::WindowsDefault;
    }
    if (hasStoredSelection) {
        *hasStoredSelection = true;
    }
    return static_cast<AudioOutputMode>(storedMode);
}

bool LoadStoredStringValue(PCWSTR valueName,
                           size_t maximumCharacters,
                           PCWSTR description,
                           std::wstring* value) {
    if (!valueName || maximumCharacters == 0 || !description || !value) {
        return false;
    }

    std::vector<wchar_t> buffer(maximumCharacters);
    size_t length =
        Wh_GetStringValue(valueName, buffer.data(), buffer.size());
    if (length >= buffer.size()) {
        Wh_Log(L"The stored %s is too long and was ignored.", description);
        value->clear();
        return true;
    }

    value->assign(buffer.data(), length);
    return true;
}

bool LoadSettings(Settings* loadedSettings) {
    if (!loadedSettings) {
        return false;
    }

    try {
        Settings settings;
        settings.lowBatteryThreshold = ClampAndLog(
            L"LowBatteryThreshold", Wh_GetIntSetting(L"LowBatteryThreshold"),
            1, 100);
        settings.criticalBatteryThreshold = ClampAndLog(
            L"CriticalBatteryThreshold",
            Wh_GetIntSetting(L"CriticalBatteryThreshold"), 1, 100);
        if (settings.criticalBatteryThreshold > settings.lowBatteryThreshold) {
            Wh_Log(L"CriticalBatteryThreshold was clamped to the effective "
                   L"LowBatteryThreshold value of %d.",
                   settings.lowBatteryThreshold);
            settings.criticalBatteryThreshold = settings.lowBatteryThreshold;
        }

        settings.pollingIntervalSeconds = ClampAndLog(
            L"PollingIntervalSeconds",
            Wh_GetIntSetting(L"PollingIntervalSeconds"), 5, 3600);
        settings.lowRepeatIntervalSeconds = ClampAndLog(
            L"LowRepeatIntervalSeconds",
            Wh_GetIntSetting(L"LowRepeatIntervalSeconds"), 1, 86400);
        settings.criticalRepeatIntervalSeconds = ClampAndLog(
            L"CriticalRepeatIntervalSeconds",
            Wh_GetIntSetting(L"CriticalRepeatIntervalSeconds"), 1, 86400);

        if (!LoadStringSetting(L"SoundFilePath",
                               &settings.configuredSoundFilePath) ||
            !LoadStoredStringValue(kStoredSoundFilePath, 32768,
                                   L"sound file path",
                                   &settings.pickerSoundFilePath) ||
            !LoadStoredStringValue(kStoredAudioEndpointId, 4096,
                                   L"audio endpoint ID",
                                   &settings.audioOutputDevice)) {
            return false;
        }
        settings.soundFilePath = settings.pickerSoundFilePath.empty()
                                     ? settings.configuredSoundFilePath
                                     : settings.pickerSoundFilePath;
        settings.configureAudioOutput =
            Wh_GetIntSetting(L"ConfigureAudioOutput") != 0;
        settings.audioOutputMode = LoadStoredAudioOutputMode(
            &settings.hasStoredAudioOutputSelection);

        settings.repetitionsPerAlert = ClampAndLog(
            L"RepetitionsPerAlert",
            Wh_GetIntSetting(L"RepetitionsPerAlert"), 1, 10);
        settings.pauseBetweenRepetitionsMilliseconds = ClampAndLog(
            L"PauseBetweenRepetitionsMilliseconds",
            Wh_GetIntSetting(L"PauseBetweenRepetitionsMilliseconds"), 0,
            60000);
        settings.useSystemSoundIfFileInvalid =
            Wh_GetIntSetting(L"UseSystemSoundIfFileInvalid") != 0;
        settings.testSoundAfterSettingsChange =
            Wh_GetIntSetting(L"TestSoundAfterSettingsChange") != 0;
        settings.temporarilyIncreaseVolume =
            Wh_GetIntSetting(L"TemporarilyIncreaseVolume") != 0;
        settings.alarmVolumePercent = ClampAndLog(
            L"AlarmVolumePercent", Wh_GetIntSetting(L"AlarmVolumePercent"), 1,
            100);
        settings.temporarilyUnmute =
            Wh_GetIntSetting(L"TemporarilyUnmute") != 0;
        settings.restorePreviousVolume =
            Wh_GetIntSetting(L"RestorePreviousVolume") != 0;

        *loadedSettings = std::move(settings);
        return true;
    } catch (...) {
        Wh_Log(L"An exception occurred while loading settings.");
        return false;
    }
}

bool InitializeConfigurationTrigger(const Settings& settings) {
    int storedState = Wh_GetIntValue(kStoredConfigurationTriggerState, -1);
    bool requestConfiguration =
        !settings.hasStoredAudioOutputSelection ||
        (storedState == 0 && settings.configureAudioOutput);

    if (!Wh_SetIntValue(kStoredConfigurationTriggerState,
                        settings.configureAudioOutput ? 1 : 0)) {
        Wh_Log(L"Unable to initialize the alarm configuration trigger state.");
    }
    return requestConfiguration;
}

bool ConsumeConfigurationTrigger(const Settings& previousSettings,
                                 const Settings& updatedSettings) {
    int fallbackState = previousSettings.configureAudioOutput ? 1 : 0;
    int storedState =
        Wh_GetIntValue(kStoredConfigurationTriggerState, fallbackState);
    bool requestConfiguration =
        updatedSettings.configureAudioOutput && storedState == 0;

    if (!Wh_SetIntValue(kStoredConfigurationTriggerState,
                        updatedSettings.configureAudioOutput ? 1 : 0)) {
        Wh_Log(L"Unable to update the alarm configuration trigger state.");
    }
    return requestConfiguration;
}

bool ReplaceSettings(Settings settings) {
    AcquireSRWLockExclusive(&g_settingsLock);
    try {
        g_settings = std::move(settings);
    } catch (...) {
        ReleaseSRWLockExclusive(&g_settingsLock);
        Wh_Log(L"Unable to allocate memory while replacing settings.");
        return false;
    }
    ReleaseSRWLockExclusive(&g_settingsLock);
    return true;
}

bool GetSettingsSnapshot(Settings* snapshot) {
    if (!snapshot) {
        return false;
    }

    AcquireSRWLockShared(&g_settingsLock);
    try {
        *snapshot = g_settings;
    } catch (...) {
        ReleaseSRWLockShared(&g_settingsLock);
        Wh_Log(L"An exception occurred while copying the settings snapshot.");
        return false;
    }
    ReleaseSRWLockShared(&g_settingsLock);
    return true;
}

bool PlaybackSettingsDiffer(const Settings& first, const Settings& second) {
    return first.soundFilePath != second.soundFilePath ||
           first.audioOutputMode != second.audioOutputMode ||
           first.audioOutputDevice != second.audioOutputDevice ||
           first.repetitionsPerAlert != second.repetitionsPerAlert ||
           first.pauseBetweenRepetitionsMilliseconds !=
               second.pauseBetweenRepetitionsMilliseconds ||
           first.useSystemSoundIfFileInvalid !=
               second.useSystemSoundIfFileInvalid ||
           first.temporarilyIncreaseVolume !=
               second.temporarilyIncreaseVolume ||
           first.alarmVolumePercent != second.alarmVolumePercent ||
           first.temporarilyUnmute != second.temporarilyUnmute ||
           first.restorePreviousVolume != second.restorePreviousVolume;
}

ULONGLONG SecondsToTicks(int seconds) {
    return static_cast<ULONGLONG>(seconds) * 1000ULL;
}

DWORD TicksUntil(ULONGLONG now, ULONGLONG deadline) {
    if (deadline <= now) {
        return 0;
    }

    ULONGLONG difference = deadline - now;
    if (difference >= static_cast<ULONGLONG>(INFINITE)) {
        return INFINITE - 1;
    }

    return static_cast<DWORD>(difference);
}

PCWSTR PlaybackKindName(PlaybackKind kind) {
    switch (kind) {
        case PlaybackKind::Low:
            return L"low-battery";
        case PlaybackKind::Critical:
            return L"critical-battery";
        case PlaybackKind::Test:
            return L"test";
    }

    return L"unknown";
}

int PlaybackPriority(PlaybackKind kind) {
    switch (kind) {
        case PlaybackKind::Critical:
            return 3;
        case PlaybackKind::Low:
            return 2;
        case PlaybackKind::Test:
            return 1;
    }

    return 0;
}

void StopActiveWaveformWhileControlLocked() {
    // WASAPI playback watches g_playbackCancelEvent and stops on its owning
    // playback thread. PlaySoundW is only used by the final system-alias fallback.
    PlaySoundW(nullptr, nullptr, 0);
}

void StopActiveWaveform() {
    AcquireSRWLockExclusive(&g_playbackControlLock);
    StopActiveWaveformWhileControlLocked();
    ReleaseSRWLockExclusive(&g_playbackControlLock);
}

bool IsPlaybackBusy() {
    AcquireSRWLockShared(&g_playbackLock);
    bool busy = g_playbackActive || g_hasPendingPlayback;
    ReleaseSRWLockShared(&g_playbackLock);
    return busy;
}

bool QueuePlayback(PlaybackKind kind,
                   const Settings& settings,
                   bool replaceCurrentPlayback) {
    PlaybackRequest request;
    try {
        request.kind = kind;
        request.settings = settings;
    } catch (...) {
        Wh_Log(L"Unable to allocate a playback request.");
        return false;
    }

    AcquireSRWLockExclusive(&g_playbackControlLock);
    AcquireSRWLockExclusive(&g_playbackLock);
    if (g_stopping.load() ||
        (kind != PlaybackKind::Test && g_externalPowerConnected.load())) {
        ReleaseSRWLockExclusive(&g_playbackLock);
        ReleaseSRWLockExclusive(&g_playbackControlLock);
        return false;
    }

    bool stopActiveWaveform = replaceCurrentPlayback && g_playbackActive;
    if (replaceCurrentPlayback) {
        g_hasPendingPlayback = false;
        ResetEvent(g_playbackRequestEvent);
    }
    if (stopActiveWaveform) {
        if (!SetEvent(g_playbackCancelEvent)) {
            LogWin32Failure(L"SetEvent for playback cancellation",
                            GetLastError());
        }
    }
    ReleaseSRWLockExclusive(&g_playbackLock);

    if (stopActiveWaveform) {
        StopActiveWaveformWhileControlLocked();
    }

    AcquireSRWLockExclusive(&g_playbackLock);
    if (g_stopping.load()) {
        ReleaseSRWLockExclusive(&g_playbackLock);
        ReleaseSRWLockExclusive(&g_playbackControlLock);
        return false;
    }

    if (!replaceCurrentPlayback && g_hasPendingPlayback &&
        PlaybackPriority(g_pendingPlayback.kind) > PlaybackPriority(kind)) {
        ReleaseSRWLockExclusive(&g_playbackLock);
        ReleaseSRWLockExclusive(&g_playbackControlLock);
        return true;
    }
    g_pendingPlayback = std::move(request);
    g_hasPendingPlayback = true;
    BOOL signalResult = SetEvent(g_playbackRequestEvent);
    if (!signalResult) {
        g_hasPendingPlayback = false;
    }
    ReleaseSRWLockExclusive(&g_playbackLock);
    ReleaseSRWLockExclusive(&g_playbackControlLock);

    if (!signalResult) {
        LogWin32Failure(L"SetEvent for a playback request", GetLastError());
        return false;
    }

    return true;
}

void CancelAllPlayback() {
    bool hadPlayback = false;
    bool stopActiveWaveform = false;
    AcquireSRWLockExclusive(&g_playbackControlLock);
    AcquireSRWLockExclusive(&g_playbackLock);
    hadPlayback = g_playbackActive || g_hasPendingPlayback;
    g_hasPendingPlayback = false;
    ResetEvent(g_playbackRequestEvent);
    if (g_playbackActive) {
        stopActiveWaveform = true;
        if (!SetEvent(g_playbackCancelEvent)) {
            LogWin32Failure(L"SetEvent for playback cancellation",
                            GetLastError());
        }
    }
    ReleaseSRWLockExclusive(&g_playbackLock);

    if (stopActiveWaveform) {
        StopActiveWaveformWhileControlLocked();
    }
    ReleaseSRWLockExclusive(&g_playbackControlLock);

    if (hadPlayback) {
        Wh_Log(L"Sound playback cancellation requested.");
    }
}

void CancelPlaybackImmediatelyForExternalPower() {
    if (g_playbackCancelEvent && !SetEvent(g_playbackCancelEvent)) {
        LogWin32Failure(L"SetEvent for immediate AC playback cancellation",
                        GetLastError());
    }

    CancelAllPlayback();
}

LRESULT CALLBACK PowerNotificationWindowProcedure(HWND window,
                                                  UINT message,
                                                  WPARAM wParam,
                                                  LPARAM lParam) {
    if (message == WM_POWERBROADCAST) {
        bool requestReevaluation = false;
        bool externalPowerConnected = false;

        if (wParam == PBT_POWERSETTINGCHANGE && lParam != 0) {
            auto* setting = reinterpret_cast<const POWERBROADCAST_SETTING*>(
                lParam);
            if (IsEqualGUID(setting->PowerSetting, GUID_ACDC_POWER_SOURCE) &&
                setting->DataLength >= sizeof(DWORD)) {
                DWORD condition = 0;
                std::memcpy(&condition, setting->Data, sizeof(condition));
                requestReevaluation = true;
                externalPowerConnected =
                    condition == static_cast<DWORD>(PoAc);
                g_externalPowerConnected.store(externalPowerConnected);

                int previousCondition = g_lastNotifiedPowerCondition.exchange(
                    static_cast<int>(condition));
                if (previousCondition != static_cast<int>(condition)) {
                    if (condition == static_cast<DWORD>(PoAc)) {
                        Wh_Log(L"Power notification reports external power.");
                    } else if (condition == static_cast<DWORD>(PoDc)) {
                        Wh_Log(L"Power notification reports battery power.");
                    } else {
                        Wh_Log(L"Power notification reports a short-term power "
                               L"source.");
                    }
                }
            }
        } else if (wParam == PBT_APMPOWERSTATUSCHANGE) {
            SYSTEM_POWER_STATUS powerStatus{};
            if (GetSystemPowerStatus(&powerStatus)) {
                requestReevaluation = true;
                externalPowerConnected = powerStatus.ACLineStatus == 1;
                g_externalPowerConnected.store(externalPowerConnected);
            }
        }

        if (externalPowerConnected) {
            CancelPlaybackImmediatelyForExternalPower();
        }
        if (requestReevaluation && g_powerSourceChangedEvent &&
            !SetEvent(g_powerSourceChangedEvent)) {
            LogWin32Failure(L"SetEvent for a power-source change",
                            GetLastError());
        }
        return TRUE;
    }

    if (message == WM_CLOSE) {
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(window, message, wParam, lParam);
}

void ClosePowerNotificationController() {
    HWND window = g_powerNotificationWindow.load();
    if (window && IsWindow(window) &&
        !PostMessageW(window, WM_CLOSE, 0, 0)) {
        LogWin32Failure(L"PostMessageW for the power notification controller",
                        GetLastError());
    }
}

DWORD WINAPI PowerNotificationThreadProcedure(void*) {
    Wh_Log(L"Power notification thread started.");

    HINSTANCE instance = GetModuleHandleW(nullptr);
    WNDCLASSEXW windowClass{};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.lpfnWndProc = PowerNotificationWindowProcedure;
    windowClass.hInstance = instance;
    windowClass.lpszClassName = kPowerNotificationWindowClass;

    ATOM classAtom = RegisterClassExW(&windowClass);
    if (!classAtom && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        LogWin32Failure(L"RegisterClassExW for power notifications",
                        GetLastError());
        SetEvent(g_powerNotificationReadyEvent);
        return 0;
    }

    HWND window = CreateWindowExW(
        0, kPowerNotificationWindowClass, L"", 0, 0, 0, 0, 0, HWND_MESSAGE,
        nullptr, instance, nullptr);
    if (!window) {
        LogWin32Failure(L"CreateWindowExW for power notifications",
                        GetLastError());
        if (classAtom) {
            UnregisterClassW(kPowerNotificationWindowClass, instance);
        }
        SetEvent(g_powerNotificationReadyEvent);
        return 0;
    }

    g_powerNotificationWindow.store(window);
    HPOWERNOTIFY registration = RegisterPowerSettingNotification(
        window, &GUID_ACDC_POWER_SOURCE, DEVICE_NOTIFY_WINDOW_HANDLE);
    if (!registration) {
        LogWin32Failure(L"RegisterPowerSettingNotification for AC/DC power",
                        GetLastError());
    } else {
        g_powerNotificationInitialized.store(true);
        Wh_Log(L"Immediate AC/DC power notifications are active.");
    }

    if (!SetEvent(g_powerNotificationReadyEvent)) {
        LogWin32Failure(L"SetEvent for power notification readiness",
                        GetLastError());
    }
    if (g_stopping.load()) {
        PostMessageW(window, WM_CLOSE, 0, 0);
    }

    MSG message{};
    BOOL messageResult = 0;
    while ((messageResult = GetMessageW(&message, nullptr, 0, 0)) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
    if (messageResult == -1) {
        LogWin32Failure(L"GetMessageW for power notifications",
                        GetLastError());
    }

    if (registration && !UnregisterPowerSettingNotification(registration)) {
        LogWin32Failure(L"UnregisterPowerSettingNotification", GetLastError());
    }
    g_powerNotificationWindow.store(nullptr);
    if (IsWindow(window)) {
        DestroyWindow(window);
    }
    if (classAtom &&
        !UnregisterClassW(kPowerNotificationWindowClass, instance)) {
        LogWin32Failure(L"UnregisterClassW for power notifications",
                        GetLastError());
    }

    Wh_Log(L"Power notification thread stopped.");
    return 0;
}

bool TakePendingPlayback(PlaybackRequest* request) {
    AcquireSRWLockExclusive(&g_playbackLock);
    if (!g_hasPendingPlayback || g_stopping.load()) {
        ReleaseSRWLockExclusive(&g_playbackLock);
        return false;
    }

    *request = std::move(g_pendingPlayback);
    g_hasPendingPlayback = false;
    g_playbackActive = true;
    ResetEvent(g_playbackCancelEvent);
    ReleaseSRWLockExclusive(&g_playbackLock);
    return true;
}

void FinishPlayback() {
    AcquireSRWLockExclusive(&g_playbackLock);
    g_playbackActive = false;
    bool hasPendingPlayback = g_hasPendingPlayback && !g_stopping.load();
    if (hasPendingPlayback) {
        SetEvent(g_playbackRequestEvent);
    }
    ReleaseSRWLockExclusive(&g_playbackLock);

    if (!SetEvent(g_playbackFinishedEvent)) {
        LogWin32Failure(L"SetEvent for playback completion", GetLastError());
    }
}

bool IsPlaybackCancelled() {
    return WaitForSingleObject(g_shutdownEvent, 0) == WAIT_OBJECT_0 ||
           WaitForSingleObject(g_playbackCancelEvent, 0) == WAIT_OBJECT_0;
}

bool WaitForPlaybackDelay(DWORD milliseconds) {
    HANDLE events[] = {g_shutdownEvent, g_playbackCancelEvent};
    DWORD waitResult =
        WaitForMultipleObjects(ARRAYSIZE(events), events, FALSE, milliseconds);
    if (waitResult == WAIT_TIMEOUT) {
        return true;
    }
    if (waitResult == WAIT_FAILED) {
        LogWin32Failure(L"Playback delay wait", GetLastError());
    }
    return false;
}

std::wstring TrimAndUnquote(std::wstring value) {
    size_t first = 0;
    while (first < value.size() && iswspace(value[first])) {
        ++first;
    }

    size_t last = value.size();
    while (last > first && iswspace(value[last - 1])) {
        --last;
    }

    value = value.substr(first, last - first);
    if (value.size() >= 2 &&
        ((value.front() == L'"' && value.back() == L'"') ||
         (value.front() == L'\'' && value.back() == L'\''))) {
        value = value.substr(1, value.size() - 2);
        first = 0;
        while (first < value.size() && iswspace(value[first])) {
            ++first;
        }
        last = value.size();
        while (last > first && iswspace(value[last - 1])) {
            --last;
        }
        value = value.substr(first, last - first);
    }

    return value;
}

bool HasWavExtension(const std::wstring& path) {
    size_t separator = path.find_last_of(L"\\/");
    size_t dot = path.find_last_of(L'.');
    if (dot == std::wstring::npos ||
        (separator != std::wstring::npos && dot < separator)) {
        return false;
    }

    return _wcsicmp(path.c_str() + dot, L".wav") == 0;
}

bool ResolveCustomSoundPath(const std::wstring& storedPath,
                            std::wstring* resolvedPath) {
    std::wstring path;
    try {
        path = TrimAndUnquote(storedPath);
    } catch (...) {
        Wh_Log(L"Unable to allocate memory while processing the custom sound "
               L"path.");
        return false;
    }

    if (path.empty()) {
        Wh_Log(L"The custom sound path is empty.");
        return false;
    }

    if (path.size() >= 32768) {
        Wh_Log(L"The custom sound path is too long.");
        return false;
    }

    if (!HasWavExtension(path)) {
        Wh_Log(L"The custom sound path does not have a WAV extension: %s",
               path.c_str());
        return false;
    }

    SetLastError(ERROR_SUCCESS);
    DWORD attributes = GetFileAttributesW(path.c_str());
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        DWORD error = GetLastError();
        Wh_Log(L"The custom WAV file is invalid or inaccessible: %s",
               path.c_str());
        LogWin32Failure(L"GetFileAttributesW", error);
        return false;
    }
    if ((attributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
        Wh_Log(L"The custom WAV path refers to a directory: %s",
               path.c_str());
        return false;
    }

    *resolvedPath = std::move(path);
    return true;
}

struct LoadedWaveFile {
    std::vector<BYTE> formatBytes;
    std::vector<BYTE> audioData;

    WAVEFORMATEX* Format() {
        return reinterpret_cast<WAVEFORMATEX*>(formatBytes.data());
    }
};

bool LoadWaveFile(const std::wstring& path, LoadedWaveFile* loadedWave) {
    if (!loadedWave) {
        return false;
    }

    std::vector<wchar_t> mutablePath;
    try {
        mutablePath.assign(path.begin(), path.end());
        mutablePath.push_back(L'\0');
    } catch (...) {
        Wh_Log(L"Unable to allocate memory while inspecting the WAV file.");
        return false;
    }

    HMMIO waveFile = mmioOpenW(mutablePath.data(), nullptr,
                               MMIO_READ | MMIO_ALLOCBUF);
    if (!waveFile) {
        Wh_Log(L"Unable to open the WAV file for duration inspection: %s",
               path.c_str());
        return false;
    }

    try {
        MMCKINFO riffChunk{};
        riffChunk.fccType = mmioFOURCC('W', 'A', 'V', 'E');
        MMRESULT result =
            mmioDescend(waveFile, &riffChunk, nullptr, MMIO_FINDRIFF);
        if (result != MMSYSERR_NOERROR) {
            mmioClose(waveFile, 0);
            Wh_Log(L"The selected file is not a readable RIFF/WAVE file: %s",
                   path.c_str());
            return false;
        }

        MMCKINFO formatChunk{};
        formatChunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
        result =
            mmioDescend(waveFile, &formatChunk, &riffChunk, MMIO_FINDCHUNK);
        constexpr DWORD kBaseWaveFormatBytes = 16;
        constexpr DWORD kMaximumWaveFormatBytes = 65536;
        if (result != MMSYSERR_NOERROR ||
            formatChunk.cksize < kBaseWaveFormatBytes ||
            formatChunk.cksize > kMaximumWaveFormatBytes) {
            mmioClose(waveFile, 0);
            Wh_Log(L"The selected WAV file has an invalid format chunk: %s",
                   path.c_str());
            return false;
        }

        LoadedWaveFile temporaryWave;
        temporaryWave.formatBytes.resize(
            (std::max)(static_cast<size_t>(formatChunk.cksize),
                       sizeof(WAVEFORMATEX)),
            0);
        if (mmioRead(waveFile,
                     reinterpret_cast<HPSTR>(
                         temporaryWave.formatBytes.data()),
                     static_cast<LONG>(formatChunk.cksize)) !=
            static_cast<LONG>(formatChunk.cksize)) {
            mmioClose(waveFile, 0);
            Wh_Log(L"The WAV format chunk could not be read: %s",
                   path.c_str());
            return false;
        }

        WAVEFORMATEX* format = temporaryWave.Format();
        if (format->wFormatTag == 0 || format->nChannels == 0 ||
            format->nSamplesPerSec == 0 || format->nAvgBytesPerSec == 0 ||
            format->nBlockAlign == 0) {
            mmioClose(waveFile, 0);
            Wh_Log(L"The selected WAV file contains an unusable format: %s",
                   path.c_str());
            return false;
        }

        result = mmioAscend(waveFile, &formatChunk, 0);
        MMCKINFO dataChunk{};
        dataChunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
        if (result == MMSYSERR_NOERROR) {
            result = mmioDescend(waveFile, &dataChunk, &riffChunk,
                                 MMIO_FINDCHUNK);
        }
        if (result != MMSYSERR_NOERROR || dataChunk.cksize == 0 ||
            dataChunk.cksize > kMaximumWaveFileBytes) {
            mmioClose(waveFile, 0);
            Wh_Log(L"The selected WAV file has no usable data chunk or exceeds "
                   L"the %lu-byte alarm limit: %s",
                   kMaximumWaveFileBytes, path.c_str());
            return false;
        }

        temporaryWave.audioData.resize(dataChunk.cksize);
        if (mmioRead(waveFile,
                     reinterpret_cast<HPSTR>(temporaryWave.audioData.data()),
                     static_cast<LONG>(dataChunk.cksize)) !=
            static_cast<LONG>(dataChunk.cksize)) {
            mmioClose(waveFile, 0);
            Wh_Log(L"The WAV audio data could not be read: %s", path.c_str());
            return false;
        }

        mmioClose(waveFile, 0);
        *loadedWave = std::move(temporaryWave);
        return true;
    } catch (...) {
        mmioClose(waveFile, 0);
        Wh_Log(L"Unable to allocate memory while loading the WAV file: %s",
               path.c_str());
        return false;
    }
}

ERole GetDefaultAudioRole(AudioOutputMode mode) {
    switch (mode) {
        case AudioOutputMode::Multimedia:
            return eMultimedia;
        case AudioOutputMode::Communications:
            return eCommunications;
        case AudioOutputMode::WindowsDefault:
        case AudioOutputMode::EndpointId:
            return eConsole;
    }
    return eConsole;
}

HRESULT ResolveConfiguredAudioDevice(IMMDeviceEnumerator* enumerator,
                                     AudioOutputMode mode,
                                     const std::wstring& configuredValue,
                                     IMMDevice** resolvedDevice) {
    if (!enumerator || !resolvedDevice) {
        return E_INVALIDARG;
    }
    *resolvedDevice = nullptr;

    if (mode != AudioOutputMode::EndpointId) {
        return enumerator->GetDefaultAudioEndpoint(eRender,
                                                   GetDefaultAudioRole(mode),
                                                   resolvedDevice);
    }

    std::wstring value;
    try {
        value = TrimAndUnquote(configuredValue);
    } catch (...) {
        return E_OUTOFMEMORY;
    }
    if (value.empty()) {
        return E_INVALIDARG;
    }

    ComPtr<IMMDevice> device;
    HRESULT result = enumerator->GetDevice(value.c_str(), device.Put());
    if (FAILED(result)) {
        return result;
    }

    DWORD state = 0;
    result = device->GetState(&state);
    if (FAILED(result)) {
        return result;
    }
    if ((state & DEVICE_STATE_ACTIVE) == 0) {
        return HRESULT_FROM_WIN32(ERROR_NOT_READY);
    }

    device.Get()->AddRef();
    *resolvedDevice = device.Get();
    return S_OK;
}

struct AudioOutputOption {
    AudioOutputMode mode = AudioOutputMode::WindowsDefault;
    std::wstring endpointId;
    std::wstring displayName;
};

struct AudioPickerWindowState {
    const std::vector<AudioOutputOption>* options = nullptr;
    int initialSelection = 0;
    HWND comboBox = nullptr;
    HWND soundPathEdit = nullptr;
    std::wstring selectedSoundFilePath;
    bool soundFilePathChanged = false;
};

bool GetAudioEndpointDisplayName(IMMDevice* device, std::wstring* displayName) {
    if (!device || !displayName) {
        return false;
    }

    ComPtr<IPropertyStore> propertyStore;
    HRESULT result = device->OpenPropertyStore(STGM_READ, propertyStore.Put());
    if (FAILED(result)) {
        return false;
    }

    PROPVARIANT value{};
    PropVariantInit(&value);
    result = propertyStore->GetValue(PKEY_Device_FriendlyName, &value);
    bool succeeded = false;
    try {
        if (SUCCEEDED(result) && value.vt == VT_LPWSTR && value.pwszVal &&
            value.pwszVal[0] != L'\0') {
            displayName->assign(value.pwszVal);
            succeeded = true;
        }
    } catch (...) {
        succeeded = false;
    }
    PropVariantClear(&value);
    return succeeded;
}

bool GetAudioEndpointId(IMMDevice* device, std::wstring* endpointId) {
    if (!device || !endpointId) {
        return false;
    }

    LPWSTR rawEndpointId = nullptr;
    HRESULT result = device->GetId(&rawEndpointId);
    if (FAILED(result) || !rawEndpointId) {
        CoTaskMemFree(rawEndpointId);
        return false;
    }

    bool succeeded = false;
    try {
        endpointId->assign(rawEndpointId);
        succeeded = true;
    } catch (...) {
        succeeded = false;
    }
    CoTaskMemFree(rawEndpointId);
    return succeeded;
}

bool BuildAudioOutputOptions(std::vector<AudioOutputOption>* options,
                             const Settings& currentSettings,
                             int* initialSelection) {
    if (!options || !initialSelection) {
        return false;
    }

    try {
        options->clear();
        options->push_back({AudioOutputMode::WindowsDefault, L"",
                            L"Windows default output (console)"});
        options->push_back({AudioOutputMode::Multimedia, L"",
                            L"Windows default multimedia output"});
        options->push_back({AudioOutputMode::Communications, L"",
                            L"Windows default communications output"});

        ComPtr<IMMDeviceEnumerator> enumerator;
        HRESULT result = CoCreateInstance(
            __uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER,
            __uuidof(IMMDeviceEnumerator),
            reinterpret_cast<void**>(enumerator.Put()));
        if (FAILED(result)) {
            LogHResultFailure(L"Creating the audio device enumerator", result);
        } else {
            ComPtr<IMMDeviceCollection> collection;
            result = enumerator->EnumAudioEndpoints(
                eRender, DEVICE_STATE_ACTIVE, collection.Put());
            if (FAILED(result)) {
                LogHResultFailure(L"Enumerating active audio outputs", result);
            } else {
                UINT count = 0;
                result = collection->GetCount(&count);
                if (FAILED(result)) {
                    LogHResultFailure(L"Counting active audio outputs", result);
                } else {
                    size_t deviceOptionsBegin = options->size();
                    for (UINT index = 0; index < count; ++index) {
                        ComPtr<IMMDevice> device;
                        if (FAILED(collection->Item(index, device.Put()))) {
                            continue;
                        }

                        std::wstring endpointId;
                        if (!GetAudioEndpointId(device.Get(), &endpointId)) {
                            continue;
                        }

                        std::wstring displayName;
                        if (!GetAudioEndpointDisplayName(device.Get(),
                                                         &displayName)) {
                            displayName = endpointId;
                        }
                        options->push_back({AudioOutputMode::EndpointId,
                                            std::move(endpointId),
                                            L"Device: " + displayName});
                    }

                    std::sort(options->begin() +
                                  static_cast<std::ptrdiff_t>(deviceOptionsBegin),
                              options->end(),
                              [](const AudioOutputOption& first,
                                 const AudioOutputOption& second) {
                                  return CompareStringOrdinal(
                                             first.displayName.c_str(), -1,
                                             second.displayName.c_str(), -1,
                                             TRUE) == CSTR_LESS_THAN;
                              });
                }
            }
        }

        *initialSelection = 0;
        for (size_t index = 0; index < options->size(); ++index) {
            const AudioOutputOption& option = (*options)[index];
            bool matches = option.mode == currentSettings.audioOutputMode;
            if (matches && option.mode == AudioOutputMode::EndpointId) {
                matches = option.endpointId ==
                          currentSettings.audioOutputDevice;
            }
            if (matches) {
                *initialSelection = static_cast<int>(index);
                break;
            }
        }
        return true;
    } catch (...) {
        Wh_Log(L"Unable to allocate memory while enumerating audio outputs.");
        return false;
    }
}

bool BrowseForWaveFile(HWND owner,
                       const std::wstring& initialPath,
                       std::wstring* selectedPath) {
    if (!selectedPath) {
        return false;
    }

    try {
        std::vector<wchar_t> filePath(32768);
        if (initialPath.size() < filePath.size()) {
            std::copy(initialPath.begin(), initialPath.end(), filePath.begin());
        }

        OPENFILENAMEW dialog{};
        dialog.lStructSize = sizeof(dialog);
        dialog.hwndOwner = owner;
        dialog.lpstrFilter =
            L"WAV audio files (*.wav)\0*.wav\0All files (*.*)\0*.*\0\0";
        dialog.lpstrFile = filePath.data();
        dialog.nMaxFile = static_cast<DWORD>(filePath.size());
        dialog.lpstrDefExt = L"wav";
        dialog.lpstrTitle = L"Select the Low Battery Alarm WAV file";
        dialog.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST |
                       OFN_NOCHANGEDIR | OFN_HIDEREADONLY;

        if (!GetOpenFileNameW(&dialog)) {
            DWORD error = CommDlgExtendedError();
            if (error != 0) {
                Wh_Log(L"The WAV file dialog failed. ExtendedError=%lu.",
                       error);
                MessageBoxW(owner, L"The WAV file dialog could not be opened.",
                            L"Low Battery Alarm", MB_OK | MB_ICONERROR);
            }
            return false;
        }

        std::wstring path(filePath.data());
        if (!HasWavExtension(path)) {
            MessageBoxW(owner, L"Select a file with the .wav extension.",
                        L"Low Battery Alarm", MB_OK | MB_ICONWARNING);
            return false;
        }
        *selectedPath = std::move(path);
        return true;
    } catch (...) {
        Wh_Log(L"Unable to allocate memory for the WAV file dialog.");
        MessageBoxW(owner, L"The WAV file dialog could not be prepared.",
                    L"Low Battery Alarm", MB_OK | MB_ICONERROR);
        return false;
    }
}

bool SavePickerConfiguration(const AudioOutputOption& option,
                             const std::wstring* selectedSoundFilePath) {
    Settings previousSettings;
    bool havePreviousSettings = GetSettingsSnapshot(&previousSettings);

    if (!Wh_SetStringValue(kStoredAudioEndpointId, option.endpointId.c_str())) {
        Wh_Log(L"Unable to store the selected audio endpoint ID.");
        return false;
    }
    if (selectedSoundFilePath &&
        !Wh_SetStringValue(kStoredSoundFilePath,
                           selectedSoundFilePath->c_str())) {
        Wh_Log(L"Unable to store the selected WAV file path.");
        if (havePreviousSettings) {
            if (!Wh_SetStringValue(
                    kStoredAudioEndpointId,
                    previousSettings.audioOutputDevice.c_str())) {
                Wh_Log(L"Unable to restore the previous audio endpoint ID "
                       L"after the WAV path could not be stored.");
            }
        }
        return false;
    }
    if (!Wh_SetIntValue(kStoredAudioOutputMode,
                        static_cast<int>(option.mode))) {
        Wh_Log(L"Unable to store the selected audio output mode.");
        if (havePreviousSettings) {
            if (!Wh_SetStringValue(
                    kStoredAudioEndpointId,
                    previousSettings.audioOutputDevice.c_str())) {
                Wh_Log(L"Unable to restore the previous audio endpoint ID "
                       L"after the output mode could not be stored.");
            }
            if (selectedSoundFilePath &&
                !Wh_SetStringValue(
                    kStoredSoundFilePath,
                    previousSettings.pickerSoundFilePath.c_str())) {
                Wh_Log(L"Unable to restore the previous WAV picker path after "
                       L"the output mode could not be stored.");
            }
        }
        return false;
    }

    bool settingsUpdated = false;
    AcquireSRWLockExclusive(&g_settingsLock);
    try {
        g_settings.audioOutputMode = option.mode;
        g_settings.audioOutputDevice = option.endpointId;
        g_settings.hasStoredAudioOutputSelection = true;
        if (selectedSoundFilePath) {
            g_settings.pickerSoundFilePath = *selectedSoundFilePath;
            g_settings.soundFilePath = *selectedSoundFilePath;
        }
        settingsUpdated = true;
    } catch (...) {
        Wh_Log(L"The audio output selection was stored but could not be "
               L"applied to the current settings snapshot.");
    }
    ReleaseSRWLockExclusive(&g_settingsLock);

    if (settingsUpdated) {
        CancelAllPlayback();
        Wh_Log(L"Alarm output selection changed to: %s",
               option.displayName.c_str());
        if (selectedSoundFilePath) {
            Wh_Log(L"Alarm WAV file selected with Windows Explorer: %s",
                   selectedSoundFilePath->c_str());
        }

        Settings selectedSettings;
        if (GetSettingsSnapshot(&selectedSettings) &&
            selectedSettings.testSoundAfterSettingsChange) {
            QueuePlayback(PlaybackKind::Test, selectedSettings, false);
        }
    }
    return true;
}

BOOL CALLBACK SetAudioPickerControlFont(HWND window, LPARAM fontParameter) {
    SendMessageW(window, WM_SETFONT, static_cast<WPARAM>(fontParameter), TRUE);
    return TRUE;
}

LRESULT CALLBACK AudioPickerWindowProcedure(HWND window,
                                            UINT message,
                                            WPARAM wParam,
                                            LPARAM lParam) {
    AudioPickerWindowState* state = reinterpret_cast<AudioPickerWindowState*>(
        GetWindowLongPtrW(window, GWLP_USERDATA));

    switch (message) {
        case WM_NCCREATE: {
            auto* create = reinterpret_cast<CREATESTRUCTW*>(lParam);
            SetWindowLongPtrW(window, GWLP_USERDATA,
                              reinterpret_cast<LONG_PTR>(create->lpCreateParams));
            return TRUE;
        }

        case WM_CREATE: {
            state = reinterpret_cast<AudioPickerWindowState*>(
                GetWindowLongPtrW(window, GWLP_USERDATA));
            if (!state || !state->options) {
                return -1;
            }

            HWND instruction = CreateWindowExW(
                0, L"STATIC",
                L"Select the output used for Low Battery Alarm:",
                WS_CHILD | WS_VISIBLE, 16, 16, 568, 20, window, nullptr,
                nullptr, nullptr);
            state->comboBox = CreateWindowExW(
                WS_EX_CLIENTEDGE, L"COMBOBOX", L"",
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL |
                    CBS_DROPDOWNLIST,
                16, 44, 568, 300, window,
                reinterpret_cast<HMENU>(
                    static_cast<INT_PTR>(kAudioPickerComboId)),
                nullptr, nullptr);
            HWND soundPathLabel = CreateWindowExW(
                0, L"STATIC", L"Alarm WAV file:", WS_CHILD | WS_VISIBLE, 16,
                82, 568, 20, window, nullptr, nullptr, nullptr);
            state->soundPathEdit = CreateWindowExW(
                WS_EX_CLIENTEDGE, L"EDIT",
                state->selectedSoundFilePath.c_str(),
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL |
                    ES_READONLY,
                16, 106, 456, 25, window,
                reinterpret_cast<HMENU>(
                    static_cast<INT_PTR>(kAudioPickerSoundPathId)),
                nullptr, nullptr);
            HWND browseButton = CreateWindowExW(
                0, L"BUTTON", L"Browse...",
                WS_CHILD | WS_VISIBLE | WS_TABSTOP, 480, 104, 104, 29,
                window,
                reinterpret_cast<HMENU>(
                    static_cast<INT_PTR>(kAudioPickerBrowseId)),
                nullptr, nullptr);
            HWND hint = CreateWindowExW(
                0, L"STATIC",
                L"Only active output devices are shown. Device selection is "
                L"stored by endpoint ID.",
                WS_CHILD | WS_VISIBLE, 16, 145, 568, 34, window,
                reinterpret_cast<HMENU>(
                    static_cast<INT_PTR>(kAudioPickerHintId)),
                nullptr, nullptr);
            HWND okButton = CreateWindowExW(
                0, L"BUTTON", L"OK",
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON, 376,
                187, 100, 30, window,
                reinterpret_cast<HMENU>(static_cast<INT_PTR>(IDOK)), nullptr,
                nullptr);
            HWND cancelButton = CreateWindowExW(
                0, L"BUTTON", L"Cancel",
                WS_CHILD | WS_VISIBLE | WS_TABSTOP, 484, 187, 100, 30,
                window,
                reinterpret_cast<HMENU>(static_cast<INT_PTR>(IDCANCEL)),
                nullptr, nullptr);

            if (!instruction || !state->comboBox || !soundPathLabel ||
                !state->soundPathEdit || !browseButton || !hint ||
                !okButton || !cancelButton) {
                return -1;
            }

            for (const AudioOutputOption& option : *state->options) {
                if (SendMessageW(state->comboBox, CB_ADDSTRING, 0,
                                 reinterpret_cast<LPARAM>(
                                     option.displayName.c_str())) < 0) {
                    return -1;
                }
            }
            SendMessageW(state->comboBox, CB_SETCURSEL,
                         static_cast<WPARAM>(state->initialSelection), 0);

            HFONT font = static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
            if (font) {
                EnumChildWindows(window, SetAudioPickerControlFont,
                                 reinterpret_cast<LPARAM>(font));
            }
            return 0;
        }

        case WM_COMMAND:
            if (LOWORD(wParam) == kAudioPickerBrowseId &&
                HIWORD(wParam) == BN_CLICKED) {
                if (!state || !state->soundPathEdit) {
                    return 0;
                }
                std::wstring selectedPath;
                if (BrowseForWaveFile(window, state->selectedSoundFilePath,
                                      &selectedPath)) {
                    state->selectedSoundFilePath = std::move(selectedPath);
                    state->soundFilePathChanged = true;
                    SetWindowTextW(state->soundPathEdit,
                                   state->selectedSoundFilePath.c_str());
                }
                return 0;
            }
            if (LOWORD(wParam) == IDOK && HIWORD(wParam) == BN_CLICKED) {
                if (!state || !state->options || !state->comboBox) {
                    return 0;
                }
                LRESULT selection =
                    SendMessageW(state->comboBox, CB_GETCURSEL, 0, 0);
                if (selection == CB_ERR ||
                    static_cast<size_t>(selection) >= state->options->size()) {
                    MessageBoxW(window, L"Select an audio output first.",
                                L"Low Battery Alarm", MB_OK | MB_ICONWARNING);
                    return 0;
                }
                const std::wstring* selectedSoundFilePath =
                    state->soundFilePathChanged
                        ? &state->selectedSoundFilePath
                        : nullptr;
                if (!SavePickerConfiguration(
                        (*state->options)[static_cast<size_t>(selection)],
                        selectedSoundFilePath)) {
                    MessageBoxW(window,
                                L"The alarm configuration could not be saved.",
                                L"Low Battery Alarm", MB_OK | MB_ICONERROR);
                    return 0;
                }
                DestroyWindow(window);
                return 0;
            }
            if (LOWORD(wParam) == IDCANCEL &&
                HIWORD(wParam) == BN_CLICKED) {
                DestroyWindow(window);
                return 0;
            }
            break;

        case WM_CLOSE:
            DestroyWindow(window);
            return 0;

        case WM_CTLCOLORSTATIC: {
            HDC deviceContext = reinterpret_cast<HDC>(wParam);
            SetBkMode(deviceContext, TRANSPARENT);
            return reinterpret_cast<LRESULT>(GetSysColorBrush(COLOR_WINDOW));
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        default:
            if (message == kAudioPickerBringToFrontMessage) {
                ShowWindow(window, SW_RESTORE);
                SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0,
                             SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
                SetForegroundWindow(window);
                return 0;
            }
            break;
    }

    return DefWindowProcW(window, message, wParam, lParam);
}

bool RunAudioOutputPicker(const std::vector<AudioOutputOption>& options,
                          int initialSelection,
                          const std::wstring& currentSoundFilePath) {
    WNDCLASSEXW windowClass{};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.lpfnWndProc = AudioPickerWindowProcedure;
    windowClass.hInstance = GetModuleHandleW(nullptr);
    windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    windowClass.hIcon = LoadIconW(nullptr, IDI_INFORMATION);
    windowClass.hbrBackground = GetSysColorBrush(COLOR_WINDOW);
    windowClass.lpszClassName = kAudioPickerWindowClass;
    ATOM classAtom = RegisterClassExW(&windowClass);
    if (!classAtom) {
        LogWin32Failure(L"Registering the audio picker window class",
                        GetLastError());
        return false;
    }

    AudioPickerWindowState state;
    state.options = &options;
    state.initialSelection = initialSelection;
    state.selectedSoundFilePath = currentSoundFilePath;
    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
    DWORD extendedStyle = WS_EX_TOPMOST | WS_EX_DLGMODALFRAME;
    RECT windowRectangle{0, 0, 600, 235};
    AdjustWindowRectEx(&windowRectangle, style, FALSE, extendedStyle);
    int windowWidth = windowRectangle.right - windowRectangle.left;
    int windowHeight = windowRectangle.bottom - windowRectangle.top;

    RECT workArea{};
    if (!SystemParametersInfoW(SPI_GETWORKAREA, 0, &workArea, 0)) {
        workArea.right = GetSystemMetrics(SM_CXSCREEN);
        workArea.bottom = GetSystemMetrics(SM_CYSCREEN);
    }
    int windowX = workArea.left +
                  ((workArea.right - workArea.left) - windowWidth) / 2;
    int windowY = workArea.top +
                  ((workArea.bottom - workArea.top) - windowHeight) / 2;

    HWND window = CreateWindowExW(
        extendedStyle, kAudioPickerWindowClass,
        L"Low Battery Alarm configuration", style, windowX, windowY,
        windowWidth, windowHeight, nullptr, nullptr, windowClass.hInstance,
        &state);
    if (!window) {
        LogWin32Failure(L"Creating the audio picker window", GetLastError());
        UnregisterClassW(kAudioPickerWindowClass, windowClass.hInstance);
        return false;
    }

    AcquireSRWLockExclusive(&g_audioPickerLock);
    g_audioPickerWindow = window;
    ReleaseSRWLockExclusive(&g_audioPickerLock);

    if (g_stopping.load()) {
        DestroyWindow(window);
    } else {
        ShowWindow(window, SW_SHOW);
        SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
        SetForegroundWindow(window);
    }

    MSG message{};
    BOOL getMessageResult = 0;
    while ((getMessageResult = GetMessageW(&message, nullptr, 0, 0)) > 0) {
        if (!IsDialogMessageW(window, &message)) {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    }
    if (getMessageResult < 0) {
        LogWin32Failure(L"Reading an audio picker window message",
                        GetLastError());
    }

    AcquireSRWLockExclusive(&g_audioPickerLock);
    if (g_audioPickerWindow == window) {
        g_audioPickerWindow = nullptr;
    }
    ReleaseSRWLockExclusive(&g_audioPickerLock);

    if (IsWindow(window)) {
        DestroyWindow(window);
    }
    if (!UnregisterClassW(kAudioPickerWindowClass, windowClass.hInstance)) {
        LogWin32Failure(L"Unregistering the audio picker window class",
                        GetLastError());
    }
    return getMessageResult >= 0;
}

DWORD WINAPI AudioOutputPickerThreadProcedure(void*) {
    Wh_Log(L"Audio output picker thread started.");
    HRESULT comResult = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    bool comInitialized = SUCCEEDED(comResult);
    if (FAILED(comResult)) {
        LogHResultFailure(L"CoInitializeEx for the audio picker", comResult);
        MessageBoxW(nullptr,
                    L"The audio output picker could not initialize COM.",
                    L"Low Battery Alarm", MB_OK | MB_ICONERROR | MB_TOPMOST |
                                                MB_SETFOREGROUND);
    }

    try {
        if (comInitialized && !g_stopping.load()) {
            Settings currentSettings;
            std::vector<AudioOutputOption> options;
            int initialSelection = 0;
            if (!GetSettingsSnapshot(&currentSettings) ||
                !BuildAudioOutputOptions(&options, currentSettings,
                                         &initialSelection)) {
                MessageBoxW(nullptr,
                            L"The list of audio output devices could not be "
                            L"created.",
                            L"Low Battery Alarm",
                            MB_OK | MB_ICONERROR | MB_TOPMOST |
                                MB_SETFOREGROUND);
            } else if (!g_stopping.load() &&
                       !RunAudioOutputPicker(options, initialSelection,
                                             currentSettings.soundFilePath)) {
                MessageBoxW(nullptr,
                            L"The audio output picker window could not be "
                            L"created.",
                            L"Low Battery Alarm",
                            MB_OK | MB_ICONERROR | MB_TOPMOST |
                                MB_SETFOREGROUND);
            }
        }
    } catch (...) {
        Wh_Log(L"An exception occurred in the audio output picker thread.");
    }

    if (comInitialized) {
        CoUninitialize();
    }
    Wh_Log(L"Audio output picker thread stopped.");
    return 0;
}

void RequestAudioOutputPicker() {
    Wh_Log(L"Audio output picker requested.");
    DWORD threadCreationError = ERROR_SUCCESS;
    AcquireSRWLockExclusive(&g_audioPickerLock);
    if (g_audioPickerThread) {
        DWORD waitResult = WaitForSingleObject(g_audioPickerThread, 0);
        if (waitResult == WAIT_TIMEOUT) {
            HWND window = g_audioPickerWindow;
            ReleaseSRWLockExclusive(&g_audioPickerLock);
            if (window) {
                PostMessageW(window, kAudioPickerBringToFrontMessage, 0, 0);
            }
            return;
        }
        CloseHandle(g_audioPickerThread);
        g_audioPickerThread = nullptr;
        g_audioPickerWindow = nullptr;
    }

    g_audioPickerThread = CreateThread(
        nullptr, 0, AudioOutputPickerThreadProcedure, nullptr, 0, nullptr);
    if (!g_audioPickerThread) {
        threadCreationError = GetLastError();
    }
    ReleaseSRWLockExclusive(&g_audioPickerLock);

    if (threadCreationError != ERROR_SUCCESS) {
        LogWin32Failure(L"Creating the audio output picker thread",
                        threadCreationError);
        MessageBoxW(nullptr,
                    L"The audio output picker thread could not be created.",
                    L"Low Battery Alarm", MB_OK | MB_ICONERROR | MB_TOPMOST |
                                                MB_SETFOREGROUND);
    }
}

void StopAudioOutputPicker() {
    AcquireSRWLockShared(&g_audioPickerLock);
    HANDLE thread = g_audioPickerThread;
    HWND window = g_audioPickerWindow;
    if (window) {
        PostMessageW(window, WM_CLOSE, 0, 0);
    }
    ReleaseSRWLockShared(&g_audioPickerLock);

    if (!thread) {
        return;
    }

    DWORD waitResult = WaitForSingleObject(
        thread, kShutdownDiagnosticTimeoutMilliseconds);
    if (waitResult == WAIT_TIMEOUT) {
        Wh_Log(L"The audio output picker did not stop within %lu "
               L"milliseconds; waiting for safe shutdown.",
               kShutdownDiagnosticTimeoutMilliseconds);
        WaitForSingleObject(thread, INFINITE);
    } else if (waitResult == WAIT_FAILED) {
        LogWin32Failure(L"Waiting for the audio output picker",
                        GetLastError());
        WaitForSingleObject(thread, INFINITE);
    }

    AcquireSRWLockExclusive(&g_audioPickerLock);
    if (g_audioPickerThread == thread) {
        CloseHandle(g_audioPickerThread);
        g_audioPickerThread = nullptr;
        g_audioPickerWindow = nullptr;
    }
    ReleaseSRWLockExclusive(&g_audioPickerLock);
}

bool PlayWaveFileInterruptibly(const std::wstring& path,
                               const Settings& settings) {
    LoadedWaveFile wave;
    if (!LoadWaveFile(path, &wave)) {
        return false;
    }

    WAVEFORMATEX* format = wave.Format();
    if (format->nBlockAlign == 0 ||
        wave.audioData.size() % format->nBlockAlign != 0) {
        Wh_Log(L"The WAV data is not aligned to complete audio frames: %s",
               path.c_str());
        return false;
    }

    ComPtr<IMMDeviceEnumerator> enumerator;
    HRESULT result = CoCreateInstance(
        __uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER,
        __uuidof(IMMDeviceEnumerator),
        reinterpret_cast<void**>(enumerator.Put()));
    if (FAILED(result)) {
        LogHResultFailure(L"CoCreateInstance for MMDeviceEnumerator", result);
        return false;
    }

    ComPtr<IMMDevice> device;
    result = ResolveConfiguredAudioDevice(
        enumerator.Get(), settings.audioOutputMode,
        settings.audioOutputDevice, device.Put());
    if (FAILED(result) &&
        settings.audioOutputMode != AudioOutputMode::WindowsDefault) {
        LogHResultFailure(L"Resolving the selected alarm output", result);
        Wh_Log(L"The selected output is unavailable; the "
               L"Windows default output will be used for this alert.");
        result = enumerator->GetDefaultAudioEndpoint(eRender, eConsole,
                                                     device.Put());
    }
    if (FAILED(result)) {
        LogHResultFailure(L"Obtaining the alarm output endpoint", result);
        return false;
    }

    ComPtr<IAudioClient> audioClient;
    result = device->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER,
                              nullptr,
                              reinterpret_cast<void**>(audioClient.Put()));
    if (FAILED(result)) {
        LogHResultFailure(L"IMMDevice::Activate for IAudioClient", result);
        return false;
    }

    HANDLE audioEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!audioEvent) {
        LogWin32Failure(L"CreateEventW for WASAPI playback", GetLastError());
        return false;
    }

    constexpr DWORD streamFlags =
        AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST |
        AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
        AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY;
    result = audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, streamFlags, 0, 0,
                                     format, nullptr);
    if (SUCCEEDED(result)) {
        result = audioClient->SetEventHandle(audioEvent);
    }

    UINT32 bufferFrames = 0;
    if (SUCCEEDED(result)) {
        result = audioClient->GetBufferSize(&bufferFrames);
    }

    ComPtr<IAudioRenderClient> renderClient;
    if (SUCCEEDED(result)) {
        result = audioClient->GetService(
            __uuidof(IAudioRenderClient),
            reinterpret_cast<void**>(renderClient.Put()));
    }

    UINT32 totalFrames = static_cast<UINT32>(
        wave.audioData.size() / format->nBlockAlign);
    UINT32 frameOffset = 0;
    auto writeFrames = [&](UINT32 frameCount) -> HRESULT {
        if (frameCount == 0) {
            return S_OK;
        }
        BYTE* destination = nullptr;
        HRESULT writeResult =
            renderClient->GetBuffer(frameCount, &destination);
        if (FAILED(writeResult)) {
            return writeResult;
        }
        size_t byteOffset =
            static_cast<size_t>(frameOffset) * format->nBlockAlign;
        size_t byteCount =
            static_cast<size_t>(frameCount) * format->nBlockAlign;
        std::memcpy(destination, wave.audioData.data() + byteOffset, byteCount);
        writeResult = renderClient->ReleaseBuffer(frameCount, 0);
        if (SUCCEEDED(writeResult)) {
            frameOffset += frameCount;
        }
        return writeResult;
    };

    if (SUCCEEDED(result)) {
        result = writeFrames((std::min)(bufferFrames, totalFrames));
    }

    bool started = false;
    if (SUCCEEDED(result)) {
        AcquireSRWLockExclusive(&g_playbackControlLock);
        if (!IsPlaybackCancelled()) {
            result = audioClient->Start();
            started = SUCCEEDED(result);
        }
        ReleaseSRWLockExclusive(&g_playbackControlLock);
    }

    bool succeeded = SUCCEEDED(result);
    if (started) {
        HANDLE waitHandles[] = {g_shutdownEvent, g_playbackCancelEvent,
                                audioEvent};
        while (!IsPlaybackCancelled()) {
            if (frameOffset >= totalFrames) {
                UINT32 padding = 0;
                result = audioClient->GetCurrentPadding(&padding);
                if (FAILED(result)) {
                    succeeded = false;
                    break;
                }
                if (padding == 0) {
                    break;
                }
            }

            DWORD waitResult = WaitForMultipleObjects(
                ARRAYSIZE(waitHandles), waitHandles, FALSE, INFINITE);
            if (waitResult == WAIT_OBJECT_0 ||
                waitResult == WAIT_OBJECT_0 + 1) {
                break;
            }
            if (waitResult != WAIT_OBJECT_0 + 2) {
                if (waitResult == WAIT_FAILED) {
                    LogWin32Failure(L"Waiting for WASAPI playback",
                                    GetLastError());
                } else {
                    Wh_Log(L"Unexpected WASAPI wait result: %lu.", waitResult);
                }
                result = HRESULT_FROM_WIN32(ERROR_GEN_FAILURE);
                succeeded = false;
                break;
            }

            if (frameOffset < totalFrames) {
                UINT32 padding = 0;
                result = audioClient->GetCurrentPadding(&padding);
                if (FAILED(result)) {
                    succeeded = false;
                    break;
                }
                if (padding > bufferFrames) {
                    result = E_UNEXPECTED;
                    succeeded = false;
                    break;
                }
                UINT32 availableFrames = bufferFrames - padding;
                UINT32 remainingFrames = totalFrames - frameOffset;
                result =
                    writeFrames((std::min)(availableFrames, remainingFrames));
                if (FAILED(result)) {
                    succeeded = false;
                    break;
                }
            }
        }
    }

    if (started) {
        HRESULT stopResult = audioClient->Stop();
        if (FAILED(stopResult)) {
            LogHResultFailure(L"IAudioClient::Stop", stopResult);
        }
        audioClient->Reset();
    }
    CloseHandle(audioEvent);

    if (!succeeded && !IsPlaybackCancelled()) {
        LogHResultFailure(L"WASAPI WAV playback", result);
        return false;
    }
    return true;
}

class AudioStateGuard {
public:
    AudioStateGuard(const Settings& settings, bool comAvailable)
        : restoreEnabled_(settings.restorePreviousVolume) {
        if (!comAvailable ||
            (!settings.temporarilyIncreaseVolume &&
             !settings.temporarilyUnmute)) {
            return;
        }

        ComPtr<IMMDeviceEnumerator> enumerator;
        HRESULT result = CoCreateInstance(
            __uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER,
            __uuidof(IMMDeviceEnumerator),
            reinterpret_cast<void**>(enumerator.Put()));
        if (FAILED(result)) {
            LogHResultFailure(L"CoCreateInstance for MMDeviceEnumerator", result);
            return;
        }

        ComPtr<IMMDevice> device;
        result = ResolveConfiguredAudioDevice(
            enumerator.Get(), settings.audioOutputMode,
            settings.audioOutputDevice, device.Put());
        if (FAILED(result) &&
            settings.audioOutputMode != AudioOutputMode::WindowsDefault) {
            result = enumerator->GetDefaultAudioEndpoint(eRender, eConsole,
                                                         device.Put());
        }
        if (FAILED(result)) {
            LogHResultFailure(L"Obtaining the alarm output audio endpoint",
                              result);
            return;
        }

        result = device->Activate(
            __uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr,
            reinterpret_cast<void**>(endpoint_.Put()));
        if (FAILED(result)) {
            LogHResultFailure(L"IMMDevice::Activate for IAudioEndpointVolume",
                              result);
            return;
        }

        float originalVolume = 0.0f;
        result = endpoint_->GetMasterVolumeLevelScalar(&originalVolume);
        bool haveOriginalVolume = SUCCEEDED(result);
        if (!haveOriginalVolume) {
            LogHResultFailure(L"GetMasterVolumeLevelScalar", result);
        }

        BOOL originalMute = FALSE;
        result = endpoint_->GetMute(&originalMute);
        bool haveOriginalMute = SUCCEEDED(result);
        if (!haveOriginalMute) {
            LogHResultFailure(L"GetMute", result);
        }

        if (settings.temporarilyIncreaseVolume && haveOriginalVolume) {
            originalVolume_ = originalVolume;
            appliedVolume_ =
                static_cast<float>(settings.alarmVolumePercent) / 100.0f;
            if (std::fabs(originalVolume_ - appliedVolume_) >
                kVolumeComparisonTolerance) {
                result = endpoint_->SetMasterVolumeLevelScalar(appliedVolume_,
                                                                nullptr);
                if (SUCCEEDED(result)) {
                    volumeChanged_ = true;
                    float actualAppliedVolume = appliedVolume_;
                    HRESULT readBackResult = endpoint_->GetMasterVolumeLevelScalar(
                        &actualAppliedVolume);
                    if (SUCCEEDED(readBackResult)) {
                        appliedVolume_ = actualAppliedVolume;
                    } else {
                        LogHResultFailure(
                            L"GetMasterVolumeLevelScalar after applying the "
                            L"alarm volume",
                            readBackResult);
                    }
                    Wh_Log(L"Temporary alarm volume set to %d%%.",
                           settings.alarmVolumePercent);
                } else {
                    LogHResultFailure(L"SetMasterVolumeLevelScalar", result);
                }
            }
        }

        if (settings.temporarilyUnmute && haveOriginalMute && originalMute) {
            originalMute_ = originalMute;
            appliedMute_ = FALSE;
            result = endpoint_->SetMute(appliedMute_, nullptr);
            if (SUCCEEDED(result)) {
                muteChanged_ = true;
                Wh_Log(L"The selected output device was temporarily unmuted.");
            } else {
                LogHResultFailure(L"IAudioEndpointVolume::SetMute", result);
            }
        }
    }

    ~AudioStateGuard() {
        Restore();
    }

    AudioStateGuard(const AudioStateGuard&) = delete;
    AudioStateGuard& operator=(const AudioStateGuard&) = delete;

private:
    void Restore() {
        if (!restoreEnabled_ || !endpoint_) {
            return;
        }

        if (volumeChanged_) {
            float currentVolume = 0.0f;
            HRESULT result =
                endpoint_->GetMasterVolumeLevelScalar(&currentVolume);
            if (FAILED(result)) {
                LogHResultFailure(
                    L"GetMasterVolumeLevelScalar during restoration", result);
            } else if (std::fabs(currentVolume - appliedVolume_) <=
                       kVolumeComparisonTolerance) {
                result = endpoint_->SetMasterVolumeLevelScalar(originalVolume_,
                                                                nullptr);
                if (FAILED(result)) {
                    LogHResultFailure(
                        L"SetMasterVolumeLevelScalar during restoration",
                        result);
                } else {
                    Wh_Log(L"The previous master volume was restored.");
                }
            } else {
                Wh_Log(L"The master volume changed during playback; the user's "
                       L"current value was preserved.");
            }
            volumeChanged_ = false;
        }

        if (muteChanged_) {
            BOOL currentMute = FALSE;
            HRESULT result = endpoint_->GetMute(&currentMute);
            if (FAILED(result)) {
                LogHResultFailure(L"GetMute during restoration", result);
            } else if (currentMute == appliedMute_) {
                result = endpoint_->SetMute(originalMute_, nullptr);
                if (FAILED(result)) {
                    LogHResultFailure(L"SetMute during restoration", result);
                } else {
                    Wh_Log(L"The previous mute state was restored.");
                }
            } else {
                Wh_Log(L"The mute state changed during playback; the user's "
                       L"current value was preserved.");
            }
            muteChanged_ = false;
        }
    }

    ComPtr<IAudioEndpointVolume> endpoint_;
    bool restoreEnabled_ = false;
    bool volumeChanged_ = false;
    bool muteChanged_ = false;
    float originalVolume_ = 0.0f;
    float appliedVolume_ = 0.0f;
    BOOL originalMute_ = FALSE;
    BOOL appliedMute_ = FALSE;
};

bool ResolveWindowsCriticalSoundPath(std::wstring* path) {
    if (!path) {
        return false;
    }

    constexpr wchar_t registryPath[] =
        L"AppEvents\\Schemes\\Apps\\.Default\\SystemHand\\.Current";
    DWORD requiredBytes = 0;
    LSTATUS status = RegGetValueW(
        HKEY_CURRENT_USER, registryPath, nullptr,
        RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ, nullptr, nullptr,
        &requiredBytes);
    if (status == ERROR_SUCCESS && requiredBytes >= sizeof(wchar_t)) {
        try {
            std::vector<wchar_t> buffer(
                requiredBytes / sizeof(wchar_t) + 1, L'\0');
            DWORD bufferBytes =
                static_cast<DWORD>(buffer.size() * sizeof(wchar_t));
            status = RegGetValueW(
                HKEY_CURRENT_USER, registryPath, nullptr,
                RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ, nullptr, buffer.data(),
                &bufferBytes);
            if (status == ERROR_SUCCESS && buffer.front() != L'\0') {
                std::wstring candidate = TrimAndUnquote(buffer.data());
                DWORD attributes = GetFileAttributesW(candidate.c_str());
                if (attributes != INVALID_FILE_ATTRIBUTES &&
                    (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0 &&
                    HasWavExtension(candidate)) {
                    *path = std::move(candidate);
                    return true;
                }
            }
        } catch (...) {
            Wh_Log(L"Unable to allocate the Windows critical-sound path.");
        }
    }

    wchar_t windowsDirectory[MAX_PATH]{};
    UINT length = GetWindowsDirectoryW(windowsDirectory,
                                       ARRAYSIZE(windowsDirectory));
    if (length == 0 || length >= ARRAYSIZE(windowsDirectory)) {
        return false;
    }
    try {
        std::wstring candidate = windowsDirectory;
        candidate += L"\\Media\\Windows Critical Stop.wav";
        DWORD attributes = GetFileAttributesW(candidate.c_str());
        if (attributes == INVALID_FILE_ATTRIBUTES ||
            (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
            return false;
        }
        *path = std::move(candidate);
        return true;
    } catch (...) {
        return false;
    }
}

bool PlaySystemAliasFallbackInterruptibly() {
    AcquireSRWLockExclusive(&g_playbackControlLock);
    if (IsPlaybackCancelled()) {
        ReleaseSRWLockExclusive(&g_playbackControlLock);
        return true;
    }

    SetLastError(ERROR_SUCCESS);
    BOOL started = PlaySoundW(L"SystemHand", nullptr,
                              SND_ALIAS | SND_NODEFAULT | SND_SYSTEM |
                                  SND_ASYNC);
    ReleaseSRWLockExclusive(&g_playbackControlLock);
    if (!started) {
        return false;
    }

    WaitForPlaybackDelay(kFallbackSoundDurationMilliseconds);
    PlaySoundW(nullptr, nullptr, 0);
    return true;
}

bool PlayFallbackSound(const Settings& settings) {
    Wh_Log(L"Using the Windows critical system sound fallback.");
    std::wstring fallbackPath;
    bool result = ResolveWindowsCriticalSoundPath(&fallbackPath) &&
                  PlayWaveFileInterruptibly(fallbackPath, settings);
    if (!result && !IsPlaybackCancelled()) {
        Wh_Log(L"The Windows critical-sound WAV could not be routed through "
               L"the selected output; using the system alias as a last resort.");
        result = PlaySystemAliasFallbackInterruptibly();
    }
    if (!result && !IsPlaybackCancelled()) {
        LogWin32Failure(L"Playing the Windows fallback sound",
                        GetLastError());
        return false;
    }
    return result;
}

void PlayAlert(const PlaybackRequest& request, bool comAvailable) {
    if (IsPlaybackCancelled()) {
        return;
    }

    Wh_Log(L"Starting %s sound playback.", PlaybackKindName(request.kind));
    AudioStateGuard audioState(request.settings, comAvailable);

    std::wstring resolvedPath;
    bool useCustomSound = ResolveCustomSoundPath(
        request.settings.soundFilePath, &resolvedPath);
    if (useCustomSound) {
        Wh_Log(L"Using custom WAV file: %s", resolvedPath.c_str());
    } else if (!request.settings.useSystemSoundIfFileInvalid) {
        Wh_Log(L"The Windows fallback sound is disabled; this alert is silent.");
        return;
    }

    for (int repetition = 0;
         repetition < request.settings.repetitionsPerAlert; ++repetition) {
        if (IsPlaybackCancelled()) {
            Wh_Log(L"%s sound playback was cancelled.",
                   PlaybackKindName(request.kind));
            return;
        }

        if (useCustomSound) {
            bool played =
                PlayWaveFileInterruptibly(resolvedPath, request.settings);
            if (!played && !IsPlaybackCancelled()) {
                Wh_Log(L"The custom WAV file could not be played: %s",
                       resolvedPath.c_str());
                useCustomSound = false;
                if (!request.settings.useSystemSoundIfFileInvalid) {
                    Wh_Log(L"The Windows fallback sound is disabled; remaining "
                           L"repetitions are silent.");
                    return;
                }
                PlayFallbackSound(request.settings);
            }
        } else {
            PlayFallbackSound(request.settings);
        }

        if (IsPlaybackCancelled()) {
            Wh_Log(L"%s sound playback was cancelled.",
                   PlaybackKindName(request.kind));
            return;
        }

        if (repetition + 1 < request.settings.repetitionsPerAlert &&
            request.settings.pauseBetweenRepetitionsMilliseconds > 0 &&
            !WaitForPlaybackDelay(static_cast<DWORD>(
                request.settings.pauseBetweenRepetitionsMilliseconds))) {
            Wh_Log(L"%s sound playback was cancelled during a repetition "
                   L"pause.",
                   PlaybackKindName(request.kind));
            return;
        }
    }

    Wh_Log(L"Finished %s sound playback.", PlaybackKindName(request.kind));
}

DWORD WINAPI PlaybackThreadProcedure(void*) {
    Wh_Log(L"Playback thread started.");

    HRESULT comResult = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    bool comAvailable = SUCCEEDED(comResult);
    bool mustUninitializeCom = SUCCEEDED(comResult);
    if (!comAvailable) {
        LogHResultFailure(L"CoInitializeEx on the playback thread", comResult);
        Wh_Log(L"Temporary volume management is unavailable, but sound "
               L"playback will still be attempted.");
    }

    HANDLE waitHandles[] = {g_shutdownEvent, g_playbackRequestEvent};
    while (!g_stopping.load()) {
        DWORD waitResult = WaitForMultipleObjects(ARRAYSIZE(waitHandles),
                                                  waitHandles, FALSE, INFINITE);
        if (waitResult == WAIT_OBJECT_0) {
            break;
        }
        if (waitResult == WAIT_FAILED) {
            LogWin32Failure(L"Playback thread wait", GetLastError());
            break;
        }
        if (waitResult != WAIT_OBJECT_0 + 1) {
            Wh_Log(L"Playback thread received an unexpected wait result: %lu.",
                   waitResult);
            break;
        }

        PlaybackRequest request;
        if (!TakePendingPlayback(&request)) {
            continue;
        }

        try {
            PlayAlert(request, comAvailable);
        } catch (...) {
            Wh_Log(L"An exception occurred during sound playback.");
            StopActiveWaveform();
        }
        FinishPlayback();
    }

    StopActiveWaveform();
    AcquireSRWLockExclusive(&g_playbackLock);
    g_playbackActive = false;
    g_hasPendingPlayback = false;
    ReleaseSRWLockExclusive(&g_playbackLock);

    if (mustUninitializeCom) {
        CoUninitialize();
    }

    Wh_Log(L"Playback thread stopped.");
    return 0;
}

class PowerStatusLogger {
public:
    BatteryAlertState Evaluate(const Settings& settings, bool* acConnectedNow) {
        SYSTEM_POWER_STATUS powerStatus{};
        if (!GetSystemPowerStatus(&powerStatus)) {
            DWORD error = GetLastError();
            if (!queryFailed_ || error != lastQueryError_) {
                LogWin32Failure(L"GetSystemPowerStatus", error);
            }
            queryFailed_ = true;
            lastQueryError_ = error;
            *acConnectedNow = false;
            return BatteryAlertState::Normal;
        }

        if (queryFailed_) {
            Wh_Log(L"GetSystemPowerStatus recovered.");
            queryFailed_ = false;
            lastQueryError_ = ERROR_SUCCESS;
        }

        bool validAcStatus = powerStatus.ACLineStatus == 0 ||
                             powerStatus.ACLineStatus == 1;
        bool unknownBatteryFlag = powerStatus.BatteryFlag == 255;
        constexpr BYTE validBatteryFlagBits = 1 | 2 | 4 | 8 | 128;
        bool invalidBatteryFlag =
            !unknownBatteryFlag &&
            (powerStatus.BatteryFlag & ~validBatteryFlagBits) != 0;
        bool noBattery = !unknownBatteryFlag &&
                         (powerStatus.BatteryFlag & 128) != 0;
        bool validPercentage = powerStatus.BatteryLifePercent <= 100;
        bool reliable = validAcStatus && !unknownBatteryFlag &&
                        !invalidBatteryFlag && !noBattery && validPercentage;

        if (powerStatus.ACLineStatus != lastAcStatus_) {
            if (powerStatus.ACLineStatus == 0) {
                Wh_Log(L"External power disconnected; the system is running on "
                       L"battery power.");
            } else if (powerStatus.ACLineStatus == 1) {
                Wh_Log(L"External power connected.");
            } else {
                Wh_Log(L"The power-source status is unknown or invalid: %u.",
                       static_cast<unsigned int>(powerStatus.ACLineStatus));
            }
            lastAcStatus_ = powerStatus.ACLineStatus;
        }

        if (noBattery != noBattery_) {
            if (noBattery) {
                Wh_Log(L"No system battery was detected.");
            } else if (noBattery_) {
                Wh_Log(L"A system battery was detected.");
            }
            noBattery_ = noBattery;
        }

        bool statusUnknown = !validAcStatus || unknownBatteryFlag ||
                             invalidBatteryFlag ||
                             (!noBattery && !validPercentage);
        if (statusUnknown != statusUnknown_) {
            if (statusUnknown) {
                Wh_Log(L"Battery information is unknown or invalid; alerts are "
                       L"suppressed.");
            } else {
                Wh_Log(L"Reliable battery information is available again.");
            }
            statusUnknown_ = statusUnknown;
        }

        if (validPercentage && !noBattery &&
            static_cast<int>(powerStatus.BatteryLifePercent) !=
                lastBatteryPercentage_) {
            lastBatteryPercentage_ = powerStatus.BatteryLifePercent;
            Wh_Log(L"Battery charge is %d%%.", lastBatteryPercentage_);
        }

        *acConnectedNow = powerStatus.ACLineStatus == 1;
        if (!reliable || powerStatus.ACLineStatus == 1) {
            return BatteryAlertState::Normal;
        }

        int percentage = powerStatus.BatteryLifePercent;
        if (percentage <= settings.criticalBatteryThreshold) {
            return BatteryAlertState::Critical;
        }
        if (percentage <= settings.lowBatteryThreshold) {
            return BatteryAlertState::Low;
        }
        return BatteryAlertState::Normal;
    }

private:
    bool queryFailed_ = false;
    DWORD lastQueryError_ = ERROR_SUCCESS;
    int lastAcStatus_ = -1;
    int lastBatteryPercentage_ = -1;
    bool noBattery_ = false;
    bool statusUnknown_ = false;
};

PCWSTR BatteryStateName(BatteryAlertState state) {
    switch (state) {
        case BatteryAlertState::Normal:
            return L"Normal";
        case BatteryAlertState::Low:
            return L"Low";
        case BatteryAlertState::Critical:
            return L"Critical";
    }

    return L"Unknown";
}

PlaybackKind PlaybackKindForState(BatteryAlertState state) {
    return state == BatteryAlertState::Critical ? PlaybackKind::Critical
                                                : PlaybackKind::Low;
}

int RepeatIntervalForState(BatteryAlertState state,
                           const Settings& settings) {
    return state == BatteryAlertState::Critical
               ? settings.criticalRepeatIntervalSeconds
               : settings.lowRepeatIntervalSeconds;
}

bool RunMonitorThread() {
    Settings settings;
    if (!GetSettingsSnapshot(&settings)) {
        Wh_Log(L"The monitoring thread could not obtain its initial settings "
               L"snapshot.");
        return false;
    }

    PowerStatusLogger statusLogger;
    BatteryAlertState currentState = BatteryAlertState::Normal;
    ULONGLONG nextBatteryCheck = 0;
    ULONGLONG nextAlert = kNoDeadline;
    ULONGLONG lastBatteryAlertRequest = 0;
    bool firstEvaluation = true;
    bool wasAcConnected = false;

    HANDLE waitHandles[] = {g_shutdownEvent, g_settingsChangedEvent,
                            g_powerSourceChangedEvent,
                            g_playbackFinishedEvent};

    while (!g_stopping.load()) {
        ULONGLONG now = GetTickCount64();
        bool evaluatedBatteryThisIteration = false;

        if (now >= nextBatteryCheck) {
            evaluatedBatteryThisIteration = true;
            bool acConnected = false;
            BatteryAlertState newState =
                statusLogger.Evaluate(settings, &acConnected);
            g_externalPowerConnected.store(acConnected);

            if (acConnected && (!wasAcConnected ||
                                currentState != BatteryAlertState::Normal)) {
                CancelAllPlayback();
            }
            wasAcConnected = acConnected;

            if (newState != currentState) {
                Wh_Log(L"Battery alert state changed from %s to %s.",
                       BatteryStateName(currentState),
                       BatteryStateName(newState));
                currentState = newState;

                if (currentState == BatteryAlertState::Normal) {
                    CancelAllPlayback();
                    nextAlert = kNoDeadline;
                    lastBatteryAlertRequest = 0;
                } else {
                    if (QueuePlayback(PlaybackKindForState(currentState),
                                      settings, true)) {
                        lastBatteryAlertRequest = now;
                        nextAlert = now + SecondsToTicks(
                                              RepeatIntervalForState(
                                                  currentState, settings));
                    } else {
                        lastBatteryAlertRequest = 0;
                        nextAlert = now + 1000;
                    }
                }
            }

            nextBatteryCheck =
                now + SecondsToTicks(settings.pollingIntervalSeconds);
            if (currentState != BatteryAlertState::Normal &&
                IsPlaybackBusy()) {
                ULONGLONG cancellationCheck =
                    now + kActivePlaybackPowerCheckMilliseconds;
                if (cancellationCheck < nextBatteryCheck) {
                    nextBatteryCheck = cancellationCheck;
                }
            }

            if (firstEvaluation) {
                firstEvaluation = false;
                g_initialEvaluationCompleted.store(true);
                if (!SetEvent(g_initialEvaluationEvent)) {
                    LogWin32Failure(L"SetEvent for initial battery evaluation",
                                    GetLastError());
                }
            }
        }

        now = GetTickCount64();
        if (currentState != BatteryAlertState::Normal && now >= nextAlert &&
            !IsPlaybackBusy()) {
            // Recheck AC power immediately before every repeated alarm.
            if (!evaluatedBatteryThisIteration) {
                nextBatteryCheck = 0;
                continue;
            }
            if (QueuePlayback(PlaybackKindForState(currentState), settings,
                              false)) {
                lastBatteryAlertRequest = now;
                nextAlert = now + SecondsToTicks(
                                      RepeatIntervalForState(currentState,
                                                             settings));
                ULONGLONG cancellationCheck =
                    now + kActivePlaybackPowerCheckMilliseconds;
                if (cancellationCheck < nextBatteryCheck) {
                    nextBatteryCheck = cancellationCheck;
                }
            } else {
                nextAlert = now + 1000;
            }
        }

        now = GetTickCount64();
        DWORD waitMilliseconds = TicksUntil(now, nextBatteryCheck);
        if (currentState != BatteryAlertState::Normal &&
            !IsPlaybackBusy()) {
            DWORD alertWait = TicksUntil(now, nextAlert);
            if (alertWait < waitMilliseconds) {
                waitMilliseconds = alertWait;
            }
        }

        DWORD waitResult = WaitForMultipleObjects(
            ARRAYSIZE(waitHandles), waitHandles, FALSE, waitMilliseconds);
        if (waitResult == WAIT_OBJECT_0) {
            break;
        }
        if (waitResult == WAIT_OBJECT_0 + 1) {
            Settings updatedSettings;
            if (GetSettingsSnapshot(&updatedSettings)) {
                settings = std::move(updatedSettings);
                Wh_Log(L"Settings reloaded by the monitoring thread.");
                nextBatteryCheck = 0;
                if (currentState != BatteryAlertState::Normal &&
                    lastBatteryAlertRequest != 0) {
                    nextAlert = lastBatteryAlertRequest +
                                SecondsToTicks(RepeatIntervalForState(
                                    currentState, settings));
                }
            }
            continue;
        }
        if (waitResult == WAIT_OBJECT_0 + 2) {
            nextBatteryCheck = 0;
            continue;
        }
        if (waitResult == WAIT_OBJECT_0 + 3 || waitResult == WAIT_TIMEOUT) {
            continue;
        }
        if (waitResult == WAIT_FAILED) {
            LogWin32Failure(L"Monitoring thread wait", GetLastError());
            break;
        }

        Wh_Log(L"Monitoring thread received an unexpected wait result: %lu.",
               waitResult);
        break;
    }

    CancelAllPlayback();
    return true;
}

DWORD WINAPI MonitorThreadProcedure(void*) {
    Wh_Log(L"Battery monitoring thread started.");
    try {
        RunMonitorThread();
    } catch (...) {
        Wh_Log(L"An exception occurred in the battery monitoring thread.");
    }

    if (!g_initialEvaluationCompleted.load()) {
        SetEvent(g_initialEvaluationEvent);
    }
    Wh_Log(L"Battery monitoring thread stopped.");
    return 0;
}

bool CreateSynchronizationObjects() {
    g_shutdownEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_shutdownEvent) {
        LogWin32Failure(L"CreateEventW for shutdown", GetLastError());
        return false;
    }

    g_settingsChangedEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_settingsChangedEvent) {
        LogWin32Failure(L"CreateEventW for settings changes", GetLastError());
        return false;
    }

    g_playbackCancelEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_playbackCancelEvent) {
        LogWin32Failure(L"CreateEventW for playback cancellation",
                        GetLastError());
        return false;
    }

    g_playbackRequestEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_playbackRequestEvent) {
        LogWin32Failure(L"CreateEventW for playback requests", GetLastError());
        return false;
    }

    g_playbackFinishedEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_playbackFinishedEvent) {
        LogWin32Failure(L"CreateEventW for playback completion",
                        GetLastError());
        return false;
    }

    g_initialEvaluationEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_initialEvaluationEvent) {
        LogWin32Failure(L"CreateEventW for initial battery evaluation",
                        GetLastError());
        return false;
    }

    g_powerNotificationReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_powerNotificationReadyEvent) {
        LogWin32Failure(L"CreateEventW for power notification readiness",
                        GetLastError());
        return false;
    }

    g_powerSourceChangedEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_powerSourceChangedEvent) {
        LogWin32Failure(L"CreateEventW for power-source changes",
                        GetLastError());
        return false;
    }

    return true;
}

void SignalWorkersToStop() {
    g_stopping.store(true);
    if (g_shutdownEvent) {
        SetEvent(g_shutdownEvent);
    }
    if (g_playbackCancelEvent) {
        SetEvent(g_playbackCancelEvent);
    }
    if (g_settingsChangedEvent) {
        SetEvent(g_settingsChangedEvent);
    }
    if (g_playbackRequestEvent) {
        SetEvent(g_playbackRequestEvent);
    }
    if (g_playbackFinishedEvent) {
        SetEvent(g_playbackFinishedEvent);
    }
    if (g_powerSourceChangedEvent) {
        SetEvent(g_powerSourceChangedEvent);
    }
    ClosePowerNotificationController();
    StopActiveWaveform();
}

void WaitForWorkers() {
    HANDLE threads[3]{};
    DWORD threadCount = 0;
    if (g_monitorThread) {
        threads[threadCount++] = g_monitorThread;
    }
    if (g_playbackThread) {
        threads[threadCount++] = g_playbackThread;
    }
    if (g_powerNotificationThread) {
        threads[threadCount++] = g_powerNotificationThread;
    }
    if (threadCount == 0) {
        g_workersJoined = true;
        return;
    }

    DWORD waitResult = WaitForMultipleObjects(
        threadCount, threads, TRUE, kShutdownDiagnosticTimeoutMilliseconds);
    if (waitResult == WAIT_TIMEOUT) {
        Wh_Log(L"Worker-thread shutdown exceeded %lu milliseconds; waiting "
               L"until safe cleanup is possible.",
               kShutdownDiagnosticTimeoutMilliseconds);
        StopActiveWaveform();
        waitResult = WaitForMultipleObjects(threadCount, threads, TRUE, INFINITE);
    }

    if (waitResult == WAIT_FAILED) {
        LogWin32Failure(L"Worker-thread shutdown wait", GetLastError());
        for (DWORD index = 0; index < threadCount; ++index) {
            WaitForSingleObject(threads[index], INFINITE);
        }
    }

    g_workersJoined = true;
}

void StopAndJoinWorkers() {
    SignalWorkersToStop();
    WaitForWorkers();
}

void CloseHandleIfPresent(HANDLE* handle) {
    if (*handle) {
        CloseHandle(*handle);
        *handle = nullptr;
    }
}

void CleanupHandles() {
    if (!g_workersJoined &&
        (g_monitorThread || g_playbackThread || g_powerNotificationThread)) {
        Wh_Log(L"Cleanup was requested before worker threads were joined; "
               L"joining them now.");
        StopAndJoinWorkers();
    }

    CloseHandleIfPresent(&g_monitorThread);
    CloseHandleIfPresent(&g_playbackThread);
    CloseHandleIfPresent(&g_powerNotificationThread);
    CloseHandleIfPresent(&g_initialEvaluationEvent);
    CloseHandleIfPresent(&g_powerSourceChangedEvent);
    CloseHandleIfPresent(&g_powerNotificationReadyEvent);
    CloseHandleIfPresent(&g_playbackFinishedEvent);
    CloseHandleIfPresent(&g_playbackRequestEvent);
    CloseHandleIfPresent(&g_playbackCancelEvent);
    CloseHandleIfPresent(&g_settingsChangedEvent);
    CloseHandleIfPresent(&g_shutdownEvent);

}

bool StartWorkers() {
    g_playbackThread =
        CreateThread(nullptr, 0, PlaybackThreadProcedure, nullptr, 0, nullptr);
    if (!g_playbackThread) {
        LogWin32Failure(L"CreateThread for sound playback", GetLastError());
        return false;
    }

    g_powerNotificationThread = CreateThread(
        nullptr, 0, PowerNotificationThreadProcedure, nullptr, 0, nullptr);
    if (!g_powerNotificationThread) {
        LogWin32Failure(L"CreateThread for power notifications",
                        GetLastError());
        return false;
    }

    HANDLE powerNotificationWaitHandles[] = {
        g_powerNotificationReadyEvent, g_powerNotificationThread};
    DWORD powerNotificationWaitResult = WaitForMultipleObjects(
        ARRAYSIZE(powerNotificationWaitHandles), powerNotificationWaitHandles,
        FALSE, kInitialEvaluationTimeoutMilliseconds);
    if (powerNotificationWaitResult != WAIT_OBJECT_0 ||
        !g_powerNotificationInitialized.load()) {
        if (powerNotificationWaitResult == WAIT_TIMEOUT) {
            Wh_Log(L"Power notification initialization timed out after %lu "
                   L"milliseconds.",
                   kInitialEvaluationTimeoutMilliseconds);
        } else if (powerNotificationWaitResult == WAIT_FAILED) {
            LogWin32Failure(L"Power notification initialization wait",
                            GetLastError());
        } else if (powerNotificationWaitResult == WAIT_OBJECT_0 + 1) {
            Wh_Log(L"The power notification thread stopped before registering "
                   L"for AC/DC changes.");
        } else {
            Wh_Log(L"The power notification thread could not register for "
                   L"AC/DC changes.");
        }
        return false;
    }

    g_monitorThread =
        CreateThread(nullptr, 0, MonitorThreadProcedure, nullptr, 0, nullptr);
    if (!g_monitorThread) {
        LogWin32Failure(L"CreateThread for battery monitoring", GetLastError());
        return false;
    }

    HANDLE initialWaitHandles[] = {g_initialEvaluationEvent, g_monitorThread};
    DWORD waitResult = WaitForMultipleObjects(
        ARRAYSIZE(initialWaitHandles), initialWaitHandles, FALSE,
        kInitialEvaluationTimeoutMilliseconds);
    if (waitResult != WAIT_OBJECT_0 ||
        !g_initialEvaluationCompleted.load()) {
        if (waitResult == WAIT_TIMEOUT) {
            Wh_Log(L"Initial battery evaluation timed out after %lu "
                   L"milliseconds.",
                   kInitialEvaluationTimeoutMilliseconds);
        } else if (waitResult == WAIT_FAILED) {
            LogWin32Failure(L"Initial battery evaluation wait", GetLastError());
        } else {
            Wh_Log(L"The monitoring thread stopped before completing the "
                   L"initial battery evaluation.");
        }
        return false;
    }

    return true;
}

}  // namespace

BOOL WhTool_ModInit() {
    Wh_Log(L"Low Battery Alarm initialization started.");

    try {
        g_stopping.store(false);
        g_initialEvaluationCompleted.store(false);
        g_powerNotificationInitialized.store(false);
        g_externalPowerConnected.store(false);
        g_lastNotifiedPowerCondition.store(-1);
        g_workersJoined = false;

        Settings initialSettings;
        if (!LoadSettings(&initialSettings)) {
            Wh_Log(L"Initial settings could not be loaded.");
            return FALSE;
        }
        bool requestAudioPickerOnStartup =
            InitializeConfigurationTrigger(initialSettings);
        if (!ReplaceSettings(std::move(initialSettings))) {
            return FALSE;
        }

        if (!CreateSynchronizationObjects()) {
            CleanupHandles();
            return FALSE;
        }

        if (!StartWorkers()) {
            Wh_Log(L"Worker threads could not be started safely.");
            StopAndJoinWorkers();
            CleanupHandles();
            return FALSE;
        }

        if (requestAudioPickerOnStartup) {
            RequestAudioOutputPicker();
        }

        Wh_Log(L"Low Battery Alarm initialized successfully.");
        return TRUE;
    } catch (...) {
        Wh_Log(L"An exception occurred during Low Battery Alarm "
               L"initialization.");
        StopAndJoinWorkers();
        CleanupHandles();
        return FALSE;
    }
}

void WhTool_ModSettingsChanged() {
    if (g_stopping.load()) {
        return;
    }

    try {
        Settings previousSettings;
        if (!GetSettingsSnapshot(&previousSettings)) {
            return;
        }

        Settings updatedSettings;
        if (!LoadSettings(&updatedSettings)) {
            Wh_Log(L"Settings reload failed; the previous settings remain "
                   L"active.");
            return;
        }

        if (updatedSettings.configuredSoundFilePath !=
            previousSettings.configuredSoundFilePath) {
            if (!Wh_SetStringValue(kStoredSoundFilePath, L"")) {
                Wh_Log(L"The manual WAV path is active, but the previously "
                       L"stored picker path could not be cleared.");
            }
            updatedSettings.pickerSoundFilePath.clear();
            updatedSettings.soundFilePath =
                updatedSettings.configuredSoundFilePath;
        }

        bool cancelPlayback =
            PlaybackSettingsDiffer(previousSettings, updatedSettings);
        bool requestTest = updatedSettings.testSoundAfterSettingsChange;
        if (!ReplaceSettings(updatedSettings)) {
            Wh_Log(L"Settings reload failed; the previous settings remain "
                   L"active.");
            return;
        }
        Wh_Log(L"Settings reloaded successfully.");
        bool requestAudioPicker = ConsumeConfigurationTrigger(
            previousSettings, updatedSettings);

        if (cancelPlayback) {
            CancelAllPlayback();
        }

        if (g_settingsChangedEvent && !SetEvent(g_settingsChangedEvent)) {
            LogWin32Failure(L"SetEvent for settings changes", GetLastError());
        }

        if (requestTest && !requestAudioPicker) {
            QueuePlayback(PlaybackKind::Test, updatedSettings, false);
        }
        if (requestAudioPicker) {
            RequestAudioOutputPicker();
        }
    } catch (...) {
        Wh_Log(L"An exception occurred while applying changed settings.");
    }
}

void WhTool_ModUninit() {
    Wh_Log(L"Low Battery Alarm shutdown started.");

    if (g_stopping.exchange(true)) {
        return;
    }

    try {
        StopAudioOutputPicker();
        CancelAllPlayback();
        StopAndJoinWorkers();
        CleanupHandles();
        Wh_Log(L"All Low Battery Alarm worker activity has stopped.");
    } catch (...) {
        Wh_Log(L"An exception occurred while stopping Low Battery Alarm; "
               L"forcing a final safe worker join.");
        StopAudioOutputPicker();
        SignalWorkersToStop();
        WaitForWorkers();
        CleanupHandles();
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
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
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }
    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }
    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
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

    if (isExcluded) {
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
