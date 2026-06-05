// ==WindhawkMod==
// @id             settings-to-control-panel
// @name           Redirect Settings to Control Panel
// @description    Forces classic Control Panel to open instead of Windows 10/11 Settings app using native components. Primarily designed for Windows 10; Windows 11 support is limited due to Microsoft's shell architecture changes.
// @version        9.8.8
// @author         babamohammed
// @github         https://github.com/babamohammed2022
// @include        explorer.exe
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
# Redirect Settings → Control Panel (v9.8.8)

This Windhawk mod intercepts modern `ms-settings:` URIs and redirects them to
their corresponding classic Control Panel applets, relying exclusively on native
Windows components (no external binaries or dependencies).

---

## Important: Windows 10 vs Windows 11

**This mod is primarily designed for Windows 10**, where the classic Control Panel
infrastructure is fully intact and most user interactions go through `explorer.exe`.

**On Windows 11**, Microsoft rewrote the shell (taskbar, system tray, desktop
right-click menus) using XAML/UWP technology. The modern taskbar uses
`DelegateExecute` COM activation and `ms-contact-support://` protocol instead of
`ShellExecute`, which means:

- **Control Panel navigation** works on both OSes (with Windows 10 being more compatible)
- Some Control Panel components have been **deprecated or removed** by Microsoft on Windows 11
- Some CLSIDs still exist in the registry but open **empty panels**
- **Desktop right-click → Personalize** uses a different code path on Win11 (Could be fixed in future)
- **Start Menu search results** are not intercepted (to avoid breaking search)

When no valid classic equivalent exists, behavior is controlled via fallback
configuration (see Settings section).

---

## What This Mod Does (and Doesn't Do)

### ✅ Redirected (both Windows 10 and 11)
- Control Panel navigation (System, Network, Sound, Accounts, Devices, etc.)
- `Win+R` → `ms-settings:*` commands
- `explorer shell:::*` protocol
- Classic Personalization CPL — including Colors and Background sub-pages
- Right-click "This PC" → Properties
- Troubleshooting → MSDT on Win11 (`msdt.exe`)

### 🟡 Redirected on Windows 10 only
- Desktop right-click → Personalize
- Taskbar notification area settings

### 🔴 Not Redirected (architectural limitation)
- Modern Windows 11 taskbar tray (uses `DelegateExecute` COM activation)
- Start Menu search results (excluded to avoid breaking search)
- Internal Settings app navigation
**Note:** System tray icon clicks (audio, network) are NOT redirected on either OS.
The tray uses DCOM activation which bypasses ShellExecute hooks. This is an
architectural limitation documented above. (Could be fixed soon)

---

## Overview

This mod restores access to the legacy Control Panel experience. When a system
setting link is opened (typically triggering the modern Settings app), it is
transparently redirected to the equivalent classic Control Panel interface.

---

## Features

- Over 90 URI-to-applet mappings covering:
  System, Network, Personalization, Sound, Devices, Accounts, Fonts, and more
- Smart Personalization detection (desktop right-click vs in-window navigation)
- Classic Personalization CPL with working Colors and Background sub-pages
- MSDT troubleshooting on Windows 11
- Safe fallback handling for unmapped settings
- Anti-loop protection (cross-process, in-process, bounce-back detection)
- Lightweight implementation using native Windows APIs only

---

## Configuration Options

- **EnableRedirects**: Master switch — turn the mod on or off
- **UIOnlyRedirects**: Only intercept ShellExecute* calls (leaves CreateProcessW alone)
- **SmartPersonalizationDetection**: Differentiate desktop vs in-window personalization
- **FallbackMode**: Behavior when no classic equivalent exists
  - `0` — Ignore (silent fail)
  - `1` — Open Control Panel root (`control.exe`)
  - `2` — Pass through to modern Settings app (default)
- **Win11CompatibilityMode**: Additional Win11 safeguards for unverified CLSIDs
- **MaxLaunchesPerUri**: Anti-loop safety valve (max launches per URI in 5 seconds)
*/
// ==/WindhawkModReadme==
// ==WindhawkModSettings==
/*
- EnableRedirects: true
  $name: Enable Redirects
  $description: "Turns the mod on or off. When off, all Settings calls open normally."
- UIOnlyRedirects: false
  $name: Non-Invasive UI Mode
  $description: "Only intercepts ShellExecute* calls. Leaves CreateProcessW alone."
- SmartPersonalizationDetection: true
  $name: Smart Personalization Detection
  $description: "Right-clicking the desktop opens Personalization; clicking Desktop Background from inside it opens the wallpaper picker."
- FallbackMode: "2"
  $name: Fallback Mode (unmapped URIs)
  $description: "What to do when a Settings page has no classic equivalent."
  $options:
  - "0": Ignore (silent fail)
  - "1": Open the Control Panel (control.exe)
  - "2": Pass through to the modern Settings application (ms-settings.exe)
- Win11CompatibilityMode: false
  $name: Windows 11 Compatibility Mode
  $description: "On Windows 11, also replaces CLSIDs not confirmed safe (beyond the always-blocked known-loop CLSIDs)."
- MaxLaunchesPerUri: 3
  $name: Loop Guard — max launches per URI (per 5 s)
  $description: "Safety valve: if the same redirect target fires more than this many times in 5 seconds the mod stops launching it. Set to 0 to disable."
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>
#include <windows.h>
#include <shellapi.h>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <mutex>

// ── Constants ─────────────────────────────────────────────────────────────────

static const HINSTANCE SHELL_EXECUTE_SUCCESS = (HINSTANCE)33;

#define SYSTEM_PROPS_CLSID  L"shell:::{BB06C0E4-D293-4f75-8A90-CB05B6477EEE}"
#define NOTIF_AREA_CLSID    L"shell:::{05d7b0f4-2121-4eff-bf6b-ed3f69b894d9}"

#define PERS_ED_CLSID   L"shell:::{ED834ED6-4B5A-4bfe-8F11-A626DCB6A921}"
#define PERS_ROOT       L"explorer shell:::{ED834ED6-4B5A-4bfe-8F11-A626DCB6A921}"
#define PERS_WALLPAPER  L"explorer shell:::{ED834ED6-4B5A-4bfe-8F11-A626DCB6A921} -Microsoft.Personalization\\pageWallpaper"
#define PERS_COLORS     L"explorer shell:::{ED834ED6-4B5A-4bfe-8F11-A626DCB6A921} -Microsoft.Personalization\\pageColorization"

#define WIN11_PASSTHROUGH L"__PASSTHROUGH__"

// ── Forward declarations ──────────────────────────────────────────────────────

using CreateProcessW_t = BOOL(WINAPI*)(
    LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES,
    BOOL, DWORD, LPVOID, LPCWSTR, LPSTARTUPINFOW, LPPROCESS_INFORMATION);
static CreateProcessW_t CreateProcessW_orig = nullptr;

using ShellExecuteW_t = HINSTANCE(WINAPI*)(HWND, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, INT);
using ShellExecuteExW_t = BOOL(WINAPI*)(SHELLEXECUTEINFOW*);
static ShellExecuteExW_t ShellExecuteExW_orig = nullptr;
static ShellExecuteW_t ShellExecuteW_orig = nullptr;

// ── In-process reentry guard ──────────────────────────────────────────────────

static thread_local int g_hookDepth = 0;

struct HookGuard {
    HookGuard()  { ++g_hookDepth; }
    ~HookGuard() { --g_hookDepth; }
    bool IsReentrant() const { return g_hookDepth > 1; }
};

// ── Cross-process reentry guard ───────────────────────────────────────────────

static std::wstring g_childEnvBlock;

static void BuildChildEnvironment() {
    LPWCH curEnv = GetEnvironmentStringsW();
    if (curEnv) {
        LPWCH p = curEnv;
        while (*p) {
            std::wstring entry(p);
            if (entry.find(L"WH_STC_NOREDIRECT=") != 0) {
                g_childEnvBlock += entry + L'\0';
            }
            p += entry.length() + 1;
        }
        FreeEnvironmentStringsW(curEnv);
    }
    g_childEnvBlock += L"WH_STC_NOREDIRECT=1\0\0";
}

static bool IsChildProcess() {
    return GetEnvironmentVariableW(L"WH_STC_NOREDIRECT", nullptr, 0) > 0;
}

// ── Settings ──────────────────────────────────────────────────────────────────

struct ModSettings {
    bool enableRedirects            = true;
    bool uiOnlyRedirects            = false;
    bool smartPersonalizationDetect = true;
    int  fallbackMode               = 2;
    bool win11CompatibilityMode     = false;
    int  maxLaunchesPerUri          = 3;
};

static ModSettings g_settings;

static void LoadSettings() {
    g_settings.enableRedirects            = Wh_GetIntSetting(L"EnableRedirects") != 0;
    g_settings.uiOnlyRedirects            = Wh_GetIntSetting(L"UIOnlyRedirects") != 0;
    g_settings.smartPersonalizationDetect = Wh_GetIntSetting(L"SmartPersonalizationDetection") != 0;

    PCWSTR fallbackStr = Wh_GetStringSetting(L"FallbackMode");
    if (fallbackStr[0] != L'\0') {
        int mode = _wtoi(fallbackStr);
        g_settings.fallbackMode = (mode >= 0 && mode <= 2) ? mode : 2;
    } else {
        g_settings.fallbackMode = 2;
    }
    Wh_FreeStringSetting(fallbackStr);

    g_settings.win11CompatibilityMode = Wh_GetIntSetting(L"Win11CompatibilityMode") != 0;

    int ml = Wh_GetIntSetting(L"MaxLaunchesPerUri");
    g_settings.maxLaunchesPerUri = (ml >= 0 && ml <= 20) ? ml : 3;

    Wh_Log(L"EnableRedirects=%d  UIOnly=%d  SmartPers=%d  Fallback=%d  Win11Compat=%d  MaxLaunches=%d",
        (int)g_settings.enableRedirects,
        (int)g_settings.uiOnlyRedirects,
        (int)g_settings.smartPersonalizationDetect,
        g_settings.fallbackMode,
        (int)g_settings.win11CompatibilityMode,
        g_settings.maxLaunchesPerUri);
}

// ── Win11 detection ───────────────────────────────────────────────────────────

static bool g_isWin11 = false;

static void DetectWindowsVersion() {
    OSVERSIONINFOEXW osvi = {};
    osvi.dwOSVersionInfoSize = sizeof(osvi);
    using RtlGetVersion_t = NTSTATUS(WINAPI*)(OSVERSIONINFOEXW*);
    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (hNtdll) {
        auto fn = (RtlGetVersion_t)GetProcAddress(hNtdll, "RtlGetVersion");
        if (fn) fn(&osvi);
    }
    g_isWin11 = (osvi.dwMajorVersion == 10 &&
                 osvi.dwMinorVersion == 0   &&
                 osvi.dwBuildNumber >= 22000);
    Wh_Log(L"Build %lu  IsWin11=%d", osvi.dwBuildNumber, (int)g_isWin11);
}

// ── Loop guard + Bounce-back detection ───────────────────────────────────────

struct LaunchRecord {
    int   count     = 0;
    DWORD firstTick = 0;
};

struct BounceRecord {
    DWORD lastRedirectTick = 0;
};

static std::mutex                                      g_loopGuardMtx;
static std::unordered_map<std::wstring, LaunchRecord>  g_loopGuard;
static std::unordered_map<std::wstring, BounceRecord>  g_bounceGuard;

static constexpr DWORD LOOP_WINDOW_MS   = 5000;
static constexpr DWORD BOUNCE_WINDOW_MS = 600;

static void BounceGuardRecord(const std::wstring& uri) {
    std::lock_guard<std::mutex> lk(g_loopGuardMtx);
    g_bounceGuard[uri].lastRedirectTick = GetTickCount();
}

static bool BounceGuardIsBounce(const std::wstring& uri) {
    std::lock_guard<std::mutex> lk(g_loopGuardMtx);
    auto it = g_bounceGuard.find(uri);
    if (it == g_bounceGuard.end()) return false;
    DWORD elapsed = GetTickCount() - it->second.lastRedirectTick;
    if (elapsed < BOUNCE_WINDOW_MS) {
        Wh_Log(L"BOUNCE-BACK: '%s' returned %lu ms after redirect — target is dead, routing to fallback",
               uri.c_str(), elapsed);
        it->second.lastRedirectTick = 0;
        return true;
    }
    return false;
}

static bool LoopGuardAllow(const std::wstring& target) {
    if (g_settings.maxLaunchesPerUri <= 0) return true;

    std::lock_guard<std::mutex> lk(g_loopGuardMtx);
    DWORD now = GetTickCount();
    auto& rec = g_loopGuard[target];

    if (rec.count == 0 || (now - rec.firstTick) >= LOOP_WINDOW_MS) {
        rec.count     = 1;
        rec.firstTick = now;
        return true;
    }

    if (rec.count < g_settings.maxLaunchesPerUri) {
        rec.count++;
        return true;
    }

    Wh_Log(L"LOOP GUARD: suppressing launch of '%s' (fired %d times in %lu ms)",
           target.c_str(), rec.count, (now - rec.firstTick));
    return false;
}

// ── CLSID classification ──────────────────────────────────────────────────────

static const std::unordered_set<std::wstring> g_win11SafeClsids = {
    L"shell:::{8e908fc9-becc-40f6-915b-f4ca0e70d03d}",
    L"shell:::{7007acc7-3202-11d1-aad2-00805fc1270e}",
    L"shell:::{a8a91a66-3a7d-4424-8d24-04e180695c7a}",
    L"shell:::{4026492f-2f69-46b8-b9bf-5654fc07e423}",
    L"shell:::{20d04fe0-3aea-1069-a2d8-08002b30309d}",
    L"shell:::{60632754-c523-4b62-b45c-4172da012619}",
    L"shell:::{05d7b0f4-2121-4eff-bf6b-ed3f69b894d9}",
    L"shell:::{2227a280-3aea-1069-a2de-08002b30309d}",
    L"shell:::{59031a47-3f72-44a7-89c5-5595fe6b30ee}",
    L"shell:::{9c60de1e-e5fc-40f4-a487-460851a8d915}",
    L"shell:::{b98a2bea-7d42-4558-8bd1-832f41bac6fd}",
    L"shell:::{bd84b380-8ca2-1069-ab1d-08000948f534}",
    L"shell:::{d555645e-d4f8-4c29-a827-d93c859c4f2a}",
    L"shell:::{d9ef8727-cac2-4e60-809e-86f80a666c91}",
    L"shell:::{c58c4893-3be0-4b45-abb5-a63e4b8c8651}",
};

static const std::unordered_set<std::wstring> g_win11LoopClsids = {
    L"shell:::{bb06c0e4-d293-4f75-8a90-cb05b6477eee}",
    L"shell:::{ed834ed6-4b5a-4bfe-8f11-a626dcb6a921}",
    L"shell:::{17cd9488-1228-4b2f-88ce-4298e93e0966}",
    L"shell:::{80f3f1d5-feca-45f3-bc32-752c152e456e}",
    L"shell:::{9fe63afd-59cf-4419-9775-abcc3849f861}",
};

static bool IsClsidSafeOnWin11(const std::wstring& lowerTarget) {
    return g_win11SafeClsids.count(lowerTarget) > 0;
}

static bool IsClsidLoopOnWin11(const std::wstring& lowerTarget) {
    std::wstring base = lowerTarget;
    size_t brace = base.rfind(L'}');
    if (brace != std::wstring::npos && brace + 1 < base.size())
        base = base.substr(0, brace + 1);
    return g_win11LoopClsids.count(base) > 0;
}

// ── String utilities ──────────────────────────────────────────────────────────

static std::wstring ToLower(std::wstring s) {
    std::transform(s.begin(), s.end(), s.begin(), ::towlower);
    return s;
}

// ── Mappings ──────────────────────────────────────────────────────────────────

static std::unordered_map<std::wstring, std::wstring> g_mappings;

static void InitMappings() {
    const bool w11 = g_isWin11;

    g_mappings = {

        // ── Personalization ──────────────────────────────────────────────────
        {L"ms-settings:personalization",             PERS_ROOT},
        {L"ms-settings:personalization-colors",      PERS_COLORS},
        {L"ms-settings:colors",                      PERS_COLORS},
        {L"ms-settings:themes",                      PERS_ROOT},
        {L"ms-settings:lockscreen",                  PERS_ROOT},
        {L"ms-settings:personalization-start",       PERS_ROOT},
        {L"ms-settings:personalization-start-places",PERS_ROOT},
        {L"ms-settings:background",                  PERS_WALLPAPER},
        {L"ms-settings:personalization-background-wallpaper",  PERS_WALLPAPER},
        {L"ms-settings:personalization-background-slideshow",  PERS_WALLPAPER},

        {L"ms-settings:fonts",
            L"shell:::{BD84B380-8CA2-1069-AB1D-08000948F534}"},

        // ── Color Management ─────────────────────────────────────────────────
        {L"ms-settings:display-advanced-color",              L"colorcpl.exe"},
        {L"ms-settings:colorcpl",                            L"colorcpl.exe"},

        // ── System / About ───────────────────────────────────────────────────
        {L"ms-settings:about",
            w11 ? L"sysdm.cpl" : SYSTEM_PROPS_CLSID},
        {L"ms-settings:system",
            w11 ? L"sysdm.cpl" : SYSTEM_PROPS_CLSID},
        {L"ms-settings:sysinfo",
            w11 ? L"sysdm.cpl" : SYSTEM_PROPS_CLSID},
        {L"ms-settings:system-about",
            w11 ? L"sysdm.cpl" : SYSTEM_PROPS_CLSID},
        {L"ms-settings:system-protection",                   L"sysdm.cpl,,4"},
        {L"ms-settings:system-remotedesktop",                L"sysdm.cpl,,5"},
        {L"ms-settings:remotedesktop",                       L"sysdm.cpl,,5"},
        {L"ms-settings:devicemanager",                       L"devmgmt.msc"},
        {L"ms-settings:system-devicemanager",                L"devmgmt.msc"},
        {L"ms-settings:computermanagement",                  L"compmgmt.msc"},
        {L"ms-settings:activation",                          L"slui.exe"},
        {L"ms-settings:appsfeatures",                        L"appwiz.cpl"},
        {L"ms-settings:appsforwebsites",                     L"appwiz.cpl"},
        {L"ms-settings:optionalfeatures",                    L"OptionalFeatures.exe"},

        // ── Power ────────────────────────────────────────────────────────────
        {L"ms-settings:powersleep",                          L"powercfg.cpl"},
        {L"ms-settings:battery",                             L"powercfg.cpl"},
        {L"ms-settings:batterysaver",                        L"powercfg.cpl"},
        {L"ms-settings:batterysaver-settings",               L"powercfg.cpl"},
        {L"ms-settings:batterysaver-usagedetails",           L"powercfg.cpl"},

        // ── Sound ────────────────────────────────────────────────────────────
        {L"ms-settings:sound",                               L"mmsys.cpl"},
        {L"ms-settings:sound-devices",                       L"mmsys.cpl"},
        {L"ms-settings:audio",                               L"mmsys.cpl"},
        {L"ms-settings:apps-volume",                         L"sndvol.exe"},

        // ── Notifications / Taskbar ──────────────────────────────────────────
        {L"ms-settings:notifications",                       NOTIF_AREA_CLSID},
        {L"ms-settings:taskbar-notifications",               NOTIF_AREA_CLSID},
        {L"ms-settings:taskbar-systemtray",                  NOTIF_AREA_CLSID},
        {L"ms-settings:notifications-systemtray",            NOTIF_AREA_CLSID},
        {L"ms-settings:systemtray",                          NOTIF_AREA_CLSID},
        {L"ms-settings:notificationiconpreferences",         NOTIF_AREA_CLSID},

        // ── Input devices ────────────────────────────────────────────────────
        {L"ms-settings:mousetouchpad",                       L"main.cpl"},
        {L"ms-settings:devices-touchpad",                    L"main.cpl"},
        {L"ms-settings:keyboard",                            L"main.cpl,,1"},
        {L"ms-settings:typing",                              L"main.cpl,,1"},
        {L"ms-settings:pen",
            w11 ? L"control.exe" : L"shell:::{80F3F1D5-FECA-45F3-BC32-752C152E456E}"},
        {L"ms-settings:pen-windowsink",
            w11 ? L"control.exe" : L"shell:::{80F3F1D5-FECA-45F3-BC32-752C152E456E}"},
        {L"ms-settings:pen-windowsinksettings",
            w11 ? L"control.exe" : L"shell:::{80F3F1D5-FECA-45F3-BC32-752C152E456E}"},
        {L"ms-settings:devices-touch",
            w11 ? L"control.exe" : L"shell:::{80F3F1D5-FECA-45F3-BC32-752C152E456E}"},
        {L"ms-settings:autoplay",
            L"shell:::{9C60DE1E-E5FC-40f4-A487-460851A8D915}"},

        // ── Devices / Printers ───────────────────────────────────────────────
        {L"ms-settings:printers",
            L"shell:::{A8A91A66-3A7D-4424-8D24-04E180695C7A}"},
        {L"ms-settings:printers-scanners",
            L"shell:::{2227A280-3AEA-1069-A2DE-08002B30309D}"},
        {L"ms-settings:bluetooth",
            L"shell:::{A8A91A66-3A7D-4424-8D24-04E180695C7A}"},
        {L"ms-settings:usb",
            L"shell:::{A8A91A66-3A7D-4424-8D24-04E180695C7A}"},
        {L"ms-settings:connecteddevices",
            L"shell:::{A8A91A66-3A7D-4424-8D24-04E180695C7A}"},
        {L"ms-settings:mobile-devices",
            L"shell:::{A8A91A66-3A7D-4424-8D24-04E180695C7A}"},
        {L"ms-settings:camera",
            L"shell:::{A8A91A66-3A7D-4424-8D24-04E180695C7A}"},
        {L"ms-settings:privacy-customdevices",
            L"shell:::{A8A91A66-3A7D-4424-8D24-04E180695C7A}"},

        // ── Network ──────────────────────────────────────────────────────────
        {L"ms-settings:network",
            L"shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}"},
        {L"ms-settings:network-wifi",
            L"shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}"},
        {L"ms-settings:network-ethernet",
            L"shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}"},
        {L"ms-settings:network-proxy",                       L"inetcpl.cpl,,4"},
        {L"ms-settings:network-status", 
            L"shell:::{7007ACC7-3202-11D1-AAD2-00805FC1270E}"},
        {L"ms-settings:network-status",
            L"shell:::{7007ACC7-3202-11D1-AAD2-00805FC1270E}"},
        {L"ms-settings:network-dialup",
            L"shell:::{7007ACC7-3202-11D1-AAD2-00805FC1270E}"},
        {L"ms-settings:network-advancedsettings",
            L"shell:::{7007ACC7-3202-11D1-AAD2-00805FC1270E}"},
        {L"ms-settings:firewall",
            L"shell:::{4026492F-2F69-46B8-B9BF-5654FC07E423}"},
        {L"ms-settings:network-firewall",
            L"shell:::{4026492F-2F69-46B8-B9BF-5654FC07E423}"},
        {L"ms-settings:windowsdefender",
            L"shell:::{4026492F-2F69-46B8-B9BF-5654FC07E423}"},

        // ── Accounts ─────────────────────────────────────────────────────────
        {L"ms-settings:yourinfo",
            L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},
        {L"ms-settings:yourinfo-profile",
            L"shell:::{59031a47-3f72-44a7-89c5-5595fe6b30ee}"},
        {L"ms-settings:emailandaccounts",
            L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},
        {L"ms-settings:accounts",
            L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},
        {L"ms-settings:startupapps",                         L"msconfig.exe"},

        // ── Default Apps ──────────────────────────────────────────────────────
        {L"ms-settings:defaultapps",
            w11 ? WIN11_PASSTHROUGH
                : L"shell:::{17cd9488-1228-4b2f-88ce-4298e93e0966}"},

        // ── Time & Language ───────────────────────────────────────────────────
        {L"ms-settings:dateandtime",                         L"timedate.cpl"},
        {L"ms-settings:dateandtime-region",                  L"timedate.cpl"},
        {L"ms-settings:dateandtime-addclocks",               L"timedate.cpl,,1"},
        {L"ms-settings:regionlanguage",                      L"intl.cpl"},
        {L"ms-settings:regionformatting",                    L"intl.cpl"},
        {L"ms-settings:language",                            L"intl.cpl"},

        // ── Ease of Access ────────────────────────────────────────────────────,
        // Ease Of Access entries
        {L"ms-settings:easeofaccess", L"shell:::{D555645E-D4F8-4c29-A827-D93C859C4F2A}"},
        {L"ms-settings:easeofaccess-narrator", L"shell:::{D555645E-D4F8-4c29-A827-D93C859C4F2A}\\pageNoVisual"},
        {L"ms-settings:easeofaccess-magnifier", L"shell:::{D555645E-D4F8-4c29-A827-D93C859C4F2A}\\pageEasierToSee"},
        {L"ms-settings:easeofaccess-speech", L"shell:::{D555645E-D4F8-4c29-A827-D93C859C4F2A"},
        {L"ms-settings:easeofaccess-colorfilter", L"shell:::{D555645E-D4F8-4c29-A827-D93C859C4F2A}\\pageEasierToSee"},
        {L"ms-settings:easeofaccess-display", L"shell:::{D555645E-D4F8-4c29-A827-D93C859C4F2A}\\pageEasierToSee"},

        // ── Recovery / Backup ─────────────────────────────────────────────────
        {L"ms-settings:backup",
            L"shell:::{B98A2BEA-7D42-4558-8BD1-832F41BAC6FD}"},
        {L"ms-settings:recovery",
            w11 ? L"control.exe" : L"shell:::{9FE63AFD-59CF-4419-9775-ABCC3849F861}"},

        // ── Troubleshooting ───────────────────────────────────────────────────
        // added a legacy redirect in windows 11
        {L"ms-settings:troubleshoot",
                w11 ? L"msdt.exe -id DeviceDiagnostic" : L"shell:::{C58C4893-3BE0-4B45-ABB5-A63E4B8C8651}"},

        {L"ms-settings:deviceencryption",
            L"shell:::{D9EF8727-CAC2-4e60-809E-86F80A666C91}"},
    };
}

// ── URI normalization ─────────────────────────────────────────────────────────

static std::wstring NormalizeUri(const std::wstring& uri) {
    std::wstring result = ToLower(uri);

    const std::wstring PROTOCOL = L"ms-settings://";
    size_t pos = result.find(PROTOCOL);
    if (pos != std::wstring::npos)
    { result = L"ms-settings:" + result.substr(pos + PROTOCOL.length()); }

    pos = result.find(L'?');
    if (pos != std::wstring::npos)
        result = result.substr(0, pos);

    while (!result.empty() && result.back() == L'/')
        result.pop_back();

    return result;
}

static bool IsMsSettings(const wchar_t* s) {
    if (!s) return false;
    return ToLower(s).find(L"ms-settings:") != std::wstring::npos;
}

static bool IsShellClsid(const wchar_t* s) {
    if (!s) return false;
    return ToLower(s).find(L"shell:::") != std::wstring::npos;
}

// ── Win11 CLSID filter ────────────────────────────────────────────────────────

static std::wstring ApplyWin11Filter(const std::wstring& target) {
    if (!g_isWin11) return target;

    std::wstring lower = ToLower(target);

    if (lower.find(L"shell:::") != 0) return target;

    if (IsClsidLoopOnWin11(lower)) {
        if (lower.find(L"ed834ed6") != std::wstring::npos) {
            if (lower.find(L"pagewallpaper") != std::wstring::npos) {
                Wh_Log(L"Win11 loop-guard: {ED834ED6}\\pageWallpaper -> PERS_WALLPAPER");
                return PERS_WALLPAPER;
            }
            Wh_Log(L"Win11 loop-guard: {ED834ED6} -> PERS_ROOT");
            return PERS_ROOT;
        }
        if (lower.find(L"bb06c0e4") != std::wstring::npos) {
            Wh_Log(L"Win11 loop-guard: {BB06C0E4} -> sysdm.cpl");
            return L"sysdm.cpl";
        }
        Wh_Log(L"Win11 loop-guard: replacing loop CLSID '%s' with control.exe",
               target.c_str());
        return L"control.exe";
    }

    if (g_settings.win11CompatibilityMode && !IsClsidSafeOnWin11(lower)) {
        Wh_Log(L"Win11 compat: replacing unconfirmed CLSID '%s' with control.exe",
               target.c_str());
        return L"control.exe";
    }

    return target;
}

// ── Fallback handling ─────────────────────────────────────────────────────────

static bool HandleFallback(const std::wstring& uri) {
    switch (g_settings.fallbackMode) {
        case 0:
            Wh_Log(L"Fallback: ignoring unmapped URI: %s", uri.c_str());
            return true;

        case 1: {
            Wh_Log(L"Fallback: opening control.exe for unmapped URI: %s", uri.c_str());
            std::wstring cmd = L"control.exe";
            STARTUPINFOW si = {}; si.cb = sizeof(si);
            si.dwFlags = STARTF_USESHOWWINDOW; si.wShowWindow = SW_SHOWNORMAL;
            PROCESS_INFORMATION pi = {};
            if (CreateProcessW_orig(nullptr, cmd.data(), nullptr, nullptr,
                                    FALSE, 0, nullptr, nullptr, &si, &pi)) {
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
            }
            return true;
        }

        case 2:
        default:
            Wh_Log(L"Fallback: passing through unmapped URI: %s", uri.c_str());
            return false;
    }
}

// ── LaunchTarget ──────────────────────────────────────────────────────────────

static void LaunchTarget(const std::wstring& command) {
    Wh_Log(L"Launching: %s", command.c_str());

    if (!LoopGuardAllow(command)) {
        Wh_Log(L"Launch suppressed by loop guard: %s", command.c_str());
        return;
    }

    {
        std::wstring lower = ToLower(command);
        
        // Gestisci "explorer shell:::" separatamente
        if (lower.find(L"explorer shell:::") != std::wstring::npos) {
            std::wstring clsid = command.substr(command.find(L"shell:::"));
            SHELLEXECUTEINFOW sei = {};
            sei.cbSize = sizeof(sei);
            sei.fMask = SEE_MASK_FLAG_NO_UI;
            sei.lpVerb = L"open";
            sei.lpFile = clsid.c_str();
            sei.nShow = SW_SHOWNORMAL;
            ShellExecuteExW_orig(&sei);
            return;
        }
        
        bool isFullCmdLine =
            (lower.find(L"rundll32.exe ")    != std::wstring::npos) ||
            (lower.find(L"explorer.exe ")    != std::wstring::npos) ||
            (lower.find(L"msdt.exe ")        != std::wstring::npos) ||
            (lower.find(L"control.exe /")    != std::wstring::npos);

        if (isFullCmdLine) {
            STARTUPINFOW si = {}; si.cb = sizeof(si);
            si.dwFlags = STARTF_USESHOWWINDOW; si.wShowWindow = SW_SHOWNORMAL;
            PROCESS_INFORMATION pi = {};
            std::wstring mutable_cmd = command;
            if (!CreateProcessW_orig(nullptr, mutable_cmd.data(), nullptr, nullptr,
                                     FALSE, CREATE_UNICODE_ENVIRONMENT,
                                     (LPVOID)g_childEnvBlock.c_str(),
                                     nullptr, &si, &pi)) {
                Wh_Log(L"CreateProcess failed for '%s' (%lu)",
                       command.c_str(), GetLastError());
            } else {
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
            }
            return;
        }
    }


    // Generic router
    STARTUPINFOW si = {};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOWNORMAL;
    PROCESS_INFORMATION pi = {};
    std::wstring cmdLine;

    if (command.find(L".msc") != std::wstring::npos) {
        cmdLine = L"mmc.exe \"" + command + L"\"";
    } else if (command.find(L".cpl") != std::wstring::npos) {
        cmdLine = L"control.exe " + command;
    } else if (command.find(L".exe") != std::wstring::npos) {
        cmdLine = command;
    } else if (command.find(L"shell:::") == 0) {
        // Apri CLSID direttamente
        SHELLEXECUTEINFOW sei = {};
        sei.cbSize = sizeof(sei);
        sei.fMask = SEE_MASK_FLAG_NO_UI;
        sei.lpVerb = L"open";
        sei.lpFile = command.c_str();
        sei.nShow = SW_SHOWNORMAL;
        ShellExecuteExW_orig(&sei);
        return;
    } else if (command.empty()) {
        cmdLine = L"control.exe";
    } else {
        cmdLine = L"control.exe " + command;
    }

    std::wstring mutableCmd = cmdLine;
    if (!CreateProcessW_orig(nullptr, mutableCmd.data(), nullptr, nullptr,
                             FALSE, CREATE_UNICODE_ENVIRONMENT,
                             (LPVOID)g_childEnvBlock.c_str(),
                             nullptr, &si, &pi)) {
        Wh_Log(L"CreateProcess failed for '%s' (error %lu)",
               cmdLine.c_str(), GetLastError());
        return;
    }
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

// ── Personalization hwnd detection ───────────────────────────────────────────

static bool IsPersonalizationWindow(HWND hwnd) {
    if (!hwnd) {
        Wh_Log(L"HWND null -> desktop context menu path");
        return false;
    }

    HWND h = hwnd;
    while (h) {
        wchar_t cls[256]   = {};
        wchar_t title[512] = {};
        GetClassNameW(h, cls, 256);
        GetWindowTextW(h, title, 512);

        std::wstring c = ToLower(cls);
        std::wstring t = ToLower(title);

        if (c == L"progman" || c == L"workerw" || c == L"shelldll_defview") {
            Wh_Log(L"HWND: desktop class '%s' -> context menu", cls);
            return false;
        }
        if (c == L"cabinetwclass") {
            if (t.find(L"personaliz") != std::wstring::npos) {
                Wh_Log(L"HWND: CabinetWClass personalization confirmed");
                return true;
        }

            Wh_Log(L"HWND: CabinetWClass but not personalization");
            return false;
        }
        if (t.find(L"personaliz") != std::wstring::npos) {
            Wh_Log(L"HWND: title match 'personaliz' -> Personalization window");
            return true;
        }

        HWND parent = GetParent(h);
        if (!parent || parent == h) break;
        h = parent;
    }

    Wh_Log(L"HWND: no personalization window found -> context menu");
    return false;
}

static std::wstring ResolvePersonalizationBackground(HWND hwnd) {
    if (!g_settings.smartPersonalizationDetect) {
        Wh_Log(L"SmartPersonalizationDetection OFF -> Personalization root");
        return PERS_ROOT;
    }
    if (IsPersonalizationWindow(hwnd)) {
        Wh_Log(L"personalization-background -> wallpaper page");
        return PERS_WALLPAPER;
    }
    Wh_Log(L"personalization-background -> Personalization root");
    return PERS_ROOT;
}

// ── Core resolve logic ────────────────────────────────────────────────────────

struct ResolveResult {
    std::wstring target;
    bool         intercept;
};

static ResolveResult ResolveUri(const std::wstring& uri, HWND hwnd) {
    if (uri == L"ms-settings:personalization-background") {
        if (BounceGuardIsBounce(uri)) {
            Wh_Log(L"Bounce-back on personalization-background, routing to fallback");
            bool handled = HandleFallback(uri);
            return {L"", handled};
        }
        std::wstring t = ApplyWin11Filter(ResolvePersonalizationBackground(hwnd));
        BounceGuardRecord(uri);
        return {t, true};
    }

    // ─── EDIT FOR EASE OF ACCESS IN PERSONALIZATION ───

    if (g_settings.smartPersonalizationDetect && IsPersonalizationWindow(hwnd)) {
        if (uri.find(L"ms-settings:easeofaccess") == 0) {
            Wh_Log(L"SmartPersonalization: Accesso da finestra Personalizzazione, forzo radice Centro Accessibilità");
            std::wstring t = ApplyWin11Filter(L"shell:::{D555645E-D4F8-4c29-A827-D93C859C4F2A}");
            BounceGuardRecord(uri);
            return {t, true};
        }
    }
    // ─────────────────────────────────────────────────────────────────

    auto it = g_mappings.find(uri);
    if (it != g_mappings.end()) {
        if (BounceGuardIsBounce(uri)) {
            Wh_Log(L"Bounce-back on '%s', routing to fallback", uri.c_str());
            bool handled = HandleFallback(uri);
            return {L"", handled};
        }

        std::wstring t = ApplyWin11Filter(it->second);

        if (t == WIN11_PASSTHROUGH) {
            Wh_Log(L"Passthrough sentinel for '%s', routing to fallback", uri.c_str());
            bool handled = HandleFallback(uri);
            return {L"", handled};
        }

        Wh_Log(L"Mapped: %s -> %s", uri.c_str(), t.c_str());
        BounceGuardRecord(uri);
        return {t, true};
    }

    if (uri.find(L"ms-settings:") == 0) {
        bool handled = HandleFallback(uri);
        return {L"", handled};
    }

    if (uri.find(L"shell:::") == 0) {
        if (g_isWin11 && IsClsidLoopOnWin11(uri)) {
            std::wstring t = ApplyWin11Filter(uri);
            Wh_Log(L"Win11 loop-CLSID intercepted: %s -> %s", uri.c_str(), t.c_str());
            return {t, true};
        }
    }

    Wh_Log(L"Unmapped, passing through: %s", uri.c_str());
    return {L"", false};
}
// ── Precise control system detection ─────────────────────────────────────────

static std::wstring BaseNameLower(const std::wstring& path) {
    size_t pos = path.rfind(L'\\');
    return ToLower((pos != std::wstring::npos) ? path.substr(pos + 1) : path);
}

static bool IsControlSystemParams(const wchar_t* file, const wchar_t* params) {
    if (!file || !params) return false;
    std::wstring exe = BaseNameLower(file);
    if (exe != L"control.exe" && exe != L"control") return false;
    std::wstring arg = ToLower(params);
    return (arg == L"system" || arg == L"microsoft.system");
}

static bool IsControlSystemCommand(const std::wstring& cmdLine) {
    std::vector<std::wstring> tokens;
    std::wstring current;
    bool inQuotes = false;

    for (wchar_t c : cmdLine) {
        if (c == L'"') {
            inQuotes = !inQuotes;
        } else if (c == L' ' && !inQuotes) {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }
    if (!current.empty())
        tokens.push_back(current);

    if (tokens.size() != 2)
        return false;

    std::wstring exe = BaseNameLower(tokens[0]);
    if (exe != L"control.exe" && exe != L"control")
        return false;

    std::wstring arg = ToLower(tokens[1]);
    return (arg == L"system" || arg == L"microsoft.system");
}

// ── Hook: ShellExecuteExW ─────────────────────────────────────────────────────



BOOL WINAPI ShellExecuteExW_hook(SHELLEXECUTEINFOW* pei) {
    if (IsChildProcess())
        return ShellExecuteExW_orig(pei);

    HookGuard guard;
    if (guard.IsReentrant()) {
        Wh_Log(L"ShellExecuteExW: reentrant call, passing through");
        return ShellExecuteExW_orig(pei);
    }

    if (!g_settings.enableRedirects || !pei)
        return ShellExecuteExW_orig(pei);

    Wh_Log(L"ShellExecuteExW: hwnd=%p  file=%s  params=%s",
           pei->hwnd,
           pei->lpFile       ? pei->lpFile       : L"(null)",
           pei->lpParameters ? pei->lpParameters : L"(null)");

    if (IsControlSystemParams(pei->lpFile, pei->lpParameters)) {
        Wh_Log(L"ShellExecuteExW: intercepted control system");
        LaunchTarget(g_isWin11 ? L"sysdm.cpl" : SYSTEM_PROPS_CLSID);
        if (pei->fMask & SEE_MASK_NOCLOSEPROCESS) pei->hProcess = nullptr;
        return TRUE;
    }

    std::wstring uri;
    if (IsMsSettings(pei->lpFile))
        uri = NormalizeUri(pei->lpFile);
    else if (IsMsSettings(pei->lpParameters))
        uri = NormalizeUri(pei->lpParameters);
    else if (IsShellClsid(pei->lpFile)) {
        std::wstring lower = ToLower(pei->lpFile);
        if (g_isWin11 && IsClsidLoopOnWin11(lower))
            uri = lower;
    }
    else if (IsShellClsid(pei->lpParameters)) {
        std::wstring lower = ToLower(pei->lpParameters);
        if (g_isWin11 && IsClsidLoopOnWin11(lower))
            uri = lower;
    }

    if (!uri.empty()) {
        auto result = ResolveUri(uri, pei->hwnd);
        if (result.intercept) {
            if (!result.target.empty())
                LaunchTarget(result.target);
            if (pei->fMask & SEE_MASK_NOCLOSEPROCESS)
                pei->hProcess = nullptr;
            return TRUE;
        }
    }
    return ShellExecuteExW_orig(pei);
}

// ── Hook: ShellExecuteW ───────────────────────────────────────────────────────

HINSTANCE WINAPI ShellExecuteW_hook(HWND hwnd, LPCWSTR op, LPCWSTR file,
                                     LPCWSTR params, LPCWSTR dir, INT show) {
    if (IsChildProcess())
        return ShellExecuteW_orig(hwnd, op, file, params, dir, show);

    HookGuard guard;
    if (guard.IsReentrant()) {
        Wh_Log(L"ShellExecuteW: reentrant call, passing through");
        return ShellExecuteW_orig(hwnd, op, file, params, dir, show);
    }

    if (!g_settings.enableRedirects)
        return ShellExecuteW_orig(hwnd, op, file, params, dir, show);

    Wh_Log(L"ShellExecuteW: hwnd=%p  file=%s  params=%s",
           hwnd,
           file   ? file   : L"(null)",
           params ? params : L"(null)");

    if (IsControlSystemParams(file, params)) {
        Wh_Log(L"ShellExecuteW: intercepted control system");
        LaunchTarget(g_isWin11 ? L"sysdm.cpl" : SYSTEM_PROPS_CLSID);
        return SHELL_EXECUTE_SUCCESS;
    }

    std::wstring uri;
    if (IsMsSettings(file))
        uri = NormalizeUri(file);
    else if (IsMsSettings(params))
        uri = NormalizeUri(params);
    else if (IsShellClsid(file)) {
        std::wstring lower = ToLower(file);
        if (g_isWin11 && IsClsidLoopOnWin11(lower))
            uri = lower;
    }
    else if (IsShellClsid(params)) {
        std::wstring lower = ToLower(params);
        if (g_isWin11 && IsClsidLoopOnWin11(lower))
            uri = lower;
    }

    if (!uri.empty()) {
        auto result = ResolveUri(uri, hwnd);
        if (result.intercept) {
            if (!result.target.empty())
                LaunchTarget(result.target);
            return SHELL_EXECUTE_SUCCESS;
        }
    }
    return ShellExecuteW_orig(hwnd, op, file, params, dir, show);
}

// ── Hook: CreateProcessW ──────────────────────────────────────────────────────

BOOL WINAPI CreateProcessW_hook(
    LPCWSTR lpApplicationName,
    LPWSTR  lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL    bInheritHandles,
    DWORD   dwCreationFlags,
    LPVOID  lpEnvironment,
    LPCWSTR lpCurrentDirectory,
    LPSTARTUPINFOW       lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation)
{
    if (IsChildProcess()) {
        return CreateProcessW_orig(
            lpApplicationName, lpCommandLine,
            lpProcessAttributes, lpThreadAttributes,
            bInheritHandles, dwCreationFlags,
            lpEnvironment, lpCurrentDirectory,
            lpStartupInfo, lpProcessInformation);
    }

    HookGuard guard;
    if (guard.IsReentrant()) {
        return CreateProcessW_orig(
            lpApplicationName, lpCommandLine,
            lpProcessAttributes, lpThreadAttributes,
            bInheritHandles, dwCreationFlags,
            lpEnvironment, lpCurrentDirectory,
            lpStartupInfo, lpProcessInformation);
    }

    if (!g_settings.enableRedirects || g_settings.uiOnlyRedirects) {
        return CreateProcessW_orig(
            lpApplicationName, lpCommandLine,
            lpProcessAttributes, lpThreadAttributes,
            bInheritHandles, dwCreationFlags,
            lpEnvironment, lpCurrentDirectory,
            lpStartupInfo, lpProcessInformation);
    }

    std::wstring cmdLine = lpCommandLine ? std::wstring(lpCommandLine) : L"";

    if (IsControlSystemCommand(cmdLine)) {
        Wh_Log(L"CreateProcessW: intercepted 'control system': %s", lpCommandLine);
        LaunchTarget(g_isWin11 ? L"sysdm.cpl" : SYSTEM_PROPS_CLSID);

        if (lpProcessInformation)
            ZeroMemory(lpProcessInformation, sizeof(PROCESS_INFORMATION));

        SetLastError(ERROR_SUCCESS);
        return TRUE;
    }

    return CreateProcessW_orig(
        lpApplicationName, lpCommandLine,
        lpProcessAttributes, lpThreadAttributes,
        bInheritHandles, dwCreationFlags,
        lpEnvironment, lpCurrentDirectory,
        lpStartupInfo, lpProcessInformation);
}

// ── Windhawk entry points ─────────────────────────────────────────────────────

BOOL Wh_ModInit() {
    Wh_Log(L"Redirect Settings to Control Panel v9.8.8 init");

    DetectWindowsVersion();
    LoadSettings();
    BuildChildEnvironment();
    InitMappings();
    Wh_Log(L"%zu URI mappings loaded", g_mappings.size());

    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    if (!hShell32) hShell32 = LoadLibraryW(L"shell32.dll");
    if (!hShell32) {
        Wh_Log(L"ERROR: could not load shell32.dll");
        return FALSE;
    }

    auto pExW = (void*)GetProcAddress(hShell32, "ShellExecuteExW");
    auto pW   = (void*)GetProcAddress(hShell32, "ShellExecuteW");
    if (!pExW || !pW) {
        Wh_Log(L"ERROR: required exports not found in shell32.dll");
        return FALSE;
    }

    bool ok1 = Wh_SetFunctionHook(pExW, (void*)ShellExecuteExW_hook,
                                   (void**)&ShellExecuteExW_orig);
    bool ok2 = Wh_SetFunctionHook(pW,   (void*)ShellExecuteW_hook,
                                   (void**)&ShellExecuteW_orig);
    Wh_Log(L"ShellExecuteExW hook=%d  ShellExecuteW hook=%d", ok1, ok2);

    if (!ok1 && !ok2) {
        Wh_Log(L"ERROR: failed to install any hooks");
        return FALSE;
    }

    HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
    if (!hKernel32) hKernel32 = LoadLibraryW(L"kernel32.dll");
    if (hKernel32) {
        auto pCPW = (void*)GetProcAddress(hKernel32, "CreateProcessW");
        if (pCPW) {
            bool ok3 = Wh_SetFunctionHook(pCPW, (void*)CreateProcessW_hook,
                                           (void**)&CreateProcessW_orig);
            Wh_Log(L"CreateProcessW hook=%d", ok3);
        }
    }
    Wh_Log(L"Ready — EnableRedirects=%d UIOnly=%d SmartPers=%d Fallback=%d Win11Compat=%d MaxLaunches=%d",
        (int)g_settings.enableRedirects,
        (int)g_settings.uiOnlyRedirects,
        (int)g_settings.smartPersonalizationDetect,
        g_settings.fallbackMode,
        (int)g_settings.win11CompatibilityMode,
        g_settings.maxLaunchesPerUri);
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Unloaded.");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed, reloading");
    LoadSettings();
    InitMappings();
}
