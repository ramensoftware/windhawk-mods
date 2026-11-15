// ==WindhawkMod==
// @id              bluetooth-auto-toggle
// @name            Bluetooth Auto Toggle
// @description     Automatically cycles Bluetooth off and back on in milliseconds when a connected device is detected at startup
// @version         1.4.0
// @author          Dev-StudioCode
// @github          https://github.com/Dev-StudioCode
// @include         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshell32 -lwtsapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Bluetooth Auto Toggle

A Windhawk tool mod that automatically cycles Windows Bluetooth off and back on
(in milliseconds) whenever Windhawk boots with your system and a connected
Bluetooth device is detected.

### Why this mod?
Some Bluetooth hardware adapters, drivers, and wireless peripherals (e.g.,
Bluetooth headphones, earbuds, mice, and keyboards) enter an unresponsive or
degraded state on fresh system boots even when Windows displays them as
"connected" causing audio silence, latency, no absolute volume or connection
drops unless Bluetooth is power-cycled. This mod automates that process by
waiting until a device is connected, pausing a configurable few seconds for the
driver stack to settle, and cleanly cycling the Bluetooth radio off and then
back on in the background.

### Key Features
- **Startup & Manual Turn-On Cycle:** Automatically cycles Bluetooth off and
back on at startup if active. If Bluetooth was OFF at boot, it listens for when
you manually turn it ON later and automatically triggers the fast toggle cycle.
- **Device-Aware Activation:** Optionally only cycles Bluetooth when a
connected Bluetooth device (headset, mouse, keyboard, etc.) is found.
- **Continuous Device Monitoring:** Optionally monitors for connected devices
and cycles Bluetooth whenever any eligible Bluetooth device connects throughout
the day.
- **Per-Device Target Filtering:** Supports allowing all connected devices by
default or restricting auto-toggle triggers to specific devices using flexible
keyword and token matching (explicit allowlist or blocklist rules).
- **Precise Millisecond Timing:** Configurable startup delay, post-connection
delay, and off-to-on toggle duration in milliseconds.
- **Dedicated Tool Mod Architecture:** Runs in an isolated `windhawk.exe`
background tool process without injecting into or hooking `explorer.exe` or any
other critical system process.
- **Cold Boot Hardware Retry:** Automatically retries radio enumeration if the
Bluetooth service (`bthserv`) or adapter driver is still initializing during
Windows boot.
- **State-Safe Cycling:** Only toggles radios that are already powered ON
(leaving radios in Airplane mode or turned off alone).
- **Safe Power Restoration & Crash Recovery:** Minimizes the off window, ensures
radios are commanded back on with retries even if the mod is unloaded mid-cycle,
and persistently tracks in-progress cycles to restore power upon next launch if
interrupted.
- **Session-Safe:** Remembers the current Windows logon session across tool
process restarts using Windows Terminal Services (WTS) logon timestamps.
- **Graceful Lifecycle & Live Testing:** Responsive to settings updates, mod
reload, and instant cancellation on shutdown.

### Changelog

- **v1.4.0**
  - Added `applyToAllDevices` setting (enabled by default) and `targetDevices`
filter list to allow restricting auto-toggle initiation to specific Bluetooth
devices or excluding unwanted peripherals.
  - Added flexible case-insensitive keyword and token matching with
allow/exclude toggles evaluated top-to-bottom.
  - Updated startup device detection and the `DeviceWatcher` event monitor to
respect per-device filter rules and avoid triggering on excluded peripherals.
  - Added crash and interruption recovery tracking (`cycleInProgress`) to
automatically restore Bluetooth radios back to ON if a cycle is ever interrupted.
  - Added 60-second expiration to dropped-device tracking so subsequent
reconnections are never permanently absorbed.
- **v1.3.0**
  - Added `toggleOnEveryDeviceConnection` setting to continuously monitor and
power-cycle Bluetooth on every newly connected Bluetooth device throughout the
session.
  - Event-driven device monitoring using Windows Runtime `DeviceWatcher`
(`System.Devices.Aep.IsConnected`) for zero idle CPU overhead.
  - Tracks devices dropped during radio cycling (`g_droppedByCycle`) so
subsequent reconnection of active devices does not trigger redundant cycles.
- **v1.2.0**
  - Replaced `onlyWhenDeviceConnected` with `activationMode` (`deviceConnected`
vs `startup`).
  - Added `startupBluetoothState` setting to enforce Bluetooth power state (ON /
OFF / Unchanged) at startup.
  - Added manual turn-on event detection if Bluetooth starts in an OFF state.
  - Reordered initialization flow so radio discovery and power state enforcement
run before device connection checks.
- **v1.1.0**
  - Added `onlyWhenDeviceConnected` setting to only cycle when a connected
device is detected.
  - Added WTS session logon tracking to persist single-cycle state across
restarts.
- **v1.0.0**
  - Initial release.
*/
// ==/WindhawkModReadme==

// clang-format off
// ==WindhawkModSettings==
/*
- startupBluetoothState: unchanged
  $name: Startup Bluetooth State
  $description: Choose whether Bluetooth should be forced ON or OFF when Windows starts (Windows persists radio states globally across reboots even if mod is disabled).
  $options:
  - unchanged: Keep previous Windows state (do not force ON or OFF)
  - on: Turn Bluetooth ON at startup
  - off: Turn Bluetooth OFF at startup (triggers fast toggle if turned ON later)

- activationMode: deviceConnected
  $name: Activation Mode
  $description: Choose when the Bluetooth cycle occurs (only after a connected Bluetooth device is detected, or on startup / manual turn-on).
  $options:
  - deviceConnected: Only when a device is connected
  - startup: On startup (cycle if ON, or on manual turn-on if OFF)

- toggleOnEveryDeviceConnection: false
  $name: Toggle on Every Device Connection
  $description: Continuously monitors and cycles Bluetooth whenever any eligible Bluetooth device connects throughout the session (briefly power-cycling the radio and disconnecting active devices). Overrides single-cycle limit for new device connections.

- applyToAllDevices: true
  $name: Apply to all devices by default
  $description: If enabled, any connected Bluetooth device without a matching rule can initiate the auto-toggle in device-connected mode or continuous monitoring. Disable this to only trigger for enabled devices in Target Devices List.

- targetDevices:
  - - name: ""
      $name: Device Name or Keyword
    - enabled: true
      $name: Enable Auto-Toggle
  $name: Target Devices List
  $description: Filter specific Bluetooth devices by name or keyword (e.g. Sony, AirPods, WH-1000XM4) for device-connected mode and continuous monitoring. Rules are evaluated top-to-bottom and the first matching rule wins.

- delayAfterDeviceConnectedMs: 2000
  $name: Delay After Device Found (ms)
  $description: Milliseconds to wait after a connected device is detected before executing the off-and-on cycle (minimum 0 ms, applied in device-connected mode and to connection-triggered runs).

- deviceWaitTimeoutSec: 60
  $name: Device Connection Wait Timeout (seconds)
  $description: Maximum seconds to wait for a Bluetooth device to connect after startup (0 for indefinite/infinite wait until a device connects).

- startupDelayMs: 2000
  $name: Startup Settle Delay (ms)
  $description: Milliseconds to wait after Windhawk starts before beginning checks (allows the driver stack to settle, minimum 0 ms).

- toggleDelayMs: 500
  $name: Toggle Off-to-On Duration (ms)
  $description: Duration in milliseconds the Bluetooth radio remains OFF before turning back ON (minimum 50 ms).

- maxRetries: 10
  $name: Radio Detection Retries
  $description: Number of attempts to locate the Bluetooth radio if not immediately enumerated on cold boot (minimum 1).

- retryIntervalMs: 500
  $name: Radio Retry Interval (ms)
  $description: Milliseconds to wait between radio discovery retry attempts (minimum 50 ms).

- onlyOncePerSession: true
  $name: Toggle Only Once Per Logon Session
  $description: Prevents re-cycling Bluetooth on startup if Windhawk is restarted or updated during the same Windows logon session.

- forceToggleOnSettingsChange: false
  $name: Re-toggle on Settings Save
  $description: If enabled, saving settings in Windhawk will immediately trigger a Bluetooth off-on toggle test (bypassing session check, startup delay, and connected device check).
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <wtsapi32.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cwctype>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Devices.Enumeration.h>
#include <winrt/Windows.Devices.Bluetooth.h>
#include <winrt/Windows.Devices.Radios.h>
// clang-format on

using namespace winrt;
using namespace winrt::Windows::Devices::Enumeration;
using namespace winrt::Windows::Devices::Bluetooth;
using namespace winrt::Windows::Devices::Radios;

enum class StartupBluetoothState { On, Off, Unchanged };

enum class ActivationMode { Startup, DeviceConnected };

struct DeviceFilterRule {
  std::wstring originalName;
  std::wstring nameLower;
  std::vector<std::wstring> tokens;
  bool enabled;
};

struct Settings {
  StartupBluetoothState startupBluetoothState;
  ActivationMode activationMode;
  bool toggleOnEveryDeviceConnection;
  bool applyToAllDevices;
  int delayAfterDeviceConnectedMs;
  int deviceWaitTimeoutSec;
  int startupDelayMs;
  int toggleDelayMs;
  int maxRetries;
  int retryIntervalMs;
  bool onlyOncePerSession;
  bool forceToggleOnSettingsChange;
};

static Settings g_settings;
static std::vector<DeviceFilterRule> g_deviceRules;
static SRWLOCK g_settingsLock = SRWLOCK_INIT;

static HANDLE g_hWorkerThread = NULL;
static HANDLE g_hStopEvent = NULL;
static HANDLE g_hTriggerEvent = NULL;
static HANDLE g_hWakeEvent = NULL;
static std::atomic<bool> g_forceNextRun{false};
static std::atomic<bool> g_isCycling{false};
static std::atomic<bool> g_isInitialRun{true};

static std::unordered_set<std::wstring> g_knownConnectedDeviceIds;
static SRWLOCK g_connectedDevicesLock = SRWLOCK_INIT;

static std::unordered_map<std::wstring, DWORD> g_droppedByCycle;
static SRWLOCK g_droppedByCycleLock = SRWLOCK_INIT;

static std::vector<std::wstring> g_pendingConnectedDeviceIds;
static std::vector<std::wstring> g_pendingDisconnectedDeviceIds;
static std::unordered_map<std::wstring, std::wstring> g_deviceNamesById;
static SRWLOCK g_pendingDevicesLock = SRWLOCK_INIT;

[[clang::no_destroy]] static DeviceWatcher g_classicWatcher{nullptr};
[[clang::no_destroy]] static DeviceWatcher g_leWatcher{nullptr};

static std::wstring TrimWhitespace(const std::wstring &str) {
  size_t first = 0;
  while (first < str.size() && iswspace((wint_t)str[first])) {
    ++first;
  }
  size_t last = str.size();
  while (last > first && iswspace((wint_t)str[last - 1])) {
    --last;
  }
  return str.substr(first, last - first);
}

static std::wstring ToLower(const std::wstring &str) {
  if (str.empty()) {
    return {};
  }
  int len = LCMapStringEx(LOCALE_NAME_INVARIANT, LCMAP_LOWERCASE, str.data(),
                          (int)str.size(), nullptr, 0, nullptr, nullptr, 0);
  if (len <= 0) {
    std::wstring result = str;
    for (wchar_t &c : result) {
      c = (wchar_t)towlower((wint_t)c);
    }
    return result;
  }
  std::wstring result(len, L'\0');
  LCMapStringEx(LOCALE_NAME_INVARIANT, LCMAP_LOWERCASE, str.data(),
                (int)str.size(), result.data(), len, nullptr, nullptr, 0);
  return result;
}

static std::vector<std::wstring> GetTokens(const std::wstring &strLower) {
  std::vector<std::wstring> tokens;
  std::wstring cur;
  bool hasDigit = false;
  auto flushToken = [&]() {
    if (!cur.empty()) {
      if (cur.length() >= 2 || hasDigit) {
        tokens.push_back(cur);
      }
      cur.clear();
      hasDigit = false;
    }
  };
  for (wchar_t ch : strLower) {
    if (iswalnum((wint_t)ch)) {
      if (iswdigit((wint_t)ch)) {
        hasDigit = true;
      }
      cur += ch;
    } else {
      flushToken();
    }
  }
  flushToken();
  return tokens;
}

static bool CheckDeviceMatch(const std::wstring &candidateLower,
                             const std::vector<std::wstring> &devTokens,
                             const DeviceFilterRule &rule) {
  if (candidateLower.empty() || rule.nameLower.empty()) {
    return false;
  }

  // 1. Direct substring match: device name contains the rule keyword or phrase
  if (candidateLower.find(rule.nameLower) != std::wstring::npos) {
    return true;
  }

  // 2. Token match: every meaningful rule token must match a device token
  if (rule.tokens.empty() || devTokens.empty()) {
    return false;
  }

  for (const auto &rt : rule.tokens) {
    bool tokenMatched = false;
    for (const auto &dt : devTokens) {
      if (dt == rt || (rt.length() >= 3 && dt.find(rt) != std::wstring::npos)) {
        tokenMatched = true;
        break;
      }
    }
    if (!tokenMatched) {
      return false;
    }
  }

  return true;
}

static bool ShouldDeviceInitiateToggle(const std::wstring &deviceName,
                                       bool *outMatchedRule = nullptr,
                                       std::wstring *outRuleName = nullptr) {
  AcquireSRWLockShared(&g_settingsLock);
  bool applyToAll = g_settings.applyToAllDevices;
  std::vector<DeviceFilterRule> rules = g_deviceRules;
  ReleaseSRWLockShared(&g_settingsLock);

  if (outMatchedRule) {
    *outMatchedRule = false;
  }
  if (outRuleName) {
    outRuleName->clear();
  }

  if (!deviceName.empty()) {
    std::wstring candidateLower = ToLower(deviceName);
    std::vector<std::wstring> devTokens = GetTokens(candidateLower);

    for (const auto &rule : rules) {
      if (CheckDeviceMatch(candidateLower, devTokens, rule)) {
        if (outMatchedRule) {
          *outMatchedRule = true;
        }
        if (outRuleName) {
          *outRuleName = rule.originalName;
        }
        return rule.enabled;
      }
    }
  }

  return applyToAll;
}

static DeviceWatcher StartConnectionWatcher(winrt::hstring const &selector) {
  try {
    auto watcher = DeviceInformation::CreateWatcher(
        selector,
        std::vector<winrt::hstring>{L"System.Devices.Aep.IsConnected"},
        DeviceInformationKind::AssociationEndpoint);

    watcher.Added([](DeviceWatcher const &,
                     DeviceInformation const &info) {
      try {
        std::wstring id = info.Id().c_str();
        std::wstring name = info.Name().c_str();
        if (!name.empty()) {
          AcquireSRWLockExclusive(&g_pendingDevicesLock);
          g_deviceNamesById[id] = std::move(name);
          ReleaseSRWLockExclusive(&g_pendingDevicesLock);
        }
      } catch (...) {
      }
    });

    watcher.Removed(
        [](DeviceWatcher const &, DeviceInformationUpdate const &update) {
          try {
            std::wstring id = update.Id().c_str();
            AcquireSRWLockExclusive(&g_pendingDevicesLock);
            g_deviceNamesById.erase(id);
            g_pendingDisconnectedDeviceIds.push_back(id);
            ReleaseSRWLockExclusive(&g_pendingDevicesLock);
          } catch (...) {
          }
        });

    watcher.Updated([](DeviceWatcher const &,
                       DeviceInformationUpdate const &update) {
      try {
        auto value =
            update.Properties().TryLookup(L"System.Devices.Aep.IsConnected");
        if (value) {
          bool isConnected = winrt::unbox_value<bool>(value);
          AcquireSRWLockExclusive(&g_pendingDevicesLock);
          if (isConnected) {
            g_pendingConnectedDeviceIds.push_back(update.Id().c_str());
          } else {
            g_pendingDisconnectedDeviceIds.push_back(update.Id().c_str());
          }
          ReleaseSRWLockExclusive(&g_pendingDevicesLock);

          if (isConnected && g_hWakeEvent) {
            SetEvent(g_hWakeEvent);
          }
        }
      } catch (...) {
      }
    });

    watcher.Start();
    return watcher;
  } catch (const winrt::hresult_error &ex) {
    Wh_Log(L"Exception starting connection watcher: 0x%08X (%s)",
           (unsigned int)ex.code(), ex.message().c_str());
    return nullptr;
  }
}

static void StopConnectionWatchers() {
  if (g_classicWatcher) {
    try {
      auto status = g_classicWatcher.Status();
      if (status == DeviceWatcherStatus::Started ||
          status == DeviceWatcherStatus::EnumerationCompleted) {
        g_classicWatcher.Stop();
      }
    } catch (...) {
    }
    g_classicWatcher = nullptr;
  }

  if (g_leWatcher) {
    try {
      auto status = g_leWatcher.Status();
      if (status == DeviceWatcherStatus::Started ||
          status == DeviceWatcherStatus::EnumerationCompleted) {
        g_leWatcher.Stop();
      }
    } catch (...) {
    }
    g_leWatcher = nullptr;
  }
}

static void EnsureConnectionWatchers(bool enable) {
  if (enable) {
    if (!g_classicWatcher) {
      try {
        g_classicWatcher = StartConnectionWatcher(
            BluetoothDevice::GetDeviceSelectorFromPairingState(true));
      } catch (...) {
      }
    }
    if (!g_leWatcher) {
      try {
        g_leWatcher = StartConnectionWatcher(
            BluetoothLEDevice::GetDeviceSelectorFromPairingState(true));
      } catch (...) {
      }
    }
  } else {
    StopConnectionWatchers();
  }
}

struct RadioSubscription {
  Radio radio{nullptr};
  winrt::event_token token{};
};
static std::vector<RadioSubscription> g_radioSubscriptions;
static SRWLOCK g_subscriptionsLock = SRWLOCK_INIT;

static void UnsubscribeRadioStateChanged() {
  AcquireSRWLockExclusive(&g_subscriptionsLock);
  for (auto &sub : g_radioSubscriptions) {
    try {
      if (sub.radio) {
        sub.radio.StateChanged(sub.token);
      }
    } catch (...) {
    }
  }
  g_radioSubscriptions.clear();
  ReleaseSRWLockExclusive(&g_subscriptionsLock);
}

static void LoadSettings() {
  Settings newSettings{};

  WindhawkUtils::StringSetting state =
      WindhawkUtils::StringSetting::make(L"startupBluetoothState");
  if (wcscmp(state, L"on") == 0) {
    newSettings.startupBluetoothState = StartupBluetoothState::On;
  } else if (wcscmp(state, L"off") == 0) {
    newSettings.startupBluetoothState = StartupBluetoothState::Off;
  } else {
    newSettings.startupBluetoothState = StartupBluetoothState::Unchanged;
  }

  WindhawkUtils::StringSetting mode =
      WindhawkUtils::StringSetting::make(L"activationMode");
  if (wcscmp(mode, L"startup") == 0) {
    newSettings.activationMode = ActivationMode::Startup;
  } else if (wcscmp(mode, L"deviceConnected") == 0) {
    newSettings.activationMode = ActivationMode::DeviceConnected;
  } else {
    newSettings.activationMode =
        (Wh_GetIntSetting(L"onlyWhenDeviceConnected") == 0)
            ? ActivationMode::Startup
            : ActivationMode::DeviceConnected;
  }

  newSettings.toggleOnEveryDeviceConnection =
      Wh_GetIntSetting(L"toggleOnEveryDeviceConnection") != 0;

  newSettings.applyToAllDevices = Wh_GetIntSetting(L"applyToAllDevices") != 0;

  std::vector<DeviceFilterRule> localRules;
  for (int i = 0; i < 100; ++i) {
    WindhawkUtils::StringSetting ruleName =
        WindhawkUtils::StringSetting::make(L"targetDevices[%d].name", i);
    std::wstring trimmedName = TrimWhitespace(ruleName.get());
    if (trimmedName.empty()) {
      continue;
    }

    DeviceFilterRule rule;
    rule.originalName = trimmedName;
    rule.nameLower = ToLower(trimmedName);
    rule.tokens = GetTokens(rule.nameLower);
    rule.enabled = Wh_GetIntSetting(L"targetDevices[%d].enabled", i) != 0;
    localRules.push_back(std::move(rule));
  }

  newSettings.delayAfterDeviceConnectedMs =
      std::max(Wh_GetIntSetting(L"delayAfterDeviceConnectedMs"), 0);

  newSettings.deviceWaitTimeoutSec =
      std::max(Wh_GetIntSetting(L"deviceWaitTimeoutSec"), 0);

  newSettings.startupDelayMs = std::max(Wh_GetIntSetting(L"startupDelayMs"), 0);

  newSettings.toggleDelayMs = std::max(Wh_GetIntSetting(L"toggleDelayMs"), 50);

  newSettings.maxRetries = std::max(Wh_GetIntSetting(L"maxRetries"), 1);

  newSettings.retryIntervalMs =
      std::max(Wh_GetIntSetting(L"retryIntervalMs"), 50);

  newSettings.onlyOncePerSession = Wh_GetIntSetting(L"onlyOncePerSession") != 0;
  newSettings.forceToggleOnSettingsChange =
      Wh_GetIntSetting(L"forceToggleOnSettingsChange") != 0;

  size_t rulesCount = localRules.size();

  AcquireSRWLockExclusive(&g_settingsLock);
  g_settings = newSettings;
  g_deviceRules = std::move(localRules);
  ReleaseSRWLockExclusive(&g_settingsLock);

  Wh_Log(
      L"Settings: startupState=%s, mode=%s, toggleEveryConn=%d, "
      L"applyAll=%d, targetRules=%zu, delayAfterConn=%dms, "
      L"waitTimeout=%ds, startupDelay=%dms, toggleDelay=%dms, onlyOnce=%d",
      newSettings.startupBluetoothState == StartupBluetoothState::Off
          ? L"off"
          : (newSettings.startupBluetoothState == StartupBluetoothState::Unchanged
                 ? L"unchanged"
                 : L"on"),
      newSettings.activationMode == ActivationMode::Startup ? L"startup"
                                                            : L"deviceConnected",
      (int)newSettings.toggleOnEveryDeviceConnection,
      (int)newSettings.applyToAllDevices, rulesCount,
      newSettings.delayAfterDeviceConnectedMs, newSettings.deviceWaitTimeoutSec,
      newSettings.startupDelayMs, newSettings.toggleDelayMs,
      (int)newSettings.onlyOncePerSession);
}

static bool GetLogonTime(LONGLONG *logonTime, DWORD *outSessionId) {
  DWORD sessionId;
  WTSINFOW *info;
  DWORD bytes;
  if (!ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) ||
      !WTSQuerySessionInformationW(WTS_CURRENT_SERVER_HANDLE, sessionId,
                                   WTSSessionInfo, (LPWSTR *)&info, &bytes)) {
    return false;
  }
  *logonTime = info->LogonTime.QuadPart;
  *outSessionId = sessionId;
  WTSFreeMemory(info);
  return true;
}

static bool HasSessionMarker(PCWSTR prefix) {
  LONGLONG logonTime, stored = 0;
  DWORD sessionId;
  if (!GetLogonTime(&logonTime, &sessionId)) {
    return false;
  }
  WCHAR valueName[64];
  swprintf_s(valueName, L"%s_%u", prefix, sessionId);
  return Wh_GetBinaryValue(valueName, &stored, sizeof(stored)) ==
             sizeof(stored) &&
         stored == logonTime;
}

static void SetSessionMarker(PCWSTR prefix) {
  LONGLONG logonTime;
  DWORD sessionId;
  if (GetLogonTime(&logonTime, &sessionId)) {
    WCHAR valueName[64];
    swprintf_s(valueName, L"%s_%u", prefix, sessionId);
    Wh_SetBinaryValue(valueName, &logonTime, sizeof(logonTime));
  }
}

static bool GetConnectedBluetoothDevices(
    std::vector<std::pair<std::wstring, std::wstring>> &outDevices) {
  outDevices.clear();
  try {
    auto aqsClassic = BluetoothDevice::GetDeviceSelectorFromConnectionStatus(
        BluetoothConnectionStatus::Connected);
    auto devicesClassic = DeviceInformation::FindAllAsync(aqsClassic).get();
    for (uint32_t i = 0; i < devicesClassic.Size(); ++i) {
      auto dev = devicesClassic.GetAt(i);
      outDevices.emplace_back(dev.Id().c_str(), dev.Name().c_str());
    }

    auto aqsBle = BluetoothLEDevice::GetDeviceSelectorFromConnectionStatus(
        BluetoothConnectionStatus::Connected);
    auto devicesBle = DeviceInformation::FindAllAsync(aqsBle).get();
    for (uint32_t i = 0; i < devicesBle.Size(); ++i) {
      auto dev = devicesBle.GetAt(i);
      outDevices.emplace_back(dev.Id().c_str(), dev.Name().c_str());
    }
    return true;
  } catch (const winrt::hresult_error &ex) {
    Wh_Log(L"Exception checking connected devices: 0x%08X (%s)",
           (unsigned int)ex.code(), ex.message().c_str());
    return false;
  }
}

static bool GetFirstEligibleConnectedBluetoothDevice(
    std::wstring &outName,
    std::vector<std::wstring> *outSkippedNames = nullptr) {
  std::vector<std::pair<std::wstring, std::wstring>> devices;
  if (!GetConnectedBluetoothDevices(devices)) {
    return false;
  }
  for (const auto &[id, name] : devices) {
    if (ShouldDeviceInitiateToggle(name)) {
      outName = name;
      return true;
    }
    if (outSkippedNames) {
      outSkippedNames->push_back(name);
    }
  }
  return false;
}

static void PerformBluetoothToggle(bool isManualTest = false,
                                   bool isConnectionTriggered = false) {
  Settings cfg;
  AcquireSRWLockShared(&g_settingsLock);
  cfg = g_settings;
  ReleaseSRWLockShared(&g_settingsLock);

  bool isInitial = g_isInitialRun.exchange(false);

  bool skipForSession = !isManualTest && cfg.onlyOncePerSession &&
                        HasSessionMarker(L"toggledLogon") &&
                        (isInitial || !cfg.toggleOnEveryDeviceConnection);
  if (skipForSession && !Wh_GetIntValue(L"cycleInProgress", 0)) {
    Wh_Log(L"Bluetooth has already been toggled in this logon session. "
           L"Skipping.");
    return;
  }

  if (isConnectionTriggered && cfg.delayAfterDeviceConnectedMs > 0) {
    Wh_Log(L"Waiting %d ms settle time after device connection before "
           L"toggling...",
           cfg.delayAfterDeviceConnectedMs);
    if (WaitForSingleObject(g_hStopEvent, cfg.delayAfterDeviceConnectedMs) ==
        WAIT_OBJECT_0) {
      return;
    }
  }

  if (!isManualTest && isInitial && cfg.startupDelayMs > 0) {
    Wh_Log(L"Waiting initial startup delay of %d ms...", cfg.startupDelayMs);
    if (WaitForSingleObject(g_hStopEvent, cfg.startupDelayMs) ==
        WAIT_OBJECT_0) {
      Wh_Log(L"Stop event signaled during startup delay.");
      return;
    }
  }

  std::vector<Radio> targetRadios;
  for (int attempt = 1; attempt <= cfg.maxRetries; ++attempt) {
    if (WaitForSingleObject(g_hStopEvent, 0) == WAIT_OBJECT_0) {
      return;
    }

    try {
      RadioAccessStatus access = Radio::RequestAccessAsync().get();
      if (access == RadioAccessStatus::Allowed) {
        auto radios = Radio::GetRadiosAsync().get();
        for (const auto &r : radios) {
          if (r.Kind() == RadioKind::Bluetooth) {
            targetRadios.push_back(r);
          }
        }
      } else {
        Wh_Log(L"Radio::RequestAccessAsync returned status: %d (attempt %d/%d)",
               (int)access, attempt, cfg.maxRetries);
      }
    } catch (const winrt::hresult_error &ex) {
      Wh_Log(L"Exception during radio discovery (attempt %d): 0x%08X (%s)",
             attempt, (unsigned int)ex.code(), ex.message().c_str());
    }

    if (!targetRadios.empty()) {
      Wh_Log(L"Found %u Bluetooth radio(s) on attempt %d.",
             (unsigned int)targetRadios.size(), attempt);
      break;
    }

    if (attempt < cfg.maxRetries) {
      Wh_Log(L"Bluetooth radio not found yet (attempt %d/%d). Retrying in %d "
             L"ms...",
             attempt, cfg.maxRetries, cfg.retryIntervalMs);
      if (WaitForSingleObject(g_hStopEvent, cfg.retryIntervalMs) ==
          WAIT_OBJECT_0) {
        return;
      }
    }
  }

  if (targetRadios.empty()) {
    Wh_Log(L"No Bluetooth radio found after %d detection attempts.",
           cfg.maxRetries);
    return;
  }

  if (Wh_GetIntValue(L"cycleInProgress", 0)) {
    Wh_Log(L"Previous cycle was interrupted, restoring Bluetooth radios to ON "
           L"state...");
    for (const auto &radio : targetRadios) {
      try {
        radio.SetStateAsync(RadioState::On).get();
      } catch (...) {
      }
    }
    Wh_DeleteValue(L"cycleInProgress");
  }

  if (skipForSession) {
    Wh_Log(L"Bluetooth has already been toggled in this logon session. "
           L"Skipping.");
    return;
  }

  if (!isManualTest && !HasSessionMarker(L"startupStateLogon")) {
    if (cfg.startupBluetoothState == StartupBluetoothState::On) {
      for (const auto &radio : targetRadios) {
        try {
          if (radio.State() != RadioState::On) {
            std::wstring name = radio.Name().c_str();
            Wh_Log(L"Adapter \"%s\" is not ON at startup. Powering ON as "
                   L"configured...",
                   name.c_str());
            RadioAccessStatus res = radio.SetStateAsync(RadioState::On).get();
            if (res != RadioAccessStatus::Allowed) {
              Wh_Log(L"Adapter \"%s\" -> SetStateAsync(On) status: %d",
                     name.c_str(), (int)res);
            }
          }
        } catch (const winrt::hresult_error &ex) {
          Wh_Log(L"Exception powering ON adapter: 0x%08X (%s)",
                 (unsigned int)ex.code(), ex.message().c_str());
        }
      }
    } else if (cfg.startupBluetoothState == StartupBluetoothState::Off) {
      for (const auto &radio : targetRadios) {
        try {
          if (radio.State() == RadioState::On) {
            std::wstring name = radio.Name().c_str();
            Wh_Log(L"Adapter \"%s\" is ON at startup. Powering OFF as "
                   L"configured...",
                   name.c_str());
            RadioAccessStatus res = radio.SetStateAsync(RadioState::Off).get();
            if (res != RadioAccessStatus::Allowed) {
              Wh_Log(L"Adapter \"%s\" -> SetStateAsync(Off) status: %d",
                     name.c_str(), (int)res);
            }
          }
        } catch (const winrt::hresult_error &ex) {
          Wh_Log(L"Exception powering OFF adapter: 0x%08X (%s)",
                 (unsigned int)ex.code(), ex.message().c_str());
        }
      }
    }
    SetSessionMarker(L"startupStateLogon");
  }

  bool anyRadioOn = false;
  for (const auto &radio : targetRadios) {
    try {
      if (radio.State() == RadioState::On) {
        anyRadioOn = true;
        break;
      }
    } catch (...) {
    }
  }

  if (!anyRadioOn && !isManualTest) {
    Wh_Log(L"Bluetooth is currently OFF. Listening for manual turn-on to "
           L"trigger fast toggle...");
    UnsubscribeRadioStateChanged();
    AcquireSRWLockExclusive(&g_subscriptionsLock);
    for (const auto &radio : targetRadios) {
      try {
        winrt::event_token token =
            radio.StateChanged([onlyOnce = cfg.onlyOncePerSession,
                                everyConn = cfg.toggleOnEveryDeviceConnection](
                                   Radio const &sender, auto const &) {
              try {
                if (sender.State() == RadioState::On) {
                  if (!g_isCycling.load() &&
                      (everyConn || !onlyOnce ||
                       !HasSessionMarker(L"toggledLogon"))) {
                    Wh_Log(L"Bluetooth radio was manually turned ON later! "
                           L"Triggering fast toggle...");
                    SetEvent(g_hTriggerEvent);
                  }
                }
              } catch (...) {
              }
            });
        g_radioSubscriptions.push_back({radio, token});
      } catch (...) {
      }
    }
    ReleaseSRWLockExclusive(&g_subscriptionsLock);
    return;
  }

  if (!isManualTest && !isConnectionTriggered &&
      cfg.activationMode == ActivationMode::DeviceConnected) {
    std::wstring connectedDeviceName;
    std::vector<std::wstring> skippedNames;
    if (!GetFirstEligibleConnectedBluetoothDevice(connectedDeviceName,
                                                  &skippedNames)) {
      if (!cfg.applyToAllDevices) {
        AcquireSRWLockShared(&g_settingsLock);
        bool hasRules = !g_deviceRules.empty();
        ReleaseSRWLockShared(&g_settingsLock);
        if (!hasRules) {
          Wh_Log(L"Warning: 'applyToAllDevices' is disabled and no target "
                 L"device rules are configured. No devices will match.");
        }
      }

      if (!skippedNames.empty()) {
        Wh_Log(L"Connected Bluetooth device(s) found, but excluded by device "
               L"filter rules. Waiting for an eligible device (timeout: %d "
               L"s)...",
               cfg.deviceWaitTimeoutSec);
        for (const auto &skipped : skippedNames) {
          Wh_Log(L"  Skipped device: \"%s\"", skipped.c_str());
        }
      } else {
        Wh_Log(L"Monitoring for connected Bluetooth device (timeout: %d s)...",
               cfg.deviceWaitTimeoutSec);
      }
      DWORD startTick = GetTickCount();

      while (true) {
        if (WaitForSingleObject(g_hStopEvent, 0) == WAIT_OBJECT_0) {
          return;
        }

        if (GetFirstEligibleConnectedBluetoothDevice(connectedDeviceName)) {
          Wh_Log(L"Eligible connected Bluetooth device detected: \"%s\"",
                 connectedDeviceName.c_str());
          break;
        }

        if (cfg.deviceWaitTimeoutSec > 0) {
          DWORD elapsedSec = (GetTickCount() - startTick) / 1000;
          if (elapsedSec >= (DWORD)cfg.deviceWaitTimeoutSec) {
            Wh_Log(L"No eligible Bluetooth device connected within timeout "
                   L"window (%d s). Skipping toggle.",
                   cfg.deviceWaitTimeoutSec);
            return;
          }
        }

        HANDLE waitStartupHandles[] = {g_hStopEvent, g_hWakeEvent};
        DWORD waitRes =
            WaitForMultipleObjects(2, waitStartupHandles, FALSE, 1000);
        if (waitRes == WAIT_OBJECT_0) {
          return;
        }
      }
    } else {
      Wh_Log(L"Eligible connected Bluetooth device detected: \"%s\"",
             connectedDeviceName.c_str());
    }

    if (cfg.delayAfterDeviceConnectedMs > 0) {
      Wh_Log(L"Waiting %d ms settle time after device connection before "
             L"toggling...",
             cfg.delayAfterDeviceConnectedMs);
      if (WaitForSingleObject(g_hStopEvent, cfg.delayAfterDeviceConnectedMs) ==
          WAIT_OBJECT_0) {
        return;
      }
    }
  }

  g_isCycling.store(true);
  Wh_SetIntValue(L"cycleInProgress", 1);

  std::vector<std::pair<std::wstring, std::wstring>> connectedNow;
  if (GetConnectedBluetoothDevices(connectedNow)) {
    DWORD now = GetTickCount();
    AcquireSRWLockExclusive(&g_droppedByCycleLock);
    for (const auto &[id, name] : connectedNow) {
      g_droppedByCycle[id] = now;
    }
    ReleaseSRWLockExclusive(&g_droppedByCycleLock);
  }

  std::vector<std::pair<Radio, std::wstring>> cycledRadios;
  for (const auto &radio : targetRadios) {
    try {
      std::wstring name = radio.Name().c_str();
      RadioState state = radio.State();
      if (state != RadioState::On) {
        Wh_Log(L"Adapter \"%s\" is not ON (state: %d). Leaving as-is.",
               name.c_str(), (int)state);
        continue;
      }

      Wh_Log(L"Adapter \"%s\" is ON. Commanding OFF...", name.c_str());
      RadioAccessStatus res = radio.SetStateAsync(RadioState::Off).get();
      if (res == RadioAccessStatus::Allowed) {
        cycledRadios.emplace_back(radio, name);
        Wh_Log(L"Adapter \"%s\" commanded OFF successfully.", name.c_str());
      } else {
        Wh_Log(L"Adapter \"%s\" -> SetStateAsync(Off) status: %d", name.c_str(),
               (int)res);
      }
    } catch (const winrt::hresult_error &ex) {
      Wh_Log(L"Error commanding Bluetooth OFF: 0x%08X (%s)",
             (unsigned int)ex.code(), ex.message().c_str());
    }
  }

  if (cycledRadios.empty()) {
    Wh_DeleteValue(L"cycleInProgress");
    g_isCycling.store(false);
    Wh_Log(L"No active Bluetooth radios to cycle.");
    return;
  }

  Wh_Log(L"Waiting %d ms before turning Bluetooth back ON...",
         cfg.toggleDelayMs);
  WaitForSingleObject(g_hStopEvent, cfg.toggleDelayMs);

  for (const auto &[radio, name] : cycledRadios) {
    Wh_Log(L"Adapter \"%s\". Commanding back ON...", name.c_str());

    bool turnedOn = false;
    for (int retry = 0; retry < 3 && !turnedOn; ++retry) {
      try {
        turnedOn = radio.SetStateAsync(RadioState::On).get() ==
                   RadioAccessStatus::Allowed;
      } catch (...) {
      }
      if (!turnedOn) {
        Sleep(100);
      }
    }

    try {
      Wh_Log(L"Adapter \"%s\" final state: %d (success=%d)", name.c_str(),
             (int)radio.State(), (int)turnedOn);
    } catch (...) {
      Wh_Log(L"Adapter \"%s\" state read failed after turn ON (success=%d)",
             name.c_str(), (int)turnedOn);
    }
  }

  Wh_DeleteValue(L"cycleInProgress");

  if (!isManualTest) {
    SetSessionMarker(L"toggledLogon");
    UnsubscribeRadioStateChanged();
  }

  g_isCycling.store(false);
  Wh_Log(L"Bluetooth automatic toggle cycle completed successfully.");
}

static DWORD WINAPI BluetoothWorkerThread(LPVOID lpParam) {
  Wh_Log(L"Bluetooth worker thread started.");

  try {
    winrt::init_apartment(winrt::apartment_type::multi_threaded);
  } catch (const winrt::hresult_error &ex) {
    Wh_Log(L"Failed to initialize WinRT apartment: 0x%08X",
           (unsigned int)ex.code());
    return 1;
  }

  HANDLE waitHandles[] = {g_hStopEvent, g_hTriggerEvent, g_hWakeEvent};

  while (true) {
    Settings cfg;
    AcquireSRWLockShared(&g_settingsLock);
    cfg = g_settings;
    ReleaseSRWLockShared(&g_settingsLock);

    EnsureConnectionWatchers(cfg.toggleOnEveryDeviceConnection);

    DWORD waitRes = WaitForMultipleObjects(3, waitHandles, FALSE, INFINITE);

    if (waitRes == WAIT_OBJECT_0) {
      Wh_Log(L"Worker thread stop event signaled.");
      break;
    } else if (waitRes == WAIT_OBJECT_0 + 1) {
      Wh_Log(L"Toggle trigger event signaled.");
      bool isManual = g_forceNextRun.exchange(false);
      PerformBluetoothToggle(isManual);

      AcquireSRWLockShared(&g_settingsLock);
      bool watchEnabled = g_settings.toggleOnEveryDeviceConnection;
      ReleaseSRWLockShared(&g_settingsLock);

      if (watchEnabled) {
        std::vector<std::pair<std::wstring, std::wstring>> currentDevices;
        if (GetConnectedBluetoothDevices(currentDevices)) {
          AcquireSRWLockExclusive(&g_connectedDevicesLock);
          g_knownConnectedDeviceIds.clear();
          for (const auto &dev : currentDevices) {
            g_knownConnectedDeviceIds.insert(dev.first);
          }
          ReleaseSRWLockExclusive(&g_connectedDevicesLock);

          AcquireSRWLockExclusive(&g_pendingDevicesLock);
          for (const auto &dev : currentDevices) {
            if (!dev.second.empty()) {
              g_deviceNamesById[dev.first] = dev.second;
            }
          }
          ReleaseSRWLockExclusive(&g_pendingDevicesLock);
        }
      }
    } else if (waitRes == WAIT_OBJECT_0 + 2) {
      AcquireSRWLockShared(&g_settingsLock);
      bool watchEnabled = g_settings.toggleOnEveryDeviceConnection;
      ReleaseSRWLockShared(&g_settingsLock);

      if (!watchEnabled) {
        AcquireSRWLockExclusive(&g_pendingDevicesLock);
        g_pendingConnectedDeviceIds.clear();
        g_pendingDisconnectedDeviceIds.clear();
        ReleaseSRWLockExclusive(&g_pendingDevicesLock);
        continue;
      }

      std::vector<std::wstring> connectedPending;
      std::vector<std::wstring> disconnectedPending;
      AcquireSRWLockExclusive(&g_pendingDevicesLock);
      connectedPending.swap(g_pendingConnectedDeviceIds);
      disconnectedPending.swap(g_pendingDisconnectedDeviceIds);
      ReleaseSRWLockExclusive(&g_pendingDevicesLock);

      AcquireSRWLockExclusive(&g_connectedDevicesLock);
      for (const auto &id : disconnectedPending) {
        g_knownConnectedDeviceIds.erase(id);
      }
      ReleaseSRWLockExclusive(&g_connectedDevicesLock);

      if (connectedPending.empty()) {
        continue;
      }

      std::vector<std::wstring> newCandidateIds;

      AcquireSRWLockExclusive(&g_droppedByCycleLock);
      AcquireSRWLockExclusive(&g_connectedDevicesLock);

      DWORD now = GetTickCount();
      for (const auto &id : connectedPending) {
        // If this device was dropped by our own cycle recently (within 60s), absorb reconnect
        auto it = g_droppedByCycle.find(id);
        if (it != g_droppedByCycle.end()) {
          DWORD elapsed = now - it->second;
          g_droppedByCycle.erase(it);
          if (elapsed <= 60000) {
            g_knownConnectedDeviceIds.insert(id);
            continue;
          }
        }

        // If already known as connected, ignore
        if (g_knownConnectedDeviceIds.find(id) !=
            g_knownConnectedDeviceIds.end()) {
          continue;
        }

        g_knownConnectedDeviceIds.insert(id);
        newCandidateIds.push_back(id);
      }

      ReleaseSRWLockExclusive(&g_connectedDevicesLock);
      ReleaseSRWLockExclusive(&g_droppedByCycleLock);

      if (newCandidateIds.empty()) {
        continue;
      }

      // Query currently connected devices once to resolve fresh names
      std::unordered_map<std::wstring, std::wstring> liveNamesById;
      std::vector<std::pair<std::wstring, std::wstring>> liveConnected;
      if (GetConnectedBluetoothDevices(liveConnected)) {
        AcquireSRWLockExclusive(&g_pendingDevicesLock);
        for (const auto &[devId, devName] : liveConnected) {
          if (!devName.empty()) {
            liveNamesById[devId] = devName;
            g_deviceNamesById[devId] = devName;
          }
        }
        ReleaseSRWLockExclusive(&g_pendingDevicesLock);
      }

      bool hasNewEligibleDevice = false;
      std::wstring triggerDeviceName;

      for (const auto &id : newCandidateIds) {
        std::wstring devName;
        auto itLive = liveNamesById.find(id);
        if (itLive != liveNamesById.end()) {
          devName = itLive->second;
        } else {
          AcquireSRWLockShared(&g_pendingDevicesLock);
          auto itCached = g_deviceNamesById.find(id);
          if (itCached != g_deviceNamesById.end()) {
            devName = itCached->second;
          }
          ReleaseSRWLockShared(&g_pendingDevicesLock);
        }

        bool matchedRule = false;
        std::wstring ruleName;
        if (ShouldDeviceInitiateToggle(devName, &matchedRule, &ruleName)) {
          if (!hasNewEligibleDevice) {
            hasNewEligibleDevice = true;
            triggerDeviceName = devName;
          }
          if (matchedRule) {
            Wh_Log(L"Newly connected device \"%s\" matched enabled rule "
                   L"\"%s\".",
                   devName.empty() ? id.c_str() : devName.c_str(),
                   ruleName.c_str());
          }
        } else {
          if (matchedRule) {
            Wh_Log(L"Ignoring newly connected device \"%s\" (matched disabled "
                   L"rule \"%s\").",
                   devName.empty() ? id.c_str() : devName.c_str(),
                   ruleName.c_str());
          } else {
            Wh_Log(L"Ignoring newly connected device \"%s\" (not in target "
                   L"devices allowlist).",
                   devName.empty() ? id.c_str() : devName.c_str());
          }
        }
      }

      if (hasNewEligibleDevice) {
        Wh_Log(L"New eligible Bluetooth device connection detected via "
               L"watcher (\"%s\"). Triggering cycle...",
               triggerDeviceName.empty() ? L"Unknown"
                                         : triggerDeviceName.c_str());
        PerformBluetoothToggle(false, true);
      }
    } else {
      break;
    }
  }

  StopConnectionWatchers();
  UnsubscribeRadioStateChanged();

  try {
    winrt::uninit_apartment();
  } catch (...) {
  }

  Wh_Log(L"Bluetooth worker thread exiting.");
  return 0;
}

BOOL WhTool_ModInit() {
  Wh_Log(L"Bluetooth Auto Toggle tool mod initializing...");

  LoadSettings();

  g_hStopEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
  if (!g_hStopEvent) {
    Wh_Log(L"CreateEvent (stop) failed: %d", GetLastError());
    return FALSE;
  }

  g_hTriggerEvent = CreateEventW(NULL, FALSE, TRUE, NULL);
  if (!g_hTriggerEvent) {
    Wh_Log(L"CreateEvent (trigger) failed: %d", GetLastError());
    CloseHandle(g_hStopEvent);
    g_hStopEvent = NULL;
    return FALSE;
  }

  g_hWakeEvent = CreateEventW(NULL, FALSE, FALSE, NULL);
  if (!g_hWakeEvent) {
    Wh_Log(L"CreateEvent (wake) failed: %d", GetLastError());
    CloseHandle(g_hTriggerEvent);
    g_hTriggerEvent = NULL;
    CloseHandle(g_hStopEvent);
    g_hStopEvent = NULL;
    return FALSE;
  }

  g_hWorkerThread = CreateThread(NULL, 0, BluetoothWorkerThread, NULL, 0, NULL);
  if (!g_hWorkerThread) {
    Wh_Log(L"CreateThread failed: %d", GetLastError());
    CloseHandle(g_hWakeEvent);
    g_hWakeEvent = NULL;
    CloseHandle(g_hTriggerEvent);
    g_hTriggerEvent = NULL;
    CloseHandle(g_hStopEvent);
    g_hStopEvent = NULL;
    return FALSE;
  }

  Wh_Log(L"Bluetooth Auto Toggle tool mod initialized successfully.");
  return TRUE;
}

void WhTool_ModUninit() {
  Wh_Log(L"Bluetooth Auto Toggle tool mod uninitializing...");

  if (g_hStopEvent) {
    SetEvent(g_hStopEvent);
  }

  if (g_hWorkerThread) {
    WaitForSingleObject(g_hWorkerThread, 15000);
    CloseHandle(g_hWorkerThread);
    g_hWorkerThread = NULL;
  }

  if (g_hStopEvent) {
    CloseHandle(g_hStopEvent);
    g_hStopEvent = NULL;
  }

  if (g_hTriggerEvent) {
    CloseHandle(g_hTriggerEvent);
    g_hTriggerEvent = NULL;
  }

  if (g_hWakeEvent) {
    CloseHandle(g_hWakeEvent);
    g_hWakeEvent = NULL;
  }

  Wh_Log(L"Bluetooth Auto Toggle tool mod uninitialized.");
}

void WhTool_ModSettingsChanged() {
  Wh_Log(L"Settings changed notification received.");
  LoadSettings();

  AcquireSRWLockShared(&g_settingsLock);
  bool force = g_settings.forceToggleOnSettingsChange;
  ReleaseSRWLockShared(&g_settingsLock);

  if (force && g_hTriggerEvent) {
    Wh_Log(L"forceToggleOnSettingsChange enabled, triggering immediate test "
           L"toggle...");
    g_forceNextRun.store(true);
    SetEvent(g_hTriggerEvent);
  } else if (g_hWakeEvent) {
    SetEvent(g_hWakeEvent);
  }
}

// clang-format off
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
    LPWSTR *argv = CommandLineToArgvW(GetCommandLine(), &argc);
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

        IMAGE_DOS_HEADER *dosHeader = (IMAGE_DOS_HEADER *)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS *ntHeaders =
            (IMAGE_NT_HEADERS *)((BYTE *)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void *entryPoint = (BYTE *)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void *)EntryPoint_Hook, nullptr);
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

    WCHAR commandLine[MAX_PATH + 2 +
                      (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) -
                      1];
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

    using CreateProcessInternalW_t = BOOL(WINAPI *)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation,
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
// clang-format on
