// ==WindhawkMod==
// @id              smart-process-priority-ram-optimizer
// @name            Smart Process Priority & RAM Optimizer
// @description     Boosts foreground responsiveness, shields audio, network, and AI workloads, throttles runaway background CPU, and safely reclaims idle memory.
// @version         3.8.0
// @author          gilnett
// @github          https://github.com/gilnett
// @include         windhawk.exe
// @compilerOptions -lpsapi -lole32 -lshell32 -ldwmapi -ladvapi32
// @donateUrl       https://ko-fi.com/gilnet
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Smart Process Priority & RAM Optimizer

Dynamically manages process priorities and memory allocation to keep foreground applications responsive and prevent background tasks from causing system hitches.

## Expectations
- **What it does**: Mitigates micro-stutters and input latency spikes caused by background processes competing for CPU resources.
- **What it does not do**: It does not increase maximum hardware performance or raw gaming FPS.

## Key Features

### 1. Foreground Priority Boost
- Elevates the CPU priority of the active window and its related child processes.
- Optionally assigns the active application to performance cores on hybrid CPU architectures.

### 2. Background Throttling
- Temporarily reduces the CPU priority of background processes exceeding defined utilization thresholds.
- Optionally applies deep thread-level I/O background mode (`THREAD_MODE_BACKGROUND_BEGIN`) to minimize disk I/O and SSD latency.
- Optionally restricts throttled tasks to efficiency cores or applies Windows EcoQoS.

### 3. Audio & Network Protection
- Shields processes with active audio playback from throttling and memory trimming.
- Shields processes performing active network transfers from throttling.

### 4. Compute & AI Workload Protection
- Prevents throttling of intensive background computation and model inference tasks.

### 5. Memory Optimization
- Gradually trims unused working set memory from idle applications via Windows memory APIs.
- Automatically suspends trimming on battery power and during post-sleep transitions.
- Includes a configurable hotkey (`Ctrl+Alt+F11`) for manual memory cleanup.

### 6. Dynamic Power Scheme
- Automatically engages the High Performance power plan during demanding workloads on AC power.
- Restores the original system power plan when returning to the desktop or entering sleep.

### 7. Inactive Browser Tab Trimming
- Detects dormant background browser renderers and trims their unused memory while keeping active tabs intact.

### 8. Passive Environment Diagnostics
- Passively detects and logs Hardware-Accelerated GPU Scheduling (HAGS), Virtualization-Based Security (VBS/Core Isolation), and Memory Compression status.

## Compatibility
- Supported OS: Windows 10 (version 1809 and later) and Windows 11 (all versions including 24H2).
- Supported Architectures: x86 (32-bit), x64 (64-bit), and ARM64.
- Works seamlessly on standard and hybrid CPU topologies (Intel Core 12th+ Gen P/E-cores and AMD heterogeneous architectures).

## Support
If you find this mod useful, you can support its development and maintenance:
- [Support on Ko-fi](https://ko-fi.com/gilnet)

## Credits & Acknowledgments
- **Process Lasso (Bitsum)**: Inspiration for the ProBalance concept and foreground responsiveness prioritization.
- **LiveTuner (LT)**: Inspiration for dynamic real-time priority tuning heuristics.
- **ISLC (Intelligent Standby List Cleaner by Wagnardsoft)**: Inspiration for adaptive threshold triggers and gaming memory management.
- **Mem Reduct (Henry++)**: Inspiration for safe working-set trimming techniques.
*/
// ==/WindhawkModReadme==

// clang-format off
// ==WindhawkModSettings==
/*
- enableProBalance: true
  $name: 1. Foreground Priority Boost (ProBalance)
  $description: Automatically increases the CPU priority of the active foreground window family for maximum responsiveness.
- foregroundPriorityLevel: "aboveNormal"
  $name: Foreground Priority Level
  $description: Priority class assigned to the active foreground application.
  $options:
    - "aboveNormal": Above Normal (Balanced & Safe)
    - "high": High (Maximum Performance)
- enableForegroundCpuSets: false
  $name: Suggest P-Cores to Foreground Window
  $description: Directs the active foreground app to Performance Cores (P-cores) on hybrid Intel/AMD CPUs to eliminate micro-stutter.
- enableDynamicPowerPlan: false
  $name: Dynamic High-Performance Power Scheme
  $description: Automatically engages the High Performance power plan while a heavy application or game is active on AC power, and restores your plan on desktop.
- enableBackgroundThrottling: true
  $name: 2. Throttle CPU-Heavy Background Processes
  $description: Temporarily lowers the priority of background processes consuming excessive CPU while you are working.
- enableThreadBackgroundMode: false
  $name: Deep Background I/O & CPU Mode
  $description: Places threads of throttled background processes into Windows background mode (THREAD_MODE_BACKGROUND_BEGIN) to minimize disk I/O and SSD latency.
- enableBackgroundCpuSets: false
  $name: Restrict Throttled Background Apps to E-Cores
  $description: Confines throttled background tasks to Efficiency Cores (E-cores) on hybrid Intel/AMD CPUs, reserving P-cores for your active app.
- enableEcoQosManagement: false
  $name: Windows Efficiency Mode (EcoQoS)
  $description: Applies Windows power throttling to background processes, scheduling them on efficiency cores.
- backgroundCpuThrottleThresholdPercent: 15
  $name: Background CPU Threshold (%)
  $description: Percentage of CPU usage required to throttle a background process (5% to 50%).
- systemCpuContentionThresholdPercent: 60
  $name: System Contention Trigger Threshold (%)
  $description: Total system CPU usage (load) required before background throttling can engage (e.g. 60% CPU in use, not idle).
- backgroundThrottlePriorityLevel: "belowNormal"
  $name: Background Throttle Priority Level
  $description: Priority class applied to throttled background processes.
  $options:
    - "belowNormal": Below Normal (Recommended)
    - "idle": Idle (Strict Throttling)
- enableAudioShielding: true
  $name: 3. Audio & Music Protection Shield
  $description: Shields applications actively streaming or playing audio (Spotify, YouTube, DAWs, games) against throttling and memory sweeps.
- enableNetworkShielding: true
  $name: Network Activity Shield (Streaming & Downloads)
  $description: Protects background applications actively downloading files, streaming media, or handling VoIP calls.
- enableSmartAiOptimization: true
  $name: Protect Local AI & Shader Compilers
  $description: Prevents throttling or memory trimming during local AI generation (Ollama, LM Studio) or background shader compilation.
- customAiProcesses: ""
  $name: Custom AI & Model Processes
  $description: Optional comma-separated list of executable names to treat as local AI workloads.
- freeRamThresholdPercent: 0
  $name: 4. Standard Free RAM Threshold (%)
  $description: Percentage of available physical memory below which background memory cleanup is triggered (0 = Auto-Adaptive, 5% to 80%). Automatically adapts thresholds to your installed RAM (e.g. cleans at 60% free RAM on 32 GB).
- trimMinimizedWindows: true
  $name: Clean Minimized Windows
  $description: Reclaims unused memory in the background from applications minimized to the taskbar.
- enableBrowserTabTrim: true
  $name: Inactive Browser Tabs Memory Trim
  $description: Safely reclaims unused memory from dormant background browser renderer tabs without closing tabs or affecting active ones.
- pauseOnBattery: true
  $name: Pause on Battery Power
  $description: Suspends memory cleanups and background throttling on battery to maximize laptop battery life.
- enableElectronMemoryCap: true
  $name: Early Memory Cap for Heavy Apps
  $description: Reclaims memory from listed heavy apps once their memory exceeds the configured cap, without waiting for system-wide RAM pressure.
- electronMemoryCapMb: 300
  $name: Heavy Apps Memory Cap (MB)
  $description: Maximum memory threshold before an inactive listed application is cleaned (100 to 2000 MB).
- customTargetList: "zen.exe, chrome.exe, msedge.exe, brave.exe, firefox.exe, opera.exe, vivaldi.exe, discord.exe, slack.exe, teams.exe, telegram.exe, whatsapp.exe, signal.exe, skype.exe, spotify.exe, steam.exe, epicgameslauncher.exe, code.exe, obs64.exe"
  $name: Heavy Memory Hogs Process List
  $description: All background apps are optimized when RAM drops; this list is only for preemptive memory trimming on heavy web/Electron apps exceeding the cap above.
- enablePanicHotkey: true
  $name: 5. Emergency Clean Hotkey (Ctrl+Alt+F11)
  $description: Enables the Ctrl+Alt+F11 shortcut to trigger an immediate memory reclamation pass.
- enableHotkeySound: true
  $name: Emergency Hotkey Audio Feedback
  $description: Emits a subtle confirmation chime when memory reclamation initiated by Ctrl+Alt+F11 is completely finished.
- excludedProcesses: "explorer.exe, windhawk.exe, dwm.exe, csrss.exe, lsass.exe, smss.exe, services.exe, system, wininit.exe, winlogon.exe, logonui.exe, lockapp.exe, consent.exe, credentialuibroker.exe, smartscreen.exe, svchost.exe, memcompression, registry, fontdrvhost.exe, audiodg.exe, sihost.exe, taskhostw.exe, ctfmon.exe, wlanext.exe, dashost.exe, syntpenh.exe, syntphelper.exe, etdcontrol.exe, etdctrl.exe, alpspad.exe, hcontrol.exe, wireguard.exe, openvpn.exe, tailscale.exe, splwow64.exe, printfilterpipelinesvc.exe, spoolsv.exe, wudfhost.exe, devicecensus.exe"
  $name: Excluded Processes (Immunity List)
  $description: Comma-separated list of executable names that must never be throttled or trimmed.
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
#include <cstdint>
#include <deque>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// ---------------------------------------------------------------------------
// NTDLL & Low-Level Definitions
// ---------------------------------------------------------------------------

typedef NTSTATUS(NTAPI* pfnNtQueryInformationProcess)(
    HANDLE ProcessHandle, INT ProcessInformationClass, PVOID ProcessInformation,
    ULONG ProcessInformationLength, PULONG ReturnLength);

typedef NTSTATUS(NTAPI* pfnNtSetInformationProcess)(
    HANDLE ProcessHandle, INT ProcessInformationClass, PVOID ProcessInformation,
    ULONG ProcessInformationLength);

typedef HRESULT(WINAPI* pfnSHQueryUserNotificationState)(
    QUERY_USER_NOTIFICATION_STATE* pquns);

static constexpr INT ProcessIoPriorityInfoClass = 33;
enum IoPriorityHint : uint8_t {
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
#define PROCESS_POWER_THROTTLING_EXECUTION_SPEED 0x1u
#endif
#ifndef PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION
#define PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION 0x4u
#endif
#ifndef DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 ((DPI_AWARENESS_CONTEXT) - 4)
#endif
static constexpr INT ProcessPowerThrottlingInfoClass = 4;

typedef BOOL(WINAPI* pfnGetSystemCpuSetInformation)(
    PSYSTEM_CPU_SET_INFORMATION Information, ULONG BufferLength,
    PULONG ReturnedLength, HANDLE Process, ULONG Flags);

typedef BOOL(WINAPI* pfnSetProcessDefaultCpuSets)(
    HANDLE Process, const ULONG* CpuSetIds, ULONG CpuSetIdCount);

static pfnNtQueryInformationProcess g_pfnNtQueryInformationProcess = nullptr;
static pfnNtSetInformationProcess g_pfnNtSetInformationProcess = nullptr;
static pfnSHQueryUserNotificationState g_pfnSHQueryUserNotificationState =
    nullptr;
static pfnGetSystemCpuSetInformation g_pfnGetSystemCpuSetInformation = nullptr;
static pfnSetProcessDefaultCpuSets g_pfnSetProcessDefaultCpuSets = nullptr;

static constexpr UINT_PTR kPanicHotkeyId = 0xA1CE;

// Hysteresis cooldown and memory growth thresholds between cleanup passes
static constexpr int kTriggerCooldownSec = 180;
static constexpr DWORDLONG kMinRamGrowthForReevaluationBytes =
    500ULL * 1024ULL * 1024ULL;
static constexpr int kLogTopAppsCount = 5;
static constexpr int kTopAppsLogThresholdMb = 500;

// ---------------------------------------------------------------------------
// Priority Class Ranking & CPU Core Helpers
// ---------------------------------------------------------------------------
// Win32 priority class values are non-monotonic bit flags. This helper maps
// them to a linear rank for relative comparisons.
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
    bool isLowRamTier = false;              // < 12 GB
    bool isHighRamTier = false;             // >= 24 GB
    bool isLowCoreCount = false;            // <= 4 cores
    bool isHybridCpu = false;               // Heterogeneous CPU architecture
    bool isHagsEnabled = false;             // Hardware-Accelerated GPU Scheduling
    bool isVbsEnabled = false;              // Virtualization-Based Security (Core Isolation)
    bool isMemoryCompressionActive = false; // Windows Memory Compression
    std::vector<ULONG> pCoreCpuSetIds;      // Performance core IDs
    std::vector<ULONG> eCoreCpuSetIds;      // Efficiency core IDs
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

        HKEY hKeyGfx = nullptr;
        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
                          L"SYSTEM\\CurrentControlSet\\Control\\GraphicsDrivers",
                          0, KEY_READ, &hKeyGfx) == ERROR_SUCCESS) {
            DWORD val = 0;
            DWORD sz = sizeof(val);
            if (RegQueryValueExW(hKeyGfx, L"HwSchMode", nullptr, nullptr,
                                 reinterpret_cast<LPBYTE>(&val), &sz) == ERROR_SUCCESS) {
                p.isHagsEnabled = (val == 2);
            }
            RegCloseKey(hKeyGfx);
        }

        HKEY hKeyDg = nullptr;
        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
                          L"SYSTEM\\CurrentControlSet\\Control\\DeviceGuard",
                          0, KEY_READ, &hKeyDg) == ERROR_SUCCESS) {
            DWORD val = 0;
            DWORD sz = sizeof(val);
            if (RegQueryValueExW(hKeyDg, L"EnableVirtualizationBasedSecurity", nullptr,
                                 nullptr, reinterpret_cast<LPBYTE>(&val), &sz) == ERROR_SUCCESS) {
                p.isVbsEnabled = (val == 1);
            }
            RegCloseKey(hKeyDg);
        }

        SC_HANDLE hSCM = OpenSCManagerW(nullptr, nullptr, SC_MANAGER_CONNECT);
        if (hSCM) {
            SC_HANDLE hSvc = OpenServiceW(hSCM, L"SysMain", SERVICE_QUERY_STATUS);
            if (hSvc) {
                SERVICE_STATUS_PROCESS ssp{};
                DWORD bytesNeeded = 0;
                if (QueryServiceStatusEx(hSvc, SC_STATUS_PROCESS_INFO,
                                         reinterpret_cast<LPBYTE>(&ssp), sizeof(ssp), &bytesNeeded)) {
                    p.isMemoryCompressionActive = (ssp.dwCurrentState == SERVICE_RUNNING);
                }
                CloseServiceHandle(hSvc);
            }
            CloseServiceHandle(hSCM);
        }

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
                    BYTE maxEff = 0;
                    BYTE minEff = 255;
                    ULONG offset = 0;
                    while (offset + sizeof(SYSTEM_CPU_SET_INFORMATION) <= len) {
                        auto* item = reinterpret_cast<PSYSTEM_CPU_SET_INFORMATION>(
                            buf.data() + offset);
                        if (item->Size == 0) {
                            break;
                        }
                        if (item->Type == CpuSetInformation) {
                            BYTE eff = item->CpuSet.EfficiencyClass;
                            if (eff > maxEff)
                                maxEff = eff;
                            if (eff < minEff)
                                minEff = eff;
                        }
                        offset += item->Size;
                    }
                    if (maxEff > minEff) {
                        p.isHybridCpu = true;
                        offset = 0;
                        while (offset + sizeof(SYSTEM_CPU_SET_INFORMATION) <= len) {
                            auto* item = reinterpret_cast<PSYSTEM_CPU_SET_INFORMATION>(
                                buf.data() + offset);
                            if (item->Size == 0) {
                                break;
                            }
                            if (item->Type == CpuSetInformation) {
                                if (item->CpuSet.EfficiencyClass == maxEff) {
                                    p.pCoreCpuSetIds.push_back(item->CpuSet.Id);
                                } else if (item->CpuSet.EfficiencyClass < maxEff) {
                                    // Collects all efficiency cores (standard E-cores and LP E-cores on Core Ultra architectures)
                                    p.eCoreCpuSetIds.push_back(item->CpuSet.Id);
                                }
                            }
                            offset += item->Size;
                        }
                    }
                }
            }
        }

        return p;
    }();
    return profile;
}

// Calculates effective free RAM threshold based on installed hardware and user setting.
// If userSettingPercent <= 0 (Auto Mode):
// Dynamically selects proactive thresholds tailored to memory capacity so systems with abundant
// RAM (e.g. 32 GB / 64 GB) maintain performance and prevent memory pressure before it affects foreground apps:
// - <= 8 GB:  25% standard / 40% tiered hogs (leaves 2.0 GB - 3.2 GB buffer)
// - <= 16 GB: 30% standard / 50% tiered hogs (leaves 4.8 GB - 8.0 GB buffer)
// - <= 32 GB: 45% standard / 60% tiered hogs (leaves 14.4 GB - 19.2 GB buffer)
// - > 32 GB:  50% standard / 65% tiered hogs (leaves 32.0 GB - 41.6 GB buffer)
// If userSettingPercent > 0:
// The explicit user setting overrides automatic calculation directly.
static double GetEffectiveRamThreshold(int userSettingPercent, double totalRamGb, bool isTieredHogs = false) {
    if (userSettingPercent > 0) {
        double base = static_cast<double>(userSettingPercent);
        double minLimit = isTieredHogs ? 10.0 : 5.0;
        double maxLimit = isTieredHogs ? 95.0 : 90.0;
        double target = isTieredHogs ? (base + 15.0) : base;
        return std::clamp(target, minLimit, maxLimit);
    }

    if (totalRamGb <= 8.5) {
        return isTieredHogs ? 40.0 : 25.0;
    }
    if (totalRamGb <= 16.5) {
        return isTieredHogs ? 50.0 : 30.0;
    }
    if (totalRamGb <= 32.5) {
        return isTieredHogs ? 60.0 : 45.0;
    }
    return isTieredHogs ? 65.0 : 50.0;
}

// ---------------------------------------------------------------------------
// Settings Structure & Enums
// ---------------------------------------------------------------------------

enum class CleanMode : uint8_t {
    SmartThreshold,
    Periodic,
    SmartAndPeriodic,
};

enum class ForegroundPrioritySetting : uint8_t {
    AboveNormal,
    High,
};

enum class ThrottlePrioritySetting : uint8_t {
    BelowNormal,
    Idle,
};

enum class LogDetailLevel : uint8_t {
    Minimal,
    Detailed,
    Debug,
};

struct ModSettings {
    bool enableProBalance = true;
    ForegroundPrioritySetting foregroundPriorityLevel =
        ForegroundPrioritySetting::AboveNormal;
    bool enableForegroundCpuSets = false;
    bool enableBackgroundCpuSets = false;
    bool enableBackgroundThrottling = true;
    bool enableThreadBackgroundMode = false;
    int backgroundCpuThrottleThresholdPercent = 15;
    int systemCpuContentionThresholdPercent = 60;
    ThrottlePrioritySetting backgroundThrottlePriorityLevel =
        ThrottlePrioritySetting::BelowNormal;
    bool enableEcoQosManagement = false;
    bool enableNetworkShielding = true;
    bool enableSmartAiOptimization = true;
    int aiInactivityGraceMinutes = 5;
    std::vector<std::wstring> customAiProcesses;
    bool enableAudioShielding = true;
    bool enableMultitaskingAdaptation = true;
    bool enableGameModeDetection = true;
    CleanMode cleanMode = CleanMode::SmartThreshold;
    int freeRamThresholdPercent = 0;
    bool enableTieredRamThreshold = true;
    int tieredHogThresholdPercent = 40;
    bool enableIdleBoost = true;
    int idleThresholdMinutes = 15;
    bool trimMinimizedWindows = true;
    bool enableProcessAging = true;
    int recentActivityGraceMinutes = 3;
    bool enableProcessTreeTrimming = true;
    bool enableElectronMemoryCap = true;
    int electronMemoryCapMb = 300;
    bool cleanBackgroundWorkingSets = true;
    DWORD minProcessMemoryToTrimMb = 50;
    int periodicIntervalMinutes = 10;
    bool targetProcessesOnly = false;
    std::vector<std::wstring> customTargetList;
    std::vector<std::wstring> excludedProcesses;
    bool enablePanicHotkey = true;
    bool enableHotkeySound = true;
    bool enableDynamicPowerPlan = false;
    bool enableBrowserTabTrim = true;
    int browserTabInactivityMinutes = 10;
    int gameAltTabGracePeriodSeconds = 180;
    int ioActivityWriteThresholdKbps = 250;
    int ioActivityTransferThresholdKbps = 100;
    int compressionBurstHysteresisSeconds = 20;
    bool pauseOnBattery = true;
    int checkIntervalSec = 10;
    LogDetailLevel logDetailLevel = LogDetailLevel::Detailed;
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
[[clang::no_destroy]] static std::optional<std::thread> g_workerThread;

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
// Unified priority management state (foreground boost & background throttling),
// guarded by g_priorityMutex so neither feature can misread or overwrite
// an original priority set by the other.
static std::mutex g_priorityMutex;

// ARCH-01: Unified Process State Machine (FSM)
enum class ProcessState : uint8_t {
    Untracked = 0,
    ForegroundBoosted,
    ShieldedAudioNetwork,
    BackgroundNormal,
    BackgroundThrottled,
    DormantPaged,
};

struct ProcessContext {
    DWORD pid = 0;
    std::wstring name;
    ProcessState state = ProcessState::Untracked;
    HANDLE hProcess = nullptr;
    DWORD originalPriority = NORMAL_PRIORITY_CLASS;
    ULONG originalIoPriority = IoPriorityNormal;
    ULONG originalMemoryPriority = 5; // MEMORY_PRIORITY_NORMAL
    bool ecoQosApplied = false;
    bool cpuSetsApplied = false;
    bool threadBackgroundApplied = false;
    std::chrono::steady_clock::time_point stateEnteredAt{};
};
static std::unordered_map<DWORD, ProcessContext> g_processContexts;

struct BoostedProcessEntry {
    DWORD pid = 0;
    HANDLE hProcess = nullptr;
    DWORD originalPriority = NORMAL_PRIORITY_CLASS;
    ULONG originalIoPriority = IoPriorityNormal;
    ULONG originalMemoryPriority = 5; // MEMORY_PRIORITY_NORMAL
    bool cpuSetsApplied = false;
};
static std::atomic<DWORD> g_currentBoostedPid{0};
static std::vector<BoostedProcessEntry> g_boostedProcesses;

// Fast-path synchronization atomics for hook thread
static std::atomic<bool> g_fastProBalanceEnabled{true};
static std::atomic<DWORD> g_fastForegroundPriority{ABOVE_NORMAL_PRIORITY_CLASS};
static std::atomic<DWORD> g_fastBoostedPid{0};
static std::atomic<ULONGLONG> g_lastFocusChangeTick{0};

// ---------------------------------------------------------------------------
// Dynamic Power Plan Management (High Performance on Demand & POWR-02 Async Dispatch)
// ---------------------------------------------------------------------------

typedef DWORD(WINAPI* pfnPowerGetActiveScheme)(HKEY UserRootPowerKey,
                                               GUID** ActivePolicyGuid);
typedef DWORD(WINAPI* pfnPowerSetActiveScheme)(HKEY UserRootPowerKey,
                                               const GUID* SchemeGuid);

static pfnPowerGetActiveScheme g_pfnPowerGetActiveScheme = nullptr;
static pfnPowerSetActiveScheme g_pfnPowerSetActiveScheme = nullptr;
static HMODULE g_hPowrProf = nullptr;

static const GUID kGuidHighPerformance = {
    0x8c5e7fda, 0xe8bf, 0x4a96, {0x9a, 0x85, 0xa6, 0xe2, 0x3a, 0x8e, 0x63, 0x5c}};

static std::mutex g_powerSchemeMutex;
static GUID g_originalPowerScheme{};
static bool g_hasOriginalPowerScheme = false;
static std::atomic<bool> g_isHighPerformanceActive{false};

// POWR-02: Async background dispatcher for non-blocking ACPI power plan switches
static std::atomic<bool> g_desiredHighPerformance{false};
static std::atomic<bool> g_powerWorkerRunning{false};
static HANDLE g_powerWakeEvent = nullptr;
[[clang::no_destroy]] static std::optional<std::thread> g_powerWorkerThread;

static const wchar_t* kPowerSchemeRegKey =
    L"Software\\Windhawk\\SmartProcessOptimizer";
static const wchar_t* kPowerSchemeRegValOriginal = L"OriginalPowerScheme";
static const wchar_t* kPowerSchemeRegValActive = L"HighPerformanceEngaged";

static void SavePowerSchemeToRegistry(const GUID& guid, bool engaged) {
    HKEY hKey = nullptr;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, kPowerSchemeRegKey, 0, nullptr,
                        REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, nullptr, &hKey,
                        nullptr) == ERROR_SUCCESS) {
        RegSetValueExW(hKey, kPowerSchemeRegValOriginal, 0, REG_BINARY,
                       reinterpret_cast<const BYTE*>(&guid), sizeof(GUID));
        DWORD valEngaged = engaged ? 1 : 0;
        RegSetValueExW(hKey, kPowerSchemeRegValActive, 0, REG_DWORD,
                       reinterpret_cast<const BYTE*>(&valEngaged),
                       sizeof(DWORD));
        RegCloseKey(hKey);
    }
}

static void ClearPowerSchemeRegistry() {
    HKEY hKey = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, kPowerSchemeRegKey, 0, KEY_SET_VALUE,
                      &hKey) == ERROR_SUCCESS) {
        DWORD valEngaged = 0;
        RegSetValueExW(hKey, kPowerSchemeRegValActive, 0, REG_DWORD,
                       reinterpret_cast<const BYTE*>(&valEngaged),
                       sizeof(DWORD));
        RegCloseKey(hKey);
    }
}

static void RestoreOriginalPowerScheme() {
    std::lock_guard<std::mutex> lock(g_powerSchemeMutex);
    if (!g_isHighPerformanceActive.load() || !g_hasOriginalPowerScheme ||
        !g_pfnPowerSetActiveScheme) {
        return;
    }
    g_pfnPowerSetActiveScheme(nullptr, &g_originalPowerScheme);
    g_isHighPerformanceActive.store(false);
    ClearPowerSchemeRegistry();
    Wh_Log(L"[SmartOptimizer] Restored original system power plan.");
}

static void EngageHighPerformancePowerScheme() {
    std::lock_guard<std::mutex> lock(g_powerSchemeMutex);
    if (g_isHighPerformanceActive.load() || !g_pfnPowerGetActiveScheme ||
        !g_pfnPowerSetActiveScheme) {
        return;
    }

    GUID* pCurrent = nullptr;
    if (g_pfnPowerGetActiveScheme(nullptr, &pCurrent) == ERROR_SUCCESS && pCurrent) {
        if (IsEqualGUID(*pCurrent, kGuidHighPerformance)) {
            LocalFree(pCurrent);
            return;
        }

        g_originalPowerScheme = *pCurrent;
        g_hasOriginalPowerScheme = true;
        LocalFree(pCurrent);

        SavePowerSchemeToRegistry(g_originalPowerScheme, /*engaged=*/true);

        if (g_pfnPowerSetActiveScheme(nullptr, &kGuidHighPerformance) == ERROR_SUCCESS) {
            g_isHighPerformanceActive.store(true);
            Wh_Log(L"[SmartOptimizer] Switched system power plan to High Performance for active foreground workload.");
        }
    }
}

static void PowerSchemeWorkerProc() {
    while (g_powerWorkerRunning.load(std::memory_order_relaxed)) {
        DWORD waitRes = WaitForSingleObject(g_powerWakeEvent, INFINITE);
        if (!g_powerWorkerRunning.load(std::memory_order_relaxed)) {
            break;
        }
        if (waitRes == WAIT_OBJECT_0) {
            bool target = g_desiredHighPerformance.load(std::memory_order_acquire);
            if (target) {
                EngageHighPerformancePowerScheme();
            } else {
                RestoreOriginalPowerScheme();
            }
        }
    }
}

static void RequestPowerSchemeAsync(bool highPerformance) {
    g_desiredHighPerformance.store(highPerformance, std::memory_order_release);
    if (g_powerWakeEvent) {
        SetEvent(g_powerWakeEvent);
    }
}

static void CheckAndRecoverCrashedPowerScheme() {
    if (!g_pfnPowerSetActiveScheme)
        return;
    HKEY hKey = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, kPowerSchemeRegKey, 0, KEY_READ | KEY_SET_VALUE,
                      &hKey) == ERROR_SUCCESS) {
        DWORD valEngaged = 0;
        DWORD cbEngaged = sizeof(valEngaged);
        if (RegQueryValueExW(hKey, kPowerSchemeRegValActive, nullptr, nullptr,
                             reinterpret_cast<BYTE*>(&valEngaged),
                             &cbEngaged) == ERROR_SUCCESS &&
            valEngaged == 1) {
            GUID savedGuid{};
            DWORD cbGuid = sizeof(savedGuid);
            if (RegQueryValueExW(hKey, kPowerSchemeRegValOriginal, nullptr,
                                 nullptr, reinterpret_cast<BYTE*>(&savedGuid),
                                 &cbGuid) == ERROR_SUCCESS &&
                cbGuid == sizeof(GUID)) {
                g_pfnPowerSetActiveScheme(nullptr, &savedGuid);
                Wh_Log(L"[SmartOptimizer] Recovered and restored original power scheme after previous crash or shutdown.");
            }
            DWORD zero = 0;
            RegSetValueExW(hKey, kPowerSchemeRegValActive, 0, REG_DWORD,
                           reinterpret_cast<const BYTE*>(&zero), sizeof(DWORD));
        }
        RegCloseKey(hKey);
    }
}

static bool IsBrowserProcessName(const std::wstring& name) {
    static const wchar_t* const kBrowserNames[] = {
        L"chrome.exe", L"msedge.exe", L"brave.exe",
        L"firefox.exe", L"opera.exe", L"vivaldi.exe",
        L"zen.exe"};
    for (const wchar_t* b : kBrowserNames) {
        if (_wcsicmp(name.c_str(), b) == 0)
            return true;
    }
    return false;
}

// Background CPU-throttling state, guarded by g_priorityMutex.
// We hold an open handle for each throttled process to prevent Windows from
// recycling its PID.
struct ThrottledProcessInfo {
    HANDLE hProcess = nullptr;
    DWORD originalPriority = NORMAL_PRIORITY_CLASS;
    ULONG originalIoPriority = IoPriorityNormal;
    ULONG originalMemoryPriority = 5; // MEMORY_PRIORITY_NORMAL
    bool ecoQosApplied = false;
    bool cpuSetsApplied = false;
    bool threadBackgroundApplied = false;
    DWORD staleSampleCount = 0;
};
static std::unordered_map<DWORD, ThrottledProcessInfo>
    g_throttledProcesses; // pid -> info
struct CpuSample {
    ULONGLONG kernelPlusUser100ns = 0;
    std::chrono::steady_clock::time_point sampleTime{};
};
static std::unordered_map<DWORD, CpuSample> g_cpuSamples;

// AI Workload Tracking (Inference Activity Timestamps & Dynamic Classification)
enum class AiClassification : uint8_t {
    Unknown = 0,
    Pending,
    IsAi,
    NotAi,
};

struct AiProcessClassification {
    AiClassification status = AiClassification::Unknown;
    std::chrono::steady_clock::time_point firstSeen{};
    std::chrono::steady_clock::time_point lastEvaluated{};
};

static std::unordered_map<DWORD, AiProcessClassification> g_aiProcessCache;
static std::unordered_map<DWORD, std::chrono::steady_clock::time_point>
    g_aiLastInferenceTime;
// Last known working-set size per AI pid; a meaningful change is used as a
// second "still active" signal alongside CPU usage (see
// UpdateAiProcessActivity).
static std::unordered_map<DWORD, SIZE_T> g_aiLastWorkingSetSize;
static std::unordered_map<DWORD, CpuSample> g_aiCpuSamples;

// Focus / Trim Bookkeeping & Multitasking Tracker
static std::mutex g_focusMapMutex;
static std::unordered_map<DWORD, std::chrono::steady_clock::time_point>
    g_processLastFocusedTime;
static std::deque<std::chrono::steady_clock::time_point> g_focusSwitchHistory;

// Only touched by the worker thread.
static std::unordered_map<DWORD, std::chrono::steady_clock::time_point>
    g_processLastTrimmed;

// Game-sweep debounce; only touched by the hook thread.
static DWORD g_lastGameSweepPid = 0;
static std::chrono::steady_clock::time_point g_lastGameSweepTime{};
static std::mutex g_gameSanctuaryMutex;
static DWORD g_gameSanctuaryPid = 0;
static HANDLE g_gameSanctuaryHandle = nullptr;
static FILETIME g_gameSanctuaryCreateTime{};
static std::chrono::steady_clock::time_point g_gameLastForegroundTime{};

static void SetGameSanctuary(DWORD pid) {
    std::lock_guard<std::mutex> lock(g_gameSanctuaryMutex);
    g_gameLastForegroundTime = std::chrono::steady_clock::now();
    if (g_gameSanctuaryPid == pid && g_gameSanctuaryHandle) {
        // Validate that the handle still points to the same process instance
        // by checking both liveness and creation-time signature.
        if (WaitForSingleObject(g_gameSanctuaryHandle, 0) == WAIT_TIMEOUT) {
            FILETIME ct{}, x{}, y{}, z{};
            if (GetProcessTimes(g_gameSanctuaryHandle, &ct, &x, &y, &z) &&
                ct.dwLowDateTime == g_gameSanctuaryCreateTime.dwLowDateTime &&
                ct.dwHighDateTime == g_gameSanctuaryCreateTime.dwHighDateTime) {
                return; // Same PID, same process instance — keep sanctuary.
            }
        }
    }
    if (g_gameSanctuaryHandle) {
        CloseHandle(g_gameSanctuaryHandle);
        g_gameSanctuaryHandle = nullptr;
    }
    g_gameSanctuaryPid = 0;
    g_gameSanctuaryCreateTime = {};
    if (pid == 0) {
        return;
    }
    HANDLE handle = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION,
                                FALSE, pid);
    if (handle && WaitForSingleObject(handle, 0) == WAIT_TIMEOUT) {
        FILETIME ct{}, x{}, y{}, z{};
        if (GetProcessTimes(handle, &ct, &x, &y, &z)) {
            g_gameSanctuaryPid = pid;
            g_gameSanctuaryHandle = handle;
            g_gameSanctuaryCreateTime = ct;
        } else {
            CloseHandle(handle);
        }
    } else if (handle) {
        CloseHandle(handle);
    }
}

static DWORD GetLiveGameSanctuaryPid() {
    std::lock_guard<std::mutex> lock(g_gameSanctuaryMutex);
    if (!g_gameSanctuaryHandle ||
        WaitForSingleObject(g_gameSanctuaryHandle, 0) != WAIT_TIMEOUT) {
        if (g_gameSanctuaryHandle) {
            CloseHandle(g_gameSanctuaryHandle);
            g_gameSanctuaryHandle = nullptr;
        }
        g_gameSanctuaryPid = 0;
        g_gameSanctuaryCreateTime = {};
        return 0;
    }
    // Verify creation-time signature to guard against PID reuse.
    FILETIME ct{}, x{}, y{}, z{};
    if (GetProcessTimes(g_gameSanctuaryHandle, &ct, &x, &y, &z)) {
        if (ct.dwLowDateTime != g_gameSanctuaryCreateTime.dwLowDateTime ||
            ct.dwHighDateTime != g_gameSanctuaryCreateTime.dwHighDateTime) {
            // PID was reused by a different process — clear sanctuary.
            CloseHandle(g_gameSanctuaryHandle);
            g_gameSanctuaryHandle = nullptr;
            g_gameSanctuaryPid = 0;
            g_gameSanctuaryCreateTime = {};
            return 0;
        }
    }
    return g_gameSanctuaryPid;
}

static void ClearGameSanctuary() {
    std::lock_guard<std::mutex> lock(g_gameSanctuaryMutex);
    if (g_gameSanctuaryHandle) {
        CloseHandle(g_gameSanctuaryHandle);
        g_gameSanctuaryHandle = nullptr;
    }
    g_gameSanctuaryPid = 0;
    g_gameSanctuaryCreateTime = {};
    g_gameLastForegroundTime = {};
}

// Returns true when a verified game process is active in foreground,
// or within a configurable Alt-Tab grace period after losing focus.
static bool IsGamingLockdownActive(DWORD fgPid,
                                   std::chrono::steady_clock::time_point now,
                                   int gracePeriodSeconds = 180) {
    DWORD sanctuaryPid = GetLiveGameSanctuaryPid();
    if (sanctuaryPid == 0) {
        return false;
    }
    if (fgPid == sanctuaryPid) {
        std::lock_guard<std::mutex> lock(g_gameSanctuaryMutex);
        g_gameLastForegroundTime = now;
        return true;
    }
    // Maintain sanctuary immunity and memory lockdown for configured grace period after Alt-Tab
    std::lock_guard<std::mutex> lock(g_gameSanctuaryMutex);
    auto elapsedSec = std::chrono::duration_cast<std::chrono::seconds>(
                          now - g_gameLastForegroundTime)
                          .count();
    return elapsedSec <= static_cast<int64_t>(gracePeriodSeconds);
}

static std::chrono::steady_clock::time_point g_lastPeriodicCleanTime{};
static std::chrono::steady_clock::time_point g_lastTriggerCleanTime{};
static std::chrono::steady_clock::time_point g_lastIdleCleanTime{};
static bool g_wasIdle = false;

// Tracks elevated or system processes skipped in user session
static std::atomic<DWORD> g_accessDeniedCount{0};

// Dynamic Immunity Cache: remembers PIDs failing OpenProcess with ERROR_ACCESS_DENIED
// (anti-cheat, PPL) to skip redundant query calls.
static std::mutex g_immunitySetMutex;
static std::unordered_set<DWORD> g_accessDeniedImmunitySet;

static bool IsAccessDeniedImmune(DWORD pid) {
    std::lock_guard<std::mutex> lock(g_immunitySetMutex);
    return g_accessDeniedImmunitySet.count(pid) != 0;
}

static void RecordAccessDeniedImmunity(DWORD pid) {
    std::lock_guard<std::mutex> lock(g_immunitySetMutex);
    g_accessDeniedImmunitySet.insert(pid);
}

static void PruneAccessDeniedImmunity(const std::unordered_set<DWORD>& alivePids) {
    std::lock_guard<std::mutex> lock(g_immunitySetMutex);
    for (auto it = g_accessDeniedImmunitySet.begin();
         it != g_accessDeniedImmunitySet.end();) {
        it = (alivePids.count(*it) == 0) ? g_accessDeniedImmunitySet.erase(it)
                                         : std::next(it);
    }
}

// Session stats.
static std::chrono::steady_clock::time_point g_modStartTime{};
static std::atomic<ULONGLONG> g_sessionBytesReclaimed{0};
static std::atomic<ULONGLONG> g_sessionBytesStandbyDemoted{0};
static std::atomic<DWORDLONG> g_lastCleanAvailPhys{0};
static std::atomic<DWORD> g_sessionCleanupPasses{0};
static std::atomic<DWORD> g_sessionProcessesTrimmedTotal{0};
static std::atomic<DWORD> g_sessionBoostTransitions{0};
static std::atomic<DWORD> g_sessionThrottleTransitions{0};
static std::atomic<ULONGLONG> g_lastResumeTick{0};
static std::atomic<bool> g_resetSamplesRequested{false};

// ---------------------------------------------------------------------------
// Precision Telemetry & Categorized Diagnostic Logging
// ---------------------------------------------------------------------------

enum class LogCategory : uint8_t {
    Boot,
    ProBalance,
    Throttler,
    Sanctuary,
    Memory,
    Power,
    Shutdown,
};

static void LogEvent(LogCategory cat, LogDetailLevel /*minLevel*/, const wchar_t* format, ...) {
    const wchar_t* prefix = L"[SmartOptimizer]";
    switch (cat) {
    case LogCategory::Boot:
        prefix = L"[SmartOptimizer::Boot]";
        break;
    case LogCategory::ProBalance:
        prefix = L"[SmartOptimizer::ProBalance]";
        break;
    case LogCategory::Throttler:
        prefix = L"[SmartOptimizer::Throttler]";
        break;
    case LogCategory::Sanctuary:
        prefix = L"[SmartOptimizer::Sanctuary]";
        break;
    case LogCategory::Memory:
        prefix = L"[SmartOptimizer::Memory]";
        break;
    case LogCategory::Power:
        prefix = L"[SmartOptimizer::Power]";
        break;
    case LogCategory::Shutdown:
        prefix = L"[SmartOptimizer::Shutdown]";
        break;
    }

    wchar_t buffer[1024];
    va_list args;
    va_start(args, format);
    _vsnwprintf_s(buffer, _countof(buffer), _TRUNCATE, format, args);
    va_end(args);

    Wh_Log(L"%s %s", prefix, buffer);
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

template <typename TMap>
static void PruneDeadPids(TMap& map, const std::unordered_set<DWORD>& alivePids) {
    for (auto it = map.begin(); it != map.end();) {
        it = (alivePids.count(it->first) == 0) ? map.erase(it) : std::next(it);
    }
}

static std::wstring ToLower(std::wstring str) {
    std::transform(str.begin(), str.end(), str.begin(), ::towlower);
    return str;
}

static std::wstring Trim(const std::wstring& str) {
    size_t first = str.find_first_not_of(L" \t\r\n");
    if (first == std::wstring::npos)
        return L"";
    size_t last = str.find_last_not_of(L" \t\r\n");
    return str.substr(first, (last - first + 1));
}

static std::vector<std::wstring> ParseProcessList(const std::wstring& input) {
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
        // ACLineStatus == 0: Running on battery power
        // SystemStatusFlag == 1: Windows Battery Saver or Windows 11 24H2 Energy Saver active
        return (sps.ACLineStatus == 0 || sps.SystemStatusFlag == 1);
    }
    return false;
}

static DWORD GetWindowProcessIdResolved(HWND hwnd) {
    if (!hwnd)
        return 0;
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == 0)
        return 0;

    WCHAR className[256];
    if (GetClassNameW(hwnd, className, ARRAYSIZE(className)) > 0) {
        if (wcscmp(className, L"ApplicationFrameWindow") == 0) {
            // 1. Try direct child Windows.UI.Core.CoreWindow lookup
            HWND hCore = FindWindowExW(hwnd, nullptr, L"Windows.UI.Core.CoreWindow", nullptr);
            if (hCore) {
                DWORD corePid = 0;
                GetWindowThreadProcessId(hCore, &corePid);
                if (corePid != 0 && corePid != pid) {
                    return corePid;
                }
            }
            // 2. WIN3-01: Direct immediate child window inspect without recursive tree enum
            HWND hChild = FindWindowExW(hwnd, nullptr, nullptr, nullptr);
            while (hChild) {
                DWORD childPid = 0;
                GetWindowThreadProcessId(hChild, &childPid);
                if (childPid != 0 && childPid != pid) {
                    return childPid;
                }
                hChild = FindWindowExW(hwnd, hChild, nullptr, nullptr);
            }
        }
    }
    return pid;
}

static DWORD GetForegroundProcessId() {
    HWND fgWnd = GetForegroundWindow();
    if (!fgWnd)
        return 0;
    return GetWindowProcessIdResolved(fgWnd);
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

static std::wstring BuildSessionStatsString() {
    auto now = std::chrono::steady_clock::now();
    std::wstring uptime = FormatUptime(g_modStartTime, now);
    ULONGLONG reclaimedMb =
        g_sessionBytesReclaimed.load(std::memory_order_relaxed) /
        (1024ULL * 1024ULL);
    ULONGLONG standbyMb =
        g_sessionBytesStandbyDemoted.load(std::memory_order_relaxed) /
        (1024ULL * 1024ULL);

    wchar_t buf[512];
    swprintf_s(buf, _countof(buf),
               L"SmartOptimizer Session Report\n"
               L"  Uptime: %s\n"
               L"  RAM reclaimed: %llu MB hard, %llu MB standby (%u passes)\n"
               L"  ProBalance boosts: %u transitions\n"
               L"  Background throttles: %u transitions\n"
               L"  Processes trimmed: %u total",
               uptime.c_str(), reclaimedMb, standbyMb,
               g_sessionCleanupPasses.load(std::memory_order_relaxed),
               g_sessionBoostTransitions.load(std::memory_order_relaxed),
               g_sessionThrottleTransitions.load(std::memory_order_relaxed),
               g_sessionProcessesTrimmedTotal.load(std::memory_order_relaxed));
    return buf;
}

static bool IsInList(const std::wstring& name,
                     const std::vector<std::wstring>& list) {
    for (const auto& item : list) {
        if (name == item)
            return true;
    }
    return false;
}

// Critical OS authentication dialogs excluded from priority modifications.
static bool IsEssentialSystemSecurityProcess(const std::wstring& name) {
    static const std::unordered_set<std::wstring> kEssential = {
        L"logonui.exe", L"lockapp.exe", L"consent.exe", L"credentialuibroker.exe",
        L"smartscreen.exe", L"securityhealthservice.exe", L"securityhealthsystray.exe",
        L"splwow64.exe", L"printfilterpipelinesvc.exe", L"spoolsv.exe",
        L"wudfhost.exe", L"devicecensus.exe",
        L"tiworker.exe", L"trustedinstaller.exe", L"msiexec.exe",
        L"dismhost.exe", L"dism.exe"};
    return kEssential.count(name) != 0;
}

#ifndef LIST_MODULES_ALL
#define LIST_MODULES_ALL 0x03
#endif

struct ProcessCommandLineUnicodeString {
    USHORT Length = 0;
    USHORT MaximumLength = 0;
    PWSTR Buffer = nullptr;
};

static std::wstring GetProcessCommandLine(HANDLE hProcess) {
    if (!g_pfnNtQueryInformationProcess)
        return {};

    ULONG returnLength = 0;
    // ProcessCommandLineInformation is 60
    NTSTATUS status = g_pfnNtQueryInformationProcess(
        hProcess, 60, nullptr, 0, &returnLength);
    if (returnLength == 0)
        return {};

    std::vector<BYTE> buffer(returnLength + sizeof(WCHAR) * 2);
    status = g_pfnNtQueryInformationProcess(
        hProcess, 60, buffer.data(), returnLength, &returnLength);
    if (status < 0 || buffer.size() < sizeof(ProcessCommandLineUnicodeString))
        return {};

    auto* pus =
        reinterpret_cast<ProcessCommandLineUnicodeString*>(buffer.data());
    if (pus->Length == 0 || pus->Buffer == nullptr)
        return {};

    size_t charCount = pus->Length / sizeof(WCHAR);
    uintptr_t bufStart = reinterpret_cast<uintptr_t>(buffer.data());
    uintptr_t bufEnd = bufStart + buffer.size();
    uintptr_t strStart = reinterpret_cast<uintptr_t>(pus->Buffer);

    if (strStart >= bufStart && (strStart + pus->Length) <= bufEnd) {
        return std::wstring(pus->Buffer, charCount);
    }
    return {};
}

static bool DoesCommandLineMatchAiMarkers(const std::wstring& cmdline) {
    if (cmdline.empty())
        return false;
    std::wstring lowerCmd = ToLower(cmdline);

    static const std::vector<std::wstring_view> kMarkers = {
        L".gguf",
        L".safetensors",
        L".onnx",
        L"--model",
        L"-m ",
        L"ollama",
        L"comfyui",
        L"vllm",
        L"koboldcpp",
        L"text-generation-webui",
        L"sd-webui",
        L"diffusers",
        L"llama_cpp",
        L"transformers",
        L"stable-diffusion"};

    for (const auto& marker : kMarkers) {
        if (lowerCmd.find(marker) != std::wstring::npos)
            return true;
    }
    return false;
}

static bool DoesProcessLoadAiModules(HANDLE hProcess) {
    HMODULE hMods[256];
    DWORD cbNeeded = 0;
    if (!EnumProcessModulesEx(hProcess, hMods, sizeof(hMods), &cbNeeded,
                              LIST_MODULES_ALL)) {
        return false;
    }

    DWORD count = cbNeeded / sizeof(HMODULE);
    if (count > static_cast<DWORD>(std::size(hMods))) {
        count = static_cast<DWORD>(std::size(hMods));
    }

    WCHAR modName[MAX_PATH];
    for (DWORD i = 0; i < count; ++i) {
        if (hMods[i] == nullptr)
            continue;
        DWORD len = GetModuleBaseNameW(hProcess, hMods[i], modName,
                                       static_cast<DWORD>(std::size(modName)));
        if (len == 0)
            continue;

        for (DWORD j = 0; j < len; ++j) {
            modName[j] = static_cast<WCHAR>(::towlower(modName[j]));
        }
        std::wstring_view sv(modName, len);

        if (sv.find(L"ggml") != std::wstring_view::npos ||
            sv.find(L"llama") != std::wstring_view::npos ||
            sv.find(L"cublas") != std::wstring_view::npos ||
            sv.find(L"cudart") != std::wstring_view::npos ||
            sv.find(L"cudnn") != std::wstring_view::npos ||
            sv.find(L"onnxruntime") != std::wstring_view::npos ||
            sv.find(L"torch") != std::wstring_view::npos ||
            sv.find(L"libtorch") != std::wstring_view::npos ||
            sv.find(L"c10_cuda") != std::wstring_view::npos ||
            sv.find(L"amdhip") != std::wstring_view::npos ||
            sv.find(L"rocblas") != std::wstring_view::npos ||
            sv.find(L"openvino") != std::wstring_view::npos) {
            return true;
        }
    }
    return false;
}

static uint64_t GetProcessAgeSeconds(HANDLE hProcess) {
    FILETIME ftCreation{}, ftExit{}, ftKernel{}, ftUser{};
    if (!GetProcessTimes(hProcess, &ftCreation, &ftExit, &ftKernel, &ftUser))
        return 1000;

    ULARGE_INTEGER ulCreation{};
    ulCreation.LowPart = ftCreation.dwLowDateTime;
    ulCreation.HighPart = ftCreation.dwHighDateTime;

    FILETIME ftNow{};
    GetSystemTimeAsFileTime(&ftNow);
    ULARGE_INTEGER ulNow{};
    ulNow.LowPart = ftNow.dwLowDateTime;
    ulNow.HighPart = ftNow.dwHighDateTime;

    if (ulNow.QuadPart > ulCreation.QuadPart) {
        return (ulNow.QuadPart - ulCreation.QuadPart) / 10000000ULL;
    }
    return 0;
}

// ---------------------------------------------------------------------------
// Dynamic AI & Tensor Workload Detection
// ---------------------------------------------------------------------------

static bool IsAiProcess(DWORD pid,
                        const std::wstring& name,
                        const ModSettings& settings) {
    if (!settings.enableSmartAiOptimization || pid <= 4)
        return false;

    if (IsInList(name, settings.excludedProcesses) ||
        IsEssentialSystemSecurityProcess(name)) {
        return false;
    }

    if (IsInList(name, settings.customAiProcesses))
        return true;

    auto now = std::chrono::steady_clock::now();
    auto itCache = g_aiProcessCache.find(pid);
    if (itCache != g_aiProcessCache.end()) {
        if (itCache->second.status == AiClassification::IsAi)
            return true;
        if (itCache->second.status == AiClassification::NotAi)
            return false;
        if (itCache->second.status == AiClassification::Pending) {
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                               now - itCache->second.firstSeen)
                               .count();
            if (elapsed < 5)
                return true;
        }
    }

    auto firstSeen = (itCache != g_aiProcessCache.end())
                         ? itCache->second.firstSeen
                         : now;

    // Check loaded tensor modules first (requires VM read)
    HANDLE hProcess =
        OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (hProcess) {
        if (DoesProcessLoadAiModules(hProcess)) {
            g_aiProcessCache[pid] = {AiClassification::IsAi, firstSeen, now};
            CloseHandle(hProcess);
            return true;
        }
        CloseHandle(hProcess);
    }

    // Fallback: command-line arguments and startup age via query-limited handle
    hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (hProcess) {
        if (DoesCommandLineMatchAiMarkers(GetProcessCommandLine(hProcess))) {
            g_aiProcessCache[pid] = {AiClassification::IsAi, firstSeen, now};
            CloseHandle(hProcess);
            return true;
        }

        uint64_t age = GetProcessAgeSeconds(hProcess);
        CloseHandle(hProcess);

        if (age < 5) {
            g_aiProcessCache[pid] = {AiClassification::Pending, firstSeen, now};
            return true;
        }

        g_aiProcessCache[pid] = {AiClassification::NotAi, firstSeen, now};
        return false;
    }

    return false;
}

// ---------------------------------------------------------------------------
// Generic Process Classification (name-list independent)
// ---------------------------------------------------------------------------

// Checks if PID belongs to the interactive user session, excluding session-0 services.
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

// Packaged (UWP/MSIX) apps are managed by Windows PLM and excluded from throttling.
static bool IsPackagedApp(HANDLE hProcess) {
    UINT32 len = 0;
    LONG rc = GetPackageFullName(hProcess, &len, nullptr);
    return rc != APPMODEL_ERROR_NO_PACKAGE && len > 0;
}

// ---------------------------------------------------------------------------
// Process Classification & Bounded O(1) Cache (FlyWire Root-ID Lookup)
// ---------------------------------------------------------------------------

enum class ProcessClass : uint8_t {
    Unknown = 0,
    System,
    Packaged,
    AiEngine,
    UserApp
};

struct ProcessClassEntry {
    ProcessClass cls = ProcessClass::Unknown;
    std::chrono::steady_clock::time_point cachedAt{};
};

static std::mutex g_classifyCacheMutex;
static std::unordered_map<DWORD, ProcessClassEntry> g_processClassCache;
static constexpr auto kClassifyCacheTtl = std::chrono::seconds(60);

static bool IsPackagedAppCached(DWORD pid, HANDLE hProcess) {
    if (!hProcess) {
        return false;
    }
    auto now = std::chrono::steady_clock::now();
    {
        std::lock_guard<std::mutex> lock(g_classifyCacheMutex);
        auto it = g_processClassCache.find(pid);
        if (it != g_processClassCache.end() &&
            (now - it->second.cachedAt) < kClassifyCacheTtl) {
            if (it->second.cls == ProcessClass::Packaged) {
                return true;
            }
            if (it->second.cls == ProcessClass::System ||
                it->second.cls == ProcessClass::AiEngine) {
                return false;
            }
        }
    }

    bool isPkg = IsPackagedApp(hProcess);
    {
        std::lock_guard<std::mutex> lock(g_classifyCacheMutex);
        if (g_processClassCache.size() >= 512) {
            g_processClassCache.clear();
        }
        auto& entry = g_processClassCache[pid];
        entry.cachedAt = now;
        if (isPkg) {
            entry.cls = ProcessClass::Packaged;
        } else if (entry.cls == ProcessClass::Unknown) {
            entry.cls = ProcessClass::UserApp;
        }
    }
    return isPkg;
}

static void PruneClassifyCache(const std::unordered_set<DWORD>& alivePids) {
    std::lock_guard<std::mutex> lock(g_classifyCacheMutex);
    for (auto it = g_processClassCache.begin(); it != g_processClassCache.end();) {
        it = (alivePids.count(it->first) == 0) ? g_processClassCache.erase(it)
                                               : std::next(it);
    }
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

static void ScanDeviceAudioSessions(IMMDevice* pDevice,
                                    std::unordered_set<DWORD>& audioPids) {
    if (!pDevice)
        return;
    IAudioSessionManager2* pSessionManager = nullptr;
    HRESULT hr =
        pDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, nullptr,
                          reinterpret_cast<void**>(&pSessionManager));
    if (SUCCEEDED(hr) && pSessionManager) {
        IAudioSessionEnumerator* pSessionList = nullptr;
        hr = pSessionManager->GetSessionEnumerator(&pSessionList);
        if (SUCCEEDED(hr) && pSessionList) {
            int count = 0;
            pSessionList->GetCount(&count);
            for (int i = 0; i < count; i++) {
                IAudioSessionControl* pSessionControl = nullptr;
                if (SUCCEEDED(pSessionList->GetSession(i, &pSessionControl)) &&
                    pSessionControl) {
                    AudioSessionState state = AudioSessionStateInactive;
                    if (SUCCEEDED(pSessionControl->GetState(&state)) &&
                        state == AudioSessionStateActive) {
                        IAudioSessionControl2* pControl2 = nullptr;
                        if (SUCCEEDED(pSessionControl->QueryInterface(
                                __uuidof(IAudioSessionControl2),
                                reinterpret_cast<void**>(&pControl2))) &&
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
    if (SUCCEEDED(hrCom)) {
        weInitializedCom = true;
    }

    IMMDeviceEnumerator* pEnumerator = nullptr;
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                                  CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
                                  reinterpret_cast<void**>(&pEnumerator));
    if (SUCCEEDED(hr) && pEnumerator) {
        // 1. Enumerate all active audio render endpoints (headphones, speakers,
        // virtual channels)
        IMMDeviceCollection* pCollection = nullptr;
        hr = pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE,
                                             &pCollection);
        if (SUCCEEDED(hr) && pCollection) {
            UINT devCount = 0;
            pCollection->GetCount(&devCount);
            for (UINT i = 0; i < devCount; ++i) {
                IMMDevice* pDevice = nullptr;
                if (SUCCEEDED(pCollection->Item(i, &pDevice)) && pDevice) {
                    ScanDeviceAudioSessions(pDevice, audioPids);
                    pDevice->Release();
                }
            }
            pCollection->Release();
        }

        // Fallback to default console and multimedia endpoints
        if (audioPids.empty()) {
            IMMDevice* pDef = nullptr;
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

// Forward declarations for early audio elastic re-engagement & pre-emptive staging
static bool SetProcessMemoryPriorityHint(HANDLE hProcess, ULONG priority);
static bool ResetProcessEcoQoS(HANDLE hProcess);

static std::wstring GetProcessNameByPid(DWORD pid) {
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W pe{};
        pe.dwSize = sizeof(pe);
        if (Process32FirstW(hSnap, &pe)) {
            do {
                if (pe.th32ProcessID == pid) {
                    CloseHandle(hSnap);
                    return ToLower(pe.szExeFile);
                }
            } while (Process32NextW(hSnap, &pe));
        }
        CloseHandle(hSnap);
    }
    return L"process";
}

// Cached WASAPI audio sessions with hysteresis grace period.
static std::chrono::steady_clock::time_point g_audioPidsCacheTime{};
static std::unordered_set<DWORD> g_audioPidsCache;
static std::unordered_map<DWORD, std::chrono::steady_clock::time_point>
    g_audioPidLastActive;

static std::unordered_set<DWORD>
GetActiveAudioProcessIdsCached(const ModSettings& settings) {
    if (!settings.enableAudioShielding) {
        return {};
    }
    auto now = std::chrono::steady_clock::now();
    // Refresh interval: 1500 ms
    if (g_audioPidsCacheTime.time_since_epoch().count() != 0 &&
        std::chrono::duration_cast<std::chrono::milliseconds>(
            now - g_audioPidsCacheTime)
                .count() < 1500) {
        return g_audioPidsCache;
    }

    auto rawPids = GetActiveAudioProcessIds();

    // 1. Instant Elastic Rebound: for any audio stream newly started or resumed,
    // immediately restore full memory priority (5 / Normal) and clear any EcoQoS throttling.
    for (DWORD pid : rawPids) {
        if (g_audioPidsCache.find(pid) == g_audioPidsCache.end()) {
            HANDLE hProc = OpenProcess(
                PROCESS_SET_INFORMATION | PROCESS_QUERY_LIMITED_INFORMATION,
                FALSE, pid);
            if (hProc) {
                SetProcessMemoryPriorityHint(hProc, 5 /* MEMORY_PRIORITY_NORMAL */);
                ResetProcessEcoQoS(hProc);
                CloseHandle(hProc);
            }
            std::wstring procName = GetProcessNameByPid(pid);
            LogEvent(LogCategory::Sanctuary, LogDetailLevel::Minimal,
                     L"Active audio stream detected on %s (PID %u) -> Engaging topological sanctuary | Memory: Normal",
                     procName.c_str(), pid);
        }
        g_audioPidLastActive[pid] = now;
    }

    // 2. Pre-emptive Memory Staging: for any PID that stopped playing audio,
    // stage memory gently to standby list (2 / Low) once the grace period elapses.
    DWORD fgPid = GetForegroundProcessId();
    std::unordered_set<DWORD> combined;
    for (auto it = g_audioPidLastActive.begin();
         it != g_audioPidLastActive.end();) {
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                           now - it->second)
                           .count();
        if (elapsed <= 5) {
            combined.insert(it->first);
            ++it;
        } else {
            DWORD stoppedPid = it->first;
            std::wstring procName = GetProcessNameByPid(stoppedPid);
            std::wstring fgName = (fgPid != 0) ? GetProcessNameByPid(fgPid) : L"";
            bool isForegroundApp =
                (stoppedPid == fgPid) ||
                (!fgName.empty() && _wcsicmp(procName.c_str(), fgName.c_str()) == 0);

            if (!isForegroundApp && stoppedPid != 0 && stoppedPid != 4) {
                HANDLE hProc = OpenProcess(
                    PROCESS_SET_INFORMATION | PROCESS_QUERY_LIMITED_INFORMATION,
                    FALSE, stoppedPid);
                if (hProc) {
                    SetProcessMemoryPriorityHint(hProc, 2 /* MEMORY_PRIORITY_LOW */);
                    CloseHandle(hProc);
                }
                LogEvent(LogCategory::Sanctuary, LogDetailLevel::Minimal,
                         L"Audio stream ceased on %s (PID %u) -> Pre-emptively staging to Standby memory (Memory: Low)",
                         procName.c_str(), stoppedPid);
            }
            it = g_audioPidLastActive.erase(it);
        }
    }

    g_audioPidsCache = combined;
    g_audioPidsCacheTime = now;
    return g_audioPidsCache;
}

// ---------------------------------------------------------------------------
// Process Snapshot Cache (single toolhelp snapshot per watchdog cycle)
// ---------------------------------------------------------------------------

struct ProcessSnapshotEntry {
    DWORD pid = 0;
    DWORD parentPid = 0;
    DWORD threadCount = 0;
    std::wstring name;
};

static std::mutex g_processSnapshotCacheMutex;
static std::chrono::steady_clock::time_point g_processSnapshotCacheTime{};
static std::vector<ProcessSnapshotEntry> g_processSnapshotCache;

static std::vector<ProcessSnapshotEntry> CaptureProcessSnapshotCached() {
    std::lock_guard<std::mutex> lock(g_processSnapshotCacheMutex);
    auto now = std::chrono::steady_clock::now();
    if (g_processSnapshotCacheTime.time_since_epoch().count() != 0 &&
        std::chrono::duration_cast<std::chrono::milliseconds>(
            now - g_processSnapshotCacheTime)
                .count() < 1000) {
        return g_processSnapshotCache;
    }

    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) {
        return g_processSnapshotCache;
    }

    std::vector<ProcessSnapshotEntry> result;
    PROCESSENTRY32W pe{};
    pe.dwSize = sizeof(pe);
    if (Process32FirstW(hSnap, &pe)) {
        do {
            ProcessSnapshotEntry entry;
            entry.pid = pe.th32ProcessID;
            entry.parentPid = pe.th32ParentProcessID;
            entry.threadCount = pe.cntThreads;
            entry.name = ToLower(pe.szExeFile);
            result.push_back(std::move(entry));
        } while (Process32NextW(hSnap, &pe));
    }
    CloseHandle(hSnap);

    g_processSnapshotCache = result;
    g_processSnapshotCacheTime = now;
    return g_processSnapshotCache;
}

// ---------------------------------------------------------------------------
// Process Tree Helper
// ---------------------------------------------------------------------------

static void CollectDescendants(
    DWORD rootPid, const std::unordered_map<DWORD, std::vector<DWORD>>& childrenOf,
    std::vector<DWORD>& outDescendants, std::unordered_set<DWORD>& visited) {
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

static bool SetProcessEcoQoS(HANDLE hProcess, bool enableThrottling) {
    PROCESS_POWER_THROTTLING_STATE state{};
    state.Version = PROCESS_POWER_THROTTLING_CURRENT_VERSION;
    state.ControlMask = PROCESS_POWER_THROTTLING_EXECUTION_SPEED |
                        PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION;
    state.StateMask =
        enableThrottling ? (PROCESS_POWER_THROTTLING_EXECUTION_SPEED |
                            PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION)
                         : 0;
    return SetProcessInformation(hProcess,
                                 (PROCESS_INFORMATION_CLASS)ProcessPowerThrottlingInfoClass,
                                 &state, sizeof(state)) != 0;
}

// Explicitly disables EcoQoS on Windows 11 22H2+. ControlMask must specify
// which mechanisms to control; passing 0 is a no-op on modern kernels.
static bool ResetProcessEcoQoS(HANDLE hProcess) {
    PROCESS_POWER_THROTTLING_STATE state{};
    state.Version = PROCESS_POWER_THROTTLING_CURRENT_VERSION;
    state.ControlMask = PROCESS_POWER_THROTTLING_EXECUTION_SPEED |
                        PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION;
    state.StateMask = 0; // Disable all controlled mechanisms
    return SetProcessInformation(hProcess,
                                 (PROCESS_INFORMATION_CLASS)ProcessPowerThrottlingInfoClass,
                                 &state, sizeof(state)) != 0;
}

// Applies or clears Windows thread background processing mode (THREAD_MODE_BACKGROUND_BEGIN / END)
// on all threads of the target process. This sets disk I/O and memory priorities to very low,
// eliminating storage contention and micro-stutters during heavy background operations.
static bool SetProcessThreadsBackgroundMode(DWORD pid, bool enable) {
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hSnap == INVALID_HANDLE_VALUE) {
        return false;
    }
    THREADENTRY32 te{};
    te.dwSize = sizeof(te);
    bool anyApplied = false;
    if (Thread32First(hSnap, &te)) {
        do {
            if (te.th32OwnerProcessID == pid) {
                HANDLE hThread = OpenThread(
                    THREAD_SET_INFORMATION | THREAD_QUERY_LIMITED_INFORMATION,
                    FALSE, te.th32ThreadID);
                if (hThread) {
                    if (SetThreadPriority(hThread, enable ? THREAD_MODE_BACKGROUND_BEGIN
                                                          : THREAD_MODE_BACKGROUND_END)) {
                        anyApplied = true;
                    }
                    CloseHandle(hThread);
                }
            }
        } while (Thread32Next(hSnap, &te));
    }
    CloseHandle(hSnap);
    return anyApplied;
}

// ---------------------------------------------------------------------------
// Foreground Priority Boost (Process Family Zero-Stutter)
// ---------------------------------------------------------------------------

// Checks if process has terminated without requiring SYNCHRONIZE rights.
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
    for (auto& entry : g_boostedProcesses) {
        if (entry.hProcess) {
            if (!IsProcessTerminated(entry.hProcess)) {
                SetPriorityClass(entry.hProcess, entry.originalPriority);
                SetProcessIoPriorityHint(entry.hProcess, entry.originalIoPriority);
                SetProcessMemoryPriorityHint(entry.hProcess, entry.originalMemoryPriority);
                if (entry.cpuSetsApplied && g_pfnSetProcessDefaultCpuSets) {
                    g_pfnSetProcessDefaultCpuSets(entry.hProcess, nullptr, 0);
                }
            }
            CloseHandle(entry.hProcess);
        }
        auto itCtx = g_processContexts.find(entry.pid);
        if (itCtx != g_processContexts.end()) {
            itCtx->second.state = ProcessState::BackgroundNormal;
            itCtx->second.stateEnteredAt = std::chrono::steady_clock::now();
        }
    }
    g_boostedProcesses.clear();
    g_currentBoostedPid.store(0, std::memory_order_release);
    if (g_isHighPerformanceActive.load()) {
        RequestPowerSchemeAsync(false);
    }
}

static void UpdateForegroundBoost(DWORD newForegroundPid,
                                  const ModSettings& settings) {
    std::lock_guard<std::mutex> lock(g_priorityMutex);

    DWORD currentPid = GetCurrentProcessId();

    if (!settings.enableProBalance) {
        RestoreForegroundBoostLocked();
        return;
    }

    DWORD previousBoostedPid =
        g_currentBoostedPid.load(std::memory_order_relaxed);
    if (newForegroundPid == previousBoostedPid) {
        return;
    }

    if (newForegroundPid == 0 || newForegroundPid == currentPid ||
        newForegroundPid == 4) {
        RestoreForegroundBoostLocked();
        return;
    }

    // Discover process tree descendants (helpers, renderers, workers)
    std::vector<ProcessSnapshotEntry> snapshot = CaptureProcessSnapshotCached();
    std::unordered_map<DWORD, std::vector<DWORD>> childrenOf;
    std::unordered_map<DWORD, std::wstring> nameByPid;
    std::unordered_map<DWORD, DWORD> parentOf;
    for (const auto& entry : snapshot) {
        childrenOf[entry.parentPid].push_back(entry.pid);
        nameByPid[entry.pid] = entry.name;
        parentOf[entry.pid] = entry.parentPid;
    }

    auto itRootName = nameByPid.find(newForegroundPid);
    std::wstring rootName =
        (itRootName != nameByPid.end()) ? itRootName->second : L"";
    bool isShellOrLauncher = (rootName == L"explorer.exe" ||
                              rootName == L"cmd.exe" ||
                              rootName == L"powershell.exe" ||
                              rootName == L"pwsh.exe" ||
                              rootName == L"windowsterminal.exe");

    // Family Continuity: preserve elevated state without teardown/reboost jitter
    // when switching between windows belonging to the same application tree or executable.
    bool isFamilyContinuation = false;
    if (previousBoostedPid != 0 && !isShellOrLauncher) {
        for (const auto& entry : g_boostedProcesses) {
            if (entry.pid == newForegroundPid) {
                isFamilyContinuation = true;
                break;
            }
        }

        if (!isFamilyContinuation) {
            auto itPrevName = nameByPid.find(previousBoostedPid);
            if (itPrevName != nameByPid.end() && !itPrevName->second.empty() &&
                _wcsicmp(itPrevName->second.c_str(), rootName.c_str()) == 0) {
                isFamilyContinuation = true;
            }
        }

        if (!isFamilyContinuation) {
            DWORD curr = newForegroundPid;
            for (int depth = 0; depth < 5 && curr != 0; ++depth) {
                auto itP = parentOf.find(curr);
                if (itP != parentOf.end() && itP->second == previousBoostedPid) {
                    isFamilyContinuation = true;
                    break;
                }
                curr = (itP != parentOf.end()) ? itP->second : 0;
            }
        }
    }

    if (!isFamilyContinuation) {
        RestoreForegroundBoostLocked();
    } else {
        auto it = g_boostedProcesses.begin();
        while (it != g_boostedProcesses.end()) {
            if (!it->hProcess || IsProcessTerminated(it->hProcess)) {
                if (it->hProcess) {
                    CloseHandle(it->hProcess);
                }
                it = g_boostedProcesses.erase(it);
            } else {
                ++it;
            }
        }
        LogEvent(LogCategory::ProBalance, LogDetailLevel::Debug,
                 L"Family Continuity maintained: PID %u belongs to active family of PID %u",
                 newForegroundPid, previousBoostedPid);
    }

    std::vector<DWORD> pidsToBoost;
    pidsToBoost.push_back(newForegroundPid);
    std::unordered_set<DWORD> visited;
    visited.insert(newForegroundPid);

    if (!isShellOrLauncher) {
        CollectDescendants(newForegroundPid, childrenOf, pidsToBoost, visited);
    }

    DWORD targetPriority =
        (settings.foregroundPriorityLevel == ForegroundPrioritySetting::High)
            ? HIGH_PRIORITY_CLASS
            : ABOVE_NORMAL_PRIORITY_CLASS;
    constexpr ULONG targetIo = IoPriorityNormal;

    for (DWORD pid : pidsToBoost) {
        if (pid == 0 || pid == 4 || pid == currentPid)
            continue;
        if (IsAccessDeniedImmune(pid))
            continue;

        bool alreadyElevated = false;
        for (const auto& entry : g_boostedProcesses) {
            if (entry.pid == pid) {
                alreadyElevated = true;
                break;
            }
        }
        if (alreadyElevated) {
            continue;
        }

        auto itName = nameByPid.find(pid);
        if (itName != nameByPid.end() && IsEssentialSystemSecurityProcess(itName->second)) {
            continue; // Never alter priority of UAC (consent.exe), Windows Hello, or login UI
        }

        HANDLE hProc =
            OpenProcess(PROCESS_SET_INFORMATION | PROCESS_QUERY_LIMITED_INFORMATION | SYNCHRONIZE,
                        FALSE, pid);
        if (!hProc) {
            if (GetLastError() == ERROR_ACCESS_DENIED) {
                RecordAccessDeniedImmunity(pid);
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
            bool cpuSetsWasApplied = itThrottled->second.cpuSetsApplied;
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
            if (cpuSetsWasApplied && g_pfnSetProcessDefaultCpuSets) {
                g_pfnSetProcessDefaultCpuSets(hProc, nullptr, 0);
            }
        } else {
            DWORD prevPriority = GetPriorityClass(hProc);
            origPriority = (prevPriority != 0) ? prevPriority : NORMAL_PRIORITY_CLASS;
            origIoPriority = GetProcessIoPriorityHint(hProc);
            origMemoryPriority = GetProcessMemoryPriorityHint(hProc);
            // If previously soft-trimmed, ensure restored memory priority returns to NORMAL
            if (origMemoryPriority < 5 /* MEMORY_PRIORITY_NORMAL */) {
                origMemoryPriority = 5;
            }
        }

        // Foreground processes maintain normal memory priority
        SetProcessMemoryPriorityHint(hProc, 5 /* MEMORY_PRIORITY_NORMAL */);

        // Only elevate if original priority was normal or below
        int origRank = PriorityClassToRank(origPriority);
        if (origRank > 0 && origRank <= PriorityClassToRank(NORMAL_PRIORITY_CLASS)) {
            if (SetPriorityClass(hProc, targetPriority)) {
                SetProcessIoPriorityHint(hProc, targetIo);

                // Assign P-cores to the primary foreground process on hybrid architectures
                bool appliedCpuSets = false;
                const SystemHardwareProfile& hw = GetHardwareProfile();
                if (settings.enableForegroundCpuSets && pid == newForegroundPid &&
                    hw.isHybridCpu && g_pfnSetProcessDefaultCpuSets && !hw.pCoreCpuSetIds.empty()) {
                    if (g_pfnSetProcessDefaultCpuSets(hProc, hw.pCoreCpuSetIds.data(),
                                                      static_cast<ULONG>(hw.pCoreCpuSetIds.size()))) {
                        appliedCpuSets = true;
                    }
                }

                g_sessionBoostTransitions.fetch_add(1, std::memory_order_relaxed);
                LogEvent(LogCategory::ProBalance, LogDetailLevel::Detailed,
                         L"Elevating foreground %s (PID %u) -> Priority: %s | I/O: %s%s",
                         (itName != nameByPid.end()) ? itName->second.c_str() : L"process",
                         pid,
                         (targetPriority == HIGH_PRIORITY_CLASS) ? L"High" : L"AboveNormal",
                         L"Normal",
                         appliedCpuSets ? L" | P-Cores Assigned" : L"");

                BoostedProcessEntry entry;
                entry.pid = pid;
                entry.hProcess = hProc;
                entry.originalPriority = origPriority;
                entry.originalIoPriority = origIoPriority;
                entry.originalMemoryPriority = origMemoryPriority;
                entry.cpuSetsApplied = appliedCpuSets;
                g_boostedProcesses.push_back(entry);

                // ARCH-01: Update unified process state machine
                auto& ctx = g_processContexts[pid];
                ctx.pid = pid;
                if (itName != nameByPid.end()) {
                    ctx.name = itName->second;
                }
                ctx.state = ProcessState::ForegroundBoosted;
                ctx.hProcess = hProc;
                ctx.originalPriority = origPriority;
                ctx.originalIoPriority = origIoPriority;
                ctx.originalMemoryPriority = origMemoryPriority;
                ctx.cpuSetsApplied = appliedCpuSets;
                ctx.stateEnteredAt = std::chrono::steady_clock::now();
                continue;
            }
        }

        CloseHandle(hProc);
    }

    g_currentBoostedPid.store(newForegroundPid, std::memory_order_release);
    g_fastBoostedPid.store(newForegroundPid, std::memory_order_release);
    // POWR-01 & POWR-02: Condition power scheme engagement on confirmed active game & dispatch asynchronously
    if (settings.enableDynamicPowerPlan) {
        auto now = std::chrono::steady_clock::now();
        bool isGameActive = IsGamingLockdownActive(
            newForegroundPid, now, settings.gameAltTabGracePeriodSeconds);
        if (isGameActive && !(settings.pauseOnBattery && IsRunningOnBattery())) {
            RequestPowerSchemeAsync(true);
        } else {
            RequestPowerSchemeAsync(false);
        }
    }
}

// ---------------------------------------------------------------------------
// Background CPU/I/O Throttling with Audio & AI Workload Protection
// ---------------------------------------------------------------------------

static double
SampleCpuPercent(DWORD pid, HANDLE hProcess,
                 std::unordered_map<DWORD, CpuSample>& sampleMap = g_cpuSamples) {
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
    ULONGLONG totalTransferBytes = 0;
    std::chrono::steady_clock::time_point sampleTime{};
};
static std::unordered_map<DWORD, IoSample> g_ioSamples;
static std::unordered_map<DWORD, std::chrono::steady_clock::time_point>
    g_ioLastWriteBurst;

struct ProcessIoActivity {
    bool isWritingDisk = false;
    bool isTransferringNetworkOrIo = false;
};

// Detects active disk writes and network/IO streams using dynamic configurable thresholds.
// Features a burst hysteresis timer to shield archive decompression (e.g. 7-Zip LZMA2 pauses in RAM).
static ProcessIoActivity GetProcessIoActivity(DWORD pid, HANDLE hProcess,
                                              const ModSettings& settings) {
    ProcessIoActivity activity{};
    IO_COUNTERS io{};
    if (!GetProcessIoCounters(hProcess, &io))
        return activity;

    auto now = std::chrono::steady_clock::now();
    ULONGLONG totalBytes =
        io.ReadTransferCount + io.WriteTransferCount + io.OtherTransferCount;

    double writeThresholdBps =
        static_cast<double>(settings.ioActivityWriteThresholdKbps) * 1024.0;
    double transferThresholdBps =
        static_cast<double>(settings.ioActivityTransferThresholdKbps) * 1024.0;

    auto it = g_ioSamples.find(pid);
    if (it == g_ioSamples.end()) {
        g_ioSamples[pid] = {io.WriteTransferCount, totalBytes, now};
        return activity;
    }

    double deltaWallMs =
        (double)std::chrono::duration_cast<std::chrono::milliseconds>(
            now - it->second.sampleTime)
            .count();
    if (deltaWallMs >= 500.0) {
        if (io.WriteTransferCount > it->second.writeBytes) {
            double deltaBytes =
                (double)(io.WriteTransferCount - it->second.writeBytes);
            double writeRateBps = (deltaBytes / deltaWallMs) * 1000.0;
            if (writeRateBps >= writeThresholdBps) {
                activity.isWritingDisk = true;
                g_ioLastWriteBurst[pid] = now;
            }
        }
        if (totalBytes > it->second.totalTransferBytes) {
            double deltaTotal =
                (double)(totalBytes - it->second.totalTransferBytes);
            double totalRateBps = (deltaTotal / deltaWallMs) * 1000.0;
            if (totalRateBps >= transferThresholdBps) {
                activity.isTransferringNetworkOrIo = true;
            }
        }
        it->second = {io.WriteTransferCount, totalBytes, now};
    }

    // Compression & installation burst hysteresis: maintain write protection
    // during compute pauses in RAM (e.g. LZMA2 dictionary builds)
    if (!activity.isWritingDisk) {
        auto itBurst = g_ioLastWriteBurst.find(pid);
        if (itBurst != g_ioLastWriteBurst.end()) {
            auto elapsedBurstSec =
                std::chrono::duration_cast<std::chrono::seconds>(now -
                                                                 itBurst->second)
                    .count();
            if (elapsedBurstSec <=
                static_cast<int64_t>(
                    settings.compressionBurstHysteresisSeconds)) {
                activity.isWritingDisk = true;
            }
        }
    }

    return activity;
}

// Samples system-wide CPU usage across all cores to gate background throttling.
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
    bool cpuSetsApplied = it->second.cpuSetsApplied;
    bool threadBackgroundApplied = it->second.threadBackgroundApplied;
    g_throttledProcesses.erase(it);

    auto itCtx = g_processContexts.find(pid);
    if (itCtx != g_processContexts.end()) {
        itCtx->second.state = ProcessState::BackgroundNormal;
        itCtx->second.stateEnteredAt = std::chrono::steady_clock::now();
    }

    if (hSaved) {
        if (!IsProcessTerminated(hSaved)) {
            SetPriorityClass(hSaved, originalPriority);
            SetProcessIoPriorityHint(hSaved, originalIoPriority);
            SetProcessMemoryPriorityHint(hSaved, originalMemoryPriority);
            if (ecoQosApplied) {
                ResetProcessEcoQoS(hSaved);
            }
            if (cpuSetsApplied && g_pfnSetProcessDefaultCpuSets) {
                g_pfnSetProcessDefaultCpuSets(hSaved, nullptr, 0);
            }
            if (threadBackgroundApplied) {
                SetProcessThreadsBackgroundMode(pid, false);
            }
        }
        CloseHandle(hSaved);
    }
    return true;
}

static void RestoreAllThrottledProcesses() {
    std::lock_guard<std::mutex> lock(g_priorityMutex);
    auto now = std::chrono::steady_clock::now();
    for (auto& kv : g_throttledProcesses) {
        auto& info = kv.second;
        auto itCtx = g_processContexts.find(kv.first);
        if (itCtx != g_processContexts.end()) {
            itCtx->second.state = ProcessState::BackgroundNormal;
            itCtx->second.stateEnteredAt = now;
        }
        if (info.hProcess) {
            if (!IsProcessTerminated(info.hProcess)) {
                SetPriorityClass(info.hProcess, info.originalPriority);
                SetProcessIoPriorityHint(info.hProcess, info.originalIoPriority);
                SetProcessMemoryPriorityHint(info.hProcess, info.originalMemoryPriority);
                if (info.ecoQosApplied) {
                    ResetProcessEcoQoS(info.hProcess);
                }
                if (info.cpuSetsApplied && g_pfnSetProcessDefaultCpuSets) {
                    g_pfnSetProcessDefaultCpuSets(info.hProcess, nullptr, 0);
                }
                if (info.threadBackgroundApplied) {
                    SetProcessThreadsBackgroundMode(kv.first, false);
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
    auto* map = reinterpret_cast<std::unordered_map<DWORD, WindowState>*>(lParam);
    if (!IsWindowVisible(hwnd))
        return TRUE;

    // Exclude cloaked windows (virtual desktops or shell-suspended applications)
    int cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) &&
        cloaked != 0) {
        return TRUE;
    }

    // Ignore stub or zero-sized helper windows
    RECT rc{};
    if (GetWindowRect(hwnd, &rc)) {
        if ((rc.right - rc.left) <= 1 || (rc.bottom - rc.top) <= 1) {
            return TRUE;
        }
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    auto& state = (*map)[pid];
    state.hasVisibleWindow = true;
    if (IsIconic(hwnd)) {
        state.isMinimized = true;
    }
    return TRUE;
}

static std::chrono::steady_clock::time_point g_windowStateCacheTime{};
static std::unordered_map<DWORD, WindowState> g_windowStateCache;

static std::unordered_map<DWORD, WindowState> BuildWindowStateMapCached() {
    auto now = std::chrono::steady_clock::now();
    if (g_windowStateCacheTime.time_since_epoch().count() != 0 &&
        std::chrono::duration_cast<std::chrono::milliseconds>(
            now - g_windowStateCacheTime)
                .count() < 2000) {
        return g_windowStateCache;
    }

    std::unordered_map<DWORD, WindowState> map;
    EnumWindows(EnumWindowStateProc, reinterpret_cast<LPARAM>(&map));
    g_windowStateCache = map;
    g_windowStateCacheTime = now;
    return g_windowStateCache;
}

// ---------------------------------------------------------------------------
// Audio Process Tree Expansion Helper (Topological Liveness Sanctuary)
// ---------------------------------------------------------------------------

struct AudioWorkerSample {
    ULONGLONG streamBytes = 0;
    ULONGLONG cpuTime100ns = 0;
    std::chrono::steady_clock::time_point sampleTime{};
};

static std::unordered_map<DWORD, AudioWorkerSample> g_audioWorkerSamples;
static std::unordered_map<DWORD, std::chrono::steady_clock::time_point>
    g_audioWorkerActiveUntil;
static std::unordered_set<DWORD> g_loggedAudioWorkers;

static std::unordered_set<DWORD>
ExpandAudioProcessShield(const std::unordered_set<DWORD>& rawAudioPids,
                         const std::vector<ProcessSnapshotEntry>& /*processList*/,
                         const std::unordered_map<DWORD, std::vector<DWORD>>& childrenOf,
                         const std::unordered_map<DWORD, DWORD>& parentOf,
                         const std::unordered_map<DWORD, WindowState>& windowStates) {
    if (rawAudioPids.empty()) {
        g_loggedAudioWorkers.clear();
        return {};
    }

    auto now = std::chrono::steady_clock::now();
    std::unordered_set<DWORD> activeAudioPids = rawAudioPids;

    for (DWORD aPid : rawAudioPids) {
        // 1. Walk up parentOf to find root ancestor in the user session
        std::unordered_set<DWORD> ancestors{aPid};
        DWORD cur = aPid;
        DWORD rootPid = aPid;
        while (true) {
            auto itP = parentOf.find(cur);
            if (itP == parentOf.end() || itP->second == 0 || itP->second == 4 ||
                !ancestors.insert(itP->second).second)
                break;
            cur = itP->second;
            rootPid = cur;
        }

        // Always protect the direct ancestral lineage orchestrating the audio session
        for (DWORD anc : ancestors) {
            activeAudioPids.insert(anc);
        }

        // 2. Collect all descendant processes across the application tree
        std::vector<DWORD> treeMembers;
        std::unordered_set<DWORD> visited;
        CollectDescendants(rootPid, childrenOf, treeMembers, visited);

        // 3. Strict Liveness Gate: inspect each descendant to avoid immunizing dormant/dead helpers
        for (DWORD dPid : treeMembers) {
            if (activeAudioPids.count(dPid)) {
                continue;
            }

            // A visible, non-minimized GUI window has active user presence
            auto wsIt = windowStates.find(dPid);
            if (wsIt != windowStates.end() && wsIt->second.hasVisibleWindow &&
                !wsIt->second.isMinimized) {
                activeAudioPids.insert(dPid);
                continue;
            }

            // For headless/background workers, require genuine sustained streaming throughput
            // (>= 64 KB/s continuous transfer) or active decoding (>= 2.0% CPU) over >= 1 second
            HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dPid);
            if (hProc) {
                bool isLively = false;
                IO_COUNTERS io{};
                FILETIME creation{}, exit{}, kernel{}, user{};
                if (GetProcessIoCounters(hProc, &io) &&
                    GetProcessTimes(hProc, &creation, &exit, &kernel, &user)) {
                    ULARGE_INTEGER k{}, u{};
                    k.LowPart = kernel.dwLowDateTime;
                    k.HighPart = kernel.dwHighDateTime;
                    u.LowPart = user.dwLowDateTime;
                    u.HighPart = user.dwHighDateTime;
                    ULONGLONG curCpu100ns = k.QuadPart + u.QuadPart;
                    ULONGLONG curStreamBytes =
                        io.ReadTransferCount + io.WriteTransferCount;

                    auto itSample = g_audioWorkerSamples.find(dPid);
                    if (itSample != g_audioWorkerSamples.end()) {
                        double deltaSec =
                            (double)std::chrono::duration_cast<
                                std::chrono::milliseconds>(
                                now - itSample->second.sampleTime)
                                .count() /
                            1000.0;
                        if (deltaSec >= 1.0) {
                            double bytesPerSec =
                                (curStreamBytes >= itSample->second.streamBytes)
                                    ? ((double)(curStreamBytes -
                                                itSample->second.streamBytes) /
                                       deltaSec)
                                    : 0.0;
                            double cpuTimeMs =
                                (curCpu100ns >= itSample->second.cpuTime100ns)
                                    ? ((double)(curCpu100ns -
                                                itSample->second.cpuTime100ns) /
                                       10000.0)
                                    : 0.0;
                            double cpuPercent =
                                (cpuTimeMs / (deltaSec * 1000.0)) * 100.0 /
                                GetSystemCoreCount();

                            // Threshold: >= 64 KB/s streaming throughput or >= 2.0% CPU decoding
                            if (bytesPerSec >= 65536.0 || cpuPercent >= 2.0) {
                                isLively = true;
                                g_audioWorkerActiveUntil[dPid] =
                                    now + std::chrono::seconds(5);
                            }
                            itSample->second = {curStreamBytes, curCpu100ns,
                                                now};
                        }
                    } else {
                        g_audioWorkerSamples[dPid] = {curStreamBytes, curCpu100ns,
                                                      now};
                    }
                }
                CloseHandle(hProc);

                // Hysteresis hold: maintain sanctuary state across momentary network buffers
                if (!isLively) {
                    auto itHyst = g_audioWorkerActiveUntil.find(dPid);
                    if (itHyst != g_audioWorkerActiveUntil.end() &&
                        now < itHyst->second) {
                        isLively = true;
                    }
                }

                if (isLively) {
                    activeAudioPids.insert(dPid);
                    if (g_loggedAudioWorkers.insert(dPid).second) {
                        std::wstring dName = GetProcessNameByPid(dPid);
                        LogEvent(LogCategory::Sanctuary, LogDetailLevel::Detailed,
                                 L"Audio worker/renderer active: %s (PID %u) under parent PID %u -> Shielded via liveness gate",
                                 dName.c_str(), dPid, rootPid);
                    }
                }
            }
        }
    }

    // Debounce log prune: remove PIDs that are no longer active in the audio sanctuary
    for (auto it = g_loggedAudioWorkers.begin(); it != g_loggedAudioWorkers.end();) {
        if (activeAudioPids.count(*it) == 0) {
            it = g_loggedAudioWorkers.erase(it);
        } else {
            ++it;
        }
    }

    return activeAudioPids;
}

// ---------------------------------------------------------------------------
// Dedicated AI Activity Tracker (Independent of Window & Throttle State)
// ---------------------------------------------------------------------------

static void UpdateAiProcessActivity(const ModSettings& settings) {
    if (!settings.enableSmartAiOptimization)
        return;

    auto now = std::chrono::steady_clock::now();
    std::vector<ProcessSnapshotEntry> processList =
        CaptureProcessSnapshotCached();
    std::unordered_set<DWORD> alivePids;
    alivePids.reserve(processList.size());
    for (const auto& entry : processList) {
        alivePids.insert(entry.pid);
    }

    for (const auto& entry : processList) {
        if (!IsAiProcess(entry.pid, entry.name, settings))
            continue;

        DWORD pid = entry.pid;
        if (pid == 0 || pid == 4)
            continue;

        // Freshly detected AI process: initialize timestamp so it starts protected
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
                                 reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc),
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
    PruneDeadPids(g_aiProcessCache, alivePids);
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
    const std::unordered_map<DWORD, DWORD>& parentOf,
    const std::unordered_map<DWORD, WindowState>& windowStates,
    const ModSettings& settings,
    DWORD coreCount) {
    // W11-03 & GAME-01: DirectStorage & Game Sanctuary full tree immunity
    DWORD gameSanctuaryPid = GetLiveGameSanctuaryPid();
    if (gameSanctuaryPid != 0) {
        if (pid == gameSanctuaryPid) {
            return true;
        }
        DWORD checkAnc = pid;
        for (int depth = 0; depth < 8; ++depth) {
            auto itP = parentOf.find(checkAnc);
            if (itP == parentOf.end() || itP->second == 0 || itP->second == 4) {
                break;
            }
            if (itP->second == gameSanctuaryPid) {
                return true;
            }
            checkAnc = itP->second;
        }
    }

    // Cooperative priority: processes already at BELOW_NORMAL or IDLE are skipped
    if (!isThrottled) {
        DWORD prio = GetPriorityClass(hProcess);
        if (prio == BELOW_NORMAL_PRIORITY_CLASS || prio == IDLE_PRIORITY_CLASS) {
            return true;
        }
    }

    auto now = std::chrono::steady_clock::now();

    // Lineage check: shield workers descended from active or recently focused GUI applications
    int graceMinutes = settings.recentActivityGraceMinutes;
    if (graceMinutes < 2)
        graceMinutes = 2;
    auto maxInactiveDuration = std::chrono::minutes(graceMinutes);

    DWORD current = pid;
    for (int depth = 0; depth < 8; ++depth) {
        auto itParent = parentOf.find(current);
        if (itParent == parentOf.end() || itParent->second == 0 || itParent->second == 4) {
            break;
        }
        DWORD parentPid = itParent->second;
        if (parentPid == current)
            break; // Avoid cycles

        auto itWin = windowStates.find(parentPid);
        if (itWin != windowStates.end() && itWin->second.hasVisibleWindow && !itWin->second.isMinimized) {
            return true;
        }

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

    // SHAD-01: Universal Shader & Parallel Compute Heuristic (Zero Hardcoding)
    // Headless multi-threaded compute task (PSO compilation, DX12/Vulkan shader workers, asset prep)
    auto wsIt = windowStates.find(pid);
    bool hasVisibleGui = (wsIt != windowStates.end() && wsIt->second.hasVisibleWindow && !wsIt->second.isMinimized);
    if (!hasVisibleGui && threadCount >= 4 && cpuPercent >= 15.0) {
        DWORD curAnc = pid;
        for (int depth = 0; depth < 8; ++depth) {
            auto itP = parentOf.find(curAnc);
            if (itP == parentOf.end() || itP->second == 0 || itP->second == 4) {
                break;
            }
            auto itWin = windowStates.find(itP->second);
            if (itWin != windowStates.end() && itWin->second.hasVisibleWindow) {
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

    // Parallel compute heuristic: multi-threaded worker pools scaling with core count
    DWORD minParallelThreads = (coreCount > 4) ? (coreCount / 2) : 4;
    if (threadCount >= minParallelThreads && cpuPercent >= 15.0) {
        {
            std::lock_guard<std::mutex> lock(g_focusMapMutex);
            auto itSelfFocus = g_processLastFocusedTime.find(pid);
            if (itSelfFocus != g_processLastFocusedTime.end()) {
                if ((now - itSelfFocus->second) <= std::chrono::minutes(30)) {
                    return true;
                }
            }
        }
        DWORD curAnc = pid;
        for (int depth = 0; depth < 8; ++depth) {
            auto itP = parentOf.find(curAnc);
            if (itP == parentOf.end() || itP->second == 0 || itP->second == 4)
                break;
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

    // Heavy compute sanctuary: protects background tasks with working set >= 2000 MB and threads >= 4
    PROCESS_MEMORY_COUNTERS pmc{};
    pmc.cb = sizeof(pmc);
    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
        SIZE_T wsMb = pmc.WorkingSetSize / (1024ULL * 1024ULL);
        if (threadCount >= 4 && wsMb >= 2000) {
            return true;
        }
    }

    return false;
}

// ---------------------------------------------------------------------------
// Central Action & Immunity Arbiter (Mediator / Policy Engine)
// ---------------------------------------------------------------------------

enum class ActionType : uint8_t {
    ThrottleCpu,
    TrimWorkingSet
};

enum class VetoReason : uint16_t {
    None = 0,
    KernelSecurity = 1 << 0,         // System PID, essential dialogs, or excluded (Weight: 1000)
    AntiCheatProtected = 1 << 1,     // Access denied / anti-cheat PPL (Weight: 1000)
    NonInteractiveSession = 1 << 2,  // Non-interactive session 0 services (Weight: 1000)
    ForegroundOrFamily = 1 << 3,     // Foreground process or UI descendant helpers (Weight: 980)
    AudioSessionActive = 1 << 4,     // Active WASAPI audio session (Weight: 950)
    AiInferenceActive = 1 << 5,      // Active local AI token generation (Weight: 900)
    AiGracePeriod = 1 << 6,          // Dormant AI within warm grace period (Weight: 900)
    VoluntaryCompute = 1 << 7,       // Multi-threaded voluntary compute workload (Weight: 880)
    DiskWriteActive = 1 << 8,        // Active disk write transfer (Weight: 850)
    NetworkStreaming = 1 << 9,       // Active network download or VoIP stream (Weight: 800)
    VisibleGuiWindow = 1 << 10,      // Visible non-minimized GUI window (Weight: 750)
    UserRecentFocus = 1 << 11,       // Recent user interaction grace (Weight: 600)
    TargetFilterExclusion = 1 << 12, // targetProcessesOnly is active and not listed (Weight: 500)
    PackagedApp = 1 << 13,           // UWP/MSIX managed by Windows PLM (Weight: 450)
};

struct DecisionResult {
    bool isAllowed = false;
    VetoReason primaryVeto = VetoReason::None;
    const wchar_t* explanation = L"Allowed";
};

struct SystemActionArbiter {
    const ModSettings& settings;
    DWORD currentPid = 0;
    DWORD foregroundPid = 0;
    const std::unordered_set<DWORD>& activeAudioPids;
    const std::unordered_set<DWORD>& immuneFamilyPids;
    std::chrono::steady_clock::time_point now;

    [[nodiscard]] DecisionResult EvaluatePreOpen(
        DWORD pid,
        const std::wstring& name,
        ActionType action,
        const std::unordered_map<DWORD, WindowState>* windowStates = nullptr,
        int64_t graceSeconds = 0) const noexcept {
        if (pid == 0 || pid == 4 || pid == currentPid) {
            return {false, VetoReason::KernelSecurity, L"System / Self PID"};
        }
        if (IsInList(name, settings.excludedProcesses) ||
            IsEssentialSystemSecurityProcess(name)) {
            return {false, VetoReason::KernelSecurity,
                    L"Excluded or Essential System Process"};
        }
        if (IsAccessDeniedImmune(pid)) {
            return {false, VetoReason::AntiCheatProtected,
                    L"Anti-Cheat / PPL Protected (Cached)"};
        }
        if (!IsInteractiveSessionProcess(pid)) {
            return {false, VetoReason::NonInteractiveSession,
                    L"Non-Interactive Session (Session 0)"};
        }
        if (foregroundPid != 0 && pid == foregroundPid) {
            return {false, VetoReason::ForegroundOrFamily,
                    L"Active Foreground Application"};
        }
        if (immuneFamilyPids.count(pid)) {
            return {false, VetoReason::ForegroundOrFamily,
                    L"Foreground Family / Visible UI Helper"};
        }
        if (settings.enableAudioShielding && activeAudioPids.count(pid)) {
            return {false, VetoReason::AudioSessionActive,
                    L"Active WASAPI Audio Session"};
        }
        if (settings.enableSmartAiOptimization &&
            IsAiProcess(pid, name, settings)) {
            auto itAi = g_aiLastInferenceTime.find(pid);
            if (itAi == g_aiLastInferenceTime.end()) {
                g_aiLastInferenceTime[pid] = now;
                return {false, VetoReason::AiInferenceActive,
                        L"AI Process (Freshly detected, protected)"};
            }
            if (action == ActionType::TrimWorkingSet) {
                auto inactiveAiSec =
                    std::chrono::duration_cast<std::chrono::seconds>(now -
                                                                     itAi->second)
                        .count();
                if (inactiveAiSec <
                    static_cast<int64_t>(settings.aiInactivityGraceMinutes) *
                        60) {
                    return {false, VetoReason::AiGracePeriod,
                            L"AI Model Warmth Grace Period Active"};
                }
            }
        }
        if (action == ActionType::TrimWorkingSet && settings.targetProcessesOnly) {
            if (!IsInList(name, settings.customTargetList)) {
                return {false, VetoReason::TargetFilterExclusion,
                        L"Process Not in Custom Target List"};
            }
        }
        if (windowStates) {
            auto wsIt = windowStates->find(pid);
            bool hasVisibleWindow =
                (wsIt != windowStates->end() && wsIt->second.hasVisibleWindow);
            bool isMinimized =
                (wsIt != windowStates->end() && wsIt->second.isMinimized);

            if (action == ActionType::ThrottleCpu) {
                if (hasVisibleWindow && !isMinimized) {
                    return {false, VetoReason::VisibleGuiWindow,
                            L"Visible Non-Minimized GUI Window"};
                }
            } else if (action == ActionType::TrimWorkingSet) {
                if (!settings.trimMinimizedWindows && isMinimized) {
                    return {false, VetoReason::VisibleGuiWindow,
                            L"Minimized Windows Trimming Disabled"};
                }
                if (hasVisibleWindow && !isMinimized) {
                    return {false, VetoReason::VisibleGuiWindow,
                            L"Visible Non-Minimized GUI Window"};
                }
                if (!isMinimized && settings.enableProcessAging &&
                    graceSeconds > 0) {
                    std::lock_guard<std::mutex> lock(g_focusMapMutex);
                    auto itFocus = g_processLastFocusedTime.find(pid);
                    if (itFocus != g_processLastFocusedTime.end()) {
                        auto inactiveSeconds =
                            std::chrono::duration_cast<std::chrono::seconds>(
                                now - itFocus->second)
                                .count();
                        if (inactiveSeconds < graceSeconds) {
                            return {false, VetoReason::UserRecentFocus,
                                    L"Recent User Interaction Grace Period"};
                        }
                    }
                }
            }
        }
        return {true, VetoReason::None, L"Eligible for Inspection"};
    }

    [[nodiscard]] DecisionResult EvaluatePostOpen(
        DWORD pid,
        HANDLE hProcess,
        ActionType action,
        const ProcessIoActivity& ioAct,
        bool isThrottled = false,
        DWORD threadCount = 0,
        double cpuPercent = 0.0,
        const std::unordered_map<DWORD, DWORD>* parentOf = nullptr,
        const std::unordered_map<DWORD, WindowState>* windowStates =
            nullptr) const noexcept {
        if (action == ActionType::TrimWorkingSet) {
            if (pid == foregroundPid || immuneFamilyPids.count(pid)) {
                return {false, VetoReason::ForegroundOrFamily,
                        L"Foreground Application or Family"};
            }
            if (windowStates) {
                auto wsIt = windowStates->find(pid);
                if (wsIt != windowStates->end() && wsIt->second.hasVisibleWindow &&
                    !wsIt->second.isMinimized) {
                    return {false, VetoReason::VisibleGuiWindow,
                            L"Visible Non-Minimized GUI Window"};
                }
            }
            // VRAM-01: Protect active AI inference, model staging, and Shared GPU Memory
            if (settings.enableSmartAiOptimization) {
                auto itAi = g_aiLastInferenceTime.find(pid);
                if (itAi != g_aiLastInferenceTime.end()) {
                    auto elapsedSec = std::chrono::duration_cast<std::chrono::seconds>(
                                          now - itAi->second)
                                          .count();
                    if (elapsedSec <
                        static_cast<int64_t>(settings.aiInactivityGraceMinutes) * 60) {
                        return {false, VetoReason::AiInferenceActive,
                                L"Protected Local AI Model / VRAM Shared GPU Memory"};
                    }
                }
            }
            // VRAM-01 & SHAD-01: Protect active voluntary compute and shader staging from working set trimming
            if (parentOf && windowStates) {
                const SystemHardwareProfile& hw = GetHardwareProfile();
                if (IsVoluntaryComputeTask(pid, hProcess, threadCount, cpuPercent,
                                           false, *parentOf, *windowStates,
                                           settings, hw.coreCount)) {
                    return {false, VetoReason::VoluntaryCompute,
                            L"Protected Compute / Shader / GPU Staging Workload"};
                }
            }
        }
        if (IsPackagedAppCached(pid, hProcess)) {
            return {false, VetoReason::PackagedApp,
                    L"UWP/MSIX App Managed by Windows PLM"};
        }
        if (action == ActionType::ThrottleCpu &&
            settings.enableSmartAiOptimization) {
            auto itAi = g_aiLastInferenceTime.find(pid);
            if (itAi != g_aiLastInferenceTime.end()) {
                bool isGenerating =
                    (std::chrono::duration_cast<std::chrono::seconds>(
                         now - itAi->second)
                         .count() < 10);
                if (isGenerating) {
                    return {false, VetoReason::AiInferenceActive,
                            L"Active Local AI Token Generation"};
                }
            }
        }
        if (action == ActionType::ThrottleCpu && parentOf && windowStates) {
            const SystemHardwareProfile& hw = GetHardwareProfile();
            if (IsVoluntaryComputeTask(pid, hProcess, threadCount, cpuPercent,
                                       isThrottled, *parentOf, *windowStates,
                                       settings, hw.coreCount)) {
                return {false, VetoReason::VoluntaryCompute,
                        L"Voluntary Cooperative Compute Workload"};
            }
        }
        if (settings.enableNetworkShielding && ioAct.isTransferringNetworkOrIo) {
            return {false, VetoReason::NetworkStreaming,
                    L"Active Network Download / Streaming / VoIP"};
        }
        return {true, VetoReason::None, L"Action Approved"};
    }
};

static void ApplyBackgroundThrottling(const ModSettings& settings,
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

    std::vector<ProcessSnapshotEntry> snapshot = CaptureProcessSnapshotCached();
    std::unordered_set<DWORD> alivePids;
    std::unordered_map<DWORD, std::wstring> procNames;
    std::unordered_map<DWORD, std::vector<DWORD>> childrenOf;
    std::unordered_map<DWORD, DWORD> parentOf;
    std::unordered_map<DWORD, DWORD> threadCounts;
    for (const auto& entry : snapshot) {
        alivePids.insert(entry.pid);
        procNames[entry.pid] = entry.name;
        childrenOf[entry.parentPid].push_back(entry.pid);
        parentOf[entry.pid] = entry.parentPid;
        threadCounts[entry.pid] = entry.threadCount;
    }

    std::unordered_map<DWORD, WindowState> windowStates = BuildWindowStateMapCached();

    // Expand audio shield to entire process tree & executable family
    std::unordered_set<DWORD> activeAudioPids =
        ExpandAudioProcessShield(rawAudioPids, snapshot, childrenOf, parentOf, windowStates);

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

    // Immunize descendants of visible windows to prevent IPC priority inversions
    std::unordered_set<DWORD> visibleFamilyPids = fgFamilyPids;
    for (const auto& kv : windowStates) {
        if (kv.second.hasVisibleWindow && !kv.second.isMinimized) {
            visibleFamilyPids.insert(kv.first);
            std::vector<DWORD> desc;
            std::unordered_set<DWORD> visited;
            CollectDescendants(kv.first, childrenOf, desc, visited);
            for (DWORD dPid : desc) {
                visibleFamilyPids.insert(dPid);
            }
        }
    }

    DWORD gameSanctuaryPid = GetLiveGameSanctuaryPid();
    auto sanctuaryRoot = procNames.find(gameSanctuaryPid);
    if (sanctuaryRoot != procNames.end()) {
        std::vector<DWORD> sanctuaryDescendants;
        std::unordered_set<DWORD> visited;
        CollectDescendants(gameSanctuaryPid, childrenOf, sanctuaryDescendants, visited);
        visibleFamilyPids.insert(gameSanctuaryPid);
        for (DWORD sanctuaryChild : sanctuaryDescendants) {
            visibleFamilyPids.insert(sanctuaryChild);
        }
    }

    SystemActionArbiter arbiter{settings, currentPid, foregroundPid,
                                activeAudioPids, visibleFamilyPids, now};

    // PERF-01: Persistent differential sampling watchlist for calm background processes
    static std::unordered_map<DWORD, std::chrono::steady_clock::time_point> g_lastCalmSampleTime;

    for (DWORD pid : alivePids) {
        if (pid == 0 || pid == 4 || pid == currentPid)
            continue;

        // PERF-01: Filter non-interactive Session 0 system services early
        DWORD sessionId = 0;
        if (ProcessIdToSessionId(pid, &sessionId) && sessionId == 0) {
            continue;
        }

        bool isThrottled = false;
        {
            std::lock_guard<std::mutex> lock(g_priorityMutex);
            isThrottled = (g_throttledProcesses.count(pid) != 0);
        }

        // PERF-01: Differential sampling watchlist.
        // Calm background processes (< 5% CPU in previous cycle and not throttled)
        // are only sampled once every 30 seconds, maintaining 0.00% idle CPU.
        if (!isThrottled) {
            auto itCalm = g_lastCalmSampleTime.find(pid);
            if (itCalm != g_lastCalmSampleTime.end()) {
                auto elapsedCalm = std::chrono::duration_cast<std::chrono::seconds>(
                                       now - itCalm->second)
                                       .count();
                if (elapsedCalm < 30) {
                    continue; // Skip querying calm background process
                }
            }
        }

        const std::wstring& name = procNames[pid];
        auto preCheck = arbiter.EvaluatePreOpen(pid, name, ActionType::ThrottleCpu,
                                                &windowStates);
        if (!preCheck.isAllowed) {
            if (preCheck.primaryVeto == VetoReason::ForegroundOrFamily ||
                preCheck.primaryVeto == VetoReason::AudioSessionActive ||
                preCheck.primaryVeto == VetoReason::VisibleGuiWindow) {
                RestoreAndEraseThrottledProcess(pid);
            }
            continue;
        }

        HANDLE hProc =
            OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_SET_INFORMATION | SYNCHRONIZE,
                        FALSE, pid);
        if (!hProc) {
            if (GetLastError() == ERROR_ACCESS_DENIED) {
                RecordAccessDeniedImmunity(pid);
                g_accessDeniedCount.fetch_add(1, std::memory_order_relaxed);
            }
            continue;
        }

        double cpuPercent = SampleCpuPercent(pid, hProc);
        if (cpuPercent >= 0.0 && cpuPercent < 5.0 && !isThrottled) {
            g_lastCalmSampleTime[pid] = now;
        } else {
            g_lastCalmSampleTime.erase(pid); // Place on active high-frequency watchlist
        }

        DWORD threads = 0;
        auto itThCnt = threadCounts.find(pid);
        if (itThCnt != threadCounts.end()) {
            threads = itThCnt->second;
        }

        ProcessIoActivity ioAct = GetProcessIoActivity(pid, hProc, settings);

        auto postCheck = arbiter.EvaluatePostOpen(
            pid, hProc, ActionType::ThrottleCpu, ioAct, isThrottled, threads,
            cpuPercent, &parentOf, &windowStates);
        if (!postCheck.isAllowed) {
            if (isThrottled &&
                (postCheck.primaryVeto == VetoReason::AiInferenceActive ||
                 postCheck.primaryVeto == VetoReason::VoluntaryCompute ||
                 postCheck.primaryVeto == VetoReason::NetworkStreaming)) {
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

        if (isThrottled && (ioAct.isWritingDisk || ioAct.isTransferringNetworkOrIo)) {
            // Restore every mod-controlled resource before allowing active transfers.
            RestoreAndEraseThrottledProcess(pid);
            isThrottled = false;
        }

        if (isCpuHeavy && !isThrottled && systemUnderContention) {
            DWORD prevPriority = GetPriorityClass(hProc);
            if (PriorityClassToRank(prevPriority) >
                    PriorityClassToRank(IDLE_PRIORITY_CLASS) &&
                PriorityClassToRank(prevPriority) <=
                    PriorityClassToRank(NORMAL_PRIORITY_CLASS)) {
                // Floor throttling at BELOW_NORMAL to prevent priority inversion deadlocks on IPC/mutexes
                DWORD targetThrottlePrio = BELOW_NORMAL_PRIORITY_CLASS;

                if (PriorityClassToRank(prevPriority) >
                    PriorityClassToRank(targetThrottlePrio)) {
                    ULONG prevIo = GetProcessIoPriorityHint(hProc);
                    ULONG prevMem = GetProcessMemoryPriorityHint(hProc);
                    bool appliedEcoQos = false;
                    bool appliedCpuSets = false;
                    bool appliedThreadBackground = false;
                    {
                        std::lock_guard<std::mutex> lock(g_priorityMutex);
                        if (pid != g_currentBoostedPid.load(std::memory_order_relaxed) &&
                            g_throttledProcesses.count(pid) == 0) {
                            if (SetPriorityClass(hProc, targetThrottlePrio)) {
                                SetProcessMemoryPriorityHint(hProc, 1 /* MEMORY_PRIORITY_VERY_LOW */);

                                // Never degrade I/O priority, apply EcoQoS, or thread background mode on active disk/network transfers
                                if (!ioAct.isWritingDisk && !ioAct.isTransferringNetworkOrIo) {
                                    SetProcessIoPriorityHint(hProc, IoPriorityLow);
                                    if (settings.enableEcoQosManagement) {
                                        appliedEcoQos =
                                            SetProcessEcoQoS(hProc, /*enableThrottling=*/true);
                                    }
                                    if (settings.enableThreadBackgroundMode) {
                                        appliedThreadBackground =
                                            SetProcessThreadsBackgroundMode(pid, true);
                                    }
                                }

                                // Hybrid Architecture: Confine throttled background process to E-cores
                                const SystemHardwareProfile& hwProfile = GetHardwareProfile();
                                if (settings.enableBackgroundCpuSets && hwProfile.isHybridCpu &&
                                    g_pfnSetProcessDefaultCpuSets && !hwProfile.eCoreCpuSetIds.empty()) {
                                    if (g_pfnSetProcessDefaultCpuSets(
                                            hProc, hwProfile.eCoreCpuSetIds.data(),
                                            static_cast<ULONG>(hwProfile.eCoreCpuSetIds.size()))) {
                                        appliedCpuSets = true;
                                    }
                                }

                                g_sessionThrottleTransitions.fetch_add(1, std::memory_order_relaxed);
                                LogEvent(LogCategory::Throttler, LogDetailLevel::Detailed,
                                         L"Throttling background %s (PID %u): CPU load %.1f%% >= %d%% -> Priority: %s | I/O: Low%s%s%s",
                                         name.c_str(), pid, cpuPercent,
                                         settings.backgroundCpuThrottleThresholdPercent,
                                         (targetThrottlePrio == IDLE_PRIORITY_CLASS) ? L"Idle" : L"BelowNormal",
                                         appliedEcoQos ? L" | EcoQoS" : L"",
                                         appliedCpuSets ? L" | E-Cores Confined" : L"",
                                         appliedThreadBackground ? L" | Thread Background Mode" : L"");

                                g_throttledProcesses[pid] = {hProc, prevPriority, prevIo,
                                                             prevMem, appliedEcoQos, appliedCpuSets,
                                                             appliedThreadBackground, 0};

                                // ARCH-01: Update unified process state machine
                                auto& ctx = g_processContexts[pid];
                                ctx.pid = pid;
                                ctx.name = name;
                                ctx.state = ProcessState::BackgroundThrottled;
                                ctx.hProcess = hProc;
                                ctx.originalPriority = prevPriority;
                                ctx.originalIoPriority = prevIo;
                                ctx.originalMemoryPriority = prevMem;
                                ctx.ecoQosApplied = appliedEcoQos;
                                ctx.cpuSetsApplied = appliedCpuSets;
                                ctx.threadBackgroundApplied = appliedThreadBackground;
                                ctx.stateEnteredAt = now;

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
            // Restore process after 5 consecutive missing CPU samples
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
    PruneDeadPids(g_lastCalmSampleTime, alivePids);
    PruneDeadPids(g_ioSamples, alivePids);
    PruneDeadPids(g_audioWorkerSamples, alivePids);
    PruneDeadPids(g_audioWorkerActiveUntil, alivePids);
    PruneDeadPids(g_aiLastInferenceTime, alivePids);
    PruneDeadPids(g_aiLastWorkingSetSize, alivePids);
    PruneDeadPids(g_aiProcessCache, alivePids);
    PruneAccessDeniedImmunity(alivePids);
    PruneClassifyCache(alivePids);
}

// ---------------------------------------------------------------------------
// Fullscreen / Direct3D Game Detection
// ---------------------------------------------------------------------------

static bool IsExcludedFromGameDetection(const std::wstring& name) {
    static const std::unordered_set<std::wstring> kNonGames = {
        L"chrome.exe", L"msedge.exe", L"firefox.exe", L"brave.exe",
        L"opera.exe", L"vivaldi.exe", L"zen.exe",
        L"powerpnt.exe", L"excel.exe", L"winword.exe", L"outlook.exe",
        L"mpv.exe", L"vlc.exe", L"wmplayer.exe", L"potplayer64.exe",
        L"mpc-hc.exe", L"mpc-hc64.exe", L"mpc-be.exe", L"mpc-be64.exe", L"kodi.exe",
        L"foobar2000.exe", L"zoom.exe", L"teams.exe", L"ms-teams.exe",
        L"obs64.exe", L"obs32.exe",
        L"mstsc.exe", L"teamviewer.exe", L"anydesk.exe",
        L"windowsterminal.exe", L"cmd.exe", L"powershell.exe", L"conhost.exe",
        L"explorer.exe"};
    return kNonGames.count(name) != 0;
}

static bool IsLikelyGameOrFullscreenWindow(HWND hwnd, DWORD pid) {
    if (!hwnd || !IsWindowVisible(hwnd) || pid == 0)
        return false;

    // Exclude shell and desktop window classes carrying WS_POPUP to prevent false-positive sweeps
    wchar_t className[64]{};
    GetClassNameW(hwnd, className, _countof(className));
    static const std::unordered_set<std::wstring> kSystemClasses = {
        L"Shell_TrayWnd", L"Progman", L"WorkerW",
        L"#32768", // Win32 menu
        L"#32769", // Desktop
        L"tooltips_class32",
        L"IME",
        L"MSCTFIME UI",
        L"Windows.UI.Core.CoreWindow" // UWP shell overlays
    };
    if (kSystemClasses.count(className) != 0)
        return false;

    // Exclusion check for media players, browsers, and terminals.
    std::vector<ProcessSnapshotEntry> snapshot = CaptureProcessSnapshotCached();
    for (const auto& entry : snapshot) {
        if (entry.pid == pid) {
            if (IsExcludedFromGameDetection(entry.name)) {
                return false;
            }
            break;
        }
    }

    // Direct3D exclusive fullscreen check (fast-path, works on older Windows too).
    if (g_pfnSHQueryUserNotificationState) {
        QUERY_USER_NOTIFICATION_STATE quns = QUNS_NOT_PRESENT;
        if (SUCCEEDED(g_pfnSHQueryUserNotificationState(&quns))) {
            if (quns == QUNS_RUNNING_D3D_FULL_SCREEN) {
                return true;
            }
        }
    }

    // Use DWM extended frame bounds to get the actual rendered rect without shadow margins
    RECT rcBounds{};
    if (FAILED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS,
                                     &rcBounds, sizeof(rcBounds)))) {
        GetWindowRect(hwnd, &rcBounds);
    }

    HMONITOR hMon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(hMon, &mi))
        return false;

    // Borderless/popup window must fully cover the monitor (allowing 2px for DWM rounding)
    bool coversMonitor = (rcBounds.left <= mi.rcMonitor.left + 2 &&
                          rcBounds.top <= mi.rcMonitor.top + 2 &&
                          rcBounds.right >= mi.rcMonitor.right - 2 &&
                          rcBounds.bottom >= mi.rcMonitor.bottom - 2);

    if (!coversMonitor)
        return false;

    LONG style = GetWindowLongW(hwnd, GWL_STYLE);
    bool borderless = (style & WS_CAPTION) == 0 && (style & WS_THICKFRAME) == 0;

    return borderless || (style & WS_POPUP) != 0;
}

static void MaybeRequestGameSweep(DWORD pid, HWND hwnd,
                                  const ModSettings& settings) {
    if (!settings.enableGameModeDetection || pid == 0)
        return;
    if (!IsLikelyGameOrFullscreenWindow(hwnd, pid))
        return;

    // Keep the detected game family protected after Alt-Tab while Unity continues
    // streaming assets, compiling shaders, or writing save data in the background.
    SetGameSanctuary(pid);

    if (pid == g_lastGameSweepPid)
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

    LogEvent(LogCategory::Sanctuary, LogDetailLevel::Detailed,
             L"3D / Fullscreen game detected (PID %u). Requesting pre-game RAM sweep...", pid);

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
    bool isSoftTrim = false;
    SIZE_T bytesFreed = 0;
    SIZE_T standbyDemotedBytes = 0;
    SIZE_T beforeBytes = 0;
    SIZE_T afterBytes = 0;
};

struct TrimStats {
    DWORD processesTrimmed = 0;
    DWORD processesSoftTrimmed = 0;
    DWORD processesHardTrimmed = 0;
    DWORD processesSkippedRecent = 0;
    SIZE_T bytesReclaimed = 0;
    SIZE_T bytesStandbyDemoted = 0;
    std::vector<AppTrimEntry> topApps;
};

struct TrimAttemptResult {
    bool trimmed = false;
    bool isSoftTrim = false;
    SIZE_T freedBytes = 0;
    SIZE_T standbyDemotedBytes = 0;
    SIZE_T beforeBytes = 0;
    SIZE_T afterBytes = 0;
};

static TrimAttemptResult
TryTrimProcess(DWORD pid, const SystemActionArbiter& arbiter,
               const std::unordered_map<DWORD, WindowState>* windowStates = nullptr,
               bool emergency = false,
               bool forceHardTrim = false) {
    TrimAttemptResult result;
    const ModSettings& settings = arbiter.settings;
    auto now = arbiter.now;

    if (IsAccessDeniedImmune(pid))
        return result;

    // Request PROCESS_SET_QUOTA only for hard trim operations (soft trim needs only PROCESS_SET_INFORMATION)
    DWORD accessRights =
        PROCESS_SET_INFORMATION | PROCESS_QUERY_LIMITED_INFORMATION;
    if (forceHardTrim || emergency) {
        accessRights |= PROCESS_SET_QUOTA;
    }

    HANDLE hProc = OpenProcess(accessRights, FALSE, pid);
    if (!hProc) {
        if (GetLastError() == ERROR_ACCESS_DENIED) {
            RecordAccessDeniedImmunity(pid);
            g_accessDeniedCount.fetch_add(1, std::memory_order_relaxed);
        }
        return result;
    }

    ProcessIoActivity ioAct = GetProcessIoActivity(pid, hProc, settings);
    auto postCheck = arbiter.EvaluatePostOpen(
        pid, hProc, ActionType::TrimWorkingSet, ioAct, false, 0, 0.0, nullptr,
        windowStates);
    if (!postCheck.isAllowed) {
        CloseHandle(hProc);
        return result;
    }

    PROCESS_MEMORY_COUNTERS_EX pmc;
    ZeroMemory(&pmc, sizeof(pmc));
    pmc.cb = sizeof(pmc);

    if (GetProcessMemoryInfo(hProc,
                             reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc),
                             sizeof(pmc))) {
        SIZE_T wsMb = pmc.WorkingSetSize / (1024ULL * 1024ULL);
        bool isMemoryCapExceeded =
            settings.enableElectronMemoryCap &&
            (wsMb >= static_cast<SIZE_T>(settings.electronMemoryCapMb));

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

            // Hard eviction (SetProcessWorkingSetSize) is reserved for emergency memory pressure or configured caps
            if (forceHardTrim || emergency || isMemoryCapExceeded) {
                // If hard trim is requested due to memory cap but handle lacks quota, attempt upgrade
                if (!(accessRights & PROCESS_SET_QUOTA)) {
                    HANDLE hQuotaProc = OpenProcess(
                        accessRights | PROCESS_SET_QUOTA, FALSE, pid);
                    if (hQuotaProc) {
                        CloseHandle(hProc);
                        hProc = hQuotaProc;
                        accessRights |= PROCESS_SET_QUOTA;
                    }
                }

                if (accessRights & PROCESS_SET_QUOTA) {
                    SIZE_T beforeBytes = pmc.WorkingSetSize;
                    if (SetProcessWorkingSetSize(hProc, static_cast<SIZE_T>(-1),
                                                 static_cast<SIZE_T>(-1))) {
                        g_processLastTrimmed[pid] = now;
                        g_aiLastWorkingSetSize.erase(pid);

                        PROCESS_MEMORY_COUNTERS_EX afterPmc;
                        ZeroMemory(&afterPmc, sizeof(afterPmc));
                        afterPmc.cb = sizeof(afterPmc);
                        if (GetProcessMemoryInfo(
                                hProc,
                                reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&afterPmc),
                                sizeof(afterPmc))) {
                            if (beforeBytes > afterPmc.WorkingSetSize) {
                                result.trimmed = true;
                                result.isSoftTrim = false;
                                result.beforeBytes = beforeBytes;
                                result.afterBytes = afterPmc.WorkingSetSize;
                                result.freedBytes =
                                    beforeBytes - afterPmc.WorkingSetSize;
                                result.standbyDemotedBytes = 0;
                            }
                        }
                    }
                } else {
                    // Fall back to soft trim
                    ULONG currentMemPri = GetProcessMemoryPriorityHint(hProc);
                    if (currentMemPri > 2 /* MEMORY_PRIORITY_LOW */) {
                        SetProcessMemoryPriorityHint(
                            hProc, 2 /* MEMORY_PRIORITY_LOW */);
                        g_processLastTrimmed[pid] = now;

                        result.trimmed = true;
                        result.isSoftTrim = true;
                        result.beforeBytes = pmc.WorkingSetSize;
                        result.afterBytes = pmc.WorkingSetSize;
                        result.freedBytes = 0;
                        result.standbyDemotedBytes = pmc.WorkingSetSize;
                    }
                }
            } else {
                // Soft trim: demote memory priority to let standby list manager gradually reclaim pages
                ULONG currentMemPri = GetProcessMemoryPriorityHint(hProc);
                if (currentMemPri > 2 /* MEMORY_PRIORITY_LOW */) {
                    SetProcessMemoryPriorityHint(
                        hProc, 2 /* MEMORY_PRIORITY_LOW */);
                    g_processLastTrimmed[pid] = now;

                    result.trimmed = true;
                    result.isSoftTrim = true;
                    result.beforeBytes = pmc.WorkingSetSize;
                    result.afterBytes = pmc.WorkingSetSize;
                    result.freedBytes = 0;
                    result.standbyDemotedBytes = pmc.WorkingSetSize;
                }
            }
        }
    }

    CloseHandle(hProc);
    return result;
}

static TrimStats TrimBackgroundWorkingSets(const ModSettings& settings,
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
    for (const auto& entry : processList) {
        alivePids.insert(entry.pid);
    }

    std::unordered_map<DWORD, std::vector<DWORD>> childrenOf;
    std::unordered_map<DWORD, DWORD> parentOf;
    std::unordered_map<DWORD, const ProcessSnapshotEntry*> byPid;
    for (const auto& entry : processList) {
        childrenOf[entry.parentPid].push_back(entry.pid);
        parentOf[entry.pid] = entry.parentPid;
        byPid[entry.pid] = &entry;
    }

    std::unordered_map<DWORD, WindowState> windowStates = BuildWindowStateMapCached();

    // Expand audio shielding to the whole process tree & executable family
    std::unordered_set<DWORD> activeAudioPids =
        ExpandAudioProcessShield(rawAudioPids, processList, childrenOf, parentOf, windowStates);

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

    DWORD gameSanctuaryPid = GetLiveGameSanctuaryPid();
    auto sanctuaryRoot = byPid.find(gameSanctuaryPid);
    if (sanctuaryRoot != byPid.end()) {
        std::vector<DWORD> sanctuaryDescendants;
        std::unordered_set<DWORD> visited;
        CollectDescendants(gameSanctuaryPid, childrenOf, sanctuaryDescendants, visited);
        handledPids.insert(gameSanctuaryPid);
        for (DWORD sanctuaryChild : sanctuaryDescendants) {
            handledPids.insert(sanctuaryChild);
        }
    }

    SystemActionArbiter arbiter{settings, currentPid, foregroundPid,
                                activeAudioPids, handledPids, now};

    auto tryTrimAndRecord = [&](DWORD pid, const std::wstring& name) {
        TrimAttemptResult attempt =
            TryTrimProcess(pid, arbiter, &windowStates, emergency, forceHardTrim);
        if (attempt.trimmed) {
            stats.processesTrimmed++;
            if (attempt.isSoftTrim) {
                stats.processesSoftTrimmed++;
                stats.bytesStandbyDemoted += attempt.standbyDemotedBytes;
            } else {
                stats.processesHardTrimmed++;
                stats.bytesReclaimed += attempt.freedBytes;
            }
            AppTrimEntry appEntry;
            appEntry.procName = name;
            appEntry.pid = pid;
            appEntry.isSoftTrim = attempt.isSoftTrim;
            appEntry.bytesFreed = attempt.freedBytes;
            appEntry.standbyDemotedBytes = attempt.standbyDemotedBytes;
            appEntry.beforeBytes = attempt.beforeBytes;
            appEntry.afterBytes = attempt.afterBytes;
            trimmedEntries.push_back(std::move(appEntry));
        }
    };

    auto shouldTrimProcess = [&](DWORD p, const std::wstring& n) -> bool {
        if (handledPids.count(p))
            return false;

        auto preDec = arbiter.EvaluatePreOpen(p, n, ActionType::TrimWorkingSet,
                                              &windowStates, graceSeconds);
        bool isTargetListed = IsInList(n, settings.customTargetList);
        bool capEligible = false;

        if (!preDec.isAllowed) {
            // Check if process exceeded Electron memory cap, which can override UserRecentFocus grace period
            if (preDec.primaryVeto == VetoReason::UserRecentFocus &&
                settings.enableElectronMemoryCap && isTargetListed &&
                p != foregroundPid) {
                if (IsAccessDeniedImmune(p)) {
                    return false;
                }
                HANDLE hPeek =
                    OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, p);
                if (hPeek) {
                    PROCESS_MEMORY_COUNTERS_EX pmc{};
                    pmc.cb = sizeof(pmc);
                    if (GetProcessMemoryInfo(
                            hPeek, reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc),
                            sizeof(pmc))) {
                        SIZE_T wsMb = pmc.WorkingSetSize / (1024ULL * 1024ULL);
                        if (wsMb >= static_cast<SIZE_T>(settings.electronMemoryCapMb)) {
                            capEligible = true;
                        }
                    }
                    CloseHandle(hPeek);
                } else if (GetLastError() == ERROR_ACCESS_DENIED) {
                    RecordAccessDeniedImmunity(p);
                }
            }

            if (preDec.primaryVeto == VetoReason::UserRecentFocus && !capEligible) {
                stats.processesSkippedRecent++;
                return false;
            } else if (!capEligible) {
                return false;
            }
        }

        if (hogsOnly) {
            // Tier 1 (moderate RAM pressure): trim only targeted, minimized, or deeply inactive processes
            if (!isTargetListed && !capEligible) {
                return false;
            }
            auto wsIt = windowStates.find(p);
            bool isMinimized =
                (wsIt != windowStates.end() && wsIt->second.isMinimized);
            if (!isMinimized) {
                return false;
            }
            if (settings.enableProcessAging) {
                std::lock_guard<std::mutex> lock(g_focusMapMutex);
                auto itFocus = g_processLastFocusedTime.find(p);
                if (itFocus != g_processLastFocusedTime.end()) {
                    auto inactiveSec =
                        std::chrono::duration_cast<std::chrono::seconds>(
                            now - itFocus->second)
                            .count();
                    if (inactiveSec < 15 * 60) {
                        return false;
                    }
                } else {
                    // Initialize timestamp to allow the 15-minute grace period to run
                    g_processLastFocusedTime[p] = now;
                    return false;
                }
            }
        }

        return true;
    };

    for (const auto& entry : processList) {
        DWORD pid = entry.pid;
        const std::wstring& procName = entry.name;
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
                const std::wstring& childName = childIt->second->name;
                if (!shouldTrimProcess(childPid, childName))
                    continue;

                handledPids.insert(childPid);
                tryTrimAndRecord(childPid, childName);
            }
        }
    }

    // Dedicated Smart Browser Inactive Tab & Renderer Trim Pass
    if (settings.enableBrowserTabTrim) {
        int64_t tabInactivitySeconds =
            static_cast<int64_t>(settings.browserTabInactivityMinutes) * 60;
        for (const auto& entry : processList) {
            DWORD pid = entry.pid;
            if (handledPids.count(pid) || pid == foregroundPid) {
                continue;
            }
            if (!IsBrowserProcessName(entry.name)) {
                continue;
            }

            // Never trim processes actively playing audio
            if (activeAudioPids.count(pid)) {
                continue;
            }

            // Exclude main browser UI windows
            auto wsIt = windowStates.find(pid);
            if (wsIt != windowStates.end() && wsIt->second.hasVisibleWindow) {
                continue;
            }

            bool isDormant = false;
            {
                std::lock_guard<std::mutex> lock(g_focusMapMutex);
                auto itFocus = g_processLastFocusedTime.find(pid);
                if (itFocus != g_processLastFocusedTime.end()) {
                    auto inactiveSec =
                        std::chrono::duration_cast<std::chrono::seconds>(
                            now - itFocus->second)
                            .count();
                    if (inactiveSec >= tabInactivitySeconds) {
                        isDormant = true;
                    }
                } else {
                    g_processLastFocusedTime[pid] = now;
                }
            }

            if (isDormant) {
                handledPids.insert(pid);
                tryTrimAndRecord(pid, entry.name);
            }
        }
    }

    std::sort(trimmedEntries.begin(), trimmedEntries.end(),
              [](const AppTrimEntry& a, const AppTrimEntry& b) {
                  return (a.bytesFreed + a.standbyDemotedBytes) >
                         (b.bytesFreed + b.standbyDemotedBytes);
              });

    size_t count =
        (std::min)(static_cast<size_t>(kLogTopAppsCount), trimmedEntries.size());
    stats.topApps.assign(trimmedEntries.begin(), trimmedEntries.begin() + count);

    {
        std::lock_guard<std::mutex> lock(g_focusMapMutex);
        PruneDeadPids(g_processLastFocusedTime, alivePids);
    }
    PruneDeadPids(g_processLastTrimmed, alivePids);
    PruneAccessDeniedImmunity(alivePids);
    PruneClassifyCache(alivePids);

    return stats;
}

// ---------------------------------------------------------------------------
// Cleanup Pass Dispatcher
// ---------------------------------------------------------------------------

static void PerformMemoryCleanup(const wchar_t* triggerReason,
                                 bool allowWorkingSetTrim = true,
                                 bool hogsOnly = false,
                                 bool forceHardTrim = false) {
    ModSettings settings = GetSettingsSnapshot();

    // Suppress all memory purges during active gaming sessions except explicit panic triggers
    if (IsGamingLockdownActive(GetForegroundProcessId(),
                               std::chrono::steady_clock::now(),
                               settings.gameAltTabGracePeriodSeconds)) {
        if (wcscmp(triggerReason, L"[Panic Hotkey Trigger]") != 0 &&
            wcscmp(triggerReason, L"[Pre-Game Sweep]") != 0) {
            return;
        }
    }

    DWORD accessDenied = g_accessDeniedCount.exchange(0);
    if (accessDenied > 0) {
        LogEvent(LogCategory::Sanctuary, LogDetailLevel::Detailed,
                 L"Notice: %u elevated/system processes skipped (access denied; mod runs in user session).",
                 accessDenied);
    }

    MEMORYSTATUSEX memBefore;
    memBefore.dwLength = sizeof(memBefore);
    if (!GlobalMemoryStatusEx(&memBefore) || memBefore.ullTotalPhys == 0) {
        return;
    }

    double freeRamPercent =
        (static_cast<double>(memBefore.ullAvailPhys) / memBefore.ullTotalPhys) *
        100.0;
    DWORD fgPid = GetForegroundProcessId();

    // Below 10% free RAM, relax the anti-thrashing cooldowns so the mod can
    // react before the system runs out of memory.
    bool emergency = freeRamPercent <= 10.0;

    TrimStats trimStats;
    if (settings.cleanBackgroundWorkingSets && allowWorkingSetTrim) {
        trimStats = TrimBackgroundWorkingSets(settings, fgPid, freeRamPercent,
                                              emergency, hogsOnly, forceHardTrim);
    }

    MEMORYSTATUSEX memAfter;
    memAfter.dwLength = sizeof(memAfter);
    if (!GlobalMemoryStatusEx(&memAfter)) {
        return;
    }

    // Record available physical RAM for post-clean growth hysteresis checks
    g_lastCleanAvailPhys.store(memAfter.ullAvailPhys, std::memory_order_relaxed);

    DWORDLONG freedTotalBytes =
        (memAfter.ullAvailPhys > memBefore.ullAvailPhys)
            ? (memAfter.ullAvailPhys - memBefore.ullAvailPhys)
            : trimStats.bytesReclaimed;

    g_sessionCleanupPasses.fetch_add(1, std::memory_order_relaxed);
    g_sessionBytesReclaimed.fetch_add(static_cast<ULONGLONG>(freedTotalBytes), std::memory_order_relaxed);
    g_sessionBytesStandbyDemoted.fetch_add(static_cast<ULONGLONG>(trimStats.bytesStandbyDemoted), std::memory_order_relaxed);
    g_sessionProcessesTrimmedTotal.fetch_add(trimStats.processesTrimmed, std::memory_order_relaxed);

    double freedMb = freedTotalBytes / (1024.0 * 1024.0);
    double standbyMb = trimStats.bytesStandbyDemoted / (1024.0 * 1024.0);
    double availAfterGb = memAfter.ullAvailPhys / (1024.0 * 1024.0 * 1024.0);
    double totalGb = memBefore.ullTotalPhys / (1024.0 * 1024.0 * 1024.0);
    double sessionGb = g_sessionBytesReclaimed.load(std::memory_order_relaxed) / (1024.0 * 1024.0 * 1024.0);
    double sessionStandbyGb = g_sessionBytesStandbyDemoted.load(std::memory_order_relaxed) / (1024.0 * 1024.0 * 1024.0);
    std::wstring uptime =
        FormatUptime(g_modStartTime, std::chrono::steady_clock::now());

    wchar_t detailStr[256];
    if (trimStats.processesSoftTrimmed > 0 && trimStats.processesHardTrimmed > 0) {
        swprintf_s(detailStr, _countof(detailStr),
                   L"+%.1f MB freed (hard), +%.1f MB marked for standby (soft)",
                   freedMb, standbyMb);
    } else if (trimStats.processesSoftTrimmed > 0) {
        swprintf_s(detailStr, _countof(detailStr),
                   L"+%.1f MB marked for standby", standbyMb);
    } else {
        swprintf_s(detailStr, _countof(detailStr),
                   L"+%.1f MB freed", freedMb);
    }

    LogEvent(LogCategory::Memory, LogDetailLevel::Minimal,
             L"%s -> %s (Avail: %.2f/%.1f GB | %u trimmed [%u soft, %u hard], %u kept warm) | Session: %.2f GB freed, %.2f GB standby over %u passes (%u procs), %s uptime",
             triggerReason, detailStr, availAfterGb, totalGb,
             trimStats.processesTrimmed, trimStats.processesSoftTrimmed, trimStats.processesHardTrimmed,
             trimStats.processesSkippedRecent,
             sessionGb, sessionStandbyGb,
             g_sessionCleanupPasses.load(std::memory_order_relaxed),
             g_sessionProcessesTrimmedTotal.load(std::memory_order_relaxed),
             uptime.c_str());

    bool gainWorthLogging =
        (freedTotalBytes + trimStats.bytesStandbyDemoted) >=
        static_cast<DWORDLONG>(kTopAppsLogThresholdMb) * 1024 * 1024;
    if (!trimStats.topApps.empty() && gainWorthLogging) {
        LogEvent(LogCategory::Memory, LogDetailLevel::Detailed, L"--- Top Reclaimed Memory Hogs ---");
        int rank = 1;
        for (const auto& app : trimStats.topApps) {
            double beforeMb = app.beforeBytes / (1024.0 * 1024.0);
            double afterMb = app.afterBytes / (1024.0 * 1024.0);
            if (app.isSoftTrim) {
                double demotedMb = app.standbyDemotedBytes / (1024.0 * 1024.0);
                LogEvent(LogCategory::Memory, LogDetailLevel::Detailed,
                         L"  #%d. %s (PID %u): ~%.1f MB marked for standby (working set: %.1f MB)",
                         rank++, app.procName.c_str(), app.pid, demotedMb, beforeMb);
            } else {
                double appFreedMb = app.bytesFreed / (1024.0 * 1024.0);
                LogEvent(LogCategory::Memory, LogDetailLevel::Detailed,
                         L"  #%d. %s (PID %u): -%.1f MB (was %.1f MB -> now %.1f MB)",
                         rank++, app.procName.c_str(), app.pid, appFreedMb, beforeMb, afterMb);
            }
        }
    }

    // Emergency Panic Hotkey completion feedback & session report:
    if (wcscmp(triggerReason, L"[Panic Hotkey Trigger]") == 0) {
        std::wstring report = BuildSessionStatsString();
        LogEvent(LogCategory::Memory, LogDetailLevel::Minimal, L"%s", report.c_str());
        Wh_Log(L"[SmartOptimizer] %s", report.c_str());
        if (settings.enableHotkeySound) {
            std::thread([]() { MessageBeep(MB_OK); }).detach();
        }
    }
}

// ---------------------------------------------------------------------------
// Event Hook Thread (Instant Foreground Detection + Panic Hotkey)
// ---------------------------------------------------------------------------

static void HandleForegroundChanged(HWND hwnd) {
    DWORD pid = GetWindowProcessIdResolved(hwnd);
    if (pid == 0 || pid == 4 || pid == GetCurrentProcessId())
        return;

    if (pid != g_currentBoostedPid.load(std::memory_order_relaxed)) {
        // Fast-path: immediately boost active foreground window without allocations or I/O
        if (g_fastProBalanceEnabled.load(std::memory_order_relaxed)) {
            HANDLE hFastProc = OpenProcess(
                PROCESS_SET_INFORMATION | PROCESS_QUERY_LIMITED_INFORMATION,
                FALSE, pid);
            if (hFastProc) {
                DWORD targetPrio =
                    g_fastForegroundPriority.load(std::memory_order_relaxed);
                SetPriorityClass(hFastProc, targetPrio);
                CloseHandle(hFastProc);
            }
            g_fastBoostedPid.store(pid, std::memory_order_release);
        }

        RecordFocusSwitch(pid);
        g_lastFocusChangeTick.store(GetTickCount64(), std::memory_order_release);

        ModSettings settings = GetSettingsSnapshot();
        MaybeRequestGameSweep(pid, hwnd, settings);

        // Wake worker for deferred background adjustment and tree discovery
        if (g_wakeEvent) {
            SetEvent(g_wakeEvent);
        }
    }
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
    if (uMsg == WM_QUERYENDSESSION || uMsg == WM_ENDSESSION) {
        RestoreOriginalPowerScheme();
        return TRUE;
    }
    if (uMsg == WM_POWERBROADCAST) {
        if (wParam == PBT_APMSUSPEND) {
            LogEvent(LogCategory::Power, LogDetailLevel::Minimal,
                     L"System entering sleep/suspend. Pausing engine and restoring priorities...");
            g_systemSuspended.store(true);
            {
                std::lock_guard<std::mutex> lock(g_priorityMutex);
                RestoreForegroundBoostLocked();
            }
            RestoreAllThrottledProcesses();
        } else if (wParam == PBT_APMRESUMEAUTOMATIC ||
                   wParam == PBT_APMRESUMESUSPEND) {
            g_lastResumeTick.store(GetTickCount64(), std::memory_order_release);
            g_resetSamplesRequested.store(true, std::memory_order_release);
            LogEvent(LogCategory::Power, LogDetailLevel::Minimal,
                     L"System resumed from sleep/suspend. Resetting aging timers, clearing stale CPU/IO metrics, and engaging 30s warm-up grace period...");
            auto now = std::chrono::steady_clock::now();
            {
                std::lock_guard<std::mutex> lock(g_focusMapMutex);
                for (auto& entry : g_processLastFocusedTime) {
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
            std::wstring report = BuildSessionStatsString();
            Wh_Log(L"[SmartOptimizer] %s", report.c_str());
            LogEvent(LogCategory::Memory, LogDetailLevel::Minimal,
                     L"Panic hotkey triggered (Ctrl+Alt+F11). Requesting immediate working set sweep...");
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
    g_lastTriggerCleanTime = g_modStartTime;

    bool isWarmupPass = true;

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

        if (g_resetSamplesRequested.exchange(false, std::memory_order_acq_rel)) {
            g_cpuSamples.clear();
            g_ioSamples.clear();
            g_audioWorkerSamples.clear();
            g_audioWorkerActiveUntil.clear();
            g_loggedAudioWorkers.clear();
            g_aiCpuSamples.clear();
        }

        // Slow-path debounce: wait for focus stabilization before running tree discovery and CPU sampling
        ULONGLONG lastFocusTick =
            g_lastFocusChangeTick.load(std::memory_order_acquire);
        if (lastFocusTick != 0) {
            ULONGLONG nowTick = GetTickCount64();
            if (nowTick >= lastFocusTick) {
                ULONGLONG elapsedMs = nowTick - lastFocusTick;
                if (elapsedMs < 1500) {
                    DWORD waitMs = static_cast<DWORD>(1500 - elapsedMs);
                    DWORD waitRes = WaitForSingleObject(g_stopEvent, waitMs);
                    if (waitRes == WAIT_OBJECT_0) {
                        break;
                    }
                }
            }
        }

        DWORD fgPid = GetForegroundProcessId();
        if (fgPid != 0) {
            UpdateForegroundBoost(fgPid, settings);
        }

        bool recentlyResumed = false;
        ULONGLONG resumeTick = g_lastResumeTick.load(std::memory_order_acquire);
        if (resumeTick != 0) {
            ULONGLONG nowTick = GetTickCount64();
            if (nowTick >= resumeTick && (nowTick - resumeTick) < 30000) {
                recentlyResumed = true;
            }
        }

        static bool s_lastOnBatteryState = false;
        if (onBattery != s_lastOnBatteryState) {
            s_lastOnBatteryState = onBattery;
            if (onBattery) {
                LogEvent(LogCategory::Power, LogDetailLevel::Minimal,
                         L"Battery power or Energy Saver active. Background throttling and memory cleanups paused.");
            } else {
                LogEvent(LogCategory::Power, LogDetailLevel::Minimal,
                         L"AC power restored. Resuming background optimization and throttling engine.");
            }
        }

        if (!onBattery && !recentlyResumed) {
            UpdateAiProcessActivity(settings);
            ApplyBackgroundThrottling(settings, GetForegroundProcessId());
        } else if (onBattery || recentlyResumed) {
            RestoreAllThrottledProcesses();
        }

        bool inGamingLockdown =
            IsGamingLockdownActive(fgPid, std::chrono::steady_clock::now(),
                                   settings.gameAltTabGracePeriodSeconds);
        static bool s_lastGamingLockdownState = false;
        if (inGamingLockdown != s_lastGamingLockdownState) {
            s_lastGamingLockdownState = inGamingLockdown;
            if (inGamingLockdown) {
                LogEvent(LogCategory::Sanctuary, LogDetailLevel::Minimal,
                         L"Gaming Lockdown active: RAM calculation and trimming cycles suspended.");
            } else {
                LogEvent(LogCategory::Sanctuary, LogDetailLevel::Minimal,
                         L"Gaming Lockdown released: resuming normal memory management.");
            }
        }

        if (!onBattery && !recentlyResumed && !inGamingLockdown) {
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

                // Hysteresis: ensure consumed RAM has meaningfully increased since last cleanup
                bool ramGrowthConditionMet = true;
                DWORDLONG lastAvail = g_lastCleanAvailPhys.load(std::memory_order_relaxed);
                if (lastAvail != 0) {
                    if (mem.ullAvailPhys + kMinRamGrowthForReevaluationBytes > lastAvail) {
                        ramGrowthConditionMet = false;
                    }
                }

                SystemHardwareProfile hw = GetHardwareProfile();
                double effectiveFreeRamThreshold = GetEffectiveRamThreshold(
                    settings.freeRamThresholdPercent, hw.totalRamGb, /*isTieredHogs=*/false);
                double effectiveTieredThreshold = GetEffectiveRamThreshold(
                    settings.freeRamThresholdPercent, hw.totalRamGb, /*isTieredHogs=*/true);

                // Smart threshold trigger
                bool hogsOnly = false;
                if (!isWarmupPass && (settings.cleanMode == CleanMode::SmartThreshold ||
                                      settings.cleanMode == CleanMode::SmartAndPeriodic)) {
                    if (freePercent <= effectiveFreeRamThreshold &&
                        triggerCooldownElapsed && ramGrowthConditionMet) {
                        shouldClean = true;
                        hogsOnly = false;
                        wchar_t buf[160];
                        swprintf_s(buf, L"[%s: Free RAM %.1f%% <= %.1f%% (adapted for %.1f GB)]",
                                   (settings.freeRamThresholdPercent <= 0) ? L"Auto Standard Threshold" : L"Custom Standard Threshold",
                                   freePercent, effectiveFreeRamThreshold, hw.totalRamGb);
                        reason = buf;
                        g_lastTriggerCleanTime = now;
                    } else if (settings.enableTieredRamThreshold &&
                               freePercent <= effectiveTieredThreshold &&
                               triggerCooldownElapsed && ramGrowthConditionMet) {
                        shouldClean = true;
                        hogsOnly = true;
                        wchar_t buf[160];
                        swprintf_s(
                            buf,
                            L"[%s: Free RAM %.1f%% <= %.1f%% (adapted for %.1f GB, Inactive Hogs Only)]",
                            (settings.freeRamThresholdPercent <= 0) ? L"Auto Tiered Threshold" : L"Custom Tiered Threshold",
                            freePercent, effectiveTieredThreshold, hw.totalRamGb);
                        reason = buf;
                        g_lastTriggerCleanTime = now;
                    }
                }

                // 2. Periodic timer trigger (gated on memory pressure).
                if (!shouldClean && !isWarmupPass &&
                    (settings.cleanMode == CleanMode::Periodic ||
                     settings.cleanMode == CleanMode::SmartAndPeriodic)) {
                    auto elapsedMinutes =
                        std::chrono::duration_cast<std::chrono::minutes>(
                            now - g_lastPeriodicCleanTime)
                            .count();
                    if (elapsedMinutes >= settings.periodicIntervalMinutes) {
                        if (freePercent <= effectiveFreeRamThreshold) {
                            shouldClean = true;
                            hogsOnly = false;
                            wchar_t buf[160];
                            swprintf_s(buf,
                                       L"[Periodic Trigger: %d min interval (Free RAM %.1f%% "
                                       L"<= %.1f%%)]",
                                       settings.periodicIntervalMinutes, freePercent,
                                       effectiveFreeRamThreshold);
                            reason = buf;
                        }
                        g_lastPeriodicCleanTime = now;
                    }
                }

                // Idle trigger: sweeps on entering idle state with minimum cooldown and memory pressure check
                DWORD idleSec = GetSystemIdleSeconds();
                DWORD idleThresholdSec =
                    static_cast<DWORD>(settings.idleThresholdMinutes) * 60;
                bool isSystemIdle = (idleSec >= idleThresholdSec);

                if (!shouldClean && !isWarmupPass && settings.enableIdleBoost && isSystemIdle &&
                    triggerCooldownElapsed) {
                    auto elapsedSinceIdleClean =
                        std::chrono::duration_cast<std::chrono::minutes>(
                            now - g_lastIdleCleanTime)
                            .count();
                    int idleIntervalMin =
                        (std::max)(10, settings.periodicIntervalMinutes);
                    if (!g_wasIdle || elapsedSinceIdleClean >= idleIntervalMin) {
                        if (freePercent <= effectiveFreeRamThreshold) {
                            shouldClean = true;
                            hogsOnly = false;
                            wchar_t buf[160];
                            swprintf_s(buf,
                                       L"[Idle Trigger: idle for %u min (Free RAM %.1f%% <= "
                                       L"%.1f%%)]",
                                       idleSec / 60, freePercent,
                                       effectiveFreeRamThreshold);
                            reason = buf;
                            g_lastIdleCleanTime = now;
                            g_lastTriggerCleanTime = now;
                        }
                    }
                }

                // 4. Critical emergency safety: if free RAM drops to <= 10.0%,
                // intervene immediately even during the first cycle.
                if (!shouldClean && freePercent <= 10.0 && triggerCooldownElapsed) {
                    shouldClean = true;
                    hogsOnly = false;
                    wchar_t buf[128];
                    swprintf_s(buf, L"[Critical Emergency: Free RAM %.1f%% <= 10.0%%]",
                               freePercent);
                    reason = buf;
                    g_lastTriggerCleanTime = now;
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

        isWarmupPass = false;

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

    // Synchronize lock-free atomics for hook thread fast-path
    g_fastProBalanceEnabled.store(g_settings.enableProBalance,
                                  std::memory_order_release);
    g_fastForegroundPriority.store(
        (g_settings.foregroundPriorityLevel == ForegroundPrioritySetting::High)
            ? HIGH_PRIORITY_CLASS
            : ABOVE_NORMAL_PRIORITY_CLASS,
        std::memory_order_release);

    g_settings.enableForegroundCpuSets =
        Wh_GetIntSetting(L"enableForegroundCpuSets") != 0;

    g_settings.enableBackgroundCpuSets =
        Wh_GetIntSetting(L"enableBackgroundCpuSets") != 0;

    g_settings.enableBackgroundThrottling =
        Wh_GetIntSetting(L"enableBackgroundThrottling") != 0;

    g_settings.enableThreadBackgroundMode =
        Wh_GetIntSetting(L"enableThreadBackgroundMode") != 0;

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

    g_settings.enableAudioShielding =
        Wh_GetIntSetting(L"enableAudioShielding") != 0;

    g_settings.enableNetworkShielding =
        Wh_GetIntSetting(L"enableNetworkShielding") != 0;

    // Section 2: Smart Memory Management
    int thresh = (int)Wh_GetIntSetting(L"freeRamThresholdPercent");
    g_settings.freeRamThresholdPercent = std::clamp(thresh, 0, 80);

    g_settings.trimMinimizedWindows =
        Wh_GetIntSetting(L"trimMinimizedWindows") != 0;

    g_settings.enableBrowserTabTrim =
        Wh_GetIntSetting(L"enableBrowserTabTrim") != 0;

    g_settings.pauseOnBattery = Wh_GetIntSetting(L"pauseOnBattery") != 0;

    g_settings.enableElectronMemoryCap =
        Wh_GetIntSetting(L"enableElectronMemoryCap") != 0;

    int capMb = (int)Wh_GetIntSetting(L"electronMemoryCapMb");
    g_settings.electronMemoryCapMb = std::clamp(capMb, 100, 4000);

    // Section 3: Hardware & Workload Protection
    g_settings.enableSmartAiOptimization =
        Wh_GetIntSetting(L"enableSmartAiOptimization") != 0;

    g_settings.enableDynamicPowerPlan =
        Wh_GetIntSetting(L"enableDynamicPowerPlan") != 0;
    if (!g_settings.enableDynamicPowerPlan) {
        RestoreOriginalPowerScheme();
    }

    // Section 4: Process Lists & Hotkeys
    auto customListStr = WindhawkUtils::StringSetting::make(L"customTargetList");
    g_settings.customTargetList =
        ParseProcessList(customListStr.get() ? customListStr.get() : L"");

    auto customAiStr = WindhawkUtils::StringSetting::make(L"customAiProcesses");
    g_settings.customAiProcesses =
        ParseProcessList(customAiStr.get() ? customAiStr.get() : L"");

    auto exclListStr = WindhawkUtils::StringSetting::make(L"excludedProcesses");
    g_settings.excludedProcesses =
        ParseProcessList(exclListStr.get() ? exclListStr.get() : L"");

    g_settings.enablePanicHotkey = Wh_GetIntSetting(L"enablePanicHotkey") != 0;
    g_settings.enableHotkeySound = Wh_GetIntSetting(L"enableHotkeySound") != 0;
    g_settings.logDetailLevel = LogDetailLevel::Detailed;

    // Architectural defaults and internal safety heuristics:
    g_settings.enableTieredRamThreshold = true;
    g_settings.tieredHogThresholdPercent = 40;
    g_settings.recentActivityGraceMinutes = 3;
    g_settings.enableIdleBoost = true;
    g_settings.aiInactivityGraceMinutes = 5;
    g_settings.browserTabInactivityMinutes = 10;
    g_settings.gameAltTabGracePeriodSeconds = 180;
    g_settings.ioActivityWriteThresholdKbps = 250;
    g_settings.ioActivityTransferThresholdKbps = 100;
    g_settings.compressionBurstHysteresisSeconds = 20;
    g_settings.enableMultitaskingAdaptation = true;
    g_settings.enableGameModeDetection = true;
    g_settings.enableProcessTreeTrimming = true;
    g_settings.cleanBackgroundWorkingSets = true;
    g_settings.minProcessMemoryToTrimMb = 50;
    g_settings.periodicIntervalMinutes = 10;
    g_settings.targetProcessesOnly = false;

    // Hardware-Aware Auto-Tuning:
    // Automatically adapt thresholds to the machine's physical hardware capacity.
    SystemHardwareProfile hw = GetHardwareProfile();
    if (hw.isLowRamTier) {
        if (g_settings.electronMemoryCapMb > 250) {
            g_settings.electronMemoryCapMb = 250;
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
    LogEvent(
        LogCategory::Boot, LogDetailLevel::Minimal,
        L"Initializing Smart Process Priority & RAM Optimizer (Dedicated Tool Process)...");

    // Dynamically initialize Per-Monitor V2 DPI awareness for physical pixel evaluation
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (hUser32) {
        using pfnSetProcessDpiAwarenessContext_t =
            BOOL(WINAPI*)(DPI_AWARENESS_CONTEXT);
        auto pfnSetDpiContext = (pfnSetProcessDpiAwarenessContext_t)GetProcAddress(
            hUser32, "SetProcessDpiAwarenessContext");
        if (pfnSetDpiContext) {
            pfnSetDpiContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        } else {
            using pfnSetProcessDPIAware_t = BOOL(WINAPI*)();
            auto pfnSetDpiAware =
                (pfnSetProcessDPIAware_t)GetProcAddress(hUser32, "SetProcessDPIAware");
            if (pfnSetDpiAware) {
                pfnSetDpiAware();
            }
        }
    }

    SystemHardwareProfile hw = GetHardwareProfile();
    LogEvent(LogCategory::Boot, LogDetailLevel::Minimal,
             L"Hardware Profile: %.1f GB RAM (%s), %u CPU cores (%s)%s | HAGS: %s | VBS: %s | Memory Compression: %s.",
             hw.totalRamGb,
             hw.isLowRamTier ? L"Low-RAM Tier / iGPU Buffer Elevated"
                             : (hw.isHighRamTier ? L"High-RAM Tier" : L"Standard RAM Tier"),
             hw.coreCount,
             hw.isLowCoreCount ? L"Aggressive Contention Guard" : L"Standard Contention Guard",
             hw.isHybridCpu ? L", Intel/AMD Hybrid P/E-Cores Active" : L"",
             hw.isHagsEnabled ? L"Enabled" : L"Disabled",
             hw.isVbsEnabled ? L"Enabled" : L"Disabled",
             hw.isMemoryCompressionActive ? L"Active" : L"Inactive");

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

    g_hPowrProf = LoadLibraryW(L"powrprof.dll");
    if (g_hPowrProf) {
        g_pfnPowerGetActiveScheme = (pfnPowerGetActiveScheme)GetProcAddress(
            g_hPowrProf, "PowerGetActiveScheme");
        g_pfnPowerSetActiveScheme = (pfnPowerSetActiveScheme)GetProcAddress(
            g_hPowrProf, "PowerSetActiveScheme");
        CheckAndRecoverCrashedPowerScheme();
    }

    LoadSettings();

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_wakeEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_hookThreadReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_powerWakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);

    if (!g_stopEvent || !g_wakeEvent || !g_hookThreadReadyEvent || !g_powerWakeEvent) {
        Wh_Log(L"[SmartOptimizer] Fatal Error: Failed to create synchronization "
               L"events.");
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
        if (g_powerWakeEvent) {
            CloseHandle(g_powerWakeEvent);
            g_powerWakeEvent = nullptr;
        }
        if (g_hPowrProf) {
            FreeLibrary(g_hPowrProf);
            g_hPowrProf = nullptr;
        }
        return FALSE;
    }

    g_powerWorkerRunning.store(true, std::memory_order_release);
    g_powerWorkerThread.emplace(PowerSchemeWorkerProc);

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

    LogEvent(LogCategory::Boot, LogDetailLevel::Minimal,
             L"Mod engine initialized successfully.");
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LogEvent(LogCategory::Boot, LogDetailLevel::Minimal,
             L"Settings updated. Reloading configuration...");
    LoadSettings();

    ModSettings settings = GetSettingsSnapshot();

    // If foreground boost was disabled, restore boosted process immediately.
    // Note: Background throttling restoration is handled safely by the worker
    // thread upon waking to avoid cross-thread data races on throttled processes.
    if (!settings.enableProBalance) {
        std::lock_guard<std::mutex> lock(g_priorityMutex);
        RestoreForegroundBoostLocked();
    } else {
        // Reset cached boosted PID so changes to priority level or CPU sets take effect immediately.
        g_currentBoostedPid.store(0, std::memory_order_release);
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

void WhTool_ModUninit() {
    LogEvent(LogCategory::Shutdown, LogDetailLevel::Minimal,
             L"Deinitializing mod engine...");

    if (g_hookThreadRunning.load()) {
        g_hookThreadRunning.store(false);
        if (g_hookThreadHandle) {
            if (g_hookThreadId != 0) {
                PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
            }
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
        if (g_workerThread && g_workerThread->joinable()) {
            // 3-second timeout before detach to prevent hanging during unload
            HANDLE hThread = g_workerThread->native_handle();
            DWORD waitRes = WaitForSingleObject(hThread, 3000);
            if (waitRes == WAIT_OBJECT_0) {
                g_workerThread->join();
            } else {
                LogEvent(LogCategory::Shutdown, LogDetailLevel::Minimal,
                         L"Worker thread did not terminate within 3s; detaching to prevent hang.");
                g_workerThread->detach();
            }
        }
        g_workerThread.reset();
    }

    if (g_powerWorkerRunning.load()) {
        g_powerWorkerRunning.store(false, std::memory_order_release);
        if (g_powerWakeEvent) {
            SetEvent(g_powerWakeEvent);
        }
        if (g_powerWorkerThread && g_powerWorkerThread->joinable()) {
            HANDLE hPower = g_powerWorkerThread->native_handle();
            DWORD waitRes = WaitForSingleObject(hPower, 2000);
            if (waitRes == WAIT_OBJECT_0) {
                g_powerWorkerThread->join();
            } else {
                g_powerWorkerThread->detach();
            }
        }
        g_powerWorkerThread.reset();
    }
    if (g_powerWakeEvent) {
        CloseHandle(g_powerWakeEvent);
        g_powerWakeEvent = nullptr;
    }

    RestoreAllThrottledProcesses();
    g_audioPidLastActive.clear();
    g_audioPidsCache.clear();
    g_audioWorkerSamples.clear();
    g_audioWorkerActiveUntil.clear();
    g_loggedAudioWorkers.clear();
    {
        std::lock_guard<std::mutex> lock(g_priorityMutex);
        RestoreForegroundBoostLocked();
        g_processContexts.clear();
    }
    RestoreOriginalPowerScheme();
    {
        std::lock_guard<std::mutex> lock(g_immunitySetMutex);
        g_accessDeniedImmunitySet.clear();
    }
    {
        std::lock_guard<std::mutex> lock(g_classifyCacheMutex);
        g_processClassCache.clear();
    }
    ClearGameSanctuary();

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
    if (g_hPowrProf) {
        FreeLibrary(g_hPowrProf);
        g_hPowrProf = nullptr;
    }

    {
        double sessionGb = g_sessionBytesReclaimed.load(std::memory_order_relaxed) / (1024.0 * 1024.0 * 1024.0);
        double sessionStandbyGb = g_sessionBytesStandbyDemoted.load(std::memory_order_relaxed) / (1024.0 * 1024.0 * 1024.0);
        std::wstring uptime =
            FormatUptime(g_modStartTime, std::chrono::steady_clock::now());
        size_t immuneCount = 0;
        {
            std::lock_guard<std::mutex> lock(g_immunitySetMutex);
            immuneCount = g_accessDeniedImmunitySet.size();
        }
        Wh_Log(L"[SmartOptimizer::Shutdown] ========================================");
        Wh_Log(L"[SmartOptimizer::Shutdown] Smart Process Priority & RAM Optimizer");
        Wh_Log(L"[SmartOptimizer::Shutdown] Session Summary Report:");
        Wh_Log(L"[SmartOptimizer::Shutdown]   • Total Uptime: %s", uptime.c_str());
        Wh_Log(L"[SmartOptimizer::Shutdown]   • Total RAM Reclaimed: %.2f GB freed, %.2f GB standby across %u passes (%u process trimmings)",
               sessionGb, sessionStandbyGb,
               g_sessionCleanupPasses.load(std::memory_order_relaxed),
               g_sessionProcessesTrimmedTotal.load(std::memory_order_relaxed));
        Wh_Log(L"[SmartOptimizer::Shutdown]   • ProBalance Boost Transitions: %u",
               g_sessionBoostTransitions.load());
        Wh_Log(L"[SmartOptimizer::Shutdown]   • Background Throttle Transitions: %u",
               g_sessionThrottleTransitions.load());
        Wh_Log(L"[SmartOptimizer::Shutdown]   • Dynamic Anti-Cheat Immune Processes: %zu",
               immuneCount);
        Wh_Log(L"[SmartOptimizer::Shutdown] ========================================");
    }

    Wh_Log(L"[SmartOptimizer] Mod unloaded cleanly.");
}

// ---------------------------------------------------------------------------
// Tool Mod Process Bootstrap
// ---------------------------------------------------------------------------
// Dedicated tool-mod bootstrap: re-launches windhawk.exe with -tool-mod and drives lifecycle

bool g_isToolModProcessLauncher = false;
HANDLE g_toolModProcessMutex = nullptr;

void WINAPI EntryPoint_Hook() {
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

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
            LocalFree(reinterpret_cast<HLOCAL>(argv));
            return FALSE;
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

    LocalFree(reinterpret_cast<HLOCAL>(argv));

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

        IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
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
    DWORD modLen = GetModuleFileName(nullptr, currentProcessPath,
                                     ARRAYSIZE(currentProcessPath));
    if (modLen == 0 || modLen == ARRAYSIZE(currentProcessPath)) {
        Wh_Log(L"GetModuleFileName failed");
        return;
    }

    WCHAR commandLine[MAX_PATH + 64];
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
