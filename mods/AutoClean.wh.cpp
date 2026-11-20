// ==WindhawkMod==
// @id              autoclean
// @name            AutoClean
// @description     Zero-UI event-driven background temp cleaner with AI agent temp protection, KB/MB/GB/TB thresholds, oldest-first pruning, and OLED toasts
// @version         2.2026.1811
// @author          Ashish
// @github          https://github.com/k-ashix
// @twitter         https://x.com/k_ashix
// @donateUrl       https://ko-fi.com/kashix
// @include         windhawk-mod.exe
// @include         windhawk.exe
// @compilerOptions -ladvapi32 -lole32 -loleaut32 -lshell32 -lshlwapi -luser32 -lgdi32 -lgdiplus -ldwmapi -lwtsapi32 -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# AutoClean

Silent, zero-UI background temporary folder cleaner running in a dedicated Windhawk Tool Mod process.

## Action Triggers — Temp & Recycle Bin
| Icon | Shortcut / Trigger | Action |
|---|---|---|
| 🖱️ | `3x Left-Click` (`Desktop & Taskbar`) | Clean Temp folders immediately and show OLED corner toast |
| 🖱️ | `Middle-Click` (`1x`/`2x`/`3x`, `Desktop & Taskbar`) | Empty Recycle Bin immediately and show OLED corner toast |
| 🚀 | `Windows Login / Startup` | Clean Temp folders and/or Recycle Bin automatically after login |
| 📦 | `Size Limit` (`KB`/`MB`/`GB`/`TB`) | Prune oldest items first or full sweep when Temp or Recycle Bin hits limit |
| 🔢 | `File Count Limit` (`X items`) | Prune oldest-deleted items or empty Recycle Bin at `X` items |
| ⏱️ | `Timer Schedule` (`15s` to `Days`) | Clean Temp and Recycle Bin on independent repeating schedules |
| ⚠️ | `Low Disk Space Emergency` | Immediately free Temp and Recycle Bin when drive free space drops below floor |

## Features
- **Event-Driven Watching**: Uses `ReadDirectoryChangesW` so idle CPU and disk usage stay at zero until temp files are actually written.
- **Dual Mutual Triggers**: Run by **Size Threshold** (`KB`/`MB`/`GB`/`TB`, debounced 15–30s+), by **Periodic Timer** (`15s` up to `Days`), or **both together**.
- **Oldest-First Target Pruning vs. Full Sweep**: Separate toggles let you prune only the oldest files down to a target percentage of your threshold, or perform a full sweep of all eligible files.
- **Background I/O Priority**: Runs scans and deletions under `THREAD_MODE_BACKGROUND_BEGIN` so foreground builds, Photoshop, Blender, and games never stall.
- **Fullscreen & Battery Smart Deferral**: Automatically defers non-emergency runs during fullscreen apps/games or on battery power.
- **Low-Disk Emergency Floor**: Triggers an immediate cleanup if drive free space drops below your `MB`/`GB`/`TB` floor.
- **AI Agents Temp Protection**: Respects local `%TEMP%` folders created by AI coding & desktop agents (Claude Code, Codex, Hermes Agent, OpenClaw, Gemini CLI, OpenCode, Cursor, Windsurf, Goose, Aider, Copilot, MCP) by default, with a configurable pattern list so agents manage their own scratchpads and worktrees.
- **Security & Safety Guards**: Strict `%TEMP%` allowlist (`C:\Windows\Temp` opt-in without UAC), reparse-point/junction/symlink blocking, Windows file-lock respect, minimum file age grace period, wildcard exclusions, and Dry-Run logging.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- aiAgentProtection:
  - protectionMode: Enabled
    $name: Let AI agents handle their own Temp
    $description: Keep local %TEMP% folders and scratchpads owned by AI agents untouched during all automatic, scheduled, startup, and manual Temp cleanups.
    $options:
    - Enabled: On — Respect AI agents' local Temp folders (Recommended)
    - Disabled: Off — Clean AI agent Temp files too
  - agentTempPatterns:
    - "claude*"
    - "tmpclaude-*"
    - "wvm-*"
    - "codex*"
    - "openai-codex*"
    - "hermes*"
    - ".hermes*"
    - "openclaw*"
    - ".openclaw*"
    - "gemini-*"
    - "opencode*"
    - "cursor-*"
    - "windsurf-*"
    - "goose-*"
    - "aider*"
    - "copilot-*"
    - "mcp-*"
    $name: Protected AI agent Temp folder & file patterns
    $description: Wildcard folder/file names inside %TEMP% to preserve for AI agents (e.g. claude*, codex*, hermes*, openclaw*). Add your own custom agent patterns here.
  $name: 1. AI Agents Temp Protection
- tempCleaner:
  - thresholdEnabled: true
    $name: Enable Temp size threshold trigger
    $description: Automatically clean when Temp folder size reaches or exceeds the limit.
  - thresholdValue: 500
    $name: Temp size limit
    $description: Numeric size limit (minimum 1).
  - thresholdUnit: MB
    $name: Temp size unit
    $options:
    - KB: KB
    - MB: MB
    - GB: GB
    - TB: TB
  - debounceSeconds: 20
    $name: Write debounce interval (seconds)
    $description: Wait time after Temp writes before measuring size (5 to 3600 seconds).
  - timerEnabled: true
    $name: Enable Temp periodic timer
    $description: Automatically clean Temp folders on a fixed schedule (default every 1 hour).
  - timerValue: 1
    $name: Temp timer interval
    $description: Numeric interval (enforced minimum 15 seconds total).
  - timerUnit: Hours
    $name: Temp timer unit
    $options:
    - Seconds: Seconds
    - Minutes: Minutes
    - Hours: Hours
    - Days: Days
  - oldestFirst: true
    $name: Oldest-first target pruning
    $description: Prune oldest-modified eligible files first until Temp size drops to the target percentage.
  - fullSweep: false
    $name: Always perform full sweep
    $description: Delete all eligible Temp files instead of stopping at the target percentage.
  - targetPercent: 50
    $name: Target percentage to keep (%)
    $description: Stop pruning oldest-first once Temp size is at or below this percentage (10 to 90%).
  - mouseGesture: TripleLeftClick
    $name: Mouse shortcut to clean Temp folders
    $description: Triple left-click (3x) on empty Desktop or Taskbar space to clean Temp immediately.
    $options:
    - Disabled: Off (Disabled)
    - TripleLeftClick: Triple Left-Click (3x - Recommended)
  - mouseScope: DesktopAndTaskbar
    $name: Where Temp mouse shortcut works
    $description: Choose whether 3x left-click works on empty Desktop & Taskbar, Taskbar only, or Desktop only.
    $options:
    - DesktopAndTaskbar: Desktop & Taskbar empty space (Recommended)
    - TaskbarOnly: Taskbar empty space only
    - DesktopOnly: Desktop empty space only
  - minFileAgeMinutes: 10
    $name: Minimum file age (minutes)
    $description: Skip Temp files modified more recently than this grace period (0 disables).
  - cooldownSeconds: 30
    $name: Post-clean cooldown (seconds)
    $description: Minimum wait after a Temp cleanup before running another automatic pass.
  - cleanWindowsTemp: false
    $name: Also clean accessible C:\Windows\Temp
    $description: Best-effort cleanup of user-accessible unlocked files without UAC prompts.
  - excludePatterns:
    - "*.lock"
    - "*.lck"
    - "windhawk*"
    $name: Exclude wildcard patterns
    $description: File or folder name patterns to skip (e.g. *.lock, windhawk*).
  $name: 2. Temp Folder Cleaner
- recycleBinCleaner:
  - sizeThresholdEnabled: true
    $name: Enable Recycle Bin size threshold
    $description: Automatically clean the Recycle Bin when its total size reaches the limit.
  - sizeThresholdValue: 500
    $name: Recycle Bin size limit
    $description: Numeric size limit (minimum 1).
  - sizeThresholdUnit: MB
    $name: Recycle Bin size unit
    $options:
    - KB: KB
    - MB: MB
    - GB: GB
    - TB: TB
  - countThresholdEnabled: false
    $name: Enable Recycle Bin item-count limit
    $description: Automatically clean the Recycle Bin when deleted item count reaches the limit.
  - maxItems: 50
    $name: Maximum items in Recycle Bin
    $description: Clean when the Recycle Bin holds at least this many items (minimum 1).
  - timerEnabled: true
    $name: Enable Recycle Bin 30-day / periodic auto-clean
    $description: Automatically clean Recycle Bin items older than the schedule (default 30 Days) and on schedule across reboots.
  - timerValue: 30
    $name: Recycle Bin retention / schedule interval
    $description: Auto-clean items older than this interval and run scheduled pruning (default 30 Days).
  - timerUnit: Days
    $name: Recycle Bin timer unit
    $options:
    - Seconds: Seconds
    - Minutes: Minutes
    - Hours: Hours
    - Days: Days
  - fullSweep: false
    $name: Empty entire Recycle Bin (Full Sweep)
    $description: Permanently remove all items in the Recycle Bin (keep off so scheduled runs prune items >30 days & oldest-first).
  - oldestFirst: true
    $name: Oldest-first & 30-day age pruning
    $description: When Full Sweep is off, automatically prune items older than the schedule (30 days) or oldest-first down to target %.
  - targetPercent: 50
    $name: Target percentage to keep (%)
    $description: Stop pruning oldest items once the Recycle Bin shrinks to this percentage (10 to 90%).
  - minItemAgeMinutes: 0
    $name: Minimum deleted item age (minutes)
    $description: Keep recently deleted items during oldest-first pruning (0 disables).
  - checkIntervalSeconds: 30
    $name: Background check interval (seconds)
    $description: How often to check Recycle Bin size and item count (15 to 3600 seconds).
  - cooldownSeconds: 15
    $name: Post-clean cooldown (seconds)
    $description: Minimum wait between automatic Recycle Bin cleanups (5 to 3600 seconds).
  - mouseGesture: DoubleMiddleClick
    $name: Middle-click shortcut to empty Recycle Bin
    $description: Click the middle mouse button (scroll wheel) to empty the Recycle Bin immediately.
    $options:
    - Disabled: Off (Disabled)
    - SingleMiddleClick: Single Middle-Click (1x)
    - DoubleMiddleClick: Double Middle-Click (2x - Recommended)
    - TripleMiddleClick: Triple Middle-Click (3x)
  - mouseScope: DesktopAndTaskbar
    $name: Where Recycle Bin middle-click works
    $description: Desktop & Taskbar only prevents accidental triggers in browsers or 3D/CAD apps.
    $options:
    - DesktopAndTaskbar: Desktop & Taskbar only (Recommended)
    - Anywhere: Anywhere on screen (except fullscreen games)
  - mouseSwallowTriggerClick: true
    $name: Block triggering middle-click from reaching apps
    $description: Stop the final middle-click from closing a taskbar window or starting auto-scroll.
  - playSound: false
    $name: Play Windows sound when emptied
    $description: Play the standard Windows Recycle Bin sound on full empty.
  $name: 3. Recycle Bin Cleaner
- universalSafety:
  - cleanTempOnStart: true
    $name: Clean Temp folders on startup
    $description: Automatically clean Temp folders after Windows login or mod load.
  - cleanRecycleBinOnStart: true
    $name: Empty Recycle Bin on startup
    $description: Automatically clean the Recycle Bin after Windows login or mod load.
  - startupDelaySeconds: 12
    $name: Lazy post-login delay (seconds)
    $description: Wait time after actual Windows desktop login & taskbar load before running startup cleanups (5 to 300 seconds).
  - startupFullSweep: true
    $name: Full sweep on startup
    $description: Perform a full sweep of Temp and Recycle Bin on startup instead of keeping a percentage.
  - emergencyEnabled: true
    $name: Enable low-disk emergency floor
    $description: Trigger an immediate cleanup if free space on the Temp drive drops below the floor.
  - freeSpaceValue: 5
    $name: Minimum free disk space
    $description: Numeric free space floor (minimum 1).
  - freeSpaceUnit: GB
    $name: Free space unit
    $options:
    - MB: MB
    - GB: GB
    - TB: TB
  - emergencyCheckSeconds: 60
    $name: Free space check interval (seconds)
    $description: How often to check drive free space (15 to 3600 seconds).
  - cleanRecycleOnEmergency: true
    $name: Also empty Recycle Bin on low-disk emergency
    $description: Empty the Recycle Bin in addition to Temp folders during a low-disk emergency.
  - bypassDeferralOnEmergency: true
    $name: Bypass fullscreen/battery deferral on emergency
    $description: Run immediately during low-disk emergencies even if fullscreen or on battery.
  - deferOnFullscreen: true
    $name: Defer during fullscreen apps/games
    $description: Pause non-emergency cleanups while a fullscreen or D3D app is active.
  - deferOnBattery: true
    $name: Defer while on battery power
    $description: Pause non-emergency cleanups when running on battery power.
  - mouseMaxIntervalMs: 400
    $name: Multi-click speed window (ms)
    $description: Maximum time between clicks for 2x/3x mouse shortcuts (150 to 800 ms).
  - toastEnabled: true
    $name: Show OLED corner toast notification
    $description: Display an OLED black corner toast when Temp or Recycle Bin cleanup runs.
  - toastTriggerScope: AllCleanups
    $name: When to show toast notifications
    $description: Show OLED corner toasts for both scheduled/automatic cleanups and manual shortcuts, or manual shortcuts only.
    $options:
    - AllCleanups: All cleanups — Scheduled, Automatic & Manual (Recommended)
    - ManualOnly: Manual mouse shortcuts only
  - toastDurationMs: 3200
    $name: Toast display duration (ms)
    $description: How long the corner toast stays visible before fading out (1200 to 10000 ms).
  - screenshotBehavior: ExcludeFromCapture
    $name: Toast visibility in screenshots & recordings
    $description: Choose whether the toast is hidden from screenshots/OBS or shown normally in screenshots.
    $options:
    - ExcludeFromCapture: Hide toast from screenshots & screen recordings (Exclude)
    - ShowInCapture: Keep toast visible inside screenshots & recordings (Ignore/Show)
  - dryRun: false
    $name: Dry-run preview mode (single-shot simulation, no deletions)
    $description: Runs a single simulation pass on screen showing what would be cleaned without touching any file.
  $name: 4. Universal, Toast & Safety
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <objbase.h>
#include <sddl.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <dwmapi.h>
#include <wtsapi32.h>
#include <gdiplus.h>
#include <uiautomation.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <deque>
#include <limits>
#include <string>
#include <vector>

// ============================================================================
// Settings & State (Ledger)
// ============================================================================

struct ModSettings {
    bool thresholdEnabled = true;
    uint64_t thresholdBytes = 500ULL * 1024ULL * 1024ULL;
    uint64_t debounceMs = 20000ULL;

    bool timerEnabled = true;
    uint64_t timerIntervalMs = 3600000ULL;

    bool oldestFirst = true;
    bool fullSweep = false;
    int targetPercent = 50;

    bool emergencyEnabled = true;
    uint64_t emergencyFloorBytes = 5ULL * 1024ULL * 1024ULL * 1024ULL;
    uint64_t emergencyCheckMs = 60000ULL;
    bool emergencyBypassDeferral = true;

    bool deferOnFullscreen = true;
    bool deferOnBattery = true;

    uint64_t minFileAgeMs = 600000ULL;
    uint64_t cooldownMs = 30000ULL;
    bool cleanWindowsTemp = false;
    bool dryRun = false;
    bool aiAgentProtectionEnabled = true;
    std::vector<std::wstring> aiAgentPatterns;
    std::vector<std::wstring> excludePatterns;

    // Startup & Windows Login (12s post-login lazy start)
    bool startupCleanTemp = true;
    bool startupCleanRecycle = true;
    uint64_t startupDelayMs = 12000ULL;
    bool startupFullSweep = true;

    // Recycle Bin Cleaner (default: 30-day retention & schedule enabled)
    bool rbSizeThresholdEnabled = true;
    uint64_t rbSizeThresholdBytes = 500ULL * 1024ULL * 1024ULL;
    bool rbCountThresholdEnabled = false;
    uint64_t rbMaxItems = 50ULL;
    bool rbTimerEnabled = true;
    uint64_t rbTimerIntervalMs = 30ULL * 86400ULL * 1000ULL;
    bool rbFullSweep = false;
    bool rbOldestFirst = true;
    int rbTargetPercent = 50;
    uint64_t rbMinAgeMs = 0ULL;
    uint64_t rbCheckMs = 30000ULL;
    uint64_t rbCooldownMs = 15000ULL;
    int rbMouseClicksRequired = 2;
    int rbMouseScope = 0; // 0=DesktopAndTaskbar, 1=Anywhere
    uint64_t rbMouseMaxIntervalMs = 400ULL;
    bool rbMouseSwallowClick = true;
    bool rbPlaySound = false;
    bool cleanRecycleOnEmergency = true;

    // Taskbar 3x Left-Click Temp Shortcut
    bool tempGestureEnabled = true;
    bool tempGestureEmptySpaceOnly = true;
    uint64_t tempGestureMaxIntervalMs = 400ULL;

    // Taskbar & Desktop 3x Left-Click Temp Shortcut Scope (0=DesktopAndTaskbar, 1=TaskbarOnly, 2=DesktopOnly)
    int tempGestureScope = 0;

    // Corner Toast Feedback & Screenshot Visibility
    bool toastEnabled = true;
    bool toastShowOnAuto = true;
    uint32_t toastDurationMs = 3200U;
    bool toastExcludeFromCapture = true;
};

static CRITICAL_SECTION g_settingsMtx;
static bool g_settingsMtxInit = false;
static ModSettings g_settings;
static HANDLE g_wakeEvent = nullptr;
static HANDLE g_workerThread = nullptr;
static HANDLE g_mouseThread = nullptr;
static DWORD g_mouseThreadId = 0;
static HANDLE g_uiThread = nullptr;
static HANDLE g_uiReadyEvent = nullptr;
static HWND g_uiMsgHwnd = nullptr;
static std::atomic<int> g_mouseClicksRequired{2};
static std::atomic<int> g_mouseTriggerScope{0};
static std::atomic<uint32_t> g_mouseMaxIntervalMs{400};
static std::atomic<bool> g_mouseSwallowClick{true};
static std::atomic<bool> g_tempGestureEnabled{true};
static std::atomic<int> g_tempGestureScope{0};
static std::atomic<uint32_t> g_tempGestureMaxIntervalMs{400};
static std::atomic<bool> g_toastEnabled{true};
static std::atomic<bool> g_toastShowOnAuto{true};
static std::atomic<uint32_t> g_toastDurationMs{3200};
static std::atomic<bool> g_toastExcludeFromCapture{true};

static constexpr UINT WM_APP_CHECK_TEMP_TAP = WM_APP + 1;
static constexpr UINT WM_APP_SHOW_TOAST = WM_APP + 2;
static std::atomic<bool> g_stopRequested{false};
static std::atomic<bool> g_manualTempRequested{false};
static std::atomic<bool> g_manualRecycleRequested{false};
static std::atomic<bool> g_settingsReconfigured{false};
static uint64_t g_totalFreedBytes = 0;

static void ShowCleanToast(const std::wstring& title, const std::wstring& subtitle, bool isManual);

// ============================================================================
// Helpers: Overflow-Safe Units, Paths, Matching, Time
// ============================================================================

static uint64_t ScaleByPercentU64(uint64_t value, int pct) {
    if (pct <= 0) return 0;
    if (pct >= 100) return value;
    uint64_t upct = static_cast<uint64_t>(pct);
    return (value / 100ULL) * upct + ((value % 100ULL) * upct) / 100ULL;
}

static std::wstring FormatBytesHuman(uint64_t bytes) {
    WCHAR buf[64]{};
    if (bytes < 1024ULL) {
        swprintf_s(buf, L"%llu B", static_cast<unsigned long long>(bytes));
    } else if (bytes < 1024ULL * 1024ULL) {
        swprintf_s(buf, L"%.1f KB", static_cast<double>(bytes) / 1024.0);
    } else if (bytes < 1024ULL * 1024ULL * 1024ULL) {
        swprintf_s(buf, L"%.1f MB", static_cast<double>(bytes) / (1024.0 * 1024.0));
    } else {
        swprintf_s(buf, L"%.2f GB", static_cast<double>(bytes) / (1024.0 * 1024.0 * 1024.0));
    }
    return buf;
}

static uint64_t MultiplyClampU64(uint64_t a, uint64_t b) {
    if (a == 0 || b == 0) {
        return 0;
    }
    if (a > (std::numeric_limits<uint64_t>::max)() / b) {
        return (std::numeric_limits<uint64_t>::max)();
    }
    return a * b;
}

static uint64_t UnitToMultiplier(PCWSTR unit) {
    if (!unit) return 1024ULL * 1024ULL;
    if (wcscmp(unit, L"KB") == 0) return 1024ULL;
    if (wcscmp(unit, L"MB") == 0) return 1024ULL * 1024ULL;
    if (wcscmp(unit, L"GB") == 0) return 1024ULL * 1024ULL * 1024ULL;
    if (wcscmp(unit, L"TB") == 0) return 1024ULL * 1024ULL * 1024ULL * 1024ULL;
    return 1024ULL * 1024ULL;
}

static int ParseMouseGestureClicks(PCWSTR val) {
    if (!val) return 2;
    if (wcscmp(val, L"Disabled") == 0) return 0;
    if (wcscmp(val, L"SingleMiddleClick") == 0) return 1;
    if (wcscmp(val, L"DoubleMiddleClick") == 0) return 2;
    if (wcscmp(val, L"TripleMiddleClick") == 0) return 3;
    return 2;
}

static int ParseMouseScope(PCWSTR val) {
    if (!val) return 0;
    if (wcscmp(val, L"Anywhere") == 0) return 1;
    return 0;
}

static int ParseTempMouseScope(PCWSTR val) {
    if (!val) return 0;
    if (wcscmp(val, L"TaskbarOnly") == 0) return 1;
    if (wcscmp(val, L"DesktopOnly") == 0) return 2;
    return 0; // DesktopAndTaskbar
}

static uint64_t TimeUnitToMs(PCWSTR unit) {
    if (!unit) return 3600000ULL;
    if (wcscmp(unit, L"Seconds") == 0) return 1000ULL;
    if (wcscmp(unit, L"Minutes") == 0) return 60ULL * 1000ULL;
    if (wcscmp(unit, L"Hours") == 0) return 3600ULL * 1000ULL;
    if (wcscmp(unit, L"Days") == 0) return 86400ULL * 1000ULL;
    return 3600000ULL;
}

static std::wstring StripTrailingSlashes(std::wstring path) {
    while (path.size() > 3 && (path.back() == L'\\' || path.back() == L'/')) {
        path.pop_back();
    }
    return path;
}

static std::wstring ToExtendedPath(const std::wstring& path) {
    if (path.rfind(L"\\\\?\\", 0) == 0) {
        return path;
    }
    if (path.rfind(L"\\\\", 0) == 0) {
        return L"\\\\?\\UNC\\" + path.substr(2);
    }
    return L"\\\\?\\" + path;
}

static std::wstring ResolveUserTempDir() {
    using GetTempPath2W_t = DWORD(WINAPI*)(DWORD BufferLength, LPWSTR Buffer);
    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    auto pGetTempPath2W = kernelBase
        ? reinterpret_cast<GetTempPath2W_t>(GetProcAddress(kernelBase, "GetTempPath2W"))
        : nullptr;

    WCHAR rawPath[MAX_PATH + 4]{};
    DWORD len = pGetTempPath2W
        ? pGetTempPath2W(ARRAYSIZE(rawPath), rawPath)
        : GetTempPathW(ARRAYSIZE(rawPath), rawPath);
    if (len == 0 || len >= ARRAYSIZE(rawPath)) {
        return L"";
    }

    WCHAR fullPath[MAX_PATH + 4]{};
    DWORD fullLen = GetFullPathNameW(rawPath, ARRAYSIZE(fullPath), fullPath, nullptr);
    if (fullLen == 0 || fullLen >= ARRAYSIZE(fullPath)) {
        return StripTrailingSlashes(rawPath);
    }
    return StripTrailingSlashes(fullPath);
}

static std::wstring ResolveWindowsTempDir() {
    WCHAR winDir[MAX_PATH]{};
    UINT len = GetWindowsDirectoryW(winDir, ARRAYSIZE(winDir));
    if (len == 0 || len >= ARRAYSIZE(winDir)) {
        return L"";
    }
    std::wstring path = StripTrailingSlashes(winDir);
    path += L"\\Temp";
    return path;
}

static std::vector<std::wstring> GetTargetTempDirs(const ModSettings& cfg) {
    std::vector<std::wstring> dirs;
    std::wstring userTemp = ResolveUserTempDir();
    if (!userTemp.empty()) {
        dirs.push_back(userTemp);
    }
    if (cfg.cleanWindowsTemp) {
        std::wstring winTemp = ResolveWindowsTempDir();
        if (!winTemp.empty() && _wcsicmp(winTemp.c_str(), userTemp.c_str()) != 0) {
            dirs.push_back(winTemp);
        }
    }
    return dirs;
}

static std::wstring NormalizeExcludePattern(PCWSTR raw) {
    if (!raw) return L"";
    std::wstring s(raw);
    while (!s.empty() && (s.front() == L' ' || s.front() == L'\t' || s.front() == L'\\' || s.front() == L'/')) {
        s.erase(s.begin());
    }
    while (!s.empty() && (s.back() == L' ' || s.back() == L'\t' || s.back() == L'\\' || s.back() == L'/')) {
        s.pop_back();
    }
    return s;
}

static std::vector<std::wstring> DefaultAiAgentPatterns() {
    return {
        L"claude*",
        L"tmpclaude-*",
        L"wvm-*",
        L"codex*",
        L"openai-codex*",
        L"hermes*",
        L".hermes*",
        L"openclaw*",
        L".openclaw*",
        L"gemini-*",
        L"opencode*",
        L"cursor-*",
        L"windsurf-*",
        L"goose-*",
        L"aider*",
        L"copilot-*",
        L"mcp-*",
    };
}

static bool IsExcludedName(const wchar_t* name, const std::vector<std::wstring>& patterns) {
    for (const auto& pat : patterns) {
        if (!pat.empty() && PathMatchSpecW(name, pat.c_str())) {
            return true;
        }
    }
    return false;
}

static bool IsProtectedAiAgentName(const wchar_t* name, const ModSettings& cfg) {
    if (!cfg.aiAgentProtectionEnabled) {
        return false;
    }
    return IsExcludedName(name, cfg.aiAgentPatterns);
}

static uint64_t FileTimeToU64(const FILETIME& ft) {
    ULARGE_INTEGER uli;
    uli.LowPart = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;
    return uli.QuadPart;
}

static bool IsOlderThanGracePeriod(const WIN32_FIND_DATAW& fd, uint64_t now100Ns, uint64_t graceMs) {
    if (graceMs == 0) {
        return true;
    }
    uint64_t grace100Ns = MultiplyClampU64(graceMs, 10000ULL);
    uint64_t lastWrite = FileTimeToU64(fd.ftLastWriteTime);
    uint64_t created = FileTimeToU64(fd.ftCreationTime);
    uint64_t newest = (std::max)(lastWrite, created);
    return (now100Ns > newest) && ((now100Ns - newest) >= grace100Ns);
}

// ============================================================================
// Smart Deferral, Capture Guard & Low-Disk Emergency Checks
// ============================================================================

static bool IsCaptureOverlayActive() {
    static const wchar_t* const kCaptureClasses[] = {
        L"ScreenClippingHostWindowClass",
        L"Microsoft.Windows.Shell.ScreenClipping",
        L"XboxGameBarFlyout",
    };
    for (const wchar_t* cls : kCaptureClasses) {
        HWND h = FindWindowW(cls, nullptr);
        if (h && IsWindowVisible(h)) {
            return true;
        }
    }
    return false;
}

static std::atomic<bool> g_sessionLocked{false};

// Verifies the user has actually passed the Windows logon/password screen and the interactive
// desktop ("Default") plus Explorer shell/taskbar are live on screen.
static bool IsInteractiveUserDesktopReady() {
    if (g_sessionLocked.load(std::memory_order_relaxed)) {
        return false;
    }

    HDESK hDesk = OpenInputDesktop(0, FALSE, DESKTOP_READOBJECTS);
    if (!hDesk) {
        return false;
    }

    WCHAR deskName[64]{};
    DWORD needed = 0;
    BOOL nameOk = GetUserObjectInformationW(hDesk, UOI_NAME, deskName, sizeof(deskName), &needed);
    CloseDesktop(hDesk);
    if (!nameOk || _wcsicmp(deskName, L"Default") != 0) {
        return false;
    }

    HWND hShell = GetShellWindow();
    HWND hTray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!hShell || !hTray || !IsWindowVisible(hTray)) {
        return false;
    }

    return true;
}

static bool IsBorderlessFullscreenActive() {
    HWND fg = GetForegroundWindow();
    if (!fg || !IsWindowVisible(fg) || IsIconic(fg) || fg == GetShellWindow()) {
        return false;
    }

    WCHAR cls[64]{};
    if (GetClassNameW(fg, cls, ARRAYSIZE(cls)) > 0) {
        if (wcscmp(cls, L"Progman") == 0 ||
            wcscmp(cls, L"WorkerW") == 0 ||
            wcscmp(cls, L"Shell_TrayWnd") == 0 ||
            wcscmp(cls, L"Shell_SecondaryTrayWnd") == 0 ||
            wcscmp(cls, L"ApplicationFrameWindow") == 0 ||
            wcscmp(cls, L"Windows.UI.Core.CoreWindow") == 0) {
            return false;
        }
    }

    // Ordinary maximized/titled desktop windows are not fullscreen games
    LONG_PTR style = GetWindowLongPtrW(fg, GWL_STYLE);
    if ((style & WS_CAPTION) && (style & (WS_THICKFRAME | WS_MAXIMIZE))) {
        return false;
    }

    RECT wr{};
    if (!GetWindowRect(fg, &wr)) {
        return false;
    }

    HMONITOR mon = MonitorFromWindow(fg, MONITOR_DEFAULTTONEAREST);
    if (!mon) {
        return false;
    }
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(mon, &mi)) {
        return false;
    }

    return wr.left <= mi.rcMonitor.left &&
           wr.top <= mi.rcMonitor.top &&
           wr.right >= mi.rcMonitor.right &&
           wr.bottom >= mi.rcMonitor.bottom;
}

static bool IsFullscreenBusy() {
    QUERY_USER_NOTIFICATION_STATE state = QUNS_ACCEPTS_NOTIFICATIONS;
    if (SUCCEEDED(SHQueryUserNotificationState(&state))) {
        // Only defer on true D3D exclusive fullscreen or presentation mode (not QUNS_BUSY Focus Assist)
        if (state == QUNS_RUNNING_D3D_FULL_SCREEN ||
            state == QUNS_PRESENTATION_MODE) {
            return true;
        }
    }
    return IsBorderlessFullscreenActive();
}

static bool IsOnBatteryPower() {
    SYSTEM_POWER_STATUS sps{};
    if (!GetSystemPowerStatus(&sps)) {
        return false;
    }
    return sps.ACLineStatus == 0;
}

static bool ShouldDeferCleanup(const ModSettings& cfg, bool isEmergency, const wchar_t** outReason) {
    if (isEmergency && cfg.emergencyBypassDeferral) {
        return false;
    }
    if (cfg.deferOnFullscreen && IsFullscreenBusy()) {
        if (outReason) *outReason = L"Fullscreen";
        return true;
    }
    if (cfg.deferOnBattery && IsOnBatteryPower()) {
        if (outReason) *outReason = L"Battery";
        return true;
    }
    return false;
}

static bool IsDiskSpaceBelowFloor(const ModSettings& cfg, uint64_t* outFreeBytes) {
    if (!cfg.emergencyEnabled) {
        return false;
    }
    std::wstring userTemp = ResolveUserTempDir();
    if (userTemp.empty()) {
        return false;
    }

    ULARGE_INTEGER freeBytesAvail{};
    if (!GetDiskFreeSpaceExW(userTemp.c_str(), &freeBytesAvail, nullptr, nullptr)) {
        return false;
    }
    if (outFreeBytes) {
        *outFreeBytes = freeBytesAvail.QuadPart;
    }
    return freeBytesAvail.QuadPart < cfg.emergencyFloorBytes;
}

// ============================================================================
// UI Thread (UIA Empty-Taskbar Check + Toast Host) & Mouse Hook Thread
// ============================================================================

static bool IsShellDesktopOrTaskbarRootClass(PCWSTR cls) {
    if (!cls || !*cls) return false;
    return wcscmp(cls, L"Progman") == 0 ||
           wcscmp(cls, L"WorkerW") == 0 ||
           wcscmp(cls, L"Shell_TrayWnd") == 0 ||
           wcscmp(cls, L"Shell_SecondaryTrayWnd") == 0 ||
           wcscmp(cls, L"XamlExplorerHostIslandWindow") == 0;
}

static bool IsPointOnTaskbarRect(POINT pt) {
    HWND hTray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (hTray && IsWindowVisible(hTray)) {
        RECT rc{};
        if (GetWindowRect(hTray, &rc) && PtInRect(&rc, pt)) {
            return true;
        }
    }
    HWND hSec = nullptr;
    while ((hSec = FindWindowExW(nullptr, hSec, L"Shell_SecondaryTrayWnd", nullptr)) != nullptr) {
        if (IsWindowVisible(hSec)) {
            RECT rc{};
            if (GetWindowRect(hSec, &rc) && PtInRect(&rc, pt)) {
                return true;
            }
        }
    }
    return false;
}

static bool IsPointAllowedForMiddleClick(POINT pt) {
    if (g_toastExcludeFromCapture.load(std::memory_order_relaxed) && IsCaptureOverlayActive()) {
        return false;
    }
    if (g_mouseTriggerScope.load(std::memory_order_relaxed) == 1) {
        return !IsFullscreenBusy();
    }
    if (IsPointOnTaskbarRect(pt)) {
        return true;
    }
    HWND hWnd = WindowFromPoint(pt);
    if (!hWnd) return false;
    HWND hRoot = GetAncestor(hWnd, GA_ROOT);
    if (!hRoot) hRoot = hWnd;
    WCHAR rootCls[96]{};
    GetClassNameW(hRoot, rootCls, ARRAYSIZE(rootCls));
    return IsShellDesktopOrTaskbarRootClass(rootCls);
}

static bool IsPointOnDesktopWindow(POINT pt) {
    HWND hWnd = WindowFromPoint(pt);
    if (!hWnd) return false;
    WCHAR cls[96]{};
    GetClassNameW(hWnd, cls, ARRAYSIZE(cls));
    if (wcscmp(cls, L"SysListView32") == 0 || wcscmp(cls, L"SHELLDLL_DefView") == 0) {
        return true;
    }
    HWND hRoot = GetAncestor(hWnd, GA_ROOT);
    if (!hRoot) hRoot = hWnd;
    WCHAR rootCls[96]{};
    GetClassNameW(hRoot, rootCls, ARRAYSIZE(rootCls));
    return wcscmp(rootCls, L"Progman") == 0 || wcscmp(rootCls, L"WorkerW") == 0;
}

static bool IsPointCandidateForTempTap(POINT pt) {
    int scope = g_tempGestureScope.load(std::memory_order_relaxed);
    bool onTaskbar = IsPointOnTaskbarRect(pt);
    if (scope == 1) { // TaskbarOnly
        return onTaskbar;
    }
    bool onDesktop = !onTaskbar && IsPointOnDesktopWindow(pt);
    if (scope == 2) { // DesktopOnly
        return onDesktop;
    }
    return onTaskbar || onDesktop; // DesktopAndTaskbar
}

static bool IsPointEmptyDesktopOrTaskbarUIA(IUIAutomation* pUia, POINT pt) {
    int scope = g_tempGestureScope.load(std::memory_order_relaxed);
    bool onTaskbar = IsPointOnTaskbarRect(pt);
    bool onDesktop = !onTaskbar && IsPointOnDesktopWindow(pt);

    if (scope == 1 && !onTaskbar) return false;
    if (scope == 2 && !onDesktop) return false;
    if (!onTaskbar && !onDesktop) return false;

    if (!pUia) {
        return true;
    }

    IUIAutomationElement* pElem = nullptr;
    HRESULT hr = pUia->ElementFromPoint(pt, &pElem);
    if (FAILED(hr) || !pElem) {
        // On desktop, fail closed so rapid clicks near icons/menus never misfire.
        return onTaskbar;
    }

    CONTROLTYPEID ctrlType = 0;
    pElem->get_CurrentControlType(&ctrlType);

    BSTR bstrCls = nullptr;
    BSTR bstrId = nullptr;
    pElem->get_CurrentClassName(&bstrCls);
    pElem->get_CurrentAutomationId(&bstrId);

    std::wstring cls = bstrCls ? bstrCls : L"";
    std::wstring aid = bstrId ? bstrId : L"";
    if (bstrCls) SysFreeString(bstrCls);
    if (bstrId) SysFreeString(bstrId);
    pElem->Release();

    if (onDesktop) {
        // On Desktop, reject clicks on desktop icons (UIA_ListItemControlTypeId) or context menus/rename boxes
        if (ctrlType == UIA_ListItemControlTypeId ||
            ctrlType == UIA_ButtonControlTypeId ||
            ctrlType == UIA_MenuItemControlTypeId ||
            ctrlType == UIA_EditControlTypeId ||
            ctrlType == UIA_TreeItemControlTypeId) {
            return false;
        }
        // Empty desktop wallpaper is UIA_ListControlTypeId ("SysListView32") or UIA_PaneControlTypeId
        return true;
    }

    // On Taskbar: reject interactive app buttons, Start, Search, Task View, Widgets, Tray icons
    if (ctrlType == UIA_ButtonControlTypeId ||
        ctrlType == UIA_ListItemControlTypeId ||
        ctrlType == UIA_MenuItemControlTypeId ||
        ctrlType == UIA_EditControlTypeId ||
        ctrlType == UIA_CheckBoxControlTypeId ||
        ctrlType == UIA_SplitButtonControlTypeId ||
        ctrlType == UIA_TabItemControlTypeId) {
        return false;
    }

    if (cls.find(L"TaskbarFrameRepeater") != std::wstring::npos ||
        cls.find(L"Taskbar.TaskbarFrame") != std::wstring::npos ||
        cls.find(L"MSTaskSwWClass") != std::wstring::npos ||
        cls.find(L"MSTaskListWClass") != std::wstring::npos ||
        cls.find(L"Shell_TrayWnd") != std::wstring::npos ||
        cls.find(L"Shell_SecondaryTrayWnd") != std::wstring::npos ||
        cls.find(L"ReBarWindow32") != std::wstring::npos ||
        aid == L"TaskbarFrame") {
        return true;
    }

    return ctrlType == UIA_PaneControlTypeId ||
           ctrlType == UIA_WindowControlTypeId ||
           ctrlType == UIA_GroupControlTypeId ||
           ctrlType == UIA_ToolBarControlTypeId ||
           ctrlType == UIA_CustomControlTypeId;
}

struct ClickGestureState {
    uint64_t lastDownTick = 0;
    POINT firstDownPt{};
    int completedClicks = 0;
    bool downInProgress = false;
    bool swallowNextUp = false;
};

static ClickGestureState g_mclickState;
static ClickGestureState g_leftTapState;
static ClickGestureState g_rightTapState;
static HHOOK g_mouseHook = nullptr;

static void ResetClickGesture(ClickGestureState& s) {
    s.lastDownTick = 0;
    s.firstDownPt = {};
    s.completedClicks = 0;
    s.downInProgress = false;
}

static bool IsWithinMultiClickTolerance(POINT a, POINT b) {
    int sysX = GetSystemMetrics(SM_CXDOUBLECLK);
    int sysY = GetSystemMetrics(SM_CYDOUBLECLK);
    int tolX = (std::max)(sysX, 16);
    int tolY = (std::max)(sysY, 16);
    return std::abs(a.x - b.x) <= tolX && std::abs(a.y - b.y) <= tolY;
}

static LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode < 0 || !lParam) {
        return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
    }

    const auto* ms = reinterpret_cast<const MSLLHOOKSTRUCT*>(lParam);
    if ((ms->flags & LLMHF_INJECTED) || (ms->flags & LLMHF_LOWER_IL_INJECTED)) {
        return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
    }

    // Gesture Guard: X-buttons and wheel scrolls reset all click gestures immediately.
    if (wParam == WM_XBUTTONDOWN || wParam == WM_XBUTTONUP ||
        wParam == WM_MOUSEWHEEL || wParam == WM_MOUSEHWHEEL) {
        ResetClickGesture(g_mclickState);
        ResetClickGesture(g_leftTapState);
        ResetClickGesture(g_rightTapState);
        return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
    }

    if (wParam == WM_MOUSEMOVE) {
        if ((g_mclickState.downInProgress || g_mclickState.completedClicks > 0) &&
            !IsWithinMultiClickTolerance(ms->pt, g_mclickState.firstDownPt)) {
            ResetClickGesture(g_mclickState);
        }
        if ((g_leftTapState.downInProgress || g_leftTapState.completedClicks > 0) &&
            !IsWithinMultiClickTolerance(ms->pt, g_leftTapState.firstDownPt)) {
            ResetClickGesture(g_leftTapState);
        }
        if ((g_rightTapState.downInProgress || g_rightTapState.completedClicks > 0) &&
            !IsWithinMultiClickTolerance(ms->pt, g_rightTapState.firstDownPt)) {
            ResetClickGesture(g_rightTapState);
        }
        return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
    }

    uint64_t now = GetTickCount64();

    // 1. 3x Left-Click or 3x Right-Click on Empty Taskbar Space -> Temp Cleanup
    if (wParam == WM_LBUTTONDOWN || wParam == WM_LBUTTONUP ||
        wParam == WM_RBUTTONDOWN || wParam == WM_RBUTTONUP) {
        ResetClickGesture(g_mclickState);
        bool isLeft = (wParam == WM_LBUTTONDOWN || wParam == WM_LBUTTONUP);
        bool isDown = (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN);
        ClickGestureState& tapState = isLeft ? g_leftTapState : g_rightTapState;
        ClickGestureState& otherState = isLeft ? g_rightTapState : g_leftTapState;
        ResetClickGesture(otherState);

        if (!g_tempGestureEnabled.load(std::memory_order_relaxed)) {
            ResetClickGesture(tapState);
            return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
        }

        uint64_t maxInterval = g_tempGestureMaxIntervalMs.load(std::memory_order_relaxed);
        if (isDown) {
            if (tapState.completedClicks > 0) {
                uint64_t elapsed = now - tapState.lastDownTick;
                if (elapsed > maxInterval || !IsWithinMultiClickTolerance(ms->pt, tapState.firstDownPt)) {
                    ResetClickGesture(tapState);
                }
            }
            if ((g_toastExcludeFromCapture.load(std::memory_order_relaxed) && IsCaptureOverlayActive()) ||
                !IsPointCandidateForTempTap(ms->pt)) {
                ResetClickGesture(tapState);
                return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
            }
            if (tapState.completedClicks == 0) {
                tapState.firstDownPt = ms->pt;
            }
            tapState.lastDownTick = now;
            tapState.downInProgress = true;

            if (tapState.completedClicks + 1 >= 3) {
                ResetClickGesture(tapState);
                if (g_uiMsgHwnd) {
                    PostMessageW(
                        g_uiMsgHwnd,
                        WM_APP_CHECK_TEMP_TAP,
                        static_cast<WPARAM>(ms->pt.x),
                        static_cast<LPARAM>(ms->pt.y));
                }
            }
        } else {
            if (tapState.downInProgress) {
                uint64_t elapsed = now - tapState.lastDownTick;
                if (elapsed <= maxInterval &&
                    IsWithinMultiClickTolerance(ms->pt, tapState.firstDownPt)) {
                    tapState.downInProgress = false;
                    tapState.completedClicks++;
                } else {
                    ResetClickGesture(tapState);
                }
            } else {
                ResetClickGesture(tapState);
            }
        }
        return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
    }

    // 2. Middle-Click Shortcut -> Recycle Bin Cleanup
    int requiredClicks = g_mouseClicksRequired.load(std::memory_order_relaxed);
    if (requiredClicks <= 0) {
        ResetClickGesture(g_mclickState);
        g_mclickState.swallowNextUp = false;
        return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
    }

    uint64_t maxInterval = g_mouseMaxIntervalMs.load(std::memory_order_relaxed);
    if (wParam == WM_MBUTTONDOWN) {
        ResetClickGesture(g_leftTapState);
        ResetClickGesture(g_rightTapState);
        if (g_mclickState.completedClicks > 0) {
            uint64_t elapsed = now - g_mclickState.lastDownTick;
            if (elapsed > maxInterval || !IsWithinMultiClickTolerance(ms->pt, g_mclickState.firstDownPt)) {
                ResetClickGesture(g_mclickState);
            }
        }
        if (!IsPointAllowedForMiddleClick(ms->pt)) {
            ResetClickGesture(g_mclickState);
            return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
        }
        if (g_mclickState.completedClicks == 0) {
            g_mclickState.firstDownPt = ms->pt;
        }
        g_mclickState.lastDownTick = now;
        g_mclickState.downInProgress = true;

        if (g_mclickState.completedClicks + 1 >= requiredClicks) {
            ResetClickGesture(g_mclickState);
            g_manualRecycleRequested.store(true, std::memory_order_release);
            if (g_wakeEvent) {
                SetEvent(g_wakeEvent);
            }
            if (g_mouseSwallowClick.load(std::memory_order_relaxed)) {
                g_mclickState.swallowNextUp = true;
                return 1;
            }
        }
        return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
    }

    if (wParam == WM_MBUTTONUP) {
        if (g_mclickState.swallowNextUp) {
            g_mclickState.swallowNextUp = false;
            return 1;
        }
        if (g_mclickState.downInProgress) {
            uint64_t elapsed = now - g_mclickState.lastDownTick;
            if (elapsed <= maxInterval &&
                IsWithinMultiClickTolerance(ms->pt, g_mclickState.firstDownPt)) {
                g_mclickState.downInProgress = false;
                g_mclickState.completedClicks++;
            } else {
                ResetClickGesture(g_mclickState);
            }
        } else {
            ResetClickGesture(g_mclickState);
        }
        return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
    }

    return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
}

static DWORD WINAPI MouseHookThreadProc(LPVOID) {
    MSG msg{};
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, GetModuleHandleW(nullptr), 0);
    if (!g_mouseHook) {
        Wh_Log(L"AutoClean: SetWindowsHookExW(WH_MOUSE_LL) failed (err=%lu)", GetLastError());
        return 0;
    }

    while (!g_stopRequested.load(std::memory_order_relaxed)) {
        BOOL ret = GetMessageW(&msg, nullptr, 0, 0);
        if (ret <= 0 || msg.message == WM_QUIT) {
            break;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_mouseHook) {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = nullptr;
    }
    return 0;
}

static void SyncMouseHookThread(bool needHook) {
    if (needHook && !g_mouseThread && !g_stopRequested.load(std::memory_order_relaxed)) {
        ResetClickGesture(g_mclickState);
        ResetClickGesture(g_leftTapState);
        ResetClickGesture(g_rightTapState);
        g_mouseThread = CreateThread(nullptr, 0, MouseHookThreadProc, nullptr, 0, &g_mouseThreadId);
    } else if (!needHook && g_mouseThread) {
        if (g_mouseThreadId != 0) {
            PostThreadMessageW(g_mouseThreadId, WM_QUIT, 0, 0);
        }
        WaitForSingleObject(g_mouseThread, 2000);
        CloseHandle(g_mouseThread);
        g_mouseThread = nullptr;
        g_mouseThreadId = 0;
        ResetClickGesture(g_mclickState);
        ResetClickGesture(g_leftTapState);
        ResetClickGesture(g_rightTapState);
    }
}

static IUIAutomation* g_pUia = nullptr;

static void HandleToastShowMessage(LPARAM lParam);

#ifndef WDA_EXCLUDEFROMCAPTURE
#define WDA_EXCLUDEFROMCAPTURE 0x00000011
#endif

struct ToastPayload {
    std::wstring title;
    std::wstring subtitle;
    uint32_t durationMs = 3200;
    bool isManual = false;
};

struct ToastWindowState {
    HWND hwnd = nullptr;
    bool visible = false;
    bool hovered = false;
    bool fadingOut = false;
    uint64_t qpcFreq = 1;
    uint64_t animStartQpc = 0;
    uint64_t showStartTick = 0;
    uint32_t durationMs = 3200;
    BYTE currentAlpha = 0;
    int widthPx = 0;
    int heightPx = 0;
    HBITMAP hDib = nullptr;
    void* dibBits = nullptr;
    std::deque<ToastPayload> pendingQueue;
};

static ToastWindowState g_toast;
static constexpr UINT_PTR kToastAnimTimerId = 1;
static constexpr UINT_PTR kToastHideTimerId = 2;
static constexpr wchar_t kToastWndClass[] = L"AutoCleanToastWndClass";

static void EnablePerMonitorDpiV2ForCurrentThread() {
    using SetThreadDpiAwarenessContext_t = DPI_AWARENESS_CONTEXT(WINAPI*)(DPI_AWARENESS_CONTEXT);
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    auto pSetThreadDpi = user32
        ? reinterpret_cast<SetThreadDpiAwarenessContext_t>(
              GetProcAddress(user32, "SetThreadDpiAwarenessContext"))
        : nullptr;
    if (pSetThreadDpi) {
        pSetThreadDpi(reinterpret_cast<DPI_AWARENESS_CONTEXT>(-4)); // DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
    }
}

static UINT GetMonitorDpiForPoint(POINT pt, HWND fallbackHwnd) {
    HMONITOR hMon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    HMODULE shcore = GetModuleHandleW(L"shcore.dll");
    if (!shcore) {
        shcore = LoadLibraryW(L"shcore.dll");
    }
    if (shcore && hMon) {
        using GetDpiForMonitor_t = HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*);
        auto pGetDpiForMonitor = reinterpret_cast<GetDpiForMonitor_t>(
            GetProcAddress(shcore, "GetDpiForMonitor"));
        if (pGetDpiForMonitor) {
            UINT dpiX = 96, dpiY = 96;
            if (SUCCEEDED(pGetDpiForMonitor(hMon, 0 /*MDT_EFFECTIVE_DPI*/, &dpiX, &dpiY)) && dpiX >= 96) {
                return dpiX;
            }
        }
    }
    using GetDpiForWindow_t = UINT(WINAPI*)(HWND);
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    auto pGetDpiForWindow = user32
        ? reinterpret_cast<GetDpiForWindow_t>(GetProcAddress(user32, "GetDpiForWindow"))
        : nullptr;
    if (pGetDpiForWindow && fallbackHwnd) {
        UINT dpi = pGetDpiForWindow(fallbackHwnd);
        if (dpi >= 96) return dpi;
    }
    HDC hdc = GetDC(nullptr);
    UINT dpi = hdc ? static_cast<UINT>(GetDeviceCaps(hdc, LOGPIXELSX)) : 96;
    if (hdc) ReleaseDC(nullptr, hdc);
    return dpi >= 96 ? dpi : 96;
}

static void DestroyToastDib() {
    if (g_toast.hDib) {
        DeleteObject(g_toast.hDib);
        g_toast.hDib = nullptr;
        g_toast.dibBits = nullptr;
    }
}

static double SmoothStep01(double t) {
    if (t <= 0.0) return 0.0;
    if (t >= 1.0) return 1.0;
    return t * t * (3.0 - 2.0 * t);
}

static void PresentToastAlpha(BYTE alpha) {
    if (!g_toast.hwnd || !g_toast.hDib) return;
    HDC hdcScreen = GetDC(nullptr);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    HGDIOBJ oldBmp = SelectObject(hdcMem, g_toast.hDib);

    SIZE sz{g_toast.widthPx, g_toast.heightPx};
    POINT ptSrc{0, 0};
    BLENDFUNCTION bf{};
    bf.BlendOp = AC_SRC_OVER;
    bf.SourceConstantAlpha = alpha;
    bf.AlphaFormat = AC_SRC_ALPHA;

    UpdateLayeredWindow(g_toast.hwnd, hdcScreen, nullptr, &sz, hdcMem, &ptSrc, 0, &bf, ULW_ALPHA);

    SelectObject(hdcMem, oldBmp);
    DeleteDC(hdcMem);
    ReleaseDC(nullptr, hdcScreen);
    g_toast.currentAlpha = alpha;
}

static void BuildRoundedRectPath(Gdiplus::GraphicsPath& path, float x, float y, float w, float h, float radius) {
    float r = (std::min)(radius, (std::min)(w, h) * 0.5f);
    float d = r * 2.0f;
    if (d <= 0.5f) {
        path.AddRectangle(Gdiplus::RectF(x, y, w, h));
        return;
    }
    path.AddArc(x, y, d, d, 180.0f, 90.0f);
    path.AddArc(x + w - d, y, d, d, 270.0f, 90.0f);
    path.AddArc(x + w - d, y + h - d, d, d, 0.0f, 90.0f);
    path.AddArc(x, y + h - d, d, d, 90.0f, 90.0f);
    path.CloseFigure();
}

static void RenderToastContent(const std::wstring& title, const std::wstring& subtitle) {
    if (!g_toast.hwnd) return;
    POINT cursorPt{};
    GetCursorPos(&cursorPt);
    UINT dpi = GetMonitorDpiForPoint(cursorPt, g_toast.hwnd);
    auto scale = [dpi](int logical) -> int {
        return MulDiv(logical, static_cast<int>(dpi), 96);
    };

    // Determine semantic state for fill-only left accent bar
    bool isDryRun = (title.find(L"DRY RUN") != std::wstring::npos || title.find(L"Dry-Run") != std::wstring::npos);
    bool isError = (title.find(L"Error") != std::wstring::npos);
    bool isEmpty = (title.find(L"Empty") != std::wstring::npos || title.find(L"Already Clean") != std::wstring::npos);

    // Strictly warm Terracotta & Tan palette (zero blue/cool tones)
    Gdiplus::Color accentFill = isDryRun
        ? Gdiplus::Color(255, 224, 138, 68)   // Warm amber-terracotta (#E08A44) for Dry-Run preview
        : (isError
            ? Gdiplus::Color(255, 200, 90, 50)   // Deep rust terracotta (#C85A32) for Error
            : (isEmpty
                ? Gdiplus::Color(255, 212, 163, 115) // Warm desert tan (#D4A373) for Already Clean / Empty
                : Gdiplus::Color(255, 217, 119, 87))); // Signature terracotta orange (#D97757) for Cleaned/Freed

    int accentBarW = (std::max)(3, scale(3));
    int leftInset = scale(12) + accentBarW + scale(10);
    int rightPad = scale(16);
    int padY = scale(12);
    int gapY = scale(4);
    int minW = scale(240);
    int maxW = scale(460);
    float radius = static_cast<float>(scale(12));

    // Exact physical-pixel font sizes (UnitPixel) prevent GDI+ 96-DPI bitmap point under-scaling blur
    float titlePx = static_cast<float>(MulDiv(13, static_cast<int>(dpi), 96));
    float subPx = static_cast<float>(MulDiv(11, static_cast<int>(dpi), 96));

    Gdiplus::FontFamily ff(L"Segoe UI");
    Gdiplus::Font titleFont(&ff, titlePx, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
    Gdiplus::Font subFont(&ff, subPx, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    const Gdiplus::StringFormat* fmt = Gdiplus::StringFormat::GenericTypographic();

    HDC hdcScreen = GetDC(nullptr);
    Gdiplus::Graphics measureG(hdcScreen);
    measureG.SetPageUnit(Gdiplus::UnitPixel);
    measureG.SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);

    Gdiplus::RectF layoutMax(0.0f, 0.0f, static_cast<float>(maxW - leftInset - rightPad), 240.0f);
    Gdiplus::RectF titleBox{}, subBox{};
    measureG.MeasureString(title.c_str(), -1, &titleFont, layoutMax, fmt, &titleBox);
    measureG.MeasureString(subtitle.c_str(), -1, &subFont, layoutMax, fmt, &subBox);
    ReleaseDC(nullptr, hdcScreen);

    int titleH = static_cast<int>(std::ceil(titleBox.Height));
    int subH = static_cast<int>(std::ceil(subBox.Height));
    int contentW = (std::max)(static_cast<int>(std::ceil(titleBox.Width)), static_cast<int>(std::ceil(subBox.Width)));
    int w = (std::min)(maxW, (std::max)(minW, leftInset + contentW + rightPad));
    int h = padY * 2 + titleH + gapY + subH;

    DestroyToastDib();
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    HDC hdcMem = CreateCompatibleDC(nullptr);
    g_toast.hDib = CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS, &g_toast.dibBits, nullptr, 0);
    if (!g_toast.hDib || !g_toast.dibBits) {
        DeleteDC(hdcMem);
        return;
    }
    ZeroMemory(g_toast.dibBits, static_cast<size_t>(w) * static_cast<size_t>(h) * 4);
    HGDIOBJ oldBmp = SelectObject(hdcMem, g_toast.hDib);

    {
        Gdiplus::Graphics g(hdcMem);
        g.SetPageUnit(Gdiplus::UnitPixel);
        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
        // ClearTypeGridFit against opaque fills (alpha=255) yields razor-sharp subpixel glyphs
        g.SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);
        g.SetTextContrast(6);

        float fw = static_cast<float>(w);
        float fh = static_cast<float>(h);

        // 1. Outer fill-only base container (warm dark espresso-terracotta tint, zero border/stroke/outline)
        Gdiplus::GraphicsPath outerPath;
        BuildRoundedRectPath(outerPath, 0.0f, 0.0f, fw, fh, radius);
        Gdiplus::SolidBrush outerSurfaceBrush(Gdiplus::Color(255, 24, 17, 13));
        g.FillPath(&outerSurfaceBrush, &outerPath);

        // 2. Inner OLED Black (#000000) card fill inset by 1px (creates subtle modern depth purely via fills)
        float inset = 1.0f;
        Gdiplus::GraphicsPath innerPath;
        BuildRoundedRectPath(
            innerPath, inset, inset, fw - inset * 2.0f, fh - inset * 2.0f, (std::max)(2.0f, radius - inset));
        Gdiplus::SolidBrush oledBlackBrush(Gdiplus::Color(255, 0, 0, 0));
        g.FillPath(&oledBlackBrush, &innerPath);

        // 3. Thin vertical rounded accent pill bar on the left (pure fill, no stroke)
        float barX = static_cast<float>(scale(11));
        float barY = static_cast<float>(scale(11));
        float barW = static_cast<float>(accentBarW);
        float barH = fh - barY * 2.0f;
        Gdiplus::GraphicsPath accentBarPath;
        BuildRoundedRectPath(accentBarPath, barX, barY, barW, barH, barW * 0.5f);
        Gdiplus::SolidBrush accentBrush(accentFill);
        g.FillPath(&accentBrush, &accentBarPath);

        // 4. Crisp integer-snapped title and subtitle glyphs (warm cream & muted tan)
        Gdiplus::SolidBrush titleBrush(Gdiplus::Color(255, 245, 240, 235));
        Gdiplus::SolidBrush subBrush(Gdiplus::Color(255, 196, 178, 162));

        int titleY = padY;
        Gdiplus::PointF ptTitle(static_cast<float>(leftInset), static_cast<float>(titleY));
        Gdiplus::PointF ptSub(
            static_cast<float>(leftInset),
            static_cast<float>(padY + titleH + gapY));

        g.DrawString(title.c_str(), -1, &titleFont, ptTitle, fmt, &titleBrush);
        g.DrawString(subtitle.c_str(), -1, &subFont, ptSub, fmt, &subBrush);
    }

    SelectObject(hdcMem, oldBmp);
    DeleteDC(hdcMem);

    g_toast.widthPx = w;
    g_toast.heightPx = h;

    // Position in bottom-right of active cursor's monitor work area
    RECT wa{};
    {
        HMONITOR hMon = MonitorFromPoint(cursorPt, MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi{};
        mi.cbSize = sizeof(mi);
        if (hMon && GetMonitorInfoW(hMon, &mi)) {
            wa = mi.rcWork;
        }
    }
    if (wa.right <= wa.left || wa.bottom <= wa.top) {
        SystemParametersInfoW(SPI_GETWORKAREA, 0, &wa, 0);
    }
    int margin = scale(16);
    int x = wa.right - w - margin;
    int y = wa.bottom - h - margin;

    SetWindowPos(
        g_toast.hwnd,
        HWND_TOPMOST,
        x, y, w, h,
        SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

static void DisplayToastPayload(const ToastPayload& item);

static void HideToastImmediate(bool drainQueue = true) {
    if (!g_toast.hwnd) return;
    KillTimer(g_toast.hwnd, kToastAnimTimerId);
    KillTimer(g_toast.hwnd, kToastHideTimerId);
    ShowWindow(g_toast.hwnd, SW_HIDE);
    g_toast.visible = false;
    g_toast.hovered = false;
    g_toast.fadingOut = false;
    g_toast.currentAlpha = 0;

    if (drainQueue && !g_stopRequested.load(std::memory_order_relaxed) && !g_toast.pendingQueue.empty()) {
        ToastPayload nextItem = std::move(g_toast.pendingQueue.front());
        g_toast.pendingQueue.pop_front();
        DisplayToastPayload(nextItem);
    }
}

static LRESULT CALLBACK ToastWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_MOUSEMOVE:
        if (!g_toast.hovered) {
            g_toast.hovered = true;
            KillTimer(hwnd, kToastHideTimerId);
            TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, hwnd, 0};
            TrackMouseEvent(&tme);
        }
        return 0;

    case WM_MOUSELEAVE:
        g_toast.hovered = false;
        if (g_toast.visible && !g_toast.fadingOut) {
            SetTimer(hwnd, kToastHideTimerId, 1400, nullptr);
        }
        return 0;

    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
        HideToastImmediate();
        return 0;

    case WM_TIMER:
        if (wParam == kToastHideTimerId) {
            KillTimer(hwnd, kToastHideTimerId);
            if (!g_toast.hovered && g_toast.visible) {
                g_toast.fadingOut = true;
                LARGE_INTEGER qpc{};
                QueryPerformanceCounter(&qpc);
                g_toast.animStartQpc = static_cast<uint64_t>(qpc.QuadPart);
                SetTimer(hwnd, kToastAnimTimerId, 16, nullptr);
            }
            return 0;
        }
        if (wParam == kToastAnimTimerId) {
            LARGE_INTEGER qpc{};
            QueryPerformanceCounter(&qpc);
            double elapsedMs = static_cast<double>(static_cast<uint64_t>(qpc.QuadPart) - g_toast.animStartQpc) *
                               1000.0 / static_cast<double>(g_toast.qpcFreq);
            double duration = g_toast.fadingOut ? 200.0 : 160.0;
            double p = SmoothStep01(elapsedMs / duration);
            BYTE alpha = g_toast.fadingOut
                ? static_cast<BYTE>(255.0 * (1.0 - p))
                : static_cast<BYTE>(255.0 * p);
            PresentToastAlpha(alpha);
            if (p >= 1.0) {
                KillTimer(hwnd, kToastAnimTimerId);
                if (g_toast.fadingOut) {
                    HideToastImmediate();
                }
            }
            return 0;
        }
        break;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static void ApplyToastCaptureAffinity() {
    if (!g_toast.hwnd) return;
    DWORD affinity = g_toastExcludeFromCapture.load(std::memory_order_relaxed)
        ? WDA_EXCLUDEFROMCAPTURE
        : WDA_NONE;
    SetWindowDisplayAffinity(g_toast.hwnd, affinity);
}

static void EnsureToastWindowCreated() {
    if (g_toast.hwnd) return;
    LARGE_INTEGER freq{};
    QueryPerformanceFrequency(&freq);
    g_toast.qpcFreq = freq.QuadPart > 0 ? static_cast<uint64_t>(freq.QuadPart) : 1;

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = ToastWndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.hCursor = LoadCursorW(nullptr, IDC_HAND);
    wc.lpszClassName = kToastWndClass;
    RegisterClassExW(&wc);

    g_toast.hwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        kToastWndClass,
        L"",
        WS_POPUP,
        0, 0, 0, 0,
        nullptr,
        nullptr,
        wc.hInstance,
        nullptr);
    if (g_toast.hwnd) {
        // Suppress any Windows 11 DWM system border around the layered popup window
        COLORREF borderNone = 0xFFFFFFFE; // DWMWA_COLOR_NONE
        DwmSetWindowAttribute(g_toast.hwnd, 34 /*DWMWA_BORDER_COLOR*/, &borderNone, sizeof(borderNone));
    }
    ApplyToastCaptureAffinity();
}

static void DisplayToastPayload(const ToastPayload& item) {
    if (g_toastExcludeFromCapture.load(std::memory_order_relaxed) && IsCaptureOverlayActive()) {
        g_toast.pendingQueue.clear();
        return;
    }

    EnsureToastWindowCreated();
    if (!g_toast.hwnd) return;
    ApplyToastCaptureAffinity();

    KillTimer(g_toast.hwnd, kToastAnimTimerId);
    KillTimer(g_toast.hwnd, kToastHideTimerId);

    g_toast.visible = true;
    g_toast.hovered = false;
    g_toast.fadingOut = false;
    g_toast.durationMs = item.durationMs;

    RenderToastContent(item.title, item.subtitle);
    PresentToastAlpha(0);

    LARGE_INTEGER qpc{};
    QueryPerformanceCounter(&qpc);
    g_toast.animStartQpc = static_cast<uint64_t>(qpc.QuadPart);
    SetTimer(g_toast.hwnd, kToastAnimTimerId, 16, nullptr);
    SetTimer(g_toast.hwnd, kToastHideTimerId, item.durationMs, nullptr);
}

static void HandleToastShowMessage(LPARAM lParam) {
    auto* payload = reinterpret_cast<ToastPayload*>(lParam);
    if (!payload) return;

    ToastPayload item = std::move(*payload);
    delete payload;

    if (g_toastExcludeFromCapture.load(std::memory_order_relaxed) && IsCaptureOverlayActive()) {
        return;
    }

    // Queue back-to-back automatic toasts (e.g. Startup Temp + Recycle Bin) so neither overwrites the other
    if (g_toast.visible && !item.isManual) {
        if (g_toast.pendingQueue.size() < 4) {
            g_toast.pendingQueue.push_back(std::move(item));
        }
        return;
    }

    if (item.isManual) {
        g_toast.pendingQueue.clear();
    }
    DisplayToastPayload(item);
}

static LRESULT CALLBACK UiMsgWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_WTSSESSION_CHANGE) {
        if (wParam == WTS_SESSION_LOCK || wParam == WTS_SESSION_LOGOFF) {
            g_sessionLocked.store(true, std::memory_order_release);
        } else if (wParam == WTS_SESSION_UNLOCK || wParam == WTS_SESSION_LOGON || wParam == WTS_CONSOLE_CONNECT) {
            g_sessionLocked.store(false, std::memory_order_release);
            if (g_wakeEvent) {
                SetEvent(g_wakeEvent);
            }
        }
        return 0;
    }
    if (msg == WM_APP_CHECK_TEMP_TAP) {
        POINT pt{static_cast<LONG>(wParam), static_cast<LONG>(lParam)};
        if (IsPointEmptyDesktopOrTaskbarUIA(g_pUia, pt)) {
            g_manualTempRequested.store(true, std::memory_order_release);
            if (g_wakeEvent) {
                SetEvent(g_wakeEvent);
            }
        }
        return 0;
    }
    if (msg == WM_APP_SHOW_TOAST) {
        HandleToastShowMessage(lParam);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static DWORD WINAPI UiThreadProc(LPVOID) {
    EnablePerMonitorDpiV2ForCurrentThread();
    ULONG_PTR gdiplusToken = 0;
    Gdiplus::GdiplusStartupInput gdiplusInput;
    bool gdiplusOk = (Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusInput, nullptr) == Gdiplus::Ok);

    HRESULT hrCom = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hrCom)) {
        CoCreateInstance(
            CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER, IID_IUIAutomation,
            reinterpret_cast<void**>(&g_pUia));
    }

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = UiMsgWndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = L"AutoCleanUiMsgClass";
    RegisterClassExW(&wc);

    // Use top-level invisible window so WTSRegisterSessionNotification receives WM_WTSSESSION_CHANGE
    g_uiMsgHwnd = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        wc.lpszClassName, L"", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, wc.hInstance, nullptr);
    bool wtsRegistered = false;
    if (g_uiMsgHwnd) {
        wtsRegistered = (WTSRegisterSessionNotification(g_uiMsgHwnd, NOTIFY_FOR_THIS_SESSION) != FALSE);
    }

    if (g_uiReadyEvent) {
        SetEvent(g_uiReadyEvent);
    }

    MSG msg{};
    while (!g_stopRequested.load(std::memory_order_relaxed)) {
        BOOL ret = GetMessageW(&msg, nullptr, 0, 0);
        if (ret <= 0 || msg.message == WM_QUIT) {
            break;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    g_toast.pendingQueue.clear();
    if (g_toast.hwnd) {
        KillTimer(g_toast.hwnd, kToastAnimTimerId);
        KillTimer(g_toast.hwnd, kToastHideTimerId);
        DestroyWindow(g_toast.hwnd);
        g_toast.hwnd = nullptr;
    }
    DestroyToastDib();
    UnregisterClassW(kToastWndClass, wc.hInstance);

    if (g_uiMsgHwnd) {
        if (wtsRegistered) {
            WTSUnRegisterSessionNotification(g_uiMsgHwnd);
        }
        DestroyWindow(g_uiMsgHwnd);
        g_uiMsgHwnd = nullptr;
    }
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    if (g_pUia) {
        g_pUia->Release();
        g_pUia = nullptr;
    }
    if (SUCCEEDED(hrCom)) {
        CoUninitialize();
    }
    if (gdiplusOk) {
        Gdiplus::GdiplusShutdown(gdiplusToken);
    }
    return 0;
}

// ============================================================================
// Event-Driven Directory Watcher (ReadDirectoryChangesW)
// ============================================================================

struct DirWatchHandle {
    HANDLE dirHandle = INVALID_HANDLE_VALUE;
    HANDLE eventHandle = nullptr;
    OVERLAPPED overlapped{};
    alignas(DWORD) BYTE buffer[16384]{};
    bool pending = false;
};

static bool ArmDirWatch(DirWatchHandle& w) {
    if (w.dirHandle == INVALID_HANDLE_VALUE || !w.eventHandle) {
        return false;
    }
    ResetEvent(w.eventHandle);
    w.overlapped = {};
    w.overlapped.hEvent = w.eventHandle;

    DWORD bytesReturned = 0;
    BOOL ok = ReadDirectoryChangesW(
        w.dirHandle,
        w.buffer,
        sizeof(w.buffer),
        TRUE,
        FILE_NOTIFY_CHANGE_FILE_NAME |
            FILE_NOTIFY_CHANGE_DIR_NAME |
            FILE_NOTIFY_CHANGE_SIZE |
            FILE_NOTIFY_CHANGE_LAST_WRITE,
        &bytesReturned,
        &w.overlapped,
        nullptr);
    w.pending = (ok != FALSE);
    return w.pending;
}

static void CloseDirWatches(std::vector<DirWatchHandle*>& watches) {
    for (DirWatchHandle* w : watches) {
        if (w->dirHandle != INVALID_HANDLE_VALUE) {
            if (w->pending) {
                CancelIoEx(w->dirHandle, &w->overlapped);
            }
            CloseHandle(w->dirHandle);
            w->dirHandle = INVALID_HANDLE_VALUE;
        }
        if (w->eventHandle) {
            CloseHandle(w->eventHandle);
            w->eventHandle = nullptr;
        }
        delete w;
    }
    watches.clear();
}

static void OpenDirWatches(const ModSettings& cfg, std::vector<DirWatchHandle*>& watches) {
    CloseDirWatches(watches);
    if (!cfg.thresholdEnabled) {
        return;
    }
    for (const auto& dir : GetTargetTempDirs(cfg)) {
        HANDLE hDir = CreateFileW(
            ToExtendedPath(dir).c_str(),
            FILE_LIST_DIRECTORY,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            nullptr,
            OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
            nullptr);
        if (hDir == INVALID_HANDLE_VALUE) {
            continue;
        }
        HANDLE hEv = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!hEv) {
            CloseHandle(hDir);
            continue;
        }
        auto* w = new DirWatchHandle();
        w->dirHandle = hDir;
        w->eventHandle = hEv;
        if (ArmDirWatch(*w)) {
            watches.push_back(w);
        } else {
            CloseHandle(hEv);
            CloseHandle(hDir);
            delete w;
        }
    }
}

// ============================================================================
// Recycle Bin Engine (Full Sweep + Oldest-First $I/$R Pruning)
// ============================================================================

enum class RecycleCleanStatus {
    Emptied,
    Pruned,
    AlreadyEmpty,
    DryRun,
    Failed,
};

struct RecycleCleanResult {
    RecycleCleanStatus status = RecycleCleanStatus::AlreadyEmpty;
    uint64_t bytesFreed = 0;
    uint64_t itemsCleaned = 0;
    uint64_t itemsRemaining = 0;
};

static void PersistTelemetry(uint64_t freedBytes, const wchar_t* reason, bool dryRun);

static bool QueryRecycleBinStats(uint64_t* outBytes, uint64_t* outItems) {
    SHQUERYRBINFO rbInfo{};
    rbInfo.cbSize = sizeof(rbInfo);
    HRESULT hr = SHQueryRecycleBinW(nullptr, &rbInfo);
    if (FAILED(hr)) {
        return false;
    }
    if (outBytes) {
        *outBytes = rbInfo.i64Size > 0 ? static_cast<uint64_t>(rbInfo.i64Size) : 0;
    }
    if (outItems) {
        *outItems = rbInfo.i64NumItems > 0 ? static_cast<uint64_t>(rbInfo.i64NumItems) : 0;
    }
    return true;
}

static std::wstring GetCurrentUserSidString() {
    HANDLE hToken = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        return L"";
    }
    DWORD len = 0;
    GetTokenInformation(hToken, TokenUser, nullptr, 0, &len);
    if (len == 0) {
        CloseHandle(hToken);
        return L"";
    }
    std::vector<BYTE> buf(len);
    std::wstring result;
    if (GetTokenInformation(hToken, TokenUser, buf.data(), len, &len)) {
        auto* tu = reinterpret_cast<TOKEN_USER*>(buf.data());
        LPWSTR sidStr = nullptr;
        if (ConvertSidToStringSidW(tu->User.Sid, &sidStr) && sidStr) {
            result = sidStr;
            LocalFree(sidStr);
        }
    }
    CloseHandle(hToken);
    return result;
}

struct RecycleItemCandidate {
    std::wstring metaFileExtPath;
    std::wstring dataFileExtPath;
    uint64_t sizeBytes = 0;
    uint64_t deletedAt100Ns = 0;
};

static bool ReadRecycleMetaFile(const std::wstring& metaExtPath, uint64_t* outSize, uint64_t* outDeleted100Ns) {
    HANDLE hFile = CreateFileW(
        metaExtPath.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        return false;
    }

    #pragma pack(push, 1)
    struct RecycleMetaHeader {
        uint64_t version;
        uint64_t fileSize;
        uint64_t deletedTime100Ns;
    };
    #pragma pack(pop)

    RecycleMetaHeader hdr{};
    DWORD bytesRead = 0;
    BOOL ok = ReadFile(hFile, &hdr, sizeof(hdr), &bytesRead, nullptr);
    CloseHandle(hFile);

    if (!ok || bytesRead < sizeof(hdr) || (hdr.version != 1 && hdr.version != 2)) {
        return false;
    }
    if (outSize) *outSize = hdr.fileSize;
    if (outDeleted100Ns) *outDeleted100Ns = hdr.deletedTime100Ns;
    return true;
}

static bool DeleteTreeRecursive(const std::wstring& extPath) {
    std::wstring pattern = extPath + L"\\*";
    WIN32_FIND_DATAW fd{};
    HANDLE hFind = FindFirstFileExW(
        pattern.c_str(), FindExInfoBasic, &fd, FindExSearchNameMatch, nullptr, FIND_FIRST_EX_LARGE_FETCH);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0) {
                continue;
            }
            std::wstring child = extPath + L"\\" + fd.cFileName;
            if (fd.dwFileAttributes & (FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN)) {
                SetFileAttributesW(child.c_str(), FILE_ATTRIBUTE_NORMAL);
            }
            if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
                !(fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)) {
                DeleteTreeRecursive(child);
            } else if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                RemoveDirectoryW(child.c_str());
            } else {
                DeleteFileW(child.c_str());
            }
        } while (FindNextFileW(hFind, &fd));
        FindClose(hFind);
    }
    SetFileAttributesW(extPath.c_str(), FILE_ATTRIBUTE_NORMAL);
    return RemoveDirectoryW(extPath.c_str()) != FALSE;
}

static bool DeleteRecycleCandidate(const RecycleItemCandidate& item) {
    bool dataRemoved = true;
    DWORD attrs = GetFileAttributesW(item.dataFileExtPath.c_str());
    if (attrs != INVALID_FILE_ATTRIBUTES) {
        if (attrs & (FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN)) {
            SetFileAttributesW(item.dataFileExtPath.c_str(), FILE_ATTRIBUTE_NORMAL);
        }
        if ((attrs & FILE_ATTRIBUTE_DIRECTORY) && !(attrs & FILE_ATTRIBUTE_REPARSE_POINT)) {
            dataRemoved = DeleteTreeRecursive(item.dataFileExtPath);
        } else if (attrs & FILE_ATTRIBUTE_DIRECTORY) {
            dataRemoved = RemoveDirectoryW(item.dataFileExtPath.c_str()) != FALSE;
        } else {
            dataRemoved = DeleteFileW(item.dataFileExtPath.c_str()) != FALSE;
        }
    }
    if (!dataRemoved) {
        return false;
    }
    SetFileAttributesW(item.metaFileExtPath.c_str(), FILE_ATTRIBUTE_NORMAL);
    return DeleteFileW(item.metaFileExtPath.c_str()) != FALSE;
}

static void CollectRecycleBinCandidates(
    uint64_t now100Ns,
    uint64_t minAgeMs,
    std::vector<RecycleItemCandidate>& outCandidates,
    uint64_t* outTotalBytes,
    uint64_t* outTotalItems) {
    std::wstring sid = GetCurrentUserSidString();
    if (sid.empty()) {
        return;
    }

    uint64_t grace100Ns = MultiplyClampU64(minAgeMs, 10000ULL);
    DWORD driveMask = GetLogicalDrives();
    for (int d = 0; d < 26; d++) {
        if (!(driveMask & (1U << d))) {
            continue;
        }
        wchar_t root[] = {static_cast<wchar_t>(L'A' + d), L':', L'\\', L'\0'};
        if (GetDriveTypeW(root) != DRIVE_FIXED) {
            continue;
        }

        std::wstring binDir = std::wstring(L"\\\\?\\") + root + L"$Recycle.Bin\\" + sid;
        std::wstring pattern = binDir + L"\\$I*";
        WIN32_FIND_DATAW fd{};
        HANDLE hFind = FindFirstFileExW(
            pattern.c_str(), FindExInfoBasic, &fd, FindExSearchNameMatch, nullptr, FIND_FIRST_EX_LARGE_FETCH);
        if (hFind == INVALID_HANDLE_VALUE) {
            continue;
        }

        do {
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                continue;
            }
            if (wcsncmp(fd.cFileName, L"$I", 2) != 0 || fd.cFileName[2] == L'\0') {
                continue;
            }

            std::wstring metaPath = binDir + L"\\" + fd.cFileName;
            std::wstring rName = std::wstring(L"$R") + (fd.cFileName + 2);
            std::wstring dataPath = binDir + L"\\" + rName;

            uint64_t itemSize = 0;
            uint64_t deletedAt = 0;
            if (!ReadRecycleMetaFile(metaPath, &itemSize, &deletedAt)) {
                deletedAt = FileTimeToU64(fd.ftLastWriteTime);
            }

            if (outTotalBytes) *outTotalBytes += itemSize;
            if (outTotalItems) (*outTotalItems)++;

            if (grace100Ns > 0 && now100Ns > deletedAt && (now100Ns - deletedAt) < grace100Ns) {
                continue;
            }

            outCandidates.push_back({metaPath, dataPath, itemSize, deletedAt});
        } while (FindNextFileW(hFind, &fd));

        FindClose(hFind);
    }
}

static uint64_t GetCurrentSystemFileTime100Ns() {
    FILETIME ftNow{};
    GetSystemTimeAsFileTime(&ftNow);
    return FileTimeToU64(ftNow);
}

static uint64_t LoadPersistedFileTime100Ns(PCWSTR keyName) {
    WCHAR buf[64]{};
    if (Wh_GetStringValue(keyName, buf, ARRAYSIZE(buf)) == 0 || !buf[0]) {
        return 0;
    }
    return _wcstoui64(buf, nullptr, 10);
}

static void SavePersistedFileTime100Ns(PCWSTR keyName, uint64_t val100Ns) {
    WCHAR buf[64]{};
    swprintf_s(buf, L"%llu", static_cast<unsigned long long>(val100Ns));
    Wh_SetStringValue(keyName, buf);
}

static RecycleCleanResult RunRecycleBinClean(const ModSettings& cfg, const wchar_t* reason, bool forceFullSweep) {
    RecycleCleanResult res;
    uint64_t rbBytes = 0;
    uint64_t rbItems = 0;
    bool statsOk = QueryRecycleBinStats(&rbBytes, &rbItems);

    uint64_t now100Ns = GetCurrentSystemFileTime100Ns();
    std::vector<RecycleItemCandidate> candidates;
    uint64_t scannedBytes = 0;
    uint64_t scannedItems = 0;
    uint64_t effectiveMinAgeMs = forceFullSweep ? 0ULL : cfg.rbMinAgeMs;
    CollectRecycleBinCandidates(now100Ns, effectiveMinAgeMs, candidates, &scannedBytes, &scannedItems);

    if (scannedItems > rbItems) rbItems = scannedItems;
    if (scannedBytes > rbBytes) rbBytes = scannedBytes;

    if (!statsOk && scannedItems == 0) {
        Wh_Log(L"AutoClean [%s | RecycleBin]: SHQueryRecycleBinW failed", reason);
        res.status = RecycleCleanStatus::Failed;
        return res;
    }
    if (rbItems == 0 && rbBytes == 0) {
        Wh_Log(L"AutoClean [%s | RecycleBin]: already empty", reason);
        res.status = RecycleCleanStatus::AlreadyEmpty;
        return res;
    }

    if (cfg.dryRun) {
        Wh_Log(
            L"AutoClean [%s | RecycleBin] (DRY-RUN ONLY): would clean %llu bytes (%llu items) — zero files touched",
            reason,
            static_cast<unsigned long long>(rbBytes),
            static_cast<unsigned long long>(rbItems));
        res.status = RecycleCleanStatus::DryRun;
        res.bytesFreed = rbBytes;
        res.itemsCleaned = rbItems;
        res.itemsRemaining = 0;
        return res;
    }

    bool isExpireOnlyPass = (wcscmp(reason, L"RecycleAge30d") == 0);
    bool useOldestFirst = isExpireOnlyPass || (!forceFullSweep && !cfg.rbFullSweep && cfg.rbOldestFirst);
    BOOL bgEntered = SetThreadPriority(GetCurrentThread(), THREAD_MODE_BACKGROUND_BEGIN);

    if (useOldestFirst) {
        if (!candidates.empty()) {
            std::sort(candidates.begin(), candidates.end(), [](const RecycleItemCandidate& a, const RecycleItemCandidate& b) {
                return a.deletedAt100Ns < b.deletedAt100Ns;
            });

            uint64_t expireAge100Ns = cfg.rbTimerEnabled
                ? MultiplyClampU64(cfg.rbTimerIntervalMs, 10000ULL)
                : MultiplyClampU64(30ULL * 86400ULL * 1000ULL, 10000ULL);

            uint64_t remBytes = scannedBytes > 0 ? scannedBytes : rbBytes;
            uint64_t remItems = scannedItems > 0 ? scannedItems : rbItems;
            uint64_t targetBytes = ScaleByPercentU64(remBytes, cfg.rbTargetPercent);
            uint64_t targetItems = ScaleByPercentU64(remItems, cfg.rbTargetPercent);
            if (cfg.rbSizeThresholdEnabled && remBytes >= cfg.rbSizeThresholdBytes) {
                targetBytes = ScaleByPercentU64(cfg.rbSizeThresholdBytes, cfg.rbTargetPercent);
            }
            if (cfg.rbCountThresholdEnabled && remItems >= cfg.rbMaxItems) {
                targetItems = ScaleByPercentU64(cfg.rbMaxItems, cfg.rbTargetPercent);
            }

            uint64_t freedBytes = 0;
            uint64_t deletedItems = 0;
            for (const auto& item : candidates) {
                if (g_stopRequested.load(std::memory_order_relaxed)) {
                    break;
                }
                bool isExpiredByAge = (expireAge100Ns > 0 && now100Ns > item.deletedAt100Ns &&
                                       (now100Ns - item.deletedAt100Ns) >= expireAge100Ns);
                if (isExpireOnlyPass && !isExpiredByAge) {
                    // Candidates are sorted oldest-first; once an item is newer than 30d/schedule, stop.
                    break;
                }
                if (!isExpireOnlyPass && !isExpiredByAge &&
                    remBytes <= targetBytes && remItems <= targetItems && deletedItems > 0) {
                    break;
                }
                if (DeleteRecycleCandidate(item)) {
                    freedBytes += item.sizeBytes;
                    deletedItems++;
                    if (remBytes >= item.sizeBytes) remBytes -= item.sizeBytes;
                    else remBytes = 0;
                    if (remItems > 0) remItems--;
                }
            }

            if (bgEntered) {
                SetThreadPriority(GetCurrentThread(), THREAD_MODE_BACKGROUND_END);
            }

            if (deletedItems > 0) {
                SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
                PersistTelemetry(freedBytes, reason, cfg.dryRun);
            }
            Wh_Log(
                L"AutoClean [%s | RecycleBin-OldestFirst]: freed %llu bytes (%llu items pruned, %llu remaining)",
                reason,
                static_cast<unsigned long long>(freedBytes),
                static_cast<unsigned long long>(deletedItems),
                static_cast<unsigned long long>(remItems));
            res.status = deletedItems > 0 ? RecycleCleanStatus::Pruned : RecycleCleanStatus::AlreadyEmpty;
            res.bytesFreed = freedBytes;
            res.itemsCleaned = deletedItems;
            res.itemsRemaining = remItems;
            return res;
        }
        if (isExpireOnlyPass) {
            if (bgEntered) {
                SetThreadPriority(GetCurrentThread(), THREAD_MODE_BACKGROUND_END);
            }
            res.status = RecycleCleanStatus::AlreadyEmpty;
            return res;
        }
    }

    DWORD flags = SHERB_NOCONFIRMATION | SHERB_NOPROGRESSUI;
    if (!cfg.rbPlaySound) {
        flags |= SHERB_NOSOUND;
    }
    HRESULT hr = SHEmptyRecycleBinW(nullptr, nullptr, flags);
    if (FAILED(hr) && !candidates.empty()) {
        // Fallback: direct $I/$R removal if SHEmptyRecycleBinW fails on background thread
        uint64_t fallbackFreed = 0;
        uint64_t fallbackDeleted = 0;
        for (const auto& item : candidates) {
            if (g_stopRequested.load(std::memory_order_relaxed)) break;
            if (DeleteRecycleCandidate(item)) {
                fallbackFreed += item.sizeBytes;
                fallbackDeleted++;
            }
        }
        if (fallbackDeleted > 0) {
            SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
            hr = S_OK;
            rbBytes = fallbackFreed;
            rbItems = fallbackDeleted;
        }
    }
    if (bgEntered) {
        SetThreadPriority(GetCurrentThread(), THREAD_MODE_BACKGROUND_END);
    }

    if (FAILED(hr)) {
        Wh_Log(
            L"AutoClean [%s | RecycleBin-FullSweep]: SHEmptyRecycleBinW failed (hr=0x%08X)",
            reason,
            static_cast<unsigned int>(hr));
        res.status = RecycleCleanStatus::Failed;
        return res;
    }

    PersistTelemetry(rbBytes, reason, cfg.dryRun);
    Wh_Log(
        L"AutoClean [%s | RecycleBin-FullSweep]: freed %llu bytes (%llu items)",
        reason,
        static_cast<unsigned long long>(rbBytes),
        static_cast<unsigned long long>(rbItems));
    res.status = RecycleCleanStatus::Emptied;
    res.bytesFreed = rbBytes;
    res.itemsCleaned = rbItems;
    res.itemsRemaining = 0;
    return res;
}

// ============================================================================
// Temp Engine: Background-I/O Measure, Oldest-First Pruning, Full Sweep
// ============================================================================

struct CleanStats {
    uint64_t totalBytesScanned = 0;
    uint64_t eligibleBytes = 0;
    uint64_t bytesFreed = 0;
    uint64_t filesDeleted = 0;
    uint64_t dirsDeleted = 0;
    uint64_t skippedLocked = 0;
    uint64_t skippedRecent = 0;
    uint64_t skippedExcluded = 0;
    uint64_t skippedProtectedAgents = 0;
    uint64_t skippedReparse = 0;
};

struct FileCandidate {
    std::wstring extPath;
    uint64_t sizeBytes = 0;
    uint64_t timestamp100Ns = 0;
};

struct DirCandidate {
    std::wstring extPath;
    size_t depth = 0;
};

static uint64_t MeasureDirectoryBytes(const std::wstring& extDir, const ModSettings& cfg) {
    if (g_stopRequested.load(std::memory_order_relaxed)) {
        return 0;
    }

    std::wstring pattern = extDir + L"\\*";
    WIN32_FIND_DATAW fd{};
    HANDLE hFind = FindFirstFileExW(
        pattern.c_str(), FindExInfoBasic, &fd, FindExSearchNameMatch, nullptr, FIND_FIRST_EX_LARGE_FETCH);
    if (hFind == INVALID_HANDLE_VALUE) {
        return 0;
    }

    uint64_t total = 0;
    do {
        if (g_stopRequested.load(std::memory_order_relaxed)) {
            break;
        }
        if (wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0) {
            continue;
        }
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
            continue;
        }
        if (IsProtectedAiAgentName(fd.cFileName, cfg) || IsExcludedName(fd.cFileName, cfg.excludePatterns)) {
            continue;
        }
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            total += MeasureDirectoryBytes(extDir + L"\\" + fd.cFileName, cfg);
        } else {
            uint64_t fileSize = (static_cast<uint64_t>(fd.nFileSizeHigh) << 32) | fd.nFileSizeLow;
            total += fileSize;
        }
    } while (FindNextFileW(hFind, &fd));

    FindClose(hFind);
    return total;
}

static void CollectCandidatesRecursive(
    const std::wstring& extDir,
    bool isRoot,
    size_t depth,
    uint64_t now100Ns,
    uint64_t effectiveMinAgeMs,
    const ModSettings& cfg,
    std::vector<FileCandidate>& files,
    std::vector<DirCandidate>& dirs,
    CleanStats& stats) {
    if (g_stopRequested.load(std::memory_order_relaxed)) {
        return;
    }

    std::wstring pattern = extDir + L"\\*";
    WIN32_FIND_DATAW fd{};
    HANDLE hFind = FindFirstFileExW(
        pattern.c_str(), FindExInfoBasic, &fd, FindExSearchNameMatch, nullptr, FIND_FIRST_EX_LARGE_FETCH);
    if (hFind == INVALID_HANDLE_VALUE) {
        return;
    }

    do {
        if (g_stopRequested.load(std::memory_order_relaxed)) {
            break;
        }
        if (wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0) {
            continue;
        }
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
            stats.skippedReparse++;
            continue;
        }
        if (_wcsicmp(fd.cFileName, L"desktop.ini") == 0) {
            continue;
        }
        if (IsProtectedAiAgentName(fd.cFileName, cfg)) {
            stats.skippedProtectedAgents++;
            continue;
        }
        if (IsExcludedName(fd.cFileName, cfg.excludePatterns)) {
            stats.skippedExcluded++;
            continue;
        }

        std::wstring childPath = extDir + L"\\" + fd.cFileName;
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            CollectCandidatesRecursive(
                childPath, false, depth + 1, now100Ns, effectiveMinAgeMs, cfg, files, dirs, stats);
            // Always queue subdirectories for empty-folder removal; RemoveDirectoryW only removes empty dirs
            dirs.push_back({childPath, depth + 1});
        } else {
            uint64_t fileSize = (static_cast<uint64_t>(fd.nFileSizeHigh) << 32) | fd.nFileSizeLow;
            stats.totalBytesScanned += fileSize;
            if (!IsOlderThanGracePeriod(fd, now100Ns, effectiveMinAgeMs)) {
                stats.skippedRecent++;
                continue;
            }
            stats.eligibleBytes += fileSize;
            uint64_t newest = (std::max)(FileTimeToU64(fd.ftLastWriteTime), FileTimeToU64(fd.ftCreationTime));
            files.push_back({childPath, fileSize, newest});
        }
    } while (FindNextFileW(hFind, &fd));

    FindClose(hFind);
    (void)isRoot;
}

static void PersistTelemetry(uint64_t freedBytes, const wchar_t* reason, bool dryRun) {
    if (dryRun || freedBytes == 0) {
        return;
    }
    g_totalFreedBytes += freedBytes;

    SYSTEMTIME st{};
    GetLocalTime(&st);
    WCHAR timeBuf[64]{};
    swprintf_s(
        timeBuf,
        L"%04u-%02u-%02u %02u:%02u:%02u",
        st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

    WCHAR lastBytesBuf[32]{};
    WCHAR totalMbBuf[32]{};
    swprintf_s(lastBytesBuf, L"%llu", static_cast<unsigned long long>(freedBytes));
    swprintf_s(totalMbBuf, L"%llu", static_cast<unsigned long long>(g_totalFreedBytes / (1024ULL * 1024ULL)));

    Wh_SetStringValue(L"telemetry.lastCleanTime", timeBuf);
    Wh_SetStringValue(L"telemetry.lastReason", reason);
    Wh_SetStringValue(L"telemetry.lastFreedBytes", lastBytesBuf);
    Wh_SetStringValue(L"telemetry.totalFreedMB", totalMbBuf);
}

static CleanStats RunCleanupPass(const ModSettings& cfg, const wchar_t* reason, bool forceFullSweep) {
    // Enter Windows background I/O & CPU mode without holding any locks.
    BOOL bgEntered = SetThreadPriority(GetCurrentThread(), THREAD_MODE_BACKGROUND_BEGIN);

    uint64_t now100Ns = GetCurrentSystemFileTime100Ns();
    bool isFullSweepMode = forceFullSweep || cfg.fullSweep;
    // Full sweep (including Startup Full Sweep) cleans all unlocked temp files regardless of minFileAgeMs
    uint64_t effectiveMinAgeMs = isFullSweepMode ? 0ULL : cfg.minFileAgeMs;

    CleanStats stats;
    std::vector<FileCandidate> files;
    std::vector<DirCandidate> dirs;

    for (const auto& dir : GetTargetTempDirs(cfg)) {
        CollectCandidatesRecursive(
            ToExtendedPath(dir), true, 0, now100Ns, effectiveMinAgeMs, cfg, files, dirs, stats);
    }

    bool isTimerOrManual = (wcscmp(reason, L"Timer") == 0 || wcscmp(reason, L"Taskbar3xClick") == 0);
    bool useOldestFirst = !isFullSweepMode && cfg.oldestFirst;
    if (useOldestFirst && isTimerOrManual && stats.totalBytesScanned < cfg.thresholdBytes) {
        // When below the size threshold on a scheduled timer or manual click, clean all eligible files
        useOldestFirst = false;
    }

    uint64_t targetBytes = 0;
    uint64_t remainingBytes = stats.eligibleBytes;
    if (useOldestFirst) {
        uint64_t basis = (stats.totalBytesScanned >= cfg.thresholdBytes)
            ? cfg.thresholdBytes
            : stats.eligibleBytes;
        targetBytes = ScaleByPercentU64(basis, cfg.targetPercent);
        std::sort(files.begin(), files.end(), [](const FileCandidate& a, const FileCandidate& b) {
            return a.timestamp100Ns < b.timestamp100Ns;
        });
    }

    for (const auto& f : files) {
        if (g_stopRequested.load(std::memory_order_relaxed)) {
            break;
        }
        if (useOldestFirst && remainingBytes <= targetBytes && stats.filesDeleted > 0) {
            break;
        }
        if (cfg.dryRun) {
            // Strictly non-destructive: never modify attributes or delete files in Dry-Run mode
            stats.bytesFreed += f.sizeBytes;
            stats.filesDeleted++;
            if (remainingBytes >= f.sizeBytes) remainingBytes -= f.sizeBytes;
            else remainingBytes = 0;
            continue;
        }
        DWORD attrs = GetFileAttributesW(f.extPath.c_str());
        if (attrs != INVALID_FILE_ATTRIBUTES &&
            (attrs & (FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM))) {
            SetFileAttributesW(f.extPath.c_str(), FILE_ATTRIBUTE_NORMAL);
        }
        if (DeleteFileW(f.extPath.c_str())) {
            stats.bytesFreed += f.sizeBytes;
            stats.filesDeleted++;
            if (remainingBytes >= f.sizeBytes) remainingBytes -= f.sizeBytes;
            else remainingBytes = 0;
        } else {
            stats.skippedLocked++;
        }
    }

    // Prune deepest empty directories first.
    std::sort(dirs.begin(), dirs.end(), [](const DirCandidate& a, const DirCandidate& b) {
        return a.depth > b.depth;
    });
    for (const auto& d : dirs) {
        if (g_stopRequested.load(std::memory_order_relaxed)) {
            break;
        }
        if (cfg.dryRun) {
            continue;
        }
        DWORD dAttrs = GetFileAttributesW(d.extPath.c_str());
        if (dAttrs != INVALID_FILE_ATTRIBUTES &&
            (dAttrs & (FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM))) {
            SetFileAttributesW(d.extPath.c_str(), FILE_ATTRIBUTE_NORMAL);
        }
        if (RemoveDirectoryW(d.extPath.c_str())) {
            stats.dirsDeleted++;
        }
    }

    if (bgEntered) {
        SetThreadPriority(GetCurrentThread(), THREAD_MODE_BACKGROUND_END);
    }

    PersistTelemetry(stats.bytesFreed, reason, cfg.dryRun);

    Wh_Log(
        L"AutoClean [%s | %s]%s: freed %llu bytes (%llu files, %llu dirs); scanned=%llu skipped locked=%llu recent=%llu aiAgents=%llu excluded=%llu reparse=%llu",
        reason,
        useOldestFirst ? L"OldestFirst" : L"FullSweep",
        cfg.dryRun ? L" (DRY-RUN ONLY)" : L"",
        static_cast<unsigned long long>(stats.bytesFreed),
        static_cast<unsigned long long>(stats.filesDeleted),
        static_cast<unsigned long long>(stats.dirsDeleted),
        static_cast<unsigned long long>(stats.totalBytesScanned),
        static_cast<unsigned long long>(stats.skippedLocked),
        static_cast<unsigned long long>(stats.skippedRecent),
        static_cast<unsigned long long>(stats.skippedProtectedAgents),
        static_cast<unsigned long long>(stats.skippedExcluded),
        static_cast<unsigned long long>(stats.skippedReparse));
    return stats;
}

static void NotifyTempToast(const CleanStats& stats, bool dryRun, bool isManual) {
    bool didCleanSomething = (stats.bytesFreed > 0 || stats.filesDeleted > 0 || stats.dirsDeleted > 0);
    if (!isManual && !dryRun && !didCleanSomething) {
        return;
    }
    std::wstring title;
    std::wstring subtitle;
    if (dryRun) {
        title = L"DRY RUN ONLY — Temp Not Cleaned";
        subtitle = L"Simulation only: Would free " + FormatBytesHuman(stats.bytesFreed) +
                   L" (" + std::to_wstring(stats.filesDeleted) + L" files untouched)";
    } else if (didCleanSomething) {
        title = L"Temp Cleaned";
        subtitle = L"Freed " + FormatBytesHuman(stats.bytesFreed) +
                   L" (" + std::to_wstring(stats.filesDeleted) + L" files";
        if (stats.dirsDeleted > 0) {
            subtitle += L", " + std::to_wstring(stats.dirsDeleted) + L" folders";
        }
        subtitle += L")";
    } else {
        title = L"Temp Already Clean";
        if (stats.skippedLocked > 0 || stats.skippedRecent > 0) {
            subtitle = L"0 bytes freed (" + std::to_wstring(stats.skippedLocked) +
                       L" in-use locked, " + std::to_wstring(stats.skippedRecent) + L" recent)";
        } else {
            subtitle = L"No eligible temp files to remove";
        }
    }
    ShowCleanToast(title, subtitle, isManual || dryRun);
}

static void NotifyRecycleToast(const RecycleCleanResult& res, bool isManual) {
    bool isDryRun = (res.status == RecycleCleanStatus::DryRun);
    bool didCleanSomething = (res.bytesFreed > 0 || res.itemsCleaned > 0);
    if (!isManual && !isDryRun && !didCleanSomething) {
        return;
    }
    std::wstring title;
    std::wstring subtitle;
    switch (res.status) {
    case RecycleCleanStatus::Emptied:
        title = L"Recycle Bin Emptied";
        subtitle = L"Freed " + FormatBytesHuman(res.bytesFreed) +
                   L" (" + std::to_wstring(res.itemsCleaned) + L" items)";
        break;
    case RecycleCleanStatus::Pruned:
        title = L"Recycle Bin Pruned";
        subtitle = L"Freed " + FormatBytesHuman(res.bytesFreed) +
                   L" (" + std::to_wstring(res.itemsCleaned) + L" old items removed)";
        break;
    case RecycleCleanStatus::DryRun:
        title = L"DRY RUN ONLY — Recycle Bin Untouched";
        subtitle = L"Simulation only: Would free " + FormatBytesHuman(res.bytesFreed) +
                   L" (" + std::to_wstring(res.itemsCleaned) + L" items untouched)";
        break;
    case RecycleCleanStatus::AlreadyEmpty:
        title = L"Recycle Bin Empty";
        subtitle = L"Already empty (0 items)";
        break;
    case RecycleCleanStatus::Failed:
        title = L"Recycle Bin Error";
        subtitle = L"Could not empty Recycle Bin";
        break;
    }
    ShowCleanToast(title, subtitle, isManual || isDryRun);
}

static void NotifySingleShotDryRunToast(const CleanStats& tempStats, const RecycleCleanResult& rbRes) {
    std::wstring title = L"DRY RUN ONLY — No Files Deleted";
    std::wstring subtitle = L"Simulation: Temp " + FormatBytesHuman(tempStats.bytesFreed) +
                            L" (" + std::to_wstring(tempStats.filesDeleted) + L" files) • Bin " +
                            FormatBytesHuman(rbRes.bytesFreed) +
                            L" (" + std::to_wstring(rbRes.itemsCleaned) + L" items)";
    ShowCleanToast(title, subtitle, true);
}

// ============================================================================
// Worker Loop & Settings Loading
// ============================================================================

static ModSettings SnapshotSettings() {
    EnterCriticalSection(&g_settingsMtx);
    ModSettings copy = g_settings;
    LeaveCriticalSection(&g_settingsMtx);
    return copy;
}

static DWORD WINAPI WorkerThreadProc(LPVOID) {
    HRESULT hrCom = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    ModSettings cfg = SnapshotSettings();
    std::vector<DirWatchHandle*> watches;
    OpenDirWatches(cfg, watches);

    uint64_t now = GetTickCount64();
    uint64_t loginDesktopReadyTick = 0;
    if (IsInteractiveUserDesktopReady()) {
        loginDesktopReadyTick = now;
        Wh_Log(L"AutoClean: interactive user desktop active; lazy startup scheduled in %llu ms",
               static_cast<unsigned long long>(cfg.startupDelayMs));
    } else {
        Wh_Log(L"AutoClean: waiting for user to pass Windows logon gate before starting timers");
    }

    uint64_t lastTimerClean = now;
    uint64_t lastRecycleTimerClean = now;
    uint64_t lastTempTimerFt100Ns = LoadPersistedFileTime100Ns(L"schedule.lastTempTimerFt");
    uint64_t lastRecycleTimerFt100Ns = LoadPersistedFileTime100Ns(L"schedule.lastRecycleTimerFt");
    uint64_t nowFt100Ns = GetCurrentSystemFileTime100Ns();
    if (lastTempTimerFt100Ns == 0 || lastTempTimerFt100Ns > nowFt100Ns) {
        lastTempTimerFt100Ns = nowFt100Ns;
        SavePersistedFileTime100Ns(L"schedule.lastTempTimerFt", lastTempTimerFt100Ns);
    }
    if (lastRecycleTimerFt100Ns == 0 || lastRecycleTimerFt100Ns > nowFt100Ns) {
        lastRecycleTimerFt100Ns = nowFt100Ns;
        SavePersistedFileTime100Ns(L"schedule.lastRecycleTimerFt", lastRecycleTimerFt100Ns);
    }

    uint64_t lastEmergencyCheck = 0;
    uint64_t lastRecycleCheck = 0;
    uint64_t lastCleanTime = 0;
    uint64_t lastRecycleCleanTime = 0;
    bool startupTempPending = cfg.startupCleanTemp;
    bool startupRecyclePending = cfg.startupCleanRecycle;
    bool watchDirty = cfg.thresholdEnabled;
    uint64_t lastWriteEventTime = watchDirty ? now : 0;
    bool deferredPending = false;
    bool lastCleanWinTemp = cfg.cleanWindowsTemp;
    bool lastThreshEnabled = cfg.thresholdEnabled;
    bool lastTimerEnabled = cfg.timerEnabled;
    bool lastRbTimerEnabled = cfg.rbTimerEnabled;
    bool lastDryRun = false;
    bool dryRunCompletedOnce = false;

    while (!g_stopRequested.load(std::memory_order_relaxed)) {
        cfg = SnapshotSettings();
        now = GetTickCount64();

        if (cfg.cleanWindowsTemp != lastCleanWinTemp || cfg.thresholdEnabled != lastThreshEnabled) {
            OpenDirWatches(cfg, watches);
            lastCleanWinTemp = cfg.cleanWindowsTemp;
            lastThreshEnabled = cfg.thresholdEnabled;
            watchDirty = cfg.thresholdEnabled;
            lastWriteEventTime = now;
        }

        if (!cfg.dryRun) {
            dryRunCompletedOnce = false;
            lastDryRun = false;
        }

        if (g_settingsReconfigured.exchange(false, std::memory_order_acq_rel)) {
            lastRecycleCheck = 0;
            lastEmergencyCheck = 0;
            if (cfg.timerEnabled && !lastTimerEnabled) {
                lastTimerClean = now;
                lastTempTimerFt100Ns = GetCurrentSystemFileTime100Ns();
                SavePersistedFileTime100Ns(L"schedule.lastTempTimerFt", lastTempTimerFt100Ns);
            }
            if (cfg.rbTimerEnabled && !lastRbTimerEnabled) {
                lastRecycleTimerClean = now;
                lastRecycleTimerFt100Ns = GetCurrentSystemFileTime100Ns();
                SavePersistedFileTime100Ns(L"schedule.lastRecycleTimerFt", lastRecycleTimerFt100Ns);
            }
            lastTimerEnabled = cfg.timerEnabled;
            lastRbTimerEnabled = cfg.rbTimerEnabled;
            if (cfg.thresholdEnabled) {
                watchDirty = true;
                lastWriteEventTime = 0;
            }
        }

        // Single-shot Dry-Run Mode: when enabled, run exactly once, show explicit preview toast, and stop repeating
        if (cfg.dryRun && (!dryRunCompletedOnce || !lastDryRun)) {
            lastDryRun = true;
            dryRunCompletedOnce = true;
            Wh_Log(L"AutoClean Dry-Run mode enabled: running single-shot non-destructive preview");
            CleanStats tempPreview = RunCleanupPass(cfg, L"DryRunPreview", cfg.fullSweep);
            RecycleCleanResult rbPreview = RunRecycleBinClean(cfg, L"DryRunPreview", cfg.rbFullSweep);
            NotifySingleShotDryRunToast(tempPreview, rbPreview);
        }

        // 0a. Immediate manual 3x left-click Temp trigger
        if (g_manualTempRequested.exchange(false, std::memory_order_acq_rel)) {
            Wh_Log(L"AutoClean 3x taskbar/desktop left-click Temp trigger activated");
            CleanStats stats = RunCleanupPass(cfg, L"Taskbar3xClick", cfg.fullSweep);
            lastCleanTime = GetTickCount64();
            watchDirty = false;
            NotifyTempToast(stats, cfg.dryRun, true);
        }

        // 0b. Immediate manual middle-click Recycle Bin trigger
        if (g_manualRecycleRequested.exchange(false, std::memory_order_acq_rel)) {
            Wh_Log(L"AutoClean middle-click Recycle Bin trigger activated");
            RecycleCleanResult rbRes = RunRecycleBinClean(cfg, L"MiddleClick", true);
            lastRecycleCleanTime = GetTickCount64();
            NotifyRecycleToast(rbRes, true);
        }

        // Gate all automatic background triggers until the user has passed the Windows logon screen
        // and also pause automatic background loops once single-shot Dry-Run has executed.
        bool desktopReady = IsInteractiveUserDesktopReady();
        now = GetTickCount64();
        if (desktopReady && loginDesktopReadyTick == 0) {
            loginDesktopReadyTick = now;
            lastTimerClean = now;
            lastRecycleTimerClean = now;
            Wh_Log(L"AutoClean: user passed Windows logon gate; lazy startup will fire in %llu ms",
                   static_cast<unsigned long long>(cfg.startupDelayMs));
        }

        const wchar_t* deferReason = nullptr;
        bool allowAutoRuns = desktopReady && (!cfg.dryRun || !dryRunCompletedOnce);

        // 1. Lazy Post-Logon Startup cleanup trigger (waits startupDelayMs AFTER interactive desktop login)
        if (allowAutoRuns && (startupTempPending || startupRecyclePending) &&
            loginDesktopReadyTick != 0 && (now - loginDesktopReadyTick >= cfg.startupDelayMs)) {
            if (!ShouldDeferCleanup(cfg, false, &deferReason)) {
                if (startupTempPending) {
                    Wh_Log(L"AutoClean lazy post-login startup Temp cleanup triggered (fullSweep=%d)",
                           cfg.startupFullSweep ? 1 : 0);
                    CleanStats stats = RunCleanupPass(cfg, L"Startup", cfg.startupFullSweep);
                    startupTempPending = false;
                    lastCleanTime = GetTickCount64();
                    lastTimerClean = lastCleanTime;
                    lastTempTimerFt100Ns = GetCurrentSystemFileTime100Ns();
                    SavePersistedFileTime100Ns(L"schedule.lastTempTimerFt", lastTempTimerFt100Ns);
                    watchDirty = false;
                    NotifyTempToast(stats, cfg.dryRun, false);
                }
                if (startupRecyclePending) {
                    Wh_Log(L"AutoClean lazy post-login startup Recycle Bin cleanup triggered");
                    // Respect 30-day retention on startup unless user explicitly enabled Recycle Bin Full Sweep
                    bool forceRbStartupSweep = cfg.startupFullSweep && cfg.rbFullSweep;
                    RecycleCleanResult rbRes = RunRecycleBinClean(
                        cfg, forceRbStartupSweep ? L"Startup" : L"RecycleAge30d", forceRbStartupSweep);
                    startupRecyclePending = false;
                    lastRecycleCleanTime = GetTickCount64();
                    lastRecycleTimerClean = lastRecycleCleanTime;
                    NotifyRecycleToast(rbRes, false);
                }
                deferredPending = false;
            } else {
                deferredPending = true;
            }
        }

        // 2. Low-disk emergency floor check
        now = GetTickCount64();
        if (allowAutoRuns && cfg.emergencyEnabled &&
            (lastEmergencyCheck == 0 || (now - lastEmergencyCheck >= cfg.emergencyCheckMs))) {
            lastEmergencyCheck = now;
            uint64_t freeBytes = 0;
            if (IsDiskSpaceBelowFloor(cfg, &freeBytes)) {
                if (!ShouldDeferCleanup(cfg, true, &deferReason)) {
                    Wh_Log(L"AutoClean emergency floor triggered: %llu < %llu free bytes",
                           static_cast<unsigned long long>(freeBytes),
                           static_cast<unsigned long long>(cfg.emergencyFloorBytes));
                    CleanStats stats = RunCleanupPass(cfg, L"EmergencyFloor", true);
                    NotifyTempToast(stats, cfg.dryRun, false);
                    if (cfg.cleanRecycleOnEmergency) {
                        RecycleCleanResult rbRes = RunRecycleBinClean(cfg, L"EmergencyFloor", true);
                        lastRecycleCleanTime = GetTickCount64();
                        NotifyRecycleToast(rbRes, false);
                    }
                    lastCleanTime = GetTickCount64();
                    watchDirty = false;
                    deferredPending = false;
                } else {
                    deferredPending = true;
                }
            }
        }

        // 3. Temp periodic timer trigger (tick + persisted FILETIME across sleep/reboots)
        now = GetTickCount64();
        nowFt100Ns = GetCurrentSystemFileTime100Ns();
        uint64_t tempTimerFtDeltaMs = (nowFt100Ns > lastTempTimerFt100Ns)
            ? ((nowFt100Ns - lastTempTimerFt100Ns) / 10000ULL)
            : 0ULL;
        if (allowAutoRuns && cfg.timerEnabled &&
            ((now - lastTimerClean >= cfg.timerIntervalMs) || (tempTimerFtDeltaMs >= cfg.timerIntervalMs))) {
            if (!ShouldDeferCleanup(cfg, false, &deferReason)) {
                Wh_Log(L"AutoClean scheduled Temp timer triggered (interval=%llu ms)",
                       static_cast<unsigned long long>(cfg.timerIntervalMs));
                CleanStats stats = RunCleanupPass(cfg, L"Timer", cfg.fullSweep);
                lastTimerClean = GetTickCount64();
                lastCleanTime = lastTimerClean;
                lastTempTimerFt100Ns = GetCurrentSystemFileTime100Ns();
                SavePersistedFileTime100Ns(L"schedule.lastTempTimerFt", lastTempTimerFt100Ns);
                watchDirty = false;
                deferredPending = false;
                NotifyTempToast(stats, cfg.dryRun, false);
            } else {
                deferredPending = true;
            }
        }

        // 4. Recycle Bin periodic timer trigger (tick + persisted FILETIME across reboots, default 30 Days)
        now = GetTickCount64();
        nowFt100Ns = GetCurrentSystemFileTime100Ns();
        uint64_t rbTimerFtDeltaMs = (nowFt100Ns > lastRecycleTimerFt100Ns)
            ? ((nowFt100Ns - lastRecycleTimerFt100Ns) / 10000ULL)
            : 0ULL;
        if (allowAutoRuns && cfg.rbTimerEnabled &&
            ((now - lastRecycleTimerClean >= cfg.rbTimerIntervalMs) || (rbTimerFtDeltaMs >= cfg.rbTimerIntervalMs))) {
            if (!ShouldDeferCleanup(cfg, false, &deferReason)) {
                Wh_Log(L"AutoClean scheduled Recycle Bin timer triggered (interval=%llu ms)",
                       static_cast<unsigned long long>(cfg.rbTimerIntervalMs));
                RecycleCleanResult rbRes = RunRecycleBinClean(cfg, L"RecycleTimer", cfg.rbFullSweep);
                lastRecycleTimerClean = GetTickCount64();
                lastRecycleCleanTime = lastRecycleTimerClean;
                lastRecycleTimerFt100Ns = GetCurrentSystemFileTime100Ns();
                SavePersistedFileTime100Ns(L"schedule.lastRecycleTimerFt", lastRecycleTimerFt100Ns);
                deferredPending = false;
                NotifyRecycleToast(rbRes, false);
            } else {
                deferredPending = true;
            }
        }

        // 5. Recycle Bin size, item-count, OR 30-day expired item check
        now = GetTickCount64();
        bool rbThresholdsActive = cfg.rbSizeThresholdEnabled || cfg.rbCountThresholdEnabled || cfg.rbTimerEnabled;
        if (allowAutoRuns && rbThresholdsActive &&
            (lastRecycleCheck == 0 || (now - lastRecycleCheck >= cfg.rbCheckMs)) &&
            (lastRecycleCleanTime == 0 || (now - lastRecycleCleanTime >= cfg.rbCooldownMs))) {
            lastRecycleCheck = now;
            uint64_t rbBytes = 0;
            uint64_t rbItems = 0;
            if (QueryRecycleBinStats(&rbBytes, &rbItems) && rbItems > 0) {
                bool sizeHit = cfg.rbSizeThresholdEnabled && (rbBytes >= cfg.rbSizeThresholdBytes);
                bool countHit = cfg.rbCountThresholdEnabled && (rbItems >= cfg.rbMaxItems);
                if (sizeHit || countHit) {
                    if (!ShouldDeferCleanup(cfg, false, &deferReason)) {
                        Wh_Log(
                            L"AutoClean Recycle Bin %s reached: %llu bytes, %llu items",
                            sizeHit ? L"size threshold" : L"item-count threshold",
                            static_cast<unsigned long long>(rbBytes),
                            static_cast<unsigned long long>(rbItems));
                        RecycleCleanResult rbRes = RunRecycleBinClean(
                            cfg, sizeHit ? L"RecycleSizeThreshold" : L"RecycleCountThreshold", cfg.rbFullSweep);
                        lastRecycleCleanTime = GetTickCount64();
                        deferredPending = false;
                        NotifyRecycleToast(rbRes, false);
                    } else {
                        deferredPending = true;
                    }
                } else if (cfg.rbTimerEnabled && !ShouldDeferCleanup(cfg, false, &deferReason)) {
                    // Auto-prune any individual Recycle Bin items that have been in the bin longer than 30d/schedule
                    RecycleCleanResult ageRes = RunRecycleBinClean(cfg, L"RecycleAge30d", false);
                    if (ageRes.itemsCleaned > 0) {
                        lastRecycleCleanTime = GetTickCount64();
                        NotifyRecycleToast(ageRes, false);
                    }
                }
            }
        }

        // 6. Event-driven Temp threshold trigger (runs only after writes + debounce)
        now = GetTickCount64();
        if (allowAutoRuns && cfg.thresholdEnabled && (watchDirty || deferredPending) &&
            (lastWriteEventTime == 0 || (now - lastWriteEventTime >= cfg.debounceMs)) &&
            (lastCleanTime == 0 || (now - lastCleanTime >= cfg.cooldownMs))) {
            if (!ShouldDeferCleanup(cfg, false, &deferReason)) {
                BOOL bgEntered = SetThreadPriority(GetCurrentThread(), THREAD_MODE_BACKGROUND_BEGIN);
                uint64_t totalSize = 0;
                for (const auto& dir : GetTargetTempDirs(cfg)) {
                    totalSize += MeasureDirectoryBytes(ToExtendedPath(dir), cfg);
                }
                if (bgEntered) {
                    SetThreadPriority(GetCurrentThread(), THREAD_MODE_BACKGROUND_END);
                }

                watchDirty = false;
                deferredPending = false;
                if (totalSize >= cfg.thresholdBytes) {
                    Wh_Log(L"AutoClean threshold reached: %llu >= %llu bytes",
                           static_cast<unsigned long long>(totalSize),
                           static_cast<unsigned long long>(cfg.thresholdBytes));
                    CleanStats stats = RunCleanupPass(cfg, L"Threshold", cfg.fullSweep);
                    lastCleanTime = GetTickCount64();
                    NotifyTempToast(stats, cfg.dryRun, false);
                }
            } else {
                deferredPending = true;
                lastWriteEventTime = now;
            }
        }

        // Compute exact sleep duration until next due event (0% CPU at idle)
        now = GetTickCount64();
        uint64_t waitMs = INFINITE;
        auto minWait = [&](uint64_t candidate) {
            if (waitMs == INFINITE || candidate < waitMs) {
                waitMs = candidate;
            }
        };

        if (!desktopReady) {
            // Poll lightly every 2s until the user completes Windows login
            minWait(2000ULL);
        } else if (allowAutoRuns) {
            if (deferredPending) {
                minWait(15000ULL);
            } else {
                if ((startupTempPending || startupRecyclePending) && loginDesktopReadyTick != 0) {
                    uint64_t elapsed = now - loginDesktopReadyTick;
                    minWait(elapsed < cfg.startupDelayMs ? (cfg.startupDelayMs - elapsed) : 1000ULL);
                }
                if (cfg.thresholdEnabled && watchDirty) {
                    uint64_t debounceRem = (lastWriteEventTime != 0 && now - lastWriteEventTime < cfg.debounceMs)
                        ? (cfg.debounceMs - (now - lastWriteEventTime))
                        : 0;
                    uint64_t coolRem = (lastCleanTime != 0 && now - lastCleanTime < cfg.cooldownMs)
                        ? (cfg.cooldownMs - (now - lastCleanTime))
                        : 0;
                    minWait((std::max)(debounceRem, coolRem));
                }
                if (cfg.timerEnabled) {
                    uint64_t elapsed = now - lastTimerClean;
                    minWait(elapsed < cfg.timerIntervalMs ? (cfg.timerIntervalMs - elapsed) : 1000ULL);
                }
                if (cfg.rbTimerEnabled) {
                    uint64_t elapsed = now - lastRecycleTimerClean;
                    minWait(elapsed < cfg.rbTimerIntervalMs ? (cfg.rbTimerIntervalMs - elapsed) : 1000ULL);
                }
                if (rbThresholdsActive) {
                    uint64_t elapsed = (lastRecycleCheck == 0) ? cfg.rbCheckMs : (now - lastRecycleCheck);
                    minWait(elapsed < cfg.rbCheckMs ? (cfg.rbCheckMs - elapsed) : 1000ULL);
                }
                if (cfg.emergencyEnabled) {
                    uint64_t elapsed = (lastEmergencyCheck == 0) ? cfg.emergencyCheckMs : (now - lastEmergencyCheck);
                    minWait(elapsed < cfg.emergencyCheckMs ? (cfg.emergencyCheckMs - elapsed) : 1000ULL);
                }
            }
        }
        if (waitMs == 0) {
            waitMs = 1000ULL;
        }

        std::vector<HANDLE> waitHandles;
        waitHandles.push_back(g_wakeEvent);
        for (DirWatchHandle* w : watches) {
            waitHandles.push_back(w->eventHandle);
        }

        DWORD waitParam = (waitMs == INFINITE)
            ? INFINITE
            : static_cast<DWORD>((std::min)(waitMs, static_cast<uint64_t>(0x7FFFFFFFULL)));

        DWORD res = WaitForMultipleObjects(
            static_cast<DWORD>(waitHandles.size()),
            waitHandles.data(),
            FALSE,
            waitParam);

        if (res >= WAIT_OBJECT_0 + 1 && res < WAIT_OBJECT_0 + waitHandles.size()) {
            size_t idx = static_cast<size_t>(res - (WAIT_OBJECT_0 + 1));
            DirWatchHandle* w = watches[idx];
            DWORD bytesTransferred = 0;
            GetOverlappedResult(w->dirHandle, &w->overlapped, &bytesTransferred, FALSE);
            ArmDirWatch(*w);
            watchDirty = true;
            lastWriteEventTime = GetTickCount64();
        }
    }

    CloseDirWatches(watches);
    if (SUCCEEDED(hrCom)) {
        CoUninitialize();
    }
    return 0;
}

static void LoadSettings() {
    ModSettings next;

    // Guard against Windhawk's Wh_GetIntSetting returning 0 for unsaved registry keys
    bool tempGroupSaved = (Wh_GetIntSetting(L"tempCleaner.thresholdValue") > 0);
    bool rbGroupSaved = (Wh_GetIntSetting(L"recycleBinCleaner.sizeThresholdValue") > 0);
    bool univGroupSaved = (Wh_GetIntSetting(L"universalSafety.freeSpaceValue") > 0);

    // 1. AI Agents Temp Protection (upgrade-safe: defaults to Enabled + built-in list when unsaved)
    PCWSTR aiModeStr = Wh_GetStringSetting(L"aiAgentProtection.protectionMode");
    bool aiGroupSaved = (aiModeStr && *aiModeStr);
    next.aiAgentProtectionEnabled = !aiGroupSaved || wcscmp(aiModeStr, L"Disabled") != 0;
    Wh_FreeStringSetting(aiModeStr);

    for (int i = 0;; i++) {
        PCWSTR pat = Wh_GetStringSetting(L"aiAgentProtection.agentTempPatterns[%d]", i);
        if (!pat || !*pat) {
            if (pat) Wh_FreeStringSetting(pat);
            break;
        }
        std::wstring cleanPat = NormalizeExcludePattern(pat);
        if (!cleanPat.empty()) {
            next.aiAgentPatterns.push_back(std::move(cleanPat));
        }
        Wh_FreeStringSetting(pat);
    }
    if (next.aiAgentPatterns.empty() && !aiGroupSaved) {
        next.aiAgentPatterns = DefaultAiAgentPatterns();
    }

    // 2. Temp Folder Cleaner
    next.thresholdEnabled = tempGroupSaved ? (Wh_GetIntSetting(L"tempCleaner.thresholdEnabled") != 0) : true;
    int rawThresh = tempGroupSaved ? Wh_GetIntSetting(L"tempCleaner.thresholdValue") : 500;
    if (rawThresh < 1) rawThresh = 500;
    PCWSTR threshUnit = Wh_GetStringSetting(L"tempCleaner.thresholdUnit");
    next.thresholdBytes = MultiplyClampU64(static_cast<uint64_t>(rawThresh), UnitToMultiplier(threshUnit));
    Wh_FreeStringSetting(threshUnit);

    int debSec = tempGroupSaved ? Wh_GetIntSetting(L"tempCleaner.debounceSeconds") : 20;
    if (debSec < 5) debSec = 5;
    if (debSec > 3600) debSec = 3600;
    next.debounceMs = static_cast<uint64_t>(debSec) * 1000ULL;

    next.timerEnabled = tempGroupSaved ? (Wh_GetIntSetting(L"tempCleaner.timerEnabled") != 0) : true;
    int rawTimerVal = tempGroupSaved ? Wh_GetIntSetting(L"tempCleaner.timerValue") : 1;
    if (rawTimerVal < 1) rawTimerVal = 1;
    PCWSTR timerUnit = Wh_GetStringSetting(L"tempCleaner.timerUnit");
    uint64_t timerUnitMult = (timerUnit && *timerUnit) ? TimeUnitToMs(timerUnit) : 3600000ULL;
    uint64_t timerMs = MultiplyClampU64(static_cast<uint64_t>(rawTimerVal), timerUnitMult);
    Wh_FreeStringSetting(timerUnit);
    if (timerMs < 15000ULL) timerMs = 15000ULL;
    next.timerIntervalMs = timerMs;

    next.oldestFirst = tempGroupSaved ? (Wh_GetIntSetting(L"tempCleaner.oldestFirst") != 0) : true;
    next.fullSweep = tempGroupSaved ? (Wh_GetIntSetting(L"tempCleaner.fullSweep") != 0) : false;
    int targetPct = tempGroupSaved ? Wh_GetIntSetting(L"tempCleaner.targetPercent") : 50;
    if (targetPct < 10) targetPct = 10;
    if (targetPct > 90) targetPct = 90;
    next.targetPercent = targetPct;

    PCWSTR tempGestureStr = Wh_GetStringSetting(L"tempCleaner.mouseGesture");
    next.tempGestureEnabled = !tempGestureStr || !*tempGestureStr || wcscmp(tempGestureStr, L"Disabled") != 0;
    Wh_FreeStringSetting(tempGestureStr);

    PCWSTR tempScopeStr = Wh_GetStringSetting(L"tempCleaner.mouseScope");
    next.tempGestureScope = ParseTempMouseScope(tempScopeStr);
    Wh_FreeStringSetting(tempScopeStr);

    int minAgeMin = tempGroupSaved ? Wh_GetIntSetting(L"tempCleaner.minFileAgeMinutes") : 10;
    if (minAgeMin < 0) minAgeMin = 0;
    if (minAgeMin > 10080) minAgeMin = 10080;
    next.minFileAgeMs = static_cast<uint64_t>(minAgeMin) * 60000ULL;

    int cooldownSec = tempGroupSaved ? Wh_GetIntSetting(L"tempCleaner.cooldownSeconds") : 30;
    if (cooldownSec < 5) cooldownSec = 5;
    if (cooldownSec > 3600) cooldownSec = 3600;
    next.cooldownMs = static_cast<uint64_t>(cooldownSec) * 1000ULL;

    next.cleanWindowsTemp = tempGroupSaved ? (Wh_GetIntSetting(L"tempCleaner.cleanWindowsTemp") != 0) : false;

    for (int i = 0;; i++) {
        PCWSTR pat = Wh_GetStringSetting(L"tempCleaner.excludePatterns[%d]", i);
        if (!pat || !*pat) {
            if (pat) Wh_FreeStringSetting(pat);
            break;
        }
        std::wstring cleanPat = NormalizeExcludePattern(pat);
        if (!cleanPat.empty()) {
            next.excludePatterns.push_back(std::move(cleanPat));
        }
        Wh_FreeStringSetting(pat);
    }
    if (next.excludePatterns.empty() && !tempGroupSaved) {
        next.excludePatterns = {L"*.lock", L"*.lck", L"windhawk*"};
    }

    // 2. Recycle Bin Cleaner (default: 30-day schedule & retention active)
    next.rbSizeThresholdEnabled = rbGroupSaved ? (Wh_GetIntSetting(L"recycleBinCleaner.sizeThresholdEnabled") != 0) : true;
    int rawRbThresh = rbGroupSaved ? Wh_GetIntSetting(L"recycleBinCleaner.sizeThresholdValue") : 500;
    if (rawRbThresh < 1) rawRbThresh = 500;
    PCWSTR rbUnit = Wh_GetStringSetting(L"recycleBinCleaner.sizeThresholdUnit");
    next.rbSizeThresholdBytes = MultiplyClampU64(static_cast<uint64_t>(rawRbThresh), UnitToMultiplier(rbUnit));
    Wh_FreeStringSetting(rbUnit);

    next.rbCountThresholdEnabled = rbGroupSaved ? (Wh_GetIntSetting(L"recycleBinCleaner.countThresholdEnabled") != 0) : false;
    int rawRbMaxItems = rbGroupSaved ? Wh_GetIntSetting(L"recycleBinCleaner.maxItems") : 50;
    if (rawRbMaxItems < 1) rawRbMaxItems = 50;
    next.rbMaxItems = static_cast<uint64_t>(rawRbMaxItems);

    next.rbTimerEnabled = rbGroupSaved ? (Wh_GetIntSetting(L"recycleBinCleaner.timerEnabled") != 0) : true;
    int rawRbTimerVal = rbGroupSaved ? Wh_GetIntSetting(L"recycleBinCleaner.timerValue") : 30;
    if (rawRbTimerVal < 1) rawRbTimerVal = 30;
    PCWSTR rbTimerUnit = Wh_GetStringSetting(L"recycleBinCleaner.timerUnit");
    uint64_t rbUnitMult = (rbTimerUnit && *rbTimerUnit) ? TimeUnitToMs(rbTimerUnit) : (86400ULL * 1000ULL);
    uint64_t rbTimerMs = MultiplyClampU64(static_cast<uint64_t>(rawRbTimerVal), rbUnitMult);
    Wh_FreeStringSetting(rbTimerUnit);
    if (rbTimerMs < 15000ULL) rbTimerMs = 15000ULL;
    next.rbTimerIntervalMs = rbTimerMs;

    next.rbFullSweep = rbGroupSaved ? (Wh_GetIntSetting(L"recycleBinCleaner.fullSweep") != 0) : false;
    next.rbOldestFirst = rbGroupSaved ? (Wh_GetIntSetting(L"recycleBinCleaner.oldestFirst") != 0) : true;
    int rbTargetPct = rbGroupSaved ? Wh_GetIntSetting(L"recycleBinCleaner.targetPercent") : 50;
    if (rbTargetPct < 10) rbTargetPct = 10;
    if (rbTargetPct > 90) rbTargetPct = 90;
    next.rbTargetPercent = rbTargetPct;

    int rbMinAgeMin = rbGroupSaved ? Wh_GetIntSetting(L"recycleBinCleaner.minItemAgeMinutes") : 0;
    if (rbMinAgeMin < 0) rbMinAgeMin = 0;
    if (rbMinAgeMin > 525600) rbMinAgeMin = 525600;
    next.rbMinAgeMs = static_cast<uint64_t>(rbMinAgeMin) * 60000ULL;

    int rbCheckSec = rbGroupSaved ? Wh_GetIntSetting(L"recycleBinCleaner.checkIntervalSeconds") : 30;
    if (rbCheckSec < 15) rbCheckSec = 15;
    if (rbCheckSec > 3600) rbCheckSec = 3600;
    next.rbCheckMs = static_cast<uint64_t>(rbCheckSec) * 1000ULL;

    int rbCoolSec = rbGroupSaved ? Wh_GetIntSetting(L"recycleBinCleaner.cooldownSeconds") : 15;
    if (rbCoolSec < 5) rbCoolSec = 5;
    if (rbCoolSec > 3600) rbCoolSec = 3600;
    next.rbCooldownMs = static_cast<uint64_t>(rbCoolSec) * 1000ULL;

    PCWSTR gestureStr = Wh_GetStringSetting(L"recycleBinCleaner.mouseGesture");
    next.rbMouseClicksRequired = (gestureStr && *gestureStr) ? ParseMouseGestureClicks(gestureStr) : 2;
    Wh_FreeStringSetting(gestureStr);

    PCWSTR scopeStr = Wh_GetStringSetting(L"recycleBinCleaner.mouseScope");
    next.rbMouseScope = ParseMouseScope(scopeStr);
    Wh_FreeStringSetting(scopeStr);

    next.rbMouseSwallowClick = rbGroupSaved ? (Wh_GetIntSetting(L"recycleBinCleaner.mouseSwallowTriggerClick") != 0) : true;
    next.rbPlaySound = rbGroupSaved ? (Wh_GetIntSetting(L"recycleBinCleaner.playSound") != 0) : false;

    // 3. Universal, Toast & Safety
    next.startupCleanTemp = univGroupSaved ? (Wh_GetIntSetting(L"universalSafety.cleanTempOnStart") != 0) : true;
    next.startupCleanRecycle = univGroupSaved ? (Wh_GetIntSetting(L"universalSafety.cleanRecycleBinOnStart") != 0) : true;
    int startDelay = univGroupSaved ? Wh_GetIntSetting(L"universalSafety.startupDelaySeconds") : 12;
    if (startDelay < 5) startDelay = 5;
    if (startDelay > 300) startDelay = 300;
    next.startupDelayMs = static_cast<uint64_t>(startDelay) * 1000ULL;
    next.startupFullSweep = univGroupSaved ? (Wh_GetIntSetting(L"universalSafety.startupFullSweep") != 0) : true;

    next.emergencyEnabled = univGroupSaved ? (Wh_GetIntSetting(L"universalSafety.emergencyEnabled") != 0) : true;
    int rawFloor = univGroupSaved ? Wh_GetIntSetting(L"universalSafety.freeSpaceValue") : 5;
    if (rawFloor < 1) rawFloor = 5;
    PCWSTR floorUnit = Wh_GetStringSetting(L"universalSafety.freeSpaceUnit");
    uint64_t floorUnitMult = (floorUnit && *floorUnit) ? UnitToMultiplier(floorUnit) : (1024ULL * 1024ULL * 1024ULL);
    next.emergencyFloorBytes = MultiplyClampU64(static_cast<uint64_t>(rawFloor), floorUnitMult);
    Wh_FreeStringSetting(floorUnit);

    int emergSec = univGroupSaved ? Wh_GetIntSetting(L"universalSafety.emergencyCheckSeconds") : 60;
    if (emergSec < 15) emergSec = 15;
    if (emergSec > 3600) emergSec = 3600;
    next.emergencyCheckMs = static_cast<uint64_t>(emergSec) * 1000ULL;

    next.cleanRecycleOnEmergency = univGroupSaved ? (Wh_GetIntSetting(L"universalSafety.cleanRecycleOnEmergency") != 0) : true;
    next.emergencyBypassDeferral = univGroupSaved ? (Wh_GetIntSetting(L"universalSafety.bypassDeferralOnEmergency") != 0) : true;
    next.deferOnFullscreen = univGroupSaved ? (Wh_GetIntSetting(L"universalSafety.deferOnFullscreen") != 0) : true;
    next.deferOnBattery = univGroupSaved ? (Wh_GetIntSetting(L"universalSafety.deferOnBattery") != 0) : true;

    int maxIntMs = univGroupSaved ? Wh_GetIntSetting(L"universalSafety.mouseMaxIntervalMs") : 400;
    if (maxIntMs < 150) maxIntMs = 150;
    if (maxIntMs > 800) maxIntMs = 800;
    next.rbMouseMaxIntervalMs = static_cast<uint64_t>(maxIntMs);
    next.tempGestureMaxIntervalMs = static_cast<uint64_t>(maxIntMs);

    next.toastEnabled = univGroupSaved ? (Wh_GetIntSetting(L"universalSafety.toastEnabled") != 0) : true;
    PCWSTR toastScopeStr = Wh_GetStringSetting(L"universalSafety.toastTriggerScope");
    next.toastShowOnAuto = !toastScopeStr || !*toastScopeStr || wcscmp(toastScopeStr, L"ManualOnly") != 0;
    Wh_FreeStringSetting(toastScopeStr);
    int toastDur = univGroupSaved ? Wh_GetIntSetting(L"universalSafety.toastDurationMs") : 3200;
    if (toastDur < 1200) toastDur = 1200;
    if (toastDur > 10000) toastDur = 10000;
    next.toastDurationMs = static_cast<uint32_t>(toastDur);

    PCWSTR capStr = Wh_GetStringSetting(L"universalSafety.screenshotBehavior");
    next.toastExcludeFromCapture = !capStr || !*capStr || wcscmp(capStr, L"ShowInCapture") != 0;
    Wh_FreeStringSetting(capStr);

    next.dryRun = Wh_GetIntSetting(L"universalSafety.dryRun") != 0;

    g_mouseClicksRequired.store(next.rbMouseClicksRequired, std::memory_order_relaxed);
    g_mouseTriggerScope.store(next.rbMouseScope, std::memory_order_relaxed);
    g_mouseMaxIntervalMs.store(static_cast<uint32_t>(next.rbMouseMaxIntervalMs), std::memory_order_relaxed);
    g_mouseSwallowClick.store(next.rbMouseSwallowClick, std::memory_order_relaxed);
    g_tempGestureEnabled.store(next.tempGestureEnabled, std::memory_order_relaxed);
    g_tempGestureScope.store(next.tempGestureScope, std::memory_order_relaxed);
    g_tempGestureMaxIntervalMs.store(static_cast<uint32_t>(next.tempGestureMaxIntervalMs), std::memory_order_relaxed);
    g_toastEnabled.store(next.toastEnabled, std::memory_order_relaxed);
    g_toastShowOnAuto.store(next.toastShowOnAuto, std::memory_order_relaxed);
    g_toastDurationMs.store(next.toastDurationMs, std::memory_order_relaxed);
    g_toastExcludeFromCapture.store(next.toastExcludeFromCapture, std::memory_order_relaxed);

    bool needMouseHook = (next.rbMouseClicksRequired > 0) || next.tempGestureEnabled;

    EnterCriticalSection(&g_settingsMtx);
    g_settings = std::move(next);
    LeaveCriticalSection(&g_settingsMtx);

    SyncMouseHookThread(needMouseHook);
}

static void ShowCleanToast(const std::wstring& title, const std::wstring& subtitle, bool isManual) {
    if (!g_toastEnabled.load(std::memory_order_relaxed)) {
        return;
    }
    if (!isManual && !g_toastShowOnAuto.load(std::memory_order_relaxed)) {
        return;
    }
    if (!g_uiMsgHwnd) {
        return;
    }
    auto* payload = new ToastPayload{
        title,
        subtitle,
        g_toastDurationMs.load(std::memory_order_relaxed),
        isManual,
    };
    if (!PostMessageW(g_uiMsgHwnd, WM_APP_SHOW_TOAST, 0, reinterpret_cast<LPARAM>(payload))) {
        delete payload;
    }
}

// ============================================================================
// Tool Mod Entry Points
// ============================================================================

BOOL WhTool_ModInit() {
    InitializeCriticalSection(&g_settingsMtx);
    g_settingsMtxInit = true;
    g_stopRequested.store(false, std::memory_order_relaxed);

    g_wakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_uiReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_wakeEvent || !g_uiReadyEvent) {
        return FALSE;
    }

    g_uiThread = CreateThread(nullptr, 0, UiThreadProc, nullptr, 0, nullptr);
    if (g_uiThread) {
        WaitForSingleObject(g_uiReadyEvent, 2000);
    }

    LoadSettings();

    g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, nullptr);
    if (!g_workerThread) {
        SyncMouseHookThread(false);
        if (g_uiMsgHwnd) {
            PostMessageW(g_uiMsgHwnd, WM_QUIT, 0, 0);
        }
        if (g_uiThread) {
            WaitForSingleObject(g_uiThread, 2000);
            CloseHandle(g_uiThread);
            g_uiThread = nullptr;
        }
        CloseHandle(g_uiReadyEvent);
        g_uiReadyEvent = nullptr;
        CloseHandle(g_wakeEvent);
        g_wakeEvent = nullptr;
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    g_settingsReconfigured.store(true, std::memory_order_release);
    if (g_wakeEvent) {
        SetEvent(g_wakeEvent);
    }
}

void WhTool_ModUninit() {
    g_stopRequested.store(true, std::memory_order_release);
    if (g_wakeEvent) {
        SetEvent(g_wakeEvent);
    }
    SyncMouseHookThread(false);
    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, 5000);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }
    if (g_uiMsgHwnd) {
        PostMessageW(g_uiMsgHwnd, WM_QUIT, 0, 0);
    }
    if (g_uiThread) {
        WaitForSingleObject(g_uiThread, 3000);
        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
    }
    if (g_uiReadyEvent) {
        CloseHandle(g_uiReadyEvent);
        g_uiReadyEvent = nullptr;
    }
    if (g_wakeEvent) {
        CloseHandle(g_wakeEvent);
        g_wakeEvent = nullptr;
    }
    if (g_settingsMtxInit) {
        DeleteCriticalSection(&g_settingsMtx);
        g_settingsMtxInit = false;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    // Keep main thread alive at 0% CPU so g_toolModProcessMutex is never abandoned (WAIT_ABANDONED)
    Sleep(INFINITE);
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

    // Windhawk 2.0 native host (windhawk-mod.exe) does not pass "-tool-mod <id>" on the command line.
    WCHAR modulePath[MAX_PATH]{};
    if (!isCurrentToolModProcess && GetModuleFileNameW(nullptr, modulePath, ARRAYSIZE(modulePath)) > 0) {
        PCWSTR baseName = PathFindFileNameW(modulePath);
        if (baseName && (
            _wcsicmp(baseName, L"windhawk-mod.exe") == 0 ||
            _wcsicmp(baseName, L"windhawk-mod-uiaccess.exe") == 0 ||
            _wcsicmp(baseName, L"windhawk-mod-elevated.exe") == 0)) {
            isToolModProcess = true;
            isCurrentToolModProcess = true;
        }
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex = CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }
        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            DWORD waitRes = WaitForSingleObject(g_toolModProcessMutex, 5000);
            if (waitRes != WAIT_OBJECT_0) {
                Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
                ExitProcess(1);
            }
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

    WCHAR commandLine[MAX_PATH + 2 + (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
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
        HANDLE hUserToken,
        LPCWSTR lpApplicationName,
        LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes,
        WINBOOL bInheritHandles,
        DWORD dwCreationFlags,
        LPVOID lpEnvironment,
        LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);

    auto pCreateProcessInternalW =
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
    if (!pCreateProcessInternalW(
            nullptr, currentProcessPath, commandLine, nullptr, nullptr, FALSE,
            NORMAL_PRIORITY_CLASS, nullptr, nullptr, &si, &pi, nullptr)) {
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
    if (g_toolModProcessMutex) {
        ReleaseMutex(g_toolModProcessMutex);
        CloseHandle(g_toolModProcessMutex);
        g_toolModProcessMutex = nullptr;
    }
    WhTool_ModUninit();
    ExitProcess(0);
}
