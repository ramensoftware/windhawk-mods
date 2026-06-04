// ==WindhawkMod==
// @id             settings-to-control-panel
// @name           Redirect Settings to Control Panel
// @description    Forces classic Control Panel to open instead of Windows 10/11 Settings app using native components
// @version        9.7.1
// @author         babamohammed
// @github         https://github.com/babamohammed2022
// @license        MIT
// @include        explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Redirect Settings → Control Panel (v9.7.1)

This mod intercepts modern `ms-settings:` URI protocols and forces Windows to open their classic
Control Panel equivalents when possible. It relies entirely on native Windows components and
legacy CLSIDs without requiring any third-party external programs.

**Scope:** This mod only intercepts calls made from within Explorer (explorer.exe).
`ms-settings:` links opened from Start Menu, Search (StartMenuExperienceHost, SearchHost),
or other applications will not be redirected.

### Compatibility & Limitations:
- **Windows 10:** Effective for the mapped entries since most legacy control panels from the Windows 7/8/8.1 era are still intact
  and accessible via standard shell hooks.
- **Windows 11:** Limited. Microsoft has deprecated or hardcoded many legacy CLSIDs.
  However, key elements like network properties (`ncpa.cpl`) and specific dialog mappings
  still function. Unmapped pages pass through to the Settings app by default
  (configurable via FallbackMode).

### Key Features:
- **Smart Desktop Personalization Hook:** Right-clicking the desktop opens the Personalization
  window; clicking "Desktop Background" from inside it opens the wallpaper page.
- **Precise command interception:** Only intercepts `control.exe system` (exact lone argument),
  avoiding false positives from paths containing "System32".

### Settings (v9.7.1):
- **EnableRedirects** — Turns the mod on or off completely.
- **UIOnlyRedirects** — Safer mode: only intercepts ShellExecute calls, not CreateProcessW.
- **SmartPersonalizationDetection** — Contextual awareness for the desktop right-click.
- **FallbackMode** — What happens for unmapped `ms-settings:` URIs.
- **Win11CompatibilityMode** — Skips CLSIDs known to be broken on Win11.
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
  $description: "On Windows 11, replaces CLSIDs known to be broken with a plain Control Panel open."
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

// ── Constants ─────────────────────────────────────────────────────────────────

// ShellExecuteW: any value > 32 signals success to the caller
static const HINSTANCE SHELL_EXECUTE_SUCCESS = (HINSTANCE)33;

#define SYSTEM_PROPS_CLSID  L"shell:::{BB06C0E4-D293-4f75-8A90-CB05B6477EEE}"
#define NOTIF_AREA_CLSID    L"shell:::{05d7b0f4-2121-4eff-bf6b-ed3f69b894d9}"
#define PERS_ROOT_CLSID     L"shell:::{ED834ED6-4B5A-4bfe-8F11-A626DCB6A921}"
#define PERS_WALLPAPER      PERS_ROOT_CLSID L"\\pageWallpaper"
#define PERS_COLORS         PERS_ROOT_CLSID L"\\pageColorization"

// ── Forward declarations ──────────────────────────────────────────────────────

using CreateProcessW_t = BOOL(WINAPI*)(
    LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES,
    BOOL, DWORD, LPVOID, LPCWSTR, LPSTARTUPINFOW, LPPROCESS_INFORMATION);
static CreateProcessW_t CreateProcessW_orig = nullptr;

using ShellExecuteW_t = HINSTANCE(WINAPI*)(HWND, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, INT);
static ShellExecuteW_t ShellExecuteW_orig = nullptr;

// ── Settings ──────────────────────────────────────────────────────────────────

struct ModSettings {
    bool enableRedirects            = true;
    bool uiOnlyRedirects            = false;
    bool smartPersonalizationDetect = true;
    int  fallbackMode               = 2;
    bool win11CompatibilityMode     = false;
};

static ModSettings g_settings;

static void LoadSettings() {
    g_settings.enableRedirects            = Wh_GetIntSetting(L"EnableRedirects") != 0;
    g_settings.uiOnlyRedirects            = Wh_GetIntSetting(L"UIOnlyRedirects") != 0;
    g_settings.smartPersonalizationDetect = Wh_GetIntSetting(L"SmartPersonalizationDetection") != 0;

    PCWSTR fallbackStr = Wh_GetStringSetting(L"FallbackMode");
    if (fallbackStr && fallbackStr[0] != L'\0') {
        int mode = _wtoi(fallbackStr);
        if (mode >= 0 && mode <= 2)
            g_settings.fallbackMode = mode;
        else
            g_settings.fallbackMode = 2;
    } else {
        g_settings.fallbackMode = 2;
    }
    Wh_FreeStringSetting(fallbackStr);

    g_settings.win11CompatibilityMode = Wh_GetIntSetting(L"Win11CompatibilityMode") != 0;

    Wh_Log(L"EnableRedirects=%d  UIOnly=%d  SmartPers=%d  Fallback=%d  Win11Compat=%d",
        (int)g_settings.enableRedirects,
        (int)g_settings.uiOnlyRedirects,
        (int)g_settings.smartPersonalizationDetect,
        g_settings.fallbackMode,
        (int)g_settings.win11CompatibilityMode);
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

static const std::unordered_set<std::wstring> g_win11SafeClsids = {
    L"shell:::{8e908fc9-becc-40f6-915b-f4ca0e70d03d}",
    L"shell:::{7007acc7-3202-11d1-aad2-00805fc1270e}",
    L"shell:::{a8a91a66-3a7d-4424-8d24-04e180695C7A}",
    L"shell:::{4026492f-2f69-46b8-b9bf-5654fc07e423}",
    L"shell:::{20d04fe0-3aea-1069-a2d8-08002b30309d}",
    L"shell:::{60632754-c523-4b62-b45c-4172da012619}",
    L"shell:::{05d7b0f4-2121-4eff-bf6b-ed3f69b894d9}",
};

static bool IsClsidSafeOnWin11(const std::wstring& target) {
    std::wstring lower = target;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::towlower);
    return g_win11SafeClsids.count(lower) > 0;
}

// ── String utilities ──────────────────────────────────────────────────────────

static std::wstring ToLower(std::wstring s) {
    std::transform(s.begin(), s.end(), s.begin(), ::towlower);
    return s;
}

// ── Mappings ──────────────────────────────────────────────────────────────────

static std::unordered_map<std::wstring, std::wstring> g_mappings;

static void InitMappings() {
    g_mappings = {

        // ── Personalization ──────────────────────────────────────────────────
        {L"ms-settings:personalization",                    PERS_ROOT_CLSID},
        {L"ms-settings:personalization-colors",             PERS_COLORS},
        {L"ms-settings:colors",                             PERS_COLORS},
        {L"ms-settings:themes",                             PERS_ROOT_CLSID},
        {L"ms-settings:lockscreen",                         PERS_ROOT_CLSID},
        {L"ms-settings:personalization-start",              PERS_ROOT_CLSID},
        {L"ms-settings:personalization-start-places",       PERS_ROOT_CLSID},
        {L"ms-settings:background",                         PERS_WALLPAPER},
        {L"ms-settings:personalization-background-wallpaper",PERS_WALLPAPER},
        {L"ms-settings:personalization-background-slideshow",PERS_WALLPAPER},
        {L"ms-settings:fonts",
            L"shell:::{BD84B380-8CA2-1069-AB1D-08000948F534}"},

        // ── Color Management ─────────────────────────────────────────────────
        {L"ms-settings:display-advanced-color",             L"colorcpl.exe"},
        {L"ms-settings:colorcpl",                           L"colorcpl.exe"},

        // ── System / About ───────────────────────────────────────────────────
        {L"ms-settings:about",                              SYSTEM_PROPS_CLSID},
        {L"ms-settings:system",                             SYSTEM_PROPS_CLSID},
        {L"ms-settings:sysinfo",                            SYSTEM_PROPS_CLSID},
        {L"ms-settings:system-about",                       SYSTEM_PROPS_CLSID},
        {L"ms-settings:system-protection",                  L"sysdm.cpl,,4"},
        {L"ms-settings:system-remotedesktop",               L"sysdm.cpl,,5"},
        {L"ms-settings:remotedesktop",                      L"sysdm.cpl,,5"},
        {L"ms-settings:devicemanager",                      L"devmgmt.msc"},
        {L"ms-settings:system-devicemanager",               L"devmgmt.msc"},
        {L"ms-settings:computermanagement",                 L"compmgmt.msc"},
        {L"ms-settings:activation",                         L"slui.exe"},
        {L"ms-settings:appsfeatures",                       L"appwiz.cpl"},
        {L"ms-settings:appsforwebsites",                    L"appwiz.cpl"},
        {L"ms-settings:optionalfeatures",                   L"OptionalFeatures.exe"},

        // ── Power ────────────────────────────────────────────────────────────
        {L"ms-settings:powersleep",                         L"powercfg.cpl"},
        {L"ms-settings:battery",                            L"powercfg.cpl"},
        {L"ms-settings:batterysaver",                       L"powercfg.cpl"},
        {L"ms-settings:batterysaver-settings",              L"powercfg.cpl"},
        {L"ms-settings:batterysaver-usagedetails",          L"powercfg.cpl"},

        // ── Sound ────────────────────────────────────────────────────────────
        {L"ms-settings:sound",                              L"mmsys.cpl"},
        {L"ms-settings:sound-devices",                      L"mmsys.cpl"},
        {L"ms-settings:audio",                              L"mmsys.cpl"},
        {L"ms-settings:apps-volume",                        L"sndvol.exe"},

        // ── Notifications / Taskbar ──────────────────────────────────────────
        {L"ms-settings:notifications",                      NOTIF_AREA_CLSID},
        {L"ms-settings:taskbar-notifications",              NOTIF_AREA_CLSID},
        {L"ms-settings:taskbar-systemtray",                 NOTIF_AREA_CLSID},
        {L"ms-settings:notifications-systemtray",           NOTIF_AREA_CLSID},
        {L"ms-settings:systemtray",                         NOTIF_AREA_CLSID},
        {L"ms-settings:notificationiconpreferences",        NOTIF_AREA_CLSID},

        // ── Input devices ────────────────────────────────────────────────────
        {L"ms-settings:mousetouchpad",                      L"main.cpl"},
        {L"ms-settings:devices-touchpad",                   L"main.cpl"},
        {L"ms-settings:keyboard",                           L"main.cpl,,1"},
        {L"ms-settings:typing",                             L"main.cpl,,1"},
        {L"ms-settings:pen",
            L"shell:::{80F3F1D5-FECA-45F3-BC32-752C152E456E}"},
        {L"ms-settings:pen-windowsink",
            L"shell:::{80F3F1D5-FECA-45F3-BC32-752C152E456E}"},
        {L"ms-settings:pen-windowsinksettings",
            L"shell:::{80F3F1D5-FECA-45F3-BC32-752C152E456E}"},
        {L"ms-settings:devices-touch",
            L"shell:::{80F3F1D5-FECA-45F3-BC32-752C152E456E}"},
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
        {L"ms-settings:network-vpn",
            L"shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}"},
        {L"ms-settings:network-airplanemode",
            L"shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}"},
        {L"ms-settings:network-mobilehotspot",
            L"shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}"},
        {L"ms-settings:network-cellular",
            L"shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}"},
        {L"ms-settings:datausage",
            L"shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}"},
        {L"ms-settings:network-proxy",                      L"inetcpl.cpl,,4"},
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
        {L"ms-settings:startupapps",                        L"msconfig.exe"},

        // ── Time & Language ───────────────────────────────────────────────────
        {L"ms-settings:dateandtime",                        L"timedate.cpl"},
        {L"ms-settings:dateandtime-region",                 L"timedate.cpl"},
        {L"ms-settings:dateandtime-addclocks",              L"timedate.cpl,,1"},
        {L"ms-settings:regionlanguage",                     L"intl.cpl"},
        {L"ms-settings:regionformatting",                   L"intl.cpl"},
        {L"ms-settings:language",                           L"intl.cpl"},

        // ── Ease of Access (root only) ───────────────────────────────────────
        {L"ms-settings:easeofaccess",
            L"shell:::{D555645E-D4F8-4c29-A827-D93C859C4F2A}"},

        // ── Default Apps ──────────────────────────────────────────────────────
        {L"ms-settings:defaultapps",
            L"shell:::{17cd9488-1228-4b2f-88ce-4298e93e0966}"},

        // ── Recovery / Backup ─────────────────────────────────────────────────
        {L"ms-settings:backup",
            L"shell:::{B98A2BEA-7D42-4558-8BD1-832F41BAC6FD}"},
        {L"ms-settings:recovery",
            L"shell:::{9FE63AFD-59CF-4419-9775-ABCC3849F861}"},
        {L"ms-settings:troubleshoot",
            L"shell:::{C58C4893-3BE0-4B45-ABB5-A63E4B8C8651}"},
        {L"ms-settings:deviceencryption",
            L"shell:::{D9EF8727-CAC2-4e60-809E-86F80A666C91}"},
    };
}

// ── URI normalization ─────────────────────────────────────────────────────────

static std::wstring NormalizeUri(const std::wstring& uri) {
    std::wstring result = ToLower(uri);

    size_t pos = result.find(L"ms-settings://");
    if (pos != std::wstring::npos)
        result = L"ms-settings:" + result.substr(pos + 14);

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

// ── Win11 compat filter ───────────────────────────────────────────────────────

static std::wstring ApplyWin11Compat(const std::wstring& target) {
    if (!g_settings.win11CompatibilityMode || !g_isWin11) return target;

    std::wstring lower = ToLower(target);
    if (lower.find(L"shell:::") == 0 && !IsClsidSafeOnWin11(lower)) {
        Wh_Log(L"Win11 compat: replacing unsafe CLSID '%s' with control.exe", target.c_str());
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

    // Use ShellExecuteW_orig for targets that need elevation / UAC
    if (command == L"devmgmt.msc" ||
    command == L"compmgmt.msc" ||
    command == L"slui.exe" ||
    command == L"C:\\Windows\\System32\\devmgmt.msc" ||
    command == L"C:\\Windows\\System32\\compmgmt.msc" ||
    command == L"C:\\Windows\\System32\\slui.exe" ||
    command == L"OptionalFeatures.exe" ||
    command == L"C:\\Windows\\System32\\OptionalFeatures.exe") {
    ShellExecuteW_orig(nullptr, L"open", command.c_str(),
                       nullptr, nullptr, SW_SHOWNORMAL);
    return;
}


    STARTUPINFOW si = {};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOWNORMAL;
    PROCESS_INFORMATION pi = {};
    std::wstring cmdLine;

    if (command.find(L".msc") != std::wstring::npos) {
        cmdLine = L"mmc.exe \"" + command + L"\"";
    } else if (command.find(L".exe") != std::wstring::npos ||
               command.find(L".cpl") != std::wstring::npos) {
        if (command.find(L".cpl") != std::wstring::npos)
            cmdLine = L"control.exe " + command;
        else
            cmdLine = command;
    } else if (command.find(L"shell:::") == 0) {
        cmdLine = L"explorer.exe " + command;
    } else if (command.empty()) {
        cmdLine = L"control.exe";
    } else {
        cmdLine = L"control.exe " + command;
    }

    std::wstring mutableCmd = cmdLine;
    if (!CreateProcessW_orig(nullptr, mutableCmd.data(), nullptr, nullptr,
                             FALSE, 0, nullptr, nullptr, &si, &pi)) {
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
            Wh_Log(L"HWND: CabinetWClass -> inside Personalization");
            return true;
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
        return PERS_ROOT_CLSID;
    }
    if (IsPersonalizationWindow(hwnd)) {
        Wh_Log(L"personalization-background -> wallpaper page");
        return PERS_WALLPAPER;
    }
    Wh_Log(L"personalization-background -> Personalization root");
    return PERS_ROOT_CLSID;
}

// ── Core resolve logic ────────────────────────────────────────────────────────

struct ResolveResult {
    std::wstring target;
    bool         intercept;
};

static ResolveResult ResolveUri(const std::wstring& uri, HWND hwnd) {
    if (uri == L"ms-settings:personalization-background") {
        std::wstring t = ApplyWin11Compat(ResolvePersonalizationBackground(hwnd));
        return {t, true};
    }

    auto it = g_mappings.find(uri);
    if (it != g_mappings.end()) {
        std::wstring t = ApplyWin11Compat(it->second);
        Wh_Log(L"Mapped: %s -> %s", uri.c_str(), t.c_str());
        return {t, true};
    }

    if (uri.find(L"ms-settings:") == 0) {
        bool handled = HandleFallback(uri);
        return {L"", handled};
    }

    Wh_Log(L"Unmapped shell target, passing through: %s", uri.c_str());
    return {L"", false};
}

// ── Precise control system detection ─────────────────────────────────────────

static std::wstring BaseNameLower(const std::wstring& path) {
    size_t pos = path.rfind(L'\\');
    return ToLower((pos != std::wstring::npos) ? path.substr(pos + 1) : path);
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

using ShellExecuteExW_t = BOOL(WINAPI*)(SHELLEXECUTEINFOW*);
ShellExecuteExW_t ShellExecuteExW_orig;

BOOL WINAPI ShellExecuteExW_hook(SHELLEXECUTEINFOW* pei) {
    if (!g_settings.enableRedirects || !pei)
        return ShellExecuteExW_orig(pei);

    Wh_Log(L"ShellExecuteExW: hwnd=%p  file=%s  params=%s",
           pei->hwnd,
           pei->lpFile       ? pei->lpFile       : L"(null)",
           pei->lpParameters ? pei->lpParameters : L"(null)");

    std::wstring uri;
    if (IsMsSettings(pei->lpFile))
        uri = NormalizeUri(pei->lpFile);
    else if (IsMsSettings(pei->lpParameters))
        uri = NormalizeUri(pei->lpParameters);
    else if (IsShellClsid(pei->lpFile))
        uri = ToLower(pei->lpFile);
    else if (IsShellClsid(pei->lpParameters))
        uri = ToLower(pei->lpParameters);

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
    if (!g_settings.enableRedirects)
        return ShellExecuteW_orig(hwnd, op, file, params, dir, show);

    Wh_Log(L"ShellExecuteW: hwnd=%p  file=%s  params=%s",
           hwnd,
           file   ? file   : L"(null)",
           params ? params : L"(null)");

    std::wstring uri;
    if (IsMsSettings(file))
        uri = NormalizeUri(file);
    else if (IsMsSettings(params))
        uri = NormalizeUri(params);
    else if (IsShellClsid(file))
        uri = ToLower(file);
    else if (IsShellClsid(params))
        uri = ToLower(params);

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
        LaunchTarget(SYSTEM_PROPS_CLSID);

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
    Wh_Log(L"Redirect Settings to Control Panel v9.7.1 init");

    DetectWindowsVersion();
    LoadSettings();
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

    Wh_Log(L"Ready (EnableRedirects=%d UIOnly=%d SmartPers=%d Fallback=%d Win11Compat=%d)",
        (int)g_settings.enableRedirects,
        (int)g_settings.uiOnlyRedirects,
        (int)g_settings.smartPersonalizationDetect,
        g_settings.fallbackMode,
        (int)g_settings.win11CompatibilityMode);
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Unloaded.");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed, reloading");
    LoadSettings();
}
