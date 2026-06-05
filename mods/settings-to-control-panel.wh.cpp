// ==WindhawkMod==
// @id             settings-to-control-panel
// @name           Redirect Settings to Control Panel
// @description    Forces classic Control Panel to open instead of Windows 10/11 Settings app using native components. Primarily designed for Windows 10; Windows 11 support is limited due to Microsoft's shell architecture changes.
// @version        9.9.6
// @author         babamohammed
// @github         https://github.com/babamohammed2022
// @include        explorer.exe
// @include        SystemSettings.exe
// @include        StartMenuExperienceHost.exe
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
# Redirect Settings → Control Panel (v9.9.6)

Debug version - logging all ShellExecuteW calls to diagnose audio tray issues.
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
  - "2": Pass through to the modern Settings application (ms-settings.exe)"
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
#include <objbase.h>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <mutex>
#include <shobjidl.h>     
#include <initguid.h>  

// Dynamic COM handling - no linking required
typedef HRESULT (WINAPI *CoCreateInstance_t)(const GUID* rclsid, IUnknown* pUnkOuter, DWORD dwClsContext, const GUID* riid, void** ppv);
static CoCreateInstance_t dyn_CoCreateInstance = nullptr;
static HMODULE g_hOle32 = nullptr;

// IShellDispatch2 vtable hook
using IShellDispatch2_ShellExecute_t = HRESULT(WINAPI*)(void* pThis, BSTR File, void* vArgs, void* vDir, void* vOperation, void* vShow);
static IShellDispatch2_ShellExecute_t IShellDispatch2_ShellExecute_orig = nullptr;

// CLSID_Shell / IID_IShellDispatch2
static const GUID CLSID_Shell_ = {0x13709620,0xC279,0x11CE,{0xA4,0x9E,0x44,0x45,0x53,0x54,0x00,0x00}};
static const GUID IID_IShellDispatch2_ = {0xA4C6892C,0x3BA9,0x11D2,{0x9D,0xEA,0x00,0xC0,0x4F,0xB9,0x60,0x3F}};

// Constants
static const HINSTANCE SHELL_EXECUTE_SUCCESS = (HINSTANCE)33;

#define SYSTEM_PROPS_CLSID  L"shell:::{BB06C0E4-D293-4f75-8A90-CB05B6477EEE}"
#define NOTIF_AREA_CLSID    L"shell:::{05d7b0f4-2121-4eff-bf6b-ed3f69b894d9}"
#define PERS_ROOT       L"explorer shell:::{ED834ED6-4B5A-4bfe-8F11-A626DCB6A921}"
#define PERS_WALLPAPER  L"explorer shell:::{ED834ED6-4B5A-4bfe-8F11-A626DCB6A921} -Microsoft.Personalization\\pageWallpaper"
#define PERS_COLORS     L"explorer shell:::{ED834ED6-4B5A-4bfe-8F11-A626DCB6A921} -Microsoft.Personalization\\pageColorization"
#define WIN11_PASSTHROUGH L"__PASSTHROUGH__"
#define EASE_OF_ACCESS  L"control.exe /name Microsoft.EaseOfAccessCenter"

// Forward declarations
using CreateProcessW_t = BOOL(WINAPI*)(LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, BOOL, DWORD, LPVOID, LPCWSTR, LPSTARTUPINFOW, LPPROCESS_INFORMATION);
static CreateProcessW_t CreateProcessW_orig = nullptr;

using ShellExecuteW_t = HINSTANCE(WINAPI*)(HWND, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, INT);
using ShellExecuteExW_t = BOOL(WINAPI*)(SHELLEXECUTEINFOW*);
static ShellExecuteExW_t ShellExecuteExW_orig = nullptr;
static ShellExecuteW_t ShellExecuteW_orig = nullptr;

// Reentry guards
static thread_local int g_hookDepth = 0;

struct HookGuard {
    HookGuard() { ++g_hookDepth; }
    ~HookGuard() { --g_hookDepth; }
    bool IsReentrant() const { return g_hookDepth > 1; }
};

// Cross-process reentry guard
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

// Settings
struct ModSettings {
    bool enableRedirects = true;
    bool uiOnlyRedirects = false;
    bool smartPersonalizationDetect = true;
    int fallbackMode = 2;
    bool win11CompatibilityMode = false;
    int maxLaunchesPerUri = 3;
};

static ModSettings g_settings;

static void LoadSettings() {
    g_settings.enableRedirects = Wh_GetIntSetting(L"EnableRedirects") != 0;
    g_settings.uiOnlyRedirects = Wh_GetIntSetting(L"UIOnlyRedirects") != 0;
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

    Wh_Log(L"EnableRedirects=%d UIOnly=%d SmartPers=%d Fallback=%d Win11Compat=%d MaxLaunches=%d",
        (int)g_settings.enableRedirects, (int)g_settings.uiOnlyRedirects,
        (int)g_settings.smartPersonalizationDetect, g_settings.fallbackMode,
        (int)g_settings.win11CompatibilityMode, g_settings.maxLaunchesPerUri);
}

// Win11 detection
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
    g_isWin11 = (osvi.dwMajorVersion == 10 && osvi.dwMinorVersion == 0 && osvi.dwBuildNumber >= 22000);
    Wh_Log(L"Build %lu IsWin11=%d", osvi.dwBuildNumber, (int)g_isWin11);
}

// Loop guard
struct LaunchRecord {
    int count = 0;
    DWORD firstTick = 0;
};

static std::mutex g_loopGuardMtx;
static std::unordered_map<std::wstring, LaunchRecord> g_loopGuard;

static constexpr DWORD LOOP_WINDOW_MS = 3000;

static bool LoopGuardAllow(const std::wstring& target) {
    if (g_settings.maxLaunchesPerUri <= 0) return true;

    std::lock_guard<std::mutex> lk(g_loopGuardMtx);
    DWORD now = GetTickCount();
    auto& rec = g_loopGuard[target];

    if (rec.count == 0 || (now - rec.firstTick) >= LOOP_WINDOW_MS) {
        rec.count = 1;
        rec.firstTick = now;
        return true;
    }

    if (rec.count < g_settings.maxLaunchesPerUri) {
        rec.count++;
        return true;
    }

    Wh_Log(L"LOOP GUARD: suppressing launch of '%s' (%d times in %lu ms)", target.c_str(), rec.count, (now - rec.firstTick));
    return false;
}

// CLSID classification
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

static const std::unordered_set<std::wstring> g_win11NoForceRedirect = {
    L"ms-settings:defaultapps",
    L"ms-settings:appsfeatures",
    L"ms-settings:network",
    L"ms-settings:bluetooth",
    L"ms-settings:printers",
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

// String utilities
static std::wstring ToLower(std::wstring s) {
    std::transform(s.begin(), s.end(), s.begin(), ::towlower);
    return s;
}

// Mappings
static std::unordered_map<std::wstring, std::wstring> g_mappings;

static void InitMappings() {
    const bool w11 = g_isWin11;

    g_mappings = {
        // Personalization
        {L"ms-settings:personalization", PERS_ROOT},
        {L"ms-settings:personalization-colors", PERS_COLORS},
        {L"ms-settings:colors", PERS_COLORS},
        {L"ms-settings:themes", PERS_ROOT},
        {L"ms-settings:lockscreen", PERS_ROOT},
        {L"ms-settings:personalization-start", PERS_ROOT},
        {L"ms-settings:personalization-start-places", PERS_ROOT},
        {L"ms-settings:background", PERS_WALLPAPER},
        {L"ms-settings:personalization-background-wallpaper", PERS_WALLPAPER},
        {L"ms-settings:personalization-background-slideshow", PERS_WALLPAPER},
        
        // Fonts & Color
        {L"ms-settings:fonts", L"shell:::{BD84B380-8CA2-1069-AB1D-08000948F534}"},
        {L"ms-settings:display-advanced-color", L"colorcpl.exe"},
        {L"ms-settings:colorcpl", L"colorcpl.exe"},
        
        // Display (using rundll32 for advanced properties)
        {L"ms-settings:display", L"rundll32.exe display.dll,ShowAdapterSettings 0"},
        {L"ms-settings:display-advanced", L"rundll32.exe display.dll,ShowAdapterSettings 0"},
        {L"ms-settings:display-advanced-graphics", L"rundll32.exe display.dll,ShowAdapterSettings 0"},
        {L"ms-settings:graphics-settings", L"rundll32.exe display.dll,ShowAdapterSettings 0"},
        {L"ms-settings:display-adapter-properties", L"rundll32.exe display.dll,ShowAdapterSettings 0"},
        
        // System
        {L"ms-settings:about", w11 ? L"sysdm.cpl" : SYSTEM_PROPS_CLSID},
        {L"ms-settings:system", w11 ? L"sysdm.cpl" : SYSTEM_PROPS_CLSID},
        {L"ms-settings:sysinfo", w11 ? L"sysdm.cpl" : SYSTEM_PROPS_CLSID},
        {L"ms-settings:system-about", w11 ? L"sysdm.cpl" : SYSTEM_PROPS_CLSID},
        {L"ms-settings:system-protection", L"sysdm.cpl,,4"},
        {L"ms-settings:system-remotedesktop", L"sysdm.cpl,,5"},
        {L"ms-settings:remotedesktop", L"sysdm.cpl,,5"},
        {L"ms-settings:devicemanager", L"devmgmt.msc"},
        {L"ms-settings:system-devicemanager", L"devmgmt.msc"},
        {L"ms-settings:computermanagement", L"compmgmt.msc"},
        {L"ms-settings:activation", L"slui.exe"},
        {L"ms-settings:appsfeatures", L"appwiz.cpl"},
        {L"ms-settings:appsforwebsites", L"appwiz.cpl"},
        {L"ms-settings:optionalfeatures", L"OptionalFeatures.exe"},
        
        // Power
        {L"ms-settings:powersleep", L"powercfg.cpl"},
        {L"ms-settings:battery", L"powercfg.cpl"},
        {L"ms-settings:batterysaver", L"powercfg.cpl"},
        {L"ms-settings:batterysaver-settings", L"powercfg.cpl"},
        {L"ms-settings:batterysaver-usagedetails", L"powercfg.cpl"},
        
        // Sound
        {L"ms-settings:audio", L"mmsys.cpl"},
        {L"ms-settings:sound-control-panel", L"control.exe /name Microsoft.Sound"},
        {L"ms-settings:sound-playback", L"control.exe mmsys.cpl,,0"},
        {L"ms-settings:sound-recording", L"control.exe mmsys.cpl,,1"},
        {L"ms-settings:sound-sounds", L"control.exe mmsys.cpl,,2"},
        {L"ms-settings:sound-volume-flyout", L"sndvol.exe -f"},
        {L"ms-settings:sound-devices", L"control.exe mmsys.cpl,,0"},
        {L"ms-settings:sound-output", L"control.exe mmsys.cpl,,0"},
        {L"ms-settings:sound-input", L"control.exe mmsys.cpl,,1"},
        {L"ms-settings:apps-volume", L"control.exe mmsys.cpl,,0"},
        {L"ms-settings:sound", L"control.exe mmsys.cpl,,0"},
        
        // Notifications / Taskbar
        {L"ms-settings:notifications", NOTIF_AREA_CLSID},
        {L"ms-settings:taskbar-notifications", NOTIF_AREA_CLSID},
        {L"ms-settings:taskbar-systemtray", NOTIF_AREA_CLSID},
        {L"ms-settings:notifications-systemtray", NOTIF_AREA_CLSID},
        {L"ms-settings:systemtray", NOTIF_AREA_CLSID},
        {L"ms-settings:notificationiconpreferences", NOTIF_AREA_CLSID},
        
        // Input devices
        {L"ms-settings:mousetouchpad", L"main.cpl"},
        {L"ms-settings:devices-touchpad", L"main.cpl"},
        {L"ms-settings:keyboard", L"main.cpl,,1"},
        {L"ms-settings:typing", L"main.cpl,,1"},
        {L"ms-settings:pen", w11 ? L"control.exe" : L"shell:::{80F3F1D5-FECA-45F3-BC32-752C152E456E}"},
        {L"ms-settings:pen-windowsink", w11 ? L"control.exe" : L"shell:::{80F3F1D5-FECA-45F3-BC32-752C152E456E}"},
        {L"ms-settings:pen-windowsinksettings", w11 ? L"control.exe" : L"shell:::{80F3F1D5-FECA-45F3-BC32-752C152E456E}"},
        {L"ms-settings:devices-touch", w11 ? L"control.exe" : L"shell:::{80F3F1D5-FECA-45F3-BC32-752C152E456E}"},
        {L"ms-settings:autoplay", L"shell:::{9C60DE1E-E5FC-40f4-A487-460851A8D915}"},
        
        // Devices / Printers
        {L"ms-settings:printers", L"shell:::{A8A91A66-3A7D-4424-8D24-04E180695C7A}"},
        {L"ms-settings:printers-scanners", L"shell:::{2227A280-3AEA-1069-A2DE-08002B30309D}"},
        {L"ms-settings:bluetooth", L"shell:::{A8A91A66-3A7D-4424-8D24-04E180695C7A}"},
        {L"ms-settings:usb", L"shell:::{A8A91A66-3A7D-4424-8D24-04E180695C7A}"},
        {L"ms-settings:connecteddevices", L"shell:::{A8A91A66-3A7D-4424-8D24-04E180695C7A}"},
        {L"ms-settings:mobile-devices", L"shell:::{A8A91A66-3A7D-4424-8D24-04E180695C7A}"},
        {L"ms-settings:camera", L"shell:::{A8A91A66-3A7D-4424-8D24-04E180695C7A}"},
        {L"ms-settings:privacy-customdevices", L"shell:::{A8A91A66-3A7D-4424-8D24-04E180695C7A}"},
        
        // Network
        {L"ms-settings:network", L"shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}"},
        {L"ms-settings:network-wifi", L"shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}"},
        {L"ms-settings:network-ethernet", L"shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}"},
        {L"ms-settings:network-proxy", L"inetcpl.cpl,,4"},
        {L"ms-settings:network-status", L"shell:::{7007ACC7-3202-11D1-AAD2-00805FC1270E}"},
        {L"ms-settings:network-dialup", L"shell:::{7007ACC7-3202-11D1-AAD2-00805FC1270E}"},
        {L"ms-settings:network-advancedsettings", L"shell:::{7007ACC7-3202-11D1-AAD2-00805FC1270E}"},
        {L"ms-settings:firewall", L"shell:::{4026492F-2F69-46B8-B9BF-5654FC07E423}"},
        {L"ms-settings:network-firewall", L"shell:::{4026492F-2F69-46B8-B9BF-5654FC07E423}"},
        {L"ms-settings:windowsdefender", L"shell:::{4026492F-2F69-46B8-B9BF-5654FC07E423}"},
        
        // Accounts
        {L"ms-settings:yourinfo", L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},
        {L"ms-settings:yourinfo-profile", L"shell:::{59031a47-3f72-44a7-89c5-5595fe6b30ee}"},
        {L"ms-settings:emailandaccounts", L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},
        {L"ms-settings:accounts", L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},
        {L"ms-settings:startupapps", L"msconfig.exe"},
        
        // Default Apps
        {L"ms-settings:defaultapps", w11 ? WIN11_PASSTHROUGH : L"shell:::{17cd9488-1228-4b2f-88ce-4298e93e0966}"},
        
        // Time & Language
        {L"ms-settings:dateandtime", L"timedate.cpl"},
        {L"ms-settings:dateandtime-region", L"timedate.cpl"},
        {L"ms-settings:dateandtime-addclocks", L"timedate.cpl,,1"},
        {L"ms-settings:regionlanguage", L"intl.cpl"},
        {L"ms-settings:regionformatting", L"intl.cpl"},
        {L"ms-settings:language", L"intl.cpl"},
        
        // Ease of Access
        {L"ms-settings:easeofaccess", EASE_OF_ACCESS},
        {L"ms-settings:easeofaccess-narrator", EASE_OF_ACCESS},
        {L"ms-settings:easeofaccess-magnifier", EASE_OF_ACCESS},
        {L"ms-settings:easeofaccess-speech", EASE_OF_ACCESS},
        {L"ms-settings:easeofaccess-colorfilter", EASE_OF_ACCESS},
        {L"ms-settings:easeofaccess-display", EASE_OF_ACCESS},
        {L"ms-settings:easeofaccess-uiaccess", EASE_OF_ACCESS},
        {L"ms-settings:easeofaccess-highcontrast", EASE_OF_ACCESS},
        {L"ms-settings:easeofaccess-closedcaptioning", EASE_OF_ACCESS},
        {L"ms-settings:easeofaccess-audio", EASE_OF_ACCESS},
        {L"ms-settings:easeofaccess-mouse", EASE_OF_ACCESS},
        {L"ms-settings:easeofaccess-keyboard", EASE_OF_ACCESS},
        
        // Recovery / Backup / Troubleshooting
        {L"ms-settings:backup", L"shell:::{B98A2BEA-7D42-4558-8BD1-832F41BAC6FD}"},
        {L"ms-settings:recovery", w11 ? L"control.exe" : L"shell:::{9FE63AFD-59CF-4419-9775-ABCC3849F861}"},
        {L"ms-settings:troubleshoot", w11 ? L"msdt.exe -id DeviceDiagnostic" : L"shell:::{C58C4893-3BE0-4B45-ABB5-A63E4B8C8651}"},
        {L"ms-settings:deviceencryption", L"shell:::{D9EF8727-CAC2-4e60-809E-86F80A666C91}"},
    };
    
    // Additional Windows 11 24H2 specific mappings
    if (g_isWin11) {
        g_mappings[L"ms-settings:power"] = L"powercfg.cpl";
        g_mappings[L"ms-settings:display-hdr"] = L"colorcpl.exe";
        g_mappings[L"ms-settings:taskbar"] = NOTIF_AREA_CLSID;
        g_mappings[L"ms-settings:personalization-taskbar"] = NOTIF_AREA_CLSID;
        g_mappings[L"ms-settings:multitasking"] = L"control.exe";
        g_mappings[L"ms-settings:storage"] = L"control.exe";
        g_mappings[L"ms-settings:storagesense"] = L"control.exe";
        g_mappings[L"ms-settings:backup"] = L"control.exe /name Microsoft.BackupAndRestore";
        g_mappings[L"ms-settings:windowsupdate"] = L"control.exe /name Microsoft.WindowsUpdate";
    }
}

// URI normalization
static std::wstring NormalizeUri(const std::wstring& uri) {
    std::wstring result = ToLower(uri);
    const std::wstring PROTOCOL = L"ms-settings://";
    size_t pos = result.find(PROTOCOL);
    if (pos != std::wstring::npos) {
        result = L"ms-settings:" + result.substr(pos + PROTOCOL.length());
    }
    pos = result.find(L'?');
    if (pos != std::wstring::npos) {
        result = result.substr(0, pos);
    }
    while (!result.empty() && result.back() == L'/') {
        result.pop_back();
    }
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

// Win11 CLSID filter
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
        Wh_Log(L"Win11 loop-guard: replacing loop CLSID '%s' with control.exe", target.c_str());
        return L"control.exe";
    }

    if (g_settings.win11CompatibilityMode && !IsClsidSafeOnWin11(lower)) {
        Wh_Log(L"Win11 compat: replacing unconfirmed CLSID '%s' with control.exe", target.c_str());
        return L"control.exe";
    }

    return target;
}

// Fallback handling
static bool HandleFallback(const std::wstring& uri) {
    switch (g_settings.fallbackMode) {
        case 0:
            Wh_Log(L"Fallback: ignoring unmapped URI: %s", uri.c_str());
            return true;
        case 1: {
            Wh_Log(L"Fallback: opening control.exe for unmapped URI: %s", uri.c_str());
            std::wstring cmd = L"control.exe";
            STARTUPINFOW si = {};
            si.cb = sizeof(si);
            si.dwFlags = STARTF_USESHOWWINDOW;
            si.wShowWindow = SW_SHOWNORMAL;
            PROCESS_INFORMATION pi = {};
            if (CreateProcessW_orig(nullptr, cmd.data(), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
            }
            return true;
        }
        default:
            Wh_Log(L"Fallback: passing through unmapped URI: %s", uri.c_str());
            return false;
    }
}

// LaunchTarget
static void LaunchTarget(const std::wstring& command) {
    Wh_Log(L"Launching: %s", command.c_str());

    if (!LoopGuardAllow(command)) {
        Wh_Log(L"Launch suppressed by loop guard: %s", command.c_str());
        return;
    }

    std::wstring lower = ToLower(command);

    // Handle rundll32.exe specially (needs comma in arguments)
    if (command.find(L"rundll32.exe") != std::wstring::npos) {
        std::wstring mutable_cmd = command;
        STARTUPINFOW si = {};
        si.cb = sizeof(si);
        PROCESS_INFORMATION pi = {};
        if (!CreateProcessW_orig(nullptr, mutable_cmd.data(), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
            Wh_Log(L"rundll32 CreateProcess failed for '%s' (%lu)", command.c_str(), GetLastError());
        } else {
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
        return;
    }

    // Handle "explorer shell:::" directly
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

    // Full command line detection
    bool isFullCmdLine = (lower.find(L"rundll32.exe ") != std::wstring::npos) ||
                         (lower.find(L"explorer.exe ") != std::wstring::npos) ||
                         (lower.find(L"msdt.exe ") != std::wstring::npos) ||
                         (lower.find(L"control.exe /") != std::wstring::npos);

    if (isFullCmdLine) {
        STARTUPINFOW si = {};
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_SHOWNORMAL;
        PROCESS_INFORMATION pi = {};
        std::wstring mutable_cmd = command;
        if (!CreateProcessW_orig(nullptr, mutable_cmd.data(), nullptr, nullptr, FALSE, CREATE_UNICODE_ENVIRONMENT,
                                 (LPVOID)g_childEnvBlock.c_str(), nullptr, &si, &pi)) {
            Wh_Log(L"CreateProcess failed for '%s' (%lu)", command.c_str(), GetLastError());
        } else {
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
        return;
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
    if (!CreateProcessW_orig(nullptr, mutableCmd.data(), nullptr, nullptr, FALSE, CREATE_UNICODE_ENVIRONMENT,
                             (LPVOID)g_childEnvBlock.c_str(), nullptr, &si, &pi)) {
        Wh_Log(L"CreateProcess failed for '%s' (error %lu)", cmdLine.c_str(), GetLastError());
        return;
    }
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

// Personalization detection
static bool IsPersonalizationWindow(HWND hwnd) {
    if (!hwnd) {
        hwnd = GetForegroundWindow();
        if (!hwnd) return false;
    }

    HWND h = hwnd;
    while (h) {
        wchar_t cls[256] = {};
        wchar_t title[512] = {};
        GetClassNameW(h, cls, 256);
        GetWindowTextW(h, title, 512);
        std::wstring c = ToLower(cls);
        std::wstring t = ToLower(title);

        if (c == L"progman" || c == L"workerw" || c == L"shelldll_defview") return false;
        if (c == L"cabinetwclass" && t.find(L"personaliz") != std::wstring::npos) return true;
        if (t.find(L"personaliz") != std::wstring::npos) return true;

        HWND parent = GetParent(h);
        if (!parent || parent == h) break;
        h = parent;
    }
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

// Audio window closing (workaround for modern windows)
static bool g_isClosingAudioWindow = false;

static void CloseModernAudioWindow() {
    if (g_isClosingAudioWindow) {
        Wh_Log(L"CloseModernAudioWindow: already running, skipping");
        return;
    }
    g_isClosingAudioWindow = true;
    
    Wh_Log(L"CloseModernAudioWindow: searching for modern audio windows");
    
    HWND hwnd = FindWindowW(L"ApplicationFrameWindow", nullptr);
    bool found = false;
    
    while (hwnd && !found) {
        if (IsWindowVisible(hwnd)) {
            DWORD processId = 0;
            GetWindowThreadProcessId(hwnd, &processId);
            
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
            if (hProcess) {
                wchar_t processPath[MAX_PATH] = {0};
                DWORD size = MAX_PATH;
                if (QueryFullProcessImageNameW(hProcess, 0, processPath, &size)) {
                    std::wstring processName = ToLower(processPath);
                    
                    if (processName.find(L"systemsettings.exe") != std::wstring::npos ||
                        processName.find(L"settings.exe") != std::wstring::npos ||
                        processName.find(L"windows.immersivecontrolpanel") != std::wstring::npos) {
                        
                        HWND childHwnd = FindWindowExW(hwnd, nullptr, L"Windows.UI.Core.CoreWindow", nullptr);
                        if (childHwnd) {
                            wchar_t className[256] = {0};
                            GetClassNameW(childHwnd, className, 256);
                            std::wstring classNameStr = ToLower(className);
                            
                            if (classNameStr.find(L"audio") != std::wstring::npos ||
                                classNameStr.find(L"sound") != std::wstring::npos) {
                                
                                Wh_Log(L"CloseModernAudioWindow: found audio window (process: %s, class: %s)", 
                                       processPath, className);
                                PostMessageW(hwnd, WM_CLOSE, 0, 0);
                                found = true;
                            }
                        }
                        
                        if (!found) {
                            wchar_t windowTitle[256] = {0};
                            GetWindowTextW(hwnd, windowTitle, 256);
                            std::wstring title = ToLower(windowTitle);
                            
                            if (title.find(L"audio") != std::wstring::npos ||
                                title.find(L"sound") != std::wstring::npos ||
                                title.find(L"suono") != std::wstring::npos) {
                                
                                Wh_Log(L"CloseModernAudioWindow: found audio window by title (process: %s, title: %s)", 
                                       processPath, windowTitle);
                                PostMessageW(hwnd, WM_CLOSE, 0, 0);
                                found = true;
                            }
                        }
                    }
                }
                CloseHandle(hProcess);
            }
        }
        
        if (!found) {
            hwnd = FindWindowExW(nullptr, hwnd, L"ApplicationFrameWindow", nullptr);
        } else {
            break;
        }
    }
    
    if (!found) {
        Wh_Log(L"CloseModernAudioWindow: no modern audio window found");
    }
    
    g_isClosingAudioWindow = false;
}

// Helper function to open sound panel
static bool OpenSoundPanel() {
    Wh_Log(L"Opening sound panel via LaunchTarget");
    LaunchTarget(L"control.exe mmsys.cpl,,0");
    return true;
}

// Core resolve logic
struct ResolveResult {
    std::wstring target;
    bool intercept;
};

static ResolveResult ResolveUri(const std::wstring& uri, HWND hwnd) {
    // Win11: some URIs should not be forced
    if (g_isWin11 && g_win11NoForceRedirect.count(uri) > 0) {
        Wh_Log(L"Win11 whitelist: skipping redirect for %s (pass-through)", uri.c_str());
        return {L"", false};
    }
    
    // Audio URI detection with window closing
    if (uri.find(L"ms-settings:sound") == 0 ||
        uri.find(L"ms-settings:audio") == 0 ||
        uri.find(L"ms-settings:apps-volume") == 0 ||
        uri.find(L"ms-settings:sound-devices") == 0 ||
        uri.find(L"ms-settings:sound-output") == 0 ||
        uri.find(L"ms-settings:sound-input") == 0) {
        Wh_Log(L"Audio URI detected, closing modern windows");
        CloseModernAudioWindow();
    }
    
    // Personalization Background (smart detection)
    if (uri == L"ms-settings:personalization-background") {
        std::wstring t = ApplyWin11Filter(ResolvePersonalizationBackground(hwnd));
        Wh_Log(L"personalization-background -> %s", t.c_str());
        return {t, true};
    }

    // Display settings (use rundll32 for advanced properties)
    if (uri == L"ms-settings:display" ||
        uri == L"ms-settings:display-advanced" ||
        uri == L"ms-settings:display-advanced-graphics" ||
        uri == L"ms-settings:graphics-settings" ||
        uri == L"ms-settings:display-adapter-properties" ||
        uri == L"ms-settings:display-resolution" ||
        uri == L"ms-settings:screenrotation") {
        std::wstring t = L"rundll32.exe display.dll,ShowAdapterSettings 0";
        Wh_Log(L"Display redirect: %s -> %s", uri.c_str(), t.c_str());
        return {t, true};
    }

    // Ease of Access (use control.exe /name to avoid CLSID loops)
    if (uri == L"ms-settings:easeofaccess" ||
        uri.find(L"ms-settings:easeofaccess-") == 0) {
        std::wstring t = EASE_OF_ACCESS;
        Wh_Log(L"EaseOfAccess redirect: %s -> %s", uri.c_str(), t.c_str());
        return {t, true};
    }

    // Personalization CLSID handling (from inside classic CPL)
    if (uri.find(L"shell:::{ed834ed6-4b5a-4bfe-8f11-a626dcb6a921}") == 0) {
        if (uri.find(L"pagewallpaper") != std::wstring::npos) {
            Wh_Log(L"Personalization CLSID: forcing wallpaper for '%s'", uri.c_str());
            return {PERS_WALLPAPER, true};
        }
        if (uri.find(L"pagecolorization") != std::wstring::npos) {
            Wh_Log(L"Personalization CLSID: forcing colors for '%s'", uri.c_str());
            return {PERS_COLORS, true};
        }
        Wh_Log(L"Personalization CLSID: forcing root for '%s'", uri.c_str());
        return {PERS_ROOT, true};
    }

    // Check mappings table
    auto it = g_mappings.find(uri);
    if (it != g_mappings.end()) {
        std::wstring t = ApplyWin11Filter(it->second);
        if (t == WIN11_PASSTHROUGH) {
            Wh_Log(L"Passthrough sentinel for '%s'", uri.c_str());
            bool handled = HandleFallback(uri);
            return {L"", handled};
        }
        Wh_Log(L"Mapped: %s -> %s", uri.c_str(), t.c_str());
        return {t, true};
    }

    // Unmapped ms-settings: URI
    if (uri.find(L"ms-settings:") == 0) {
        bool handled = HandleFallback(uri);
        return {L"", handled};
    }

    // Unmapped shell::: CLSID (only intercept known loop CLSIDs on Win11)
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

// Control system detection helpers
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
    if (!current.empty()) tokens.push_back(current);

    if (tokens.size() != 2) return false;

    std::wstring exe = BaseNameLower(tokens[0]);
    if (exe != L"control.exe" && exe != L"control") return false;

    std::wstring arg = ToLower(tokens[1]);
    return (arg == L"system" || arg == L"microsoft.system");
}

// Hook: ShellExecuteExW
BOOL WINAPI ShellExecuteExW_hook(SHELLEXECUTEINFOW* pei) {
    if (IsChildProcess()) return ShellExecuteExW_orig(pei);

    HookGuard guard;
    if (guard.IsReentrant()) {
        Wh_Log(L"ShellExecuteExW: reentrant call, passing through");
        return ShellExecuteExW_orig(pei);
    }

    if (!g_settings.enableRedirects || !pei) return ShellExecuteExW_orig(pei);

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
    else if (IsShellClsid(pei->lpFile))
        uri = ToLower(pei->lpFile);
    else if (IsShellClsid(pei->lpParameters))
        uri = ToLower(pei->lpParameters);

    // Audio URI handling with debug log
    if (!uri.empty() && (uri.find(L"ms-settings:sound") == 0 ||
        uri.find(L"ms-settings:audio") == 0 ||
        uri.find(L"ms-settings:apps-volume") == 0 ||
        uri.find(L"ms-settings:sound-devices") == 0 ||
        uri.find(L"ms-settings:sound-output") == 0 ||
        uri.find(L"ms-settings:sound-input") == 0)) {
        
        Wh_Log(L"Audio URI detected in ShellExecuteExW: %s", uri.c_str());
        CloseModernAudioWindow();
        
        if (OpenSoundPanel()) {
            if (pei->fMask & SEE_MASK_NOCLOSEPROCESS) pei->hProcess = nullptr;
            return TRUE;
        }
    }

    if (!uri.empty()) {
        auto result = ResolveUri(uri, pei->hwnd);
        if (result.intercept) {
            if (!result.target.empty()) LaunchTarget(result.target);
            if (pei->fMask & SEE_MASK_NOCLOSEPROCESS) pei->hProcess = nullptr;
            return TRUE;
        }
    }
    return ShellExecuteExW_orig(pei);
}

// Hook: ShellExecuteW with DEBUG logging
HINSTANCE WINAPI ShellExecuteW_hook(HWND hwnd, LPCWSTR op, LPCWSTR file, LPCWSTR params, LPCWSTR dir, INT show) {
    if (IsChildProcess()) return ShellExecuteW_orig(hwnd, op, file, params, dir, show);

    HookGuard guard;
    if (guard.IsReentrant()) {
        Wh_Log(L"ShellExecuteW: reentrant call, passing through");
        return ShellExecuteW_orig(hwnd, op, file, params, dir, show);
    }

    if (!g_settings.enableRedirects) return ShellExecuteW_orig(hwnd, op, file, params, dir, show);

    // DEBUG: Log everything that passes through ShellExecuteW
    Wh_Log(L"DEBUG ShellExecuteW: hwnd=%p, op='%s', file='%s', params='%s', dir='%s'", 
           hwnd, 
           op ? op : L"(null)", 
           file ? file : L"(null)", 
           params ? params : L"(null)", 
           dir ? dir : L"(null)");

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
    else if (IsShellClsid(file))
        uri = ToLower(file);
    else if (IsShellClsid(params))
        uri = ToLower(params);

    // DEBUG: Log specifically for audio-related calls
    if (file && (wcsstr(file, L"sound") || wcsstr(file, L"audio") || wcsstr(file, L"mmsys"))) {
        Wh_Log(L"AUDIO DEBUG: file='%s', params='%s', uri='%s'", 
               file, params ? params : L"(null)", uri.c_str());
    }

    // Audio URI handling
    if (!uri.empty() && (uri.find(L"ms-settings:sound") == 0 ||
        uri.find(L"ms-settings:audio") == 0 ||
        uri.find(L"ms-settings:apps-volume") == 0 ||
        uri.find(L"ms-settings:sound-devices") == 0 ||
        uri.find(L"ms-settings:sound-output") == 0 ||
        uri.find(L"ms-settings:sound-input") == 0)) {
        
        Wh_Log(L"Audio URI detected in ShellExecuteW: %s", uri.c_str());
        CloseModernAudioWindow();
        
        if (OpenSoundPanel()) {
            return SHELL_EXECUTE_SUCCESS;
        }
    }

    if (!uri.empty()) {
        auto result = ResolveUri(uri, hwnd);
        if (result.intercept) {
            if (!result.target.empty()) LaunchTarget(result.target);
            return SHELL_EXECUTE_SUCCESS;
        }
    }
    return ShellExecuteW_orig(hwnd, op, file, params, dir, show);
}

// Hook: CreateProcessW
BOOL WINAPI CreateProcessW_hook(LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
                                 LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes,
                                 BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment,
                                 LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo,
                                 LPPROCESS_INFORMATION lpProcessInformation) {
    if (IsChildProcess()) {
        return CreateProcessW_orig(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes,
                                   bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory,
                                   lpStartupInfo, lpProcessInformation);
    }

    HookGuard guard;
    if (guard.IsReentrant()) {
        return CreateProcessW_orig(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes,
                                   bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory,
                                   lpStartupInfo, lpProcessInformation);
    }

    if (!g_settings.enableRedirects || g_settings.uiOnlyRedirects) {
        return CreateProcessW_orig(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes,
                                   bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory,
                                   lpStartupInfo, lpProcessInformation);
    }

    std::wstring cmdLine = lpCommandLine ? std::wstring(lpCommandLine) : L"";

    if (IsControlSystemCommand(cmdLine)) {
        Wh_Log(L"CreateProcessW: intercepted 'control system': %s", lpCommandLine);
        LaunchTarget(g_isWin11 ? L"sysdm.cpl" : SYSTEM_PROPS_CLSID);
        if (lpProcessInformation) ZeroMemory(lpProcessInformation, sizeof(PROCESS_INFORMATION));
        SetLastError(ERROR_SUCCESS);
        return TRUE;
    }

    return CreateProcessW_orig(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes,
                               bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory,
                               lpStartupInfo, lpProcessInformation);
}

// Hook: IShellDispatch2::ShellExecute (for COM-based invocations)
HRESULT WINAPI IShellDispatch2_ShellExecute_hook(
    void* pThis, BSTR File, void* vArgs, void* vDir, void* vOperation, void* vShow)
{
    if (IsChildProcess()) {
        return IShellDispatch2_ShellExecute_orig(pThis, File, vArgs, vDir, vOperation, vShow);
    }

    HookGuard guard;
    if (guard.IsReentrant()) {
        return IShellDispatch2_ShellExecute_orig(pThis, File, vArgs, vDir, vOperation, vShow);
    }

    if (!g_settings.enableRedirects || !File) {
        return IShellDispatch2_ShellExecute_orig(pThis, File, vArgs, vDir, vOperation, vShow);
    }

    std::wstring fileStr(File);
    std::wstring uri;

    if (IsMsSettings(fileStr.c_str()))
        uri = NormalizeUri(fileStr);
    else if (IsShellClsid(fileStr.c_str()))
        uri = ToLower(fileStr);

    if (!uri.empty()) {
        Wh_Log(L"IShellDispatch2::ShellExecute intercepted: %s", uri.c_str());
        auto result = ResolveUri(uri, nullptr);
        if (result.intercept) {
            if (!result.target.empty()) LaunchTarget(result.target);
            return S_OK;
        }
    }

    return IShellDispatch2_ShellExecute_orig(pThis, File, vArgs, vDir, vOperation, vShow);
}

static const GUID IID_IUnknown_Manual = {0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}};
#define IID_IUnknown IID_IUnknown_Manual

// Install IShellDispatch2 COM hook (runs inside explorer.exe)
static void InstallIShellDispatch2Hook() {
    if (!dyn_CoCreateInstance) {
        Wh_Log(L"IShellDispatch2 hook: COM not available");
        return;
    }
    
    IUnknown* pUnk = nullptr;
    HRESULT hr = dyn_CoCreateInstance(&CLSID_Shell_, nullptr, 1, &IID_IUnknown, (void**)&pUnk);
    if (FAILED(hr) || !pUnk) {
        Wh_Log(L"IShellDispatch2 hook: CoCreateInstance failed hr=0x%08lX", hr);
        return;
    }

    void* pDisp = nullptr;
    hr = pUnk->QueryInterface(IID_IShellDispatch2_, &pDisp);
    pUnk->Release();

    if (FAILED(hr) || !pDisp) {
        Wh_Log(L"IShellDispatch2 hook: QI failed hr=0x%08lX", hr);
        return;
    }

    void** vtable = *reinterpret_cast<void***>(pDisp);
    void* pShellExecute = vtable[37];

    ((IUnknown*)pDisp)->Release();

    bool ok = Wh_SetFunctionHook(pShellExecute,
                                  (void*)IShellDispatch2_ShellExecute_hook,
                                  (void**)&IShellDispatch2_ShellExecute_orig);
    Wh_Log(L"IShellDispatch2::ShellExecute hook=%d addr=%p", ok, pShellExecute);
}

// Windhawk entry points
BOOL Wh_ModInit() {
    Wh_Log(L"Redirect Settings to Control Panel v9.9.6");
    
    // Load COM dynamically for IShellDispatch2 hook
    g_hOle32 = LoadLibraryW(L"ole32.dll");
    if (g_hOle32) {
        dyn_CoCreateInstance = (CoCreateInstance_t)GetProcAddress(g_hOle32, "CoCreateInstance");
        if (!dyn_CoCreateInstance) {
            Wh_Log(L"Warning: CoCreateInstance not found");
        }
    }

    DetectWindowsVersion();
    LoadSettings();
    BuildChildEnvironment();
    InitMappings();
    Wh_Log(L"%zu URI mappings loaded", g_mappings.size());

    // Install ShellExecute hooks
    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    if (!hShell32) hShell32 = LoadLibraryW(L"shell32.dll");
    if (!hShell32) {
        Wh_Log(L"ERROR: could not load shell32.dll");
        return FALSE;
    }

    FARPROC pExW = GetProcAddress(hShell32, "ShellExecuteExW");
    FARPROC pW = GetProcAddress(hShell32, "ShellExecuteW");
    if (!pExW || !pW) {
        Wh_Log(L"ERROR: required exports not found in shell32.dll");
        return FALSE;
    }

    bool ok1 = Wh_SetFunctionHook((void*)pExW, (void*)ShellExecuteExW_hook, (void**)&ShellExecuteExW_orig);
    bool ok2 = Wh_SetFunctionHook((void*)pW, (void*)ShellExecuteW_hook, (void**)&ShellExecuteW_orig);
    Wh_Log(L"ShellExecuteExW hook=%d ShellExecuteW hook=%d", ok1, ok2);

    if (!ok1 && !ok2) {
        Wh_Log(L"ERROR: failed to install any hooks");
        return FALSE;
    }

    // Install CreateProcessW hook
    HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
    if (!hKernel32) hKernel32 = LoadLibraryW(L"kernel32.dll");
    if (hKernel32) {
        FARPROC pCPW = GetProcAddress(hKernel32, "CreateProcessW");
        if (pCPW) {
            bool ok3 = Wh_SetFunctionHook((void*)pCPW, (void*)CreateProcessW_hook, (void**)&CreateProcessW_orig);
            Wh_Log(L"CreateProcessW hook=%d", ok3);
        }
    }

    // Install COM hook for IShellDispatch2 (for deeper interception)
    InstallIShellDispatch2Hook();

    Wh_Log(L"Ready — EnableRedirects=%d UIOnly=%d SmartPers=%d Fallback=%d Win11Compat=%d MaxLaunches=%d",
        (int)g_settings.enableRedirects, (int)g_settings.uiOnlyRedirects, (int)g_settings.smartPersonalizationDetect,
        g_settings.fallbackMode, (int)g_settings.win11CompatibilityMode, g_settings.maxLaunchesPerUri);

    return TRUE;
}

void Wh_ModUninit() {
    if (g_hOle32) {
        FreeLibrary(g_hOle32);
        g_hOle32 = nullptr;
    }
    Wh_Log(L"Redirect Settings to Control Panel v9.9.6 unloaded.");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed, reloading");
    LoadSettings();
    InitMappings();
}
