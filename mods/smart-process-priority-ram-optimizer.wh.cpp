// ==WindhawkMod==
// @id              smart-process-priority-ram-optimizer
// @name            Smart Process Priority & RAM Optimizer
// @description     Boosts foreground responsiveness, shields audio and AI workloads, throttles runaway background CPU, and safely reclaims idle memory.
// @version         2.0.0
// @author          gilnett
// @github          https://github.com/gilnett
// @include         windhawk.exe
// @compilerOptions -lpsapi -lole32 -lshell32 -ldwmapi
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Smart Process Priority & RAM Optimizer

Boosts the responsiveness of the active foreground application, protects
real-time audio and local AI engines from throttling or trimming, throttles
CPU-heavy background tasks, and safely reclaims idle memory with
hardware-protective SSD safeguards.

## Foreground Priority Boost
Elevates the active window's entire process family to Above Normal or High CPU priority via a system event hook, ensuring maximum responsiveness, and restores original priority when focus moves away. Background tasks are throttled to low I/O priority during contention to grant the foreground application relative I/O precedence. The foreground process is never trimmed.

## Local AI Engine Handling
Recognizes common local AI inference processes (llama-server, LM Studio, Ollama,
KoboldCPP, Jan, ComfyUI, and others). While one is actively generating -
detected from CPU usage or working-set changes, so GPU-offloaded inference is
covered too - it keeps full priority and its memory is never trimmed. After a
configurable period of inactivity, its memory can be released to the Windows
standby cache and reloads almost instantly on the next prompt.

## Audio & Multimedia Shield
Monitors active WASAPI audio sessions and exempts the whole process family of an
app currently playing audio (browser, renderer, and helper processes) from CPU
throttling and memory trimming.

*Note for DAW / audio power users*: Applications utilizing exclusive ASIO drivers or virtual loopback devices that bypass the Windows standard audio engine session manager will not report audio sessions via WASAPI; such audio applications should be added to the Excluded Processes list.

## Multitasking vs Single-Task Adaptation
Detects rapid window switching and extends grace periods and protects visible
windows accordingly. Detects fullscreen/3D game windows and runs one preventive
memory sweep before the game allocates its own memory.

## Automatic Process Classification
Skips processes outside the interactive user session (background services) and
packaged UWP/MSIX apps (already managed by Windows itself), regardless of name,
on top of the manual exclusion list.

## SSD-Protective Memory Reclaim
Trims idle background processes' working sets, gated by per-process cooldowns
and free-RAM thresholds to avoid unnecessary SSD writes. Below 5% free RAM these
cooldowns are relaxed so the mod can react before the system runs out of memory.

## Panic Hotkey
Ctrl+Alt+F11 triggers an immediate cleanup pass.

## Credits & Acknowledgments
- **Inspirations & Concepts**:
  - **Process Lasso (Bitsum)**: Inspired by the ProBalance concept for
foreground responsiveness and background runaway CPU restraint.
  - **LiveTuner (LT)**: Inspired by dynamic real-time priority tuning and
Pressing Ctrl+Alt+F11 triggers an immediate memory cleanup pass.
*/
// ==/WindhawkModReadme==

// clang-format off
// ==WindhawkModSettings==
/*
- enableProBalance: true
  $name: Elevate Foreground Priority (ProBalance)
  $description: Increases the CPU priority of the active foreground window family and restrains background CPU/IO contention for maximum responsiveness.
- foregroundPriorityLevel: "aboveNormal"
  $name: Foreground Priority Level
  $description: CPU priority class assigned to the active foreground application.
  $options:
    - "aboveNormal": Above Normal (Balanced & Safe)
    - "high": High (Maximum Performance)
- enableForegroundCpuSets: false
  $name: Suggest P-Cores to Foreground Window (Experimental)
  $description: Suggests performance cores (P-cores) to the main foreground window on hybrid Intel/AMD architectures. Child renderers remain free to use efficiency cores.
- enableBackgroundThrottling: true
  $name: Throttle CPU-Heavy Background Processes
  $description: Temporarily lowers the priority of background processes that consume excessive CPU while a foreground app is active.
- backgroundCpuThrottleThresholdPercent: 15
  $name: Background CPU Threshold (%)
  $description: Percentage of CPU usage required to throttle a background process (5% to 50%).
- systemCpuContentionThresholdPercent: 60
  $name: Total System CPU Contention Threshold (%)
  $description: Overall system CPU load required before background throttling activates (0% to 95%).
- backgroundThrottlePriorityLevel: "belowNormal"
  $name: Background Throttle Priority Level
  $description: Priority class applied to throttled background processes.
  $options:
    - "belowNormal": Below Normal (Recommended)
    - "idle": Idle (Strict Throttling)
- enableEcoQosManagement: false
  $name: Windows Efficiency Mode (EcoQoS)
  $description: Applies Windows power throttling to background processes, scheduling them on efficiency cores.
- freeRamThresholdPercent: 20
  $name: Standard Free RAM Threshold (%)
  $description: Percentage of available physical memory below which background memory cleanup is triggered (5% to 50%).
- enableTieredRamThreshold: true
  $name: Multi-Tiered Memory Thresholds
  $description: Performs an early cleanup of minimized inactive heavy applications before the standard threshold is reached.
- tieredHogThresholdPercent: 40
  $name: Inactive Apps Free RAM Threshold (%)
  $description: Free RAM percentage threshold to clean minimized heavy applications inactive for over 15 minutes (10% to 80%).
- recentActivityGraceMinutes: 3
  $name: Inactivity Grace Period (Minutes)
  $description: Minimum minutes an application must remain inactive in the background before its memory can be reclaimed (1 to 60 minutes).
- trimMinimizedWindows: true
  $name: Clean Minimized Windows
  $description: Reclaims unused memory from applications minimized to the taskbar.
- enableElectronMemoryCap: true
  $name: Memory Cap for Heavy Applications
  $description: Reclaims memory from listed heavy applications once their memory usage exceeds the configured cap.
- electronMemoryCapMb: 500
  $name: Heavy Applications Memory Cap (MB)
  $description: Maximum memory threshold before an inactive listed application is cleaned (100 to 4000 MB).
- enableIdleBoost: true
  $name: Idle System Memory Optimization
  $description: Automatically reclaims unused memory when the computer is completely idle.
- enableSmartAiOptimization: true
  $name: Protect Local AI Workloads
  $description: Prevents throttling or memory trimming during active local AI model inference.
- aiInactivityGraceMinutes: 5
  $name: AI Inactivity Grace Period (Minutes)
  $description: Minutes of inactivity before dormant local AI model memory can be released (1 to 30 minutes).
- pauseOnBattery: true
  $name: Pause on Battery Power
  $description: Suspends memory cleanups and background throttling on battery to maximize laptop battery life.
- customTargetList: "zen.exe, chrome.exe, msedge.exe, brave.exe, firefox.exe, opera.exe, vivaldi.exe, discord.exe, slack.exe, teams.exe, telegram.exe, whatsapp.exe, signal.exe, skype.exe, spotify.exe, steam.exe, epicgameslauncher.exe, code.exe, obs64.exe"
  $name: Heavy Memory Hogs Process List
  $description: Comma-separated list of executable names targeted by the memory cap and tiered inactive cleanup.
- excludedProcesses: "explorer.exe, windhawk.exe, dwm.exe, csrss.exe, lsass.exe, smss.exe, services.exe, system, wininit.exe, winlogon.exe, logonui.exe, lockapp.exe, consent.exe, credentialuibroker.exe, smartscreen.exe, svchost.exe, memcompression, registry, fontdrvhost.exe, audiodg.exe, sihost.exe, taskhostw.exe, ctfmon.exe, wlanext.exe, dashost.exe, syntpenh.exe, syntphelper.exe, etdcontrol.exe, etdctrl.exe, alpspad.exe, hcontrol.exe, wireguard.exe, openvpn.exe, tailscale.exe, splwow64.exe, printfilterpipelinesvc.exe, spoolsv.exe, wudfhost.exe, devicecensus.exe"
  $name: Excluded Processes (Immunity List)
  $description: Comma-separated list of executable names that must never be throttled or trimmed.
- enablePanicHotkey: true
  $name: Emergency Clean Hotkey (Ctrl+Alt+F11)
  $description: Enables the Ctrl+Alt+F11 shortcut to trigger an immediate memory reclamation pass.
- enableLogging: true
  $name: Diagnostic Logging
  $description: Logs memory reclamation statistics and hardware profile information in Windhawk.
*/
// ==/WindhawkModSettings==
// clang-format on

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif

#include <windows.h>

#include <windhawk_utils.h>

#include <appmodel.h>
#include <audiopolicy.h>
#include <dwmapi.h>
#include <mmdeviceapi.h>
#include <psapi.h>
#include <shellapi.h>
#include <tlhelp32.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <deque>
#include <map>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>

// ---------------------------------------------------------------------------
// NTDLL & Low-Level Definitions
// ---------------------------------------------------------------------------

typedef NTSTATUS(NTAPI *pfnNtQueryInformationProcess)(
    HANDLE ProcessHandle, INT ProcessInformationClass, PVOID ProcessInformation,
    ULONG ProcessInformationLength, PULONG ReturnLength);

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
#ifndef PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION
#define PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION 0x4
#endif
static constexpr INT ProcessPowerThrottlingInfoClass = 4;

typedef BOOL(WINAPI *pfnGetSystemCpuSetInformation)(
    PSYSTEM_CPU_SET_INFORMATION Information, ULONG BufferLength,
    PULONG ReturnedLength, HANDLE Process, ULONG Flags);

typedef BOOL(WINAPI *pfnSetProcessDefaultCpuSets)(
    HANDLE Process, const ULONG *CpuSetIds, ULONG CpuSetIdCount);

static pfnNtQueryInformationProcess g_pfnNtQueryInformationProcess = nullptr;
static pfnNtSetInformationProcess g_pfnNtSetInformationProcess = nullptr;
static pfnSHQueryUserNotificationState g_pfnSHQueryUserNotificationState =
    nullptr;
static pfnGetSystemCpuSetInformation g_pfnGetSystemCpuSetInformation = nullptr;
static pfnSetProcessDefaultCpuSets g_pfnSetProcessDefaultCpuSets = nullptr;

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

struct SystemHardwareProfile {
  double totalRamGb = 0.0;
  DWORD coreCount = 0;
  bool isLowRamTier = false;   // < 12 GB (e.g. 8 GB system, needs extra margin for shared iGPU)
  bool isHighRamTier = false;  // >= 24 GB (e.g. 32 GB, 64 GB workstation)
  bool isLowCoreCount = false; // <= 4 cores (single runaway process paralyzes 25-50% CPU)
  bool isHybridCpu = false;    // Intel P/E-cores or heterogeneous CPU
  std::vector<ULONG> pCoreCpuSetIds; // IDs of performance cores (EfficiencyClass == max)
};

static SystemHardwareProfile GetHardwareProfile() {
  static const SystemHardwareProfile profile = [] {
    SystemHardwareProfile p;
    MEMORYSTATUSEX mem{};
    mem.dwLength = sizeof(mem);
    if (GlobalMemoryStatusEx(&mem)) {
      p.totalRamGb = mem.ullTotalPhys / (1024.0 * 1024.0 * 1024.0);
    }
    p.coreCount = GetSystemCoreCount();
    p.isLowRamTier = (p.totalRamGb > 0.0 && p.totalRamGb < 12.0);
    p.isHighRamTier = (p.totalRamGb >= 24.0);
    p.isLowCoreCount = (p.coreCount <= 4);

    HMODULE hK32 = GetModuleHandleW(L"kernel32.dll");
    if (hK32) {
      g_pfnGetSystemCpuSetInformation =
          (pfnGetSystemCpuSetInformation)GetProcAddress(
              hK32, "GetSystemCpuSetInformation");
      g_pfnSetProcessDefaultCpuSets =
          (pfnSetProcessDefaultCpuSets)GetProcAddress(
              hK32, "SetProcessDefaultCpuSets");
    }

    if (g_pfnGetSystemCpuSetInformation) {
      ULONG len = 0;
      g_pfnGetSystemCpuSetInformation(nullptr, 0, &len, GetCurrentProcess(), 0);
      if (len > 0) {
        std::vector<BYTE> buf(len);
        if (g_pfnGetSystemCpuSetInformation(
                reinterpret_cast<PSYSTEM_CPU_SET_INFORMATION>(buf.data()), len,
                &len, GetCurrentProcess(), 0)) {
          ULONG count = len / sizeof(SYSTEM_CPU_SET_INFORMATION);
          BYTE maxEff = 0;
          BYTE minEff = 255;
          for (ULONG i = 0; i < count; i++) {
            auto *item = reinterpret_cast<PSYSTEM_CPU_SET_INFORMATION>(
                buf.data() + i * sizeof(SYSTEM_CPU_SET_INFORMATION));
            if (item->Type == CpuSetInformation) {
              BYTE eff = item->CpuSet.EfficiencyClass;
              if (eff > maxEff) maxEff = eff;
              if (eff < minEff) minEff = eff;
            }
          }
          if (maxEff > minEff) {
            p.isHybridCpu = true;
            for (ULONG i = 0; i < count; i++) {
              auto *item = reinterpret_cast<PSYSTEM_CPU_SET_INFORMATION>(
                  buf.data() + i * sizeof(SYSTEM_CPU_SET_INFORMATION));
              if (item->Type == CpuSetInformation &&
                  item->CpuSet.EfficiencyClass == maxEff) {
                p.pCoreCpuSetIds.push_back(item->CpuSet.Id);
              }
            }
          }
        }
      }
    }

    return p;
  }();
  return profile;
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

enum class ThrottlePrioritySetting {
  BelowNormal,
  Idle,
};

struct ModSettings {
  bool enableProBalance = true;
  ForegroundPrioritySetting foregroundPriorityLevel =
      ForegroundPrioritySetting::AboveNormal;
  bool enableForegroundCpuSets = false;
  bool enableBackgroundThrottling = true;
  int backgroundCpuThrottleThresholdPercent = 15;
  int systemCpuContentionThresholdPercent = 60;
  ThrottlePrioritySetting backgroundThrottlePriorityLevel =
      ThrottlePrioritySetting::BelowNormal;
  bool enableEcoQosManagement = false;
  bool enableSmartAiOptimization = true;
  int aiInactivityGraceMinutes = 5;
  bool enableAudioShielding = true;
  bool enableMultitaskingAdaptation = true;
  bool enableGameModeDetection = true;
  CleanMode cleanMode = CleanMode::SmartThreshold;
  int freeRamThresholdPercent = 20;
  bool enableTieredRamThreshold = true;
  int tieredHogThresholdPercent = 40;
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
  bool pauseOnBattery = true;
  int checkIntervalSec = 10;
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
static std::optional<std::thread> g_workerThread;

static std::atomic<bool> g_hookThreadRunning{false};
static HANDLE g_hookThreadHandle = nullptr;
static HANDLE g_hookThreadReadyEvent = nullptr;
static DWORD g_hookThreadId = 0;
static HWINEVENTHOOK g_winEventHook = nullptr;
static std::atomic<bool> g_panicHotkeyRegistered{false};
static std::atomic<bool> g_systemSuspended{false};

static std::atomic<bool> g_forceCleanupRequested{false};
static std::atomic<bool> g_gameSweepRequested{false};

// Unified priority management state (foreground boost & background throttling),
// guarded by g_priorityMutex so neither feature can misread or overwrite
// an original priority set by the other.
static std::mutex g_priorityMutex;

struct BoostedProcessEntry {
  DWORD pid = 0;
  HANDLE hProcess = nullptr;
  DWORD originalPriority = NORMAL_PRIORITY_CLASS;
  ULONG originalIoPriority = IoPriorityNormal;
  ULONG originalMemoryPriority = 5; // MEMORY_PRIORITY_NORMAL
};
static DWORD g_currentBoostedPid = 0;
static std::vector<BoostedProcessEntry> g_boostedProcesses;

// Background CPU-throttling state, guarded by g_priorityMutex.
// We hold an open handle for each throttled process to prevent Windows from
// recycling its PID.
struct ThrottledProcessInfo {
  HANDLE hProcess = nullptr;
  DWORD originalPriority = NORMAL_PRIORITY_CLASS;
  ULONG originalIoPriority = IoPriorityNormal;
  ULONG originalMemoryPriority = 5; // MEMORY_PRIORITY_NORMAL
  bool ecoQosApplied = false;
  DWORD staleSampleCount = 0;
};
static std::map<DWORD, ThrottledProcessInfo>
    g_throttledProcesses; // pid -> info
struct CpuSample {
  ULONGLONG kernelPlusUser100ns = 0;
  std::chrono::steady_clock::time_point sampleTime{};
};
static std::map<DWORD, CpuSample> g_cpuSamples;

// AI Workload Tracking (Inference Activity Timestamps)
static std::map<DWORD, std::chrono::steady_clock::time_point>
    g_aiLastInferenceTime;
// Last known working-set size per AI pid; a meaningful change is used as a
// second "still active" signal alongside CPU usage (see
// UpdateAiProcessActivity).
static std::map<DWORD, SIZE_T> g_aiLastWorkingSetSize;
static std::map<DWORD, CpuSample> g_aiCpuSamples;

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

// Access-denied counter: tracks elevated/SYSTEM processes skipped while running in user session
static std::atomic<DWORD> g_accessDeniedCount{0};

// Session stats.
static std::chrono::steady_clock::time_point g_modStartTime{};
static ULONGLONG g_sessionBytesReclaimed = 0;
static DWORD g_sessionCleanupPasses = 0;
static DWORD g_sessionProcessesTrimmedTotal = 0;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

template <typename TMap>
static void PruneDeadPids(TMap &map, const std::unordered_set<DWORD> &alivePids) {
  for (auto it = map.begin(); it != map.end();) {
    it = (alivePids.count(it->first) == 0) ? map.erase(it) : std::next(it);
  }
}

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
      L"llama-server.exe",
      L"llama-cli.exe",
      L"lm studio.exe",
      L"lmstudio.exe",
      L"lms.exe",
      L"ollama.exe",
      L"ollama_llama_server.exe",
      L"ollama runner.exe",
      L"koboldcpp.exe",
      L"jan.exe",
      L"cortex.exe",
      L"nitro.exe",
      L"text-generation-webui.exe",
      L"oobabooga.exe",
      L"comfyui.exe",
      L"comfyui-electron.exe",
      L"fooocus.exe",
      L"invokeai.exe",
      L"anythingllm.exe",
      L"anythingllm-desktop.exe",
      L"msty.exe",
      L"msty-app.exe",
      L"gpt4all.exe",
      L"backyard.exe",
      L"faraday.exe",
      L"local-ai.exe",
      L"localai.exe",
      L"vllm.exe",
      L"tabby.exe",
      L"aphrodite.exe",
      L"exllama.exe"};
  if (IsInList(name, kAiProcesses))
    return true;

  if (name.starts_with(L"koboldcpp"))
    return true;

  return false;
}

// Critical OS authentication and security dialogs that must NEVER be touched,
// regardless of user settings or desktop state.
static bool IsEssentialSystemSecurityProcess(const std::wstring &name) {
  static const std::vector<std::wstring> kEssential = {
      L"logonui.exe", L"lockapp.exe", L"consent.exe", L"credentialuibroker.exe",
      L"smartscreen.exe", L"securityhealthservice.exe", L"securityhealthsystray.exe",
      L"splwow64.exe", L"printfilterpipelinesvc.exe", L"spoolsv.exe",
      L"wudfhost.exe", L"devicecensus.exe"
  };
  return IsInList(name, kEssential);
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

static void ScanDeviceAudioSessions(IMMDevice *pDevice,
                                    std::unordered_set<DWORD> &audioPids) {
  if (!pDevice)
    return;
  IAudioSessionManager2 *pSessionManager = nullptr;
  HRESULT hr =
      pDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, nullptr,
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
}

static std::unordered_set<DWORD> GetActiveAudioProcessIds() {
  std::unordered_set<DWORD> audioPids;

  bool weInitializedCom = false;
  HRESULT hrCom = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
  if (hrCom == S_OK) {
    weInitializedCom = true;
  }

  IMMDeviceEnumerator *pEnumerator = nullptr;
  HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                                CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
                                reinterpret_cast<void **>(&pEnumerator));
  if (SUCCEEDED(hr) && pEnumerator) {
    // 1. Enumerate all active audio render endpoints (headphones, speakers,
    // virtual channels)
    IMMDeviceCollection *pCollection = nullptr;
    hr = pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE,
                                         &pCollection);
    if (SUCCEEDED(hr) && pCollection) {
      UINT devCount = 0;
      pCollection->GetCount(&devCount);
      for (UINT i = 0; i < devCount; ++i) {
        IMMDevice *pDevice = nullptr;
        if (SUCCEEDED(pCollection->Item(i, &pDevice)) && pDevice) {
          ScanDeviceAudioSessions(pDevice, audioPids);
          pDevice->Release();
        }
      }
      pCollection->Release();
    }

    // 2. Fallback check for default console & multimedia endpoints if needed
    if (audioPids.empty()) {
      IMMDevice *pDef = nullptr;
      if (SUCCEEDED(
              pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDef)) &&
          pDef) {
        ScanDeviceAudioSessions(pDef, audioPids);
        pDef->Release();
      }
      if (SUCCEEDED(pEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia,
                                                         &pDef)) &&
          pDef) {
        ScanDeviceAudioSessions(pDef, audioPids);
        pDef->Release();
      }
    }

    pEnumerator->Release();
  }

  if (weInitializedCom) {
    CoUninitialize();
  }

  return audioPids;
}

// Cache to avoid enumerating WASAPI sessions twice back-to-back
// and maintain a sticky audio grace period (hysteresis against buffer gaps).
static std::chrono::steady_clock::time_point g_audioPidsCacheTime{};
static std::unordered_set<DWORD> g_audioPidsCache;
static std::map<DWORD, std::chrono::steady_clock::time_point>
    g_audioPidLastActive;

static std::unordered_set<DWORD>
GetActiveAudioProcessIdsCached(const ModSettings &settings) {
  if (!settings.enableAudioShielding) {
    return {};
  }
  auto now = std::chrono::steady_clock::now();
  // Refresh every 1.5s so newly started media streams are shielded promptly.
  if (g_audioPidsCacheTime.time_since_epoch().count() != 0 &&
      std::chrono::duration_cast<std::chrono::milliseconds>(
          now - g_audioPidsCacheTime)
              .count() < 1500) {
    return g_audioPidsCache;
  }

  std::unordered_set<DWORD> freshPids = GetActiveAudioProcessIds();
  for (DWORD pid : freshPids) {
    g_audioPidLastActive[pid] = now;
  }

  // 30-second sticky grace period: protects against transient buffer pauses,
  // stream chunk switching, or silent intervals in video playback.
  std::unordered_set<DWORD> shieldedPids;
  for (auto it = g_audioPidLastActive.begin();
       it != g_audioPidLastActive.end();) {
    auto sec =
        std::chrono::duration_cast<std::chrono::seconds>(now - it->second)
            .count();
    if (sec <= 30) {
      shieldedPids.insert(it->first);
      ++it;
    } else {
      it = g_audioPidLastActive.erase(it);
    }
  }

  g_audioPidsCache = std::move(shieldedPids);
  g_audioPidsCacheTime = now;
  return g_audioPidsCache;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// Process Snapshot Helper
// ---------------------------------------------------------------------------

struct ProcessSnapshotEntry {
  DWORD pid = 0;
  DWORD parentPid = 0;
  std::wstring name;
  DWORD threadCount = 0;
};

// Short cache so passes immediately following each other don't walk
// the whole process list repeatedly for the same answer.
static std::mutex g_processSnapshotMutex;
static std::chrono::steady_clock::time_point g_processSnapshotCacheTime{};
static std::vector<ProcessSnapshotEntry> g_processSnapshotCache;

static std::vector<ProcessSnapshotEntry> CaptureProcessSnapshotCached() {
  std::lock_guard<std::mutex> lock(g_processSnapshotMutex);
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
        entry.threadCount = pe.cntThreads;
        result.push_back(std::move(entry));
      } while (Process32NextW(snapshot, &pe));
    }
    CloseHandle(snapshot);
  }

  g_processSnapshotCache = std::move(result);
  g_processSnapshotCacheTime = now;
  return g_processSnapshotCache;
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
// I/O & Memory Priority & EcoQoS Helpers
// ---------------------------------------------------------------------------

static ULONG GetProcessIoPriorityHint(HANDLE hProcess) {
  if (!g_pfnNtQueryInformationProcess)
    return IoPriorityNormal;
  ULONG ioPriority = IoPriorityNormal;
  ULONG returnLength = 0;
  NTSTATUS status = g_pfnNtQueryInformationProcess(
      hProcess, ProcessIoPriorityInfoClass, &ioPriority, sizeof(ioPriority),
      &returnLength);
  if (status >= 0) {
    return ioPriority;
  }
  return IoPriorityNormal;
}

static void SetProcessIoPriorityHint(HANDLE hProcess, ULONG priority) {
  if (!g_pfnNtSetInformationProcess)
    return;
  // User-mode applications operate at IoPriorityNormal (2), IoPriorityLow (1), or
  // IoPriorityVeryLow (0). IoPriorityHigh (3) is reserved for kernel MMCSS audio and
  // system paging, which requires SeIncreaseBasePriorityPrivilege. Clamping to IoPriorityNormal
  // ensures unprivileged execution with zero security errors.
  if (priority > IoPriorityNormal) {
    priority = IoPriorityNormal;
  }
  g_pfnNtSetInformationProcess(
      hProcess, ProcessIoPriorityInfoClass, &priority, sizeof(priority));
}

static ULONG GetProcessMemoryPriorityHint(HANDLE hProcess) {
  MEMORY_PRIORITY_INFORMATION mpi{};
  if (GetProcessInformation(hProcess, ProcessMemoryPriority, &mpi, sizeof(mpi))) {
    return mpi.MemoryPriority;
  }
  return 5; // MEMORY_PRIORITY_NORMAL
}

static bool SetProcessMemoryPriorityHint(HANDLE hProcess, ULONG priority) {
  MEMORY_PRIORITY_INFORMATION mpi{};
  mpi.MemoryPriority = priority;
  return SetProcessInformation(hProcess, ProcessMemoryPriority, &mpi, sizeof(mpi)) != 0;
}

static void SetProcessEcoQoS(HANDLE hProcess, bool enableThrottling) {
  PROCESS_POWER_THROTTLING_STATE state{};
  state.Version = PROCESS_POWER_THROTTLING_CURRENT_VERSION;
  state.ControlMask = PROCESS_POWER_THROTTLING_EXECUTION_SPEED |
                      PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION;
  state.StateMask =
      enableThrottling ? (PROCESS_POWER_THROTTLING_EXECUTION_SPEED |
                          PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION)
                       : 0;
  SetProcessInformation(
      hProcess, (PROCESS_INFORMATION_CLASS)ProcessPowerThrottlingInfoClass,
      &state, sizeof(state));
}

// Restore: hand power management control cleanly back to the OS scheduler.
// ControlMask specifies which flags to modify; StateMask = 0 clears throttling.
static void ResetProcessEcoQoS(HANDLE hProcess) {
  PROCESS_POWER_THROTTLING_STATE state{};
  state.Version = PROCESS_POWER_THROTTLING_CURRENT_VERSION;
  state.ControlMask = PROCESS_POWER_THROTTLING_EXECUTION_SPEED |
                      PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION;
  state.StateMask = 0;
  SetProcessInformation(
      hProcess, (PROCESS_INFORMATION_CLASS)ProcessPowerThrottlingInfoClass,
      &state, sizeof(state));
}

// ---------------------------------------------------------------------------
// Foreground Priority Boost (Process Family Zero-Stutter)
// ---------------------------------------------------------------------------

// Helper to safely detect terminated processes even if SYNCHRONIZE was not granted
static bool IsProcessTerminated(HANDLE hProcess) {
  if (!hProcess)
    return true;
  DWORD exitCode = 0;
  if (GetExitCodeProcess(hProcess, &exitCode) && exitCode != STILL_ACTIVE) {
    return true;
  }
  DWORD waitRes = WaitForSingleObject(hProcess, 0);
  return (waitRes == WAIT_OBJECT_0);
}

// Must be called while holding g_priorityMutex.
static void RestoreForegroundBoostLocked() {
  for (auto &entry : g_boostedProcesses) {
    if (entry.hProcess) {
      if (!IsProcessTerminated(entry.hProcess)) {
        SetPriorityClass(entry.hProcess, entry.originalPriority);
        SetProcessIoPriorityHint(entry.hProcess, entry.originalIoPriority);
        SetProcessMemoryPriorityHint(entry.hProcess, entry.originalMemoryPriority);
        if (g_pfnSetProcessDefaultCpuSets) {
          g_pfnSetProcessDefaultCpuSets(entry.hProcess, nullptr, 0);
        }
      }
      CloseHandle(entry.hProcess);
    }
  }
  g_boostedProcesses.clear();
  g_currentBoostedPid = 0;
}

static void UpdateForegroundBoost(DWORD newForegroundPid,
                                  const ModSettings &settings) {
  std::lock_guard<std::mutex> lock(g_priorityMutex);

  DWORD currentPid = GetCurrentProcessId();

  if (!settings.enableProBalance) {
    RestoreForegroundBoostLocked();
    // Even if ProBalance CPU elevation is disabled, ensure the active foreground
    // process family does not retain MEMORY_PRIORITY_VERY_LOW from background trimming.
    if (newForegroundPid != 0 && newForegroundPid != currentPid &&
        newForegroundPid != 4) {
      std::vector<ProcessSnapshotEntry> snapshot = CaptureProcessSnapshotCached();
      std::map<DWORD, std::vector<DWORD>> childrenOf;
      for (const auto &entry : snapshot) {
        childrenOf[entry.parentPid].push_back(entry.pid);
      }
      std::vector<DWORD> pidsToRestoreMem;
      pidsToRestoreMem.push_back(newForegroundPid);
      std::unordered_set<DWORD> visited;
      visited.insert(newForegroundPid);
      CollectDescendants(newForegroundPid, childrenOf, pidsToRestoreMem, visited);
      for (DWORD pid : pidsToRestoreMem) {
        if (pid == 0 || pid == 4 || pid == currentPid)
          continue;
        HANDLE hProc = OpenProcess(
            PROCESS_SET_INFORMATION | PROCESS_QUERY_LIMITED_INFORMATION, FALSE,
            pid);
        if (hProc) {
          SetProcessMemoryPriorityHint(hProc, 5 /* MEMORY_PRIORITY_NORMAL */);
          ResetProcessEcoQoS(hProc);
          CloseHandle(hProc);
        }
      }
    }
    return;
  }

  if (newForegroundPid == g_currentBoostedPid) {
    return;
  }

  // 1. Restore previous foreground app process family.
  RestoreForegroundBoostLocked();

  if (newForegroundPid == 0 || newForegroundPid == currentPid ||
      newForegroundPid == 4) {
    return;
  }

  // 2. Discover entire process family (root + all descendants: renderers, GPU, worker processes)
  std::vector<ProcessSnapshotEntry> snapshot = CaptureProcessSnapshotCached();
  std::map<DWORD, std::vector<DWORD>> childrenOf;
  for (const auto &entry : snapshot) {
    childrenOf[entry.parentPid].push_back(entry.pid);
  }

  std::vector<DWORD> pidsToBoost;
  pidsToBoost.push_back(newForegroundPid);
  std::unordered_set<DWORD> visited;
  visited.insert(newForegroundPid);
  CollectDescendants(newForegroundPid, childrenOf, pidsToBoost, visited);

  DWORD targetPriority =
      (settings.foregroundPriorityLevel == ForegroundPrioritySetting::High)
          ? HIGH_PRIORITY_CLASS
          : ABOVE_NORMAL_PRIORITY_CLASS;
  ULONG targetIo = IoPriorityNormal;

  for (DWORD pid : pidsToBoost) {
    if (pid == 0 || pid == 4 || pid == currentPid)
      continue;

    HANDLE hProc =
        OpenProcess(PROCESS_SET_INFORMATION | PROCESS_QUERY_LIMITED_INFORMATION | SYNCHRONIZE,
                    FALSE, pid);
    if (!hProc) {
      if (GetLastError() == ERROR_ACCESS_DENIED) {
        g_accessDeniedCount.fetch_add(1, std::memory_order_relaxed);
      }
      continue;
    }

    DWORD origPriority = NORMAL_PRIORITY_CLASS;
    ULONG origIoPriority = IoPriorityNormal;
    ULONG origMemoryPriority = 5; // MEMORY_PRIORITY_NORMAL

    // If previously throttled by background throttler, recover true pre-throttled states
    auto itThrottled = g_throttledProcesses.find(pid);
    if (itThrottled != g_throttledProcesses.end()) {
      origPriority = itThrottled->second.originalPriority;
      origIoPriority = itThrottled->second.originalIoPriority;
      origMemoryPriority = itThrottled->second.originalMemoryPriority;
      bool ecoQosWasApplied = itThrottled->second.ecoQosApplied;
      if (itThrottled->second.hProcess) {
        CloseHandle(itThrottled->second.hProcess);
      }
      g_throttledProcesses.erase(itThrottled);

      SetPriorityClass(hProc, origPriority);
      SetProcessIoPriorityHint(hProc, origIoPriority);
      SetProcessMemoryPriorityHint(hProc, origMemoryPriority);
      if (ecoQosWasApplied) {
        ResetProcessEcoQoS(hProc);
      }
    } else {
      DWORD prevPriority = GetPriorityClass(hProc);
      origPriority = (prevPriority != 0) ? prevPriority : NORMAL_PRIORITY_CLASS;
      origIoPriority = GetProcessIoPriorityHint(hProc);
      origMemoryPriority = GetProcessMemoryPriorityHint(hProc);
    }

    // Anti-EcoQoS Lock: Prevent Windows 11 power throttling from restricting active foreground app
    ResetProcessEcoQoS(hProc);
    // Ensure active foreground process family always has normal memory priority
    SetProcessMemoryPriorityHint(hProc, 5 /* MEMORY_PRIORITY_NORMAL */);

    // Only elevate if original priority was normal or lower (IDLE, BELOW_NORMAL, NORMAL)
    int origRank = PriorityClassToRank(origPriority);
    if (origRank > 0 && origRank <= PriorityClassToRank(NORMAL_PRIORITY_CLASS)) {
      if (SetPriorityClass(hProc, targetPriority)) {
        SetProcessIoPriorityHint(hProc, targetIo);

        // Hybrid Architecture Optimization: Suggest P-cores to the main foreground window only
        const SystemHardwareProfile &hw = GetHardwareProfile();
        if (settings.enableForegroundCpuSets && pid == newForegroundPid &&
            hw.isHybridCpu && g_pfnSetProcessDefaultCpuSets && !hw.pCoreCpuSetIds.empty()) {
          g_pfnSetProcessDefaultCpuSets(hProc, hw.pCoreCpuSetIds.data(),
                                        static_cast<ULONG>(hw.pCoreCpuSetIds.size()));
        }

        BoostedProcessEntry entry;
        entry.pid = pid;
        entry.hProcess = hProc;
        entry.originalPriority = origPriority;
        entry.originalIoPriority = origIoPriority;
        entry.originalMemoryPriority = origMemoryPriority;
        g_boostedProcesses.push_back(entry);
        continue;
      }
    }

    CloseHandle(hProc);
  }

  g_currentBoostedPid = newForegroundPid;
}

// ---------------------------------------------------------------------------
// Background CPU/I/O Throttling with Audio & AI Workload Protection
// ---------------------------------------------------------------------------

static double
SampleCpuPercent(DWORD pid, HANDLE hProcess,
                 std::map<DWORD, CpuSample> &sampleMap = g_cpuSamples) {
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
  auto it = sampleMap.find(pid);
  if (it != sampleMap.end()) {
    ULONGLONG delta100ns =
        (totalTime100ns > it->second.kernelPlusUser100ns)
            ? (totalTime100ns - it->second.kernelPlusUser100ns)
            : 0;
    auto deltaWallMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                           now - it->second.sampleTime)
                           .count();
    if (deltaWallMs > 0) {
      double deltaTimeMs = delta100ns / 10000.0;
      cpuPercent =
          (deltaTimeMs / (double)deltaWallMs) * 100.0 / GetSystemCoreCount();
    }
  }

  sampleMap[pid] = {totalTime100ns, now};
  return cpuPercent;
}

struct IoSample {
  ULONGLONG writeBytes = 0;
  std::chrono::steady_clock::time_point sampleTime{};
};
static std::map<DWORD, IoSample> g_ioSamples;

// Detects active disk writes (e.g. copying files to a USB flash drive, external SSD, or disk).
// Returns true if the process is actively writing at >= 500 KB/s.
static bool IsProcessActivelyWritingDisk(DWORD pid, HANDLE hProcess) {
  IO_COUNTERS io{};
  if (!GetProcessIoCounters(hProcess, &io))
    return false;

  auto now = std::chrono::steady_clock::now();
  auto it = g_ioSamples.find(pid);
  if (it == g_ioSamples.end()) {
    g_ioSamples[pid] = {io.WriteTransferCount, now};
    return false;
  }

  double deltaWallMs =
      (double)std::chrono::duration_cast<std::chrono::milliseconds>(
          now - it->second.sampleTime)
          .count();
  bool isWriting = false;
  if (deltaWallMs >= 500.0) {
    if (io.WriteTransferCount > it->second.writeBytes) {
      double deltaBytes = (double)(io.WriteTransferCount - it->second.writeBytes);
      double writeRateBps = (deltaBytes / deltaWallMs) * 1000.0;
      // Active write rate >= 500 KB/s (e.g. file transfers, USB copies, backups)
      if (writeRateBps >= 500.0 * 1024.0) {
        isWriting = true;
      }
    }
    it->second = {io.WriteTransferCount, now};
  }
  return isWriting;
}

// Samples overall system-wide CPU usage (0.0% to 100.0%) across all cores.
// Used to gate background throttling so idle systems are never throttled unnecessarily.
static double SampleSystemCpuPercent() {
  static FILETIME prevIdle{}, prevKernel{}, prevUser{};
  static bool hasPrev = false;

  FILETIME idle, kernel, user;
  if (!GetSystemTimes(&idle, &kernel, &user)) {
    return -1.0;
  }

  ULARGE_INTEGER i, k, u;
  i.LowPart = idle.dwLowDateTime;
  i.HighPart = idle.dwHighDateTime;
  k.LowPart = kernel.dwLowDateTime;
  k.HighPart = kernel.dwHighDateTime;
  u.LowPart = user.dwLowDateTime;
  u.HighPart = user.dwHighDateTime;

  if (!hasPrev) {
    prevIdle = idle;
    prevKernel = kernel;
    prevUser = user;
    hasPrev = true;
    return -1.0;
  }

  ULARGE_INTEGER pi, pk, pu;
  pi.LowPart = prevIdle.dwLowDateTime;
  pi.HighPart = prevIdle.dwHighDateTime;
  pk.LowPart = prevKernel.dwLowDateTime;
  pk.HighPart = prevKernel.dwHighDateTime;
  pu.LowPart = prevUser.dwLowDateTime;
  pu.HighPart = prevUser.dwHighDateTime;

  prevIdle = idle;
  prevKernel = kernel;
  prevUser = user;

  ULONGLONG idleDelta = (i.QuadPart > pi.QuadPart) ? (i.QuadPart - pi.QuadPart) : 0;
  ULONGLONG kernelDelta = (k.QuadPart > pk.QuadPart) ? (k.QuadPart - pk.QuadPart) : 0;
  ULONGLONG userDelta = (u.QuadPart > pu.QuadPart) ? (u.QuadPart - pu.QuadPart) : 0;

  // In GetSystemTimes, kernel time includes idle time across all cores.
  ULONGLONG totalDelta = kernelDelta + userDelta;
  if (totalDelta == 0) {
    return 0.0;
  }

  if (idleDelta > totalDelta) {
    idleDelta = totalDelta;
  }

  ULONGLONG busyDelta = totalDelta - idleDelta;
  double percent = (static_cast<double>(busyDelta) * 100.0) / static_cast<double>(totalDelta);
  return (std::clamp)(percent, 0.0, 100.0);
}

static bool RestoreAndEraseThrottledProcess(DWORD pid) {
  std::lock_guard<std::mutex> lock(g_priorityMutex);
  auto it = g_throttledProcesses.find(pid);
  if (it == g_throttledProcesses.end()) {
    return false;
  }
  HANDLE hSaved = it->second.hProcess;
  DWORD originalPriority = it->second.originalPriority;
  ULONG originalIoPriority = it->second.originalIoPriority;
  ULONG originalMemoryPriority = it->second.originalMemoryPriority;
  bool ecoQosApplied = it->second.ecoQosApplied;
  g_throttledProcesses.erase(it);

  if (hSaved) {
    if (!IsProcessTerminated(hSaved)) {
      SetPriorityClass(hSaved, originalPriority);
      SetProcessIoPriorityHint(hSaved, originalIoPriority);
      SetProcessMemoryPriorityHint(hSaved, originalMemoryPriority);
      if (ecoQosApplied) {
        ResetProcessEcoQoS(hSaved);
      }
    }
    CloseHandle(hSaved);
  }
  return true;
}

static void RestoreAllThrottledProcesses() {
  std::lock_guard<std::mutex> lock(g_priorityMutex);
  for (auto &kv : g_throttledProcesses) {
    auto &info = kv.second;
    if (info.hProcess) {
      if (!IsProcessTerminated(info.hProcess)) {
        SetPriorityClass(info.hProcess, info.originalPriority);
        SetProcessIoPriorityHint(info.hProcess, info.originalIoPriority);
        SetProcessMemoryPriorityHint(info.hProcess, info.originalMemoryPriority);
        if (info.ecoQosApplied) {
          ResetProcessEcoQoS(info.hProcess);
        }
      }
      CloseHandle(info.hProcess);
    }
  }
  g_throttledProcesses.clear();
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

  // Under Windows 10/11, windows on other virtual desktops or suspended by the shell
  // report IsWindowVisible() == TRUE but have DWMWA_CLOAKED != 0. They are not active
  // on the current user workspace.
  int cloaked = 0;
  if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) &&
      cloaked != 0) {
    return TRUE;
  }

  // Filter out 0x0 or 1x1 stub/helper windows (common for background tray tools)
  RECT rc{};
  if (GetWindowRect(hwnd, &rc)) {
    if ((rc.right - rc.left) <= 1 || (rc.bottom - rc.top) <= 1) {
      return TRUE;
    }
  }

  DWORD pid = 0;
  GetWindowThreadProcessId(hwnd, &pid);
  auto &state = (*map)[pid];
  state.hasVisibleWindow = true;
  if (IsIconic(hwnd)) {
    state.isMinimized = true;
  }
  return TRUE;
}

static std::chrono::steady_clock::time_point g_windowStateCacheTime{};
static std::map<DWORD, WindowState> g_windowStateCache;

static std::map<DWORD, WindowState> BuildWindowStateMapCached() {
  auto now = std::chrono::steady_clock::now();
  if (g_windowStateCacheTime.time_since_epoch().count() != 0 &&
      std::chrono::duration_cast<std::chrono::milliseconds>(
          now - g_windowStateCacheTime)
              .count() < 2000) {
    return g_windowStateCache;
  }

  std::map<DWORD, WindowState> map;
  EnumWindows(EnumWindowStateProc, reinterpret_cast<LPARAM>(&map));
  g_windowStateCache = map;
  g_windowStateCacheTime = now;
  return g_windowStateCache;
}

// ---------------------------------------------------------------------------
// Audio Process Tree Expansion Helper
// ---------------------------------------------------------------------------

static std::unordered_set<DWORD>
ExpandAudioProcessShield(const std::unordered_set<DWORD> &rawAudioPids,
                         const std::vector<ProcessSnapshotEntry> &processList,
                         const std::map<DWORD, std::vector<DWORD>> &childrenOf,
                         const std::map<DWORD, DWORD> &parentOf) {
  if (rawAudioPids.empty())
    return {};

  std::map<DWORD, std::wstring> procNames;
  for (const auto &entry : processList) {
    procNames[entry.pid] = entry.name;
  }

  std::unordered_set<DWORD> activeAudioPids = rawAudioPids;
  for (DWORD aPid : rawAudioPids) {
    // 1. All processes sharing the exact same executable name (browser
    // tabs/renderers/audio engine), provided it's not a generic host process
    auto itName = procNames.find(aPid);
    if (itName != procNames.end()) {
      const std::wstring &aName = itName->second;
      static const std::vector<std::wstring> kGenericAudioHosts = {
          L"svchost.exe", L"msedgewebview2.exe", L"node.exe", L"rundll32.exe",
          L"dllhost.exe", L"cmd.exe", L"powershell.exe"
      };
      if (!IsInList(aName, kGenericAudioHosts)) {
        for (const auto &kv : procNames) {
          if (kv.second == aName) {
            activeAudioPids.insert(kv.first);
          }
        }
      }
    }

    // 2. All descendants in the process tree (child tabs, audio utilities,
    // workers)
    std::vector<DWORD> descendants;
    std::unordered_set<DWORD> visited;
    CollectDescendants(aPid, childrenOf, descendants, visited);
    for (DWORD dPid : descendants) {
      activeAudioPids.insert(dPid);
    }

    // 3. All ancestor processes (main browser / launcher)
    std::unordered_set<DWORD> seen{aPid};
    DWORD cur = aPid;
    while (true) {
      auto itP = parentOf.find(cur);
      if (itP == parentOf.end() || itP->second == 0 || itP->second == 4 ||
          !seen.insert(itP->second).second)
        break;
      activeAudioPids.insert(itP->second);
      cur = itP->second;
    }
  }

  return activeAudioPids;
}

// ---------------------------------------------------------------------------
// Dedicated AI Activity Tracker (Independent of Window & Throttle State)
// ---------------------------------------------------------------------------

static void UpdateAiProcessActivity(const ModSettings &settings) {
  if (!settings.enableSmartAiOptimization)
    return;

  auto now = std::chrono::steady_clock::now();
  std::vector<ProcessSnapshotEntry> processList =
      CaptureProcessSnapshotCached();
  std::unordered_set<DWORD> alivePids;
  alivePids.reserve(processList.size());
  for (const auto &entry : processList) {
    alivePids.insert(entry.pid);
  }

  for (const auto &entry : processList) {
    if (!IsKnownAiProcess(entry.name))
      continue;

    DWORD pid = entry.pid;
    if (pid == 0 || pid == 4)
      continue;

    // Brand new AI process: initialize timestamp so it starts protected
    if (g_aiLastInferenceTime.find(pid) == g_aiLastInferenceTime.end()) {
      g_aiLastInferenceTime[pid] = now;
    }

    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProc)
      continue;

    double cpuPercent = SampleCpuPercent(pid, hProc, g_aiCpuSamples);
    if (cpuPercent >= 2.0) {
      g_aiLastInferenceTime[pid] = now;
    }

    PROCESS_MEMORY_COUNTERS_EX pmc{};
    pmc.cb = sizeof(pmc);
    if (GetProcessMemoryInfo(hProc,
                             reinterpret_cast<PROCESS_MEMORY_COUNTERS *>(&pmc),
                             sizeof(pmc))) {
      auto itWs = g_aiLastWorkingSetSize.find(pid);
      if (itWs != g_aiLastWorkingSetSize.end()) {
        if (pmc.WorkingSetSize > itWs->second &&
            (pmc.WorkingSetSize - itWs->second) > 8ull * 1024 * 1024) {
          g_aiLastInferenceTime[pid] = now;
        }
      }
      g_aiLastWorkingSetSize[pid] = pmc.WorkingSetSize;
    }
    CloseHandle(hProc);
  }

  // Prune dead AI processes even if background throttling is disabled
  PruneDeadPids(g_aiLastInferenceTime, alivePids);
  PruneDeadPids(g_aiLastWorkingSetSize, alivePids);
  PruneDeadPids(g_aiCpuSamples, alivePids);
}

// ---------------------------------------------------------------------------
// Heuristic Compute Sanctuary (Zero-hardcoding detection for voluntary heavy compute)
// ---------------------------------------------------------------------------

static bool IsVoluntaryComputeTask(
    DWORD pid,
    HANDLE hProcess,
    DWORD threadCount,
    double cpuPercent,
    bool isThrottled,
    const std::map<DWORD, DWORD> &parentOf,
    const std::map<DWORD, WindowState> &windowStates,
    const ModSettings &settings,
    DWORD coreCount) {
  // 1. Cooperative priority check:
  // If the process was NOT throttled by us, but has already voluntarily configured
  // itself to BELOW_NORMAL or IDLE, it is already well-behaved and running cooperatively.
  // Imposing EcoQoS or further demoting it would penalize legitimate background renders / exports.
  // CRITICAL: We only consider this if the process was NOT already throttled by us!
  if (!isThrottled) {
    DWORD prio = GetPriorityClass(hProcess);
    if (prio == BELOW_NORMAL_PRIORITY_CLASS || prio == IDLE_PRIORITY_CLASS) {
      return true;
    }
  }

  auto now = std::chrono::steady_clock::now();

  // 2. Lineage / Parent Ancestry Check:
  // Check if this process descends from an active user application (one with a visible window
  // or focused recently within aging grace minutes).
  // This automatically shields compilers (cl.exe, rustc.exe), encoders (ffmpeg.exe),
  // archivers (7z.exe), and render workers spawned by IDEs, terminals, video editors, or 3D suites.
  int graceMinutes = settings.recentActivityGraceMinutes;
  if (graceMinutes < 2) graceMinutes = 2;
  auto maxInactiveDuration = std::chrono::minutes(graceMinutes);

  DWORD current = pid;
  for (int depth = 0; depth < 8; ++depth) {
    auto itParent = parentOf.find(current);
    if (itParent == parentOf.end() || itParent->second == 0 || itParent->second == 4) {
      break;
    }
    DWORD parentPid = itParent->second;
    if (parentPid == current) break; // Avoid cycles

    // Does ancestor have a visible, non-minimized window?
    auto itWin = windowStates.find(parentPid);
    if (itWin != windowStates.end() && itWin->second.hasVisibleWindow && !itWin->second.isMinimized) {
      return true;
    }

    // Was ancestor focused within recent grace period?
    {
      std::lock_guard<std::mutex> lock(g_focusMapMutex);
      auto itFocus = g_processLastFocusedTime.find(parentPid);
      if (itFocus != g_processLastFocusedTime.end()) {
        if ((now - itFocus->second) <= maxInactiveDuration) {
          return true;
        }
      }
    }

    current = parentPid;
  }

  // 3. Parallel Compute Heuristic:
  // Runaway bugs or stuck UI loops are typically single-threaded (1-2 threads spinning).
  // Voluntary heavy compute engines (renderers, compressors, encoders) spawn multi-threaded
  // worker pools scaling with CPU cores (e.g. >= 4 threads, or >= coreCount/2).
  DWORD minParallelThreads = (coreCount > 4) ? (coreCount / 2) : 4;
  if (threadCount >= minParallelThreads && cpuPercent >= 15.0) {
    // If it's running with parallel worker threads, check if it was focused in recent history (up to 30 min)
    {
      std::lock_guard<std::mutex> lock(g_focusMapMutex);
      auto itSelfFocus = g_processLastFocusedTime.find(pid);
      if (itSelfFocus != g_processLastFocusedTime.end()) {
        if ((now - itSelfFocus->second) <= std::chrono::minutes(30)) {
          return true;
        }
      }
    }
    // Also check if any ancestor is an active GUI application (has a visible non-minimized window)
    // or was focused in recent history (up to 30 min)
    DWORD curAnc = pid;
    for (int depth = 0; depth < 8; ++depth) {
      auto itP = parentOf.find(curAnc);
      if (itP == parentOf.end() || itP->second == 0 || itP->second == 4) break;
      auto itWin = windowStates.find(itP->second);
      if (itWin != windowStates.end() && itWin->second.hasVisibleWindow && !itWin->second.isMinimized) {
        return true;
      }
      std::lock_guard<std::mutex> lock(g_focusMapMutex);
      auto itF = g_processLastFocusedTime.find(itP->second);
      if (itF != g_processLastFocusedTime.end() && (now - itF->second) <= std::chrono::minutes(30)) {
        return true;
      }
      curAnc = itP->second;
    }
  }

  return false;
}

static void ApplyBackgroundThrottling(const ModSettings &settings,
                                      DWORD foregroundPid) {
  if (!settings.enableBackgroundThrottling) {
    RestoreAllThrottledProcesses();
    return;
  }
  if (foregroundPid == 0)
    return;

  // If foreground process was throttled, restore and erase it immediately
  RestoreAndEraseThrottledProcess(foregroundPid);

  DWORD currentPid = GetCurrentProcessId();

  // Guarantee that the active foreground process never retains a demoted memory priority
  if (foregroundPid != 0 && foregroundPid != 4 && foregroundPid != currentPid) {
    HANDLE hFg = OpenProcess(
        PROCESS_SET_INFORMATION | PROCESS_QUERY_LIMITED_INFORMATION, FALSE,
        foregroundPid);
    if (hFg) {
      SetProcessMemoryPriorityHint(hFg, 5 /* MEMORY_PRIORITY_NORMAL */);
      CloseHandle(hFg);
    }
  }

  auto now = std::chrono::steady_clock::now();

  // Shield active audio sessions from being throttled.
  std::unordered_set<DWORD> rawAudioPids;
  if (settings.enableAudioShielding) {
    rawAudioPids = GetActiveAudioProcessIdsCached(settings);
  }

  // Sample overall system CPU usage for ProBalance contention gating.
  double systemCpuPercent = SampleSystemCpuPercent();
  bool systemUnderContention =
      (settings.systemCpuContentionThresholdPercent <= 0) ||
      (systemCpuPercent >= settings.systemCpuContentionThresholdPercent);
  bool systemContentionCleared =
      (settings.systemCpuContentionThresholdPercent > 0) &&
      (systemCpuPercent >= 0.0) &&
      (systemCpuPercent < settings.systemCpuContentionThresholdPercent * 0.7);

  bool isMultiTasking =
      settings.enableMultitaskingAdaptation && IsActiveMultiTaskingMode();

  std::vector<ProcessSnapshotEntry> snapshot = CaptureProcessSnapshotCached();
  std::unordered_set<DWORD> alivePids;
  std::map<DWORD, std::wstring> procNames;
  std::map<DWORD, std::vector<DWORD>> childrenOf;
  std::map<DWORD, DWORD> parentOf;
  std::map<DWORD, DWORD> threadCounts;
  for (const auto &entry : snapshot) {
    alivePids.insert(entry.pid);
    procNames[entry.pid] = entry.name;
    childrenOf[entry.parentPid].push_back(entry.pid);
    parentOf[entry.pid] = entry.parentPid;
    threadCounts[entry.pid] = entry.threadCount;
  }

  // Expand audio shield to entire process tree & executable family
  std::unordered_set<DWORD> activeAudioPids =
      ExpandAudioProcessShield(rawAudioPids, snapshot, childrenOf, parentOf);

  // Immunize the entire active foreground family from background throttling
  std::unordered_set<DWORD> fgFamilyPids;
  if (foregroundPid != 0) {
    fgFamilyPids.insert(foregroundPid);
    std::vector<DWORD> fgDescendants;
    std::unordered_set<DWORD> visited;
    CollectDescendants(foregroundPid, childrenOf, fgDescendants, visited);
    for (DWORD fgChild : fgDescendants) {
      fgFamilyPids.insert(fgChild);
    }
  }

  std::map<DWORD, WindowState> windowStates = BuildWindowStateMapCached();

  for (DWORD pid : alivePids) {
    if (pid == 0 || pid == 4 || pid == currentPid || fgFamilyPids.count(pid))
      continue;

    const std::wstring &name = procNames[pid];
    if (IsInList(name, settings.excludedProcesses) ||
        IsEssentialSystemSecurityProcess(name))
      continue;

    // Session-0/service processes aren't something the user is "using";
    // skip them regardless of name (more robust than a name blocklist).
    if (!IsInteractiveSessionProcess(pid))
      continue;

    bool isAi = settings.enableSmartAiOptimization && IsKnownAiProcess(name);

    // Audio stream protection for entire browser/media tree:
    // If active or recently active, immediately restore priority if throttled.
    if (settings.enableAudioShielding && activeAudioPids.count(pid)) {
      RestoreAndEraseThrottledProcess(pid);
      continue;
    }

    auto wsIt = windowStates.find(pid);
    bool hasVisibleWindow =
        (wsIt != windowStates.end() && wsIt->second.hasVisibleWindow &&
         !wsIt->second.isMinimized);

    // Visible Window Protection:
    // An application with a visible non-minimized window must NEVER be
    // throttled.
    if (hasVisibleWindow) {
      RestoreAndEraseThrottledProcess(pid);
      continue;
    }

    HANDLE hProc =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_SET_INFORMATION | SYNCHRONIZE,
                    FALSE, pid);
    if (!hProc) {
      if (GetLastError() == ERROR_ACCESS_DENIED) {
        g_accessDeniedCount.fetch_add(1, std::memory_order_relaxed);
      }
      continue;
    }

    // Packaged (UWP/MSIX) apps are already suspended/managed by Windows
    // itself; leave them alone rather than fight with the OS scheduler.
    if (IsPackagedApp(hProc)) {
      CloseHandle(hProc);
      continue;
    }

    double cpuPercent = SampleCpuPercent(pid, hProc);
    bool isThrottled = false;
    {
      std::lock_guard<std::mutex> lock(g_priorityMutex);
      isThrottled = (g_throttledProcesses.count(pid) != 0);
    }

    // AI Engine Sanctuary: Never throttle local AI during token generation.
    if (isAi) {
      auto itAi = g_aiLastInferenceTime.find(pid);
      bool isGenerating =
          (itAi != g_aiLastInferenceTime.end()) &&
          (std::chrono::duration_cast<std::chrono::seconds>(now - itAi->second)
               .count() < 10);
      if (isGenerating) {
        RestoreAndEraseThrottledProcess(pid);
        CloseHandle(hProc);
        continue;
      }
    }

    // Heuristic Compute Sanctuary:
    // Protect voluntary heavy compute workloads (3D rendering, video exports,
    // parallel compilations, multi-threaded compression) dynamically without hardcoded names.
    DWORD threads = 0;
    auto itThCnt = threadCounts.find(pid);
    if (itThCnt != threadCounts.end()) {
      threads = itThCnt->second;
    }
    const SystemHardwareProfile &hw = GetHardwareProfile();
    if (IsVoluntaryComputeTask(pid, hProc, threads, cpuPercent, isThrottled,
                               parentOf, windowStates, settings, hw.coreCount)) {
      if (isThrottled) {
        RestoreAndEraseThrottledProcess(pid);
      }
      CloseHandle(hProc);
      continue;
    }

    bool isCpuHeavy =
        (cpuPercent >= settings.backgroundCpuThrottleThresholdPercent);

    bool isCpuCalm =
        (cpuPercent >= 0) &&
        (cpuPercent < settings.backgroundCpuThrottleThresholdPercent / 2.0);

    bool isDiskWriting = IsProcessActivelyWritingDisk(pid, hProc);

    if (isThrottled && isDiskWriting) {
      // If an already-throttled process begins actively copying/writing files
      // (e.g. to a USB key or disk), restore its normal I/O priority immediately.
      SetProcessIoPriorityHint(hProc, IoPriorityNormal);
      if (settings.enableEcoQosManagement) {
        ResetProcessEcoQoS(hProc);
      }
    }

    if (isCpuHeavy && !isThrottled && systemUnderContention) {
      DWORD prevPriority = GetPriorityClass(hProc);
      if (PriorityClassToRank(prevPriority) >
              PriorityClassToRank(IDLE_PRIORITY_CLASS) &&
          PriorityClassToRank(prevPriority) <=
              PriorityClassToRank(NORMAL_PRIORITY_CLASS)) {
        DWORD targetThrottlePrio = BELOW_NORMAL_PRIORITY_CLASS;
        if (!isMultiTasking && settings.backgroundThrottlePriorityLevel ==
                                   ThrottlePrioritySetting::Idle) {
          targetThrottlePrio = IDLE_PRIORITY_CLASS;
        }

        if (PriorityClassToRank(prevPriority) >
            PriorityClassToRank(targetThrottlePrio)) {
          ULONG prevIo = GetProcessIoPriorityHint(hProc);
          ULONG prevMem = GetProcessMemoryPriorityHint(hProc);
          bool appliedEcoQos = false;
          {
            std::lock_guard<std::mutex> lock(g_priorityMutex);
            if (pid != g_currentBoostedPid &&
                g_throttledProcesses.count(pid) == 0) {
              if (SetPriorityClass(hProc, targetThrottlePrio)) {
                // Apply MEMORY_PRIORITY_VERY_LOW to throttled process
                SetProcessMemoryPriorityHint(hProc, 1 /* MEMORY_PRIORITY_VERY_LOW */);

                // Never degrade I/O priority or apply EcoQoS on active disk/USB writers
                if (!isDiskWriting) {
                  SetProcessIoPriorityHint(hProc, IoPriorityLow);
                  if (settings.enableEcoQosManagement) {
                    SetProcessEcoQoS(hProc, /*enableThrottling=*/true);
                    appliedEcoQos = true;
                  }
                }
                g_throttledProcesses[pid] = {hProc, prevPriority, prevIo,
                                             prevMem, appliedEcoQos, 0};
                hProc = nullptr; // Transferred ownership to
                                 // g_throttledProcesses: blocks PID reuse!
              }
            }
          }
        }
      }
    } else if (isThrottled && (isCpuCalm || systemContentionCleared)) {
      RestoreAndEraseThrottledProcess(pid);
    } else if (isThrottled && cpuPercent < 0) {
      // No CPU sample this cycle; after a few consecutive misses restore
      // the process instead of leaving it throttled forever.
      bool shouldRestore = false;
      {
        std::lock_guard<std::mutex> lock(g_priorityMutex);
        auto itTh = g_throttledProcesses.find(pid);
        if (itTh != g_throttledProcesses.end()) {
          if (++itTh->second.staleSampleCount >= 5) {
            shouldRestore = true;
          }
        }
      }
      if (shouldRestore) {
        RestoreAndEraseThrottledProcess(pid);
      }
    } else if (isThrottled) {
      std::lock_guard<std::mutex> lock(g_priorityMutex);
      auto itTh = g_throttledProcesses.find(pid);
      if (itTh != g_throttledProcesses.end()) {
        itTh->second.staleSampleCount = 0;
      }
    }

    if (hProc) {
      CloseHandle(hProc);
    }
  }

  std::vector<HANDLE> deadHandles;
  {
    std::lock_guard<std::mutex> lock(g_priorityMutex);
    for (auto it = g_throttledProcesses.begin();
         it != g_throttledProcesses.end();) {
      if (IsProcessTerminated(it->second.hProcess) ||
          alivePids.count(it->first) == 0) {
        deadHandles.push_back(it->second.hProcess);
        it = g_throttledProcesses.erase(it);
      } else {
        ++it;
      }
    }
  }
  for (HANDLE h : deadHandles) {
    if (h) {
      CloseHandle(h);
    }
  }

  PruneDeadPids(g_cpuSamples, alivePids);
  PruneDeadPids(g_ioSamples, alivePids);
  PruneDeadPids(g_aiLastInferenceTime, alivePids);
  PruneDeadPids(g_aiLastWorkingSetSize, alivePids);
}

// ---------------------------------------------------------------------------
// Fullscreen / Direct3D Game Detection
// ---------------------------------------------------------------------------

static bool IsExcludedFromGameDetection(const std::wstring &name) {
  static const std::vector<std::wstring> kNonGames = {
      L"chrome.exe", L"msedge.exe", L"firefox.exe", L"brave.exe",
      L"opera.exe", L"vivaldi.exe", L"zen.exe",
      L"powerpnt.exe", L"excel.exe", L"winword.exe", L"outlook.exe",
      L"mpv.exe", L"vlc.exe", L"wmplayer.exe", L"potplayer64.exe",
      L"mstsc.exe", L"teamviewer.exe", L"anydesk.exe",
      L"windowsterminal.exe", L"cmd.exe", L"powershell.exe", L"conhost.exe",
      L"explorer.exe"
  };
  return IsInList(name, kNonGames);
}

static bool IsLikelyGameOrFullscreenWindow(HWND hwnd, DWORD pid) {
  if (!hwnd || !IsWindowVisible(hwnd) || pid == 0)
    return false;

  RECT wndRect;
  if (!GetWindowRect(hwnd, &wndRect))
    return false;

  HMONITOR hMon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
  MONITORINFO mi{};
  mi.cbSize = sizeof(mi);
  if (!GetMonitorInfoW(hMon, &mi))
    return false;

  // Allow a +/- 2 pixel tolerance margin to accommodate DWM sizing borders
  // and multi-monitor mixed DPI scaling offsets on Windows 10/11.
  bool coversMonitor =
      (wndRect.left <= mi.rcMonitor.left + 2 &&
       wndRect.top <= mi.rcMonitor.top + 2 &&
       wndRect.right >= mi.rcMonitor.right - 2 &&
       wndRect.bottom >= mi.rcMonitor.bottom - 2);
  if (!coversMonitor)
    return false;

  // Direct3D exclusive fullscreen check (DirectX games)
  if (g_pfnSHQueryUserNotificationState) {
    QUERY_USER_NOTIFICATION_STATE quns = QUNS_NOT_PRESENT;
    if (SUCCEEDED(g_pfnSHQueryUserNotificationState(&quns))) {
      if (quns == QUNS_RUNNING_D3D_FULL_SCREEN) {
        return true;
      }
    }
  }

  // Fallback borderless / popup check:
  // Must verify that the owning process is not an excluded application
  // (browsers playing fullscreen video, office presentations, terminals, media players).
  std::vector<ProcessSnapshotEntry> snapshot = CaptureProcessSnapshotCached();
  for (const auto &entry : snapshot) {
    if (entry.pid == pid) {
      if (IsExcludedFromGameDetection(entry.name)) {
        return false;
      }
      break;
    }
  }

  LONG style = GetWindowLongW(hwnd, GWL_STYLE);
  bool borderless = (style & WS_CAPTION) == 0 && (style & WS_THICKFRAME) == 0;

  return borderless || (style & WS_POPUP) != 0;
}

static void MaybeRequestGameSweep(DWORD pid, HWND hwnd,
                                  const ModSettings &settings) {
  if (!settings.enableGameModeDetection || pid == 0 ||
      pid == g_lastGameSweepPid)
    return;
  if (!IsLikelyGameOrFullscreenWindow(hwnd, pid))
    return;

  auto now = std::chrono::steady_clock::now();
  auto elapsedMin = std::chrono::duration_cast<std::chrono::minutes>(
                        now - g_lastGameSweepTime)
                        .count();
  if (elapsedMin < 2)
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
               bool emergency = false,
               bool forceHardTrim = false) {
  TrimAttemptResult result;

  HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION |
                                 PROCESS_SET_INFORMATION | PROCESS_SET_QUOTA | PROCESS_VM_READ,
                             FALSE, pid);
  if (!hProc) {
    hProc = OpenProcess(PROCESS_SET_QUOTA | PROCESS_SET_INFORMATION | PROCESS_QUERY_LIMITED_INFORMATION,
                        FALSE, pid);
  }
  if (!hProc) {
    if (GetLastError() == ERROR_ACCESS_DENIED) {
      g_accessDeniedCount.fetch_add(1, std::memory_order_relaxed);
    }
    return result;
  }

  // Packaged (UWP/MSIX) apps are managed by Windows' Process Lifetime Manager;
  // leave them alone rather than fighting the OS and causing Start/Search
  // latency.
  if (IsPackagedApp(hProc)) {
    CloseHandle(hProc);
    return result;
  }

  // Modern OS-Native Memory Prioritization (Windows 8+):
  // Apply MEMORY_PRIORITY_VERY_LOW to background candidate apps so the OS balance set
  // manager naturally reclaims their physical pages first under memory pressure, without
  // forcing immediate disk page-outs when physical RAM is plentiful.
  SetProcessMemoryPriorityHint(hProc, 1 /* MEMORY_PRIORITY_VERY_LOW */);

  PROCESS_MEMORY_COUNTERS_EX pmc;
  ZeroMemory(&pmc, sizeof(pmc));
  pmc.cb = sizeof(pmc);

  if (GetProcessMemoryInfo(hProc,
                           reinterpret_cast<PROCESS_MEMORY_COUNTERS *>(&pmc),
                           sizeof(pmc))) {
    SIZE_T wsMb = pmc.WorkingSetSize / (1024 * 1024);
    bool isMemoryCapExceeded =
        settings.enableElectronMemoryCap &&
        (wsMb >= static_cast<SIZE_T>(settings.electronMemoryCapMb));

    // A physical working set eviction (SetProcessWorkingSetSize(-1, -1)) is only
    // performed if strictly necessary:
    // 1. Critical memory emergency (free RAM <= 5%).
    // 2. Explicit user panic hotkey / game sweep (forceHardTrim == true).
    // 3. Process is consuming memory beyond the configured memory cap (memory leak/hog).
    if (forceHardTrim || emergency || isMemoryCapExceeded) {
      if (wsMb >= settings.minProcessMemoryToTrimMb) {
        int cooldownSec = emergency ? 45 : 180;
        auto itTrim = g_processLastTrimmed.find(pid);
        if (itTrim != g_processLastTrimmed.end()) {
          auto diffSec = std::chrono::duration_cast<std::chrono::seconds>(
                             now - itTrim->second)
                             .count();
          if (diffSec < cooldownSec) {
            CloseHandle(hProc);
            return result;
          }
        }

        SIZE_T beforeBytes = pmc.WorkingSetSize;
        if (SetProcessWorkingSetSize(hProc, static_cast<SIZE_T>(-1),
                                     static_cast<SIZE_T>(-1))) {
          g_processLastTrimmed[pid] = now;
          g_aiLastWorkingSetSize.erase(pid);

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
  }

  CloseHandle(hProc);
  return result;
}

static TrimStats TrimBackgroundWorkingSets(const ModSettings &settings,
                                           DWORD foregroundPid,
                                           double freeRamPercent,
                                           bool emergency = false,
                                           bool hogsOnly = false,
                                           bool forceHardTrim = false) {
  TrimStats stats;
  auto now = std::chrono::steady_clock::now();

  bool isMultiTasking =
      settings.enableMultitaskingAdaptation && IsActiveMultiTaskingMode();

  int effectiveGraceMinutes = settings.recentActivityGraceMinutes;
  if (isMultiTasking) {
    effectiveGraceMinutes *= 4;
  } else if (freeRamPercent >= 30.0) {
    effectiveGraceMinutes *= 2;
  }
  int64_t graceSeconds = static_cast<int64_t>(effectiveGraceMinutes) * 60;

  std::unordered_set<DWORD> rawAudioPids;
  if (settings.enableAudioShielding) {
    rawAudioPids = GetActiveAudioProcessIdsCached(settings);
  }

  DWORD currentPid = GetCurrentProcessId();
  std::vector<ProcessSnapshotEntry> processList =
      CaptureProcessSnapshotCached();
  std::unordered_set<DWORD> alivePids;
  for (const auto &entry : processList) {
    alivePids.insert(entry.pid);
  }

  std::map<DWORD, std::vector<DWORD>> childrenOf;
  std::map<DWORD, DWORD> parentOf;
  std::map<DWORD, const ProcessSnapshotEntry *> byPid;
  for (const auto &entry : processList) {
    childrenOf[entry.parentPid].push_back(entry.pid);
    parentOf[entry.pid] = entry.parentPid;
    byPid[entry.pid] = &entry;
  }

  // Expand audio shielding to the whole process tree & executable family
  std::unordered_set<DWORD> activeAudioPids =
      ExpandAudioProcessShield(rawAudioPids, processList, childrenOf, parentOf);

  std::map<DWORD, WindowState> windowStates = BuildWindowStateMapCached();

  std::vector<AppTrimEntry> trimmedEntries;
  std::unordered_set<DWORD> handledPids;
  if (foregroundPid != 0) {
    handledPids.insert(foregroundPid);
    std::vector<DWORD> fgDescendants;
    std::unordered_set<DWORD> visited;
    CollectDescendants(foregroundPid, childrenOf, fgDescendants, visited);
    for (DWORD fgChild : fgDescendants) {
      handledPids.insert(fgChild);
    }
  }

  auto tryTrimAndRecord = [&](DWORD pid, const std::wstring &name) {
    TrimAttemptResult attempt =
        TryTrimProcess(pid, settings, now, emergency, forceHardTrim);
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

  auto shouldTrimProcess = [&](DWORD p, const std::wstring &n) -> bool {
    if (p == 0 || p == 4 || p == currentPid)
      return false;
    if (handledPids.count(p))
      return false;
    if (foregroundPid != 0 && p == foregroundPid)
      return false;
    if (settings.enableAudioShielding && activeAudioPids.count(p))
      return false;
    if (IsInList(n, settings.excludedProcesses) ||
        IsEssentialSystemSecurityProcess(n))
      return false;
    if (!IsInteractiveSessionProcess(p))
      return false;

    // Smart AI Engine Inactivity & Inference Shield
    if (settings.enableSmartAiOptimization && IsKnownAiProcess(n)) {
      auto itAi = g_aiLastInferenceTime.find(p);
      if (itAi == g_aiLastInferenceTime.end()) {
        // AI process with no activity record: protect it
        return false;
      }
      auto inactiveAiSec =
          std::chrono::duration_cast<std::chrono::seconds>(now - itAi->second)
              .count();
      if (inactiveAiSec <
          static_cast<int64_t>(settings.aiInactivityGraceMinutes) * 60) {
        // AI model generated tokens recently: keep memory 100% warm.
        return false;
      }
    }

    bool isTargetListed = IsInList(n, settings.customTargetList);
    if (settings.targetProcessesOnly && !isTargetListed)
      return false;

    auto wsIt = windowStates.find(p);
    bool hasVisibleWindow =
        wsIt != windowStates.end() && wsIt->second.hasVisibleWindow;
    bool isWindowMinimized =
        wsIt != windowStates.end() && wsIt->second.isMinimized;

    // If "Trim Minimized Windows" is disabled, do not trim minimized apps
    if (!settings.trimMinimizedWindows && isWindowMinimized) {
      return false;
    }

    bool isMinimized = isWindowMinimized;

    // Visible Window Protection:
    // Applications with a visible non-minimized window (e.g., on a secondary monitor
    // or side-by-side) must NEVER have their working set trimmed.
    if (hasVisibleWindow && !isMinimized) {
      return false;
    }

    bool graceEligible = true;
    if (!isMinimized && settings.enableProcessAging) {
      std::lock_guard<std::mutex> lock(g_focusMapMutex);
      auto itFocus = g_processLastFocusedTime.find(p);
      if (itFocus != g_processLastFocusedTime.end()) {
        auto inactiveSeconds =
            std::chrono::duration_cast<std::chrono::seconds>(now - itFocus->second)
                .count();
        if (inactiveSeconds < graceSeconds) {
          graceEligible = false;
        }
      }
    }

    bool capEligible = false;
    if (settings.enableElectronMemoryCap && isTargetListed &&
        p != foregroundPid && (!hasVisibleWindow || isMinimized)) {
      HANDLE hPeek = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, p);
      if (hPeek) {
        PROCESS_MEMORY_COUNTERS_EX pmc{};
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

    if (!graceEligible && !capEligible) {
      stats.processesSkippedRecent++;
      return false;
    }

    if (hogsOnly) {
      // In Tier 1 / Light Mode (e.g. 20% to 40% free RAM):
      // Only trim processes that are:
      // 1. In customTargetList or capEligible (heavy hogs / bloated apps)
      // 2. Minimized (not merely background)
      // 3. Deeply inactive (inactive for at least 15 minutes)
      if (!isTargetListed && !capEligible) {
        return false;
      }
      if (!isMinimized) {
        return false;
      }
      if (settings.enableProcessAging) {
        std::lock_guard<std::mutex> lock(g_focusMapMutex);
        auto itFocus = g_processLastFocusedTime.find(p);
        if (itFocus != g_processLastFocusedTime.end()) {
          auto inactiveSec =
              std::chrono::duration_cast<std::chrono::seconds>(now - itFocus->second)
                  .count();
          if (inactiveSec < 15 * 60) {
            return false;
          }
        }
      }
    }

    return true;
  };

  for (const auto &entry : processList) {
    DWORD pid = entry.pid;
    const std::wstring &procName = entry.name;
    if (!shouldTrimProcess(pid, procName))
      continue;

    handledPids.insert(pid);
    tryTrimAndRecord(pid, procName);

    if (settings.enableProcessTreeTrimming) {
      std::vector<DWORD> descendants;
      std::unordered_set<DWORD> visited;
      CollectDescendants(pid, childrenOf, descendants, visited);
      for (DWORD childPid : descendants) {
        auto childIt = byPid.find(childPid);
        if (childIt == byPid.end())
          continue;
        const std::wstring &childName = childIt->second->name;
        if (!shouldTrimProcess(childPid, childName))
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
    PruneDeadPids(g_processLastFocusedTime, alivePids);
  }
  PruneDeadPids(g_processLastTrimmed, alivePids);

  return stats;
}

// ---------------------------------------------------------------------------
// Cleanup Pass Dispatcher
// ---------------------------------------------------------------------------

static void PerformMemoryCleanup(const wchar_t *triggerReason,
                                 bool allowWorkingSetTrim = true,
                                 bool hogsOnly = false,
                                 bool forceHardTrim = false) {
  ModSettings settings = GetSettingsSnapshot();

  DWORD accessDenied = g_accessDeniedCount.exchange(0);
  if (accessDenied > 0 && settings.enableLogging) {
    Wh_Log(L"[SmartOptimizer] Notice: %u elevated/system processes skipped "
           L"(access denied; mod runs in user session).",
           accessDenied);
  }

  MEMORYSTATUSEX memBefore;
  memBefore.dwLength = sizeof(memBefore);
  GlobalMemoryStatusEx(&memBefore);

  double freeRamPercent =
      (static_cast<double>(memBefore.ullAvailPhys) / memBefore.ullTotalPhys) *
      100.0;
  DWORD fgPid = GetForegroundProcessId();

  // Below 5% free RAM, relax the anti-thrashing cooldowns so the mod can
  // react before the system runs out of memory.
  bool emergency = freeRamPercent <= 5.0;

  TrimStats trimStats;
  if (settings.cleanBackgroundWorkingSets && allowWorkingSetTrim) {
    trimStats = TrimBackgroundWorkingSets(settings, fgPid, freeRamPercent,
                                          emergency, hogsOnly, forceHardTrim);
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
        Wh_Log(L"[SmartOptimizer]   #%d. %s (PID %u): -%.1f MB (was %.1f MB -> "
               L"now "
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

static LRESULT CALLBACK PowerWndProc(HWND hwnd, UINT uMsg, WPARAM wParam,
                                        LPARAM lParam) {
  if (uMsg == WM_POWERBROADCAST) {
    if (wParam == PBT_APMSUSPEND) {
      Wh_Log(L"[SmartOptimizer] System entering sleep/suspend. Pausing engine "
             L"and restoring priorities...");
      g_systemSuspended.store(true);
      {
        std::lock_guard<std::mutex> lock(g_priorityMutex);
        RestoreForegroundBoostLocked();
      }
      RestoreAllThrottledProcesses();
    } else if (wParam == PBT_APMRESUMEAUTOMATIC ||
               wParam == PBT_APMRESUMESUSPEND) {
      Wh_Log(L"[SmartOptimizer] System resumed from sleep/suspend. Resetting "
             L"aging timers and re-evaluating foreground boost...");
      auto now = std::chrono::steady_clock::now();
      {
        std::lock_guard<std::mutex> lock(g_focusMapMutex);
        for (auto &entry : g_processLastFocusedTime) {
          entry.second = now;
        }
        g_focusSwitchHistory.clear();
      }
      g_systemSuspended.store(false);
      HWND hFg = GetForegroundWindow();
      if (hFg) {
        HandleForegroundChanged(hFg);
      }
      if (g_wakeEvent) {
        SetEvent(g_wakeEvent);
      }
    }
    return TRUE;
  }
  return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

static DWORD WINAPI HookThreadProc(LPVOID) {
  // Ensure message queue is created immediately so PostThreadMessageW never
  // fails
  MSG msg;
  PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

  WNDCLASSEXW wc{};
  wc.cbSize = sizeof(wc);
  wc.lpfnWndProc = PowerWndProc;
  wc.hInstance = GetModuleHandleW(nullptr);
  wc.lpszClassName = L"SmartOptimizerPowerMsgWnd";
  RegisterClassExW(&wc);

  HWND hPowerWnd =
      CreateWindowExW(0, wc.lpszClassName, nullptr, 0, 0, 0, 0, 0,
                      HWND_MESSAGE, nullptr, wc.hInstance, nullptr);
  HPOWERNOTIFY hPowerNotify = nullptr;
  if (hPowerWnd) {
    hPowerNotify = RegisterSuspendResumeNotification(
        hPowerWnd, DEVICE_NOTIFY_WINDOW_HANDLE);
  }

  g_winEventHook = SetWinEventHook(
      EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr, WinEventProc,
      0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);

  {
    ModSettings settings = GetSettingsSnapshot();
    if (settings.enablePanicHotkey) {
      g_panicHotkeyRegistered.store(
          RegisterHotKey(nullptr, kPanicHotkeyId,
                         MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, VK_F11));
    }
  }

  if (g_hookThreadReadyEvent) {
    SetEvent(g_hookThreadReadyEvent);
  }

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
      if (msg.wParam == 1 && !g_panicHotkeyRegistered.load()) {
        g_panicHotkeyRegistered.store(
            RegisterHotKey(nullptr, kPanicHotkeyId,
                           MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, VK_F11));
      } else if (msg.wParam == 0 && g_panicHotkeyRegistered.load()) {
        UnregisterHotKey(nullptr, kPanicHotkeyId);
        g_panicHotkeyRegistered.store(false);
      }
    }

    TranslateMessage(&msg);
    DispatchMessageW(&msg);
  }

  if (g_panicHotkeyRegistered.load()) {
    UnregisterHotKey(nullptr, kPanicHotkeyId);
    g_panicHotkeyRegistered.store(false);
  }
  if (g_winEventHook) {
    UnhookWinEvent(g_winEventHook);
    g_winEventHook = nullptr;
  }
  if (hPowerNotify) {
    UnregisterSuspendResumeNotification(hPowerNotify);
    hPowerNotify = nullptr;
  }
  if (hPowerWnd) {
    DestroyWindow(hPowerWnd);
    hPowerWnd = nullptr;
  }
  UnregisterClassW(wc.lpszClassName, wc.hInstance);
  return 0;
}

// ---------------------------------------------------------------------------
// Watchdog Worker Thread
// ---------------------------------------------------------------------------

static void MemoryOptimizerWorker() {
  HRESULT hrCom = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

  Wh_Log(L"[SmartOptimizer] Adaptive Memory & Priority Optimizer engine "
         L"started "
         L"(0.00%% CPU passive wait).");

  HANDLE waitHandles[2] = {g_stopEvent, g_wakeEvent};
  g_modStartTime = std::chrono::steady_clock::now();
  g_lastPeriodicCleanTime = g_modStartTime;
  g_lastIdleCleanTime = g_modStartTime;

  while (g_workerRunning.load()) {
    if (g_systemSuspended.load()) {
      DWORD waitRes = WaitForSingleObject(g_stopEvent, 1000);
      if (waitRes == WAIT_OBJECT_0) {
        break;
      }
      continue;
    }

    ModSettings settings = GetSettingsSnapshot();

    bool onBattery = settings.pauseOnBattery && IsRunningOnBattery();

    if (g_forceCleanupRequested.exchange(false)) {
      PerformMemoryCleanup(L"[Panic Hotkey Trigger]",
                           /*allowWorkingSetTrim=*/true, /*hogsOnly=*/false,
                           /*forceHardTrim=*/true);
    }
    if (g_gameSweepRequested.exchange(false)) {
      Wh_Log(L"[SmartOptimizer] Fullscreen / 3D game detected. Running "
             L"preventive RAM sweep...");
      PerformMemoryCleanup(L"[Pre-Game Sweep]", /*allowWorkingSetTrim=*/true,
                           /*hogsOnly=*/false, /*forceHardTrim=*/true);
    }

    bool hookActive = (g_winEventHook != nullptr);
    if (!hookActive) {
      DWORD fgPid = GetForegroundProcessId();
      if (fgPid != 0) {
        RecordFocusSwitch(fgPid);
        UpdateForegroundBoost(fgPid, settings);
      }
    }

    if (!onBattery) {
      UpdateAiProcessActivity(settings);
      ApplyBackgroundThrottling(settings, GetForegroundProcessId());
    } else {
      RestoreAllThrottledProcesses();
    }

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
        bool hogsOnly = false;
        if (settings.cleanMode == CleanMode::SmartThreshold ||
            settings.cleanMode == CleanMode::SmartAndPeriodic) {
          if (freePercent <= settings.freeRamThresholdPercent &&
              triggerCooldownElapsed) {
            shouldClean = true;
            hogsOnly = false;
            wchar_t buf[128];
            swprintf_s(buf, L"[Standard Threshold: Free RAM %.1f%% <= %d%%]",
                       freePercent, settings.freeRamThresholdPercent);
            reason = buf;
            g_lastTriggerCleanTime = now;
          } else if (settings.enableTieredRamThreshold &&
                     freePercent <= settings.tieredHogThresholdPercent &&
                     triggerCooldownElapsed) {
            shouldClean = true;
            hogsOnly = true;
            wchar_t buf[128];
            swprintf_s(
                buf,
                L"[Tiered Threshold: Free RAM %.1f%% <= %d%% (Inactive Hogs Only)]",
                freePercent, settings.tieredHogThresholdPercent);
            reason = buf;
            g_lastTriggerCleanTime = now;
          }
        }

        // 2. Periodic timer trigger (gated on memory pressure).
        if (!shouldClean &&
            (settings.cleanMode == CleanMode::Periodic ||
             settings.cleanMode == CleanMode::SmartAndPeriodic)) {
          auto elapsedMinutes =
              std::chrono::duration_cast<std::chrono::minutes>(
                  now - g_lastPeriodicCleanTime)
                  .count();
          if (elapsedMinutes >= settings.periodicIntervalMinutes) {
            if (freePercent <= settings.freeRamThresholdPercent) {
              shouldClean = true;
              hogsOnly = false;
              wchar_t buf[128];
              swprintf_s(buf,
                         L"[Periodic Trigger: %d min interval (Free RAM %.1f%% "
                         L"<= %d%%)]",
                         settings.periodicIntervalMinutes, freePercent,
                         settings.freeRamThresholdPercent);
              reason = buf;
            }
            g_lastPeriodicCleanTime = now;
          }
        }

        // 3. Smart Idle Trigger: triggers upon entering idle state,
        // then throttles subsequent sweeps to a relaxed cadence of at least
        // 10 minutes (or the configured periodic interval if higher),
        // gated on memory pressure to prevent SSD churn when plenty of RAM is
        // free.
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
            if (freePercent <= settings.freeRamThresholdPercent) {
              shouldClean = true;
              hogsOnly = false;
              wchar_t buf[128];
              swprintf_s(buf,
                         L"[Idle Trigger: idle for %u min (Free RAM %.1f%% <= "
                         L"%d%%)]",
                         idleSec / 60, freePercent,
                         settings.freeRamThresholdPercent);
              reason = buf;
              g_lastIdleCleanTime = now;
              g_lastTriggerCleanTime = now;
            }
          }
        }

        if (!isSystemIdle) {
          g_wasIdle = false;
        } else if (shouldClean) {
          g_wasIdle = true;
        }

        if (shouldClean) {
          PerformMemoryCleanup(reason.c_str(), /*allowWorkingSetTrim=*/true,
                               hogsOnly, /*forceHardTrim=*/false);
        }
      }
    }

    DWORD waitSec = (DWORD)std::clamp(settings.checkIntervalSec, 1, 60);
    DWORD waitRes =
        WaitForMultipleObjects(2, waitHandles, FALSE, waitSec * 1000);

    if (waitRes == WAIT_OBJECT_0) {
      break;
    } else if (waitRes == WAIT_OBJECT_0 + 1) {
      ResetEvent(g_wakeEvent);
    }
  }

  {
    std::lock_guard<std::mutex> lock(g_priorityMutex);
    RestoreForegroundBoostLocked();
  }

  RestoreAllThrottledProcesses();

  Wh_Log(
      L"[SmartOptimizer] Memory & Priority Optimizer engine stopped cleanly.");

  if (hrCom == S_OK || hrCom == S_FALSE) {
    CoUninitialize();
  }
}

// ---------------------------------------------------------------------------
// Settings Loader
// ---------------------------------------------------------------------------

static void LoadSettings() {
  std::lock_guard<std::mutex> lock(g_settingsMutex);

  // Section 1: Foreground Responsiveness & CPU Balancing
  g_settings.enableProBalance = Wh_GetIntSetting(L"enableProBalance") != 0;

  auto prioStr = WindhawkUtils::StringSetting::make(L"foregroundPriorityLevel");
  if (prioStr.get() && wcscmp(prioStr.get(), L"high") == 0) {
    g_settings.foregroundPriorityLevel = ForegroundPrioritySetting::High;
  } else {
    g_settings.foregroundPriorityLevel = ForegroundPrioritySetting::AboveNormal;
  }

  g_settings.enableForegroundCpuSets =
      Wh_GetIntSetting(L"enableForegroundCpuSets") != 0;

  g_settings.enableBackgroundThrottling =
      Wh_GetIntSetting(L"enableBackgroundThrottling") != 0;

  int bgThrottle =
      (int)Wh_GetIntSetting(L"backgroundCpuThrottleThresholdPercent");
  g_settings.backgroundCpuThrottleThresholdPercent =
      std::clamp(bgThrottle, 5, 50);

  int sysContention =
      (int)Wh_GetIntSetting(L"systemCpuContentionThresholdPercent");
  g_settings.systemCpuContentionThresholdPercent =
      std::clamp(sysContention, 0, 95);

  auto bgPrioStr =
      WindhawkUtils::StringSetting::make(L"backgroundThrottlePriorityLevel");
  if (bgPrioStr.get() && wcscmp(bgPrioStr.get(), L"idle") == 0) {
    g_settings.backgroundThrottlePriorityLevel = ThrottlePrioritySetting::Idle;
  } else {
    g_settings.backgroundThrottlePriorityLevel =
        ThrottlePrioritySetting::BelowNormal;
  }

  g_settings.enableEcoQosManagement =
      Wh_GetIntSetting(L"enableEcoQosManagement") != 0;

  // Section 2: Smart Memory Management
  int thresh = (int)Wh_GetIntSetting(L"freeRamThresholdPercent");
  g_settings.freeRamThresholdPercent = std::clamp(thresh, 5, 50);

  g_settings.enableTieredRamThreshold =
      Wh_GetIntSetting(L"enableTieredRamThreshold") != 0;

  int tieredThresh = (int)Wh_GetIntSetting(L"tieredHogThresholdPercent");
  g_settings.tieredHogThresholdPercent = std::clamp(tieredThresh, 10, 80);

  int graceMin = (int)Wh_GetIntSetting(L"recentActivityGraceMinutes");
  g_settings.recentActivityGraceMinutes = std::clamp(graceMin, 1, 60);

  g_settings.trimMinimizedWindows =
      Wh_GetIntSetting(L"trimMinimizedWindows") != 0;

  g_settings.enableElectronMemoryCap =
      Wh_GetIntSetting(L"enableElectronMemoryCap") != 0;

  int capMb = (int)Wh_GetIntSetting(L"electronMemoryCapMb");
  g_settings.electronMemoryCapMb = std::clamp(capMb, 100, 4000);

  g_settings.enableIdleBoost = Wh_GetIntSetting(L"enableIdleBoost") != 0;

  // Section 3: Hardware & Workload Protection
  g_settings.enableSmartAiOptimization =
      Wh_GetIntSetting(L"enableSmartAiOptimization") != 0;

  int aiGraceMin = (int)Wh_GetIntSetting(L"aiInactivityGraceMinutes");
  g_settings.aiInactivityGraceMinutes = std::clamp(aiGraceMin, 1, 30);

  g_settings.pauseOnBattery = Wh_GetIntSetting(L"pauseOnBattery") != 0;

  // Section 4: Process Lists & Diagnostics
  auto customListStr = WindhawkUtils::StringSetting::make(L"customTargetList");
  g_settings.customTargetList =
      ParseProcessList(customListStr.get() ? customListStr.get() : L"");

  auto exclListStr = WindhawkUtils::StringSetting::make(L"excludedProcesses");
  g_settings.excludedProcesses =
      ParseProcessList(exclListStr.get() ? exclListStr.get() : L"");

  g_settings.enablePanicHotkey = Wh_GetIntSetting(L"enablePanicHotkey") != 0;
  g_settings.enableLogging = Wh_GetIntSetting(L"enableLogging") != 0;

  // Hardcoded core optimizations & safety guards (essential system features):
  g_settings.enableAudioShielding = true;
  g_settings.enableMultitaskingAdaptation = true;
  g_settings.enableGameModeDetection = true;
  g_settings.cleanMode = CleanMode::SmartThreshold;
  g_settings.idleThresholdMinutes = 15;
  g_settings.enableProcessAging = true;
  g_settings.enableProcessTreeTrimming = true;
  g_settings.cleanBackgroundWorkingSets = true;
  g_settings.minProcessMemoryToTrimMb = 50;
  g_settings.periodicIntervalMinutes = 10;
  g_settings.targetProcessesOnly = false;
  g_settings.checkIntervalSec = 10;

  // Hardware-Aware Auto-Tuning:
  // Automatically adapt thresholds to the machine's physical hardware capacity.
  SystemHardwareProfile hw = GetHardwareProfile();
  if (hw.isLowRamTier) {
    if (g_settings.freeRamThresholdPercent < 25) {
      g_settings.freeRamThresholdPercent = 25;
    }
    if (g_settings.electronMemoryCapMb > 350) {
      g_settings.electronMemoryCapMb = 350;
    }
  }
  if (hw.isLowCoreCount) {
    if (g_settings.systemCpuContentionThresholdPercent > 50) {
      g_settings.systemCpuContentionThresholdPercent = 50;
    }
  }
}

// ---------------------------------------------------------------------------
// Mod Lifecycle Entry Points (Tool Mod)
// ---------------------------------------------------------------------------

BOOL WhTool_ModInit() {
  Wh_Log(
      L"[SmartOptimizer] Initializing Smart Process Priority & RAM Optimizer "
      L"(Dedicated Tool Process)...");

  SystemHardwareProfile hw = GetHardwareProfile();
  Wh_Log(L"[SmartOptimizer] Hardware Profile: %.1f GB RAM (%s), %u CPU cores (%s)%s.",
         hw.totalRamGb,
         hw.isLowRamTier ? L"Low-RAM Tier / iGPU Buffer Elevated"
                         : (hw.isHighRamTier ? L"High-RAM Tier" : L"Standard RAM Tier"),
         hw.coreCount,
         hw.isLowCoreCount ? L"Aggressive Contention Guard" : L"Standard Contention Guard",
         hw.isHybridCpu ? L", Intel/AMD Hybrid P/E-Cores Active" : L"");

  HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
  if (hNtdll) {
    g_pfnNtSetInformationProcess = (pfnNtSetInformationProcess)GetProcAddress(
        hNtdll, "NtSetInformationProcess");
    g_pfnNtQueryInformationProcess =
        (pfnNtQueryInformationProcess)GetProcAddress(
            hNtdll, "NtQueryInformationProcess");
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
  g_hookThreadReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);

  if (!g_stopEvent || !g_wakeEvent || !g_hookThreadReadyEvent) {
    Wh_Log(L"[SmartOptimizer] Fatal Error: Failed to create synchronization "
           L"events.");
    return FALSE;
  }

  g_hookThreadRunning.store(true);
  DWORD hookThreadId = 0;
  g_hookThreadHandle =
      CreateThread(nullptr, 0, HookThreadProc, nullptr, 0, &hookThreadId);
  g_hookThreadId = hookThreadId;
  if (g_hookThreadHandle) {
    WaitForSingleObject(g_hookThreadReadyEvent, 3000);
  } else {
    Wh_Log(L"[SmartOptimizer] Warning: Failed to start event-hook thread; "
           L"falling back to polling-only detection.");
    g_hookThreadRunning.store(false);
  }

  g_workerRunning.store(true);
  g_workerThread.emplace(MemoryOptimizerWorker);

  Wh_Log(L"[SmartOptimizer] Mod initialized successfully.");
  return TRUE;
}

void WhTool_ModSettingsChanged() {
  Wh_Log(L"[SmartOptimizer] Settings updated. Reloading configuration...");
  LoadSettings();

  ModSettings settings = GetSettingsSnapshot();

  // If foreground boost was disabled, restore boosted process immediately.
  // Note: Background throttling restoration is handled safely by the worker
  // thread upon waking to avoid cross-thread data races on throttled processes.
  if (!settings.enableProBalance) {
    std::lock_guard<std::mutex> lock(g_priorityMutex);
    RestoreForegroundBoostLocked();
  }

  if (g_hookThreadRunning.load() && g_hookThreadId != 0) {
    if (settings.enablePanicHotkey && !g_panicHotkeyRegistered.load()) {
      PostThreadMessageW(g_hookThreadId, WM_APP, 1, 0);
    } else if (!settings.enablePanicHotkey && g_panicHotkeyRegistered.load()) {
      PostThreadMessageW(g_hookThreadId, WM_APP, 0, 0);
    }
  }

  if (g_wakeEvent) {
    SetEvent(g_wakeEvent);
  }
}

static HANDLE g_parentProcessHandle = nullptr;
static HANDLE g_parentWaitHandle = nullptr;
static HANDLE g_hChildJob = nullptr;

static void CALLBACK ParentProcessTerminatedCallback(PVOID lpParameter,
                                                     BOOLEAN TimerOrWaitFired) {
  // Parent windhawk.exe process terminated or crashed; terminate child to
  // prevent orphaned background execution.
  (void)lpParameter;
  (void)TimerOrWaitFired;
  ExitProcess(0);
}

void WhTool_ModUninit() {
  Wh_Log(L"[SmartOptimizer] Deinitializing mod...");

  if (g_hookThreadRunning.load()) {
    g_hookThreadRunning.store(false);
    if (g_hookThreadHandle) {
      if (g_hookThreadId != 0) {
        PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
      }
      WaitForSingleObject(g_hookThreadHandle, INFINITE);
      CloseHandle(g_hookThreadHandle);
      g_hookThreadHandle = nullptr;
    }
  }

  if (g_workerRunning.load()) {
    g_workerRunning.store(false);
    if (g_stopEvent) {
      SetEvent(g_stopEvent);
    }
    if (g_workerThread && g_workerThread->joinable()) {
      g_workerThread->join();
    }
    g_workerThread.reset();
  }

  RestoreAllThrottledProcesses();
  g_audioPidLastActive.clear();
  g_audioPidsCache.clear();
  {
    std::lock_guard<std::mutex> lock(g_priorityMutex);
    RestoreForegroundBoostLocked();
  }

  if (g_stopEvent) {
    CloseHandle(g_stopEvent);
    g_stopEvent = nullptr;
  }
  if (g_wakeEvent) {
    CloseHandle(g_wakeEvent);
    g_wakeEvent = nullptr;
  }
  if (g_hookThreadReadyEvent) {
    CloseHandle(g_hookThreadReadyEvent);
    g_hookThreadReadyEvent = nullptr;
  }
  if (g_parentWaitHandle) {
    UnregisterWait(g_parentWaitHandle);
    g_parentWaitHandle = nullptr;
  }
  if (g_parentProcessHandle) {
    CloseHandle(g_parentProcessHandle);
    g_parentProcessHandle = nullptr;
  }

  Wh_Log(L"[SmartOptimizer] Mod unloaded cleanly.");
}

// ---------------------------------------------------------------------------
// Tool Mod Process Bootstrap
// ---------------------------------------------------------------------------
// This mod runs as a dedicated tool-mod process rather than injecting into
// other processes: Wh_ModInit re-launches windhawk.exe with "-tool-mod
// <mod-id>" and hooks its entry point, and that relaunched process drives the
// actual WhTool_ModInit / WhTool_ModSettingsChanged / WhTool_ModUninit
// lifecycle defined above.

bool g_isToolModProcessLauncher = false;
HANDLE g_toolModProcessMutex = nullptr;

void WINAPI EntryPoint_Hook() { ExitThread(0); }

BOOL Wh_ModInit() {
  DWORD sessionId;
  if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
      sessionId == 0) {
    return FALSE;
  }

  bool isExcluded = false;
  bool isToolModProcess = false;
  bool isCurrentToolModProcess = false;
  DWORD parentPid = 0;
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
    } else if (wcscmp(argv[i], L"-parent-pid") == 0) {
      parentPid = wcstoul(argv[i + 1], nullptr, 10);
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

    if (parentPid != 0) {
      g_parentProcessHandle = OpenProcess(SYNCHRONIZE, FALSE, parentPid);
      if (g_parentProcessHandle) {
        RegisterWaitForSingleObject(
            &g_parentWaitHandle, g_parentProcessHandle,
            ParentProcessTerminatedCallback, nullptr, INFINITE,
            WT_EXECUTEONLYONCE);
      }
    }

    if (!WhTool_ModInit()) {
      ExitProcess(1);
    }

    IMAGE_DOS_HEADER *dosHeader = (IMAGE_DOS_HEADER *)GetModuleHandle(nullptr);
    IMAGE_NT_HEADERS *ntHeaders =
        (IMAGE_NT_HEADERS *)((BYTE *)dosHeader + dosHeader->e_lfanew);

    DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
    void *entryPoint = (BYTE *)dosHeader + entryPointRVA;

    using EntryPoint_t = void(WINAPI *)();
    WindhawkUtils::SetFunctionHook((EntryPoint_t)entryPoint, EntryPoint_Hook,
                                   nullptr);
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

  WCHAR commandLine[MAX_PATH + 128];
  swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\" -parent-pid %lu",
             currentProcessPath, WH_MOD_ID, GetCurrentProcessId());

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

  STARTUPINFO si{};
  si.cb = sizeof(STARTUPINFO);
  si.dwFlags = STARTF_FORCEOFFFEEDBACK;
  PROCESS_INFORMATION pi;
  if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                               nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                               nullptr, nullptr, &si, &pi, nullptr)) {
    Wh_Log(L"CreateProcess failed");
    return;
  }

  HANDLE hJob = CreateJobObject(nullptr, nullptr);
  if (hJob) {
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli{};
    jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    SetInformationJobObject(hJob, JobObjectExtendedLimitInformation, &jeli,
                            sizeof(jeli));
    AssignProcessToJobObject(hJob, pi.hProcess);
    g_hChildJob = hJob;
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
    if (g_hChildJob) {
      CloseHandle(g_hChildJob);
      g_hChildJob = nullptr;
    }
    return;
  }

  WhTool_ModUninit();
  ExitProcess(0);
}
