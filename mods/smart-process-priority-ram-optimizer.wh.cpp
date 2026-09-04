// ==WindhawkMod==
// @id              smart-process-priority-ram-optimizer
// @name            Smart Process Priority & RAM Optimizer
// @description     Boosts foreground responsiveness, shields audio and AI workloads, throttles runaway background CPU, and safely reclaims idle memory.
// @version         1.0.0
// @author          gilnett & Antigravity
// @github          https://github.com/gilnett
// @include         explorer.exe
// @compilerOptions -lntdll -lpsapi -ladvapi32 -lole32 -lshell32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Smart Process Priority & RAM Optimizer (v1.0.0)

Boosts the responsiveness of the active foreground application, protects real-time audio and local AI engines from throttling or trimming, throttles CPU-heavy background tasks, and safely reclaims idle memory with hardware-protective SSD safeguards.

## Foreground Priority & I/O Boost
Elevates the active window's process to Above Normal or High CPU priority and High disk I/O priority via a system event hook, and restores the previous priority when focus moves away. The foreground process is never trimmed.

## Local AI Engine Handling
Recognizes common local AI inference processes (llama-server, LM Studio, Ollama, KoboldCPP, Jan, ComfyUI, and others). While one is actively generating - detected from CPU usage or working-set changes, so GPU-offloaded inference is covered too - it keeps full priority and its memory is never trimmed. After a configurable period of inactivity, its memory can be released to the Windows standby cache and reloads almost instantly on the next prompt.

## Audio & Multimedia Shield
Monitors active WASAPI audio sessions and exempts the whole process family of an app currently playing audio (browser, renderer, and helper processes) from CPU throttling and memory trimming.

## Multitasking vs Single-Task Adaptation
Detects rapid window switching and extends grace periods and protects visible windows accordingly. Detects fullscreen/3D game windows and runs one preventive memory sweep before the game allocates its own memory.

## Automatic Process Classification
Skips processes outside the interactive user session (background services) and packaged UWP/MSIX apps (already managed by Windows itself), regardless of name, on top of the manual exclusion list.

## SSD-Protective Memory Reclaim
Trims idle background processes' working sets, gated by per-process cooldowns and free-RAM thresholds to avoid unnecessary SSD writes. Below 5% free RAM these cooldowns are relaxed so the mod can react before the system runs out of memory.

## Panic Hotkey
Ctrl+Alt+F11 triggers an immediate cleanup pass.

## Credits & Acknowledgments
- **Inspirations & Concepts**:
  - **Process Lasso (Bitsum)**: Inspired by the ProBalance concept for foreground responsiveness and background runaway CPU restraint.
  - **LiveTuner (LT)**: Inspired by dynamic real-time priority tuning and responsiveness heuristics.
  - **ISLC (Intelligent Standby List Cleaner by Wagnardsoft)**: Inspired by adaptive threshold triggers and gaming memory management.
  - **Mem Reduct (Henry++)**: Inspired by safe working-set trimming techniques.
- **Development**:
  - Developed by **gilnett** with architectural and optimization assistance from **Antigravity**.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enableProBalance: true
  $name: Enable Foreground Priority Boost (CPU & I/O)
  $description: Automatically elevates the active foreground application's CPU and I/O priority for maximum smoothness.
- foregroundPriorityLevel: "aboveNormal"
  $name: Foreground Priority Level
  $description: Priority class assigned to the active application in use.
  $options:
    - "aboveNormal": Above Normal (Recommended - Balanced & Rock-Solid)
    - "high": High Priority (Maximum Performance for Gaming)
- enableBackgroundThrottling: true
  $name: Throttle CPU-Heavy Background Apps
  $description: Temporarily lowers the priority of background processes that spike CPU usage while a boosted foreground app is active.
- backgroundCpuThrottleThresholdPercent: 15
  $name: Background CPU Throttle Threshold (%)
  $description: A background process using more total CPU than this (or maxing out a full CPU core) while a foreground app is boosted gets temporarily throttled (5% to 50%).
- enableSmartAiOptimization: true
  $name: Smart Local AI Optimization
  $description: Protects recognized local AI engines during active generation and releases dormant compute memory after inactivity.
- aiInactivityGraceMinutes: 5
  $name: AI Inactivity Grace Period (Minutes)
  $description: Minutes without token generation before releasing dormant AI model scratch buffers (1 to 30 minutes).
- enableAudioShielding: true
  $name: Audio & Multimedia Stream Shield (Anti-Glitch)
  $description: Fully exempts apps playing audio, and their helper processes, from CPU throttling and memory trimming.
- enableMultitaskingAdaptation: true
  $name: Dynamic Multitasking vs Immersion Adaptation
  $description: Automatically quadruples grace periods and protects visible windows during frequent Alt+Tab switching.
- enableGameModeDetection: true
  $name: Detect Fullscreen / 3D Games & Pre-Sweep RAM
  $description: Runs one preventive memory cleanup when a new 3D/fullscreen game takes focus, before it allocates memory.
- cleanMode: "smartThreshold"
  $name: Cleanup Trigger Mode
  $description: Choose when memory optimization should automatically trigger.
  $options:
    - "smartThreshold": Smart Threshold (When Free RAM drops below threshold %)
    - "periodic": Periodic Timer (Clean at regular time intervals)
    - "smartAndPeriodic": Smart Threshold + Periodic (Recommended)
- freeRamThresholdPercent: 20
  $name: Free RAM Trigger Threshold (%)
  $description: Triggers cleanup when available physical RAM drops below this percentage (5% to 50%).
- enableIdleBoost: true
  $name: Aggressive Cleanup When System Idle
  $description: Proactively reclaims memory when no user input has been detected for the idle threshold duration.
- idleThresholdMinutes: 15
  $name: Idle Threshold (Minutes)
  $description: Minutes without user input before system is considered idle (5 to 120 minutes).
- trimMinimizedWindows: true
  $name: Trim Minimized Windows
  $description: Automatically reclaims unused memory from applications that have been minimized to the taskbar.
- enableProcessAging: true
  $name: Smart Inactivity Aging (Protects Recent Alt+Tab)
  $description: Do not trim applications that were active in the foreground recently.
- recentActivityGraceMinutes: 3
  $name: Inactivity Grace Period (Minutes)
  $description: How many minutes an app must remain in the background before its memory is trimmed (1 to 60 minutes).
- enableProcessTreeTrimming: true
  $name: Trim Whole Process Trees Together
  $description: When a minimized app's main window is trimmed, also trims its background helper processes.
- enableElectronMemoryCap: true
  $name: Cap Memory for Heavy Memory Hogs List
  $description: Trims a process from the Heavy Memory Hogs list as soon as it exceeds the memory cap below.
- electronMemoryCapMb: 500
  $name: Memory Cap for Heavy Memory Hogs List (MB)
  $description: Background processes from the Heavy Memory Hogs list are trimmed once their working set exceeds this size (100 to 4000 MB).
- cleanBackgroundWorkingSets: true
  $name: Trim Background Applications Working Sets
  $description: Releases unused RAM held by background processes.
- minProcessMemoryToTrimMb: 50
  $name: Minimum Process RAM to Trim (MB)
  $description: Only trim processes consuming more than this amount of RAM to prevent SSD write micro-thrashing.
- periodicIntervalMinutes: 10
  $name: Periodic Cleanup Interval (Minutes)
  $description: Used when Periodic mode is enabled (1 to 60 minutes).
- targetProcessesOnly: false
  $name: Only Target Heavy Memory Hogs
  $description: If enabled, only trims processes from the list below.
- customTargetList: "zen.exe, chrome.exe, msedge.exe, brave.exe, firefox.exe, opera.exe, vivaldi.exe, discord.exe, slack.exe, teams.exe, telegram.exe, whatsapp.exe, signal.exe, skype.exe, spotify.exe, steam.exe, epicgameslauncher.exe, code.exe, obs64.exe"
  $name: Heavy Memory Hogs Process List
  $description: List of executables, comma-separated, used by "Only Target Heavy Memory Hogs" and by the memory cap feature.
- excludedProcesses: "explorer.exe, windhawk.exe, dwm.exe, csrss.exe, lsass.exe, smss.exe, services.exe, system, wininit.exe, winlogon.exe, svchost.exe, memcompression, registry, fontdrvhost.exe, audiodg.exe, sihost.exe, taskhostw.exe, ctfmon.exe, wlanext.exe, dashost.exe"
  $name: Excluded Critical Processes (Hardware & Crash Protection)
  $description: Critical OS processes that should NEVER be trimmed or throttled, comma-separated.
- enablePanicHotkey: true
  $name: Enable Panic Clean Hotkey (Ctrl+Alt+F11)
  $description: Press Ctrl+Alt+F11 anytime to trigger an immediate memory cleanup pass.
- pauseOnBattery: false
  $name: Pause on Battery Power
  $description: Pause periodic cleanups when running on battery to maximize laptop battery life.
- checkIntervalSec: 3
  $name: Watchdog Check Interval (seconds)
  $description: How often the watchdog monitors memory and background CPU usage (1 to 30 seconds).
- enableLogging: true
  $name: Enable Diagnostic Logging
  $description: Displays detailed RAM recovery stats in Windhawk's log tab.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <appmodel.h>
#include <audiopolicy.h>
#include <mmdeviceapi.h>
#include <psapi.h>
#include <shellapi.h>
#include <tlhelp32.h>
#include <windows.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <deque>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>

// ---------------------------------------------------------------------------
// NTDLL & Low-Level Definitions
// ---------------------------------------------------------------------------

typedef NTSTATUS(NTAPI *pfnNtSetInformationProcess)(
    HANDLE ProcessHandle, INT ProcessInformationClass, PVOID ProcessInformation,
    ULONG ProcessInformationLength);

typedef HRESULT(WINAPI *pfnSHQueryUserNotificationState)(
    QUERY_USER_NOTIFICATION_STATE *pquns);

static constexpr INT ProcessIoPriorityInfoClass = 33;
enum IoPriorityHint : ULONG {
  IoPriorityVeryLow = 0,
  IoPriorityLow = 1,
  IoPriorityNormal = 2,
  IoPriorityHigh = 3,
};

#ifndef PROCESS_POWER_THROTTLING_CURRENT_VERSION
typedef struct _PROCESS_POWER_THROTTLING_STATE {
  ULONG Version;
  ULONG ControlMask;
  ULONG StateMask;
} PROCESS_POWER_THROTTLING_STATE, *PPROCESS_POWER_THROTTLING_STATE;
#define PROCESS_POWER_THROTTLING_CURRENT_VERSION 1
#define PROCESS_POWER_THROTTLING_EXECUTION_SPEED 0x1
#endif
static constexpr INT ProcessPowerThrottlingInfoClass = 4;

static pfnNtSetInformationProcess g_pfnNtSetInformationProcess = nullptr;
static pfnSHQueryUserNotificationState g_pfnSHQueryUserNotificationState =
    nullptr;

static constexpr UINT_PTR kPanicHotkeyId = 0xA1CE;

// Values that used to be settings but had little real configuration value;
// kept as sensible fixed defaults instead of cluttering the settings UI.
static constexpr int kTriggerCooldownSec = 30;
static constexpr int kLogTopAppsCount = 5;
static constexpr int kTopAppsLogThresholdMb = 500;

// ---------------------------------------------------------------------------
// Priority Class Ranking & CPU Core Helpers
// ---------------------------------------------------------------------------
// Win32 priority classes are arbitrary bit flags (e.g. NORMAL is 0x20=32,
// IDLE is 0x40=64, BELOW_NORMAL is 0x4000=16384). Comparing them directly with
// < or > produces completely incorrect results. This helper maps them to a
// monotonic linear scale.
static int PriorityClassToRank(DWORD priorityClass) {
  switch (priorityClass) {
    case IDLE_PRIORITY_CLASS:
      return 1;
    case BELOW_NORMAL_PRIORITY_CLASS:
      return 2;
    case NORMAL_PRIORITY_CLASS:
      return 3;
    case ABOVE_NORMAL_PRIORITY_CLASS:
      return 4;
    case HIGH_PRIORITY_CLASS:
      return 5;
    case REALTIME_PRIORITY_CLASS:
      return 6;
    default:
      return 0;
  }
}

static DWORD GetSystemCoreCount() {
  static const DWORD numCores = [] {
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return (std::max)(si.dwNumberOfProcessors, (DWORD)1);
  }();
  return numCores;
}

// ---------------------------------------------------------------------------
// Settings Structure & Enums
// ---------------------------------------------------------------------------

enum class CleanMode {
  SmartThreshold,
  Periodic,
  SmartAndPeriodic,
};

enum class ForegroundPrioritySetting {
  AboveNormal,
  High,
};

struct ModSettings {
  bool enableProBalance = true;
  ForegroundPrioritySetting foregroundPriorityLevel =
      ForegroundPrioritySetting::AboveNormal;
  bool enableBackgroundThrottling = true;
  int backgroundCpuThrottleThresholdPercent = 15;
  bool enableSmartAiOptimization = true;
  int aiInactivityGraceMinutes = 5;
  bool enableAudioShielding = true;
  bool enableMultitaskingAdaptation = true;
  bool enableGameModeDetection = true;
  CleanMode cleanMode = CleanMode::SmartAndPeriodic;
  int freeRamThresholdPercent = 20;
  bool enableIdleBoost = true;
  int idleThresholdMinutes = 15;
  bool trimMinimizedWindows = true;
  bool enableProcessAging = true;
  int recentActivityGraceMinutes = 3;
  bool enableProcessTreeTrimming = true;
  bool enableElectronMemoryCap = true;
  int electronMemoryCapMb = 500;
  bool cleanBackgroundWorkingSets = true;
  DWORD minProcessMemoryToTrimMb = 50;
  int periodicIntervalMinutes = 10;
  bool targetProcessesOnly = false;
  std::vector<std::wstring> customTargetList;
  std::vector<std::wstring> excludedProcesses;
  bool enablePanicHotkey = true;
  bool pauseOnBattery = false;
  int checkIntervalSec = 3;
  bool enableLogging = true;
};

static ModSettings g_settings;
static std::mutex g_settingsMutex;

static ModSettings GetSettingsSnapshot() {
  std::lock_guard<std::mutex> lock(g_settingsMutex);
  return g_settings;
}

// ---------------------------------------------------------------------------
// Worker / Hook Thread State
// ---------------------------------------------------------------------------

static std::atomic<bool> g_workerRunning{false};
static HANDLE g_stopEvent = nullptr;
static HANDLE g_wakeEvent = nullptr;
static std::thread g_workerThread;

static std::atomic<bool> g_hookThreadRunning{false};
static HANDLE g_hookThreadHandle = nullptr;
static DWORD g_hookThreadId = 0;
static HWINEVENTHOOK g_winEventHook = nullptr;
static bool g_panicHotkeyRegistered = false;

static std::atomic<bool> g_forceCleanupRequested{false};
static std::atomic<bool> g_gameSweepRequested{false};

// Foreground priority-boost state, guarded by g_probalanceMutex.
static std::mutex g_probalanceMutex;
static DWORD g_currentBoostedPid = 0;
static DWORD g_originalBoostedPriority = NORMAL_PRIORITY_CLASS;

// Background CPU-throttling state; only touched from the worker thread.
static std::map<DWORD, DWORD> g_throttledProcesses; // pid -> original priority
struct CpuSample {
  ULONGLONG kernelPlusUser100ns = 0;
  std::chrono::steady_clock::time_point sampleTime{};
};
static std::map<DWORD, CpuSample> g_cpuSamples;

// Consecutive failed CPU samples for a throttled process; forces a
// de-escalation if we can no longer verify it's still CPU-heavy.
static std::map<DWORD, DWORD> g_throttleStaleSampleCount;

// AI Workload Tracking (Inference Activity Timestamps)
static std::map<DWORD, std::chrono::steady_clock::time_point>
    g_aiLastInferenceTime;
// Last known working-set size per AI pid; a meaningful change is used as a
// second "still active" signal alongside CPU usage (see
// ApplyBackgroundThrottling).
static std::map<DWORD, SIZE_T> g_aiLastWorkingSetSize;

// Focus / Trim Bookkeeping & Multitasking Tracker
static std::mutex g_focusMapMutex;
static std::map<DWORD, std::chrono::steady_clock::time_point>
    g_processLastFocusedTime;
static std::deque<std::chrono::steady_clock::time_point> g_focusSwitchHistory;

// Only touched by the worker thread.
static std::map<DWORD, std::chrono::steady_clock::time_point>
    g_processLastTrimmed;

// Game-sweep debounce; only touched by the hook thread.
static DWORD g_lastGameSweepPid = 0;
static std::chrono::steady_clock::time_point g_lastGameSweepTime{};

static std::chrono::steady_clock::time_point g_lastPeriodicCleanTime{};
static std::chrono::steady_clock::time_point g_lastTriggerCleanTime{};
static std::chrono::steady_clock::time_point g_lastIdleCleanTime{};
static bool g_wasIdle = false;

// Session stats.
static std::chrono::steady_clock::time_point g_modStartTime{};
static ULONGLONG g_sessionBytesReclaimed = 0;
static DWORD g_sessionCleanupPasses = 0;
static DWORD g_sessionProcessesTrimmedTotal = 0;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static std::wstring ToLower(std::wstring str) {
  std::transform(str.begin(), str.end(), str.begin(), ::towlower);
  return str;
}

static std::wstring Trim(const std::wstring &str) {
  size_t first = str.find_first_not_of(L" \t\r\n");
  if (first == std::wstring::npos)
    return L"";
  size_t last = str.find_last_not_of(L" \t\r\n");
  return str.substr(first, (last - first + 1));
}

static std::vector<std::wstring> ParseProcessList(const std::wstring &input) {
  std::vector<std::wstring> result;
  std::wstring current;
  auto flush = [&]() {
    std::wstring trimmed = ToLower(Trim(current));
    if (!trimmed.empty()) {
      result.push_back(trimmed);
    }
    current.clear();
  };
  for (wchar_t c : input) {
    if (c == L',' || c == L'\n' || c == L'\r') {
      flush();
    } else {
      current.push_back(c);
    }
  }
  flush();
  return result;
}

static bool IsMainShellProcess() {
  HWND hShell = GetShellWindow();
  if (hShell) {
    DWORD shellPid = 0;
    GetWindowThreadProcessId(hShell, &shellPid);
    if (shellPid != 0 && shellPid != GetCurrentProcessId()) {
      return false;
    }
  }
  return true;
}

static bool IsRunningOnBattery() {
  SYSTEM_POWER_STATUS sps;
  if (GetSystemPowerStatus(&sps)) {
    return (sps.ACLineStatus == 0);
  }
  return false;
}

static DWORD GetForegroundProcessId() {
  HWND fgWnd = GetForegroundWindow();
  if (!fgWnd)
    return 0;
  DWORD pid = 0;
  GetWindowThreadProcessId(fgWnd, &pid);
  return pid;
}

static DWORD GetSystemIdleSeconds() {
  LASTINPUTINFO lii;
  lii.cbSize = sizeof(LASTINPUTINFO);
  if (!GetLastInputInfo(&lii))
    return 0;
  DWORD idleMs = GetTickCount() - lii.dwTime;
  return idleMs / 1000;
}

static std::wstring FormatUptime(std::chrono::steady_clock::time_point start,
                                 std::chrono::steady_clock::time_point now) {
  int64_t totalSeconds =
      std::chrono::duration_cast<std::chrono::seconds>(now - start).count();
  if (totalSeconds < 0)
    totalSeconds = 0;
  int64_t hours = totalSeconds / 3600;
  int64_t minutes = (totalSeconds % 3600) / 60;
  int64_t seconds = totalSeconds % 60;

  wchar_t buf[64];
  if (hours > 0) {
    swprintf_s(buf, L"%lldh %lldm", hours, minutes);
  } else if (minutes > 0) {
    swprintf_s(buf, L"%lldm %llds", minutes, seconds);
  } else {
    swprintf_s(buf, L"%llds", seconds);
  }
  return buf;
}

static bool IsInList(const std::wstring &name,
                     const std::vector<std::wstring> &list) {
  for (const auto &item : list) {
    if (name == item)
      return true;
  }
  return false;
}

static bool IsKnownAiProcess(const std::wstring &name) {
  static const std::vector<std::wstring> kAiProcesses = {
      L"llama-server.exe", L"lm studio.exe",
      L"ollama.exe",       L"ollama_llama_server.exe",
      L"koboldcpp.exe",    L"jan.exe",
      L"cortex.exe",       L"text-generation-webui.exe",
      L"comfyui.exe",      L"vllm.exe",
      L"tabby.exe"};
  return IsInList(name, kAiProcesses);
}

// ---------------------------------------------------------------------------
// Generic Process Classification (name-list independent)
// ---------------------------------------------------------------------------

// True if pid runs in the same session as this mod (the interactive user
// session). Services and other session-0 processes fail this check, which
// is a more robust way to exclude them than name-matching alone.
static bool IsInteractiveSessionProcess(DWORD pid) {
  static const DWORD ourSessionId = [] {
    DWORD sid = 0;
    ProcessIdToSessionId(GetCurrentProcessId(), &sid);
    return sid;
  }();
  DWORD sid = 0;
  if (!ProcessIdToSessionId(pid, &sid))
    return false;
  return sid == ourSessionId;
}

// True if hProcess belongs to a packaged (UWP/MSIX) app. Windows already
// suspends/trims these itself via its Process Lifetime Manager, so this
// mod's own throttling would be redundant at best.
static bool IsPackagedApp(HANDLE hProcess) {
  UINT32 len = 0;
  LONG rc = GetPackageFullName(hProcess, &len, nullptr);
  return rc != APPMODEL_ERROR_NO_PACKAGE && len > 0;
}

// ---------------------------------------------------------------------------
// Multitasking vs Immersion Tracker
// ---------------------------------------------------------------------------

static void RecordFocusSwitch(DWORD pid) {
  auto now = std::chrono::steady_clock::now();
  std::lock_guard<std::mutex> lock(g_focusMapMutex);
  g_processLastFocusedTime[pid] = now;
  g_focusSwitchHistory.push_back(now);

  while (!g_focusSwitchHistory.empty()) {
    auto diff = std::chrono::duration_cast<std::chrono::seconds>(
                    now - g_focusSwitchHistory.front())
                    .count();
    if (diff > 60) {
      g_focusSwitchHistory.pop_front();
    } else {
      break;
    }
  }
}

static bool IsActiveMultiTaskingMode() {
  auto now = std::chrono::steady_clock::now();
  std::lock_guard<std::mutex> lock(g_focusMapMutex);
  // Re-validate the 60s window here too: RecordFocusSwitch() only prunes
  // on a NEW switch, so stale entries would otherwise linger forever.
  while (!g_focusSwitchHistory.empty()) {
    auto diff = std::chrono::duration_cast<std::chrono::seconds>(
                    now - g_focusSwitchHistory.front())
                    .count();
    if (diff > 60) {
      g_focusSwitchHistory.pop_front();
    } else {
      break;
    }
  }
  return g_focusSwitchHistory.size() >= 3;
}

// ---------------------------------------------------------------------------
// Audio Session Shield (WASAPI Real-Time Protection)
// ---------------------------------------------------------------------------

static std::unordered_set<DWORD> GetActiveAudioProcessIds() {
  std::unordered_set<DWORD> audioPids;

  HRESULT hrCom = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

  IMMDeviceEnumerator *pEnumerator = nullptr;
  HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                                CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
                                reinterpret_cast<void **>(&pEnumerator));
  if (SUCCEEDED(hr) && pEnumerator) {
    IMMDevice *pDevice = nullptr;
    hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDevice);
    if (SUCCEEDED(hr) && pDevice) {
      IAudioSessionManager2 *pSessionManager = nullptr;
      hr = pDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL,
                             nullptr,
                             reinterpret_cast<void **>(&pSessionManager));
      if (SUCCEEDED(hr) && pSessionManager) {
        IAudioSessionEnumerator *pSessionList = nullptr;
        hr = pSessionManager->GetSessionEnumerator(&pSessionList);
        if (SUCCEEDED(hr) && pSessionList) {
          int count = 0;
          pSessionList->GetCount(&count);
          for (int i = 0; i < count; i++) {
            IAudioSessionControl *pSessionControl = nullptr;
            if (SUCCEEDED(pSessionList->GetSession(i, &pSessionControl)) &&
                pSessionControl) {
              AudioSessionState state = AudioSessionStateInactive;
              if (SUCCEEDED(pSessionControl->GetState(&state)) &&
                  state == AudioSessionStateActive) {
                IAudioSessionControl2 *pControl2 = nullptr;
                if (SUCCEEDED(pSessionControl->QueryInterface(
                        __uuidof(IAudioSessionControl2),
                        reinterpret_cast<void **>(&pControl2))) &&
                    pControl2) {
                  DWORD pid = 0;
                  if (SUCCEEDED(pControl2->GetProcessId(&pid)) && pid != 0) {
                    audioPids.insert(pid);
                  }
                  pControl2->Release();
                }
              }
              pSessionControl->Release();
            }
          }
          pSessionList->Release();
        }
        pSessionManager->Release();
      }
      pDevice->Release();
    }
    pEnumerator->Release();
  }

  // Only uninit if THIS call initialized COM (S_OK). S_FALSE means it was
  // already initialized elsewhere; uninitializing then would unbalance it.
  if (hrCom == S_OK) {
    CoUninitialize();
  }

  return audioPids;
}

// Short cache to avoid enumerating WASAPI sessions twice back-to-back
// when a cleanup pass follows a throttling tick.
static std::chrono::steady_clock::time_point g_audioPidsCacheTime{};
static std::unordered_set<DWORD> g_audioPidsCache;

static std::unordered_set<DWORD> GetActiveAudioProcessIdsCached() {
  auto now = std::chrono::steady_clock::now();
  if (g_audioPidsCacheTime.time_since_epoch().count() != 0 &&
      std::chrono::duration_cast<std::chrono::milliseconds>(
          now - g_audioPidsCacheTime)
              .count() < 1500) {
    return g_audioPidsCache;
  }
  g_audioPidsCache = GetActiveAudioProcessIds();
  g_audioPidsCacheTime = now;
  return g_audioPidsCache;
}

// ---------------------------------------------------------------------------
// Process Tree Helper
// ---------------------------------------------------------------------------

static void CollectDescendants(
    DWORD rootPid, const std::map<DWORD, std::vector<DWORD>> &childrenOf,
    std::vector<DWORD> &outDescendants, std::unordered_set<DWORD> &visited) {
  auto it = childrenOf.find(rootPid);
  if (it == childrenOf.end())
    return;
  for (DWORD childPid : it->second) {
    if (visited.count(childPid))
      continue;
    visited.insert(childPid);
    outDescendants.push_back(childPid);
    CollectDescendants(childPid, childrenOf, outDescendants, visited);
  }
}

// ---------------------------------------------------------------------------
// I/O Priority & EcoQoS Helpers
// ---------------------------------------------------------------------------

static void SetProcessIoPriorityHint(HANDLE hProcess, ULONG priority) {
  if (!g_pfnNtSetInformationProcess)
    return;
  g_pfnNtSetInformationProcess(hProcess, ProcessIoPriorityInfoClass, &priority,
                               sizeof(priority));
}

static void SetProcessEcoQoS(HANDLE hProcess, bool enableThrottling) {
  PROCESS_POWER_THROTTLING_STATE state{};
  state.Version = PROCESS_POWER_THROTTLING_CURRENT_VERSION;
  state.ControlMask = PROCESS_POWER_THROTTLING_EXECUTION_SPEED;
  state.StateMask =
      enableThrottling ? PROCESS_POWER_THROTTLING_EXECUTION_SPEED : 0;
  SetProcessInformation(
      hProcess, (PROCESS_INFORMATION_CLASS)ProcessPowerThrottlingInfoClass,
      &state, sizeof(state));
}

// ---------------------------------------------------------------------------
// Foreground Priority Boost (Zero-Stutter)
// ---------------------------------------------------------------------------

static void UpdateForegroundBoost(DWORD newForegroundPid,
                                  const ModSettings &settings) {
  std::lock_guard<std::mutex> lock(g_probalanceMutex);

  if (!settings.enableProBalance) {
    if (g_currentBoostedPid != 0) {
      HANDLE hOld = OpenProcess(PROCESS_SET_INFORMATION |
                                    PROCESS_QUERY_LIMITED_INFORMATION,
                                FALSE, g_currentBoostedPid);
      if (hOld) {
        SetPriorityClass(hOld, g_originalBoostedPriority);
        SetProcessIoPriorityHint(hOld, IoPriorityNormal);
        CloseHandle(hOld);
      }
      g_currentBoostedPid = 0;
    }
    return;
  }

  DWORD currentPid = GetCurrentProcessId();

  if (newForegroundPid == g_currentBoostedPid) {
    return;
  }

  // 1. Restore previous foreground app priority.
  if (g_currentBoostedPid != 0 && g_currentBoostedPid != newForegroundPid &&
      g_currentBoostedPid != currentPid) {
    HANDLE hOld =
        OpenProcess(PROCESS_SET_INFORMATION | PROCESS_QUERY_LIMITED_INFORMATION,
                    FALSE, g_currentBoostedPid);
    if (hOld) {
      SetPriorityClass(hOld, g_originalBoostedPriority);
      SetProcessIoPriorityHint(hOld, IoPriorityNormal);
      CloseHandle(hOld);
    }
    g_currentBoostedPid = 0;
  }

  // 2. Elevate the new foreground application (WITHOUT trimming its working
  // set).
  if (newForegroundPid != 0 && newForegroundPid != currentPid &&
      newForegroundPid != 4) {
    HANDLE hNew =
        OpenProcess(PROCESS_SET_INFORMATION | PROCESS_QUERY_LIMITED_INFORMATION,
                    FALSE, newForegroundPid);
    if (hNew) {
      DWORD prevPriority = GetPriorityClass(hNew);
      g_originalBoostedPriority =
          (prevPriority != 0) ? prevPriority : NORMAL_PRIORITY_CLASS;

      DWORD targetPriority =
          (settings.foregroundPriorityLevel == ForegroundPrioritySetting::High)
              ? HIGH_PRIORITY_CLASS
              : ABOVE_NORMAL_PRIORITY_CLASS;

      // Only elevate if it was normal or lower (IDLE, BELOW_NORMAL, NORMAL).
      int origRank = PriorityClassToRank(g_originalBoostedPriority);
      if (origRank > 0 &&
          origRank <= PriorityClassToRank(NORMAL_PRIORITY_CLASS)) {
        if (SetPriorityClass(hNew, targetPriority)) {
          g_currentBoostedPid = newForegroundPid;
          SetProcessIoPriorityHint(hNew, IoPriorityHigh);
          SetProcessEcoQoS(hNew, /*enableThrottling=*/false);
        }
      }
      CloseHandle(hNew);
    }
  }
}

static DWORD GetCurrentlyBoostedPid() {
  std::lock_guard<std::mutex> lock(g_probalanceMutex);
  return g_currentBoostedPid;
}

// ---------------------------------------------------------------------------
// Background CPU/I/O Throttling with Audio & AI Workload Protection
// ---------------------------------------------------------------------------

static double SampleCpuPercent(DWORD pid, HANDLE hProcess) {
  FILETIME creation, exit, kernel, user;
  if (!GetProcessTimes(hProcess, &creation, &exit, &kernel, &user))
    return -1.0;

  ULARGE_INTEGER k, u;
  k.LowPart = kernel.dwLowDateTime;
  k.HighPart = kernel.dwHighDateTime;
  u.LowPart = user.dwLowDateTime;
  u.HighPart = user.dwHighDateTime;
  ULONGLONG totalTime100ns = k.QuadPart + u.QuadPart;
  auto now = std::chrono::steady_clock::now();

  double cpuPercent = -1.0;
  auto it = g_cpuSamples.find(pid);
  if (it != g_cpuSamples.end()) {
    ULONGLONG delta100ns =
        (totalTime100ns > it->second.kernelPlusUser100ns)
            ? (totalTime100ns - it->second.kernelPlusUser100ns)
            : 0;
    auto deltaWallMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                           now - it->second.sampleTime)
                           .count();
    if (deltaWallMs > 0) {
      double deltaTimeMs = delta100ns / 10000.0;
      cpuPercent = (deltaTimeMs / (double)deltaWallMs) * 100.0 /
                   GetSystemCoreCount();
    }
  }

  g_cpuSamples[pid] = {totalTime100ns, now};
  return cpuPercent;
}

struct ProcessSnapshotEntry {
  DWORD pid = 0;
  DWORD parentPid = 0;
  std::wstring name;
};

// Short cache so a cleanup pass immediately following a throttling tick
// doesn't walk the whole process list twice for (nearly) the same answer.
static std::chrono::steady_clock::time_point g_processSnapshotCacheTime{};
static std::vector<ProcessSnapshotEntry> g_processSnapshotCache;

static std::vector<ProcessSnapshotEntry> CaptureProcessSnapshotCached() {
  auto now = std::chrono::steady_clock::now();
  if (g_processSnapshotCacheTime.time_since_epoch().count() != 0 &&
      std::chrono::duration_cast<std::chrono::milliseconds>(
          now - g_processSnapshotCacheTime)
              .count() < 2000) {
    return g_processSnapshotCache;
  }

  std::vector<ProcessSnapshotEntry> result;
  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (snapshot != INVALID_HANDLE_VALUE) {
    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(PROCESSENTRY32W);
    if (Process32FirstW(snapshot, &pe)) {
      do {
        ProcessSnapshotEntry entry;
        entry.pid = pe.th32ProcessID;
        entry.parentPid = pe.th32ParentProcessID;
        entry.name = ToLower(pe.szExeFile);
        result.push_back(std::move(entry));
      } while (Process32NextW(snapshot, &pe));
    }
    CloseHandle(snapshot);
  }

  g_processSnapshotCache = std::move(result);
  g_processSnapshotCacheTime = now;
  return g_processSnapshotCache;
}

static void ApplyBackgroundThrottling(const ModSettings &settings,
                                      DWORD foregroundPid) {
  if (!settings.enableBackgroundThrottling || foregroundPid == 0)
    return;

  DWORD currentPid = GetCurrentProcessId();
  auto now = std::chrono::steady_clock::now();

  // Shield active audio sessions from being throttled.
  std::unordered_set<DWORD> rawAudioPids;
  if (settings.enableAudioShielding) {
    rawAudioPids = GetActiveAudioProcessIdsCached();
  }

  bool isMultiTasking =
      settings.enableMultitaskingAdaptation && IsActiveMultiTaskingMode();

  std::unordered_set<DWORD> alivePids;
  std::map<DWORD, std::wstring> procNames;
  for (const auto &entry : CaptureProcessSnapshotCached()) {
    alivePids.insert(entry.pid);
    procNames[entry.pid] = entry.name;
  }

  // Expand audio shield to entire process tree (all siblings/parent with same
  // exe name).
  std::unordered_set<DWORD> activeAudioPids = rawAudioPids;
  for (DWORD aPid : rawAudioPids) {
    auto itName = procNames.find(aPid);
    if (itName != procNames.end()) {
      const std::wstring &aName = itName->second;
      for (const auto &kv : procNames) {
        if (kv.second == aName) {
          activeAudioPids.insert(kv.first);
        }
      }
    }
  }

  for (DWORD pid : alivePids) {
    if (pid == 0 || pid == 4 || pid == currentPid || pid == foregroundPid)
      continue;

    const std::wstring &name = procNames[pid];
    if (IsInList(name, settings.excludedProcesses))
      continue;

    // Session-0/service processes aren't something the user is "using";
    // skip them regardless of name (more robust than a name blocklist).
    if (!IsInteractiveSessionProcess(pid))
      continue;

    bool isAi = settings.enableSmartAiOptimization && IsKnownAiProcess(name);

    // Audio stream protection for entire browser/media tree.
    if (settings.enableAudioShielding && activeAudioPids.count(pid)) {
      if (g_throttledProcesses.count(pid)) {
        HANDLE hProc = OpenProcess(PROCESS_SET_INFORMATION, FALSE, pid);
        if (hProc) {
          SetPriorityClass(hProc, g_throttledProcesses[pid]);
          SetProcessIoPriorityHint(hProc, IoPriorityNormal);
          SetProcessEcoQoS(hProc, false);
          CloseHandle(hProc);
        }
        g_throttledProcesses.erase(pid);
      }
      continue;
    }

    HANDLE hProc =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_SET_INFORMATION,
                    FALSE, pid);
    if (!hProc)
      continue;

    // Packaged (UWP/MSIX) apps are already suspended/managed by Windows
    // itself; leave them alone rather than fight with the OS scheduler.
    if (IsPackagedApp(hProc)) {
      CloseHandle(hProc);
      continue;
    }

    double cpuPercent = SampleCpuPercent(pid, hProc);
    bool isThrottled = g_throttledProcesses.count(pid) != 0;

    // Track AI token generation activity. CPU usage alone misses
    // GPU-offloaded inference (CUDA/Vulkan), where the host process can
    // stay near 0% CPU while still actively serving requests - so a
    // working-set change (KV cache growth, etc.) also counts as activity.
    if (isAi) {
      if (cpuPercent >= 2.0) {
        g_aiLastInferenceTime[pid] = now;
      }
      PROCESS_MEMORY_COUNTERS_EX pmc{};
      pmc.cb = sizeof(pmc);
      if (GetProcessMemoryInfo(
              hProc, reinterpret_cast<PROCESS_MEMORY_COUNTERS *>(&pmc),
              sizeof(pmc))) {
        auto itWs = g_aiLastWorkingSetSize.find(pid);
        if (itWs != g_aiLastWorkingSetSize.end()) {
          SIZE_T delta = (pmc.WorkingSetSize > itWs->second)
                             ? (pmc.WorkingSetSize - itWs->second)
                             : (itWs->second - pmc.WorkingSetSize);
          if (delta > 8ull * 1024 * 1024) {
            g_aiLastInferenceTime[pid] = now;
          }
        }
        g_aiLastWorkingSetSize[pid] = pmc.WorkingSetSize;
      }
    }

    // AI Engine Sanctuary: Never throttle local AI during token generation.
    if (isAi) {
      auto itAi = g_aiLastInferenceTime.find(pid);
      bool isGenerating =
          (itAi != g_aiLastInferenceTime.end()) &&
          (std::chrono::duration_cast<std::chrono::seconds>(now - itAi->second)
               .count() < 10);
      if (isGenerating) {
        if (isThrottled) {
          DWORD original = g_throttledProcesses[pid];
          g_throttledProcesses.erase(pid);
          SetPriorityClass(hProc, original);
          SetProcessIoPriorityHint(hProc, IoPriorityNormal);
          SetProcessEcoQoS(hProc, false);
        }
        CloseHandle(hProc);
        continue;
      }
    }

    DWORD numCores = GetSystemCoreCount();
    bool isCpuHeavy =
        (cpuPercent >= settings.backgroundCpuThrottleThresholdPercent) ||
        (cpuPercent * numCores >= 85.0);

    bool isCpuCalm =
        (cpuPercent >= 0) &&
        (cpuPercent < settings.backgroundCpuThrottleThresholdPercent / 2.0) &&
        (cpuPercent * numCores < 40.0);

    if (isCpuHeavy && !isThrottled) {
      DWORD prevPriority = GetPriorityClass(hProc);
      if (PriorityClassToRank(prevPriority) >
          PriorityClassToRank(IDLE_PRIORITY_CLASS)) {
        DWORD targetThrottlePrio =
            isMultiTasking ? BELOW_NORMAL_PRIORITY_CLASS : IDLE_PRIORITY_CLASS;

        if (SetPriorityClass(hProc, targetThrottlePrio)) {
          SetProcessIoPriorityHint(hProc, IoPriorityLow);
          SetProcessEcoQoS(hProc, /*enableThrottling=*/true);
          g_throttledProcesses[pid] = prevPriority;
          g_throttleStaleSampleCount.erase(pid);
        }
      }
    } else if (isThrottled && isCpuCalm) {
      DWORD original = g_throttledProcesses[pid];
      g_throttledProcesses.erase(pid);
      g_throttleStaleSampleCount.erase(pid);
      SetPriorityClass(hProc, original);
      SetProcessIoPriorityHint(hProc, IoPriorityNormal);
      SetProcessEcoQoS(hProc, /*enableThrottling=*/false);
    } else if (isThrottled && cpuPercent < 0) {
      // No CPU sample this cycle; after a few consecutive misses restore
      // the process instead of leaving it throttled forever.
      DWORD &missCount = g_throttleStaleSampleCount[pid];
      if (++missCount >= 5) {
        DWORD original = g_throttledProcesses[pid];
        g_throttledProcesses.erase(pid);
        g_throttleStaleSampleCount.erase(pid);
        SetPriorityClass(hProc, original);
        SetProcessIoPriorityHint(hProc, IoPriorityNormal);
        SetProcessEcoQoS(hProc, /*enableThrottling=*/false);
      }
    } else if (isThrottled) {
      g_throttleStaleSampleCount.erase(pid);
    }

    CloseHandle(hProc);
  }

  for (auto it = g_throttledProcesses.begin();
       it != g_throttledProcesses.end();) {
    it = (alivePids.count(it->first) == 0) ? g_throttledProcesses.erase(it)
                                           : std::next(it);
  }
  for (auto it = g_cpuSamples.begin(); it != g_cpuSamples.end();) {
    it = (alivePids.count(it->first) == 0) ? g_cpuSamples.erase(it)
                                           : std::next(it);
  }
  for (auto it = g_aiLastInferenceTime.begin();
       it != g_aiLastInferenceTime.end();) {
    it = (alivePids.count(it->first) == 0) ? g_aiLastInferenceTime.erase(it)
                                           : std::next(it);
  }
  for (auto it = g_aiLastWorkingSetSize.begin();
       it != g_aiLastWorkingSetSize.end();) {
    it = (alivePids.count(it->first) == 0) ? g_aiLastWorkingSetSize.erase(it)
                                           : std::next(it);
  }
  for (auto it = g_throttleStaleSampleCount.begin();
       it != g_throttleStaleSampleCount.end();) {
    it = (alivePids.count(it->first) == 0)
             ? g_throttleStaleSampleCount.erase(it)
             : std::next(it);
  }
}

static void RestoreAllThrottledProcesses() {
  for (auto &kv : g_throttledProcesses) {
    HANDLE h = OpenProcess(PROCESS_SET_INFORMATION, FALSE, kv.first);
    if (h) {
      SetPriorityClass(h, kv.second);
      CloseHandle(h);
    }
  }
  g_throttledProcesses.clear();
  g_throttleStaleSampleCount.clear();
}

// ---------------------------------------------------------------------------
// Window State Map
// ---------------------------------------------------------------------------

struct WindowState {
  bool hasVisibleWindow = false;
  bool isMinimized = false;
};

static BOOL CALLBACK EnumWindowStateProc(HWND hwnd, LPARAM lParam) {
  auto *map = reinterpret_cast<std::map<DWORD, WindowState> *>(lParam);
  if (!IsWindowVisible(hwnd))
    return TRUE;
  DWORD pid = 0;
  GetWindowThreadProcessId(hwnd, &pid);
  auto &state = (*map)[pid];
  state.hasVisibleWindow = true;
  if (IsIconic(hwnd)) {
    state.isMinimized = true;
  }
  return TRUE;
}

static std::map<DWORD, WindowState> BuildWindowStateMap() {
  std::map<DWORD, WindowState> map;
  EnumWindows(EnumWindowStateProc, reinterpret_cast<LPARAM>(&map));
  return map;
}

// ---------------------------------------------------------------------------
// Fullscreen / Direct3D Game Detection
// ---------------------------------------------------------------------------

static bool IsLikelyGameOrFullscreenWindow(HWND hwnd) {
  if (!hwnd || !IsWindowVisible(hwnd))
    return false;

  if (g_pfnSHQueryUserNotificationState) {
    QUERY_USER_NOTIFICATION_STATE quns = QUNS_NOT_PRESENT;
    if (SUCCEEDED(g_pfnSHQueryUserNotificationState(&quns))) {
      if (quns == QUNS_RUNNING_D3D_FULL_SCREEN ||
          quns == QUNS_PRESENTATION_MODE) {
        return true;
      }
    }
  }

  RECT wndRect;
  if (!GetWindowRect(hwnd, &wndRect))
    return false;

  HMONITOR hMon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
  MONITORINFO mi{};
  mi.cbSize = sizeof(mi);
  if (!GetMonitorInfoW(hMon, &mi))
    return false;

  bool coversMonitor =
      (wndRect.left <= mi.rcMonitor.left && wndRect.top <= mi.rcMonitor.top &&
       wndRect.right >= mi.rcMonitor.right &&
       wndRect.bottom >= mi.rcMonitor.bottom);
  if (!coversMonitor)
    return false;

  LONG style = GetWindowLongW(hwnd, GWL_STYLE);
  bool borderless = (style & WS_CAPTION) == 0 && (style & WS_THICKFRAME) == 0;

  return borderless || (style & WS_POPUP) != 0;
}

static void MaybeRequestGameSweep(DWORD pid, HWND hwnd,
                                  const ModSettings &settings) {
  if (!settings.enableGameModeDetection || pid == 0 ||
      pid == g_lastGameSweepPid)
    return;
  if (!IsLikelyGameOrFullscreenWindow(hwnd))
    return;

  auto now = std::chrono::steady_clock::now();
  auto elapsedMin = std::chrono::duration_cast<std::chrono::minutes>(
                        now - g_lastGameSweepTime)
                        .count();
  if (elapsedMin < 1)
    return;

  MEMORYSTATUSEX mem{};
  mem.dwLength = sizeof(mem);
  if (GlobalMemoryStatusEx(&mem)) {
    double freeRamPercent =
        (static_cast<double>(mem.ullAvailPhys) / mem.ullTotalPhys) * 100.0;
    if (freeRamPercent > 25.0) {
      return;
    }
  }

  g_lastGameSweepPid = pid;
  g_lastGameSweepTime = now;

  g_gameSweepRequested.store(true);
  if (g_wakeEvent) {
    SetEvent(g_wakeEvent);
  }
}

// ---------------------------------------------------------------------------
// Working-Set Trimming with SSD Cooldown, Audio Immunity & AI Idle Offload
// ---------------------------------------------------------------------------

struct AppTrimEntry {
  std::wstring procName;
  DWORD pid = 0;
  SIZE_T bytesFreed = 0;
  SIZE_T beforeBytes = 0;
  SIZE_T afterBytes = 0;
};

struct TrimStats {
  DWORD processesTrimmed = 0;
  DWORD processesSkippedRecent = 0;
  SIZE_T bytesReclaimed = 0;
  std::vector<AppTrimEntry> topApps;
};

struct TrimAttemptResult {
  bool trimmed = false;
  SIZE_T freedBytes = 0;
  SIZE_T beforeBytes = 0;
  SIZE_T afterBytes = 0;
};

static TrimAttemptResult
TryTrimProcess(DWORD pid, const ModSettings &settings,
               std::chrono::steady_clock::time_point now,
               bool emergency = false) {
  TrimAttemptResult result;

  // SSD protection: 180s cooldown between trims of the same process,
  // shortened to 45s in an emergency (better than risking an OOM).
  int cooldownSec = emergency ? 45 : 180;
  auto itTrim = g_processLastTrimmed.find(pid);
  if (itTrim != g_processLastTrimmed.end()) {
    auto diffSec =
        std::chrono::duration_cast<std::chrono::seconds>(now - itTrim->second)
            .count();
    if (diffSec < cooldownSec)
      return result;
  }

  HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION |
                                 PROCESS_SET_QUOTA | PROCESS_VM_READ,
                             FALSE, pid);
  if (!hProc) {
    hProc = OpenProcess(PROCESS_SET_QUOTA | PROCESS_QUERY_LIMITED_INFORMATION,
                        FALSE, pid);
  }
  if (!hProc)
    return result;

  PROCESS_MEMORY_COUNTERS_EX pmc;
  ZeroMemory(&pmc, sizeof(pmc));
  pmc.cb = sizeof(pmc);

  if (GetProcessMemoryInfo(hProc,
                           reinterpret_cast<PROCESS_MEMORY_COUNTERS *>(&pmc),
                           sizeof(pmc))) {
    SIZE_T wsMb = pmc.WorkingSetSize / (1024 * 1024);

    if (wsMb >= settings.minProcessMemoryToTrimMb) {
      SIZE_T beforeBytes = pmc.WorkingSetSize;

      if (SetProcessWorkingSetSize(hProc, static_cast<SIZE_T>(-1),
                                   static_cast<SIZE_T>(-1))) {
        g_processLastTrimmed[pid] = now;

        PROCESS_MEMORY_COUNTERS_EX afterPmc;
        ZeroMemory(&afterPmc, sizeof(afterPmc));
        afterPmc.cb = sizeof(afterPmc);
        if (GetProcessMemoryInfo(
                hProc, reinterpret_cast<PROCESS_MEMORY_COUNTERS *>(&afterPmc),
                sizeof(afterPmc))) {
          if (beforeBytes > afterPmc.WorkingSetSize) {
            result.trimmed = true;
            result.beforeBytes = beforeBytes;
            result.afterBytes = afterPmc.WorkingSetSize;
            result.freedBytes = beforeBytes - afterPmc.WorkingSetSize;
          }
        }
      }
    }
  }

  CloseHandle(hProc);
  return result;
}

static TrimStats TrimBackgroundWorkingSets(const ModSettings &settings,
                                           DWORD foregroundPid,
                                           double freeRamPercent,
                                           bool systemIdle,
                                           bool emergency = false) {
  TrimStats stats;
  auto now = std::chrono::steady_clock::now();

  bool isMultiTasking =
      settings.enableMultitaskingAdaptation && IsActiveMultiTaskingMode();

  int effectiveGraceMinutes = settings.recentActivityGraceMinutes;
  if (systemIdle) {
    effectiveGraceMinutes = 0;
  } else if (isMultiTasking) {
    effectiveGraceMinutes *= 4;
  } else if (freeRamPercent >= 30.0) {
    effectiveGraceMinutes *= 2;
  }
  int64_t graceSeconds = static_cast<int64_t>(effectiveGraceMinutes) * 60;

  std::unordered_set<DWORD> rawAudioPids;
  if (settings.enableAudioShielding) {
    rawAudioPids = GetActiveAudioProcessIdsCached();
  }

  DWORD currentPid = GetCurrentProcessId();
  std::vector<ProcessSnapshotEntry> processList =
      CaptureProcessSnapshotCached();
  std::unordered_set<DWORD> alivePids;
  for (const auto &entry : processList) {
    alivePids.insert(entry.pid);
  }

  std::map<DWORD, std::vector<DWORD>> childrenOf;
  std::map<DWORD, const ProcessSnapshotEntry *> byPid;
  for (const auto &entry : processList) {
    childrenOf[entry.parentPid].push_back(entry.pid);
    byPid[entry.pid] = &entry;
  }

  // Expand audio shielding to the whole process tree/family of active media
  // players (Zen, Chrome, Spotify, etc.)
  std::unordered_set<DWORD> activeAudioPids = rawAudioPids;
  for (DWORD aPid : rawAudioPids) {
    auto itEntry = byPid.find(aPid);
    if (itEntry != byPid.end()) {
      const std::wstring &aName = itEntry->second->name;
      for (const auto &entry : processList) {
        if (entry.name == aName) {
          activeAudioPids.insert(entry.pid);
        }
      }
    }
  }

  std::map<DWORD, WindowState> windowStates = BuildWindowStateMap();

  std::vector<AppTrimEntry> trimmedEntries;
  std::unordered_set<DWORD> handledPids;

  auto tryTrimAndRecord = [&](DWORD pid, const std::wstring &name) {
    TrimAttemptResult attempt = TryTrimProcess(pid, settings, now, emergency);
    if (attempt.trimmed) {
      stats.processesTrimmed++;
      stats.bytesReclaimed += attempt.freedBytes;
      AppTrimEntry appEntry;
      appEntry.procName = name;
      appEntry.pid = pid;
      appEntry.bytesFreed = attempt.freedBytes;
      appEntry.beforeBytes = attempt.beforeBytes;
      appEntry.afterBytes = attempt.afterBytes;
      trimmedEntries.push_back(std::move(appEntry));
    }
  };

  for (const auto &entry : processList) {
    DWORD pid = entry.pid;
    if (pid == 0 || pid == 4 || pid == currentPid)
      continue;
    if (handledPids.count(pid))
      continue;

    if (foregroundPid != 0 && pid == foregroundPid) {
      continue;
    }

    if (settings.enableAudioShielding && activeAudioPids.count(pid)) {
      continue;
    }

    const std::wstring &procName = entry.name;

    if (IsInList(procName, settings.excludedProcesses))
      continue;

    if (!IsInteractiveSessionProcess(pid))
      continue;

    // Smart AI Engine Inactivity & Inference Shield
    bool isAi =
        settings.enableSmartAiOptimization && IsKnownAiProcess(procName);
    if (isAi) {
      auto itAi = g_aiLastInferenceTime.find(pid);
      if (itAi != g_aiLastInferenceTime.end()) {
        auto inactiveAiSec =
            std::chrono::duration_cast<std::chrono::seconds>(now - itAi->second)
                .count();
        if (inactiveAiSec <
            static_cast<int64_t>(settings.aiInactivityGraceMinutes) * 60) {
          // AI model generated tokens recently: keep memory 100% warm.
          continue;
        }
      }
    }

    bool isTargetListed = IsInList(procName, settings.customTargetList);
    if (settings.targetProcessesOnly && !isTargetListed && !isAi)
      continue;

    auto wsIt = windowStates.find(pid);
    bool hasVisibleWindow =
        wsIt != windowStates.end() && wsIt->second.hasVisibleWindow;
    bool isMinimized = settings.trimMinimizedWindows &&
                       wsIt != windowStates.end() && wsIt->second.isMinimized;

    if (isMultiTasking && hasVisibleWindow && !isMinimized && !isAi) {
      continue;
    }

    bool graceEligible = true;
    if (!isMinimized && settings.enableProcessAging && !isAi) {
      std::lock_guard<std::mutex> lock(g_focusMapMutex);
      auto itFocus = g_processLastFocusedTime.find(pid);
      if (itFocus != g_processLastFocusedTime.end()) {
        auto inactiveSeconds = std::chrono::duration_cast<std::chrono::seconds>(
                                   now - itFocus->second)
                                   .count();
        if (inactiveSeconds < graceSeconds) {
          graceEligible = false;
        }
      }
    }

    bool capEligible = false;
    if (settings.enableElectronMemoryCap && isTargetListed &&
        pid != foregroundPid) {
      HANDLE hPeek = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
      if (hPeek) {
        PROCESS_MEMORY_COUNTERS_EX pmc;
        ZeroMemory(&pmc, sizeof(pmc));
        pmc.cb = sizeof(pmc);
        if (GetProcessMemoryInfo(
                hPeek, reinterpret_cast<PROCESS_MEMORY_COUNTERS *>(&pmc),
                sizeof(pmc))) {
          SIZE_T wsMb = pmc.WorkingSetSize / (1024 * 1024);
          if (wsMb >= static_cast<SIZE_T>(settings.electronMemoryCapMb)) {
            capEligible = true;
          }
        }
        CloseHandle(hPeek);
      }
    }

    if (!graceEligible && !capEligible && !isAi) {
      stats.processesSkippedRecent++;
      continue;
    }

    handledPids.insert(pid);
    tryTrimAndRecord(pid, procName);

    if (settings.enableProcessTreeTrimming && hasVisibleWindow) {
      std::vector<DWORD> descendants;
      std::unordered_set<DWORD> visited;
      CollectDescendants(pid, childrenOf, descendants, visited);
      for (DWORD childPid : descendants) {
        if (handledPids.count(childPid))
          continue;
        if (settings.enableAudioShielding && activeAudioPids.count(childPid))
          continue;
        auto childIt = byPid.find(childPid);
        if (childIt == byPid.end())
          continue;
        const std::wstring &childName = childIt->second->name;
        if (IsInList(childName, settings.excludedProcesses))
          continue;
        handledPids.insert(childPid);
        tryTrimAndRecord(childPid, childName);
      }
    }
  }

  std::sort(trimmedEntries.begin(), trimmedEntries.end(),
            [](const AppTrimEntry &a, const AppTrimEntry &b) {
              return a.bytesFreed > b.bytesFreed;
            });

  size_t count =
      (std::min)(static_cast<size_t>(kLogTopAppsCount), trimmedEntries.size());
  stats.topApps.assign(trimmedEntries.begin(), trimmedEntries.begin() + count);

  {
    std::lock_guard<std::mutex> lock(g_focusMapMutex);
    for (auto it = g_processLastFocusedTime.begin();
         it != g_processLastFocusedTime.end();) {
      it = (alivePids.count(it->first) == 0)
               ? g_processLastFocusedTime.erase(it)
               : std::next(it);
    }
  }
  for (auto it = g_processLastTrimmed.begin();
       it != g_processLastTrimmed.end();) {
    it = (alivePids.count(it->first) == 0) ? g_processLastTrimmed.erase(it)
                                           : std::next(it);
  }

  return stats;
}

// ---------------------------------------------------------------------------
// Cleanup Pass Dispatcher
// ---------------------------------------------------------------------------

static void PerformMemoryCleanup(const wchar_t *triggerReason,
                                 bool allowWorkingSetTrim = true) {
  ModSettings settings = GetSettingsSnapshot();

  MEMORYSTATUSEX memBefore;
  memBefore.dwLength = sizeof(memBefore);
  GlobalMemoryStatusEx(&memBefore);

  double freeRamPercent =
      (static_cast<double>(memBefore.ullAvailPhys) / memBefore.ullTotalPhys) *
      100.0;
  DWORD fgPid = GetForegroundProcessId();

  bool systemIdle = settings.enableIdleBoost &&
                    (GetSystemIdleSeconds() >=
                     static_cast<DWORD>(settings.idleThresholdMinutes) * 60);

  // Below 5% free RAM, relax the anti-thrashing cooldowns so the mod can
  // react before the system runs out of memory.
  bool emergency = freeRamPercent <= 5.0;

  TrimStats trimStats;
  if (settings.cleanBackgroundWorkingSets && allowWorkingSetTrim) {
    trimStats = TrimBackgroundWorkingSets(settings, fgPid, freeRamPercent,
                                          systemIdle, emergency);
  }

  MEMORYSTATUSEX memAfter;
  memAfter.dwLength = sizeof(memAfter);
  GlobalMemoryStatusEx(&memAfter);

  DWORDLONG freedTotalBytes =
      (memAfter.ullAvailPhys > memBefore.ullAvailPhys)
          ? (memAfter.ullAvailPhys - memBefore.ullAvailPhys)
          : trimStats.bytesReclaimed;

  g_sessionCleanupPasses++;
  g_sessionBytesReclaimed += freedTotalBytes;
  g_sessionProcessesTrimmedTotal += trimStats.processesTrimmed;

  if (settings.enableLogging) {
    double freedMb = freedTotalBytes / (1024.0 * 1024.0);
    double availAfterGb = memAfter.ullAvailPhys / (1024.0 * 1024.0 * 1024.0);
    double totalGb = memBefore.ullTotalPhys / (1024.0 * 1024.0 * 1024.0);
    double sessionGb = g_sessionBytesReclaimed / (1024.0 * 1024.0 * 1024.0);
    std::wstring uptime =
        FormatUptime(g_modStartTime, std::chrono::steady_clock::now());

    Wh_Log(L"[SmartOptimizer] %s -> +%.1f MB freed (Avail: %.2f/%.1f GB | %u "
           L"trimmed, %u kept warm) | "
           L"Session: %.2f GB over %u passes (%u procs), %s uptime",
           triggerReason, freedMb, availAfterGb, totalGb,
           trimStats.processesTrimmed, trimStats.processesSkippedRecent,
           sessionGb, g_sessionCleanupPasses, g_sessionProcessesTrimmedTotal,
           uptime.c_str());

    bool gainWorthLogging =
        freedTotalBytes >=
        static_cast<DWORDLONG>(kTopAppsLogThresholdMb) * 1024 * 1024;
    if (!trimStats.topApps.empty() && gainWorthLogging) {
      Wh_Log(L"[SmartOptimizer] --- Top Reclaimed Memory Hogs ---");
      int rank = 1;
      for (const auto &app : trimStats.topApps) {
        double appFreedMb = app.bytesFreed / (1024.0 * 1024.0);
        double beforeMb = app.beforeBytes / (1024.0 * 1024.0);
        double afterMb = app.afterBytes / (1024.0 * 1024.0);
        Wh_Log(L"[SmartOptimizer]   #%d. %s (PID %u): -%.1f MB (was %.1f MB -> now "
               L"%.1f MB)",
               rank++, app.procName.c_str(), app.pid, appFreedMb, beforeMb,
               afterMb);
      }
    }
  }
}

// ---------------------------------------------------------------------------
// Event Hook Thread (Instant Foreground Detection + Panic Hotkey)
// ---------------------------------------------------------------------------

static void HandleForegroundChanged(HWND hwnd) {
  DWORD pid = 0;
  GetWindowThreadProcessId(hwnd, &pid);
  if (pid == 0)
    return;

  ModSettings settings = GetSettingsSnapshot();

  RecordFocusSwitch(pid);

  UpdateForegroundBoost(pid, settings);

  MaybeRequestGameSweep(pid, hwnd, settings);
}

static void CALLBACK WinEventProc(HWINEVENTHOOK /*hook*/, DWORD event,
                                  HWND hwnd, LONG idObject, LONG /*idChild*/,
                                  DWORD /*idEventThread*/,
                                  DWORD /*dwmsEventTime*/) {
  if (event != EVENT_SYSTEM_FOREGROUND || !hwnd || idObject != OBJID_WINDOW)
    return;
  HandleForegroundChanged(hwnd);
}

static DWORD WINAPI HookThreadProc(LPVOID) {
  g_winEventHook = SetWinEventHook(
      EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr, WinEventProc,
      0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);

  {
    ModSettings settings = GetSettingsSnapshot();
    if (settings.enablePanicHotkey) {
      g_panicHotkeyRegistered =
          RegisterHotKey(nullptr, kPanicHotkeyId,
                         MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, VK_F11);
    }
  }

  MSG msg;
  while (g_hookThreadRunning.load()) {
    BOOL result = GetMessageW(&msg, nullptr, 0, 0);
    if (result <= 0)
      break;

    if (msg.message == WM_HOTKEY && msg.wParam == kPanicHotkeyId) {
      Wh_Log(L"[SmartOptimizer] Panic hotkey triggered (Ctrl+Alt+F11).");
      g_forceCleanupRequested.store(true);
      if (g_wakeEvent) {
        SetEvent(g_wakeEvent);
      }
    } else if (msg.message == WM_APP) {
      if (msg.wParam == 1 && !g_panicHotkeyRegistered) {
        g_panicHotkeyRegistered =
            RegisterHotKey(nullptr, kPanicHotkeyId,
                           MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, VK_F11);
      } else if (msg.wParam == 0 && g_panicHotkeyRegistered) {
        UnregisterHotKey(nullptr, kPanicHotkeyId);
        g_panicHotkeyRegistered = false;
      }
    }

    TranslateMessage(&msg);
    DispatchMessageW(&msg);
  }

  if (g_panicHotkeyRegistered) {
    UnregisterHotKey(nullptr, kPanicHotkeyId);
    g_panicHotkeyRegistered = false;
  }
  if (g_winEventHook) {
    UnhookWinEvent(g_winEventHook);
    g_winEventHook = nullptr;
  }
  return 0;
}

// ---------------------------------------------------------------------------
// Watchdog Worker Thread
// ---------------------------------------------------------------------------

static void MemoryOptimizerWorker() {
  HRESULT hrCom = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

  Wh_Log(
      L"[SmartOptimizer] Adaptive Memory & Priority Optimizer engine v1.0.0 started "
      L"(0.00%% CPU passive wait).");

  HANDLE waitHandles[2] = {g_stopEvent, g_wakeEvent};
  g_modStartTime = std::chrono::steady_clock::now();
  g_lastPeriodicCleanTime = g_modStartTime;
  g_lastIdleCleanTime = g_modStartTime;

  PerformMemoryCleanup(L"[Startup Optimization]",
                       /*allowWorkingSetTrim=*/false);

  while (g_workerRunning.load()) {
    ModSettings settings = GetSettingsSnapshot();

    if (g_forceCleanupRequested.exchange(false)) {
      PerformMemoryCleanup(L"[Panic Hotkey Trigger]");
    }
    if (g_gameSweepRequested.exchange(false)) {
      Wh_Log(L"[SmartOptimizer] Fullscreen / 3D game detected. Running "
             L"preventive RAM sweep...");
      PerformMemoryCleanup(L"[Pre-Game Sweep]");
    }

    bool hookActive = (g_winEventHook != nullptr);
    if (!hookActive) {
      DWORD fgPid = GetForegroundProcessId();
      if (fgPid != 0) {
        RecordFocusSwitch(fgPid);
        UpdateForegroundBoost(fgPid, settings);
      }
    }

    ApplyBackgroundThrottling(settings, GetCurrentlyBoostedPid());

    bool onBattery = settings.pauseOnBattery && IsRunningOnBattery();

    if (!onBattery) {
      MEMORYSTATUSEX mem;
      mem.dwLength = sizeof(mem);
      if (GlobalMemoryStatusEx(&mem)) {
        double freePercent =
            (static_cast<double>(mem.ullAvailPhys) / mem.ullTotalPhys) * 100.0;
        auto now = std::chrono::steady_clock::now();

        bool shouldClean = false;
        std::wstring reason;

        auto elapsedSinceTrigger =
            std::chrono::duration_cast<std::chrono::seconds>(
                now - g_lastTriggerCleanTime)
                .count();
        bool triggerCooldownElapsed =
            elapsedSinceTrigger >= kTriggerCooldownSec;

        // 1. Smart threshold trigger (low free RAM).
        if (settings.cleanMode == CleanMode::SmartThreshold ||
            settings.cleanMode == CleanMode::SmartAndPeriodic) {
          if (freePercent <= settings.freeRamThresholdPercent &&
              triggerCooldownElapsed) {
            shouldClean = true;
            wchar_t buf[128];
            swprintf_s(buf, L"[Threshold Trigger: Free RAM %.1f%% <= %d%%]",
                       freePercent, settings.freeRamThresholdPercent);
            reason = buf;
            g_lastTriggerCleanTime = now;
          }
        }

        // 2. Periodic timer trigger.
        if (!shouldClean &&
            (settings.cleanMode == CleanMode::Periodic ||
             settings.cleanMode == CleanMode::SmartAndPeriodic)) {
          auto elapsedMinutes =
              std::chrono::duration_cast<std::chrono::minutes>(
                  now - g_lastPeriodicCleanTime)
                  .count();
          if (elapsedMinutes >= settings.periodicIntervalMinutes) {
            shouldClean = true;
            wchar_t buf[128];
            swprintf_s(buf, L"[Periodic Trigger: %d min interval]",
                       settings.periodicIntervalMinutes);
            reason = buf;
            g_lastPeriodicCleanTime = now;
          }
        }

        // 3. Smart Idle Trigger: triggers once upon entering idle state,
        // then throttles subsequent sweeps to a relaxed 10-15 minute cadence
        // (preventing 30s log spam).
        DWORD idleSec = GetSystemIdleSeconds();
        DWORD idleThresholdSec =
            static_cast<DWORD>(settings.idleThresholdMinutes) * 60;
        bool isSystemIdle = (idleSec >= idleThresholdSec);

        if (!shouldClean && settings.enableIdleBoost && isSystemIdle &&
            triggerCooldownElapsed) {
          auto elapsedSinceIdleClean =
              std::chrono::duration_cast<std::chrono::minutes>(
                  now - g_lastIdleCleanTime)
                  .count();
          int idleIntervalMin =
              (std::max)(10, settings.periodicIntervalMinutes);
          if (!g_wasIdle || elapsedSinceIdleClean >= idleIntervalMin) {
            shouldClean = true;
            wchar_t buf[128];
            swprintf_s(buf, L"[Idle Trigger: idle for %u min]", idleSec / 60);
            reason = buf;
            g_lastIdleCleanTime = now;
            g_lastTriggerCleanTime = now;
          }
        }

        if (!isSystemIdle) {
          g_wasIdle = false;
        } else if (shouldClean) {
          g_wasIdle = true;
        }

        if (shouldClean) {
          PerformMemoryCleanup(reason.c_str());
        }
      }
    }

    DWORD waitSec = (DWORD)std::clamp(settings.checkIntervalSec, 1, 30);
    DWORD waitRes =
        WaitForMultipleObjects(2, waitHandles, FALSE, waitSec * 1000);

    if (waitRes == WAIT_OBJECT_0) {
      break;
    } else if (waitRes == WAIT_OBJECT_0 + 1) {
      ResetEvent(g_wakeEvent);
    }
  }

  {
    std::lock_guard<std::mutex> lock(g_probalanceMutex);
    if (g_currentBoostedPid != 0) {
      HANDLE hOld = OpenProcess(PROCESS_SET_INFORMATION |
                                    PROCESS_QUERY_LIMITED_INFORMATION,
                                FALSE, g_currentBoostedPid);
      if (hOld) {
        SetPriorityClass(hOld, g_originalBoostedPriority);
        CloseHandle(hOld);
      }
      g_currentBoostedPid = 0;
    }
  }

  RestoreAllThrottledProcesses();

  Wh_Log(L"[SmartOptimizer] Memory & Priority Optimizer engine stopped cleanly.");

  if (hrCom == S_OK || hrCom == S_FALSE) {
    CoUninitialize();
  }
}

// ---------------------------------------------------------------------------
// Settings Loader
// ---------------------------------------------------------------------------

static void LoadSettings() {
  std::lock_guard<std::mutex> lock(g_settingsMutex);

  g_settings.enableProBalance = Wh_GetIntSetting(L"enableProBalance") != 0;

  PCWSTR prioStr = Wh_GetStringSetting(L"foregroundPriorityLevel");
  if (prioStr && wcscmp(prioStr, L"high") == 0) {
    g_settings.foregroundPriorityLevel = ForegroundPrioritySetting::High;
  } else {
    g_settings.foregroundPriorityLevel = ForegroundPrioritySetting::AboveNormal;
  }
  Wh_FreeStringSetting(prioStr);

  g_settings.enableBackgroundThrottling =
      Wh_GetIntSetting(L"enableBackgroundThrottling") != 0;

  int bgThrottle =
      (int)Wh_GetIntSetting(L"backgroundCpuThrottleThresholdPercent");
  g_settings.backgroundCpuThrottleThresholdPercent =
      std::clamp(bgThrottle, 5, 50);

  g_settings.enableSmartAiOptimization =
      Wh_GetIntSetting(L"enableSmartAiOptimization") != 0;

  int aiGraceMin = (int)Wh_GetIntSetting(L"aiInactivityGraceMinutes");
  g_settings.aiInactivityGraceMinutes = std::clamp(aiGraceMin, 1, 30);

  g_settings.enableAudioShielding =
      Wh_GetIntSetting(L"enableAudioShielding") != 0;
  g_settings.enableMultitaskingAdaptation =
      Wh_GetIntSetting(L"enableMultitaskingAdaptation") != 0;
  g_settings.enableGameModeDetection =
      Wh_GetIntSetting(L"enableGameModeDetection") != 0;

  PCWSTR modeStr = Wh_GetStringSetting(L"cleanMode");
  if (modeStr && wcscmp(modeStr, L"smartThreshold") == 0) {
    g_settings.cleanMode = CleanMode::SmartThreshold;
  } else if (modeStr && wcscmp(modeStr, L"periodic") == 0) {
    g_settings.cleanMode = CleanMode::Periodic;
  } else {
    g_settings.cleanMode = CleanMode::SmartAndPeriodic;
  }
  Wh_FreeStringSetting(modeStr);

  int thresh = (int)Wh_GetIntSetting(L"freeRamThresholdPercent");
  g_settings.freeRamThresholdPercent = std::clamp(thresh, 5, 50);

  g_settings.enableIdleBoost = Wh_GetIntSetting(L"enableIdleBoost") != 0;

  int idleMin = (int)Wh_GetIntSetting(L"idleThresholdMinutes");
  g_settings.idleThresholdMinutes = std::clamp(idleMin, 5, 120);

  g_settings.trimMinimizedWindows =
      Wh_GetIntSetting(L"trimMinimizedWindows") != 0;
  g_settings.enableProcessAging = Wh_GetIntSetting(L"enableProcessAging") != 0;

  int graceMin = (int)Wh_GetIntSetting(L"recentActivityGraceMinutes");
  g_settings.recentActivityGraceMinutes = std::clamp(graceMin, 1, 60);

  g_settings.enableProcessTreeTrimming =
      Wh_GetIntSetting(L"enableProcessTreeTrimming") != 0;
  g_settings.enableElectronMemoryCap =
      Wh_GetIntSetting(L"enableElectronMemoryCap") != 0;

  int capMb = (int)Wh_GetIntSetting(L"electronMemoryCapMb");
  g_settings.electronMemoryCapMb = std::clamp(capMb, 100, 4000);

  g_settings.cleanBackgroundWorkingSets =
      Wh_GetIntSetting(L"cleanBackgroundWorkingSets") != 0;

  int minMem = (int)Wh_GetIntSetting(L"minProcessMemoryToTrimMb");
  g_settings.minProcessMemoryToTrimMb = (DWORD)std::clamp(minMem, 10, 1000);

  int periodicMin = (int)Wh_GetIntSetting(L"periodicIntervalMinutes");
  g_settings.periodicIntervalMinutes = std::clamp(periodicMin, 1, 60);

  g_settings.targetProcessesOnly =
      Wh_GetIntSetting(L"targetProcessesOnly") != 0;

  PCWSTR customListStr = Wh_GetStringSetting(L"customTargetList");
  g_settings.customTargetList =
      ParseProcessList(customListStr ? customListStr : L"");
  Wh_FreeStringSetting(customListStr);

  PCWSTR exclListStr = Wh_GetStringSetting(L"excludedProcesses");
  g_settings.excludedProcesses =
      ParseProcessList(exclListStr ? exclListStr : L"");
  Wh_FreeStringSetting(exclListStr);

  g_settings.enablePanicHotkey = Wh_GetIntSetting(L"enablePanicHotkey") != 0;

  g_settings.pauseOnBattery = Wh_GetIntSetting(L"pauseOnBattery") != 0;

  int checkSec = (int)Wh_GetIntSetting(L"checkIntervalSec");
  g_settings.checkIntervalSec = std::clamp(checkSec, 1, 30);

  g_settings.enableLogging = Wh_GetIntSetting(L"enableLogging") != 0;
}

// ---------------------------------------------------------------------------
// Mod Lifecycle Entry Points
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
  if (!IsMainShellProcess()) {
    return TRUE;
  }

  Wh_Log(L"[SmartOptimizer] Initializing Smart Process Priority & RAM Optimizer "
         L"v1.0.0...");

  HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
  if (hNtdll) {
    g_pfnNtSetInformationProcess = (pfnNtSetInformationProcess)GetProcAddress(
        hNtdll, "NtSetInformationProcess");
  }

  HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
  if (hShell32) {
    g_pfnSHQueryUserNotificationState =
        (pfnSHQueryUserNotificationState)GetProcAddress(
            hShell32, "SHQueryUserNotificationState");
  }

  LoadSettings();

  g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
  g_wakeEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);

  if (!g_stopEvent || !g_wakeEvent) {
    Wh_Log(L"[SmartOptimizer] Fatal Error: Failed to create synchronization events.");
    return FALSE;
  }

  g_hookThreadRunning.store(true);
  DWORD hookThreadId = 0;
  g_hookThreadHandle =
      CreateThread(nullptr, 0, HookThreadProc, nullptr, 0, &hookThreadId);
  g_hookThreadId = hookThreadId;
  if (!g_hookThreadHandle) {
    Wh_Log(L"[SmartOptimizer] Warning: Failed to start event-hook thread; "
           L"falling back to polling-only detection.");
    g_hookThreadRunning.store(false);
  }

  g_workerRunning.store(true);
  g_workerThread = std::thread(MemoryOptimizerWorker);

  Wh_Log(L"[SmartOptimizer] Mod v1.0.0 initialized successfully.");
  return TRUE;
}

void Wh_ModSettingsChanged() {
  if (!IsMainShellProcess())
    return;

  Wh_Log(L"[SmartOptimizer] Settings updated. Reloading configuration...");
  LoadSettings();

  ModSettings settings = GetSettingsSnapshot();
  if (g_hookThreadRunning.load() && g_hookThreadId != 0) {
    if (settings.enablePanicHotkey && !g_panicHotkeyRegistered) {
      PostThreadMessageW(g_hookThreadId, WM_APP, 1, 0);
    } else if (!settings.enablePanicHotkey && g_panicHotkeyRegistered) {
      PostThreadMessageW(g_hookThreadId, WM_APP, 0, 0);
    }
  }

  if (g_wakeEvent) {
    SetEvent(g_wakeEvent);
  }
}

void Wh_ModUninit() {
  if (!IsMainShellProcess())
    return;

  Wh_Log(L"[SmartOptimizer] Deinitializing mod v1.0.0...");

  if (g_hookThreadRunning.load()) {
    g_hookThreadRunning.store(false);
    if (g_hookThreadId != 0) {
      PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
    }
    if (g_hookThreadHandle) {
      WaitForSingleObject(g_hookThreadHandle, 3000);
      CloseHandle(g_hookThreadHandle);
      g_hookThreadHandle = nullptr;
    }
  }

  if (g_workerRunning.load()) {
    g_workerRunning.store(false);
    if (g_stopEvent) {
      SetEvent(g_stopEvent);
    }
    if (g_workerThread.joinable()) {
      g_workerThread.join();
    }
  }

  if (g_stopEvent) {
    CloseHandle(g_stopEvent);
    g_stopEvent = nullptr;
  }
  if (g_wakeEvent) {
    CloseHandle(g_wakeEvent);
    g_wakeEvent = nullptr;
  }

  Wh_Log(L"[SmartOptimizer] Mod unloaded cleanly.");
}
