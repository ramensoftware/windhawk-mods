// ==WindhawkMod==
// @id              resource-aware-task-terminator
// @name            Resource-Aware Task Terminator
// @description     Terminate the active window process, a top resource offender, or a weighted resource-heavy app group with confirmation and sortable usage metrics.
// @version         1.0.0
// @author          Math Shamenson
// @github          https://github.com/insane66613
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -lpsapi -lgdi32 -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Resource-Aware Task Terminator

- Press the configured hotkey to terminate a target process or app group.
- Modes:
  - foreground: Kill the active window process.
  - top_cpu: Sample CPU over a configurable window and show the top PID candidates.
  - top_pagefaults: Sample page-fault deltas and show the top PID candidates.
  - top_disk_io: Sample process read/write byte deltas and show the top PID candidates.
  - aggregate_super_score: Group eligible processes by executable path/name and rank app groups by CPU, page faults, disk I/O, and memory.
  - top_gpu_experimental: Disabled in this no-PDH build to avoid loader dependency failures.

Defaults
- Hotkey: Ctrl + Alt + F4. Pick a different hotkey if another app or Windows reserves it.
- Protected apps: Conservative defaults including explorer.exe, windhawk.exe, taskmgr.exe, conhost.exe, dwm.exe, and critical system processes.

Notes and Safety
- You may need to run Windhawk elevated to kill elevated targets; otherwise OpenProcess(PROCESS_TERMINATE) can fail with Access Denied.
- Do not remove critical system processes from the protected list. Killing them can destabilize Windows.
- Terminating an app can cause data loss. Save work first.
- Confirmation mode is enabled by default and shows the selected process/app group, mode, sampling evidence, and protection check before termination.
- The chooser supports sortable metric columns, severity-colored metric cells, manual refresh, and optional live refresh.

Troubleshooting
- If the hotkey does not work, pick a different combo. Some Win+ combos or hotkeys taken by other apps will fail to register.
- GPU mode is disabled in this no-PDH build.
- This build runs the hotkey in a dedicated Windhawk tool process for a persistent per-session host.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- kill_mode: foreground
  $name: Target mode
  $description: "What to target: active window process, top resource offender, or aggregate app group."
  $options:
    - foreground: "Active window process"
    - top_cpu: "Top CPU PID candidates"
    - top_pagefaults: "Top page-fault PID candidates"
    - top_disk_io: "Top disk I/O PID candidates"
    - aggregate_super_score: "Aggregate app-group super score"
    - top_gpu_experimental: "Disabled in no-PDH build"

- sample_ms: 3000
  $name: "Sampling window (ms)"
  $description: "Sampling window for CPU/page-fault/disk/aggregate modes (500-15000). Longer windows are more stable."

- candidate_count: 5
  $name: "Candidate count"
  $description: "How many top candidates/app groups to show (1-10). Default 5 shows the top offender plus the next four."

- live_update_default: false
  $name: "Live update by default"
  $description: "When true, the chooser starts with live refresh enabled."

- live_poll_ms: 5000
  $name: "Live refresh interval (ms)"
  $description: "Polling interval for live chooser refresh (1000-60000). Each refresh re-samples using the sampling window."

- disable_dark_ui: false
  $name: "Disable dark chooser UI"
  $description: "Default false keeps the candidate chooser in a dark UI style."

- super_cpu_weight: 45
  $name: "Super score CPU weight"
  $description: "Aggregate mode CPU weight, 0-100. Default 45."

- super_pagefault_weight: 20
  $name: "Super score page-fault weight"
  $description: "Aggregate mode page-fault weight, 0-100. Default 20."

- super_disk_weight: 25
  $name: "Super score disk I/O weight"
  $description: "Aggregate mode disk read/write byte weight, 0-100. Default 25."

- super_memory_weight: 10
  $name: "Super score memory weight"
  $description: "Aggregate mode working-set memory weight, 0-100. Default 10."

- kill_key: F4
  $name: "Main hotkey key"
  $description: "Use A-Z, 0-9, or F1-F24."

- disable_ctrl: false
  $name: "Do not require Ctrl"
  $description: "Default false keeps Ctrl required."

- disable_alt: false
  $name: "Do not require Alt"
  $description: "Default false keeps Alt required."

- require_shift: false
  $name: "Require Shift"

- require_win: false
  $name: "Require Win"

- protected_apps: "explorer.exe, windhawk.exe, taskmgr.exe, conhost.exe, lsass.exe, csrss.exe, wininit.exe, services.exe, winlogon.exe, dwm.exe"
  $name: "Protected apps"
  $description: "Comma-separated exe names that must never be terminated."

- disable_confirmation: false
  $name: "Disable confirmation before kill"
  $description: "When false, show a confirmation dialog with selected target and decision evidence before TerminateProcess is called."
*/
// ==/WindhawkModSettings==





#include <algorithm>
#include <atomic>
#include <cwctype>
#include <cstdio>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <commctrl.h>


#define HOTKEY_ID 1
#define WM_RELOAD_HOTKEY (WM_APP + 1)

HWND g_hHiddenWindow = NULL;
HANDLE g_hThread = NULL;
DWORD g_dwThreadId = 0;

// Settings + sync
std::mutex g_settingsMutex;
UINT g_modifiers = 0;
UINT g_vkCode = VK_F4;
std::vector<std::wstring> g_blacklist;

enum class KillMode { Foreground, TopCpu, TopPagefaults, TopDiskIo, AggregateSuperScore, TopGpuExperimental };
KillMode g_killMode = KillMode::Foreground;
DWORD g_sampleMs = 3000;
int g_candidateCount = 5;
bool g_liveUpdateDefault = false;
DWORD g_livePollMs = 5000;
bool g_darkChooserUi = true;
int g_superCpuWeight = 45;
int g_superPageFaultWeight = 20;
int g_superDiskWeight = 25;
int g_superMemoryWeight = 10;
bool g_disableConfirmation = false;
std::atomic<bool> g_killInProgress{false};

// ------------------------ Settings helpers ------------------------

static UINT ParseKeyCode(PCWSTR keyStr) {
    std::wstring k = keyStr ? keyStr : L"";
    if (k.empty()) return VK_F4;

    if (k.length() == 1) {
        wchar_t c = towupper(k[0]);
        if ((c >= L'A' && c <= L'Z') || (c >= L'0' && c <= L'9'))
            return (UINT)c;
    } else if (k.length() >= 2 && towupper(k[0]) == L'F') {
        int fNum = _wtoi(k.c_str() + 1);
        if (fNum >= 1 && fNum <= 24)
            return VK_F1 + fNum - 1;
    }
    return VK_F4;
}

static KillMode ParseKillMode(PCWSTR s) {
    if (!s) return KillMode::Foreground;
    std::wstring w = s;
    std::transform(w.begin(), w.end(), w.begin(), ::towlower);
    if (w == L"top_cpu") return KillMode::TopCpu;
    if (w == L"top_pagefaults") return KillMode::TopPagefaults;
    if (w == L"top_disk_io" || w == L"top_disk" || w == L"disk_activity") return KillMode::TopDiskIo;
    if (w == L"aggregate_super_score" || w == L"super_score" || w == L"aggregate") return KillMode::AggregateSuperScore;
    if (w == L"top_gpu_experimental") return KillMode::TopGpuExperimental;
    return KillMode::Foreground;
}

static void LoadSettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);

    g_modifiers = 0;
    // Windhawk may return 0 for unset/default booleans. Use inverted names
    // so the default hotkey remains Ctrl+Alt+F4 unless explicitly disabled.
    if (Wh_GetIntSetting(L"disable_ctrl") == 0) g_modifiers |= MOD_CONTROL;
    if (Wh_GetIntSetting(L"disable_alt") == 0)  g_modifiers |= MOD_ALT;
    if (Wh_GetIntSetting(L"require_shift") != 0) g_modifiers |= MOD_SHIFT;
    if (Wh_GetIntSetting(L"require_win") != 0)   g_modifiers |= MOD_WIN;
    g_modifiers |= MOD_NOREPEAT;

    PCWSTR keyStr = Wh_GetStringSetting(L"kill_key");
    if (keyStr && *keyStr) {
        g_vkCode = ParseKeyCode(keyStr);
    } else {
        g_vkCode = VK_F4;
    }
    if (keyStr) {
        Wh_FreeStringSetting(keyStr);
    }

    PCWSTR modeStr = Wh_GetStringSetting(L"kill_mode");
    g_killMode = ParseKillMode(modeStr);
    Wh_FreeStringSetting(modeStr);

    int ms = Wh_GetIntSetting(L"sample_ms");
    g_sampleMs = (ms >= 500 && ms <= 15000) ? (DWORD)ms : 3000;

    int candidates = Wh_GetIntSetting(L"candidate_count");
    g_candidateCount = (candidates >= 1 && candidates <= 10) ? candidates : 5;

    g_liveUpdateDefault = Wh_GetIntSetting(L"live_update_default") != 0;
    int pollMs = Wh_GetIntSetting(L"live_poll_ms");
    g_livePollMs = (pollMs >= 1000 && pollMs <= 60000) ? (DWORD)pollMs : 5000;
    g_darkChooserUi = Wh_GetIntSetting(L"disable_dark_ui") == 0;

    auto clampWeight = [](int value, int fallback) { return (value >= 0 && value <= 100) ? value : fallback; };
    g_superCpuWeight = clampWeight(Wh_GetIntSetting(L"super_cpu_weight"), 45);
    g_superPageFaultWeight = clampWeight(Wh_GetIntSetting(L"super_pagefault_weight"), 20);
    g_superDiskWeight = clampWeight(Wh_GetIntSetting(L"super_disk_weight"), 25);
    g_superMemoryWeight = clampWeight(Wh_GetIntSetting(L"super_memory_weight"), 10);

    g_disableConfirmation = Wh_GetIntSetting(L"disable_confirmation") != 0;

    g_blacklist.clear();
    PCWSTR appsStr = Wh_GetStringSetting(L"protected_apps");
    std::wstring rawApps = appsStr ? appsStr : L"";
    Wh_FreeStringSetting(appsStr);

    size_t start = 0, end = 0;
    while ((end = rawApps.find(L',', start)) != std::wstring::npos) {
        std::wstring app = rawApps.substr(start, end - start);
        app.erase(std::remove_if(app.begin(), app.end(), iswspace), app.end());
        std::transform(app.begin(), app.end(), app.begin(), ::towlower);
        if (!app.empty())
            g_blacklist.push_back(app);
        start = end + 1;
    }
    std::wstring lastApp = rawApps.substr(start);
    lastApp.erase(std::remove_if(lastApp.begin(), lastApp.end(), iswspace), lastApp.end());
    std::transform(lastApp.begin(), lastApp.end(), lastApp.begin(), ::towlower);
    if (!lastApp.empty())
        g_blacklist.push_back(lastApp);
}

// ------------------------ Target protection ------------------------

static bool IsProcessProtected(DWORD pid) {
    if (pid <= 4 || pid == GetCurrentProcessId())
        return true;

    // Quick short-circuit for known criticals by filename (in addition to user list)
    auto isCriticalName = [](const std::wstring& exe) {
        return exe == L"lsass.exe" || exe == L"csrss.exe" || exe == L"wininit.exe" ||
               exe == L"smss.exe"  || exe == L"services.exe" || exe == L"winlogon.exe" ||
               exe == L"windhawk.exe";
    };

    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProc)
        return false;

    wchar_t path[MAX_PATH];
    DWORD size = MAX_PATH;
    bool isProtected = false;

    if (QueryFullProcessImageNameW(hProc, 0, path, &size)) {
        std::wstring wPath(path);
        size_t pos = wPath.find_last_of(L"\\/");
        std::wstring exeName = (pos != std::wstring::npos) ? wPath.substr(pos + 1) : wPath;
        std::transform(exeName.begin(), exeName.end(), exeName.begin(), ::towlower);

        if (isCriticalName(exeName)) {
            isProtected = true;
        } else {
            std::lock_guard<std::mutex> lock(g_settingsMutex);
            for (const auto& blocked : g_blacklist) {
                if (exeName == blocked) {
                    isProtected = true;
                    break;
                }
            }
        }
    }

    CloseHandle(hProc);
    return isProtected;
}

// ------------------------ Process sampling utilities ------------------------

struct ProcSnapshot {
    DWORD pid{};
    ULONGLONG cpuKernel100ns{};
    ULONGLONG cpuUser100ns{};
    DWORD pageFaultCount{};
    ULONGLONG ioReadBytes{};
    ULONGLONG ioWriteBytes{};
    ULONGLONG ioOtherBytes{};
    SIZE_T workingSetBytes{};
};

static bool EnumerateProcesses(std::vector<ProcSnapshot>& out) {
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) return false;

    PROCESSENTRY32 pe{ sizeof(pe) };
    if (!Process32First(hSnap, &pe)) { CloseHandle(hSnap); return false; }

    do {
        DWORD pid = pe.th32ProcessID;
        if (pid <= 4) continue;
        HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ, FALSE, pid);
        if (!h) continue;

        FILETIME ct{}, et{}, kt{}, ut{};
        ProcSnapshot ps{ .pid = pid };
        if (GetProcessTimes(h, &ct, &et, &kt, &ut)) {
            ULARGE_INTEGER k{}, u{};
            k.LowPart = kt.dwLowDateTime; k.HighPart = kt.dwHighDateTime;
            u.LowPart = ut.dwLowDateTime; u.HighPart = ut.dwHighDateTime;
            ps.cpuKernel100ns = k.QuadPart;
            ps.cpuUser100ns = u.QuadPart;
        }

        PROCESS_MEMORY_COUNTERS pmc{};
        if (GetProcessMemoryInfo(h, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
            ps.pageFaultCount = pmc.PageFaultCount;
            ps.workingSetBytes = pmc.WorkingSetSize;
        }

        IO_COUNTERS io{};
        if (GetProcessIoCounters(h, &io)) {
            ps.ioReadBytes = io.ReadTransferCount;
            ps.ioWriteBytes = io.WriteTransferCount;
            ps.ioOtherBytes = io.OtherTransferCount;
        }

        CloseHandle(h);
        out.push_back(ps);
    } while (Process32Next(hSnap, &pe));

    CloseHandle(hSnap);
    return true;
}

// Experimental GPU picker disabled in no-PDH build.
// The previous implementation linked against pdh.dll, and Windhawk failed to
// load the mod DLL with 8007007E on this system. Keep the mode parseable so old
// settings do not break, but return no target instead of importing PDH.
static DWORD PickTopGpuPid_Experimental(DWORD sampleMs, double* outBestGpu = nullptr) {
    UNREFERENCED_PARAMETER(sampleMs);
    if (outBestGpu) {
        *outBestGpu = 0.0;
    }
    return 0;
}

// ------------------------ Hotkey + message loop ------------------------


struct KillDecision {
    DWORD pid{};
    KillMode mode = KillMode::Foreground;
    DWORD sampleMs{};
    HWND foregroundWindow = nullptr;
    std::wstring windowTitle;
    std::wstring windowClass;
    std::wstring processPath;
    std::wstring exeName;
    std::wstring reason;
    std::wstring metricName;
    double metricValue = 0.0;
    double cpuPercent = 0.0;
    double pageFaultDelta = 0.0;
    double diskIoBytes = 0.0;
    double memoryMb = 0.0;
    double superScore = 0.0;
    bool terminateGroup = false;
    std::vector<DWORD> targetPids;
};

static PCWSTR KillModeName(KillMode mode) {
    switch (mode) {
    case KillMode::Foreground:
        return L"foreground";
    case KillMode::TopCpu:
        return L"top_cpu";
    case KillMode::TopPagefaults:
        return L"top_pagefaults";
    case KillMode::TopDiskIo:
        return L"top_disk_io";
    case KillMode::AggregateSuperScore:
        return L"aggregate_super_score";
    case KillMode::TopGpuExperimental:
        return L"top_gpu_experimental";
    }
    return L"unknown";
}

static std::wstring GetWindowTextSafe(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) {
        return L"";
    }

    int length = GetWindowTextLengthW(hwnd);
    if (length <= 0) {
        return L"";
    }

    std::wstring title(static_cast<size_t>(length) + 1, L'\0');
    int copied = GetWindowTextW(hwnd, title.data(), length + 1);
    if (copied <= 0) {
        return L"";
    }

    title.resize(static_cast<size_t>(copied));
    return title;
}

static std::wstring GetClassNameSafe(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) {
        return L"";
    }

    WCHAR buffer[256] = {};
    int copied = GetClassNameW(hwnd, buffer, ARRAYSIZE(buffer));
    if (copied <= 0) {
        return L"";
    }

    return std::wstring(buffer, static_cast<size_t>(copied));
}

static void FillProcessInfo(KillDecision& decision) {
    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, decision.pid);
    if (!hProc) {
        decision.exeName = L"<query failed>";
        decision.processPath = L"<OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION) failed>";
        return;
    }

    WCHAR path[MAX_PATH] = {};
    DWORD size = ARRAYSIZE(path);
    if (QueryFullProcessImageNameW(hProc, 0, path, &size)) {
        decision.processPath.assign(path, size);
        size_t pos = decision.processPath.find_last_of(L"\\/");
        decision.exeName = (pos == std::wstring::npos)
            ? decision.processPath
            : decision.processPath.substr(pos + 1);
    } else {
        decision.exeName = L"<unknown>";
        decision.processPath = L"<QueryFullProcessImageNameW failed>";
    }

    CloseHandle(hProc);
}


static std::vector<KillDecision> BuildResourceCandidateDecisions(KillMode mode, DWORD sampleMs, int limit) {
    std::vector<KillDecision> candidates;
    if (limit < 1) {
        limit = 1;
    }
    if (limit > 10) {
        limit = 10;
    }

    FILETIME idleA{}, kernelA{}, userA{}, idleB{}, kernelB{}, userB{};
    std::vector<ProcSnapshot> a, b;

    if (mode == KillMode::TopCpu) {
        GetSystemTimes(&idleA, &kernelA, &userA);
    }

    if (!EnumerateProcesses(a)) {
        return candidates;
    }

    Sleep(sampleMs);

    if (!EnumerateProcesses(b)) {
        return candidates;
    }

    ULONGLONG sysDelta = 1;
    if (mode == KillMode::TopCpu) {
        GetSystemTimes(&idleB, &kernelB, &userB);
        auto toUL = [](FILETIME t) {
            ULARGE_INTEGER x;
            x.LowPart = t.dwLowDateTime;
            x.HighPart = t.dwHighDateTime;
            return x.QuadPart;
        };
        sysDelta = (toUL(kernelB) - toUL(kernelA)) + (toUL(userB) - toUL(userA));
        if (!sysDelta) {
            sysDelta = 1;
        }
    }

    for (const auto& pb : b) {
        auto it = std::find_if(a.begin(), a.end(), [&](const ProcSnapshot& x) { return x.pid == pb.pid; });
        if (it == a.end()) {
            continue;
        }

        if (IsProcessProtected(pb.pid)) {
            continue;
        }

        double value = 0.0;
        std::wstring reason;
        std::wstring metricName;

        if (mode == KillMode::TopCpu) {
            ULONGLONG pDelta = (pb.cpuKernel100ns - it->cpuKernel100ns) +
                               (pb.cpuUser100ns - it->cpuUser100ns);
            value = (double)pDelta / (double)sysDelta * 100.0;
            reason = L"Top CPU mode sampled process CPU deltas and ranked the highest non-protected processes.";
            metricName = L"CPU usage during sample, normalized across all logical processors (%)";
        } else if (mode == KillMode::TopPagefaults) {
            long long delta = (long long)pb.pageFaultCount - (long long)it->pageFaultCount;
            value = (double)delta;
            reason = L"Top page-fault mode sampled page-fault counters and ranked the highest non-protected deltas.";
            metricName = L"Page-fault delta during sample";
        } else if (mode == KillMode::TopDiskIo) {
            ULONGLONG before = it->ioReadBytes + it->ioWriteBytes;
            ULONGLONG after = pb.ioReadBytes + pb.ioWriteBytes;
            value = (after >= before) ? (double)(after - before) : 0.0;
            reason = L"Top disk I/O mode sampled process read/write byte counters and ranked the highest non-protected deltas.";
            metricName = L"Disk read/write byte delta during sample";
        } else {
            continue;
        }

        if (value <= 0.0) {
            continue;
        }

        KillDecision d;
        d.pid = pb.pid;
        d.mode = mode;
        d.sampleMs = sampleMs;
        d.reason = reason;
        d.metricName = metricName;
        d.metricValue = value;
        if (mode == KillMode::TopCpu) d.cpuPercent = value;
        if (mode == KillMode::TopPagefaults) d.pageFaultDelta = value;
        if (mode == KillMode::TopDiskIo) d.diskIoBytes = value;
        d.memoryMb = (double)pb.workingSetBytes / (1024.0 * 1024.0);
        FillProcessInfo(d);
        candidates.push_back(std::move(d));
    }

    std::sort(candidates.begin(), candidates.end(), [](const KillDecision& a, const KillDecision& b) {
        return a.metricValue > b.metricValue;
    });

    if ((int)candidates.size() > limit) {
        candidates.resize((size_t)limit);
    }

    return candidates;
}


static std::wstring LowerCopy(std::wstring value) {
    std::transform(value.begin(), value.end(), value.begin(), ::towlower);
    return value;
}

static std::wstring BuildPidListText(const std::vector<DWORD>& pids) {
    std::wstring text;
    for (size_t i = 0; i < pids.size(); i++) {
        if (i) text += L", ";
        text += std::to_wstring(pids[i]);
    }
    return text.empty() ? L"<none>" : text;
}

static double NormalizeMetric(double value, double maxValue) {
    if (maxValue <= 0.0 || value <= 0.0) {
        return 0.0;
    }
    return value / maxValue;
}

static std::vector<KillDecision> BuildAggregateSuperScoreCandidates(DWORD sampleMs, int limit) {
    std::vector<KillDecision> groups;
    if (limit < 1) limit = 1;
    if (limit > 10) limit = 10;

    FILETIME idleA{}, kernelA{}, userA{}, idleB{}, kernelB{}, userB{};
    std::vector<ProcSnapshot> a, b;

    GetSystemTimes(&idleA, &kernelA, &userA);
    if (!EnumerateProcesses(a)) return groups;
    Sleep(sampleMs);
    if (!EnumerateProcesses(b)) return groups;
    GetSystemTimes(&idleB, &kernelB, &userB);

    auto toUL = [](FILETIME t) {
        ULARGE_INTEGER x;
        x.LowPart = t.dwLowDateTime;
        x.HighPart = t.dwHighDateTime;
        return x.QuadPart;
    };
    ULONGLONG sysDelta = (toUL(kernelB) - toUL(kernelA)) + (toUL(userB) - toUL(userA));
    if (!sysDelta) sysDelta = 1;

    std::unordered_map<std::wstring, size_t> indexByKey;

    for (const auto& pb : b) {
        auto it = std::find_if(a.begin(), a.end(), [&](const ProcSnapshot& x) { return x.pid == pb.pid; });
        if (it == a.end() || IsProcessProtected(pb.pid)) {
            continue;
        }

        KillDecision proc;
        proc.pid = pb.pid;
        FillProcessInfo(proc);
        if (proc.exeName.empty() || proc.exeName == L"<query failed>" || proc.exeName == L"<unknown>") {
            continue;
        }

        std::wstring key = !proc.processPath.empty() && proc.processPath[0] != L'<'
            ? LowerCopy(proc.processPath)
            : LowerCopy(proc.exeName);

        ULONGLONG pDelta = (pb.cpuKernel100ns - it->cpuKernel100ns) +
                           (pb.cpuUser100ns - it->cpuUser100ns);
        double cpu = (double)pDelta / (double)sysDelta * 100.0;
        double faults = (double)((long long)pb.pageFaultCount - (long long)it->pageFaultCount);
        if (faults < 0.0) faults = 0.0;
        ULONGLONG ioBefore = it->ioReadBytes + it->ioWriteBytes;
        ULONGLONG ioAfter = pb.ioReadBytes + pb.ioWriteBytes;
        double disk = (ioAfter >= ioBefore) ? (double)(ioAfter - ioBefore) : 0.0;
        double memory = (double)pb.workingSetBytes / (1024.0 * 1024.0);

        if (cpu <= 0.0 && faults <= 0.0 && disk <= 0.0 && memory <= 0.0) {
            continue;
        }

        size_t groupIndex = 0;
        auto found = indexByKey.find(key);
        if (found == indexByKey.end()) {
            KillDecision group;
            group.pid = pb.pid;
            group.mode = KillMode::AggregateSuperScore;
            group.sampleMs = sampleMs;
            group.exeName = proc.exeName;
            group.processPath = proc.processPath;
            group.terminateGroup = true;
            group.reason = L"Aggregate super score mode grouped eligible processes by executable path/name and combined CPU, page-fault, disk I/O, and memory activity.";
            group.metricName = L"Aggregate super score";
            groups.push_back(std::move(group));
            groupIndex = groups.size() - 1;
            indexByKey.emplace(key, groupIndex);
        } else {
            groupIndex = found->second;
        }

        KillDecision& group = groups[groupIndex];
        group.targetPids.push_back(pb.pid);
        group.cpuPercent += cpu;
        group.pageFaultDelta += faults;
        group.diskIoBytes += disk;
        group.memoryMb += memory;

        // Keep the most CPU-active PID as the primary PID shown for the group.
        if (cpu > group.metricValue) {
            group.pid = pb.pid;
            group.metricValue = cpu;
        }
    }

    double maxCpu = 0.0, maxFaults = 0.0, maxDisk = 0.0, maxMemory = 0.0;
    for (const auto& group : groups) {
        maxCpu = std::max(maxCpu, group.cpuPercent);
        maxFaults = std::max(maxFaults, group.pageFaultDelta);
        maxDisk = std::max(maxDisk, group.diskIoBytes);
        maxMemory = std::max(maxMemory, group.memoryMb);
    }

    int cpuWeight = 45, faultWeight = 20, diskWeight = 25, memoryWeight = 10;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        cpuWeight = g_superCpuWeight;
        faultWeight = g_superPageFaultWeight;
        diskWeight = g_superDiskWeight;
        memoryWeight = g_superMemoryWeight;
    }
    int totalWeight = cpuWeight + faultWeight + diskWeight + memoryWeight;
    if (totalWeight <= 0) {
        cpuWeight = 45;
        faultWeight = 20;
        diskWeight = 25;
        memoryWeight = 10;
        totalWeight = 100;
    }

    for (auto& group : groups) {
        double score = 0.0;
        score += (double)cpuWeight * NormalizeMetric(group.cpuPercent, maxCpu);
        score += (double)faultWeight * NormalizeMetric(group.pageFaultDelta, maxFaults);
        score += (double)diskWeight * NormalizeMetric(group.diskIoBytes, maxDisk);
        score += (double)memoryWeight * NormalizeMetric(group.memoryMb, maxMemory);
        group.superScore = score / (double)totalWeight * 100.0;
        group.metricValue = group.superScore;
    }

    std::sort(groups.begin(), groups.end(), [](const KillDecision& a, const KillDecision& b) {
        return a.metricValue > b.metricValue;
    });

    if ((int)groups.size() > limit) {
        groups.resize((size_t)limit);
    }
    return groups;
}

static int ShowCandidateChooser(std::vector<KillDecision>& candidates);

static bool ResolveTargetDecision(KillDecision& decision) {
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        decision.mode = g_killMode;
        decision.sampleMs = g_sampleMs;
    }

    if (decision.mode == KillMode::Foreground) {
        HWND hActive = GetForegroundWindow();
        decision.foregroundWindow = hActive;
        decision.windowTitle = GetWindowTextSafe(hActive);
        decision.windowClass = GetClassNameSafe(hActive);

        if (!hActive || hActive == GetShellWindow()) {
            return false;
        }

        DWORD pid = 0;
        GetWindowThreadProcessId(hActive, &pid);
        decision.pid = pid;
        decision.reason = L"Foreground mode selected the process that owns the current foreground window.";
        decision.metricName = L"Foreground HWND";
        decision.metricValue = static_cast<double>(reinterpret_cast<ULONG_PTR>(hActive));
    } else if (decision.mode == KillMode::TopCpu ||
               decision.mode == KillMode::TopPagefaults ||
               decision.mode == KillMode::TopDiskIo ||
               decision.mode == KillMode::AggregateSuperScore) {
        int candidateCount = 5;
        {
            std::lock_guard<std::mutex> lock(g_settingsMutex);
            candidateCount = g_candidateCount;
        }

        std::vector<KillDecision> candidates =
            (decision.mode == KillMode::AggregateSuperScore)
                ? BuildAggregateSuperScoreCandidates(decision.sampleMs, candidateCount)
                : BuildResourceCandidateDecisions(decision.mode, decision.sampleMs, candidateCount);
        if (candidates.empty()) {
            return false;
        }

        int selectedIndex = 0;
        if (candidates.size() > 1) {
            selectedIndex = ShowCandidateChooser(candidates);
            if (selectedIndex < 0 || selectedIndex >= (int)candidates.size()) {
                return false;
            }
        }

        decision = candidates[(size_t)selectedIndex];
        return true;
    } else if (decision.mode == KillMode::TopGpuExperimental) {
        double gpu = 0.0;
        decision.pid = PickTopGpuPid_Experimental(decision.sampleMs, &gpu);
        decision.reason = L"Experimental GPU mode is disabled in the no-PDH build; no target was selected.";
        decision.metricName = L"GPU engine utilization aggregate";
        decision.metricValue = gpu;
    }

    if (!decision.pid) {
        return false;
    }

    FillProcessInfo(decision);
    return true;
}

static std::wstring FormatDouble(double value) {
    WCHAR buffer[64] = {};
    swprintf(buffer, ARRAYSIZE(buffer), L"%.2f", value);
    return buffer;
}



#define IDC_CANDIDATE_LIST 4201
#define IDC_REFRESH_BUTTON 4202
#define IDC_LIVE_CHECK 4203
#define IDC_POLL_EDIT 4204
#define IDC_STATUS_STATIC 4205
#define TIMER_LIVE_REFRESH 4301

struct CandidateDialogState {
    std::vector<KillDecision>* candidates{};
    int selectedIndex = -1;
    HWND hList{};
    HWND hRefresh{};
    HWND hLive{};
    HWND hPollEdit{};
    HWND hStatus{};
    KillMode mode = KillMode::Foreground;
    DWORD sampleMs = 3000;
    int candidateLimit = 5;
    DWORD pollMs = 5000;
    bool live = false;
    bool darkUi = true;
    int visualSelectedRow = 0;
    bool refreshing = false;
    HBRUSH hDarkBrush{};
    HBRUSH hDarkEditBrush{};
    int sortColumn = 3;
    bool sortDescending = true;
};

static std::wstring FormatInteger(double value) {
    WCHAR buffer[64] = {};
    swprintf(buffer, ARRAYSIZE(buffer), L"%.0f", value);
    return buffer;
}

static std::wstring FormatMb(double bytesOrMb, bool valueIsBytes) {
    double mb = valueIsBytes ? bytesOrMb / (1024.0 * 1024.0) : bytesOrMb;
    WCHAR buffer[64] = {};
    swprintf(buffer, ARRAYSIZE(buffer), L"%.2f", mb);
    return buffer;
}

static double CandidateMetricForColumn(const KillDecision& d, int column) {
    switch (column) {
    case 0: return d.metricValue;
    case 2: return d.terminateGroup ? (double)d.targetPids.size() : (double)d.pid;
    case 3: return d.mode == KillMode::AggregateSuperScore ? d.superScore : d.metricValue;
    case 4: return d.cpuPercent;
    case 5: return d.pageFaultDelta;
    case 6: return d.diskIoBytes;
    case 7: return d.memoryMb;
    default: return 0.0;
    }
}

static std::wstring CandidateTextForColumn(const KillDecision& d, int row, int column) {
    switch (column) {
    case 0: return std::to_wstring(row + 1);
    case 1: return d.terminateGroup ? (L"APP " + d.exeName) : d.exeName;
    case 2:
        return d.terminateGroup ? std::to_wstring(d.targetPids.size()) : std::to_wstring(d.pid);
    case 3:
        return FormatDouble(d.mode == KillMode::AggregateSuperScore ? d.superScore : d.metricValue);
    case 4: return FormatDouble(d.cpuPercent);
    case 5: return FormatInteger(d.pageFaultDelta);
    case 6: return FormatMb(d.diskIoBytes, true);
    case 7: return FormatDouble(d.memoryMb);
    case 8: return d.processPath;
    }
    return L"";
}

static double MaxMetricForColumn(const CandidateDialogState* state, int column) {
    if (!state || !state->candidates) {
        return 0.0;
    }
    double maxValue = 0.0;
    for (const auto& d : *state->candidates) {
        maxValue = std::max(maxValue, CandidateMetricForColumn(d, column));
    }
    return maxValue;
}

static COLORREF DarkBg() { return RGB(30, 30, 30); }
static COLORREF DarkPanelBg() { return RGB(38, 38, 38); }
static COLORREF DarkText() { return RGB(235, 235, 235); }
static COLORREF TargetSelectedBg() { return RGB(0, 95, 170); }

static void TrySetDarkTheme(HWND hwnd) {
    HMODULE hUx = GetModuleHandleW(L"uxtheme.dll");
    if (!hUx) return;
    using SetWindowTheme_t = HRESULT (WINAPI*)(HWND, LPCWSTR, LPCWSTR);
    auto pSetWindowTheme = reinterpret_cast<SetWindowTheme_t>(GetProcAddress(hUx, "SetWindowTheme"));
    if (pSetWindowTheme) {
        pSetWindowTheme(hwnd, L"DarkMode_Explorer", nullptr);
    }
}

static COLORREF SeverityColor(double normalized) {
    if (normalized < 0.0) normalized = 0.0;
    if (normalized > 1.0) normalized = 1.0;

    int r = 0, g = 0, b = 64;
    if (normalized < 0.5) {
        double t = normalized / 0.5;
        r = (int)(255.0 * t);
        g = 255;
    } else {
        double t = (normalized - 0.5) / 0.5;
        r = 255;
        g = (int)(255.0 * (1.0 - t));
    }

    // Pastel blend for readable black text.
    r = (r + 510) / 3;
    g = (g + 510) / 3;
    b = (b + 510) / 3;
    return RGB(r, g, b);
}

static void SortCandidatesForColumn(CandidateDialogState* state) {
    if (!state || !state->candidates) {
        return;
    }

    int column = state->sortColumn;
    bool desc = state->sortDescending;
    std::sort(state->candidates->begin(), state->candidates->end(), [&](const KillDecision& a, const KillDecision& b) {
        if (column == 1 || column == 8) {
            std::wstring at = CandidateTextForColumn(a, 0, column);
            std::wstring bt = CandidateTextForColumn(b, 0, column);
            int cmp = _wcsicmp(at.c_str(), bt.c_str());
            return desc ? cmp > 0 : cmp < 0;
        }

        double av = CandidateMetricForColumn(a, column);
        double bv = CandidateMetricForColumn(b, column);
        if (av == bv) {
            int cmp = _wcsicmp(a.exeName.c_str(), b.exeName.c_str());
            return cmp < 0;
        }
        return desc ? av > bv : av < bv;
    });
}

static void InsertListColumn(HWND hList, int index, PCWSTR title, int width) {
    LVCOLUMNW col{};
    col.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    col.pszText = const_cast<PWSTR>(title);
    col.cx = width;
    col.iSubItem = index;
    ListView_InsertColumn(hList, index, &col);
}

static void PopulateCandidateList(CandidateDialogState* state) {
    if (!state || !state->hList || !state->candidates) {
        return;
    }

    SortCandidatesForColumn(state);
    ListView_DeleteAllItems(state->hList);
    state->visualSelectedRow = state->candidates->empty() ? -1 : 0;

    for (int i = 0; i < (int)state->candidates->size(); i++) {
        std::wstring text0 = CandidateTextForColumn((*state->candidates)[i], i, 0);
        LVITEMW item{};
        item.mask = LVIF_TEXT;
        item.iItem = i;
        item.iSubItem = 0;
        item.pszText = text0.data();
        ListView_InsertItem(state->hList, &item);

        for (int col = 1; col <= 8; col++) {
            std::wstring text = CandidateTextForColumn((*state->candidates)[i], i, col);
            ListView_SetItemText(state->hList, i, col, text.data());
        }
    }

    if (!state->candidates->empty()) {
        InvalidateRect(state->hList, nullptr, TRUE);
    }
}

static DWORD ReadPollInterval(CandidateDialogState* state) {
    if (!state || !state->hPollEdit) {
        return 5000;
    }
    WCHAR buffer[32] = {};
    GetWindowTextW(state->hPollEdit, buffer, ARRAYSIZE(buffer));
    int ms = _wtoi(buffer);
    if (ms < 1000) ms = 1000;
    if (ms > 60000) ms = 60000;
    return (DWORD)ms;
}

static void SetStatus(CandidateDialogState* state, PCWSTR text) {
    if (state && state->hStatus) {
        SetWindowTextW(state->hStatus, text);
        UpdateWindow(state->hStatus);
    }
}

static void RefreshCandidateData(HWND hwnd, CandidateDialogState* state) {
    if (!state || state->refreshing || !state->candidates) {
        return;
    }

    state->refreshing = true;
    SetStatus(state, L"Refreshing sample...");
    EnableWindow(state->hRefresh, FALSE);
    UpdateWindow(hwnd);

    std::vector<KillDecision> refreshed =
        (state->mode == KillMode::AggregateSuperScore)
            ? BuildAggregateSuperScoreCandidates(state->sampleMs, state->candidateLimit)
            : BuildResourceCandidateDecisions(state->mode, state->sampleMs, state->candidateLimit);

    if (!refreshed.empty()) {
        *state->candidates = std::move(refreshed);
        PopulateCandidateList(state);
        SYSTEMTIME st{};
        GetLocalTime(&st);
        WCHAR status[128] = {};
        swprintf(status, ARRAYSIZE(status), L"Updated %02u:%02u:%02u - %u candidate(s)",
                 st.wHour, st.wMinute, st.wSecond, (UINT)state->candidates->size());
        SetStatus(state, status);
    } else {
        SetStatus(state, L"Refresh found no eligible candidates.");
    }

    EnableWindow(state->hRefresh, TRUE);
    state->refreshing = false;
}

static LRESULT CALLBACK CandidateChooserProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    CandidateDialogState* state = reinterpret_cast<CandidateDialogState*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));

    switch (msg) {
    case WM_CREATE: {
        auto cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
        state = reinterpret_cast<CandidateDialogState*>(cs->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(state));
        if (state->darkUi) {
            state->hDarkBrush = CreateSolidBrush(DarkBg());
            state->hDarkEditBrush = CreateSolidBrush(DarkPanelBg());
            TrySetDarkTheme(hwnd);
        }

        INITCOMMONCONTROLSEX icc{ sizeof(icc), ICC_LISTVIEW_CLASSES };
        InitCommonControlsEx(&icc);

        HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
        HWND hStatic = CreateWindowW(L"STATIC",
            L"Select a target. Click a column to sort. Metric cells are color-coded green/yellow/red by relative severity.",
            WS_CHILD | WS_VISIBLE,
            12, 10, 1420, 22,
            hwnd, nullptr, GetModuleHandleW(nullptr), nullptr);
        if (hStatic && hFont) SendMessageW(hStatic, WM_SETFONT, (WPARAM)hFont, TRUE);
        if (state->darkUi && hStatic) TrySetDarkTheme(hStatic);

        state->hList = CreateWindowExW(WS_EX_CLIENTEDGE,
            WC_LISTVIEWW,
            nullptr,
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS,
            12, 38, 1420, 365,
            hwnd, reinterpret_cast<HMENU>(IDC_CANDIDATE_LIST),
            GetModuleHandleW(nullptr), nullptr);

        if (state->hList) {
            ListView_SetExtendedListViewStyle(state->hList, LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
            if (state->darkUi) {
                TrySetDarkTheme(state->hList);
                ListView_SetBkColor(state->hList, DarkBg());
                ListView_SetTextBkColor(state->hList, DarkBg());
                ListView_SetTextColor(state->hList, DarkText());
            }
            if (hFont) SendMessageW(state->hList, WM_SETFONT, (WPARAM)hFont, TRUE);
            InsertListColumn(state->hList, 0, L"#", 42);
            InsertListColumn(state->hList, 1, L"Target", 260);
            InsertListColumn(state->hList, 2, L"PID(s)", 90);
            InsertListColumn(state->hList, 3, L"Score", 100);
            InsertListColumn(state->hList, 4, L"CPU total %", 115);
            InsertListColumn(state->hList, 5, L"Page faults", 135);
            InsertListColumn(state->hList, 6, L"Disk MB", 115);
            InsertListColumn(state->hList, 7, L"Mem MB", 115);
            InsertListColumn(state->hList, 8, L"Path", 640);
            PopulateCandidateList(state);
        }

        state->hRefresh = CreateWindowW(L"BUTTON", L"Refresh",
            WS_CHILD | WS_VISIBLE,
            12, 420, 120, 32,
            hwnd, reinterpret_cast<HMENU>(IDC_REFRESH_BUTTON), GetModuleHandleW(nullptr), nullptr);
        state->hLive = CreateWindowW(L"BUTTON", L"Live update",
            WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
            150, 426, 155, 24,
            hwnd, reinterpret_cast<HMENU>(IDC_LIVE_CHECK), GetModuleHandleW(nullptr), nullptr);
        CreateWindowW(L"STATIC", L"Poll ms:",
            WS_CHILD | WS_VISIBLE,
            320, 430, 60, 20,
            hwnd, nullptr, GetModuleHandleW(nullptr), nullptr);
        state->hPollEdit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", nullptr,
            WS_CHILD | WS_VISIBLE | ES_NUMBER,
            386, 424, 90, 28,
            hwnd, reinterpret_cast<HMENU>(IDC_POLL_EDIT), GetModuleHandleW(nullptr), nullptr);
        state->hStatus = CreateWindowW(L"STATIC", L"Ready",
            WS_CHILD | WS_VISIBLE,
            492, 430, 460, 20,
            hwnd, reinterpret_cast<HMENU>(IDC_STATUS_STATIC), GetModuleHandleW(nullptr), nullptr);

        HWND hOk = CreateWindowW(L"BUTTON", L"Terminate selected",
            WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
            1070, 420, 190, 32,
            hwnd, reinterpret_cast<HMENU>(IDOK), GetModuleHandleW(nullptr), nullptr);
        HWND hCancel = CreateWindowW(L"BUTTON", L"Cancel",
            WS_CHILD | WS_VISIBLE,
            1276, 420, 155, 32,
            hwnd, reinterpret_cast<HMENU>(IDCANCEL), GetModuleHandleW(nullptr), nullptr);

        if (state->darkUi) {
            TrySetDarkTheme(state->hRefresh);
            TrySetDarkTheme(state->hLive);
            TrySetDarkTheme(state->hPollEdit);
            TrySetDarkTheme(state->hStatus);
            TrySetDarkTheme(hOk);
            TrySetDarkTheme(hCancel);
        }

        HWND controls[] = { state->hRefresh, state->hLive, state->hPollEdit, state->hStatus, hOk, hCancel };
        for (HWND control : controls) {
            if (control && hFont) SendMessageW(control, WM_SETFONT, (WPARAM)hFont, TRUE);
        }

        WCHAR pollText[32] = {};
        swprintf(pollText, ARRAYSIZE(pollText), L"%lu", state->pollMs);
        SetWindowTextW(state->hPollEdit, pollText);
        if (state->live && state->hLive) {
            SendMessageW(state->hLive, BM_SETCHECK, BST_CHECKED, 0);
            SetTimer(hwnd, TIMER_LIVE_REFRESH, state->pollMs, nullptr);
        }
        return 0;
    }
    case WM_COMMAND: {
        WORD id = LOWORD(wParam);
        if (id == IDOK) {
            int idx = state ? state->visualSelectedRow : -1;
            state->selectedIndex = idx >= 0 ? idx : 0;
            DestroyWindow(hwnd);
            return 0;
        }
        if (id == IDCANCEL) {
            if (state) state->selectedIndex = -1;
            DestroyWindow(hwnd);
            return 0;
        }
        if (id == IDC_REFRESH_BUTTON) {
            RefreshCandidateData(hwnd, state);
            return 0;
        }
        if (id == IDC_LIVE_CHECK) {
            bool checked = state && state->hLive && SendMessageW(state->hLive, BM_GETCHECK, 0, 0) == BST_CHECKED;
            if (checked) {
                state->pollMs = ReadPollInterval(state);
                SetTimer(hwnd, TIMER_LIVE_REFRESH, state->pollMs, nullptr);
                SetStatus(state, L"Live update enabled.");
            } else {
                KillTimer(hwnd, TIMER_LIVE_REFRESH);
                SetStatus(state, L"Live update disabled.");
            }
            return 0;
        }
        break;
    }
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLOREDIT: {
        if (state && state->darkUi) {
            HDC hdc = reinterpret_cast<HDC>(wParam);
            SetTextColor(hdc, DarkText());
            SetBkColor(hdc, msg == WM_CTLCOLOREDIT ? DarkPanelBg() : DarkBg());
            return reinterpret_cast<LRESULT>(msg == WM_CTLCOLOREDIT ? state->hDarkEditBrush : state->hDarkBrush);
        }
        break;
    }
    case WM_ERASEBKGND:
        if (state && state->darkUi) {
            HDC hdc = reinterpret_cast<HDC>(wParam);
            RECT rc{};
            GetClientRect(hwnd, &rc);
            FillRect(hdc, &rc, state->hDarkBrush);
            return 1;
        }
        break;
    case WM_TIMER:
        if (wParam == TIMER_LIVE_REFRESH) {
            RefreshCandidateData(hwnd, state);
            return 0;
        }
        break;
    case WM_NOTIFY: {
        auto hdr = reinterpret_cast<NMHDR*>(lParam);
        if (state && hdr && hdr->idFrom == IDC_CANDIDATE_LIST) {
            if (hdr->code == NM_CLICK) {
                DWORD pos = GetMessagePos();
                POINT pt{ (LONG)(SHORT)LOWORD(pos), (LONG)(SHORT)HIWORD(pos) };
                ScreenToClient(state->hList, &pt);
                LVHITTESTINFO hit{};
                hit.pt = pt;
                int hitRow = ListView_SubItemHitTest(state->hList, &hit);
                if (hitRow >= 0) {
                    state->visualSelectedRow = hitRow;
                    SetFocus(state->hList);
                    InvalidateRect(state->hList, nullptr, TRUE);
                    return 0;
                }
            }
            if (hdr->code == LVN_ITEMCHANGED) {
                auto lv = reinterpret_cast<NMLISTVIEW*>(lParam);
                if (lv && lv->iItem >= 0 && (lv->uNewState & LVIS_SELECTED)) {
                    state->visualSelectedRow = lv->iItem;
                    ListView_SetItemState(state->hList, -1, 0, LVIS_SELECTED | LVIS_FOCUSED);
                    InvalidateRect(state->hList, nullptr, TRUE);
                    return 0;
                }
            }
            if (hdr->code == LVN_COLUMNCLICK) {
                auto lv = reinterpret_cast<NMLISTVIEW*>(lParam);
                if (state->sortColumn == lv->iSubItem) {
                    state->sortDescending = !state->sortDescending;
                } else {
                    state->sortColumn = lv->iSubItem;
                    state->sortDescending = true;
                }
                PopulateCandidateList(state);
                return 0;
            }
            if (hdr->code == NM_DBLCLK) {
                DWORD pos = GetMessagePos();
                POINT pt{ (LONG)(SHORT)LOWORD(pos), (LONG)(SHORT)HIWORD(pos) };
                ScreenToClient(state->hList, &pt);
                LVHITTESTINFO hit{};
                hit.pt = pt;
                int hitRow = ListView_SubItemHitTest(state->hList, &hit);
                int idx = hitRow >= 0 ? hitRow : state->visualSelectedRow;
                state->visualSelectedRow = idx;
                state->selectedIndex = idx >= 0 ? idx : 0;
                DestroyWindow(hwnd);
                return 0;
            }
            if (hdr->code == NM_CUSTOMDRAW) {
                auto cd = reinterpret_cast<NMLVCUSTOMDRAW*>(lParam);
                if (cd->nmcd.dwDrawStage == CDDS_PREPAINT) {
                    return CDRF_NOTIFYITEMDRAW;
                }
                if (cd->nmcd.dwDrawStage == CDDS_ITEMPREPAINT) {
                    return CDRF_NOTIFYSUBITEMDRAW;
                }
                if (cd->nmcd.dwDrawStage == (CDDS_ITEMPREPAINT | CDDS_SUBITEM)) {
                    int subItem = cd->iSubItem;
                    int row = (int)cd->nmcd.dwItemSpec;
                    if (state->candidates && row >= 0 && row < (int)state->candidates->size()) {
                        if (subItem == 1 && row == state->visualSelectedRow) {
                            cd->clrTextBk = TargetSelectedBg();
                            cd->clrText = RGB(255, 255, 255);
                            return CDRF_NEWFONT;
                        }
                        if (subItem >= 3 && subItem <= 7) {
                            double value = CandidateMetricForColumn((*state->candidates)[row], subItem);
                            double maxValue = MaxMetricForColumn(state, subItem);
                            double normalized = (maxValue > 0.0) ? value / maxValue : 0.0;
                            cd->clrTextBk = SeverityColor(normalized);
                            cd->clrText = RGB(0, 0, 0);
                            return CDRF_NEWFONT;
                        }
                        if (state->darkUi) {
                            cd->clrTextBk = DarkBg();
                            cd->clrText = DarkText();
                            return CDRF_NEWFONT;
                        }
                    }
                }
            }
        }
        break;
    }
    case WM_CLOSE:
        if (state) state->selectedIndex = -1;
        DestroyWindow(hwnd);
        return 0;
    case WM_DESTROY:
        KillTimer(hwnd, TIMER_LIVE_REFRESH);
        if (state) {
            if (state->hDarkBrush) {
                DeleteObject(state->hDarkBrush);
                state->hDarkBrush = nullptr;
            }
            if (state->hDarkEditBrush) {
                DeleteObject(state->hDarkEditBrush);
                state->hDarkEditBrush = nullptr;
            }
        }
        return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static int ShowCandidateChooser(std::vector<KillDecision>& candidates) {
    if (candidates.empty()) {
        return -1;
    }
    if (candidates.size() == 1) {
        return 0;
    }

    HINSTANCE hInst = GetModuleHandleW(nullptr);
    const wchar_t* className = L"ResourceAwareTaskTerminator_CandidateChooser";

    WNDCLASSW wc{};
    wc.lpfnWndProc = CandidateChooserProc;
    wc.hInstance = hInst;
    wc.lpszClassName = className;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    RegisterClassW(&wc);

    CandidateDialogState state;
    state.candidates = &candidates;
    state.mode = candidates[0].mode;
    state.sampleMs = candidates[0].sampleMs;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        state.candidateLimit = g_candidateCount;
        state.pollMs = g_livePollMs;
        state.live = g_liveUpdateDefault;
        state.darkUi = g_darkChooserUi;
    }
    state.sortColumn = 3;
    state.sortDescending = true;

    RECT rc{0, 0, 1460, 500};
    AdjustWindowRectEx(&rc, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, FALSE, WS_EX_TOPMOST | WS_EX_TOOLWINDOW);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;
    int x = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
    int y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

    HWND hwnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        className,
        L"Select Resource-Aware Task Target",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        x, y, width, height,
        nullptr, nullptr, hInst, &state);
    if (!hwnd) {
        return 0;
    }

    ShowWindow(hwnd, SW_SHOW);
    SetForegroundWindow(hwnd);

    MSG msg{};
    while (IsWindow(hwnd) && GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (!IsDialogMessageW(hwnd, &msg)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    return state.selectedIndex;
}

static std::wstring BuildConfirmationMessage(const KillDecision& decision) {
    std::wstring message;
    message += L"Resource-Aware Task Terminator is about to terminate a process.\n\n";
    if (decision.terminateGroup) {
        message += L"Target app group: " + decision.exeName + L"\n";
        message += L"Primary PID: " + std::to_wstring(decision.pid) + L"\n";
        message += L"Eligible group PIDs: " + BuildPidListText(decision.targetPids) + L"\n";
        message += L"Group process count: " + std::to_wstring(decision.targetPids.size()) + L"\n";
    } else {
        message += L"Target PID: " + std::to_wstring(decision.pid) + L"\n";
    }
    message += L"Executable: " + decision.exeName + L"\n";
    message += L"Path: " + decision.processPath + L"\n\n";

    message += L"Decision mode: ";
    message += KillModeName(decision.mode);
    message += L"\n";
    message += L"Reason: " + decision.reason + L"\n";
    message += L"Sample window: " + std::to_wstring(decision.sampleMs) + L" ms\n";
    message += L"Evidence: " + decision.metricName + L" = " + FormatDouble(decision.metricValue) + L"\n";
    if (decision.mode == KillMode::AggregateSuperScore) {
        message += L"Aggregate components:\n";
        message += L"  CPU total %: " + FormatDouble(decision.cpuPercent) + L"\n";
        message += L"  Page-fault delta: " + FormatDouble(decision.pageFaultDelta) + L"\n";
        message += L"  Disk I/O bytes: " + FormatDouble(decision.diskIoBytes) + L"\n";
        message += L"  Working set MB: " + FormatDouble(decision.memoryMb) + L"\n";
    }

    if (decision.foregroundWindow) {
        message += L"\nForeground window details:\n";
        WCHAR hwndBuffer[32] = {};
        swprintf(hwndBuffer, ARRAYSIZE(hwndBuffer), L"0x%p", decision.foregroundWindow);
        message += L"HWND: ";
        message += hwndBuffer;
        message += L"\nTitle: " + (decision.windowTitle.empty() ? L"<empty>" : decision.windowTitle) + L"\n";
        message += L"Class: " + (decision.windowClass.empty() ? L"<empty>" : decision.windowClass) + L"\n";
    }

    message += L"\nProtection check: PID was not in the protected-app list, was not a known critical system process, and is not the tool process itself.\n";
    message += decision.terminateGroup ? L"\nTerminate all eligible processes in this app group now?" : L"\nTerminate this process now?";
    return message;
}

static bool ConfirmKillDecision(const KillDecision& decision) {
    bool disableConfirmation = false;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        disableConfirmation = g_disableConfirmation;
    }

    if (disableConfirmation) {
        return true;
    }

    std::wstring message = BuildConfirmationMessage(decision);
    int result = MessageBoxW(
        nullptr,
        message.c_str(),
        L"Confirm Force Kill",
        MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2 | MB_TOPMOST | MB_SETFOREGROUND | MB_TASKMODAL);
    return result == IDYES;
}

static LRESULT CALLBACK HiddenWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_HOTKEY && wParam == HOTKEY_ID) {
        if (g_killInProgress.exchange(true)) {
            Wh_Log(L"Kill already in progress; ignoring hotkey.");
            return 0;
        }
        struct ResetBusy {
            ~ResetBusy() { g_killInProgress.store(false); }
        } resetBusy;

        KillDecision decision;
        if (!ResolveTargetDecision(decision)) {
            Wh_Log(L"No suitable target found.");
            return 0;
        }

        std::vector<DWORD> targetPids = decision.targetPids.empty()
            ? std::vector<DWORD>{ decision.pid }
            : decision.targetPids;

        for (DWORD pid : targetPids) {
            if (IsProcessProtected(pid)) {
                Wh_Log(L"Chosen PID %lu is protected. Skipping entire request.", pid);
                return 0;
            }
        }

        if (!ConfirmKillDecision(decision)) {
            Wh_Log(L"Kill cancelled by user for primary PID: %lu", decision.pid);
            return 0;
        }

        int terminatedCount = 0;
        for (DWORD pid : targetPids) {
            // Re-check immediately before termination in case the target changed
            // while the confirmation dialog was open.
            if (IsProcessProtected(pid)) {
                Wh_Log(L"PID %lu became protected before termination. Skipping PID.", pid);
                continue;
            }

            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
            if (hProcess) {
                TerminateProcess(hProcess, 1);
                CloseHandle(hProcess);
                terminatedCount++;
                Wh_Log(L"Terminated PID: %lu", pid);
            } else {
                Wh_Log(L"Failed to terminate PID: %lu (try running Windhawk as admin).", pid);
            }
        }
        Wh_Log(L"Termination request finished. Terminated %d/%d process(es).",
               terminatedCount, (int)targetPids.size());
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

static DWORD WINAPI HotkeyThread(LPVOID) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = HiddenWindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"ResourceAwareTaskTerminator_HotkeyHost_Class";
    RegisterClass(&wc);

    g_hHiddenWindow =
        CreateWindowEx(0, wc.lpszClassName, L"ResourceAwareTaskTerminator_HotkeyHost", 0, 0, 0, 0, 0,
                       HWND_MESSAGE, NULL, wc.hInstance, NULL);
    if (!g_hHiddenWindow) {
        Wh_Log(L"CreateWindowEx failed (error=%lu); aborting hotkey thread.", GetLastError());
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        if (!RegisterHotKey(g_hHiddenWindow, HOTKEY_ID, g_modifiers, g_vkCode)) {
            Wh_Log(L"Failed to register hotkey (modifiers=%u, vk=%u, error=%lu).", g_modifiers, g_vkCode, GetLastError());
        } else {
            Wh_Log(L"Registered hotkey (modifiers=%u, vk=%u).", g_modifiers, g_vkCode);
        }
    }

    MSG msg;
    PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        if (msg.message == WM_RELOAD_HOTKEY) {
            UnregisterHotKey(g_hHiddenWindow, HOTKEY_ID);
            std::lock_guard<std::mutex> lock(g_settingsMutex);
            if (!RegisterHotKey(g_hHiddenWindow, HOTKEY_ID, g_modifiers, g_vkCode)) {
                Wh_Log(L"Failed to re-register hotkey after settings change (modifiers=%u, vk=%u, error=%lu).", g_modifiers, g_vkCode, GetLastError());
            } else {
                Wh_Log(L"Re-registered hotkey (modifiers=%u, vk=%u).", g_modifiers, g_vkCode);
            }
            continue;
        }

        if (msg.message == WM_HOTKEY && msg.wParam == HOTKEY_ID) {
            SendMessage(g_hHiddenWindow, WM_HOTKEY, msg.wParam, msg.lParam);
            continue;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_hHiddenWindow) {
        UnregisterHotKey(g_hHiddenWindow, HOTKEY_ID);
        DestroyWindow(g_hHiddenWindow);
        g_hHiddenWindow = NULL;
    }
    UnregisterClass(wc.lpszClassName, wc.hInstance);
    return 0;
}

// ------------------------ Dedicated tool-process host ------------------------
// This mod runs as a dedicated Windhawk tool process:
//     windhawk.exe -tool-mod "resource-aware-task-terminator"
// The normal loaded mod only launches that process.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

static void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL WhTool_ModInit() {
    LoadSettings();
    g_hThread = CreateThread(NULL, 0, HotkeyThread, NULL, 0, &g_dwThreadId);
    if (!g_hThread) {
        Wh_Log(L"Failed to create hotkey thread (%lu).", GetLastError());
        return FALSE;
    }
    Wh_Log(L"Tool hotkey host started. PID=%lu thread=%lu.", GetCurrentProcessId(), g_dwThreadId);
    return TRUE;
}

void WhTool_ModUninit() {
    if (g_dwThreadId) {
        PostThreadMessage(g_dwThreadId, WM_QUIT, 0, 0);
    }
    if (g_hThread) {
        WaitForSingleObject(g_hThread, 5000);
        CloseHandle(g_hThread);
        g_hThread = NULL;
        g_dwThreadId = 0;
    }
    if (g_toolModProcessMutex) {
        CloseHandle(g_toolModProcessMutex);
        g_toolModProcessMutex = NULL;
    }
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    if (g_dwThreadId) {
        PostThreadMessage(g_dwThreadId, WM_RELOAD_HOTKEY, 0, 0);
    }
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) && sessionId == 0) {
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
        g_toolModProcessMutex = CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
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
        IMAGE_NT_HEADERS* ntHeaders = (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

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
    switch (GetModuleFileName(nullptr, currentProcessPath, ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR commandLine[MAX_PATH + 2 +
                      (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath, WH_MOD_ID);

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
        (CreateProcessInternalW_t)GetProcAddress(kernelModule, "CreateProcessInternalW");
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

