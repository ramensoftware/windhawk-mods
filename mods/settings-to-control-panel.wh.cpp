// ==WindhawkMod==
// @id             settings-to-control-panel
// @name           Redirect Settings to Control Panel
// @description    Forces classic Control Panel to open instead of Windows 10/11 Settings app using native components
// @version        9.6.0
// @author         babamohammed
// @github         https://github.com/babamohammed2022
// @license        MIT                                        
// @include        explorer.exe
// @compilerOptions -lshell32 -lkernel32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Redirect Settings → Control Panel (v9.6.0)

This mod intercepts modern `ms-settings:` URI protocols and forces Windows to open their classic Control Panel equivalents when possible. It relies entirely on native Windows components and legacy CLSIDs without requiring any third-party external programs.

### Compatibility & Limitations:
- **Windows 10:** Highly effective (~70% redirection success rate), as most legacy control panels are fully intact and accessible via standard shell hooks.
- **Windows 11:** Limited effectiveness (acts as a ~5% baseline restoration). Microsoft has deeply hardcoded or deprecated many legacy CLSIDs, bypassing classic shell activation methods. However, key elements like native network properties (`ncpa.cpl`) and specific dialog mappings still function.

### Key Features:
- **Smart Desktop Personalization Hook:** Contextual awareness based on `hwnd` detection. Clicking "Personalize" from the desktop right-click menu correctly targets the main classic Personalization window, while clicking "Desktop Background" inside an existing shell folder still brings up the wallpaper page.
- **CreateProcess Interception:** Intercepts modern overrides on commands like `control system` to dynamically fall back onto legacy System Properties (`sysdm.cpl`).

### Settings (v9.6.0):
- **EnableRedirects** — Turns the mod on or off completely.
- **UIOnlyRedirects** — Safer mode: only intercepts clicks from Explorer. Leaves scripts and installers alone.
- **SmartPersonalizationDetection** — When ON, right-clicking the desktop opens Personalization; clicking "Desktop Background" from inside it opens the wallpaper picker. When OFF, both always open Personalization.
- **FallbackMode** — What happens when a Settings page has no classic equivalent: do nothing, open Control Panel, or let the original Settings app open.
- **Win11CompatibilityMode** — On Windows 11, skips legacy shortcuts that no longer work and opens Control Panel instead.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- EnableRedirects: true
  $name: Enable Redirects (Kill Switch)
  $description: "Turns the mod on or off. When off, all Settings calls open normally without any redirect."
- UIOnlyRedirects: false
  $name: Non-Invasive UI Mode
  $description: "Only intercepts clicks from Explorer. Safer option if you use scripts, installers, or older apps that might be affected."
- SmartPersonalizationDetection: true
  $name: Smart Personalization Detection
  $description: "When ON, right-clicking the desktop opens Personalization, while clicking Desktop Background from inside it opens the wallpaper picker. When OFF, both always open Personalization."
- FallbackMode: 1
  $name: Fallback Mode (unmapped URIs)
  $description: "What to do when a Settings page has no classic equivalent. Ignore it silently, open Control Panel as a fallback, or let the original Settings app open."
  $options:
  - 0: Ignore (silent fail)
  - 1: Open control.exe
  - 2: Pass through to ms-settings
- Win11CompatibilityMode: false
  $name: Windows 11 Compatibility Mode
  $description: "On Windows 11, replaces legacy shortcuts that no longer work with a plain Control Panel open instead."
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>
#include <windows.h>
#include <shellapi.h>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

#define SYSTEM_PROPERTIES_FALLBACK L"shell:::{BB06C0E4-D293-4f75-8A90-CB05B6477EEE}"
#define NOTIF_AREA_CLSID L"shell:::{05d7b0f4-2121-4eff-bf6b-ed3f69b894d9}"

// ── Settings ─────────────────────────────────────────────────────────────────

struct ModSettings {
    bool enableRedirects            = true;
    bool uiOnlyRedirects            = false;
    bool smartPersonalizationDetect = true;
    int  fallbackMode               = 1;   // 0=ignore, 1=control.exe, 2=passthrough
    bool win11CompatibilityMode     = false;
};

static ModSettings g_settings;

static void LoadSettings() {
    g_settings.enableRedirects            = Wh_GetIntSetting(L"EnableRedirects")            != 0;
    g_settings.uiOnlyRedirects            = Wh_GetIntSetting(L"UIOnlyRedirects")            != 0;
    g_settings.smartPersonalizationDetect = Wh_GetIntSetting(L"SmartPersonalizationDetection") != 0;
    g_settings.fallbackMode               = (int)Wh_GetIntSetting(L"FallbackMode");
    g_settings.win11CompatibilityMode     = Wh_GetIntSetting(L"Win11CompatibilityMode")     != 0;

    Wh_Log(L"[SETTINGS] EnableRedirects=%d  UIOnly=%d  SmartPers=%d  Fallback=%d  Win11Compat=%d",
        (int)g_settings.enableRedirects,
        (int)g_settings.uiOnlyRedirects,
        (int)g_settings.smartPersonalizationDetect,
        g_settings.fallbackMode,
        (int)g_settings.win11CompatibilityMode);
}

// ── Win11 detection (cached) ─────────────────────────────────────────────────

static bool g_isWin11 = false;

static void DetectWindowsVersion() {
    OSVERSIONINFOEXW osvi = {};
    osvi.dwOSVersionInfoSize = sizeof(osvi);
    // Use RtlGetVersion to bypass compat shims
    using RtlGetVersion_t = NTSTATUS(WINAPI*)(OSVERSIONINFOEXW*);
    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (hNtdll) {
        auto fn = (RtlGetVersion_t)GetProcAddress(hNtdll, "RtlGetVersion");
        if (fn) fn(&osvi);
    }
    // Windows 11 is 10.0.22000+
    g_isWin11 = (osvi.dwMajorVersion == 10 && osvi.dwMinorVersion == 0 && osvi.dwBuildNumber >= 22000);
    Wh_Log(L"[VERSION] Build %lu  IsWin11=%d", osvi.dwBuildNumber, (int)g_isWin11);
}

// CLSIDs considered safe on Win11 (well-tested, not deprecated)
static const std::unordered_set<std::wstring> g_win11SafeClsids = {
    L"shell:::{8e908fc9-becc-40f6-915b-f4ca0e70d03d}", // Network and Sharing
    L"shell:::{7007acc7-3202-11d1-aad2-00805fc1270e}", // Network Connections
    L"shell:::{a8a91a66-3a7d-4424-8d24-04e180695c7a}", // Devices and Printers
    L"shell:::{4026492f-2f69-46b8-b9bf-5654fc07e423}", // Windows Firewall
    L"shell:::{20d04fe0-3aea-1069-a2d8-08002b30309d}", // This PC / Computer
    L"shell:::{60632754-c523-4b62-b45c-4172da012619}", // User Accounts
    L"shell:::{05d7b0f4-2121-4eff-bf6b-ed3f69b894d9}", // Notification Area Icons
};

static bool IsClsidSafeOnWin11(const std::wstring& target) {
    std::wstring lower = target;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::towlower);
    return g_win11SafeClsids.count(lower) > 0;
}

// ── Mappings ─────────────────────────────────────────────────────────────────

static std::unordered_map<std::wstring, std::wstring> g_mappings;
static std::unordered_map<std::wstring, std::wstring> g_controlNameMappings;

static std::wstring ToLower(std::wstring s) {
    std::transform(s.begin(), s.end(), s.begin(), ::towlower);
    return s;
}

static void InitMappings() {
    const std::wstring kPers = L"shell:::{ED834ED6-4B5A-4bfe-8F11-A626DCB6A921}";
    const std::wstring kWall = kPers + L"\\pageWallpaper";

    g_mappings = {
        // NOTE: ms-settings:personalization-background is handled specially via hwnd
        // detection in the hook — it is intentionally absent from this table.
        {L"shell:::{52205fd8-5dfb-447d-801a-d0b52f2e83e1}", L"explorer.exe shell:UsersLibrariesFolder"},
        {L"shell:::{bb06c0e4-d293-4f75-8a90-cb05b6477eee}", SYSTEM_PROPERTIES_FALLBACK},
        {L"shell:::{05d7b0f4-2121-4eff-bf6b-ed3f69b894d9}", L"shell:::{05d7b0f4-2121-4eff-bf6b-ed3f69b894d9}"},
        {L"explorer.exe", L"explorer.exe"},
        {L"explorer", L"explorer"},
        {L"ms-settings:personalization-background-wallpaper", kWall},
        {L"ms-settings:personalization-background-slideshow", kWall},
        {L"ms-settings:background", kWall},
        {L"ms-settings:personalization", kPers},
        {L"ms-settings:personalization-colors", kPers + L"\\pageColorization"},
        {L"ms-settings:colors", kPers + L"\\pageColorization"},
        {L"ms-settings:themes", kPers},
        {L"ms-settings:lockscreen", kPers},
        {L"ms-settings:regionlanguage", L"intl.cpl"},
        {L"ms-settings:regionformatting", L"intl.cpl"},
        {L"ms-settings:language", L"intl.cpl"},
        {L"ms-settings:typing", L"main.cpl,,1"},
        {L"ms-settings:system", SYSTEM_PROPERTIES_FALLBACK},
        {L"ms-settings:about", SYSTEM_PROPERTIES_FALLBACK},
        {L"ms-settings:sysinfo", SYSTEM_PROPERTIES_FALLBACK},
        {L"ms-settings:display", L"desk.cpl"},
        {L"ms-settings:display-advanced", L"desk.cpl"},
        {L"ms-settings:notifications", NOTIF_AREA_CLSID},
        {L"ms-settings:backup", L"shell:::{B98A2BEA-7D42-4558-8BD1-832F41BAC6FD}"},
        {L"ms-settings:optionalfeatures", L"OptionalFeatures.exe"},
        {L"ms-settings:devicemanager", L"devmgmt.msc"},
        {L"ms-settings:display-advanced-color", L"colorcpl.exe"},
        {L"ms-settings:colorcpl", L"colorcpl.exe"},
        {L"ms-settings:sound", L"mmsys.cpl"},
        {L"ms-settings:network", L"shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}"},
        {L"ms-settings:network-wifi", L"shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}"},
        {L"ms-settings:network-ethernet", L"shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}"},
        {L"ms-settings:network-vpn", L"shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}"},
        {L"ms-settings:network-proxy", L"inetcpl.cpl,,4"},
        {L"ms-settings:network-status", L"shell:::{7007ACC7-3202-11D1-AAD2-00805FC1270E}"},
        {L"ms-settings:datausage", L"shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}"},
        {L"ms-settings:yourinfo", L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},
        {L"ms-settings:emailandaccounts", L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},
        {L"ms-settings:signinoptions", L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},
        {L"ms-settings:otherusers", L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},
        {L"ms-settings:workplace", L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},
        {L"ms-settings:family", L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},
        {L"ms-settings:dateandtime", L"timedate.cpl"},
        {L"ms-settings:easeofaccess", L"shell:::{D555645E-D4F8-4c29-A827-D93C859C4F2A}"},
        {L"ms-settings:easeofaccess-highcontrast", L"shell:::{D555645E-D4F8-4c29-A827-D93C859C4F2A}"},
        {L"ms-settings:easeofaccess-closedcaptioning", L"shell:::{D555645E-D4F8-4c29-A827-D93C859C4F2A}"},
        {L"ms-settings:printers", L"shell:::{A8A91A66-3A7D-4424-8D24-04E180695C7A}"},
        {L"ms-settings:printers-scanners", L"shell:::{2227A280-3AEA-1069-A2DE-08002B30309D}"},
        {L"ms-settings:mousetouchpad", L"main.cpl"},
        {L"ms-settings:devices-touchpad", L"main.cpl"},
        {L"ms-settings:keyboard", L"main.cpl"},
        {L"ms-settings:bluetooth", L"shell:::{A8A91A66-3A7D-4424-8D24-04E180695C7A}"},
        {L"ms-settings:usb", L"shell:::{A8A91A66-3A7D-4424-8D24-04E180695C7A}"},
        {L"ms-settings:autoplay", L"shell:::{9C60DE1E-E5FC-40f4-A487-460851A8D915}"},
        {L"ms-settings:powersleep", L"powercfg.cpl"},
        {L"ms-settings:pen", L"shell:::{80F3F1D5-FECA-45F3-BC32-752C152E456E}"},
        {L"ms-settings:appsfeatures", L"appwiz.cpl"},
        {L"ms-settings:defaultapps", L"shell:::{17cd9488-1228-4b2f-88ce-4298e93e0966}"},
        {L"ms-settings:firewall", L"shell:::{4026492F-2F69-46B8-B9BF-5654FC07E423}"},
        {L"ms-settings:network-firewall", L"shell:::{4026492F-2F69-46B8-B9BF-5654FC07E423}"},
        {L"ms-settings:home", L"shell:::{59031a47-3f72-44a7-89c5-5595fe6b30ee}"},
        {L"ms-settings:yourinfo-profile", L"shell:::{59031a47-3f72-44a7-89c5-5595fe6b30ee}"},
        {L"ms-settings:storagesense", L"shell:::{20D04FE0-3AEA-1069-A2D8-08002B30309D}"},
        {L"ms-settings:storagepolicies", L"shell:::{20D04FE0-3AEA-1069-A2D8-08002B30309D}"},
        {L"ms-settings:disksandvolumes", L"shell:::{20D04FE0-3AEA-1069-A2D8-08002B30309D}"},
        {L"ms-settings:computerManagement", L"compmgmt.msc"},
        {L"ms-settings:taskbar", NOTIF_AREA_CLSID},
        {L"ms-settings:taskbar-notifications", NOTIF_AREA_CLSID},
        {L"ms-settings:notifications-systemtray", NOTIF_AREA_CLSID},
        {L"ms-settings:systemtray", NOTIF_AREA_CLSID},
        {L"ms-settings:taskbar-systemtray", NOTIF_AREA_CLSID},
        {L"ms-settings:notificationiconpreferences", NOTIF_AREA_CLSID},
        {L"explorer shell:::{ed834ed6-4b5a-4bfe-8f11-a626dcb6a921}", L"explorer shell:::{ED834ED6-4B5A-4bfe-8F11-A626DCB6A921}"},
        {L"shell:::{f20df4e5-ea01-41a2-b02a-dcbd92d4696e}", L"__display__"},
        {L"shell:::{20d04fe0-3aea-1069-a2d8-08002b30309d}", L"shell:::{20D04FE0-3AEA-1069-A2D8-08002B30309D}"},
        {L"shell:::{26ee0668-a00a-44d7-9371-beb064c98683}\\0\\::{05d7b0f4-2121-4eff-bf6b-ed3f69b894d9}", L"shell:::{26ee0668-a00a-44d7-9371-beb064c98683}\\0\\::{05d7b0f4-2121-4eff-bf6b-ed3f69b894d9}"},
        {L"ms-settings:easeofaccess-narrator", L"shell:::{D555645E-D4F8-4c29-A827-D93C859C4F2A}"},
        {L"ms-settings:easeofaccess-magnifier", L"shell:::{D555645E-D4F8-4c29-A827-D93C859C4F2A}"},
        {L"ms-settings:easeofaccess-cursor", L"shell:::{D555645E-D4F8-4c29-A827-D93C859C4F2A}"},
        {L"ms-settings:easeofaccess-keyboard", L"shell:::{D555645E-D4F8-4c29-A827-D93C859C4F2A}"},
        {L"ms-settings:easeofaccess-mouse", L"shell:::{D555645E-D4F8-4c29-A827-D93C859C4F2A}"},
        {L"ms-settings:easeofaccess-eyecontrol", L"shell:::{D555645E-D4F8-4c29-A827-D93C859C4F2A}"},
        {L"ms-settings:easeofaccess-colorfilter", L"shell:::{D555645E-D4F8-4c29-A827-D93C859C4F2A}"},
        {L"ms-settings:easeofaccess-audio", L"shell:::{D555645E-D4F8-4c29-A827-D93C859C4F2A}"},
        {L"ms-settings:easeofaccess-speechrecognition", L"shell:::{D555645E-D4F8-4c29-A827-D93C859C4F2A}"},
        {L"ms-settings:audio", L"mmsys.cpl"},
        {L"ms-settings:system-advanced", SYSTEM_PROPERTIES_FALLBACK},
        {L"ms-settings:system-remotedesktop", L"sysdm.cpl,,5"},
        {L"ms-settings:system-about", SYSTEM_PROPERTIES_FALLBACK},
        {L"ms-settings:system-protection", L"sysdm.cpl,,4"},
        {L"ms-settings:system-devicemanager", L"devmgmt.msc"},
        {L"ms-settings:privacy", L"shell:::{D555645E-D4F8-4c29-A827-D93C859C4F2A}"},
        {L"ms-settings:privacy-webcam", L"shell:::{D555645E-D4F8-4c29-A827-D93C859C4F2A}"},
        {L"ms-settings:privacy-microphone", L"shell:::{D555645E-D4F8-4c29-A827-D93C859C4F2A}"},
        {L"ms-settings:privacy-location", L"shell:::{D555645E-D4F8-4c29-A827-D93C859C4F2A}"},
        {L"ms-settings:accounts", L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},
        {L"ms-settings:accounts-signinoptions", L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},
        {L"ms-settings:accounts-otherusers", L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},
        {L"ms-settings:gaming", L"control.exe"},
        {L"ms-settings:gaming-gamebar", L"control.exe"},
        {L"ms-settings:gaming-gamemode", L"control.exe"},
        {L"ms-settings:gaming-gameDVR", L"control.exe"},
        {L"ms-settings:gaming-broadcasting", L"control.exe"},
        {L"ms-settings:appsforwebsites", L"appwiz.cpl"},
        {L"ms-settings:startupapps", L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},
        {L"ms-settings:videoplayback", L"control.exe"},
        {L"ms-settings:defaultapps-maps", L"shell:::{17cd9488-1228-4b2f-88ce-4298e93e0966}"},
        {L"ms-settings:cortana", L"control.exe"},
        {L"ms-settings:search", L"control.exe"},
        {L"ms-settings:cortana-permissions", L"control.exe"},
        {L"ms-settings:dateandtime-region", L"timedate.cpl"},
        {L"ms-settings:dateandtime-addclocks", L"timedate.cpl,,1"},
        {L"ms-settings:speech", L"control.exe"},
        {L"ms-settings:phone", L"control.exe"},
        {L"ms-settings:phone-defaultapps", L"control.exe"},
        {L"ms-settings:multitasking", L"control.exe"},
        {L"ms-settings:multitasking-snap", L"control.exe"},
        {L"ms-settings:project", L"shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}"},
        {L"ms-settings:pen-windowsink", L"shell:::{80F3F1D5-FECA-45F3-BC32-752C152E456E}"},
        {L"ms-settings:pen-windowsinksettings", L"shell:::{80F3F1D5-FECA-45F3-BC32-752C152E456E}"},
        {L"ms-settings:network-airplanemode", L"shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}"},
        {L"ms-settings:network-mobilehotspot", L"shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}"},
        {L"ms-settings:network-dialup", L"shell:::{7007ACC7-3202-11D1-AAD2-00805FC1270E}"},
        {L"ms-settings:network-advancedsettings", L"shell:::{7007ACC7-3202-11D1-AAD2-00805FC1270E}"},
        {L"ms-settings:battery", L"powercfg.cpl"},
        {L"ms-settings:batterysaver", L"powercfg.cpl"},
        {L"ms-settings:batterysaver-settings", L"powercfg.cpl"},
        {L"ms-settings:batterysaver-usagedetails", L"powercfg.cpl"},
        {L"ms-settings:storagepolicies-other", L"shell:::{20D04FE0-3AEA-1069-A2D8-08002B30309D}"},
        {L"ms-settings:recovery", L"control.exe"},
        {L"ms-settings:recovery-reset", L"control.exe"},
        {L"ms-settings:tabletmode", L"control.exe"},
        {L"ms-settings:connecteddevices", L"shell:::{A8A91A66-3A7D-4424-8D24-04E180695C7A}"},
        {L"ms-settings:signinoptions-face", L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},
        {L"ms-settings:signinoptions-fingerprint", L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},
        {L"ms-settings:signinoptions-dynamiclock", L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},
        {L"ms-settings:easeofaccess-cursorandpointersize", L"shell:::{D555645E-D4F8-4c29-A827-D93C859C4F2A}"},
        {L"ms-settings:easeofaccess-visualeffects", L"shell:::{D555645E-D4F8-4c29-A827-D93C859C4F2A}"},
        {L"ms-settings:easeofaccess-display", L"shell:::{D555645E-D4F8-4c29-A827-D93C859C4F2A}"},
        {L"ms-settings:nearbysharing", L"control.exe"},
        {L"ms-settings:crossdevice", L"control.exe"},
        {L"ms-settings:clipboard", L"control.exe"},
        {L"ms-settings:messaging", L"control.exe"},
        {L"ms-settings:remotedesktop", L"sysdm.cpl,,5"},
        {L"shell:::{26ee0668-a00a-44d7-9371-beb064c98683}\\1\\::{0df44eaa-ff21-4412-828e-260a8728e7f1}", L"wscui.cpl"},
        {L"shell:::{26ee0668-a00a-44d7-9371-beb064c98683}\\2\\::{025a5937-a6be-4686-a844-36fe4bec8b6d}", L"powercfg.cpl"},
        {L"shell:::{26ee0668-a00a-44d7-9371-beb064c98683}\\3\\::{c555438b-3c23-4769-a71f-b6d3d9b6053a}", L"__display__"},
        {L"shell:microsoft.actioncenter", L"wscui.cpl"},
        {L"ms-settings:recovery", L"shell:::{9FE63AFD-59CF-4419-9775-ABCC3849F861}"},
        {L"ms-settings:troubleshoot", L"shell:::{C58C4893-3BE0-4B45-ABB5-A63E4B8C8651}"},
        {L"ms-settings:activation", L"slui.exe"},
        {L"ms-settings:savelocations", L"shell:::{20D04FE0-3AEA-1069-A2D8-08002B30309D}"},
        {L"ms-settings:quiethours", L"shell:::{BB64F8A7-BEE7-4E1A-AB8D-7D8273F7FDB6}"},
        {L"ms-settings:quietmomentsscheduled", L"shell:::{BB64F8A7-BEE7-4E1A-AB8D-7D8273F7FDB6}"},
        {L"ms-settings:quietmomentspresentation", L"shell:::{BB64F8A7-BEE7-4E1A-AB8D-7D8273F7FDB6}"},
        {L"ms-settings:quietmomentsgame", L"shell:::{BB64F8A7-BEE7-4E1A-AB8D-7D8273F7FDB6}"},
        {L"ms-settings:sound-devices", L"mmsys.cpl"},
        {L"ms-settings:apps-volume", L"sndvol.exe"},
        {L"ms-settings:personalization-start", L"shell:::{ED834ED6-4B5A-4bfe-8F11-A626DCB6A921}"},
        {L"ms-settings:personalization-start-places", L"shell:::{ED834ED6-4B5A-4bfe-8F11-A626DCB6A921}"},
        {L"ms-settings:personalization-touchkeyboard", L"shell:::{ED834ED6-4B5A-4bfe-8F11-A626DCB6A921}"},
        {L"ms-settings:deviceusage", L"shell:::{ED834ED6-4B5A-4bfe-8F11-A626DCB6A921}"},
        {L"ms-settings:fonts", L"shell:::{BD84B380-8CA2-1069-AB1D-08000948F534}"},
        {L"ms-settings:nightlight", L"desk.cpl"},
        {L"ms-settings:display-advancedgraphics", L"desk.cpl"},
        {L"ms-settings:easeofaccess-mousepointer", L"shell:::{D555645E-D4F8-4c29-A827-D93C859C4F2A}"},
        {L"ms-settings:network-cellular", L"shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}"},
        {L"ms-settings:privacy-speech", L"shell:::{58E3C745-D971-4081-9034-86E34B30836A}"},
        {L"ms-settings:privacy-feedback", L"shell:::{BB64F8A7-BEE7-4E1A-AB8D-7D8273F7FDB6}"},
        {L"ms-settings:privacy-activityhistory", L"shell:::{BB64F8A7-BEE7-4E1A-AB8D-7D8273F7FDB6}"},
        {L"ms-settings:privacy-accountinfo", L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},
        {L"ms-settings:privacy-contacts", L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},
        {L"ms-settings:privacy-calendar", L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},
        {L"ms-settings:privacy-phonecalls", L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},
        {L"ms-settings:privacy-callhistory", L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},
        {L"ms-settings:privacy-email", L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},
        {L"ms-settings:privacy-tasks", L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},
        {L"ms-settings:privacy-messaging", L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},
        {L"ms-settings:privacy-notifications", L"shell:::{05d7b0f4-2121-4eff-bf6b-ed3f69b894d9}"},
        {L"ms-settings:privacy-voiceactivation", L"shell:::{58E3C745-D971-4081-9034-86E34B30836A}"},
        {L"ms-settings:privacy-radios", L"shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}"},
        {L"ms-settings:privacy-customdevices", L"shell:::{A8A91A66-3A7D-4424-8D24-04E180695C7A}"},
        {L"ms-settings:privacy-appdiagnostics", L"shell:::{BB64F8A7-BEE7-4E1A-AB8D-7D8273F7FDB6}"},
        {L"ms-settings:privacy-automaticfiledownloads", L"shell:::{20D04FE0-3AEA-1069-A2D8-08002B30309D}"},
        {L"ms-settings:privacy-documents", L"shell:::{20D04FE0-3AEA-1069-A2D8-08002B30309D}"},
        {L"ms-settings:privacy-downloadsfolder", L"shell:::{20D04FE0-3AEA-1069-A2D8-08002B30309D}"},
        {L"ms-settings:privacy-musiclibrary", L"shell:::{20D04FE0-3AEA-1069-A2D8-08002B30309D}"},
        {L"ms-settings:privacy-pictures", L"shell:::{20D04FE0-3AEA-1069-A2D8-08002B30309D}"},
        {L"ms-settings:privacy-broadfilesystemaccess", L"shell:::{20D04FE0-3AEA-1069-A2D8-08002B30309D}"},
        {L"ms-settings:privacy-speechtyping", L"shell:::{58E3C745-D971-4081-9034-86E34B30836A}"},
        {L"ms-settings:search-permissions", L"shell:::{04731B67-D933-450a-90E6-4ACD2E9408FE}"},
        {L"ms-settings:cortana-windowssearch", L"shell:::{04731B67-D933-450a-90E6-4ACD2E9408FE}"},
        {L"ms-settings:windowsdefender", L"shell:::{BB64F8A7-BEE7-4E1A-AB8D-7D8273F7FDB6}"},
        {L"ms-settings:findmydevice", L"shell:::{F942C606-0914-47AB-BE56-1321B8035096}"},
        {L"ms-settings:deviceencryption", L"shell:::{D9EF8727-CAC2-4e60-809E-86F80A666C91}"},
        {L"ms-settings:developers", L"control.exe"},
        {L"ms-settings:windowsinsider", L"control.exe"},
        {L"ms-settings:privacy-feedback-telemetryviewergroup", L"shell:::{BB64F8A7-BEE7-4E1A-AB8D-7D8273F7FDB6}"},
        {L"ms-settings:signinoptions-launchfaceenrollment", L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},
        {L"ms-settings:signinoptions-launchfingerprintenrollment", L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},
        {L"ms-settings:signinoptions-launchsecuritykeyenrollment", L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},
        {L"ms-settings:camera", L"shell:::{A8A91A66-3A7D-4424-8D24-04E180695C7A}"},
        {L"ms-settings:devices-touch", L"shell:::{80F3F1D5-FECA-45F3-BC32-752C152E456E}"},
        {L"ms-settings:mobile-devices", L"shell:::{A8A91A66-3A7D-4424-8D24-04E180695C7A}"},
        {L"shell:::{1206f5f1-0569-412c-8fec-3204630dfb70}", L"shell:::{1206F5F1-0569-412C-8FEC-3204630DFB70}"},
        {L"shell:::{d9ef8727-cac2-4e60-809e-86f80a666c91}", L"shell:::{D9EF8727-CAC2-4e60-809E-86F80A666C91}"},
        {L"shell:::{9fe63afd-59cf-4419-9775-abcc3849f861}", L"shell:::{9FE63AFD-59CF-4419-9775-ABCC3849F861}"},
        {L"shell:::{c58c4893-3be0-4b45-abb5-a63e4b8c8651}", L"shell:::{C58C4893-3BE0-4B45-ABB5-A63E4B8C8651}"},
        {L"shell:::{f942c606-0914-47ab-be56-1321b8035096}", L"shell:::{F942C606-0914-47AB-BE56-1321B8035096}"},
        {L"shell:::{241d7c96-f8bf-4f85-b01f-e2b043341a4b}", L"shell:::{241D7C96-F8BF-4F85-B01F-E2B043341A4B}"},
        {L"shell:::{bd7a2e7b-21cb-41b2-a086-b309680c6b7e}", L"shell:::{BD7A2E7B-21CB-41b2-A086-B309680C6B7E}"},
        {L"shell:::{d20ea4e1-3957-11d2-a40b-0c5020524153}", L"shell:::{D20EA4E1-3957-11d2-A40B-0C5020524153}"},
        {L"shell:::{bb64f8a7-bee7-4e1a-ab8d-7d8273f7fdb6}", L"shell:::{BB64F8A7-BEE7-4E1A-AB8D-7D8273F7FDB6}"},
        {L"shell:::{58e3c745-d971-4081-9034-86e34b30836a}", L"shell:::{58E3C745-D971-4081-9034-86E34B30836A}"},
        {L"shell:::{04731b67-d933-450a-90e6-4acd2e9408fe}", L"shell:::{04731B67-D933-450a-90E6-4ACD2E9408FE}"}
    };

    // control.exe /name mappings
    g_controlNameMappings = {
        {L"microsoft.personalization", kPers},
        {L"microsoft.personalization/page pagewallpaper", kWall},
        {L"microsoft.color", kPers + L"\\pageColorization"},
        {L"microsoft.sound", L"mmsys.cpl"},
        {L"microsoft.system", SYSTEM_PROPERTIES_FALLBACK},
        {L"microsoft.programsandfeatures", L"appwiz.cpl"},
        {L"microsoft.devicesandprinters", L"shell:::{A8A91A66-3A7D-4424-8D24-04E180695C7A}"},
        {L"microsoft.networkandsharingcenter", L"shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}"},
        {L"microsoft.windowsfirewall", L"shell:::{4026492F-2F69-46B8-B9BF-5654FC07E423}"},
        {L"microsoft.poweroptions", L"powercfg.cpl"},
        {L"microsoft.mouse", L"main.cpl"},
        {L"microsoft.keyboard", L"main.cpl,,1"},
        {L"microsoft.useraccounts", L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},
        {L"microsoft.easeofaccesscenter", L"shell:::{D555645E-D4F8-4c29-A827-D93C859C4F2A}"},
        {L"microsoft.dateandtime", L"timedate.cpl"},
        {L"microsoft.region", L"intl.cpl"},
        {L"microsoft.defaultprograms", L"shell:::{17cd9488-1228-4b2f-88ce-4298e93e0966}"},
        {L"microsoft.notificationareaicons", NOTIF_AREA_CLSID}
    };
}

// ── URI utilities ─────────────────────────────────────────────────────────────

static std::wstring NormalizeUri(const std::wstring& uri) {
    std::wstring result = ToLower(uri);

    size_t pos = result.find(L"ms-settings://");
    if (pos != std::wstring::npos)
        result = L"ms-settings:" + result.substr(pos + 15);

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

// Applies Win11CompatibilityMode: if the resolved target is a shell CLSID that is
// not in the known-safe list, substitute control.exe.
static std::wstring ApplyWin11Compat(const std::wstring& target) {
    if (!g_settings.win11CompatibilityMode || !g_isWin11) return target;

    // Only shell CLSID targets are at risk of being broken on Win11.
    // Executable targets (.exe, .cpl, .msc) are left as-is.
    std::wstring lower = ToLower(target);
    if (lower.find(L"shell:::") == 0) {
        if (!IsClsidSafeOnWin11(lower)) {
            Wh_Log(L"[WIN11-COMPAT] Replacing unsafe CLSID '%s' with control.exe", target.c_str());
            return L"control.exe";
        }
    }
    return target;
}

// ── Fallback handling ─────────────────────────────────────────────────────────

// Returns false if the caller should fall through to the original function (passthrough mode).
// Returns true if the caller should return early (ignore or control.exe modes were handled).
static bool HandleFallback(const std::wstring& uri) {
    switch (g_settings.fallbackMode) {
        case 0: // ignore
            Wh_Log(L"[FALLBACK] Ignoring unmapped URI: %s", uri.c_str());
            return true;
        case 1: // control.exe
            Wh_Log(L"[FALLBACK] Opening control.exe for unmapped URI: %s", uri.c_str());
            // LaunchTarget is defined below; we call it inline here via a forward path.
            // We use CreateProcessW directly to avoid circular call with LaunchTarget.
            {
                std::wstring cmd = L"control.exe";
                STARTUPINFOW si = {}; si.cb = sizeof(si);
                si.dwFlags = STARTF_USESHOWWINDOW; si.wShowWindow = SW_SHOWNORMAL;
                PROCESS_INFORMATION pi = {};
                CreateProcessW(nullptr, cmd.data(), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi);
                if (pi.hProcess) CloseHandle(pi.hProcess);
                if (pi.hThread)  CloseHandle(pi.hThread);
            }
            return true;
        case 2: // passthrough
        default:
            Wh_Log(L"[FALLBACK] Passing through unmapped URI to original: %s", uri.c_str());
            return false;
    }
}

// ── LaunchTarget ──────────────────────────────────────────────────────────────

static void LaunchTarget(const std::wstring& command) {
    Wh_Log(L"[EXEC] %s", command.c_str());

    STARTUPINFOW si = {};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOWNORMAL;
    PROCESS_INFORMATION pi = {};

    std::wstring cmdLine;
    if (command.find(L".exe") != std::wstring::npos || command.find(L".msc") != std::wstring::npos) {
        cmdLine = command;
    } else if (command.find(L"shell:::") == 0) {
        cmdLine = L"explorer.exe " + command;
    } else if (command.empty()) {
        cmdLine = L"control.exe";
    } else {
        cmdLine = L"control.exe " + command;
    }

    std::wstring mutableCmd = cmdLine;
    if (CreateProcessW(nullptr, mutableCmd.data(), nullptr, nullptr, FALSE,
                       0, nullptr, nullptr, &si, &pi)) {
        if (pi.hProcess) CloseHandle(pi.hProcess);
        if (pi.hThread)  CloseHandle(pi.hThread);
    } else {
        Wh_Log(L"[ERROR] CreateProcess failed for: %s (error: %lu)", cmdLine.c_str(), GetLastError());
    }
}

// ── Personalization hwnd detection ───────────────────────────────────────────

static bool IsPersonalizationWindow(HWND hwnd) {
    if (!hwnd) {
        Wh_Log(L"[HWND] hwnd is NULL -> context menu, not Personalization window");
        return false;
    }

    HWND h = hwnd;
    while (h) {
        wchar_t cls[256]   = {};
        wchar_t title[512] = {};
        GetClassNameW(h, cls, 256);
        GetWindowTextW(h, title, 512);

        Wh_Log(L"[HWND] %p  class='%s'  title='%s'", h, cls, title);

        std::wstring c = ToLower(cls);
        std::wstring t = ToLower(title);

        if (c == L"progman" || c == L"workerw" || c == L"shelldll_defview") {
            Wh_Log(L"[HWND] Desktop window class -> context menu");
            return false;
        }

        if (c == L"cabinetwclass") {
            Wh_Log(L"[HWND] CabinetWClass with title='%s' -> Personalization window", title);
            return true;
        }

        if (t.find(L"personaliz") != std::wstring::npos) {
            Wh_Log(L"[HWND] Title match -> Personalization window: %s", title);
            return true;
        }

        HWND parent = GetParent(h);
        if (!parent || parent == h) break;
        h = parent;
    }

    Wh_Log(L"[HWND] No Personalization window found in chain -> context menu");
    return false;
}

static std::wstring ResolvePersonalizationBackground(HWND hwnd) {
    const std::wstring kPers = L"shell:::{ED834ED6-4B5A-4bfe-8F11-A626DCB6A921}";
    const std::wstring kWall = kPers + L"\\pageWallpaper";

    // If SmartPersonalizationDetection is OFF, always go to the root.
    if (!g_settings.smartPersonalizationDetect) {
        Wh_Log(L"[RESOLVE] SmartPersonalizationDetection=OFF -> Personalization root");
        return kPers;
    }

    if (IsPersonalizationWindow(hwnd)) {
        Wh_Log(L"[RESOLVE] personalization-background -> wallpaper (called from inside Personalization)");
        return kWall;
    } else {
        Wh_Log(L"[RESOLVE] personalization-background -> Personalization (called from context menu)");
        return kPers;
    }
}

// ── Shared hook logic ─────────────────────────────────────────────────────────
// Returns {resolved_target, should_intercept}.
// should_intercept=false means the caller must fall through to the original.

struct ResolveResult {
    std::wstring target;
    bool         intercept;
};

static ResolveResult ResolveUri(const std::wstring& uri, HWND hwnd) {
    if (uri == L"ms-settings:personalization-background") {
        std::wstring t = ResolvePersonalizationBackground(hwnd);
        t = ApplyWin11Compat(t);
        return {t, true};
    }

    auto it = g_mappings.find(uri);
    if (it != g_mappings.end()) {
        std::wstring t = ApplyWin11Compat(it->second);
        Wh_Log(L"[MAP] %s -> %s", uri.c_str(), t.c_str());
        return {t, true};
    }

    // Not in map: apply fallback policy.
    bool handled = HandleFallback(uri);
    // handled=true  → ignore or control.exe already launched, intercept=true so caller exits.
    // handled=false → passthrough requested, intercept=false so caller calls original.
    return {L"", handled};
}

// ── Hook: ShellExecuteExW ─────────────────────────────────────────────────────

using ShellExecuteExW_t = BOOL(WINAPI*)(SHELLEXECUTEINFOW*);
ShellExecuteExW_t ShellExecuteExW_orig;

BOOL WINAPI ShellExecuteExW_hook(SHELLEXECUTEINFOW* pei) {
    if (!g_settings.enableRedirects) return ShellExecuteExW_orig(pei);

    if (pei) {
        Wh_Log(L"[DEBUG] hwnd=%p  lpFile=%s  lpParameters=%s",
               pei->hwnd,
               pei->lpFile       ? pei->lpFile       : L"(null)",
               pei->lpParameters ? pei->lpParameters : L"(null)");

        const wchar_t* file   = pei->lpFile;
        const wchar_t* params = pei->lpParameters;
        std::wstring uri;

        if (IsMsSettings(file) || IsMsSettings(params)) {
            const wchar_t* raw = IsMsSettings(file) ? file : params;
            uri = NormalizeUri(raw);
        } else if (IsShellClsid(file) || IsShellClsid(params)) {
            const wchar_t* raw = IsShellClsid(file) ? file : params;
            uri = ToLower(raw);
        }

        if (!uri.empty()) {
            Wh_Log(L"[HOOK] %s", uri.c_str());
            auto result = ResolveUri(uri, pei->hwnd);

            if (result.intercept) {
                if (!result.target.empty()) LaunchTarget(result.target);
                if (pei->fMask & SEE_MASK_NOCLOSEPROCESS) pei->hProcess = nullptr;
                return TRUE;
            }
            // fallthrough for passthrough mode
        }
    }
    return ShellExecuteExW_orig(pei);
}

// ── Hook: ShellExecuteW ───────────────────────────────────────────────────────

using ShellExecuteW_t = HINSTANCE(WINAPI*)(HWND, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, INT);
ShellExecuteW_t ShellExecuteW_orig;

HINSTANCE WINAPI ShellExecuteW_hook(HWND hwnd, LPCWSTR op, LPCWSTR file,
                                     LPCWSTR params, LPCWSTR dir, INT show) {
    if (!g_settings.enableRedirects) return ShellExecuteW_orig(hwnd, op, file, params, dir, show);

    Wh_Log(L"[DEBUG-W] hwnd=%p  file=%s  params=%s",
           hwnd,
           file   ? file   : L"(null)",
           params ? params : L"(null)");

    std::wstring uri;

    if (IsMsSettings(file) || IsMsSettings(params)) {
        const wchar_t* raw = IsMsSettings(file) ? file : params;
        uri = NormalizeUri(raw);
    } else if (IsShellClsid(file) || IsShellClsid(params)) {
        const wchar_t* raw = IsShellClsid(file) ? file : params;
        uri = ToLower(raw);
    }

    if (!uri.empty()) {
        Wh_Log(L"[HOOK-W] %s", uri.c_str());
        auto result = ResolveUri(uri, hwnd);

        if (result.intercept) {
            if (!result.target.empty()) LaunchTarget(result.target);
            return (HINSTANCE)42;
        }
        // fallthrough for passthrough mode
    }
    return ShellExecuteW_orig(hwnd, op, file, params, dir, show);
}

// ── Hook: CreateProcessW ──────────────────────────────────────────────────────

using CreateProcessW_t = BOOL(WINAPI*)(
    LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES,
    BOOL, DWORD, LPVOID, LPCWSTR, LPSTARTUPINFOW, LPPROCESS_INFORMATION
);
CreateProcessW_t CreateProcessW_orig = nullptr;

BOOL WINAPI CreateProcessW_hook(
    LPCWSTR lpApplicationName,
    LPWSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL bInheritHandles,
    DWORD dwCreationFlags,
    LPVOID lpEnvironment,
    LPCWSTR lpCurrentDirectory,
    LPSTARTUPINFOW lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation)
{
    // Respect global kill switch and UI-only mode.
    if (!g_settings.enableRedirects || g_settings.uiOnlyRedirects) {
        return CreateProcessW_orig(
            lpApplicationName, lpCommandLine,
            lpProcessAttributes, lpThreadAttributes,
            bInheritHandles, dwCreationFlags,
            lpEnvironment, lpCurrentDirectory,
            lpStartupInfo, lpProcessInformation);
    }

    std::wstring cmdLine = lpCommandLine ? ToLower(std::wstring(lpCommandLine)) : L"";
    
    if (cmdLine.find(L"control") != std::wstring::npos && 
        cmdLine.find(L"system") != std::wstring::npos &&
        cmdLine.find(L"mmsys.cpl") == std::wstring::npos &&
        cmdLine.find(L"timedate.cpl") == std::wstring::npos &&
        cmdLine.find(L"main.cpl") == std::wstring::npos &&
        cmdLine.find(L"ncpa.cpl") == std::wstring::npos &&
        cmdLine.find(L"inetcpl.cpl") == std::wstring::npos &&
        cmdLine.find(L"desk.cpl") == std::wstring::npos &&
        cmdLine.find(L"telephon.cpl") == std::wstring::npos &&
        cmdLine.find(L"control.exe srchadmin.dll") == std::wstring::npos &&
        cmdLine.find(L"intl.cpl") == std::wstring::npos &&
        cmdLine.find(L"mmsys.cpl") == std::wstring::npos &&
        cmdLine.find(L"microsoft.sound") == std::wstring::npos &&
        cmdLine.find(L"sndvol") == std::wstring::npos) {

        Wh_Log(L"[HOOK-CPW] Intercepted control system command: %s", lpCommandLine);
        LaunchTarget(L"shell:::{BB06C0E4-D293-4f75-8A90-CB05B6477EEE}");
        SetLastError(ERROR_SUCCESS);
        return TRUE;
    }
    
    return CreateProcessW_orig(
        lpApplicationName, lpCommandLine,
        lpProcessAttributes, lpThreadAttributes,
        bInheritHandles, dwCreationFlags,
        lpEnvironment, lpCurrentDirectory,
        lpStartupInfo, lpProcessInformation
    );
}

// ── Windhawk Entry Points ─────────────────────────────────────────────────────

BOOL Wh_ModInit() {
    Wh_Log(L"[v9.6.0] Redirect Settings to Control Panel - Initializing...");

    DetectWindowsVersion();
    LoadSettings();
    InitMappings();
    Wh_Log(L"[v9.6.0] %zu URI mappings loaded", g_mappings.size());

    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    if (!hShell32) hShell32 = LoadLibraryW(L"shell32.dll");
    if (!hShell32) {
        Wh_Log(L"[v9.6.0] ERROR: Could not load shell32.dll");
        return FALSE;
    }

    auto pExW = (void*)GetProcAddress(hShell32, "ShellExecuteExW");
    auto pW   = (void*)GetProcAddress(hShell32, "ShellExecuteW");

    if (!pExW || !pW) {
        Wh_Log(L"[v9.6.0] ERROR: Required functions not found in shell32.dll");
        return FALSE;
    }

    bool ok1 = Wh_SetFunctionHook(pExW, (void*)ShellExecuteExW_hook, (void**)&ShellExecuteExW_orig);
    bool ok2 = Wh_SetFunctionHook(pW,   (void*)ShellExecuteW_hook,   (void**)&ShellExecuteW_orig);

    Wh_Log(L"[v9.6.0] Hook results: ShellExecuteExW=%d  ShellExecuteW=%d", ok1, ok2);

    // CreateProcessW hook: only install if UIOnlyRedirects is OFF.
    // We always install but the hook itself checks g_settings.uiOnlyRedirects at runtime,
    // which allows settings changes without a full reinit.
    HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
    if (!hKernel32) hKernel32 = LoadLibraryW(L"kernel32.dll");
    if (hKernel32) {
        auto pCPW = (void*)GetProcAddress(hKernel32, "CreateProcessW");
        if (pCPW) {
            bool ok3 = Wh_SetFunctionHook(pCPW, (void*)CreateProcessW_hook, (void**)&CreateProcessW_orig);
            Wh_Log(L"[v9.6.0] CreateProcessW hook: %d", ok3);
        }
    }

    if (!ok1 && !ok2) {
        Wh_Log(L"[v9.6.0] ERROR: Failed to install any hooks");
        return FALSE;
    }

    Wh_Log(L"[v9.6.0] Ready! (EnableRedirects=%d, UIOnly=%d, SmartPers=%d, Fallback=%d, Win11Compat=%d)",
        (int)g_settings.enableRedirects,
        (int)g_settings.uiOnlyRedirects,
        (int)g_settings.smartPersonalizationDetect,
        g_settings.fallbackMode,
        (int)g_settings.win11CompatibilityMode);
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"[v9.6.0] Unloaded.");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"[v9.6.0] Settings changed, reloading...");
    LoadSettings();
}
