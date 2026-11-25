// ==WindhawkMod==
// @id              autoclean
// @name            AutoClean
// @description     Event-driven background Temp folder and Recycle Bin cleaner with AI agent protection, oldest-first pruning, mouse shortcuts, and OLED toasts
// @version         2.2026.2211
// @author          Ashix
// @github          https://github.com/k-ashix
// @twitter         https://x.com/k_ashix
// @donateUrl       https://ko-fi.com/kashix
// @include         windhawk.exe
// @compilerOptions -ladvapi32 -lole32 -loleaut32 -lshell32 -lshlwapi -lshcore -luser32 -lgdi32 -lgdiplus -ldwmapi -lwtsapi32 -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# AutoClean

Silent, event-driven background Temp folder and Recycle Bin cleaner running in a dedicated Windhawk Tool Mod process.

![AutoClean OLED Toast](https://raw.githubusercontent.com/k-ashix/ashix-windhawk-mods/main/Auto_Clean/Auto_Clean.png)

## Action Triggers — Temp & Recycle Bin
| Icon | Shortcut / Trigger | Action |
|---|---|---|
| 🖱️ | `3x Left-Click` (`Desktop & Taskbar empty space`) | Clean Temp folders immediately and show OLED corner toast |
| 🖱️ | `Middle-Click` (`1x`/`2x`/`3x`, `Desktop & Taskbar empty space`) | Empty Recycle Bin immediately and show OLED corner toast |
| 🚀 | `Windows Login / Startup` | Clean Temp folders (and optionally Recycle Bin) automatically after login |
| 📦 | `Size Limit` (`KB`/`MB`/`GB`/`TB`) | Prune oldest items first or full sweep when Temp or Recycle Bin hits limit |
| 🔢 | `File Count Limit` (`X items`) | Prune oldest-deleted items or empty Recycle Bin at `X` items |
| ⏱️ | `Timer / Retention` | Clean Temp on a repeating schedule and expire Recycle Bin items older than your retention window |
| ⚠️ | `Low Disk Space Emergency` | Free Temp and oldest Recycle Bin items on the low-space drive when free space crosses below the floor |

## How AutoClean Differs from Windows Storage Sense
Windows Storage Sense covers basic periodic Temp and Recycle Bin cleanup, whereas AutoClean adds:
- **True Event-Driven Watching**: Uses `ReadDirectoryChangesW` for `%TEMP%` and `SHChangeNotifyRegister` on the Recycle Bin so idle CPU and disk usage stay at zero (secondary HDDs can spin down normally).
- **AI Agent `%TEMP%` Protection**: Preserves local `%TEMP%` scratchpads and worktrees created by AI coding & desktop agents (Claude Code, Codex, Hermes, OpenClaw, Gemini CLI, OpenCode, Cursor, Windsurf, Goose, Aider, Copilot, MCP) by default.
- **Instant Empty-Space Mouse Shortcuts**: Triple left-click empty Desktop/Taskbar space to clean Temp, or middle-click empty space to empty the Recycle Bin, with UI Automation checks so taskbar buttons and desktop icons are never triggered.
- **Granular Controls & Smart Deferral**: Exact `KB`/`MB`/`GB`/`TB` size caps, oldest-first pruning to a target percentage, wildcard exclusions, fullscreen/battery deferral, single-shot Dry-Run simulation, and borderless OLED corner toasts.

## Safety Guards
- **Temp Path Validation**: Refuses to clean drive roots, `%windir%`, `%USERPROFILE%`, or known user folders (Desktop, Documents, Downloads, Pictures, Music, Videos, AppData) if `%TEMP%` is misconfigured.
- **Minimum Age Grace Periods**: Honored on all Temp sweeps (default 24 hours / 1440 minutes) and Recycle Bin pruning (default 24 hours / 1440 minutes) so active installers, build tools, PyInstaller payloads, and recent deletions stay protected.
- **Reparse-Point & Attribute Safety**: Skips junctions/symlinks, respects Windows file locks, and restores Read-Only/Hidden/System attributes if a locked file cannot be deleted.
- **Safe Recycle Bin Defaults**: Automatic Recycle Bin size/count thresholds and startup cleaning are off by default; only 30-day age expiry is enabled out of the box.
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
    $description: Wait time after Temp writes before measuring size (5 to 3600 seconds; capped under continuous writes).
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
  - fullSweep: false
    $name: Always perform full sweep
    $description: Delete all eligible Temp files (still honoring minimum file age) instead of pruning oldest-first down to the target percentage.
  - targetPercent: 50
    $name: Target percentage to keep (%)
    $description: When Full Sweep is off, prune oldest-first once Temp exceeds this percentage of the size limit (10 to 90%).
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
  - minFileAgeMinutes: 1440
    $name: Minimum file age (minutes)
    $description: Skip Temp files and empty folders modified more recently than this grace period on all passes (default 1440 = 24 hours; 0 disables).
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
  - sizeThresholdEnabled: false
    $name: Enable Recycle Bin size threshold
    $description: Automatically prune the Recycle Bin when its total size reaches the limit (off by default; if recent items within the grace period alone exceed the target, all older items are removed).
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
    $description: Automatically prune the Recycle Bin when deleted item count reaches the limit (off by default).
  - maxItems: 50
    $name: Maximum items in Recycle Bin
    $description: Clean when the Recycle Bin holds at least this many items (minimum 1).
  - retentionEnabled: true
    $name: Enable Recycle Bin age expiry
    $description: Automatically delete individual Recycle Bin items once they have been in the bin longer than the retention age (default 30 Days).
  - retentionValue: 30
    $name: Recycle Bin item retention age
    $description: Delete individual items older than this age (default 30 Days).
  - retentionUnit: Days
    $name: Recycle Bin retention unit
    $options:
    - Minutes: Minutes
    - Hours: Hours
    - Days: Days
  - fullSweep: false
    $name: Empty entire Recycle Bin on threshold (Full Sweep)
    $description: When a size or count threshold triggers, permanently empty the entire Recycle Bin instead of pruning oldest-first down to the target %.
  - targetPercent: 50
    $name: Target percentage to keep (%)
    $description: Stop pruning oldest items once total Recycle Bin size/count drops to this percentage of the triggered limit (10 to 90%; protected recent items still count toward total size/count).
  - minItemAgeMinutes: 1440
    $name: Minimum deleted item age (minutes)
    $description: Keep items deleted more recently than this grace period during oldest-first and emergency pruning (default 1440 = 24 hours; 0 disables).
  - cooldownSeconds: 15
    $name: Post-clean cooldown (seconds)
    $description: Minimum wait between automatic Recycle Bin cleanups (5 to 3600 seconds).
  - mouseGesture: DoubleMiddleClick
    $name: Middle-click shortcut to empty Recycle Bin
    $description: Click the middle mouse button (scroll wheel) on empty Desktop or Taskbar space to empty the Recycle Bin immediately.
    $options:
    - Disabled: Off (Disabled)
    - SingleMiddleClick: Single Middle-Click (1x)
    - DoubleMiddleClick: Double Middle-Click (2x - Recommended)
    - TripleMiddleClick: Triple Middle-Click (3x)
  - mouseScope: DesktopAndTaskbar
    $name: Where Recycle Bin middle-click works
    $description: Empty Desktop & Taskbar space prevents accidental triggers on taskbar buttons, icons, browsers, or 3D/CAD apps.
    $options:
    - DesktopAndTaskbar: Desktop & Taskbar empty space only (Recommended)
    - Anywhere: Anywhere on screen (except fullscreen games)
  - playSound: false
    $name: Play Windows sound when emptied
    $description: Play the standard Windows Recycle Bin sound on full empty.
  $name: 3. Recycle Bin Cleaner
- universalSafety:
  - cleanTempOnStart: true
    $name: Clean Temp folders on startup
    $description: Automatically clean Temp folders after Windows login or mod load (honors minimum file age).
  - cleanRecycleBinOnStart: false
    $name: Clean Recycle Bin on startup
    $description: Automatically clean the Recycle Bin after Windows login or mod load (off by default).
  - startupDelaySeconds: 12
    $name: Lazy post-login delay (seconds)
    $description: Wait time after interactive Windows desktop login before running startup cleanups (5 to 300 seconds).
  - startupFullSweep: true
    $name: Full sweep on startup
    $description: Clean all eligible Temp files (still honoring minimum file age) on startup instead of keeping a target percentage.
  - emergencyEnabled: true
    $name: Enable low-disk emergency floor
    $description: Trigger a cleanup once when free space on the Temp drive crosses below the floor (re-arms after recovering above the floor).
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
  - cleanRecycleOnEmergency: false
    $name: Also prune Recycle Bin on low-disk emergency
    $description: Prune oldest eligible Recycle Bin items on the low-space drive (honoring minimum item age) when free space crosses below the floor (off by default).
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

#include <windhawk_utils.h>

#include <windows.h>
#include <objbase.h>
#include <sddl.h>
#include <shlobj.h>
#include <knownfolders.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <shellscalingapi.h>
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

    bool fullSweep = false;
    int targetPercent = 50;

    bool emergencyEnabled = true;
    uint64_t emergencyFloorBytes = 5ULL * 1024ULL * 1024ULL * 1024ULL;
    uint64_t emergencyCheckMs = 60000ULL;
    bool emergencyBypassDeferral = true;

    bool deferOnFullscreen = true;
    bool deferOnBattery = true;

    uint64_t minFileAgeMs = 1440ULL * 60000ULL;
    uint64_t cooldownMs = 30000ULL;
    bool cleanWindowsTemp = false;
    bool dryRun = false;
    bool aiAgentProtectionEnabled = true;
    std::vector<std::wstring> aiAgentPatterns;
    std::vector<std::wstring> excludePatterns;

    // Startup & Windows Login
    bool startupCleanTemp = true;
    bool startupCleanRecycle = false;
    uint64_t startupDelayMs = 12000ULL;
    bool startupFullSweep = true;

    // Recycle Bin Cleaner
    bool rbSizeThresholdEnabled = false;
    uint64_t rbSizeThresholdBytes = 500ULL * 1024ULL * 1024ULL;
    bool rbCountThresholdEnabled = false;
    uint64_t rbMaxItems = 50ULL;
    bool rbRetentionEnabled = true;
    uint64_t rbRetentionAgeMs = 30ULL * 86400ULL * 1000ULL;
    bool rbFullSweep = false;
    int rbTargetPercent = 50;
    uint64_t rbMinAgeMs = 1440ULL * 60000ULL;
    uint64_t rbCooldownMs = 15000ULL;
    int rbMouseClicksRequired = 2;
    int rbMouseScope = 0; // 0=DesktopAndTaskbar, 1=Anywhere
    uint64_t rbMouseMaxIntervalMs = 400ULL;
    bool rbPlaySound = false;
    bool cleanRecycleOnEmergency = false;

    // Temp 3x Left-Click Shortcut
    bool tempGestureEnabled = true;
    uint64_t tempGestureMaxIntervalMs = 400ULL;
    int tempGestureScope = 0; // 0=DesktopAndTaskbar, 1=TaskbarOnly, 2=DesktopOnly

    // Corner Toast Feedback & Screenshot Visibility
    bool toastEnabled = true;
    bool toastShowOnAuto = true;
    uint32_t toastDurationMs = 3200U;
    bool toastExcludeFromCapture = true;
};

static CRITICAL_SECTION g_settingsMtx;
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
static std::atomic<bool> g_tempGestureEnabled{true};
static std::atomic<int> g_tempGestureScope{0};
static std::atomic<uint32_t> g_tempGestureMaxIntervalMs{400};
static std::atomic<bool> g_toastEnabled{true};
static std::atomic<bool> g_toastShowOnAuto{true};
static std::atomic<uint32_t> g_toastDurationMs{3200};
static std::atomic<bool> g_toastExcludeFromCapture{true};

static constexpr UINT WM_APP_CHECK_TEMP_TAP = WM_APP + 1;
static constexpr UINT WM_APP_SHOW_TOAST = WM_APP + 2;
static constexpr UINT WM_APP_CHECK_RECYCLE_TAP = WM_APP + 3;
static constexpr UINT WM_APP_RECYCLE_CHANGED = WM_APP + 4;

static std::atomic<bool> g_stopRequested{false};
static std::atomic<bool> g_manualTempRequested{false};
static std::atomic<bool> g_manualRecycleRequested{false};
static std::atomic<bool> g_settingsReconfigured{false};
static std::atomic<bool> g_recycleDirty{true};
static std::atomic<uint64_t> g_ignoreRecycleNotifyUntilTick{0};

static void ShowCleanToast(const std::wstring& title, const std::wstring& subtitle, bool isManual);

// ============================================================================
// Helpers: Overflow-Safe Units, Paths, Validation, Matching, Time
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
    if (wcscmp(unit, L"KB") == 0) return 1024ULL;
    if (wcscmp(unit, L"MB") == 0) return 1024ULL * 1024ULL;
    if (wcscmp(unit, L"GB") == 0) return 1024ULL * 1024ULL * 1024ULL;
    if (wcscmp(unit, L"TB") == 0) return 1024ULL * 1024ULL * 1024ULL * 1024ULL;
    return 1024ULL * 1024ULL;
}

static int ParseMouseGestureClicks(PCWSTR val) {
    if (wcscmp(val, L"Disabled") == 0) return 0;
    if (wcscmp(val, L"SingleMiddleClick") == 0) return 1;
    if (wcscmp(val, L"DoubleMiddleClick") == 0) return 2;
    if (wcscmp(val, L"TripleMiddleClick") == 0) return 3;
    return 2;
}

static int ParseMouseScope(PCWSTR val) {
    if (wcscmp(val, L"Anywhere") == 0) return 1;
    return 0;
}

static int ParseTempMouseScope(PCWSTR val) {
    if (wcscmp(val, L"TaskbarOnly") == 0) return 1;
    if (wcscmp(val, L"DesktopOnly") == 0) return 2;
    return 0;
}

static uint64_t TimeUnitToMs(PCWSTR unit, uint64_t fallbackMs) {
    if (wcscmp(unit, L"Seconds") == 0) return 1000ULL;
    if (wcscmp(unit, L"Minutes") == 0) return 60ULL * 1000ULL;
    if (wcscmp(unit, L"Hours") == 0) return 3600ULL * 1000ULL;
    if (wcscmp(unit, L"Days") == 0) return 86400ULL * 1000ULL;
    return fallbackMs;
}

static std::wstring StripTrailingSlashes(std::wstring path) {
    while (path.size() > 3 && (path.back() == L'\\' || path.back() == L'/')) {
        path.pop_back();
    }
    return path;
}

static std::wstring CanonicalizeDirPath(const std::wstring& raw) {
    if (raw.empty()) {
        return L"";
    }
    WCHAR fullPath[MAX_PATH + 4]{};
    DWORD fullLen = GetFullPathNameW(raw.c_str(), ARRAYSIZE(fullPath), fullPath, nullptr);
    std::wstring res = (fullLen > 0 && fullLen < ARRAYSIZE(fullPath)) ? fullPath : raw;
    std::replace(res.begin(), res.end(), L'/', L'\\');
    return StripTrailingSlashes(std::move(res));
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

static bool IsSameOrParentDir(const std::wstring& candidate, const std::wstring& protectedPath) {
    if (candidate.empty() || protectedPath.empty()) {
        return false;
    }
    if (_wcsicmp(candidate.c_str(), protectedPath.c_str()) == 0) {
        return true;
    }
    if (candidate.size() < protectedPath.size() &&
        _wcsnicmp(candidate.c_str(), protectedPath.c_str(), candidate.size()) == 0 &&
        protectedPath[candidate.size()] == L'\\') {
        return true;
    }
    return false;
}

static bool IsSameOrChildDir(const std::wstring& candidate, const std::wstring& basePath) {
    return IsSameOrParentDir(basePath, candidate);
}

static bool IsSafeTempDir(const std::wstring& dir) {
    if (dir.size() < 4) {
        return false;
    }
    if (PathIsRootW(dir.c_str())) {
        return false;
    }

    WCHAR winDir[MAX_PATH]{};
    if (GetWindowsDirectoryW(winDir, ARRAYSIZE(winDir)) > 0) {
        std::wstring canonWin = CanonicalizeDirPath(winDir);
        std::wstring canonWinTemp = canonWin + L"\\Temp";
        if (IsSameOrParentDir(dir, canonWin) ||
            (IsSameOrChildDir(dir, canonWin) && _wcsicmp(dir.c_str(), canonWinTemp.c_str()) != 0)) {
            return false;
        }
    }

    WCHAR pf64[MAX_PATH]{};
    DWORD pf64Len = GetEnvironmentVariableW(L"ProgramW6432", pf64, ARRAYSIZE(pf64));
    if (pf64Len > 0 && pf64Len < ARRAYSIZE(pf64)) {
        std::wstring canonPf64 = CanonicalizeDirPath(pf64);
        if (IsSameOrParentDir(dir, canonPf64) || IsSameOrChildDir(dir, canonPf64)) {
            return false;
        }
    }

    static const KNOWNFOLDERID* const kProtectedFolders[] = {
        &FOLDERID_Profile,
        &FOLDERID_Desktop,
        &FOLDERID_Documents,
        &FOLDERID_Downloads,
        &FOLDERID_Pictures,
        &FOLDERID_Music,
        &FOLDERID_Videos,
        &FOLDERID_LocalAppData,
        &FOLDERID_RoamingAppData,
        &FOLDERID_ProgramFiles,
        &FOLDERID_ProgramFilesX86,
        &FOLDERID_ProgramData,
        &FOLDERID_SavedGames,
        &FOLDERID_UserProfiles,
    };

    for (const KNOWNFOLDERID* fid : kProtectedFolders) {
        PWSTR knownPath = nullptr;
        if (SUCCEEDED(SHGetKnownFolderPath(*fid, KF_FLAG_DONT_VERIFY, nullptr, &knownPath)) && knownPath) {
            std::wstring canonKnown = CanonicalizeDirPath(knownPath);
            CoTaskMemFree(knownPath);
            // Allow subdirectories of Profile/LocalAppData/UserProfiles (e.g. AppData\Local\Temp),
            // but refuse if Temp resolves to those folders themselves, or to/inside/above other known folders.
            if (*fid == FOLDERID_Profile || *fid == FOLDERID_LocalAppData || *fid == FOLDERID_UserProfiles) {
                if (_wcsicmp(dir.c_str(), canonKnown.c_str()) == 0) {
                    return false;
                }
            } else if (IsSameOrParentDir(dir, canonKnown) || IsSameOrChildDir(dir, canonKnown)) {
                return false;
            }
        }
    }

    return true;
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

    std::wstring resolved = CanonicalizeDirPath(rawPath);
    if (!IsSafeTempDir(resolved)) {
        Wh_Log(L"Refusing unsafe Temp folder path: %s", resolved.c_str());
        return L"";
    }
    return resolved;
}

static std::wstring ResolveWindowsTempDir() {
    WCHAR winDir[MAX_PATH]{};
    UINT len = GetWindowsDirectoryW(winDir, ARRAYSIZE(winDir));
    if (len == 0 || len >= ARRAYSIZE(winDir)) {
        return L"";
    }
    std::wstring path = CanonicalizeDirPath(winDir);
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
    std::wstring s(raw);
    while (!s.empty() && (s.front() == L' ' || s.front() == L'\t' || s.front() == L'\\' || s.front() == L'/')) {
        s.erase(s.begin());
    }
    while (!s.empty() && (s.back() == L' ' || s.back() == L'\t' || s.back() == L'\\' || s.back() == L'/')) {
        s.pop_back();
    }
    return s;
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

// Verifies the user is on the interactive "Default" desktop (past the Windows logon/lock screen)
// without requiring a visible Shell_TrayWnd, so alternative shells and hidden taskbars work too.
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
    return nameOk && _wcsicmp(deskName, L"Default") == 0;
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

static bool IsDiskSpaceBelowFloor(const ModSettings& cfg, uint64_t* outFreeBytes, wchar_t* outDriveLetter) {
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
    if (outDriveLetter && userTemp.size() >= 2 && userTemp[1] == L':') {
        *outDriveLetter = static_cast<wchar_t>(towupper(userTemp[0]));
    }
    return freeBytesAvail.QuadPart < cfg.emergencyFloorBytes;
}

// ============================================================================
// UI Thread (UIA Empty-Space Check, Recycle Bin Watcher & Toast Host)
// ============================================================================

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

static bool IsPointOnDesktopWindow(POINT pt) {
    HWND hWnd = WindowFromPoint(pt);
    if (!hWnd) return false;
    HWND hRoot = GetAncestor(hWnd, GA_ROOT);
    if (!hRoot) hRoot = hWnd;
    WCHAR rootCls[96]{};
    GetClassNameW(hRoot, rootCls, ARRAYSIZE(rootCls));
    return wcscmp(rootCls, L"Progman") == 0 || wcscmp(rootCls, L"WorkerW") == 0;
}

static bool IsPointCandidateForScope(POINT pt, int scope) {
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

static bool IsPointCandidateForMiddleClick(POINT pt) {
    if (g_toastExcludeFromCapture.load(std::memory_order_relaxed) && IsCaptureOverlayActive()) {
        return false;
    }
    if (g_mouseTriggerScope.load(std::memory_order_relaxed) == 1) {
        return !IsFullscreenBusy();
    }
    return IsPointCandidateForScope(pt, 0);
}

static bool IsPointEmptyDesktopOrTaskbarUIA(IUIAutomation* pUia, POINT pt, int scope) {
    bool onTaskbar = IsPointOnTaskbarRect(pt);
    bool onDesktop = !onTaskbar && IsPointOnDesktopWindow(pt);

    if (scope == 1 && !onTaskbar) return false;
    if (scope == 2 && !onDesktop) return false;
    if (!onTaskbar && !onDesktop) return false;

    if (!pUia) {
        return false;
    }

    IUIAutomationElement* pElem = nullptr;
    HRESULT hr = pUia->ElementFromPoint(pt, &pElem);
    if (FAILED(hr) || !pElem) {
        return false;
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
        if (ctrlType == UIA_ListItemControlTypeId ||
            ctrlType == UIA_ButtonControlTypeId ||
            ctrlType == UIA_MenuItemControlTypeId ||
            ctrlType == UIA_EditControlTypeId ||
            ctrlType == UIA_TreeItemControlTypeId) {
            return false;
        }
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
};

static ClickGestureState g_mclickState;
static ClickGestureState g_leftTapState;
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

    // Gesture Guard: non-target buttons and wheel scrolls reset all click gestures immediately.
    if (wParam == WM_RBUTTONDOWN || wParam == WM_RBUTTONUP ||
        wParam == WM_XBUTTONDOWN || wParam == WM_XBUTTONUP ||
        wParam == WM_MOUSEWHEEL || wParam == WM_MOUSEHWHEEL) {
        ResetClickGesture(g_mclickState);
        ResetClickGesture(g_leftTapState);
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
        return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
    }

    uint64_t now = GetTickCount64();

    // 1. 3x Left-Click on Empty Desktop/Taskbar Space -> Temp Cleanup
    if (wParam == WM_LBUTTONDOWN || wParam == WM_LBUTTONUP) {
        ResetClickGesture(g_mclickState);
        bool isDown = (wParam == WM_LBUTTONDOWN);

        if (!g_tempGestureEnabled.load(std::memory_order_relaxed)) {
            ResetClickGesture(g_leftTapState);
            return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
        }

        uint64_t maxInterval = g_tempGestureMaxIntervalMs.load(std::memory_order_relaxed);
        if (isDown) {
            if (g_leftTapState.completedClicks > 0) {
                uint64_t elapsed = now - g_leftTapState.lastDownTick;
                if (elapsed > maxInterval || !IsWithinMultiClickTolerance(ms->pt, g_leftTapState.firstDownPt)) {
                    ResetClickGesture(g_leftTapState);
                }
            }
            int tempScope = g_tempGestureScope.load(std::memory_order_relaxed);
            if ((g_toastExcludeFromCapture.load(std::memory_order_relaxed) && IsCaptureOverlayActive()) ||
                !IsPointCandidateForScope(ms->pt, tempScope)) {
                ResetClickGesture(g_leftTapState);
                return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
            }
            if (g_leftTapState.completedClicks == 0) {
                g_leftTapState.firstDownPt = ms->pt;
            }
            g_leftTapState.lastDownTick = now;
            g_leftTapState.downInProgress = true;

            if (g_leftTapState.completedClicks + 1 >= 3) {
                ResetClickGesture(g_leftTapState);
                if (g_uiMsgHwnd) {
                    PostMessageW(
                        g_uiMsgHwnd,
                        WM_APP_CHECK_TEMP_TAP,
                        static_cast<WPARAM>(ms->pt.x),
                        static_cast<LPARAM>(ms->pt.y));
                }
            }
        } else {
            if (g_leftTapState.downInProgress) {
                uint64_t elapsed = now - g_leftTapState.lastDownTick;
                if (elapsed <= maxInterval &&
                    IsWithinMultiClickTolerance(ms->pt, g_leftTapState.firstDownPt)) {
                    g_leftTapState.downInProgress = false;
                    g_leftTapState.completedClicks++;
                } else {
                    ResetClickGesture(g_leftTapState);
                }
            } else {
                ResetClickGesture(g_leftTapState);
            }
        }
        return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
    }

    // 2. Middle-Click Shortcut -> Recycle Bin Cleanup (verified asynchronously via UIA on UI thread)
    int requiredClicks = g_mouseClicksRequired.load(std::memory_order_relaxed);
    if (requiredClicks <= 0) {
        ResetClickGesture(g_mclickState);
        return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
    }

    uint64_t maxInterval = g_mouseMaxIntervalMs.load(std::memory_order_relaxed);
    if (wParam == WM_MBUTTONDOWN) {
        ResetClickGesture(g_leftTapState);
        if (g_mclickState.completedClicks > 0) {
            uint64_t elapsed = now - g_mclickState.lastDownTick;
            if (elapsed > maxInterval || !IsWithinMultiClickTolerance(ms->pt, g_mclickState.firstDownPt)) {
                ResetClickGesture(g_mclickState);
            }
        }
        if (!IsPointCandidateForMiddleClick(ms->pt)) {
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
            if (g_uiMsgHwnd) {
                PostMessageW(
                    g_uiMsgHwnd,
                    WM_APP_CHECK_RECYCLE_TAP,
                    static_cast<WPARAM>(ms->pt.x),
                    static_cast<LPARAM>(ms->pt.y));
            }
        }
        return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
    }

    if (wParam == WM_MBUTTONUP) {
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
        Wh_Log(L"SetWindowsHookExW(WH_MOUSE_LL) failed (err=%lu)", GetLastError());
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
        g_mouseThread = CreateThread(nullptr, 0, MouseHookThreadProc, nullptr, 0, &g_mouseThreadId);
    } else if (!needHook && g_mouseThread) {
        if (g_mouseThreadId != 0) {
            PostThreadMessageW(g_mouseThreadId, WM_QUIT, 0, 0);
        }
        WaitForSingleObject(g_mouseThread, INFINITE);
        CloseHandle(g_mouseThread);
        g_mouseThread = nullptr;
        g_mouseThreadId = 0;
        ResetClickGesture(g_mclickState);
        ResetClickGesture(g_leftTapState);
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

static UINT GetMonitorDpiForPoint(POINT pt, HWND fallbackHwnd) {
    HMONITOR hMon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    if (hMon) {
        UINT dpiX = 96, dpiY = 96;
        if (SUCCEEDED(GetDpiForMonitor(hMon, MDT_EFFECTIVE_DPI, &dpiX, &dpiY)) && dpiX >= 96) {
            return dpiX;
        }
    }
    if (fallbackHwnd) {
        UINT dpi = GetDpiForWindow(fallbackHwnd);
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

    bool isDryRun = (title.find(L"DRY RUN") != std::wstring::npos || title.find(L"Dry-Run") != std::wstring::npos);
    bool isError = (title.find(L"Error") != std::wstring::npos);
    bool isEmpty = (title.find(L"Empty") != std::wstring::npos || title.find(L"Already Clean") != std::wstring::npos);

    Gdiplus::Color accentFill = isDryRun
        ? Gdiplus::Color(255, 224, 138, 68)
        : (isError
            ? Gdiplus::Color(255, 200, 90, 50)
            : (isEmpty
                ? Gdiplus::Color(255, 212, 163, 115)
                : Gdiplus::Color(255, 217, 119, 87)));

    int accentBarW = (std::max)(3, scale(3));
    int leftInset = scale(12) + accentBarW + scale(10);
    int rightPad = scale(16);
    int padY = scale(12);
    int gapY = scale(4);
    int minW = scale(240);
    int maxW = scale(460);
    float radius = static_cast<float>(scale(12));

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
        g.SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);
        g.SetTextContrast(6);

        float fw = static_cast<float>(w);
        float fh = static_cast<float>(h);

        Gdiplus::GraphicsPath outerPath;
        BuildRoundedRectPath(outerPath, 0.0f, 0.0f, fw, fh, radius);
        Gdiplus::SolidBrush outerSurfaceBrush(Gdiplus::Color(255, 24, 17, 13));
        g.FillPath(&outerSurfaceBrush, &outerPath);

        float inset = 1.0f;
        Gdiplus::GraphicsPath innerPath;
        BuildRoundedRectPath(
            innerPath, inset, inset, fw - inset * 2.0f, fh - inset * 2.0f, (std::max)(2.0f, radius - inset));
        Gdiplus::SolidBrush oledBlackBrush(Gdiplus::Color(255, 0, 0, 0));
        g.FillPath(&oledBlackBrush, &innerPath);

        float barX = static_cast<float>(scale(11));
        float barY = static_cast<float>(scale(11));
        float barW = static_cast<float>(accentBarW);
        float barH = fh - barY * 2.0f;
        Gdiplus::GraphicsPath accentBarPath;
        BuildRoundedRectPath(accentBarPath, barX, barY, barW, barH, barW * 0.5f);
        Gdiplus::SolidBrush accentBrush(accentFill);
        g.FillPath(&accentBrush, &accentBarPath);

        Gdiplus::SolidBrush titleBrush(Gdiplus::Color(255, 245, 240, 235));
        Gdiplus::SolidBrush subBrush(Gdiplus::Color(255, 196, 178, 162));

        Gdiplus::PointF ptTitle(static_cast<float>(leftInset), static_cast<float>(padY));
        Gdiplus::PointF ptSub(static_cast<float>(leftInset), static_cast<float>(padY + titleH + gapY));

        g.DrawString(title.c_str(), -1, &titleFont, ptTitle, fmt, &titleBrush);
        g.DrawString(subtitle.c_str(), -1, &subFont, ptSub, fmt, &subBrush);
    }

    SelectObject(hdcMem, oldBmp);
    DeleteDC(hdcMem);

    g_toast.widthPx = w;
    g_toast.heightPx = h;

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
        int scope = g_tempGestureScope.load(std::memory_order_relaxed);
        if (IsPointEmptyDesktopOrTaskbarUIA(g_pUia, pt, scope)) {
            g_manualTempRequested.store(true, std::memory_order_release);
            if (g_wakeEvent) {
                SetEvent(g_wakeEvent);
            }
        }
        return 0;
    }
    if (msg == WM_APP_CHECK_RECYCLE_TAP) {
        POINT pt{static_cast<LONG>(wParam), static_cast<LONG>(lParam)};
        int scope = g_mouseTriggerScope.load(std::memory_order_relaxed);
        bool allowed = (scope == 1)
            ? !IsFullscreenBusy()
            : IsPointEmptyDesktopOrTaskbarUIA(g_pUia, pt, 0);
        if (allowed) {
            g_manualRecycleRequested.store(true, std::memory_order_release);
            if (g_wakeEvent) {
                SetEvent(g_wakeEvent);
            }
        }
        return 0;
    }
    if (msg == WM_APP_RECYCLE_CHANGED) {
        PIDLIST_ABSOLUTE* ppidl = nullptr;
        LONG lEvent = 0;
        HANDLE hLock = SHChangeNotification_Lock(
            reinterpret_cast<HANDLE>(wParam), static_cast<DWORD>(lParam), &ppidl, &lEvent);
        if (hLock) {
            SHChangeNotification_Unlock(hLock);
        }
        uint64_t ignoreUntil = g_ignoreRecycleNotifyUntilTick.load(std::memory_order_acquire);
        if (GetTickCount64() >= ignoreUntil) {
            g_recycleDirty.store(true, std::memory_order_release);
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

    g_uiMsgHwnd = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        wc.lpszClassName, L"", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, wc.hInstance, nullptr);

    bool wtsRegistered = false;
    ULONG shNotifyId = 0;
    if (g_uiMsgHwnd) {
        wtsRegistered = (WTSRegisterSessionNotification(g_uiMsgHwnd, NOTIFY_FOR_THIS_SESSION) != FALSE);

        PIDLIST_ABSOLUTE pidlBin = nullptr;
        if (SUCCEEDED(SHGetKnownFolderIDList(FOLDERID_RecycleBinFolder, 0, nullptr, &pidlBin)) && pidlBin) {
            SHChangeNotifyEntry entry{};
            entry.pidl = pidlBin;
            entry.fRecursive = TRUE;
            shNotifyId = SHChangeNotifyRegister(
                g_uiMsgHwnd,
                SHCNRF_ShellLevel | SHCNRF_InterruptLevel | SHCNRF_NewDelivery,
                SHCNE_CREATE | SHCNE_DELETE | SHCNE_MKDIR | SHCNE_RMDIR |
                    SHCNE_RENAMEITEM | SHCNE_RENAMEFOLDER | SHCNE_UPDATEITEM | SHCNE_UPDATEDIR,
                WM_APP_RECYCLE_CHANGED,
                1,
                &entry);
            CoTaskMemFree(pidlBin);
        }
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

    if (shNotifyId != 0) {
        SHChangeNotifyDeregister(shNotifyId);
        shNotifyId = 0;
    }
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

static void DrainAndRearmDirWatches(std::vector<DirWatchHandle*>& watches) {
    for (DirWatchHandle* w : watches) {
        if (!w || !w->eventHandle) {
            continue;
        }
        if (w->pending) {
            while (WaitForSingleObject(w->eventHandle, 0) == WAIT_OBJECT_0) {
                DWORD bytesTransferred = 0;
                GetOverlappedResult(w->dirHandle, &w->overlapped, &bytesTransferred, FALSE);
                w->pending = false;
                if (!ArmDirWatch(*w)) {
                    break;
                }
            }
        } else {
            ArmDirWatch(*w);
        }
    }
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
// Recycle Bin Engine (Event-Driven + Oldest-First $I/$R Pruning)
// ============================================================================

enum class RecycleCleanStatus {
    Emptied,
    Pruned,
    AlreadyEmpty,
    DryRun,
    Failed,
};

enum class RecycleCleanMode {
    FullEmpty,
    ExpireOldItems,
    PruneThresholds,
    EmergencyPrune,
};

struct RecycleCleanResult {
    RecycleCleanStatus status = RecycleCleanStatus::AlreadyEmpty;
    uint64_t bytesFreed = 0;
    uint64_t itemsCleaned = 0;
    uint64_t oldestRemainingDeletedAt100Ns = 0;
};

static void NotifyRecycleBinChanged() {
    g_ignoreRecycleNotifyUntilTick.store(GetTickCount64() + 1500ULL, std::memory_order_release);
    PIDLIST_ABSOLUTE pidlBin = nullptr;
    if (SUCCEEDED(SHGetKnownFolderIDList(FOLDERID_RecycleBinFolder, 0, nullptr, &pidlBin)) && pidlBin) {
        SHChangeNotify(SHCNE_UPDATEDIR, SHCNF_IDLIST, pidlBin, nullptr);
        CoTaskMemFree(pidlBin);
    }
}

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
    if (g_stopRequested.load(std::memory_order_relaxed)) {
        return false;
    }

    std::wstring pattern = extPath + L"\\*";
    WIN32_FIND_DATAW fd{};
    HANDLE hFind = FindFirstFileExW(
        pattern.c_str(), FindExInfoBasic, &fd, FindExSearchNameMatch, nullptr, FIND_FIRST_EX_LARGE_FETCH);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (g_stopRequested.load(std::memory_order_relaxed)) {
                break;
            }
            if (wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0) {
                continue;
            }
            std::wstring child = extPath + L"\\" + fd.cFileName;
            DWORD origAttrs = fd.dwFileAttributes;
            bool clearedAttrs = false;
            if (origAttrs & (FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN)) {
                clearedAttrs = (SetFileAttributesW(child.c_str(), FILE_ATTRIBUTE_NORMAL) != FALSE);
            }
            bool removed = false;
            if ((origAttrs & FILE_ATTRIBUTE_DIRECTORY) &&
                !(origAttrs & FILE_ATTRIBUTE_REPARSE_POINT)) {
                removed = DeleteTreeRecursive(child);
            } else if (origAttrs & FILE_ATTRIBUTE_DIRECTORY) {
                removed = (RemoveDirectoryW(child.c_str()) != FALSE);
            } else {
                removed = (DeleteFileW(child.c_str()) != FALSE);
            }
            if (!removed && clearedAttrs) {
                SetFileAttributesW(child.c_str(), origAttrs);
            }
        } while (FindNextFileW(hFind, &fd));
        FindClose(hFind);
    }

    if (g_stopRequested.load(std::memory_order_relaxed)) {
        return false;
    }

    DWORD dirAttrs = GetFileAttributesW(extPath.c_str());
    bool clearedDirAttrs = false;
    if (dirAttrs != INVALID_FILE_ATTRIBUTES &&
        (dirAttrs & (FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN))) {
        clearedDirAttrs = (SetFileAttributesW(extPath.c_str(), FILE_ATTRIBUTE_NORMAL) != FALSE);
    }
    bool dirRemoved = (RemoveDirectoryW(extPath.c_str()) != FALSE);
    if (!dirRemoved && clearedDirAttrs) {
        SetFileAttributesW(extPath.c_str(), dirAttrs);
    }
    return dirRemoved;
}

static bool DeleteRecycleCandidate(const RecycleItemCandidate& item) {
    bool dataRemoved = true;
    DWORD attrs = GetFileAttributesW(item.dataFileExtPath.c_str());
    if (attrs != INVALID_FILE_ATTRIBUTES) {
        bool clearedAttrs = false;
        if (attrs & (FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN)) {
            clearedAttrs = (SetFileAttributesW(item.dataFileExtPath.c_str(), FILE_ATTRIBUTE_NORMAL) != FALSE);
        }
        if ((attrs & FILE_ATTRIBUTE_DIRECTORY) && !(attrs & FILE_ATTRIBUTE_REPARSE_POINT)) {
            dataRemoved = DeleteTreeRecursive(item.dataFileExtPath);
        } else if (attrs & FILE_ATTRIBUTE_DIRECTORY) {
            dataRemoved = (RemoveDirectoryW(item.dataFileExtPath.c_str()) != FALSE);
        } else {
            dataRemoved = (DeleteFileW(item.dataFileExtPath.c_str()) != FALSE);
        }
        if (!dataRemoved && clearedAttrs) {
            SetFileAttributesW(item.dataFileExtPath.c_str(), attrs);
        }
    }
    if (!dataRemoved) {
        return false;
    }

    DWORD metaAttrs = GetFileAttributesW(item.metaFileExtPath.c_str());
    bool clearedMetaAttrs = false;
    if (metaAttrs != INVALID_FILE_ATTRIBUTES &&
        (metaAttrs & (FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN))) {
        clearedMetaAttrs = (SetFileAttributesW(item.metaFileExtPath.c_str(), FILE_ATTRIBUTE_NORMAL) != FALSE);
    }
    bool metaRemoved = (DeleteFileW(item.metaFileExtPath.c_str()) != FALSE);
    if (!metaRemoved && clearedMetaAttrs) {
        SetFileAttributesW(item.metaFileExtPath.c_str(), metaAttrs);
    }
    return metaRemoved;
}

static void CollectRecycleBinCandidates(
    uint64_t now100Ns,
    uint64_t minAgeMs,
    wchar_t driveFilter,
    std::vector<RecycleItemCandidate>& outCandidates,
    uint64_t* outTotalBytes,
    uint64_t* outTotalItems,
    uint64_t* outOldestProtectedDeletedAt100Ns) {
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
        wchar_t driveLetter = static_cast<wchar_t>(L'A' + d);
        if (driveFilter != 0 && towupper(driveLetter) != towupper(driveFilter)) {
            continue;
        }
        wchar_t root[] = {driveLetter, L':', L'\\', L'\0'};
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
                if (outOldestProtectedDeletedAt100Ns &&
                    (*outOldestProtectedDeletedAt100Ns == 0 || deletedAt < *outOldestProtectedDeletedAt100Ns)) {
                    *outOldestProtectedDeletedAt100Ns = deletedAt;
                }
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

static std::wstring GetUserScheduleKey(PCWSTR baseName) {
    std::wstring sid = GetCurrentUserSidString();
    if (sid.empty()) {
        return baseName;
    }
    return std::wstring(baseName) + L"." + sid;
}

static uint64_t LoadPersistedFileTime100Ns(const std::wstring& keyName) {
    WCHAR buf[64]{};
    if (Wh_GetStringValue(keyName.c_str(), buf, ARRAYSIZE(buf)) == 0 || !buf[0]) {
        return 0;
    }
    return _wcstoui64(buf, nullptr, 10);
}

static void SavePersistedFileTime100Ns(const std::wstring& keyName, uint64_t val100Ns) {
    WCHAR buf[64]{};
    swprintf_s(buf, L"%llu", static_cast<unsigned long long>(val100Ns));
    Wh_SetStringValue(keyName.c_str(), buf);
}

static RecycleCleanResult RunRecycleBinClean(
    const ModSettings& cfg,
    const wchar_t* reason,
    RecycleCleanMode mode,
    bool sizeHit = false,
    bool countHit = false,
    wchar_t driveFilter = 0) {
    RecycleCleanResult res;
    uint64_t rbBytes = 0;
    uint64_t rbItems = 0;
    bool statsOk = QueryRecycleBinStats(&rbBytes, &rbItems);

    uint64_t now100Ns = GetCurrentSystemFileTime100Ns();
    std::vector<RecycleItemCandidate> candidates;
    uint64_t scannedBytes = 0;
    uint64_t scannedItems = 0;
    uint64_t oldestProtected = 0;
    uint64_t effectiveMinAgeMs = (mode == RecycleCleanMode::FullEmpty) ? 0ULL : cfg.rbMinAgeMs;
    CollectRecycleBinCandidates(
        now100Ns, effectiveMinAgeMs, driveFilter, candidates, &scannedBytes, &scannedItems, &oldestProtected);

    uint64_t activeBytes = (driveFilter != 0) ? scannedBytes : (std::max)(rbBytes, scannedBytes);
    uint64_t activeItems = (driveFilter != 0) ? scannedItems : (std::max)(rbItems, scannedItems);

    if (!statsOk && scannedItems == 0) {
        Wh_Log(L"[%s | RecycleBin]: SHQueryRecycleBinW failed", reason);
        res.status = RecycleCleanStatus::Failed;
        return res;
    }
    if (activeItems == 0 && activeBytes == 0) {
        Wh_Log(L"[%s | RecycleBin]: already empty", reason);
        res.status = RecycleCleanStatus::AlreadyEmpty;
        return res;
    }

    BOOL bgEntered = SetThreadPriority(GetCurrentThread(), THREAD_MODE_BACKGROUND_BEGIN);

    if (mode != RecycleCleanMode::FullEmpty) {
        if (candidates.empty()) {
            if (bgEntered) {
                SetThreadPriority(GetCurrentThread(), THREAD_MODE_BACKGROUND_END);
            }
            Wh_Log(L"[%s | RecycleBin-Prune]%s: 0 eligible items (all within min age or skipped)",
                   reason, cfg.dryRun ? L" (DRY-RUN ONLY)" : L"");
            res.status = cfg.dryRun ? RecycleCleanStatus::DryRun : RecycleCleanStatus::AlreadyEmpty;
            res.oldestRemainingDeletedAt100Ns = oldestProtected;
            return res;
        }

        std::sort(candidates.begin(), candidates.end(), [](const RecycleItemCandidate& a, const RecycleItemCandidate& b) {
            return a.deletedAt100Ns < b.deletedAt100Ns;
        });

        uint64_t expireAge100Ns = cfg.rbRetentionEnabled
            ? MultiplyClampU64(cfg.rbRetentionAgeMs, 10000ULL)
            : 0ULL;

        uint64_t remBytes = activeBytes;
        uint64_t remItems = activeItems;

        bool enforceByteTarget = false;
        bool enforceItemTarget = false;
        uint64_t targetBytes = remBytes;
        uint64_t targetItems = remItems;

        if (mode == RecycleCleanMode::PruneThresholds) {
            if (sizeHit) {
                enforceByteTarget = true;
                targetBytes = ScaleByPercentU64(cfg.rbSizeThresholdBytes, cfg.rbTargetPercent);
            }
            if (countHit) {
                enforceItemTarget = true;
                targetItems = ScaleByPercentU64(cfg.rbMaxItems, cfg.rbTargetPercent);
            }
        } else if (mode == RecycleCleanMode::EmergencyPrune) {
            enforceByteTarget = true;
            targetBytes = ScaleByPercentU64(remBytes, cfg.rbTargetPercent);
        }

        uint64_t freedBytes = 0;
        uint64_t deletedItems = 0;
        uint64_t failedItems = 0;
        uint64_t oldestSurvivor = oldestProtected;

        for (const auto& item : candidates) {
            if (g_stopRequested.load(std::memory_order_relaxed)) {
                break;
            }

            bool isExpiredByAge = (expireAge100Ns > 0 && now100Ns > item.deletedAt100Ns &&
                                   (now100Ns - item.deletedAt100Ns) >= expireAge100Ns);

            bool shouldDelete = false;
            if (mode == RecycleCleanMode::ExpireOldItems) {
                shouldDelete = isExpiredByAge;
            } else {
                bool needMoreBytes = enforceByteTarget && (remBytes > targetBytes);
                bool needMoreItems = enforceItemTarget && (remItems > targetItems);
                shouldDelete = isExpiredByAge || needMoreBytes || needMoreItems;
            }

            if (!shouldDelete) {
                if (oldestSurvivor == 0 || item.deletedAt100Ns < oldestSurvivor) {
                    oldestSurvivor = item.deletedAt100Ns;
                }
                continue;
            }

            if (cfg.dryRun) {
                freedBytes += item.sizeBytes;
                deletedItems++;
                if (remBytes >= item.sizeBytes) remBytes -= item.sizeBytes;
                else remBytes = 0;
                if (remItems > 0) remItems--;
            } else if (DeleteRecycleCandidate(item)) {
                freedBytes += item.sizeBytes;
                deletedItems++;
                if (remBytes >= item.sizeBytes) remBytes -= item.sizeBytes;
                else remBytes = 0;
                if (remItems > 0) remItems--;
            } else {
                // Do not add failed items to oldestSurvivor so they cannot cause a 1s retry loop;
                // they will be retried on the next Recycle Bin change or next surviving item's expiry.
                failedItems++;
            }
        }

        if (bgEntered) {
            SetThreadPriority(GetCurrentThread(), THREAD_MODE_BACKGROUND_END);
        }

        if (!cfg.dryRun && deletedItems > 0) {
            NotifyRecycleBinChanged();
        }

        Wh_Log(
            L"[%s | RecycleBin-Prune]%s: %s %llu bytes (%llu items, %llu remaining, %llu failed)",
            reason,
            cfg.dryRun ? L" (DRY-RUN ONLY)" : L"",
            cfg.dryRun ? L"would free" : L"freed",
            static_cast<unsigned long long>(freedBytes),
            static_cast<unsigned long long>(deletedItems),
            static_cast<unsigned long long>(remItems),
            static_cast<unsigned long long>(failedItems));
        res.status = cfg.dryRun
            ? RecycleCleanStatus::DryRun
            : (deletedItems > 0 ? RecycleCleanStatus::Pruned : RecycleCleanStatus::AlreadyEmpty);
        res.bytesFreed = freedBytes;
        res.itemsCleaned = deletedItems;
        res.oldestRemainingDeletedAt100Ns = oldestSurvivor;
        return res;
    }

    // Full Empty path (explicit user middle-click or Full Sweep enabled on threshold)
    if (cfg.dryRun) {
        if (bgEntered) {
            SetThreadPriority(GetCurrentThread(), THREAD_MODE_BACKGROUND_END);
        }
        Wh_Log(
            L"[%s | RecycleBin-FullSweep] (DRY-RUN ONLY): would free %llu bytes (%llu items) — zero files touched",
            reason,
            static_cast<unsigned long long>(activeBytes),
            static_cast<unsigned long long>(activeItems));
        res.status = RecycleCleanStatus::DryRun;
        res.bytesFreed = activeBytes;
        res.itemsCleaned = activeItems;
        return res;
    }

    g_ignoreRecycleNotifyUntilTick.store(GetTickCount64() + 1500ULL, std::memory_order_release);
    DWORD flags = SHERB_NOCONFIRMATION | SHERB_NOPROGRESSUI;
    if (!cfg.rbPlaySound) {
        flags |= SHERB_NOSOUND;
    }
    HRESULT hr = SHEmptyRecycleBinW(nullptr, nullptr, flags);
    if (FAILED(hr) && !candidates.empty()) {
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
            NotifyRecycleBinChanged();
            hr = S_OK;
            activeBytes = fallbackFreed;
            activeItems = fallbackDeleted;
        }
    }
    if (bgEntered) {
        SetThreadPriority(GetCurrentThread(), THREAD_MODE_BACKGROUND_END);
    }

    if (FAILED(hr)) {
        Wh_Log(
            L"[%s | RecycleBin-FullSweep]: SHEmptyRecycleBinW failed (hr=0x%08X)",
            reason,
            static_cast<unsigned int>(hr));
        res.status = RecycleCleanStatus::Failed;
        return res;
    }

    Wh_Log(
        L"[%s | RecycleBin-FullSweep]: freed %llu bytes (%llu items)",
        reason,
        static_cast<unsigned long long>(activeBytes),
        static_cast<unsigned long long>(activeItems));
    res.status = RecycleCleanStatus::Emptied;
    res.bytesFreed = activeBytes;
    res.itemsCleaned = activeItems;
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
                childPath, depth + 1, now100Ns, effectiveMinAgeMs, cfg, files, dirs, stats);
            if (IsOlderThanGracePeriod(fd, now100Ns, effectiveMinAgeMs)) {
                dirs.push_back({childPath, depth + 1});
            }
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
}

static CleanStats RunCleanupPass(const ModSettings& cfg, const wchar_t* reason, bool forceFullSweep) {
    BOOL bgEntered = SetThreadPriority(GetCurrentThread(), THREAD_MODE_BACKGROUND_BEGIN);

    uint64_t now100Ns = GetCurrentSystemFileTime100Ns();
    bool isFullSweepMode = forceFullSweep || cfg.fullSweep;
    // Always honor minFileAgeMs so full sweeps ignore targetPercent without deleting brand-new files.
    uint64_t effectiveMinAgeMs = cfg.minFileAgeMs;

    CleanStats stats;
    std::vector<FileCandidate> files;
    std::vector<DirCandidate> dirs;

    for (const auto& dir : GetTargetTempDirs(cfg)) {
        CollectCandidatesRecursive(
            ToExtendedPath(dir), 0, now100Ns, effectiveMinAgeMs, cfg, files, dirs, stats);
    }

    bool useOldestFirst = !isFullSweepMode;

    uint64_t targetBytes = 0;
    uint64_t remainingBytes = stats.eligibleBytes;
    if (useOldestFirst) {
        targetBytes = ScaleByPercentU64(cfg.thresholdBytes, cfg.targetPercent);
        std::sort(files.begin(), files.end(), [](const FileCandidate& a, const FileCandidate& b) {
            return a.timestamp100Ns < b.timestamp100Ns;
        });
    }

    for (const auto& f : files) {
        if (g_stopRequested.load(std::memory_order_relaxed)) {
            break;
        }
        if (useOldestFirst && remainingBytes <= targetBytes) {
            break;
        }
        if (cfg.dryRun) {
            stats.bytesFreed += f.sizeBytes;
            stats.filesDeleted++;
            if (remainingBytes >= f.sizeBytes) remainingBytes -= f.sizeBytes;
            else remainingBytes = 0;
            continue;
        }
        DWORD attrs = GetFileAttributesW(f.extPath.c_str());
        bool clearedAttrs = false;
        if (attrs != INVALID_FILE_ATTRIBUTES &&
            (attrs & (FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM))) {
            clearedAttrs = (SetFileAttributesW(f.extPath.c_str(), FILE_ATTRIBUTE_NORMAL) != FALSE);
        }
        if (DeleteFileW(f.extPath.c_str())) {
            stats.bytesFreed += f.sizeBytes;
            stats.filesDeleted++;
            if (remainingBytes >= f.sizeBytes) remainingBytes -= f.sizeBytes;
            else remainingBytes = 0;
        } else {
            if (clearedAttrs) {
                SetFileAttributesW(f.extPath.c_str(), attrs);
            }
            stats.skippedLocked++;
        }
    }

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
        bool clearedAttrs = false;
        if (dAttrs != INVALID_FILE_ATTRIBUTES &&
            (dAttrs & (FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM))) {
            clearedAttrs = (SetFileAttributesW(d.extPath.c_str(), FILE_ATTRIBUTE_NORMAL) != FALSE);
        }
        if (RemoveDirectoryW(d.extPath.c_str())) {
            stats.dirsDeleted++;
        } else if (clearedAttrs) {
            SetFileAttributesW(d.extPath.c_str(), dAttrs);
        }
    }

    if (bgEntered) {
        SetThreadPriority(GetCurrentThread(), THREAD_MODE_BACKGROUND_END);
    }

    Wh_Log(
        L"[%s | %s]%s: freed %llu bytes (%llu files, %llu dirs); scanned=%llu skipped locked=%llu recent=%llu aiAgents=%llu excluded=%llu reparse=%llu",
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

    std::wstring tempTimerKey = GetUserScheduleKey(L"schedule.lastTempTimerFt");

    uint64_t now = GetTickCount64();
    uint64_t loginDesktopReadyTick = 0;
    if (IsInteractiveUserDesktopReady()) {
        loginDesktopReadyTick = now;
        Wh_Log(L"Interactive user desktop active; lazy startup scheduled in %llu ms",
               static_cast<unsigned long long>(cfg.startupDelayMs));
    } else {
        Wh_Log(L"Waiting for interactive desktop before starting automatic timers");
    }

    uint64_t lastTimerClean = now;
    uint64_t lastTempTimerFt100Ns = LoadPersistedFileTime100Ns(tempTimerKey);
    uint64_t nowFt100Ns = GetCurrentSystemFileTime100Ns();
    if (lastTempTimerFt100Ns == 0 || lastTempTimerFt100Ns > nowFt100Ns) {
        lastTempTimerFt100Ns = nowFt100Ns;
        SavePersistedFileTime100Ns(tempTimerKey, lastTempTimerFt100Ns);
    }

    uint64_t lastEmergencyCheck = 0;
    bool emergencyBaselineRecorded = false;
    bool emergencyFloorActive = false;
    uint64_t lastCleanTime = 0;
    uint64_t lastRecycleCleanTime = 0;
    uint64_t nextRecycleExpireFt100Ns = 0;
    bool startupTempPending = cfg.startupCleanTemp;
    bool startupRecyclePending = cfg.startupCleanRecycle;
    bool watchDirty = cfg.thresholdEnabled;
    uint64_t lastWriteEventTime = watchDirty ? now : 0;
    bool deferredPending = false;
    bool lastCleanWinTemp = cfg.cleanWindowsTemp;
    bool lastThreshEnabled = cfg.thresholdEnabled;
    bool lastTimerEnabled = cfg.timerEnabled;
    bool lastEmergEnabled = cfg.emergencyEnabled;
    uint64_t lastEmergFloorBytes = cfg.emergencyFloorBytes;
    bool lastDryRun = false;
    bool dryRunCompletedOnce = false;

    auto runTempPassAndDrain = [&](const wchar_t* reason, bool forceFullSweep) -> CleanStats {
        CleanStats stats = RunCleanupPass(cfg, reason, forceFullSweep);
        if (!cfg.dryRun) {
            DrainAndRearmDirWatches(watches);
        }
        watchDirty = false;
        lastWriteEventTime = 0;
        return stats;
    };

    auto updateNextRecycleExpiry = [&](uint64_t oldestSurvivor100Ns) {
        if (cfg.rbRetentionEnabled && oldestSurvivor100Ns > 0) {
            uint64_t effectiveAgeMs = (std::max)(cfg.rbRetentionAgeMs, cfg.rbMinAgeMs);
            uint64_t age100Ns = MultiplyClampU64(effectiveAgeMs, 10000ULL);
            nextRecycleExpireFt100Ns = oldestSurvivor100Ns + age100Ns;
        } else {
            nextRecycleExpireFt100Ns = 0;
        }
    };

    while (!g_stopRequested.load(std::memory_order_relaxed)) {
        cfg = SnapshotSettings();
        now = GetTickCount64();

        if (cfg.cleanWindowsTemp != lastCleanWinTemp || cfg.thresholdEnabled != lastThreshEnabled) {
            OpenDirWatches(cfg, watches);
            lastCleanWinTemp = cfg.cleanWindowsTemp;
            lastThreshEnabled = cfg.thresholdEnabled;
            watchDirty = cfg.thresholdEnabled;
            lastWriteEventTime = watchDirty ? now : 0;
        }

        if (!cfg.dryRun) {
            dryRunCompletedOnce = false;
            lastDryRun = false;
        }

        if (g_settingsReconfigured.exchange(false, std::memory_order_acq_rel)) {
            g_recycleDirty.store(true, std::memory_order_release);
            lastEmergencyCheck = 0;
            if (cfg.emergencyEnabled != lastEmergEnabled || cfg.emergencyFloorBytes != lastEmergFloorBytes) {
                emergencyBaselineRecorded = false;
                emergencyFloorActive = false;
                lastEmergEnabled = cfg.emergencyEnabled;
                lastEmergFloorBytes = cfg.emergencyFloorBytes;
            }
            if (cfg.timerEnabled && !lastTimerEnabled) {
                lastTimerClean = now;
                lastTempTimerFt100Ns = GetCurrentSystemFileTime100Ns();
                SavePersistedFileTime100Ns(tempTimerKey, lastTempTimerFt100Ns);
            }
            lastTimerEnabled = cfg.timerEnabled;
            if (cfg.thresholdEnabled) {
                watchDirty = true;
                lastWriteEventTime = 0;
            }
        }

        // Single-shot Dry-Run Mode
        if (cfg.dryRun && (!dryRunCompletedOnce || !lastDryRun)) {
            lastDryRun = true;
            dryRunCompletedOnce = true;
            Wh_Log(L"Dry-Run mode enabled: running single-shot non-destructive preview");
            CleanStats tempPreview = runTempPassAndDrain(L"DryRunPreview", cfg.fullSweep);
            RecycleCleanResult rbPreview = RunRecycleBinClean(
                cfg,
                L"DryRunPreview",
                cfg.rbFullSweep ? RecycleCleanMode::FullEmpty : RecycleCleanMode::ExpireOldItems);
            NotifySingleShotDryRunToast(tempPreview, rbPreview);
        }

        // 0a. Immediate manual 3x left-click Temp trigger
        if (g_manualTempRequested.exchange(false, std::memory_order_acq_rel)) {
            Wh_Log(L"3x empty-space left-click Temp trigger activated");
            CleanStats stats = runTempPassAndDrain(L"Taskbar3xClick", true);
            lastCleanTime = GetTickCount64();
            NotifyTempToast(stats, cfg.dryRun, true);
        }

        // 0b. Immediate manual middle-click Recycle Bin trigger
        if (g_manualRecycleRequested.exchange(false, std::memory_order_acq_rel)) {
            Wh_Log(L"Middle-click Recycle Bin trigger activated");
            RecycleCleanResult rbRes = RunRecycleBinClean(cfg, L"MiddleClick", RecycleCleanMode::FullEmpty);
            lastRecycleCleanTime = GetTickCount64();
            updateNextRecycleExpiry(rbRes.oldestRemainingDeletedAt100Ns);
            NotifyRecycleToast(rbRes, true);
        }

        bool desktopReady = IsInteractiveUserDesktopReady();
        now = GetTickCount64();
        if (desktopReady && loginDesktopReadyTick == 0) {
            loginDesktopReadyTick = now;
            lastTimerClean = now;
            Wh_Log(L"Interactive desktop ready; lazy startup will fire in %llu ms",
                   static_cast<unsigned long long>(cfg.startupDelayMs));
        }

        const wchar_t* deferReason = nullptr;
        bool allowAutoRuns = desktopReady && (!cfg.dryRun || !dryRunCompletedOnce);

        // 1. Lazy Post-Logon Startup cleanup trigger
        if (allowAutoRuns && (startupTempPending || startupRecyclePending) &&
            loginDesktopReadyTick != 0 && (now - loginDesktopReadyTick >= cfg.startupDelayMs)) {
            if (!ShouldDeferCleanup(cfg, false, &deferReason)) {
                if (startupTempPending) {
                    Wh_Log(L"Startup Temp cleanup triggered (fullSweep=%d)", cfg.startupFullSweep ? 1 : 0);
                    CleanStats stats = runTempPassAndDrain(L"Startup", cfg.startupFullSweep);
                    startupTempPending = false;
                    lastCleanTime = GetTickCount64();
                    lastTimerClean = lastCleanTime;
                    lastTempTimerFt100Ns = GetCurrentSystemFileTime100Ns();
                    SavePersistedFileTime100Ns(tempTimerKey, lastTempTimerFt100Ns);
                    NotifyTempToast(stats, cfg.dryRun, false);
                }
                if (startupRecyclePending) {
                    Wh_Log(L"Startup Recycle Bin cleanup triggered");
                    RecycleCleanMode rbMode = (cfg.startupFullSweep && cfg.rbFullSweep)
                        ? RecycleCleanMode::FullEmpty
                        : RecycleCleanMode::ExpireOldItems;
                    RecycleCleanResult rbRes = RunRecycleBinClean(cfg, L"Startup", rbMode);
                    startupRecyclePending = false;
                    lastRecycleCleanTime = GetTickCount64();
                    updateNextRecycleExpiry(rbRes.oldestRemainingDeletedAt100Ns);
                    NotifyRecycleToast(rbRes, false);
                }
                deferredPending = false;
            } else {
                deferredPending = true;
            }
        }

        // 2. Low-disk emergency floor check (edge-triggered once per crossing below the floor)
        now = GetTickCount64();
        if (allowAutoRuns && cfg.emergencyEnabled &&
            (lastEmergencyCheck == 0 || (now - lastEmergencyCheck >= cfg.emergencyCheckMs))) {
            lastEmergencyCheck = now;
            uint64_t freeBytes = 0;
            wchar_t lowDrive = 0;
            bool belowFloor = IsDiskSpaceBelowFloor(cfg, &freeBytes, &lowDrive);
            if (!emergencyBaselineRecorded) {
                // First check after process start only records whether the drive is already below the floor
                // so emergency cleanup runs only on an actual crossing below the floor.
                emergencyBaselineRecorded = true;
                emergencyFloorActive = belowFloor;
            } else if (!belowFloor) {
                emergencyFloorActive = false;
            } else if (!emergencyFloorActive) {
                if (!ShouldDeferCleanup(cfg, true, &deferReason)) {
                    emergencyFloorActive = true;
                    Wh_Log(L"Emergency floor crossed on %c:\\: %llu < %llu free bytes",
                           lowDrive ? lowDrive : L'?',
                           static_cast<unsigned long long>(freeBytes),
                           static_cast<unsigned long long>(cfg.emergencyFloorBytes));
                    CleanStats stats = runTempPassAndDrain(L"EmergencyFloor", true);
                    NotifyTempToast(stats, cfg.dryRun, false);
                    if (cfg.cleanRecycleOnEmergency) {
                        RecycleCleanResult rbRes = RunRecycleBinClean(
                            cfg, L"EmergencyFloor", RecycleCleanMode::EmergencyPrune, false, false, lowDrive);
                        lastRecycleCleanTime = GetTickCount64();
                        if (rbRes.itemsCleaned > 0) {
                            g_recycleDirty.store(true, std::memory_order_release);
                        }
                        NotifyRecycleToast(rbRes, false);
                    }
                    lastCleanTime = GetTickCount64();
                    deferredPending = false;
                } else {
                    deferredPending = true;
                }
            }
        }

        // 3. Temp periodic timer trigger (tick + per-user persisted FILETIME across sleep/reboots)
        now = GetTickCount64();
        nowFt100Ns = GetCurrentSystemFileTime100Ns();
        uint64_t tempTimerFtDeltaMs = (nowFt100Ns > lastTempTimerFt100Ns)
            ? ((nowFt100Ns - lastTempTimerFt100Ns) / 10000ULL)
            : 0ULL;
        if (allowAutoRuns && cfg.timerEnabled &&
            ((now - lastTimerClean >= cfg.timerIntervalMs) || (tempTimerFtDeltaMs >= cfg.timerIntervalMs))) {
            if (!ShouldDeferCleanup(cfg, false, &deferReason)) {
                Wh_Log(L"Scheduled Temp timer triggered (interval=%llu ms)",
                       static_cast<unsigned long long>(cfg.timerIntervalMs));
                CleanStats stats = runTempPassAndDrain(L"Timer", cfg.fullSweep);
                lastTimerClean = GetTickCount64();
                lastCleanTime = lastTimerClean;
                lastTempTimerFt100Ns = GetCurrentSystemFileTime100Ns();
                SavePersistedFileTime100Ns(tempTimerKey, lastTempTimerFt100Ns);
                deferredPending = false;
                NotifyTempToast(stats, cfg.dryRun, false);
            } else {
                deferredPending = true;
            }
        }

        // 4. Event-driven Recycle Bin threshold & age-expiry pass (driven by SHChangeNotifyRegister + next-expiry wakeup)
        now = GetTickCount64();
        nowFt100Ns = GetCurrentSystemFileTime100Ns();
        bool rbAutoActive = cfg.rbSizeThresholdEnabled || cfg.rbCountThresholdEnabled || cfg.rbRetentionEnabled;
        bool rbExpireDue = cfg.rbRetentionEnabled && nextRecycleExpireFt100Ns > 0 && nowFt100Ns >= nextRecycleExpireFt100Ns;
        bool rbNeedsCheck = rbAutoActive && (g_recycleDirty.load(std::memory_order_acquire) || rbExpireDue);

        if (allowAutoRuns && rbNeedsCheck &&
            (lastRecycleCleanTime == 0 || (now - lastRecycleCleanTime >= cfg.rbCooldownMs))) {
            if (!ShouldDeferCleanup(cfg, false, &deferReason)) {
                g_recycleDirty.store(false, std::memory_order_release);
                uint64_t rbBytes = 0;
                uint64_t rbItems = 0;
                if (QueryRecycleBinStats(&rbBytes, &rbItems) && rbItems > 0) {
                    bool sizeHit = cfg.rbSizeThresholdEnabled && (rbBytes >= cfg.rbSizeThresholdBytes);
                    bool countHit = cfg.rbCountThresholdEnabled && (rbItems >= cfg.rbMaxItems);
                    if (sizeHit || countHit) {
                        Wh_Log(
                            L"Recycle Bin threshold reached: %llu bytes, %llu items (sizeHit=%d countHit=%d)",
                            static_cast<unsigned long long>(rbBytes),
                            static_cast<unsigned long long>(rbItems),
                            sizeHit ? 1 : 0,
                            countHit ? 1 : 0);
                        RecycleCleanMode rbMode = cfg.rbFullSweep
                            ? RecycleCleanMode::FullEmpty
                            : RecycleCleanMode::PruneThresholds;
                        RecycleCleanResult rbRes = RunRecycleBinClean(
                            cfg,
                            sizeHit ? L"RecycleSizeThreshold" : L"RecycleCountThreshold",
                            rbMode,
                            sizeHit,
                            countHit);
                        lastRecycleCleanTime = GetTickCount64();
                        updateNextRecycleExpiry(rbRes.oldestRemainingDeletedAt100Ns);
                        NotifyRecycleToast(rbRes, false);
                    } else if (cfg.rbRetentionEnabled) {
                        RecycleCleanResult ageRes = RunRecycleBinClean(
                            cfg, L"RecycleAgeExpiry", RecycleCleanMode::ExpireOldItems);
                        lastRecycleCleanTime = GetTickCount64();
                        updateNextRecycleExpiry(ageRes.oldestRemainingDeletedAt100Ns);
                        if (ageRes.itemsCleaned > 0) {
                            NotifyRecycleToast(ageRes, false);
                        }
                    }
                } else {
                    nextRecycleExpireFt100Ns = 0;
                }
                deferredPending = false;
            } else {
                deferredPending = true;
            }
        }

        // 5. Event-driven Temp threshold trigger (watches stay paused while watchDirty to avoid I/O wakeups)
        now = GetTickCount64();
        bool debounceElapsed = (lastWriteEventTime == 0) || (now - lastWriteEventTime >= cfg.debounceMs);
        if (allowAutoRuns && cfg.thresholdEnabled && (watchDirty || deferredPending) &&
            debounceElapsed &&
            (lastCleanTime == 0 || (now - lastCleanTime >= cfg.cooldownMs))) {
            if (!ShouldDeferCleanup(cfg, false, &deferReason)) {
                DrainAndRearmDirWatches(watches);

                BOOL bgEntered = SetThreadPriority(GetCurrentThread(), THREAD_MODE_BACKGROUND_BEGIN);
                uint64_t totalSize = 0;
                for (const auto& dir : GetTargetTempDirs(cfg)) {
                    totalSize += MeasureDirectoryBytes(ToExtendedPath(dir), cfg);
                }
                if (bgEntered) {
                    SetThreadPriority(GetCurrentThread(), THREAD_MODE_BACKGROUND_END);
                }

                watchDirty = false;
                lastWriteEventTime = 0;
                deferredPending = false;
                if (totalSize >= cfg.thresholdBytes) {
                    Wh_Log(L"Temp threshold reached: %llu >= %llu bytes",
                           static_cast<unsigned long long>(totalSize),
                           static_cast<unsigned long long>(cfg.thresholdBytes));
                    CleanStats stats = runTempPassAndDrain(L"Threshold", cfg.fullSweep);
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
        nowFt100Ns = GetCurrentSystemFileTime100Ns();
        uint64_t waitMs = INFINITE;
        auto minWait = [&](uint64_t candidate) {
            if (waitMs == INFINITE || candidate < waitMs) {
                waitMs = candidate;
            }
        };

        if (!desktopReady) {
            // While locked, WTSRegisterSessionNotification wakes g_wakeEvent on unlock; no polling needed.
            if (!g_sessionLocked.load(std::memory_order_acquire)) {
                minWait(2000ULL);
            }
        } else if (allowAutoRuns) {
            if (deferredPending) {
                minWait(15000ULL);
            } else {
                if ((startupTempPending || startupRecyclePending) && loginDesktopReadyTick != 0) {
                    uint64_t elapsed = now - loginDesktopReadyTick;
                    minWait(elapsed < cfg.startupDelayMs ? (cfg.startupDelayMs - elapsed) : 1000ULL);
                }
                if (cfg.thresholdEnabled && watchDirty) {
                    uint64_t debRem = (lastWriteEventTime != 0 && now - lastWriteEventTime < cfg.debounceMs)
                        ? (cfg.debounceMs - (now - lastWriteEventTime))
                        : 0;
                    uint64_t coolRem = (lastCleanTime != 0 && now - lastCleanTime < cfg.cooldownMs)
                        ? (cfg.cooldownMs - (now - lastCleanTime))
                        : 0;
                    minWait((std::max)(debRem, coolRem));
                }
                if (cfg.timerEnabled) {
                    uint64_t elapsed = now - lastTimerClean;
                    minWait(elapsed < cfg.timerIntervalMs ? (cfg.timerIntervalMs - elapsed) : 1000ULL);
                }
                if (cfg.rbRetentionEnabled && nextRecycleExpireFt100Ns > 0) {
                    if (nextRecycleExpireFt100Ns > nowFt100Ns) {
                        minWait((nextRecycleExpireFt100Ns - nowFt100Ns) / 10000ULL + 1000ULL);
                    } else {
                        minWait(1000ULL);
                    }
                }
                if (rbAutoActive && g_recycleDirty.load(std::memory_order_acquire)) {
                    uint64_t coolRem = (lastRecycleCleanTime != 0 && now - lastRecycleCleanTime < cfg.rbCooldownMs)
                        ? (cfg.rbCooldownMs - (now - lastRecycleCleanTime))
                        : 1000ULL;
                    minWait(coolRem);
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
        if (!watchDirty) {
            for (DirWatchHandle* w : watches) {
                if (w->pending) {
                    waitHandles.push_back(w->eventHandle);
                }
            }
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
            HANDLE signaled = waitHandles[res - WAIT_OBJECT_0];
            for (DirWatchHandle* w : watches) {
                if (w->eventHandle == signaled) {
                    DWORD bytesTransferred = 0;
                    GetOverlappedResult(w->dirHandle, &w->overlapped, &bytesTransferred, FALSE);
                    w->pending = false;
                    break;
                }
            }
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

    // 1. AI Agents Temp Protection
    auto aiModeStr = WindhawkUtils::StringSetting::make(L"aiAgentProtection.protectionMode");
    next.aiAgentProtectionEnabled = wcscmp(aiModeStr.get(), L"Disabled") != 0;

    for (int i = 0;; i++) {
        auto pat = WindhawkUtils::StringSetting::make(L"aiAgentProtection.agentTempPatterns[%d]", i);
        if (!*pat.get()) {
            break;
        }
        std::wstring cleanPat = NormalizeExcludePattern(pat.get());
        if (!cleanPat.empty()) {
            next.aiAgentPatterns.push_back(std::move(cleanPat));
        }
    }

    // 2. Temp Folder Cleaner
    next.thresholdEnabled = Wh_GetIntSetting(L"tempCleaner.thresholdEnabled") != 0;
    int rawThresh = Wh_GetIntSetting(L"tempCleaner.thresholdValue");
    if (rawThresh < 1) rawThresh = 1;
    auto threshUnit = WindhawkUtils::StringSetting::make(L"tempCleaner.thresholdUnit");
    next.thresholdBytes = MultiplyClampU64(static_cast<uint64_t>(rawThresh), UnitToMultiplier(threshUnit.get()));

    int debSec = Wh_GetIntSetting(L"tempCleaner.debounceSeconds");
    if (debSec < 5) debSec = 5;
    if (debSec > 3600) debSec = 3600;
    next.debounceMs = static_cast<uint64_t>(debSec) * 1000ULL;

    next.timerEnabled = Wh_GetIntSetting(L"tempCleaner.timerEnabled") != 0;
    int rawTimerVal = Wh_GetIntSetting(L"tempCleaner.timerValue");
    if (rawTimerVal < 1) rawTimerVal = 1;
    auto timerUnit = WindhawkUtils::StringSetting::make(L"tempCleaner.timerUnit");
    uint64_t timerMs = MultiplyClampU64(
        static_cast<uint64_t>(rawTimerVal), TimeUnitToMs(timerUnit.get(), 3600000ULL));
    if (timerMs < 15000ULL) timerMs = 15000ULL;
    next.timerIntervalMs = timerMs;

    next.fullSweep = Wh_GetIntSetting(L"tempCleaner.fullSweep") != 0;
    int targetPct = Wh_GetIntSetting(L"tempCleaner.targetPercent");
    if (targetPct < 10) targetPct = 10;
    if (targetPct > 90) targetPct = 90;
    next.targetPercent = targetPct;

    auto tempGestureStr = WindhawkUtils::StringSetting::make(L"tempCleaner.mouseGesture");
    next.tempGestureEnabled = wcscmp(tempGestureStr.get(), L"Disabled") != 0;

    auto tempScopeStr = WindhawkUtils::StringSetting::make(L"tempCleaner.mouseScope");
    next.tempGestureScope = ParseTempMouseScope(tempScopeStr.get());

    int minAgeMin = Wh_GetIntSetting(L"tempCleaner.minFileAgeMinutes");
    if (minAgeMin < 0) minAgeMin = 0;
    if (minAgeMin > 10080) minAgeMin = 10080;
    next.minFileAgeMs = static_cast<uint64_t>(minAgeMin) * 60000ULL;

    int cooldownSec = Wh_GetIntSetting(L"tempCleaner.cooldownSeconds");
    if (cooldownSec < 5) cooldownSec = 5;
    if (cooldownSec > 3600) cooldownSec = 3600;
    next.cooldownMs = static_cast<uint64_t>(cooldownSec) * 1000ULL;

    next.cleanWindowsTemp = Wh_GetIntSetting(L"tempCleaner.cleanWindowsTemp") != 0;

    for (int i = 0;; i++) {
        auto pat = WindhawkUtils::StringSetting::make(L"tempCleaner.excludePatterns[%d]", i);
        if (!*pat.get()) {
            break;
        }
        std::wstring cleanPat = NormalizeExcludePattern(pat.get());
        if (!cleanPat.empty()) {
            next.excludePatterns.push_back(std::move(cleanPat));
        }
    }

    // 3. Recycle Bin Cleaner
    next.rbSizeThresholdEnabled = Wh_GetIntSetting(L"recycleBinCleaner.sizeThresholdEnabled") != 0;
    int rawRbThresh = Wh_GetIntSetting(L"recycleBinCleaner.sizeThresholdValue");
    if (rawRbThresh < 1) rawRbThresh = 1;
    auto rbUnit = WindhawkUtils::StringSetting::make(L"recycleBinCleaner.sizeThresholdUnit");
    next.rbSizeThresholdBytes = MultiplyClampU64(static_cast<uint64_t>(rawRbThresh), UnitToMultiplier(rbUnit.get()));

    next.rbCountThresholdEnabled = Wh_GetIntSetting(L"recycleBinCleaner.countThresholdEnabled") != 0;
    int rawRbMaxItems = Wh_GetIntSetting(L"recycleBinCleaner.maxItems");
    if (rawRbMaxItems < 1) rawRbMaxItems = 1;
    next.rbMaxItems = static_cast<uint64_t>(rawRbMaxItems);

    next.rbRetentionEnabled = Wh_GetIntSetting(L"recycleBinCleaner.retentionEnabled") != 0;
    int rawRbRetVal = Wh_GetIntSetting(L"recycleBinCleaner.retentionValue");
    if (rawRbRetVal < 1) rawRbRetVal = 1;
    auto rbRetUnit = WindhawkUtils::StringSetting::make(L"recycleBinCleaner.retentionUnit");
    uint64_t rbRetMs = MultiplyClampU64(
        static_cast<uint64_t>(rawRbRetVal), TimeUnitToMs(rbRetUnit.get(), 86400ULL * 1000ULL));
    if (rbRetMs < 60000ULL) rbRetMs = 60000ULL;
    next.rbRetentionAgeMs = rbRetMs;

    next.rbFullSweep = Wh_GetIntSetting(L"recycleBinCleaner.fullSweep") != 0;
    int rbTargetPct = Wh_GetIntSetting(L"recycleBinCleaner.targetPercent");
    if (rbTargetPct < 10) rbTargetPct = 10;
    if (rbTargetPct > 90) rbTargetPct = 90;
    next.rbTargetPercent = rbTargetPct;

    int rbMinAgeMin = Wh_GetIntSetting(L"recycleBinCleaner.minItemAgeMinutes");
    if (rbMinAgeMin < 0) rbMinAgeMin = 0;
    if (rbMinAgeMin > 525600) rbMinAgeMin = 525600;
    next.rbMinAgeMs = static_cast<uint64_t>(rbMinAgeMin) * 60000ULL;

    int rbCoolSec = Wh_GetIntSetting(L"recycleBinCleaner.cooldownSeconds");
    if (rbCoolSec < 5) rbCoolSec = 5;
    if (rbCoolSec > 3600) rbCoolSec = 3600;
    next.rbCooldownMs = static_cast<uint64_t>(rbCoolSec) * 1000ULL;

    auto gestureStr = WindhawkUtils::StringSetting::make(L"recycleBinCleaner.mouseGesture");
    next.rbMouseClicksRequired = ParseMouseGestureClicks(gestureStr.get());

    auto scopeStr = WindhawkUtils::StringSetting::make(L"recycleBinCleaner.mouseScope");
    next.rbMouseScope = ParseMouseScope(scopeStr.get());

    next.rbPlaySound = Wh_GetIntSetting(L"recycleBinCleaner.playSound") != 0;

    // 4. Universal, Toast & Safety
    next.startupCleanTemp = Wh_GetIntSetting(L"universalSafety.cleanTempOnStart") != 0;
    next.startupCleanRecycle = Wh_GetIntSetting(L"universalSafety.cleanRecycleBinOnStart") != 0;
    int startDelay = Wh_GetIntSetting(L"universalSafety.startupDelaySeconds");
    if (startDelay < 5) startDelay = 5;
    if (startDelay > 300) startDelay = 300;
    next.startupDelayMs = static_cast<uint64_t>(startDelay) * 1000ULL;
    next.startupFullSweep = Wh_GetIntSetting(L"universalSafety.startupFullSweep") != 0;

    next.emergencyEnabled = Wh_GetIntSetting(L"universalSafety.emergencyEnabled") != 0;
    int rawFloor = Wh_GetIntSetting(L"universalSafety.freeSpaceValue");
    if (rawFloor < 1) rawFloor = 1;
    auto floorUnit = WindhawkUtils::StringSetting::make(L"universalSafety.freeSpaceUnit");
    next.emergencyFloorBytes = MultiplyClampU64(static_cast<uint64_t>(rawFloor), UnitToMultiplier(floorUnit.get()));

    int emergSec = Wh_GetIntSetting(L"universalSafety.emergencyCheckSeconds");
    if (emergSec < 15) emergSec = 15;
    if (emergSec > 3600) emergSec = 3600;
    next.emergencyCheckMs = static_cast<uint64_t>(emergSec) * 1000ULL;

    next.cleanRecycleOnEmergency = Wh_GetIntSetting(L"universalSafety.cleanRecycleOnEmergency") != 0;
    next.emergencyBypassDeferral = Wh_GetIntSetting(L"universalSafety.bypassDeferralOnEmergency") != 0;
    next.deferOnFullscreen = Wh_GetIntSetting(L"universalSafety.deferOnFullscreen") != 0;
    next.deferOnBattery = Wh_GetIntSetting(L"universalSafety.deferOnBattery") != 0;

    int maxIntMs = Wh_GetIntSetting(L"universalSafety.mouseMaxIntervalMs");
    if (maxIntMs < 150) maxIntMs = 150;
    if (maxIntMs > 800) maxIntMs = 800;
    next.rbMouseMaxIntervalMs = static_cast<uint64_t>(maxIntMs);
    next.tempGestureMaxIntervalMs = static_cast<uint64_t>(maxIntMs);

    next.toastEnabled = Wh_GetIntSetting(L"universalSafety.toastEnabled") != 0;
    auto toastScopeStr = WindhawkUtils::StringSetting::make(L"universalSafety.toastTriggerScope");
    next.toastShowOnAuto = wcscmp(toastScopeStr.get(), L"ManualOnly") != 0;
    int toastDur = Wh_GetIntSetting(L"universalSafety.toastDurationMs");
    if (toastDur < 1200) toastDur = 1200;
    if (toastDur > 10000) toastDur = 10000;
    next.toastDurationMs = static_cast<uint32_t>(toastDur);

    auto capStr = WindhawkUtils::StringSetting::make(L"universalSafety.screenshotBehavior");
    next.toastExcludeFromCapture = wcscmp(capStr.get(), L"ShowInCapture") != 0;

    next.dryRun = Wh_GetIntSetting(L"universalSafety.dryRun") != 0;

    g_mouseClicksRequired.store(next.rbMouseClicksRequired, std::memory_order_relaxed);
    g_mouseTriggerScope.store(next.rbMouseScope, std::memory_order_relaxed);
    g_mouseMaxIntervalMs.store(static_cast<uint32_t>(next.rbMouseMaxIntervalMs), std::memory_order_relaxed);
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
    g_stopRequested.store(false, std::memory_order_relaxed);

    g_wakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_uiReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_wakeEvent || !g_uiReadyEvent) {
        return FALSE;
    }

    g_uiThread = CreateThread(nullptr, 0, UiThreadProc, nullptr, 0, nullptr);
    if (g_uiThread) {
        WaitForSingleObject(g_uiReadyEvent, INFINITE);
    }

    LoadSettings();

    g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, nullptr);
    if (!g_workerThread) {
        SyncMouseHookThread(false);
        if (g_uiMsgHwnd) {
            PostMessageW(g_uiMsgHwnd, WM_QUIT, 0, 0);
        }
        if (g_uiThread) {
            WaitForSingleObject(g_uiThread, INFINITE);
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
        WaitForSingleObject(g_workerThread, INFINITE);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }
    if (g_uiMsgHwnd) {
        PostMessageW(g_uiMsgHwnd, WM_QUIT, 0, 0);
    }
    if (g_uiThread) {
        WaitForSingleObject(g_uiThread, INFINITE);
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
    WhTool_ModUninit();
    ExitProcess(0);
}
