// ==WindhawkMod==
// @id              auto-refresh-rate
// @name            Auto Refresh Rate
// @description     Automatically switch monitor refresh rates based on AC/battery power, fullscreen games, foreground apps, and docking.
// @version         1.0.0
// @author          roypriyanshu02
// @github          https://github.com/roypriyanshu02
// @homepage        https://github.com/roypriyanshu02/windhawk-auto-refresh-rate
// @include         explorer.exe
// @architecture    x86-64
// @architecture    arm64
// @architecture    x86
// @compilerOptions -lole32 -lgdi32 -luuid -luser32 -ladvapi32 -lpowrprof
// @license         MIT
// ==/WindhawkMod==

// Source code is published under the MIT License.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/roypriyanshu02/windhawk-auto-refresh-rate/issues

// ==WindhawkModReadme==
/*
# Auto Refresh Rate

Display refresh rate automation for Windows 10 (1809+) and Windows 11 on x86-64, ARM64, and x86.

Auto Refresh Rate is a lightweight Windhawk mod that automates display refresh rates based on power state, fullscreen games, foreground apps, and docking.

It runs in-process inside `explorer.exe`. Unplug your charger, and your panel drops to 60 Hz to stretch battery runtime. Plug in or launch a game, and it immediately boosts back to full speed. Because it hooks native Windows power broadcasts (`RegisterPowerSettingNotification`) without background polling loops, idle CPU usage stays at 0%.

Configure target rates, application rules, and hotkeys in the **Settings** tab above.

_Tip: To test immediately without unplugging power, toggle Windows Energy Saver in Quick Settings (Win + A), or enable the cycle hotkey (Win + Ctrl + R) in the Settings tab above._

## Key features

* **Power source switching:** Run at maximum refresh rate on AC power, drop to 60 Hz (or lowest supported rate) on battery, and lower further when Windows Energy Saver turns on.
* **Fullscreen game boost:** Detects borderless and exclusive fullscreen games to automatically switch into maximum refresh rate.
* **Per-application rules:** Set custom refresh rates for creative applications and lock media players to battery refresh rates. Semicolon-separated executable names (`cs2; blender; vlc`); `.exe` extensions are optional.
* **Protected applications:** Locks refresh rate switching while capture or presentation tools run (e.g. OBS Studio, PowerPoint) to avoid display stutter.
* **Smart laptop docking:** When running on battery with external monitors connected, lowers only the built-in laptop screen while keeping desktop monitors at full refresh rate.
* **Quiet transitions:** Waits for keyboard and mouse activity to rest before lowering refresh rates, with an anti-flicker cooldown between switches.
* **Night schedule:** Enforces battery refresh rates during designated night hours to reduce eye strain.
* **On-screen display badge:** Click-through layered notification confirms rate changes without stealing window focus; includes DPI scaling support.
* **Global cycle hotkey:** Step sequentially through each supported display frequency before wrapping back to auto mode (`Win + Ctrl + R`; disabled by default, enable under Settings).

## Evaluation priority

When multiple conditions match simultaneously, target refresh rates resolve in the following order:

1. **Manual hotkey lock:** Overrides all automated rules until unlocked or cycled back to auto.
2. **Protected applications:** Pauses switching while capture or presentation tools run (e.g. OBS Studio, PowerPoint).
3. **Windows Energy Saver:** Drops to power-saving rate immediately when battery saver triggers.
4. **Foreground application rules & game boost:** Applies matched high/low app rules or fullscreen game boost.
5. **Night schedule:** Applies battery refresh rate during scheduled evening hours.
6. **Power source baseline:** Applies plugged-in rate on AC or on-battery rate on DC power.

## Tips & troubleshooting

* **Display flicker:** If your panel flashes during transitions, increase **Anti-flicker cooldown** (default: 3 seconds) under the **Display & transitions** settings tab.
* **External monitors on battery:** Smart docking keeps external desktop monitors at full refresh rate while on battery. To switch all connected displays, set **Target displays** to `all`.
* **Live diagnostics:** Open the **Log** tab in Windhawk to inspect real-time display adjustments, AC/battery transitions, and active process detection.

## Changelog

### Version 1.0.0 (2026-09-21)
* Initial release.
* Automatic display refresh rate switching on AC and battery power transitions.
* Windows Energy Saver integration with instant frequency step-down.
* Borderless and fullscreen game detection with automatic high-refresh boost.
* Per-application refresh rate rules and screen capture protection (`obs64`, `powerpnt`).
* Smart laptop docking: maintains external monitor speeds while lowering internal panel.
* On-screen display notification badge with DPI awareness and click-through transparency.
* Global cycle hotkey (`Win + Ctrl + R`) to step through supported frequencies.
* Purely event-driven Win32 architecture with zero idle CPU overhead.

## Feedback & source code

For bug reports, feature requests, and source code, visit the **[GitHub repository](https://github.com/roypriyanshu02/windhawk-auto-refresh-rate)**.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- PowerAndBattery:
    - ChargeSwitchingEnabled: true
      $name: "Power source switching"
      $description: "Switch refresh rates when connecting or disconnecting power."
    - PluggedInRate: max
      $name: "Plugged-in rate"
      $description: "Refresh rate when connected to AC power."
      $options:
        - max: "Highest supported (recommended)"
        - "60": "60 Hz"
        - custom: "Custom rate"
    - CustomPluggedInRate: 144
      $name: "Custom plugged-in rate"
      $description: "Target refresh rate in Hz."
    - OnBatteryRate: "60"
      $name: "On-battery rate"
      $description: "Refresh rate while running on battery."
      $options:
        - "60": "60 Hz (recommended)"
        - min: "Lowest supported"
        - match_ac: "Match plugged-in rate"
        - custom: "Custom rate"
    - CustomBatteryRate: 60
      $name: "Custom on-battery rate"
      $description: "Target refresh rate in Hz."
    - EnergySaverEnabled: true
      $name: "Energy Saver sync"
      $description: "Lower refresh rate while Windows Energy Saver is active."
    - EnergySaverRate: "60"
      $name: "Energy Saver rate"
      $description: "Refresh rate while Energy Saver is active."
      $options:
        - "60": "60 Hz (recommended)"
        - min: "Lowest supported"
        - custom: "Custom rate"
    - CustomEnergySaverRate: 60
      $name: "Custom Energy Saver rate"
      $description: "Target refresh rate in Hz."
  $name: "Power & battery"
  $description: "Refresh rate automation for AC power, battery, and Windows Energy Saver."

- GamingAndApps:
    - AutoGameBoost: true
      $name: "Fullscreen game boost"
      $description: "Boost to highest supported refresh rate in borderless and fullscreen games."
    - AppRulesEnabled: false
      $name: "Per-app refresh rates"
      $description: "Apply custom refresh rates when designated apps are focused."
    - HighRefreshApps: "cs2; valorant; overwatch; cyberpunk2077; blender"
      $name: "High-refresh apps"
      $description: "Apps that boost to maximum refresh rate when focused. Separate names with semicolons (e.g. cs2; blender)."
    - LowRefreshApps: "vlc; mpc-hc64; netflix; acrobat"
      $name: "Low-refresh apps"
      $description: "Apps locked to battery refresh rate when focused. Separate names with semicolons (e.g. vlc; acrobat)."
    - InhibitAppsEnabled: true
      $name: "Protected apps lock"
      $description: "Pause display switching while capture or presentation tools run."
    - InhibitApps: "obs64; obs; streamlabs; powerpnt"
      $name: "Protected apps"
      $description: "Apps that block refresh rate changes while running. Separate names with semicolons (e.g. obs64; powerpnt)."
  $name: "Gaming & applications"
  $description: "Fullscreen game boost, per-app rules, and screen capture protection."

- Schedule:
    - TimeScheduleEnabled: false
      $name: "Night schedule"
      $description: "Lower refresh rate during scheduled hours to reduce eye strain."
    - ScheduleStart: "22:00"
      $name: "Start time"
      $description: "Schedule start time in 24h or 12h format (e.g. 22:00 or 10:00 PM)."
    - ScheduleEnd: "07:00"
      $name: "End time"
      $description: "Schedule end time in 24h or 12h format (e.g. 07:00 or 7:00 AM)."
  $name: "Night schedule"
  $description: "Scheduled refresh rate limits for late night hours."

- DisplayAndTransitions:
    - TargetDisplays: primary
      $name: "Target displays"
      $description: "Displays to adjust when switching refresh rates."
      $options:
        - primary: "Primary display only"
        - all: "All connected displays"
    - SmartDockingEnabled: true
      $name: "Smart laptop docking"
      $description: "Keep external monitors at high refresh rate while lowering only the internal laptop screen on battery."
    - QuietSwitchEnabled: true
      $name: "Idle-only switching"
      $description: "Wait for keyboard and mouse input to pause before lowering refresh rate."
    - AntiFlickerCooldown: 3
      $name: "Anti-flicker cooldown"
      $description: "Minimum seconds to wait between display switches to avoid rapid panel flashing."
  $name: "Display & transitions"
  $description: "Target display selection, laptop docking, and transition timing."

- ShortcutsAndNotifications:
    - OsdBadgeEnabled: true
      $name: "On-screen notification badge"
      $description: "Show a temporary on-screen badge when the refresh rate changes."
    - GlobalHotkeyEnabled: false
      $name: "Cycle hotkey"
      $description: "Cycle through supported refresh rates or return to auto mode via hotkey."
    - GlobalHotkey: "Win+Ctrl+R"
      $name: "Hotkey combination"
      $description: "Key combination to cycle rates (e.g. Win+Ctrl+R, Ctrl+Alt+R)."
    - VerboseLogging: true
      $name: "Verbose event logging"
      $description: "Log power transitions and refresh rate events to the Windhawk log."
  $name: "Shortcuts & notifications"
  $description: "On-screen badge, keyboard shortcuts, and event logging."
*/
// ==/WindhawkModSettings==

#ifndef WINVER
#define WINVER 0x0A00
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif

#include <windows.h>
#include <objbase.h>
#include <winuser.h>
#include <powrprof.h>
#include <tlhelp32.h>
#include <vector>
#include <string>
#include <string_view>
#include <array>
#include <algorithm>
#include <cmath>
#include <optional>
#include <atomic>

// ============================================================================
// RAII Utilities for Win32 and GDI Resources
// ============================================================================

struct ScopedHandle {
    HANDLE m_h = nullptr;
    constexpr ScopedHandle(HANDLE h = nullptr) noexcept : m_h(h) {}
    ~ScopedHandle() noexcept {
        if (isValid()) {
            ::CloseHandle(m_h);
        }
    }
    ScopedHandle(const ScopedHandle&) = delete;
    ScopedHandle& operator=(const ScopedHandle&) = delete;
    ScopedHandle(ScopedHandle&& other) noexcept : m_h(other.m_h) { other.m_h = nullptr; }
    ScopedHandle& operator=(ScopedHandle&& other) noexcept {
        if (this != &other) {
            if (isValid()) ::CloseHandle(m_h);
            m_h = other.m_h;
            other.m_h = nullptr;
        }
        return *this;
    }
    [[nodiscard]] HANDLE get() const noexcept { return m_h; }
    [[nodiscard]] bool isValid() const noexcept { return m_h != nullptr && m_h != INVALID_HANDLE_VALUE; }
    [[nodiscard]] explicit operator bool() const noexcept { return isValid(); }
};

struct ScopedGdiObject {
    HGDIOBJ m_obj = nullptr;
    explicit ScopedGdiObject(HGDIOBJ o) noexcept : m_obj(o) {}
    ~ScopedGdiObject() noexcept {
        if (m_obj) {
            ::DeleteObject(m_obj);
        }
    }
    ScopedGdiObject(const ScopedGdiObject&) = delete;
    ScopedGdiObject& operator=(const ScopedGdiObject&) = delete;
    ScopedGdiObject(ScopedGdiObject&& other) noexcept : m_obj(other.m_obj) { other.m_obj = nullptr; }
    ScopedGdiObject& operator=(ScopedGdiObject&& other) noexcept {
        if (this != &other) {
            if (m_obj) ::DeleteObject(m_obj);
            m_obj = other.m_obj;
            other.m_obj = nullptr;
        }
        return *this;
    }
    [[nodiscard]] HGDIOBJ get() const noexcept { return m_obj; }
};

struct ScopedRegKey {
    HKEY m_k = nullptr;
    constexpr ScopedRegKey(HKEY k = nullptr) noexcept : m_k(k) {}
    ~ScopedRegKey() noexcept {
        if (m_k) {
            ::RegCloseKey(m_k);
        }
    }
    ScopedRegKey(const ScopedRegKey&) = delete;
    ScopedRegKey& operator=(const ScopedRegKey&) = delete;
    ScopedRegKey(ScopedRegKey&& other) noexcept : m_k(other.m_k) { other.m_k = nullptr; }
    ScopedRegKey& operator=(ScopedRegKey&& other) noexcept {
        if (this != &other) {
            if (m_k) ::RegCloseKey(m_k);
            m_k = other.m_k;
            other.m_k = nullptr;
        }
        return *this;
    }
    [[nodiscard]] HKEY get() const noexcept { return m_k; }
    [[nodiscard]] HKEY* addressof() noexcept { return &m_k; }
    [[nodiscard]] explicit operator bool() const noexcept { return m_k != nullptr; }
};

struct ScopedDcState {
    HDC m_hdc = nullptr;
    int m_state = 0;
    explicit ScopedDcState(HDC hdc) noexcept : m_hdc(hdc), m_state(hdc ? ::SaveDC(hdc) : 0) {}
    ~ScopedDcState() noexcept {
        if (m_hdc && m_state != 0) {
            ::RestoreDC(m_hdc, m_state);
        }
    }
    ScopedDcState(const ScopedDcState&) = delete;
    ScopedDcState& operator=(const ScopedDcState&) = delete;
};

// GUID Definitions
static constexpr GUID GUID_NULL_LOCAL = {
    0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
};
static constexpr GUID GUID_ACDC_POWER_SOURCE_LOCAL = {
    0x5D3E9A59, 0xE9D5, 0x4B00, { 0xA6, 0xBD, 0xFF, 0x34, 0xFF, 0x51, 0x65, 0x48 }
};
static constexpr GUID GUID_BATTERY_PERCENTAGE_REMAINING_LOCAL = {
    0xA7AD8041, 0xB45A, 0x4CAE, { 0x87, 0xA3, 0xEE, 0xCB, 0xB4, 0x68, 0xA9, 0xE1 }
};
static constexpr GUID GUID_POWER_SAVING_STATUS_LOCAL = {
    0xE00958C0, 0xC213, 0x4ACE, { 0xAC, 0x77, 0xFE, 0xCC, 0xED, 0x2E, 0xEE, 0xA5 }
};
static constexpr GUID GUID_POWERSCHEME_PERSONALITY_LOCAL = {
    0x245D8541, 0x3943, 0x4422, { 0xB0, 0x25, 0x13, 0xA7, 0x84, 0xF6, 0x79, 0xB7 }
};
static constexpr GUID GUID_MIN_POWER_SAVINGS_LOCAL = {
    0x8C5E7FDA, 0xE8BF, 0x4A96, { 0x9A, 0x85, 0xA6, 0xE2, 0x3A, 0x8C, 0x63, 0x5C }
};
static constexpr GUID GUID_OVERLAY_BEST_PERFORMANCE = {
    0xDED574B5, 0x45A0, 0x4F42, { 0x87, 0x37, 0x46, 0x34, 0x5C, 0x09, 0xC2, 0x38 }
};

#ifndef PBT_POWERSETTINGCHANGE
#define PBT_POWERSETTINGCHANGE 0x8013
#endif
#ifndef CDS_NORESET
#define CDS_NORESET 0x10000000
#endif
#ifndef QDC_ONLY_ACTIVE_PATHS
#define QDC_ONLY_ACTIVE_PATHS 0x00000002
#endif
#ifndef DISPLAYCONFIG_OUTPUT_TECHNOLOGY_INTERNAL
#define DISPLAYCONFIG_OUTPUT_TECHNOLOGY_INTERNAL 0x80000000
#endif
#ifndef DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DISPLAYPORT_EMBEDDED
#define DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DISPLAYPORT_EMBEDDED 11
#endif
#ifndef DISPLAYCONFIG_OUTPUT_TECHNOLOGY_UDI_EMBEDDED
#define DISPLAYCONFIG_OUTPUT_TECHNOLOGY_UDI_EMBEDDED 13
#endif
#ifndef DISPLAYCONFIG_OUTPUT_TECHNOLOGY_LVDS
#define DISPLAYCONFIG_OUTPUT_TECHNOLOGY_LVDS 6
#endif
#ifndef DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME
#define DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME 1
#endif
#ifndef EDS_ROTATEDMODE
#define EDS_ROTATEDMODE 0x00000004
#endif

// Custom window messages and timer identifiers
constexpr UINT WM_APP_REAPPLY_POWER_STATE   = WM_APP + 101;
constexpr UINT WM_APP_FOREGROUND_CHANGED    = WM_APP + 102;
constexpr UINT WM_APP_SETTINGS_CHANGED      = WM_APP + 103;

constexpr UINT_PTR TIMER_ID_POWER_DEBOUNCE      = 1;
constexpr UINT_PTR TIMER_ID_RESUME_SYNC         = 2;
constexpr UINT_PTR TIMER_ID_DISPLAY_CHANGE      = 3;
constexpr UINT_PTR TIMER_ID_TIME_CHECK          = 4;
constexpr UINT_PTR TIMER_ID_FOREGROUND_DEBOUNCE = 6;
constexpr UINT_PTR TIMER_ID_QUIET_SWITCH        = 7;
constexpr UINT_PTR TIMER_ID_COOLDOWN_SWITCH     = 8;
constexpr UINT_PTR TIMER_ID_INHIBIT_RECHECK     = 9;

constexpr UINT_PTR TIMER_ID_OSD_HOLD            = 101;
constexpr UINT_PTR TIMER_ID_OSD_FADE            = 102;

constexpr int HOTKEY_ID_CYCLE                   = 0x415A;

constexpr DWORD DEBOUNCE_DELAY_MS               = 350;
constexpr DWORD RESUME_DELAY_MS                 = 1000;
constexpr DWORD DISPLAY_CHANGE_DELAY_MS         = 500;
constexpr DWORD FOREGROUND_DEBOUNCE_MS          = 100;
constexpr DWORD QUIET_SWITCH_TIMEOUT_MS         = 2000;
constexpr DWORD TIME_CHECK_INTERVAL_MS          = 30000;
constexpr DWORD INHIBIT_RECHECK_INTERVAL_MS     = 2500;

// ============================================================================
// Mod Configuration & State
// ============================================================================

struct ModSettings {
    bool chargeSwitchingEnabled = true;
    DWORD targetAC = 0; // 0 = automatic maximum supported
    DWORD targetDC = 60; // 1 = lowest, 0 = match AC, or explicit Hz

    bool energySaverEnabled = true;
    std::wstring energySaverAction = L"60"; // "60", "min", or "custom"
    DWORD targetEnergySaver = 60; // if energySaverAction is custom

    bool autoGameBoost = true;
    bool appRulesEnabled = false;
    std::wstring highRefreshApps = L"cs2; valorant; overwatch; cyberpunk2077; blender";
    std::wstring lowRefreshApps = L"vlc; mpc-hc64; netflix; acrobat";
    bool smartDockingEnabled = true;

    bool quietSwitchEnabled = true;
    bool inhibitAppsEnabled = true;
    std::wstring inhibitApps = L"obs64; obs; streamlabs; powerpnt";
    DWORD switchCooldownMs = 3000;

    bool osdEnabled = true;
    DWORD osdDurationMs = 1500;
    bool globalHotkeyEnabled = false;
    std::wstring globalHotkey = L"Win+Ctrl+R";
    UINT hotkeyModifiers = MOD_WIN | MOD_CONTROL;
    UINT hotkeyVk = 'R';

    bool targetDisplayAll = false;
    bool timeScheduleEnabled = false;
    std::wstring scheduleStart = L"22:00";
    std::wstring scheduleEnd = L"07:00";
    bool verboseLogging = true;
};

struct PowerStateSnapshot {
    bool isAC = true;
    BYTE batteryPercent = 100;
    bool isBatterySaverActive = false;
    GUID powerScheme = GUID_NULL_LOCAL;
};

static ModSettings g_settings;
static PowerStateSnapshot g_state;
static std::vector<std::wstring> g_parsedHighRefreshApps;
static std::vector<std::wstring> g_parsedLowRefreshApps;
static std::vector<std::wstring> g_parsedInhibitApps;
static ULONGLONG g_lastSuccessfulSwitchTick = 0;

static HANDLE g_hMutex = nullptr;
static HANDLE g_hThread = nullptr;
static std::atomic<HWND> g_hWnd{nullptr};
static HWND g_hOsdWnd = nullptr;
static HWINEVENTHOOK g_hWinEventHook = nullptr;

static constexpr std::array<const GUID*, 4> g_powerGuids = {
    &GUID_ACDC_POWER_SOURCE_LOCAL,
    &GUID_BATTERY_PERCENTAGE_REMAINING_LOCAL,
    &GUID_POWER_SAVING_STATUS_LOCAL,
    &GUID_POWERSCHEME_PERSONALITY_LOCAL
};
static std::array<HPOWERNOTIFY, 4> g_hPowerNotify = {};

static constexpr WCHAR g_szClassName[] = L"Windhawk_AutoRefreshRate_MsgWnd";
static constexpr WCHAR g_szOsdClassName[] = L"Windhawk_AutoRefreshRate_OsdWnd";
static constexpr WCHAR g_szMutexName[] = L"Local\\Windhawk_AutoRefreshRate_PowerMonitor";

static bool g_manualOverrideActive = false;
static DWORD g_manualOverrideHz = 0;

static DWORD g_osdCurrentHz = 0;
static std::wstring g_osdCurrentReason;
static BYTE g_osdAlpha = 0;

enum class OsdState { Hidden, Holding, Fading };
static OsdState g_osdState = OsdState::Hidden;
static std::atomic<bool> g_foregroundPending{false};

void SynchronizeAndApplyPolicy(bool forceOsd = false, const std::wstring& forcedBrief = L"");
void ShowOsdBadge(DWORD hz, const std::wstring& reasonBrief);
void LoadSettings();

// ============================================================================
// Settings & String Helpers
// ============================================================================

[[nodiscard]] inline int ReadIntSettingSafe(PCWSTR key, int defaultValue = 0) {
    PCWSTR s = Wh_GetStringSetting(key);
    if (!s) return defaultValue;
    if (*s == L'\0') {
        Wh_FreeStringSetting(s);
        return defaultValue;
    }
    Wh_FreeStringSetting(s);
    return Wh_GetIntSetting(key);
}

[[nodiscard]] inline bool ReadBoolSettingSafe(PCWSTR key, bool defaultValue = false) {
    PCWSTR s = Wh_GetStringSetting(key);
    if (!s) return defaultValue;
    if (*s == L'\0') {
        Wh_FreeStringSetting(s);
        return defaultValue;
    }
    Wh_FreeStringSetting(s);
    return Wh_GetIntSetting(key) != 0;
}

[[nodiscard]] inline std::wstring ReadStringSettingSafe(PCWSTR key, const std::wstring& defaultValue) {
    PCWSTR s = Wh_GetStringSetting(key);
    if (s) {
        std::wstring res = (*s != L'\0') ? s : defaultValue;
        Wh_FreeStringSetting(s);
        return res;
    }
    return defaultValue;
}

[[nodiscard]] inline std::optional<int> ParsePositiveInt(std::wstring_view sv) noexcept {
    if (sv.empty() || sv.length() > 6) return std::nullopt;
    int val = 0;
    for (wchar_t c : sv) {
        if (c < L'0' || c > L'9') return std::nullopt;
        val = val * 10 + (c - L'0');
    }
    return val;
}

[[nodiscard]] constexpr std::wstring_view Trim(std::wstring_view sv) noexcept {
    constexpr std::wstring_view whitespace = L" \t\r\n\"'";
    const auto start = sv.find_first_not_of(whitespace);
    if (start == std::wstring_view::npos) return {};
    const auto end = sv.find_last_not_of(whitespace);
    return sv.substr(start, end - start + 1);
}

[[nodiscard]] inline bool EqualsIgnoreCase(std::wstring_view a, std::wstring_view b) noexcept {
    if (a.size() != b.size()) return false;
    if (a.empty()) return true;
    return _wcsnicmp(a.data(), b.data(), a.size()) == 0;
}

[[nodiscard]] std::wstring FormatAppNameForDisplay(std::wstring_view appName) {
    auto slash = appName.find_last_of(L"\\/");
    if (slash != std::wstring_view::npos) {
        appName = appName.substr(slash + 1);
    }
    if (appName.length() > 4 && EqualsIgnoreCase(appName.substr(appName.length() - 4), L".exe")) {
        appName = appName.substr(0, appName.length() - 4);
    }
    return std::wstring(appName);
}

[[nodiscard]] std::vector<std::wstring> ParseAppList(std::wstring_view listStr) {
    std::vector<std::wstring> result;
    size_t start = 0;
    while (start < listStr.length()) {
        size_t end = listStr.find_first_of(L";,", start);
        if (end == std::wstring_view::npos) end = listStr.length();
        std::wstring_view token = Trim(listStr.substr(start, end - start));
        start = end + 1;
        if (token.empty()) continue;

        auto slash = token.find_last_of(L"\\/");
        if (slash != std::wstring_view::npos) token = token.substr(slash + 1);
        if (token.empty()) continue;

        std::wstring clean(token);
        for (auto& c : clean) c = static_cast<wchar_t>(::towlower(c));
        if (clean.length() < 4 || clean.compare(clean.length() - 4, 4, L".exe") != 0) {
            clean += L".exe";
        }
        if (std::find(result.begin(), result.end(), clean) == result.end()) {
            result.push_back(std::move(clean));
        }
    }
    return result;
}

[[nodiscard]] bool IsAppInList(std::wstring_view appName, const std::vector<std::wstring>& list) noexcept {
    auto slash = appName.find_last_of(L"\\/");
    if (slash != std::wstring_view::npos) {
        appName = appName.substr(slash + 1);
    }
    if (appName.empty()) return false;

    for (const auto& item : list) {
        std::wstring_view itemV = item;
        if (EqualsIgnoreCase(appName, itemV)) return true;
        if (itemV.length() > 4 && EqualsIgnoreCase(itemV.substr(itemV.length() - 4), L".exe")) {
            if (EqualsIgnoreCase(appName, itemV.substr(0, itemV.length() - 4))) return true;
        }
    }
    return false;
}

struct TimeOfDay {
    int hour = 0;
    int minute = 0;
};

[[nodiscard]] std::optional<TimeOfDay> ParseFlexibleTime(std::wstring_view rawInput) noexcept {
    std::wstring_view s = Trim(rawInput);
    if (s.empty() || s.length() > 32) return std::nullopt;

    bool isPM = false;
    bool hasAmPm = false;

    if (s.length() >= 2) {
        wchar_t c1 = static_cast<wchar_t>(::towlower(s[s.length() - 2]));
        wchar_t c2 = static_cast<wchar_t>(::towlower(s[s.length() - 1]));
        if (c1 == L'p' && c2 == L'm') {
            isPM = true;
            hasAmPm = true;
            s = Trim(s.substr(0, s.length() - 2));
        } else if (c1 == L'a' && c2 == L'm') {
            hasAmPm = true;
            s = Trim(s.substr(0, s.length() - 2));
        }
    }

    std::wstring buf(s);
    int h = -1, m = 0;
    int matched = swscanf_s(buf.c_str(), L"%d:%d", &h, &m);
    if (matched < 1) {
        return std::nullopt;
    }
    if (matched == 1) {
        m = 0;
    }

    if (hasAmPm) {
        if (h < 1 || h > 12) return std::nullopt;
        if (isPM && h < 12) h += 12;
        else if (!isPM && h == 12) h = 0;
    }

    if (h >= 0 && h <= 23 && m >= 0 && m <= 59) {
        return TimeOfDay{ h, m };
    }
    return std::nullopt;
}

[[nodiscard]] bool IsCurrentTimeInSchedule(std::wstring_view startStr, std::wstring_view endStr) noexcept {
    const auto startOpt = ParseFlexibleTime(startStr);
    const auto endOpt = ParseFlexibleTime(endStr);
    if (!startOpt || !endOpt) return false;

    SYSTEMTIME st = {};
    GetLocalTime(&st);
    const int cur = st.wHour * 60 + st.wMinute;
    const int start = startOpt->hour * 60 + startOpt->minute;
    const int end = endOpt->hour * 60 + endOpt->minute;

    if (start == end) return false;
    return (start < end) ? (cur >= start && cur < end) : (cur >= start || cur < end);
}

// ============================================================================
// Hotkey Parsing & Configuration
// ============================================================================

[[nodiscard]] bool ParseHotkeyString(std::wstring_view rawInput, UINT& outModifiers, UINT& outVk) noexcept {
    outModifiers = 0;
    outVk = 0;

    std::wstring_view s = Trim(rawInput);
    if (s.empty()) return false;

    size_t start = 0;
    while (start < s.length()) {
        size_t end = s.find_first_of(L"+- \t", start);
        if (end == std::wstring_view::npos) end = s.length();

        std::wstring_view token = Trim(s.substr(start, end - start));
        start = end + 1;
        if (token.empty()) continue;

        if (EqualsIgnoreCase(token, L"ctrl") || EqualsIgnoreCase(token, L"control")) {
            outModifiers |= MOD_CONTROL;
        } else if (EqualsIgnoreCase(token, L"alt")) {
            outModifiers |= MOD_ALT;
        } else if (EqualsIgnoreCase(token, L"shift")) {
            outModifiers |= MOD_SHIFT;
        } else if (EqualsIgnoreCase(token, L"win") || EqualsIgnoreCase(token, L"windows") || EqualsIgnoreCase(token, L"super")) {
            outModifiers |= MOD_WIN;
        } else {
            if (outVk != 0) {
                // Reject multiple non-modifier keys (e.g. Ctrl+R+X)
                return false;
            }

            UINT vk = 0;
            if (token.length() == 1) {
                wchar_t c = token[0];
                if ((c >= L'a' && c <= L'z') || (c >= L'A' && c <= L'Z')) {
                    vk = static_cast<UINT>(::towupper(c));
                } else if (c >= L'0' && c <= L'9') {
                    vk = static_cast<UINT>(c);
                }
            } else if (token.length() >= 2 && (token[0] == L'f' || token[0] == L'F')) {
                int fNum = 0;
                bool validF = true;
                for (size_t i = 1; i < token.length(); ++i) {
                    if (token[i] >= L'0' && token[i] <= L'9') {
                        fNum = fNum * 10 + (token[i] - L'0');
                        if (fNum > 24) {
                            validF = false;
                            break;
                        }
                    } else {
                        validF = false;
                        break;
                    }
                }
                if (validF && fNum >= 1 && fNum <= 24) {
                    vk = VK_F1 + static_cast<UINT>(fNum - 1);
                }
            } else if (EqualsIgnoreCase(token, L"space")) {
                vk = VK_SPACE;
            } else if (EqualsIgnoreCase(token, L"tab")) {
                vk = VK_TAB;
            } else if (EqualsIgnoreCase(token, L"enter") || EqualsIgnoreCase(token, L"return")) {
                vk = VK_RETURN;
            } else if (EqualsIgnoreCase(token, L"esc") || EqualsIgnoreCase(token, L"escape")) {
                vk = VK_ESCAPE;
            } else if (EqualsIgnoreCase(token, L"up")) {
                vk = VK_UP;
            } else if (EqualsIgnoreCase(token, L"down")) {
                vk = VK_DOWN;
            } else if (EqualsIgnoreCase(token, L"left")) {
                vk = VK_LEFT;
            } else if (EqualsIgnoreCase(token, L"right")) {
                vk = VK_RIGHT;
            } else if (EqualsIgnoreCase(token, L"home")) {
                vk = VK_HOME;
            } else if (EqualsIgnoreCase(token, L"end")) {
                vk = VK_END;
            } else if (EqualsIgnoreCase(token, L"pageup") || EqualsIgnoreCase(token, L"pgup")) {
                vk = VK_PRIOR;
            } else if (EqualsIgnoreCase(token, L"pagedown") || EqualsIgnoreCase(token, L"pgdn")) {
                vk = VK_NEXT;
            } else if (EqualsIgnoreCase(token, L"insert") || EqualsIgnoreCase(token, L"ins")) {
                vk = VK_INSERT;
            } else if (EqualsIgnoreCase(token, L"delete") || EqualsIgnoreCase(token, L"del")) {
                vk = VK_DELETE;
            }

            if (vk == 0) {
                // Reject unknown/bogus key tokens (e.g. Ctrl+R+bogus)
                return false;
            }
            outVk = vk;
        }
    }

    return (outModifiers != 0 && outVk != 0);
}

// ============================================================================
// Process Tracking & Window Inspection Helpers
// ============================================================================

[[nodiscard]] std::wstring GetProcessNameFromPID(DWORD pid) {
    if (pid <= 4) return L"";
    ScopedHandle hProc(OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid));
    if (!hProc) return L"";

    WCHAR path[1024] = {};
    DWORD size = static_cast<DWORD>(std::size(path));
    if (QueryFullProcessImageNameW(hProc.get(), 0, path, &size)) {
        const WCHAR* pSlash = wcsrchr(path, L'\\');
        return pSlash ? (pSlash + 1) : path;
    }
    return L"";
}

[[nodiscard]] std::wstring GetForegroundProcessName() {
    HWND hFore = GetForegroundWindow();
    if (!hFore) return L"";

    DWORD pid = 0;
    GetWindowThreadProcessId(hFore, &pid);
    std::wstring procName = GetProcessNameFromPID(pid);

    if (_wcsicmp(procName.c_str(), L"ApplicationFrameHost.exe") == 0) {
        HWND hChild = FindWindowExW(hFore, nullptr, L"Windows.UI.Core.CoreWindow", nullptr);
        if (hChild) {
            DWORD childPid = 0;
            GetWindowThreadProcessId(hChild, &childPid);
            if (childPid != 0 && childPid != pid) {
                std::wstring childProc = GetProcessNameFromPID(childPid);
                if (!childProc.empty()) return childProc;
            }
        }
    }
    return procName;
}

static ULONGLONG s_lastInhibitCheckTick = 0;
static std::optional<std::wstring> s_cachedInhibitMatch;

void InvalidateInhibitAppCache() noexcept {
    s_lastInhibitCheckTick = 0;
    s_cachedInhibitMatch = std::nullopt;
}

[[nodiscard]] std::optional<std::wstring> CheckAppInRunningList(const std::vector<std::wstring>& list) {
    if (list.empty()) return std::nullopt;

    ULONGLONG now = GetTickCount64();
    if (s_lastInhibitCheckTick > 0 && (now - s_lastInhibitCheckTick < 4000)) {
        return s_cachedInhibitMatch;
    }
    s_lastInhibitCheckTick = now;

    ScopedHandle hSnapshot(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0));
    if (!hSnapshot) {
        s_cachedInhibitMatch = std::nullopt;
        return std::nullopt;
    }

    PROCESSENTRY32W pe = { sizeof(pe) };
    if (Process32FirstW(hSnapshot.get(), &pe)) {
        do {
            if (IsAppInList(pe.szExeFile, list)) {
                s_cachedInhibitMatch = pe.szExeFile;
                return s_cachedInhibitMatch;
            }
        } while (Process32NextW(hSnapshot.get(), &pe));
    }
    s_cachedInhibitMatch = std::nullopt;
    return std::nullopt;
}

[[nodiscard]] DWORD ResolveRefreshRate(const WCHAR* pDevice, DWORD targetHz, const DEVMODEW& dmCurrent, std::vector<DWORD>& outRates) {
    outRates.clear();
    DEVMODEW dmEnum = {};
    dmEnum.dmSize = sizeof(dmEnum);

    DWORD exactMatch = 0, toleranceMatch = 0, closestMatch = 0;
    int minDiff = 999999;

    for (DWORD i = 0; EnumDisplaySettingsExW(pDevice, i, &dmEnum, EDS_ROTATEDMODE); ++i) {
        if (dmEnum.dmPelsWidth == dmCurrent.dmPelsWidth &&
            dmEnum.dmPelsHeight == dmCurrent.dmPelsHeight &&
            dmEnum.dmBitsPerPel == dmCurrent.dmBitsPerPel) {

            DWORD hz = dmEnum.dmDisplayFrequency;
            if (std::find(outRates.begin(), outRates.end(), hz) == outRates.end()) {
                outRates.push_back(hz);
            }
            if (targetHz > 1) {
                if (hz == targetHz) {
                    exactMatch = hz;
                } else if (std::abs(static_cast<int>(hz) - static_cast<int>(targetHz)) <= 1 && toleranceMatch == 0) {
                    toleranceMatch = hz;
                }
                int diff = std::abs(static_cast<int>(hz) - static_cast<int>(targetHz));
                if (diff < minDiff) {
                    minDiff = diff;
                    closestMatch = hz;
                }
            }
        }
    }

    std::sort(outRates.begin(), outRates.end());
    std::vector<DWORD> dedup;
    for (DWORD r : outRates) {
        if (dedup.empty()) {
            dedup.push_back(r);
        } else {
            DWORD prev = dedup.back();
            if (std::abs(static_cast<int>(r) - static_cast<int>(prev)) <= 1) {
                dedup.back() = std::max(prev, r);
            } else {
                dedup.push_back(r);
            }
        }
    }
    outRates = std::move(dedup);

    if (outRates.empty()) return 0; // Fail closed: no supported modes enumerated

    // targetHz == 0: Highest supported refresh rate
    if (targetHz == 0) return outRates.back();

    // targetHz == 1: Lowest supported refresh rate (usually 60 Hz)
    if (targetHz == 1) return outRates.front();

    if (exactMatch != 0) return exactMatch;
    if (toleranceMatch != 0) return toleranceMatch;
    if (closestMatch != 0) return closestMatch;
    return outRates.back();
}

[[nodiscard]] std::wstring FormatRatesList(const std::vector<DWORD>& rates) {
    std::wstring s;
    for (size_t i = 0; i < rates.size(); ++i) {
        if (i > 0) s += L", ";
        s += std::to_wstring(rates[i]) + L"Hz";
    }
    return s;
}

[[nodiscard]] DWORD GetCurrentPrimaryRefreshRate() {
    DEVMODEW dm = {};
    dm.dmSize = sizeof(dm);
    if (EnumDisplaySettingsExW(nullptr, ENUM_CURRENT_SETTINGS, &dm, EDS_ROTATEDMODE)) {
        return dm.dmDisplayFrequency;
    }
    return 60;
}

[[nodiscard]] DWORD GetMaxRefreshRate(const WCHAR* pDevice = nullptr) {
    DEVMODEW dm = {};
    dm.dmSize = sizeof(dm);
    if (!EnumDisplaySettingsExW(pDevice, ENUM_CURRENT_SETTINGS, &dm, EDS_ROTATEDMODE)) return 60;
    std::vector<DWORD> rates;
    DWORD res = ResolveRefreshRate(pDevice, 0, dm, rates);
    return (res > 0) ? res : (dm.dmDisplayFrequency > 0 ? dm.dmDisplayFrequency : 60);
}

[[nodiscard]] DWORD GetMinRefreshRate(const WCHAR* pDevice = nullptr) {
    DEVMODEW dm = {};
    dm.dmSize = sizeof(dm);
    if (!EnumDisplaySettingsExW(pDevice, ENUM_CURRENT_SETTINGS, &dm, EDS_ROTATEDMODE)) return 60;
    std::vector<DWORD> rates;
    DWORD res = ResolveRefreshRate(pDevice, 1, dm, rates);
    return (res > 0) ? res : (dm.dmDisplayFrequency > 0 ? dm.dmDisplayFrequency : 60);
}

[[nodiscard]] std::optional<std::wstring> IsForegroundWindowFullscreen(const std::wstring& knownProc = L"") {
    HWND hFore = GetForegroundWindow();
    if (!hFore || !IsWindowVisible(hFore) || IsIconic(hFore)) return std::nullopt;

    WCHAR szClass[128] = {};
    GetClassNameW(hFore, szClass, 127);
    if (_wcsicmp(szClass, L"Progman") == 0 ||
        _wcsicmp(szClass, L"WorkerW") == 0 ||
        _wcsicmp(szClass, L"Shell_TrayWnd") == 0 ||
        _wcsicmp(szClass, L"Shell_SecondaryTrayWnd") == 0 ||
        _wcsicmp(szClass, L"CabinetWClass") == 0 ||
        _wcsicmp(szClass, L"TaskManagerWindow") == 0 ||
        _wcsicmp(szClass, L"Windows.UI.Core.CoreWindow") == 0) {
        return std::nullopt;
    }

    std::wstring proc = !knownProc.empty() ? knownProc : GetForegroundProcessName();
    if (_wcsicmp(proc.c_str(), L"LockApp.exe") == 0 ||
        _wcsicmp(proc.c_str(), L"LogonUI.exe") == 0 ||
        _wcsicmp(proc.c_str(), L"SearchHost.exe") == 0 ||
        _wcsicmp(proc.c_str(), L"explorer.exe") == 0 ||
        _wcsicmp(proc.c_str(), L"StartMenuExperienceHost.exe") == 0) {
        return std::nullopt;
    }

    HMONITOR hMon = MonitorFromWindow(hFore, MONITOR_DEFAULTTONEAREST);
    if (!hMon) return std::nullopt;

    MONITORINFO mi = { sizeof(mi) };
    if (!GetMonitorInfoW(hMon, &mi)) return std::nullopt;

    RECT rcWnd = {};
    if (!GetWindowRect(hFore, &rcWnd)) return std::nullopt;

    if (rcWnd.left <= mi.rcMonitor.left &&
        rcWnd.top <= mi.rcMonitor.top &&
        rcWnd.right >= mi.rcMonitor.right &&
        rcWnd.bottom >= mi.rcMonitor.bottom) {

        LONG style = GetWindowLongW(hFore, GWL_STYLE);
        if ((style & WS_CAPTION) != WS_CAPTION || (style & WS_POPUP)) {
            return proc;
        }
    }
    return std::nullopt;
}


// ============================================================================
// Power Scheme & Windows 11 Overlay Inspection
// ============================================================================

[[nodiscard]] GUID QueryWindows11OverlayPowerScheme(bool isAC) {
    HKEY rawKey = nullptr;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
        L"SYSTEM\\CurrentControlSet\\Control\\Power\\User\\PowerSchemes",
        0, KEY_READ, &rawKey) != ERROR_SUCCESS) {
        return GUID_NULL_LOCAL;
    }
    ScopedRegKey hKey(rawKey);

    WCHAR szOverlay[45] = {};
    DWORD cbData = sizeof(szOverlay) - sizeof(WCHAR);
    const WCHAR* valName = isAC ? L"ActiveOverlayAcPowerScheme" : L"ActiveOverlayDcPowerScheme";
    GUID result = GUID_NULL_LOCAL;

    if (RegQueryValueExW(hKey.get(), valName, nullptr, nullptr, reinterpret_cast<LPBYTE>(szOverlay), &cbData) == ERROR_SUCCESS) {
        szOverlay[cbData / sizeof(WCHAR)] = L'\0';
        GUID overlayGuid = GUID_NULL_LOCAL;
        if (SUCCEEDED(IIDFromString(szOverlay, &overlayGuid))) {
            if (IsEqualGUID(overlayGuid, GUID_OVERLAY_BEST_PERFORMANCE)) {
                result = GUID_MIN_POWER_SAVINGS_LOCAL;
            }
        }
    }
    return result;
}

[[nodiscard]] GUID QueryEffectivePowerPersonality(bool isAC) {
    GUID overlayScheme = QueryWindows11OverlayPowerScheme(isAC);
    if (!IsEqualGUID(overlayScheme, GUID_NULL_LOCAL)) return overlayScheme;

    GUID* pGuid = nullptr;
    GUID result = GUID_NULL_LOCAL;
    if (PowerGetActiveScheme(nullptr, &pGuid) == ERROR_SUCCESS && pGuid) {
        result = *pGuid;
        LocalFree(pGuid);
    }
    return result;
}

// ============================================================================
// Internal vs external display detection
// ============================================================================

static std::vector<std::pair<std::wstring, bool>> s_internalDisplayCache;
static bool s_internalCacheValid = false;

void InvalidateDisplayDeviceCache() noexcept {
    s_internalCacheValid = false;
    s_internalDisplayCache.clear();
}

[[nodiscard]] bool IsInternalDisplayDevice(const WCHAR* pDeviceName) {
    std::wstring resolvedName;
    if (!pDeviceName || !*pDeviceName) {
        DISPLAY_DEVICEW dd = { sizeof(dd) };
        for (DWORD i = 0; EnumDisplayDevicesW(nullptr, i, &dd, 0); ++i) {
            if ((dd.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) &&
                (dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)) {
                resolvedName = dd.DeviceName;
                pDeviceName = resolvedName.c_str();
                break;
            }
        }
        if (!pDeviceName || !*pDeviceName) return true;
    }

    if (s_internalCacheValid) {
        for (const auto& entry : s_internalDisplayCache) {
            if (_wcsicmp(entry.first.c_str(), pDeviceName) == 0) {
                return entry.second;
            }
        }
    }

    UINT32 pathCount = 0, modeCount = 0;
    std::vector<DISPLAYCONFIG_PATH_INFO> paths;
    std::vector<DISPLAYCONFIG_MODE_INFO> modes;
    LONG result = ERROR_SUCCESS;

    for (int retry = 0; retry < 3; ++retry) {
        if (GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &pathCount, &modeCount) != ERROR_SUCCESS || pathCount == 0) {
            return false;
        }
        paths.resize(pathCount);
        modes.resize(modeCount);
        result = QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &pathCount, paths.data(), &modeCount, modes.data(), nullptr);
        if (result == ERROR_SUCCESS) break;
        if (result != ERROR_INSUFFICIENT_BUFFER) return false;
    }
    if (result != ERROR_SUCCESS) return false;

    bool isInternal = false;
    for (UINT32 i = 0; i < pathCount; ++i) {
        DISPLAYCONFIG_SOURCE_DEVICE_NAME sourceName = {};
        sourceName.header.type = static_cast<DISPLAYCONFIG_DEVICE_INFO_TYPE>(DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME);
        sourceName.header.size = sizeof(sourceName);
        sourceName.header.adapterId = paths[i].sourceInfo.adapterId;
        sourceName.header.id = paths[i].sourceInfo.id;

        if (DisplayConfigGetDeviceInfo(&sourceName.header) == ERROR_SUCCESS) {
            if (_wcsicmp(sourceName.viewGdiDeviceName, pDeviceName) == 0) {
                UINT32 tech = paths[i].targetInfo.outputTechnology;
                isInternal = (tech == DISPLAYCONFIG_OUTPUT_TECHNOLOGY_INTERNAL ||
                              tech == DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DISPLAYPORT_EMBEDDED ||
                              tech == DISPLAYCONFIG_OUTPUT_TECHNOLOGY_UDI_EMBEDDED ||
                              tech == DISPLAYCONFIG_OUTPUT_TECHNOLOGY_LVDS);
                break;
            }
        }
    }

    s_internalCacheValid = true;
    s_internalDisplayCache.emplace_back(pDeviceName, isInternal);
    return isInternal;
}

// ============================================================================
// Settings Management
// ============================================================================

void LoadSettings() {
    // 1. Power & Battery
    g_settings.chargeSwitchingEnabled = ReadBoolSettingSafe(L"PowerAndBattery.ChargeSwitchingEnabled", true);

    std::wstring acRateStr = ReadStringSettingSafe(L"PowerAndBattery.PluggedInRate", L"max");
    if (acRateStr == L"max" || acRateStr == L"auto") {
        g_settings.targetAC = 0; // 0 = automatic highest supported
    } else if (acRateStr == L"custom") {
        int customAc = ReadIntSettingSafe(L"PowerAndBattery.CustomPluggedInRate", 144);
        g_settings.targetAC = (customAc >= 30) ? static_cast<DWORD>(customAc) : 144;
    } else {
        auto r = ParsePositiveInt(acRateStr);
        g_settings.targetAC = (r && *r >= 30) ? static_cast<DWORD>(*r) : 0;
    }

    std::wstring dcRateStr = ReadStringSettingSafe(L"PowerAndBattery.OnBatteryRate", L"60");
    if (dcRateStr == L"min") {
        g_settings.targetDC = 1; // 1 = lowest supported
    } else if (dcRateStr == L"match_ac") {
        g_settings.targetDC = 0; // 0 = match AC
    } else if (dcRateStr == L"custom") {
        int customDc = ReadIntSettingSafe(L"PowerAndBattery.CustomBatteryRate", 60);
        g_settings.targetDC = (customDc >= 30) ? static_cast<DWORD>(customDc) : 60;
    } else {
        auto r = ParsePositiveInt(dcRateStr);
        g_settings.targetDC = (r && *r >= 30) ? static_cast<DWORD>(*r) : 60;
    }

    g_settings.energySaverEnabled = ReadBoolSettingSafe(L"PowerAndBattery.EnergySaverEnabled", true);
    g_settings.energySaverAction = ReadStringSettingSafe(L"PowerAndBattery.EnergySaverRate", L"60");
    if (g_settings.energySaverAction == L"force_low" || g_settings.energySaverAction.empty()) {
        g_settings.energySaverAction = L"60";
    }
    if (g_settings.energySaverAction == L"custom") {
        int customEs = ReadIntSettingSafe(L"PowerAndBattery.CustomEnergySaverRate", 60);
        g_settings.targetEnergySaver = (customEs >= 30) ? static_cast<DWORD>(customEs) : 60;
    }

    // 2. Gaming & Applications
    g_settings.autoGameBoost = ReadBoolSettingSafe(L"GamingAndApps.AutoGameBoost", true);
    g_settings.appRulesEnabled = ReadBoolSettingSafe(L"GamingAndApps.AppRulesEnabled", false);
    g_settings.highRefreshApps = ReadStringSettingSafe(L"GamingAndApps.HighRefreshApps", L"cs2; valorant; overwatch; cyberpunk2077; blender");
    g_settings.lowRefreshApps = ReadStringSettingSafe(L"GamingAndApps.LowRefreshApps", L"vlc; mpc-hc64; netflix; acrobat");
    g_settings.inhibitAppsEnabled = ReadBoolSettingSafe(L"GamingAndApps.InhibitAppsEnabled", true);
    g_settings.inhibitApps = ReadStringSettingSafe(L"GamingAndApps.InhibitApps", L"obs64; obs; streamlabs; powerpnt");

    g_parsedHighRefreshApps = ParseAppList(g_settings.highRefreshApps);
    g_parsedLowRefreshApps = ParseAppList(g_settings.lowRefreshApps);
    g_parsedInhibitApps = ParseAppList(g_settings.inhibitApps);
    s_lastInhibitCheckTick = 0;
    s_cachedInhibitMatch = std::nullopt;

    // 3. Night Schedule
    g_settings.timeScheduleEnabled = ReadBoolSettingSafe(L"Schedule.TimeScheduleEnabled", false);
    g_settings.scheduleStart = ReadStringSettingSafe(L"Schedule.ScheduleStart", L"22:00");
    g_settings.scheduleEnd = ReadStringSettingSafe(L"Schedule.ScheduleEnd", L"07:00");

    // 4. Display & Transitions
    std::wstring targetDisp = ReadStringSettingSafe(L"DisplayAndTransitions.TargetDisplays", L"primary");
    g_settings.targetDisplayAll = (targetDisp == L"all");
    g_settings.smartDockingEnabled = ReadBoolSettingSafe(L"DisplayAndTransitions.SmartDockingEnabled", true);
    g_settings.quietSwitchEnabled = ReadBoolSettingSafe(L"DisplayAndTransitions.QuietSwitchEnabled", true);
    int coolSec = ReadIntSettingSafe(L"DisplayAndTransitions.AntiFlickerCooldown", 3);
    if (coolSec < 0 || coolSec > 10) coolSec = 3;
    g_settings.switchCooldownMs = static_cast<DWORD>(coolSec * 1000);

    // 5. Shortcuts & Notifications
    g_settings.osdEnabled = ReadBoolSettingSafe(L"ShortcutsAndNotifications.OsdBadgeEnabled", true);
    g_settings.globalHotkeyEnabled = ReadBoolSettingSafe(L"ShortcutsAndNotifications.GlobalHotkeyEnabled", false);
    g_settings.globalHotkey = ReadStringSettingSafe(L"ShortcutsAndNotifications.GlobalHotkey", L"Win+Ctrl+R");

    UINT parsedMod = 0, parsedVk = 0;
    if (ParseHotkeyString(g_settings.globalHotkey, parsedMod, parsedVk)) {
        g_settings.hotkeyModifiers = parsedMod;
        g_settings.hotkeyVk = parsedVk;
    } else {
        Wh_Log(L"Auto Refresh Rate: Invalid hotkey '%s'. Falling back to Win+Ctrl+R.", g_settings.globalHotkey.c_str());
        g_settings.hotkeyModifiers = MOD_WIN | MOD_CONTROL;
        g_settings.hotkeyVk = 'R';
    }

    g_settings.verboseLogging = ReadBoolSettingSafe(L"ShortcutsAndNotifications.VerboseLogging", true);

    HWND hWnd = g_hWnd.load();
    if (hWnd) {
        if (g_settings.timeScheduleEnabled) {
            SetTimer(hWnd, TIMER_ID_TIME_CHECK, TIME_CHECK_INTERVAL_MS, nullptr);
        } else {
            KillTimer(hWnd, TIMER_ID_TIME_CHECK);
        }

        UnregisterHotKey(hWnd, HOTKEY_ID_CYCLE);
        if (g_settings.globalHotkeyEnabled) {
            if (!RegisterHotKey(hWnd, HOTKEY_ID_CYCLE, g_settings.hotkeyModifiers | MOD_NOREPEAT, g_settings.hotkeyVk)) {
                if (!RegisterHotKey(hWnd, HOTKEY_ID_CYCLE, g_settings.hotkeyModifiers, g_settings.hotkeyVk)) {
                    Wh_Log(L"Auto Refresh Rate: Failed to register hotkey '%s' (mod=0x%X, vk=0x%X). Combination may be reserved.",
                           g_settings.globalHotkey.c_str(), g_settings.hotkeyModifiers, g_settings.hotkeyVk);
                }
            }
        }
    }

    Wh_Log(L"Auto Refresh Rate Settings loaded: PluggedIn=%s, Battery=%s, EnergySaver=%s, GameBoost=%s, AppRules=%s, QuietSwitch=%s, Inhibit=%s, Hotkey=%s",
           (g_settings.targetAC == 0 ? L"Max" : std::to_wstring(g_settings.targetAC).c_str()),
           (g_settings.targetDC == 1 ? L"Min" : (g_settings.targetDC == 0 ? L"MatchAC" : std::to_wstring(g_settings.targetDC).c_str())),
           g_settings.energySaverAction.c_str(),
           g_settings.autoGameBoost ? L"ON" : L"OFF",
           g_settings.appRulesEnabled ? L"ON" : L"OFF",
           g_settings.quietSwitchEnabled ? L"ON" : L"OFF",
           g_settings.inhibitAppsEnabled ? L"ON" : L"OFF",
           g_settings.globalHotkeyEnabled ? g_settings.globalHotkey.c_str() : L"Disabled");
}

// ============================================================================
// Target refresh rate evaluation policy
// ============================================================================

[[nodiscard]] DWORD EvaluateTargetRefreshRate(const PowerStateSnapshot& state, std::wstring& outReason, std::wstring& outBrief, bool& outIsInhibited) {
    outIsInhibited = false;

    // 1. Manual hotkey lock
    if (g_manualOverrideActive && g_manualOverrideHz > 0) {
        outBrief = L"Manual lock";
        outReason = L"Manual hotkey lock active (" + std::to_wstring(g_manualOverrideHz) + L" Hz)";
        return g_manualOverrideHz;
    }

    // 2. Protected applications
    if (g_settings.inhibitAppsEnabled) {
        std::wstring foreProc = GetForegroundProcessName();
        if (!foreProc.empty() && IsAppInList(foreProc, g_parsedInhibitApps)) {
            outBrief = L"Protected: " + FormatAppNameForDisplay(foreProc);
            outReason = L"Protected app in focus ('" + foreProc + L"'), display switching inhibited";
            outIsInhibited = true;
            return GetCurrentPrimaryRefreshRate();
        }

        auto matchedInhibit = CheckAppInRunningList(g_parsedInhibitApps);
        if (matchedInhibit) {
            outBrief = L"Protected: " + FormatAppNameForDisplay(*matchedInhibit);
            outReason = L"Protected app running ('" + *matchedInhibit + L"'), display switching inhibited";
            outIsInhibited = true;
            return GetCurrentPrimaryRefreshRate();
        }
    }

    DWORD resolvedAC = (g_settings.targetAC == 0) ? GetMaxRefreshRate() : g_settings.targetAC;
    DWORD resolvedDC = (g_settings.targetDC == 1) ? GetMinRefreshRate() : ((g_settings.targetDC == 0) ? resolvedAC : g_settings.targetDC);

    // 3. Windows Energy Saver
    if (g_settings.energySaverEnabled && state.isBatterySaverActive && g_settings.energySaverAction != L"ignore") {
        DWORD resolvedSaver = 60;
        if (g_settings.energySaverAction == L"min") {
            resolvedSaver = GetMinRefreshRate();
        } else if (g_settings.energySaverAction == L"custom") {
            resolvedSaver = (g_settings.targetEnergySaver >= 30) ? g_settings.targetEnergySaver : 60;
        }
        outBrief = L"Energy saver";
        outReason = L"Windows Energy Saver active (" + std::to_wstring(resolvedSaver) + L" Hz)";
        return resolvedSaver;
    }

    // Windows 11 Power Mode slider (Best Performance boost)
    if (g_settings.chargeSwitchingEnabled && !state.isAC && IsEqualGUID(state.powerScheme, GUID_MIN_POWER_SAVINGS_LOCAL)) {
        outBrief = L"Best performance";
        outReason = L"Windows Power Mode: Best Performance (" + std::to_wstring(resolvedAC) + L" Hz)";
        return resolvedAC;
    }

    // 4. Foreground application rules
    std::wstring foreProc;
    if (g_settings.appRulesEnabled || g_settings.autoGameBoost) {
        foreProc = GetForegroundProcessName();
    }

    if (g_settings.appRulesEnabled && !foreProc.empty()) {
        if (IsAppInList(foreProc, g_parsedLowRefreshApps)) {
            outBrief = L"App saver: " + FormatAppNameForDisplay(foreProc);
            outReason = L"Low-refresh app in focus ('" + foreProc + L"' -> " + std::to_wstring(resolvedDC) + L" Hz)";
            return resolvedDC;
        } else if (IsAppInList(foreProc, g_parsedHighRefreshApps)) {
            outBrief = L"App boost: " + FormatAppNameForDisplay(foreProc);
            outReason = L"High-refresh app in focus ('" + foreProc + L"' -> " + std::to_wstring(resolvedAC) + L" Hz)";
            return resolvedAC;
        }
    }

    // Fullscreen game boost
    if (g_settings.autoGameBoost) {
        auto fsProc = IsForegroundWindowFullscreen(foreProc);
        if (fsProc) {
            std::wstring gameName = fsProc->empty() ? L"Fullscreen" : *fsProc;
            outBrief = L"Game: " + FormatAppNameForDisplay(gameName);
            outReason = L"Fullscreen game detected ('" + gameName + L"' -> " + std::to_wstring(resolvedAC) + L" Hz)";
            return resolvedAC;
        }
    }

    // 5. Night schedule
    if (g_settings.timeScheduleEnabled) {
        if (IsCurrentTimeInSchedule(g_settings.scheduleStart, g_settings.scheduleEnd)) {
            outBrief = L"Night schedule";
            outReason = L"Night schedule active (" + std::to_wstring(resolvedDC) + L" Hz)";
            return resolvedDC;
        }
    }

    // 6. Power source baseline (AC vs battery)
    if (g_settings.chargeSwitchingEnabled) {
        if (state.isAC) {
            outBrief = L"Plugged in";
            outReason = L"AC power connected (" + std::to_wstring(resolvedAC) + L" Hz)";
            return resolvedAC;
        } else {
            outBrief = L"Battery (" + std::to_wstring(state.batteryPercent) + L"%)";
            outReason = L"Battery power (" + std::to_wstring(resolvedDC) + L" Hz, " + std::to_wstring(state.batteryPercent) + L"% remaining)";
            return resolvedDC;
        }
    }

    outBrief = L"Active";
    outReason = L"Power rules inactive, keeping current rate (" + std::to_wstring(GetCurrentPrimaryRefreshRate()) + L" Hz)";
    return GetCurrentPrimaryRefreshRate();
}

// ============================================================================
// Event-Gated Status Dashboard
// ============================================================================

static DWORD g_lastLoggedHz = 0;
static std::wstring g_lastLoggedReason;
static bool g_lastLoggedAC = false;
static BYTE g_lastLoggedBatt = 255;
static bool g_lastLoggedSaver = false;

void PrintStatusDashboard(const PowerStateSnapshot& state, DWORD targetHz, const std::wstring& reason) {
    if (!g_settings.verboseLogging) return;

    if (targetHz == g_lastLoggedHz &&
        reason == g_lastLoggedReason &&
        state.isAC == g_lastLoggedAC &&
        state.batteryPercent == g_lastLoggedBatt &&
        state.isBatterySaverActive == g_lastLoggedSaver) return;

    g_lastLoggedHz = targetHz;
    g_lastLoggedReason = reason;
    g_lastLoggedAC = state.isAC;
    g_lastLoggedBatt = state.batteryPercent;
    g_lastLoggedSaver = state.isBatterySaverActive;

    WCHAR primary[32] = {};
    DISPLAY_DEVICEW dd = { sizeof(dd) };
    for (DWORD i = 0; EnumDisplayDevicesW(nullptr, i, &dd, 0); ++i) {
        if ((dd.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) && (dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)) {
            wcsncpy_s(primary, dd.DeviceName, _TRUNCATE);
            break;
        }
    }

    DEVMODEW dm = {};
    dm.dmSize = sizeof(dm);
    EnumDisplaySettingsExW(primary[0] ? primary : nullptr, ENUM_CURRENT_SETTINGS, &dm, EDS_ROTATEDMODE);

    Wh_Log(L"================================================================================");
    Wh_Log(L"Auto Refresh Rate Dashboard | Target: %u Hz | Reason: %s", targetHz, reason.c_str());
    Wh_Log(L"  Primary Monitor  : %s (%ux%u @ %u Hz)", primary[0] ? primary : L"Default", dm.dmPelsWidth, dm.dmPelsHeight, dm.dmDisplayFrequency);
    Wh_Log(L"  Power Source     : %s (Battery: %u%%) | Energy Saver: %s", state.isAC ? L"AC" : L"Battery", state.batteryPercent, state.isBatterySaverActive ? L"ON" : L"OFF");
    Wh_Log(L"================================================================================");
}

// ============================================================================
// On-screen display badge
// ============================================================================

typedef UINT (WINAPI *PFN_GetDpiForWindow)(HWND);

[[nodiscard]] float GetOsdDpiScale(HWND hWnd) {
    static PFN_GetDpiForWindow pfn = nullptr;
    static bool s_inited = false;
    if (!s_inited) {
        HMODULE hUser = GetModuleHandleW(L"user32.dll");
        if (hUser) {
            pfn = reinterpret_cast<PFN_GetDpiForWindow>(GetProcAddress(hUser, "GetDpiForWindow"));
        }
        s_inited = true;
    }
    if (pfn && hWnd) {
        const UINT dpi = pfn(hWnd);
        if (dpi > 0) return static_cast<float>(dpi) / 96.0f;
    }
    return 1.0f;
}

static HFONT s_hOsdFontHz = nullptr;
static HFONT s_hOsdFontSub = nullptr;
static float s_osdFontScale = 0.0f;

void CleanupOsdFonts() noexcept {
    if (s_hOsdFontHz) {
        DeleteObject(s_hOsdFontHz);
        s_hOsdFontHz = nullptr;
    }
    if (s_hOsdFontSub) {
        DeleteObject(s_hOsdFontSub);
        s_hOsdFontSub = nullptr;
    }
    s_osdFontScale = 0.0f;
}

void EnsureOsdFonts(float scale) noexcept {
    if (s_hOsdFontHz && s_hOsdFontSub && std::abs(s_osdFontScale - scale) < 0.001f) {
        return;
    }
    CleanupOsdFonts();
    s_osdFontScale = scale;
    s_hOsdFontHz = CreateFontW(static_cast<int>(20.0f * scale), 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI Variable Display");
    s_hOsdFontSub = CreateFontW(static_cast<int>(13.0f * scale), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI Variable Text");
}

LRESULT CALLBACK OsdWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_PAINT: {
        PAINTSTRUCT ps = {};
        HDC hdc = BeginPaint(hWnd, &ps);
        if (!hdc) return 0;

        RECT rc = {};
        GetClientRect(hWnd, &rc);
        const float scale = GetOsdDpiScale(hWnd);

        HDC memDC = CreateCompatibleDC(hdc);
        if (memDC) {
            HBITMAP memBmp = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
            if (memBmp) {
                HBITMAP hOldBmp = static_cast<HBITMAP>(SelectObject(memDC, memBmp));
                {
                    ScopedDcState dcSaver(memDC);

                    {
                        ScopedGdiObject hBgBrush(CreateSolidBrush(RGB(24, 24, 27)));
                        ScopedGdiObject hBorderPen(CreatePen(PS_SOLID, 1, RGB(63, 63, 70)));
                        ScopedDcState bgSaver(memDC);
                        SelectObject(memDC, hBgBrush.get());
                        SelectObject(memDC, hBorderPen.get());

                        const int roundCorner = static_cast<int>(24.0f * scale);
                        RoundRect(memDC, rc.left, rc.top, rc.right, rc.bottom, roundCorner, roundCorner);
                    }

                    {
                        const COLORREF accentColor = (g_osdCurrentHz >= 100) ? RGB(59, 130, 246) : RGB(16, 185, 129);
                        ScopedGdiObject hAccentBrush(CreateSolidBrush(accentColor));
                        ScopedGdiObject hAccentPen(CreatePen(PS_SOLID, 1, accentColor));
                        ScopedDcState dotSaver(memDC);
                        SelectObject(memDC, hAccentBrush.get());
                        SelectObject(memDC, hAccentPen.get());

                        const int dotLeft = static_cast<int>(14.0f * scale);
                        const int dotTop = static_cast<int>(17.0f * scale);
                        const int dotSize = static_cast<int>(12.0f * scale);
                        Ellipse(memDC, dotLeft, dotTop, dotLeft + dotSize, dotTop + dotSize);
                    }

                    SetBkMode(memDC, TRANSPARENT);
                    EnsureOsdFonts(scale);

                    if (s_hOsdFontHz) {
                        ScopedDcState fontSaver(memDC);
                        SelectObject(memDC, s_hOsdFontHz);
                        SetTextColor(memDC, RGB(244, 244, 245));

                        const std::wstring hzText = std::to_wstring(g_osdCurrentHz) + L" Hz";
                        const int hzX = static_cast<int>(34.0f * scale);
                        const int hzY = static_cast<int>(13.0f * scale);
                        TextOutW(memDC, hzX, hzY, hzText.c_str(), static_cast<int>(hzText.length()));

                        SIZE hzSize = {};
                        GetTextExtentPoint32W(memDC, hzText.c_str(), static_cast<int>(hzText.length()), &hzSize);

                        if (s_hOsdFontSub) {
                            ScopedDcState subFontSaver(memDC);
                            SelectObject(memDC, s_hOsdFontSub);
                            SetTextColor(memDC, RGB(161, 161, 170));
                            TextOutW(memDC, hzX + hzSize.cx + static_cast<int>(14.0f * scale), static_cast<int>(17.0f * scale),
                                     g_osdCurrentReason.c_str(), static_cast<int>(g_osdCurrentReason.length()));
                        }
                    }

                    BitBlt(hdc, 0, 0, rc.right, rc.bottom, memDC, 0, 0, SRCCOPY);
                }
                SelectObject(memDC, hOldBmp);
                DeleteObject(memBmp);
            }
            DeleteDC(memDC);
        }

        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_ERASEBKGND:
        return 1;

    case WM_MOUSEACTIVATE:
        return MA_NOACTIVATE;

    case WM_NCHITTEST:
        return HTTRANSPARENT;

    case WM_TIMER:
        if (wParam == TIMER_ID_OSD_HOLD) {
            if (g_osdState != OsdState::Holding) return 0;
            KillTimer(hWnd, TIMER_ID_OSD_HOLD);
            g_osdState = OsdState::Fading;
            SetTimer(hWnd, TIMER_ID_OSD_FADE, 20, nullptr);
            return 0;
        } else if (wParam == TIMER_ID_OSD_FADE) {
            if (g_osdState != OsdState::Fading) return 0;
            if (g_osdAlpha > 20) {
                g_osdAlpha -= 20;
                SetLayeredWindowAttributes(hWnd, 0, g_osdAlpha, LWA_ALPHA);
            } else {
                KillTimer(hWnd, TIMER_ID_OSD_FADE);
                g_osdState = OsdState::Hidden;
                g_osdAlpha = 0;
                ShowWindow(hWnd, SW_HIDE);
            }
            return 0;
        }
        break;

    case WM_DESTROY:
        KillTimer(hWnd, TIMER_ID_OSD_HOLD);
        KillTimer(hWnd, TIMER_ID_OSD_FADE);
        CleanupOsdFonts();
        g_osdState = OsdState::Hidden;
        g_hOsdWnd = nullptr;
        return 0;
    }

    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

void InitializeOsdWindow(HINSTANCE hInstance) {
    WNDCLASSEXW wc = { sizeof(wc) };
    wc.lpfnWndProc = OsdWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = g_szOsdClassName;
    RegisterClassExW(&wc);

    HWND hOsd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        g_szOsdClassName, L"AutoRefreshRateOsdWindow", WS_POPUP, 0, 0, 260, 48, nullptr, nullptr, hInstance, nullptr);

    if (hOsd) {
        g_hOsdWnd = hOsd;
        SetLayeredWindowAttributes(hOsd, 0, 0, LWA_ALPHA);
        ShowWindow(hOsd, SW_HIDE);
    }
}

void ShowOsdBadge(DWORD hz, const std::wstring& reasonBrief) {
    if (!g_settings.osdEnabled || !g_hOsdWnd) return;

    g_osdCurrentHz = hz;
    g_osdCurrentReason = reasonBrief;

    float scale = GetOsdDpiScale(g_hOsdWnd);
    int baseWidth = 240;
    int approx = static_cast<int>(reasonBrief.length());
    if (approx > 14) baseWidth += (approx - 14) * 8;

    int width = static_cast<int>(baseWidth * scale);
    int height = static_cast<int>(48 * scale);

    static HWND s_lastOsdHwnd = nullptr;
    static int s_lastWidth = 0;
    static int s_lastHeight = 0;
    if (g_hOsdWnd != s_lastOsdHwnd || width != s_lastWidth || height != s_lastHeight) {
        s_lastOsdHwnd = g_hOsdWnd;
        s_lastWidth = width;
        s_lastHeight = height;
        int roundCorner = static_cast<int>(24 * scale);
        HRGN hRgn = CreateRoundRectRgn(0, 0, width + 1, height + 1, roundCorner, roundCorner);
        if (hRgn) {
            if (!SetWindowRgn(g_hOsdWnd, hRgn, FALSE)) {
                DeleteObject(hRgn);
            }
        }
    }

    RECT rcWork = {};
    POINT ptOrigin = { 0, 0 };
    HMONITOR hMon = MonitorFromPoint(ptOrigin, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi = { sizeof(mi) };
    if (hMon && GetMonitorInfoW(hMon, &mi)) {
        rcWork = mi.rcWork;
    } else {
        SystemParametersInfoW(SPI_GETWORKAREA, 0, &rcWork, 0);
    }

    int margin = static_cast<int>(24 * scale);
    int x = rcWork.right - width - margin;
    int y = rcWork.top + margin;

    KillTimer(g_hOsdWnd, TIMER_ID_OSD_HOLD);
    KillTimer(g_hOsdWnd, TIMER_ID_OSD_FADE);

    g_osdState = OsdState::Holding;
    g_osdAlpha = 240;
    SetLayeredWindowAttributes(g_hOsdWnd, 0, g_osdAlpha, LWA_ALPHA);
    SetWindowPos(g_hOsdWnd, HWND_TOPMOST, x, y, width, height, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    InvalidateRect(g_hOsdWnd, nullptr, TRUE);
    UpdateWindow(g_hOsdWnd);

    SetTimer(g_hOsdWnd, TIMER_ID_OSD_HOLD, g_settings.osdDurationMs, nullptr);
}

// ============================================================================
// Display Enumeration & Mode Switching
// ============================================================================

[[nodiscard]] std::vector<std::wstring> GetTargetDisplayDevices(bool allDisplays) {
    std::vector<std::wstring> devices;
    DISPLAY_DEVICEW dd = { sizeof(dd) };

    for (DWORD i = 0; EnumDisplayDevicesW(nullptr, i, &dd, 0); ++i) {
        if ((dd.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) &&
            !(dd.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER)) {
            if (allDisplays || (dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)) {
                devices.push_back(dd.DeviceName);
                if (!allDisplays) break;
            }
        }
    }
    if (devices.empty()) devices.push_back(L"");
    return devices;
}

[[nodiscard]] bool SetDisplayRefreshRate(const WCHAR* pDevice, DWORD targetHz, bool noReset = false, bool* pOutChanged = nullptr) {
    if (pOutChanged) *pOutChanged = false;

    const WCHAR* pDevParam = (pDevice && *pDevice) ? pDevice : nullptr;
    const WCHAR* pDevLog = pDevParam ? pDevParam : L"Primary";

    DEVMODEW dmCurrent = {};
    dmCurrent.dmSize = sizeof(dmCurrent);
    if (!EnumDisplaySettingsExW(pDevParam, ENUM_CURRENT_SETTINGS, &dmCurrent, EDS_ROTATEDMODE)) {
        Wh_Log(L"Failed to query current display settings for %s", pDevLog);
        return false;
    }

    if (targetHz > 1 && (dmCurrent.dmDisplayFrequency == targetHz ||
        std::abs(static_cast<int>(dmCurrent.dmDisplayFrequency) - static_cast<int>(targetHz)) <= 1)) {
        return true;
    }

    std::vector<DWORD> supported;
    DWORD resolved = ResolveRefreshRate(pDevParam, targetHz, dmCurrent, supported);
    if (resolved == 0) {
        Wh_Log(L"Target %u Hz unsupported on %s. Available: [%s]", targetHz, pDevLog, FormatRatesList(supported).c_str());
        return false;
    }

    if (dmCurrent.dmDisplayFrequency == resolved ||
        std::abs(static_cast<int>(dmCurrent.dmDisplayFrequency) - static_cast<int>(resolved)) <= 1) {
        return true;
    }

    Wh_Log(L"Adjusting %s: %u Hz -> %u Hz...", pDevLog, dmCurrent.dmDisplayFrequency, resolved);
    DEVMODEW dmTarget = dmCurrent;
    dmTarget.dmFields |= DM_DISPLAYFREQUENCY;
    dmTarget.dmDisplayFrequency = resolved;

    if (ChangeDisplaySettingsExW(pDevParam, &dmTarget, nullptr, CDS_TEST, nullptr) != DISP_CHANGE_SUCCESSFUL) {
        return false;
    }

    DWORD flags = (noReset ? CDS_NORESET : 0);
    if (ChangeDisplaySettingsExW(pDevParam, &dmTarget, nullptr, flags, nullptr) == DISP_CHANGE_SUCCESSFUL) {
        if (pOutChanged) *pOutChanged = true;
        Wh_Log(L"Success: Display (%s) set to %u Hz.", pDevLog, resolved);
        return true;
    }
    return false;
}

void ApplyRefreshRateToTargets(DWORD targetHz, const std::wstring& reasonBrief, bool forceOsd = false) {
    auto devices = GetTargetDisplayDevices(g_settings.targetDisplayAll);
    if (devices.empty()) return;

    bool isMulti = (g_settings.targetDisplayAll && devices.size() > 1);
    bool anyStaged = false;
    bool allStagesSucceeded = true;

    for (const auto& dev : devices) {
        const WCHAR* pDev = dev.empty() ? nullptr : dev.c_str();
        DWORD devTargetHz = targetHz;
        if (!g_manualOverrideActive && !g_state.isAC && g_settings.smartDockingEnabled && !IsInternalDisplayDevice(pDev)) {
            devTargetHz = (g_settings.targetAC == 0) ? GetMaxRefreshRate(pDev) : g_settings.targetAC;
        }

        bool changed = false;
        if (!SetDisplayRefreshRate(pDev, devTargetHz, isMulti, &changed)) {
            allStagesSucceeded = false;
        } else if (changed) {
            anyStaged = true;
        }
    }

    bool commitSuccessful = false;
    if (isMulti) {
        if (anyStaged) {
            if (allStagesSucceeded && ChangeDisplaySettingsExW(nullptr, nullptr, nullptr, 0, nullptr) == DISP_CHANGE_SUCCESSFUL) {
                commitSuccessful = true;
            } else {
                Wh_Log(L"Multi-display refresh rate commit failed or partial stage failure. Reverting staged changes...");
                ChangeDisplaySettingsExW(nullptr, nullptr, nullptr, 0, nullptr);
            }
        }
    } else {
        commitSuccessful = anyStaged;
    }

    if (commitSuccessful) {
        g_lastSuccessfulSwitchTick = GetTickCount64();
    }

    if (commitSuccessful || forceOsd) {
        DWORD osdHz = (targetHz == 0) ? GetCurrentPrimaryRefreshRate() : targetHz;
        ShowOsdBadge(osdHz, reasonBrief);
    }
}

void SynchronizeAndApplyPolicy(bool forceOsd, const std::wstring& forcedBrief) {
    SYSTEM_POWER_STATUS sps = {};
    if (GetSystemPowerStatus(&sps)) {
        if (sps.ACLineStatus != 255) {
            bool newAC = (sps.ACLineStatus == 1);
            if (newAC != g_state.isAC) {
                g_state.isAC = newAC;
                g_manualOverrideActive = false;
                g_manualOverrideHz = 0;
            }
        }
        if (sps.BatteryLifePercent != 255) g_state.batteryPercent = sps.BatteryLifePercent;
        g_state.isBatterySaverActive = (sps.SystemStatusFlag == 1);
    }

    g_state.powerScheme = QueryEffectivePowerPersonality(g_state.isAC);

    std::wstring reason, brief;
    bool isInhibited = false;
    DWORD targetHz = EvaluateTargetRefreshRate(g_state, reason, brief, isInhibited);
    if (!forcedBrief.empty()) brief = forcedBrief;

    if (isInhibited) {
        if (g_hWnd) {
            SetTimer(g_hWnd, TIMER_ID_INHIBIT_RECHECK, INHIBIT_RECHECK_INTERVAL_MS, nullptr);
            KillTimer(g_hWnd, TIMER_ID_QUIET_SWITCH);
            KillTimer(g_hWnd, TIMER_ID_COOLDOWN_SWITCH);
        }
        PrintStatusDashboard(g_state, targetHz, reason);
        Wh_Log(L"Auto Refresh Rate: Switching inhibited by protected application. Re-checking in %u ms.", INHIBIT_RECHECK_INTERVAL_MS);
        return;
    } else {
        if (g_hWnd) {
            KillTimer(g_hWnd, TIMER_ID_INHIBIT_RECHECK);
        }
    }

    DWORD currentHz = GetCurrentPrimaryRefreshRate();
    bool wouldChangeRate = (targetHz != currentHz &&
                            std::abs(static_cast<int>(targetHz) - static_cast<int>(currentHz)) > 1);

    if (wouldChangeRate && !forceOsd) {
        ULONGLONG now64 = GetTickCount64();

        // Anti-flicker cooldown
        if (g_settings.switchCooldownMs > 0 && g_lastSuccessfulSwitchTick > 0) {
            ULONGLONG elapsed = now64 - g_lastSuccessfulSwitchTick;
            if (elapsed < g_settings.switchCooldownMs) {
                DWORD remaining = static_cast<DWORD>(g_settings.switchCooldownMs - elapsed);
                if (g_hWnd) {
                    SetTimer(g_hWnd, TIMER_ID_COOLDOWN_SWITCH, remaining + 50, nullptr);
                }
                Wh_Log(L"Transition cooldown active (%u ms remaining). Deferring switch to %u Hz.", remaining, targetHz);
                return;
            }
        }

        // Mouse drag and selection protection
        if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) || (GetAsyncKeyState(VK_RBUTTON) & 0x8000)) {
            if (g_hWnd) {
                SetTimer(g_hWnd, TIMER_ID_QUIET_SWITCH, 250, nullptr);
            }
            Wh_Log(L"Mouse button held. Deferring switch to %u Hz until release.", targetHz);
            return;
        }

        // Quiet switching: defer until user input is idle
        if (g_settings.quietSwitchEnabled) {
            bool isDrop = (targetHz < currentHz);
            if (isDrop) {
                LASTINPUTINFO lii = { sizeof(lii) };
                if (GetLastInputInfo(&lii)) {
                    DWORD now32 = static_cast<DWORD>(now64);
                    DWORD inactiveMs = now32 - lii.dwTime;
                    if (inactiveMs < QUIET_SWITCH_TIMEOUT_MS) {
                        DWORD waitMs = QUIET_SWITCH_TIMEOUT_MS - inactiveMs + 100;
                        if (waitMs < 250) waitMs = 250;
                        if (g_hWnd) {
                            SetTimer(g_hWnd, TIMER_ID_QUIET_SWITCH, waitMs, nullptr);
                        }
                        Wh_Log(L"User active (input %u ms ago, threshold %u ms). Deferring switch to %u Hz until idle.",
                               inactiveMs, QUIET_SWITCH_TIMEOUT_MS, targetHz);
                        return;
                    }
                }
            }
        }
    }

    if (g_hWnd) {
        KillTimer(g_hWnd, TIMER_ID_QUIET_SWITCH);
        KillTimer(g_hWnd, TIMER_ID_COOLDOWN_SWITCH);
    }

    PrintStatusDashboard(g_state, targetHz, reason);
    ApplyRefreshRateToTargets(targetHz, brief, forceOsd);
}

void CycleRefreshRatesViaHotkey() {
    DEVMODEW dmCurrent = {};
    dmCurrent.dmSize = sizeof(dmCurrent);
    if (!EnumDisplaySettingsExW(nullptr, ENUM_CURRENT_SETTINGS, &dmCurrent, EDS_ROTATEDMODE)) return;

    std::vector<DWORD> rates;
    (void)ResolveRefreshRate(nullptr, 0, dmCurrent, rates);
    if (rates.empty()) return;

    DWORD currentHz = dmCurrent.dmDisplayFrequency;

    if (!g_manualOverrideActive) {
        // Step to the next supported rate above current active frequency, or wrap to lowest
        DWORD nextHz = rates[0];
        for (DWORD r : rates) {
            if (r > currentHz) {
                nextHz = r;
                break;
            }
        }
        g_manualOverrideActive = true;
        g_manualOverrideHz = nextHz;
        Wh_Log(L"Hotkey %s: Locked to %u Hz (was %u Hz).", g_settings.globalHotkey.c_str(), nextHz, currentHz);
        SynchronizeAndApplyPolicy(true, L"Manual lock");
    } else {
        size_t curIdx = rates.size();
        for (size_t i = 0; i < rates.size(); ++i) {
            if (rates[i] == g_manualOverrideHz) { curIdx = i; break; }
        }
        if (curIdx == rates.size()) {
            for (size_t i = 0; i < rates.size(); ++i) {
                if (std::abs(static_cast<int>(rates[i]) - static_cast<int>(g_manualOverrideHz)) <= 1) { curIdx = i; break; }
            }
        }

        if (curIdx >= rates.size() - 1) {
            g_manualOverrideActive = false;
            g_manualOverrideHz = 0;
            Wh_Log(L"Hotkey %s: Returned to automatic mode.", g_settings.globalHotkey.c_str());
            SynchronizeAndApplyPolicy(true, L"Auto mode");
        } else {
            DWORD nextHz = rates[curIdx + 1];
            g_manualOverrideActive = true;
            g_manualOverrideHz = nextHz;
            Wh_Log(L"Hotkey %s: Cycled to %u Hz.", g_settings.globalHotkey.c_str(), nextHz);
            SynchronizeAndApplyPolicy(true, L"Manual lock");
        }
    }
}

VOID CALLBACK WinEventProc(HWINEVENTHOOK, DWORD event, HWND, LONG, LONG, DWORD, DWORD) {
    if (event == EVENT_SYSTEM_FOREGROUND && g_hWnd) {
        if (!g_foregroundPending.exchange(true)) {
            if (!PostMessageW(g_hWnd, WM_APP_FOREGROUND_CHANGED, 0, 0)) {
                g_foregroundPending.store(false);
            }
        }
    }
}

void RegisterAllPowerNotifications(HWND hWnd) {
    for (size_t i = 0; i < g_powerGuids.size(); ++i) {
        g_hPowerNotify[i] = RegisterPowerSettingNotification(hWnd, g_powerGuids[i], DEVICE_NOTIFY_WINDOW_HANDLE);
    }
}

void UnregisterAllPowerNotifications() {
    for (auto& h : g_hPowerNotify) {
        if (h) {
            UnregisterPowerSettingNotification(h);
            h = nullptr;
        }
    }
}

// ============================================================================
// Window Procedure & Worker Thread
// ============================================================================

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_POWERBROADCAST:
        if (wParam == PBT_POWERSETTINGCHANGE) {
            const auto* pSetting = reinterpret_cast<const POWERBROADCAST_SETTING*>(lParam);
            if (pSetting) {
                if (IsEqualGUID(pSetting->PowerSetting, GUID_ACDC_POWER_SOURCE_LOCAL) && pSetting->DataLength >= sizeof(DWORD)) {
                    DWORD val = 0;
                    memcpy(&val, pSetting->Data, sizeof(DWORD));
                    bool newAC = (val == 0);
                    if (newAC != g_state.isAC) {
                        g_state.isAC = newAC;
                        g_manualOverrideActive = false;
                        g_manualOverrideHz = 0;
                    }
                } else if (IsEqualGUID(pSetting->PowerSetting, GUID_BATTERY_PERCENTAGE_REMAINING_LOCAL) && pSetting->DataLength >= sizeof(DWORD)) {
                    DWORD val = 0;
                    memcpy(&val, pSetting->Data, sizeof(DWORD));
                    g_state.batteryPercent = static_cast<BYTE>(val);
                } else if (IsEqualGUID(pSetting->PowerSetting, GUID_POWER_SAVING_STATUS_LOCAL) && pSetting->DataLength >= sizeof(DWORD)) {
                    DWORD val = 0;
                    memcpy(&val, pSetting->Data, sizeof(DWORD));
                    g_state.isBatterySaverActive = (val != 0);
                } else if (IsEqualGUID(pSetting->PowerSetting, GUID_POWERSCHEME_PERSONALITY_LOCAL) && pSetting->DataLength >= sizeof(GUID)) {
                    GUID scheme = GUID_NULL_LOCAL;
                    memcpy(&scheme, pSetting->Data, sizeof(GUID));
                    g_state.powerScheme = scheme;
                }
                SetTimer(hWnd, TIMER_ID_POWER_DEBOUNCE, DEBOUNCE_DELAY_MS, nullptr);
            }
            return TRUE;
        } else if (wParam == PBT_APMRESUMEAUTOMATIC || wParam == PBT_APMRESUMESUSPEND) {
            Wh_Log(L"System resumed. Scheduling display sync in %u ms...", RESUME_DELAY_MS);
            SetTimer(hWnd, TIMER_ID_RESUME_SYNC, RESUME_DELAY_MS, nullptr);
            return TRUE;
        }
        break;

    case WM_HOTKEY:
        if (wParam == HOTKEY_ID_CYCLE) {
            CycleRefreshRatesViaHotkey();
            return 0;
        }
        break;

    case WM_APP_SETTINGS_CHANGED:
        LoadSettings();
        SynchronizeAndApplyPolicy();
        return 0;

    case WM_APP_FOREGROUND_CHANGED:
        SetTimer(hWnd, TIMER_ID_FOREGROUND_DEBOUNCE, FOREGROUND_DEBOUNCE_MS, nullptr);
        return 0;

    case WM_DISPLAYCHANGE:
        InvalidateDisplayDeviceCache();
        SetTimer(hWnd, TIMER_ID_DISPLAY_CHANGE, DISPLAY_CHANGE_DELAY_MS, nullptr);
        break;

    case WM_TIMER:
        if (wParam == TIMER_ID_POWER_DEBOUNCE) {
            KillTimer(hWnd, TIMER_ID_POWER_DEBOUNCE);
            SynchronizeAndApplyPolicy();
            return 0;
        } else if (wParam == TIMER_ID_FOREGROUND_DEBOUNCE) {
            KillTimer(hWnd, TIMER_ID_FOREGROUND_DEBOUNCE);
            g_foregroundPending.store(false);
            SynchronizeAndApplyPolicy();
            return 0;
        } else if (wParam == TIMER_ID_RESUME_SYNC) {
            KillTimer(hWnd, TIMER_ID_RESUME_SYNC);
            SynchronizeAndApplyPolicy();
            return 0;
        } else if (wParam == TIMER_ID_DISPLAY_CHANGE) {
            KillTimer(hWnd, TIMER_ID_DISPLAY_CHANGE);
            SynchronizeAndApplyPolicy();
            return 0;
        } else if (wParam == TIMER_ID_TIME_CHECK) {
            if (g_settings.timeScheduleEnabled) SynchronizeAndApplyPolicy();
            return 0;
        } else if (wParam == TIMER_ID_QUIET_SWITCH || wParam == TIMER_ID_COOLDOWN_SWITCH) {
            KillTimer(hWnd, wParam);
            SynchronizeAndApplyPolicy();
            return 0;
        } else if (wParam == TIMER_ID_INHIBIT_RECHECK) {
            KillTimer(hWnd, TIMER_ID_INHIBIT_RECHECK);
            InvalidateInhibitAppCache();
            SynchronizeAndApplyPolicy();
            return 0;
        }
        break;

    case WM_APP_REAPPLY_POWER_STATE:
        SynchronizeAndApplyPolicy();
        return 0;

    case WM_CLOSE:
        KillTimer(hWnd, TIMER_ID_POWER_DEBOUNCE);
        KillTimer(hWnd, TIMER_ID_FOREGROUND_DEBOUNCE);
        KillTimer(hWnd, TIMER_ID_RESUME_SYNC);
        KillTimer(hWnd, TIMER_ID_DISPLAY_CHANGE);
        KillTimer(hWnd, TIMER_ID_TIME_CHECK);
        KillTimer(hWnd, TIMER_ID_QUIET_SWITCH);
        KillTimer(hWnd, TIMER_ID_COOLDOWN_SWITCH);
        KillTimer(hWnd, TIMER_ID_INHIBIT_RECHECK);

        UnregisterHotKey(hWnd, HOTKEY_ID_CYCLE);
        if (g_hWinEventHook) {
            UnhookWinEvent(g_hWinEventHook);
            g_hWinEventHook = nullptr;
        }
        UnregisterAllPowerNotifications();

        if (g_hOsdWnd) {
            DestroyWindow(g_hOsdWnd);
            g_hOsdWnd = nullptr;
        }
        DestroyWindow(hWnd);
        return 0;

    case WM_DESTROY:
        g_foregroundPending.store(false);
        g_hWnd.store(nullptr);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

DWORD WINAPI PowerMonitorThreadProc(LPVOID lpParam) {
    HANDLE hInitEvent = static_cast<HANDLE>(lpParam);
    HINSTANCE hInstance = GetModuleHandleW(nullptr);

    WNDCLASSEXW wc = { sizeof(wc) };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = g_szClassName;

    if (!RegisterClassExW(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        if (hInitEvent) SetEvent(hInitEvent);
        return 1;
    }

    HWND hWnd = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        g_szClassName, L"WindhawkAutoRefreshRateWindow", WS_POPUP, 0, 0, 0, 0,
        nullptr, nullptr, hInstance, nullptr);

    if (!hWnd) {
        UnregisterClassW(g_szClassName, hInstance);
        if (hInitEvent) SetEvent(hInitEvent);
        return 1;
    }

    g_hWnd.store(hWnd);
    InitializeOsdWindow(hInstance);
    RegisterAllPowerNotifications(hWnd);

    if (g_settings.globalHotkeyEnabled) {
        if (!RegisterHotKey(hWnd, HOTKEY_ID_CYCLE, g_settings.hotkeyModifiers | MOD_NOREPEAT, g_settings.hotkeyVk)) {
            RegisterHotKey(hWnd, HOTKEY_ID_CYCLE, g_settings.hotkeyModifiers, g_settings.hotkeyVk);
        }
    }

    g_foregroundPending.store(false);

    g_hWinEventHook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr, WinEventProc,
        0, 0, WINEVENT_OUTOFCONTEXT);

    if (g_settings.timeScheduleEnabled) {
        SetTimer(hWnd, TIMER_ID_TIME_CHECK, TIME_CHECK_INTERVAL_MS, nullptr);
    }

    if (hInitEvent) SetEvent(hInitEvent);

    SynchronizeAndApplyPolicy();

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_hWinEventHook) {
        UnhookWinEvent(g_hWinEventHook);
        g_hWinEventHook = nullptr;
    }

    UnregisterAllPowerNotifications();

    if (g_hOsdWnd) {
        DestroyWindow(g_hOsdWnd);
        g_hOsdWnd = nullptr;
    }
    HWND hCur = g_hWnd.exchange(nullptr);
    if (hCur) {
        DestroyWindow(hCur);
    }
    UnregisterClassW(g_szClassName, hInstance);
    UnregisterClassW(g_szOsdClassName, hInstance);
    return 0;
}

// ============================================================================
// Windhawk Mod Lifecycle Entry Points
// ============================================================================

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing Auto Refresh Rate mod (Version 1.0.0)...");

    g_hMutex = CreateMutexW(nullptr, FALSE, g_szMutexName);
    if (!g_hMutex || GetLastError() == ERROR_ALREADY_EXISTS) {
        if (g_hMutex) {
            CloseHandle(g_hMutex);
            g_hMutex = nullptr;
        }
        Wh_Log(L"Auto Refresh Rate mod is already active or failed to acquire mutex.");
        return FALSE;
    }

    LoadSettings();

    ScopedHandle hInitEvent(CreateEventW(nullptr, TRUE, FALSE, nullptr));
    if (!hInitEvent) {
        if (g_hMutex) {
            CloseHandle(g_hMutex);
            g_hMutex = nullptr;
        }
        return FALSE;
    }

    g_hThread = CreateThread(nullptr, 0, PowerMonitorThreadProc, hInitEvent.get(), 0, nullptr);
    if (!g_hThread) {
        if (g_hMutex) {
            CloseHandle(g_hMutex);
            g_hMutex = nullptr;
        }
        return FALSE;
    }

    DWORD waitRes = WaitForSingleObject(hInitEvent.get(), 5000);
    HWND hWnd = g_hWnd.load();

    if (waitRes != WAIT_OBJECT_0 || !hWnd) {
        if (hWnd) {
            PostMessageW(hWnd, WM_CLOSE, 0, 0);
        } else if (g_hThread) {
            PostThreadMessageW(GetThreadId(g_hThread), WM_QUIT, 0, 0);
        }
        if (g_hThread) {
            WaitForSingleObject(g_hThread, 5000);
            CloseHandle(g_hThread);
            g_hThread = nullptr;
        }
        if (g_hMutex) {
            CloseHandle(g_hMutex);
            g_hMutex = nullptr;
        }
        return FALSE;
    }

    Wh_Log(L"Auto Refresh Rate mod initialized.");
    return TRUE;
}

void Wh_ModSettingsChanged() {
    HWND hWnd = g_hWnd.load();
    if (hWnd) PostMessageW(hWnd, WM_APP_SETTINGS_CHANGED, 0, 0);
}

void Wh_ModUninit() {
    HWND hWnd = g_hWnd.load();
    if (hWnd) {
        PostMessageW(hWnd, WM_CLOSE, 0, 0);
    } else if (g_hThread) {
        PostThreadMessageW(GetThreadId(g_hThread), WM_QUIT, 0, 0);
    }

    if (g_hThread) {
        WaitForSingleObject(g_hThread, INFINITE);
        CloseHandle(g_hThread);
        g_hThread = nullptr;
    }

    if (g_hMutex) {
        CloseHandle(g_hMutex);
        g_hMutex = nullptr;
    }

    // Restore displays to default settings recorded in registry upon mod unload
    ChangeDisplaySettingsExW(nullptr, nullptr, nullptr, 0, nullptr);

    Wh_Log(L"Auto Refresh Rate mod uninitialized.");
}
