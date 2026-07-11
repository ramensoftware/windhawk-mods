// ==WindhawkMod==
// @id              low-battery-alarm
// @name            Low Battery Alarm
// @description     Plays a customizable alarm when a laptop battery reaches a low or critical charge level
// @version         1.0.0
// @author          communism420
// @github          https://github.com/communism420
// @homepage        https://github.com/communism420
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lwinmm -lole32 -luuid -lcomdlg32 -lgdi32 -ladvapi32
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

Enable **Configure sound source** and save the settings to open the sound-source
window. Choose a method with its radio button. In **Windows file picker** mode,
use **Browse...** to select a WAV file. In **Manual full path** mode, type or paste
the complete path into the editable field. Both paths remain visible, but the
controls belonging to the inactive method are disabled. The paths are independent,
and only the selected method supplies the alarm sound path.

The same window contains **Alarm output device**. Choose **Windows default
output** to follow the normal Windows route, or choose a specific active output
such as the laptop speakers to route the alarm directly to that device. If the
saved device is temporarily unavailable, the mod logs the condition and uses the
Windows default output for that alert so that the alarm is not silently lost.

WAV is the guaranteed supported custom format. If no file has been selected, or
if the selected file is later removed, becomes inaccessible, is not a WAV file,
or cannot be played, the mod logs the reason. When the fallback option is
enabled, the Windows critical system sound is used instead. A short, clearly
audible, and recognizable WAV file is recommended.

## Volume and testing

The mod can temporarily change the default output device's master volume and can
temporarily unmute it while an alert is playing. The previous volume and mute
state are normally restored afterward. If the user changes the volume during
playback, the new value is preserved when the change can be detected by comparing
it with the exact value applied by the mod.

Enable **Test sound after settings change** and save the settings to request one
test alert. The test uses the current sound and volume settings without changing
the battery state machine. The option is evaluated only when settings are saved;
it does not run during normal polling.

Desktop computers without a system battery are ignored. Except for the requested
sound-source window and the standard Windows file picker, the mod creates no
persistent UI, console, tray icon, child process, or external player. The alarm
runs inside Explorer and uses low-overhead battery-monitoring, power-notification,
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
- ConfigureSoundSource: false
  $name: Configure sound source
  $description: To open the window with two audio-file selection methods and an alarm output-device list, first turn this option off and save the settings, then turn it on and save the settings again. Repeat these steps whenever you need to reopen the window.
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
  $description: Temporarily increase the default output-device volume while the alarm is playing.
- AlarmVolumePercent: 100
  $name: Alarm volume
  $description: Temporary volume level used for the alarm.
- TemporarilyUnmute: true
  $name: Temporarily unmute
  $description: Temporarily unmute the default output device while the alarm is playing.
- RestorePreviousVolume: true
  $name: Restore previous audio state
  $description: Restore the previous volume and mute state after the alarm finishes.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <commdlg.h>
#include <endpointvolume.h>
#include <functiondiscoverykeys_devpkey.h>
#include <mmddk.h>
#include <mmdeviceapi.h>
#include <mmsystem.h>
#include <objbase.h>
#include <propsys.h>

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
constexpr wchar_t kSoundSelectionMethodStorageName[] =
    L"SoundFileSelectionMethod";
constexpr wchar_t kSelectedSoundPathStorageName[] = L"SelectedSoundFilePath";
constexpr wchar_t kManualSoundPathStorageName[] = L"ManualSoundFilePath";
constexpr wchar_t kAudioOutputDeviceIdStorageName[] = L"AudioOutputDeviceId";
constexpr wchar_t kAudioOutputDeviceNameStorageName[] =
    L"AudioOutputDeviceName";
constexpr wchar_t kSoundConfigurationWindowClass[] =
    L"WindhawkLowBatteryAlarmSoundConfiguration";
constexpr wchar_t kPowerNotificationWindowClass[] =
    L"WindhawkLowBatteryAlarmPowerNotification";

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

enum class SoundFileSelectionMethod {
    WindowsFilePicker,
    ManualPath
};

struct Settings {
    int lowBatteryThreshold = 10;
    int criticalBatteryThreshold = 5;
    int pollingIntervalSeconds = 15;
    int lowRepeatIntervalSeconds = 120;
    int criticalRepeatIntervalSeconds = 30;
    SoundFileSelectionMethod soundFileSelectionMethod =
        SoundFileSelectionMethod::WindowsFilePicker;
    std::wstring soundFilePath;
    std::wstring audioOutputDeviceId;
    std::wstring audioOutputDeviceName;
    bool configureSoundSource = false;
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
HWAVEOUT g_activeWaveOut = nullptr;

SRWLOCK g_soundConfigurationLock = SRWLOCK_INIT;
HANDLE g_soundConfigurationThread = nullptr;
std::atomic<HWND> g_soundConfigurationWindow{nullptr};
std::atomic<HWND> g_filePickerWindow{nullptr};

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
HANDLE g_instanceMutex = nullptr;


std::atomic<bool> g_stopping{false};
std::atomic<bool> g_initialEvaluationCompleted{false};
bool g_isPrimaryInstance = false;
bool g_workersJoined = false;

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

bool LoadStoredString(PCWSTR valueName, std::wstring* value) {
    if (!valueName || !value) {
        return false;
    }

    try {
        std::vector<wchar_t> buffer(32768);
        size_t length =
            Wh_GetStringValue(valueName, buffer.data(), buffer.size());
        if (length == 0) {
            value->clear();
        } else {
            value->assign(buffer.data(), length);
        }
        return true;
    } catch (...) {
        Wh_Log(L"An exception occurred while loading a stored string value.");
        return false;
    }
}

bool LoadStoredSoundFileSelectionMethod(SoundFileSelectionMethod* method) {
    if (!method) {
        return false;
    }

    int storedMethod = Wh_GetIntValue(
        kSoundSelectionMethodStorageName,
        static_cast<int>(SoundFileSelectionMethod::WindowsFilePicker));
    *method = storedMethod == static_cast<int>(SoundFileSelectionMethod::ManualPath)
                  ? SoundFileSelectionMethod::ManualPath
                  : SoundFileSelectionMethod::WindowsFilePicker;
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

        settings.configureSoundSource =
            Wh_GetIntSetting(L"ConfigureSoundSource") != 0;
        if (!LoadStoredSoundFileSelectionMethod(
                &settings.soundFileSelectionMethod)) {
            return false;
        }
        if (settings.soundFileSelectionMethod ==
            SoundFileSelectionMethod::ManualPath) {
            if (!LoadStoredString(kManualSoundPathStorageName,
                                  &settings.soundFilePath)) {
                return false;
            }
        } else if (!LoadStoredString(kSelectedSoundPathStorageName,
                                     &settings.soundFilePath)) {
            return false;
        }
        if (!LoadStoredString(kAudioOutputDeviceIdStorageName,
                              &settings.audioOutputDeviceId) ||
            !LoadStoredString(kAudioOutputDeviceNameStorageName,
                              &settings.audioOutputDeviceName)) {
            return false;
        }

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
    return first.soundFileSelectionMethod != second.soundFileSelectionMethod ||
           first.soundFilePath != second.soundFilePath ||
           first.audioOutputDeviceId != second.audioOutputDeviceId ||
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
    if (g_activeWaveOut) {
        MMRESULT result = waveOutReset(g_activeWaveOut);
        if (result != MMSYSERR_NOERROR) {
            Wh_Log(L"waveOutReset failed while stopping playback: %u.", result);
        }
    }
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

UINT_PTR CALLBACK SoundFilePickerHookProcedure(HWND hookWindow,
                                               UINT message,
                                               WPARAM,
                                               LPARAM) {
    if (message == WM_INITDIALOG) {
        HWND dialogWindow = GetParent(hookWindow);
        if (!dialogWindow) {
            dialogWindow = hookWindow;
        }
        g_filePickerWindow.store(dialogWindow);

        if (g_shutdownEvent &&
            WaitForSingleObject(g_shutdownEvent, 0) == WAIT_OBJECT_0) {
            PostMessageW(dialogWindow, WM_CLOSE, 0, 0);
        }
    }

    return 0;
}

void CloseSoundFilePicker() {
    HWND dialogWindow = g_filePickerWindow.load();
    if (dialogWindow && IsWindow(dialogWindow) &&
        !PostMessageW(dialogWindow, WM_CLOSE, 0, 0)) {
        LogWin32Failure(L"PostMessageW for the sound file picker",
                        GetLastError());
    }
}

bool ValidateLiteralWavPath(const std::wstring& path, PCWSTR* errorMessage) {
    *errorMessage = nullptr;
    if (path.empty()) {
        return true;
    }

    if (!HasWavExtension(path)) {
        *errorMessage = L"The selected file must have a .wav extension.";
        return false;
    }

    DWORD attributes = GetFileAttributesW(path.c_str());
    if (attributes == INVALID_FILE_ATTRIBUTES ||
        (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
        *errorMessage =
            L"The WAV file does not exist, is inaccessible, or is a directory.";
        return false;
    }

    return true;
}

bool BrowseForWavFile(HWND owner,
                      const std::wstring& currentPath,
                      std::wstring* selectedPath) {
    std::vector<wchar_t> selectedPathBuffer;
    try {
        selectedPathBuffer.resize(32768);
        if (!currentPath.empty()) {
            wcsncpy_s(selectedPathBuffer.data(), selectedPathBuffer.size(),
                      currentPath.c_str(), _TRUNCATE);
        }
    } catch (...) {
        Wh_Log(L"Unable to allocate the sound file picker path buffer.");
        return false;
    }

    constexpr wchar_t filter[] =
        L"WAV audio files (*.wav)\0*.wav\0All files (*.*)\0*.*\0\0";
    OPENFILENAMEW openFileName{};
    openFileName.lStructSize = sizeof(openFileName);
    openFileName.hwndOwner = owner;
    openFileName.lpstrFilter = filter;
    openFileName.nFilterIndex = 1;
    openFileName.lpstrFile = selectedPathBuffer.data();
    openFileName.nMaxFile = static_cast<DWORD>(selectedPathBuffer.size());
    openFileName.lpstrTitle = L"Select a WAV file for Low Battery Alarm";
    openFileName.lpstrDefExt = L"wav";
    openFileName.Flags = OFN_EXPLORER | OFN_ENABLEHOOK | OFN_FILEMUSTEXIST |
                         OFN_PATHMUSTEXIST | OFN_HIDEREADONLY |
                         OFN_DONTADDTORECENT;
    openFileName.lpfnHook = SoundFilePickerHookProcedure;

    BOOL selected = GetOpenFileNameW(&openFileName);
    g_filePickerWindow.store(nullptr);
    if (!selected) {
        DWORD dialogError = CommDlgExtendedError();
        if (dialogError != 0) {
            Wh_Log(L"GetOpenFileNameW failed. CommDlgExtendedError=0x%08lX.",
                   static_cast<unsigned long>(dialogError));
        }
        return false;
    }

    try {
        std::wstring path(selectedPathBuffer.data());
        PCWSTR errorMessage = nullptr;
        if (!ValidateLiteralWavPath(path, &errorMessage)) {
            MessageBoxW(owner, errorMessage, L"Low Battery Alarm",
                        MB_OK | MB_ICONWARNING);
            return false;
        }
        *selectedPath = std::move(path);
        return true;
    } catch (...) {
        Wh_Log(L"Unable to store the path returned by GetOpenFileNameW.");
        return false;
    }
}

struct AudioOutputDevice {
    std::wstring id;
    std::wstring storedName;
    std::wstring displayName;
};

bool QueryWaveOutDeviceInterface(UINT deviceId, std::wstring* interfaceId) {
    if (!interfaceId) {
        return false;
    }

    DWORD requiredBytes = 0;
    HWAVEOUT deviceHandle =
        reinterpret_cast<HWAVEOUT>(static_cast<UINT_PTR>(deviceId));
    MMRESULT result = waveOutMessage(
        deviceHandle, DRV_QUERYDEVICEINTERFACESIZE,
        reinterpret_cast<DWORD_PTR>(&requiredBytes), 0);
    if (result != MMSYSERR_NOERROR || requiredBytes < sizeof(wchar_t) ||
        requiredBytes % sizeof(wchar_t) != 0) {
        return false;
    }

    try {
        std::vector<wchar_t> buffer(requiredBytes / sizeof(wchar_t), L'\0');
        result = waveOutMessage(deviceHandle, DRV_QUERYDEVICEINTERFACE,
                                reinterpret_cast<DWORD_PTR>(buffer.data()),
                                requiredBytes);
        if (result != MMSYSERR_NOERROR || buffer.front() == L'\0') {
            return false;
        }
        buffer.back() = L'\0';
        interfaceId->assign(buffer.data());
        return true;
    } catch (...) {
        Wh_Log(L"Unable to allocate a wave-output device identifier.");
        return false;
    }
}

bool GetAudioEndpointFriendlyName(IMMDevice* device, std::wstring* name) {
    if (!device || !name) {
        return false;
    }

    ComPtr<IPropertyStore> propertyStore;
    HRESULT result = device->OpenPropertyStore(STGM_READ, propertyStore.Put());
    if (FAILED(result)) {
        return false;
    }

    PROPVARIANT friendlyName{};
    PropVariantInit(&friendlyName);
    result = propertyStore->GetValue(PKEY_Device_FriendlyName, &friendlyName);
    bool succeeded = false;
    try {
        if (SUCCEEDED(result) && friendlyName.vt == VT_LPWSTR &&
            friendlyName.pwszVal && friendlyName.pwszVal[0] != L'\0') {
            name->assign(friendlyName.pwszVal);
            succeeded = true;
        }
    } catch (...) {
        succeeded = false;
    }
    PropVariantClear(&friendlyName);
    return succeeded;
}

bool EnumerateAudioOutputDevices(std::vector<AudioOutputDevice>* devices) {
    if (!devices) {
        return false;
    }

    try {
        devices->clear();
        devices->push_back({L"", L"Windows default output",
                            L"Windows default output"});

        ComPtr<IMMDeviceEnumerator> enumerator;
        HRESULT comResult = CoCreateInstance(
            __uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER,
            __uuidof(IMMDeviceEnumerator),
            reinterpret_cast<void**>(enumerator.Put()));
        ComPtr<IMMDeviceCollection> collection;
        if (SUCCEEDED(comResult)) {
            comResult = enumerator->EnumAudioEndpoints(
                eRender, DEVICE_STATE_ACTIVE, collection.Put());
        }
        UINT endpointCount = 0;
        if (SUCCEEDED(comResult)) {
            comResult = collection->GetCount(&endpointCount);
        }
        if (SUCCEEDED(comResult)) {
            for (UINT index = 0; index < endpointCount; ++index) {
                ComPtr<IMMDevice> endpoint;
                if (FAILED(collection->Item(index, endpoint.Put()))) {
                    continue;
                }

                LPWSTR endpointId = nullptr;
                HRESULT idResult = endpoint->GetId(&endpointId);
                if (FAILED(idResult) || !endpointId) {
                    CoTaskMemFree(endpointId);
                    continue;
                }
                std::wstring name;
                bool haveName = GetAudioEndpointFriendlyName(endpoint.Get(),
                                                              &name);
                std::wstring endpointIdString;
                if (haveName) {
                    try {
                        endpointIdString.assign(endpointId);
                    } catch (...) {
                        CoTaskMemFree(endpointId);
                        throw;
                    }
                }
                CoTaskMemFree(endpointId);
                if (haveName) {
                    devices->push_back(
                        {std::move(endpointIdString), name, name});
                }
            }
            return true;
        }

        Wh_Log(L"Core Audio endpoint enumeration failed; using legacy "
               L"wave-output names. HRESULT=0x%08lX.",
               static_cast<unsigned long>(comResult));
        UINT deviceCount = waveOutGetNumDevs();
        for (UINT deviceId = 0; deviceId < deviceCount; ++deviceId) {
            WAVEOUTCAPSW capabilities{};
            MMRESULT result = waveOutGetDevCapsW(
                deviceId, &capabilities, sizeof(capabilities));
            if (result != MMSYSERR_NOERROR) {
                Wh_Log(L"waveOutGetDevCapsW failed for device %u: %u.",
                       deviceId, result);
                continue;
            }

            std::wstring name = capabilities.szPname;
            if (name.empty()) {
                name = L"Audio output " + std::to_wstring(deviceId + 1);
            }

            std::wstring interfaceId;
            if (!QueryWaveOutDeviceInterface(deviceId, &interfaceId)) {
                interfaceId = L"legacy-waveout:" + name;
            }

            bool duplicate = false;
            for (const auto& existingDevice : *devices) {
                if (_wcsicmp(existingDevice.id.c_str(),
                             interfaceId.c_str()) == 0) {
                    duplicate = true;
                    break;
                }
            }
            if (!duplicate) {
                devices->push_back(
                    {std::move(interfaceId), name, std::move(name)});
            }
        }
        return true;
    } catch (...) {
        Wh_Log(L"Unable to allocate the audio-output device list.");
        return false;
    }
}

bool ResolveWaveOutDeviceId(const std::wstring& selectedDeviceId,
                            const std::wstring& selectedDeviceName,
                            UINT* deviceId) {
    if (!deviceId) {
        return false;
    }
    if (selectedDeviceId.empty()) {
        *deviceId = WAVE_MAPPER;
        return true;
    }

    constexpr wchar_t legacyPrefix[] = L"legacy-waveout:";
    bool legacyId = selectedDeviceId.starts_with(legacyPrefix);
    PCWSTR legacyName = legacyId
                            ? selectedDeviceId.c_str() +
                                  (ARRAYSIZE(legacyPrefix) - 1)
                            : nullptr;

    UINT deviceCount = waveOutGetNumDevs();
    UINT uniquePrefixMatch = WAVE_MAPPER;
    bool havePrefixMatch = false;
    bool ambiguousPrefixMatch = false;
    for (UINT candidateId = 0; candidateId < deviceCount; ++candidateId) {
        WAVEOUTCAPSW capabilities{};
        bool haveCapabilities =
            waveOutGetDevCapsW(candidateId, &capabilities,
                               sizeof(capabilities)) == MMSYSERR_NOERROR;
        if (legacyId) {
            if (haveCapabilities &&
                _wcsicmp(capabilities.szPname, legacyName) == 0) {
                *deviceId = candidateId;
                return true;
            }
            continue;
        }

        std::wstring candidateInterfaceId;
        if (QueryWaveOutDeviceInterface(candidateId,
                                        &candidateInterfaceId) &&
            (_wcsicmp(candidateInterfaceId.c_str(),
                      selectedDeviceId.c_str()) == 0 ||
             candidateInterfaceId.find(selectedDeviceId) !=
                 std::wstring::npos)) {
            *deviceId = candidateId;
            return true;
        }

        if (haveCapabilities && !selectedDeviceName.empty()) {
            size_t waveNameLength = wcslen(capabilities.szPname);
            if (_wcsicmp(selectedDeviceName.c_str(),
                         capabilities.szPname) == 0) {
                *deviceId = candidateId;
                return true;
            }
            if (waveNameLength > 0 &&
                selectedDeviceName.size() >= waveNameLength &&
                _wcsnicmp(selectedDeviceName.c_str(), capabilities.szPname,
                           waveNameLength) == 0) {
                if (havePrefixMatch) {
                    ambiguousPrefixMatch = true;
                } else {
                    uniquePrefixMatch = candidateId;
                    havePrefixMatch = true;
                }
            }
        }
    }

    if (havePrefixMatch && !ambiguousPrefixMatch) {
        *deviceId = uniquePrefixMatch;
        return true;
    }

    *deviceId = WAVE_MAPPER;
    return false;
}

std::wstring ExtractEndpointIdFromWaveOutInterface(
    const std::wstring& interfaceId) {
    size_t firstSeparator = interfaceId.find(L'#');
    size_t secondSeparator =
        firstSeparator == std::wstring::npos
            ? std::wstring::npos
            : interfaceId.find(L'#', firstSeparator + 1);
    size_t thirdSeparator =
        secondSeparator == std::wstring::npos
            ? std::wstring::npos
            : interfaceId.find(L'#', secondSeparator + 1);
    if (secondSeparator == std::wstring::npos ||
        thirdSeparator == std::wstring::npos ||
        thirdSeparator <= secondSeparator + 1) {
        return {};
    }
    return interfaceId.substr(secondSeparator + 1,
                              thirdSeparator - secondSeparator - 1);
}

enum SoundConfigurationControlId {
    kPickerRadioId = 1001,
    kPickerPathId,
    kBrowseButtonId,
    kManualRadioId,
    kManualPathId,
    kOutputDeviceComboId,
    kAcceptButtonId,
    kCancelButtonId
};

struct SoundConfigurationWindowState {
    SoundFileSelectionMethod method =
        SoundFileSelectionMethod::WindowsFilePicker;
    std::wstring pickerPath;
    std::wstring manualPath;
    std::vector<AudioOutputDevice> outputDevices;
    size_t selectedOutputDeviceIndex = 0;
    HWND pickerRadio = nullptr;
    HWND pickerDescription = nullptr;
    HWND pickerPathEdit = nullptr;
    HWND browseButton = nullptr;
    HWND manualRadio = nullptr;
    HWND manualDescription = nullptr;
    HWND manualPathEdit = nullptr;
    HWND outputDeviceCombo = nullptr;
};

bool ReadWindowText(HWND window, std::wstring* text) {
    if (!window || !text) {
        return false;
    }

    int length = GetWindowTextLengthW(window);
    if (length < 0 || length >= 32768) {
        return false;
    }

    try {
        std::vector<wchar_t> buffer(static_cast<size_t>(length) + 1);
        int copied = GetWindowTextW(window, buffer.data(),
                                    static_cast<int>(buffer.size()));
        if (copied < 0) {
            return false;
        }
        text->assign(buffer.data(), static_cast<size_t>(copied));
        return true;
    } catch (...) {
        Wh_Log(L"Unable to allocate memory while reading a sound path field.");
        return false;
    }
}

void SetSoundConfigurationFont(HWND window, HFONT font) {
    if (window) {
        SendMessageW(window, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE);
    }
}

HWND CreateSoundConfigurationControl(DWORD extendedStyle,
                                     PCWSTR className,
                                     PCWSTR text,
                                     DWORD style,
                                     int x,
                                     int y,
                                     int width,
                                     int height,
                                     HWND parent,
                                     int controlId,
                                     HFONT font) {
    HWND control = CreateWindowExW(
        extendedStyle, className, text, WS_CHILD | WS_VISIBLE | style, x, y,
        width, height, parent, reinterpret_cast<HMENU>(
                                   static_cast<INT_PTR>(controlId)),
        GetModuleHandleW(nullptr), nullptr);
    SetSoundConfigurationFont(control, font);
    return control;
}

void UpdateSoundConfigurationControls(SoundConfigurationWindowState* state) {
    if (!state) {
        return;
    }

    bool pickerSelected =
        state->method == SoundFileSelectionMethod::WindowsFilePicker;
    SendMessageW(state->pickerRadio, BM_SETCHECK,
                 pickerSelected ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessageW(state->manualRadio, BM_SETCHECK,
                 pickerSelected ? BST_UNCHECKED : BST_CHECKED, 0);

    EnableWindow(state->pickerPathEdit, pickerSelected);
    EnableWindow(state->browseButton, pickerSelected);
    EnableWindow(state->pickerDescription, pickerSelected);
    EnableWindow(state->manualDescription, !pickerSelected);
    EnableWindow(state->manualPathEdit, !pickerSelected);
}

bool ApplySoundConfiguration(SoundConfigurationWindowState* state,
                             HWND owner) {
    if (!state || !ReadWindowText(state->manualPathEdit, &state->manualPath)) {
        MessageBoxW(owner, L"The manual path could not be read.",
                    L"Low Battery Alarm", MB_OK | MB_ICONERROR);
        return false;
    }

    try {
        state->manualPath = TrimAndUnquote(state->manualPath);
    } catch (...) {
        MessageBoxW(owner, L"The manual path could not be processed.",
                    L"Low Battery Alarm", MB_OK | MB_ICONERROR);
        return false;
    }

    LRESULT selectedDeviceIndex =
        SendMessageW(state->outputDeviceCombo, CB_GETCURSEL, 0, 0);
    if (selectedDeviceIndex == CB_ERR || selectedDeviceIndex < 0 ||
        static_cast<size_t>(selectedDeviceIndex) >=
            state->outputDevices.size()) {
        MessageBoxW(owner, L"Select a valid alarm output device.",
                    L"Low Battery Alarm", MB_OK | MB_ICONERROR);
        return false;
    }
    const AudioOutputDevice& selectedDevice =
        state->outputDevices[static_cast<size_t>(selectedDeviceIndex)];

    if (!Wh_SetStringValue(kSelectedSoundPathStorageName,
                           state->pickerPath.c_str()) ||
        !Wh_SetStringValue(kManualSoundPathStorageName,
                           state->manualPath.c_str()) ||
        !Wh_SetStringValue(kAudioOutputDeviceIdStorageName,
                           selectedDevice.id.c_str()) ||
        !Wh_SetStringValue(kAudioOutputDeviceNameStorageName,
                           selectedDevice.storedName.c_str()) ||
        !Wh_SetIntValue(kSoundSelectionMethodStorageName,
                        static_cast<int>(state->method))) {
        MessageBoxW(owner, L"The sound-source settings could not be saved.",
                    L"Low Battery Alarm", MB_OK | MB_ICONERROR);
        Wh_Log(L"Unable to save the sound-source configuration.");
        return false;
    }

    Settings updatedSettings;
    if (!GetSettingsSnapshot(&updatedSettings)) {
        MessageBoxW(owner,
                    L"The sound-source settings were saved but could not be "
                    L"applied immediately.",
                    L"Low Battery Alarm", MB_OK | MB_ICONWARNING);
        return true;
    }

    try {
        updatedSettings.soundFileSelectionMethod = state->method;
        updatedSettings.soundFilePath =
            state->method == SoundFileSelectionMethod::WindowsFilePicker
                ? state->pickerPath
                : state->manualPath;
        updatedSettings.audioOutputDeviceId = selectedDevice.id;
        updatedSettings.audioOutputDeviceName = selectedDevice.storedName;
        if (!ReplaceSettings(updatedSettings)) {
            MessageBoxW(owner,
                        L"The sound-source settings were saved but could not be "
                        L"applied immediately.",
                        L"Low Battery Alarm", MB_OK | MB_ICONWARNING);
            return true;
        }
    } catch (...) {
        MessageBoxW(owner,
                    L"The sound-source settings were saved but could not be "
                    L"applied immediately.",
                    L"Low Battery Alarm", MB_OK | MB_ICONWARNING);
        return true;
    }
    CancelAllPlayback();
    if (g_settingsChangedEvent && !SetEvent(g_settingsChangedEvent)) {
        LogWin32Failure(L"SetEvent after configuring the sound source",
                        GetLastError());
    }

    Wh_Log(L"Sound source configured in %s mode. Effective path: %s. "
           L"Output device: %s",
           state->method == SoundFileSelectionMethod::WindowsFilePicker
               ? L"Windows file picker"
               : L"manual full path",
           updatedSettings.soundFilePath.c_str(),
           updatedSettings.audioOutputDeviceName.c_str());
    if (updatedSettings.testSoundAfterSettingsChange && !g_stopping.load()) {
        QueuePlayback(PlaybackKind::Test, updatedSettings, false);
    }
    return true;
}

LRESULT CALLBACK SoundConfigurationWindowProcedure(HWND window,
                                                   UINT message,
                                                   WPARAM wParam,
                                                   LPARAM lParam) {
    auto* state = reinterpret_cast<SoundConfigurationWindowState*>(
        GetWindowLongPtrW(window, GWLP_USERDATA));

    if (message == WM_NCCREATE) {
        auto* create = reinterpret_cast<CREATESTRUCTW*>(lParam);
        state = static_cast<SoundConfigurationWindowState*>(
            create->lpCreateParams);
        SetWindowLongPtrW(window, GWLP_USERDATA,
                          reinterpret_cast<LONG_PTR>(state));
    }

    switch (message) {
        case WM_CREATE: {
            HFONT font = static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
            state->pickerRadio = CreateSoundConfigurationControl(
                0, L"BUTTON", L"Windows file picker",
                WS_TABSTOP | WS_GROUP | BS_AUTORADIOBUTTON, 18, 16, 250, 24,
                window, kPickerRadioId, font);
            state->pickerDescription = CreateSoundConfigurationControl(
                0, L"STATIC",
                L"Select a WAV file through the standard Windows dialog.", 0,
                42, 43, 610, 20, window, 0, font);
            state->pickerPathEdit = CreateSoundConfigurationControl(
                WS_EX_CLIENTEDGE, L"EDIT", state->pickerPath.c_str(),
                WS_TABSTOP | ES_AUTOHSCROLL | ES_READONLY, 42, 66, 500, 25,
                window, kPickerPathId, font);
            state->browseButton = CreateSoundConfigurationControl(
                0, L"BUTTON", L"Browse...", WS_TABSTOP | BS_PUSHBUTTON, 552,
                65, 100, 27, window, kBrowseButtonId, font);
            state->manualRadio = CreateSoundConfigurationControl(
                0, L"BUTTON", L"Manual full path",
                WS_TABSTOP | BS_AUTORADIOBUTTON, 18, 110, 250, 24, window,
                kManualRadioId, font);
            state->manualDescription = CreateSoundConfigurationControl(
                0, L"STATIC",
                L"Type or paste the complete WAV file path.", 0, 42, 137,
                610, 20, window, 0, font);
            state->manualPathEdit = CreateSoundConfigurationControl(
                WS_EX_CLIENTEDGE, L"EDIT", state->manualPath.c_str(),
                WS_TABSTOP | ES_AUTOHSCROLL, 42, 160, 610, 25, window,
                kManualPathId, font);
            HWND outputDeviceLabel = CreateSoundConfigurationControl(
                0, L"STATIC", L"Alarm output device", 0, 18, 201, 634, 20,
                window, 0, font);
            state->outputDeviceCombo = CreateSoundConfigurationControl(
                0, L"COMBOBOX", L"",
                WS_TABSTOP | WS_VSCROLL | CBS_DROPDOWNLIST, 18, 224, 634, 220,
                window, kOutputDeviceComboId, font);
            HWND note = CreateSoundConfigurationControl(
                0, L"STATIC",
                L"Only the selected method is used. The other method is "
                L"disabled and its path is kept independently.",
                0, 18, 266, 634, 20, window, 0, font);
            HWND acceptButton = CreateSoundConfigurationControl(
                0, L"BUTTON", L"OK", WS_TABSTOP | BS_DEFPUSHBUTTON, 442, 307,
                100, 29, window, kAcceptButtonId, font);
            HWND cancelButton = CreateSoundConfigurationControl(
                0, L"BUTTON", L"Cancel", WS_TABSTOP | BS_PUSHBUTTON, 552, 307,
                100, 29, window, kCancelButtonId, font);

            if (!state->pickerRadio || !state->pickerDescription ||
                !state->pickerPathEdit || !state->browseButton ||
                !state->manualRadio || !state->manualDescription ||
                !state->manualPathEdit || !outputDeviceLabel ||
                !state->outputDeviceCombo || !note || !acceptButton ||
                !cancelButton || state->outputDevices.empty()) {
                return -1;
            }

            for (const auto& device : state->outputDevices) {
                if (SendMessageW(state->outputDeviceCombo, CB_ADDSTRING, 0,
                                 reinterpret_cast<LPARAM>(
                                     device.displayName.c_str())) < 0) {
                    return -1;
                }
            }
            SendMessageW(state->outputDeviceCombo, CB_SETCURSEL,
                         static_cast<WPARAM>(state->selectedOutputDeviceIndex),
                         0);

            SendMessageW(state->pickerPathEdit, EM_SETLIMITTEXT, 32767, 0);
            SendMessageW(state->manualPathEdit, EM_SETLIMITTEXT, 32767, 0);
            UpdateSoundConfigurationControls(state);
            return 0;
        }

        case WM_COMMAND: {
            if (!state) {
                return 0;
            }

            switch (LOWORD(wParam)) {
                case kPickerRadioId:
                    if (HIWORD(wParam) == BN_CLICKED) {
                        state->method =
                            SoundFileSelectionMethod::WindowsFilePicker;
                        UpdateSoundConfigurationControls(state);
                    }
                    return 0;

                case kManualRadioId:
                    if (HIWORD(wParam) == BN_CLICKED) {
                        state->method = SoundFileSelectionMethod::ManualPath;
                        UpdateSoundConfigurationControls(state);
                        SetFocus(state->manualPathEdit);
                    }
                    return 0;

                case kBrowseButtonId:
                    if (HIWORD(wParam) == BN_CLICKED &&
                        state->method ==
                            SoundFileSelectionMethod::WindowsFilePicker) {
                        std::wstring selectedPath;
                        if (BrowseForWavFile(window, state->pickerPath,
                                             &selectedPath)) {
                            state->pickerPath = std::move(selectedPath);
                            SetWindowTextW(state->pickerPathEdit,
                                           state->pickerPath.c_str());
                        }
                    }
                    return 0;

                case kAcceptButtonId:
                    if (HIWORD(wParam) == BN_CLICKED &&
                        ApplySoundConfiguration(state, window)) {
                        DestroyWindow(window);
                    }
                    return 0;

                case kCancelButtonId:
                    if (HIWORD(wParam) == BN_CLICKED) {
                        DestroyWindow(window);
                    }
                    return 0;
            }
            break;
        }

        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLORBTN: {
            HDC deviceContext = reinterpret_cast<HDC>(wParam);
            HWND control = reinterpret_cast<HWND>(lParam);
            SetBkMode(deviceContext, TRANSPARENT);
            SetTextColor(deviceContext,
                         GetSysColor(IsWindowEnabled(control) ? COLOR_BTNTEXT
                                                              : COLOR_GRAYTEXT));
            return reinterpret_cast<LRESULT>(GetSysColorBrush(COLOR_BTNFACE));
        }

        case WM_CLOSE:
            DestroyWindow(window);
            return 0;

        case WM_DESTROY:
            g_soundConfigurationWindow.store(nullptr);
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcW(window, message, wParam, lParam);
}

void RunSoundConfiguration() {
    if (g_stopping.load()) {
        Wh_Log(L"Sound-source configuration was cancelled during shutdown.");
        return;
    }

    Settings settings;
    if (!GetSettingsSnapshot(&settings)) {
        Wh_Log(L"The sound-source window could not obtain settings.");
        return;
    }

    SoundConfigurationWindowState state;
    state.method = settings.soundFileSelectionMethod;
    if (!LoadStoredString(kSelectedSoundPathStorageName, &state.pickerPath) ||
        !LoadStoredString(kManualSoundPathStorageName, &state.manualPath)) {
        Wh_Log(L"The sound-source window could not load its saved paths.");
        return;
    }
    if (!EnumerateAudioOutputDevices(&state.outputDevices)) {
        Wh_Log(L"The sound-source window could not enumerate audio outputs.");
        return;
    }

    bool selectedDeviceFound = settings.audioOutputDeviceId.empty();
    for (size_t index = 0; index < state.outputDevices.size(); ++index) {
        if (_wcsicmp(state.outputDevices[index].id.c_str(),
                     settings.audioOutputDeviceId.c_str()) == 0) {
            state.selectedOutputDeviceIndex = index;
            selectedDeviceFound = true;
            break;
        }
    }
    if (!selectedDeviceFound) {
        try {
            std::wstring storedName = settings.audioOutputDeviceName.empty()
                                          ? L"Previously selected output"
                                          : settings.audioOutputDeviceName;
            std::wstring displayName =
                storedName + L" (currently unavailable; Windows default will "
                             L"be used)";
            state.outputDevices.push_back({settings.audioOutputDeviceId,
                                           std::move(storedName),
                                           std::move(displayName)});
            state.selectedOutputDeviceIndex = state.outputDevices.size() - 1;
        } catch (...) {
            Wh_Log(L"Unable to represent the unavailable saved audio output.");
            return;
        }
    }

    HINSTANCE instance = nullptr;
    if (!GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<PCWSTR>(&g_soundConfigurationWindow),
            &instance)) {
        LogWin32Failure(L"GetModuleHandleExW for sound-source configuration",
                        GetLastError());
        return;
    }

    WNDCLASSEXW windowClass{};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.lpfnWndProc = SoundConfigurationWindowProcedure;
    windowClass.hInstance = instance;
    windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    windowClass.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
    windowClass.lpszClassName = kSoundConfigurationWindowClass;
    bool registeredWindowClass = RegisterClassExW(&windowClass) != 0;
    if (!registeredWindowClass) {
        DWORD error = GetLastError();
        WNDCLASSEXW existingClass{};
        existingClass.cbSize = sizeof(existingClass);
        if (error != ERROR_CLASS_ALREADY_EXISTS ||
            !GetClassInfoExW(instance, kSoundConfigurationWindowClass,
                             &existingClass) ||
            existingClass.lpfnWndProc !=
                SoundConfigurationWindowProcedure) {
            LogWin32Failure(L"RegisterClassExW for sound-source configuration",
                            error);
            return;
        }
    }

    RECT workArea{};
    if (!SystemParametersInfoW(SPI_GETWORKAREA, 0, &workArea, 0)) {
        workArea = {0, 0, GetSystemMetrics(SM_CXSCREEN),
                    GetSystemMetrics(SM_CYSCREEN)};
    }
    constexpr int windowWidth = 690;
    constexpr int windowHeight = 390;
    int x = workArea.left +
            ((workArea.right - workArea.left) - windowWidth) / 2;
    int y = workArea.top +
            ((workArea.bottom - workArea.top) - windowHeight) / 2;

    HWND window = CreateWindowExW(
        WS_EX_DLGMODALFRAME | WS_EX_TOPMOST,
        kSoundConfigurationWindowClass,
        L"Low Battery Alarm - Sound Source",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, x, y, windowWidth,
        windowHeight, nullptr, nullptr, instance, &state);
    if (!window) {
        LogWin32Failure(L"CreateWindowExW for sound-source configuration",
                        GetLastError());
        if (registeredWindowClass) {
            UnregisterClassW(kSoundConfigurationWindowClass, instance);
        }
        return;
    }

    g_soundConfigurationWindow.store(window);
    if (!SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0,
                      SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE)) {
        LogWin32Failure(L"SetWindowPos for topmost sound-source configuration",
                        GetLastError());
    }
    ShowWindow(window, SW_SHOWNORMAL);
    SetForegroundWindow(window);
    UpdateWindow(window);

    MSG message{};
    while (!g_stopping.load()) {
        BOOL result = GetMessageW(&message, nullptr, 0, 0);
        if (result <= 0) {
            if (result < 0) {
                LogWin32Failure(L"GetMessageW for sound-source configuration",
                                GetLastError());
            }
            break;
        }
        if (!IsDialogMessageW(window, &message)) {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    }

    if (IsWindow(window)) {
        DestroyWindow(window);
    }
    g_soundConfigurationWindow.store(nullptr);
    if (registeredWindowClass &&
        !UnregisterClassW(kSoundConfigurationWindowClass, instance)) {
        LogWin32Failure(L"UnregisterClassW for sound-source configuration",
                        GetLastError());
    }
}

void CloseSoundConfiguration() {
    CloseSoundFilePicker();
    HWND window = g_soundConfigurationWindow.load();
    if (window && IsWindow(window) &&
        !PostMessageW(window, WM_CLOSE, 0, 0)) {
        LogWin32Failure(L"PostMessageW for sound-source configuration",
                        GetLastError());
    }
}

DWORD WINAPI SoundConfigurationThreadProcedure(void*) {
    Wh_Log(L"Sound-source configuration thread started.");
    HRESULT comResult = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    bool mustUninitializeCom = SUCCEEDED(comResult);
    if (FAILED(comResult)) {
        LogHResultFailure(L"CoInitializeEx on the sound configuration thread",
                          comResult);
    }
    try {
        RunSoundConfiguration();
    } catch (...) {
        Wh_Log(L"An exception occurred in the sound-source configuration "
               L"thread.");
    }

    g_filePickerWindow.store(nullptr);
    g_soundConfigurationWindow.store(nullptr);
    if (mustUninitializeCom) {
        CoUninitialize();
    }
    Wh_Log(L"Sound-source configuration thread stopped.");
    return 0;
}

bool RequestSoundConfiguration() {
    AcquireSRWLockExclusive(&g_soundConfigurationLock);

    if (g_soundConfigurationThread) {
        DWORD waitResult = WaitForSingleObject(g_soundConfigurationThread, 0);
        if (waitResult == WAIT_OBJECT_0) {
            CloseHandle(g_soundConfigurationThread);
            g_soundConfigurationThread = nullptr;
        } else if (waitResult == WAIT_TIMEOUT) {
            ReleaseSRWLockExclusive(&g_soundConfigurationLock);
            Wh_Log(L"The sound-source configuration window is already open.");
            HWND window = g_soundConfigurationWindow.load();
            if (window) {
                SetForegroundWindow(window);
            }
            return true;
        } else {
            DWORD error = GetLastError();
            ReleaseSRWLockExclusive(&g_soundConfigurationLock);
            LogWin32Failure(L"Sound-source configuration thread status check",
                            error);
            return false;
        }
    }

    if (g_stopping.load()) {
        ReleaseSRWLockExclusive(&g_soundConfigurationLock);
        return false;
    }

    g_soundConfigurationThread = CreateThread(
        nullptr, 0, SoundConfigurationThreadProcedure, nullptr, 0, nullptr);
    if (!g_soundConfigurationThread) {
        DWORD error = GetLastError();
        ReleaseSRWLockExclusive(&g_soundConfigurationLock);
        LogWin32Failure(L"CreateThread for sound-source configuration", error);
        return false;
    }

    ReleaseSRWLockExclusive(&g_soundConfigurationLock);
    return true;
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

void LogWaveOutFailure(PCWSTR action, MMRESULT result) {
    wchar_t message[MAXERRORLENGTH]{};
    if (waveOutGetErrorTextW(result, message, ARRAYSIZE(message)) ==
        MMSYSERR_NOERROR) {
        Wh_Log(L"%s failed: %s (%u).", action, message, result);
    } else {
        Wh_Log(L"%s failed with multimedia error %u.", action, result);
    }
}

HRESULT FindAudioEndpointByFriendlyNamePrefix(
    IMMDeviceEnumerator* enumerator,
    PCWSTR namePrefix,
    IMMDevice** matchedDevice) {
    if (!enumerator || !namePrefix || !matchedDevice || namePrefix[0] == L'\0') {
        return E_INVALIDARG;
    }
    *matchedDevice = nullptr;

    ComPtr<IMMDeviceCollection> collection;
    HRESULT result = enumerator->EnumAudioEndpoints(
        eRender, DEVICE_STATE_ACTIVE, collection.Put());
    if (FAILED(result)) {
        return result;
    }

    UINT count = 0;
    result = collection->GetCount(&count);
    if (FAILED(result)) {
        return result;
    }

    size_t prefixLength = wcslen(namePrefix);
    IMMDevice* prefixMatch = nullptr;
    bool ambiguousPrefixMatch = false;
    for (UINT index = 0; index < count; ++index) {
        ComPtr<IMMDevice> candidate;
        if (FAILED(collection->Item(index, candidate.Put()))) {
            continue;
        }
        std::wstring friendlyName;
        if (!GetAudioEndpointFriendlyName(candidate.Get(), &friendlyName)) {
            continue;
        }

        if (_wcsicmp(friendlyName.c_str(), namePrefix) == 0) {
            if (prefixMatch) {
                prefixMatch->Release();
            }
            candidate.Get()->AddRef();
            *matchedDevice = candidate.Get();
            return S_OK;
        }
        if (friendlyName.size() < prefixLength ||
            _wcsnicmp(friendlyName.c_str(), namePrefix, prefixLength) != 0) {
            continue;
        }

        if (prefixMatch) {
            prefixMatch->Release();
            prefixMatch = nullptr;
            ambiguousPrefixMatch = true;
        } else if (!ambiguousPrefixMatch) {
            candidate.Get()->AddRef();
            prefixMatch = candidate.Get();
        }
    }

    if (prefixMatch && !ambiguousPrefixMatch) {
        *matchedDevice = prefixMatch;
        return S_OK;
    }
    return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
}

bool PlayWaveFileInterruptibly(const std::wstring& path,
                               const Settings& settings) {
    LoadedWaveFile wave;
    if (!LoadWaveFile(path, &wave)) {
        return false;
    }

    UINT outputDeviceId = WAVE_MAPPER;
    if (!ResolveWaveOutDeviceId(settings.audioOutputDeviceId,
                                settings.audioOutputDeviceName,
                                &outputDeviceId)) {
        Wh_Log(L"The selected alarm output is unavailable: %s. Windows default "
               L"output will be used for this alert.",
               settings.audioOutputDeviceName.c_str());
    }

    HANDLE completionEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!completionEvent) {
        LogWin32Failure(L"CreateEventW for wave-output completion",
                        GetLastError());
        return false;
    }

    HWAVEOUT waveOut = nullptr;
    WAVEHDR header{};
    header.lpData = reinterpret_cast<LPSTR>(wave.audioData.data());
    header.dwBufferLength = static_cast<DWORD>(wave.audioData.size());
    bool prepared = false;
    bool submitted = false;
    MMRESULT result = MMSYSERR_NOERROR;

    AcquireSRWLockExclusive(&g_playbackControlLock);
    if (IsPlaybackCancelled()) {
        ReleaseSRWLockExclusive(&g_playbackControlLock);
        CloseHandle(completionEvent);
        return true;
    }

    result = waveOutOpen(
        &waveOut, outputDeviceId, wave.Format(),
        reinterpret_cast<DWORD_PTR>(completionEvent), 0, CALLBACK_EVENT);
    if (result == MMSYSERR_NOERROR) {
        g_activeWaveOut = waveOut;
        ResetEvent(completionEvent);
        result = waveOutPrepareHeader(waveOut, &header, sizeof(header));
        prepared = result == MMSYSERR_NOERROR;
    }
    if (prepared) {
        ResetEvent(completionEvent);
        result = waveOutWrite(waveOut, &header, sizeof(header));
        submitted = result == MMSYSERR_NOERROR;
    }
    ReleaseSRWLockExclusive(&g_playbackControlLock);

    bool waitSucceeded = submitted;
    if (submitted) {
        HANDLE waitHandles[] = {g_shutdownEvent, g_playbackCancelEvent,
                                completionEvent};
        while ((header.dwFlags & WHDR_DONE) == 0) {
            DWORD waitResult = WaitForMultipleObjects(
                ARRAYSIZE(waitHandles), waitHandles, FALSE, INFINITE);
            if (waitResult == WAIT_OBJECT_0 ||
                waitResult == WAIT_OBJECT_0 + 1) {
                break;
            }
            if (waitResult == WAIT_OBJECT_0 + 2) {
                continue;
            }
            if (waitResult == WAIT_FAILED) {
                LogWin32Failure(L"Waiting for wave-output completion",
                                GetLastError());
            } else {
                Wh_Log(L"Unexpected wave-output wait result: %lu.",
                       waitResult);
            }
            waitSucceeded = false;
            break;
        }
    }

    AcquireSRWLockExclusive(&g_playbackControlLock);
    if (waveOut) {
        if ((header.dwFlags & WHDR_DONE) == 0) {
            MMRESULT resetResult = waveOutReset(waveOut);
            if (resetResult != MMSYSERR_NOERROR) {
                LogWaveOutFailure(L"waveOutReset during cleanup", resetResult);
            }
        }
        if (prepared) {
            MMRESULT unprepareResult =
                waveOutUnprepareHeader(waveOut, &header, sizeof(header));
            if (unprepareResult == WAVERR_STILLPLAYING) {
                waveOutReset(waveOut);
                unprepareResult =
                    waveOutUnprepareHeader(waveOut, &header, sizeof(header));
            }
            if (unprepareResult != MMSYSERR_NOERROR) {
                LogWaveOutFailure(L"waveOutUnprepareHeader", unprepareResult);
            }
        }
        if (g_activeWaveOut == waveOut) {
            g_activeWaveOut = nullptr;
        }
        MMRESULT closeResult = waveOutClose(waveOut);
        if (closeResult != MMSYSERR_NOERROR) {
            LogWaveOutFailure(L"waveOutClose", closeResult);
        }
    }
    ReleaseSRWLockExclusive(&g_playbackControlLock);
    CloseHandle(completionEvent);

    if (!submitted) {
        LogWaveOutFailure(prepared ? L"waveOutWrite"
                                   : waveOut ? L"waveOutPrepareHeader"
                                             : L"waveOutOpen",
                          result);
        return false;
    }
    return waitSucceeded;
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
        if (settings.audioOutputDeviceId.empty()) {
            result = enumerator->GetDefaultAudioEndpoint(eRender, eMultimedia,
                                                         device.Put());
        } else {
            std::wstring endpointId;
            try {
                constexpr wchar_t legacyPrefix[] = L"legacy-waveout:";
                if (settings.audioOutputDeviceId.starts_with(legacyPrefix)) {
                    result = FindAudioEndpointByFriendlyNamePrefix(
                        enumerator.Get(),
                        settings.audioOutputDeviceId.c_str() +
                            (ARRAYSIZE(legacyPrefix) - 1),
                        device.Put());
                } else {
                    endpointId = settings.audioOutputDeviceId.starts_with(L"\\\\?\\")
                                     ? ExtractEndpointIdFromWaveOutInterface(
                                           settings.audioOutputDeviceId)
                                     : settings.audioOutputDeviceId;
                    result = endpointId.empty()
                                 ? E_INVALIDARG
                                 : enumerator->GetDevice(endpointId.c_str(),
                                                         device.Put());
                }
            } catch (...) {
                Wh_Log(L"Unable to allocate the selected audio endpoint ID.");
                return;
            }
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
                Wh_Log(L"The default output device was temporarily unmuted.");
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

bool CreateSessionInstanceMutex() {
    DWORD sessionId = 0;
    if (!ProcessIdToSessionId(GetCurrentProcessId(), &sessionId)) {
        LogWin32Failure(L"ProcessIdToSessionId", GetLastError());
        return false;
    }

    wchar_t mutexName[96]{};
    int characters = swprintf_s(
        mutexName, ARRAYSIZE(mutexName),
        L"Local\\WindhawkLowBatteryAlarm_%lu", sessionId);
    if (characters <= 0) {
        Wh_Log(L"Unable to format the per-session mutex name.");
        return false;
    }

    SetLastError(ERROR_SUCCESS);
    // The open handle, rather than thread-affine mutex ownership, is the
    // per-session lifetime marker.
    g_instanceMutex = CreateMutexW(nullptr, FALSE, mutexName);
    if (!g_instanceMutex) {
        LogWin32Failure(L"CreateMutexW for the per-session controller",
                        GetLastError());
        return false;
    }

    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        CloseHandle(g_instanceMutex);
        g_instanceMutex = nullptr;
        g_isPrimaryInstance = false;
        Wh_Log(L"Another Low Battery Alarm controller is active in session "
               L"%lu; this Explorer instance will remain secondary.",
               sessionId);
        return true;
    }

    g_isPrimaryInstance = true;
    Wh_Log(L"This Explorer instance is the primary Low Battery Alarm controller "
           L"for session %lu.",
           sessionId);
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
    CloseSoundConfiguration();
    StopActiveWaveform();
}

void WaitForWorkers() {
    HANDLE threads[4]{};
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
    AcquireSRWLockShared(&g_soundConfigurationLock);
    if (g_soundConfigurationThread) {
        threads[threadCount++] = g_soundConfigurationThread;
    }
    ReleaseSRWLockShared(&g_soundConfigurationLock);

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
        (g_monitorThread || g_playbackThread || g_powerNotificationThread ||
         g_soundConfigurationThread)) {
        Wh_Log(L"Cleanup was requested before worker threads were joined; "
               L"joining them now.");
        StopAndJoinWorkers();
    }

    CloseHandleIfPresent(&g_monitorThread);
    CloseHandleIfPresent(&g_playbackThread);
    CloseHandleIfPresent(&g_powerNotificationThread);
    AcquireSRWLockExclusive(&g_soundConfigurationLock);
    CloseHandleIfPresent(&g_soundConfigurationThread);
    ReleaseSRWLockExclusive(&g_soundConfigurationLock);
    CloseHandleIfPresent(&g_initialEvaluationEvent);
    CloseHandleIfPresent(&g_powerSourceChangedEvent);
    CloseHandleIfPresent(&g_powerNotificationReadyEvent);
    CloseHandleIfPresent(&g_playbackFinishedEvent);
    CloseHandleIfPresent(&g_playbackRequestEvent);
    CloseHandleIfPresent(&g_playbackCancelEvent);
    CloseHandleIfPresent(&g_settingsChangedEvent);
    CloseHandleIfPresent(&g_shutdownEvent);

    if (g_instanceMutex) {
        CloseHandleIfPresent(&g_instanceMutex);
    }
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

BOOL Wh_ModInit() {
    Wh_Log(L"Low Battery Alarm initialization started.");

    try {
        g_stopping.store(false);
        g_initialEvaluationCompleted.store(false);
        g_powerNotificationInitialized.store(false);
        g_externalPowerConnected.store(false);
        g_lastNotifiedPowerCondition.store(-1);
        g_activeWaveOut = nullptr;
        g_workersJoined = false;

        Settings initialSettings;
        if (!LoadSettings(&initialSettings)) {
            Wh_Log(L"Initial settings could not be loaded.");
            return FALSE;
        }
        if (!ReplaceSettings(std::move(initialSettings))) {
            return FALSE;
        }

        if (!CreateSynchronizationObjects()) {
            CleanupHandles();
            return FALSE;
        }

        if (!CreateSessionInstanceMutex()) {
            CleanupHandles();
            return FALSE;
        }

        if (!g_isPrimaryInstance) {
            Wh_Log(L"Low Battery Alarm initialized as a secondary instance.");
            return TRUE;
        }

        if (!StartWorkers()) {
            Wh_Log(L"Worker threads could not be started safely.");
            StopAndJoinWorkers();
            CleanupHandles();
            return FALSE;
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

void Wh_ModSettingsChanged() {
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

        bool cancelPlayback =
            PlaybackSettingsDiffer(previousSettings, updatedSettings);
        bool requestTest = updatedSettings.testSoundAfterSettingsChange;
        bool requestSoundConfiguration =
            updatedSettings.configureSoundSource &&
            !previousSettings.configureSoundSource;
        if (!ReplaceSettings(updatedSettings)) {
            Wh_Log(L"Settings reload failed; the previous settings remain "
                   L"active.");
            return;
        }
        Wh_Log(L"Settings reloaded successfully.");

        if (!g_isPrimaryInstance) {
            return;
        }

        if (cancelPlayback) {
            CancelAllPlayback();
        }

        if (g_settingsChangedEvent && !SetEvent(g_settingsChangedEvent)) {
            LogWin32Failure(L"SetEvent for settings changes", GetLastError());
        }

        if (requestSoundConfiguration) {
            RequestSoundConfiguration();
        } else if (requestTest) {
            QueuePlayback(PlaybackKind::Test, updatedSettings, false);
        }
    } catch (...) {
        Wh_Log(L"An exception occurred while applying changed settings.");
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"Low Battery Alarm shutdown started.");

    if (g_stopping.exchange(true)) {
        return;
    }

    try {
        CancelAllPlayback();
        StopAndJoinWorkers();
        Wh_Log(L"All Low Battery Alarm worker activity has stopped.");
    } catch (...) {
        Wh_Log(L"An exception occurred while stopping Low Battery Alarm; "
               L"forcing a final safe worker join.");
        SignalWorkersToStop();
        WaitForWorkers();
    }
}

void Wh_ModUninit() {
    try {
        CleanupHandles();
        Wh_Log(L"Low Battery Alarm cleanup completed successfully.");
    } catch (...) {
        Wh_Log(L"An exception occurred during final Low Battery Alarm cleanup.");
    }
}
