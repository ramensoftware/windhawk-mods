// ==WindhawkMod==
// @id             settings-to-control-panel
// @name           Redirect Settings to Control Panel
// @description    Forces classic Control Panel to open instead of Windows 10/11 Settings app using native components. Primarily designed for Windows 10; Windows 11 support is limited due to Microsoft's shell architecture changes.
// @version        10.0.3
// @author         babamohammed
// @github         https://github.com/babamohammed2022
// @include        explorer.exe
// @compilerOptions -lshell32
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
# Redirect Settings → Control Panel

This mod intercepts modern `ms-settings:` URIs and redirects them to their
corresponding classic Control Panel applets using only native Windows components.

---

## Compatibility

- **Windows 10** – Mostly complete support
- **Windows 11** – Partial support (some redirects may vary by build)

---

## Features

- Redirects numerous `ms-settings:` URIs to classic Control Panel
- Anti-loop protection
- Configurable fallback modes
- **NEW in 10.0.3: Option to redirect Audio & Network system tray icons to classic panels (Tested in Windows 10 21H2)**

---

## Configuration

- **EnableRedirects** – Turn the mod on or off
- **RedirectSystemTray** – Redirects volume and network tray context menus to classic Control Panel applets (mmsys.cpl / ncpa.cpl)
- **UIOnlyRedirects** – Only redirect clicks (safer, may miss some)
- **FallbackMode** – What to do when no classic page exists:
  - `0` = ignore (do nothing)
  - `1` = open Control Panel
  - `2` = open modern Settings (default)
- **Win11CompatibilityMode** – Extra safety for Windows 11
- **MaxLaunchesPerUri** – Prevents infinite loops (default: 3 launches per 5 seconds)

---
## Limitations

- Windows 11 support is limited due to Microsoft's architectural changes and some redirects might change based on versions.

---

## How It Works

The mod hooks:
- `ShellExecuteExW` / `ShellExecuteW`
- `CreateProcessW`
- `IShellDispatch2::ShellExecute`
- `TrackPopupMenuEx` (for the system tray redirect)

When a `ms-settings:` URI is detected, it launches the corresponding classic
Control Panel target instead of the modern Settings app.

---

## Credits

- m417z – Code reviews and feedback
- Anixx – Testing on Windows 11 23H2
- dbilanoski – CLSID documentation
*/
// ==/WindhawkModReadme==
// ==WindhawkModSettings==
/*
- EnableRedirects: true
  $name: Enable Redirects
  $description: "Turns the mod on or off. When off, all Settings calls open normally."
- RedirectSystemTray: false
  $name: Redirect System Tray Audio/Network to Control Panel
  $description: "If true, right-clicking the volume or network icon in the system tray will open the classic Sound (mmsys.cpl) or Network Connections (ncpa.cpl) panels instead of the modern Settings."
- UIOnlyRedirects: false
  $name: Non-Invasive UI Mode
  $description: "Only intercepts ShellExecute* calls. Leaves CreateProcessW alone."
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
#include <shellapi.h>
#include <initguid.h>  

// Dynamic COM handling - no linking required
typedef HRESULT (WINAPI *CoCreateInstance_t)(const GUID* rclsid, IUnknown* pUnkOuter, DWORD dwClsContext, const GUID* riid, void** ppv);
static CoCreateInstance_t dyn_CoCreateInstance = nullptr;
static HMODULE g_hOle32 = nullptr;

// IShellDispatch2 vtable hook
using IShellDispatch2_ShellExecute_t = HRESULT(WINAPI*)(void* pThis, BSTR File, void* vArgs, void* vDir, void* vOperation, void* vShow);
static IShellDispatch2_ShellExecute_t IShellDispatch2_ShellExecute_orig = nullptr;

// TrackPopupMenuEx hook
using TrackPopupMenuEx_t = BOOL(WINAPI*)(HMENU, UINT, int, int, HWND, const TPMPARAMS*);
static TrackPopupMenuEx_t g_origTrackPopupMenuEx = nullptr;

// Constants
static const HINSTANCE SHELL_EXECUTE_SUCCESS = (HINSTANCE)33;
#define PERS_ED_CLSID   L"shell:::{ED834ED6-4B5A-4bfe-8F11-A626DCB6A921}"
#define PERS_ROOT       L"explorer shell:::{ED834ED6-4B5A-4bfe-8F11-A626DCB6A921}"
#define PERS_WALLPAPER  L"explorer shell:::{ED834ED6-4B5A-4bfe-8F11-A626DCB6A921} -Microsoft.Personalization\\pageWallpaper"
#define PERS_COLORS     L"explorer shell:::{ED834ED6-4B5A-4bfe-8F11-A626DCB6A921} -Microsoft.Personalization\\pageColorization"

#define SYSTEM_PROPS_CLSID  L"shell:::{BB06C0E4-D293-4f75-8A90-CB05B6477EEE}"
#define NOTIF_AREA_CLSID    L"shell:::{05d7b0f4-2121-4eff-bf6b-ed3f69b894d9}"
#define WIN11_PASSTHROUGH L"__PASSTHROUGH__"
#define EASE_OF_ACCESS  L"explorer shell:::{D555645E-D4F8-4c29-A827-D93C859C4F2A}"

// Forward declarations
using CreateProcessW_t = BOOL(WINAPI*)(LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, BOOL, DWORD, LPVOID, LPCWSTR, LPSTARTUPINFOW, LPPROCESS_INFORMATION);
static CreateProcessW_t CreateProcessW_orig = nullptr;

using ShellExecuteW_t = HINSTANCE(WINAPI*)(HWND, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, INT);
using ShellExecuteExW_t = BOOL(WINAPI*)(SHELLEXECUTEINFOW*);
static ShellExecuteExW_t ShellExecuteExW_orig = nullptr;
static ShellExecuteW_t ShellExecuteW_orig = nullptr;

// ID dei comandi Audio e Rete per il menu contestuale della tray
static constexpr UINT CMD_OPEN_SOUND_SETTINGS   = 40012;
static constexpr UINT CMD_SOUNDS                = 40007;
static constexpr UINT CMD_OPEN_NETWORK_SETTINGS = 3109;

// Struttura per la mappatura dei comandi del menu contestuale della tray
struct TrayCommandMapping {
    UINT commandId;
    const wchar_t* target;
};

static const TrayCommandMapping g_trayCommandMappings[] = {
    {CMD_OPEN_SOUND_SETTINGS, L"rundll32.exe shell32.dll,Control_RunDLL mmsys.cpl,,0"},
    {CMD_SOUNDS, L"rundll32.exe shell32.dll,Control_RunDLL mmsys.cpl,,0"},
    {CMD_OPEN_NETWORK_SETTINGS, L"shell:::{7007ACC7-3202-11D1-AAD2-00805FC1270E}"}
};

static const wchar_t* FindTrayCommand(UINT cmd) {
    for (size_t i = 0; i < sizeof(g_trayCommandMappings)/sizeof(g_trayCommandMappings[0]); i++) {
        if (g_trayCommandMappings[i].commandId == cmd) {
            return g_trayCommandMappings[i].target;
        }
    }
    return nullptr;
}

// Core resolve logic struct
struct ResolveResult {
    const wchar_t* target;
    bool intercept;
};

// Reentry guards
static thread_local int g_hookDepth = 0;

struct HookGuard {
    HookGuard() { ++g_hookDepth; }
    ~HookGuard() { --g_hookDepth; }
    bool IsReentrant() const { return g_hookDepth > 1; }
};

// Cross-process reentry guard
static wchar_t g_childEnvBlock[32768] = {0};
static void BuildChildEnvironment() {
    LPWCH curEnv = GetEnvironmentStringsW();
    size_t pos = 0;
    if (curEnv) {
        LPWCH p = curEnv;
        while (*p && pos < sizeof(g_childEnvBlock)/sizeof(wchar_t) - 100) {
            size_t len = wcslen(p);
            if (wcsncmp(p, L"WH_STC_NOREDIRECT=", 19) != 0) {
                wcscpy_s(g_childEnvBlock + pos, sizeof(g_childEnvBlock)/sizeof(wchar_t) - pos, p);
                pos += len;
                g_childEnvBlock[pos++] = L'\0';
            }
            p += len + 1;
        }
        FreeEnvironmentStringsW(curEnv);
    }
    wcscpy_s(g_childEnvBlock + pos, sizeof(g_childEnvBlock)/sizeof(wchar_t) - pos, L"WH_STC_NOREDIRECT=1\0");
    g_childEnvBlock[pos + 19] = L'\0';
    g_childEnvBlock[pos + 20] = L'\0';
}
static bool IsChildProcess() {
    return GetEnvironmentVariableW(L"WH_STC_NOREDIRECT", nullptr, 0) > 0;
}

// Settings
struct ModSettings {
    bool enableRedirects = true;
    bool redirectSystemTray = false;
    bool uiOnlyRedirects = false;
    int fallbackMode = 2;
    bool win11CompatibilityMode = false;
    int maxLaunchesPerUri = 3;
};

static ModSettings g_settings;

static void LoadSettings() {
    g_settings.enableRedirects = Wh_GetIntSetting(L"EnableRedirects") != 0;
    g_settings.redirectSystemTray = Wh_GetIntSetting(L"RedirectSystemTray") != 0;
    g_settings.uiOnlyRedirects = Wh_GetIntSetting(L"UIOnlyRedirects") != 0;

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

    Wh_Log(L"EnableRedirects=%d RedirectSystemTray=%d UIOnly=%d SmartPers=always_on Fallback=%d Win11Compat=%d MaxLaunches=%d",
        (int)g_settings.enableRedirects, (int)g_settings.redirectSystemTray, (int)g_settings.uiOnlyRedirects,
        g_settings.fallbackMode,
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

// Bounce-back guard using simple arrays
#define MAX_BOUNCE_RECORDS 100
struct BounceRecord {
    wchar_t uri[256];
    DWORD lastRedirectTick;
};

static BounceRecord g_bounceGuard[MAX_BOUNCE_RECORDS];
static int g_bounceCount = 0;
static CRITICAL_SECTION g_bounceGuardCs;

static constexpr DWORD BOUNCE_WINDOW_MS = 3000;

static void BounceGuardRecord(const wchar_t* uri) {
    EnterCriticalSection(&g_bounceGuardCs);
    
    for (int i = 0; i < g_bounceCount; i++) {
        if (wcscmp(g_bounceGuard[i].uri, uri) == 0) {
            g_bounceGuard[i].lastRedirectTick = GetTickCount();
            LeaveCriticalSection(&g_bounceGuardCs);
            return;
        }
    }
    
    if (g_bounceCount < MAX_BOUNCE_RECORDS) {
        wcscpy_s(g_bounceGuard[g_bounceCount].uri, 256, uri);
        g_bounceGuard[g_bounceCount].lastRedirectTick = GetTickCount();
        g_bounceCount++;
    }
    LeaveCriticalSection(&g_bounceGuardCs);
}

static bool BounceGuardIsBounce(const wchar_t* uri) {
    EnterCriticalSection(&g_bounceGuardCs);
    for (int i = 0; i < g_bounceCount; i++) {
        if (wcscmp(g_bounceGuard[i].uri, uri) == 0) {
            DWORD elapsed = GetTickCount() - g_bounceGuard[i].lastRedirectTick;
            if (elapsed < BOUNCE_WINDOW_MS) {
                Wh_Log(L"BOUNCE-BACK: '%s' returned %lu ms after redirect", uri, elapsed);
                g_bounceGuard[i].lastRedirectTick = 0;
                LeaveCriticalSection(&g_bounceGuardCs);
                return true;
            }
            LeaveCriticalSection(&g_bounceGuardCs);
            return false;
        }
    }
    LeaveCriticalSection(&g_bounceGuardCs);
    return false;
}

// Loop guard using simple arrays
#define MAX_LAUNCH_RECORDS 100
struct LaunchRecord {
    wchar_t target[256];
    int count;
    DWORD firstTick;
};

static LaunchRecord g_loopGuard[MAX_LAUNCH_RECORDS];
static int g_loopCount = 0;
static CRITICAL_SECTION g_loopGuardCs;

static constexpr DWORD LOOP_WINDOW_MS = 5000;

static bool LoopGuardAllow(const wchar_t* target) {
    if (g_settings.maxLaunchesPerUri <= 0) return true;

    EnterCriticalSection(&g_loopGuardCs);
    DWORD now = GetTickCount();
    
    for (int i = 0; i < g_loopCount; i++) {
        if (wcscmp(g_loopGuard[i].target, target) == 0) {
            if (g_loopGuard[i].count == 0 || (now - g_loopGuard[i].firstTick) >= LOOP_WINDOW_MS) {
                g_loopGuard[i].count = 1;
                g_loopGuard[i].firstTick = now;
                LeaveCriticalSection(&g_loopGuardCs);
                return true;
            }
            
            if (g_loopGuard[i].count < g_settings.maxLaunchesPerUri) {
                g_loopGuard[i].count++;
                LeaveCriticalSection(&g_loopGuardCs);
                return true;
            }
            
            Wh_Log(L"LOOP GUARD: suppressing launch of '%s' (%d times in %lu ms)", 
                   target, g_loopGuard[i].count, (now - g_loopGuard[i].firstTick));
            LeaveCriticalSection(&g_loopGuardCs);
            return false;
        }
    }
    
    if (g_loopCount < MAX_LAUNCH_RECORDS) {
        wcscpy_s(g_loopGuard[g_loopCount].target, 256, target);
        g_loopGuard[g_loopCount].count = 1;
        g_loopGuard[g_loopCount].firstTick = now;
        g_loopCount++;
        LeaveCriticalSection(&g_loopGuardCs);
        return true;
    }
    
    LeaveCriticalSection(&g_loopGuardCs);
    return true;
}

// CLSID classification
static const wchar_t* g_win11SafeClsids[] = {
    L"shell:::{025a5937-a6be-4686-a844-36fe4bec8b6d}",
    L"shell:::{05d7b0f4-2121-4eff-bf6b-ed3f69b894d9}",
    L"shell:::{15eae92e-f17a-4431-9f28-805e482dafd4}",
    L"shell:::{20d04fe0-3aea-1069-a2d8-08002b30309d}",
    L"shell:::{2227a280-3aea-1069-a2de-08002b30309d}",
    L"shell:::{26ee0668-a00a-44d7-9371-beb064c98683}",
    L"shell:::{4026492f-2f69-46b8-b9bf-5654fc07e423}",
    L"shell:::{59031a47-3f72-44a7-89c5-5595fe6b30ee}",
    L"shell:::{60632754-c523-4b62-b45c-4172da012619}",
    L"shell:::{6dfd7c5c-2451-11d3-a299-00c04f8ef6af}",
    L"shell:::{7007acc7-3202-11d1-aad2-00805fc1270e}",
    L"shell:::{725be8f7-668e-4c7b-8f90-46bdb0936430}",
    L"shell:::{7a9d77bd-5403-11d2-8785-2e0420524153}",
    L"shell:::{8e908fc9-becc-40f6-915b-f4ca0e70d03d}",
    L"shell:::{9c60de1e-e5fc-40f4-a487-460851a8d915}",
    L"shell:::{a8a91a66-3a7d-4424-8d24-04e180695c7a}",
    L"shell:::{b98a2bea-7d42-4558-8bd1-832f41bac6fd}",
    L"shell:::{bb64f8a7-bee7-4e1a-ab8d-7d8273f7fdb6}",
    L"shell:::{bd84b380-8ca2-1069-ab1d-08000948f534}",
    L"shell:::{c58c4893-3be0-4b45-abb5-a63e4b8c8651}",
    L"shell:::{d17d1d6d-cc3f-4815-8fe3-607e7d5d10b3}",
    L"shell:::{d450a8a1-9568-45c7-9c0e-b4f9fb4537bd}",
    L"shell:::{d555645e-d4f8-4c29-a827-d93c859c4f2a}",
    L"shell:::{d9ef8727-cac2-4e60-809e-86f80a666c91}",
    L"shell:::{ecd0924-4208-451e-8ee0-373c0956de16}",
    L"shell:::{ed7ba470-8e54-465e-825c-99712043e01c}",
    L"shell:::{f02c1a0d-be21-4350-88b0-7367fc96ef3c}",
};

static const wchar_t* g_win11LoopClsids[] = {
    L"shell:::{17cd9488-1228-4b2f-88ce-4298e93e0966}",
    L"shell:::{80f3f1d5-feca-45f3-bc32-752c152e456e}",
    L"shell:::{9fe63afd-59cf-4419-9775-abcc3849f861}",
    L"shell:::{bb06c0e4-d293-4f75-8a90-cb05b6477eee}",
    L"shell:::{ed834ed6-4b5a-4bfe-8f11-a626dcb6a921}",
};

static bool IsInArray(const wchar_t* const* arr, size_t count, const wchar_t* value) {
    for (size_t i = 0; i < count; i++) {
        if (wcscmp(arr[i], value) == 0) return true;
    }
    return false;
}

static bool IsClsidSafeOnWin11(const wchar_t* lowerTarget) {
    return IsInArray(g_win11SafeClsids, sizeof(g_win11SafeClsids)/sizeof(g_win11SafeClsids[0]), lowerTarget);
}

static bool IsClsidLoopOnWin11(const wchar_t* lowerTarget) {
    wchar_t base[256];
    wcscpy_s(base, 256, lowerTarget);
    wchar_t* brace = wcsrchr(base, L'}');
    if (brace && *(brace + 1) != L'\0') {
        *(brace + 1) = L'\0';
    }
    return IsInArray(g_win11LoopClsids, sizeof(g_win11LoopClsids)/sizeof(g_win11LoopClsids[0]), base);
}

// String utilities
static void ToLowerC(wchar_t* s) {
    while (*s) {
        if (*s >= L'A' && *s <= L'Z') *s += 32;
        s++;
    }
}

static wchar_t* ToLowerCopy(const wchar_t* s) {
    static thread_local wchar_t buf[512];
    wcscpy_s(buf, 512, s);
    ToLowerC(buf);
    return buf;
}

// URI mapping structure
struct UriMapping {
    const wchar_t* uri;
    const wchar_t* target;
};

// All mappings as static array
static const UriMapping g_allMappings[] = {
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
    
    // Display
    {L"ms-settings:display-advanced", L"rundll32.exe display.dll,ShowAdapterSettings 0"},
    {L"ms-settings:display-advanced-graphics", L"rundll32.exe display.dll,ShowAdapterSettings 0"},
    {L"ms-settings:display-adapter-properties", L"rundll32.exe display.dll,ShowAdapterSettings 0"},
    {L"ms-settings:display-resolution", L"rundll32.exe display.dll,ShowAdapterSettings 0"},
    {L"ms-settings:screenrotation", L"rundll32.exe display.dll,ShowAdapterSettings 0"},
    
    // System
    {L"ms-settings:about", nullptr},
    {L"ms-settings:system", nullptr},
    {L"ms-settings:sysinfo", nullptr},
    {L"ms-settings:system-about", nullptr},
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
    {L"ms-settings:system-settings", L"shell:::{025A5937-A6BE-4686-A844-36FE4BEC8B6D}\\pageGlobalSettings"},
    
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
    {L"ms-settings:pen", nullptr},
    {L"ms-settings:pen-windowsink", nullptr},
    {L"ms-settings:pen-windowsinksettings", nullptr},
    {L"ms-settings:devices-touch", nullptr},
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
    
    // Network - Usando shell:::{7007ACC7-3202-11D1-AAD2-00805FC1270E} (come nel Codice 2)
    {L"ms-settings:network", L"shell:::{7007ACC7-3202-11D1-AAD2-00805FC1270E}"},
    {L"ms-settings:network-wifi", L"shell:::{7007ACC7-3202-11D1-AAD2-00805FC1270E}"},
    {L"ms-settings:network-ethernet", L"shell:::{7007ACC7-3202-11D1-AAD2-00805FC1270E}"},
    {L"ms-settings:network-vpn", L"shell:::{7007ACC7-3202-11D1-AAD2-00805FC1270E}"},
    {L"ms-settings:network-airplanemode", L"shell:::{7007ACC7-3202-11D1-AAD2-00805FC1270E}"},
    {L"ms-settings:network-mobilehotspot", L"shell:::{7007ACC7-3202-11D1-AAD2-00805FC1270E}"},
    {L"ms-settings:network-cellular", L"shell:::{7007ACC7-3202-11D1-AAD2-00805FC1270E}"},
    {L"ms-settings:datausage", L"shell:::{7007ACC7-3202-11D1-AAD2-00805FC1270E}"},
    {L"ms-settings:network-proxy", L"inetcpl.cpl,,4"},
    {L"ms-settings:network-status", L"shell:::{7007ACC7-3202-11D1-AAD2-00805FC1270E}"},
    {L"ms-settings:network-dialup", L"shell:::{7007ACC7-3202-11D1-AAD2-00805FC1270E}"},
    {L"ms-settings:network-advancedsettings", L"shell:::{7007ACC7-3202-11D1-AAD2-00805FC1270E}"},
    {L"ms-settings:firewall", L"shell:::{4026492F-2F69-46B8-B9BF-5654FC07E423}"},
    {L"ms-settings:network-firewall", L"shell:::{4026492F-2F69-46B8-B9BF-5654FC07E423}"},
    {L"ms-settings:windowsdefender", L"shell:::{4026492F-2F69-46B8-B9BF-5654FC07E423}"},
    {L"ms-settings:network-places", L"shell:::{F02C1A0D-BE21-4350-88B0-7367FC96EF3C}"},
    // Accounts
    {L"ms-settings:yourinfo", L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},
    {L"ms-settings:yourinfo-profile", L"shell:::{59031a47-3f72-44a7-89c5-5595fe6b30ee}"},
    {L"ms-settings:emailandaccounts", L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},
    {L"ms-settings:accounts", L"shell:::{60632754-c523-4b62-b45c-4172da012619}"},
    {L"ms-settings:startupapps", L"msconfig.exe"},
    {L"ms-settings:netplwiz", L"shell:::{7A9D77BD-5403-11d2-8785-2E0420524153}"},
    {L"ms-settings:workplace", L"shell:::{26EE0668-A00A-44D7-9371-BEB064C98683}\\0\\::{ECDB0924-4208-451E-8EE0-373C0956DE16}"},
    
    // Default Apps
    {L"ms-settings:defaultapps", nullptr},
    
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
    {L"ms-settings:recovery", nullptr},
    {L"ms-settings:troubleshoot", nullptr},
    {L"ms-settings:deviceencryption", L"shell:::{D9EF8727-CAC2-4e60-809E-86F80A666C91}"},
    
    // Gaming
    {L"ms-settings:gaming-gamebar", L"joy.cpl"},
    
    // File Explorer Options
    {L"ms-settings:folders", L"shell:::{6DFD7C5C-2451-11d3-A299-00C04F8EF6AF}"},
    
    // Get Programs (modern: Apps & Features)
    {L"ms-settings:appsfeatures-app", L"shell:::{15eae92e-f17a-4431-9f28-805e482dafd4}"},
    
    // Installed Updates
    {L"ms-settings:windowsupdate-history", L"shell:::{d450a8a1-9568-45c7-9c0e-b4f9fb4537bd}"},
    
    // Troubleshoot history
    {L"ms-settings:troubleshoot-history", L"shell:::{C58C4893-3BE0-4B45-ABB5-A63E4B8C8651}\\historyPage"},
    
    // Keyboard
    {L"ms-settings:keyboard-advanced", L"shell:::{26EE0668-A00A-44D7-9371-BEB064C98683}\\0\\::{725BE8F7-668E-4C7B-8F90-46BDB0936430}"},
    {L"ms-settings:keyboard-properties", L"shell:::{725BE8F7-668E-4C7B-8F90-46BDB0936430}"},
    
    // Problem Details / Reports
    {L"ms-settings:privacy-feedback", L"shell:::{BB64F8A7-BEE7-4E1A-AB8D-7D8273F7FDB6}\\pageReportDetails"},
    {L"ms-settings:problem-reporting-settings", L"shell:::{BB64F8A7-BEE7-4E1A-AB8D-7D8273F7FDB6}\\pageSettings"},
    {L"ms-settings:problem-reports", L"shell:::{BB64F8A7-BEE7-4E1A-AB8D-7D8273F7FDB6}\\pageProblems"},
    {L"ms-settings:reliability", L"shell:::{BB64F8A7-BEE7-4E1A-AB8D-7D8273F7FDB6}\\pageReliabilityView"},
    
    // Speech Properties
    {L"ms-settings:speech", L"shell:::{D17D1D6D-CC3F-4815-8FE3-607E7D5D10B3}"},
    
    // Search Troubleshooting
    {L"ms-settings:search-diagnostics", L"shell:::{C58C4893-3BE0-4B45-ABB5-A63E4B8C8651}\\searchPage"},
    
    // Control Panel All Tasks
    {L"ms-settings:controlpanel", L"shell:::{ED7BA470-8E54-465E-825C-99712043E01C}"},
    
    // Sign-in options
    {L"ms-settings:signinoptions", L"netplwiz"},
    {L"ms-settings:accounts-signinoptions", L"netplwiz"},
    {L"ms-settings:accounts-users", L"netplwiz"},
    {L"ms-settings:family-users", L"netplwiz"},
};

static const wchar_t* g_dynamicSystemTarget = nullptr;
static const wchar_t* g_dynamicPenTarget = nullptr;
static const wchar_t* g_dynamicDefaultAppsTarget = nullptr;
static const wchar_t* g_dynamicRecoveryTarget = nullptr;
static const wchar_t* g_dynamicTroubleshootTarget = nullptr;

static void InitDynamicMappings() {
    const bool w11 = g_isWin11;
    g_dynamicSystemTarget = w11 ? L"sysdm.cpl" : SYSTEM_PROPS_CLSID;
    g_dynamicPenTarget = w11 ? L"control.exe" : L"shell:::{80F3F1D5-FECA-45F3-BC32-752C152E456E}";
    g_dynamicDefaultAppsTarget = w11 ? WIN11_PASSTHROUGH : L"shell:::{17cd9488-1228-4b2f-88ce-4298e93e0966}";
    g_dynamicRecoveryTarget = w11 ? L"control.exe" : L"shell:::{9FE63AFD-59CF-4419-9775-ABCC3849F861}";
    g_dynamicTroubleshootTarget = w11 ? L"msdt.exe -id DeviceDiagnostic" : L"shell:::{C58C4893-3BE0-4B45-ABB5-A63E4B8C8651}";
}

static const wchar_t* FindMapping(const wchar_t* uri) {
    for (size_t i = 0; i < sizeof(g_allMappings)/sizeof(g_allMappings[0]); i++) {
        if (wcscmp(g_allMappings[i].uri, uri) == 0) {
            if (wcscmp(uri, L"ms-settings:about") == 0 ||
                wcscmp(uri, L"ms-settings:system") == 0 ||
                wcscmp(uri, L"ms-settings:sysinfo") == 0 ||
                wcscmp(uri, L"ms-settings:system-about") == 0) {
                return g_dynamicSystemTarget;
            }
            if (wcscmp(uri, L"ms-settings:pen") == 0 ||
                wcscmp(uri, L"ms-settings:pen-windowsink") == 0 ||
                wcscmp(uri, L"ms-settings:pen-windowsinksettings") == 0 ||
                wcscmp(uri, L"ms-settings:devices-touch") == 0) {
                return g_dynamicPenTarget;
            }
            if (wcscmp(uri, L"ms-settings:defaultapps") == 0) {
                return g_dynamicDefaultAppsTarget;
            }
            if (wcscmp(uri, L"ms-settings:recovery") == 0) {
                return g_dynamicRecoveryTarget;
            }
            if (wcscmp(uri, L"ms-settings:troubleshoot") == 0) {
                return g_dynamicTroubleshootTarget;
            }
            return g_allMappings[i].target;
        }
    }
    return nullptr;
}

// URI normalization
static void NormalizeUri(const wchar_t* uri, wchar_t* out, size_t outSize) {
    wcscpy_s(out, outSize, uri);
    ToLowerC(out);
    
    const wchar_t PROTOCOL[] = L"ms-settings://";
    size_t protoLen = wcslen(PROTOCOL);
    if (wcsncmp(out, PROTOCOL, protoLen) == 0) {
        wchar_t temp[512];
        wcscpy_s(temp, 512, L"ms-settings:");
        wcscat_s(temp, 512, out + protoLen);
        wcscpy_s(out, outSize, temp);
    }
    
    wchar_t* qmark = wcschr(out, L'?');
    if (qmark) *qmark = L'\0';
    
    size_t len = wcslen(out);
    while (len > 0 && out[len-1] == L'/') {
        out[--len] = L'\0';
    }
}

static bool IsMsSettings(const wchar_t* s) {
    if (!s) return false;
    wchar_t lower[512];
    wcscpy_s(lower, 512, s);
    ToLowerC(lower);
    return wcsstr(lower, L"ms-settings:") != nullptr;
}

static bool IsShellClsid(const wchar_t* s) {
    if (!s) return false;
    wchar_t lower[512];
    wcscpy_s(lower, 512, s);
    ToLowerC(lower);
    return wcsstr(lower, L"shell:::") != nullptr;
}

// Win11 CLSID filter
static const wchar_t* ApplyWin11Filter(const wchar_t* target) {
    if (!g_isWin11) return target;

    wchar_t lower[256];
    wcscpy_s(lower, 256, target);
    ToLowerC(lower);
    
    if (wcsncmp(lower, L"shell:::", 8) != 0 && wcsncmp(lower, L"explorer shell:::", 17) != 0) return target;
    
    // Estrai la parte CLSID
    const wchar_t* clsPart = lower;
    if (wcsncmp(lower, L"explorer ", 9) == 0) clsPart = lower + 9;

    if (IsClsidLoopOnWin11(clsPart)) {
        if (wcsstr(lower, L"ed834ed6")) {
            if (wcsstr(lower, L"pagewallpaper")) {
                Wh_Log(L"Win11 loop-guard: {ED834ED6}\\pageWallpaper -> PERS_WALLPAPER");
                return PERS_WALLPAPER;
            }
            Wh_Log(L"Win11 loop-guard: {ED834ED6} -> PERS_ROOT");
            return PERS_ROOT;
        }
        if (wcsstr(lower, L"bb06c0e4")) {
            Wh_Log(L"Win11 loop-guard: {BB06C0E4} -> sysdm.cpl");
            return L"sysdm.cpl";
        }
        Wh_Log(L"Win11 loop-guard: replacing loop CLSID '%s' with control.exe", target);
        return L"control.exe";
    }

    if (g_settings.win11CompatibilityMode && !IsClsidSafeOnWin11(clsPart)) {
        Wh_Log(L"Win11 compat: replacing unconfirmed CLSID '%s' with control.exe", target);
        return L"control.exe";
    }

    return target;
}

// Fallback handling
static bool HandleFallback(const wchar_t* uri) {
    switch (g_settings.fallbackMode) {
        case 0:
            Wh_Log(L"Fallback: ignoring unmapped URI: %s", uri);
            return true;
        case 1: {
            Wh_Log(L"Fallback: opening control.exe for unmapped URI: %s", uri);
            wchar_t cmd[] = L"control.exe";
            STARTUPINFOW si = {};
            si.cb = sizeof(si);
            si.dwFlags = STARTF_USESHOWWINDOW;
            si.wShowWindow = SW_SHOWNORMAL;
            PROCESS_INFORMATION pi = {};
            if (CreateProcessW_orig(nullptr, cmd, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
            }
            return true;
        }
        default:
            Wh_Log(L"Fallback: passing through unmapped URI: %s", uri);
            return false;
    }
}

static void LaunchTarget(const wchar_t* command) {
    Wh_Log(L"Launching: %s", command);

    if (!LoopGuardAllow(command)) {
        Wh_Log(L"Launch suppressed by loop guard: %s", command);
        return;
    }

    wchar_t lower[512];
    wcscpy_s(lower, 512, command);
    ToLowerC(lower);
    
    if (wcsstr(lower, L"explorer shell:::")) {
        SHELLEXECUTEINFOW sei = {};
        sei.cbSize = sizeof(sei);
        sei.fMask = SEE_MASK_FLAG_NO_UI;
        sei.lpVerb = L"open";
        sei.lpFile = L"explorer.exe";
        sei.lpParameters = command + 9;
        sei.nShow = SW_SHOWNORMAL;
        
        Wh_Log(L"Using ShellExecuteExW: explorer.exe params='%s'", sei.lpParameters);
        ShellExecuteExW_orig(&sei);
        return;
    }
    
    if (wcsncmp(lower, L"rundll32.exe ", 13) == 0) {
        Wh_Log(L"Detected rundll32 command: %s", command);
        
        wchar_t rundll32Path[MAX_PATH];
        if (GetSystemDirectoryW(rundll32Path, MAX_PATH)) {
            wcscat_s(rundll32Path, MAX_PATH, L"\\rundll32.exe");
        } else {
            wcscpy_s(rundll32Path, MAX_PATH, L"rundll32.exe");
        }
        
        SHELLEXECUTEINFOW sei = {};
        sei.cbSize = sizeof(sei);
        sei.fMask = SEE_MASK_FLAG_NO_UI;
        sei.lpVerb = L"open";
        sei.lpFile = rundll32Path;
        sei.lpParameters = command + 13;
        sei.nShow = SW_SHOWNORMAL;
        
        Wh_Log(L"Using ShellExecuteExW: %s params='%s'", rundll32Path, sei.lpParameters);
        ShellExecuteExW_orig(&sei);
        return;
    }
    
    bool isFullCmdLine =
        (wcsstr(lower, L"explorer.exe ") != nullptr) ||
        (wcsstr(lower, L"control.exe /") != nullptr);

    if (isFullCmdLine) {
        STARTUPINFOW si = {};
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_SHOWNORMAL;
        PROCESS_INFORMATION pi = {};
        wchar_t mutable_cmd[512];
        wcscpy_s(mutable_cmd, 512, command);
        if (!CreateProcessW_orig(nullptr, mutable_cmd, nullptr, nullptr,
                                 FALSE, CREATE_UNICODE_ENVIRONMENT,
                                 g_childEnvBlock,
                                 nullptr, &si, &pi)) {
            Wh_Log(L"CreateProcess failed for '%s' (%lu)", command, GetLastError());
        } else {
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
        return;
    }

    if (wcscmp(command, L"devmgmt.msc") == 0 || 
        wcscmp(command, L"compmgmt.msc") == 0 ||
        wcscmp(command, L"slui.exe") == 0 || 
        wcscmp(command, L"OptionalFeatures.exe") == 0) {
        ShellExecuteW_orig(nullptr, L"open", command, nullptr, nullptr, SW_SHOWNORMAL);
        return;
    }

    STARTUPINFOW si = {};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOWNORMAL;
    PROCESS_INFORMATION pi = {};
    wchar_t cmdLine[512] = {0};

    if (wcsstr(command, L".msc")) {
        wcscpy_s(cmdLine, 512, L"mmc.exe \"");
        wcscat_s(cmdLine, 512, command);
        wcscat_s(cmdLine, 512, L"\"");
    } else if (wcsstr(command, L".cpl")) {
        Wh_Log(L"Using ShellExecuteW for .cpl: control.exe %s", command);
        ShellExecuteW_orig(nullptr, L"open", L"control.exe", command, nullptr, SW_SHOWNORMAL);
        return;
    } else if (wcsstr(command, L".exe")) {
        wcscpy_s(cmdLine, 512, command);
    } else if (wcsncmp(command, L"shell:::", 8) == 0) {
        // Forza l'uso di ShellExecuteExW per i CLSID - più affidabile
        // rispetto a CreateProcess
        SHELLEXECUTEINFOW sei = {};
        sei.cbSize = sizeof(sei);
        sei.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_INVOKEIDLIST;
        sei.lpVerb = L"open";
        sei.lpFile = L"explorer.exe";
        sei.lpParameters = command;
        sei.nShow = SW_SHOWNORMAL;
        
        HRESULT hr = ShellExecuteExW_orig(&sei);
        Wh_Log(L"ShellExecuteExW for CLSID '%s' returned 0x%08X", command, hr);
        return;
    } else if (command[0] == L'\0') {
        wcscpy_s(cmdLine, 512, L"control.exe");
    } else {
        wcscpy_s(cmdLine, 512, L"control.exe ");
        wcscat_s(cmdLine, 512, command);
    }

    if (cmdLine[0] != L'\0') {
        if (!CreateProcessW_orig(nullptr, cmdLine, nullptr, nullptr,
                                 FALSE, CREATE_UNICODE_ENVIRONMENT,
                                 g_childEnvBlock,
                                 nullptr, &si, &pi)) {
            Wh_Log(L"CreateProcess failed for '%s' (error %lu)", cmdLine, GetLastError());
            return;
        }
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}

static void ExecuteMappedCommand(const wchar_t* command) {
    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    sei.lpVerb = L"open";
    sei.nShow = SW_SHOWNORMAL;
    
    const wchar_t* spacePos = wcschr(command, L' ');
    if (spacePos) {
        wchar_t program[256];
        size_t progLen = spacePos - command;
        wcsncpy_s(program, 256, command, progLen);
        program[progLen] = L'\0';
        sei.lpFile = program;
        sei.lpParameters = spacePos + 1;
    } else {
        sei.lpFile = command;
    }
    
    ShellExecuteExW_orig(&sei);
}

static BOOL WINAPI TrackPopupMenuEx_Hook(HMENU hMenu, UINT uFlags, int x, int y,
                                         HWND hWnd, const TPMPARAMS* lptpm) {
    if (!g_settings.redirectSystemTray)
        return g_origTrackPopupMenuEx(hMenu, uFlags, x, y, hWnd, lptpm);

    HookGuard guard;
    if (guard.IsReentrant())
        return g_origTrackPopupMenuEx(hMenu, uFlags, x, y, hWnd, lptpm);

    int itemCount = GetMenuItemCount(hMenu);
    Wh_Log(L"TrackPopupMenuEx_Hook called, itemCount=%d", itemCount);  // <-- AGGIUNGI

    if (itemCount <= 0 || itemCount > 6)
        return g_origTrackPopupMenuEx(hMenu, uFlags, x, y, hWnd, lptpm);

    UINT firstItemId = GetMenuItemID(hMenu, 0);
    Wh_Log(L"firstItemId=%u", firstItemId);  // <-- AGGIUNGI

    BOOL callerWantedReturnCmd = (uFlags & TPM_RETURNCMD) != 0;
    UINT modifiedFlags = uFlags | TPM_RETURNCMD;
    BOOL result = g_origTrackPopupMenuEx(hMenu, modifiedFlags, x, y, hWnd, lptpm);

    Wh_Log(L"TrackPopupMenuEx result cmd=%d", result);  // <-- AGGIUNGI

    if (result > 0) {
        UINT cmd = (UINT)result;

        const wchar_t* mappedCmd = FindTrayCommand(cmd);
        Wh_Log(L"FindTrayCommand(%u) = %s", cmd, mappedCmd ? mappedCmd : L"(null)");  // <-- AGGIUNGI

        if (mappedCmd) {
            ExecuteMappedCommand(mappedCmd);
            return 0;
        }

        if (cmd == firstItemId) {
            Wh_Log(L"TrackPopupMenuEx: cmd=%u (prima voce, itemCount=%d) non mappato in g_trayCommandMappings", cmd, itemCount);
        }

        if (!callerWantedReturnCmd) {
            PostMessageW(hWnd, WM_COMMAND, MAKEWPARAM((WORD)cmd, 0), 0);
            return TRUE;
        }
    }
    return result;
}
static bool IsPersonalizationWindow(HWND hwnd) {
    if (!hwnd) return false;

    HWND h = hwnd;
    while (h) {
        wchar_t cls[256] = {};
        wchar_t title[512] = {};
        GetClassNameW(h, cls, 256);
        GetWindowTextW(h, title, 512);

        wchar_t cLower[256], tLower[512];
        wcscpy_s(cLower, 256, cls); ToLowerC(cLower);
        wcscpy_s(tLower, 512, title); ToLowerC(tLower);

        if (wcscmp(cLower, L"progman") == 0 || wcscmp(cLower, L"workerw") == 0 || wcscmp(cLower, L"shelldll_defview") == 0)
            return false;
        if (wcscmp(cLower, L"cabinetwclass") == 0) return true;
        if (wcsstr(tLower, L"personaliz")) return true;

        HWND parent = GetParent(h);
        if (!parent || parent == h) break;
        h = parent;
    }
    return false;
}

static const wchar_t* ResolvePersonalizationBackground(HWND hwnd) {
    return IsPersonalizationWindow(hwnd) ? PERS_WALLPAPER : PERS_ROOT;
}

static ResolveResult ResolveUri(const wchar_t* uri, HWND hwnd) {
    if (wcscmp(uri, L"ms-settings:personalization-background") == 0) {
        if (BounceGuardIsBounce(uri)) {
            ResolveResult r = {L"", true}; return r;
        }
        const wchar_t* t = ApplyWin11Filter(ResolvePersonalizationBackground(hwnd));
        BounceGuardRecord(uri);
        ResolveResult r = {t, true}; return r;
    }

    const wchar_t* mapped = FindMapping(uri);
    if (mapped) {
        if (BounceGuardIsBounce(uri)) {
            bool handled = HandleFallback(uri);
            ResolveResult r = {L"", handled}; return r;
        }
        const wchar_t* t = ApplyWin11Filter(mapped);
        if (wcscmp(t, WIN11_PASSTHROUGH) == 0) {
            bool handled = HandleFallback(uri);
            ResolveResult r = {L"", handled}; return r;
        }
        Wh_Log(L"Mapped: %s -> %s", uri, t);
        BounceGuardRecord(uri);
        ResolveResult r = {t, true}; return r;
    }

    if (wcsncmp(uri, L"ms-settings:", 12) == 0) {
        bool handled = HandleFallback(uri);
        ResolveResult r = {L"", handled}; return r;
    }

    if (wcsncmp(uri, L"shell:::", 8) == 0) {
        if (g_isWin11 && IsClsidLoopOnWin11(uri)) {
            const wchar_t* t = ApplyWin11Filter(uri);
            ResolveResult r = {t, true}; return r;
        }
    }

    ResolveResult r = {L"", false}; return r;
}

static const wchar_t* BaseNameLower(const wchar_t* path) {
    const wchar_t* lastSlash = wcsrchr(path, L'\\');
    return ToLowerCopy(lastSlash ? lastSlash + 1 : path);
}

static bool IsControlSystemParams(const wchar_t* file, const wchar_t* params) {
    if (!file || !params) return false;
    const wchar_t* exe = BaseNameLower(file);
    if (wcscmp(exe, L"control.exe") != 0 && wcscmp(exe, L"control") != 0) return false;
    wchar_t argLower[256];
    wcscpy_s(argLower, 256, params);
    ToLowerC(argLower);
    return (wcscmp(argLower, L"system") == 0 || wcscmp(argLower, L"microsoft.system") == 0);
}

static bool IsControlSystemCommand(const wchar_t* cmdLine) {
    wchar_t tokens[3][256] = {{0}};
    int tokenCount = 0;
    wchar_t current[256] = {0};
    int curPos = 0;
    bool inQuotes = false;

    for (const wchar_t* c = cmdLine; *c; c++) {
        if (*c == L'"') { inQuotes = !inQuotes; }
        else if (*c == L' ' && !inQuotes) {
            if (curPos > 0) {
                current[curPos] = L'\0';
                wcscpy_s(tokens[tokenCount], 256, current);
                tokenCount++;
                if (tokenCount >= 3) break;
                curPos = 0;
            }
        } else {
            if (curPos < 255) current[curPos++] = *c;
        }
    }
    if (curPos > 0 && tokenCount < 3) {
        current[curPos] = L'\0';
        wcscpy_s(tokens[tokenCount], 256, current);
        tokenCount++;
    }
    if (tokenCount != 2) return false;

    const wchar_t* exe = BaseNameLower(tokens[0]);
    if (wcscmp(exe, L"control.exe") != 0 && wcscmp(exe, L"control") != 0) return false;

    wchar_t argLower[256];
    wcscpy_s(argLower, 256, tokens[1]);
    ToLowerC(argLower);
    return (wcscmp(argLower, L"system") == 0 || wcscmp(argLower, L"microsoft.system") == 0);
}

static bool OpenSoundPanel() {
    LaunchTarget(L"control.exe mmsys.cpl,,0");
    return true;
}

// Hook: ShellExecuteExW
BOOL WINAPI ShellExecuteExW_hook(SHELLEXECUTEINFOW* pei) {
    if (IsChildProcess()) return ShellExecuteExW_orig(pei);
    HookGuard guard;
    if (guard.IsReentrant()) return ShellExecuteExW_orig(pei);
    if (!g_settings.enableRedirects || !pei) return ShellExecuteExW_orig(pei);

    if (IsControlSystemParams(pei->lpFile, pei->lpParameters)) {
        LaunchTarget(g_isWin11 ? L"sysdm.cpl" : SYSTEM_PROPS_CLSID);
        if (pei->fMask & SEE_MASK_NOCLOSEPROCESS) pei->hProcess = nullptr;
        return TRUE;
    }
    
    wchar_t checkUri[256] = {0};
    if (IsMsSettings(pei->lpFile)) NormalizeUri(pei->lpFile, checkUri, 256);
    else if (IsMsSettings(pei->lpParameters)) NormalizeUri(pei->lpParameters, checkUri, 256);
    if (checkUri[0] && wcsncmp(checkUri, L"ms-settings:taskbar", 18) == 0)
        return ShellExecuteExW_orig(pei);
    
    wchar_t uri[256] = {0};
    if (IsMsSettings(pei->lpFile)) NormalizeUri(pei->lpFile, uri, 256);
    else if (IsMsSettings(pei->lpParameters)) NormalizeUri(pei->lpParameters, uri, 256);
    else if (IsShellClsid(pei->lpFile)) wcscpy_s(uri, 256, ToLowerCopy(pei->lpFile));
    else if (IsShellClsid(pei->lpParameters)) wcscpy_s(uri, 256, ToLowerCopy(pei->lpParameters));

    if (uri[0] && (wcsncmp(uri, L"ms-settings:sound", 17) == 0 ||
        wcsncmp(uri, L"ms-settings:audio", 17) == 0 ||
        wcsncmp(uri, L"ms-settings:apps-volume", 23) == 0)) {
        if (OpenSoundPanel()) { if (pei->fMask & SEE_MASK_NOCLOSEPROCESS) pei->hProcess = nullptr; return TRUE; }
    }

    if (uri[0] && (wcsncmp(uri, L"ms-settings:display", 19) == 0 ||
        wcsncmp(uri, L"ms-settings:screenrotation", 26) == 0 ||
        wcsncmp(uri, L"ms-settings:graphics-settings", 29) == 0)) {
        LaunchTarget(L"rundll32.exe display.dll,ShowAdapterSettings 0");
        if (pei->fMask & SEE_MASK_NOCLOSEPROCESS) pei->hProcess = nullptr;
        return TRUE;
    }

    if (uri[0]) {
        auto result = ResolveUri(uri, pei->hwnd);
        if (result.intercept) {
            if (result.target[0]) LaunchTarget(result.target);
            if (pei->fMask & SEE_MASK_NOCLOSEPROCESS) pei->hProcess = nullptr;
            return TRUE;
        }
    }
    return ShellExecuteExW_orig(pei);
}

// Hook: ShellExecuteW
HINSTANCE WINAPI ShellExecuteW_hook(HWND hwnd, LPCWSTR op, LPCWSTR file, LPCWSTR params, LPCWSTR dir, INT show) {
    if (IsChildProcess()) return ShellExecuteW_orig(hwnd, op, file, params, dir, show);
    HookGuard guard;
    if (guard.IsReentrant()) return ShellExecuteW_orig(hwnd, op, file, params, dir, show);
    if (!g_settings.enableRedirects) return ShellExecuteW_orig(hwnd, op, file, params, dir, show);

    if (IsControlSystemParams(file, params)) {
        LaunchTarget(g_isWin11 ? L"sysdm.cpl" : SYSTEM_PROPS_CLSID);
        return SHELL_EXECUTE_SUCCESS;
    }
    
    wchar_t checkUri[256] = {0};
    if (IsMsSettings(file)) NormalizeUri(file, checkUri, 256);
    else if (IsMsSettings(params)) NormalizeUri(params, checkUri, 256);
    if (checkUri[0] && wcsncmp(checkUri, L"ms-settings:taskbar", 18) == 0)
        return ShellExecuteW_orig(hwnd, op, file, params, dir, show);
    
    wchar_t uri[256] = {0};
    if (IsMsSettings(file)) NormalizeUri(file, uri, 256);
    else if (IsMsSettings(params)) NormalizeUri(params, uri, 256);
    else if (IsShellClsid(file)) wcscpy_s(uri, 256, ToLowerCopy(file));
    else if (IsShellClsid(params)) wcscpy_s(uri, 256, ToLowerCopy(params));

    if (uri[0] && (wcsncmp(uri, L"ms-settings:sound", 17) == 0 ||
        wcsncmp(uri, L"ms-settings:audio", 17) == 0 ||
        wcsncmp(uri, L"ms-settings:apps-volume", 23) == 0)) {
        if (OpenSoundPanel()) return SHELL_EXECUTE_SUCCESS;
    }

    if (uri[0] && (wcsncmp(uri, L"ms-settings:display", 19) == 0 ||
        wcsncmp(uri, L"ms-settings:screenrotation", 26) == 0 ||
        wcsncmp(uri, L"ms-settings:graphics-settings", 29) == 0)) {
        LaunchTarget(L"rundll32.exe display.dll,ShowAdapterSettings 0");
        return SHELL_EXECUTE_SUCCESS;
    }

    if (uri[0]) {
        auto result = ResolveUri(uri, hwnd);
        if (result.intercept) {
            if (result.target[0]) LaunchTarget(result.target);
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
    if (IsChildProcess()) return CreateProcessW_orig(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
    HookGuard guard;
    if (guard.IsReentrant()) return CreateProcessW_orig(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
    if (!g_settings.enableRedirects || g_settings.uiOnlyRedirects) return CreateProcessW_orig(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);

    if (lpCommandLine && IsControlSystemCommand(lpCommandLine)) {
        LaunchTarget(g_isWin11 ? L"sysdm.cpl" : SYSTEM_PROPS_CLSID);
        if (lpProcessInformation) ZeroMemory(lpProcessInformation, sizeof(PROCESS_INFORMATION));
        SetLastError(ERROR_SUCCESS);
        return TRUE;
    }
    return CreateProcessW_orig(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
}

// Hook: IShellDispatch2::ShellExecute
HRESULT WINAPI IShellDispatch2_ShellExecute_hook(void* pThis, BSTR File, void* vArgs, void* vDir, void* vOperation, void* vShow) {
    if (IsChildProcess()) return IShellDispatch2_ShellExecute_orig(pThis, File, vArgs, vDir, vOperation, vShow);
    HookGuard guard;
    if (guard.IsReentrant()) return IShellDispatch2_ShellExecute_orig(pThis, File, vArgs, vDir, vOperation, vShow);
    if (!g_settings.enableRedirects || !File) return IShellDispatch2_ShellExecute_orig(pThis, File, vArgs, vDir, vOperation, vShow);

    wchar_t uri[256] = {0};
    if (IsMsSettings(File)) NormalizeUri(File, uri, 256);
    else if (IsShellClsid(File)) wcscpy_s(uri, 256, ToLowerCopy(File));

    if (uri[0]) {
        auto result = ResolveUri(uri, nullptr);
        if (result.intercept) {
            if (result.target[0]) LaunchTarget(result.target);
            return S_OK;
        }
    }
    return IShellDispatch2_ShellExecute_orig(pThis, File, vArgs, vDir, vOperation, vShow);
}

// Windhawk entry points
BOOL Wh_ModInit() {
    Wh_Log(L"Redirect Settings to Control Panel v10.0.3");
    
    InitializeCriticalSection(&g_bounceGuardCs);
    InitializeCriticalSection(&g_loopGuardCs);
    
    g_hOle32 = LoadLibraryW(L"ole32.dll");
    if (g_hOle32) {
        dyn_CoCreateInstance = (CoCreateInstance_t)GetProcAddress(g_hOle32, "CoCreateInstance");
    }

    DetectWindowsVersion();
    LoadSettings();
    BuildChildEnvironment();
    InitDynamicMappings();
    Wh_Log(L"URI mappings loaded");

    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    if (!hShell32) hShell32 = LoadLibraryW(L"shell32.dll");
    if (!hShell32) { Wh_Log(L"ERROR: could not load shell32.dll"); return FALSE; }

    FARPROC pExW = GetProcAddress(hShell32, "ShellExecuteExW");
    FARPROC pW = GetProcAddress(hShell32, "ShellExecuteW");
    if (!pExW || !pW) { Wh_Log(L"ERROR: required exports not found"); return FALSE; }

    bool ok1 = Wh_SetFunctionHook((void*)pExW, (void*)ShellExecuteExW_hook, (void**)&ShellExecuteExW_orig);
    bool ok2 = Wh_SetFunctionHook((void*)pW, (void*)ShellExecuteW_hook, (void**)&ShellExecuteW_orig);
    Wh_Log(L"ShellExecuteExW hook=%d ShellExecuteW hook=%d", ok1, ok2);
    if (!ok1 && !ok2) { Wh_Log(L"ERROR: failed to install any hooks"); return FALSE; }

    HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
    if (!hKernel32) hKernel32 = LoadLibraryW(L"kernel32.dll");
    if (hKernel32) {
        FARPROC pCPW = GetProcAddress(hKernel32, "CreateProcessW");
        if (pCPW) Wh_SetFunctionHook((void*)pCPW, (void*)CreateProcessW_hook, (void**)&CreateProcessW_orig);
    }

    if (g_settings.redirectSystemTray) {
        HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
        if (!hUser32) hUser32 = LoadLibraryW(L"user32.dll");
        if (hUser32) {
            void* pTrackPopupMenuEx = (void*)GetProcAddress(hUser32, "TrackPopupMenuEx");
            if (pTrackPopupMenuEx) Wh_SetFunctionHook(pTrackPopupMenuEx, (void*)TrackPopupMenuEx_Hook, (void**)&g_origTrackPopupMenuEx);
        }
    }

    Wh_Log(L"Ready");
    return TRUE;
}

void Wh_ModUninit() {
    DeleteCriticalSection(&g_bounceGuardCs);
    DeleteCriticalSection(&g_loopGuardCs);
    if (g_hOle32) { FreeLibrary(g_hOle32); g_hOle32 = nullptr; }
    Wh_Log(L"Redirect Settings to Control Panel v10.0.3 unloaded.");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed, reloading");
    bool oldRedirectSystemTray = g_settings.redirectSystemTray;
    LoadSettings();
    InitDynamicMappings();

    if (g_settings.redirectSystemTray && !oldRedirectSystemTray && !g_origTrackPopupMenuEx) {
        HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
        if (!hUser32) hUser32 = LoadLibraryW(L"user32.dll");
        if (hUser32) {
            void* pTrackPopupMenuEx = (void*)GetProcAddress(hUser32, "TrackPopupMenuEx");
            if (pTrackPopupMenuEx) {
                bool ok = Wh_SetFunctionHook(pTrackPopupMenuEx, (void*)TrackPopupMenuEx_Hook, (void**)&g_origTrackPopupMenuEx);
                Wh_Log(L"TrackPopupMenuEx hook installed at runtime: %d", (int)ok);
            }
        }
    }
}
