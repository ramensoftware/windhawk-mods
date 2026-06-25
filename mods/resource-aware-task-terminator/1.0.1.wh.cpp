// ==WindhawkMod==
// @id              resource-aware-task-terminator
// @name            Resource-Aware Task Terminator
// @description     Terminate the active window process, a top resource offender, or a weighted resource-heavy app group with verified process identity and sortable usage metrics.
// @version         1.0.1
// @author          Math Shamenson
// @github          https://github.com/insane66613
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -lpsapi -lgdi32 -lcomctl32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Resource-Aware Task Terminator

A global-hotkey process terminator that runs in a dedicated Windhawk tool process.
It can target:

- The process that owns the foreground window.
- The highest CPU, page-fault, or read/write I/O offender measured over a sample window.
- An executable group ranked by a weighted aggregate of CPU, page faults, I/O, and working-set memory.

## Safety model

This mod force-terminates processes. There is no undo and applications do not receive a
normal save/close request. Unsaved data can be lost.

Before termination, the mod:

1. Excludes its own process, low system PIDs, known critical Windows processes, and names
   listed in **Protected applications**.
2. Captures each target's PID, process creation time, and full executable path.
3. Re-verifies creation time and path immediately before termination using the same handle
   passed to `TerminateProcess`.
4. Skips any target that cannot be inspected or whose identity changed.

A PID alone is never treated as process identity.

## Hotkey and prompts

The default hotkey is **Ctrl+Alt+F4**. The multi-candidate chooser is always shown when
more than one candidate exists. **Confirm before termination** controls only the final
Yes/No warning; it does not bypass the chooser.

## Resource metrics

- CPU is the process's percentage of total machine CPU capacity across all logical processors.
- Page faults include both soft and hard faults.
- I/O uses process read/write transfer counters. It is not limited to physical disk traffic.
- Aggregate mode may terminate multiple verified PIDs that share the same executable path.
  It terminates only the PIDs displayed and confirmed; newly spawned processes are not added
  silently.

## Permissions

Processes that cannot be queried are treated as protected and omitted. Terminating elevated
or otherwise restricted targets may require running Windhawk elevated. The chooser reports
how many processes could not be inspected, and the result dialog reports per-PID failures.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- disable_ctrl: false
  $name: Do not require Ctrl
  $description: Keep false to include Ctrl in the global hotkey.
- disable_alt: false
  $name: Do not require Alt
  $description: Keep false to include Alt in the global hotkey.
- require_shift: false
  $name: Require Shift
  $description: Include Shift in the global hotkey.
- require_win: false
  $name: Require Windows key
  $description: Include the Windows key in the global hotkey.
- kill_key: F4
  $name: Hotkey key
  $description: One letter, one digit, or F1 through F24.
- kill_mode: foreground
  $name: Target mode
  $description: Choose the foreground process, a sampled resource offender, or a weighted executable group.
  $options:
    - foreground: Foreground window process
    - top_cpu: Top CPU offender
    - top_pagefaults: Top page-fault offender
    - top_disk_io: Top read/write I/O offender
    - aggregate_super_score: Weighted executable group
- sample_ms: 3000
  $name: Sample window (milliseconds)
  $description: Allowed range 500 through 15000.
- candidate_count: 5
  $name: Candidate count
  $description: Number of ranked targets shown, from 1 through 10.
- live_update_default: false
  $name: Enable live update by default
- live_poll_ms: 5000
  $name: Live-update interval (milliseconds)
  $description: Allowed range 1000 through 60000. Overlapping refreshes are suppressed.
- disable_dark_ui: false
  $name: Disable dark chooser UI
- super_cpu_weight: 45
  $name: Aggregate CPU weight
  $description: Allowed range 0 through 100.
- super_pagefault_weight: 20
  $name: Aggregate page-fault weight
  $description: Allowed range 0 through 100.
- super_disk_weight: 25
  $name: Aggregate I/O weight
  $description: Allowed range 0 through 100.
- super_memory_weight: 10
  $name: Aggregate memory weight
  $description: Allowed range 0 through 100.
- disable_confirmation: false
  $name: Disable confirmation before termination
  $description: Keep false to show the final Yes/No warning. The multi-candidate chooser remains enabled.
- show_result_summary: true
  $name: Show termination result
  $description: Display success and failure counts after each termination request.
- protected_apps: "explorer.exe, windhawk.exe, taskmgr.exe, conhost.exe, lsass.exe, csrss.exe, wininit.exe, services.exe, winlogon.exe, dwm.exe"
  $name: Protected applications
  $description: Comma-separated executable names, for example explorer.exe, My App.exe. Leading and trailing spaces are ignored; interior spaces are preserved.
*/
// ==/WindhawkModSettings==

#include <algorithm>
#include <atomic>
#include <cstdio>
#include <cwctype>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <windhawk_utils.h>
#include <windows.h>
#include <commctrl.h>
#include <psapi.h>
#include <shellapi.h>
#include <tlhelp32.h>

#define HOTKEY_ID 1
#define WM_RELOAD_HOTKEY (WM_APP + 1)
#define WM_CANDIDATE_REFRESH_COMPLETE (WM_APP + 2)

#define IDC_CANDIDATE_LIST 4201
#define IDC_REFRESH_BUTTON 4202
#define IDC_LIVE_CHECK 4203
#define IDC_POLL_EDIT 4204
#define IDC_STATUS_STATIC 4205
#define TIMER_LIVE_REFRESH 4301

HWND g_hHiddenWindow = nullptr;
HANDLE g_hThread = nullptr;
DWORD g_dwThreadId = 0;
HANDLE g_hStopEvent = nullptr;

std::mutex g_settingsMutex;
UINT g_modifiers = 0;
UINT g_vkCode = VK_F4;
std::vector<std::wstring> g_blacklist;

enum class KillMode {
    Foreground,
    TopCpu,
    TopPagefaults,
    TopIo,
    AggregateSuperScore,
};

KillMode g_killMode = KillMode::Foreground;
DWORD g_sampleMs = 3000;
int g_candidateCount = 5;
bool g_liveUpdateDefault = false;
DWORD g_livePollMs = 5000;
bool g_darkChooserUi = true;
int g_superCpuWeight = 45;
int g_superPageFaultWeight = 20;
int g_superIoWeight = 25;
int g_superMemoryWeight = 10;
bool g_confirmBeforeKill = true;
bool g_showResultSummary = true;

std::atomic<bool> g_killInProgress{false};
std::atomic<HWND> g_activeChooser{nullptr};
std::mutex g_workerMutex;
HANDLE g_hKillWorker = nullptr;
DWORD g_dwKillWorkerId = 0;

// ------------------------ General helpers ------------------------

static ULONGLONG FileTimeToUInt64(const FILETIME& value) {
    ULARGE_INTEGER result{};
    result.LowPart = value.dwLowDateTime;
    result.HighPart = value.dwHighDateTime;
    return result.QuadPart;
}

static std::wstring LowerCopy(std::wstring value) {
    std::transform(value.begin(), value.end(), value.begin(), [](wchar_t ch) {
        return static_cast<wchar_t>(std::towlower(ch));
    });
    return value;
}

static std::wstring TrimCopy(const std::wstring& value) {
    size_t first = 0;
    while (first < value.size() && std::iswspace(value[first])) {
        first++;
    }

    size_t last = value.size();
    while (last > first && std::iswspace(value[last - 1])) {
        last--;
    }

    return value.substr(first, last - first);
}

static std::wstring ExeNameFromPath(const std::wstring& path) {
    size_t separator = path.find_last_of(L"\\/");
    return separator == std::wstring::npos ? path : path.substr(separator + 1);
}

static std::wstring NormalizePath(const std::wstring& path) {
    std::wstring normalized = path;
    if (normalized.rfind(L"\\\\?\\", 0) == 0) {
        normalized.erase(0, 4);
    }
    return LowerCopy(normalized);
}

static bool IsCriticalProcessName(const std::wstring& exeName) {
    std::wstring lower = LowerCopy(exeName);
    return lower == L"lsass.exe" || lower == L"csrss.exe" ||
           lower == L"wininit.exe" || lower == L"smss.exe" ||
           lower == L"services.exe" || lower == L"winlogon.exe" ||
           lower == L"windhawk.exe";
}

static bool IsConfiguredProtectedName(const std::wstring& exeName) {
    std::wstring lower = LowerCopy(exeName);
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return std::find(g_blacklist.begin(), g_blacklist.end(), lower) != g_blacklist.end();
}

static bool IsProtectedName(const std::wstring& exeName) {
    return IsCriticalProcessName(exeName) || IsConfiguredProtectedName(exeName);
}

static bool QueryProcessPathFromHandle(HANDLE process, std::wstring& path) {
    std::vector<WCHAR> buffer(32768);
    DWORD size = static_cast<DWORD>(buffer.size());
    if (!QueryFullProcessImageNameW(process, 0, buffer.data(), &size) || size == 0) {
        return false;
    }

    path.assign(buffer.data(), size);
    return true;
}

static UINT ParseKeyCode(PCWSTR keyStr) {
    std::wstring key = TrimCopy(keyStr ? keyStr : L"");
    if (key.empty()) {
        return VK_F4;
    }

    if (key.length() == 1) {
        wchar_t value = static_cast<wchar_t>(std::towupper(key[0]));
        if ((value >= L'A' && value <= L'Z') ||
            (value >= L'0' && value <= L'9')) {
            return static_cast<UINT>(value);
        }
    } else if (key.length() >= 2 && std::towupper(key[0]) == L'F') {
        wchar_t* end = nullptr;
        long functionNumber = wcstol(key.c_str() + 1, &end, 10);
        if (end && *end == L'\0' && functionNumber >= 1 && functionNumber <= 24) {
            return VK_F1 + static_cast<UINT>(functionNumber - 1);
        }
    }

    Wh_Log(L"Invalid kill_key setting '%s'; using F4.", key.c_str());
    return VK_F4;
}

static KillMode ParseKillMode(PCWSTR value) {
    std::wstring mode = LowerCopy(TrimCopy(value ? value : L""));
    if (mode == L"top_cpu") return KillMode::TopCpu;
    if (mode == L"top_pagefaults") return KillMode::TopPagefaults;
    if (mode == L"top_io" || mode == L"top_disk_io" ||
        mode == L"top_disk" || mode == L"disk_activity") {
        return KillMode::TopIo;
    }
    if (mode == L"aggregate_super_score" || mode == L"super_score" ||
        mode == L"aggregate") {
        return KillMode::AggregateSuperScore;
    }
    return KillMode::Foreground;
}

static PCWSTR KillModeName(KillMode mode) {
    switch (mode) {
    case KillMode::Foreground: return L"foreground";
    case KillMode::TopCpu: return L"top_cpu";
    case KillMode::TopPagefaults: return L"top_pagefaults";
    case KillMode::TopIo: return L"top_io";
    case KillMode::AggregateSuperScore: return L"aggregate_super_score";
    }
    return L"unknown";
}

static std::wstring FormatDouble(double value) {
    WCHAR buffer[64]{};
    swprintf(buffer, ARRAYSIZE(buffer), L"%.2f", value);
    return buffer;
}

static std::wstring FormatInteger(double value) {
    WCHAR buffer[64]{};
    swprintf(buffer, ARRAYSIZE(buffer), L"%.0f", value);
    return buffer;
}

static std::wstring FormatMb(double bytesOrMb, bool valueIsBytes) {
    double mb = valueIsBytes ? bytesOrMb / (1024.0 * 1024.0) : bytesOrMb;
    WCHAR buffer[64]{};
    swprintf(buffer, ARRAYSIZE(buffer), L"%.2f", mb);
    return buffer;
}

static void ShowTopmostMessage(PCWSTR text, PCWSTR title, UINT flags) {
    MessageBoxW(nullptr, text, title,
                flags | MB_TOPMOST | MB_SETFOREGROUND | MB_TASKMODAL);
}

// ------------------------ Settings ------------------------

static void LoadSettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);

    g_modifiers = MOD_NOREPEAT;
    if (Wh_GetIntSetting(L"disable_ctrl") == 0) g_modifiers |= MOD_CONTROL;
    if (Wh_GetIntSetting(L"disable_alt") == 0) g_modifiers |= MOD_ALT;
    if (Wh_GetIntSetting(L"require_shift") != 0) g_modifiers |= MOD_SHIFT;
    if (Wh_GetIntSetting(L"require_win") != 0) g_modifiers |= MOD_WIN;

    auto keyStr = WindhawkUtils::StringSetting::make(L"kill_key");
    g_vkCode = ParseKeyCode(keyStr.get());

    auto modeStr = WindhawkUtils::StringSetting::make(L"kill_mode");
    g_killMode = ParseKillMode(modeStr.get());

    int sampleMs = Wh_GetIntSetting(L"sample_ms");
    g_sampleMs = sampleMs >= 500 && sampleMs <= 15000
        ? static_cast<DWORD>(sampleMs)
        : 3000;

    int candidateCount = Wh_GetIntSetting(L"candidate_count");
    g_candidateCount = candidateCount >= 1 && candidateCount <= 10
        ? candidateCount
        : 5;

    g_liveUpdateDefault = Wh_GetIntSetting(L"live_update_default") != 0;

    int livePollMs = Wh_GetIntSetting(L"live_poll_ms");
    g_livePollMs = livePollMs >= 1000 && livePollMs <= 60000
        ? static_cast<DWORD>(livePollMs)
        : 5000;

    g_darkChooserUi = Wh_GetIntSetting(L"disable_dark_ui") == 0;

    auto clampWeight = [](int value, int fallback) {
        return value >= 0 && value <= 100 ? value : fallback;
    };
    g_superCpuWeight = clampWeight(Wh_GetIntSetting(L"super_cpu_weight"), 45);
    g_superPageFaultWeight =
        clampWeight(Wh_GetIntSetting(L"super_pagefault_weight"), 20);
    g_superIoWeight = clampWeight(Wh_GetIntSetting(L"super_disk_weight"), 25);
    g_superMemoryWeight =
        clampWeight(Wh_GetIntSetting(L"super_memory_weight"), 10);

    g_confirmBeforeKill = Wh_GetIntSetting(L"disable_confirmation") == 0;
    g_showResultSummary = Wh_GetIntSetting(L"show_result_summary") != 0;

    g_blacklist.clear();
    auto appsStr = WindhawkUtils::StringSetting::make(L"protected_apps");
    std::wstring rawApps = appsStr.get();

    size_t start = 0;
    while (start <= rawApps.size()) {
        size_t comma = rawApps.find(L',', start);
        std::wstring app = TrimCopy(rawApps.substr(
            start, comma == std::wstring::npos ? std::wstring::npos : comma - start));
        app = LowerCopy(app);
        if (!app.empty()) {
            g_blacklist.push_back(std::move(app));
        }
        if (comma == std::wstring::npos) {
            break;
        }
        start = comma + 1;
    }
}

// ------------------------ Process identity and protection ------------------------

struct ProcessIdentity {
    DWORD pid{};
    ULONGLONG creationTime100ns{};
    std::wstring processPath;
    std::wstring exeName;
};

static bool CaptureIdentityFromHandle(HANDLE process, DWORD pid,
                                      ProcessIdentity& identity) {
    FILETIME creation{}, exit{}, kernel{}, user{};
    if (!GetProcessTimes(process, &creation, &exit, &kernel, &user)) {
        return false;
    }

    std::wstring path;
    if (!QueryProcessPathFromHandle(process, path)) {
        return false;
    }

    identity.pid = pid;
    identity.creationTime100ns = FileTimeToUInt64(creation);
    identity.processPath = std::move(path);
    identity.exeName = ExeNameFromPath(identity.processPath);
    return true;
}

static bool CaptureProcessIdentity(DWORD pid, ProcessIdentity& identity) {
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        return false;
    }

    bool success = CaptureIdentityFromHandle(process, pid, identity);
    CloseHandle(process);
    return success;
}

static bool SameProcessIdentity(const ProcessIdentity& left,
                                const ProcessIdentity& right) {
    return left.pid == right.pid &&
           left.creationTime100ns == right.creationTime100ns &&
           _wcsicmp(NormalizePath(left.processPath).c_str(),
                    NormalizePath(right.processPath).c_str()) == 0;
}

static bool IsProcessProtected(DWORD pid,
                               const std::wstring& knownExeName = L"") {
    if (pid <= 4 || pid == GetCurrentProcessId()) {
        return true;
    }

    if (!knownExeName.empty() && IsProtectedName(knownExeName)) {
        return true;
    }

    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        return true;  // Fail closed: an unverifiable target is never killable.
    }

    ProcessIdentity identity;
    bool captured = CaptureIdentityFromHandle(process, pid, identity);
    CloseHandle(process);
    if (!captured) {
        return true;
    }

    return IsProtectedName(identity.exeName);
}

static bool OpenVerifiedProcessForTermination(const ProcessIdentity& expected,
                                              HANDLE& process,
                                              std::wstring& failureReason) {
    process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_TERMINATE,
                          FALSE, expected.pid);
    if (!process) {
        DWORD error = GetLastError();
        failureReason = L"OpenProcess failed (error " + std::to_wstring(error) + L")";
        return false;
    }

    ProcessIdentity current;
    if (!CaptureIdentityFromHandle(process, expected.pid, current)) {
        DWORD error = GetLastError();
        failureReason = L"Identity query failed (error " +
                        std::to_wstring(error) + L")";
        CloseHandle(process);
        process = nullptr;
        return false;
    }

    if (!SameProcessIdentity(expected, current)) {
        failureReason = L"PID identity changed after selection";
        CloseHandle(process);
        process = nullptr;
        return false;
    }

    if (IsProtectedName(current.exeName) || current.pid <= 4 ||
        current.pid == GetCurrentProcessId()) {
        failureReason = L"Target is protected";
        CloseHandle(process);
        process = nullptr;
        return false;
    }

    return true;
}

static bool VerifySelectedIdentity(const ProcessIdentity& expected,
                                   std::wstring& failureReason) {
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE,
                                 expected.pid);
    if (!process) {
        DWORD error = GetLastError();
        failureReason = L"Cannot inspect PID " + std::to_wstring(expected.pid) +
                        L" (error " + std::to_wstring(error) + L")";
        return false;
    }

    ProcessIdentity current;
    bool captured = CaptureIdentityFromHandle(process, expected.pid, current);
    DWORD error = captured ? ERROR_SUCCESS : GetLastError();
    CloseHandle(process);

    if (!captured) {
        failureReason = L"Cannot verify PID " + std::to_wstring(expected.pid) +
                        L" (error " + std::to_wstring(error) + L")";
        return false;
    }

    if (!SameProcessIdentity(expected, current)) {
        failureReason = L"PID " + std::to_wstring(expected.pid) +
                        L" no longer belongs to the selected process";
        return false;
    }

    if (IsProtectedName(current.exeName) || current.pid <= 4 ||
        current.pid == GetCurrentProcessId()) {
        failureReason = L"PID " + std::to_wstring(expected.pid) +
                        L" is protected";
        return false;
    }

    return true;
}

// ------------------------ Process sampling ------------------------

struct ProcSnapshot {
    ProcessIdentity identity;
    bool cpuValid{};
    bool memoryValid{};
    bool ioValid{};
    ULONGLONG cpuKernel100ns{};
    ULONGLONG cpuUser100ns{};
    DWORD pageFaultCount{};
    ULONGLONG ioReadBytes{};
    ULONGLONG ioWriteBytes{};
    SIZE_T workingSetBytes{};
};

struct SamplingDiagnostics {
    size_t enumerated{};
    size_t inaccessible{};
    size_t identityFailures{};
    size_t metricFailures{};
    bool cancelled{};
};

static bool CancellationRequested(HANDLE localCancelEvent = nullptr) {
    if (g_hStopEvent &&
        WaitForSingleObject(g_hStopEvent, 0) == WAIT_OBJECT_0) {
        return true;
    }
    return localCancelEvent &&
           WaitForSingleObject(localCancelEvent, 0) == WAIT_OBJECT_0;
}

static bool WaitSampleInterval(DWORD sampleMs, HANDLE localCancelEvent) {
    HANDLE events[2]{};
    DWORD count = 0;
    if (g_hStopEvent) {
        events[count++] = g_hStopEvent;
    }
    if (localCancelEvent && localCancelEvent != g_hStopEvent) {
        events[count++] = localCancelEvent;
    }

    if (count == 0) {
        Sleep(sampleMs);
        return true;
    }

    DWORD wait = WaitForMultipleObjects(count, events, FALSE, sampleMs);
    return wait == WAIT_TIMEOUT;
}

static bool EnumerateProcesses(std::vector<ProcSnapshot>& output,
                               SamplingDiagnostics& diagnostics,
                               HANDLE cancelEvent) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return false;
    }

    PROCESSENTRY32W entry{};
    entry.dwSize = sizeof(entry);
    if (!Process32FirstW(snapshot, &entry)) {
        CloseHandle(snapshot);
        return false;
    }

    do {
        if (CancellationRequested(cancelEvent)) {
            diagnostics.cancelled = true;
            CloseHandle(snapshot);
            return false;
        }

        diagnostics.enumerated++;
        DWORD pid = entry.th32ProcessID;
        std::wstring toolhelpName = entry.szExeFile;
        if (pid <= 4 || pid == GetCurrentProcessId() ||
            IsProtectedName(toolhelpName)) {
            continue;
        }

        HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if (!process) {
            diagnostics.inaccessible++;
            continue;
        }

        ProcSnapshot processSnapshot;
        processSnapshot.identity.pid = pid;

        FILETIME creation{}, exit{}, kernel{}, user{};
        if (GetProcessTimes(process, &creation, &exit, &kernel, &user)) {
            processSnapshot.identity.creationTime100ns = FileTimeToUInt64(creation);
            processSnapshot.cpuKernel100ns = FileTimeToUInt64(kernel);
            processSnapshot.cpuUser100ns = FileTimeToUInt64(user);
            processSnapshot.cpuValid = true;
        } else {
            diagnostics.identityFailures++;
            CloseHandle(process);
            continue;
        }

        if (!QueryProcessPathFromHandle(process,
                                        processSnapshot.identity.processPath)) {
            diagnostics.identityFailures++;
            CloseHandle(process);
            continue;
        }

        processSnapshot.identity.exeName =
            ExeNameFromPath(processSnapshot.identity.processPath);
        if (IsProtectedName(processSnapshot.identity.exeName)) {
            CloseHandle(process);
            continue;
        }

        PROCESS_MEMORY_COUNTERS memoryCounters{};
        if (GetProcessMemoryInfo(process, &memoryCounters,
                                 sizeof(memoryCounters))) {
            processSnapshot.pageFaultCount = memoryCounters.PageFaultCount;
            processSnapshot.workingSetBytes = memoryCounters.WorkingSetSize;
            processSnapshot.memoryValid = true;
        } else {
            diagnostics.metricFailures++;
        }

        IO_COUNTERS ioCounters{};
        if (GetProcessIoCounters(process, &ioCounters)) {
            processSnapshot.ioReadBytes = ioCounters.ReadTransferCount;
            processSnapshot.ioWriteBytes = ioCounters.WriteTransferCount;
            processSnapshot.ioValid = true;
        } else {
            diagnostics.metricFailures++;
        }

        CloseHandle(process);
        output.push_back(std::move(processSnapshot));
    } while (Process32NextW(snapshot, &entry));

    CloseHandle(snapshot);
    return true;
}

struct SamplePair {
    std::vector<ProcSnapshot> before;
    std::vector<ProcSnapshot> after;
    SamplingDiagnostics diagnostics;
    ULONGLONG systemCpuDelta{};
    bool systemCpuValid{};
    bool success{};
};

static SamplePair CaptureSamplePair(DWORD sampleMs, HANDLE cancelEvent) {
    SamplePair result;

    FILETIME idleBefore{}, kernelBefore{}, userBefore{};
    FILETIME idleAfter{}, kernelAfter{}, userAfter{};
    bool systemBefore =
        GetSystemTimes(&idleBefore, &kernelBefore, &userBefore) != FALSE;

    if (!EnumerateProcesses(result.before, result.diagnostics, cancelEvent)) {
        return result;
    }

    if (!WaitSampleInterval(sampleMs, cancelEvent)) {
        result.diagnostics.cancelled = true;
        return result;
    }

    if (!EnumerateProcesses(result.after, result.diagnostics, cancelEvent)) {
        return result;
    }

    bool systemAfter =
        GetSystemTimes(&idleAfter, &kernelAfter, &userAfter) != FALSE;
    if (systemBefore && systemAfter) {
        ULONGLONG before = FileTimeToUInt64(kernelBefore) +
                           FileTimeToUInt64(userBefore);
        ULONGLONG after = FileTimeToUInt64(kernelAfter) +
                          FileTimeToUInt64(userAfter);
        if (after > before) {
            result.systemCpuDelta = after - before;
            result.systemCpuValid = true;
        }
    }

    result.success = true;
    return result;
}

static bool SameSnapshotIdentity(const ProcSnapshot& before,
                                 const ProcSnapshot& after) {
    return SameProcessIdentity(before.identity, after.identity);
}

static double NormalizeMetric(double value, double maxValue) {
    if (value <= 0.0 || maxValue <= 0.0) {
        return 0.0;
    }
    return value / maxValue;
}

struct KillDecision {
    ProcessIdentity identity;
    KillMode mode = KillMode::Foreground;
    DWORD sampleMs{};
    HWND foregroundWindow = nullptr;
    std::wstring windowTitle;
    std::wstring windowClass;
    std::wstring reason;
    std::wstring metricName;
    double metricValue{};
    bool cpuValid{};
    bool pageFaultValid{};
    bool ioValid{};
    bool memoryValid{};
    double cpuPercent{};
    double pageFaultDelta{};
    double ioBytes{};
    double memoryMb{};
    double superScore{};
    bool terminateGroup{};
    std::vector<ProcessIdentity> targetIdentities;
};

struct CandidateBuildResult {
    std::vector<KillDecision> candidates;
    SamplingDiagnostics diagnostics;
    bool cancelled{};
};

static std::wstring FormatPrimaryMetric(const KillDecision& decision) {
    switch (decision.mode) {
    case KillMode::TopPagefaults:
        return FormatInteger(decision.metricValue);
    case KillMode::TopIo:
        return FormatMb(decision.metricValue, true);
    case KillMode::Foreground:
        return L"selected window";
    case KillMode::TopCpu:
    case KillMode::AggregateSuperScore:
        return FormatDouble(decision.metricValue);
    }
    return FormatDouble(decision.metricValue);
}

static void PopulateDecisionMetrics(KillDecision& decision,
                                    const ProcSnapshot& before,
                                    const ProcSnapshot& after,
                                    const SamplePair& sample) {
    if (before.cpuValid && after.cpuValid && sample.systemCpuValid) {
        ULONGLONG beforeCpu = before.cpuKernel100ns + before.cpuUser100ns;
        ULONGLONG afterCpu = after.cpuKernel100ns + after.cpuUser100ns;
        if (afterCpu >= beforeCpu) {
            decision.cpuPercent =
                static_cast<double>(afterCpu - beforeCpu) /
                static_cast<double>(sample.systemCpuDelta) * 100.0;
            decision.cpuValid = true;
        }
    }

    if (before.memoryValid && after.memoryValid) {
        DWORD moduloDelta =
            static_cast<DWORD>(after.pageFaultCount - before.pageFaultCount);
        decision.pageFaultDelta = static_cast<double>(moduloDelta);
        decision.pageFaultValid = true;
        decision.memoryMb = static_cast<double>(after.workingSetBytes) /
                            (1024.0 * 1024.0);
        decision.memoryValid = true;
    }

    if (before.ioValid && after.ioValid &&
        after.ioReadBytes >= before.ioReadBytes &&
        after.ioWriteBytes >= before.ioWriteBytes) {
        ULONGLONG readDelta = after.ioReadBytes - before.ioReadBytes;
        ULONGLONG writeDelta = after.ioWriteBytes - before.ioWriteBytes;
        decision.ioBytes = static_cast<double>(readDelta) +
                           static_cast<double>(writeDelta);
        decision.ioValid = true;
    }
}

static CandidateBuildResult BuildResourceCandidateDecisions(
    KillMode mode, DWORD sampleMs, int limit, HANDLE cancelEvent = nullptr) {
    CandidateBuildResult result;
    limit = std::clamp(limit, 1, 10);

    SamplePair sample = CaptureSamplePair(sampleMs, cancelEvent);
    result.diagnostics = sample.diagnostics;
    result.cancelled = sample.diagnostics.cancelled;
    if (!sample.success || result.cancelled) {
        return result;
    }

    std::unordered_map<DWORD, const ProcSnapshot*> beforeByPid;
    beforeByPid.reserve(sample.before.size());
    for (const auto& process : sample.before) {
        beforeByPid[process.identity.pid] = &process;
    }

    for (const auto& after : sample.after) {
        auto found = beforeByPid.find(after.identity.pid);
        if (found == beforeByPid.end()) {
            continue;
        }
        const ProcSnapshot& before = *found->second;
        if (!SameSnapshotIdentity(before, after)) {
            continue;
        }

        KillDecision decision;
        decision.identity = after.identity;
        decision.mode = mode;
        decision.sampleMs = sampleMs;
        decision.memoryMb = after.memoryValid
            ? static_cast<double>(after.workingSetBytes) / (1024.0 * 1024.0)
            : 0.0;
        decision.memoryValid = after.memoryValid;
        PopulateDecisionMetrics(decision, before, after, sample);

        switch (mode) {
        case KillMode::TopCpu:
            if (!decision.cpuValid || decision.cpuPercent <= 0.0) continue;
            decision.metricValue = decision.cpuPercent;
            decision.metricName =
                L"CPU usage during sample, normalized across all logical processors (%)";
            decision.reason =
                L"Top CPU mode ranked verified process CPU deltas.";
            break;
        case KillMode::TopPagefaults:
            if (!decision.pageFaultValid || decision.pageFaultDelta <= 0.0) continue;
            decision.metricValue = decision.pageFaultDelta;
            decision.metricName = L"Page-fault delta during sample";
            decision.reason =
                L"Top page-fault mode ranked verified page-fault deltas.";
            break;
        case KillMode::TopIo:
            if (!decision.ioValid || decision.ioBytes <= 0.0) continue;
            decision.metricValue = decision.ioBytes;
            decision.metricName =
                L"Process read/write I/O during sample (MB)";
            decision.reason =
                L"Top I/O mode ranked verified process read/write transfer deltas.";
            break;
        default:
            continue;
        }

        result.candidates.push_back(std::move(decision));
    }

    std::sort(result.candidates.begin(), result.candidates.end(),
              [](const KillDecision& left, const KillDecision& right) {
                  return left.metricValue > right.metricValue;
              });
    if (static_cast<int>(result.candidates.size()) > limit) {
        result.candidates.resize(static_cast<size_t>(limit));
    }
    return result;
}

static CandidateBuildResult BuildAggregateSuperScoreCandidates(
    DWORD sampleMs, int limit, HANDLE cancelEvent = nullptr) {
    CandidateBuildResult result;
    limit = std::clamp(limit, 1, 10);

    SamplePair sample = CaptureSamplePair(sampleMs, cancelEvent);
    result.diagnostics = sample.diagnostics;
    result.cancelled = sample.diagnostics.cancelled;
    if (!sample.success || result.cancelled) {
        return result;
    }

    std::unordered_map<DWORD, const ProcSnapshot*> beforeByPid;
    beforeByPid.reserve(sample.before.size());
    for (const auto& process : sample.before) {
        beforeByPid[process.identity.pid] = &process;
    }

    std::unordered_map<std::wstring, size_t> groupByPath;

    for (const auto& after : sample.after) {
        auto foundBefore = beforeByPid.find(after.identity.pid);
        if (foundBefore == beforeByPid.end()) {
            continue;
        }
        const ProcSnapshot& before = *foundBefore->second;
        if (!SameSnapshotIdentity(before, after)) {
            continue;
        }

        KillDecision processDecision;
        processDecision.identity = after.identity;
        PopulateDecisionMetrics(processDecision, before, after, sample);
        if (!processDecision.cpuValid && !processDecision.pageFaultValid &&
            !processDecision.ioValid && !processDecision.memoryValid) {
            continue;
        }

        std::wstring key = NormalizePath(after.identity.processPath);
        size_t groupIndex;
        auto foundGroup = groupByPath.find(key);
        if (foundGroup == groupByPath.end()) {
            KillDecision group;
            group.identity = after.identity;
            group.mode = KillMode::AggregateSuperScore;
            group.sampleMs = sampleMs;
            group.terminateGroup = true;
            group.reason =
                L"Aggregate mode grouped verified processes by executable path and combined CPU, page-fault, I/O, and memory activity.";
            group.metricName = L"Weighted aggregate score";
            result.candidates.push_back(std::move(group));
            groupIndex = result.candidates.size() - 1;
            groupByPath.emplace(std::move(key), groupIndex);
        } else {
            groupIndex = foundGroup->second;
        }

        KillDecision& group = result.candidates[groupIndex];
        group.targetIdentities.push_back(after.identity);
        if (processDecision.cpuValid) {
            group.cpuPercent += processDecision.cpuPercent;
            group.cpuValid = true;
        }
        if (processDecision.pageFaultValid) {
            group.pageFaultDelta += processDecision.pageFaultDelta;
            group.pageFaultValid = true;
        }
        if (processDecision.ioValid) {
            group.ioBytes += processDecision.ioBytes;
            group.ioValid = true;
        }
        if (processDecision.memoryValid) {
            group.memoryMb += processDecision.memoryMb;
            group.memoryValid = true;
        }

    }

    double maxCpu = 0.0;
    double maxFaults = 0.0;
    double maxIo = 0.0;
    double maxMemory = 0.0;
    for (const auto& group : result.candidates) {
        maxCpu = std::max(maxCpu, group.cpuPercent);
        maxFaults = std::max(maxFaults, group.pageFaultDelta);
        maxIo = std::max(maxIo, group.ioBytes);
        maxMemory = std::max(maxMemory, group.memoryMb);
    }

    int cpuWeight;
    int faultWeight;
    int ioWeight;
    int memoryWeight;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        cpuWeight = g_superCpuWeight;
        faultWeight = g_superPageFaultWeight;
        ioWeight = g_superIoWeight;
        memoryWeight = g_superMemoryWeight;
    }

    int totalWeight = 0;
    if (maxCpu > 0.0) totalWeight += cpuWeight;
    if (maxFaults > 0.0) totalWeight += faultWeight;
    if (maxIo > 0.0) totalWeight += ioWeight;
    if (maxMemory > 0.0) totalWeight += memoryWeight;
    if (totalWeight <= 0) {
        cpuWeight = 45;
        faultWeight = 20;
        ioWeight = 25;
        memoryWeight = 10;
        if (maxCpu > 0.0) totalWeight += cpuWeight;
        if (maxFaults > 0.0) totalWeight += faultWeight;
        if (maxIo > 0.0) totalWeight += ioWeight;
        if (maxMemory > 0.0) totalWeight += memoryWeight;
    }
    if (totalWeight <= 0) {
        result.candidates.clear();
        return result;
    }

    for (auto& group : result.candidates) {
        double score = 0.0;
        if (maxCpu > 0.0)
            score += cpuWeight * NormalizeMetric(group.cpuPercent, maxCpu);
        if (maxFaults > 0.0)
            score += faultWeight * NormalizeMetric(group.pageFaultDelta, maxFaults);
        if (maxIo > 0.0)
            score += ioWeight * NormalizeMetric(group.ioBytes, maxIo);
        if (maxMemory > 0.0)
            score += memoryWeight * NormalizeMetric(group.memoryMb, maxMemory);
        group.superScore = score / static_cast<double>(totalWeight) * 100.0;
        group.metricValue = group.superScore;
    }

    result.candidates.erase(
        std::remove_if(result.candidates.begin(), result.candidates.end(),
                       [](const KillDecision& decision) {
                           return decision.targetIdentities.empty() ||
                                  decision.metricValue <= 0.0;
                       }),
        result.candidates.end());

    std::sort(result.candidates.begin(), result.candidates.end(),
              [](const KillDecision& left, const KillDecision& right) {
                  return left.metricValue > right.metricValue;
              });
    if (static_cast<int>(result.candidates.size()) > limit) {
        result.candidates.resize(static_cast<size_t>(limit));
    }
    return result;
}

// ------------------------ Window/process details ------------------------

static std::wstring GetWindowTextSafe(HWND window) {
    if (!window || !IsWindow(window)) {
        return L"";
    }
    int length = GetWindowTextLengthW(window);
    if (length <= 0) {
        return L"";
    }
    std::wstring title(static_cast<size_t>(length) + 1, L'\0');
    int copied = GetWindowTextW(window, title.data(), length + 1);
    if (copied <= 0) {
        return L"";
    }
    title.resize(static_cast<size_t>(copied));
    return title;
}

static std::wstring GetClassNameSafe(HWND window) {
    if (!window || !IsWindow(window)) {
        return L"";
    }
    WCHAR buffer[256]{};
    int copied = GetClassNameW(window, buffer, ARRAYSIZE(buffer));
    return copied > 0 ? std::wstring(buffer, static_cast<size_t>(copied)) : L"";
}

static std::vector<ProcessIdentity> DecisionTargets(const KillDecision& decision) {
    if (decision.terminateGroup) {
        return decision.targetIdentities;
    }
    return decision.identity.pid
        ? std::vector<ProcessIdentity>{decision.identity}
        : std::vector<ProcessIdentity>{};
}

static std::wstring BuildPidListText(
    const std::vector<ProcessIdentity>& identities) {
    std::wstring text;
    for (size_t index = 0; index < identities.size(); index++) {
        if (index) text += L", ";
        text += std::to_wstring(identities[index].pid);
    }
    return text.empty() ? L"<none>" : text;
}

static std::wstring DecisionSelectionKey(const KillDecision& decision) {
    if (decision.terminateGroup) {
        return L"group|" + NormalizePath(decision.identity.processPath);
    }
    return L"pid|" + std::to_wstring(decision.identity.pid) + L"|" +
           std::to_wstring(decision.identity.creationTime100ns) + L"|" +
           NormalizePath(decision.identity.processPath);
}

// ------------------------ Candidate chooser ------------------------

struct CandidateDialogState {
    std::vector<KillDecision>* candidates{};
    SamplingDiagnostics diagnostics{};
    int selectedIndex = -1;
    std::wstring selectedKey;
    HWND hList{};
    HWND hRefresh{};
    HWND hLive{};
    HWND hPollEdit{};
    HWND hStatus{};
    HWND hTerminate{};
    KillMode mode = KillMode::Foreground;
    DWORD sampleMs = 3000;
    int candidateLimit = 5;
    DWORD pollMs = 5000;
    bool live{};
    bool darkUi = true;
    bool refreshing{};
    bool closing{};
    HBRUSH hDarkBrush{};
    HBRUSH hDarkEditBrush{};
    int sortColumn = 3;
    bool sortDescending = true;
    HANDLE hRefreshCancelEvent{};
    HANDLE hRefreshThread{};
    UINT dpi = 96;
};

struct RefreshWorkerInput {
    HWND hwnd{};
    KillMode mode{};
    DWORD sampleMs{};
    int candidateLimit{};
    HANDLE cancelEvent{};
};

struct RefreshWorkerResult {
    CandidateBuildResult build;
};

static int ScaleForDpi(int value, UINT dpi) {
    return MulDiv(value, static_cast<int>(dpi), 96);
}

static UINT EffectiveDpi(HWND window = nullptr) {
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (user32) {
        using GetDpiForWindow_t = UINT(WINAPI*)(HWND);
        auto getDpiForWindow = reinterpret_cast<GetDpiForWindow_t>(
            GetProcAddress(user32, "GetDpiForWindow"));
        if (window && getDpiForWindow) {
            UINT dpi = getDpiForWindow(window);
            if (dpi) return dpi;
        }
    }

    HDC dc = GetDC(nullptr);
    UINT dpi = dc ? static_cast<UINT>(GetDeviceCaps(dc, LOGPIXELSX)) : 96;
    if (dc) ReleaseDC(nullptr, dc);
    return dpi ? dpi : 96;
}

static double CandidateMetricForColumn(const KillDecision& decision,
                                       int column) {
    switch (column) {
    case 2:
        return decision.terminateGroup
            ? static_cast<double>(decision.targetIdentities.size())
            : static_cast<double>(decision.identity.pid);
    case 3:
        return decision.mode == KillMode::AggregateSuperScore
            ? decision.superScore
            : decision.metricValue;
    case 4: return decision.cpuPercent;
    case 5: return decision.pageFaultDelta;
    case 6: return decision.ioBytes;
    case 7: return decision.memoryMb;
    default: return 0.0;
    }
}

static bool CandidateHasMetricForColumn(const KillDecision& decision,
                                        int column) {
    switch (column) {
    case 3: return true;
    case 4: return decision.cpuValid;
    case 5: return decision.pageFaultValid;
    case 6: return decision.ioValid;
    case 7: return decision.memoryValid;
    default: return true;
    }
}

static std::wstring CandidateTextForColumn(const KillDecision& decision,
                                           int row, int column) {
    switch (column) {
    case 0:
        return std::to_wstring(row + 1);
    case 1:
        return decision.terminateGroup
            ? L"APP " + decision.identity.exeName
            : decision.identity.exeName;
    case 2:
        return decision.terminateGroup
            ? std::to_wstring(decision.targetIdentities.size())
            : std::to_wstring(decision.identity.pid);
    case 3:
        return FormatPrimaryMetric(decision);
    case 4:
        return decision.cpuValid ? FormatDouble(decision.cpuPercent) : L"—";
    case 5:
        return decision.pageFaultValid
            ? FormatInteger(decision.pageFaultDelta)
            : L"—";
    case 6:
        return decision.ioValid ? FormatMb(decision.ioBytes, true) : L"—";
    case 7:
        return decision.memoryValid ? FormatDouble(decision.memoryMb) : L"—";
    case 8:
        return decision.identity.processPath;
    }
    return L"";
}

static double MaxMetricForColumn(const CandidateDialogState* state,
                                 int column) {
    if (!state || !state->candidates) {
        return 0.0;
    }
    double maximum = 0.0;
    for (const auto& decision : *state->candidates) {
        if (CandidateHasMetricForColumn(decision, column)) {
            maximum = std::max(maximum,
                               CandidateMetricForColumn(decision, column));
        }
    }
    return maximum;
}

static COLORREF DarkBg() { return RGB(30, 30, 30); }
static COLORREF DarkPanelBg() { return RGB(38, 38, 38); }
static COLORREF DarkText() { return RGB(235, 235, 235); }
static COLORREF TargetSelectedBg() { return RGB(0, 95, 170); }

static void TrySetDarkTheme(HWND window) {
    HMODULE uxTheme = GetModuleHandleW(L"uxtheme.dll");
    if (!uxTheme) return;
    using SetWindowTheme_t = HRESULT(WINAPI*)(HWND, LPCWSTR, LPCWSTR);
    auto setWindowTheme = reinterpret_cast<SetWindowTheme_t>(
        GetProcAddress(uxTheme, "SetWindowTheme"));
    if (setWindowTheme) {
        setWindowTheme(window, L"DarkMode_Explorer", nullptr);
    }
}

static COLORREF SeverityColor(double normalized) {
    normalized = std::clamp(normalized, 0.0, 1.0);
    int red;
    int green;
    int blue = 80;
    if (normalized < 0.5) {
        double t = normalized / 0.5;
        red = static_cast<int>(220.0 * t);
        green = 210;
    } else {
        double t = (normalized - 0.5) / 0.5;
        red = 220;
        green = static_cast<int>(210.0 * (1.0 - t));
    }
    red = (red + 400) / 3;
    green = (green + 400) / 3;
    blue = (blue + 400) / 3;
    return RGB(red, green, blue);
}

static void SortCandidatesForColumn(CandidateDialogState* state) {
    if (!state || !state->candidates || state->sortColumn == 0) {
        return;
    }

    int column = state->sortColumn;
    bool descending = state->sortDescending;
    std::sort(state->candidates->begin(), state->candidates->end(),
              [&](const KillDecision& left, const KillDecision& right) {
        if (column == 1 || column == 8) {
            std::wstring leftText = CandidateTextForColumn(left, 0, column);
            std::wstring rightText = CandidateTextForColumn(right, 0, column);
            int comparison = _wcsicmp(leftText.c_str(), rightText.c_str());
            return descending ? comparison > 0 : comparison < 0;
        }

        bool leftValid = CandidateHasMetricForColumn(left, column);
        bool rightValid = CandidateHasMetricForColumn(right, column);
        if (leftValid != rightValid) {
            return leftValid;
        }

        double leftValue = CandidateMetricForColumn(left, column);
        double rightValue = CandidateMetricForColumn(right, column);
        if (leftValue == rightValue) {
            int comparison = _wcsicmp(left.identity.exeName.c_str(),
                                      right.identity.exeName.c_str());
            return comparison < 0;
        }
        return descending ? leftValue > rightValue : leftValue < rightValue;
    });
}

static void InsertListColumn(HWND list, int index, PCWSTR title, int width) {
    LVCOLUMNW column{};
    column.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    column.pszText = const_cast<PWSTR>(title);
    column.cx = width;
    column.iSubItem = index;
    ListView_InsertColumn(list, index, &column);
}

static int FindCandidateByKey(const CandidateDialogState* state,
                              const std::wstring& key) {
    if (!state || !state->candidates || key.empty()) {
        return -1;
    }
    for (int index = 0; index < static_cast<int>(state->candidates->size());
         index++) {
        if (DecisionSelectionKey((*state->candidates)[index]) == key) {
            return index;
        }
    }
    return -1;
}

static void CaptureCurrentSelection(CandidateDialogState* state) {
    if (!state || !state->hList || !state->candidates) return;
    int selected = ListView_GetNextItem(state->hList, -1, LVNI_SELECTED);
    if (selected >= 0 && selected < static_cast<int>(state->candidates->size())) {
        state->selectedKey =
            DecisionSelectionKey((*state->candidates)[selected]);
    }
}

static void PopulateCandidateList(CandidateDialogState* state,
                                  bool preserveSelection) {
    if (!state || !state->hList || !state->candidates) {
        return;
    }

    SortCandidatesForColumn(state);
    ListView_DeleteAllItems(state->hList);

    for (int row = 0; row < static_cast<int>(state->candidates->size()); row++) {
        std::wstring first =
            CandidateTextForColumn((*state->candidates)[row], row, 0);
        LVITEMW item{};
        item.mask = LVIF_TEXT;
        item.iItem = row;
        item.iSubItem = 0;
        item.pszText = first.data();
        ListView_InsertItem(state->hList, &item);

        for (int column = 1; column <= 8; column++) {
            std::wstring text =
                CandidateTextForColumn((*state->candidates)[row], row, column);
            ListView_SetItemText(state->hList, row, column, text.data());
        }
    }

    int selected = preserveSelection
        ? FindCandidateByKey(state, state->selectedKey)
        : (state->candidates->empty() ? -1 : 0);
    if (selected >= 0) {
        ListView_SetItemState(state->hList, selected,
                              LVIS_SELECTED | LVIS_FOCUSED,
                              LVIS_SELECTED | LVIS_FOCUSED);
        ListView_EnsureVisible(state->hList, selected, FALSE);
        state->selectedKey =
            DecisionSelectionKey((*state->candidates)[selected]);
    } else {
        state->selectedKey.clear();
    }

    EnableWindow(state->hTerminate,
                 !state->refreshing && selected >= 0);
    InvalidateRect(state->hList, nullptr, TRUE);
}

static DWORD ReadPollInterval(CandidateDialogState* state) {
    if (!state || !state->hPollEdit) {
        return 5000;
    }
    WCHAR buffer[32]{};
    GetWindowTextW(state->hPollEdit, buffer, ARRAYSIZE(buffer));
    wchar_t* end = nullptr;
    long value = wcstol(buffer, &end, 10);
    if (!end || *end != L'\0') value = 5000;
    value = std::clamp(value, 1000L, 60000L);
    return static_cast<DWORD>(value);
}

static void SetStatus(CandidateDialogState* state, PCWSTR text) {
    if (state && state->hStatus) {
        SetWindowTextW(state->hStatus, text);
        UpdateWindow(state->hStatus);
    }
}

static std::wstring DiagnosticsStatus(const CandidateBuildResult& build) {
    if (build.cancelled) {
        return L"Refresh cancelled.";
    }
    std::wstring status = L"Updated: " +
        std::to_wstring(build.candidates.size()) + L" candidate(s)";
    if (build.diagnostics.inaccessible ||
        build.diagnostics.identityFailures) {
        status += L"; " +
            std::to_wstring(build.diagnostics.inaccessible +
                            build.diagnostics.identityFailures) +
            L" process inspection attempt(s) failed";
    }
    return status;
}

static CandidateBuildResult BuildCandidatesForMode(KillMode mode,
                                                    DWORD sampleMs,
                                                    int candidateLimit,
                                                    HANDLE cancelEvent) {
    return mode == KillMode::AggregateSuperScore
        ? BuildAggregateSuperScoreCandidates(sampleMs, candidateLimit,
                                             cancelEvent)
        : BuildResourceCandidateDecisions(mode, sampleMs, candidateLimit,
                                          cancelEvent);
}

static DWORD WINAPI RefreshWorkerThread(LPVOID parameter) {
    std::unique_ptr<RefreshWorkerInput> input(
        static_cast<RefreshWorkerInput*>(parameter));
    auto result = std::make_unique<RefreshWorkerResult>();
    result->build = BuildCandidatesForMode(input->mode, input->sampleMs,
                                           input->candidateLimit,
                                           input->cancelEvent);

    if (!PostMessageW(input->hwnd, WM_CANDIDATE_REFRESH_COMPLETE, 0,
                      reinterpret_cast<LPARAM>(result.get()))) {
        return 0;
    }
    result.release();
    return 0;
}

static void StartCandidateRefresh(HWND window, CandidateDialogState* state) {
    if (!state || state->refreshing || state->closing ||
        !state->candidates) {
        return;
    }

    state->refreshing = true;
    ResetEvent(state->hRefreshCancelEvent);
    CaptureCurrentSelection(state);
    EnableWindow(state->hRefresh, FALSE);
    EnableWindow(state->hTerminate, FALSE);
    SetStatus(state, L"Sampling in background...");

    auto input = std::make_unique<RefreshWorkerInput>();
    input->hwnd = window;
    input->mode = state->mode;
    input->sampleMs = state->sampleMs;
    input->candidateLimit = state->candidateLimit;
    input->cancelEvent = state->hRefreshCancelEvent;

    state->hRefreshThread =
        CreateThread(nullptr, 0, RefreshWorkerThread, input.get(), 0, nullptr);
    if (!state->hRefreshThread) {
        state->refreshing = false;
        EnableWindow(state->hRefresh, TRUE);
        EnableWindow(state->hTerminate,
                     !state->candidates->empty() &&
                     !state->selectedKey.empty());
        SetStatus(state, L"Could not start refresh worker.");
        return;
    }
    input.release();
}

static LRESULT CALLBACK CandidateChooserProc(HWND window, UINT message,
                                              WPARAM wParam, LPARAM lParam) {
    CandidateDialogState* state = reinterpret_cast<CandidateDialogState*>(
        GetWindowLongPtrW(window, GWLP_USERDATA));

    switch (message) {
    case WM_CREATE: {
        auto create = reinterpret_cast<CREATESTRUCTW*>(lParam);
        state = static_cast<CandidateDialogState*>(create->lpCreateParams);
        SetWindowLongPtrW(window, GWLP_USERDATA,
                          reinterpret_cast<LONG_PTR>(state));
        state->dpi = EffectiveDpi(window);
        auto S = [&](int value) { return ScaleForDpi(value, state->dpi); };

        if (state->darkUi) {
            state->hDarkBrush = CreateSolidBrush(DarkBg());
            state->hDarkEditBrush = CreateSolidBrush(DarkPanelBg());
            TrySetDarkTheme(window);
        }

        INITCOMMONCONTROLSEX controls{sizeof(controls), ICC_LISTVIEW_CLASSES};
        InitCommonControlsEx(&controls);
        HFONT font = static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));

        HWND heading = CreateWindowW(
            L"STATIC",
            L"Select a target. Click a column to sort. Refreshes preserve selection by process identity.",
            WS_CHILD | WS_VISIBLE, S(12), S(10), S(1420), S(22), window,
            nullptr, GetModuleHandleW(nullptr), nullptr);
        if (heading && font) {
            SendMessageW(heading, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE);
        }

        state->hList = CreateWindowExW(
            WS_EX_CLIENTEDGE, WC_LISTVIEWW, nullptr,
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | LVS_REPORT |
                LVS_SINGLESEL | LVS_SHOWSELALWAYS,
            S(12), S(38), S(1420), S(365), window,
            reinterpret_cast<HMENU>(IDC_CANDIDATE_LIST),
            GetModuleHandleW(nullptr), nullptr);

        if (state->hList) {
            ListView_SetExtendedListViewStyle(
                state->hList, LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER |
                                  LVS_EX_FULLROWSELECT);
            if (state->darkUi) {
                TrySetDarkTheme(state->hList);
                ListView_SetBkColor(state->hList, DarkBg());
                ListView_SetTextBkColor(state->hList, DarkBg());
                ListView_SetTextColor(state->hList, DarkText());
            }
            if (font) {
                SendMessageW(state->hList, WM_SETFONT,
                             reinterpret_cast<WPARAM>(font), TRUE);
            }
            InsertListColumn(state->hList, 0, L"#", S(42));
            InsertListColumn(state->hList, 1, L"Target", S(260));
            InsertListColumn(state->hList, 2, L"PID(s)", S(90));
            InsertListColumn(state->hList, 3, L"Primary", S(105));
            InsertListColumn(state->hList, 4, L"CPU total %", S(115));
            InsertListColumn(state->hList, 5, L"Page faults", S(135));
            InsertListColumn(state->hList, 6, L"I/O MB", S(115));
            InsertListColumn(state->hList, 7, L"Mem MB", S(115));
            InsertListColumn(state->hList, 8, L"Path", S(640));
        }

        state->hRefresh = CreateWindowW(
            L"BUTTON", L"Refresh", WS_CHILD | WS_VISIBLE,
            S(12), S(420), S(120), S(32), window,
            reinterpret_cast<HMENU>(IDC_REFRESH_BUTTON),
            GetModuleHandleW(nullptr), nullptr);
        state->hLive = CreateWindowW(
            L"BUTTON", L"Live update", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
            S(150), S(426), S(155), S(24), window,
            reinterpret_cast<HMENU>(IDC_LIVE_CHECK),
            GetModuleHandleW(nullptr), nullptr);
        HWND pollLabel = CreateWindowW(
            L"STATIC", L"Poll ms:", WS_CHILD | WS_VISIBLE,
            S(320), S(430), S(60), S(20), window, nullptr,
            GetModuleHandleW(nullptr), nullptr);
        state->hPollEdit = CreateWindowExW(
            WS_EX_CLIENTEDGE, L"EDIT", nullptr,
            WS_CHILD | WS_VISIBLE | ES_NUMBER,
            S(386), S(424), S(90), S(28), window,
            reinterpret_cast<HMENU>(IDC_POLL_EDIT),
            GetModuleHandleW(nullptr), nullptr);
        state->hStatus = CreateWindowW(
            L"STATIC", L"Ready", WS_CHILD | WS_VISIBLE,
            S(492), S(430), S(550), S(20), window,
            reinterpret_cast<HMENU>(IDC_STATUS_STATIC),
            GetModuleHandleW(nullptr), nullptr);
        state->hTerminate = CreateWindowW(
            L"BUTTON", L"Terminate selected",
            WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
            S(1070), S(420), S(190), S(32), window,
            reinterpret_cast<HMENU>(IDOK), GetModuleHandleW(nullptr), nullptr);
        HWND cancel = CreateWindowW(
            L"BUTTON", L"Cancel", WS_CHILD | WS_VISIBLE,
            S(1276), S(420), S(155), S(32), window,
            reinterpret_cast<HMENU>(IDCANCEL), GetModuleHandleW(nullptr), nullptr);

        if (!state->hList || !state->hRefresh || !state->hLive ||
            !state->hPollEdit || !state->hStatus || !state->hTerminate ||
            !cancel) {
            Wh_Log(L"A required chooser control could not be created (error=%lu).",
                   GetLastError());
            if (state->hDarkBrush) {
                DeleteObject(state->hDarkBrush);
                state->hDarkBrush = nullptr;
            }
            if (state->hDarkEditBrush) {
                DeleteObject(state->hDarkEditBrush);
                state->hDarkEditBrush = nullptr;
            }
            return -1;
        }

        HWND childControls[] = {heading, state->hRefresh, state->hLive,
                                pollLabel, state->hPollEdit, state->hStatus,
                                state->hTerminate, cancel};
        for (HWND control : childControls) {
            if (control && font) {
                SendMessageW(control, WM_SETFONT,
                             reinterpret_cast<WPARAM>(font), TRUE);
            }
            if (state->darkUi && control) {
                TrySetDarkTheme(control);
            }
        }

        WCHAR pollText[32]{};
        swprintf(pollText, ARRAYSIZE(pollText), L"%lu", state->pollMs);
        SetWindowTextW(state->hPollEdit, pollText);
        PopulateCandidateList(state, false);

        CandidateBuildResult initial;
        initial.candidates = *state->candidates;
        initial.diagnostics = state->diagnostics;
        SetStatus(state, DiagnosticsStatus(initial).c_str());

        if (state->live && state->hLive) {
            SendMessageW(state->hLive, BM_SETCHECK, BST_CHECKED, 0);
            SetTimer(window, TIMER_LIVE_REFRESH, state->pollMs, nullptr);
        }
        return 0;
    }

    case WM_COMMAND: {
        WORD id = LOWORD(wParam);
        if (id == IDOK) {
            if (!state || state->refreshing || !state->candidates) return 0;
            int selected =
                ListView_GetNextItem(state->hList, -1, LVNI_SELECTED);
            if (selected < 0 ||
                selected >= static_cast<int>(state->candidates->size())) {
                ShowTopmostMessage(L"Select a target first.",
                                   L"Resource-Aware Task Terminator",
                                   MB_OK | MB_ICONINFORMATION);
                return 0;
            }
            state->selectedIndex = selected;
            DestroyWindow(window);
            return 0;
        }
        if (id == IDCANCEL) {
            if (state) state->selectedIndex = -1;
            DestroyWindow(window);
            return 0;
        }
        if (id == IDC_REFRESH_BUTTON) {
            StartCandidateRefresh(window, state);
            return 0;
        }
        if (id == IDC_LIVE_CHECK) {
            bool checked = state && state->hLive &&
                SendMessageW(state->hLive, BM_GETCHECK, 0, 0) == BST_CHECKED;
            if (checked) {
                state->pollMs = ReadPollInterval(state);
                SetTimer(window, TIMER_LIVE_REFRESH, state->pollMs, nullptr);
                SetStatus(state, L"Live update enabled.");
            } else {
                KillTimer(window, TIMER_LIVE_REFRESH);
                SetStatus(state, L"Live update disabled.");
            }
            return 0;
        }
        break;
    }

    case WM_CANDIDATE_REFRESH_COMPLETE: {
        std::unique_ptr<RefreshWorkerResult> result(
            reinterpret_cast<RefreshWorkerResult*>(lParam));
        if (!state) return 0;
        if (state->hRefreshThread) {
            CloseHandle(state->hRefreshThread);
            state->hRefreshThread = nullptr;
        }
        state->refreshing = false;
        EnableWindow(state->hRefresh, TRUE);

        if (!state->closing && !result->build.cancelled) {
            CaptureCurrentSelection(state);
            std::wstring status = DiagnosticsStatus(result->build);
            *state->candidates = std::move(result->build.candidates);
            state->diagnostics = result->build.diagnostics;
            PopulateCandidateList(state, true);
            SetStatus(state, status.c_str());
        } else if (!state->closing) {
            SetStatus(state, L"Refresh cancelled.");
            PopulateCandidateList(state, true);
        }
        return 0;
    }

    case WM_TIMER:
        if (wParam == TIMER_LIVE_REFRESH) {
            StartCandidateRefresh(window, state);
            return 0;
        }
        break;

    case WM_NOTIFY: {
        auto header = reinterpret_cast<NMHDR*>(lParam);
        if (!state || !header || header->idFrom != IDC_CANDIDATE_LIST) {
            break;
        }

        if (header->code == LVN_ITEMCHANGED) {
            auto change = reinterpret_cast<NMLISTVIEW*>(lParam);
            if (change->iItem >= 0 &&
                (change->uNewState & LVIS_SELECTED) && state->candidates &&
                change->iItem < static_cast<int>(state->candidates->size())) {
                state->selectedKey = DecisionSelectionKey(
                    (*state->candidates)[change->iItem]);
                EnableWindow(state->hTerminate, !state->refreshing);
            }
            return 0;
        }

        if (header->code == LVN_COLUMNCLICK) {
            auto column = reinterpret_cast<NMLISTVIEW*>(lParam);
            if (column->iSubItem == 0) {
                SetStatus(state, L"The rank column is display-only.");
                return 0;
            }
            CaptureCurrentSelection(state);
            if (state->sortColumn == column->iSubItem) {
                state->sortDescending = !state->sortDescending;
            } else {
                state->sortColumn = column->iSubItem;
                state->sortDescending = true;
            }
            PopulateCandidateList(state, true);
            return 0;
        }

        if (header->code == NM_DBLCLK) {
            auto activate = reinterpret_cast<NMITEMACTIVATE*>(lParam);
            if (!state->refreshing && activate->iItem >= 0 &&
                state->candidates &&
                activate->iItem < static_cast<int>(state->candidates->size())) {
                state->selectedIndex = activate->iItem;
                DestroyWindow(window);
            }
            return 0;
        }

        if (header->code == NM_CUSTOMDRAW) {
            auto draw = reinterpret_cast<NMLVCUSTOMDRAW*>(lParam);
            if (draw->nmcd.dwDrawStage == CDDS_PREPAINT) {
                return CDRF_NOTIFYITEMDRAW;
            }
            if (draw->nmcd.dwDrawStage == CDDS_ITEMPREPAINT) {
                return CDRF_NOTIFYSUBITEMDRAW;
            }
            if (draw->nmcd.dwDrawStage ==
                (CDDS_ITEMPREPAINT | CDDS_SUBITEM)) {
                int column = draw->iSubItem;
                int row = static_cast<int>(draw->nmcd.dwItemSpec);
                if (!state->candidates || row < 0 ||
                    row >= static_cast<int>(state->candidates->size())) {
                    break;
                }

                bool selected =
                    (ListView_GetItemState(state->hList, row, LVIS_SELECTED) &
                     LVIS_SELECTED) != 0;
                if (selected) {
                    draw->clrTextBk = TargetSelectedBg();
                    draw->clrText = RGB(255, 255, 255);
                    return CDRF_NEWFONT;
                }

                const KillDecision& decision = (*state->candidates)[row];
                if (column >= 3 && column <= 7 &&
                    CandidateHasMetricForColumn(decision, column)) {
                    double maximum = MaxMetricForColumn(state, column);
                    double value = CandidateMetricForColumn(decision, column);
                    draw->clrTextBk = SeverityColor(
                        maximum > 0.0 ? value / maximum : 0.0);
                    draw->clrText = RGB(0, 0, 0);
                    return CDRF_NEWFONT;
                }

                if (state->darkUi) {
                    draw->clrTextBk = DarkBg();
                    draw->clrText = DarkText();
                    return CDRF_NEWFONT;
                }
            }
        }
        break;
    }

    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLOREDIT:
        if (state && state->darkUi) {
            HDC dc = reinterpret_cast<HDC>(wParam);
            SetTextColor(dc, DarkText());
            SetBkColor(dc, message == WM_CTLCOLOREDIT
                               ? DarkPanelBg()
                               : DarkBg());
            return reinterpret_cast<LRESULT>(
                message == WM_CTLCOLOREDIT
                    ? state->hDarkEditBrush
                    : state->hDarkBrush);
        }
        break;

    case WM_ERASEBKGND:
        if (state && state->darkUi) {
            HDC dc = reinterpret_cast<HDC>(wParam);
            RECT rectangle{};
            GetClientRect(window, &rectangle);
            FillRect(dc, &rectangle, state->hDarkBrush);
            return 1;
        }
        break;

    case WM_CLOSE:
        if (state) state->selectedIndex = -1;
        DestroyWindow(window);
        return 0;

    case WM_DESTROY:
        if (state) {
            state->closing = true;
            KillTimer(window, TIMER_LIVE_REFRESH);
            if (state->hRefreshCancelEvent) {
                SetEvent(state->hRefreshCancelEvent);
            }
            if (state->hRefreshThread) {
                WaitForSingleObject(state->hRefreshThread, INFINITE);
                CloseHandle(state->hRefreshThread);
                state->hRefreshThread = nullptr;
            }

            MSG pending{};
            while (PeekMessageW(&pending, window,
                                WM_CANDIDATE_REFRESH_COMPLETE,
                                WM_CANDIDATE_REFRESH_COMPLETE, PM_REMOVE)) {
                delete reinterpret_cast<RefreshWorkerResult*>(pending.lParam);
            }

            if (state->hDarkBrush) {
                DeleteObject(state->hDarkBrush);
                state->hDarkBrush = nullptr;
            }
            if (state->hDarkEditBrush) {
                DeleteObject(state->hDarkEditBrush);
                state->hDarkEditBrush = nullptr;
            }
        }
        g_activeChooser.store(nullptr);
        return 0;
    }

    return DefWindowProcW(window, message, wParam, lParam);
}

static int ShowCandidateChooser(std::vector<KillDecision>& candidates,
                                const SamplingDiagnostics& diagnostics) {
    if (candidates.empty()) return -1;
    if (candidates.size() == 1) return 0;

    HINSTANCE instance = GetModuleHandleW(nullptr);
    const wchar_t* className =
        L"ResourceAwareTaskTerminator_CandidateChooser";

    WNDCLASSW windowClass{};
    windowClass.lpfnWndProc = CandidateChooserProc;
    windowClass.hInstance = instance;
    windowClass.lpszClassName = className;
    windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    if (!RegisterClassW(&windowClass) &&
        GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        Wh_Log(L"RegisterClassW failed for chooser (error=%lu).",
               GetLastError());
        return -1;
    }

    CandidateDialogState state;
    state.candidates = &candidates;
    state.diagnostics = diagnostics;
    state.mode = candidates[0].mode;
    state.sampleMs = candidates[0].sampleMs;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        state.candidateLimit = g_candidateCount;
        state.pollMs = g_livePollMs;
        state.live = g_liveUpdateDefault;
        state.darkUi = g_darkChooserUi;
    }
    state.hRefreshCancelEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!state.hRefreshCancelEvent) {
        Wh_Log(L"CreateEvent failed for chooser refresh cancellation.");
        return -1;
    }

    UINT dpi = EffectiveDpi();
    int clientWidth = ScaleForDpi(1460, dpi);
    int clientHeight = ScaleForDpi(500, dpi);
    RECT rectangle{0, 0, clientWidth, clientHeight};
    AdjustWindowRectEx(&rectangle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
                       FALSE, WS_EX_TOPMOST | WS_EX_TOOLWINDOW);
    int width = rectangle.right - rectangle.left;
    int height = rectangle.bottom - rectangle.top;

    HWND foreground = GetForegroundWindow();
    HMONITOR monitor = MonitorFromWindow(
        foreground, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    GetMonitorInfoW(monitor, &monitorInfo);
    RECT work = monitorInfo.rcWork;
    int x = work.left + ((work.right - work.left) - width) / 2;
    int y = work.top + ((work.bottom - work.top) - height) / 2;

    HWND window = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW, className,
        L"Select Resource-Aware Task Target",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        x, y, width, height, nullptr, nullptr, instance, &state);
    if (!window) {
        Wh_Log(L"CreateWindowExW failed for chooser (error=%lu).",
               GetLastError());
        CloseHandle(state.hRefreshCancelEvent);
        return -1;  // A chooser failure must never select candidate zero.
    }

    g_activeChooser.store(window);
    ShowWindow(window, SW_SHOW);
    SetForegroundWindow(window);

    MSG message{};
    while (IsWindow(window)) {
        BOOL result = GetMessageW(&message, nullptr, 0, 0);
        if (result == 0) {
            PostQuitMessage(static_cast<int>(message.wParam));
            break;
        }
        if (result < 0) {
            break;
        }
        if (!IsDialogMessageW(window, &message)) {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    }

    if (state.hRefreshCancelEvent) {
        CloseHandle(state.hRefreshCancelEvent);
    }
    return state.selectedIndex;
}

// ------------------------ Decision resolution and confirmation ------------------------

static bool ResolveTargetDecision(KillDecision& decision,
                                  std::wstring& failureMessage) {
    int candidateCount;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        decision.mode = g_killMode;
        decision.sampleMs = g_sampleMs;
        candidateCount = g_candidateCount;
    }

    if (decision.mode == KillMode::Foreground) {
        HWND foreground = GetForegroundWindow();
        if (!foreground || foreground == GetShellWindow()) {
            failureMessage = L"No eligible foreground window was found.";
            return false;
        }

        DWORD pid = 0;
        GetWindowThreadProcessId(foreground, &pid);
        if (!pid || !CaptureProcessIdentity(pid, decision.identity)) {
            failureMessage =
                L"The foreground process identity could not be captured.";
            return false;
        }
        if (IsProcessProtected(pid, decision.identity.exeName)) {
            failureMessage =
                L"The foreground process is protected or could not be verified.";
            return false;
        }

        decision.foregroundWindow = foreground;
        decision.windowTitle = GetWindowTextSafe(foreground);
        decision.windowClass = GetClassNameSafe(foreground);
        decision.reason =
            L"Foreground mode selected the verified process that owns the current foreground window.";
        decision.metricName = L"Foreground HWND";
        decision.metricValue = static_cast<double>(
            reinterpret_cast<ULONG_PTR>(foreground));
        return true;
    }

    CandidateBuildResult build = BuildCandidatesForMode(
        decision.mode, decision.sampleMs, candidateCount, g_hStopEvent);
    if (build.cancelled || CancellationRequested()) {
        failureMessage = L"Target sampling was cancelled.";
        return false;
    }
    if (build.candidates.empty()) {
        failureMessage =
            L"No eligible active target was found. The system may be idle, all candidates may be protected, or some processes may require elevation.";
        if (build.diagnostics.inaccessible ||
            build.diagnostics.identityFailures) {
            failureMessage += L"\n\nUninspectable processes: " +
                std::to_wstring(build.diagnostics.inaccessible +
                                build.diagnostics.identityFailures);
        }
        return false;
    }

    int selected = 0;
    if (build.candidates.size() > 1) {
        selected = ShowCandidateChooser(build.candidates, build.diagnostics);
        if (selected < 0 ||
            selected >= static_cast<int>(build.candidates.size())) {
            failureMessage.clear();  // User cancellation is not an error.
            return false;
        }
    }

    decision = build.candidates[static_cast<size_t>(selected)];
    return true;
}

static bool FilterDecisionTargets(KillDecision& decision,
                                  std::vector<std::wstring>& skipped) {
    std::vector<ProcessIdentity> original = DecisionTargets(decision);
    std::vector<ProcessIdentity> verified;
    verified.reserve(original.size());

    for (const auto& identity : original) {
        std::wstring reason;
        if (VerifySelectedIdentity(identity, reason)) {
            verified.push_back(identity);
        } else {
            skipped.push_back(L"PID " + std::to_wstring(identity.pid) +
                              L": " + reason);
        }
    }

    if (verified.empty()) {
        return false;
    }

    if (decision.terminateGroup) {
        decision.targetIdentities = std::move(verified);
        decision.identity = decision.targetIdentities.front();
    } else {
        decision.identity = verified.front();
    }
    return true;
}

static std::wstring BuildConfirmationMessage(const KillDecision& decision) {
    std::wstring message =
        L"Resource-Aware Task Terminator is about to force-terminate ";

    std::vector<ProcessIdentity> targets = DecisionTargets(decision);
    if (decision.terminateGroup) {
        message += L"an application group.\n\n";
        message += L"Executable: " + decision.identity.exeName + L"\n";
        message += L"Path: " + decision.identity.processPath + L"\n";
        message += L"Verified PIDs: " + BuildPidListText(targets) + L"\n";
        message += L"Process count: " + std::to_wstring(targets.size()) + L"\n";
    } else {
        message += L"a process.\n\n";
        message += L"PID: " + std::to_wstring(decision.identity.pid) + L"\n";
        message += L"Executable: " + decision.identity.exeName + L"\n";
        message += L"Path: " + decision.identity.processPath + L"\n";
    }

    message += L"\nDecision mode: ";
    message += KillModeName(decision.mode);
    message += L"\nReason: " + decision.reason + L"\n";
    if (decision.sampleMs) {
        message += L"Sample window: " + std::to_wstring(decision.sampleMs) +
                   L" ms\n";
    }
    message += L"Evidence: " + decision.metricName + L" = " +
               FormatPrimaryMetric(decision) + L"\n";

    if (decision.cpuValid) {
        message += L"CPU total %: " + FormatDouble(decision.cpuPercent) + L"\n";
    }
    if (decision.pageFaultValid) {
        message += L"Page-fault delta: " +
                   FormatInteger(decision.pageFaultDelta) + L"\n";
    }
    if (decision.ioValid) {
        message += L"Read/write I/O MB: " +
                   FormatMb(decision.ioBytes, true) + L"\n";
    }
    if (decision.memoryValid) {
        message += L"Working-set MB: " + FormatDouble(decision.memoryMb) + L"\n";
    }

    if (decision.foregroundWindow) {
        WCHAR handleText[32]{};
        swprintf(handleText, ARRAYSIZE(handleText), L"%p",
                 decision.foregroundWindow);
        message += L"\nForeground window:\nHWND: ";
        message += handleText;
        message += L"\nTitle: " +
            (decision.windowTitle.empty() ? L"<empty>" : decision.windowTitle);
        message += L"\nClass: " +
            (decision.windowClass.empty() ? L"<empty>" : decision.windowClass) +
            L"\n";
    }

    message +=
        L"\nEach PID will be re-verified by creation time and executable path immediately before termination. Unsaved data will be lost.";
    message += decision.terminateGroup
        ? L"\n\nTerminate all displayed verified PIDs now?"
        : L"\n\nTerminate this verified process now?";
    return message;
}

static bool ConfirmKillDecision(const KillDecision& decision) {
    bool confirm;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        confirm = g_confirmBeforeKill;
    }
    if (!confirm) return true;

    std::wstring message = BuildConfirmationMessage(decision);
    return MessageBoxW(nullptr, message.c_str(), L"Confirm Force Termination",
                       MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2 | MB_TOPMOST |
                           MB_SETFOREGROUND | MB_TASKMODAL) == IDYES;
}

// ------------------------ Kill workflow ------------------------

struct KillBusyReset {
    ~KillBusyReset() {
        g_killInProgress.store(false);
        std::lock_guard<std::mutex> lock(g_workerMutex);
        g_dwKillWorkerId = 0;
    }
};

static DWORD WINAPI KillWorkflowThread(LPVOID) {
    KillBusyReset reset;

    KillDecision decision;
    std::wstring resolutionFailure;
    if (!ResolveTargetDecision(decision, resolutionFailure)) {
        if (!resolutionFailure.empty() && !CancellationRequested()) {
            ShowTopmostMessage(resolutionFailure.c_str(),
                               L"Resource-Aware Task Terminator",
                               MB_OK | MB_ICONINFORMATION);
        }
        return 0;
    }

    std::vector<std::wstring> preConfirmationSkips;
    if (!FilterDecisionTargets(decision, preConfirmationSkips)) {
        if (!CancellationRequested()) {
            std::wstring message =
                L"No selected targets remain eligible after identity and protection verification.";
            for (const auto& skipped : preConfirmationSkips) {
                message += L"\n- " + skipped;
            }
            ShowTopmostMessage(message.c_str(),
                               L"Resource-Aware Task Terminator",
                               MB_OK | MB_ICONWARNING);
        }
        return 0;
    }

    if (!ConfirmKillDecision(decision) || CancellationRequested()) {
        Wh_Log(L"Termination cancelled for primary PID %lu.",
               decision.identity.pid);
        return 0;
    }

    std::vector<ProcessIdentity> targets = DecisionTargets(decision);
    int succeeded = 0;
    std::vector<std::wstring> failures = preConfirmationSkips;

    for (const auto& identity : targets) {
        if (CancellationRequested()) {
            failures.push_back(L"PID " + std::to_wstring(identity.pid) +
                               L": cancelled during shutdown");
            continue;
        }

        HANDLE process = nullptr;
        std::wstring reason;
        if (!OpenVerifiedProcessForTermination(identity, process, reason)) {
            failures.push_back(L"PID " + std::to_wstring(identity.pid) +
                               L" (" + identity.exeName + L"): " + reason);
            Wh_Log(L"Skipped PID %lu: %s", identity.pid, reason.c_str());
            continue;
        }

        if (TerminateProcess(process, 1)) {
            succeeded++;
            Wh_Log(L"TerminateProcess succeeded for PID %lu.", identity.pid);
        } else {
            DWORD error = GetLastError();
            std::wstring failure =
                L"PID " + std::to_wstring(identity.pid) + L" (" +
                identity.exeName + L"): TerminateProcess failed (error " +
                std::to_wstring(error) + L")";
            failures.push_back(std::move(failure));
            Wh_Log(L"TerminateProcess failed for PID %lu (error=%lu).",
                   identity.pid, error);
        }
        CloseHandle(process);
    }

    Wh_Log(L"Termination request finished: %d/%d succeeded.", succeeded,
           static_cast<int>(targets.size()));

    bool showSummary;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        showSummary = g_showResultSummary;
    }
    if (showSummary && !CancellationRequested()) {
        std::wstring summary = L"Termination result\n\nSucceeded: " +
            std::to_wstring(succeeded) + L"\nFailed or skipped: " +
            std::to_wstring(failures.size());
        for (const auto& failure : failures) {
            summary += L"\n- " + failure;
        }
        if (!failures.empty()) {
            summary +=
                L"\n\nAccess-denied failures may require running Windhawk elevated.";
        }
        ShowTopmostMessage(summary.c_str(),
                           L"Resource-Aware Task Terminator",
                           MB_OK | (failures.empty() ? MB_ICONINFORMATION
                                                   : MB_ICONWARNING));
    }

    return 0;
}

static void CleanupCompletedKillWorkerHandle() {
    std::lock_guard<std::mutex> lock(g_workerMutex);
    if (g_hKillWorker &&
        WaitForSingleObject(g_hKillWorker, 0) == WAIT_OBJECT_0) {
        CloseHandle(g_hKillWorker);
        g_hKillWorker = nullptr;
        g_dwKillWorkerId = 0;
    }
}

static LRESULT CALLBACK HiddenWindowProc(HWND window, UINT message,
                                         WPARAM wParam, LPARAM lParam) {
    if (message == WM_HOTKEY && wParam == HOTKEY_ID) {
        if (CancellationRequested()) {
            return 0;
        }
        if (g_killInProgress.exchange(true)) {
            Wh_Log(L"A termination workflow is already active; hotkey ignored.");
            return 0;
        }

        CleanupCompletedKillWorkerHandle();
        DWORD threadId = 0;
        HANDLE worker = CreateThread(nullptr, 0, KillWorkflowThread, nullptr,
                                     CREATE_SUSPENDED, &threadId);
        if (!worker) {
            g_killInProgress.store(false);
            Wh_Log(L"Could not create kill worker thread (error=%lu).",
                   GetLastError());
            return 0;
        }

        DWORD resumeResult;
        DWORD resumeError = ERROR_SUCCESS;
        {
            std::lock_guard<std::mutex> lock(g_workerMutex);
            g_hKillWorker = worker;
            g_dwKillWorkerId = threadId;
            resumeResult = ResumeThread(worker);
            if (resumeResult == static_cast<DWORD>(-1)) {
                resumeError = GetLastError();
                CloseHandle(g_hKillWorker);
                g_hKillWorker = nullptr;
                g_dwKillWorkerId = 0;
            }
        }
        if (resumeResult == static_cast<DWORD>(-1)) {
            g_killInProgress.store(false);
            Wh_Log(L"Could not resume kill worker thread (error=%lu).",
                   resumeError);
        }
        return 0;
    }

    return DefWindowProcW(window, message, wParam, lParam);
}

static DWORD WINAPI HotkeyThread(LPVOID) {
    WNDCLASSW windowClass{};
    windowClass.lpfnWndProc = HiddenWindowProc;
    windowClass.hInstance = GetModuleHandleW(nullptr);
    windowClass.lpszClassName =
        L"ResourceAwareTaskTerminator_HotkeyHost_Class";
    RegisterClassW(&windowClass);

    g_hHiddenWindow = CreateWindowExW(
        0, windowClass.lpszClassName,
        L"ResourceAwareTaskTerminator_HotkeyHost", 0,
        0, 0, 0, 0, HWND_MESSAGE, nullptr, windowClass.hInstance, nullptr);
    if (!g_hHiddenWindow) {
        Wh_Log(L"CreateWindowExW failed for hotkey host (error=%lu).",
               GetLastError());
        UnregisterClassW(windowClass.lpszClassName, windowClass.hInstance);
        return 1;
    }

    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        if (!RegisterHotKey(g_hHiddenWindow, HOTKEY_ID, g_modifiers, g_vkCode)) {
            Wh_Log(L"Failed to register hotkey (modifiers=%u, vk=%u, error=%lu).",
                   g_modifiers, g_vkCode, GetLastError());
        } else {
            Wh_Log(L"Registered hotkey (modifiers=%u, vk=%u).",
                   g_modifiers, g_vkCode);
        }
    }

    MSG message{};
    PeekMessageW(&message, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        if (message.message == WM_RELOAD_HOTKEY) {
            UnregisterHotKey(g_hHiddenWindow, HOTKEY_ID);
            std::lock_guard<std::mutex> lock(g_settingsMutex);
            if (!RegisterHotKey(g_hHiddenWindow, HOTKEY_ID,
                                g_modifiers, g_vkCode)) {
                Wh_Log(L"Failed to re-register hotkey (modifiers=%u, vk=%u, error=%lu).",
                       g_modifiers, g_vkCode, GetLastError());
            } else {
                Wh_Log(L"Re-registered hotkey (modifiers=%u, vk=%u).",
                       g_modifiers, g_vkCode);
            }
            continue;
        }

        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    UnregisterHotKey(g_hHiddenWindow, HOTKEY_ID);
    DestroyWindow(g_hHiddenWindow);
    g_hHiddenWindow = nullptr;
    UnregisterClassW(windowClass.lpszClassName, windowClass.hInstance);
    return 0;
}

// ------------------------ Dedicated tool-process callbacks ------------------------

BOOL WhTool_ModInit() {
    LoadSettings();

    g_hStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_hStopEvent) {
        Wh_Log(L"Failed to create stop event (error=%lu).", GetLastError());
        return FALSE;
    }

    g_hThread = CreateThread(nullptr, 0, HotkeyThread, nullptr, 0,
                             &g_dwThreadId);
    if (!g_hThread) {
        Wh_Log(L"Failed to create hotkey thread (error=%lu).", GetLastError());
        CloseHandle(g_hStopEvent);
        g_hStopEvent = nullptr;
        return FALSE;
    }

    Wh_Log(L"Tool hotkey host started. PID=%lu thread=%lu.",
           GetCurrentProcessId(), g_dwThreadId);
    return TRUE;
}

static BOOL CALLBACK CloseThreadWindow(HWND window, LPARAM) {
    PostMessageW(window, WM_CLOSE, 0, 0);
    return TRUE;
}

void WhTool_ModUninit() {
    if (g_hStopEvent) {
        SetEvent(g_hStopEvent);
    }

    HWND chooser = g_activeChooser.load();
    if (chooser && IsWindow(chooser)) {
        PostMessageW(chooser, WM_CLOSE, 0, 0);
    }

    DWORD killThreadId = 0;
    HANDLE killWorker = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_workerMutex);
        killThreadId = g_dwKillWorkerId;
        killWorker = g_hKillWorker;
        g_hKillWorker = nullptr;
    }
    if (killThreadId) {
        EnumThreadWindows(killThreadId, CloseThreadWindow, 0);
        PostThreadMessageW(killThreadId, WM_QUIT, 0, 0);
    }

    if (g_dwThreadId) {
        PostThreadMessageW(g_dwThreadId, WM_QUIT, 0, 0);
    }

    if (killWorker) {
        DWORD wait = WaitForSingleObject(killWorker, 5000);
        if (wait == WAIT_TIMEOUT) {
            Wh_Log(L"Kill worker did not exit within shutdown timeout.");
        }
        CloseHandle(killWorker);
    }

    {
        std::lock_guard<std::mutex> lock(g_workerMutex);
        g_dwKillWorkerId = 0;
    }

    if (g_hThread) {
        DWORD wait = WaitForSingleObject(g_hThread, 5000);
        if (wait == WAIT_TIMEOUT) {
            Wh_Log(L"Hotkey thread did not exit within shutdown timeout.");
        }
        CloseHandle(g_hThread);
        g_hThread = nullptr;
        g_dwThreadId = 0;
    }

    if (g_hStopEvent) {
        CloseHandle(g_hStopEvent);
        g_hStopEvent = nullptr;
    }

}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    if (g_dwThreadId) {
        PostThreadMessageW(g_dwThreadId, WM_RELOAD_HOTKEY, 0, 0);
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
