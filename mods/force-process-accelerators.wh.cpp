// ==WindhawkMod==
// @id              force-process-accelerators
// @name            Force Process CPU/GPU Preferences
// @description     Applies per-process CPU/memory topology and scheduling, Windows GPU preference, conditional co-running-process rules, and best-effort ONNX Runtime NPU modes. Arbitrary NPU forcing isn't available.
// @version         1.8.0
// @author          m417z
// @github          https://github.com/m417z
// @include         *
// @compilerOptions -ladvapi32 -lshlwapi -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Force Process CPU/GPU Preferences

This mod applies hardware-related process preferences on a per-process basis:

- CPU affinity
- Topology-aware CPU Set placement
- CPU priority class
- Dynamic CPU priority boosting
- CPU execution-speed throttling / EcoQoS / full Eco QoS
- System timer resolution control
- Process working set management
- Efficiency-class CPU override for hybrid CPUs
- Composite low-latency profile
- Process I/O priority and page priority
- Process memory priority
- Windows per-app GPU preference
- Conditional rule matching based on other running processes
- Best-effort ONNX Runtime NPU preference, QNN HTP, and OpenVINO NPU modes
- A running-process picker hosted inside Windhawk

## Important limitation: NPU

Windows doesn't expose a generic "force this arbitrary process onto the NPU"
switch that a Windhawk mod can flip from outside the app. An app has to be
written to use Windows ML / DirectML / ONNX Runtime / a vendor NPU runtime in
the first place. This mod can help with CPU and GPU policy, but it can't make a
random process start using the NPU if the process doesn't already support it.

This mod's NPU option is intentionally narrow: it only targets apps that use
`onnxruntime.dll`, and it only applies ONNX Runtime's own execution-provider
selection policy for new session options.

## Matching

Each rule matches either:

- the executable name, such as `notepad.exe`
- the full path, such as `C:\Apps\Foo\foo.exe`
- a wildcard pattern, such as `*\\foo.exe`
- the special token `@picked`, which resolves to the last process chosen in the
  running-process picker

The first enabled matching rule wins.

Additional optional rule conditions let you vary a process's policy depending on
what else is running:

- `whenProcessesRunning`: only match if one of the listed processes is running
- `whenProcessesNotRunning`: only match if none of the listed processes are
  running

These conditions are evaluated when the target process starts or when Windhawk
reloads the settings.

## Running-process picker

Windhawk settings don't provide a native live process dropdown, so this mod
ships a small picker window that opens inside `windhawk.exe`.

1. Toggle `picker.launch` on.
2. Pick a running process from the list.
3. The selected value is copied to the clipboard and saved as `@picked`.

If you use `match: @picked`, restart the target app or touch a setting after
choosing a new process so the new selection is re-evaluated.

## CPU options

- `cpuCores`: `unchanged`, `all`, or a comma-separated list/range such as
  `0-3,8`
- `cpuSets`: `unchanged`, `all`, `performance`, `efficiency`, or a
  comma-separated CPU Set ID/range such as `0,4-7`
- `cpuSetNumaNode`: optional NUMA-node filter for CPU Sets
- `cpuSetLastLevelCache`: optional last-level-cache filter for CPU Sets
- `cpuSetLimit`: optional cap on how many CPU Sets remain after filtering
- `cpuSetPlacement`: optionally keep the limited set compact for locality or
  spread it across cache/NUMA domains to reduce contention
- `cpuSetAvoidSmt`: optionally keep only one logical CPU Set per physical core
- `cpuPriority`: process priority class
- `dynamicPriorityBoost`: whether Windows may temporarily boost the process for
  interactive bursts
- `cpuPowerMode`: whether Windows execution-speed throttling should be left
  alone, disabled, or enabled. `eco-qos-full` additionally ignores timer
  resolution requests for maximum battery savings.
- `cpuSetEfficiencyPreference`: on hybrid CPUs (Intel 12th-gen+, ARM big.LITTLE),
  forces the process onto performance-only or efficiency-only cores.
- `timerResolution`: lowers the system timer tick to 0.5 ms for the process,
  reducing input latency and improving frame pacing.
- `workingSetMode`: pins the process's pages in RAM (`lock-minimum`) or trims
  idle memory (`trim`).
- `profile`: a composite preset. `low-latency` automatically applies optimal
  latency settings (high priority, disable throttling, max timer resolution,
  high-perf GPU). Explicit per-field settings override the profile defaults.
- `ioPriority`: I/O priority for the process (very-low, low, normal, high, critical)
- `pagePriority`: Page priority for the process
- `memoryPriority`: how aggressively Windows should reclaim this process's
  memory under pressure

## GPU option

`gpuPreference` updates the per-app Windows Graphics Settings entry for the
current executable under:

`HKCU\Software\Microsoft\DirectX\UserGpuPreferences`

If the process already loaded DXGI / D3D modules before the setting was applied,
restart the process to guarantee the new GPU preference is used.

## NPU option

The NPU controls are all best-effort hooks for apps that already use ONNX
Runtime:

- `onnxruntime-prefer-npu`: ONNX Runtime 1.22+ automatic EP selection prefers
  NPU devices for new sessions
- `onnxruntime-qnn`: append the ONNX Runtime QNN EP with the HTP backend for
  new sessions
- `onnxruntime-openvino`: append the ONNX Runtime OpenVINO EP for new sessions,
  defaulting to `device_type=NPU`

Additional NPU options:

- `npuDisableCpuFallback`: disables ONNX Runtime CPU EP fallback for new
  sessions, turning unsupported-node fallback into session-creation failure
- `npuQnnPerformanceMode`: optional Qualcomm HTP performance hint such as
  `balanced`, `burst`, or `high_performance`
- `npuOpenVinoDeviceType`: choose `NPU`, `AUTO:NPU,CPU`, `HETERO:NPU,CPU`, or
  `MULTI:NPU,CPU`
- `npuOpenVinoEnableFastCompile`: enables OpenVINO's `enable_npu_fast_compile`
  hint

Important limits:

- It only affects apps that load `onnxruntime.dll`.
- It only affects new ONNX Runtime sessions created after the hook is active.
- It doesn't override apps that don't use ONNX Runtime.
- It doesn't invent NPU support for models, drivers, or provider stacks that
  aren't already available.
- QNN mode requires an ONNX Runtime build with the QNN EP and the Qualcomm QNN
  user-mode stack.
- OpenVINO mode requires an ONNX Runtime build with the OpenVINO EP and Intel's
  OpenVINO/NPU support.

## Notes

- `realtime` CPU priority is dangerous and can make the system unstable.
- CPU affinity uses logical CPU indexes in the current processor group.
- CPU Sets can span processor groups and are more topology-aware than affinity.
- CPU Set selection skips parked CPUs and CPUs allocated to other target
  processes.
- `cpuSetPlacement: compact` favors locality and shared-cache reuse.
- `cpuSetPlacement: spread` favors cache/bandwidth contention avoidance.
- `cpuSetPlacement` only changes which CPUs survive when `cpuSetLimit` trims a
  larger candidate set.
- If both `cpuCores` and `cpuSets` are configured, Windows effectively uses the
  intersection of both restrictions.
- If you add a rule for a process that is already running and this mod wasn't
  loaded in it yet, restart that process.
- If you change the NPU option, recreate the app's ONNX Runtime sessions or
  restart the app.
- `whenProcessesRunning` and `whenProcessesNotRunning` are only re-evaluated
  when the target process starts or when settings reload.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- rules:
  - - match: example.exe
      $name: Executable name, full path, or wildcard
      $description: Matches the current process name or full path. Wildcards such as * and ? are supported. You can also use @picked.
    - whenProcessesRunning: unchanged
      $name: When other processes are running
      $description: Optional comma-separated list of exe names, full paths, wildcards, or @picked. This rule only matches if at least one of them is running when the target process starts.
    - whenProcessesNotRunning: unchanged
      $name: When other processes are not running
      $description: Optional comma-separated list of exe names, full paths, wildcards, or @picked. This rule only matches if none of them are running when the target process starts.
    - enabled: true
      $name: Enabled
    - cpuCores: all
      $name: CPU cores
      $description: Use unchanged, all, or a comma-separated list/range such as 0-3,8.
    - cpuSets: unchanged
      $name: CPU sets
      $description: Use unchanged, all, performance, efficiency, or a comma-separated CPU Set ID/range such as 0,4-7. Parked CPU Sets are skipped.
    - cpuSetNumaNode: unchanged
      $name: CPU set NUMA node
      $description: Optional filter for CPU set selection. Use unchanged or an integer such as 0.
    - cpuSetLastLevelCache: unchanged
      $name: CPU set cache index
      $description: Optional last-level-cache filter for CPU set selection. Use unchanged or an integer such as 0.
    - cpuSetLimit: unchanged
      $name: CPU set limit
      $description: Optional cap on how many CPU Sets remain after filtering. Use unchanged or an integer such as 4.
    - cpuSetPlacement: unchanged
      $name: CPU set placement
      $description: When CPU set limit trims a larger candidate set, compact favors locality while spread favors contention avoidance.
      $options:
        - unchanged: Unchanged
        - compact: Compact
        - spread: Spread
    - cpuSetAvoidSmt: false
      $name: CPU set avoid SMT
      $description: When enabled, keeps only one logical CPU Set per physical core.
    - cpuPriority: high
      $name: CPU priority class
      $options:
        - unchanged: Unchanged
        - idle: Idle
        - below-normal: Below normal
        - normal: Normal
        - above-normal: Above normal
        - high: High
        - realtime: Realtime
    - dynamicPriorityBoost: enable
      $name: Dynamic priority boost
      $description: Controls SetProcessPriorityBoost. Enable for better interactive responsiveness, disable for steadier scheduling.
      $options:
        - unchanged: Unchanged
        - enable: Enable
        - disable: Disable
    - cpuPowerMode: disable-throttling
      $name: CPU power mode
      $description: Controls ProcessPowerThrottling / execution-speed throttling for the process. eco-qos-full also ignores timer resolution requests.
      $options:
        - unchanged: Unchanged
        - disable-throttling: Disable throttling
        - enable-throttling: Enable throttling
        - eco-qos-full: Full Eco QoS
    - cpuSetEfficiencyPreference: unchanged
      $name: CPU set efficiency preference
      $description: On hybrid CPUs, force the process onto performance-only or efficiency-only cores.
      $options:
        - unchanged: Unchanged
        - performance-only: Performance only
        - efficiency-only: Efficiency only
        - balanced: Balanced (all)
    - timerResolution: unchanged
      $name: Timer resolution
      $description: Lowers the system timer tick for this process. Reduces input latency but increases power draw.
      $options:
        - unchanged: Unchanged
        - max-resolution: Max resolution (0.5 ms)
    - workingSetMode: unchanged
      $name: Working set mode
      $description: Pin pages in RAM to prevent stutters, or trim idle bloat.
      $options:
        - unchanged: Unchanged
        - lock-minimum: Lock minimum (keep pages resident)
        - trim: Trim (empty working set)
    - profile: unchanged
      $name: Profile
      $description: A composite preset. low-latency applies optimal latency settings. Explicit per-field settings override profile defaults.
      $options:
        - unchanged: Unchanged
        - low-latency: Low latency
    - ioPriority: unchanged
      $name: I/O priority
      $description: Controls the process I/O priority.
      $options:
        - unchanged: Unchanged
        - very-low: Very low
        - low: Low
        - normal: Normal
        - high: High
        - critical: Critical
    - pagePriority: unchanged
      $name: Page priority
      $description: Controls the process page priority.
      $options:
        - unchanged: Unchanged
        - very-low: Very low
        - low: Low
        - medium: Medium
        - below-normal: Below normal
        - normal: Normal
    - memoryPriority: normal
      $name: Memory priority
      $description: Controls how aggressively Windows trims this process's memory under pressure.
      $options:
        - unchanged: Unchanged
        - very-low: Very low
        - low: Low
        - medium: Medium
        - below-normal: Below normal
        - normal: Normal
    - gpuPreference: high-performance
      $name: GPU preference
      $description: Writes the Windows Graphics Settings preference for this executable.
      $options:
        - unchanged: Unchanged
        - system-default: System default
        - power-saving: Power saving
        - high-performance: High performance
    - autoHdr: unchanged
      $name: Auto HDR
      $description: Force Auto HDR state for this executable.
      $options:
        - unchanged: Unchanged
        - enable: Enable
        - disable: Disable
    - windowedOptimizations: unchanged
      $name: Windowed Optimizations
      $description: Upgrade legacy DX10/11 presentation from blt-model to flip-model (reduces latency & enables VRR in borderless window).
      $options:
        - unchanged: Unchanged
        - enable: Enable
        - disable: Disable
    - variableRefreshRate: unchanged
      $name: Variable Refresh Rate (VRR)
      $description: Force Variable Refresh Rate (G-Sync/FreeSync) optimizations for this executable.
      $options:
        - unchanged: Unchanged
        - enable: Enable
        - disable: Disable
    - npuMode: unchanged
      $name: NPU mode
      $description: Best-effort NPU forcing for ONNX Runtime apps. Only affects new ONNX Runtime sessions.
      $options:
        - unchanged: Unchanged
        - onnxruntime-prefer-npu: ONNX Runtime prefer NPU
        - onnxruntime-qnn: ONNX Runtime QNN HTP
        - onnxruntime-openvino: ONNX Runtime OpenVINO NPU
    - npuDisableCpuFallback: false
      $name: NPU disable CPU fallback
      $description: Writes session.disable_cpu_ep_fallback=1 for new ONNX Runtime sessions. Unsupported nodes will fail session creation instead of silently falling back to CPU.
    - npuQnnPerformanceMode: unchanged
      $name: NPU QNN performance mode
      $description: Optional Qualcomm HTP performance hint used with onnxruntime-qnn.
      $options:
        - unchanged: Unchanged
        - default: Default
        - balanced: Balanced
        - burst: Burst
        - high-performance: High performance
        - sustained-high-performance: Sustained high performance
        - power-saver: Power saver
        - high-power-saver: High power saver
        - low-balanced: Low balanced
        - low-power-saver: Low power saver
        - extreme-power-saver: Extreme power saver
    - npuOpenVinoDeviceType: unchanged
      $name: NPU OpenVINO device type
      $description: Optional OpenVINO device_type used with onnxruntime-openvino.
      $options:
        - unchanged: Unchanged
        - npu: NPU
        - auto-npu-cpu: AUTO:NPU,CPU
        - hetero-npu-cpu: HETERO:NPU,CPU
        - multi-npu-cpu: MULTI:NPU,CPU
    - npuOpenVinoEnableFastCompile: false
      $name: NPU OpenVINO fast compile
      $description: Enables OpenVINO's enable_npu_fast_compile hint when using onnxruntime-openvino.
  $name: Target rules

- logVerbose: true
  $name: Verbose logging
  $description: Logs the chosen rule and every applied or restored override.

- picker:
  - launch: false
    $name: Open running-process picker
    $description: Toggle on to open a dialog in Windhawk. The selected value is copied to the clipboard and saved as @picked.
  - output: full-path
    $name: Picker output
    $description: Controls what the picker copies to the clipboard and stores as @picked.
    $options:
      - full-path: Full path
      - exe-name: Executable name
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shlwapi.h>
#include <tlhelp32.h>

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <cwctype>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

typedef LONG NTSTATUS;
#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#endif

constexpr ULONG ProcessIoPriorityClass = 33;
constexpr ULONG ProcessPagePriorityClass = 39;

using NtSetInformationProcess_t = NTSTATUS(WINAPI*)(HANDLE, ULONG, PVOID, ULONG);
using NtQueryInformationProcess_t = NTSTATUS(WINAPI*)(HANDLE, ULONG, PVOID, ULONG, ULONG*);
using NtSetTimerResolution_t = NTSTATUS(WINAPI*)(ULONG, BOOLEAN, PULONG);
using NtQueryTimerResolution_t = NTSTATUS(WINAPI*)(PULONG, PULONG, PULONG);
using SetProcessWorkingSetSizeEx_t = BOOL(WINAPI*)(HANDLE, SIZE_T, SIZE_T, DWORD);
using GetProcessWorkingSetSizeEx_t = BOOL(WINAPI*)(HANDLE, PSIZE_T, PSIZE_T, PDWORD);

#ifndef QUOTA_LIMITS_HARDWS_MIN_ENABLE
#define QUOTA_LIMITS_HARDWS_MIN_ENABLE  0x00000001
#define QUOTA_LIMITS_HARDWS_MIN_DISABLE 0x00000002
#define QUOTA_LIMITS_HARDWS_MAX_ENABLE  0x00000004
#define QUOTA_LIMITS_HARDWS_MAX_DISABLE 0x00000008
#endif

#ifndef PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION
#define PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION 0x4
#endif

#ifndef PROCESS_POWER_THROTTLING_CURRENT_VERSION
typedef struct _PROCESS_POWER_THROTTLING_STATE {
    ULONG Version;
    ULONG ControlMask;
    ULONG StateMask;
} PROCESS_POWER_THROTTLING_STATE, *PPROCESS_POWER_THROTTLING_STATE;

#define PROCESS_POWER_THROTTLING_CURRENT_VERSION 1
#define PROCESS_POWER_THROTTLING_EXECUTION_SPEED 0x1
#endif

#ifndef MEMORY_PRIORITY_NORMAL
#define MEMORY_PRIORITY_VERY_LOW 1
#define MEMORY_PRIORITY_LOW 2
#define MEMORY_PRIORITY_MEDIUM 3
#define MEMORY_PRIORITY_BELOW_NORMAL 4
#define MEMORY_PRIORITY_NORMAL 5
#endif

typedef struct _COMPAT_MEMORY_PRIORITY_INFORMATION {
    ULONG MemoryPriority;
} COMPAT_MEMORY_PRIORITY_INFORMATION;

namespace {

constexpr wchar_t kGpuPreferencesKeyPath[] =
    L"SOFTWARE\\Microsoft\\DirectX\\UserGpuPreferences";
constexpr wchar_t kPickedProcessValueName[] = L"pickedProcessMatch";
constexpr DWORD kProcessPowerThrottlingInfoClass = 4;

constexpr wchar_t kPickerWindowClassName[] =
    L"WindhawkForceProcessAcceleratorsPicker";
constexpr int kPickerWindowWidth = 980;
constexpr int kPickerWindowHeight = 560;
constexpr int kPickerPadding = 12;
constexpr int kPickerButtonWidth = 140;
constexpr int kPickerButtonHeight = 28;

constexpr int kPickerListId = 1001;
constexpr int kPickerSelectId = 1002;
constexpr int kPickerCancelId = 1003;

constexpr wchar_t kOnnxRuntimeModuleName[] = L"onnxruntime.dll";
constexpr char kOrtDisableCpuEpFallbackKey[] = "session.disable_cpu_ep_fallback";
constexpr char kOrtQnnProviderName[] = "QNN";
constexpr char kOrtOpenVinoProviderName[] = "OpenVINO";
constexpr uint32_t kOrtBaseApiVersion = 1;
constexpr uint32_t kOrtGenericProviderApiVersion = 12;
constexpr uint32_t kOrtNpuPolicyApiVersion = 22;
constexpr size_t kOrtApiCreateSessionOptionsIndex = 10;
constexpr size_t kOrtApiGetErrorMessageIndex = 2;
constexpr size_t kOrtApiReleaseStatusIndex = 93;
constexpr size_t kOrtApiAddSessionConfigEntryIndex = 130;
constexpr size_t kOrtApiSessionOptionsAppendExecutionProviderIndex = 216;
constexpr size_t kOrtApiSetEpSelectionPolicyIndex = 304;

enum class CpuPowerMode {
    kUnchanged,
    kDisableThrottling,
    kEnableThrottling,
    kEcoQosFull,
};

enum class TimerResolutionMode {
    kUnchanged,
    kMaxResolution,
};

enum class WorkingSetMode {
    kUnchanged,
    kLockMinimum,
    kTrim,
};

enum class ProfileMode {
    kUnchanged,
    kLowLatency,
};

enum class EfficiencyPreference {
    kUnchanged,
    kPerformanceOnly,
    kEfficiencyOnly,
    kBalanced,
};

enum class CpuSetSelectionMode {
    kUnchanged,
    kAll,
    kPerformance,
    kEfficiency,
    kExplicitIds,
};

enum class CpuSetPlacementMode {
    kUnchanged,
    kCompact,
    kSpread,
};

enum class DynamicPriorityBoostMode {
    kUnchanged,
    kEnable,
    kDisable,
};

enum class ToggleMode {
    kUnchanged,
    kEnable,
    kDisable,
};

enum class NpuMode {
    kUnchanged,
    kOnnxRuntimePreferNpu,
    kOnnxRuntimeQnn,
    kOnnxRuntimeOpenVino,
};

enum class GpuPreference {
    kUnchanged,
    kSystemDefault,
    kPowerSaving,
    kHighPerformance,
};

struct RuleSettings {
    bool matched = false;
    std::wstring match;
    std::wstring whenProcessesRunning;
    std::wstring whenProcessesNotRunning;
    std::wstring cpuCores;
    std::wstring cpuSets;
    std::wstring cpuSetNumaNode;
    std::wstring cpuSetLastLevelCache;
    std::wstring cpuSetLimit;
    std::wstring cpuSetPlacement;
    bool cpuSetAvoidSmt = false;
    std::wstring cpuPriority;
    std::wstring dynamicPriorityBoost;
    std::wstring cpuPowerMode;
    std::wstring cpuSetEfficiencyPreference;
    std::wstring timerResolution;
    std::wstring workingSetMode;
    std::wstring profile;
    std::wstring ioPriority;
    std::wstring pagePriority;
    std::wstring memoryPriority;
    std::wstring gpuPreference;
    std::wstring autoHdr;
    std::wstring windowedOptimizations;
    std::wstring variableRefreshRate;
    std::wstring npuMode;
    bool npuDisableCpuFallback = false;
    std::wstring npuQnnPerformanceMode;
    std::wstring npuOpenVinoDeviceType;
    bool npuOpenVinoEnableFastCompile = false;
};

struct OriginalState {
    bool captured = false;

    bool priorityValid = false;
    DWORD priorityClass = 0;

    bool priorityBoostValid = false;
    BOOL disablePriorityBoost = FALSE;

    bool affinityValid = false;
    DWORD_PTR processAffinityMask = 0;
    DWORD_PTR systemAffinityMask = 0;

    bool cpuSetsValid = false;
    std::vector<ULONG> processDefaultCpuSets;

    bool powerThrottlingValid = false;
    PROCESS_POWER_THROTTLING_STATE powerThrottlingState{};

    bool ioPriorityValid = false;
    ULONG ioPriority = 0;

    bool pagePriorityValid = false;
    ULONG pagePriority = 0;

    bool timerResolutionValid = false;
    ULONG timerResolutionOriginal = 0;
    bool timerResolutionActive = false;

    bool workingSetValid = false;
    SIZE_T workingSetMin = 0;
    SIZE_T workingSetMax = 0;
    DWORD workingSetFlags = 0;

    bool memoryPriorityValid = false;
    COMPAT_MEMORY_PRIORITY_INFORMATION memoryPriorityInfo{};

    bool gpuPreferenceValid = false;
    bool gpuPreferenceExisted = false;
    std::wstring gpuPreferenceValue;
};

struct RunningProcessEntry {
    DWORD pid = 0;
    std::wstring processName;
    std::wstring processPath;
};

struct ProcessPickerState {
    std::wstring outputMode;
    std::vector<RunningProcessEntry> entries;
    HWND listBox = nullptr;
};

struct CpuSetInfo {
    ULONG id = 0;
    WORD group = 0;
    BYTE logicalProcessorIndex = 0;
    BYTE coreIndex = 0;
    BYTE lastLevelCacheIndex = 0;
    BYTE numaNodeIndex = 0;
    BYTE efficiencyClass = 0;
    bool parked = false;
    bool allocated = false;
    bool allocatedToTargetProcess = false;
};

struct ParsedCpuSetSelection {
    CpuSetSelectionMode mode = CpuSetSelectionMode::kUnchanged;
    std::vector<ULONG> explicitIds;
};

struct OrtStatus;
struct OrtSessionOptions;
struct OrtApi;

enum OrtExecutionProviderDevicePolicy {
    OrtExecutionProviderDevicePolicy_DEFAULT = 0,
    OrtExecutionProviderDevicePolicy_PREFER_CPU = 1,
    OrtExecutionProviderDevicePolicy_PREFER_NPU = 2,
    OrtExecutionProviderDevicePolicy_PREFER_GPU = 3,
};

using OrtGetApi_t = const OrtApi*(WINAPI*)(uint32_t version);
using OrtGetVersionString_t = const char*(WINAPI*)();

struct OrtApiBase {
    OrtGetApi_t GetApi;
    OrtGetVersionString_t GetVersionString;
};

using OrtGetApiBaseExport_t = const OrtApiBase*(WINAPI*)();
using OrtCreateSessionOptions_t = OrtStatus*(WINAPI*)(OrtSessionOptions** options);
using OrtGetErrorMessage_t = const char*(WINAPI*)(const OrtStatus* status);
using OrtReleaseStatus_t = void(WINAPI*)(OrtStatus* status);
using OrtAddSessionConfigEntry_t =
    OrtStatus*(WINAPI*)(OrtSessionOptions* sessionOptions,
                        const char* configKey,
                        const char* configValue);
using OrtSessionOptionsAppendExecutionProvider_t =
    OrtStatus*(WINAPI*)(OrtSessionOptions* sessionOptions,
                        const char* providerName,
                        const char* const* providerOptionsKeys,
                        const char* const* providerOptionsValues,
                        size_t numKeys);
using OrtSessionOptionsSetEpSelectionPolicy_t =
    OrtStatus*(WINAPI*)(OrtSessionOptions* sessionOptions,
                        OrtExecutionProviderDevicePolicy policy);

struct OrtPatchedApiTable {
    OrtApi* api = nullptr;
    OrtCreateSessionOptions_t originalCreateSessionOptions = nullptr;
};

using SetProcessInformation_t = BOOL(WINAPI*)(HANDLE, DWORD, LPVOID, DWORD);
using GetProcessInformation_t = BOOL(WINAPI*)(HANDLE, DWORD, LPVOID, DWORD);
using GetSystemCpuSetInformation_t =
    BOOL(WINAPI*)(PSYSTEM_CPU_SET_INFORMATION, ULONG, PULONG, HANDLE, ULONG);
using GetProcessDefaultCpuSets_t =
    BOOL(WINAPI*)(HANDLE, PULONG, ULONG, PULONG);
using SetProcessDefaultCpuSets_t =
    BOOL(WINAPI*)(HANDLE, const ULONG*, ULONG);
using LoadLibraryW_t = HMODULE(WINAPI*)(LPCWSTR);
using LoadLibraryA_t = HMODULE(WINAPI*)(LPCSTR);
using LoadLibraryExW_t = HMODULE(WINAPI*)(LPCWSTR, HANDLE, DWORD);
using LoadLibraryExA_t = HMODULE(WINAPI*)(LPCSTR, HANDLE, DWORD);

bool g_logVerbose = true;
bool g_isPickerHost = false;
bool g_pickerLaunchArmed = false;
std::atomic<bool> g_pickerOpen = false;
std::atomic<bool> g_loggedOrtNpuUnsupported = false;
std::atomic<bool> g_loggedOrtNpuFailure = false;
std::atomic<bool> g_loggedOrtNpuApplied = false;
std::wstring g_processPath;
std::wstring g_processName;
RuleSettings g_activeRule;
OriginalState g_originalState;
NtSetInformationProcess_t g_ntSetInformationProcess = nullptr;
NtQueryInformationProcess_t g_ntQueryInformationProcess = nullptr;
NtSetTimerResolution_t g_ntSetTimerResolution = nullptr;
NtQueryTimerResolution_t g_ntQueryTimerResolution = nullptr;
SetProcessWorkingSetSizeEx_t g_setProcessWorkingSetSizeEx = nullptr;
GetProcessWorkingSetSizeEx_t g_getProcessWorkingSetSizeEx = nullptr;
SetProcessInformation_t g_setProcessInformation = nullptr;
GetProcessInformation_t g_getProcessInformation = nullptr;
GetSystemCpuSetInformation_t g_getSystemCpuSetInformation = nullptr;
GetProcessDefaultCpuSets_t g_getProcessDefaultCpuSets = nullptr;
SetProcessDefaultCpuSets_t g_setProcessDefaultCpuSets = nullptr;
LoadLibraryW_t g_originalLoadLibraryW = nullptr;
LoadLibraryA_t g_originalLoadLibraryA = nullptr;
LoadLibraryExW_t g_originalLoadLibraryExW = nullptr;
LoadLibraryExA_t g_originalLoadLibraryExA = nullptr;
OrtGetApiBaseExport_t g_originalOrtGetApiBase = nullptr;
OrtGetApi_t g_originalOrtGetApi = nullptr;
OrtCreateSessionOptions_t g_originalOrtCreateSessionOptions = nullptr;
OrtGetErrorMessage_t g_ortGetErrorMessage = nullptr;
OrtReleaseStatus_t g_ortReleaseStatus = nullptr;
OrtAddSessionConfigEntry_t g_ortAddSessionConfigEntry = nullptr;
OrtSessionOptionsAppendExecutionProvider_t g_ortAppendExecutionProvider = nullptr;
OrtSessionOptionsSetEpSelectionPolicy_t g_ortSetEpSelectionPolicy = nullptr;
const OrtApiBase* g_patchedOrtApiBase = nullptr;
std::vector<OrtPatchedApiTable> g_patchedOrtApiTables;
SRWLOCK g_ortHookLock = SRWLOCK_INIT;
bool g_loadLibraryHooksInstalled = false;
bool g_ortExportHookInstalled = false;

std::wstring TrimCopy(std::wstring_view value) {
    size_t start = 0;
    size_t end = value.size();

    while (start < end && iswspace(value[start])) {
        start++;
    }

    while (end > start && iswspace(value[end - 1])) {
        end--;
    }

    return std::wstring(value.substr(start, end - start));
}

std::wstring ToLowerCopy(std::wstring_view value) {
    std::wstring result(value);
    std::transform(result.begin(), result.end(), result.begin(),
                   [](wchar_t ch) { return static_cast<wchar_t>(towlower(ch)); });
    return result;
}

bool EqualsIgnoreCase(std::wstring_view left, std::wstring_view right) {
    return ToLowerCopy(left) == ToLowerCopy(right);
}

bool IsDefaultSetting(std::wstring_view value) {
    std::wstring lowered = ToLowerCopy(TrimCopy(value));
    return lowered.empty() || lowered == L"unchanged";
}

bool StartsWithIgnoreCase(std::wstring_view value, std::wstring_view prefix) {
    if (value.size() < prefix.size()) {
        return false;
    }

    return EqualsIgnoreCase(value.substr(0, prefix.size()), prefix);
}

std::wstring NormalizeMatchPattern(std::wstring pattern) {
    pattern = TrimCopy(pattern);

    if (pattern.size() >= 2 && pattern.front() == L'"' && pattern.back() == L'"') {
        pattern = pattern.substr(1, pattern.size() - 2);
    }

    std::replace(pattern.begin(), pattern.end(), L'/', L'\\');
    return pattern;
}

std::wstring GetStoredStringValue(PCWSTR valueName);
bool PatternMatches(std::wstring_view candidate, const std::wstring& pattern);
bool EvaluateRunningProcessConditions(const std::wstring& whenProcessesRunning,
                                      const std::wstring& whenProcessesNotRunning,
                                      std::wstring* errorMessage);

std::wstring ResolveMatchPattern(const std::wstring& rawPattern) {
    std::wstring pattern = NormalizeMatchPattern(rawPattern);
    if (EqualsIgnoreCase(pattern, L"@picked")) {
        return NormalizeMatchPattern(GetStoredStringValue(kPickedProcessValueName));
    }

    return pattern;
}

template <typename... Args>
std::wstring GetStringSettingCopy(PCWSTR valueName, Args... args) {
    auto value = WindhawkUtils::StringSetting::make(valueName, args...);
    return value.get() ? std::wstring(value.get()) : std::wstring();
}

std::wstring GetStoredStringValue(PCWSTR valueName) {
    std::vector<wchar_t> buffer(32768, L'\0');
    size_t copied =
        Wh_GetStringValue(valueName, buffer.data(), buffer.size());
    if (copied == 0) {
        return {};
    }

    return std::wstring(buffer.data(), copied);
}

std::wstring GetCurrentProcessPath() {
    std::wstring path(MAX_PATH, L'\0');

    for (;;) {
        DWORD copied =
            GetModuleFileNameW(nullptr, path.data(), static_cast<DWORD>(path.size()));
        if (copied == 0) {
            return {};
        }

        if (copied < path.size() - 1) {
            path.resize(copied);
            return path;
        }

        path.resize(path.size() * 2);
    }
}

std::wstring GetExeNameFromPath(const std::wstring& path) {
    size_t slash = path.find_last_of(L"\\/");
    if (slash == std::wstring::npos) {
        return path;
    }

    return path.substr(slash + 1);
}

CpuSetPlacementMode ParseCpuSetPlacementMode(const std::wstring& rawValue) {
    std::wstring value = ToLowerCopy(TrimCopy(rawValue));

    if (value.empty() || value == L"unchanged") {
        return CpuSetPlacementMode::kUnchanged;
    }

    if (value == L"compact") {
        return CpuSetPlacementMode::kCompact;
    }

    if (value == L"spread") {
        return CpuSetPlacementMode::kSpread;
    }

    return CpuSetPlacementMode::kUnchanged;
}

NpuMode ParseNpuMode(const std::wstring& rawValue) {
    std::wstring value = ToLowerCopy(TrimCopy(rawValue));

    if (value.empty() || value == L"unchanged") {
        return NpuMode::kUnchanged;
    }

    if (value == L"onnxruntime-prefer-npu") {
        return NpuMode::kOnnxRuntimePreferNpu;
    }

    if (value == L"onnxruntime-qnn") {
        return NpuMode::kOnnxRuntimeQnn;
    }

    if (value == L"onnxruntime-openvino") {
        return NpuMode::kOnnxRuntimeOpenVino;
    }

    return NpuMode::kUnchanged;
}

DynamicPriorityBoostMode ParseDynamicPriorityBoostMode(
    const std::wstring& rawValue) {
    std::wstring value = ToLowerCopy(TrimCopy(rawValue));

    if (value.empty() || value == L"unchanged") {
        return DynamicPriorityBoostMode::kUnchanged;
    }

    if (value == L"enable") {
        return DynamicPriorityBoostMode::kEnable;
    }

    if (value == L"disable") {
        return DynamicPriorityBoostMode::kDisable;
    }

    return DynamicPriorityBoostMode::kUnchanged;
}

std::optional<ULONG> ParseMemoryPriority(const std::wstring& rawValue) {
    std::wstring value = ToLowerCopy(TrimCopy(rawValue));

    if (value.empty() || value == L"unchanged") {
        return std::nullopt;
    }

    if (value == L"very-low") {
        return MEMORY_PRIORITY_VERY_LOW;
    }

    if (value == L"low") {
        return MEMORY_PRIORITY_LOW;
    }

    if (value == L"medium") {
        return MEMORY_PRIORITY_MEDIUM;
    }

    if (value == L"below-normal") {
        return MEMORY_PRIORITY_BELOW_NORMAL;
    }

    if (value == L"normal") {
        return MEMORY_PRIORITY_NORMAL;
    }

    return std::nullopt;
}

std::optional<ULONG> ParseIoPriority(const std::wstring& rawValue) {
    std::wstring value = ToLowerCopy(TrimCopy(rawValue));

    if (value.empty() || value == L"unchanged") {
        return std::nullopt;
    }

    if (value == L"very-low") {
        return 0;
    }

    if (value == L"low") {
        return 1;
    }

    if (value == L"normal") {
        return 2;
    }

    if (value == L"high") {
        return 3;
    }

    if (value == L"critical") {
        return 4;
    }

    return std::nullopt;
}

std::optional<ULONG> ParsePagePriority(const std::wstring& rawValue) {
    std::wstring value = ToLowerCopy(TrimCopy(rawValue));

    if (value.empty() || value == L"unchanged") {
        return std::nullopt;
    }

    if (value == L"very-low") {
        return MEMORY_PRIORITY_VERY_LOW;
    }

    if (value == L"low") {
        return MEMORY_PRIORITY_LOW;
    }

    if (value == L"medium") {
        return MEMORY_PRIORITY_MEDIUM;
    }

    if (value == L"below-normal") {
        return MEMORY_PRIORITY_BELOW_NORMAL;
    }

    if (value == L"normal") {
        return MEMORY_PRIORITY_NORMAL;
    }

    return std::nullopt;
}

bool IsOnnxRuntimeNpuModeEnabled() {
    return ParseNpuMode(g_activeRule.npuMode) != NpuMode::kUnchanged ||
           g_activeRule.npuDisableCpuFallback;
}

std::wstring Utf8ToWideCopy(const char* text) {
    if (!text || !*text) {
        return {};
    }

    int size = MultiByteToWideChar(CP_UTF8, 0, text, -1, nullptr, 0);
    if (size <= 0) {
        size = MultiByteToWideChar(CP_ACP, 0, text, -1, nullptr, 0);
        if (size <= 0) {
            return {};
        }

        std::wstring converted(size - 1, L'\0');
        MultiByteToWideChar(CP_ACP, 0, text, -1, converted.data(), size);
        return converted;
    }

    std::wstring converted(size - 1, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, text, -1, converted.data(), size);
    return converted;
}

std::wstring MultiByteToWideCopy(UINT codePage, const char* text) {
    if (!text || !*text) {
        return {};
    }

    int size = MultiByteToWideChar(codePage, 0, text, -1, nullptr, 0);
    if (size <= 0) {
        return {};
    }

    std::wstring converted(size - 1, L'\0');
    MultiByteToWideChar(codePage, 0, text, -1, converted.data(), size);
    return converted;
}

std::wstring GetModulePath(HMODULE module) {
    std::wstring path(MAX_PATH, L'\0');

    for (;;) {
        DWORD copied = GetModuleFileNameW(module, path.data(),
                                          static_cast<DWORD>(path.size()));
        if (copied == 0) {
            return {};
        }

        if (copied < path.size() - 1) {
            path.resize(copied);
            return path;
        }

        path.resize(path.size() * 2);
    }
}

bool IsOnnxRuntimeModulePath(const std::wstring& path) {
    if (path.empty()) {
        return false;
    }

    std::wstring normalized = NormalizeMatchPattern(path);
    return EqualsIgnoreCase(GetExeNameFromPath(normalized), kOnnxRuntimeModuleName);
}

bool IsOnnxRuntimeModule(HMODULE module) {
    return module && IsOnnxRuntimeModulePath(GetModulePath(module));
}

template <typename T>
bool ReplaceFunctionPointer(T* target, T replacement, T* originalValue = nullptr) {
    if (!target) {
        return false;
    }

    if (originalValue && !*originalValue) {
        *originalValue = *target;
    }

    if (*target == replacement) {
        return true;
    }

    DWORD oldProtect = 0;
    if (!VirtualProtect(target, sizeof(*target), PAGE_READWRITE, &oldProtect)) {
        Wh_Log(L"VirtualProtect failed while patching ONNX Runtime pointers: %lu",
               GetLastError());
        return false;
    }

    *target = replacement;
    FlushInstructionCache(GetCurrentProcess(), target, sizeof(*target));

    DWORD unused = 0;
    VirtualProtect(target, sizeof(*target), oldProtect, &unused);
    return true;
}

template <typename T>
bool RestoreFunctionPointer(T* target, T expectedCurrent, T originalValue) {
    if (!target || !originalValue || *target != expectedCurrent) {
        return true;
    }

    DWORD oldProtect = 0;
    if (!VirtualProtect(target, sizeof(*target), PAGE_READWRITE, &oldProtect)) {
        return false;
    }

    *target = originalValue;
    FlushInstructionCache(GetCurrentProcess(), target, sizeof(*target));

    DWORD unused = 0;
    VirtualProtect(target, sizeof(*target), oldProtect, &unused);
    return true;
}

const OrtApi* WINAPI OrtGetApi_Hook(uint32_t version);
const OrtApiBase* WINAPI OrtGetApiBase_Hook();
OrtStatus* WINAPI OrtCreateSessionOptions_Hook(OrtSessionOptions** options);
HMODULE WINAPI LoadLibraryW_Hook(LPCWSTR lpLibFileName);
HMODULE WINAPI LoadLibraryA_Hook(LPCSTR lpLibFileName);
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile,
                                   DWORD dwFlags);
HMODULE WINAPI LoadLibraryExA_Hook(LPCSTR lpLibFileName, HANDLE hFile,
                                   DWORD dwFlags);

bool ResolveOnnxRuntimeNpuFunctionsLocked() {
    if (!g_originalOrtGetApi) {
        return false;
    }

    const OrtApi* baseApi = g_originalOrtGetApi(kOrtBaseApiVersion);
    if (baseApi) {
        auto slots = reinterpret_cast<void* const*>(baseApi);

        if (!g_originalOrtCreateSessionOptions) {
            g_originalOrtCreateSessionOptions =
                reinterpret_cast<OrtCreateSessionOptions_t>(
                    const_cast<void*>(slots[kOrtApiCreateSessionOptionsIndex]));
        }

        if (!g_ortGetErrorMessage) {
            g_ortGetErrorMessage = reinterpret_cast<OrtGetErrorMessage_t>(
                const_cast<void*>(slots[kOrtApiGetErrorMessageIndex]));
        }

        if (!g_ortReleaseStatus) {
            g_ortReleaseStatus = reinterpret_cast<OrtReleaseStatus_t>(
                const_cast<void*>(slots[kOrtApiReleaseStatusIndex]));
        }
    }

    const OrtApi* providerApi = g_originalOrtGetApi(kOrtGenericProviderApiVersion);
    if (providerApi) {
        auto slots = reinterpret_cast<void* const*>(providerApi);

        if (!g_ortAddSessionConfigEntry) {
            g_ortAddSessionConfigEntry =
                reinterpret_cast<OrtAddSessionConfigEntry_t>(
                    const_cast<void*>(slots[kOrtApiAddSessionConfigEntryIndex]));
        }

        if (!g_ortAppendExecutionProvider) {
            g_ortAppendExecutionProvider =
                reinterpret_cast<OrtSessionOptionsAppendExecutionProvider_t>(
                    const_cast<void*>(
                        slots[kOrtApiSessionOptionsAppendExecutionProviderIndex]));
        }
    }

    const OrtApi* policyApi = g_originalOrtGetApi(kOrtNpuPolicyApiVersion);
    if (policyApi && !g_ortSetEpSelectionPolicy) {
        auto slots = reinterpret_cast<void* const*>(policyApi);
        g_ortSetEpSelectionPolicy =
            reinterpret_cast<OrtSessionOptionsSetEpSelectionPolicy_t>(
                const_cast<void*>(slots[kOrtApiSetEpSelectionPolicyIndex]));
    }

    return g_originalOrtCreateSessionOptions != nullptr;
}

bool PatchOrtApiTableLocked(const OrtApi* api) {
    if (!api) {
        return false;
    }

    auto mutableApi = const_cast<OrtApi*>(api);
    void** slots = reinterpret_cast<void**>(mutableApi);
    void* currentValue = slots[kOrtApiCreateSessionOptionsIndex];

    if (currentValue == reinterpret_cast<void*>(&OrtCreateSessionOptions_Hook)) {
        return true;
    }

    OrtPatchedApiTable record;
    record.api = mutableApi;
    record.originalCreateSessionOptions =
        reinterpret_cast<OrtCreateSessionOptions_t>(currentValue);
    g_patchedOrtApiTables.push_back(record);

    if (!g_originalOrtCreateSessionOptions) {
        g_originalOrtCreateSessionOptions = record.originalCreateSessionOptions;
    }

    return ReplaceFunctionPointer(
        &slots[kOrtApiCreateSessionOptionsIndex],
        reinterpret_cast<void*>(&OrtCreateSessionOptions_Hook));
}

bool PatchOrtApiBaseLocked(const OrtApiBase* apiBase) {
    if (!apiBase) {
        return false;
    }

    auto mutableApiBase = const_cast<OrtApiBase*>(apiBase);

    if (!g_originalOrtGetApi) {
        g_originalOrtGetApi = mutableApiBase->GetApi;
    }

    ResolveOnnxRuntimeNpuFunctionsLocked();

    if (mutableApiBase->GetApi == &OrtGetApi_Hook) {
        g_patchedOrtApiBase = apiBase;
        return true;
    }

    if (!ReplaceFunctionPointer(&mutableApiBase->GetApi, &OrtGetApi_Hook)) {
        return false;
    }

    g_patchedOrtApiBase = apiBase;
    if (g_logVerbose) {
        Wh_Log(L"Patched ONNX Runtime API base in %s.", g_processName.c_str());
    }

    return true;
}

void EnsureOnnxRuntimeModuleHooked(HMODULE module) {
    if (!module || !IsOnnxRuntimeModule(module)) {
        return;
    }

    AcquireSRWLockExclusive(&g_ortHookLock);

    if (!g_ortExportHookInstalled) {
        auto target = reinterpret_cast<OrtGetApiBaseExport_t>(
            GetProcAddress(module, "OrtGetApiBase"));
        if (!target) {
            ReleaseSRWLockExclusive(&g_ortHookLock);
            return;
        }

        if (!WindhawkUtils::SetFunctionHook(target, OrtGetApiBase_Hook,
                                            &g_originalOrtGetApiBase) ||
            !Wh_ApplyHookOperations()) {
            Wh_Log(L"Failed to hook OrtGetApiBase in %s.", g_processName.c_str());
            ReleaseSRWLockExclusive(&g_ortHookLock);
            return;
        }

        g_ortExportHookInstalled = true;
    }

    if (g_originalOrtGetApiBase) {
        PatchOrtApiBaseLocked(g_originalOrtGetApiBase());
    }

    ReleaseSRWLockExclusive(&g_ortHookLock);
}

bool EnsureOnnxRuntimeNpuHooks() {
    AcquireSRWLockExclusive(&g_ortHookLock);

    if (!g_loadLibraryHooksInstalled) {
        if (!WindhawkUtils::SetFunctionHook(LoadLibraryW, LoadLibraryW_Hook,
                                            &g_originalLoadLibraryW) ||
            !WindhawkUtils::SetFunctionHook(LoadLibraryA, LoadLibraryA_Hook,
                                            &g_originalLoadLibraryA) ||
            !WindhawkUtils::SetFunctionHook(LoadLibraryExW, LoadLibraryExW_Hook,
                                            &g_originalLoadLibraryExW) ||
            !WindhawkUtils::SetFunctionHook(LoadLibraryExA, LoadLibraryExA_Hook,
                                            &g_originalLoadLibraryExA) ||
            !Wh_ApplyHookOperations()) {
            Wh_Log(L"Failed to install ONNX Runtime loader hooks in %s.",
                   g_processName.c_str());
            ReleaseSRWLockExclusive(&g_ortHookLock);
            return false;
        }

        g_loadLibraryHooksInstalled = true;
    }

    ReleaseSRWLockExclusive(&g_ortHookLock);

    EnsureOnnxRuntimeModuleHooked(GetModuleHandleW(kOnnxRuntimeModuleName));
    return true;
}

void RestoreOnnxRuntimePatches() {
    AcquireSRWLockExclusive(&g_ortHookLock);

    for (const OrtPatchedApiTable& record : g_patchedOrtApiTables) {
        if (!record.api || !record.originalCreateSessionOptions) {
            continue;
        }

        auto slots = reinterpret_cast<void**>(record.api);
        RestoreFunctionPointer(&slots[kOrtApiCreateSessionOptionsIndex],
                               reinterpret_cast<void*>(&OrtCreateSessionOptions_Hook),
                               reinterpret_cast<void*>(
                                   record.originalCreateSessionOptions));
    }

    g_patchedOrtApiTables.clear();

    if (g_patchedOrtApiBase && g_originalOrtGetApi) {
        auto mutableApiBase = const_cast<OrtApiBase*>(g_patchedOrtApiBase);
        RestoreFunctionPointer(&mutableApiBase->GetApi, &OrtGetApi_Hook,
                               g_originalOrtGetApi);
    }

    g_patchedOrtApiBase = nullptr;
    ReleaseSRWLockExclusive(&g_ortHookLock);
}

bool PatternMatches(std::wstring_view candidate, const std::wstring& pattern) {
    return PathMatchSpecExW(candidate.data(), pattern.c_str(), 0) == S_OK;
}

bool RuleMatchesCurrentProcess(const std::wstring& rawPattern,
                               const std::wstring& whenProcessesRunning,
                               const std::wstring& whenProcessesNotRunning,
                               std::wstring* errorMessage) {
    errorMessage->clear();

    std::wstring pattern = ResolveMatchPattern(rawPattern);
    if (pattern.empty()) {
        return false;
    }

    if (!(PatternMatches(g_processPath, pattern) ||
          PatternMatches(g_processName, pattern))) {
        return false;
    }

    return EvaluateRunningProcessConditions(whenProcessesRunning,
                                            whenProcessesNotRunning,
                                            errorMessage);
}

void LoadProcessInformationFunctions() {
    HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
    if (!kernel32) {
        return;
    }

    g_setProcessInformation = reinterpret_cast<SetProcessInformation_t>(
        GetProcAddress(kernel32, "SetProcessInformation"));
    g_getProcessInformation = reinterpret_cast<GetProcessInformation_t>(
        GetProcAddress(kernel32, "GetProcessInformation"));
    g_getSystemCpuSetInformation =
        reinterpret_cast<GetSystemCpuSetInformation_t>(
            GetProcAddress(kernel32, "GetSystemCpuSetInformation"));
    g_getProcessDefaultCpuSets =
        reinterpret_cast<GetProcessDefaultCpuSets_t>(
            GetProcAddress(kernel32, "GetProcessDefaultCpuSets"));
    g_setProcessDefaultCpuSets =
        reinterpret_cast<SetProcessDefaultCpuSets_t>(
            GetProcAddress(kernel32, "SetProcessDefaultCpuSets"));
            
    g_setProcessWorkingSetSizeEx = reinterpret_cast<SetProcessWorkingSetSizeEx_t>(
        GetProcAddress(kernel32, "SetProcessWorkingSetSizeEx"));
    g_getProcessWorkingSetSizeEx = reinterpret_cast<GetProcessWorkingSetSizeEx_t>(
        GetProcAddress(kernel32, "GetProcessWorkingSetSizeEx"));

    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    if (ntdll) {
        g_ntSetInformationProcess = reinterpret_cast<NtSetInformationProcess_t>(
            GetProcAddress(ntdll, "NtSetInformationProcess"));
        g_ntQueryInformationProcess = reinterpret_cast<NtQueryInformationProcess_t>(
            GetProcAddress(ntdll, "NtQueryInformationProcess"));
        g_ntSetTimerResolution = reinterpret_cast<NtSetTimerResolution_t>(
            GetProcAddress(ntdll, "NtSetTimerResolution"));
        g_ntQueryTimerResolution = reinterpret_cast<NtQueryTimerResolution_t>(
            GetProcAddress(ntdll, "NtQueryTimerResolution"));
    }
}

bool IsGraphicsStackAlreadyLoaded() {
    return GetModuleHandleW(L"dxgi.dll") || GetModuleHandleW(L"d3d11.dll") ||
           GetModuleHandleW(L"d3d12.dll");
}

bool QueryProcessImagePath(DWORD pid, std::wstring* processPath) {
    processPath->clear();

    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        return false;
    }

    std::vector<wchar_t> buffer(32768, L'\0');
    DWORD size = static_cast<DWORD>(buffer.size());
    BOOL success = QueryFullProcessImageNameW(process, 0, buffer.data(), &size);
    CloseHandle(process);

    if (!success) {
        return false;
    }

    processPath->assign(buffer.data(), size);
    return true;
}

std::vector<RunningProcessEntry> EnumerateRunningProcesses() {
    std::vector<RunningProcessEntry> entries;

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        Wh_Log(L"CreateToolhelp32Snapshot failed: %lu", GetLastError());
        return entries;
    }

    PROCESSENTRY32W processEntry = {};
    processEntry.dwSize = sizeof(processEntry);

    if (!Process32FirstW(snapshot, &processEntry)) {
        Wh_Log(L"Process32FirstW failed: %lu", GetLastError());
        CloseHandle(snapshot);
        return entries;
    }

    do {
        RunningProcessEntry entry;
        entry.pid = processEntry.th32ProcessID;
        entry.processName = processEntry.szExeFile;
        QueryProcessImagePath(entry.pid, &entry.processPath);
        entries.push_back(std::move(entry));
    } while (Process32NextW(snapshot, &processEntry));

    CloseHandle(snapshot);

    std::sort(entries.begin(), entries.end(),
              [](const RunningProcessEntry& left,
                 const RunningProcessEntry& right) {
                  std::wstring leftName = ToLowerCopy(left.processName);
                  std::wstring rightName = ToLowerCopy(right.processName);
                  if (leftName != rightName) {
                      return leftName < rightName;
                  }

                  return left.pid < right.pid;
              });

    return entries;
}

bool RunningProcessEntryMatchesPattern(const RunningProcessEntry& entry,
                                       const std::wstring& pattern) {
    return PatternMatches(entry.processPath, pattern) ||
           PatternMatches(entry.processName, pattern);
}

bool ParseProcessConditionPatterns(const std::wstring& rawValue,
                                   std::vector<std::wstring>* patterns,
                                   std::wstring* errorMessage) {
    patterns->clear();
    errorMessage->clear();

    if (IsDefaultSetting(rawValue)) {
        return true;
    }

    std::wstring value = rawValue;
    size_t start = 0;
    while (start < value.size()) {
        size_t delimiter = value.find_first_of(L",;\r\n", start);
        std::wstring token = TrimCopy(
            value.substr(start, delimiter == std::wstring::npos
                                    ? std::wstring::npos
                                    : delimiter - start));
        if (!token.empty()) {
            std::wstring resolved = ResolveMatchPattern(token);
            if (resolved.empty()) {
                *errorMessage =
                    L"Process condition contains an empty or unresolved pattern.";
                patterns->clear();
                return false;
            }

            patterns->push_back(std::move(resolved));
        }

        if (delimiter == std::wstring::npos) {
            break;
        }

        start = delimiter + 1;
    }

    if (patterns->empty()) {
        *errorMessage = L"Process condition didn't contain any usable patterns.";
        return false;
    }

    return true;
}

bool AnyOtherRunningProcessMatches(const std::vector<RunningProcessEntry>& entries,
                                   const std::vector<std::wstring>& patterns) {
    DWORD currentPid = GetCurrentProcessId();

    for (const RunningProcessEntry& entry : entries) {
        if (entry.pid == 0 || entry.pid == currentPid) {
            continue;
        }

        for (const std::wstring& pattern : patterns) {
            if (RunningProcessEntryMatchesPattern(entry, pattern)) {
                return true;
            }
        }
    }

    return false;
}

bool EvaluateRunningProcessConditions(
    const std::wstring& whenProcessesRunning,
    const std::wstring& whenProcessesNotRunning,
    std::wstring* errorMessage) {
    errorMessage->clear();

    if (IsDefaultSetting(whenProcessesRunning) &&
        IsDefaultSetting(whenProcessesNotRunning)) {
        return true;
    }

    std::vector<std::wstring> requiredPatterns;
    if (!ParseProcessConditionPatterns(whenProcessesRunning, &requiredPatterns,
                                       errorMessage)) {
        return false;
    }

    std::vector<std::wstring> forbiddenPatterns;
    if (!ParseProcessConditionPatterns(whenProcessesNotRunning, &forbiddenPatterns,
                                       errorMessage)) {
        return false;
    }

    std::vector<RunningProcessEntry> entries = EnumerateRunningProcesses();

    if (!requiredPatterns.empty() &&
        !AnyOtherRunningProcessMatches(entries, requiredPatterns)) {
        return false;
    }

    if (!forbiddenPatterns.empty() &&
        AnyOtherRunningProcessMatches(entries, forbiddenPatterns)) {
        return false;
    }

    return true;
}

std::wstring BuildProcessDisplayText(const RunningProcessEntry& entry) {
    std::wstring text = entry.processName;
    text.append(L"    PID ");
    text.append(std::to_wstring(entry.pid));
    text.append(L"    ");

    if (!entry.processPath.empty()) {
        text.append(entry.processPath);
    } else {
        text.append(L"[path unavailable]");
    }

    return text;
}

std::wstring GetPickerOutputMode() {
    std::wstring outputMode =
        ToLowerCopy(TrimCopy(GetStringSettingCopy(L"picker.output")));
    if (outputMode != L"exe-name") {
        outputMode = L"full-path";
    }

    return outputMode;
}

std::wstring GetPickedValueForEntry(const RunningProcessEntry& entry,
                                    const std::wstring& outputMode) {
    if (outputMode == L"exe-name") {
        return entry.processName;
    }

    return entry.processPath;
}

bool CopyTextToClipboard(HWND hwnd, const std::wstring& text) {
    if (!OpenClipboard(hwnd)) {
        Wh_Log(L"OpenClipboard failed: %lu", GetLastError());
        return false;
    }

    if (!EmptyClipboard()) {
        Wh_Log(L"EmptyClipboard failed: %lu", GetLastError());
        CloseClipboard();
        return false;
    }

    size_t bytes = (text.size() + 1) * sizeof(wchar_t);
    HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (!memory) {
        Wh_Log(L"GlobalAlloc failed for clipboard text.");
        CloseClipboard();
        return false;
    }

    void* lockedMemory = GlobalLock(memory);
    if (!lockedMemory) {
        Wh_Log(L"GlobalLock failed for clipboard text.");
        GlobalFree(memory);
        CloseClipboard();
        return false;
    }

    memcpy(lockedMemory, text.c_str(), bytes);
    GlobalUnlock(memory);

    if (!SetClipboardData(CF_UNICODETEXT, memory)) {
        Wh_Log(L"SetClipboardData failed: %lu", GetLastError());
        GlobalFree(memory);
        CloseClipboard();
        return false;
    }

    CloseClipboard();
    return true;
}

void ApplyDefaultGuiFont(HWND window) {
    SendMessageW(window, WM_SETFONT,
                 reinterpret_cast<WPARAM>(GetStockObject(DEFAULT_GUI_FONT)),
                 TRUE);
}

bool FinalizeProcessPickerSelection(HWND hwnd, ProcessPickerState* state) {
    LRESULT selectedIndex =
        SendMessageW(state->listBox, LB_GETCURSEL, 0, 0);
    if (selectedIndex == LB_ERR) {
        MessageBoxW(hwnd, L"Select a process first.", L"Process Picker",
                    MB_OK | MB_ICONINFORMATION);
        return false;
    }

    LRESULT itemData =
        SendMessageW(state->listBox, LB_GETITEMDATA, selectedIndex, 0);
    if (itemData == LB_ERR ||
        itemData < 0 ||
        static_cast<size_t>(itemData) >= state->entries.size()) {
        MessageBoxW(hwnd, L"The selected process entry is invalid.",
                    L"Process Picker", MB_OK | MB_ICONERROR);
        return false;
    }

    const RunningProcessEntry& entry =
        state->entries[static_cast<size_t>(itemData)];
    std::wstring pickedValue =
        GetPickedValueForEntry(entry, state->outputMode);

    if (pickedValue.empty()) {
        MessageBoxW(hwnd,
                    L"This process doesn't expose a full path to the picker. "
                    L"Choose another process or change the picker output to "
                    L"Executable name.",
                    L"Process Picker", MB_OK | MB_ICONWARNING);
        return false;
    }

    if (!Wh_SetStringValue(kPickedProcessValueName, pickedValue.c_str())) {
        MessageBoxW(hwnd, L"Failed to store the selected process.",
                    L"Process Picker", MB_OK | MB_ICONERROR);
        return false;
    }

    bool clipboardOk = CopyTextToClipboard(hwnd, pickedValue);
    Wh_Log(L"Stored running-process picker selection as @picked: %s",
           pickedValue.c_str());

    std::wstring message =
        L"Saved as @picked and copied to the clipboard:\n\n";
    message.append(pickedValue);
    if (!clipboardOk) {
        message.append(
            L"\n\nClipboard copy failed, but the value was still saved as "
            L"@picked.");
    }

    MessageBoxW(hwnd, message.c_str(), L"Process Picker",
                MB_OK | MB_ICONINFORMATION);
    DestroyWindow(hwnd);
    return true;
}

LRESULT CALLBACK ProcessPickerWindowProc(HWND hwnd, UINT message,
                                         WPARAM wParam, LPARAM lParam) {
    ProcessPickerState* state = reinterpret_cast<ProcessPickerState*>(
        GetWindowLongPtrW(hwnd, GWLP_USERDATA));

    switch (message) {
        case WM_NCCREATE: {
            CREATESTRUCTW* createStruct =
                reinterpret_cast<CREATESTRUCTW*>(lParam);
            SetWindowLongPtrW(
                hwnd, GWLP_USERDATA,
                reinterpret_cast<LONG_PTR>(createStruct->lpCreateParams));
            return TRUE;
        }

        case WM_CREATE: {
            state = reinterpret_cast<ProcessPickerState*>(
                GetWindowLongPtrW(hwnd, GWLP_USERDATA));

            std::wstring description =
                L"Double-click a running process or select one and click "
                L"Use Selection. The chosen value is copied to the clipboard "
                L"and saved as @picked.";
            HWND descriptionLabel = CreateWindowExW(
                0, L"STATIC", description.c_str(),
                WS_CHILD | WS_VISIBLE | SS_LEFT, kPickerPadding, kPickerPadding,
                kPickerWindowWidth - (kPickerPadding * 2), 36, hwnd, nullptr,
                GetModuleHandleW(nullptr), nullptr);
            ApplyDefaultGuiFont(descriptionLabel);

            state->listBox = CreateWindowExW(
                WS_EX_CLIENTEDGE, L"LISTBOX", nullptr,
                WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | LBS_NOTIFY |
                    LBS_HASSTRINGS | WS_TABSTOP,
                kPickerPadding, 56, kPickerWindowWidth - (kPickerPadding * 2),
                kPickerWindowHeight - 120, hwnd,
                reinterpret_cast<HMENU>(static_cast<INT_PTR>(kPickerListId)),
                GetModuleHandleW(nullptr), nullptr);
            ApplyDefaultGuiFont(state->listBox);

            HWND selectButton = CreateWindowExW(
                0, L"BUTTON", L"Use Selection",
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON,
                kPickerWindowWidth - (kPickerPadding * 2) - kPickerButtonWidth -
                    12 - kPickerButtonWidth,
                kPickerWindowHeight - 52, kPickerButtonWidth,
                kPickerButtonHeight, hwnd,
                reinterpret_cast<HMENU>(static_cast<INT_PTR>(kPickerSelectId)),
                GetModuleHandleW(nullptr), nullptr);
            ApplyDefaultGuiFont(selectButton);

            HWND cancelButton = CreateWindowExW(
                0, L"BUTTON", L"Cancel", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                kPickerWindowWidth - kPickerPadding - kPickerButtonWidth,
                kPickerWindowHeight - 52, kPickerButtonWidth,
                kPickerButtonHeight, hwnd,
                reinterpret_cast<HMENU>(static_cast<INT_PTR>(kPickerCancelId)),
                GetModuleHandleW(nullptr), nullptr);
            ApplyDefaultGuiFont(cancelButton);

            for (size_t i = 0; i < state->entries.size(); i++) {
                std::wstring displayText =
                    BuildProcessDisplayText(state->entries[i]);
                LRESULT listIndex = SendMessageW(
                    state->listBox, LB_ADDSTRING, 0,
                    reinterpret_cast<LPARAM>(displayText.c_str()));
                if (listIndex != LB_ERR && listIndex != LB_ERRSPACE) {
                    SendMessageW(state->listBox, LB_SETITEMDATA, listIndex, i);
                }
            }

            if (!state->entries.empty()) {
                SendMessageW(state->listBox, LB_SETCURSEL, 0, 0);
            }

            return 0;
        }

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case kPickerListId:
                    if (HIWORD(wParam) == LBN_DBLCLK && state) {
                        FinalizeProcessPickerSelection(hwnd, state);
                    }
                    return 0;

                case kPickerSelectId:
                    if (state) {
                        FinalizeProcessPickerSelection(hwnd, state);
                    }
                    return 0;

                case kPickerCancelId:
                    DestroyWindow(hwnd);
                    return 0;
            }
            break;

        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcW(hwnd, message, wParam, lParam);
}

DWORD WINAPI ProcessPickerThreadProc(LPVOID parameter) {
    std::unique_ptr<ProcessPickerState> state(
        reinterpret_cast<ProcessPickerState*>(parameter));

    state->entries = EnumerateRunningProcesses();
    if (state->entries.empty()) {
        MessageBoxW(nullptr, L"No running processes were found.",
                    L"Process Picker", MB_OK | MB_ICONINFORMATION);
        g_pickerOpen = false;
        return 0;
    }

    WNDCLASSEXW windowClass = {};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.lpfnWndProc = ProcessPickerWindowProc;
    windowClass.hInstance = GetModuleHandleW(nullptr);
    windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    windowClass.hbrBackground =
        reinterpret_cast<HBRUSH>(GetSysColorBrush(COLOR_WINDOW));
    windowClass.lpszClassName = kPickerWindowClassName;

    if (!RegisterClassExW(&windowClass) &&
        GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        MessageBoxW(nullptr, L"Failed to register the process picker window.",
                    L"Process Picker", MB_OK | MB_ICONERROR);
        g_pickerOpen = false;
        return 0;
    }

    HWND window = CreateWindowExW(
        WS_EX_DLGMODALFRAME, kPickerWindowClassName,
        L"Force Process Accelerators - Running Process Picker",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, kPickerWindowWidth, kPickerWindowHeight,
        nullptr, nullptr, GetModuleHandleW(nullptr), state.get());

    if (!window) {
        MessageBoxW(nullptr, L"Failed to create the process picker window.",
                    L"Process Picker", MB_OK | MB_ICONERROR);
        g_pickerOpen = false;
        return 0;
    }

    RECT windowRect = {};
    GetWindowRect(window, &windowRect);
    int width = windowRect.right - windowRect.left;
    int height = windowRect.bottom - windowRect.top;
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    SetWindowPos(window, nullptr, (screenWidth - width) / 2,
                 (screenHeight - height) / 2, 0, 0,
                 SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
    ShowWindow(window, SW_SHOW);
    UpdateWindow(window);
    SetForegroundWindow(window);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    g_pickerOpen = false;
    return 0;
}

void MaybeLaunchProcessPicker() {
    if (!g_isPickerHost) {
        return;
    }

    bool launch = Wh_GetIntSetting(L"picker.launch") != 0;
    if (!launch) {
        g_pickerLaunchArmed = false;
        return;
    }

    if (g_pickerLaunchArmed) {
        return;
    }

    g_pickerLaunchArmed = true;

    if (g_pickerOpen.exchange(true)) {
        return;
    }

    auto* state = new ProcessPickerState();
    state->outputMode = GetPickerOutputMode();

    HANDLE thread =
        CreateThread(nullptr, 0, ProcessPickerThreadProc, state, 0, nullptr);
    if (!thread) {
        Wh_Log(L"Failed to create the process picker thread: %lu",
               GetLastError());
        delete state;
        g_pickerOpen = false;
        return;
    }

    CloseHandle(thread);
}

const OrtApi* WINAPI OrtGetApi_Hook(uint32_t version) {
    if (!g_originalOrtGetApi) {
        return nullptr;
    }

    const OrtApi* api = g_originalOrtGetApi(version);
    if (!api) {
        return nullptr;
    }

    AcquireSRWLockExclusive(&g_ortHookLock);
    PatchOrtApiTableLocked(api);
    ReleaseSRWLockExclusive(&g_ortHookLock);

    return api;
}

const OrtApiBase* WINAPI OrtGetApiBase_Hook() {
    if (!g_originalOrtGetApiBase) {
        return nullptr;
    }

    const OrtApiBase* apiBase = g_originalOrtGetApiBase();
    AcquireSRWLockExclusive(&g_ortHookLock);
    PatchOrtApiBaseLocked(apiBase);
    ReleaseSRWLockExclusive(&g_ortHookLock);
    return apiBase;
}

std::wstring ConsumeOrtStatusMessage(OrtStatus* status,
                                     OrtGetErrorMessage_t getErrorMessage,
                                     OrtReleaseStatus_t releaseStatus) {
    if (!status) {
        return {};
    }

    std::wstring errorMessage;
    if (getErrorMessage) {
        errorMessage = Utf8ToWideCopy(getErrorMessage(status));
    }

    if (releaseStatus) {
        releaseStatus(status);
    }

    return errorMessage;
}

bool TryGetQnnPerformanceModeValue(const std::wstring& rawValue,
                                   const char** value) {
    *value = nullptr;

    std::wstring normalized = ToLowerCopy(TrimCopy(rawValue));
    if (normalized.empty() || normalized == L"unchanged") {
        return true;
    }

    if (normalized == L"default") {
        *value = "default";
    } else if (normalized == L"balanced") {
        *value = "balanced";
    } else if (normalized == L"burst") {
        *value = "burst";
    } else if (normalized == L"high-performance") {
        *value = "high_performance";
    } else if (normalized == L"sustained-high-performance") {
        *value = "sustained_high_performance";
    } else if (normalized == L"power-saver") {
        *value = "power_saver";
    } else if (normalized == L"high-power-saver") {
        *value = "high_power_saver";
    } else if (normalized == L"low-balanced") {
        *value = "low_balanced";
    } else if (normalized == L"low-power-saver") {
        *value = "low_power_saver";
    } else if (normalized == L"extreme-power-saver") {
        *value = "extreme_power_saver";
    } else {
        return false;
    }

    return true;
}

bool TryGetOpenVinoDeviceTypeValue(const RuleSettings& rule,
                                   const char** value) {
    *value = "NPU";

    std::wstring normalized = ToLowerCopy(TrimCopy(rule.npuOpenVinoDeviceType));
    if (normalized.empty() || normalized == L"unchanged" ||
        normalized == L"npu") {
        *value = "NPU";
        return true;
    }

    if (normalized == L"auto-npu-cpu") {
        *value = "AUTO:NPU,CPU";
        return true;
    }

    if (normalized == L"hetero-npu-cpu") {
        *value = "HETERO:NPU,CPU";
        return true;
    }

    if (normalized == L"multi-npu-cpu") {
        *value = "MULTI:NPU,CPU";
        return true;
    }

    return false;
}

std::wstring DescribeOnnxRuntimeNpuMode(const RuleSettings& rule) {
    switch (ParseNpuMode(rule.npuMode)) {
        case NpuMode::kOnnxRuntimePreferNpu:
            return L"prefer NPU";
        case NpuMode::kOnnxRuntimeQnn:
            return L"QNN HTP";
        case NpuMode::kOnnxRuntimeOpenVino:
            return L"OpenVINO NPU";
        case NpuMode::kUnchanged:
            break;
    }

    return L"unchanged";
}

bool ApplyOrtSessionConfigEntry(OrtAddSessionConfigEntry_t addSessionConfigEntry,
                                OrtSessionOptions* options,
                                const char* configKey,
                                const char* configValue,
                                OrtGetErrorMessage_t getErrorMessage,
                                OrtReleaseStatus_t releaseStatus,
                                std::wstring* errorMessage) {
    errorMessage->clear();

    if (!addSessionConfigEntry) {
        *errorMessage =
            L"ONNX Runtime's session config API isn't available in this build.";
        return false;
    }

    OrtStatus* status = addSessionConfigEntry(options, configKey, configValue);
    if (!status) {
        return true;
    }

    *errorMessage =
        ConsumeOrtStatusMessage(status, getErrorMessage, releaseStatus);
    if (errorMessage->empty()) {
        *errorMessage = L"unknown error";
    }

    return false;
}

bool AppendOrtExecutionProvider(
    OrtSessionOptionsAppendExecutionProvider_t appendExecutionProvider,
    OrtSessionOptions* options,
    const char* providerName,
    const std::vector<const char*>& providerOptionsKeys,
    const std::vector<const char*>& providerOptionsValues,
    OrtGetErrorMessage_t getErrorMessage,
    OrtReleaseStatus_t releaseStatus,
    std::wstring* errorMessage) {
    errorMessage->clear();

    if (!appendExecutionProvider) {
        *errorMessage =
            L"ONNX Runtime's generic provider append API isn't available in this build.";
        return false;
    }

    OrtStatus* status = appendExecutionProvider(
        options, providerName,
        providerOptionsKeys.empty() ? nullptr : providerOptionsKeys.data(),
        providerOptionsValues.empty() ? nullptr : providerOptionsValues.data(),
        providerOptionsKeys.size());
    if (!status) {
        return true;
    }

    *errorMessage =
        ConsumeOrtStatusMessage(status, getErrorMessage, releaseStatus);
    if (errorMessage->empty()) {
        *errorMessage = L"unknown error";
    }

    return false;
}

void ApplyOnnxRuntimeNpuSessionOptions(const RuleSettings& rule,
                                       OrtSessionOptions* options,
                                       OrtGetErrorMessage_t getErrorMessage,
                                       OrtReleaseStatus_t releaseStatus,
                                       OrtAddSessionConfigEntry_t addSessionConfigEntry,
                                       OrtSessionOptionsAppendExecutionProvider_t
                                           appendExecutionProvider,
                                       OrtSessionOptionsSetEpSelectionPolicy_t
                                           setEpSelectionPolicy) {
    NpuMode npuMode = ParseNpuMode(rule.npuMode);
    if ((npuMode == NpuMode::kUnchanged && !rule.npuDisableCpuFallback) ||
        !options) {
        return;
    }

    bool appliedPrimaryMode = false;
    bool appliedNoCpuFallback = false;

    if (rule.npuDisableCpuFallback) {
        std::wstring errorMessage;
        if (ApplyOrtSessionConfigEntry(addSessionConfigEntry, options,
                                       kOrtDisableCpuEpFallbackKey, "1",
                                       getErrorMessage, releaseStatus,
                                       &errorMessage)) {
            appliedNoCpuFallback = true;
        } else if (!g_loggedOrtNpuFailure.exchange(true)) {
            Wh_Log(L"Failed to disable ONNX Runtime CPU fallback in %s: %s",
                   g_processName.c_str(), errorMessage.c_str());
        }
    }

    if (npuMode == NpuMode::kOnnxRuntimePreferNpu) {
        if (!setEpSelectionPolicy) {
            if (!g_loggedOrtNpuUnsupported.exchange(true)) {
                Wh_Log(L"ONNX Runtime automatic NPU preference isn't available in %s. "
                       L"ORT 1.22+ is required.",
                       g_processName.c_str());
            }
        } else {
            OrtStatus* policyStatus = setEpSelectionPolicy(
                options, OrtExecutionProviderDevicePolicy_PREFER_NPU);
            if (!policyStatus) {
                appliedPrimaryMode = true;
            } else if (!g_loggedOrtNpuFailure.exchange(true)) {
                std::wstring errorMessage = ConsumeOrtStatusMessage(
                    policyStatus, getErrorMessage, releaseStatus);
                if (!errorMessage.empty()) {
                    Wh_Log(L"ONNX Runtime rejected the NPU preference in %s: %s",
                           g_processName.c_str(), errorMessage.c_str());
                } else {
                    Wh_Log(L"ONNX Runtime rejected the NPU preference in %s.",
                           g_processName.c_str());
                }
            }
        }
    } else if (npuMode == NpuMode::kOnnxRuntimeQnn) {
        std::vector<const char*> keys = {"backend_type"};
        std::vector<const char*> values = {"htp"};
        const char* qnnPerformanceMode = nullptr;
        if (!TryGetQnnPerformanceModeValue(rule.npuQnnPerformanceMode,
                                           &qnnPerformanceMode)) {
            if (!g_loggedOrtNpuFailure.exchange(true)) {
                Wh_Log(L"Invalid npuQnnPerformanceMode value '%s' in rule '%s'.",
                       rule.npuQnnPerformanceMode.c_str(), rule.match.c_str());
            }
        } else if (qnnPerformanceMode) {
            keys.push_back("htp_performance_mode");
            values.push_back(qnnPerformanceMode);
        }

        std::wstring errorMessage;
        if (AppendOrtExecutionProvider(appendExecutionProvider, options,
                                       kOrtQnnProviderName, keys, values,
                                       getErrorMessage, releaseStatus,
                                       &errorMessage)) {
            appliedPrimaryMode = true;
        } else if (!g_loggedOrtNpuUnsupported.exchange(true)) {
            Wh_Log(L"Failed to append the ONNX Runtime QNN HTP provider in %s: %s",
                   g_processName.c_str(), errorMessage.c_str());
        }
    } else if (npuMode == NpuMode::kOnnxRuntimeOpenVino) {
        const char* openVinoDeviceType = nullptr;
        if (!TryGetOpenVinoDeviceTypeValue(rule, &openVinoDeviceType)) {
            if (!g_loggedOrtNpuFailure.exchange(true)) {
                Wh_Log(L"Invalid npuOpenVinoDeviceType value '%s' in rule '%s'.",
                       rule.npuOpenVinoDeviceType.c_str(), rule.match.c_str());
            }
        } else {
            std::vector<const char*> keys = {"device_type"};
            std::vector<const char*> values = {openVinoDeviceType};
            if (rule.npuOpenVinoEnableFastCompile) {
                keys.push_back("enable_npu_fast_compile");
                values.push_back("1");
            }

            std::wstring errorMessage;
            if (AppendOrtExecutionProvider(appendExecutionProvider, options,
                                           kOrtOpenVinoProviderName, keys, values,
                                           getErrorMessage, releaseStatus,
                                           &errorMessage)) {
                appliedPrimaryMode = true;
            } else if (!g_loggedOrtNpuUnsupported.exchange(true)) {
                Wh_Log(L"Failed to append the ONNX Runtime OpenVINO provider in %s: %s",
                       g_processName.c_str(), errorMessage.c_str());
            }
        }
    }

    if ((appliedPrimaryMode || appliedNoCpuFallback) && g_logVerbose &&
        !g_loggedOrtNpuApplied.exchange(true)) {
        std::wstring summary = DescribeOnnxRuntimeNpuMode(rule);
        if (appliedPrimaryMode && appliedNoCpuFallback) {
            Wh_Log(L"Applied ONNX Runtime NPU mode '%s' with CPU fallback disabled "
                   L"for new sessions in %s.",
                   summary.c_str(), g_processName.c_str());
        } else if (appliedPrimaryMode) {
            Wh_Log(L"Applied ONNX Runtime NPU mode '%s' for new sessions in %s.",
                   summary.c_str(), g_processName.c_str());
        } else {
            Wh_Log(L"Applied ONNX Runtime CPU fallback disable for new sessions in %s.",
                   g_processName.c_str());
        }
    }
}

OrtStatus* WINAPI OrtCreateSessionOptions_Hook(OrtSessionOptions** options) {
    OrtCreateSessionOptions_t originalCreateSessionOptions = nullptr;
    OrtGetErrorMessage_t getErrorMessage = nullptr;
    OrtReleaseStatus_t releaseStatus = nullptr;
    OrtAddSessionConfigEntry_t addSessionConfigEntry = nullptr;
    OrtSessionOptionsAppendExecutionProvider_t appendExecutionProvider = nullptr;
    OrtSessionOptionsSetEpSelectionPolicy_t setEpSelectionPolicy = nullptr;

    AcquireSRWLockShared(&g_ortHookLock);
    originalCreateSessionOptions = g_originalOrtCreateSessionOptions;
    getErrorMessage = g_ortGetErrorMessage;
    releaseStatus = g_ortReleaseStatus;
    addSessionConfigEntry = g_ortAddSessionConfigEntry;
    appendExecutionProvider = g_ortAppendExecutionProvider;
    setEpSelectionPolicy = g_ortSetEpSelectionPolicy;
    ReleaseSRWLockShared(&g_ortHookLock);

    if (!originalCreateSessionOptions) {
        return nullptr;
    }

    OrtStatus* status = originalCreateSessionOptions(options);
    if (status || !options || !*options || !IsOnnxRuntimeNpuModeEnabled()) {
        return status;
    }
    ApplyOnnxRuntimeNpuSessionOptions(
        g_activeRule, *options, getErrorMessage, releaseStatus,
        addSessionConfigEntry, appendExecutionProvider, setEpSelectionPolicy);

    return status;
}

HMODULE WINAPI LoadLibraryW_Hook(LPCWSTR lpLibFileName) {
    HMODULE module = g_originalLoadLibraryW(lpLibFileName);
    if (module &&
        ((lpLibFileName && IsOnnxRuntimeModulePath(lpLibFileName)) ||
         IsOnnxRuntimeModule(module))) {
        EnsureOnnxRuntimeModuleHooked(module);
    }

    return module;
}

HMODULE WINAPI LoadLibraryA_Hook(LPCSTR lpLibFileName) {
    HMODULE module = g_originalLoadLibraryA(lpLibFileName);
    if (module) {
        std::wstring path = MultiByteToWideCopy(CP_ACP, lpLibFileName);
        if ((lpLibFileName && IsOnnxRuntimeModulePath(path)) ||
            IsOnnxRuntimeModule(module)) {
            EnsureOnnxRuntimeModuleHooked(module);
        }
    }

    return module;
}

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName, HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = g_originalLoadLibraryExW(lpLibFileName, hFile, dwFlags);
    if (module &&
        ((lpLibFileName && IsOnnxRuntimeModulePath(lpLibFileName)) ||
         IsOnnxRuntimeModule(module))) {
        EnsureOnnxRuntimeModuleHooked(module);
    }

    return module;
}

HMODULE WINAPI LoadLibraryExA_Hook(LPCSTR lpLibFileName, HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = g_originalLoadLibraryExA(lpLibFileName, hFile, dwFlags);
    if (module) {
        std::wstring path = MultiByteToWideCopy(CP_ACP, lpLibFileName);
        if ((lpLibFileName && IsOnnxRuntimeModulePath(path)) ||
            IsOnnxRuntimeModule(module)) {
            EnsureOnnxRuntimeModuleHooked(module);
        }
    }

    return module;
}

bool ReadCurrentGpuPreferenceValue(std::wstring* value, bool* existed) {
    value->clear();
    *existed = false;

    HKEY key = nullptr;
    LSTATUS status = RegOpenKeyExW(HKEY_CURRENT_USER, kGpuPreferencesKeyPath, 0,
                                   KEY_READ, &key);
    if (status == ERROR_FILE_NOT_FOUND) {
        return true;
    }

    if (status != ERROR_SUCCESS) {
        Wh_Log(L"Failed to open %s: %lu", kGpuPreferencesKeyPath, status);
        return false;
    }

    DWORD type = 0;
    DWORD bytes = 0;
    status = RegQueryValueExW(key, g_processPath.c_str(), nullptr, &type, nullptr,
                              &bytes);
    if (status == ERROR_FILE_NOT_FOUND) {
        RegCloseKey(key);
        return true;
    }

    if (status != ERROR_SUCCESS) {
        Wh_Log(L"Failed to query GPU preference for %s: %lu", g_processPath.c_str(),
               status);
        RegCloseKey(key);
        return false;
    }

    if (type != REG_SZ && type != REG_EXPAND_SZ) {
        Wh_Log(L"Unexpected registry type for GPU preference of %s: %lu",
               g_processPath.c_str(), type);
        RegCloseKey(key);
        return false;
    }

    std::wstring buffer(bytes / sizeof(wchar_t), L'\0');
    status = RegQueryValueExW(key, g_processPath.c_str(), nullptr, &type,
                              reinterpret_cast<LPBYTE>(buffer.data()), &bytes);
    RegCloseKey(key);

    if (status != ERROR_SUCCESS) {
        Wh_Log(L"Failed to read GPU preference for %s: %lu", g_processPath.c_str(),
               status);
        return false;
    }

    if (!buffer.empty() && buffer.back() == L'\0') {
        buffer.pop_back();
    }

    *existed = true;
    *value = std::move(buffer);
    return true;
}

bool WriteCurrentGpuPreferenceValue(const std::optional<std::wstring>& value) {
    HKEY key = nullptr;
    LSTATUS status = RegCreateKeyExW(HKEY_CURRENT_USER, kGpuPreferencesKeyPath, 0,
                                     nullptr, 0, KEY_READ | KEY_WRITE, nullptr,
                                     &key, nullptr);
    if (status != ERROR_SUCCESS) {
        Wh_Log(L"Failed to open or create %s: %lu", kGpuPreferencesKeyPath,
               status);
        return false;
    }

    if (!value.has_value()) {
        status = RegDeleteValueW(key, g_processPath.c_str());
        RegCloseKey(key);

        if (status != ERROR_SUCCESS && status != ERROR_FILE_NOT_FOUND) {
            Wh_Log(L"Failed to delete GPU preference for %s: %lu",
                   g_processPath.c_str(), status);
            return false;
        }

        return true;
    }

    const std::wstring& data = value.value();
    status = RegSetValueExW(
        key, g_processPath.c_str(), 0, REG_SZ,
        reinterpret_cast<const BYTE*>(data.c_str()),
        static_cast<DWORD>((data.size() + 1) * sizeof(wchar_t)));
    RegCloseKey(key);

    if (status != ERROR_SUCCESS) {
        Wh_Log(L"Failed to write GPU preference for %s: %lu",
               g_processPath.c_str(), status);
        return false;
    }

    return true;
}

std::wstring BuildGraphicsFeatureRegistryValue(
    const std::wstring& existingValue, GpuPreference gpuPreference,
    ToggleMode autoHdr, ToggleMode windowOptimizations,
    ToggleMode variableRefreshRate) {
    std::vector<std::wstring> parts;
    size_t start = 0;

    while (start <= existingValue.size()) {
        size_t end = existingValue.find(L';', start);
        std::wstring part = TrimCopy(
            existingValue.substr(start, end == std::wstring::npos
                                            ? std::wstring::npos
                                            : end - start));
        if (!part.empty() && !StartsWithIgnoreCase(part, L"GpuPreference=") &&
            !StartsWithIgnoreCase(part, L"AutoHDREnable=") &&
            !StartsWithIgnoreCase(part, L"SwapEffectUpgradeEnable=") &&
            !StartsWithIgnoreCase(part, L"VRROptimizeEnable=")) {
            parts.push_back(part);
        }

        if (end == std::wstring::npos) {
            break;
        }

        start = end + 1;
    }

    switch (gpuPreference) {
        case GpuPreference::kPowerSaving:
            parts.push_back(L"GpuPreference=1");
            break;
        case GpuPreference::kHighPerformance:
            parts.push_back(L"GpuPreference=2");
            break;
        case GpuPreference::kSystemDefault:
            parts.push_back(L"GpuPreference=0");
            break;
        case GpuPreference::kUnchanged:
            break;
    }

    if (autoHdr == ToggleMode::kEnable) {
        parts.push_back(L"AutoHDREnable=1");
    } else if (autoHdr == ToggleMode::kDisable) {
        parts.push_back(L"AutoHDREnable=0");
    }

    if (windowOptimizations == ToggleMode::kEnable) {
        parts.push_back(L"SwapEffectUpgradeEnable=1");
    } else if (windowOptimizations == ToggleMode::kDisable) {
        parts.push_back(L"SwapEffectUpgradeEnable=0");
    }

    if (variableRefreshRate == ToggleMode::kEnable) {
        parts.push_back(L"VRROptimizeEnable=1");
    } else if (variableRefreshRate == ToggleMode::kDisable) {
        parts.push_back(L"VRROptimizeEnable=0");
    }

    std::wstring result;
    for (const std::wstring& part : parts) {
        result.append(part);
        result.push_back(L';');
    }

    return result;
}

ToggleMode ParseToggleMode(const std::wstring& rawValue) {
    std::wstring value = ToLowerCopy(TrimCopy(rawValue));

    if (value == L"enable") {
        return ToggleMode::kEnable;
    }

    if (value == L"disable") {
        return ToggleMode::kDisable;
    }

    return ToggleMode::kUnchanged;
}

GpuPreference ParseGpuPreference(const std::wstring& rawValue) {
    std::wstring value = ToLowerCopy(TrimCopy(rawValue));

    if (value.empty() || value == L"unchanged") {
        return GpuPreference::kUnchanged;
    }

    if (value == L"system-default") {
        return GpuPreference::kSystemDefault;
    }

    if (value == L"power-saving") {
        return GpuPreference::kPowerSaving;
    }

    if (value == L"high-performance") {
        return GpuPreference::kHighPerformance;
    }

    return GpuPreference::kUnchanged;
}

CpuPowerMode ParseCpuPowerMode(const std::wstring& rawValue) {
    std::wstring value = ToLowerCopy(TrimCopy(rawValue));

    if (value.empty() || value == L"unchanged") {
        return CpuPowerMode::kUnchanged;
    }

    if (value == L"disable-throttling") {
        return CpuPowerMode::kDisableThrottling;
    }

    if (value == L"enable-throttling") {
        return CpuPowerMode::kEnableThrottling;
    }

    if (value == L"eco-qos-full") {
        return CpuPowerMode::kEcoQosFull;
    }

    return CpuPowerMode::kUnchanged;
}

DWORD ParsePriorityClass(const std::wstring& rawValue) {
    std::wstring value = ToLowerCopy(TrimCopy(rawValue));

    if (value.empty() || value == L"unchanged") {
        return 0;
    }

    if (value == L"idle") {
        return IDLE_PRIORITY_CLASS;
    }

    if (value == L"below-normal") {
        return BELOW_NORMAL_PRIORITY_CLASS;
    }

    if (value == L"normal") {
        return NORMAL_PRIORITY_CLASS;
    }

    if (value == L"above-normal") {
        return ABOVE_NORMAL_PRIORITY_CLASS;
    }

    if (value == L"high") {
        return HIGH_PRIORITY_CLASS;
    }

    if (value == L"realtime") {
        return REALTIME_PRIORITY_CLASS;
    }

    return 0;
}

bool ParseUnsignedLong(const std::wstring& rawValue, unsigned long* value) {
    errno = 0;
    wchar_t* end = nullptr;
    unsigned long parsed = wcstoul(rawValue.c_str(), &end, 10);
    if (errno != 0 || end == rawValue.c_str() || *end != L'\0') {
        return false;
    }

    *value = parsed;
    return true;
}

bool ParseOptionalUnsignedLongSetting(const std::wstring& rawValue,
                                      std::optional<ULONG>* value) {
    std::wstring trimmedValue = TrimCopy(rawValue);
    if (trimmedValue.empty() ||
        EqualsIgnoreCase(trimmedValue, L"unchanged")) {
        *value = std::nullopt;
        return true;
    }

    unsigned long parsed = 0;
    if (!ParseUnsignedLong(trimmedValue, &parsed)) {
        return false;
    }

    *value = parsed;
    return true;
}

bool ParseCpuSetIdList(const std::wstring& rawValue, std::vector<ULONG>* ids) {
    ids->clear();
    std::wstring value = ToLowerCopy(TrimCopy(rawValue));
    if (value.empty()) {
        return false;
    }

    size_t start = 0;
    while (start < value.size()) {
        size_t comma = value.find(L',', start);
        std::wstring token = TrimCopy(
            value.substr(start, comma == std::wstring::npos ? std::wstring::npos
                                                            : comma - start));
        if (token.empty()) {
            return false;
        }

        size_t dash = token.find(L'-');
        unsigned long first = 0;
        unsigned long last = 0;
        if (dash == std::wstring::npos) {
            if (!ParseUnsignedLong(token, &first)) {
                return false;
            }
            last = first;
        } else {
            std::wstring left = TrimCopy(token.substr(0, dash));
            std::wstring right = TrimCopy(token.substr(dash + 1));
            if (!ParseUnsignedLong(left, &first) ||
                !ParseUnsignedLong(right, &last) || last < first) {
                return false;
            }
        }

        for (unsigned long id = first; id <= last; id++) {
            if (std::find(ids->begin(), ids->end(), id) == ids->end()) {
                ids->push_back(id);
            }
        }

        if (comma == std::wstring::npos) {
            break;
        }

        start = comma + 1;
    }

    return !ids->empty();
}

bool ParseCpuSetSelection(const std::wstring& rawValue,
                          ParsedCpuSetSelection* selection) {
    selection->mode = CpuSetSelectionMode::kUnchanged;
    selection->explicitIds.clear();

    std::wstring value = ToLowerCopy(TrimCopy(rawValue));
    if (value.empty() || value == L"unchanged") {
        return true;
    }

    if (value == L"all") {
        selection->mode = CpuSetSelectionMode::kAll;
        return true;
    }

    if (value == L"performance") {
        selection->mode = CpuSetSelectionMode::kPerformance;
        return true;
    }

    if (value == L"efficiency") {
        selection->mode = CpuSetSelectionMode::kEfficiency;
        return true;
    }

    if (!ParseCpuSetIdList(value, &selection->explicitIds)) {
        return false;
    }

    selection->mode = CpuSetSelectionMode::kExplicitIds;
    return true;
}

unsigned long long GetCpuSetCoreKey(const CpuSetInfo& cpuSetInfo) {
    return (static_cast<unsigned long long>(cpuSetInfo.group) << 32) |
           cpuSetInfo.coreIndex;
}

unsigned long long GetCpuSetDomainKey(const CpuSetInfo& cpuSetInfo) {
    return (static_cast<unsigned long long>(cpuSetInfo.group) << 32) |
           (static_cast<unsigned long long>(cpuSetInfo.numaNodeIndex) << 16) |
           cpuSetInfo.lastLevelCacheIndex;
}

bool HasCpuSetConfiguration(const RuleSettings& rule) {
    std::wstring cpuSets = TrimCopy(rule.cpuSets);
    std::wstring cpuSetNumaNode = TrimCopy(rule.cpuSetNumaNode);
    std::wstring cpuSetLastLevelCache = TrimCopy(rule.cpuSetLastLevelCache);
    std::wstring cpuSetLimit = TrimCopy(rule.cpuSetLimit);

    auto isConfigured = [](const std::wstring& value) {
        return !value.empty() && !EqualsIgnoreCase(value, L"unchanged");
    };

    return isConfigured(cpuSets) || isConfigured(cpuSetNumaNode) ||
           isConfigured(cpuSetLastLevelCache) || isConfigured(cpuSetLimit) ||
           rule.cpuSetAvoidSmt;
}

bool EnumerateCpuSets(std::vector<CpuSetInfo>* cpuSets) {
    cpuSets->clear();

    if (!g_getSystemCpuSetInformation) {
        return false;
    }

    ULONG returnedLength = 0;
    if (!g_getSystemCpuSetInformation(nullptr, 0, &returnedLength,
                                      GetCurrentProcess(), 0)) {
        DWORD error = GetLastError();
        if (error != ERROR_INSUFFICIENT_BUFFER || returnedLength == 0) {
            return false;
        }
    }

    if (returnedLength == 0) {
        return true;
    }

    std::vector<BYTE> buffer(returnedLength);
    if (!g_getSystemCpuSetInformation(
            reinterpret_cast<PSYSTEM_CPU_SET_INFORMATION>(buffer.data()),
            returnedLength, &returnedLength, GetCurrentProcess(), 0)) {
        return false;
    }

    ULONG offset = 0;
    while (offset + sizeof(SYSTEM_CPU_SET_INFORMATION) <= returnedLength) {
        auto* info = reinterpret_cast<PSYSTEM_CPU_SET_INFORMATION>(
            buffer.data() + offset);
        if (info->Size == 0) {
            break;
        }

        if (info->Type == CpuSetInformation) {
            CpuSetInfo cpuSetInfo;
            cpuSetInfo.id = info->CpuSet.Id;
            cpuSetInfo.group = info->CpuSet.Group;
            cpuSetInfo.logicalProcessorIndex =
                info->CpuSet.LogicalProcessorIndex;
            cpuSetInfo.coreIndex = info->CpuSet.CoreIndex;
            cpuSetInfo.lastLevelCacheIndex = info->CpuSet.LastLevelCacheIndex;
            cpuSetInfo.numaNodeIndex = info->CpuSet.NumaNodeIndex;
            cpuSetInfo.efficiencyClass = info->CpuSet.EfficiencyClass;
            cpuSetInfo.parked = info->CpuSet.Parked != 0;
            cpuSetInfo.allocated = info->CpuSet.Allocated != 0;
            cpuSetInfo.allocatedToTargetProcess =
                info->CpuSet.AllocatedToTargetProcess != 0;
            cpuSets->push_back(cpuSetInfo);
        }

        offset += info->Size;
    }

    std::sort(cpuSets->begin(), cpuSets->end(),
              [](const CpuSetInfo& left, const CpuSetInfo& right) {
                  if (left.group != right.group) {
                      return left.group < right.group;
                  }
                  if (left.numaNodeIndex != right.numaNodeIndex) {
                      return left.numaNodeIndex < right.numaNodeIndex;
                  }
                  if (left.lastLevelCacheIndex != right.lastLevelCacheIndex) {
                      return left.lastLevelCacheIndex <
                             right.lastLevelCacheIndex;
                  }
                  if (left.coreIndex != right.coreIndex) {
                      return left.coreIndex < right.coreIndex;
                  }
                  if (left.logicalProcessorIndex != right.logicalProcessorIndex) {
                      return left.logicalProcessorIndex <
                             right.logicalProcessorIndex;
                  }
                  return left.id < right.id;
              });

    return true;
}

bool QueryCurrentProcessDefaultCpuSets(std::vector<ULONG>* ids) {
    ids->clear();

    if (!g_getProcessDefaultCpuSets) {
        return false;
    }

    ULONG requiredCount = 0;
    if (!g_getProcessDefaultCpuSets(GetCurrentProcess(), nullptr, 0,
                                    &requiredCount)) {
        DWORD error = GetLastError();
        if (error != ERROR_INSUFFICIENT_BUFFER || requiredCount == 0) {
            return false;
        }
    }

    if (requiredCount == 0) {
        return true;
    }

    ids->resize(requiredCount);
    if (!g_getProcessDefaultCpuSets(GetCurrentProcess(), ids->data(),
                                    static_cast<ULONG>(ids->size()),
                                    &requiredCount)) {
        ids->clear();
        return false;
    }

    ids->resize(requiredCount);
    return true;
}

bool ApplyProcessDefaultCpuSets(const std::vector<ULONG>& ids) {
    if (!g_setProcessDefaultCpuSets) {
        return false;
    }

    return g_setProcessDefaultCpuSets(
               GetCurrentProcess(), ids.empty() ? nullptr : ids.data(),
               static_cast<ULONG>(ids.size())) != FALSE;
}

std::vector<std::vector<CpuSetInfo>> GroupCpuSetsByDomain(
    const std::vector<CpuSetInfo>& cpuSets) {
    std::vector<std::vector<CpuSetInfo>> groups;

    for (const CpuSetInfo& cpuSetInfo : cpuSets) {
        if (groups.empty() ||
            GetCpuSetDomainKey(groups.back().front()) !=
                GetCpuSetDomainKey(cpuSetInfo)) {
            groups.push_back({});
        }

        groups.back().push_back(cpuSetInfo);
    }

    return groups;
}

bool ApplyCpuSetPlacementLimit(std::vector<CpuSetInfo>* cpuSets,
                               ULONG limit,
                               CpuSetPlacementMode placementMode) {
    if (limit == 0) {
        cpuSets->clear();
        return false;
    }

    if (cpuSets->size() <= limit) {
        return true;
    }

    if (placementMode == CpuSetPlacementMode::kUnchanged) {
        cpuSets->resize(limit);
        return true;
    }

    std::vector<std::vector<CpuSetInfo>> domainGroups =
        GroupCpuSetsByDomain(*cpuSets);
    std::vector<CpuSetInfo> limitedCpuSets;
    limitedCpuSets.reserve(limit);

    if (placementMode == CpuSetPlacementMode::kCompact) {
        std::sort(domainGroups.begin(), domainGroups.end(),
                  [](const std::vector<CpuSetInfo>& left,
                     const std::vector<CpuSetInfo>& right) {
                      if (left.size() != right.size()) {
                          return left.size() > right.size();
                      }

                      return GetCpuSetDomainKey(left.front()) <
                             GetCpuSetDomainKey(right.front());
                  });

        for (const auto& domainGroup : domainGroups) {
            for (const CpuSetInfo& cpuSetInfo : domainGroup) {
                limitedCpuSets.push_back(cpuSetInfo);
                if (limitedCpuSets.size() == limit) {
                    *cpuSets = std::move(limitedCpuSets);
                    return true;
                }
            }
        }
    } else if (placementMode == CpuSetPlacementMode::kSpread) {
        std::vector<size_t> domainIndexes(domainGroups.size(), 0);
        while (limitedCpuSets.size() < limit) {
            bool progress = false;
            for (size_t i = 0; i < domainGroups.size(); i++) {
                if (domainIndexes[i] >= domainGroups[i].size()) {
                    continue;
                }

                limitedCpuSets.push_back(domainGroups[i][domainIndexes[i]++]);
                progress = true;

                if (limitedCpuSets.size() == limit) {
                    *cpuSets = std::move(limitedCpuSets);
                    return true;
                }
            }

            if (!progress) {
                break;
            }
        }
    }

    *cpuSets = std::move(limitedCpuSets);
    return cpuSets->size() == limit;
}

bool BuildSelectedCpuSetIds(const RuleSettings& rule,
                            std::vector<ULONG>* selectedIds,
                            std::wstring* errorMessage) {
    selectedIds->clear();
    errorMessage->clear();

    ParsedCpuSetSelection selection;
    if (!ParseCpuSetSelection(rule.cpuSets, &selection)) {
        *errorMessage = L"Invalid cpuSets value.";
        return false;
    }

    if (selection.mode == CpuSetSelectionMode::kUnchanged &&
        !HasCpuSetConfiguration(rule)) {
        return true;
    }

    if (selection.mode == CpuSetSelectionMode::kUnchanged) {
        selection.mode = CpuSetSelectionMode::kAll;
    }

    std::optional<ULONG> numaNode;
    if (!ParseOptionalUnsignedLongSetting(rule.cpuSetNumaNode, &numaNode)) {
        *errorMessage = L"Invalid cpuSetNumaNode value.";
        return false;
    }

    std::optional<ULONG> lastLevelCache;
    if (!ParseOptionalUnsignedLongSetting(rule.cpuSetLastLevelCache,
                                          &lastLevelCache)) {
        *errorMessage = L"Invalid cpuSetLastLevelCache value.";
        return false;
    }

    std::optional<ULONG> cpuSetLimit;
    if (!ParseOptionalUnsignedLongSetting(rule.cpuSetLimit, &cpuSetLimit)) {
        *errorMessage = L"Invalid cpuSetLimit value.";
        return false;
    }

    if (cpuSetLimit.has_value() && cpuSetLimit.value() == 0) {
        *errorMessage = L"cpuSetLimit must be greater than zero.";
        return false;
    }

    CpuSetPlacementMode placementMode =
        ParseCpuSetPlacementMode(rule.cpuSetPlacement);
    if (placementMode == CpuSetPlacementMode::kUnchanged) {
        std::wstring trimmedCpuSetPlacement = TrimCopy(rule.cpuSetPlacement);
        if (!trimmedCpuSetPlacement.empty() &&
            !EqualsIgnoreCase(trimmedCpuSetPlacement, L"unchanged")) {
            *errorMessage = L"Invalid cpuSetPlacement value.";
            return false;
        }
    }

    std::vector<CpuSetInfo> cpuSets;
    if (!EnumerateCpuSets(&cpuSets)) {
        *errorMessage =
            L"GetSystemCpuSetInformation isn't available or failed.";
        return false;
    }

    std::vector<CpuSetInfo> filteredCpuSets;
    for (const CpuSetInfo& cpuSetInfo : cpuSets) {
        if (cpuSetInfo.parked) {
            continue;
        }

        if (cpuSetInfo.allocated && !cpuSetInfo.allocatedToTargetProcess) {
            continue;
        }

        if (numaNode.has_value() && cpuSetInfo.numaNodeIndex != numaNode.value()) {
            continue;
        }

        if (lastLevelCache.has_value() &&
            cpuSetInfo.lastLevelCacheIndex != lastLevelCache.value()) {
            continue;
        }

        filteredCpuSets.push_back(cpuSetInfo);
    }

    if (filteredCpuSets.empty()) {
        *errorMessage = L"No usable CPU Sets matched the requested filters.";
        return false;
    }

    if (selection.mode == CpuSetSelectionMode::kPerformance ||
        selection.mode == CpuSetSelectionMode::kEfficiency) {
        BYTE targetEfficiencyClass = filteredCpuSets.front().efficiencyClass;
        for (const CpuSetInfo& cpuSetInfo : filteredCpuSets) {
            if (selection.mode == CpuSetSelectionMode::kPerformance) {
                targetEfficiencyClass = std::max(targetEfficiencyClass,
                                                 cpuSetInfo.efficiencyClass);
            } else {
                targetEfficiencyClass = std::min(targetEfficiencyClass,
                                                 cpuSetInfo.efficiencyClass);
            }
        }

        filteredCpuSets.erase(
            std::remove_if(filteredCpuSets.begin(), filteredCpuSets.end(),
                           [targetEfficiencyClass](const CpuSetInfo& cpuSetInfo) {
                               return cpuSetInfo.efficiencyClass !=
                                      targetEfficiencyClass;
                           }),
            filteredCpuSets.end());
    } else if (selection.mode == CpuSetSelectionMode::kExplicitIds) {
        std::vector<CpuSetInfo> explicitCpuSets;
        for (ULONG requestedId : selection.explicitIds) {
            auto it = std::find_if(filteredCpuSets.begin(), filteredCpuSets.end(),
                                   [requestedId](const CpuSetInfo& cpuSetInfo) {
                                       return cpuSetInfo.id == requestedId;
                                   });
            if (it == filteredCpuSets.end()) {
                *errorMessage = L"One or more requested CPU Set IDs weren't found.";
                return false;
            }

            explicitCpuSets.push_back(*it);
        }

        filteredCpuSets = std::move(explicitCpuSets);
    }

    if (rule.cpuSetAvoidSmt) {
        std::vector<CpuSetInfo> onePerCoreCpuSets;
        std::vector<unsigned long long> seenCoreKeys;
        for (const CpuSetInfo& cpuSetInfo : filteredCpuSets) {
            unsigned long long key = GetCpuSetCoreKey(cpuSetInfo);
            if (std::find(seenCoreKeys.begin(), seenCoreKeys.end(), key) !=
                seenCoreKeys.end()) {
                continue;
            }

            seenCoreKeys.push_back(key);
            onePerCoreCpuSets.push_back(cpuSetInfo);
        }

        filteredCpuSets = std::move(onePerCoreCpuSets);
    }

    if (filteredCpuSets.empty()) {
        *errorMessage = L"No CPU Sets remained after applying the requested policy.";
        return false;
    }

    if (cpuSetLimit.has_value() &&
        !ApplyCpuSetPlacementLimit(&filteredCpuSets, cpuSetLimit.value(),
                                   placementMode)) {
        *errorMessage =
            L"Failed to apply the requested CPU Set limit and placement.";
        return false;
    }

    for (const CpuSetInfo& cpuSetInfo : filteredCpuSets) {
        selectedIds->push_back(cpuSetInfo.id);
    }

    return true;
}

bool RuleLooksPerformanceOriented(const RuleSettings& rule) {
    std::wstring cpuPriority = ToLowerCopy(TrimCopy(rule.cpuPriority));
    std::wstring cpuSets = ToLowerCopy(TrimCopy(rule.cpuSets));
    std::wstring gpuPreference = ToLowerCopy(TrimCopy(rule.gpuPreference));
    std::wstring cpuPowerMode = ToLowerCopy(TrimCopy(rule.cpuPowerMode));

    return cpuPriority == L"above-normal" || cpuPriority == L"high" ||
           cpuPriority == L"realtime" || cpuSets == L"performance" ||
           gpuPreference == L"high-performance" ||
           cpuPowerMode == L"disable-throttling";
}

bool ParseCpuCoreMask(const std::wstring& rawValue,
                      DWORD_PTR systemAffinityMask,
                      DWORD_PTR* affinityMask) {
    std::wstring value = ToLowerCopy(TrimCopy(rawValue));
    if (value.empty() || value == L"unchanged") {
        return false;
    }

    if (value == L"all") {
        *affinityMask = systemAffinityMask;
        return systemAffinityMask != 0;
    }

    constexpr size_t kBitCount = sizeof(DWORD_PTR) * 8;
    DWORD_PTR parsedMask = 0;

    size_t start = 0;
    while (start < value.size()) {
        size_t comma = value.find(L',', start);
        std::wstring token = TrimCopy(
            value.substr(start, comma == std::wstring::npos ? std::wstring::npos
                                                            : comma - start));
        if (token.empty()) {
            return false;
        }

        size_t dash = token.find(L'-');
        unsigned long first = 0;
        unsigned long last = 0;

        if (dash == std::wstring::npos) {
            if (!ParseUnsignedLong(token, &first)) {
                return false;
            }
            last = first;
        } else {
            std::wstring left = TrimCopy(token.substr(0, dash));
            std::wstring right = TrimCopy(token.substr(dash + 1));
            if (!ParseUnsignedLong(left, &first) ||
                !ParseUnsignedLong(right, &last) || last < first) {
                return false;
            }
        }

        if (last >= kBitCount) {
            return false;
        }

        for (unsigned long index = first; index <= last; index++) {
            parsedMask |= (static_cast<DWORD_PTR>(1) << index);
        }

        if (comma == std::wstring::npos) {
            break;
        }

        start = comma + 1;
    }

    parsedMask &= systemAffinityMask;
    if (parsedMask == 0) {
        return false;
    }

    *affinityMask = parsedMask;
    return true;
}

bool QueryPowerThrottlingState(PROCESS_POWER_THROTTLING_STATE* state) {
    if (!g_getProcessInformation) {
        return false;
    }

    state->Version = PROCESS_POWER_THROTTLING_CURRENT_VERSION;
    return g_getProcessInformation(GetCurrentProcess(),
                                   kProcessPowerThrottlingInfoClass, state,
                                   sizeof(*state)) != FALSE;
}

bool ApplyPowerThrottlingMode(CpuPowerMode cpuPowerMode) {
    if (!g_setProcessInformation) {
        Wh_Log(L"SetProcessInformation(ProcessPowerThrottling) isn't available.");
        return false;
    }

    PROCESS_POWER_THROTTLING_STATE state = {};
    state.Version = PROCESS_POWER_THROTTLING_CURRENT_VERSION;
    state.ControlMask = PROCESS_POWER_THROTTLING_EXECUTION_SPEED;

    switch (cpuPowerMode) {
        case CpuPowerMode::kDisableThrottling:
            state.StateMask = 0;
            break;
        case CpuPowerMode::kEnableThrottling:
            state.StateMask = PROCESS_POWER_THROTTLING_EXECUTION_SPEED;
            break;
        case CpuPowerMode::kUnchanged:
            return true;
    }

    if (!g_setProcessInformation(GetCurrentProcess(),
                                 kProcessPowerThrottlingInfoClass, &state,
                                 sizeof(state))) {
        Wh_Log(L"SetProcessInformation(ProcessPowerThrottling) failed: %lu",
               GetLastError());
        return false;
    }

    return true;
}

void CaptureOriginalState() {
    if (g_originalState.captured) {
        return;
    }

    g_originalState.captured = true;

    DWORD priorityClass = GetPriorityClass(GetCurrentProcess());
    if (priorityClass != 0) {
        g_originalState.priorityValid = true;
        g_originalState.priorityClass = priorityClass;
    }

    BOOL disablePriorityBoost = FALSE;
    if (GetProcessPriorityBoost(GetCurrentProcess(), &disablePriorityBoost)) {
        g_originalState.priorityBoostValid = true;
        g_originalState.disablePriorityBoost = disablePriorityBoost;
    }

    g_originalState.cpuSetsValid =
        QueryCurrentProcessDefaultCpuSets(&g_originalState.processDefaultCpuSets);

    DWORD_PTR processAffinityMask = 0;
    DWORD_PTR systemAffinityMask = 0;
    if (GetProcessAffinityMask(GetCurrentProcess(), &processAffinityMask,
                               &systemAffinityMask)) {
        g_originalState.affinityValid = true;
        g_originalState.processAffinityMask = processAffinityMask;
        g_originalState.systemAffinityMask = systemAffinityMask;
    }

    g_originalState.powerThrottlingValid =
        QueryPowerThrottlingState(&g_originalState.powerThrottlingState);

    if (g_ntQueryInformationProcess) {
        if (g_ntQueryInformationProcess(GetCurrentProcess(), ProcessIoPriorityClass,
                                        &g_originalState.ioPriority, sizeof(ULONG),
                                        nullptr) == STATUS_SUCCESS) {
            g_originalState.ioPriorityValid = true;
        }

        if (g_ntQueryInformationProcess(GetCurrentProcess(), ProcessPagePriorityClass,
                                        &g_originalState.pagePriority, sizeof(ULONG),
                                        nullptr) == STATUS_SUCCESS) {
            g_originalState.pagePriorityValid = true;
        }
    }

    if (g_ntQueryTimerResolution) {
        ULONG minRes = 0, maxRes = 0, curRes = 0;
        if (g_ntQueryTimerResolution(&minRes, &maxRes, &curRes) == STATUS_SUCCESS) {
            g_originalState.timerResolutionValid = true;
            g_originalState.timerResolutionOriginal = curRes;
        }
    }

    if (g_getProcessWorkingSetSizeEx) {
        if (g_getProcessWorkingSetSizeEx(GetCurrentProcess(),
                                         &g_originalState.workingSetMin,
                                         &g_originalState.workingSetMax,
                                         &g_originalState.workingSetFlags)) {
            g_originalState.workingSetValid = true;
        }
    }

    if (g_getProcessInformation) {
        g_originalState.memoryPriorityInfo = {};
        g_originalState.memoryPriorityValid =
            g_getProcessInformation(GetCurrentProcess(), ProcessMemoryPriority,
                                    &g_originalState.memoryPriorityInfo,
                                    sizeof(g_originalState.memoryPriorityInfo)) !=
            FALSE;
    }

    g_originalState.gpuPreferenceValid = ReadCurrentGpuPreferenceValue(
        &g_originalState.gpuPreferenceValue,
        &g_originalState.gpuPreferenceExisted);
}

void RestoreOriginalState() {
    if (!g_originalState.captured) {
        return;
    }

    if (g_originalState.cpuSetsValid && g_setProcessDefaultCpuSets) {
        if (!ApplyProcessDefaultCpuSets(g_originalState.processDefaultCpuSets)) {
            Wh_Log(L"Failed to restore CPU Sets for %s: %lu",
                   g_processName.c_str(), GetLastError());
        } else if (g_logVerbose) {
            Wh_Log(L"Restored original CPU Sets for %s.", g_processName.c_str());
        }
    }

    if (g_originalState.affinityValid) {
        if (!SetProcessAffinityMask(GetCurrentProcess(),
                                    g_originalState.processAffinityMask)) {
            Wh_Log(L"Failed to restore process affinity for %s: %lu",
                   g_processName.c_str(), GetLastError());
        } else if (g_logVerbose) {
            Wh_Log(L"Restored original CPU affinity for %s.", g_processName.c_str());
        }
    }

    if (g_originalState.priorityValid) {
        if (!SetPriorityClass(GetCurrentProcess(), g_originalState.priorityClass)) {
            Wh_Log(L"Failed to restore priority class for %s: %lu",
                   g_processName.c_str(), GetLastError());
        } else if (g_logVerbose) {
            Wh_Log(L"Restored original CPU priority for %s.", g_processName.c_str());
        }
    }

    if (g_originalState.priorityBoostValid) {
        if (!SetProcessPriorityBoost(GetCurrentProcess(),
                                     g_originalState.disablePriorityBoost)) {
            Wh_Log(L"Failed to restore dynamic priority boost for %s: %lu",
                   g_processName.c_str(), GetLastError());
        } else if (g_logVerbose) {
            Wh_Log(L"Restored original dynamic priority boost for %s.",
                   g_processName.c_str());
        }
    }

    if (g_originalState.powerThrottlingValid && g_setProcessInformation) {
        if (!g_setProcessInformation(GetCurrentProcess(),
                                     kProcessPowerThrottlingInfoClass,
                                     &g_originalState.powerThrottlingState,
                                     sizeof(g_originalState.powerThrottlingState))) {
            Wh_Log(L"Failed to restore process power throttling for %s: %lu",
                   g_processName.c_str(), GetLastError());
        } else if (g_logVerbose) {
            Wh_Log(L"Restored original CPU power mode for %s.",
                   g_processName.c_str());
        }
    }

    if (g_originalState.ioPriorityValid && g_ntSetInformationProcess) {
        if (g_ntSetInformationProcess(GetCurrentProcess(), ProcessIoPriorityClass,
                                      &g_originalState.ioPriority,
                                      sizeof(ULONG)) != STATUS_SUCCESS) {
            Wh_Log(L"Failed to restore I/O priority for %s.",
                   g_processName.c_str());
        } else if (g_logVerbose) {
            Wh_Log(L"Restored original I/O priority for %s.",
                   g_processName.c_str());
        }
    }

    if (g_originalState.pagePriorityValid && g_ntSetInformationProcess) {
        if (g_ntSetInformationProcess(GetCurrentProcess(), ProcessPagePriorityClass,
                                      &g_originalState.pagePriority,
                                      sizeof(ULONG)) != STATUS_SUCCESS) {
            Wh_Log(L"Failed to restore page priority for %s.",
                   g_processName.c_str());
        } else if (g_logVerbose) {
            Wh_Log(L"Restored original page priority for %s.",
                   g_processName.c_str());
        }
    }

    if (g_originalState.timerResolutionActive && g_ntSetTimerResolution) {
        ULONG actual = 0;
        g_ntSetTimerResolution(g_originalState.timerResolutionOriginal, FALSE, &actual);
        g_originalState.timerResolutionActive = false;
        if (g_logVerbose) {
            Wh_Log(L"Restored original timer resolution for %s.",
                   g_processName.c_str());
        }
    }

    if (g_originalState.workingSetValid && g_setProcessWorkingSetSizeEx) {
        if (!g_setProcessWorkingSetSizeEx(GetCurrentProcess(),
                                          g_originalState.workingSetMin,
                                          g_originalState.workingSetMax,
                                          g_originalState.workingSetFlags)) {
            Wh_Log(L"Failed to restore working set for %s: %lu",
                   g_processName.c_str(), GetLastError());
        } else if (g_logVerbose) {
            Wh_Log(L"Restored original working set for %s.",
                   g_processName.c_str());
        }
    }

    if (g_originalState.memoryPriorityValid && g_setProcessInformation) {
        if (!g_setProcessInformation(GetCurrentProcess(), ProcessMemoryPriority,
                                     &g_originalState.memoryPriorityInfo,
                                     sizeof(g_originalState.memoryPriorityInfo))) {
            Wh_Log(L"Failed to restore memory priority for %s: %lu",
                   g_processName.c_str(), GetLastError());
        } else if (g_logVerbose) {
            Wh_Log(L"Restored original memory priority for %s.",
                   g_processName.c_str());
        }
    }

    if (g_originalState.gpuPreferenceValid) {
        std::optional<std::wstring> value;
        if (g_originalState.gpuPreferenceExisted) {
            value = g_originalState.gpuPreferenceValue;
        }

        if (!WriteCurrentGpuPreferenceValue(value)) {
            Wh_Log(L"Failed to restore GPU preference for %s.",
                   g_processName.c_str());
        } else if (g_logVerbose) {
            Wh_Log(L"Restored original GPU preference for %s.",
                   g_processName.c_str());
        }
    }
}

RuleSettings LoadMatchingRule() {
    RuleSettings rule;

    g_logVerbose = Wh_GetIntSetting(L"logVerbose") != 0;

    for (int i = 0;; i++) {
        std::wstring match = GetStringSettingCopy(L"rules[%d].match", i);
        if (match.empty()) {
            break;
        }

        if (!Wh_GetIntSetting(L"rules[%d].enabled", i)) {
            continue;
        }

        std::wstring whenProcessesRunning =
            GetStringSettingCopy(L"rules[%d].whenProcessesRunning", i);
        std::wstring whenProcessesNotRunning =
            GetStringSettingCopy(L"rules[%d].whenProcessesNotRunning", i);
        std::wstring matchError;
        if (!RuleMatchesCurrentProcess(match, whenProcessesRunning,
                                       whenProcessesNotRunning, &matchError)) {
            if (!matchError.empty() && g_logVerbose) {
                Wh_Log(L"Rule '%s' process conditions couldn't be evaluated for %s: %s",
                       match.c_str(), g_processName.c_str(), matchError.c_str());
            }
            continue;
        }

        rule.matched = true;
        rule.match = ResolveMatchPattern(match);
        rule.whenProcessesRunning = whenProcessesRunning;
        rule.whenProcessesNotRunning = whenProcessesNotRunning;
        rule.cpuCores = GetStringSettingCopy(L"rules[%d].cpuCores", i);
        rule.cpuSets = GetStringSettingCopy(L"rules[%d].cpuSets", i);
        rule.cpuSetNumaNode =
            GetStringSettingCopy(L"rules[%d].cpuSetNumaNode", i);
        rule.cpuSetLastLevelCache =
            GetStringSettingCopy(L"rules[%d].cpuSetLastLevelCache", i);
        rule.cpuSetLimit = GetStringSettingCopy(L"rules[%d].cpuSetLimit", i);
        rule.cpuSetPlacement =
            GetStringSettingCopy(L"rules[%d].cpuSetPlacement", i);
        rule.cpuSetAvoidSmt = Wh_GetIntSetting(L"rules[%d].cpuSetAvoidSmt", i) != 0;
        rule.cpuPriority = GetStringSettingCopy(L"rules[%d].cpuPriority", i);
        rule.dynamicPriorityBoost =
            GetStringSettingCopy(L"rules[%d].dynamicPriorityBoost", i);
        rule.cpuPowerMode = GetStringSettingCopy(L"rules[%d].cpuPowerMode", i);
        rule.cpuSetEfficiencyPreference =
            GetStringSettingCopy(L"rules[%d].cpuSetEfficiencyPreference", i);
        rule.timerResolution = GetStringSettingCopy(L"rules[%d].timerResolution", i);
        rule.workingSetMode = GetStringSettingCopy(L"rules[%d].workingSetMode", i);
        rule.profile = GetStringSettingCopy(L"rules[%d].profile", i);
        rule.ioPriority = GetStringSettingCopy(L"rules[%d].ioPriority", i);
        rule.pagePriority = GetStringSettingCopy(L"rules[%d].pagePriority", i);
        rule.memoryPriority = GetStringSettingCopy(L"rules[%d].memoryPriority", i);
        rule.gpuPreference = GetStringSettingCopy(L"rules[%d].gpuPreference", i);
        rule.autoHdr = GetStringSettingCopy(L"rules[%d].autoHdr", i);
        rule.windowedOptimizations = GetStringSettingCopy(L"rules[%d].windowedOptimizations", i);
        rule.variableRefreshRate = GetStringSettingCopy(L"rules[%d].variableRefreshRate", i);
        rule.npuMode = GetStringSettingCopy(L"rules[%d].npuMode", i);
        rule.npuDisableCpuFallback =
            Wh_GetIntSetting(L"rules[%d].npuDisableCpuFallback", i) != 0;
        rule.npuQnnPerformanceMode =
            GetStringSettingCopy(L"rules[%d].npuQnnPerformanceMode", i);
        rule.npuOpenVinoDeviceType =
            GetStringSettingCopy(L"rules[%d].npuOpenVinoDeviceType", i);
        rule.npuOpenVinoEnableFastCompile =
            Wh_GetIntSetting(L"rules[%d].npuOpenVinoEnableFastCompile", i) != 0;
        return rule;
    }

    return rule;
}

RuleSettings ApplyProfileDefaults(RuleSettings rule) {
    std::wstring profileStr = ToLowerCopy(TrimCopy(rule.profile));
    if (profileStr == L"low-latency") {
        auto isDefault = [](const std::wstring& v) {
            std::wstring lv = ToLowerCopy(TrimCopy(v));
            return lv.empty() || lv == L"unchanged";
        };
        if (isDefault(rule.cpuPriority)) rule.cpuPriority = L"high";
        if (isDefault(rule.cpuPowerMode)) rule.cpuPowerMode = L"disable-throttling";
        if (isDefault(rule.dynamicPriorityBoost)) rule.dynamicPriorityBoost = L"enable";
        if (isDefault(rule.ioPriority)) rule.ioPriority = L"normal";
        if (isDefault(rule.timerResolution)) rule.timerResolution = L"max-resolution";
        if (isDefault(rule.gpuPreference)) rule.gpuPreference = L"high-performance";
        if (isDefault(rule.windowedOptimizations)) rule.windowedOptimizations = L"enable";
        if (isDefault(rule.variableRefreshRate)) rule.variableRefreshRate = L"enable";
        if (isDefault(rule.workingSetMode)) rule.workingSetMode = L"lock-minimum";
    }
    return rule;
}

void ApplyRule(const RuleSettings& rawRule) {
    CaptureOriginalState();

    RuleSettings rule = ApplyProfileDefaults(RuleSettings(rawRule));

    g_loggedOrtNpuUnsupported = false;
    g_loggedOrtNpuFailure = false;
    g_loggedOrtNpuApplied = false;

    if (g_logVerbose) {
        Wh_Log(L"Matched rule '%s' for process '%s'.", rule.match.c_str(),
               g_processPath.c_str());
    }

    DWORD priorityClass = ParsePriorityClass(rule.cpuPriority);
    if (!TrimCopy(rule.cpuPriority).empty() &&
        !EqualsIgnoreCase(TrimCopy(rule.cpuPriority), L"unchanged")) {
        if (priorityClass == 0) {
            Wh_Log(L"Invalid cpuPriority value '%s' in rule '%s'.",
                   rule.cpuPriority.c_str(), rule.match.c_str());
        } else if (!SetPriorityClass(GetCurrentProcess(), priorityClass)) {
            Wh_Log(L"Failed to set CPU priority class for %s: %lu",
                   g_processName.c_str(), GetLastError());
        } else if (g_logVerbose) {
            Wh_Log(L"Applied CPU priority class '%s' to %s.",
                   rule.cpuPriority.c_str(), g_processName.c_str());
        }

        if (priorityClass == REALTIME_PRIORITY_CLASS) {
            Wh_Log(L"Warning: realtime CPU priority can starve the desktop, "
                   L"audio, and input threads in %s.",
                   g_processName.c_str());
        }
    }

    std::wstring trimmedDynamicPriorityBoost =
        TrimCopy(rule.dynamicPriorityBoost);
    if (!trimmedDynamicPriorityBoost.empty() &&
        !EqualsIgnoreCase(trimmedDynamicPriorityBoost, L"unchanged")) {
        DynamicPriorityBoostMode dynamicPriorityBoostMode =
            ParseDynamicPriorityBoostMode(trimmedDynamicPriorityBoost);
        if ((dynamicPriorityBoostMode == DynamicPriorityBoostMode::kUnchanged) &&
            !EqualsIgnoreCase(trimmedDynamicPriorityBoost, L"unchanged")) {
            Wh_Log(L"Invalid dynamicPriorityBoost value '%s' in rule '%s'.",
                   trimmedDynamicPriorityBoost.c_str(), rule.match.c_str());
        } else {
            BOOL disablePriorityBoost =
                dynamicPriorityBoostMode == DynamicPriorityBoostMode::kDisable;
            if (!SetProcessPriorityBoost(GetCurrentProcess(),
                                         disablePriorityBoost)) {
                Wh_Log(L"Failed to set dynamic priority boost for %s: %lu",
                       g_processName.c_str(), GetLastError());
            } else if (g_logVerbose) {
                Wh_Log(L"Applied dynamic priority boost '%s' to %s.",
                       trimmedDynamicPriorityBoost.c_str(),
                       g_processName.c_str());
            }
        }
    }

    std::wstring trimmedCpuCores = TrimCopy(rule.cpuCores);
    if (!trimmedCpuCores.empty() &&
        !EqualsIgnoreCase(trimmedCpuCores, L"unchanged")) {
        if (!g_originalState.affinityValid) {
            Wh_Log(L"Can't set CPU affinity for %s because GetProcessAffinityMask "
                   L"failed during initialization.",
                   g_processName.c_str());
        } else {
            DWORD_PTR affinityMask = 0;
            if (!ParseCpuCoreMask(trimmedCpuCores, g_originalState.systemAffinityMask,
                                  &affinityMask)) {
                Wh_Log(L"Invalid cpuCores value '%s' in rule '%s'.",
                       trimmedCpuCores.c_str(), rule.match.c_str());
            } else if (!SetProcessAffinityMask(GetCurrentProcess(), affinityMask)) {
                Wh_Log(L"Failed to set CPU affinity for %s: %lu",
                       g_processName.c_str(), GetLastError());
            } else if (g_logVerbose) {
                Wh_Log(L"Applied CPU affinity '%s' to %s.", trimmedCpuCores.c_str(),
                       g_processName.c_str());
            }
        }
    }

    if (HasCpuSetConfiguration(rule)) {
        if (!g_setProcessDefaultCpuSets || !g_getSystemCpuSetInformation ||
            !g_getProcessDefaultCpuSets) {
            Wh_Log(L"CPU Set APIs aren't available on this system for %s.",
                   g_processName.c_str());
        } else {
            std::vector<ULONG> selectedCpuSetIds;
            std::wstring cpuSetError;
            if (!BuildSelectedCpuSetIds(rule, &selectedCpuSetIds, &cpuSetError)) {
                if (!cpuSetError.empty()) {
                    Wh_Log(L"%s Rule '%s'.", cpuSetError.c_str(),
                           rule.match.c_str());
                }
            } else if (!selectedCpuSetIds.empty()) {
                if (!ApplyProcessDefaultCpuSets(selectedCpuSetIds)) {
                    Wh_Log(L"Failed to set CPU Sets for %s: %lu",
                           g_processName.c_str(), GetLastError());
                } else if (g_logVerbose) {
                    Wh_Log(L"Applied %lu CPU Set selection(s) to %s.",
                           static_cast<unsigned long>(selectedCpuSetIds.size()),
                           g_processName.c_str());
                }
            }
        }
    }

    std::wstring trimmedCpuPowerMode = TrimCopy(rule.cpuPowerMode);
    if (!trimmedCpuPowerMode.empty() &&
        !EqualsIgnoreCase(trimmedCpuPowerMode, L"unchanged")) {
        CpuPowerMode cpuPowerMode = ParseCpuPowerMode(trimmedCpuPowerMode);
        if ((cpuPowerMode == CpuPowerMode::kUnchanged) &&
            !EqualsIgnoreCase(trimmedCpuPowerMode, L"unchanged")) {
            Wh_Log(L"Invalid cpuPowerMode value '%s' in rule '%s'.",
                   trimmedCpuPowerMode.c_str(), rule.match.c_str());
        } else if (cpuPowerMode == CpuPowerMode::kEcoQosFull) {
            if (!g_setProcessInformation) {
                Wh_Log(L"SetProcessInformation isn't available for eco-qos-full.");
            } else {
                PROCESS_POWER_THROTTLING_STATE state = {};
                state.Version = PROCESS_POWER_THROTTLING_CURRENT_VERSION;
                state.ControlMask = PROCESS_POWER_THROTTLING_EXECUTION_SPEED |
                                    PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION;
                state.StateMask = PROCESS_POWER_THROTTLING_EXECUTION_SPEED |
                                  PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION;
                if (!g_setProcessInformation(GetCurrentProcess(),
                                             kProcessPowerThrottlingInfoClass,
                                             &state, sizeof(state))) {
                    Wh_Log(L"Failed to set eco-qos-full for %s: %lu",
                           g_processName.c_str(), GetLastError());
                } else if (g_logVerbose) {
                    Wh_Log(L"Applied eco-qos-full to %s.", g_processName.c_str());
                }
            }
        } else if (ApplyPowerThrottlingMode(cpuPowerMode) && g_logVerbose) {
            Wh_Log(L"Applied CPU power mode '%s' to %s.",
                   trimmedCpuPowerMode.c_str(), g_processName.c_str());
        }

        if ((cpuPowerMode == CpuPowerMode::kEnableThrottling ||
             cpuPowerMode == CpuPowerMode::kEcoQosFull) &&
            RuleLooksPerformanceOriented(rule)) {
            Wh_Log(L"Warning: cpuPowerMode '%s' conflicts with "
                   L"latency/throughput-oriented settings in rule '%s'.",
                   trimmedCpuPowerMode.c_str(), rule.match.c_str());
        }
    }

    // --- Efficiency Class Override ---
    std::wstring trimmedEffPref = TrimCopy(rule.cpuSetEfficiencyPreference);
    if (!trimmedEffPref.empty() && !EqualsIgnoreCase(trimmedEffPref, L"unchanged")) {
        std::wstring lowerEffPref = ToLowerCopy(trimmedEffPref);
        if (lowerEffPref == L"performance-only") {
            if (EqualsIgnoreCase(TrimCopy(rule.cpuSets), L"unchanged") || TrimCopy(rule.cpuSets).empty()) {
                rule.cpuSets = L"performance";
            }
            if (g_logVerbose) {
                Wh_Log(L"Applied efficiency preference '%s' to %s (CPU sets -> performance).",
                       trimmedEffPref.c_str(), g_processName.c_str());
            }
        } else if (lowerEffPref == L"efficiency-only") {
            if (EqualsIgnoreCase(TrimCopy(rule.cpuSets), L"unchanged") || TrimCopy(rule.cpuSets).empty()) {
                rule.cpuSets = L"efficiency";
            }
            if (g_logVerbose) {
                Wh_Log(L"Applied efficiency preference '%s' to %s (CPU sets -> efficiency).",
                       trimmedEffPref.c_str(), g_processName.c_str());
            }
        } else if (lowerEffPref == L"balanced") {
            if (g_logVerbose) {
                Wh_Log(L"Efficiency preference 'balanced' - no CPU set filter for %s.",
                       g_processName.c_str());
            }
        } else {
            Wh_Log(L"Invalid cpuSetEfficiencyPreference value '%s' in rule '%s'.",
                   trimmedEffPref.c_str(), rule.match.c_str());
        }
    }

    // Re-apply CPU Sets if efficiency preference changed the value
    if (HasCpuSetConfiguration(rule)) {
        if (!g_setProcessDefaultCpuSets || !g_getSystemCpuSetInformation ||
            !g_getProcessDefaultCpuSets) {
            Wh_Log(L"CPU Set APIs aren't available on this system for %s.",
                   g_processName.c_str());
        } else {
            std::vector<ULONG> selectedCpuSetIds;
            std::wstring cpuSetError;
            if (!BuildSelectedCpuSetIds(rule, &selectedCpuSetIds, &cpuSetError)) {
                if (!cpuSetError.empty()) {
                    Wh_Log(L"%s Rule '%s'.", cpuSetError.c_str(),
                           rule.match.c_str());
                }
            } else if (!selectedCpuSetIds.empty()) {
                if (!ApplyProcessDefaultCpuSets(selectedCpuSetIds)) {
                    Wh_Log(L"Failed to set CPU Sets for %s: %lu",
                           g_processName.c_str(), GetLastError());
                } else if (g_logVerbose) {
                    Wh_Log(L"Applied %lu CPU Set selection(s) to %s (efficiency-pref).",
                           static_cast<unsigned long>(selectedCpuSetIds.size()),
                           g_processName.c_str());
                }
            }
        }
    }

    // --- Timer Resolution ---
    std::wstring trimmedTimerRes = TrimCopy(rule.timerResolution);
    if (!trimmedTimerRes.empty() && !EqualsIgnoreCase(trimmedTimerRes, L"unchanged")) {
        if (EqualsIgnoreCase(trimmedTimerRes, L"max-resolution")) {
            if (!g_ntSetTimerResolution) {
                Wh_Log(L"NtSetTimerResolution isn't available.");
            } else {
                ULONG actual = 0;
                NTSTATUS status = g_ntSetTimerResolution(5000, TRUE, &actual);
                if (status == STATUS_SUCCESS) {
                    g_originalState.timerResolutionActive = true;
                    if (g_logVerbose) {
                        Wh_Log(L"Applied max timer resolution (0.5 ms, actual %lu) for %s.",
                               actual, g_processName.c_str());
                    }
                } else {
                    Wh_Log(L"Failed to set timer resolution for %s: 0x%08lX",
                           g_processName.c_str(), static_cast<unsigned long>(status));
                }
            }
        } else {
            Wh_Log(L"Invalid timerResolution value '%s' in rule '%s'.",
                   trimmedTimerRes.c_str(), rule.match.c_str());
        }
    }

    // --- Working Set Management ---
    std::wstring trimmedWorkingSet = TrimCopy(rule.workingSetMode);
    if (!trimmedWorkingSet.empty() && !EqualsIgnoreCase(trimmedWorkingSet, L"unchanged")) {
        std::wstring lowerWS = ToLowerCopy(trimmedWorkingSet);
        if (lowerWS == L"lock-minimum") {
            if (!g_setProcessWorkingSetSizeEx) {
                Wh_Log(L"SetProcessWorkingSetSizeEx isn't available.");
            } else {
                SIZE_T curMin = 0, curMax = 0;
                DWORD curFlags = 0;
                if (g_getProcessWorkingSetSizeEx &&
                    g_getProcessWorkingSetSizeEx(GetCurrentProcess(), &curMin, &curMax, &curFlags)) {
                    DWORD newFlags = QUOTA_LIMITS_HARDWS_MIN_ENABLE;
                    if (!g_setProcessWorkingSetSizeEx(GetCurrentProcess(), curMin, curMax, newFlags)) {
                        Wh_Log(L"Failed to lock working set minimum for %s: %lu",
                               g_processName.c_str(), GetLastError());
                    } else if (g_logVerbose) {
                        Wh_Log(L"Applied working set lock-minimum for %s (min=%llu, max=%llu).",
                               g_processName.c_str(),
                               static_cast<unsigned long long>(curMin),
                               static_cast<unsigned long long>(curMax));
                    }
                } else {
                    Wh_Log(L"GetProcessWorkingSetSizeEx failed for %s: %lu",
                           g_processName.c_str(), GetLastError());
                }
            }
        } else if (lowerWS == L"trim") {
            if (!g_setProcessWorkingSetSizeEx) {
                Wh_Log(L"SetProcessWorkingSetSizeEx isn't available.");
            } else {
                if (!g_setProcessWorkingSetSizeEx(GetCurrentProcess(),
                                                   static_cast<SIZE_T>(-1),
                                                   static_cast<SIZE_T>(-1), 0)) {
                    Wh_Log(L"Failed to trim working set for %s: %lu",
                           g_processName.c_str(), GetLastError());
                } else if (g_logVerbose) {
                    Wh_Log(L"Applied working set trim for %s.", g_processName.c_str());
                }
            }
        } else {
            Wh_Log(L"Invalid workingSetMode value '%s' in rule '%s'.",
                   trimmedWorkingSet.c_str(), rule.match.c_str());
        }
    }

    std::wstring trimmedIoPriority = TrimCopy(rule.ioPriority);
    if (!trimmedIoPriority.empty() &&
        !EqualsIgnoreCase(trimmedIoPriority, L"unchanged")) {
        std::optional<ULONG> ioPriority = ParseIoPriority(trimmedIoPriority);
        if (!ioPriority.has_value()) {
            Wh_Log(L"Invalid ioPriority value '%s' in rule '%s'.",
                   trimmedIoPriority.c_str(), rule.match.c_str());
        } else if (!g_ntSetInformationProcess) {
            Wh_Log(L"NtSetInformationProcess isn't available.");
        } else {
            ULONG priorityValue = ioPriority.value();
            if (g_ntSetInformationProcess(GetCurrentProcess(), ProcessIoPriorityClass,
                                          &priorityValue,
                                          sizeof(ULONG)) != STATUS_SUCCESS) {
                Wh_Log(L"Failed to set I/O priority for %s.",
                       g_processName.c_str());
            } else if (g_logVerbose) {
                Wh_Log(L"Applied I/O priority '%s' to %s.",
                       trimmedIoPriority.c_str(), g_processName.c_str());
            }
        }
    }

    std::wstring trimmedPagePriority = TrimCopy(rule.pagePriority);
    if (!trimmedPagePriority.empty() &&
        !EqualsIgnoreCase(trimmedPagePriority, L"unchanged")) {
        std::optional<ULONG> pagePriority = ParsePagePriority(trimmedPagePriority);
        if (!pagePriority.has_value()) {
            Wh_Log(L"Invalid pagePriority value '%s' in rule '%s'.",
                   trimmedPagePriority.c_str(), rule.match.c_str());
        } else if (!g_ntSetInformationProcess) {
            Wh_Log(L"NtSetInformationProcess isn't available.");
        } else {
            ULONG priorityValue = pagePriority.value();
            if (g_ntSetInformationProcess(GetCurrentProcess(), ProcessPagePriorityClass,
                                          &priorityValue,
                                          sizeof(ULONG)) != STATUS_SUCCESS) {
                Wh_Log(L"Failed to set page priority for %s.",
                       g_processName.c_str());
            } else if (g_logVerbose) {
                Wh_Log(L"Applied page priority '%s' to %s.",
                       trimmedPagePriority.c_str(), g_processName.c_str());
            }
        }
    }

    std::wstring trimmedMemoryPriority = TrimCopy(rule.memoryPriority);
    if (!trimmedMemoryPriority.empty() &&
        !EqualsIgnoreCase(trimmedMemoryPriority, L"unchanged")) {
        std::optional<ULONG> memoryPriority =
            ParseMemoryPriority(trimmedMemoryPriority);
        if (!memoryPriority.has_value()) {
            Wh_Log(L"Invalid memoryPriority value '%s' in rule '%s'.",
                   trimmedMemoryPriority.c_str(), rule.match.c_str());
        } else if (!g_setProcessInformation) {
            Wh_Log(L"SetProcessInformation(ProcessMemoryPriority) isn't available.");
        } else {
            COMPAT_MEMORY_PRIORITY_INFORMATION memoryPriorityInfo = {};
            memoryPriorityInfo.MemoryPriority = memoryPriority.value();
            if (!g_setProcessInformation(GetCurrentProcess(),
                                         ProcessMemoryPriority,
                                         &memoryPriorityInfo,
                                         sizeof(memoryPriorityInfo))) {
                Wh_Log(L"Failed to set memory priority for %s: %lu",
                       g_processName.c_str(), GetLastError());
            } else if (g_logVerbose) {
                Wh_Log(L"Applied memory priority '%s' to %s.",
                       trimmedMemoryPriority.c_str(), g_processName.c_str());
            }
        }
    }

    std::wstring trimmedGpuPreference = TrimCopy(rule.gpuPreference);
    std::wstring trimmedAutoHdr = TrimCopy(rule.autoHdr);
    std::wstring trimmedWindowedOptimizations = TrimCopy(rule.windowedOptimizations);
    std::wstring trimmedVariableRefreshRate = TrimCopy(rule.variableRefreshRate);

    bool hasGpuPreference = !trimmedGpuPreference.empty() && !EqualsIgnoreCase(trimmedGpuPreference, L"unchanged");
    bool hasAutoHdr = !trimmedAutoHdr.empty() && !EqualsIgnoreCase(trimmedAutoHdr, L"unchanged");
    bool hasWindowedOptimizations = !trimmedWindowedOptimizations.empty() && !EqualsIgnoreCase(trimmedWindowedOptimizations, L"unchanged");
    bool hasVariableRefreshRate = !trimmedVariableRefreshRate.empty() && !EqualsIgnoreCase(trimmedVariableRefreshRate, L"unchanged");

    if (hasGpuPreference || hasAutoHdr || hasWindowedOptimizations || hasVariableRefreshRate) {
        GpuPreference gpuPreference = GpuPreference::kUnchanged;
        if (hasGpuPreference) {
            gpuPreference = ParseGpuPreference(trimmedGpuPreference);
            if (gpuPreference == GpuPreference::kUnchanged) {
                Wh_Log(L"Invalid gpuPreference value '%s' in rule '%s'.",
                       trimmedGpuPreference.c_str(), rule.match.c_str());
            }
        }

        ToggleMode autoHdr = ToggleMode::kUnchanged;
        if (hasAutoHdr) {
            autoHdr = ParseToggleMode(trimmedAutoHdr);
            if (autoHdr == ToggleMode::kUnchanged) {
                Wh_Log(L"Invalid autoHdr value '%s' in rule '%s'.",
                       trimmedAutoHdr.c_str(), rule.match.c_str());
            }
        }

        ToggleMode windowedOptimizations = ToggleMode::kUnchanged;
        if (hasWindowedOptimizations) {
            windowedOptimizations = ParseToggleMode(trimmedWindowedOptimizations);
            if (windowedOptimizations == ToggleMode::kUnchanged) {
                Wh_Log(L"Invalid windowedOptimizations value '%s' in rule '%s'.",
                       trimmedWindowedOptimizations.c_str(), rule.match.c_str());
            }
        }

        ToggleMode variableRefreshRate = ToggleMode::kUnchanged;
        if (hasVariableRefreshRate) {
            variableRefreshRate = ParseToggleMode(trimmedVariableRefreshRate);
            if (variableRefreshRate == ToggleMode::kUnchanged) {
                Wh_Log(L"Invalid variableRefreshRate value '%s' in rule '%s'.",
                       trimmedVariableRefreshRate.c_str(), rule.match.c_str());
            }
        }

        std::wstring currentValue;
        bool existed = false;
        if (!ReadCurrentGpuPreferenceValue(&currentValue, &existed)) {
            Wh_Log(L"Couldn't read current UserGpuPreferences before applying rule "
                   L"'%s'.",
                   rule.match.c_str());
        } else {
            std::wstring newValue = BuildGraphicsFeatureRegistryValue(
                currentValue, gpuPreference, autoHdr, windowedOptimizations,
                variableRefreshRate);
            std::optional<std::wstring> valueToWrite;
            if (!newValue.empty()) {
                valueToWrite = newValue;
            }

            if (WriteCurrentGpuPreferenceValue(valueToWrite)) {
                if (g_logVerbose) {
                    Wh_Log(L"Applied UserGpuPreferences (GPU=%d, HDR=%d, WinOpt=%d, "
                           L"VRR=%d) to %s.",
                           (int)gpuPreference, (int)autoHdr,
                           (int)windowedOptimizations, (int)variableRefreshRate,
                           g_processName.c_str());
                }

                if (IsGraphicsStackAlreadyLoaded()) {
                    Wh_Log(L"GPU modules are already loaded in %s. Restart the "
                           L"process if the UserGpuPreferences don't take effect.",
                           g_processName.c_str());
                }
            }
        }
    }

    std::wstring trimmedNpuMode = TrimCopy(rule.npuMode);
    if (rule.npuDisableCpuFallback ||
        (!trimmedNpuMode.empty() &&
         !EqualsIgnoreCase(trimmedNpuMode, L"unchanged"))) {
        NpuMode npuMode = ParseNpuMode(trimmedNpuMode);
        if ((npuMode == NpuMode::kUnchanged) &&
            !EqualsIgnoreCase(trimmedNpuMode, L"unchanged")) {
            Wh_Log(L"Invalid npuMode value '%s' in rule '%s'.",
                   trimmedNpuMode.c_str(), rule.match.c_str());
        } else {
            const char* qnnPerformanceMode = nullptr;
            if (npuMode == NpuMode::kOnnxRuntimeQnn &&
                !TryGetQnnPerformanceModeValue(rule.npuQnnPerformanceMode,
                                               &qnnPerformanceMode)) {
                Wh_Log(L"Invalid npuQnnPerformanceMode value '%s' in rule '%s'.",
                       rule.npuQnnPerformanceMode.c_str(), rule.match.c_str());
            }

            const char* openVinoDeviceType = nullptr;
            if (npuMode == NpuMode::kOnnxRuntimeOpenVino &&
                !TryGetOpenVinoDeviceTypeValue(rule, &openVinoDeviceType)) {
                Wh_Log(L"Invalid npuOpenVinoDeviceType value '%s' in rule '%s'.",
                       rule.npuOpenVinoDeviceType.c_str(), rule.match.c_str());
            }

            if (EnsureOnnxRuntimeNpuHooks()) {
                if (g_logVerbose) {
                    std::wstring npuModeSummary =
                        DescribeOnnxRuntimeNpuMode(rule);
                    if (npuMode == NpuMode::kUnchanged &&
                        rule.npuDisableCpuFallback) {
                        npuModeSummary = L"cpu-fallback-disabled";
                    }
                    if (GetModuleHandleW(kOnnxRuntimeModuleName)) {
                        Wh_Log(L"Armed ONNX Runtime NPU hooks (%s) for %s. "
                               L"onnxruntime.dll is already loaded.",
                               npuModeSummary.c_str(), g_processName.c_str());
                    } else {
                        Wh_Log(L"Armed ONNX Runtime NPU hooks (%s) for %s. "
                               L"Waiting for onnxruntime.dll to load.",
                               npuModeSummary.c_str(), g_processName.c_str());
                    }

                    if (rule.npuDisableCpuFallback) {
                        Wh_Log(L"ONNX Runtime CPU fallback will be disabled for new "
                               L"sessions in %s.",
                               g_processName.c_str());
                    }
                }
            } else {
                Wh_Log(L"Failed to arm ONNX Runtime NPU hooks for %s.",
                       g_processName.c_str());
            }
        }
    }
}

}  // namespace

BOOL Wh_ModInit() {
    g_processPath = GetCurrentProcessPath();
    g_processName = GetExeNameFromPath(g_processPath);
    g_isPickerHost = EqualsIgnoreCase(g_processName, L"windhawk.exe");
    LoadProcessInformationFunctions();

    g_activeRule = LoadMatchingRule();
    if (g_activeRule.matched) {
        ApplyRule(g_activeRule);
    }

    MaybeLaunchProcessPicker();

    if (g_isPickerHost) {
        return TRUE;
    }

    return g_activeRule.matched ? TRUE : FALSE;
}

void Wh_ModSettingsChanged() {
    MaybeLaunchProcessPicker();
    RestoreOriginalState();

    g_activeRule = LoadMatchingRule();
    if (!g_activeRule.matched) {
        if (g_logVerbose) {
            Wh_Log(L"No active rule remains for %s after settings reload.",
                   g_processName.c_str());
        }
        return;
    }

    ApplyRule(g_activeRule);
}

void Wh_ModUninit() {
    RestoreOnnxRuntimePatches();
    RestoreOriginalState();
}
